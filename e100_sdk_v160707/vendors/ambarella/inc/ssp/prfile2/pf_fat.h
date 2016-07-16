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
 [pf_fat.h] - 
 ****************************************************************************/
#ifndef __PF_FAT_H__
#define __PF_FAT_H__

#include "prfile2/pdm_common.h"
#include "prfile2/pfm_module.h"
#include "pf_common.h"

/****************************************************************************
  Type Definitions
 ****************************************************************************/
/*---------------------------------------------------------------------------
  Last Access Cluster Number and Index by Reading and Writing operation
  * Holds information about the position of the last access cluster number
  * and index of the cluster in chain.
  * 
  * Each PF_FILE (=UFD: user file descriptor) holds its own variables for a
  * structure of this type describing the last access to the file made
  * through it. The goal is to avoid going through the file's whole FAT
  * chain each time an access is made to it's contents.
  *
  * To be used in the FAT/CLUSTER modules' implementation only.
  *
  * Note: This members is changed at Read / Write functions only.
 ----------------------------------------------------------------------------*/
typedef struct PF_CLST_IDX_TAG  PF_LAST_ACCESS;

/*---------------------------------------------------------------------------
  Last Allocated Cluster Number and Index
  * Holds information about the "logical" cluster number and the index of EOC.
  *
  * Each PF_FILE (=UFD: user file descriptor) holds its own structure when it
  * reaches the EOC or appends the new clusters.
  * The goal is to avoid going through the file's whole FAT chain each time
  * an access is made to it's contents.
 ----------------------------------------------------------------------------*/
typedef struct PF_CLST_IDX_TAG  PF_LAST_ALLOC;

/*
 * Cluster Link, Cluster Link 2
 */
typedef PF_U_LONG  PF_CL_IS_USE;

#define PF_CL_NOTUSE                (0x00000001ul)
#define PF_CL_USE_FILE              (0x00000002ul)
#define PF_CL_USE_DIR               (0x00000004ul)
#define PF_CL_USE_MASK              (PF_CL_NOTUSE | PF_CL_USE_FILE | PF_CL_USE_DIR)


#if PF_USE_CLSTLINK
/*---------------------------------------------------------------------------
  Cluster Link Manager for volume structure
 ----------------------------------------------------------------------------*/
typedef struct PF_CLUSTER_LINK_VOL
{
    /* Cluster Link Flag */
    PF_U_SHORT      flag;

    /* Cluster Link Interval */
    PF_U_SHORT      interval;

    /* Cluster Link Buffer */
    PF_U_LONG*      buffer;

    /* Cluster Link Max */
    PF_U_LONG       link_max;

} PF_CLUSTER_LINK_VOL;

/*---------------------------------------------------------------------------
  Cluster Link Manager
 ----------------------------------------------------------------------------*/
typedef struct PF_CLUSTER_LINK
{
    /* buffer address pointer */
    PF_CLUSTER_BUF  *buffer;

    /* cluster link interval */
    /* - 0 origin */
    PF_U_SHORT      interval;

    /* recording position */
    /* - 0 origin 
       - CL_POSISION_VOID is invalid value
     */
    PF_U_LONG       position;

    /* allow recording count */
    PF_U_LONG       max_count;

} PF_CLUSTER_LINK;
#endif /* PF_USE_CLSTLINK*/


/*---------------------------------------------------------------------------
  FAT file descriptor
 ----------------------------------------------------------------------------*/
struct PF_FFD
{
    /* Pointer to the variable holding the start cluster of the file */
    PF_U_LONG       start_cluster;

    /* Pointer to the variable holding the start cluster of the file */
    PF_U_LONG       *p_start_cluster;

    /* Cluster information (The start cluster number of the last allocated free clusters area) */
    /* When '.cluster' member is 0, this information is invalid */
    PF_LAST_ALLOC   last_alloc_start;

    /* Cluster information (EOC or last allocated cluster) */
    PF_LAST_ALLOC   last_alloc_end;

