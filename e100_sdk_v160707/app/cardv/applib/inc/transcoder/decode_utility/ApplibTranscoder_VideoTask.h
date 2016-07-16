/**
 * @file src/app/connected/applib/inc/transcoder/decode_utility/ApplibTranscoder_VideoTask.h
 *
 * A task handling some callbacks of video player.
 *
 * History:
 *    2015/02/06 - [cichen] Create file
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
 * @defgroup ApplibTranscoder_VideoTask         ApplibTranscoder_VideoTask
 * @brief A task handling some callbacks of video transcoder.
 */

/**
 * @addtogroup ApplibTranscoder_VideoTask
 * @ingroup DecodeUtility
 * @{
 */

#ifndef _APPLIB_PLYR_VIDEO_TASK_H_
#define _APPLIB_PLYR_VIDEO_TASK_H_

#include <msgqueue.h>

/**
 * The type of Vout message in "Video Vout Task"
 */
typedef enum _APPLIB_TRANSCODE_VIDEO_TASK_MSG_e_ {
    APPLIB_VIDEO_TASK_MSG_EOS = 0,      ///< The PTS of currently displayed frame has reached EOS PTS
    APPLIB_VIDEO_TASK_MSG_FEED_END,     ///< Demux has finished feeding a file
    APPLIB_VIDEO_TASK_MSG_NUMBER        ///< Total number of video Vout message type
} APPLIB_TRANSCODE_VIDEO_TASK_MSG_e;

/**
 * Message as an input to "Video Vout Task"
 */
typedef struct _APPLIB_TRANSCODE_VIDEO_TASK_MSG_s_ {
    /**
     * The type of message.
     */
    APPLIB_TRANSCODE_VIDEO_TASK_MSG_e MessageType;
    /**
     * Video decode handler.
     */
    AMP_AVDEC_HDLR_s *AvcDecHdlr;
} APPLIB_TRANSCODE_VIDEO_TASK_MSG_s;

/**
 * Resource for still decode task and still display task.
 */
typedef struct _APPLIB_TRANSCODE_VIDEO_TASK_RESOURCE_s_{
    /**
     * Whether the task is initialized.  \n
     * 0: Not initialized, 1: Initialized
     */
    UINT8 IsInit;
    /**
     * Video task.
     */
    AMBA_KAL_TASK_t VideoTask;
    /**
     * Message queue for all events.
     */
    AMP_MSG_QUEUE_HDLR_s VideoMsgQueue;
    /**
     * Task message pool.
     */
    APPLIB_TRANSCODE_VIDEO_TASK_MSG_s *VideoMsgPool;
} APPLIB_TRANSCODE_VIDEO_TASK_RESOURCE_s;

/**
 * Whether the video task is initialized.
 *
 * @return 0 - Not initialized, 1 - Initialized
 */
extern int AppLibTranscoderVideoTask_IsTaskInitialized(void);

/**
 * Deinitialize video task.
 *
 * @return 0 - Success, Others - Failure
 */
extern int AppLibTranscoderVideoTask_DeinitTask(void);

/**
 * Initialize video task.
 *
 * @return 0 - Success, Others - Failure
 */
extern int AppLibTranscoderVideoTask_InitTask(void);

/**
 * Send a message to video task.
 *
 * @param [in] VideoMsg         Message to video task
 * @param [in] Timeout          The limit of execution time (in ms) for this function. Set a value of 0xFFFFFFFF for endless waiting.
 *
 * @return 0 - OK, -1 - Task is not initialized, Others - AMP_MSG_QUEUE_RESULT_e
 * @see AMP_MSG_QUEUE_RESULT_e
 */
extern int AppLibTranscoderVideoTask_SendVideoMsg(const APPLIB_TRANSCODE_VIDEO_TASK_MSG_s *VideoMsg, const UINT32 Timeout);

#endif /* _APPLIB_PLYR_VIDEO_TASK_H_ */

/**
 * @}
 */     // End of group ApplibPlayer_VideoTask
