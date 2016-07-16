/**
 * @file system/src/peripheral/gps/gps_drv.c
 *
 * Initilzation of GPS device drivers
 *
 * History:
 *    2009/11/13 - [Jack Huang] created file
 *
 * Copyright (C) 2004-2007, Ambarella, Inc.
 *
 * All rights reserved. No Part of this file may be reproduced, stored
 * in a retrieval system, or transmitted, in any form, or by any means,
 * electronic, meinstical, photocopying, recording, or otherwise,
 * without the prior consent of Ambarella, Inc.
 */


#include <AmbaDataType.h>
#include <AmbaKAL.h>
#include <AmbaPrint.h>
#include <AmbaPrintk.h>
#include <string.h>

#include <gps.h>
#include <gps_dev.h>
#include <gps_dev_list.h>

#define MAX_GPS_INSTANCES   4
#define __GPS_DEV_IMPL__
struct gps_obj_s {
    gps_dev_list_t  *dev_list;
    gps_dev_s   dev[MAX_GPS_INSTANCES];
    gps_update  hdl[MAX_GPS_INSTANCES];
    UINT32     total_gps_dev;
    AMBA_KAL_BYTE_POOL_t* pBytePool;
};

struct gps_obj_s G_gps;
static gps_dev_list_t *va_gps_list, *va_gps_next;
struct gps_device_t gps_session;

#ifdef CONFIG_ECL_RADAR_MODEL
RADAR_DET_DATA_T radar_session;
#endif

gps_drv_hookup driver_list[] = {
//#if CONFIG_GPS_PROGIN_SR92
//    gps_drv_in/it_progin,
//#endif
#ifdef CONFIG_GPS_UBLOX6_GM6XX
    gps_drv_init_ublox,
#endif

#ifdef CONFIG_ECL_RADAR_MODEL
    radar_drv_init_ublox,
#endif

//#if CONFIG_GPS_GOTOP_GAM_2222_MTR
//    gps_drv_init_gotop,
//#endif
    NULL
};

AMBA_KAL_MUTEX_t GPS_Mutex;

#define GPS_LOC AmbaKAL_MutexTake(&GPS_Mutex, AMBA_KAL_WAIT_FOREVER);
#define GPS_UNL AmbaKAL_MutexGive(&GPS_Mutex);
/**
 * This function is used to get the available device list from the gps
 * abstract layer.
 */
const gps_dev_list_t* gps_get_dev_list(void)
{
    return G_gps.dev_list;
}

/**
 * This function is used for runtime plugin gps device, the available device
 * list needs to be update.
 */

int gps_update_dev_list(void)
{
    int Rval = 0;
    int i = 0;
    GPS_HANDLE id;
 //   ER ercd;
    gps_dev_s *curr;
    while (1) {
        if (driver_list[i] == NULL)
            break;
        id = driver_list[i]();
        //device is connected to system, added device in the list
        curr = &(G_gps.dev[id]);
        if (curr->detect(curr) != 0) {
            Rval = AmbaKAL_BytePoolAllocate( G_gps.pBytePool, (void **) &va_gps_list, sizeof(gps_dev_list_t), 100);
            if (Rval < 0)
            {
                AmbaPrint("gps got no mem!!");
                return GPS_LACK_OF_MEMORY;
            }
            memset(va_gps_list, 0, sizeof(gps_dev_list_t));
            memcpy(va_gps_list->name, curr->name, 40);
            va_gps_list->i_type = curr->i_type;
            va_gps_list->update_period = (curr->status).update_period;
            if (G_gps.dev_list == NULL) {
                G_gps.dev_list = va_gps_list;
            } else {
                while (1) {
                    va_gps_next = G_gps.dev_list->next;
                    if (va_gps_next == NULL) {
                        va_gps_next = va_gps_list;
                        break;
                    }
                }
            }
        }
        gps_drv_remove(id);
        i++;
    }
    return 0;
}

