
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "AmbaDataType.h"
#include "AmbaDSP.h"
#include "AmbaSensor.h"
#include "AmbaSensor_TI913914_OV2710.h"

/*-----------------------------------------------------------------------------------------------*\
 * Sensor Device Information
\*-----------------------------------------------------------------------------------------------*/
const AMBA_SENSOR_DEVICE_INFO_s OV2710DeviceInfo = {
    .UnitCellWidth          = 3.0,
    .UnitCellHeight         = 3.0,
    .NumTotalPixelCols      = 1952,
    .NumTotalPixelRows      = 1092,
    .NumEffectivePixelCols  = 1920,
    .NumEffectivePixelRows  = 1080,
    .MinAnalogGainFactor    = 1.0,
    .MaxAnalogGainFactor    = 15.5, /* 24 dB */
    .MinDigitalGainFactor   = 1.0,
    .MaxDigitalGainFactor   = 4.0,  /* 12 dB */

    .FrameRateCtrlInfo = {
        .CommunicationTime      = AMBA_SENSOR_COMMUNICATION_AT_VBLANK,
        .FirstReflectedFrame    = 2,
        .FirstBadFrame          = 1,
        .NumBadFrames           = 1
    },
    .ShutterSpeedCtrlInfo = {
        .CommunicationTime      = AMBA_SENSOR_COMMUNICATION_AT_VBLANK,
        .FirstReflectedFrame    = 1,
        .FirstBadFrame          = 0,
        .NumBadFrames           = 0
    },
    .AnalogGainCtrlInfo = {
        .CommunicationTime      = AMBA_SENSOR_COMMUNICATION_AT_VBLANK,
        .FirstReflectedFrame    = 0,
        .FirstBadFrame          = 0,
        .NumBadFrames           = 0
    },
    .DigitalGainCtrlInfo = {
        .CommunicationTime      = AMBA_SENSOR_COMMUNICATION_AT_VBLANK,
        .FirstReflectedFrame    = 0,
        .FirstBadFrame          = 0,
        .NumBadFrames           = 0
    },
};

