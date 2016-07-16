/**
  * @file src/app/apps/gui/widget/menu/connectedcam/gui_menu_quick.c
  *
  *  Implementation for Quick Menu GUI flow
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
#include <apps/gui/widget/menu/gui_menu_quick.h>
#include <apps/gui/resource/gui_settle.h>

#define OPT_HL_Y    (150)

typedef struct _GUI_MENU_OPT_s_ {
    int GuiObj;
    int HighlightX;
} GUI_MENU_OPT_s;

static GUI_MENU_OPT_s gui_menu_opts[GUI_OPT_NUM] = {
    {GOBJ_QMENU_OPT_1, 164},
    {GOBJ_QMENU_OPT_2, 292},
    {GOBJ_QMENU_OPT_3, 420},
    {GOBJ_QMENU_OPT_4, 548},
    {GOBJ_QMENU_OPT_5, 676}
};

typedef struct _GUI_MENU_BTN_s_ {
    int GuiObj;
} GUI_MENU_BTN_s;

static GUI_MENU_BTN_s gui_menu_btns[GUI_BTN_NUM] = {
    {GOBJ_QMENU_CANCEL},
    {GOBJ_QMENU_SET},
    {GOBJ_QMENU_UP},
    {GOBJ_QMENU_DOWN},
};
////////////////////////////
typedef struct _GUI_ICON_s_ {
    int GuiObj;
} GUI_ICON_s;

static GUI_ICON_s gui_menu_icns[GUI_ICN_NUM] = {
    {GOBJ_MOTION_DETECT},
};

/////////////////////////////////////////



/** Menu quick description GUI item */
typedef struct _GUI_MENU_DESC_s_ {
    int GuiObj;
} GUI_MENU_DESC_s;

static GUI_MENU_DESC_s gui_menu_desc = {
    GOBJ_QMENU_DESC
};

static int highlight_btn(UINT32 btnId, UINT32 hl)
{
    switch (btnId) {
    case GUI_BTN_SET:
        if (hl) {
            AppLibGraph_UpdateBMP(GRAPH_CH_DUAL, gui_menu_btns[btnId].GuiObj, BMP_ICN_QMENU_YES_HL);
        } else {
            AppLibGraph_UpdateBMP(GRAPH_CH_DUAL, gui_menu_btns[btnId].GuiObj, BMP_ICN_QMENU_YES);
        }
        break;
    case GUI_BTN_CANCEL:
        if (hl) {
            AppLibGraph_UpdateBMP(GRAPH_CH_DUAL, gui_menu_btns[btnId].GuiObj, BMP_ICN_QMENU_NO_HL);
        } else {
            AppLibGraph_UpdateBMP(GRAPH_CH_DUAL, gui_menu_btns[btnId].GuiObj, BMP_ICN_QMENU_NO);
        }
        break;
    case GUI_BTN_UP:
        if (hl) {
            AppLibGraph_UpdateBMP(GRAPH_CH_DUAL, gui_menu_btns[btnId].GuiObj, BMP_ICN_QMENU_UP_HL);
        } else {
            AppLibGraph_UpdateBMP(GRAPH_CH_DUAL, gui_menu_btns[btnId].GuiObj, BMP_ICN_QMENU_UP);
        }
        break;
    case GUI_BTN_DOWN:
        if (hl) {
            AppLibGraph_UpdateBMP(GRAPH_CH_DUAL, gui_menu_btns[btnId].GuiObj, BMP_ICN_QMENU_DOWN_HL);
        } else {
            AppLibGraph_UpdateBMP(GRAPH_CH_DUAL, gui_menu_btns[btnId].GuiObj, BMP_ICN_QMENU_DOWN);
        }
        break;
    default:
        break;
    }

    return 0;
}

