  /*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaNOR_Def.h
 *
 *  @Copyright      :: Copyright (C) 2012 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Definitions of Run Time Support Library for SD flash
\*-------------------------------------------------------------------------------------------------------------------*/

#ifndef _AMBA_NOR_DEF_H_
#define _AMBA_NOR_DEF_H_

#include "AmbaPartition_Def.h"
/*-----------------------------------------------------------------------------------------------*\
 * Firmware partitions.
\*-----------------------------------------------------------------------------------------------*/
#if 0
typedef enum _AMBA_NOR_PARTITION_ID_e_ {
    AMBA_NOR_PARTITION_BOOTSTRAP = 0,              /* for Bootstrap */
    AMBA_NOR_PARTITION_TABLE,                      /* for Partition Table */
    AMBA_NOR_PARTITION_BOOTLOADER,                 /* for Bootloader */
    AMBA_NOR_PARTITION_SD_FIRMWARE_UPDATE,         /* for SD Firmware Update s/w */
    AMBA_NOR_PARTITION_SYS_SOFTWARE,               /* for System Software */
    AMBA_NOR_PARTITION_DSP_uCODE,                  /* for DSP uCode */
    AMBA_NOR_PARTITION_SYS_ROM_DATA,               /* for System ROM Data */

    /* optional */
    AMBA_NOR_PARTITION_LINUX_KERNEL,               /* optional for Linux Kernel */
    AMBA_NOR_PARTITION_LINUX_ROOT_FS,              /* optional for Linux Root File System */
    AMBA_NOR_PARTITION_LINUX_HIBERNATION_IMAGE,    /* optional for Linux Hibernation Image */

    /* without pre-built image */
    AMBA_NOR_PARTITION_STORAGE0,                   /* for FAT: Drive 'A' */
    AMBA_NOR_PARTITION_STORAGE1,                   /* for FAT: Drive 'B'; Slot-0: Drive 'C'; Slot-1: Drive 'D' */

    AMBA_NOR_PARTITION_VIDEO_REC_INDEX,            /* for Video Recording Index */
    AMBA_NOR_PARTITION_USER_SETTING,               /* for User Settings */
    AMBA_NOR_PARTITION_CALIBRATION_DATA,           /* for Calibration Data */

    AMBA_NUM_NOR_PARTITION,                        /* Total Number of NAND Partitions */

    /* Total Number of NAND Media Partitions */
    AMBA_NUM_NOR_MEDIA_PARTITION = AMBA_NUM_NOR_PARTITION - AMBA_NOR_PARTITION_USER_SETTING,
} AMBA_NOR_PARTITION_ID_e;

#define HAS_NOR_IMG_PARTS                   AMBA_NUM_NOR_PARTITION
#define TOTAL_NOR_FW_PARTS                  (HAS_NOR_IMG_PARTS)
#endif
#define PART_ON_SPINOR                         (2)
/*-----------------------------------------------------------------------------------------------*\
 * NOR device Status error code definitions.
\*-----------------------------------------------------------------------------------------------*/
#define NOR_STATUS_PROGRAM_ERR      (1 << 6)
#define NOR_STATUS_ERASE_ERR            (1 << 5)
#define NOR_STATUS_BLOCK_PROTECT2       (1 << 4)
#define NOR_STATUS_BLOCK_PROTECT1       (1 << 3)
#define NOR_STATUS_BLOCK_PROTECT0       (1 << 2)
#define NOR_STATUS_WRTIE_ENABLE     (1 << 1)
#define NOR_STATUS_WRTIE_IN_PROCESS     (1 << 0)

/*-----------------------------------------------------------------------------------------------*\
 * Data structure describing a NAND flash device(s) configuration.
\*-----------------------------------------------------------------------------------------------*/
typedef enum _AMBA_NOR_SIZE_e_ {
    AMBA_NOR_SIZE_128M_BITS = 0,
    AMBA_NOR_SIZE_256M_BITS,
    AMBA_NOR_SIZE_512M_BITS,
    AMBA_NOR_SIZE_1G_BITS,
    AMBA_NOR_SIZE_2G_BITS,
    AMBA_NOR_SIZE_4G_BITS,
    AMBA_NOR_SIZE_8G_BITS
} AMBA_NOR_SIZE_e;

typedef struct _AMBA_NOR_DEV_INFO_s_ {
    char    *pDevName;              /* pointer to the Device name */
    UINT8  ManufactuerID;      /* Manufactuer ID */
    UINT8  DeviceID;        /* Device ID */
    UINT32  TotalByteSize;      /* Total number of bytes */
    UINT16  PageSize;   /* Program Page size in Byte  */
    UINT32  EraseBlockSize; /* Erase Block Size in Byte */
    UINT32  NORFrequncy;
    UINT32  Addr_Bits;
    UINT8   LSBFirst;
    UINT8  ReadCMD;
    UINT8  WriteCMD;
    UINT8  WriteRegCMD;
    UINT8  ReadIDCMD;
    UINT8  ResetEnableCMD;
    UINT8  ResetCMD;
    UINT8  ReadStatus0CMD;
    UINT8  ReadStatus1CMD;
    UINT8  ClearStatusCMD;
    UINT8  WriteEnableCMD;
    UINT8  WriteDisableCMD;
    UINT8  EraseBlkCMD;
    UINT8  EraseChipCMD;
    UINT8  ReadDummyCycle;
} AMBA_NOR_DEV_INFO_s;

#endif  /* _AMBA_NOR_DEF_H_ */
