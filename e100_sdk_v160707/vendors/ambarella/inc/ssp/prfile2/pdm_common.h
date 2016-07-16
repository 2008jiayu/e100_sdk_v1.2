/*
    Copyright(C) 2005 eSOL Co., Ltd. All rights reserved.

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
[pdm_common.h] - .
****************************************************************************/
#ifndef __PDM_COMMON_H__
#define __PDM_COMMON_H__

#include "prfile2/pdm_config.h"
#include "prfile2/pdm_types.h"
#include "prfile2/pf_clib.h"


/****************************************************************************
  Macro definitions
****************************************************************************/
/*---------------------------------------------------------------------------
  Unmount parameter.
---------------------------------------------------------------------------*/
#define PDM_RELEASE_NORMAL          (0)
#define PDM_RELEASE_FORCE           (1)


/*---------------------------------------------------------------------------
  FAT Type
---------------------------------------------------------------------------*/
typedef enum {
    PDM_FAT_12  = 0,
    PDM_FAT_16  = 1,
    PDM_FAT_32  = 2,
    PDM_EXFAT   = 3,
    PDM_FAT_ERR = -1
} PDM_FAT_TYPE;


/*---------------------------------------------------------------------------
  BPB format parameter
---------------------------------------------------------------------------*/
#define PDM_BPB_DEFAULT             (0xffffffffUL)


/*---------------------------------------------------------------------------
  Media state flags
---------------------------------------------------------------------------*/
/* Detect */
#define PDM_MEDIA_DETECT_CHANGED                ((PDM_U_LONG)0x00000001UL)

/* Attribute */
#define PDM_MEDIA_ATTR_WRITE_PROTECT            ((PDM_U_LONG)0x00000001UL)
#define PDM_MEDIA_ATTR_VOLATILE_MEMORY          ((PDM_U_LONG)0x00000002UL)
#define PDM_MEDIA_ATTR_REMOVABLE                ((PDM_U_LONG)0x00000004UL)
#define PDM_MEDIA_ATTR_NOT_EXE_LOGICAL_FORMAT   ((PDM_U_LONG)0x00000008UL)

/* Attribute check macros */
#define IS_MEDIA_ATTR_WRITE_PROTECT(mattr)              \
                (((mattr) & PDM_MEDIA_ATTR_WRITE_PROTECT) == PDM_MEDIA_ATTR_WRITE_PROTECT)

#define IS_MEDIA_ATTR_VOLATILE_MEMORY(mattr)            \
                (((mattr) & PDM_MEDIA_ATTR_VOLATILE_MEMORY) == PDM_MEDIA_ATTR_VOLATILE_MEMORY)

#define IS_MEDIA_ATTR_REMOVABLE(mattr)                  \
                (((mattr) & PDM_MEDIA_ATTR_REMOVABLE) == PDM_MEDIA_ATTR_REMOVABLE)

#define IS_MEDIA_ATTR_NOT_EXE_LOGICAL_FORMAT(mattr)     \
                (((mattr) & PDM_MEDIA_ATTR_NOT_EXE_LOGICAL_FORMAT) == PDM_MEDIA_ATTR_NOT_EXE_LOGICAL_FORMAT)


/*---------------------------------------------------------------------------
  The media events
---------------------------------------------------------------------------*/
#define PDM_PART_MEDIA_EVENT_INSERT             (0x00000001ul)
#define PDM_PART_MEDIA_EVENT_EJECT              (0x00000002ul)


/*---------------------------------------------------------------------------
  Error code number
---------------------------------------------------------------------------*/
#define PDM_ERR_INVALID_PARAMETER               (0x0001)
#define PDM_ERR_INVALID_MASTER_BOOT             (0x0002)
#define PDM_ERR_INVALID_BOOT_SECTOR             (0x0003)
#define PDM_ERR_INVALID_BPB                     (0x0004)
#define PDM_ERR_NOT_EXIST_MBR                   (0x0005)
#define PDM_ERR_NOT_EXIST_EPBR                  (0x0006)
#define PDM_ERR_NOT_EXIST_PARTITION             (0x0007)
#define PDM_ERR_NOT_EXIST_FREE_DISK_STRUCT      (0x0008)
#define PDM_ERR_NOT_EXIST_PARTITION_STRUCT      (0x0009)
#define PDM_ERR_NOT_EXIST_FREE_PARTITION_STRUCT (0x000a)
#define PDM_ERR_STATE_OPENED                    (0x000b)
#define PDM_ERR_STATE_CLOSED                    (0x000c)
#define PDM_ERR_STATE_LOCKED                    (0x000d)
#define PDM_ERR_STATE_UNLOCKED                  (0x000e)
#define PDM_ERR_ACCESS_PERMISSION               (0x000f)
#define PDM_ERR_WRITE_PROTECTED                 (0x0010)
#define PDM_ERR_MEDIA_EJECTED                   (0x0011)
#define PDM_ERR_OUT_OF_RANGE                    (0x0012)
#define PDM_ERR_SYSTEM_CALL_ERROR               (0x0013)
#define PDM_ERR_LOCK_ERROR                      (0x0014)
#define PDM_ERR_DRIVER_ERROR                    (0x0015)
#define PDM_ERR_UNSUPPORT_DISK_FORMAT           (0x0016)


/*---------------------------------------------------------------------------
  Size of directory entry
---------------------------------------------------------------------------*/
#define PDM_LOG2_DIR_ENTRY_SIZE                 (5) /* log2(DIR_ENTRY_SIZE) */


/**
 * Swaps the order of the bytes in an 'PDM_U_SHORT' if platform uses big
 * endian representation (i.e. if 'BIG_ENDIAN' is defined).
 */
