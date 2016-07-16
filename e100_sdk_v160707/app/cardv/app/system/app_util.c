/**
 * @file app/connected/app/system/app_util.c
 *
 * Implemention of Demo application utility
 *
 * History:
 *    2013/08/16 - [Martin Lai] created file
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
#include "app_util.h"
#include "app_pref.h"
#include "status.h"
#include <applib.h>
#include <wchar.h>
#include <AmbaRTC.h>
#include <usb/AmbaUSB_API.h>
#include <calibration/ApplibCalibMgr.h>
#include <AmbaGPIO.h>
#ifdef CONFIG_APP_ARD
#include "AmbaSysCtrl.h"
#include "AmbaADC.h"
#include "AmbaWDT.h"
#include <system/ApplibSys_Lcd.h>
#include <apps/flow/rec/rec_cam.h>
#include <apps/gui/resource/gui_settle.h>
#endif

static int usb_charge_check_running = 0;
#ifdef CONFIG_APP_ARD
static int power_off_reset_flag = 0;
static int power_off_time = 0;
#endif

/**
 *  @brief Get the status of USB charger mode.
 *
 *  Get the status of USB charger mode.
 *
 *  @return Status of USB charger mode.
 */
int AppUtil_UsbChargeCheckingGetStatus(void)
{
    return usb_charge_check_running;
}

void AppUtil_UsbChargeCheckingSetStatus(int status)
{
    usb_charge_check_running = status;
}

/**
 *  @brief To excute the functions that system block them when the Ready flag is not enabled.
 *
 *  To excute the functions that system block them when the Ready flag is not enabled.
 *
 *  @param [in] param Parameter
 *
 *  @return >=0 success, <0 failure
 */
int AppUtil_ReadyCheck(UINT32 param)
{
    int ReturnValue = 0;

    /* Check unsaved movie status */
    if (app_status.MvRecover == 1) {
        AppUtil_SwitchApp(APP_MISC_MVRECOVER);
        ReturnValue = 0;
        return ReturnValue;
    }

    /* Check firmware existence */
    if (app_status.FwUpdate == 1) {
        AppUtil_SwitchApp(APP_MISC_FWUPDATE);
        ReturnValue = 0;
        return ReturnValue;
    }

    /* Check Calibration Script */
    if (app_status.CalibRework == 1) {
        AppUtil_SwitchApp(APP_MISC_CALIBUPDATE);
        ReturnValue = 0;
        return ReturnValue;
    }

    /* Check card format parameter */
    if ((app_status.CardFmtParam == APP_CARD_FMT_NONOPTIMUM)) {
        AppUtil_SwitchApp(APP_MISC_FORMATCARD);
        ReturnValue = 0;
        return ReturnValue;
    }

    /* Check USB cable */
    if ((app_status.UsbPluginFlag == 1) &&
        (AppUtil_UsbChargeCheckingGetStatus() == 0) &&
        (app_status.UsbChargeMode == 0) &&
        (AmbaUser_CheckIsUsbSlave())) {
        APP_APP_s *CurApp;
        AppAppMgt_GetCurApp(&CurApp);
        CurApp->OnMessage(AMSG_CMD_USB_APP_START, 0, 0);
    }

#ifdef CONFIG_APP_ARD
    if ((app_status.UsbPluginFlag == 1) && (app_status.UsbTreatAsDC == 1)){
    AppLibComSvcHcmgr_SendMsg(HMSG_USB_DETECT_CONNECT, 0, 0);
    app_status.UsbTreatAsDC = 0;
    app_status.UsbPluginFlag = 0;
    app_status.UsbChargeMode = 0;
    AmbaPrintColor(GREEN,"ReadyCheck usb");
    }
#endif

    /* Enable the fchan device after finishing the mode switch */
    if (app_status.LockDecMode == 1) {
        ReturnValue =  AppLibDisp_GetDeviceID(DISP_CH_FCHAN);
        if (ReturnValue != AMP_DSIP_NONE) {
            AppLibDisp_SelectDevice(DISP_CH_FCHAN, DISP_ANY_DEV);
            AppLibDisp_ConfigMode(DISP_CH_FCHAN, AppUtil_GetVoutMode(0));
            AppLibDisp_SetupChan(DISP_CH_FCHAN);
            AppLibDisp_ChanStart(DISP_CH_FCHAN);
        }
    }

    /**Check vout */
    if ((app_status.HdmiPluginFlag == 1) &&
        (app_status.HdmiPluginFlag != AppLibSysVout_CheckJackHDMI())) {
        APP_APP_s *CurApp;
        AppAppMgt_GetCurApp(&CurApp);
        CurApp->OnMessage(HMSG_HDMI_INSERT_SET, 0, 0);
    } else if ((app_status.HdmiPluginFlag == 0) &&
        (app_status.HdmiPluginFlag != AppLibSysVout_CheckJackHDMI())) {
        APP_APP_s *CurApp;
        AppAppMgt_GetCurApp(&CurApp);
        CurApp->OnMessage(HMSG_HDMI_INSERT_CLR, 0, 0);
    }
    if ((app_status.CompositePluginFlag == 1) &&
        (app_status.CompositePluginFlag != AppLibSysVout_CheckJackCs())) {
        APP_APP_s *CurApp;
        AppAppMgt_GetCurApp(&CurApp);
        CurApp->OnMessage(HMSG_CS_INSERT_SET, 0, 0);
    } else if ((app_status.CompositePluginFlag == 0) &&
        (app_status.CompositePluginFlag != AppLibSysVout_CheckJackCs())) {
        APP_APP_s *CurApp;
        AppAppMgt_GetCurApp(&CurApp);
        CurApp->OnMessage(HMSG_CS_INSERT_CLR, 0, 0);
    }
#if 0
    /**Check audio input */
    if ((app_status.LineinPluginFlag == 1) &&
         (app_status.LineinPluginFlag != app_audio_check_jack_linein())) {
        appmgt_get_curapp(&CurApp);
        CurApp->OnMessage(HMSG_LINEIN_IN_SET, 0, 0);
    } else if ((app_status.LineinPluginFlag == 0) &&
         (app_status.LineinPluginFlag != app_audio_check_jack_linein())) {
        appmgt_get_curapp(&CurApp);
        CurApp->OnMessage(HMSG_LINEIN_IN_CLR, 0, 0);
    }

    /**Check audio output */
    if ((app_status.LineoutPluginFlag == 1) &&
         (app_status.LineoutPluginFlag != app_audio_check_jack_lineout())) {
             appmgt_get_curapp(&CurApp);
        CurApp->OnMessage(HMSG_LINEOUT_IN_SET, 0, 0);
    } else if ((app_status.LineoutPluginFlag == 0) &&
         (app_status.LineoutPluginFlag != app_audio_check_jack_lineout())) {
             appmgt_get_curapp(&CurApp);
        CurApp->OnMessage(HMSG_LINEOUT_IN_CLR, 0, 0);
    }
    if ((app_status.hp_plugin_flag == 1) &&
         (app_status.hp_plugin_flag != app_audio_check_jack_hp())) {
        appmgt_get_curapp(&CurApp);
        CurApp->OnMessage(HMSG_HP_IN_SET, 0, 0);
    } else if ((app_status.hp_plugin_flag == 0) &&
         (app_status.hp_plugin_flag != app_audio_check_jack_hp())) {
        appmgt_get_curapp(&CurApp);
        CurApp->OnMessage(HMSG_HP_IN_CLR, 0, 0);
    }
#endif
    /**Check app switch*/
    if (app_status.AppSwitchBlocked > 0) {
        AppUtil_SwitchApp(app_status.AppSwitchBlocked);
        app_status.AppSwitchBlocked = 0;
        ReturnValue = 0;
        return ReturnValue;
    }

    /**Check card insert block */
    {
        int i = 0;
        for (i = 0; i< CARD_NUM; i++) {
            if (AppLibCard_StatusCheckBlock(i) == 0) {
                if ((i < AppLibCard_GetActiveCardId()) && (AppLibCard_GetActiveCardId()!= -1)) {
                    APP_APP_s *CurApp;
                    AppAppMgt_GetCurApp(&CurApp);
                    CurApp->OnMessage(AMSG_CMD_CARD_UPDATE_ACTIVE_CARD, i, 0);
                } else {
                    AppLibCard_StatusSetBlock(i, 0);
                    AppLibComSvcAsyncOp_CardInsert(AppLibCard_GetSlot(i));
                }
                ReturnValue = 0;
                return ReturnValue;
            }
        }
    }

    return ReturnValue;
}

/**
 *  @brief To excute the functions that system block them when the Busy flag is enabled.
 *
 *  To excute the functions that system block them when the Busy flag is enabled.
 *
 *  @param [in] param Parameter
 *
 *  @return >=0 success, <0 failure
 */
