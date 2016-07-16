/**
  * @file src/app/apps/flow/widget/menu/connectedcam/menu_adjust.c
  *
  * Implementation of adjusting Menu
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
#include "menu_adjust.h"
#include <apps/flow/widget/menu/menu.h>
#include <apps/gui/widget/menu/gui_menu_adjust.h>
#include <system/app_pref.h>


/*************************************************************************
 * Adusting Menu definitions
 ************************************************************************/
typedef enum _MENU_ADJ_FUNC_ID_e_ {
    MENU_ADJ_SET_OP_CONT_TIMER,
    MENU_ADJ_MOVE_TO_VAL
} MENU_ADJ_FUNC_ID_e;

static int menu_adj_func(UINT32 funcId, UINT32 param1, UINT32 param2);

static int menu_adj_button_left(void);
static int menu_adj_button_right(void);
static int menu_adj_button_zoom_out(void);
static int menu_adj_button_zoom_in(void);
static int menu_adj_button_set(void);
static int menu_adj_button_menu(void);

typedef struct _MENU_ADJ_OP_s_ {
    int (*ButtonLeft)(void);
    int (*ButtonRight)(void);
    int (*button_zoom_out)(void);
    int (*button_zoom_in)(void);
    int (*ButtonSet)(void);
    int (*ButtonMenu)(void);
} MENU_ADJ_OP_s;

static MENU_ADJ_OP_s menu_adj_op = {
    menu_adj_button_left,
    menu_adj_button_right,
    menu_adj_button_zoom_out,
    menu_adj_button_zoom_in,
    menu_adj_button_set,
    menu_adj_button_menu
};

#define MENU_ADJ_OP_CONT        (0x0001)

typedef struct _MENU_ADJ_s_ {
    int ValOld;
    MENU_SEL_s *Sel;
    UINT32 Flags;
    int (*OpCont)(void);
    int (*Func)(UINT32 funcId, UINT32 param1, UINT32 param2);
    int (*Gui)(UINT32 guiCmd, UINT32 param1, UINT32 param2);
    MENU_ADJ_OP_s *Op;
} MENU_ADJ_s;

static MENU_ADJ_s menu_adj = {0};

static int app_menu_adj_on(UINT32 param);
static int app_menu_adj_off(UINT32 param);
static int app_menu_adj_on_message(UINT32 msg, UINT32 param1, UINT32 param2);

WIDGET_ITEM_s widget_menu_adj = {
    app_menu_adj_on,
    app_menu_adj_off,
    app_menu_adj_on_message
};

/*************************************************************************
 * Adjusting Menu APIs
 ************************************************************************/
#define MENU_ADJ_OP_CONT_FIRST_CD_COUNT    (10)
#define MENU_ADJ_OP_CONT_CD_COUNT    (4)
static int menu_adj_op_cont_cd_cnt = MENU_ADJ_OP_CONT_FIRST_CD_COUNT;

static void menu_adj_op_cont_cd_handler(int eid)
{
    if (eid == TIMER_UNREGISTER) {
        menu_adj_op_cont_cd_cnt = MENU_ADJ_OP_CONT_FIRST_CD_COUNT;
        return;
    }

    menu_adj_op_cont_cd_cnt--;
    if (menu_adj_op_cont_cd_cnt == 0) {
        menu_adj_op_cont_cd_cnt = MENU_ADJ_OP_CONT_CD_COUNT;
        if (APP_CHECKFLAGS(menu_adj.Flags, MENU_ADJ_OP_CONT)) {
            menu_adj.OpCont();
        } else {
            APP_ADDFLAGS(menu_adj.Flags, MENU_ADJ_OP_CONT);
        }
    }
}

