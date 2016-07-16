/*
    Copyright(C) 2010 eSOL Co., Ltd. All rights reserved.

    This software is protected by the law and the agreement concerning
    a Japanese country copyright method, an international agreement,
    and other intellectual property right and may be used and copied
    only in accordance with the terms of such license and with the inclusion
    of the above copyright notice.

    This software or any other copies thereof may not be provided
    or otherwise made available to any other person.  No title to
    and ownership of the software is hereby transferred.

    The information in this software is subject to change without
    notice and should not be construed as a commitment by eSOL Co.,Ltd.
*/
/****************************************************************************
[pdm_exfat_common.h] - .
****************************************************************************/
#ifndef __PDM_EXFAT_COMMON_H__
#define __PDM_EXFAT_COMMON_H__

#include "prfile2/pdm_common.h"

#if PF_EXFAT_SUPPORT
/****************************************************************************
  Macro definitions
****************************************************************************/
/* FileSystemRevision   */
#define PDM_BSS_REVISION_MIN                (0x0100)
#define PDM_BSS_REVISION_MAX                (0x0100)

/*---------------------------------------------------------------------------
  Offsets of Boot Sector (BS) and Boot Sector Sub-regions fields
---------------------------------------------------------------------------*/
#define PDM_BSS_JUMP_BOOT                   (+0)        /*   3 Bytes */
#define PDM_BSS_FILE_SYSTEM_NAME            (+3)        /*   8 Bytes */
#define PDM_BSS_MUST_BE_ZERO                (+11)       /*  53 Bytes */
#define PDM_BSS_PARTITION_OFFSET            (+64)       /*   8 Bytes */
#define PDM_BSS_TOTAL_SECTORS               (+72)       /*   8 Bytes */
#define PDM_BSS_FAT_OFFSET                  (+80)       /*   4 Bytes */
#define PDM_BSS_SECTORS_PER_FAT             (+84)       /*   4 Bytes */
#define PDM_BSS_FIRST_DATA_SECTOR           (+88)       /*   4 Bytes */
#define PDM_BSS_NUM_CLUSTERS                (+92)       /*   4 Bytes */
#define PDM_BSS_ROOT_DIRECTORY_CLUSTER      (+96)       /*   4 Bytes */
#define PDM_BSS_VOLUME_SERIAL_NUMBER        (+100)      /*   4 Bytes */
#define PDM_BSS_FILE_SYSTEM_VERSION         (+104)      /*   2 Bytes */
#define PDM_BSS_VOLUME_FLAGS                (+106)      /*   2 Bytes */
#define PDM_BSS_LOG2_BYTES_PER_SECTOR       (+108)      /*   1 Byte  */
#define PDM_BSS_LOG2_SECTORS_PER_CLUSTER    (+109)      /*   1 Byte  */
#define PDM_BSS_NUM_OF_FATS                 (+110)      /*   1 Byte  */
#define PDM_BSS_DRIVE_SELECT                (+111)      /*   1 Byte  */
#define PDM_BSS_PERCENT_IN_USE              (+112)      /*   1 Byte  */
#define PDM_BSS_BOOT_CODE                   (+120)      /* 390 Bytes */
#define PDM_BSS_TRAIL_SIG1                  (+510)      /*   1 Byte  */
#define PDM_BSS_TRAIL_SIG2                  (+511)      /*   1 Byte  */
#define PDM_BSS_EXCESS_SPACE                (+512)      /*   ? Bytes */

#define PDM_BSS_JUMP_BOOT_SIZE              (3)
#define PDM_BSS_FILE_SYSTEM_NAME_SIZE       (8)
#define PDM_BSS_VOLUME_SERIAL_NUM_SIZE      (4)

#define PDM_BSS_FIRST_FAT_ACTIVE            (1)
#define PDM_BSS_SECOND_FAT_ACTIVE           (2)

#define PDM_BSS_TRAIL_SIG1_VALUE            (0x55)
#define PDM_BSS_TRAIL_SIG2_VALUE            (0xaa)

#define PDM_BSS_ACTIVE_FAT_FLAG             (0x0001)    /*   0 bit */

