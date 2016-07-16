/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaExceptionHandler.c
 *
 *  @Copyright      :: Copyright (C) 2013 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Exception handler.
\*-------------------------------------------------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <intrinsics.h>

#include "AmbaDataType.h"
#include "AmbaKAL.h"

#include "AmbaRTSL_UART.h"

UINT32 _AmbaCoreDumpStack[64] __attribute__((aligned(4)));

#define USR26_MODE      0x00
#define FIQ26_MODE      0x01
#define IRQ26_MODE      0x02
#define SVC26_MODE      0x03
#define USR_MODE        0x10
#define FIQ_MODE        0x11
#define IRQ_MODE        0x12
#define SVC_MODE        0x13
#define ABT_MODE        0x17
#define UND_MODE        0x1b
#define SYSTEM_MODE     0x1f
#define MODE_MASK       0x1f
#define T_BIT           0x20
#define F_BIT           0x40
#define I_BIT           0x80
#define CC_V_BIT        0x10000000
#define CC_C_BIT        0x20000000
#define CC_Z_BIT        0x40000000
#define CC_N_BIT        0x80000000
#define PCMASK          0x0

#define ARM_ORIG_R0     _AmbaCoreDumpStack[17]
#define ARM_CPSR        _AmbaCoreDumpStack[16]
#define ARM_PC          _AmbaCoreDumpStack[15]
#define ARM_LR          _AmbaCoreDumpStack[14]
#define ARM_SP          _AmbaCoreDumpStack[13]
#define ARM_IP          _AmbaCoreDumpStack[12]
#define ARM_FP          _AmbaCoreDumpStack[11]
#define ARM_R10         _AmbaCoreDumpStack[10]
#define ARM_R9          _AmbaCoreDumpStack[9]
#define ARM_R8          _AmbaCoreDumpStack[8]
#define ARM_R7          _AmbaCoreDumpStack[7]
#define ARM_R6          _AmbaCoreDumpStack[6]
#define ARM_R5          _AmbaCoreDumpStack[5]
#define ARM_R4          _AmbaCoreDumpStack[4]
#define ARM_R3          _AmbaCoreDumpStack[3]
#define ARM_R2          _AmbaCoreDumpStack[2]
#define ARM_R1          _AmbaCoreDumpStack[1]
#define ARM_R0          _AmbaCoreDumpStack[0]

#define PCPointer(v)            ((v) & ~PCMASK)
#define InstructionPointer()    (PCPointer(ARM_PC))
#define UserNode()              ((ARM_CPSR & 0xf) == 0)

#define ThumbMode()             ((ARM_CPSR & T_BIT))

#define ProcessorMode()         (ARM_CPSR & MODE_MASK)
#define IRQEnabled()            (!(ARM_CPSR & I_BIT))
#define FIQEnabled()            (!(ARM_CPSR & F_BIT))
#define ConditionCodes()        (ARM_CPSR & (CC_V_BIT|CC_C_BIT|CC_Z_BIT|CC_N_BIT))

static const char *_ProcessorMode[] = {
    "USER_26",  "FIQ_26",   "IRQ_26",   "SVC_26",
    "UK4_26",   "UK5_26",   "UK6_26",   "UK7_26",
    "UK8_26",   "UK9_26",   "UK10_26",  "UK11_26",
    "UK12_26",  "UK13_26",  "UK14_26",  "UK15_26",
    "USER_32",  "FIQ_32",   "IRQ_32",   "SVC_32",
    "UK4_32",   "UK5_32",   "UK6_32",   "ABT_32",
    "UK8_32",   "UK9_32",   "UK10_32",  "UND_32",
    "UK12_32",  "UK13_32",  "UK14_32",  "SYS_32"
};

static const char *_ExceptionTag[] = {
    "--- ARM UNDEFINED INSTRUCTION EXCEPTION ---",
    "--- ARM PREFETCH EXCEPTION ---",
    "--- ARM DATA ABORT EXCEPTION ---",
};

static VOID_UINT32_IN_FUNCTION _UserDefinedExceptionHandler = NULL;   /* pointer to the Callback Function */

static char _ExceptionStrBuf[256]__attribute__((section(".bss.noinit")));

static int Exception_Print(const char *pFmt, ...)
{
    char *pTxStr = _ExceptionStrBuf;
    va_list Args;
    int NumChar;
    int RetStatus;

    if (pFmt == NULL)
        return NG;

    va_start(Args, pFmt);

    /* Print to temporary buffer first */
    NumChar = vsprintf(_ExceptionStrBuf, pFmt, Args);
    _ExceptionStrBuf[NumChar++] = '\r';
    _ExceptionStrBuf[NumChar++] = '\n';
    _ExceptionStrBuf[NumChar++] = '\0';

    va_end(Args);

    while ((RetStatus = AmbaRTSL_UartWrite(AMBA_UART_CHANNEL0, NumChar, (UINT8 *)pTxStr)) != NG) {
        NumChar -= RetStatus;
        pTxStr += RetStatus;
        if (NumChar <= 0)
            break;
    }

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaExceptionHandler
 *
 *  @Description:: Exception Handler
 *
 *  @Input      ::
 *      ExceptionID: Exception ID
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
void AmbaExceptionHandler(UINT32 ExceptionID)
{
    UINT32 Flags = ConditionCodes();

    Exception_Print("%s", _ExceptionTag[ExceptionID]);
    ARM_PC -= 0x4 * ExceptionID;

    Exception_Print("Oops: CPU Exception!");
    Exception_Print("pc : [<%08lx>]    lr : [<%08lx>]",
                    InstructionPointer(), ARM_LR);

    Exception_Print("sp : %08lx  ip : %08lx  fp : %08lx",
                    ARM_SP, ARM_IP, ARM_FP);
    Exception_Print("r10: %08lx  r9 : %08lx  r8 : %08lx",
                    ARM_R10, ARM_R9, ARM_R8);

    Exception_Print("r7 : %08lx  r6 : %08lx  r5 : %08lx",
                    ARM_R7, ARM_R6, ARM_R5);

    Exception_Print("r4 : %08lx  r3 : %08lx  r2 : %08lx",
                    ARM_R4, ARM_R3, ARM_R2);

    Exception_Print("r1 : %08lx  r0 : %08lx",
                    ARM_R1, ARM_R0);

    Exception_Print("Flags: %c%c%c%c",
                    (Flags & CC_N_BIT) ? 'N' : 'n',
                    (Flags & CC_Z_BIT) ? 'Z' : 'z',
                    (Flags & CC_C_BIT) ? 'C' : 'c',
                    (Flags & CC_V_BIT) ? 'V' : 'v');

    Exception_Print("IRQs %s  FIQs %s  Mode %s%s",
                    IRQEnabled() ? "on" : "off",
                    FIQEnabled() ? "on" : "off",
                    _ProcessorMode[ProcessorMode()],
                    ThumbMode() ? " (T)" : "");

    if (_UserDefinedExceptionHandler != NULL)
        _UserDefinedExceptionHandler(ExceptionID);  /* invoke User Defined Exception Handler */

    while (1) { /* step here */
      //  __WFI();
      __asm__ __volatile__ ("wfi" : : : "memory");
    }
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaUserDefinedExceptionHandlerRegister
 *
 *  @Description:: Register User Defined Exception Handler
 *
 *  @Input      ::
 *      pUserDefinedExceptionHandler: pointer to the User Defined Exception Handler
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
void AmbaUserDefinedExceptionHandlerRegister(VOID_UINT32_IN_FUNCTION pUserDefinedExceptionHandler)
{
    _UserDefinedExceptionHandler = pUserDefinedExceptionHandler;
}
