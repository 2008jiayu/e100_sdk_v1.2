/**
 * @file src/app/connected/applib/inc/calibration/blc/ApplibCalibBlc.h
 *
 * header file for black level calibration
 *
 * History:
 *    07/10/2013  Allen Chiu Created
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
#include <calibration/ApplibCalibMgr.h>
#include <imgcalib/AmbaCalib_Blc.h>

/**
 * @defgroup ApplibCalibBlackLevel
 * @brief extern functions of ApplibCalibBlc
 *
 */

/**
 * @addtogroup ApplibCalibBlc
 * @{
 */

/**
 * Black level calibration function
 *
 * @param [in] CalBlcInfo     Information of black level calibration
 *
 * @param [out] *BlResult     Black level Black level result
 *
 * @param [out] *OutputStr    Black level Black level result
 *
 * @return 0 - OK, others - BL_CALIB_ERROR_MSG_e
 * @see BL_CALIB_ERROR_MSG_e
 */
extern int AmpCalib_BLCFunc(Cal_Blc_Info_s *CalBlcInfo, AMBA_DSP_IMG_BLACK_CORRECTION_s *BlResult, char *OutputStr);

extern void Amba_AeAwb_Get_AAA_OP_Info(AMBA_3A_OP_INFO_s *pAaaOpInfo);
extern void Amba_AeAwb_Set_AAA_OP_Info(AMBA_3A_OP_INFO_s *pAaaOpInfo);
extern int AeSetVideoExposureValue(AMBA_AE_INFO_s *VideoInfo);
extern int Amba_AeAwb_Set_Ae_Info(UINT8 Mode, AMBA_AE_INFO_s *AeInfo);
extern int Amba_AeAwb_Set_Dgain(UINT32 Dgain);
extern int AppLib_CalibSiteInit(void);

/**
 * @}
 */

