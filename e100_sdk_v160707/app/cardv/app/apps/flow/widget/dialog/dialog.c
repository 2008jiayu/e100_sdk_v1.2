/**
  * @file src/app/apps/flow/widget/dialog/dialog.c
  *
  *  Implementation of Dialog
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

#include <apps/flow/widget/dialog/dialog.h>
#include <apps/gui/widget/dialog/gui_dialog.h>

/*************************************************************************
 * Dialog definitions
 ************************************************************************/

static UINT32 *DialogSubjecStrTable;

typedef enum _DIALOG_FUNC_ID_e_ {
    DIALOG_CUR_LEFT = 0,
    DIALOG_CUR_RIGHT,
    DIALOG_CUR_SET
} DIALOG_FUNC_ID_e;

static int dialog_func(UINT32 funcId, UINT32 param1, UINT32 param2);

static int dialog_button_up(void);
static int dialog_button_down(void);
static int dialog_button_left(void);
static int dialog_button_right(void);
static int dialog_button_set(void);
static int dialog_button_menu(void);

typedef struct _DIALOG_OP_s_ {
    int (*ButtonUp)(void);
    int (*ButtonDown)(void);
    int (*ButtonLeft)(void);
    int (*ButtonRight)(void);
    int (*ButtonSet)(void);
    int (*ButtonMenu)(void);
} DIALOG_OP_s;

static DIALOG_OP_s dialog_op = {
    dialog_button_up,
    dialog_button_down,
    dialog_button_left,
    dialog_button_right,
    dialog_button_set,
    dialog_button_menu
};

typedef struct _DIALOG_s_ {
    UINT8 Type;
    INT8 Select;
    UINT32 Strings;
    dialog_set_handler Set;
    int (*Func)(UINT32 funcId, UINT32 param1, UINT32 param2);
    int (*Gui)(UINT32 guiCmd, UINT32 param1, UINT32 param2);
    DIALOG_OP_s *Op;
} DIALOG_s;

static DIALOG_s dialog = {0};

static int AppDialog_On(UINT32 param);
static int AppDialog_Off(UINT32 param);
static int AppDialog_OnMessage(UINT32 msg, UINT32 param1, UINT32 param2);

WIDGET_ITEM_s widget_dialog = {
    AppDialog_On,
    AppDialog_Off,
    AppDialog_OnMessage
};

/*************************************************************************
 * Dialog APIs
 ************************************************************************/
static int dialog_func(UINT32 funcId, UINT32 param1, UINT32 param2)
{
    int ReturnValue = 0;

    switch (funcId) {
    case DIALOG_CUR_LEFT:
        if (dialog.Type == DIALOG_TYPE_OK) {
            dialog.Select = DIALOG_SEL_OK;
        } else {
            dialog.Gui(GUI_DIALOG_BTN_HIGHLIGHT, dialog.Select, 0);
            dialog.Select--;
            if (dialog.Select < GUI_DIALOG_BTN_CANCEL) {
                dialog.Select = GUI_DIALOG_BTN_NUM - 1;
            }
        }
        break;
    case DIALOG_CUR_RIGHT:
        if (dialog.Type == DIALOG_TYPE_OK) {
            dialog.Select = DIALOG_SEL_OK;
        } else {
            dialog.Gui(GUI_DIALOG_BTN_HIGHLIGHT, dialog.Select, 0);
            dialog.Select++;
            if (dialog.Select == GUI_DIALOG_BTN_NUM) {
                dialog.Select = GUI_DIALOG_BTN_CANCEL;
            }
        }
        break;
    case DIALOG_CUR_SET:
        dialog.Set(dialog.Select, 0, 0);
        break;
    default:
        AmbaPrint("[app_dialog] The function is undefined");
        break;
    }

    dialog.Gui(GUI_DIALOG_BTN_HIGHLIGHT, dialog.Select, 1);
    dialog.Gui(GUI_FLUSH, 0, 0);

    return ReturnValue;
}

static int dialog_button_up(void)
{
    return 0;
}

static int dialog_button_down(void)
{
    return 0;
}

static int dialog_button_left(void)
{
    dialog.Func(DIALOG_CUR_LEFT, 0, 0);

    return 0;
}

static int dialog_button_right(void)
{
    dialog.Func(DIALOG_CUR_RIGHT, 0, 0);

    return 0;
}

static int dialog_button_set(void)
{
#ifdef CONFIG_APP_ARD
    AppWidget_Off(WIDGET_ALL, 0);
#else
    AppWidget_Off(WIDGET_DIALOG, 0);
#endif
    dialog.Func(DIALOG_CUR_SET, 0, 0);
    return 0;
}

