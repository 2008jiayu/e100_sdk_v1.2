/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaCalibInfo_IMX117.c
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
#include "AmbaSensor_IMX117.h"

const UINT32 IMX117_AmbaBadPixelModeTable[] = {
    [0] = AMBA_SENSOR_IMX117_TYPE_2_3_MODE_0A_35P,  /* full view */
    [1] = AMBA_SENSOR_IMX117_TYPE_2_3_MODE_2A_60P,  /* 2 x 2 binning */
    [2] = AMBA_SENSOR_IMX117_TYPE_2_3_MODE_4_120P,  /* 3 x 3 binning */
    [3] = AMBA_SENSOR_IMX117_TYPE_2_3_MODE_5_240P,  /* 3 x 9 binning */
    [4] = AMBA_SENSOR_IMX117_TYPE_2_3_MODE_7_480P,  /* 3 x 2/17 binning */
    [5] = AMBA_SENSOR_IMX117_TYPE_2_3_MODE_3_60P    /* 3 x 3 binning for mode3 */
};

const UINT32 IMX117_AmbaBadPixelModeAssignment[] = {
    [AMBA_SENSOR_IMX117_TYPE_2_3_MODE_0_30P]    = 0,
    [AMBA_SENSOR_IMX117_TYPE_2_3_MODE_0A_35P]   = 0,
    [AMBA_SENSOR_IMX117_TYPE_2_3_MODE_2A_60P]   = 1,
    [AMBA_SENSOR_IMX117_TYPE_2_3_MODE_4A_240P]  = 2,
    [AMBA_SENSOR_IMX117_TYPE_2_3_MODE_4_120P]   = 2,
    [AMBA_SENSOR_IMX117_TYPE_2_3_MODE_3_60P]    = 5,
    [AMBA_SENSOR_IMX117_TYPE_2_3_MODE_5_240P]   = 3,
    [AMBA_SENSOR_IMX117_TYPE_2_3_MODE_6_30P]    = 3,
    [AMBA_SENSOR_IMX117_TYPE_2_3_MODE_7_480P]   = 4,
    [AMBA_SENSOR_IMX117_TYPE_2_3_MODE_3_30P]    = 5,

    [AMBA_SENSOR_IMX117_TYPE_2_5_MODE_0_30P]    = 0,
    [AMBA_SENSOR_IMX117_TYPE_2_5_MODE_1_60P]    = 0,
    [AMBA_SENSOR_IMX117_TYPE_2_5_MODE_4_120P]   = 2,

    [AMBA_SENSOR_IMX117_TYPE_4_4_MODE_1_120P]   = 0
};

AMBA_CALIB_INFO_OBJ_s AmbaCalibInfo_IMX117Obj = {
    .AmbaVignetteMode           = AMBA_SENSOR_IMX117_TYPE_2_3_MODE_0A_35P,
    .AmbaNumBadPixelMode        = GetArraySize(IMX117_AmbaBadPixelModeTable),
    .AmbaBadPixelModeTable      = IMX117_AmbaBadPixelModeTable,
    .AmbaBadPixelModeAssignment = IMX117_AmbaBadPixelModeAssignment,
    .AmbaCalibWidth             = 4096,
    .AmbaCalibHeight            = 3000,
};
