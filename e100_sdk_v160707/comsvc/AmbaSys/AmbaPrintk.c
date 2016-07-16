/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaPrintk.c
 *
 *  @Copyright      :: Copyright (C) 2012 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Print kernel message to the console.
\*-------------------------------------------------------------------------------------------------------------------*/

#include <stdio.h>
#include <string.h>

#include "AmbaDataType.h"
#include "AmbaRTSL_Timer.h"
#include "AmbaRTSL_Cache.h"
#include "AmbaRTSL_CPU.h"
#include "AmbaRTSL_UART.h"

#include "AmbaKAL.h"
#include "AmbaCortexA9.h"
#include "AmbaPrintk.h"
#include "AmbaUART.h"

#include "AmbaDSP.h"

/*-----------------------------------------------------------------------------------------------*\
 * Data structure of print control
\*-----------------------------------------------------------------------------------------------*/
typedef struct _AMBA_PRINT_CTRL_s_ {
    /* RTOS Message buffer control */
    UINT8   *pMsgBuf;           /* Ring buffer to queue the messages ready to print */
    int     MsgBufSize;         /* Ring buffer size */
    UINT32  ReadIdx;            /* The next read buffer position */
    UINT32  WriteIdx;           /* The next write buffer position */
    UINT32  LineBreakPos[2048]; /* Ring buffer to store line message start position of pMsgBuf */
    UINT32  NextLineBreakIdx;   /* Array index to store the next line break position */

    /* DSP message buffer cntrol */
    AMBA_DSP_PRINTF_s   *pDspLog;
    UINT32  DspMsgNum;
    UINT32  DspMsgIdx;
    UINT32  SeqNum;

    /* Printk Configuration */
    AMBA_PRINTK_CONFIG_s        Config;
    AMBA_PRINTK_DSP_CONFIG_s    DspConfig;
} AMBA_PRINT_CTRL_s;

typedef enum _AMBA_SYS_LOG_STATE_e_ {
    AMBA_SYS_LOG_STATE_NO_INIT = 0,
    AMBA_SYS_LOG_STATE_DISABLE,
    AMBA_SYS_LOG_STATE_ENABLE,
    AMBA_SYS_LOG_STATE_NUM,
} AMBA_SYS_LOG_STATE_e;

typedef struct _AMBA_SYS_LOG_s_ {
    UINT8   *Base;
    UINT8   *Ptr;
    UINT8   *Limit;
    UINT32  Size;
    AMBA_SYS_LOG_STATE_e      State;
    AMBA_KAL_MUTEX_t          Mutex;
} AMBA_SYS_LOG_s;

/*-----------------------------------------------------------------------------------------------*\
 * OS resources
\*-----------------------------------------------------------------------------------------------*/
static UINT8 _AmbaPrintTaskStack[4096]__attribute__((section(".bss.noinit")));

static char _AmbaPrint_LineBuf[4096]__attribute__((section(".bss.noinit")));
static AMBA_PRINT_CTRL_s _AmbaPrintCtrl __attribute__((section(".bss.noinit")));
static int _AmbaPrintInitFlag = 0;
static AMBA_PRINT_PUT_CHAR_f AmbaPrint_ConsoleWriteCb;

static AMBA_KAL_TASK_t _AmbaPrintTask;
static AMBA_KAL_EVENT_FLAG_t _AmbaPrintEventFlag;
static AMBA_KAL_MUTEX_t _AmbaPrintMutex;
static AMBA_KAL_MUTEX_t _AmbaPrintRtosMsgWriteMutex;
static AMBA_KAL_MUTEX_t _AmbaPrintRtosMsgReadMutex;

static AMBA_PRINT_MODULE_s _AmbaPrintRtosModule[] = {
    [AMBA_PRINT_MODULE_RTOS_CA9] = {"CA9", 1},
};

static AMBA_PRINT_MODULE_s _AmbaPrintDspModule[] = {
    [AMBA_PRINT_MODULE_DSP_CORE] = {"CODE", 1},
    [AMBA_PRINT_MODULE_DSP_MDXF] = {"MDXF", 1},
    [AMBA_PRINT_MODULE_DSP_MEMD] = {"MEMD", 1},
};

static const char *_AmbaPrintColor[] = {
    [AMBA_PRINT_COLOR_BLACK]    = "\033[0m",
    [AMBA_PRINT_COLOR_RED]      = "\033[0;31m",
    [AMBA_PRINT_COLOR_GREEN]    = "\033[0;32m",
    [AMBA_PRINT_COLOR_YELLOW]   = "\033[0;33m",
    [AMBA_PRINT_COLOR_BLUE]     = "\033[0;34m",
    [AMBA_PRINT_COLOR_MAGENTA]  = "\033[0;35m",
    [AMBA_PRINT_COLOR_CYAN]     = "\033[0;36m",
    [AMBA_PRINT_COLOR_GRAY]     = "\033[0;37m",
};

#ifdef AMBA_PRINT_SHOW_CPU_ID
const char *CpuName[] = {
    "Printed by Cortex A9\r",
    "\r",
};
#endif

/*-----------------------------------------------------------------------------------------------*\
 * Defined in system log mechanism
\*-----------------------------------------------------------------------------------------------*/
static UINT8 _AmbaSysLogBuf[MAX_SYS_LOG_SIZE]__attribute__((section(".bss.noinit")));

static AMBA_SYS_LOG_s _AmbaSysLogCtrl = {
    .Base     = _AmbaSysLogBuf,
    .Ptr      = &_AmbaSysLogBuf[0],
    .Limit    = &_AmbaSysLogBuf[sizeof(_AmbaSysLogBuf) - 1],
    .Size     = 0,
    .State    = AMBA_SYS_LOG_STATE_NO_INIT,
};

