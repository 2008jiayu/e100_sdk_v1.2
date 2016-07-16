/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaSensor_OV4689.h
 *
 *  @Copyright      :: Copyright (C) 2013 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Control APIs of OmniVision OV4689 CMOS sensor with MIPI interface
\*-------------------------------------------------------------------------------------------------------------------*/

#ifndef _AMBA_SENSOR_OV4689_H_
#define _AMBA_SENSOR_OV4689_H_

#define NA (0xff)

/*-----------------------------------------------------------------------------------------------*\
 * Control registers
\*-----------------------------------------------------------------------------------------------*/
#define OV4689_NUM_READOUT_MODE_REG      54

typedef enum _OV4689_READOUT_MODE_e_ {
    OV4689_S1_10_2688_1520_60P = 0,      /* 10bit, MIPI(4-lane),  720MHz(DDR) */
    OV4689_S1_10_2688_1520_30P,          /* 10bit, MIPI(4-lane),  420MHz(DDR) */
    OV4689_S2_10_1344_760_30P,           /* 10bit, MIPI(2-lane),  240MHz(DDR) */
    OV4689_S1_10_2688_1520_HDR_30P,      /* 10bit, MIPI(4-lane),  840MHz(DDR) */
    OV4689_S1_10_2120_1196_CROP_120P,    /* 10bit, MIPI(4-lane), 1008MHz(DDR) */
    OV4689_S2_10_1344_760_120P,          /* 10bit, MIPI(4-lane),  600MHz(DDR) */
    OV4689_S1_10_2688_1520_2LANE_30P,    /* 10bit, MIPI(4-lane),  648MHz(DDR) */
    OV4689_NUM_READOUT_MODE,
} OV4689_READOUT_MODE_e;

typedef enum _AMBA_SENSOR_OV4689_MODE_ID_e_ {
    AMBA_SENSOR_OV4689_V1_10_2688_1512_60P = 0,
    AMBA_SENSOR_OV4689_V1_10_2688_1512_45P,
    AMBA_SENSOR_OV4689_V1_10_2688_1512_40P,
    AMBA_SENSOR_OV4689_V1_10_2688_1512_30P,
    AMBA_SENSOR_OV4689_V1_10_2688_1512_60P_30P,
    AMBA_SENSOR_OV4689_V1_10_2688_1512_50P,
    AMBA_SENSOR_OV4689_V1_10_2688_1512_25P,
    AMBA_SENSOR_OV4689_V1_10_2688_1512_50P_25P,
    AMBA_SENSOR_OV4689_V2_10_1344_760_30P,
    AMBA_SENSOR_OV4689_V2_10_1344_760_25P,
    AMBA_SENSOR_OV4689_V1_10_2688_1512_30P_HDR,
    AMBA_SENSOR_OV4689_V1_10_2688_1512_25P_HDR,
    AMBA_SENSOR_OV4689_V1_10_2112_1188_CROP_120P,
    AMBA_SENSOR_OV4689_V1_10_2112_1188_CROP_100P,
    AMBA_SENSOR_OV4689_V2_10_1344_760_120P,
    AMBA_SENSOR_OV4689_V2_10_1344_760_100P,
    AMBA_SENSOR_OV4689_V2_10_2688_1512_2LANE_30P,
    AMBA_SENSOR_OV4689_V2_10_2688_1512_2LANE_25P,
    AMBA_SENSOR_OV4689_NUM_MODE,
} AMBA_SENSOR_MODE_ID_e;

typedef struct _OV4689_REG_s_ {
    UINT16  Addr;
    UINT8   Data[OV4689_NUM_READOUT_MODE];
} OV4689_REG_s;

typedef struct _OV4689_FRAME_TIMING_s_ {
    UINT32  InputClk;                       /* Sensor side input clock frequency */
    UINT32  Linelengthpck;                  /* Number of pixel clock cycles per line of frame */
    UINT32  FrameLengthLines;               /* Number of lines per frame */
    AMBA_DSP_FRAME_RATE_s   FrameRate;      /* Frame rate value of this sensor mode */
} OV4689_FRAME_TIMING_s;

typedef struct _OV4689_MODE_INFO_s_ {
    OV4689_READOUT_MODE_e      ReadoutMode;
    OV4689_FRAME_TIMING_s      FrameTiming;
} OV4689_MODE_INFO_s;

typedef struct _OV4689_CURRENT_AE_INFO_s_ {
    UINT32                      CurrentAgcCtrl[2];      /* HDR id: 0 = long exposure frame, 1 = short exposure frame */
    UINT32                      CurrentDgcCtrl[2];      /* HDR id: 0 = long exposure frame, 1 = short exposure frame */
    UINT32                      CurrentShutterCtrl[2];  /* HDR id: 0 = long exposure frame, 1 = short exposure frame */
} OV4689_CURRENT_AE_INFO_s;

typedef struct _OV4689_CTRL_s_ {
    AMBA_SENSOR_STATUS_INFO_s   Status;
    AMBA_SENSOR_MODE_INFO_s     ModeInfo;
    OV4689_CURRENT_AE_INFO_s    CurrentAEInfo;
} OV4689_CTRL_s;

/*-----------------------------------------------------------------------------------------------*\
 *  Defined in AmbaSensor_OV4689.c
\*-----------------------------------------------------------------------------------------------*/
extern const AMBA_SENSOR_DEVICE_INFO_s OV4689DeviceInfo;
extern const AMBA_SENSOR_INPUT_INFO_s OV4689InputInfo[OV4689_NUM_READOUT_MODE_REG];
extern const AMBA_SENSOR_OUTPUT_INFO_s OV4689OutputInfo[OV4689_NUM_READOUT_MODE_REG];
extern const OV4689_REG_s OV4689RegTable[OV4689_NUM_READOUT_MODE_REG];
extern const OV4689_MODE_INFO_s OV4689ModeInfoList[AMBA_SENSOR_OV4689_NUM_MODE];
extern const AMBA_SENSOR_HDR_INFO_s OV4689HdrInfo[AMBA_SENSOR_OV4689_NUM_MODE];

#endif /* _AMBA_SENSOR_OV4689_H_ */
