/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaCalibInfo_OV9750.c
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
#include "AmbaSensor_OV9750.h"

const UINT32 OV9750_AmbaBadPixelModeTable[] = {
    [0] = AMBA_SENSOR_OV9750_V1_10_1280_960_60P_L1,  /* full view */
};

const UINT32 OV9750_AmbaBadPixelModeAssignment[] = {
    [AMBA_SENSOR_OV9750_V1_10_1280_960_60P_L1]     = 0,
    [AMBA_SENSOR_OV9750_V1_10_1280_960_60P_L2]     = 0,
    [AMBA_SENSOR_OV9750_V1_10_1280_960_50P_L1]     = 0,
    [AMBA_SENSOR_OV9750_V1_10_1280_960_50P_L2]     = 0,
    [AMBA_SENSOR_OV9750_V1_10_1280_960_60P_30P_L1] = 0,
    [AMBA_SENSOR_OV9750_V1_10_1280_960_60P_30P_L2] = 0,
    [AMBA_SENSOR_OV9750_V1_10_1280_960_50P_25P_L1] = 0,
    [AMBA_SENSOR_OV9750_V1_10_1280_960_50P_25P_L2] = 0,
    [AMBA_SENSOR_OV9750_V1_10_1280_960_30P_L2] = 0,
    [AMBA_SENSOR_OV9750_V1_10_1280_960_25P_L2] = 0,
};

AMBA_CALIB_INFO_OBJ_s AmbaCalibInfo_OV9750Obj = {
    .AmbaVignetteMode           = AMBA_SENSOR_OV9750_V1_10_1280_960_60P_L1,
    .AmbaNumBadPixelMode        = GetArraySize(OV9750_AmbaBadPixelModeTable),
    .AmbaBadPixelModeTable      = OV9750_AmbaBadPixelModeTable,
    .AmbaBadPixelModeAssignment = OV9750_AmbaBadPixelModeAssignment,
    .AmbaCalibWidth             = 1280,
    .AmbaCalibHeight            = 960,
};
