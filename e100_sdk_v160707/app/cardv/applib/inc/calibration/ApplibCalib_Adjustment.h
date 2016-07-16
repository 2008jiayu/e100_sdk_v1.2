/**
 * @file app/connected/applib/inc/calibration/vig/ApplibCalib_Adjustment.h
 *
 * header file for adjust calibration parameters
 *
 * History:
 *    02/16/2015  Allen Chiu Created
 *
 *
 * Copyright (c) 2015 Ambarella, Inc.
 *
 * This file and its contents (¡°Software¡±) are protected by intellectual property rights
 * including, without limitation, U.S. and/or foreign copyrights.  This Software is also the
 * confidential and proprietary information of Ambarella, Inc. and its licensors.  You may
 * not use, reproduce, disclose, distribute, modify, or otherwise prepare derivative
 * works of this Software or any portion thereof except pursuant to a signed license
 * agreement or nondisclosure agreement with Ambarella, Inc. or its authorized
 * affiliates.	In the absence of such an agreement, you agree to promptly notify and
 * return this Software to Ambarella, Inc.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF NON-
 * INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL AMBARELLA, INC. OR ITS AFFILIATES BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; COMPUTER FAILURE OR
 * MALFUNCTION; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include "AmbaDSP_ImgFilter.h"

/**
* @defgroup ApplibCalib_Adjustment
* @brief Header file for Calibration control
*
*/

/**
 * @addtogroup ApplibCalib_Adjustment
 * @{
 */

#define CALIB_PARAM_VER     (0x14031400)
#define LV_TABLE_NUM        (21)
#define WB_GAIN_TAB_NUM     (9)

/**
 * Vignette Luma Strength table
 */
typedef struct _VIG_LUMA_TABLE_s_ {
	UINT16 VideoWeightTable[LV_TABLE_NUM];  /**< luma strength weightting for video */
	UINT16 StillWeightTable[LV_TABLE_NUM];  /**< luma strength weightting for still */
} VIG_LUMA_TABLE_s;

/**
 * Vignette WB Strength table
 */
typedef struct _VIG_WB_GAIN_TABLE_s_ {
	AMBA_DSP_IMG_WB_GAIN_s WbGain;  /**< WB weightting */
	int WbIdx;                      /**< WB index */
} VIG_WB_GAIN_TABLE_s;

/**
 * Vignette WB blend table
 */
typedef struct _WB_BLEND_CURVE_s_ {
	VIG_WB_GAIN_TABLE_s     *Start; /**< start of blend region */
	VIG_WB_GAIN_TABLE_s     *End;   /**< end of blend region */
	UINT16                  VideoWeightTab[WB_GAIN_TAB_NUM]; /**< video weighting  */
	UINT16                  StillWeightTab[WB_GAIN_TAB_NUM]; /**< still weighting */
} WB_BLEND_CURVE_s;

/**
 * calibration control parameters
 */
typedef struct _CALIBRATION_ADJUST_PARAM_s_ {
    UINT32                   FWVersionNumber;       /**< FW version number */
    UINT32                   ParamVersionNum;       /**< parameter version number */
    UINT32                   VigLumaStrCtlEnable;   /**< vignette luma control flag */
    UINT32                   VigChromaStrCtlEnable; /**< vignette chroma control flag */    
    UINT32                   VigBlendOzoomEnable;   /**< vignette OZ blend control flag */
    UINT32                   VigBlendWBEnable;      /**< vignette WB blend control flag */
    UINT32                   CheckFrameNum;         /**< frame period to control vignette luma strength */
    UINT32                   AverageNum;            /**< frame NO. to average the vignette strength (1*new_gain+(AverageNum-1)*old)/AverageNum */
    UINT32                   LumaThreshold;         /**< LV Threshold to change the luma strength*/
    UINT32                   ChromaThreshold;     /**< LV Threshold to change the chroma strength*/
    VIG_LUMA_TABLE_s         VigLumaTable[4];       /**< vignette luma table */
    VIG_LUMA_TABLE_s         VigChromaTable[4];     /**< vignette chroma table */    
    WB_BLEND_CURVE_s         VigWbBlendTable[4];    /**< vignette WB table */
} CALIBRATION_ADJUST_PARAM_s;

/**
 * @}
 */


