/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaSensor_AR0230.h
 *
 *  @Copyright      :: Copyright (C) 2014 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Control APIs of Aptina AR0230 CMOS sensor with HiSPi interface
 *
\*-------------------------------------------------------------------------------------------------------------------*/

#ifndef _AMBA_SENSOR_AR0230_H_
#define _AMBA_SENSOR_AR0230_H_

#define NA (0xff)

/*-----------------------------------------------------------------------------------------------*\
 * Control registers
\*-----------------------------------------------------------------------------------------------*/
#define AR0230_NUM_READOUT_MODE_REG         17
#define AR0230_NUM_SEQUENCER_OPTIMIZED_REG  25
#define AR0230_NUM_SEQUENCER_LINEAR_REG     176
#define AR0230_NUM_SEQUENCER_HDR_REG        286
#define AR0230_NUM_AGC_STEP                 52

typedef struct _AR0230_FRAME_TIMING_s_ {
    UINT32  InputClk;                       /* Sensor side input clock frequency */
    UINT32  Linelengthpck;                  /* XHS period (in input clock cycles) */
    UINT32  FrameLengthLines;               /* horizontal operating period (in number of XHS pulses) */
    AMBA_DSP_FRAME_RATE_s   FrameRate;      /* framerate value of this sensor mode */
} AR0230_FRAME_TIMING_s;

typedef struct _AR0230_CTRL_s_ {
    AMBA_SENSOR_STATUS_INFO_s   Status;
    UINT32                      CurrentHDRRatio; /* T1/T2 ratio */
    UINT32                      CurrentShtCtrl;  /* Shutter setting */
    UINT32                      CurrentAgc;      /* AGC table index */
    UINT32                      CurrentDgc;      /* DGC setting */
} AR0230_CTRL_s;

typedef enum _AR0230_OPERATION_MODE_e_ {
    AR0230_OPERATION_MODE_LINEAR = 0,
    AR0230_OPERATION_MODE_INTERLEAVE,
    AR0230_OPERATION_MODE_HDR_ALTM,
    AR0230_OPERATION_MODE_HDR_LINEAR,

    AR0230_NUM_OPERATION_MODE,
} AR0230_OPERATION_MODE_e;

typedef enum _AR0230_PLL_e_ {
    AR0230_PLL_1 = 0,
    AR0230_PLL_2,
    AR0230_PLL_3,
    AR0230_PLL_4
} AR0230_PLL_e;

typedef enum _AMBA_SENSOR_AR0230_MODE_ID_e_ {
    AMBA_SENSOR_AR0230_12_1920_1080_30P_HDR_14BIT = 0,
    AMBA_SENSOR_AR0230_12_1920_1080_60P_HDR_14BIT,
    AMBA_SENSOR_AR0230_12_1920_1080_30P_HDR_12BIT,
    AMBA_SENSOR_AR0230_12_1920_1080_60P_HDR_12BIT,
    AMBA_SENSOR_AR0230_12_1920_1080_30P_Interleave,
    AMBA_SENSOR_AR0230_12_1920_1080_30P,
    AMBA_SENSOR_AR0230_12_1920_1080_60P,

    AMBA_SENSOR_AR0230_12_1920_1080_25P_HDR_14BIT,
    AMBA_SENSOR_AR0230_12_1920_1080_50P_HDR_14BIT,
    AMBA_SENSOR_AR0230_12_1920_1080_25P_HDR_12BIT,
    AMBA_SENSOR_AR0230_12_1920_1080_50P_HDR_12BIT,
    AMBA_SENSOR_AR0230_12_1920_1080_25P_Interleave,
    AMBA_SENSOR_AR0230_12_1920_1080_25P,
    AMBA_SENSOR_AR0230_12_1920_1080_50P,

    AMBA_SENSOR_AR0230_NUM_MODE,
    AMBA_SENSOR_AR0230_NUM_ORIGINAL_MODE = 5,
} AMBA_SENSOR_AR0230_MODE_ID_e;

