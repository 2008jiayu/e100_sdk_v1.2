/**
  * @file src/app/apps/flow/widget/menu/menu.c
  *
  *  Implementation of Main Menu
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
#include <apps/apps.h>
#include <apps/flow/widget/menu/menu.h>
#include <apps/gui/widget/menu/gui_menu.h>
#include <system/app_pref.h>
#include <AmbaUART_Def.h>
#include <apps/gui/resource/gui_settle.h>


typedef enum _REC_CAM_FUNC_ID_e_ {
   REC_CAM_WARNING_MSG_SHOW_START=72,
} REC_CAM_FUNC_ID_e;

typedef struct
{
   unsigned short flg;
   unsigned short size;
   unsigned short index;
   unsigned short cmd;
   unsigned short data;
	
}K_ADC_TEST_CMD; 

/*************************************************************************
 * Menu definitions
 ************************************************************************/
extern MENU_TAB_CTRL_s menu_video_ctrl;
extern MENU_TAB_CTRL_s menu_photo_ctrl;
extern MENU_TAB_CTRL_s menu_pback_ctrl;
extern MENU_TAB_CTRL_s menu_image_ctrl;
#if defined(ENABLE_SOUND_RECORDER_PIPE)
extern MENU_TAB_CTRL_s menu_sound_ctrl;
#endif
#if defined(ENABLE_MW_PICTBRIDGE)
extern MENU_TAB_CTRL_s menu_pictb_setup_ctrl;
extern MENU_TAB_CTRL_s menu_pictb_print_ctrl;
#endif
extern MENU_TAB_CTRL_s menu_setup_ctrl;
#ifdef CONFIG_APP_ARD
extern MENU_TAB_CTRL_s menu_factory_ctrl;
#endif
extern MENU_TAB_CTRL_s adas_setup_ctrl;
#ifdef CONFIG_ECL_GUI
extern MENU_TAB_CTRL_s menu_adas_cali_ctrl;
extern MENU_TAB_CTRL_s menu_adas_func_ctrl;
extern MENU_TAB_CTRL_s menu_adas_alarm_dis_ctrl;
extern MENU_TAB_CTRL_s menu_recorder_set_ctrl;
extern MENU_TAB_CTRL_s menu_version_ctrl;
extern MENU_TAB_CTRL_s menu_default_ctrl;
#endif

static MENU_TAB_CTRL_s *menu_tab_ctrls[MENU_ECL_TAB_NUM] = {
//#ifdef CONFIG_ECL_GUI
    &menu_adas_cali_ctrl,
    &menu_adas_func_ctrl,
    &menu_adas_alarm_dis_ctrl,
    &menu_recorder_set_ctrl,
    &menu_version_ctrl,
    &menu_default_ctrl,    
    &menu_pback_ctrl,    // MENU_PBACK    
/*#else	
    &adas_setup_ctrl,
    &menu_video_ctrl,    // MENU_VIDEO
    &menu_setup_ctrl,    // MENU_SETUP
    &menu_photo_ctrl,    // MENU_PHOTO
    &menu_pback_ctrl,    // MENU_PBACK
#ifdef CONFIG_APP_ARD
    NULL,                // MENU_IMAGE
    &menu_factory_ctrl,   // MENU_FACTORY
#endif*/
//#endif
};

/** Menu funcions */
typedef enum _MENU_FUNC_ID_e_ {
    MENU_CAL_TAB_DISP_PARAM = 0,
    MENU_CAL_ITEM_DISP_PARAM,
    MENU_SWITCH_TO_TAB,
    MENU_SWITCH_TO_ITEM,
    MENU_CUR_ITEM_SET,
    MENU_SELF_TEST
} MENU_FUNC_ID_e;

static int menu_cal_tab_disp_param(void);
#define MENU_TAB_PAGE_CHANGED    (0x01)
static int menu_cal_item_disp_param(void);
#define MENU_ITEM_PAGE_CHANGED    (0x01)
static int menu_switch_to_tab(int tabIdx);
static int menu_switch_to_item(int itemIdx);

static int menu_func(UINT32 funcId, UINT32 param1, UINT32 param2);

/** Menu operations */
static int menu_button_up(void);
static int menu_button_down(void);
static int menu_button_left(void);
static int menu_button_right(void);
static int menu_button_set(void);
static int menu_button_menu(void);
int AppMenu_ReflushItem(void);


typedef struct _MENU_OP_s_ {
    int (*ButtonUp)(void);
    int (*ButtonDown)(void);
    int (*ButtonLeft)(void);
    int (*ButtonRight)(void);
    int (*ButtonSet)(void);
    int (*ButtonMenu)(void);
} MENU_OP_s;

static MENU_OP_s menu_op = {
    menu_button_up,
    menu_button_down,
    menu_button_left,
    menu_button_right,
    menu_button_set,
    menu_button_menu
};

/** Menu status */
typedef struct _MENU_s_ {
    int TabNum;
    int TabCur;
    int DispTabStart;
    int DispTabNum;
    int DispTabCur;
    int DispItemStart;
    int DispItemNum;
    int DispItemCur;
    MENU_TAB_s *tabs[MENU_TAB_NUM];
    int (*Func)(UINT32 funcId, UINT32 param1, UINT32 param2);
    int (*Gui)(UINT32 guiCmd, UINT32 param1, UINT32 param2);
    MENU_OP_s *Op;
} MENU_s;

static MENU_s menu;

static int cur_menu_tab_id = -1;
//static int set_key_is_down = FALSE;
static int Dispflag = FALSE;
static int Hide_bg_flag = FALSE;
static int Menu_list_flag = FALSE;
/** Menu interface */
static int app_menu_on(UINT32 param);
static int app_menu_off(UINT32 param);
static int app_menu_on_message(UINT32 msg, UINT32 param1, UINT32 param2);

WIDGET_ITEM_s widget_menu = {
    app_menu_on,
    app_menu_off,
    app_menu_on_message
};


