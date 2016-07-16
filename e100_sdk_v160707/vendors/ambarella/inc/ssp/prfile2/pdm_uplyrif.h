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
[pdm_uplyrif.h] - Disk Manager API functions for upper layer.
****************************************************************************/
#ifndef __PDM_UPLYRIF_H__
#define __PDM_UPLYRIF_H__

#include "prfile2/pdm_common.h"


/****************************************************************************
  Partition Module Functions Prototypes
****************************************************************************/
/*---------------------------------------------------------------------------
    pdm_part_check_media_insert - Get media insert status.
---------------------------------------------------------------------------*/
PDM_ERROR pdm_part_check_media_insert(PDM_PARTITION* p_part, PDM_BOOL* is_insert);

/*---------------------------------------------------------------------------
    pdm_part_check_media_detect - Get media state information.
---------------------------------------------------------------------------*/
PDM_ERROR pdm_part_check_media_detect(PDM_PARTITION* p_part, PDM_BOOL* is_detect);

/*---------------------------------------------------------------------------
    pdm_part_check_media_write_protect - Get media write protect status.
---------------------------------------------------------------------------*/
PDM_ERROR pdm_part_check_media_write_protect(PDM_PARTITION* p_part, PDM_BOOL* is_wprotect);

/*---------------------------------------------------------------------------
    pdm_part_check_data_erase - Check the data erase request.
---------------------------------------------------------------------------*/
PDM_ERROR pdm_part_check_data_erase(PDM_PARTITION* p_part, PDM_BOOL* is_erase);

/*---------------------------------------------------------------------------
    pdm_part_get_driver_error_code - Get error code.
---------------------------------------------------------------------------*/
PDM_ERROR pdm_part_get_driver_error_code(PDM_PARTITION* p_part);

/*---------------------------------------------------------------------------
    pdm_part_get_media_information - Get media attribute.
---------------------------------------------------------------------------*/
PDM_ERROR pdm_part_get_media_information(PDM_PARTITION* p_part,
                                         PDM_DISK_INFO* p_disk_info);

/*---------------------------------------------------------------------------
    pdm_part_get_permission - Get access permission and lock partition.
---------------------------------------------------------------------------*/
PDM_ERROR pdm_part_get_permission(PDM_PARTITION* p_part);

/*---------------------------------------------------------------------------
    pdm_part_release_permission - Release access permission and unlock
                                  partition.
---------------------------------------------------------------------------*/
PDM_ERROR pdm_part_release_permission(PDM_PARTITION* p_part, PDM_U_LONG mode);

/*---------------------------------------------------------------------------
    pdm_part_format - Disk format.
---------------------------------------------------------------------------*/
PDM_ERROR pdm_part_format(PDM_PARTITION* p_part, const PDM_U_CHAR* param);

/*---------------------------------------------------------------------------
    pdm_part_logical_read - Read data.
---------------------------------------------------------------------------*/
PDM_ERROR pdm_part_logical_read(PDM_PARTITION*  p_part,
                                PDM_U_CHAR      *buf,
                                PDM_U_LONG      lsector,
                                PDM_U_LONG      num_sector,
                                PDM_U_SHORT     bps,
                                PDM_U_LONG      *p_num_success);

/*---------------------------------------------------------------------------
    pdm_part_logical_write - Write data.
---------------------------------------------------------------------------*/
PDM_ERROR pdm_part_logical_write(PDM_PARTITION*     p_part,
                                 const PDM_U_CHAR   *buf,
                                 PDM_U_LONG         lsector,
                                 PDM_U_LONG         num_sector,
                                 PDM_U_SHORT        bps,
                                 PDM_U_LONG         *p_num_success);

/*---------------------------------------------------------------------------
    pdm_part_logical_erase - Erase data.
---------------------------------------------------------------------------*/
PDM_ERROR pdm_part_logical_erase(PDM_PARTITION* p_part,
                                 PDM_U_LONG     lsector,
                                 PDM_U_LONG     num_sector,
                                 PDM_U_SHORT    bps);

/*---------------------------------------------------------------------------
    pdm_part_get_associated_disk_handle - Get associated disk handle of specified partition.
---------------------------------------------------------------------------*/
PDM_ERROR pdm_part_get_associated_disk_handle(PDM_PARTITION* p_part,
                                              PDM_DISK**     pp_disk);


/*---------------------------------------------------------------------------
    pdm_part_get_partition_information - Get partition table information.
---------------------------------------------------------------------------*/
PDM_ERROR pdm_part_get_partition_information(PDM_PARTITION* p_part,
                                             PDM_PART_INFO* p_part_info);

/*---------------------------------------------------------------------------
    pdm_part_notify_media_event - Getting the disk events
---------------------------------------------------------------------------*/
PDM_ERROR pdm_part_set_event_callback(PDM_PARTITION*                p_part,
                                      PDM_PARTITION_EVENT_CALLBACK  event_callback,
                                      void*                         p_param);

