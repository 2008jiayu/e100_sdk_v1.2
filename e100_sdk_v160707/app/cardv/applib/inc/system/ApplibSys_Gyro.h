/**
 * @file src/app/connected/applib/inc/system/ApplibSys_Gyro.h
 *
 * Header of Gyro interface.
 *
 * History:
 *    2014/02/13 - [Martin Lai] created file
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
#ifndef APP_GYRO_H_
#define APP_GYRO_H_
/**
 * @defgroup System
 * @brief Interface of utility
 *
 *
 * interface of utility
 * Ex: Vin ,Vout,Sensor, Lens
 */

/**
* @defgroup ApplibSys_Gyro
* @brief Gyro interface.
*
*
*/

/**
 * @addtogroup ApplibSys_Gyro
 * @ingroup System
 * @{
 */
#include <mw.h>
#include <applib.h>

__BEGIN_C_PROTO__

/*************************************************************************
 * Gyro structure
 ************************************************************************/
/**
 *  This data structure describes the interface of a gyro
 */
typedef struct _APPLIB_GYRO_s_ {
    /** Module ID */
    UINT32 Id;
    /** Module name */
    WCHAR Name[32];
    /** Module init interface */
    UINT16 Enable:1;      /**<Enable*/
	UINT16 TaskInit:1;
	UINT16 TaskEn:1;
	AMBA_KAL_TASK_t DetectTask;

	int (*Init)(void);        
	int (*DetectHandle)(void);       
	int (*event_sensitivity)(UINT16 sens);
} APPLIB_GYRO_s;

typedef enum _AMBA_GYRO_TASK_STATUS_e_ {
	GYRO_DETECT_TASK_DIS = 0,
	GYRO_DETECT_TASK_EN = 0x1,
} AMBA_GYRO_TASK_STATUS_e;

/**
 *  Remove the gyro input device
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibSysGyro_Remove(void);

/**
 *  Attach the gyro input device and enable the device control.
 *
 *  @param [in] dev Device information
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibSysGyro_Attach(APPLIB_GYRO_s *dev);

/**
 *  Clean Gyro configuration
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibSysGyro_PreInit(void);

/**
 *  Initialize the gyro
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibSysGyro_Init(void);
extern int AppLibSysGyro_Detect_Task_En(AMBA_GYRO_TASK_STATUS_e En);
extern int AppLibSysGyro_Set_Event_Sensitivity(UINT16 Data);
extern short AppLibSysGyro_getdata(void);


__END_C_PROTO__
/**
 * @}
 */
#endif /* APP_GYRO_H_ */

