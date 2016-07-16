/**
 * @file src/app/connected/applib/inc/format/ApplibFormat_MuxerManager.h
 *
 * Header of Muxer manager (for sport cam APP only)
 *
 * History:
 *    2014/01/20 - [Martin Lai] created file
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

#ifndef APPLIB_MUX_MGR_H_
#define APPLIB_MUX_MGR_H_
/**
* @defgroup ApplibFormat_MuxerManager
* @brief  Muxer manager
*
*
*/

/**
 * @addtogroup ApplibFormat_MuxerManager
 * @ingroup Format
 * @{
 */
#include <applib.h>

__BEGIN_C_PROTO__

/**
 * The handler of muxer.
 */
typedef struct _APPLIB_MUX_MGR_HANDLER_s_ {
    int (*MuxerInit)(void); /**< The initial function of muxer. */
    int (*MuxerOpen)(void); /**< The function to open muxer. */
    int (*MuxerClose)(void);/**< The function to close muxer. */
    UINT16 DataReadyNum;    /**< The number of data ready. */
    UINT8 Used;            /**< The flag that this hander us used. */
    UINT8 Reserved;            /**< The flag that this hander us used. */
    UINT32 Type;            /**< Muxer handler type */
#define STILL_MUXER_HANDLER   (0x0000)
#define VIDEO_MUXER_HANDLER   (0x0001)
#define PIV_MUXER_HANDLER   (0x0002)
#define VIDEO_EVENTRECORD_MUXER_HANDLER   (0x0003)
} APPLIB_MUX_MGR_HANDLER_s;

/** Muxer manager command messages */
#define APPLIB_MUX_MGR_EVENT_START             HMSG_FORMAT_MODULE_MUX_MANAGER(0x0001)    /**<APPLIB_MUX_MGR_EVENT_START     */
#define APPLIB_MUX_MGR_EVENT_STOP              HMSG_FORMAT_MODULE_MUX_MANAGER(0x0002)    /**<APPLIB_MUX_MGR_EVENT_STOP      */
#define APPLIB_MUX_MGR_EVENT_STILL_DATA_READY        HMSG_FORMAT_MODULE_MUX_MANAGER(0x0003)    /**<APPLIB_MUX_MGR_EVENT_STILL_DATA_READY*/
#define APPLIB_MUX_MGR_EVENT_VIDEO_DATA_READY        HMSG_FORMAT_MODULE_MUX_MANAGER(0x0004)    /**<APPLIB_MUX_MGR_EVENT_VIDEO_DATA_READY*/
#define APPLIB_MUX_MGR_EVENT_VIDEO_DATA_EOS          HMSG_FORMAT_MODULE_MUX_MANAGER(0x0005)    /**<APPLIB_MUX_MGR_EVENT_VIDEO_DATA_EOS  */
#define APPLIB_MUX_MGR_EVENT_VIDEO_DATA_READY_EVENTRECORD        HMSG_FORMAT_MODULE_MUX_MANAGER(0x0006)


/**
 *  Register Muxer Handler
 *
 *  @param [in] muxerHandler Muxer Handler.
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibFormatMuxMgr_RegMuxHandler(APPLIB_MUX_MGR_HANDLER_s *muxerHandler);

/**
 *  @brief Unregister Muxer Handler
 *
 *  Unregister Muxer Handler
 *
 *  @param [in] muxerHandler Muxer Handler.
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibFormatMuxMgr_UnRegMuxHandler(APPLIB_MUX_MGR_HANDLER_s *muxerHandler);

/**
 *   Initialization of muxer manager.
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibFormatMuxMgr_Init(void);

/**
 *  Receive the data ready event
 *
 *  @param [in] handler Handler
 *  @param [in] info Information
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibFormatMuxMgr_DataReady(void* handler, void* info, UINT32 Type);

/**
 *  Receive the data EOS event from video encode
 *
 *  @param [in] handler Handler
 *  @param [in] info Information
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibFormatMuxMgr_DataEos(void* handler, void* info);

/**
 *  Muxer start
 *
 *  @param [in] handler FIFO handler
 *  @param [in] info information
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibFormatMuxMgr_MuxStart(void* handler, void* info);

/**
 *  @brief remove semaphore after muxer close
 *
 *  remove semaphore after muxer close
 *
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibFormatMuxMgr_MuxEnd(void);

__END_C_PROTO__
/**
 * @}
 */
#endif /* APPLIB_MUX_MGR_H_ */