static int menu_adj_func(UINT32 funcId, UINT32 param1, UINT32 param2)
{
    int ReturnValue = 0;

    switch (funcId) {
    case MENU_ADJ_MOVE_TO_VAL:
        if (param1 > menu_adj.Sel->AdjExt->Max) {
            param1 = menu_adj.Sel->AdjExt->Max;
        } else if (param1 < menu_adj.Sel->AdjExt->Min) {
            param1 = menu_adj.Sel->AdjExt->Min;
        }
        menu_adj.Sel->Val = param1;
        ReturnValue = menu_adj.Sel->AdjExt->ValSet(param1);
        break;
    case MENU_ADJ_SET_OP_CONT_TIMER:
        if (param1) {
            AppLibComSvcTimer_Register(TIMER_10HZ, menu_adj_op_cont_cd_handler);
        } else {
            AppLibComSvcTimer_Unregister(TIMER_10HZ, menu_adj_op_cont_cd_handler);
        }
        break;
    default:
        AmbaPrint("The function is undefined");
        break;
    }

    return ReturnValue;
}
static int menu_adj_button_right(void)
{
    GUI_ADJ_STATUS_s GuiStatus = {0};

    if (menu_adj.Sel->Val > menu_adj.Sel->AdjExt->Min) {

        menu_adj.Func(MENU_ADJ_MOVE_TO_VAL, menu_adj.Sel->Val-menu_adj.Sel->AdjExt->Step, 0);
        GuiStatus.Cur = menu_adj.Sel->Val;
        GuiStatus.Step = menu_adj.Sel->AdjExt->Step;
        GuiStatus.Max = menu_adj.Sel->AdjExt->Max;
        GuiStatus.Min = menu_adj.Sel->AdjExt->Min;
        menu_adj.Gui(GUI_STATUS_UPDATE, (UINT32)&GuiStatus, 0);
        menu_adj.Gui(GUI_VALUE_UPDATE, menu_adj.Sel->AdjExt->GetValStr(menu_adj.Sel->Val), 0);
        menu_adj.Gui(GUI_FLUSH, 0, 0);
    }
    return 0;
}

    
static int menu_adj_button_left(void)
{
    GUI_ADJ_STATUS_s GuiStatus = {0};

    if (menu_adj.Sel->Val < menu_adj.Sel->AdjExt->Max) {
        menu_adj.Func(MENU_ADJ_MOVE_TO_VAL, menu_adj.Sel->Val+menu_adj.Sel->AdjExt->Step, 0);
        GuiStatus.Cur = menu_adj.Sel->Val;
        GuiStatus.Step = menu_adj.Sel->AdjExt->Step;
        GuiStatus.Max = menu_adj.Sel->AdjExt->Max;
        GuiStatus.Min = menu_adj.Sel->AdjExt->Min;
        menu_adj.Gui(GUI_STATUS_UPDATE, (UINT32)&GuiStatus, 0);
        menu_adj.Gui(GUI_VALUE_UPDATE, menu_adj.Sel->AdjExt->GetValStr(menu_adj.Sel->Val), 0);
        menu_adj.Gui(GUI_FLUSH, 0, 0);
    }

    return 0;
}

static int menu_adj_button_zoom_out(void)
{
    GUI_ADJ_STATUS_s GuiStatus = {0};
    int ReturnValue = 0;

    if (menu_adj.Sel->Val > menu_adj.Sel->AdjExt->Min) {
        ReturnValue = menu_adj.Sel->Val - 5*menu_adj.Sel->AdjExt->Step;
        if (ReturnValue < menu_adj.Sel->AdjExt->Min) {
            menu_adj.Func(MENU_ADJ_MOVE_TO_VAL, menu_adj.Sel->AdjExt->Min, 0);
        } else {
            menu_adj.Func(MENU_ADJ_MOVE_TO_VAL, ReturnValue, 0);
        }
        GuiStatus.Cur = menu_adj.Sel->Val;
        GuiStatus.Step = menu_adj.Sel->AdjExt->Step;
        GuiStatus.Max = menu_adj.Sel->AdjExt->Max;
        GuiStatus.Min = menu_adj.Sel->AdjExt->Min;
        menu_adj.Gui(GUI_STATUS_UPDATE, (UINT32)&GuiStatus, 0);
        menu_adj.Gui(GUI_VALUE_UPDATE, menu_adj.Sel->AdjExt->GetValStr(menu_adj.Sel->Val), 0);
        menu_adj.Gui(GUI_FLUSH, 0, 0);
    }

    return 0;
}

