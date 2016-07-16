/**
 *  @file Demuxer.h
 *
 *  Demuxer header
 *
 *  @copyright 2013 Ambarella Corporation. All rights reserved.
 *                 No part of this file may be reproduced, stored in a retrieval system,
 *                 or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                 recording, or otherwise, without the prior consent of Ambarella Corporation.
 */
#ifndef __DEMUXER_H__
#define __DEMUXER_H__

/**
 * @defgroup Demuxer
 * @brief Demuxer flow implementation
 *
 * The implementation of the Demuxer module and Demuxer handler.
 * The primary function of the Demuxer module is to handle Demuxer pipes.
 * Each Demuxer pipe includes multiple formats and their media information objects.
 * Demuxer module will process the Demuxer pipes which are added to it.
 *
 * The Demuxer module includes the following functions:
 * 1. Initialize the Demuxer module
 * 2. Create a Demuxer pipe
 * 3. Delete a Demuxer pipe
 * 4. Add a Demuxer pipe to Demuxer
 * 5. Remove a Demuxer pipe from Demuxer
 * 6. Other Demuxer related functions
 *
 */

/**
 * @addtogroup Demuxer
 * @ingroup DataPipeline
 * @{
 */

#include <mw.h>
#include <fifo/Fifo.h>
#include <format/Format.h>

#define AMP_DEMUXER_MAX_FORMAT_PER_PIPE 2   /**< The number of Format handlers in a pipe */
#define AMP_DEMUXER_MAX_PIPE    16          /**< The maximum number of pipes held in the Demuxer module (The value includes the number of new tasks.) */
#define AMP_DEMUXER_MAX_TASK    8           /**< The maximum number of tasks held in the Demuxer module (Each task includes a pipe.) */

/**
 * The task mode of Demuxer
 */
typedef enum {
    AMP_DEMUXER_TASK_MODE_MAIN = 0x00,          /**< Main task mode (use a main task to process Demuxer pipes) */
    AMP_DEMUXER_TASK_MODE_STANDALONE = 0x01,    /**< Stand-alone task mode (use a stand-alone task to process a Demuxer pipe) */
    AMP_DEMUXER_TASK_MODE_CALLER = 0x02         /**< Caller task mode (use caller task to process pipes, i.e., directly execute on AmpDemuxer_OnDataRequest) */
} AMP_DEMUXER_TASK_MODE_e;

/**
 * Demuxer event
 */
typedef enum {
    AMP_DEMUXER_EVENT_START = 0x01,             /**< The event raised when Demuxer starts a demuxing */
    AMP_DEMUXER_EVENT_END = 0x02,               /**< The event raised when Demuxer completes a demuxing */
    AMP_DEMUXER_EVENT_GENERAL_ERROR = 0xF0,     /**< The event raised when a general error occurs in demuxing (not I/O error or FIFO error) */
    AMP_DEMUXER_EVENT_IO_ERROR = 0xF1,          /**< The event raised when an I/O error occurs in demuxing (e.g., stream I/O error) */
    AMP_DEMUXER_EVENT_FIFO_ERROR = 0xF2         /**< The event raised when a FIFO error occurs in demuxing (e.g., FIFO read/write error) */
} AMP_DEMUXER_EVENT_e;

/**
 * The state of Demuxer pipe (in UINT8)
 */
typedef enum {
    AMP_DEMUXER_STATE_IDLE = 0x00,              /**< The state to indicate that the Demuxer pipe is idle */
    AMP_DEMUXER_STATE_START = 0x01,             /**< The state to indicate that the Demuxer pipe starts demuxing */
    AMP_DEMUXER_STATE_RUNNING = 0x2,            /**< The state to indicate that the Demuxer pipe is running */
    AMP_DEMUXER_STATE_STOPPING = 0x3,           /**< The state to indicate that the Demuxer pipe is changing its state from running to stop */
    AMP_DEMUXER_STATE_STOPPING_ON_IDLE = 0x04,  /**< The state to indicate that the Demuxer pipe is changing its state from idle to stop */
    AMP_DEMUXER_STATE_END = 0x10,               /**< The state to indicate that the Demuxer pipe is stopped */
    AMP_DEMUXER_STATE_ERROR = 0xF0              /**< The state to indicate that the Demuxer pipe has encountered an error */
} AMP_DEMUXER_STATE_e;

