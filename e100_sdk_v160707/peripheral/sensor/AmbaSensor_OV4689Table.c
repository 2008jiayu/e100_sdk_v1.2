/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaSensor_OV4689Table.c
 *
 *  @Copyright      :: Copyright (C) 2013 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Control APIs of OmniVision OV4689 CMOS sensor with MIPI interface
\*-------------------------------------------------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "AmbaDataType.h"
#include "AmbaDSP.h"
#include "AmbaSensor.h"
#include "AmbaSensor_OV4689.h"

/*-----------------------------------------------------------------------------------------------*\
 * Sensor Device Information
\*-----------------------------------------------------------------------------------------------*/
const AMBA_SENSOR_DEVICE_INFO_s OV4689DeviceInfo = {
    .UnitCellWidth          = 2.0,
    .UnitCellHeight         = 2.0,
    .NumTotalPixelCols      = 2688,
    .NumTotalPixelRows      = 1520,
    .NumEffectivePixelCols  = 2688,
    .NumEffectivePixelRows  = 1520,
    .MinAnalogGainFactor    = 1.0,
    .MaxAnalogGainFactor    = 15.5,
    .MinDigitalGainFactor   = 1.0,
    .MaxDigitalGainFactor   = 15.99,

    .FrameRateCtrlInfo = {
        .CommunicationTime      = AMBA_SENSOR_COMMUNICATION_AT_NON_VBLANK,
        .FirstReflectedFrame    = 2,
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
        .FirstReflectedFrame    = 1,
        .FirstBadFrame          = 0,
        .NumBadFrames           = 0
    },
    .DigitalGainCtrlInfo = {
        .CommunicationTime      = AMBA_SENSOR_COMMUNICATION_AT_NON_VBLANK,
        .FirstReflectedFrame    = 1,
        .FirstBadFrame          = 0,
        .NumBadFrames           = 0
    },

    .HdrIsSupport = 1
};

