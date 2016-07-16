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
#include "AmbaSensor_TI913914_OV2710.h"

const UINT32 TI913914_OV2710_AmbaBadPixelModeTable[] = {
    [0] = AMBA_SENSOR_OV2710_1920_1080_30P,  /* full view */
};

const UINT32 TI913914_OV2710_AmbaBadPixelModeAssignment[] = {
    [AMBA_SENSOR_OV2710_1920_1080_30P]    = 0,
    [AMBA_SENSOR_OV2710_1280_720_60P]    = 0,
    [AMBA_SENSOR_OV2710_1280_720_30P]    = 0,
    [AMBA_SENSOR_OV2710_1280_720_60_30P]    = 0,
};

AMBA_CALIB_INFO_OBJ_s AmbaCalibInfo_TI913914_OV2710Obj = {
    .AmbaVignetteMode           = AMBA_SENSOR_OV2710_1920_1080_30P,
    .AmbaNumBadPixelMode        = GetArraySize(TI913914_OV2710_AmbaBadPixelModeTable),
    .AmbaBadPixelModeTable      = TI913914_OV2710_AmbaBadPixelModeTable,
    .AmbaBadPixelModeAssignment = TI913914_OV2710_AmbaBadPixelModeAssignment,
    .AmbaCalibWidth             = 1920,
    .AmbaCalibHeight            = 1080,
};
