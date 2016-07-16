/**
 *  @file FirmwareLoader.c
 *
 *  format common function implementation
 *
 *  **History**
 *      |Date       |Name        |Comments       |
 *      |-----------|------------|---------------|
 *      |2014/09/15 |clchan      |Created        |
 *
 *
 *  @copyright 2014 Ambarella Corporation. All rights reserved.
 *                 No part of this file may be reproduced, stored in a retrieval system,
 *                 or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                 recording, or otherwise, without the prior consent of Ambarella Corporation.
 */
#include <FirmwareLoader.h>
#include <AmbaKAL.h>
#include <AmbaPrintk.h>
#include <stdio.h>
#include <AmbaROM.h>
#include <AmbaFS.h>
#include <AmbaCardManager.h>
#include <AmbaSysCtrl.h>
#include <AmbaDSP.h>
#include <AmbaFwLoader.h>

#define Fwld_Perror(level, color, fmt, ...) {\
    char Buffer[160];\
    snprintf(Buffer, 160, fmt, ##__VA_ARGS__);\
    Buffer[159] = '\0';\
    AmbaPrintColor(color, "[Error]%s:%u %s", __FUNCTION__, __LINE__, Buffer);\
}

#define FWLD_MAX_REGION         9       /**< hard limit : 32 */
#define FWLD_TASK_PRIORITY      10      /**< the task priority */

/** seq : the sequence of the firmware */
#define FWLD_STA_REGION_DONE(seq)   (0x00000001 << ((seq) + 1))

#define ORCCODE_REGION_LVIEW_OFFSET 0x020000
#define ORCCODE_REGION_ENC_OFFSET   0x150000
#define ORCCODE_REGION_DEC_OFFSET   0x2F0000

#define ORCME_REGION_LVIEW_OFFSET   0x08000
#define ORCME_REGION_ENC_OFFSET     0x30000
#define ORCME_REGION_DEC_OFFSET     0x90000 // it is very small, skip it?

#define UCODE_FILE_AMOUNT           3   // A12 has only 3 ucode files

typedef struct _AMBA_DSP_UCODE_CTRL_s_ {
    char    pFileName[33];
    UINT32  BaseAddr;
} AMBA_DSP_UCODE_CTRL_s;

/**
 * interface of firmware loading functions
 */
typedef int (*LOAD_FW_FP)(UINT32);

typedef struct {
    UINT8 Id;
    UINT8 Param;
    LOAD_FW_FP Load;
} FIRMWARE_INFO_s;

/**
 * the definition of the firmware loader
 */
typedef struct {
    BOOL Init;                 /**< has been initoiated */
    BOOL FromCard;
    AMBA_KAL_TASK_t Task;       /**< task > */
    AMBA_KAL_EVENT_FLAG_t Flag; /**< event flag */
} FIRMWARE_LOADER_s;

static int UcodeLoader_Load(UINT32 region);

static AMBA_DSP_UCODE_CTRL_s g_DspUcodeCtrl[UCODE_FILE_AMOUNT] = {
    {"c:\\uCode\\orccode.bin", 0},
    {"c:\\uCode\\orcme.bin",   0},
    {"c:\\uCode\\default_binary.bin", 0},
};

static FIRMWARE_INFO_s g_FwInfo[FWLD_MAX_REGION] = {
    {AMP_KERNEL_REGION1, 1, AmbaFwLoader_Load},
    {AMP_UCODE_REGION_BOOT, AMP_UCODE_REGION_BOOT, (LOAD_FW_FP)UcodeLoader_Load},
    {AMP_KERNEL_REGION2, 2, AmbaFwLoader_Load},
    {AMP_UCODE_REGION_LVIEW, AMP_UCODE_REGION_LVIEW, (LOAD_FW_FP)UcodeLoader_Load},
    {AMP_KERNEL_REGION3, 3, AmbaFwLoader_Load},
    {AMP_UCODE_REGION_ENC, AMP_UCODE_REGION_ENC, (LOAD_FW_FP)UcodeLoader_Load},
    {AMP_KERNEL_REGION4, 4, AmbaFwLoader_Load},
    {AMP_UCODE_REGION_DEC, AMP_UCODE_REGION_DEC, (LOAD_FW_FP)UcodeLoader_Load},
    {AMP_KERNEL_REGION5, 5, AmbaFwLoader_Load}
};

