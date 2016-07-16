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
 [pf_common.h] - PrFILE2-local Common Definitions & Declarations

  NOTES:
   - No need to include this header for User Applications; This is used
     only for compiling PrFILE2 itself.
 ****************************************************************************/
#ifndef __PF_COMMON_H__
#define __PF_COMMON_H__

#include "prfile2/pf_apicmn.h"
#include "prfile2/pf_clib.h"
#if PF_UNICODE_INTERFACE_SUPPORT || PF_EXFAT_SUPPORT
#include "prfile2/pf_w_clib.h"
#endif /* PF_UNICODE_INTERFACE_SUPPORT || PF_EXFAT_SUPPORT */
#include "pf_str.h"
#include "pf_path.h"
#include "pf_code.h"
#include "pf_service.h"
#include "pf_critical.h"

/****************************************************************************
  Macro definitions
 ****************************************************************************/
/*---------------------------------------------------------------------------
  LOCAL_IS_DEFINED_AS_STATIC

    If set, 'LOCAL' functions and variables (file-local functions/variables
    with 'LOCAL' pseudo storage class specifier) are defined as 'static'
    functions and variables to prevent them from being called by external
    functions.

    If not set, 'LOCAL' functions and variables are defined as 'global'
    functions/variables with external linkage. This is useful to debug
    with a old-fashioned debugger which does not handle 'static' symbols.

 ----------------------------------------------------------------------------*/
#define LOCAL_IS_DEFINED_AS_STATIC      (1)

#if LOCAL_IS_DEFINED_AS_STATIC
  #define PF_LOCAL                      static
#else
  #define PF_LOCAL
#endif

/*---------------------------------------------------------------------------
  Resource limits
 ----------------------------------------------------------------------------*/
#define MAX_FILE_SYS_OPEN               PF_MAX_FILE_NUM
#define MAX_FILE_USR_OPEN               PF_MAX_FILE_NUM
#define MAX_DIR_SYS_OPEN                PF_MAX_DIR_NUM
#define MAX_DIR_USR_OPEN                PF_MAX_DIR_NUM
#define MAX_VALID_CONTEXT_IDX           PF_MAX_CONTEXT_NUM
#define MAX_DLOCK_OWNER                 (1)

/*---------------------------------------------------------------------------
  Internal Error numbers
 ----------------------------------------------------------------------------*/
#ifndef OK
#define OK                              NO_ERR    /* No error; Done successfully  */
#endif
#define NO_ERR                          (0)       /* No error; Done successfully  */
#define ERR                             (-1)      /* General error                */

