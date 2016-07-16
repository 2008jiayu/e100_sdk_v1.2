/**
 * @file src/app/connected/applib/inc/player/audio_decode/ApplibPlayer_AudioDec.h
 *
 * Audio player with trickplay features.
 *
 * History:
 *    2015/01/06 - [Qiang Su] created file
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

/**
 * @defgroup ApplibPlayer_AudioDec      ApplibPlayer_AudioDec
 * @brief Audio player with trickplay features.
 *
 * Support trickplay features as follows.   \n
 * 1. Speed change                          \n
 * 2. Time search                           \n
 * 3. Pause                                 \n
 * 4. Resume                                \n
 * 5. Step                                  \n
 * 6. Zoom                                  \n
 * 7. Backward
 */

/**
 * @addtogroup ApplibPlayer_AudioDec
 * @ingroup AudioDecode
 * @{
 */

#ifndef APPLIB_AUDIO_DEC_H_
#define APPLIB_AUDIO_DEC_H_

#include <applib.h>
#include <player/Decode.h>
#include <player/VideoDec.h>
#include <player/AudioDec.h>

__BEGIN_C_PROTO__

/**
 * Audio playback dirtection.
 */

/* Audio player state */
typedef enum _APPLIB_AUDIO_PLAYER_STATE_e_ {
    APPLIB_AUDIO_PLAYER_STATE_INVALID = 0,  ///< Not ready.
    APPLIB_AUDIO_PLAYER_STATE_IDLE,         ///< Ready to load a file.
    APPLIB_AUDIO_PLAYER_STATE_PLAY,         ///< Audio is playing.
    APPLIB_AUDIO_PLAYER_STATE_PAUSE,        ///< Audio is paused.
    APPLIB_AUDIO_PLAYER_STATE_PAUSE_CHANGE, ///< Audio is paused with some features (ex. speed, start time...) changed.
    APPLIB_AUDIO_PLAYER_STATE_NUM           ///< Total number of audio states.
} APPLIB_AUDIO_PLAYER_STATE_e;

typedef enum _BEEP_FILE_ID_e_ {
    BEEP_POWER_ON,
    BEEP_POWER_OFF,
    BEEP_ERROR,
    BEEP_OPTONE,
#ifdef CONFIG_APP_ARD
    BEEP_LLWS_WARNING,
    BEEP_FCMD_WARNING,
    BEEP_FCWS_WARNING,
    BEEP_LDWS_WARNING,
#endif
    BEEP_ID_NUM
}BEEP_FILE_ID_e;

extern void AppLibAudioDec_Beep(BEEP_FILE_ID_e beep_id,UINT8 wait_done);

/*************************************************************************
 * MW format module declaration
 ************************************************************************/
/**
 * Initialize audio decoder.
 *
 * @return 0 - OK, others - Error
 */
extern int AppLibAudioDec_Init(void);

/**
 * Get default audio settings.
 *
 * @param [out] OutputAudioStartInfo    Audio settings.
 *
 * @return 0 - OK, others - Error
 */
//extern int AppLibAudioDec_GetStartDefaultCfg(APPLIB_AUDIO_START_INFO_s* OutputAudioStartInfo);

/**
 * Start playing audio.
 *
 * @param [in] AudioStartInfo           Information for playing a audio.
 *
 * @return 0 - OK, others - Error
 */
extern int AppLibAudioDec_Start(char* fn);
#if 0
/**
 * Get default settings for multiple audios.
 *
 * @param [out] OutputAudioStartInfo    Audio settings.
 *
 * @return 0 - OK, others - Error
 */
extern int AppLibAudioDec_GetMultiStartDefaultCfg(APPLIB_AUDIO_START_MULTI_INFO_s* OutputAudioStartInfo);

/**
 * Get movie informations for multiple audios.
 *
 * @param [in,out] AudioStartInfo       Information for playing multiple audios.
 *
 * @return 0 - OK, others - Error
 */
extern int AppLibAudioDec_GetMultiFileInfo(APPLIB_AUDIO_START_MULTI_INFO_s* AudioStartInfo);

