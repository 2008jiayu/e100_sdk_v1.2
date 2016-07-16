/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaSensor_OV9750.h
 *
 *  @Copyright      :: Copyright (C) 2013 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Control APIs of OmniVision OV9750 CMOS sensor with MIPI interface
\*-------------------------------------------------------------------------------------------------------------------*/

#ifndef _AMBA_SENSOR_OV9750_H_
#define _AMBA_SENSOR_OV9750_H_

#define NA (0xff)

/*-----------------------------------------------------------------------------------------------*\
 * Control registers
\*-----------------------------------------------------------------------------------------------*/
#define OV9750_NUM_READOUT_MODE_REG         222

typedef struct _OV9750_FRAME_TIMING_s_ {
    UINT32  InputClk;                       /* Sensor side input clock frequency */
    UINT32  Linelengthpck;                  /* XHS period (in input clock cycles) */
    UINT32  FrameLengthLines;               /* horizontal operating period (in number of XHS pulses) */
    AMBA_DSP_FRAME_RATE_s   FrameRate;      /* framerate value of this sensor mode */
} OV9750_FRAME_TIMING_s;

typedef struct _OV9750_CURRENT_AE_INFO_s_ {
    UINT32                      CurrentAgcCtrl;
    UINT32                      CurrentShutterCtrl;
} OV9750_CURRENT_AE_INFO_s;

typedef struct _OV9750_CTRL_s_ {
    AMBA_SENSOR_STATUS_INFO_s   Status;
    OV9750_FRAME_TIMING_s       FrameTime;
    OV9750_CURRENT_AE_INFO_s    CurrentAEInfo;
} OV9750_CTRL_s;

typedef enum _OV9750_READOUT_MODE_e_ {
    OV9750_S1_10_1280_960_60P_L1 = 0,
    OV9750_S1_10_1280_960_60P_L2,
    OV9750_S1_10_1280_960_50P_L1,
    OV9750_S1_10_1280_960_50P_L2,
    OV9750_S1_10_1280_960_60P_30P_L1,
    OV9750_S1_10_1280_960_60P_30P_L2,
    OV9750_S1_10_1280_960_50P_25P_L1,
    OV9750_S1_10_1280_960_50P_25P_L2,
    OV9750_S1_10_1280_960_30P_L2,
    OV9750_S1_10_1280_960_25P_L2,
    OV9750_NUM_READOUT_MODE,
} OV9750_READOUT_MODE_e;

typedef enum _OV9750_INPUT_MODE_ID_e_ {
    OV9750_INPUT_MODE_0 = 0,
    OV9750_NUM_INPUT_MODE,
} OV9750_INPUT_MODE_ID_e;

typedef enum _OV9750_OUTPUT_MODE_ID_e_ {
    OV9750_OUTPUT_MODE_0 = 0,
    OV9750_OUTPUT_MODE_1,
    OV9750_OUTPUT_MODE_2,
    OV9750_NUM_OUTPUT_MODE,
} OV9750_OUTPUT_MODE_ID_e;

typedef enum _AMBA_SENSOR_OV9750_MODE_ID_e_ {
    AMBA_SENSOR_OV9750_V1_10_1280_960_60P_L1 = 0,
    AMBA_SENSOR_OV9750_V1_10_1280_960_60P_L2,
    AMBA_SENSOR_OV9750_V1_10_1280_960_50P_L1,
    AMBA_SENSOR_OV9750_V1_10_1280_960_50P_L2,
    AMBA_SENSOR_OV9750_V1_10_1280_960_60P_30P_L1,
    AMBA_SENSOR_OV9750_V1_10_1280_960_60P_30P_L2,
    AMBA_SENSOR_OV9750_V1_10_1280_960_50P_25P_L1,
    AMBA_SENSOR_OV9750_V1_10_1280_960_50P_25P_L2,
    AMBA_SENSOR_OV9750_V1_10_1280_960_30P_L2,
    AMBA_SENSOR_OV9750_V1_10_1280_960_25P_L2,
    AMBA_SENSOR_OV9750_NUM_MODE,
} AMBA_SENSOR_OV9750_MODE_ID_e;

typedef struct _OV9750_REG_s_ {
    UINT16  Addr;
    UINT8   Data[OV9750_NUM_READOUT_MODE];
} OV9750_REG_s;

typedef struct _OV9750_MODE_INFO_s_ {
    OV9750_READOUT_MODE_e      ReadoutMode;
    OV9750_INPUT_MODE_ID_e     InputMode;
    OV9750_OUTPUT_MODE_ID_e    OutputMode;
    OV9750_FRAME_TIMING_s      FrameTiming;
} OV9750_MODE_INFO_s;

/*-----------------------------------------------------------------------------------------------*\
 *  Defined in AmbaSensor_OV9750.c
\*-----------------------------------------------------------------------------------------------*/
extern const AMBA_SENSOR_DEVICE_INFO_s OV9750DeviceInfo;
extern const AMBA_SENSOR_INPUT_INFO_s OV9750InputInfo[OV9750_NUM_INPUT_MODE];
extern const AMBA_SENSOR_OUTPUT_INFO_s OV9750OutputInfo[OV9750_NUM_OUTPUT_MODE];
extern const OV9750_MODE_INFO_s OV9750ModeInfoList[AMBA_SENSOR_OV9750_NUM_MODE];
extern OV9750_REG_s OV9750RegTable[OV9750_NUM_READOUT_MODE_REG];

#endif /* _AMBA_SENSOR_OV9750_H_ */
