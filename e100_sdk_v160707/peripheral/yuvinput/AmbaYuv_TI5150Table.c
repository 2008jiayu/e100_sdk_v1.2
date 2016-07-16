/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaSensor_TI5150Table.c
 *
 *  @Copyright      :: Copyright (C) 2013 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Control APIs of OmniVision TI5150 CMOS sensor with MIPI interface
\*-------------------------------------------------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "AmbaDataType.h"
#include "AmbaDSP.h"
#include "AmbaYuv.h"
#include "AmbaYuv_TI5150.h"

/*-----------------------------------------------------------------------------------------------*\
 * Sensor register settings of each yuv modes
\*-----------------------------------------------------------------------------------------------*/
TI5150_REG_s TI5150RegTable[TI5150_NUM_MODE_REG] = {
  /* Addr    0        1   */
    {0x0F, {0x02, 0x02}},
    {0x00, {0x00, 0x00}},
    {0x03, {0x6f, 0x6f}},
    {0x0D, {0x47, 0x47}},
    {0x15, {0x25, 0x25}},
    {0x28, {0x00, 0x00}},
    {0x04, {0x00, 0x00}}
};

const TI5150_MODE_INFO_s TI5150ModeInfo[TI5150_NUM_YUV_MODE] = {
/* TI5150_720_480_60I */ {27000000, {1716, 525, {0, 12, 720, 240}, AMBA_DSP_CB_Y0_CR_Y1, {.Interlace = 1, .TimeScale =  60000, .NumUnitsInTick = 1001}}},
/* TI5150_720_576_50I */ {27000000, {1728, 625, {0, 16, 720, 288}, AMBA_DSP_CB_Y0_CR_Y1, {.Interlace = 1, .TimeScale =  50, .NumUnitsInTick = 1}}}
};
