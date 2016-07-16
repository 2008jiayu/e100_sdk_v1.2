/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaFS.h
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

#ifndef _AMBA_FS_H_
#define _AMBA_FS_H_

#include "AmbaFS_Def.h"

/*-----------------------------------------------------------------------------------------------*\
 * Macro definitions
\*-----------------------------------------------------------------------------------------------*/
#define AMBA_FS_CWD_LEN             256
#define AMBA_FS_NAME_LEN            520

#define AMBA_FS_RFS_FORMAT_ID       (0x8c)

#define AMBA_FS_SLOT_A              SCM_SLOT_FL0
#define AMBA_FS_SLOT_B              SCM_SLOT_FL1
#define AMBA_FS_SLOT_C              SCM_SLOT_SD0
#define AMBA_FS_SLOT_D              SCM_SLOT_SD1
#define AMBA_FS_SLOT_E              SCM_SLOT_RD
#define AMBA_FS_SLOT_Y              SCM_SLOT_RF1
#define AMBA_FS_SLOT_Z              SCM_SLOT_RF2
#define AMBA_FS_MAX_DRIVE           (AMBA_FS_SLOT_Z + 1)
#define AMBA_FS_UNKNOWN_SLOT        (-1)

/* Code Mode */
#define AMBA_FS_ASCII               0
#define AMBA_FS_UNICODE             1

/* Filesystem type */
#define AMBA_FS_NULL                0
#define AMBA_FS_FAT                 1
#define AMBA_FS_RFS                 2

/* error code */
#define AMBA_FS_INTERNAL_ERROR      (-1)
#define AMBA_FS_PARAMETER_ERROR     (-2)
#define AMBA_FS_UNKNOWN_FS_ERROR    (-3)
#define AMBA_FS_UNKNOWN_FS_TYPE     (-4)
#define AMBA_FS_RFS_INTERNAL_ERROR  (-5)

/* Macros for buffering */
#define AMBA_FS_WRTSOON              PF_WRTSOON
#define AMBA_FS_NWRTSOON             PF_NWRTSOON
#define AMBA_FS_AUTOSYNC             PF_AUTOSYNC
#define AMBA_FS_NAUTOSYNC            PF_NAUTOSYNC
#define AMBA_FS_META_WRITE_THRU      PF_FAT_WRITE_THRU
#define AMBA_FS_META_WRITE_BACK      PF_FAT_WRITE_BACK

/* Used for ff_buffering functin */
#define AMBA_FS_EJECT_SAFE_WITH_WRITE_THRU      (AMBA_FS_WRTSOON)
#define AMBA_FS_EJECT_SAFE_WITH_WRITE_BACK      (AMBA_FS_NWRTSOON | AMBA_FS_AUTOSYNC | \
                                                 AMBA_FS_META_WRITE_THRU)
#define AMBA_FS_WRITE_BACK_ON_SIGNIF_API        (AMBA_FS_NWRTSOON | AMBA_FS_AUTOSYNC)
#define AMBA_FS_WRITE_BACK_CACHE                (AMBA_FS_NWRTSOON | AMBA_FS_NAUTOSYNC)

#define AMBA_FS_WRITE_BACK_CACHE_ON_SYNC_CLOSE  0x80

/* Macros for sync */
#define AMBA_FS_NINVALIDATE                     PF_NINVALIDATE
#define AMBA_FS_INVALIDATE                      PF_INVALIDATE

/*-----------------------------------------------------------------------------------------------*\
 * AmbaFS data structure definition
\*-----------------------------------------------------------------------------------------------*/
#define AMBA_FS_OPEN_MODE_WRITE         0x01
#define AMBA_FS_OPEN_MODE_READ          0x02
#define AMBA_FS_OPEN_MODE_APPEND        0x04
#define AMBA_FS_OPEN_MODE_PLUS          0x08
#define AMBA_FS_OPEN_MODE_NO_OVERWEITE  0x10

typedef struct _AMBA_FS_FILE_s_ {
    int Slot;       /* Record the location of file.*/
    int FsType;     /* FileSystem Type. Ex: Fat or Romfs */
    union {
        PF_FILE *pFat;
    } Fp;
} AMBA_FS_FILE;

