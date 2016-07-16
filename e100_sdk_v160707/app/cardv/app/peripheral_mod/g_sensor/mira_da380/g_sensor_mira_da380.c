/**
 * @file src/app/cardv/app/peripheral_mod/g_sensor/mira_da380/g_sensor_mira_da380.c
 *
 * Implementation of APIs
 *
 * History:
 *    2015/06/25 - [Jim Meng] created file
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

#include <system/ApplibSys_GSensor.h>
#include <AmbaGSensor.h>
#include <apps/apps.h>
int AppGSensor_DA380_RegRead(UINT16 Addr, UINT16* Data);

int AppGSensor_DA380_Init(void)
{
	return AmbaGSensor_Init(AMBA_G_SENSOR_0);
}

int AppGSensor_DA380_GetInfo(AMBA_G_SENSOR_INFO_s *pGSensorInfo)
{
	return AmbaGSensor_GetInfo(AMBA_G_SENSOR_0, pGSensorInfo) ;
}

int AppGSensor_DA380_SetInfo(AMBA_G_SENSOR_INFO_s *pGSensorInfo)
{
	return AmbaGSensor_GetInfo(AMBA_G_SENSOR_0, pGSensorInfo) ;
}

int AppGSensor_DA380_SetWorkMode(void)
{
	return OK;
}

int AppGSensor_DA380_CallBack(void)
{
	// to be continued
	return OK;
}

/**/
#define NSA_REG_ACC_X_LSB               0x02
#define NSA_REG_ACC_X_MSB               0x03
#define NSA_REG_ACC_Y_LSB               0x04
#define NSA_REG_ACC_Y_MSB               0x05
#define NSA_REG_ACC_Z_LSB               0x06
#define NSA_REG_ACC_Z_MSB               0x07
static float g_target =  0.5;
#ifdef CONFIG_APP_ARD
static float gx_cur = 0, gy_cur = 0, gz_cur = 0;
#endif

void calc_gvalue(UINT16 g, float *fg)
{
	float temp = 0.00;
	float temp_g = (float)(g);
    AMBA_G_SENSOR_INFO_s GSensorInfo = {0};

	AppLibSysGSensor_GetInfo(&(GSensorInfo));
//	AmbaPrintColor(BLUE,"g = 0x%x   temp_g = %.2f", g, temp_g);
	if(g<(1 << (GSensorInfo.AdcResolution - 1)))
	{
		temp = (float)(temp_g/GSensorInfo.MaxSense);

	}else{
		temp_g=GSensorInfo.MaxBias-temp_g;
		temp=(float)(-temp_g/GSensorInfo.MaxSense);

	}
//	AmbaPrintColor(BLUE,"temp = %.2f", temp);
	*fg = temp;
}

void Max3(float *dtmax, float a, float b, float c)
{
	float temp = 0.0;
	if (temp<a)
		temp = a;
	if (temp<b)
		temp = b;
	if(temp<c)
		temp = c;
	*dtmax =  temp;
}

void Gfabs(float *a, float b)
{
	*a = (((b) < 0) ? -(b) : (b));
}

void Check_even(float x, float y, float z)
{
	static float g_data[5][3] = {0.0};
	static int gc =0;
	int i = 0, j = 0;
	static float avg[3] = {0.0};
	static float dtmax, dtx, dty, dtz;

	if (gc<5){
		g_data[gc][0] = x;
		g_data[gc][1] = y;
		g_data[gc][2] = z;
		gc++;
		avg[0] = (g_data[0][0] + g_data[1][0] + g_data[2][0] + g_data[3][0] + g_data[4][0])/gc;
		avg[1] = (g_data[0][1] + g_data[1][1] + g_data[2][1] + g_data[3][1] + g_data[4][1])/gc;
		avg[2] = (g_data[0][2] + g_data[1][2] + g_data[2][2] + g_data[3][2] + g_data[4][2])/gc;
		return;
	}
	else{
	//	printk("x = %.2f  avg[0] = %.2f", x, avg[0]);
	//	printk("y = %.2f  avg[1] = %.2f", y, avg[1]);
	//	printk("z = %.2f  avg[2] = %.2f", z, avg[2]);

		Gfabs(&dtx, (x-avg[0]));
		Gfabs(&dty, (y-avg[1]));
		Gfabs(&dtz, (z-avg[2]));
		Max3(&dtmax, dtx, dty, dtz);
	//	printk("dtmax = %.2f", dtmax);
		if (dtmax > g_target){
			//printk("urgent event");
			AmbaPrintColor(BLUE,"urgent event:send msg:AMSG_CMD_GSENSOR_EVEN");
			AppLibComSvcHcmgr_SendMsg(AMSG_CMD_EVENT_RECORD, 0, 0);
			gc = 0;
			return;
		}

		for(i=0; i<4; i++){
			for(j=0; j<3; j++){
			g_data[i][j] = g_data[i+1][j];
			}
		}

		g_data[4][0] = x;
		g_data[4][1] = y;
		g_data[4][2] = z;

		avg[0] = (g_data[0][0] + g_data[1][0] + g_data[2][0] + g_data[3][0] + g_data[4][0])/5.0;
		avg[1] = (g_data[0][1] + g_data[1][1] + g_data[2][1] + g_data[3][1] + g_data[4][1])/5.0;
		avg[2] = (g_data[0][2] + g_data[1][2] + g_data[2][2] + g_data[3][2] + g_data[4][2])/5.0;
	}

}

