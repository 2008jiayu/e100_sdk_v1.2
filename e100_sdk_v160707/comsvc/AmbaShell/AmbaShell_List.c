/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaShell_List.c
 *
 *  @Copyright      :: Copyright (C) 2014 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Ambarella shell command list management.
\*-------------------------------------------------------------------------------------------------------------------*/

#include <stdarg.h>
#include <string.h>
#include <stdio.h>

#include <AmbaDataType.h>
#include "AmbaPrintk.h"
#include "AmbaKAL.h"

#include "AmbaShell.h"
#include "AmbaShell_Priv.h"

#define __AMBSH_DECL(x)     extern int ambsh_##x(AMBA_SHELL_ENV_s *, int, char **)
#define __AMBSH_ENTRY(x)    { #x, ambsh_##x, (AMBA_SHELL_LIST_s *) NULL }

__AMBSH_DECL(cardmgr);
__AMBSH_DECL(cat);
__AMBSH_DECL(cd);
__AMBSH_DECL(chmod);
__AMBSH_DECL(date);
__AMBSH_DECL(dmesg);
__AMBSH_DECL(echo);
__AMBSH_DECL(format);
__AMBSH_DECL(help);
__AMBSH_DECL(ls);
__AMBSH_DECL(mkdir);
__AMBSH_DECL(mv);
__AMBSH_DECL(ps);
__AMBSH_DECL(pwd);
__AMBSH_DECL(readb);
__AMBSH_DECL(readl);
__AMBSH_DECL(readw);
__AMBSH_DECL(reboot);
__AMBSH_DECL(hotboot);
__AMBSH_DECL(poweroff);
__AMBSH_DECL(rm);
__AMBSH_DECL(savebin);
__AMBSH_DECL(sleep);
__AMBSH_DECL(suspend);
__AMBSH_DECL(true);
__AMBSH_DECL(ver);
__AMBSH_DECL(vol);
__AMBSH_DECL(cleandir);
__AMBSH_DECL(writeb);
__AMBSH_DECL(writel);
__AMBSH_DECL(writew);
__AMBSH_DECL(firmfl);
__AMBSH_DECL(nftl);
#ifdef AMBA_LINK
__AMBSH_DECL(ambalink);
#endif
__AMBSH_DECL(t);

extern AMBA_KAL_MUTEX_t _AmbaShell_CmdListMutex;

AMBA_SHELL_LIST_s AmbaShell_DefaultCmdList[] = {
    __AMBSH_ENTRY(help),
    __AMBSH_ENTRY(echo),
    __AMBSH_ENTRY(dmesg),
    __AMBSH_ENTRY(sleep),
    __AMBSH_ENTRY(readb),
    __AMBSH_ENTRY(readl),
    __AMBSH_ENTRY(readw),
    __AMBSH_ENTRY(writeb),
    __AMBSH_ENTRY(writel),
    __AMBSH_ENTRY(writew),
    __AMBSH_ENTRY(nftl),
    __AMBSH_ENTRY(ps),
#ifndef AMBA_SHELL_A12_DEVELOP
    __AMBSH_ENTRY(suspend),
    __AMBSH_ENTRY(date),
    __AMBSH_ENTRY(firmfl),
#endif
    __AMBSH_ENTRY(cd),
    __AMBSH_ENTRY(ls),
    __AMBSH_ENTRY(format),
    __AMBSH_ENTRY(reboot),
    __AMBSH_ENTRY(hotboot),
    __AMBSH_ENTRY(poweroff),
    __AMBSH_ENTRY(cardmgr),
	__AMBSH_ENTRY(ver),
    __AMBSH_ENTRY(vol),
    __AMBSH_ENTRY(true),
    __AMBSH_ENTRY(mkdir),
#ifndef AMBA_SHELL_NO_FS
    __AMBSH_ENTRY(pwd),
    __AMBSH_ENTRY(chmod),
    __AMBSH_ENTRY(cat),
    __AMBSH_ENTRY(rm),
    __AMBSH_ENTRY(cleandir),
    __AMBSH_ENTRY(mv),
    __AMBSH_ENTRY(savebin),
#endif
#ifdef AMBA_LINK
    //    __AMBSH_ENTRY(ambalink),
#endif
    __AMBSH_ENTRY(t),
};

UINT32 AmbaShell_DefaultCmdListSize = GetArraySize(AmbaShell_DefaultCmdList);

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaShell_AddCmd
 *
 *  @Description:: Add user-defined ambsh commands.
 *
 *  @Input      ::
 *                 pName : Command name to delete
 *                 Proc  : Data structure of AmbaShell_Proc
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
int AmbaShell_AddCmd(const char *pName, AMBA_SHELL_PROC_f Proc)
{
    AMBA_SHELL_LIST_s *pNewCmd = NULL;
    AMBA_SHELL_LIST_s *pCmd;

    if (pName == NULL || Proc == NULL)
        return NG;

    if (AmbaShell_MemAlloc((void**) &pNewCmd, sizeof(AMBA_SHELL_LIST_s)) != OK) {
        return NG;
    }

    if (AmbaShell_MemAlloc((void**) &pNewCmd->pName, strlen(pName) + 1) != OK) {
        AmbaShell_MemFree((void *) pNewCmd);
        return NG;
    }

    AmbaKAL_MutexTake(&_AmbaShell_CmdListMutex, AMBA_KAL_WAIT_FOREVER);

    pCmd = &AmbaShell_DefaultCmdList[AmbaShell_DefaultCmdListSize - 1];
    while (pCmd->pNext != NULL) {
        pCmd = pCmd->pNext;
    }
    pCmd->pNext = pNewCmd;

    strcpy((char *) pNewCmd->pName, pName);
    pNewCmd->Proc = Proc;
    pNewCmd->pNext = NULL;

    AmbaKAL_MutexGive(&_AmbaShell_CmdListMutex);

    return 0;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaShell_DeleteCmd
 *
 *  @Description:: Delete user-defined ambsh command.
 *
 *  @Input      ::
 *                 pName : Command name to delete
 *                 Proc  : Data structure of AmbaShell_Proc
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
int AmbaShell_DeleteCmd(const char *pName, AMBA_SHELL_PROC_f Proc)
{
    AMBA_SHELL_LIST_s *pCmd;
    AMBA_SHELL_LIST_s *pNextCmd;

    AmbaKAL_MutexTake(&_AmbaShell_CmdListMutex, AMBA_KAL_WAIT_FOREVER);

    pCmd = &AmbaShell_DefaultCmdList[AmbaShell_DefaultCmdListSize - 1];
    while (pCmd->pNext) {
        pNextCmd = pCmd->pNext;
        if (strcmp(pNextCmd->pName, pName) == 0 && pNextCmd->Proc == Proc) {
            pCmd->pNext = pNextCmd->pNext;
            if (pNextCmd->pName)
                AmbaShell_MemFree((void *) pNextCmd->pName);
            AmbaShell_MemFree((void *) pNextCmd);
            break;
        }
        pCmd = pCmd->pNext;
    }

    AmbaKAL_MutexGive(&_AmbaShell_CmdListMutex);

    return 0;
}

