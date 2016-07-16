/*
     Copyright (C) 2009 By eSOL Co.,Ltd. Tokyo, Japan

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
 [pf_apicmn_def.h] - Definitions and Declarations commonly used in PrFILE2 API.

 NOTES:
  - Except headers which are included by 'pf_apicmn.h' itself, any of
    source (*.c) and header (*.h) files of PrFILE2 shall include this
    header 'pf_apicmn.h' (or 'pf_common.h' which includes the header
    internally) prior to any other header.
 ****************************************************************************/
#ifndef __PF_APICMN_DEF_H__
#define __PF_APICMN_DEF_H__

#ifndef _AMBA_KAL_
#define _AMBA_KAL_
#endif

#include "prfile2/pf_config.h"
#include "prfile2/pf_types.h"
#if defined(_AMBA_KAL_)
#include "hardcfg.h"
#endif

/****************************************************************************
  Internal Definitions and Declarations

  NOTES:
   - These definitions and declarations are only for internal use in PrFILE2;
     User don't have to use them in user applications.
 ****************************************************************************/
/*---------------------------------------------------------------------------
  File Lock Mode
 ----------------------------------------------------------------------------*/
#define PF_LOCK_SH                  (0x01ul)  /* sharing lock       */
#define PF_LOCK_EX                  (0x02ul)  /* exclusion lock     */
#define PF_LOCK_UN                  (0x04ul)  /* unlock             */
#define PF_LOCK_NB                  (0x08ul)  /* non-blocking mode  */


/*---------------------------------------------------------------------------
  Directory Lock Mode
 ----------------------------------------------------------------------------*/
#define PF_DLOCK_EX                 (0x02ul)  /* exclusion lock     */
#define PF_DLOCK_UN                 (0x04ul)  /* unlock             */
#define PF_DLOCK_NB                 (0x08ul)  /* non-blocking mode  */

#define PF_DLOCK_MASK               (PF_DLOCK_EX | PF_DLOCK_UN | PF_DLOCK_NB)


/*---------------------------------------------------------------------------
  Cluster link information
 ----------------------------------------------------------------------------*/
#define PF_CLST_LINK_ENABLE         (1)
#define PF_CLST_LINK_DISABLE        (0)


/*---------------------------------------------------------------------------
  Maximum length of primary part of filename
 ----------------------------------------------------------------------------*/
#define SHORT_NAME_BASE_SIZE        (8)


/*---------------------------------------------------------------------------
  Maximum length of extention of filename
 ----------------------------------------------------------------------------*/
#define SHORT_NAME_EXT_SIZE         (3)


/*---------------------------------------------------------------------------
  Maximum length of Short/Long File Name in characters
 ----------------------------------------------------------------------------*/
#define SHORT_NAME_CHARS            (11)    /* = 8 + 3      */
#define SHORT_DOT_NAME_CHARS        (12)    /* = 8 + 1 + 3  */
#define LONG_NAME_CHARS             (255)


/*---------------------------------------------------------------------------
  Maximum size of Short/Long File Name in bytes
 ----------------------------------------------------------------------------*/
#define SHORT_NAME_SIZE             (11)    /* = SHORT_NAME_CHARS       */
#define SHORT_DOT_NAME_SIZE         (12)    /* = SHORT_DOT_NAME_CHARS   */
#define LONG_NAME_SIZE              (510)   /* = LONG_NAME_CHARS * 2    */

#define VOLUME_LABEL_SIZE_FAT       (11)    /* 11[byte]                 */

#if PF_EXFAT_SUPPORT
#define VOLUME_LABEL_SIZE_EXFAT     (11)    /* 11[Characters] (UNICODE) */
#endif


/*---------------------------------------------------------------------------
  Buffer size for Short/Long File Name in bytes
 ----------------------------------------------------------------------------*/
#define SHORT_NAME_BUF_SIZE         (SHORT_DOT_NAME_SIZE+1) /* Including NUL    */
#define LONG_NAME_BUF_SIZE          (LONG_NAME_SIZE+2)      /* Including NUL    */

#define NAME_BUF_SIZE               LONG_NAME_BUF_SIZE

#if PF_EXFAT_SUPPORT
#define VOLUME_LABEL_CHARS          (VOLUME_LABEL_SIZE_EXFAT)   /* Without NUL */
#define VOLUME_LABEL_BUF_SIZE       (VOLUME_LABEL_CHARS*2)      /* Without NUL */
#else
#define VOLUME_LABEL_CHARS          (VOLUME_LABEL_SIZE_FAT)     /* Without NUL */
#define VOLUME_LABEL_BUF_SIZE       (VOLUME_LABEL_CHARS)        /* Without NUL */
#endif

/*---------------------------------------------------------------------------
  Maximum path length of Short/Long File Name in characters
 ----------------------------------------------------------------------------*/
#define SHORT_NAME_PATH_CHARS       (80-1)  /* Without NUL              */
#define LONG_NAME_PATH_CHARS        (260-1) /* Without NUL              */


/*---------------------------------------------------------------------------
  Maximum path size of Short/Long File Name in bytes
 ----------------------------------------------------------------------------*/
#define SHORT_NAME_PATH_SIZE        SHORT_NAME_PATH_CHARS       /* Without NUL */
#define LONG_NAME_PATH_SIZE         (LONG_NAME_PATH_CHARS*2)    /* Without NUL */


/*---------------------------------------------------------------------------
  Buffer size for path of Short/Long File Name in bytes
 ----------------------------------------------------------------------------*/
#define SHORT_NAME_PATH_BUF_SIZE    (SHORT_NAME_PATH_SIZE+1)    /* Including NUL    */
#define LONG_NAME_PATH_BUF_SIZE     (LONG_NAME_PATH_SIZE+2)     /* Including NUL    */

#define PATH_BUF_SIZE               LONG_NAME_PATH_BUF_SIZE


/*---------------------------------------------------------------------------
  Volume information
 ----------------------------------------------------------------------------*/
typedef struct PF_VOLUME            PF_VOLUME;


/*---------------------------------------------------------------------------
  PF_FUNCTBL - Driver Interface Function Table

  NOTES:
   - This is equivalent of 'FUNCTBL' for PrFILE1.
 ----------------------------------------------------------------------------*/
