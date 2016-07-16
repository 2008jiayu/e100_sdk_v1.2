/*-----------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaShell_Cardmgr.c
 *
 *  @Copyright      :: Copyright (C) 2012 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Ambarella utilities.
\*-----------------------------------------------------------------------------------------------*/

#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include <AmbaDataType.h>

#include "AmbaUtility.h"
#include "AmbaPrintk.h"
#include "AmbaFS.h"

#include "AmbaKAL.h"
#include "AmbaCardManager.h"

#include "AmbaShell.h"
#include "AmbaShell_Priv.h"

static inline void usage(AMBA_SHELL_ENV_s *pEnv, int argc, char **argv)
{
    AmbaShell_Print(pEnv, "Usage: %s [drive] op\n", argv[0]);
    AmbaShell_Print(pEnv, "\tvalid ops:\n");
    AmbaShell_Print(pEnv, "\tinfo - information of card\n");
    AmbaShell_Print(pEnv, "\tinsert - software insert\n");
    AmbaShell_Print(pEnv, "\teject - software eject\n");
    AmbaShell_Print(pEnv, "\tsync - sync card operations\n");
    AmbaShell_Print(pEnv, "\ten_ffmon - enable free fall monitor\n");
    AmbaShell_Print(pEnv, "\tdis_ffmon - disable free fall monitor\n");
    AmbaShell_Print(pEnv, "\tmount [Slot]\n");
    AmbaShell_Print(pEnv, "\tunmount [Slot]\n");
    AmbaShell_Print(pEnv, "\tfast_ecn [Slot] [pages]\n");
    AmbaShell_Print(pEnv, "\treinit - re-initialize devices operations\n");
    AmbaShell_Print(pEnv, "\t\tSD working frequence:\n");
    AmbaShell_Print(pEnv, "\t\t\t[KHz]\n");
    AmbaShell_Print(pEnv, "\tiostat_enable\n");
    AmbaShell_Print(pEnv, "\tiostat_disable\n");
    AmbaShell_Print(pEnv, "\tiostat [diagram bar number]\n");
    AmbaShell_Print(pEnv, "\tfat32_ecl\n");
    AmbaShell_Print(pEnv, "\tdebou\n");
}

const char *GetFsFormat(UINT8 Id)
{
    switch (Id) {
    case FS_FAT12:
        return "FAT12";
    case FS_FAT16S:
        return "FAT16S";
    case FS_FAT16_EXT_DOS:
        return "Extended MS-DOS FAT16";
    case FS_FAT16:
        return "FAT16";
    case FS_FAT32:
        return "FAT32";
    case FS_FAT32L:
        return "FAT32L";
    case FS_EXFAT:
        return "EXFAT";
    case FS_FAT16L:
        return "FAT16L";
    case FS_FAT16L_EXT_DOS:
        return "Extended MS-DOS FAT16L";
    case AMBA_FS_RFS_FORMAT_ID:
        return "ROMFS";
    }

    return "UNKNOWN";
}

static void DumpIoStatDiagram(AMBA_SHELL_ENV_s *pEnv, UINT32 *lvl, int BarNum)
{
    int i, j;
    UINT32 TotalSecs = 0;

    if (BarNum <= 0) {
        BarNum = 100;  /* set default bar number */
    }

    for (i = 0; i < AMBA_FS_PRF2_IOSTAT_MAX_LVL; i++) {
        TotalSecs += lvl[i];
    }

    if (TotalSecs < BarNum) {
        AmbaShell_Print(pEnv, "No diagram\n");
        return;
    }

    for (i = 0; i < AMBA_FS_PRF2_IOSTAT_MAX_LVL; i++) {
        int lvlcnt;
        lvlcnt = lvl[i] * BarNum / TotalSecs;
        AmbaShell_Print(pEnv, "[--%5d secs] ", 1 << i);
        for (j = 0; j < lvlcnt; j++) {
            AmbaShell_Print(pEnv, "*");
        }
        AmbaShell_Print(pEnv, " [%d]\n", lvl[i]);
    }
}