/*---------------------------------------------------------------------------
    pdm_part_get_physical_block - Get physical block information of the partition
---------------------------------------------------------------------------*/
PDM_ERROR pdm_part_get_physical_block(PDM_PARTITION *p_part,
                                      PDM_U_LONG    erase_block_size,
                                      PDM_U_LONG    *p_sectors_per_pblock,
                                      PDM_U_LONG    *p_start_psector,
                                      PDM_U_LONG    *p_bytes_per_psector);

/****************************************************************************
  Disk Module Functions Prototypes
****************************************************************************/
/*---------------------------------------------------------------------------
    pdm_disk_check_media_insert -  Get media insert status.
---------------------------------------------------------------------------*/
PDM_ERROR pdm_disk_check_media_insert(PDM_DISK* p_disk, PDM_BOOL* is_insert);

/*---------------------------------------------------------------------------
    pdm_disk_check_media_detect - Check media state.
---------------------------------------------------------------------------*/
PDM_ERROR pdm_disk_check_media_detect(PDM_DISK* p_disk, PDM_BOOL* is_detect);

/*---------------------------------------------------------------------------
    pdm_disk_check_media_write_protect - Get media write protect status.
---------------------------------------------------------------------------*/
PDM_ERROR pdm_disk_check_media_write_protect(PDM_DISK* p_disk, PDM_BOOL* is_wprotect);

/*---------------------------------------------------------------------------
    pdm_disk_check_data_erase - Check the data erase request.
---------------------------------------------------------------------------*/
PDM_ERROR pdm_disk_check_data_erase(PDM_DISK* p_disk, PDM_BOOL* is_erase);

/*---------------------------------------------------------------------------
    pdm_disk_check_partition_open - Check partition open status on disk.
---------------------------------------------------------------------------*/
PDM_ERROR pdm_disk_check_partition_open(PDM_DISK* p_disk, PDM_BOOL* is_open);

/*---------------------------------------------------------------------------
    pdm_disk_get_permission - Get access permision and lock disk.
---------------------------------------------------------------------------*/
PDM_ERROR pdm_disk_get_permission(PDM_DISK *p_disk);

/*---------------------------------------------------------------------------
    pdm_disk_release_permission - Release access permission and unlock disk.
---------------------------------------------------------------------------*/
PDM_ERROR pdm_disk_release_permission(PDM_DISK *p_disk, PDM_U_LONG mode);

/*---------------------------------------------------------------------------
    pdm_disk_format - Disk format.
---------------------------------------------------------------------------*/
PDM_ERROR pdm_disk_format(PDM_DISK* p_disk, const PDM_U_CHAR* param);

/*---------------------------------------------------------------------------
    pdm_disk_physical_read - Read data.
---------------------------------------------------------------------------*/
PDM_ERROR pdm_disk_physical_read(PDM_DISK*  p_disk,
                                 PDM_U_CHAR *buf,
                                 PDM_U_LONG block,
                                 PDM_U_LONG num_block,
                                 PDM_U_LONG *p_num_success);

/*---------------------------------------------------------------------------
    pdm_disk_physical_write - Write data.
---------------------------------------------------------------------------*/
PDM_ERROR pdm_disk_physical_write(PDM_DISK*         p_disk,
                                  const PDM_U_CHAR  *buf,
                                  PDM_U_LONG        psector,
                                  PDM_U_LONG        num_sector,
                                  PDM_U_LONG        *p_num_success);

/*---------------------------------------------------------------------------
    pdm_disk_physical_erase - Erase data.
---------------------------------------------------------------------------*/
PDM_ERROR pdm_disk_physical_erase(PDM_DISK*     p_disk,
                                  PDM_U_LONG    block,
                                  PDM_U_LONG    num_block);

/*---------------------------------------------------------------------------
    pdm_disk_convert_psector_to_block - Convert from physical(LBA) sector
                                        to block.
---------------------------------------------------------------------------*/
PDM_ERROR pdm_disk_convert_psector_to_block(PDM_DISK*   p_disk,
                                            PDM_U_LONG  psector,
                                            PDM_U_LONG  num_psector,
                                            PDM_U_LONG  *p_block,
                                            PDM_U_LONG  *p_num_block);

/*---------------------------------------------------------------------------
    pdm_disk_convert_block_to_psector - Convert from block to physical(LBA)
                                        sector.

---------------------------------------------------------------------------*/
PDM_ERROR pdm_disk_convert_block_to_psector(PDM_DISK*   p_disk,
                                            PDM_U_LONG  num_block,
                                            PDM_U_LONG  *p_num_psector);

/*---------------------------------------------------------------------------
    pdm_disk_get_media_information - Get media information.
---------------------------------------------------------------------------*/
PDM_ERROR pdm_disk_get_media_information(PDM_DISK*      p_disk,
                                         PDM_DISK_INFO* p_disk_info);


/****************************************************************************
  BPB Library Functions Prototypes
****************************************************************************/
/*---------------------------------------------------------------------------
 pdm_bpb_load_string - Load the string information.
 or otherwise.
 ----------------------------------------------------------------------------*/
