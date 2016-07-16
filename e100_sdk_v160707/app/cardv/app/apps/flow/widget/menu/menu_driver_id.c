/**
 * system/src/app/apps/flow/widget/menu/menu_driver_id.c
 *
 * Implementation of Driver Id Menu
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

#include <apps/apps.h>
#include <apps/gui/widget/menu/gui_menu_driver_id.h>
#include <apps/flow/widget/menu/menu_driver_id.h>
#include <apps/flow/widget/menu/menu.h>
#include <apps/gui/resource/gui_resource.h>
#include <system/app_pref.h>

#define DRIVER_ID_NUM_LENGTH 9

static char driver_id_num[GUI_DRIVER_ID_NUM_MAX]={'T','E','S','T','0','1','2','3',0};

static char driver_id_resource[DRIVER_ID_RESOURCE_MAX] = {
#if 0
	'0','1','2','3','4','5','6','7','8','9',
	'A','B','C','D','E','F','G','H','I','J',
	'K','L','M','N','O','P','Q','R','S','T',
	'U','V','W','X','Y','Z','-'
#else
	'1','2','3','4','5','6','7','8','9','0','-',
	'Q','W','E','R','T','Y','U','I','O','P',
	'A','S','D','F','G','H','J','K','L',
	'Z','X','C','V','B','N','M'
#endif
};

enum menu_driver_id_func_id_e {
	MENU_DRIVER_ID_SWITCH_TO_VAL = 0,
	MENU_DRIVER_ID_CUR_VAL_SET,
	MENU_DRIVER_ID_SET_NUM
};

static int menu_driver_id_func(UINT32 func_id, UINT32 param1, UINT32 param2);

static int menu_driver_id_button_up(void);
static int menu_driver_id_button_down(void);
static int menu_driver_id_button_left(void);
static int menu_driver_id_button_right(void);
static int menu_driver_id_button_set(void);
static int menu_driver_id_button_menu(void);

typedef struct menu_driver_id_op_s {
	int (*button_up)(void);
	int (*button_down)(void);
	int (*button_left)(void);
	int (*button_right)(void);
	int (*button_set)(void);
	int (*button_menu)(void);
} menu_driver_id_op_t;

static menu_driver_id_op_t menu_driver_id_op = {
	menu_driver_id_button_up,
	menu_driver_id_button_down,
	menu_driver_id_button_left,
	menu_driver_id_button_right,
	menu_driver_id_button_set,
	menu_driver_id_button_menu
};

typedef struct driver_id_menu_s {
	int val_num;
	int val_cur;
	int id_num;
	UINT8 button_mode;
	MENU_ITEM_s *item;
	int (*func)(UINT32 func_id, UINT32 param1, UINT32 param2);
	int (*gui)(UINT32 gui_cmd, UINT32 param1, UINT32 param2);
	menu_driver_id_op_t *op;
} driver_id_menu_t;

static driver_id_menu_t menu_driver_id = {0};

static int app_menu_driver_id_on(UINT32 param);
static int app_menu_driver_id_off(UINT32 param);
static int app_menu_driver_id_on_message(UINT32 msg, UINT32 param1, UINT32 param2);

WIDGET_ITEM_s widget_menu_driver_id = {
	app_menu_driver_id_on,
	app_menu_driver_id_off,
	app_menu_driver_id_on_message
};

/*************************************************************************
 * driver_id Menu APIs
 ************************************************************************/

static int menu_driver_id_func(UINT32 func_id, UINT32 param1, UINT32 param2)
{
	int rval = 0;

	switch (func_id) {
	case MENU_DRIVER_ID_SWITCH_TO_VAL:
		menu_driver_id.val_cur = param1;
		menu_driver_id.gui(GUI_VALUE_HL_UPDATE_POS, param1, 0);
		menu_driver_id.gui(GUI_KEY_FRAME_UPDATE_POS, driver_id_num[menu_driver_id.val_cur], 0);
		menu_driver_id.gui(GUI_KEY_FRAME_SHOW, 0, 0);
		menu_driver_id.gui(GUI_FLUSH, 0, 0);
		break;
	case MENU_DRIVER_ID_CUR_VAL_SET:
		{
			//AppGuiUtil_SaveDriverId(&driver_id_num[0]);
			int i=0;
			for(i =0;i< DRIVER_ID_NUM_LENGTH; i++){
				UserSetting->SetupPref.driver_id[i] = driver_id_num[i];
			}
			/* The flow only changes the preference of user.*/
			AppPref_Save();
		}
		break;
	case MENU_DRIVER_ID_SET_NUM:
		{
			driver_id_num[param1] = driver_id_resource[param2];
			menu_driver_id.id_num = param2;
		}
		break;

	default:
		AmbaPrint("The function is undefined");
		break;
	}

	return rval;
}

