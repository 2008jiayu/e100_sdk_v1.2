/**
 * @file rtos/peripheral/g_sensor/AmbaGSensor_MIRA_DA380.c
 *
 * This driver is for MIRA DA380 G-Sensor.
 *
 * History:
 *    2015/06/24 [Jim Meng] - created file
 *
 * Copyright (C) 2009-2015, Ambarella, Inc.
 *
 * All rights reserved. No Part of this file may be reproduced, stored
 * in a retrieval system, or transmitted, in any form, or by any means,
 * electronic, mechanical, photocopying, recording, or otherwise,
 * without the prior consent of Ambarella, Inc.
 */

#include <AmbaDataType.h>
#include <AmbaKAL.h>
#include <AmbaPrint.h>
#include <AmbaPrintk.h>
#include <AmbaUART.h>

#include <string.h>
#include <stdlib.h>

#include "AmbaI2C.h"
#include "AmbaGSensor.h"
#include "AmbaGSensor_MIRA_DA380.h"
#include "AmbaGPIO.h"

//#define AMBA_GSENSOR_DBG
#ifdef AMBA_GSENSOR_DBG
#define GSENSOR_DBG   AmbaPrint
#else
#define GSENSOR_DBG(...)
#endif

static AMBA_G_SENSOR_INFO_s DA380_Info = {
	.Interface = G_SENSOR_DIGITAL_INTERFACE_I2C,
	.Axis = G_SENSOR_3_AXIS,
	.Polarity = G_SENSOR_NEGATIVE_POLARITY,
	.MaxSense = 1023,
	.MaxBias = 4095,
	.AdcResolution = 12,
	.SampleCycle = 100,

	.I2CCHN = AMBA_I2C_CHANNEL0,
	.I2CAddr = GSENSOR_MIRA_DA380_I2C_ADDR,
	.MaxReg = MAX_REG,
};

int GSENSOR_MIRA_DA380_GetInfo(AMBA_G_SENSOR_INFO_s *Info)
{
	int res = NG;

	if(Info != NULL){
		memcpy((void*)Info, &DA380_Info, sizeof(DA380_Info));
		res = OK;
	}

	return res;
}

int GSENSOR_MIRA_DA380_SetInfo(AMBA_G_SENSOR_INFO_s *Info)
{
	int res = NG;

	if(Info != NULL){
		memcpy((void*)(&DA380_Info), Info, sizeof(DA380_Info));
		res = OK;
	}

	return res;
}

int GSENSOR_MIRA_DA380_Read(UINT16 Addr, UINT16 *Data)
{
	UINT8 RxDataBuf = 0;
	UINT16 WorkUINT16[3];

	WorkUINT16[0] = AMBA_I2C_RESTART_FLAG | DA380_Info.I2CAddr; /* Slave Address + r/w (0) */
	WorkUINT16[1] = Addr;											         /* Sub Address */
	WorkUINT16[2] = AMBA_I2C_RESTART_FLAG | DA380_Info.I2CAddr | 0x01;

	if (AmbaI2C_ReadAfterWrite(DA380_Info.I2CCHN, AMBA_I2C_SPEED_FAST,
							   3, WorkUINT16, 1, &RxDataBuf, 5000) != OK) {
		GSENSOR_DBG("[%s] fail", __func__);
		return NG;
	}else{
		*Data = (UINT16)RxDataBuf;
		GSENSOR_DBG("[%s] read 0x%x data:0x%x", __func__, Addr, RxDataBuf);
	}

	return OK;
}

int GSENSOR_MIRA_DA380_Write(UINT16 Addr, UINT16 Data)
{
	UINT8 TxDataBuf[2];
	int rval = OK;

	TxDataBuf[0] = (UINT8)Addr;
	TxDataBuf[1] = (UINT8)(Data);

	GSENSOR_DBG("[%s]: Addr: 0x%02x, Data: 0x%02x", __func__, Addr, Data);

	if(AmbaI2C_Write(DA380_Info.I2CCHN,
	                AMBA_I2C_SPEED_FAST,
					DA380_Info.I2CAddr,
					 2,
					TxDataBuf,
					 5000) != OK){
		GSENSOR_DBG("[%s]: Addr: 0x%02x, Data: 0x%02x fail", __func__, Addr, Data);
		rval = NG;
	}

	return rval;

}

static void GSENSOR_MIRA_DA380_Mask_Write(UINT16  addr, UINT16  mask, UINT16  data)
{
    UINT16  tmp_data;

	GSENSOR_MIRA_DA380_Read(addr,&tmp_data);
    tmp_data &= ~mask;
    tmp_data |= data & mask;
	GSENSOR_MIRA_DA380_Write(addr,tmp_data);
}


