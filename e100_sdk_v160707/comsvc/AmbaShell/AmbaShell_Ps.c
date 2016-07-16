/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaShell_Ps.c
 *
 *  @Copyright      :: Copyright (C) 2012 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Ambarella Shell APIs
 \*-------------------------------------------------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include "AmbaDataType.h"
#include "AmbaKAL.h"
#include "AmbaShell.h"
#include "AmbaSysCtrl.h"

/*-----------------------------------------------------------------------------------------------*\
 * Defined in AmbaSysProfile.c
\*-----------------------------------------------------------------------------------------------*/
void AmbaSysProfile_ResetStatistics(void);
void AmbaSysProfile_ShowStatistics(AMBA_SYS_INFO_PRINT_f);

static int SHELL_Print(const char *pFormattedString, ...)
{
    AMBA_SHELL_ENV_s *pShellEnv = AmbaShell_GetEnv();
    char Buf[256];
    int RetVal;
    va_list Args;

    if (pShellEnv == NULL)
        return -1;

    va_start(Args, pFormattedString);
    RetVal = vsprintf(Buf, pFormattedString, Args);
    va_end(Args);
    Buf[RetVal++] = '\0';

    return AmbaShell_Write(pShellEnv, (const UINT8 *) Buf, RetVal);
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: SHELL_PsUsage
 *
 *  @Description:: Show the collected statistics info.
 *
 *  @Input      ::
 *      ArgCount:   number of arguments
 *      pArgVector: argument strings
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static void SHELL_PsUsage(int ArgCount, char **pArgVector)
{
    AMBA_SHELL_ENV_s *pShellEnv = AmbaShell_GetEnv();

    AmbaShell_Print(pShellEnv, "Usage: %s [tsk|mtx|sem|flg|msg|mem|all|irq|profile] ([option])\n", pArgVector[0]);
    AmbaShell_Print(pShellEnv, "       %s tsk - Task info\n", pArgVector[0]);
    AmbaShell_Print(pShellEnv, "       %s mtx - Mutex info\n", pArgVector[0]);
    AmbaShell_Print(pShellEnv, "       %s sem - Semaphore info\n", pArgVector[0]);
    AmbaShell_Print(pShellEnv, "       %s flg - Eventflag info\n", pArgVector[0]);
    AmbaShell_Print(pShellEnv, "       %s msg - Message queue info\n", pArgVector[0]);
    AmbaShell_Print(pShellEnv, "       %s mem - Memory pool info\n", pArgVector[0]);
    AmbaShell_Print(pShellEnv, "       %s all - All the above info\n", pArgVector[0]);
    AmbaShell_Print(pShellEnv, "       %s irq [reset|all|<id>] - IRQ info\n", pArgVector[0]);
    AmbaShell_Print(pShellEnv, "       %s profile [reset] - profiler result\n", pArgVector[0]);
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: ambsh_ps
 *
 *  @Description:: Process State command.
 *
 *  @Input      ::
 *      pShellEnv:  shell pShellEnvironment
 *      ArgCount:   number of arguments
 *      pArgVector: argument strings
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
int ambsh_ps(AMBA_SHELL_ENV_s *pShellEnv, int ArgCount, char **pArgVector)
{
    if (ArgCount == 1) {
        SHELL_PsUsage(ArgCount, pArgVector);
    } else if (strcmp(pArgVector[1], "tsk") == 0) {
        AmbaSysShowTaskInfo(SHELL_Print);
    } else if (strcmp(pArgVector[1], "mtx") == 0) {
        AmbaSysShowMutexInfo(SHELL_Print);
    } else if (strcmp(pArgVector[1], "sem") == 0) {
        AmbaSysShowSemaphoreInfo(SHELL_Print);
    } else if (strcmp(pArgVector[1], "flg") == 0) {
        AmbaSysShowEventFlagInfo(SHELL_Print);
    } else if (strcmp(pArgVector[1], "msg") == 0) {
        AmbaSysShowMsgQueueInfo(SHELL_Print);
    } else if (strcmp(pArgVector[1], "mem") == 0) {
        AmbaSysShowBytePoolInfo(SHELL_Print);
        AmbaSysShowBlockPoolInfo(SHELL_Print);
    } else if (strcmp(pArgVector[1], "all") == 0) {
        AmbaSysShowTaskInfo(SHELL_Print);
        AmbaSysShowMutexInfo(SHELL_Print);
        AmbaSysShowSemaphoreInfo(SHELL_Print);
        AmbaSysShowEventFlagInfo(SHELL_Print);
        AmbaSysShowMsgQueueInfo(SHELL_Print);
        AmbaSysShowBytePoolInfo(SHELL_Print);
        AmbaSysShowBlockPoolInfo(SHELL_Print);
    } else if (strcmp(pArgVector[1], "irq") == 0) {
        if (ArgCount >= 3) {
            if (strcmp(pArgVector[2], "reset") == 0) {
                AmbaSysResetIrqCount();
            } else if (strcmp(pArgVector[2], "all") == 0) {
                AmbaSysShowIrqInfo(SHELL_Print, -1);
            } else {
                AmbaSysShowIrqInfo(SHELL_Print, strtoul(pArgVector[2], NULL, 0));
            }
        } else {
            AmbaSysShowIrqInfo(SHELL_Print, -1);
        }
    } else if (strcmp(pArgVector[1], "profile") == 0) {
        if (ArgCount >= 3 && strcmp(pArgVector[2], "reset") == 0) {
            AmbaSysProfile_ResetStatistics();
        } else {
            AmbaSysProfile_ShowStatistics(SHELL_Print);
        }
    } else {
        SHELL_PsUsage(ArgCount, pArgVector);
        return NG;
    }

    return OK;
}
