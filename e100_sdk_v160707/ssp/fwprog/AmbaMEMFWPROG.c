/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaNAND_Prog.c
 *
 *  @Copyright      :: Copyright (C) 2012 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: In memory NAND flash firmware program utilities APIs
 *
 *  @History        ::
 *      Date        Name        Comments
 *      09/03/2012  Kerson      Created
 *      11/06/2013  W.Shi       Replaced g_PartStr[] with AmbaNAND_PartitionName[]
 *      04/18/2013  W.Shi       Renamed AmbaRTSL_I2sEnableFdma to AmbaRTSL_FdmaEnable
 *
\*-------------------------------------------------------------------------------------------------------------------*/

#include <stdio.h>
#include <string.h>

#include "AmbaDataType.h"
#include "AmbaHwIO.h"

#include "AmbaSD_Def.h"
#include "AmbaNAND_Def.h"
#include "AmbaNOR_Def.h"
#include "AmbaRTSL_NAND.h"
#include "AmbaNAND_PartitionTable.h"
#include "AmbaNOR_PartitionTable.h"
#include "AmbaNonOS_NAND.h"
#include "AmbaNonOS_EMMC.h"
#include "AmbaNonOS_NOR.h"
#include "AmbaNonOS_NandBBT.h"
#include "AmbaNonOS_UART.h"
#include "AmbaFWPROG.h"
#include "AmbaNORFWPROG.h"
#include "AmbaCortexA9.h"
#include "AmbaGPIO.h"
#include "AmbaRTSL_PLL.h"
#include "AmbaRTSL_FIO.h"
#include "AmbaRTSL_I2S.h"
#include "AmbaRTSL_MMU.h"
#include "AmbaRTSL_GPIO.h"

extern UINT32 begin_bst_image;
extern UINT32 end_bst_image;
extern UINT32 begin_bld_image;
extern UINT32 end_bld_image;
extern UINT32 begin_pba_image;
extern UINT32 end_pba_image;
extern UINT32 begin_sys_image;
extern UINT32 end_sys_image;
extern UINT32 begin_rom_image;
extern UINT32 end_rom_image;
extern UINT32 begin_dsp_image;
extern UINT32 end_dsp_image;
extern UINT32 begin_lnx_image;
extern UINT32 end_lnx_image;
extern UINT32 begin_dtb_image;
extern UINT32 end_dtb_image;
extern UINT32 begin_rfs_image;
extern UINT32 end_rfs_image;

UINT32 FirmwareStart = 0;
UINT32 BstImageBegin = 0;
UINT32 BstImageEnd = 0;
UINT32 BldImageBegin = 0;
UINT32 BldImageEnd = 0;
UINT32 PbaImageBegin = 0;
UINT32 PbaImageEnd = 0;
UINT32 SysImageBegin = 0;
UINT32 SysImageEnd = 0;
UINT32 DspImageBegin = 0;
UINT32 DspImageEnd = 0;
UINT32 RomImageBegin = 0;
UINT32 RomImageEnd = 0;
UINT32 LnxImageBegin = 0;
UINT32 LnxImageEnd = 0;
UINT32 DtbImageBegin = 0;
UINT32 DtbImageEnd = 0;
UINT32 RfsImageBegin = 0;
UINT32 RfsImageEnd = 0;

#if defined(CONFIG_EMMC_ACCPART_USER)
#define EMMC_ACCESS_PART	EMMC_ACCP_USER
#elif defined(CONFIG_EMMC_ACCPART_BP1)
#define EMMC_ACCESS_PART	EMMC_ACCP_BP_1
#else
#define EMMC_ACCESS_PART	EMMC_ACCP_BP_2
#endif

#if defined(CONFIG_EMMC_BOOTPART_USER)
#define EMMC_BOOT_PART		EMMC_BOOTP_USER
#elif defined(CONFIG_EMMC_BOOTPART_BP1)
#define EMMC_BOOT_PART		EMMC_BOOTP_BP_1
#else
#define EMMC_BOOT_PART		EMMC_BOOTP_BP_2
#endif

#if defined(CONFIG_EMMC_BOOT_1BIT)
#define EMMC_BOOT_BUS	(EMMC_BOOT_1BIT | EMMC_BOOT_HIGHSPEED)
#elif defined(CONFIG_EMMC_BOOT_4BIT)
#define EMMC_BOOT_BUS	(EMMC_BOOT_4BIT | EMMC_BOOT_HIGHSPEED)
#else
#define EMMC_BOOT_BUS	(EMMC_BOOT_8BIT | EMMC_BOOT_HIGHSPEED)
#endif

AMBA_MMU_USER_CTRL_s AmbaMmuUserCtrl = {
        .MemMapInit      = NULL,
        .pMemMapInfo     = NULL,
        .MemMapNumRegion = 0,
        .HighVectFlag    = 0,
};