    /* Last access cluster to be changed at Read / Write functions only. */
    PF_LAST_ACCESS  last_access;
    
#if PF_USE_CLSTLINK
    /* Cluster Link Manager */
    PF_CLUSTER_LINK cl;
#endif /* PF_USE_CLSTLINK*/

    /* Pointer to fat hint structure from PF_FILE */
    PF_FAT_HINT     *p_hint;

    /* Pointer to the volume on which the file is located */
    PF_VOLUME       *p_vol;

#if PF_EXFAT_SUPPORT
    /* GeneralSecondaryFlags */
    PF_U_CHAR       general_flg;

    /* Pointer to the GeneralSecondaryFlags */
    PF_U_CHAR       *p_general_flg;

    /* Data Size */
    PF_FSIZE_T      size;

    /* Pointer to the Data Size */
    PF_FSIZE_T      *p_size;
#endif /* PF_EXFAT_SUPPORT */

};

/****************************************************************************
  Macro Definitions
 ****************************************************************************/
/*---------------------------------------------------------------------------
  Special cluster numbers
 ----------------------------------------------------------------------------*/
#define CLUSTER_FREE                ((PF_CLST)0)              /* free cluster     */
#define CLUSTER_ROOT                ((PF_CLST)1)              /* root directory   */
#define CLUSTER_VOID                ((PF_CLST)0xFFFFFFFFuL)   /* void value       */
#define CLUSTER_NOT_ALLOC           ((PF_CLST)0)              /* file does not have a cluster */
#define CLUSTER_START               (MIN_VALID_CLUSTER)       /* start cluster    */

#define CHAIN_INDEX_VOID            ((PF_CLST)0xFFFFFFFFul)

/*
 * Cluster allocate request flags
 *   for search_type of PFFAT_SearchFreeClusters()
 * (*) same PFM_FCS_REQOPT_STYPE_xxx
 */
#define CLUSTER_SEARCH_TYPE_FIRST           (1)
#define CLUSTER_SEARCH_TYPE_BEST            (2)
#define CLUSTER_SEARCH_TYPE_GROUP_FIRST     (3)
#define CLUSTER_SEARCH_TYPE_GROUP_BEST      (4)

#define IS_CLUSTER_SEARCH_TYPE_VALID(type)  ((type == CLUSTER_SEARCH_TYPE_FIRST)            \
                                             || (type == CLUSTER_SEARCH_TYPE_BEST)          \
                                             || (type == CLUSTER_SEARCH_TYPE_GROUP_FIRST)   \
                                             || (type == CLUSTER_SEARCH_TYPE_GROUP_BEST))

#define IS_CLUSTER_SEARCH_TYPE_FIRST(type)  ((type == CLUSTER_SEARCH_TYPE_FIRST)            \
                                             || (type == CLUSTER_SEARCH_TYPE_GROUP_FIRST))

#define IS_CLUSTER_SEARCH_TYPE_BEST(type)   ((type == CLUSTER_SEARCH_TYPE_BEST)             \
                                             || (type == CLUSTER_SEARCH_TYPE_GROUP_BEST))

#define IS_CLUSTER_SEARCH_TYPE_GROUP(type)  ((type == CLUSTER_SEARCH_TYPE_GROUP_FIRST)      \
                                             || (type == CLUSTER_SEARCH_TYPE_GROUP_BEST))

/*
 * Cluster Link
 */
#if PF_USE_CLSTLINK
  #define CL_POSITION_VOID                  (0xFFFFFFFFul)
                                             
  #define CL_BUFF(p_ffd)                    ((p_ffd)->cl.buffer)
  #define CL_INTERVAL(p_ffd)                ((p_ffd)->cl.interval)
  #define CL_POS(p_ffd)                     ((p_ffd)->cl.position)
  #define CL_MAX(p_ffd)                     ((p_ffd)->cl.max_count)
  #define CL_POSIDX(p_ffd)                  (CL_POS(p_ffd) * (CL_INTERVAL(p_ffd) + 1))
                                                      
  #define DISABLE_CL(p_ffd)                 (CL_BUFF(p_ffd) = NULL)
                                                           
  #define IS_VALID_CL(p_ffd)                (CL_BUFF(p_ffd) != NULL)
                                                           
  #define IS_WRITABLE_CL(p_ffd)             ((CL_POS(p_ffd) == CHAIN_INDEX_VOID)    \
                                             || (CL_POS(p_ffd) < (CL_MAX(p_ffd) - 1)))

  #define IS_WRITABLE_CL_POS(p_ffd, pos)    ((pos) < CL_MAX(p_ffd))

  #define CL_VALUE(p_ffd, pos)              (CL_BUFF(p_ffd)[pos])