static int tab_menu_off(void)
{
    int i = 0;
    menu.tabs[menu.TabCur]->Stop();
    //menu.Gui(GUI_MENU_ITEM_HIGHLIGHT, menu.TabCur, 0);	
    menu.Gui(GUI_MENU_BG_HIDE, 0, 0);   
    menu.Gui(GUI_MENU_BG_UPDATE_BITMAP, 1, 0);        
    menu.Gui(GUI_MENU_BG_SHOW, 1, 0);           	
    for (i=0; i<MENU_ECL_TAB_NUM-1; i++) {
        menu.Gui(GUI_MENU_TAB_HIDE, i, 0);
    }	
    menu.Gui(GUI_MENU_BG_STRING_HIDE,0,0);    	        	
    menu.Gui(GUI_FLUSH, 0, 0);
    Dispflag = TRUE;			
    return 0;	
}

static int item_menu_off(void)
{
	int i = 0;
	int dis_sel = 0;
    MENU_TAB_s *CurTab;

    CurTab = menu.tabs[menu.TabCur];

    menu.Gui(GUI_MENU_BG_HIDE, 1, 0);           
    menu.Gui(GUI_MENU_BG_UPDATE_BITMAP, 0, 0);        
    menu.Gui(GUI_MENU_BG_SHOW, 0, 0);   	  
    //if(CurTab->Id == MENU_LANGUAGE)
    //{
    //menu.Gui(GUI_MENU_ITEM_CONF_HIDE,UserSetting->SetupPref.LanguageID,0);//menu.DispItemCur    		       
    //}	
    if(CurTab->Id == MENU_ADAS_ALARM_DIS)  
    {
      /*if(UserSetting->SetupPref.adas_alarm_dis == 1.2)
      	{
         	   dis_sel = 1;
          menu.Gui(GUI_MENU_ITEM_CONF_HIDE,dis_sel,0);//menu.DispItemCur    		 
    }
    if(UserSetting->SetupPref.adas_alarm_dis == 1.5)
    {
         	   dis_sel = 2;
          menu.Gui(GUI_MENU_ITEM_CONF_HIDE,dis_sel,0);//menu.DispItemCur    	
    }
    if(UserSetting->SetupPref.adas_alarm_dis == 1.0)
    {
         	   dis_sel = 0;
          menu.Gui(GUI_MENU_ITEM_CONF_HIDE,dis_sel,0);//menu.DispItemCur    		 
    }*/
        dis_sel = UserSetting->SetupPref.adas_alarm_dis;
        menu.Gui(GUI_MENU_ITEM_CONF_HIDE,dis_sel,0);//menu.DispItemCur   		
    }
    else
    {
        ;
	}
    menu.Gui(GUI_MENU_ITEM_HIGHLIGHT, menu.DispItemCur, 0);		
    for (i=0; i<GUI_MENU_ITEM_NUM; i++) 
    {
        if((CurTab->Id == MENU_ADAS_FUNC)||(CurTab->Id == MENU_ADAS_CALI))
        {   
            menu.Gui(GUI_MENU_ITEM_CONF_HIDE, i, 0);
        }
        if((cur_menu_tab_id == MENU_RECORDER_SETTTING)||(cur_menu_tab_id == MENU_ADAS_CALI))
        {   
            menu.Gui(GUI_MENU_ITEM_ENTER_HIDE, i, 0);			
        }
        menu.Gui(GUI_MENU_ITEM_HIDE, i, 0);
    }
    menu.Gui(GUI_MENU_BG_STRING_HIDE, 1,0);	
    menu.Gui(GUI_FLUSH, 0, 0);
    Dispflag = FALSE;  
    return 0;	 
}

static int Reflushtab(void)
{
    //int ReturnValue = 0;
    int i = 0;
    MENU_TAB_s *CurTab;
	
    CurTab = menu.tabs[menu.TabCur];	
    /** Get menu tab display parameters */
    //menu.Func(MENU_CAL_TAB_DISP_PARAM, 0, 0);   
    /** refresh all tab GUI */
    for (i=0; i<MENU_ECL_TAB_NUM-1; i++) 
    {
    //if (i < menu.DispTabNum) {
        menu.Gui(GUI_MENU_TAB_UPDATE_BITMAP, i, menu.tabs[menu.DispTabStart+i]->Bmp);
        menu.Gui(GUI_MENU_TAB_SHOW, i, 0);
    //} else {
       // menu.Gui(GUI_MENU_TAB_HIDE, i, 0);
    //}
    }
    menu.Gui(GUI_MENU_BG_UPDATE_STRING, 0,menu.TabCur);			
    menu.Gui(GUI_MENU_BG_STRING_SHOW, 0,0);	
    /** Highlight current tab */
    menu.Gui(GUI_MENU_TAB_UPDATE_BITMAP, menu.TabCur, CurTab->BmpHl);
    menu.Gui(GUI_FLUSH, 0, 0);	
	return 0;
}

/*************************************************************************
 * Menu APIs
 ************************************************************************/
/** Menu functions */
////////////////////////////////////////////////////
static menu_hide_pback_bg(void)
{   
    int i=0;
    menu.Gui(GUI_MENU_BG_HIDE,2,0);
    for(i=0;i<2;i++)
    {
        menu.Gui(GUI_MENU_ITEM_HIDE,i,0);
    }
    return 0;
}

///////////////////////////////////////////////////
static int menu_cal_tab_disp_param(void)
{
    int ReturnValue = 0;
    int disp_tab_start_old = 0, page = 0, num = 0;

    disp_tab_start_old = menu.DispTabStart;
    page = menu.TabCur / GUI_MENU_TAB_NUM;
    menu.DispTabStart = GUI_MENU_TAB_NUM * page;
    num = menu.TabNum - menu.DispTabStart;
    menu.DispTabNum = (num > GUI_MENU_TAB_NUM) ? GUI_MENU_TAB_NUM : num;
    menu.DispTabCur = menu.TabCur - menu.DispTabStart;

    if (disp_tab_start_old != menu.DispTabStart) 
    {
        ReturnValue = MENU_TAB_PAGE_CHANGED;
    }
    return ReturnValue;
}