void stop(void);

extern const char *AmbaNAND_PartitionName[];
extern AMBA_NAND_DEV_INFO_s AmbaNAND_DevInfo;
extern AMBA_NOR_DEV_INFO_s AmbaNOR_DevInfo;

extern AMBA_GPIO_PIN_GROUP_CONFIG_s GpioPinGrpConfig;
extern unsigned int __memfwprog_result;
extern unsigned int __memfwprog_command;
extern int AmbaNonOS_EmmcEnableHWReset(void);
static const char *MEMFWPROG_LOGO =
    "\r\n" \
    "----------------------------------------------------------------\r\n"  \
    " NAND Flash Programming Utility\r\n"                                    \
    " Copyright (C) 2012 Ambarella Corporation. All rights reserved.\r\n"    \
    "----------------------------------------------------------------\r\n";

static void OutputHeader(AMBA_IMG_HEADER *pHeader, UINT32 Len)
{
    char PrintBuf[64];

    snprintf(PrintBuf, sizeof(PrintBuf), "\r\tCrc32  :\t0x%x\n", pHeader->Crc32);
    AmbaNonOS_UartPrintf(PrintBuf, sizeof(PrintBuf));
    snprintf(PrintBuf, sizeof(PrintBuf), "\r\tVersion:\t%d.%d.%d\n", pHeader->VerNum >> 24, (pHeader->VerNum >> 16) & 0xff, pHeader->VerNum & 0xffff);
    AmbaNonOS_UartPrintf(PrintBuf, sizeof(PrintBuf));
    snprintf(PrintBuf, sizeof(PrintBuf), "\r\tVerDate:\t%d/%d/%d\n", pHeader->VerDate >> 16, (pHeader->VerDate >> 8) & 0xff, pHeader->VerDate & 0xff);
    AmbaNonOS_UartPrintf(PrintBuf, sizeof(PrintBuf));
    snprintf(PrintBuf, sizeof(PrintBuf), "\r\tImgLen :\t%d\n", pHeader->ImgLen);
    AmbaNonOS_UartPrintf(PrintBuf, sizeof(PrintBuf));
    snprintf(PrintBuf, sizeof(PrintBuf), "\r\tMemAddr:\t0x%x\n", pHeader->MemAddr);
    AmbaNonOS_UartPrintf(PrintBuf, sizeof(PrintBuf));
    snprintf(PrintBuf, sizeof(PrintBuf), "\r\tFlag   :\t0x%x\n", (pHeader->Flag & 0xf));
    AmbaNonOS_UartPrintf(PrintBuf, sizeof(PrintBuf));
    snprintf(PrintBuf, sizeof(PrintBuf), "\r\tMagic  :\t0x%x\n", pHeader->Magic);
    AmbaNonOS_UartPrintf(PrintBuf, sizeof(PrintBuf));
}

static void OutputFailure(int ErrCode)
{
    if (ErrCode >= 0) {
        AmbaNonOS_UartPrintf("\r\nprogram ok\n\n");
    } else {
        AmbaNonOS_UartPrintf("\r\nfailed\n");
    }
}

static void OutputReport(const char *pName, int Flag)
{
    AmbaNonOS_UartPrintf("\r%-26s:\t", pName);

    if ((Flag & FWPROG_RESULT_FLAG_CODE_MASK) == 0)
        AmbaNonOS_UartPrintf("success\n");
    else
        AmbaNonOS_UartPrintf("FAILED!\n");
}

int AmbaFWPROG_ProgPart(int PartID, UINT8 *pImage, UINT32 Len)
{
#ifdef CONFIG_ENABLE_EMMC_BOOT
    return AmbaFWPROG_EmmcPart(PartID, pImage, Len);
#elif defined(CONFIG_ENABLE_SPINOR_BOOT)
    return AmbaFWPROG_NorPart(PartID, pImage, Len);
#else
    return AmbaFWPROG_NandPart(PartID, pImage, Len);
#endif
}

/* FIXME:  */
#define AMBA_CORTEX_DRAM_BASE_ADDR       0x0

