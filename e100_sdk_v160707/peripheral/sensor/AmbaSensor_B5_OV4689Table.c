/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaSensor_B5_OV4689Table.c
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
#include "AmbaSensor_B5_OV4689.h"

/*-----------------------------------------------------------------------------------------------*\
 * Sensor Device Information
\*-----------------------------------------------------------------------------------------------*/
const AMBA_SENSOR_DEVICE_INFO_s B5_OV4689DeviceInfo = {
    .UnitCellWidth          = 2,
    .UnitCellHeight         = 2,
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
        .FirstBadFrame          = 1,
        .NumBadFrames           = 1
    },
    .ShutterSpeedCtrlInfo = {
        .CommunicationTime      = AMBA_SENSOR_COMMUNICATION_AT_NON_VBLANK,
        .FirstReflectedFrame    = 2,
        .FirstBadFrame          = 1,
        .NumBadFrames           = 1
    },
    .AnalogGainCtrlInfo = {
        .CommunicationTime      = AMBA_SENSOR_COMMUNICATION_AT_NON_VBLANK,
        .FirstReflectedFrame    = 1,
        .FirstBadFrame          = 1,
        .NumBadFrames           = 1
    },
    .DigitalGainCtrlInfo = {
        .CommunicationTime      = AMBA_SENSOR_COMMUNICATION_AT_NON_VBLANK,
        .FirstReflectedFrame    = 1,
        .FirstBadFrame          = 1,
        .NumBadFrames           = 1
    },
};

