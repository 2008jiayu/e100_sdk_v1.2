/*-------------------------------------------------------------------------------------------------------------------
 *  @FileName       :: AmbaSensor_IMX206.h
 *
 *  @Copyright      :: Copyright (C) 2013 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Control APIs of SONY IMX206 CMOS sensor with LVDS interface
\*-------------------------------------------------------------------------------------------------------------------*/

#ifndef _AMBA_SENSOR_IMX206_H_
#define _AMBA_SENSOR_IMX206_H_

#define NA (0xff)

/*-----------------------------------------------------------------------------------------------*\
 * Control registers
\*-----------------------------------------------------------------------------------------------*/
#define IMX206_NUM_READOUT_MODE_REG         53

typedef enum _IMX206_CTRL_REG_e_ {
    IMX206_OP_REG              = 0x0000,
    IMX206_DCKRST_CLPSQRST_REG = 0x0001,
    IMX206_SSBRK_REG           = 0x0002,
    IMX206_SHR_LSB_REG         = 0x000b,
    IMX206_SHR_MSB_REG         = 0x000c,
    IMX206_SVR_LSB_REG         = 0x000d,
    IMX206_SVR_MSB_REG         = 0x000e,
    IMX206_SPL_LSB_REG         = 0x000f,
    IMX206_SPL_MSB_REG         = 0x0010,
    IMX206_PGC_LSB_REG         = 0x0009,
    IMX206_PGC_MSB_REG         = 0x000a,
    IMX206_DGAIN_REG           = 0x0011,
    IMX206_MDSEL1_REG          = 0x0004,
    IMX206_MDSEL2_REG          = 0x0005,
    IMX206_MDSEL3_REG          = 0x0006,
    IMX206_MDSEL4_REG          = 0x0007,
    IMX206_SMD_REG             = 0x0008,
    IMX206_MDVREV_REG          = 0x001a,
    IMX206_HCOUNT_HALF_LSB_REG = 0x007c,
    IMX206_HCOUNT_HALF_MSB_REG = 0x007d,

} IMX206_CTRL_REG_e;

typedef enum _IMX206_READOUT_MODE_e_ {
    IMX206_TYPE_2_3_MODE_2 = 0, /* 10bit A/D, 10bit OUT,  2 x 2 binning  */
    IMX206_TYPE_2_3_MODE_2A,    /* 10bit A/D, 10bit OUT,  2 x 2 binning  */
    IMX206_TYPE_2_3_MODE_0,     /* 10bit A/D, 10bit OUT,  non binning  */
    IMX206_TYPE_2_3_MODE_4,     /* 10bit A/D, 10bit OUT,  3 x 9 binning  */
    IMX206_TYPE_2_3_MODE_3B,    /* 10bit A/D, 10bit OUT,  3 x 3 binning  */
    IMX206_TYPE_2_3_MODE_3,     /* 10bit A/D, 10bit OUT,  3 x 3 binning  */
    IMX206_TYPE_2_3_MODE_5,     /* 10bit A/D, 10bit OUT,  3 x 9 binning  */

    IMX206_NUM_READOUT_MODE,
} IMX206_READOUT_MODE_e;

typedef enum _IMX206_LVDS_CHANNEL_SELECT_e_ {
    IMX206_LVDS_4CH = 0,
    IMX206_LVDS_2CH,
    IMX206_LVDS_1CH
} IMX206_LVDS_CHANNEL_SELECT_e;

typedef struct _IMX206_REG_s_ {
    UINT16  Addr;
    UINT8   Data[IMX206_NUM_READOUT_MODE];
} IMX206_REG_s;

typedef struct _IMX206_FRAME_TIMING_s_ {
    UINT32  InputClk;                       /* Sensor side input clock frequency */
    UINT32  NumTickPerXhs;                  /* XHS period (in input clock cycles) */
    UINT32  NumXhsPerH;                     /* horizontal operating period (in number of XHS pulses) */
    UINT32  NumXhsPerV;                     /* vertical operating period (in number of XHS pulses) */
    UINT32  NumXvsPerV;                     /* vertical operating period (in number of XVS pulses) */
    AMBA_DSP_FRAME_RATE_s   FrameRate;      /* framerate value of this sensor mode */
} IMX206_FRAME_TIMING_s;

typedef struct _IMX206_MODE_INFO_s_ {
    IMX206_READOUT_MODE_e   ReadoutMode;
    IMX206_FRAME_TIMING_s   FrameTiming;
} IMX206_MODE_INFO_s;

typedef struct _IMX206_CTRL_s_ {
    AMBA_SENSOR_STATUS_INFO_s   Status;
    IMX206_FRAME_TIMING_s       FrameTime;
    UINT32                      CurrentAgc;
    UINT32                      CurrentDgc;
    UINT32                      CurrentShrCtrlSVR;
    UINT32                      CurrentShrCtrlSHR;
    UINT32                      CurrentShrCtrlSPL;
} IMX206_CTRL_s;

/*-----------------------------------------------------------------------------------------------*\
 *  Application Mode Definition
\*-----------------------------------------------------------------------------------------------*/
typedef enum _AMBA_SENSOR_IMX206_MODE_ID_e_ {
    AMBA_SENSOR_IMX206_TYPE_2_3_MODE_2_60P = 0,
    AMBA_SENSOR_IMX206_TYPE_2_3_MODE_2A_30P,
    AMBA_SENSOR_IMX206_TYPE_2_3_MODE_0_10P,
    AMBA_SENSOR_IMX206_TYPE_2_3_MODE_4_240P,
    AMBA_SENSOR_IMX206_TYPE_2_3_MODE_2_50P,
    AMBA_SENSOR_IMX206_TYPE_2_3_MODE_2A_25P,
    AMBA_SENSOR_IMX206_TYPE_2_3_MODE_4_200P,
    AMBA_SENSOR_IMX206_TYPE_2_3_MODE_3B_120P,
    AMBA_SENSOR_IMX206_TYPE_2_3_MODE_3B_100P,
    AMBA_SENSOR_IMX206_TYPE_2_3_MODE_3_60P,
    AMBA_SENSOR_IMX206_TYPE_2_3_MODE_3_50P,
    AMBA_SENSOR_IMX206_TYPE_2_3_MODE_4_120P,
    AMBA_SENSOR_IMX206_TYPE_2_3_MODE_4_100P,
    AMBA_SENSOR_IMX206_TYPE_2_3_MODE_5_30P,
    AMBA_SENSOR_IMX206_TYPE_2_3_MODE_5_25P,

    AMBA_SENSOR_IMX206_NUM_MODE,
} AMBA_SENSOR_IMX206_MODE_ID_e;

/*-----------------------------------------------------------------------------------------------*\
 *  Defined in AmbaSensor_IMX206.c
\*-----------------------------------------------------------------------------------------------*/
extern const AMBA_SENSOR_DEVICE_INFO_s IMX206DeviceInfo;
extern const AMBA_SENSOR_INPUT_INFO_s IMX206InputInfoNormalReadout[IMX206_NUM_READOUT_MODE];
extern const AMBA_SENSOR_INPUT_INFO_s IMX206InputInfoInversionReadout[IMX206_NUM_READOUT_MODE];
extern const AMBA_SENSOR_OUTPUT_INFO_s IMX206OutputInfo[IMX206_NUM_READOUT_MODE];
extern const IMX206_MODE_INFO_s IMX206ModeInfoList[AMBA_SENSOR_IMX206_NUM_MODE];

extern IMX206_REG_s IMX206RegTable[IMX206_NUM_READOUT_MODE_REG];

#endif /* _AMBA_SENSOR_IMX206_H_ */
