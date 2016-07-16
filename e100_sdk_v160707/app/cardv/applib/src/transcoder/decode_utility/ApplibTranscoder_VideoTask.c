/**
 * @file src/app/connected/applib/src/transcoder/decode_utility/ApplibTranscoder_VideoTask.c
 *
 * A task handling some callbacks of video player.
 *
 * History:
 *    2015/02/06 - [cichen] Create
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

#include <applib.h>
#include <transcoder/decode_utility/ApplibTranscoder_VideoTask.h>
#include <transcoder/video_decode/ApplibTranscoder_VideoDec.h>
#include "../../AppLibTask_Priority.h"

// Task variables
#define APPLIB_VIDEO_TASK_MSG_NUM       (64)        ///< Maximum number of Vout messages in message pool.
#define APPLIB_VIDEO_TASK_STACK_SIZE    (0x4000)    ///< Size of the stack used by the task.

#define APPLIB_VIDEO_TASK_FLOW_DEBUG                ///< Print message
#undef TRANSC_TASK_DBG
#ifdef APPLIB_VIDEO_TASK_FLOW_DEBUG
#define TRANSC_TASK_DBG(fmt,args...) AmbaPrintColor(BLUE,fmt,##args);
#else
#define TRANSC_TASK_DBG(fmt,args...)
#endif



// Gloabal variable
static APPLIB_TRANSCODE_VIDEO_TASK_RESOURCE_s APPLIB_G_TRANSCODE_VIDEO_TASK = { 0 }; // Video task

static void *VideoMsgPoolRaw = NULL; // Original address of task message pool
static void *VideoStackRaw = NULL;   // Original address of stack for task
static void *VideoStack = NULL;      // Aligned address of stack for task

/**
 * Handle EOS message type.
 *
 * @param [in,out] Msg              Display message.
 *
 * @return 0 - Success, Others - Failure
 */
static int AppLibTranscoderVideoTask_EOS_Internal(const APPLIB_TRANSCODE_VIDEO_TASK_MSG_s *Msg)
{
#if 0
    int Rval = 0; // Function call return

    // TODO: Update CurrentPlayingFileID and feed next file if the feeding needs to be controled. Also, send HMSG_PLAYER_PLY_EOS only once.
    AppLibTranscoderVideoDec_PlayEOS();
    Rval = AppLibComSvcHcmgr_SendMsgNoWait(HMSG_PLAYER_PLY_EOS, 0, 0); // Send EOS message to App
    if (Rval < 0) {
        AmbaPrintColor(RED, "[Applib - VTranscoderideoTask] %s:%u Failed to send HMSG_PLAYER_PLY_EOS message (%d).", __FUNCTION__, __LINE__, Rval);
        return -1; // Error
    }
#endif

    return 0;
}

/**
 * Handle APPLIB_VIDEO_TASK_MSG_FEED_END message type.
 *
 * @param [in,out] Msg              Display message.
 *
 * @return 0 - Success, Others - Failure
 */
static int AppLibTranscoderVideoTask_FeedEnd_Internal(const APPLIB_TRANSCODE_VIDEO_TASK_MSG_s *Msg)
{
    AppLibTranscoderVideoDec_FeedEndHandling();

    return 0;
}

/**
 * Handle unknown message type.\n
 * Do not change any content of the message or reference the location to which the message point.
 *
 * @param [in,out] Msg              Display message.
 *
 * @return 0 - Success, Others - Failure
 */
static int AppLibTranscoderVideoTask_Unknown_Internal(const APPLIB_TRANSCODE_VIDEO_TASK_MSG_s *Msg)
{
    // Do not access any content of the message or invoke any callback.
    // Since we don't know if any of its content is valid.
    AmbaPrintColor(RED, "[Applib - VTranscoderideoTask] <VideoTask> Cannot recognize MessageType: %d", Msg->MessageType);
    return 0;
}

/**
 * Receive and handle messages of video task.
 *
 * @param [in] EntryArg         Argument of EntryArg when the task is created
 *
 * @return None
 */
