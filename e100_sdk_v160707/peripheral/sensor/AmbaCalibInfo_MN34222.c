/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaCalibInfo_MN34222.c
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
#include "AmbaSensor_MN34222.h"

const UINT32 MN34222_AmbaBadPixelModeTable[] = {
    [0] = AMBA_SENSOR_MN34222_V1_12_1944X1092_60P,  /* full view */
};

const UINT32 MN34222_AmbaBadPixelModeAssignment[] = {
    [AMBA_SENSOR_MN34222_V1_12_1944X1092_60P] = 0,
    [AMBA_SENSOR_MN34222_V1_12_1944X1092_60P_TO_30P] = 0,
    [AMBA_SENSOR_MN34222_V1_12_1944X1092_30P] = 0,
    [AMBA_SENSOR_MN34222_V1_12_1944X1092_60P_HDR] = 0,
    [AMBA_SENSOR_MN34222_V1_12_1944X1092_30P_HDR_V_1500] = 0,
    [AMBA_SENSOR_MN34222_V1_12_1944X1092_30P_HDR_V_1250] = 0,
};

AMBA_CALIB_INFO_OBJ_s AmbaCalibInfo_MN34222Obj = {
    .AmbaVignetteMode           = AMBA_SENSOR_MN34222_V1_12_1944X1092_60P,
    .AmbaNumBadPixelMode        = GetArraySize(MN34222_AmbaBadPixelModeTable),
    .AmbaBadPixelModeTable      = MN34222_AmbaBadPixelModeTable,
    .AmbaBadPixelModeAssignment = MN34222_AmbaBadPixelModeAssignment,
    .AmbaCalibWidth             = 1944,
    .AmbaCalibHeight            = 1092,
};