/*-----------------------------------------------------------------------------------------------*\
 * Sensor register settings of each readout modes
\*-----------------------------------------------------------------------------------------------*/
OV2710_REG_s OV2710RegTable[OV2710_NUM_READOUT_MODE_REG] = {
    /*        0                      */
    {0x3800, 0x01, 0x01, 0x01, 0x01, 0x01}, // HREF horizontal start [11:8]
    {0x3801, 0xD2, 0xD2, 0xD6, 0xD6, 0xD6}, // HREF horizontal start [7:0]
    {0x3802, 0x00, 0x00, 0x00, 0x00, 0x00}, // HREF vertical start [11:8]
    {0x3803, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A}, // HREF vertical start [7:0]
    {0x3804, 0x07, 0x07, 0x05, 0x05, 0x05}, // HREF horizontal width [11:8]
    {0x3805, 0x88, 0x88, 0x08, 0x08, 0x08}, // HREF horizontal width [7:0]
    {0x3806, 0x04, 0x04, 0x02, 0x02, 0x02}, // HREF vertical height [11:8]
    {0x3807, 0x40, 0x40, 0xD8, 0xD8, 0xD8}, // HREF vertical height [7:0]
    {0x3808, 0x07, 0x07, 0x05, 0x05, 0x05}, // DVP output horizontal width [11:8]
    {0x3809, 0x88, 0x88, 0x08, 0x08, 0x08}, // DVP output horizontal width [7:0]
    {0x380A, 0x04, 0x04, 0x02, 0x02, 0x02}, // DVP output vertical height [11:8]
    {0x380B, 0x40, 0x40, 0xD8, 0xD8, 0xD8}, // DVP output vertical height [7:0]
    {0x380C, 0x09, 0x09, 0x07, 0x07, 0x07}, // Total horizontal size [11:8]
    {0x380D, 0xEC, 0xEC, 0x5A, 0x5A, 0x5A}, // Total horizontal size [7:0]
    {0x380E, 0x04, 0x05, 0x02, 0x02, 0x05}, // Total vertical size [11:8]
    {0x380F, 0x50, 0x2c, 0xE8, 0xE8, 0xD0}, // Total vertical size [7:0]
    {0x3810, 0x08, 0x08, 0x08, 0x08, 0x08}, // ?
    {0x3811, 0x02, 0x02, 0x02, 0x02, 0x02}, // ?
    {0x381C, 0x00, 0x00, 0x10, 0x10, 0x10}, // ?
    {0x381D, 0x02, 0x02, 0xBE, 0xBE, 0xBE}, // ?
    {0x381E, 0x04, 0x04, 0x02, 0x02, 0x02}, // ?
    {0x381F, 0x44, 0x44, 0xDC, 0xDC, 0xDC}, // ?
    {0x3820, 0x00, 0x00, 0x09, 0x09, 0x09}, // ?
    {0x3821, 0x00, 0x00, 0x29, 0x29, 0x29}, // ?
    {0x5684, 0x07, 0x07, 0x05, 0x05, 0x05}, // ?
    {0x5685, 0x88, 0x88, 0x00, 0x00, 0x00}, // ?
    {0x5686, 0x04, 0x04, 0x02, 0x02, 0x02}, // ?
    {0x5687, 0x40, 0x40, 0xD0, 0xD0, 0xD0}, // ?
    {0x3a08, 0x14, 0x14, 0x1B, 0x1B, 0x1B}, // ?
    {0x3a09, 0xB3, 0xB3, 0xE6, 0xE6, 0xE6}, // ?
    {0x3a0a, 0x11, 0x11, 0x17, 0x17, 0x17}, // ?
    {0x3a0b, 0x40, 0x40, 0x40, 0x40, 0x40}, // ?
    {0x3a0d, 0x04, 0x04, 0x02, 0x02, 0x02}, // ?
    {0x3a0e, 0x03, 0x03, 0x01, 0x01, 0x01}, // ?
};

OV2710_INIT_REG_s OV2710InitRegTable[OV2710_NUM_INIT_MODE_REG] = {
    /*        0                      */
    {0x3103, 0x93},
    {0x3008, 0x82},
    {0x3706, 0x61},
    {0x3712, 0x0c},
    {0x3630, 0x6d},
    {0x3801, 0xb4},
    {0x3621, 0x04},
    {0x3604, 0x60},
    {0x3603, 0xa7},
    {0x3631, 0x26},
    {0x3600, 0x04},
    {0x3620, 0x37},
    {0x3623, 0x00},
    {0x3702, 0x9e},
    {0x3703, 0x61},
    {0x3704, 0x44},
    {0x370d, 0x0f},
    {0x3713, 0x9f},
    {0x3714, 0x4c},
    {0x3710, 0x9e},
    {0x3801, 0xc4},
    {0x3605, 0x05},
    {0x3606, 0x3f},
    {0x302d, 0x90},
    {0x370b, 0x40},
    {0x3716, 0x31},
    {0x3707, 0x52},
    {0x380d, 0x74},
    {0x5181, 0x20},
    {0x518f, 0x00},
    {0x4301, 0xff},
    {0x4303, 0x00},
    {0x3a00, 0x78},
    {0x300f, 0x88},
    {0x3011, 0x2a},
    {0x3a1a, 0x06},
    {0x3a18, 0x00},
    {0x3a19, 0x7a},
    {0x3a13, 0x54},
    {0x382e, 0x0f},
    {0x381a, 0x1a},
    {0x401d, 0x02},
    {0x5688, 0x03},
    {0x3010, 0x00},
    {0x3a0f, 0x40},
    {0x3a10, 0x38},
    {0x3a1b, 0x48},
    {0x3a1e, 0x30},
    {0x3a11, 0x90},
    {0x3a1f, 0x10},
    {0x3017, 0x7F},
    {0x3018, 0xFC},
    {0x5000, 0x5f},
    {0x5001, 0x4e},
    {0x5002, 0xe0},
    {0x3503, 0x07},
    {0x350C, 0x00},
    {0x350D, 0x00},
    {0x4000, 0x05},
    {0x4006, 0x00},
    {0x4007, 0x20},
#if 0// Mirror on and flip on
    {0x3621, 0x14},
    {0x3818, 0xe0},
#endif
};


