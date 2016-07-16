/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaIMU_DMY0000.c
 *
 *  @Copyright      :: Copyright (C) 2013 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Control APIs of Ambarella dummy Motion sensor with SPI interface
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
#include "AmbaIMU_DMY0000.h"
#include "AmbaSysTimer.h"

#define PI	3.14159

#define DMY0000_DEBUG
#ifdef DMY0000_DEBUG
#define DMY0000Print AmbaPrint
#else
#define DMY0000Print(...)
#endif

#define    GYRO_XOUT_POLARITY    1
#define    GYRO_YOUT_POLARITY   -1
#define    GYRO_XOUT_CHANNEL     1
#define    GYRO_YOUT_CHANNEL     0

//#define     AMBA_SPI_DUMMY_CHANNEL     AMBA_SPI_MASTER1

#ifdef      AMBA_SPI_DUMMY_CHANNEL
#define     AmbaSPI_Transfer_Dummy  AmbaSPI_Transfer
#else
#define     AmbaSPI_Transfer_Dummy(...)
#endif

#ifdef      AMBA_SPI_DUMMY_CHANNEL
static AMBA_SPI_CONFIG_s Dmy0000_Spi_Ctrl = {
    //.ChanNo        = AMBA_SPI_DUMMY_CHANNEL,                   /* SPI Channel Number */
    .SlaveID       = 0,                                 /* Slave ID */
    .ProtocolMode  = AMBA_SPI_CPOL_HIGH_CPHA_HIGH,      /* SPI Protocol mode */
    .CsPolarity    = AMBA_SPI_CHIP_SELECT_ACTIVE_LOW,   /* Slave select polarity */
    .DataFrameSize = 8,                                 /* Data Frame Size in Bit */
    .BaudRate      = 1000000                            /* Transfer BaudRate in Hz */
};

static AMBA_SPI_CONFIG_s MpuDmy0000_Spi_Ctrl_Read = {
    //.ChanNo        = AMBA_SPI_DUMMY_CHANNEL,                   /* SPI Channel Number */
    .SlaveID       = 0,                                 /* Slave ID */
    .ProtocolMode  = AMBA_SPI_CPOL_HIGH_CPHA_HIGH,      /* SPI Protocol mode */
    .CsPolarity    = AMBA_SPI_CHIP_SELECT_ACTIVE_LOW,   /* Slave select polarity */
    .DataFrameSize = 8,                                 /* Data Frame Size in Bit */
    .BaudRate      = 7000000                            /* Transfer BaudRate in Hz */
};
#endif

