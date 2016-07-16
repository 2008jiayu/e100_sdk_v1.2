/**
  * @file app/cardv/app/apps/flow/widget/info_prompt/info_prompt.c
  *
  * Implementation of info prompt
  *
  * History:
  *    2015/2/5 - [Emmett Xie] created file
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
#include <apps/flow/widget/menu/menu.h>
#include <apps/gui/widget/info_prompt/gui_info_prompt.h>
#include <apps/gui/resource/gui_table.h>
#include <system/app_pref.h>
#include <graphics/canvas/ApplibGraphics_Canvas.h>

/*************************************************************************
 * Info prompt definitions
 ************************************************************************/
typedef enum _INFO_PROMPT_FUNC_ID_e_ {
    INFO_PROMPT_CUR_SET = 0
} INFO_PROMPT_FUNC_ID_e;

static int info_prompt_func(UINT32 funcId, UINT32 param1, UINT32 param2);

static int info_prompt_button_up(void);
static int info_prompt_button_down(void);
static int info_prompt_button_left(void);
static int info_prompt_button_right(void);
static int info_prompt_button_set(void);
static int info_prompt_button_menu(void);

typedef struct _Info_Prompt_OP_s_ {
    int (*ButtonUp)(void);
    int (*ButtonDown)(void);
    int (*ButtonLeft)(void);
    int (*ButtonRight)(void);
    int (*ButtonSet)(void);
    int (*ButtonMenu)(void);
} Info_Prompt_OP_s;

static Info_Prompt_OP_s info_prompt_op = {
    info_prompt_button_up,
    info_prompt_button_down,
    info_prompt_button_left,
    info_prompt_button_right,
    info_prompt_button_set,
    info_prompt_button_menu
};

typedef struct _Info_Prompt_s_ {
    int (*Func)(UINT32 funcId, UINT32 param1, UINT32 param2);
    int (*Gui)(UINT32 guiCmd, UINT32 param1, UINT32 param2);
    Info_Prompt_OP_s *Op;
} Info_Prompt_s;

static Info_Prompt_s info_prompt = {0};

static int AppInfo_Prompt_On(UINT32 param);
static int AppInfo_Prompt_Off(UINT32 param);
static int AppInfo_Prompt_OnMessage(UINT32 msg, UINT32 param1, UINT32 param2);

WIDGET_ITEM_s widget_info_prompt = {
    AppInfo_Prompt_On,
    AppInfo_Prompt_Off,
    AppInfo_Prompt_OnMessage
};

/*************************************************************************
 * info_prompt APIs
 ************************************************************************/
static int info_prompt_func(UINT32 funcId, UINT32 param1, UINT32 param2)
{
    return 0;
}

static int info_prompt_button_up(void)
{
    return 0;
}

static int info_prompt_button_down(void)
{
    return 0;
}

static int info_prompt_button_left(void)
{
    return 0;
}

static int info_prompt_button_right(void)
{
    return 0;
}

static int info_prompt_button_set(void)
{
    AmbaPrint("%s, %s, %d", __FILE__, __func__, __LINE__);

    AppWidget_Off(WIDGET_INFO_PROMPT, 0);

    return 0;
}

static int info_prompt_button_menu(void)
{

    return 0;
}

static int AppInfo_Prompt_On(UINT32 param)
{
    int ReturnValue = 0;

    info_prompt.Func = info_prompt_func;
    info_prompt.Gui = gui_info_prompt_func;
    info_prompt.Op = &info_prompt_op;
/*
    PtrCanvas = AppLibGraph_GetCanvas(GRAPH_CH_DCHAN);
    GuiInfoPromptX = PtrCanvas->Area.X + PtrCanvas->Area.Width * 1/8;
    GuiInfoPromptY = PtrCanvas->Area.Y + PtrCanvas->Area.Height * 7/8;
    GuiInfoPromptW = PtrCanvas->Area.Width * 3/4;
    GuiInfoPromptH = PtrCanvas->Area.Height * 3/4;
    AmbaPrint("%s, %s, %d", __FILE__, __func__, __LINE__);
    AppLibGraph_UpdatePosition(GRAPH_CH_DCHAN, GOBJ_INFO_PROMPT_BG, GuiInfoPromptX, GuiInfoPromptY);
    AppLibGraph_UpdateSize(GRAPH_CH_DCHAN, GOBJ_INFO_PROMPT_BG, GuiInfoPromptW, GuiInfoPromptH, 0);
    AmbaPrint("%s, %s, %d", __FILE__, __func__, __LINE__);

    AppLibGraph_UpdatePosition(GRAPH_CH_DCHAN, GOBJ_INFO_PROMPT_BG_STR, GuiInfoPromptX, GuiInfoPromptY);
    AppLibGraph_UpdateSize(GRAPH_CH_DCHAN, GOBJ_INFO_PROMPT_BG_STR, GuiInfoPromptW, GuiInfoPromptH, 0);
    */
    info_prompt.Gui(GUI_INFO_PROMPT_BG_SHOW, 0, 0);
    info_prompt.Gui(GUI_INFO_PROMPT_BG_STR_SHOW, 0, 0);

    AmbaPrint("%s, %s, %d", __FILE__, __func__, __LINE__);

    /** Flush GUI */
    info_prompt.Gui(GUI_FLUSH, 0, 0);

    return ReturnValue;
}

static int AppInfo_Prompt_Off(UINT32 param)
{
    info_prompt.Gui(GUI_INFO_PROMPT_BG_HIDE, 0, 0);
    info_prompt.Gui(GUI_INFO_PROMPT_BG_STR_HIDE, 0, 0);

    info_prompt.Gui(GUI_FLUSH, 0, 0);
    return 0;
}

static int AppInfo_Prompt_OnMessage(UINT32 msg, UINT32 param1, UINT32 param2)
{
    int ReturnValue = WIDGET_PASSED_MSG;

    switch (msg) {
    case HMSG_USER_SET_BUTTON:
    case HMSG_USER_IR_SET_BUTTON:
    case HMSG_USER_MENU_BUTTON:
    case HMSG_USER_IR_MENU_BUTTON:
        ReturnValue = info_prompt.Op->ButtonSet();
        break;
    default:
        ReturnValue = WIDGET_PASSED_MSG;
        break;
    }

    return ReturnValue;
}

/*************************************************************************
 * info_prompt APIs for widget management
 ************************************************************************/
WIDGET_ITEM_s* AppInfo_Prompt_GetWidget(void)
{
    return &widget_info_prompt;
}
