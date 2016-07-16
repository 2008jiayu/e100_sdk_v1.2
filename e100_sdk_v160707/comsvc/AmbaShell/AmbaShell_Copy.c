/**
 * system/src/ui/ambsh/cp.c
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

#define AMBSH_CP_BUF_SIZE   (128 << 10)

static inline void usage(AMBA_SHELL_ENV_s *pEnv, int argc, char **argv)
{
    AmbaShell_Print(pEnv, "Usage: %s [from] [to]\n", argv[0]);
}

int ambsh_cp(AMBA_SHELL_ENV_s *pEnv, int argc, char **argv)
{
    int Rval;
    AMBA_FS_DTA ff_dta;
    char tmp[AMBA_FS_NAME_LEN];
    char tmp2[AMBA_FS_NAME_LEN];
    char *src = NULL;
    char *dst = NULL;
    char *buf = NULL;
    int p, p2, w;
    AMBA_FS_FILE *fin = NULL;
    AMBA_FS_FILE *fout = NULL;
    const char *fname;
    AMBA_MEM_CTRL_s MemCtrl;

    if (argc != 3) {
        usage(pEnv, argc, argv);
        Rval = -1;
        goto done;
    }

    if (AmbaKAL_MemAllocate(AmbaShell_GetHeap(), &MemCtrl, AMBSH_CP_BUF_SIZE, 32) == OK) {
        buf = MemCtrl.pMemAlignedBase;
    } else {
        AmbaShell_Print(pEnv, "%s: out of memory!\n", argv[0]);
        Rval = -2;
        goto done;
    }

    AMBSH_CHKCWD();

    src = argv[1];
    p = AmbaUtility_FindRepeatSep(src);
    w = AmbaUtility_FindWild(src);

    dst = argv[2];
    if (strlen(dst) > sizeof(tmp)) {
        AmbaShell_Print(pEnv, "%s: string is too long\n", argv[2]);
        Rval = -3;
        goto done;
    }

    p2 = AmbaUtility_FindRepeatSep(dst);

    for (Rval = AmbaFS_FirstDirEnt(src, AMBA_FS_ATTR_ALL, &ff_dta); Rval >= 0;
         Rval = AmbaFS_NextDirEnt(&ff_dta)) {

        if (ff_dta.Attribute & AMBA_FS_ATTR_VOLUME)
            continue;
        if (ff_dta.Attribute & AMBA_FS_ATTR_DIR)
            continue;

        fname = (ff_dta.LongName[0] != '\0') ? (char *)ff_dta.LongName :
                (char *)ff_dta.FileName;

        if (p) {
            strncpy(tmp, src, p);
            tmp[p] = '\0';
            strcat(tmp, (const char *) fname);
        } else {
            strcpy(tmp, (const char *) fname);
        }

        if (w >= 0) {
            if (p2) {
                strncpy(tmp2, dst, p2);
                tmp2[p2] = '\0';
                strcat(tmp2, (const char *) fname);
            } else {
                strcpy(tmp2, dst);
                strcat(tmp2, "\\");
                strcat(tmp2, (const char *) fname);
            }
        } else {
            if (strcmp(dst, ".") == 0 ||
                strcmp(dst, "..") == 0) {
                strcpy(tmp2, dst);
                strcat(tmp2, "\\");
                strcat(tmp2, (const char *) fname);
            } else {
                strcpy(tmp2, dst);
            }
        }

        fin = AmbaFS_fopen(tmp, "r");
        if (fin == NULL) {
            AmbaShell_Print(pEnv, "%s: '%s' fail!\n",
                            argv[0],
                            tmp);
            Rval = -3;
            goto done;
        }

        fout = AmbaFS_fopen(tmp2, "w");
        if (fout == NULL) {
            AmbaShell_Print(pEnv, "%s: '%s' fail!\n",
                            argv[0],
                            tmp2);
            Rval = -4;
            goto done;
        }

        /* Loop for: read from src, and write to dst */
        do {
            Rval = AmbaFS_fread(buf, 1, AMBSH_CP_BUF_SIZE, fin);
            if (Rval > 0) {
                Rval = AmbaFS_fwrite(buf, 1, Rval, fout);
                if (Rval <= 0) {
                    AmbaShell_Print(pEnv,
                                    "%s: I/O error\n",
                                    argv[0]);
                    Rval = -5;
                    goto done;
                }
            }
        } while (Rval > 0);

        if (fin)
            AmbaFS_fclose(fin);
        if (fout)
            AmbaFS_fclose(fout);
        if (w < 0)
            break;
    }
done:
    if (buf) {
        AmbaKAL_BytePoolFree(&MemCtrl);
    }

    if (fin)
        AmbaFS_fclose(fin);
    if (fout)
        AmbaFS_fclose(fout);

    return 0;
}