static int menu_cal_item_disp_param(void)
{
    int ReturnValue = 0;
    int DispItemStartOld = 0, page = 0, num = 0;
    MENU_TAB_s *CurTab = menu.tabs[menu.TabCur];
    DispItemStartOld = menu.DispItemStart;
    page = CurTab->ItemCur / GUI_MENU_ITEM_NUM;
    menu.DispItemStart = GUI_MENU_ITEM_NUM * page;
    num = CurTab->ItemNum - menu.DispItemStart;
    menu.DispItemNum = (num > GUI_MENU_ITEM_NUM) ? GUI_MENU_ITEM_NUM : num;
    menu.DispItemCur = CurTab->ItemCur - menu.DispItemStart;
    if (DispItemStartOld != menu.DispItemStart) 
    {
        ReturnValue = MENU_ITEM_PAGE_CHANGED;
    }
    return ReturnValue;
}

static int menu_switch_to_tab(int tabIdx)
{
    int ReturnValue = 0;
    //int i = 0;
    MENU_TAB_s *CurTab;

    /** Stop current tab */
    CurTab = menu.tabs[menu.TabCur];
    CurTab->Stop();

    /** Change current GUI */
    menu.Gui(GUI_MENU_TAB_UPDATE_BITMAP, menu.TabCur, CurTab->Bmp);
    menu.Gui(GUI_MENU_BG_STRING_HIDE,0,0);	
	
    //menu.Gui(GUI_MENU_ITEM_HIGHLIGHT, menu.DispItemCur, 0);

    /** Current tab cursor change */
    menu.TabCur = tabIdx;
    CurTab = menu.tabs[menu.TabCur];    
    //cur_menu_tab_id = CurTab->Id;    
    CurTab->Init();	
    menu.Gui(GUI_MENU_BG_UPDATE_STRING, 0,menu.TabCur);			
    menu.Gui(GUI_MENU_BG_STRING_SHOW, 0,0);
    /** Highlight current tab */
    menu.Gui(GUI_MENU_TAB_UPDATE_BITMAP, menu.TabCur, CurTab->BmpHl);
    /** Flush GUI */
    menu.Gui(GUI_FLUSH, 0, 0);
    return ReturnValue;
}

static int menu_switch_to_item(int itemIdx)
{
    int ReturnValue = 0;
    int i = 0;
    MENU_TAB_s *CurTab = menu.tabs[menu.TabCur];

    /** Unhighlight current item */
    menu.Gui(GUI_MENU_ITEM_HIGHLIGHT, menu.DispItemCur, 0);

    /** Change item cursor */
    CurTab->ItemCur = itemIdx;

    /** Get new menu item display parameters */
    ReturnValue = menu.Func(MENU_CAL_ITEM_DISP_PARAM, 0, 0);

    /** If menu item content changed, refresh all item GUI */
    if (ReturnValue == MENU_ITEM_PAGE_CHANGED) 
    {
        for (i=0; i< GUI_MENU_ITEM_NUM; i++) 
        {
            if (i < menu.DispItemNum) 
            {
                menu.Gui(GUI_MENU_ITEM_UPDATE_STRING, i, CurTab->Items[menu.DispItemStart+i]->GetTabStr());
                menu.Gui(GUI_MENU_ITEM_UPDATE_ICN_ENTER, i,0 );
                if (APP_CHECKFLAGS(CurTab->Flags, MENU_TAB_FLAGS_LOCKED) ||
                    APP_CHECKFLAGS(CurTab->Items[menu.DispItemStart+i]->Flags, MENU_ITEM_FLAGS_LOCKED)) 
                {
                    menu.Gui(GUI_MENU_ITEM_LOCK, i, 1);
                } 
                else 
                {
                    menu.Gui(GUI_MENU_ITEM_LOCK, i, 0);
                }
                menu.Gui(GUI_MENU_ITEM_SHOW, i, 0);
                menu.Gui(GUI_MENU_ITEM_ENTER_SHOW, i, 0);								
            } 
            else 
            {
                menu.Gui(GUI_MENU_ITEM_HIDE, i, 0);
                menu.Gui(GUI_MENU_ITEM_ENTER_HIDE, i, 0);				
            }
        }
    }
    /** Highlight current item */
    menu.Gui(GUI_MENU_ITEM_HIGHLIGHT, menu.DispItemCur, 1);
    /** Flush GUI */
    menu.Gui(GUI_FLUSH, 0, 0);
    return ReturnValue;
}
/*=====================addd for test mod=======================================================*/
static int send_cmd_flg=2;
static void menu_test_timer_handler(int eid)
{
    static int count=20;
    K_ADC_TEST_CMD RadarEn_cmd = {0x0110,0x0004,0x0000,0x000a,0x0001};
                 
    if (eid == TIMER_UNREGISTER) {
        send_cmd_flg= 2;
        count=20;
        return;
    }
   
    count--;
    if (count == 0 )
    {
        count=20; 
        AppLibComSvcTimer_Unregister(TIMER_20HZ, menu_test_timer_handler);
    }
    if (send_cmd_flg == 0)
    {
        send_cmd_flg = 2;
        Hide_bg_flag = TRUE;  
        Menu_list_flag = FALSE;
        extern record_sta_set(int sta);
        record_sta_set(1);
        AppWidget_Off(WIDGET_MENU, 0);	
        rec_cam_func(REC_CAM_WARNING_MSG_SHOW_START, GUI_WARNING_COMMUNICATIONS_TEST, 0);
        
        AppLibVideo_Emgency_AlarmFlag_Set(0);
        AmbaUART_Write(AMBA_UART_CHANNEL1, sizeof(K_ADC_TEST_CMD), (UINT8 *)&RadarEn_cmd,  AMBA_KAL_WAIT_FOREVER);
    }
   
}

static int menu_self_test_sys(void)
{
    send_cmd_flg--;
    AppLibComSvcTimer_Register(TIMER_20HZ, menu_test_timer_handler);
}

