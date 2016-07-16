/**
  * @file src/app/apps/gui/resource/connectedcam/gui_table.h
  *
  * Header for GUI object table
  *
  * History:
  *    2013/11/22 - [Martin Lai] created file
  *
  *
 * Copyright (c) 2015 Ambarella, Inc.
 *
 * This file and its contents (��Software��) are protected by intellectual property rights
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
#ifndef APP_GUI_TABLE_H_
#define APP_GUI_TABLE_H_

#include "gui_resource.h"

__BEGIN_C_PROTO__

typedef enum _GUI_LAYER_ID_e_ {
    GP_MAIN_0,
    GP_MAIN_1,
    GP_MAIN_2,
    GP_MAIN_3,
    GP_MENU_0,
    GP_MENU_1,
    GP_MENU_2,
    GP_SUBMENU_0,
    GP_SUBMENU_1,
    GP_SUBMENU_2
} GUI_LAYER_ID_e;


typedef enum _GUI_OBJ_ID_e_ {
    /** General */
    ///////////////////////////////
    GOBJ_TEST_STR_TITLE=0,
    GOBJ_MOTION_DETECT,
    GOBJ_CLIBRATION,
    GOBJ_TEST_BATTRY,
    GOBJ_TEST_1,
    GOBJ_TEST_2,
    GOBJ_DATE_TIME,

    GOBJ_SETTING_MENU_BG1,
    GOBJ_MENU_RECT1_1,
    GOBJ_MENU_RECT1_2,
    GOBJ_MENU_RECT1_3,
    GOBJ_MENU_RECT1_4,

    GOBJ_MENU_STRING1_1,
    GOBJ_MENU_STRING1_2,//160
    GOBJ_MENU_STRING1_3,
    GOBJ_MENU_STRING1_4,
	
    //////////////////////////////  
    GOBJ_APP_ICON,
    GOBJ_BATTERY,
    GOBJ_DC_PLUG,
    GOBJ_CARD_CURSOR,
    GOBJ_CARD,
    GOBJ_CARD_STATE,
    GOBJ_DISPLAY_ICON,
    GOBJ_MENU_ICON,
    GOBJ_RETURN_ICON,    //010
    GOBJ_MIC_ICON,
    GOBJ_WARNING,
#ifdef CONFIG_APP_ARD
    GOBJ_WARNING_2,
#endif
    GOBJ_FWUPDATE_RATIO,
    GOBJ_FWUPDATE_STAGE,
    GOBJ_USB_MSG,
    GOBJ_PRINT_MSG,
    GOBJ_PRINT_MSG_2,
#ifdef CONFIG_APP_ARD
    GOBJ_EVENT_ICON,
    GOBJ_LOOP_REC_ICON,
    GOBJ_EVENT_NUM_STR,
    GOBJ_HDR_ICON,
#endif
    /** Fullview status icons */
    GOBJ_FV_ICON_UP_1_CURSOR,
    GOBJ_FV_ICON_UP_1,
    GOBJ_FV_ICON_UP_2_CURSOR,//020
    GOBJ_FV_ICON_UP_2,
    GOBJ_FV_ICON_UP_3_CURSOR,
    GOBJ_FV_ICON_UP_3,
    GOBJ_FV_ICON_UP_4_CURSOR,
    GOBJ_FV_ICON_UP_4,
    GOBJ_FV_ICON_LEFT_1_CURSOR,
    GOBJ_FV_ICON_LEFT_1,
    GOBJ_FV_ICON_LEFT_2_CURSOR,
    GOBJ_FV_ICON_LEFT_2,
    GOBJ_FV_ICON_LEFT_3_CURSOR,//030
    GOBJ_FV_ICON_LEFT_3,
    GOBJ_FV_ICON_LEFT_4_CURSOR,
    GOBJ_FV_ICON_LEFT_4,
    GOBJ_FV_ICON_LEFT_5_CURSOR,
    GOBJ_FV_ICON_LEFT_5,

    /** Recorder mode */
    GOBJ_REC_STATE,
    GOBJ_REC_TIME,
    GOBJ_REC_EMERGENCY,

    /** Thumbnail mode */
    GOBJ_THUMB_3_TAB_BASE,
    GOBJ_THUMB_3_TAB_HL_1,
    GOBJ_THUMB_3_TAB_HL_2,//040
    GOBJ_THUMB_3_TAB_HL_3,
#ifdef CONFIG_APP_ARD
    GOBJ_THUMB_2_TAB_BASE1,
    GOBJ_THUMB_2_TAB_BASE2,
    GOBJ_THUMB_2_TAB_HL_1,
    GOBJ_THUMB_2_TAB_HL_2,
