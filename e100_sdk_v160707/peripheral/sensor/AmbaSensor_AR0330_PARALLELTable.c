/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaSensor_AR0330_PARALLELTable.c
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

#include <AmbaSSP.h>

#include "AmbaDSP.h"
#include "AmbaSensor.h"
#include "AmbaSensor_AR0330_PARALLEL.h"

/*-----------------------------------------------------------------------------------------------*\
 * Sensor Device Information
\*-----------------------------------------------------------------------------------------------*/
const AMBA_SENSOR_DEVICE_INFO_s AR0330_PARALLELDeviceInfo = {
    .UnitCellWidth          = 2.2,
    .UnitCellHeight         = 2.2,
    .NumTotalPixelCols      = 2304,
    .NumTotalPixelRows      = 1536,
    .NumEffectivePixelCols  = 2304,
    .NumEffectivePixelRows  = 1536,
    .MinAnalogGainFactor    = 1.0,
    .MaxAnalogGainFactor    = 8.0,
    .MinDigitalGainFactor   = 1.0,
    .MaxDigitalGainFactor   = 15.9921875,

    .FrameRateCtrlInfo = {
        .CommunicationTime      = AMBA_SENSOR_COMMUNICATION_AT_NON_VBLANK,
        .FirstReflectedFrame    = 3,
        .FirstBadFrame          = 0,
        .NumBadFrames           = 0
    },
    .ShutterSpeedCtrlInfo = {
        .CommunicationTime      = AMBA_SENSOR_COMMUNICATION_AT_NON_VBLANK,
        .FirstReflectedFrame    = 2,
        .FirstBadFrame          = 0,
        .NumBadFrames           = 0
    },
    .AnalogGainCtrlInfo = {
        .CommunicationTime      = AMBA_SENSOR_COMMUNICATION_AT_NON_VBLANK,
        .FirstReflectedFrame    = 2,
        .FirstBadFrame          = 0,
        .NumBadFrames           = 0
    },
    .DigitalGainCtrlInfo = {
        .CommunicationTime      = AMBA_SENSOR_COMMUNICATION_AT_NON_VBLANK,
        .FirstReflectedFrame    = 2,
        .FirstBadFrame          = 0,
        .NumBadFrames           = 0
    },
};