typedef struct PF_FUNCTBL {
    /* Initialize drive for attach */
    PF_ERROR    (*init)(PF_VOLUME*);

    /* Finalize drive for detach */
    PF_ERROR    (*finalize)(PF_VOLUME*);

    /* Mount driver to access */
    PF_ERROR    (*mount)(PF_VOLUME*);

    /* Un-mount driver */
    PF_ERROR    (*unmount)(PF_VOLUME*);

    /* Format the volume */
    PF_ERROR    (*format)(PF_VOLUME*, const PF_U_CHAR*);

    /* Read logical sectors */
    PF_ERROR    (*lread)(PF_VOLUME* p_vol, PF_U_CHAR* buf, PF_U_LONG sector, PF_U_LONG num, PF_U_LONG* success_num);

    /* Write logical sectors*/
    PF_ERROR    (*lwrite)(PF_VOLUME* p_vol, const PF_U_CHAR* buf, PF_U_LONG sector, PF_U_LONG num, PF_U_LONG* success_num);

} PF_FUNCTBL;


#if PF_BACKWARD_COMPATIBILITY_STRUCTURE

typedef PF_FUNCTBL                  FUNCTBL;        /* OBSOLETE         */

#endif /* PF_BACKWARD_COMPATIBILITY_STRUCTURE */


/****************************************************************************
  Public Type Definitions

  NOTES:
   - These types are used with API functions in user applications.
 ****************************************************************************/
/*---------------------------------------------------------------------------
  File information for pf_create(), pf_fopen(), pf_fclose(), ...

  NOTES:
   - This type is intentionally declared as 'Incomplete Type';
     Users should use this type only in the form of 'PF_FILE *'.
 ----------------------------------------------------------------------------*/
typedef struct PF_FILE              PF_FILE;

#if PF_STDLIB_ALIAS_SUPPORT
  #define FILE                      PF_FILE
#endif /* PF_STDLIB_ALIAS_SUPPORT */


/*---------------------------------------------------------------------------
  Directory information for pf_opendir(), pf_closedir(), pf_fchdir(), ...

  NOTES:
   - This type is intentionally declared as 'Incomplete Type';
     Users should use this type only in the form of 'PF_DIR *'.
 ----------------------------------------------------------------------------*/
typedef struct PF_DIR               PF_DIR;


/*---------------------------------------------------------------------------
  Tail Tracker Buffer for pf_settailbuf().
 ----------------------------------------------------------------------------*/
typedef PF_U_LONG  PF_TAIL_BUF;


#if PF_USE_CLSTLINK
/*---------------------------------------------------------------------------
  Cluster Link Buffer for pf_setclstlink().

  NOTES:
   - This type is setting for volume structure.
 ----------------------------------------------------------------------------*/
typedef PF_U_LONG  PF_CLSTSET_BUF[PF_MAX_FILE_NUM];


/*---------------------------------------------------------------------------
  Cluster Link Buffer for pf_fsetclstlink().

  NOTES:
   - This type is setting for FAT file discripter(within PF_CLUSTER_LINK).
 ----------------------------------------------------------------------------*/
typedef PF_U_LONG  PF_CLUSTER_BUF;


/*---------------------------------------------------------------------------
  Cluster Link Information for pf_setclstlink().
 ----------------------------------------------------------------------------*/
typedef struct PF_CLUSTER {
    /* Cluster Link Buffer */
    PF_CLSTSET_BUF*   buffer;

    /* Cluster Link Max */
    PF_U_LONG         max_count;

    /* Cluster Link Interval */
    PF_U_SHORT        interval;

} PF_CLUSTER;


/*---------------------------------------------------------------------------
  Cluster Link Information for pf_fsetclstlink().
 ----------------------------------------------------------------------------*/
typedef struct PF_CLUSTER_FILE {
    /* Cluster Link Buffer */
    PF_CLUSTER_BUF*   buffer;

    /* Cluster Link Max */
    PF_U_LONG         max_count;

    /* Cluster Link Interval */
    PF_U_SHORT        interval;

} PF_CLUSTER_FILE;
#endif /* PF_USE_CLSTLINK */


/*---------------------------------------------------------------------------
  FAT search
 ----------------------------------------------------------------------------*/
struct PF_CLST_IDX_TAG
{
    /* The index of 'ci_clst' in the cluster chain (0 origin) */
    PF_CLST     ci_idx;

    /* The cluster number */
    PF_CLST     ci_clst;
};
typedef struct PF_CLST_IDX_TAG  PF_CLST_IDX;


/*---------------------------------------------------------------------------
  Last Access Cluster Number and Index
  * Holds information about the position of the last access cluster number
  * and index of the cluster in chain.
  *
  * Each PF_FILE (=UFD: user file descriptor) holds its own pointer to a
  * structure of this type describing the last access to the file made
  * through it. The goal is to avoid going through the file's whole FAT
  * chain each time an access is made to it's contents.
  *
  * To be used in the FAT modules' implementation only.
 ----------------------------------------------------------------------------*/
typedef struct PF_CLST_IDX_TAG  PF_FAT_HINT;


/*---------------------------------------------------------------------------
  Information for both pf_fsfirst() and pf_fsnext().
 ----------------------------------------------------------------------------*/
typedef struct PF_DTA {
    /*-----------------------------------------------------------------------*
      User fields: this is for user area, fsexec API sets file/dir pointer to this area/
     *-----------------------------------------------------------------------*/
    /* File descriptor */
    PF_FILE*    p_file;

    /* Directory descriptor */
    PF_DIR*     p_dir;

    /*-----------------------------------------------------------------------*
      Internal fields: do not use these as they will change without notice
     *-----------------------------------------------------------------------*/
    /* Volume of the parent directory   */
    PF_VOLUME*  p_vol;

    /* Parent directory start cluster */
    PF_U_LONG   parent_start_cluster;

    /* Next entry position within parent */
    PF_U_LONG   parent_pos; /* started from 1 */

    /* Last access cluster information */
    PF_FAT_HINT hint;

    /* Status of PF_DTA */
    PF_U_LONG   status;

    /* Located path length */
#if PF_CHECK_PATH_LENGTH
    PF_U_LONG   path_len;
#endif /* PF_CHECK_PATH_LENGTH */

    /* Number of LFNs; If it is zero, string in 'long_name' field is not valid */
    PF_U_CHAR   num_entry_LFNs;

    /* Ordinal field of LFN entry, without LAST_LONG_ENTRY flag */
    PF_U_CHAR   ordinal;

    /* FAT  : Checksum field of LFN entry
     * exFAT: Hash field of File name     */
    PF_U_SHORT   check_sum;

#if PF_EXFAT_SUPPORT
    /* Parent directory GeneralSecondaryFlags */
    PF_U_CHAR parent_general_flg;

    /* Parent directory size in bytes */
    PF_FSIZE_T parent_dir_size;

    /* Parent Path */
    PF_WCHAR parent_path[LONG_NAME_PATH_CHARS + 1];

    /* Number of Secondary Entry */
    PF_U_CHAR   num_secondary;

    /* GeneralSecondaryFlags */
    PF_U_CHAR   general_flg;
#endif

    /* Attributes searched for */
    PF_U_CHAR   attr;

    /* Regular expression for file name */
    PF_CHAR     reg_exp[PATH_BUF_SIZE];


    /*-----------------------------------------------------------------------*
      Public fields:
     *-----------------------------------------------------------------------*/
#if PF_EXFAT_SUPPORT
    /* Modification milliseconds (actually contains a count of tenths of a second) */
    PF_U_SHORT  TimeMs;
#endif

    /* Modification time */
    PF_U_SHORT  Time;

    /* Modification date */
    PF_U_SHORT  Date;

#if PF_EXFAT_SUPPORT
    /* Offset from UTC of Modification time */
    PF_U_CHAR   UtcOffset;
#endif

#if PF_EXFAT_SUPPORT
    /* Access time */
    PF_U_SHORT  ATime;
#endif

    /* Access date */
    PF_U_SHORT  ADate;

#if PF_EXFAT_SUPPORT
    /* Offset from UTC of Access time */
    PF_U_CHAR   AUtcOffset;
#endif

    /* Create milliseconds (actually contains a count of tenths of a second) */
    PF_U_SHORT  CTimeMs;

    /* Create time */
    PF_U_SHORT  CTime;

    /* Create date */
    PF_U_SHORT  CDate;

#if PF_EXFAT_SUPPORT
    /* Offset from UTC of Create time */
    PF_U_CHAR   CUtcOffset;
#endif

    /* File size in bytes */
    PF_FSIZE_T  FileSize;

    /* File attributes */
    PF_U_SHORT  Attribute;

    /* Short file name (8.3) */
    PF_CHAR     FileName[SHORT_NAME_BUF_SIZE];

    /* Long file name */
    PF_CHAR     LongName[LONG_NAME_BUF_SIZE];

#if defined(__PRKERNEL_AMB__) || defined(_AMBA_KAL_)
    PF_U_LONG   StartClst;
#endif

} PF_DTA;