#define ERR_INVALID_FILENAME            PF_IER_INVALID_FILENAME
#define ERR_INVALID_PATHNAME            PF_IER_INVALID_PATHNAME
#define ERR_FILE_NOT_FOUND              PF_IER_FILE_NOT_FOUND
#define ERR_TOO_MANY_VOLUMES_ATTACHED   PF_IER_TOO_MANY_VOLUMES_ATTACHED
#define ERR_DIRECTORY_FULL              PF_IER_DIRECTORY_FULL
#define ERR_VOLUME_FULL                 PF_IER_VOLUME_FULL
#define ERR_INVALID_DISK_FORMAT         PF_IER_INVALID_DISK_FORMAT
#define ERR_FILE_ALREADY_EXISTS         PF_IER_FILE_ALREADY_EXISTS
#define ERR_VOLUME_NOT_MOUNTED          PF_IER_VOLUME_NOT_MOUNTED
#define ERR_INVALID_PARAMETER           PF_IER_INVALID_PARAMETER
#define ERR_WRITE_PROTECTED             PF_IER_WRITE_PROTECTED
#define ERR_UNSUPPORTED_FORMAT          PF_IER_UNSUPPORTED_FORMAT
#define ERR_BROKEN_CLUSTER_CHAIN        PF_IER_BROKEN_CLUSTER_CHAIN
#define ERR_INVALID_CLUSTER_NUM         PF_IER_INVALID_CLUSTER_NUM
#define ERR_INVALID_BPB                 PF_IER_INVALID_BPB
#define ERR_ACCESS_OUT_OF_VOLUME        PF_IER_ACCESS_OUT_OF_VOLUME
#define ERR_DRIVER_ERROR                PF_IER_DRIVER_ERROR
#define ERR_INVALID_VOLUME_LABEL        PF_IER_INVALID_VOLUME_LABEL
#define ERR_FILE_OPENED                 PF_IER_FILE_OPENED
#define ERR_NOT_A_DIRECTORY             PF_IER_NOT_A_DIRECTORY
#define ERR_TOO_MANY_FILES_OPENED_S     PF_IER_TOO_MANY_FILES_OPENED_S
#define ERR_TOO_MANY_FILES_OPENED_U     PF_IER_TOO_MANY_FILES_OPENED_U
#define ERR_NOT_A_FILE                  PF_IER_NOT_A_FILE
#define ERR_READ_ONLY                   PF_IER_READ_ONLY
#define ERR_LOCK_ERROR                  PF_IER_LOCK_ERROR
#define ERR_INTERNAL_ERROR              PF_IER_INTERNAL_ERROR
#define ERR_END_OF_FILE                 PF_IER_END_OF_FILE
#define ERR_ACCESS_NOT_ALLOWED          PF_IER_ACCESS_NOT_ALLOWED
#define ERR_DIRECTORY_NOT_EMPTY         PF_IER_DIRECTORY_NOT_EMPTY
#define ERR_NOT_ENOUGH_CACHE_PAGES      PF_IER_NOT_ENOUGH_CACHE_PAGES
#define ERR_DIFFERENT_DRIVE             PF_IER_DIFFERENT_DRIVE
#define ERR_DIFFERENT_ENTRY             PF_IER_DIFFERENT_ENTRY
#define ERR_INVALID_ENTRY               PF_IER_INVALID_ENTRY
#define ERR_INVALID_SECTOR              PF_IER_INVALID_SECTOR
#define ERR_BROKEN_VOLUME               PF_IER_BROKEN_VOLUME
#define ERR_NOT_EFFECTIVE               PF_IER_NOT_EFFECTIVE
#define ERR_FILE_SIZE_OVER              PF_IER_FILE_SIZE_OVER
#define ERR_INVALID_FILE_DISCRIPTOR     PF_IER_INVALID_FILE_DISCRIPTOR
#define ERR_INVALID_LOCK_FILE           PF_IER_INVALID_LOCK_FILE
#define ERR_MODULE_NOT_SUPPORTED        PF_IER_MODULE_NOT_SUPPORTED
#define ERR_MODULE_NOT_REGISTERED       PF_IER_MODULE_NOT_REGISTERED
#define ERR_MODULE_FCS_ERROR            PF_IER_MODULE_FCS_ERROR
#define ERR_VOLUME_DIRTY                PF_IER_VOLUME_DIRTY
#define ERR_MEDIA_FAILURE               PF_IER_MEDIA_FAILURE
#define ERR_INVALID_CHECKSUM            PF_IER_INVALID_CHECKSUM
#define ERR_NO_MEDIUM                   PF_IER_NO_MEDIUM
#define ERR_CONTEXT_NOT_REGISTERED      PF_IER_CONTEXT_NOT_REGISTERED
#define ERR_MALLOC                      PF_IER_MALLOC
#define ERR_DIR_LOCKED                  PF_IER_DIR_LOCKED
#define ERR_DIR_ALREADY_LOCKED          PF_IER_DIR_ALREADY_LOCKED

/*---------------------------------------------------------------------------
  Internal return numbers
 ----------------------------------------------------------------------------*/
#define RET_NOT_END_OF_FILE             (0)     /* Not end of file              */
                                                /* [Use API] feof               */
#define RET_END_OF_FILE                 (1)     /* End of file                  */
                                                /* [Use API] feof               */
#define RET_UNMOUNT_ERR                 (1)     /* _unmount API error           */
                                                /* [Use API] _unmount           */
                                                /* Note: This is for force mode */

/*---------------------------------------------------------------------------
  File Open Mode (File Accessing Mode)

  Notes: Do not change the value of the macro.
         It is used by the bit shift.
 ----------------------------------------------------------------------------*/
#define PF_OPEN_MODE                PF_U_LONG
#define OPEN_MODE_COUNT_MAX         (7)

#define FILE_OPEN_READ              PF_OPEN_MODE_READ
#define FILE_OPEN_WRITE             PF_OPEN_MODE_WRITE
#define FILE_OPEN_APPEND            PF_OPEN_MODE_APPEND
#define FILE_OPEN_PLUS              PF_OPEN_MODE_PLUS
#define FILE_OPEN_NO_OVERWRITE      PF_OPEN_MODE_NO_OVERWRITE
#define FILE_OPEN_WRITE_ONLY        PF_OPEN_MODE_WRITE_ONLY
#define FILE_OPEN_CONT_CLUSTER      PF_OPEN_MODE_CONT_CLUSTER

#define IS_OPEN_MODE_READ(mode)     (((mode) & FILE_OPEN_READ)   != 0)
#define IS_OPEN_MODE_WRITE(mode)    (((mode) & FILE_OPEN_WRITE)  != 0)
#define IS_OPEN_MODE_APPEND(mode)   (((mode) & FILE_OPEN_APPEND) != 0)
#define IS_OPEN_MODE_PLUS(mode)     (((mode) & FILE_OPEN_PLUS)   != 0)
#define IS_OPEN_MODE_NO_OVERWRITE(mode) \
                                    (((mode) & FILE_OPEN_NO_OVERWRITE) != 0)
