/**
 * system/src/ui/ambsh/savebin.c
 *
 * History:
 *    2006/02/06 - [Chien Yang Chen] created file
 *
 * Copyright (C) 2004-2006, Ambarella, Inc.
 *
 * All rights reserved. No Part of this file may be reproduced, stored
 * in a retrieval system, or transmitted, in any form, or by any means,
 * electronic, mechanical, photocopying, recording, or otherwise,
 * without the prior consent of Ambarella, Inc.
 */

#include    <stdio.h>
#include    <ctype.h>
#include    <string.h>

#include    "AmbaDataType.h"
#include    "AmbaKAL.h"
#include    "AmbaShell.h"

#include    "AmbaFS.h"
#include    "AmbaCardManager.h"
#include    "AmbaUtility.h"

static inline void Ambashell_SaveBinUsage(AMBA_SHELL_ENV_s *pEnv, int argc, char **argv)
{
    AmbaShell_Print(pEnv, "Usage: %s [filename] [from] to/l [to/len]\n", argv[0]);
}

int ambsh_savebin(AMBA_SHELL_ENV_s *pEnv, int argc, char **argv)
{
    int Slot;
    int Rval;
    UINT32 from, len;
    char FileName[256];
    AMBA_FS_FILE *pFile = NULL;
    AMBA_SCM_STATUS_s Status;

    if (argc != 5) {
        Ambashell_SaveBinUsage(pEnv, argc, argv);
        return -1;
    }

    if (argv[1][1] != ':') {
        AmbaShell_Print(pEnv, "no drive assigned\n");
        return -2;
    }

    Slot = tolower((int) argv[1][0]) - 'a';

    Rval = AmbaSCM_GetSlotStatus(Slot, &Status);
    if (Rval < 0 || !Status.CardPresent) {
        AmbaShell_Print(pEnv, "media unavailable!\n");
        return -3;
    }

    strncpy(FileName, argv[1], sizeof(FileName));
    AmbaUtility_StringToUINT32(argv[2], &from);

    if (strcmp(argv[3], "to") == 0) {
        AmbaUtility_StringToUINT32(argv[4], &len);
        len = len - from + 1;
    } else if (strcmp(argv[3], "l") == 0) {
        AmbaUtility_StringToUINT32(argv[4], &len);
    } else {
        AmbaShell_Print(pEnv, "unknown syntax!\n");
        return -4;
    }

    pFile = AmbaFS_fopen(FileName, "w");
    if (pFile == NULL) {
        AmbaFS_GetError(&Rval);
        AmbaShell_Print(pEnv, "%s: '%s' err:%d!\n", argv[0], FileName, Rval);
        return -5;
    }

    Rval = AmbaFS_fwrite((void *) from, 1, len, pFile);
    if (Rval != len) {
        AmbaFS_GetError(&Rval);
        AmbaShell_Print(pEnv, "%s: '%s' err:%d!\n", argv[0], FileName, Rval);
        return -5;
    }

    Rval = AmbaFS_fclose(pFile);
    if (Rval < 0) {
        AmbaFS_GetError(&Rval);
        AmbaShell_Print(pEnv, "%s: '%s' err:%d!\n", argv[0], FileName, Rval);
        return -5;
    }

    return 0;
}

