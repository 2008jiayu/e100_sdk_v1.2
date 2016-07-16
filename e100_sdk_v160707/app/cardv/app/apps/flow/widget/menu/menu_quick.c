/**
  * @file src/app/apps/flow/widget/menu/connectedcam/menu_quick.c
  *
  * Implementation of Quick Menu
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

#include "menu_adjust.h"
#include <apps/flow/widget/menu/menu.h>
#include <apps/gui/widget/menu/gui_menu_quick.h>
#include <system/app_pref.h>
#include <apps/flow/widget/menu/widget.h>

/*************************************************************************
 * Quick Menu definitions
 ************************************************************************/
typedef enum _MENU_QUICK_FUNC_ID_e_{
    MENU_QUICK_CAL_SEL_DISP_PARAM = 0,
    MENU_QUICK_SWITCH_TO_SEL,
    MENU_QUICK_CUR_SEL_SET
} MENU_QUICK_FUNC_ID_e;

static int menu_quick_cal_sel_disp_param(void);
#define MENU_QUICK_PAGE_CHANGED    (0x01)
static int menu_quick_switch_to_sel(int selIdx);

static int menu_quick_func(UINT32 FuncId, UINT32 param1, UINT32 param2);

static int menu_quick_button_up(void);
static int menu_quick_button_down(void);
static int menu_quick_button_left(void);
static int menu_quick_button_right(void);
static int menu_quick_button_set(void);
static int menu_quick_button_menu(void);

typedef struct _MENU_QUICK_OP_s_ {
    int (*ButtonUp)(void);
    int (*ButtonDown)(void);
    int (*ButtonLeft)(void);
    int (*ButtonRight)(void);
    int (*ButtonSet)(void);
    int (*ButtonMenu)(void);
} MENU_QUICK_OP_s;

static MENU_QUICK_OP_s menu_quick_op = {
    menu_quick_button_up,
    menu_quick_button_down,
    menu_quick_button_left,
    menu_quick_button_right,
    menu_quick_button_set,
    menu_quick_button_menu
};

typedef struct _MENU_QUICK_s_ {
    int PageNum;
    int PageCur;
    int DispSelStart;
    int DispSelNum;
    int DispSelCur;
    MENU_ITEM_s *Item;
    int (*Func)(UINT32 funcId, UINT32 param1, UINT32 param2);
    int (*Gui)(UINT32 guiCmd, UINT32 param1, UINT32 param2);
    MENU_QUICK_OP_s *Op;
} MENU_QUICK_s;

static MENU_QUICK_s menu_quick;

static int app_menu_quick_on(UINT32 param);
static int app_menu_quick_off(UINT32 param);
static int app_menu_quick_on_message(UINT32 msg, UINT32 param1, UINT32 param2);

WIDGET_ITEM_s widget_menu_quick = {
    app_menu_quick_on,
    app_menu_quick_off,
    app_menu_quick_on_message
};

/*************************************************************************
 * Quick Menu APIs
 ************************************************************************/
static int menu_quick_cal_sel_disp_param(void)
{
    int ReturnValue = 0;
    int PageOld = 0;

    PageOld = menu_quick.PageCur;
    menu_quick.PageNum = (menu_quick.Item->SelNum+(GUI_OPT_NUM-1))/GUI_OPT_NUM;
    menu_quick.PageCur = menu_quick.Item->SelCur / GUI_OPT_NUM;
    menu_quick.DispSelStart = GUI_OPT_NUM * menu_quick.PageCur;
    menu_quick.DispSelNum = menu_quick.Item->SelNum - menu_quick.DispSelStart;
    if (menu_quick.DispSelNum > GUI_OPT_NUM) {
        menu_quick.DispSelNum = GUI_OPT_NUM;
    }
    menu_quick.DispSelCur = menu_quick.Item->SelCur - menu_quick.DispSelStart;

    if (PageOld != menu_quick.PageCur) {
        ReturnValue = MENU_QUICK_PAGE_CHANGED;
    }
    return ReturnValue;
}

