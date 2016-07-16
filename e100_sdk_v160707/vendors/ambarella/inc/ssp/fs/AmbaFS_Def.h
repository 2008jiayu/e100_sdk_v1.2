/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaFS_Def.h
 *
 *  @Copyright      :: Copyright (C) 2012 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Abstraction layer of Ambarella File System.
 *
 *  @History        ::
 *      Date        Name        Comments
 *      09/07/2012  CYChen      Created
 *
\*-------------------------------------------------------------------------------------------------------------------*/

#ifndef _AMBA_FS_DEF_H_
#define _AMBA_FS_DEF_H_

#include "prfile2/pf_w_apicmn.h"

/* For unicode support */
#define WDTA            PF_W_DTA

/* Make the software is compatible on PrFILE and PrFILE2 */
#define DTA             PF_DTA
#define DEV_INF         PF_DEV_INF
#define VOLTAB          PF_VOLTAB

/* File System Description */
#define FS_FAT_CONTAIN_ERROR    (-3)    /**< File system contain currupt */
#define FS_FAT_ERROR            (-2)    /**< can't get file system info */
#define FS_FAT_NO_FORMAT        (-1)    /**< initial value of format field */
#define FS_FAT_UNKNOWN          (0x0)
#define FS_FAT12                (0x01)  /**< FAT12            */
#define FS_FAT16S               (0x04)  /**< FAT16 < 32MB     */
#define FS_FAT16_EXT_DOS        (0x05)  /**< Extended DOS partition (0~2GB) */
#define FS_FAT16                (0x06)  /**< FAT16 >= 32MB    */
#define FS_FAT32                (0x0b)  /**< FAT32            */
#define FS_FAT32L               (0x0c)  /**< FAT32 require extend int13 support(LBA) */
#define FS_EXFAT                (0x07)  /**< EX-FAT           */
#define FS_FAT16L               (0x0e)  /**< FAT16 require extend int13 support(LBA) */
#define FS_FAT16L_EXT_DOS       (0x0f)  /**< FAT16 require extend int13 support(LBA) */

#define FAT12_TYPE              12
#define FAT16_TYPE              16
#define FAT32_TYPE              32
#define EXFAT_TYPE              64

#define AMBA_FS_PRF2_IOSTAT_MAX_LVL 16

/**
 * For IO statistics
 */
typedef struct __AMBA_FS_IO_STATISTICS__ {
    UINT8   Enable;
    UINT64  ReadBytes;
    UINT64  WriteBytes;
    UINT32      ReadTime;   /**< in ms */
    UINT32      WriteTime;  /**< in ms */
    /* For read/write Sectors distribution diagram */
    UINT32  Rlvl[AMBA_FS_PRF2_IOSTAT_MAX_LVL];  /**< for read level */
    UINT32  Wlvl[AMBA_FS_PRF2_IOSTAT_MAX_LVL];  /**< for write level */
} AMBA_FS_IO_STATISTICS;

static inline void AmbaFS_Prf2IoStatDiagram(UINT32 *pLvl, UINT32 Sectors)
{
    int i;

    for (i = 0; i < AMBA_FS_PRF2_IOSTAT_MAX_LVL; i++) {
        if (Sectors <= (1 << i)) {
            pLvl[i]++;
            return;
        }
    }

    pLvl[AMBA_FS_PRF2_IOSTAT_MAX_LVL - 1]++;
    return;
}

#endif /* _AMBA_FS_DEF_H_ */