/**
 * The configuration for initializing the Demuxer module
 */
typedef struct {
    UINT8 *Buffer;                  /**< The work buffer of the Demuxer module */
    UINT32 BufferSize;              /**< The size of the work buffer */
    AMP_TASK_INFO_s TaskInfo;       /**< The information of a Demuxer task */
    UINT8 MaxPipe;                  /**< The maximum number of pipes held in the Demuxer module */
    UINT8 MaxTask;                  /**< The maximum number of tasks held in the Demuxer module */
} AMP_DEMUXER_INIT_CFG_s;

/**
 * The configuration for initializing a Demuxer pipe
 */
typedef struct {
    AMP_DMX_FORMAT_HDLR_s *Format[AMP_DEMUXER_MAX_FORMAT_PER_PIPE]; /**< The Format handlers in a pipe (See AMP_DMX_FORMAT_HDLR_s.) */
    AMP_MEDIA_INFO_s *Media[AMP_DEMUXER_MAX_FORMAT_PER_PIPE];       /**< The Media Info objects in a pipe (See AMP_MEDIA_INFO_s.) */
    UINT32 TaskPriority;    /**< The task priority of a Demuxer pipe if TaskMode is AMP_DEMUXER_TASK_MODE_STANDALONE (The default value is the same as the one of the Demuxer module.) */
    AMP_CALLBACK_f OnEvent; /**< The callback function for handling Demuxer events */
    UINT32 ProcParam;       /**< The process parameters of a Demuxer pipe (In demuxing a movie and sound, the value means process duration (ms). In demuxing an image, the value means a frame number.) */
    UINT8 FormatCount;      /**< The number of Format handlers in a pipe */
    UINT8 TaskMode;         /**< The value indicating how Demuxer executes a pipe (See AMP_DEMUXER_TASK_MODE_e.) */
    UINT8 Speed;            /**< Demuxing speed (e.g., 1, 2, 4, 8, and 16) */
} AMP_DEMUXER_PIPE_CFG_s;

/**
 * Demuxer pipe handler
 */
typedef struct {
    AMP_DMX_FORMAT_HDLR_s *Format[AMP_DEMUXER_MAX_FORMAT_PER_PIPE];    /**< Format handlers in a Demuxer pipe (See AMP_MUX_FORMAT_HDLR_s.) */
    UINT8 FormatCount;      /**< The number of Format handlers in a Demuxer pipe */
} AMP_DEMUXER_PIPE_HDLR_s;

/**
 * The configuration of a Demuxer video track
 */
typedef struct {
    UINT8 Resv[4];      /**< Reserve */
} AMP_DMX_VIDEO_TRACK_CFG_s;

/**
 * The configuration of a Demuxer audio track
 */
typedef struct {
    UINT8 Resv[4];      /**< Reserve */
} AMP_DMX_AUDIO_TRACK_CFG_s;

/**
 * The configuration of a Demuxer text track
 */
typedef struct {
    UINT8 Resv[4];      /**< Reserve */
} AMP_DMX_TEXT_TRACK_CFG_s;

/**
 * The configuration of a media track for demuxing
 */
typedef struct {
    AMP_FIFO_HDLR_s *Fifo;      /**< The FIFO handler of a track (Each track has an individual FIFO handler.) */
    UINT8 *BufferBase;          /**< The start address of a FIFO buffer (Users push data into a FIFO; the FIFO will write the data to its buffer.) */
    UINT8 *BufferLimit;         /**< The end address of a FIFO buffer (FIFO size = FIFO buffer limit - FIFO buffer base) */
    union {
        AMP_DMX_VIDEO_TRACK_CFG_s Video;    /**< The information of a video track (See AMP_DMX_VIDEO_TRACK_CFG_s.) */
        AMP_DMX_AUDIO_TRACK_CFG_s Audio;    /**< The information of an audio track (See AMP_DMX_AUDIO_TRACK_CFG_s.) */
        AMP_DMX_TEXT_TRACK_CFG_s Text;      /**< The information of a text track (See AMP_DMX_TEXT_TRACK_CFG_s.) */
    } Info;
} AMP_DMX_MEDIA_TRACK_CFG_s;

