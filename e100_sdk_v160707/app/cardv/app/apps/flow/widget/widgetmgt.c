/**
  * @file src/app/apps/flow/widget/widgetmgt.c
  *
  * Implementation of Widget Management
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

#include <apps/flow/widget/widgetmgt.h>
#include <apps/flow/widget/menu/menu.h>
#include <apps/flow/widget/dialog/dialog.h>

extern WIDGET_ITEM_s* AppMenuQuick_GetWidget(void);
extern WIDGET_ITEM_s* AppMenuAdj_GetWidget(void);
extern WIDGET_ITEM_s* AppMenuTime_GetWidget(void);
#ifdef CONFIG_APP_ARD
extern WIDGET_ITEM_s* AppInfo_Prompt_GetWidget(void);
extern WIDGET_ITEM_s* app_menu_driver_id_get_widget(void);
extern WIDGET_ITEM_s* app_menu_ckbx_get_widget(void);
#endif

/*************************************************************************
 * Widget Definitions
 ************************************************************************/
typedef struct _WIDGET_s_ {
    UINT32 Flags;
    UINT32 Cur;
    WIDGET_ITEM_s *Menu;
    WIDGET_ITEM_s *MenuQuick;
    WIDGET_ITEM_s *MenuAdj;
    WIDGET_ITEM_s *MenuTime;
    WIDGET_ITEM_s *Dialog;
    WIDGET_ITEM_s *MenuAdasCalib;
#ifdef CONFIG_APP_ARD
    WIDGET_ITEM_s *InfoPrompt;
    WIDGET_ITEM_s *menu_driver_id;
    WIDGET_ITEM_s *menu_ckbx;
#endif
   UINT32 Pback_menu;
} WIDGET_s;

static WIDGET_s widget;

/*************************************************************************
 * Widget APIs
 ************************************************************************/
int AppWidget_Init(void)
{
    memset(&widget, 0, sizeof(WIDGET_s));
    widget.Flags = WIDGET_NONE;
    widget.Menu = (WIDGET_ITEM_s *)AppMenu_GetWidget();
    widget.MenuQuick = (WIDGET_ITEM_s *)AppMenuQuick_GetWidget();
    widget.MenuAdj = (WIDGET_ITEM_s *)AppMenuAdj_GetWidget();
    widget.MenuTime = (WIDGET_ITEM_s *)AppMenuTime_GetWidget();
    widget.Dialog = (WIDGET_ITEM_s *)AppDialog_GetWidget();
    widget.MenuAdasCalib = (WIDGET_ITEM_s *)AppAdasCalib_GetWidget();
#ifdef CONFIG_APP_ARD
    widget.InfoPrompt = (WIDGET_ITEM_s *)AppInfo_Prompt_GetWidget();
    widget.menu_driver_id = (WIDGET_ITEM_s *)app_menu_driver_id_get_widget();
    widget.menu_ckbx = (WIDGET_ITEM_s *)app_menu_ckbx_get_widget();
#endif
    return 0;
}

int AppWidget_GetCur(void)
{
    return widget.Cur;
}

