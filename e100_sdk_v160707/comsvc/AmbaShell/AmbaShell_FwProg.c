/**
 * system/src/ui/ambsh/firmfl.c
 *
 * History:
 *    2005/11/10 - [Charles Chiou] created file
 *    2007/08/02 - [Charles Chiou] cleaned up and simplified this utility
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

#include    "AmbaRTSL_NAND.h"
#include    "AmbaRTSL_NOR.h"
#include    "AmbaNAND.h"
#include    "AmbaNOR.h"
#include    "AmbaNFTL.h"
#include    "AmbaNAND_PartitionTable.h"
#include    "AmbaPrint.h"
#include    "AmbaSysCtrl.h"
#include    "AmbaEMMC_Def.h"

int AmbaFwUpdaterSD_SetMagicCode(void);
UINT32 AmbaSysGetBootDevice(void);
extern int AmbaEMMC_MediaPartRead(int ID, UINT8 *pBuf, UINT32 Sec, UINT32 Secs);
extern int AmbaEmmcFwUpdaterSD_SetMagicCode(void);

static int BackUpMediaPartition(void)
{
    char *MediaPartString[] = {"Storage0", "Storage1",  "IndexForVideoRecording", "UserSetting", "CalibrationData"};
    int Id, Sec;
    char TmpFileName[32], Drive;
    AMBA_NFTL_STATUS_s Status;
    AMBA_FS_FILE *pFile;
    UINT8 Buf[512];

    Drive = 'c';

    for (Id = 0; Id < MP_MAX; Id ++) {

        if ((Id == MP_Storage0) || (Id == MP_Storage1) || (Id == MP_IndexForVideoRecording))
            continue;

        if (AmbaNFTL_GetStatus(Id, &Status) < 0) {
            AmbaPrint("AmbaNFTL_GetStatus fail...");
            return -1;
        }

        sprintf(TmpFileName, "%c:\\%s.bin", Drive, MediaPartString[Id]);

        pFile = AmbaFS_fopen(TmpFileName, "w");
        if (pFile == NULL) {
            AmbaPrint("AmbaFS_fopen fail %s ...", TmpFileName);
            return -1;
        }

        for (Sec = 0; Sec < Status.TotalSecs; Sec ++) {
            if (AmbaNFTL_Read(Id, Buf, Sec, 1) < 0) {
                AmbaPrint("AmbaNFTL_Read %d fail ...", Id);
                return -1;
            }
            if (AmbaFS_fwrite(Buf, 1, 512, pFile) != 512) {
                return -1;
            }
        }
        AmbaFS_fclose(pFile);
    }

    return 0;
}

static int BackUpMediaPartitionEmmc(void)
{
    char *MediaPartString[] = {"Storage0", "Storage1",  "IndexForVideoRecording", "UserSetting", "CalibrationData"};
    int Id, Sec;
    char TmpFileName[32], Drive;
    AMBA_FS_FILE *pFile;
    UINT8 Buf[512];
    extern AMBA_EMMC_MEDIA_PART_s AmbaEMMC_MediaPartInfo;

    Drive = 'c';

    for (Id = 0; Id < MP_MAX; Id ++) {

        if ((Id == MP_Storage0) || (Id == MP_Storage1) || (Id == MP_IndexForVideoRecording))
            continue;

        sprintf(TmpFileName, "%c:\\%s.bin", Drive, MediaPartString[Id]);

        pFile = AmbaFS_fopen(TmpFileName, "w");
        if (pFile == NULL) {
            AmbaPrint("AmbaFS_fopen fail %s ...", TmpFileName);
            return -1;
        }

        for (Sec = 0; Sec < AmbaEMMC_MediaPartInfo.NumBlks[Id]; Sec ++) {
            if (AmbaEMMC_MediaPartRead(Id, Buf, Sec, 1) < 0) {
                AmbaPrint("AmbaEMMC_MediaPartRead %d fail ...", Id);
                return -1;
            }
            if (AmbaFS_fwrite(Buf, 1, 512, pFile) != 512) {
                return -1;
            }
        }
        AmbaFS_fclose(pFile);
    }
    return 0;
}

static int BackUpMediaPartitionNor(void)
{
    char *MediaPartString[] = {"Storage0", "Storage1",  "IndexForVideoRecording", "UserSetting", "CalibrationData"};
    int Id;
    char TmpFileName[32], Drive;
    AMBA_FS_FILE *pFile;
    UINT8 Buf[512];
    UINT32 Sec, TotalSec;

    Drive = 'c';

    for (Id = 0; Id < MP_MAX; Id ++) {

        if ((Id == MP_Storage0) || (Id == MP_Storage1) || (Id == MP_IndexForVideoRecording))
            continue;

        sprintf(TmpFileName, "%c:\\%s.bin", Drive, MediaPartString[Id]);

        pFile = AmbaFS_fopen(TmpFileName, "w");
        if (pFile == NULL) {
            AmbaPrint("AmbaFS_fopen fail %s ...", TmpFileName);
            return -1;
        }
        TotalSec = AmbaNOR_MediaPartGetTotalSector(Id);
        AmbaPrint("AmbaNOR_MediaPart ID :%d Total sec is %d", Id, TotalSec);

        for (Sec = 0; Sec < TotalSec; Sec ++) {
            if (AmbaNOR_MediaPartRead(Id, Buf, Sec, 1) < 0) {
                AmbaPrint("AmbaNOR_MediaPartRead %d fail ...", Id);
                return -1;
            }
            if (AmbaFS_fwrite(Buf, 1, 512, pFile) != 512) {
                return -1;
            }
        }
        AmbaFS_fclose(pFile);
    }
    return 0;
}

static int firmfl_prog(AMBA_SHELL_ENV_s *pEnv, int argc, char **argv)
{
    int Rval;

    AMBSH_CHKCWD();

    /* Turn off VT100 insertion mode */
    AmbaShell_Print(pEnv, "\x1b[4l");

    /* Program firmware with a file */
    if (AmbaSysGetBootDevice() == BOOT_FROM_EMMC) {
        BackUpMediaPartitionEmmc();
        Rval = AmbaEmmcFwUpdaterSD_SetMagicCode();
    } else if (AmbaSysGetBootDevice() == BOOT_FROM_NOR) {
        BackUpMediaPartitionNor();
        Rval = AmbaNorFwUpdaterSD_SetMagicCode();
    } else {
        BackUpMediaPartition();
        Rval = AmbaFwUpdaterSD_SetMagicCode();
    }

    /* Turn on VT100 insertion mode */
    AmbaShell_Print(pEnv, "\x1b[4h");

    if (Rval < 0)
        return -1;

    AmbaSysSoftReset();

    return 0;
}

