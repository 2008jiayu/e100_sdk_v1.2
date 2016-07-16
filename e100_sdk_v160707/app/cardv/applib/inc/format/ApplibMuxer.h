/**
 * @file app/connected/applib/inc/format/ApplibMuxer.h
 *
 * Muxer manager implementation (for demo APP only)
 *
 * History:
 *    2013/11/13 - [clchan  ] created file
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
#ifndef __APPLIBMUXER_H__
#define __APPLIBMUXER_H__
/**
* @defgroup ApplibFormat_Muxer
* @brief Muxer format
*
*
*/

/**
 * @addtogroup ApplibFormat_Muxer
 * @ingroup Format
 * @{
 */
#include <format/Muxer.h>

#define APPLIB_MUXER_MAX_PIPE   4   /**< hard limit : AMP_MUXER_MAX_PIPE */
#define APPLIB_MUXER_MAX_TASK   2   /**< hard limit : AMP_MUXER_MAX_TASK */
#define APPLIB_MUXER_PRIORITY   81  /**<muxer priority*/
#define APPLIB_MUXER_STACK_SIZE 0x4000 /**<muxer stack size*/
/*!
 * muxer event enum
 */
typedef enum {
    APPLIB_MUXER_EVENT_START,
    APPLIB_MUXER_EVENT_END,
    APPLIB_MUXER_EVENT_AUTO_SPLIT,
#if 0
    APPLIB_MUXER_EVENT_PAUSE,
    APPLIB_MUXER_EVENT_RESUME,
#endif
    APPLIB_MUXER_EVENT_IO_ERROR,
    APPLIB_MUXER_EVENT_FIFO_ERROR,
    APPLIB_MUXER_EVENT_GENERAL_ERROR
} APPLIB_MUXER_EVENT_e;

/*!
 * the auto split mode (in UINT8)
 */
typedef enum {
    APPLIB_MUXER_SPLIT_OFF = 0x00,          /**< do nothing, just invoke event callback */
    APPLIB_MUXER_SPLIT_CONNECTED = 0x01,    /**< keep DTS continuous */
    APPLIB_MUXER_SPLIT_NEW_SESSION = 0x02   /**< reset DTS */
} APPLIB_MUXER_SPLIT_MODE_e;

/**
 *
 *muxer initialization config
 *
 */
typedef struct {
    UINT8 *Buffer; /**< buffer*/
    UINT32 BufferSize; /**< buffer size*/
} APPLIB_MUXER_INIT_CFG_s;
/**
 *
 *muxer pipe handler
 *
 */
typedef struct {
    UINT8 Resv[4]; /**<resv*/
} APPLIB_MUXER_PIPE_HDLR_s;

typedef int (*APPLIB_MUXER_GET_NAME_FP)(APPLIB_MUXER_PIPE_HDLR_s *, AMP_MUX_FORMAT_HDLR_s *, char *, UINT32);/**<muxer get name API*/
typedef int (*APPLIB_MUXER_ON_EVENT_FP)(APPLIB_MUXER_PIPE_HDLR_s *, UINT32);/**<muxer ON EVENT API*/
/**
 *
 *muxer media config
 *
 */
typedef struct {
    char Name[MAX_FILENAME_LENGTH]; /**<sz name*/
    union {
        AMP_MUX_MOVIE_INFO_CFG_s Movie;
        AMP_MUX_IMAGE_INFO_CFG_s Image;
        AMP_MUX_SOUND_INFO_CFG_s Sound;
    } Info; /**<info */
    UINT8 MediaType; /**<Media Type*/
} APPLIB_MUXER_MEDIA_CFG_s;
/**
 *
 *muxer pipe config
 *
 */
typedef struct {
    UINT32 MaxDuration;    /**< the max duration of a clip, would trigger auto split */
    UINT64 MaxSize;        /**< the max size of a clip, would trigger auto split */
    UINT8 SplitMode;       /**< APPLIB_MUXER_SPLIT_MODE_e */
} APPLIB_MUXER_AUTO_SPLIT_CFG_s;
/**
 *
 *muxer pipe config
 *
 */
