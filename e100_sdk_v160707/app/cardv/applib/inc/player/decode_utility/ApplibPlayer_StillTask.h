/**
 * @file src/app/connected/applib/inc/player/decode_utility/ApplibPlayer_StillTask.h
 *
 * Create a task that controls still Vout process (which involves feeding, decoding, rescaling, and displaying images)
 *
 * History:
 *    2014/01/14 - [phcheng] Create file
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
 * @defgroup ApplibPlayer_StillTask         ApplibPlayer_StillTask
 * @brief A task handling the processes of displaying images on screen.
 *
 * Decode process is seperated into 4 steps.\n
 * 1. Feed      \n
 * 2. Decode    \n
 * 3. Rescale   \n
 * 4. Display   \n
 * With combinations of those steps images can be displayed.
 */

/**
 * @addtogroup ApplibPlayer_StillTask
 * @ingroup DecodeUtility
 * @{
 */

#ifndef _APPLIB_PLYR_STILL_TASK_H_
#define _APPLIB_PLYR_STILL_TASK_H_

#include <msgqueue.h>

/**
 * The type of Vout message in "Still Task"
 */
typedef enum _APPLIB_STILL_TASK_MSG_e_ {
    APPLIB_STILL_TASK_MSG_FEED = 0,     ///< Feed message
    APPLIB_STILL_TASK_MSG_DECODE,       ///< Decode message
    APPLIB_STILL_TASK_MSG_RESCALE,      ///< Rescale message
    APPLIB_STILL_TASK_MSG_DISPLAY,      ///< Display message
    APPLIB_STILL_TASK_MSG_DUMMY,        ///< Dummy message is sent before or after a set of messages to handle some action at the beginning or ending of them
    APPLIB_STILL_TASK_MSG_NUM           ///< Total number of type of Vout message
} APPLIB_STILL_TASK_MSG_e;

/**
 * The execution result of "Still Task"
 */
typedef enum _APPLIB_STILL_TASK_RESULT_e_ {
    APPLIB_STILL_TASK_DISPLAY_SUCCESS   = 4,    ///< Success. Succeeded in displaying the buffer.
    APPLIB_STILL_TASK_RESCALE_SUCCESS   = 3,    ///< Success. Succeeded in rescaling the buffer.
    APPLIB_STILL_TASK_DECODE_SUCCESS    = 2,    ///< Success. Succeeded in decoding the file.
    APPLIB_STILL_TASK_FEED_SUCCESS      = 1,    ///< Success. Succeeded in feeding the file.
    APPLIB_STILL_TASK_SUCCESS           = 0,    ///< Success.
    APPLIB_STILL_TASK_UNKNOWN_MSG       = -1,   ///< Failure. Unknown message type.
    APPLIB_STILL_TASK_UNKNOWN_FORMAT    = -2,   ///< Failure. File format is not supported.
    APPLIB_STILL_TASK_FEED_ERROR        = -3,   ///< Failure. Failed to feed the file.
    APPLIB_STILL_TASK_DECODE_ERROR      = -4,   ///< Failure. Failed to decode the file.
    APPLIB_STILL_TASK_RESCALE_ERROR     = -5,   ///< Failure. Failed to rescale the buffer.
    APPLIB_STILL_TASK_DISPLAY_ERROR     = -6,   ///< Failure. Failed to display the buffer.
    APPLIB_STILL_TASK_MSG_QUEUE_ABORT   = -90   ///< Failure. Messages in message queue have been deleted.
} APPLIB_STILL_TASK_RESULT_e;

/**
 * The state of a set of messages related to the same file
 */
typedef enum _APPLIB_STILL_TASK_STATE_e_ {
    APPLIB_STILL_TASK_STATE_NOT_LOADED  = 0,    ///< The file has not been loaded successfully.
    APPLIB_STILL_TASK_STATE_LOADED      = 1     ///< The file has been loaded successfully.
} APPLIB_STILL_TASK_STATE_e;

/**
 * The callback event of Still Task
 */
