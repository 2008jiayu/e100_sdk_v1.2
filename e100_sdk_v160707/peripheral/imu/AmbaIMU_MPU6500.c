/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaIMU_MPU6500.c
 *
 *  @Copyright      :: Copyright (C) 2013 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Control APIs of INVENSENSE MPU6500 6-axis Motion sensor with SPI interface
\*-------------------------------------------------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "AmbaDataType.h"
#include "AmbaKAL.h"
//#include "AmbaUserSysCtrl.h"
#include "AmbaSPI.h"
#include "AmbaGPIO.h"
#include "AmbaPrintk.h"
#include "AmbaIMU.h"
#include "AmbaIMU_MPU6500.h"

#define MPU6500_DEBUG
#ifdef MPU6500_DEBUG
#define MPU6500Print AmbaPrint
#else
#define MPU6500Print(...)
#endif

#define    GYRO_XOUT_POLARITY    1
#define    GYRO_YOUT_POLARITY   -1
#define    GYRO_XOUT_CHANNEL     1
#define    GYRO_YOUT_CHANNEL     0


static AMBA_SPI_CONFIG_s Mpu6500_Spi_Ctrl = {
    //.ChanNo        = AMBA_SPI_MASTER1,                   /* SPI Channel Number */
    .SlaveID       = 0,                                 /* Slave ID */
    .ProtocolMode  = AMBA_SPI_CPOL_HIGH_CPHA_HIGH,      /* SPI Protocol mode */
    .CsPolarity    = AMBA_SPI_CHIP_SELECT_ACTIVE_LOW,   /* Slave select polarity */
    .DataFrameSize = 8,                                 /* Data Frame Size in Bit */
    .BaudRate      = 1000000                            /* Transfer BaudRate in Hz */
};

static AMBA_SPI_CONFIG_s Mpu6500_Spi_Ctrl_Read = {
    //.ChanNo        = AMBA_SPI_MASTER1,                   /* SPI Channel Number */
    .SlaveID       = 0,                                 /* Slave ID */
    .ProtocolMode  = AMBA_SPI_CPOL_HIGH_CPHA_HIGH,      /* SPI Protocol mode */
    .CsPolarity    = AMBA_SPI_CHIP_SELECT_ACTIVE_LOW,   /* Slave select polarity */
    .DataFrameSize = 8,                                 /* Data Frame Size in Bit */
    .BaudRate      = 7000000                            /* Transfer BaudRate in Hz */
};

