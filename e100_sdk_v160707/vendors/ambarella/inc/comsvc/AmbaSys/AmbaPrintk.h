/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaPrintk.h
 *
 *  @Copyright      :: Copyright (C) 2012 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Definitions & Constants used for Ambarella System
\*-------------------------------------------------------------------------------------------------------------------*/

#ifndef _AMBA_PRINTK_H_
#define _AMBA_PRINTK_H_

#include <stdarg.h>
#include <string.h>
#include <intrinsics.h>

#include "AmbaDataType.h"
#include "AmbaSysTimer.h"
#include "AmbaPrint.h"

/*-----------------------------------------------------------------------------------------------*\
 * Defined for AmbaPrintk DSP message buffer
\*-----------------------------------------------------------------------------------------------*/
#define AMBA_PRINTK_DSP_MSG_START               (AMBA_CORTEX_A9_DRAM_BASE_ADDR + 0x80000)
#define AMBA_PRINTK_DSP_MSG_SIZE                0x00020000

/*-----------------------------------------------------------------------------------------------*\
 * Data structure of DSP core logging format in the DRAM area.
\*-----------------------------------------------------------------------------------------------*/
typedef struct _AMBA_DSP_PRINTF_s_ {
    UINT32 SeqNum;      /* Sequence number */
    UINT8  DspCore;
    UINT8  ThreadId;
    UINT16 Reserved;
    UINT32 FormatAddr;  /* Address (offset) to find '%s' arg */
    UINT32 Arg1;        /* 1st var. arg */
    UINT32 Arg2;        /* 2nd var. arg */
    UINT32 Arg3;        /* 3rd var. arg */
    UINT32 Arg4;        /* 4th var. arg */
    UINT32 Arg5;        /* 5th var. arg */
} AMBA_DSP_PRINTF_s;

/*-----------------------------------------------------------------------------------------------*\
 * Defined for DspCore member in AMBA_DSP_PRINTF_s
\*-----------------------------------------------------------------------------------------------*/
#define AMBA_PRINTK_DSP_CORE    0
#define AMBA_PRINTK_DSP_MDXF    1
#define AMBA_PRINTK_DSP_MEMD    2

/*-----------------------------------------------------------------------------------------------*\
 * Data structure of RTOS to get last messages
\*-----------------------------------------------------------------------------------------------*/
typedef struct _AMBA_PRINTK_RTOS_LAST_MSG_PARAM_s_ {
    char *Head;
    char *Tail;
    char *AbsStart;
    char *AbsEnd;
} AMBA_PRINTK_RTOS_LAST_MSG_PARAM_s;

/*-----------------------------------------------------------------------------------------------*\
 * Data structure of DSP to get last messages
\*-----------------------------------------------------------------------------------------------*/
typedef struct _AMBA_PRINTK_DSP_LAST_MSG_PARAM_s_ {
    AMBA_DSP_PRINTF_s *Head;
    AMBA_DSP_PRINTF_s *Tail;
    AMBA_DSP_PRINTF_s *AbsStart;
    AMBA_DSP_PRINTF_s *AbsEnd;
    UINT32 CodeAddr;
    UINT32 MemdAddr;
    UINT32 MdxfAddr;
} AMBA_PRINTK_DSP_LAST_MSG_PARAM_s;

