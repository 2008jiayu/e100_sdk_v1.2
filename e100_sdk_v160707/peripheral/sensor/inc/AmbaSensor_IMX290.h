/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaSensor_IMX290.h
 *
 *  @Copyright      :: Copyright (C) 2015 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Control APIs of SONY IMX290 CMOS sensor with LVDS interface
 *
\*-------------------------------------------------------------------------------------------------------------------*/

#ifndef _AMBA_SENSOR_IMX290_H_
#define _AMBA_SENSOR_IMX290_H_

#define NA (0xffff)

/*-----------------------------------------------------------------------------------------------*\
 * Control registers
\*-----------------------------------------------------------------------------------------------*/
#define IMX290_NUM_INIT_REG                 41
#define IMX290_NUM_READOUT_MODE_REG         31

typedef enum _IMX290_CTRL_REG_e_ {
    IMX290_STANDBY                  = 0x3000,
    IMX290_MASTERSTOP               = 0x3002,
    IMX290_RESET                    = 0x3003,
    IMX290_AGAIN                    = 0x3014,
    IMX290_VMAX_LSB                 = 0x3018,
    IMX290_VMAX_MSB                 = 0x3019,
    IMX290_VMAX_HSB                 = 0x301A,
    IMX290_SHS1_LSB                 = 0x3020,
    IMX290_SHS1_MSB                 = 0x3021,
    IMX290_SHS1_HSB                 = 0x3022,
    IMX290_SHS2_LSB                 = 0x3024,
    IMX290_SHS2_MSB                 = 0x3025,
    IMX290_SHS2_HSB                 = 0x3026,
    IMX290_RHS1_LSB                 = 0x3030,
    IMX290_RHS1_MSB                 = 0x3031,
    IMX290_RHS1_HSB                 = 0x3032,
    IMX290_DOL_FORMAT               = 0x3045,
    IMX290_DOL_SYNCSIGNAL           = 0x3106,
    IMX290_DOL_HBFIXEN              = 0x3107,
    IMX290_NULL0_SIZEV              = 0x3415,
} IMX290_CTRL_REG_e;

typedef enum _IMX290_READOUT_MODE_e_ {
    IMX290_1080P120_10BIT_8CH = 0,
    IMX290_1080P60_12BIT_4CH,
    IMX290_1080P30_12BIT_2CH,
    IMX290_1080P30_12BIT_4CH_DLO_2FRAME,
    IMX290_1080P60_10BIT_8CH_DLO_2FRAME,
    IMX290_1080P60_10BIT_8CH_DLO_2FRAME_EXTEND_EXP_S,
    IMX290_1080P60_P30_10BIT_8CH_DLO_2FRAME,
    IMX290_1080P100_10BIT_8CH,
    IMX290_1080P50_12BIT_4CH,
    IMX290_1080P25_12BIT_2CH,
    IMX290_1080P25_12BIT_4CH_DLO_2FRAME,
    IMX290_1080P50_10BIT_8CH_DLO_2FRAME,
    IMX290_1080P50_10BIT_8CH_DLO_2FRAME_EXTEND_EXP_S,
    IMX290_1080P50_P25_10BIT_8CH_DLO_2FRAME,
    IMX290_NUM_READOUT_MODE,
} IMX290_READOUT_MODE_e;

typedef enum _IMX290_LVDS_CHANNEL_SELECT_e_ {
    IMX290_LVDS_8CH,
    IMX290_LVDS_4CH,
    IMX290_LVDS_2CH
} IMX290_LVDS_CHANNEL_SELECT_e;

typedef struct _IMX290_SEQ_REG_s_ {
    UINT16  Addr;
    UINT8  Data;
} IMX290_SEQ_REG_s;

typedef struct _IMX290_REG_s_ {
    UINT16  Addr;
    UINT8  Data[IMX290_NUM_READOUT_MODE];
} IMX290_REG_s;

typedef struct _IMX290_FRAME_TIMING_s_ {
    UINT32  InputClk;                       /* Sensor side input clock frequency */
    UINT32  Linelengthpck;                  /* XHS period (in input clock cycles) */
    UINT32  FrameLengthLines;               /* Horizontal operating period (in number of XHS pulses) */
    AMBA_DSP_FRAME_RATE_s   FrameRate;      /* Frame rate value of this sensor mode */
} IMX290_FRAME_TIMING_s;

typedef struct _IMX290_MODE_INFO_s_ {
    IMX290_READOUT_MODE_e   ReadoutMode;
    IMX290_FRAME_TIMING_s   FrameTiming;
} IMX290_MODE_INFO_s;

typedef struct _IMX290_CTRL_s_ {
    AMBA_SENSOR_STATUS_INFO_s   Status;
    AMBA_SENSOR_MODE_INFO_s     ModeInfo;
    UINT32                      AgcCtrl;
    UINT32                      ShutterCtrl[2];
} IMX290_CTRL_s;

/*-----------------------------------------------------------------------------------------------*\
 *  Application Mode Definition
\*-----------------------------------------------------------------------------------------------*/
typedef enum _AMBA_SENSOR_IMX290_MODE_ID_e_ {
    AMBA_SENSOR_IMX290_1080P120_10BIT = 0,
    AMBA_SENSOR_IMX290_1080P60_12BIT,
    AMBA_SENSOR_IMX290_1080P30_12BIT,
    AMBA_SENSOR_IMX290_1080P30_12BIT_DLO_2FRAME,
    AMBA_SENSOR_IMX290_1080P60_10BIT_DLO_2FRAME,
    AMBA_SENSOR_IMX290_1080P60_10BIT_DLO_2FRAME_EXTEND_EXP_S,
    AMBA_SENSOR_IMX290_1080P60_P30_10BIT_DLO_2FRAME,
    AMBA_SENSOR_IMX290_1080P100_10BIT,
    AMBA_SENSOR_IMX290_1080P50_12BIT,
    AMBA_SENSOR_IMX290_1080P25_12BIT,
    AMBA_SENSOR_IMX290_1080P25_12BIT_DLO_2FRAME,
    AMBA_SENSOR_IMX290_1080P50_10BIT_DLO_2FRAME,
    AMBA_SENSOR_IMX290_1080P50_10BIT_DLO_2FRAME_EXTEND_EXP_S,
    AMBA_SENSOR_IMX290_1080P50_P25_10BIT_DLO_2FRAME,
    AMBA_SENSOR_IMX290_NUM_MODE,
} AMBA_SENSOR_IMX290_MODE_ID_e;

/*-----------------------------------------------------------------------------------------------*\
 *  Defined in AmbaSensor_IMX290.c
\*-----------------------------------------------------------------------------------------------*/
extern const AMBA_SENSOR_DEVICE_INFO_s IMX290DeviceInfo;
extern const AMBA_SENSOR_INPUT_INFO_s IMX290InputInfo[IMX290_NUM_READOUT_MODE];
extern const AMBA_SENSOR_OUTPUT_INFO_s IMX290OutputInfo[IMX290_NUM_READOUT_MODE];
extern const IMX290_MODE_INFO_s IMX290ModeInfoList[AMBA_SENSOR_IMX290_NUM_MODE];
extern const AMBA_SENSOR_HDR_INFO_s IMX290HdrInfo[AMBA_SENSOR_IMX290_NUM_MODE];
extern IMX290_SEQ_REG_s IMX290InitRegTable[IMX290_NUM_INIT_REG];
extern IMX290_REG_s IMX290RegTable[IMX290_NUM_READOUT_MODE_REG];

#endif /* _AMBA_SENSOR_IMX290_H_ */