#endif /* PF_USE_CLSTLINK*/

/* Does 'cluster' indicate the root directory? */
#define IS_ROOT_DIR_CLUSTER(p_vol,cluster)                  \
            (((cluster) == CLUSTER_ROOT)                    \
             || ((IS_FAT32(p_vol) || IS_EXFAT(p_vol))       \
                 && ((cluster) == ROOT_DIR_CLUSTER(p_vol))))

#if PF_EXFAT_SUPPORT
#define MAX_FREE_FILE_SIZE          MAX_FILE_SIZE_EXFAT
#else /* PF_EXFAT_SUPPORT */
#define MAX_FREE_FILE_SIZE          MAX_FILE_SIZE_FAT
#endif /* PF_EXFAT_SUPPORT */
#define MAX_FREE_DIR_SIZE           MAX_FREE_FILE_SIZE

/*---------------------------------------------------------------------------
  GET_FAT_OFFSET12 - Get the byte offset of the 'i'-th FAT12 entry, relative
                     to the start of the FAT structure.

    offset = i * 12 / 8
           = i * 1.5
           = i + i / 2
           = i + (i >> 1)
 ----------------------------------------------------------------------------*/
#define GET_FAT_OFFSET12(i)         ((PF_U_SHORT)(i + ((i) >> 1)))

/*---------------------------------------------------------------------------
  GET_FAT_OFFSET16 - Get the byte offset of the 'i'-th FAT16 entry, relative
                     to the start of the FAT structure.

    index = i * 16 / 8
          = i * 2
          = i << 1
 ----------------------------------------------------------------------------*/
#define GET_FAT_OFFSET16(i)         (((i) << 1))

/*---------------------------------------------------------------------------
  GET_FAT_OFFSET32 - Get the byte offset of the 'i'-th FAT32 entry, relative
                     to the start of the FAT structure.

    index = i * 32 / 8
          = i * 4
          = i << 2
 ----------------------------------------------------------------------------*/
#define GET_FAT_OFFSET32(i)         ((PF_U_LONG)((i) << 2))

/*---------------------------------------------------------------------------
  GET_FAT_SECTOR - Get the sector number of the FAT sector which contains the
                   'index'-th byte from the start of the FAT.

    sector = first_sector_of_active_FAT + fat_offset / bytes_per_sector
           = first_sector_of_active_FAT + (fat_offset >> log2(bytes_per_sector))
 ----------------------------------------------------------------------------*/
#define GET_FAT_SECTOR(p_vol,fat_offset) \
            ((ACTIVE_FAT_SECTOR(p_vol) + BYTES_TO_SECTOR_IDX(p_vol,(fat_offset))))

/*---------------------------------------------------------------------------
  GET_OFFSET_IN_SECTOR - Get the offset of the FAT entry relative to the
                         first byte of the sector.

    offset = fat_offset % bytes_per_sector
           = fat_offset & (bytes_per_sector - 1)
 ----------------------------------------------------------------------------*/
#define GET_OFFSET_IN_SECTOR(p_vol,fat_offset) \
            ((PF_U_SHORT)BYTES_TO_OFFSET_IN_SECTOR(p_vol,fat_offset))

/****************************************************************************
  Prototypes
 ****************************************************************************/
/*---------------------------------------------------------------------------
  PFFAT_ReadFATSector - Read the FAT Sector into the 'nEntry'-th FAT entry.
 ----------------------------------------------------------------------------*/
