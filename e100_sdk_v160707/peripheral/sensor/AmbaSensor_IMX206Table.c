/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaSensor_IMX206Table.c
 *
 *  @Copyright      :: Copyright (C) 2013 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Control APIs of SONY IMX206 CMOS sensor with LVDS interface
\*-------------------------------------------------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "AmbaDataType.h"
#include "AmbaDSP.h"
#include "AmbaSensor.h"
#include "AmbaSensor_IMX206.h"

/*-----------------------------------------------------------------------------------------------*\
 * Sensor Device Information
\*-----------------------------------------------------------------------------------------------*/
const AMBA_SENSOR_DEVICE_INFO_s IMX206DeviceInfo = {
    .UnitCellWidth          = 1.34,
    .UnitCellHeight         = 1.34,
    .NumTotalPixelCols      = 4768,
    .NumTotalPixelRows      = 3516,
    .NumEffectivePixelCols  = 4672,
    .NumEffectivePixelRows  = 3500,
    .MinAnalogGainFactor    = 1.0,
    .MaxAnalogGainFactor    = 16.0, /* 24 dB */
    .MinDigitalGainFactor   = 1.0,
    .MaxDigitalGainFactor   = 8.0,  /* 18 dB */

    .FrameRateCtrlInfo = {
        .CommunicationTime      = AMBA_SENSOR_COMMUNICATION_AT_NON_VBLANK,
        .FirstReflectedFrame    = 2,
        .FirstBadFrame          = 2,
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
        .CommunicationTime      = AMBA_SENSOR_COMMUNICATION_AT_NON_VBLANK,
        .FirstReflectedFrame    = 1,
        .FirstBadFrame          = 0,
        .NumBadFrames           = 0
    },
};

/*-----------------------------------------------------------------------------------------------*\
 * Sensor register settings of each readout modes
\*-----------------------------------------------------------------------------------------------*/
IMX206_REG_s IMX206RegTable[IMX206_NUM_READOUT_MODE_REG] = {
    /*     [Type1/2.3 12.4M]*/
    /* Addr       2,    2A,    0,     4,    3B,     3,     5 */
    {0x0003, {0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x55}},
    {0x0004, {0x10, 0x10, 0x00, 0x18, 0x14, 0x14, 0x18}},
    {0x0005, {0x11, 0x11, 0x03, 0x19, 0x18, 0x19, 0x19}},
    {0x0006, {0x20, 0x20, 0x30, 0x20, 0x20, 0x20, 0xa3}},
    {0x0007, {0x09, 0x09, 0x00, 0x05, 0x05, 0x05, 0x05}},
    {0x000d, {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07}},
    {0x000e, {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}},
    {0x006f, {0x6c, 0x00, 0x00, 0x00, 0x48, 0x00, 0x00}},
    {0x0070, {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}},
    {0x0071, {0xd8, 0x00, 0x00, 0x00, 0x90, 0x00, 0x00}},
    {0x0072, {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}},

    {0x002c, {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}},
    {0x010e, {0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11}},
    {0x0308, {0x13, 0x13, 0x13, 0x13, 0x13, 0x13, 0x13}},
    {0x0309, {0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10}},
    {0x030a, {0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11}},
    {0x0320, {0x14, 0x14, 0x14, 0x14, 0x14, 0x14, 0x14}},
    {0x0321, {0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01}},
    {0x0322, {0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18}},
    {0x0323, {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}},
    {0x0338, {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}},
    {0x0342, {0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06}},
    {0x0343, {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}},
    {0x0344, {0x13, 0x13, 0x13, 0x13, 0x13, 0x13, 0x13}},
    {0x0345, {0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01}},
    {0x034a, {0x12, 0x12, 0x12, 0x12, 0x12, 0x12, 0x12}},
    {0x034b, {0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01}},
    {0x034c, {0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07}},
    {0x034d, {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}},
    {0x0352, {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}},
    {0x0353, {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}},
    {0x0354, {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}},
    {0x0359, {0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11}},
    {0x035f, {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff}},
    {0x0360, {0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01}},
    {0x0361, {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}},
    {0x0362, {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}},
    {0x0406, {0x7e, 0x7e, 0x7e, 0x7e, 0x7e, 0x7e, 0x7e}},
    {0x0407, {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}},
    {0x048b, {0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01}},
    {0x0506, {0x1b, 0x1b, 0x1b, 0x1b, 0x1b, 0x1b, 0x1b}},
    {0x0507, {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}},
    {0x050e, {0xb7, 0xb7, 0xb7, 0xb7, 0xb7, 0xb7, 0xb7}},
    {0x050f, {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}},
    {0x053e, {0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c}},
    {0x053f, {0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08}},
    {0x0540, {0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03}},
    {0x0542, {0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01}},
    {0x055b, {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}},
    {0x0578, {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff}},
    {0x0579, {0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01}},
    {0x057a, {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}},
    {0x057b, {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}},
};