/**
 * the main object of the firmware loader
 */
static FIRMWARE_LOADER_s g_FwLoader = {0};

static int UcodeLoader_LoadFromCard(void)
{
    UINT32 i;
    AMBA_SCM_STATUS_s Status;

    /* Wait SD card initial. */
    do {
        if (AmbaSCM_GetSlotStatus(SCM_SLOT_SD0, &Status) < 0) {
            Fwld_Perror(0, 0, "AmbaSCM_GetSlotStatus() failed");
            return NG;
        }
        /* continue to delay 1 msec to check SD ready to load ucode from SD card */
        AmbaKAL_TaskSleep(1);
    } while (Status.CardPresent && (Status.Format == -1));
    for (i = 0; i < UCODE_FILE_AMOUNT; i++) {
        int RetStatus = NG;
        AMBA_DSP_UCODE_CTRL_s * const UcodeInfo = &g_DspUcodeCtrl[i];
        char * const FileName = UcodeInfo->pFileName;
        AMBA_FS_FILE * const File = AmbaFS_fopen(FileName, "r+b");
        if (File != NULL) {
            if (AmbaFS_fseek(File, 0, AMBA_FS_SEEK_END) == 0) {
                const INT64 DataSize = AmbaFS_ftell(File);
                if (DataSize > 0) {
                    if (AmbaFS_fseek(File, 0, AMBA_FS_SEEK_START) == 0) {
                        if (AmbaFS_fread((void *)UcodeInfo->BaseAddr, DataSize, 1, File) == 1) {
                            AmbaPrint("Load %s to 0x%08x, size = %d", FileName, UcodeInfo->BaseAddr, DataSize);
                            RetStatus = OK;
                        } else {
                            Fwld_Perror(0, 0, "AmbaFS_fread() failed");
                        }
                    } else {
                        Fwld_Perror(0, 0, "AmbaFS_fseek() failed");
                    }
                } else {
                    Fwld_Perror(0, 0, "incorrect file size");
                }
            } else {
               Fwld_Perror(0, 0, "AmbaFS_fseek() failed");
            }
            AmbaFS_fclose(File);
        } else {
            Fwld_Perror(0, 0, "AmbaFS_fopen() failed");
        }
        if (RetStatus == NG) {
            return NG;
        }
    }
    return OK;
}

static inline int UcodeLoader_LoadPartial(AMBA_DSP_UCODE_CTRL_s *ucodeInfo, UINT32 targetAddress, UINT32 ucodeSize, UINT32 ucodePos)
{
    char * const FileName = &ucodeInfo->pFileName[9];
    K_ASSERT(ucodeSize > 0);
    /* check to see if there are uCode binary files on NAND flash memory */
    if (AmbaROM_LoadByName(AMBA_ROM_DSP_UCODE, FileName, (UINT8 *)targetAddress, ucodeSize, ucodePos) < ucodeSize) {
        Fwld_Perror(0, 0, "AmbaROM_LoadByName() failed");
        return NG;
    }
    AmbaPrint("Load %s to 0x%08x, size = %x", FileName, targetAddress, ucodeSize);
    return OK;
}