/*-----------------------------------------------------------------------------------------------*\
 * Sensor register settings of each readout modes
\*-----------------------------------------------------------------------------------------------*/
const OV4689_REG_s OV4689RegTable[OV4689_NUM_READOUT_MODE_REG] = {
    {0x0300, {0x00, 0x00, 0x01, 0x00, 0x00, 0x02, 0x00}},
    {0x0302, {0x1e, 0x23, 0x2d, 0x23, 0x2a, 0x32, 0x1b}},
    {0x0303, {0x00, 0x01, 0x02, 0x00, 0x00, 0x00, 0x00}},
    {0x3018, {0x72, 0x72, 0x32, 0x72, 0x72, 0x72, 0x32}},
    {0x3019, {0x00, 0x00, 0x0c, 0x00, 0x00, 0x00, 0x0c}},
    {0x3501, {0x60, 0x60, 0x31, 0x60, 0x4c, 0x31, 0x60}},
    {0x3632, {0x00, 0x00, 0x05, 0x00, 0x00, 0x05, 0x00}},
    {0x374a, {0x43, 0x43, 0x43, 0x43, 0x43, 0x43, 0x43}},
    {0x376b, {0x20, 0x20, 0x40, 0x20, 0x20, 0x40, 0x20}},
    {0x3800, {0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00}},
    {0x3801, {0x08, 0x08, 0x08, 0x08, 0x24, 0x08, 0x08}},
    {0x3802, {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}},
    {0x3803, {0x04, 0x04, 0x04, 0x04, 0xa6, 0x04, 0x04}},
    {0x3804, {0x0a, 0x0a, 0x0a, 0x0a, 0x09, 0x0a, 0x0a}},
    {0x3805, {0x97, 0x97, 0x97, 0x97, 0x7b, 0x97, 0x97}},
    {0x3806, {0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05}},
    {0x3807, {0xfb, 0xfb, 0xfb, 0xfb, 0x59, 0xfb, 0xfb}},
    {0x3808, {0x0a, 0x0a, 0x05, 0x0a, 0x08, 0x05, 0x0a}},
    {0x3809, {0x80, 0x80, 0x40, 0x80, 0x48, 0x40, 0x80}},
    {0x380a, {0x05, 0x05, 0x02, 0x05, 0x04, 0x02, 0x05}},
    {0x380b, {0xf0, 0xf0, 0xf8, 0xf0, 0xac, 0xf8, 0xf0}},
    {0x380c, {0x04, 0x09, 0x13, 0x04, 0x03, 0x03, 0x0a}},
    {0x380d, {0xb0, 0x60, 0x98, 0x12, 0x30, 0x48, 0x0c}},
    {0x380e, {0x06, 0x06, 0x03, 0x07, 0x04, 0x04, 0x06}},
    {0x380f, {0x86, 0x84, 0x1d, 0x80, 0xce, 0xad, 0x14}},
    {0x3810, {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}},
    {0x3811, {0x08, 0x08, 0x04, 0x08, 0x08, 0x04, 0x08}},
    {0x3812, {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}},
    {0x3813, {0x04, 0x04, 0x02, 0x04, 0x04, 0x02, 0x04}},
    {0x3814, {0x01, 0x01, 0x03, 0x01, 0x01, 0x03, 0x01}},
    {0x3815, {0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01}},
    {0x3819, {0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01}},
    {0x3820, {0x00, 0x00, 0x10, 0x00, 0x00, 0x10, 0x00}},
    {0x3821, {0x06, 0x06, 0x07, 0x06, 0x06, 0x07, 0x06}},
    {0x3829, {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}},
    {0x382a, {0x01, 0x01, 0x03, 0x01, 0x01, 0x03, 0x01}},
    {0x3830, {0x04, 0x04, 0x08, 0x04, 0x04, 0x08, 0x04}},
    {0x3836, {0x01, 0x01, 0x02, 0x01, 0x01, 0x02, 0x01}},
    {0x4001, {0x40, 0x40, 0x50, 0x40, 0x40, 0x50, 0x40}},
    {0x4003, {0x14, 0x14, 0x14, 0x14, 0x14, 0x14, 0x14}},
    {0x4022, {0x07, 0x07, 0x03, 0x07, 0x06, 0x03, 0x07}},
    {0x4023, {0xcf, 0xcf, 0xe7, 0xcf, 0x3f, 0xe7, 0xcf}},
    {0x4024, {0x09, 0x09, 0x05, 0x09, 0x07, 0x05, 0x09}},
    {0x4025, {0x60, 0x60, 0x14, 0x60, 0x6c, 0x14, 0x60}},
    {0x4026, {0x09, 0x09, 0x05, 0x09, 0x07, 0x05, 0x09}},
    {0x4027, {0x6f, 0x6f, 0x23, 0x6f, 0x7b, 0x23, 0x6f}},
    {0x402a, {0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06}},
    {0x402b, {0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04}},
    {0x402e, {0x0e, 0x0e, 0x0e, 0x0e, 0x0e, 0x0e, 0x0e}},
    {0x402f, {0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04}},
    {0x4502, {0x40, 0x40, 0x44, 0x40, 0x40, 0x44, 0x40}},
    {0x4601, {0x04, 0x04, 0x53, 0x04, 0x83, 0x53, 0xa7}},
    {0x4837, {0x16, 0x26, 0x43, 0x10, 0x10, 0x1b, 0x18}},
    {0x5050, {0x0c, 0x0c, 0x3c, 0x0c, 0x0c, 0x3c, 0x0c}},
};