static int menu_driver_id_button_up(void)
{
	int val = 0;

	val = menu_driver_id.id_num - 1;
	if( val < DRIVER_ID_RESOURCE_MIN ) {
		val = DRIVER_ID_RESOURCE_MAX-1;
	}
	menu_driver_id.func(MENU_DRIVER_ID_SET_NUM, menu_driver_id.val_cur, val);
	menu_driver_id.gui(GUI_VALUE_UPDATE, menu_driver_id.val_cur, driver_id_resource[val]);
	menu_driver_id.gui(GUI_KEY_FRAME_UPDATE_POS, driver_id_resource[val], 0);
	menu_driver_id.gui(GUI_KEY_FRAME_SHOW, 0, 0);
	menu_driver_id.gui(GUI_FLUSH, 0, 0);

	return 0;
}

static int menu_driver_id_button_down(void)
{
	int val = 0;
	val = menu_driver_id.id_num + 1;
	if( val >= DRIVER_ID_RESOURCE_MAX) {
		val = DRIVER_ID_RESOURCE_MIN;
	}
	menu_driver_id.func(MENU_DRIVER_ID_SET_NUM, menu_driver_id.val_cur, val);
	menu_driver_id.gui(GUI_VALUE_UPDATE, menu_driver_id.val_cur, driver_id_resource[val]);
	menu_driver_id.gui(GUI_KEY_FRAME_UPDATE_POS, driver_id_resource[val], 0);
	menu_driver_id.gui(GUI_KEY_FRAME_SHOW, 0, 0);
	menu_driver_id.gui(GUI_FLUSH, 0, 0);

	return 0;
}

static int menu_driver_id_button_left(void)
{
	int target = menu_driver_id.val_cur-1;
	int index = 0;

	if (target < 0) {
		target = menu_driver_id.val_num-1;
	}
	menu_driver_id.func(MENU_DRIVER_ID_SWITCH_TO_VAL, target, 0);

	for(index = 0;index<DRIVER_ID_RESOURCE_MAX;index++)
	{
		if(driver_id_num[menu_driver_id.val_cur]==driver_id_resource[index]){
			menu_driver_id.id_num = index;
			break;
		} else {
			menu_driver_id.id_num = 0;
		}
	}

	return 0;
}

static int menu_driver_id_button_right(void)
{
	int target = menu_driver_id.val_cur+1;
	int index = 0;

	if (target == menu_driver_id.val_num) {
		target = 0;
	}

	menu_driver_id.func(MENU_DRIVER_ID_SWITCH_TO_VAL, target, 0);

	for(index = 0;index<DRIVER_ID_RESOURCE_MAX;index++)
	{
		if(driver_id_num[menu_driver_id.val_cur]==driver_id_resource[index]) {
			menu_driver_id.id_num = index;
			break;
		} else {
			menu_driver_id.id_num = 0;
		}
	}

	return 0;
}

static int menu_driver_id_button_set(void)
{
	return 0;
}

static int menu_driver_id_button_menu(void)
{
	menu_driver_id.func(MENU_DRIVER_ID_CUR_VAL_SET, 0, 0);
	return AppWidget_Off(WIDGET_MENU_DRIVER_ID, 0);
}