/*-----------------------------------------------------------------------------------------------*\
 * Sensor register settings of each readout modes
\*-----------------------------------------------------------------------------------------------*/
AR0330_PARALLEL_REG_s AR0330_PARALLELRegTable[AR0330_PARALLEL_NUM_READOUT_MODE_REG] = {
    //    0       1       2       3       4       5       6       7       8       9      10      11
    //  PRI: PLL_03, PLL_04, PLL_01, PLL_04, PLL_04, PLL_04, PLL_03, PLL_04, PLL_01, PLL_04, PLL_04
    {0x3004,    198,      6,      6,    390,      6,      6,    198,      6,      6,    390,      6}, // 00, X_ADDR_START
    {0x3008,   2117,   2309,   2309,   1925,   2309,   2309,   2117,   2309,   2309,   1925,   2309}, // 01, X_ADDR_END
    {0x3002,    230,    126,      6,      6,    126,      6,    230,    126,      6,      6,    126}, // 02, Y_ADDR_START
    {0x3006,   1317,   1421,   1541,   1541,   1421,   1541,   1317,   1421,   1541,   1541,   1421}, // 03, Y_ADDR_END
    {0x30A2,      1,      1,      3,      1,      3,      1,      1,      1,      3,      1,      3}, // 04, X_ODD_INCREMENT
    {0x30A6,      1,      1,      3,      1,      3,      1,      1,      1,      3,      1,      3}, // 05, Y_ODD_INCREMENT
    {0x3040, 0x0000, 0x0000, 0x2000, 0x0000, 0x2000, 0x0000, 0x0000, 0x0000, 0x2000, 0x0000, 0x2000}, // 06, READ_MODE
    {0x300C,   1122,   1248,   1242,   1056,   1236,   1248,   1122,   1248,   1242,   1056,   1236}, // 07, LINE_LENGTH_PCK
    {0x300A,   1100,   1310,   1289,   1548,    660,   1634,   1318,   1570,   1545,   1856,    792}, // 08, FRAME_LENGTH_LINE
    {0x3014,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0}, // 09, FINE_INTEGRATION_TIME
    {0x3012,   1095,   1305,   1284,   1543,    655,   1629,   1313,   1565,   1540,   1851,    787}, // 10, Coarse_Integration_Time
    {0x3042,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0}, // 11, EXTRA_DELAY

    {0x30BA, 0x002C, 0x002C, 0x002C, 0x006C, 0x002C, 0x002C, 0x002C, 0x002C, 0x002C, 0x006C, 0x002C}, // 12, Digital_Ctrl_Adc_High_Speed (bit[6])

    {0x3088,     NA,     NA, 0x80BA, 0x80BA, 0x80BA,     NA,     NA,     NA, 0x80BA, 0x80BA, 0x80BA}, // 13
    {0x3086,     NA,     NA, 0x0253, 0x0253, 0x0253,     NA,     NA,     NA, 0x0253, 0x0253, 0x0253}, // 14

    {0x3ED2,     NA,     NA,     NA,     NA,     NA,     NA,     NA,     NA,     NA,     NA,     NA}, // 15, Reserved0
    {0x3ED4, 0x8F6C, 0x8F6C, 0x8F3C, 0x8F6C, 0x8F3C, 0x8F6C, 0x8F6C, 0x8F6C, 0x8F3C, 0x8F6C, 0x8F3C}, // 16, Reserved1
    {0x3ED6, 0x66CC, 0x66CC, 0x33CC, 0x66CC, 0x33CC, 0x66CC, 0x66CC, 0x66CC, 0x33CC, 0x66CC, 0x33CC}, // 17, Reserved2
    {0x30E8,     NA,     NA,     NA,     NA,     NA,     NA,     NA,     NA,     NA,     NA,     NA}, // 18, Reserved3
};

AR0330_PARALLEL_PLL_REG_s AR0330_PARALLELPllRegTable[AR0330_PARALLEL_NUM_PLL_REG] = {
    // Addr,    PLL_1,  PLL_2,  PLL_3,  PLL_4
    {0x31AE, { 0x0301, 0x0301, 0x0301, 0x0301}},
    {0x302A, {      8,      8,      6,      6}},
    {0x302C, {      1,      1,      1,      1}},
    {0x302E, {      1,      2,      2,      4}},
    {0x3030, {     32,     32,     37,     98}},
    {0x3036, {      8,      8,      8,      8}},
    {0x3038, {      1,      1,      1,      1}},
    {0x31AC, { 0x0C0C, 0x0C0C, 0x0C0C, 0x0C0C}},
};