int main(void)
{
    int i, RetVal = 0;
    UINT8 *pImage;
    UINT32 PartLen[HAS_IMG_PARTS], ImageStart[HAS_IMG_PARTS];
    UINT8 Buffer[512], Code;
    char PrintBuf[64];
    UINT32 FwResultAddr = (UINT32)(&__memfwprog_result);
    AMBA_FWPROG_RESULT_s Result;
#ifdef _AMBA_SIGPRINT_
    extern volatile UINT32 AmbaPrintTarget;

    AmbaPrintTarget = AMBA_PRINTF_TARGET_CHAMELEON;
#endif
#ifdef CONFIG_ENABLE_EMMC_BOOT
    UINT8 accp;
#endif

    for (i = 0; i < HAS_IMG_PARTS; i++) {
        PartLen[i] = ImageStart[i] = 0;
    }

    AmbaRTSL_PllInit(24000000, NULL);       /* PLL RTSL Initializations */
    AmbaRTSL_PllSetCoreClk(216000000);      /* set Core Clock */
    AmbaRTSL_PllSetIdspClk(264000000);      /* set IDSP Clock */
    AmbaRTSL_PllSetCortexClk(504000000);    /* set Coretex Clock */

    AmbaRTSL_GpioInit(&GpioPinGrpConfig);
    AmbaRTSL_I2sEnableFdma();
    AmbaRTSL_FioResetAll();
    AmbaRTSL_FioExitRandomMode();

    /* Initialize the UART */
    AmbaNonOS_UartInit();

    AmbaNonOS_UartPrintf("\x1b[4l"); /* Set terminal to replacement mode */
    AmbaNonOS_UartPrintf("\r\n");    /* First, output a blank line to UART */

    AmbaNonOS_UartPrintf(MEMFWPROG_LOGO, strlen(MEMFWPROG_LOGO));

#if defined(CONFIG_ENABLE_EMMC_BOOT)
    AmbaNonOS_BootDeviceInit();
#elif defined(CONFIG_ENABLE_SPINOR_BOOT)
    AmbaNonOS_NorInit(&AmbaNOR_DevInfo);
#else
    AmbaNonOS_NandInit(&AmbaNAND_DevInfo);
#endif

    memset(Buffer, 0x0, sizeof(Buffer));
    memset(&Result, 0x0, sizeof(Result));
    Result.Magic = FWPROG_RESULT_MAGIC;

    BstImageBegin   = (UINT32) begin_bst_image;
    BldImageBegin   = (UINT32) begin_bld_image;
    PbaImageBegin   = (UINT32) begin_pba_image;
    SysImageBegin   = (UINT32) begin_sys_image;
    DspImageBegin   = (UINT32) begin_dsp_image;
    RomImageBegin   = (UINT32) begin_rom_image;
    LnxImageBegin   = (UINT32) begin_lnx_image;
    DtbImageBegin   = (UINT32) begin_dtb_image;
    RfsImageBegin   = (UINT32) begin_rfs_image;

    BstImageEnd     = (UINT32) end_bst_image;
    BldImageEnd     = (UINT32) end_bld_image;
    PbaImageEnd     = (UINT32) end_pba_image;
    SysImageEnd     = (UINT32) end_sys_image;
    DspImageEnd     = (UINT32) end_dsp_image;
    RomImageEnd     = (UINT32) end_rom_image;
    LnxImageEnd     = (UINT32) end_lnx_image;
    DtbImageEnd     = (UINT32) end_dtb_image;
    RfsImageEnd     = (UINT32) end_rfs_image;

    /* Calculate the firmware payload offsets of images */
    ImageStart[AMBA_PARTITION_BOOTSTRAP]                  = BstImageBegin;
    ImageStart[AMBA_PARTITION_TABLE]                      = 0;
    ImageStart[AMBA_PARTITION_BOOTLOADER]                 = BldImageBegin;
    ImageStart[AMBA_PARTITION_SD_FIRMWARE_UPDATE]         = PbaImageBegin;
    ImageStart[AMBA_PARTITION_SYS_SOFTWARE]               = SysImageBegin;
    ImageStart[AMBA_PARTITION_DSP_uCODE]                  = DspImageBegin;
    ImageStart[AMBA_PARTITION_SYS_ROM_DATA]               = RomImageBegin;

    ImageStart[AMBA_PARTITION_LINUX_KERNEL]               = LnxImageBegin;
    ImageStart[AMBA_PARTITION_LINUX_ROOT_FS]              = RfsImageBegin;

    PartLen[AMBA_PARTITION_BOOTSTRAP]                     = BstImageEnd - BstImageBegin;
    PartLen[AMBA_PARTITION_TABLE]                         = 0;
    PartLen[AMBA_PARTITION_BOOTLOADER]                    = BldImageEnd - BldImageBegin;
    PartLen[AMBA_PARTITION_SD_FIRMWARE_UPDATE]            = PbaImageEnd - PbaImageBegin;;
    PartLen[AMBA_PARTITION_SYS_SOFTWARE]                  = SysImageEnd - SysImageBegin;
    PartLen[AMBA_PARTITION_DSP_uCODE]                     = DspImageEnd - DspImageBegin;;
    PartLen[AMBA_PARTITION_SYS_ROM_DATA]                  = RomImageEnd - RomImageBegin;

    PartLen[AMBA_PARTITION_LINUX_KERNEL]                  = LnxImageEnd - LnxImageBegin;
    PartLen[AMBA_PARTITION_LINUX_ROOT_FS]                 = RfsImageEnd - RfsImageBegin;

    i = AMBA_PARTITION_BOOTSTRAP;

    if (PartLen[i] > 0) {
        snprintf(PrintBuf, sizeof(PrintBuf), "\r\n %s code found in firmware!\n", AmbaNAND_PartitionName[i]);
        AmbaNonOS_UartPrintf(PrintBuf, sizeof(PrintBuf));

        pImage = (UINT8 *) FirmwareStart + ImageStart[i];

        OutputHeader((AMBA_IMG_HEADER *) pImage, PartLen[i]);

#ifdef CONFIG_ENABLE_EMMC_BOOT
        accp = (EMMC_BOOT_PART == EMMC_BOOTP_USER) ? (EMMC_ACCP_USER) :
        (EMMC_BOOT_PART == EMMC_BOOTP_BP_1) ? (EMMC_ACCP_BP_1) :
        (EMMC_ACCP_BP_2);

        RetVal = AmbaNonOS_EmmcSetBootConfig(EMMC_BOOT_PART, accp, EMMC_BOOT_BUS);
        if (RetVal != 0){
           AmbaNonOS_UartPrintf("\r\n Set EMMC_BOOT_CONFIG failed \r\n");
           return FLPROG_ERR_PROG_IMG;
        }
#endif
#ifdef CONFIG_ENABLE_EMMC_BOOT
#ifdef CONFIG_ENABLE_EMMC_HW_RESET
        RetVal = AmbaNonOS_EmmcEnableHWReset();
        if (RetVal != 0)
            AmbaNonOS_UartPrintf("\r\n Enable HW-Reset feature of EMMC failed\r\n");
#endif
#endif
        RetVal = AmbaFWPROG_ProgPart(AMBA_PARTITION_BOOTSTRAP, pImage, PartLen[i]);
        Code = RetVal < 0 ? -RetVal : RetVal;
        Result.Flag[i] = FWPROG_RESULT_MAKE(Code, PartLen[i]);
        if (RetVal == FLPROG_ERR_PROG_IMG)
            Result.BadBlockInfo |= (0x1 << i);

        OutputFailure(RetVal);
#ifdef CONFIG_ENABLE_EMMC_BOOT
        if (EMMC_BOOT_PART != EMMC_BOOTP_USER) {
            RetVal = AmbaNonOS_EmmcSetBootConfig(EMMC_BOOT_PART, EMMC_ACCP_USER, EMMC_BOOT_BUS);
            if (RetVal != 0){
                AmbaNonOS_UartPrintf("\r\n Set EMMC_BOOT_CONFIG failed \r\n");
                return FLPROG_ERR_PROG_IMG;
            }
        }
#endif
    }

    for (i = AMBA_PARTITION_BOOTLOADER; i < HAS_IMG_PARTS; i++) {
        if (PartLen[i] > 0) {
            snprintf(PrintBuf, sizeof(PrintBuf), "\r %s code found in firmware!\n", AmbaNAND_PartitionName[i]);
            AmbaNonOS_UartPrintf(PrintBuf, sizeof(PrintBuf));

            pImage = (UINT8 *)(FirmwareStart + ImageStart[i]);

            OutputHeader((AMBA_IMG_HEADER *)pImage, PartLen[i]);

            RetVal = AmbaFWPROG_ProgPart(i, pImage, PartLen[i]);

            Code = RetVal < 0 ? -RetVal : RetVal;
            Result.Flag[i] = FWPROG_RESULT_MAKE(Code, PartLen[i]);
            if (RetVal == FLPROG_ERR_PROG_IMG)
                Result.BadBlockInfo |= (0x1 << i);

            OutputFailure(RetVal);
        }
    }

    AmbaNonOS_UartPrintf("\r\n------ Report ------\r\n");
    for (i = 0; i < HAS_IMG_PARTS - 1; i++) {
        if (i == AMBA_PARTITION_TABLE || PartLen[i] == 0)
            continue;
        OutputReport(AmbaNAND_PartitionName[i], Result.Flag[i]);
    }

    AmbaNonOS_UartPrintf("\r\n\t- Program Terminated -\r\n");
    memcpy((void *) FwResultAddr, &Result, sizeof(Result));

#if 1 /* Customization start. */
    {
	    // Enable this if you want to verify the result with AmbaUSB.
	    // The address changes with different chips.
	    // For A12 and A9, it's 0x0.
	    // AmbaUSB will put its own BLD to this address.
	    void (*bld_start)(void) = 0x00000000;
	    AmbaNonOS_UartPrintf("\r\n\t- Re-enter USB Download Mode to Verify (addr 0x0) -\r\n");
	    bld_start();
    }
#endif /* Customization stop. */

    stop();

    return 0;
}
