/**
 * @file src/app/connected/applib/inc/applibhmi.h
 *
 * User-defined HMI messages
 *
 * History:
 *    2013/07/05 - [Martin Lai] created file
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

#include "AmbaCardManager.h"

#ifndef APP_APPLIBHMI_H_
#define APP_APPLIBHMI_H_

/**
*  Defines for Module ID
*/
#define MDL_APPLIB_SYSTEM_ID    0x01
#define MDL_APPLIB_RECORDER_ID    0x02
#define MDL_APPLIB_PLAYER_ID    0x03
#define MDL_APPLIB_AUDIO_ID    0x04
#define MDL_APPLIB_MONITOR_ID    0x05
#define MDL_APPLIB_DCF_ID        0x06
#define MDL_APPLIB_STORAGE_ID    0x07
#define MDL_APPLIB_STREAM_ID    0x08
#define MDL_APPLIB_FORMAT_ID    0x09
#define MDL_APPLIB_EDITOR_ID    0x0A
#define MDL_APPLIB_DISPLAY_ID    0x0B
#define MDL_APPLIB_GRAPHICS_ID    0x0C
#define MDL_APPLIB_IMAGE_ID    0x0D
#define MDL_APPLIB_3A_ID        0x0E
#define MDL_APPLIB_TUNE_ID        0x0F
#define MDL_APPLIB_CALIB_ID    0x11
#define MDL_APPLIB_USB_ID        0x12
#define MDL_APPLIB_COMSVC_ID    0x13
#define MDL_APPLIB_UTILITY_ID    0x14
#define MDL_APPLIB_THIRD_ID    0x15
#define MDL_APPLIB_VA_ID       0x16

/**
* MDL_APP_KEY_ID 0x1B is defined for the message group of netwok control.
**/
#define MDL_APP_NET_ID          0x1B


/**
* MDL_APP_KEY_ID 0x1B is defined for the message group of user
* operation input, such as buttons, IR remote, etc.
**/
#define MDL_APP_KEY_ID          0x1C

/**
* MDL_APP_JACK_ID 0x1C is defined for the message group of
* device peripheral jack, such as vin/vout jack, ain/aout jack,
* card jack, etc.
**/
#define MDL_APP_JACK_ID         0x1D


/**
* MDL_APP_FLOW_ID 0x1E is defined for the message group of
* application flows and test flows, such as app state messages,
* app command messages, and app test messages
**/
#define MDL_APP_FLOW_ID         0x1E

/**
*  Defines for Message Type
*/
#define MSG_TYPE_HMI            0x00
#define MSG_TYPE_CMD            0x01
#define MSG_TYPE_PARAM          0x02
#define MSG_TYPE_ERROR          0x03

#define MSG_ID(mdl_id, msg_type, msg_par)       (((mdl_id & 0x0000001F) << 27) | ((msg_type & 0x00000007) << 24) | (msg_par & 0x00FFFFFF))
#define MSG_MDL_ID(id)          ((id & 0xF8000000)>>27)
#define MSG_TYPE(id)            ((id & 0x07000000)>>24)

#define MSG_NULL_ID             0x00000000

#endif /* APP_APPLIBHMI_H_ */
