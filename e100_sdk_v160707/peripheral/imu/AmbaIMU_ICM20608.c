/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaIMU_ICM20608.c
 *
 *  @Copyright      :: Copyright (C) 2013 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Control APIs of INVENSENSE ICM20608 6-axis Motion sensor with SPI interface
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
#include "AmbaIMU_ICM20608.h"

#define ICM20608_DEBUG
#ifdef ICM20608_DEBUG
#define ICM20608Print AmbaPrint
#else
#define ICM20608Print(...)
#endif

#define    GYRO_XOUT_POLARITY    1
#define    GYRO_YOUT_POLARITY    1


static AMBA_SPI_CONFIG_s Icm20608_Spi_Ctrl = {
    //.ChanNo        = AMBA_SPI_MASTER1,                   /* SPI Channel Number */
    .SlaveID       = 0,                                 /* Slave ID */
    .ProtocolMode  = AMBA_SPI_CPOL_HIGH_CPHA_HIGH,      /* SPI Protocol mode */
    .CsPolarity    = AMBA_SPI_CHIP_SELECT_ACTIVE_LOW,   /* Slave select polarity */
    .DataFrameSize = 8,                                 /* Data Frame Size in Bit */
    .BaudRate      = 1000000                            /* Transfer BaudRate in Hz */
};