#if PDM_BIG_ENDIAN

  #define PDM_LE16_TO_U16(x)            ((PDM_U_SHORT)((((x) >> 8) & 0x00FF) | \
                                                      (((x) << 8) & 0xFF00)))
  #define PDM_U16_TO_LE16(x)            PDM_LE16_TO_U16(x)

  #define PDM_LE32_TO_U32(x)            ((PDM_U_LONG)((((x) >> 24) & 0x000000FF) | \
                                                      (((x) >>  8) & 0x0000FF00) | \
                                                      (((x) <<  8) & 0x00FF0000) | \
                                                      (((x) << 24) & 0xFF000000)))
  #define PDM_U32_TO_LE32(x)            PDM_LE32_TO_U32(x)

#else   /* !PDM_BIG_ENDIAN */

  #define PDM_LE16_TO_U16(x)            ((PDM_U_SHORT)(x))
  #define PDM_U16_TO_LE16(x)            PDM_LE16_TO_U16(x)

  #define PDM_LE32_TO_U32(x)            ((PDM_U_LONG)(x))
  #define PDM_U32_TO_LE32(x)            PDM_LE32_TO_U32(x)

#endif  /* PDM_BIG_ENDIAN */


#define PDM_LOAD_FROM_LE16_BUF(buf)         PDM_LE16_TO_U16(*(PDM_U_SHORT*)(buf))
#define PDM_LOAD_FROM_LE32_BUF(buf)         PDM_LE32_TO_U32(*(PDM_U_LONG *)(buf))

#define PDM_STORE_INTO_LE16_BUF(buf,val)    (*(PDM_U_SHORT*)(buf) = PDM_U16_TO_LE16(val))
#define PDM_STORE_INTO_LE32_BUF(buf,val)    (*(PDM_U_LONG *)(buf) = PDM_U32_TO_LE32(val))


#define PDM_LOAD_FROM_LE8_FIELD(buf,offset)                                       \
                (*(PDM_U_CHAR *)((buf)+(offset)))

#define PDM_LOAD_FROM_LE16_FIELD(buf,offset)                                      \
                (((PDM_U_SHORT)(*(PDM_U_CHAR*)((buf)+(offset))))        |         \
                 ((PDM_U_SHORT)(*(PDM_U_CHAR*)((buf)+(offset+1))) << 8))

#define PDM_LOAD_FROM_LE32_FIELD(buf,offset)                                      \
                (((PDM_U_LONG)(*(PDM_U_CHAR*)((buf)+(offset))))         |         \
                 ((PDM_U_LONG)(*(PDM_U_CHAR*)((buf)+(offset+1))) << 8)  |         \
                 ((PDM_U_LONG)(*(PDM_U_CHAR*)((buf)+(offset+2))) << 16) |         \
                 ((PDM_U_LONG)(*(PDM_U_CHAR*)((buf)+(offset+3))) << 24))

#if PF_EXFAT_SUPPORT
#define PDM_LOAD_FROM_LE64_FIELD(buf,offset)                                      \
                (((PDM_UINT64)(*(PDM_U_CHAR*)((buf)+(offset))))         |         \
                 ((PDM_UINT64)(*(PDM_U_CHAR*)((buf)+(offset+1))) << 8)  |         \
                 ((PDM_UINT64)(*(PDM_U_CHAR*)((buf)+(offset+2))) << 16) |         \
                 ((PDM_UINT64)(*(PDM_U_CHAR*)((buf)+(offset+3))) << 24) |         \
                 ((PDM_UINT64)(*(PDM_U_CHAR*)((buf)+(offset+4))) << 32) |         \
                 ((PDM_UINT64)(*(PDM_U_CHAR*)((buf)+(offset+5))) << 40) |         \
                 ((PDM_UINT64)(*(PDM_U_CHAR*)((buf)+(offset+6))) << 48) |         \
                 ((PDM_UINT64)(*(PDM_U_CHAR*)((buf)+(offset+7))) << 56))
#endif /* PF_EXFAT_SUPPORT */

#define PDM_STORE_INTO_LE8_FIELD(buf,offset,val)                                  \
                (*(PDM_U_CHAR *)((buf)+(offset)) = (val))

#define PDM_STORE_INTO_LE16_FIELD(buf,offset,val)                                 \
                (*(PDM_U_CHAR*)((buf)+(offset)) = (PDM_U_CHAR)(val));             \
                (*(PDM_U_CHAR*)((buf)+(offset+1)) = (PDM_U_CHAR)(val >> 8));      \

#define PDM_STORE_INTO_LE32_FIELD(buf,offset,val)                                 \
                (*(PDM_U_CHAR *)((buf)+(offset)) = (PDM_U_CHAR)(val));            \
                (*(PDM_U_CHAR *)((buf)+(offset+1)) = (PDM_U_CHAR)(val >> 8));     \
                (*(PDM_U_CHAR *)((buf)+(offset+2)) = (PDM_U_CHAR)(val >> 16));    \
                (*(PDM_U_CHAR *)((buf)+(offset+3)) = (PDM_U_CHAR)(val >> 24));    \


/*---------------------------------------------------------------------------
  Offsets of Boot Sector (BS) and BPB fields
---------------------------------------------------------------------------*/
/* BPB fields for FAT12/16/32 */
#define PDM_BPB_BYTES_PER_SECTOR            (+11)       /*  2 bytes */
#define PDM_BPB_SECTORS_PER_CLUSTER         (+13)       /*  1 byte  */
#define PDM_BPB_NUM_OF_RESERVED_SECTORS     (+14)       /*  2 bytes */
#define PDM_BPB_NUM_OF_FATS                 (+16)       /*  1 byte  */
#define PDM_BPB_NUM_OF_ROOT_DIR_ENTRIES     (+17)       /*  2 bytes */
#define PDM_BPB_NUM_OF_TOTAL_SECTORS_16     (+19)       /*  2 bytes */
#define PDM_BPB_MEDIA_DESCRIPTER            (+21)       /*  1 byte  */
#define PDM_BPB_SECTORS_PER_FAT_16          (+22)       /*  2 bytes */
#define PDM_BPB_SECOTRS_PER_TRACK           (+24)       /*  2 bytes */
#define PDM_BPB_NUM_HEADS                   (+26)       /*  2 bytes */
#define PDM_BPB_NUM_HIDDEN_SECTORS          (+28)       /*  4 bytes */
#define PDM_BPB_NUM_OF_TOTAL_SECTORS_32     (+32)       /*  4 bytes */