static void AppLibTranscoderVideoTask_VideoTask(UINT32 EntryArg)
{
    APPLIB_TRANSCODE_VIDEO_TASK_MSG_s VideoMsg;       // Message
    int Rval;                               // Function call return value

    while (1) {
        // Initialize message
        SET_ZERO(VideoMsg);

        // Receive decode message.
        Rval = AmpMsgQueue_Receive(&APPLIB_G_TRANSCODE_VIDEO_TASK.VideoMsgQueue, &VideoMsg, AMBA_KAL_WAIT_FOREVER);

        if (Rval != AMP_MQ_SUCCESS) {
            switch (Rval) {
                case AMP_MQ_INVALID_ERROR: // Invalid pointer of the message queue handler.
                    AmbaPrintColor(RED, "[Applib - TranscoderideoTask] <VideoTask> Task stopped! Invalid pointer of the message queue handler (%d)!", Rval);
                    return; // Stop the task
                case AMP_MQ_NOT_CREATED_ERROR: // The message queue handler is not created.
                    AmbaPrintColor(RED, "[Applib - TranscoderideoTask] <VideoTask> Task stopped! The message queue handler is not created (%d)!", Rval);
                    return; // Stop the task
                case AMP_MQ_MSG_DEST_ERROR: // Invalid destination pointer for message.
                    AmbaPrintColor(RED, "[Applib - TranscoderideoTask] <VideoTask> Task stopped! Invalid destination pointer for message (%d)!", Rval);
                    return; // Stop the task
                case AMP_MQ_DELETED: // The queue has been deleted while the thread was suspended.
                    AmbaPrintColor(RED, "[Applib - TranscoderideoTask] <VideoTask> Task stopped! The queue has been deleted while the thread was suspended (%d)!", Rval);
                    return; // Stop the task
                default:
                    AmbaPrintColor(RED, "[Applib - TranscoderideoTask] <VideoTask> Failed to receive message (%d)!", Rval);
                    // Sleep for a while to avoid busy waiting.
                    AmbaKAL_TaskSleep(10);
                    continue; // Try again
            }
        }

        switch (VideoMsg.MessageType) {
            case APPLIB_VIDEO_TASK_MSG_EOS:
                // The PTS of displayed frame has reached EOS PTS
                TRANSC_TASK_DBG("[Applib - TranscoderideoTask] <VideoTask> EOS: Start");
                Rval = AppLibTranscoderVideoTask_EOS_Internal(&VideoMsg);
                TRANSC_TASK_DBG("[Applib - TranscoderideoTask] <VideoTask> EOS: End");
                break; // Get the next message.
            case APPLIB_VIDEO_TASK_MSG_FEED_END:
                // Demux has finished feeding a file
                TRANSC_TASK_DBG("[Applib - TranscoderideoTask] <VideoTask> FEED_END: Start");
                Rval = AppLibTranscoderVideoTask_FeedEnd_Internal(&VideoMsg);
                TRANSC_TASK_DBG("[Applib - TranscoderideoTask] <VideoTask> FEED_END: End");
                break; // Get the next message.

            default:
                Rval = AppLibTranscoderVideoTask_Unknown_Internal(&VideoMsg);
                break; // Get the next message.
        }
    }
}

/**
 * Whether the video task is initialized.
 *
 * @return 0 - Not initialized, 1 - Initialized
 */
int AppLibTranscoderVideoTask_IsTaskInitialized(void)
{
    return APPLIB_G_TRANSCODE_VIDEO_TASK.IsInit;
}

/**
 * Deinitialize video task.
 *
 * @return 0 - Success, Others - Failure
 */