/*===============================================================================================*/
#if 0
/*=====================addd for test mod=======================================================*/
static int send_cmd_flg=2;
static void menu_test_timer_handler(int eid)
{
    static int count=2;
    K_ADC_TEST_CMD RadarEn_cmd = {0x0110,0x0004,0x0000,0x000a,0x0001};
                 
    if (eid == TIMER_UNREGISTER) {
        send_cmd_flg= 2;
        count=2;
        return;
    }
   
    count--;
    if (count == 0 )
    {
        count=2; 
        AppLibComSvcTimer_Unregister(TIMER_1HZ, menu_test_timer_handler);
    }
    if (send_cmd_flg == 0)
    {
        send_cmd_flg = 2;
        Hide_bg_flag = TRUE;  
        Menu_list_flag = FALSE;
        extern record_sta_set(int sta);
        record_sta_set(1);
        AppWidget_Off(WIDGET_MENU, 0);	
        rec_cam_func(REC_CAM_WARNING_MSG_SHOW_START, GUI_WARNING_COMMUNICATIONS_TEST, 0);
        
        AppLibVideo_Emgency_AlarmFlag_Set(0);
        AmbaUART_Write(AMBA_UART_CHANNEL1, sizeof(K_ADC_TEST_CMD), (UINT8 *)&RadarEn_cmd,  AMBA_KAL_WAIT_FOREVER);
    }
   
}

static int menu_self_test_sys(void)
{
    send_cmd_flg--;
    AppLibComSvcTimer_Register(TIMER_1HZ, menu_test_timer_handler);
}

/*===============================================================================================*/
#endif
static int menu_func(UINT32 funcId, UINT32 param1, UINT32 param2)
{
    int ReturnValue = 0;
    MENU_TAB_s *CurTab;

    switch (funcId) {
    case MENU_CAL_TAB_DISP_PARAM:
        ReturnValue = menu_cal_tab_disp_param();
        break;
    case MENU_CAL_ITEM_DISP_PARAM:
        ReturnValue = menu_cal_item_disp_param();
        break;
    case MENU_SWITCH_TO_TAB:
        ReturnValue = menu_switch_to_tab(param1);
        break;
    case MENU_SWITCH_TO_ITEM:
        ReturnValue = menu_switch_to_item(param1);
        break;
    case MENU_CUR_ITEM_SET:
        CurTab = menu.tabs[menu.TabCur];
        if (APP_CHECKFLAGS(CurTab->Flags, MENU_TAB_FLAGS_LOCKED) ||
            APP_CHECKFLAGS(CurTab->Items[CurTab->ItemCur]->Flags, MENU_ITEM_FLAGS_LOCKED)) 
        {
            AmbaPrint("[app_menu] Tab or item is locked");
            ReturnValue = -1;
        } 
        else 
        {       
            ReturnValue = CurTab->Items[CurTab->ItemCur]->Set();
        }	
        
        if(cur_menu_tab_id == MENU_ECL_PBACK)  
        {
            menu_hide_pback_bg();//hide playback menu bg and ..
        }
        break;
    case MENU_SELF_TEST:
        menu_self_test_sys();
        break;    
    default:
        AmbaPrint("[app_menu] The function is undefined");
        break;
    }

    return ReturnValue;
}
static int menu_button_down(void)
{   
    int TabTarget = 0;
    int ItemTarget = 0;
    MENU_TAB_s *CurTab = menu.tabs[menu.TabCur];
	if(Dispflag)   
    {
	    if (CurTab->ItemNum > 1) 
        {
	        ItemTarget = CurTab->ItemCur+1;
	        if (ItemTarget == CurTab->ItemNum) 
            {
	            ItemTarget = 0;
	        }
	        menu.Func(MENU_SWITCH_TO_ITEM, ItemTarget, 0);
	    }
    }	
    if(!Dispflag)   
    {
        if (menu.TabNum > 1) 
        {
            TabTarget = menu.TabCur+1;
            if (TabTarget == menu.TabNum) 
            {
                TabTarget = 0;
            }
            menu.Func(MENU_SWITCH_TO_TAB, TabTarget, 0);
        }
    } 
    
     
    return 0;
}

    
static int menu_button_up(void)
{
    int TabTarget = 0;
    int ItemTarget = 0;
    MENU_TAB_s *CurTab = menu.tabs[menu.TabCur];
    if(Dispflag)   
    {
	    if (CurTab->ItemNum > 1) 
        {
	        ItemTarget = CurTab->ItemCur-1;
	        if (ItemTarget < 0) 
            {
	            ItemTarget = CurTab->ItemNum-1;
	        }
	        menu.Func(MENU_SWITCH_TO_ITEM, ItemTarget, 0);
	    }
    }
    if(!Dispflag)   
    {
        if (menu.TabNum > 1) 
        {
            TabTarget = menu.TabCur-1;
            if (TabTarget < 0) 
            {
                TabTarget = menu.TabNum-1;
            }
            menu.Func(MENU_SWITCH_TO_TAB, TabTarget, 0);
        }
    }
   
    return 0;
}

static int menu_button_right(void)
{
    if((AppWidget_GetPback() != WIDGET_MENU)&&(AppWidget_GetPback() != WIDGET_MENU_QUICK)&&(!Dispflag))
    {
        menu.Func(MENU_SELF_TEST, 0, 0);
    }
    return 0;
}

static int menu_button_left(void)
{
    int TabTarget = 0;

    if(!Dispflag)   
    {
	    if (menu.TabNum > 1) 
        {
	        TabTarget = menu.TabCur-1;
	        if (TabTarget < 0) 
            {
	            TabTarget = menu.TabNum-1;
	        }
	        menu.Func(MENU_SWITCH_TO_TAB, TabTarget, 0);
	    }
    }
    return 0;
}