#define AMBA_FS_MAX_SHORT_NAME_LEN      13
#define AMBA_FS_MAX_LONG_NAME_LEN       256

typedef struct _AMBA_FS_DIRENT_s_ {
    int     FsType;
    char    FileName[AMBA_FS_MAX_SHORT_NAME_LEN * 2];
    char    LongName[AMBA_FS_MAX_LONG_NAME_LEN * 2];
    UINT32  Attr;
    UINT64  Size;
    UINT32  Year;
    UINT32  Month;
    UINT32  Day;
    UINT32  Hour;
    UINT32  Minute;
    UINT32  Second;
} AMBA_FS_DIRENT;

typedef union _AMBA_FS_DTA_u_ {
    DTA dta;
    WDTA wdta;
} AMBA_FS_DTA_UNION;

typedef struct _AMBA_FS_DTA_s_ {
    AMBA_FS_DTA_UNION u;
    int FsType;
    UINT16 Time;
    UINT16 Date;
    UINT64 FileSize;
    UINT32 StartClst;
    char FileName[AMBA_FS_NAME_LEN];
    char LongName[AMBA_FS_NAME_LEN];
    char Attribute;
    int SearchMode; /* search for single file, wildcard(Regx). */
#define AMBA_FS_WILDCARD_SEARCH_ALL     1
#define AMBA_FS_WILDCARD_SEARCH_PARTIAL 2
#define AMBA_FS_SEARCH_SINGLE           3
    void *pRomfsCurrentInode;   /* current inode for searching files */
} AMBA_FS_DTA;

#define AMBA_FS_WILDCARD_SEARCH_ALL     1
#define AMBA_FS_WILDCARD_SEARCH_PARTIAL 2
#define AMBA_FS_SEARCH_SINGLE           3

typedef struct _AMBA_FS_FILE_DATE_s_ {
    UINT16  Day:            5;  /* [4:0]: 1-31 */
    UINT16  Month:          4;  /* [8:5]: 1-12 */
    UINT16  Year:           7;  /* [15:9]+1980: 0-127: (1980-2107) */
} AMBA_FS_FILE_DATE;

/* The valid time range is from Midnight 00:00:00 to 23:59:58. */
typedef struct _AMBA_FS_FILE_TIME_s_ {
    UINT16  Second:         5;  /* [4:0]*2: 0-29 (0-58) */
    UINT16  Minute:         6;  /* [10:5]: 0-59 */
    UINT16  Hour:           5;  /* [15:11]: 0-23 */
} AMBA_FS_FILE_TIME;

typedef struct _AMBA_FS_FILE_COMP_TIME_s_ {
    UINT16  Second:         8;  /* [7:0]*10: 0-199 (0-1990 ms)*/
    UINT16  Hour:           8;  /* [15:4] */
} AMBA_FS_FILE_COMP_TIME;

typedef struct _AMBA_FS_STAT_s_ {
    PF_STAT                 Stat;
    UINT64                  Size;           /* file size in bytes */
    AMBA_FS_FILE_DATE       LastAccDate;    /* Update on read or write */
    AMBA_FS_FILE_TIME       LastAccTime;    /* Update on read or write */
    AMBA_FS_FILE_TIME       LastMdyTime;    /* Update on write */
    AMBA_FS_FILE_DATE       LastMdyDate;    /* Update on write */
    AMBA_FS_FILE_TIME       CreateTime;     /* Update on create */
    AMBA_FS_FILE_DATE       CreateDate;     /* Update on create */
    AMBA_FS_FILE_COMP_TIME  CreateCompTime; /* Update on create */
    UINT32                  Attr;
} AMBA_FS_STAT;

#define AMBA_FS_MAX_VOL_NAME_LEN            12
#define AMBA_FS_VOLSRC_BOOT_SECTOR          FX_BOOT_SECTOR
#define AMBA_FS_VOLSRC_DIRECTORY_SECTOR     FX_DIRECTORY_SECTOR