typedef enum _AR0230_LVDS_CHANNEL_SELECT_e_ {
    AR0230_LVDS_10CH = 0,
    AR0230_LVDS_8CH,
    AR0230_LVDS_6CH,
    AR0230_LVDS_4CH,
    AR0230_LVDS_2CH,
    AR0230_LVDS_1CH
} AR0230_LVDS_CHANNEL_SELECT_e;

typedef enum _AR0230_ADACD_OPERATION_e_ {
    AR0230_ADACD_DISABLE = 0,
    AR0230_ADACD_LOWLIGHT,
    AR0230_ADACD_LOWDCG,
    AR0230_ADACD_HIGHDCG,
} AR0230_ADACD_OPERATION_e;

typedef enum _AR0230_CONVERSION_GAIN_e_ {
    AR0230_LOW_CONVERSION_GAIN = 0,
    AR0230_HIGH_CONVERSION_GAIN
} AR0230_CONVERSION_GAIN_e;

typedef struct _AR0230_MODE_INFO_s_ {
    AR0230_OPERATION_MODE_e    OperationMode;
    AR0230_FRAME_TIMING_s      FrameTiming;
    AMBA_SENSOR_HDR_TYPE_e     HdrType;
    UINT8                      ActiveChannels;
    AR0230_PLL_e               PllSetting;
    float                      RowTime;   /* calculated in AR0230_Init */
    float                      PixelRate; /* calculated in AR0230_Init */
} AR0230_MODE_INFO_s;

typedef struct _AR0230_REG_s_ {
    UINT16  Addr;
    UINT16  Data[AMBA_SENSOR_AR0230_NUM_MODE];
} AR0230_REG_s;

typedef struct _AR0230_SEQ_REG_s_ {
    UINT16  Addr;
    UINT16  Data[1];
} AR0230_SEQ_REG_s;

typedef struct _AR0230_AGC_REG_s_ {
    float   Factor;
    UINT16  Data;
    AR0230_CONVERSION_GAIN_e ConvGain;
} AR0230_AGC_REG_s;
/*-----------------------------------------------------------------------------------------------*\
 *  Defined in AmbaSensor_AR0230.c
\*-----------------------------------------------------------------------------------------------*/
extern const AMBA_SENSOR_DEVICE_INFO_s AR0230DeviceInfo;
extern const AMBA_SENSOR_INPUT_INFO_s AR0230InputInfo[AMBA_SENSOR_AR0230_NUM_MODE];
extern const AMBA_SENSOR_OUTPUT_INFO_s AR0230OutputInfo[AMBA_SENSOR_AR0230_NUM_MODE];
extern const AMBA_SENSOR_HDR_CHANNEL_INFO_s AR0230HdrLongExposureInfo[AMBA_SENSOR_AR0230_NUM_MODE];
extern const AMBA_SENSOR_HDR_CHANNEL_INFO_s AR0230HdrShortExposureInfo[AMBA_SENSOR_AR0230_NUM_MODE];

extern AR0230_MODE_INFO_s AR0230ModeInfoList[AMBA_SENSOR_AR0230_NUM_MODE];
extern AR0230_REG_s AR0230RegTable[AR0230_NUM_READOUT_MODE_REG];
extern AR0230_SEQ_REG_s AR0230OptSeqRegTable[AR0230_NUM_SEQUENCER_OPTIMIZED_REG];
extern AR0230_SEQ_REG_s AR0230LinearSeqRegTable[AR0230_NUM_SEQUENCER_LINEAR_REG];
extern AR0230_SEQ_REG_s AR0230HDRSeqRegTable[AR0230_NUM_SEQUENCER_HDR_REG];
extern AR0230_AGC_REG_s AR0230AgcRegTable[AR0230_NUM_AGC_STEP];

#endif /* _AMBA_SENSOR_AR0230_H_ */