#endif
    GOBJ_THUMB_SEP_UP,
    GOBJ_THUMB_ITEM_0,
    GOBJ_THUMB_ITEM_1,
    GOBJ_THUMB_ITEM_2,
    GOBJ_THUMB_ITEM_3,
    GOBJ_THUMB_ITEM_4,
    GOBJ_THUMB_ITEM_5,    
    GOBJ_THUMB_MODE_NAME,        
    GOBJ_THUMB_FILENAME,
    /** Fullview pb mode */
    GOBJ_PB_MEDIA_RES,
    GOBJ_PB_MEDIA_AR,
    GOBJ_PB_CONTROL_BAR,//090
    GOBJ_PB_STATUS_BAR_BASE,
    GOBJ_PB_STATUS_BAR,
    GOBJ_PB_STATUS_BAR_HL,
    GOBJ_PB_STATUS_BAR_IDX,
    GOBJ_PB_PLAY_TIME,
    GOBJ_PB_TOTAL_TIME,
    GOBJ_PB_PLAY_TOTAL_TIME_SEP,
    GOBJ_PB_PLAY_STATE,
    GOBJ_PB_FILENAME,
    GOBJ_PB_ZOOM_TAG,//100
    GOBJ_PB_ZOOM_RATIO,
    GOBJ_PB_SPEED_RATIO,
    
   // GOBJ_PB_TV_FILENAME,
    /** Zoom bar */
    GOBJ_ZOOMBAR_BASE,
    GOBJ_ZOOMBAR_WIDE,    /** zoomout */
    GOBJ_ZOOMBAR_TELE,    /** zoomin */
    GOBJ_ZOOMBAR_HL,
    GOBJ_ZOOMBAR_INDEX,
    GOBJ_ZOOMBAR_TAG,    /** ozoom/dzoom */    //130
    
    /** Main menu */
    GOBJ_SETTING_MENU_BG,
    GOBJ_MENU_TAB_1,
    GOBJ_MENU_TAB_2,
    GOBJ_MENU_TAB_3,
    GOBJ_MENU_TAB_4,
    GOBJ_MENU_TAB_5,
    GOBJ_MENU_TAB_6,    
    GOBJ_MENU_RECT_1,
    GOBJ_MENU_RECT_2,
    GOBJ_MENU_RECT_3,
    GOBJ_MENU_RECT_4,
    GOBJ_MENU_CONF_RECT_1,
    GOBJ_MENU_CONF_RECT_2,
    GOBJ_MENU_CONF_RECT_3,
    GOBJ_MENU_CONF_RECT_4,    
    GOBJ_MENU_STRING_1,
    GOBJ_MENU_STRING_2,//160
    GOBJ_MENU_STRING_3,
    GOBJ_MENU_STRING_4,
    GOBJ_MENU_BG_STRING_1,
    GOBJ_MENU_BG_STRING_2,//160
    GOBJ_MENU_ICN_ENTER_1, 
    GOBJ_MENU_ICN_ENTER_2, 
    GOBJ_MENU_ICN_ENTER_3, 
    GOBJ_MENU_ICN_ENTER_4, 
    /** Quick menu */
    GOBJ_QMENU_BG,
    GOBJ_QMENU_OPT_HL,
    GOBJ_QMENU_OPT_1,
    GOBJ_QMENU_OPT_2,
    GOBJ_QMENU_OPT_3,
    GOBJ_QMENU_OPT_4,
    GOBJ_QMENU_OPT_5,
    GOBJ_QMENU_SET,//170
    GOBJ_QMENU_CANCEL,
    GOBJ_QMENU_UP,
    GOBJ_QMENU_DOWN,
    GOBJ_QMENU_DESC,
    GOBJ_QMENU_PAGE,

    /** Adjusting menu */
    GOBJ_AMENU_BAR_BASE,
    GOBJ_AMENU_BAR_HL,
    GOBJ_AMENU_BAR_IDX,
    GOBJ_AMENU_ICON_LEFT,
    GOBJ_AMENU_ICON_RIGHT,//180
    GOBJ_AMENU_SET,
    GOBJ_AMENU_CANCEL,
    GOBJ_AMENU_TITLE,
    GOBJ_AMENU_VALUE,

    /** Time setup menu */
    GOBJ_TMENU_BASE,
    GOBJ_TMENU_TITLE,
    GOBJ_TMENU_VAL_HL,
    GOBJ_TMENU_YEAR,
    GOBJ_TMENU_MONTH,
    GOBJ_TMENU_DAY,//190
    GOBJ_TMENU_HOUR,
    GOBJ_TMENU_MINUTE,
    GOBJ_TMENU_SECOND,
    GOBJ_TMENU_Y_M_SEP,
    GOBJ_TMENU_M_D_SEP,
    GOBJ_TMENU_H_M_SEP,
    GOBJ_TMENU_M_S_SEP,

    GOBJ_SURROUND_RECT_0,
    GOBJ_SURROUND_RECT_1,
    GOBJ_SURROUND_RECT_2,
    GOBJ_SURROUND_RECT_3,
    GOBJ_SURROUND_RECT_4,
    GOBJ_SURROUND_RECT_5,

    /** ADAS calibration menu */
    GOBJ_ADAS_CALIB_SKY,
    GOBJ_ADAS_CALIB_HOOD,

    // gps status
    GOBJ_GPS_STATUS,
    GOBJ_FACE_RECT_1,
    GOBJ_FACE_RECT_2,
    GOBJ_FACE_RECT_3,
    GOBJ_FACE_RECT_4,
    GOBJ_FACE_RECT_5,
    GOBJ_FACE_RECT_INFO, 

    /** Info prompt */
    GOBJ_INFO_PROMPT_BG,
    GOBJ_INFO_PROMPT_BG_STR_ITEM0,
    GOBJ_INFO_PROMPT_BG_STR_ITEM1,
    GOBJ_INFO_PROMPT_BG_STR_ITEM2,
    GOBJ_INFO_PROMPT_BG_STR_ITEM3,

    //stamp
    GOBJ_DATE,
    GOBJ_TIME_H_M,
    GOBJ_TIME_S,
    GOBJ_DRIVER_ID,
    GOBJ_ADAS_CEL,