int AppWidget_On(UINT32 widgetId, UINT32 param)
{
    int rval = 0;

    if (widget.Cur) {
        switch (widget.Cur) {
        case WIDGET_MENU:
            widget.Menu->off(0);
            widget.Cur = WIDGET_NONE;
            AmbaPrint("Menu is hidden for widget %d", widgetId);
            break;
        case WIDGET_MENU_QUICK:
            widget.MenuQuick->off(0);
            widget.Cur = WIDGET_NONE;
            AmbaPrint("Quick Menu is hidden for widget %d", widgetId);
            break;
        case WIDGET_MENU_ADJ:
            widget.MenuAdj->off(0);
            widget.Cur = WIDGET_NONE;
            AmbaPrint("Adjusting Menu is hidden for widget %d", widgetId);
            break;
        case WIDGET_MENU_TIME:
            widget.MenuTime->off(0);
            widget.Cur = WIDGET_NONE;
            AmbaPrint("Time Menu is hidden for widget %d", widgetId);
            break;
        case WIDGET_DIALOG:
            AmbaPrint("Dialog should be decided!!!");
            rval = -1;
            break;
        case WIDGET_MENU_ADAS_CALIB:
            widget.MenuAdasCalib->off(0);
            widget.Cur = WIDGET_NONE;
            AmbaPrint("ADAS calibration Menu is hidden for widget %d", widgetId);
            break;
#ifdef CONFIG_APP_ARD
        case WIDGET_INFO_PROMPT:
            widget.InfoPrompt->off(0);
            widget.Cur = WIDGET_NONE;
            AmbaPrint("Info prompt is hidden for widget %d", widgetId);
            break;
        case WIDGET_MENU_DRIVER_ID:
            widget.menu_driver_id->off(0);
            widget.Cur = WIDGET_NONE;
            AmbaPrint("Driver menu is hidden for widget %d", widgetId);
            break;
        case WIDGET_MENU_CKBX:
            widget.menu_ckbx->off(0);
            widget.Cur = WIDGET_NONE;
            AmbaPrint("CheckBox menu is hidden for widget %d", widgetId);
            break;
#endif
        default:
            AmbaPrint("[App - Wiget] Unknown current widget!!!");
            rval = -1;
            break;
        }
    }

    if (rval < 0) {
        return rval;
    }

    switch (widgetId) {
    case WIDGET_MENU:
        APP_ADDFLAGS(widget.Flags, WIDGET_MENU);
        widget.Menu->on(0);
        widget.Cur = widgetId;
        break;
    case WIDGET_MENU_QUICK:
        APP_ADDFLAGS(widget.Flags, WIDGET_MENU_QUICK);
        widget.MenuQuick->on(0);
        widget.Cur = widgetId;
        break;
    case WIDGET_MENU_ADJ:
        APP_ADDFLAGS(widget.Flags, WIDGET_MENU_ADJ);
        widget.MenuAdj->on(0);
        widget.Cur = widgetId;
        break;
    case WIDGET_MENU_TIME:
        APP_ADDFLAGS(widget.Flags, WIDGET_MENU_TIME);
        widget.MenuTime->on(0);
        widget.Cur = widgetId;
        break;
    case WIDGET_DIALOG:
        APP_ADDFLAGS(widget.Flags, WIDGET_DIALOG);
        widget.Dialog->on(0);
        widget.Cur = widgetId;
        break;
    case WIDGET_MENU_ADAS_CALIB:
        APP_ADDFLAGS(widget.Flags, WIDGET_MENU_ADAS_CALIB);
        widget.MenuAdasCalib->on(0);
        widget.Cur = widgetId;
        break;
#ifdef CONFIG_APP_ARD
    case WIDGET_INFO_PROMPT:
        APP_ADDFLAGS(widget.Flags, WIDGET_INFO_PROMPT);
        widget.InfoPrompt->on(0);
        widget.Cur = widgetId;
        break;
    case WIDGET_MENU_DRIVER_ID:
        APP_ADDFLAGS(widget.Flags, WIDGET_MENU_DRIVER_ID);
        widget.menu_driver_id->on(0);
        widget.Cur = widgetId;
        break;
    case WIDGET_MENU_CKBX:
        APP_ADDFLAGS(widget.Flags, WIDGET_MENU_CKBX);
        widget.menu_ckbx->on(0);
        widget.Cur = widgetId;
        break;
#endif
    default:
        AmbaPrint("[App - Wiget] Unknown widget!!!");
        rval = -1;
        break;
    }

    return rval;
}