static int app_menu_driver_id_on(UINT32 param)
{
	int rval = 0;
	int i = 0;
	int index = 0;
	//struct tm tm;

	menu_driver_id.func = menu_driver_id_func;
	menu_driver_id.gui = gui_menu_driver_id_func;
	menu_driver_id.op = &menu_driver_id_op;

	menu_driver_id.val_num = DRIVER_ID_NUM_LENGTH;

	menu_driver_id.val_cur = 0;


	for(index = 0;index<DRIVER_ID_RESOURCE_MAX;index++)
	{
		if(driver_id_num[menu_driver_id.val_cur]==driver_id_resource[index]){
			menu_driver_id.id_num = index;
			break;
		} else {
			menu_driver_id.id_num = 0;
		}
	}

	/** Show driver id menu frame */
	menu_driver_id.gui(GUI_DRMENU_SHOW, 0, 0);

	for (i=0; i<DRIVER_ID_NUM_LENGTH; i++) {
		if( driver_id_num[i] == 0 ) {
			driver_id_num[i] = driver_id_resource[0];
		}
		menu_driver_id.gui(GUI_VALUE_UPDATE, i, driver_id_num[i]);
		menu_driver_id.gui(GUI_VALUE_SHOW, i, 0);
	}

	menu_driver_id.gui(GUI_KEY_FRAME_UPDATE_POS, driver_id_num[menu_driver_id.val_cur], 0);
	menu_driver_id.gui(GUI_KEY_FRAME_SHOW, 0, 0);

	menu_driver_id.gui(GUI_VALUE_HL_UPDATE_POS, menu_driver_id.val_cur, 0);

	// High light the "value cursor" and dim the "keyboard cursor".
	menu_driver_id.gui(GUI_KEY_FRAME_UPDATE_ICON, BMP_ICN_KEYBOARD_CURSOR, 0);
	menu_driver_id.gui(GUI_VALUE_HL_UPDATE_ICON, BMP_ICN_KEYBOARD_VAL_CURSOR, 0);

	/** Flush GUI */
	menu_driver_id.gui(GUI_FLUSH, 0, 0);

	return rval;
}

static int app_menu_driver_id_off(UINT32 param)
{
	int i = 0;
	//printk_co(RED,"app_menu_driver_id_off");
	menu_driver_id.gui(GUI_DRMENU_HIDE, 0, 0);
	for (i=0; i<GUI_DRIVER_ID_NUM_MAX; i++) {
		menu_driver_id.gui(GUI_VALUE_HIDE, i, 0);
	}
	menu_driver_id.gui(GUI_KEY_FRAME_HIDE, i, 0);
	//menu_driver_id.gui(GUI_HIDE_ALL, 0, 0);
	menu_driver_id.gui(GUI_FLUSH, 0, 0);
	return 0;
}

static int app_menu_driver_id_on_message(UINT32 msg, UINT32 param1, UINT32 param2)
{
	int rval = WIDGET_PASSED_MSG;

	switch (msg) {
	case HMSG_USER_UP_BUTTON:
	case HMSG_USER_IR_UP_BUTTON:
		rval = menu_driver_id.op->button_up();
		break;
	case HMSG_USER_DOWN_BUTTON:
	case HMSG_USER_IR_DOWN_BUTTON:
		rval = menu_driver_id.op->button_down();
		break;
	case HMSG_USER_LEFT_BUTTON:
	case HMSG_USER_IR_LEFT_BUTTON:
		rval = menu_driver_id.op->button_left();
		break;
	case HMSG_USER_RIGHT_BUTTON:
	case HMSG_USER_IR_RIGHT_BUTTON:
		rval = menu_driver_id.op->button_right();
		break;
	case HMSG_USER_SET_BUTTON:
	case HMSG_USER_IR_SET_BUTTON:
		rval = menu_driver_id.op->button_set();
		break;
	case HMSG_USER_MENU_BUTTON:
	case HMSG_USER_IR_MENU_BUTTON:
		rval = menu_driver_id.op->button_menu();
		break;
	default:
		rval = WIDGET_PASSED_MSG;
		break;
	}

	return rval;
}

/*************************************************************************
 * Driver ID Menu APIs for widget management
 ************************************************************************/
WIDGET_ITEM_s* app_menu_driver_id_get_widget(void)
{
	return &widget_menu_driver_id;
}

/*************************************************************************
 * Public Driver ID Menu Widget APIs
 ************************************************************************/
int app_widget_menu_driver_id_set_item(UINT32 tab_id, UINT32 item_id)
{
	menu_driver_id.item = AppMenu_GetItem(tab_id, item_id);
	return 0;
}

char* app_widget_menu_driver_id_get_num_string()
{
	return &driver_id_num[0];
}

