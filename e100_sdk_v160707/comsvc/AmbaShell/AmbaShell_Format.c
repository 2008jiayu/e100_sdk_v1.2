/**
 * system/src/ui/ambsh/format.c
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
#include    <ctype.h>

#include    "AmbaDataType.h"
#include    "AmbaKAL.h"
#include    "AmbaShell.h"

#include    "AmbaFS.h"
#include    "AmbaCardManager.h"

static inline void usage(AMBA_SHELL_ENV_s *env, int Argc, char **Argv)
{
    AmbaShell_Print(env, "usage: %s [drive] [param]\n", Argv[0]);
    AmbaShell_Print(env,
                    "\t[param]: FAT12|FAT16|FAT32|UDF|udf|MS_FULL|MS_QUICK|"
                    "ll1|ll2|ll3|DVD-RAM|DVD-SINGLE|DVD-MULTI,"
                    "SPC=<num>,BS=<num>\n");
}

int ambsh_format(AMBA_SHELL_ENV_s *env, int Argc, char **Argv)
{
    int i, rval;
    int slot = -1;
    char drv;
    char dummy = '\0';
    AMBA_SCM_STATUS_s status;

    for (i = 0; i < Argc; i++) {
        if (Argv[i][1] == '\0') {
            slot = tolower((int) Argv[i][0]) - 'a';
        }
    }

    if (slot == -1) {
        usage(env, Argc, Argv);
        return -1;
    }

    rval = AmbaSCM_GetSlotStatus(slot, &status);
    if (rval < 0 || !status.CardPresent) {
        AmbaShell_Print(env, "media unavailable!\n");
        return -2;
    }

    drv = 'a' + slot;
    if (Argc == 3)
        rval = AmbaFS_Format(drv, Argv[2]);
    else
        rval = AmbaFS_Format(drv, &dummy);

    if (rval < 0) {
        AmbaShell_Print(env, "%s: %s!\n", Argv[0], AmbaShell_GetFsError());
        return -3;
    }

    AmbaShell_Print(env, "format successful!\n");

    return 0;
}