int AppLibTranscoderVideoTask_DeinitTask(void)
{
    int ReturnValue = 0; // Success
    int Rval = 0; // Function call return

    AmbaPrint("[Applib - VTranscoderideoTask] <DeinitTask> Start");

    /** Step 1: Terminate task */
    Rval = AmbaKAL_TaskTerminate(&APPLIB_G_TRANSCODE_VIDEO_TASK.VideoTask);
    // TX_THREAD_ERROR: The task is not created.
    if ((Rval != AMP_OK) && (Rval != TX_THREAD_ERROR)) { // TODO: Wait DSP definition of TX_THREAD_ERROR
        AmbaPrint("[Applib - VTranscoderideoTask] %s:%u Failed to terminate task (%d).", __FUNCTION__, __LINE__, Rval);
        ReturnValue = -1; // Error
    }

    /** Step 2: Delete task */
    Rval = AmbaKAL_TaskDelete(&APPLIB_G_TRANSCODE_VIDEO_TASK.VideoTask);
    // TX_THREAD_ERROR: The task is not created.
    // TX_DELETE_ERROR: The task is not terminated.
    if ((Rval != AMP_OK) && (Rval != TX_THREAD_ERROR) && (Rval != TX_DELETE_ERROR)) { // TODO: Wait DSP definition of TX_THREAD_ERROR and TX_DELETE_ERROR
        AmbaPrint("[Applib - VTranscoderideoTask] %s:%u Failed to delete task (%d).", __FUNCTION__, __LINE__, Rval);
        ReturnValue = -1; // Error
    }

    /** Step 3: Release stack */
    if (VideoStackRaw != NULL) {
        Rval = AmbaKAL_BytePoolFree(VideoStackRaw);
        if (Rval != AMP_OK) {
            AmbaPrint("[Applib - VTranscoderideoTask] %s:%u Failed to release stack (%d).", __FUNCTION__, __LINE__, Rval);
            ReturnValue = -1; // Error
        }
        VideoStackRaw = NULL;
        VideoStack = NULL;
    }

    /** Step 4: Delete message queue */
    if (AmpMsgQueue_IsCreated(&APPLIB_G_TRANSCODE_VIDEO_TASK.VideoMsgQueue)) {
        Rval = AmpMsgQueue_Delete(&APPLIB_G_TRANSCODE_VIDEO_TASK.VideoMsgQueue);
        if (Rval != AMP_MQ_SUCCESS) {
            AmbaPrint("[Applib - VTranscoderideoTask] %s:%u Failed to delete message queue (%d).", __FUNCTION__, __LINE__, Rval);
            ReturnValue = -1; // Error
        }
    }

    /** Step 5: Release message pool */
    if (VideoMsgPoolRaw != NULL) {
        if (AmbaKAL_BytePoolFree(VideoMsgPoolRaw) != AMP_OK) {
            AmbaPrint("[Applib - VTranscoderideoTask] %s:%u Failed to release message pool.", __FUNCTION__, __LINE__);
            ReturnValue = -1; // Error
        }
        VideoMsgPoolRaw = NULL;
        APPLIB_G_TRANSCODE_VIDEO_TASK.VideoMsgPool = NULL;
    }

    /** Step 6: Reset flag */
    APPLIB_G_TRANSCODE_VIDEO_TASK.IsInit = 0;

    AmbaPrint("[Applib - VTranscoderideoTask] <DeinitTask> End");
    return ReturnValue;
}

/**
 * Initialize video task.
 *
 * @return 0 - Success, Others - Failure
 */