typedef enum _APPLIB_STILL_TASK_EVENT_ID_e_ {
    APPLIB_STILL_TASK_EVENT_FEED_ALL_BEGIN = 0,             ///< Feed stage (of all feed messages) begin event
    APPLIB_STILL_TASK_EVENT_FEED_ALL_END,                   ///< Feed stage (of all feed messages) end event
    APPLIB_STILL_TASK_EVENT_FEED_BEGIN,                     ///< Feed stage (of a message) begin event
    APPLIB_STILL_TASK_EVENT_FEED_END,                       ///< Feed stage (of a message) end event

    APPLIB_STILL_TASK_EVENT_DECODE_ALL_BEGIN,               ///< Decode stage (of all decode messages) begin event
    APPLIB_STILL_TASK_EVENT_DECODE_ALL_END,                 ///< Decode stage (of all decode messages) end event
    APPLIB_STILL_TASK_EVENT_DECODE_BEGIN,                   ///< Decode stage (of a message) begin event
    APPLIB_STILL_TASK_EVENT_DECODE_END,                     ///< Decode stage (of a message) end event

    APPLIB_STILL_TASK_EVENT_LOAD_END,                       ///< (Feed + Decode) stage end event

    APPLIB_STILL_TASK_EVENT_RESCALE_ALL_CHAN_BEGIN,         ///< Rescale stage (of all channels) begin event
    APPLIB_STILL_TASK_EVENT_RESCALE_ALL_CHAN_END,           ///< Rescale stage (of all channels) end event
    APPLIB_STILL_TASK_EVENT_RESCALE_CHAN_BEGIN,             ///< Rescale stage (of a particular channel) begin event
    APPLIB_STILL_TASK_EVENT_RESCALE_CHAN_END,               ///< Rescale stage (of a particular channel) end event
    APPLIB_STILL_TASK_EVENT_RESCALE_BEGIN,                  ///< Rescale stage (of a message) begin event
    APPLIB_STILL_TASK_EVENT_RESCALE_END,                    ///< Rescale stage (of a message) end event

    APPLIB_STILL_TASK_EVENT_DISPLAY_ALL_CHAN_BEGIN,         ///< Display stage (of all channels) begin event
    APPLIB_STILL_TASK_EVENT_DISPLAY_ALL_CHAN_END,           ///< Display stage (of all channels) end event
    APPLIB_STILL_TASK_EVENT_DISPLAY_CHAN_BEGIN,             ///< Display stage (of a particular channel) begin event
    APPLIB_STILL_TASK_EVENT_DISPLAY_CHAN_END,               ///< Display stage (of a particular channel) end event
    APPLIB_STILL_TASK_EVENT_DISPLAY_BEGIN,                  ///< Display stage (of a message) begin event
    APPLIB_STILL_TASK_EVENT_DISPLAY_END,                    ///< Display stage (of a message) end event

    APPLIB_STILL_TASK_EVENT_SHOW_END,                       ///< (Rescale + Display) stage (of a all channels) end event

    APPLIB_STILL_VOUT_TASK_EVENT_NUM                        ///< Number of events.
} APPLIB_STILL_TASK_EVENT_ID_e;

/**
 * Event info for the following events:                     \n
 *      APPLIB_STILL_TASK_EVENT_FEED_BEGIN                  \n
 *      APPLIB_STILL_TASK_EVENT_FEED_END                    \n
 *      APPLIB_STILL_TASK_EVENT_DECODE_BEGIN                \n
 *      APPLIB_STILL_TASK_EVENT_DECODE_END                  \n
 *      APPLIB_STILL_TASK_EVENT_LOAD_END                    \n
 *      APPLIB_STILL_TASK_EVENT_SHOW_END
 */
typedef struct _APPLIB_STILL_TASK_EVENT_GENERAL_s_ {
    /**
     * Task result.
     */
    APPLIB_STILL_TASK_RESULT_e ResultCode;
} APPLIB_STILL_TASK_EVENT_GENERAL_s;

/**
 * Event info for the following events:                     \n
 *      APPLIB_STILL_TASK_EVENT_FEED_ALL_BEGIN              \n
 *      APPLIB_STILL_TASK_EVENT_FEED_ALL_END                \n
 *      APPLIB_STILL_TASK_EVENT_DECODE_ALL_BEGIN            \n
 *      APPLIB_STILL_TASK_EVENT_DECODE_ALL_END              \n
 *      APPLIB_STILL_TASK_EVENT_RESCALE_ALL_CHAN_BEGIN      \n
 *      APPLIB_STILL_TASK_EVENT_RESCALE_ALL_CHAN_END        \n
 *      APPLIB_STILL_TASK_EVENT_RESCALE_CHAN_BEGIN          \n
 *      APPLIB_STILL_TASK_EVENT_RESCALE_CHAN_END            \n
 *      APPLIB_STILL_TASK_EVENT_RESCALE_BEGIN               \n
 *      APPLIB_STILL_TASK_EVENT_RESCALE_END                 \n
 *      APPLIB_STILL_TASK_EVENT_DISPLAY_ALL_CHAN_BEGIN      \n
 *      APPLIB_STILL_TASK_EVENT_DISPLAY_ALL_CHAN_END        \n
 *      APPLIB_STILL_TASK_EVENT_DISPLAY_CHAN_BEGIN          \n
 *      APPLIB_STILL_TASK_EVENT_DISPLAY_CHAN_END            \n
 *      APPLIB_STILL_TASK_EVENT_DISPLAY_BEGIN               \n
 *      APPLIB_STILL_TASK_EVENT_DISPLAY_END
 */
