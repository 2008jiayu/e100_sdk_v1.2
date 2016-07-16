/**
 * @file src/app/cardv/applib/src/system/ApplibSys_GSensor.c
 *
 * Implementation of G-Sensor interface.
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

#include <AmbaDataType.h>
#include <AmbaRTSL_Timer.h>
#include <system/ApplibSys_GSensor.h>

//#define AMBA_GSENSOR_DBG
#ifdef AMBA_GSENSOR_DBG
#define GSENSOR_DBG   AmbaPrint
#else
#define GSENSOR_DBG(...)
#endif

static APPLIB_GSENSOR_s AppLibGSensorGlobal = {0};

#define GSENSOR_DETECT_TASK_SIZE              0x10000 // TO BE CONTINUED
#define GSENSOR_DETECT_TASK_PRIORITY      210 // TO BE CONTINUED
#define GSENSOR_DETECT_TASK_NAMEE          "ApplibGSensor_Detect_Task"
static UINT8 GSENSOR_DETECT_TASK_STACK[GSENSOR_DETECT_TASK_SIZE];

static void GSensor_Detect_Task(UINT32 info)
{
	UINT32 tm1, tm2;

	while (1){

		if (AppLibGSensorGlobal.TaskEn) {

			tm1 = AmbaRTSL_TimerGetTickCount();
			if(AppLibGSensorGlobal.DetectHandle){
				if(AppLibGSensorGlobal.DetectHandle() == OK){
					// callback
					if(AppLibGSensorGlobal.CallBack){
						AppLibGSensorGlobal.CallBack();
					}
				}
			}

			tm2 = AmbaRTSL_TimerGetTickCount();
			if((tm2 >= tm1) && ((tm2 - tm1) < AppLibGSensorGlobal.GSensorInfo.SampleCycle)) {
				AmbaKAL_TaskSleep(AppLibGSensorGlobal.GSensorInfo.SampleCycle - (tm2 - tm1));
			}

		}else{
			AmbaKAL_TaskSleep(50);
		}
	}

}

int AppLibSysGSensor_Detect_Task_Init(void)
{
	int Rval = OK;

	// create task
	Rval = AmbaKAL_TaskCreate(&(AppLibGSensorGlobal.DetectTask),
								GSENSOR_DETECT_TASK_NAMEE,
								GSENSOR_DETECT_TASK_PRIORITY ,
								GSensor_Detect_Task,
								0x0,
								GSENSOR_DETECT_TASK_STACK,
								GSENSOR_DETECT_TASK_SIZE,
								AMBA_KAL_AUTO_START);

	if (Rval == OK) {
		AmbaPrintColor(GREEN, "[GSensor_Detect_Task_Init] Create Task success");
		AppLibGSensorGlobal.TaskInit = 1;
	} else {
		AmbaPrintColor(RED, "[GSensor_Detect_Task_Init] Create Task fail = %d", Rval);
		AppLibGSensorGlobal.TaskInit = 0;
		return NG;
	}

	AmbaPrint("GSensor_Detect_Task_Init done");

	return Rval;
}

int AppLibSysGSensor_Detect_Task_En(AMBA_G_SENSOR_TASK_STATUS_e En)
{
	int rval = NG;

	if(AppLibGSensorGlobal.TaskInit){
		AppLibGSensorGlobal.TaskEn = En&0x1;
		rval = OK;
	}

	return rval;
}

/**
 *  @brief Remove the gsensor input device
 *
 *  Remove the gsensor input device
 *
 *  @return >=0 success, <0 failure
 */
int AppLibSysGSensor_Remove(void)
{
	memset(&AppLibGSensorGlobal, 0x0, sizeof(APPLIB_GSENSOR_s));
	return 0;
}

/**
 *  @brief Attach the gsensor input device and enable the device control.
 *
 *  Attach the gsensor input device and enable the device control.
 *
 *  @param [in] dev Device information
 *
 *  @return >=0 success, <0 failure
 */
int AppLibSysGSensor_Attach(APPLIB_GSENSOR_s *dev)
{
	if (dev == NULL)
		return -1;

	AppLibSysGSensor_Remove();
	memcpy(&AppLibGSensorGlobal, dev, sizeof(APPLIB_GSENSOR_s));

	return 0;
}


