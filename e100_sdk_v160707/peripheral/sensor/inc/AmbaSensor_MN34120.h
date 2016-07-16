/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaSensor_MN34120.h
 *
 *  @Copyright      :: Copyright (C) 2013 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Control APIs of SONY MN34120 CMOS sensor with LVDS interface
 *
\*-------------------------------------------------------------------------------------------------------------------*/

#ifndef _AMBA_SENSOR_MN34120_H_
#define _AMBA_SENSOR_MN34120_H_

#define NA (0xffff)

/*-----------------------------------------------------------------------------------------------*\
 * Control registers
\*-----------------------------------------------------------------------------------------------*/
#define MN34120_NUM_READOUT_MODE_REG         369
#define MN34120_TG_REG                       110
#define MN34120_NUM_TG_REG                   259

typedef enum _MN34120_CTRL_REG_e_ {
    
    MN34120_RESET                    = 0x0000,
    MN34120_SETTING                  = 0x0004,
    MN34120_UPDOWN_CONV              = 0x0008,
    MN34120_POWER_SAVING             = 0x000f,
    MN34120_MODE                     = 0x0060,
    MN34120_AGAIN                    = 0x0061,
    MN34120_DGAIN                    = 0x0062,
    MN34120_LONG_EXPOSURE            = 0x0063,
    MN34120_SHTPOS                   = 0x0064,
    MN34120_AUTOSTILL_ENABLE         = 0x0065,
    MN34120_STILL_FRAME              = 0x0066, 
} MN34120_CTRL_REG_e;

typedef enum _MN34120_READOUT_MODE_e_ {
    MN34120_MODE_1 = 0,  /* mode 1:  Full Scan 12bit FPS16.25*/
    MN34120_MODE_3 = 1,  /* mode 3:  FHD30/HD30 */
    MN34120_MODE_4 = 2,  /* mode 4:  FHD60/HD60 */
    MN34120_MODE_6 = 3,  /* mode 6:  PhotoPreview */
    MN34120_MODE_12 = 4, /* mode 12: 4k2kp30/1440p30 */
    MN34120_MODE_23 = 5, /* mode 23: HDp120 */

    MN34120_NUM_READOUT_MODE,
} MN34120_READOUT_MODE_e;

typedef enum _MN34120_LVDS_CHANNEL_SELECT_e_ {
    MN34120_LVDS_8CH,
    MN34120_LVDS_6CH,
    MN34120_LVDS_4CH,
    MN34120_LVDS_1CH
} MN34120_LVDS_CHANNEL_SELECT_e;

typedef struct _MN34120_REG_s_ {
    UINT16  Addr;
    UINT16  Data[MN34120_NUM_READOUT_MODE];
} MN34120_REG_s;

typedef struct _MN34120_FRAME_TIMING_s_ {
    UINT32  InputClk;                       /* Sensor side input clock frequency */
    UINT32  NumTickPerXhs;                  /* XHS period (in input clock cycles) */
    UINT32  NumXhsPerH;                     /* horizontal operating period (in number of XHS pulses) */
    UINT32  NumXhsPerV;                     /* vertical operating period (in number of XHS pulses) */
    UINT32  NumXvsPerV;                     /* vertical operating period (in number of XVS pulses) */
    AMBA_DSP_FRAME_RATE_s   FrameRate;      /* framerate value of this sensor mode */
} MN34120_FRAME_TIMING_s;

typedef struct _MN34120_MODE_INFO_s_ {
    MN34120_READOUT_MODE_e   ReadoutMode;
    MN34120_FRAME_TIMING_s   FrameTiming;
} MN34120_MODE_INFO_s;

typedef struct _MN34120_CTRL_s_ {
    AMBA_SENSOR_STATUS_INFO_s   Status;
    MN34120_FRAME_TIMING_s      FrameTime;
    UINT32                      GainFactor;
    UINT32                      CurrentShrCtrlSVR;
    UINT32                      CurrentShrCtrlSHR;
} MN34120_CTRL_s;

/*-----------------------------------------------------------------------------------------------*\
 *  Application Mode Definition
\*-----------------------------------------------------------------------------------------------*/
typedef enum _AMBA_SENSOR_MN34120_MODE_ID_e_ {
    AMBA_SENSOR_MN34120_MODE_1_16P = 0,
    AMBA_SENSOR_MN34120_MODE_3_30P,
    AMBA_SENSOR_MN34120_MODE_4_60P,
    AMBA_SENSOR_MN34120_MODE_4_30P,
    AMBA_SENSOR_MN34120_MODE_6_27P,
    AMBA_SENSOR_MN34120_MODE_12_30P,
    AMBA_SENSOR_MN34120_MODE_23_120P,

    AMBA_SENSOR_MN34120_MODE_3_25P,
    AMBA_SENSOR_MN34120_MODE_4_50P,
    AMBA_SENSOR_MN34120_MODE_4_25P,
    AMBA_SENSOR_MN34120_MODE_12_25P,
    AMBA_SENSOR_MN34120_MODE_23_100P,

    AMBA_SENSOR_MN34120_NUM_MODE,
} AMBA_SENSOR_MN34120_MODE_ID_e;

/*-----------------------------------------------------------------------------------------------*\
 *  Defined in AmbaSensor_MN34120.c
\*-----------------------------------------------------------------------------------------------*/
extern const AMBA_SENSOR_DEVICE_INFO_s MN34120DeviceInfo;
extern const AMBA_SENSOR_INPUT_INFO_s MN34120InputInfo[MN34120_NUM_READOUT_MODE];
extern const AMBA_SENSOR_OUTPUT_INFO_s MN34120OutputInfo[MN34120_NUM_READOUT_MODE];
extern const MN34120_MODE_INFO_s MN34120ModeInfoList[AMBA_SENSOR_MN34120_NUM_MODE];

extern MN34120_REG_s MN34120RegTable[MN34120_NUM_READOUT_MODE_REG];

#endif /* _AMBA_SENSOR_MN34120_H_ */
