/**
 * system/src/ui/ambsh/mkdir.c
 *
 * History:
 *    2005/11/11 - [Charles Chiou] created file
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
    AmbaShell_Print(pEnv , "Usage: %s [dir]\n", Argv[0]);
}

int ambsh_mkdir(AMBA_SHELL_ENV_s *pEnv , int Argc, char **Argv)
{
    int Rval;

    if (Argc != 2) {
        usage(pEnv , Argc, Argv);
        return -1;
    }

    if (Argv[1][1] == ':') {
        Rval = AmbaFS_Mkdir(Argv[1]);
        if (Rval < 0) {
            AmbaShell_Print(pEnv ,
                            "%s: '%s': cannot create directory!\n",
                            Argv[0], Argv[1]);
            return -2;
        }
    } else {
        char tmp[256];
        char buf[256];

        AMBSH_CHKCWD();

        strcpy(tmp, pEnv ->Cwd);
        if ((strlen(tmp) + strlen(Argv[1])) < sizeof(tmp)) {
            strcat(tmp, Argv[1]);
        } else {
            AmbaShell_Print(pEnv , "mkdir %s failed", Argv[1]);
            return -1;
        }

        AmbaShell_Ascii2Unicode(tmp, buf);
        Rval = AmbaFS_Mkdir(buf);
        if (Rval < 0) {
            AmbaShell_Print(pEnv , "%s: '%s': cannot create directory!\n", Argv[0], tmp);
            return -2;
        }
    }

    return 0;
}

