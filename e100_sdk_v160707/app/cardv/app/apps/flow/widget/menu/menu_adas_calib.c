/**
  * @file src/app/apps/flow/widget/menu/connectedcam/menu_time.c
  *
  * Implementation of Time Menu
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
#include "menu_adas_calib.h"
#include <apps/flow/widget/menu/menu.h>
#include <apps/gui/widget/menu/gui_menu_adas_calib.h>
#include <AmbaRTC.h>
#include <system/app_pref.h>


#define DEBUG_MENU_ADAS_CALIB
#if defined(DEBUG_MENU_ADAS_CALIB)
#define DBGMSG AmbaPrint
#define DBGMSGc(x) AmbaPrintColor(GREEN,x)
#define DBGMSGc2 AmbaPrintColor
#else
#define DBGMSG(...)
#define DBGMSGc(...)
#define DBGMSGc2(...)
#endif

/*************************************************************************
 * Time Menu definitions
 ************************************************************************/
typedef enum _MENU_ADAS_CALIB_FUNC_ID_e_ {
	MENU_ADAS_CALIB_CUR_SEL_SET = 0,
	MENU_ADAS_CALIB_SWITCH_CUR_SEL,
	MENU_ADAS_CALIB_SHOW_CUR_CALIB,
	MENU_ADAS_CALIB_SET_OP_CONT_TIMER
} MENU_ADAS_CALIB_FUNC_ID_e;

#define MENU_ADAS_CALIB_OP_CONT		(0x0001)

static int sky_line_position;
static int hood_line_position;
int adas_calib_selection;

static int menu_adas_calib_func(UINT32 funcId, UINT32 param1, UINT32 param2);

static int menu_adas_calib_button_up(void);
static int menu_adas_calib_button_down(void);
static int menu_adas_calib_button_left(void);
static int menu_adas_calib_button_right(void);
static int menu_adas_calib_button_set(void);
static int menu_adas_calib_button_menu(void);

typedef struct _MENU_ADAS_CALIB_OP_s_ {
    int (*ButtonUp)(void);
    int (*ButtonDown)(void);
    int (*ButtonLeft)(void);
    int (*ButtonRight)(void);
    int (*ButtonSet)(void);
    int (*ButtonMenu)(void);
} MENU_ADAS_CALIB_OP_s;

static MENU_ADAS_CALIB_OP_s menu_adas_calib_op = {
    menu_adas_calib_button_up,
    menu_adas_calib_button_down,
    menu_adas_calib_button_left,
    menu_adas_calib_button_right,
    menu_adas_calib_button_set,
    menu_adas_calib_button_menu
};

typedef struct _MENU_ADAS_CALIB_s_ {
    int ValNum;
    int ValCur;
    MENU_ITEM_s *Item;
    int (*Func)(UINT32 funcId, UINT32 param1, UINT32 param2);
    int (*Gui)(UINT32 gui_cmd, UINT32 param1, UINT32 param2);
    MENU_ADAS_CALIB_OP_s *Op;
} MENU_ADAS_CALIB_s;

static MENU_ADAS_CALIB_s menu_adas_calib = {0};

static int app_menu_adas_calib_on(UINT32 param);
static int app_menu_adas_calib_off(UINT32 param);
static int app_menu_adas_calib_on_message(UINT32 msg, UINT32 param1, UINT32 param2);

WIDGET_ITEM_s widget_menu_adas_calib = {
    app_menu_adas_calib_on,
    app_menu_adas_calib_off,
    app_menu_adas_calib_on_message
};

/*************************************************************************
 * ADAS calibration menu APIs
 ************************************************************************/

int menu_adas_calib_per2pos(int dev_w, int dev_h, float dev_pix_ar, int video_w, int video_h, int layout_h, int partition)
{
	float border_ratio = 0.0, border = 0.0, video_ar = 0.0, dev_ar = 0.0;
	int rval;

	dev_ar = (float)dev_h /((float)dev_w * dev_pix_ar);
	video_ar = (float)video_h /((float)video_w * dev_pix_ar);
	border_ratio = (1.0 - (video_ar/dev_ar))/2.0;
	//gui_ratio = (float)layout_h / (float)dev_h;
	border = (float)border_ratio * layout_h;

	/** partition% = (output_height - border)/(layout_h - 2*border) */
	rval = ((float)partition/100.0)*(layout_h-2*border)+border;
	return rval;
}

