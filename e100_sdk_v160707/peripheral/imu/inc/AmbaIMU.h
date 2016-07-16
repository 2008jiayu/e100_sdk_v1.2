/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaIMU.h
 *
 *  @Copyright      :: Copyright (C) 2013 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Definitions for Ambarella IMU (Accelerometer and Gyroscope Devices) driver APIs
\*-------------------------------------------------------------------------------------------------------------------*/

#ifndef _AMBA_IMU_H_
#define _AMBA_IMU_H_

typedef enum _AMBA_IMU_NUM_e_ {
    AMBA_IMU_SENSOR0,
    AMBA_IMU_SENSOR1,

    AMBA_NUM_IMU_SENSOR
} AMBA_IMU_NUM_e;

typedef enum _AMBA_IMU_TYPE_e_ {
    AMBA_IMU_NULL,
    AMBA_IMU_GYRO,      /* gyroscope */
    AMBA_IMU_ACCMETER,  /* accelerometer */

    AMBA_IMU_COMBO
} AMBA_IMU_TYPE_e;

typedef enum _AMBA_IMU_INTERFACE_e_ {
    AMBA_IMU_SPI,
    AMBA_IMU_I2C,

} AMBA_IMU_INTERFACE_e_;

typedef enum _AMBA_IMU_AXIS_e_ {
    AMBA_IMU_2AXIS,
    AMBA_IMU_3AXIS,
    AMBA_IMU_6AXIS,

} AMBA_IMU_AXIS_e_;

typedef enum _AMBA_IMU_DIR_e_ {
    AMBA_IMU_X,
    AMBA_IMU_Y,
    AMBA_IMU_Z,

} AMBA_IMU_DIR_e_;

typedef enum _AMBA_IMU_POLARITY_e_ {
    AMBA_IMU_NEGATIVE,
    AMBA_IMU_POSITIVE,

} AMBA_IMU_POLARITY_e_;

typedef struct _AMBA_IMU_DATA_s_ {
    UINT16 GyroXData;
    UINT16 GyroYData;
    UINT16 GyroZData;
    UINT16 TemperatureData;
    UINT16 AccXData;
    UINT16 AccYData;
    UINT16 AccZData;
} AMBA_IMU_DATA_s;

typedef struct _AMBA_IMU_INFO_s_ {
    UINT8   GyroID;             /* gyro sensor id*/

    UINT8   GyroPwrGpio;        /* GPIO number controls gyro sensor power*/
    UINT8   GyroHpsGpio;        /* GPIO number controls gyro sensor hps*/
    UINT8   GyroIntGpio;        /* GPIO number connect to gyro sensor interrupt pin*/
    UINT8   GyroXChan;          /* gyro sensor x axis Channel*/
    UINT8   GyroYChan;          /* gyro sensor y axis Channel*/
    UINT8   GyroZChan;          /* gyro sensor z axis Channel*/
    UINT8   GyroTChan;          /* gyro sensor t axis Channel*/
    UINT8   GyroXReg;           /* gyro sensor x axis reg*/
    UINT8   GyroYReg;           /* gyro sensor y axis reg*/
    UINT8   GyroZReg;           /* gyro sensor z axis reg*/
    INT8    GyroXPolar;         /* gyro sensor x polarity*/
    INT8    GyroYPolar;         /* gyro sensor y polarity*/
    INT8    GyroZPolar;         /* gyro sensor z polarity*/
    UINT8   VolDivNum;          /* numerator of voltage divider*/
    UINT8   VolDivDen;          /* denominator of voltage divider*/

    UINT8   SensorInterface;    /* gyro sensor interface*/
    UINT8   SensorAxis;         /* gyro sensor axis*/
    UINT8   MaxRmsNoise;        /* gyro sensor rms noise level*/
    UINT8   AdcResolution;      /* gyro internal adc resolution, unit in bit(s)*/
    INT8    PhsDly;             /* gyro sensor phase delay, unit in ms*/
    UINT16  SamplingRate;       /* digital gyro internal sampling rate, unit in samples / sec*/
    UINT16  MaxSamplingRate;    /* max digital gyro internal sampling rate, unit in samples / sec*/
    UINT16  MaxBias;            /* max gyro sensor bias*/
    UINT16  MinBias;            /* min gyro sensor bias*/
    UINT16  MaxSense;           /* max gyro sensor sensitivity*/
    UINT16  MinSense;           /* min gyro sensor sensitivity*/
    UINT16  StartUpTime;        /* gyro sensor start-up time*/
    UINT16  FullScaleRange;     /* gyro full scale range*/
    UINT16  MaxSclk;            /* max serial clock for digital interface, unit in 100khz*/

    UINT16  AccXChan;           /* acc sensor x axis Channel*/
    UINT16  AccYChan;           /* acc sensor y axis Channel*/
    UINT16  AccZChan;           /* acc sensor z axis Channel*/
    INT8    AccXPolar;          /* acc sensor x polarity*/
    INT8    AccYPolar;          /* acc sensor y polarity*/
    INT8    AccZPolar;          /* acc sensor z polarity*/
    UINT16  AccMaxBias;
    UINT16  AccMinBias;
    UINT16  AccMaxSense;        /* max acc sensor sensitivity*/
    UINT16  AccMinSense;        /* min acc sensor sensitivity*/
    UINT8   AccMaxRmsNoise;     /* acc sensor rms noise level*/
    UINT16  LevelShift;
} AMBA_IMU_INFO_s;

