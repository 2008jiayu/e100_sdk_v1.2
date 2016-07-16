/**
  * @file src/app/apps/gui/rec/gui_pb_multi.h
  *
  *  Header of video playback GUI display flows
  *
  * History:
  *    2014/09/16 - [Annie Ting] created file
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

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <AmbaDataType.h>
#include <applib.h>
#include <apps/apps.h>
#include <apps/gui/utility/gui_utility.h>

#ifndef APP_GUI_PB_MULTI_H_
#define APP_GUI_PB_MULTI_H_

__BEGIN_C_PROTO__

typedef enum _GUI_PB_MULTI_GUI_CMD_e_ {
    GUI_FLUSH = 0,
    GUI_HIDE_ALL,
    GUI_SET_LAYOUT,
    GUI_APP_ICON_SHOW,
    GUI_APP_ICON_HIDE,
    GUI_POWER_STATE_SHOW,
    GUI_POWER_STATE_HIDE,
    GUI_POWER_STATE_UPDATE,
    GUI_CARD_SHOW,
    GUI_CARD_HIDE,
    GUI_CARD_UPDATE,
    GUI_WARNING_SHOW,
    GUI_WARNING_HIDE,
    GUI_WARNING_UPDATE,
    GUI_PLAY_STATE_SHOW,
    GUI_PLAY_STATE_HIDE,
    GUI_PLAY_STATE_UPDATE,
    GUI_PLAY_TIMER_SHOW,
    GUI_PLAY_TIMER_HIDE,
    GUI_PLAY_TIMER_UPDATE,
    GUI_PLAY_SPEED_SHOW,
    GUI_PLAY_SPEED_HIDE,
    GUI_FILENAME_SHOW,
    GUI_FILENAME_HIDE,
    GUI_FILENAME_UPDATE,
    GUI_MEDIA_INFO_SHOW,
    GUI_MEDIA_INFO_HIDE,
    GUI_MEDIA_INFO_UPDATE,
    GUI_VIDEO_SENSOR_RES_SHOW,
    GUI_VIDEO_SENSOR_RES_HIDE,
    GUI_VIDEO_SENSOR_RES_UPDATE,
    GUI_ZOOMBAR_SHOW,
    GUI_ZOOMBAR_HIDE,
    GUI_ZOOMBAR_UPDATE
} GUI_PB_MULTI_GUI_CMD_e;

/* Battery and power status parameters */
#define GUI_HIDE_POWER_EXCEPT_BAT   (1)
#define GUI_HIDE_POWER_EXCEPT_DC    (2)

/** Video Play state parameters */
typedef enum _GUI_PB_MULTI_PLAY_STATE_ID_e_ {
    GUI_PAUSE = 0,
    GUI_FWD_NORMAL,
    GUI_FWD_FAST,
    GUI_FWD_FAST_END,
    GUI_FWD_SLOW,
    GUI_FWD_STEP,
    GUI_REW_NORMAL,
    GUI_REW_FAST,
    GUI_REW_FAST_END,
    GUI_REW_SLOW,
    GUI_REW_STEP
} GUI_PB_MULTI_PLAY_STATE_ID_e;

typedef enum _APPLIB_VIDEO_PLAYER_STATE_e_ {
    APPLIB_VIDEO_PLAYER_STATE_INVALID = 0,  ///< Not ready.
    APPLIB_VIDEO_PLAYER_STATE_IDLE,         ///< Ready to load a file.
    APPLIB_VIDEO_PLAYER_STATE_PLAY,         ///< Video is playing.
    APPLIB_VIDEO_PLAYER_STATE_PAUSE,        ///< Video is paused.
    APPLIB_VIDEO_PLAYER_STATE_PAUSE_CHANGE, ///< Video is paused with some features (ex. speed, start time...) changed.
    APPLIB_VIDEO_PLAYER_STATE_NUM           ///< Total number of video states.
} APPLIB_VIDEO_PLAYER_STATE_e;

#ifdef CONFIG_APP_ARD
/** Play speed definitions */
#define PBACK_SPEED_NORMAL        (0x1 <<  8)    /** 0x0100 */
#define PBACK_SPEED_MIN            (0x1 <<  2)    /** 0x0004 */
#define PBACK_SPEED_MAX            (0x1 << 14)    /** 0x4000 */
#endif

extern int gui_pb_multi_func(UINT32 guiCmd, UINT32 param1, UINT32 param2);

__END_C_PROTO__

#endif /* APP_GUI_PB_MULTI_H_ */
