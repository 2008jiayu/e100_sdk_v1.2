/**
 * system/src/ui/ambsh/nftl.c
 *
 * Test "program" for nftl.
 *
 * History:
 *    2007/04/03 - [Chien-Yang Chen] created file
 *
 * Copyright (C) 2004-2008, Ambarella, Inc.
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
#include    "AmbaNAND.h"
#include    "AmbaNFTL.h"
#include    "AmbaPrintk.h"
#include    "AmbaCardManager.h"
#include    "AmbaNandBadBlockTable.h"
#include     "AmbaSysTimer.h"

extern int AmbaUtility_StringToUINT32(const char *str, UINT32 *value);
extern AMBA_NFTL_OBJ_s *NftlGetPartObj(int id);

static char g_str1[] = {"stg|stg2|raw|prf|cal"};

static char *g_nftl_str[] = {"nftl_stg", "nftl_stg2", "nftl_idx", "nftl_prf", "nftl_cal"};

static void tnrs_usage(AMBA_SHELL_ENV_s *env, int argc, char **argv)
{
    AmbaShell_Print(env,
                    "Usage: %s [rand|inc|dec|dump] [sector] [sectors] [%s]\n",
                    argv[0], g_str1);
}

/**
 * Read (by-passing the file system) directly from sector(s) on
 * the media.
 */
int ambsh_nftl_read_sector(AMBA_SHELL_ENV_s *env, int argc, char **argv)
{
    int Rval = 0, er = 0;
    UINT32 sector, sectors;
    UINT32 id;
    int size = 512;
    UINT8 *buf = NULL;
    int i;
    AMBA_MEM_CTRL_s MemCtrl;

    if (argc != 5) {
        tnrs_usage(env, argc, argv);
        Rval = -1;
        goto done;
    }

    if (!(strcmp(argv[1], "rand") == 0 ||
          strcmp(argv[1], "inc")  == 0 ||
          strcmp(argv[1], "dec")  == 0 ||
          strcmp(argv[1], "dump")  == 0)) {
        tnrs_usage(env, argc, argv);
        Rval = -1;
        goto done;
    }

    AmbaUtility_StringToUINT32(argv[2], &sector);
    AmbaUtility_StringToUINT32(argv[3], &sectors);

    if (strcmp(argv[4], "prf") == 0) {
        id = NFTL_ID_PRF;
    } else if (strcmp(argv[4], "cal") == 0) {
        id = NFTL_ID_CAL;
    } else if (strcmp(argv[4], "raw") == 0) {
        id = NFTL_ID_IDX;
    } else if (strcmp(argv[4], "stg") == 0) {
        id = NFTL_ID_STORAGE;
    } else if (strcmp(argv[4], "stg2") == 0) {
        id = NFTL_ID_STORAGE2;
    } else {
        tnrs_usage(env, argc, argv);
        Rval = -1;
        goto done;
    }

    if (sectors == 0) {
        AmbaShell_Print(env, "invalid argument!\n");
        Rval = -1;
        goto done;
    }

    Rval = AmbaKAL_MemAllocate(AmbaShell_GetHeap(), &MemCtrl, size * sectors, 1);
    if (Rval != OK) {
        AmbaShell_Print(env, "out of memory!\n");
        Rval = -2;
        goto done;
    }

    buf = MemCtrl.pMemAlignedBase;

    AmbaShell_Print(env, "read sector %d with %d sectors... \n",
                    sector, sectors);

    Rval = AmbaNFTL_Read(id, buf, sector, sectors);
    if (Rval < 0) {
        AmbaShell_Print(env, "read error!\n");
        Rval = -3;
        goto done;
    }

    if (strcmp(argv[1], "inc") == 0) {
        for (i = 0; i < (size * sectors); i++) {
            if (buf[i] != (i & 0xff)) {
                AmbaShell_Print(env, "sector %d byte %d is 0x%x"
                                " but should be 0x%x\n",
                                i / size, i % size, buf[i],
                                i & 0xff);
                er = -1;
            }

            if (er < 0) {
                Rval = -1;
                break;
            }
        }
    } else if (strcmp(argv[1], "dec") == 0) {
        for (i = 0; i < (size * sectors); i++) {
            if (buf[i] != (0xff - (i & 0xff))) {
                AmbaShell_Print(env, "sector %d byte %d is 0x%x"
                                " but should be 0x%x\n",
                                i / size, i % size, buf[i],
                                0xff - (i & 0xff));
                er = -1;
            }

            if (er < 0) {
                Rval = -1;
                break;
            }
        }
    } else if (strcmp(argv[1], "dump") == 0) {
        AmbaShell_Print(env, "==================");
        for (i = 0; i < (size * sectors); i++) {
            if ((i % 16) == 0) {
                AmbaShell_Print(env, "\n");
            }
            AmbaShell_Print(env, "%2x ", buf[i]);
        }
        AmbaShell_Print(env, "\n==================\n");
    }

    if (Rval < 0)
        AmbaShell_Print(env, "failed!\n");
    else
        AmbaShell_Print(env, "success\n");

done:
    if (buf)
        AmbaKAL_MemFree(&MemCtrl);

    return Rval;
}

static void tnws_usage(AMBA_SHELL_ENV_s *env, int argc, char **argv)
{
    AmbaShell_Print(env,
                    "Usage: %s [rand|inc|dec|zero] [sector] [sectors] [%s]\n",
                    argv[0], g_str1);
}

/**
 * Write patterns (by-passing the file system) directly to sector(s) on
 * the media.
 */
int ambsh_nftl_write_sector(AMBA_SHELL_ENV_s *env, int argc, char **argv)
{
    int Rval = 0;
    UINT32 sector, sectors;
    UINT32 id;
    int size = 512;
    UINT8 *buf = NULL;
    int i;
    AMBA_MEM_CTRL_s MemCtrl;

    if (argc != 5) {
        tnws_usage(env, argc, argv);
        Rval = -1;
        goto done;
    }

    if (!(strcmp(argv[1], "rand") == 0 ||
          strcmp(argv[1], "inc")  == 0 ||
          strcmp(argv[1], "dec")  == 0 ||
          strcmp(argv[1], "zero")  == 0)) {
        tnws_usage(env, argc, argv);
        Rval = -1;
        goto done;
    }

    AmbaUtility_StringToUINT32(argv[2], &sector);
    AmbaUtility_StringToUINT32(argv[3], &sectors);

    if (strcmp(argv[4], "prf") == 0) {
        id = NFTL_ID_PRF;
    } else if (strcmp(argv[4], "cal") == 0) {
        id = NFTL_ID_CAL;
    } else if (strcmp(argv[4], "raw") == 0) {
        id = NFTL_ID_IDX;
    } else if (strcmp(argv[4], "stg") == 0) {
        id = NFTL_ID_STORAGE;
    } else if (strcmp(argv[4], "stg2") == 0) {
        id = NFTL_ID_STORAGE2;
    } else {
        tnws_usage(env, argc, argv);
        Rval = -1;
        goto done;
    }

    if (sectors == 0) {
        AmbaShell_Print(env, "invalid argument!\n");
        Rval = -1;
        goto done;
    }

    Rval = AmbaKAL_MemAllocate(AmbaShell_GetHeap(), &MemCtrl, size * sectors, 32);
    if (Rval != OK) {
        AmbaShell_Print(env, "out of memory!\n");
        Rval = -2;
        goto done;
    }

    buf = MemCtrl.pMemAlignedBase;

    if (strcmp(argv[1], "rand") == 0) {
        for (i = 0; i < (size * sectors); i++)
            buf[i] = rand();
    } else if (strcmp(argv[1], "inc") == 0) {
        for (i = 0; i < (size * sectors); i++)
            buf[i] = (i & 0xff);
    } else if (strcmp(argv[1], "dec") == 0) {
        for (i = 0; i < (size * sectors); i++)
            buf[i] = (0xff - (i & 0xff));
    } else if (strcmp(argv[1], "zero") == 0) {
        memset(buf, 0x0, size * sectors);
    } else {
        tnws_usage(env, argc, argv);
        Rval = -3;
        goto done;
    }

    AmbaShell_Print(env, "write sector %d with %d sectors... \n", sector, sectors);

    Rval = AmbaNFTL_Write(id, buf, sector, sectors);

    if (Rval < 0)
        AmbaShell_Print(env, "failed!\n");
    else
        AmbaShell_Print(env, "success\n");

done:
    if (buf)
        AmbaKAL_MemFree(&MemCtrl);

    return Rval;
}

static void tnvs_usage(AMBA_SHELL_ENV_s *env, int argc, char **argv)
{
    AmbaShell_Print(env,
                    "Usage: %s [rand|inc|dec|zero] [sector] [sectors] [%s]\n",
                    argv[0], g_str1);
}

