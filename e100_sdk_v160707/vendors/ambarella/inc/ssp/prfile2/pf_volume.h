/*
     Copyright (C) 2004 By eSOL Co.,Ltd. Tokyo, Japan

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
 [pf_volume.h] - Volume Module.
 ****************************************************************************/
#ifndef __PF_VOLUME_H__
#define __PF_VOLUME_H__

#include "prfile2/pfm_module.h"
#include "pf_common.h"
#include "pf_drv.h"
#include "pf_fat.h"
#include "pf_entry.h"
#include "pf_cache.h"
#if PF_EXFAT_SUPPORT
#include "pf_exfat_entry.h"
#include "pf_exfat_volume.h"
#endif /* PF_EXFAT_SUPPORT */

#if defined(__KERNEL_PRKERNELV4__) || defined(_AMBA_KAL_)
/* #include "bios_ep/kernel/prkernelv4/prfile2/pfk_api.h" */
#elif defined __KERNEL_ET_KERNEL__ || defined __KERNEL_T_KERNEL_FOR_MX1__
#include "bios_ep/kernel/tk/prfile2/pft_api.h"
#elif defined __WIN32__
#include "bios_ep/kernel/win32/prfile2/pfk_api.h"
#else
#error The specified kernel is not supported.
#endif

/*------------------------------------------------------------------*
  Cursor (file position indicator)
 *------------------------------------------------------------------*/
typedef struct PF_CURSOR
{
    /* Cursor position in bytes, relative to the start of the file */
    PF_FPOS_T       position;

    /* Current sector number */
    PF_U_LONG       sector;

    /* Current file sector index */
    PF_U_LONG       file_sector_index;

    /* Current byte offset in sector */
    PF_U_SHORT      offset_in_sector;

} PF_CURSOR;


/*------------------------------------------------------------------*
  Directory cursor (directory position indicator)
 *------------------------------------------------------------------*/
typedef struct PF_DIR_CURSOR
{
    /* Current directory sector index */
    PF_U_LONG       physical_entry_index;

    /* Current entry offset from start of directory */
    PF_U_LONG       logical_entry_index;

    /* Current seek offset from current directory I/O */
    PF_U_LONG       logical_seek_index;

} PF_DIR_CURSOR;


/*------------------------------------------------------------------*
  Lock information structure
 *------------------------------------------------------------------*/
/*
 * File lock
 */
typedef struct PF_LOCK
{
    /* File lock mode */
    PF_U_SHORT  mode;

    /* File lock count */
    PF_U_SHORT  count;

    /* File lock waiting count */
    PF_U_LONG   wcount;

    /* User File Descriptor who locked */
    PF_FILE*    owner;

    /* Resource handle for lock status */
    PF_S_LONG   resource;

} PF_LOCK;


#if PF_ACTIVE_DIR_LOCK_ENABLE
/*
 * Directory Lock
 */
struct PF_DLOCK
{
    /* lock mode */
    PF_U_LONG   dl_mode;

    /* locked count */
    PF_U_LONG   dl_count;

    /* locked task-id(s) */
    PF_S_LONG   dl_owner[MAX_DLOCK_OWNER];
};
typedef struct PF_DLOCK  PF_DLOCK_T;
#endif /* PF_ACTIVE_DIR_LOCK_ENABLE*/


/**
 * System Directory Descriptor (SDD) structure. Holds system-level
 * information on an opened directory (directory name and attributes,
 * number of user handles, etc.).
 *
 * User Directory Descriptors hold a pointer to the associated system
 * directory descriptor, used for all directory operations.
 */
struct PF_SDD
{
    /* Syste file descriptor status */
    PF_U_LONG       stat;

    /* Number of user directory descriptors (handles) pointing to this structure */
    PF_U_SHORT      num_handlers;

    /* FAT file descriptor (version, cluster, sector hint) */
    PF_FFD          ffd;

    /* Directory entry for the file (name, attr, cluster) */
    PF_DENT         dir_entry;

#if PF_ACTIVE_DIR_LOCK_ENABLE
    /* Directory lock information */
    PF_DLOCK_T      dlock;

    /* The sub-directory that locks this directory */
    struct PF_SDD   *p_dlock_by;
#endif /* PF_ACTIVE_DIR_LOCK_ENABLE*/

};


/**
 * System File Descriptor (SFD) structure. Holds system-level information
 * on an opened file (file name and attributes, lock status, number of
 * user handles, etc.).
 *
 * User File Descriptors hold a pointer to the associated system file
 * descriptor, used for all file operations.
 */
typedef struct PF_SFD
{
    /* Syste file descriptor status */
    PF_U_LONG       stat;

    /* FAT file descriptor (version, cluster, sector hint) */
    PF_FFD          ffd;

    /* Directory entry for the file (name, attr, cluster) */
    PF_DENT         dir_entry;

    /* File lock information */
    PF_LOCK         lock;

    /* Number of user file descriptors (handles) pointing to this structure */
    PF_U_SHORT      num_handlers;

#if PF_ACTIVE_DIR_LOCK_ENABLE
    /* The sub-directory that locks this file */
    PF_SDD          *p_dlock_by;
#endif /* PF_ACTIVE_DIR_LOCK_ENABLE*/

} PF_SFD;


/**
 * User Directory Descriptor (UDD) structure. Holds user-level information
 * on an opened directory (open flags, cursor position, last sector hint
 * and a pointer to the associated system directory descriptor used for
 * all directory operations).
 */
struct PF_DIR
{
    /* User directory descriptor status */
    PF_U_LONG       stat;

    /* Pointer to associated system directory descriptor */
    PF_SDD*         p_sdd;

    /* Hint for FAT module to last accessed sector */
    PF_FAT_HINT     hint;

    /* Cursor (directory position indicator) */
    PF_DIR_CURSOR   cursor;

#if PF_ACTIVE_DIR_LOCK_ENABLE
    /* The task-id that calling pf_dlock */
    PF_S_LONG       dlock_tskid;
#endif /* PF_ACTIVE_DIR_LOCK_ENABLE*/

};


/**
 * User File Descriptor (UFD) structure. Holds user-level information on
 * an opened file (open status, cursor position, last sector hint and a
 * pointer to the associated system file descriptor used for all file
 * operations).
 */
struct PF_FILE
{
    /* User file descriptor status */
    PF_U_LONG       stat;

    /* 'fopen' (read/write) flags */
    PF_OPEN_MODE    open_mode;

    /* Pointer to associated system file descriptor */
    PF_SFD*         p_sfd;

    /* Hint for FAT module to last accessed sector */
    PF_FAT_HINT     hint;

    PF_LAST_ACCESS  last_access;

    /* Error at last file access */
    PF_ERROR        last_error;

    /* Cursor (file position indicator) */
    PF_CURSOR       cursor;

    /* File lock count */
    PF_U_SHORT      lock_count;

};


#if PF_DIRCACHE_SUPPORT
/*---------------------------------------------------------------------------
 PF_DIRCACHE_INFO - Directory Cache information
 ----------------------------------------------------------------------------*/
struct PF_DIRCACHE_INFO_TAG
{
    /* Absolute pass of directory */
    PF_CHAR     abs_path[LONG_NAME_PATH_BUF_SIZE];

    /* Size of 'abs_path' */
    PF_U_LONG   path_size;

    /* DirectoryEntry */
    PF_DENT     entry;

    /* Position of entry search */
    PF_U_LONG   ppos;

    /* Last access cluster information */
    PF_FAT_HINT hint;
};
typedef struct PF_DIRCACHE_INFO_TAG  PF_DIRCACHE_INFO;
#endif /* PF_DIRCACHE_SUPPORT */


/*---------------------------------------------------------------------------
 PF_CONTEXT - Context information
 ----------------------------------------------------------------------------*/
/*
    [Free Context List]
                            PF_CONTEXT.p_new >>>
                    +---+    +---+    +---+    +---+
    FREE_CTX()   -> |   | -> |   | -> |   | -> |   | -> NULL
                    +---+    +---+    +---+    +---+


    [Using Context List]
                            PF_CONTEXT.p_new >>>
    OLDEST_CTX() -> +---+ -> +---+ -> +---+ -> +---+ -> NULL
                    |   |    |   |    |   |    |   |
            NULL <- +---+ <- +---+ <- +---+ <- +---+ <- NEWEST_CTX()
                        <<< PF_CONTEXT.p_old
*/
typedef struct PF_CONTEXT
{
    /* Context ID */
    PF_S_LONG       context_id;

    /* Current Volume Index */
    PF_S_LONG       cur_vol_idx;

    /* Current Directory */
    PF_DENT         cur_dir[PF_MAX_VOLUME];
#if PF_SEQUENTIAL_FILE_SEARCH_SUPPORT
    PF_U_LONG       cur_dent_idx[PF_MAX_VOLUME];
#endif /* PF_SEQUENTIAL_FILE_SEARCH_SUPPORT*/
#if PF_DIRCACHE_SUPPORT
    /* Last Directory */
    PF_DIRCACHE_INFO   dircache_info;
#endif /* PF_DIRCACHE_SUPPORT */

    /* Last Error */
    PF_ERROR        last_error;
    /* Last Driver Error */
    PF_ERROR        last_driver_error[PF_MAX_VOLUME];
#if PF_FREE_CLUSTER_SEARCH_SUPPORT
    /* Last Module Error */
    PFM_ERROR       last_fcs_error[PF_MAX_VOLUME];
#endif /* PF_FREE_CLUSTER_SEARCH_SUPPORT */

    struct PF_CONTEXT  *p_old;
    struct PF_CONTEXT  *p_new;

} PF_CONTEXT;


