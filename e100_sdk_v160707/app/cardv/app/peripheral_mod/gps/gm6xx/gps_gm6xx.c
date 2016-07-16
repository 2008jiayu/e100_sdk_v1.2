/**
 * @file src/app/connected/applib/src/va/ApplibVideoAnal_StmpHdlr.c
 *
 * Implementation of VA Stamp Handler APIs
 *
 * History:
 *    2015/01/06 - [Bill Chou] created file
 *
 *
 * Copyright (c) 2015 Ambarella, Inc.
 *
 * This file and its contents (¡°Software¡±) are protected by intellectual property rights
 * including, without limitation, U.S. and/or foreign copyrights.  This Software is also the
 * confidential and proprietary information of Ambarella, Inc. and its licensors.  You may
 * not use, reproduce, disclose, distribute, modify, or otherwise prepare derivative
 * works of this Software or any portion thereof except pursuant to a signed license
 * agreement or nondisclosure agreement with Ambarella, Inc. or its authorized
 * affiliates.	In the absence of such an agreement, you agree to promptly notify and
 * return this Software to Ambarella, Inc.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF NON-
 * INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL AMBARELLA, INC. OR ITS AFFILIATES BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; COMPUTER FAILURE OR
 * MALFUNCTION; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <system/ApplibSys_Gps.h>
#include <gps.h>
#include <gps_dev.h>
#include <gps_dev_list.h>
#include <gps_parser.h>

extern AMBA_KAL_BYTE_POOL_t G_MMPL;
static int GpsId = -1;
static gps_data_t GpsData = {0};

extern AMBA_KAL_BYTE_POOL_t G_MMPL;

int GpsGM6xx_Init(void)
{
    int ReturnValue = 0;
    static int GpsInit = 0;
    gps_dev_list_t *GpsDev = 0;
    
    AmbaPrintColor(GREEN,"AppLibVideoAnal_GPSHdlr_init");
    ReturnValue = gps_drv_init( &G_MMPL);
    if (ReturnValue < 0) {
        AmbaPrintColor(RED, "gps_drv_init init failed !! \n");
        AmbaKAL_TaskSleep(100);
        return -1;
    }
    GpsDev = gps_get_dev_list();
    if (GpsDev != NULL) {
        ReturnValue = gps_open(GpsDev->name, &GpsId);
        if (ReturnValue == 0) {
            GpsInit = 1;
        }
        else{
            AmbaPrintColor(RED, "gps_open failed !! \n");
            AmbaKAL_TaskSleep(100);
            return -1;
        }
    } else
        AmbaPrintColor(RED, "GpsDev == NULL \n");
    
    if (GpsId == -1) {
        AmbaPrintColor(RED, "gps got no id !! \n");
        AmbaKAL_TaskSleep(100);
        return -1;
    }
	
#ifdef CONFIG_APP_ARD	
	gps_add_notify_func(GpsId,AppLibSysGps_GpsNotifyFunc);
#endif

    AmbaPrintColor(GREEN,"GPS init OK!!!!");
    return ReturnValue;
}

int GpsGM6XX_GetGpsId(void)
{
    return GpsId;
}

//int GpsGM6XX_GetData(gps_data_t *gps_data)
gps_data_t* GpsGM6XX_GetData(void)
{
    gps_get_data(GpsId, &GpsData);
    return &GpsData;
}

int AppGps_RegisterGM6xx(void)
{
    APPLIB_GPS_s Dev = {0};
    WCHAR DevName[] = {'g','m','6','x','x','\0'};
    w_strcpy(Dev.Name, DevName);
    Dev.Enable = 1;

    Dev.Init = GpsGM6xx_Init;
    Dev.GetGpsId = GpsGM6XX_GetGpsId;
    Dev.GetData = GpsGM6XX_GetData;

    AppLibSysGps_Attach(&Dev);

    return 0;
}

