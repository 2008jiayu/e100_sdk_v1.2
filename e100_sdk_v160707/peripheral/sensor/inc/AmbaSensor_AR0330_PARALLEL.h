/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaSensor_AR0330_PARALLEL.h
 *
 *  @Copyright      :: Copyright (C) 2014 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Control APIs of Aptina AR0330 CMOS sensor with PARALLEL interface
 *
\*-------------------------------------------------------------------------------------------------------------------*/

#ifndef _AMBA_SENSOR_AR0330_PARALLEL_H_
#define _AMBA_SENSOR_AR0330_PARALLEL_H_

#define NA (0xff)

/*-----------------------------------------------------------------------------------------------*\
 * Control registers
\*-----------------------------------------------------------------------------------------------*/
#define AR0330_PARALLEL_NUM_READOUT_MODE_REG         19
#define AR0330_PARALLEL_NUM_PLL_REG                  8
#define AR0330_PARALLEL_NUM_SEQUENCER_A_REG          149
#define AR0330_PARALLEL_NUM_SEQUENCER_B_REG          160

#define AR0330_PARALLEL_RESOLUTION_REG_OFFSET        0   /* Resolution setting */
#define AR0330_PARALLEL_ADC_REG_OFFSET               12  /* ADC control        */
#define AR0330_PARALLEL_SEQ_REG_OFFSET               13  /* Sequencer patch    */
#define AR0330_PARALLEL_RESERVED_REG_OFFSET          15  /* Reserved           */

#define AR0330_PARALLEL_RESOLUTION_REG_NUM    (AR0330_PARALLEL_ADC_REG_OFFSET - \
                                               AR0330_PARALLEL_RESOLUTION_REG_OFFSET)

#define AR0330_PARALLEL_ADC_REG_NUM           (AR0330_PARALLEL_SEQ_REG_OFFSET - \
                                               AR0330_PARALLEL_ADC_REG_OFFSET)

#define AR0330_PARALLEL_SEQ_REG_NUM           (AR0330_PARALLEL_RESERVED_REG_OFFSET - \
                                               AR0330_PARALLEL_SEQ_REG_OFFSET)

#define AR0330_PARALLEL_RESERVED_REG_NUM      (AR0330_PARALLEL_NUM_READOUT_MODE_REG - \
                                               AR0330_PARALLEL_RESERVED_REG_OFFSET)

typedef struct _AR0330_PARALLEL_FRAME_TIMING_s_ {
    UINT32  InputClk;                       /* Sensor side input clock frequency */
    UINT32  Linelengthpck;                  /* XHS period (in input clock cycles) */
    UINT32  FrameLengthLines;               /* horizontal operating period (in number of XHS pulses) */
    AMBA_DSP_FRAME_RATE_s   FrameRate;      /* framerate value of this sensor mode */
} AR0330_PARALLEL_FRAME_TIMING_s;

typedef struct _AR0330_PARALLEL_CTRL_s_ {
    AMBA_SENSOR_STATUS_INFO_s        Status;
    AR0330_PARALLEL_FRAME_TIMING_s   FrameTime;
	UINT32                           ShutterCtrl;
	UINT32                           GainFactor;
} AR0330_PARALLEL_CTRL_s;

typedef enum _AR0330_PARALLEL_READOUT_MODE_e_ {
    AR0330_PARALLEL_READOUT_MODE_0 = 0,  /* 12bit OUT, 2304x1296, native */
    AR0330_PARALLEL_READOUT_MODE_1,
    AR0330_PARALLEL_READOUT_MODE_2,
    AR0330_PARALLEL_READOUT_MODE_3,
    AR0330_PARALLEL_READOUT_MODE_4,
    AR0330_PARALLEL_READOUT_MODE_5,
    AR0330_PARALLEL_READOUT_MODE_6,
    AR0330_PARALLEL_READOUT_MODE_7,
    AR0330_PARALLEL_READOUT_MODE_8,
    AR0330_PARALLEL_READOUT_MODE_9,
    AR0330_PARALLEL_READOUT_MODE_10,
    
    AR0330_PARALLEL_NUM_READOUT_MODE,
} AR0330_PARALLEL_READOUT_MODE_e;

typedef enum _AR0330_PARALLEL_PLL_CONFIG_e_ {
    AR0330_PARALLEL_PLL_1,   /* Parallel, 12bit, PIX_RATE=96MHz  */
    AR0330_PARALLEL_PLL_2,   /* Parallel, 12bit, PIX_RATE=96MHz  */
    AR0330_PARALLEL_PLL_3,   /* Parallel, 12bit, PIX_RATE=74MHz */
    AR0330_PARALLEL_PLL_4,   /* Parallel, 12bit, PIX_RATE=98MHz   */
    AR0330_PARALLEL_NUM_PLL_CONFIG,
} AR0330_PARALLEL_PLL_CONFIG_e;