#define PDM_BSS_MAIN_BOOT_REGION_SECTORS    (12)
#define PDM_BSS_BACKUP_BOOT_SECTOR          (+12)

#define PDM_BSS_MAIN_BOOT_CHECKSUM          (+11)

#define PDM_BSS_VOLUME_FLAGS_LOW_ORDER      (106)
#define PDM_BSS_VOLUME_FLAGS_HIGE_ORDER     (107)

#define PDM_BSS_REV_L_OFFSET                (PDM_BSS_FILE_SYSTEM_VERSION + 0)
#define PDM_BSS_REV_H_OFFSET                (PDM_BSS_FILE_SYSTEM_VERSION + 1)

/*---------------------------------------------------------------------------
  Accessor for Main Boot Sector Sub-regions (BSS)
 ----------------------------------------------------------------------------*/
/* BSS fields for exFAT */
#define PDM_LOAD_BSS_PARTITION_OFFSET(buf)              PDM_LOAD_FROM_LE64_FIELD ((buf), PDM_BSS_PARTITION_OFFSET        )
#define PDM_LOAD_BSS_TOTAL_SECTORS(buf)                 PDM_LOAD_FROM_LE64_FIELD ((buf), PDM_BSS_TOTAL_SECTORS           )
#define PDM_LOAD_BSS_FAT_OFFSET(buf)                    PDM_LOAD_FROM_LE32_FIELD ((buf), PDM_BSS_FAT_OFFSET              )
#define PDM_LOAD_BSS_SECTORS_PER_FAT(buf)               PDM_LOAD_FROM_LE32_FIELD ((buf), PDM_BSS_SECTORS_PER_FAT         )
#define PDM_LOAD_BSS_FIRST_DATA_SECTOR(buf)             PDM_LOAD_FROM_LE32_FIELD ((buf), PDM_BSS_FIRST_DATA_SECTOR       )
#define PDM_LOAD_BSS_NUM_CLUSTERS(buf)                  PDM_LOAD_FROM_LE32_FIELD ((buf), PDM_BSS_NUM_CLUSTERS            )
#define PDM_LOAD_BSS_ROOT_DIRECTORY_CLUSTER(buf)        PDM_LOAD_FROM_LE32_FIELD ((buf), PDM_BSS_ROOT_DIRECTORY_CLUSTER  )
#define PDM_LOAD_BSS_FILE_SYSTEM_VERSION(buf)           PDM_LOAD_FROM_LE16_FIELD ((buf), PDM_BSS_FILE_SYSTEM_VERSION     )
#define PDM_LOAD_BSS_VOLUME_FLAGS(buf)                  PDM_LOAD_FROM_LE16_FIELD ((buf), PDM_BSS_VOLUME_FLAGS            )
#define PDM_LOAD_BSS_LOG2_BYTES_PER_SECTOR(buf)         PDM_LOAD_FROM_LE8_FIELD  ((buf), PDM_BSS_LOG2_BYTES_PER_SECTOR   )
#define PDM_LOAD_BSS_LOG2_SECTORS_PER_CLUSTER(buf)      PDM_LOAD_FROM_LE8_FIELD  ((buf), PDM_BSS_LOG2_SECTORS_PER_CLUSTER)
#define PDM_LOAD_BSS_NUM_OF_FATS(buf)                   PDM_LOAD_FROM_LE8_FIELD  ((buf), PDM_BSS_NUM_OF_FATS             )
#define PDM_LOAD_BSS_DRIVE_SELECT(buf)                  PDM_LOAD_FROM_LE8_FIELD  ((buf), PDM_BSS_DRIVE_SELECT            )
#define PDM_LOAD_BSS_REVISION(buf)                      (((PDM_LOAD_FROM_LE8_FIELD((buf), PDM_BSS_REV_H_OFFSET) << 8) & 0xFF00) \
                                                         | PDM_LOAD_FROM_LE8_FIELD((buf), PDM_BSS_REV_L_OFFSET))
