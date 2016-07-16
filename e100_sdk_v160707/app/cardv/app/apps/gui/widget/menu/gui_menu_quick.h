/**
  * @file src/app/apps/gui/widget/menu/sportcam/gui_menu_quick.h
  *
  *  Header for Quick Menu GUI flow
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
#ifndef APP_GUI_WIDGET_MENU_QUICK_H_
#define APP_GUI_WIDGET_MENU_QUICK_H_

__BEGIN_C_PROTO__

/*************************************************************************
 * Quick Menu GUI definitions
 ************************************************************************/
typedef enum _GUI_MENU_QUICK_OPT_ID_e_ {
    GUI_OPT_1 = 0,
    GUI_OPT_2,
    GUI_OPT_3,
    GUI_OPT_4,
    GUI_OPT_5,
    GUI_OPT_NUM,
    GUI_OPT_HL
} GUI_MENU_QUICK_OPT_ID_e;

typedef enum _GUI_MENU_QUICK_BTN_ID_e_ {
    GUI_BTN_CANCEL=0,
    GUI_BTN_SET,
    GUI_BTN_UP,
    GUI_BTN_DOWN,
    GUI_BTN_NUM
} GUI_MENU_QUICK_BTN_ID_e;
typedef enum _GUI_MENU_QUICK_ICN_ID_e_ {
    GUI_ICN_DETECT_MOTION=0,
    GUI_PARKING_MONITOR,
    GUI_ICN_NUM
} GUI_MENU_QUICK_ICN_ID_e;


typedef enum _GUI_MENU_QUICK_CMD_ID_e_ {
    GUI_FLUSH = 0,
    GUI_HIDE_ALL,
    GUI_RESET,
    GUI_QMENU_SHOW,
    GUI_QMENU_HIDE,
    GUI_BTN_SHOW,
    GUI_BTN_HIDE,
    GUI_DETECT_MOTION_ICN_SHOW,
    GUI_BTN_HIGHLIGHT,
    GUI_OPT_SHOW,
    GUI_OPT_HIDE,
    GUI_OPT_UPDATE,
    GUI_OPT_HIGHLIGHT,
    GUI_DESC_SHOW,
    GUI_DESC_HIDE,
    GUI_DESC_UPDATE,
    GUI_PAGE_SHOW,
    GUI_PAGE_HIDE,
    GUI_PAGE_UPDATE,
    GUI_MENU_QUICK_BG_UPDATE_BITMAP, 
    GUI_MENU_QUICK_BG_SHOW,    
    GUI_MENU_QUICK_BG_HIDE,
    GUI_MENU_QUICK_CONF_HIGHLIGHT,
    GUI_MENU_QUICK_CONF_SHOW,    
    GUI_MENU_QUICK_CONF_HIDE,
    GUI_QUICK_BG_UPDATE_STRING,
    GUI_QUICK_BG_STRING_SHOW,
    GUI_QUICK_BG_STRING_HIDE,    
} GUI_MENU_QUICK_CMD_ID_e;

/*************************************************************************
 * Quick Menu Widget GUI functions
 ************************************************************************/
extern int gui_menu_quick_func(UINT32 guiCmd, UINT32 param1, UINT32 param2);

__END_C_PROTO__

#endif /* APP_GUI_WIDGET_MENU_QUICK_H_ */