const AMBA_SENSOR_OUTPUT_INFO_s OV2710OutputInfo[] = {
    {84040759,  10, 10, AMBA_DSP_BAYER_BG, {AMBA_DSP_PHASE_SHIFT_MODE_0, AMBA_DSP_PHASE_SHIFT_MODE_0}, 1928, 1088, {   4,   4, 1920, 1080}, {0}},  /* Mode 0  */
    {83928551,  10, 10, AMBA_DSP_BAYER_BG, {AMBA_DSP_PHASE_SHIFT_MODE_0, AMBA_DSP_PHASE_SHIFT_MODE_0}, 1288,  728, {   4,   4, 1280,  720}, {0}},  /* Mode 0  */
    {41964275,  10, 10, AMBA_DSP_BAYER_BG, {AMBA_DSP_PHASE_SHIFT_MODE_0, AMBA_DSP_PHASE_SHIFT_MODE_0}, 1288,  728, {   4,   4, 1280,  720}, {0}},  /* Mode 0  */
    {83928551,  10, 10, AMBA_DSP_BAYER_BG, {AMBA_DSP_PHASE_SHIFT_MODE_0, AMBA_DSP_PHASE_SHIFT_MODE_0}, 1288,  728, {   4,   4, 1280,  720}, {0}},  /* Mode 0  */
};

const AMBA_SENSOR_INPUT_INFO_s OV2710InputInfo[] = {
    {{  0,   0, 1928, 1088}, {AMBA_SENSOR_SUBSAMPLE_TYPE_NORMAL,   1, 1}, {AMBA_SENSOR_SUBSAMPLE_TYPE_NORMAL,   1, 1}},   /* Mode 0  */
    {{320, 180, 1288,  728}, {AMBA_SENSOR_SUBSAMPLE_TYPE_NORMAL,   1, 1}, {AMBA_SENSOR_SUBSAMPLE_TYPE_NORMAL,   1, 1}},   /* Mode 0  */
};

const OV2710_MODE_INFO_s OV2710ModeInfoList[AMBA_SENSOR_OV2710_NUM_MODE] = {
    {OV2710_INPUT_MODE_0, OV2710_OUTPUT_MODE_0, 24011645,  2540,    1104,  { .Interlace = 0, .TimeScale = 30000,    .NumUnitsInTick = 1001 }}, /* Mode 0  */
	{OV2710_INPUT_MODE_0, OV2710_OUTPUT_MODE_0, 24021142,  2540,	1324,  { .Interlace = 0, .TimeScale = 25	,	.NumUnitsInTick = 1	   }}, /* Mode 0  */
	{OV2710_INPUT_MODE_1, OV2710_OUTPUT_MODE_1, 23979586,  1882,     744,  { .Interlace = 0, .TimeScale = 60000,    .NumUnitsInTick = 1001 }}, /* Mode 0  */
    {OV2710_INPUT_MODE_1, OV2710_OUTPUT_MODE_2, 11989793,  1882,     744,  { .Interlace = 0, .TimeScale = 30000,    .NumUnitsInTick = 1001 }}, /* Mode 0  */
    {OV2710_INPUT_MODE_1, OV2710_OUTPUT_MODE_3, 23979586,  1882,    1488,  { .Interlace = 0, .TimeScale = 30000,    .NumUnitsInTick = 1001 }}, /* Mode 0  */

};

