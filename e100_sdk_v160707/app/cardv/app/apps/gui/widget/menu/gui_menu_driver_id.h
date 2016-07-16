/**
 * system/src/app/apps/gui/widget/menu/gui_menu_driver_id.h
 *
 * Header for Driver ID Menu GUI flow
 *
 * History:
 *	  2010/08/11 - [Jili Kuang] created file
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

#ifndef __APP_GUI_WIDGET_MENU_DRIVER_ID_H__
#define __APP_GUI_WIDGET_MENU_DRIVER_ID_H__

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <AmbaDataType.h>
#include <applib.h>

__BEGIN_C_PROTO__

/*************************************************************************
 * Driver id Menu GUI definitions
 ************************************************************************/
 typedef enum _GUI_MENU_DRIVER_ID_CMD_ID_e_ {
	GUI_FLUSH = 0,
	GUI_HIDE_ALL,
	GUI_SET_LAYOUT,
	GUI_DRMENU_SHOW,
	GUI_DRMENU_HIDE,
	GUI_VALUE_SHOW,
	GUI_VALUE_HIDE,
	GUI_VALUE_HL_UPDATE_POS,
	GUI_VALUE_HL_UPDATE_ICON,
	GUI_VALUE_UPDATE,
	GUI_KEY_FRAME_SHOW,
	GUI_KEY_FRAME_HIDE,
	GUI_KEY_FRAME_UPDATE_POS,
	GUI_KEY_FRAME_UPDATE_ICON
}GUI_MENU_DRIVER_ID_CMD_ID_e;

typedef enum _GUI_MENU_DRIVER_ID_VAL_ID_e_ {
	GUI_DRIVER_ID_NUM_1 = 0,
	GUI_DRIVER_ID_NUM_2,
	GUI_DRIVER_ID_NUM_3,
	GUI_DRIVER_ID_NUM_4,
	GUI_DRIVER_ID_NUM_5,
	GUI_DRIVER_ID_NUM_6,
	GUI_DRIVER_ID_NUM_7,
	GUI_DRIVER_ID_NUM_8,
	GUI_DRIVER_ID_NUM_9,
	GUI_DRIVER_ID_NUM_10,
	GUI_DRIVER_ID_NUM_MAX
}GUI_MENU_DRIVER_ID_VAL_ID_e;

#define DRIVER_ID_RESOURCE_MAX 37
#define DRIVER_ID_RESOURCE_MIN 0

/*************************************************************************
 * Driver Id Menu Widget GUI functions
 ************************************************************************/
extern int gui_menu_driver_id_func(UINT32 gui_cmd, UINT32 param1, UINT32 param2);

__END_C_PROTO__

#endif /* __APP_GUI_WIDGET_MENU_DRIVER_ID_H__ */