const AMBA_SENSOR_OUTPUT_INFO_s IMX206OutputInfo[IMX206_NUM_READOUT_MODE] = {
/*IMX206_TYPE_2_3_MODE_2  */ {576000000, 4, 10, AMBA_DSP_BAYER_RG, {AMBA_DSP_PHASE_SHIFT_MODE_0, AMBA_DSP_PHASE_SHIFT_MODE_0}, 2384, 1316, { 62,  12, 2304, 1296}, { 56, 0, 2316,  4}},
/*IMX206_TYPE_2_3_MODE_2A */ {576000000, 4, 10, AMBA_DSP_BAYER_RG, {AMBA_DSP_PHASE_SHIFT_MODE_0, AMBA_DSP_PHASE_SHIFT_MODE_0}, 2384, 1748, { 62,  12, 2304, 1728}, { 56, 0, 2316,  4}},
/*IMX206_TYPE_2_3_MODE_0 */  {576000000, 4, 12, AMBA_DSP_BAYER_RG, {AMBA_DSP_PHASE_SHIFT_MODE_0, AMBA_DSP_PHASE_SHIFT_MODE_0}, 4768, 3516, {124,  38, 4608, 3456}, {112, 0, 4632, 16}},
/*IMX206_TYPE_2_3_MODE_4 */  {576000000, 4, 10, AMBA_DSP_BAYER_RG, {AMBA_DSP_PHASE_SHIFT_MODE_0, AMBA_DSP_PHASE_SHIFT_MODE_0}, 1590,  396, { 42,   8, 1536,  384}, { 38, 0, 1544,  4}},
/*IMX206_TYPE_2_3_MODE_3B*/  {576000000, 4, 10, AMBA_DSP_BAYER_RG, {AMBA_DSP_PHASE_SHIFT_MODE_0, AMBA_DSP_PHASE_SHIFT_MODE_0}, 1590,  884, { 42,  12, 1536,  864}, { 38, 0, 1544,  4}},
/*IMX206_TYPE_2_3_MODE_3*/   {576000000, 4, 10, AMBA_DSP_BAYER_RG, {AMBA_DSP_PHASE_SHIFT_MODE_0, AMBA_DSP_PHASE_SHIFT_MODE_0}, 1590, 1172, { 42,  12, 1536, 1152}, { 38, 0, 1544,  4}},
/*IMX206_TYPE_2_3_MODE_5 */  {576000000, 1, 10, AMBA_DSP_BAYER_RG, {AMBA_DSP_PHASE_SHIFT_MODE_0, AMBA_DSP_PHASE_SHIFT_MODE_0}, 1590,  396, { 42,   8, 1536,  384}, { 38, 0, 1544,  4}},
};

const AMBA_SENSOR_INPUT_INFO_s IMX206InputInfoNormalReadout[IMX206_NUM_READOUT_MODE] = {
/*IMX206_TYPE_2_3_MODE_2  */ {{ 124, 502, 4608, 2592}, { AMBA_SENSOR_SUBSAMPLE_TYPE_BINNING, 1, 2}, { AMBA_SENSOR_SUBSAMPLE_TYPE_BINNING, 1,  2}, 1},
/*IMX206_TYPE_2_3_MODE_2A */ {{ 124,  70, 4608, 3456}, { AMBA_SENSOR_SUBSAMPLE_TYPE_BINNING, 1, 2}, { AMBA_SENSOR_SUBSAMPLE_TYPE_BINNING, 1,  2}, 1},
/*IMX206_TYPE_2_3_MODE_0  */ {{ 124,  70, 4608, 3456}, { AMBA_SENSOR_SUBSAMPLE_TYPE_NORMAL,  1, 1}, { AMBA_SENSOR_SUBSAMPLE_TYPE_NORMAL,  1,  1}, 1},
/*IMX206_TYPE_2_3_MODE_4  */ {{ 124,  72, 4608, 3456}, { AMBA_SENSOR_SUBSAMPLE_TYPE_BINNING, 1, 3}, { AMBA_SENSOR_SUBSAMPLE_TYPE_BINNING, 1,  9}, 1},
/*IMX206_TYPE_2_3_MODE_3B */ {{ 124, 502, 4608, 2592}, { AMBA_SENSOR_SUBSAMPLE_TYPE_BINNING, 1, 3}, { AMBA_SENSOR_SUBSAMPLE_TYPE_BINNING, 1,  3}, 1},
/*IMX206_TYPE_2_3_MODE_3  */ {{ 124,  70, 4608, 3456}, { AMBA_SENSOR_SUBSAMPLE_TYPE_BINNING, 1, 3}, { AMBA_SENSOR_SUBSAMPLE_TYPE_BINNING, 1,  3}, 1},
/*IMX206_TYPE_2_3_MODE_5  */ {{ 124,  70, 4608, 3456}, { AMBA_SENSOR_SUBSAMPLE_TYPE_BINNING, 1, 3}, { AMBA_SENSOR_SUBSAMPLE_TYPE_BINNING, 1,  9}, 1},
};

