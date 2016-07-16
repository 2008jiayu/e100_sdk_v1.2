/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaSensor_B5_AR0230.h
 *
 *  @Copyright      :: Copyright (C) 2014 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Control APIs of Aptina B5_AR0230 CMOS sensor with HiSPi interface
 *
\*-------------------------------------------------------------------------------------------------------------------*/

#ifndef _AMBA_SENSOR_B5_AR0230_H_
#define _AMBA_SENSOR_B5_AR0230_H_

#define NA (0xff)

/*-----------------------------------------------------------------------------------------------*\
 * Control registers
\*-----------------------------------------------------------------------------------------------*/
#define B5_AR0230_NUM_READOUT_MODE_REG         17
#define B5_AR0230_NUM_SEQUENCER_OPTIMIZED_REG  25
#define B5_AR0230_NUM_SEQUENCER_LINEAR_REG     176
#define B5_AR0230_NUM_SEQUENCER_HDR_REG        286
#define B5_AR0230_NUM_AGC_STEP                    52

typedef struct _B5_AR0230_FRAME_TIMING_s_ {
    UINT32  InputClk;                       /* Sensor side input clock frequency */
    UINT32  Linelengthpck;                  /* XHS period (in input clock cycles) */
    UINT32  FrameLengthLines;               /* horizontal operating period (in number of XHS pulses) */
    AMBA_DSP_FRAME_RATE_s   FrameRate;      /* framerate value of this sensor mode */
} B5_AR0230_FRAME_TIMING_s;

typedef struct _B5_AR0230_CTRL_s_ {
    AMBA_SENSOR_STATUS_INFO_s   Status;
    UINT32                      CurrentHDRRatio; /* T1/T2 ratio */
    UINT32                      CurrentShtCtrl;  /* Shutter setting */
    UINT32                      CurrentAgc;      /* AGC table index */
    UINT32                      CurrentDgc;      /* DGC setting */
} B5_AR0230_CTRL_s;

typedef enum _B5_AR0230_OPERATION_MODE_e_ {
    B5_AR0230_OPERATION_MODE_LINEAR = 0,
    B5_AR0230_OPERATION_MODE_INTERLEAVE,
    B5_AR0230_OPERATION_MODE_HDR_ALTM,
    B5_AR0230_OPERATION_MODE_HDR_LINEAR,

    B5_AR0230_NUM_OPERATION_MODE,
} B5_AR0230_OPERATION_MODE_e;

typedef enum _B5_AR0230_PLL_e_ {
    B5_AR0230_PLL_1 = 0,
    B5_AR0230_PLL_2,
    B5_AR0230_PLL_3,
    B5_AR0230_PLL_4
} B5_AR0230_PLL_e;

typedef enum _AMBA_SENSOR_B5_AR0230_MODE_ID_e_ {
    AMBA_SENSOR_B5_AR0230_12_1920_1080_30P_HDR_14BIT = 0,
    AMBA_SENSOR_B5_AR0230_12_1920_1080_60P_HDR_14BIT,
    AMBA_SENSOR_B5_AR0230_12_1920_1080_30P_HDR_12BIT,
    AMBA_SENSOR_B5_AR0230_12_1920_1080_60P_HDR_12BIT,
    AMBA_SENSOR_B5_AR0230_12_1920_1080_30P_Interleave,
    AMBA_SENSOR_B5_AR0230_12_1920_1080_30P,
    AMBA_SENSOR_B5_AR0230_12_1920_1080_60P,

    AMBA_SENSOR_B5_AR0230_12_1920_1080_25P_HDR_14BIT,
    AMBA_SENSOR_B5_AR0230_12_1920_1080_50P_HDR_14BIT,
    AMBA_SENSOR_B5_AR0230_12_1920_1080_25P_HDR_12BIT,
    AMBA_SENSOR_B5_AR0230_12_1920_1080_50P_HDR_12BIT,
    AMBA_SENSOR_B5_AR0230_12_1920_1080_25P_Interleave,
    AMBA_SENSOR_B5_AR0230_12_1920_1080_25P,
    AMBA_SENSOR_B5_AR0230_12_1920_1080_50P,

    AMBA_SENSOR_B5_AR0230_NUM_MODE,
    AMBA_SENSOR_B5_AR0230_NUM_ORIGINAL_MODE = 5,
} AMBA_SENSOR_B5_AR0230_MODE_ID_e;

