/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaTest.c
 *
 *  @Copyright      :: Copyright (C) 2014 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Test command API
\*-------------------------------------------------------------------------------------------------------------------*/

#include <stdio.h>
#include <string.h>

#include "AmbaDataType.h"
#include "AmbaKAL.h"

#define __TEST_IMPL__
#include "AmbaTest.h"

#define AMBA_TEST_BUFFER_SIZE 0x4000
static UINT8 AmbaTestBuffer[AMBA_TEST_BUFFER_SIZE];

static AMBA_KAL_BYTE_POOL_t AmbaTestBytePool;
static AMBA_KAL_MUTEX_t AmbaTestMutex;

static int AmbaTest_Help(AMBA_SHELL_ENV_s *pShell, int ArgCount, char **pArgVector);

static AMBA_TEST_LIST_s AmbaTestCmdListHead = {
    .Name       = "help",
    .Handler    = AmbaTest_Help,
    .pNext      = NULL
};

int ambsh_t(AMBA_SHELL_ENV_s *pShell, int ArgCount, char **ArgVector)
{
	AMBA_TEST_LIST_s *pCmd = AmbaTest_GetCommandList();

	if (ArgCount == 1) {
        pCmd->Handler(pShell, ArgCount - 1, &ArgVector[1]); /* Show help page */

		AmbaShell_Print(pShell, "supported test commands:\n");
        while (pCmd) {
			AmbaShell_Print(pShell, "\t%s\n", pCmd->Name);
            pCmd = pCmd->pNext;
        }
	} else {
        while (pCmd) {
			if (strcmp(ArgVector[1], pCmd->Name) == 0) {
				return pCmd->Handler(pShell, ArgCount - 1, &ArgVector[1]);
			}
            pCmd = pCmd->pNext;
        }

		AmbaShell_Print(pShell, "%s: test command not found!\n", ArgVector[1]);
    }

	return 0;
}
/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaTest_Help
 *
 *  @Description:: Help page of test command.
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static int AmbaTest_Help(AMBA_SHELL_ENV_s *pShell, int ArgCount, char **pArgVector)
{
    return AmbaShell_Print(pShell, "\rAmbaShell Test Command [Version 1.0.0000]\n"
    "\r(C) Copyright 2004-2014 Ambarella Corp.\n\n");
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaTest_Init
 *
 *  @Description:: Initialize test commands.
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
void AmbaTest_Init(void)
{
    AmbaKAL_BytePoolCreate(&AmbaTestBytePool, (void *) AmbaTestBuffer, sizeof(AmbaTestBuffer));
    AmbaKAL_MutexCreate(&AmbaTestMutex);
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaTest_RegisterCommand
 *
 *  @Description:: Register test command.
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *      int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
int AmbaTest_RegisterCommand(const char *CmdName, AMBA_TEST_COMMAND_HANDLER_f CmdHandler)
{
    AMBA_TEST_LIST_s *pNewCommand = NULL;
    AMBA_TEST_LIST_s *pCommand = &AmbaTestCmdListHead;

    if (CmdHandler == NULL || CmdName == NULL || strlen(CmdName) > (AMBA_TEST_MAX_NAME_LENGTH - 1))
        return NG;

    /*-----------------------------------------------------------------------*\
     * Take the Mutex
    \*-----------------------------------------------------------------------*/
    if (AmbaKAL_MutexTake(&AmbaTestMutex, AMBA_KAL_WAIT_FOREVER) != OK)
        return NG;  /* should never happen */

    if (AmbaKAL_BytePoolAllocate(&AmbaTestBytePool, (void**) &pNewCommand, sizeof(AMBA_TEST_LIST_s), AMBA_KAL_NO_WAIT) != OK) {
        return NG;
    }

    if (AmbaKAL_BytePoolAllocate(&AmbaTestBytePool, (void**) &pNewCommand->Name, strlen(CmdName) + 1, AMBA_KAL_NO_WAIT) != OK) {
        AmbaKAL_BytePoolFree((void*) pNewCommand);
        return NG;
    }
    strncpy(pNewCommand->Name, CmdName, strlen(CmdName) + 1);

    pNewCommand->Handler = CmdHandler;
    pNewCommand->pNext = NULL;

    while (pCommand->pNext != NULL) {
        pCommand = pCommand->pNext;
    }
    pCommand->pNext = pNewCommand;

    /*-----------------------------------------------------------------------*\
     * Release the Mutex
    \*-----------------------------------------------------------------------*/
    AmbaKAL_MutexGive(&AmbaTestMutex);

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaTest_UnRegisterCommand
 *
 *  @Description:: UnRegister test command.
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *      int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
int AmbaTest_UnRegisterCommand(const char *CmdName, AMBA_TEST_COMMAND_HANDLER_f CmdHandler)
{
    AMBA_TEST_LIST_s *pCommand = &AmbaTestCmdListHead;
    AMBA_TEST_LIST_s *pLastCommand = NULL;

    /*-----------------------------------------------------------------------*\
     * Take the Mutex
    \*-----------------------------------------------------------------------*/
    if (AmbaKAL_MutexTake(&AmbaTestMutex, AMBA_KAL_WAIT_FOREVER) != OK)
        return NG;  /* should never happen */

    while (pCommand != NULL) {
        if (strncmp(pCommand->Name, CmdName, AMBA_TEST_MAX_NAME_LENGTH) == 0)
            break;

        pLastCommand = pCommand;
        pCommand = pCommand->pNext;
    }

    if (pCommand->Handler == CmdHandler) {
        if (pLastCommand == NULL) {
            /* should never happen */
        } else {
            pLastCommand->pNext = pCommand->pNext;
            AmbaKAL_BytePoolFree((void*) pCommand->Name);
            AmbaKAL_BytePoolFree((void*) pCommand);
        }
    }

    /*-----------------------------------------------------------------------*\
     * Release the Mutex
    \*-----------------------------------------------------------------------*/
    AmbaKAL_MutexGive(&AmbaTestMutex);

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaTest_GetCommandList
 *
 *  @Description:: Get registered test command list.
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *      AMBA_TEST_LIST_s : Test command list
\*-----------------------------------------------------------------------------------------------*/
AMBA_TEST_LIST_s *AmbaTest_GetCommandList(void)
{
    return &AmbaTestCmdListHead;
};
