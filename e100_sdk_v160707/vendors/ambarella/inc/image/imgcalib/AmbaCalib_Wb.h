/**
 * @file vendor/ambarella/inc/image/imgcalib/AmbaCalib_Wb.h
 *
 * header file for Amba WB calibration
 *
 * Copyright (C) 2013, Ambarella, Inc.
 *
 * All rights reserved. No Part of this file may be reproduced, stored
 * in a retrieval system, or transmitted, in any form, or by any means,
 * electronic, mechanical, photocopying, recording, or otherwise,
 * without the prior consent of Ambarella, Inc.
 */

/**
* @defgroup AmbaCalib_Wb
* @brief White balance calibration
*
* Wb provide function to calculate White balance.
*/

/**
 * @addtogroup AmbaCalib_Wb
 * @ingroup Calibration
 * @{
 */

#include "AmbaDSP_ImgFilter.h"

#define WB_CAL_MAX_CH   8

/**
* White balance gain data structure
*/
typedef struct Idsp_WB_Gain_s_ {
    UINT32 GainR;       /**< Red color gain value */
    UINT32 GainG;       /**< Green color gain value */
    UINT32 GainB;       /**< Blue color gain value */
} Idsp_WB_Gain_s;
typedef Idsp_WB_Gain_s WB_Gain_s; // for backward compatible

/**
* White balance computation data structure
*/
typedef struct WB_Comp_Info_s_ {
    WB_Gain_s OrgWB[WB_CAL_MAX_CH][2];  /**< original wb. WB_CAL_MAX_CH: ChId, 2: LCT and HCT */
    WB_Gain_s RefWB[WB_CAL_MAX_CH][2];  /**< reference wb. WB_CAL_MAX_CH: ChId, 2: LCT and HCT */
    UINT8 Status[WB_CAL_MAX_CH];        /**< wb info status(see below). WB_CAL_MAX_CH: ChId */
#define WB_CAL_LHCT_RESET   0
#define WB_CAL_LCT_DONE         1
#define WB_CAL_HCT_DONE         2
} WB_Comp_Info_s;

/**
* White balance information data structure
*/
typedef struct  WB_Detect_Info_s_ {
    UINT8  Index;           /**< Current index */
#define WB_CAL_TEST             0
#define WB_CAL_LCT              1
#define WB_CAL_HCT              2

#define WB_CAL_STORE_LCT        5
#define WB_CAL_STORE_HCT        6

#define WB_CAL_GET_LCT      9
#define WB_CAL_GET_HCT      10

#define WB_CAL_RESET        98
#define WB_CAL_INVALID      99
    UINT16 CurR;            /**< Current red strength */
    UINT16 CurG;            /**< Current green strength */
    UINT16 CurB;            /**< Current blue strength */
    UINT16 RefR;            /**< Reference red strength */
    UINT16 RefG;            /**< Reference red strength */
    UINT16 RefB;            /**< Reference red strength */
    UINT8  ChID;            /**< Channel ID */
    UINT8  FlickerMode;     /**< Flag for FlickerMode  */
    UINT8  Reserved1;       /**< Reserved for further usage */
    UINT8  Reserved2;       /**< Reserved for further usage */
    UINT8  Reserved3;       /**< Reserved for further usage */

}  WB_Detect_Info_s;

/**
*  White balance error massage
*/
typedef enum _WB_CALIB_ERROR_MSG_e_ {
    WB_CALIB_OK          = 0,                   /**< Ok */
    WB_CALIB_ERROR_PARAMETER = -1,              /**< Wrong parameter */
    WB_CALIB_ERROR_INCORRECT_INDEX = -2,        /**< Wrong index */
    WB_CALIB_ERROR_DIFF_OVER_THRESHOLD = -3,    /**< Max color strength difference is too big */
    WB_CALIB_ERROR_SENSOR_DIFF_TOO_BIG = -4,    /**< Sensor input color strength is too big */
    WB_CALIB_ERROR_NEGATIVE_THRESHOLD = -5,     /**< The threshold is negative */
    WB_CALIB_ERROR_THRESHOLD_TOO_BIG = -6,      /**< The threshold is too large */
    WB_CALIB_ERROR_OPEN_RAW_ERROR = -7,         /**< Can't open raw image file */
    WB_CALIB_ERROR_NO_VIG = -8,                 /**< do vignette calibration before WB calibration */
} WB_CALIB_ERROR_MSG_e;



/**
*  White balance calibration function
*
*  @param [in] WbDetectInfo calibration information for white balance calibration
*  @param [out] OutputStr Output message
*
*  @return 0 - OK, others - WB_CALIB_ERROR_MSG_e
*  @see WB_CALIB_ERROR_MSG_e
*/
extern int AmpCalib_CalWB(WB_Detect_Info_s *WbDetectInfo, char *OutputStr);//,AMBA_DSP_IMG_WB_GAIN_s AwbGain);

/**
*  Set WB compensation Info
*
*  @param [in] WbCompInfo RGB value for compensating information
*
*  @return 0 - OK, others - WB_CALIB_ERROR_MSG_e
*  @see WB_CALIB_ERROR_MSG_e
*/
//extern void AmpCalib_SetWBCompInfo(WB_Comp_Info_s *WbCompInfo);

/**
*  Get WB compensation Info
*
*  @param [in] WbCompInfo R RGB value for compensating information
*
*  @return 0 - OK, others - WB_CALIB_ERROR_MSG_e
*  @see WB_CALIB_ERROR_MSG_e
*/
//extern void AmpCalib_GetWBCompInfo(WB_Comp_Info_s *WbCompInfo);

/**
 * @}
 */