/*---------------------------------------------------------------------------
  PF_BPB - BPB (BIOS Parameter Block) and FSInfo parameters, and other
           values derived from them.

    NOTES: This is equivalent of 'FBPBTAB' for PrFILE1.
 ----------------------------------------------------------------------------*/
struct PF_BPB
{
    /*------------------------------------------------------------------*
      Parameters from BPB in Boot Sector
     *------------------------------------------------------------------*/
    /* Count of bytes per sector. (corresponds to BPB_BytsPerSec) */
    PF_U_SHORT      bytes_per_sector;

    /* Number of reserved sectors. (corresponds to BPB_RsvdSecCnt) */
    PF_U_LONG       num_reserved_sectors;

    /* Count of root directory entries. (for FAT12/16) (corresponds to BPB_RootEntCnt) */
    PF_U_SHORT      num_root_dir_entries;

    /* Sectors per cluster  (corresponds to BPB_SecPerClus) */
    PF_U_LONG       sectors_per_cluster;

    /* Count of FAT data structures on the volume. (corresponds to BPB_NumFATs) */
    PF_U_CHAR       num_FATs;

    /* Total number of sectors on volume. (corresponds to BPB_TotSec16 and BPB_TotSec32) */
    PF_U_LONG       total_sectors;

    /* Count of Sectors occupied by one FAT. (corresponds to BPB_FATSz16 and BPB_FATSz32) */
    PF_U_LONG       sectors_per_FAT;

    /* Cluster number of the start cluster of the root directory (for FAT32) (corresponds to BPB_RootClus) */
    PF_U_LONG       root_dir_cluster;

    /* Sector number of FSInfo structure. (for FAT32) (corresponds to BPB_FSInfo) */
    PF_U_SHORT      fs_info_sector;

    /* Sector number of a copy of the Boot Record (for FAT32) (corresponds to BPB_BkBootSec) */
    PF_U_SHORT      backup_boot_sector;

    /* Extended flags (mirroring and active FAT) (for FAT32) (corresponds to BPB_ExtFlags) */
    PF_U_SHORT      ext_flags;

    /* Media descriptor. (corresponds to BPB_Media) */
    PF_U_CHAR       media;

    /*------------------------------------------------------------------*
      Values computed from the parameters above.
     *------------------------------------------------------------------*/
    /* Filesystem type (FAT12/16/32/exFAT) */
    PF_FAT_TYPE     fat_type;

    /* log2(bytes_per_sector) -- for shifting */
    PF_U_CHAR       log2_bytes_per_sector;

    /* log2(sectors_per_cluster) -- for shifting */
    PF_U_CHAR       log2_sectors_per_cluster;

    /* Number of active FATs (if mirroring enabled) */
    PF_U_CHAR       num_active_FATs;

    /* Root directory sectors (FAT12 and FAT16 only) */
    PF_U_SHORT      num_root_dir_sectors;

    /* Start of active FAT entry (first FAT if mirroring) */
    PF_U_LONG       active_FAT_sector;

    /* First root directory sector */
    PF_U_LONG       first_root_dir_sector;

    /* First data sector */
    PF_U_LONG       first_data_sector;

    /* Count of clusters on volume */
    PF_U_LONG       num_clusters;

    /* Max file size (byte) */
    PF_FSIZE_T      max_file_size;

    /* Number of clusters of max_file_size */
    PF_CLST         num_clst_max_fsize;

#if PF_EXFAT_SUPPORT
    /* FileSystemRevision */
    /* Major = revision >> 8;
     * Minor = revision & 0xFF;
     */
    PF_U_SHORT      revision;

    /* PercentInUse (exFAT only) */
    PF_U_CHAR       percent_in_use;
#endif

};

#if PF_BACKWARD_COMPATIBILITY_STRUCTURE

typedef PF_BPB          FBPBTAB;                /* OBSOLETE         */

#endif /* PF_BACKWARD_COMPATIBILITY_STRUCTURE */

typedef void  (*PF_VOL_CALLBACK)(void);

/* Cluster group */
struct PF_CLUSTER_GROUP_TAG
{
    /* Cluster group size */
    PF_U_LONG   clusters_per_group;

    /* Group start cluster number */
    PF_U_LONG   start_cluster;
};
typedef struct PF_CLUSTER_GROUP_TAG PF_CLUSTER_GROUP;


/*---------------------------------------------------------------------------
  PF_VOLUME - Volume Information
 ----------------------------------------------------------------------------*/
struct PF_VOLUME
{
    /*------------------------------------------------------------------*
      BPB Parameters
     *------------------------------------------------------------------*/
    PF_BPB          bpb;

    /*------------------------------------------------------------------*
      Fields related to FSInfo Sector
     *------------------------------------------------------------------*/
    /* Count of free clusters on the volume. (corresponds to FSI_Free_Count)*/
    PF_U_LONG       num_free_clusters;

    /* Count of free cluster groups on the volume. */
    PF_U_LONG       num_free_cluster_groups;

    /* Last allocated free cluster  (corresponds to FSI_Nxt_Free)*/
    PF_U_LONG       last_free_cluster;

    /*------------------------------------------------------------------*
      Fields for FATFS Layer
     *------------------------------------------------------------------*/
    /* System File Descriptors. */
    PF_SFD          sfds[MAX_FILE_SYS_OPEN];

    /* User File Descriptors. */
    PF_FILE         ufds[MAX_FILE_USR_OPEN];

    /* System Directory Descriptors. */
    PF_SDD          sdds[MAX_DIR_SYS_OPEN];

    /* User Directory Descriptors. */
    PF_DIR          udds[MAX_DIR_USR_OPEN];

    /* Number of currently opened files */
    PF_S_LONG       num_opened_files;

    /* Number of currently opened directorys */
    PF_S_LONG       num_opened_directories;

#if PF_ACTIVE_DIR_LOCK_ENABLE
    /* Number of locked directories */
    PF_S_LONG       num_locked_directories;

    /* The directory pointer that locks the root directory */
    PF_SDD          *p_dlock_root_by;
    PF_DIR          *p_dlock_root_dir[MAX_DLOCK_OWNER];
#endif /* PF_ACTIVE_DIR_LOCK_ENABLE*/

    /* Sector Cache */
    PF_SECTOR_CACHE cache;

#if USE_EBINDER
    /* Volume label (null-terminated string) */
    PF_CHAR         label[VOLUME_LABEL_BUF_SIZE+1];
#endif /* USE_EBINDER */

    /* Current context on the volume. */
    PF_CONTEXT      *p_cur_ctx;

    /* Current task-id on the volume. */
    PF_S_LONG       cur_tskid;

    PF_DIR_TAIL     tail_entry;

    /* Configuration for the volume. */
    PF_U_LONG       volume_config;

    /* Configuration for all files in the volume. */
    PF_U_LONG       file_config;

    /* Flags that indicate status of the volume. */
    PF_U_LONG       flags;

    /* Drive letter for the volume. */
    PF_DRV_CHAR     drv_char;

#if PF_CRITICAL_SECTION_ENABLE
    /* The structure to implement critical sections */
    PF_CRITICAL     critical;
#endif /* PF_CRITICAL_SECTION_ENABLE */

    /* FSINFO Flag */
    PF_U_SHORT      fsi_flag;

#if PF_USE_CLSTLINK
    /* Cluster Link */
    PF_CLUSTER_LINK_VOL cluster_link;
#endif /* PF_USE_CLSTLINK */

#if PF_EXFAT_SUPPORT
    /*------------------------------------------------------------------*
      Fields for Allocation Bitmap of exFAT
     *------------------------------------------------------------------*/
    /* Allocation Bitmap Directory Entry Information */
    PF_DENT_INFO    allocate_bmp;

    /* Allocation Bitmap information */
    PF_ALLOCATE_BMP allocate_info;

    /*------------------------------------------------------------------*
      Fields for Up-case Table of exFAT
     *------------------------------------------------------------------*/
    /* Up-case Table Directory Entry Information */
    /* (*) start_cluster member must be initialized to 0. (not CLUSTER_VOID) */
    PF_DENT_INFO    up_case_tbl;

    /* Up-case Table Buffer Information */
    PF_UPCASE_BUF   upcase_buf;
#endif /* PF_EXFAT_SUPPORT */
    /*------------------------------------------------------------------*
      Fields for Driver Layer
     *------------------------------------------------------------------*/
    /* Pointer to a partition */
    void*           p_part;

    /* Call back for FAT read error */
    PF_VOL_CALLBACK p_callback;