int AppWidget_Off(UINT32 widgetId, UINT32 param)
{
    int rval = 0;

    if (widgetId == WIDGET_ALL) {
        if (APP_CHECKFLAGS(widget.Flags, WIDGET_MENU)) {
            APP_REMOVEFLAGS(widget.Flags, WIDGET_MENU);
            if (widget.Cur == WIDGET_MENU) {
                widget.Menu->off(0);
                widget.Cur = WIDGET_NONE;
            }
        }
        if (APP_CHECKFLAGS(widget.Flags, WIDGET_MENU_QUICK)) {
            APP_REMOVEFLAGS(widget.Flags, WIDGET_MENU_QUICK);
            if (widget.Cur == WIDGET_MENU_QUICK) {
                widget.MenuQuick->off(0);
                widget.Cur = WIDGET_NONE;
            }
        }
        if (APP_CHECKFLAGS(widget.Flags, WIDGET_MENU_ADJ)) {
            APP_REMOVEFLAGS(widget.Flags, WIDGET_MENU_ADJ);
            if (widget.Cur == WIDGET_MENU_ADJ) {
                widget.MenuAdj->off(0);
                widget.Cur = WIDGET_NONE;
            }
        }
        if (APP_CHECKFLAGS(widget.Flags, WIDGET_MENU_TIME)) {
            APP_REMOVEFLAGS(widget.Flags, WIDGET_MENU_TIME);
            if (widget.Cur == WIDGET_MENU_ADJ) {
                widget.MenuTime->off(0);
                widget.Cur = WIDGET_NONE;
            }
        }
        if (APP_CHECKFLAGS(widget.Flags, WIDGET_DIALOG)) {
            APP_REMOVEFLAGS(widget.Flags, WIDGET_DIALOG);
            if (widget.Cur == WIDGET_DIALOG) {
                widget.Dialog->OnMessage(DIALOG_SEL_NO, 0, 0);
                widget.Dialog->off(0);
                widget.Cur = WIDGET_NONE;
            }
        }
        if (APP_CHECKFLAGS(widget.Flags, WIDGET_MENU_ADAS_CALIB)) {
            APP_REMOVEFLAGS(widget.Flags, WIDGET_MENU_ADAS_CALIB);
            if (widget.Cur == WIDGET_MENU_ADAS_CALIB) {
                widget.MenuAdasCalib->off(0);
                widget.Cur = WIDGET_NONE;
            }
        }
#ifdef CONFIG_APP_ARD
        if (APP_CHECKFLAGS(widget.Flags, WIDGET_INFO_PROMPT)) {
            APP_REMOVEFLAGS(widget.Flags, WIDGET_INFO_PROMPT);
            if (widget.Cur == WIDGET_INFO_PROMPT) {
                widget.InfoPrompt->off(0);
                widget.Cur = WIDGET_NONE;
            }
        }
        if (APP_CHECKFLAGS(widget.Flags, WIDGET_MENU_DRIVER_ID)) {
            APP_REMOVEFLAGS(widget.Flags, WIDGET_MENU_DRIVER_ID);
            if (widget.Cur == WIDGET_MENU_DRIVER_ID) {
                widget.menu_driver_id->off(0);
                widget.Cur = WIDGET_NONE;
            }
        }
        if (APP_CHECKFLAGS(widget.Flags, WIDGET_MENU_CKBX)) {
            APP_REMOVEFLAGS(widget.Flags, WIDGET_MENU_CKBX);
            if (widget.Cur == WIDGET_MENU_CKBX) {
                widget.menu_ckbx->off(0);
                widget.Cur = WIDGET_NONE;
            }
        }
#endif
        if (!APP_CHECKFLAGS(param, WIDGET_HIDE_SILENT)) {
            AppLibComSvcHcmgr_SendMsg(AMSG_STATE_WIDGET_CLOSED, 0, 0);
        }
    } else {
        if (widgetId == widget.Cur) {
            switch (widgetId) {
            case WIDGET_MENU:
                APP_REMOVEFLAGS(widget.Flags, WIDGET_MENU);
                widget.Menu->off(0);
                widget.Cur = WIDGET_NONE;
                break;
            case WIDGET_MENU_QUICK:
                APP_REMOVEFLAGS(widget.Flags, WIDGET_MENU_QUICK);
                widget.MenuQuick->off(0);
                widget.Cur = WIDGET_NONE;
                break;
            case WIDGET_MENU_ADJ:
                APP_REMOVEFLAGS(widget.Flags, WIDGET_MENU_ADJ);
                widget.MenuAdj->off(0);
                widget.Cur = WIDGET_NONE;
                break;
            case WIDGET_MENU_TIME:
                APP_REMOVEFLAGS(widget.Flags, WIDGET_MENU_TIME);
                widget.MenuTime->off(0);
                widget.Cur = WIDGET_NONE;
                break;
            case WIDGET_DIALOG:
                APP_REMOVEFLAGS(widget.Flags, WIDGET_DIALOG);
                widget.Dialog->off(0);
                widget.Cur = WIDGET_NONE;
                break;
            case WIDGET_MENU_ADAS_CALIB:
                APP_REMOVEFLAGS(widget.Flags, WIDGET_MENU_ADAS_CALIB);
                widget.MenuAdasCalib->off(0);
                widget.Cur = WIDGET_NONE;
                break;
#ifdef CONFIG_APP_ARD
            case WIDGET_INFO_PROMPT:
                APP_REMOVEFLAGS(widget.Flags, WIDGET_INFO_PROMPT);
                widget.InfoPrompt->off(0);
                widget.Cur = WIDGET_NONE;
                break;
            case WIDGET_MENU_DRIVER_ID:
                APP_REMOVEFLAGS(widget.Flags, WIDGET_MENU_DRIVER_ID);
                widget.menu_driver_id->off(0);
                widget.Cur = WIDGET_NONE;
                break;
            case WIDGET_MENU_CKBX:
                APP_REMOVEFLAGS(widget.Flags, WIDGET_MENU_CKBX);
                widget.menu_ckbx->off(0);
                widget.Cur = WIDGET_NONE;
                break;
#endif
            default:
                AmbaPrint("[App - Wiget] Unknown widget!!!");
                rval = -1;
                break;
            }
            if (widget.Flags) {
                AmbaPrint("Some other widgets are still ON");
                if (APP_CHECKFLAGS(widget.Flags, WIDGET_DIALOG)) {
                    AppWidget_On(WIDGET_DIALOG, 0);
                } else if (APP_CHECKFLAGS(widget.Flags, WIDGET_MENU_TIME)) {
                    AppWidget_On(WIDGET_MENU_TIME, 0);
                } else if (APP_CHECKFLAGS(widget.Flags, WIDGET_MENU_ADJ)) {
                    AppWidget_On(WIDGET_MENU_ADJ, 0);
                } else if (APP_CHECKFLAGS(widget.Flags, WIDGET_MENU_QUICK)) {
                    AppWidget_On(WIDGET_MENU_QUICK, 0);
                } else if (APP_CHECKFLAGS(widget.Flags, WIDGET_MENU_ADAS_CALIB)) {
                    AppWidget_On(WIDGET_MENU_ADAS_CALIB, 0);
#ifdef CONFIG_APP_ARD
                } else if (APP_CHECKFLAGS(widget.Flags, WIDGET_INFO_PROMPT)) {
                    AppWidget_On(WIDGET_INFO_PROMPT, 0);
                } else if (APP_CHECKFLAGS(widget.Flags, WIDGET_MENU_DRIVER_ID)) {
                    AppWidget_On(WIDGET_MENU_DRIVER_ID, 0);
                } else if (APP_CHECKFLAGS(widget.Flags, WIDGET_MENU_CKBX)) {
                    AppWidget_On(WIDGET_MENU_CKBX, 0);
#endif
                } else if (APP_CHECKFLAGS(widget.Flags, WIDGET_MENU)) {
                    AppWidget_On(WIDGET_MENU, 0);
                }
            } else if (!APP_CHECKFLAGS(param, WIDGET_HIDE_SILENT)) {
                AppLibComSvcHcmgr_SendMsg(AMSG_STATE_WIDGET_CLOSED, 0, 0);
            }
        } else {
            AmbaPrint("[App - Wiget] Should not close non-current widget");
        }
    }

    return rval;
}