static int UcodeLoader_Load(UINT32 region)
{
    if (AmbaSysIsWarmBoot() == FALSE) {
        int UcodeSize;
        AMBA_DSP_UCODE_CTRL_s *UcodeInfo;
        UINT32 TargetAddress;
        /* load uCode from NAND flash */
        switch (region){
        case AMP_UCODE_REGION_BOOT:
            if (g_FwLoader.FromCard) {
                /* load uCode from SD card */
                if (UcodeLoader_LoadFromCard() != OK) {
                    return NG;
                }
            } else {
                // load orccode
                UcodeInfo = &g_DspUcodeCtrl[0];
                UcodeSize = ORCCODE_REGION_LVIEW_OFFSET;
                TargetAddress = UcodeInfo->BaseAddr;
                if (UcodeLoader_LoadPartial(UcodeInfo, TargetAddress, UcodeSize, 0) != OK) {
                    return NG;
                }
                // load orcme
                UcodeInfo = &g_DspUcodeCtrl[1];
                UcodeSize = ORCME_REGION_LVIEW_OFFSET;
                TargetAddress = UcodeInfo->BaseAddr;
                if (UcodeLoader_LoadPartial(UcodeInfo, TargetAddress, UcodeSize, 0) != OK) {
                    return NG;
                }
                // load default
                UcodeInfo = &g_DspUcodeCtrl[2];
                UcodeSize = AmbaROM_GetSize(AMBA_ROM_DSP_UCODE, &UcodeInfo->pFileName[9], 0);
                if (UcodeSize < 0) {
                    Fwld_Perror(0, 0, "AmbaROM_GetSize() failed");
                    return NG;
                }
                TargetAddress = UcodeInfo->BaseAddr;
                if (UcodeLoader_LoadPartial(UcodeInfo, TargetAddress, UcodeSize, 0) != OK) {
                    return NG;
                }
            }
            break;
        case AMP_UCODE_REGION_LVIEW:
            // load orccode
            UcodeInfo = &g_DspUcodeCtrl[0];
            UcodeSize = ORCCODE_REGION_ENC_OFFSET - ORCCODE_REGION_LVIEW_OFFSET;
            TargetAddress = UcodeInfo->BaseAddr + ORCCODE_REGION_LVIEW_OFFSET;
            if (UcodeLoader_LoadPartial(UcodeInfo, TargetAddress, UcodeSize, ORCCODE_REGION_LVIEW_OFFSET) != OK) {
                return NG;
            }
            // load orcme
            UcodeInfo = &g_DspUcodeCtrl[1];
            UcodeSize = ORCME_REGION_ENC_OFFSET - ORCME_REGION_LVIEW_OFFSET;
            TargetAddress = UcodeInfo->BaseAddr + ORCME_REGION_LVIEW_OFFSET;
            if (UcodeLoader_LoadPartial(UcodeInfo, TargetAddress, UcodeSize, ORCME_REGION_LVIEW_OFFSET) != OK) {
                return NG;
            }
            break;
        case AMP_UCODE_REGION_ENC:
            // load orccode
            UcodeInfo = &g_DspUcodeCtrl[0];
            UcodeSize = ORCCODE_REGION_DEC_OFFSET - ORCCODE_REGION_ENC_OFFSET;
            TargetAddress = UcodeInfo->BaseAddr + ORCCODE_REGION_ENC_OFFSET;
            if (UcodeLoader_LoadPartial(UcodeInfo, TargetAddress, UcodeSize, ORCCODE_REGION_ENC_OFFSET) != OK) {
                return NG;
            }
            // load orcme
            UcodeInfo = &g_DspUcodeCtrl[1];
            UcodeSize = AmbaROM_GetSize(AMBA_ROM_DSP_UCODE, &UcodeInfo->pFileName[9], 0);
            if (UcodeSize < 0) {
                Fwld_Perror(0, 0, "AmbaROM_GetSize() failed");
                return NG;
            }
            if (UcodeSize > ORCME_REGION_DEC_OFFSET) {
                UcodeSize = ORCME_REGION_DEC_OFFSET - ORCME_REGION_ENC_OFFSET;
            } else {
                UcodeSize -= ORCME_REGION_ENC_OFFSET;
            }
            TargetAddress = UcodeInfo->BaseAddr + ORCME_REGION_ENC_OFFSET;
            if (UcodeLoader_LoadPartial(UcodeInfo, TargetAddress, UcodeSize, ORCME_REGION_ENC_OFFSET) != OK) {
                return NG;
            }
            break;
        case AMP_UCODE_REGION_DEC:
            // load orccode
            UcodeInfo = &g_DspUcodeCtrl[0];
            UcodeSize = AmbaROM_GetSize(AMBA_ROM_DSP_UCODE, &UcodeInfo->pFileName[9], 0);
            if (UcodeSize < 0) {
                Fwld_Perror(0, 0, "AmbaROM_GetSize() failed");
                return NG;
            }
            K_ASSERT(UcodeSize > ORCCODE_REGION_DEC_OFFSET);
            UcodeSize -= ORCCODE_REGION_DEC_OFFSET;
            TargetAddress = UcodeInfo->BaseAddr + ORCCODE_REGION_DEC_OFFSET;
            if (UcodeLoader_LoadPartial(UcodeInfo, TargetAddress, UcodeSize, ORCCODE_REGION_DEC_OFFSET) != OK) {
                return NG;
            }
            // load orcme
            UcodeInfo = &g_DspUcodeCtrl[1];
            UcodeSize = AmbaROM_GetSize(AMBA_ROM_DSP_UCODE, &UcodeInfo->pFileName[9], 0);
            if (UcodeSize < 0) {
                Fwld_Perror(0, 0, "AmbaROM_GetSize() failed");
                return NG;
            }
            if (UcodeSize > ORCME_REGION_DEC_OFFSET) {
                UcodeSize -= ORCME_REGION_DEC_OFFSET;
                TargetAddress = UcodeInfo->BaseAddr + ORCME_REGION_DEC_OFFSET;
                if (UcodeLoader_LoadPartial(UcodeInfo, TargetAddress, UcodeSize, ORCME_REGION_DEC_OFFSET) != OK) {
                    return NG;
                }
            }
            break;
        default:
            Fwld_Perror(0, 0, "invalid region");
            return NG;
        }
    }
    return OK;
}

