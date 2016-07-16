#if 0
/**
 * @file src/app/connected/applib/inc/player/ApplibPlayer_Message.h
 *
 * Header of decorder's message.
 *
 * History:
 *    2013/09/14 - [Martin Lai] created file
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
 * @defgroup ApplibPlayer_Message       ApplibPlayer_Message
 * @brief Definition of decorder's message.
 *
 * Five types of the events.\n
 * 1. Sub-group.\n
 * 2. Common.\n
 * 3. Video.\n
 * 4. Photo.\n
 * 5. Thumbnail.
 */

/**
 * @addtogroup ApplibPlayer_Message
 * @ingroup ApplibPlayer
 * @{
 */

#ifndef APPLIB_DECODER_MSG_H_
#define APPLIB_DECODER_MSG_H_

#include <applibhmi.h>

__BEGIN_C_PROTO__

/**********************************************************************/
/* MDL_APPLIB_PLAYER_ID messsages                                        */
/**********************************************************************/
/**
* Partition: |31 - 27|26 - 24|23 - 16|15 -  8| 7 -  0|
*   |31 - 27|: MDL_APPLIB_PLAYER_ID
*   |26 - 24|: MSG_TYPE_HMI
*   |23 - 16|: module or interface type ID
*   |15 -  8|: Self-defined
*   | 7 -  0|: Self-defined
* Note:
*   bit 0-15 could be defined in the module itself (individual
*   header files). However, module ID should be defined here for arrangement
**/
#define HMSG_PLAYER_MODULE(x)  MSG_ID(MDL_APPLIB_PLAYER_ID, MSG_TYPE_HMI, (x))
/** Sub-group:type of interface events */
#define HMSG_PLAYER_MODULE_ID_COMMON    (0x01)    /**<HMSG_PLAYER_MODULE_ID_COMMON*/
#define HMSG_PLAYER_MODULE_ID_VIDEO     (0x02)    /**<HMSG_PLAYER_MODULE_ID_VIDEO */
#define HMSG_PLAYER_MODULE_ID_PHOTO     (0x03)    /**<HMSG_PLAYER_MODULE_ID_PHOTO */
#define HMSG_PLAYER_MODULE_ID_THUMB     (0x04)    /**<HMSG_PLAYER_MODULE_ID_THUMB */

/** Player common events */
#define HMSG_PLAYER_MODULE_COMMON(x)    HMSG_PLAYER_MODULE(((UINT32)HMSG_PLAYER_MODULE_ID_COMMON << 16) | (x))/**< HMSG_PLAYER_MODULE_COMMON(x) */
#define HMSG_PLAYER_STATE_IDLE          HMSG_PLAYER_MODULE_COMMON(0x0001) /**<HMSG_PLAYER_STATE_IDLE */

/** Video player events */
#define HMSG_PLAYER_MODULE_VIDEO(x)     HMSG_PLAYER_MODULE(((UINT32)HMSG_PLAYER_MODULE_ID_VIDEO << 16) | (x)) /**<HMSG_PLAYER_MODULE_VIDEO(x)*/
#define HMSG_PLAYER_PLY_EOS             HMSG_PLAYER_MODULE_VIDEO(0x0001)   /**<HMSG_PLAYER_PLY_EOS          */
#define HMSG_PLAYER_PLY_TIME_VALID      HMSG_PLAYER_MODULE_VIDEO(0x0002)   /**<HMSG_PLAYER_PLY_TIME_VALID   */
#define HMSG_PLAYER_PLY_TIME_INVALID    HMSG_PLAYER_MODULE_VIDEO(0x0003)   /**<HMSG_PLAYER_PLY_TIME_INVALID */
#define HMSG_PLAYER_PLY_RUN_TIME_ERR    HMSG_PLAYER_MODULE_VIDEO(0x0004)   /**<HMSG_PLAYER_PLY_RUN_TIME_ERR */

/** Photo player events */
#define HMSG_PLAYER_MODULE_PHOTO(x)     HMSG_PLAYER_MODULE(((UINT32)HMSG_PLAYER_MODULE_ID_PHOTO << 16) | (x)) /**<HMSG_PLAYER_MODULE_PHOTO(x)*/
#define HMSG_PLAYER_JPEG_OPEN_COMPLETE  HMSG_PLAYER_MODULE_PHOTO(0x0001)  /**<HMSG_PLAYER_JPEG_OPEN_COMPLETE*/
#define HMSG_PLAYER_JPEG_OPEN_FAIL      HMSG_PLAYER_MODULE_PHOTO(0x0002)  /**<HMSG_PLAYER_JPEG_OPEN_FAIL    */
#define HMSG_PLAYER_JPEG_ZOOM_COMPLETE  HMSG_PLAYER_MODULE_PHOTO(0x0003)  /**<HMSG_PLAYER_JPEG_ZOOM_COMPLETE*/
#define HMSG_PLAYER_JPEG_PIP_COMPLETE   HMSG_PLAYER_MODULE_PHOTO(0x0004)  /**<HMSG_PLAYER_JPEG_PIP_COMPLETE */

/** Thumbnail player events */
#define HMSG_PLAYER_MODULE_THUMB(x)     HMSG_PLAYER_MODULE(((UINT32)HMSG_PLAYER_MODULE_ID_THUMB << 16) | (x)) /**<HMSG_PLAYER_MODULE_THUMB(x) */
#define HMSG_PLAYER_THUMB_OPEN_COMPLETE         HMSG_PLAYER_MODULE_THUMB(0x0001)   /**<HMSG_PLAYER_THUMB_OPEN_COMPLETE    */
#define HMSG_PLAYER_THUMB_OPEN_FAIL             HMSG_PLAYER_MODULE_THUMB(0x0002)   /**<HMSG_PLAYER_THUMB_OPEN_FAIL        */
#define HMSG_PLAYER_THUMB_MOTION_COMPLETE       HMSG_PLAYER_MODULE_THUMB(0x0003)   /**<HMSG_PLAYER_THUMB_MOTION_COMPLETE  */
#define HMSG_PLAYER_THUMB_PRELOAD_COMPLETE      HMSG_PLAYER_MODULE_THUMB(0x0004)   /**<HMSG_PLAYER_THUMB_PRELOAD_COMPLETE */
#define HMSG_PLAYER_THUMB_BLEND_COMPLETE        HMSG_PLAYER_MODULE_THUMB(0x0005)   /**<HMSG_PLAYER_THUMB_BLEND_COMPLETE   */

__END_C_PROTO__

#endif /* APPLIB_DECODER_MSG_H_ */

/**
 * @}
 */     // End of group ApplibPlayer_Message
#endif