PF_ERROR PFFAT_ReadFATSector(PF_VOLUME *p_vol, PF_CACHE_PAGE **pp_page, PF_CLST clst);

/*---------------------------------------------------------------------------
  PFFAT_ReadFATEntryPage - Get the contents of the 'cluster'-th FAT entry.
 ----------------------------------------------------------------------------*/
PF_ERROR PFFAT_ReadFATEntryPage(PF_VOLUME     *p_vol,
                                PF_CLST       clst,
                                PF_CLST       *p_value,
                                PF_CACHE_PAGE **pp_page);

/*---------------------------------------------------------------------------
 PFFAT_UpdateFATEntry - Flush FAT entry.
 ----------------------------------------------------------------------------*/
PF_ERROR PFFAT_UpdateFATEntry(PF_VOLUME *p_vol, PF_CACHE_PAGE *p_page);

/*---------------------------------------------------------------------------
 PFFAT_UpdateAlternateFATEntry - Flush alternate FAT entry.
 ----------------------------------------------------------------------------*/
PF_ERROR PFFAT_UpdateAlternateFATEntry(PF_VOLUME *p_vol,
                                       PF_U_CHAR *p_buf,
                                       PF_SECT   sect,
                                       PF_SECT   num_sect);

/*---------------------------------------------------------------------------
 PFFAT_InitClstIdx
 ----------------------------------------------------------------------------*/
void  PFFAT_InitClstIdx(PF_CLST_IDX *p_ci);

/*---------------------------------------------------------------------------
 PFFAT_InitClstIdx
 ----------------------------------------------------------------------------*/
void  PFFAT_InitClstIdxIfNeeded(PF_CLST_IDX *p_ci, PF_CLST max_chain_idx);

/*---------------------------------------------------------------------------
  PFFAT_GetClusterSpecified - Convert chain index (file cluster index) into
 ----------------------------------------------------------------------------*/
PF_ERROR  PFFAT_GetClusterSpecified(PF_FFD  *p_ffd,
                                    PF_CLST chain_idx,
                                    PF_BOOL may_allocate,
                                    PF_CLST *p_clst);

/*---------------------------------------------------------------------------
  PFFAT_GetSectorSpecified - Convert file sector index into sector number.
 ----------------------------------------------------------------------------*/
PF_ERROR  PFFAT_GetSectorSpecified(PF_FFD  *p_ffd,
                                   PF_SECT file_sect_idx,
                                   PF_BOOL may_allocate,
                                   PF_SECT *p_sect);

/*---------------------------------------------------------------------------
  PFFAT_GetSectorAllocated - Convert file sector index into sector number.
 ----------------------------------------------------------------------------*/
PF_ERROR  PFFAT_GetSectorAllocated(PF_FFD     *p_ffd,
                                   PF_SECT    file_sect_idx,
                                   PF_FSIZE_T size,
                                   PF_U_LONG  search_type,
                                   PF_SECT    *p_sect,
                                   PF_SECT    *p_num_sect);

/*---------------------------------------------------------------------------
  PFFAT_GetClusterIndex - Get the index of the specified cluster in chain.
 ----------------------------------------------------------------------------*/
PF_ERROR  PFFAT_GetClusterIndex(PF_FFD  *p_ffd,       /* in/out */
                                PF_CLST target_clst,  /* in     */
                                PF_CLST *p_chain_idx);/* out    */

/*---------------------------------------------------------------------------
  PFFAT_GetContinuousSector - Get the number of the continuous sectors.
 ----------------------------------------------------------------------------*/
PF_ERROR  PFFAT_GetContinuousSector(PF_FFD     *p_ffd,
                                    PF_SECT    file_sect_idx,
                                    PF_FSIZE_T size,
                                    PF_SECT    *p_sect,
                                    PF_SECT    *p_num_sect);

/*---------------------------------------------------------------------------
  PFFAT_CountAllocatedClusters -
 ----------------------------------------------------------------------------*/