#define IS_OPEN_MODE_WRITE_ONLY(mode) \
                                    (((mode) & FILE_OPEN_WRITE_ONLY) != 0)
#define IS_OPEN_MODE_CONT_CLUSTER(mode) \
                                    (((mode) & FILE_OPEN_CONT_CLUSTER) != 0)

#define IS_OPEN_MODE_READABLE(mode) \
            (IS_OPEN_MODE_READ(mode) || IS_OPEN_MODE_PLUS(mode))
#define IS_OPEN_MODE_WRITABLE(mode) \
            ((mode) & (FILE_OPEN_WRITE | FILE_OPEN_APPEND | FILE_OPEN_PLUS))

/*---------------------------------------------------------------------------
  Bit-wise control
 ----------------------------------------------------------------------------*/
#define BSET(var,mask)          ((var) |= (mask))
#define BCLR(var,mask)          ((var) &= ~(mask))
#define BTST(var,mask)          ((var) & (mask))

/*---------------------------------------------------------------------------
  Bit number
 ----------------------------------------------------------------------------*/
#define PF_BIT_NUM_1            (1)
#define PF_BIT_NUM_2            (2)
#define PF_BIT_NUM_3            (3)
#define PF_BIT_NUM_4            (4)
#define PF_BIT_NUM_5            (5)

#define PF_BIT_NUM_8            (8)
#define PF_BIT_NUM_16           (16)
#define PF_BIT_NUM_24           (24)

/*---------------------------------------------------------------------------
  Byte order conversion
 ----------------------------------------------------------------------------*/
#if PF_BIG_ENDIAN

  /* For Big Endian */

  #define CHG_ITOI(X)           ((PF_U_SHORT)((((PF_U_SHORT)(X) & (U16)0x00ff) << 8) + \
                                              (((PF_U_SHORT)(X) & (PF_U_SHORT)0xff00) >> 8)))

  #define CHG_LTOL(X)           ((PF_U_LONG)((((PF_U_LONG)(X) & (PF_U_LONG)0x000000ff) << 24) + \
                                             (((PF_U_LONG)(X) & (PF_U_LONG)0x0000ff00) <<  8) + \
                                             (((PF_U_LONG)(X) & (PF_U_LONG)0x00ff0000) >>  8) + \
                                             (((PF_U_LONG)(X) & (PF_U_LONG)0xff000000) >> 24)))

  #define CHG_ITOL(X)           ((PF_U_LONG)((((PF_U_LONG)(X) & (PF_U_LONG)0x000000ff) << 24) + \
                                             (((PF_U_LONG)(X) & (PF_U_LONG)0x0000ff00) <<  8)))

#else /* ! PF_BIG_ENDIAN */

  /* For Little Endian */

  #define CHG_ITOI(X)           (X)
  #define CHG_LTOL(X)           (X)
  #define CHG_ITOL(X)           (X)

#endif /* PF_BIG_ENDIAN */

/*===========================================================================
  Macro constants definitions associated with FAT Format
 ============================================================================*/
/*---------------------------------------------------------------------------
  Size of directory entry
 ----------------------------------------------------------------------------*/
#define DIR_ENTRY_SIZE          (32)

#define LOG2_DIR_ENTRY_SIZE     (5)         /* log2(DIR_ENTRY_SIZE)         */

/*---------------------------------------------------------------------------
  File attributes
 ----------------------------------------------------------------------------*/

#define ATTR_TRUE_NONE          (0x00)

#define ATTR_NOT_WRITABLE_FILE  (ATTR_RDONLY | ATTR_VOLUME | ATTR_DIR)

/* flag combination used by long filename (LFN) entries */
#define ATTR_LFN                (ATTR_RDONLY | ATTR_HIDDEN | ATTR_SYSTEM | ATTR_VOLUME)

#define ATTR_LFN_MASK           (ATTR_RDONLY | ATTR_HIDDEN | ATTR_SYSTEM | ATTR_VOLUME | \
                                 ATTR_DIR | ATTR_ARCH)

#define ATTR_FILE               (ATTR_ALL &~ (ATTR_VOLUME | ATTR_DIR))

#define ATTR_FILE_DIR           (ATTR_ALL &~ ATTR_VOLUME)

/* mask of all valid file attributes */
#define ATTR_MASK               ((PF_U_CHAR)(ATTR_RDONLY | ATTR_HIDDEN | ATTR_SYSTEM | \
                                             ATTR_VOLUME | ATTR_DIR | ATTR_ARCH | ATTR_NONE))

/* All except the volume */
#define ATTR_ALL_WITHOUT_VOLUME ((PF_U_CHAR)(ATTR_RDONLY | ATTR_HIDDEN | ATTR_SYSTEM | \
                                             ATTR_DIR | ATTR_ARCH | ATTR_NONE))