/* BPB fields for FAT32 */
#define PDM_BPB_SECTORS_PER_FAT_32          (+36)       /*  4 bytes */
#define PDM_BPB_EXT_FLAGS                   (+40)       /*  2 bytes */
#define PDM_BPB_FS_VERSION                  (+42)       /*  2 bytes */
#define PDM_BPB_ROOT_DIR_CLUSTER            (+44)       /*  4 bytes */
#define PDM_BPB_FS_INFO_SECTOR              (+48)       /*  2 bytes */
#define PDM_BPB_BAK_BS_SECTOR               (+50)       /*  2 bytes */
#define PDM_BPB_RESERVED                    (+52)       /* 12 bytes */

/* BS fields for FAT12/16/32 */
#define PDM_BS_JUMP_BOOT                    (+0)        /*  3 bytes */
#define PDM_BS_OEM_NAME                     (+3)        /*  8 bytes */
#define PDM_BS_TRAIL_SIG1                   (+510)      /*  1 bytes */
#define PDM_BS_TRAIL_SIG2                   (+511)      /*  1 bytes */

/* BS fields for FAT12/16 */
#define PDM_BS_DRIVE_16                     (+36)       /*  1 byte  */
#define PDM_BS_RESERVED_1_16                (+37)       /*  1 byte  */
#define PDM_BS_BOOT_SIG_16                  (+38)       /*  1 byte  */
#define PDM_BS_VOLUME_ID_16                 (+39)       /*  4 bytes */
#define PDM_BS_VOLUME_LABEL_16              (+43)       /* 11 bytes */
#define PDM_BS_FILE_SYS_TYPE_16             (+54)       /*  8 bytes */

/* BS fields for FAT32 */
#define PDM_BS_DRIVE_32                     (+64)       /*  1 byte  */
#define PDM_BS_RESERVED_1_32                (+65)       /*  1 byte  */
#define PDM_BS_BOOT_SIG_32                  (+66)       /*  1 byte  */
#define PDM_BS_VOLUME_ID_32                 (+67)       /*  4 bytes */
#define PDM_BS_VOLUME_LABEL_32              (+71)       /* 11 bytes */
#define PDM_BS_FILE_SYS_TYPE_32             (+82)       /*  8 bytes */


#define PDM_BS_JUMP_BOOT_SIZE               (3)
#define PDM_BS_OEM_NAME_SIZE                (8)
#define PDM_BS_VOLUME_LABEL_SIZE            (11)
#define PDM_BS_FILE_SYS_TYPE_SIZE           (8)

#define PDM_BPB_RESERVED_SIZE               (12)        /* 12 bytes */

#define PDM_BS_TRAIL_SIG1_VALUE             (0x55)
#define PDM_BS_TRAIL_SIG2_VALUE             (0xaa)


/*---------------------------------------------------------------------------
  Accessor for Boot Sector (BS) and BPB fields
 ----------------------------------------------------------------------------*/
/* BPB fields for FAT12/16/32 */
#define PDM_LOAD_BPB_BYTES_PER_SECTOR(buf)              PDM_LOAD_FROM_LE16_FIELD((buf), PDM_BPB_BYTES_PER_SECTOR        )
#define PDM_LOAD_BPB_SECTORS_PER_CLUSTER(buf)           PDM_LOAD_FROM_LE8_FIELD ((buf), PDM_BPB_SECTORS_PER_CLUSTER     )
#define PDM_LOAD_BPB_NUM_OF_RESERVED_SECTORS(buf)       PDM_LOAD_FROM_LE16_FIELD((buf), PDM_BPB_NUM_OF_RESERVED_SECTORS )
#define PDM_LOAD_BPB_NUM_OF_FATS(buf)                   PDM_LOAD_FROM_LE8_FIELD ((buf), PDM_BPB_NUM_OF_FATS             )
#define PDM_LOAD_BPB_NUM_OF_ROOT_DIR_ENTRIES(buf)       PDM_LOAD_FROM_LE16_FIELD((buf), PDM_BPB_NUM_OF_ROOT_DIR_ENTRIES )
#define PDM_LOAD_BPB_NUM_OF_TOTAL_SECTORS_16(buf)       PDM_LOAD_FROM_LE16_FIELD((buf), PDM_BPB_NUM_OF_TOTAL_SECTORS_16 )
#define PDM_LOAD_BPB_MEDIA_DESCRIPTER(buf)              PDM_LOAD_FROM_LE8_FIELD ((buf), PDM_BPB_MEDIA_DESCRIPTER        )
#define PDM_LOAD_BPB_SECTORS_PER_FAT_16(buf)            PDM_LOAD_FROM_LE16_FIELD((buf), PDM_BPB_SECTORS_PER_FAT_16      )
#define PDM_LOAD_BPB_SECOTRS_PER_TRACK(buf)             PDM_LOAD_FROM_LE16_FIELD((buf), PDM_BPB_SECOTRS_PER_TRACK       )
#define PDM_LOAD_BPB_NUM_HEADS(buf)                     PDM_LOAD_FROM_LE16_FIELD((buf), PDM_BPB_NUM_HEADS               )
#define PDM_LOAD_BPB_NUM_HIDDEN_SECTORS(buf)            PDM_LOAD_FROM_LE32_FIELD((buf), PDM_BPB_NUM_HIDDEN_SECTORS      )
#define PDM_LOAD_BPB_NUM_OF_TOTAL_SECTORS_32(buf)       PDM_LOAD_FROM_LE32_FIELD((buf), PDM_BPB_NUM_OF_TOTAL_SECTORS_32 )