#if PF_BACKWARD_COMPATIBILITY_STRUCTURE

typedef PF_DTA                      DTA;            /* OBSOLETE */

#endif /* PF_BACKWARD_COMPATIBILITY_STRUCTURE */


/*---------------------------------------------------------------------------
  Information for pf_fsexec().
 ----------------------------------------------------------------------------*/
typedef struct PF_DTA_EXEC {
    /*-----------------------------------------------------------------------*
      User fields: this is for user area, fsexec API sets file/dir pointer to this area/
     *-----------------------------------------------------------------------*/
    /* File descriptor */
    PF_FILE*    p_file;

    /* Directory descriptor */
    PF_DIR*     p_dir;

    /*-----------------------------------------------------------------------*
      Internal fields: do not use these as they will change without notice
     *-----------------------------------------------------------------------*/
    /* Volume of the parent directory   */
    PF_VOLUME*  p_vol;

    /* Parent directory start cluster */
    PF_U_LONG   parent_start_cluster;

    /* Next entry position within parent */
    PF_U_LONG   parent_pos; /* started from 1 */

    /* Last access cluster information */
    PF_FAT_HINT hint;

    /* Status of PF_DTA */
    PF_U_LONG   status;

    /* Located path length */
#if PF_CHECK_PATH_LENGTH
    PF_U_LONG   path_len;
#endif /* PF_CHECK_PATH_LENGTH */

    /* Number of LFNs; If it is zero, string in 'long_name' field is not valid */
    PF_U_CHAR   num_entry_LFNs;

    /* Ordinal field of LFN entry, without LAST_LONG_ENTRY flag */
    PF_U_CHAR   ordinal;

    /* FAT  : Checksum field of LFN entry
     * exFAT: Hash field of File name     */
    PF_U_SHORT   check_sum;

#if PF_EXFAT_SUPPORT
    /* Parent directory GeneralSecondaryFlags */
    PF_U_CHAR parent_general_flg;

    /* Parent directory size in bytes */
    PF_FSIZE_T parent_dir_size;

    /* Parent Path */
    PF_WCHAR parent_path[LONG_NAME_PATH_CHARS + 1];

    /* Number of Secondary Entry */
    PF_U_CHAR   num_secondary;
#endif /* PF_EXFAT_SUPPORT */

} PF_DTA_EXEC;

/*---------------------------------------------------------------------------
  File information table for pf_fstat().
 ----------------------------------------------------------------------------*/
typedef struct PF_STAT {
    /* file size in bytes */
    PF_FSIZE_T  fstfz;

#if PF_EXFAT_SUPPORT
    /* Last access time */
    PF_U_SHORT  fstact;
#endif

    /* Last access date */
    PF_U_SHORT  fstad;

#if PF_EXFAT_SUPPORT
    /* Offset from UTC to the Last access date and time */
    PF_U_CHAR   fstautc;
#endif

    /* Last modify time */
    PF_U_SHORT  fstut;

#if PF_EXFAT_SUPPORT
    /* Component of the file Last modify time */
    PF_U_SHORT  fstuc;
#endif

    /* Last modify date */
    PF_U_SHORT  fstud;

#if PF_EXFAT_SUPPORT
    /* Offset from UTC to the Last modify date and time */
    PF_U_CHAR   fstuutc;
#endif

    /* Create time      */
    PF_U_SHORT  fstct;

    /* Create date      */
    PF_U_SHORT  fstcd;

    /* Component of the file creation time */
    PF_U_SHORT  fstcc;

#if PF_EXFAT_SUPPORT
    /* Offset from UTC to the Create date and time */
    PF_U_CHAR   fstcutc;
#endif

    /* file attributes */
    PF_U_CHAR   fstat;
} PF_STAT;

#if PF_STDLIB_ALIAS_SUPPORT
  #define _stat                     PF_STAT
#endif /* PF_STDLIB_ALIAS_SUPPORT */


/*---------------------------------------------------------------------------
  Drive information table for pf_getdev().
 ----------------------------------------------------------------------------*/
enum PF_FAT_TYPE_TAG
{
    PF_FMT_FAT12  = 0,
    PF_FMT_FAT16  = 1,
    PF_FMT_FAT32  = 2,
    PF_FMT_EXFAT  = 3,
    PF_FMT_ERR    = -1
};
typedef enum PF_FAT_TYPE_TAG  PF_FAT_TYPE;

typedef struct PF_DEV_INF {
    /* total number of logical clusters */
    PF_U_LONG   cls;

    /* number of empty clusters */
    PF_U_LONG   ecl;

    /* bytes per logical sector */
    PF_U_LONG   bps;

    /* logical sectors per cluster */
    PF_U_LONG   spc;

    /* clusters per cluster group */
    PF_U_LONG   cpg;

    /* number of empty cluster groups */
    PF_U_LONG   ecg;

    /* format type */
    PF_FAT_TYPE fmt;

} PF_DEV_INF;


