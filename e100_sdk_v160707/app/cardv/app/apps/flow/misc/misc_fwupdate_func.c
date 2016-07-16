/**
  * @file src/app/apps/flow/misc/connectedcam/misc_fwupdate_func.c
  *
  *  Functions of photo playback application
  *
  * History:
  *    2014/03/20 - [Martin Lai] created file
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


#include <apps/flow/misc/misc_fwupdate.h>
#include <system/status.h>
#include <system/app_pref.h>
#include <system/app_util.h>
#include <apps/gui/resource/gui_settle.h>
#include <apps/flow/widget/menu/widget.h>
#include <comsvc/FwUpdate/AmbaFwUpdaterSD.h>
#ifdef CONFIG_APP_ARD
#include "AmbaSysCtrl.h"
#include "AmbaGPIO.h"
#include "AmbaPWC.h"
#endif

#ifdef CONFIG_APP_ARD
#if 0
static void misc_fwupdate_LED_timer_handler(int eid)
{
    static UINT8 led_on = 0;
	
    if (eid == TIMER_UNREGISTER) {
        return;
    }

    AmbaPrint("LED_timer");

    if(led_on){
    	led_on = 0;
	AmbaGPIO_ConfigOutput(GPIO_PIN_25, AMBA_GPIO_LEVEL_LOW);
    }else{
	led_on = 1;
	AmbaGPIO_ConfigOutput(GPIO_PIN_25, AMBA_GPIO_LEVEL_HIGH);
    }

}
#endif
#endif

static int misc_fwupdate_start(void)
{
    int ReturnValue = 0;
    misc_fwupdate.Func(MISC_FWUPDATE_WARNING_MSG_SHOW_START, GUI_WARNING_SYSREM_UPGRATE_PROCESSING, 1);

    misc_fwupdate.Func(MISC_FWUPDATE_DIALOG_SHOW_FWUPDATE, 0, 0);

    app_status.FwUpdate = 0;

    return ReturnValue;
}

static int misc_fwupdate_stop(void)
{
    int ReturnValue = 0;

    if (APP_CHECKFLAGS(app_misc_fwupdate.GFlags, APP_AFLAGS_POPUP)) {
        APP_REMOVEFLAGS(app_misc_fwupdate.GFlags, APP_AFLAGS_POPUP);
        ReturnValue = AppWidget_Off(WIDGET_DIALOG, 0);
    }

    misc_fwupdate.Func(MISC_FWUPDATE_WARNING_MSG_SHOW_STOP, 0, 0);
    misc_fwupdate.Gui(GUI_FWUPDATE_RATIO_HIDE, 0, 0);
    misc_fwupdate.Gui(GUI_FWUPDATE_STAGE_HIDE, 0, 0);
    misc_fwupdate.Gui(GUI_FLUSH, 0, 0);
    misc_fwupdate.Gui(GUI_HIDE_ALL, 0, 0);
    misc_fwupdate.Gui(GUI_FLUSH, 0, 0);

    return ReturnValue;
}

static int misc_fwupdate_mode_switch(void)
{
    int ReturnValue = 0;
    /** Switch to decode mode if the Fchan will be enabled and the app_status.jack_to_pb_mode is on.*/
    ReturnValue = AppLibDisp_SelectDevice(DISP_CH_FCHAN, DISP_ANY_DEV);
    if (APP_CHECKFLAGS(ReturnValue, DISP_FCHAN_NO_DEVICE) || (app_status.FchanDecModeOnly == 0) ) {
        app_status.LockDecMode = 0;
        AppUtil_SwitchApp(AppUtil_GetStartApp(0));
    } else {
        ReturnValue = AppLibDisp_ConfigMode(DISP_CH_FCHAN, AppLibSysVout_GetVoutMode(VOUT_DISP_MODE_1080P));
        if (ReturnValue < 0) {
            app_status.LockDecMode = 0;
            AppUtil_SwitchApp(AppUtil_GetStartApp(0));
        } else {
            /*Switch to thumbnail mode if the fchan will be enabled.*/
            app_status.LockDecMode = 1;
            AppUtil_SwitchApp(APP_THUMB_MOTION);
        }
    }

    return 0;
}

