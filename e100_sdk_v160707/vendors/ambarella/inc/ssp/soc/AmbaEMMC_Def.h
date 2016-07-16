  /*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaEMMC_Def.h
 *
 *  @Copyright      :: Copyright (C) 2012 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Definitions of Run Time Support Library for SD flash
\*-------------------------------------------------------------------------------------------------------------------*/

#ifndef _AMBA_EMMC_DEF_H_
#define _AMBA_EMMC_DEF_H_

/*-----------------------------------------------------------------------------------------------*\
 * Firmware partitions.
\*-----------------------------------------------------------------------------------------------*/
#if 0
typedef enum _AMBA_EMMC_PARTITION_ID_e_ {
    AMBA_PARTITION_BOOTSTRAP = 0,              /* for Bootstrap */
    AMBA_PARTITION_TABLE,                      /* for Partition Table */
    AMBA_PARTITION_BOOTLOADER,                 /* for Bootloader */
    AMBA_PARTITION_SD_FIRMWARE_UPDATE,         /* for SD Firmware Update s/w */
    AMBA_PARTITION_SYS_SOFTWARE,               /* for System Software */
    AMBA_PARTITION_DSP_uCODE,                  /* for DSP uCode */
    AMBA_PARTITION_SYS_ROM_DATA,               /* for System ROM Data */

    /* optional */
    AMBA_PARTITION_LINUX_KERNEL,               /* optional for Linux Kernel */
    AMBA_PARTITION_LINUX_ROOT_FS,              /* optional for Linux Root File System */
    AMBA_PARTITION_LINUX_HIBERNATION_IMAGE,    /* optional for Linux Hibernation Image */

    /* without pre-built image */
    AMBA_PARTITION_STORAGE0,                   /* for FAT: Drive 'A' */
    AMBA_PARTITION_STORAGE1,                   /* for FAT: Drive 'B'; Slot-0: Drive 'C'; Slot-1: Drive 'D' */

    AMBA_PARTITION_VIDEO_REC_INDEX,            /* for Video Recording Index */
    AMBA_PARTITION_USER_SETTING,               /* for User Settings */
    AMBA_PARTITION_CALIBRATION_DATA,           /* for Calibration Data */

    AMBA_NUM_FW_PARTITION,                        /* Total Number of NAND Partitions */

    /* Total Number of NAND Media Partitions */
     AMBA_NUM_FW_MEDIA_PARTITION = AMBA_NUM_FW_PARTITION - AMBA_PARTITION_STORAGE0,
} AMBA_EMMC_PARTITION_ID_e;

#undef HAS_IMG_PARTS
#undef TOTAL_FW_PARTS

#define HAS_IMG_PARTS                   (AMBA_PARTITION_LINUX_HIBERNATION_IMAGE + 1)
#define TOTAL_FW_PARTS                  (HAS_IMG_PARTS)
#endif
/*-----------------------------------------------------------------------------------------------*\
 * Data structure describing partitions in NAND flash.
\*-----------------------------------------------------------------------------------------------*/
typedef struct _AMBA_EMMC_FW_PART_s_ {
    UINT32  StartBlk[TOTAL_FW_PARTS];           /* Start block */
    UINT32  NumBlks[TOTAL_FW_PARTS];            /* Number of blocks */
} AMBA_EMMC_FW_PART_s;

/*-----------------------------------------------------------------------------------------------*\
 * Data structure describing partitions in NAND flash.
\*-----------------------------------------------------------------------------------------------*/
typedef struct _AMBA_EMMC_MEDIA_PART_s_ {
    UINT32  StartBlk[ AMBA_NUM_FW_MEDIA_PARTITION];           /* Start block */
    UINT32  NumBlks[ AMBA_NUM_FW_MEDIA_PARTITION];            /* Number of blocks */
} AMBA_EMMC_MEDIA_PART_s;

/*-----------------------------------------------------------------------------------------------*\
 * Indicate the partition is on which device.
\*-----------------------------------------------------------------------------------------------*/
#define PART_ON_EMMC                         (1)

#define HAS_EMMC_IMG_PARTS                   AMBA_NUM_FW_PARTITION
#define TOTAL_EMMC_FW_PARTS                  (HAS_EMMC_IMG_PARTS)

#define EMMC_SECTOR_SIZE                     (512)
#ifndef SEC_SIZE
#define SEC_SIZE                             (512)
#endif

extern const int AmbaFW_PartitionSize[AMBA_NUM_FW_PARTITION];

extern AMBA_EMMC_FW_PART_s    AmbaEMMC_FwPartInfo;
extern AMBA_EMMC_MEDIA_PART_s AmbaEMMC_MediaPartInfo;
int AmbaEMMC_WritePartition(UINT8 * pRaw, UINT32 RawSize, UINT32 PartID);
int AmbaEMMC_ErasePartition(UINT32 PartID);
int AmbaEMMC_LoadPartitionData(int PartID, UINT32 ImageOffset, UINT8 * pDst, UINT32 Length);

#endif  /* _AMBA_NOR_DEF_H_ */

