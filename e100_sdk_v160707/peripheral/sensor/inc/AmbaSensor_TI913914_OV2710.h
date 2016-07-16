
#ifndef _AMBA_SENSOR_OV2710_H_
#define _AMBA_SENSOR_OV2710_H_

#define NA (0xff)

/*-----------------------------------------------------------------------------------------------*\
 * Control registers
\*-----------------------------------------------------------------------------------------------*/
#define OV2710_GROUP_REG          0x3212
#define OV2710_EXPO_REG_HIGH      0x3500
#define OV2710_EXPO_REG_MID       0x3501
#define OV2710_EXPO_REG_LOW       0x3502
#define OV2710_AGC_REG_HIGH       0x350A
#define OV2710_AGC_REG_LOW        0x350B

#define OV2710_SLAVE_ADDR_WR 0x6c
#define OV2710_SLAVE_ADDR_RD 0x6d


#define OV2710_NUM_READOUT_MODE_REG         34
#define OV2710_NUM_INIT_MODE_REG            61

typedef struct _OV2710_FRAME_TIMING_s_ {
    UINT32  InputClk;                       /* Sensor side input clock frequency */
    UINT32  Linelengthpck;                  /* XHS period (in input clock cycles) */
    UINT32  FrameLengthLines;               /* horizontal operating period (in number of XHS pulses) */
    AMBA_DSP_FRAME_RATE_s   FrameRate;      /* framerate value of this sensor mode */
} OV2710_FRAME_TIMING_s;

typedef struct _OV2710_CTRL_s_ {
    AMBA_SENSOR_STATUS_INFO_s   Status;
    OV2710_FRAME_TIMING_s       FrameTime;
} OV2710_CTRL_s;

typedef enum _OV2710_INPUT_MODE_ID_e_ {
    OV2710_INPUT_MODE_0 = 0,
    OV2710_INPUT_MODE_1,

    OV2710_NUM_INPUT_MODE,
} OV2710_INPUT_MODE_ID_e;

typedef enum _OV2710_OUTPUT_MODE_ID_e_ {
    OV2710_OUTPUT_MODE_0 = 0,
    OV2710_OUTPUT_MODE_1,
    OV2710_OUTPUT_MODE_2,
    OV2710_OUTPUT_MODE_3,

    OV2710_NUM_OUTPUT_MODE,
} OV2710_OUTPUT_MODE_ID_e;


typedef enum _AMBA_SENSOR_OV2710_MODE_ID_e_ {
    AMBA_SENSOR_OV2710_1920_1080_30P = 0,
    AMBA_SENSOR_OV2710_1920_1080_25P,		
    AMBA_SENSOR_OV2710_1280_720_60P,
    AMBA_SENSOR_OV2710_1280_720_30P,
    AMBA_SENSOR_OV2710_1280_720_60_30P,

    AMBA_SENSOR_OV2710_NUM_MODE,
} AMBA_SENSOR_OV2710_MODE_ID_e;

typedef struct _OV2710_MODE_INFO_s_ {
    OV2710_INPUT_MODE_ID_e     InputMode;
    OV2710_OUTPUT_MODE_ID_e    OutputMode;
    OV2710_FRAME_TIMING_s      FrameTiming;
} OV2710_MODE_INFO_s;


typedef struct _OV2710_REG_s_ {
    UINT16  Addr;
    UINT8   Data[AMBA_SENSOR_OV2710_NUM_MODE];
} OV2710_REG_s;

typedef struct _OV2710_INIT_REG_s_ {
    UINT16  Addr;
    UINT8   Data;
} OV2710_INIT_REG_s;


/*-----------------------------------------------------------------------------------------------*\
 *  Defined in AmbaSensor_OV2710.c
\*-----------------------------------------------------------------------------------------------*/
extern const AMBA_SENSOR_DEVICE_INFO_s OV2710DeviceInfo;
extern const AMBA_SENSOR_INPUT_INFO_s OV2710InputInfo[OV2710_NUM_INPUT_MODE];
extern const AMBA_SENSOR_OUTPUT_INFO_s OV2710OutputInfo[AMBA_SENSOR_OV2710_NUM_MODE];
extern const OV2710_MODE_INFO_s OV2710ModeInfoList[AMBA_SENSOR_OV2710_NUM_MODE];


extern OV2710_REG_s OV2710RegTable[OV2710_NUM_READOUT_MODE_REG];
extern OV2710_INIT_REG_s OV2710InitRegTable[OV2710_NUM_INIT_MODE_REG];


#endif /* _AMBA_SENSOR_OV2710_H_ */