int AppGSensor_DA380_DetectHandle(void)
{
	UINT16 data[6];
	UINT16 reg;
	UINT16 tmp;
	UINT16 xdata,ydata,zdata;
	float gx, gy, gz;

	reg = NSA_REG_ACC_X_LSB;
	AppGSensor_DA380_RegRead(reg, &data[0]);
	reg = NSA_REG_ACC_X_MSB;
	AppGSensor_DA380_RegRead(reg, &data[1]);
	reg = NSA_REG_ACC_Y_LSB;
	AppGSensor_DA380_RegRead(reg, &data[2]);
	reg = NSA_REG_ACC_Y_MSB;
	AppGSensor_DA380_RegRead(reg, &data[3]);
	reg = NSA_REG_ACC_Z_LSB;
	AppGSensor_DA380_RegRead(reg, &data[4]);
	reg = NSA_REG_ACC_Z_MSB;
	AppGSensor_DA380_RegRead(reg, &data[5]);
	tmp = ((data[1] << 8) | (data[0] & 0xff))>>4;
	xdata = tmp&0x0fff;
	tmp = ((data[3] << 8) | (data[2] & 0xff))>>4;
	ydata = tmp&0x0fff;
	tmp = ((data[5] << 8) | (data[4] & 0xff))>>4;
	zdata = tmp&0x0fff;
	calc_gvalue(xdata, &gx);
	calc_gvalue(ydata, &gy);
	calc_gvalue(zdata, &gz);
#ifdef CONFIG_APP_ARD
	gx_cur = gx;
	gy_cur = gy;
	gz_cur = gz;
#endif

	Check_even(gx, gy, gz);
	return OK;
}

int AppGSensor_DA380_RegRead(UINT16 Addr, UINT16* Data)
{
	return (AmbaGSensor_Read(AMBA_G_SENSOR_0, Addr, Data));
}

int AppGSensor_DA380_RedWrite(UINT16 Addr, UINT16 Data)
{
	return (AmbaGSensor_Write(AMBA_G_SENSOR_0, Addr, Data)) ;
}

void AppGSensor_DA380_set_event_sensitivity(UINT16 sens)
{
	if(sens == 1)
		g_target = 0.5;
	else if(sens == 2)
		g_target = 1.0;
	else if(sens == 3)
		g_target = 1.5;
	else
		g_target = 0;

}

int AppGSensor_DA380_Enable_Wakeup(UINT16 sens)
{
	return AmbaGSensor_Enable_Wakeup(AMBA_G_SENSOR_0,sens);
}

int AppGSensor_DA380_Disable_Wakeup(void)
{
	return AmbaGSensor_Disable_Wakeup(AMBA_G_SENSOR_0);
}

int AppGSensor_DA380_Poll_Wakeup_Status(void)
{
	return AmbaGSensor_Poll_Interrupt_Status(AMBA_G_SENSOR_0);
}
#ifdef CONFIG_APP_ARD
void AppGSensor_DA380_Get_Current_Value(float *gx_val,float *gy_val,float *gz_val)
{
    *gx_val = gx_cur;
    *gy_val = gy_cur;
    *gz_val = gz_cur;
}
#endif

int AppGSensor_RegisterMIRADA380(void)
{
	APPLIB_GSENSOR_s Dev = {0};
	WCHAR DevName[] = {'D','A','3','8','0','\0'};
	w_strcpy(Dev.Name, DevName);
	Dev.Enable = 0;

	Dev.Init = AppGSensor_DA380_Init;
	Dev.GetInfo = AppGSensor_DA380_GetInfo;
	Dev.SetInfo = AppGSensor_DA380_SetInfo;
	Dev.SetWorkMode = AppGSensor_DA380_SetWorkMode;
	Dev.CallBack = AppGSensor_DA380_CallBack;
	Dev.DetectHandle= AppGSensor_DA380_DetectHandle;
	Dev.RegRead= AppGSensor_DA380_RegRead;
	Dev.RegWrite= AppGSensor_DA380_RedWrite;
	Dev.event_sensitivity= AppGSensor_DA380_set_event_sensitivity;
	Dev.enable_wakeup= AppGSensor_DA380_Enable_Wakeup;
	Dev.disable_wakeup= AppGSensor_DA380_Disable_Wakeup;
	Dev.poll_wakeup_status= AppGSensor_DA380_Poll_Wakeup_Status;
#ifdef CONFIG_APP_ARD
	Dev.get_current_value = AppGSensor_DA380_Get_Current_Value;
#endif

	AppLibSysGSensor_Attach(&Dev);

	return 0;
}