static inline UINT16 TransletData(UINT8 *pData){
    UINT16 TempH = 0x0000;
    UINT16 TempL = 0x0000;
    TempH = TempH | pData[0];
    TempL = TempL | pData[1];
    return ((TempH << 8) | (TempL));
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MPU6500_GetInfo
 *
 *  @Description:: Get Motion Sensor information
 *
 *  @Input      :: none
 *
 *  @Output     ::
 *      pImuInfo: pointer to Device Info
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/

static int MPU6500_GetInfo(AMBA_IMU_INFO_s *pImuInfo)
{
    static UINT8 flg_once = 0;

    if (!flg_once) {
        flg_once = 1;
        memset(&MPU6500DeviceInfo, 0, sizeof(AMBA_IMU_INFO_s));

        MPU6500DeviceInfo.GyroID = AMBA_IMU_COMBO;

#ifdef GYRO_PWR_GPIO
        MPU6500DeviceInfo.GyroPwrGpio = GYRO_PWR_GPIO;
#endif

#ifdef GYRO_INT_GPIO
        MPU6500DeviceInfo.GyroIntGpio = GYRO_INT_GPIO;
#endif

#ifndef GYRO_XOUT_POLARITY
        MPU6500DeviceInfo.GyroXPolar = 1;
#else
        MPU6500DeviceInfo.GyroXPolar = GYRO_XOUT_POLARITY;
        if (!MPU6500DeviceInfo.GyroXPolar)
            MPU6500DeviceInfo.GyroXPolar = -1;
#endif

#ifndef GYRO_YOUT_POLARITY
        MPU6500DeviceInfo.GyroYPolar = 1;
#else
        MPU6500DeviceInfo.GyroYPolar = GYRO_YOUT_POLARITY;
        if (!MPU6500DeviceInfo.GyroYPolar)
            MPU6500DeviceInfo.GyroYPolar = -1;
#endif

#ifndef GYRO_ZOUT_POLARITY
        MPU6500DeviceInfo.GyroZPolar = -1;
#else
        MPU6500DeviceInfo.GyroZPolar = GYRO_ZOUT_POLARITY;
        if (!MPU6500DeviceInfo.GyroZPolar)
            MPU6500DeviceInfo.GyroZPolar = -1;
#endif

        if (MPU6500DeviceInfo.GyroXPolar == MPU6500DeviceInfo.GyroYPolar) {
            if (MPU6500DeviceInfo.GyroXPolar > 0) {
                MPU6500DeviceInfo.AccXPolar = AMBA_IMU_NEGATIVE;
                MPU6500DeviceInfo.AccYPolar = AMBA_IMU_POSITIVE;
            } else {
                MPU6500DeviceInfo.AccXPolar = AMBA_IMU_POSITIVE;
                MPU6500DeviceInfo.AccYPolar = AMBA_IMU_NEGATIVE;
            }
        } else {
            if (MPU6500DeviceInfo.GyroXPolar > 0)
                MPU6500DeviceInfo.AccXPolar = MPU6500DeviceInfo.AccYPolar = AMBA_IMU_POSITIVE;
            else
                MPU6500DeviceInfo.AccXPolar = MPU6500DeviceInfo.AccYPolar = AMBA_IMU_NEGATIVE;
        }
        if (!MPU6500DeviceInfo.AccXPolar)
            MPU6500DeviceInfo.AccXPolar = -1;
        if (!MPU6500DeviceInfo.AccYPolar)
            MPU6500DeviceInfo.AccYPolar = -1;

#ifndef GYRO_XOUT_CHANNEL
        MPU6500DeviceInfo.GyroXChan = 0;
#else
        MPU6500DeviceInfo.GyroXChan = GYRO_XOUT_CHANNEL;
#endif

#ifndef GYRO_YOUT_CHANNEL
        MPU6500DeviceInfo.GyroYChan = 1;
#else
        MPU6500DeviceInfo.GyroYChan = GYRO_YOUT_CHANNEL;
#endif

        if ((MPU6500DeviceInfo.GyroXChan == AMBA_IMU_X) && (MPU6500DeviceInfo.GyroYChan == AMBA_IMU_Y)) {
            MPU6500DeviceInfo.AccXChan = AMBA_IMU_Y;
            MPU6500DeviceInfo.AccYChan = AMBA_IMU_X;
        } else {
            MPU6500DeviceInfo.AccXChan = AMBA_IMU_X;
            MPU6500DeviceInfo.AccYChan = AMBA_IMU_Y;
        }

#ifndef GYRO_VOL_DIVIDER_NUM
        MPU6500DeviceInfo.VolDivNum = 1;
#else
        MPU6500DeviceInfo.VolDivNum = GYRO_VOL_DIVIDER_NUM;
#endif

#ifndef GYRO_VOL_DIVIDER_DEN
        MPU6500DeviceInfo.VolDivDen = 1;
#else
        MPU6500DeviceInfo.VolDivDen = GYRO_VOL_DIVIDER_DEN;
#endif

        MPU6500DeviceInfo.SensorInterface   = AMBA_IMU_SPI;
        MPU6500DeviceInfo.SensorAxis        = AMBA_IMU_6AXIS;
        MPU6500DeviceInfo.MaxSense          = 135;                     /* LSB/(deg/sec)  */
        MPU6500DeviceInfo.MinSense          = 127;                     /* LSB/(deg/sec)  */
        MPU6500DeviceInfo.MaxBias           = 35388;//2620;            /* LSB            */
        MPU6500DeviceInfo.MinBias           = 30148;//-2620;           /* LSB            */
        MPU6500DeviceInfo.MaxRmsNoise       = 30;                      /* LSB            */
        MPU6500DeviceInfo.StartUpTime       = 100;                     /* ms             */
        MPU6500DeviceInfo.FullScaleRange    = 250;                     /* deg/sec        */
        MPU6500DeviceInfo.MaxSamplingRate   = 8000;                    /* sample/sec     */
        MPU6500DeviceInfo.AdcResolution     = 16;                      /* bits           */
        MPU6500DeviceInfo.PhsDly            = 3;                       /* ms             */
        MPU6500DeviceInfo.AccMaxSense       = 16876;                   /* LSB/g          */
        MPU6500DeviceInfo.AccMinSense       = 15892;                   /* LSB/g          */
        MPU6500DeviceInfo.AccMaxBias        = 34734;//1966;            /* LSB            */
        MPU6500DeviceInfo.AccMinBias        = 30802;//-1966;           /* LSB            */
        MPU6500DeviceInfo.SamplingRate      = MPU6500DeviceInfo.MaxSamplingRate / (SMPLRT_DIV + 1);
        MPU6500DeviceInfo.LevelShift        = 1 << (MPU6500DeviceInfo.AdcResolution - 1);
    }
    memcpy(pImuInfo, &MPU6500DeviceInfo, sizeof(AMBA_IMU_INFO_s));
    return OK;
}
/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MPU6500_Open
 *
 *  @Description:: Open Motion Sensor
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static int MPU6500_Open(void)
{
    static INT16 open_success = 0;
    UINT8 reg[2];
    UINT8 ID[2] = {0};

    if (open_success == -1) {
        MPU6500Print("ERROR: Gyro open fail!!!");
        return NG;
    }
    
    //--------- config GPIO    
    //AmbaGPIO_ConfigAltFunc(GPIO_PIN_7_SSI1_SCLK);
    //AmbaGPIO_ConfigAltFunc(GPIO_PIN_8_SSI1_TXD);
    //AmbaGPIO_ConfigAltFunc(GPIO_PIN_9_SSI1_RXD);
    //AmbaGPIO_ConfigAltFunc(GPIO_PIN_10_SSI1_EN0);
    //---------------------

    reg[0] = REG_USER_CTRL;
    reg[1] = FIFO_EN | I2C_IF_DIS;
    AmbaSPI_Transfer(AMBA_SPI_MASTER1, &Mpu6500_Spi_Ctrl, 2, reg, NULL, 1000);

    reg[0] = REG_ID | OP_READ;
    AmbaSPI_Transfer(AMBA_SPI_MASTER1, &Mpu6500_Spi_Ctrl, 2, reg, ID, 1000);
    MPU6500Print("Invensense gyro ID [0x%x], read gyro ID [0x%x]", WHO_AM_I_ID, ID[1]);
    if (ID[1] != WHO_AM_I_ID) {
        MPU6500Print("ERROR: Invensense gyro ID [0x%x], read gyro ID [0x%x]",
                     WHO_AM_I_ID, ID[1]);
        MPU6500Print("ERROR: Cannot open gyro with SPI!!!");
        open_success = -1;
        return NG;
    }

    //---------- power set------------
    reg[0] = REG_POW_MGM1;
    reg[1] = TEMP_DIS | CLK_SEL;
    AmbaSPI_Transfer(AMBA_SPI_MASTER1, &Mpu6500_Spi_Ctrl, 2, reg, NULL, 1000);
    //--------------------------------

    reg[0] = REG_DIV;
    reg[1] = SMPLRT_DIV;
    AmbaSPI_Transfer(AMBA_SPI_MASTER1, &Mpu6500_Spi_Ctrl, 2, reg, NULL, 1000);

    reg[0] = REG_CFG;
    reg[1] = EXT_SYNC_SET | DLPF_CFG;
    AmbaSPI_Transfer(AMBA_SPI_MASTER1, &Mpu6500_Spi_Ctrl, 2, reg, NULL, 1000);

    reg[0] = REG_GYRO_CFG;
    reg[1] = FS_SEL;
    AmbaSPI_Transfer(AMBA_SPI_MASTER1, &Mpu6500_Spi_Ctrl, 2, reg, NULL, 1000);
    reg[0] = REG_ACC_CFG;
    reg[1] = AFS_SEL;
    AmbaSPI_Transfer(AMBA_SPI_MASTER1, &Mpu6500_Spi_Ctrl, 2, reg, NULL, 1000);

    reg[0] = REG_FIFO_EN;
    reg[1] = TEMP_FIFO_EN | XG_FIFO_EN | YG_FIFO_EN | ZG_FIFO_EN | ACCEL_FIFO_EN;
    AmbaSPI_Transfer(AMBA_SPI_MASTER1, &Mpu6500_Spi_Ctrl, 2, reg, NULL, 1000);
    /*
        reg[0] = REG_POW_MGM1;
        reg[1] = TEMP_DIS | CLK_SEL;
        AmbaSPI_Transfer(AMBA_SPI_MASTER1, &Mpu6500_Spi_Ctrl, 2, reg, NULL, 1000);
    */
    reg[0] = REG_INT_CFG;
    if (MPU6500DeviceInfo.GyroIntGpio)
        reg[1] = INT_LVL | INT_OPEN | INT_ANYRD_2CLEAR | LATCH_INT_EN | FSYNC_INT_LEVEL | FSYNC_INT_MODE_EN;
    else
        reg[1] = LATCH_INT_EN;
    AmbaSPI_Transfer(AMBA_SPI_MASTER1, &Mpu6500_Spi_Ctrl, 2, reg, NULL, 1000);

    reg[0] = REG_INT_EN;
    if (MPU6500DeviceInfo.GyroIntGpio) {
        reg[1] = DATA_RDY_EN;
        AmbaSPI_Transfer(AMBA_SPI_MASTER1, &Mpu6500_Spi_Ctrl, 2, reg, NULL, 1000);
    }
    //dly_tsk(50);

    return OK;
}
/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MPU6500_Read
 *
 *  @Description:: Read Motion Sensor Data
 *
 *  @Input      ::
 *      Type:   Decide Gyro or Acc data to read
 *
 *  @Output     ::
 *      pData:  Motion Sensor Data Structure
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static int MPU6500_Read(UINT8 Type, AMBA_IMU_DATA_s* pData)
{
    UINT8 reg;
    UINT8 data[15];

    if (Type == AMBA_IMU_GYRO) {
        /*User get Gyro data*/
        reg = REG_GYRO_XOUT_H | OP_READ;
        AmbaSPI_Transfer(AMBA_SPI_MASTER1, &Mpu6500_Spi_Ctrl_Read, 7, &reg, data, 1000);

        if ((MPU6500DeviceInfo.GyroXChan == AMBA_IMU_X) && (MPU6500DeviceInfo.GyroYChan == AMBA_IMU_Y)) {
            pData->GyroXData = TransletData(&data[1]);
            pData->GyroYData = TransletData(&data[3]);
        } else {
            pData->GyroYData = TransletData(&data[1]);
            pData->GyroXData = TransletData(&data[3]);
        }
        pData->GyroZData = TransletData(&data[5]);

        pData->GyroXData += MPU6500DeviceInfo.LevelShift;
        pData->GyroYData += MPU6500DeviceInfo.LevelShift;
        pData->GyroZData += MPU6500DeviceInfo.LevelShift;
    } else if (Type == AMBA_IMU_ACCMETER) {
        /*User get G-Sensor data*/
        reg = REG_ACCEL_XOUT_H | OP_READ;
        AmbaSPI_Transfer(AMBA_SPI_MASTER1, &Mpu6500_Spi_Ctrl_Read, 7, &reg, data, 1000);

        if ((MPU6500DeviceInfo.AccXChan == AMBA_IMU_Y) && (MPU6500DeviceInfo.AccYChan == AMBA_IMU_X)) {
            pData->AccXData = TransletData(&data[3]);
            pData->AccYData = TransletData(&data[1]);
        } else {
            pData->AccXData = TransletData(&data[1]);
            pData->AccYData = TransletData(&data[3]);
        }
        pData->AccZData = TransletData(&data[5]);

        pData->AccXData += MPU6500DeviceInfo.LevelShift;
        pData->AccYData += MPU6500DeviceInfo.LevelShift;
        pData->AccZData += MPU6500DeviceInfo.LevelShift;
    } else if (Type == AMBA_IMU_COMBO) {
        /*User get ALL data*/
        reg = REG_ACCEL_XOUT_H | OP_READ;
        AmbaSPI_Transfer(AMBA_SPI_MASTER1, &Mpu6500_Spi_Ctrl_Read, 15, &reg, data, 1000);

        if ((MPU6500DeviceInfo.AccXChan == AMBA_IMU_Y) && (MPU6500DeviceInfo.AccYChan == AMBA_IMU_X)) {
            pData->AccXData = TransletData(&data[3]);
            pData->AccYData = TransletData(&data[1]);
        } else {
            pData->AccXData = TransletData(&data[1]);
            pData->AccYData = TransletData(&data[3]);
        }
        pData->AccZData = TransletData(&data[5]);

        pData->AccXData += MPU6500DeviceInfo.LevelShift;
        pData->AccYData += MPU6500DeviceInfo.LevelShift;
        pData->AccZData += MPU6500DeviceInfo.LevelShift;

        /* Output Temperature data */
        pData->TemperatureData = TransletData(&data[7]);
        pData->TemperatureData += MPU6500DeviceInfo.LevelShift;

        if ((MPU6500DeviceInfo.GyroXChan == AMBA_IMU_X) && (MPU6500DeviceInfo.GyroYChan == AMBA_IMU_Y)) {
            pData->GyroXData = TransletData(&data[9]);
            pData->GyroYData = TransletData(&data[11]);
        } else {
            pData->GyroYData = TransletData(&data[9]);
            pData->GyroXData = TransletData(&data[11]);
        }
        pData->GyroZData = TransletData(&data[13]);

        pData->GyroXData += MPU6500DeviceInfo.LevelShift;
        pData->GyroYData += MPU6500DeviceInfo.LevelShift;
        pData->GyroZData += MPU6500DeviceInfo.LevelShift;
    }
    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MPU6500_RegRead
 *
 *  @Description:: MPU6500 Register Read Function
 *
 *  @Input      ::
 *      Addr:       Register Address
 *      SubAddr:    NULL
 *      NumBytes:   The number of data read
 *
 *  @Output     ::
 *      pInbuf :    pointer to user buffer
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static int MPU6500_RegRead(UINT16 Addr, UINT16 SubAddr, UINT16 *pInbuf, UINT16 NumBytes)
{
    UINT8 reg;
    UINT8 data_temp[3];

    if (NumBytes > 2)
        return NG;

    reg = Addr | OP_READ;
    AmbaSPI_Transfer(AMBA_SPI_MASTER1, &Mpu6500_Spi_Ctrl, NumBytes + 1, &reg, data_temp, 1000);

    pInbuf[0] = data_temp[1];
    if (NumBytes == 2)
        pInbuf[1] = data_temp[2];

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MPU6500_RegWrite
 *
 *  @Description:: MPU6500 Register Write Function
 *
 *  @Input      ::
 *      Addr:       Register Address
 *      SubAddr:    NULL
 *      InData:     Data user want to write
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static int MPU6500_RegWrite(UINT16 Addr, UINT16 SubAddr, UINT16 InData)
{
    UINT8 reg[2];

    reg[0] = Addr;
    reg[1] = InData;
    AmbaSPI_Transfer(AMBA_SPI_MASTER1, &Mpu6500_Spi_Ctrl, 2, reg, NULL, 1000);

    return OK;
}
/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MPU6500_SetSampleRate
 *
 *  @Description:: Set Motion Sensor Sampling Rate
 *
 *  @Input      ::
 *      TimeInUs: Desired sampling period with unit us
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static int MPU6500_SetSampleRate(UINT16 TimeInUs)
{
    UINT8 reg[2], rate;

    if (TimeInUs == 0) {
        /*use default value*/
        reg[1] = SMPLRT_DIV;
        rate = MPU6500DeviceInfo.SamplingRate / (SMPLRT_DIV + 1);
    } else {
        if ((1000000 / TimeInUs) > MPU6500DeviceInfo.MaxSamplingRate)
            return (-1);
        else {
            reg[1] = MPU6500DeviceInfo.MaxSamplingRate * TimeInUs / 1000000;
            reg[1] -= 1;
            rate = 1000000 / TimeInUs;
        }
    }

    reg[0] = REG_DIV;

    AmbaSPI_Transfer(AMBA_SPI_MASTER1, &Mpu6500_Spi_Ctrl, 2, &reg, NULL, 1000);
    MPU6500DeviceInfo.SamplingRate = rate;

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MPU6500_Close
 *
 *  @Description::
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static int MPU6500_Close(void)
{
    return OK;
}

/*-----------------------------------------------------------------------------------------------*/
AMBA_IMU_OBJ_s AmbaIMU_MPU6500Obj = {
    .GetDeviceInfo            = MPU6500_GetInfo,
    .Open                     = MPU6500_Open,
    .Read                     = MPU6500_Read,
    .RegRead                  = MPU6500_RegRead,
    .RegWrite                 = MPU6500_RegWrite,
    .SetSampleRate            = MPU6500_SetSampleRate,
    .Close                    = MPU6500_Close,
};
