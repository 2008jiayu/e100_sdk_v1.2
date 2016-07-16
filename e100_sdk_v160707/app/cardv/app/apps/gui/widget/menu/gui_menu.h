/**
  * @file src/app/apps/gui/widget/menu/gui_menu.h
  *
  *  Header for Menu GUI flow
  *
  * History:
  *    2013/11/22 - [Martin Lai] created file
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

#ifndef APP_GUI_WIDGET_MENU_H_
#define APP_GUI_WIDGET_MENU_H_

__BEGIN_C_PROTO__

/*************************************************************************
 * Menu GUI definitions
 ************************************************************************/
typedef enum _GUI_MENU_TAB_ID_e_ {
    GUI_MENU_TAB_1 = 0,
    GUI_MENU_TAB_2,
    GUI_MENU_TAB_3,
    GUI_MENU_TAB_4,
    GUI_MENU_TAB_NUM
} GUI_MENU_TAB_ID_e;

typedef enum _GUI_MENU_ITEM_ID_e_ {
    GUI_MENU_ITEM_1 = 0,
    GUI_MENU_ITEM_2,
    GUI_MENU_ITEM_3,
    GUI_MENU_ITEM_4,
    GUI_MENU_ITEM_NUM
} GUI_MENU_ITEM_ID_e;

typedef enum _GUI_MENU_CMD_ID_e_ {
    GUI_FLUSH = 0,
    GUI_HIDE_ALL,
    GUI_MENU_TAB_SHOW,
    GUI_MENU_TAB_HIDE,
    GUI_MENU_TAB_UPDATE_BITMAP,
    GUI_MENU_ITEM_SHOW,
    GUI_MENU_ITEM_HIDE,
    GUI_MENU_ITEM_UPDATE_STRING,
    GUI_MENU_ITEM_UPDATE_BITMAP,
    GUI_MENU_ITEM_HIGHLIGHT,
    GUI_MENU_ITEM_LOCK,
    GUI_MENU_BG_UPDATE_BITMAP,
    GUI_MENU_BG_SHOW,    
    GUI_MENU_BG_HIDE,        
    GUI_MENU_BG_UPDATE_STRING,    
    GUI_MENU_BG_STRING_SHOW,                    
    GUI_MENU_BG_STRING_HIDE,     
    GUI_MENU_ITEM_CONF_HIGHLIGHT,    
    GUI_MENU_ITEM_CONF_SHOW,
    GUI_MENU_ITEM_CONF_HIDE,
    GUI_MENU_ITEM_ENTER_SHOW,
    GUI_MENU_ITEM_ENTER_HIDE,
    GUI_MENU_ITEM_UPDATE_ICN_ENTER,
} GUI_MENU_CMD_ID_e;

/*************************************************************************
 * Menu Widget GUI functions
 ************************************************************************/
extern int gui_menu_func(UINT32 guiCmd, UINT32 param1, UINT32 param2);

__END_C_PROTO__

#endif /* APP_GUI_WIDGET_MENU_H_ */