const AMBA_SENSOR_INPUT_INFO_s OV4689InputInfo[OV4689_NUM_READOUT_MODE_REG] = {
/* OV4689_S1_10_2688_1520_60P       */{{  16,   8, 2688, 1520}, { AMBA_SENSOR_SUBSAMPLE_TYPE_NORMAL, 1, 1}, { AMBA_SENSOR_SUBSAMPLE_TYPE_NORMAL, 1, 1}},
/* OV4689_S1_10_2688_1520_30P       */{{  16,   8, 2688, 1520}, { AMBA_SENSOR_SUBSAMPLE_TYPE_NORMAL, 1, 1}, { AMBA_SENSOR_SUBSAMPLE_TYPE_NORMAL, 1, 1}},
/* OV4689_S2_10_1344_760_30P        */{{  16,   8, 1344,  760}, {AMBA_SENSOR_SUBSAMPLE_TYPE_BINNING, 1, 2}, {AMBA_SENSOR_SUBSAMPLE_TYPE_BINNING, 1, 2}},
/* OV4689_S1_10_2688_1520_HDR_30P   */{{  16,   8, 2688, 1520}, { AMBA_SENSOR_SUBSAMPLE_TYPE_NORMAL, 1, 1}, { AMBA_SENSOR_SUBSAMPLE_TYPE_NORMAL, 1, 1}},
/* OV4689_S1_10_2120_1196_CROP_120P */{{ 300, 170, 2120, 1196}, { AMBA_SENSOR_SUBSAMPLE_TYPE_NORMAL, 1, 1}, { AMBA_SENSOR_SUBSAMPLE_TYPE_NORMAL, 1, 1}},
/* OV4689_S2_10_1344_760_120P       */{{  16,   8, 1344,  760}, {AMBA_SENSOR_SUBSAMPLE_TYPE_BINNING, 1, 2}, {AMBA_SENSOR_SUBSAMPLE_TYPE_BINNING, 1, 2}},
/* OV4689_S1_10_2688_1520_2LANE_30P */{{  16,   8, 2688, 1520}, { AMBA_SENSOR_SUBSAMPLE_TYPE_NORMAL, 1, 1}, { AMBA_SENSOR_SUBSAMPLE_TYPE_NORMAL, 1, 1}},
};

const AMBA_SENSOR_OUTPUT_INFO_s OV4689OutputInfo[OV4689_NUM_READOUT_MODE_REG] = {
/* OV4689_S1_10_2688_1520_60P       */ { 720000000, 4, 10, AMBA_DSP_BAYER_BG, {AMBA_DSP_PHASE_SHIFT_MODE_0, AMBA_DSP_PHASE_SHIFT_MODE_0}, 2688, 1520, {   0,   4, 2688, 1512}, {0}},
/* OV4689_S1_10_2688_1520_30P       */ { 420000000, 4, 10, AMBA_DSP_BAYER_BG, {AMBA_DSP_PHASE_SHIFT_MODE_0, AMBA_DSP_PHASE_SHIFT_MODE_0}, 2688, 1520, {   0,   4, 2688, 1512}, {0}},
/* OV4689_S2_10_1344_760_30P        */ { 240000000, 2, 10, AMBA_DSP_BAYER_BG, {AMBA_DSP_PHASE_SHIFT_MODE_0, AMBA_DSP_PHASE_SHIFT_MODE_0}, 1344,  760, {   0,   0, 1344,  760}, {0}},
/* OV4689_S1_10_2688_1520_HDR_30P   */ { 840000000, 4, 10, AMBA_DSP_BAYER_BG, {AMBA_DSP_PHASE_SHIFT_MODE_0, AMBA_DSP_PHASE_SHIFT_MODE_0}, 2688, 3640, {   0,   0, 2688, 3640}, {0}},
/* OV4689_S1_10_2120_1196_CROP_120P */ {1008000000, 4, 10, AMBA_DSP_BAYER_BG, {AMBA_DSP_PHASE_SHIFT_MODE_0, AMBA_DSP_PHASE_SHIFT_MODE_0}, 2120, 1196, {   4,   4, 2112, 1188}, {0}},
/* OV4689_S2_10_1344_760_120P       */ { 600000000, 4, 10, AMBA_DSP_BAYER_BG, {AMBA_DSP_PHASE_SHIFT_MODE_0, AMBA_DSP_PHASE_SHIFT_MODE_0}, 1344,  760, {   0,   0, 1344,  760}, {0}},
/* OV4689_S1_10_2688_1520_2LANE_30P */ { 648000000, 2, 10, AMBA_DSP_BAYER_BG, {AMBA_DSP_PHASE_SHIFT_MODE_0, AMBA_DSP_PHASE_SHIFT_MODE_0}, 2688, 1520, {   0,   4, 2688, 1512}, {0}},
};