int AppUtil_BusyCheck(UINT32 param)
{
    int ReturnValue = 0;

    /* Check unsaved movie status */
    if (app_status.MvRecover == 1) {
        AppUtil_SwitchApp(APP_MISC_MVRECOVER);
        ReturnValue = 0;
        return ReturnValue;
    }

    /* Check firmware existence */
    if (app_status.FwUpdate == 1) {
        AppUtil_SwitchApp(APP_MISC_FWUPDATE);
        ReturnValue = 0;
        return ReturnValue;
    }

    /* Check Calibration Script */
    if (app_status.CalibRework == 1) {
        AppUtil_SwitchApp(APP_MISC_CALIBUPDATE);
        ReturnValue = 0;
        return ReturnValue;
    }

    /* Check card format parameter */
    if ((app_status.CardFmtParam == APP_CARD_FMT_NONOPTIMUM)) {
        AppUtil_SwitchApp(APP_MISC_FORMATCARD);
        ReturnValue = 0;
        return ReturnValue;
    }

    /* Check USB cable */
    if ((app_status.UsbPluginFlag == 1) &&
        (AppUtil_UsbChargeCheckingGetStatus() == 0) &&
        (app_status.UsbChargeMode == 0) &&
        (AmbaUser_CheckIsUsbSlave())) {
        APP_APP_s *CurApp;
        AppAppMgt_GetCurApp(&CurApp);
        CurApp->OnMessage(AMSG_CMD_USB_APP_START, 0, 0);
    }

#ifdef CONFIG_APP_ARD
    if ((app_status.UsbPluginFlag == 1) && (app_status.UsbTreatAsDC == 1)){
    AppLibComSvcHcmgr_SendMsg(HMSG_USB_DETECT_CONNECT, 0, 0);
    app_status.UsbTreatAsDC = 0;
    app_status.UsbPluginFlag = 0;
    app_status.UsbChargeMode = 0;
    AmbaPrintColor(GREEN,"BusyCheck usb");
    }
#endif

    /* Enable the fchan device after finishing the mode switch */
    if (app_status.LockDecMode == 1) {
        ReturnValue =  AppLibDisp_GetDeviceID(DISP_CH_FCHAN);
        if (ReturnValue != AMP_DSIP_NONE) {
            AppLibDisp_SelectDevice(DISP_CH_FCHAN, DISP_ANY_DEV);
            AppLibDisp_ConfigMode(DISP_CH_FCHAN, AppUtil_GetVoutMode(0));
            AppLibDisp_SetupChan(DISP_CH_FCHAN);
            AppLibDisp_ChanStart(DISP_CH_FCHAN);
        }
    }

    /**Check vout */
    if ((app_status.HdmiPluginFlag == 1) &&
        (app_status.HdmiPluginFlag != AppLibSysVout_CheckJackHDMI())) {
        APP_APP_s *CurApp;
        AppAppMgt_GetCurApp(&CurApp);
        CurApp->OnMessage(HMSG_HDMI_INSERT_SET, 0, 0);
    } else if ((app_status.HdmiPluginFlag == 0) &&
        (app_status.HdmiPluginFlag != AppLibSysVout_CheckJackHDMI())) {
        APP_APP_s *CurApp;
        AppAppMgt_GetCurApp(&CurApp);
        CurApp->OnMessage(HMSG_HDMI_INSERT_CLR, 0, 0);
    }
    if ((app_status.CompositePluginFlag == 1) &&
        (app_status.CompositePluginFlag != AppLibSysVout_CheckJackCs())) {
        APP_APP_s *CurApp;
        AppAppMgt_GetCurApp(&CurApp);
        CurApp->OnMessage(HMSG_CS_INSERT_SET, 0, 0);
    } else if ((app_status.CompositePluginFlag == 0) &&
        (app_status.CompositePluginFlag != AppLibSysVout_CheckJackCs())) {
        APP_APP_s *CurApp;
        AppAppMgt_GetCurApp(&CurApp);
        CurApp->OnMessage(HMSG_CS_INSERT_CLR, 0, 0);
    }
#if 0
    /**Check audio input */
    if ((app_status.LineinPluginFlag == 1) &&
         (app_status.LineinPluginFlag != app_audio_check_jack_linein())) {
        appmgt_get_curapp(&CurApp);
        CurApp->OnMessage(HMSG_LINEIN_IN_SET, 0, 0);
    } else if ((app_status.LineinPluginFlag == 0) &&
         (app_status.LineinPluginFlag != app_audio_check_jack_linein())) {
        appmgt_get_curapp(&CurApp);
        CurApp->OnMessage(HMSG_LINEIN_IN_CLR, 0, 0);
    }

    /**Check audio output */
    if ((app_status.LineoutPluginFlag == 1) &&
         (app_status.LineoutPluginFlag != app_audio_check_jack_lineout())) {
             appmgt_get_curapp(&CurApp);
        CurApp->OnMessage(HMSG_LINEOUT_IN_SET, 0, 0);
    } else if ((app_status.LineoutPluginFlag == 0) &&
         (app_status.LineoutPluginFlag != app_audio_check_jack_lineout())) {
             appmgt_get_curapp(&CurApp);
        CurApp->OnMessage(HMSG_LINEOUT_IN_CLR, 0, 0);
    }
    if ((app_status.hp_plugin_flag == 1) &&
         (app_status.hp_plugin_flag != app_audio_check_jack_hp())) {
        appmgt_get_curapp(&CurApp);
        CurApp->OnMessage(HMSG_HP_IN_SET, 0, 0);
    } else if ((app_status.hp_plugin_flag == 0) &&
         (app_status.hp_plugin_flag != app_audio_check_jack_hp())) {
        appmgt_get_curapp(&CurApp);
        CurApp->OnMessage(HMSG_HP_IN_CLR, 0, 0);
    }
#endif
    /**Check app switch*/
    if (app_status.AppSwitchBlocked > 0) {
        AppUtil_SwitchApp(app_status.AppSwitchBlocked);
        app_status.AppSwitchBlocked = 0;
        ReturnValue = 0;
        return ReturnValue;
    }

    /**Check card insert block */
    {
        int i = 0;
        for (i = 0; i< CARD_NUM; i++) {
            if (AppLibCard_StatusCheckBlock(i) == 0) {
                if ((i < AppLibCard_GetActiveCardId()) && (AppLibCard_GetActiveCardId()!= -1)) {
                    APP_APP_s *CurApp;
                    AppAppMgt_GetCurApp(&CurApp);
                    CurApp->OnMessage(AMSG_CMD_CARD_UPDATE_ACTIVE_CARD, i, 0);
                } else {
                    AppLibCard_StatusSetBlock(i, 0);
                    AppLibComSvcAsyncOp_CardInsert(AppLibCard_GetSlot(i));
                }
                ReturnValue = 0;
                return ReturnValue;
            }
        }
    }

    return ReturnValue;
}


/**
 *  @brief Switch the applicaton.
 *
 *  Switch the applicaton.
 *
 *  @param [in] appId Application Index
 *
 *  @return >=0 success, <0 failure
 */
int AppUtil_SwitchApp(int appId)
{
    int ReturnValue = 0;
    APP_APP_s *CurApp;
    APP_APP_s *NewApp;

    if (appId < 0) {
        AmbaPrintColor(RED, "[App Util] <SwitchApp> This app is not registered");
        return -1;
    }

    AppAppMgt_GetCurApp(&CurApp);
    AppAppMgt_GetApp(appId, &NewApp);

    if (AppAppMgt_CheckIdle() || // app is idle
        (CurApp->Tier < NewApp->Tier) || // new app is descendant
        APP_CHECKFLAGS(NewApp->GFlags, APP_AFLAGS_READY)) { // new app is ancestor
        AppAppMgt_SwitchApp(appId);
        if (CurApp->Tier >= NewApp->Tier) {
            AppPref_Save();
        }
    } else {
        AmbaPrint("[App Util] <SwitchApp> App switch target appId = %d is blocked", appId);
        app_status.AppSwitchBlocked = appId;
        ReturnValue = AppAppMgt_CheckBusy();
        if (ReturnValue) {
            AmbaPrint("[App Util] <SwitchApp> App switch is blocked by busy appId = %d", ReturnValue);
            CurApp->OnMessage(AMSG_CMD_SWITCH_APP, appId, 0);
        }
    }
    return ReturnValue;
}


/**
 *  @brief Switch application
 *
 *  Switch application
 *
 *  @param [in] param
 *
 *  @return >=0 success, <0 failure
 */
int AppUtil_SwitchMode (UINT32 param)
{
    int ReturnValue = 0;

    if (param != 0) {
        AmbaPrintColor (BLUE, "[App Util] <SwitchMode> Mode switch to %d ", param);
        AppUtil_SwitchApp (param);
    } 
    else
    {
        if (app_status.Type == APP_TYPE_DV) {
            if (UserSetting->SystemPref.SystemMode == APP_MODE_DEC) {
#ifdef CONFIG_APP_ARD
                app_status.CurrEncMode = APP_VIDEO_ENC_MODE;
#endif          
                AppUtil_SwitchApp(APP_REC_CAM); /* CarDV REC */
            } else {
#ifndef CONFIG_ECL_GUI
                {
                    /* Send the message to the current app. */
                    APP_APP_s *curapp;
                    AppAppMgt_GetCurApp (&curapp);
                    if (app_status.CurrEncMode == APP_VIDEO_ENC_MODE) 
                    {
                        curapp->OnMessage (AMSG_CMD_SET_RECORD_MODE, 1, 0);
                    } 
                    else 
                    {
                        if (curapp->Id == APP_REC_CAM)
                        {
                            app_status.thumb_motion_tab_switched = THUMB_MOTION_TAB_SWITCHED_NO;
                        }
                        AppUtil_SwitchApp(APP_THUMB_MOTION);
                    }
                }
#else
                AppUtil_SwitchApp (APP_THUMB_MOTION); //switch to payback
#endif
            }
        }
    }
    return ReturnValue;
}

/**
 *  @brief Get the first application.
 *
 *  Get the first application.
 *
 *  @param [in] param
 *
 *  @return >=0 success, <0 failure
 */
int AppUtil_GetStartApp(UINT32 param)
{
    int ReturnValue = 0;

    if (app_status.Type == APP_TYPE_DV) {
        if (UserSetting->SystemPref.SystemMode == APP_MODE_DEC) {
            ReturnValue = APP_THUMB_MOTION;
        } else {
            ReturnValue = APP_REC_CAM;/* CarDV REC */
        }
    }
    return ReturnValue;
}

char FirmwareName[64] = {'A','m','b','a','S','y','s','F','W','.','b','i','n','\0'};

/**
 *  @brief Check that the SD card have the firmware updated file.
 *
 *  Check that the SD card have the firmware updated file.
 *
 *  @param [in] drive Storage drive id
 *
 *  @return >=0 success, <0 failure
 */