int menu_adas_calib_show(void)
{
    int ReturnValue = 0;
    int YPosition;
    
    AMP_DISP_INFO_s DispDev = {0};
    AMP_DISP_WINDOW_CFG_s Window;
    APPLIB_GRAPHIC_DIRECTDRAW_CNT_s ddCnt;

    ReturnValue = AppLibDisp_GetDeviceInfo(DISP_CH_DCHAN, &DispDev);
    ReturnValue = AppLibDisp_GetWindowConfig(DISP_CH_DCHAN, AppLibDisp_GetWindowId(DISP_CH_DCHAN, 0), &Window);

    //_AppLibGraph_GetDirectDrawBuf(GRAPH_CH_DCHAN, &ddCnt);
    //DBGMSG("== %s (%d) == GUI layout (W, H) = (%d, %d)",__FUNCTION__, __LINE__, ddCnt.Width, ddCnt.Height);

    /* Update sky position */
    YPosition = menu_adas_calib_per2pos(DispDev.DeviceInfo.VoutWidth, DispDev.DeviceInfo.VoutHeight, DispDev.DeviceInfo.DevPixelAr,
                                        Window.TargetAreaOnPlane.Width, Window.TargetAreaOnPlane.Height,
                                        480, sky_line_position);
    //AppLibGraph_UpdatePosition(GRAPH_CH_DCHAN, GOBJ_ADAS_CALIB_SKY, 0, YPosition);
    menu_adas_calib.Gui(GUI_UPDATE_ADAS_CALIB_SKY, 1, YPosition);   

    //menu_adas_calib.Gui(GUI_SHOW_ADAS_CALIB_SKY, 0, 0);

    /* Update hood position */
    YPosition = menu_adas_calib_per2pos(DispDev.DeviceInfo.VoutWidth, DispDev.DeviceInfo.VoutHeight, DispDev.DeviceInfo.DevPixelAr,
                                        Window.TargetAreaOnPlane.Width, Window.TargetAreaOnPlane.Height,
                                        480, hood_line_position);
    //AppLibGraph_UpdatePosition(GRAPH_CH_DCHAN, GOBJ_ADAS_CALIB_HOOD, 0, YPosition);
    menu_adas_calib.Gui(GUI_UPDATE_ADAS_CALIB_HOOD, 1, YPosition);   
    
	//menu_adas_calib.Gui(GUI_SHOW_ADAS_CALIB_HOOD, 0, 0);   

    return ReturnValue;
}

static int menu_adas_calib_func(UINT32 funcId, UINT32 param1, UINT32 param2)
{
    int ReturnValue = 0;

    switch (funcId) {
    case MENU_ADAS_CALIB_CUR_SEL_SET:
        UserSetting->VAPref.AdasCalibSky = sky_line_position;
        UserSetting->VAPref.AdasCalibHood = hood_line_position;
        AppPref_Save();
        break;

    case MENU_ADAS_CALIB_SWITCH_CUR_SEL:
        if (adas_calib_selection == ADAS_CALIB_SELEC_SKY) {
            adas_calib_selection = ADAS_CALIB_SELEC_HOOD;
        } else {
            adas_calib_selection = ADAS_CALIB_SELEC_SKY;
        }
        break;

    case MENU_ADAS_CALIB_SHOW_CUR_CALIB:
        DBGMSG("== %s (%d) == SKY = %d%%  HOOD = %d%%",__FUNCTION__, __LINE__,sky_line_position,hood_line_position);
        menu_adas_calib_show();
        menu_adas_calib.Gui(GUI_FLUSH, 0, 0); 
        break;
        
    default:
        break;
    }

    return ReturnValue;
}

static int menu_adas_calib_button_up(void)
{
	if (adas_calib_selection == ADAS_CALIB_SELEC_SKY) {
		if (sky_line_position > 0) 
			sky_line_position --;
	} else {
		if (hood_line_position > 0)
			hood_line_position --;
	}
    menu_adas_calib.Func(MENU_ADAS_CALIB_SHOW_CUR_CALIB, 0, 0);
    
    return 0;
}