/*-----------------------------------------------------------------------------------------------*\
 * Data structure of RTOS printk configuration
\*-----------------------------------------------------------------------------------------------*/
typedef struct _AMBA_PRINTK_CONFIG_s_ {

    struct _AMBA_PRINTK_DEFAULT_LEVEL_s_ {
        UINT32  Console;                /* Messages without an explicit priority will be printed with priority ConsoleLevel. */
        UINT32  Message;                /* Messages without an explicit priority will be printed with priority MessageLevel. */
    } DefaultLevel;

    struct _AMBA_PRINTK_CTRL_s_ {
        UINT32  NewLineEn:          1;  /* Enable outputs module name to console for RTOS messages */
        UINT32  SystemTimeEn:       1;  /* Enable outputs system time to console for RTOS messages */
        UINT32  MoudleStrEn:        1;  /* Enable outputs module name to console for RTOS messages */
        UINT32  MessageLevel:       3;  /* Rtos Messages with a higher priority than MessageLevel will be printed to the Message buffer */
        UINT32  ConsoleLevel:       3;  /* Rtos Messages with a higher priority than ConsoleLevel will be printed to the console */
        UINT32  MessageColor:       3;  /* Rtos default color */
        UINT32  RtosConsoleEn:      1;  /* Enable output to console for RTOS messages */
        UINT32  DspConsoleEn:       1;  /* Enable output to console for DSP messages */
        UINT32  Rsv:                21;
    } Ctrl;
} AMBA_PRINTK_CONFIG_s;

/*-----------------------------------------------------------------------------------------------*\
 * Defined for AmbaPrintColor.
 * Defined for MessageColor member in AMBA_PRINTK_CONFIG_s.
\*-----------------------------------------------------------------------------------------------*/
typedef enum _AMBA_PRINT_COLOR_e_ {
    AMBA_PRINT_COLOR_BLACK = 0,
    AMBA_PRINT_COLOR_RED,
    AMBA_PRINT_COLOR_GREEN,
    AMBA_PRINT_COLOR_YELLOW,
    AMBA_PRINT_COLOR_BLUE,
    AMBA_PRINT_COLOR_MAGENTA,
    AMBA_PRINT_COLOR_CYAN,
    AMBA_PRINT_COLOR_GRAY,

    AMBA_PRINT_NUM_COLOR
} AMBA_PRINT_COLOR_e;

#define BLACK           AMBA_PRINT_COLOR_BLACK
#define RED             AMBA_PRINT_COLOR_RED
#define GREEN           AMBA_PRINT_COLOR_GREEN
#define YELLOW          AMBA_PRINT_COLOR_YELLOW
#define BLUE            AMBA_PRINT_COLOR_BLUE
#define MAGENTA         AMBA_PRINT_COLOR_MAGENTA
#define CYAN            AMBA_PRINT_COLOR_CYAN
#define GRAY            AMBA_PRINT_COLOR_GRAY

/*-----------------------------------------------------------------------------------------------*\
 * Defined for AmbaPrintk Level.
 * Defined for MessageLevel member in AMBA_PRINTK_CONFIG_s.
\*-----------------------------------------------------------------------------------------------*/
#define AMBA_PRINTK_LV_ERROR        0
#define AMBA_PRINTK_LV_WARNING      1
#define AMBA_PRINTK_LV_NOTICE       2
#define AMBA_PRINTK_LV_INFO         3
#define AMBA_PRINTK_LV_DEBUG        4
#define AMBA_PRINTK_LV_NUM          5

/*-----------------------------------------------------------------------------------------------*\
 * Data structure of DSP printk configuration
\*-----------------------------------------------------------------------------------------------*/
typedef struct _AMBA_PRINTK_DSP_CONFIG_s_ {
    UINT32  CoreUcodeAddr;              /* Dsp ucode base address for core */
    UINT32  MemdUcodeAddr;              /* Dsp ucode base address for memd */
    UINT32  MdxfUcodeAddr;              /* Dsp ucode base address for mdxf */
    UINT32  DefaultDataAddr;            /* Dsp ucode base address for default data */
    UINT32  CoreTextAddr;               /* Dsp text base address for code */
    UINT32  MemdTextAddr;               /* Dsp text base address for memd */
    UINT32  MdxfTextAddr;               /* Dsp text base address for mdxf */
} AMBA_PRINTK_DSP_CONFIG_s;