PDM_ERROR pdm_bpb_load_string(const PDM_U_CHAR* buf,
                              PDM_U_LONG        length,
                              PDM_U_CHAR*       p_string);

/*---------------------------------------------------------------------------
 pdm_bpb_store_string - Store the string information.
 ----------------------------------------------------------------------------*/
PDM_ERROR pdm_bpb_store_string(const PDM_U_CHAR*    p_string,
                               PDM_U_LONG           length,
                               PDM_U_CHAR*          buf);

/*---------------------------------------------------------------------------
 pdm_bpb_get_bpb_information - Load the boot sector (including BPB)
                               information.
---------------------------------------------------------------------------*/
PDM_ERROR pdm_bpb_get_bpb_information(PDM_U_CHAR* buf, PDM_BPB* p_bpb);

/*---------------------------------------------------------------------------
 pdm_bpb_set_bpb_information - Store the boot sector (including BPB)
                               information.
---------------------------------------------------------------------------*/
PDM_ERROR pdm_bpb_set_bpb_information(PDM_BPB* p_bpb, PDM_U_CHAR* buf);

/*---------------------------------------------------------------------------
 pdm_bpb_get_fsinfo_information - Parse the FSINFO sector (including FSINFO)
                                  information.
 ----------------------------------------------------------------------------*/
PDM_ERROR pdm_bpb_get_fsinfo_information(PDM_U_CHAR* buf, PDM_FSINFO* p_fsinfo);

/*---------------------------------------------------------------------------
 pdm_bpb_set_fsinfo_information - Store the FSINFO sector information.
 ----------------------------------------------------------------------------*/
PDM_ERROR pdm_bpb_set_fsinfo_information(PDM_FSINFO* p_fsinfo, PDM_U_CHAR* buf);

/*---------------------------------------------------------------------------
    pdm_bpb_check_boot_sector - Check boot sector.
---------------------------------------------------------------------------*/
PDM_ERROR pdm_bpb_check_boot_sector(PDM_U_CHAR* buf, PDM_BOOL* is_boot);

/*---------------------------------------------------------------------------
    pdm_bpb_check_fsinfo_sector - Check FSINFO sector.
---------------------------------------------------------------------------*/
PDM_ERROR pdm_bpb_check_fsinfo_sector(PDM_U_CHAR* buf, PDM_BOOL* is_fsinfo);

/*---------------------------------------------------------------------------
    pdm_bpb_build_boot_sector - Build BPB informations.
---------------------------------------------------------------------------*/
PDM_ERROR pdm_bpb_build_boot_sector(PDM_BPB_FORMAT* p_format, PDM_BPB* p_bpb);


/****************************************************************************
  MBR Module Functions Prototypes
****************************************************************************/
/*---------------------------------------------------------------------------
    pdm_mbr_get_table - Get all partition table informations in buffer.
---------------------------------------------------------------------------*/
PDM_ERROR pdm_mbr_get_table(PDM_U_CHAR* buf, PDM_U_LONG sector, PDM_MBR* p_mbr_tbl);

/*---------------------------------------------------------------------------
    pdm_mbr_set_table - Set all partition table informations in buffer.
---------------------------------------------------------------------------*/
PDM_ERROR pdm_mbr_set_table(PDM_MBR* p_mbr_tbl, PDM_U_LONG sector, PDM_U_CHAR* buf);

/*---------------------------------------------------------------------------
    pdm_mbr_get_mbr_part_table - Get all partition table informations in MBR.
---------------------------------------------------------------------------*/
PDM_ERROR pdm_mbr_get_mbr_part_table(PDM_DISK* p_disk, PDM_MBR* p_mbr_tbl);

/*---------------------------------------------------------------------------
    pdm_mbr_get_epbr_part_table - Get all partition table informations in EPBR.
---------------------------------------------------------------------------*/
PDM_ERROR pdm_mbr_get_epbr_part_table(PDM_DISK*  p_disk, PDM_MBR* p_mbr_tbl);

/*---------------------------------------------------------------------------
    pdm_mbr_set_part_table - Set all partition table informations in MBR.
---------------------------------------------------------------------------*/
PDM_ERROR pdm_mbr_set_part_table(PDM_DISK* p_disk, PDM_MBR* p_mbr_tbl);

/*---------------------------------------------------------------------------
    pdm_mbr_build_master_boot_record - build or initialize MBR sector.
---------------------------------------------------------------------------*/
PDM_ERROR pdm_mbr_build_master_boot_record(PDM_DISK* p_disk, PDM_U_LONG sector);

/*---------------------------------------------------------------------------
    pdm_mbr_check_master_boot_record - Check master boot recorde.
---------------------------------------------------------------------------*/
PDM_ERROR pdm_mbr_check_master_boot_record(PDM_DISK*    p_disk,
                                           PDM_U_CHAR*  buf,
                                           PDM_BOOL*    is_master_boot);

#endif  /* __PDM_UPLYRIF_H__ */
