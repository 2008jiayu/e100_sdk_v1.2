/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaNVD.h
 *
 *  @Copyright      :: Copyright (C) 2013 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Definitions & Constants for Non-volatile data instance APIs.
\*-------------------------------------------------------------------------------------------------------------------*/

#ifndef _AMBA_NVD_H_
#define _AMBA_NVD_H_

/*-----------------------------------------------------------------------------------------------*\
 * Non-volatile data instance ID.
\*-----------------------------------------------------------------------------------------------*/
#define NVD_ID_PREF             0
#define NVD_ID_CALIB            1
#define NVD_ID_MAX              2

/*-----------------------------------------------------------------------------------------------*\
 * Maximun number of objects of non-volatile data instances.
\*-----------------------------------------------------------------------------------------------*/

#define NVD_PREF_MAX_OBJS       5
#define NVD_CALIB_MAX_OBJS      32
#define NVD_MAX_SECTORS         1024

#if (NVD_PREF_MAX_OBJS > NVD_CALIB_MAX_OBJS)
#define NVD_MAX_OBJS            NVD_PREF_MAX_OBJS
#else
#define NVD_MAX_OBJS            NVD_CALIB_MAX_OBJS
#endif

/*-----------------------------------------------------------------------------------------------*\
 * NVD support thread-safe.
\*-----------------------------------------------------------------------------------------------*/
#define NVD_LOCK                1

#define NVD_WAIT_INIT_TIMEOUT   5000

/*-----------------------------------------------------------------------------------------------*\
 * Error codes of nvd APIs.
\*-----------------------------------------------------------------------------------------------*/
#define NVD_FLASH_READ_ERROR            (-21)
#define NVD_FLASH_WRITE_ERROR           (-22)
#define NVD_SAVE_META_ERROR             (-23)
#define NVD_OUT_OF_SYSTEM_MEMORY_ERROR  (-24)
#define NVD_OUT_OF_FLASH_MEMORY_ERROR   (-25)
#define NVD_NOT_INIT_ERROR              (-26)
#define NVD_INVALID_META_ERROR          (-27)
#define NVD_ERR_UNKNOW_NVD_ID           (-28)
#define NVD_SM_INIT_ERROR               (-29)
#define NVD_INVALID_SIZE                (-30)
#define NVD_INVALID_OBJECT_ID           (-31)
#define NVD_OBJECT_NOT_EXIST            (-32)
#define NVD_ERR_OUT_OF_SYSTEM_RESOURCE  (-33)
#define NVD_SYSTEM_ERROR                (-34)

#define NVD_CRC_ERROR                   (-60)
#define NVD_FATAL_ERROR                 (-100)

/*-----------------------------------------------------------------------------------------------*\
 * A binary data object that are stored physically in the flash device.
\*-----------------------------------------------------------------------------------------------*/
typedef struct _NVD_OBJECT_s_ {
    UINT32 Size;                    /**< data size */
    UINT32 StartSec;                /**< start of sector */
    UINT32 NumSecsUsed;             /**< number of sector used */
    UINT32 Crc;                     /**< CRC32 check sum */
} NVD_OBJECT_s;

#define NVD_OBJECT_SIZE             sizeof(struct _NVD_OBJECT_s_)

/*-----------------------------------------------------------------------------------------------*\
 * The meta data structure that stored in nand-flash.
\*-----------------------------------------------------------------------------------------------*/
typedef struct _NVD_META_s_ {
    UINT32 Magic;                   /**< magic number for meta data */
    UINT32 MaxObjs;                 /**< max objects can be stored */
    UINT32 Size;                    /**< meta size including the alloc table */

    struct _NVD_OBJECT_s_ Object[NVD_MAX_OBJS];

    UINT32 StartSec;                /**< start sector of meta */
    UINT32 NumSecs;                 /**< sector number of meta */
    UINT32 FreeSecs;                /**< free sector number of SM */
    UINT32 Crc;                     /**< CRC32 check sum */
} NVD_META_s;

#define NVD_MANAGER_EMPTY           0xffffffff
#define NVD_MANAGER_MAGIC(x)        (0x3044564e + (x << 24))
#define NVD_MANAGER_START_SEC       0

/*-----------------------------------------------------------------------------------------------*\
 * Information about logical layout that are used by the nvd module.
\*-----------------------------------------------------------------------------------------------*/
struct _NVD_SM_INFO_s_ {
    int ID;                         /**< SM ID of partition */

    UINT32 StartSec;                /**< nvd start sector */
    UINT32 TotalSecs;               /**< total sectorss available */
    UINT32 TotalSize;               /**< total avaiable size */
    UINT32 SecSize;                 /**< sector size */
};

/*-----------------------------------------------------------------------------------------------*\
 * Functions to interface sector-media functions.
\*-----------------------------------------------------------------------------------------------*/
struct _NVD_SM_FUNC_s_ {
    int (*IsInit)(int ID);
    int (*Init)(int ID, int Mode);
    int (*GetTotalSectors)(int ID, UINT32 *pTotalSecs);
    int (*Read)(int ID, UINT8 *pBuf, UINT32 Sector, UINT32 Sectors);
    int (*Write)(int ID, UINT8 *pBuf, UINT32 Sector, UINT32 Sectors);
    int (*ErasePartition)(int ID);
};