/**
 * Verify patterns (by-passing the file system) directly to sector(s) on
 * the media.
 */
int ambsh_nftl_verify_sector(AMBA_SHELL_ENV_s *env, int argc, char **argv)
{
    int Rval = 0;
    UINT32 sector, sectors;
    UINT32 id;
    int size = 512;
    UINT8 *wrbuf = NULL;
    UINT8 *rdbuf = NULL;
    int i, j;
    AMBA_MEM_CTRL_s MemCtrlW, MemCtrlR;

    if (argc != 5) {
        tnvs_usage(env, argc, argv);
        Rval = -1;
        goto done;
    }

    if (!(strcmp(argv[1], "rand") == 0 ||
          strcmp(argv[1], "inc")  == 0 ||
          strcmp(argv[1], "dec")  == 0 ||
          strcmp(argv[1], "zero")  == 0)) {
        tnvs_usage(env, argc, argv);
        Rval = -1;
        goto done;
    }

    AmbaUtility_StringToUINT32(argv[2], &sector);
    AmbaUtility_StringToUINT32(argv[3], &sectors);

    if (strcmp(argv[4], "prf") == 0) {
        id = NFTL_ID_PRF;
    } else if (strcmp(argv[4], "cal") == 0) {
        id = NFTL_ID_CAL;
    } else if (strcmp(argv[4], "raw") == 0) {
        id = NFTL_ID_IDX;
    } else if (strcmp(argv[4], "stg") == 0) {
        id = NFTL_ID_STORAGE;
    } else if (strcmp(argv[4], "stg2") == 0) {
        id = NFTL_ID_STORAGE2;
    } else {
        tnvs_usage(env, argc, argv);
        Rval = -1;
        goto done;
    }

    if (sectors == 0) {
        AmbaShell_Print(env, "invalid argument!\n");
        Rval = -1;
        goto done;
    }

    Rval = AmbaKAL_MemAllocate(AmbaShell_GetHeap(), &MemCtrlW, size, 32);
    if (Rval != OK) {
        AmbaShell_Print(env, "out of memory!\n");
        Rval = -2;
        goto done;
    }

    wrbuf = MemCtrlW.pMemAlignedBase;


    Rval = AmbaKAL_MemAllocate(AmbaShell_GetHeap(), &MemCtrlR, size, 32);
    if (Rval != OK) {
        AmbaShell_Print(env, "out of memory!\n");
        Rval = -2;
        goto done;
    }

    rdbuf = MemCtrlR.pMemAlignedBase;

    if (strcmp(argv[1], "rand") == 0) {
        for (i = 0; i < size; i++)
            wrbuf[i] = rand();
    } else if (strcmp(argv[1], "inc") == 0) {
        for (i = 0; i < size; i++)
            wrbuf[i] = (i & 0xff);
    } else if (strcmp(argv[1], "dec") == 0) {
        for (i = 0; i < size; i++)
            wrbuf[i] = (0xff - (i & 0xff));
    } else if (strcmp(argv[1], "zero") == 0) {
        memset(wrbuf, 0x0, size);
    } else {
        tnvs_usage(env, argc, argv);
        Rval = -3;
        goto done;
    }

    AmbaShell_Print(env, "verify sector %d with %d sectors... \n", sector, sectors);

    /* Turn off VT100 insertion mode */
    AmbaShell_Print(env, "\x1b[4l");

    for (i = 0; i < sectors; i++) {
        Rval = AmbaNFTL_Write(id, wrbuf, sector, 1);
        if (Rval < 0) {
            AmbaShell_Print(env, "write sector %d error!\n", sector);
            Rval = -3;
            break;
        }

        Rval = AmbaNFTL_Read(id, rdbuf, sector, 1);
        if (Rval < 0) {
            AmbaShell_Print(env, "read sector %d error!\n", sector);
            Rval = -3;
            break;
        }

        Rval = memcmp(wrbuf, rdbuf, size);
        if (Rval != 0) {
            AmbaShell_Print(env, "verify sector %d error!\n", sector);
            Rval = -3;
            break;
        }

        sector++;

        AmbaShell_Print(env, ".");

        if ((i & 0xf) == 0xf) {
            AmbaShell_Print(env, " ");
            AmbaShell_Print(env, "%d", i);
            AmbaShell_Print(env, "/");
            AmbaShell_Print(env, "%d", sectors);
            AmbaShell_Print(env, " (");
            AmbaShell_Print(env, "%d", i * 100 / sectors);
            AmbaShell_Print(env, "%%)\t\t\r");
        }

        if (strcmp(argv[1], "rand") == 0) {
            for (j = 0; j < size; j++)
                wrbuf[j] = rand();
        }
    }

    /* Turn on VT100 insertion mode */
    AmbaShell_Print(env, "\x1b[4h");

    AmbaShell_Print(env, "\n");

    if (Rval < 0)
        AmbaShell_Print(env, "failed!\n");
    else
        AmbaShell_Print(env, "success\n");

done:

    if (wrbuf)
        AmbaKAL_MemFree(&MemCtrlW);

    if (rdbuf)
        AmbaKAL_MemFree(&MemCtrlR);

    return Rval;
}

static void tnpr_usage(AMBA_SHELL_ENV_s *env, int argc, char **argv)
{
    AmbaShell_Print(env,
                    "Usage: %s [%s] [m|me|s] [rand|inc|dec|dump] [block] [page] [pages]\n",
                    argv[0], g_str1);
}

/**
 * Read (by-passing the nftl) directly from page(s) on
 * the media in the nftl partition.
 */
int ambsh_nftl_pread(AMBA_SHELL_ENV_s *env, int argc, char **argv)
{
    int Rval = 0, er = 0;
    UINT32 area, block, page, pages;
    AMBA_NAND_DEV_s *ndev;
    int size;
    UINT8 *buf = NULL;
    int id, i;
    AMBA_NFTL_STATUS_s status;
    AMBA_MEM_CTRL_s MemCtrl;

    ndev = AmbaRTSL_NandGetDev();

    K_ASSERT(ndev != NULL);

    if (argc != 7) {
        tnpr_usage(env, argc, argv);
        Rval = -1;
        goto done;
    }

    if (strcmp(argv[1], "prf") == 0) {
        id = NFTL_ID_PRF;
    } else if (strcmp(argv[1], "cal") == 0) {
        id = NFTL_ID_CAL;
    } else if (strcmp(argv[1], "raw") == 0) {
        id = NFTL_ID_IDX;
    } else if (strcmp(argv[1], "stg") == 0) {
        id = NFTL_ID_STORAGE;
    } else if (strcmp(argv[1], "stg2") == 0) {
        id = NFTL_ID_STORAGE2;
    } else {
        tnpr_usage(env, argc, argv);
        Rval = -1;
        goto done;
    }

    if (strcmp(argv[2], "m") == 0) {
        area = NAND_MAIN_ONLY;
        size = ndev->DevLogicInfo.MainSize;
    } else if (strcmp(argv[2], "me") == 0) {
        area = NAND_MAIN_ECC;
        size = ndev->DevLogicInfo.MainSize;
    } else if (strcmp(argv[2], "s") == 0) {
        area = NAND_SPARE_ONLY;
        size = ndev->DevLogicInfo.SpareSize;
    } else {
        tnpr_usage(env, argc, argv);
        Rval = -1;
        goto done;
    }

    if (!(strcmp(argv[3], "rand") == 0 ||
          strcmp(argv[3], "inc")  == 0 ||
          strcmp(argv[3], "dec")  == 0 ||
          strcmp(argv[3], "dump")  == 0)) {
        tnpr_usage(env, argc, argv);
        Rval = -1;
        goto done;
    }

    AmbaUtility_StringToUINT32(argv[4], &block);
    AmbaUtility_StringToUINT32(argv[5], &page);
    AmbaUtility_StringToUINT32(argv[6], &pages);

    if (pages == 0) {
        AmbaShell_Print(env, "invalid argument!\n");
        Rval = -1;
        goto done;
    }

    Rval = AmbaKAL_MemAllocate(AmbaShell_GetHeap(), &MemCtrl, size, 32);
    if (Rval != OK) {
        AmbaShell_Print(env, "out of memory!\n");
        Rval = -2;
        goto done;
    }

    buf = MemCtrl.pMemAlignedBase;

    AmbaNFTL_GetStatus(id, &status);

    AmbaShell_Print(env, "read block %d page %d with %d pages... \n", block, page, pages);

    Rval = AmbaNAND_Read(status.StartFtlBlk + block, page, pages, buf, buf, area, 5000);
    if (Rval < 0) {
        AmbaShell_Print(env, "read error!\n");
        Rval = -3;
        goto done;
    }

    if (strcmp(argv[3], "inc") == 0) {
        for (i = 0; i < (size * pages); i++) {
            if (buf[i] != (i & 0xff)) {
                AmbaShell_Print(env, "page %d byte %d is 0x%x"
                                " but should be 0x%x\n",
                                i / size, i % size, buf[i],
                                i & 0xff);
                er = -1;
            }

            if (er < 0) {
                Rval = -1;
                break;
            }
        }
    } else if (strcmp(argv[3], "dec") == 0) {
        for (i = 0; i < (size * pages); i++) {
            if (buf[i] != (0xff - (i & 0xff))) {
                AmbaShell_Print(env, "page %d byte %d is 0x%x"
                                " but should be 0x%x\n",
                                i / size, i % size, buf[i],
                                0xff - (i & 0xff));
                er = -1;
            }

            if (er < 0) {
                Rval = -1;
                break;
            }
        }
    } else if (strcmp(argv[3], "dump") == 0) {
        AmbaShell_Print(env, "==================");
        for (i = 0; i < (size * pages); i++) {
            if ((i % 16) == 0) {
                AmbaShell_Print(env, "\n");
            }
            AmbaShell_Print(env, "%2x ", buf[i]);
        }
        AmbaShell_Print(env, "\n==================\n");
    }

    if (Rval < 0)
        AmbaShell_Print(env, "failed!\n");
    else
        AmbaShell_Print(env, "success\n");

done:
    if (buf)
        AmbaKAL_MemFree(&MemCtrl);

    return Rval;
}