static int misc_fwupdate_dialog_ok_handler(UINT32 sel, UINT32 param1, UINT32 param2)
{
    int ReturnValue=0;

    switch (sel) {
    default:
    case DIALOG_SEL_OK:
#ifdef CONFIG_APP_ARD
        AmbaPrint("[App Handler] AMSG_CMD_SW_REBOOT !");
        {        
		extern void sd_update(void);
		
		//ReturnValue = AppLibComSvcTimer_Register(TIMER_20HZ, misc_fwupdate_LED_timer_handler);
		AmbaGPIO_ConfigOutput(GPIO_PIN_25, AMBA_GPIO_LEVEL_HIGH);
		sd_update();
		AmbaGPIO_ConfigOutput(GPIO_PIN_25, AMBA_GPIO_LEVEL_LOW);
        }
        misc_fwupdate.Func(MISC_FWUPDATE_WARNING_MSG_SHOW_STOP, 0, 0);
        misc_fwupdate.Func(MISC_FWUPDATE_WARNING_MSG_SHOW_START,GUI_WARNING_SYSREM_UPGRATE_SUCCESS, 1);
        AmbaKAL_TaskSleep(200);
    	AmbaPWC_Init();
    	AmbaPWC_ForcePowerDownSequence();
#endif
        /**DO NOTHING*/
        break;
    }

    return ReturnValue;
}

static int misc_fwupdate_dialog_fwupdate_handler(UINT32 sel, UINT32 param1, UINT32 param2)
{
    int ReturnValue=0;
    APP_REMOVEFLAGS(app_misc_fwupdate.GFlags, APP_AFLAGS_BUSY);
    APP_REMOVEFLAGS(app_misc_fwupdate.GFlags, APP_AFLAGS_POPUP);
    switch (sel) {
    case DIALOG_SEL_YES:
        APP_ADDFLAGS(app_misc_fwupdate.GFlags, APP_AFLAGS_BUSY);
        APP_ADDFLAGS(app_misc_fwupdate.GFlags, APP_AFLAGS_POPUP);
        #ifdef CONFIG_ENABLE_EMMC_BOOT
        ReturnValue = AmbaEmmcFwUpdaterSD_SetMagicCode();
        #else
        #ifndef CONFIG_APP_ARD
        ReturnValue = AmbaFwUpdaterSD_SetMagicCode();
        #endif
        #endif
        ReturnValue = AppDialog_SetDialog(DIALOG_TYPE_OK, DIALOG_SUB_DEFAULT_SETTING_REBOOT, misc_fwupdate_dialog_ok_handler);
        ReturnValue = AppWidget_On(WIDGET_DIALOG, 0);
        break;
    default:
    case DIALOG_SEL_NO:
        ReturnValue = AppUtil_BusyCheck(0);
        if (APP_CHECKFLAGS(app_misc_fwupdate.GFlags, APP_AFLAGS_READY)) {
            /* The system could switch the current app to other in the function "AppUtil_BusyCheck". */
            misc_fwupdate.Func(MISC_FWUPDATE_SWITCH_APP, 0, 0);
        }
        break;
    }

    return ReturnValue;
}


static int misc_fwupdate_dialog_show_fwupdate(void)
{
    int ReturnValue = 0;

    ReturnValue = AppLibCard_CheckStatus(0);

    if (ReturnValue == CARD_STATUS_NO_CARD) {
        AmbaPrintColor(RED,"[app_misc_fwupdate] WARNING_NO_CARD");
        AppUtil_SwitchApp(AppUtil_GetStartApp(0));
    } else {
        ReturnValue = AppDialog_SetDialog(DIALOG_TYPE_Y_N, DIALOG_SUB_FWUPDATE_YES_OR_NO, misc_fwupdate_dialog_fwupdate_handler);
        ReturnValue = AppWidget_On(WIDGET_DIALOG, 0);
        APP_ADDFLAGS(app_misc_fwupdate.GFlags, APP_AFLAGS_POPUP);
        APP_ADDFLAGS(app_misc_fwupdate.GFlags, APP_AFLAGS_BUSY);
    }

    return ReturnValue;
}

static int misc_fwupdate_card_removed(void)
{
    int ReturnValue = 0;

    misc_fwupdate.Func(MISC_FWUPDATE_WARNING_MSG_SHOW_STOP, 0, 0);
    APP_REMOVEFLAGS(app_misc_fwupdate.GFlags, APP_AFLAGS_BUSY);

    /* Skip the non-optimum format when card remove.*/
    app_status.CardFmtParam = 0;

    misc_fwupdate.Func(MISC_FWUPDATE_SWITCH_APP, 0, 0);

    return ReturnValue;
}