const IMX206_MODE_INFO_s IMX206ModeInfoList[AMBA_SENSOR_IMX206_NUM_MODE] = {
/*AMBA_SENSOR_IMX206_TYPE_2_3_MODE_2_60P  */ { IMX206_TYPE_2_3_MODE_2, { 72000000,  880, 1, 1365, 1, { .Interlace = 0, .TimeScale = 60000,  .NumUnitsInTick = 1001}}},
/*AMBA_SENSOR_IMX206_TYPE_2_3_MODE_2A_30P */ { IMX206_TYPE_2_3_MODE_2A,{ 72000000, 1320, 1, 1820, 1, { .Interlace = 0, .TimeScale = 30000,  .NumUnitsInTick = 1001}}},
/*AMBA_SENSOR_IMX206_TYPE_2_3_MODE_0_10P  */ { IMX206_TYPE_2_3_MODE_0 ,{ 72000000, 2016, 1, 3575, 1, { .Interlace = 0, .TimeScale = 10000,  .NumUnitsInTick = 1001}}},
/*AMBA_SENSOR_IMX206_TYPE_2_3_MODE_4_240P */ { IMX206_TYPE_2_3_MODE_4 ,{ 72000000,  715, 1,  420, 1, { .Interlace = 0, .TimeScale =240000,  .NumUnitsInTick = 1001}}},
/*AMBA_SENSOR_IMX206_TYPE_2_3_MODE_2_50P  */ { IMX206_TYPE_2_3_MODE_2, { 72000000,  900, 1, 1600, 1, { .Interlace = 0, .TimeScale =    50,  .NumUnitsInTick =    1}}},
/*AMBA_SENSOR_IMX206_TYPE_2_3_MODE_2A_25P */ { IMX206_TYPE_2_3_MODE_2A,{ 72000000, 1440, 1, 2000, 1, { .Interlace = 0, .TimeScale =    25,  .NumUnitsInTick =    1}}},
/*AMBA_SENSOR_IMX206_TYPE_2_3_MODE_4_200P */ { IMX206_TYPE_2_3_MODE_4 ,{ 72000000,  750, 1,  480, 1, { .Interlace = 0, .TimeScale =   200,  .NumUnitsInTick =    1}}},
/*AMBA_SENSOR_IMX206_TYPE_2_3_MODE_3B_120P */{ IMX206_TYPE_2_3_MODE_3B,{ 72000000,  650, 1,  924, 1, { .Interlace = 0, .TimeScale =120000,  .NumUnitsInTick = 1001}}},
/*AMBA_SENSOR_IMX206_TYPE_2_3_MODE_3B_100P */{ IMX206_TYPE_2_3_MODE_3B,{ 72000000,  720, 1, 1000, 1, { .Interlace = 0, .TimeScale =   100,  .NumUnitsInTick =    1}}},
/*AMBA_SENSOR_IMX206_TYPE_2_3_MODE_3_60P  */ { IMX206_TYPE_2_3_MODE_3, { 72000000,  880, 1, 1365, 1, { .Interlace = 0, .TimeScale = 60000,  .NumUnitsInTick = 1001}}},
/*AMBA_SENSOR_IMX206_TYPE_2_3_MODE_3_50P  */ { IMX206_TYPE_2_3_MODE_3, { 72000000,  900, 1, 1600, 1, { .Interlace = 0, .TimeScale =    50,  .NumUnitsInTick =    1}}},
/*AMBA_SENSOR_IMX206_TYPE_2_3_MODE_4_120P */ { IMX206_TYPE_2_3_MODE_4 ,{ 72000000,  715, 1,  840, 1, { .Interlace = 0, .TimeScale =120000,  .NumUnitsInTick = 1001}}},
/*AMBA_SENSOR_IMX206_TYPE_2_3_MODE_4_100P */ { IMX206_TYPE_2_3_MODE_4 ,{ 72000000,  750, 1,  960, 1, { .Interlace = 0, .TimeScale =   100,  .NumUnitsInTick =    1}}},
/*AMBA_SENSOR_IMX206_TYPE_2_3_MODE_5_30P  */ { IMX206_TYPE_2_3_MODE_5 ,{ 72000000,  715, 4, 3360, 8, { .Interlace = 0, .TimeScale = 30000,  .NumUnitsInTick = 1001}}},
/*AMBA_SENSOR_IMX206_TYPE_2_3_MODE_5_25P  */ { IMX206_TYPE_2_3_MODE_5 ,{ 72000000,  800, 4, 3600, 8, { .Interlace = 0, .TimeScale =    25,  .NumUnitsInTick =    1}}},
};
