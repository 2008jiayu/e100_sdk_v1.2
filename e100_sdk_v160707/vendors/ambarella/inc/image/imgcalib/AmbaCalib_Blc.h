/**
 * @file vendor/ambarella/inc/image/imgcalib/AmbaCalib_Blc.h
 *
 * header file for black level calibration
 *
 * Copyright (C) 2013, Ambarella, Inc.
 *
 * All rights reserved. No Part of this file may be reproduced, stored
 * in a retrieval system, or transmitted, in any form, or by any means,
 * electronic, mechanical, photocopying, recording, or otherwise,
 * without the prior consent of Ambarella, Inc.
 */

#include "AmbaDSP_ImgFilter.h"

#ifndef _AMBACALIB_BLC_
#define _AMBACALIB_BLC_

#define BLC_MODE_TEST           0
#define BLC_MODE_CALIBRATION    1
#define BLC_MODE_RESET          2
#define BLC_MODE_APPLY          3
#define BLC_MODE_GET            4
#define BLC_MODE_CALCULATE      5

/**
* @defgroup Calibration
* @brief Calibration flows
*
*
*/

/**
* @defgroup AmbaCalib_BLC
* @brief Black level calibration flows
*
* Blc provide function to calculate black level. \n
*/


/**
 * @addtogroup AmbaCalib_BLC
 * @ingroup Calibration
 * @{
 */


#if 1
/**
* Black Level infomation
*/
typedef struct Cal_Blc_Info_s_ {
    AMBA_DSP_IMG_BLACK_CORRECTION_s BlVideo[10];    /**< black level values in videoenc mode */
    AMBA_DSP_IMG_BLACK_CORRECTION_s BlStill[10];    /**< black level values in stilltuning mode */
    UINT32 SrcRawAddr;                              /**< image raw data address */
    UINT32 SrcRawWidth;                             /**< image raw data width */
    UINT32 SrcRawHeight;                            /**< image raw data height */
    UINT8 Bayer;                                    /**< image raw data bayer pattern */
    UINT8 BlMode;                                   /**< current blc function mode */
    AMBA_DSP_IMG_BLACK_CORRECTION_s BlStd;          /**< standard black level value */
    INT8 BlCalResult;                               /**< black level calibration result. 1:OK 0:init -1:NG */  //TBD
#define BL_CAL_OK       1   //TBD
#define BL_CAL_INIT     0   //TBD
#define BL_CAL_NG      -1   //TBD
} Cal_Blc_Info_s;
#endif

/**
* Black Level error message
*/
typedef enum _BL_CALIB_ERROR_MSG_e_ {
    BL_CALIB_OK          = 0,               /**< OK */
    BL_CALIB_ERROR_OVERFLOW = -1,           /**< BLC Overflow */
    BL_CALIB_ERROR_BAYER = -2,              /**< Unknown bayer pattern */
    BL_CALIB_ERROR_PARAMETER = -3,          /**< BLC Parameter Wrong */
    BL_CALIB_ERROR_RAW_CAPTURE = -4,        /**< BLC Raw Capture Fail */
    BL_CALIB_ERROR_OVER_THRESHOLD = -5,     /**< BLC exceed threshold */
    BL_CALIB_ERROR_OPEN_RAW_ERROR = -6,     /**< Open img raw data failed */
} BL_CALIB_ERROR_MSG_e;

/**
*  Black level calibration function
*
*  @param [in] CalBlcInfo information of black level calibration
*  @param [out] BlResult black level calibration result. 1:OK 0:init -1:NG
*  @param [out] OutputStr black level Black level result message
*
*  @return 0 - OK, others - BL_CALIB_ERROR_MSG_e    //TBD
*  @see BL_CALIB_ERROR_MSG_e
*/
extern int AmpCalib_BLCFunc(Cal_Blc_Info_s *CalBlcInfo, AMBA_DSP_IMG_BLACK_CORRECTION_s *BlResult, char *OutputStr);

/**
 * @}
 */

#endif
