/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaSensor_IMX290Table.c
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

#include <AmbaSSP.h>

#include "AmbaDSP.h"
#include "AmbaSensor.h"
#include "AmbaSensor_IMX290.h"

/*-----------------------------------------------------------------------------------------------*\
 * Sensor Device Information
\*-----------------------------------------------------------------------------------------------*/
const AMBA_SENSOR_DEVICE_INFO_s IMX290DeviceInfo = {
    .UnitCellWidth          = 2.9,
    .UnitCellHeight         = 2.9,
    .NumTotalPixelCols      = 1945,
    .NumTotalPixelRows      = 1109,
    .NumEffectivePixelCols  = 1945,
    .NumEffectivePixelRows  = 1097,
    .MinAnalogGainFactor    = 1.0,
    .MaxAnalogGainFactor    = 3981.071706,
    .MinDigitalGainFactor   = 1.0,
    .MaxDigitalGainFactor   = 1.0,

    .FrameRateCtrlInfo = {
        .CommunicationTime      = AMBA_SENSOR_COMMUNICATION_AT_VBLANK,
        .FirstReflectedFrame    = 0,
        .FirstBadFrame          = 0,
        .NumBadFrames           = 0
    },
    .ShutterSpeedCtrlInfo = {
        .CommunicationTime      = AMBA_SENSOR_COMMUNICATION_AT_VBLANK,
        .FirstReflectedFrame    = 1,
        .FirstBadFrame          = 0,
        .NumBadFrames           = 0
    },
    .AnalogGainCtrlInfo = {
        .CommunicationTime      = AMBA_SENSOR_COMMUNICATION_AT_VBLANK,
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

    .HdrIsSupport = 1,
};

/*-----------------------------------------------------------------------------------------------*\
 * Sensor register settings of each readout modes
\*-----------------------------------------------------------------------------------------------*/

IMX290_SEQ_REG_s IMX290InitRegTable[IMX290_NUM_INIT_REG] = {
    /* Chip ID: 02h */
    {0x300F, 0x00},
    {0x3010, 0x21},
    {0x3012, 0x64},
    {0x3016, 0x09},
    {0x3070, 0x02},
    {0x3071, 0x11},
    {0x309B, 0x10},
    {0x309C, 0x22},
    {0x30A2, 0x02},
    {0x30A6, 0x20},
    {0x30A8, 0x20},
    {0x30AA, 0x20},
    {0x30AC, 0x20},
    {0x30B0, 0x43},
    /* Chip ID: 03h */
    //{0x310B, 0x01}, /* According to SONY FAE's suggestion, set 1 to fix lowlight issue for 1080p */
    {0x3119, 0x9E},
    {0x311C, 0x1E},
    {0x311E, 0x08},
    {0x3128, 0x05},
    {0x313D, 0x83},
    {0x3150, 0x03},
    {0x317E, 0x00},
    /* Chip ID: 04h */
    {0x32B8, 0x50},
    {0x32B9, 0x10},
    {0x32BA, 0x00},
    {0x32BB, 0x04},
    {0x32C8, 0x50},
    {0x32C9, 0x10},
    {0x32CA, 0x00},
    {0x32CB, 0x04},
    /* Chip ID: 05h */
    {0x332C, 0xD3},
    {0x332D, 0x10},
    {0x332E, 0x0D},
    {0x3358, 0x06},
    {0x3359, 0xE1},
    {0x335A, 0x11},
    {0x3360, 0x1E},
    {0x3361, 0x61},
    {0x3362, 0x10},
    {0x33B0, 0x50},
    {0x33B2, 0x1A},
    {0x33B3, 0x04},
};

    /*         37.125(MHz)  74.25(MHz)
     * 0x305C  0x18         0x0C       INCKSEL 1
     * 0x305D  0x00         0x00       INCKSEL 2
     * 0x305E  0x20         0x10       INCKSEL 3
     * 0x305F  0x01         0x01       INCKSEL 4
     * 0x315E  0x1A         0x1B       INCKSEL 5
     * 0x3164  0x1A         0x1B       INCKSEL 6 */

IMX290_REG_s IMX290RegTable[IMX290_NUM_READOUT_MODE_REG] = {
    {0x305C, {0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18}}, // INCKSEL 1
    {0x305D, {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}}, // INCKSEL 2
    {0x305E, {0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20}}, // INCKSEL 3
    {0x305F, {0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01}}, // INCKSEL 4
    {0x315E, {0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A}}, // INCKSEL 5
    {0x3164, {0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A}}, // INCKSEL 6
    {0x3005, {0x00, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00}}, // ADBIT[0] 0:10bit, 1:12bit
    {0x3007, {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}}, // VREVERSE[0]; HREVERSE[1]; WINMODE[6:4];
    {0x3009, {0x00, 0x01, 0x02, 0x01, 0x00, 0x00, 0x00, 0x00, 0x01, 0x02, 0x01, 0x00, 0x00, 0x00}}, // FRSEL[1:0] 2:30/25fps, 1:60/50fps, 0:120/100fps; FDGSEL[4] 0:LCG, 1:HCG
    {0x300A, {0x3C, 0xF0, 0xF0, 0xF0, 0x3C, 0x3C, 0x3C, 0x3C, 0xF0, 0xF0, 0xF0, 0x3C, 0x3C, 0x3C}}, // BLKLEVEL_LSB 12bit:0xF0 10bit:0x3C
    {0x300B, {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}}, // BLKLEVEL_MSB
    {0x300C, {0x00, 0x00, 0x00, 0x11, 0x11, 0x11, 0x11, 0x00, 0x00, 0x00, 0x11, 0x11, 0x11, 0x11}}, // WDMODE [0] 0:Normal mode, 1:DOL mode, [5:4] 0:Normal mode, 1:DOL 2frame, 2:DOL 3frame
    {0x3018, {0x65, 0x65, 0x65, 0x65, 0x65, 0xC4, 0xCA, 0x65, 0x65, 0x65, 0x65, 0x65, 0xC4, 0xCA}}, // VMAX_LSB
    {0x3019, {0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x08, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x08}}, // VMAX_MSB
    {0x301A, {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}}, // VMAX_HSB
    {0x301C, {0x4C, 0x98, 0x30, 0x98, 0x4C, 0xF6, 0x4C, 0x28, 0x50, 0xA0, 0x50, 0x28, 0xC0, 0x28}}, // HMAX_LSB
    {0x301D, {0x04, 0x08, 0x11, 0x08, 0x04, 0x03, 0x04, 0x05, 0x0A, 0x14, 0x0A, 0x05, 0x04, 0x05}}, // HMAX_HSB
    {0x3046, {0xF0, 0xE1, 0xD1, 0xE1, 0xF0, 0xF0, 0xF0, 0xF0, 0xE1, 0xD1, 0xE1, 0xF0, 0xF0, 0xF0}}, // ODBIT[1:0] 0:10bit, 1:12bit; OPORTSEL[7:4] 0:Parallel, D:2ch LVDS, E:4ch LVDS, F:8ch LVDS,
    {0x3129, {0x1D, 0x00, 0x00, 0x00, 0x1D, 0x1D, 0x1D, 0x1D, 0x00, 0x00, 0x00, 0x1D, 0x1D, 0x1D}}, // ADBIT2 10bit:0x1D, 12bit:0x00
    {0x317C, {0x12, 0x00, 0x00, 0x00, 0x12, 0x12, 0x12, 0x12, 0x00, 0x00, 0x00, 0x12, 0x12, 0x12}}, // ADBIT2 10bit:0x12, 12bit:0x00
    {0x31EC, {0x37, 0x0E, 0x0E, 0x0E, 0x37, 0x37, 0x37, 0x37, 0x0E, 0x0E, 0x0E, 0x37, 0x37, 0x37}}, // ADBIT3 10bit:0x37, 12bit:0x0E
    {0x31A0, {0xFC, 0xFC, 0xFC, 0xFC, 0xB4, 0x50, 0xB4, 0xFC, 0xFC, 0xFC, 0xFC, 0xB4, 0x50, 0xB4}}, // HBLANK_LSB
    {0x31A1, {0x00, 0x00, 0x00, 0x00, 0x02, 0x02, 0x02, 0x00, 0x00, 0x00, 0x00, 0x02, 0x02, 0x02}}, // HBLANK_MSB
    {0x3415, {0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00}}, // NULL0_Size
    {0x3418, {0x49, 0x49, 0x49, 0xB2, 0xB2, 0xB2, 0xB2, 0x49, 0x49, 0x49, 0xB2, 0xB2, 0xB2, 0xB2}}, // YOUTSIZE_LSB
    {0x3419, {0x04, 0x04, 0x04, 0x08, 0x08, 0x08, 0x08, 0x04, 0x04, 0x04, 0x08, 0x08, 0x08, 0x08}}, // YOUTSIZE_MSB
    {0x3030, {0x00, 0x00, 0x00, 0x0B, 0x0B, 0xC9, 0xA3, 0x00, 0x00, 0x00, 0x0B, 0x0B, 0xC9, 0xA3}}, // RHS1_LSB
    {0x3031, {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02}}, // RHS1_MSB
    {0x3032, {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}}, // RHS1_HSB
    {0x3045, {0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00}}, // DOL_FORMAT
    {0x3106, {0x00, 0x00, 0x00, 0x90, 0x90, 0x90, 0x90, 0x00, 0x00, 0x00, 0x90, 0x90, 0x90, 0x90}}, // DOL_SYNCSIGNAL
};

const AMBA_SENSOR_INPUT_INFO_s IMX290InputInfo[IMX290_NUM_READOUT_MODE] = {
/* IMX290_1080P120_10BIT_8CH                        */ {{0,   0, 1920, 1080}, { AMBA_SENSOR_SUBSAMPLE_TYPE_NORMAL,  1, 1}, { AMBA_SENSOR_SUBSAMPLE_TYPE_NORMAL,  1,  1}, 1},
/* IMX290_1080P60_12BIT_4CH                         */ {{0,   0, 1920, 1080}, { AMBA_SENSOR_SUBSAMPLE_TYPE_NORMAL,  1, 1}, { AMBA_SENSOR_SUBSAMPLE_TYPE_NORMAL,  1,  1}, 1},
/* IMX290_1080P30_12BIT_2CH                         */ {{0,   0, 1920, 1080}, { AMBA_SENSOR_SUBSAMPLE_TYPE_NORMAL,  1, 1}, { AMBA_SENSOR_SUBSAMPLE_TYPE_NORMAL,  1,  1}, 1},
/* IMX290_1080P30_12BIT_4CH_DLO_2FRAME              */ {{0,   0, 1920, 1080}, { AMBA_SENSOR_SUBSAMPLE_TYPE_NORMAL,  1, 1}, { AMBA_SENSOR_SUBSAMPLE_TYPE_NORMAL,  1,  1}, 1},
/* IMX290_1080P60_10BIT_8CH_DLO_2FRAME              */ {{0,   0, 1920, 1080}, { AMBA_SENSOR_SUBSAMPLE_TYPE_NORMAL,  1, 1}, { AMBA_SENSOR_SUBSAMPLE_TYPE_NORMAL,  1,  1}, 1},
/* IMX290_1080P60_10BIT_8CH_DLO_2FRAME_EXTEND_EXP_S */ {{0,   0, 1920, 1080}, { AMBA_SENSOR_SUBSAMPLE_TYPE_NORMAL,  1, 1}, { AMBA_SENSOR_SUBSAMPLE_TYPE_NORMAL,  1,  1}, 1},
/* IMX290_1080P60_P30_10BIT_8CH_DLO_2FRAME          */ {{0,   0, 1920, 1080}, { AMBA_SENSOR_SUBSAMPLE_TYPE_NORMAL,  1, 1}, { AMBA_SENSOR_SUBSAMPLE_TYPE_NORMAL,  1,  1}, 1},
/* IMX290_1080P100_10BIT_8CH                        */ {{0,   0, 1920, 1080}, { AMBA_SENSOR_SUBSAMPLE_TYPE_NORMAL,  1, 1}, { AMBA_SENSOR_SUBSAMPLE_TYPE_NORMAL,  1,  1}, 1},
/* IMX290_1080P50_12BIT_4CH                         */ {{0,   0, 1920, 1080}, { AMBA_SENSOR_SUBSAMPLE_TYPE_NORMAL,  1, 1}, { AMBA_SENSOR_SUBSAMPLE_TYPE_NORMAL,  1,  1}, 1},
/* IMX290_1080P25_12BIT_2CH                         */ {{0,   0, 1920, 1080}, { AMBA_SENSOR_SUBSAMPLE_TYPE_NORMAL,  1, 1}, { AMBA_SENSOR_SUBSAMPLE_TYPE_NORMAL,  1,  1}, 1},
/* IMX290_1080P25_12BIT_4CH_DLO_2FRAME              */ {{0,   0, 1920, 1080}, { AMBA_SENSOR_SUBSAMPLE_TYPE_NORMAL,  1, 1}, { AMBA_SENSOR_SUBSAMPLE_TYPE_NORMAL,  1,  1}, 1},
/* IMX290_1080P50_10BIT_8CH_DLO_2FRAME              */ {{0,   0, 1920, 1080}, { AMBA_SENSOR_SUBSAMPLE_TYPE_NORMAL,  1, 1}, { AMBA_SENSOR_SUBSAMPLE_TYPE_NORMAL,  1,  1}, 1},
/* IMX290_1080P50_10BIT_8CH_DLO_2FRAME_EXTEND_EXP_S */ {{0,   0, 1920, 1080}, { AMBA_SENSOR_SUBSAMPLE_TYPE_NORMAL,  1, 1}, { AMBA_SENSOR_SUBSAMPLE_TYPE_NORMAL,  1,  1}, 1},
/* IMX290_1080P50_P25_10BIT_8CH_DLO_2FRAME          */ {{0,   0, 1920, 1080}, { AMBA_SENSOR_SUBSAMPLE_TYPE_NORMAL,  1, 1}, { AMBA_SENSOR_SUBSAMPLE_TYPE_NORMAL,  1,  1}, 1},
};

const AMBA_SENSOR_OUTPUT_INFO_s IMX290OutputInfo[IMX290_NUM_READOUT_MODE] = {
/* IMX290_1080P120_10BIT_8CH = 0                    */ {445054945, 8, 10, AMBA_DSP_BAYER_RG, {AMBA_DSP_PHASE_SHIFT_MODE_0, AMBA_DSP_PHASE_SHIFT_MODE_0}, 1948, 1110, {12, 21, 1920, 1080}, {0}},
/* IMX290_1080P60_12BIT_4CH                         */ {445054945, 4, 12, AMBA_DSP_BAYER_RG, {AMBA_DSP_PHASE_SHIFT_MODE_0, AMBA_DSP_PHASE_SHIFT_MODE_0}, 1948, 1110, {12, 21, 1920, 1080}, {0}},
/* IMX290_1080P30_12BIT_2CH                         */ {445054945, 2, 12, AMBA_DSP_BAYER_RG, {AMBA_DSP_PHASE_SHIFT_MODE_0, AMBA_DSP_PHASE_SHIFT_MODE_0}, 1948, 1110, {12, 21, 1920, 1080}, {0}},
/* IMX290_1080P30_12BIT_4CH_DLO_2FRAME              */ {445054945, 4, 12, AMBA_DSP_BAYER_RG, {AMBA_DSP_PHASE_SHIFT_MODE_0, AMBA_DSP_PHASE_SHIFT_MODE_0}, 1948, 2230, {12,  0, 1920, 2230}, {0}},
/* IMX290_1080P60_10BIT_8CH_DLO_2FRAME              */ {445054945, 8, 10, AMBA_DSP_BAYER_RG, {AMBA_DSP_PHASE_SHIFT_MODE_0, AMBA_DSP_PHASE_SHIFT_MODE_0}, 1948, 2230, {12,  0, 1920, 2230}, {0}},
/* IMX290_1080P60_10BIT_8CH_DLO_2FRAME_EXTEND_EXP_S */ {445054945, 8, 10, AMBA_DSP_BAYER_RG, {AMBA_DSP_PHASE_SHIFT_MODE_0, AMBA_DSP_PHASE_SHIFT_MODE_0}, 1948, 2420, {12,  0, 1920, 2420}, {0}},
/* IMX290_1080P60_P30_10BIT_8CH_DLO_2FRAME          */ {445054945, 8, 10, AMBA_DSP_BAYER_RG, {AMBA_DSP_PHASE_SHIFT_MODE_0, AMBA_DSP_PHASE_SHIFT_MODE_0}, 1948, 2894, {12,  0, 1920, 2894}, {0}},
/* IMX290_1080P100_10BIT_8CH                        */ {445054945, 8, 10, AMBA_DSP_BAYER_RG, {AMBA_DSP_PHASE_SHIFT_MODE_0, AMBA_DSP_PHASE_SHIFT_MODE_0}, 1948, 1110, {12, 21, 1920, 1080}, {0}},
/* IMX290_1080P50_12BIT_4CH                         */ {445054945, 4, 12, AMBA_DSP_BAYER_RG, {AMBA_DSP_PHASE_SHIFT_MODE_0, AMBA_DSP_PHASE_SHIFT_MODE_0}, 1948, 1110, {12, 21, 1920, 1080}, {0}},
/* IMX290_1080P25_12BIT_2CH                         */ {445054945, 2, 12, AMBA_DSP_BAYER_RG, {AMBA_DSP_PHASE_SHIFT_MODE_0, AMBA_DSP_PHASE_SHIFT_MODE_0}, 1948, 1110, {12, 21, 1920, 1080}, {0}},
/* IMX290_1080P25_12BIT_4CH_DLO_2FRAME              */ {445054945, 4, 12, AMBA_DSP_BAYER_RG, {AMBA_DSP_PHASE_SHIFT_MODE_0, AMBA_DSP_PHASE_SHIFT_MODE_0}, 1948, 2230, {12,  0, 1920, 2230}, {0}},
/* IMX290_1080P50_10BIT_8CH_DLO_2FRAME              */ {445054945, 8, 10, AMBA_DSP_BAYER_RG, {AMBA_DSP_PHASE_SHIFT_MODE_0, AMBA_DSP_PHASE_SHIFT_MODE_0}, 1948, 2230, {12,  0, 1920, 2230}, {0}},
/* IMX290_1080P50_10BIT_8CH_DLO_2FRAME_EXTEND_EXP_S */ {445054945, 8, 10, AMBA_DSP_BAYER_RG, {AMBA_DSP_PHASE_SHIFT_MODE_0, AMBA_DSP_PHASE_SHIFT_MODE_0}, 1948, 2420, {12,  0, 1920, 2420}, {0}},
/* IMX290_1080P50_P25_10BIT_8CH_DLO_2FRAME          */ {445054945, 8, 10, AMBA_DSP_BAYER_RG, {AMBA_DSP_PHASE_SHIFT_MODE_0, AMBA_DSP_PHASE_SHIFT_MODE_0}, 1948, 2894, {12,  0, 1920, 2894}, {0}},
};

const IMX290_MODE_INFO_s IMX290ModeInfoList[AMBA_SENSOR_IMX290_NUM_MODE] = {
/* AMBA_SENSOR_IMX290_1080P120_10BIT                        */ {                       IMX290_1080P120_10BIT_8CH, {37087912, 2640, 1125, { .Interlace = 0, .TimeScale = 120000, .NumUnitsInTick = 1001}}},
/* AMBA_SENSOR_IMX290_1080P60_12BIT                         */ {                        IMX290_1080P60_12BIT_4CH, {37087912, 2200, 1125, { .Interlace = 0, .TimeScale =  60000, .NumUnitsInTick = 1001}}},
/* AMBA_SENSOR_IMX290_1080P30_12BIT                         */ {                        IMX290_1080P30_12BIT_2CH, {37087912, 2200, 1125, { .Interlace = 0, .TimeScale =  30000, .NumUnitsInTick = 1001}}},
/* AMBA_SENSOR_IMX290_1080P30_12BIT_DLO_2FRAME              */ {             IMX290_1080P30_12BIT_4CH_DLO_2FRAME, {37087912, 2200, 2250, { .Interlace = 0, .TimeScale =  30000, .NumUnitsInTick = 1001}}},
/* AMBA_SENSOR_IMX290_1080P60_10BIT_DLO_2FRAME              */ {             IMX290_1080P60_10BIT_8CH_DLO_2FRAME, {37087912, 2640, 2250, { .Interlace = 0, .TimeScale =  60000, .NumUnitsInTick = 1001}}},
/* AMBA_SENSOR_IMX290_1080P60_10BIT_DLO_2FRAME_EXTEND_EXP_S */ {IMX290_1080P60_10BIT_8CH_DLO_2FRAME_EXTEND_EXP_S, {37075325, 2434, 2440, { .Interlace = 0, .TimeScale =  60000, .NumUnitsInTick = 1001}}},
/* AMBA_SENSOR_IMX290_1080P60_P30_10BIT_DLO_2FRAME          */ {         IMX290_1080P60_P30_10BIT_8CH_DLO_2FRAME, {37087912, 2640, 4500, { .Interlace = 0, .TimeScale =  30000, .NumUnitsInTick = 1001}}},
/* AMBA_SENSOR_IMX290_1080P100_10BIT                        */ {                       IMX290_1080P100_10BIT_8CH, {37125000, 3168, 1125, { .Interlace = 0, .TimeScale =    100, .NumUnitsInTick =    1}}},
/* AMBA_SENSOR_IMX290_1080P50_12BIT                         */ {                        IMX290_1080P50_12BIT_4CH, {37125000, 2640, 1125, { .Interlace = 0, .TimeScale =     50, .NumUnitsInTick =    1}}},
/* AMBA_SENSOR_IMX290_1080P25_12BIT                         */ {                        IMX290_1080P25_12BIT_2CH, {37125000, 2640, 1125, { .Interlace = 0, .TimeScale =     25, .NumUnitsInTick =    1}}},
/* AMBA_SENSOR_IMX290_1080P25_12BIT_DLO_2FRAME              */ {             IMX290_1080P25_12BIT_4CH_DLO_2FRAME, {37125000, 2640, 2250, { .Interlace = 0, .TimeScale =     25, .NumUnitsInTick =    1}}},
/* AMBA_SENSOR_IMX290_1080P50_10BIT_DLO_2FRAME              */ {             IMX290_1080P50_10BIT_8CH_DLO_2FRAME, {37125000, 3168, 2250, { .Interlace = 0, .TimeScale =     50, .NumUnitsInTick =    1}}},
/* AMBA_SENSOR_IMX290_1080P50_10BIT_DLO_2FRAME_EXTEND_EXP_S */ {IMX290_1080P50_10BIT_8CH_DLO_2FRAME_EXTEND_EXP_S, {37088000, 2918, 2440, { .Interlace = 0, .TimeScale =     50, .NumUnitsInTick =    1}}},
/* AMBA_SENSOR_IMX290_1080P50_P25_10BIT_DLO_2FRAME          */ {         IMX290_1080P50_P25_10BIT_8CH_DLO_2FRAME, {37125000, 3168, 4500, { .Interlace = 0, .TimeScale =     25, .NumUnitsInTick =    1}}},
};

const AMBA_SENSOR_HDR_INFO_s IMX290HdrInfo[AMBA_SENSOR_IMX290_NUM_MODE] = {
    [AMBA_SENSOR_IMX290_1080P30_12BIT_DLO_2FRAME] = {
        .HdrType = AMBA_SENSOR_HDR_TYPE_MULTI_SLICE,
        .ActiveChannels = 2,
        .ChannelInfo = {
            [0] = {
                .EffectiveArea = {0, 21, 1920, 1080},
                .OpticalBlackPixels = {0},
                .ShutterSpeedCtrlInfo = {AMBA_SENSOR_COMMUNICATION_AT_VBLANK, 2, 0, 0},
                .OutputFormatCtrlInfo = {AMBA_SENSOR_COMMUNICATION_AT_VBLANK, 1, 0, 0},
                . MaxExposureLine = 2237,
                . MinExposureLine = 2,
            },
            [1] = {
                .EffectiveArea = {1920, 26, 1920, 1080},
                .OpticalBlackPixels = {0},
                .ShutterSpeedCtrlInfo = {AMBA_SENSOR_COMMUNICATION_AT_VBLANK, 2, 0, 0},
                .OutputFormatCtrlInfo = {AMBA_SENSOR_COMMUNICATION_AT_VBLANK, 1, 0, 0},
                . MaxExposureLine = 9,
                . MinExposureLine = 2,
            },
        },
    },
    [AMBA_SENSOR_IMX290_1080P60_10BIT_DLO_2FRAME] = {
        .HdrType = AMBA_SENSOR_HDR_TYPE_MULTI_SLICE,
        .ActiveChannels = 2,
        .ChannelInfo = {
            [0] = {
                .EffectiveArea = {0, 21, 1920, 1080},
                .OpticalBlackPixels = {0},
                .ShutterSpeedCtrlInfo = {AMBA_SENSOR_COMMUNICATION_AT_VBLANK, 2, 0, 0},
                .OutputFormatCtrlInfo = {AMBA_SENSOR_COMMUNICATION_AT_VBLANK, 1, 0, 0},
                . MaxExposureLine = 2237,
                . MinExposureLine = 2,
            },
            [1] = {
                .EffectiveArea = {1920, 26, 1920, 1080},
                .OpticalBlackPixels = {0},
                .ShutterSpeedCtrlInfo = {AMBA_SENSOR_COMMUNICATION_AT_VBLANK, 2, 0, 0},
                .OutputFormatCtrlInfo = {AMBA_SENSOR_COMMUNICATION_AT_VBLANK, 1, 0, 0},
                . MaxExposureLine = 9,
                . MinExposureLine = 2,
            },
        },
    },
    [AMBA_SENSOR_IMX290_1080P60_10BIT_DLO_2FRAME_EXTEND_EXP_S] = {
        .HdrType = AMBA_SENSOR_HDR_TYPE_MULTI_SLICE,
        .ActiveChannels = 2,
        .ChannelInfo = {
            [0] = {
                .EffectiveArea = {0, 21, 1920, 1080},
                .OpticalBlackPixels = {0},
                .ShutterSpeedCtrlInfo = {AMBA_SENSOR_COMMUNICATION_AT_VBLANK, 2, 0, 0},
                .OutputFormatCtrlInfo = {AMBA_SENSOR_COMMUNICATION_AT_VBLANK, 1, 0, 0},
                . MaxExposureLine = 2237,
                . MinExposureLine = 2,
            },
            [1] = {
                .EffectiveArea = {1920, 121, 1920, 1080},
                .OpticalBlackPixels = {0},
                .ShutterSpeedCtrlInfo = {AMBA_SENSOR_COMMUNICATION_AT_VBLANK, 2, 0, 0},
                .OutputFormatCtrlInfo = {AMBA_SENSOR_COMMUNICATION_AT_VBLANK, 1, 0, 0},
                . MaxExposureLine = 199,
                . MinExposureLine = 2,
            },
        },
    },
    [AMBA_SENSOR_IMX290_1080P60_P30_10BIT_DLO_2FRAME] = {
        .HdrType = AMBA_SENSOR_HDR_TYPE_MULTI_SLICE,
        .ActiveChannels = 2,
        .ChannelInfo = {
            [0] = {
                .EffectiveArea = {0, 21, 1920, 1080},
                .OpticalBlackPixels = {0},
                .ShutterSpeedCtrlInfo = {AMBA_SENSOR_COMMUNICATION_AT_VBLANK, 2, 0, 0},
                .OutputFormatCtrlInfo = {AMBA_SENSOR_COMMUNICATION_AT_VBLANK, 1, 0, 0},
                . MaxExposureLine = 3823,
                . MinExposureLine = 2,
            },
            [1] = {
                .EffectiveArea = {1920, 358, 1920, 1080},
                .OpticalBlackPixels = {0},
                .ShutterSpeedCtrlInfo = {AMBA_SENSOR_COMMUNICATION_AT_VBLANK, 2, 0, 0},
                .OutputFormatCtrlInfo = {AMBA_SENSOR_COMMUNICATION_AT_VBLANK, 1, 0, 0},
                . MaxExposureLine = 673,
                . MinExposureLine = 2,
            },
        },
    },
    [AMBA_SENSOR_IMX290_1080P25_12BIT_DLO_2FRAME] = {
        .HdrType = AMBA_SENSOR_HDR_TYPE_MULTI_SLICE,
        .ActiveChannels = 2,
        .ChannelInfo = {
            [0] = {
                .EffectiveArea = {0, 21, 1920, 1080},
                .OpticalBlackPixels = {0},
                .ShutterSpeedCtrlInfo = {AMBA_SENSOR_COMMUNICATION_AT_VBLANK, 2, 0, 0},
                .OutputFormatCtrlInfo = {AMBA_SENSOR_COMMUNICATION_AT_VBLANK, 1, 0, 0},
                . MaxExposureLine = 2237,
                . MinExposureLine = 2,
            },
            [1] = {
                .EffectiveArea = {1920, 26, 1920, 1080},
                .OpticalBlackPixels = {0},
                .ShutterSpeedCtrlInfo = {AMBA_SENSOR_COMMUNICATION_AT_VBLANK, 2, 0, 0},
                .OutputFormatCtrlInfo = {AMBA_SENSOR_COMMUNICATION_AT_VBLANK, 1, 0, 0},
                . MaxExposureLine = 9,
                . MinExposureLine = 2,
            },
        },
    },
    [AMBA_SENSOR_IMX290_1080P50_10BIT_DLO_2FRAME] = {
        .HdrType = AMBA_SENSOR_HDR_TYPE_MULTI_SLICE,
        .ActiveChannels = 2,
        .ChannelInfo = {
            [0] = {
                .EffectiveArea = {0, 21, 1920, 1080},
                .OpticalBlackPixels = {0},
                .ShutterSpeedCtrlInfo = {AMBA_SENSOR_COMMUNICATION_AT_VBLANK, 2, 0, 0},
                .OutputFormatCtrlInfo = {AMBA_SENSOR_COMMUNICATION_AT_VBLANK, 1, 0, 0},
                . MaxExposureLine = 2237,
                . MinExposureLine = 2,
            },
            [1] = {
                .EffectiveArea = {1920, 26, 1920, 1080},
                .OpticalBlackPixels = {0},
                .ShutterSpeedCtrlInfo = {AMBA_SENSOR_COMMUNICATION_AT_VBLANK, 2, 0, 0},
                .OutputFormatCtrlInfo = {AMBA_SENSOR_COMMUNICATION_AT_VBLANK, 1, 0, 0},
                . MaxExposureLine = 9,
                . MinExposureLine = 2,
            },
        },
    },
    [AMBA_SENSOR_IMX290_1080P50_10BIT_DLO_2FRAME_EXTEND_EXP_S] = {
        .HdrType = AMBA_SENSOR_HDR_TYPE_MULTI_SLICE,
        .ActiveChannels = 2,
        .ChannelInfo = {
            [0] = {
                .EffectiveArea = {0, 21, 1920, 1080},
                .OpticalBlackPixels = {0},
                .ShutterSpeedCtrlInfo = {AMBA_SENSOR_COMMUNICATION_AT_VBLANK, 2, 0, 0},
                .OutputFormatCtrlInfo = {AMBA_SENSOR_COMMUNICATION_AT_VBLANK, 1, 0, 0},
                . MaxExposureLine = 2237,
                . MinExposureLine = 2,
            },
            [1] = {
                .EffectiveArea = {1920, 121, 1920, 1080},
                .OpticalBlackPixels = {0},
                .ShutterSpeedCtrlInfo = {AMBA_SENSOR_COMMUNICATION_AT_VBLANK, 2, 0, 0},
                .OutputFormatCtrlInfo = {AMBA_SENSOR_COMMUNICATION_AT_VBLANK, 1, 0, 0},
                . MaxExposureLine = 199,
                . MinExposureLine = 2,
            },
        },
    },
    [AMBA_SENSOR_IMX290_1080P50_P25_10BIT_DLO_2FRAME] = {
        .HdrType = AMBA_SENSOR_HDR_TYPE_MULTI_SLICE,
        .ActiveChannels = 2,
        .ChannelInfo = {
            [0] = {
                .EffectiveArea = {0, 21, 1920, 1080},
                .OpticalBlackPixels = {0},
                .ShutterSpeedCtrlInfo = {AMBA_SENSOR_COMMUNICATION_AT_VBLANK, 2, 0, 0},
                .OutputFormatCtrlInfo = {AMBA_SENSOR_COMMUNICATION_AT_VBLANK, 1, 0, 0},
                . MaxExposureLine = 3823,
                . MinExposureLine = 2,
            },
            [1] = {
                .EffectiveArea = {1920, 358, 1920, 1080},
                .OpticalBlackPixels = {0},
                .ShutterSpeedCtrlInfo = {AMBA_SENSOR_COMMUNICATION_AT_VBLANK, 2, 0, 0},
                .OutputFormatCtrlInfo = {AMBA_SENSOR_COMMUNICATION_AT_VBLANK, 1, 0, 0},
                . MaxExposureLine = 673,
                . MinExposureLine = 2,
            },
        },
    },
};