typedef struct _AMBA_FS_VOLTAB_s_ {
    PF_VOLTAB   VolTab;
    int     FsType;
    char    Name[AMBA_FS_MAX_VOL_NAME_LEN];
    UINT32  VolSrc;
    UINT8   VolAttr;
    UINT32  VolDate;
    UINT32  VolTime;
} AMBA_FS_VOLTAB;

typedef enum _AMBA_FS_FAT_TYPE_s_ {
    AMBA_FS_FMT_FAT12  = 0,
    AMBA_FS_FMT_FAT16  = 1,
    AMBA_FS_FMT_FAT32  = 2,
    AMBA_FS_FMT_EXFAT  = 3,
    AMBA_FS_FMT_ERR    = -1
} AMBA_FS_FAT_TYPE_e;

typedef struct _AMBA_FS_DEVINF_s_ {
    PF_DEV_INF          DevInfo;
    UINT32              Cls;        /* Total clusters */
    UINT32              Ucl;        /* Unused clusters */
    UINT32              Bps;        /* Bytes per sector */
    UINT32              Spc;        /* Sectors per cluster */
    UINT32              Cpg;        /* Clusters per cluster group */
    UINT32              Ucg;        /* Unused cluster groups */
    AMBA_FS_FAT_TYPE_e  Fmt;        /* Format type */
} AMBA_FS_DEVINF;

typedef enum _AMBA_FS_ATTR_e_ {
    /* Standard file attribute */
    AMBA_FS_ATTR_RDONLY         =   0x01,
    AMBA_FS_ATTR_HIDDEN         =   0x02,
    AMBA_FS_ATTR_SYSTEM         =   0x04,
    AMBA_FS_ATTR_VOLUME         =   0x08,
    AMBA_FS_ATTR_DIR            =   0x10,
    AMBA_FS_ATTR_ARCH           =   0x20,

    /* Other file attribute */
    AMBA_FS_ATTR_NONE           =   0x40,
    AMBA_FS_ATTR_ALL            =   0x7f,
    AMBA_FS_ATTR_FILE_ONLY      =   0x100,
} AMBA_FS_ATTR_e;

typedef enum _AMBA_FS_MODE_e_ {
    /* Comparing mode */
    AMBA_FS_MODE_CMP_AND        =   0x0080,
    AMBA_FS_MODE_CMP_MATCH      =   0x1000,

    /* Attribute Change mode */
    AMBA_FS_MODE_ATTR_ADD       =   0x2000,
    AMBA_FS_MODE_ATTR_SUB       =   0x4000,

    /* Search mode */
    AMBA_FS_MODE_SEARCH_BELOW   =   0x10000,
    AMBA_FS_MODE_SEARCH_TREE    =   0x20000,

    /* Test mode */
    AMBA_FS_MODE_TEST           =   0x10000000,
} AMBA_FS_MODE_e;

/* fsexec specified types */
#define AMBA_FS_FSEXEC_REMOVE        (0x01ul)
#define AMBA_FS_FSEXEC_CHGMOD        (0x06ul)
#define AMBA_FS_FSEXEC_FOPEN         (0x10ul)
#define AMBA_FS_FSEXEC_OPENDIR       (0x20ul)
#define AMBA_FS_FSEXEC_DELETEDIR     (0x40ul)
#define AMBA_FS_FSEXEC_DTA_EXEC      (0x80000000ul)

/* fsexec specified types */
#define AMBA_FS_FSEXEC_WRITE         (0x01)
#define AMBA_FS_FSEXEC_READ          (0x02)
#define AMBA_FS_FSEXEC_APPEND        (0x04)
#define AMBA_FS_FSEXEC_PLUS          (0x08)
#define AMBA_FS_FSEXEC_WRITE_ONLY    (0x20)
#define AMBA_FS_FSEXEC_CONT_CLUSTER  (0x40)

#define AMBA_FS_DIR         PF_DIR
#define AMBA_FS_FPOS        PF_FPOS_T
#define AMBA_FS_DTA_EXEC    PF_DTA_EXEC

