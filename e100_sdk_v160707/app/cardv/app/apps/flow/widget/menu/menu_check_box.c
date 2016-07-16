/**
 * system/src/app/apps/flow/widget/menu/menu_check_box.c
 *
 * Implementation of Check Box Menu
 *
 * History:
 *	  2010/02/01 - [Check Box] created file
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
#include <apps/gui/widget/menu/gui_menu_check_box.h>
#include <apps/flow/widget/menu/menu_check_box.h>
#include <apps/flow/widget/menu/menu.h>
#include <apps/gui/resource/gui_resource.h>
#include <system/app_pref.h>

/*************************************************************************
 * CKBX definitions
 ************************************************************************/

/** CKBX funcions */
typedef enum _MENU_CHBX_FUNC_ID_e_ {
	MENU_CKBX_SWITCH_TO_ITEM = 0,
	MENU_CKBX_CUR_ITEM_SET
} MENU_CHBX_FUNC_ID_e;

#define MENU_ITEM_PAGE_CHANGED	(0x01)
static int menu_ckbx_func(UINT32 func_id, UINT32 param1, UINT32 param2);

/** CKBX operations */
static int menu_ckbx_button_up(void);
static int menu_ckbx_button_down(void);
static int menu_ckbx_button_left(void);
static int menu_ckbx_button_right(void);
static int menu_ckbx_button_set(void);
static int menu_ckbx_button_menu(void);

typedef struct menu_ckbx_op_s {
	int (*button_up)(void);
	int (*button_down)(void);
	int (*button_left)(void);
	int (*button_right)(void);
	int (*button_set)(void);
	int (*button_menu)(void);
} menu_ckbx_op_t;

static menu_ckbx_op_t menu_ckbx_op = {
	menu_ckbx_button_up,
	menu_ckbx_button_down,
	menu_ckbx_button_left,
	menu_ckbx_button_right,
	menu_ckbx_button_set,
	menu_ckbx_button_menu
};

/** CKBX status */
typedef struct menu_ckbx_s {
	int item_total_num;
	int curr_item_idx;
	CHECK_BOX_ITEM_S * item_addr;
	int curr_obj_idx;
	Check_Box_Set_Func p_Set;
	MENU_ITEM_s *item;
	int (*func)(UINT32 func_id, UINT32 param1, UINT32 param2);
	int (*gui)(UINT32 gui_cmd, UINT32 param1, UINT32 param2);
	menu_ckbx_op_t *op;
} menu_ckbx_t;

static menu_ckbx_t menu_ckbx;

/** Menu interface */
static int app_menu_ckbx_on(UINT32 param);
static int app_menu_ckbx_off(UINT32 param);
static int app_menu_ckbx_on_message(UINT32 msg, UINT32 param1, UINT32 param2);

WIDGET_ITEM_s widget_menu_ckbx = {
	app_menu_ckbx_on,
	app_menu_ckbx_off,
	app_menu_ckbx_on_message
};

/*************************************************************************
 * Menu ckbx_ APIs
 ************************************************************************/
/** Menu ckbx_ functions */

static int need_update_strings = 0;

static int menu_switch_to_item(int item_idx)
{
	int rval = 0;
	int i = 0;

	menu_ckbx.curr_item_idx = item_idx;

	// update highlight
	menu_ckbx.gui(GUI_CKBXMENU_ITEM_HIGHLIGHT,menu_ckbx.curr_obj_idx,0 );

	if( need_update_strings == 1)
	{
		if(menu_ckbx.curr_obj_idx==0){
			// up
			for( i=0; i < GUI_MENU_CHECK_BOX_ITEM_NUM;i++)
			{
				if( menu_ckbx.curr_item_idx >= 0 )
				{
					CHECK_BOX_ITEM_S * p_item;
					p_item =  &(menu_ckbx.item_addr[menu_ckbx.curr_item_idx + i]);
					menu_ckbx.gui(GUI_CKBXMENU_ITEM_UPDATE_STRING,i,p_item->string_id );
					menu_ckbx.gui(GUI_CKBXMENU_ITEM_UPDATE_BOX,i,p_item->check_or_not );
				}
			}
		}else if(menu_ckbx.curr_obj_idx == (GUI_MENU_CHECK_BOX_ITEM_NUM - 1)){
			// down
			for( i=0; i < GUI_MENU_CHECK_BOX_ITEM_NUM;i++)
			{
				if( menu_ckbx.curr_item_idx < menu_ckbx.item_total_num )
				{
					CHECK_BOX_ITEM_S * p_item;
					p_item = &(menu_ckbx.item_addr[menu_ckbx.curr_item_idx - menu_ckbx.curr_obj_idx + i]);
					menu_ckbx.gui(GUI_CKBXMENU_ITEM_UPDATE_STRING,i,p_item->string_id );
					menu_ckbx.gui(GUI_CKBXMENU_ITEM_UPDATE_BOX,i,p_item->check_or_not );
				}
			}
		}
	}
	menu_ckbx.gui(GUI_FLUSH, 0, 0);

	return rval;
}