typedef enum _AR0330_PARALLEL_INPUT_MODE_ID_e_ {
    AR0330_PARALLEL_INPUT_MODE_0 = 0,
    AR0330_PARALLEL_INPUT_MODE_1,
    AR0330_PARALLEL_INPUT_MODE_2,
    AR0330_PARALLEL_INPUT_MODE_3,
    AR0330_PARALLEL_INPUT_MODE_4,
    AR0330_PARALLEL_INPUT_MODE_5,
    
    AR0330_PARALLEL_NUM_INPUT_MODE,
} AR0330_PARALLEL_INPUT_MODE_ID_e;

typedef enum _AR0330_PARALLEL_OUTPUT_MODE_ID_e_ {
    AR0330_PARALLEL_OUTPUT_MODE_0 = 0,
    AR0330_PARALLEL_OUTPUT_MODE_1,
    AR0330_PARALLEL_OUTPUT_MODE_2,
    AR0330_PARALLEL_OUTPUT_MODE_3,
    AR0330_PARALLEL_OUTPUT_MODE_4,
    AR0330_PARALLEL_OUTPUT_MODE_5,
    
    AR0330_PARALLEL_NUM_OUTPUT_MODE,
} AR0330_PARALLEL_OUTPUT_MODE_ID_e;

typedef enum _AMBA_SENSOR_AR0330_PARALLEL_MODE_ID_e_ {
    AMBA_SENSOR_AR0330_PARALLEL_12_1920_1080_30P = 0,
    AMBA_SENSOR_AR0330_PARALLEL_12_2304_1296_30P,
    AMBA_SENSOR_AR0330_PARALLEL_12_1152_648_60P,
    AMBA_SENSOR_AR0330_PARALLEL_12_1536_1536_30P,
    AMBA_SENSOR_AR0330_PARALLEL_12_1152_768_30P,
    AMBA_SENSOR_AR0330_PARALLEL_12_2304_1536_24P,

    AMBA_SENSOR_AR0330_PARALLEL_12_1920_1080_25P,
    AMBA_SENSOR_AR0330_PARALLEL_12_2304_1296_25P,
    AMBA_SENSOR_AR0330_PARALLEL_12_1152_648_50P,
    AMBA_SENSOR_AR0330_PARALLEL_12_1536_1536_25P,
    AMBA_SENSOR_AR0330_PARALLEL_12_1152_768_25P,
    
    AMBA_SENSOR_AR0330_PARALLEL_NUM_MODE,
} AMBA_SENSOR_AR0330_PARALLEL_MODE_ID_e;

typedef struct _AR0330_PARALLEL_MODE_INFO_s_ {
    AR0330_PARALLEL_READOUT_MODE_e      ReadoutMode;
    AR0330_PARALLEL_PLL_CONFIG_e        PllConfig;
    AR0330_PARALLEL_INPUT_MODE_ID_e     InputMode;
    AR0330_PARALLEL_OUTPUT_MODE_ID_e    OutputMode;
    AR0330_PARALLEL_FRAME_TIMING_s      FrameTiming;
} AR0330_PARALLEL_MODE_INFO_s;

typedef struct _AR0330_PARALLEL_REG_s_ {
    UINT16  Addr;
    UINT16  Data[AR0330_PARALLEL_NUM_READOUT_MODE];
} AR0330_PARALLEL_REG_s;

typedef struct _AR0330_PARALLEL_PLL_REG_s_ {
    UINT16  Addr;
    UINT16  Data[AR0330_PARALLEL_NUM_PLL_CONFIG];
} AR0330_PARALLEL_PLL_REG_s;

typedef struct _AR0330_PARALLEL_SEQ_REG_s_ {
    UINT16  Addr;
    UINT16  Data[1];
} AR0330_PARALLEL_SEQ_REG_s;

/*-----------------------------------------------------------------------------------------------*\
 *  Defined in AmbaSensor_AR0330_PARALLEL.c
\*-----------------------------------------------------------------------------------------------*/
extern const AMBA_SENSOR_DEVICE_INFO_s AR0330_PARALLELDeviceInfo;
extern const AMBA_SENSOR_INPUT_INFO_s AR0330_PARALLELInputInfo[AR0330_PARALLEL_NUM_INPUT_MODE];
extern const AMBA_SENSOR_OUTPUT_INFO_s AR0330_PARALLELOutputInfo[AR0330_PARALLEL_NUM_OUTPUT_MODE];
extern const AR0330_PARALLEL_MODE_INFO_s AR0330_PARALLELModeInfoList[AMBA_SENSOR_AR0330_PARALLEL_NUM_MODE];

extern AR0330_PARALLEL_REG_s AR0330_PARALLELRegTable[AR0330_PARALLEL_NUM_READOUT_MODE_REG];
extern AR0330_PARALLEL_PLL_REG_s AR0330_PARALLELPllRegTable[AR0330_PARALLEL_NUM_PLL_REG];
extern AR0330_PARALLEL_SEQ_REG_s AR0330_PARALLELSeqARegTable[AR0330_PARALLEL_NUM_SEQUENCER_A_REG];
extern AR0330_PARALLEL_SEQ_REG_s AR0330_PARALLELSeqBRegTable[AR0330_PARALLEL_NUM_SEQUENCER_B_REG];

#endif /* _AMBA_SENSOR_AR0330_PARALLEL_H_ */