/*-----------------------------------------------------------------------------------------------*\
 * Preference manager data structure.
\*-----------------------------------------------------------------------------------------------*/
typedef struct _NVD_MANAGER_s_ {
    int Init;
    int ID;
    UINT32 MaxObjs;                         /**< max objects can be stored */

    struct _NVD_META_s_     Meta;
    struct _NVD_SM_INFO_s_  SmInfo;
    struct _NVD_SM_FUNC_s_  SmFunc;

    AMBA_KAL_BLOCK_POOL_t SecLinkPool;      /**< Fixed size memory pool for sector link */

    UINT32 *pObjAllocTbl;                   /**< object allocation table */
} NVD_MANAGER_s;

#define NVD_FREE_SECTOR             0x00000000
#define NVD_END_OF_CHAIN            0xfffffff8
#define NVD_META_USED_SECTOR        0xfffffffe

/*-----------------------------------------------------------------------------------------------*\
 * Stat data structures of nvd.
\*-----------------------------------------------------------------------------------------------*/
typedef struct _AMBA_NVD_INSTANCE_s_ {
    NVD_MANAGER_s       NvdManager;

    AMBA_KAL_MUTEX_t    Mutex;      /* Mutex */
    int Lock;                       /* lock to prevent concurrent access */
} AMBA_NVD_INSTANCE_s;

/*-----------------------------------------------------------------------------------------------*\
 * Stat data structures of nvd.
\*-----------------------------------------------------------------------------------------------*/
typedef struct _AMBA_NVD_STAT_s_ {
    UINT32 MetaNumSecs;         /**< sector number of meta */
    UINT32 MaxObjs;             /**< max objects can be stored */
    UINT32 NumObjs;             /**< number of objects stored */
    UINT32 FreeSecs;            /**< free sector number of data */

    struct {
        UINT32 Size;            /**< data size of object */
        UINT32 StartSec;        /**< start sector of object */
        UINT32 NumSecs;         /**< sector numbers of object */
        UINT32 Crc;             /**< crc check sum */
    } Obj[NVD_MAX_OBJS];
} AMBA_NVD_STAT_s;

/*-----------------------------------------------------------------------------------------------*\
 * The sector linked list
\*-----------------------------------------------------------------------------------------------*/
typedef struct _NVD_SECTOR_LINK_s_ {
    struct _NVD_SECTOR_LINK_s_ *pNext;
    UINT32 SecNo;
} NVD_SECTOR_LINK_s;

#define NVD_SIZE_OF_SECTOR_LINK     sizeof(NVD_SECTOR_LINK_s)

__BEGIN_C_PROTO__

/*-----------------------------------------------------------------------------------------------*\
 * Initialize nvd mutex.
\*-----------------------------------------------------------------------------------------------*/
int AmbaNVD_InitLock(int NvdID);

/*-----------------------------------------------------------------------------------------------*\
 * De-initialize nvd mutex.
\*-----------------------------------------------------------------------------------------------*/
int AmbaNVD_DeinitLock(int NvdID);

/*-----------------------------------------------------------------------------------------------*\
 * Initialize nvd. Actually nvd stack initializes automatically when
 * nvd APIs first called.
\*-----------------------------------------------------------------------------------------------*/
int AmbaNVD_Init(int NvdID);

/*-----------------------------------------------------------------------------------------------*\
 * De-initialize nvd.
\*-----------------------------------------------------------------------------------------------*/
int AmbaNVD_Deinit(int NvdID);

/*-----------------------------------------------------------------------------------------------*\
 * Load nvd data from non-volatile memory.
\*-----------------------------------------------------------------------------------------------*/
int AmbaNVD_Load(int NvdID, int ObjID, void *pBuf, unsigned int Size);

/*-----------------------------------------------------------------------------------------------*\
 * Load nvd data from non-volatile memory.
\*-----------------------------------------------------------------------------------------------*/
int AmbaNVD_LoadToFile(int NvdID, int ObjID, unsigned int Size, AMBA_FS_FILE *pFile);

/*-----------------------------------------------------------------------------------------------*\
 * Save nvd data to non-volatile memory.
\*-----------------------------------------------------------------------------------------------*/
int AmbaNVD_Save(int NvdID, int ObjID, void *pBuf, unsigned int Size);

/*-----------------------------------------------------------------------------------------------*\
 * Erase content of an object.
\*-----------------------------------------------------------------------------------------------*/
int AmbaNVD_Erase(int NvdID, int ObjID);

/*-----------------------------------------------------------------------------------------------*\
 * Erase an entire nvd partition.
\*-----------------------------------------------------------------------------------------------*/
int AmbaNVD_EraseEntire(int NvdID);

/*-----------------------------------------------------------------------------------------------*\
 * Get nvd object data size.
\*-----------------------------------------------------------------------------------------------*/
int AmbaNVD_GetSize(int NvdID, int ObjID);

/*-----------------------------------------------------------------------------------------------*\
 * Get status of nvd.
\*-----------------------------------------------------------------------------------------------*/
void AmbaNVD_GetStat(int NvdID, AMBA_NVD_STAT_s *pStat);

__END_C_PROTO__

#endif  /* _AMBA_NVD_H_ */