#define PDM_STORE_BPB_BYTES_PER_SECTOR(buf,v)           PDM_STORE_INTO_LE16_FIELD((buf), PDM_BPB_BYTES_PER_SECTOR,       (v))
#define PDM_STORE_BPB_SECTORS_PER_CLUSTER(buf,v)        PDM_STORE_INTO_LE8_FIELD ((buf), PDM_BPB_SECTORS_PER_CLUSTER,    (v))
#define PDM_STORE_BPB_NUM_OF_RESERVED_SECTORS(buf,v)    PDM_STORE_INTO_LE16_FIELD((buf), PDM_BPB_NUM_OF_RESERVED_SECTORS,(v))
#define PDM_STORE_BPB_NUM_OF_FATS(buf,v)                PDM_STORE_INTO_LE8_FIELD ((buf), PDM_BPB_NUM_OF_FATS,            (v))
#define PDM_STORE_BPB_NUM_OF_ROOT_DIR_ENTRIES(buf,v)    PDM_STORE_INTO_LE16_FIELD((buf), PDM_BPB_NUM_OF_ROOT_DIR_ENTRIES,(v))
#define PDM_STORE_BPB_NUM_OF_TOTAL_SECTORS_16(buf,v)    PDM_STORE_INTO_LE16_FIELD((buf), PDM_BPB_NUM_OF_TOTAL_SECTORS_16,(v))
#define PDM_STORE_BPB_MEDIA_DESCRIPTER(buf,v)           PDM_STORE_INTO_LE8_FIELD ((buf), PDM_BPB_MEDIA_DESCRIPTER,       (v))
#define PDM_STORE_BPB_SECTORS_PER_FAT_16(buf,v)         PDM_STORE_INTO_LE16_FIELD((buf), PDM_BPB_SECTORS_PER_FAT_16,     (v))
#define PDM_STORE_BPB_SECOTRS_PER_TRACK(buf,v)          PDM_STORE_INTO_LE16_FIELD((buf), PDM_BPB_SECOTRS_PER_TRACK,      (v))
#define PDM_STORE_BPB_NUM_HEADS(buf,v)                  PDM_STORE_INTO_LE16_FIELD((buf), PDM_BPB_NUM_HEADS,              (v))
#define PDM_STORE_BPB_NUM_HIDDEN_SECTORS(buf,v)         PDM_STORE_INTO_LE32_FIELD((buf), PDM_BPB_NUM_HIDDEN_SECTORS,     (v))
#define PDM_STORE_BPB_NUM_OF_TOTAL_SECTORS_32(buf,v)    PDM_STORE_INTO_LE32_FIELD((buf), PDM_BPB_NUM_OF_TOTAL_SECTORS_32,(v))

/* BPB fields for FAT32 */
#define PDM_LOAD_BPB_SECTORS_PER_FAT_32(buf)            PDM_LOAD_FROM_LE32_FIELD((buf), PDM_BPB_SECTORS_PER_FAT_32)
#define PDM_LOAD_BPB_EXT_FLAGS(buf)                     PDM_LOAD_FROM_LE16_FIELD((buf), PDM_BPB_EXT_FLAGS         )
#define PDM_LOAD_BPB_FS_VERSION(buf)                    PDM_LOAD_FROM_LE16_FIELD((buf), PDM_BPB_FS_VERSION        )
#define PDM_LOAD_BPB_ROOT_DIR_CLUSTER(buf)              PDM_LOAD_FROM_LE32_FIELD((buf), PDM_BPB_ROOT_DIR_CLUSTER  )
#define PDM_LOAD_BPB_FS_INFO_SECTOR(buf)                PDM_LOAD_FROM_LE16_FIELD((buf), PDM_BPB_FS_INFO_SECTOR    )
#define PDM_LOAD_BPB_BAK_BS_SECTOR(buf)                 PDM_LOAD_FROM_LE16_FIELD((buf), PDM_BPB_BAK_BS_SECTOR     )

#define PDM_STORE_BPB_SECTORS_PER_FAT_32(buf,v)         PDM_STORE_INTO_LE32_FIELD((buf), PDM_BPB_SECTORS_PER_FAT_32, (v))
#define PDM_STORE_BPB_EXT_FLAGS(buf,v)                  PDM_STORE_INTO_LE16_FIELD((buf), PDM_BPB_EXT_FLAGS,          (v))
#define PDM_STORE_BPB_FS_VERSION(buf,v)                 PDM_STORE_INTO_LE16_FIELD((buf), PDM_BPB_FS_VERSION,         (v))
#define PDM_STORE_BPB_ROOT_DIR_CLUSTER(buf,v)           PDM_STORE_INTO_LE32_FIELD((buf), PDM_BPB_ROOT_DIR_CLUSTER,   (v))
#define PDM_STORE_BPB_FS_INFO_SECTOR(buf,v)             PDM_STORE_INTO_LE16_FIELD((buf), PDM_BPB_FS_INFO_SECTOR,     (v))
#define PDM_STORE_BPB_BAK_BS_SECTOR(buf,v)              PDM_STORE_INTO_LE16_FIELD((buf), PDM_BPB_BAK_BS_SECTOR,      (v))

/* BS fields for FAT12/16/32 */
#define PDM_LOAD_BS_TRAIL_SIG1(buf)                     PDM_LOAD_FROM_LE8_FIELD ((buf), PDM_BS_TRAIL_SIG1   )
#define PDM_LOAD_BS_TRAIL_SIG2(buf)                     PDM_LOAD_FROM_LE8_FIELD ((buf), PDM_BS_TRAIL_SIG2   )

#define PDM_STORE_BS_TRAIL_SIG1(buf, v)                 PDM_STORE_INTO_LE8_FIELD ((buf), PDM_BS_TRAIL_SIG1,    (v))
#define PDM_STORE_BS_TRAIL_SIG2(buf, v)                 PDM_STORE_INTO_LE8_FIELD ((buf), PDM_BS_TRAIL_SIG2,    (v))


