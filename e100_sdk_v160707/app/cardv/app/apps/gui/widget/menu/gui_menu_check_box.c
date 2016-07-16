/**
 * system/src/app/apps/gui/widget/menu/gui_menu_check_box.c
 *
 * Implementation for Check Box Menu GUI flow
 *
 * History:
 *    2014/01/10 - [David Li] created file
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

#include <wchar.h>
#include <apps/gui/resource/gui_settle.h>
#include <system/app_pref.h>
#include <apps/gui/widget/menu/gui_menu_check_box.h>

#define HIGHLIGHT_X 140
//#define HIGHLIGHT_W 620
//#define HIGHLIGHT_H 55
static int item_highlight_y[GUI_MENU_CHECK_BOX_ITEM_NUM] = {215, 270, 325};

static int update_item_highlight_coordinate( int obj_index )
{
    UINT32 x[GUI_LAYOUT_NUM] = {0};
    UINT32 y[GUI_LAYOUT_NUM] = {0};
    UINT32 w[GUI_LAYOUT_NUM] = {0};
    UINT32 h[GUI_LAYOUT_NUM] = {0};
    /*
    w[0] = 0;
    h[0] = 0;
    w[1] = HIGHLIGHT_W;
    h[1] = HIGHLIGHT_H;
    AppLibGraph_UpdateSize(GRAPH_CH_FCHAN, GOBJ_CKBXMENU_STRING_HL, w[1], h[1], 0);
    */
    x[0] = 0;
    y[0] = 0;
    x[1] = HIGHLIGHT_X;
    y[1] = item_highlight_y[obj_index];
    AppLibGraph_UpdatePosition(GRAPH_CH_FCHAN, GOBJ_CKBXMENU_STRING_HL, x[1],y[1]);
    /*
    w[0] = HIGHLIGHT_W;
    h[0] = HIGHLIGHT_H;
    w[1] = 0;
    h[1] = 0;
    AppLibGraph_UpdateSize(GRAPH_CH_DCHAN, GOBJ_CKBXMENU_STRING_HL, w[0], h[0], 0);
    */
    x[0] = HIGHLIGHT_X;
    y[0] = item_highlight_y[obj_index];
    x[1] = 0;
    y[1] = 0;
    AppLibGraph_UpdatePosition(GRAPH_CH_DCHAN, GOBJ_CKBXMENU_STRING_HL, x[0],y[0]);
    return 0;
}

static int update_item_box( int obj_index, int check_or_not )
{
    if( check_or_not == 0 )
    {
        AppLibGraph_UpdateBMP(GRAPH_CH_DUAL, GOBJ_CKBXMENU_BOX_1+obj_index, BMP_ICN_SELECT_NO);
    }
    else
    {
        AppLibGraph_UpdateBMP(GRAPH_CH_DUAL, GOBJ_CKBXMENU_BOX_1+obj_index, BMP_ICN_SELECT_YES);
    }

    return 0;
}

