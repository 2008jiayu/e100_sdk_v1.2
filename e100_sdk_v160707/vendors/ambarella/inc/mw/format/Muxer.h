/**
 *  @file Muxer.h
 *
 *  Muxer header
 *
 *  @copyright 2013 Ambarella Corporation. All rights reserved.
 *                 No part of this file may be reproduced, stored in a retrieval system,
 *                 or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                 recording, or otherwise, without the prior consent of Ambarella Corporation.
 */
#ifndef __MUXER_H__
#define __MUXER_H__

/**
 * @defgroup Muxer
 * @brief Muxer flow implementation
 *
 * The implementation of the Muxer module and Muxer handler.
 * The primary function of the Muxer module is to handle Muxer pipes.
 * Each Muxer pipe includes multiple formats and their media information objects.
 * Muxer module will process the Muxer pipes which are added to it.
 *
 * The Muxer module includes the following functions:
 * 1. Initiate the Muxer module
 * 2. Create a Muxer pipe
 * 3. Delete a Muxer pipe
 * 4. Add a Muxer pipe to Muxer
 * 5. Remove a Muxer pipe from Muxer
 * 6. Other Muxer related functions
 */

/**
 * @addtogroup Muxer
 * @ingroup DataPipeline
 * @{
 */

#include <mw.h>
#include <fifo/Fifo.h>
#include <format/Format.h>

#define AMP_MUXER_MAX_FORMAT_PER_PIPE   4   /**< The maximum number of Format handlers in a Muxer pipe */
#define AMP_MUXER_MAX_PIPE  16              /**< The maximum number of pipes held in the Muxer module (It can create 16 pipes in a Muxer handler. The number of the pipes includes the number of new tasks.) */
#define AMP_MUXER_MAX_TASK  8               /**< The maximum number of tasks held in the Muxer module (Each task includes a pipe.) */

/**
 * Muxer event (in UINT8)
 */
typedef enum {
    AMP_MUXER_EVENT_START = 0x01,           /**< The event raised when Muxer starts muxing */
    AMP_MUXER_EVENT_END = 0x02,             /**< The event raised when Muxer completes the muxing */
    AMP_MUXER_EVENT_REACH_LIMIT = 0x03,     /**< The event raised in reaching the limitation of media time or file size */
    AMP_MUXER_EVENT_IO_ERROR = 0xF0,        /**< The event raised when an I/O error occurs in muxing (e.g., stream I/O error) */
    AMP_MUXER_EVENT_FIFO_ERROR = 0xF1,      /**< The event raised when a FIFO error occurs in muxing (e.g., FIFO read/write error) */
    AMP_MUXER_EVENT_GENERAL_ERROR = 0xF2    /**< The event raised when a general error occurs in muxing (not I/O error or FIFO error) */
} AMP_MUXER_EVENT_e;

/**
 * The state of Muxer pipe
 */
typedef enum {
    AMP_MUXER_STATE_IDLE = 0x00,                /**< The state to indicate that the Muxer pipe is idle */
    AMP_MUXER_STATE_START = 0x01,               /**< The state to indicate that the Muxer pipe starts muxing  */
    AMP_MUXER_STATE_RUNNING = 0x02,             /**< The state to indicate that the Muxer pipe is running */
    AMP_MUXER_STATE_PAUSING = 0x03,             /**< The state to indicate that the Muxer pipe is going to paused */
    AMP_MUXER_STATE_PAUSED = 0x04,              /**< The state to indicate that the Muxer pipe is paused */
    AMP_MUXER_STATE_RESUME = 0x05,              /**< The state to indicate that the Muxer pipe is restart muxing */
    AMP_MUXER_STATE_STOPPING = 0x06,            /**< The state to indicate that the Muxer pipe is changing its state from running to stop */
    AMP_MUXER_STATE_STOPPING_ON_IDLE = 0x08,    /**< The state to indicate that the Muxer pipe is changing its state from idle to stop */
    AMP_MUXER_STATE_END = 0x11,                 /**< The state to indicate that the Muxer pipe is stopped */
    AMP_MUXER_STATE_ERROR = 0xF0                /**< The state to indicate that the Muxer pipe has encountered an error */
} AMP_MUXER_STATE_e;

/**
 * The configuration for initializing the Muxer module
 */
typedef struct {
    UINT8 *Buffer;              /**< The work buffer of the Muxer module */
    UINT32 BufferSize;          /**< The size of the work buffer */
    AMP_TASK_INFO_s TaskInfo;   /**< The information of a Muxer task (See AMP_TASK_INFO_s.) */
    UINT8 MaxPipe;              /**< The maximum number of pipes held in the Muxer module */
    UINT8 MaxTask;              /**< The maximum number of tasks held in the Muxer module */
} AMP_MUXER_INIT_CFG_s;

