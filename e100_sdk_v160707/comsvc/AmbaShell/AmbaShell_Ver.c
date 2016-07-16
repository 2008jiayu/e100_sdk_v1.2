/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaShell_Ver.c
 *
 *  @Copyright      :: Copyright (C) 2014 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: the module of Ambarella Network Support
\*-------------------------------------------------------------------------------------------------------------------*/

#include    <stdio.h>
#include    <string.h>

#include    "AmbaDataType.h"
#include    "AmbaKAL.h"
#include    "AmbaShell.h"
#include    "AmbaVer.h"

extern const char *pAmbaVer_LinkLibs[];
extern unsigned int AmbaVer_LinkLibsNum;

static void Show(AMBA_SHELL_ENV_s *pEnv, AMBA_VerInfo_s *pVerInfo)
{
	AmbaShell_Print(pEnv, "Module:          %s\n", pVerInfo->Key);
	AmbaShell_Print(pEnv, "Built Machine:   %s\n", pVerInfo->MachStr);

	if (pVerInfo->DateStr != NULL)
		if (strcmp(pVerInfo->DateStr, ""))
			AmbaShell_Print(pEnv, "Built Date:      %s\n", pVerInfo->DateStr);

	AmbaShell_Print(pEnv, "Commit count:    %d\n", pVerInfo->CiCount);
	AmbaShell_Print(pEnv, "Commit ID:       %s\n", pVerInfo->CiIdStr);
}

static void ShowAllVerInfo(AMBA_SHELL_ENV_s *pEnv)
{
	int i, j;
	const char *pModuleName;
	AMBA_VerInfo_s *pVerInfo = NULL;

	for (j = 0; j < AmbaVer_LinkLibsNum; j++) {
		for (i = 0; i < AmbaVer_GetCount(); i++) {
			pModuleName = AmbaVer_GetKey(i);

			if (strcmp(pAmbaVer_LinkLibs[j], pModuleName) == 0) {
				pVerInfo = AmbaVer_GetVerInfo(pModuleName);
				if (pVerInfo)
					Show(pEnv, pVerInfo);

				break;
			}
		}
	}
}

static void ShowLinkVerInfo(AMBA_SHELL_ENV_s *pEnv)
{
	const char *pTarget;
	const char *pMachine;
	const char *pDate;

	AmbaVer_GetLinkVerInfo(&pTarget, &pMachine, &pDate);
	AmbaShell_Print(pEnv, "Built %s by %s at %s\n", pTarget, pMachine, pDate);
}

static void ShowAllLibs(AMBA_SHELL_ENV_s *pEnv)
{
	int i;

	for (i = 0; i < AmbaVer_LinkLibsNum; i++) {
		AmbaShell_Print(pEnv, "%s\n", pAmbaVer_LinkLibs[i]);
	}
}

static void ShowVerInfo(AMBA_SHELL_ENV_s *pEnv, const char *pModuleName)
{
	AMBA_VerInfo_s *pVerInfo;

	pVerInfo = AmbaVer_GetVerInfo(pModuleName);
	if (pVerInfo)
		Show(pEnv, pVerInfo);
}


static void usage(AMBA_SHELL_ENV_s *pEnv , int Argc, char **Argv)
{
    AmbaShell_Print(pEnv , "Usage: %s               Show all libraries Version\n"
    					   "       %s [LibName]     Show the library version\n"
    					   "       %s -e            Show elf info\n"
    					   "       %s -h            Show help\n",
    					   Argv[0], Argv[0], Argv[0], Argv[0]);
}

int ambsh_ver(AMBA_SHELL_ENV_s *pEnv , int Argc, char **Argv)
{
	if (Argc == 1) {
		ShowAllVerInfo(pEnv);
		ShowLinkVerInfo(pEnv);

	} else if (!strcmp(Argv[1], "-h")) {
		usage(pEnv, Argc, Argv);
		AmbaShell_Print(pEnv, "Avaiable library name:\n");
		ShowAllLibs(pEnv);

	} else if (Argc == 2 && !strcmp(Argv[1], "-e")) {
		ShowLinkVerInfo(pEnv);

	} else if (Argc == 2) {
		ShowVerInfo(pEnv, Argv[1]);
	}
    return 0;
}