/**
 * Added a call-back function to notify application that data is updated.
 */
int gps_add_notify_func(GPS_HANDLE id, gps_update fn)
{
    int rval = GPS_NOTIFY_FUNC_OVERWRITE;

    if (G_gps.hdl[id] == 0)
        rval = 0;
    G_gps.hdl[id] = fn;

    return rval;
}

/**
 * Delete the registered call-back function.
 */
int gps_del_notify_func(GPS_HANDLE id)
{
    int rval = GPS_DELETE_NULL_NOTIFY_FUNC;

    if (G_gps.hdl[id] != 0)
        rval = 0;
    G_gps.hdl[id] = 0;

    return rval;
}

/**
 * Initilzation of default GPS device driver at the system bootup.
 */
int gps_drv_init(AMBA_KAL_BYTE_POOL_t *pBytePool)
{
    int Rval = 0;
    memset(&G_gps, 0x0, sizeof(struct gps_obj_s));

    // error hdlr
    if (pBytePool == NULL ) {
        AmbaPrintColor(RED, "[gps_drv_init] AMBA_KAL_BYTE_POOL_t == NULL !! \n");
        return -1;
    }

    G_gps.pBytePool = pBytePool;
    Rval = gps_update_dev_list();
    return Rval;
}

/**
 * Open gps device and attached this driver on GPS abstract layer
 * Please use gps_get_dev_list to get the exactly device name
 */
GPS_HANDLE gps_open(char * dev_name, int* gpsid)
{
    GPS_HANDLE id = -1;
    int Rval = 0;
    int len = 0, i = 0, rval = -1;
    struct gps_dev_t *curr;
    Rval = AmbaKAL_MutexCreate(&GPS_Mutex);
    if (Rval == OK) {
        GPSDBGMSGc( "[gps_open] Create Mutex success");
    } else {
        AmbaPrintColor(RED, "[gps_openT] Create Mutex fail = %d", Rval);
        return NG;
    }
    gpsd_init(&gps_session);
    len = strlen(dev_name);

    while (1) {
        if (driver_list[i] == NULL)
        {
            AmbaPrintColor(RED, "[gps_open] driver_list[i] == NULL !! \n");
            break;
        }
        id = driver_list[i]();

        //device is connected to system, added device in the list
        curr = &(G_gps.dev[id]);
        if (strncmp(dev_name, curr->name, len) == 0){
            break;
        }
        i++;
    }

    /* power-on device */
    if (G_gps.dev[id].power_on != NULL)
        rval = G_gps.dev[id].power_on(&(G_gps.dev[id]));
    else
        return -1;

    /* power-on device */
    if (G_gps.dev[id].init != NULL){
        int initrval = 0;
        initrval = G_gps.dev[id].init(&(G_gps.dev[id]));
        *gpsid = id;
        return initrval;
    }
    else{
        return -1;
    }
}

/**
 * Close gps device and process gps power off sequence
 */
int gps_close(GPS_HANDLE id)
{
    if (G_gps.dev[id].power_off != NULL) {
        G_gps.dev[id].power_off(&(G_gps.dev[id]));
        return gps_drv_remove(id);
    } else
        return GPS_DEV_NOT_FOUND;
}

/**
 * Change device output data period
 */
int gps_set_update_period(GPS_HANDLE id, u32 period)
{
    if (G_gps.dev[id].set_update_period != NULL)
        return (G_gps.dev[id].set_update_period(&(G_gps.dev[id]),
            period));
    else
        return -1;
}

/**
 * Put device into power-saving mode.
 */
int gps_suspend(GPS_HANDLE id)
{
    if (G_gps.dev[id].suspend != NULL)
        return (G_gps.dev[id].suspend(&(G_gps.dev[id])));
    else
        return -1;
}

/**
 * Configure device into normal operating mode
 */