/*-----------------------------------------------------------------------------------------------*\
 * Data structure of print module
\*-----------------------------------------------------------------------------------------------*/
typedef struct _AMBA_PRINT_MODULE_s_ {
    char    Name[10];
    int     Valid;
} AMBA_PRINT_MODULE_s;

/*-----------------------------------------------------------------------------------------------*\
 * Defines for AmbaPrint modules
\*-----------------------------------------------------------------------------------------------*/
#define AMBA_PRINT_MODE_RTOS            0
#define AMBA_PRINT_MODE_DSP             1

typedef enum _AMBA_PRINT_RTOS_MODULE_e_ {
    AMBA_PRINT_MODULE_RTOS_CA9 = 0,

    AMBA_PRINT_NUM_RTOS_MODULE
} AMBA_PRINT_RTOS_MODULE_e;

typedef enum _AMBA_PRINT_DSP_MODULE_e_ {
    AMBA_PRINT_MODULE_DSP_CORE = 0,
    AMBA_PRINT_MODULE_DSP_MDXF,
    AMBA_PRINT_MODULE_DSP_MEMD,

    AMBA_PRINT_NUM_DSP_MODULE
} AMBA_PRINT_DSP_MODULE_e;

/*-----------------------------------------------------------------------------------------------*\
 * Exported Printk APIs in AmbaPrintk.c
\*-----------------------------------------------------------------------------------------------*/
int  AmbaPrintk_SetDspConfig(AMBA_PRINTK_DSP_CONFIG_s *pDspConfig);
int  AmbaPrintk_GetDspConfig(AMBA_PRINTK_DSP_CONFIG_s *pDspConfig);

int  AmbaPrintk_GetRtosLastMsg(int Lines, AMBA_PRINTK_RTOS_LAST_MSG_PARAM_s *pParam);
int  AmbaPrintk_GetDspLastMsg(int Lines, AMBA_PRINTK_DSP_LAST_MSG_PARAM_s *pParam);
int  AmbaPrintk_PrintfDspMsgToBuf(char *pBuf, AMBA_DSP_PRINTF_s *pDspPf, UINT32 CodeAddr, UINT32 MemdAddr, UINT32 MdxfAddr);
void AmbaPrintk_Flush(void);
int  AmbaPrintk(int NewLineEn, int SystemTimeEn, int MoudleStrEn, int MessageLevel, int ConsoleLevel, int MessageColor,
                const char *pFmt, ...);

/*-----------------------------------------------------------------------------------------------*\
 * Exported Printf APIs in AmbaPrintk.c
\*-----------------------------------------------------------------------------------------------*/
int AmbaPrintf(int PrintTarget, int NewLineEn, const char * pFmt, ...);
#define AMBA_PRINTF_TARGET_DEBUGGER     0
#define AMBA_PRINTF_TARGET_UART         1
#define AMBA_PRINTF_TARGET_NUM          2

/*-----------------------------------------------------------------------------------------------*\
 * Default printk configuration in AmbaPrintk_Init()
\*-----------------------------------------------------------------------------------------------*/
#define AMBA_PRINTK_NEW_LINE_DEFAULT            1
#define AMBA_PRINTK_SYSTEM_TIME_DEFAULT         1
#define AMBA_PRINTK_MODULE_STR_DEFAULT          1
#define AMBA_PRINTK_MESSAGE_LEVEL_DEFAULT       AMBA_PRINTK_LV_NOTICE
#define AMBA_PRINTK_CONSOLE_LEVEL_DEFAULT       AMBA_PRINTK_LV_NOTICE
#define AMBA_PRINTK_COLOR_DEFAULT               BLACK

#define AMBA_PRINTK_RTOS_CONSOLE_EN_DEFAULT     1
#define AMBA_PRINTK_DSP_CONSOLE_EN_DEFAULT      0

