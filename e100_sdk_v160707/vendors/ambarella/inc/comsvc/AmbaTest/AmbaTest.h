/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaTest.h
 *
 *  @Copyright      :: Copyright (C) 2014 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Test command API
\*-------------------------------------------------------------------------------------------------------------------*/

#ifndef _AMBA_TEST_H_
#define _AMBA_TEST_H_

#include "AmbaShell.h"
//#include "AmbaShell_Priv.h"

#define AMBA_TEST_MAX_NAME_LENGTH   20

typedef int (*AMBA_TEST_COMMAND_HANDLER_f)(AMBA_SHELL_ENV_s *, int, char **);

typedef struct _AMBA_TEST_LIST_s_ {
	char		                *Name;
	AMBA_TEST_COMMAND_HANDLER_f	Handler;
	struct _AMBA_TEST_LIST_s_   *pNext;
} AMBA_TEST_LIST_s;

/*-----------------------------------------------------------------------------------------------*\
 * Defined in AmbaTest.c
\*-----------------------------------------------------------------------------------------------*/
void AmbaTest_Init(void);
AMBA_TEST_LIST_s *AmbaTest_GetCommandList(void);
int AmbaTest_RegisterCommand(const char *, AMBA_TEST_COMMAND_HANDLER_f);
int AmbaTest_UnRegisterCommand(const char *, AMBA_TEST_COMMAND_HANDLER_f);
extern int test_gpio(AMBA_SHELL_ENV_s *pShell, int ArgCount, char **pArgVector);
extern int test_spi(AMBA_SHELL_ENV_s *pShell, int ArgCount, char **pArgVector);
extern int test_i2c(AMBA_SHELL_ENV_s *pShell, int ArgCount, char **pArgVector);
extern int test_timer(AMBA_SHELL_ENV_s *pShell, int ArgCount, char **pArgVector);
/*-----------------------------------------------------------------------------------------------*\
 * Defined in AmbaTest_FIO.c
\*-----------------------------------------------------------------------------------------------*/
void AmbaTest_FioAddCommands(void);

/*-----------------------------------------------------------------------------------------------*\
 * Defined in AmbaTest_PIO.c
\*-----------------------------------------------------------------------------------------------*/
void AmbaTest_PioAddCommands(void);

#endif