AR0330_PARALLEL_SEQ_REG_s AR0330_PARALLELSeqARegTable[AR0330_PARALLEL_NUM_SEQUENCER_A_REG] = {
    {0x3088, {0x8000}},
    {0x3086, {0x4540}},
    {0x3086, {0x6134}},
    {0x3086, {0x4A31}},
    {0x3086, {0x4342}},
    {0x3086, {0x4560}},
    {0x3086, {0x2714}},
    {0x3086, {0x3DFF}},
    {0x3086, {0x3DFF}},
    {0x3086, {0x3DEA}},
    {0x3086, {0x2704}},
    {0x3086, {0x3D10}},
    {0x3086, {0x2705}},
    {0x3086, {0x3D10}},
    {0x3086, {0x2715}},
    {0x3086, {0x3527}},
    {0x3086, {0x053D}},
    {0x3086, {0x1045}},
    {0x3086, {0x4027}},
    {0x3086, {0x0427}},
    {0x3086, {0x143D}},
    {0x3086, {0xFF3D}},
    {0x3086, {0xFF3D}},
    {0x3086, {0xEA62}},
    {0x3086, {0x2728}},
    {0x3086, {0x3627}},
    {0x3086, {0x083D}},
    {0x3086, {0x6444}},
    {0x3086, {0x2C2C}},
    {0x3086, {0x2C2C}},
    {0x3086, {0x4B01}},
    {0x3086, {0x432D}},
    {0x3086, {0x4643}},
    {0x3086, {0x1647}},
    {0x3086, {0x435F}},
    {0x3086, {0x4F50}},
    {0x3086, {0x2604}},
    {0x3086, {0x2684}},
    {0x3086, {0x2027}},
    {0x3086, {0xFC53}},
    {0x3086, {0x0D5C}},
    {0x3086, {0x0D60}},
    {0x3086, {0x5754}},
    {0x3086, {0x1709}},
    {0x3086, {0x5556}},
    {0x3086, {0x4917}},
    {0x3086, {0x145C}},
    {0x3086, {0x0945}},
    {0x3086, {0x0045}},
    {0x3086, {0x8026}},
    {0x3086, {0xA627}},
    {0x3086, {0xF817}},
    {0x3086, {0x0227}},
    {0x3086, {0xFA5C}},
    {0x3086, {0x0B5F}},
    {0x3086, {0x5307}},
    {0x3086, {0x5302}},
    {0x3086, {0x4D28}},
    {0x3086, {0x6C4C}},
    {0x3086, {0x0928}},
    {0x3086, {0x2C28}},
    {0x3086, {0x294E}},
    {0x3086, {0x1718}},
    {0x3086, {0x26A2}},
    {0x3086, {0x5C03}},
    {0x3086, {0x1744}},
    {0x3086, {0x2809}},
    {0x3086, {0x27F2}},
    {0x3086, {0x1714}},
    {0x3086, {0x2808}},
    {0x3086, {0x164D}},
    {0x3086, {0x1A26}},
    {0x3086, {0x8317}},
    {0x3086, {0x0145}},
    {0x3086, {0xA017}},
    {0x3086, {0x0727}},
    {0x3086, {0xF317}},
    {0x3086, {0x2945}},
    {0x3086, {0x8017}},
    {0x3086, {0x0827}},
    {0x3086, {0xF217}},
    {0x3086, {0x285D}},
    {0x3086, {0x27FA}},
    {0x3086, {0x170E}},
    {0x3086, {0x2681}},
    {0x3086, {0x5300}},
    {0x3086, {0x17E6}},
    {0x3086, {0x5302}},
    {0x3086, {0x1710}},
    {0x3086, {0x2683}},
    {0x3086, {0x2682}},
    {0x3086, {0x4827}},
    {0x3086, {0xF24D}},
    {0x3086, {0x4E28}},
    {0x3086, {0x094C}},
    {0x3086, {0x0B17}},
    {0x3086, {0x6D28}},
    {0x3086, {0x0817}},
    {0x3086, {0x014D}},
    {0x3086, {0x1A17}},
    {0x3086, {0x0126}},
    {0x3086, {0x035C}},
    {0x3086, {0x0045}},
    {0x3086, {0x4027}},
    {0x3086, {0x9017}},
    {0x3086, {0x2A4A}},
    {0x3086, {0x0A43}},
    {0x3086, {0x160B}},
    {0x3086, {0x4327}},
    {0x3086, {0x9445}},
    {0x3086, {0x6017}},
    {0x3086, {0x0727}},
    {0x3086, {0x9517}},
    {0x3086, {0x2545}},
    {0x3086, {0x4017}},
    {0x3086, {0x0827}},
    {0x3086, {0x905D}},
    {0x3086, {0x2808}},
    {0x3086, {0x530D}},
    {0x3086, {0x2645}},
    {0x3086, {0x5C01}},
    {0x3086, {0x2798}},
    {0x3086, {0x4B12}},
    {0x3086, {0x4452}},
    {0x3086, {0x5117}},
    {0x3086, {0x0260}},
    {0x3086, {0x184A}},
    {0x3086, {0x0343}},
    {0x3086, {0x1604}},
    {0x3086, {0x4316}},
    {0x3086, {0x5843}},
    {0x3086, {0x1659}},
    {0x3086, {0x4316}},
    {0x3086, {0x5A43}},
    {0x3086, {0x165B}},
    {0x3086, {0x4327}},
    {0x3086, {0x9C45}},
    {0x3086, {0x6017}},
    {0x3086, {0x0727}},
    {0x3086, {0x9D17}},
    {0x3086, {0x2545}},
    {0x3086, {0x4017}},
    {0x3086, {0x1027}},
    {0x3086, {0x9817}},
    {0x3086, {0x2022}},
    {0x3086, {0x4B12}},
    {0x3086, {0x442C}},
    {0x3086, {0x2C2C}},
    {0x3086, {0x2C00}},
};

