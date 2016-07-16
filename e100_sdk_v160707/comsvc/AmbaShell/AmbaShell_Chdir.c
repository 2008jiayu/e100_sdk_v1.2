/**
 * system/src/ui/ambsh/cd.c
 *
 * History:
 *    2005/11/10 - [Charles Chiou] created file
 *
 * Copyright (C) 2004-2005, Ambarella, Inc.
 *
 * All rights reserved. No Part of this file may be reproduced, stored
 * in a retrieval system, or transmitted, in any form, or by any means,
 * electronic, mechanical, photocopying, recording, or otherwise,
 * without the prior consent of Ambarella, Inc.
 */

#include    <stdio.h>
#include    <string.h>

#include    "AmbaDataType.h"
#include    "AmbaKAL.h"
#include    "AmbaShell.h"

#include    "AmbaFS.h"

static inline void usage(AMBA_SHELL_ENV_s *pEnv , int Argc, char **Argv)
{
    AmbaShell_Print(pEnv , "Usage: %s [path]\n", Argv[0]);
}

int ambsh_cd(AMBA_SHELL_ENV_s *pEnv , int Argc, char **Argv)
{
    int Rval;
    int l;
    char Buf[512];

    if (Argc != 2) {
        usage(pEnv , Argc, Argv);
        return -1;
    }

    /* cd . is a nop */
    if (Argv[1][0] == '.' && Argv[1][1] == '\0')
        return 0;
    /* cd .. is a nop */
    if (Argv[1][0] == '.' && Argv[1][1] == '.' && Argv[1][2] == '\0' &&
        pEnv ->Cwd[3] == '\0')
        return 0;

    if (Argv[1][1] == ':') {

        AmbaShell_Ascii2Unicode(Argv[1], Buf);

        Rval = AmbaFS_Chdir(Buf);
        if (Rval < 0) {
            AmbaShell_Print(pEnv , "%s: '%s': %s!\n",
                            Argv[0],
                            Argv[1],
                            AmbaShell_GetFsError());
            return -2;
        } else {
            strncpy(pEnv ->Cwd, Argv[1], sizeof(pEnv ->Cwd));
        }
    } else {
        char tmp[256];

        AMBSH_CHKCWD();

        strcpy(tmp, pEnv ->Cwd);
        if ((strlen(tmp) + strlen(Argv[1])) < sizeof(tmp)) {
            strcat(tmp, Argv[1]);
        } else {
            AmbaShell_Print(pEnv , "cd %s failed", Argv[1]);
            return -1;
        }

        AmbaShell_Ascii2Unicode(tmp, Buf);

        Rval = AmbaFS_Chdir(Buf);
        if (Rval < 0) {
            AmbaShell_Print(pEnv , "%s '%s' %s\n",
                            Argv[0],
                            tmp,
                            AmbaShell_GetFsError());
            return -2;
        } else {
            strcpy(pEnv ->Cwd, tmp);
        }
    }

    /* We are here because of success in AmbaFS_Chdir() */
    if (Argv[1][0] == '.' && Argv[1][1] == '.' && Argv[1][2] == '\0') {
        /* go back up one directory if it was a 'cd ..' */
        l = strlen(pEnv ->Cwd);
        while (pEnv ->Cwd[l] != '\\') {
            pEnv ->Cwd[l] = '\0';
            l--;
        }
        pEnv ->Cwd[l] = '\0';
        l--;
        while (pEnv ->Cwd[l] != '\\') {
            pEnv ->Cwd[l] = '\0';
            l--;
        }
        pEnv ->Cwd[l] = '\0';
        l--;
    }

    /* make sure the last character is '\\' */
    l = strlen(pEnv ->Cwd);
    if (pEnv ->Cwd[l - 1] != '\\') {
        pEnv ->Cwd[l] = '\\';
        pEnv ->Cwd[l + 1] = '\0';
    }

    if (pEnv ->pParent)
        strcpy(pEnv ->pParent->Cwd, pEnv ->Cwd);

    return 0;
}

