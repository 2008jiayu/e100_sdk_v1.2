/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaSensor_IMX117.h
 *
 *  @Copyright      :: Copyright (C) 2013 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Control APIs of SONY IMX117 CMOS sensor with LVDS interface
\*-------------------------------------------------------------------------------------------------------------------*/

#ifndef _AMBA_SENSOR_IMX117_H_
#define _AMBA_SENSOR_IMX117_H_

#define NA (0xff)

/*-----------------------------------------------------------------------------------------------*\
 * Control registers
\*-----------------------------------------------------------------------------------------------*/
#define IMX117_NUM_READOUT_MODE_REG         105

typedef enum _IMX117_CTRL_REG_e_ {
    IMX117_OP_REG              = 0x0000,
    IMX117_DCKRST_CLPSQRST_REG = 0x0001,
    IMX117_SHR_LSB_REG         = 0x000b,
    IMX117_SHR_MSB_REG         = 0x000c,
    IMX117_SVR_LSB_REG         = 0x000d,
    IMX117_SVR_MSB_REG         = 0x000e,
    IMX117_SPL_LSB_REG         = 0x000f,
    IMX117_SPL_MSB_REG         = 0x0010,
    IMX117_PGC_LSB_REG         = 0x0009,
    IMX117_PGC_MSB_REG         = 0x000a,
    IMX117_DGAIN_REG           = 0x0011,
    IMX117_MDSEL1_REG          = 0x0004,
    IMX117_MDSEL2_REG          = 0x0005,
    IMX117_MDSEL3_REG          = 0x0006,
    IMX117_MDSEL4_REG          = 0x0007,
    IMX117_SMD_REG             = 0x0008,
    IMX117_APGC01_LSB_REG      = 0x0352,
    IMX117_APGC01_MSB_REG      = 0x0353,
    IMX117_APGC02_LSB_REG      = 0x0356,
    IMX117_APGC02_MSB_REG      = 0x0357,
    IMX117_MDVREV_REG          = 0x001a,
} IMX117_CTRL_REG_e;

typedef enum _IMX117_READOUT_MODE_e_ {
    IMX117_TYPE_2_3_MODE_0 = 0, /* 12bit A/D, 12bit OUT, non-subsampling */
    IMX117_TYPE_2_3_MODE_0A,    /* 12bit A/D, 12bit OUT, non-subsampling */
    IMX117_TYPE_2_3_MODE_2A,    /* 10bit A/D, 12bit OUT,  2 x 2 binning  */
    IMX117_TYPE_2_3_MODE_4A,    /* 10bit A/D, 10bit OUT,  3 x 3 binning  */
    IMX117_TYPE_2_3_MODE_4,     /* 10bit A/D, 12bit OUT,  3 x 3 binning  */
    IMX117_TYPE_2_3_MODE_3,     /* 10bit A/D, 12bit OUT,  3 x 3 binning  */
    IMX117_TYPE_2_3_MODE_5,     /* 10bit A/D, 12bit OUT,  3 x 9 binning  */
    IMX117_TYPE_2_3_MODE_6,     /* 10bit A/D, 12bit OUT, 3 x 17 binning  */
    IMX117_TYPE_2_3_MODE_7,     /* 10bit A/D, 12bit OUT, 3 x 17 binning  */
    IMX117_TYPE_2_3_MODE_10,    /* 10bit A/D, 12bit OUT,  2 x 2 binning  */

    IMX117_TYPE_2_5_MODE_0,     /* Type 1/2.5 mode 0,  12bit A/D, 12bit OUT */
    IMX117_TYPE_2_5_MODE_1,     /* Type 1/2.5 mode 1,  10bit A/D, 10bit OUT */
    IMX117_TYPE_2_5_MODE_4,     /* Type 1/2.5 mode 4,  10bit A/D, 12bit OUT, 3 x 3 binning */

    IMX117_TYPE_4_4_MODE_1,     /* Type 1/4.4 mode 1,  10bit A/D, 10bit OUT */

    IMX117_CUSTOM_MODE,         /* 3008x1692 */

    IMX117_NUM_READOUT_MODE,
} IMX117_READOUT_MODE_e;

typedef enum _IMX117_LVDS_CHANNEL_SELECT_e_ {
    IMX117_LVDS_10CH = 0,
    IMX117_LVDS_8CH,
    IMX117_LVDS_6CH,
    IMX117_LVDS_4CH,
    IMX117_LVDS_2CH,
    IMX117_LVDS_1CH
} IMX117_LVDS_CHANNEL_SELECT_e;

typedef struct _IMX117_REG_s_ {
    UINT16  Addr;
    UINT8   Data[IMX117_NUM_READOUT_MODE];
} IMX117_REG_s;

typedef struct _IMX117_FRAME_TIMING_s_ {
    UINT32  InputClk;                       /* Sensor side input clock frequency */
    UINT32  NumTickPerXhs;                  /* XHS period (in input clock cycles) */
    UINT32  NumXhsPerH;                     /* horizontal operating period (in number of XHS pulses) */
    UINT32  NumXhsPerV;                     /* vertical operating period (in number of XHS pulses) */
    UINT32  NumXvsPerV;                     /* vertical operating period (in number of XVS pulses) */
    AMBA_DSP_FRAME_RATE_s   FrameRate;      /* framerate value of this sensor mode */
} IMX117_FRAME_TIMING_s;