AR0330_PARALLEL_SEQ_REG_s AR0330_PARALLELSeqBRegTable[AR0330_PARALLEL_NUM_SEQUENCER_B_REG] = {
    {0x3088, {0x8000}},
    {0x3086, {0x4A03}},
    {0x3086, {0x4316}},
    {0x3086, {0x0443}},
    {0x3086, {0x1645}},
    {0x3086, {0x4045}},
    {0x3086, {0x6017}},
    {0x3086, {0x2045}},
    {0x3086, {0x404B}},
    {0x3086, {0x1244}},
    {0x3086, {0x6134}},
    {0x3086, {0x4A31}},
    {0x3086, {0x4342}},
    {0x3086, {0x4560}},
    {0x3086, {0x2714}},
    {0x3086, {0x3DFF}},
    {0x3086, {0x3DFF}},
    {0x3086, {0x3DEA}},
    {0x3086, {0x2704}},
    {0x3086, {0x3D10}},
    {0x3086, {0x2705}},
    {0x3086, {0x3D10}},
    {0x3086, {0x2715}},
    {0x3086, {0x3527}},
    {0x3086, {0x053D}},
    {0x3086, {0x1045}},
    {0x3086, {0x4027}},
    {0x3086, {0x0427}},
    {0x3086, {0x143D}},
    {0x3086, {0xFF3D}},
    {0x3086, {0xFF3D}},
    {0x3086, {0xEA62}},
    {0x3086, {0x2728}},
    {0x3086, {0x3627}},
    {0x3086, {0x083D}},
    {0x3086, {0x6444}},
    {0x3086, {0x2C2C}},
    {0x3086, {0x2C2C}},
    {0x3086, {0x4B01}},
    {0x3086, {0x432D}},
    {0x3086, {0x4643}},
    {0x3086, {0x1647}},
    {0x3086, {0x435F}},
    {0x3086, {0x4F50}},
    {0x3086, {0x2604}},
    {0x3086, {0x2684}},
    {0x3086, {0x2027}},
    {0x3086, {0xFC53}},
    {0x3086, {0x0D5C}},
    {0x3086, {0x0D57}},
    {0x3086, {0x5417}},
    {0x3086, {0x0955}},
    {0x3086, {0x5649}},
    {0x3086, {0x5307}},
    {0x3086, {0x5302}},
    {0x3086, {0x4D28}},
    {0x3086, {0x6C4C}},
    {0x3086, {0x0928}},
    {0x3086, {0x2C28}},
    {0x3086, {0x294E}},
    {0x3086, {0x5C09}},
    {0x3086, {0x6045}},
    {0x3086, {0x0045}},
    {0x3086, {0x8026}},
    {0x3086, {0xA627}},
    {0x3086, {0xF817}},
    {0x3086, {0x0227}},
    {0x3086, {0xFA5C}},
    {0x3086, {0x0B17}},
    {0x3086, {0x1826}},
    {0x3086, {0xA25C}},
    {0x3086, {0x0317}},
    {0x3086, {0x4427}},
    {0x3086, {0xF25F}},
    {0x3086, {0x2809}},
    {0x3086, {0x1714}},
    {0x3086, {0x2808}},
    {0x3086, {0x1701}},
    {0x3086, {0x4D1A}},
    {0x3086, {0x2683}},
    {0x3086, {0x1701}},
    {0x3086, {0x27FA}},
    {0x3086, {0x45A0}},
    {0x3086, {0x1707}},
    {0x3086, {0x27FB}},
    {0x3086, {0x1729}},
    {0x3086, {0x4580}},
    {0x3086, {0x1708}},
    {0x3086, {0x27FA}},
    {0x3086, {0x1728}},
    {0x3086, {0x5D17}},
    {0x3086, {0x0E26}},
    {0x3086, {0x8153}},
    {0x3086, {0x0117}},
    {0x3086, {0xE653}},
    {0x3086, {0x0217}},
    {0x3086, {0x1026}},
    {0x3086, {0x8326}},
    {0x3086, {0x8248}},
    {0x3086, {0x4D4E}},
    {0x3086, {0x2809}},
    {0x3086, {0x4C0B}},
    {0x3086, {0x6017}},
    {0x3086, {0x2027}},
    {0x3086, {0xF217}},
    {0x3086, {0x535F}},
    {0x3086, {0x2808}},
    {0x3086, {0x164D}},
    {0x3086, {0x1A17}},
    {0x3086, {0x0127}},
    {0x3086, {0xFA26}},
    {0x3086, {0x035C}},
    {0x3086, {0x0145}},
    {0x3086, {0x4027}},
    {0x3086, {0x9817}},
    {0x3086, {0x2A4A}},
    {0x3086, {0x0A43}},
    {0x3086, {0x160B}},
    {0x3086, {0x4327}},
    {0x3086, {0x9C45}},
    {0x3086, {0x6017}},
    {0x3086, {0x0727}},
    {0x3086, {0x9D17}},
    {0x3086, {0x2545}},
    {0x3086, {0x4017}},
    {0x3086, {0x0827}},
    {0x3086, {0x985D}},
    {0x3086, {0x2645}},
    {0x3086, {0x5C01}},
    {0x3086, {0x4B17}},
    {0x3086, {0x0A28}},
    {0x3086, {0x0853}},
    {0x3086, {0x0D52}},
    {0x3086, {0x5112}},
    {0x3086, {0x4460}},
    {0x3086, {0x184A}},
    {0x3086, {0x0343}},
    {0x3086, {0x1604}},
    {0x3086, {0x4316}},
    {0x3086, {0x5843}},
    {0x3086, {0x1659}},
    {0x3086, {0x4316}},
    {0x3086, {0x5A43}},
    {0x3086, {0x165B}},
    {0x3086, {0x4345}},
    {0x3086, {0x4027}},
    {0x3086, {0x9C45}},
    {0x3086, {0x6017}},
    {0x3086, {0x0727}},
    {0x3086, {0x9D17}},
    {0x3086, {0x2545}},
    {0x3086, {0x4017}},
    {0x3086, {0x1027}},
    {0x3086, {0x9817}},
    {0x3086, {0x2022}},
    {0x3086, {0x4B12}},
    {0x3086, {0x442C}},
    {0x3086, {0x2C2C}},
    {0x3086, {0x2C00}},
    {0x3086, {0x0000}},
};