#define PDM_LOAD_BSS_PERCENT_IN_USE(buf)                PDM_LOAD_FROM_LE8_FIELD  ((buf), PDM_BSS_PERCENT_IN_USE          )
#define PDM_LOAD_BSS_BOOT_CHECKSUM(buf)                 PDM_LOAD_FROM_LE32_FIELD ((buf), 0                               )

#define PDM_STORE_BSS_VOLUME_FLAGS(buf, v)              PDM_STORE_INTO_LE16_FIELD ((buf), PDM_BSS_VOLUME_FLAGS, (v))
#define PDM_STORE_BSS_PERCENT_IN_USE(buf, v)            PDM_STORE_INTO_LE8_FIELD ((buf), PDM_BSS_PERCENT_IN_USE, (v))

/****************************************************************************
  Struct definitions
****************************************************************************/
/*---------------------------------------------------------------------------
  PDM_BSS - BSS (Boot Sector Sub-regions) and other values derived from them.
---------------------------------------------------------------------------*/
struct PDM_BSS_TAG
{
    /*------------------------------------------------------------------*
      Parameters from BSS in Boot Sector
     *------------------------------------------------------------------*/
    /* FileSystemName string. */
    PDM_U_CHAR      fs_name[PDM_BSS_FILE_SYSTEM_NAME_SIZE];

    /* BytesPerSectorShift (log2). */
    PDM_U_CHAR      log2_bytes_per_sector;

    /* SectorsPerClusterShift (log2). */
    PDM_U_CHAR      log2_sectors_per_cluster;

    /* Count of FAT data structures on the volume. */
    PDM_U_CHAR      num_FATs;

    /* FAT offset in sector. */
    PDM_U_LONG      FAT_offset;

    /* VolumeFlags. */
    PDM_U_SHORT     volume_flags;

    /* DriveSelect. */
    PDM_U_CHAR      drive_select;

    /* VolumeSerialNumber. */
    PDM_U_CHAR      vol_serial_number[PDM_BSS_VOLUME_SERIAL_NUM_SIZE];

    /* Count of Sectors occupied by one FAT. */
    PDM_U_LONG      sectors_per_FAT;

    /* FileSystemRevision. */
    PDM_U_SHORT     fs_version;

    /* FirstClusterOfRootDirectory. */
    PDM_U_LONG      root_dir_cluster;

    /* PartitionOffset. */
    PDM_UINT64      partition_offset;

    /* VolumeLength. */
    PDM_UINT64      total_sectors;

    /* ClusterHeapOffset. */
    PDM_U_LONG      first_data_sector;

    /* ClusterCount. */
    PDM_U_LONG      num_clusters;

    /* FileSystemRevision */
    PDM_U_SHORT     revision;

    /* PercentInUse. */
    PDM_U_CHAR      percent_in_use;

    /* JumpBoot. */
    PDM_U_CHAR      jump_boot[PDM_BSS_JUMP_BOOT_SIZE];

    /*------------------------------------------------------------------*
      Values computed from the parameters above.
     *------------------------------------------------------------------*/
    /* Backup boot sector offset. */
    PDM_U_SHORT     backup_boot_sector;

    /* Numbers of active FATs */
    PDM_U_CHAR      num_active_FATs;

    /* File system type.  */
    PDM_FAT_TYPE    fat_type;

    /* Bytes per sectors calculated from BytesPerSectorShift. */
    PDM_U_SHORT     bytes_per_sector;

    /* Sectors per cluster calculated from SectorsPerClusterShift. */
    PDM_U_LONG      sectors_per_cluster;

    /* Start of active FAT entry (first FAT if mirroring). */
    PDM_U_LONG      active_FAT_sector;

    /* First root directory sector. */
    PDM_UINT64      first_root_dir_sector;

    /* Max file size (byte) */
    PDM_FSIZE_T     max_file_size;

    /* Number of clusters of max_file_size */
    PDM_CLST        num_clst_max_fsize;
};
typedef struct PDM_BSS_TAG  PDM_BSS;

#endif /* #if PF_EXFAT_SUPPORT */

#endif  /* __PDM_EXFAT_COMMON_H__ */
