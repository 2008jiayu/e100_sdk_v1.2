/**
 * @file app/connected/applib/inc/calibration/adjust/AmbaCalibParams_default.h
 *
 * calibration parameters to control the calibration in running time
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

#include "AmbaDataType.h"
#include <calibration/ApplibCalib_Adjustment.h>
/**
 * calibration control parameters for Default
 */
CALIBRATION_ADJUST_PARAM_s AmbaCalibParamsDefault = {
    0x14031400,		/**< FW version number */
    0x14031400,		/**< parameter version number */
    0,              /**< vignette luma control flag */
    0,              /**< vignette chroma control flag */
    0,              /**< vignette OZ blend control flag */
    0,              /**< vignette WB blend control flag */
    10,             /**< frame period to control vignette luma strength */
    8,              /**< frame NO. to average the vignette strength (1*new_gain+(AverageNum-1)*old)/AverageNum */
    20,             /**< LV Threshold to change the luma strength*/ 
    20,             /**< LV Threshold to change the chroma strength*/
    
    //	   0, 100, 200, 300, 400, 500, 600, 700, 800, 900, 1000, 1100, 1200, 1300, 1400, 1500, 1600, 1700 1800 1900 2000  // LV0 - LV20
    {{{  256, 256, 256, 256, 256, 256, 256, 256, 256, 256,  256, 256, 256, 256, 256, 256,  256, 256, 256, 256, 256}, //luma strength weight for video (0 ~ 256)
      {  256, 256, 256, 256, 256, 256, 256, 256, 256, 256,  256, 256, 256, 256, 256, 256,  256, 256, 256, 256, 256}}, //luma strength weight for still (0 ~ 256)
     {{  256, 256, 256, 256, 256, 256, 256, 256, 256, 256,  256, 256, 256, 256, 256, 256,  256, 256, 256, 256, 256}, //luma strength weight for video (0 ~ 256)
      {  256, 256, 256, 256, 256, 256, 256, 256, 256, 256,  256, 256, 256, 256, 256, 256,  256, 256, 256, 256, 256}}, //luma strength weight for still (0 ~ 256
     {{  256, 256, 256, 256, 256, 256, 256, 256, 256, 256,  256, 256, 256, 256, 256, 256,  256, 256, 256, 256, 256}, //luma strength weight for video (0 ~ 256)
      {  256, 256, 256, 256, 256, 256, 256, 256, 256, 256,  256, 256, 256, 256, 256, 256,  256, 256, 256, 256, 256}}, //luma strength weight for still (0 ~ 256
     {{  256, 256, 256, 256, 256, 256, 256, 256, 256, 256,  256, 256, 256, 256, 256, 256,  256, 256, 256, 256, 256}, //luma strength weight for video (0 ~ 256)
      {  256, 256, 256, 256, 256, 256, 256, 256, 256, 256,  256, 256, 256, 256, 256, 256,  256, 256, 256, 256, 256}}}, //luma strength weight for still (0 ~ 256
      
    {{{  256, 256, 256, 256, 256, 256, 256, 256, 256, 256,  256, 256, 256, 256, 256, 256,  256, 256, 256, 256, 256}, //chroma strength weight for video (0 ~ 256)
       {  256, 256, 256, 256, 256, 256, 256, 256, 256, 256,  256, 256, 256, 256, 256, 256,  256, 256, 256, 256, 256}}, //chroma strength weight for still (0 ~ 256)
     {{  256, 256, 256, 256, 256, 256, 256, 256, 256, 256,  256, 256, 256, 256, 256, 256,  256, 256, 256, 256, 256}, //chroma strength weight for video (0 ~ 256)
      {  256, 256, 256, 256, 256, 256, 256, 256, 256, 256,  256, 256, 256, 256, 256, 256,  256, 256, 256, 256, 256}}, //chroma strength weight for still (0 ~ 256
     {{  256, 256, 256, 256, 256, 256, 256, 256, 256, 256,  256, 256, 256, 256, 256, 256,  256, 256, 256, 256, 256}, //chroma strength weight for video (0 ~ 256)
      {  256, 256, 256, 256, 256, 256, 256, 256, 256, 256,  256, 256, 256, 256, 256, 256,  256, 256, 256, 256, 256}}, //chroma strength weight for still (0 ~ 256
     {{  256, 256, 256, 256, 256, 256, 256, 256, 256, 256,  256, 256, 256, 256, 256, 256,  256, 256, 256, 256, 256}, //chroma strength weight for video (0 ~ 256)
      {  256, 256, 256, 256, 256, 256, 256, 256, 256, 256,  256, 256, 256, 256, 256, 256,  256, 256, 256, 256, 256}}}, //chroma strength weight for still (0 ~ 256      
       // wb_idx = (b_gain * 4096)/r_gain, wb gain should be normalized.
    /* blend region weight table (0 ~ 256) */                                /* start */       /* end */
    { {0,0,{256, 224, 192, 160, 128,  96, 64, 32,  0}, {256, 224, 192, 160, 128,  96, 64, 32,  0}}, // Blend region #0 (6200K to 4000K)
      {0,0,{256, 224, 192, 160, 128,  96, 64, 32,  0}, {256, 224, 192, 160, 128,  96, 64, 32,  0}}, // Blend region #0 (6200K to 4000K)
      {0,0,{256, 224, 192, 160, 128,  96, 64, 32,  0}, {256, 224, 192, 160, 128,  96, 64, 32,  0}}, // Blend region #0 (6200K to 4000K)
      {0,0,{256, 224, 192, 160, 128,  96, 64, 32,  0}, {256, 224, 192, 160, 128,  96, 64, 32,  0}}}, // Blend region #0 (6200K to 4000K)
};