/*-----------------------------------------------------------------------------------------------*\
 * Default setting of AmbaPrint()
\*-----------------------------------------------------------------------------------------------*/
#define AMBA_PRINTK_NEW_LINE            (-1)
#define AMBA_PRINTK_SYSTEM_TIME         (-1)
#define AMBA_PRINTK_MODULE_STR          (-1)
#define AMBA_PRINTK_MESSAGE_LEVEL       (-1)
#define AMBA_PRINTK_CONSOLE_LEVEL       (-1)
#define AMBA_PRINTK_COLOR               (-1)

/*-----------------------------------------------------------------------------------------------*\
 * Macros to print to message buffer and then output to console.
 * Programmer should use these macros instead of using AmbaPrintk APIs directly.
\*-----------------------------------------------------------------------------------------------*/
 #ifdef CONFIG_ECL_GUI
  #define RELEASE_WITHOUT_PRINT
 #endif

 
#if  defined(RELEASE_WITHOUT_PRINT)

// #define AmbaPrint(...)
#define AmbaPrintColor(...)

#define AmbaPrError(...)
#define AmbaPrWarning(...)
#define AmbaPrNotice(...)
#define AmbaPrInfo(...)
#define AmbaPrDebug(...)

#define AmbaPrintf_DEBUGGER(pFmt, ...)
#define AmbaPrintf_UART(pFmt, ...)

#elif   defined(_AMBA_NON_OS_)

int AmbaNonOS_UartPrintf(const char *pFormatString, ...);
#define AmbaPrint       AmbaNonOS_UartPrintf
#define AmbaPrintColor(...)

#define AmbaPrError(...)
#define AmbaPrWarning(...)
#define AmbaPrNotice(...)
#define AmbaPrInfo(...)
#define AmbaPrDebug(...)

#define AmbaPrintf_DEBUGGER(pFmt, ...)
#define AmbaPrintf_UART(pFmt, ...)

#else

#define AmbaPrintColor(Color, pFmt, ...)       AmbaPrintk(AMBA_PRINTK_NEW_LINE,        AMBA_PRINTK_SYSTEM_TIME, \
                                                       AMBA_PRINTK_MODULE_STR,      AMBA_PRINTK_MESSAGE_LEVEL,  \
                                                       AMBA_PRINTK_CONSOLE_LEVEL,   Color,                      \
                                                       pFmt,                        ##__VA_ARGS__)

#define AmbaPrError(pFmt, ...)              AmbaPrintk(AMBA_PRINTK_NEW_LINE,        AMBA_PRINTK_SYSTEM_TIME,    \
                                                       AMBA_PRINTK_MODULE_STR,      AMBA_PRINTK_LV_ERROR,       \
                                                       AMBA_PRINTK_LV_ERROR,        RED,          \
                                                       pFmt,                        ##__VA_ARGS__)

#define AmbaPrWarning(pFmt, ...)            AmbaPrintk(AMBA_PRINTK_NEW_LINE,        AMBA_PRINTK_SYSTEM_TIME,    \
                                                       AMBA_PRINTK_MODULE_STR,      AMBA_PRINTK_LV_WARNING,     \
                                                       AMBA_PRINTK_LV_WARNING,      BLUE,                       \
                                                       pFmt,                        ##__VA_ARGS__)

#define AmbaPrNotice(pFmt, ...)             AmbaPrintk(AMBA_PRINTK_NEW_LINE,        AMBA_PRINTK_SYSTEM_TIME,    \
                                                       AMBA_PRINTK_MODULE_STR,      AMBA_PRINTK_LV_NOTICE,      \
                                                       AMBA_PRINTK_LV_NOTICE,       MAGENTA,                    \
                                                       pFmt,                        ##__VA_ARGS__)

#define AmbaPrInfo(pFmt, ...)               AmbaPrintk(AMBA_PRINTK_NEW_LINE,        AMBA_PRINTK_SYSTEM_TIME,    \
                                                       AMBA_PRINTK_MODULE_STR,      AMBA_PRINTK_LV_INFO,        \
                                                       AMBA_PRINTK_LV_INFO,         CYAN,                       \
                                                       pFmt,                        ##__VA_ARGS__)