/*-----------------------------------------------------------------------------------------------*\
 * Sensor register settings of each readout modes
\*-----------------------------------------------------------------------------------------------*/
B5_OV4689_REG_s B5_OV4689RegTable[B5_OV4689_NUM_READOUT_MODE_REG] = {
    {0x0300, {0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00}},
    {0x0302, {0x1e, 0x1e, 0x1e, 0x23, 0x1e, 0x2d, 0x1e, 0x23, 0x1e, 0x2d, 0x23, 0x23}},
    {0x0303, {0x00, 0x00, 0x00, 0x01, 0x00, 0x02, 0x00, 0x01, 0x00, 0x02, 0x00, 0x00}},
    {0x3018, {0x72, 0x72, 0x72, 0x72, 0x72, 0x32, 0x72, 0x72, 0x72, 0x32, 0x72, 0x72}},
    {0x3019, {0x00, 0x00, 0x00, 0x00, 0x00, 0x0c, 0x00, 0x00, 0x00, 0x0c, 0x00, 0x00}},
    {0x3501, {0x60, 0x60, 0x60, 0x60, 0x60, 0x31, 0x60, 0x60, 0x60, 0x31, 0x60, 0x60}},
    {0x3632, {0x00, 0x00, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00}},
    {0x374a, {0x43, 0x43, 0x43, 0x43, 0x43, 0x43, 0x43, 0x43, 0x43, 0x43, 0x43, 0x43}},
    {0x376b, {0x20, 0x20, 0x20, 0x20, 0x20, 0x40, 0x20, 0x20, 0x20, 0x40, 0x20, 0x20}},
    {0x3800, {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}},
    {0x3801, {0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08}},
    {0x3802, {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}},
    {0x3803, {0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04}},
    {0x3804, {0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a}},
    {0x3805, {0x97, 0x97, 0x97, 0x97, 0x97, 0x97, 0x97, 0x97, 0x97, 0x97, 0x97, 0x97}},
    {0x3806, {0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05}},
    {0x3807, {0xfb, 0xfb, 0xfb, 0xfb, 0xfb, 0xfb, 0xfb, 0xfb, 0xfb, 0xfb, 0xfb, 0xfb}},
    {0x3808, {0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x05, 0x0a, 0x0a, 0x0a, 0x05, 0x0a, 0x0a}},
    {0x3809, {0x80, 0x80, 0x80, 0x80, 0x80, 0x40, 0x80, 0x80, 0x80, 0x40, 0x80, 0x80}},
    {0x380a, {0x05, 0x05, 0x05, 0x05, 0x05, 0x02, 0x05, 0x05, 0x05, 0x02, 0x05, 0x05}},
    {0x380b, {0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf8, 0xf0, 0xf0, 0xf0, 0xf8, 0xf0, 0xf0}},
    {0x380c, {0x04, 0x04, 0x04, 0x09, 0x04, 0x13, 0x04, 0x09, 0x04, 0x13, 0x04, 0x04}},
    {0x380d, {0xb0, 0xb0, 0xb0, 0x60, 0xb0, 0x98, 0xb0, 0x60, 0xb0, 0x98, 0x12, 0x12}},
    {0x380e, {0x06, 0x08, 0x09, 0x06, 0x0d, 0x03, 0x07, 0x07, 0x0f, 0x03, 0x07, 0x08}},
    {0x380f, {0x86, 0xb2, 0xC8, 0x84, 0x0c, 0x1d, 0xd4, 0xd2, 0xa8, 0xbd, 0x80, 0xfe}},
    {0x3810, {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}},
    {0x3811, {0x08, 0x08, 0x08, 0x08, 0x08, 0x04, 0x08, 0x08, 0x08, 0x04, 0x08, 0x08}},
    {0x3812, {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}},
    {0x3813, {0x04, 0x04, 0x04, 0x04, 0x04, 0x02, 0x04, 0x04, 0x04, 0x02, 0x04, 0x04}},
    {0x3814, {0x01, 0x01, 0x01, 0x01, 0x01, 0x03, 0x01, 0x01, 0x01, 0x03, 0x01, 0x01}},
    {0x3815, {0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01}},
    {0x3819, {0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01}},
    {0x3820, {0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00}},
    {0x3821, {0x06, 0x06, 0x06, 0x06, 0x06, 0x07, 0x06, 0x06, 0x06, 0x07, 0x06, 0x06}},
    {0x3829, {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}},
    {0x382a, {0x01, 0x01, 0x01, 0x01, 0x01, 0x03, 0x01, 0x01, 0x01, 0x03, 0x01, 0x01}},
    {0x3830, {0x04, 0x04, 0x04, 0x04, 0x04, 0x08, 0x04, 0x04, 0x04, 0x08, 0x04, 0x04}},
    {0x3836, {0x01, 0x01, 0x01, 0x01, 0x01, 0x02, 0x01, 0x01, 0x01, 0x02, 0x01, 0x01}},
    {0x4001, {0x40, 0x40, 0x40, 0x40, 0x40, 0x50, 0x40, 0x40, 0x40, 0x50, 0x40, 0x40}},
    {0x4003, {0x14, 0x14, 0x14, 0x14, 0x14, 0x14, 0x14, 0x14, 0x14, 0x14, 0x14, 0x14}},
    {0x4022, {0x07, 0x07, 0x07, 0x07, 0x07, 0x03, 0x07, 0x07, 0x07, 0x03, 0x07, 0x07}},
    {0x4023, {0xcf, 0xcf, 0xcf, 0xcf, 0xcf, 0xe7, 0xcf, 0xcf, 0xcf, 0xe7, 0xcf, 0xcf}},
    {0x4024, {0x09, 0x09, 0x09, 0x09, 0x09, 0x05, 0x09, 0x09, 0x09, 0x05, 0x09, 0x09}},
    {0x4025, {0x60, 0x60, 0x60, 0x60, 0x60, 0x14, 0x60, 0x60, 0x60, 0x14, 0x60, 0x60}},
    {0x4026, {0x09, 0x09, 0x09, 0x09, 0x09, 0x05, 0x09, 0x09, 0x09, 0x05, 0x09, 0x09}},
    {0x4027, {0x6f, 0x6f, 0x6f, 0x6f, 0x6f, 0x23, 0x6f, 0x6f, 0x6f, 0x23, 0x6f, 0x6f}},
    {0x402a, {0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06}},
    {0x402b, {0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04}},
    {0x402e, {0x0e, 0x0e, 0x0e, 0x0e, 0x0e, 0x0e, 0x0e, 0x0e, 0x0e, 0x0e, 0x0e, 0x0e}},
    {0x402f, {0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04}},
    {0x4502, {0x40, 0x40, 0x40, 0x40, 0x40, 0x44, 0x40, 0x40, 0x40, 0x44, 0x40, 0x40}},
    {0x4601, {0x04, 0x04, 0x04, 0x04, 0x04, 0x53, 0x04, 0x04, 0x04, 0x53, 0x04, 0x04}},
    {0x4837, {0x16, 0x16, 0x16, 0x26, 0x16, 0x43, 0x16, 0x26, 0x16, 0x43, 0x10, 0x10}},
    {0x5050, {0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x3c, 0x0c, 0x0c, 0x0c, 0x3c, 0x0c, 0x0c}},
};