    /* Pointer to formatting parameter string for 'Format After Mount' */
    const PF_U_CHAR*  format_param;

#if PF_FREE_CLUSTER_SEARCH_SUPPORT
    /*------------------------------------------------------------------*
      Fields for Free Cluster Search
     *------------------------------------------------------------------*/
    /* The status of FCS */
    PF_U_LONG       fcs_state;

    /* The function table of FCS Module */
    PFM_FCS_IF      *p_fcsm_if;

    /* FCS Pointer */
    PFM_FCSP        fcsd;
#endif /* PF_FREE_CLUSTER_SEARCH_SUPPORT */

    /*------------------------------------------------------------------*
      Fields for Continuous Cluster Write feature
     *------------------------------------------------------------------*/
    PF_CLUSTER_GROUP    cluster_group;

#if PF_EXFAT_SUPPORT
    /*------------------------------------------------------------------*
      Fields for Flash Parameters (OEM Parameters Sub-regions)
     *------------------------------------------------------------------*/
    /* Size of the erase block. */
    PF_U_LONG           erase_block_size;
#endif /* PF_EXFAT_SUPPORT */

};


/*---------------------------------------------------------------------------
 Set of volumes
 ----------------------------------------------------------------------------*/
struct PF_VOLUME_SET
{
    /* Initilalized flag */
    PF_BOOL         is_initialized;

    /* Number of currently attached drives (volumes) */
    PF_S_LONG       num_attached_drives;

    /* Number of currently mounted volumes */
    PF_S_LONG       num_mounted_volumes;

    /* init configuration by pf_init_prfile2() */
    PF_U_LONG       config;

    /* init parameter by pf_init_prfile2() */
    void*           param;

    /* functions for character set */
    PF_CHARCODE     codeset;

    /* Current settings */
    PF_U_LONG       setting;

#if PF_CRITICAL_SECTION_ENABLE
    /* Critical section who block simultanious requests */
    PF_CRITICAL     critical;
#endif /* PF_CRITICAL_SECTION_ENABLE */

#if PF_USE_API_STUB
    /* Current context ID          */
    /* This field for PrFILE2 stub */
#if defined(__KERNEL_THREADX__)
    PF_U_LONG       cur_ctxid;
#else
    PF_S_LONG       cur_ctxid;
#endif
#endif /* PF_USE_API_STUB && (PF_MAX_CONTEXT_NUM > 0) */

    /* Context relations */
    PF_CONTEXT      context[MAX_VALID_CONTEXT_IDX + 1];
    PF_CONTEXT      *p_ctx_new; /* Newest context */
    PF_CONTEXT      *p_ctx_old; /* Oldest context */
    PF_CONTEXT      *p_ctx_free;/* Free context */
#if PF_AUTO_REGCTX_SUPPORT
    PF_S_LONG       ctx_sem_id; /* Semaphore ID for context */
#endif /* PF_AUTO_REGCTX_SUPPORT */

    /* Table containing all mounted volumes */
    PF_VOLUME       volumes[PF_MAX_VOLUME];

#ifdef EB_PRFILE2_SUPPORT_PFS_IF
    PF_TAIL_BUF     *p_tailbuf[PF_MAX_VOLUME];
#endif /* EB_PRFILE2_SUPPORT_PFS_IF */

};


/****************************************************************************
  Variables
 ****************************************************************************/
/*
 * Volume set structure
 *
 * NOTES:
 *  - DO NOT ACCESS THIS GLOBAL VARIABLES DIRECTLY;
 *    Instead, use macro 'CURRENT_VOL_SET()' which returns a pointer to
 *    the variable.
 */
extern PF_VOLUME_SET    pf_vol_set;

#define CURRENT_VOL_SET()                   (&pf_vol_set)


/****************************************************************************
  PF_VOLUME/PF_VOLUME_SET Related Macro definitions

  NOTES:
   - 'p_vol' is a pointer to volume structure, which type is 'PF_VOLUME*'.
   - 'cluster' is a cluster number which type is 'PF_U_LONG'.
   - 'drv_char' is a drive letter which type is 'PF_CHAR'.
   - 'err' is a error number which type is 'PF_ERROR'.
 ****************************************************************************/
/*---------------------------------------------------------------------------
  Macro definitions
 ----------------------------------------------------------------------------*/
/* Signature of descriptor */
#define PF_SIG_SFD                          (0x10000000ul)
#define PF_SIG_UFD                          (0x20000000ul)
#define PF_SIG_SDD                          (0x40000000ul)
#define PF_SIG_UDD                          (0x80000000ul)

/* Status of systme file descriptor (PF_SFD) */
#define PF_SFD_USED                         (0x00000001ul)
#define PF_SFD_VALID                        (0x00000002ul)
#define PF_SFD_MODIFIED                     (0x00000004ul)
#define PF_SFD_ACCESSED                     (0x00000008ul)
#define PF_SFD_STAT_MASK                    (0x0000000Ful)

/* Status of user file descriptor (PF_FILE) */
#define PF_FILE_USED                        (0x00000001ul)
#define PF_FILE_STAT_MASK                   (0x00000001ul)

/* Status of systme directory descriptor (PF_SDD) */
#define PF_SDD_USED                         (0x00000001ul)
#define PF_SDD_VALID                        (0x00000002ul)
#define PF_SDD_STAT_MASK                    (0x00000003ul)

/* Status of user directory descriptor (PF_DIR) */
#define PF_DIR_USED                         (0x00000001ul)
#define PF_DIR_STAT_MASK                    (0x00000001ul)

/* Default volume label */
#define NO_VOLUME_LABEL_MARKER              "NO NAME    "

/* Cluster Count FREE */
#define FSI_FREE_COUNT_UNKNOWN              (0xFFFFFFFFul)
#define FSI_NEXT_FREE_UNKNOWN               (0xFFFFFFFFul)


/*---------------------------------------------------------------------------
  Accessor macros
 ----------------------------------------------------------------------------*/
/* Getter macros */
#define BYTES_PER_SECTOR(p_vol)             ((p_vol)->bpb.bytes_per_sector)
#define NUM_RESERVED_SECTORS(p_vol)         ((p_vol)->bpb.num_reserved_sectors)
#define NUM_ROOT_DIR_ENTRIES(p_vol)         ((p_vol)->bpb.num_root_dir_entries)
#define SECTORS_PER_CLUSTER(p_vol)          ((p_vol)->bpb.sectors_per_cluster)
#define NUM_FATS(p_vol)                     ((p_vol)->bpb.num_FATs)
#define TOTAL_SECTORS(p_vol)                ((p_vol)->bpb.total_sectors)
#define SECTORS_PER_FAT(p_vol)              ((p_vol)->bpb.sectors_per_FAT)
#define ROOT_DIR_CLUSTER(p_vol)             ((p_vol)->bpb.root_dir_cluster)
#define FS_INFO_SECTOR(p_vol)               ((p_vol)->bpb.fs_info_sector)
#define BACKUP_BOOT_SECTOR(p_vol)           ((p_vol)->bpb.backup_boot_sector)
#define EXT_FLAGS(p_vol)                    ((p_vol)->bpb.ext_flags)
#define MEDIA(p_vol)                        ((p_vol)->bpb.media)

#define FAT_TYPE(p_vol)                     ((p_vol)->bpb.fat_type)
#define LOG2_BYTES_PER_SECTOR(p_vol)        ((p_vol)->bpb.log2_bytes_per_sector)
#define LOG2_SECTORS_PER_CLUSTER(p_vol)     ((p_vol)->bpb.log2_sectors_per_cluster)
#define LOG2_BYTES_PER_CLUSTER(p_vol)       (LOG2_BYTES_PER_SECTOR(p_vol) + LOG2_SECTORS_PER_CLUSTER(p_vol))
#define NUM_ACTIVE_FATS(p_vol)              ((p_vol)->bpb.num_active_FATs)
#define NUM_ROOT_DIR_SECTORS(p_vol)         ((p_vol)->bpb.num_root_dir_sectors)
#define ACTIVE_FAT_SECTOR(p_vol)            ((p_vol)->bpb.active_FAT_sector)
#define FIRST_DATA_SECTOR(p_vol)            ((p_vol)->bpb.first_data_sector)
#define NUM_CLUSTERS(p_vol)                 ((p_vol)->bpb.num_clusters)

#define MAX_FILE_SIZE(p_vol)                ((p_vol)->bpb.max_file_size)
#define NUM_CLST_OF_MAX_FILE_SIZE(p_vol)    ((p_vol)->bpb.num_clst_max_fsize)

#define FILE_SYSTEM_REVISION(p_vol)         ((p_vol)->bpb.revision)
#define PERCENT_IN_USE(p_vol)               ((p_vol)->bpb.percent_in_use)

#define NUM_FREE_CLUSTERS(p_vol)            ((p_vol)->num_free_clusters)
#define NUM_FREE_CLUSTGROUPS(p_vol)         ((p_vol)->num_free_cluster_groups)
#define LAST_FREE_CLUSTER(p_vol)            ((p_vol)->last_free_cluster)

#define CLUSTERS_PER_GROUP(p_vol)           ((p_vol)->cluster_group.clusters_per_group)
#define GROUP_START_CLUSTER(p_vol)          ((p_vol)->cluster_group.start_cluster)