#define AmbaPrDebug(pFmt, ...)              AmbaPrintk(AMBA_PRINTK_NEW_LINE,        AMBA_PRINTK_SYSTEM_TIME,    \
                                                       AMBA_PRINTK_MODULE_STR,      AMBA_PRINTK_LV_DEBUG,       \
                                                       AMBA_PRINTK_LV_DEBUG,        GRAY,                       \
                                                       pFmt,                        ##__VA_ARGS__)

#define AmbaPrintf_DEBUGGER(pFmt, ...)      AmbaPrintf(AMBA_PRINTF_TARGET_DEBUGGER,  1, pFmt, ##__VA_ARGS__)
#define AmbaPrintf_UART(pFmt, ...)          AmbaPrintf(AMBA_PRINTF_TARGET_UART,      1, pFmt, ##__VA_ARGS__)

#endif

/*-----------------------------------------------------------------------------------------------*\
 * Defined for print in debugger.
\*-----------------------------------------------------------------------------------------------*/
void sigprint(const char *t);
static inline int AmbaDebugger_PutStr(char *pStr)
{
    sigprint(pStr);
    return strlen(pStr);
}

/*-----------------------------------------------------------------------------------------------*\
 * Macros to print message to console directly.
\*-----------------------------------------------------------------------------------------------*/
#define AMBA_UART_CHAN_CONSOLE              AMBA_UART_CHANNEL0

/*-----------------------------------------------------------------------------------------------*\
 * Macros for system debug.
\*-----------------------------------------------------------------------------------------------*/
#ifdef __AMBA_NON_OS__
#ifndef K_ASSERT
#define K_ASSERT(x) {                               \
        if (!(x)) {                                 \
            while (1);                              \
        }                                           \
    }
#endif

#else
#ifndef K_ASSERT
#define K_ASSERT(x) {                               \
        if (!(x)) {                                 \
            AmbaPrint("K_ASSERT at %s %d", __FUNCTION__, __LINE__); \
            AmbaPrintk_Flush();                     \
            __disable_interrupt();                  \
            while (1);                              \
        }                                           \
    }
#endif
#endif  /* _AMBA_NON_OS_ */

/*-----------------------------------------------------------------------------------------------*\
 * Defined for system log mechanism
\*-----------------------------------------------------------------------------------------------*/
#define MAX_SYS_LOG_SIZE 0x20000        //128 KB

typedef enum _AMBA_SYS_LOG_TYPE_e_ {
    AMBA_SYS_LOG_NORMAL         = (1),
    AMBA_SYS_LOG_INSERT_LINE    = (1 << 1),
    AMBA_SYS_LOG_SYS_TIME       = (1 << 2),
} AMBA_SYS_LOG_TYPE_e;

typedef struct _ABMA_SYS_LOG_BUF_INFO_s_ {
    UINT8  *Base;
    UINT8  *Ptr;
    UINT8  *Limit;
    UINT32 Size;
    UINT8  State;
} ABMA_SYS_LOG_BUF_INFO_s;

int AmbaSysLog_Init(void);
int AmbaSysLog_Enable(UINT8 Enable);
int AmbaSysLog_Flush(void);
int AmbaSysLog_Write(UINT8 Type, UINT8 ToConsole, const char *fmt, ...);
int AmbaSysLog_GetInfo(ABMA_SYS_LOG_BUF_INFO_s *pSysLogInfo);
int AmbaPrint_RtosEnable(UINT8 Enable);

#define AmbaSysLog(pFmt, ...) AmbaSysLog_Write(AMBA_SYS_LOG_INSERT_LINE | AMBA_SYS_LOG_SYS_TIME, 0, pFmt, ##__VA_ARGS__)

#endif  /* _AMBA_PRINTK_H_ */