#if PF_BACKWARD_COMPATIBILITY_STRUCTURE

typedef PF_DEV_INF                  DEV_INF;            /* OBSOLETE */

#endif /* PF_BACKWARD_COMPATIBILITY_STRUCTURE */


/*---------------------------------------------------------------------------
  Volume label table for pf_getvol().
 ----------------------------------------------------------------------------*/
typedef struct PF_VOLTAB
{
    /* Attributes of the volume label */
    PF_U_CHAR   v_attr;

    /* Date */
    PF_U_LONG   v_date;

    /* Time */
    PF_U_LONG   v_time;

    /* Volume label string (null-terminated) */
    PF_CHAR     v_name[VOLUME_LABEL_BUF_SIZE+1];
} PF_VOLTAB;


#if PF_BACKWARD_COMPATIBILITY_STRUCTURE

typedef PF_VOLTAB                   VOLTAB;             /* OBSOLETE */

#endif /* PF_BACKWARD_COMPATIBILITY_STRUCTURE */


/*---------------------------------------------------------------------------
    Cache Setting Table for pf_attach()

 ----------------------------------------------------------------------------*/
/* Cache buffer block size */
#define SECTOR_BUF_SIZE                 (512)
#define LOG2_SECTOR_BUF_SIZE            (9)


/*---------------------------------------------------------------------------
  PF_CACHE_PAGE
 ----------------------------------------------------------------------------*/
typedef struct PF_CACHE_PAGE
{
    /* Cache page status */
    PF_U_SHORT      stat;

    PF_U_SHORT      option;         /* User area (dose not use cache module) */
    PF_U_CHAR*      buffer;         /* the head address in buffer            */
    PF_U_CHAR*      p_buf;          /* the current pointer in buffer         */
    PF_U_CHAR*      p_mod_sbuf;     /* the start modified pointer in buffer  */
    PF_U_CHAR*      p_mod_ebuf;     /* the end modified pointer in buffer    */
    PF_U_LONG       size;           /* number of sector per buffer           */
    PF_U_LONG       sector;         /* the head sector number                */
    void*           signature;
    struct PF_CACHE_PAGE*  p_next;
    struct PF_CACHE_PAGE*  p_prev;
} PF_CACHE_PAGE;


/*---------------------------------------------------------------------------
  PF_SECTOR_BUF
 ----------------------------------------------------------------------------*/
typedef PF_U_CHAR  PF_SECTOR_BUF[SECTOR_BUF_SIZE];
typedef struct PF_CACHE_SETTING
{
    PF_CACHE_PAGE  *pages;            /* Cache page                 */
    PF_SECTOR_BUF  *buffers;          /* Cache buffer               */
    PF_U_SHORT     num_fat_pages;
    PF_U_SHORT     num_data_pages;
    PF_U_LONG      num_fat_buf_size;
    PF_U_LONG      num_data_buf_size;
} PF_CACHE_SETTING;


/*---------------------------------------------------------------------------
  Drive Table for pf_attach() (and pf_mount())

    NOTES: This is equivalent of 'DRV_TBL' for PrFILE1.
 ----------------------------------------------------------------------------*/
typedef struct PF_DRV_TBL {
    /* Pointer to a partition */
    void*           p_part;

    /* Cache Settings */
    PF_CACHE_SETTING* cache;

    /* Attached driver number */
    PF_DRV_CHAR     drive;

    /* Drive status */
    PF_U_CHAR       stat;

} PF_DRV_TBL;

#if PF_BACKWARD_COMPATIBILITY_STRUCTURE

typedef PF_DRV_TBL                  DRV_TBL;        /* OBSOLETE             */

#endif /* PF_BACKWARD_COMPATIBILITY_STRUCTURE */

/* The flag of 'stat' of PF_DRV_TBL */
#define PF_ATTACHED             (0x01ul)
#define PF_MOUNTED              (0x02ul)
#define PF_DISK_INSERTED        (0x04ul)


/*---------------------------------------------------------------------------
  File information table for pf_finfo().
 ----------------------------------------------------------------------------*/
typedef struct PF_INFO {

    /* file size in bytes */
    PF_FSIZE_T  file_size;

    /* file I/O pointer in bytes */
    PF_FPOS_T   io_pointer;

    /* file size in bytes */
    PF_FSIZE_T  empty_size;

    /* file size in bytes */
    PF_FSIZE_T  allocated_size;

    /* file lock mode */
    PF_U_LONG   lock_mode;

    /* file lock owner */
    PF_FILE*    lock_owner;

    /* lock count by this descriptor */
    PF_U_LONG   lock_count;

    /* total lock count by all descriptor */
    PF_U_LONG   lock_tcount;

    /* file open mode */
    PF_U_LONG   open_mode;

} PF_INFO;


/*---------------------------------------------------------------------------
 Character set of volumes
 ----------------------------------------------------------------------------*/
typedef struct PF_CHARCODE
{
    /* function to transfer OEM code to Unicode */
    PF_S_LONG   (*oem2unicode)(const PF_CHAR*, PF_WCHAR*);

    /* function to transfer Unicode to OEM code */
    PF_S_LONG   (*unicode2oem)(const PF_WCHAR*, PF_CHAR*);

    /* function to get the width of OEM code */
    PF_S_LONG   (*oem_char_width)(const PF_CHAR*);

    /* function to check 1st multiple-byte character */
    PF_BOOL     (*is_oem_mb_char)(const PF_CHAR, PF_U_LONG);

    /* function to get the width of Unicode */
    PF_S_LONG   (*unicode_char_width)(const PF_WCHAR*);

    /* function to check 1st multiple-byte character */
    PF_BOOL     (*is_unicode_mb_char)(const PF_WCHAR, PF_U_LONG);

} PF_CHARCODE;


/*---------------------------------------------------------------------------
  Directory information for pf_readdir()
 ----------------------------------------------------------------------------*/
typedef struct PF_DIRENT {

    /* long file name field */
    PF_CHAR         long_dname[LONG_NAME_BUF_SIZE];

    /* short file name field */
    PF_CHAR         short_dname[SHORT_NAME_BUF_SIZE];

    /* Attributes */
    PF_U_LONG       attr;

} PF_DIRENT;


/*---------------------------------------------------------------------------
  System date and time
 ----------------------------------------------------------------------------*/
typedef struct PF_SYS_DATE
{
    PF_U_SHORT  sys_year;               /* year                */
    PF_U_SHORT  sys_month;              /* month (1-12)        */
    PF_U_SHORT  sys_day;                /* day (1-31)          */

} PF_SYS_DATE;