static int menu_adj_button_zoom_in(void)
{
    GUI_ADJ_STATUS_s GuiStatus = {0};
    int ReturnValue = 0;

    if (menu_adj.Sel->Val < menu_adj.Sel->AdjExt->Max) {
        ReturnValue = menu_adj.Sel->Val + 5*menu_adj.Sel->AdjExt->Step;
        if (ReturnValue > menu_adj.Sel->AdjExt->Max) {
            menu_adj.Func(MENU_ADJ_MOVE_TO_VAL, menu_adj.Sel->AdjExt->Max, 0);
        } else {
            menu_adj.Func(MENU_ADJ_MOVE_TO_VAL, ReturnValue, 0);
        }
        GuiStatus.Cur = menu_adj.Sel->Val;
        GuiStatus.Step = menu_adj.Sel->AdjExt->Step;
        GuiStatus.Max = menu_adj.Sel->AdjExt->Max;
        GuiStatus.Min = menu_adj.Sel->AdjExt->Min;
        menu_adj.Gui(GUI_STATUS_UPDATE, (UINT32)&GuiStatus, 0);
        menu_adj.Gui(GUI_VALUE_UPDATE, menu_adj.Sel->AdjExt->GetValStr(menu_adj.Sel->Val), 0);
        menu_adj.Gui(GUI_FLUSH, 0, 0);
    }

    return 0;
}

static int menu_adj_button_set(void)
{
    menu_adj.ValOld = menu_adj.Sel->Val;
    menu_adj.Func(MENU_ADJ_MOVE_TO_VAL, menu_adj.Sel->Val, 0);
    /* The flow only changes the preference of user.*/
    AppPref_Save();
    return AppWidget_Off(WIDGET_MENU_ADJ, 0);
}

static int menu_adj_button_menu(void)
{
    menu_adj.Func(MENU_ADJ_MOVE_TO_VAL, menu_adj.ValOld, 0);
    return AppWidget_Off(WIDGET_MENU_ADJ, 0);
}

static int app_menu_adj_on(UINT32 param)
{
    int ReturnValue = 0;
    GUI_ADJ_STATUS_s GuiStatus = {0};

    menu_adj.Func = menu_adj_func;
    menu_adj.Gui = gui_menu_adj_func;
    menu_adj.Op = &menu_adj_op;

    menu_adj.ValOld = menu_adj.Sel->Val;

    /** Show quick menu frame */
    menu_adj.Gui(GUI_AMENU_SHOW, menu_adj.Sel->Str, 0);

    /** Selection content should be loaded */
    //menu_adj.Gui(GUI_ICON_UPDATE, 0, menu_adj.Sel->Bmp);
    //menu_adj.Gui(GUI_ICON_SHOW, 0, 0);
    //menu_adj.Gui(GUI_ICON_UPDATE, 1, menu_adj.Sel->BmpHl);
   //menu_adj.Gui(GUI_ICON_SHOW, 1, 0);

    GuiStatus.Cur = menu_adj.Sel->Val;
    GuiStatus.Step = menu_adj.Sel->AdjExt->Step;
    GuiStatus.Max = menu_adj.Sel->AdjExt->Max;
    GuiStatus.Min = menu_adj.Sel->AdjExt->Min;
    menu_adj.Gui(GUI_STATUS_UPDATE, (UINT32)&GuiStatus, 0);
    menu_adj.Gui(GUI_STATUS_SHOW, 0, 0);

    /** Show adjustment value */
    menu_adj.Gui(GUI_VALUE_UPDATE, menu_adj.Sel->AdjExt->GetValStr(menu_adj.Sel->Val), 0);
    menu_adj.Gui(GUI_VALUE_SHOW, 0, 0);

    /** Flush GUI */
    menu_adj.Gui(GUI_FLUSH, 0, 0);

    return ReturnValue;
}

static int app_menu_adj_off(UINT32 param)
{
    APP_REMOVEFLAGS(menu_adj.Flags, MENU_ADJ_OP_CONT);
    menu_adj.Func(MENU_ADJ_SET_OP_CONT_TIMER, 0, 0);
    if (menu_adj.Sel->Val != menu_adj.ValOld) {
        menu_adj.Sel->AdjExt->ValSet(menu_adj.ValOld);
    }
    menu_adj.Gui(GUI_AMENU_HIDE, 0, 0);
    menu_adj.Gui(GUI_ICON_HIDE, 0, 0);
    menu_adj.Gui(GUI_ICON_HIDE, 1, 0);
    menu_adj.Gui(GUI_STATUS_HIDE, 0, 0);
    menu_adj.Gui(GUI_VALUE_HIDE, 0, 0);
    menu_adj.Gui(GUI_FLUSH, 0, 0);

    return 0;
}