int gui_menu_quick_func(UINT32 guiCmd, UINT32 param1, UINT32 param2)
{
    int rval = 0;

    switch (guiCmd) {
    case GUI_RESET:
        break;
    case GUI_QMENU_SHOW:
        AppLibGraph_Show(GRAPH_CH_DUAL, GOBJ_QMENU_BG);
        break;
    case GUI_QMENU_HIDE:
        AppLibGraph_Hide(GRAPH_CH_DUAL, GOBJ_QMENU_BG);
        break;
    case GUI_BTN_SHOW:
        AppLibGraph_Show(GRAPH_CH_DUAL, gui_menu_btns[param1].GuiObj);
        break;
    case GUI_BTN_HIDE:
        AppLibGraph_Hide(GRAPH_CH_DUAL, gui_menu_btns[param1].GuiObj);
        break;
    case GUI_DETECT_MOTION_ICN_SHOW:
        if(param1==1)
        {
            AppLibGraph_Show(GRAPH_CH_DUAL, gui_menu_icns[GUI_ICN_DETECT_MOTION].GuiObj);
        }
        else if(param1==0)
        {
            AppLibGraph_Hide(GRAPH_CH_DUAL, gui_menu_icns[GUI_ICN_DETECT_MOTION].GuiObj);
        }
        break;
    case GUI_BTN_HIGHLIGHT:
        highlight_btn(param1, param2);
        break;
    case GUI_OPT_SHOW:
        if (param1 < GUI_OPT_NUM) {
            AppLibGraph_Show(GRAPH_CH_DUAL, gui_menu_opts[param1].GuiObj);
        } else {
            AppLibGraph_Show(GRAPH_CH_DUAL, GOBJ_QMENU_OPT_HL);
        }
        break;
    case GUI_OPT_HIDE:
        if (param1 < GUI_OPT_NUM) {
            AppLibGraph_Hide(GRAPH_CH_DUAL, gui_menu_opts[param1].GuiObj);
        } else {
            AppLibGraph_Hide(GRAPH_CH_DUAL, GOBJ_QMENU_OPT_HL);
        }
        break;
    case GUI_OPT_UPDATE:
        AppLibGraph_UpdateBMP(GRAPH_CH_DUAL, gui_menu_opts[param1].GuiObj, param2);
        break;
    case GUI_OPT_HIGHLIGHT:
#if 0		
        {
            UINT16 x[2] = {0};
            UINT16 y[2] = {0};
            x[0] = 0;
            y[0] = 0;
            x[1] = gui_menu_opts[param1].HighlightX;
            y[1] = OPT_HL_Y;
            AppLibGraph_UpdatePosition(GRAPH_CH_FCHAN, GOBJ_QMENU_OPT_HL, x[1], y[1]);

            x[0] = gui_menu_opts[param1].HighlightX;
            y[0] = OPT_HL_Y;
            x[1] = 0;
            y[1] = 0;
            AppLibGraph_UpdatePosition(GRAPH_CH_DCHAN, GOBJ_QMENU_OPT_HL, x[0], y[0]);
        }
#endif
        if (param2) {
            AppLibGraph_UpdateBMP(GRAPH_CH_DUAL, GOBJ_MENU_RECT_1+param1, BMP_MENU_STRIPE_HL);
        } else {
            AppLibGraph_UpdateBMP(GRAPH_CH_DUAL, GOBJ_MENU_RECT_1+param1, BMP_MENU_STRIPE);
        }
        break;
    case GUI_DESC_SHOW:
#if 0		
        AppLibGraph_Show(GRAPH_CH_DUAL, gui_menu_desc.GuiObj);
#endif
        AppLibGraph_Show(GRAPH_CH_DUAL, GOBJ_MENU_RECT_1+param1);
        AppLibGraph_Show(GRAPH_CH_DUAL, GOBJ_MENU_STRING_1+param1);
        break;
    case GUI_DESC_HIDE:
#if 0		
        AppLibGraph_Hide(GRAPH_CH_DUAL, gui_menu_desc.GuiObj);
#endif
        AppLibGraph_Hide(GRAPH_CH_DUAL, GOBJ_MENU_RECT_1+param1);
        AppLibGraph_Hide(GRAPH_CH_DUAL, GOBJ_MENU_STRING_1+param1);
        break;
    case GUI_DESC_UPDATE:
#if 0		
        if (param2) {
            AppLibGraph_UpdateColor(GRAPH_CH_DUAL,  gui_menu_desc.GuiObj, COLOR_DARKGRAY, COLOR_TEXT_BORDER );
        } else {
            AppLibGraph_UpdateColor(GRAPH_CH_DUAL,  gui_menu_desc.GuiObj, COLOR_LIGHTGRAY, COLOR_TEXT_BORDER );
        }
        AppLibGraph_UpdateString(GRAPH_CH_DUAL, gui_menu_desc.GuiObj, param1);
#endif		
        AppLibGraph_UpdateString(GRAPH_CH_DUAL, GOBJ_MENU_STRING_1+param1, param2);        
        break;
    case GUI_PAGE_SHOW:
        break;
    case GUI_PAGE_HIDE:
        break;
    case GUI_PAGE_UPDATE:
        break;
    case GUI_HIDE_ALL:
        AppLibGraph_HideAll(GRAPH_CH_DUAL);
        break;
    case GUI_FLUSH:
        AppLibGraph_Draw(GRAPH_CH_DUAL);
        break;
    case GUI_MENU_QUICK_BG_UPDATE_BITMAP:
        AppLibGraph_UpdateBMP(GRAPH_CH_DUAL, GOBJ_SETTING_MENU_BG, BMP_Z_ADAS_SAFE_BG);
	  break;
    case GUI_MENU_QUICK_BG_SHOW:
        AppLibGraph_Show(GRAPH_CH_DUAL, GOBJ_SETTING_MENU_BG);
	 break;    
    case GUI_MENU_QUICK_BG_HIDE:
        AppLibGraph_Hide(GRAPH_CH_DUAL, GOBJ_SETTING_MENU_BG);
	 break;    	
    case GUI_MENU_QUICK_CONF_HIGHLIGHT:
        if(param2 == 0) 
	 {
            AppLibGraph_UpdateBMP(GRAPH_CH_DUAL, GOBJ_MENU_CONF_RECT_1+param1, BMP_ICN_QMENU_YES);
        }		
        else if(param2 == 1) {
            AppLibGraph_UpdateBMP(GRAPH_CH_DUAL, GOBJ_MENU_CONF_RECT_1+param1, BMP_ICN_QMENU_YES_HL);
        }
	 break;
    case GUI_MENU_QUICK_CONF_SHOW:
        AppLibGraph_Show(GRAPH_CH_DUAL, GOBJ_MENU_CONF_RECT_1+param1);		
        break;

    case GUI_MENU_QUICK_CONF_HIDE:
        AppLibGraph_Hide(GRAPH_CH_DUAL, GOBJ_MENU_CONF_RECT_1+param1);		
        break;			 
    case GUI_QUICK_BG_UPDATE_STRING:
        AppLibGraph_UpdateString(GRAPH_CH_DUAL, GOBJ_MENU_BG_STRING_2, param2);
        break;		
    case GUI_QUICK_BG_STRING_SHOW:
        AppLibGraph_Show(GRAPH_CH_DUAL, GOBJ_MENU_BG_STRING_2);
	 break;      
    case GUI_QUICK_BG_STRING_HIDE:
        AppLibGraph_Hide(GRAPH_CH_DUAL, GOBJ_MENU_BG_STRING_2);
	 break;      			
    default:
        AmbaPrint("Undefined GUI command");
        rval = -1;
        break;
    }

    return rval;
}
