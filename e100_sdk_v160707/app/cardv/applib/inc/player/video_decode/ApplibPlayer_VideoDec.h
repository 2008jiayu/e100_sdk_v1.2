/**
 * @file src/app/connected/applib/inc/player/video_decode/ApplibPlayer_VideoDec.h
 *
 * Video player with trickplay features.
 *
 * History:
 *    2013/09/26 - [Martin Lai] created file
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
 * @defgroup ApplibPlayer_VideoDec      ApplibPlayer_VideoDec
 * @brief Video player with trickplay features.
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
 * @addtogroup ApplibPlayer_VideoDec
 * @ingroup VideoDecode
 * @{
 */

#ifndef APPLIB_VIDEO_DEC_H_
#define APPLIB_VIDEO_DEC_H_

#include <applib.h>
#include <player/Decode.h>
#include <player/VideoDec.h>
#include <player/AudioDec.h>

__BEGIN_C_PROTO__

/**
 * Video playback dirtection.
 */
typedef enum _APPLIB_VIDEO_PLAY_DIRECTION_e_ {
    APPLIB_VIDEO_PLAY_FW = AMP_VIDEO_PLAY_FW,   ///< Play forward
    APPLIB_VIDEO_PLAY_BW = AMP_VIDEO_PLAY_BW    ///< Play backward
} APPLIB_VIDEO_PLAY_DIRECTION_e;

/**
 * Information to play a video.
 */
typedef struct _APPLIB_VIDEO_START_INFO_s_{
    /**
     * Start address of filename. Full path of a video.
     */
    char* Filename;
    /**
     * Play after opening the file successfully.
     * 0 - Open and pause the video.
     * 1 - Open and play the video.
     */
    UINT8 AutoPlay;
    /**
     * Video start time (in ms).
     */
    UINT32 StartTime;
    /**
     * Play video forward or backward.
     * APPLIB_VIDEO_PLAY_FW - Forward.
     * APPLIB_VIDEO_PLAY_BW - Backward.
     */
    APPLIB_VIDEO_PLAY_DIRECTION_e Direction;
    /**
     * Set playing speed to the default value.
     * 0 - Play at current speed.
     *     If the last action prior to "start" is "pause", the video will play at speed = 0.
     *     Call "resume" to play at the speed before "pause".
     * 1 - Play at normal speed.
     */
    UINT8 ResetSpeed;
    /**
     * Set display size to the default value.
     * 0 - Play with current zoom settings.
     * 1 - Play with original size.
     */
    UINT8 ResetZoom;
} APPLIB_VIDEO_START_INFO_s;

/**
 * Information to play a video.
 */
typedef struct _APPLIB_VIDEO_FILE_INFO_s_{
    /**
     * Start address of filename. Full path of a video.
     */
    char* Filename;
    /**
     * Time offset of the first frame. (in ms)
     */
    UINT32 InitTime;
    /**
     *  Time of the video. (in ms)
     */
    UINT32 Duration;
    /**
     * The time of the last frame. (in "file time")
     */
    UINT64 EosFileTime;
    /**
     * Deviation of EosFileTime. Usually the time between frames. (in "file time")
     */
    UINT32 DeltaFileTime;
    /**
     * How many "file time" are there in a second.
     */
    UINT32 FileTimeScale;
} APPLIB_VIDEO_FILE_INFO_s;

/**
 * Information to play several videos.
 */
typedef struct _APPLIB_VIDEO_START_MULTI_INFO_s_{
    /**
     * Array of file information.
     */
    APPLIB_VIDEO_FILE_INFO_s* File;
    /**
     * Number of files.
     */
    UINT8 FileNum;
    /**
     * Play after opening the file successfully.
     * 0 - Open and pause the video.
     * 1 - Open and play the video.
     */
    UINT8 AutoPlay;
    /**
     * Video start time (in ms).
     */
    UINT32 StartTime;
    /**
     * Play video forward or backward.
     * APPLIB_VIDEO_PLAY_FW - Forward.
     * APPLIB_VIDEO_PLAY_BW - Backward.
     */
    APPLIB_VIDEO_PLAY_DIRECTION_e Direction;
    /**
     * Whether to play videos specified in "File" or not.
     * 0 - Play previous video. "File" and "FileNum" are invalid.
     * 1 - Play videos specified in "File".
     */
    UINT8 ReloadFile;
    /**
     * Set playing speed to the default value.
     * 0 - Play at current speed.
     *     If the last action prior to "start" is "pause", the video will play at speed = 0.
     *     Call "resume" to play at the speed before "pause".
     * 1 - Play at normal speed.
     */
    UINT8 ResetSpeed;
    /**
     * Set display size to the default value.
     * 0 - Play with current zoom settings.
     * 1 - Play with original size.
     */
    UINT8 ResetZoom;
} APPLIB_VIDEO_START_MULTI_INFO_s;

/*************************************************************************
 * MW format module declaration
 ************************************************************************/
/**
 * Initialize video decoder.
 *
 * @return 0 - OK, others - Error
 */
extern int AppLibVideoDec_Init(void);

