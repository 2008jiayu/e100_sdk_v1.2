/**
* @file src/app/connected/applib/inc/format/ApplibFormat_Message.h
*
* Header of format's message.
*
* History:
*    2014/04/15 - [Martin Lai] created file
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
#ifndef APPLIB_FORMAT_MSG_H_
#define APPLIB_FORMAT_MSG_H_
/**
* @defgroup ApplibFormat_Message
* @brief format related message
*
*
*/

/**
 * @addtogroup ApplibFormat_Message
 * @ingroup Format
 * @{
 */
#include <applibhmi.h>

__BEGIN_C_PROTO__

/**********************************************************************/
/* MDL_APPLIB_FORMAT_ID messsages                                        */
/**********************************************************************/
/**
* Partition: |31 - 27|26 - 24|23 - 16|15 -  8| 7 -  0|
*   |31 - 27|: MDL_APPLIB_FORMAT_ID
*   |26 - 24|: MSG_TYPE_HMI
*   |23 - 16|: module or interface type ID
*   |15 -  8|: Self-defined
*   | 7 -  0|: Self-defined
* Note:
*   bit 0-15 could be defined in the module itself (individual
*   header files). However, module ID should be defined here for arrangement
**/
#define HMSG_FORMAT_MODULE(x)  MSG_ID(MDL_APPLIB_FORMAT_ID, MSG_TYPE_HMI, (x))
/** Sub-group:type of app library & interface events */
#define HMSG_FORMAT_MODULE_ID_MUXER             (0x01)/**<Sub-group:MUXER*/
#define HMSG_FORMAT_MODULE_ID_STREAM            (0x02)/**<Sub-group:STREAM*/
#define HMSG_FORMAT_MODULE_ID_MUX_MANAGER       (0x03)/**<Sub-group:MUX_MANAGER*/

#define HMSG_FORMAT_MODULE_MUXER(x)   HMSG_FORMAT_MODULE(((UINT32)HMSG_FORMAT_MODULE_ID_MUXER << 16) | (x))/**<HMSG_FORMAT_MODULE_MUXER */
/** The recorder status about muxer.*/
#define HMSG_MUXER_START                HMSG_FORMAT_MODULE_MUXER(0x0001)   /**<HMSG_MUXER_START         */
#define HMSG_MUXER_END                  HMSG_FORMAT_MODULE_MUXER(0x0002)   /**<HMSG_MUXER_END           */
#define HMSG_MUXER_REACH_LIMIT          HMSG_FORMAT_MODULE_MUXER(0x0003)   /**<HMSG_MUXER_REACH_LIMIT   */
#define HMSG_MUXER_PAUSE                HMSG_FORMAT_MODULE_MUXER(0x0004)   /**<HMSG_MUXER_PAUSE         */
#define HMSG_MUXER_RESUME               HMSG_FORMAT_MODULE_MUXER(0x0005)   /**<HMSG_MUXER_RESUME        */
#define HMSG_MUXER_STOP_ON_PAUSED       HMSG_FORMAT_MODULE_MUXER(0x0006)   /**<HMSG_MUXER_STOP_ON_PAUSED*/
#define HMSG_MUXER_IO_ERROR             HMSG_FORMAT_MODULE_MUXER(0x0007)   /**<HMSG_MUXER_IO_ERROR      */
#define HMSG_MUXER_FIFO_ERROR           HMSG_FORMAT_MODULE_MUXER(0x0008)   /**<HMSG_MUXER_FIFO_ERROR    */
#define HMSG_MUXER_GENERAL_ERROR        HMSG_FORMAT_MODULE_MUXER(0x0009)   /**<HMSG_MUXER_GENERAL_ERROR */
#define HMSG_MUXER_REACH_LIMIT_EVENTRECORD          HMSG_FORMAT_MODULE_MUXER(0x000A)   /**<HMSG_MUXER_REACH_LIMIT_EVENTRECORD   */
#define HMSG_MUXER_END_EVENTRECORD                  HMSG_FORMAT_MODULE_MUXER(0x000B)   /**<HMSG_MUXER_END_EVENTRECORD           */
#define HMSG_MUXER_OPEN                  HMSG_FORMAT_MODULE_MUXER(0x000C)   /**<HMSG_MUXER_OPEN           */
#ifdef CONFIG_APP_ARD
#define CURRENT_FORMAT_NORMAL 0
#define CURRENT_FORMAT_EVENT 1
#define HMSG_MUXER_IO_ERROR_EVENTRECORD  HMSG_FORMAT_MODULE_MUXER(0x000D)   /**<HMSG_MUXER_IO_ERROR_EVENTRECORD */
#define HMSG_MUXER_OPEN_EVENTRECORD      HMSG_FORMAT_MODULE_MUXER(0x000E)   /**<HMSG_MUXER_OPEN_EVENTRECORD  */
#define HMSG_MUXER_END_HAS_EMPTY_TRACK      HMSG_FORMAT_MODULE_MUXER(0x000F)   /**<HMSG_MUXER_END_HAS_EMPTY_TRACK */
#define HMSG_MUXER_FORMAT_CHANGED           HMSG_FORMAT_MODULE_MUXER(0x0010)   /**<HMSG_MUXER_FORMAT_CHANGED */
#endif

#define HMSG_FORMAT_MODULE_STREAM(x)   HMSG_FORMAT_MODULE(((UINT32)HMSG_FORMAT_MODULE_ID_STREAM << 16) | (x))/**<HMSG_FORMAT_MODULE_STREAM */
/** The recorder status about storage.*/
#define HMSG_STORAGE_RUNOUT             HMSG_FORMAT_MODULE_STREAM(0x0001)  /**<HMSG_STORAGE_RUNOUT           */
#define HMSG_STORAGE_IO_TOO_SLOW        HMSG_FORMAT_MODULE_STREAM(0x0002)  /**<HMSG_STORAGE_IO_TOO_SLOW      */
#define HMSG_STORAGE_REACH_FILE_LIMIT   HMSG_FORMAT_MODULE_STREAM(0x0003)  /**<HMSG_STORAGE_REACH_FILE_LIMIT */
#define HMSG_STORAGE_REACH_FILE_NUMBER  HMSG_FORMAT_MODULE_STREAM(0x0004)  /**<HMSG_STORAGE_REACH_FILE_NUMBER*/


/** Muxer manager module events */
/** Details in applib\format\applibformat_muxmanager.h */
#define HMSG_FORMAT_MODULE_MUX_MANAGER(x)   HMSG_FORMAT_MODULE(((UINT32)HMSG_FORMAT_MODULE_ID_MUX_MANAGER << 16) | (x))


__END_C_PROTO__
/**
 * @}
 */
#endif /* APPLIB_FORMAT_MSG_H_ */