static void tnpw_usage(AMBA_SHELL_ENV_s *env, int argc, char **argv)
{
    AmbaShell_Print(env,
                    "Usage: %s [%s] [m|me|s] [rand|inc|dec|zero] [block] [page] [pages]\n",
                    argv[0], g_str1);
}

/**
 * Write (by-passing the nftl) directly from page(s) on
 * the media in the nftl partition.
 */
int ambsh_nftl_pwrite(AMBA_SHELL_ENV_s *env, int argc, char **argv)
{
    int Rval = 0;
    UINT32 area, block, page, pages;
    AMBA_NAND_DEV_s *ndev;
    int size;
    UINT8 *buf = NULL;
    int i, id;
    AMBA_NFTL_STATUS_s status;
    UINT8 * pMain = NULL,  *pSpare = NULL;
    AMBA_MEM_CTRL_s MemCtrl;

    ndev = AmbaRTSL_NandGetDev();

    K_ASSERT(ndev != NULL);

    if (argc != 7) {
        tnpw_usage(env, argc, argv);
        Rval = -1;
        goto done;
    }

    if (strcmp(argv[1], "prf") == 0) {
        id = NFTL_ID_PRF;
    } else if (strcmp(argv[1], "cal") == 0) {
        id = NFTL_ID_CAL;
    } else if (strcmp(argv[1], "raw") == 0) {
        id = NFTL_ID_IDX;
    } else if (strcmp(argv[1], "stg") == 0) {
        id = NFTL_ID_STORAGE;
    } else if (strcmp(argv[1], "stg2") == 0) {
        id = NFTL_ID_STORAGE2;
    } else {
        tnpw_usage(env, argc, argv);
        Rval = -1;
        goto done;
    }

    if (strcmp(argv[2], "m") == 0) {
        area = NAND_MAIN_ONLY;
        size = ndev->DevLogicInfo.MainSize;
    } else if (strcmp(argv[2], "me") == 0) {
        area = NAND_MAIN_ECC;
        size = ndev->DevLogicInfo.MainSize;
    } else if (strcmp(argv[2], "s") == 0) {
        area = NAND_SPARE_ONLY;
        size = ndev->DevLogicInfo.SpareSize;
    } else {
        tnpw_usage(env, argc, argv);
        Rval = -1;
        goto done;
    }

    if (!(strcmp(argv[3], "rand") == 0 ||
          strcmp(argv[3], "inc")  == 0 ||
          strcmp(argv[3], "dec")  == 0 ||
          strcmp(argv[3], "zero")  == 0)) {
        tnpw_usage(env, argc, argv);
        Rval = -1;
        goto done;
    }

    AmbaUtility_StringToUINT32(argv[4], &block);
    AmbaUtility_StringToUINT32(argv[5], &page);
    AmbaUtility_StringToUINT32(argv[6], &pages);

    if (pages == 0) {
        AmbaShell_Print(env, "invalid argument!\n");
        Rval = -1;
        goto done;
    }

    Rval = AmbaKAL_MemAllocate(AmbaShell_GetHeap(), &MemCtrl, size * pages, 32);
    if (Rval != OK) {
        AmbaShell_Print(env, "out of memory!\n");
        Rval = -2;
        goto done;
    }

    buf = MemCtrl.pMemAlignedBase;

    if (strcmp(argv[3], "rand") == 0) {
        for (i = 0; i < (size * pages); i++)
            buf[i] = rand();
    } else if (strcmp(argv[3], "inc") == 0) {
        for (i = 0; i < (size * pages); i++)
            buf[i] = (i & 0xff);
    } else if (strcmp(argv[3], "dec") == 0) {
        for (i = 0; i < (size * pages); i++)
            buf[i] = (0xff - (i & 0xff));
    } else if (strcmp(argv[3], "zero") == 0) {
        memset(buf, 0x0, size * pages);
    } else {
        tnpw_usage(env, argc, argv);
        Rval = -3;
        goto done;
    }

    AmbaShell_Print(env, "write block %d page %d with %d pages... \n",
                    block, page, pages);

    AmbaNFTL_GetStatus(id, &status);

    if (area == NAND_SPARE_ONLY)
        pSpare = buf;
    else
        pMain = buf;

    Rval = AmbaNAND_Program(status.StartFtlBlk + block, page, pages, pMain, pSpare, area, 5000);
    if (Rval < 0)
        AmbaShell_Print(env, "failed!\n");
    else
        AmbaShell_Print(env, "success\n");

done:
    if (buf)
        AmbaKAL_MemFree(&MemCtrl);

    return Rval;
}

static void tne_usage(AMBA_SHELL_ENV_s *env, int argc, char **argv)
{
//    AmbaShell_Print(env, "Usage: %s [e|rb|vrfy] [%s]\n", argv[0], g_str1);
    AmbaShell_Print(env, "Usage: %s [%s]\n", argv[0], g_str1);
}

int ambsh_nftl_erase(AMBA_SHELL_ENV_s *env, int argc, char **argv)
{
    int Rval;
    int id;

    if (argc != 2) {
        tne_usage(env, argc, argv);
        Rval = -1;
        goto done;
    }

    if (strcmp(argv[1], "prf") == 0) {
        id = NFTL_ID_PRF;
    } else if (strcmp(argv[1], "cal") == 0) {
        id = NFTL_ID_CAL;
    } else if (strcmp(argv[1], "idx") == 0) {
        id = NFTL_ID_IDX;
    } else if (strcmp(argv[1], "stg") == 0) {
        id = NFTL_ID_STORAGE;
    } else if (strcmp(argv[1], "stg2") == 0) {
        id = NFTL_ID_STORAGE2;
    } else {
        tne_usage(env, argc, argv);
        return -1;
    }

    Rval = AmbaNFTL_ErasePart(id);

    if (Rval == 0)
        AmbaShell_Print(env, "erase succeed\r\n");
    else
        AmbaShell_Print(env, "erase failed\r\n");

done:

    return Rval;
}

static void erase_lba_usage(AMBA_SHELL_ENV_s *env, int argc, char **argv)
{
    AmbaShell_Print(env, "Usage: %s [%s] [start_lba] [blocks]\n", argv[0], g_str1);
    AmbaShell_Print(env, "[start_lba]	start logical block\n");
    AmbaShell_Print(env, "[blocks]	number of blocks to be erased\n");
}

int ambsh_nftl_erase_lba(AMBA_SHELL_ENV_s *env, int argc, char **argv)
{
    int Rval;
    UINT32 id, start_blk, blks;

    if (argc != 4) {
        erase_lba_usage(env, argc, argv);
        Rval = -1;
        goto done;
    }

    if (strcmp(argv[1], "prf") == 0) {
        id = NFTL_ID_PRF;
    } else if (strcmp(argv[1], "cal") == 0) {
        id = NFTL_ID_CAL;
    } else if (strcmp(argv[1], "raw") == 0) {
        id = NFTL_ID_IDX;
    } else if (strcmp(argv[1], "stg") == 0) {
        id = NFTL_ID_STORAGE;
    } else if (strcmp(argv[1], "stg2") == 0) {
        id = NFTL_ID_STORAGE2;
    } else {
        erase_lba_usage(env, argc, argv);
        return -1;
    }

    AmbaUtility_StringToUINT32(argv[2], &start_blk);
    AmbaUtility_StringToUINT32(argv[3], &blks);

    AmbaShell_Print(env, "Clean up from logical block %d to %d\r\n",
                    start_blk, start_blk + blks, blks);

    Rval = AmbaNFTL_EraseLogicalBlocks(id, start_blk, blks);

    if (Rval == 0)
        AmbaShell_Print(env, "Erase logical blocks succeed\r\n");
    else
        AmbaShell_Print(env, "Erase logical blocks failed\r\n");

done:

    return Rval;
}