/* BS fields for FAT12/16 */
#define PDM_LOAD_BS_DRIVE_16(buf)                       PDM_LOAD_FROM_LE8_FIELD ((buf), PDM_BS_DRIVE_16     )
#define PDM_LOAD_BS_RESERVED_1_16(buf)                  PDM_LOAD_FROM_LE8_FIELD ((buf), PDM_BS_RESERVED_1_16)
#define PDM_LOAD_BS_BOOT_SIG_16(buf)                    PDM_LOAD_FROM_LE8_FIELD ((buf), PDM_BS_BOOT_SIG_16  )
#define PDM_LOAD_BS_VOLUME_ID_16(buf)                   PDM_LOAD_FROM_LE32_FIELD((buf), PDM_BS_VOLUME_ID_16 )

#define PDM_STORE_BS_DRIVE_16(buf,v)                    PDM_STORE_INTO_LE8_FIELD ((buf), PDM_BS_DRIVE_16,      (v))
#define PDM_STORE_BS_RESERVED_1_16(buf,v)               PDM_STORE_INTO_LE8_FIELD ((buf), PDM_BS_RESERVED_1_16, (v))
#define PDM_STORE_BS_BOOT_SIG_16(buf,v)                 PDM_STORE_INTO_LE8_FIELD ((buf), PDM_BS_BOOT_SIG_16,   (v))
#define PDM_STORE_BS_VOLUME_ID_16(buf,v)                PDM_STORE_INTO_LE32_FIELD((buf), PDM_BS_VOLUME_ID_16,  (v))

/* BS fields for FAT32 */
#define PDM_LOAD_BS_DRIVE_32(buf)                       PDM_LOAD_FROM_LE8_FIELD ((buf), PDM_BS_DRIVE_32)
#define PDM_LOAD_BS_RESERVED_1_32(buf)                  PDM_LOAD_FROM_LE8_FIELD ((buf), PDM_BS_RESERVED_1_32)
#define PDM_LOAD_BS_BOOT_SIG_32(buf)                    PDM_LOAD_FROM_LE8_FIELD ((buf), PDM_BS_BOOT_SIG_32  )
#define PDM_LOAD_BS_VOLUME_ID_32(buf)                   PDM_LOAD_FROM_LE32_FIELD((buf), PDM_BS_VOLUME_ID_32 )

#define PDM_STORE_BS_DRIVE_32(buf,v)                    PDM_STORE_INTO_LE8_FIELD ((buf), PDM_BS_DRIVE_32,      (v))
#define PDM_STORE_BS_RESERVED_1_32(buf,v)               PDM_STORE_INTO_LE8_FIELD ((buf), PDM_BS_RESERVED_1_32, (v))
#define PDM_STORE_BS_BOOT_SIG_32(buf,v)                 PDM_STORE_INTO_LE8_FIELD ((buf), PDM_BS_BOOT_SIG_32,   (v))
#define PDM_STORE_BS_VOLUME_ID_32(buf,v)                PDM_STORE_INTO_LE32_FIELD((buf), PDM_BS_VOLUME_ID_32,  (v))


/*---------------------------------------------------------------------------
  Offsets of FSInfo Sector fields
---------------------------------------------------------------------------*/
#define PDM_FSI_LEAD_SIG                    (+  0)      /*   4 bytes */
#define PDM_FSI_RESERVED1                   (+  4)      /* 480 bytes */
#define PDM_FSI_STRUCT_SIG                  (+484)      /*   4 bytes */
#define PDM_FSI_FREE_COUNT                  (+488)      /*   4 bytes */
#define PDM_FSI_NEXT_FREE                   (+492)      /*   4 bytes */
#define PDM_FSI_RESERVED2                   (+496)      /*  12 bytes */
#define PDM_FSI_TRAIL_SIG                   (+508)      /*   4 bytes */

#define PDM_FSI_RESERVED1_SIZE              (480)       /* 480 bytes */
#define PDM_FSI_RESERVED2_SIZE              (12)        /*  12 bytes */

#define PDM_FSI_LEAD_SIG_VALUE              (0x41615252UL)
#define PDM_FSI_STRUCT_SIG_VALUE            (0x61417272UL)
#define PDM_FSI_TRAIL_SIG_VALUE             (0xaa550000UL)

#define PDM_FSI_FREE_COUNT_UNKNOWN          (0xffffffff)
#define PDM_FSI_NEXT_FREE_UNKNOWN           (0xffffffff)


/*---------------------------------------------------------------------------
  Accessor for FSInfo Sector fields
---------------------------------------------------------------------------*/
#define PDM_LOAD_FSI_LEAD_SIG(buf)          PDM_LOAD_FROM_LE32_FIELD((buf), PDM_FSI_LEAD_SIG  )
#define PDM_LOAD_FSI_STRUCT_SIG(buf)        PDM_LOAD_FROM_LE32_FIELD((buf), PDM_FSI_STRUCT_SIG)
#define PDM_LOAD_FSI_FREE_COUNT(buf)        PDM_LOAD_FROM_LE32_FIELD((buf), PDM_FSI_FREE_COUNT)
#define PDM_LOAD_FSI_NEXT_FREE(buf)         PDM_LOAD_FROM_LE32_FIELD((buf), PDM_FSI_NEXT_FREE )
#define PDM_LOAD_FSI_TRAIL_SIG(buf)         PDM_LOAD_FROM_LE32_FIELD((buf), PDM_FSI_TRAIL_SIG )

