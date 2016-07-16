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
 [pf_driver.h] - Service functions for Driver Layer Lower Sublayer.

    This functions are service functions for Driver Layer Lower Sublayer
    which is the drive-specific part of Driver Layer.


 ****************************************************************************/
#ifndef     __PF_DRIVER_H__
#define     __PF_DRIVER_H__

#include "prfile2/pf_apicmn.h"

/****************************************************************************
  Macro definitions
 ****************************************************************************/
/*---------------------------------------------------------------------------
  FAT access error macros
 ----------------------------------------------------------------------------*/
#define PF_DRV_RETRY            (0)     /* retry same FAT   */
#define PF_DRV_NEXT             (1)     /* retry next FAT   */
#define PF_DRV_ABORT            (2)     /* access abort     */

/****************************************************************************
  Type definitions
 ****************************************************************************/
/*---------------------------------------------------------------------------
  Call back for FAT access error
 ----------------------------------------------------------------------------*/
typedef PF_ERROR    (*PF_FAT_ERR)(PF_ERROR);

/*---------------------------------------------------------------------------
 PFDRV_SetDriverCallBack - Set call back for FAT read error.
 ----------------------------------------------------------------------------*/
void  PFDRV_SetDriverCallBack(PF_VOLUME* p_vol, void* function);


#endif /* __PF_DRIVER_H__ */
