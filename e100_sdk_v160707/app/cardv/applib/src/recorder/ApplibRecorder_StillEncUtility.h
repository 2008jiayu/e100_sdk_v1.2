/**
 * @file src/app/connected/applib/src/recorder/ApplibRecorder_StillEncUtility.h
 *
 * Header of photo utility
 *
 * History:
 *    2013/12/27 - [Martin Lai] created file
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

#ifndef APPLIB_STILL_ENC_UTILITY_H_
#define APPLIB_STILL_ENC_UTILITY_H_

#include <applib.h>

__BEGIN_C_PROTO__
extern UINT8 ApplibJpegQTable[3][128];
extern UINT32 AppLibStillEnc_jpegBRCPredictCB(UINT16 targetBpp, UINT32 stillProc, UINT8* predictQ, UINT8 *curveNum, UINT32 *curveAddr);
extern int AppLibStillEnc_QvShowBufferAllocate(UINT32 QvLCDSize, UINT32 QvHDMISize);
extern UINT32 AppLibStillEnc_RawCapCB(AMP_STILLENC_RAWCAP_FLOW_CTRL_s *ctrl);
extern UINT32 AppLibStillEnc_MultiRawCapCB(AMP_STILLENC_RAWCAP_FLOW_CTRL_s *ctrl);
extern UINT32 AppLibStillEnc_PostWBCalculation(AMBA_DSP_EVENT_STILL_CFA_3A_DATA_s *cfaStat);

extern int StillUtil_parseStillCFAStat(AMBA_DSP_EVENT_STILL_CFA_3A_DATA_s *cfaStat, UINT8 dump);
extern int AppLibCalib_SetDspMode(AMBA_DSP_IMG_MODE_CFG_s *pMode);


__END_C_PROTO__

#endif /* APPLIB_STILL_ENC_UTILITY_H_ */