const AMBA_SENSOR_INPUT_INFO_s AR0330_PARALLELInputInfo[] = {
    {{ 192, 228, 1920, 1080}, { AMBA_SENSOR_SUBSAMPLE_TYPE_NORMAL,  1, 1}, { AMBA_SENSOR_SUBSAMPLE_TYPE_NORMAL,  1, 1}, 1},  /* Input Mode 0 */
    {{   0, 120, 2304, 1296}, { AMBA_SENSOR_SUBSAMPLE_TYPE_NORMAL,  1, 1}, { AMBA_SENSOR_SUBSAMPLE_TYPE_NORMAL,  1, 1}, 1},  /* Input Mode 1 */
    {{   0, 120, 2304, 1296}, { AMBA_SENSOR_SUBSAMPLE_TYPE_BINNING, 1, 2}, { AMBA_SENSOR_SUBSAMPLE_TYPE_BINNING, 1, 2}, 1},  /* Input Mode 2 */
    {{ 384,   0, 1536, 1536}, { AMBA_SENSOR_SUBSAMPLE_TYPE_NORMAL,  1, 1}, { AMBA_SENSOR_SUBSAMPLE_TYPE_NORMAL,  1, 1}, 1},  /* Input Mode 3 */
    {{   0,   0, 2304, 1536}, { AMBA_SENSOR_SUBSAMPLE_TYPE_BINNING, 1, 2}, { AMBA_SENSOR_SUBSAMPLE_TYPE_BINNING, 1, 2}, 1},  /* Input Mode 4 */
    {{   0,   0, 2304, 1536}, { AMBA_SENSOR_SUBSAMPLE_TYPE_NORMAL,  1, 1}, { AMBA_SENSOR_SUBSAMPLE_TYPE_NORMAL,  1, 1}, 1},  /* Input Mode 5 */
};