typedef struct _IMX117_MODE_INFO_s_ {
    IMX117_READOUT_MODE_e   ReadoutMode;
    IMX117_FRAME_TIMING_s   FrameTiming;
} IMX117_MODE_INFO_s;

typedef struct _IMX117_CTRL_s_ {
    AMBA_SENSOR_STATUS_INFO_s   Status;
    IMX117_FRAME_TIMING_s       FrameTime;
    UINT32                      CurrentAgc;
    UINT32                      CurrentDgc;
    UINT32                      CurrentShrCtrlSVR;
    UINT32                      CurrentShrCtrlSHR;
} IMX117_CTRL_s;

/*-----------------------------------------------------------------------------------------------*\
 *  Application Mode Definition
\*-----------------------------------------------------------------------------------------------*/
typedef enum _AMBA_SENSOR_IMX117_MODE_ID_e_ {
    AMBA_SENSOR_IMX117_TYPE_2_3_MODE_0_30P = 0,
    AMBA_SENSOR_IMX117_TYPE_2_3_MODE_0A_35P,
    AMBA_SENSOR_IMX117_TYPE_2_3_MODE_2A_60P,
    AMBA_SENSOR_IMX117_TYPE_2_3_MODE_4A_240P,
    AMBA_SENSOR_IMX117_TYPE_2_3_MODE_4_120P,
    AMBA_SENSOR_IMX117_TYPE_2_3_MODE_3_60P,
    AMBA_SENSOR_IMX117_TYPE_2_3_MODE_5_240P,
    AMBA_SENSOR_IMX117_TYPE_2_3_MODE_6_30P,
    AMBA_SENSOR_IMX117_TYPE_2_3_MODE_7_480P,
    AMBA_SENSOR_IMX117_TYPE_2_3_MODE_3_30P,
    AMBA_SENSOR_IMX117_TYPE_2_3_MODE_0_2P,

    AMBA_SENSOR_IMX117_TYPE_2_5_MODE_0_30P, //11
    AMBA_SENSOR_IMX117_TYPE_2_5_MODE_1_60P,
    AMBA_SENSOR_IMX117_TYPE_2_5_MODE_4_120P,

    AMBA_SENSOR_IMX117_TYPE_4_4_MODE_1_120P,
    AMBA_SENSOR_IMX117_TYPE_4_4_MODE_1_60P,

    AMBA_SENSOR_IMX117_TYPE_2_3_MODE_0_25P,
    AMBA_SENSOR_IMX117_TYPE_2_3_MODE_2A_50P,
    AMBA_SENSOR_IMX117_TYPE_2_3_MODE_4A_200P,
    AMBA_SENSOR_IMX117_TYPE_2_3_MODE_4_100P,
    AMBA_SENSOR_IMX117_TYPE_2_3_MODE_3_50P, //20
    AMBA_SENSOR_IMX117_TYPE_2_3_MODE_5_200P,
    AMBA_SENSOR_IMX117_TYPE_2_3_MODE_7_400P,

    AMBA_SENSOR_IMX117_TYPE_2_5_MODE_0_25P,
    AMBA_SENSOR_IMX117_TYPE_2_5_MODE_1_50P,
    AMBA_SENSOR_IMX117_TYPE_2_5_MODE_4_100P,

    AMBA_SENSOR_IMX117_TYPE_4_4_MODE_1_100P,

    AMBA_SENSOR_IMX117_TYPE_2_5_MODE_1_24P,

    AMBA_SENSOR_IMX117_CUSTOM_MODE_60P,

    AMBA_SENSOR_IMX117_TYPE_2_5_MODE_1_20P, //29

    AMBA_SENSOR_IMX117_TYPE_2_3_MODE_0_24P, //30

    AMBA_SENSOR_IMX117_TYPE_2_3_MODE_10_120P,
    AMBA_SENSOR_IMX117_TYPE_2_3_MODE_10_100P,

    AMBA_SENSOR_IMX117_TYPE_2_3_MODE_2A_30P,
    AMBA_SENSOR_IMX117_TYPE_2_3_MODE_2A_25P,
    
    AMBA_SENSOR_IMX117_NUM_MODE,
} AMBA_SENSOR_IMX117_MODE_ID_e;

/*-----------------------------------------------------------------------------------------------*\
 *  Defined in AmbaSensor_IMX117.c
\*-----------------------------------------------------------------------------------------------*/
extern const AMBA_SENSOR_DEVICE_INFO_s IMX117DeviceInfo;
extern const AMBA_SENSOR_INPUT_INFO_s IMX117InputInfoNormalReadout[IMX117_NUM_READOUT_MODE];
extern const AMBA_SENSOR_INPUT_INFO_s IMX117InputInfoInversionReadout[IMX117_NUM_READOUT_MODE];
extern const AMBA_SENSOR_OUTPUT_INFO_s IMX117OutputInfo[IMX117_NUM_READOUT_MODE];
extern const IMX117_MODE_INFO_s IMX117ModeInfoList[AMBA_SENSOR_IMX117_NUM_MODE];

extern IMX117_REG_s IMX117RegTable[IMX117_NUM_READOUT_MODE_REG];

#endif /* _AMBA_SENSOR_IMX117_H_ */