static int menu_quick_switch_to_sel(int selIdx)
{
    int ReturnValue = 0;
    int i = 0;

    menu_quick.Gui(GUI_OPT_HIGHLIGHT, menu_quick.Item->SelCur, 0);
    /** Change selection cursor */
    menu_quick.Item->SelCur = selIdx;
#if 0	
    /** Get new menu selection display parameters */
    ReturnValue = menu_quick.Func(MENU_QUICK_CAL_SEL_DISP_PARAM, 0, 0);
    /** If menu selection content changed, refresh all Item GUI */
    if (ReturnValue == MENU_QUICK_PAGE_CHANGED) {
        for (i=0; i< GUI_OPT_NUM; i++) {
            if (i < menu_quick.DispSelNum) {
                menu_quick.Gui(GUI_OPT_UPDATE, i, menu_quick.Item->GetSelBmp(menu_quick.DispSelStart+i));
                menu_quick.Gui(GUI_OPT_SHOW, i, 0);
            } else {
                menu_quick.Gui(GUI_OPT_HIDE, i, 0);
            }
        }
    }
    /** Highlight current selection */
    menu_quick.Gui(GUI_OPT_HIGHLIGHT, menu_quick.DispSelCur, 0);
    if (APP_CHECKFLAGS(menu_quick.Item->Sels[menu_quick.Item->SelCur]->Flags, MENU_SEL_FLAGS_LOCKED)) {
        menu_quick.Gui(GUI_DESC_UPDATE, menu_quick.Item->GetSelStr(menu_quick.Item->SelCur), 1);
    } else {
        menu_quick.Gui(GUI_DESC_UPDATE, menu_quick.Item->GetSelStr(menu_quick.Item->SelCur), 0);
    }
#endif
    menu_quick.Gui(GUI_OPT_HIGHLIGHT, menu_quick.Item->SelCur, 1);
    /** Flush GUI */
    menu_quick.Gui(GUI_FLUSH, 0, 0);

    return ReturnValue;
}

static int menu_quick_func(UINT32 funcId, UINT32 param1, UINT32 param2)
{
    int ReturnValue = 0;

    switch (funcId) {
    case MENU_QUICK_CAL_SEL_DISP_PARAM:
        ReturnValue = menu_quick_cal_sel_disp_param();
        break;
    case MENU_QUICK_SWITCH_TO_SEL:
        ReturnValue = menu_quick_switch_to_sel(param1);
        break;
    case MENU_QUICK_CUR_SEL_SET:
        if (APP_CHECKFLAGS(menu_quick.Item->Sels[menu_quick.Item->SelCur]->Flags, MENU_SEL_FLAGS_LOCKED)) {
            ReturnValue = -1;
        } else {
            ReturnValue = menu_quick.Item->SelSet();
            /* The flow only changes the preference of user.*/
            AppPref_Save();
        }
        break;
    default:
        AmbaPrint("[app_menu_quick] The function is undefined");
        break;
    }

    return ReturnValue;
}

static int menu_quick_button_up(void)
{
    int SelTarget = 0;

    if ((menu_quick.PageCur == 0) && (menu_quick.PageNum > 1)) {
        SelTarget = ((menu_quick.PageNum-1)*GUI_OPT_NUM)+menu_quick.DispSelCur;
        if (SelTarget >= menu_quick.Item->SelNum) {
            SelTarget = menu_quick.Item->SelNum-1;
        }
        menu_quick.Func(MENU_QUICK_SWITCH_TO_SEL, SelTarget, 0);
    } else if (menu_quick.PageCur > 0) {
        SelTarget = menu_quick.Item->SelCur-GUI_OPT_NUM;
        menu_quick.Func(MENU_QUICK_SWITCH_TO_SEL, SelTarget, 0);
    }

    return 0;
}

static int menu_quick_button_down(void)
{
    int SelTarget = 0;

    if ((menu_quick.PageCur == (menu_quick.PageNum-1)) &&
        (menu_quick.PageNum > 1)) {
        SelTarget = menu_quick.DispSelCur;
        menu_quick.Func(MENU_QUICK_SWITCH_TO_SEL, SelTarget, 0);
    } else if (menu_quick.PageCur < (menu_quick.PageNum-1)) {
        SelTarget = menu_quick.Item->SelCur+GUI_OPT_NUM;
        if (SelTarget >= menu_quick.Item->SelNum) {
            SelTarget = menu_quick.Item->SelNum-1;
        }
        menu_quick.Func(MENU_QUICK_SWITCH_TO_SEL, SelTarget, 0);
    }

    return 0;
}