#define PDM_STORE_FSI_LEAD_SIG(buf,v)       PDM_STORE_INTO_LE32_FIELD((buf), PDM_FSI_LEAD_SIG,   (v))
#define PDM_STORE_FSI_STRUCT_SIG(buf,v)     PDM_STORE_INTO_LE32_FIELD((buf), PDM_FSI_STRUCT_SIG, (v))
#define PDM_STORE_FSI_FREE_COUNT(buf,v)     PDM_STORE_INTO_LE32_FIELD((buf), PDM_FSI_FREE_COUNT, (v))
#define PDM_STORE_FSI_NEXT_FREE(buf,v)      PDM_STORE_INTO_LE32_FIELD((buf), PDM_FSI_NEXT_FREE,  (v))
#define PDM_STORE_FSI_TRAIL_SIG(buf,v)      PDM_STORE_INTO_LE32_FIELD((buf), PDM_FSI_TRAIL_SIG,  (v))


/*---------------------------------------------------------------------------
  MBR (Master Boot Recorde) macros.
---------------------------------------------------------------------------*/
#define PDM_MBR_MAX_PARTTBL                 (4)


/*---------------------------------------------------------------------------
  FAT access error macros
---------------------------------------------------------------------------*/
#define PDM_DRV_RETRY                       (0)     /* retry same FAT   */
#define PDM_DRV_NEXT                        (1)     /* retry next FAT   */
#define PDM_DRV_ABORT                       (2)     /* access abort     */

/*---------------------------------------------------------------------------
  Physical Block macros
---------------------------------------------------------------------------*/
#define PDM_BLOCK_FIRST_DATA_SECTOR    (0xFFFFFFFF)  /* The start of the physics block is
                                                        First data sector. */
#define PDM_BLOCK_PARAM_ERASE_SIZE     (0xFFFFFFFF)  /* The size of the physics block is
                                                        EraseBlockSize in "Flash Parameters Structure". */


/*---------------------------------------------------------------------------
  Maximum file size
 ----------------------------------------------------------------------------*/
#define MAX_FILE_SIZE_FAT               (0xFFFFFFFFuL)
#if PF_EXFAT_SUPPORT
#define MAX_FILE_SIZE_EXFAT             PDM_ULL(0x1fffffffc00)
#endif /* PF_EXFAT_SUPPORT */


/****************************************************************************
  Struct definitions
****************************************************************************/
/*---------------------------------------------------------------------------
  Disk information
---------------------------------------------------------------------------*/
typedef struct PDM_DISK         PDM_DISK;

/*---------------------------------------------------------------------------
  Partition information
---------------------------------------------------------------------------*/
typedef struct PDM_PARTITION    PDM_PARTITION;

/*---------------------------------------------------------------------------
  The callback function to get a media event 
---------------------------------------------------------------------------*/
typedef void (*PDM_PARTITION_EVENT_CALLBACK)(PDM_U_LONG event, void* p_param);

/*---------------------------------------------------------------------------
  PDM_ERASE_FUNC - 
---------------------------------------------------------------------------*/
typedef PDM_ERROR               (*PDM_ERASE_FUNC)(PDM_U_LONG, PDM_U_LONG);

/*---------------------------------------------------------------------------
  PDM_DISK_INFO - 
---------------------------------------------------------------------------*/
/* Physical Block Information 
   - When the driver sets sectors_per_block to 0, PrF2 considers that 
     physical-block size is the same as sector size.
     --> (sectors_per_block==1 && start_sector==0)
 */
struct PDM_DISK_PHYSICAL_BLOCK_tag
{
    /* Size of the physical block (in sector). */
    PDM_U_LONG      sectors_per_block;

    /* Physical block start sector number.
       - The sector number which corresponds to the start position 
         of the physical block.
       - 0 origin value.
     */
    PDM_U_LONG      start_sector;
};
typedef struct PDM_DISK_PHYSICAL_BLOCK_tag  PDM_DISK_PHYSICAL_BLOCK;


typedef struct PDM_DISK_INFO {

    /* Total sector in disk */
    PDM_U_LONG      total_sectors;

    /* Number of cylinder */
    PDM_U_SHORT     cylinders;

    /* Number of head */
    PDM_U_CHAR      heads;

    /* Sector per track */
    PDM_U_CHAR      sectors_per_track;

    /* Byte per sector */
    PDM_U_SHORT     bytes_per_sector;

    /* Write protect. 1: write protect, 0: not write protect */
    PDM_U_LONG      media_attr;

    /* It used when format after mount. */
    void*           format_param;

    /* Physical Block Information */
    PDM_DISK_PHYSICAL_BLOCK     physical_block;

} PDM_DISK_INFO;


/*---------------------------------------------------------------------------
  PDM_FUNCTBL - Driver Interface Function Table
---------------------------------------------------------------------------*/
typedef struct PDM_FUNCTBL {

    /* Initialize drive for attach */
    PDM_ERROR   (*init)(PDM_DISK*);

    /* Finalize drive for detach */
    PDM_ERROR   (*finalize)(PDM_DISK*);

    /* Mount driver to access */
    PDM_ERROR   (*mount)(PDM_DISK*);

    /* Un-mount driver */
    PDM_ERROR   (*unmount)(PDM_DISK*);

    /* Format the volume */
    PDM_ERROR   (*format)(PDM_DISK*, const PDM_U_CHAR*);

    /* Read physical sectors */
    PDM_ERROR   (*physical_read)(PDM_DISK* p_disk,
                                 PDM_U_CHAR* buf,
                                 PDM_U_LONG block,
                                 PDM_U_LONG num,
                                 PDM_U_LONG* success_num);

    /* Write physical sectors*/
    PDM_ERROR   (*physical_write)(PDM_DISK* p_disk,
                                  const PDM_U_CHAR* buf,
                                  PDM_U_LONG block,
                                  PDM_U_LONG num,
                                  PDM_U_LONG* success_num);

    /* Get media information */
    PDM_ERROR   (*get_disk_info)(PDM_DISK* p_disk, PDM_DISK_INFO* p_disk_info);

} PDM_FUNCTBL;