/**
 * The configuration of a Movie Info object for demuxing
 */
typedef struct {
    AMP_DMX_MEDIA_TRACK_CFG_s Track[AMP_FORMAT_MAX_TRACK_PER_MEDIA]; /**< The track configurations (See AMP_MUX_MEDIA_TRACK_CFG_s.) */
    UINT32 InitTime;    /**< The initial time (ms) of the media */
} AMP_DMX_MOVIE_INFO_CFG_s;

/**
 * The configuration of an Image Info object for demuxing
 */
typedef struct {
    AMP_FIFO_HDLR_s *Fifo;      /**< The FIFO handler of image frames (Each Image Info object has an individual FIFO handler.) */
    UINT8 *BufferBase;          /**< The start address of a FIFO buffer (Users push data into a FIFO; the FIFO will write the data to its buffer.) */
    UINT8 *BufferLimit;         /**< The end address of a FIFO buffer (FIFO size = FIFO buffer limit - FIFO buffer base) */
} AMP_DMX_IMAGE_INFO_CFG_s;

/**
 * The configuration of a Sound Info object for demuxing
 */
typedef struct {
    AMP_DMX_MEDIA_TRACK_CFG_s Track[AMP_FORMAT_MAX_TRACK_PER_MEDIA]; /**< Track configurations (See AMP_MUX_MEDIA_TRACK_CFG_s.) */
    UINT32 InitTime;    /**< The initial time (ms) of the media */
} AMP_DMX_SOUND_INFO_CFG_s;

/**
 * Get the default configuration for initializing the Demuxer module.
 * @param [out] config The returned configuration
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpDemuxer_GetInitDefaultCfg(AMP_DEMUXER_INIT_CFG_s *config);

/**
 * Get the required buffer size for initializing the Demuxer module.
 * @param [in] maxPipe The maximum number of Demuxer pipes
 * @param [in] maxTask The maximum number of Demuxer tasks
 * @param [in] stackSize The stack size of each task (bytes)
 * @return The required buffer size
 */
extern UINT32 AmpDemuxer_GetRequiredBufferSize(UINT8 maxPipe, UINT8 maxTask, UINT32 stackSize);

/**
 * Initialize the Demuxer module.
 * @param [in] config The configuration used to initialize the Demuxer module
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpDemuxer_Init(AMP_DEMUXER_INIT_CFG_s *config);

/**
 * Get the default configuration for initializing Demuxer pipes.
 * @param [out] config The returned configuration
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpDemuxer_GetDefaultCfg(AMP_DEMUXER_PIPE_CFG_s *config);

/**
 * Create a Demuxer pipe.
 * @param [in] config The configuration used to create a Demuxer pipe
 * @param [out] pipe The created pipe
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 * @note Media types of Demuxer pipes indicate the kinds of media that will be processed. An image type
 * cannot appear concurrently with any other types; however, movie and sound types can appear with each other.
 */
extern int AmpDemuxer_Create(AMP_DEMUXER_PIPE_CFG_s *config, AMP_DEMUXER_PIPE_HDLR_s **pipe);

/**
 * Delete a Demuxer pipe.
 * @param [in] pipe The Demuxer pipe being deleted
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpDemuxer_Delete(AMP_DEMUXER_PIPE_HDLR_s *pipe);

/**
 * Add a Demuxer pipe to the Demuxer module
 * @param [in] pipe The Demuxer pipe being added.
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpDemuxer_Add(AMP_DEMUXER_PIPE_HDLR_s *pipe);

/**
 * Remove a Demuxer pipe
 * @oaram [in] pipe The Demuxer pipe being removed
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpDemuxer_Remove(AMP_DEMUXER_PIPE_HDLR_s *pipe);

/**
 * Start a Demuxer pipe.
 * @param [in] pipe The Demuxer pipe being started
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpDemuxer_Start(AMP_DEMUXER_PIPE_HDLR_s *pipe);

/**
 * Stop a Demuxer pipe.
 * @param [in] pipe The Demuxer pipe being stopped
 * @return 0 - OK, others - AMP_ER_CODE_e
 */