int gui_menu_check_box_func(UINT32 gui_cmd, UINT32 param1, UINT32 param2)
{
    int rval = 0;

    switch (gui_cmd) {
    case GUI_CKBXMENU_TITLE_SHOW:
        update_item_highlight_coordinate(GUI_MENU_CHECK_BOX_ITEM_1);
        //if (!bootparam_exists("osd", "tv_full")){
        //  AppLibGraph_Show(GRAPH_CH_DCHAN, GOBJ_CKBXMENU_BASE);
        //  AppLibGraph_Show(GRAPH_CH_DCHAN, GOBJ_CKBXMENU_TITLE);
        //  AppLibGraph_Show(GRAPH_CH_DCHAN, GOBJ_CKBXMENU_STRING_HL);

        //  AppLibGraph_Hide(GRAPH_CH_FCHAN, GOBJ_CKBXMENU_BASE);
        //  AppLibGraph_Hide(GRAPH_CH_FCHAN, GOBJ_CKBXMENU_TITLE);
        //  AppLibGraph_Hide(GRAPH_CH_FCHAN, GOBJ_CKBXMENU_STRING_HL);
        //}else{
            AppLibGraph_Show(GRAPH_CH_DUAL, GOBJ_CKBXMENU_BASE);
            AppLibGraph_Show(GRAPH_CH_DUAL, GOBJ_CKBXMENU_TITLE);
            AppLibGraph_Show(GRAPH_CH_DUAL, GOBJ_CKBXMENU_STRING_HL);
        //}
        break;
    case GUI_CKBXMENU_TITLE_HIDE:
        AppLibGraph_Hide(GRAPH_CH_DUAL, GOBJ_CKBXMENU_BASE);
        AppLibGraph_Hide(GRAPH_CH_DUAL, GOBJ_CKBXMENU_TITLE);
        AppLibGraph_Hide(GRAPH_CH_DUAL, GOBJ_CKBXMENU_STRING_HL);
        break;
    case GUI_CKBXMENU_ITEM_SHOW:
        //if (!bootparam_exists("osd", "tv_full")){
        //  AppLibGraph_Show(GRAPH_CH_DCHAN, GOBJ_CKBXMENU_STRING_1+param1);
        //  AppLibGraph_Show(GRAPH_CH_DCHAN, GOBJ_CKBXMENU_BOX_1+param1);
        //  AppLibGraph_Show(GRAPH_CH_DCHAN, GOBJ_CKBXMENU_STRING_HL);

        //  AppLibGraph_Hide(GRAPH_CH_FCHAN, GOBJ_CKBXMENU_STRING_1+param1);
        //  AppLibGraph_Hide(GRAPH_CH_FCHAN, GOBJ_CKBXMENU_BOX_1+param1);
        //  AppLibGraph_Hide(GRAPH_CH_FCHAN, GOBJ_CKBXMENU_STRING_HL);
        //}else{
            AppLibGraph_Show(GRAPH_CH_DUAL, GOBJ_CKBXMENU_STRING_1+param1);
            AppLibGraph_Show(GRAPH_CH_DUAL, GOBJ_CKBXMENU_BOX_1+param1);
            AppLibGraph_Show(GRAPH_CH_DUAL, GOBJ_CKBXMENU_STRING_HL);
        //}
        break;
    case GUI_CKBXMENU_ITEM_HIDE:
        AppLibGraph_Hide(GRAPH_CH_DUAL, GOBJ_CKBXMENU_STRING_1+param1);
        AppLibGraph_Hide(GRAPH_CH_DUAL, GOBJ_CKBXMENU_BOX_1+param1);
        AppLibGraph_Hide(GRAPH_CH_DUAL, GOBJ_CKBXMENU_STRING_HL);
        break;
    case GUI_CKBXMENU_ITEM_UPDATE_STRING:
        AppLibGraph_UpdateString(GRAPH_CH_DUAL, GOBJ_CKBXMENU_STRING_1+param1, param2);
        break;
    case GUI_CKBXMENU_ITEM_UPDATE_BOX:
        update_item_box(param1,param2);
        //if (!bootparam_exists("osd", "tv_full")){
        //  AppLibGraph_Show(GRAPH_CH_DCHAN, GOBJ_CKBXMENU_BOX_1+param1);
        //  AppLibGraph_Hide(GRAPH_CH_FCHAN, GOBJ_CKBXMENU_BOX_1+param1);
        //}else{
            AppLibGraph_Show(GRAPH_CH_DUAL, GOBJ_CKBXMENU_BOX_1+param1);
        //}
        break;
    case GUI_CKBXMENU_ITEM_HIGHLIGHT:
        update_item_highlight_coordinate(param1);
        //if (!bootparam_exists("osd", "tv_full")){
        //  AppLibGraph_Show(GRAPH_CH_DCHAN, GOBJ_CKBXMENU_STRING_HL);
        //  AppLibGraph_Hide(GRAPH_CH_FCHAN, GOBJ_CKBXMENU_STRING_HL);
        //}else{
            AppLibGraph_Show(GRAPH_CH_DUAL, GOBJ_CKBXMENU_STRING_HL);
        //}
        break;
    case GUI_CKBXMENU_ITEM_LOCK:
        break;
    //case GUI_REVEAL:
        //app_gplane_reveal(GRAPH_CH_DUAL, GRAPH_GPLANE_3);
        //break;
    //case GUI_CONCEAL:
        //app_gplane_conceal(GRAPH_CH_DUAL, GRAPH_GPLANE_3);
        //break;
    case GUI_HIDE_ALL:
        AppLibGraph_HideAll(GRAPH_CH_DUAL);
        break;
    case GUI_FLUSH:
        AppLibGraph_Draw(GRAPH_CH_DUAL);
        break;
    case GUI_SET_LAYOUT:
        AppLibGraph_SetGUILayout(GRAPH_CH_DCHAN, Gui_Resource_Dchan_Id, Gui_Table_Dchan, UserSetting->SetupPref.LanguageID);
        AppLibGraph_SetGUILayout(GRAPH_CH_FCHAN, Gui_Resource_Fchan_Id, Gui_Table_Fchan, UserSetting->SetupPref.LanguageID);
        break;
    default:
        AmbaPrint("Undefined GUI command");
        rval = -1;
        break;
    }

    return rval;
}

