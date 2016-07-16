/**
 * @file src/app/connected/applib/src/system/ApplibSys_Lcd.c
 *
 * Implementation of LCD panel interface.
 *
 * History:
 *    2013/07/17 - [Martin Lai] created file
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
#include <AmbaUtility.h>

//#define DEBUG_APPLIB_SYS_GPS
#if defined(DEBUG_APPLIB_SYS_GPS)
#define DBGMSG AmbaPrint
#define DBGMSGc(x) AmbaPrintColor(GREEN,x)
#define DBGMSGc2 AmbaPrintColor
#else
#define DBGMSG(...)
#define DBGMSGc(...)
#define DBGMSGc2(...)
#endif

static APPLIB_GPS_s AppLibGpsGlobal = {0};
static gps_data_t gps_data = {0};
#ifdef CONFIG_APP_ARD	
UINT8 gps_raw_data_gprmc[200];
#endif

/*************************************************************************
 * GPS Internal APIs
 ************************************************************************/

/**
 *  @brief Remove the GPS device
 *
 *  Remove the GPS device
 *
 *  @return >=0 success, <0 failure
 */
int AppLibSysGps_Remove(void)
{
    memset(&AppLibGpsGlobal, 0x0, sizeof(APPLIB_GPS_s));
    return 0;
}

/**
 *  @brief Attach the GPS device and enable the device control.
 *
 *  Attach the GPS device and enable the device control.
 *
 *  @param [in] dev Device information
 *
 *  @return >=0 success, <0 failure
 */
int AppLibSysGps_Attach(APPLIB_GPS_s *dev)
{
    char Ft[32] = {0};
    if (dev == NULL)
        return -1;

    AppLibSysGps_Remove();
    memcpy(&AppLibGpsGlobal, dev, sizeof(APPLIB_GPS_s));
    AmbaUtility_Unicode2Ascii(AppLibGpsGlobal.Name, Ft);
    AmbaPrint("[Applib - GPS] Registered GPS %s", Ft);
    return 0;
}

/*************************************************************************
 * GPS Public APIs
 ************************************************************************/
 
/**
 *  @brief Clean GPS configuration
 *
 *  Clean GPS configuration
 *
 *  @return >=0 success, <0 failure
 */
int AppLibSysGps_PreInit(void)
{
    /* Clear AppLibGpsGlobal.*/
    memset(&AppLibGpsGlobal, 0x0, sizeof(APPLIB_GPS_s));
    return 0;
}

/**
 *  @brief GPS initiation
 *
 *  GPS initiation
 *
 *  @return >=0 success, <0 failure
 */
int AppLibSysGps_Init(void)
{
    return AppLibGpsGlobal.Init();
}

/**
 *  @brief To get the GPS ID
 *
 *  To get the GPS ID
 *
 *  @return >=0 success, <0 failure
 */
int AppLibSysGps_GetGpsId(void)
{
    return AppLibGpsGlobal.GetGpsId();
}

/**
 *  @brief To get the GPS data
 *
 *  To get the GPS data
 *
 *  @return >=0 The address of gps data, <0 failure
 */

gps_data_t* AppLibSysGps_GetData(void)
{
    return AppLibGpsGlobal.GetData(); 
}


#ifdef CONFIG_ECL_RADAR_MODEL
K_RADAR_STATUS_INFO* AppLibSysGps_GetStatusData(void)
{
    return AppLibGpsGlobal.GetStatus();
}

K_RADAR_OBJ_INFO* AppLibSysGps_GetObjData(void)
{
    return AppLibGpsGlobal.GetObjData();
}
#endif


#ifdef CONFIG_APP_ARD
/**
 *  @brief To GPS notify function
 *
 *  To GPS notify function
 *
 *  @return >=0 The address of gps data, <0 failure
 */
void AppLibSysGps_GpsNotifyFunc(UINT8 *ptr, int len)
{
	if( ptr[1]=='G' && ptr[2]=='P' && ptr[3]=='R' && ptr[4]=='M' && ptr[5]=='C'){  
		 memcpy(gps_raw_data_gprmc, ptr, len);
	}
}
#endif