static AMBA_SPI_CONFIG_s Icm20608_Spi_Ctrl_Read = {
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
 *  @RoutineName:: ICM20608_GetInfo
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

static int ICM20608_GetInfo(AMBA_IMU_INFO_s *pImuInfo)
{
    static UINT8 flg_once = 0;

    if (!flg_once) {
        flg_once = 1;
        memset(&ICM20608DeviceInfo, 0, sizeof(AMBA_IMU_INFO_s));

        ICM20608DeviceInfo.GyroID = AMBA_IMU_COMBO;

#ifdef GYRO_PWR_GPIO
        ICM20608DeviceInfo.GyroPwrGpio = GYRO_PWR_GPIO;
#endif

#ifdef GYRO_INT_GPIO
        ICM20608DeviceInfo.GyroIntGpio = GYRO_INT_GPIO;
#endif

#ifndef GYRO_XOUT_POLARITY
        ICM20608DeviceInfo.GyroXPolar = 1;
#else
        ICM20608DeviceInfo.GyroXPolar = GYRO_XOUT_POLARITY;
        if (!ICM20608DeviceInfo.GyroXPolar)
            ICM20608DeviceInfo.GyroXPolar = -1;
#endif

#ifndef GYRO_YOUT_POLARITY
        ICM20608DeviceInfo.GyroYPolar = 1;
#else
        ICM20608DeviceInfo.GyroYPolar = GYRO_YOUT_POLARITY;
        if (!ICM20608DeviceInfo.GyroYPolar)
            ICM20608DeviceInfo.GyroYPolar = -1;
#endif

#ifndef GYRO_ZOUT_POLARITY
        ICM20608DeviceInfo.GyroZPolar = -1;
#else
        ICM20608DeviceInfo.GyroZPolar = GYRO_ZOUT_POLARITY;
        if (!ICM20608DeviceInfo.GyroZPolar)
            ICM20608DeviceInfo.GyroZPolar = -1;
#endif

        if (ICM20608DeviceInfo.GyroXPolar == ICM20608DeviceInfo.GyroYPolar) {
            if (ICM20608DeviceInfo.GyroXPolar > 0) {
                ICM20608DeviceInfo.AccXPolar = AMBA_IMU_NEGATIVE;
                ICM20608DeviceInfo.AccYPolar = AMBA_IMU_POSITIVE;
            } else {
                ICM20608DeviceInfo.AccXPolar = AMBA_IMU_POSITIVE;
                ICM20608DeviceInfo.AccYPolar = AMBA_IMU_NEGATIVE;
            }
        } else {
            if (ICM20608DeviceInfo.GyroXPolar > 0)
                ICM20608DeviceInfo.AccXPolar = ICM20608DeviceInfo.AccYPolar = AMBA_IMU_POSITIVE;
            else
                ICM20608DeviceInfo.AccXPolar = ICM20608DeviceInfo.AccYPolar = AMBA_IMU_NEGATIVE;
        }
        if (!ICM20608DeviceInfo.AccXPolar)
            ICM20608DeviceInfo.AccXPolar = -1;
        if (!ICM20608DeviceInfo.AccYPolar)
            ICM20608DeviceInfo.AccYPolar = -1;

#ifndef GYRO_XOUT_CHANNEL
        ICM20608DeviceInfo.GyroXChan = 0;
#else
        ICM20608DeviceInfo.GyroXChan = GYRO_XOUT_CHANNEL;
#endif

#ifndef GYRO_YOUT_CHANNEL
        ICM20608DeviceInfo.GyroYChan = 1;
#else
        ICM20608DeviceInfo.GyroYChan = GYRO_YOUT_CHANNEL;
#endif

        if ((ICM20608DeviceInfo.GyroXChan == AMBA_IMU_X) && (ICM20608DeviceInfo.GyroYChan == AMBA_IMU_Y)) {
            ICM20608DeviceInfo.AccXChan = AMBA_IMU_Y;
            ICM20608DeviceInfo.AccYChan = AMBA_IMU_X;
        } else {
            ICM20608DeviceInfo.AccXChan = AMBA_IMU_X;
            ICM20608DeviceInfo.AccYChan = AMBA_IMU_Y;
        }

#ifndef GYRO_VOL_DIVIDER_NUM
        ICM20608DeviceInfo.VolDivNum = 1;
#else
        ICM20608DeviceInfo.VolDivNum = GYRO_VOL_DIVIDER_NUM;
#endif

#ifndef GYRO_VOL_DIVIDER_DEN
        ICM20608DeviceInfo.VolDivDen = 1;
#else
        ICM20608DeviceInfo.VolDivDen = GYRO_VOL_DIVIDER_DEN;
#endif

        ICM20608DeviceInfo.SensorInterface   = AMBA_IMU_SPI;
        ICM20608DeviceInfo.SensorAxis        = AMBA_IMU_6AXIS;
        ICM20608DeviceInfo.MaxSense          = 135;                     /* LSB/(deg/sec)  */
        ICM20608DeviceInfo.MinSense          = 127;                     /* LSB/(deg/sec)  */
        ICM20608DeviceInfo.MaxBias           = 35388;//2620;            /* LSB            */
        ICM20608DeviceInfo.MinBias           = 30148;//-2620;           /* LSB            */
        ICM20608DeviceInfo.MaxRmsNoise       = 30;                      /* LSB            */
        ICM20608DeviceInfo.StartUpTime       = 100;                     /* ms             */
        ICM20608DeviceInfo.FullScaleRange    = 250;                     /* deg/sec        */
        ICM20608DeviceInfo.MaxSamplingRate   = 8000;                    /* sample/sec     */
        ICM20608DeviceInfo.AdcResolution     = 16;                      /* bits           */
        ICM20608DeviceInfo.PhsDly            = 3;                       /* ms             */
        ICM20608DeviceInfo.AccMaxSense       = 16876;                   /* LSB/g          */
        ICM20608DeviceInfo.AccMinSense       = 15892;                   /* LSB/g          */
        ICM20608DeviceInfo.AccMaxBias        = 34734;//1966;            /* LSB            */
        ICM20608DeviceInfo.AccMinBias        = 30802;//-1966;           /* LSB            */
        ICM20608DeviceInfo.SamplingRate      = ICM20608DeviceInfo.MaxSamplingRate / (SMPLRT_DIV + 1);
        ICM20608DeviceInfo.LevelShift        = 1 << (ICM20608DeviceInfo.AdcResolution - 1);
    }
    memcpy(pImuInfo, &ICM20608DeviceInfo, sizeof(AMBA_IMU_INFO_s));
    return OK;
}
/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: ICM20608_Open
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
static int ICM20608_Open(void)
{
    static INT16 open_success = 0;
    UINT8 reg[2];
    UINT8 ID[2] = {0};

    if (open_success == -1) {
        ICM20608Print("ERROR: Gyro open fail!!!");
        return NG;
    }
        
    reg[0] = REG_USER_CTRL;
    reg[1] = FIFO_EN | I2C_IF_DIS;
    AmbaSPI_Transfer(AMBA_SPI_MASTER1, &Icm20608_Spi_Ctrl, 2, reg, NULL, 1000);

    reg[0] = REG_ID | OP_READ;
    AmbaSPI_Transfer(AMBA_SPI_MASTER1, &Icm20608_Spi_Ctrl, 2, reg, ID, 1000);
    ICM20608Print("Invensense gyro ID [0x%x], read gyro ID [0x%x]", WHO_AM_I_ID, ID[1]);
    if (ID[1] != WHO_AM_I_ID) {
        ICM20608Print("ERROR: Invensense gyro ID [0x%x], read gyro ID [0x%x]",
                     WHO_AM_I_ID, ID[1]);
        ICM20608Print("ERROR: Cannot open gyro with SPI!!!");
        open_success = -1;
        return NG;
    }

    //---------- power set------------
    reg[0] = REG_POW_MGM1;
    reg[1] = TEMP_DIS | CLK_SEL;
    AmbaSPI_Transfer(AMBA_SPI_MASTER1, &Icm20608_Spi_Ctrl, 2, reg, NULL, 1000);
    //--------------------------------

    reg[0] = REG_DIV;
    reg[1] = SMPLRT_DIV;
    AmbaSPI_Transfer(AMBA_SPI_MASTER1, &Icm20608_Spi_Ctrl, 2, reg, NULL, 1000);

    reg[0] = REG_CFG;
    reg[1] = EXT_SYNC_SET | DLPF_CFG;
    AmbaSPI_Transfer(AMBA_SPI_MASTER1, &Icm20608_Spi_Ctrl, 2, reg, NULL, 1000);
    
    //-------- read test
    //reg[0] = REG_CFG | OP_READ;
    //AmbaSPI_Transfer(AMBA_SPI_MASTER1, &Icm20608_Spi_Ctrl, 2, reg, ID, 1000);
    //ICM20608Print("REG_CFG = 0x%x", ID[1]);
    //------------------

    reg[0] = REG_GYRO_CFG;
    reg[1] = FS_SEL;
    AmbaSPI_Transfer(AMBA_SPI_MASTER1, &Icm20608_Spi_Ctrl, 2, reg, NULL, 1000);
    
    //-------- read test
    //reg[0] = REG_GYRO_CFG | OP_READ;
    //AmbaSPI_Transfer(AMBA_SPI_MASTER1, &Icm20608_Spi_Ctrl, 2, reg, ID, 1000);
    //ICM20608Print("REG_GYRO_CFG = 0x%x", ID[1]);  
    //reg[0] = REG_POW_MGM1 | OP_READ;
    //AmbaSPI_Transfer(AMBA_SPI_MASTER1, &Icm20608_Spi_Ctrl, 2, reg, ID, 1000);
    //ICM20608Print("REG_POW_MGM1 = 0x%x", ID[1]);    
    //------------------    
    
    reg[0] = REG_ACC_CFG;
    reg[1] = AFS_SEL;
    AmbaSPI_Transfer(AMBA_SPI_MASTER1, &Icm20608_Spi_Ctrl, 2, reg, NULL, 1000);    

    reg[0] = REG_FIFO_EN;
    reg[1] = TEMP_FIFO_EN | XG_FIFO_EN | YG_FIFO_EN | ZG_FIFO_EN | ACCEL_FIFO_EN;
    AmbaSPI_Transfer(AMBA_SPI_MASTER1, &Icm20608_Spi_Ctrl, 2, reg, NULL, 1000);
    /*
        reg[0] = REG_POW_MGM1;
        reg[1] = TEMP_DIS | CLK_SEL;
        AmbaSPI_Transfer(AMBA_SPI_MASTER1, &Icm20608_Spi_Ctrl, 2, reg, NULL, 1000);
    */
    reg[0] = REG_INT_CFG;
    if (ICM20608DeviceInfo.GyroIntGpio)
        reg[1] = INT_LVL | INT_OPEN | INT_ANYRD_2CLEAR | LATCH_INT_EN | FSYNC_INT_LEVEL | FSYNC_INT_MODE_EN;
    else
        reg[1] = LATCH_INT_EN;
    AmbaSPI_Transfer(AMBA_SPI_MASTER1, &Icm20608_Spi_Ctrl, 2, reg, NULL, 1000);

    reg[0] = REG_INT_EN;
    if (ICM20608DeviceInfo.GyroIntGpio) {
        reg[1] = DATA_RDY_EN;
        AmbaSPI_Transfer(AMBA_SPI_MASTER1, &Icm20608_Spi_Ctrl, 2, reg, NULL, 1000);
    }
    
    //--------------------- read offset
    /*
    reg[0] = 0x00 | OP_READ;
    AmbaSPI_Transfer(AMBA_SPI_MASTER1, &Icm20608_Spi_Ctrl, 2, reg, ID, 1000);
    ICM20608Print("0x00 = 0x%x", ID[1]);
    reg[0] = 0x01 | OP_READ;
    AmbaSPI_Transfer(AMBA_SPI_MASTER1, &Icm20608_Spi_Ctrl, 2, reg, ID, 1000);
    ICM20608Print("0x01 = 0x%x", ID[1]);
    reg[0] = 0x02 | OP_READ;
    AmbaSPI_Transfer(AMBA_SPI_MASTER1, &Icm20608_Spi_Ctrl, 2, reg, ID, 1000);
    ICM20608Print("0x02 = 0x%x", ID[1]);
    reg[0] = 0x0D | OP_READ;
    AmbaSPI_Transfer(AMBA_SPI_MASTER1, &Icm20608_Spi_Ctrl, 2, reg, ID, 1000);
    ICM20608Print("0x0D = 0x%x", ID[1]);
    reg[0] = 0x0E | OP_READ;
    AmbaSPI_Transfer(AMBA_SPI_MASTER1, &Icm20608_Spi_Ctrl, 2, reg, ID, 1000);
    ICM20608Print("0x0E = 0x%x", ID[1]);
    reg[0] = 0x0F | OP_READ;
    AmbaSPI_Transfer(AMBA_SPI_MASTER1, &Icm20608_Spi_Ctrl, 2, reg, ID, 1000);
    ICM20608Print("0x0F = 0x%x", ID[1]);
    reg[0] = 0x13 | OP_READ;
    AmbaSPI_Transfer(AMBA_SPI_MASTER1, &Icm20608_Spi_Ctrl, 2, reg, ID, 1000);
    ICM20608Print("0x13 = 0x%x", ID[1]);
    reg[0] = 0x14 | OP_READ;
    AmbaSPI_Transfer(AMBA_SPI_MASTER1, &Icm20608_Spi_Ctrl, 2, reg, ID, 1000);
    ICM20608Print("0x14 = 0x%x", ID[1]);
    reg[0] = 0x15 | OP_READ;
    AmbaSPI_Transfer(AMBA_SPI_MASTER1, &Icm20608_Spi_Ctrl, 2, reg, ID, 1000);
    ICM20608Print("0x15 = 0x%x", ID[1]);
    reg[0] = 0x16 | OP_READ;
    AmbaSPI_Transfer(AMBA_SPI_MASTER1, &Icm20608_Spi_Ctrl, 2, reg, ID, 1000);
    ICM20608Print("0x16 = 0x%x", ID[1]);
    reg[0] = 0x17 | OP_READ;
    AmbaSPI_Transfer(AMBA_SPI_MASTER1, &Icm20608_Spi_Ctrl, 2, reg, ID, 1000);
    ICM20608Print("0x17 = 0x%x", ID[1]);
    reg[0] = 0x18 | OP_READ;
    AmbaSPI_Transfer(AMBA_SPI_MASTER1, &Icm20608_Spi_Ctrl, 2, reg, ID, 1000);
    ICM20608Print("0x18 = 0x%x", ID[1]);                                                
    */
    //--------------------------------------------------------------------------

    return OK;
}
/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: ICM20608_Read
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
static int ICM20608_Read(UINT8 Type, AMBA_IMU_DATA_s* pData)
{
    UINT8 reg;
    UINT8 data[15];

    if (Type == AMBA_IMU_GYRO) {
        /*User get Gyro data*/
        reg = REG_GYRO_XOUT_H | OP_READ;
        AmbaSPI_Transfer(AMBA_SPI_MASTER1, &Icm20608_Spi_Ctrl_Read, 7, &reg, data, 1000);

        if ((ICM20608DeviceInfo.GyroXChan == AMBA_IMU_X) && (ICM20608DeviceInfo.GyroYChan == AMBA_IMU_Y)) {
            pData->GyroXData = TransletData(&data[1]);
            pData->GyroYData = TransletData(&data[3]);
        } else {
            pData->GyroYData = TransletData(&data[1]);
            pData->GyroXData = TransletData(&data[3]);
        }
        pData->GyroZData = TransletData(&data[5]);

        pData->GyroXData += ICM20608DeviceInfo.LevelShift;
        pData->GyroYData += ICM20608DeviceInfo.LevelShift;
        pData->GyroZData += ICM20608DeviceInfo.LevelShift;
    } else if (Type == AMBA_IMU_ACCMETER) {
        /*User get G-Sensor data*/
        reg = REG_ACCEL_XOUT_H | OP_READ;
        AmbaSPI_Transfer(AMBA_SPI_MASTER1, &Icm20608_Spi_Ctrl_Read, 7, &reg, data, 1000);

        if ((ICM20608DeviceInfo.AccXChan == AMBA_IMU_Y) && (ICM20608DeviceInfo.AccYChan == AMBA_IMU_X)) {
            pData->AccXData = TransletData(&data[3]);
            pData->AccYData = TransletData(&data[1]);
        } else {
            pData->AccXData = TransletData(&data[1]);
            pData->AccYData = TransletData(&data[3]);
        }
        pData->AccZData = TransletData(&data[5]);

        pData->AccXData += ICM20608DeviceInfo.LevelShift;
        pData->AccYData += ICM20608DeviceInfo.LevelShift;
        pData->AccZData += ICM20608DeviceInfo.LevelShift;
    } else if (Type == AMBA_IMU_COMBO) {
        /*User get ALL data*/
        reg = REG_ACCEL_XOUT_H | OP_READ;
        AmbaSPI_Transfer(AMBA_SPI_MASTER1, &Icm20608_Spi_Ctrl_Read, 15, &reg, data, 1000);

        if ((ICM20608DeviceInfo.AccXChan == AMBA_IMU_Y) && (ICM20608DeviceInfo.AccYChan == AMBA_IMU_X)) {
            pData->AccXData = TransletData(&data[3]);
            pData->AccYData = TransletData(&data[1]);
        } else {
            pData->AccXData = TransletData(&data[1]);
            pData->AccYData = TransletData(&data[3]);
        }
        pData->AccZData = TransletData(&data[5]);

        pData->AccXData += ICM20608DeviceInfo.LevelShift;
        pData->AccYData += ICM20608DeviceInfo.LevelShift;
        pData->AccZData += ICM20608DeviceInfo.LevelShift;

        /* Output Temperature data */
        pData->TemperatureData = TransletData(&data[7]);
        pData->TemperatureData += ICM20608DeviceInfo.LevelShift;

        if ((ICM20608DeviceInfo.GyroXChan == AMBA_IMU_X) && (ICM20608DeviceInfo.GyroYChan == AMBA_IMU_Y)) {
            pData->GyroXData = TransletData(&data[9]);
            pData->GyroYData = TransletData(&data[11]);
        } else {
            pData->GyroYData = TransletData(&data[9]);
            pData->GyroXData = TransletData(&data[11]);
        }
        pData->GyroZData = TransletData(&data[13]);

        pData->GyroXData += ICM20608DeviceInfo.LevelShift;
        pData->GyroYData += ICM20608DeviceInfo.LevelShift;
        pData->GyroZData += ICM20608DeviceInfo.LevelShift;
    }
    
    //AmbaPrint("X=%d  Y=%d  Z=%d",pData->GyroXData,pData->GyroYData,pData->GyroZData);
    //AmbaPrint("AX=%d  AY=%d  AZ=%d",pData->AccXData,pData->AccYData,pData->AccZData);
    
    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: ICM20608_RegRead
 *
 *  @Description:: ICM20608 Register Read Function
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
static int ICM20608_RegRead(UINT16 Addr, UINT16 SubAddr, UINT16 *pInbuf, UINT16 NumBytes)
{
    UINT8 reg;
    UINT8 data_temp[3];

    if (NumBytes > 2)
        return NG;

    reg = Addr | OP_READ;
    AmbaSPI_Transfer(AMBA_SPI_MASTER1, &Icm20608_Spi_Ctrl, NumBytes + 1, &reg, data_temp, 1000);

    pInbuf[0] = data_temp[1];
    if (NumBytes == 2)
        pInbuf[1] = data_temp[2];

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: ICM20608_RegWrite
 *
 *  @Description:: ICM20608 Register Write Function
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
static int ICM20608_RegWrite(UINT16 Addr, UINT16 SubAddr, UINT16 InData)
{
    UINT8 reg[2];

    reg[0] = Addr;
    reg[1] = InData;
    AmbaSPI_Transfer(AMBA_SPI_MASTER1, &Icm20608_Spi_Ctrl, 2, reg, NULL, 1000);

    return OK;
}
/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: ICM20608_SetSampleRate
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
static int ICM20608_SetSampleRate(UINT16 TimeInUs)
{
    UINT8 reg[2], rate;

    if (TimeInUs == 0) {
        /*use default value*/
        reg[1] = SMPLRT_DIV;
        rate = ICM20608DeviceInfo.SamplingRate / (SMPLRT_DIV + 1);
    } else {
        if ((1000000 / TimeInUs) > ICM20608DeviceInfo.MaxSamplingRate)
            return (-1);
        else {
            reg[1] = ICM20608DeviceInfo.MaxSamplingRate * TimeInUs / 1000000;
            reg[1] -= 1;
            rate = 1000000 / TimeInUs;
        }
    }

    reg[0] = REG_DIV;

    AmbaSPI_Transfer(AMBA_SPI_MASTER1, &Icm20608_Spi_Ctrl, 2, &reg, NULL, 1000);
    ICM20608DeviceInfo.SamplingRate = rate;

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: ICM20608_Close
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
static int ICM20608_Close(void)
{
    return OK;
}

/*-----------------------------------------------------------------------------------------------*/
AMBA_IMU_OBJ_s AmbaIMU_ICM20608Obj = {
    .GetDeviceInfo            = ICM20608_GetInfo,
    .Open                     = ICM20608_Open,
    .Read                     = ICM20608_Read,
    .RegRead                  = ICM20608_RegRead,
    .RegWrite                 = ICM20608_RegWrite,
    .SetSampleRate            = ICM20608_SetSampleRate,
    .Close                    = ICM20608_Close,
};
