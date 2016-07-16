/**
 * @file system/include/peripheral/gps.h
 *
 * Header file of GPS device abstract layer.
 *
 * History:
 *    2009/12/17 - [Jack Huang] created filea
 *    2015/04/13 - [Bill Chou] porting to a12
 *
 * Usage:
 *  1. Get available device list :
 *      gps_get_dev_list();
 *
 *  2. Open/close a gps device :
 *      gps_open();
 *      gps_close();
 *
 *  3. Get gps data:
 *      a. Direct polling data (Non-blocking):
 *          gps_get_data()/gps_get_raw_data();
 *
 *      b. Regster/unregister call-back function,
 *         call-back function will be triggered when data is updated,
 *         and the paramter is the updated gps data
 *          gps_add_notify_func();
 *          gps_del_notify_func();
 *
 *  4. Device control:
 *      a. get device status:
 *          gps_get_status();
 *
 *      b. switch device mode between power-saving and normal-operating
 *          gps_suspend();
 *          gps_resume();
 *
 *      c. gps device data output rate control:
 *          gps_set_update_period();
 *
 * Copyright (C) 2009-2010, Ambarella, Inc.
 *
 * All rights reserved. No Part of this file may be reproduced, stored
 * in a retrieval system, or transmitted, in any form, or by any means,
 * electronic, mechanical, photocopying, recording, or otherwise,
 * without the prior consent of Ambarella, Inc.
 */
#ifndef __GPS_H__
#define __GPS_H__
#include <gps_struct.h>
#include <gps_parser.h>
#include <AmbaDataType.h>
#include <AmbaKAL.h>
#include <AmbaPrint.h>
#include <AmbaPrintk.h>
#include <AmbaUART.h>

__BEGIN_C_PROTO__

typedef int GPS_HANDLE;
typedef UINT32 u32;
typedef UINT16 u16;
typedef UINT8  u8;


//#define DEBUG_GPS
#if defined(DEBUG_GPS)
#define GPSDBGMSG AmbaPrint
#define GPSDBGMSGc(x) AmbaPrintColor(RED,x)
#else
#define GPSDBGMSG(...)
#define GPSDBGMSGc(...)
#endif

/*
 * This data structure contains basic gps device infomation.
 */
typedef struct gps_dev_list_s {
    char name[40];
    u32 i_type;         /* interface type */
#define GPS_UART        1
#define GPS_SPI         2
#define GPS_BLUETOOTH       3
    u32 update_period;  /* data update rate from gps device (ms)*/
    struct gps_dev_list_s *next;
} gps_dev_list_t;

/*
 * This data structure contains basic gps device infomation.
 */
typedef struct gps_status_s {
    int mode;       /* power status */
#define GPS_POWER_OFF       0
#define GPS_NORMAL_MODE     1
#define GPS_POWER_SAVING_MODE   2
    u32 update_period;  /* device data update period (ms)*/
} gps_status_t;

/**
 * Function proto-type for call-back when data input is updated
 * from the GPS device
 */
typedef void (*gps_update)(u8 *ptr, int len);

/**
 * Function proto-type for hookup function
 */
typedef GPS_HANDLE (*gps_drv_hookup)(void);

/**
 * Added a call-back function to notify application that data is updated.
 *
 * @param id - GPS id
 * @param fn - Call-back function which is provided by application
 * @return - 0 successfunl, < 0 Please check the error code
 */
extern int gps_add_notify_func(GPS_HANDLE id, gps_update fn);

/**
 * Delete the registered call-back function.
 *
 * @param id - GPS id
 * @return - 0 successfunl, < 0 Please check the error code
 */
extern int gps_del_notify_func(GPS_HANDLE id);

/**
 * This function is used to get the available device list from the gps
 * abstract layer.
 *
 * @return - Available device list
 */
extern const gps_dev_list_t* gps_get_dev_list(void);

/**
 * This function is used for runtime plugin gps device, the available device
 * list needs to be update.
 *
 * @return - Available device list
 */
extern int gps_update_dev_list(void);

/**
 * Get current gps device status
 *
 * @param id - GPS id
 * @return - structure of device status
 */
extern const gps_status_t* gps_get_status(GPS_HANDLE id);

/**
 * Open gps device and attached this driver on GPS abstract layer
 * Please use gps_get_dev_list to get the exactly device name
 *
 * @param dev_name - GPS device device name
 * @return - >= 0 gps handler, < 0 Please check the error code
 */
extern GPS_HANDLE gps_open(char * dev_name, int *gpsid);

/**
 * Close gps device and process gps power off sequence
 *
 * @param id - GPS id
 * @return - 0 successful, < 0 Please check the error code
 */
extern int gps_close(GPS_HANDLE id);

/**
 * Put gps device into power saving mode.
 *
 * @param id - GPS id
 * @return - 0 successful, < 0 Please check the error code
 */
extern int gps_suspend(GPS_HANDLE id);

/**
 * Configure gps device into normal operating mode.
 *
 * @param id - GPS id
 * @return - 0 successful, < 0 Please check the error code
 */
extern int gps_resume(GPS_HANDLE id);

/**
 * Set GPS device output sentence period.
 *
 * @param id - GPS id
 * @param period - gps data update period
 * @return - 0 successful, < 0 Please check the error code
 */
extern int gps_set_update_period(GPS_HANDLE id, u32 period);

/**
 * Get gps status and position. This api is non-blocking function.
 * GPS data will be the same if polling frequency is more than data update rate
 *
 * @param id - GPS id
 * @param gps_data - data structure contains position and gps status
 * @return - 0 successful, < 0 Please check the error code
 */
extern int gps_get_data(GPS_HANDLE id, gps_data_t *gps_data);

#ifdef CONFIG_ECL_RADAR_MODEL
extern int radar_get_obj_info(GPS_HANDLE id, K_RADAR_OBJ_INFO *radar_data);

extern int radar_get_status_info(GPS_HANDLE id, K_RADAR_STATUS_INFO *radar_data);
#endif

/**
 * Get raw string from gps device. This api is non-blocking function.
 * The gps raw string will be the same if polling frequency is more than
 * data update rate
 *
 * @param id - GPS id
 * @param data_buf - data buffer used to store raw gps string.
 * @return - number of raw data in byte
 */
extern int gps_get_raw_data(GPS_HANDLE id, char *data_buf);

/* GPS error code definition */
#define GPS_OK               0
#define GPS_DEV_NOT_FOUND       -1
#define GPS_DEV_NOT_OPENED      -2
#define GPS_DEV_POWER_DOWN      -3
#define GPS_DATA_NOT_UPDATED        -4
#define GPS_CHECKSUM_ERROR      -5
#define GPS_CONST_UPDATE_RATE       -6
#define GPS_CHANGE_UPDATE_PERIOD_FAIL   -7
#define GPS_NOTIFY_FUNC_OVERWRITE   -8
#define GPS_DELETE_NULL_NOTIFY_FUNC -9
#define GPS_LACK_OF_MEMORY      -10

__END_C_PROTO__

#endif /* __GPS_H__ */
