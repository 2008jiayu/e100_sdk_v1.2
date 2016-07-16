
/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaCalib_Utility.h
 *
 *  @Copyright      :: Copyright (C) 2013 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Header file for calibration parameters
 *
\*-------------------------------------------------------------------------------------------------------------------*/
#include "AmbaDSP_ImgFilter.h"
#include "AmbaDSP.h"

/**
 * @addtogroup Calibration
 * @{
 */


typedef struct AMBA_CALIB_SENSOR_MODE_INFO_s_ {
    UINT16                              Mode;           /* Sensor Mode ID */
    UINT16                              VerticalFlip;   /* Vertical flip flag */
    AMBA_DSP_IMG_VIN_SENSOR_GEOMETRY_s  VinSensorGeo;   /* Current Vin sensor geometry */
    AMBA_DSP_IMG_MODE_CFG_s             DSPImgMode;     /* DSP Image Mode */
} AMBA_CALIB_SENSOR_MODE_INFO_s;

extern int AmpCalib_SetDspMode(AMBA_DSP_IMG_MODE_CFG_s *pMode);
extern int AmpCalib_GetDspMode(AMBA_DSP_IMG_MODE_CFG_s *pMode);
extern int AmpCalib_SetSensorModeInfo(UINT8 Channel,AMBA_CALIB_SENSOR_MODE_INFO_s *CalibSensorModeInfo);
extern int AmpCalib_GetSensorModeInfo(UINT8 Channel,AMBA_CALIB_SENSOR_MODE_INFO_s *CalibSensorModeInfo);
extern void AmpCalib_SetOpticalBlackFlag(UINT8 *OpticalBlackFlag);
extern UINT8 AmpCalib_GetOpticalBlackFlag(void);
/**
 * @}
 */