/**
 * Start playing multiple audios.
 *
 * @param [in] AudioStartInfo           Information for playing multiple audios.
 *
 * @return 0 - OK, others - Error
 */
extern int AppLibAudioDec_StartMultiple(const APPLIB_AUDIO_START_MULTI_INFO_s* AudioStartInfo);
#endif
/**
 * Speed up the audio.
 *
 * @param [out] CurSpeed        Playback speed after speeding up. A speed of 256 indicates normal speed.
 *
 * @return 0 - OK, others - Error
 */
extern int AppLibAudioDec_SpeedUp(UINT32 *CurSpeed);

/**
 * Slow down the audio.
 *
 * @param [out] CurSpeed        Playback speed after slowing down. A speed of 256 indicates normal speed.
 *
 * @return 0 - OK, others - Error
 */
extern int AppLibAudioDec_SpeedDown(UINT32 *CurSpeed);

/**
 * Pause the audio.
 *
 * @return 0 - OK, others - Error
 */
extern int AppLibAudioDec_Pause(void);

/**
 * Resume the audio.                                            \n
 * Play at the speed right before "pause" action.               \n
 * Do nothing if the audio is still playing.
 *
 * @return 0 - OK, others - Error
 */
extern int AppLibAudioDec_Resume(void);

/**
 * Step one frame forwards.
 *
 * @return 0 - OK, others - Error
 */
extern int AppLibAudioDec_Step(void);

/**
 * Stop the audio.
 *
 * @return 0 - OK, others - Error
 */
extern int AppLibAudioDec_Stop(void);

/**
 * Zoom, shift and play the audio.
 *
 * @param [in] Factor           Magnification Factor. A factor of 100 indicates the original size.
 * @param [in] X                Shift on X-axis. Number of pixels (of the original image) to shift along X-axis.
 * @param [in] Y                Shift on Y-axis. Number of pixels (of the original image) to shift along Y-axis.
 *
 * @return 0 - OK, others - Error
 */
extern int AppLibAudioDec_Zoom(const UINT32 Factor, const INT32 X, const INT32 Y);

/**
 * Exit the audio.
 *
 * @return 0 - OK, others - Error
 */
extern int AppLibAudioDec_Exit(void);

/**
 * Get the current audio time.
 *
 * @param [out] time            Current audio time.
 *
 * @return 0 - OK, others - Error
 */
extern int AppLibAudioDec_GetTime(UINT64 *time);

/**
 * Set the PTS value at the end of the audio.
 *
 * @param [in] frameCount       Number of frames.
 * @param [in] timePerFrame     File time of a frame.
 * @param [in] timePerSec       File time in a second.
 *
 * @return 0 - OK, others - Error
 */
extern int AppLibAudioDec_SetPtsFrame(UINT32 frameCount, UINT32 timePerFrame, UINT32 timePerSec);

/**
 * Set the PTS value at the end of the audio.
 *
 * @param [in] eosFileTime      File time of the last frame.
 * @param [in] timePerFrame     File time of a frame.
 * @param [in] timePerSec       File time in a second.
 *
 * @return 0 - OK, others - Error
 */
extern int AppLibAudioDec_SetEosPts(UINT64 eosFileTime, UINT32 timePerFrame, UINT32 timePerSec);

/**
 * Feed next file.
 * Used when playing split file.
 *
 * @param [in] startTime        Start time in ms.
 *
 * @return 0 - Success, Others - Failure
 */
extern int AppLibAudioDec_FeedNextFile(UINT32 startTime);
extern void AppLibAudioDec_Beep_Init(void);
extern void AppLibAudioDec_Beep_PowerOnOff(int PowerOn);
extern int AppLibAudioDec_SetVolume(UINT8 Volumes);
extern int AppLibAudioDec_SetOutputVolume(void);
extern APPLIB_AUDIO_PLAYER_STATE_e AppLibAudioDec_GetPlayerState(void);
extern void AppLibAudioDec_Beep_Enable(UINT8 enable);
__END_C_PROTO__

#endif /* APPLIB_AUDIO_DEC_H_ */

/**
 * @}
 */     // End of group ApplibPlayer_AudioDec
