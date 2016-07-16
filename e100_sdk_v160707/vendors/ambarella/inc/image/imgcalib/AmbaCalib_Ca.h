/**
 * @file vendor/ambarella/inc/image/imgcalib/AmbaCalib_Ca.h
 *
 * header file for Amba Ca calibration
 *
 * Copyright (C) 2013, Ambarella, Inc.
 *
 * All rights reserved. No Part of this file may be reproduced, stored
 * in a retrieval system, or transmitted, in any form, or by any means,
 * electronic, mechanical, photocopying, recording, or otherwise,
 * without the prior consent of Ambarella, Inc.
 */

#include "AmbaDSP_ImgFilter.h"
#include "AmbaCalibInfo.h"

/**
* @defgroup AmbaCalib_Ca
* @brief Chromatic abbreation calibration
*
* Set/Get Chromatic abbreation info address.
*/


/**
 * @addtogroup AmbaCalib_Ca
 * @ingroup Calibration
 * @{
 */

/**
*  Set CA data address
*
*  @param [in] Channel channel id
*  @param [in] pVignetteCalcInfo address of Ca calibration buffer
*
*  @return none
*/

/**
*  Get CA data address
*
*  @param [in] Channel channel id
*  @param [in] pVignetteCalcInfo address of Ca calibration buffer
*
*  @return none
*/
extern void AmpCalib_GetCaInfoAddr(UINT8 Channel,UINT32 *pCaCalInfo);

/**
 * @}
 */