static int menu_button_set(void)
{
    int ReturnValue = 0;
    int i;
    MENU_TAB_s *CurTab = menu.tabs[menu.TabCur];
    //app_beep_play_beep(BEEP_OPERATION, 0);
    if(Dispflag)   
    {
        if(AppWidget_GetPback() == WIDGET_MENU)
    	{
	        Dispflag = FALSE;		
    	}  
        else
        {
            if(menu.TabCur == MENU_RECORDER_SETTTING)
            {
                if((CurTab->ItemCur == 0)||(CurTab->ItemCur == 1)|| (CurTab->ItemCur == 8))
                {
                    Hide_bg_flag = TRUE;                    
                }
                else
                {
                    Menu_list_flag = TRUE;
                }
            }  
            if(menu.TabCur == MENU_ADAS_CALI)
            {
                if(CurTab->ItemCur == 0) 
                Hide_bg_flag = TRUE;        		 	
            } 
        }
        ReturnValue = menu.Func(MENU_CUR_ITEM_SET, 0, 0);
        if(ReturnValue == CARD_STATUS_NO_CARD)
        {
            Hide_bg_flag = FALSE;
        }
    }	  
    else
    {
        //ReturnValue = AppWidget_On(WIDGET_MENU, 0);
        if((menu.TabCur == MENU_DEFAULT) || (menu.TabCur == MENU_VERSION))
        {
            //Hide_bg_flag = TRUE;        
            ReturnValue = menu.Func(MENU_CUR_ITEM_SET, 0, 0);
        }
        else
        {             
            tab_menu_off();
            AppMenu_ReflushItem();	  
        }	  
    }	  
    return ReturnValue;	
}

static int menu_button_menu(void)
{
    int ReturnValue = 0;
#ifdef CONFIG_BSP_ORTHRUS
    int tab_target = 0;
    if (menu.TabNum > 1) 
    {
        tab_target = menu.TabCur-1;
        if (tab_target < 0) 
        {
            tab_target = menu.TabNum-1;
        }

        if(tab_target == menu.TabNum-1) 
        {
            int rval = AppWidget_Off(WIDGET_MENU, 0);
            menu.TabCur = tab_target;
            cur_menu_tab_id = -1;
            return rval;
        } 
        else 
        {
            menu.Func(MENU_SWITCH_TO_TAB, tab_target, 0);
            return 0;
        }
    }
    return 0;
#else
    if(Dispflag)
    {
        if(AppWidget_GetPback() == WIDGET_MENU)
        {
            AppWidget_SetPback(WIDGET_NONE);            
            ReturnValue = AppWidget_Off(WIDGET_MENU, 0);			
        }		 
        else
        {
            item_menu_off();    
            Reflushtab();
        }	
    }   
    else
    {
        Hide_bg_flag = TRUE;  
        Menu_list_flag = FALSE;
        ReturnValue = AppWidget_Off(WIDGET_MENU, 0);			
    }		
    return ReturnValue;
#endif
}

static int menu_init = 0;

static int app_menu_on(UINT32 param)
{
    int ReturnValue = 0;
    int i = 0;
    MENU_TAB_s *CurTab;
    app_set_menu_scr_status(0);
    menu.Func = menu_func;
    menu.Gui = gui_menu_func;
    menu.Op = &menu_op;	

    menu_init = 1;
    if(Menu_list_flag == TRUE)
    {
        Menu_list_flag = FALSE;
        Dispflag	= TRUE;		  
        menu.TabCur	 = MENU_RECORDER_SETTTING;		  
        AppMenu_ReflushItem();	  
    }
    else
    {
        if ((cur_menu_tab_id < 0) ||(cur_menu_tab_id != menu.tabs[menu.TabCur]->Id))
        {
            menu.TabCur =0;//menu.TabNum-1;
            cur_menu_tab_id = menu.tabs[menu.TabCur]->Id;
        }
        CurTab = menu.tabs[menu.TabCur];

        /** Initialize current tab */
        CurTab->Init();
        /** Start current tab */
        CurTab->Start();
        if(AppWidget_GetPback() == WIDGET_MENU)
        {
            menu.Gui(GUI_MENU_BG_UPDATE_BITMAP, 2, 0); 
            menu.Gui(GUI_MENU_BG_SHOW, 2, 0);
            Dispflag = TRUE;
            for (i=0; i<CurTab->ItemNum; i++)
            {
                CurTab->Items[i]->Init();
            }    
            /** Get menu item display parameters */
            menu.Func(MENU_CAL_ITEM_DISP_PARAM, 0, 0);
            /** Item content should be loaded */
            for (i=0; i<GUI_MENU_ITEM_NUM; i++) 
            {
                if (i < menu.DispItemNum)
                {
                    menu.Gui(GUI_MENU_ITEM_UPDATE_STRING, i, CurTab->Items[menu.DispItemStart+i]->GetTabStr());

                    //menu.Gui(GUI_MENU_ITEM_CONF_HIGHLIGHT,i,0);//menu.DispItemCur    			
                    if (APP_CHECKFLAGS(CurTab->Flags, MENU_TAB_FLAGS_LOCKED) ||
                        APP_CHECKFLAGS(CurTab->Items[menu.DispItemStart+i]->Flags, MENU_ITEM_FLAGS_LOCKED)) 
                    {
                        menu.Gui(GUI_MENU_ITEM_LOCK, i, 1);
                    } 
                    else 
                    {
                        menu.Gui(GUI_MENU_ITEM_LOCK, i, 0);
                    }		
                    //menu.Gui(GUI_MENU_ITEM_SHOW, i, 0);
                    menu.Gui(GUI_MENU_ITEM_SHOW, i, 1);
                } 
                else 
                {
                    menu.Gui(GUI_MENU_ITEM_HIDE, i, 0);
                }
            }
            /** Highlight current item */
            menu.Gui(GUI_MENU_ITEM_HIGHLIGHT,menu.DispItemCur,1);//menu.DispItemCur  	
        }
        else
        {
            menu.Gui(GUI_MENU_BG_UPDATE_BITMAP, 0, 0);    
            menu.Gui(GUI_MENU_BG_SHOW, 0, 0);    		
            /** Get menu tab display parameters */
            for (i=0; i<MENU_ECL_TAB_NUM-1; i++)
            {
                menu.Gui(GUI_MENU_TAB_UPDATE_BITMAP, i, menu.tabs[i]->Bmp);
                menu.Gui(GUI_MENU_TAB_SHOW, i, 0);
            }
            menu.Gui(GUI_MENU_BG_UPDATE_STRING, 0,menu.TabCur);			
            menu.Gui(GUI_MENU_BG_STRING_SHOW, 0,0);				
            /** Highlight current tab */
            menu.Gui(GUI_MENU_TAB_UPDATE_BITMAP, menu.TabCur, CurTab->BmpHl);
        }
    }	
    /** Flush GUI */
    menu.Gui(GUI_FLUSH, 0, 0);
    return ReturnValue;
}