/*---------------------------------------------------------------------------
  Check FAT types
 ----------------------------------------------------------------------------*/
#define IS_FAT12(p_vol)                     (FAT_TYPE(p_vol) == FAT_12)
#define IS_FAT16(p_vol)                     (FAT_TYPE(p_vol) == FAT_16)
#define IS_FAT32(p_vol)                     (FAT_TYPE(p_vol) == FAT_32)
#define IS_EXFAT(p_vol)                     (FAT_TYPE(p_vol) == FAT_EX)

/*---------------------------------------------------------------------------
  EXT_FLAGS
 ----------------------------------------------------------------------------*/
/* Current FAT (0 origin) */
#define CURRENT_FAT_FAT(p_vol)              (EXT_FLAGS(p_vol) & 0x07)
#define IS_FAT_MIRRORED(p_vol)              (EXT_FLAGS(p_vol) & 0x80)

#if PF_EXFAT_SUPPORT
#define CURRENT_FAT(p_vol)                  (IS_EXFAT(p_vol) ? CURRENT_FAT_EXFAT(p_vol) : CURRENT_FAT_FAT(p_vol))
#else /* !PF_EXFAT_SUPPORT */
#define CURRENT_FAT(p_vol)                  CURRENT_FAT_FAT(p_vol)
#endif /* PF_EXFAT_SUPPORT */

/*---------------------------------------------------------------------------
  Values computed from BPB fields
 ----------------------------------------------------------------------------*/
/* Start sector of FAT Region */
#define FAT_REGION_START_SECTOR(p_vol)      NUM_RESERVED_SECTORS(p_vol)

/* Start sector of 'n'-th FAT */
#define FAT_START_SECTOR(p_vol,n)           (FAT_REGION_START_SECTOR(p_vol) + \
                                             (n) * SECTORS_PER_FAT(p_vol))

/* Start sector of Root Directory Region (for FAT12/16) */
#define ROOT_DIR_REGION_START_SECTOR(p_vol) ((p_vol)->bpb.first_root_dir_sector)


/*---------------------------------------------------------------------------
  Helper macros using BPB fields
 ----------------------------------------------------------------------------*/
#define BYTES_TO_SECTORS(p_vol,bytes)                               \
    ((PF_SECT)((((PF_FSIZE_T)(bytes)) >> LOG2_BYTES_PER_SECTOR(p_vol))  \
               + (((((PF_FSIZE_T)(bytes)) & ((PF_FSIZE_T)BYTES_PER_SECTOR(p_vol) - 1)) != 0) ? 1 : 0)))

#define BYTES_TO_SECTOR_IDX(p_vol,bytes)            \
    (PF_SECT)(((PF_FSIZE_T)(bytes)) >> LOG2_BYTES_PER_SECTOR(p_vol))

#define BYTES_TO_OFFSET_IN_SECTOR(p_vol,bytes)      \
    (PF_U_SHORT)(((PF_FSIZE_T)(bytes)) & (PF_FSIZE_T)(BYTES_PER_SECTOR(p_vol)-1UL))

#define SECTORS_TO_OFFSET_IN_CLUSTER(p_vol,sects)   \
    (PF_SECT)(((PF_SECT)(sects)) & (PF_SECT)(SECTORS_PER_CLUSTER(p_vol)-1UL))

#define BYTES_TO_CLUSTERS(p_vol, bytes)                                 \
    ((PF_CLST) ((((PF_FSIZE_T)(bytes)) >> LOG2_BYTES_PER_CLUSTER(p_vol))    \
                + (((((PF_FSIZE_T)(bytes)) & ((PF_FSIZE_T)BYTES_PER_CLUSTER(p_vol) - 1)) != 0) ? 1 : 0)))

#define BYTES_TO_CLUSTER_IDX(p_vol, bytes)                          \
    (PF_CLST)(((PF_FSIZE_T)(bytes)) >> LOG2_BYTES_PER_CLUSTER(p_vol))

#define SECTORS_TO_BYTES(p_vol,sects)               \
    (PF_FSIZE_T)(((PF_FSIZE_T)(sects)) << LOG2_BYTES_PER_SECTOR(p_vol))

#define CLUSTERS_TO_BYTES(p_vol,clsts)              \
    ((PF_FSIZE_T)(((PF_FSIZE_T)(clsts)) << LOG2_BYTES_PER_CLUSTER(p_vol)))

#define CLUSTERS_TO_SECTORS(p_vol,clsts)            \
    (PF_SECT)(((PF_SECT)(clsts)) << LOG2_SECTORS_PER_CLUSTER(p_vol))

#define BYTES_PER_CLUSTER(p_vol)                    \
    CLUSTERS_TO_BYTES(p_vol,1UL)

#define BYTES_PER_CLUSTER_GROUP(p_vol)              \
    (PF_FSIZE_T)((PF_FSIZE_T)CLUSTERS_PER_GROUP(p_vol) << LOG2_BYTES_PER_CLUSTER(p_vol))


/*---------------------------------------------------------------------------
  BPB parameter related macros
 ----------------------------------------------------------------------------*/
#define MAX_BPB_COUNT                       (2)

#define LOWER_BOUND_CLUSTER(p_vol)          (1)
#define MIN_VALID_CLUSTER(p_vol)            (2)
#define MAX_VALID_CLUSTER(p_vol)            (NUM_CLUSTERS(p_vol) + 1)
#define UPPER_BOUND_CLUSTER(p_vol)          (NUM_CLUSTERS(p_vol) + 2)

#define IS_CLUSTER_NUMBER_VALID(p_vol,cluster) \
            (2 <= (cluster) && (cluster) < NUM_CLUSTERS(p_vol) + 2)

#define CLUSTER_TO_SECTOR(p_vol,cluster) \
            (FIRST_DATA_SECTOR(p_vol) \
             + (((cluster)-2) << LOG2_SECTORS_PER_CLUSTER(p_vol)))

#define SECTOR_TO_CLUSTER(p_vol,sector) \
            ((((sector) - FIRST_DATA_SECTOR(p_vol)) >> LOG2_SECTORS_PER_CLUSTER(p_vol)) + 2)

#define CLUSTGROUP_ALIGN_OFFSET(p_vol,clst)    \
            (((clst) - GROUP_START_CLUSTER(p_vol)) % CLUSTERS_PER_GROUP(p_vol))

#define CLUSTGROUP_END_OFFSET(p_vol,clst)   \
            (((clst) - (GROUP_START_CLUSTER(p_vol) + CLUSTERS_PER_GROUP(p_vol) - 1)) % CLUSTERS_PER_GROUP(p_vol))

/*---------------------------------------------------------------------------
  drv_char
 ----------------------------------------------------------------------------*/
#define VOID_DRV_IDX                (-1)

#define IS_VALID_VOL_IDX(vol_idx) \
            (0 <= (vol_idx) && (vol_idx) < PF_MAX_VOLUME)

#define IS_DRV_CHAR_VALID(drv_char) \
            ('A' <= (drv_char) && (drv_char) <= 'Z')

#define VOL_IDX_TO_DRV_CHAR(vol_idx)    ((PF_CHAR)('A' + (vol_idx)))
#define DRV_CHAR_TO_VOL_IDX(drv_char)   (pf_toupper(drv_char) - 'A')

/*---------------------------------------------------------------------------
  flags
 ----------------------------------------------------------------------------*/
/*
 * Bit flags
 */
#define B_ATTACHED                          ((PF_U_LONG)0x00000001ul)
#define B_MOUNTED                           ((PF_U_LONG)0x00000002ul)
#define B_FORMAT_AFTER_MOUNT_REQ            ((PF_U_LONG)0x00000010ul)
#define B_NO_FORMATTING_BY_FATFS_LAYER_REQ  ((PF_U_LONG)0x00000020ul)
#define B_DATA_ERASE_REQ                    ((PF_U_LONG)0x00000040ul)
#define B_WRITE_PROTECTED                   ((PF_U_LONG)0x00000100ul)
#define B_REMOVABLE                         ((PF_U_LONG)0x00000200ul)

/*
 * Attached/Detached
 */
#define IS_ATTACHED(p_vol)                  (((p_vol)->flags &  B_ATTACHED) != 0)
#define SET_ATTACHED(p_vol)                 ((p_vol)->flags |=  B_ATTACHED)
#define CLEAR_ATTACHED(p_vol)               ((p_vol)->flags &= ~B_ATTACHED)

/*
 * Mount/Unmount
 */
#define IS_MOUNTED(p_vol)                   (((p_vol)->flags &  B_MOUNTED) != 0)
#define SET_MOUNTED(p_vol)                  ((p_vol)->flags |=  B_MOUNTED)
#define CLEAR_MOUNTED(p_vol)                ((p_vol)->flags &= ~B_MOUNTED)

/*
 * Media insert/eject/detect
 */
#define IS_INSERTED(p_vol)                  (PFDRV_IsInserted(p_vol) != PF_FALSE)
#define IS_DETECTED(p_vol)                  (PFDRV_IsDetected(p_vol) != PF_FALSE)

