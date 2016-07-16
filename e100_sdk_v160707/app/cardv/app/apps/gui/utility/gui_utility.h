/**
  * @file src/app/apps/gui/pb/gui_thumb_motion.h
  *
  *  Header of photo playback GUI display flows
  *
  * History:
  *    2013/11/08 - [Martin Lai] created file
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

#ifndef APP_GUI_UTILITY_H_
#define APP_GUI_UTILITY_H_

__BEGIN_C_PROTO__

/* Battery and power status parameters */
#define GUI_HIDE_POWER_EXCEPT_BAT   (1)
#define GUI_HIDE_POWER_EXCEPT_DC    (2)

typedef enum _GUI_UTILITY_BAT_STATE_ID_e_ {
    GUI_BATTERY_NONE = 0,
    GUI_BATTERY_EMPTY,
    GUI_BATTERY_STATE_0,
    GUI_BATTERY_STATE_1,
    GUI_BATTERY_STATE_2,
    GUI_BATTERY_STATE_3
} GUI_UTILITY_BAT_STATE_ID_e;

typedef enum _GUI_UTILITY_DC_STATE_ID_e_ {
    GUI_POWER_UNKNOWN = 0,
    GUI_POWER_ADAPTER,
    GUI_POWER_BATTERY
} GUI_UTILITY_DC_STATE_ID_e;

/* Card state parameters */
typedef enum _GUI_UTILITY_CARD_STATE_ID_e_ {
    GUI_NO_CARD = 0,
    GUI_CARD_REFRESHING,
    GUI_CARD_READY
} GUI_UTILITY_CARD_STATE_ID_e;

/* Video sensor resolution parameters */
typedef enum _GUI_UTILITY_VIDEO_RES_ID_e_ {
    GUI_VIDEO_SENSOR_RES_1 = 0,
    GUI_VIDEO_SENSOR_RES_2,
    GUI_VIDEO_SENSOR_RES_3,
    GUI_VIDEO_SENSOR_RES_4,
    GUI_VIDEO_SENSOR_RES_5,
    GUI_VIDEO_SENSOR_RES_6,
    GUI_VIDEO_SENSOR_RES_7,
    GUI_VIDEO_SENSOR_RES_8
} GUI_UTILITY_VIDEO_RES_ID_e;

/* Photo size parameters */
typedef enum _GUI_UTILITY_PHOTO_SIZE_ID_e_ {
    GUI_PHOTO_SIZE_1 = 0,
    GUI_PHOTO_SIZE_2,
    GUI_PHOTO_SIZE_3,
    GUI_PHOTO_SIZE_4,
    GUI_PHOTO_SIZE_5,
    GUI_PHOTO_SIZE_6,
    GUI_PHOTO_SIZE_7,
    GUI_PHOTO_SIZE_8,
    GUI_PHOTO_SIZE_9,
    GUI_PHOTO_SIZE_10
} GUI_UTILITY_PHOTO_SIZE_ID_e;


typedef enum _GUI_UTILITY_FILENAME_STYLE_e_ {
    GUI_PB_FN_STYLE_FULL = 0,
    GUI_PB_FN_STYLE_HYPHEN,
    GUI_PB_FN_STYLE_UNDERSCORE
} GUI_UTILITY_FILENAME_STYLE_e;

#define GUI_FILENAME_SIZE    (20)

extern int AppGuiUtil_PowerIconShow(UINT32 param1, UINT32 param2);
extern int AppGuiUtil_PowerIconHide(UINT32 param1, UINT32 param2);
extern int AppGuiUtil_PowerIconUpdate(int powerType, int batteryState);
extern int AppGuiUtil_CardIconShow(UINT32 param1, UINT32 param2);
extern int AppGuiUtil_CardIconHide(UINT32 param1, UINT32 param2);
extern int AppGuiUtil_CardIconUpdate(int state);
extern UINT32 AppGuiUtil_GetVideoResolutionBitmapSizeId(int videoRes);
extern UINT32 AppGuiUtil_GetVideoResolutionBitmapFrateARId(int videoRes);
extern int AppGuiUtil_GetFilenameStrings(WCHAR *fnGui, WCHAR *fnDmf, GUI_UTILITY_FILENAME_STYLE_e style);
__END_C_PROTO__

#endif /* APP_GUI_UTILITY_H_ */
