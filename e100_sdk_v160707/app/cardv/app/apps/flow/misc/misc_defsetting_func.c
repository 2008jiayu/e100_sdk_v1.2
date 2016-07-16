/**
  * @file src/app/apps/flow/misc/connectedcam/misc_defsetting_func.c
  *
  *  Functions of Reset the system setting to default application
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


#include <apps/flow/misc/misc_defsetting.h>
#include <system/status.h>
#include <system/app_pref.h>
#include <system/app_util.h>
#include <apps/gui/resource/gui_settle.h>
#include <apps/flow/widget/menu/widget.h>
#ifdef CONFIG_APP_ARD
#include "AmbaSysCtrl.h"
#endif

static int misc_defsetting_start(void)
{
    int ReturnValue = 0;

    misc_defsetting.Func(MISC_DEFSETTING_DIALOG_SHOW_DEFSETTING, 0, 0);

    return ReturnValue;
}


static int misc_defsetting_stop(void)
{
    int ReturnValue = 0;

    if (APP_CHECKFLAGS(app_misc_defsetting.GFlags, APP_AFLAGS_POPUP)) {
        APP_REMOVEFLAGS(app_misc_defsetting.GFlags, APP_AFLAGS_POPUP);
        ReturnValue = AppWidget_Off(WIDGET_DIALOG, 0);
    }

    return ReturnValue;
}

static int default_dialog_ok_handler(UINT32 sel, UINT32 param1, UINT32 param2)
{
    int ReturnValue = 0;
    APP_REMOVEFLAGS(app_misc_defsetting.GFlags, APP_AFLAGS_POPUP);
    switch (sel) {
    default:
    case DIALOG_SEL_OK:
#ifdef CONFIG_APP_ARD
	AmbaSysSoftReset();
#else
    AppUtil_SwitchApp(AppUtil_GetStartApp(0));
#endif
        break;
    }
    return ReturnValue;
}

#if 0
static void default_param_set(void)
{
        /* Set the "initial version", system will refresh the preference when next boot.*/
        UserSetting->SystemPref.SystemVersion = 0x0011;
#ifdef CONFIG_APP_ARD
	 //UserSetting->VideoPref.parkingmode_sensitivity = 0;
#endif
        UserSetting->SetupPref.LanguageID = MENU_SETUP_LANGUAGE_CHINESE_SIMPLIFIED;
        UserSetting->MotionDetectPref.MotionDetect = MOTION_DETECT_OFF; 
        UserSetting->GSensorSentivityPref.Gsensor_sensitivity = MENU_VIDEO_GSENSOR_SENSITIVITY_M;
        UserSetting->VideoPref.parkingmode_sensitivity = MENU_VIDEO_PARKINGMODE_OFF;
        UserSetting->VideoPref.video_split_rec_time = MENU_VIDEO_SPLIT_REC_MODE_3MIN;
        UserSetting->ImagePref.Flicker = ANTI_FLICKER_50HZ;		
        UserSetting->SetupPref.backlightoff_time = BACKLIGHTOFF_TIME_OFF;	
        UserSetting->SetupPref.fcws_mode_onoff	 = 1;	
        UserSetting->SetupPref.ldws_mode_onoff = 1;	
        UserSetting->SetupPref.hmws_mode_onoff = 1;	
        UserSetting->SetupPref.fcmr_mode_onoff = 1;	
        UserSetting->SetupPref.adas_alarm_dis = 1;		
}
#endif

static int default_dialog_default_handler(UINT32 sel, UINT32 param1, UINT32 param2)
{
    int ReturnValue = 0;
    APP_REMOVEFLAGS(app_misc_defsetting.GFlags, APP_AFLAGS_POPUP);
    switch (sel) {
    case DIALOG_SEL_YES:
        /* Set the "initial version", system will refresh the preference when next boot.*/
        UserSetting->SystemPref.SystemVersion = 0;
#ifdef CONFIG_APP_ARD
	 UserSetting->VideoPref.parkingmode_sensitivity = 0;
#endif        
         /* Save the system preference.  */
        AppPref_Save();
        //ReturnValue = AppDialog_SetDialog(DIALOG_TYPE_OK, DIALOG_SUB_DEFAULT_SETTING_REBOOT, default_dialog_ok_handler);
        //ReturnValue = AppWidget_On(WIDGET_DIALOG, 0);
        //APP_ADDFLAGS(app_misc_defsetting.GFlags, APP_AFLAGS_POPUP);
#ifdef CONFIG_APP_ARD
        AmbaSysSoftReset();
#else
	 AppUtil_SwitchApp(AppUtil_GetStartApp(0));
#endif        
        break;

    default:
    case DIALOG_SEL_NO:
        AppUtil_SwitchApp(AppUtil_GetStartApp(0));
        break;
    }

    return ReturnValue;
}

static int misc_defsetting_dialog_show_defsetting(void)
{
    int ReturnValue = 0;

    ReturnValue = AppDialog_SetDialog(DIALOG_TYPE_Y_N, DIALOG_SUB_DEFAULT_SETTING_YES_OR_NO, default_dialog_default_handler);
    ReturnValue = AppWidget_On(WIDGET_DIALOG, 0);
    APP_ADDFLAGS(app_misc_defsetting.GFlags, APP_AFLAGS_POPUP);

    return ReturnValue;
}

int misc_defsetting_func(UINT32 funcId, UINT32 param1, UINT32 param2)
{
    int ReturnValue = 0;

    switch (funcId) {
    case MISC_DEFSETTING_START:
        misc_defsetting_start();
        break;
    case MISC_DEFSETTING_STOP:
        misc_defsetting_stop();
        break;
    case MISC_DEFSETTING_DIALOG_SHOW_DEFSETTING:
        misc_defsetting_dialog_show_defsetting();
        break;
    default:
        AmbaPrint("[app_misc_defsetting] The function is not defined");
        break;
    }

    return ReturnValue;
}