const AMBA_SENSOR_INPUT_INFO_s B5_OV4689InputInfo[] = {
    {{ 0, 0, 2688, 1520}, { AMBA_SENSOR_SUBSAMPLE_TYPE_NORMAL, 1, 1}, { AMBA_SENSOR_SUBSAMPLE_TYPE_NORMAL, 1, 1}},  /* Input Mode 0     */
    {{ 0, 0, 1344,  760}, {AMBA_SENSOR_SUBSAMPLE_TYPE_BINNING, 1, 2}, {AMBA_SENSOR_SUBSAMPLE_TYPE_BINNING, 1, 2}},  /* Input Mode 1     */
    {{ 0, 0, 2688, 3640}, { AMBA_SENSOR_SUBSAMPLE_TYPE_NORMAL, 1, 1}, { AMBA_SENSOR_SUBSAMPLE_TYPE_NORMAL, 1, 1}},  /* Input Mode 2 HDR */
};

const AMBA_SENSOR_OUTPUT_INFO_s B5_OV4689OutputInfo[] = {
    {720000000, 4, 10, AMBA_DSP_BAYER_BG, {AMBA_DSP_PHASE_SHIFT_MODE_0, AMBA_DSP_PHASE_SHIFT_MODE_0}, 2688, 1520, {   0,   4, 2688, 1512}, {0}},  /* Output Mode 0     */
    {720000000, 4, 10, AMBA_DSP_BAYER_BG, {AMBA_DSP_PHASE_SHIFT_MODE_0, AMBA_DSP_PHASE_SHIFT_MODE_0}, 2688, 1520, {   0, 184, 2688, 1152}, {0}},  /* Output Mode 1     */
    {720000000, 4, 10, AMBA_DSP_BAYER_BG, {AMBA_DSP_PHASE_SHIFT_MODE_0, AMBA_DSP_PHASE_SHIFT_MODE_0}, 2688, 1520, { 336,   4, 2016, 1512}, {0}},  /* Output Mode 2     */
    {420000000, 4, 10, AMBA_DSP_BAYER_BG, {AMBA_DSP_PHASE_SHIFT_MODE_0, AMBA_DSP_PHASE_SHIFT_MODE_0}, 2688, 1520, {   0,   4, 2688, 1512}, {0}},  /* Output Mode 3     */
    {420000000, 4, 10, AMBA_DSP_BAYER_BG, {AMBA_DSP_PHASE_SHIFT_MODE_0, AMBA_DSP_PHASE_SHIFT_MODE_0}, 2688, 1520, {   0, 184, 2688, 1152}, {0}},  /* Output Mode 4     */
    {420000000, 4, 10, AMBA_DSP_BAYER_BG, {AMBA_DSP_PHASE_SHIFT_MODE_0, AMBA_DSP_PHASE_SHIFT_MODE_0}, 2688, 1520, { 336,   4, 2016, 1512}, {0}},  /* Output Mode 5     */
    {240000000, 2, 10, AMBA_DSP_BAYER_BG, {AMBA_DSP_PHASE_SHIFT_MODE_0, AMBA_DSP_PHASE_SHIFT_MODE_0}, 1344,  760, {   0,   0, 1344,  760}, {0}},  /* Output Mode 6     */
    {840000000, 4, 10, AMBA_DSP_BAYER_BG, {AMBA_DSP_PHASE_SHIFT_MODE_0, AMBA_DSP_PHASE_SHIFT_MODE_0}, 2688, 3640, {   0,   0, 2688, 3640}, {0}},  /* Output Mode 7 HDR */
};

