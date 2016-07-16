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
 [pfm_module.h] - Definitions and Declarations commonly used in PrFILE2 Module.
 ****************************************************************************/
#ifndef __PFM_MODULE_H__
#define __PFM_MODULE_H__

#include "prfile2/pfm_api.h"


#if PF_FREE_CLUSTER_SEARCH_SUPPORT
/****************************************************************************
  Free Cluster Search 
 ****************************************************************************/
/*---------------------------------------------------------------------------
  Definitions
 ----------------------------------------------------------------------------*/
/* The error code */
#define PFM_ERR_OK_RESERVE      (1)     /* reserved for pf_ctrl_module */
#define PFM_ERR_OK              (0)     /* Success */
#define PFM_ERR_NG              (-1)    /* Error */

/* The mount attribute */
#define PFM_FCS_MOUNTED         (0x00000001)    /* mounted */
#define PFM_FCS_NOTSUPPORTED    (0x00000002)    /* not mounted because the media isn't supported */


/* The free cluster search descriptor */
struct PFM_FCSP_tag
{ 
    /* The FAT descriptor */
    PFM_FATD    fatd;

    /* The parameter which is set to pf_regfclst() */
    void        *p_param;
};
typedef struct PFM_FCSP_tag  PFM_FCSP; 


/* Cluster group */
struct PFM_FCS_CGROUP_tag
{
    PFM_U_LONG  clst_per_group;
    PFM_U_LONG  start_clst;
};
typedef struct PFM_FCS_CGROUP_tag   PFM_FCS_CGROUP;


/* The optional request */
struct PFM_FCS_REQOPT_tag
{
    /* Search type 
       - PFM_FCS_REQOPT_STYPE_xxx
     */
    PFM_U_LONG      search_type;

    /* Search start cluster number and limit number.
       - The FCS module must search the free cluster between 'area_start' 
         and 'area_end' ('area_end' is included in the area).
     */
    PFM_U_LONG      area_start;
    PFM_U_LONG      area_end;

    /* Cluster group information
       - Minimum allocated unit 
       - The module must search the free cluster of the specified unit.
       - This menber is available only when 'search_type' is PFM_FCS_REQOPT_STYPE_GROUP_xxx.
       - When this member is invalid, 'clst_group.clst_per_group' is 0.
     */
    PFM_FCS_CGROUP  clst_group;
};
typedef struct PFM_FCS_REQOPT_tag PFM_FCS_REQOPT;

#define PFM_FCS_REQOPT_STYPE_FIRST          (1)
#define PFM_FCS_REQOPT_STYPE_BEST           (2)
#define PFM_FCS_REQOPT_STYPE_GROUP_FIRST    (3)
#define PFM_FCS_REQOPT_STYPE_GROUP_BEST     (4)


/* The inferface of Free Cluster Search Module */
struct PFM_FCS_IF_tag
{ 
    /* Initialize the internal structures */
    PFM_ERROR   (*init) (PFM_FCSP *p_fcs); 

    /* Release the internal structures */
    PFM_ERROR   (*finalize) (PFM_FCSP *p_fcs); 

    /* Make the free clusters information */
    PFM_ERROR   (*mount) (PFM_FCSP   *p_fcs,
                          PFM_BOOL   is_formatted,
                          PFM_U_LONG *p_attr); 

    /* Release the free clusters information */
    PFM_ERROR   (*unmount) (PFM_FCSP *p_fcs); 

    /* Media eject happened */
    void        (*media_eject) (PFM_FCSP *p_fcs); 

    /* Initialize the free clusters information */
    PFM_ERROR   (*format) (PFM_FCSP *p_fcs); 

    /* Return the free clusters number */
    PFM_ERROR   (*notify_free_clst) (PFM_FCSP       *p_fcs, 
                                     PFM_U_LONG     req_cnt, 
                                     PFM_FCS_REQOPT *p_req_opt, 
                                     PFM_U_LONG     *p_area_start, 
                                     PFM_U_LONG     *p_area_num); 

    /* Set the free clusters information to 'allocated' */
    PFM_ERROR   (*alloc_clst) (PFM_FCSP   *p_fcs, 
                               PFM_U_LONG area_start, 
                               PFM_U_LONG area_num); 

    /* Set the free clusters information to 'free' */
    PFM_ERROR   (*release_clst) (PFM_FCSP   *p_fcs, 
                                 PFM_U_LONG area_start, 
                                 PFM_U_LONG area_num); 

    /* The extension function */
    PFM_S_LONG  (*control) (PFM_FCSP   *p_fcs, 
                            PFM_U_LONG req_code, 
                            PFM_U_LONG *p_param,
                            PFM_BOOL   is_blocking);
};
typedef struct PFM_FCS_IF_tag PFM_FCS_IF;

#endif /* PF_FREE_CLUSTER_SEARCH_SUPPORT */

#endif /* __PFM_MODULE_H__ */