extern int AppLibTranscoderVideoTask_InitTask(void)
{
    int Rval = 0; // Function call return value

    //AmbaPrint("[Applib - VTranscoderideoTask] <InitTask> Start");

    if (APPLIB_G_TRANSCODE_VIDEO_TASK.IsInit) {
        AmbaPrint("[Applib - VTranscoderideoTask] <InitTask> Already init");
        goto ReturnSuccess;
    }

    /** Step 1: Create message pool for message queue */
    if (VideoMsgPoolRaw == NULL) {
        Rval = AmpUtil_GetAlignedPool(
                APPLIB_G_MMPL,
                (void**) &APPLIB_G_TRANSCODE_VIDEO_TASK.VideoMsgPool,
                &VideoMsgPoolRaw,
                (sizeof(APPLIB_TRANSCODE_VIDEO_TASK_MSG_s) * APPLIB_VIDEO_TASK_MSG_NUM),
                1 << 5 // Align 32
                );
        if (Rval != AMP_OK) {
            AmbaPrintColor(RED, "[Applib - VTranscoderideoTask] %s:%u Failed to allocate memory (%d).", __FUNCTION__, __LINE__, Rval);
            goto ReturnError;
        }
    }

    /** Step 2: Create message queue */
    if (AmpMsgQueue_IsCreated(&APPLIB_G_TRANSCODE_VIDEO_TASK.VideoMsgQueue) == 0) {
        Rval = AmpMsgQueue_Create(
                &APPLIB_G_TRANSCODE_VIDEO_TASK.VideoMsgQueue,
                APPLIB_G_TRANSCODE_VIDEO_TASK.VideoMsgPool,
                sizeof(APPLIB_TRANSCODE_VIDEO_TASK_MSG_s),
                APPLIB_VIDEO_TASK_MSG_NUM
                );
        if (Rval != AMP_MQ_SUCCESS) {
            AmbaPrintColor(RED, "[Applib - VTranscoderideoTask] %s:%u Failed to create message queue (%d).", __FUNCTION__, __LINE__, Rval);
            goto ReturnError;
        }
    }

    /** Step 3: Create stack for task */
    if (VideoStackRaw == NULL) {
        Rval = AmpUtil_GetAlignedPool(
                APPLIB_G_MMPL,
                (void**) &VideoStack,
                &VideoStackRaw,
                APPLIB_VIDEO_TASK_STACK_SIZE,
                1 << 5 // Align 32
                );
        if (Rval != AMP_OK) {
            AmbaPrintColor(RED, "[Applib - VTranscoderideoTask] %s:%u Failed to allocate memory (%d).", __FUNCTION__, __LINE__, Rval);
            goto ReturnError;
        }
    }

    /** Step 4: Create task */
    Rval = AmbaKAL_TaskCreate(
            &APPLIB_G_TRANSCODE_VIDEO_TASK.VideoTask, // pTask
            "AppLib_Transcoder_VideoTask", // pTaskName
            APPLIB_VIDEO_TRANSCODER_TASK_PRIORITY, // Priority, original value is 37
            AppLibTranscoderVideoTask_VideoTask, // void (*EntryFunction)(UINT32)
            0x0, // EntryArg
            VideoStack, // pStackBase
            APPLIB_VIDEO_TASK_STACK_SIZE, // StackByteSize
            AMBA_KAL_AUTO_START // AutoStart
            );
    if (Rval != AMP_OK) {
        AmbaPrintColor(RED, "[Applib - VTranscoderideoTask] %s:%u Failed to create task (%d).", __FUNCTION__, __LINE__, Rval);
        goto ReturnError;
    }

    /** Step 5: Set flag */
    APPLIB_G_TRANSCODE_VIDEO_TASK.IsInit = 1;

ReturnSuccess:
    //AmbaPrint("[Applib - VTranscoderideoTask] <InitTask> End");
    return 0; // Success

ReturnError:
    AppLibTranscoderVideoTask_DeinitTask();
    AmbaPrint("[Applib - VTranscoderideoTask] <InitTask> End");
    return -1; // Error
}

/**
 * Send a message to video task.
 *
 * @param [in] VideoMsg         Message to video task
 * @param [in] Timeout          The limit of execution time (in ms) for this function. Set a value of 0xFFFFFFFF for endless waiting.
 *
 * @return 0 - OK, -1 - Task is not initialized, Others - AMP_MSG_QUEUE_RESULT_e
 * @see AMP_MSG_QUEUE_RESULT_e
 */
int AppLibTranscoderVideoTask_SendVideoMsg(const APPLIB_TRANSCODE_VIDEO_TASK_MSG_s *VideoMsg, const UINT32 Timeout)
{
    if (AppLibTranscoderVideoTask_IsTaskInitialized() == 0) {
        return -1; // Failure. The task is not initialized.
    }

    return AmpMsgQueue_Send(&APPLIB_G_TRANSCODE_VIDEO_TASK.VideoMsgQueue, VideoMsg, Timeout);
}