static int misc_fwupdate_card_error_removed(void)
{
    int ReturnValue = 0;

    misc_fwupdate.Func(MISC_FWUPDATE_CARD_REMOVED, 0, 0);

    return ReturnValue;
}

static int misc_fwupdate_update_fchan_vout(UINT32 msg)
{
   int ReturnValue = 0;

    switch (msg) {
    case HMSG_HDMI_INSERT_SET:
    case HMSG_HDMI_INSERT_CLR:
        AppLibSysVout_SetJackHDMI(app_status.HdmiPluginFlag);
        break;
    case HMSG_CS_INSERT_SET:
    case HMSG_CS_INSERT_CLR:
        AppLibSysVout_SetJackCs(app_status.CompositePluginFlag);
        break;
    default:
        AmbaPrint("[app_misc_fwupdate] Vout no changed");
        return 0;
        break;
    }
    ReturnValue = AppLibDisp_SelectDevice(DISP_CH_FCHAN, DISP_ANY_DEV);
    if (APP_CHECKFLAGS(ReturnValue, DISP_FCHAN_NO_CHANGE)) {
        AmbaPrint("[app_misc_fwupdate] Display FCHAN has no changed");
    } else {
        if (APP_CHECKFLAGS(ReturnValue, DISP_FCHAN_NO_DEVICE)) {
            AppLibDisp_ChanStop(DISP_CH_FCHAN);
            AppLibDisp_DeactivateWindow(DISP_CH_FCHAN, AppLibDisp_GetWindowId(DISP_CH_FCHAN, 0));
            AppLibDisp_FlushWindow(DISP_CH_FCHAN);
            app_status.LockDecMode = 0;
        } else {
            AppLibDisp_ConfigMode(DISP_CH_FCHAN, AppLibSysVout_GetVoutMode(VOUT_DISP_MODE_1080P));
            AppLibDisp_SetupChan(DISP_CH_FCHAN);
            AppLibDisp_ChanStart(DISP_CH_FCHAN);
            {
                AMP_DISP_WINDOW_CFG_s Window;
                AMP_DISP_INFO_s DispDev = {0};

                memset(&Window, 0, sizeof(AMP_DISP_WINDOW_CFG_s));

                ReturnValue = AppLibDisp_GetDeviceInfo(DISP_CH_FCHAN, &DispDev);
                if ((ReturnValue < 0) || (DispDev.DeviceInfo.Enable == 0)) {
                    DBGMSG("[app_misc_fwupdate] FChan Disable. Disable the fchan Window");
                    AppLibDisp_DeactivateWindow(DISP_CH_FCHAN, AppLibDisp_GetWindowId(DISP_CH_FCHAN, 0));
                    AppLibDisp_FlushWindow(DISP_CH_FCHAN);
                    app_status.LockDecMode = 0;
                } else {
                    /** FCHAN Window*/
                    AppLibDisp_GetWindowConfig(DISP_CH_FCHAN, AppLibDisp_GetWindowId(DISP_CH_FCHAN, 0), &Window);
                    Window.Source = AMP_DISP_DEC;
                    Window.SourceDesc.Dec.DecHdlr = 0;
                    Window.CropArea.Width = 0;
                    Window.CropArea.Height = 0;
                    Window.CropArea.X = 0;
                    Window.CropArea.Y = 0;
                    Window.TargetAreaOnPlane.Width = DispDev.DeviceInfo.VoutWidth;
                    Window.TargetAreaOnPlane.Height = DispDev.DeviceInfo.VoutHeight;
                    Window.TargetAreaOnPlane.X = 0;
                    Window.TargetAreaOnPlane.Y = 0;
                    AppLibDisp_SetWindowConfig(DISP_CH_FCHAN, AppLibDisp_GetWindowId(DISP_CH_FCHAN, 0), &Window);
                    AppLibDisp_ActivateWindow(DISP_CH_FCHAN, AppLibDisp_GetWindowId(DISP_CH_FCHAN, 0));
                    AppLibDisp_FlushWindow(DISP_CH_FCHAN);
                }
            }
            AppLibGraph_SetWindowConfig(GRAPH_CH_FCHAN);
            AppLibGraph_ActivateWindow(GRAPH_CH_FCHAN);
            AppLibGraph_FlushWindow(GRAPH_CH_FCHAN);
            misc_fwupdate.Gui(GUI_SET_LAYOUT, 0, 0);
            misc_fwupdate.Gui(GUI_FLUSH, 0, 0);
        }
    }

    return ReturnValue;
}