static int app_menu_off(UINT32 param)
{
    int i = 0;
    menu.tabs[menu.TabCur]->Stop();

    if(AppWidget_GetPback() == WIDGET_ALL)
    {
        AppWidget_SetPback(WIDGET_NONE);    
	    for (i=0; i<MENU_ECL_TAB_NUM-1; i++) 
        {
	        menu.Gui(GUI_MENU_TAB_HIDE, i, 0);
	    }	   
	    menu.Gui(GUI_MENU_BG_HIDE, 0, 0);    	    
	    menu.Gui(GUI_MENU_BG_STRING_HIDE,0,0);    	    
        item_menu_off();			
    } 
    else
    {
	    if(Dispflag)	
        {
            menu.Gui(GUI_MENU_BG_HIDE, 2, 0);
            menu.Gui(GUI_MENU_ITEM_HIGHLIGHT,menu.DispItemCur, 0);		    
            for (i=0; i<GUI_MENU_ITEM_NUM; i++) 
            {
                menu.Gui(GUI_MENU_ITEM_HIDE, i, 0);
                if((cur_menu_tab_id == MENU_RECORDER_SETTTING)||(cur_menu_tab_id == MENU_ADAS_CALI))
                {   
                    menu.Gui(GUI_MENU_ITEM_ENTER_HIDE, i, 0);			
                }			
            }   
            //set_key_is_down = FALSE;
            menu.Gui(GUI_MENU_BG_STRING_HIDE, 1,0);		        
            Dispflag = FALSE;		
        }
	    else
	    {   		
		    //menu.Gui(GUI_MENU_ITEM_HIGHLIGHT, menu.TabCur, 0);	
		    for (i=0; i<MENU_ECL_TAB_NUM-1; i++) 
            {
		        menu.Gui(GUI_MENU_TAB_HIDE, i, 0);
		    }
	        menu.Gui(GUI_MENU_BG_HIDE, 0, 0);    	    			
	    }	
	    if(Hide_bg_flag == TRUE)
	    {
	        Hide_bg_flag = FALSE;
	        menu.Gui(GUI_MENU_BG_HIDE, 0, 0);    	    
	    }		
        if(AppWidget_GetPback() != WIDGET_MENU)
        {   
            menu.Gui(GUI_MENU_BG_STRING_HIDE,0,0);  
        }
        else
        {
            menu.Gui(GUI_MENU_ITEM_HIGHLIGHT,menu.DispItemCur, 0);                        
        }   
        menu.Gui(GUI_FLUSH, 0, 0);
        menu.TabCur = 0;		
    }
    return 0;
}

static int app_menu_on_message(UINT32 msg, UINT32 param1, UINT32 param2)
{
    int ReturnValue = WIDGET_PASSED_MSG;

    switch (msg) {
    case HMSG_USER_UP_BUTTON:
    case HMSG_USER_IR_UP_BUTTON:
        ReturnValue = menu.Op->ButtonUp();
        break;
        
    case HMSG_USER_DOWN_BUTTON:
    case HMSG_USER_IR_DOWN_BUTTON:
        ReturnValue = menu.Op->ButtonDown();
        break;

    case HMSG_USER_LEFT_BUTTON:
    case HMSG_USER_IR_LEFT_BUTTON:
        ReturnValue = menu.Op->ButtonLeft();
        break;

    case HMSG_USER_RIGHT_BUTTON:
    case HMSG_USER_IR_RIGHT_BUTTON:
        ReturnValue = menu.Op->ButtonRight();
        break;

    case HMSG_USER_SET_BUTTON:
    case HMSG_USER_IR_SET_BUTTON:
        ReturnValue = menu.Op->ButtonSet();
        break;

    case HMSG_USER_MENU_BUTTON:
    case HMSG_USER_IR_MENU_BUTTON:
        ReturnValue = menu.Op->ButtonMenu();
        break;

    case AMSG_CMD_UPDATE_TIME_DISPLAY:
        {
            int i = 0;
            MENU_TAB_s *CurTab = menu.tabs[menu.TabCur];
            /** Reload items */
            for (i=0; i<menu.DispItemNum; i++) {
                menu.Gui(GUI_MENU_ITEM_UPDATE_STRING, i, CurTab->Items[menu.DispItemStart+i]->GetTabStr());
            }
            menu.Gui(GUI_FLUSH, 0, 0);
        }
        break;
    default:
        ReturnValue = WIDGET_PASSED_MSG;
        break;
    }

    return ReturnValue;
}

/*************************************************************************
 * Menu APIs for widget management
 ************************************************************************/
WIDGET_ITEM_s* AppMenu_GetWidget(void)
{
    return &widget_menu;
}

/*************************************************************************
 * Public Menu Widget APIs
 ************************************************************************/
int AppMenu_Reset(void)
{
    memset(&menu, 0, sizeof(MENU_s));
    menu_init = 0;
    return 0;
}

int AppMenu_RegisterTab(UINT32 tabId)
{
    if (menu.TabNum < MENU_ECL_TAB_NUM-1) 
    {
        menu.tabs[menu.TabNum] = menu_tab_ctrls[tabId]->GetTab();
        if (cur_menu_tab_id == menu.tabs[menu.TabNum]->Id)
        {
           menu.TabCur = menu.TabNum;
        }
        menu.TabNum++;
    } 
    else 
    {
        return -1;
    }
    return 0;
}

