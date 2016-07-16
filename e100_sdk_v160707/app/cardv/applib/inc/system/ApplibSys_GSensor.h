/**
 * @file src/app/cardv/applib/inc/system/ApplibSys_GSensor.h
 *
 * Header of GSensor interface.
 *
 * History:
 *    2015/05/20 - [Yuchi Wei] created file
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
#ifndef APP_GSENSOR_H_
#define APP_GSENSOR_H_
/**
 * @defgroup System
 * @brief Interface of utility
 *
 *
 * interface of utility
 * Ex: Vin ,Vout,Sensor, Lens
 */

/**
* @defgroup ApplibSys_GSensor
* @brief GSensor interface.
*
*
*/

/**
 * @addtogroup ApplibSys_GSensor
 * @ingroup System
 * @{
 */

#include <AmbaDataType.h>
#include <AmbaKAL.h>
#include <mw.h>
#include <applib.h>
#include <AmbaGSensor.h>

__BEGIN_C_PROTO__

/*************************************************************************
 * GSensor structure
 ************************************************************************/
/**
 *  This data structure describes the interface of a g-sensor
 */
typedef struct _APPLIB_GSENSOR_s_ {
	WCHAR Name[32];    /**< Module name */
	UINT16 Enable:1;      /**<Enable*/
	UINT16 TaskInit:1;
	UINT16 TaskEn:1;
	AMBA_KAL_TASK_t DetectTask;
	AMBA_G_SENSOR_INFO_s GSensorInfo;

	int (*Init)(void);        /**< Module init interface */
	int (*GetInfo)(AMBA_G_SENSOR_INFO_s *pGSensorInfo);        /**< Module init interface */
	int (*SetInfo)(AMBA_G_SENSOR_INFO_s *pGSensorInfo);        /**< Module init interface */
	int (*SetWorkMode)(void);        /**< Module init interface */
	int (*CallBack)(void);        /**< Module init interface */
	int (*DetectHandle)(void);        /**< Module init interface */
	int (*RegRead)(UINT16 Addr, UINT16* Data);        /**< Module init interface */
	int (*RegWrite)(UINT16 Addr, UINT16 Data);        /**< Module init interface */
	int (*event_sensitivity)(UINT16 sens);
	int (*enable_wakeup)(UINT16 sens);
	int (*disable_wakeup)(void);
	int (*poll_wakeup_status)(void);
#ifdef CONFIG_APP_ARD
    void (*get_current_value)(float *gx_val,float *gy_val,float *gz_val);
#endif
} APPLIB_GSENSOR_s;

typedef enum _AMBA_G_SENSOR_TASK_STATUS_e_ {
	G_SENSOR_DETECT_TASK_DIS = 0,
	G_SENSOR_DETECT_TASK_EN = 0x1,
} AMBA_G_SENSOR_TASK_STATUS_e;

/**
 *  Remove the gsensor input device
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibSysGSensor_Remove(void);

/**
 *  Attach the g-sensor input device and enable the device control.
 *
 *  @param [in] dev Device information
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibSysGSensor_Attach(APPLIB_GSENSOR_s *dev);

/**
 *  Clean GSensor configuration
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibSysGSensor_PreInit(void);

/**
 *  Initialize the g-sensor
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibSysGSensor_Init(void);

extern int AppLibSysGSensor_GetInfo(AMBA_G_SENSOR_INFO_s *pGSensorInfo);
extern int AppLibSysGSensor_SetInfo(AMBA_G_SENSOR_INFO_s *pGSensorInfo);
extern int AppLibSysGSensor_SetWorkMode(void);
extern int AppLibSysGSensor_RegRead(UINT16 Addr, UINT16* Data);
extern int AppLibSysGSensor_RegWrite(UINT16 Addr, UINT16 Data);
extern int AppLibSysGSensor_Set_Event_Sensitivity(UINT16 Data);
extern int AppLibSysGSensor_Enable_Wakeup(UINT16 Data);
extern int AppLibSysGSensor_Disable_Wakeup(void);
extern int AppLibSysGSensor_Get_Wakeup_Status(void);
#ifdef CONFIG_APP_ARD
extern void AppLibSysGSensor_Get_Cur_Value(float *gx_val,float *gy_val,float *gz_val);
#endif
__END_C_PROTO__
/**
 * @}
 */
#endif /* APP_GSENSOR_H_ */

