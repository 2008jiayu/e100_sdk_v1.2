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
 [pf_drv.h] - Driver Layer Interface Functions
 
    Driver Layer Interface Functions is provided by Driver Layer Upper
    Sublayer to FATFS Layer.
  ****************************************************************************/
#ifndef __PF_DRV_H__
#define __PF_DRV_H__

#include "prfile2/pf_driver.h"
#include "pf_common.h"

/***********************************************************************
  Macro definitions
 ***********************************************************************/
/*---------------------------------------------------------------------*
  Error codes that can be returned by driver functions.
 *---------------------------------------------------------------------*/
#define DRV_SUCCEED             OK
#define DRV_FAILED              ERR

/***********************************************************************
  Type definitions
 ***********************************************************************/
/* The callback function to get a media event */
typedef void (*MEDIA_EVENT_CALLBACK)(PF_U_LONG event, void *p_param);

/* The media event */
#define MEDIA_EVENT_INSERT      (0x00000001ul)
#define MEDIA_EVENT_EJECT       (0x00000002ul)

/***********************************************************************
  Fixed values 
 ***********************************************************************/
#define BOOT_SECTOR_NUMBER      (0)
#define SECTERS_PER_BOOT_SECTOR (1)
#define BYTES_PER_BOOT_SECTOR   (512)

/***********************************************************************
  Driver Layer Interface Functions prototypes
 ***********************************************************************/
/*---------------------------------------------------------------------------
 PFDRV_GetFSINFOInformation - Get the FSINFO sector informations.
 ----------------------------------------------------------------------------*/
PF_ERROR PFDRV_GetFSINFOInformation(PF_VOLUME* p_vol);

/*---------------------------------------------------------------------------
  PFDRV_StoreVolumeLabelToBPB - Store Volume Lable into BPB on Boot Sector.
 ----------------------------------------------------------------------------*/
PF_ERROR  PFDRV_StoreVolumeLabelToBPB(PF_VOLUME* p_vol, const PF_U_CHAR* buf);

/*---------------------------------------------------------------------------
  PFDRV_StoreFreeCountToFSINFO - Store the count of free cluster into FSINFO.
 ----------------------------------------------------------------------------*/
PF_ERROR  PFDRV_StoreFreeCountToFSINFO(PF_VOLUME* p_vol);

/*---------------------------------------------------------------------------
  PFDRV_init - Initialize the volume.
l' structure for 'attach'.
  ---------------------------------------------------------------------------*/
PF_ERROR  PFDRV_init(PF_VOLUME* p_vol);

/*---------------------------------------------------------------------------
   PFDRV_finalize - Finalize the volume.
  ---------------------------------------------------------------------------*/
PF_ERROR  PFDRV_finalize(PF_VOLUME* p_vol);

/*---------------------------------------------------------------------------
  PFDRV_mount - Mount RAM drive.
  ---------------------------------------------------------------------------*/
PF_ERROR  PFDRV_mount(PF_VOLUME* p_vol);

/*---------------------------------------------------------------------------
   PFDRV_unmount - Unmount the volume.
  ---------------------------------------------------------------------------*/
PF_ERROR  PFDRV_unmount(PF_VOLUME* p_vol, PF_U_LONG mode);

/*---------------------------------------------------------------------------
  PFDRV_format - Format media on the specified drive.
  ---------------------------------------------------------------------------*/
PF_ERROR  PFDRV_format(PF_VOLUME* p_vol, const PF_U_CHAR* param);

/*---------------------------------------------------------------------------
  PFDRV_lread - Read logical sectors.
  ---------------------------------------------------------------------------*/
PF_ERROR  PFDRV_lread(PF_VOLUME*    p_vol,
                      PF_U_CHAR*    buf,
                      PF_U_LONG     sector,
                      PF_U_LONG     num_sectors,
                      PF_U_LONG*    p_num_success);

/*---------------------------------------------------------------------------
  PFDRV_lwrite - Write data onto the specified logical sectors
  ---------------------------------------------------------------------------*/
PF_ERROR  PFDRV_lwrite(PF_VOLUME*       p_vol,
                       const PF_U_CHAR* buf,
                       PF_U_LONG        sector,
                       PF_U_LONG        num_sectors,
                       PF_U_LONG*       p_num_success);

/*---------------------------------------------------------------------------
   PFDRV_lerase - Erase data onto the specified logical sectors
  ---------------------------------------------------------------------------*/
PF_ERROR  PFDRV_lerase(PF_VOLUME*   p_vol,
                       PF_U_LONG    sector,
                       PF_U_LONG    num_sectors);

/*---------------------------------------------------------------------------
 PFDRV_IsInserted - Check media inserted or not.
 ----------------------------------------------------------------------------*/
PF_BOOL  PFDRV_IsInserted(PF_VOLUME* p_vol);

/*---------------------------------------------------------------------------
 PFDRV_IsDetected - Check media insert status is changed.
 ----------------------------------------------------------------------------*/
PF_BOOL  PFDRV_IsDetected(PF_VOLUME* p_vol);

/*---------------------------------------------------------------------------
 PFDRV_IsWProtected - Check media write protected or not.
 ----------------------------------------------------------------------------*/
PF_BOOL  PFDRV_IsWProtected(PF_VOLUME* p_vol);

/*---------------------------------------------------------------------------
   PFDRV_set_event_callback - Set the callback function 
  ---------------------------------------------------------------------------*/
PF_ERROR  PFDRV_set_event_callback(PF_VOLUME*           p_vol,
                                   MEDIA_EVENT_CALLBACK event_callback,
                                   void*                p_param);

#endif /* __PF_DRV_H__ */