static int menu_quick_button_left(void)
    {
    int SelTarget = 0;

    if (menu_quick.Item->SelNum > 1) {
        SelTarget = menu_quick.Item->SelCur-1;
        if (SelTarget < 0) {
            SelTarget = menu_quick.Item->SelNum-1;
        }
        menu_quick.Func(MENU_QUICK_SWITCH_TO_SEL, SelTarget, 0);
    }

    return 0;
}

static int menu_quick_button_right(void)
{
    int SelTarget = 0;

    if (menu_quick.Item->SelNum > 1) {
        SelTarget = menu_quick.Item->SelCur+1;
        if (SelTarget == menu_quick.Item->SelNum) {
            SelTarget = 0;
        }
        menu_quick.Func(MENU_QUICK_SWITCH_TO_SEL, SelTarget, 0);
    }

    return 0;
}

static int menu_quick_button_set(void)
{
    menu_quick.Func(MENU_QUICK_CUR_SEL_SET, 0, 0);
    //return AppWidget_Off(WIDGET_MENU_QUICK, 0);
}

static int menu_quick_button_menu(void)
{
    return AppWidget_Off(WIDGET_MENU_QUICK, 0);
}

static int app_menu_quick_on(UINT32 param)
{
    int ReturnValue = 0;
    int i = 0;

    menu_quick.Func = menu_quick_func;
    menu_quick.Gui = gui_menu_quick_func;
    menu_quick.Op = &menu_quick_op;


    menu_quick.Item->Init();

    /** Reset current selection */
    menu_quick.Item->SelCur = menu_quick.Item->SelSaved;
#if 0	
    /** Show quick menu frame */
    menu_quick.Gui(GUI_QMENU_SHOW, 0, 0);
    menu_quick.Gui(GUI_BTN_HIGHLIGHT, GUI_BTN_SET, 0);
    menu_quick.Gui(GUI_BTN_SHOW, GUI_BTN_SET, 0);
    menu_quick.Gui(GUI_BTN_HIGHLIGHT, GUI_BTN_CANCEL, 0);
    menu_quick.Gui(GUI_BTN_SHOW, GUI_BTN_CANCEL, 0);
    menu_quick.Gui(GUI_BTN_HIGHLIGHT, GUI_BTN_UP, 0);
    menu_quick.Gui(GUI_BTN_SHOW, GUI_BTN_UP, 0);
    menu_quick.Gui(GUI_BTN_HIGHLIGHT, GUI_BTN_DOWN, 0);
    menu_quick.Gui(GUI_BTN_SHOW, GUI_BTN_DOWN, 0);

    /** Get menu selection display parameters */
    ReturnValue = menu_quick.Func(MENU_QUICK_CAL_SEL_DISP_PARAM, 0, 0);

    /** Selection content should be loaded */
    for (i=0; i<GUI_OPT_NUM; i++) {
    if (i < menu_quick.DispSelNum) {
        menu_quick.Gui(GUI_OPT_UPDATE, i, menu_quick.Item->GetSelBmp(menu_quick.DispSelStart+i));
        menu_quick.Gui(GUI_OPT_SHOW, i, 0);
    } else {
        menu_quick.Gui(GUI_OPT_HIDE, i, 0);
    }
    }
    /** Show description*/
    if (APP_CHECKFLAGS(menu_quick.Item->Sels[menu_quick.DispSelCur]->Flags, MENU_SEL_FLAGS_LOCKED)) {
        menu_quick.Gui(GUI_DESC_UPDATE, menu_quick.Item->GetSelStr(menu_quick.DispSelCur), 1);
    } else {
        menu_quick.Gui(GUI_DESC_UPDATE, menu_quick.Item->GetSelStr(menu_quick.DispSelCur), 0);
    }
    menu_quick.Gui(GUI_DESC_SHOW, 0, 0);	
    menu_quick.Gui(GUI_OPT_HIGHLIGHT, menu_quick.DispSelCur, 0);
    menu_quick.Gui(GUI_OPT_SHOW, GUI_OPT_HL, 0);	
#endif	
   /* if(menu_quick.Item->Id == MENU_LANGUAGE_SET)
    {
	      menu_quick.Gui(GUI_MENU_QUICK_CONF_HIGHLIGHT,UserSetting->SetupPref.LanguageID,1);//menu.DispItemCur    		 
	      menu_quick.Gui(GUI_MENU_QUICK_CONF_SHOW,UserSetting->SetupPref.LanguageID,0);//menu.DispItemCur    		                     
    }
    if(menu_quick.Item->Id == MENU_BACKLIGHT)*/
    if(AppWidget_GetPback() == WIDGET_MENU)
    {
         if(menu_quick.Item->Id == MENU_PBACK_VIDEO_PLAY_OPTION)
         {
	      menu_quick.Gui(GUI_MENU_QUICK_CONF_HIGHLIGHT,UserSetting->PlaybackPref.VideoPlayOpt,1);//menu.DispItemCur    		 
	      menu_quick.Gui(GUI_MENU_QUICK_CONF_SHOW,UserSetting->PlaybackPref.VideoPlayOpt,0);//menu.DispItemCur    		                     		 	
         }  
    }
    else
    {
	    switch(menu_quick.Item->Id)
	    {
	        case MENU_LANGUAGE_SET:
		      menu_quick.Gui(GUI_MENU_QUICK_CONF_HIGHLIGHT,UserSetting->SetupPref.LanguageID,1);//menu.DispItemCur    		 
		      menu_quick.Gui(GUI_MENU_QUICK_CONF_SHOW,UserSetting->SetupPref.LanguageID,0);//menu.DispItemCur    		                     
		    break;
	        case MENU_BACKLIGHT:
			 switch(UserSetting->SetupPref.backlightoff_time)
			 {
			        case BACKLIGHTOFF_TIME_OFF:
					      menu_quick.Gui(GUI_MENU_QUICK_CONF_HIGHLIGHT,0,1);  		 
					      menu_quick.Gui(GUI_MENU_QUICK_CONF_SHOW,0,0);
				      break;	
			        case BACKLIGHTOFF_TIME_1_MIN:
					      menu_quick.Gui(GUI_MENU_QUICK_CONF_HIGHLIGHT,1,1);  		 
					      menu_quick.Gui(GUI_MENU_QUICK_CONF_SHOW,1,0);				
				       break;	
			        case BACKLIGHTOFF_TIME_3_MIN:
					      menu_quick.Gui(GUI_MENU_QUICK_CONF_HIGHLIGHT,2,1);  		 
					      menu_quick.Gui(GUI_MENU_QUICK_CONF_SHOW,2,0);				
				       break;	
			        case BACKLIGHTOFF_TIME_5_MIN:
					      menu_quick.Gui(GUI_MENU_QUICK_CONF_HIGHLIGHT,3,1);  		 
					      menu_quick.Gui(GUI_MENU_QUICK_CONF_SHOW,3,0);
					break;	
				 default:
				 	break;
			}			
		    break;		
		 case MENU_MOTION_DAT:	 
		      menu_quick.Gui(GUI_MENU_QUICK_CONF_HIGHLIGHT,UserSetting->MotionDetectPref.MotionDetect,1);   		 
		      menu_quick.Gui(GUI_MENU_QUICK_CONF_SHOW,UserSetting->MotionDetectPref.MotionDetect,0);   		                     
		    break;	
		 case MENU_GSENSOR:	 
		      menu_quick.Gui(GUI_MENU_QUICK_CONF_HIGHLIGHT,UserSetting->GSensorSentivityPref.Gsensor_sensitivity,1);   		 
		      menu_quick.Gui(GUI_MENU_QUICK_CONF_SHOW,UserSetting->GSensorSentivityPref.Gsensor_sensitivity,0);   		                     
		    break;	
		 case MENU_PARKING_CON:	 
		      menu_quick.Gui(GUI_MENU_QUICK_CONF_HIGHLIGHT,UserSetting->VideoPref.parkingmode_sensitivity,1);   		 
		      menu_quick.Gui(GUI_MENU_QUICK_CONF_SHOW,UserSetting->VideoPref.parkingmode_sensitivity,0);   		                     
		    break;	
		 case MENU_SPLIT_REC:	 
		      menu_quick.Gui(GUI_MENU_QUICK_CONF_HIGHLIGHT,UserSetting->VideoPref.video_split_rec_time,1);   		 
		      menu_quick.Gui(GUI_MENU_QUICK_CONF_SHOW,UserSetting->VideoPref.video_split_rec_time,0);   		                     
		    break;
		 case MENU_FLICKER:	 
		      menu_quick.Gui(GUI_MENU_QUICK_CONF_HIGHLIGHT,UserSetting->ImagePref.Flicker-1,1);   		 
		      menu_quick.Gui(GUI_MENU_QUICK_CONF_SHOW,UserSetting->ImagePref.Flicker-1,0);   		                     
		    break;		
		 default:
		    break;	
	    }
    }
    //menu_quick.Gui(GUI_MENU_QUICK_BG_UPDATE_BITMAP, 0, 0);    
    //menu_quick.Gui(GUI_MENU_QUICK_BG_SHOW, 0, 0);    	
    for (i=0; i<GUI_OPT_5; i++) {
	    if (i < menu_quick.Item->SelNum) {
		    /** Show description*/
		    //if (APP_CHECKFLAGS(menu_quick.Item->Sels[menu_quick.DispSelCur]->Flags, MENU_SEL_FLAGS_LOCKED)) {
		       // menu_quick.Gui(GUI_DESC_UPDATE, menu_quick.Item->GetSelStr(menu_quick.DispSelCur), 1);
		    //} else {
		       // menu_quick.Gui(GUI_DESC_UPDATE, menu_quick.Item->GetSelStr(menu_quick.DispSelCur), 0);
		    //}
		    menu_quick.Gui(GUI_DESC_UPDATE, i, menu_quick.Item->GetSelStr(i));		    
		    menu_quick.Gui(GUI_DESC_SHOW, i, 0);
           } else {
		   menu_quick.Gui(GUI_DESC_HIDE, i, 0);
	   }
    }
    /** Highlight current sel */
    menu_quick.Gui(GUI_OPT_HIGHLIGHT, menu_quick.Item->SelCur, 1);
    if(AppWidget_GetPback() != WIDGET_MENU)
    {
	    menu_quick.Gui(GUI_QUICK_BG_UPDATE_STRING,0, menu_quick.Item->GetTabStr());		
	    menu_quick.Gui(GUI_QUICK_BG_STRING_SHOW,0,0);			
    }	
    //menu_quick.Gui(GUI_OPT_SHOW, GUI_OPT_HL, 0);
    /** Flush GUI */
    menu_quick.Gui(GUI_FLUSH, 0, 0);
    return ReturnValue;
}

