/*
     Copyright (C) 2008 By eSOL Co.,Ltd. Tokyo, Japan
 
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
 [pfm_apicmn.h] - Definitions and Declarations commonly used in PrFILE2 ExAPI.
 ****************************************************************************/
#ifndef __PFM_APICMN_H__
#define __PFM_APICMN_H__

#include "prfile2/pf_config.h"
#include "prfile2/pfm_types.h"

#if PF_EXAPI_SUPPORT

#if PF_FATINFO_API_SUPPORT
/****************************************************************************
  FAT Information API
 ****************************************************************************/
/*---------------------------------------------------------------------------
  Definitions
 ----------------------------------------------------------------------------*/
/* Handle */
typedef PFM_U_LONG              PFM_HANDLE;


/* The FAT descriptor */
typedef PFM_HANDLE              PFM_FATD;

#define PFM_FATD_VOID           (0)


/* BPB information */
struct PFM_FAT_BPB_tag
{
    PFM_U_LONG  total_sect;     /* Total sectors                        */
    PFM_U_LONG  total_clst;     /* Total clusters                       */
    PFM_U_LONG  sect_per_FAT;   /* Count of sectors occupied by one FAT */
    PFM_U_SHORT byte_per_sect;  /* Bytes per sector                     */
    PFM_U_CHAR  sect_per_clst;  /* Sectors per cluster                  */
    PFM_S_CHAR  fat_type;       /* FAT type                             */
    PFM_U_LONG  root_dir_clst;  /* Cluster number of root directory     */
};
typedef struct PFM_FAT_BPB_tag   PFM_FAT_BPB;


/* FAT type */
#define PFM_FAT_12              (0)
#define PFM_FAT_16              (1)
#define PFM_FAT_32              (2)

/* The cluster value */
#define PFM_CLUSTER_FREE        (0)
#define PFM_CLUSTER_BAD         (0xFFFFFFF7ul)
#define PFM_CLUSTER_EOC         (0xFFFFFFFFul)

/* The special cluster value */
#define PFM_CLUSTER_VOID        (0xFFFFFFFFul)
#define PFM_CLUSTER_NONEXISTENT (0xFFFFFFFEul)  /* There is not free cluster in the volume */
#define PFM_CLUSTER_UNKOWN      (0xFFFFFFFDul)  /* The FCS Module can not return free cluster(s).
                                                   because the module does not manage the area. */


/*---------------------------------------------------------------------------
 pfm_fat_getBPB
 ----------------------------------------------------------------------------*/
PFM_ERROR  pfm2_fat_getBPB(PFM_FATD    fatd, 
                           PFM_FAT_BPB *p_bpb);
#if PF_USE_API_STUB
PFM_ERROR  pfmstub_fat_getBPB(PFM_FATD    fatd,
                              PFM_FAT_BPB *p_bpb);
#endif /* PF_USE_API_STUB */

/*---------------------------------------------------------------------------
 pfm_fat_getClst
 ----------------------------------------------------------------------------*/
PFM_ERROR  pfm2_fat_getClst(PFM_FATD   fatd, 
                            PFM_U_LONG area_start, 
                            PFM_U_LONG area_num, 
                            PFM_U_LONG *clst_array,
                            PFM_U_LONG *p_num_got);
#if PF_USE_API_STUB
PFM_ERROR  pfmstub_fat_getClst(PFM_FATD   fatd, 
                               PFM_U_LONG area_start, 
                               PFM_U_LONG area_num, 
                               PFM_U_LONG *clst_array,
                               PFM_U_LONG *p_num_got);
#endif /* PF_USE_API_STUB */

/*---------------------------------------------------------------------------
 pfm_fat_searchFreeClst
 ----------------------------------------------------------------------------*/
PFM_ERROR  pfm2_fat_searchFreeClst(PFM_FATD   fatd,
                                   PFM_U_LONG area_start, 
                                   PFM_U_LONG area_num, 
                                   PFM_U_LONG *p_free_start,
                                   PFM_U_LONG *p_free_num);
#if PF_USE_API_STUB
PFM_ERROR  pfmstub_fat_searchFreeClst(PFM_FATD   fatd,
                                      PFM_U_LONG area_start, 
                                      PFM_U_LONG area_num, 
                                      PFM_U_LONG *p_free_start,
                                      PFM_U_LONG *p_free_num);
#endif /* PF_USE_API_STUB */

#endif /* PF_FATINFO_API_SUPPORT */

#endif /* PF_EXAPI_SUPPORT */

#endif /* __PFM_APICMN_H__ */