/**
 *  @brief The timer of warning message.
 *
 *  The timer of warning message.
 *
 *  @param [in] eid event id
 *
 *  @return >=0 success, <0 failure
 */
static void misc_fwupdate_warning_timer_handler(int eid)
{
    static int BlinkCount;

    if (eid == TIMER_UNREGISTER) {
        BlinkCount = 0;
        return;
    }

    BlinkCount++;

    if (BlinkCount & 0x01) {
        misc_fwupdate.Gui(GUI_WARNING_HIDE, 0, 0);
    } else {
        misc_fwupdate.Gui(GUI_WARNING_SHOW, 0, 0);
    }

    if (BlinkCount >= 5) {
        APP_REMOVEFLAGS(app_misc_fwupdate.Flags, MISC_FWUPDATE_WARNING_MSG_RUN);
        AppLibComSvcTimer_Unregister(TIMER_2HZ, misc_fwupdate_warning_timer_handler);
        misc_fwupdate.Gui(GUI_WARNING_HIDE, 0, 0);
    }
    misc_fwupdate.Gui(GUI_FLUSH, 0, 0);

}

static int misc_fwupdate_warning_msg_show(int enable, int param1, int param2)
{
    int ReturnValue = 0;

    if (enable) {
        if (param2) {
            misc_fwupdate.Gui(GUI_WARNING_UPDATE, param1, 0);
            misc_fwupdate.Gui(GUI_WARNING_SHOW, 0, 0);
        } else {
            if (!APP_CHECKFLAGS(app_misc_fwupdate.Flags, MISC_FWUPDATE_WARNING_MSG_RUN)) {
                APP_ADDFLAGS(app_misc_fwupdate.Flags, MISC_FWUPDATE_WARNING_MSG_RUN);
                misc_fwupdate.Gui(GUI_WARNING_UPDATE, param1, 0);
                misc_fwupdate.Gui(GUI_WARNING_SHOW, 0, 0);
                AppLibComSvcTimer_Register(TIMER_2HZ, misc_fwupdate_warning_timer_handler);
            }
        }
    } else {
        if (APP_CHECKFLAGS(app_misc_fwupdate.Flags, MISC_FWUPDATE_WARNING_MSG_RUN)) {
            APP_REMOVEFLAGS(app_misc_fwupdate.Flags, MISC_FWUPDATE_WARNING_MSG_RUN);
            AppLibComSvcTimer_Unregister(TIMER_2HZ, misc_fwupdate_warning_timer_handler);
        }
        misc_fwupdate.Gui(GUI_WARNING_HIDE, 0, 0);
    }
    misc_fwupdate.Gui(GUI_FLUSH, 0, 0);
    return ReturnValue;
}


int misc_fwupdate_func(UINT32 funcId, UINT32 param1, UINT32 param2)
{
    int ReturnValue = 0;

    switch (funcId) {
    case MISC_FWUPDATE_START:
        ReturnValue = misc_fwupdate_start();
        break;
    case MISC_FWUPDATE_STOP:
        ReturnValue = misc_fwupdate_stop();
        break;
    case MISC_FWUPDATE_SWITCH_APP:
        ReturnValue = misc_fwupdate_mode_switch();
        break;
    case MISC_FWUPDATE_DIALOG_SHOW_FWUPDATE:
        ReturnValue = misc_fwupdate_dialog_show_fwupdate();
        break;
    case MISC_FWUPDATE_CARD_REMOVED:
        ReturnValue = misc_fwupdate_card_removed();
        break;
    case MISC_FWUPDATE_CARD_ERROR_REMOVED:
        ReturnValue = misc_fwupdate_card_error_removed();
        break;
    case MISC_FWUPDATE_UPDATE_FCHAN_VOUT:
        ReturnValue = misc_fwupdate_update_fchan_vout(param1);
        break;
    case MISC_FWUPDATE_WARNING_MSG_SHOW_START:
        ReturnValue = misc_fwupdate_warning_msg_show( 1, param1, param2);
        break;
    case MISC_FWUPDATE_WARNING_MSG_SHOW_STOP:
        ReturnValue = misc_fwupdate_warning_msg_show( 0, param1, param2);
        break;
    default:
        AmbaPrint("[app_misc_fwupdate] The function is not defined");
        break;
    }

    return ReturnValue;
}
