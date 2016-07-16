/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaSensor_B5_OV4689.h
 *
 *  @Copyright      :: Copyright (C) 2013 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Control APIs of OmniVision OV4689 CMOS sensor with MIPI interface
\*-------------------------------------------------------------------------------------------------------------------*/

#ifndef _AMBA_SENSOR_B5_OV4689_H_
#define _AMBA_SENSOR_B5_OV4689_H_

#define NA (0xff)

/*-----------------------------------------------------------------------------------------------*\
 * Control registers
\*-----------------------------------------------------------------------------------------------*/
#define B5_OV4689_NUM_READOUT_MODE_REG         54

typedef struct _B5_OV4689_FRAME_TIMING_s_ {
    UINT32  InputClk;                       /* Sensor side input clock frequency */
    UINT32  Linelengthpck;                  /* XHS period (in input clock cycles) */
    UINT32  FrameLengthLines;               /* Horizontal operating period (in number of XHS pulses) */
    AMBA_DSP_FRAME_RATE_s   FrameRate;      /* Frame rate value of this sensor mode */
} B5_OV4689_FRAME_TIMING_s;

typedef struct _B5_OV4689_HDR_INFO_s_ {
    UINT32 HdrIsSupport;
    UINT32 MaxExposureStepPerLongFrame;
    UINT32 MaxExposureStepPerShortFrame;
} B5_OV4689_HDR_INFO_s;

typedef enum _B5_OV4689_READOUT_MODE_e_ {
    B5_OV4689_S1_10_2688_1520_60P = 0,
    B5_OV4689_S1_10_2688_1520_45P,
    B5_OV4689_S1_10_2688_1520_40P,
    B5_OV4689_S1_10_2688_1520_30P,
    B5_OV4689_S1_10_2688_1520_60P_30P,
    B5_OV4689_S2_10_1344x760_30P,
    B5_OV4689_S1_10_2688_1520_50P,
    B5_OV4689_S1_10_2688_1520_25P,
    B5_OV4689_S1_10_2688_1520_50P_25P,
    B5_OV4689_S2_10_1344x760_25P,
    B5_OV4689_S1_10_2688_1520_HDR_30P,
    B5_OV4689_S1_10_2688_1520_HDR_25P,
    B5_OV4689_NUM_READOUT_MODE,
} B5_OV4689_READOUT_MODE_e;

typedef enum _B5_OV4689_INPUT_MODE_ID_e_ {
    B5_OV4689_INPUT_MODE_0 = 0,
    B5_OV4689_INPUT_MODE_1,
    B5_OV4689_INPUT_MODE_2_HDR,
    B5_OV4689_NUM_INPUT_MODE,
} B5_OV4689_INPUT_MODE_ID_e;

typedef enum _B5_OV4689_OUTPUT_MODE_ID_e_ {
    B5_OV4689_OUTPUT_MODE_0 = 0,
    B5_OV4689_OUTPUT_MODE_1,
    B5_OV4689_OUTPUT_MODE_2,
    B5_OV4689_OUTPUT_MODE_3,
    B5_OV4689_OUTPUT_MODE_4,
    B5_OV4689_OUTPUT_MODE_5,
    B5_OV4689_OUTPUT_MODE_6,
    B5_OV4689_OUTPUT_MODE_7_HDR,
    B5_OV4689_NUM_OUTPUT_MODE,
} B5_OV4689_OUTPUT_MODE_ID_e;