/*------------------------------------
 * Every slot has one ff_env instance.
\*-----------------------------------*/
typedef struct _AMBA_FS_ENV_s_ {
    int     Status;                         /* status of env */
#define AMBA_FS_ACTIVE_SLOT 0x1
    int     FSType;                         /* UDF or FAT fs */
    char    Cwd[AMBA_FS_CWD_LEN];           /* current working directory */
    UINT16  UnicodeCCwd[AMBA_FS_CWD_LEN];   /* Unicode current working directory */
    int     BufMode;                        /* buffering mode */
} AMBA_FS_ENV;

/*-----------------------------------------------------------------------------------------------*\
 * Exported APIs.
\*-----------------------------------------------------------------------------------------------*/
AMBA_FS_FILE *AmbaFS_fopen(const char *pFileName, const char *pMode);
int AmbaFS_fclose(AMBA_FS_FILE *pFile);

int AmbaFS_remove(const char *pFileName);
int AmbaFS_rename(const char *pOldName, const char *pNewName);
int AmbaFS_Move(const char *pSrcName, const char *pDstName);

UINT64 AmbaFS_fread(void *pBuf, UINT64 Size, UINT64 Count, AMBA_FS_FILE *pFile);
UINT64 AmbaFS_fwrite(const void *pBuf, UINT64 Size, UINT64 Count, AMBA_FS_FILE *pFile);
UINT64 AmbaFS_fappend(AMBA_FS_FILE * pFile, UINT64 Size);
int AmbaFS_fadjust(AMBA_FS_FILE * pFile);
int AmbaFS_fseek(AMBA_FS_FILE *pFile, INT64 Offset, int Origin);
typedef enum _AMBA_FS_SEEK_ORIGIN_ {
    AMBA_FS_SEEK_START = 0,
    AMBA_FS_SEEK_CUR,
    AMBA_FS_SEEK_END,
} AMBA_FS_SEEK_ORIGIN_e;

int AmbaFS_fsexec(AMBA_FS_DTA *pDta, int Type, unsigned char Attr);
AMBA_FS_FILE *AmbaFS_GetFpFromDta(AMBA_FS_DTA *pDta);

int AmbaFS_FirstDirEnt(const char *pDirName, unsigned char Attr, AMBA_FS_DTA *pDirEnt);
int AmbaFS_NextDirEnt(AMBA_FS_DTA *pDirEnt);
int AmbaFS_Chdir(const char *pDirName);
int AmbaFS_Mkdir(const char *pDirName);
int AmbaFS_Rmdir(const char *pDirName);

int AmbaFS_Stat(const char *pName, AMBA_FS_STAT *pStat);
int AmbaFS_SetVol(char Drive, const char *pVolName);
int AmbaFS_GetVol(char Drive, AMBA_FS_VOLTAB *pVolTab);
int AmbaFS_GetDev(char Drive, AMBA_FS_DEVINF *pDevInf);
int AmbaFS_SpaceAvaiable(char Drive, UINT64 *pSizeByte);

int AmbaFS_Chmod(const char *pFileName, int Attr);
int AmbaFS_Chdmod(const char *pDirName, int Attr);
int AmbaFS_ChmodDir(const char * pPath, const char * pFileName,
                    UINT32 Mode, UINT32 Attr, UINT32 * pCount);

int AmbaFS_Cinsert(const char *pFileName, UINT32 Offset, UINT32 Number);
int AmbaFS_Cdelete(const char *pFileName, UINT32 Offset, UINT32 Number);
int AmbaFS_Combine(char *pFileNameBase, char *pFileNameAdd);
int AmbaFS_Divide(char *pOriginPath, char *pNewPath, UINT32 Offset);

int AmbaFS_Format(char Drive, const char *pParam);
int AmbaFS_FSync(AMBA_FS_FILE *pFile);
int AmbaFS_Sync(char Drive, int Mode);
typedef enum _AMBA_FS_SYNC_MODE_e_ {
    AMBA_FS_SYNC_FLUSH      =   0,  /* flush and not invalidate cache */
    AMBA_FS_SYNC_INVALIDATE =   1,  /* flush and invalidate cache */
} AMBA_FS_SYNC_MODE_e;

INT64 AmbaFS_ftell(AMBA_FS_FILE *pFile);
int AmbaFS_feof(AMBA_FS_FILE *pFile);