static int dialog_button_menu(void)
{
    AppWidget_Off(WIDGET_DIALOG, 0);
    dialog.Select = DIALOG_SEL_NO;
    dialog.Func(DIALOG_CUR_SET, 0, 0);
    return 0;
}

static int AppDialog_On(UINT32 param)
{
    int ReturnValue = 0;

    dialog.Func = dialog_func;
    dialog.Gui = gui_dialog_func;
    dialog.Op = &dialog_op;

    /** Show dialog frame */
    dialog.Gui(GUI_DIALOG_SHOW, 0, 0);
    dialog.Gui(GUI_DIALOG_BTN_HIGHLIGHT, GUI_DIALOG_BTN_CANCEL, 0);
    dialog.Gui(GUI_DIALOG_BTN_SHOW, GUI_DIALOG_BTN_CANCEL, 0);
    if (dialog.Type != DIALOG_TYPE_OK) {
        dialog.Gui(GUI_DIALOG_BTN_HIGHLIGHT, GUI_DIALOG_BTN_SET, 0);
        dialog.Gui(GUI_DIALOG_BTN_SHOW, GUI_DIALOG_BTN_SET, 0);
    }

    if (dialog.Type == DIALOG_TYPE_OK) {
        dialog.Select = DIALOG_SEL_OK;
    } else {
        dialog.Select = DIALOG_SEL_NO;
    }

    /** Highlight current btn */
    dialog.Gui(GUI_DIALOG_BTN_HIGHLIGHT, dialog.Select, 1);

    dialog.Gui(GUI_DESC_UPDATE, dialog.Strings, 0);
    dialog.Gui(GUI_DESC_SHOW, 0, 0);

    /** Flush GUI */
    dialog.Gui(GUI_FLUSH, 0, 0);

    return ReturnValue;
}

static int AppDialog_Off(UINT32 param)
{
    int i = 0;

    dialog.Gui(GUI_DIALOG_HIDE, 0, 0);
    for (i=0; i<GUI_DIALOG_BTN_NUM; i++) {
        dialog.Gui(GUI_DIALOG_BTN_HIDE, i, 0);
    }
    dialog.Gui(GUI_DESC_HIDE, 0, 0);
    dialog.Gui(GUI_FLUSH, 0, 0);
    return 0;
}

static int AppDialog_OnMessage(UINT32 msg, UINT32 param1, UINT32 param2)
{
    int ReturnValue = WIDGET_PASSED_MSG;

    switch (msg) {
    case HMSG_USER_UP_BUTTON:
    case HMSG_USER_IR_UP_BUTTON:
        ReturnValue = dialog.Op->ButtonUp();
        break;
    case HMSG_USER_DOWN_BUTTON:
    case HMSG_USER_IR_DOWN_BUTTON:
        ReturnValue = dialog.Op->ButtonDown();
        break;
    case HMSG_USER_LEFT_BUTTON:
    case HMSG_USER_IR_LEFT_BUTTON:
        ReturnValue = dialog.Op->ButtonLeft();
        break;
    case HMSG_USER_RIGHT_BUTTON:
    case HMSG_USER_IR_RIGHT_BUTTON:
        ReturnValue = dialog.Op->ButtonRight();
        break;
    case HMSG_USER_SET_BUTTON:
    case HMSG_USER_IR_SET_BUTTON:
        ReturnValue = dialog.Op->ButtonSet();
        break;
    case HMSG_USER_MENU_BUTTON:
    case HMSG_USER_IR_MENU_BUTTON:
        ReturnValue = dialog.Op->ButtonMenu();
        break;
    default:
        ReturnValue = WIDGET_PASSED_MSG;
    break;
    }

    return ReturnValue;
}

/*************************************************************************
 * Dialog APIs for widget management
 ************************************************************************/
WIDGET_ITEM_s* AppDialog_GetWidget(void)
{
    return &widget_dialog;
}

/*************************************************************************
 * Public Dialog Widget APIs
 ************************************************************************/
int AppDialog_SetDialog(UINT32 Type, UINT32 subject, dialog_set_handler Set)
{
    dialog.Type = Type;
    dialog.Strings = DialogSubjecStrTable[subject];
    dialog.Set = Set;
    return 0;
}

int AppDialog_SetDialogTable(UINT32 *table)
{
    DialogSubjecStrTable = table;
    return 0;
}

