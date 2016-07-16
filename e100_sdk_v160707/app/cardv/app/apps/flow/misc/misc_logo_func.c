/**
  * @file src/app/apps/flow/misc/connectedcam/misc_logo_func.c
  *
  *  Functions of Logo application
  *
  * History:
  *    2014/11/27 - SuQiang created file
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


#include "misc_logo.h"
#include <system/status.h>
#include <system/app_pref.h>
#include <system/app_util.h>
#include <applib.h>
#include <AmbaUtility.h>
#include <system/ApplibSys_Lcd.h>
#include <AmbaROM.h>
#include <AmbaPLL.h>
#include "AmbaPWC.h"
#include "audio/ApplibAudio_beep.h"
#include <system/ApplibSys_GSensor.h>

static int misc_logo_init(void)
{
    int ReturnValue;

    AppLibFormat_DemuxerInit();
    AppLibStillSingle_Init();
    AppLibStillSingle_VoutAr_Config(0);

    AppLibDisp_SelectDevice(DISP_CH_FCHAN | DISP_CH_DCHAN, DISP_ANY_DEV);
    AppLibDisp_ConfigMode(DISP_CH_FCHAN | DISP_CH_DCHAN, AppLibSysVout_GetVoutMode(VOUT_DISP_MODE_1080P));
    AppLibDisp_SetupChan(DISP_CH_FCHAN | DISP_CH_DCHAN);
    AppLibDisp_ChanStart(DISP_CH_FCHAN | DISP_CH_DCHAN);
    {
    AMP_DISP_WINDOW_CFG_s Window;
    AMP_DISP_INFO_s DispDev = {0};

    memset(&Window, 0x0, sizeof(AMP_DISP_WINDOW_CFG_s));

    ReturnValue = AppLibDisp_GetDeviceInfo(DISP_CH_FCHAN, &DispDev);
    if ((ReturnValue < 0) || (DispDev.DeviceInfo.Enable == 0)) {
        AmbaPrintColor(RED,"[misc_logo] FChan Disable. Disable the fchan Window");
        AppLibDisp_DeactivateWindow(DISP_CH_FCHAN, AppLibDisp_GetWindowId(DISP_CH_FCHAN, 0));
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
    }

    ReturnValue = AppLibDisp_GetDeviceInfo(DISP_CH_DCHAN, &DispDev);
    if ((ReturnValue < 0) || (DispDev.DeviceInfo.Enable == 0)) {
        AmbaPrintColor(RED,"[misc_logo] DChan Disable. Disable the Dchan Window");
        AppLibDisp_DeactivateWindow(DISP_CH_DCHAN, AppLibDisp_GetWindowId(DISP_CH_DCHAN, 0));
    } else {
        /** DCHAN Window*/
        AppLibDisp_GetWindowConfig(DISP_CH_DCHAN, AppLibDisp_GetWindowId(DISP_CH_DCHAN, 0), &Window);
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
        AppLibDisp_SetWindowConfig(DISP_CH_DCHAN, AppLibDisp_GetWindowId(DISP_CH_DCHAN, 0), &Window);
        AppLibDisp_ActivateWindow(DISP_CH_DCHAN, AppLibDisp_GetWindowId(DISP_CH_DCHAN, 0));
    }
        AppLibDisp_FlushWindow(DISP_CH_FCHAN | DISP_CH_DCHAN);
    }

    return 0;
}

static int misc_logo_load_file(UINT8 type,UINT8 IsBlocking)
{
    APPLIB_STILL_FILE_s StillFile;
    //static WCHAR file[MAX_FILENAME_LENGTH] = {0};
    char fileAscii[MAX_FILENAME_LENGTH] = {0}; // Temporary space storing file path in ASCII format
    int Rval = 0;
    UINT32 WaitEventID = 0;

    // Initialize StillFile
    SET_ZERO(StillFile);
    // Get filename
    if(type == LOGO_ON){
        strcpy(fileAscii, "logo.jpg");
    }else if(type == LOGO_OFF){
        strcpy(fileAscii, "logo.jpg");
    }else{
    }

        //check file exist
    if (AmbaROM_FileExists(AMBA_ROM_SYS_DATA, fileAscii) != 1) {
                AmbaPrintColor(RED,"%s is not exist.",fileAscii);
                return -1;
    }

    // Confugure StillFile settings
    strcpy(StillFile.Filename,"ROMFS:\\");
    strcat(StillFile.Filename,fileAscii);
    StillFile.FileSource = 0; // Decode from full-sized image
    // Register callback
    StillFile.LoadEndCB = NULL; // Callback when it's done
    // Output value
    StillFile.OutputWaitEventID = &WaitEventID;

    // Load file
    Rval = AppLibStillSingle_Load(&StillFile);
    if (IsBlocking) {
        if (Rval == 0) {
            // Wait for result
            AppLibStillSingle_WaitLoad(WaitEventID);
        }
    }
    return 0;
}