/*
 * Write-protected
 */
#define IS_WRITE_PROTECTED(p_vol)           (((p_vol)->flags &  B_WRITE_PROTECTED) != 0)
#define SET_WRITE_PROTECTED(p_vol)          ((p_vol)->flags |=  B_WRITE_PROTECTED)
#define CLEAR_WRITE_PROTECTED(p_vol)        ((p_vol)->flags &= ~B_WRITE_PROTECTED)

/*
 * Removable Media
 */
#define IS_REMOVABLE(p_vol)                 (((p_vol)->flags &  B_REMOVABLE) != 0)
#define SET_REMOVABLE(p_vol)                ((p_vol)->flags |=  B_REMOVABLE)
#define CLEAR_REMOVABLE(p_vol)              ((p_vol)->flags &= ~B_REMOVABLE)

/*
 * Formatting
 */
#define IS_FORMAT_AFTER_MOUNT_REQUESTED(p_vol)  (((p_vol)->flags &  B_FORMAT_AFTER_MOUNT_REQ) != 0)
#define SET_FORMAT_AFTER_MOUNT_REQ(p_vol)       ((p_vol)->flags |=  B_FORMAT_AFTER_MOUNT_REQ)
#define CLEAR_FORMAT_AFTER_MOUNT_REQ(p_vol)     ((p_vol)->flags &= ~B_FORMAT_AFTER_MOUNT_REQ)

#define IS_NO_FORMATTING_BY_FATFS_LAYER(p_vol)  (((p_vol)->flags &  B_NO_FORMATTING_BY_FATFS_LAYER_REQ) != 0)
#define SET_NO_FORMATTING_BY_FATFS_LAYER(p_vol) ((p_vol)->flags |=  B_NO_FORMATTING_BY_FATFS_LAYER_REQ)
#define CLEAR_NO_FORMATTING_BY_FATFS_LAYER(p_vol) ((p_vol)->flags &= ~B_NO_FORMATTING_BY_FATFS_LAYER_REQ)

/*
 * Data erasing
 */
#define IS_DATA_ERASE_REQUESTED(p_vol)          (((p_vol)->flags &  B_DATA_ERASE_REQ) != 0)
#define SET_DATA_ERASE_REQ(p_vol)               ((p_vol)->flags |=  B_DATA_ERASE_REQ)
#define CLEAR_DATA_ERASE_REQ(p_vol)             ((p_vol)->flags &= ~B_DATA_ERASE_REQ)

/*
 * Combinations
 */
#define IS_ACCESSIBLE(p_vol)                (IS_ATTACHED(p_vol) && IS_INSERTED(p_vol))
#define IS_READABLE(p_vol)                  (IS_ATTACHED(p_vol) && IS_INSERTED(p_vol))
#define IS_WRITABLE(p_vol)                  (IS_ATTACHED(p_vol) && IS_INSERTED(p_vol) && \
                                             !IS_WRITE_PROTECTED(p_vol))

/*
 *FSINFO flag
 */
#define B_TRUST_FSI                         (0x0001ul)
#define B_REFRESH_FSI                       (0x0002ul)
#define B_FREE_CLUSTER_VALID                (0x0004ul)

#define IS_TRUST_FSI(p_vol)                 (((p_vol)->fsi_flag &  B_TRUST_FSI) != 0)
#define SET_TRUST_FSI(p_vol)                (p_vol)->fsi_flag |= B_TRUST_FSI
#define CLEAR_TRUST_FSI(p_vol)              (p_vol)->fsi_flag &= ~B_TRUST_FSI

#define IS_REFRESH_FSI(p_vol)               (((p_vol)->fsi_flag &  B_REFRESH_FSI) != 0)
#define SET_REFRESH_FSI(p_vol)              (p_vol)->fsi_flag |= B_REFRESH_FSI
#define CLEAR_REFRESH_FSI(p_vol)            (p_vol)->fsi_flag &= ~B_REFRESH_FSI

#define IS_FREE_CLUSTER_VALID(p_vol)        (((p_vol)->fsi_flag & B_FREE_CLUSTER_VALID) != 0)
#define SET_FREE_CLUSTER_VALID(p_vol)       (p_vol)->fsi_flag |= B_FREE_CLUSTER_VALID
#define CLEAR_FREE_CLUSTER_VALID(p_vol)     (p_vol)->fsi_flag &= ~B_FREE_CLUSTER_VALID

/*
 *Cluster Link
 */
#if PF_USE_CLSTLINK
  #define B_CLUSTER_LINK_VALID              (0x0001ul)

  #define IS_CLUSTER_LINK_VALID(p_vol)      (p_vol)->cluster_link.flag &  B_CLUSTER_LINK_VALID
  #define SET_CLUSTER_LINK_VALID(p_vol)     (p_vol)->cluster_link.flag |= B_CLUSTER_LINK_VALID
  #define CLEAR_CLUSTER_LINK_VALID(p_vol)   (p_vol)->cluster_link.flag &= ~B_CLUSTER_LINK_VALID

  #define CLUSTER_LINK_INTERVAL(p_vol)      (p_vol)->cluster_link.interval
  #define CLUSTER_LINK_BUFFER(p_vol)        (p_vol)->cluster_link.buffer
  #define CLUSTER_LINK_MAX(p_vol)           (p_vol)->cluster_link.link_max
#endif /* PF_USE_CLSTLINK*/

/*
 *Cache Signature
 */
#define SET_CACHE_SIGNATURE(p_vol, fd)      (p_vol)->cache.signature = (void*)fd
#define GET_CACHE_SIGNATURE(p_vol)          (p_vol)->cache.signature
#define CLEAR_CACHE_SIGNATURE(p_vol)        (p_vol)->cache.signature = NULL

/*
 *Cache Write Mode
 */
#define SECTOR_CACHE_WRITE_THROUGH_ON(p_vol)    (p_vol)->cache.mode |= PF_CACHE_WRITE_THROUGH
#define SECTOR_CACHE_WRITE_THROUGH_OFF(p_vol)   (p_vol)->cache.mode &= ~PF_CACHE_WRITE_THROUGH
#define IS_SECTOR_CACHE_WRITE_THROUGH_ON(p_vol) ((p_vol)->cache.mode & PF_CACHE_WRITE_THROUGH)

/*
 *Cache Synchronized Mode
 */
#define SECTOR_CACHE_AUTO_SYNC_ON(p_vol)        (p_vol)->cache.mode |= PF_CACHE_AUTO_SYNC
#define SECTOR_CACHE_AUTO_SYNC_OFF(p_vol)       (p_vol)->cache.mode &= ~PF_CACHE_AUTO_SYNC
#define IS_SECTOR_CACHE_AUTO_SYNC_ON(p_vol)     ((p_vol)->cache.mode & PF_CACHE_AUTO_SYNC)

/*
 *FAT Cache Synchronized Mode
 */
#define SECTOR_FAT_CACHE_FORCE_SYNC_ON(p_vol)       (p_vol)->cache.mode |= PF_CACHE_FAT_AUTO_SYNC
#define SECTOR_FAT_CACHE_FORCE_SYNC_OFF(p_vol)      (p_vol)->cache.mode &= ~PF_CACHE_FAT_AUTO_SYNC
#define IS_SECTOR_FAT_CACHE_FORCE_SYNC_ON(p_vol)    ((p_vol)->cache.mode & PF_CACHE_FAT_AUTO_SYNC)

/*
 *FAT Cache Size
 */
#define SECTOR_FAT_CACHE_BUFF_SIZE(p_vol)           (p_vol)->cache.fat_buff_size
#define SET_SECTOR_FAT_CACHE_BUFF_SIZE(p_vol, size) SECTOR_FAT_CACHE_BUFF_SIZE(p_vol) = (size)

/*
 *Data Cache Size
 */
#define SECTOR_DATA_CACHE_BUFF_SIZE(p_vol)           (p_vol)->cache.data_buff_size
#define SET_SECTOR_DATA_CACHE_BUFF_SIZE(p_vol, size) SECTOR_DATA_CACHE_BUFF_SIZE(p_vol) = (size)

/*
 *Encode Mode
 */
#define CLEAR_ENCODE_MODE()                 (pf_vol_set.setting &= ~PF_ENCODE_MODE_ALL)
#define SET_ENCODE_MODE_UTF16()             { CLEAR_ENCODE_MODE(); pf_vol_set.setting |= PF_ENCODE_MODE_UTF16; }
#define SET_ENCODE_MODE_UTF16_EXT()        { CLEAR_ENCODE_MODE(); pf_vol_set.setting |= PF_ENCODE_MODE_UTF16_EXT; }
#define IS_ENCODE_MODE_UTF16_EXT()         ((pf_vol_set.setting & PF_ENCODE_MODE_UTF16_EXT) == \
                                             PF_ENCODE_MODE_UTF16_EXT)
/*
 * VolumeSet configurations
 */