/*---------------------------------------------------------------------------
  Disk Table
---------------------------------------------------------------------------*/
typedef struct PDM_DISK_TBL {

    /* Pointer to a set of driver functions */
    PDM_FUNCTBL*        p_func;

    /* Extended information */
    PDM_U_LONG          ui_ext;

} PDM_DISK_TBL;


/*---------------------------------------------------------------------------
  PDM_INIT_DISK - Table for initialized driver.
---------------------------------------------------------------------------*/
typedef struct PDM_INIT_DISK {

    /* Driver interface functions */
    PDM_ERROR           (*p_func)(PDM_DISK_TBL *p_disk_tbl, PDM_U_LONG ui_ext);

    /* Extension information of driver */
    PDM_U_LONG          ui_ext;

} PDM_INIT_DISK;


/*---------------------------------------------------------------------------
  PDM_BPB - BPB (BIOS Parameter Block) and other values derived from them.
---------------------------------------------------------------------------*/
typedef struct PDM_BPB
{
    /*------------------------------------------------------------------*
      Parameters from BPB in Boot Sector
     *------------------------------------------------------------------*/
    /* OEM name string. (corresponds to BS_OEMName) */
    PDM_U_CHAR      oem_name[PDM_BS_OEM_NAME_SIZE];

    /* Count of bytes per sector. (corresponds to BPB_BytsPerSec) */
    PDM_U_SHORT     bytes_per_sector;

    /* Sectors per cluster  (corresponds to BPB_SecPerClus) */
    PDM_U_CHAR      sectors_per_cluster;

    /* Count of FAT data structures on the volume. (corresponds to BPB_NumFATs) */
    PDM_U_CHAR      num_FATs;

    /* Number of reserved sectors. (corresponds to BPB_RsvdSecCnt) */
    PDM_U_SHORT     num_reserved_sectors;

    /* Count of root directory entries. (for FAT12/16) (corresponds to BPB_RootEntCnt) */
    PDM_U_SHORT     num_root_dir_entries;

    /* Total number of sectors on volume. (corresponds to BPB_TotSec16) */
    PDM_U_SHORT     total_sectors16;

    /* Count of Sectors occupied by one FAT (for FAT12/16). (corresponds to BPB_FATSz16) */
    PDM_U_SHORT     sectors_per_FAT16;

    /* Sector per track */
    PDM_U_SHORT     sector_per_track;

    /* Number of hidden sectors */
    PDM_U_LONG      num_hidden_sectors;

    /* Total number of sectors on volume. (corresponds to BPB_TotSec32) */
    PDM_U_LONG      total_sectors32;

    /* Number of heads */
    PDM_U_SHORT     num_heads;

    /* Media descriptor. (corresponds to BPB_Media) */
    PDM_U_CHAR      media;

    /* Drive number (corresponds to BS_DrvNum) */
    PDM_U_CHAR      drive;

    /* Volume ID (corresponds to BS_VolID) */
    PDM_U_LONG      vol_id;

    /* Boot signature (corresponds to BS_BootSig) */
    PDM_U_CHAR      boot_sig;

    /* Volume label (corresponds to BS_VolLab) */
    PDM_U_CHAR      vol_label[PDM_BS_VOLUME_LABEL_SIZE];

    /* Filesystem type (corresponds to BS_FilSysType) */
    PDM_U_CHAR      fs_type[PDM_BS_FILE_SYS_TYPE_SIZE];

    /* Count of Sectors occupied by one FAT. (corresponds to BPB_FATSz32) */
    PDM_U_LONG      sectors_per_FAT32;

    /* Extended flags (mirroring and active FAT) (corresponds to BPB_ExtFlags) */
    PDM_U_SHORT     ext_flags;

    /* File system version (corresponds to BPB_FSVer) */
    PDM_U_SHORT     fs_version;

    /* Cluster number of the start cluster of the root directory (corresponds to BPB_RootClus) */
    PDM_U_LONG      root_dir_cluster;

    /* Sector number of FSInfo structure. (corresponds to BPB_FSInfo) */
    PDM_U_SHORT     fs_info_sector;

    /* Sector number of a copy of the Boot Record (corresponds to BPB_BkBootSec) */
    PDM_U_SHORT     backup_boot_sector;

    /* Jump instruction to boot code. (corresponds to BS_jmpBoot) */
    PDM_U_CHAR      jump_boot[PDM_BS_JUMP_BOOT_SIZE];

    /*------------------------------------------------------------------*
      Values computed from the parameters above.
     *------------------------------------------------------------------*/
    /* Number of active FATs (if mirroring enabled) */
    PDM_U_CHAR      num_active_FATs;

    /* Filesystem type (FAT12/16/32) */
    PDM_FAT_TYPE    fat_type;

    /* log2(bytes_per_sector) -- for shifting */
    PDM_U_CHAR      log2_bytes_per_sector;

    /* log2(sectors_per_cluster) -- for shifting */
    PDM_U_CHAR      log2_sectors_per_cluster;

    /* Root directory sectors (FAT12 and FAT16 only) */
    PDM_U_SHORT     num_root_dir_sectors;

    /* Start of active FAT entry (first FAT if mirroring) */
    PDM_U_LONG      active_FAT_sector;

    /* First root directory sector */
    PDM_U_LONG      first_root_dir_sector;

    /* First data sector */
    PDM_U_LONG      first_data_sector;

    /* Count of clusters on volume */
    PDM_U_LONG      num_clusters;

    /* Total number of sectors on volume. */
    PDM_U_LONG      total_sectors;

    /* Count of Sectors occupied by one FAT. */
    PDM_U_LONG      sectors_per_FAT;

    /* Max file size (byte) */
    PDM_FSIZE_T     max_file_size;

    /* Number of clusters of max_file_size */
    PDM_CLST        num_clst_max_fsize;
} PDM_BPB;