int GSENSOR_MIRA_DA380_Init(void)
{
	UINT16 reg;
	UINT16 data;

	reg = NSA_REG_WHO_AM_I;;
	GSENSOR_MIRA_DA380_Read(reg, &data);
	if (data != WHO_AM_I_ID) {
		AmbaPrintColor(RED, "ERROR: MIRA G-Sensor ID [0x%x], read G-Sensor ID [0x%x]",
			WHO_AM_I_ID, data);
		return (-1);
	}
	GSENSOR_MIRA_DA380_Mask_Write(NSA_REG_SPI_I2C, 0x24, 0x24);
    AmbaKAL_TaskSleep(20);

	GSENSOR_MIRA_DA380_Mask_Write(NSA_REG_G_RANGE, 0x0f, 0x04);
	GSENSOR_MIRA_DA380_Mask_Write(NSA_REG_POWERMODE_BW, 0xFF, 0x1E);
	GSENSOR_MIRA_DA380_Mask_Write(NSA_REG_ODR_AXIS_DISABLE, 0xFF, 0x07);

	GSENSOR_MIRA_DA380_Mask_Write(NSA_REG_INT_PIN_CONFIG, 0x0F, 0x05);//set int_pin level
	GSENSOR_MIRA_DA380_Mask_Write(NSA_REG_INT_LATCH, 0x8F, 0x8f);//clear latch and set latch mode

	GSENSOR_MIRA_DA380_Mask_Write(NSA_REG_ENGINEERING_MODE, 0xFF, 0x83);
	GSENSOR_MIRA_DA380_Mask_Write(NSA_REG_ENGINEERING_MODE, 0xFF, 0x69);
	GSENSOR_MIRA_DA380_Mask_Write(NSA_REG_ENGINEERING_MODE, 0xFF, 0xBD);

	return OK;
}

static int GSENSOR_MIRA_DA380_enable_interrupt(int sensitivity){

	UINT16 Data;
    UINT16  tmp_data;

	Data = 0x03;
	GSENSOR_MIRA_DA380_Write(NSA_REG_INTERRUPT_SETTINGS1,Data);
	Data = 0x03;
	GSENSOR_MIRA_DA380_Write(NSA_REG_ACTIVE_DURATION,0x03 );

	switch(sensitivity){
	case 0:
		Data = 0x1B;
		GSENSOR_MIRA_DA380_Write(NSA_REG_ACTIVE_THRESHOLD,Data );
		break;
	case 1:
		Data = 0x12;
		GSENSOR_MIRA_DA380_Write(NSA_REG_ACTIVE_THRESHOLD,Data );
		break;
	case 2:
		Data = 0x1B;
		GSENSOR_MIRA_DA380_Write(NSA_REG_ACTIVE_THRESHOLD,Data );
		break;
	case 3:
		Data = 0x24;
		GSENSOR_MIRA_DA380_Write(NSA_REG_ACTIVE_THRESHOLD,Data );

		break;
	default:
		Data = 0x1B;
		GSENSOR_MIRA_DA380_Write(NSA_REG_ACTIVE_THRESHOLD,Data );
		break;
	}
	Data = 0x04;
	GSENSOR_MIRA_DA380_Write(NSA_REG_INTERRUPT_MAPPING1,Data );
	GSENSOR_MIRA_DA380_Write(NSA_REG_INTERRUPT_MAPPING3,Data );
	GSENSOR_MIRA_DA380_Mask_Write(NSA_REG_INT_LATCH, 0x8F, 0x8f);//clear latch and set latch mode

	return OK;
}

static int GSENSOR_MIRA_DA380_disable_interrupt(void){

	UINT16 Data;

	Data = 0;
	GSENSOR_MIRA_DA380_Write(NSA_REG_INTERRUPT_SETTINGS1,Data );
	Data = 0;
	GSENSOR_MIRA_DA380_Write(NSA_REG_INTERRUPT_MAPPING1,Data );
	return OK;
}


static int GSENSOR_MIRA_DA380_poll_interrupt_status(void)
{
	int rval = -1;
	UINT16 Data;
	AMBA_GPIO_PIN_INFO_s pinInfo;
	AMBA_GPIO_PIN_LEVEL_e level;
    AmbaGPIO_ConfigInput(GPIO_PIN_7);
	AmbaGPIO_GetPinInfo(GPIO_PIN_7, &pinInfo);
	level = pinInfo.Level;
	if(level)
	rval = OK;
	return rval;
}


AMBA_G_SENSOR_OBJ_s AmbaGSensor_DA380Obj = {
	.Init       = GSENSOR_MIRA_DA380_Init,
	.GetInfo = GSENSOR_MIRA_DA380_GetInfo,
	.SetInfo = GSENSOR_MIRA_DA380_SetInfo,
	.Write    = GSENSOR_MIRA_DA380_Write,
	.Read    = GSENSOR_MIRA_DA380_Read,
	.enable_interrupt = GSENSOR_MIRA_DA380_enable_interrupt,
	.disable_interrupt    = GSENSOR_MIRA_DA380_disable_interrupt,
	.poll_interrupt_status    = GSENSOR_MIRA_DA380_poll_interrupt_status,
};


