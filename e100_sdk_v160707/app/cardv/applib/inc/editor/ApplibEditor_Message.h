/**
 * @file src/app/connected/applib/inc/editor/ApplibEditor_Message.h
 *
 * Header of editor's message.
 *
 * History:
 *    2014/01/14 - [Martin Lai] created file
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
#ifndef APPLIB_EDITOR_MSG_H_
#define APPLIB_EDITOR_MSG_H_

/**
* @defgroup ApplibEditor_Message
* @brief Editor related Message
*
*
*/

/**
 * @addtogroup ApplibEditor_Message
 * @ingroup Editor
 * @{
 */
#include <applibhmi.h>

__BEGIN_C_PROTO__


/**********************************************************************/
/* MDL_APPLIB_EDITOR_ID messsages                                        */
/**********************************************************************/
/**
* Partition: |31 - 27|26 - 24|23 - 16|15 -  8| 7 -  0|
*   |31 - 27|: MDL_APPLIB_EDITOR_ID
*   |26 - 24|: MSG_TYPE_HMI
*   |23 - 16|: module or interface type ID
*   |15 -  8|: Self-defined
*   | 7 -  0|: Self-defined
* Note:
*   bit 0-15 could be defined in the module itself (individual
*   header files). However, module ID should be defined here for arrangement
**/
#define HMSG_FORMAT_MODULE(x)  MSG_ID(MDL_APPLIB_FORMAT_ID, MSG_TYPE_HMI, (x))

#define HMSG_EDTMGR_SUCCESS        HMSG_FORMAT_MODULE(0x0000) /*< message of editor success*/
#define HMSG_EDTMGR_FAIL        HMSG_FORMAT_MODULE(0x0001) /*< message of editor fail*/
#define HMSG_EDTMGR_LIST_FULL        HMSG_FORMAT_MODULE(0x0002)/*< message of editor process list full*/
#define HMSG_EDTMGR_DISK_FULL        HMSG_FORMAT_MODULE(0x0003)/*< message of process disk editor*/
#define HMSG_EDTMGR_UNKNOWN_FORMAT    HMSG_FORMAT_MODULE(0x0004)/*< input unknown format message*/
#define HMSG_EDTMGR_ILLEGAL_INTERVAL    HMSG_FORMAT_MODULE(0x0005)/*< input time interval invalid*/
#define HMSG_EDTMGR_VIDEO_MISMATCH    HMSG_FORMAT_MODULE(0x0006)/*< input video mismatch*/
#define HMSG_EDTMGR_AUDIO_MISMATCH    HMSG_FORMAT_MODULE(0x0007)/*< input audio mismatch*/
#define HMSG_EDTMGR_EXT_MISMATCH    HMSG_FORMAT_MODULE(0x0008)/*< input ext mismatch*/
#define HMSG_EDTMGR_FORCE_STOP        HMSG_FORMAT_MODULE(0x0009)/*< force stop editor*/
#define HMSG_EDTMGR_MOVIE_RECOVER_BEGIN        HMSG_FORMAT_MODULE_MUXER(0x000A)/*< movie recover begin*/
#define HMSG_EDTMGR_MOVIE_RECOVER_COMPLETE    HMSG_FORMAT_MODULE_MUXER(0x000B)/*< movie recover finish*/

__END_C_PROTO__

/**
 * @}
 */

#endif /* APPLIB_EDITOR_MSG_H_ */