/*---------------------------------------------------------------------------
  PDM_BPB_FORMAT - BPB (BIOS Parameter Block) format parameter.
---------------------------------------------------------------------------*/
typedef struct PDM_BPB_FORMAT
{
    /*------------------------------------------------------------------*
      Format Parameters.
     *------------------------------------------------------------------*/
    /* Filesystem type (FAT12/16/32) */
    PDM_FAT_TYPE    fat_type;

    /* Count of bytes per sector. (corresponds to BPB_BytsPerSec) */
    PDM_U_SHORT     bytes_per_sector;

    /* Number of reserved sectors. (corresponds to BPB_RsvdSecCnt) */
    PDM_U_SHORT     num_reserved_sectors;

    /* Sectors per cluster  (corresponds to BPB_SecPerClus) */
    PDM_U_CHAR      sectors_per_cluster;

    /* Count of FAT data structures on the volume. (corresponds to BPB_NumFATs) */
    PDM_U_CHAR      num_fats;

    /* Count of root directory entries. (for FAT12/16) (corresponds to BPB_RootEntCnt) */
    PDM_U_SHORT     num_root_dir_entries;

    /* Total number of sectors on volume. */
    PDM_U_LONG      total_sectors;

    /* Cluster number of the start cluster of the root directory. (for FAT32) (corresponds to BPB_RootClus) */
    PDM_U_LONG      root_dir_cluster;

    /* Sector number of FSInfo structure.  (for FAT32) (corresponds to BPB_FSInfo) */
    PDM_U_SHORT     fs_info_sector;

    /* Extended flags (mirroring and active FAT) (corresponds to BPB_ExtFlags) */
    PDM_U_SHORT     ext_flags;

    /* Volume label (corresponds to BS_VolLab) */
    PDM_U_CHAR      vol_label[PDM_BS_VOLUME_LABEL_SIZE];

    /* Jump instruction to boot code. (corresponds to BS_jmpBoot) */
    PDM_U_CHAR      jump_boot[PDM_BS_JUMP_BOOT_SIZE];

    /* Media descriptor. (corresponds to BPB_Media) */
    PDM_U_CHAR      media;

    /* Drive number (corresponds to BS_DrvNum) */
    PDM_U_CHAR      drive;

    /* OEM name string. (corresponds to BS_OEMName) */
    PDM_U_CHAR      oem_name[PDM_BS_OEM_NAME_SIZE];

    /* Sector per track */
    PDM_U_SHORT     sector_per_track;

    /* Number of heads */
    PDM_U_SHORT     num_heads;

    /* Number of hidden sectors */
    PDM_U_LONG      num_hidden_sectors;

    /* Volume ID (corresponds to BS_VolID) */
    PDM_U_LONG      vol_id;

    /* File system version (for FAT32) (corresponds to BPB_FSVer) */
    PDM_U_SHORT     fs_version;

    /* Sector number of a copy of the Boot Record (for FAT32) (corresponds to BPB_BkBootSec) */
    PDM_U_SHORT     backup_boot_sector;

    /* Filesystem type (corresponds to BS_FilSysType) */
    PDM_U_CHAR      fs_type[PDM_BS_FILE_SYS_TYPE_SIZE];

    /* Boot signature (corresponds to BS_BootSig) */
    PDM_U_CHAR      boot_sig;

} PDM_BPB_FORMAT;


/*---------------------------------------------------------------------------
  PDM_FSINFO - FSINFO parameters
---------------------------------------------------------------------------*/
typedef struct PDM_FSINFO
{
    /*------------------------------------------------------------------*
      Parameters from FSINFO in FSINFO Sector
     *------------------------------------------------------------------*/
    /* Count of the last known free cluster count on the volume */
    PDM_U_LONG      free_count;

    /* Number of the last known free cluster on the volume */
    PDM_U_LONG      next_free;

} PDM_FSINFO;


/*---------------------------------------------------------------------------
  PDM_PART_TBL - Partition table parameters in MBR (Master Boot Recorde).
---------------------------------------------------------------------------*/
typedef struct PDM_PART_TBL
{
    /*------------------------------------------------------------------*
      Parameters in Partition Table
     *------------------------------------------------------------------*/
    /* Boot flag */
    PDM_U_CHAR      boot_flag;

    /* Partition type */
    PDM_U_CHAR      partition_type;

    /* CHS start address */
    PDM_U_SHORT     s_cylinder;
    PDM_U_CHAR      s_head;
    PDM_U_CHAR      s_sector;

    /* CHS end address */
    PDM_U_SHORT     e_cylinder;
    PDM_U_CHAR      e_head;
    PDM_U_CHAR      e_sector;

    /* padding */
    PDM_U_CHAR      pad[2];

    /* LBA start address */
    PDM_U_LONG      lba_start_sector;

    /* LBA number of total sectors */
    PDM_U_LONG      lba_num_sectors;

} PDM_PART_TBL;


/*---------------------------------------------------------------------------
  PDM_MBR - MBR (Master Boot Recorde) parameters.
---------------------------------------------------------------------------*/
typedef struct PDM_MBR
{
    /*------------------------------------------------------------------*
      System Parameters
     *------------------------------------------------------------------*/
    /* current sector number */
    PDM_U_LONG      current_sector;

    /* EPBR base sector number */
    PDM_U_LONG      epbr_base_sector;

    /*------------------------------------------------------------------*
      Parameters in MBR
     *------------------------------------------------------------------*/
    /* Partition table */
    PDM_PART_TBL    partition_table[PDM_MBR_MAX_PARTTBL];

} PDM_MBR;


/*---------------------------------------------------------------------------
  PDM_PART_INFO - 
---------------------------------------------------------------------------*/
typedef struct PDM_PART_INFO {

    /* Start sector in partition */
    PDM_U_LONG      start_sector;

    /* Total sector in partition */
    PDM_U_LONG      total_sector;

    /* Partition type */
    PDM_U_CHAR      partition_type;

} PDM_PART_INFO;


#endif  /* __PDM_COMMON_H__ */