/**
 * The configuration for initializing a Muxer pipe
 */
typedef struct {
    AMP_MUX_FORMAT_HDLR_s *Format[AMP_MUXER_MAX_FORMAT_PER_PIPE];   /**< Format handlers in a pipe (See AMP_MUX_FORMAT_HDLR_s.) */
    AMP_MEDIA_INFO_s *Media[AMP_MUXER_MAX_FORMAT_PER_PIPE];         /**< Media Info objects in a pipe (See AMP_MEDIA_INFO_s.) */
    UINT32 TaskPriority;        /**< The task priority of a Muxer pipe if NewTask is TRUE (The default value is the same as the one of Muxer.) */
    AMP_CALLBACK_f OnEvent;     /**< The callback function for handling Muxer events */
    UINT32 ProcParam;           /**< The process parameters of a pipe (In muxing a movie and sound, the value means process duration. No use in muxing an image.) */
    UINT32 DelayTime;           /**< The time to delay muxing (ms, for movie and sound) */
    UINT32 MaxDuration;         /**< The maximum duration of the media (ms, for movie and sound) */
    UINT64 MaxSize;             /**< The maximum size of the media (for movie and sound) */
    UINT8 FormatCount;          /**< The number of the Format handlers in a pipe */
    BOOL8 NewTask;              /**< The flag indicating that the pipe will run in a new task (for realtime muxing) */
} AMP_MUXER_PIPE_CFG_s;

/**
 * Muxer pipe handler
 */
typedef struct {
    AMP_MUX_FORMAT_HDLR_s *Format[AMP_MUXER_MAX_FORMAT_PER_PIPE];   /**< Format handlers in a pipe (See AMP_MUX_FORMAT_HDLR_s.) */
    UINT8 FormatCount;          /**< The number of Format handlers in a pipe */
} AMP_MUXER_PIPE_HDLR_s;

/**
 * The configuration of a video track for muxing
 */
typedef struct {
    UINT32 GOPSize;             /**< The number of pictures between IDR pictures */
    UINT32 CodecTimeScale;      /**< The time scale of the codec (TODO: It needs to be got from H264 bitstream; it is not configurable.) */
    UINT16 Width;               /**< Picture width */
    UINT16 Height;              /**< Picture height */
    UINT16 M;                   /**< The number of pictures between reference pictures (IDR, I, P) */
    UINT16 N;                   /**< The number of pictures between I pictures */
    BOOL8 IsDefault;            /**< The flag indicating the track is the default video track */
    UINT8 Mode;                 /**< The picture mode of a video (It has progressive and interlaced mode. Interlaced mode has Field Per Sample and Frame Per Sample. See AMP_VIDEO_MODE_s.) */
    BOOL8 ClosedGOP;            /**< The flag indicating if a GOP structure is closed (The sequence pattern of a closed GOP structure is IPBBPBB, and the one of an open GOP structure is IBBPBB. If the functions of resuming or auto splitting a video are enabled, the value is always false, open GOP.) */
} AMP_MUX_VIDEO_TRACK_CFG_s;

/**
 * The configuration of an audio track for muxing
 */
typedef struct {
    UINT32 SampleRate;          /**< The sample rate (Hz) of an audio track */
    BOOL8 IsDefault;            /**< The flag indicating the track is the default audio track  */
    UINT8 Channels;             /**< The number of audio channels */
    UINT8 BitsPerSample;        /**< Bits per audio sample (e.g., 8 bits and 16 bits) */
} AMP_MUX_AUDIO_TRACK_CFG_s;

/**
 * The configuration of a text track for muxing
 */
typedef struct {
    BOOL8 IsDefault;            /**< The flag indicating the track is the default text track  */
} AMP_MUX_TEXT_TRACK_CFG_s;

/**
 * The configuration of a media track for muxing
 */
typedef struct {
    UINT32 MediaId;             /**< The media type of a track (The id is a media ID. See AMP_FORMAT_MID_e.) */
    UINT32 TimeScale;           /**< The ticks per second */
    UINT32 TimePerFrame;        /**< The ticks per frame */
    UINT32 InitDelay;           /**< Initial delay time of a track (ms) */
    AMP_FIFO_HDLR_s *Fifo;      /**< The FIFO handler of a track (Each track has an individual FIFO handler.) */
    UINT8 *BufferBase;          /**< The start address of a FIFO buffer (Users push data into a FIFO; the FIFO will write the data to its buffer.) */
    UINT8 *BufferLimit;         /**< The end address of a FIFO buffer (FIFO size = FIFO buffer limit - FIFO buffer base) */
    union {
        AMP_MUX_VIDEO_TRACK_CFG_s Video;    /**< The information of a video track (See AMP_VIDEO_TRACK_INFO_s.) */
        AMP_MUX_AUDIO_TRACK_CFG_s Audio;    /**< The information of an audio track (See AMP_AUDIO_TRACK_INFO_s.) */
        AMP_MUX_TEXT_TRACK_CFG_s Text;      /**< The information of a text track (See AMP_TEXT_TRACK_INFO_s.) */
    } Info;
    UINT8 TrackType;            /**< Track type (See AMP_MEDIA_TRACK_TYPE_e.) */
} AMP_MUX_MEDIA_TRACK_CFG_s;

