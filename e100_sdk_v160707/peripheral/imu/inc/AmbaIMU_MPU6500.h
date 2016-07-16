/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaIMU_MPU6500.h
 *
 *  @Copyright      :: Copyright (C) 2013 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Control APIs of INVENSENSE MPU6500 6-axis Motion sensor with SPI interface
\*-------------------------------------------------------------------------------------------------------------------*/

#ifndef _AMBA_IMU_MPU6500_H_
#define _AMBA_IMU_MPU6500_H_

AMBA_IMU_INFO_s MPU6500DeviceInfo = {0};

/*-----------------------------------------------------------------------------------------------*\
 * Control registers
\*-----------------------------------------------------------------------------------------------*/


//Digital Gyro Register Define
#define REG_DIV             0x19
#define REG_CFG             0x1A
#define REG_GYRO_CFG        0x1B
#define REG_ACC_CFG         0x1C
#define REG_ACC_CFG_2       0x1D
#define REG_FIFO_EN         0x23
#define REG_INT_CFG         0x37
#define REG_INT_EN          0x38
#define REG_INT_STATUS      0x3A
#define REG_ACCEL_XOUT_H    0x3B
#define REG_ACCEL_XOUT_L    0x3C
#define REG_ACCEL_YOUT_H    0x3D
#define REG_ACCEL_YOUT_L    0x3E
#define REG_ACCEL_ZOUT_H    0x3F
#define REG_ACCEL_ZOUT_L    0x40
#define REG_TEMP_OUT_H      0x41
#define REG_TEMP_OUT_L      0x42
#define REG_GYRO_XOUT_H     0x43
#define REG_GYRO_XOUT_L     0x44
#define REG_GYRO_YOUT_H     0x45
#define REG_GYRO_YOUT_L     0x46
#define REG_GYRO_ZOUT_H     0x47
#define REG_GYRO_ZOUT_L     0x48
#define REG_USER_CTRL       0x6A
#define REG_POW_MGM1        0x6B
#define REG_POW_MGM2        0x6C
#define REG_FIFO_CNT_H      0x72
#define REG_FIFO_RW         0x74
#define REG_ID              0x75
//Digital Gyro Register Define End

//Data Define
#define OP_READ             (1 << 7)

/*------------------ Register 0x19 ------------------*/
#ifdef GYRO_INT_GPIO
#define SMPLRT_DIV          0//3    //This value can be modfied if needed
#else
#define SMPLRT_DIV          0
#endif
/*------------------ Register 0x1A ------------------*/
#define FIFO_MODE           (0 << 6)
#define EXT_SYNC_SET        (0 << 3)

/*
    DLPF_CFG         Gyro Scope
                    (BW,   Delay, Fs)
    0 :         ( 250hz,  0.97ms, 8khz)
    1 :         ( 184hz,  2.9 ms, 1khz)
    2 :         (  92hz,  3.9 ms, 1khz)
    3 :         (  41hz,  5.9 ms, 1khz)
    4 :         (  20hz,  9.9 ms, 1khz)
    5 :         (  10hz, 17.85ms, 1khz)
    6 :         (   5hz, 33.48ms, 1khz)
    7 :         (3600hz,  0.17ms  8khz)
*/
#define DLPF_CFG            0
/*------------------ Register 0x1B ------------------*/
/*
    FS_SEL      Full Scale Range    LSB Sensitivity(LSB/dps)
    0 :         +/-  250            131.0
    1 :         +/-  500            65.5
    2 :         +/- 1000            32.8
    3 :         +/- 2000            16.4
*/
#define FS_SEL              (0 << 3)
/*------------------ Register 0x1C ------------------*/
/*
    AFS_SEL     Full Scale Range    LSB Sensitivity(LSB/g)
    0 :         +/-  2g             16384
    1 :         +/-  4g             8192
    2 :         +/-  8g             4096
    3 :         +/- 16g             2048
*/
#define AFS_SEL             (0 << 3)
/*------------------ Register 0x1D ------------------*/
/*
A_DLPF_CFG      Accelerometer
                (BW, Delay, Fs)
    0 :         ( 460hz,  1.94ms, 1khz)
    1 :         ( 184hz,  5.8 ms, 1khz)
    2 :         (  92hz,  7.8 ms, 1khz)
    3 :         (  41hz, 11.8 ms, 1khz)
    4 :         (  20hz, 19.8 ms, 1khz)
    5 :         (  10hz, 35.7 ms, 1khz)
    6 :         (   5hz, 66.96ms, 1khz)
    7 :         ( 460hz,  1.94ms, 1khz)
*/
#define A_DLPF_CFG          2
/*------------------ Register 0x23 ------------------*/
#define TEMP_FIFO_EN        (0 << 7)
#define XG_FIFO_EN          (0 << 6)
#define YG_FIFO_EN          (0 << 5)
#define ZG_FIFO_EN          (0 << 4)
#define ACCEL_FIFO_EN       (0 << 3)
/*------------------ Register 0x37 ------------------*/
#define INT_LVL             (1 << 7)  // 0: active high, 1: active low
#define INT_OPEN            (0 << 6)  // 0: push-pull, 1: open drain
#define LATCH_INT_EN        (1 << 5)
#define INT_ANYRD_2CLEAR    (1 << 4)
#define FSYNC_INT_LEVEL     (0 << 3)
#define FSYNC_INT_MODE_EN   (1 << 2)
/*------------------ Register 0x38 ------------------*/
#define FIFO_OVERFLOW_EN    (0 << 4)
#define FSYNC_INT_EN        (0 << 3)
#define DATA_RDY_EN         (0 << 0)
/*------------------ Register 0x3A ------------------*/
#define DATA_RDY_INT_MASK   0x1
/*------------------ Register 0x6A ------------------*/
#define FIFO_EN             (0 << 6)
#define I2C_IF_DIS          (1 << 4)
/*------------------ Register 0x6B ------------------*/
#define TEMP_DIS            (1 << 3)
#define CLK_SEL             (3 << 0)
/*------------------ Register 0x75 ------------------*/
#define WHO_AM_I_ID         (0x70)

//Data Define End

#endif /* _AMBA_IMU_MPU6500_H_ */