/**
 *  @brief Clean Gsensor configuration
 *
 *  Clean GSensor configuration
 *
 *  @return >=0 success, <0 failure
 */
int AppLibSysGSensor_PreInit(void)
{
	memset(&AppLibGSensorGlobal, 0x0, sizeof(APPLIB_GSENSOR_s));
	return 0;
}


/**
 *  @brief Initialize the g-sensor
 *
 *  Initialize the g-sensor
 *
 *  @return >=0 success, <0 failure
 */
int AppLibSysGSensor_Init(void)
{
	int rval = NG;

	if (AppLibGSensorGlobal.Init == NULL) {
		GSENSOR_DBG("[AppLibSysGSensor_Init] fail");
	} else {
		// 1, to init device
		if(AppLibGSensorGlobal.Init() == OK){

			// 2, to get dev info
			if(AppLibSysGSensor_GetInfo(&(AppLibGSensorGlobal.GSensorInfo)) == OK){

				// 3, to init detect task
				if(AppLibSysGSensor_Detect_Task_Init() == OK){

					AppLibGSensorGlobal.Enable = 1;
					rval = OK;

					// to run task?
					AppLibSysGSensor_Detect_Task_En(G_SENSOR_DETECT_TASK_EN);
				}
			}
		}
	}

	return rval;
}

int AppLibSysGSensor_GetInfo(AMBA_G_SENSOR_INFO_s *pGSensorInfo)
{
	if (AppLibGSensorGlobal.GetInfo == NULL) {
		return -1;
	} else {
		return AppLibGSensorGlobal.GetInfo(pGSensorInfo);
	}
}

int AppLibSysGSensor_SetInfo(AMBA_G_SENSOR_INFO_s *pGSensorInfo)
{
	if (AppLibGSensorGlobal.SetInfo == NULL) {
		return -1;
	} else {
		return AppLibGSensorGlobal.SetInfo(pGSensorInfo);
	}
}

int AppLibSysGSensor_SetWorkMode(void)
{
	if (AppLibGSensorGlobal.SetWorkMode == NULL) {
		return -1;
	} else {
		return AppLibGSensorGlobal.SetWorkMode();
	}
}

int AppLibSysGSensor_RegRead(UINT16 Addr, UINT16* Data)
{
	if (AppLibGSensorGlobal.RegRead == NULL) {
		return -1;
	} else {
		return AppLibGSensorGlobal.RegRead( Addr, Data);
	}
}

int AppLibSysGSensor_RegWrite(UINT16 Addr, UINT16 Data)
{
	if (AppLibGSensorGlobal.RegWrite  == NULL) {
		return -1;
	} else {
		return AppLibGSensorGlobal.RegWrite( Addr, Data);
	}
}

int AppLibSysGSensor_Set_Event_Sensitivity(UINT16 Data)
{
	if (AppLibGSensorGlobal.event_sensitivity  == NULL) {
		return -1;
	} else {
		return AppLibGSensorGlobal.event_sensitivity(Data);
	}
}

int AppLibSysGSensor_Enable_Wakeup(UINT16 Data)
{
	if (AppLibGSensorGlobal.enable_wakeup  == NULL) {
		return -1;
	} else {
		return AppLibGSensorGlobal.enable_wakeup(Data);
	}
}

int AppLibSysGSensor_Disable_Wakeup(void)
{
	if (AppLibGSensorGlobal.disable_wakeup  == NULL) {
		return -1;
	} else {
		return AppLibGSensorGlobal.disable_wakeup();
	}
}

int AppLibSysGSensor_Get_Wakeup_Status(void)
{
	if (AppLibGSensorGlobal.poll_wakeup_status  == NULL) {
		return -1;
	} else {
		return AppLibGSensorGlobal.poll_wakeup_status();
	}
}

#ifdef CONFIG_APP_ARD
void AppLibSysGSensor_Get_Cur_Value(float *gx_val,float *gy_val,float *gz_val)
{
    AppLibGSensorGlobal.get_current_value(gx_val, gy_val, gz_val);
}
#endif