static void tnthru_usage(AMBA_SHELL_ENV_s *env, int argc, char **argv)
{
    AmbaShell_Print(env, "Usage: %s [bs] [areas] [%s]\n", argv[0], g_str1);
}

#define HUGE_BUFFint_ADDR    0xc1000000
#define HUGE_BUFFint_SIZE    0x01000000

#define LOG_SIZE    0x8000
#define LOG_ADDR    (HUGE_BUFFint_ADDR + 0x800000)

struct thruput_log_s {
    UINT32  start;
    float   r_rate;
    float   w_rate;
};

int ambsh_nftl_thruput(AMBA_SHELL_ENV_s *env, int argc, char **argv)
{
    AMBA_NAND_DEV_s *dev;

    UINT32 dtime;
    UINT32 systim_s, systim_e;
    UINT32 n, secs_per_part, bs;
    long sec_num;
    int sec_cnt;
    float sec;
    float rate;
    int i, j;
#if 1
    UINT8 *huge_buf = NULL;
    UINT8 *log_buf = NULL, *log_buf2 = NULL;
    int log_buf_len = LOG_SIZE;
#else
    UINT8 *huge_buf = (UINT8 *) HUGE_BUFFint_ADDR;
    UINT8 *log_buf = (UINT8 *) LOG_ADDR;
#endif
    struct thruput_log_s log;
    UINT32 log_len = 0, len;
    UINT32 huge_secs, id;
    int Rval;
    AMBA_SCM_STATUS_s status;
    AMBA_MEM_CTRL_s MemCtrl, MemCtrlLog;

    if (argc != 4) {
        tnthru_usage(env, argc, argv);
        return -1;
    }

    AmbaUtility_StringToUINT32(argv[1], &bs);
    K_ASSERT(bs >= 512);
    /* sectors per huge buffer */
    huge_secs = bs >> 9;
    K_ASSERT(bs % 512 == 0);

    dev = AmbaRTSL_NandGetDev();
    K_ASSERT(dev != NULL);

    /* number of partitions */
    AmbaUtility_StringToUINT32(argv[2], &n);
    /* sectors per partition */
    secs_per_part = AmbaNFTL_GetTotalSecs(NFTL_ID_STORAGE, &secs_per_part) / n;
    AmbaShell_Print(env, "area size: %d MB\n", secs_per_part >> 11);

    if (strcmp(argv[3], "prf") == 0) {
        id = NFTL_ID_PRF;
    } else if (strcmp(argv[3], "cal") == 0) {
        id = NFTL_ID_CAL;
    } else if (strcmp(argv[3], "raw") == 0) {
        id = NFTL_ID_IDX;
    } else if (strcmp(argv[3], "stg") == 0) {
        id = NFTL_ID_STORAGE;
    } else if (strcmp(argv[3], "stg2") == 0) {
        id = NFTL_ID_STORAGE2;
    } else {
        tnthru_usage(env, argc, argv);
        return -1;
    }

    Rval = AmbaKAL_MemAllocate(AmbaShell_GetHeap(), &MemCtrl, bs, 32);
    if (Rval < 0) {
        AmbaShell_Print(env, "out of memory\n");
        goto done;
    }

    huge_buf = MemCtrl.pMemAlignedBase;

    Rval = AmbaKAL_MemAllocate(AmbaShell_GetHeap(), &MemCtrlLog, log_buf_len, 32);
    if (Rval < 0) {
        AmbaShell_Print(env, "out of memory\n");
        goto done;
    }

    log_buf = MemCtrlLog.pMemAlignedBase;

    log_buf2 = log_buf;

    for (i = 0; i < n; i++) {
        sec_num = i * secs_per_part;

        /**************************/
        /* physical write thruput */
        /**************************/
        systim_s = AmbaSysTimer_GetTickCount();

        for (j = 0; j < secs_per_part; j += huge_secs) {
            if (secs_per_part - j > huge_secs)
                sec_cnt = huge_secs;
            else
                sec_cnt = secs_per_part - j;

            Rval = AmbaNFTL_Write(id, huge_buf, sec_num + j, sec_cnt);
        }

        systim_e = AmbaSysTimer_GetTickCount();

        if (systim_s < systim_e) {
            dtime = systim_e - systim_s;
        } else {
            dtime = 0xffffffff - systim_s + systim_e + 1;
        }

        sec = ((float) dtime) / 1000;
        rate = (secs_per_part >> (11 - 3)) / sec;

        AmbaShell_Print(env, "area %d start from %d MB",
                        i, i * (secs_per_part >> 11));
        AmbaShell_Print(env, "write time elapsed : %f sec\n", sec);
        AmbaShell_Print(env, "write time throughput : %f Mbit/sec\n", rate);

        /* Log write thruput */
        log.w_rate = rate;

        /*************************/
        /* physical read thruput */
        /*************************/

        systim_s = AmbaSysTimer_GetTickCount();

        for (j = 0; j < secs_per_part; j += huge_secs) {
            if (secs_per_part - j > huge_secs)
                sec_cnt = huge_secs;
            else
                sec_cnt = secs_per_part - j;

            Rval = AmbaNFTL_Read(id, huge_buf, sec_num + j, sec_cnt);
        }

        systim_e = AmbaSysTimer_GetTickCount();

        if (systim_s < systim_e) {
            dtime = systim_e - systim_s;
        } else {
            dtime = 0xffffffff - systim_s + systim_e + 1;
        }

        sec = ((float) dtime) / 1000;
        rate = (secs_per_part >> (11 - 3)) / sec;

        AmbaShell_Print(env,
                        "area %d start from %d MB",
                        i, i * (secs_per_part >> 11));
        AmbaShell_Print(env, "read time elapsed : %f sec\n", sec);
        AmbaShell_Print(env, "read time throughput : %f Mbit/sec\n", rate);

        /* Log read thruput */
        log.start = i * (secs_per_part >> 11);
        log.r_rate = rate;

        if ((log_buf_len - 100) >= log_len) {
            sprintf((char *) log_buf,
                    "%ld,%f,%f\n",
                    (long)log.start, log.r_rate, log.w_rate);

            len = strlen((const char *)log_buf);
            log_buf += len;
            log_len += len;
        }
    }

    AmbaSCM_GetSlotStatus(SCM_SLOT_SD0, &status);

    if (status.CardPresent) {
        AMBA_FS_FILE *fp;
        char path[32];

        sprintf(path, "d:\\nftl_log.csv");

        fp = AmbaFS_fopen(path, "w");
        if (fp == NULL) {
            AmbaFS_GetError(&Rval);
            AmbaShell_Print(env, "fopen failed (%d)", Rval);
        }

        Rval = AmbaFS_fwrite((void *) log_buf2, 1, log_len, fp);
        if (Rval !=  log_len) {
            AmbaFS_GetError(&Rval);
            AmbaShell_Print(env, "fwrite failed (%d)", Rval);
        }

        AmbaFS_fclose(fp);
    }

done:
    if (huge_buf)
        AmbaKAL_MemFree(&MemCtrl);

    if (log_buf)
        AmbaKAL_MemFree(&MemCtrlLog);

    return 0;
}

static void tni_usage(AMBA_SHELL_ENV_s *env, int argc, char **argv)
{
    AmbaShell_Print(env, "Usage: %s [%s|all] [mode]\n"
                    "\tmode list:\n"
                    "\t\t0 - without save transition table\n"
                    "\t\t1 - save transitio table\n",
                    argv[0], g_str1);
}