#ifdef CONFIG_APP_ARD
    // Driver ID setup menu
    GOBJ_DRMENU_KEY_FRAME,
    GOBJ_DRMENU_TITLE,//160
    GOBJ_DRMENU_TITLE_BASE,
    GOBJ_DRMENU_VAL_HL,
    GOBJ_DRMENU_NUM_1,
    GOBJ_DRMENU_NUM_2,
    GOBJ_DRMENU_NUM_3,
    GOBJ_DRMENU_NUM_4,
    GOBJ_DRMENU_NUM_5,
    GOBJ_DRMENU_NUM_6,
    GOBJ_DRMENU_NUM_7,
    GOBJ_DRMENU_NUM_8,//170
    GOBJ_DRMENU_NUM_9,
    GOBJ_DRMENU_NUM_10,
    // Check Box setup menu
    GOBJ_CKBXMENU_BASE,
    GOBJ_CKBXMENU_TITLE,
    GOBJ_CKBXMENU_STRING_HL,
    GOBJ_CKBXMENU_STRING_1,//190
    GOBJ_CKBXMENU_STRING_2,
    GOBJ_CKBXMENU_STRING_3,
    GOBJ_CKBXMENU_BOX_1,
    GOBJ_CKBXMENU_BOX_2,
    GOBJ_CKBXMENU_BOX_3,



#endif
    GOBJ_END,    // Used for G2 ending sign
    GOBJ_NUM
}GUI_OBJ_ID_e;

#if (GUI_TABLE_SOURCE == GUI_TABLE_STATIC)
extern APPLIB_GRAPHIC_UIOBJ_s *gui_table_960x480_8bit[];
//extern APPLIB_GRAPHIC_UIOBJ_s *gui_table_960x480_16bit[];
extern APPLIB_GRAPHIC_UIOBJ_s *gui_table_960x480_32bit[];
//extern APPLIB_GRAPHIC_UIOBJ_s *gui_table_480x960_8bit[];
//extern APPLIB_GRAPHIC_UIOBJ_s *gui_table_480x960_16bit[];
//extern APPLIB_GRAPHIC_UIOBJ_s *gui_table_480x960_32bit[];
//extern APPLIB_GRAPHIC_UIOBJ_s *gui_table_1920x135_tv_8bit[];
//extern APPLIB_GRAPHIC_UIOBJ_s *gui_table_1920x135_tv_16bit[];
//extern APPLIB_GRAPHIC_UIOBJ_s *gui_table_1920x135_tv_32bit[];
extern APPLIB_GRAPHIC_UIOBJ_s *gui_table_960x540_tv_8bit[];
//extern APPLIB_GRAPHIC_UIOBJ_s *gui_table_960x540_tv_16bit[];
extern APPLIB_GRAPHIC_UIOBJ_s *gui_table_960x540_tv_32bit[];