#if PF_EXFAT_SUPPORT
#define IS_UPDATE_VOLUME_DIRTY()        ((pf_vol_set.config & PF_UPDATE_VOLUME_DIRTY_ENABLE) == PF_UPDATE_VOLUME_DIRTY_ENABLE)
#define IS_CHECK_VOLUME_DIRTY()         ((pf_vol_set.config & PF_CHECK_VOLUME_DIRTY_ENABLE) == PF_CHECK_VOLUME_DIRTY_ENABLE)
#endif /* PF_EXFAT_SUPPORT */

/*
 * Volume configuration
 */
#define PF_CHAR_CHECK_MASK              (PF_CHAR_CHECK_DISABLE|PF_CHAR_CHECK_ENABLE)
#define PF_VOLCFG_VOLUME_SET_MASK       (PF_CHAR_CHECK_MASK)
#define PF_VOLCFG_VOLUME_MASK           (PF_SET_FAT_CACHE_SIZE|PF_SET_DATA_CACHE_SIZE)

#define PF_CONT_CLUSTER_MASK            (PF_CONT_CLUSTER_ENABLE           | \
                                         PF_CONT_CLUSTER_DISABLE          | \
                                         PF_CONT_CLUSTER_DIS_NOT_DRIVER   | \
                                         PF_CONT_CLUSTER_DIS_NOT_BPS      | \
                                         PF_CONT_CLUSTER_DIS_NOT_BOUNDARY | \
                                         PF_CONT_CLUSTER_DIS_NOT_DIVIDE   | \
                                         PF_CONT_CLUSTER_DIS_NOT_CLUSTER)

#define IS_CONT_CLUSTER_ENABLE(p_vol)   (((p_vol)->volume_config & PF_CONT_CLUSTER_ENABLE)  \
                                         == PF_CONT_CLUSTER_ENABLE)
#define IS_CONT_CLUSTER_DISABLE(p_vol)  (((p_vol)->volume_config & PF_CONT_CLUSTER_DISABLE) \
                                         == PF_CONT_CLUSTER_DISABLE)

#define SET_CONT_CLUSTER_DIS_NOT_CLUSTER(p_vol)                                         \
                        ((p_vol)->volume_config |= PF_CONT_CLUSTER_DIS_NOT_CLUSTER)
#define CLEAR_CONT_CLUSTER_DIS_NOT_CLUSTER(p_vol)                                       \
                        ((p_vol)->volume_config &= ~PF_CONT_CLUSTER_DIS_NOT_CLUSTER)
#define IS_CONT_CLUSTER_DIS_NOT_CLUSTER(p_vol)                                          \
                        (((p_vol)->volume_config & PF_CONT_CLUSTER_DIS_NOT_CLUSTER)     \
                         == PF_CONT_CLUSTER_DIS_NOT_CLUSTER)

/* File configuration of volume */
#define PF_UPDATE_ACCESS_DATE_MASK      (PF_UPDATE_ACCESS_DATE_DISABLE|PF_UPDATE_ACCESS_DATE_ENABLE)
#define PF_WRITE_TIMESTMP_MASK          (PF_WRITE_TIMESTMP_AT_UPDATE|PF_WRITE_TIMESTMP_AT_FCLOSE)
#define PF_VOLCFG_FILE_MASK             (PF_UPDATE_ACCESS_DATE_MASK|PF_WRITE_TIMESTMP_MASK)

#define IS_ACCESS_DATE_DISABLE(p_vol)   (((p_vol)->file_config & PF_UPDATE_ACCESS_DATE_DISABLE) \
                                         == PF_UPDATE_ACCESS_DATE_DISABLE)
#define IS_TIMESTMP_AT_UPDATE(p_vol)    (((p_vol)->file_config & PF_WRITE_TIMESTMP_AT_UPDATE)  \
                                         == PF_WRITE_TIMESTMP_AT_UPDATE)

/*---------------------------------------------------------------------------
  Context
 ----------------------------------------------------------------------------*/
#define SYSTEM_CONTEXT_IDX                  (0)
#define MIN_USER_CONTEXT_IDX                (1)
#define MAX_USER_CONTEXT_IDX                MAX_VALID_CONTEXT_IDX

#define SYSTEM_CONTEXT()                    (&pf_vol_set.context[SYSTEM_CONTEXT_IDX])

#define CURRENT_CONTEXT(p_vol)              ((p_vol)->p_cur_ctx)
#define CURRENT_DIRECTORY(p_ctx)            ((p_ctx)->cur_dir[(p_ctx)->cur_vol_idx])
#define CURRENT_DENT_IDX(p_ctx)             ((p_ctx)->cur_dent_idx[(p_ctx)->cur_vol_idx])
#define CURRENT_VOLUME(p_ctx)               ((p_ctx)->cur_dir[(p_ctx)->cur_vol_idx].p_vol)

#define SET_CURRENT_VOLUME(p_ctx,p_vol)     ((p_ctx)->cur_vol_idx = DRV_CHAR_TO_VOL_IDX((p_vol)->drv_char))

#if PF_DIRCACHE_SUPPORT
#define DIRCACHE_INFO(p_ctx)                    (&((p_ctx)->dircache_info))
#define IS_DIRCACHE_INFO_VALID(p_dircache_info) (((p_dircache_info) != NULL)     \
                                                 && (0 < (p_dircache_info)->path_size))
#endif /* PF_DIRCACHE_SUPPORT */

#define NEWEST_CTX()                        (pf_vol_set.p_ctx_new)
#define OLDEST_CTX()                        (pf_vol_set.p_ctx_old)
#define FREE_CTX()                          (pf_vol_set.p_ctx_free)

/* Get/Release Semaphore for Context */
#if PF_AUTO_REGCTX_SUPPORT
#define GET_CTX_SEMAPHORE()                 (pfk_get_semaphore(pf_vol_set.ctx_sem_id))
#define RELEASE_CTX_SEMAPHORE()             (pfk_release_semaphore(pf_vol_set.ctx_sem_id))
#else /* ! PF_AUTO_REGCTX_SUPPORT */
#define GET_CTX_SEMAPHORE()                 do{;}while(0)
#define RELEASE_CTX_SEMAPHORE()             do{;}while(0)
#endif /* PF_AUTO_REGCTX_SUPPORT */

/* Get/Release Current context */
#if PF_AUTO_REGCTX_SUPPORT
#define GET_CURRENT_CONTEXT(p_tskid)        (PFCTX_regctx(p_tskid))
#define RELEASE_CURRENT_CONTEXT(p_ctx)      RELEASE_CTX_SEMAPHORE()
#else /* !PF_AUTO_REGCTX_SUPPORT */
#define GET_CURRENT_CONTEXT(p_tskid)        (PFCTX_GetCurrentContext(p_tskid))
#define RELEASE_CURRENT_CONTEXT(p_ctx)      do{;}while(0)
#endif /* PF_AUTO_REGCTX_SUPPORT */

/*---------------------------------------------------------------------------
  Task
 ----------------------------------------------------------------------------*/
#define CURRENT_TASKID(p_vol)               ((p_vol)->cur_tskid)

/*---------------------------------------------------------------------------
  last_error/last_driver_error
 ----------------------------------------------------------------------------*/

/* Set error */
#define CTX_LAST_ERROR(p_ctx)                   ((p_ctx)->last_error)
#define CTX_SET_ERROR(p_ctx, err)               ((p_ctx)->last_error = (PF_ERROR)(err))

/* Set file error */
#define SET_FILE_ERROR(p_file,err)              ((p_file)->last_error = CTX_SET_ERROR(CURRENT_CONTEXT(p_file->p_sfd->ffd.p_vol), (PF_ERROR)(err)))

/* Set driver error */
#define LAST_DRIVER_ERROR(p_vol)                (CURRENT_CONTEXT(p_vol)->last_driver_error[DRV_CHAR_TO_VOL_IDX((p_vol)->drv_char)])
#define SET_VOLUME_DRIVER_ERROR(p_vol,err)      (CURRENT_CONTEXT(p_vol)->last_driver_error[DRV_CHAR_TO_VOL_IDX((p_vol)->drv_char)] = (PF_ERROR)(err))

/* Set error (for pf2_xxx() functions only) */
#if PF_MAX_CONTEXT_NUM == 0
#define SET_ERROR(err)                          (SYSTEM_CONTEXT()->last_error = (PF_ERROR)(err))
#else /* if (0 < PF_MAX_CONTEXT_NUM) */
#define SET_ERROR(err)                          (PFCTX_SetLastErrorToCurrentCtx(err))
#endif

/*---------------------------------------------------------------------------
  Free Cluster Fast Search
 ----------------------------------------------------------------------------*/
#if PF_FREE_CLUSTER_SEARCH_SUPPORT

/* getter */
#define FCSM(p_vol)                         ((p_vol)->p_fcsm_if)

/* status */
#define FCS_STAT_UNREGISTERED               (0x00000000ul)
#define FCS_STAT_REGISTERED                 (0x00000001ul)
#define FCS_STAT_MOUNTED                    (0x00000002ul)

#define SET_FCSM_UNREGISTERED(p_vol)        (((p_vol)->fcs_state) = FCS_STAT_UNREGISTERED)
#define SET_FCSM_REGISTERED(p_vol)          (((p_vol)->fcs_state) = FCS_STAT_REGISTERED)
#define SET_FCSM_MOUNTED(p_vol)             (((p_vol)->fcs_state) = (FCS_STAT_REGISTERED | FCS_STAT_MOUNTED))

