/**
  * @file src/app/apps/gui/widget/menu/connectedcam/gui_menu_time.c
  *
  *  Implementation for Time Menu GUI flow
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
#include <system/app_util.h>
#include <apps/gui/widget/menu/gui_menu_time.h>
#include <apps/gui/resource/gui_settle.h>


static UINT16 val_hl_x[GUI_VALUE_NUM] = {
    350, 480, 558, 402, 467,532
};
static UINT16 val_hl_y[GUI_VALUE_NUM] = {
    195, 195, 195, 275, 275, 275
};

//#define VAL_HL_Y    (210)

static UINT16 val_hl_w[GUI_VALUE_NUM] = {
    104, 52, 52, 52, 52,52
};
#define VAL_HL_H    (46)

static int set_val_hl(UINT32 gui_val_id)
{
    UINT16 x[GUI_LAYOUT_NUM] = {0};
    UINT16 y[GUI_LAYOUT_NUM] = {0};
    UINT16 w[GUI_LAYOUT_NUM] = {0};
    UINT16 h[GUI_LAYOUT_NUM] = {0};

    w[0] = 0;
    h[0] = 0;
    w[1] = val_hl_w[gui_val_id];
    h[1] = VAL_HL_H;
    AppLibGraph_UpdateSize(GRAPH_CH_FCHAN, GOBJ_TMENU_VAL_HL, w[1], h[1], 0);
    x[0] = 0;
    y[0] = 0;
    x[1] = val_hl_x[gui_val_id];
    y[1] = val_hl_y[gui_val_id]+50;
    AppLibGraph_UpdatePosition(GRAPH_CH_FCHAN, GOBJ_TMENU_VAL_HL, x[1], y[1]);
    w[0] = val_hl_w[gui_val_id];
    h[0] = VAL_HL_H;
    w[1] = 0;
    h[1] = 0;
    AppLibGraph_UpdateSize(GRAPH_CH_DCHAN, GOBJ_TMENU_VAL_HL, w[0], h[0], 0);
    x[0] = val_hl_x[gui_val_id];
    y[0] = val_hl_y[gui_val_id];
    x[1] = 0;
    y[1] = 0;
    AppLibGraph_UpdatePosition(GRAPH_CH_DCHAN, GOBJ_TMENU_VAL_HL, x[0], y[0]);

    return 0;
}

static int set_time_value(UINT32 gui_val_id, UINT32 value)
{
    char str_val[5] = {0};
    UINT16 str_val_uni[5] = {0};

    if (gui_val_id == GUI_YEAR) {
        sprintf(str_val, "%04d", value);
    } else {
        sprintf(str_val, "%02d", value);
    }
    AppUtil_AsciiToUnicode(str_val, str_val_uni);
#ifdef CONFIG_APP_ARD
    AppLibGraph_UpdateStringContext(AppPref_GetLanguageID(), STR_TIME_VALUE_YEAR + gui_val_id, str_val_uni);
#else
    AppLibGraph_UpdateStringContext(0, STR_TIME_VALUE_YEAR + gui_val_id, str_val_uni);
#endif
    AppLibGraph_UpdateString(GRAPH_CH_DUAL, GOBJ_TMENU_YEAR + gui_val_id, STR_TIME_VALUE_YEAR + gui_val_id);

    return 0;
}
int gui_menu_time_func(UINT32 gui_cmd, UINT32 param1, UINT32 param2)
{
    int rval = 0;

    switch (gui_cmd) {
    case GUI_TMENU_UPDATE:
        AppLibGraph_UpdateString(GRAPH_CH_DUAL, GOBJ_TMENU_TITLE, STR_TIME_SETUP);
	break;	
    case GUI_TMENU_SHOW:
        AppLibGraph_Show(GRAPH_CH_DUAL, GOBJ_TMENU_BASE);
        AppLibGraph_Show(GRAPH_CH_DUAL, GOBJ_TMENU_TITLE);
        AppLibGraph_Show(GRAPH_CH_DUAL, GOBJ_TMENU_VAL_HL);
        AppLibGraph_Show(GRAPH_CH_DUAL, GOBJ_TMENU_Y_M_SEP);
        AppLibGraph_Show(GRAPH_CH_DUAL, GOBJ_TMENU_M_D_SEP);
        AppLibGraph_Show(GRAPH_CH_DUAL, GOBJ_TMENU_H_M_SEP);
        AppLibGraph_Show(GRAPH_CH_DUAL, GOBJ_TMENU_M_S_SEP);
        break;
    case GUI_TMENU_HIDE:
        AppLibGraph_Hide(GRAPH_CH_DUAL, GOBJ_TMENU_BASE);
        AppLibGraph_Hide(GRAPH_CH_DUAL, GOBJ_TMENU_TITLE);
        AppLibGraph_Hide(GRAPH_CH_DUAL, GOBJ_TMENU_VAL_HL);
        AppLibGraph_Hide(GRAPH_CH_DUAL, GOBJ_TMENU_Y_M_SEP);
        AppLibGraph_Hide(GRAPH_CH_DUAL, GOBJ_TMENU_M_D_SEP);
        AppLibGraph_Hide(GRAPH_CH_DUAL, GOBJ_TMENU_H_M_SEP);
        AppLibGraph_Hide(GRAPH_CH_DUAL, GOBJ_TMENU_M_S_SEP);
        break;
    case GUI_VALUE_SHOW:
        AppLibGraph_Show(GRAPH_CH_DUAL, GOBJ_TMENU_YEAR+param1);
        break;
    case GUI_VALUE_HIDE:
        AppLibGraph_Hide(GRAPH_CH_DUAL, GOBJ_TMENU_YEAR+param1);
        break;
    case GUI_VALUE_HL:
        set_val_hl(param1);
        break;
    case GUI_VALUE_UPDATE:
        set_time_value(param1, param2);
        break;
    case GUI_HIDE_ALL:
        AppLibGraph_HideAll(GRAPH_CH_DUAL);
        break;
    case GUI_FLUSH:
        AppLibGraph_Draw(GRAPH_CH_DUAL);
        break;
    default:
        AmbaPrint("Undefined GUI command");
        rval = -1;
        break;
    }

    return rval;
}