const B5_OV4689_MODE_INFO_s B5_OV4689ModeInfoList[AMBA_SENSOR_B5_OV4689_NUM_MODE] = {
    {    B5_OV4689_S1_10_2688_1520_60P,     B5_OV4689_INPUT_MODE_0,     B5_OV4689_OUTPUT_MODE_0,  {24023976, 2875/*1200 * 4*/,     1670,  { .Interlace = 0, .TimeScale = 60000, .NumUnitsInTick = 1001 }},            {0}, 2688},  /* Mode  0 */
    {    B5_OV4689_S1_10_2688_1520_45P,     B5_OV4689_INPUT_MODE_0,     B5_OV4689_OUTPUT_MODE_0,  {24040800, 2875/*1200 * 4*/,     2226,  { .Interlace = 0, .TimeScale =    45, .NumUnitsInTick =    1 }},            {0}, 2688},  /* Mode  1 */
    {    B5_OV4689_S1_10_2688_1520_40P,     B5_OV4689_INPUT_MODE_0,     B5_OV4689_OUTPUT_MODE_0,  {24038400, 2875/*1200 * 4*/,     2504,  { .Interlace = 0, .TimeScale =    40, .NumUnitsInTick =    1 }},            {0}, 2688},  /* Mode  2 */
    {    B5_OV4689_S1_10_2688_1520_30P,     B5_OV4689_INPUT_MODE_0,     B5_OV4689_OUTPUT_MODE_3,  {23995205, 3360/*2400 * 4*/,     1668,  { .Interlace = 0, .TimeScale = 30000, .NumUnitsInTick = 1001 }},            {0}, 2688},  /* Mode  3 */
    {B5_OV4689_S1_10_2688_1520_60P_30P,     B5_OV4689_INPUT_MODE_0,     B5_OV4689_OUTPUT_MODE_0,  {24023976, 2875/*1200 * 4*/,     3340,  { .Interlace = 0, .TimeScale = 30000, .NumUnitsInTick = 1001 }},            {0}, 2688},  /* Mode  4 */
    {    B5_OV4689_S1_10_2688_1520_60P,     B5_OV4689_INPUT_MODE_0,     B5_OV4689_OUTPUT_MODE_1,  {24023976, 2875/*1200 * 4*/,     1670,  { .Interlace = 0, .TimeScale = 60000, .NumUnitsInTick = 1001 }},            {0}, 2688},  /* Mode  5 */
    {    B5_OV4689_S1_10_2688_1520_45P,     B5_OV4689_INPUT_MODE_0,     B5_OV4689_OUTPUT_MODE_1,  {24040800, 2875/*1200 * 4*/,     2226,  { .Interlace = 0, .TimeScale =    45, .NumUnitsInTick =    1 }},            {0}, 2688},  /* Mode  6 */
    {    B5_OV4689_S1_10_2688_1520_40P,     B5_OV4689_INPUT_MODE_0,     B5_OV4689_OUTPUT_MODE_1,  {24038400, 2875/*1200 * 4*/,     2504,  { .Interlace = 0, .TimeScale =    40, .NumUnitsInTick =    1 }},            {0}, 2688},  /* Mode  7 */
    {    B5_OV4689_S1_10_2688_1520_30P,     B5_OV4689_INPUT_MODE_0,     B5_OV4689_OUTPUT_MODE_4,  {23995205, 3360/*2400 * 4*/,     1668,  { .Interlace = 0, .TimeScale = 30000, .NumUnitsInTick = 1001 }},            {0}, 2688},  /* Mode  8 */
    {B5_OV4689_S1_10_2688_1520_60P_30P,     B5_OV4689_INPUT_MODE_0,     B5_OV4689_OUTPUT_MODE_1,  {24023976, 2875/*1200 * 4*/,     3340,  { .Interlace = 0, .TimeScale = 30000, .NumUnitsInTick = 1001 }},            {0}, 2688},  /* Mode  9 */
    {    B5_OV4689_S1_10_2688_1520_60P,     B5_OV4689_INPUT_MODE_0,     B5_OV4689_OUTPUT_MODE_2,  {24023976, 2875/*1200 * 4*/,     1670,  { .Interlace = 0, .TimeScale = 60000, .NumUnitsInTick = 1001 }},            {0}, 2016},  /* Mode 10 */
    {    B5_OV4689_S1_10_2688_1520_45P,     B5_OV4689_INPUT_MODE_0,     B5_OV4689_OUTPUT_MODE_2,  {24040800, 2875/*1200 * 4*/,     2226,  { .Interlace = 0, .TimeScale =    45, .NumUnitsInTick =    1 }},            {0}, 2016},  /* Mode 11 */
    {    B5_OV4689_S1_10_2688_1520_40P,     B5_OV4689_INPUT_MODE_0,     B5_OV4689_OUTPUT_MODE_2,  {24038400, 2875/*1200 * 4*/,     2504,  { .Interlace = 0, .TimeScale =    40, .NumUnitsInTick =    1 }},            {0}, 2016},  /* Mode 12 */
    {    B5_OV4689_S1_10_2688_1520_30P,     B5_OV4689_INPUT_MODE_0,     B5_OV4689_OUTPUT_MODE_5,  {23995205, 3360/*2400 * 4*/,     1668,  { .Interlace = 0, .TimeScale = 30000, .NumUnitsInTick = 1001 }},            {0}, 2016},  /* Mode 13 */
    {B5_OV4689_S1_10_2688_1520_60P_30P,     B5_OV4689_INPUT_MODE_0,     B5_OV4689_OUTPUT_MODE_2,  {24023976, 2875/*1200 * 4*/,     3340,  { .Interlace = 0, .TimeScale = 30000, .NumUnitsInTick = 1001 }},            {0}, 2016},  /* Mode 14 */
    {    B5_OV4689_S1_10_2688_1520_50P,     B5_OV4689_INPUT_MODE_0,     B5_OV4689_OUTPUT_MODE_0,  {24048000, 2875/*1200 * 4*/,     2004,  { .Interlace = 0, .TimeScale =    50, .NumUnitsInTick =    1 }},            {0}, 2688},  /* Mode 15 */
    {    B5_OV4689_S1_10_2688_1520_25P,     B5_OV4689_INPUT_MODE_0,     B5_OV4689_OUTPUT_MODE_3,  {24024000, 3360/*2400 * 4*/,     2002,  { .Interlace = 0, .TimeScale =    25, .NumUnitsInTick =    1 }},            {0}, 2688},  /* Mode 16 */
    {B5_OV4689_S1_10_2688_1520_50P_25P,     B5_OV4689_INPUT_MODE_0,     B5_OV4689_OUTPUT_MODE_0,  {24048000, 2875/*1200 * 4*/,     4008,  { .Interlace = 0, .TimeScale =    25, .NumUnitsInTick =    1 }},            {0}, 2688},  /* Mode 17 */
    {    B5_OV4689_S1_10_2688_1520_50P,     B5_OV4689_INPUT_MODE_0,     B5_OV4689_OUTPUT_MODE_1,  {24048000, 2875/*1200 * 4*/,     2004,  { .Interlace = 0, .TimeScale =    50, .NumUnitsInTick =    1 }},            {0}, 2688},  /* Mode 18 */
    {    B5_OV4689_S1_10_2688_1520_25P,     B5_OV4689_INPUT_MODE_0,     B5_OV4689_OUTPUT_MODE_4,  {24024000, 3360/*2400 * 4*/,     2002,  { .Interlace = 0, .TimeScale =    25, .NumUnitsInTick =    1 }},            {0}, 2688},  /* Mode 19 */
    {B5_OV4689_S1_10_2688_1520_50P_25P,     B5_OV4689_INPUT_MODE_0,     B5_OV4689_OUTPUT_MODE_1,  {24048000, 2875/*1200 * 4*/,     4008,  { .Interlace = 0, .TimeScale =    25, .NumUnitsInTick =    1 }},            {0}, 2688},  /* Mode 20 */
    {    B5_OV4689_S1_10_2688_1520_50P,     B5_OV4689_INPUT_MODE_0,     B5_OV4689_OUTPUT_MODE_2,  {24048000, 2875/*1200 * 4*/,     2004,  { .Interlace = 0, .TimeScale =    50, .NumUnitsInTick =    1 }},            {0}, 2016},  /* Mode 21 */
    {    B5_OV4689_S1_10_2688_1520_25P,     B5_OV4689_INPUT_MODE_0,     B5_OV4689_OUTPUT_MODE_5,  {24024000, 3360/*2400 * 4*/,     2002,  { .Interlace = 0, .TimeScale =    25, .NumUnitsInTick =    1 }},            {0}, 2016},  /* Mode 22 */
    {B5_OV4689_S1_10_2688_1520_50P_25P,     B5_OV4689_INPUT_MODE_0,     B5_OV4689_OUTPUT_MODE_2,  {24048000, 2875/*1200 * 4*/,     4008,  { .Interlace = 0, .TimeScale =    25, .NumUnitsInTick =    1 }},            {0}, 2016},  /* Mode 23 */
    {     B5_OV4689_S2_10_1344x760_30P,     B5_OV4689_INPUT_MODE_1,     B5_OV4689_OUTPUT_MODE_6,  {23962549, 2010/*5016 * 4*/,      797,  { .Interlace = 0, .TimeScale = 30000, .NumUnitsInTick = 1001 }},            {0}, 1344},  /* Mode 24 */
    {     B5_OV4689_S2_10_1344x760_25P,     B5_OV4689_INPUT_MODE_1,     B5_OV4689_OUTPUT_MODE_6,  {24001560, 2010/*5016 * 4*/,      957,  { .Interlace = 0, .TimeScale =    25, .NumUnitsInTick =    1 }},            {0}, 1344},  /* Mode 25 */
    {B5_OV4689_S1_10_2688_1520_HDR_30P, B5_OV4689_INPUT_MODE_2_HDR, B5_OV4689_OUTPUT_MODE_7_HDR,  {23983696, 2920/*1042 * 4*/, 1920 * 2,  { .Interlace = 0, .TimeScale = 30000, .NumUnitsInTick = 1001 }}, {1, 1616, 299}, 2688},  /* Mode 26 */
    {B5_OV4689_S1_10_2688_1520_HDR_25P, B5_OV4689_INPUT_MODE_2_HDR, B5_OV4689_OUTPUT_MODE_7_HDR,  {23986840, 2920/*1042 * 4*/, 2302 * 2,  { .Interlace = 0, .TimeScale =    25, .NumUnitsInTick =    1 }}, {1, 1998, 299}, 2688},  /* Mode 27 */
};

AMBA_SENSOR_OUTPUT_INFO_s B5_OV4689_B5nOutputInfo[] = {
    [0] = { .DataRate = 750000000 },
};
