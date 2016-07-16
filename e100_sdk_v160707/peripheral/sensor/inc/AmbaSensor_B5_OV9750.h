/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaSensor_B5_OV9750.h
 *
 *  @Copyright      :: Copyright (C) 2013 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Control APIs of OmniVision OV9750 CMOS sensor with MIPI interface
\*-------------------------------------------------------------------------------------------------------------------*/

#ifndef _AMBA_SENSOR_B5_OV9750_H_
#define _AMBA_SENSOR_B5_OV9750_H_

/*-----------------------------------------------------------------------------------------------*\
 * Control registers
\*-----------------------------------------------------------------------------------------------*/
#define B5_OV9750_NUM_READOUT_MODE_REG         222

typedef struct _B5_OV9750_FRAME_TIMING_s_ {
    UINT32  InputClk;                       /* Sensor side input clock frequency */
    UINT32  Linelengthpck;                  /* XHS period (in input clock cycles) */
    UINT32  FrameLengthLines;               /* Horizontal operating period (in number of XHS pulses) */
    AMBA_DSP_FRAME_RATE_s   FrameRate;      /* Frame rate value of this sensor mode */
} B5_OV9750_FRAME_TIMING_s;

typedef enum _B5_OV9750_READOUT_MODE_e_ {
    B5_OV9750_S1_10_1280_960_60P_L1 = 0,
    B5_OV9750_S1_10_1280_960_60P_L2,
    B5_OV9750_S1_10_1280_960_50P_L1,
    B5_OV9750_S1_10_1280_960_50P_L2,
    B5_OV9750_S1_10_1280_960_60P_30P_L1,
    B5_OV9750_S1_10_1280_960_60P_30P_L2,
    B5_OV9750_S1_10_1280_960_50P_25P_L1,
    B5_OV9750_S1_10_1280_960_50P_25P_L2,
    B5_OV9750_S1_10_1280_960_30P_L2,
    B5_OV9750_S1_10_1280_960_25P_L2,
    B5_OV9750_NUM_READOUT_MODE,
} B5_OV9750_READOUT_MODE_e;

typedef enum _B5_OV9750_INPUT_MODE_ID_e_ {
    B5_OV9750_INPUT_MODE_0 = 0,
    B5_OV9750_NUM_INPUT_MODE,
} B5_OV9750_INPUT_MODE_ID_e;

typedef enum _B5_OV9750_OUTPUT_MODE_ID_e_ {
    B5_OV9750_OUTPUT_MODE_0 = 0,
    B5_OV9750_OUTPUT_MODE_1,
    B5_OV9750_OUTPUT_MODE_2,
    B5_OV9750_NUM_OUTPUT_MODE,
} B5_OV9750_OUTPUT_MODE_ID_e;

typedef enum _AMBA_SENSOR_B5_OV9750_MODE_ID_e_ {
    AMBA_SENSOR_B5_OV9750_V1_10_1280_960_60P_L1 = 0,
    AMBA_SENSOR_B5_OV9750_V1_10_1280_960_60P_L2,
    AMBA_SENSOR_B5_OV9750_V1_10_1280_960_50P_L1,
    AMBA_SENSOR_B5_OV9750_V1_10_1280_960_50P_L2,
    AMBA_SENSOR_B5_OV9750_V1_10_1280_960_60P_30P_L1,
    AMBA_SENSOR_B5_OV9750_V1_10_1280_960_60P_30P_L2,
    AMBA_SENSOR_B5_OV9750_V1_10_1280_960_50P_25P_L1,
    AMBA_SENSOR_B5_OV9750_V1_10_1280_960_50P_25P_L2,
    AMBA_SENSOR_B5_OV9750_V1_10_1280_960_30P_L2,
    AMBA_SENSOR_B5_OV9750_V1_10_1280_960_25P_L2,
    AMBA_SENSOR_B5_OV9750_NUM_MODE,
} AMBA_SENSOR_B5_OV9750_MODE_ID_e;

typedef struct _B5_OV9750_REG_s_ {
    UINT16  Addr;
    UINT8   Data[B5_OV9750_NUM_READOUT_MODE];
} B5_OV9750_REG_s;

typedef struct _B5_OV9750_MODE_INFO_s_ {
    B5_OV9750_READOUT_MODE_e      ReadoutMode;
    B5_OV9750_INPUT_MODE_ID_e     InputMode;
    B5_OV9750_OUTPUT_MODE_ID_e    OutputMode;
    B5_OV9750_FRAME_TIMING_s      FrameTiming;
    UINT16  CfaOutputWidth;       /* Frame width after CFA scaling down */
} B5_OV9750_MODE_INFO_s;

typedef struct _B5_OV9750_CURRENT_AE_INFO_s_ {
    UINT32                      CurrentAgcCtrl[4];      /* B5 channel id : 0 - 3 */
    UINT32                      CurrentShutterCtrl[4];  /* B5 channel id : 0 - 3 */
} B5_OV9750_CURRENT_AE_INFO_s;

typedef struct _B5_OV9750_CTRL_s_ {
    AMBA_SENSOR_STATUS_INFO_s   Status;
    B5_OV9750_MODE_INFO_s       ModeInfo;
    B5_OV9750_CURRENT_AE_INFO_s CurrentAEInfo;
} B5_OV9750_CTRL_s;

/*-----------------------------------------------------------------------------------------------*\
 *  Defined in AmbaSensor_OV9750.c
\*-----------------------------------------------------------------------------------------------*/
extern const AMBA_SENSOR_DEVICE_INFO_s B5_OV9750DeviceInfo;
extern const AMBA_SENSOR_INPUT_INFO_s B5_OV9750InputInfo[B5_OV9750_NUM_INPUT_MODE];
extern const AMBA_SENSOR_OUTPUT_INFO_s B5_OV9750OutputInfo[B5_OV9750_NUM_OUTPUT_MODE];
extern const B5_OV9750_MODE_INFO_s B5_OV9750ModeInfoList[AMBA_SENSOR_B5_OV9750_NUM_MODE];

extern B5_OV9750_REG_s B5_OV9750RegTable[B5_OV9750_NUM_READOUT_MODE_REG];
extern AMBA_SENSOR_OUTPUT_INFO_s B5_OV9750_B5nOutputInfo[];

#endif /* _AMBA_SENSOR_B5_OV9750_H_ */
