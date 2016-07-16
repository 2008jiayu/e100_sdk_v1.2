/**
 * @file rtos/peripheral/g_sensor/inc/g_sensor_drv_st_lis3de.h
 *
 * Control Registers of g-sensor driver.
 *
 * History:
 *  2015/05/20 - [Yuchi Wei] created.
 *
 * Copyright (C) 2006-20015, Ambarella, Inc.
 *
 * All rights reserved. No Part of this file may be reproduced, stored
 * in a retrieval system, or transmitted, in any form, or by any means,
 * electronic, mechanical, photocopying, recording, or otherwise,
 * without the prior consent of Ambarella, Inc.
 */

#ifndef __G_SENSOR_ST_LIS3DE_H__
#define __G_SENSOR_ST_LIS3DE_H__

#define GSENSOR_ST_LIS3DE_I2C_ADDR   0x32

//Digital Gyro Register Define
#define REG_STATUS_AUX      0x07
#define REG_INT_COUNT       0x0E
#define REG_WHO_AM_I        0x0F
#define REG_TEMP_CFG        0x1F
#define REG_CTRL1           0x20
#define REG_CTRL2           0x21
#define REG_CTRL3           0x22
#define REG_CTRL4           0x23
#define REG_CTRL5           0x24
#define REG_CTRL6           0x25
#define REG_REFERENCE       0x26
#define REG_STATUS_2        0x27

#define REG_X_L             0x28
#define REG_X_H             0x29
#define REG_Y_L             0x2A
#define REG_Y_H             0x2B
#define REG_Z_L             0x2C
#define REG_Z_H             0x2D

#define REG_FIFO_CTRL       0x2E
#define REG_FIFO_SRC        0x2F

#define REG_INT1_CFG        0x30
#define REG_INT1_SRC        0x31
#define REG_INT1_THS        0x32
#define REG_INT1_DURA       0x33

#define REG_INT2_CFG        0x34
#define REG_INT2_SRC        0x35
#define REG_INT2_THS        0x36
#define REG_INT2_DURA       0x37

#define MAX_REG                      (REG_INT2_DURA)
//Digital Gyro Register Define End

//Data Define
#define WHO_AM_I_ID     0x33

#define LPEN                (0x01 << 3)
#define ODR_1HZ         (0x01 << 4)
#define ODR_100HZ       (0x05 << 4)
#define ODR_200HZ       (0x06 << 4)
#define ODR_1250HZ      (0x09 << 4) //normal mode:1.25khz, low power mode 5khz
#define Z_EN                (0x01 << 2)
#define Y_EN                (0x01 << 1)
#define X_EN                (0x01 << 0)

#define HPF_MODE1       (0x0 << 6) //normal mode reset after reading HP_RESET_FILTER
#define HPF_MODE2       (0x1 << 6) //reference signal
#define HPF_MODE3       (0x2 << 6) //normal mode
#define HPF_MODE4       (0x3 << 6) //Autoreset on interrupt event
#define HPF_CUTOFF1     (0x0 << 4)
#define HPF_CUTOFF2     (0x1 << 4)
#define HPF_CUTOFF3     (0x2 << 4)
#define HPF_CUTOFF4     (0x3 << 4)
#define HPF_EN          (0x1 << 3)

#define BDU                (0x1 << 7)
#define FS_2G           (0x0 << 4)
#define FS_4G           (0x1 << 4)
#define FS_8G           (0x2 << 4)
#define FS_16G          (0x3 << 4)
#define HR_MODE         (0x1 << 3)

//CTRL_REG3 register
#define INT1_CLICK      (0x1 << 7)
#define INT1_IG1      (0x1 << 6)    //interrupt generator 1 on INT1
#define INT1_IG2      (0x1 << 5)    //interrupt generator 2 on INT1
#define INT1_DRDY1      (0x1 << 4)
#define INT1_DRDY2      (0x1 << 3)
#define INT1_WTM      (0x1 << 2)
#define INT1_OVERRUN      (0x1 << 1)

//CTRL_REG6 register
#define INT2_CLICK      (0x1 << 7)
#define INT2_IG1         (0x1 << 6) //Interrupt generator 1 enabled on INT2 pin
#define INT2_IG2         (0x1 << 5) //Interrupt generator 2 enabled on INT2 pin
#define INT2_BOOT         (0x1 << 4)
#define INT2_ACT         (0x1 << 3)
#define HL_ACTIVE       (0x1 << 1)

#define YHIE_YUPE       (0x1 << 3)
#define YLIE_YDOWNE     (0x1 << 2)
#define XHIE_XUPE       (0x1 << 1)
#define XLIE_XDOWNE     (0x1 << 0)

#define IA              (0x1 << 6)
#define ZH              (0x1 << 5)
#define ZL              (0x1 << 4)
#define YH              (0x1 << 3)
#define YL              (0x1 << 2)
#define XH              (0x1 << 1)
#define XL              (0x1 << 0)

#define LIR_IG1     (0x1<<3)
#define LIR_IG2     (0x1<<1)

//Data Define End


//I2C Operation definition
#define OP_BURST        (0x1 << 7)
//I2C Operation definition End


#endif // END OF __G_SENSOR_ST_LIS3DE_H__