typedef struct PF_SYS_TIME
{
    PF_U_SHORT  sys_hour;               /* hour (0-23)         */
    PF_U_SHORT  sys_min;                /* minites (0-59)      */
    PF_U_SHORT  sys_sec;                /* second (0-59)       */
#if !PF_BACKWARD_COMPATIBILITY_SYSTIME
    PF_U_SHORT  sys_ms;                 /* milli second(0-199) */
#if PF_EXFAT_SUPPORT
    /* UTC Offset for exFAT */
    PF_U_SHORT  sys_utc_valid;          /* UTC Offset Valid    */
    PF_S_SHORT  sys_utc;                /* UTC Offset [minites]*/
#endif /* PF_EXFAT_SUPPORT */
#endif /* !PF_BACKWARD_COMPATIBILITY_SYSTIME */

} PF_SYS_TIME;

#define PF_SETSTMP_NONE         (0xFFFFuL)    /* No setting */

#define PF_SETSTMP_UTC_INVALID  (0x0000uL)
#define PF_SETSTMP_UTC_VALID    (0x0080uL)

#if PF_BACKWARD_COMPATIBILITY_STRUCTURE

typedef PF_SYS_DATE             SYS_DATE;   /* OBSOLETE */
typedef PF_SYS_TIME             SYS_TIME;   /* OBSOLETE */

#endif /* PF_BACKWARD_COMPATIBILITY_STRUCTURE */


/*---------------------------------------------------------------------------
  Tiemstamp information
 ----------------------------------------------------------------------------*/
typedef struct PF_TIMESTMP
{
    /* create date */
    PF_SYS_DATE     cre_date;

    /* create time */
    PF_SYS_TIME     cre_time;

    /* modified date */
    PF_SYS_DATE     mod_date;

    /* modified time */
    PF_SYS_TIME     mod_time;

    /* last access date */
    PF_SYS_DATE     acc_date;

#if PF_EXFAT_SUPPORT
    /* last access time */
    PF_SYS_TIME     acc_time;
#endif

} PF_TIMESTMP;


/*---------------------------------------------------------------------------
  Divide information for pf_fdivide()
 ----------------------------------------------------------------------------*/
struct PF_FDIVIDE_TAG
{
    /* Pointer to file path for file to divide */
    PF_CHAR         *fbase;

    /* Cluster offset from the beginning of the file to the division location */
    PF_U_LONG       idivide;

    /* Pointer to file path for file of the first half created after division */
    PF_CHAR         *f1half;

    /* Pointer to file path for file of the latter half created after division */
    PF_CHAR         *f2half;

    /* Size of file of the first half */
    PF_FSIZE_T      f1size;
};
typedef struct PF_FDIVIDE_TAG  PF_FDIVIDE;


/*---------------------------------------------------------------------------
  Seek mode for pf_fseek()
 ----------------------------------------------------------------------------*/
#define PF_SEEK_CUR             (1)             /* current              */
#define PF_SEEK_SET             (0)             /* head                 */
#define PF_SEEK_END             (2)             /* end                  */

#if PF_STDLIB_ALIAS_SUPPORT
  #define SEEK_CUR              PF_SEEK_CUR     /* current              */
  #define SEEK_SET              PF_SEEK_SET     /* head                 */
  #define SEEK_END              PF_SEEK_END     /* end                  */
#endif /* PF_STDLIB_ALIAS_SUPPORT */


/*---------------------------------------------------------------------------
  Buffering mode for pf_buffering()
 ----------------------------------------------------------------------------*/
#define PF_WRTSOON              (0x01ul)        /* write soon                */
#define PF_NWRTSOON             (0x00ul)        /* not write soon            */
#define PF_AUTOSYNC             (0x02ul)        /* cache is not used effectively */
#define PF_NAUTOSYNC            (0x00ul)        /* cache is used effectively */
#define PF_FAT_WRITE_THRU       (0x04ul)
#define PF_FAT_WRITE_BACK       (0x00ul)

#define PF_EJECT_SAFE_WITH_WRITE_THRU   PF_WRTSOON
#define PF_EJECT_SAFE_WITH_WRITE_BACK   (PF_NWRTSOON | PF_AUTOSYNC | PF_FAT_WRITE_THRU)
#define PF_WRITE_BACK_ON_SIGNIF_API     (PF_NWRTSOON | PF_AUTOSYNC)
#define PF_WRITE_BACK_CACHE             (PF_NWRTSOON | PF_NAUTOSYNC)

#if PF_BACKWARD_COMPATIBILITY_PARAMETER
#define NWRTSOON                PF_NWRTSOON     /* not write soon            */
#define WRTSOON                 PF_WRTSOON      /* write soon                */
#endif /* PF_BACKWARD_COMPATIBILITY_PARAMETER */


/*---------------------------------------------------------------------------
  Cache Invalidation mode for pf_sync()
 ----------------------------------------------------------------------------*/
#define PF_INVALIDATE           (0x1)
#define PF_NINVALIDATE          (0x0)


/*---------------------------------------------------------------------------
  FSINFO Setup Flags
 ----------------------------------------------------------------------------*/
#define PF_FSI_TRUST_ENABLE     (0x0001ul)      /* Trust FSINFO Data      */
#define PF_FSI_TRUST_DISABLE    (0x0002ul)      /* No Trust FSINFO Data   */
#define PF_FSI_REFRESH_ENABLE   (0x0004ul)      /* Refresh FSINFO Data    */
#define PF_FSI_REFRESH_DISABLE  (0x0008ul)      /* No Refresh FSINFO Data */


/*---------------------------------------------------------------------------
  Unmount mode for pf_unmount()
 ----------------------------------------------------------------------------*/
#define PF_UNMOUNT_NORMAL       (0x0)           /* normal unmount mode    */
#define PF_UNMOUNT_FORCE        (0x1)           /* force unmount mode     */


/*---------------------------------------------------------------------------
  DTA status
 ----------------------------------------------------------------------------*/
#define PF_DTA_STAT_LOCAL       (0x00000001ul)
#define PF_DTA_STAT_UNICODE     (0x00000002ul)


/*---------------------------------------------------------------------------
  DTA path_len
 ----------------------------------------------------------------------------*/
#define PF_DTA_PATHLEN_CDIR_SHIFT   (16)
#define PF_DTA_PATHLEN_CDIR_MASK    (0xFFFF0000ul)
#define PF_DTA_PATHLEN_FNAME_SHIFT  (0)
#define PF_DTA_PATHLEN_FNAME_MASK   (0x0000FFFFul)


/*---------------------------------------------------------------------------
  File attributes
 ----------------------------------------------------------------------------*/
