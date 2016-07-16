/**
  * @file src/app/apps/gui/widget/menu/gui_menu.c
  *
  *  Implementation for Menu GUI flow
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

#include <apps/flow/widget/menu/menu.h>
#include <apps/gui/widget/menu/gui_menu.h>
#include <apps/gui/resource/gui_resource.h>
#include <apps/gui/resource/gui_settle.h>

static int tab_name_string[6] = {STR_MENU_ADAS_CALI,STR_MENU_ADAS_FUNC,STR_MENU_ADAS_ALARM,STR_MENU_RECORDER_SET,
   STR_VERSION,STR_MENU_SYS_DEFAULT
};


int gui_menu_func(UINT32 gui_cmd, UINT32 param1, UINT32 param2)
{
    int rval = 0;

    switch (gui_cmd) {
    case GUI_MENU_TAB_SHOW:
        AppLibGraph_Show(GRAPH_CH_DUAL, GOBJ_MENU_TAB_1+param1);
        break;

    case GUI_MENU_TAB_HIDE:
        AppLibGraph_Hide(GRAPH_CH_DUAL, GOBJ_MENU_TAB_1+param1);
        break;

    case GUI_MENU_TAB_UPDATE_BITMAP:
        AppLibGraph_UpdateBMP(GRAPH_CH_DUAL, GOBJ_MENU_TAB_1+param1, param2);
        break;

    case GUI_MENU_ITEM_SHOW:
        if(param2==0)
        {
            AppLibGraph_Show(GRAPH_CH_DUAL, GOBJ_MENU_RECT_1+param1);
            AppLibGraph_Show(GRAPH_CH_DUAL, GOBJ_MENU_STRING_1+param1);
        }else if(param2==1)
        {
            AppLibGraph_Show(GRAPH_CH_DUAL, GOBJ_MENU_RECT1_1+param1);
            AppLibGraph_Show(GRAPH_CH_DUAL, GOBJ_MENU_STRING1_1+param1);
        }
        break;

    case GUI_MENU_ITEM_HIDE:
        
            AppLibGraph_Hide(GRAPH_CH_DUAL, GOBJ_MENU_RECT_1+param1);
            AppLibGraph_Hide(GRAPH_CH_DUAL, GOBJ_MENU_STRING_1+param1);
        
            AppLibGraph_Hide(GRAPH_CH_DUAL, GOBJ_MENU_RECT1_1+param1);
            AppLibGraph_Hide(GRAPH_CH_DUAL, GOBJ_MENU_STRING1_1+param1);
       
       
        break;

    case GUI_MENU_ITEM_CONF_SHOW:
        AppLibGraph_Show(GRAPH_CH_DUAL, GOBJ_MENU_CONF_RECT_1+param1);		
        break;

    case GUI_MENU_ITEM_CONF_HIDE:
        AppLibGraph_Hide(GRAPH_CH_DUAL, GOBJ_MENU_CONF_RECT_1+param1);		
        break;	
    case GUI_MENU_ITEM_ENTER_SHOW:
        AppLibGraph_Show(GRAPH_CH_DUAL, GOBJ_MENU_ICN_ENTER_1+param1);		
        break;

    case GUI_MENU_ITEM_ENTER_HIDE:
        AppLibGraph_Hide(GRAPH_CH_DUAL, GOBJ_MENU_ICN_ENTER_1+param1);		
        break;	

    case GUI_MENU_ITEM_UPDATE_STRING:
        AppLibGraph_UpdateString(GRAPH_CH_DUAL, GOBJ_MENU_STRING_1+param1, param2);
        AppLibGraph_UpdateString(GRAPH_CH_DUAL, GOBJ_MENU_STRING1_1+param1, param2);

        break;
    case GUI_MENU_ITEM_UPDATE_ICN_ENTER:
        AppLibGraph_UpdateBMP(GRAPH_CH_DUAL, GOBJ_MENU_ICN_ENTER_1+param1, BMP_MENU_ICN_ENTER);
        break;
    case GUI_MENU_ITEM_UPDATE_BITMAP:
        //AppLibGraph_UpdateBMP(GRAPH_CH_DUAL, gui_menu_items[param1].gobj_bmp, param2);
        break;

    case GUI_MENU_ITEM_HIGHLIGHT:
        if (param2) {
            AppLibGraph_UpdateBMP(GRAPH_CH_DUAL, GOBJ_MENU_RECT_1+param1, BMP_MENU_STRIPE_HL);
            
            AppLibGraph_UpdateBMP(GRAPH_CH_DUAL, GOBJ_MENU_RECT1_1+param1, BMP_MENU_STRIPE_PB_HL);
        } else {
            AppLibGraph_UpdateBMP(GRAPH_CH_DUAL, GOBJ_MENU_RECT_1+param1, BMP_MENU_STRIPE);
            
            AppLibGraph_UpdateBMP(GRAPH_CH_DUAL, GOBJ_MENU_RECT1_1+param1, BMP_MENU_STRIPE_PB);
        }
        break;
		
    case GUI_MENU_ITEM_CONF_HIGHLIGHT:
        if(param2 == 0) 
	 {
            AppLibGraph_UpdateBMP(GRAPH_CH_DUAL, GOBJ_MENU_CONF_RECT_1+param1, BMP_ICN_QMENU_YES);
        }		
        else if(param2 == 1) {
            AppLibGraph_UpdateBMP(GRAPH_CH_DUAL, GOBJ_MENU_CONF_RECT_1+param1, BMP_ICN_QMENU_YES_HL);
        }
        else if(param2 == 2) 
	 {
            AppLibGraph_UpdateBMP(GRAPH_CH_DUAL, GOBJ_MENU_CONF_RECT_1+param1, BMP_ICN_ITEM_OFF);
        }		
        else if(param2 == 3) {
            AppLibGraph_UpdateBMP(GRAPH_CH_DUAL, GOBJ_MENU_CONF_RECT_1+param1, BMP_ICN_ITEM_ON);
        }
	 else{
            AppLibGraph_UpdateBMP(GRAPH_CH_DUAL, GOBJ_MENU_CONF_RECT_1+param1, BMP_0_NULL);
	 }	
        break;		

    case GUI_MENU_ITEM_LOCK:
        if (param2) {
            AppLibGraph_UpdateColor(GRAPH_CH_DUAL, GOBJ_MENU_STRING_1+param1, COLOR_DARKGRAY, COLOR_TEXT_BORDER );
            AppLibGraph_UpdateColor(GRAPH_CH_DUAL, GOBJ_MENU_STRING1_1+param1, COLOR_DARKGRAY, COLOR_TEXT_BORDER );
        } else {
            AppLibGraph_UpdateColor(GRAPH_CH_DUAL, GOBJ_MENU_STRING_1+param1, COLOR_LIGHTGRAY, COLOR_TEXT_BORDER );
            AppLibGraph_UpdateColor(GRAPH_CH_DUAL, GOBJ_MENU_STRING1_1+param1, COLOR_LIGHTGRAY, COLOR_TEXT_BORDER );
        }
        break;

    case GUI_HIDE_ALL:
        AppLibGraph_HideAll(GRAPH_CH_DUAL);
        break;

    case GUI_FLUSH:
        AppLibGraph_Draw(GRAPH_CH_DUAL);
        break;
    case GUI_MENU_BG_UPDATE_BITMAP:
	 if(param1 == 0)	
            AppLibGraph_UpdateBMP(GRAPH_CH_DUAL, GOBJ_SETTING_MENU_BG, BMP_Z_ADAS_SAFE_BG);
	 if(param1 == 1)
            AppLibGraph_UpdateBMP(GRAPH_CH_DUAL, GOBJ_SETTING_MENU_BG, BMP_MENU_ITEM_BG);
     if(param1 ==2)
         AppLibGraph_UpdateBMP(GRAPH_CH_DUAL, GOBJ_SETTING_MENU_BG1, BMP_PB_ITEM_BG);
	 break;	
    case GUI_MENU_BG_UPDATE_STRING:
        AppLibGraph_UpdateString(GRAPH_CH_DUAL, GOBJ_MENU_BG_STRING_1+param1, tab_name_string[param2]);
        break;
    case GUI_MENU_BG_SHOW:    
        if(param1 ==2)
        {
            AppLibGraph_Hide(GRAPH_CH_DUAL, GOBJ_SETTING_MENU_BG);
            AppLibGraph_Show(GRAPH_CH_DUAL, GOBJ_SETTING_MENU_BG1);
        }else{
            AppLibGraph_Show(GRAPH_CH_DUAL, GOBJ_SETTING_MENU_BG);
        }
        break;    
    case GUI_MENU_BG_HIDE:
        if(param1==2)
        {
        AppLibGraph_Hide(GRAPH_CH_DUAL, GOBJ_SETTING_MENU_BG1);
        }else{
        AppLibGraph_Hide(GRAPH_CH_DUAL, GOBJ_SETTING_MENU_BG);
        }
        break;       
    case GUI_MENU_BG_STRING_SHOW:
        AppLibGraph_Show(GRAPH_CH_DUAL, GOBJ_MENU_BG_STRING_1 + param1);
	 break;      
    case GUI_MENU_BG_STRING_HIDE:
        AppLibGraph_Hide(GRAPH_CH_DUAL, GOBJ_MENU_BG_STRING_1 + param1);
	 break;      	 
    default:
        AmbaPrint("Undefined GUI command");
        rval = -1;
        break;
    }

    return rval;
}