const OV4689_MODE_INFO_s OV4689ModeInfoList[AMBA_SENSOR_OV4689_NUM_MODE] = {
/* AMBA_SENSOR_OV4689_V1_10_2688_1512_60P       */ {       OV4689_S1_10_2688_1520_60P, {24023976,  1200 * 4,     1670, { .Interlace = 0, .TimeScale =  60000, .NumUnitsInTick = 1001}}},
/* AMBA_SENSOR_OV4689_V1_10_2688_1512_45P       */ {       OV4689_S1_10_2688_1520_60P, {24040800,  1200 * 4,     2226, { .Interlace = 0, .TimeScale =     45, .NumUnitsInTick =    1}}},
/* AMBA_SENSOR_OV4689_V1_10_2688_1512_40P       */ {       OV4689_S1_10_2688_1520_60P, {24038400,  1200 * 4,     2504, { .Interlace = 0, .TimeScale =     40, .NumUnitsInTick =    1}}},
/* AMBA_SENSOR_OV4689_V1_10_2688_1512_30P       */ {       OV4689_S1_10_2688_1520_30P, {23995205,  2400 * 4,     1668, { .Interlace = 0, .TimeScale =  30000, .NumUnitsInTick = 1001}}},
/* AMBA_SENSOR_OV4689_V1_10_2688_1512_60P_30P   */ {       OV4689_S1_10_2688_1520_60P, {24023976,  1200 * 4,     3340, { .Interlace = 0, .TimeScale =  30000, .NumUnitsInTick = 1001}}},
/* AMBA_SENSOR_OV4689_V1_10_2688_1512_50P       */ {       OV4689_S1_10_2688_1520_60P, {24048000,  1200 * 4,     2004, { .Interlace = 0, .TimeScale =     50, .NumUnitsInTick =    1}}},
/* AMBA_SENSOR_OV4689_V1_10_2688_1512_25P       */ {       OV4689_S1_10_2688_1520_30P, {24024000,  2400 * 4,     2002, { .Interlace = 0, .TimeScale =     25, .NumUnitsInTick =    1}}},
/* AMBA_SENSOR_OV4689_V1_10_2688_1512_50P_25P   */ {       OV4689_S1_10_2688_1520_60P, {24048000,  1200 * 4,     4008, { .Interlace = 0, .TimeScale =     25, .NumUnitsInTick =    1}}},
/* AMBA_SENSOR_OV4689_V2_10_1344_760_30P        */ {        OV4689_S2_10_1344_760_30P, {23962549,  5016 * 4,      797, { .Interlace = 0, .TimeScale =  30000, .NumUnitsInTick = 1001}}},
/* AMBA_SENSOR_OV4689_V2_10_1344_760_25P        */ {        OV4689_S2_10_1344_760_30P, {24001560,  5016 * 4,      957, { .Interlace = 0, .TimeScale =     25, .NumUnitsInTick =    1}}},
/* AMBA_SENSOR_OV4689_V1_10_2688_1512_30P_HDR   */ {   OV4689_S1_10_2688_1520_HDR_30P, {23983696,  1042 * 4, 1920 * 2, { .Interlace = 0, .TimeScale =  30000, .NumUnitsInTick = 1001}}},
/* AMBA_SENSOR_OV4689_V1_10_2688_1512_25P_HDR   */ {   OV4689_S1_10_2688_1520_HDR_30P, {23986840,  1042 * 4, 2302 * 2, { .Interlace = 0, .TimeScale =     25, .NumUnitsInTick =    1}}},
/* AMBA_SENSOR_OV4689_V1_10_2112_1188_CROP_120P */ { OV4689_S1_10_2120_1196_CROP_120P, {24064256,   816 * 4,     1230, { .Interlace = 0, .TimeScale = 120000, .NumUnitsInTick = 1001}}},
/* AMBA_SENSOR_OV4689_V1_10_2112_1188_CROP_100P */ { OV4689_S1_10_2120_1196_CROP_120P, {24072000,   816 * 4,     1475, { .Interlace = 0, .TimeScale =    100, .NumUnitsInTick =    1}}},
/* AMBA_SENSOR_OV4689_V2_10_1344_760_120P       */ {       OV4689_S2_10_1344_760_120P, {24107413,   840 * 4,     1197, { .Interlace = 0, .TimeScale = 120000, .NumUnitsInTick = 1001}}},
/* AMBA_SENSOR_OV4689_V2_10_1344_760_100P       */ {       OV4689_S2_10_1344_760_120P, {24074400,   840 * 4,     1433, { .Interlace = 0, .TimeScale =    100, .NumUnitsInTick =    1}}},
/* AMBA_SENSOR_OV4689_V2_10_2688_1512_2LANE_30P */ { OV4689_S1_10_2688_1520_2LANE_30P, {23988204,  2572 * 4,     1556, { .Interlace = 0, .TimeScale =  30000, .NumUnitsInTick = 1001}}},
/* AMBA_SENSOR_OV4689_V2_10_2688_1512_2LANE_25P */ { OV4689_S1_10_2688_1520_2LANE_30P, {23996760,  2572 * 4,     1866, { .Interlace = 0, .TimeScale =     25, .NumUnitsInTick =    1}}},
};

