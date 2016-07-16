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
#include "AmbaSensor_OV4689.h"

const UINT32 OV4689_AmbaBadPixelModeTable[] = {
    [0] = AMBA_SENSOR_OV4689_V1_10_2688_1512_60P,  /* Full view */
    [1] = AMBA_SENSOR_OV4689_V2_10_1344_760_30P    /* 2x binning */
};

const UINT32 OV4689_AmbaBadPixelModeAssignment[] = {
    [AMBA_SENSOR_OV4689_V1_10_2688_1512_60P]       = 0,
    [AMBA_SENSOR_OV4689_V1_10_2688_1512_60P]       = 0,
    [AMBA_SENSOR_OV4689_V1_10_2688_1512_45P]       = 0,
    [AMBA_SENSOR_OV4689_V1_10_2688_1512_40P]       = 0,
    [AMBA_SENSOR_OV4689_V1_10_2688_1512_30P]       = 0,
    [AMBA_SENSOR_OV4689_V1_10_2688_1512_60P_30P]   = 0,
    [AMBA_SENSOR_OV4689_V1_10_2688_1512_50P]       = 0,
    [AMBA_SENSOR_OV4689_V1_10_2688_1512_25P]       = 0,
    [AMBA_SENSOR_OV4689_V1_10_2688_1512_50P_25P]   = 0,
    [AMBA_SENSOR_OV4689_V2_10_1344_760_30P]        = 1,
    [AMBA_SENSOR_OV4689_V2_10_1344_760_25P]        = 1,
    [AMBA_SENSOR_OV4689_V1_10_2688_1512_30P_HDR]   = 0,
    [AMBA_SENSOR_OV4689_V1_10_2688_1512_25P_HDR]   = 0,
    [AMBA_SENSOR_OV4689_V1_10_2112_1188_CROP_120P] = 0,
    [AMBA_SENSOR_OV4689_V1_10_2112_1188_CROP_100P] = 0,
    [AMBA_SENSOR_OV4689_V2_10_1344_760_120P]       = 1,
    [AMBA_SENSOR_OV4689_V2_10_1344_760_100P]       = 1,
};

AMBA_CALIB_INFO_OBJ_s AmbaCalibInfo_OV4689Obj = {
    .AmbaVignetteMode           = AMBA_SENSOR_OV4689_V1_10_2688_1512_60P,
    .AmbaNumBadPixelMode        = GetArraySize(OV4689_AmbaBadPixelModeTable),
    .AmbaBadPixelModeTable      = OV4689_AmbaBadPixelModeTable,
    .AmbaBadPixelModeAssignment = OV4689_AmbaBadPixelModeAssignment,
    .AmbaCalibWidth             = 2688,
    .AmbaCalibHeight            = 1512,
};
