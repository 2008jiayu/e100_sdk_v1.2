/**
 * @file system/include/peripheral/gps_dev.h
 *
 * GPS device driver related data structure.
 *
 * History:
 *    2009/12/17 - [Jack Huang] created file
 *    2015/04/13 - [Bill Chou] porting to a12
 *
 * Copyright (C) 2009-2010, Ambarella, Inc.
 *
 * All rights reserved. No Part of this file may be reproduced, stored
 * in a retrieval system, or transmitted, in any form, or by any means,
 * electronic, mechanical, photocopying, recording, or otherwise,
 * without the prior consent of Ambarella, Inc.
 */
#ifndef __GPS_DEV_H__
#define __GPS_DEV_H__

#include <gps.h>
__BEGIN_C_PROTO__

/*
 * Data structure containing service functions that need to be provided to the
 * gps device manager by the device drivers.
 */
typedef struct gps_dev_t {
    char name[40];
    u32 i_type; /* interface type */
    GPS_HANDLE id;
    gps_status_t status;
    int (*init) ( struct gps_dev_t *dev);/**< Turn on the gps power */
    int (*power_on) ( struct gps_dev_t *dev);/**< Turn on the gps power */
    int (*suspend) ( struct gps_dev_t *dev);/**< Put device into power saving mode */
    int (*resume) ( struct gps_dev_t *dev);/**< Restore device into normal operating mode */
    int (*power_off) ( struct gps_dev_t *dev);/**< Turn off the gps power */
    int (*detect) ( struct gps_dev_t *dev);/**<  Detect gps device, device may not be connected to system */
    int (*set_update_period) ( struct gps_dev_t *dev, u32 period);/**< Change device data update rate */
    int (*get_raw_data) ( struct gps_dev_t *dev, char *raw_data);/**< Return the raw data of gps */
} gps_dev_s;

/**
 * Attach the GPS device driver on the gps abstract layer
 *
 * @param gps - gps device struct
 * @return - >= 0 GPS_HANDLE id  < 0 fail (Please check the error code)
 */
extern GPS_HANDLE gps_drv_attach(struct gps_dev_t *gps);

/**
 * Fully remove the GPS driver from the gps abstract layer
 *
 * @param id - registered gps handle
 * @return - 0 successful, < 0 Please check the error code
 */
extern int gps_drv_remove(GPS_HANDLE id);

/**
 * Drivers use this api to notify the abstract layer when data is updated
 *
 * @param id - registered gps handle
 * @return - 0 successful, < 0 Please check the error code
 */
extern int gps_drv_data_update(GPS_HANDLE id, UINT8 *ptr, int len);

#ifdef CONFIG_ECL_RADAR_MODEL
extern int radar_drv_obj_info_update(GPS_HANDLE id, UINT8 *ptr, int len);
extern int radar_drv_status_info_update(GPS_HANDLE id, UINT8 *ptr, int len);
#endif

/**
 * This function will detect each gps device and put the avaiable device
 * into gps_dev_list.
 */
extern int gps_drv_init(AMBA_KAL_BYTE_POOL_t *pBytePool);

/* GPS Driver error code definition*/
#define GPS_DEV_OK           0
#define GPS_DEV_ATTACH_FAIL     -1
#define GPS_DEV_REMOVE_FAIL     -2
#define GPS_DEV_CHANGE_UPDATE_RATE_FAIL -3
#define GPS_DEV_NO_CALL_BACK_FUNC   -4

__END_C_PROTO__
#endif /* __GPS_DEV_H__ */
