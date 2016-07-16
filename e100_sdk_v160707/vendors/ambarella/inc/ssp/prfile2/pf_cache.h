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
 [pf_cache.h] -
 ****************************************************************************/
#ifndef __PF_CACHE_H__
#define __PF_CACHE_H__

#include "pf_common.h"

/****************************************************************************
  Macro definitions
 ****************************************************************************/
/* Initial buffer size */
#define INIT_BUF_SIZE                   (1)

/* buffer sie range */
#define MIN_FAT_BUF_SIZE                (1)
#define MAX_FAT_BUF_SIZE                (65535)
#define MIN_DATA_BUF_SIZE               (1)
#define MAX_DATA_BUF_SIZE               (32767)

/* Buffer alignment macro */
#define CACHE_BUF_ALGIN                 (0x3)

/* Minimum cache page number */
#define MIN_FAT_PAGES                   (1)
#if PF_EXFAT_SUPPORT
#define MIN_DATA_PAGES                  (4)
#else /* ! PF_EXFAT_SUPPORT */
#define MIN_DATA_PAGES                  (2)
#endif /* ! PF_EXFAT_SUPPORT */

/* Mode of cache controll structure (PF_SECTOR_CACHE) */
#define PF_CACHE_WRITE_THROUGH          (0x00000001ul)
#define PF_CACHE_AUTO_SYNC              (0x00000002ul)
#define PF_CACHE_FAT_AUTO_SYNC          (0x00000004ul)

/* Status of cache page structure (PF_CACHE_PAGE) */
#define PF_CACHE_USED                   (0x00000001ul)
#define PF_CACHE_MODIFIED               (0x00000002ul)
#define PF_CACHE_FAT                    (0x00000004ul)

#define IS_CACHE_MODIFIED(p_page)       (((p_page)->stat & PF_CACHE_MODIFIED) \
                                         == PF_CACHE_MODIFIED)


/****************************************************************************
  Type definitions
 ****************************************************************************/
/*---------------------------------------------------------------------------
  PF_SECTOR_CACHE
 ----------------------------------------------------------------------------*/
typedef struct PF_SECTOR_CACHE
{
    /* Cache controll mode */
    PF_U_LONG       mode;
    PF_U_SHORT      num_fat_pages;
    PF_U_SHORT      num_data_pages;
    PF_CACHE_PAGE*  pages;
    PF_CACHE_PAGE*  p_current_fat;
    PF_CACHE_PAGE*  p_current_data;
#if defined(__PRKERNEL_AMB__)
    PF_U_SHORT      count_flag;                /* 0 : fat, 1 : data */
    PF_U_LONG       fat_read_count;
    PF_U_LONG       fat_write_count;
    PF_U_LONG       data_read_count;
    PF_U_LONG       data_write_count;
#endif    
    PF_SECTOR_BUF*  buffers;
    PF_U_LONG       fat_buff_size;      /* number of sector (defaut is 0)   */
    PF_U_LONG       data_buff_size;     /* number of sector (defaut is 0)   */
    void*           signature;
} PF_SECTOR_CACHE;

#define GET_DATA_CACHE_BUFF_SIZE(p_vol)     (p_vol)->cache.data_buff_size
#define GET_FAT_CACHE_BUFF_SIZE(p_vol)      (p_vol)->cache.fat_buff_size

/****************************************************************************
  Prototypes
 ****************************************************************************/
/*---------------------------------------------------------------------------
  PFCACHE_SetCache - Set cache buffer and size.
 ----------------------------------------------------------------------------*/
void  PFCACHE_SetCache(PF_VOLUME*      p_vol,
                       PF_CACHE_PAGE*  p_cache_page,
                       PF_SECTOR_BUF*  p_cache_buf,
                       PF_U_SHORT      num_fat_pages,
                       PF_U_SHORT      num_data_pages);


/*---------------------------------------------------------------------------
  PFCACHE_SetFATBufferSize - Set the buffer size of FAT cache.
 ----------------------------------------------------------------------------*/
void  PFCACHE_SetFATBufferSize(PF_VOLUME* p_vol, PF_U_LONG size);

