/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaCalibInfo_AR0330_PARALLEL.c
 *
 *  @Copyright      :: Copyright (C) 2014 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
\*-------------------------------------------------------------------------------------------------------------------*/
#include "AmbaCalibInfo.h"
#include "AmbaSensor.h"
#include "AmbaSensor_AR0330_PARALLEL.h"

const UINT32 AR0330_PARALLEL_AmbaBadPixelModeTable[] = {
    [0] = AMBA_SENSOR_AR0330_PARALLEL_12_2304_1536_24P,  /* full view */
    [1] = AMBA_SENSOR_AR0330_PARALLEL_12_1152_768_30P,   /* 2 x 2 binning */
};

const UINT32 AR0330_PARALLEL_AmbaBadPixelModeAssignment[] = {
    [AMBA_SENSOR_AR0330_PARALLEL_12_2304_1536_24P]    = 0,
    [AMBA_SENSOR_AR0330_PARALLEL_12_2304_1296_30P]    = 0,
    [AMBA_SENSOR_AR0330_PARALLEL_12_1920_1080_30P]    = 0,
    [AMBA_SENSOR_AR0330_PARALLEL_12_1536_1536_30P]    = 0,
    [AMBA_SENSOR_AR0330_PARALLEL_12_1152_648_60P]     = 1,
    [AMBA_SENSOR_AR0330_PARALLEL_12_1152_768_30P]     = 1,
    
};

AMBA_CALIB_INFO_OBJ_s AmbaCalibInfo_AR0330_PARALLELObj = {
    .AmbaVignetteMode           = AMBA_SENSOR_AR0330_PARALLEL_12_2304_1536_24P,
    .AmbaNumBadPixelMode        = GetArraySize(AR0330_PARALLEL_AmbaBadPixelModeTable),
    .AmbaBadPixelModeTable      = AR0330_PARALLEL_AmbaBadPixelModeTable,
    .AmbaBadPixelModeAssignment = AR0330_PARALLEL_AmbaBadPixelModeAssignment,
    .AmbaCalibWidth             = 2304,
    .AmbaCalibHeight            = 1536,
};