int AppWidget_OnMessage(UINT32 msg, UINT32 param1, UINT32 param2)
{
    int rval = WIDGET_PASSED_MSG;

    if (widget.Cur) {
#ifdef CONFIG_APP_ARD
    if (MSG_MDL_ID(msg) == MDL_APP_KEY_ID) {
        /*Don't beep if it is release key*/
        if(((msg >>8)&0xff) != HMSG_KEY_BUTTON_ID_RELEASE) {
            AppLibAudioDec_Beep(BEEP_OPTONE,0);
        }
    }
#endif
    switch (widget.Cur) {
    case WIDGET_MENU:
        rval = widget.Menu->OnMessage(msg, param1, param2);
        break;
    case WIDGET_MENU_QUICK:
        rval = widget.MenuQuick->OnMessage(msg, param1, param2);
        break;
    case WIDGET_MENU_ADJ:
        rval = widget.MenuAdj->OnMessage(msg, param1, param2);
        break;
    case WIDGET_MENU_TIME:
        rval = widget.MenuTime->OnMessage(msg, param1, param2);
        break;
    case WIDGET_DIALOG:
        rval = widget.Dialog->OnMessage(msg, param1, param2);
        break;
    case WIDGET_MENU_ADAS_CALIB:
        rval = widget.MenuAdasCalib->OnMessage(msg, param1, param2);
        break;
#ifdef CONFIG_APP_ARD
    case WIDGET_INFO_PROMPT:
        rval = widget.InfoPrompt->OnMessage(msg, param1, param2);
        break;
    case WIDGET_MENU_DRIVER_ID:
        rval = widget.menu_driver_id->OnMessage(msg, param1, param2);
        break;
    case WIDGET_MENU_CKBX:
        rval = widget.menu_ckbx->OnMessage(msg, param1, param2);
        break;
#endif
    default:
        AmbaPrint("[App - Wiget] Unknown current widget");
        rval = -1;
        break;
    }
#if 0
    if (rval == 0) {
        AmbaPrint("[App - Widget] message was handled by widget");
    } else if (rval > 0) {
        AmbaPrint("[App - Widget] message wasn't handled by widget");
    } else if (rval < 0) {
        AmbaPrint("[App - Widget] message handling error");
    }
#endif
    }

    return rval;
}

UINT32 AppWidget_GetFlags(void)
{
    return widget.Flags;
}

void AppWidget_SetPback(UINT32 value)
{
   widget.Pback_menu = value;
}
UINT32 AppWidget_GetPback(void)
{
    return widget.Pback_menu;
}