#define ATTR_RDONLY             (0x0001uL)          /* read only    */
#define ATTR_HIDDEN             (0x0002uL)          /* hidden       */
#define ATTR_SYSTEM             (0x0004uL)          /* system       */
#define ATTR_VOLUME             (0x0008uL)          /* volume label */
#define ATTR_DIR                (0x0010uL)          /* subdirectory */
#define ATTR_ARCH               (0x0020uL)          /* archives     */
#define ATTR_NONE               (0x0040uL)          /* none         */
#define ATTR_ALL                (0x007FuL)          /* for fsfirst function */
#define ATTR_NOT_DIR            (0x0100uL)          /* except for directory and volume lavel */

#define ATTR_CAND               (0x0080uL)          /* AND compare mode     */
#define ATTR_CMATCH             (0x1000uL)          /* MATCH compare mode   */

#define ATTR_ADD                (0x2000uL)          /* Attribute add mode   */
#define ATTR_SUB                (0x4000uL)          /* Attribute del mode   */

#if PF_DIRCACHE_SUPPORT
#undef ATTR_ALL
#define ATTR_ALL                (0x0077uL)          /* = (ATTR_ALL - ATTR_VOLUME) */
                                                    /* for fsfirst function       */
#endif /* PF_DIRCACHE_SUPPORT */

#if PF_BACKWARD_COMPATIBILITY_PARAMETER
#define EATRO                   ATTR_RDONLY         /* read only    */
#define EATHD                   ATTR_HIDDEN         /* hidden       */
#define EATSY                   ATTR_SYSTEM         /* system       */
#define EATVL                   ATTR_VOLUME         /* volume label */
#define EATDR                   ATTR_DIR            /* subdirectory */
#define EATAR                   ATTR_ARCH           /* archives     */
#define EATALL                  ATTR_ALL            /* for fsfirst function */
#endif /* PF_BACKWARD_COMPATIBILITY_PARAMETER */


/*---------------------------------------------------------------------------
  File open mode
 ----------------------------------------------------------------------------*/
#define PF_OPEN_MODE_WRITE          (0x01ul)
#define PF_OPEN_MODE_READ           (0x02ul)
#define PF_OPEN_MODE_APPEND         (0x04ul)
#define PF_OPEN_MODE_PLUS           (0x08ul)
#define PF_OPEN_MODE_NO_OVERWRITE   (0x10ul)
#define PF_OPEN_MODE_WRITE_ONLY     (0x20ul)
#define PF_OPEN_MODE_CONT_CLUSTER   (0x40ul)


/*---------------------------------------------------------------------------
  definition/macro for fsexec
 ----------------------------------------------------------------------------*/
/* fsexec specified types */
#define PF_FSEXEC_REMOVE        (0x01ul)
#define PF_FSEXEC_CHGMOD        (0x06ul)
#define PF_FSEXEC_FOPEN         (0x10ul)
#define PF_FSEXEC_OPENDIR       (0x20ul)
#define PF_FSEXEC_DELETEDIR     (0x40ul)
#define PF_FSEXEC_DTA_EXEC      (0x80000000ul)

#if PF_BACKWARD_COMPATIBILITY_PARAMETER
#define FS_NORMAL               (0)
#define FS_REMOVE               PF_FSEXEC_REMOVE
#define FS_CHMOD                (0x02ul)
#define FS_CHDMOD               (0x04ul)
#define FS_FLASH                (0x08ul)
#endif /* PF_BACKWARD_COMPATIBILITY_PARAMETER */

/* fsexec specified types */
#define PF_FSEXEC_WRITE         PF_OPEN_MODE_WRITE
#define PF_FSEXEC_READ          PF_OPEN_MODE_READ
#define PF_FSEXEC_APPEND        PF_OPEN_MODE_APPEND
#define PF_FSEXEC_PLUS          PF_OPEN_MODE_PLUS
#define PF_FSEXEC_WRITE_ONLY    PF_OPEN_MODE_WRITE_ONLY
#define PF_FSEXEC_CONT_CLUSTER  PF_OPEN_MODE_CONT_CLUSTER

/* create macro for PF_DTA_EXEC */
#define PF_DTACPY(a,b)          pf_memcpy((void*)b, (void*)a, sizeof(PF_DTA_EXEC));


/*---------------------------------------------------------------------------
  _init_prfile2/_setvolcfg/_getvolcfg option
 ----------------------------------------------------------------------------*/
typedef struct PF_VOL_CFG {
    PF_U_LONG   volume_config;      /* volume configurations  */
    PF_U_LONG   file_config;        /* file configurations    */
    PF_U_LONG   fat_buff_size;      /* FAT cache buffer size  */
    PF_U_LONG   data_buff_size;     /* data cache buffer size */
} PF_VOL_CFG;

#define PF_ALL_VOLUME                       ((PF_DRV_CHAR)0x7F)

/* for pf_vol_set.config */
#define PF_CHAR_CHECK_DISABLE               (0x00010000ul)  /* used for internal flag       */
#define PF_CHAR_CHECK_ENABLE                (0x00020000ul)  /* used for only a parameter    */
#if PF_EXFAT_SUPPORT
#define PF_UPDATE_VOLUME_DIRTY_DISABLE      (0x10000000ul)  /* Not update VolumeDirty bit   */
#define PF_UPDATE_VOLUME_DIRTY_ENABLE       (0x20000000ul)  /* update VolumeDirty bit       */
#define PF_CHECK_VOLUME_DIRTY_DISABLE       (0x40000000ul)  /* Not check VolumeDirty bit    */
#define PF_CHECK_VOLUME_DIRTY_ENABLE        (0x80000000ul)  /* Checke VolumeDirty bit       */
#endif /* PF_EXFAT_SUPPORT */

/* for PF_VOL_CFG.volume_config */
#define PF_SET_FAT_CACHE_SIZE               (0x00000001ul)
#define PF_SET_DATA_CACHE_SIZE              (0x00000002ul)

/* for p_vol->config */
#define PF_CONT_CLUSTER_ENABLE              (0x00040000ul)
#define PF_CONT_CLUSTER_DISABLE             (0x00080000ul)
#define PF_CONT_CLUSTER_DIS_NOT_BPS         (0x00100000ul | PF_CONT_CLUSTER_DISABLE)
#define PF_CONT_CLUSTER_DIS_NOT_DRIVER      (0x00200000ul | PF_CONT_CLUSTER_DISABLE)
#define PF_CONT_CLUSTER_DIS_NOT_BOUNDARY    (0x00400000ul | PF_CONT_CLUSTER_DISABLE)
#define PF_CONT_CLUSTER_DIS_NOT_DIVIDE      (0x00800000ul | PF_CONT_CLUSTER_DISABLE)
#define PF_CONT_CLUSTER_DIS_NOT_CLUSTER     (0x01000000ul)  /* used for internal flag */

