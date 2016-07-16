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
#include "AmbaSensor_IMX206.h"

const UINT32 IMX206_AmbaBadPixelModeTable[] = {
        [0] = AMBA_SENSOR_IMX206_TYPE_2_3_MODE_0_10P,  /* full view */
        [1] = AMBA_SENSOR_IMX206_TYPE_2_3_MODE_2A_30P,  /* 2 x 2 binning */
        [2] = AMBA_SENSOR_IMX206_TYPE_2_3_MODE_4_240P,  /* 3 x 9 binning */
        [3] = AMBA_SENSOR_IMX206_TYPE_2_3_MODE_3_60P,   /* 3 x 3 binning */
};

const UINT32 IMX206_AmbaBadPixelModeAssignment[] = {
    [AMBA_SENSOR_IMX206_TYPE_2_3_MODE_2_60P] = 1,
    [AMBA_SENSOR_IMX206_TYPE_2_3_MODE_2A_30P] = 1,
    [AMBA_SENSOR_IMX206_TYPE_2_3_MODE_0_10P] = 0,
    [AMBA_SENSOR_IMX206_TYPE_2_3_MODE_4_240P] = 2,
    [AMBA_SENSOR_IMX206_TYPE_2_3_MODE_2_50P] = 1,
    [AMBA_SENSOR_IMX206_TYPE_2_3_MODE_2A_25P] = 1,
    [AMBA_SENSOR_IMX206_TYPE_2_3_MODE_4_200P] = 2,
    [AMBA_SENSOR_IMX206_TYPE_2_3_MODE_3B_120P] = 3,
    [AMBA_SENSOR_IMX206_TYPE_2_3_MODE_3B_100P] = 3,
    [AMBA_SENSOR_IMX206_TYPE_2_3_MODE_3_60P] = 3,
    [AMBA_SENSOR_IMX206_TYPE_2_3_MODE_3_50P] = 3,
    [AMBA_SENSOR_IMX206_TYPE_2_3_MODE_4_120P] = 2,
    [AMBA_SENSOR_IMX206_TYPE_2_3_MODE_4_100P] = 2,
    [AMBA_SENSOR_IMX206_TYPE_2_3_MODE_5_30P] = 2,
    [AMBA_SENSOR_IMX206_TYPE_2_3_MODE_5_25P] = 2,
};

AMBA_CALIB_INFO_OBJ_s AmbaCalibInfo_IMX206Obj = {
    .AmbaVignetteMode           = AMBA_SENSOR_IMX206_TYPE_2_3_MODE_0_10P,
    .AmbaNumBadPixelMode        = GetArraySize(IMX206_AmbaBadPixelModeTable),
    .AmbaBadPixelModeTable      = IMX206_AmbaBadPixelModeTable,
    .AmbaBadPixelModeAssignment = IMX206_AmbaBadPixelModeAssignment,
    .AmbaCalibWidth             = 4770,
    .AmbaCalibHeight            = 3564,
};
