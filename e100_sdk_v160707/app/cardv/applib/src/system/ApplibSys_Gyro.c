/**
 * @file src/app/connected/applib/src/system/ApplibSys_Gyro.c
 *
 * Implementation of Gyro interface.
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

#include <system/ApplibSys_Gyro.h>
//#define AMBA_GYRO_DBG
#ifdef AMBA_GYRO_DBG
#define GYRO_DBG   AmbaPrint
#else
#define GYRO_DBG(...)
#endif

static APPLIB_GYRO_s AppLibGyroGlobal = {0};
#define GYTO_DETECT_TASK_SIZE              0x10000 // TO BE CONTINUED
#define GYRO_DETECT_TASK_PRIORITY      210 // TO BE CONTINUED
#define GYRO_DETECT_TASK_NAMEE          "ApplibGyro_Detect_Task"
static UINT8 GYRO_DETECT_TASK_STACK[GYTO_DETECT_TASK_SIZE];

static void AppLibSysGyro_Detect_Task(UINT32 info)
{
	while (1)
    {

		if (AppLibGyroGlobal.TaskEn) 
        {
			
			if(AppLibGyroGlobal.DetectHandle)
            {
			    AppLibGyroGlobal.DetectHandle();
			}
			AmbaKAL_TaskSleep(10);
		}
        else
		{
			AmbaKAL_TaskSleep(50);
		}
	}

}

/**
 *  @brief Initialize the gyro
 *
 *  Initialize the gyro
 *
 *  @return >=0 success, <0 failure
 */
int AppLibSysGyro_Detect_Task_Init(void)
{
	int Rval = OK;

	// create task
	Rval = AmbaKAL_TaskCreate(&(AppLibGyroGlobal.DetectTask),
								GYRO_DETECT_TASK_NAMEE,
								GYRO_DETECT_TASK_PRIORITY ,
								AppLibSysGyro_Detect_Task,
								0x0,
								GYRO_DETECT_TASK_STACK,
								GYTO_DETECT_TASK_SIZE,
								AMBA_KAL_AUTO_START);

	if (Rval == OK) {
		AmbaPrintColor(GREEN, "[GSensor_Detect_Task_Init] Create Task success");
		AppLibGyroGlobal.TaskInit = 1;
	} else {
		AmbaPrintColor(RED, "[GSensor_Detect_Task_Init] Create Task fail = %d", Rval);
		AppLibGyroGlobal.TaskInit = 0;
		return NG;
	}

	AmbaPrint("GSensor_Detect_Task_Init done");

	return Rval;
}

int AppLibSysGyro_Detect_Task_En(AMBA_GYRO_TASK_STATUS_e En)
{
	int rval = NG;

	if(AppLibGyroGlobal.TaskInit){
		AppLibGyroGlobal.TaskEn = En&0x1;
		rval = OK;
	}

	return rval;
}

/**
 *  @brief Remove the gyro input device
 *
 *  Remove the gyro input device
 *
 *  @return >=0 success, <0 failure
 */
int AppLibSysGyro_Remove(void)
{
    memset(&AppLibGyroGlobal, 0x0, sizeof(APPLIB_GYRO_s));
    return 0;
}

/**
 *  @brief Attach the gyro input device and enable the device control.
 *
 *  Attach the gyro input device and enable the device control.
 *
 *  @param [in] dev Device information
 *
 *  @return >=0 success, <0 failure
 */
int AppLibSysGyro_Attach(APPLIB_GYRO_s *dev)
{
    if (dev == NULL)
        return -1;

    AppLibSysGyro_Remove();
    memcpy(&AppLibGyroGlobal, dev, sizeof(APPLIB_GYRO_s));
    return 0;
}


/**
 *  @brief Clean Gyro configuration
 *
 *  Clean Gyro configuration
 *
 *  @return >=0 success, <0 failure
 */
int AppLibSysGyro_PreInit(void)
{
    memset(&AppLibGyroGlobal, 0x0, sizeof(APPLIB_GYRO_s));
    return 0;
}

int AppLibSysGyro_Init(void)
{
	int rval = NG;

	if (AppLibGyroGlobal.Init == NULL) {
		GYRO_DBG("[AppLibSysGSensor_Init] fail");
	} else {
		// 1, to init device
		if(AppLibGyroGlobal.Init() == OK){


				// 2 to init detect task
				if(AppLibSysGyro_Detect_Task_Init() == OK){                    
					AppLibGyroGlobal.Enable = 1;
					rval = OK;

					// to run task?
					AppLibSysGyro_Detect_Task_En(GYRO_DETECT_TASK_EN);
				}
		}
	}

	return rval;
}

int AppLibSysGyro_Set_Event_Sensitivity(UINT16 Data)
{
	if (AppLibGyroGlobal.event_sensitivity  == NULL) {
		return -1;
	} else {
		return AppLibGyroGlobal.event_sensitivity(Data);
	}
}

short AppLibSysGyro_getdata(void)
{
    short value;
    K_RADAR_OBJ_INFO* rada_obj=NULL;
    rada_obj=AppLibVideo_Ecl_ADAS_GetObjData();
    if(rada_obj==NULL)
    {
      return -1;
    }
    else
    {
        value = rada_obj->acc_std;
        return value;
    }
}