#define IS_FCSM_UNREGISTERED(p_vol)         (((p_vol)->fcs_state) == FCS_STAT_UNREGISTERED)
#define IS_FCSM_REGISTERED(p_vol)           ((((p_vol)->fcs_state) & FCS_STAT_REGISTERED) == FCS_STAT_REGISTERED)
#define IS_FCSM_MOUNTED(p_vol)              ((((p_vol)->fcs_state) & FCS_STAT_MOUNTED) == FCS_STAT_MOUNTED)

/* last error */
#define FCSM_LAST_ERROR(p_vol)              (CURRENT_CONTEXT(p_vol)->last_fcs_error[DRV_CHAR_TO_VOL_IDX((p_vol)->drv_char)])
#define FCSM_SET_VOLUME_ERROR(p_vol,merr)   (CURRENT_CONTEXT(p_vol)->last_fcs_error[DRV_CHAR_TO_VOL_IDX((p_vol)->drv_char)] = merr)

#else /* PF_FREE_CLUSTER_SEARCH_SUPPORT */

#define IS_FCSM_UNREGISTERED(p_vol)         (0)
#define IS_FCSM_REGISTERED(p_vol)           (0)
#define IS_FCSM_MOUNTED(p_vol)              (0)

#endif /* PF_FREE_CLUSTER_SEARCH_SUPPORT */


/****************************************************************************
  Prototypes
 ****************************************************************************/
/*---------------------------------------------------------------------------
 PFVOL_InitModule - initialize module-local variables of Volume Module.
 ----------------------------------------------------------------------------*/
PF_ERROR  PFVOL_InitModule(PF_U_LONG config, void* param);

/*---------------------------------------------------------------------------
  PFVOL_CheckForRead -
 ----------------------------------------------------------------------------*/
PF_ERROR  PFVOL_CheckForRead(PF_VOLUME* p_vol);

/*---------------------------------------------------------------------------
  PFVOL_CheckForWrite -
 ----------------------------------------------------------------------------*/
PF_ERROR  PFVOL_CheckForWrite(PF_VOLUME* p_vol);

/*---------------------------------------------------------------------------
  PFVOL_CheckCurrentDir - Check the current directory on the specified volume.
 ----------------------------------------------------------------------------*/
PF_ERROR  PFVOL_CheckCurrentDir(PF_VOLUME* p_vol, PF_U_LONG start_cluster);

/*---------------------------------------------------------------------------
  PFVOL_SetCurrentDir - Set the current directory on the specified volume.
 ----------------------------------------------------------------------------*/
PF_ERROR  PFVOL_SetCurrentDir(PF_VOLUME *p_vol, PF_DENT *p_cur_dir);

/*---------------------------------------------------------------------------
  PFVOL_GetCurrentDir - Get the current directory on the specified volume.
 ----------------------------------------------------------------------------*/
PF_ERROR  PFVOL_GetCurrentDir(PF_VOLUME *p_vol, PF_DENT *p_cur_dir);

#if PF_SEQUENTIAL_FILE_SEARCH_SUPPORT
/*---------------------------------------------------------------------------
  PFVOL_GetCurrentDirStartCluster
 ----------------------------------------------------------------------------*/
PF_ERROR  PFVOL_GetCurrentDirStartCluster(PF_VOLUME *p_vol, PF_U_LONG *p_start_cluster);

/*---------------------------------------------------------------------------
  PFVOL_InitCurrentDirIdxDentLast
 ----------------------------------------------------------------------------*/
void  PFVOL_InitCurrentDirIdxDentLast(PF_VOLUME *p_vol);

/*---------------------------------------------------------------------------
  PFVOL_GetCurrentDirIdxDentLast
 ----------------------------------------------------------------------------*/
PF_ERROR  PFVOL_GetCurrentDirIdxDentLast(PF_VOLUME *p_vol, PF_U_LONG *p_idx_dent_last);

/*---------------------------------------------------------------------------
  PFVOL_SetCurrentDirIdxDentLast
 ----------------------------------------------------------------------------*/
PF_ERROR  PFVOL_SetCurrentDirIdxDentLast(PF_VOLUME *p_vol, PF_U_LONG idx_dent_last);
#endif /* PF_SEQUENTIAL_FILE_SEARCH_SUPPORT*/

#if PF_DIRCACHE_SUPPORT
/*---------------------------------------------------------------------------
  PFVOL_InitDirCache - Initialize the directory cache of the volume.
 ----------------------------------------------------------------------------*/
void  PFVOL_InitDirCache(PF_VOLUME *p_vol);

/*---------------------------------------------------------------------------
  PFVOL_GetDirCache - Get the directory cache of the current context.
 ----------------------------------------------------------------------------*/
PF_ERROR  PFVOL_GetDirCache(PF_VOLUME *p_vol, PF_DIRCACHE_INFO **pp_dircache_info);

/*---------------------------------------------------------------------------
  PFVOL_SetDirCache - Set the directory cache of the current context.
 ----------------------------------------------------------------------------*/
PF_ERROR  PFVOL_SetDirCache(PF_VOLUME *p_vol, PF_CONST PF_DIRCACHE_INFO *p_dircache_info);
#endif /* #if PF_DIRCACHE_SUPPORT */

/*---------------------------------------------------------------------------
  PFVOL_GetCurrentVolume - get PF_VOLUME structure of the current volume.
 ----------------------------------------------------------------------------*/
PF_VOLUME*  PFVOL_GetCurrentVolume(void);

/*---------------------------------------------------------------------------
  PFCTX_NotifyUpdateDirectory
 ----------------------------------------------------------------------------*/
void  PFCTX_NotifyUpdateDirectory(PF_VOLUME *p_vol, PF_DENT *p_dir);

#if ! PF_AUTO_REGCTX_SUPPORT
/*---------------------------------------------------------------------------
  PFCTX_GetCurrentContext - get PF_CONTEXT structure of the current context.
 ----------------------------------------------------------------------------*/
PF_CONTEXT*  PFCTX_GetCurrentContext(PF_S_LONG *p_tskid);
#endif /* ! PF_AUTO_REGCTX_SUPPORT */

/*---------------------------------------------------------------------------
  PFVOL_GetVolumeFromDrvChar - Get PF_VOLUME structure of the volume
                               specified by 'drv_char'.
 ----------------------------------------------------------------------------*/
PF_VOLUME*  PFVOL_GetVolumeFromDrvChar(PF_DRV_CHAR drv_char);

/*---------------------------------------------------------------------------
  PFVOL_NotifyUpdateDirectory - Notify the update directory on the specified volume.
 ----------------------------------------------------------------------------*/
void  PFVOL_NotifyUpdateDirectory(PF_VOLUME *p_vol, PF_DENT *p_dir);

/*---------------------------------------------------------------------------
  PFVOL_NotifyRemoveDirectory
 ----------------------------------------------------------------------------*/
void  PFVOL_NotifyRemoveDirectory(PF_VOLUME *p_vol, PF_DENT *p_dir);

/*---------------------------------------------------------------------------
  PFVOL_NotifyMoveDirectory
 ----------------------------------------------------------------------------*/
void  PFVOL_NotifyMoveDirectory(PF_VOLUME *p_vol, PF_DENT *p_src_dir, PF_DENT *p_dst_dir);

#if PF_EXAPI_SUPPORT
/*---------------------------------------------------------------------------
  PFVOL_GetVolumeFromFatd = Get PF_VOLUME* specified by `fatd'.
 ----------------------------------------------------------------------------*/
PF_VOLUME*  PFVOL_GetVolumeFromFatd(PFM_FATD fatd);
#endif /* PF_EXAPI_SUPPORT */

#if PF_FATINFO_API_SUPPORT
/*---------------------------------------------------------------------------
  PFVOL_fat_fatdIsInternal - Check whether the PrF2 ExAPI is called from inside of the PrF2 API.
 ----------------------------------------------------------------------------*/
PFM_S_LONG  PFVOL_fat_fatdIsInternal(PFM_FATD fatd);

/*---------------------------------------------------------------------------
  PFVOL_fat_getBPB - Return BPB information
 ----------------------------------------------------------------------------*/
PFM_ERROR  PFVOL_fat_getBPB(PFM_FATD    fatd,
                            PFM_FAT_BPB *p_bpb);
#endif /* PF_FATINFO_API_SUPPORT */

#if PF_FREE_CLUSTER_SEARCH_SUPPORT
/*---------------------------------------------------------------------------
  PFVOL_getFCSP - Get pointer of PFM_FCSP.
 ----------------------------------------------------------------------------*/
PFM_FCSP*  PFVOL_getFCSP(PF_VOLUME *p_vol);
#endif /* PF_FREE_CLUSTER_SEARCH_SUPPORT */

/*---------------------------------------------------------------------------
  PFCTX_SetLastErrorToCurrentCtx - Set last error of the context.
 ----------------------------------------------------------------------------*/
PF_ERROR  PFCTX_SetLastErrorToCurrentCtx(PF_ERROR err);