typedef struct _AMBA_IMU_OBJ_s_ {
    int (*GetDeviceInfo)(AMBA_IMU_INFO_s *pImuInfo);
    int (*Open)(void);
    int (*Read)(UINT8 Type, AMBA_IMU_DATA_s* pData);
    int (*RegRead)(UINT16 Addr, UINT16 SubAddr, UINT16 *pInbuf, UINT16 NumBytes);
    int (*RegWrite)(UINT16 Addr, UINT16 SubAddr, UINT16 InData);
    int (*SetSampleRate)(UINT16 TimeInUs);
    int (*Close)(void);
    int (*IoCtrl)(UINT16 Id, UINT32 param1, UINT32 param2);
} AMBA_IMU_OBJ_s;

/*-----------------------------------------------------------------------------------------------*\
 * Defined in AmbaIMU.c
\*-----------------------------------------------------------------------------------------------*/
extern AMBA_IMU_OBJ_s *pAmbaImuObj[];

#define AmbaIMU_Hook(Chan,pImuObj)                          pAmbaImuObj[Chan] = pImuObj

#define AmbaIMU_GetDeviceInfo(Chan,pDevInfo)                ((pAmbaImuObj[Chan]->GetDeviceInfo == NULL) ? NG : pAmbaImuObj[Chan]->GetDeviceInfo(pDevInfo))
#define AmbaIMU_Open(Chan)                                  ((pAmbaImuObj[Chan]->Open == NULL) ? NG : pAmbaImuObj[Chan]->Open())
#define AmbaIMU_Read(Chan,Type,pData)                       ((pAmbaImuObj[Chan]->Read == NULL) ? NG : pAmbaImuObj[Chan]->Read(Type, pData))
#define AmbaIMU_RegRead(Chan,Addr,SubAddr,pInbuf,NumBytes)  ((pAmbaImuObj[Chan]->RegRead == NULL) ? NG : pAmbaImuObj[Chan]->RegRead(Addr, SubAddr, pInbuf, NumBytes))
#define AmbaIMU_RegWrite(Chan,Addr,SubAddr,InData)          ((pAmbaImuObj[Chan]->RegWrite == NULL) ? NG : pAmbaImuObj[Chan]->RegWrite(Addr, SubAddr, InData))
#define AmbaIMU_SetSampleRate(Chan,TimeInUs)                ((pAmbaImuObj[Chan]->SetSampleRate == NULL) ? NG :  pAmbaImuObj[Chan]->SetSampleRate(TimeInUs))
#define AmbaIMU_Close(Chan)                                 ((pAmbaImuObj[Chan]->Close == NULL) ? NG : pAmbaImuObj[Chan]->Close())

#endif /* _AMBA_IMU_H_ */
