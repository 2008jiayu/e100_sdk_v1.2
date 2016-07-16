/*
    Copyright(C) 2005 eSOL Co., Ltd. All rights reserved.

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
[pdm_driver.h] - 
****************************************************************************/
#ifndef __PDM_DRIVER_H__
#define __PDM_DRIVER_H__

#include "prfile2/pdm_common.h"


/****************************************************************************
  Prototypes
****************************************************************************/
/*---------------------------------------------------------------------------
    pdm_disk_notify_media_eject - Notify that media eject is detected.

[Calling Procedure]
    void pdm_disk_notify_media_eject(PDM_DISK* p_disk);
        p_disk      A pointer of disk information struct.

[Explanation]
    Notify that media eject is detected.
    This function is to be called when media media is detected.
---------------------------------------------------------------------------*/
void pdm_disk_notify_media_eject(PDM_DISK* p_disk);


/*---------------------------------------------------------------------------
    pdm_disk_notify_media_insert - Notify that media insert is detected.

[Calling Procedure]
    void pdm_disk_notify_media_insert(PDM_DISK* p_disk);
        p_disk      A pointer of disk information struct.

[Explanation]
    Notify that media insert is detected.
    This function is to be called when media media is detected.
---------------------------------------------------------------------------*/
void pdm_disk_notify_media_insert(PDM_DISK* p_disk);


/*---------------------------------------------------------------------------
    pdm_disk_get_extended_info - Get extended information.

[Calling Procedure]
    PDM_U_LONG pdm_disk_get_extended_info(PDM_DISK* p_disk);
        p_disk      A pointer of disk information struct.

[Explanation]
    Get extended information of PDM_DISK_TBL.
---------------------------------------------------------------------------*/
PDM_U_LONG pdm_disk_get_extended_info(PDM_DISK* p_disk);


/*---------------------------------------------------------------------------
    pdm_disk_get_block_size - Get block size.

[Calling Procedure]
    PDM_U_LONG pdm_disk_get_extended_info(PDM_DISK* p_disk);
        p_disk      A pointer of disk information struct.

[Explanation]
    Get block size of DiskManager.
---------------------------------------------------------------------------*/
PDM_U_LONG pdm_disk_get_block_size(PDM_DISK* p_disk);


#endif /* __PDM_DRIVER_H__ */