static int firmfl_prepro(AMBA_SHELL_ENV_s *pEnv, int argc, char **argv)
{
    int Rval;

    AMBSH_CHKCWD();

    /* Turn off VT100 insertion mode */
    AmbaShell_Print(pEnv, "\x1b[4l");

    Rval = BackUpMediaPartition();

    /* Turn on VT100 insertion mode */
    AmbaShell_Print(pEnv, "\x1b[4h");

    if (Rval < 0)
        return -1;

    return 0;
}

struct firmfl_list {
    char *name;
    int (*func)(AMBA_SHELL_ENV_s *, int, char **);
};

struct firmfl_list firmfl_list[] = {
    { "prog",       firmfl_prog },
    { "prepro",     firmfl_prepro },
    { NULL, NULL }
};

static void usage(AMBA_SHELL_ENV_s *pEnv, int argc, char **argv)
{
    int i;

    AmbaShell_Print(pEnv, "valid targets for firmfl are:\n");

    for (i = 0; ; i++) {
        if (firmfl_list[i].name == NULL ||
            firmfl_list[i].func == NULL)
            break;
        AmbaShell_Print(pEnv, "\t%s\n", firmfl_list[i].name);
    }
}

int ambsh_firmfl(AMBA_SHELL_ENV_s *pEnv, int argc, char **argv)
{
    int i;

    if (argc < 2) {
        usage(pEnv, argc, argv);
        return -1;
    }

    for (i = 0; ; i++) {
        if (firmfl_list[i].name == NULL ||
            firmfl_list[i].func == NULL) {
            break;
        }

        if (strcmp(firmfl_list[i].name, argv[1]) == 0)
            return firmfl_list[i].func(pEnv, argc - 1, &argv[1]);
    }

    usage(pEnv, argc, argv);

    return -2;
}
