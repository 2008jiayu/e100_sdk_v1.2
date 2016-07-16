/**
 * system/src/app/apps/gui/widget/menu/gui_menu_driver_id.c
 *
 * Implementation for Driver ID Menu GUI flow
 *
 * History:
 *    2010/08/11 - [Jili Kuang] created file
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

#include <apps/gui/widget/menu/gui_menu_driver_id.h>
#include <wchar.h>
#include <apps/gui/resource/gui_settle.h>
#include <system/app_pref.h>

typedef struct key_frame_s {
    UINT32  key_value;
    UINT32  start_x;
    UINT32  start_y;
} key_frame_t;

#define KEY_FRAME_W 88
#define KEY_FRAME_H 60

static key_frame_t keys_frame_hl[DRIVER_ID_RESOURCE_MAX] = {
    {'1',  35, 243},//'1'
    {'2', 115, 243},//'2'
    {'3', 195, 243},//'3'
    {'4', 275, 243},//'4'
    {'5', 355, 243},//'5'
    {'6', 435, 243},//'6'
    {'7', 515, 243},//'7'
    {'8', 595, 243},//'8'
    {'9', 675, 243},//'9'
    {'0', 755, 243},//'0'
    {'-', 835, 243},//'-'
    {'Q',  74, 297},//'Q'
    {'W', 154, 297},//'W'
    {'E', 234, 297},//'E'
    {'R', 314, 297},//'R'
    {'T', 394, 297},//'T'
    {'Y', 474, 297},//'Y'
    {'U', 554, 297},//'U'
    {'I', 634, 297},//'I'
    {'O', 714, 297},//'O'
    {'P', 794, 297},//'P'
    {'A', 115, 350},//'A'
    {'S', 195, 350},//'S'
    {'D', 275, 350},//'D'
    {'F', 355, 350},//'F'
    {'G', 435, 350},//'G'
    {'H', 515, 350},//'H'
    {'J', 595, 350},//'J'
    {'K', 675, 350},//'K'
    {'L', 755, 350},//'L'
    {'Z', 154, 404},//'Z'
    {'X', 234, 404},//'X'
    {'C', 314, 404},//'C'
    {'V', 394, 404},//'V'
    {'B', 474, 404},//'B'
    {'N', 554, 404},//'N'
    {'M', 634, 404},//'M'
};

static UINT32 val_hl_x[GUI_DRIVER_ID_NUM_MAX] = {
    427, 477, 527, 577, 627, 677, 727, 777,
    827, 877

};

static UINT32 drid_val_hl_y[GUI_DRIVER_ID_NUM_MAX] = {
    114, 114, 114, 114, 114, 114, 114, 114,
    114, 114
};

static UINT16 val_hl_w[GUI_DRIVER_ID_NUM_MAX] = {
    55, 55, 55, 55, 55, 55, 55, 55, 55, 55
};
#define VAL_HL_W    (55)
#define VAL_HL_H    (59)

static int set_val_hl(UINT32 gui_val_id)
{
    UINT32 x[GUI_LAYOUT_NUM] = {0};
    UINT32 y[GUI_LAYOUT_NUM] = {0};
    UINT32 w[GUI_LAYOUT_NUM] = {0};
    UINT32 h[GUI_LAYOUT_NUM] = {0};

    w[0] = 0;
    h[0] = 0;
    w[1] = VAL_HL_W;
    h[1] = VAL_HL_H;
    AppLibGraph_UpdateSize(GRAPH_CH_FCHAN, GOBJ_DRMENU_VAL_HL, w[1], h[1], 0);

    x[0] = 0;
    y[0] = 0;
    x[1] = val_hl_x[gui_val_id];
    y[1] = drid_val_hl_y[gui_val_id]+20;
    AppLibGraph_UpdatePosition(GRAPH_CH_FCHAN, GOBJ_DRMENU_VAL_HL, x[1],y[1]);

    w[0] = VAL_HL_W;
    h[0] = VAL_HL_H;
    w[1] = 0;
    h[1] = 0;
    AppLibGraph_UpdateSize(GRAPH_CH_DCHAN, GOBJ_DRMENU_VAL_HL, w[0], h[0], 0);
    x[0] = val_hl_x[gui_val_id];
    y[0] = drid_val_hl_y[gui_val_id];
    x[1] = 0;
    y[1] = 0;
    AppLibGraph_UpdatePosition(GRAPH_CH_DCHAN, GOBJ_DRMENU_VAL_HL, x[0],y[0]);

    return 0;
}

static int update_key_frame(UINT32 value)
{
    int i=0;
    UINT32 x[GUI_LAYOUT_NUM] = {0};
    UINT32 y[GUI_LAYOUT_NUM] = {0};
    UINT32 w[GUI_LAYOUT_NUM] = {0};
    UINT32 h[GUI_LAYOUT_NUM] = {0};
    UINT32 kf_x = 0;
    UINT32 kf_y = 0;

    for(i=0;i<DRIVER_ID_RESOURCE_MAX;i++){
        if(value==keys_frame_hl[i].key_value){
        kf_x = keys_frame_hl[i].start_x;
            kf_y = keys_frame_hl[i].start_y;
            break;
        }
    }

    w[0] = 0;
    h[0] = 0;
    w[1] = KEY_FRAME_W;
    h[1] = KEY_FRAME_H;
    AppLibGraph_UpdateSize(GRAPH_CH_FCHAN, GOBJ_DRMENU_KEY_FRAME, w[1], h[1], 0);
    x[0] = 0;
    y[0] = 0;
    x[1] = kf_x;
    y[1] = kf_y+20;
    AppLibGraph_UpdatePosition(GRAPH_CH_FCHAN, GOBJ_DRMENU_KEY_FRAME, x[1],y[1]);

    w[0] = KEY_FRAME_W;
    h[0] = KEY_FRAME_H;
    w[1] = 0;
    h[1] = 0;
    AppLibGraph_UpdateSize(GRAPH_CH_DCHAN, GOBJ_DRMENU_KEY_FRAME, w[0], h[0], 0);
    x[0] = kf_x;
    y[0] = kf_y;
    x[1] = 0;
    y[1] = 0;
    AppLibGraph_UpdatePosition(GRAPH_CH_DCHAN, GOBJ_DRMENU_KEY_FRAME, x[0],y[0]);

    return 0;
}

static int set_driver_id_value(UINT32 gui_val_id, UINT32 value)
{
    UINT16 str_val[2] = {0};
    str_val[0] = (UINT16)value;
    AppLibGraph_UpdateStringContext(AppPref_GetLanguageID(), STR_DRIVER_ID_NUM_1+gui_val_id, str_val);

    AppLibGraph_UpdateString(GRAPH_CH_DUAL, GOBJ_DRMENU_NUM_1+gui_val_id,  STR_DRIVER_ID_NUM_1+gui_val_id);

    return 0;
}


int gui_menu_driver_id_func(UINT32 guiCmd, UINT32 param1, UINT32 param2)
{
    int ReturnValue = 0;

    switch (guiCmd) {
    case GUI_FLUSH:
        AppLibGraph_Draw(GRAPH_CH_DUAL);
        break;
    case GUI_HIDE_ALL:
        AppLibGraph_HideAll(GRAPH_CH_DUAL);
        break;
    case GUI_SET_LAYOUT:
        AppLibGraph_SetGUILayout(GRAPH_CH_DCHAN, Gui_Resource_Dchan_Id, Gui_Table_Dchan, UserSetting->SetupPref.LanguageID);
        AppLibGraph_SetGUILayout(GRAPH_CH_FCHAN, Gui_Resource_Fchan_Id, Gui_Table_Fchan, UserSetting->SetupPref.LanguageID);
        break;
    case GUI_DRMENU_SHOW:
        AppLibGraph_Show(GRAPH_CH_DCHAN, GOBJ_DRMENU_TITLE_BASE);
        if(param1 == 0){
           //AppLibGraph_Hide(GRAPH_CH_DCHAN, GOBJ_DRMENU_VAL_HL);
           AppLibGraph_UpdateColor(GRAPH_CH_DCHAN, GOBJ_DRMENU_TITLE, COLOR_GREEN, COLOR_TEXT_BORDER );
        }else if(param1 == 1){
           //AppLibGraph_Show(GRAPH_CH_DCHAN, GOBJ_DRMENU_VAL_HL);
           AppLibGraph_UpdateColor(GRAPH_CH_DCHAN, GOBJ_DRMENU_TITLE, COLOR_LIGHTGRAY, COLOR_TEXT_BORDER );

        }
        AppLibGraph_Show(GRAPH_CH_DCHAN, GOBJ_DRMENU_TITLE);
        set_val_hl(0);

        AppLibGraph_Hide(GRAPH_CH_FCHAN, GOBJ_DRMENU_TITLE_BASE);
        AppLibGraph_Hide(GRAPH_CH_FCHAN, GOBJ_DRMENU_TITLE);
        AppLibGraph_Show(GRAPH_CH_DCHAN, GOBJ_DRMENU_VAL_HL);

        break;
    case GUI_DRMENU_HIDE:
        AppLibGraph_Hide(GRAPH_CH_DUAL, GOBJ_DRMENU_TITLE_BASE);
        AppLibGraph_Hide(GRAPH_CH_DUAL, GOBJ_DRMENU_TITLE);
        AppLibGraph_Hide(GRAPH_CH_DUAL, GOBJ_DRMENU_VAL_HL);

        break;
    case GUI_KEY_FRAME_SHOW:
        AppLibGraph_Show(GRAPH_CH_DCHAN, GOBJ_DRMENU_KEY_FRAME);
        AppLibGraph_Hide(GRAPH_CH_FCHAN, GOBJ_DRMENU_KEY_FRAME);
        break;
    case GUI_KEY_FRAME_HIDE:
        AppLibGraph_Hide(GRAPH_CH_DUAL, GOBJ_DRMENU_KEY_FRAME);
        break;
    case GUI_KEY_FRAME_UPDATE_POS:
        update_key_frame(param1);
        break;
    case GUI_KEY_FRAME_UPDATE_ICON:
        AppLibGraph_UpdateBMP(GRAPH_CH_DUAL, GOBJ_DRMENU_KEY_FRAME, param1);
        break;
    case GUI_VALUE_SHOW:
        AppLibGraph_Show(GRAPH_CH_DCHAN, GOBJ_DRMENU_NUM_1+param1);
        AppLibGraph_Hide(GRAPH_CH_FCHAN, GOBJ_DRMENU_NUM_1+param1);
        break;
    case GUI_VALUE_HIDE:
        AppLibGraph_Hide(GRAPH_CH_DUAL, GOBJ_DRMENU_NUM_1+param1);
        break;
    case GUI_VALUE_HL_UPDATE_POS:
        set_val_hl(param1);
        AppLibGraph_Show(GRAPH_CH_DCHAN, GOBJ_DRMENU_VAL_HL);
        break;
    case GUI_VALUE_HL_UPDATE_ICON:
        AppLibGraph_UpdateBMP(GRAPH_CH_DUAL, GOBJ_DRMENU_VAL_HL, param1);
        break;
    case GUI_VALUE_UPDATE:
        set_driver_id_value(param1, param2);
        break;

    }

    return ReturnValue;
}