int AmbaFS_CleanDir(const char *pDirName, const char *pFileName, UINT32 OpMode, UINT32 *pCount);
int AmbaFS_DeleteDir(const char *pDirName);

/**********************************/
/* Initial and utility functions. */
/**********************************/
int AmbaFS_Init(UINT32 CodeMode);
UINT32 AmbaFS_GetCodeMode(void);
AMBA_FS_FILE *AmbaFS_GetFileInstance(void);
void AmbaFS_FreeFileInstance(int Slot);
AMBA_FS_ENV *AmbaFS_GetEnv(void);
const char *AmbaFS_GetErrorMsg(int Error);
char* AmbaFS_Strcpy(char *pDest, const char *pSrc);
char* AmbaFS_Strcat(char *pDest, const char *pSrc);
void AmbaFS_UpdateActiveSlot(int Slot);
void AmbaFS_GetError(int * pError);
char AmbaFS_GetDrive(const char *path);
int AmbaFS_GetFSType(char Drive);
int AmbaFS_SetCwd(char *pCwd);
int AmbaFS_GetCwd(char *pCwd);
int AmbaFS_GetActiveSlot(void);
int AmbaFS_SetBufferingMode(char Drive, int Mode);
int AmbaFS_GetBufferingMode(char Drive);
int AmbaFS_Mount(char Drive);
int AmbaFS_Unmount(char Drive);
int AmbaFS_CheckLeftSpace(int Slot, UINT32 LeftSpaceKb);
int AmbaFS_CheckFragment(char slot);
extern void AmbaFS_SdPrf2CheckMedia(int Id);

/***********************************************************************/
/* Automatic UNICODE conversion support to deal with CPxxx <-> UNICODE */
/* type access. This happens when:                                     */
/*   1. The underlying file system supports UNICODE filenames.         */
/*   2. This mode is enabled (which is enabled by default).            */
/*   3. The string passed into the AmbaFS_xxx() call is detected to be */
/*      UNICODE.                                                       */
/*                                                                     */
/* To further clarify, take the example of PrFILE2:                    */
/* - PrFILE2 supports passing the filename into its functions as       */
/*   a) ASCII string bound to a particular code page, or               */
/*   b) UTF-16 string                                                  */
/* - Each of the two types of functions mentioned above uses different */
/*   flavors of function calls in the form of pf2_xxx(), or pf2_w_xxx()*/
/*   calls.                                                            */
/*                                                                     */
/* Now, each task has three possible modes: { none, auto, always}      */
/* associated with it to invoke when a corresponding ff_xxx() call     */
/* is requested. The implementation in the AMBAFS layer is thus:       */
/*   - If the mode is set to 'none', then the filename is assumed to   */
/*     be in ASCII bound to a code-page, and pf2_xxx() function is     */
/*     invoked.                                                        */
/*   - If the mode is set to 'always', then the filename is assumed to */
/*     be in UTF-16, and pf2_w_xxx() function is invoked.              */
/*   - If the mode is set to 'auto, then the content of the parameter  */
/*     'filename' is used to determine (with best effort) whether it   */
/*     it is an ASCII or UTF-16 string and determine which PrFILE2     */
/*     function to invoke.                                             */
/*                                                                     */
/* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!  */
/* NOTE: The AMBA_FS_UC_AUTO_MODE is *STRONGLY* discourage from being  */
/*       used since auto-detecting encoding format between             */
/*       ASCII & UTF-16 is error-prone!                                */
/* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!  */
/***********************************************************************/

#define AMBA_FS_UC_NONE     0x0
#define AMBA_FS_UC_ALWAYS   0x1
#define AMBA_FS_UC_AUTO     0x2

void AmbaFS_SetUnicodeForAll(UINT8 Mode);
int AmbaFS_SetUnicodeForThisTask(UINT8 Mode);
int AmbaFS_SetUnicodeForTask(UINT32 ThreadID, UINT8 Mode);
UINT8 AmbaFS_GetUnicodeForThisTask(void);
UINT8 AmbaFS_GetUnicodeForTask(UINT32 ThreadID);

#endif  /* _AMBA_FS_H_ */