static inline int FirmwareLoader_GetFwSeqNum(UINT8 id)
{
    UINT32 i;
    for (i=0; i<FWLD_MAX_REGION; i++) {
        if (g_FwInfo[i].Id == id) {
            return i;
        }
    }
    return -1;
}

/**
 * the main loop to load all firmwares
 * @return 0 => success, -1 => error
 */
static void FirmwareLoader_Run(UINT32 param)
{
    UINT32 i;
    AmbaPrint("**** FWLD START ****");
    if (AmbaFwLoader_Init() < 0) {
        Fwld_Perror(0, 0, "AmbaFwLoader_Init() failed");
        return;
    }
    for (i=0; i<FWLD_MAX_REGION; i++) {
        // load the firmware
        AmbaPrint("%s() : load the %u-th region, param=%u", __FUNCTION__, i, g_FwInfo[i].Param);
        if (g_FwInfo[i].Load(g_FwInfo[i].Param) < 0) {
            Fwld_Perror(0, 0, "load region %u failed", g_FwInfo[i].Id);
            return;
        }
        AmbaPrint("%s() : load the %u-th region done", __FUNCTION__, i);
        // notify the firmware loaded event
        if (AmbaKAL_EventFlagGive(&g_FwLoader.Flag, FWLD_STA_REGION_DONE(i)) != OK) {
            Fwld_Perror(0, 0, "give flag failed");
            return;
        }
    }
    if (AmbaFwLoader_Finish() < 0) {
        Fwld_Perror(0, 0, "AmbaFwLoader_Finish() failed");
        return;
    }
    AmbaPrint("**** FWLD DONE ****");
}

