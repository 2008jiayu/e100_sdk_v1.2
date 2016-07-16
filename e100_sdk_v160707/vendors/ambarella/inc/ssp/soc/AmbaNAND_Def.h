/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaNAND_Def.h
 *
 *  @Copyright      :: Copyright (C) 2012 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Definitions of Run Time Support Library for SD flash
\*-------------------------------------------------------------------------------------------------------------------*/

#ifndef _AMBA_NAND_DEF_H_
#define _AMBA_NAND_DEF_H_

#include "AmbaPartition_Def.h"
/*-----------------------------------------------------------------------------------------------*\
 * Firmware partitions.
\*-----------------------------------------------------------------------------------------------*/
/* Need removed */
typedef enum _AMBA_NAND_PARTITION_ID_e_ {
    AMBA_NAND_PARTITION_BOOTSTRAP = 0,              /* for Bootstrap */
    AMBA_NAND_PARTITION_TABLE,                      /* for Partition Table */
    AMBA_NAND_PARTITION_BOOTLOADER,                 /* for Bootloader */
    AMBA_NAND_PARTITION_SD_FIRMWARE_UPDATE,         /* for SD Firmware Update s/w */
    AMBA_NAND_PARTITION_SYS_SOFTWARE,               /* for System Software */
    AMBA_NAND_PARTITION_DSP_uCODE,                  /* for DSP uCode */
    AMBA_NAND_PARTITION_SYS_ROM_DATA,               /* for System ROM Data */

    /* optional */
    AMBA_NAND_PARTITION_LINUX_KERNEL,               /* optional for Linux Kernel */
    AMBA_NAND_PARTITION_LINUX_ROOT_FS,              /* optional for Linux Root File System */
    AMBA_NAND_PARTITION_LINUX_HIBERNATION_IMAGE,    /* optional for Linux Hibernation Image */

    /* without pre-built image */
    AMBA_NAND_PARTITION_STORAGE0,                   /* for FAT: Drive 'A' */
    AMBA_NAND_PARTITION_STORAGE1,                   /* for FAT: Drive 'B'; Slot-0: Drive 'C'; Slot-1: Drive 'D' */

    AMBA_NAND_PARTITION_VIDEO_REC_INDEX,            /* for Video Recording Index */
    AMBA_NAND_PARTITION_USER_SETTING,               /* for User Settings */
    AMBA_NAND_PARTITION_CALIBRATION_DATA,           /* for Calibration Data */

    AMBA_NUM_NAND_PARTITION,                        /* Total Number of NAND Partitions */

    /* Total Number of NAND Media Partitions */
    AMBA_NUM_NAND_MEDIA_PARTITION = AMBA_NUM_NAND_PARTITION - AMBA_NAND_PARTITION_STORAGE0,
} AMBA_NAND_PARTITION_ID_e;

/*-----------------------------------------------------------------------------------------------*\
 * Media partitions. (Must have the same order as PART_STORAGE ...)
 * These should synchronize with NFTL_ID_XXX in AmbaNFTL.h.
\*-----------------------------------------------------------------------------------------------*/
//typedef enum _AMBA_MEDIA_PARTITION_LAYOUT_e_ {
//    MP_Storage0 = 0,
//    MP_Storage1,
//    MP_IndexForVideoRecording,
//    MP_UserSetting,
//    MP_CalibrationData,

//    MP_MAX,                 /* Total number of media partitions. */

//    /* Reserved from 5. */
//    MP_RESERVED5 = MP_MAX,

//    MP_MAX_WITH_RSV         /* Total number of media partitions (incouding reserved). */
//} AMBA_MEDIA_PARTITION_LAYOUT_e;

/*-----------------------------------------------------------------------------------------------*\
 * Data structure describing a NAND flash device(s) configuration.
\*-----------------------------------------------------------------------------------------------*/
typedef enum _AMBA_NAND_SIZE_e_ {
    AMBA_NAND_SIZE_64M_BITS = 0,
    AMBA_NAND_SIZE_128M_BITS,
    AMBA_NAND_SIZE_256M_BITS,
    AMBA_NAND_SIZE_512M_BITS,
    AMBA_NAND_SIZE_1G_BITS,
    AMBA_NAND_SIZE_2G_BITS,
    AMBA_NAND_SIZE_4G_BITS,
    AMBA_NAND_SIZE_8G_BITS
} AMBA_NAND_SIZE_e;

typedef struct _AMBA_NAND_DEV_INFO_s_ {
    char    *pDevName;              /* pointer to the Device name */

    AMBA_NAND_SIZE_e    ChipSize;

    UINT16  PageSize;               /* Page size in Byte including Spare area size */
    UINT16  SpareSize;              /* Spare area size */
    UINT16  PagesPerBlock;          /* Pages per block */
    UINT16  BlocksPerPlane;         /* Blocks per plane */
    UINT16  BlocksPerZone;          /* Blocks per zone */
    UINT8   TotalZones;             /* Total zones */
    UINT8   TotalPlanes;            /* Total planes */

    UINT8   ColumnCycles;           /* Column access cycles */
    UINT8   PageCycles;             /* Page access cycles */
    UINT8   IdCycles;               /* ID access cycles */
    UINT8   EccBits;                /* ECC bits per 512Byte of device */
    UINT32  PlaneAddrMask;

    /** Chip(s) timing parameters */
    UINT8   tCLS;                   /* CLE Setup Time */
    UINT8   tALS;                   /* ALE Hold Time */
    UINT8   tCS;                    /* CEsetup Time */
    UINT8   tDS;                    /* Data setup Time */
    UINT8   tCLH;                   /* CLE Hold Time */
    UINT8   tALH;                   /* ALE Hold Time */
    UINT8   tCH;                    /* CE Hold Time */
    UINT8   tDH;                    /* Data Hold Time */
    UINT8   tWP;                    /* WE Pulse Width */
    UINT8   tWH;                    /* WE High Hold Time */
    UINT8   tWB;                    /* WE High to Busy */
    UINT8   tRR;                    /* Ready to RE Low */
    UINT8   tRP;                    /* RE Pulse Width */
    UINT8   tREH;                   /* RE High Hold Time */
    UINT8   tRB;                    /* The same as tWB */
    UINT8   tCEH;                   /* tRHZ - tCHZ     */
    UINT8   tREA;                   /* RE Access Time(tRDELAY) */
    UINT8   tCLR;                   /* CLE to REDelay */
    UINT8   tWHR;                   /* WE High to RELow */
    UINT8   tIR;                    /* Output Hi-Z to RELow */
    UINT8   tWW;                    /* Write Protection time */
    UINT8   tRHZ;                   /* RE High to Output Hi-Z */
    UINT8   tAR;                    /* ALE to REDelay */
} AMBA_NAND_DEV_INFO_s;

/*-----------------------------------------------------------------------------------------------*\
 *  NAND Flash ECC area
\*-----------------------------------------------------------------------------------------------*/
#define NAND_MAIN_ONLY                  0x0
#define NAND_SPARE_ONLY                 0x1
#define NAND_MAIN_ECC                   0x2
#define NAND_SPARE_ECC                  0x3
#define NAND_FULL_ECC                   0x4

#endif  /* _AMBA_NAND_DEF_H_ */