/*---------------------------------------------------------------------------
  PFVOL_GetContextFromId - Get context from context ID.
 ----------------------------------------------------------------------------*/
PF_CONTEXT*  PFCTX_GetContextFromID(PF_S_LONG ctx_id);

#if USE_EBINDER
/*---------------------------------------------------------------------------
  PFVOL_LoadVolumeLabelFromBuf - copy volume label from buffer to Drive Table.
 ----------------------------------------------------------------------------*/
void  PFVOL_LoadVolumeLabelFromBuf(const PF_U_CHAR* buf, PF_VOLUME* p_vol);
#endif /* USE_EBINDER */

#if PF_ACTIVE_DIR_LOCK_ENABLE
/*---------------------------------------------------------------------------
 PFVOL_IsDlocked
 ----------------------------------------------------------------------------*/
PF_BOOL  PFVOL_IsDlock(const PF_VOLUME *p_vol);
#endif /* PF_ACTIVE_DIR_LOCK_ENABLE*/

#if USE_VOLUME_ERROR
/*---------------------------------------------------------------------------
  PFVOL_derrnum - return the error number of the last driver error occured
                  in the specified volume.
 ----------------------------------------------------------------------------*/
PF_ERROR  PFVOL_derrnum(PF_DRV_CHAR drv_char);
#endif /* USE_VOLUME_ERROR */

/*---------------------------------------------------------------------------
  PFVOL_errnum - return the error number of the last error
                               occured in the current Volume Set.
 ----------------------------------------------------------------------------*/
PF_ERROR  PFVOL_errnum(void);

/*======================================================================*/
/* routine name : PFVOL_setvol(PF_DRV_CHAR, PF_STR*)                    */
/*======================================================================*/
PF_ERROR  PFVOL_setvol(PF_DRV_CHAR drv_char, PF_STR *p_vname_str);

/*======================================================================*/
/* routine name : PFVOL_getvol(PF_DRV_CHAR, PF_VOLTAB*)                 */
/*======================================================================*/
PF_ERROR  PFVOL_getvol(PF_DRV_CHAR drv_char, PF_VOLTAB *voltab, PF_U_LONG code_mode);

/*======================================================================*/
/* routine name : PFVOL_rmvvol(PF_DRV_CHAR)                             */
/*======================================================================*/
PF_ERROR  PFVOL_rmvvol(PF_DRV_CHAR drv_char);

/*======================================================================*/
/* routine name : PFVOL_getdev(PF_DRV_CHAR, PF_DEV_INF*)                */
/*======================================================================*/
PF_ERROR  PFVOL_getdev(PF_DRV_CHAR drv_char, PF_DEV_INF* dev_inf);

/*======================================================================*/
/* routine name : PFVOL_buffering(PF_DRV_CHAR, PF_U_CHAR)               */
/*======================================================================*/
PF_ERROR  PFVOL_buffering(PF_DRV_CHAR drv_char, PF_U_LONG mode);

/*---------------------------------------------------------------------------
  PFVOL_attach - Attach a drive.
 ----------------------------------------------------------------------------*/
PF_ERROR  PFVOL_attach(PF_DRV_TBL* p_drv);

/*---------------------------------------------------------------------------
  PFVOL_detach - Detach a drive.
 ----------------------------------------------------------------------------*/
PF_ERROR  PFVOL_detach(PF_DRV_CHAR drv_char);

/*---------------------------------------------------------------------------
  PFVOL_format - Format a volume specified by 'drv_char'.
 ----------------------------------------------------------------------------*/
PF_ERROR  PFVOL_format(PF_DRV_CHAR drv_char, const PF_U_CHAR* param);

#if !PF_BACKWARD_COMPATIBILITY_MOUNT
/*---------------------------------------------------------------------------
  PFVOL_mount - Mount a volume specified by 'drv_char'.
 ----------------------------------------------------------------------------*/
PF_ERROR  PFVOL_mount(PF_DRV_CHAR drv_char);
#endif /* !PF_BACKWARD_COMPATIBILITY_MOUNT */

/*---------------------------------------------------------------------------
  PFVOL_unmount - Unmount a volume specified by 'drv_char'.
 ----------------------------------------------------------------------------*/
PF_ERROR  PFVOL_unmount(PF_DRV_CHAR drv_char, PF_U_LONG mode, PF_ERROR *p_true_errnum);

/*---------------------------------------------------------------------------
  PFVOL_setupfsi - Setup FSINFO Relation a volume specified by 'drv_char'.
 ----------------------------------------------------------------------------*/
PF_ERROR  PFVOL_setupfsi(PF_DRV_CHAR drv_char, PF_U_SHORT flags);

#if PF_USE_CLSTLINK
/*---------------------------------------------------------------------------
  PFVOL_setclstlink -- Setup Cluster Link Recording
 ----------------------------------------------------------------------------*/
PF_ERROR  PFVOL_setclstlink(PF_DRV_CHAR drv_char, PF_U_LONG mode, PF_CLUSTER *p_inf);
#endif /* PF_USE_CLSTLINK*/

/*---------------------------------------------------------------------------
  PFVOL_sync -- Synchronize from Cache to Media
 ----------------------------------------------------------------------------*/
PF_ERROR  PFVOL_sync(PF_DRV_CHAR drv_char, PF_U_LONG mode);

/*---------------------------------------------------------------------------
  PFVOL_settailbuf -- Setup buffer to create a file with tail.
 ----------------------------------------------------------------------------*/
PF_ERROR  PFVOL_settailbuf(PF_DRV_CHAR drv_char, PF_U_LONG size, PF_TAIL_BUF* buf);

/*---------------------------------------------------------------------------
  PFVOL_setvolcfg -- Setup configurations to a volume.
 ----------------------------------------------------------------------------*/
PF_ERROR  PFVOL_setvolcfg(PF_DRV_CHAR drv_char, PF_VOL_CFG* config);

/*---------------------------------------------------------------------------
  PFVOL_getvolcfg -- Get configurations on a volume.
 ----------------------------------------------------------------------------*/
PF_ERROR  PFVOL_getvolcfg(PF_DRV_CHAR drv_char, PF_VOL_CFG* config);

/*---------------------------------------------------------------------------
 PFVOL_setcode - Set character-set.
 ----------------------------------------------------------------------------*/
PF_ERROR  PFVOL_setcode(PF_CHARCODE* p_codeset);

#if (0 < PF_MAX_CONTEXT_NUM)
/*---------------------------------------------------------------------------
 PFCTX_regctx - Register current context ID.
 ----------------------------------------------------------------------------*/
#if PF_AUTO_REGCTX_SUPPORT
PF_CONTEXT*  PFCTX_regctx(PF_S_LONG *p_tskid);
#else /* !PF_AUTO_REGCTX_SUPPORT */
PF_CONTEXT*  PFCTX_regctx(PF_S_LONG *p_tskid);
#endif /* PF_AUTO_REGCTX_SUPPORT */
#endif /* (0 < PF_MAX_CONTEXT_NUM) */

#if (0 < PF_MAX_CONTEXT_NUM)
#if ! PF_AUTO_REGCTX_SUPPORT
/*---------------------------------------------------------------------------
 PFCTX_unregctx - Unregister current context ID.
 ----------------------------------------------------------------------------*/
PF_ERROR  PFCTX_unregctx(void);
#endif /* ! PF_AUTO_REGCTX_SUPPORT */
#endif /* (0 < PF_MAX_CONTEXT_NUM) */

/*---------------------------------------------------------------------------
 PFVOL_setencode - Set encoding mode.
 ----------------------------------------------------------------------------*/
PF_ERROR  PFVOL_setencode(PF_U_LONG encode_mode);

#if PF_MODULE_SUPPORT
/*---------------------------------------------------------------------------
 PFVOL_reg_module - Register/release a module I/F
 ----------------------------------------------------------------------------*/
PF_ERROR  PFVOL_reg_module(PF_DRV_CHAR  drive,
                           PF_MODULE_ID modid,
                           void         *p_module,
                           void         *p_param);

/*---------------------------------------------------------------------------
 PFVOL_ctrl_module - ioctl for the module
 ----------------------------------------------------------------------------*/
PF_ERROR  PFVOL_ctrl_module(PF_DRV_CHAR  drive,
                            PF_MODULE_ID modid,
                            PF_U_LONG    req_code,
                            void         *p_param);

/*---------------------------------------------------------------------------
 PFVOL_ctrl_module_nb -
 ----------------------------------------------------------------------------*/
PF_ERROR  PFVOL_ctrl_module_nb(PF_DRV_CHAR  drive,
                               PF_MODULE_ID modid,
                               PF_U_LONG    req_code,
                               void         *p_param);

#if USE_VOLUME_ERROR
/*---------------------------------------------------------------------------
 PFVOL_module_errnum - The last error of the module
 ----------------------------------------------------------------------------*/
PF_ERROR  PFVOL_module_errnum(PF_DRV_CHAR  drive,
                              PF_MODULE_ID modid);
#endif /* USE_VOLUME_ERROR */
#endif /* PF_MODULE_SUPPORT */

#endif  /* __PF_VOLUME_H__ */