/*---------------------------------------------------------------------------
  PFCACHE_SetDataBufferSize - Set the buffer size of data cache.
 ----------------------------------------------------------------------------*/
void  PFCACHE_SetDataBufferSize(PF_VOLUME* p_vol, PF_U_LONG size);

/*---------------------------------------------------------------------------
  PFCACHE_InitCaches - Initialize caches for the volume.
 ----------------------------------------------------------------------------*/
PF_ERROR  PFCACHE_InitCaches(PF_VOLUME* p_vol);

/*---------------------------------------------------------------------------
  PFCACHE_AllocateDataPage - Allocate a cache page for a data sector.
 ----------------------------------------------------------------------------*/
PF_ERROR  PFCACHE_AllocateDataPage(PF_VOLUME*      p_vol,
                                   PF_U_LONG       sector,
                                   PF_CACHE_PAGE** pp_page);

/*---------------------------------------------------------------------------
  PFCACHE_FreeFATPage - Free the allocated cache page for FAT sector.
 ----------------------------------------------------------------------------*/
void  PFCACHE_FreeFATPage(PF_VOLUME*     p_vol,
                          PF_CACHE_PAGE* p_page);

/*---------------------------------------------------------------------------
  PFCACHE_FreeDataPage - Free the allocated cache page for Data sector.
 ----------------------------------------------------------------------------*/
void  PFCACHE_FreeDataPage(PF_VOLUME*     p_vol,
                           PF_CACHE_PAGE* p_page);

/*---------------------------------------------------------------------------
  PFCACHE_ReadFATPage - Read a cache page corresponding to the specified
                        FAT sector.
 ----------------------------------------------------------------------------*/
PF_ERROR  PFCACHE_ReadFATPage(PF_VOLUME*      p_vol,
                              PF_U_LONG       sector,
                              PF_CACHE_PAGE** pp_page);

/*---------------------------------------------------------------------------
  PFCACHE_ReadDataPage - Read a cache page corresponding to the specified
                         data sector (or root directory sector).
 ----------------------------------------------------------------------------*/
PF_ERROR  PFCACHE_ReadDataPage(PF_VOLUME*      p_vol,
                               PF_U_LONG       sector,
                               PF_CACHE_PAGE** pp_page,
                               PF_BOOL         set_sig);

/*---------------------------------------------------------------------------
  PFCACHE_WriteFATPage - Write a cache page corresponding to the FAT sector
                         specified by 'sector'.
 ----------------------------------------------------------------------------*/
PF_ERROR  PFCACHE_WriteFATPage(PF_VOLUME* p_vol, PF_CACHE_PAGE* p_page);

/*---------------------------------------------------------------------------
  PFCACHE_WriteDataPage - Write a cache page corresponding to the data
                          sector (or root directory sector) specified by
                          'sector'.
 ----------------------------------------------------------------------------*/
PF_ERROR  PFCACHE_WriteDataPage(PF_VOLUME* p_vol, PF_CACHE_PAGE* p_page, PF_BOOL set_sig);

/*---------------------------------------------------------------------------
  PFCACHE_ReadDataPageAndFlushIfNeeded - Read a cache page corresponding to
                                         the specified data sector
                                         (or root directory sector)
                                         and Write data into a data sector
                                         (or a root directory sector)
                                         without buffering.
 ----------------------------------------------------------------------------*/
PF_ERROR  PFCACHE_ReadDataPageAndFlushIfNeeded(PF_VOLUME*      p_vol,
                                               PF_U_LONG       sector,
                                               PF_CACHE_PAGE** pp_page,
                                               PF_BOOL         set_sig);

/*---------------------------------------------------------------------------
  PFCACHE_ReadDataPageExceptSectorAndFlushIfNeeded
 ----------------------------------------------------------------------------*/
PF_ERROR  PFCACHE_ReadDataPageExceptSectorAndFlushIfNeeded(PF_VOLUME*      p_vol,
                                                           PF_U_LONG       sector,
                                                           PF_CACHE_PAGE** pp_page,
                                                           PF_BOOL         set_sig);

/*---------------------------------------------------------------------------
  PFCACHE_WriteFATSectorAndFreeIfNeeded - Write data into a FAT sector without buffering.
 ----------------------------------------------------------------------------*/