static int PRINT_ConsolePutChar(int TxDataSize, char *pTxDataBuf)
{
    if (AmbaPrint_ConsoleWriteCb != NULL)
        return AmbaPrint_ConsoleWriteCb(TxDataSize, pTxDataBuf, AMBA_KAL_WAIT_FOREVER);

    return 0;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: PRINT_GetRtosModule
 *
 *  @Description:: Get RTOS module info.
 *
 *  @Input      :: none
 *
 *  @Output     ::
 *      *pModuleStr: string to describe this module
 *
 *  @Return     ::
 *          int :   OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static int PRINT_GetRtosModule(char **pModuleStr)
{
    AMBA_CPU_ID_REG_s CpuID;
    int ModuleValid;

    AmbaRTSL_CpuGetCpuID(&CpuID);
    if (CpuID.CpuID < AMBA_PRINT_NUM_RTOS_MODULE) {
        ModuleValid = _AmbaPrintRtosModule[CpuID.CpuID].Valid;
        *pModuleStr = _AmbaPrintRtosModule[CpuID.CpuID].Name;
    } else {
        /* Always print if no such module. */
        ModuleValid = 1;
        *pModuleStr = NULL;
    }

    if (ModuleValid)
        return OK;
    else
        return NG;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: PRINT_Flush
 *
 *  @Description:: Flush all messages to console
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static void PRINT_Flush(void)
{
    AMBA_PRINT_CTRL_s *pPrintCtrl = &_AmbaPrintCtrl;
    int Rval = 0;
#ifdef AMBA_PRINT_SHOW_CPU_ID
    AMBA_CPU_ID_REG_s CpuID;
#endif

    if (pPrintCtrl->pMsgBuf == NULL)
        return;

    while (pPrintCtrl->ReadIdx != pPrintCtrl->WriteIdx) {
        if (pPrintCtrl->Config.Ctrl.RtosConsoleEn) {
#ifdef AMBA_PRINT_SHOW_CPU_ID
            AmbaRTSL_CpuGetCpuID(&CpuID);
            PRINT_ConsolePutChar(strlen(CpuName[0]), (CpuID.CpuID == 0) ? CpuName[0] : CpuName[1]);
#endif
            if (pPrintCtrl->WriteIdx > pPrintCtrl->ReadIdx) {
                /* No wrap around */
                Rval = PRINT_ConsolePutChar(pPrintCtrl->WriteIdx - pPrintCtrl->ReadIdx, (char *)&pPrintCtrl->pMsgBuf[pPrintCtrl->ReadIdx]);
            } else {
                /* Wrap around circular buffer */
                Rval = PRINT_ConsolePutChar(pPrintCtrl->MsgBufSize - pPrintCtrl->ReadIdx, (char *)&pPrintCtrl->pMsgBuf[pPrintCtrl->ReadIdx]);
            }

            if (Rval > 0) {
                pPrintCtrl->ReadIdx += Rval;
                if (pPrintCtrl->ReadIdx >= pPrintCtrl->MsgBufSize)
                    pPrintCtrl->ReadIdx = 0;
            }
        } else {
            pPrintCtrl->ReadIdx = pPrintCtrl->WriteIdx;
        }
    }
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: PRINT_InsertMsg
 *
 *  @Description:: Insert string into RTOS message buffer.
 *
 *  @Input      ::
 *      MsgSize:   Number of characters to be inserted to message buffer
 *      pMsg:      String to be insterted
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int :  Number of characters printed to RTOS message buffer
\*-----------------------------------------------------------------------------------------------*/
static int PRINT_InsertMsg(int MsgSize, const char *pMsg)
{
    AMBA_PRINT_CTRL_s *pPrintCtrl = &_AmbaPrintCtrl;
    int Head;
    int Tmp;

    if (pMsg == NULL || MsgSize == 0)
        return 0;

    AmbaKAL_MutexTake(&_AmbaPrintRtosMsgWriteMutex, AMBA_KAL_WAIT_FOREVER);

    /* Update to circular buffer in memory */
    Head = pPrintCtrl->WriteIdx;
    if (Head + MsgSize > pPrintCtrl->MsgBufSize) {
        /* Wrap around circular buffer */
        Tmp = pPrintCtrl->MsgBufSize - Head;
        pPrintCtrl->WriteIdx = MsgSize - Tmp;
        memcpy(&pPrintCtrl->pMsgBuf[Head], &pMsg[0], Tmp);
        memcpy(&pPrintCtrl->pMsgBuf[0], &pMsg[Tmp], MsgSize - Tmp);
    } else {
        /* No wrap */
        pPrintCtrl->WriteIdx = Head + MsgSize;
        memcpy(&pPrintCtrl->pMsgBuf[Head], pMsg, MsgSize);
    }

    AmbaKAL_MutexGive(&_AmbaPrintRtosMsgWriteMutex);

    return MsgSize;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: PRINT_DecodeVarArgList
 *
 *  @Description:: Write formatted data from variable argument list to RTOS message buffer.
 *
 *  @Input      ::
 *      pFmt:      C string that contains a format string that follows the same specifications as format in printf
 *      Args:      A value identifying a variable arguments list initialized with va_start. va_list is a special type defined in <stdarg.h>
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int :  Number of characters printed to RTOS message buffer
\*-----------------------------------------------------------------------------------------------*/
static int PRINT_DecodeVarArgList(const char *pFmt, va_list Args)
{
    int NumChar;

    if (pFmt == NULL)
        return 0;

    /* Print to temporary buffer first */
    NumChar = vsprintf(_AmbaPrint_LineBuf, pFmt, Args);

    return PRINT_InsertMsg(NumChar, _AmbaPrint_LineBuf);
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: PRINT_SetHeader
 *
 *  @Description:: Write formatted header.
 *
 *  @Input      ::
 *      pCurMsgConfig:  Pointer to current message configuration
 *      pModuleStr:     Pointer to current module string
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static void PRINT_SetHeader(AMBA_PRINTK_CONFIG_s *pCurMsgConfig, char *pModuleStr)
{
    char HeaderString[40];
    int HeaderSize = 0;

    /* Current message config has higher priority, than do printing message to buffer. */
    if (pCurMsgConfig->Ctrl.MessageColor >= AMBA_PRINT_NUM_COLOR)
        pCurMsgConfig->Ctrl.MessageColor = AMBA_PRINT_COLOR_BLACK;

    HeaderSize += snprintf(&HeaderString[HeaderSize], sizeof(HeaderString), "%s", _AmbaPrintColor[pCurMsgConfig->Ctrl.MessageColor]);

    if (pCurMsgConfig->Ctrl.SystemTimeEn) {
        /* Msg prefix: System time */
        HeaderSize += snprintf(&HeaderString[HeaderSize], sizeof(HeaderString), "[%08u]", AmbaSysTimer_GetTickCount());
    }

    if (pCurMsgConfig->Ctrl.MoudleStrEn && pModuleStr) {
        /* Msg prefix: Module name */
        HeaderSize += snprintf(&HeaderString[HeaderSize], sizeof(HeaderString), "[%s]", pModuleStr);
    }

    HeaderString[HeaderSize++] = ' ';
    HeaderString[HeaderSize++] = '\0';

    PRINT_InsertMsg(HeaderSize, &HeaderString[0]);
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: PRINT_SetFooter
 *
 *  @Description:: Write formatted footer.
 *
 *  @Input      ::
 *      pCurMsgConfig:  Pointer to current message configuration
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static void PRINT_SetFooter(AMBA_PRINTK_CONFIG_s *pCurMsgConfig)
{
    AMBA_PRINT_CTRL_s *pPrintCtrl = &_AmbaPrintCtrl;
    char FooterString[8] = "\033[0m";   /* Close the color control */

    if (pCurMsgConfig->Ctrl.NewLineEn)
        strcat(FooterString, "\r\n");

    PRINT_InsertMsg(strlen(FooterString), &FooterString[0]);

    if (pPrintCtrl->NextLineBreakIdx == GetArraySize(pPrintCtrl->LineBreakPos) - 1)
        pPrintCtrl->NextLineBreakIdx = 0;
    else
        pPrintCtrl->NextLineBreakIdx++;

    pPrintCtrl->LineBreakPos[pPrintCtrl->NextLineBreakIdx] = pPrintCtrl->WriteIdx;

    AmbaKAL_EventFlagGive(&_AmbaPrintEventFlag, 0x1);
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: PRINT_InsertDspMsg
 *
 *  @Description:: Print a DSP message(log) entry to RTOS message buffer.
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static void PRINT_InsertDspMsg(void)
{
    AMBA_PRINT_CTRL_s *pPrintCtrl = &_AmbaPrintCtrl;
    AMBA_PRINTK_CONFIG_s *pMsgConfig = &pPrintCtrl->Config;
    AMBA_PRINTK_DSP_CONFIG_s *pDspConfig = &pPrintCtrl->DspConfig;
    int MsgSize;

    if (pPrintCtrl->pDspLog == NULL || pPrintCtrl->DspMsgIdx >= pPrintCtrl->DspMsgNum)
        return;

    AmbaRTSL_CacheFlushDataCacheRange(&pPrintCtrl->pDspLog[pPrintCtrl->DspMsgIdx], sizeof(AMBA_DSP_PRINTF_s));

    /* Process the DSP buffer */
    while (pPrintCtrl->pDspLog[pPrintCtrl->DspMsgIdx].SeqNum >= pPrintCtrl->SeqNum) {
        if (pMsgConfig->Ctrl.DspConsoleEn && AmbaKAL_MutexTake(&_AmbaPrintMutex, AMBA_KAL_WAIT_FOREVER) == OK) {
            MsgSize = AmbaPrintk_PrintfDspMsgToBuf(_AmbaPrint_LineBuf,
                                                   &pPrintCtrl->pDspLog[pPrintCtrl->DspMsgIdx],
                                                   pDspConfig->CoreUcodeAddr,
                                                   pDspConfig->MemdUcodeAddr,
                                                   pDspConfig->MdxfUcodeAddr);

            /* Process the EOL characters */
            if (_AmbaPrint_LineBuf[MsgSize] == '\n')
                MsgSize--;
            _AmbaPrint_LineBuf[MsgSize++] = '\r';
            _AmbaPrint_LineBuf[MsgSize++] = '\n';
            _AmbaPrint_LineBuf[MsgSize] = '\0';

            PRINT_InsertMsg(MsgSize, _AmbaPrint_LineBuf);

            if (pPrintCtrl->NextLineBreakIdx == GetArraySize(pPrintCtrl->LineBreakPos) - 1)
                pPrintCtrl->NextLineBreakIdx = 0;
            else
                pPrintCtrl->NextLineBreakIdx++;

            pPrintCtrl->LineBreakPos[pPrintCtrl->NextLineBreakIdx] = pPrintCtrl->WriteIdx;

            AmbaKAL_EventFlagGive(&_AmbaPrintEventFlag, 0x1);

            AmbaKAL_MutexGive(&_AmbaPrintMutex);
        }

        pPrintCtrl->SeqNum = pPrintCtrl->pDspLog[pPrintCtrl->DspMsgIdx].SeqNum + 1;
        pPrintCtrl->DspMsgIdx++;
        pPrintCtrl->DspMsgIdx %= pPrintCtrl->DspMsgNum;

        AmbaRTSL_CacheFlushDataCacheRange(&pPrintCtrl->pDspLog[pPrintCtrl->DspMsgIdx], sizeof(AMBA_DSP_PRINTF_s));
    }

}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: PRINT_Task
 *
 *  @Description:: Print main task.
 *
 *  @Input      ::
 *      EntryArg:   32-bit input argument
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static void PRINT_Task(UINT32 EntryArg)
{
    UINT32 ActualFlags;

    while (1) {
        AmbaKAL_EventFlagTake(&_AmbaPrintEventFlag, 0x1, TX_OR_CLEAR, &ActualFlags, 500);

        if (AmbaKAL_MutexTake(&_AmbaPrintRtosMsgReadMutex, AMBA_KAL_NO_WAIT) == OK) {
            PRINT_Flush();
            AmbaKAL_MutexGive(&_AmbaPrintRtosMsgReadMutex);
        }

        PRINT_InsertDspMsg();
    }
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaPrintk_SetDspConfig
 *
 *  @Description:: Set printk DSP configuration.
 *
 *  @Input      ::
 *      pDspConfig:     Pointer to DSP config data structure
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
int AmbaPrintk_SetDspConfig(AMBA_PRINTK_DSP_CONFIG_s *pDspConfig)
{
    AMBA_PRINT_CTRL_s *pPrintCtrl = &_AmbaPrintCtrl;

    if (pDspConfig == NULL)
        return NG;

    AmbaKAL_MutexTake(&_AmbaPrintMutex, AMBA_KAL_WAIT_FOREVER);
    memcpy((void *) &pPrintCtrl->DspConfig, (void *) pDspConfig, sizeof(AMBA_PRINTK_DSP_CONFIG_s));
    AmbaKAL_MutexGive(&_AmbaPrintMutex);

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaPrintk_GetDspConfig
 *
 *  @Description:: Get printk DSP configuration.
 *
 *  @Input      :: none
 *
 *  @Output     ::
 *      pDspConfig:     Pointer to DSP config data structure
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
int AmbaPrintk_GetDspConfig(AMBA_PRINTK_DSP_CONFIG_s *pDspConfig)
{
    AMBA_PRINT_CTRL_s *pPrintCtrl = &_AmbaPrintCtrl;

    if (pDspConfig == NULL)
        return NG;

    AmbaKAL_MutexTake(&_AmbaPrintMutex, AMBA_KAL_WAIT_FOREVER);
    memcpy((void *) pDspConfig, (void *) &pPrintCtrl->DspConfig, sizeof(AMBA_PRINTK_DSP_CONFIG_s));
    AmbaKAL_MutexGive(&_AmbaPrintMutex);

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaPrintk_GetRtosLastMsg
 *
 *  @Description:: Get last messages (in terms of Lines) in the printk buffer.
 *
 *  @Input      ::
 *      NumLines:   Number of lines of RTOS last messages to get
 *
 *  @Output     ::
 *      pParam:     Pointer to data structure of RTOS last message parameters
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
int AmbaPrintk_GetRtosLastMsg(int NumLines, AMBA_PRINTK_RTOS_LAST_MSG_PARAM_s *pParam)
{
    AMBA_PRINT_CTRL_s *pPrintCtrl = &_AmbaPrintCtrl;
    int LineBreakPosIdx;
    int NumOfLastLines = 0;

    if (!_AmbaPrintInitFlag || pParam == NULL)
        return NG;

    pParam->Head     = (char *)pPrintCtrl->pMsgBuf;
    pParam->Tail     = (char *)pPrintCtrl->pMsgBuf + pPrintCtrl->WriteIdx;
    pParam->AbsStart = (char *)pPrintCtrl->pMsgBuf;
    pParam->AbsEnd   = (char *)pPrintCtrl->pMsgBuf + pPrintCtrl->MsgBufSize;
    LineBreakPosIdx  = pPrintCtrl->NextLineBreakIdx - 1;
    while (NumLines > 0) {
        if (LineBreakPosIdx < 0)
            LineBreakPosIdx += GetArraySize(pPrintCtrl->LineBreakPos);
        pParam->Head = (char *)pPrintCtrl->pMsgBuf + pPrintCtrl->LineBreakPos[LineBreakPosIdx];
        NumLines--;
        NumOfLastLines++;
        LineBreakPosIdx--;
    }

    if (pParam->Head == pParam->Tail)
        return 0;

    return NumOfLastLines;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaPrintk_GetDspLastMsg
 *
 *  @Description:: Get last messages (in AMBA_DSP_PRINTF_s structure) in the printk buffer.
 *
 *  @Input      ::
 *      NumLines:   Number of lines of DSP last messages to get
 *
 *  @Output     ::
 *      pParam:     Pointer to data structure of DSP last message parameters
 *
 *  @Return     ::
 *          int : Number of lines of DSP last messages
\*-----------------------------------------------------------------------------------------------*/
int AmbaPrintk_GetDspLastMsg(int NumLines, AMBA_PRINTK_DSP_LAST_MSG_PARAM_s *pParam)
{
    AMBA_PRINT_CTRL_s *pPrintCtrl = &_AmbaPrintCtrl;
    int DspPfIdx = pPrintCtrl->DspMsgIdx;
    int NumOfLastLines = 0;

    if (!_AmbaPrintInitFlag || pParam == NULL)
        return NG;

    if (pPrintCtrl->pDspLog == NULL || pPrintCtrl->DspMsgNum == 0)
        return NG;

    pParam->AbsStart = (AMBA_DSP_PRINTF_s *) &pPrintCtrl->pDspLog[0];
    pParam->AbsEnd   = (AMBA_DSP_PRINTF_s *) &pPrintCtrl->pDspLog[pPrintCtrl->DspMsgNum - 1];
    pParam->CodeAddr = pPrintCtrl->DspConfig.CoreUcodeAddr;
    pParam->MemdAddr = pPrintCtrl->DspConfig.MemdUcodeAddr;
    pParam->MdxfAddr = pPrintCtrl->DspConfig.MdxfUcodeAddr;
    pParam->Tail     = (AMBA_DSP_PRINTF_s *) &pPrintCtrl->pDspLog[DspPfIdx];
    while (NumLines > 0) {
        if (DspPfIdx < 0)
            DspPfIdx += pPrintCtrl->DspMsgNum;
        pParam->Head = (AMBA_DSP_PRINTF_s *) &pPrintCtrl->pDspLog[DspPfIdx];
        NumLines--;
        NumOfLastLines++;
        DspPfIdx--;
    }

    return NumOfLastLines;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaPrintk_PrintfDspMsgToBuf
 *
 *  @Description:: Get DSP log to the buffer.
 *
 *  @Input      ::
 *      pDspLog:     Pointer to DSP logging format
 *      CodeAddr:   Dsp uCode Code address
 *      MemdAddr:   Dsp uCode Memd address
 *      MdxfAddr:   Dsp uCode Mdxf address
 *
 *  @Output     ::
 *      pBuf:       Output buffer contains Dsp message string
 *
 *  @Return     ::
 *          int : Number of characters printed to buffer including prefix (module string) and postfix (newline).
\*-----------------------------------------------------------------------------------------------*/
int AmbaPrintk_PrintfDspMsgToBuf(char *pBuf, AMBA_DSP_PRINTF_s *pDspLog, UINT32 CodeAddr, UINT32 MemdAddr, UINT32 MdxfAddr)
{
    AMBA_PRINT_CTRL_s *pPrintCtrl = &_AmbaPrintCtrl;
    static const char *UnknownStr = "???";
    char *pFmt;
    int Len = 0;
    UINT32 BaseAddr = 0x0;
    int NumArgs = 0;
    UINT32 Arg[5] = { 0x0, 0x0, 0x0, 0x0, 0x0 };

    if (!_AmbaPrintInitFlag || pDspLog == NULL)
        return NG;

    if (_AmbaPrintDspModule[pDspLog->DspCore].Valid == 0) {
        return Len;
    }

    /* Resolved addresses of arguments */
    pFmt = (char *) pDspLog->FormatAddr;
    if (pFmt == NULL)
        return 0;

    if (pDspLog->DspCore == AMBA_PRINT_MODULE_DSP_CORE) {
        BaseAddr = CodeAddr;
        pFmt = (char *) (BaseAddr + (((UINT32) pFmt) - pPrintCtrl->DspConfig.CoreTextAddr));

    } else if (pDspLog->DspCore == AMBA_PRINT_MODULE_DSP_MEMD) {
        BaseAddr = MemdAddr;
        pFmt = (char *) (BaseAddr + (((UINT32) pFmt) - pPrintCtrl->DspConfig.MemdTextAddr));

    } else {
        BaseAddr = MdxfAddr;
        pFmt = (char *) (BaseAddr + (((UINT32) pFmt) - pPrintCtrl->DspConfig.MdxfTextAddr));
    }

    if (BaseAddr == 0x0) {
        pFmt = (char *) UnknownStr;
        NumArgs = 0;
    } else {
        const char *pStr;
        UINT32 ArgData = 0;

        pStr = pFmt;
        for (NumArgs = 0; NumArgs < 5; NumArgs++) {
            for ( ; *pStr != '\0' && *pStr != '%'; pStr++);
            if (*pStr == '\0')
                break;
            pStr++;

            switch (NumArgs) {
            case 0:
                ArgData = pDspLog->Arg1;
                break;
            case 1:
                ArgData = pDspLog->Arg2;
                break;
            case 2:
                ArgData = pDspLog->Arg3;
                break;
            case 3:
                ArgData = pDspLog->Arg4;
                break;
            case 4:
                ArgData = pDspLog->Arg5;
                break;
            }

            if (*pStr == 's' && ArgData != 0x0) {
                if (pDspLog->DspCore == AMBA_PRINTK_DSP_CORE)
                    Arg[NumArgs] = (UINT32)(BaseAddr + (((UINT32) ArgData) - pPrintCtrl->DspConfig.CoreTextAddr));
                else if (pDspLog->DspCore == AMBA_PRINTK_DSP_MEMD)
                    Arg[NumArgs] = (UINT32)(BaseAddr + (((UINT32) ArgData) - pPrintCtrl->DspConfig.MemdTextAddr));
                else if (pDspLog->DspCore == AMBA_PRINTK_DSP_MDXF)
                    Arg[NumArgs] = (UINT32)(BaseAddr + (((UINT32) ArgData) - pPrintCtrl->DspConfig.MdxfTextAddr));
            } else
                Arg[NumArgs] = ArgData;
        }
    }

    /* Insert prefix */
    if (pDspLog->ThreadId > 0) {
        Len = sprintf(pBuf, "[%s:th%d:%d] ", _AmbaPrintDspModule[pDspLog->DspCore].Name, pDspLog->ThreadId - 1, pDspLog->SeqNum);
    } else {
        Len = sprintf(pBuf, "[%s::%d] ", _AmbaPrintDspModule[pDspLog->DspCore].Name, pDspLog->SeqNum);
    }

    /* Print message */
    switch (NumArgs) {
    case 5:
        Len += sprintf(pBuf + Len, pFmt, Arg[0], Arg[1], Arg[2], Arg[3], Arg[4]);
        break;
    case 4:
        Len += sprintf(pBuf + Len, pFmt, Arg[0], Arg[1], Arg[2], Arg[3]);
        break;
    case 3:
        Len += sprintf(pBuf + Len, pFmt, Arg[0], Arg[1], Arg[2]);
        break;
    case 2:
        Len += sprintf(pBuf + Len, pFmt, Arg[0], Arg[1]);
        break;
    case 1:
        Len += sprintf(pBuf + Len, pFmt, Arg[0]);
        break;
    case 0:
    default:
        Len += sprintf(pBuf + Len, pFmt);
        break;
    }

    return Len;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaPrintk_Flush
 *
 *  @Description:: Flush what's left of the printk daemon to console on caller's context.
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
void AmbaPrintk_Flush(void)
{
    if (!_AmbaPrintInitFlag)
        return;

    AmbaKAL_MutexTake(&_AmbaPrintRtosMsgReadMutex, AMBA_KAL_WAIT_FOREVER);   /* The call in interrupt context cause TX_CALLER_ERROR */
    PRINT_Flush();
    AmbaKAL_MutexGive(&_AmbaPrintRtosMsgReadMutex);
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaPrintk
 *
 *  @Description:: Print a kernel message to the RTOS message buffer then flush to console by Printk daemon.
 *
 *  @Input      ::
 *      NewLineEn:      Auto new line to append at the end of string
 *      SystemTimeEn:   Auto system time tick printing at the start of string
 *      MoudleStrEn:    Auto module name printing at the start of string
 *      MessageLevel:   Current Rtos Message Level
 *      ConsoleLevel:   Current Rtos Console Level
 *      MessageColor:   Message Color
 *      pFmt:           C string that contains a format string that follows the same specifications as format in printf.
 *      ...:            Additional arguments.
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : Number of characters printed to RTOS message buffer including
 *                prefix (color, system time, module string) and postfix (color, newline).
\*-----------------------------------------------------------------------------------------------*/
int AmbaPrintk(int NewLineEn, int SystemTimeEn, int MoudleStrEn, int MessageLevel, int ConsoleLevel, int MessageColor,
               const char *pFmt, ...)
{
    AMBA_PRINT_CTRL_s *pPrintCtrl = &_AmbaPrintCtrl;
    AMBA_PRINTK_CONFIG_s *pMsgConfig = &pPrintCtrl->Config;
    AMBA_PRINTK_CONFIG_s CustomMsgConfig;
    char *pModuleStr;
    va_list Args;

    if (!_AmbaPrintInitFlag)
        return NG;

    if (PRINT_GetRtosModule(&pModuleStr) != OK)
        return OK;

    if (MessageLevel > pMsgConfig->Ctrl.MessageLevel)
        return OK;

    memcpy(&CustomMsgConfig, pMsgConfig, sizeof(AMBA_PRINTK_CONFIG_s));

    if (NewLineEn >= 0)
        CustomMsgConfig.Ctrl.NewLineEn       = NewLineEn;
    if (SystemTimeEn >= 0)
        CustomMsgConfig.Ctrl.SystemTimeEn    = SystemTimeEn;
    if (MoudleStrEn >= 0)
        CustomMsgConfig.Ctrl.MoudleStrEn     = MoudleStrEn;
    if (MessageColor >= 0)
        CustomMsgConfig.Ctrl.MessageColor    = MessageColor;
    if (ConsoleLevel >= 0)
        CustomMsgConfig.Ctrl.ConsoleLevel    = ConsoleLevel;

    /*-----------------------------------------------------------------------*\
     * Take the Mutex
    \*-----------------------------------------------------------------------*/
    if (AmbaKAL_MutexTake(&_AmbaPrintMutex, AMBA_KAL_WAIT_FOREVER) != OK) {
        return NG;  /* Can't take mutex */
    }

    PRINT_SetHeader(&CustomMsgConfig, pModuleStr);

    va_start(Args, pFmt);
    PRINT_DecodeVarArgList(pFmt, Args);
    va_end(Args);

    PRINT_SetFooter(&CustomMsgConfig);

    /*-----------------------------------------------------------------------*\
     * Release the Mutex
    \*-----------------------------------------------------------------------*/
    AmbaKAL_MutexGive(&_AmbaPrintMutex);

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaPrint_Init
 *
 *  @Description:: Initialize print service.
 *
 *  @Input      ::
 *      pPrintConfig:   pointer to the configuration
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int :   OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
int AmbaPrint_Init(AMBA_PRINT_CONFIG_s *pPrintConfig)
{
    AMBA_PRINT_CTRL_s *pPrintCtrl = &_AmbaPrintCtrl;
    AMBA_PRINTK_CONFIG_s *pMsgConfig = &pPrintCtrl->Config;
    int RetVal;

    AmbaPrint_ConsoleWriteCb = pPrintConfig->PutCharCallBack;

    if (AmbaKAL_MutexCreate(&_AmbaPrintMutex) != OK)
        return NG;

    if (AmbaKAL_MutexCreate(&_AmbaPrintRtosMsgWriteMutex) != OK)
        return NG;

    if (AmbaKAL_MutexCreate(&_AmbaPrintRtosMsgReadMutex) != OK)
        return NG;

    if (AmbaKAL_EventFlagCreate(&_AmbaPrintEventFlag) != OK)
        return NG;

    if (pPrintConfig->pPrintBuf == NULL || pPrintConfig->PrintBufSize == 0)
        return NG;

    pPrintCtrl->pMsgBuf = pPrintConfig->pPrintBuf;
    pPrintCtrl->MsgBufSize = pPrintConfig->PrintBufSize;
    memset((void *) pPrintCtrl->pMsgBuf, 0x0, pPrintCtrl->MsgBufSize);
    if (pPrintConfig->pDspDebugLogDataArea != NULL && pPrintConfig->DspDebugLogAreaSize != 0)
        memset((void *) pPrintConfig->pDspDebugLogDataArea, 0x0, pPrintConfig->DspDebugLogAreaSize);

    pPrintCtrl->ReadIdx = 0;
    pPrintCtrl->WriteIdx = 0;

    memset(pPrintCtrl->LineBreakPos, 0, sizeof(pPrintCtrl->LineBreakPos));
    pPrintCtrl->NextLineBreakIdx = 0;

    pMsgConfig->Ctrl.NewLineEn      = AMBA_PRINTK_NEW_LINE_DEFAULT;
    pMsgConfig->Ctrl.SystemTimeEn   = AMBA_PRINTK_SYSTEM_TIME_DEFAULT;
    pMsgConfig->Ctrl.MoudleStrEn    = AMBA_PRINTK_MODULE_STR_DEFAULT;
    pMsgConfig->Ctrl.MessageLevel   = AMBA_PRINTK_MESSAGE_LEVEL_DEFAULT;
    pMsgConfig->Ctrl.ConsoleLevel   = AMBA_PRINTK_CONSOLE_LEVEL_DEFAULT;
    pMsgConfig->Ctrl.MessageColor   = AMBA_PRINTK_COLOR_DEFAULT;
    pMsgConfig->Ctrl.RtosConsoleEn  = AMBA_PRINTK_RTOS_CONSOLE_EN_DEFAULT;
    pMsgConfig->Ctrl.DspConsoleEn   = AMBA_PRINTK_DSP_CONSOLE_EN_DEFAULT;

    pMsgConfig->DefaultLevel.Message = AMBA_PRINTK_MESSAGE_LEVEL_DEFAULT;
    pMsgConfig->DefaultLevel.Console = AMBA_PRINTK_CONSOLE_LEVEL_DEFAULT;

    pPrintCtrl->pDspLog = (AMBA_DSP_PRINTF_s *) pPrintConfig->pDspDebugLogDataArea;
    pPrintCtrl->DspMsgNum = pPrintConfig->DspDebugLogAreaSize / sizeof(AMBA_DSP_PRINTF_s);
    pPrintCtrl->DspMsgIdx = 0;
    pPrintCtrl->SeqNum = 1;

#if 0
    /* get DSP ucode base address */
    AmbaDSP_GetUcodeBaseAddr(&pPrintCtrl->DspConfig.CoreUcodeAddr,
                             &pPrintCtrl->DspConfig.MemdUcodeAddr,
                             &pPrintCtrl->DspConfig.MdxfUcodeAddr,
                             &pPrintCtrl->DspConfig.DefaultDataAddr);

    /* get DSP ucode text address */
    AmbaDSP_GetUcodeTextAddr(&pPrintCtrl->DspConfig.CoreTextAddr,
                             &pPrintCtrl->DspConfig.MemdTextAddr,
                             &pPrintCtrl->DspConfig.MdxfTextAddr);
#endif

    RetVal = AmbaKAL_TaskCreate(&_AmbaPrintTask,                    /* pTask */
                                "AmbaPrintTask",                    /* pTaskName */
                                pPrintConfig->Priority,             /* Priority */
                                PRINT_Task,                         /* void (*EntryFunction)(UINT32) */
                                (UINT32) 0x0,                       /* EntryArg */
                                _AmbaPrintTaskStack,                /* pStackBase */
                                sizeof(_AmbaPrintTaskStack),        /* StackByteSize */
                                AMBA_KAL_DO_NOT_START);             /* AutoStart */
    if (RetVal < 0)
        return RetVal;

    _AmbaPrintInitFlag = 1;

    return AmbaKAL_TaskResume(&_AmbaPrintTask);
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaPrintf
 *
 *  @Description:: Print formatted string.
 *
 *  @Input      ::
 *      PrintTarget:    Target of printing messages
 *      NewLineEn:      Auto new line to append at the end of string
 *      pFmt:           C string that contains a format string that follows the same specifications as format in printf.
 *      ...:            Additional arguments.
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : Number of characters printed
\*-----------------------------------------------------------------------------------------------*/
int AmbaPrintf(int PrintTarget, int NewLineEn, const char *pFmt, ...)
{
    va_list Args;
    int Len;

    va_start(Args, pFmt);
    Len = vsprintf(_AmbaPrint_LineBuf, pFmt, Args);

    if (NewLineEn) {
        _AmbaPrint_LineBuf[Len++] = '\r';
        _AmbaPrint_LineBuf[Len++] = '\n';
        _AmbaPrint_LineBuf[Len]   = '\0';
    }

    if (PrintTarget == AMBA_PRINTF_TARGET_DEBUGGER)
        AmbaDebugger_PutStr(_AmbaPrint_LineBuf);
    else if (PrintTarget == AMBA_PRINTF_TARGET_UART)
        AmbaUART_Write(AMBA_UART_CHAN_CONSOLE, Len, (UINT8 *) _AmbaPrint_LineBuf, AMBA_KAL_WAIT_FOREVER);

    va_end(Args);

    return Len;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaPrint
 *
 *  @Description:: Print a kernel message to the RTOS message buffer then flush to console by Printk daemon.
 *
 *  @Input      ::
 *      pFmt: C string that contains a format string that follows the same specifications as format in printf.
 *      ...:  Additional arguments.
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : Number of characters printed to RTOS message buffer including
 *                prefix (color, system time, module string) and postfix (color, newline).
\*-----------------------------------------------------------------------------------------------*/
int AmbaPrint(const char *pFmt, ...)
{
    #ifndef RELEASE_WITHOUT_PRINT
    AMBA_PRINTK_CONFIG_s *pMsgConfig = &_AmbaPrintCtrl.Config;
    char *pModuleStr;
    va_list Args;

    if (PRINT_GetRtosModule(&pModuleStr) != OK)
        return OK;

    /*-----------------------------------------------------------------------*\
     * Take the Mutex
    \*-----------------------------------------------------------------------*/
    if (AmbaKAL_MutexTake(&_AmbaPrintMutex, 5) != OK) {
        return NG;  /* Can't take mutex */
    }

    PRINT_SetHeader(pMsgConfig, pModuleStr);

    va_start(Args, pFmt);
    PRINT_DecodeVarArgList(pFmt, Args);
    va_end(Args);

    PRINT_SetFooter(pMsgConfig);

    /*-----------------------------------------------------------------------*\
     * Release the Mutex
    \*-----------------------------------------------------------------------*/
    AmbaKAL_MutexGive(&_AmbaPrintMutex);
    #endif
    return OK;
}

//==============================================================================================
// int AmbaPrintAdas(const char *pFmt, ...)
// {
//     AMBA_PRINTK_CONFIG_s *pMsgConfig = &_AmbaPrintCtrl.Config;
//     char *pModuleStr;
//     va_list Args;

//     if (PRINT_GetRtosModule(&pModuleStr) != OK)
//         return OK;

//     /*-----------------------------------------------------------------------*\
//      * Take the Mutex
//     \*-----------------------------------------------------------------------*/
//     if (AmbaKAL_MutexTake(&_AmbaPrintMutex, 5) != OK) {
//         return NG;  /* Can't take mutex */
//     }

//     //PRINT_SetHeader(pMsgConfig, pModuleStr);

//     va_start(Args, pFmt);
//     PRINT_DecodeVarArgList(pFmt, Args);
//     va_end(Args);

//     //PRINT_SetFooter(pMsgConfig);

//     /*-----------------------------------------------------------------------*\
//      * Release the Mutex
//     \*-----------------------------------------------------------------------*/
//     AmbaKAL_MutexGive(&_AmbaPrintMutex);
//     return OK;
// }
/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaSysLog_Init
 *
 *  @Description:: System log mechanism initializations
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
int AmbaSysLog_Init(void)
{
    if (_AmbaSysLogCtrl.State != AMBA_SYS_LOG_STATE_NO_INIT)
        return OK;  /* Does not need to initial again */

    if (AmbaKAL_MutexCreate(&_AmbaSysLogCtrl.Mutex) != OK)
        return NG;  /* something wrong */

    if (_AmbaSysLogCtrl.Base == NULL)
        return NG;  /* Should not happen */

    memset(_AmbaSysLogCtrl.Base, 0, MAX_SYS_LOG_SIZE);

    /* Enable system log default */
    _AmbaSysLogCtrl.State = AMBA_SYS_LOG_STATE_ENABLE;

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaSysLog_Enable
 *
 *  @Description:: Enable system log mechanism
 *
 *  @Input      ::
 *      Enable : Enable/Disable system log
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
int AmbaSysLog_Enable(UINT8 Enable)
{
    if (_AmbaSysLogCtrl.State == AMBA_SYS_LOG_STATE_NO_INIT)
        return NG;

    /*-----------------------------------------------------------------------*\
     * Take the Mutex
    \*-----------------------------------------------------------------------*/
    if (AmbaKAL_MutexTake(&_AmbaSysLogCtrl.Mutex, AMBA_KAL_WAIT_FOREVER) != OK)
        return NG;     /* should never happen */

    if (Enable)
        _AmbaSysLogCtrl.State = AMBA_SYS_LOG_STATE_ENABLE;
    else
        _AmbaSysLogCtrl.State = AMBA_SYS_LOG_STATE_DISABLE;

    /*-----------------------------------------------------------------------*\
     * Release the Mutex
    \*-----------------------------------------------------------------------*/
    AmbaKAL_MutexGive(&_AmbaSysLogCtrl.Mutex);

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaSysLog_Flush
 *
 *  @Description:: Flush system log buffer
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
int AmbaSysLog_Flush(void)
{
    if (_AmbaSysLogCtrl.State == AMBA_SYS_LOG_STATE_NO_INIT)
        return NG;

    /*-----------------------------------------------------------------------*\
     * Take the Mutex
    \*-----------------------------------------------------------------------*/
    if (AmbaKAL_MutexTake(&_AmbaSysLogCtrl.Mutex, AMBA_KAL_WAIT_FOREVER) != OK)
        return NG;     /* should never happen */

    _AmbaSysLogCtrl.Ptr = _AmbaSysLogCtrl.Base;
    _AmbaSysLogCtrl.Size = 0;

    /*-----------------------------------------------------------------------*\
     * Release the Mutex
    \*-----------------------------------------------------------------------*/
    AmbaKAL_MutexGive(&_AmbaSysLogCtrl.Mutex);

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaSysLog_Write
 *
 *  @Description:: Write the message to local buffer
 *
 *  @Input      ::
 *      Mode      : Show system time | new line
 *      toConsole : print the debug message to console too
 *      pFmt      :
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
int AmbaSysLog_Write(UINT8 Type, UINT8 ToConsole, const char *pFmt, ...)
{
    va_list Args;
    extern int vsprintf(char *, const char *, va_list);

    char StringWBuffer[128];
    char StringWOBuffer[128];
    int StrLength = 0;
    char* pString = StringWOBuffer;

    if (_AmbaSysLogCtrl.State == AMBA_SYS_LOG_STATE_NO_INIT)
        return NG;

    if (_AmbaSysLogCtrl.State != AMBA_SYS_LOG_STATE_ENABLE)
        return NG;

    va_start(Args, pFmt);
    vsprintf(StringWOBuffer, pFmt, Args);
    va_end(Args);

    StrLength = strlen(StringWOBuffer);

    if (StrLength) {
        if (ToConsole)
            AmbaPrint("%s", StringWOBuffer);

        if (_AmbaSysLogCtrl.State != AMBA_SYS_LOG_STATE_ENABLE)
            return NG;

        if (Type & AMBA_SYS_LOG_SYS_TIME) {
            sprintf(StringWBuffer, "[%u] %s",
                    AmbaRTSL_TimerGetTickCount(),
                    StringWOBuffer);
            StrLength = strlen(StringWBuffer);
            pString = StringWBuffer;
        } else {
            pString = StringWOBuffer;
        }

#if 0
        /*-----------------------------------------------------------------------*\
         * Take the Mutex
        \*-----------------------------------------------------------------------*/
        if (AmbaKAL_MutexTake(&_AmbaSysLogCtrl.Mutex, AMBA_KAL_WAIT_FOREVER) != OK)
            return NG;     /* should never happen */
#endif

        if (_AmbaSysLogCtrl.Ptr + StrLength - 1 >= _AmbaSysLogCtrl.Limit) {
            UINT32 CopySz = _AmbaSysLogCtrl.Limit - _AmbaSysLogCtrl.Ptr + 1;
            UINT32 Left;
            memmove(_AmbaSysLogCtrl.Ptr, pString, CopySz);
            Left = StrLength - CopySz;

            if (Left)
                memmove(_AmbaSysLogCtrl.Base, (pString + CopySz), Left);
            _AmbaSysLogCtrl.Ptr = _AmbaSysLogCtrl.Base + Left;
        } else {
            memmove(_AmbaSysLogCtrl.Ptr, pString, StrLength);
            _AmbaSysLogCtrl.Ptr += StrLength;
        }

        _AmbaSysLogCtrl.Size += StrLength;

        if (Type & AMBA_SYS_LOG_INSERT_LINE) {
            *_AmbaSysLogCtrl.Ptr = 0xA;
            if (_AmbaSysLogCtrl.Ptr == _AmbaSysLogCtrl.Limit)
                _AmbaSysLogCtrl.Ptr = _AmbaSysLogCtrl.Base;
            else
                _AmbaSysLogCtrl.Ptr++;
            _AmbaSysLogCtrl.Size ++;
        }

        if (_AmbaSysLogCtrl.Size > MAX_SYS_LOG_SIZE)
            _AmbaSysLogCtrl.Size = MAX_SYS_LOG_SIZE;

#if 0
        /*-----------------------------------------------------------------------*\
         * Release the Mutex
        \*-----------------------------------------------------------------------*/
        AmbaKAL_MutexGive(&_AmbaSysLogCtrl.Mutex);
#endif
    }

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaSysLog_GetInfo
 *
 *  @Description:: Get the system log information
 *
 *  @Input      ::
 *          pSysLogInfo: The pointer of the system log information
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
int AmbaSysLog_GetInfo(ABMA_SYS_LOG_BUF_INFO_s * pSysLogInfo)
{
    if (_AmbaSysLogCtrl.State == AMBA_SYS_LOG_STATE_NO_INIT)
        return NG;

    if (pSysLogInfo == NULL)
        return NG;

    /*-----------------------------------------------------------------------*\
     * Take the Mutex
    \*-----------------------------------------------------------------------*/
    if (AmbaKAL_MutexTake(&_AmbaSysLogCtrl.Mutex, AMBA_KAL_WAIT_FOREVER) != OK)
        return NG;     /* should never happen */

    memset(pSysLogInfo, 0, sizeof(ABMA_SYS_LOG_BUF_INFO_s));
    pSysLogInfo->Base  = _AmbaSysLogCtrl.Base;
    pSysLogInfo->Limit = _AmbaSysLogCtrl.Limit;
    pSysLogInfo->Ptr   = _AmbaSysLogCtrl.Ptr;
    pSysLogInfo->Size  = _AmbaSysLogCtrl.Size;
    if (_AmbaSysLogCtrl.State == AMBA_SYS_LOG_STATE_ENABLE)
        pSysLogInfo->State = 1;
    else
        pSysLogInfo->State = 0;

    /*-----------------------------------------------------------------------*\
     * Release the Mutex
    \*-----------------------------------------------------------------------*/
    AmbaKAL_MutexGive(&_AmbaSysLogCtrl.Mutex);

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaPrint_SetConfig
 *
 *  @Description:: Enable/Disable RTOS and DSP runtime messages
 *
 *  @Input      ::
 *      RtosMsgEnable:  Enable/Disable RTOS messages
 *      DspMsgEnable:   Enable/Disable DSP messages
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
int AmbaPrint_SetConfig(int RtosMsgEnable, int DspMsgEnable)
{
    AMBA_PRINTK_CONFIG_s *pMsgConfig = &_AmbaPrintCtrl.Config;

    if (RtosMsgEnable >= 0)
        pMsgConfig->Ctrl.RtosConsoleEn = RtosMsgEnable;

    if (DspMsgEnable >= 0)
        pMsgConfig->Ctrl.DspConsoleEn = DspMsgEnable;

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaPrint_GetConfig
 *
 *  @Description:: RTOS/DSP runtime message control
 *
 *  @Input      ::
 *      pRtosMsgEnable: RTOS runtime message enable
 *      pDspMsgEnable:  DSP runtime message enable
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
int AmbaPrint_GetConfig(int *pRtosMsgEnable, int *pDspMsgEnable)
{
    AMBA_PRINTK_CONFIG_s *pMsgConfig = &_AmbaPrintCtrl.Config;

    if (pRtosMsgEnable != NULL)
        *pRtosMsgEnable = pMsgConfig->Ctrl.RtosConsoleEn;

    if (pDspMsgEnable != NULL)
        *pDspMsgEnable = pMsgConfig->Ctrl.DspConsoleEn;

    return OK;
}
