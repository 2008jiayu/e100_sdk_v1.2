/**
 * @file rtos/peripheral/g_sensor/inc/g_sensor.h
 *
 * Header file of G Sensor device abstract layer.
 *
 * History:
 *    2015/05/20 - [Yuchi Wei] created filea
 *
 * Copyright (C) 2009-2015, Ambarella, Inc.
 *
 * All rights reserved. No Part of this file may be reproduced, stored
 * in a retrieval system, or transmitted, in any form, or by any means,
 * electronic, mechanical, photocopying, recording, or otherwise,
 * without the prior consent of Ambarella, Inc.
 */
#ifndef __G_SENSOR_H__
#define __G_SENSOR_H__

#include <AmbaDataType.h>

__BEGIN_C_PROTO__

/* Sensor interface definition */
typedef enum _AMBA_G_SENSOR_INTERFACE_e_ {
	G_SENSOR_ANALOG_INTERFACE		=  0,
	G_SENSOR_DIGITAL_INTERFACE_I2C,
}AMBA_G_SENSOR_INTERFACE_e;

/* Sensor axis definition */
typedef enum _AMBA_G_SENSOR_AXIS_e_ {
	G_SENSOR_2_AXIS	=  2,
	G_SENSOR_3_AXIS
}AMBA_G_SENSOR_AXIS_e;

/* Sensor direction definition */
typedef enum _AMBA_G_SENSOR_DIR_e_ {
	G_SENSOR_X_AXIS	=  0,
	G_SENSOR_Y_AXIS,
	G_SENSOR_Z_AXIS
}AMBA_G_SENSOR_DIR_e;

/* Sensor polarity definition */
typedef enum _AMBA_G_SENSOR_DIR_POLARITY_e_ {
	G_SENSOR_NEGATIVE_POLARITY	=  0,
	G_SENSOR_POSITIVE_POLARITY
}AMBA_G_SENSOR_DIR_POLARITY_e;

typedef enum _AMBA_G_SENSOR_INDEX_e_ {
	AMBA_G_SENSOR_0 = 0,       /* 0th */
	AMBA_NUM_G_SENSOR         /* Number of G-SENSOR */
} AMBA_G_SENSOR_INDEX_e;

typedef enum _AMBA_G_SENSOR_MODE_e_ {
	G_SENSOR_MODE_NORMAL = 0,
	G_SENSOR_MODE_PARKING,
} AMBA_G_SENSOR_MODE_e;

typedef struct _AMBA_G_SENSOR_INFO_s_ {
	AMBA_G_SENSOR_INTERFACE_e Interface;
	AMBA_G_SENSOR_DIR_e Axis;
	AMBA_G_SENSOR_DIR_POLARITY_e Polarity;

	UINT16 MaxSense;
	UINT16 MaxBias;
	UINT16 AdcResolution;
	UINT16 SampleCycle;

	UINT16 I2CCHN;
	UINT16 I2CAddr;
	UINT16 MaxReg;

} AMBA_G_SENSOR_INFO_s;

typedef struct _AMBA_G_SENSOR_OBJ_s_ {
	int (*Init)(void);
	int (*GetInfo)(AMBA_G_SENSOR_INFO_s *pGSensorInfo);
	int (*SetInfo)(AMBA_G_SENSOR_INFO_s *pGSensorInfo);
	int (*Write)(UINT16 Addr, UINT16 Data);
	int (*Read)(UINT16 Addr, UINT16* Data);
	int (*enable_interrupt)(int);
	int (*disable_interrupt)(void);
	int  (*poll_interrupt_status)(void);
} AMBA_G_SENSOR_OBJ_s;

/*-----------------------------------------------------------------------------------------------*\
 * Defined in AmbaGSensor.c
\*-----------------------------------------------------------------------------------------------*/
extern AMBA_G_SENSOR_OBJ_s *pAmbaGSensorObj[];

#define AmbaGSensor_Hook(Index, CodecObj)                   pAmbaGSensorObj[Index] = CodecObj
#define AmbaGSensor_Init(Index)                                      ((pAmbaGSensorObj[Index]== NULL)?(NG):((pAmbaGSensorObj[Index]->Init == NULL)?NG:(pAmbaGSensorObj[Index]->Init())))
#define AmbaGSensor_GetInfo(Index, Info)                       ((pAmbaGSensorObj[Index]== NULL)?(NG):((pAmbaGSensorObj[Index]->GetInfo == NULL)?NG:(pAmbaGSensorObj[Index]->GetInfo(Info))))
#define AmbaGSensor_SetInfo(Index, Info)                       ((pAmbaGSensorObj[Index]== NULL)?(NG):((pAmbaGSensorObj[Index]->SetInfo == NULL)?NG:(pAmbaGSensorObj[Index]->SetInfo(Info))))
#define AmbaGSensor_Write(Index, Addr, Data)                ((pAmbaGSensorObj[Index]== NULL)?(NG):((pAmbaGSensorObj[Index]->Write == NULL)?NG:(pAmbaGSensorObj[Index]->Write(Addr, Data))))
#define AmbaGSensor_Read(Index, Addr, Data)                 ((pAmbaGSensorObj[Index]== NULL)?(NG):((pAmbaGSensorObj[Index]->Read == NULL)?NG:(pAmbaGSensorObj[Index]->Read(Addr, Data))))
#define AmbaGSensor_Enable_Wakeup(Index,Data)               ((pAmbaGSensorObj[Index]== NULL)?(NG):((pAmbaGSensorObj[Index]->enable_interrupt == NULL)?NG:(pAmbaGSensorObj[Index]->enable_interrupt(Data))))
#define AmbaGSensor_Disable_Wakeup(Index)               ((pAmbaGSensorObj[Index]== NULL)?(NG):((pAmbaGSensorObj[Index]->disable_interrupt == NULL)?NG:(pAmbaGSensorObj[Index]->disable_interrupt())))
#define AmbaGSensor_Poll_Interrupt_Status(Index)               ((pAmbaGSensorObj[Index]== NULL)?(NG):((pAmbaGSensorObj[Index]->poll_interrupt_status == NULL)?NG:(pAmbaGSensorObj[Index]->poll_interrupt_status())))

__END_C_PROTO__

#endif /* __G_SENSOR_H__ */