/**
 * The configuration of a Movie Info object for muxing
 */
typedef struct {
    AMP_MUX_MEDIA_TRACK_CFG_s Track[AMP_FORMAT_MAX_TRACK_PER_MEDIA];    /**< Track configurations (See AMP_MUX_MEDIA_TRACK_CFG_s.) */
    UINT8 TrackCount;       /**< The number of tracks in a Movie Info object */
} AMP_MUX_MOVIE_INFO_CFG_s;

/**
 * The configuration of an Image Frame Info object for muxing
 */
typedef struct {
    UINT32 SeqNum;          /**< The sequence number of an image */
    UINT32 Type;            /**< Image type */
    UINT16 Width;           /**< Image width */
    UINT16 Height;          /**< Image height */
    EXIF_INFO_s ExifInfo;   /**< EXIF information (See EXIF_INFO_s.) */
} AMP_MUX_IMAGE_FRAME_INFO_CFG_s;

/**
 * The configuration of an Image Info object for muxing
 */
typedef struct {
    AMP_FIFO_HDLR_s *Fifo;  /**< The FIFO handler of image frames (Each Image Info object has an individual FIFO handler.) */
    UINT8 *BufferBase;      /**< The start address of a FIFO buffer (Users push data into a FIFO; the FIFO will write the data to its buffer.) */
    UINT8 *BufferLimit;     /**< The end address of a FIFO buffer (FIFO size = FIFO buffer limit - FIFO buffer base) */
    AMP_MUX_IMAGE_FRAME_INFO_CFG_s Frame[AMP_FORMAT_MAX_FRAME_PER_IMAGE];   /**< The frames in an Image Info object (See AMP_MUX_IMAGE_FRAME_INFO_CFG_s.) */
    UINT8 UsedFrame;        /**< The number of frames stored in an image file */
    UINT8 TotalFrame;       /**< The total number of frames that this image would reference */
} AMP_MUX_IMAGE_INFO_CFG_s;

/**
 * The configuration of a Sound Info object for muxing
 */
typedef struct {
    AMP_MUX_MEDIA_TRACK_CFG_s Track[AMP_FORMAT_MAX_TRACK_PER_MEDIA];    /**< Track configurations (See AMP_MUX_MEDIA_TRACK_CFG_s.) */
    UINT8 TrackCount;   /**< The number of tracks in a Sound Info object */
} AMP_MUX_SOUND_INFO_CFG_s;

/**
 * Get the default configuration for initializing the Muxer module.
 * @param [out] config The returned configuration
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpMuxer_GetInitDefaultCfg(AMP_MUXER_INIT_CFG_s *config);

/**
 * Get the required buffer size for initializing the Muxer module.
 * @param [in] maxPipe The maximum number of Muxer pipes
 * @param [in] maxTask The maximum number of Muxer tasks
 * @param [in] stackSize The stack size of each task (byte)
 * @return The required buffer size
 */
extern UINT32 AmpMuxer_GetRequiredBufferSize(UINT8 maxPipe, UINT8 maxTask, UINT32 stackSize);

/**
 * Initialize the Muxer module.
 * @param [in] config The configuration used to initialize the Muxer module
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpMuxer_Init(AMP_MUXER_INIT_CFG_s *config);

/**
 * Get the default configuration for initializing Muxer pipes.
 * @param [out] config The returned configuration
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpMuxer_GetDefaultCfg(AMP_MUXER_PIPE_CFG_s *config);

/**
 * Create a Muxer pipe.
 * @param [in] config The configuration used to create a Muxer pipe
 * @param [out] pipe The created pipe
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 * @note Media types of Muxer pipes indicate the kinds of media that will be processed. An image type
 * cannot appear concurrently with any other types; however, movie and sound types can appear with each other.
 */
extern int AmpMuxer_Create(AMP_MUXER_PIPE_CFG_s *config, AMP_MUXER_PIPE_HDLR_s **pipe);

