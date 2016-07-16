/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaSensor_IMX117Table.c
 *
 *  @Copyright      :: Copyright (C) 2013 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Control APIs of SONY IMX117 CMOS sensor with LVDS interface
\*-------------------------------------------------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "AmbaDataType.h"
#include "AmbaDSP.h"
#include "AmbaSensor.h"
#include "AmbaSensor_IMX117.h"

/*-----------------------------------------------------------------------------------------------*\
 * Sensor Device Information
\*-----------------------------------------------------------------------------------------------*/
const AMBA_SENSOR_DEVICE_INFO_s IMX117DeviceInfo = {
    .UnitCellWidth          = 1.55,
    .UnitCellHeight         = 1.55,
    .NumTotalPixelCols      = 4168,
    .NumTotalPixelRows      = 3062,
    .NumEffectivePixelCols  = 4072,
    .NumEffectivePixelRows  = 3046,
    .MinAnalogGainFactor    = 1.0,
    .MaxAnalogGainFactor    = 24.0, /* 27 dB */
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
IMX117_REG_s IMX117RegTable[IMX117_NUM_READOUT_MODE_REG] = {
    /*       [              Type1/2.3 12.4M                            ]  [Type1/2.5 9.03M][3.09M] */
    /* Addr    0,   0A,   2A,   4A,    4,    3,    5,    6     7     10    0     1     4     1     Cus   */
    {0x0003, {0x00, 0x00, 0x22, 0x11, 0x33, 0x44, 0x33, 0x55, 0x33, 0x11, 0x00, 0x00, 0x33, 0x11, 0x11}},
    {0x0004, {0x00, 0x00, 0x11, 0x48, 0x40, 0x2a, 0x51, 0x51, 0x61, 0x40, 0x80, 0x80, 0xa0, 0xb8, 0x00}},
    {0x0005, {0x07, 0x47, 0x0d, 0x21, 0x1d, 0x1d, 0x1d, 0x1d, 0x1d, 0x21, 0x47, 0x41, 0x5d, 0x81, 0x01}},
    {0x0006, {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x83, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}},
    {0x000d, {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}},
    {0x007e, {0x20, 0x20, 0x20, 0x00, 0x00, 0x20, 0x20, 0x20, 0x20, 0x7f, 0x20, 0x20, 0x00, 0x20, 0x61}},
    {0x007f, {0x01, 0x05, 0x01, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x11, 0x01, 0x01, 0x00, 0x01, 0x05}},
    {0x0080, {  NA,   NA,   NA,   NA,   NA,   NA,   NA,   NA,   NA, 0x12,   NA,   NA,   NA,   NA, 0x0e}},
    {0x0081, {  NA,   NA,   NA,   NA,   NA,   NA,   NA,   NA,   NA, 0x00,   NA,   NA,   NA,   NA,   NA}},
    {0x0082, {  NA,   NA,   NA,   NA,   NA,   NA,   NA,   NA,   NA, 0x0c,   NA,   NA,   NA,   NA, 0x1a}},
    {0x0083, {  NA,   NA,   NA,   NA,   NA,   NA,   NA,   NA,   NA, 0x00,   NA,   NA,   NA,   NA,   NA}},
    {0x0084, {  NA,   NA,   NA,   NA,   NA,   NA,   NA,   NA,   NA, 0xbe,   NA,   NA,   NA,   NA, 0xe2}},
    {0x0085, {  NA,   NA,   NA,   NA,   NA,   NA,   NA,   NA,   NA, 0x03,   NA,   NA,   NA,   NA, 0x05}},
    {0x0086, {  NA,   NA,   NA,   NA,   NA,   NA,   NA,   NA,   NA, 0x7a,   NA,   NA,   NA,   NA, 0xca}},
    {0x0087, {  NA,   NA,   NA,   NA,   NA,   NA,   NA,   NA,   NA, 0x04,   NA,   NA,   NA,   NA, 0x06}},
    {0x0088, {  NA,   NA,   NA,   NA,   NA,   NA,   NA,   NA,   NA, 0x01,   NA,   NA,   NA,   NA,   NA}},
    {0x0089, {  NA,   NA,   NA,   NA,   NA,   NA,   NA,   NA,   NA, 0x07,   NA,   NA,   NA,   NA,   NA}},
    {0x008a, {  NA,   NA,   NA,   NA,   NA,   NA,   NA,   NA,   NA, 0x00,   NA,   NA,   NA,   NA,   NA}},
    {0x008b, {  NA,   NA,   NA,   NA,   NA,   NA,   NA,   NA,   NA, 0x00,   NA,   NA,   NA,   NA,   NA}},
    {0x008c, {  NA,   NA,   NA,   NA,   NA,   NA,   NA,   NA,   NA, 0x00,   NA,   NA,   NA,   NA,   NA}},
    {0x008d, {  NA,   NA,   NA,   NA,   NA,   NA,   NA,   NA,   NA, 0x00,   NA,   NA,   NA,   NA,   NA}},
    {0x008e, {  NA,   NA,   NA,   NA,   NA,   NA,   NA,   NA,   NA, 0x01,   NA,   NA,   NA,   NA,   NA}},
    {0x008f, {  NA,   NA,   NA,   NA,   NA,   NA,   NA,   NA,   NA, 0x07,   NA,   NA,   NA,   NA,   NA}},
    {0x0090, {  NA,   NA,   NA,   NA,   NA,   NA,   NA,   NA,   NA, 0x00,   NA,   NA,   NA,   NA,   NA}},
    {0x0091, {  NA,   NA,   NA,   NA,   NA,   NA,   NA,   NA,   NA, 0x00,   NA,   NA,   NA,   NA,   NA}},
    {0x0092, {  NA,   NA,   NA,   NA,   NA,   NA,   NA,   NA,   NA, 0x00,   NA,   NA,   NA,   NA,   NA}},
    {0x0093, {  NA,   NA,   NA,   NA,   NA,   NA,   NA,   NA,   NA, 0x00,   NA,   NA,   NA,   NA, 0x10}},
    {0x0094, {  NA,   NA,   NA,   NA,   NA,   NA,   NA,   NA,   NA, 0x11,   NA,   NA,   NA,   NA,   NA}},
    {0x0095, {  NA,   NA,   NA,   NA,   NA,   NA,   NA,   NA,   NA, 0x14,   NA,   NA,   NA,   NA, 0x10}},
    {0x0096, {  NA,   NA,   NA,   NA,   NA,   NA,   NA,   NA,   NA, 0x00,   NA,   NA,   NA,   NA, 0x10}},
    {0x0097, {  NA,   NA,   NA,   NA,   NA,   NA,   NA,   NA,   NA, 0x0c,   NA,   NA,   NA,   NA, 0x1a}},
    {0x0098, {  NA,   NA,   NA,   NA,   NA,   NA,   NA,   NA,   NA, 0x00,   NA,   NA,   NA,   NA,   NA}},
    {0x0099, {  NA,   NA,   NA,   NA,   NA,   NA,   NA,   NA,   NA, 0xc0,   NA,   NA,   NA,   NA, 0xe4}},
    {0x009a, {  NA,   NA,   NA,   NA,   NA,   NA,   NA,   NA,   NA, 0x03,   NA,   NA,   NA,   NA, 0x05}},
    {0x009b, {  NA,   NA,   NA,   NA,   NA,   NA,   NA,   NA,   NA, 0x7a,   NA,   NA,   NA,   NA, 0xca}},
    {0x009c, {  NA,   NA,   NA,   NA,   NA,   NA,   NA,   NA,   NA, 0x04,   NA,   NA,   NA,   NA, 0x06}},
    {0x009d, {  NA,   NA,   NA,   NA,   NA,   NA,   NA,   NA,   NA, 0x01,   NA,   NA,   NA,   NA,   NA}},
    {0x009e, {  NA,   NA,   NA,   NA,   NA,   NA,   NA,   NA,   NA, 0x07,   NA,   NA,   NA,   NA,   NA}},
    {0x009f, {  NA,   NA,   NA,   NA,   NA,   NA,   NA,   NA,   NA, 0x00,   NA,   NA,   NA,   NA,   NA}},
    {0x00a0, {  NA,   NA,   NA,   NA,   NA,   NA,   NA,   NA,   NA, 0x00,   NA,   NA,   NA,   NA,   NA}},
    {0x00a1, {  NA,   NA,   NA,   NA,   NA,   NA,   NA,   NA,   NA, 0x00,   NA,   NA,   NA,   NA,   NA}},
    {0x00a2, {  NA,   NA,   NA,   NA,   NA,   NA,   NA,   NA,   NA, 0x00,   NA,   NA,   NA,   NA,   NA}},
    {0x00a3, {  NA,   NA,   NA,   NA,   NA,   NA,   NA,   NA,   NA, 0x01,   NA,   NA,   NA,   NA,   NA}},
    {0x00a4, {  NA,   NA,   NA,   NA,   NA,   NA,   NA,   NA,   NA, 0x07,   NA,   NA,   NA,   NA,   NA}},
    {0x00a5, {  NA,   NA,   NA,   NA,   NA,   NA,   NA,   NA,   NA, 0x00,   NA,   NA,   NA,   NA,   NA}},
    {0x00a6, {  NA,   NA,   NA,   NA,   NA,   NA,   NA,   NA,   NA, 0x00,   NA,   NA,   NA,   NA,   NA}},
    {0x00a7, {  NA,   NA,   NA,   NA,   NA,   NA,   NA,   NA,   NA, 0x00,   NA,   NA,   NA,   NA,   NA}},
    {0x00a8, {  NA,   NA,   NA,   NA,   NA,   NA,   NA,   NA,   NA, 0x00,   NA,   NA,   NA,   NA,   NA}},
    {0x00a9, {  NA,   NA,   NA,   NA,   NA,   NA,   NA,   NA,   NA, 0x11,   NA,   NA,   NA,   NA,   NA}},
    {0x00aa, {  NA,   NA,   NA,   NA,   NA,   NA,   NA,   NA,   NA, 0x04,   NA,   NA,   NA,   NA,   NA}},
    {0x00ab, {  NA,   NA,   NA,   NA,   NA,   NA,   NA,   NA,   NA, 0x04,   NA,   NA,   NA,   NA,   NA}},
    {0x00ac, {  NA,   NA,   NA,   NA,   NA,   NA,   NA,   NA,   NA, 0x04,   NA,   NA,   NA,   NA,   NA}},
    {0x00ad, {  NA,   NA,   NA,   NA,   NA,   NA,   NA,   NA,   NA, 0x05,   NA,   NA,   NA,   NA,   NA}},
    {0x00ae, {  NA,   NA,   NA,   NA,   NA,   NA,   NA,   NA,   NA, 0x04,   NA,   NA,   NA,   NA,   NA}},
    {0x00af, {  NA,   NA,   NA,   NA,   NA,   NA,   NA,   NA,   NA, 0x04,   NA,   NA,   NA,   NA,   NA}},
    {0x00b0, {  NA,   NA,   NA,   NA,   NA,   NA,   NA,   NA,   NA, 0x04,   NA,   NA,   NA,   NA,   NA}},
    {0x00b1, {  NA,   NA,   NA,   NA,   NA,   NA,   NA,   NA,   NA, 0x08,   NA,   NA,   NA,   NA,   NA}},
    {0x00b2, {  NA,   NA,   NA,   NA,   NA,   NA,   NA,   NA,   NA, 0x08,   NA,   NA,   NA,   NA,   NA}},
    {0x00b3, {  NA,   NA,   NA,   NA,   NA,   NA,   NA,   NA,   NA, 0x05,   NA,   NA,   NA,   NA,   NA}},
    {0x00b4, {  NA,   NA,   NA,   NA,   NA,   NA,   NA,   NA,   NA, 0x08,   NA,   NA,   NA,   NA,   NA}},
    {0x00b5, {  NA,   NA,   NA,   NA,   NA,   NA,   NA,   NA,   NA, 0x08,   NA,   NA,   NA,   NA,   NA}},
    {0x00b6, {0x47, 0x47, 0x67, 0x00, 0x00, 0x77, 0x09, 0x09, 0x09, 0x04, 0x47, 0x47, 0x00, 0x47, 0x47}},
    {0x00b7, {  NA,   NA,   NA,   NA,   NA, 0x00, 0x14, 0x14, 0x00, 0x0a,   NA,   NA,   NA,   NA,   NA}},
    {0x00b8, {  NA,   NA,   NA,   NA,   NA, 0x00, 0x14, 0x14, 0x28, 0x14,   NA,   NA,   NA,   NA,   NA}},
    {0x00b9, {  NA,   NA,   NA,   NA,   NA, 0x77, 0x09, 0x09, 0x09, 0x04,   NA,   NA,   NA,   NA,   NA}},
    {0x00ba, {  NA,   NA,   NA,   NA,   NA, 0x00, 0x14, 0x14, 0x00, 0x0a,   NA,   NA,   NA,   NA,   NA}},
    {0x00bb, {  NA,   NA,   NA,   NA,   NA, 0x00, 0x14, 0x14, 0x28, 0x14,   NA,   NA,   NA,   NA,   NA}},
    {0x00bc, {  NA,   NA,   NA,   NA,   NA,   NA,   NA,   NA,   NA, 0x00,   NA,   NA,   NA,   NA,   NA}},
    {0x00bd, {  NA,   NA,   NA,   NA,   NA,   NA,   NA,   NA,   NA, 0x00,   NA,   NA,   NA,   NA,   NA}},
    {0x00be, {  NA,   NA,   NA,   NA,   NA,   NA,   NA,   NA,   NA, 0x09,   NA,   NA,   NA,   NA, 0x07}},
    {0x00bf, {  NA,   NA,   NA,   NA,   NA,   NA,   NA,   NA,   NA, 0x00,   NA,   NA,   NA,   NA,   NA}},
    {0x00c0, {  NA,   NA,   NA,   NA,   NA,   NA,   NA,   NA,   NA, 0x48,   NA,   NA,   NA,   NA, 0x4a}},
    {0x00c1, {  NA,   NA,   NA,   NA,   NA,   NA,   NA,   NA,   NA, 0x00,   NA,   NA,   NA,   NA,   NA}},
    {0x00c2, {  NA,   NA,   NA,   NA,   NA,   NA,   NA,   NA,   NA, 0xbb,   NA,   NA,   NA,   NA, 0xcc}},
    {0x00c3, {  NA,   NA,   NA,   NA,   NA,   NA,   NA,   NA,   NA, 0x01,   NA,   NA,   NA,   NA, 0x02}},
    {0x00c4, {  NA,   NA,   NA,   NA,   NA,   NA,   NA,   NA,   NA, 0xac,   NA,   NA,   NA,   NA, 0x7e}},
    {0x00c5, {  NA,   NA,   NA,   NA,   NA,   NA,   NA,   NA,   NA, 0x15,   NA,   NA,   NA,   NA, 0x13}},
    {0x00c6, {  NA,   NA,   NA,   NA,   NA,   NA,   NA,   NA,   NA, 0x9d,   NA,   NA,   NA,   NA, 0xbc}},
    {0x00c7, {  NA,   NA,   NA,   NA,   NA,   NA,   NA,   NA,   NA, 0x01,   NA,   NA,   NA,   NA, 0x02}},
    {0x00c8, {  NA,   NA,   NA,   NA,   NA,   NA,   NA,   NA,   NA, 0x00,   NA,   NA,   NA,   NA,   NA}},
    {0x00c9, {  NA,   NA,   NA,   NA,   NA,   NA,   NA,   NA,   NA, 0x00,   NA,   NA,   NA,   NA,   NA}},
    {0x00ca, {  NA,   NA,   NA,   NA,   NA,   NA,   NA,   NA,   NA, 0x00,   NA,   NA,   NA,   NA,   NA}},
    {0x00cb, {  NA,   NA,   NA,   NA,   NA,   NA,   NA,   NA,   NA, 0x00,   NA,   NA,   NA,   NA,   NA}},
    {0x00cc, {  NA,   NA,   NA,   NA,   NA,   NA,   NA,   NA,   NA, 0x02,   NA,   NA,   NA,   NA, 0x02}},


    {0x00ce, {0x0e, 0x0e, 0x0e, 0x00, 0x00, 0x10, 0x10, 0x10, 0x10, 0x10, 0x0e, 0x0e, 0x00, 0x0e, 0x0e}},
    {0x00d3, {  NA,   NA,   NA,   NA,   NA,   NA,   NA,   NA,   NA, 0x02,   NA,   NA,   NA,   NA,   NA}},
    {0x0222, {0x31, 0x31, 0x31, 0x31, 0x31, 0x31, 0x31, 0x31, 0x31, 0x31, 0x31, 0x31, 0x31, 0x31, 0x31}},
    {0x0223, {0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01}},
    {0x0358, {0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01}},
    {0x0528, {0x0e, 0x0e, 0x0e, 0x0e, 0x0e, 0x0e, 0x0e, 0x0e, 0x0e, 0x0e, 0x0e, 0x0e, 0x0e, 0x0e, 0x0e}},
    {0x0529, {0x0e, 0x0e, 0x0e, 0x0e, 0x0e, 0x0e, 0x0e, 0x0e, 0x0e, 0x0e, 0x0e, 0x0e, 0x0e, 0x0e, 0x0e}},
    {0x052a, {0x0e, 0x0e, 0x0e, 0x0e, 0x0e, 0x0e, 0x0e, 0x0e, 0x0e, 0x0e, 0x0e, 0x0e, 0x0e, 0x0e, 0x0e}},
    {0x052b, {0x0e, 0x0e, 0x0e, 0x0e, 0x0e, 0x0e, 0x0e, 0x0e, 0x0e, 0x0e, 0x0e, 0x0e, 0x0e, 0x0e, 0x0e}},
    {0x057e, {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}},
    {0x057f, {0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10}},
    {0x0580, {0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d}},
    {0x0581, {0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d}},
    {0x0585, {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}},
    {0x0586, {0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07}},
    {0x0617, {0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10}},
    {0x065c, {0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05}},
    {0x0700, {0x19, 0x19, 0x19, 0x19, 0x19, 0x19, 0x19, 0x19, 0x19, 0x19, 0x19, 0x19, 0x19, 0x19, 0x19}},
    {0x0701, {0x19, 0x19, 0x19, 0x19, 0x19, 0x19, 0x19, 0x19, 0x19, 0x19, 0x19, 0x19, 0x19, 0x19, 0x19}},
    {0x00d0, {  NA, 0x98,   NA,   NA,   NA,   NA,   NA,   NA,   NA,   NA,   NA,   NA,   NA,   NA, 0x3c}},
    {0x00d1, {  NA, 0x10,   NA,   NA,   NA,   NA,   NA,   NA,   NA,   NA,   NA,   NA,   NA,   NA, 0x0e}},
};

const AMBA_SENSOR_OUTPUT_INFO_s IMX117OutputInfo[IMX117_NUM_READOUT_MODE] = {
/*IMX117_TYPE_2_3_MODE_0  */ {576000000, 10, 12, AMBA_DSP_BAYER_RG, {AMBA_DSP_PHASE_SHIFT_MODE_0, AMBA_DSP_PHASE_SHIFT_MODE_0}, 4168, 3062, { 132,  40, 4000, 3000}, {0}},
/*IMX117_TYPE_2_3_MODE_0A */ {576000000, 10, 12, AMBA_DSP_BAYER_RG, {AMBA_DSP_PHASE_SHIFT_MODE_0, AMBA_DSP_PHASE_SHIFT_MODE_0}, 4248, 3062, { 124,  40, 4096, 3000}, {0}},
/*IMX117_TYPE_2_3_MODE_2A */ {576000000,  6, 12, AMBA_DSP_BAYER_RG, {AMBA_DSP_PHASE_SHIFT_MODE_0, AMBA_DSP_PHASE_SHIFT_MODE_0}, 2084, 1524, {  66,  14, 2000, 1500}, {0}},
/*IMX117_TYPE_2_3_MODE_4A */ {576000000,  8, 10, AMBA_DSP_BAYER_RG, {AMBA_DSP_PHASE_SHIFT_MODE_0, AMBA_DSP_PHASE_SHIFT_MODE_0}, 2084,  770, {  66,  12, 2000,  750}, {0}},
/*IMX117_TYPE_2_3_MODE_4  */ {576000000,  4, 12, AMBA_DSP_BAYER_RG, {AMBA_DSP_PHASE_SHIFT_MODE_0, AMBA_DSP_PHASE_SHIFT_MODE_0}, 1388, 1020, {  44,  12, 1332, 1000}, {0}},
/*IMX117_TYPE_2_3_MODE_3  */ {576000000,  2, 12, AMBA_DSP_BAYER_RG, {AMBA_DSP_PHASE_SHIFT_MODE_0, AMBA_DSP_PHASE_SHIFT_MODE_0}, 1388, 1022, {  44,  14, 1332,  998}, {0}},
/*IMX117_TYPE_2_3_MODE_5  */ {576000000,  4, 12, AMBA_DSP_BAYER_RG, {AMBA_DSP_PHASE_SHIFT_MODE_0, AMBA_DSP_PHASE_SHIFT_MODE_0}, 1388,  344, {  44,   8, 1332,  332}, {0}},
/*IMX117_TYPE_2_3_MODE_6  */ {576000000,  1, 12, AMBA_DSP_BAYER_RG, {AMBA_DSP_PHASE_SHIFT_MODE_0, AMBA_DSP_PHASE_SHIFT_MODE_0}, 1388,  344, {  44,   8, 1332,  332}, {0}},
/*IMX117_TYPE_2_3_MODE_7  */ {576000000,  4, 12, AMBA_DSP_BAYER_RG, {AMBA_DSP_PHASE_SHIFT_MODE_0, AMBA_DSP_PHASE_SHIFT_MODE_0}, 1388,  186, {  44,   8, 1332,  174}, {0}},
/*IMX117_TYPE_2_3_MODE_10 */ {576000000,  8, 10, AMBA_DSP_BAYER_RG, {AMBA_DSP_PHASE_SHIFT_MODE_0, AMBA_DSP_PHASE_SHIFT_MODE_0}, 2084, 1150, {  66,  14, 2000, 1126}, {0}},

/*IMX117_TYPE_2_5_MODE_0  */ {576000000, 10, 12, AMBA_DSP_BAYER_RG, {AMBA_DSP_PHASE_SHIFT_MODE_0, AMBA_DSP_PHASE_SHIFT_MODE_0}, 4248, 2182, { 124,  18, 4096, 2160}, {0}},
/*IMX117_TYPE_2_5_MODE_1  */ {576000000, 10, 10, AMBA_DSP_BAYER_RG, {AMBA_DSP_PHASE_SHIFT_MODE_0, AMBA_DSP_PHASE_SHIFT_MODE_0}, 4248, 2182, { 124,  18, 4096, 2160}, {0}},
/*IMX117_TYPE_2_5_MODE_4  */ {576000000,  4, 12, AMBA_DSP_BAYER_RG, {AMBA_DSP_PHASE_SHIFT_MODE_0, AMBA_DSP_PHASE_SHIFT_MODE_0}, 1416,  738, {  42,  14, 1364,  720}, {0}},

/*IMX117_TYPE_4_4_MODE_1  */ {576000000,  8, 10, AMBA_DSP_BAYER_RG, {AMBA_DSP_PHASE_SHIFT_MODE_0, AMBA_DSP_PHASE_SHIFT_MODE_0}, 2456, 1318, { 124,  18, 2304, 1296}, {0}},

/*IMX117_CUSTOM_MODE      */ {576000000,  8, 10, AMBA_DSP_BAYER_RG, {AMBA_DSP_PHASE_SHIFT_MODE_0, AMBA_DSP_PHASE_SHIFT_MODE_0}, 3648, 1754, { 628,  40, 3008, 1692}, {0}},
};

const AMBA_SENSOR_INPUT_INFO_s IMX117InputInfoNormalReadout[IMX117_NUM_READOUT_MODE] = {
/*IMX117_TYPE_2_3_MODE_0  */ {{ 48,   0, 4000, 3000}, { AMBA_SENSOR_SUBSAMPLE_TYPE_NORMAL, 1, 1}, { AMBA_SENSOR_SUBSAMPLE_TYPE_NORMAL, 1,  1}, 1},
/*IMX117_TYPE_2_3_MODE_0A */ {{  0,   0, 4096, 3000}, { AMBA_SENSOR_SUBSAMPLE_TYPE_NORMAL, 1, 1}, { AMBA_SENSOR_SUBSAMPLE_TYPE_NORMAL, 1,  1}, 1},
/*IMX117_TYPE_2_3_MODE_2A */ {{ 48,   0, 4000, 3000}, {AMBA_SENSOR_SUBSAMPLE_TYPE_BINNING, 1, 2}, {AMBA_SENSOR_SUBSAMPLE_TYPE_BINNING, 1,  2}, 1},
/*IMX117_TYPE_2_3_MODE_4A */ {{ 48, 376, 4000, 2250}, {AMBA_SENSOR_SUBSAMPLE_TYPE_BINNING, 1, 2}, {AMBA_SENSOR_SUBSAMPLE_TYPE_BINNING, 1,  3}, 1},
/*IMX117_TYPE_2_3_MODE_4  */ {{ 48,   0, 3996, 3000}, {AMBA_SENSOR_SUBSAMPLE_TYPE_BINNING, 1, 3}, {AMBA_SENSOR_SUBSAMPLE_TYPE_BINNING, 1,  3}, 1},
/*IMX117_TYPE_2_3_MODE_3  */ {{ 48,   2, 3996, 2994}, {AMBA_SENSOR_SUBSAMPLE_TYPE_BINNING, 1, 3}, {AMBA_SENSOR_SUBSAMPLE_TYPE_BINNING, 1,  3}, 1},
/*IMX117_TYPE_2_3_MODE_5  */ {{ 48,   6, 3996, 2988}, {AMBA_SENSOR_SUBSAMPLE_TYPE_BINNING, 1, 3}, {AMBA_SENSOR_SUBSAMPLE_TYPE_BINNING, 1,  9}, 1},
/*IMX117_TYPE_2_3_MODE_6  */ {{ 48,   6, 3996, 2988}, {AMBA_SENSOR_SUBSAMPLE_TYPE_BINNING, 1, 3}, {AMBA_SENSOR_SUBSAMPLE_TYPE_BINNING, 1,  9}, 1},
/*IMX117_TYPE_2_3_MODE_7  */ {{ 48,  24, 3996, 2958}, {AMBA_SENSOR_SUBSAMPLE_TYPE_BINNING, 1, 3}, {AMBA_SENSOR_SUBSAMPLE_TYPE_BINNING, 1, 17}, 1},
/*IMX117_TYPE_2_3_MODE_10 */ {{ 48,  24, 4000, 2252}, {AMBA_SENSOR_SUBSAMPLE_TYPE_BINNING, 1, 2}, {AMBA_SENSOR_SUBSAMPLE_TYPE_BINNING, 1,  2}, 1},

/*IMX117_TYPE_2_5_MODE_0  */ {{  0, 420, 4096, 2160}, { AMBA_SENSOR_SUBSAMPLE_TYPE_NORMAL, 1, 1}, { AMBA_SENSOR_SUBSAMPLE_TYPE_NORMAL, 1,  1}, 1},
/*IMX117_TYPE_2_5_MODE_1  */ {{  0, 420, 4096, 2160}, { AMBA_SENSOR_SUBSAMPLE_TYPE_NORMAL, 1, 1}, { AMBA_SENSOR_SUBSAMPLE_TYPE_NORMAL, 1,  1}, 1},
/*IMX117_TYPE_2_5_MODE_4  */ {{  0, 422, 4092, 2160}, {AMBA_SENSOR_SUBSAMPLE_TYPE_BINNING, 1, 3}, {AMBA_SENSOR_SUBSAMPLE_TYPE_BINNING, 1,  3}, 1},

/*IMX117_TYPE_4_4_MODE_1  */ {{896, 852, 2304, 1296}, { AMBA_SENSOR_SUBSAMPLE_TYPE_NORMAL, 1, 1}, { AMBA_SENSOR_SUBSAMPLE_TYPE_NORMAL, 1,  1}, 1},

/*IMX117_CUSTOM_MODE      */ {{544, 654, 3008, 1692}, { AMBA_SENSOR_SUBSAMPLE_TYPE_NORMAL, 1, 1}, { AMBA_SENSOR_SUBSAMPLE_TYPE_NORMAL, 1,  1}, 1},
};

const AMBA_SENSOR_INPUT_INFO_s IMX117InputInfoInversionReadout[IMX117_NUM_READOUT_MODE] = {
/*IMX117_TYPE_2_3_MODE_0  */ {{ 48,   0, 4000, 3000}, { AMBA_SENSOR_SUBSAMPLE_TYPE_NORMAL, 1, 1}, { AMBA_SENSOR_SUBSAMPLE_TYPE_NORMAL, 1,  1}, 1},
/*IMX117_TYPE_2_3_MODE_0A */ {{  0,   0, 4096, 3000}, { AMBA_SENSOR_SUBSAMPLE_TYPE_NORMAL, 1, 1}, { AMBA_SENSOR_SUBSAMPLE_TYPE_NORMAL, 1,  1}, 1},
/*IMX117_TYPE_2_3_MODE_2A */ {{ 48,   2, 4000, 3000}, {AMBA_SENSOR_SUBSAMPLE_TYPE_BINNING, 1, 2}, {AMBA_SENSOR_SUBSAMPLE_TYPE_BINNING, 1,  2}, 1},
/*IMX117_TYPE_2_3_MODE_4A */ {{ 48, 378, 4000, 2250}, {AMBA_SENSOR_SUBSAMPLE_TYPE_BINNING, 1, 2}, {AMBA_SENSOR_SUBSAMPLE_TYPE_BINNING, 1,  3}, 1},
/*IMX117_TYPE_2_3_MODE_4  */ {{ 48,   2, 3996, 3000}, {AMBA_SENSOR_SUBSAMPLE_TYPE_BINNING, 1, 3}, {AMBA_SENSOR_SUBSAMPLE_TYPE_BINNING, 1,  3}, 1},
/*IMX117_TYPE_2_3_MODE_3  */ {{ 48,   4, 3996, 2994}, {AMBA_SENSOR_SUBSAMPLE_TYPE_BINNING, 1, 3}, {AMBA_SENSOR_SUBSAMPLE_TYPE_BINNING, 1,  3}, 1},
/*IMX117_TYPE_2_3_MODE_5  */ {{ 48,   8, 3996, 2988}, {AMBA_SENSOR_SUBSAMPLE_TYPE_BINNING, 1, 3}, {AMBA_SENSOR_SUBSAMPLE_TYPE_BINNING, 1,  9}, 1},
/*IMX117_TYPE_2_3_MODE_6  */ {{ 48,   8, 3996, 2988}, {AMBA_SENSOR_SUBSAMPLE_TYPE_BINNING, 1, 3}, {AMBA_SENSOR_SUBSAMPLE_TYPE_BINNING, 1,  9}, 1},
/*IMX117_TYPE_2_3_MODE_7  */ {{ 48,  24, 3996, 2958}, {AMBA_SENSOR_SUBSAMPLE_TYPE_BINNING, 1, 3}, {AMBA_SENSOR_SUBSAMPLE_TYPE_BINNING, 1, 17}, 1},
/*IMX117_TYPE_2_3_MODE_10 */ {{ 48,  24, 4000, 2252}, {AMBA_SENSOR_SUBSAMPLE_TYPE_BINNING, 1, 2}, {AMBA_SENSOR_SUBSAMPLE_TYPE_BINNING, 1,  2}, 1},

/*IMX117_TYPE_2_5_MODE_0  */ {{  0, 422, 4096, 2160}, { AMBA_SENSOR_SUBSAMPLE_TYPE_NORMAL, 1, 1}, { AMBA_SENSOR_SUBSAMPLE_TYPE_NORMAL, 1,  1}, 1},
/*IMX117_TYPE_2_5_MODE_1  */ {{  0, 422, 4096, 2160}, { AMBA_SENSOR_SUBSAMPLE_TYPE_NORMAL, 1, 1}, { AMBA_SENSOR_SUBSAMPLE_TYPE_NORMAL, 1,  1}, 1},
/*IMX117_TYPE_2_5_MODE_4  */ {{  0, 422, 4092, 2160}, {AMBA_SENSOR_SUBSAMPLE_TYPE_BINNING, 1, 3}, {AMBA_SENSOR_SUBSAMPLE_TYPE_BINNING, 1,  3}, 1},

/*IMX117_TYPE_4_4_MODE_1  */ {{ 896, 854, 2304, 1296}, { AMBA_SENSOR_SUBSAMPLE_TYPE_NORMAL, 1, 1}, { AMBA_SENSOR_SUBSAMPLE_TYPE_NORMAL, 1,  1}, 1},

/*IMX117_CUSTOM_MODE      */ {{544, 654, 3008, 1692}, { AMBA_SENSOR_SUBSAMPLE_TYPE_NORMAL, 1, 1}, { AMBA_SENSOR_SUBSAMPLE_TYPE_NORMAL, 1,  1}, 1},
};

const IMX117_MODE_INFO_s IMX117ModeInfoList[AMBA_SENSOR_IMX117_NUM_MODE] = {
/*AMBA_SENSOR_IMX117_TYPE_2_3_MODE_0_30P  */ { IMX117_TYPE_2_3_MODE_0, { 72000000,  770, 1, 3120, 1, { .Interlace = 0, .TimeScale = 30000,  .NumUnitsInTick = 1001}}},
/*AMBA_SENSOR_IMX117_TYPE_2_3_MODE_0A_35P */ {IMX117_TYPE_2_3_MODE_0A, { 72000000,  660, 1, 3120, 1, { .Interlace = 0, .TimeScale = 35000,  .NumUnitsInTick = 1001}}},
/*AMBA_SENSOR_IMX117_TYPE_2_3_MODE_2A_60P */ {IMX117_TYPE_2_3_MODE_2A, { 72000000,  390, 2, 3080, 1, { .Interlace = 0, .TimeScale = 60000,  .NumUnitsInTick = 1001}}},
/*AMBA_SENSOR_IMX117_TYPE_2_3_MODE_4A_240P*/ {IMX117_TYPE_2_3_MODE_4A, { 72000000,  364, 1,  825, 1, { .Interlace = 0, .TimeScale = 240000, .NumUnitsInTick = 1001}}},
/*AMBA_SENSOR_IMX117_TYPE_2_3_MODE_4_120P */ { IMX117_TYPE_2_3_MODE_4, { 72000000,  572, 1, 1050, 1, { .Interlace = 0, .TimeScale = 120000, .NumUnitsInTick = 1001}}},
/*AMBA_SENSOR_IMX117_TYPE_2_3_MODE_3_60P  */ { IMX117_TYPE_2_3_MODE_3, { 72000000,  385, 3, 3120, 1, { .Interlace = 0, .TimeScale = 60000,  .NumUnitsInTick = 1001}}},
/*AMBA_SENSOR_IMX117_TYPE_2_3_MODE_5_240P */ { IMX117_TYPE_2_3_MODE_5, { 72000000,  420, 2,  715, 1, { .Interlace = 0, .TimeScale = 240000, .NumUnitsInTick = 1001}}},
/*AMBA_SENSOR_IMX117_TYPE_2_3_MODE_6_30P  */ { IMX117_TYPE_2_3_MODE_6, { 72000000,  429, 8, 5600, 8, { .Interlace = 0, .TimeScale = 30000,  .NumUnitsInTick = 1001}}},
/*AMBA_SENSOR_IMX117_TYPE_2_3_MODE_7_480P */ { IMX117_TYPE_2_3_MODE_7, { 72000000,  385, 2,  390, 1, { .Interlace = 0, .TimeScale = 480000, .NumUnitsInTick = 1001}}},
/*AMBA_SENSOR_IMX117_TYPE_2_3_MODE_3_30P  */ { IMX117_TYPE_2_3_MODE_3, { 72000000,  385, 3, 6240, 1, { .Interlace = 0, .TimeScale = 30000,  .NumUnitsInTick = 1001}}},
/*AMBA_SENSOR_IMX117_TYPE_2_3_MODE_0_2P   */ { IMX117_TYPE_2_3_MODE_0, { 72000000,  770, 1, 46800, 15, { .Interlace = 0, .TimeScale = 2000,  .NumUnitsInTick = 1001}}},

/*AMBA_SENSOR_IMX117_TYPE_2_5_MODE_0_30P  */ { IMX117_TYPE_2_5_MODE_0, { 72000000, 1056, 1, 2275, 1, { .Interlace = 0, .TimeScale = 30000,  .NumUnitsInTick = 1001}}},
/*AMBA_SENSOR_IMX117_TYPE_2_5_MODE_1_60P  */ { IMX117_TYPE_2_5_MODE_1, { 72000000,  546, 1, 2200, 1, { .Interlace = 0, .TimeScale = 60000,  .NumUnitsInTick = 1001}}},
/*AMBA_SENSOR_IMX117_TYPE_2_5_MODE_4_120P */ { IMX117_TYPE_2_5_MODE_4, { 72000000,  780, 1,  770, 1, { .Interlace = 0, .TimeScale = 120000, .NumUnitsInTick = 1001}}},

/*AMBA_SENSOR_IMX117_TYPE_4_4_MODE_1_120P */ { IMX117_TYPE_4_4_MODE_1, { 72000000,  440, 1, 1365, 1, { .Interlace = 0, .TimeScale = 120000, .NumUnitsInTick = 1001}}},
/*AMBA_SENSOR_IMX117_TYPE_4_4_MODE_1_60P  */ { IMX117_TYPE_4_4_MODE_1, { 72000000,  440, 1, 1365*2, 1, { .Interlace = 0, .TimeScale = 60000, .NumUnitsInTick = 1001}}},

/*AMBA_SENSOR_IMX117_TYPE_2_3_MODE_0_25P  */ { IMX117_TYPE_2_3_MODE_0, { 72000000,  720, 1, 4000, 1, { .Interlace = 0, .TimeScale = 25,     .NumUnitsInTick = 1}}},
/*AMBA_SENSOR_IMX117_TYPE_2_3_MODE_2A_50P */ {IMX117_TYPE_2_3_MODE_2A, { 72000000,  400, 2, 3600, 1, { .Interlace = 0, .TimeScale = 50,     .NumUnitsInTick = 1}}},
/*AMBA_SENSOR_IMX117_TYPE_2_3_MODE_4A_200P*/ {IMX117_TYPE_2_3_MODE_4A, { 72000000,  400, 1,  900, 1, { .Interlace = 0, .TimeScale = 200,    .NumUnitsInTick = 1}}},
/*AMBA_SENSOR_IMX117_TYPE_2_3_MODE_4_100P */ { IMX117_TYPE_2_3_MODE_4, { 72000000,  640, 1, 1125, 1, { .Interlace = 0, .TimeScale = 100,    .NumUnitsInTick = 1}}},
/*AMBA_SENSOR_IMX117_TYPE_2_3_MODE_3_50P  */ { IMX117_TYPE_2_3_MODE_3, { 72000000,  450, 3, 3200, 1, { .Interlace = 0, .TimeScale = 50,     .NumUnitsInTick = 1}}},
/*AMBA_SENSOR_IMX117_TYPE_2_3_MODE_5_200P */ { IMX117_TYPE_2_3_MODE_5, { 72000000,  500, 2,  720, 1, { .Interlace = 0, .TimeScale = 200,    .NumUnitsInTick = 1}}},
/*AMBA_SENSOR_IMX117_TYPE_2_3_MODE_7_400P */ { IMX117_TYPE_2_3_MODE_7, { 72000000,  450, 2,  400, 1, { .Interlace = 0, .TimeScale = 400,    .NumUnitsInTick = 1}}},

/*AMBA_SENSOR_IMX117_TYPE_2_5_MODE_0_25P  */ { IMX117_TYPE_2_5_MODE_0, { 72000000, 1280, 1, 2250, 1, { .Interlace = 0, .TimeScale = 25,     .NumUnitsInTick = 1}}},
/*AMBA_SENSOR_IMX117_TYPE_2_5_MODE_1_50P  */ { IMX117_TYPE_2_5_MODE_1, { 72000000,  640, 1, 2250, 1, { .Interlace = 0, .TimeScale = 50,     .NumUnitsInTick = 1}}},
/*AMBA_SENSOR_IMX117_TYPE_2_5_MODE_4_100P */ { IMX117_TYPE_2_5_MODE_4, { 72000000,  900, 1,  800, 1, { .Interlace = 0, .TimeScale = 100,    .NumUnitsInTick = 1}}},

/*AMBA_SENSOR_IMX117_TYPE_4_4_MODE_1_100P */ { IMX117_TYPE_4_4_MODE_1, { 72000000,  500, 1, 1440, 1, { .Interlace = 0, .TimeScale = 100,    .NumUnitsInTick = 1}}},
/*AMBA_SENSOR_IMX117_TYPE_2_5_MODE_1_24P  */ { IMX117_TYPE_2_5_MODE_1, { 72000000,  546, 1, 5500, 1, { .Interlace = 0, .TimeScale = 24000,  .NumUnitsInTick = 1001}}},

/*AMBA_SENSOR_IMX117_CUSTOM_MODE_60P      */ {     IMX117_CUSTOM_MODE, { 72000000,  600, 1, 2002, 1, { .Interlace = 0, .TimeScale = 60000,  .NumUnitsInTick = 1001}}},

/*AMBA_SENSOR_IMX117_TYPE_2_5_MODE_1_20P  */ { IMX117_TYPE_2_5_MODE_1, { 72000000,  546, 1, 6600, 1, { .Interlace = 0, .TimeScale = 20000,  .NumUnitsInTick = 1001}}},

/*AMBA_SENSOR_IMX117_TYPE_2_3_MODE_0_24P  */ { IMX117_TYPE_2_3_MODE_0, { 72000000,  660, 1, 4550, 1, { .Interlace = 0, .TimeScale = 24000,  .NumUnitsInTick = 1001}}},
/*AMBA_SENSOR_IMX117_TYPE_2_3_MODE_10_120P*/ {IMX117_TYPE_2_3_MODE_10, { 72000000,  350, 1, 1716, 1, { .Interlace = 0, .TimeScale = 120000, .NumUnitsInTick = 1001}}},
/*AMBA_SENSOR_IMX117_TYPE_2_3_MODE_10_100P*/ {IMX117_TYPE_2_3_MODE_10, { 72000000,  400, 1, 1800, 1, { .Interlace = 0, .TimeScale = 100,    .NumUnitsInTick = 1}}},

/*AMBA_SENSOR_IMX117_TYPE_2_3_MODE_2A_30P */ {IMX117_TYPE_2_3_MODE_2A, { 72000000,  528, 2, 4550, 1, { .Interlace = 0, .TimeScale = 30000,  .NumUnitsInTick = 1001}}},
/*AMBA_SENSOR_IMX117_TYPE_2_3_MODE_2A_25P */ {IMX117_TYPE_2_3_MODE_2A, { 72000000,  400, 2, 7200, 1, { .Interlace = 0, .TimeScale =    25,  .NumUnitsInTick = 1}}},

};