typedef struct {
    UINT32 TaskPriority;   /**< the task priority (if bNewTask is enabled) */
    BOOL8 NewTask; /**<new task to do or not*/
} APPLIB_MUXER_TASK_CFG_s;
/**
 *
 *muxer pipe config
 *
 */
typedef struct {
    AMP_MUX_FORMAT_HDLR_s *Format[AMP_MUXER_MAX_FORMAT_PER_PIPE];   /**< formats */
    APPLIB_MUXER_GET_NAME_FP GetName;   /**< the callback function to get the filename */
    APPLIB_MUXER_ON_EVENT_FP OnEvent;   /**< the callback function to pass event to APP */
    APPLIB_MUXER_MEDIA_CFG_s MediaCfg[AMP_MUXER_MAX_FORMAT_PER_PIPE];   /**< media info of each format */
    APPLIB_MUXER_TASK_CFG_s TaskCfg;    /**<task config*/
    APPLIB_MUXER_AUTO_SPLIT_CFG_s SplitCfg; /**<split config*/
    UINT8 FormatCount;                      /**< number of muxer formats */
} APPLIB_MUXER_PIPE_CFG_s;

/**
 * Get the default muxer manager config
 * @param [out] Config the buffer to get default config
 * @return 0=>ok, -1=>NG
 */
extern int ApplibMuxer_GetInitDefaultCfg(APPLIB_MUXER_INIT_CFG_s *Config);

/**
 * Initiate the core of MMGR
 * @param [in] Config the init config
 * @return 0=>ok, -1=>NG
 */
extern int ApplibMuxer_Init(APPLIB_MUXER_INIT_CFG_s *Config);

/**
 * Get the default muxer pipe config
 * @param [out] Config the buffer to get default config
 * @return 0=>ok, -1=>NG
 */
extern int ApplibMuxer_GetDefaultCfg(APPLIB_MUXER_PIPE_CFG_s *Config);

/**
 * Create a mux pipe
 * @param [in] Config the config of the muxer
 * @param [out] Pipe the double pointer to get the pipe
 * @return 0=>ok, -1=>NG
 */
extern int ApplibMuxer_Create(APPLIB_MUXER_PIPE_CFG_s *Config, APPLIB_MUXER_PIPE_HDLR_s **Pipe);

/**
 * Delete a muxer pipe
 * @param [in] Pipe the muxer pipe to close
 * @return 0=>ok, -1=>NG
 */
extern int ApplibMuxer_Delete(APPLIB_MUXER_PIPE_HDLR_s *Pipe);

/**
 * Start a muxer pipe (no wait complete)
 * @param [in] Pipe the muxer pipe to start
 * @return 0=>ok, -1=>NG
 */
extern int ApplibMuxer_Start(APPLIB_MUXER_PIPE_HDLR_s *Pipe);

/**
 * Stop the muxer pipe (stop at the last IDR/I/P)
 * @param [in] Pipe the muxer to stop
 * @return 0=>ok, -1=>NG
 */
extern int ApplibMuxer_Stop(APPLIB_MUXER_PIPE_HDLR_s *Pipe);

/**
 * Start pre-record
 * @param [in] Pipe the muxer pipe to pre-record
 * @param [in] Length the length to pre-record (in ms)
 * @return 0=>ok, -1=>NG
 */
extern int ApplibMuxer_Prerecord(APPLIB_MUXER_PIPE_HDLR_s *Pipe, UINT32 Length);

/**
 * wait a pipe to complete (EOS)
 * @param [in] Pipe the pipe
 * @param [in] TimeOut the timeout value
 * @return 0=>ok, -1=>NG
 */
extern int ApplibMuxer_WaitComplete(APPLIB_MUXER_PIPE_HDLR_s *Pipe, UINT32 TimeOut);

/**
 * the function to notify that new frames are available
 * @param [in] Fifo the fifo
 * @return 0=>ok, -1=>NG
 */
extern int ApplibMuxer_OnDataReady(AMP_FIFO_HDLR_s *Fifo);

/**
 * the function to notify that EOS frame has in buffer
 * @param [in] Fifo the fifo
 * @return 0=>ok, -1=>NG
 */
extern int ApplibMuxer_OnEOS(AMP_FIFO_HDLR_s *Fifo);
/**
 * @}
 */
#endif

