/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaCalibInfo_B5_AR0230.c
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
#include "AmbaSensor_B5_AR0230.h"

const UINT32 B5_AR0230_AmbaBadPixelModeTable[] = {
    [0] = AMBA_SENSOR_B5_AR0230_12_1920_1080_30P,  /* full view */
};

const UINT32 B5_AR0230_AmbaBadPixelModeAssignment[] = {
    [AMBA_SENSOR_B5_AR0230_12_1920_1080_30P]               = 0,
    [AMBA_SENSOR_B5_AR0230_12_1920_1080_30P_Interleave]    = 0,
    [AMBA_SENSOR_B5_AR0230_12_1920_1080_60P_HDR_14BIT]     = 0,
    [AMBA_SENSOR_B5_AR0230_12_1920_1080_30P_HDR_14BIT]     = 0,
    [AMBA_SENSOR_B5_AR0230_12_1920_1080_60P_HDR_12BIT]     = 0,
    [AMBA_SENSOR_B5_AR0230_12_1920_1080_30P_HDR_12BIT]     = 0,
    
};

AMBA_CALIB_INFO_OBJ_s AmbaCalibInfo_B5_AR0230Obj = {
    .AmbaVignetteMode           = AMBA_SENSOR_B5_AR0230_12_1920_1080_30P,
    .AmbaNumBadPixelMode        = GetArraySize(B5_AR0230_AmbaBadPixelModeTable),
    .AmbaBadPixelModeTable      = B5_AR0230_AmbaBadPixelModeTable,
    .AmbaBadPixelModeAssignment = B5_AR0230_AmbaBadPixelModeAssignment,
    .AmbaCalibWidth             = 1928,
    .AmbaCalibHeight            = 1080,
    
};