/**
 * Get default video settings.
 *
 * @param [out] OutputVideoStartInfo    Video settings.
 *
 * @return 0 - OK, others - Error
 */
extern int AppLibVideoDec_GetStartDefaultCfg(APPLIB_VIDEO_START_INFO_s* OutputVideoStartInfo);

/**
 * Start playing video.
 *
 * @param [in] VideoStartInfo           Information for playing a video.
 *
 * @return 0 - OK, others - Error
 */
extern int AppLibVideoDec_Start(const APPLIB_VIDEO_START_INFO_s* VideoStartInfo);

/**
 * Get default settings for multiple videos.
 *
 * @param [out] OutputVideoStartInfo    Video settings.
 *
 * @return 0 - OK, others - Error
 */
extern int AppLibVideoDec_GetMultiStartDefaultCfg(APPLIB_VIDEO_START_MULTI_INFO_s* OutputVideoStartInfo);

/**
 * Get movie informations for multiple videos.
 *
 * @param [in,out] VideoStartInfo       Information for playing multiple videos.
 *
 * @return 0 - OK, others - Error
 */
extern int AppLibVideoDec_GetMultiFileInfo(APPLIB_VIDEO_START_MULTI_INFO_s* VideoStartInfo);

/**
 * Start playing multiple videos.
 *
 * @param [in] VideoStartInfo           Information for playing multiple videos.
 *
 * @return 0 - OK, others - Error
 */
extern int AppLibVideoDec_StartMultiple(const APPLIB_VIDEO_START_MULTI_INFO_s* VideoStartInfo);

/**
 * Speed up the video.
 *
 * @param [out] CurSpeed        Playback speed after speeding up. A speed of 256 indicates normal speed.
 *
 * @return 0 - OK, others - Error
 */
extern int AppLibVideoDec_SpeedUp(UINT32 *CurSpeed);

/**
 * Slow down the video.
 *
 * @param [out] CurSpeed        Playback speed after slowing down. A speed of 256 indicates normal speed.
 *
 * @return 0 - OK, others - Error
 */
extern int AppLibVideoDec_SpeedDown(UINT32 *CurSpeed);

/**
 * Pause the video.
 *
 * @return 0 - OK, others - Error
 */
extern int AppLibVideoDec_Pause(void);

/**
 * Resume the video.                                            \n
 * Play at the speed right before "pause" action.               \n
 * Do nothing if the video is still playing.
 *
 * @return 0 - OK, others - Error
 */
extern int AppLibVideoDec_Resume(void);

/**
 * Step one frame forwards.
 *
 * @return 0 - OK, others - Error
 */
extern int AppLibVideoDec_Step(void);

/**
 * Stop the video.
 *
 * @return 0 - OK, others - Error
 */
extern int AppLibVideoDec_Stop(void);

/**
 * Zoom, shift and play the video.
 *
 * @param [in] Factor           Magnification Factor. A factor of 100 indicates the original size.
 * @param [in] X                Shift on X-axis. Number of pixels (of the original image) to shift along X-axis.
 * @param [in] Y                Shift on Y-axis. Number of pixels (of the original image) to shift along Y-axis.
 *
 * @return 0 - OK, others - Error
 */
extern int AppLibVideoDec_Zoom(const UINT32 Factor, const INT32 X, const INT32 Y);

/**
 * Exit the video.
 *
 * @return 0 - OK, others - Error
 */
extern int AppLibVideoDec_Exit(void);

/**
 * Get the current video time.
 *
 * @param [out] time            Current video time.
 *
 * @return 0 - OK, others - Error
 */
extern int AppLibVideoDec_GetTime(UINT64 *time);

/**
 * Set the PTS value at the end of the video.
 *
 * @param [in] frameCount       Number of frames.
 * @param [in] timePerFrame     File time of a frame.
 * @param [in] timePerSec       File time in a second.
 *
 * @return 0 - OK, others - Error
 */
extern int AppLibVideoDec_SetPtsFrame(UINT32 frameCount, UINT32 timePerFrame, UINT32 timePerSec);

/**
 * Set the PTS value at the end of the video.
 *
 * @param [in] eosFileTime      File time of the last frame.
 * @param [in] timePerFrame     File time of a frame.
 * @param [in] timePerSec       File time in a second.
 *
 * @return 0 - OK, others - Error
 */
extern int AppLibVideoDec_SetEosPts(UINT64 eosFileTime, UINT32 timePerFrame, UINT32 timePerSec);

/**
 * Feed next file.
 * Used when playing split file.
 *
 * @param [in] VideoTime        Current play time.
 * @param [out] StartTime        Start time in ms.
 *
 * @return 0 - Success, Others - Failure
 */
extern int AppLibVideoDec_FeedNextFile(UINT32 StartTime);

/**
 * Handle play EOS.
 * Used when playing reach end.
 *
 *
 * @return 0 - Success, Others - Failure
 */
extern UINT8 AppLibVideoDec_PlayEOS(void);

 extern int AppLibVideoDec_DecodeErrorHandling(void);
__END_C_PROTO__

#endif /* APPLIB_VIDEO_DEC_H_ */

/**
 * @}
 */     // End of group ApplibPlayer_VideoDec