static int app_menu_quick_off(UINT32 param)
{
    int i = 0;

#if 0
    menu_quick.Gui(GUI_QMENU_HIDE, 0, 0);
    menu_quick.Gui(GUI_OPT_HIDE, GUI_OPT_HL, 0);
    menu_quick.Gui(GUI_DESC_HIDE, 0, 0);
    for (i=0; i<GUI_BTN_NUM; i++) {
        menu_quick.Gui(GUI_BTN_HIDE, i, 0);
    }
    for (i=0; i<GUI_OPT_NUM; i++) {
        menu_quick.Gui(GUI_OPT_HIDE, i, 0);
    }
#endif	
    if(AppWidget_GetPback() == WIDGET_MENU)
    {
         if(menu_quick.Item->Id == MENU_PBACK_VIDEO_PLAY_OPTION)
         {
	      menu_quick.Gui(GUI_MENU_QUICK_CONF_HIDE,UserSetting->PlaybackPref.VideoPlayOpt,0);//menu.DispItemCur    		                     		 	
         }  
    }	 
   else	
   {
	    switch(menu_quick.Item->Id)
	    {
	        case MENU_LANGUAGE_SET:
		      menu_quick.Gui(GUI_MENU_QUICK_CONF_HIDE,UserSetting->SetupPref.LanguageID,0);//menu.DispItemCur    		                     
		    break;
	        case MENU_BACKLIGHT:
			 switch(UserSetting->SetupPref.backlightoff_time)
			 {
			        case BACKLIGHTOFF_TIME_OFF:
					      menu_quick.Gui(GUI_MENU_QUICK_CONF_HIDE,0,0);
				      break;	
			        case BACKLIGHTOFF_TIME_1_MIN:
					      menu_quick.Gui(GUI_MENU_QUICK_CONF_HIDE,1,0);				
				       break;	
			        case BACKLIGHTOFF_TIME_3_MIN:
					      menu_quick.Gui(GUI_MENU_QUICK_CONF_HIDE,2,0);				
				       break;	
			        case BACKLIGHTOFF_TIME_5_MIN:
					      menu_quick.Gui(GUI_MENU_QUICK_CONF_HIDE,3,0);
					break;	
				 default:
				 	break;
			}			
		    break;	
		 case MENU_MOTION_DAT:	  		 
		      menu_quick.Gui(GUI_MENU_QUICK_CONF_HIDE,UserSetting->MotionDetectPref.MotionDetect,0);  
		    break;	
		 case MENU_GSENSOR:	 
		      menu_quick.Gui(GUI_MENU_QUICK_CONF_HIDE,UserSetting->GSensorSentivityPref.Gsensor_sensitivity,0);   		                     
		    break;	
		 case MENU_PARKING_CON:	  		 
		      menu_quick.Gui(GUI_MENU_QUICK_CONF_HIDE,UserSetting->VideoPref.parkingmode_sensitivity,0);   		                     
		    break;	
		 case MENU_SPLIT_REC:	 		 
		      menu_quick.Gui(GUI_MENU_QUICK_CONF_HIDE,UserSetting->VideoPref.video_split_rec_time,0);   		                     
		    break;
		 case MENU_FLICKER:	 
		      menu_quick.Gui(GUI_MENU_QUICK_CONF_HIDE,UserSetting->ImagePref.Flicker-1,0);   		                     
		    break;	
		 default:
		    break;	
	    }
       menu_quick.Gui(GUI_QUICK_BG_STRING_HIDE,0,0);			    		
    }
    menu_quick.Gui(GUI_OPT_HIGHLIGHT, menu_quick.Item->SelCur, 0);		
    for (i=0; i<GUI_OPT_5; i++) {		
	    menu_quick.Gui(GUI_DESC_HIDE, i, 0);
    }
    //menu_quick.Gui(GUI_MENU_QUICK_BG_HIDE, 0, 0);    
    menu_quick.Gui(GUI_FLUSH, 0, 0);

    return 0;
}

