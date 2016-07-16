/**
 * @file src/app/connected/applib/inc/recorder/ApplibRecorder_AudioEnc.h
 *
 * Header of Audio encoding config APIs
 *
 * History:
 *    2013/11/14 - [Martin Lai] created file
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
#ifndef APPLIB_AUDIO_ENC_H_
#define APPLIB_AUDIO_ENC_H_

/**
 * @defgroup Recorder
 * @brief Record related function implementation
 *
 * Implementation Recorder
 * Provide Record related function
 *
 */

/**
* @defgroup ApplibRecorder_AudioEnc
* @brief Audio encode related function
*
*
*/

/**
 * @addtogroup ApplibRecorder_AudioEnc
 * @ingroup Recorder
 * @{
 */

#include <applib.h>
#include <recorder/AudioEnc.h>
__BEGIN_C_PROTO__

/*************************************************************************
 * Audio definitions
 ************************************************************************/
#define AUDIO_TYPE_PCM         (0)                /**< AUDIO SPLIT OFF */
#define AUDIO_TYPE_AAC          (1)           /**< AUDIO SPLIT SIZE 1G  */
/**
 *  @brief Set encode type.
 *
 *  Set encode type.
 *
 *  @param [in] enctype encode type
 *                  0:PCM
 *                  1:AAC
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibAudioEnc_SetEncType(int enctype);

/**
 *  @brief Get encode type.
 *
 *  Get encode type.
 *
 *
 *  @return Encode type
 *
 */
extern UINT32 AppLibAudioEnc_GetEncType(void);

/**
 *  @brief Modify encode type source sample rate.
 *
 *  Modify encode type source sample rate.
 *
 *  @param [in] samplerate sample rate
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibAudioEnc_SetSrcSampleRate(int samplerate);

/**
 *  @brief Modify encode type destination sample rate.
 *
 *  Modify encode type destination sample rate.
 *
 *  @param [in] samplerate sample rate
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibAudioEnc_SetDstSampleRate(int samplerate);

/**
 *  @brief Get encode type source sample rate.
 *
 *  Get encode type source sample rate.
 *
 *
 *  @return >=0 success, <0 failure
 */
extern UINT32 AppLibAudioEnc_GetSrcSampleRate(void);

/**
 *  @brief Get encode type source channel mode.
 *
 *  Get encode type source channel mode.
 *
 *
 *  @return source channel mode
 */
extern UINT32 AppLibAudioEnc_GetSrcChanMode(void);

/**
 *  @brief Get encode type destination channel mode.
 *
 *  Get encode type destination channel mode.
 *
 *
 *  @return destination channel mode
 */
extern UINT32 AppLibAudioEnc_GetDstChanMode(void);


/**
 *  Set bit rate
 *
 *  @param [in] bitRate bit rate
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibAudioEnc_SetBitrate(int bitRate);
/**
 *  Get bit rate
 *
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibAudioEnc_GetBitate(void);
/**
 *  Set dual stream
 *
 *  @param [in] dualStreams dualStreams
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibAudioEnc_SetDualStreams(int dualStreams);
/**
 *  Get dual stream
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibAudioEnc_GetDualStreams(void);

/**
 *  Initial the video encoder.
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibAudioEnc_Init(void);

/**
 *  @brief Setup audio encoding parameter.
 *
 *  Setup audio encoding parameter.
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibAudioEnc_Setup(void);

/**
 *  Configure the encoder's parameter
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibAudioEnc_EncodeSetup(void);
/**
 *  Start audio encoding.
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibAudioEnc_EncodeStart(void);

/**
 *  @brief Stop encoding audio.
 *
 *  Stop encoding audio.
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibAudioEnc_EncodeStop(void);
#ifdef CONFIG_APP_ARD
extern int AppLibAudioEnc_Mux_Setup(void);
extern int AppLibAudioEnc_Mute(UINT8 enable_mute);
#endif
__END_C_PROTO__

/**
 * @}
 */

#endif /* APPLIB_AUDIO_ENC_H_ */

