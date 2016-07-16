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
[pdm_api.h] - Declarations of Disk Manager API functions.
****************************************************************************/
#ifndef __PDM_API_H__
#define __PDM_API_H__

#include "prfile2/pdm_common.h"


/****************************************************************************
  Disk Manager API Functions Prototypes
****************************************************************************/
/*---------------------------------------------------------------------------
    pdm_init_diskmanager - Initialize pdm_set_disk struct.
---------------------------------------------------------------------------*/
PDM_ERROR pdm_init_diskmanager(PDM_U_LONG config, void* param);

/*---------------------------------------------------------------------------
    pdm_open_disk - Open disk.
---------------------------------------------------------------------------*/
PDM_ERROR pdm_open_disk(PDM_INIT_DISK *p_init_disk_tbl, PDM_DISK** pp_disk);

/*---------------------------------------------------------------------------
    pdm_close_disk - Close disk.
---------------------------------------------------------------------------*/
PDM_ERROR pdm_close_disk(PDM_DISK* p_disk);

/*---------------------------------------------------------------------------
    pdm_open_partition - Open partition.
---------------------------------------------------------------------------*/
PDM_ERROR pdm_open_partition(PDM_DISK* p_disk, PDM_S_LONG part_id, PDM_PARTITION** pp_part);

/*---------------------------------------------------------------------------
    pdm_close_partition - Close partition.
---------------------------------------------------------------------------*/
PDM_ERROR pdm_close_partition(PDM_PARTITION* p_part);

/*---------------------------------------------------------------------------
    pdm_register_disk_erase - Request and register a disk erase function.
---------------------------------------------------------------------------*/
PDM_ERROR pdm_register_disk_erase(PDM_DISK* p_disk, PDM_ERASE_FUNC p_func);


#endif  /* __PDM_API_H__ */