int ambsh_cardmgr(AMBA_SHELL_ENV_s *pEnv, int argc, char **argv)
{
    int Rval;
    int Slot;
    AMBA_SCM_STATUS_s Status;
    const char *type;
    UINT64 rb, wb;
    UINT32 rt, wt;
    UINT32 rl[AMBA_FS_PRF2_IOSTAT_MAX_LVL];
    UINT32 wl[AMBA_FS_PRF2_IOSTAT_MAX_LVL];


    if (strcmp(argv[1], "switch") == 0)
        goto next;
    else if (strcmp(argv[1], "drvinfo") == 0)
        goto next;

    if ((argc != 3 && argc != 4) || argv[1][1] != '\0') {
        usage(pEnv, argc, argv);
        return -1;
    }
next:
    Slot = tolower((int) argv[1][0]) - 'a';

    if (strcmp(argv[2], "info") == 0) {
        /********/
        /* info */
        /********/

        Rval = AmbaSCM_GetSlotStatus(Slot, &Status);
        if (Rval < 0) {
            AmbaShell_Print(pEnv, "cannot get Status!\n");
            return -10;
        }

        if (Status.CardType == (SCM_CARD_MEM |
                                SCM_CARD_IO))
            type =  "memory + I/O combo";
        else if (Status.CardType == (SCM_CARD_MEM |
                                     SCM_CARD_MECH))
            type =  "mechanical memory";
        else if (Status.CardType == (SCM_CARD_MEM |
                                     SCM_CARD_SD  |
                                     SCM_CARD_SDHC |
                                     SCM_CARD_SDXC))
            type =  "SDXC memory";
        else if (Status.CardType == (SCM_CARD_MEM |
                                     SCM_CARD_SD  |
                                     SCM_CARD_SDHC))
            type =  "SDHC memory";
        else if (Status.CardType == (SCM_CARD_MEM |
                                     SCM_CARD_SD))
            type =  "SD memory";
        else if (Status.CardType == (SCM_CARD_MEM |
                                     SCM_CARD_MMC))
            type =  "MMC memory";
        else if (Status.CardType == SCM_CARD_MEM)
            type = "memory";
        else if (Status.CardType == SCM_CARD_IO)
            type = "I/O";
        else
            type = "unknown";

        AmbaShell_Print(pEnv, "Slot '%c' info\n", 'a' + Slot);
        AmbaShell_Print(pEnv, "present:\t%s\n", Status.CardPresent ? "yes" : "no");

        if (Status.CardPresent == 0)
            return 0;
#if 0
        AmbaShell_Print(pEnv, "manfid:\t\t0x%.8x\n", Status.manfid);
        AmbaShell_Print(pEnv, "vers:\t\t0x%.8x\n", Status.vers);
#endif
        AmbaShell_Print(pEnv, "type:\t\t%s\n", type);

        if ((Status.CardType & SCM_CARD_MEM) == 0)
            return 0;

        AmbaShell_Print(pEnv, "name:\t\t%s\n",        Status.Name);
        AmbaShell_Print(pEnv, "bus width:\t%d\n",     Status.BusWidth);
        AmbaShell_Print(pEnv, "total sectors:\t%d\n", Status.SecsCnt);
        AmbaShell_Print(pEnv, "sector size:\t%d\n",   Status.SecSize);
        AmbaShell_Print(pEnv, "write-protect:\t%s\n", Status.WriteProtect ? "yes" : "no");

#if 0
        if ((Status.CardType & SCM_CARD_MEM) &&
            ((Status.CardType & SCM_CARD_SD) || (Status.CardType & SCM_CARD_MMC))) {
            if (Status.wp) {
                AmbaShell_Print(pEnv, "(");
                if (Status.wp & SD_PERM_WP)
                    AmbaShell_Print(pEnv, "PERM_WP\t");
                if (Status.wp & SD_TMP_WP)
                    AmbaShell_Print(pEnv, "TMP_WP\t");
                if (Status.wp & SD_HW_WP)
                    AmbaShell_Print(pEnv, "HW_WP");
                AmbaShell_Print(pEnv, ")\t\n");
            }
        }
#endif
        AmbaShell_Print(pEnv, "format:\t\t%s\n", GetFsFormat(Status.Format));
        AmbaShell_Print(pEnv, "speed:\t\t%s\n", Status.Speed);
    } else if (strcmp(argv[2], "insert") == 0) {
        /**********/
        /* insert */
        /**********/

        Rval = AmbaSCM_GetSlotStatus(Slot, &Status);
        if (Rval < 0) {
            AmbaShell_Print(pEnv, "cannot get Status!\n");
            return -20;
        }

        if (Status.CardPresent) {
            AmbaShell_Print(pEnv, "card already present!\n");
            return -21;
        }

        Rval = AmbaSCM_Insert(Slot);
        if (Rval >= 0)
            AmbaShell_Print(pEnv, "card inserted\n");
        else
            AmbaShell_Print(pEnv, "cannot insert card!\n");
    } else if (strcmp(argv[2], "eject") == 0) {
        /*********/
        /* eject */
        /*********/

        Rval = AmbaSCM_Eject(Slot);
        if (Rval >= 0)
            AmbaShell_Print(pEnv, "card ejected\n");
        else
            AmbaShell_Print(pEnv, "cannot eject card!\n");
    } else if (strcmp(argv[2], "mount") == 0) {
        /**********/
        /* mount  */
        /**********/
        Rval = AmbaFS_Mount(AmbaUtility_Slot2Drive(Slot));
        if (Rval < 0) {
            AmbaShell_Print(pEnv, "mount fail!\n");
            return -50;
        }

    } else if (strcmp(argv[2], "unmount") == 0) {
        /**********/
        /* unmount  */
        /**********/
        AmbaFS_Sync(AmbaUtility_Slot2Drive(Slot), AMBA_FS_INVALIDATE);

        Rval = AmbaFS_Unmount(AmbaUtility_Slot2Drive(Slot));
        if (Rval < 0) {
            AmbaShell_Print(pEnv, "Unmount fail!\n");
            return -50;
        }

    } else if (strcmp(argv[2], "reinit") == 0) {
        /**********/
        /* reinit */
        /**********/
        if (argc != 4) {
            usage(pEnv, argc, argv);
            return -60;
        }

        Rval = AmbaSCM_GetSlotStatus(Slot, &Status);
        if (Rval < 0) {
            AmbaShell_Print(pEnv, "cannot get Status!\n");
            return -50;
        }

        if (!Status.CardPresent) {
            AmbaShell_Print(pEnv, "no card in Slot!\n");
            return -51;
        }

        switch (Slot) {
        case SCM_SLOT_FL0:
            AmbaShell_Print(pEnv, "not supported!\n");
            break;
        case SCM_SLOT_SD0:
        case SCM_SLOT_SD1: {
            UINT32 clock;
            AmbaUtility_StringToUINT32(argv[3], &clock);
            Rval = AmbaSCM_Reinit(Slot, clock);
        }
        break;
        case SCM_SLOT_RD:
        case SCM_SLOT_RF2:
        case SCM_SLOT_FL1:
        default:
            AmbaShell_Print(pEnv, "not supported!\n");
            break;
        }

        if (Rval < 0)
            AmbaShell_Print(pEnv, "cannot reinit card!\n");
    } else if (strcmp(argv[2], "iostat_enable") == 0) {
        /************************/
        /* IO statistics enable */
        /************************/

        Rval = AmbaSCM_GetSlotStatus(Slot, &Status);
        if (Rval < 0) {
            AmbaShell_Print(pEnv, "cannot get Status!\n");
            return -40;
        }

        if (!Status.CardPresent) {
            AmbaShell_Print(pEnv, "no card in Slot!\n");
            return -41;
        }

        /* Enable IO statistics */
        AmbaSCM_IoStat2ndSet(Slot, 1);

        AmbaShell_Print(pEnv, "IO statistics enabled\n");

    } else if (strcmp(argv[2], "iostat_disable") == 0) {
        /*************************/
        /* IO statistics disable */
        /*************************/

        Rval = AmbaSCM_GetSlotStatus(Slot, &Status);
        if (Rval < 0) {
            AmbaShell_Print(pEnv, "cannot get Status!\n");
            return -40;
        }

        if (!Status.CardPresent) {
            AmbaShell_Print(pEnv, "no card in Slot!\n");
            return -41;
        }

        /* Disable IO statistics */
        AmbaSCM_IoStat2ndSet(Slot, 0);

        AmbaShell_Print(pEnv, "IO statistics disabled\n");

    } else if (strcmp(argv[2], "iostat") == 0) {
        /**************************/
        /* Get IO statistics data */
        /**************************/

        Rval = AmbaSCM_GetSlotStatus(Slot, &Status);
        if (Rval < 0) {
            AmbaShell_Print(pEnv, "cannot get Status!\n");
            return -40;
        }

        if (!Status.CardPresent) {
            AmbaShell_Print(pEnv, "no card in Slot!\n");
            return -41;
        }

        Rval = AmbaSCM_IoStat2ndGet(Slot, &rb, &wb, &rt, &wt, rl, wl);
        if (0 == Rval) {
            AmbaShell_Print(pEnv, "Read diagram:\n");
            AmbaShell_Print(pEnv,
                            "-----------------------------------\n");
            DumpIoStatDiagram(pEnv, rl, atoi(argv[3]));
            AmbaShell_Print(pEnv,
                            "-----------------------------------\n\n");

            AmbaShell_Print(pEnv, "Write diagram:\n");
            AmbaShell_Print(pEnv,
                            "-----------------------------------\n");
            DumpIoStatDiagram(pEnv, wl, atoi(argv[3]));
            AmbaShell_Print(pEnv,
                            "-----------------------------------\n\n");

            AmbaShell_Print(pEnv, "IO statistics:\n");
            AmbaShell_Print(pEnv,
                            "-----------------------------------\n");
            AmbaShell_Print(pEnv, "bytes read: %u\n", rb);
            AmbaShell_Print(pEnv, "read time elapsed: %u ms\n", rt);
            AmbaShell_Print(pEnv, "bytes writen: %u\n", wb);
            AmbaShell_Print(pEnv, "write time elapsed: %u ms\n", wt);
            AmbaShell_Print(pEnv, "Read throughput: %f Mbit/sec\n",
                            8.0 * rb / rt / 1000);
            AmbaShell_Print(pEnv, "Write throughput: %f Mbit/sec\n",
                            8.0 * wb / wt / 1000);
            AmbaShell_Print(pEnv,
                            "-----------------------------------\n");
        } else {
            AmbaShell_Print(pEnv, "Get IO statistics fail!\n");
        }
    } else {
        usage(pEnv, argc, argv);
        return -2;
    }

    return 0;
}