int gps_resume(GPS_HANDLE id)
{
    if (G_gps.dev[id].resume != NULL)
        return (G_gps.dev[id].resume(&(G_gps.dev[id])));
    else
        return -1;
}

/**
 * Get parsed gps data
 */

int gps_get_data(GPS_HANDLE id, gps_data_t *gps_data)
{
    char raw[200];
    int i = 0;
    GPS_LOC
    ;
    memcpy(gps_data, &(gps_session.gpsdata), sizeof(gps_data_t));
    GPS_UNL
    ;
    return 0;
}

#ifdef CONFIG_ECL_RADAR_MODEL
int radar_get_obj_info(GPS_HANDLE id, K_RADAR_OBJ_INFO *radar_data)
{
    int i = 0;
    GPS_LOC
    ;
    memcpy(radar_data, &(radar_session.obj_info), sizeof(K_RADAR_OBJ_INFO));
    GPS_UNL
    ;
    return 0;
}

int radar_get_status_info(GPS_HANDLE id, K_RADAR_STATUS_INFO *radar_data)
{
    int i = 0;
    GPS_LOC
    ;
    memcpy(radar_data, &(radar_session.status_info), sizeof(K_RADAR_STATUS_INFO));
    GPS_UNL
    ;
    return 0;
}
#endif

/**
 * Get gps status and position
 */
int gps_get_raw_data(GPS_HANDLE id, char *gps_data)
{
    if (G_gps.dev[id].get_raw_data != NULL)
        return (G_gps.dev[id].get_raw_data(&(G_gps.dev[id]),
                         gps_data));
    else
        return -1;
}

/**
 * Fully remove the GPS device driver
 */
int gps_drv_remove(GPS_HANDLE id)
{
    if (id >= 0) {
        memset(&G_gps.dev[id], 0, sizeof(gps_dev_s));
        return 0;
    } else {
        return GPS_DEV_REMOVE_FAIL;
    }
}

/**
 * Attach the GPS device driver on the free gps instance and enable
 * the device control.
 */
GPS_HANDLE gps_drv_attach(gps_dev_s *gps)
{
    int i = 0;
    GPS_HANDLE id = -1;
    if (gps == NULL)
        return -1;
    //get a free instance;
    for (i = 0; i < MAX_GPS_INSTANCES; i++) {
        if(G_gps.dev[i].get_raw_data == 0) {
            memcpy(&G_gps.dev[i], gps, sizeof(gps_dev_s));
            id = i;
            break;
        }
    }
    return id;
}

/**
 * Drivers use this api to notify the abstract layer when data is updated
 */
 #ifndef CONFIG_ECL_RADAR_MODEL
int gps_drv_data_update(GPS_HANDLE id, UINT8 *ptr, int len)
{
    GPS_LOC
    ;
    // AmbaPrint("%s",ptr);
    gps_parser(ptr, &gps_session);
    GPS_UNL
    ;
    if (G_gps.hdl[id] != NULL) {
        G_gps.hdl[id](ptr, len);
        return 0;
    } else {
        return GPS_DEV_NO_CALL_BACK_FUNC;
    }
}
#else
int radar_drv_obj_info_update(GPS_HANDLE id, UINT8 *ptr, int len)
{
    GPS_LOC
    ;
    memcpy(&(radar_session.obj_info),ptr ,len);
    GPS_UNL
    ;
    if (G_gps.hdl[id] != NULL) {
        G_gps.hdl[id](ptr, len);
        return 0;
    } else {
        return GPS_DEV_NO_CALL_BACK_FUNC;
    }    
}

int radar_drv_status_info_update(GPS_HANDLE id, UINT8 *ptr, int len)
{
    GPS_LOC
    ;
    memcpy(&(radar_session.status_info), ptr , len);
    GPS_UNL
    ;
    if (G_gps.hdl[id] != NULL) {
        G_gps.hdl[id](ptr, len);
        return 0;
    } else {
        return GPS_DEV_NO_CALL_BACK_FUNC;
    }    
}
#endif