extern int AmpDemuxer_Stop(AMP_DEMUXER_PIPE_HDLR_s *pipe);

/**
 * Seek and set the start time of demuxing.
 * @param [in] pipe The Demuxer pipe
 * @param [in] targetTime The reference time being sought
 * @paran [in] direction Seek direction
 * @param [in] speed The demuxing speed after seeking.
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpDemuxer_Seek(AMP_DEMUXER_PIPE_HDLR_s *pipe, UINT32 targetTime, UINT8 direction, UINT32 speed);

/**
 * Feed a frame into a FIFO.
 * @param [in] format The Format handler
 * @param [in] trackId The ID of a track that the new frame is fed into its FIFO
 * @param [in] targetTime The reference time of the frame
 * @param [in] frameType The type of the frame
 * @return AMP_FORMAT_EVENT_e
 */
extern int AmpDemuxer_FeedFrame(AMP_DMX_FORMAT_HDLR_s *format, UINT8 trackId, UINT32 targetTime, UINT8 frameType);

/**
 * Request Demuxer to feed new frames into a FIFO.
 * @param [in] fifo The FIFO that new frames are fed to
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpDemuxer_OnDataRequest(AMP_FIFO_HDLR_s *fifo);

/**
 * Set the process parameter of a Demuxer pipe.
 * @param [in,out] pipe The pipe applying the parameters
 * @param [in] procParam The process parameter
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpDemuxer_SetProcParam(AMP_DEMUXER_PIPE_HDLR_s *pipe, UINT32 procParam);

/**
 * Poll the status of a pipe to check whether its life cycle is complete.
 * @param [in] pipe The pipe being polled
 * @param [in] timeOut The polling interval (ms)
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpDemuxer_WaitComplete(AMP_DEMUXER_PIPE_HDLR_s *pipe, UINT32 timeOut);

/**
 * Get the default configuration of a Movie Info object for demuxing.
 * @param [out] config The returned configuration
 * @param [in] movie The Movie Info object being referred
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpDemuxer_GetDefaultMovieInfoCfg(AMP_DMX_MOVIE_INFO_CFG_s *config, AMP_MOVIE_INFO_s *movie);

/**
 * Initialize a Movie Info object.
 * @param [out] movie The Movie Info object being initialized
 * @param [in] config The configuration used to initialize the Movie Info object
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpDemuxer_InitMovieInfo(AMP_MOVIE_INFO_s *movie, AMP_DMX_MOVIE_INFO_CFG_s *config);

/**
 * Get the default configuration of an Image Info object for demuxing.
 * @param [out] config The returned configuration
 * @param [in] image The Image Info object being referred
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpDemuxer_GetDefaultImageInfoCfg(AMP_DMX_IMAGE_INFO_CFG_s *config, AMP_IMAGE_INFO_s *image);

/**
 * Initialize an Image Info object.
 * @param [out] image The Image Info object being initialized
 * @param [in] config The configuration used to initialize the Image Info object
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpDemuxer_InitImageInfo(AMP_IMAGE_INFO_s *image, AMP_DMX_IMAGE_INFO_CFG_s *config);

/**
 * Get the default configuration of a Sound Info object for demuxing.
 * @param [out] config The returned configuration
 * @param [in] sound The Sound Info object being referred
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpDemuxer_GetDefaultSoundInfoCfg(AMP_DMX_SOUND_INFO_CFG_s *config, AMP_SOUND_INFO_s *sound);

/**
 * Initialize a Sound Info object.
 * @param [out] sound The Sound Info object being initialized
 * @param [in] config The configuration used to initialize the Sound Info object
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpDemuxer_InitSoundInfo(AMP_SOUND_INFO_s *sound, AMP_DMX_SOUND_INFO_CFG_s *config);

/**
 * @}
 */

#endif /* DEMUXER_H_ */