static int app_menu_quick_on_message(UINT32 msg, UINT32 param1, UINT32 param2)
{
    int ReturnValue = WIDGET_PASSED_MSG;

    switch (msg) {
    case HMSG_USER_UP_BUTTON:
    case HMSG_USER_IR_UP_BUTTON:
        ReturnValue = menu_quick.Op->ButtonUp();
        break;
    case HMSG_USER_DOWN_BUTTON:
    case HMSG_USER_IR_DOWN_BUTTON:
        ReturnValue = menu_quick.Op->ButtonDown();
        break;
    case HMSG_USER_LEFT_BUTTON:
    case HMSG_USER_IR_LEFT_BUTTON:
        ReturnValue = menu_quick.Op->ButtonLeft();
        break;
    case HMSG_USER_RIGHT_BUTTON:
    case HMSG_USER_IR_RIGHT_BUTTON:
        ReturnValue = menu_quick.Op->ButtonRight();
        break;
    case HMSG_USER_SET_BUTTON:
    case HMSG_USER_IR_SET_BUTTON:
        ReturnValue = menu_quick.Op->ButtonSet();
        break;
    case HMSG_USER_MENU_BUTTON:
    case HMSG_USER_IR_MENU_BUTTON:
        ReturnValue = menu_quick.Op->ButtonMenu();
        break;
    default:
        ReturnValue = WIDGET_PASSED_MSG;
        break;
    }

    return ReturnValue;
}

int record_item_sel(UINT8 value,int pre_value)
{
      menu_quick.Gui(GUI_MENU_QUICK_CONF_HIDE,pre_value,0);//menu.DispItemCur    		       		  
      menu_quick.Gui(GUI_MENU_QUICK_CONF_HIGHLIGHT, value, 1);
      menu_quick.Gui(GUI_MENU_QUICK_CONF_SHOW,value,0);//menu.DispItemCur    
      menu_quick.Gui(GUI_FLUSH, 0, 0);
      return 0;	   
}
int show_icn(UINT8 param1,UINT8 param2)
{
    if(param2==0)
    {
        menu_quick.Gui(GUI_DETECT_MOTION_ICN_SHOW, param1,0);
    }
    return 0;
}

/*************************************************************************
 * Quick Menu APIs for widget management
 ************************************************************************/
WIDGET_ITEM_s* AppMenuQuick_GetWidget(void)
{
    return &widget_menu_quick;
}

/*************************************************************************
 * Public Quick Menu Widget APIs
 ************************************************************************/
int AppMenuQuick_SetItem(UINT32 tabId, UINT32 itemId)
{
    menu_quick.Item = AppMenu_GetItem(tabId, itemId);
    return 0;
}