static int misc_logo_show_logo(UINT8 IsBlocking)
{
    APPLIB_STILL_SINGLE_s StillInfo;
    UINT32 WaitEventID = 0;
    int Rval = 0;
    AMP_AREA_s lcdWindow = {
        .X = 0,
        .Y = 0,
        .Width = 10000,
        .Height = 10000
    };
    AMP_AREA_s lcdPip = {
        .X = 0,
        .Y = 0,
        .Width = 0, // No PIP window
        .Height = 0 // No PIP window
    };
    AMP_AREA_s tvWindow = {
        .X = 0,
        .Y = 0,
        .Width = 10000,
        .Height = 10000
    };
    AMP_AREA_s tvPip = {
        .X = 0,
        .Y = 0,
        .Width = 0, // No PIP window
        .Height = 0 // No PIP window
    };

    // Initialize StillFile
    SET_ZERO(StillInfo);

    // Confugure StillInfo settings
    StillInfo.AreaDchanDisplayMain = lcdWindow;
    StillInfo.AreaDchanPIP = lcdPip;
    StillInfo.AreaFchanDisplayMain = tvWindow;
    StillInfo.AreaFchanPIP = tvPip;
    StillInfo.ImageShiftX = 0; // No shifting
    StillInfo.ImageShiftY = 0; // No shifting
    StillInfo.MagFactor = 100; // No magnification
    StillInfo.ImageRotate = AMP_ROTATE_0; // No rotation and flip
    // Register callback
    StillInfo.ShowEndCB = NULL; // Callback when it's done
    // Output value
    StillInfo.OutputWaitEventID = &WaitEventID;

    // Show image
    Rval = AppLibStillSingle_Show(&StillInfo);
    if (IsBlocking) {
        if (Rval == 0) {
            // Wait for result
            AppLibStillSingle_WaitShow(WaitEventID);
        }
    }

    return Rval;
}

static void misc_logo_timer_handler(int eid)
{
    if (eid == TIMER_UNREGISTER) {
        misc_logo.Func(MISC_LOGO_EXIT_APP, 0, 0);
        return;
    }

    AmbaPrint("[misc_logo] show time: %d", misc_logo.show_time);
    misc_logo.show_time++;
    if (misc_logo.show_time > LOGO_SHOW_SECONDS){
        AppLibComSvcTimer_Unregister(TIMER_1HZ, misc_logo_timer_handler);
    }
}

static int misc_logo_app_start(void)
{
    int ReturnValue = 0;

    misc_logo.show_time = 0;
    ReturnValue = misc_logo_init();
    AppLibAudioDec_Beep_Init();
    AppLibAudio_BeepInit();

    return ReturnValue;
}

static int misc_logo_app_stop(void)
{
    int ReturnValue = 0;

    /** Close the photo decoder. */
    AppLibStillSingle_Deinit();

    return ReturnValue;
}

static int misc_logo_exit(void)
{
    int ReturnValue = 0;

    AmbaPrintColor(RED,"Test mode=0x%x",AppUtil_GetSystemTestMode());

    if(app_status.logo_type == LOGO_ON) {
        /* Disable the vout. */
        AppLibDisp_DeactivateWindow(DISP_CH_FCHAN, AppLibDisp_GetWindowId(DISP_CH_FCHAN, 0));
        AppLibDisp_DeactivateWindow(DISP_CH_DCHAN, AppLibDisp_GetWindowId(DISP_CH_DCHAN, 0));
        AppLibDisp_ChanStop(DISP_CH_FCHAN);
        AppLibStillSingle_VoutAr_Config(1);
        AppUtil_SwitchApp(APP_REC_CAM);
    }else{
        if(AppUtil_SystemTestModeReboot() ==0){
            AmbaPrint("Power off");

            AmbaPWC_ConfigPowerUpSequence(20, 20, 20, 20);
            AmbaPrint("AmbaPWC_ConfigPowerUpSequence 20");
            AmbaKAL_TaskSleep(100);
            /*parkingmonitor is removed 2016-07-12*/
            #if 0
            if(UserSetting->VideoPref.parkingmode_sensitivity == 1){
                AppLibSysGSensor_Enable_Wakeup(UserSetting->VideoPref.parkingmode_sensitivity);
            }else if(UserSetting->VideoPref.parkingmode_sensitivity == 0){
                AppLibSysGSensor_Disable_Wakeup();
            }else{
                AmbaPrint("GSensor:do Nothing");
                AmbaPrintk_Flush();
            }

            if(0 == UserSetting->GSensorSentivityPref.Gsensor_sensitivity) {
                AppLibSysGSensor_Disable_Wakeup();
            }
            #endif
            
            AmbaPrint("AmbaPWC_ForcePowerDownSequence");
            AmbaPWC_ForcePowerDownSequence();
            AmbaPrint("AmbaPWC_ForcePowerDownSequence done");
        }
    }

    return ReturnValue;
}



int misc_logo_func(UINT32 funcId, UINT32 param1, UINT32 param2)
{
    int ReturnValue = 0;

    switch (funcId) {
    case MISC_LOGO_START:
        misc_logo_app_start();
        break;

    case MISC_LOGO_STOP:
        misc_logo_app_stop();
        break;

    case MISC_LOGO_SHOW_LOGO:
        if(misc_logo_load_file(app_status.logo_type,1)== 0){
        int rval;
            misc_logo_show_logo(1);
            AppLibSysLcd_SetBrightness(LCD_CH_DCHAN,1,0);
            AppLibSysLcd_SetBacklight(LCD_CH_DCHAN, 1);
            rval = AppLibComSvcTimer_Register(TIMER_1HZ, misc_logo_timer_handler);
        if(app_status.logo_type == LOGO_OFF){
            AppLibAudioDec_Beep(BEEP_POWER_OFF,1);
        }
        }else{
            misc_logo_exit();
        }
        break;
        
    case MISC_LOGO_EXIT_APP:
        misc_logo_exit();
        break;
    default:
        //AmbaPrint("[app_misc_logo] The function is not defined");
        break;
    }

    return ReturnValue;
}
