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

#ifndef _AMBA_PARTITION_DEF_H_
#define _AMBA_PARTITION_DEF_H_

/*-----------------------------------------------------------------------------------------------*\
 * Firmware partitions.
\*-----------------------------------------------------------------------------------------------*/
typedef enum _AMBA_FW_PARTITION_ID_e_ {
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
} AMBA_FW_PARTITION_ID_e;

#define HAS_IMG_PARTS                   (AMBA_PARTITION_LINUX_HIBERNATION_IMAGE + 1)
#define TOTAL_FW_PARTS                  (HAS_IMG_PARTS)

/*-----------------------------------------------------------------------------------------------*\
 * Media partitions. (Must have the same order as PART_STORAGE ...)
 * These should synchronize with NFTL_ID_XXX in AmbaNFTL.h.
\*-----------------------------------------------------------------------------------------------*/
typedef enum _AMBA_MEDIA_PARTITION_LAYOUT_e_ {
    MP_Storage0 = 0,
    MP_Storage1,
    MP_IndexForVideoRecording,
    MP_UserSetting,
    MP_CalibrationData,

    MP_MAX,                 /* Total number of media partitions. */

    /* Reserved from 5. */
    MP_RESERVED5 = MP_MAX,

    MP_MAX_WITH_RSV         /* Total number of media partitions (incouding reserved). */
} AMBA_MEDIA_PARTITION_LAYOUT_e;

#endif  /* _AMBA_PARTITION_DEF_H_ */