const AMBA_SENSOR_HDR_INFO_s OV4689HdrInfo[AMBA_SENSOR_OV4689_NUM_MODE] = {
    [AMBA_SENSOR_OV4689_V1_10_2688_1512_30P_HDR] = {
        .HdrType = AMBA_SENSOR_HDR_TYPE_MULTI_SLICE,
        .ActiveChannels = 2,
        .ChannelInfo = {
            [0] = {
                .EffectiveArea = {  0,   4, 2688, 1512},
                .OpticalBlackPixels = {0},
                .ShutterSpeedCtrlInfo = {AMBA_SENSOR_COMMUNICATION_AT_NON_VBLANK, 2, 1, 1},
                .OutputFormatCtrlInfo = {AMBA_SENSOR_COMMUNICATION_AT_NON_VBLANK, 2, 1, 1},
                . MaxExposureLine = 3232, /* 1 exposure line = 2 row time in HDR mode */
                . MinExposureLine = 1,
            },
            [1] = {
                .EffectiveArea = {2688, 304, 2688, 1512},
                .OpticalBlackPixels = {0},
                .ShutterSpeedCtrlInfo = {AMBA_SENSOR_COMMUNICATION_AT_NON_VBLANK, 2, 1, 1},
                .OutputFormatCtrlInfo = {AMBA_SENSOR_COMMUNICATION_AT_NON_VBLANK, 2, 1, 1},
                . MaxExposureLine = 598, /* 1 exposure line = 2 row time in HDR mode */
                . MinExposureLine = 1,
            },
        },
    },
    [AMBA_SENSOR_OV4689_V1_10_2688_1512_25P_HDR] = {
        .HdrType = AMBA_SENSOR_HDR_TYPE_MULTI_SLICE,
        .ActiveChannels = 2,
        .ChannelInfo = {
            [0] = {
                .EffectiveArea = {  0,   4, 2688, 1512},
                .OpticalBlackPixels = {0},
                .ShutterSpeedCtrlInfo = {AMBA_SENSOR_COMMUNICATION_AT_NON_VBLANK, 2, 0, 0},
                .OutputFormatCtrlInfo = {AMBA_SENSOR_COMMUNICATION_AT_NON_VBLANK, 2, 0, 0},
                . MaxExposureLine = 3996, /* 1 exposure line = 2 row time in HDR mode */
                . MinExposureLine = 2,
            },
            [1] = {
                .EffectiveArea = {2688, 304, 2688, 1512},
                .OpticalBlackPixels = {0},
                .ShutterSpeedCtrlInfo = {AMBA_SENSOR_COMMUNICATION_AT_NON_VBLANK, 2, 0, 0},
                .OutputFormatCtrlInfo = {AMBA_SENSOR_COMMUNICATION_AT_NON_VBLANK, 2, 0, 0},
                . MaxExposureLine = 598, /* 1 exposure line = 2 row time in HDR mode */
                . MinExposureLine = 2,
            },
        },
    },
};