static int menu_adas_calib_button_down(void)
{
	if (adas_calib_selection == ADAS_CALIB_SELEC_SKY) {
		if (sky_line_position < 100) 
			sky_line_position ++;
	} else {
		if (hood_line_position < 100)
			hood_line_position ++;
	}
    menu_adas_calib.Func(MENU_ADAS_CALIB_SHOW_CUR_CALIB, 0, 0);
    return 0;
}

static int menu_adas_calib_button_left(void)
{
    menu_adas_calib.Func(MENU_ADAS_CALIB_SWITCH_CUR_SEL, 0, 0);
    return 0;
}

static int menu_adas_calib_button_right(void)
{
    menu_adas_calib.Func(MENU_ADAS_CALIB_SWITCH_CUR_SEL, 0, 0);
    return 0;
}

static int menu_adas_calib_button_set(void)
{
    menu_adas_calib.Func(MENU_ADAS_CALIB_CUR_SEL_SET, 0, 0);
    return AppWidget_Off(WIDGET_MENU_ADAS_CALIB, 0);
}

static int menu_adas_calib_button_menu(void)
{
    return AppWidget_Off(WIDGET_MENU_ADAS_CALIB, 0);
}

static int app_menu_adas_calib_on(UINT32 param)
{
    int ReturnValue = 0;
    menu_adas_calib.Func = menu_adas_calib_func;
    menu_adas_calib.Gui = gui_menu_adas_calib_func;
    menu_adas_calib.Op = &menu_adas_calib_op;
    sky_line_position = UserSetting->VAPref.AdasCalibSky;
    hood_line_position = UserSetting->VAPref.AdasCalibHood;
    adas_calib_selection = ADAS_CALIB_SELEC_SKY;
    menu_adas_calib.Gui(GUI_SHOW_ADAS_CALIB_SKY, 0, 0);
    menu_adas_calib.Gui(GUI_SHOW_ADAS_CALIB_HOOD, 0, 0);
    menu_adas_calib.Func(MENU_ADAS_CALIB_SHOW_CUR_CALIB, 0, 0);
    return ReturnValue;
}

static int app_menu_adas_calib_off(UINT32 param)
{
	menu_adas_calib.Gui(GUI_HIDE_ADAS_CALIB_SKY, 0, 0);
	menu_adas_calib.Gui(GUI_HIDE_ADAS_CALIB_HOOD, 0, 0);
	menu_adas_calib.Gui(GUI_FLUSH, 0, 0);
    return 0;
}

static int app_menu_adas_calib_on_message(UINT32 msg, UINT32 param1, UINT32 param2)
{
    int ReturnValue = WIDGET_PASSED_MSG;

    switch (msg) {
    case HMSG_USER_UP_BUTTON:
    case HMSG_USER_IR_UP_BUTTON:
        ReturnValue = menu_adas_calib.Op->ButtonUp();
        break;

    case HMSG_USER_DOWN_BUTTON:
    case HMSG_USER_IR_DOWN_BUTTON:
        ReturnValue = menu_adas_calib.Op->ButtonDown();
        break;

    case HMSG_USER_LEFT_BUTTON:
    case HMSG_USER_IR_LEFT_BUTTON:
        ReturnValue = menu_adas_calib.Op->ButtonLeft();
        break;

    case HMSG_USER_RIGHT_BUTTON:
    case HMSG_USER_IR_RIGHT_BUTTON:
        ReturnValue = menu_adas_calib.Op->ButtonRight();
        break;

    case HMSG_USER_SET_BUTTON:
    case HMSG_USER_IR_SET_BUTTON:
         ReturnValue = menu_adas_calib.Op->ButtonSet();
         break;

    case HMSG_USER_MENU_BUTTON:
    case HMSG_USER_IR_MENU_BUTTON:
        ReturnValue = menu_adas_calib.Op->ButtonMenu();
        break;

    default:
        ReturnValue = WIDGET_PASSED_MSG;
        break;
    }

    return ReturnValue;
}

/*************************************************************************
 * ADAS clibration menu APIs for widget management
 ************************************************************************/
WIDGET_ITEM_s* AppAdasCalib_GetWidget(void)
{
    return &widget_menu_adas_calib;
}

/*************************************************************************
 * Public ADAS clibration Menu Widget APIs
 ************************************************************************/
int AppAdasCalib_SetItem(UINT32 tabId, UINT32 itemId)
{
    menu_adas_calib.Item = AppMenu_GetItem(tabId, itemId);
    return 0;
}