int AmpFwLoader_Init(UINT8 *stack, UINT32 size)
{
    if (g_FwLoader.Init == FALSE) {
        if (AmbaKAL_EventFlagCreate(&g_FwLoader.Flag) == OK) {
            UINT32 CodeAddr, MeAddr, MdxfAddr, DefaultDataAddr;
            AmbaDSP_GetUcodeBaseAddr(&CodeAddr, &MeAddr, &MdxfAddr, &DefaultDataAddr);
            g_DspUcodeCtrl[0].BaseAddr = CodeAddr;
            g_DspUcodeCtrl[1].BaseAddr = MeAddr;
            g_DspUcodeCtrl[2].BaseAddr = DefaultDataAddr;
            if (AmbaROM_GetSize(AMBA_ROM_DSP_UCODE, &g_DspUcodeCtrl[0].pFileName[9], 0) < 0) {
                AmbaPrint("Load uCode from SD card");
                g_FwLoader.FromCard = TRUE;
            }
            if (g_FwLoader.FromCard) {
                // prfile is put to region 4, so load ucode after region 4
                g_FwInfo[1].Id = AMP_KERNEL_REGION2;
                g_FwInfo[1].Param = 2;
                g_FwInfo[1].Load = AmbaFwLoader_Load;
                g_FwInfo[2].Id = AMP_KERNEL_REGION3;
                g_FwInfo[2].Param = 3;
                g_FwInfo[2].Load = AmbaFwLoader_Load;
                g_FwInfo[3].Id = AMP_KERNEL_REGION4;
                g_FwInfo[3].Param = 4;
                g_FwInfo[3].Load = AmbaFwLoader_Load;
                g_FwInfo[4].Id = g_FwInfo[4].Param = AMP_UCODE_REGION_BOOT;
                g_FwInfo[4].Load = (LOAD_FW_FP)UcodeLoader_Load;
                g_FwInfo[5].Id = g_FwInfo[5].Param = AMP_UCODE_REGION_LVIEW;
                g_FwInfo[5].Load = (LOAD_FW_FP)UcodeLoader_Load;
                g_FwInfo[6].Id = g_FwInfo[6].Param = AMP_UCODE_REGION_ENC;
                g_FwInfo[6].Load = (LOAD_FW_FP)UcodeLoader_Load;
            }
            if (AmbaKAL_TaskCreate(&g_FwLoader.Task, "COMSVC_FirmwareLoader", FWLD_TASK_PRIORITY, FirmwareLoader_Run, 0, stack, size, AMBA_KAL_AUTO_START) == OK) {
                g_FwLoader.Init = TRUE;
                return 0;
            }
            Fwld_Perror(0, 0, "create task failed");
            AmbaKAL_EventFlagDelete(&g_FwLoader.Flag);
        } else {
            Fwld_Perror(0, 0, "create flag failed");
        }
    } else {
        Fwld_Perror(0, 0, "already initiated");
    }
    return -1;
}

int AmpFwLoader_Wait(UINT32 id, UINT32 timeOut)
{
    const int SeqNum = FirmwareLoader_GetFwSeqNum(id);
    UINT32 Flag;
    K_ASSERT(g_FwLoader.Init);
    if (SeqNum < 0) {
        return -1;
    }
    if (AmbaKAL_EventFlagTake(&g_FwLoader.Flag, FWLD_STA_REGION_DONE(SeqNum), AMBA_KAL_OR, &Flag, timeOut) != OK) {
        Fwld_Perror(0, 0, "take flag failed");
        return -1;
    }
    return 0;
}

int AmpFwLoader_WaitComplete(UINT32 timeOut)
{
    int rval = 0;
    AMBA_KAL_TASK_INFO_s Info;
    K_ASSERT(g_FwLoader.Init);
    while (TRUE) {
        if (AmbaKAL_TaskQuery(&g_FwLoader.Task, &Info) != OK) {
            Fwld_Perror(0, 0, "query task failed");
            rval = -1;
            break;
        }
        if (Info.CurState == TX_COMPLETED)
            break;
        AmbaKAL_TaskSleep(50);
    }
    if (AmbaKAL_TaskDelete(&g_FwLoader.Task) != OK) {
        Fwld_Perror(0, 0, "delete task failed");
        rval = -1;
    }
    return rval;
}

int AmpFwLoader_Suspend(void)
{
    K_ASSERT(g_FwLoader.Init);
    if (AmbaKAL_TaskSuspend(&g_FwLoader.Task) != OK) {
        Fwld_Perror(0, 0, "suspend task failed");
        return -1;
    }
    return 0;
}

int AmpFwLoader_Resume(void)
{
    K_ASSERT(g_FwLoader.Init);
    if (AmbaKAL_TaskResume(&g_FwLoader.Task) != OK) {
        Fwld_Perror(0, 0, "resume task failed");
        return -1;
    }
    return 0;
}