static int app_menu_adj_on_message(UINT32 msg, UINT32 param1, UINT32 param2)
{
    int ReturnValue = WIDGET_PASSED_MSG;

    switch (msg) {
    case HMSG_USER_UP_BUTTON:
    case HMSG_USER_IR_UP_BUTTON:
    case HMSG_USER_DOWN_BUTTON:
    case HMSG_USER_IR_DOWN_BUTTON:
        ReturnValue = 0;
        break;
    case HMSG_USER_LEFT_BUTTON:
    case HMSG_USER_IR_LEFT_BUTTON:
        APP_REMOVEFLAGS(menu_adj.Flags, MENU_ADJ_OP_CONT);
        menu_adj.Func(MENU_ADJ_SET_OP_CONT_TIMER, 0, 0);
        ReturnValue = menu_adj.Op->ButtonLeft();
        menu_adj.Func(MENU_ADJ_SET_OP_CONT_TIMER, 1, 0);
        menu_adj.OpCont = menu_adj.Op->ButtonLeft;
        break;
    case HMSG_USER_RIGHT_BUTTON:
    case HMSG_USER_IR_RIGHT_BUTTON:
        APP_REMOVEFLAGS(menu_adj.Flags, MENU_ADJ_OP_CONT);
        menu_adj.Func(MENU_ADJ_SET_OP_CONT_TIMER, 0, 0);
        ReturnValue = menu_adj.Op->ButtonRight();
        menu_adj.Func(MENU_ADJ_SET_OP_CONT_TIMER, 1, 0);
        menu_adj.OpCont = menu_adj.Op->ButtonRight;
        break;
    case HMSG_USER_ZOOM_OUT_BUTTON:
    case HMSG_USER_IR_ZOOM_OUT_BUTTON:
        APP_REMOVEFLAGS(menu_adj.Flags, MENU_ADJ_OP_CONT);
        menu_adj.Func(MENU_ADJ_SET_OP_CONT_TIMER, 0, 0);
        ReturnValue = menu_adj.Op->button_zoom_out();
        menu_adj.Func(MENU_ADJ_SET_OP_CONT_TIMER, 1, 0);
        menu_adj.OpCont = menu_adj.Op->button_zoom_out;
        break;
    case HMSG_USER_ZOOM_IN_BUTTON:
    case HMSG_USER_IR_ZOOM_IN_BUTTON:
        APP_REMOVEFLAGS(menu_adj.Flags, MENU_ADJ_OP_CONT);
        menu_adj.Func(MENU_ADJ_SET_OP_CONT_TIMER, 0, 0);
        ReturnValue = menu_adj.Op->button_zoom_in();
        menu_adj.Func(MENU_ADJ_SET_OP_CONT_TIMER, 1, 0);
        menu_adj.OpCont = menu_adj.Op->button_zoom_in;
        break;
    case HMSG_USER_SET_BUTTON:
    case HMSG_USER_IR_SET_BUTTON:
        ReturnValue = menu_adj.Op->ButtonSet();
        break;
    case HMSG_USER_MENU_BUTTON:
    case HMSG_USER_IR_MENU_BUTTON:
        ReturnValue = menu_adj.Op->ButtonMenu();
        break;
    case HMSG_USER_LEFT_BUTTON_CLR:
    case HMSG_USER_IR_LEFT_BUTTON_CLR:
    case HMSG_USER_RIGHT_BUTTON_CLR:
    case HMSG_USER_IR_RIGHT_BUTTON_CLR:
    case HMSG_USER_ZOOM_IN_BUTTON_CLR:
    case HMSG_USER_IR_ZOOM_IN_BUTTON_CLR:
    case HMSG_USER_ZOOM_OUT_BUTTON_CLR:
    case HMSG_USER_IR_ZOOM_OUT_BUTTON_CLR:
        menu_adj.Func(MENU_ADJ_SET_OP_CONT_TIMER, 0, 0);
        APP_REMOVEFLAGS(menu_adj.Flags, MENU_ADJ_OP_CONT);
        break;
    default:
        ReturnValue = WIDGET_PASSED_MSG;
        break;
    }

    return ReturnValue;
}

/*************************************************************************
 * Quick Menu APIs for widget management
 ************************************************************************/
WIDGET_ITEM_s* AppMenuAdj_GetWidget(void)
{
    return &widget_menu_adj;
}

/*************************************************************************
 * Public Quick Menu Widget APIs
 ************************************************************************/
int AppMenuAdj_SetSel(UINT32 tabId, UINT32 itemId, UINT32 selId)
{
    menu_adj.Sel = AppMenu_GetSel(tabId, itemId, selId);
    return 0;
}