static inline UINT16 TransletData(UINT8 *pData){
    UINT16 TempH = 0x0000;
    UINT16 TempL = 0x0000;
    TempH = TempH | pData[0];
    TempL = TempL | pData[1];
    return ((TempH << 8) | (TempL));
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: DMY0000_GetInfo
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

static int DMY0000_GetInfo(AMBA_IMU_INFO_s *pImuInfo)
{
    static UINT8 flg_once = 0;

    if (!flg_once) {
        flg_once = 1;
        memset(&DMY0000DeviceInfo, 0, sizeof(AMBA_IMU_INFO_s));

        DMY0000DeviceInfo.GyroID = AMBA_IMU_COMBO;

#ifdef GYRO_PWR_GPIO
        DMY0000DeviceInfo.GyroPwrGpio = GYRO_PWR_GPIO;
#endif

#ifdef GYRO_INT_GPIO
        DMY0000DeviceInfo.GyroIntGpio = GYRO_INT_GPIO;
#endif

#ifndef GYRO_XOUT_POLARITY
        DMY0000DeviceInfo.GyroXPolar = 1;
#else
        DMY0000DeviceInfo.GyroXPolar = GYRO_XOUT_POLARITY;
        if (!DMY0000DeviceInfo.GyroXPolar)
            DMY0000DeviceInfo.GyroXPolar = -1;
#endif

#ifndef GYRO_YOUT_POLARITY
        DMY0000DeviceInfo.GyroYPolar = 1;
#else
        DMY0000DeviceInfo.GyroYPolar = GYRO_YOUT_POLARITY;
        if (!DMY0000DeviceInfo.GyroYPolar)
            DMY0000DeviceInfo.GyroYPolar = -1;
#endif

#ifndef GYRO_ZOUT_POLARITY
        DMY0000DeviceInfo.GyroZPolar = -1;
#else
        DMY0000DeviceInfo.GyroZPolar = GYRO_ZOUT_POLARITY;
        if (!DMY0000DeviceInfo.GyroZPolar)
            DMY0000DeviceInfo.GyroZPolar = -1;
#endif

        if (DMY0000DeviceInfo.GyroXPolar == DMY0000DeviceInfo.GyroYPolar) {
            if (DMY0000DeviceInfo.GyroXPolar > 0) {
                DMY0000DeviceInfo.AccXPolar = AMBA_IMU_NEGATIVE;
                DMY0000DeviceInfo.AccYPolar = AMBA_IMU_POSITIVE;
            } else {
                DMY0000DeviceInfo.AccXPolar = AMBA_IMU_POSITIVE;
                DMY0000DeviceInfo.AccYPolar = AMBA_IMU_NEGATIVE;
            }
        } else {
            if (DMY0000DeviceInfo.GyroXPolar > 0)
                DMY0000DeviceInfo.AccXPolar = DMY0000DeviceInfo.AccYPolar = AMBA_IMU_POSITIVE;
            else
                DMY0000DeviceInfo.AccXPolar = DMY0000DeviceInfo.AccYPolar = AMBA_IMU_NEGATIVE;
        }
        if (!DMY0000DeviceInfo.AccXPolar)
            DMY0000DeviceInfo.AccXPolar = -1;
        if (!DMY0000DeviceInfo.AccYPolar)
            DMY0000DeviceInfo.AccYPolar = -1;

#ifndef GYRO_XOUT_CHANNEL
        DMY0000DeviceInfo.GyroXChan = 0;
#else
        DMY0000DeviceInfo.GyroXChan = GYRO_XOUT_CHANNEL;
#endif

#ifndef GYRO_YOUT_CHANNEL
        DMY0000DeviceInfo.GyroYChan = 1;
#else
        DMY0000DeviceInfo.GyroYChan = GYRO_YOUT_CHANNEL;
#endif

        if ((DMY0000DeviceInfo.GyroXChan == AMBA_IMU_X) && (DMY0000DeviceInfo.GyroYChan == AMBA_IMU_Y)) {
            DMY0000DeviceInfo.AccXChan = AMBA_IMU_Y;
            DMY0000DeviceInfo.AccYChan = AMBA_IMU_X;
        } else {
            DMY0000DeviceInfo.AccXChan = AMBA_IMU_X;
            DMY0000DeviceInfo.AccYChan = AMBA_IMU_Y;
        }

#ifndef GYRO_VOL_DIVIDER_NUM
        DMY0000DeviceInfo.VolDivNum = 1;
#else
        DMY0000DeviceInfo.VolDivNum = GYRO_VOL_DIVIDER_NUM;
#endif

#ifndef GYRO_VOL_DIVIDER_DEN
        DMY0000DeviceInfo.VolDivDen = 1;
#else
        DMY0000DeviceInfo.VolDivDen = GYRO_VOL_DIVIDER_DEN;
#endif

        DMY0000DeviceInfo.SensorInterface   = AMBA_IMU_SPI;
        DMY0000DeviceInfo.SensorAxis        = AMBA_IMU_6AXIS;
        DMY0000DeviceInfo.MaxSense          = 135;                     /* LSB/(deg/sec)  */
        DMY0000DeviceInfo.MinSense          = 127;                     /* LSB/(deg/sec)  */
        DMY0000DeviceInfo.MaxBias           = 35388;//2620;            /* LSB            */
        DMY0000DeviceInfo.MinBias           = 30148;//-2620;           /* LSB            */
        DMY0000DeviceInfo.MaxRmsNoise       = 30;                      /* LSB            */
        DMY0000DeviceInfo.StartUpTime       = 100;                     /* ms             */
        DMY0000DeviceInfo.FullScaleRange    = 250;                     /* deg/sec        */
        DMY0000DeviceInfo.MaxSamplingRate   = 8000;                    /* sample/sec     */
        DMY0000DeviceInfo.AdcResolution     = 16;                      /* bits           */
        DMY0000DeviceInfo.PhsDly            = 3;                       /* ms             */
        DMY0000DeviceInfo.AccMaxSense       = 16876;                   /* LSB/g          */
        DMY0000DeviceInfo.AccMinSense       = 15892;                   /* LSB/g          */
        DMY0000DeviceInfo.AccMaxBias        = 34734;//1966;            /* LSB            */
        DMY0000DeviceInfo.AccMinBias        = 30802;//-1966;           /* LSB            */
        DMY0000DeviceInfo.SamplingRate      = DMY0000DeviceInfo.MaxSamplingRate / (SMPLRT_DIV + 1);
        DMY0000DeviceInfo.LevelShift        = 1 << (DMY0000DeviceInfo.AdcResolution - 1);
    }
    memcpy(pImuInfo, &DMY0000DeviceInfo, sizeof(AMBA_IMU_INFO_s));
    return OK;
}
/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: DMY0000_Open
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
#ifdef AMBA_SPI_DUMMY_CHANNEL
static int DMY0000_Open(void)
{
    static INT16 open_success = 0;
    UINT8 reg[2];
    UINT8 ID[2] = {0};

    if (open_success == -1) {
        DMY0000Print("ERROR: Gyro open fail!!!");
        return NG;
    }
    
    reg[0] = REG_USER_CTRL;
    reg[1] = FIFO_EN | I2C_IF_DIS;
    AmbaSPI_Transfer_Dummy(AMBA_SPI_DUMMY_CHANNEL, &Dmy0000_Spi_Ctrl, 2, reg, NULL, 1000);

    reg[0] = REG_ID | OP_READ;
    AmbaSPI_Transfer_Dummy(AMBA_SPI_DUMMY_CHANNEL, &Dmy0000_Spi_Ctrl, 2, reg, ID, 1000);
    DMY0000Print("Invensense gyro ID [0x%x], read gyro ID [0x%x]", WHO_AM_I_ID, ID[1]);
    if (ID[1] != WHO_AM_I_ID) {
        DMY0000Print("ERROR: Invensense gyro ID [0x%x], read gyro ID [0x%x]",
                     WHO_AM_I_ID, ID[1]);
        DMY0000Print("ERROR: Cannot open gyro with SPI!!!");
        open_success = -1;
        return NG;
    }

    //---------- power set------------
    reg[0] = REG_POW_MGM1;
    reg[1] = TEMP_DIS | CLK_SEL;
    AmbaSPI_Transfer_Dummy(AMBA_SPI_DUMMY_CHANNEL, &Dmy0000_Spi_Ctrl, 2, reg, NULL, 1000);
    //--------------------------------

    reg[0] = REG_DIV;
    reg[1] = SMPLRT_DIV;
    AmbaSPI_Transfer_Dummy(AMBA_SPI_DUMMY_CHANNEL, &Dmy0000_Spi_Ctrl, 2, reg, NULL, 1000);

    reg[0] = REG_CFG;
    reg[1] = EXT_SYNC_SET | DLPF_CFG;
    AmbaSPI_Transfer_Dummy(AMBA_SPI_DUMMY_CHANNEL, &Dmy0000_Spi_Ctrl, 2, reg, NULL, 1000);

    reg[0] = REG_GYRO_CFG;
    reg[1] = FS_SEL;
    AmbaSPI_Transfer_Dummy(AMBA_SPI_DUMMY_CHANNEL, &Dmy0000_Spi_Ctrl, 2, reg, NULL, 1000);
    reg[0] = REG_ACC_CFG;
    reg[1] = AFS_SEL;
    AmbaSPI_Transfer_Dummy(AMBA_SPI_DUMMY_CHANNEL, &Dmy0000_Spi_Ctrl, 2, reg, NULL, 1000);

    reg[0] = REG_FIFO_EN;
    reg[1] = TEMP_FIFO_EN | XG_FIFO_EN | YG_FIFO_EN | ZG_FIFO_EN | ACCEL_FIFO_EN;
    AmbaSPI_Transfer_Dummy(AMBA_SPI_DUMMY_CHANNEL, &Dmy0000_Spi_Ctrl, 2, reg, NULL, 1000);
    /*
        reg[0] = REG_POW_MGM1;
        reg[1] = TEMP_DIS | CLK_SEL;
        AmbaSPI_Transfer_Dummy(AMBA_SPI_DUMMY_CHANNEL, &Dmy0000_Spi_Ctrl, 2, reg, NULL, 1000);
    */
    reg[0] = REG_INT_CFG;
    if (DMY0000DeviceInfo.GyroIntGpio)
        reg[1] = INT_LVL | INT_OPEN | INT_ANYRD_2CLEAR | LATCH_INT_EN | FSYNC_INT_LEVEL | FSYNC_INT_MODE_EN;
    else
        reg[1] = LATCH_INT_EN;
    AmbaSPI_Transfer_Dummy(AMBA_SPI_DUMMY_CHANNEL, &Dmy0000_Spi_Ctrl, 2, reg, NULL, 1000);

    reg[0] = REG_INT_EN;
    if (DMY0000DeviceInfo.GyroIntGpio) {
        reg[1] = DATA_RDY_EN;
        AmbaSPI_Transfer_Dummy(AMBA_SPI_DUMMY_CHANNEL, &Dmy0000_Spi_Ctrl, 2, reg, NULL, 1000);
    }
    //dly_tsk(50);

    return OK;
}

#else

static int DMY0000_Open(void)
{
    AmbaKAL_TaskSleep(50);
    return OK;
}
#endif
/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: DMY0000_Read
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
static int DMY0000_Read(UINT8 Type, AMBA_IMU_DATA_s* pData)
{
    UINT8 reg;
    UINT8 data[15];

    static UINT8 Flag_Once = 0;
    INT16 Tmp;
    static UINT32 Tmp_Amp;
    static UINT32 AmbaSysTimer = 0;
    AMBA_IMU_DATA_s CalData = {0};
    
    if (Flag_Once == 0) {
        Flag_Once = 1;
        if (Ambarella_Gyro_Freq == 0) 
            Tmp_Amp = 0;
        else {
            if ((Ambarella_Gyro_Amp * PI * Ambarella_Gyro_Freq * (DMY0000DeviceInfo.MinSense + DMY0000DeviceInfo.MaxSense) / 2) 
                > (1 << (DMY0000DeviceInfo.AdcResolution - 1)))
                Tmp_Amp = (1 << (DMY0000DeviceInfo.AdcResolution - 1)) / ((DMY0000DeviceInfo.MinSense + DMY0000DeviceInfo.MaxSense) / 2);        
            else                                                                                                             
                Tmp_Amp = Ambarella_Gyro_Amp * PI * Ambarella_Gyro_Freq;        
        }
    }  
    AmbaSysTimer = AmbaSysTimer_GetTickCount();    
    Tmp = Tmp_Amp * (DMY0000DeviceInfo.MinSense + DMY0000DeviceInfo.MaxSense) / 2;
    Tmp = sin(2.0 * PI * AmbaSysTimer * Ambarella_Gyro_Freq / 1000.0) * Tmp;
    if (Tmp_Amp == 0) {
        pData->AccZData = 0;
        pData->AccZData += DMY0000DeviceInfo.LevelShift;
        pData->AccYData = 0;
        pData->AccYData += DMY0000DeviceInfo.LevelShift;
        pData->AccXData = 0;
        pData->AccXData += DMY0000DeviceInfo.LevelShift;
        pData->GyroZData = 0;
        pData->GyroZData += DMY0000DeviceInfo.LevelShift;
        pData->GyroYData = 0;
        pData->GyroYData += DMY0000DeviceInfo.LevelShift;
        pData->GyroXData = 0;
        pData->GyroXData += DMY0000DeviceInfo.LevelShift;      
    }                        
    else {
        if (Ambarella_Gyro_Mode == 0) {
          CalData.AccZData = Tmp;
          CalData.AccZData += DMY0000DeviceInfo.LevelShift;
          CalData.AccYData = Tmp;
          CalData.AccYData += DMY0000DeviceInfo.LevelShift;
          CalData.AccXData = Tmp;
          CalData.AccXData += DMY0000DeviceInfo.LevelShift;
          CalData.GyroZData = Tmp;
          CalData.GyroZData += DMY0000DeviceInfo.LevelShift;
          CalData.GyroYData = Tmp;
          CalData.GyroYData += DMY0000DeviceInfo.LevelShift;
          CalData.GyroXData = Tmp;
          CalData.GyroXData += DMY0000DeviceInfo.LevelShift;
        }
        else if (Ambarella_Gyro_Mode == 1) {
          CalData.AccZData = 0;
          CalData.AccZData += DMY0000DeviceInfo.LevelShift;
          CalData.AccYData = 0;
          CalData.AccYData += DMY0000DeviceInfo.LevelShift;
          CalData.AccXData = Tmp;
          CalData.AccXData += DMY0000DeviceInfo.LevelShift;
          CalData.GyroZData = 0;
          CalData.GyroZData += DMY0000DeviceInfo.LevelShift;
          CalData.GyroYData = 0;
          CalData.GyroYData += DMY0000DeviceInfo.LevelShift;
          CalData.GyroXData = Tmp;
          CalData.GyroXData += DMY0000DeviceInfo.LevelShift;      
        }
        else if (Ambarella_Gyro_Mode == 2) {
          CalData.AccZData = 0;
          CalData.AccZData += DMY0000DeviceInfo.LevelShift;
          CalData.AccYData = Tmp;
          CalData.AccYData += DMY0000DeviceInfo.LevelShift;
          CalData.AccXData = 0;
          CalData.AccXData += DMY0000DeviceInfo.LevelShift;
          CalData.GyroZData = 0;
          CalData.GyroZData += DMY0000DeviceInfo.LevelShift;
          CalData.GyroYData = Tmp;
          CalData.GyroYData += DMY0000DeviceInfo.LevelShift;
          CalData.GyroXData = 0;
          CalData.GyroXData += DMY0000DeviceInfo.LevelShift;
        }
        else if (Ambarella_Gyro_Mode == 3) {
          CalData.AccZData = Tmp;
          CalData.AccZData += DMY0000DeviceInfo.LevelShift;
          CalData.AccYData = 0;
          CalData.AccYData += DMY0000DeviceInfo.LevelShift;
          CalData.AccXData = 0;
          CalData.AccXData += DMY0000DeviceInfo.LevelShift;
          CalData.GyroZData = Tmp;
          CalData.GyroZData += DMY0000DeviceInfo.LevelShift;
          CalData.GyroYData = 0;
          CalData.GyroYData += DMY0000DeviceInfo.LevelShift;
          CalData.GyroXData = 0;
          CalData.GyroXData += DMY0000DeviceInfo.LevelShift;
        }
        else {
          CalData.AccZData = 0;
          CalData.AccZData += DMY0000DeviceInfo.LevelShift;
          CalData.AccYData = Tmp / 1.414;
          CalData.AccYData += DMY0000DeviceInfo.LevelShift;
          CalData.AccXData = Tmp / 1.414;
          CalData.AccXData += DMY0000DeviceInfo.LevelShift;
          CalData.GyroZData = 0;
          CalData.GyroZData += DMY0000DeviceInfo.LevelShift;
          CalData.GyroYData = Tmp / 1.414;
          CalData.GyroYData += DMY0000DeviceInfo.LevelShift;
          CalData.GyroXData = Tmp / 1.414;
          CalData.GyroXData += DMY0000DeviceInfo.LevelShift;
        }
    }
    if (Type == AMBA_IMU_GYRO) {
        /*User get Gyro data*/
        reg = REG_GYRO_XOUT_H | OP_READ;
        AmbaSPI_Transfer_Dummy(AMBA_SPI_DUMMY_CHANNEL, &MpuDmy0000_Spi_Ctrl_Read, 7, &reg, data, 1000);

        if ((DMY0000DeviceInfo.GyroXChan == AMBA_IMU_X) && (DMY0000DeviceInfo.GyroYChan == AMBA_IMU_Y)) {
            pData->GyroXData = CalData.GyroXData;
            pData->GyroYData = CalData.GyroYData;
        } else {
            pData->GyroYData = CalData.GyroXData;
            pData->GyroXData = CalData.GyroYData;
        }
        pData->GyroZData = CalData.GyroZData;

    } else if (Type == AMBA_IMU_ACCMETER) {
        /*User get G-Sensor data*/
        reg = REG_ACCEL_XOUT_H | OP_READ;
        AmbaSPI_Transfer_Dummy(AMBA_SPI_DUMMY_CHANNEL, &MpuDmy0000_Spi_Ctrl_Read, 7, &reg, data, 1000);

        if ((DMY0000DeviceInfo.AccXChan == AMBA_IMU_Y) && (DMY0000DeviceInfo.AccYChan == AMBA_IMU_X)) {
            pData->AccXData = CalData.AccYData;
            pData->AccYData = CalData.AccXData;
        } else {
            pData->AccXData = CalData.AccXData;
            pData->AccYData = CalData.AccYData;
        }
        pData->AccZData = CalData.AccZData;

    } else if (Type == AMBA_IMU_COMBO) {
        /*User get ALL data*/
        reg = REG_ACCEL_XOUT_H | OP_READ;
        AmbaSPI_Transfer_Dummy(AMBA_SPI_DUMMY_CHANNEL, &MpuDmy0000_Spi_Ctrl_Read, 15, &reg, data, 1000);

        if ((DMY0000DeviceInfo.AccXChan == AMBA_IMU_Y) && (DMY0000DeviceInfo.AccYChan == AMBA_IMU_X)) {
            pData->AccXData = CalData.AccYData;
            pData->AccYData = CalData.AccXData;
        } else {
            pData->AccXData = CalData.AccXData;
            pData->AccYData = CalData.AccYData;
        }
        pData->AccZData = CalData.AccZData;


        /* Output Temperature data */
        pData->TemperatureData = TransletData(&data[7]);
        pData->TemperatureData += DMY0000DeviceInfo.LevelShift;

        if ((DMY0000DeviceInfo.GyroXChan == AMBA_IMU_X) && (DMY0000DeviceInfo.GyroYChan == AMBA_IMU_Y)) {
            pData->GyroXData = CalData.GyroXData;
            pData->GyroYData = CalData.GyroYData;
        } else {
            pData->GyroYData = CalData.GyroXData;
            pData->GyroXData = CalData.GyroYData;
        }
        pData->GyroZData = CalData.GyroZData;

    }
    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: DMY0000_RegRead
 *
 *  @Description:: DMY0000 Register Read Function
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
static int DMY0000_RegRead(UINT16 Addr, UINT16 SubAddr, UINT16 *pInbuf, UINT16 NumBytes)
{
    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: DMY0000_RegWrite
 *
 *  @Description:: DMY0000 Register Write Function
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
static int DMY0000_RegWrite(UINT16 Addr, UINT16 SubAddr, UINT16 InData)
{
    return OK;
}
/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: DMY0000_SetSampleRate
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
static int DMY0000_SetSampleRate(UINT16 TimeInUs)
{
    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: DMY0000_Close
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
static int DMY0000_Close(void)
{
    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: DMY0000_IoCtrl
 *
 *  @Description::
 *
 *  @Input      :: Test command id and parameters (param1, param2)
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static int DMY0000_IoCtrl(UINT16 Id, UINT32 param1, UINT32 param2)
{
    switch (Id) {
    
    case 1:
        Ambarella_Gyro_Mode = param1;
    break;
    
    default:
        AmbaPrint("invalid t cmd id...");
    break;
    }
    
    return OK;
}

/*-----------------------------------------------------------------------------------------------*/
AMBA_IMU_OBJ_s AmbaIMU_DMY0000Obj = {
    .GetDeviceInfo            = DMY0000_GetInfo,
    .Open                     = DMY0000_Open,
    .Read                     = DMY0000_Read,
    .RegRead                  = DMY0000_RegRead,
    .RegWrite                 = DMY0000_RegWrite,
    .SetSampleRate            = DMY0000_SetSampleRate,
    .Close                    = DMY0000_Close,
    .IoCtrl                   = DMY0000_IoCtrl,
};