PF_ERROR  PFFAT_CountAllocatedClusters(PF_FFD     *p_ffd,
                                       PF_FSIZE_T cnt_byte,
                                       PF_CLST    *p_num_alloc_clst,
                                       PF_CLST    *p_end_clst,
                                       PF_CLST    *p_next_of_end_clst);

/*---------------------------------------------------------------------------
  PFFAT_CountFreeClusters -
 ----------------------------------------------------------------------------*/
PF_ERROR  PFFAT_CountFreeClusters(PF_VOLUME *p_vol, 
                                  PF_CLST   *p_num_free_clst,
                                  PF_CLST   *p_num_free_cgroup);

/*---------------------------------------------------------------------------
  PFFAT_FreeChain - Free the cluster chain starting at 'start_cluster'
                    (including 'start_cluster').
 ----------------------------------------------------------------------------*/
PF_ERROR  PFFAT_FreeChain(PF_FFD     *p_ffd,
                          PF_CLST    start_clst,
                          PF_CLST    chain_idx,
                          PF_FSIZE_T free_byte);

/*---------------------------------------------------------------------------
  PFFAT_GetPreviousCluster - Get previous cluster number.
 ----------------------------------------------------------------------------*/
PF_ERROR  PFFAT_GetPreviousCluster(PF_FFD  *p_ffd,
                                   PF_CLST cur_clst,
                                   PF_CLST *p_prev_clst);

/*---------------------------------------------------------------------------
  PFSEC_GetPreviousSector
 ----------------------------------------------------------------------------*/
PF_ERROR  PFFAT_GetPreviousSector(PF_FFD  *p_ffd,
                                  PF_SECT cur_sect,
                                  PF_SECT *p_prev_sect);

/*---------------------------------------------------------------------------
  PFFAT_InitFATRegion - Initialize FAT Region.
 ----------------------------------------------------------------------------*/
PF_ERROR  PFFAT_InitFATRegion(PF_VOLUME *p_vol);

/*---------------------------------------------------------------------------
  PFFAT_MakeRootDir - Makes the root directory of the FAT32 volume.
 ----------------------------------------------------------------------------*/
PF_ERROR  PFFAT_MakeRootDir(PF_VOLUME *p_vol);

/*---------------------------------------------------------------------------
  PFFAT_RefreshFSINFO - Refresh FSINFO sector.
 ----------------------------------------------------------------------------*/
PF_ERROR  PFFAT_RefreshFSINFO(PF_VOLUME *p_vol);

/*---------------------------------------------------------------------------
  PFFAT_WriteFATEntry - Write 'value' into the 'nEntry'-th FAT entry.
 ----------------------------------------------------------------------------*/
PF_ERROR  PFFAT_WriteFATEntry(PF_VOLUME *p_vol, PF_CLST clst, PF_CLST value);

/*---------------------------------------------------------------------------
  PFFAT_ReadFATEntry - Get the contents of the 'clst'-th FAT entry.
 ----------------------------------------------------------------------------*/
PF_ERROR  PFFAT_ReadFATEntry(PF_VOLUME *p_vol, PF_CLST clst, PF_CLST *p_value);

/*---------------------------------------------------------------------------
  PFFAT_AllocateNumClusters - Allocates clusters chain.
 ----------------------------------------------------------------------------*/
PF_ERROR  PFFAT_AllocateNumClusters(PF_VOLUME    *p_vol,
                                    PF_CLST      num_cluster,
                                    PF_CLST      *p_start_allocated,
                                    PF_CLST      *p_last_allocated,
                                    PF_CLST      *p_num_allocate_clst,
                                    PF_CL_IS_USE is_use_clstlink);

/*---------------------------------------------------------------------------
 PFFAT_InitHint - initialize a PF_FAT_HINT structure.
 ----------------------------------------------------------------------------*/
void  PFFAT_InitHint(PF_FAT_HINT* p_hint);

/*---------------------------------------------------------------------------
 PFFAT_SetHint - Set PF_FAT_HINT structure.
 ----------------------------------------------------------------------------*/
void  PFFAT_SetHint(PF_FFD* p_ffd, PF_FAT_HINT* p_hint);

