/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaCalibInfo_MN34120.c
 *
 *  @Copyright      :: Copyright (C) 2013 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Camera Information Managements
\*-------------------------------------------------------------------------------------------------------------------*/
#include "AmbaCalibInfo.h"
#include "AmbaSensor.h"
#include "AmbaSensor_MN34120.h"

const UINT32 MN34120_AmbaBadPixelModeTable[] = {
    [0] = AMBA_SENSOR_MN34120_MODE_1_16P,    /* full view 12-bit   */
    [1] = AMBA_SENSOR_MN34120_MODE_3_30P,    /* 2x2 binning 12-bit */
    [2] = AMBA_SENSOR_MN34120_MODE_12_30P,   /* full view 10-bit   */
    [3] = AMBA_SENSOR_MN34120_MODE_23_120P,  /* 3x3 binning 10-bit */
};

const UINT32 MN34120_AmbaBadPixelModeAssignment[] = {
    [AMBA_SENSOR_MN34120_MODE_1_16P]    = 0,
    [AMBA_SENSOR_MN34120_MODE_3_30P]    = 1,
    [AMBA_SENSOR_MN34120_MODE_4_60P]    = 1,
    [AMBA_SENSOR_MN34120_MODE_4_30P]    = 1,
    [AMBA_SENSOR_MN34120_MODE_12_30P]   = 2,
    [AMBA_SENSOR_MN34120_MODE_23_120P]  = 3,

};

AMBA_CALIB_INFO_OBJ_s AmbaCalibInfo_MN34120Obj = {
    .AmbaVignetteMode           = AMBA_SENSOR_MN34120_MODE_1_16P,
    .AmbaNumBadPixelMode        = GetArraySize(MN34120_AmbaBadPixelModeTable),
    .AmbaBadPixelModeTable      = MN34120_AmbaBadPixelModeTable,
    .AmbaBadPixelModeAssignment = MN34120_AmbaBadPixelModeAssignment,
    .AmbaCalibWidth             = 4632,
    .AmbaCalibHeight            = 3488,
};
