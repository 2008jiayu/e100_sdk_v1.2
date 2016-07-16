#ifndef _AMBA_SENSOR_OV10823_H_
#define _AMBA_SENSOR_OV10823_H_

#define NA (0xff)

/*-----------------------------------------------------------------------------------------------*\
 * Control registers
\*-----------------------------------------------------------------------------------------------*/
#define OV10823_SOFT_RESET_REG        0x0103
#define OV10823_GROUP_ACCESS_REG      0x3208
#define OV10823_AGC_MSB_REG           0x350a
#define OV10823_AGC_LSB_REG           0x350b
#define OV10823_EXP_MSB_REG           0x3500
#define OV10823_EXP_MID_REG           0x3501
#define OV10823_EXP_LSB_REG           0x3502
#define OV10823_WB_RED_MSB_REG        0x5056
#define OV10823_WB_RED_LSB_REG        0x5057
#define OV10823_WB_GREEN_MSB_REG      0x5058
#define OV10823_WB_GREEN_LSB_REG      0x5059
#define OV10823_WB_BLUE_MSB_REG       0x505a
#define OV10823_WB_BLUE_LSB_REG       0x505b

#define OV10823_NUM_READOUT_MODE_REG         157

typedef struct _OV10823_FRAME_TIMING_s_ {
    UINT32  InputClk;                       /* Sensor side input clock frequency */
    UINT32  Linelengthpck;                  /* XHS period (in input clock cycles) */
    UINT32  FrameLengthLines;               /* horizontal operating period (in number of XHS pulses) */
    AMBA_DSP_FRAME_RATE_s   FrameRate;      /* framerate value of this sensor mode */
} OV10823_FRAME_TIMING_s;

typedef struct _OV10823_CTRL_s_ {
    AMBA_SENSOR_STATUS_INFO_s   Status;
    OV10823_FRAME_TIMING_s       FrameTime;
} OV10823_CTRL_s;

typedef enum _OV10823_READOUT_MODE_e_ {
    OV10823_S1_10_2168_1220_30P = 0,
    OV10823_S1_10_2168_1220_60P,
    OV10823_S1_10_4336_2440_30P,

    OV10823_S1_10_2168_1220_25P,
    OV10823_S1_10_2168_1220_50P,
    OV10823_S1_10_4336_2440_25P,

    OV10823_NUM_READOUT_MODE,
} OV10823_READOUT_MODE_e;

typedef enum _OV10823_INPUT_MODE_ID_e_ {
    OV10823_INPUT_MODE_0 = 0,
    OV10823_INPUT_MODE_1,

    OV10823_NUM_INPUT_MODE,
} OV10823_INPUT_MODE_ID_e;

typedef enum _OV10823_OUTPUT_MODE_ID_e_ {
    OV10823_OUTPUT_MODE_0 = 0,
    OV10823_OUTPUT_MODE_1,
    OV10823_OUTPUT_MODE_2,
    OV10823_OUTPUT_MODE_3,

    OV10823_NUM_OUTPUT_MODE,
} OV10823_OUTPUT_MODE_ID_e;

typedef enum _AMBA_SENSOR_OV10823_MODE_ID_e_ {
    AMBA_SENSOR_OV10823_V1_10_2160_1215_30P = 0,
    AMBA_SENSOR_OV10823_V1_10_2160_1215_60P,
    AMBA_SENSOR_OV10823_V1_10_4320_2430_30P,

    AMBA_SENSOR_OV10823_V1_10_2160_1215_25P,
    AMBA_SENSOR_OV10823_V1_10_2160_1215_50P,
    AMBA_SENSOR_OV10823_V1_10_4320_2430_25P,

    AMBA_SENSOR_OV10823_NUM_MODE,
} AMBA_SENSOR_OV10823_MODE_ID_e;

typedef struct _OV10823_MODE_INFO_s_ {
    OV10823_READOUT_MODE_e      ReadoutMode;
    OV10823_INPUT_MODE_ID_e     InputMode;
    OV10823_OUTPUT_MODE_ID_e    OutputMode;
    OV10823_FRAME_TIMING_s      FrameTiming;
} OV10823_MODE_INFO_s;

typedef struct _OV10823_REG_s_ {
    UINT16  Addr;
    UINT8   Data[OV10823_NUM_READOUT_MODE];
} OV10823_REG_s;

/*-----------------------------------------------------------------------------------------------*\
 *  Defined in AmbaSensor_OV10823.c
\*-----------------------------------------------------------------------------------------------*/
extern const AMBA_SENSOR_DEVICE_INFO_s OV10823DeviceInfo;
extern const AMBA_SENSOR_INPUT_INFO_s OV10823InputInfo[OV10823_NUM_INPUT_MODE];
extern const AMBA_SENSOR_OUTPUT_INFO_s OV10823OutputInfo[OV10823_NUM_OUTPUT_MODE];
extern const OV10823_MODE_INFO_s OV10823ModeInfoList[AMBA_SENSOR_OV10823_NUM_MODE];

extern OV10823_REG_s OV10823RegTable[OV10823_NUM_READOUT_MODE_REG];

#endif /* _AMBA_SENSOR_OV10823_H_ */