typedef struct _APPLIB_STILL_TASK_EVENT_CHANNEL_s_ {
    /**
     * Task result.
     */
    APPLIB_STILL_TASK_RESULT_e ResultCode;
    /**
     * Vout channel.
     */
    UINT32 Channel;
} APPLIB_STILL_TASK_EVENT_CHANNEL_s;

/**
 * Output from "Still Task"
 */
typedef struct _APPLIB_STILL_TASK_OUTPUT_s_ {
    /**
     * Task result.
     */
    APPLIB_STILL_TASK_RESULT_e ResultCode;
    /**
     * Vout state.
     */
    APPLIB_STILL_TASK_STATE_e VoutState;
    /**
     * Width of the original image. It's also the width of the image in main buffer.
     */
    UINT32 ImageWidth;
    /**
     * Height of the original image. It's also the height of the image in main buffer.
     */
    UINT32 ImageHeight;
    /**
     * Pitch of the image in main buffer.
     */
    UINT32 ImagePitch;
    /**
     * Color format of the image in main buffer.
     */
    AMP_COLOR_FORMAT_e ImageColorFmt;
    /**
     * Chroma address of the image in main buffer.
     */
    UINT8 *ImageDecChromaAddr;
} APPLIB_STILL_TASK_OUTPUT_s;

/**
 * Feed message
 */
typedef struct _APPLIB_FEED_MSG_s_ {
    /** Filename. Full path of an image. */
    char Filename[MAX_FILENAME_LENGTH];
    /**
     * The decode source of file.                                       \n
     * 0: fullview                                                      \n
     * 1: thumbnail                                                     \n
     * 2: screennail                                                    \n
     * Comparison of resolution: fullview > screennail > thumbnail      \n
     * Comparison of decode speed: thumbnail > screennail > fullview
     */
    UINT32 FileSource;
    /**
     * Video time shift used to locate the frame to be displayed. Valid when decoding video file.
     */
    UINT32 VideoTimeShift;
    /**
     * Address of raw buffer.
     */
    void *ImageRawBuf;
    /**
     * Size of raw buffer.
     */
    UINT32 ImageRawBufSize;
} APPLIB_FEED_MSG_s;

/**
 * Decode message
 */
typedef struct _APPLIB_DECODE_MSG_s_ {
    /**
     * Address of main buffer.
     */
    UINT8 *ImageDecBuf;
    /**
     * Size of main buffer.
     */
    UINT32 ImageDecBufSize;
    /**
     * Whether rescale the image to cache buffer or not.    \n
     * 0: Don't rescale                                     \n
     * 1: Rescale
     */
    UINT8 IsRescaleToCache;
    /**
     * Area in cache buffer for storing rescaled data. Valid when IsRescaleToCache = 1.
     */
    AMP_YUV_BUFFER_s ImageCacheBuffer;
    /**
     * Rotate option while rescaling to cache. Valid when IsRescaleToCache = 1.
     */
    AMP_ROTATION_e ImageCacheRotate;
} APPLIB_DECODE_MSG_s;

/**
 * Rescale message
 */
typedef struct _APPLIB_RESCALE_MSG_s_ {
    /**
     * Buffer of rescale source.
     */
    AMP_YUV_BUFFER_s ImageSrcBuffer;
    /**
     * Buffer of rescale destination.
     */
    AMP_YUV_BUFFER_s ImageDestBuffer;
    /**
     * Rotate and flip setting.
     */
    AMP_ROTATION_e ImageRotate;
    /**
     * Luma gain while rescaling to Vout buffer.                \n
     * Adjust the luminance values (brightness) of the image.   \n
     * LumaGain =   1 ~ 127, image become darker.               \n
     * LumaGain = 128,       keep original luminance.           \n
     * LumaGain = 129 ~ 255, image become brighter.
     */
    UINT8 LumaGain;
    /**
     * Vout channel
     */
    UINT32 Channel;
} APPLIB_RESCALE_MSG_s;

/**
 * Display message
 */
typedef struct _APPLIB_DISPLAY_MSG_s_ {
    /**
     * Buffer to display.                                       \n
     * The size of AOI MUST be the same as Vout size.           \n
     * The color format MUST be 4:2:2 due to DSP restrictions.
     */
    AMP_YUV_BUFFER_s VoutBuffer;
    /**
     * Vout channel.                            \n
     * Determine which Vout to be displayed on. \n
     * DISP_CH_DCHAN: LCD, DISP_CH_FCHAN: TV
     */
    UINT32 Channel;
} APPLIB_DISPLAY_MSG_s;

/**
 * Dummy message
 */