static int AppUtil_CheckCardFw(char drive)
{
    char Firmware[64] = {0};
    AMBA_FS_STAT Fstat;
    int ReturnValue = -1;

    Firmware[0] = (char)drive;
    Firmware[1] = ':';
    Firmware[2] = '\\';
    strcat(Firmware, FirmwareName);
    ReturnValue = AmbaFS_Stat((const char *)Firmware, &Fstat);
    if ((ReturnValue == 0) && (Fstat.Size > 0)) {
        app_status.FwUpdate = 1;
#ifdef CONFIG_APP_ARD
        if (AppAppMgt_CheckIdle() || (rec_cam.RecCapState == REC_CAP_STATE_VF)) {
#else
        if (AppAppMgt_CheckIdle()) {
#endif
            if (app_status.MvRecover == 0) {
                ReturnValue = AppUtil_SwitchApp(APP_MISC_FWUPDATE);
            }
        }
    } else {
        /* when connect USB, insert SD with firmware.bin and remove firmware.bin in PC */
        app_status.FwUpdate = 0;
        ReturnValue = -1;
    }

    return ReturnValue;
}

char CalibScript[64] = CALIB_SCRIPT;
/**
 *  @brief Check that the SD card have the calibration script file.
 *
 *  Check that the SD card have the calibration script file.
 *
 *  @param [in] drive Storage drive id
 *
 *  @return >=0 success, <0 failure
 */
static int AppUtil_CheckCardScript(char drive)
{
    char Script[64] = {0};
    AMBA_FS_STAT Fstat;
    int ReturnValue = -1;

    Script[0] = (char)drive;
    Script[1] = ':';
    Script[2] = '\\';
    strcat(Script, CalibScript);

    //strcat(Script, CalibScript);
   // w_asc2uni(WScript, Script, strlen(WScript));
    ReturnValue = AmbaFS_Stat((const char *)Script, &Fstat);
    if ((ReturnValue == 0) && (Fstat.Size > 0)) {
        app_status.CalibRework = 1;
        if (AppAppMgt_CheckIdle()) {
            if (app_status.MvRecover == 0 && app_status.FwUpdate == 0) {
                ReturnValue = AppUtil_SwitchApp(APP_MISC_CALIBUPDATE);
            }
        }
    } else {
        /* when connect USB, insert SD with firmware.bin and remove firmware.bin in PC */
        app_status.CalibRework = 0;
        ReturnValue = -1;
    }

    return ReturnValue;
}

/**
 *  @brief Check card parameter
 *
 *  Check card parameter
 *
 *  @param [in] param Parameter
 *
 *  @return >=0 success, <0 failure
 */
int AppUtil_CheckCardParam(UINT32 param)
{
    int ReturnValue = -1;
    int CardId = 0;
    int Slot = 0;
    char Drive = 'A';
    APP_APP_s *CurApp;

    /* To check the movie recovery data. */
#ifdef CONFIG_APP_ARD
    if ((UserSetting->VideoPref.UnsavingData != 0) || (UserSetting->VideoPref.UnsavingEvent != 0)) {
#else
    if (UserSetting->VideoPref.UnsavingData != 0) {
        UserSetting->VideoPref.UnsavingData = 0;
#endif
        app_status.MvRecover = 1;
        if (AppAppMgt_CheckIdle()) {
            ReturnValue = AppUtil_SwitchApp(APP_MISC_MVRECOVER);
        } else {
            /*If status busy send msg to app to stop VF to do movie recover*/
            //APP_APP_s *CurApp;
            AppAppMgt_GetCurApp(&CurApp);
            CurApp->OnMessage(AMSG_CMD_CARD_MOVIE_RECOVER, 0, 0);
        }
    }

    CardId = AppLibCard_GetActiveCardId();
    Slot = AppLibCard_GetActiveSlot();
    Drive = AppLibCard_GetActiveDrive();
    /* to check firmware update file & Calib script*/
    if ((CardId == CARD_SD0) || (CardId == CARD_SD1)) {
        AppUtil_CheckCardFw(Drive);
        AppUtil_CheckCardScript(Drive);
    }

    if ((Slot == SCM_SLOT_SD0) || (Slot == SCM_SLOT_SD1)) {
        /* do format check only on SD Card */
        ReturnValue = AppLibCard_CheckFormatParam(Slot, Drive);
        if (ReturnValue == -1) {
            app_status.CardFmtParam = APP_CARD_FMT_NONOPTIMUM;
            if (AppAppMgt_CheckIdle()) {
                if ((app_status.MvRecover == 0) && (app_status.FwUpdate == 0)) {
                    ReturnValue = AppLibCard_CheckStatus(CARD_CHECK_DELETE);
                    if (ReturnValue == CARD_STATUS_NO_CARD) {
                        AmbaPrintColor(RED,"[App Util] <AppUtil_CheckCardParam> WARNING_NO_CARD");
                    } else if (ReturnValue == CARD_STATUS_WP_CARD) {
                        AmbaPrintColor(RED,"[App Util] <AppUtil_CheckCardParam> WARNING_CARD_PROTECTED");
                    } else {
                        ReturnValue = AppUtil_SwitchApp(APP_MISC_FORMATCARD);
                    }
                }
            }
             //APP_APP_s *CurApp;
             AppAppMgt_GetCurApp(&CurApp);
             CurApp->OnMessage(AMSG_CMD_CARD_FMT_NONOPTIMUM, 0, 0);
        }
    }

    return ReturnValue;
}


/**
 *  @brief Get the vout mode.
 *
 *  Get the vout mode.
 *
 *  @param [in] param Parameter
 *
 *  @return >=0 success, <0 failure
 */
int AppUtil_GetVoutMode(int param)
{
    int ReturnValue = 0;
    if (app_status.FixedVoutMode == VOUT_DISP_MODE_AUTO) {
        AmbaPrint("[App Util] <SwitchApp> Auto mode is not implemented. Use default display mode");
        ReturnValue = AppLibSysVout_GetVoutMode(VOUT_DISP_MODE_1080P);
    } else {
        ReturnValue = AppLibSysVout_GetVoutMode((VOUT_DISP_MODE_ID_e) app_status.FixedVoutMode);
    }
    return ReturnValue;
}

#ifdef CONFIG_APP_ARD
int AppUtil_SystemIsEncMode(void)
{
    if(UserSetting->SystemPref.SystemMode == APP_MODE_ENC){
        return 1;
    }else{
        return 0;
    }
}
#endif

/**
 *  @brief Initialize the app utility
 *
 *  Initialize the app utility
 *
 *  @return >=0 success, <0 failure
 */
int AppUtil_Init(void)
{
    return 0;
}


/**
 *  @brief Initialize the status
 *
 *  Initialize the status
 *
 *  @return >=0 success, <0 failure
 */
int AppUtil_StatusInit(void)
{
    int ReturnValue = 0;

    /** Correct RTC time */
    {
        AMBA_RTC_TIME_SPEC_u TimeSpec = {0};
        AmbaRTC_GetSystemTime(AMBA_TIME_STD_TAI, &TimeSpec);
        if (ReturnValue < 0) {
            AmbaPrintColor(RED, "[App Util] AmbaRTC_GetSystemTime failure. ReturnValue = %d", ReturnValue);
        }
#if 0
        AmbaPrint("TimeSpec.Calendar.Year = %d",TimeSpec.Calendar.Year);
        AmbaPrint("TimeSpec.Calendar.Month = %d",TimeSpec.Calendar.Month);
        AmbaPrint("TimeSpec.Calendar.DayOfMonth = %d",TimeSpec.Calendar.DayOfMonth);
        AmbaPrint("TimeSpec.Calendar.DayOfWeek = %d",TimeSpec.Calendar.DayOfWeek);
        AmbaPrint("TimeSpec.Calendar.Hour = %d",TimeSpec.Calendar.Hour);
        AmbaPrint("TimeSpec.Calendar.Minute = %d",TimeSpec.Calendar.Minute);
        AmbaPrint("TimeSpec.Calendar.Second = %d",TimeSpec.Calendar.Second);
        AmbaPrint("AmbaRTC_IsDevValid() = %d",AmbaRTC_IsDevValid());
#endif
        if ((TimeSpec.Calendar.Year > 2037) || (TimeSpec.Calendar.Year < 2016)) {
            TimeSpec.Calendar.Year = 2016;
            TimeSpec.Calendar.Month = 1;
            TimeSpec.Calendar.Day = 1;
            TimeSpec.Calendar.DayOfWeek = WEEK_WEDNESDAY;
            TimeSpec.Calendar.Hour = 0;
            TimeSpec.Calendar.Minute = 0;
            TimeSpec.Calendar.Second = 0;
            ReturnValue =  AmbaRTC_SetSystemTime(AMBA_TIME_STD_TAI, &TimeSpec);
            if (ReturnValue < 0) {
                AmbaPrintColor(RED, "[App Util] AmbaRTC_SetSystemTime failure. ReturnValue = %d", ReturnValue);
            }
        }
    }

    /** Init status */
    AppLibSysVin_SetSystemType(UserSetting->SetupPref.VinSystem);
    AppLibSysVout_SetSystemType(UserSetting->SetupPref.VoutSystem);
    
    AppLibAudioDec_BeepVolume(UserSetting->AudioPref.AudioVolume);
      
   
    /** Init video settings */
    UserSetting->VideoPref.SensorVideoRes=SENSOR_VIDEO_RES_TRUE_1080P_HALF;
    UserSetting->VideoPref.SensorVideoRes = AppLibSysSensor_CheckVideoRes(UserSetting->VideoPref.SensorVideoRes);
    AppLibVideoEnc_SetSensorVideoRes(UserSetting->VideoPref.SensorVideoRes);
    AppLibVideoEnc_SetQuality(UserSetting->VideoPref.VideoQuality);
    AppLibVideoEnc_SetPreRecord(UserSetting->VideoPref.PreRecord);
    AppLibVideoEnc_SetTimeLapse(UserSetting->VideoPref.TimeLapse);
    AppLibVideoEnc_SetDualStreams(UserSetting->VideoPref.DualStreams);
    AppLibVideoEnc_SetSplit(VIDEO_SPLIT_SIZE_AUTO | VIDEO_SPLIT_TIME_AUTO);
    AppLibVideoEnc_SetRecMode(REC_MODE_AV);

    /** Init photo settings */
    AppLibStillEnc_SetMultiCapMode(UserSetting->PhotoPref.PhotoMultiCap);
    AppLibStillEnc_SetNormCapMode(UserSetting->PhotoPref.PhotoCapMode);
    AppLibStillEnc_SetSizeID(UserSetting->PhotoPref.PhotoSize);
    AppLibStillEnc_SetQualityMode(UserSetting->PhotoPref.PhotoQuality);
    AppLibStillEnc_SetQuickview(PHOTO_QUICKVIEW_DUAL);
    AppLibStillEnc_SetQuickviewDelay(UserSetting->PhotoPref.QuickviewDelay);

    /** Init sound settings */
    AppLibAudioEnc_SetEncType(AUDIO_TYPE_AAC);
    return ReturnValue;
}


static int storage_slot_array[] = {
    SCM_SLOT_SD0,/**< CARD_SD0 */
    SCM_SLOT_SD1,/**< CARD_SD1 */
    SCM_SLOT_FL0,/**< CARD_NAND0 */
    SCM_SLOT_FL1,/**< CARD_NAND1 */
    SCM_SLOT_RD,/**< CARD_RD*/
    -1
};


/**
 *  @brief Polling all card slots
 *
 *  Polling all card slots
 *
 *  @return >=0 success, <0 failure
 */
int AppUtil_PollingAllSlots(void)
{
    int ReturnValue = 0;
    int i = 0;

    while (storage_slot_array[i] >= 0) {
        ReturnValue = AppLibCard_Polling(AppLibCard_GetCardId(storage_slot_array[i++]));
    }
    return ReturnValue;
}


/**
 *  @brief Convert the strings from ASCII to Unicode.
 *
 *  Convert the strings from ASCII to Unicode.
 *
 *  @param [in] ascStr strings of ASCII format
 *  @param [out] uniStr strings of Unicode format
 *
 *  @return >=0 success, <0 failure
 */
int AppUtil_AsciiToUnicode(char *ascStr, UINT16 *uniStr)
{
    int i, len;

    len = strlen(ascStr);
    for (i=0; i<len; i++) {
        uniStr[i] = ascStr[i];
    }
    uniStr[len] = 0;

    return len;
}

#define USB_CHARGE_CHECK_STAGE1_TIMEOUT    (15)
#define USB_CHARGE_CHECK_STAGE2_TIMEOUT    (50)
/**
 *  @brief The second stage handler of USB charger
 *
 *  The second stage handler of USB charger
 *
 *  @param [in] eid Event id
 *
 *  @return >=0 success, <0 failure
 */
static void AppUtil_UsbChargeCheckStage2Handler(int eid)
{
    static int cnt = USB_CHARGE_CHECK_STAGE2_TIMEOUT;
    APP_APP_s *CurApp;
    if (eid == TIMER_UNREGISTER) {
        cnt = USB_CHARGE_CHECK_STAGE2_TIMEOUT;
        return;
    }

    cnt--;

    if (AmbaUSB_System_IsConfigured()) {// Connected to PC

#if 0
        if (AppLibUSB_GetCurClass() != APPLIB_USB_CLASS_NONE) {
            AppLibUSB_ClassInit(APPLIB_USB_CLASS_NONE);
        }
#endif
        AppLibComSvcTimer_Unregister(TIMER_1HZ, AppUtil_UsbChargeCheckStage2Handler);
        usb_charge_check_running = 0;

        AppAppMgt_GetCurApp(&CurApp);
        CurApp->OnMessage(AMSG_CMD_USB_APP_START, 0, 0);
    } else if (cnt == 0) {// Time out
        AppAppMgt_GetCurApp(&CurApp);
        CurApp->OnMessage(AMSG_CMD_USB_APP_STOP, 0, 0);

#if 0
        if (AppLibUSB_GetCurClass() != APPLIB_USB_CLASS_NONE) {
            AppLibUSB_ClassInit(APPLIB_USB_CLASS_NONE);
        }
#endif
        AppLibComSvcTimer_Unregister(TIMER_1HZ, AppUtil_UsbChargeCheckStage2Handler);
        usb_charge_check_running = 0;
    ApplibUsbMsc_Stop();
        app_status.UsbChargeMode = 1;
    }
}


/**
 *  @brief The first stage handler of USB charger
 *
 *  The first stage handler of USB charger
 *
 *  @param [in] eid
 *
 *  @return >=0 success, <0 failure
 */
static void AppUtil_UsbChargeCheckStage1Handler(int eid)
{
    static int cnt = USB_CHARGE_CHECK_STAGE1_TIMEOUT;

    if (eid == TIMER_UNREGISTER) {
        cnt = USB_CHARGE_CHECK_STAGE1_TIMEOUT;
        return;
    }

    cnt--;

    if (AmbaUSB_System_IsConfigured()) {    // Connected to PC
        APP_APP_s *CurApp;
#if 0
        if (AppLibUSB_GetCurClass() != APPLIB_USB_CLASS_NONE) {
            AppLibUSB_ClassInit(APPLIB_USB_CLASS_NONE);
        }
#endif
        AppLibComSvcTimer_Unregister(TIMER_10HZ, AppUtil_UsbChargeCheckStage1Handler);
        usb_charge_check_running = 0;

        AppAppMgt_GetCurApp(&CurApp);
        CurApp->OnMessage(AMSG_CMD_USB_APP_START, 0, 0);
    } else if (cnt == 0) {    // Time out
        AppLibComSvcTimer_Unregister(TIMER_10HZ, AppUtil_UsbChargeCheckStage1Handler);

        AppLibComSvcTimer_Register(TIMER_1HZ, AppUtil_UsbChargeCheckStage2Handler);
    }
}

/**
 *  @brief To check the USB charger
 *
 *  To check the USB charger
 *
 *  @param [in] enable Enable flag
 *
 *  @return >=0 success, <0 failure
 */
int AppUtil_UsbChargeCheckingSet(UINT32 enable)
{
     if (enable && !usb_charge_check_running) {
        if (AmbaUSB_System_ChargeDetection()) { // USB charge
            usb_charge_check_running = 0;
            app_status.UsbChargeMode = 1;
        } else {
            ApplibUsbMsc_Start();
            AppLibComSvcTimer_Register(TIMER_10HZ, AppUtil_UsbChargeCheckStage1Handler);
            usb_charge_check_running = 1;
        }
    } else if (!enable ) {
        APP_APP_s *CurApp;

        ApplibUsbMsc_Stop();
        AppLibComSvcTimer_Unregister(TIMER_10HZ, AppUtil_UsbChargeCheckStage1Handler);
        AppLibComSvcTimer_Unregister(TIMER_1HZ, AppUtil_UsbChargeCheckStage2Handler);
        usb_charge_check_running = 0;
        AppAppMgt_GetCurApp(&CurApp);
        CurApp->OnMessage(AMSG_CMD_USB_APP_STOP, 0, 0);
    }

    return 0;
}

/**
 *  @brief The jack handler of composite
 *
 *  The jack handler of composite
 *
 *  @return >=0 success, <0 failure
 */
int AppUtil_JackCompositeHandler(AMBA_GPIO_PIN_ID_e gpioPinID)
{
    int ReturnValue = 0;

#if defined (GPIO_COMPOSITE)
    if (gpioPinID == GPIO_COMPOSITE) {
        AMBA_GPIO_PIN_INFO_s pinInfo;
        AMBA_GPIO_PIN_LEVEL_e level;

        AmbaGPIO_GetPinInfo(gpioPinID, &pinInfo);
        level = pinInfo.Level;

        if (!level) {
            AppLibComSvcHcmgr_SendMsgNoWait(HMSG_CS_INSERT_CLR, 0, 0);
        } else {
            AppLibComSvcHcmgr_SendMsgNoWait(HMSG_CS_INSERT_SET, 0, 0);
        }
    }
#endif

    return ReturnValue;
}


/**
 *  @brief Initialize the jack monitor
 *
 *  Initialize the jack monitor
 *
 *  @return >=0 success, <0 failure
 */
int AppUtil_JackMonitor(void)
{
    int ReturnValue = 0;
    AMBA_GPIO_PIN_INFO_s pinInfo;

#if defined (GPIO_COMPOSITE)
    AmbaGPIO_ConfigInput(GPIO_COMPOSITE);
    AmbaGPIO_IsrHook(GPIO_COMPOSITE, GPIO_INT_BOTH_EDGE_TRIGGER, (AMBA_GPIO_ISR_f)AppUtil_JackCompositeHandler);
    AmbaGPIO_IntEnable(GPIO_COMPOSITE);

    AmbaGPIO_GetPinInfo(GPIO_COMPOSITE, &pinInfo);

    if (pinInfo.Level) {
        AppLibComSvcHcmgr_SendMsg(HMSG_CS_INSERT_SET, 0, 0);
    }

#endif

    return ReturnValue;
}

#ifdef CONFIG_APP_ARD
#define BATTERY_LEVEL 5

typedef struct {
    int min;
    int max;
} battery_range_t;

/*    Vadc= 0.688*Vbat    */
#define ADC_DIV (688)
#define ADC_SOC_VCC (3300)
#define BAT_VOLT(bat_volt)    ((((bat_volt*ADC_DIV)/1000)*AMBA_ADC_RESOLUTION)/ADC_SOC_VCC)
#define ADC_TO_BAT_VOLT(adc_value) ((ADC_SOC_VCC*((adc_value*1000)/ADC_DIV))/AMBA_ADC_RESOLUTION)

#define MIN_BATTERY_ENERGY BAT_VOLT(3649) // ---3.649V--power off
static battery_range_t battery_range_table[BATTERY_LEVEL] = {
    {   0, BAT_VOLT(3650)},    //BATTERY_EMPTY   // ---Below 3.65V
    {BAT_VOLT(3650), BAT_VOLT(3700)}, //BATTERY_0     // ---3.65V~3.7
    {BAT_VOLT(3700), BAT_VOLT(3800)}, //BATTERY_1     // ---3.7V~3.8
    {BAT_VOLT(3800), BAT_VOLT(3900)}, //BATTERY_2     // ---3.8V~3.9
    {BAT_VOLT(3900), BAT_VOLT(5000)}  //BATTERY_3     // ---above 3.9V
};

#define BATT_MONITOR_TIME  (5)
#define DEBUG_BATTERY_ADC_ENABLE  (0)
#define BATTERY_VOLT_DEBUG(v)  AmbaPrint("BAT_VOLT("#v")""=%d",BAT_VOLT(v))
#define LOW_BATTERY_COUNT_INIT      (8)
#define BAT_DEBOUNCE_INIT   (10)
static int low_battery_count = LOW_BATTERY_COUNT_INIT;
static UINT32 battery_count_avg = 0;
static UINT32 volt_sum = 0;
static int bat_debounce = BAT_DEBOUNCE_INIT;
static int beep_cnt = 0;
static int LowVoltageMsg = 0;
static UINT8 PrintVoltageMsg = 0;
static UINT8 power_dc_in_flg=0;
static UINT8 dc_off_time_delay=5;
    
/*check battery voltage level*/
void AppUtil_CheckBatteryVoltageLevel(void)
{
    UINT16 volts = 0;
    int i = 0, status = 0;

    /*Get the ADC data.*/
    /*A12RefButton_AdcScanHandler() has collected*/
    //AmbaADC_DataCollection();
    volts = AmbaAdcData[AMBA_ADC_CHANNEL2];

    for (i=0; i<BATTERY_LEVEL; i++) {
        if (((int)volts>battery_range_table[i].min) && ((int)volts<=battery_range_table[i].max)) {
            status = i;
        }
    }

#if DEBUG_BATTERY_ADC_ENABLE
    AmbaPrint("Vadc=%d(%dV),status=%d,power off Vadc=%d(%dV)",volts,ADC_TO_BAT_VOLT(volts),status,
                                                                MIN_BATTERY_ENERGY,ADC_TO_BAT_VOLT(MIN_BATTERY_ENERGY));
    BATTERY_VOLT_DEBUG(3550);
    BATTERY_VOLT_DEBUG(3600);
    BATTERY_VOLT_DEBUG(3700);
    BATTERY_VOLT_DEBUG(3800);
    BATTERY_VOLT_DEBUG(3900);
#endif

    if (volts <= MIN_BATTERY_ENERGY) {
        app_status.BatteryState = APP_BATTERY_EMPTY;
        /*To do double check the voltage.*/
        battery_count_avg ++;
        volt_sum +=volts;
        if(battery_count_avg > 9){
            battery_count_avg = 9;
            volt_sum = volt_sum *9/10;
        } else {
            return;
        }

        if ((volt_sum/battery_count_avg) < MIN_BATTERY_ENERGY) {
            low_battery_count--;

            AppLibGraph_UpdateString(GRAPH_CH_DCHAN, GOBJ_WARNING, GuiWarningTable[GUI_WARNING_LOW_VOLTAGE].str);
#ifdef CONFIG_APP_ARD
            AppLibGraph_UpdateColor(GRAPH_CH_DUAL, GOBJ_WARNING, COLOR_RED, COLOR_TEXT_BORDER );
#endif
             

            if (low_battery_count == 0) {
                                    /*Don't power off if usb is plugin.*/
                if(AppLibUSB_GetVbusStatus() == 0) {
                          AmbaPrint("Low battery and VbusStatus = 0, shutdown.");
                    AppLibComSvcHcmgr_SendMsg(HMSG_USER_POWER_BUTTON, 0, 0);
                    
                    
                } else {
                  
                    AmbaPrint("Low battery and VbusStatus = 1, blink.");
                    if (low_battery_count %2 == 1) {
                        /*Hide the battery gui */
                        AppLibComSvcHcmgr_SendMsg(AMSG_STATE_BATTERY_STATE, 0, 0);
                        //AppLibGraph_Show(GRAPH_CH_DUAL, GOBJ_LOW_BATTERY);

                        /*warning sound*/
                        //app_beep_play_beep(BEEP_ERROR, 0);
                    } else {
                        /*Show the battery gui*/
                        AppLibComSvcHcmgr_SendMsg(AMSG_STATE_BATTERY_STATE, 1, 0);
                        //AppLibGraph_Hide(GRAPH_CH_DUAL, GOBJ_LOW_BATTERY);
                    }
                }
                low_battery_count = LOW_BATTERY_COUNT_INIT;
            } else {
                if (low_battery_count %2 == 1) {
                    /*Hide the battery gui */
                    AppLibComSvcHcmgr_SendMsg(AMSG_STATE_BATTERY_STATE, 0, 0);

                    if((1==low_battery_count) || (5==low_battery_count)) {
                        if(beep_cnt++ < 2) {
                            if (1 == LowVoltageMsg) {
                                LowVoltageMsg = 0;
                                AppLibGraph_Hide(GRAPH_CH_DCHAN, GOBJ_WARNING);
                                AppLibAudioDec_Beep(BEEP_ERROR,0);
                            }
                        }
                    }
                    //AppLibGraph_Show(GRAPH_CH_DUAL, GOBJ_LOW_BATTERY);
                    /*warning sound*/
                    //app_beep_play_beep(BEEP_ERROR, 0);
                } else {
                    /*Show the battery gui*/
                    AppLibComSvcHcmgr_SendMsg(AMSG_STATE_BATTERY_STATE, 1, 0);

                    LowVoltageMsg = 1;
                    AppLibGraph_Show(GRAPH_CH_DCHAN, GOBJ_WARNING);
                    //AppLibGraph_Hide(GRAPH_CH_DUAL, GOBJ_LOW_BATTERY);
                }
            }
        }
    } else {
        low_battery_count ++;
        if(low_battery_count > LOW_BATTERY_COUNT_INIT){
            low_battery_count = LOW_BATTERY_COUNT_INIT;
            battery_count_avg = 0;
            volt_sum = 0;
        }
        if (low_battery_count != LOW_BATTERY_COUNT_INIT){
            /*Low battery state.*/
            if (low_battery_count %2 == 1) {
                /*Hide the battery gui */
                AppLibComSvcHcmgr_SendMsg(AMSG_STATE_BATTERY_STATE, 0, 0);
                //AppLibGraph_Show(GRAPH_CH_DUAL, GOBJ_LOW_BATTERY);
            } else {
                /*Show the battery gui*/
                AppLibComSvcHcmgr_SendMsg(AMSG_STATE_BATTERY_STATE, 1, 0);
                //AppLibGraph_Hide(GRAPH_CH_DUAL, GOBJ_LOW_BATTERY);
            }
        } else {
            if (volts == 0) {
                app_status.BatteryState = APP_BATTERY_NONE;
            } else if (status == 0) {
                app_status.BatteryState = APP_BATTERY_EMPTY;
            } else if (status == 1) {
                app_status.BatteryState = APP_BATTERY_0;
            } else if (status == 2) {
                app_status.BatteryState = APP_BATTERY_1;
            } else if (status == 3) {
                app_status.BatteryState = APP_BATTERY_2;
            } else if (status == 4) {
                app_status.BatteryState = APP_BATTERY_3;
            }
        }
    }
}

void AppUtil_BatteryVoltagePrint(void)
{
        UINT16 volts = 0;
        volts = AmbaAdcData[AMBA_ADC_CHANNEL2];
        // AmbaPrint("ADC=%d(%dv)",volts,ADC_TO_BAT_VOLT(volts));
}

static void AppUtil_BatteryVoltageTimerHandler(int eid)
{
    if (eid == TIMER_UNREGISTER) {
        return;
    }

    PrintVoltageMsg++;
    if(PrintVoltageMsg >=BATT_MONITOR_TIME){
        AppUtil_BatteryVoltagePrint();
        PrintVoltageMsg = 0;
    }
   if(AppLibUSB_GetVbusStatus() ==1)
    {
        if( power_dc_in_flg==0) 
            {
                power_dc_in_flg=1;
                dc_off_time_delay=2;
            }
     }
     else if(power_dc_in_flg== 1)
     {
        if(dc_off_time_delay--==0)
        {    
            power_dc_in_flg=0;
            AppLibComSvcHcmgr_SendMsg(HMSG_USER_POWER_BUTTON, 0, 0);
            // AppUtil_SwitchApp(APP_MISC_LOGO);
        } 
    }

    if(app_status.PowerType == APP_POWER_BATTERY) {
    AppUtil_CheckBatteryVoltageLevel();
    bat_debounce--;
    if (bat_debounce == 0) {
        AppLibComSvcHcmgr_SendMsg(AMSG_STATE_BATTERY_STATE, 1, 0);//Update gui
        bat_debounce = BAT_DEBOUNCE_INIT;
    }
     }else{
#if DEBUG_BATTERY_ADC_ENABLE
        AppUtil_CheckBatteryVoltageLevel();
#endif
        if (1 == LowVoltageMsg) {
            LowVoltageMsg = 0;
            AppLibGraph_Hide(GRAPH_CH_DCHAN, GOBJ_WARNING);
            AppLibGraph_Draw(GRAPH_CH_DUAL);
        }

        bat_debounce = BAT_DEBOUNCE_INIT;
        battery_count_avg = 0;
        volt_sum = 0;
        low_battery_count = LOW_BATTERY_COUNT_INIT;
     }
}

void AppUtil_BatteryVoltageMonitor(void)
{
    AppLibComSvcTimer_Register(TIMER_1HZ, AppUtil_BatteryVoltageTimerHandler);
}

extern int AmbaUserGps_Detect(void);
static void AppUtil_Gps_detect_TimerHandler(int eid)
{
    int gps_connected_status;
    static gps_data_t* GpsData = NULL;
    int Speed, Status;

     GpsData = AppLibSysGps_GetData();
     Speed = GpsData->fix.speed*MPS_TO_KMPH;
     Status = GpsData->status;
    if (eid == TIMER_UNREGISTER) {
        return;
    }
    gps_connected_status = AmbaUserGps_Detect();
    if(gps_connected_status == 1)
        app_status.gps_status = GPS_DISCONNECT;
    else{
        if (Status == 0) {
        app_status.gps_status = GPS_NO_SIGNAL;
        }else{
            if(GpsData->satellites_visible<3)
                app_status.gps_status = GPS_NO_SIGNAL;
            else if(GpsData->satellites_visible<6)
                app_status.gps_status = GPS_SIGNAL_1;
            else if(GpsData->satellites_visible<9)
                app_status.gps_status = GPS_SIGNAL_2;
            else if(GpsData->satellites_visible<12)
                app_status.gps_status = GPS_SIGNAL_3;
            else
                app_status.gps_status = GPS_SIGNAL_4;
        }
    }
}
void AppUtil_Gps_Detect_Monitor(void)
{
    AppLibComSvcTimer_Register(TIMER_1HZ, AppUtil_Gps_detect_TimerHandler);
}

#endif
#ifdef CONFIG_APP_ARD
/**
 *  @brief power off handler
 *
 *  The power off handler
 *
 *  @return >=0 success, <0 failure
 */
void AppUtil_PowerOffHandler(int eid)
{
    APP_APP_s *CurApp;

    if (eid == TIMER_UNREGISTER) {
        return;
    }


    AppAppMgt_GetCurApp(&CurApp);
    if ((APP_CHECKFLAGS(CurApp->GFlags, APP_AFLAGS_BUSY)) || (UserSetting->SetupPref.AutoPoweroff == AUTO_POWEROFF_OFF)) {
        if (rec_cam.RecCapState != REC_CAP_STATE_VF) {
            power_off_reset_flag = 1;
        }
    }

    if(CurApp->Id == APP_USB_MSC) {
        power_off_reset_flag = 1;
    }

    if (power_off_reset_flag) {
        power_off_time = UserSetting->SetupPref.AutoPoweroff;
        power_off_reset_flag = 0;
    } else {
        power_off_time--;
        if(power_off_time == 0) {
            AppLibComSvcTimer_Unregister(TIMER_1HZ, AppUtil_PowerOffHandler);
            AppUtil_SwitchApp(APP_MISC_LOGO);
        }
    }
}

/**
 *  @brief initialize power off handler
 *
 *  Initialize the power off handler
 *
 *  @return >=0 success, <0 failure
 */
int AppUtil_AutoPowerOffInit(int param)
{
    int rval = 0;

    power_off_reset_flag = 0;
    AppLibComSvcTimer_Unregister(TIMER_1HZ, AppUtil_PowerOffHandler);
    if (UserSetting->SetupPref.AutoPoweroff != AUTO_POWEROFF_OFF) {
        power_off_reset_flag = 1;
        AppLibComSvcTimer_Register(TIMER_1HZ, AppUtil_PowerOffHandler);
    }

    return rval;
}

/*For test mode*/
int AppUtil_CheckSystemTestModeAutoPowerOff(void)
{
    if((UserSetting->SetupPref.test_mode == TEST_MODE_AUTO_POWER_OFF)
    ||(UserSetting->SetupPref.test_mode == TEST_MODE_AUTO_REBOOT)){
        return 1;
    }else{
        return 0;
    }
}

int AppUtil_CheckSystemTestModeLoopRec(void)
{
    if(UserSetting->SetupPref.test_mode == TEST_MODE_LOOP_REC ) {
        return 1;
    }else{
        return 0;
    }
}

int AppUtil_SystemTestModePowerOff(void)
{
    return AppLibComSvcHcmgr_SendMsg(HMSG_USER_POWER_BUTTON, 0, 0);
}

int AppUtil_GetSystemTestMode(void)
{
    return (int)(UserSetting->SetupPref.test_mode);
}

int AppUtil_SystemTestModeStartRecording(void)
{
    return AppLibComSvcHcmgr_SendMsg(HMSG_USER_RECORD_BUTTON, 0, 0);
}

int AppUtil_SystemTestModeReboot(void)
{
    if(UserSetting->SetupPref.test_mode == TEST_MODE_AUTO_REBOOT){
#if 0
        /*WDT reboot*/
        AMBA_WDT_CONFIG_s WdtConfig = {
            .Mode = AMBA_WDT_MODE_SYS_RESET,    /* WDT operation mode */
            .ResetIrqPulseWidth = 10,           /* The Pulse Width of Reset and Interrupt */
            .CounterValue = 50,         /* WDT Initial counter value (in ms) */
            .TimeoutISR = NULL,                 /* WDT Timeout (underflow) ISR for Interrupt mode */
        };
        AmbaWDT_Start(&WdtConfig);
        return 1;
#else
        AmbaSysSoftReset();
        while(1){
            AmbaKAL_TaskSleep(50);
        }
        return 1;
#endif
    }
    return 0;
}

/////////////////////////////////////////
#include "AmbaRTSL_NAND.h"
#include "AmbaNAND.h"
#include "AmbaNandBadBlockTable.h"
#include "AmbaNFTL.h"
#include "AmbaNAND_PartitionTable.h"

#define CrcBufLen (0x1000)
static UINT8 CrcBuf[CrcBufLen];
static AMBA_NAND_PART_TABLE_s _AmbaNAND_PartTable ;
static AMBA_NAND_PART_META_s _AmbaNAND_MetaData;
extern const char *AmbaNAND_PartitionName[AMBA_NUM_NAND_PARTITION + 1];
extern AMBA_NAND_NFTL_PART_s AmbaNAND_NftlPartInfo;
extern AMBA_NAND_FW_PART_s   AmbaNAND_FwPartInfo;
static AMBA_NAND_NFTL_PART_s _AmbaNAND_NftlPartOld;
static AMBA_NAND_FW_PART_s   _AmbaNAND_FwPartOld;

extern void AmbaNAND_UpdateNftlPartition(AMBA_NAND_DEV_s *pDev);
extern int AmbaFwUpdaterSD_ClearMagicCode(void);

#define ENABLE_UPDATE_MEDIA_PARTITIONS
#define IMAGE_BUF_RESERVE_SIZE 2048

#ifdef ENABLE_UPDATE_MEDIA_PARTITIONS
static char *MediaPartString[] = {
    [MP_Storage0] = "Storage0",
    [MP_Storage1] = "Storage1",
    [MP_IndexForVideoRecording] = "IndexForVideoRecording",
    [MP_UserSetting] = "UserSetting",
    [MP_CalibrationData] = "CalibrationData"
};

static UINT32 FwUpdater_GetContinousNandBlocks(UINT32 NumBlock, UINT32 StartBlock, UINT8 ArrangeOrder)
{
    UINT32 Block;
    int i;

    for (i = 0 ; i < NumBlock; i ++) {
        /* either ascending or descending block number order */
        if (ArrangeOrder == 0)
            Block = StartBlock + i;
        else
            Block = StartBlock - i;

        /* we only care about inital bad block */
        if (AmbaNandBBT_IsBadBlock(Block) != NAND_INIT_BAD_BLOCK)
            continue;

        NumBlock ++;
    }

    return NumBlock;
}
#endif

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: FwUpdater_GetFwImageCRC32
 *
 *  @Description:: Calculate CRC32 checksum of a image
 *
 *  @Input      ::
 *      pFile:  file pointer to firmware
 *      DataOffset: file position of the first data for CRC check
 *      DataSize: Number of data bytes
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *      UINT32  : CRC32 checksum.
\*-----------------------------------------------------------------------------------------------*/
static UINT32 FwUpdater_GetFwImageCRC32(AMBA_FS_FILE *pFile, UINT32 DataOffset, int DataSize)
{
    void *pCrcBuf = (void *) CrcBuf;
    int CrcBufSize = CrcBufLen;
    UINT32 Crc = AMBA_CRC32_INIT_VALUE;

    memset(pCrcBuf, 0, CrcBufSize);

    if (AmbaFS_fseek(pFile, DataOffset, AMBA_FS_SEEK_START) < 0)
        return 0xffffffff;

    while (DataSize > 0) {
        if (DataSize < CrcBufSize)
            CrcBufSize = DataSize;

        DataSize -= CrcBufSize;

        if (AmbaFS_fread(pCrcBuf, 1, CrcBufSize, pFile) != CrcBufSize)
            return 0xffffffff;

        Crc = AmbaUtility_Crc32Add(pCrcBuf, CrcBufSize, Crc);
    }

    return AmbaUtility_Crc32Finalize(Crc);
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: FwUpdater_ValidateImage
 *
 *  @Description:: Peform a basic sanity check to validate the image.
 *
 *  @Input      ::
 *      pImgHeader: pointer to image header
 *      pFile:  file pointer to firmware
 *      ImgDataOffset:  start file position of image data
 *      ImgSize:    image data size
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static int FwUpdater_ValidateImage(AMBA_FIRMWARE_IMAGE_HEADER_s *pImgHeader,
                                   AMBA_FS_FILE *pFile, UINT32 ImgDataOffset, UINT32 ImgSize)
{
    UINT32 ImgHeaderSize = sizeof(AMBA_FIRMWARE_IMAGE_HEADER_s);
    UINT32 Crc32;

    if (pFile == NULL || pImgHeader == NULL || ImgSize < ImgHeaderSize) {
        AmbaPrint("%s(%d)", __func__, __LINE__);
        return NG;
    }

    if (pImgHeader->Version == 0x0 || pImgHeader->Date == 0x0 ||
        (ImgSize != ImgHeaderSize + pImgHeader->Length)) {
        AmbaPrint("%s(%d)", __func__, __LINE__);
        return NG;
    }

    Crc32 = FwUpdater_GetFwImageCRC32(pFile, ImgDataOffset + ImgHeaderSize, pImgHeader->Length);
    if (Crc32 != pImgHeader->Crc32) {
        AmbaPrint("Verifying image CRC ... 0x%x != 0x%x failed!", Crc32, pImgHeader->Crc32);
        return NG;
    } else {
        AmbaPrint("Verifying image CRC ... done");
        return OK;
    }
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: FwUpdater_UpdateNandPartInfo
 *
 *  @Description:: Update NAND partition info
 *
 *  @Input      ::
 *      pFwHeader:  firmware binary header
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static int FwUpdater_UpdateNandPartInfo(AMBA_FIRMWARE_HEADER_s *pFwHeader)
{
    UINT32 StartBlock = 0, NumBlock = 0;
    UINT32 BlockSize;
    int i;

    BlockSize = AmbaNAND_GetPagesPerBlock() * AmbaNAND_GetMainSize();

    for (i = 0; i < TOTAL_FW_PARTS; i++) {
        NumBlock = (pFwHeader->PartitionSize[i] + (BlockSize - 1)) / BlockSize;
        /* Only Guarantee all blocks are good in MEDIA partition */
        /* NumBlock = FwUpdater_GetContinousNandBlocks(NumBlock, StartBlock, 0); */
        AmbaNAND_SetPartInfo(i, StartBlock, NumBlock);
        StartBlock += NumBlock;
    }

    return OK;
}

#ifdef ENABLE_UPDATE_MEDIA_PARTITIONS
/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: FwUpdater_UpdateNandMetaInfo
 *
 *  @Description:: Update the NAND device meta information.
 *
 *  @Input      ::
 *      pModelName  : Pointer to NAND meta model name.
 *
 *  @Output     :: none
 *
 *  @Return     :: none
 *-----------------------------------------------------------------------------------------------*/
static void FwUpdater_UpdateNandMetaInfo(char *pModelName, AMBA_FIRMWARE_HEADER_s *pFwHeader)
{
    AMBA_NAND_PART_META_s *pNandMeta = &_AmbaNAND_MetaData;
    AMBA_NAND_DEV_s *pNandDev = AmbaRTSL_NandGetDev();
    UINT32 BlockSize = AmbaNAND_GetPagesPerBlock() * AmbaNAND_GetMainSize();
    UINT32 NumFwBlock = 0;
    UINT32 NumBlock, StartBlock, EndBlock;
    int i;

    /* Update NFTL partition info */
    for (i = 0; i < TOTAL_FW_PARTS; i++) {
        AmbaNAND_GetPartInfo(i, &StartBlock, &NumBlock);
        NumFwBlock += NumBlock;
    }

    EndBlock = (pNandDev->DevLogicInfo.TotalBlocks / pNandDev->DevLogicInfo.Intlve) - 1;

    for (i = AMBA_NUM_NAND_PARTITION - 1; i > TOTAL_FW_PARTS; i--) {
        NumBlock = (pFwHeader->PartitionSize[i] + (BlockSize - 1)) / BlockSize;
        NumBlock = FwUpdater_GetContinousNandBlocks(NumBlock, EndBlock, 1);
        StartBlock = EndBlock + 1 - NumBlock;
        if (StartBlock < NumFwBlock) {
            AmbaPrint("Cannot allocate NAND partition!");
            NumBlock = 0;
        }
        AmbaNAND_SetPartInfo(i, StartBlock, NumBlock);
        EndBlock = StartBlock - 1;
    }

    /* Leave remaining NAND blocks for storage 0 */
    StartBlock = NumFwBlock;
    NumBlock = EndBlock + 1 - StartBlock;
    AmbaNAND_SetPartInfo(AMBA_NAND_PARTITION_STORAGE0, StartBlock, NumBlock);

    AmbaNAND_UpdateNftlPartition(pNandDev);

    /* Update NAND meta info */
    pNandMeta->Magic = PTB_META_MAGIC;

    if (pModelName != NULL && strlen(pModelName) <= FW_MODEL_NAME_SIZE)
        strcpy((char *)pNandMeta->ModelName, pModelName);
    else
        pNandMeta->ModelName[0] = '\0';

    for (i = 0; i < AMBA_NUM_NAND_PARTITION; i++) {
        memcpy(pNandMeta->PartInfo[i].Name, AmbaNAND_PartitionName[i], strlen(AmbaNAND_PartitionName[i]));
        pNandMeta->PartDev[i] = PART_ON_NAND;

        AmbaNAND_GetPartInfo(i, &StartBlock, &NumBlock);

        pNandMeta->PartInfo[i].StartBlk = StartBlock;
        pNandMeta->PartInfo[i].NumBlks  = NumBlock;
    }

    pNandMeta->PloadInfo = pFwHeader->PloadInfo;
    pNandMeta->Crc32 = AmbaUtility_Crc32((void *) pNandMeta, PTB_META_ACTURAL_LEN - sizeof(UINT32));

    AmbaNAND_SetMeta(pNandMeta);
}
#endif

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: FwUpdater_CheckImgHeader
 *
 *  @Description:: Validate image header
 *
 *  @Input   ::
 *      pFwHeader:  pointer to firmware binary header
 *
 *  @Output  :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static int FwUpdater_CheckImgHeader(AMBA_FIRMWARE_HEADER_s *pFwHeader)
{
    AMBA_NAND_PART_META_s NandMeta;
    int i;

    if (AmbaNAND_GetMeta(&NandMeta) < 0) {
        AmbaPrint("firmware model name doesn't match!");
        return NG;
    }

    if (strcmp((char *)pFwHeader->ModelName, (char *)NandMeta.ModelName) != 0) {
        AmbaPrint("firmware model name doesn't match!");
        return NG;
    }

    for (i = 0; i < AMBA_NUM_FIRMWARE_TYPE; i++) {
        if (pFwHeader->FwInfo[i].Size > 0) {
            AmbaPrint("Image \"%s\" is found!", AmbaNAND_PartitionName[i]);

            if (pFwHeader->FwInfo[i].Size < sizeof(AMBA_FIRMWARE_IMAGE_HEADER_s)) {
                AmbaPrint("Image \"%s\" is incorrect!", AmbaNAND_PartitionName[i]);
                return NG;
            }
        }
    }

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: FwUpdater_ParseHeader
 *
 *  @Description:: Parse firmware binary header
 *
 *  @Input   ::
 *      pFile:      file pointer to firmware
 *      pFwHeader:  pointer to firmware binary header
 *
 *  @Output  :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static int FwUpdater_ParseFwHeader(AMBA_FS_FILE *pFile, AMBA_FIRMWARE_HEADER_s *pFwHeader)
{
    UINT32 FwHeaderSize = sizeof(AMBA_FIRMWARE_HEADER_s);

    if (FwHeaderSize != AmbaFS_fread(pFwHeader, 1, FwHeaderSize, pFile)) {
        AmbaPrint("can't read firmware file!");
        return NG;
    }

    /* 2. Get old ptb & Meta from NAND. */
    if ((AmbaNAND_GetPtb(&_AmbaNAND_PartTable) != 0) || (AmbaNAND_GetMeta(&_AmbaNAND_MetaData) != 0)) {
        AmbaPrint("Can't get ptb, erase it");
    }

    memcpy(&_AmbaNAND_NftlPartOld, &AmbaNAND_NftlPartInfo, sizeof(AMBA_NAND_NFTL_PART_s));
    memcpy(&_AmbaNAND_FwPartOld,   &AmbaNAND_FwPartInfo,   sizeof(AMBA_NAND_FW_PART_s));

    AmbaNandBBT_Scan();

    if (FwUpdater_UpdateNandPartInfo(pFwHeader) != OK) {
        AmbaPrint("no partition info in firmware!");
        return NG;
    }

    /* 3. Check Header ,CRC and update PTB. */
    if (FwUpdater_CheckImgHeader(pFwHeader) != OK) {
        AmbaPrint("firmware header is illegal!");
        return NG;
    }

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: FwUpdater_CheckFwImage
 *
 *  @Description:: Check all the images in firmware binary
 *
 *  @Input   ::
 *      pFile:      file pointer to firmware
 *      pFwHeader:  pointer to firmware binary header
 *
 *  @Output  :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static int FwUpdater_CheckFwImage(AMBA_FS_FILE *pFile, AMBA_FIRMWARE_HEADER_s *pFwHeader)
{
    AMBA_FIRMWARE_IMAGE_HEADER_s FwImgHeader;
    UINT32 FwImgOffset = sizeof(AMBA_FIRMWARE_HEADER_s);
    int i;

    AmbaPrint("Start firmware CRC check...\n");

    for (i = 0; i < AMBA_NUM_FIRMWARE_TYPE; i++) {
        if (pFwHeader->FwInfo[i].Size == 0)
            continue;

        AmbaPrint("Checking %s ", AmbaNAND_PartitionName[i + AMBA_NAND_PARTITION_SYS_SOFTWARE]);

        /* Read partition header. */
        if ((AmbaFS_fseek(pFile, FwImgOffset, AMBA_FS_SEEK_START) < 0) ||
            (AmbaFS_fread(&FwImgHeader, 1, sizeof(FwImgHeader), pFile) != sizeof(FwImgHeader))) {
            AmbaPrint("Cannot read fw image header!");
            return NG;
        }

        /* validate the image */
        if (FwUpdater_ValidateImage(&FwImgHeader, pFile, FwImgOffset, pFwHeader->FwInfo[i].Size) < 0) {
            AmbaPrint("Invalid CRC32 code!");
            return NG;
        }

        AmbaPrint("\tlength:\t\t%d", pFwHeader->FwInfo[i].Size);
        AmbaPrint("\tcrc32:\t\t0x%08x", FwImgHeader.Crc32);
        AmbaPrint("\tver_num:\t%d.%d", (FwImgHeader.Version >> 16), (FwImgHeader.Version & 0xffff));
        AmbaPrint("\tver_date:\t%d/%d/%d", (FwImgHeader.Date >> 16), ((FwImgHeader.Date >> 8) & 0xff), (FwImgHeader.Date & 0xff));
        AmbaPrint("\timg_len:\t%d", FwImgHeader.Length);
        AmbaPrint("\tmem_addr:\t0x%08x\r\n", FwImgHeader.MemAddr);

        /* Get offset of the next image. */
        FwImgOffset += pFwHeader->FwInfo[i].Size;

        /* Offset should not be greater than 256MB */
        if (FwImgOffset > 0x10000000)
            return NG;
    }

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: FwUpdater_WriteFwImage
 *
 *  @Description:: Write all the images in firmware binary to NAND flash
 *
 *  @Input   ::
 *      pFile:      file pointer to firmware
 *      pFwHeader:  pointer to firmware binary header
 *
 *  @Output  :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static int FwUpdater_WriteFwImage(AMBA_FS_FILE *pFile, AMBA_FIRMWARE_HEADER_s *pFwHeader)
{
//    extern AMBA_KAL_BYTE_POOL_t  AmbaBytePool_Cached;
    UINT8 *pImgBuf = NULL;
    UINT32 ImgBufSize = 0;

    AMBA_FIRMWARE_IMAGE_HEADER_s FwImgHeader;
    AMBA_NAND_PART_s *pNandPart;
    UINT32 FwImgOffset = sizeof(AMBA_FIRMWARE_HEADER_s);
    UINT32 PartID;
    int i, rval;
    AMBA_MEM_CTRL_s MemCtrl;

    /* allocate buffer */
    for (i = 0; i < AMBA_NUM_FIRMWARE_TYPE; i++)
        if(pFwHeader->FwInfo[i].Size > ImgBufSize)
            ImgBufSize = pFwHeader->FwInfo[i].Size;

    ImgBufSize += IMAGE_BUF_RESERVE_SIZE;
    rval = AmpUtil_GetAlignedPool(APPLIB_G_MMPL, (void**) &(MemCtrl.pMemAlignedBase), &MemCtrl.pMemBase, ImgBufSize, 32);

    if (rval < 0) {
        AmbaPrint("Failed to allocate memory");
        return NG;
    }
    pImgBuf = MemCtrl.pMemAlignedBase;

    /* Program to NAND and update PTB. */
    for (i = 0; i < AMBA_NUM_FIRMWARE_TYPE; i++) {
        if (pFwHeader->FwInfo[i].Size == 0)
            continue;

        PartID = (i + AMBA_NAND_PARTITION_SYS_SOFTWARE);
        pNandPart = &_AmbaNAND_PartTable.Part[PartID];

        /* Read partition header. */
        if ((AmbaFS_fseek(pFile, FwImgOffset, AMBA_FS_SEEK_START) < 0) ||
            (AmbaFS_fread(&FwImgHeader, 1, sizeof(FwImgHeader), pFile) != sizeof(FwImgHeader))) {
            AmbaPrint("Cannot read fw image header!");
            return NG;
        }

        if (FwImgHeader.Length > ImgBufSize) {
            AmbaPrint("Cannot fw image size is larger than buffer size!");
            return NG;
        }

        if ((AmbaFS_fread(pImgBuf, 1, FwImgHeader.Length, pFile) != FwImgHeader.Length)) {
            AmbaPrint("firmware image read fail");
            return NG;
        }

        AmbaPrint("Program \"%s\" to NAND flash ...", AmbaNAND_PartitionName[PartID]);
    AmbaPrintk_Flush();

        if (i == AMBA_FIRMWARE_LINUX_KERNEL) {
            UINT32 LinuxBlkNum, BlkSize;
            AMBA_NAND_DEV_s *pNandDev = AmbaRTSL_NandGetDev();
            BlkSize = pNandDev->DevLogicInfo.PagesPerBlock * pNandDev->DevLogicInfo.MainSize;

            LinuxBlkNum = FwImgHeader.Length / BlkSize;
            LinuxBlkNum += (FwImgHeader.Length % BlkSize) ? 1 : 0;
            LinuxBlkNum += 1;

            if (LinuxBlkNum > AmbaNAND_FwPartInfo.NumBlks[PartID]) {
                AmbaPrint("Please reserve 1 block for Linux DTB! %s", AmbaNAND_PartitionName[PartID]);
                return NG;
            }
        }

        if (AmbaNAND_WritePartition(pImgBuf, FwImgHeader.Length, PartID) != OK) {
            AmbaPrint("Failed");
            return NG;
        }

        /* Update the PTB's entry */
        pNandPart->Crc32    = FwImgHeader.Crc32;
        pNandPart->VerNum   = FwImgHeader.Version;
        pNandPart->VerDate  = FwImgHeader.Date;
        pNandPart->ImgLen   = FwImgHeader.Length;
        pNandPart->MemAddr  = FwImgHeader.MemAddr;

        if (AmbaNAND_SetPtb(&_AmbaNAND_PartTable) < 0)
            AmbaPrint("Unable to update ptb %s", AmbaNAND_PartitionName[PartID]);
        {
            AMBA_NAND_PART_META_s *pNandMeta = &_AmbaNAND_MetaData;

            pNandMeta->PloadInfo = pFwHeader->PloadInfo;
            AmbaNAND_SetMeta(pNandMeta);
        }

        AmbaPrint("Done");

        if ((i == AMBA_FIRMWARE_SYS_SOFTWARE) || (i == AMBA_FIRMWARE_LINUX_KERNEL)) {
            AmbaPrint("erase hiber...");
            rval = AmbaNAND_ErasePartition(AMBA_PARTITION_LINUX_HIBERNATION_IMAGE);
            if (rval == 0)
                AmbaPrint("Done");
        }


        /* Get offset of the next image. */
        FwImgOffset += pFwHeader->FwInfo[i].Size;
    }

    return OK;
}

#ifdef ENABLE_UPDATE_MEDIA_PARTITIONS
static UINT32 FwUpdater_CheckMediaPartInfo(AMBA_NAND_NFTL_PART_s *pNewMediaPart, AMBA_NAND_NFTL_PART_s *pOldMediaPart, UINT8 PartID)
{
    if ((pNewMediaPart->StartBlk[PartID]   == pOldMediaPart->StartBlk[PartID]) &&
        (pNewMediaPart->NumBlks[PartID]    == pOldMediaPart->NumBlks[PartID])  &&
        (pNewMediaPart->RsvBlk[PartID]     == pOldMediaPart->RsvBlk[PartID])   &&
        (pNewMediaPart->NumZones[PartID]   == pOldMediaPart->NumZones[PartID]) &&
        (pNewMediaPart->NumTrlBlks[PartID] == pOldMediaPart->NumTrlBlks[PartID]))
        return 0;
    else
        return 1;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: FwUpdater_ReinitMediaPart
 *
 *  @Description::
 *
 *  @Input   ::
 *
 *  @Output  :: none
 *
 *  @Return  ::
 *
\*-----------------------------------------------------------------------------------------------*/
static int FwUpdater_ReinitMediaPart(UINT8 *Change)
{
    int i, mode, Error = 0, Max = AMBA_NUM_NAND_MEDIA_PARTITION;
    AMBA_NAND_DEV_s *pDev = AmbaRTSL_NandGetDev();

    /* Deinit all partitions first because partition may be changed. */
    for (i = 0; i < AMBA_NUM_NAND_MEDIA_PARTITION; i++) {
        if (!AmbaNFTL_IsInit(i))
            continue;

        if (AmbaNFTL_Deinit(i) < 0) {
            AmbaPrint("%d AmbaNFTL_Deinit fail",i);
            Error--;
        }
    }

    /* Update nand device info */
    memset(pDev->DevPartInfo, 0x0, sizeof(pDev->DevPartInfo));
    AmbaNAND_InitNftlPart();

    /* Erase (new firmware) partitions if partition changed. */
    for (i = 0; i < AMBA_NUM_NAND_MEDIA_PARTITION; i++) {
        if (pDev->DevPartInfo[i].FtlBlocks <= 0 || Change[i] == 0)
            continue;

        AmbaPrint("Erase partition \"%s\"!", MediaPartString[i]);
        if (AmbaNFTL_ErasePart(i) < 0) {
            AmbaPrint("%s AmbaNFTL_ErasePart fail", MediaPartString[i]);
            Error--;
        }
    }

    /* Reinit all partitions. */
    for (i = 0; i < Max; i++) {
        if (pDev->DevPartInfo[i].FtlBlocks <= 0)
            continue;

        /* Make sure nftl partition is not initialized. */
        if (AmbaNFTL_IsInit(i)) {
            if (AmbaNFTL_Deinit(i) < 0) {
                AmbaPrint("%s AmbaNFTL_Deinit fail", MediaPartString[i]);
                Error--;
            }
        }

        mode = NFTL_MODE_NO_SAVE_TRL_TBL;

        if (AmbaNFTL_Init(i, mode) < 0) {
            AmbaPrint("%s AmbaNFTL_Init fail", MediaPartString[i]);
            Error--;
        }
    }

    return Error;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: FwUpdater_RestoreMediaPart
 *
 *  @Description:: the main entry of firmware updater
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static int FwUpdater_RestoreMediaPart(UINT8 *Change)
{
   // extern AMBA_KAL_BYTE_POOL_t AmbaBytePool_Cached;
    UINT8 *pImgBuf = NULL;
    int ImgBufSize = 0;

    int i, Rval, Sec;
    char TmpFileName[32], Drive;
    AMBA_NFTL_STATUS_s Status;
    AMBA_FS_FILE *pFile;
    AMBA_MEM_CTRL_s MemCtrl;

    Drive = 'c';

    /* allocate buffer */
    for (i = 0; i < MP_MAX; i ++) {
        if ((i == MP_Storage0) || (i == MP_Storage1) || (i == MP_IndexForVideoRecording))
            continue;
        if (AmbaNAND_NftlPartInfo.NumBlks[i] > ImgBufSize)
            ImgBufSize = AmbaNAND_NftlPartInfo.NumBlks[i];
    }
    ImgBufSize = 512 * ImgBufSize + IMAGE_BUF_RESERVE_SIZE;
    Rval = AmpUtil_GetAlignedPool(APPLIB_G_MMPL, (void**) &(MemCtrl.pMemAlignedBase), &MemCtrl.pMemBase, ImgBufSize, 32);

    if (Rval < 0) {
        AmbaPrint("Failed to allocate memory");
        return NG;
    }
    pImgBuf = MemCtrl.pMemAlignedBase;

    for (i = 0; i < MP_MAX; i ++) {

        if ((i == MP_Storage0) || (i == MP_Storage1) || (i == MP_IndexForVideoRecording))
            continue;

        if (Change[i] == 0)
            continue;

        if (AmbaNFTL_GetStatus(i, &Status) < 0) {
            AmbaPrint("AmbaNFTL_GetStatus fail...");
            return -1;
        }

        sprintf(TmpFileName, "%c:\\%s.bin", Drive, MediaPartString[i]);

        pFile = AmbaFS_fopen(TmpFileName, "r");
        if (pFile == NULL) {
            AmbaPrint("AmbaFS_fopen fail %s ...", TmpFileName);
            return -1;
        }

        for (Sec = 0; Sec < Status.TotalSecs; Sec ++) {
            Rval = AmbaFS_fread((void *)pImgBuf, 1, 512, pFile);
            if (Rval != 512) {
                return -1;
            }
            Rval = AmbaNFTL_Write(i, (UINT8 *)pImgBuf, Sec, 1);
            if (Rval < 0) {
                AmbaPrint("AmbaNFTL_Write %d fail ...", i);
                return -1;
            }
        }
        AmbaFS_fclose(pFile);
    }
    return OK;
}
#endif

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: FirmwareUpdater
 *
 *  @Description::  firmware update for SD card
 *
 *  @Input   ::
 *      pFileName:  filename of the firmware file
 *
 *  @Output  :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static int FirmwareUpdater(char *pFileName)
{
    AMBA_FIRMWARE_HEADER_s FwHeader;
    AMBA_FS_FILE *pFile = NULL;
    AMBA_FS_STAT FileStat;

    /* check if firmware file exists */
    if (AmbaFS_Stat(pFileName, &FileStat) < 0) {
        AmbaPrint("firmware file %s doesn't exist!", pFileName);
        return NG;
    }

    /* Open firmware binary */
    pFile = AmbaFS_fopen(pFileName, "r");
    if (FwUpdater_ParseFwHeader(pFile, &FwHeader) != OK) {
        AmbaFS_fclose(pFile);
        return NG;
    }

    /* Validate firmware image header */
    if (FwUpdater_CheckFwImage(pFile, &FwHeader) != OK) {
        AmbaFS_fclose(pFile);
        return NG;
    }

    /* Update NAND partition table and the content of each partition */
    if (FwUpdater_WriteFwImage(pFile, &FwHeader) != OK) {
        AmbaFS_fclose(pFile);
        return NG;
    }

#ifdef ENABLE_UPDATE_MEDIA_PARTITIONS
    /* Update NAND meta info */
    FwUpdater_UpdateNandMetaInfo(NULL, &FwHeader);
#endif

    //AmbaFwUpdaterSD_ClearMagicCode();

    AmbaFS_fclose(pFile);

    return OK;
}

void sd_update(void)
{
#ifdef ENABLE_UPDATE_MEDIA_PARTITIONS
        UINT8 Change[NFTL_MAX_INSTANCE];
        int i;
#endif

    if (FirmwareUpdater("c:\\AmbaSysFW.bin") != OK) {
        AmbaPrint("AmbaFirmware update fail ...");
        return;
    }

#ifdef ENABLE_UPDATE_MEDIA_PARTITIONS
    /* Get new partition info */
    for (i = 0; i < MP_MAX; i ++) {
        Change[i] = FwUpdater_CheckMediaPartInfo(&AmbaNAND_NftlPartInfo, &_AmbaNAND_NftlPartOld, i);
    }

    FwUpdater_ReinitMediaPart(Change);


    /* Restore Media Part data */
    FwUpdater_RestoreMediaPart(Change);
#endif

}

#endif

#ifdef CONFIG_APP_ARD
#define REC_BACKLIGHT_OFF_EN  (0)
static int app_screen_save_enable = 1;
static int m_is_screen_status = 0;
static int time_1M = 0;
static int screen_lock_flag = 0;
static int screen_lock_time = 0;
static int screen_status = 0;
static int count=0;
static int close_screen_status=1;


void cardv_app_screen_lock(void)
{
    screen_lock_flag = 0;
    screen_lock_time = 0;
}

void cardv_app_screen_unlock(void)
{
    screen_lock_flag = 1;
    screen_lock_time = 0;
}

int cardv_app_screen_check_unlock(void)
{
    return screen_lock_flag;
}

void app_screen_save_set(int en)
{
    app_screen_save_enable = en;
}
void app_set_menu_scr_status(int status)
{
    close_screen_status = status;
}
int app_check_menu_scr_status(void)
{
  return close_screen_status;
}

 void app_set_scr_status(int status)
{
     screen_status = status;
}
int app_check_scr_status(void)
{
     return screen_status;
}

int app_check_screen_status(void)
{
    return m_is_screen_status;
}

void cardv_app_screen_save_timer_init(void)
{
    time_1M = UserSetting->SetupPref.backlightoff_time * 2;
}

static void cardv_app_screen_save_timer(int eid)
{
    if (eid == TIMER_UNREGISTER) {
      time_1M = UserSetting->SetupPref.backlightoff_time * 2;  
      ++count;
      if(count==2)
      {
        count=0;
        if(app_screen_save_enable) {
            //lcd backlight on
            if(cardv_app_screen_check_unlock()) {
                    AppLibSysLcd_SetBacklight_Directly(LCD_CH_DCHAN, 1);
                    m_is_screen_status = 0;
                }
               
            }
        }
        return;
    }

    //AmbaPrint("time_1M=%d",time_1M);
    if(cardv_app_screen_check_unlock()==0) {
        screen_lock_time++;
    } else {
        screen_lock_time = 0;
    }

    time_1M--;
    if (time_1M==0) {
        if(app_screen_save_enable) {
            //lcd backlight off
            if(cardv_app_screen_check_unlock()){
                AppLibSysLcd_SetBacklight_Directly(LCD_CH_DCHAN, 0);
                m_is_screen_status = 1;
            } else {
                time_1M = screen_lock_time * 2;
            }

        }
    }
}


/**
 * When system is busy and no any user key is received, go into screen saving
 */
void cardv_app_screen_handler(int on)
{
#if REC_BACKLIGHT_OFF_EN
    APP_APP_s *CurApp = NULL;
#endif

   // if(BACKLIGHTOFF_TIME_OFF != UserSetting->SetupPref.backlightoff_time) {
#if REC_BACKLIGHT_OFF_EN
        AppAppMgt_GetCurApp(&CurApp);
        if ((APP_CHECKFLAGS(CurApp->gflags, AFLAGS_BUSY)) || (on == 1)) {
#else
        if (on == 1) {
#endif
            AppLibComSvcTimer_Unregister(TIMER_2HZ, cardv_app_screen_save_timer);
        } else {
            AppLibComSvcTimer_Register(TIMER_2HZ, cardv_app_screen_save_timer);
        }
  //  }
}

void cardv_app_screen_handler_timer(int on)
{
    if(on) {
        AppLibComSvcTimer_Register(TIMER_2HZ, cardv_app_screen_save_timer);
        time_1M = UserSetting->SetupPref.backlightoff_time * 2;
    }else {
        AppLibComSvcTimer_Unregister(TIMER_2HZ, cardv_app_screen_save_timer);
    }
}


static int time_DelayPowerOff = 0;
static int enableDelayPowerOff = 0;
static void cardv_app_delay_poweroff_timer(int eid)
{
    APP_APP_s *CurrentApp = NULL;

    if (eid == TIMER_UNREGISTER) {
        time_DelayPowerOff = 0;
        return;
    }

    time_DelayPowerOff--;
    if (time_DelayPowerOff==0) {
        if(UserSetting->SetupPref.DelayPowerTime != DELAY_POWEROFF_OFF) {
            //POWER OFF
            AppAppMgt_GetCurApp(&CurrentApp);
            CurrentApp->OnMessage(HMSG_USER_POWER_BUTTON, 0, 0);
        }
    }
}


void cardv_app_delay_poweroff_handler_timer( int on )
{
    time_DelayPowerOff = UserSetting->SetupPref.DelayPowerTime * 2;
    if(on) {
        AppLibComSvcTimer_Register(TIMER_2HZ, cardv_app_delay_poweroff_timer);
        enableDelayPowerOff = 1;
    }else {
        AppLibComSvcTimer_Unregister(TIMER_2HZ, cardv_app_delay_poweroff_timer);
        enableDelayPowerOff = 0;
    }
}

void cardv_app_delay_poweroff_init(void)
{
    time_DelayPowerOff = UserSetting->SetupPref.DelayPowerTime * 2;
}

static int resend_storage_idle_msg = 0;
int AppUtil_GetRsndStorageIdleMsg(void) {
    return resend_storage_idle_msg;
}

void AppUtil_SetRsndStorageIdleMsg(int msg) {
    resend_storage_idle_msg = msg;
}
#endif


