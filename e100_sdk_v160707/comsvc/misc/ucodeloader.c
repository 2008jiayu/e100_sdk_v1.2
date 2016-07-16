 /**
  * @file src/comsvc/ucodeloader.c
  *
  * Ucode loader
  *
  * History:
  *    2013/06/21 - [Jenghung Luo] created file
  *
  * Copyright (C) 2013, Ambarella, Inc.
  *
  * All rights reserved. No Part of this file may be reproduced, stored
  * in a retrieval system, or transmitted, in any form, or by any means,
  * electronic, mechanical, photocopying, recording, or otherwise,
  * without the prior consent of Ambarella, Inc.
  */

#include <stdio.h>
#include <string.h>
#include <AmbaDataType.h>
#include <AmbaKAL.h>
#include <AmbaPrintk.h>
#include <AmbaROM.h>
#include <AmbaFS.h>
#include "AmbaCardManager.h"
#include <AmbaUtility.h>
#include <AmbaDSP.h>

typedef struct _AMBA_DSP_UCODE_CTRL_s_ {
    char    pFileName[33];
    UINT32  BaseAddr;
} AMBA_DSP_UCODE_CTRL_s;

static UINT8 UcodeLoaded = 0;

/**
 *  Load DSP uCodes from NAND to DDR
 *
 *  @return 0 - success others - fail
 */
int AmbaLoadDSPuCode(void)
{
    AMBA_DSP_UCODE_CTRL_s *pUcodeInfo;
    int i, DataSize, RetStatus = OK;
    UINT32 CodeAddr, MeAddr, MdxfAddr, DefaultDataAddr;

    AMBA_DSP_UCODE_CTRL_s AmbaDspUcodeCtrl[] = {
        {"orccode.bin", 0},
        {"orcme.bin",   0},
        {"default_binary.bin", 0},
    };

    if (UcodeLoaded == 0xFF) {
        return 0;
    }

    AmbaDSP_GetUcodeBaseAddr(&CodeAddr, &MeAddr, &MdxfAddr, &DefaultDataAddr);
    
	AmbaDspUcodeCtrl[0].BaseAddr = CodeAddr;
    AmbaDspUcodeCtrl[1].BaseAddr = MeAddr;
    AmbaDspUcodeCtrl[2].BaseAddr = DefaultDataAddr;

    pUcodeInfo = &(AmbaDspUcodeCtrl[0]);
    for (i = 0; i < GetArraySize(AmbaDspUcodeCtrl); i++, pUcodeInfo++) {
        DataSize = AmbaROM_GetSize(AMBA_ROM_DSP_UCODE, pUcodeInfo->pFileName, 0x0);
        if (DataSize > 0) {
            DataSize = AmbaROM_LoadByName(AMBA_ROM_DSP_UCODE, pUcodeInfo->pFileName, (UINT8 *) pUcodeInfo->BaseAddr, DataSize, 0);
        } else {
            RetStatus |= NG;
            AmbaPrint("No uCode in ROM uCode partition: Ret %d",DataSize);
            break;
        }
        AmbaPrint("Load %s to 0x%08x, size = %d", pUcodeInfo->pFileName, pUcodeInfo->BaseAddr, DataSize);
    }

    if(RetStatus != OK) { // Temp solution for unittest
        AMBA_FS_FILE *fp = 0;
        const char *pFileName;
        AMBA_FS_STAT fstat;
        AMBA_DSP_UCODE_CTRL_s AmbaDspUcodeCtrl2[] = {
            {"c:\\uCode\\orccode.bin", 0},
            {"c:\\uCode\\orcme.bin",   0},
            {"c:\\uCode\\default_binary.bin", 0},
        };

        AmbaDspUcodeCtrl2[0].BaseAddr = CodeAddr;
        AmbaDspUcodeCtrl2[1].BaseAddr = MeAddr;
        AmbaDspUcodeCtrl2[2].BaseAddr = DefaultDataAddr;

        {
            AMBA_SCM_STATUS_s Status;
            int i = 0;
            do {
                /* Wait SD card initial. */
                AmbaSCM_GetSlotStatus(SCM_SLOT_SD0, &Status);
                /* continue to delay 2 msec to check SD ready to load ucode from SD card */
                AmbaKAL_TaskSleep(2);
                i++;
                if(i == 1000) {
                    AmbaPrint("SD status timout, can't load ucode from SD card 0");
                }
            } while (Status.CardPresent && Status.Format == -1);
        }

        pUcodeInfo = &(AmbaDspUcodeCtrl2[0]);
        RetStatus = OK;
        for (i = 0; i < GetArraySize(AmbaDspUcodeCtrl2); i++, pUcodeInfo++) {
            UINT64 sz;
            pFileName = (const char *) &(pUcodeInfo->pFileName[0]);
            if(AmbaFS_GetCodeMode() == AMBA_FS_UNICODE) {
                WCHAR mode[4] = {'r','\0'};
                WCHAR fnu[30];

                AmbaUtility_Ascii2Unicode(pFileName, (UINT16 *)fnu);
                AmbaFS_Stat((char *)fnu, &fstat);
                fp = AmbaFS_fopen((char *)fnu, (char *)mode);
            } else {
                AmbaFS_Stat((char *)pFileName, &fstat);
                fp = AmbaFS_fopen(pFileName, "r");
            }
            if(fp == NULL) {
              RetStatus = NG;
              AmbaPrint("No uCode in C drive %s",pFileName);
              break;
            }

            DataSize = fstat.Size;
            sz = AmbaFS_fread((void *) pUcodeInfo->BaseAddr, 1, DataSize, fp);
            if(sz != DataSize) {
                int err;
                AmbaFS_GetError(&err);
                AmbaPrint("AmbaFs_GetErr %d",err);
            }
            AmbaFS_fclose(fp);

            AmbaPrint("Load %s to 0x%08x, size = %d, ret %lld", pFileName, pUcodeInfo->BaseAddr, DataSize, sz);
        }
    }
    
    if (RetStatus == OK) {
        UcodeLoaded =0xFF;
    }

    return RetStatus;
}