int AppMenu_ReflushItem(void)
{
    int ReturnValue = 0;
    int i = 0;
    int dis_sel = 0;	
    MENU_TAB_s *CurTab;

    if (menu_init == 0) 
    {
        return -1;
    }
    CurTab = menu.tabs[menu.TabCur];
    cur_menu_tab_id = CurTab->Id;
	
    /** Initialize Items under current tab */
    for (i=0; i<CurTab->ItemNum; i++) 
    {
        CurTab->Items[i]->Init();
    }    
    //if(cur_menu_tab_id == MENU_LANGUAGE)
    //{
	      //menu.Gui(GUI_MENU_ITEM_CONF_HIGHLIGHT,UserSetting->SetupPref.LanguageID,1);//menu.DispItemCur    		 
	      //menu.Gui(GUI_MENU_ITEM_CONF_SHOW,UserSetting->SetupPref.LanguageID,0);//menu.DispItemCur    		       
    //}
    if(cur_menu_tab_id == MENU_ADAS_CALI)	
    {
        if(UserSetting->SetupPref.adas_auto_cal_onoff)
        {
            menu.Gui(GUI_MENU_ITEM_CONF_HIGHLIGHT,1,3);//menu.DispItemCur   
        }
        else 
        {
            menu.Gui(GUI_MENU_ITEM_CONF_HIGHLIGHT,1,2);//menu.DispItemCur     
        }
    }
    if(cur_menu_tab_id == MENU_ADAS_FUNC)	
    {
        if(UserSetting->SetupPref.ldws_mode_onoff)
        {
            menu.Gui(GUI_MENU_ITEM_CONF_HIGHLIGHT,0,3);//menu.DispItemCur   
        }
        else 
        {
            menu.Gui(GUI_MENU_ITEM_CONF_HIGHLIGHT,0,2);//menu.DispItemCur     
        }
        
        if(UserSetting->SetupPref.fcws_mode_onoff)
        {
            menu.Gui(GUI_MENU_ITEM_CONF_HIGHLIGHT,1,3);//menu.DispItemCur   
        }
        else  
        {
            menu.Gui(GUI_MENU_ITEM_CONF_HIGHLIGHT,1,2);//menu.DispItemCur     
        }
        
        if(UserSetting->SetupPref.hmws_mode_onoff)
        {
            menu.Gui(GUI_MENU_ITEM_CONF_HIGHLIGHT,2,3);//menu.DispItemCur   
        }
        else 
        {
            menu.Gui(GUI_MENU_ITEM_CONF_HIGHLIGHT,2,2);//menu.DispItemCur     
        }
        
        if(UserSetting->SetupPref.fcmr_mode_onoff)
        {
            menu.Gui(GUI_MENU_ITEM_CONF_HIGHLIGHT,3,3);//menu.DispItemCur   
        }
        else 
        {
            menu.Gui(GUI_MENU_ITEM_CONF_HIGHLIGHT,3,2);//menu.DispItemCur                       
        }
    }
    else if(cur_menu_tab_id == MENU_ADAS_ALARM_DIS)
    {
       /* if(UserSetting->SetupPref.adas_alarm_dis == 1.2)			 	
         	{
         	   dis_sel = 1;
          menu.Gui(GUI_MENU_ITEM_CONF_HIGHLIGHT,dis_sel,1);//menu.DispItemCur    	
          menu.Gui(GUI_MENU_ITEM_CONF_SHOW,dis_sel,0);//menu.DispItemCur    		       	          	          
         	}  
      if(UserSetting->SetupPref.adas_alarm_dis == 1.5)
      	{
         	   dis_sel = 2;
          menu.Gui(GUI_MENU_ITEM_CONF_HIGHLIGHT,dis_sel,1);//menu.DispItemCur    	
          menu.Gui(GUI_MENU_ITEM_CONF_SHOW,dis_sel,0);//menu.DispItemCur    		       	          	          
    }
      if(UserSetting->SetupPref.adas_alarm_dis == 1.0)
    {
         	   dis_sel = 0;
          menu.Gui(GUI_MENU_ITEM_CONF_HIGHLIGHT,dis_sel,1);//menu.DispItemCur    	
          menu.Gui(GUI_MENU_ITEM_CONF_SHOW,dis_sel,0);//menu.DispItemCur    		       	          
    }*/
       dis_sel = UserSetting->SetupPref.adas_alarm_dis;
       menu.Gui(GUI_MENU_ITEM_CONF_HIGHLIGHT,dis_sel,1);//menu.DispItemCur    	
       menu.Gui(GUI_MENU_ITEM_CONF_SHOW,dis_sel,0);//menu.DispItemCur    		
    }
    else
    {
        ;
    }
    /** Get menu item display parameters */
    menu.Func(MENU_CAL_ITEM_DISP_PARAM, 0, 0);
    /** Item content should be loaded */
    for (i=0; i<GUI_MENU_ITEM_NUM; i++) 
    {
        if (i < menu.DispItemNum) 
        {    
            menu.Gui(GUI_MENU_ITEM_UPDATE_STRING, i, CurTab->Items[menu.DispItemStart+i]->GetTabStr());
            menu.Gui(GUI_MENU_ITEM_UPDATE_ICN_ENTER, i,0 );
            //menu.Gui(GUI_MENU_ITEM_CONF_HIGHLIGHT,i,0);//menu.DispItemCur    			
            if (APP_CHECKFLAGS(CurTab->Flags, MENU_TAB_FLAGS_LOCKED) ||
                APP_CHECKFLAGS(CurTab->Items[menu.DispItemStart+i]->Flags, MENU_ITEM_FLAGS_LOCKED)) {
                menu.Gui(GUI_MENU_ITEM_LOCK, i, 1);
                
            } 
            else 
            {
                menu.Gui(GUI_MENU_ITEM_LOCK, i, 0);
            }
            
	        if(cur_menu_tab_id == MENU_ADAS_FUNC)
            {   
                menu.Gui(GUI_MENU_ITEM_CONF_SHOW, i, 0);			
            }
            if(cur_menu_tab_id ==  MENU_ADAS_CALI)
            {   if(i==1)
                {
                    menu.Gui(GUI_MENU_ITEM_CONF_SHOW, i, 0);			
                }
            }
            if(cur_menu_tab_id == MENU_RECORDER_SETTTING)
            {   
                menu.Gui(GUI_MENU_ITEM_ENTER_SHOW, i, 0);			
            }
            if(cur_menu_tab_id == MENU_ADAS_CALI)
            {
                menu.Gui(GUI_MENU_ITEM_ENTER_SHOW, 0, 0);
            }
            if(cur_menu_tab_id==MENU_ECL_PBACK)
            {
                menu.Gui(GUI_MENU_ITEM_SHOW, i, 1);
            }
            else
            {
                menu.Gui(GUI_MENU_ITEM_SHOW, i, 0);
            }
        } else {
            menu.Gui(GUI_MENU_ITEM_HIDE, i, 0);
	        if((cur_menu_tab_id == MENU_ADAS_FUNC)||(cur_menu_tab_id == MENU_ADAS_CALI))
            {   
                menu.Gui(GUI_MENU_ITEM_CONF_HIDE, i, 0);			
            }
            if((cur_menu_tab_id == MENU_RECORDER_SETTTING)||(cur_menu_tab_id == MENU_ADAS_CALI))
            {   
                menu.Gui(GUI_MENU_ITEM_ENTER_HIDE, i, 0);			
            }			
        }
    }
    /** Highlight current item */
    menu.Gui(GUI_MENU_ITEM_HIGHLIGHT,menu.DispItemCur,1);//menu.DispItemCur
    menu.Gui(GUI_MENU_BG_UPDATE_STRING, 1,menu.TabCur);			
    menu.Gui(GUI_MENU_BG_STRING_SHOW, 1,0);	    
    //menu.Gui(GUI_MENU_ITEM_CONF_HIGHLIGHT,menu.DispItemCur,1);//menu.DispItemCur    
    /** Flush GUI */
    menu.Gui(GUI_FLUSH, 0, 0);
    return ReturnValue;	
}