#define IS_ATTR_SET(var,flags)  (((var)&(flags)) != 0)
#define SET_ATTR(var,flags)     ((var) |= (flags))
#define CLEAR_ATTR(var,flags)   ((var) &= ~(flags))


#define IS_ATTR_LFN(var)        (((var) & ATTR_LFN) == ATTR_LFN)
#define IS_ATTR_DIR(var)        IS_ATTR_SET((var),ATTR_DIR)
#define IS_ATTR_VOLUME(var)     IS_ATTR_SET((var),ATTR_VOLUME)
#define IS_ATTR_RDONLY(var)     IS_ATTR_SET((var),ATTR_RDONLY)

/* Writable regular file */
#define IS_ATTR_WRITABLE_FILE(var)  (((var) & (ATTR_RDONLY | ATTR_VOLUME | ATTR_DIR)) == 0)

/*---------------------------------------------------------------------------
  Initial file attributes for newly created files, directories, and
  volume labels.
 ----------------------------------------------------------------------------*/
/* initial attributes for newly created files */
#define EAT_INIT_FILE           ATTR_ARCH

/* initial attributes for newly created directories */
#define EAT_INIT_DIR            ATTR_DIR

/* initial attributes for newly created volume labels */
#define EAT_INIT_VOL            ATTR_VOLUME

/*---------------------------------------------------------------------------
  Count of nest max
 ----------------------------------------------------------------------------*/
#define PF_NEST_MAX             (128)

/*---------------------------------------------------------------------------
  FSINFO setup macro
 ----------------------------------------------------------------------------*/
#define PF_FSI_TRUST_MASK       (PF_FSI_TRUST_ENABLE | PF_FSI_TRUST_DISABLE)
#define PF_FSI_REFRESH_MASK     (PF_FSI_REFRESH_ENABLE | PF_FSI_REFRESH_DISABLE)
#define PF_FSI_PARAM_MASK       (PF_FSI_TRUST_MASK | PF_FSI_REFRESH_MASK)

/*---------------------------------------------------------------------------
  Special sector numbers
 ----------------------------------------------------------------------------*/
#define SECTOR_VOID             ((PF_U_LONG)0xFFFFFFFFuL)   /* void value */

/*===========================================================================
  Macro constants definitions associated with FAT Format
 ============================================================================*/
/*---------------------------------------------------------------------------
  Error Number Record to Volume Structure
 ----------------------------------------------------------------------------*/
#define USE_VOLUME_ERROR            (1)

/*---------------------------------------------------------------------------
  FAT Type
 ----------------------------------------------------------------------------*/
#define FAT_12                          PF_FMT_FAT12
#define FAT_16                          PF_FMT_FAT16
#define FAT_32                          PF_FMT_FAT32
#define FAT_EX                          PF_FMT_EXFAT
#define FAT_ERR                         PF_FMT_ERR

#if PF_EXFAT_SUPPORT
#define IS_VALID_FAT_TYPE(fat_type)     ((fat_type) == FAT_12 || \
                                         (fat_type) == FAT_16 || \
                                         (fat_type) == FAT_32 || \
                                         (fat_type) == FAT_EX)
#else /* ! PF_EXFAT_SUPPORT */
#define IS_VALID_FAT_TYPE(fat_type)     ((fat_type) == FAT_12 || \
                                         (fat_type) == FAT_16 || \
                                         (fat_type) == FAT_32)
#endif /* ! PF_EXFAT_SUPPORT */

/*===========================================================================
  Prototype definition
 ============================================================================*/
typedef struct PF_VOLUME_SET        PF_VOLUME_SET;
typedef struct PF_BPB               PF_BPB;

typedef struct PF_FFD               PF_FFD;
typedef struct PF_SDD               PF_SDD;

#if !PF_EXFAT_SUPPORT
typedef struct PF_DENT_FAT_TAG      PF_DENT;
#endif /* !PF_EXFAT_SUPPORT */
typedef struct PF_DENT_FAT_TAG      PF_DENT_FAT;
typedef struct PF_ENT_ITER_TAG      PF_ENT_ITER;
typedef struct PF_DIR_TAIL          PF_DIR_TAIL;

#if PF_EXFAT_SUPPORT
typedef struct PF_DENT_EXFAT_TAG    PF_DENT;
typedef struct PF_DENT_EXFAT_TAG    PF_DENT_EXFAT;
typedef struct PF_DENT_INFO_TAG     PF_DENT_INFO;
typedef struct PF_ALLOCATE_BMP_TAG  PF_ALLOCATE_BMP;
#endif /* PF_EXFAT_SUPPORT */

#if PF_USE_API_STUB
typedef struct FCPFNC               FCPFNC;
#endif /* PF_USE_API_STUB */

#endif  /* __PF_COMMON_H__ */
