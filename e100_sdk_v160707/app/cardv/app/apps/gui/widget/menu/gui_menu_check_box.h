/**
 * system/src/app/apps/gui/widget/menu/gui_menu_check_box.h
 *
 * Header for Check Box Menu GUI flow
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

#ifndef __APP_GUI_WIDGET_MENU_CHECK_BOX_H__
#define __APP_GUI_WIDGET_MENU_CHECK_BOX_H__

__BEGIN_C_PROTO__

extern int gui_menu_check_box_func(UINT32 gui_cmd, UINT32 param1, UINT32 param2);

typedef enum _GUI_MENU_CHECK_BOX_ITEM_ID_e_ {
	GUI_MENU_CHECK_BOX_ITEM_1 = 0,
	GUI_MENU_CHECK_BOX_ITEM_2,
	GUI_MENU_CHECK_BOX_ITEM_3,
	GUI_MENU_CHECK_BOX_ITEM_NUM,
} GUI_MENU_CHECK_BOX_ITEM_ID_e;

typedef enum _GUI_MENU_CHECK_BOX_CMD_ID_e_ {
	GUI_FLUSH = 0,
	GUI_HIDE_ALL,
	GUI_SET_LAYOUT,
	GUI_CKBXMENU_TITLE_SHOW,
	GUI_CKBXMENU_TITLE_HIDE,
	GUI_CKBXMENU_ITEM_SHOW,
	GUI_CKBXMENU_ITEM_HIDE,
	GUI_CKBXMENU_ITEM_UPDATE_STRING,
	GUI_CKBXMENU_ITEM_UPDATE_BOX,
	GUI_CKBXMENU_ITEM_HIGHLIGHT,
	GUI_CKBXMENU_ITEM_LOCK,
} GUI_MENU_CHECK_BOX_CMD_ID_e;

__END_C_PROTO__

#endif /* __APP_GUI_WIDGET_MENU_CHECK_BOX_H__ */