/* for p_vol->file_config */
#define PF_UPDATE_ACCESS_DATE_DISABLE       (0x00000001ul)  /* used for internal flag       */
#define PF_UPDATE_ACCESS_DATE_ENABLE        (0x00000002ul)  /* used for only a parameter    */

#define PF_WRITE_TIMESTMP_AT_UPDATE         (0x00000004ul)  /* used for internal flag       */
#define PF_WRITE_TIMESTMP_AT_FCLOSE         (0x00000008ul)  /* use for only a parameter     */


/*---------------------------------------------------------------------------
  create option
 ----------------------------------------------------------------------------*/
#if PF_BACKWARD_COMPATIBILITY_CREATE
#define PF_BLOCK                PF_OPEN_MODE_CONT_CLUSTER

#if PF_STDLIB_ALIAS_SUPPORT
#define O_BLOCK                 PF_BLOCK
#else
#define FCP_O_BLOCK             PF_BLOCK
#endif /* PF_STDLIB_ALIAS_SUPPORT */
#endif /* PF_BACKWARD_COMPATIBILITY_CREATE */


/*---------------------------------------------------------------------------
  encoding mode
 ----------------------------------------------------------------------------*/
#define PF_ENCODE_MODE_UTF16            (0x01ul)
#define PF_ENCODE_MODE_UTF16_EXT        (0x02ul)
#define PF_ENCODE_MODE_UTF16_ONLY       (0x02ul)
#define PF_ENCODE_MODE_ALL              (PF_ENCODE_MODE_UTF16 | PF_ENCODE_MODE_UTF16_EXT)


/*---------------------------------------------------------------------------
  Clean mode for pf_cleandir(), pf_chmoddir()
 ----------------------------------------------------------------------------*/
#define PF_SEARCH_BELOW                 (0x00010000uL)  /* Directory below only mode */
#define PF_SEARCH_TREE                  (0x00020000uL)  /* Tree all mode             */
#define PF_SEARCH_TEST                  (0x00100000uL)  /* Test mode                 */

#define PF_CLEANDIR_BELOW               PF_SEARCH_BELOW
#define PF_CLEANDIR_TREE                PF_SEARCH_TREE
#define PF_CLEANDIR_TEST                PF_SEARCH_TEST

#if PF_SEARCH_BELOW != PF_CLEANDIR_BELOW
#error "PF_SEARCH_BELOW != PF_CLEANDIR_BELOW"
#endif
#if PF_SEARCH_TREE != PF_CLEANDIR_TREE
#error "PF_SEARCH_TREE != PF_CLEANDIR_TREE"
#endif
#if PF_SEARCH_TEST != PF_CLEANDIR_TEST
#error "PF_SEARCH_TEST != PF_CLEANDIR_TEST"
#endif


/*---------------------------------------------------------------------------
  The extension module
 ----------------------------------------------------------------------------*/
#if PF_MODULE_SUPPORT

/* The Module ID */
typedef PF_U_LONG   PF_MODULE_ID;

#define PF_MODULE_ID_FCS        (0x00000002ul)  /* Free Cluster Search Module */

#endif /* PF_MODULE_SUPPORT */


/*---------------------------------------------------------------------------
  Plug-in for LFS - statvfs_np()
 ----------------------------------------------------------------------------*/
struct PF_STATVFS_TAG
{
   /* same as statvfs */

       /* cluster size (byte) */
       PF_U_LONG  f_bsize;

       /* cluster size (byte) */
       PF_U_LONG  f_frsize;

       /* total number of clusters */
       PF_U_LONG  f_blocks;

       /* total number of free clusters */
       PF_U_LONG  f_bfree;

       /* total number of free clusters */
       PF_U_LONG  f_bavail;

       /* unsupported (always 0)*/
       PF_U_LONG  f_files;
       PF_U_LONG  f_ffree;
       PF_U_LONG  f_favail;
       PF_U_LONG  f_fsid;

       /* mount flag */
       PF_U_LONG  f_flag;

       /* max length of file name */
       PF_U_LONG  f_namemax;

   /* original */

       /* sector size (byte) */
       PF_U_LONG  f_ssize;

       /* cluster group size(byte) */
       PF_U_LONG  f_gsize;

       /* total number of free cluster groups */
       PF_U_LONG  f_gfree;
};

typedef struct PF_STATVFS_TAG PF_STATVFS;

/* flags for 'f_flag' */
#define PF_ST_READONLY     (1)    /* Readonly file system */
#define PF_ST_REMOVABLE    (2)    /* Removable file system */
#define PF_ST_NOSUID       (128)  /* Does not support setuid()/ setgid() semantics */


/*---------------------------------------------------------------------------
  Error numbers for API
 ----------------------------------------------------------------------------*/
#define PF_ERR_EPERM            (1)         /* Operation is not possible    */
#define PF_ERR_ENOENT           (2)         /* No such file or directory    */
#define PF_ERR_ESRCH            (3)         /* No such process              */
#define PF_ERR_EIO              (5)         /* I/O Error(Driver Error)      */
#define PF_ERR_ENOEXEC          (8)         /* Not Executable by internal   */
#define PF_ERR_EBADF            (9)         /* Bad file descriptor          */
#define PF_ERR_ENOMEM           (12)        /* Not enough system memory     */
#define PF_ERR_EACCES           (13)        /* Permission denied            */
#define PF_ERR_EBUSY            (16)        /* Can not use system resouces  */
#define PF_ERR_EEXIST           (17)        /* File already exists          */
#define PF_ERR_EISDIR           (21)        /* find directory when file req */
#define PF_ERR_EINVAL           (22)        /* Invalid argument             */
#define PF_ERR_ENFILE           (23)        /* Too many open files(system)  */
#define PF_ERR_EMFILE           (24)        /* Too many open files(user)    */
#define PF_ERR_EFBIG            (27)        /* Over file size limit(4GB-1)  */
#define PF_ERR_ENOSPC           (28)        /* Device out of space          */
#define PF_ERR_ENOLCK           (46)        /* Can not lock the file        */
#define PF_ERR_ENOSYS           (88)        /* Not implement function       */
#define PF_ERR_ENOTEMPTY        (90)        /* Directory is not empty       */
#define PF_ERR_EMOD_NOTSPRT     (100)       /* Module is not supported      */
#define PF_ERR_EMOD_NOTREG      (101)       /* Module is not registered     */
#define PF_ERR_EMOD_FCS         (102)       /* FreeClusterSearch Module Err */
#define PF_ERR_ENOMEDIUM        (123)       /* No medium found              */
#define PF_ERR_DFNC             (0x1000)    /* Driver Error                 */
#define PF_ERR_SYSTEM           (-1)        /* system error(general error)  */

#if PF_BACKWARD_COMPATIBILITY_ERRORCODE

