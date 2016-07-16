/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaCalibInfo_IMX290.c
 *
 *  @Copyright      :: Copyright (C) 2015 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Camera Information Managements
 *
\*-------------------------------------------------------------------------------------------------------------------*/
#include "AmbaCalibInfo.h"
#include "AmbaSensor.h"
#include "AmbaSensor_IMX290.h"

const UINT32 IMX290_AmbaBadPixelModeTable[] = {
    [0] = AMBA_SENSOR_IMX290_1080P30_12BIT,    /* full view */
};

const UINT32 IMX290_AmbaBadPixelModeAssignment[] = {
    [AMBA_SENSOR_IMX290_1080P120_10BIT]                        = 0,
    [AMBA_SENSOR_IMX290_1080P60_12BIT]                         = 0,
    [AMBA_SENSOR_IMX290_1080P30_12BIT]                         = 0,
    [AMBA_SENSOR_IMX290_1080P30_12BIT_DLO_2FRAME]              = 0,
    [AMBA_SENSOR_IMX290_1080P60_10BIT_DLO_2FRAME]              = 0,
    [AMBA_SENSOR_IMX290_1080P60_10BIT_DLO_2FRAME_EXTEND_EXP_S] = 0,
    [AMBA_SENSOR_IMX290_1080P60_P30_10BIT_DLO_2FRAME]          = 0,
    [AMBA_SENSOR_IMX290_1080P100_10BIT]                        = 0,
    [AMBA_SENSOR_IMX290_1080P50_12BIT]                         = 0,
    [AMBA_SENSOR_IMX290_1080P25_12BIT]                         = 0,
    [AMBA_SENSOR_IMX290_1080P25_12BIT_DLO_2FRAME]              = 0,
    [AMBA_SENSOR_IMX290_1080P50_10BIT_DLO_2FRAME]              = 0,
    [AMBA_SENSOR_IMX290_1080P50_10BIT_DLO_2FRAME_EXTEND_EXP_S] = 0,
    [AMBA_SENSOR_IMX290_1080P50_P25_10BIT_DLO_2FRAME]          = 0,
};

AMBA_CALIB_INFO_OBJ_s AmbaCalibInfo_IMX290Obj = {
    .AmbaVignetteMode           = AMBA_SENSOR_IMX290_1080P30_12BIT,
    .AmbaNumBadPixelMode        = GetArraySize(IMX290_AmbaBadPixelModeTable),
    .AmbaBadPixelModeTable      = IMX290_AmbaBadPixelModeTable,
    .AmbaBadPixelModeAssignment = IMX290_AmbaBadPixelModeAssignment
};
