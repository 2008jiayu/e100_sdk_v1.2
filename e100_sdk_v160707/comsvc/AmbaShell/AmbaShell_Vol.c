/**
 * system/src/ui/ambsh/vol.c
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
#include    <ctype.h>

#include    "AmbaDataType.h"
#include    "AmbaKAL.h"
#include    "AmbaShell.h"

#include    "AmbaFS_Def.h"
#include    "AmbaFS.h"

static inline void usage(AMBA_SHELL_ENV_s *env, int argc, char **argv)
{
    AmbaShell_Print(env, "Usage: %s [drive] [name]\n", argv[0]);
}

int ambsh_vol(AMBA_SHELL_ENV_s *env, int argc, char **argv)
{
    int rval;
    AMBA_FS_VOLTAB ff_voltab;
    AMBA_FS_DEVINF ff_devinf;
    char name[12];

    if (argc > 3 || argc < 2 || argv[1][1] != '\0') {
        usage(env, argc, argv);
        return -1;
    }

    if (argc == 2) {
        rval = AmbaFS_GetVol(argv[1][0], &ff_voltab);
        if (rval < 0)
            memset(&ff_voltab, 0x0, sizeof(AMBA_FS_VOLTAB));

        rval = AmbaFS_GetDev(argv[1][0], &ff_devinf);
        if (rval < 0) {
            AmbaShell_Print(env, "drive %c volume is not ready!\n",
                            argv[1][0]);
            return -2;
        }

        memset(name, 0x0, sizeof(name));
        strncpy(name, ff_voltab.Name, 11);

        AmbaShell_Print(env, "volume: %s\n", name);
        AmbaShell_Print(env, "total clusters: %ld\n", ff_devinf.Cls);
        AmbaShell_Print(env, "empty clusters: %ld\n", ff_devinf.Ucl);
        AmbaShell_Print(env, "bytes per sector: %d\n", ff_devinf.Bps);
        AmbaShell_Print(env, "sectors per cluster: %d\n", ff_devinf.Spc);
        AmbaShell_Print(env, "clusters per group: %d\n", ff_devinf.Cpg);
        AmbaShell_Print(env, "empty cluster groups: %d\n", ff_devinf.Ucg);
        AmbaShell_Print(env, "total space: %lld KB\n",
                        (((UINT64)(ff_devinf.Cls * ff_devinf.Spc)) *
                         ff_devinf.Bps) >> 10);
        AmbaShell_Print(env, "used space: %lld KB\n",
                        (((UINT64)((ff_devinf.Cls - ff_devinf.Ucl) *
                                   ff_devinf.Spc)) * ff_devinf.Bps) >> 10);
        AmbaShell_Print(env, "free space: %lld KB\n",
                        (((UINT64)(ff_devinf.Ucl * ff_devinf.Spc)) *
                         ff_devinf.Bps) >> 10);
        if (ff_devinf.Fmt == AMBA_FS_FMT_EXFAT) {
            long f_entries = 0;
//            rval = pf_dgetempent(argv[1][0], &f_entries);
            if (rval < 0) {
                AmbaShell_Print(env,
                                "get free_directory_entrie faied\n");
                return -2;
            }
            AmbaShell_Print(env,
                            "free directory entries: %d\n", f_entries);
        }
    } else {
        memset(name, 0x0, sizeof(name));
        strncpy(name, argv[2], sizeof(name));

        rval = AmbaFS_SetVol(argv[1][0], name);

        if (rval < 0) {
            AmbaShell_Print(env,
                            "set volume on %c drive failed: %s!\n",
                            argv[1][0],
                            AmbaShell_GetFsError());
            return -3;
        }

        AmbaShell_Print(env, "volume on %c drive changed\n", argv[1][0]);
    }

    return 0;
}