const AMBA_SENSOR_OUTPUT_INFO_s AR0330_PARALLELOutputInfo[] = {
    {73978022, 12, 12, AMBA_DSP_BAYER_GR, {AMBA_DSP_PHASE_SHIFT_MODE_0, AMBA_DSP_PHASE_SHIFT_MODE_0}, 1920, 1088, { 0, 0, 1920, 1088}, {0}},  /* Output Mode 0 */
    {97994806, 12, 12, AMBA_DSP_BAYER_GR, {AMBA_DSP_PHASE_SHIFT_MODE_0, AMBA_DSP_PHASE_SHIFT_MODE_0}, 2304, 1296, { 0, 0, 2304, 1296}, {0}},  /* Output Mode 1 */
    {97793407, 12, 12, AMBA_DSP_BAYER_GR, {AMBA_DSP_PHASE_SHIFT_MODE_0, AMBA_DSP_PHASE_SHIFT_MODE_0}, 1152,  648, { 0, 0, 1152,  648}, {0}},  /* Output Mode 2 */
    {97983297, 12, 12, AMBA_DSP_BAYER_GR, {AMBA_DSP_PHASE_SHIFT_MODE_0, AMBA_DSP_PHASE_SHIFT_MODE_0}, 1536, 1536, { 0, 0, 1536, 1536}, {0}},  /* Output Mode 3 */
    {95960320, 12, 12, AMBA_DSP_BAYER_GR, {AMBA_DSP_PHASE_SHIFT_MODE_0, AMBA_DSP_PHASE_SHIFT_MODE_0}, 1152,  768, { 0, 0, 1152,  768}, {0}},  /* Output Mode 4 */
    {97883136, 12, 12, AMBA_DSP_BAYER_GR, {AMBA_DSP_PHASE_SHIFT_MODE_0, AMBA_DSP_PHASE_SHIFT_MODE_0}, 2304, 1536, { 0, 0, 2304, 1536}, {0}},  /* Output Mode 5 */
};