typedef enum _AMBA_SENSOR_B5_OV4689_MODE_ID_e_ {
    AMBA_SENSOR_B5_OV4689_V1_10_2688_1512_60P = 0,
    AMBA_SENSOR_B5_OV4689_V1_10_2688_1512_45P,
    AMBA_SENSOR_B5_OV4689_V1_10_2688_1512_40P,
    AMBA_SENSOR_B5_OV4689_V1_10_2688_1512_30P,
    AMBA_SENSOR_B5_OV4689_V1_10_2688_1512_60P_30P,
    AMBA_SENSOR_B5_OV4689_V1_10_2688_1152_60P,
    AMBA_SENSOR_B5_OV4689_V1_10_2688_1152_45P,
    AMBA_SENSOR_B5_OV4689_V1_10_2688_1152_40P,
    AMBA_SENSOR_B5_OV4689_V1_10_2688_1152_30P,
    AMBA_SENSOR_B5_OV4689_V1_10_2688_1152_60P_30P,
    AMBA_SENSOR_B5_OV4689_V1_10_2016_1512_60P,
    AMBA_SENSOR_B5_OV4689_V1_10_2016_1512_45P,
    AMBA_SENSOR_B5_OV4689_V1_10_2016_1512_40P,
    AMBA_SENSOR_B5_OV4689_V1_10_2016_1512_30P,
    AMBA_SENSOR_B5_OV4689_V1_10_2016_1512_60P_30P,
    AMBA_SENSOR_B5_OV4689_V1_10_2688_1512_50P,
    AMBA_SENSOR_B5_OV4689_V1_10_2688_1512_25P,
    AMBA_SENSOR_B5_OV4689_V1_10_2688_1512_50P_25P,
    AMBA_SENSOR_B5_OV4689_V1_10_2688_1152_50P,
    AMBA_SENSOR_B5_OV4689_V1_10_2688_1152_25P,
    AMBA_SENSOR_B5_OV4689_V1_10_2688_1152_50P_25P,
    AMBA_SENSOR_B5_OV4689_V1_10_2016_1512_50P,
    AMBA_SENSOR_B5_OV4689_V1_10_2016_1512_25P,
    AMBA_SENSOR_B5_OV4689_V1_10_2016_1512_50P_25P,
    AMBA_SENSOR_B5_OV4689_V2_10_1344x760_30P,
    AMBA_SENSOR_B5_OV4689_V2_10_1344x760_25P,
    AMBA_SENSOR_B5_OV4689_V1_10_2688_1512_30P_HDR,
    AMBA_SENSOR_B5_OV4689_V1_10_2688_1512_25P_HDR,
    AMBA_SENSOR_B5_OV4689_NUM_MODE,
} AMBA_SENSOR_B5_OV4689_MODE_ID_e;

typedef struct _B5_OV4689_REG_s_ {
    UINT16  Addr;
    UINT8   Data[B5_OV4689_NUM_READOUT_MODE];
} B5_OV4689_REG_s;

typedef struct _B5_OV4689_MODE_INFO_s_ {
    B5_OV4689_READOUT_MODE_e      ReadoutMode;
    B5_OV4689_INPUT_MODE_ID_e     InputMode;
    B5_OV4689_OUTPUT_MODE_ID_e    OutputMode;
    B5_OV4689_FRAME_TIMING_s      FrameTiming;
    B5_OV4689_HDR_INFO_s          HdrInfo;
    UINT16  CfaOutputWidth;       /* Frame width after B5 pre-scale */
} B5_OV4689_MODE_INFO_s;

typedef struct _B5_OV4689_CURRENT_AE_INFO_s_ {
    UINT32                      CurrentAgcCtrl[4][2];      /* B5 channel id : 0 - 3, HDR id: 0 = long exposure frame, 1 = short exposure frame */
    UINT32                      CurrentDgcCtrl[4][2];      /* B5 channel id : 0 - 3, HDR id: 0 = long exposure frame, 1 = short exposure frame */
    UINT32                      CurrentShutterCtrl[4][2];  /* B5 channel id : 0 - 3, HDR id: 0 = long exposure frame, 1 = short exposure frame */
} B5_OV4689_CURRENT_AE_INFO_s;

typedef struct _B5_OV4689_CTRL_s_ {
    AMBA_SENSOR_STATUS_INFO_s      Status;
    B5_OV4689_MODE_INFO_s          ModeInfo;
    B5_OV4689_HDR_INFO_s           HdrInfo;
    B5_OV4689_CURRENT_AE_INFO_s    CurrentAEInfo;
} B5_OV4689_CTRL_s;

/*-----------------------------------------------------------------------------------------------*\
 *  Defined in AmbaSensor_OV4689.c
\*-----------------------------------------------------------------------------------------------*/
extern const AMBA_SENSOR_DEVICE_INFO_s B5_OV4689DeviceInfo;
extern const AMBA_SENSOR_INPUT_INFO_s B5_OV4689InputInfo[B5_OV4689_NUM_INPUT_MODE];
extern const AMBA_SENSOR_OUTPUT_INFO_s B5_OV4689OutputInfo[B5_OV4689_NUM_OUTPUT_MODE];
extern const B5_OV4689_MODE_INFO_s B5_OV4689ModeInfoList[AMBA_SENSOR_B5_OV4689_NUM_MODE];

extern B5_OV4689_REG_s B5_OV4689RegTable[B5_OV4689_NUM_READOUT_MODE_REG];
extern AMBA_SENSOR_OUTPUT_INFO_s B5_OV4689_B5nOutputInfo[];

#endif /* _AMBA_SENSOR_B5_OV4689_H_ */