extern APPLIB_GRAPHIC_UIOBJ_s gui_battery_960x480_32bit;
extern APPLIB_GRAPHIC_UIOBJ_s gui_app_icon_960x480_32bit;
#endif

typedef enum _GUI_BLEND_OBJ_ID_e_ {

    BOBJ_DATE_STREAM_0,
    BOBJ_DATE_STREAM_1,
    BOBJ_DATE_STREAM_2,
    BOBJ_DATE_STREAM_3,
    BOBJ_DATE_STREAM_4,
    BOBJ_DATE_STREAM_5,
    BOBJ_TIME_H_M_STREAM_0,
    BOBJ_TIME_H_M_STREAM_1,
    BOBJ_TIME_H_M_STREAM_2,
    BOBJ_TIME_H_M_STREAM_3,
    BOBJ_TIME_H_M_STREAM_4,
    BOBJ_TIME_H_M_STREAM_5,
    BOBJ_TIME_S_STREAM_0,
    BOBJ_TIME_S_STREAM_1,
    BOBJ_TIME_S_STREAM_2,
    BOBJ_TIME_S_STREAM_3,
    BOBJ_TIME_S_STREAM_4,
    BOBJ_TIME_S_STREAM_5,
#ifdef CONFIG_APP_ARD
    BOBJ_CARDV_ADMIN_ARER1_STREAM_0,
    BOBJ_CARDVE_ADMIN_ARER1_STREAM_1,
    BOBJ_CARDV_ADMIN_ARER1_STREAM_2,
    BOBJ_CARDV_ADMIN_ARER1_STREAM_3,
    BOBJ_CARDV_ADMIN_ARER1_STREAM_4,
    BOBJ_CARDV_ADMIN_ARER1_STREAM_5,
    BOBJ_CARDV_ADMIN_ARER2_STREAM_0,
    BOBJ_CARDV_ADMIN_ARER2_STREAM_1,
    BOBJ_CARDV_ADMIN_ARER2_STREAM_2,
    BOBJ_CARDV_ADMIN_ARER2_STREAM_3,
    BOBJ_CARDV_ADMIN_ARER2_STREAM_4,
    BOBJ_CARDV_ADMIN_ARER2_STREAM_5,
    BOBJ_CARDV_ADMIN_ARER3_STREAM_0,
    BOBJ_CARDV_ADMIN_ARER3_STREAM_1,
    BOBJ_CARDV_ADMIN_ARER3_STREAM_2,
    BOBJ_CARDV_ADMIN_ARER3_STREAM_3,
    BOBJ_CARDV_ADMIN_ARER3_STREAM_4,
    BOBJ_CARDV_ADMIN_ARER3_STREAM_5,
    BOBJ_CARDV_ADMIN_ARER4_STREAM_0,
    BOBJ_CARDV_ADMIN_ARER4_STREAM_1,
    BOBJ_CARDV_ADMIN_ARER4_STREAM_2,
    BOBJ_CARDV_ADMIN_ARER4_STREAM_3,
    BOBJ_CARDV_ADMIN_ARER4_STREAM_4,
    BOBJ_CARDV_ADMIN_ARER4_STREAM_5,
    BOBJ_CARDV_ADMIN_ARER5_STREAM_0,
    BOBJ_CARDV_ADMIN_ARER5_STREAM_1,
    BOBJ_CARDV_ADMIN_ARER5_STREAM_2,
    BOBJ_CARDV_ADMIN_ARER5_STREAM_3,
    BOBJ_CARDV_ADMIN_ARER5_STREAM_4,
    BOBJ_CARDV_ADMIN_ARER5_STREAM_5,
    BOBJ_CARDV_DRIVERID_STREAM_0,
    BOBJ_CARDV_DRIVERID_STREAM_1,
    BOBJ_CARDV_DRIVERID_STREAM_2,
    BOBJ_CARDV_DRIVERID_STREAM_3,
    BOBJ_CARDV_DRIVERID_STREAM_4,
    BOBJ_CARDV_DRIVERID_STREAM_5,
#endif
    BOBJ_END,    // Used for G2 ending sign
    BOBJ_NUM
}GUI_BLEND_OBJ_ID_e;

#if (GUI_TABLE_SOURCE == GUI_TABLE_STATIC)
extern APPLIB_GRAPHIC_UIOBJ_s *gui_table_blend_8bit[];
//extern APPLIB_GRAPHIC_UIOBJ_s *gui_table_blend_16bit[];
extern APPLIB_GRAPHIC_UIOBJ_s *gui_table_blend_32bit[];
#endif

__END_C_PROTO__

#endif /* APP_GUI_TABLE_H_ */
