/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaCalibInfo_B5_OV4689.c
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
#include "AmbaSensor_B5_OV4689.h"

const UINT32 B5_OV4689_AmbaBadPixelModeTable[] = {
    [0] = AMBA_SENSOR_B5_OV4689_V1_10_2688_1512_30P,  /* full view */
};

const UINT32 B5_OV4689_AmbaBadPixelModeAssignment[] = {
    [AMBA_SENSOR_B5_OV4689_V1_10_2688_1512_30P] = 0,
};

AMBA_CALIB_INFO_OBJ_s AmbaCalibInfo_B5_OV4689Obj = {
    .AmbaVignetteMode           = AMBA_SENSOR_B5_OV4689_V1_10_2688_1512_30P,
    .AmbaNumBadPixelMode        = GetArraySize(B5_OV4689_AmbaBadPixelModeTable),
    .AmbaBadPixelModeTable      = B5_OV4689_AmbaBadPixelModeTable,
    .AmbaBadPixelModeAssignment = B5_OV4689_AmbaBadPixelModeAssignment,
    .AmbaCalibWidth             = 2688,
    .AmbaCalibHeight            = 1512,
};