#define FCP_ENOENT           PF_ERR_ENOENT  /* No such file or directory    */
#define FCP_EBADF            PF_ERR_EBADF   /* Bad file descriptor          */
#define FCP_EACCES           PF_ERR_EACCES  /* Permission denied            */
#define FCP_EEXIST           PF_ERR_EEXIST  /* File already exists          */
#define FCP_EINVAL           PF_ERR_EINVAL  /* Invalid argument             */
#define FCP_EMFILE           PF_ERR_EMFILE  /* Too many open files          */
#define FCP_ENOSPC           PF_ERR_ENOSPC  /* Device out of space          */
#define DFNC_ERR             PF_ERR_DFNC

#endif /* PF_BACKWARD_COMPATIBILITY_ERRORCODE */


/*---------------------------------------------------------------------------
  Internal Error numbers for API
 ----------------------------------------------------------------------------*/
#define PF_IER_NO_ERR                      (0)     /* No error; Done successfully     */
#define PF_IER_INVALID_FILENAME            (1)     /* Invalid file name               */
#define PF_IER_INVALID_PATHNAME            (2)     /* Invalid path name               */
#define PF_IER_FILE_NOT_FOUND              (3)     /* File not found                  */
#define PF_IER_TOO_MANY_VOLUMES_ATTACHED   (4)     /* Too many volumes attached       */
#define PF_IER_DIRECTORY_FULL              (5)     /* Directory full                  */
#define PF_IER_VOLUME_FULL                 (6)     /* Volume full                     */
#define PF_IER_INVALID_DISK_FORMAT         (7)     /* Invalid disk format             */
#define PF_IER_FILE_ALREADY_EXISTS         (8)     /* File already exists             */
#define PF_IER_VOLUME_NOT_MOUNTED          (9)     /* Volume not mounted              */
#define PF_IER_INVALID_PARAMETER           (10)    /* Invalid parameter               */
#define PF_IER_WRITE_PROTECTED             (11)    /* Write protected                 */
#define PF_IER_UNSUPPORTED_FORMAT          (12)    /* Unsupported disk format         */
#define PF_IER_BROKEN_CLUSTER_CHAIN        (13)    /* Broken cluster chain            */
#define PF_IER_INVALID_CLUSTER_NUM         (14)    /* Invalid cluster number          */
#define PF_IER_INVALID_BPB                 (15)    /* Invalid BPB in Boot Sector      */
#define PF_IER_ACCESS_OUT_OF_VOLUME        (16)    /* Access out of volume            */
#define PF_IER_DRIVER_ERROR                (17)    /* Driver error                    */
#define PF_IER_INVALID_VOLUME_LABEL        (18)    /* Invalid volume label            */
#define PF_IER_FILE_OPENED                 (19)    /* File is opened                  */
#define PF_IER_NOT_A_DIRECTORY             (20)    /* Not a directory                 */
#define PF_IER_TOO_MANY_FILES_OPENED_S     (21)    /* Too many files opened (SFD)     */
#define PF_IER_TOO_MANY_FILES_OPENED_U     (22)    /* Too many files opened (UFD)     */
#define PF_IER_NOT_A_FILE                  (23)    /* Not a (regular) file            */
#define PF_IER_READ_ONLY                   (24)    /* Read only                       */
#define PF_IER_LOCK_ERROR                  (25)    /* Lock error                      */
#define PF_IER_INTERNAL_ERROR              (26)    /* Internal error                  */
#define PF_IER_END_OF_FILE                 (27)    /* End of file                     */
#define PF_IER_ACCESS_NOT_ALLOWED          (28)    /* Access not allowed              */
#define PF_IER_DIRECTORY_NOT_EMPTY         (29)    /* Directory not empty             */
#define PF_IER_NOT_ENOUGH_CACHE_PAGES      (30)    /* Not enough cache pages          */
#define PF_IER_DIFFERENT_DRIVE             (31)    /* Drive is different              */
#define PF_IER_DIFFERENT_ENTRY             (32)    /* Entry is different              */
#define PF_IER_INVALID_ENTRY               (33)    /* Invalid directry entry          */
#define PF_IER_INVALID_SECTOR              (34)    /* Invalid sector number           */
#define PF_IER_BROKEN_VOLUME               (35)    /* Broken volume structure         */
#define PF_IER_NOT_EFFECTIVE               (36)    /* Processing is not effective     */
#define PF_IER_FILE_SIZE_OVER              (37)    /* Over file size limit(4GB-1)     */
#define PF_IER_INVALID_FILE_DISCRIPTOR     (38)    /* Invalid file discriptor         */
#define PF_IER_INVALID_LOCK_FILE           (39)    /* File is already locked          */
#define PF_IER_MODULE_NOT_SUPPORTED        (40)    /* The module isnot supported      */
#define PF_IER_MODULE_NOT_REGISTERED       (41)    /* The module isnot registered     */
#define PF_IER_MODULE_FCS_ERROR            (42)    /* FCS module error                */
#define PF_IER_VOLUME_DIRTY                (43)    /* Volume dirty                    */
#define PF_IER_MEDIA_FAILURE               (44)    /* Media failure                   */
#define PF_IER_INVALID_CHECKSUM            (45)    /* Invalid checksum                */
#define PF_IER_NO_MEDIUM                   (46)    /* No medium found                 */
#define PF_IER_CONTEXT_NOT_REGISTERED      (47)    /* The context isn't registered    */
#define PF_IER_MALLOC                      (48)    /* Not enough memory               */
#define PF_IER_DIR_LOCKED                  (49)    /* The directory is locked         */
#define PF_IER_DIR_ALREADY_LOCKED          (50)    /* The directory is already locked */


/*---------------------------------------------------------------------------
  Return numbers for API
 ----------------------------------------------------------------------------*/
#define PF_RET_NO_ERR           (0)         /* No error                     */
#define PF_RET_ERR              (-1)        /* API error                    */
#define PF_RET_NULL             NULL        /* NULL                         */
                                            /* [Use API] _create, fopen     */
#define PF_RET_NOT_END_OF_FILE  (0)         /* Not end of file              */
                                            /* [Use API] feof               */
#define PF_RET_END_OF_FILE      (1)         /* End of file                  */
                                            /* [Use API] feof               */
#define PF_RET_DERRNUM_ERR      (1)         /* _derrnum API error           */
                                            /* [Use API] _derrnum           */
#define PF_RET_MODERRNUM_ERR    (1)         /* _module_errnum API error     */
                                            /* [Use API] _ctrl_module,      */
                                            /*           _module_errnum     */
#define PF_RET_UNMOUNT_ERR      (1)         /* _unmount API error           */
                                            /* [Use API] _unmount           */
                                            /* Note: This is for force mode */

#endif /* __PF_APICMN_DEF_H__ */

