/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaCalibInfo_IMX117.c
 *
 *  @Copyright      :: Copyright (C) 2013 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Calibration Information Managements
\*-------------------------------------------------------------------------------------------------------------------*/
#include "AmbaCalibInfo.h"
#include "AmbaSensor.h"
#include "AmbaSensor_OV10823.h"

const UINT32 OV10823_AmbaBadPixelModeTable[] = {
    [0] = AMBA_SENSOR_OV10823_V1_10_4320_2430_30P,  /* full view */
    [1] = AMBA_SENSOR_OV10823_V1_10_2160_1215_30P,
};

const UINT32 OV10823_AmbaBadPixelModeAssignment[] = {
    [AMBA_SENSOR_OV10823_V1_10_4320_2430_30P]    = 0,
    [AMBA_SENSOR_OV10823_V1_10_2160_1215_30P]    = 1,
    [AMBA_SENSOR_OV10823_V1_10_2160_1215_60P]    = 1,
};

AMBA_CALIB_INFO_OBJ_s AmbaCalibInfo_OV10823Obj = {
    .AmbaVignetteMode           = AMBA_SENSOR_OV10823_V1_10_4320_2430_30P,
    .AmbaNumBadPixelMode        = GetArraySize(OV10823_AmbaBadPixelModeTable),
    .AmbaBadPixelModeTable      = OV10823_AmbaBadPixelModeTable,
    .AmbaBadPixelModeAssignment = OV10823_AmbaBadPixelModeAssignment,
    .AmbaCalibWidth             = 4320,
    .AmbaCalibHeight            = 2430,
};