typedef enum _B5_AR0230_LVDS_CHANNEL_SELECT_e_ {
    B5_AR0230_LVDS_10CH = 0,
    B5_AR0230_LVDS_8CH,
    B5_AR0230_LVDS_6CH,
    B5_AR0230_LVDS_4CH,
    B5_AR0230_LVDS_2CH,
    B5_AR0230_LVDS_1CH
} B5_AR0230_LVDS_CHANNEL_SELECT_e;

typedef enum _B5_AR0230_ADACD_OPERATION_e_ {
    B5_AR0230_ADACD_DISABLE = 0,
    B5_AR0230_ADACD_LOWLIGHT,
    B5_AR0230_ADACD_LOWDCG,
    B5_AR0230_ADACD_HIGHDCG,
} B5_AR0230_ADACD_OPERATION_e;

typedef enum _B5_AR0230_CONVERSION_GAIN_e_ {
    B5_AR0230_LOW_CONVERSION_GAIN = 0,
    B5_AR0230_HIGH_CONVERSION_GAIN
} B5_AR0230_CONVERSION_GAIN_e;

typedef struct _B5_AR0230_MODE_INFO_s_ {
    B5_AR0230_OPERATION_MODE_e    OperationMode;
    B5_AR0230_FRAME_TIMING_s      FrameTiming;
    AMBA_SENSOR_HDR_TYPE_e        HdrType;
    UINT8                         ActiveChannels;
    UINT16  CfaOutputWidth;     /* Frame width after CFA scaling down */
    B5_AR0230_PLL_e             PllSetting;
    float                       RowTime;        /* calculated in B5_AR0230_Init */
    float                       PixelRate;      /* calculated in B5_AR0230_Init */
} B5_AR0230_MODE_INFO_s;

typedef struct _B5_AR0230_REG_s_ {
    UINT16  Addr;
    UINT16  Data[AMBA_SENSOR_B5_AR0230_NUM_MODE];
} B5_AR0230_REG_s;

typedef struct _B5_AR0230_SEQ_REG_s_ {
    UINT16  Addr;
    UINT16  Data[1];
} B5_AR0230_SEQ_REG_s;

typedef struct _B5_AR0230_AGC_REG_s_ {
    float   Factor;
    UINT16  Data;
    B5_AR0230_CONVERSION_GAIN_e ConvGain;
} B5_AR0230_AGC_REG_s;

/*-----------------------------------------------------------------------------------------------*\
 *  Defined in AmbaSensor_B5_AR0230.c
\*-----------------------------------------------------------------------------------------------*/
extern const AMBA_SENSOR_DEVICE_INFO_s B5_AR0230DeviceInfo;
extern const AMBA_SENSOR_INPUT_INFO_s B5_AR0230InputInfo[AMBA_SENSOR_B5_AR0230_NUM_MODE];
extern const AMBA_SENSOR_OUTPUT_INFO_s B5_AR0230OutputInfo[AMBA_SENSOR_B5_AR0230_NUM_MODE];
extern const AMBA_SENSOR_HDR_CHANNEL_INFO_s B5_AR0230HdrLongExposureInfo[AMBA_SENSOR_B5_AR0230_NUM_MODE];
extern const AMBA_SENSOR_HDR_CHANNEL_INFO_s B5_AR0230HdrShortExposureInfo[AMBA_SENSOR_B5_AR0230_NUM_MODE];


extern B5_AR0230_MODE_INFO_s B5_AR0230ModeInfoList[AMBA_SENSOR_B5_AR0230_NUM_MODE];
extern B5_AR0230_REG_s B5_AR0230RegTable[B5_AR0230_NUM_READOUT_MODE_REG];
extern B5_AR0230_SEQ_REG_s B5_AR0230OptSeqRegTable[B5_AR0230_NUM_SEQUENCER_OPTIMIZED_REG];
extern B5_AR0230_SEQ_REG_s B5_AR0230LinearSeqRegTable[B5_AR0230_NUM_SEQUENCER_LINEAR_REG];
extern B5_AR0230_SEQ_REG_s B5_AR0230HDRSeqRegTable[B5_AR0230_NUM_SEQUENCER_HDR_REG];
extern B5_AR0230_AGC_REG_s B5_AR0230AgcRegTable[B5_AR0230_NUM_AGC_STEP];
extern AMBA_SENSOR_OUTPUT_INFO_s B5_AR0230_B5nOutputInfo[];

#endif /* _AMBA_SENSOR_B5_AR0230_H_ */
