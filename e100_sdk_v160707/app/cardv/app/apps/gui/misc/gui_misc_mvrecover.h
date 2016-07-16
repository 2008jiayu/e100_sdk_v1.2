/**
  * @file src/app/apps/gui/misc/gui_misc_formatcard.h
  *
  *  Header of Movie Recover GUI display flows
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

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <AmbaDataType.h>
#include <applib.h>


#ifndef APP_GUI_MISC_MVRECOVER_H_
#define APP_GUI_MISC_MVRECOVER_H_

__BEGIN_C_PROTO__

typedef enum _GUI_MISC_MVRECOVER_GUI_CMD_e_ {
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
    GUI_WARNING_UPDATE
} GUI_MISC_MVRECOVER_GUI_CMD_e;

/* Battery and power status parameters */
#define GUI_HIDE_POWER_EXCEPT_BAT   (1)
#define GUI_HIDE_POWER_EXCEPT_DC    (2)

typedef enum _GUI_MISC_MVRECOVER_BAT_STATE_ID_e_ {
    GUI_BATTERY_NONE = 0,
    GUI_BATTERY_EMPTY,
    GUI_BATTERY_STATE_0,
    GUI_BATTERY_STATE_1,
    GUI_BATTERY_STATE_2,
    GUI_BATTERY_STATE_3
} GUI_MISC_MVRECOVER_BAT_STATE_ID_e;

typedef enum _GUI_MISC_MVRECOVER_DC_STATE_ID_e_ {
    GUI_POWER_UNKNOWN = 0,
    GUI_POWER_ADAPTER,
    GUI_POWER_BATTERY
} GUI_MISC_MVRECOVER_DC_STATE_ID_e;

/* Card state parameters */
typedef enum _GUI_MISC_MVRECOVER_CARD_STATE_ID_e_ {
    GUI_NO_CARD = 0,
    GUI_CARD_REFRESHING,
    GUI_CARD_READY
} GUI_MISC_MVRECOVER_CARD_STATE_ID_e;

extern int gui_misc_mvrecover_func(UINT32 guiCmd, UINT32 param1, UINT32 param2);

__END_C_PROTO__

#endif /* APP_GUI_MISC_MVRECOVER_H_ */
