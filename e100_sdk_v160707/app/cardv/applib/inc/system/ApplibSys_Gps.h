/**
 * @file src/app/connected/applib/inc/system/ApplibSys_Lcd.h
 *
 * Header of LCD panel interface.
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
#ifndef APPLIB_GPS_H_
#define APPLIB_GPS_H_
/**
* @defgroup ApplibSys_Gps
* @brief GPS interface.
*
*
*/

/**
 * @addtogroup ApplibSys_Gps
 * @ingroup System
 * @{
 */
#include <mw.h>
#include <applib.h>
#include <gps_struct.h>

__BEGIN_C_PROTO__

/*************************************************************************
 * GPS definitions
 ************************************************************************/
 
#define MPS_TO_KMPH (3.6)

/*************************************************************************
 * GPS structure
 ************************************************************************/
/**
 *  This data structure describes the interface of a LCD panel
 */
typedef struct _APPLIB_GPS_s_ {
    WCHAR Name[32];/**< Module name */
    /* UINT16 start */
    UINT16 Enable:1; /**<Enable*/
    int (*Init)(void);/**< Module init interface */
    /* Module parameter get interface */
    int (*GetGpsId)(void); /**< Get GPS Mode*/
   
    gps_data_t* (*GetData)(void); /**< Get GPS data*/
    #ifdef CONFIG_ECL_RADAR_MODEL
	K_RADAR_OBJ_INFO* (*GetObjData)(void); /**< Get radar data*/
    K_RADAR_STATUS_INFO* (*GetStatus)(void); /**< Get Status data*/
    #endif
} APPLIB_GPS_s;


/*************************************************************************
 * GPS Internal APIs
 ************************************************************************/
/**
 *  Remove the GPS device
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibSysGps_Remove(void);

/**
 *  Attach the GPS device and enable the device control.
 *
 *  @param [in] dev Device information
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibSysGps_Attach(APPLIB_GPS_s *dev);

/*************************************************************************
 * GPS Public APIs
 ************************************************************************/
/**
 *  Clean GPS configuration
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibSysGps_PreInit(void);

/**
 *  GPS initiation
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibSysGps_Init(void);

/**
 *  To get the GPS ID
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibSysGps_GetGpsId(void);

/**
 *  To get the GPS data
 *
 *  @return >=0 The address of gps data, <0 failure
 */
extern gps_data_t* AppLibSysGps_GetData(void);

#ifdef CONFIG_APP_ARD	
/**
 *  @brief To GPS notify function
 *
 *  To GPS notify function
 *
 *  @return >=0 The address of gps data, <0 failure
 */
extern void AppLibSysGps_GpsNotifyFunc(UINT8 *ptr, int len);
#endif




__END_C_PROTO__
/**
 * @}
 */
#endif /* APPLIB_LCD_H_ */