/*---------------------------------------------------------------------------
  PFFAT_ResetFFD - Reset the 'PF_FFD' structure.
 ----------------------------------------------------------------------------*/
PF_ERROR  PFFAT_ResetFFD(PF_FFD       *p_ffd, 
                         PF_CLST      *p_start_cluster,
                         PF_U_CHAR    *p_general_flg,
                         PF_FSIZE_T   *p_size,
                         PF_CL_IS_USE is_use_clstlink);

/*---------------------------------------------------------------------------
  PFFAT_InitFFD - Initializes the 'PF_FFD' structure.
 ----------------------------------------------------------------------------*/
PF_ERROR  PFFAT_InitFFD(PF_FFD       *p_ffd,
                        PF_FAT_HINT  *p_hint,
                        PF_VOLUME    *p_vol,
                        PF_CLST      *p_start_cluster,
                        PF_U_CHAR    *p_general_flg,
                        PF_FSIZE_T   *p_size,
                        PF_CL_IS_USE is_use_clstlink);

/*---------------------------------------------------------------------------
  PFFAT_FinalizeFFD - Finalizes the 'PF_FFD' structure.
 ----------------------------------------------------------------------------*/
PF_ERROR  PFFAT_FinalizeFFD(PF_FFD *p_ffd);

/*---------------------------------------------------------------------------
  PFFAT_LogicalAllocateChain
 ----------------------------------------------------------------------------*/
PF_ERROR  PFFAT_LogicalAllocateChain(PF_FFD  *p_ffd, 
                                     PF_CLST chain_idx,
                                     PF_CLST start_clst,
                                     PF_CLST end_clst);

/*---------------------------------------------------------------------------
  PFFAT_LogicalFreeChain
 ----------------------------------------------------------------------------*/
PF_ERROR  PFFAT_LogicalFreeChain(PF_FFD  *p_ffd,
                                 PF_CLST clst,
                                 PF_CLST chain_idx);

/*---------------------------------------------------------------------------
 PFFAT_SetLastAccess - Set last access cluster information.
 ----------------------------------------------------------------------------*/
void  PFFAT_SetLastAccess(PF_FFD *p_ffd, PF_LAST_ACCESS *p_last_access);

/*---------------------------------------------------------------------------
  PFFAT_GetValueOfBAD - Get bad cluster value for FAT type.
 ----------------------------------------------------------------------------*/
PF_U_LONG  PFFAT_GetValueOfBAD(PF_VOLUME *p_vol);

/*---------------------------------------------------------------------------
  PFFAT_GetValueOfEOC1 - Get end of cluster value for FAT type.
 ----------------------------------------------------------------------------*/
PF_U_LONG  PFFAT_GetValueOfEOC1(PF_VOLUME *p_vol);

/*---------------------------------------------------------------------------
  PFFAT_GetValueOfEOC2 - Get end of cluster value for FAT type.
 ----------------------------------------------------------------------------*/
PF_U_LONG  PFFAT_GetValueOfEOC2(PF_VOLUME *p_vol);

#if PF_FATINFO_API_SUPPORT
/*---------------------------------------------------------------------------
  PFFAT_fat_getClst - get the value of the clusters of the specified area.
 ----------------------------------------------------------------------------*/
PFM_ERROR  PFFAT_fat_getClst(PFM_FATD   fatd, 
                             PFM_U_LONG area_start, 
                             PFM_U_LONG area_num, 
                             PFM_U_LONG *clst_array,
                             PFM_U_LONG *p_num_got);

/*---------------------------------------------------------------------------
  PFFAT_fat_searchFreeClst - search seriate free clusters area.
 ----------------------------------------------------------------------------*/
PF_ERROR  PFFAT_fat_searchFreeClst(PFM_FATD   fatd,
                                   PFM_U_LONG area_start, 
                                   PFM_U_LONG area_num, 
                                   PFM_U_LONG *p_free_start,
                                   PFM_U_LONG *p_num_free);
#endif /* PF_FATINFO_API_SUPPORT */

#endif  /* __PF_FAT_H__ */
