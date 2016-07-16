/**
  * @file src/app/apps/gui/widget/menu/sportcam/gui_menu_adj.c
  *
  *  Implementation for adjusting Menu GUI flow
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
#include <apps/gui/widget/menu/gui_menu_adjust.h>
#include <apps/gui/resource/gui_settle.h>

static UINT32 gui_icons[2] = {
    GOBJ_AMENU_ICON_LEFT,
    GOBJ_AMENU_ICON_RIGHT
};

#ifdef CONFIG_APP_ARD
#define ADJBAR_HL_TOTAL_W_0    (260)
#define ADJBAR_HL_TOTAL_H_0    (7)
#define ADJBAR_IDX_INIT_X_0    (352)
#define ADJBAR_IDX_INIT_Y_0    (362)
#else
#define ADJBAR_HL_TOTAL_W_0    (260)
#define ADJBAR_HL_TOTAL_H_0    (5)
#define ADJBAR_IDX_INIT_X_0    (338)
#define ADJBAR_IDX_INIT_Y_0    (366)
#endif

static int set_adjustbar(GUI_ADJ_STATUS_s *status)
{
    int Total = 0, Cur = 0;
    UINT16 gui_abar_hl_w[GUI_LAYOUT_NUM] = {0};
    UINT16 gui_abar_hl_h[GUI_LAYOUT_NUM] = {0};
    UINT16 gui_abar_idx_x[GUI_LAYOUT_NUM] = {0};
    UINT16 gui_abar_idx_y[GUI_LAYOUT_NUM] = {0};

    Total = (status->Max-status->Min)/status->Step;
    Cur = (status->Cur-status->Min)/status->Step;

    gui_abar_hl_w[0] = 0;
    gui_abar_hl_h[0] = 0;
    gui_abar_hl_w[1] = (Cur*ADJBAR_HL_TOTAL_W_0)/Total;
    gui_abar_hl_h[1] = ADJBAR_HL_TOTAL_H_0;
    AppLibGraph_UpdateSize(GRAPH_CH_FCHAN, GOBJ_AMENU_BAR_HL, gui_abar_hl_w[1], gui_abar_hl_h[1], 0);
    gui_abar_idx_x[0] = 0;
    gui_abar_idx_y[0] = 0;
    gui_abar_idx_x[1] = ADJBAR_IDX_INIT_X_0+gui_abar_hl_w[1];
#ifdef CONFIG_APP_ARD
    gui_abar_idx_y[1] = ADJBAR_IDX_INIT_Y_0;
#else    
    gui_abar_idx_y[1] = ADJBAR_IDX_INIT_Y_0+50;
#endif    
    AppLibGraph_UpdatePosition(GRAPH_CH_FCHAN, GOBJ_AMENU_BAR_IDX, gui_abar_idx_x[1], gui_abar_idx_y[1]);

    gui_abar_hl_w[0] = (Cur*ADJBAR_HL_TOTAL_W_0)/Total;
    gui_abar_hl_h[0] = ADJBAR_HL_TOTAL_H_0;
    gui_abar_hl_w[1] = 0;
    gui_abar_hl_h[1] = 0;
    AppLibGraph_UpdateSize(GRAPH_CH_DCHAN, GOBJ_AMENU_BAR_HL, gui_abar_hl_w[0], gui_abar_hl_h[0], 0);
    gui_abar_idx_x[0] = ADJBAR_IDX_INIT_X_0+gui_abar_hl_w[0];
    gui_abar_idx_y[0] = ADJBAR_IDX_INIT_Y_0;
    gui_abar_idx_x[1] = 0;
    gui_abar_idx_y[1] = 0;
    AppLibGraph_UpdatePosition(GRAPH_CH_DCHAN, GOBJ_AMENU_BAR_IDX, gui_abar_idx_x[0], gui_abar_idx_y[0]);

    return 0;
}

int gui_menu_adj_func(UINT32 guiCmd, UINT32 param1, UINT32 param2)
{
    int rval = 0;

    switch (guiCmd) {
    case GUI_AMENU_SHOW:
        AppLibGraph_Show(GRAPH_CH_DUAL, GOBJ_AMENU_ICON_LEFT);
        AppLibGraph_Show(GRAPH_CH_DUAL, GOBJ_AMENU_BAR_BASE);
        AppLibGraph_UpdateString(GRAPH_CH_DUAL, GOBJ_AMENU_TITLE, param1);
        AppLibGraph_Show(GRAPH_CH_DUAL, GOBJ_AMENU_TITLE);
        break;
    case GUI_AMENU_HIDE:
        AppLibGraph_Hide(GRAPH_CH_DUAL, GOBJ_AMENU_ICON_LEFT);
        AppLibGraph_Hide(GRAPH_CH_DUAL, GOBJ_AMENU_BAR_BASE);
        AppLibGraph_Hide(GRAPH_CH_DUAL, GOBJ_AMENU_TITLE);
        break;
    case GUI_ICON_SHOW:
      //  AppLibGraph_Show(GRAPH_CH_DUAL, gui_icons[param1]);
        break;
    case GUI_ICON_HIDE:
        //AppLibGraph_Hide(GRAPH_CH_DUAL, gui_icons[param1]);
        break;
    case GUI_ICON_UPDATE:
       // AppLibGraph_UpdateBMP(GRAPH_CH_DUAL, gui_icons[param1], param2);
        break;
    case GUI_STATUS_SHOW:
#ifndef CONFIG_APP_ARD    
        AppLibGraph_Show(GRAPH_CH_DUAL, GOBJ_AMENU_BAR_HL);
#endif        
        AppLibGraph_Show(GRAPH_CH_DUAL, GOBJ_AMENU_BAR_IDX);
        break;
    case GUI_STATUS_HIDE:
#ifndef CONFIG_APP_ARD    
        AppLibGraph_Hide(GRAPH_CH_DUAL, GOBJ_AMENU_BAR_HL);
#endif        
        AppLibGraph_Hide(GRAPH_CH_DUAL, GOBJ_AMENU_BAR_IDX);
        break;
    case GUI_STATUS_UPDATE:
        set_adjustbar((GUI_ADJ_STATUS_s *)param1);
        break;
    case GUI_VALUE_SHOW:
        AppLibGraph_Show(GRAPH_CH_DUAL, GOBJ_AMENU_VALUE);
        break;
    case GUI_VALUE_HIDE:
        AppLibGraph_Hide(GRAPH_CH_DUAL, GOBJ_AMENU_VALUE);
        break;
    case GUI_VALUE_UPDATE:
        AppLibGraph_UpdateString(GRAPH_CH_DUAL, GOBJ_AMENU_VALUE, param1);
        break;
    case GUI_HIDE_ALL:
        AppLibGraph_HideAll(GRAPH_CH_DUAL);
        break;
    case GUI_FLUSH:
        AppLibGraph_Draw(GRAPH_CH_DUAL);
        break;
    default:
        AmbaPrint("[Gui menu adjust] Undefined GUI command");
        rval = -1;
        break;
    }

    return rval;
}