int ambsh_nftl_init(AMBA_SHELL_ENV_s *env, int argc, char **argv)
{
    int Rval = 0;
    UINT32 id, mode;
    int max;

    if (argc != 3) {
        tni_usage(env, argc, argv);
        return -1;
    }

    if (strcmp(argv[1], "prf") == 0) {
        max = id = NFTL_ID_PRF;
    } else if (strcmp(argv[1], "cal") == 0) {
        max = id = NFTL_ID_CAL;
    } else if (strcmp(argv[1], "raw") == 0) {
        max = id = NFTL_ID_IDX;
    } else if (strcmp(argv[1], "stg") == 0) {
        max = id = NFTL_ID_STORAGE;
    } else if (strcmp(argv[1], "stg2") == 0) {
        max = id = NFTL_ID_STORAGE2;
    } else if (strcmp(argv[1], "all") == 0) {
        max = NFTL_MAX_INSTANCE;
        id = 0;
    } else {
        tni_usage(env, argc, argv);
        return -1;
    }

    AmbaUtility_StringToUINT32(argv[2], &mode);

    do {
#if 0
        if (AmbaNFTL_IsInit(id)) {
            AmbaShell_Print(env, "%s: already initialized\n", g_nftl_str[id]);
            goto done;
        }
#endif
        Rval = AmbaNFTL_InitLock(id);
        if (Rval < 0) {
            AmbaShell_Print(env, "%s: nftl init failed\r\n",
                            g_nftl_str[id]);
            goto done;
        }

        Rval = AmbaNFTL_Init(id, mode);
        if (Rval < 0) {
            AmbaShell_Print(env, "%s: nftl init failed\r\n", g_nftl_str[id]);
        } else {
            AmbaShell_Print(env, "%s: nftl init succeed\r\n", g_nftl_str[id]);
        }

done:
        id++;
    } while (id < max);

    return Rval;
}


static void tnd_usage(AMBA_SHELL_ENV_s *env, int argc, char **argv)
{
    AmbaShell_Print(env, "Usage: %s [%s|all]\n", argv[0], g_str1);
}

int ambsh_nftl_deinit(AMBA_SHELL_ENV_s *env, int argc, char **argv)
{
    UINT32 id;
    int Rval, max;

    if (argc != 2) {
        tnd_usage(env, argc, argv);
        return -1;
    }

    if (strcmp(argv[1], "prf") == 0) {
        max = id = NFTL_ID_PRF;
    } else if (strcmp(argv[1], "cal") == 0) {
        max = id = NFTL_ID_CAL;
    } else if (strcmp(argv[1], "raw") == 0) {
        max = id = NFTL_ID_IDX;
    } else if (strcmp(argv[1], "stg") == 0) {
        max = id = NFTL_ID_STORAGE;
    } else if (strcmp(argv[1], "stg2") == 0) {
        max = id = NFTL_ID_STORAGE2;
    } else if (strcmp(argv[1], "all") == 0) {
        max = NFTL_MAX_INSTANCE;
        id = 0;
    } else {
        tnd_usage(env, argc, argv);
        return -1;
    }

    do {
        Rval = AmbaNFTL_Deinit(id);
        if (Rval < 0) {
            AmbaShell_Print(env, "%s: nftl deinit failed\r\n", g_nftl_str[id]);
        } else {
            AmbaShell_Print(env, "%s: nftl deinit succeed\r\n", g_nftl_str[id]);
        }

        id++;
    } while (id < max);

    return Rval;
}

static void tnst_usage(AMBA_SHELL_ENV_s *env, int argc, char **argv)
{
    AmbaShell_Print(env, "Usage: %s [%s|all]\n", argv[0], g_str1);
}

int ambsh_nftl_get_status(AMBA_SHELL_ENV_s *env, int argc, char **argv)
{
    AMBA_NFTL_STATUS_s status;
    UINT32 id;
    int max;

    if (argc != 2) {
        tnst_usage(env, argc, argv);
        return -1;
    }

    if (strcmp(argv[1], "prf") == 0) {
        max = id = NFTL_ID_PRF;
    } else if (strcmp(argv[1], "cal") == 0) {
        max = id = NFTL_ID_CAL;
    } else if (strcmp(argv[1], "raw") == 0) {
        max = id = NFTL_ID_IDX;
    } else if (strcmp(argv[1], "stg") == 0) {
        max = id = NFTL_ID_STORAGE;
    } else if (strcmp(argv[1], "stg2") == 0) {
        max = id = NFTL_ID_STORAGE2;
    } else if (strcmp(argv[1], "all") == 0) {
        max = NFTL_MAX_INSTANCE;
        id = 0;
    } else {
        tnst_usage(env, argc, argv);
        return -1;
    }

    do {
        AmbaShell_Print(env, "-------------------------------------\n");
        AmbaShell_Print(env, "name: \t\t\t%s\n", g_nftl_str[id]);

        if (!AmbaNFTL_IsInit(id)) {
            AmbaShell_Print(env, "not initialized\n");
            goto done;
        }

        AmbaNFTL_GetStatus(id, &status);
        AmbaShell_Print(env, "StartIChunkBlk: \t%d\n",
                        status.StartIChunkBlk);
        AmbaShell_Print(env, "IChunkBlks: \t\t%d\n",
                        status.IChunkBlks);
        AmbaShell_Print(env, "StartFtlBlk: \t%d\n",
                        status.StartFtlBlk);
        AmbaShell_Print(env, "FtlBlocks: \t\t%d\n", status.FtlBlocks);
        AmbaShell_Print(env, "LBlksPerZone: \t%d\n",
                        status.LBlksPerZone);
        AmbaShell_Print(env, "PBlksPerZone: \t%d\n",
                        status.PBlksPerZone);
        AmbaShell_Print(env, "rblks_per_zone: \t%d\n",
                        status.PBlksPerZone - status.LBlksPerZone);
        AmbaShell_Print(env, "TotalZones: \t\t%d\n", status.TotalZones);
        AmbaShell_Print(env, "NumBB: \t\t\t%d\n", status.NumBB);
        AmbaShell_Print(env, "total_logical_blocks: \t%d\n",
                        status.LBlksPerZone * status.TotalZones);
        AmbaShell_Print(env, "TotalSecs: \t\t%d\n",
                        status.TotalSecs);
        AmbaShell_Print(env, "total_size: \t\t%d KB\n",
                        status.TotalSecs >> 1);
        AmbaShell_Print(env, "WrProtect: \t%d \n",
                        status.WrProtect);
done:
        id++;
    } while (id < max);

    return 0;
}

static void trclm_usage(AMBA_SHELL_ENV_s *env, int argc, char **argv)
{
    AmbaShell_Print(env, "Usage: %s [%s|all] [init|late|other|all]\n"
                    "Where [init|late|other|all] means bad block type\n",
                    argv[0], g_str1);
}

int ambsh_nftl_reclaim_bb(AMBA_SHELL_ENV_s *env, int argc, char **argv)
{
    AMBA_NFTL_STATUS_s status;
    UINT32 id, old_NumBB, new_NumBB;
    int max;
    int Rval;

    if (argc != 3) {
        trclm_usage(env, argc, argv);
        return -1;
    }

    if (strcmp(argv[1], "prf") == 0) {
        max = id = NFTL_ID_PRF;
    } else if (strcmp(argv[1], "cal") == 0) {
        max = id = NFTL_ID_CAL;
    } else if (strcmp(argv[1], "raw") == 0) {
        max = id = NFTL_ID_IDX;
    } else if (strcmp(argv[1], "stg") == 0) {
        max = id = NFTL_ID_STORAGE;
    } else if (strcmp(argv[1], "stg2") == 0) {
        max = id = NFTL_ID_STORAGE2;
    } else if (strcmp(argv[1], "all") == 0) {
        max = NFTL_MAX_INSTANCE;
        id = 0;
    } else {
        trclm_usage(env, argc, argv);
        return -1;
    }


    do {
        AmbaShell_Print(env, "-------------------------------------\n");
        AmbaShell_Print(env, "%s: \n", g_nftl_str[id]);
        if (!AmbaNFTL_IsInit(id)) {
            AmbaShell_Print(env, "not initialized\n");
            goto done;
        }

        Rval = AmbaNFTL_GetStatus(id, &status);
        old_NumBB = status.NumBB;
        if (Rval < 0) {
            AmbaShell_Print(env, "%s: reclaim bad blocks failure\n",
                            g_nftl_str[id]);
            goto done;
        }
        AmbaNFTL_GetStatus(id, &status);
        new_NumBB = status.NumBB;

        AmbaShell_Print(env,  "Before reclaim, the bad blocks: %d\n", old_NumBB);
        AmbaShell_Print(env,  "After reclaim, the bad blocks: %d\n",  new_NumBB);
        AmbaShell_Print(env,  "False bad blocks: %d\n", old_NumBB - new_NumBB);
done:
        id++;
    } while (id < max);

    return 0;
}

#if 0
static void thealthy_usage(AMBA_SHELL_ENV_s *env, int argc, char **argv)
{
    AmbaShell_Print(env, "Usage: %s [%s|all] [check|dump] [b|bp|bps]\n",
                    argv[0], g_str1);
}

static AMBA_NFTL_USPAT_RPT_s *g_rpt = NULL, *raw_g_rpt = NULL;
static int g_rpts = 2048;
static int g_rpt_abnormal_secs[NFTL_MAX_INSTANCE];