PF_ERROR  PFCACHE_WriteFATSectorAndFreeIfNeeded(PF_VOLUME*       p_vol,
                                                const PF_U_CHAR* p_buf,
                                                PF_U_LONG        sector);

/*---------------------------------------------------------------------------
  PFCACHE_WriteDataSectorAndFreeIfNeeded - Write data into a data sector
                                           (or a root directory sector)
                                           without buffering.
 ----------------------------------------------------------------------------*/
PF_ERROR  PFCACHE_WriteDataSectorAndFreeIfNeeded(PF_VOLUME*       p_vol,
                                                 const PF_U_CHAR* p_buf,
                                                 PF_U_LONG        sector);

/*---------------------------------------------------------------------------
  PFCACHE_WriteDataNumSectorAndFreeIfNeeded - Write data into some data sector
                                              (or a root directory sector)
                                              without buffering.
 ----------------------------------------------------------------------------*/
PF_ERROR  PFCACHE_WriteDataNumSectorAndFreeIfNeeded(PF_VOLUME*       p_vol,
                                                    const PF_U_CHAR* p_buf,
                                                    PF_U_LONG        sector,
                                                    PF_U_LONG        num_sector,
                                                    PF_U_LONG*       p_num_success);

/*---------------------------------------------------------------------------
  PFCACHE_ReadDataNumSector - Read data from some data sector (or a root
                              directory sector) without buffering.
 ----------------------------------------------------------------------------*/
PF_ERROR  PFCACHE_ReadDataNumSector(PF_VOLUME*          p_vol,
                                    PF_U_CHAR*          p_buf,
                                    PF_U_LONG           sector,
                                    PF_U_LONG           num_sector,
                                    PF_U_LONG*          p_num_success);

/*---------------------------------------------------------------------------
  PFCACHE_SearchDataCache - Search data into a data sector (or a root
                            directory sector) without buffering.
 ----------------------------------------------------------------------------*/
PF_CACHE_PAGE*  PFCACHE_SearchDataCache(PF_VOLUME*       p_vol,
                                        PF_U_LONG        sector);

/*---------------------------------------------------------------------------
  PFCACHE_FlushDataCacheSpecific - Flush data cache for specific signature
 ----------------------------------------------------------------------------*/
PF_ERROR  PFCACHE_FlushDataCacheSpecific(PF_VOLUME* p_vol, void* signature);

/*---------------------------------------------------------------------------
  PFCACHE_FlushFATCache - Flush FAT cache
 ----------------------------------------------------------------------------*/
PF_ERROR  PFCACHE_FlushFATCache(PF_VOLUME* p_vol);

/*---------------------------------------------------------------------------
  PFCACHE_FlushDataCache - Flush data cache
 ----------------------------------------------------------------------------*/
PF_ERROR  PFCACHE_FlushDataCache(PF_VOLUME* p_vol);

/*---------------------------------------------------------------------------
  PFCACHE_FlushAllCaches - Flush all caches
 ----------------------------------------------------------------------------*/
PF_ERROR  PFCACHE_FlushAllCaches(PF_VOLUME* p_vol);

/*---------------------------------------------------------------------------
  PFCACHE_FreeAllCaches - Free all caches
 ----------------------------------------------------------------------------*/
void  PFCACHE_FreeAllCaches(PF_VOLUME* p_vol);

/*---------------------------------------------------------------------------
  PFCACHE_SetWriteThroughMode -
 ----------------------------------------------------------------------------*/
PF_ERROR  PFCACHE_SetWriteThroughMode(PF_VOLUME* p_vol);

/*---------------------------------------------------------------------------
  PFCACHE_SetWriteBackMode -
 ----------------------------------------------------------------------------*/
PF_ERROR  PFCACHE_SetWriteBackMode(PF_VOLUME* p_vol);

/*---------------------------------------------------------------------------
  PFCACHE_UpdateModifiedSector -
 ----------------------------------------------------------------------------*/
PF_ERROR  PFCACHE_UpdateModifiedSector(PF_VOLUME        *p_vol,
                                       PF_CACHE_PAGE*   p_page,
                                       PF_U_LONG        num_sector);


#endif  /* __PF_CACHE_H__ */