int language_sel(UINT8 value,int pre_value)
{
      menu.Gui(GUI_MENU_ITEM_CONF_HIDE,pre_value,0);//menu.DispItemCur    		       		  
      menu.Gui(GUI_MENU_ITEM_CONF_HIGHLIGHT, value, 1);
      menu.Gui(GUI_MENU_ITEM_CONF_SHOW,value,0);//menu.DispItemCur    
      menu.Gui(GUI_FLUSH, 0, 0);
      return 0;	   
}

int Adas_Functions_sel(UINT8 value,int pre_value)
{
    menu.Gui(GUI_MENU_ITEM_CONF_HIDE,value,0);//menu.DispItemCur    		       		  
    switch(value)
    {
        case 0:
            if(pre_value)
            {
               menu.Gui(GUI_MENU_ITEM_CONF_HIGHLIGHT,value,3);//menu.DispItemCur   
            }
            else
            {
               menu.Gui(GUI_MENU_ITEM_CONF_HIGHLIGHT,value,2);//menu.DispItemCur             
            }
            break;
        case 1:
            if(pre_value)
            {
               menu.Gui(GUI_MENU_ITEM_CONF_HIGHLIGHT,value,3);//menu.DispItemCur   
            }
            else
            {
               menu.Gui(GUI_MENU_ITEM_CONF_HIGHLIGHT,value,2);//menu.DispItemCur                 
            }
            break;
        case 2:
            if(pre_value)
            {
               menu.Gui(GUI_MENU_ITEM_CONF_HIGHLIGHT,value,3);//menu.DispItemCur   
            }
            else 
            {
               menu.Gui(GUI_MENU_ITEM_CONF_HIGHLIGHT,value,2);//menu.DispItemCur                 
            }
    		break;
        case 3:
            if(pre_value)
            {
               menu.Gui(GUI_MENU_ITEM_CONF_HIGHLIGHT,value,3);//menu.DispItemCur   
            }
            else 
            {
               menu.Gui(GUI_MENU_ITEM_CONF_HIGHLIGHT,value,2);//menu.DispItemCur             
            }
            break;
	  default:
		    break;
    }
    menu.Gui(GUI_MENU_ITEM_CONF_SHOW,value,0);//menu.DispItemCur    
    menu.Gui(GUI_FLUSH, 0, 0);	
    return 0;
}


MENU_TAB_s* AppMenu_GetTab(UINT32 tabId)
{
    return menu_tab_ctrls[tabId]->GetTab();
}

MENU_ITEM_s* AppMenu_GetItem(UINT32 tabId, UINT32 itemId)
{
    return menu_tab_ctrls[tabId]->GetItem(itemId);
}

MENU_SEL_s* AppMenu_GetSel(UINT32 tabId, UINT32 itemId, UINT32 selId)
{
    return menu_tab_ctrls[tabId]->GetSel(itemId, selId);
}

int AppMenu_SetSelTable(UINT32 tabId, UINT32 itemId, MENU_SEL_s *selTbl)
{
    menu_tab_ctrls[tabId]->SetSelTable(itemId, selTbl);
    return 0;
}

int AppMenu_LockTab(UINT32 tabId)
{
    menu_tab_ctrls[tabId]->LockTab();
    return 0;
}

int AppMenu_UnlockTab(UINT32 tabId)
{
    menu_tab_ctrls[tabId]->UnlockTab();
    return 0;
}

int AppMenu_EnableItem(UINT32 tabId, UINT32 itemId)
{
    menu_tab_ctrls[tabId]->EnableItem(itemId);
    return 0;
}

int AppMenu_DisableItem(UINT32 tabId, UINT32 itemId)
{
    menu_tab_ctrls[tabId]->DisableItem(itemId);
    return 0;
}

int AppMenu_LockItem(UINT32 tabId, UINT32 itemId)
{
    menu_tab_ctrls[tabId]->LockItem(itemId);
    return 0;
}

int AppMenu_UnlockItem(UINT32 tabId, UINT32 itemId)
{
    menu_tab_ctrls[tabId]->UnlockItem(itemId);
    return 0;
}

int AppMenu_EnableSel(UINT32 tabId, UINT32 itemId, UINT32 selId)
{
    menu_tab_ctrls[tabId]->EnableSel(itemId, selId);
    return 0;
}

int AppMenu_DisableSel(UINT32 tabId, UINT32 itemId, UINT32 selId)
{
    menu_tab_ctrls[tabId]->DisableSel(itemId, selId);
    return 0;
}

int AppMenu_LockSel(UINT32 tabId, UINT32 itemId, UINT32 selId)
{
    menu_tab_ctrls[tabId]->LockSel(itemId, selId);
    return 0;
}

int AppMenu_UnlockSel(UINT32 tabId, UINT32 itemId, UINT32 selId)
{
    menu_tab_ctrls[tabId]->UnlockSel(itemId, selId);
    return 0;
}