int ambsh_nftl_health(AMBA_SHELL_ENV_s *env, int argc, char **argv)
{
    UINT32 id;
    int max;
    AMBA_NFTL_STATUS_s status;
    AMBA_MEM_CTRL_s MemCtrl;
    AMBA_NFTL_OBJ_s *pNftl;
    int b, bp, bps;
    int check, dump;
    int Rval, i;

    b = bp = bps = 0;
    check = dump = 0;
    if (argc != 4) {
        thealthy_usage(env, argc, argv);
        return -1;
    }

    if (strcmp(argv[1], "prf") == 0) {
        max = id = NFTL_ID_PRF;
    } else if (strcmp(argv[1], "cal") == 0) {
        max = id = NFTL_ID_CAL;
    } else if (strcmp(argv[1], "raw") == 0) {
        max = id = NFTL_ID_IDX;
    } else if (strcmp(argv[1], "stg") == 0) {
        max = id = NFTL_ID_STORAGE;
    } else if (strcmp(argv[1], "stg2") == 0) {
        max = id = NFTL_ID_STORAGE2;
    } else if (strcmp(argv[1], "all") == 0) {
        max = NFTL_MAX_INSTANCE;
        id = 0;
    } else {
        thealthy_usage(env, argc, argv);
        return -1;
    }

    if (strcmp(argv[2], "check") == 0) {
        check = 1;
    } else if (strcmp(argv[2], "dump") == 0) {
        dump = 1;
    } else {
        thealthy_usage(env, argc, argv);
        return -1;
    }

    if (strcmp(argv[3], "b") == 0) {
        b = 1;
    } else if (strcmp(argv[3], "bp") == 0) {
        bp = 1;
    } else if (strcmp(argv[3], "bps") == 0) {
        bps = 1;
    } else {
        b = 1;
    }

    if (g_rpt == NULL && dump) {
        AmbaShell_Print(env, "No reports!\n");
        goto done2;
    }

    if (g_rpt == NULL && check) {
        Rval = AmbaKAL_MemAllocate(AmbaShell_GetHeap(), &MemCtrl,
                                   g_rpts * sizeof(AMBA_NFTL_USPAT_RPT_s), 32);
        if (Rval < 0)
            goto done2;
    }

    g_rpt = MemCtrl.pMemAlignedBase;

    do {
        /* Output report. */
        AmbaNFTL_GetStatus(id, &status);
        AmbaShell_Print(env, "-------------------------------------\n");
        AmbaShell_Print(env, "%s: \n", g_nftl_str[id]);
        if (!AmbaNFTL_IsInit(id)) {
            AmbaShell_Print(env, "not initialized\n");
            goto done1;
        }

        if (check) {
            UINT32 Pat[MAX_BIT_PAT_32];
            pNftl = NftlGetPartObj(id);
            for (int Block = 0; Block < pNftl->TotalBlks; Block++) {
			    Rval = NftlCheckUnusedSecs(pNftl, Block, Pat, g_rpt, g_rpts);
				if (Rval < 0)
					break;
            }

            if (Rval < 0) {
                AmbaShell_Print(env,
                                "%s: check unused sector pattern "
                                "failure\n", g_nftl_str[id]);
                goto done1;
            }
            g_rpt_abnormal_secs[id] = Rval;
        }

        AmbaShell_Print(env, "abnormal sectors: %d\n",
                        g_rpt_abnormal_secs[id]);
        AmbaShell_Print(env, "partition offset: %d\n",
                        status.StartFtlBlk);

        if (b)
            goto dump_b;
        else if (bp)
            goto dump_bp;
        else if (bps)
            goto dump_bps;
dump_b:
        for (i = 0; i < g_rpt_abnormal_secs[id]; i++) {
            if (i >= g_rpts)
                break;
            if (i > 0 && (g_rpt[i].Block == g_rpt[i - 1].Block))
                continue;

            AmbaShell_Print(env, "block: %4d\n", g_rpt[i].Block);
        }
        goto done1;

dump_bp:
        for (i = 0; i < g_rpt_abnormal_secs[id]; i++) {
            if (i >= g_rpts)
                break;
            if (i > 0 && (g_rpt[i].Block == g_rpt[i - 1].Block) &&
                (g_rpt[i].Page == g_rpt[i - 1].Page))
                continue;

            AmbaShell_Print(env, "block: %4d  page: %2d\n",
                            g_rpt[i].Block, g_rpt[i].Page);
        }
        goto done1;

dump_bps:
        for (i = 0; i < g_rpt_abnormal_secs[id]; i++) {
            if (i >= g_rpts)
                break;
            AmbaShell_Print(env,
                            "block: %4d  page: %2d  sector: %2d\n",
                            g_rpt[i].Block, g_rpt[i].Page,
                            g_rpt[i].Sector);
        }
        goto done1;

done1:
        id++;
    } while (id < max);

done2:

    if (raw_g_rpt) {
        AmbaKAL_MemFree(&MemCtrl);
        g_rpt = raw_g_rpt = NULL;
    }
    return 0;
}
#endif


static void tbbm_usage(AMBA_SHELL_ENV_s *env, int argc, char **argv)
{
    AmbaShell_Print(env, "Usage: %s [%s|all]\n", argv[0], g_str1);
}

int ambsh_nftl_bbm(AMBA_SHELL_ENV_s *env, int argc, char **argv)
{
    AMBA_NFTL_STATUS_s status;
    AMBA_NFTL_BBM_STATUS_s bbm;
    UINT32 id, size;
    int max;
    int up, down;
    int rblks, min_rblks = 0xfffff, min_zone = 0;
    UINT32 *sbb_info = NULL;
    int ercd;
    int i, j, k;
    AMBA_MEM_CTRL_s MemCtrl;

    if (argc != 2) {
        tbbm_usage(env, argc, argv);
        return -1;
    }

    if (strcmp(argv[1], "prf") == 0) {
        max = id = NFTL_ID_PRF;
    } else if (strcmp(argv[1], "cal") == 0) {
        max = id = NFTL_ID_CAL;
    } else if (strcmp(argv[1], "raw") == 0) {
        max = id = NFTL_ID_IDX;
    } else if (strcmp(argv[1], "stg") == 0) {
        max = id = NFTL_ID_STORAGE;
    } else if (strcmp(argv[1], "stg2") == 0) {
        max = id = NFTL_ID_STORAGE2;
    } else if (strcmp(argv[1], "all") == 0) {
        max = NFTL_MAX_INSTANCE;
        id = 0;
    } else {
        tbbm_usage(env, argc, argv);
        return -1;
    }

    size = NFTL_BB_INFO_SIZE;
    ercd = AmbaKAL_MemAllocate(AmbaShell_GetHeap(), &MemCtrl, size, 32);
    if (ercd != OK)
        goto done2;

    sbb_info = MemCtrl.pMemAlignedBase;

    do {
        AmbaNFTL_GetStatus(id, &status);
        rblks = status.PBlksPerZone - status.LBlksPerZone;
        AmbaNFTL_GetBBMStatus(id, &bbm);
        AmbaShell_Print(env, "-------------------------------------\n");
        AmbaShell_Print(env, "%s: \n", g_nftl_str[id]);
        if (!AmbaNFTL_IsInit(id)) {
            AmbaShell_Print(env, "not initialized\n");
            goto done1;
        }

        AmbaShell_Print(env, "number of bad_blk_handler called: %d\n",
                        bbm.TotalCnt);
        AmbaShell_Print(env, "number of failed write: %d\n", bbm.FailWrCnt);
        AmbaShell_Print(env, "number of failed read: %d\n", bbm.FailRdCnt);
        AmbaShell_Print(env, "number of failed erase: %d\n\n", bbm.FailErCnt);

        /* Sort bb_info */
        memset(sbb_info, 0xff, size);
        for (i = 0; i < bbm.NumBB; i++) {
            k = 0;
            for (j = 0; j < bbm.NumBB; j++) {
                if (bbm.pBBInfo[i] > bbm.pBBInfo[j])
                    k++;
            }
            sbb_info[k] = bbm.pBBInfo[i];
        }

        for (i = 0; i < status.TotalZones; i++) {
            k = 0;
            down = i * status.PBlksPerZone;
            up = (i + 1) * status.PBlksPerZone;
            AmbaShell_Print(env, "zone %d: %d - %d\n", i, down, up);

            for (j = 0; j < bbm.NumBB; j++) {
                if (sbb_info[j] >= down &&
                    sbb_info[j] < up) {
                    AmbaShell_Print(env, "\t%d", sbb_info[j]);
                    k++;
                    if (k % 4 == 0)
                        AmbaShell_Print(env, "\n");
                }
            }

            /* Problems in sbb_info, dump the original bb_info. */
            if (k != bbm.NumBB) {
                AmbaShell_Print(env, "\n\n\toriginal bb_info:\n");
                for (j = 0; j < bbm.NumBB;) {
                    AmbaShell_Print(env, "\t%d",
                                    bbm.pBBInfo[j]);
                    j++;
                    if (j % 4 == 0)
                        AmbaShell_Print(env, "\n");
                }
                if (j % 4)
                    AmbaShell_Print(env, "\n");
            }

            if (k % 4)
                AmbaShell_Print(env, "\n");
            AmbaShell_Print(env, "\tbad blocks: %d\n", bbm.NumBB);
            AmbaShell_Print(env, "\tavaiable spare blocks: %d\n",
                            rblks - k);
            if ((rblks - k) < min_rblks) {
                min_rblks = rblks - k;
                min_zone = i;
            }
        }
        AmbaShell_Print(env, "\nminimun spare blocks are %d in zone %d\n",
                        min_rblks, min_zone);

done1:
        id++;
    } while (id < max);

done2:
    if (sbb_info)
        AmbaKAL_MemFree(&MemCtrl);

    return 0;
}