/**
 * Delete a Muxer pipe.
 * @param [in] pipe The Muxer pipe being deleted
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpMuxer_Delete(AMP_MUXER_PIPE_HDLR_s *pipe);

/**
 * Add a Muxer pipe to Muxer.
 * @param [in] pipe The Muxer pipe being added
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpMuxer_Add(AMP_MUXER_PIPE_HDLR_s *pipe);

/**
 * Remove a Muxer pipe.
 * @param [in] pipe The Muxer pipe being removed
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpMuxer_Remove(AMP_MUXER_PIPE_HDLR_s *pipe);

/**
 * Start a Muxer pipe. After invoking the function, the start will be postponed until the first frame is available.
 * @param [in] pipe The Muxer pipe being started
 * @param [in] timeOut The timeout value
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpMuxer_Start(AMP_MUXER_PIPE_HDLR_s *pipe, UINT32 timeOut);

/**
 * Stop a Muxer pipe. Because a clip would be ended at the last available IDR/I/P, after invoking the function, the stop will be postponed until all frames are processed.
 * @param [in] pipe The Muxer pipe being stopped
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpMuxer_Stop(AMP_MUXER_PIPE_HDLR_s *pipe);

/**
 * Notify Muxer of new frames being available in a FIFO.
 * @param [in] fifo The handler of a FIFO holding new frames
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpMuxer_OnDataReady(AMP_FIFO_HDLR_s *fifo);

/**
 * Check if a FIFO has an EOS frame.
 * @param [in] fifo The handler of a FIFO being checked
 * @return TRUE or FALSE;
 */
extern BOOL AmpMuxer_HasEOS(AMP_FIFO_HDLR_s *fifo);

/**
 * Notify Muxer that an EOS frame has appeared in a FIFO.
 * @param [in] fifo The handler of a FIFO in which an EOS frame appears
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpMuxer_OnEOS(AMP_FIFO_HDLR_s *fifo);

/**
 * Set process parameters of a Muxer pipe.
 * @param [in] pipe The pipe applying the parameter
 * @param [in] procParam The process parameter
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpMuxer_SetProcParam(AMP_MUXER_PIPE_HDLR_s *pipe, UINT32 procParam);

/**
 * Poll the status of a pipe to check whether its life cycle is complete.
 * @param [in] pipe The pipe being polled
 * @param [in] timeOut The polling interval (ms)
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpMuxer_WaitComplete(AMP_MUXER_PIPE_HDLR_s *pipe, UINT32 timeOut);

/**
 * Get the default configuration of a Movie Info object.
 * @param [out] config The returned configuration
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpMuxer_GetDefaultMovieInfoCfg(AMP_MUX_MOVIE_INFO_CFG_s *config);

/**
 * Initialize a Movie Info object.
 * @param [in, out] movie The Movie Info object being initialized
 * @param [in] config The configuration used to initialize a Movie Info object
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpMuxer_InitMovieInfo(AMP_MOVIE_INFO_s *movie, AMP_MUX_MOVIE_INFO_CFG_s *config);

/**
 * Get the default configuration of an Image Info object.
 * @param [out] config The returned configuration
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpMuxer_GetDefaultImageInfoCfg(AMP_MUX_IMAGE_INFO_CFG_s *config);

/**
 * Initialize an Image Info object.
 * @param [in, out] image The Image Info object being initialized
 * @param [in] config The configuration used to initialize an Image Info object
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpMuxer_InitImageInfo(AMP_IMAGE_INFO_s *image, AMP_MUX_IMAGE_INFO_CFG_s *config);

/**
 * Get the default configuration of a Sound Info object
 * @param [out] config The returned configuration
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpMuxer_GetDefaultSoundInfoCfg(AMP_MUX_SOUND_INFO_CFG_s *config);

/**
 * Initialize a Sound Info object.
 * @param [in, out] sound The Sound Info object being initialized
 * @param [in] config The configuration used to initialize a Sound Info object
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpMuxer_InitSoundInfo(AMP_SOUND_INFO_s *sound, AMP_MUX_SOUND_INFO_CFG_s *config);

/**
 * Lock a Muxer pipe.
 * @param [in] pipe The Muxer pipe being locked
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpMuxer_LockPipe(AMP_MUXER_PIPE_HDLR_s *pipe);

/**
 * Unlock a Muxer pipe.
 * @param [in] pipe The Muxer pipe being unlocked
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpMuxer_UnlockPipe(AMP_MUXER_PIPE_HDLR_s *pipe);

/**
 * Set the delay time of a Muxer pipe.
 * @param [in] pipe The pipe applying the delay time
 * @param [in] delayTime The delay time (ms)
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpMuxer_SetDelayTime(AMP_MUXER_PIPE_HDLR_s *pipe, UINT32 delayTime);

/**
 * Set the maximum duration of a Muxer pipe.
 * @param [in] pipe The pipe applying the maximum duration
 * @param [in] maxDuration The maximum duration (ms)
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpMuxer_SetMaxDuration(AMP_MUXER_PIPE_HDLR_s *pipe, UINT32 maxDuration);

/**
 * @}
 */

#endif /* MUXER_H_ */

