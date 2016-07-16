/**
 * system/src/ui/ambsh/rm.c
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
    AmbaShell_Print(pEnv , "Usage: %s [file]\n", Argv[0]);
}

int ambsh_rm(AMBA_SHELL_ENV_s *pEnv , int Argc, char **Argv)
{
    int Rval;
    AMBA_FS_DTA ff_dta;
    int p;
    char *file, *fname;
    char tmp[AMBA_FS_NAME_LEN];
    char Buf[AMBA_FS_NAME_LEN];
    int count = 0;

    if (Argc != 2) {
        usage(pEnv , Argc, Argv);
        return -1;
    }

    AMBSH_CHKCWD();

    file = Argv[Argc - 1];
    p = AmbaUtility_FindRepeatSep(file);

    AmbaShell_Ascii2Unicode(file, Buf);

    for (Rval = AmbaFS_FirstDirEnt(Buf, AMBA_FS_ATTR_ALL, &ff_dta); Rval >= 0;
         Rval = AmbaFS_NextDirEnt(&ff_dta)) {

        if (ff_dta.Attribute & AMBA_FS_ATTR_VOLUME)
            continue;
        if (ff_dta.Attribute & AMBA_FS_ATTR_DIR)
            continue;

        fname = (ff_dta.LongName[0] != '\0') ? (char *)ff_dta.LongName :
                (char *)ff_dta.FileName;

        AmbaShell_Unicode2Ascii(fname, Buf);
        strcpy(fname, Buf);

        if (p) {
            strncpy(tmp, file, p);
            tmp[p] = '\0';
            strcat(tmp, (const char *) fname);
        } else {
            strcpy(tmp, (const char *) fname);
        }

        AmbaShell_Ascii2Unicode(tmp, Buf);

        Rval = AmbaFS_remove(Buf);
        if (Rval < 0) {
            AmbaShell_Print(pEnv , "%s: '%s': %s!\n",
                            Argv[0],
                            tmp,
                            AmbaShell_GetFsError());
            return -2;
        }

        count++;
    }

    if (count == 0) {
        AmbaShell_Print(pEnv , "%s: %s not found!\n", Argv[0], Argv[1]);
        return -3;
    }

    return 0;
}