static void tl2p_usage(AMBA_SHELL_ENV_s *env, int argc, char **argv)
{
    AmbaShell_Print(env, "Usage: %s [%s] [block] [page] [sector]\n",
                    argv[0], g_str1);
}

int ambsh_nftl_log2phy(AMBA_SHELL_ENV_s *env, int argc, char **argv)
{
    int id, zone, lblks;
    UINT32 block, page, sector;
    int b, bp, bps;
    UINT16 *pTable;
    UINT16 pba;
    AMBA_NFTL_OBJ_s *pNftl;
    AMBA_NAND_DEV_s *pDev = AmbaRTSL_NandGetDev();

    if (argc > 5 || argc < 3) {
        tl2p_usage(env, argc, argv);
        return -1;
    }

    block = page = sector = 0;
    b = bp = bps = 0;
    if (argc == 5)
        bps = 1;
    else if (argc == 4)
        bp = 1;
    else if (argc == 3)
        b = 1;

    if (strcmp(argv[1], "prf") == 0) {
        id = NFTL_ID_PRF;
    } else if (strcmp(argv[1], "cal") == 0) {
        id = NFTL_ID_CAL;
    } else if (strcmp(argv[1], "raw") == 0) {
        id = NFTL_ID_IDX;
    } else if (strcmp(argv[1], "stg") == 0) {
        id = NFTL_ID_STORAGE;
    } else if (strcmp(argv[1], "stg2") == 0) {
        id = NFTL_ID_STORAGE2;
    } else {
        tl2p_usage(env, argc, argv);
        return -1;
    }

    if (!AmbaNFTL_IsInit(id)) {
        AmbaShell_Print(env, "not initialized\n");
        return -1;
    }

    AmbaUtility_StringToUINT32(argv[2], &block);
    if (bp || bps)
        AmbaUtility_StringToUINT32(argv[3], &page);
    if (bps)
        AmbaUtility_StringToUINT32(argv[4], &sector);

    pNftl = NftlGetPartObj(id);
    pTable = pNftl->pTrlTable;

    lblks = pNftl->TotalZones * pNftl->LBlksPerZone;
    if (block >= lblks) {
        AmbaShell_Print(env, "the maximun block address is %d\n", lblks - 1);
        return -1;
    }

    pba = pTable[block];
    if (pba == NFTL_UNUSED_BLK) {
        AmbaShell_Print(env, "no physical address\n", block);
        return 0;
    }

    zone = block / pNftl->LBlksPerZone;
    pba = zone * pNftl->PBlksPerZone + pba;

    AmbaShell_Print(env, "physical address for block %d:\n", block);
    if (b) {
        AmbaShell_Print(env, "block: %d\n", pba);
    }

    if (bp) {
        AmbaShell_Print(env, "block: %d\n", pba);
        AmbaShell_Print(env, "page: %d\n", page);

        page = pba * pDev->DevLogicInfo.PagesPerBlock + page;
        AmbaShell_Print(env, "absolute page: %d\n", page);
    }

    if (bps) {
        AmbaShell_Print(env, "block: %d\n", pba);
        AmbaShell_Print(env, "page: %d\n", page);
        AmbaShell_Print(env, "sector: %d\n", sector);

        sector = (pba * pDev->DevLogicInfo.PagesPerBlock + page) *
                 pDev->DevLogicInfo.SectorsPerPage + sector;
        AmbaShell_Print(env, "absolute sector: %d\n", sector);
    }

    return 0;
}

static void tp2l_usage(AMBA_SHELL_ENV_s *env, int argc, char **argv)
{
    AmbaShell_Print(env, "Usage: %s [%s] [block] [page] [sector]\n",
                    argv[0], g_str1);
}

int ambsh_nftl_phy2log(AMBA_SHELL_ENV_s *env, int argc, char **argv)
{
    int id, zone;
    UINT32 block, page, sector;
    int b, bp, bps;
    UINT16 *pTable;
    UINT16 lba;
    AMBA_NFTL_OBJ_s *nftl;
    AMBA_NAND_DEV_s *pDev = AmbaRTSL_NandGetDev();

    if (argc > 5 || argc < 3) {
        tp2l_usage(env, argc, argv);
        return -1;
    }

    block = page = sector = 0;
    b = bp = bps = 0;
    if (argc == 5)
        bps = 1;
    else if (argc == 4)
        bp = 1;
    else if (argc == 3)
        b = 1;

    if (strcmp(argv[1], "prf") == 0) {
        id = NFTL_ID_PRF;
    } else if (strcmp(argv[1], "cal") == 0) {
        id = NFTL_ID_CAL;
    } else if (strcmp(argv[1], "idx") == 0) {
        id = NFTL_ID_IDX;
    } else if (strcmp(argv[1], "stg") == 0) {
        id = NFTL_ID_STORAGE;
    } else if (strcmp(argv[1], "stg2") == 0) {
        id = NFTL_ID_STORAGE2;
    } else {
        tp2l_usage(env, argc, argv);
        return -1;
    }

    if (!AmbaNFTL_IsInit(id)) {
        AmbaShell_Print(env, "not initialized\n");
        return -1;
    }

    AmbaUtility_StringToUINT32(argv[2], &block);
    if (bp || bps)
        AmbaUtility_StringToUINT32(argv[3], &page);
    if (bps)
        AmbaUtility_StringToUINT32(argv[4], &sector);

    nftl = NftlGetPartObj(id);
    pTable = nftl->pPhyTable;
    if (block >= nftl->TotalBlks) {
        AmbaShell_Print(env, "the maximun block address is %d\n",
                        nftl->TotalBlks - 1);
        return -1;
    }

    lba = pTable[block];
    if (lba == NFTL_UNUSED_BLK) {
        AmbaShell_Print(env, "no logical address\n", block);
        return 0;
    }

    zone = block / nftl->PBlksPerZone;
    lba = zone * nftl->LBlksPerZone + lba;

    AmbaShell_Print(env, "logical address for block %d\n", block);
    if (b) {
        AmbaShell_Print(env, "block: %d\n", lba);
    }

    if (bp) {
        AmbaShell_Print(env, "block: %d\n", lba);
        AmbaShell_Print(env, "page: %d\n", page);

        page = lba * pDev->DevLogicInfo.PagesPerBlock + page;
        AmbaShell_Print(env, "absolute page: %d\n", page);
    }

    if (bps) {
        AmbaShell_Print(env, "block: %d\n", lba);
        AmbaShell_Print(env, "page: %d\n", page);
        AmbaShell_Print(env, "sector: %d\n", sector);

        sector = (lba * pDev->DevLogicInfo.PagesPerBlock + page) *
                 pDev->DevLogicInfo.SectorsPerPage + sector;
        AmbaShell_Print(env, "absolute sector: %d\n", sector);
    }

    return 0;
}

static void tulba_usage(AMBA_SHELL_ENV_s *env, int argc, char **argv)
{
    AmbaShell_Print(env, "Usage: %s [%s]\n", argv[0], g_str1);
}