static int menu_ckbx_func(UINT32 func_id, UINT32 param1, UINT32 param2)
{
	int rval = 0;

	switch (func_id) {
	case MENU_CKBX_SWITCH_TO_ITEM:
		rval = menu_switch_to_item(param1);
		break;
	case MENU_CKBX_CUR_ITEM_SET:
	{
		CHECK_BOX_ITEM_S * p_item;
		int check_status;
		p_item = &(menu_ckbx.item_addr[menu_ckbx.curr_item_idx]);
		check_status = p_item->check_or_not;
		if(check_status == 0){
			menu_ckbx.gui(GUI_CKBXMENU_ITEM_UPDATE_BOX,menu_ckbx.curr_obj_idx,1 );
			p_item->check_or_not = 1;
		} else {
			menu_ckbx.gui(GUI_CKBXMENU_ITEM_UPDATE_BOX,menu_ckbx.curr_obj_idx,0 );
			p_item->check_or_not = 0;
		}
		menu_ckbx.gui(GUI_FLUSH, 0, 0);
		menu_ckbx.p_Set(menu_ckbx.curr_item_idx);

		break;
	}
	default:
		AmbaPrint("The function is undefined");
		break;
	}

	return rval;
}

static int menu_ckbx_button_up(void)
{
	int item_target = 0;

	if(menu_ckbx.curr_item_idx > 0)
		item_target = menu_ckbx.curr_item_idx - 1;
	else
		item_target = 0;

	if( menu_ckbx.item_total_num > GUI_MENU_CHECK_BOX_ITEM_NUM ){
		if(menu_ckbx.curr_obj_idx > 0 ){
			menu_ckbx.curr_obj_idx = menu_ckbx.curr_obj_idx - 1;
			need_update_strings = 0;
		}else{
			menu_ckbx.curr_obj_idx = 0;
			if(item_target>=0)
				need_update_strings = 1;
		}
	} else {
		if(menu_ckbx.curr_obj_idx > 0 ){
			menu_ckbx.curr_obj_idx = menu_ckbx.curr_obj_idx - 1;
			need_update_strings = 0;
		}else{
			menu_ckbx.curr_obj_idx = 0;
		}
		need_update_strings = 0;
	}

	menu_ckbx.func(MENU_CKBX_SWITCH_TO_ITEM, item_target, 0);

	return 0;
}

static int menu_ckbx_button_down(void)
{
	int item_target = 0;

	if(menu_ckbx.curr_item_idx < ( menu_ckbx.item_total_num - 1 ) )
		item_target = menu_ckbx.curr_item_idx + 1;
	else
		item_target = menu_ckbx.curr_item_idx;

	if( menu_ckbx.item_total_num > GUI_MENU_CHECK_BOX_ITEM_NUM ){
		if(menu_ckbx.curr_obj_idx < ( GUI_MENU_CHECK_BOX_ITEM_NUM - 1 ) ){
			menu_ckbx.curr_obj_idx = menu_ckbx.curr_obj_idx + 1;
			need_update_strings = 0;
		}else{
			menu_ckbx.curr_obj_idx = GUI_MENU_CHECK_BOX_ITEM_NUM - 1;
			if(item_target<=( menu_ckbx.item_total_num - 1 ))
				need_update_strings = 1;
		}
	}
	else {
		need_update_strings = 0;
		if(menu_ckbx.curr_obj_idx < ( menu_ckbx.item_total_num - 1 ) ){
			menu_ckbx.curr_obj_idx = menu_ckbx.curr_obj_idx + 1;
		}
	}

	menu_ckbx.func(MENU_CKBX_SWITCH_TO_ITEM, item_target, 0);

	return 0;
}

static int menu_ckbx_button_left(void)
{
	return 0;
}

static int menu_ckbx_button_right(void)
{
	return 0;
}

