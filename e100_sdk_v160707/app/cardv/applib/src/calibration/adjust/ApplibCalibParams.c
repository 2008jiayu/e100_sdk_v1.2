
#include "AmbaDataType.h"
#include <calibration/ApplibCalib_Adjustment.h>

CALIBRATION_ADJUST_PARAM_s *AmbaCalibParamsAddr;

CALIBRATION_ADJUST_PARAM_s* AppLibCalibAdjustGetCalibrationParam(void)
{
    return AmbaCalibParamsAddr;
}