int ambsh_nftl_used_lba(AMBA_SHELL_ENV_s *env, int argc, char **argv)
{
    int id, n = 0;
    AMBA_NFTL_OBJ_s *nftl;
    UINT16 *pTable;
    int lba, lblks;
    UINT16 pba;

    if (argc != 2) {
        tulba_usage(env, argc, argv);
        return -1;
    }

    if (strcmp(argv[1], "prf") == 0) {
        id = NFTL_ID_PRF;
    } else if (strcmp(argv[1], "cal") == 0) {
        id = NFTL_ID_CAL;
    } else if (strcmp(argv[1], "raw") == 0) {
        id = NFTL_ID_IDX;
    } else if (strcmp(argv[1], "stg") == 0) {
        id = NFTL_ID_STORAGE;
    } else if (strcmp(argv[1], "stg2") == 0) {
        id = NFTL_ID_STORAGE2;
    } else {
        tulba_usage(env, argc, argv);
        return -1;
    }

    if (!AmbaNFTL_IsInit(id)) {
        AmbaShell_Print(env, "not initialized\n");
        return -1;
    }

    nftl = NftlGetPartObj(id);
    pTable = nftl->pTrlTable;

    lblks = nftl->TotalZones * nftl->LBlksPerZone;

    AmbaShell_Print(env, "-------------------------------------\n");
    AmbaShell_Print(env, "%s: used logical block address:\n\n",
                    g_nftl_str[id]);
    for (lba = 0; lba < lblks; lba++) {
        pba = pTable[lba];
        if (pba == NFTL_UNUSED_BLK)
            continue;
        n++;
        AmbaShell_Print(env, "%4d", lba);
        if (n % 8 == 0)
            AmbaShell_Print(env, "\n");
    }

    if (n % 8 == 0)
        AmbaShell_Print(env, "\n");
    else
        AmbaShell_Print(env, "\n\n");

    AmbaShell_Print(env, "number of used logical blocks: %d\n", n);
    AmbaShell_Print(env, "total number of logical blocks: %d\n", lblks);

    return 0;
}

static void tuupba_usage(AMBA_SHELL_ENV_s *env, int argc, char **argv)
{
    AmbaShell_Print(env, "Usage: %s [%s]\n", argv[0], g_str1);
}

int ambsh_nftl_unused_pba(AMBA_SHELL_ENV_s *env, int argc, char **argv)
{
    int id, n = 0;
    AMBA_NFTL_OBJ_s *nftl;
    UINT16 *pTable;
    int Rval, pba;
    UINT16 lba;

    if (argc != 2) {
        tuupba_usage(env, argc, argv);
        return -1;
    }

    if (strcmp(argv[1], "prf") == 0) {
        id = NFTL_ID_PRF;
    } else if (strcmp(argv[1], "cal") == 0) {
        id = NFTL_ID_CAL;
    } else if (strcmp(argv[1], "raw") == 0) {
        id = NFTL_ID_IDX;
    } else if (strcmp(argv[1], "stg") == 0) {
        id = NFTL_ID_STORAGE;
    } else if (strcmp(argv[1], "stg2") == 0) {
        id = NFTL_ID_STORAGE2;
    } else {
        tuupba_usage(env, argc, argv);
        return -1;
    }

    if (!AmbaNFTL_IsInit(id)) {
        AmbaShell_Print(env, "not initialized\n");
        return -1;
    }

    nftl = NftlGetPartObj(id);
    pTable = nftl->pPhyTable;

    AmbaShell_Print(env, "-------------------------------------\n");
    AmbaShell_Print(env, "%s: unused physical block address:\n\n", g_nftl_str[id]);

    for (pba = 0; pba < nftl->TotalBlks; pba++) {
        lba = pTable[pba];
        if (lba != NFTL_UNUSED_BLK)
            continue;
        n++;

        Rval = AmbaNandBBT_IsBadBlock(pba + nftl->StartBlkPart);
        if (Rval == 1)
            AmbaShell_Print(env, "%4d*", pba);
        else
            AmbaShell_Print(env, "%4d ", pba);

        if (n % 8 == 0)
            AmbaShell_Print(env, "\n");
    }

    if (n % 8 == 0)
        AmbaShell_Print(env, "\n");
    else
        AmbaShell_Print(env, "\n\n");

    AmbaShell_Print(env, "number of unused physical blocks: %d\n", n);
    AmbaShell_Print(env, "total number of physical blocks: %d\n", nftl->TotalBlks);

    return 0;
}

#if 0
static void tmbb_usage(AMBA_SHELL_ENV_s *env, int argc, char **argv)
{
    AmbaShell_Print(env, "Usage: %s [%s] [block...]\n", argv[0], g_str1);
}

int ambsh_nftl_mark_bb(AMBA_SHELL_ENV_s *env, int argc, char **argv)
{
    extern void NftlUpdateLogicTable(AMBA_NFTL_OBJ_s * pNftl, UINT32 Lba, UINT32 Pba);
    extern void NftlUpdatePhyTable(AMBA_NFTL_OBJ_s * pNftl, UINT32 Pba, UINT32 Lba);
    int id, Rval, i;
    UINT32 block;
    AMBA_NFTL_OBJ_s *nftl;
    UINT16 *pTable;
    UINT16 lba;
    int zone;

    if (argc < 3) {
        tmbb_usage(env, argc, argv);
        return -1;
    }

    if (strcmp(argv[1], "prf") == 0) {
        id = NFTL_ID_PRF;
    } else if (strcmp(argv[1], "cal") == 0) {
        id = NFTL_ID_CAL;
    } else if (strcmp(argv[1], "raw") == 0) {
        id = NFTL_ID_IDX;
    } else if (strcmp(argv[1], "stg") == 0) {
        id = NFTL_ID_STORAGE;
    } else if (strcmp(argv[1], "stg2") == 0) {
        id = NFTL_ID_STORAGE2;
    } else {
        tmbb_usage(env, argc, argv);
        return -1;
    }

    if (!AmbaNFTL_IsInit(id)) {
        AmbaShell_Print(env, "not initialized\n");
        return -1;
    }

    nftl = NftlGetPartObj(id);
    pTable = nftl->pPhyTable;

    for (i = 2; i < argc; i++) {
        AmbaUtility_StringToUINT32(argv[i], &block);
        if (block >= nftl->TotalBlks) {
            AmbaShell_Print(env, "the maximun block address is %d\n",
                            nftl->TotalBlks);
            continue;
        }

        Rval = AmbaNandBBT_IsBadBlock(block + nftl->StartBlkPart);
        if (Rval == 1) {
            AmbaShell_Print(env, "already bad block: %d\n", block);
            continue;
        }

        Rval = nftl_mark_bad_blk(nftl, block);
        if (Rval == 0) {
            lba = pTable[block];
            if (lba != NFTL_UNUSED_BLK) {
                zone = block / nftl->PBlksPerZone;
                lba = zone * nftl->LBlksPerZone + lba;
                NftlUpdateLogicTable(nftl, lba, NFTL_UNUSED_BLK);
                NftlUpdatePhyTable(nftl, block, NFTL_UNUSED_BLK);
            }
            AmbaShell_Print(env, "mark bad block %d success\n", block);
        } else {
            AmbaShell_Print(env, "mark bad block %d failure\n", block);
        }
    }
    return 0;
}
#endif

/* -------------------------------------------------------------------------- */

struct ambsh_nftl_list {
    char *name;
    int (*func)(AMBA_SHELL_ENV_s *, int, char **);
};

struct ambsh_nftl_list ambsh_nftl_list[] = {
    { "rs",         ambsh_nftl_read_sector },
    { "ws",         ambsh_nftl_write_sector },
    { "verify",     ambsh_nftl_verify_sector },
    { "pr",         ambsh_nftl_pread },
    { "pw",         ambsh_nftl_pwrite },
    { "erase",      ambsh_nftl_erase },
    { "erase_lba",  ambsh_nftl_erase_lba },
    { "thruput",    ambsh_nftl_thruput },
    { "init",       ambsh_nftl_init },
    { "deinit",     ambsh_nftl_deinit },
    { "status",     ambsh_nftl_get_status },
    { "rclm_bb",    ambsh_nftl_reclaim_bb },
    { "bbm",        ambsh_nftl_bbm },
    { "l2p",        ambsh_nftl_log2phy },
    { "p2l",        ambsh_nftl_phy2log },
    { "ulba",       ambsh_nftl_used_lba },
    { "uupba",      ambsh_nftl_unused_pba },
#if 0
    { "health",     ambsh_nftl_health },
    { "mark_bb",    ambsh_nftl_mark_bb },
#endif
    { NULL, NULL }
};

static void usage(AMBA_SHELL_ENV_s *env, int argc, char **argv)
{
    int i;

    AmbaShell_Print(env, "valid test targets for flash are:\n");

    for (i = 0; ; i++) {
        if (ambsh_nftl_list[i].name == NULL ||
            ambsh_nftl_list[i].func == NULL)
            break;
        AmbaShell_Print(env, "\t%s\n", ambsh_nftl_list[i].name);
    }
}

/***************************/
/* Export to .ambsh section */
/***************************/

int ambsh_nftl(AMBA_SHELL_ENV_s *env, int argc, char **argv)
{
    int i;

    if (argc < 2) {
        usage(env, argc, argv);
        return -1;
    }

    for (i = 0; ; i++) {
        if (ambsh_nftl_list[i].name == NULL ||
            ambsh_nftl_list[i].func == NULL) {
            break;
        }

        if (strcmp(ambsh_nftl_list[i].name, argv[1]) == 0)
            return ambsh_nftl_list[i].func(env, argc - 1, &argv[1]);
    }

    usage(env, argc, argv);

    return -2;
}