const AR0330_PARALLEL_MODE_INFO_s AR0330_PARALLELModeInfoList[AMBA_SENSOR_AR0330_PARALLEL_NUM_MODE] = {
    {AR0330_PARALLEL_READOUT_MODE_0, AR0330_PARALLEL_PLL_3, AR0330_PARALLEL_INPUT_MODE_0, AR0330_PARALLEL_OUTPUT_MODE_0, 23992872,  1122 * 2, 1100, { .Interlace = 0, .TimeScale = 30000, .NumUnitsInTick = 1001 }},  /* Mode  0 */
    {AR0330_PARALLEL_READOUT_MODE_1, AR0330_PARALLEL_PLL_4, AR0330_PARALLEL_INPUT_MODE_1, AR0330_PARALLEL_OUTPUT_MODE_1, 23998728,  1248 * 2, 1310, { .Interlace = 0, .TimeScale = 30000, .NumUnitsInTick = 1001 }},  /* Mode  1 */
    {AR0330_PARALLEL_READOUT_MODE_4, AR0330_PARALLEL_PLL_4, AR0330_PARALLEL_INPUT_MODE_2, AR0330_PARALLEL_OUTPUT_MODE_2, 23949406,  1236 * 2,  660, { .Interlace = 0, .TimeScale = 60000, .NumUnitsInTick = 1001 }},  /* Mode  2 */
    {AR0330_PARALLEL_READOUT_MODE_3, AR0330_PARALLEL_PLL_4, AR0330_PARALLEL_INPUT_MODE_3, AR0330_PARALLEL_OUTPUT_MODE_3, 23995909,  1056 * 2, 1548, { .Interlace = 0, .TimeScale = 30000, .NumUnitsInTick = 1001 }},  /* Mode  3 */
    {AR0330_PARALLEL_READOUT_MODE_2, AR0330_PARALLEL_PLL_1, AR0330_PARALLEL_INPUT_MODE_4, AR0330_PARALLEL_OUTPUT_MODE_4, 23990080,  1242 * 2, 1289, { .Interlace = 0, .TimeScale = 30000, .NumUnitsInTick = 1001 }},  /* Mode  4 */
    {AR0330_PARALLEL_READOUT_MODE_5, AR0330_PARALLEL_PLL_4, AR0330_PARALLEL_INPUT_MODE_5, AR0330_PARALLEL_OUTPUT_MODE_5, 23971380,  1248 * 2, 1634, { .Interlace = 0, .TimeScale = 24000, .NumUnitsInTick = 1000 }},  /* Mode  5 */

    {AR0330_PARALLEL_READOUT_MODE_6, AR0330_PARALLEL_PLL_3, AR0330_PARALLEL_INPUT_MODE_0, AR0330_PARALLEL_OUTPUT_MODE_0, 23980476,  1122 * 2, 1318, { .Interlace = 0, .TimeScale = 25000, .NumUnitsInTick = 1000 }},  /* Mode  6 */
    {AR0330_PARALLEL_READOUT_MODE_7, AR0330_PARALLEL_PLL_4, AR0330_PARALLEL_INPUT_MODE_1, AR0330_PARALLEL_OUTPUT_MODE_1, 23992163,  1248 * 2, 1570, { .Interlace = 0, .TimeScale = 25000, .NumUnitsInTick = 1000 }},  /* Mode  7 */
    {AR0330_PARALLEL_READOUT_MODE_10,AR0330_PARALLEL_PLL_4, AR0330_PARALLEL_INPUT_MODE_2, AR0330_PARALLEL_OUTPUT_MODE_2, 23973355,  1236 * 2,  792, { .Interlace = 0, .TimeScale = 50000, .NumUnitsInTick = 1000 }},  /* Mode  8 */
    {AR0330_PARALLEL_READOUT_MODE_9, AR0330_PARALLEL_PLL_4, AR0330_PARALLEL_INPUT_MODE_3, AR0330_PARALLEL_OUTPUT_MODE_3, 23999217,  1056 * 2, 1856, { .Interlace = 0, .TimeScale = 25000, .NumUnitsInTick = 1000 }},  /* Mode  9 */
    {AR0330_PARALLEL_READOUT_MODE_8, AR0330_PARALLEL_PLL_1, AR0330_PARALLEL_INPUT_MODE_4, AR0330_PARALLEL_OUTPUT_MODE_4, 23986125,  1242 * 2, 1545, { .Interlace = 0, .TimeScale = 25000, .NumUnitsInTick = 1000 }},  /* Mode 10 */
};