static int menu_ckbx_button_set(void)
{
//#if !defined(ENABLE_DUAL_CHANNEL_ARD)
//	app_beep_play_beep(BEEP_OPERATION, 0);
//#endif
	return menu_ckbx.func(MENU_CKBX_CUR_ITEM_SET, 0, 0);
}

static int menu_ckbx_button_menu(void)
{
	return AppWidget_Off(WIDGET_MENU_CKBX, 0);
}

static int app_menu_ckbx_on(UINT32 param)
{
	int rval = 0;
	int i = 0;

	menu_ckbx.func = menu_ckbx_func;
	menu_ckbx.gui = gui_menu_check_box_func;
	menu_ckbx.op = &menu_ckbx_op;

	// init the item string and icon and highlight
	for( i=0; i < GUI_MENU_CHECK_BOX_ITEM_NUM;i++)
	{
		if( i < menu_ckbx.item_total_num )
		{
			CHECK_BOX_ITEM_S * p_item;
			p_item = &(menu_ckbx.item_addr[i]);

			menu_ckbx.gui(GUI_CKBXMENU_ITEM_UPDATE_STRING,i,p_item->string_id );
			menu_ckbx.gui(GUI_CKBXMENU_ITEM_UPDATE_BOX,i,p_item->check_or_not );
		}
	}

	menu_ckbx.gui(GUI_CKBXMENU_ITEM_HIGHLIGHT,0,0 );

	// show the icons and strings
	menu_ckbx.gui(GUI_CKBXMENU_TITLE_SHOW,0,0 );
	for( i=0; i < GUI_MENU_CHECK_BOX_ITEM_NUM;i++)
	{
		if( i < menu_ckbx.item_total_num )
		{
			menu_ckbx.gui(GUI_CKBXMENU_ITEM_SHOW,i,0 );
		}
	}
	menu_ckbx.gui(GUI_FLUSH, 0, 0);

	return rval;
}

static int app_menu_ckbx_off(UINT32 param)
{
	int i = 0;

	// hide the icons and strings
	menu_ckbx.gui(GUI_CKBXMENU_TITLE_HIDE,0,0 );
	for( i=0; i < GUI_MENU_CHECK_BOX_ITEM_NUM;i++)
	{
		if( i < menu_ckbx.item_total_num )
		{
			menu_ckbx.gui(GUI_CKBXMENU_ITEM_HIDE,i,0 );
		}
	}
	menu_ckbx.gui(GUI_FLUSH, 0, 0);

	return 0;
}

static int app_menu_ckbx_on_message(UINT32 msg, UINT32 param1, UINT32 param2)
{
	int rval = WIDGET_PASSED_MSG;

	switch (msg) {
	case HMSG_USER_UP_BUTTON:
	case HMSG_USER_IR_UP_BUTTON:
		rval = menu_ckbx.op->button_up();
		break;
	case HMSG_USER_DOWN_BUTTON:
	case HMSG_USER_IR_DOWN_BUTTON:
		rval = menu_ckbx.op->button_down();
		break;
	case HMSG_USER_LEFT_BUTTON:
	case HMSG_USER_IR_LEFT_BUTTON:
		rval = menu_ckbx.op->button_left();
		break;
	case HMSG_USER_RIGHT_BUTTON:
	case HMSG_USER_IR_RIGHT_BUTTON:
		rval = menu_ckbx.op->button_right();
		break;
	case HMSG_USER_SET_BUTTON:
	case HMSG_USER_IR_SET_BUTTON:
		rval = menu_ckbx.op->button_set();
		break;
	case HMSG_USER_MENU_BUTTON:
	case HMSG_USER_IR_MENU_BUTTON:
		rval = menu_ckbx.op->button_menu();
		break;
	default:
		rval = WIDGET_PASSED_MSG;
		break;
	}

	return rval;
}

/*************************************************************************
 * Menu ckbx_ APIs for widget management
 ************************************************************************/
WIDGET_ITEM_s* app_menu_ckbx_get_widget(void)
{
	return &widget_menu_ckbx;
}

int app_widget_menu_check_box_set_item(UINT32 tab_id, UINT32 item_id)
{
	menu_ckbx.item = AppMenu_GetItem(tab_id, item_id);
	return 0;
}

int app_widget_check_box_init_item( int total_item, CHECK_BOX_ITEM_S * p_item, Check_Box_Set_Func set_func )
{
	menu_ckbx.item_total_num = total_item;
	menu_ckbx.item_addr = p_item;
	menu_ckbx.curr_item_idx = 0;
	menu_ckbx.curr_obj_idx = 0;
	menu_ckbx.p_Set = set_func;
	return 0;
}