typedef struct _APPLIB_DUMMY_MSG_s_ {
    /**
     * Vout channel.                            \n
     * Determine which Vout to be displayed on. \n
     * DISP_CH_DCHAN: LCD, DISP_CH_FCHAN: TV
     */
    UINT32 Channel;
    /**
     * The callback event of Still Task
     */
    APPLIB_STILL_TASK_EVENT_ID_e EventID;
} APPLIB_DUMMY_MSG_s;

/**
 * Union of Vout message
 */
typedef union _APPLIB_STILL_TASK_MSG_CTX_u_ {
    APPLIB_FEED_MSG_s       Feed;       /**<Feed*/
    APPLIB_DECODE_MSG_s     Decode;     /**<Decode*/
    APPLIB_RESCALE_MSG_s    Rescale;    /**<Rescale*/
    APPLIB_DISPLAY_MSG_s    Display;    /**<Display*/
    APPLIB_DUMMY_MSG_s      Dummy;      /**<Dummy*/
} APPLIB_STILL_TASK_MSG_CTX_u;

/**
 * Message as an input to "Still Task"                  \n
 * It can be either a feed message, a decode message,
 * a rescale message, or a display mesage.
 */
typedef struct _APPLIB_STILL_TASK_MSG_s_ {
    /**
     * The type of message.
     */
    APPLIB_STILL_TASK_MSG_e MessageType;
    /**
     * Still decode handler.
     */
    AMP_STLDEC_HDLR_s *StlDecHdlr;
    /**
     * Callback function that is invoked right before dealing with the first message of a specific channel.
     */
    int (*BeginCB)(AMP_STLDEC_HDLR_s *Hdlr, UINT32 EventID, void *Info);
    /**
     * Callback function that is invoked right after dealing with the last message of a specific channel.
     */
    int (*EndCB)(AMP_STLDEC_HDLR_s *Hdlr, UINT32 EventID, void *Info);
    /**
     * [Output] Output from "Still Task".
     */
    APPLIB_STILL_TASK_OUTPUT_s *Output;
    /**
     * Message context which varies according to MessageType.
     */
    APPLIB_STILL_TASK_MSG_CTX_u Message;
} APPLIB_STILL_TASK_MSG_s;

/**
 * Resource for still decode task and still display task.
 */
typedef struct _APPLIB_STILL_TASK_RESOURCE_s_{
    /**
     * Whether the decoder is initialized.  \n
     * 0: Not initialized, 1: Initialized
     */
    UINT8 IsInit;
    /**
     * Still Task.
     */
    AMBA_KAL_TASK_t VoutTask;
    /**
     * Still Vout message queue for all events.
     */
    AMP_MSG_QUEUE_HDLR_s VoutMsgQueue;
    /**
     * Still Vout message pool.
     */
    APPLIB_STILL_TASK_MSG_s *VoutMsgPool;
} APPLIB_STILL_TASK_RESOURCE_s;

/**
 * Whether the task has been initialized.
 *
 * @return 0 - Not init, Others - Already init
 */
extern int AppLibStillDec_IsTaskInitialized(void);

/**
 * Create still decode task and still display task.
 *
 * @return 0 - OK, others - Error
 */
extern int AppLibStillDec_InitTask(void);

/**
 * Delete still decode task and still display task.
 *
 * @return 0 - OK, others - Error
 */
extern int AppLibStillDec_DeinitTask(void);

/**
 * Send a display message to the still display task.
 *
 * @param [in] DispMsg         Message to task
 * @param [in] Timeout         Limitation of time (in ms) to run this function
 *
 * @return 0 - OK, -1 - The task is not initialized, others - AMP_MSG_QUEUE_RESULT_e
 */
extern int AppLibStillDec_SendVoutMsg(const APPLIB_STILL_TASK_MSG_s *DispMsg, const UINT32 Timeout);

/**
 * Reset the output of a Vout message before loading (= feeding + decoding) an image.
 *
 * @param [in,out] MsgOut      Vout message to be initialized (partially)
 *
 * @return 0 - OK, others - Error
 */
extern int AppLibStillDec_InitVoutMsgOutput_BeforeLoad(APPLIB_STILL_TASK_OUTPUT_s *MsgOut);

/**
 * Reset the output of a Vout message before showing (= rescaling + displaying) an image.
 *
 * @param [in,out] MsgOut      Vout message to be initialized (partially)
 *
 * @return 0 - OK, others - Error
 */
extern int AppLibStillDec_InitVoutMsgOutput_BeforeShow(APPLIB_STILL_TASK_OUTPUT_s *MsgOut);

#endif /* _APPLIB_PLYR_STILL_TASK_H_ */

/**
 * @}
 */     // End of group ApplibPlayer_StillTask
