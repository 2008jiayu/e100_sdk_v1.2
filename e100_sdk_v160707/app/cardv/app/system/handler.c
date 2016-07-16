/**
 * @file app/connected/app/system/handler.c
 *
 * Application Handler file.
 *
 * History:
 *    2013/08/13 - [Martin Lai] created file
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

#include <framework/appmaintask.h>
#include <framework/appdefines.h>
#include <framework/apphmi.h>
#include <framework/appmgt.h>
#include "AmbaPWC.h"
#include <apps/apps.h>
#include <apps/flow/widget/widgetmgt.h>
#include <applib.h>
#include <system/ApplibSys_Lcd.h>
#include <peripheral_mod/ui/button/button_op.h>
#include <peripheral_mod/ui/ir/irbutton_op.h>
#include <apps/gui/resource/gui_settle.h>
#include <apps/flow/widget/dialog/dialog.h>
#include <apps/flow/widget/dialog/dialog_table.h>
#include "status.h"
#include "app_pref.h"
#include "app_util.h"
#include <calibration/ApplibCalibMgr.h>
#if defined(CONFIG_APP_AMBA_LINK)
#include <net/ApplibNet_Base.h>
#include <net/ApplibNet_IPCTest.h>
#include <net/ApplibNet_JsonUtility.h>
#include "onlinePlayback.h"
#endif
#include <imgproc/AmbaImg_Proc.h>
#include <imgproc/AmbaImg_Impl_Cmd.h>

//#define ARD_DEBUG
#if defined(ARD_DEBUG)
#define DBGMSG  AmbaPrint
#define DBGMSGc2    AmbaPrintColor
#else
#define DBGMSG(...)
#define DBGMSGc2(...)
#endif

#ifdef CONFIG_APP_ARD
#include "AmbaSysCtrl.h"
#include <system/ApplibSys_GSensor.h>
//#define ENABLE_HDMI 1    //if enable HDMI preview, it will limited the sec stream wide
extern void BspRefBotton_jamed_handler_en(int enable);
static INT8 EmptyTrackNum = 0;
#endif

static APP_APP_s *CarCamApps[APPS_NUM];

/**
 *  @brief Register the sensor
 *
 *  Register the sensor
 *
 *  @return >=0 success, <0 failure
 */
static int AppHandler_RegisterSensor(void)
{
    extern UINT32 AppLibIQ_ImageInit(UINT32 chNo);
    int ReturnValue = 0;

#ifdef CONFIG_SENSOR_SONY_IMX117
    {
        extern int AppSensor_register_imx117_a12(void);
        extern int AppIQParamImx117_A12_Register(UINT32 chNo);
        extern CALIBRATION_ADJUST_PARAM_s AmbaCalibParamsImx117;

        ReturnValue = AppSensor_register_imx117_a12();
        AppLibSysSensor_SetIQChannelCount(1);

        AppLibIQ_ImageInit(1);
        AppIQParamImx117_A12_Register(0);

        AppLibCalibTableSet(&AmbaCalibParamsImx117);
    }
#endif


#ifdef CONFIG_SENSOR_SONY_IMX206
    {
        extern int AppSensor_register_imx206_a12(void);
        extern int AppIQParamImx206_A12_Register(UINT32 chNo);
        extern CALIBRATION_ADJUST_PARAM_s AmbaCalibParamsImx206;

        ReturnValue = AppSensor_register_imx206_a12();
        AppLibSysSensor_SetIQChannelCount(1);

        AppLibIQ_ImageInit(1);
        AppIQParamImx206_A12_Register(0);

        AppLibCalibTableSet(&AmbaCalibParamsImx206);
    }
#endif


#ifdef CONFIG_SENSOR_OV4689
    {
        extern int AppSensor_register_ov4689_a12(void);
        extern int AppIQParamOv4689_A12_Register(UINT32 chNo);
        extern int AmbaIQParamOv4689Hdr_A12_Register(UINT32 chNo);
        extern CALIBRATION_ADJUST_PARAM_s AmbaCalibParamsOv4689;

        ReturnValue = AppSensor_register_ov4689_a12();
        AppLibSysSensor_SetIQChannelCount(2);

        AppLibIQ_ImageInit(2);/**Ov4689 has HDR Mode, need 2 channel*/
        AppIQParamOv4689_A12_Register(0);
        AmbaIQParamOv4689Hdr_A12_Register(1);

        AppLibCalibTableSet(&AmbaCalibParamsOv4689);
    }
#endif


#ifdef CONFIG_SENSOR_AR0330_PARALLEL
    {
        extern int AppSensor_register_ar0330_parallel_a12(void);
        extern int AppIQParamAr0330_Parallel_A12_Register(UINT32 chNo);
        extern CALIBRATION_ADJUST_PARAM_s AmbaCalibParamsAr0330Parallel;

        ReturnValue = AppSensor_register_ar0330_parallel_a12();
        AppLibSysSensor_SetIQChannelCount(1);

        AppLibIQ_ImageInit(1);
        AppIQParamAr0330_Parallel_A12_Register(0);

        AppLibCalibTableSet(&AmbaCalibParamsAr0330Parallel);
    }
#endif


#ifdef CONFIG_SENSOR_AR0230
    {
        extern int AppSensor_register_ar0230_a12(void);
        extern int AppIQParamAr0230_A12_Register(UINT32 chNo);
        extern int AppIQParamAr0230Hdr_A12_Register(UINT32 chNo);
        extern CALIBRATION_ADJUST_PARAM_s AmbaCalibParamsAr0230;

        ReturnValue = AppSensor_register_ar0230_a12();
        AppLibSysSensor_SetIQChannelCount(2);

        AppLibIQ_ImageInit(2);/**AR0230 has HDR Mode, need 2 channel*/
        AppIQParamAr0230_A12_Register(0);
        AppIQParamAr0230Hdr_A12_Register(1);

        AppLibCalibTableSet(&AmbaCalibParamsAr0230);
    }
#endif


#ifdef CONFIG_SENSOR_MN34120
    {
        extern int AppSensor_register_mn34120_a12(void);
        extern int AppIQParamMn34120_A12_Register(UINT32 chNo);

        ReturnValue = AppSensor_register_mn34120_a12();
        AppLibSysSensor_SetIQChannelCount(1);

        AppLibIQ_ImageInit(1);
        AppIQParamMn34120_A12_Register(0);
    }
#endif


    return ReturnValue;
}


/**
 *  @brief Register Lens
 *
 *  Register Lens
 *
 *  @return >=0 success, <0 failure
 */
static int AppHandler_RegisterLens(void)
{
    int ReturnValue = 0;

    return ReturnValue;
}

/**
 *  @brief Register Gyro
 *
 *  Register Gyro
 *
 *  @return >=0 success, <0 failure
 */
static int AppHandler_RegisterGyro(void)
{
    int ReturnValue = 0;
    {
        extern int AppGyro_Register(void);
        AppGyro_Register();
    }

    return ReturnValue;
}

/**
 *  @brief Register Gyro
 *
 *  Register Gyro
 *
 *  @return >=0 success, <0 failure
 */
static int AppHandler_RegisterGps(void)
{
    int ReturnValue = 0;

#ifdef CONFIG_GPS_UBLOX6_GM6XX
    {
        extern int AppGps_RegisterGM6xx(void);
        AppGps_RegisterGM6xx();
    }
    AppLibSysGps_Init();
#endif

#ifdef CONFIG_ECL_RADAR_MODEL
    {
        extern int AppRadar_Register_ecl(void);
        AppRadar_Register_ecl();
    }
    AppLibSysGps_Init();
#endif

    return ReturnValue;
}

/**
 *  @brief Register Lcd
 *
 *  Register Lcd
 *
 *  @param [in] lcdChanID Lcd channel id
 *
 *  @return >=0 success, <0 failure
 */
static int AppRegisterLcd(UINT32 lcdChanID)
{
    int ReturnValue = 0;
#ifdef CONFIG_LCD_WDF9648W
    AmbaPrint("[App-LCD] LCD panel selected: wdf9648w");
    {
        extern int AppLcd_RegisterWdf9648w(UINT32 lcdChanID);
        ReturnValue = AppLcd_RegisterWdf9648w(lcdChanID);
    }
#endif
#ifdef CONFIG_LCD_T20P52
    AmbaPrint("[App-LCD] LCD panel selected: t20p52");
    {
        extern int AppLcd_RegisterT20P52(UINT32 lcdChanID);
        ReturnValue = AppLcd_RegisterT20P52(lcdChanID);
    }
#endif
#ifdef CONFIG_LCD_T27P05
        AmbaPrint("[App-LCD] LCD panel selected: t27p05");
        {
            extern int AppLcd_RegisterT27P05(UINT32 lcdChanID);
            ReturnValue = AppLcd_RegisterT27P05(lcdChanID);
        }
#endif

#ifdef CONFIG_LCD_SSD2828
    AmbaPrint("[App-LCD] LCD panel selected: SSD2828");
    {
        extern int AppLcd_RegisterSsd2828(UINT32 lcdChanID);
        ReturnValue = AppLcd_RegisterSsd2828(lcdChanID);
    }
#endif

    return ReturnValue;
}

/**
 *  @brief Register GSensor
 *
 *  Register GSensor
 *
 *  @return >=0 success, <0 failure
 */
#ifdef CONFIG_APP_ARD
static int AppHandler_RegisterGSensor(void)
{
    int ReturnValue = 0;

#ifdef CONFIG_G_SENSOR_ST_LIS3DE
    {
        extern int AppGSensor_RegisterSTLIS3DE(void);
        AppGSensor_RegisterSTLIS3DE();
    }
#endif

#ifdef CONFIG_G_SENSOR_MIRA_DA380
    {
        extern int AppGSensor_RegisterMIRADA380(void);
        AppGSensor_RegisterMIRADA380();
    }
#endif


    return ReturnValue;
}
#endif
/**
 *  @brief Pre-initialization flows for booting
 *
 *  Pre-initialization flows for booting
 *
 *  @return >=0 success, <0 failure
 */
static int AppHandler_AppPreInit(void)
{
    int ReturnValue = 0;

    /**
    * Application status and perference should be initialized
    * before initialize app library which may actually config
    * system states
    **/

    return ReturnValue;
}

/**
 *  @brief Initializations can be initialized before pipe initialization
 *
 *  Initializations can be initialized before pipe initialization
 *
 *  @return >=0 success, <0 failure
 */
static int AppHandler_AppInit(void)
{
    int ReturnValue = 0;

    /** Initialize 3A . */
    /** Register and initialize sensor.*/
    AppLibSysSensor_PreInit();
    AppHandler_RegisterSensor();
    AppLibSysSensor_Init();

    /** Load the user's perference. and initialize the status of app.*/
    AppPref_InitPref();
    AppPref_Load();

#ifdef CONFIG_APP_ARD
    UserSetting->VideoPref.DualStreams = VIDEO_DUAL_STREAMS_ON;
#else
    UserSetting->VideoPref.DualStreams = VIDEO_DUAL_STREAMS_ON;
#endif
#if defined(CONFIG_APP_AMBA_LINK)
    /** Enable net stream by default.*/
    UserSetting->VideoPref.StreamType = STREAM_TYPE_RTSP;
#endif
    /**default set time lapse of PES = 500MS*/
    UserSetting->PhotoPref.TimeLapse = PHOTO_TIME_LAPSE_500MS;
    AppUtil_StatusInit();

#ifndef CONFIG_APP_ARD
    /* default stamp as STAMP_DATE_TIME */
    UserSetting->VideoPref.VideoDateTimeStamp = STAMP_DATE_TIME;
    UserSetting->PhotoPref.PhotoTimeStamp = STAMP_DATE_TIME;
#endif

    /** Register and initialize lens.*/
    AppLibSysLens_PreInit();
    AppHandler_RegisterLens();
    AppLibSysLens_Init();

    /** Register and initialize gyro sensor.*/
    AppLibSysGyro_PreInit();
    AppHandler_RegisterGyro();
    AppLibSysGyro_Init();

     /** Initialize calibration.*/
    AppLibCalib_Load();

    /** Initialize image algo.*/
    AppLibImage_Init();
    /* Enable 3A. */
    AppLibImage_Set3A(1);

    /** Register and initialize LCD. */
    AppLibSysLcd_PreInit();
    AppRegisterLcd(LCD_CH_DCHAN);
    AppLibSysLcd_Init(LCD_CH_DCHAN);
#ifndef CONFIG_APP_ARD
    AppLibSysLcd_SetBacklight(LCD_CH_DCHAN, 1);//Enable LCD back light
#endif
    AppLibSysLcd_SetSeamless(LCD_CH_DCHAN);

    /** Initial display. */
    AppLibDisp_Init();
#ifdef CONFIG_APP_ARD
#ifdef CONFIG_BSP_ORTHRUS
    AppLibDisp_Rotate(DISP_CH_DCHAN,AMP_ROTATE_90);
#endif
#endif
    AppLibDisp_InitWindow();
    AppLibDisp_EnableChan(DISP_CH_DUAL);

    /** Initilalize GPS */
    AppLibSysGps_PreInit();
    AppHandler_RegisterGps();

#ifdef CONFIG_APP_ARD
    /** Initialize Gsensor.*/
    AppLibSysGSensor_PreInit();
    AppHandler_RegisterGSensor();
    /*park mode:detect g sensor active event before switch mode(rec_cardv app will re-init g sensor)*/
#if 0
    if(UserSetting->VideoPref.parkingmode_sensitivity == 1){
        if((AppLibSysGSensor_Get_Wakeup_Status()== 0)){
             AmbaPrintColor(RED,"park mode:g sensor active interrupt occured");
             app_status.parkingmode_on = 1;
             app_status.cardv_auto_encode = 1;
          }else{
             AmbaPrintColor(RED,"park mode:g sensor active interrupt do not occured");
             app_status.parkingmode_on = 0;
        }
     }

    AppLibSysGSensor_Disable_Wakeup();
    if(AppLibSysGSensor_Init() != OK){
        AmbaPrintColor(RED,"G-Sensor init fail");
    }
#endif
    {
    UINT8 enable = (UserSetting->SetupPref.beep_sound == BEEP_SOUND_ON)?1:0;
    AppLibAudioDec_Beep_Enable(enable);
    }
#endif

    /** Initialize jack detection.*/
    //AppUtil_JackDetectInit();

    /** Check vout status.*/
    //AppUtil_CheckVoutJack();
    AppLibDisp_SelectDevice(DISP_CH_FCHAN | DISP_CH_DCHAN, DISP_ANY_DEV);

#ifdef CONFIG_APP_ARD
/*Disable USB detection when test mode*/
if(AppUtil_GetSystemTestMode() == TEST_MODE_OFF){
#endif
    /* Initialize the USB.*/
    AppLibUSB_Init();
    ReturnValue = AppLibUSB_InitJack();
#ifdef CONFIG_APP_ARD
}
#endif

    /** Initial graphic. */
    AppGui_Init();
    app_status.Type = APP_TYPE_DV;

    /*For testing.*/
#ifdef CONFIG_APP_ARD
    app_status.UsbTreatAsDC = 0;
    if(AppLibUSB_GetVbusStatus() == 0) {
        app_status.cardv_auto_encode = 0;
        app_status.PowerType = APP_POWER_BATTERY;
    }else{
        app_status.cardv_auto_encode = 1;
        app_status.PowerType = APP_POWER_ADAPTER;
    }
#else
    app_status.PowerType = APP_POWER_ADAPTER;
#endif
#ifdef CONFIG_APP_ARD
    #ifdef ENABLE_HDMI
    app_status.FchanDecModeOnly = 0;
    AppLibVideoEnc_SetDualStreams(0);
    #else
    app_status.FchanDecModeOnly = 1;
    #endif
#else
    app_status.FchanDecModeOnly = 1;
#endif
    //AppLibVideoEnc_SetRecMode(REC_MODE_VIDEO_ONLY);

#ifdef CONFIG_APP_ARD
    app_status.anti_flicker_type = UserSetting->SetupPref.anti_flicker_type;
    /*Enable auto encode when test mode is on(burn-in test)*/
    if(AppUtil_CheckSystemTestModeAutoPowerOff()){
        app_status.cardv_auto_encode = 1;
        app_status.PowerType = APP_POWER_ADAPTER;
    }

    if(app_status.parkingmode_on == 1){
        app_status.cardv_auto_encode = 1;
    }
#endif


    /** Register applications' flow. */
    AppAppMgt_Init(CarCamApps, APPS_NUM);
    APP_MAIN = AppAppMgt_Register(&app_main);
    APP_REC_CAM = AppAppMgt_Register(&app_rec_cam);
    APP_PB_VIDEO = AppAppMgt_Register(&app_pb_video);
    APP_PB_PHOTO = AppAppMgt_Register(&app_pb_photo);
    APP_PB_MULTI = AppAppMgt_Register(&app_pb_multi);
    APP_THUMB_MOTION = AppAppMgt_Register(&app_thumb_motion);
    APP_USB_MSC = AppAppMgt_Register(&app_usb_msc);
    APP_USB_AMAGE = AppAppMgt_Register(&app_usb_amage);
    APP_MISC_FWUPDATE = AppAppMgt_Register(&app_misc_fwupdate);
    APP_MISC_MVRECOVER = AppAppMgt_Register(&app_misc_mvrecover);
    APP_MISC_FORMATCARD = AppAppMgt_Register(&app_misc_formatcard);
    APP_MISC_CALIBUPDATE = AppAppMgt_Register(&app_misc_calibupdate);
    APP_MISC_DEFSETTING = AppAppMgt_Register(&app_misc_defsetting);
#if defined (ENABLE_HDMI_TEST)
    APP_HDMI_TEST = AppAppMgt_Register(&app_hdmi_test);
#endif
#ifdef CONFIG_APP_ARD // to be continued
    APP_MISC_LOGO = AppAppMgt_Register(&app_misc_logo);
    //APP_REC_SOUND = AppAppMgt_Register(&app_rec_sound); // to be continued
    //APP_PB_SOUND = AppAppMgt_Register(&app_pb_sound); // to be continued

    // to be continued
    UserSetting->SetupPref.SensorSwitch = 0;

    app_status.logo_type = LOGO_ON;
    AppUtil_SwitchApp(APP_MISC_LOGO);

#else
    AppUtil_SwitchApp(AppUtil_GetStartApp(0));
#endif

#ifdef CONFIG_APP_ARD
    cardv_app_screen_save_timer_init();

    app_status.anti_flicker_type = UserSetting->ImagePref.Flicker;

    cardv_app_screen_unlock();
#endif

    return ReturnValue;
}

/**
 *  @brief Initializations can be initialized after pipe initialization
 *
 *  Initializations can be initialized after pipe initialization
 *
 *  @return >=0 success, <0 failure
 */
static int AppHandler_AppPostInit(void)
{
    /** Initial muxer and demuxer. */
    AppLibFormat_Init();

    /** Set the storage type for index files */
    #ifdef CONFIG_ENABLE_EMMC_BOOT
    AppLibIndex_SetStorageType(APPLIB_INDEX_FILE_STROAGE_TYPE_MEMORY);
    #else
    AppLibIndex_SetStorageType(APPLIB_INDEX_FILE_STROAGE_TYPE_NAND);
    #endif

#ifdef CONFIG_APP_ARD
    AppLibFormat_SetDualFileSaving(VIDEO_DUAL_SAVING_OFF);
#else
    AppLibFormat_SetDualFileSaving(VIDEO_DUAL_SAVING_ON);
#endif
    AppLibFormat_SetPriStreamFileSizeAlignment(20<<20);//20MB
    AppLibFormat_SetSecStreamFileSizeAlignment(20<<20);//20MB

    /** Initial storage DMF. */
    AppLibStorageDmf_Init(0);

    /** Inital DCF config*/
    {
        APPLIB_DCF_CFG_s InitConfig = {0};

        InitConfig.NumberMode = UserSetting->SetupPref.DMFMode;
        InitConfig.BrowseMode = APPLIB_DCF_MEDIA_DCIM;
        InitConfig.PhotoLastIdx = UserSetting->SetupPref.DmfPhotoLastIdx;
        InitConfig.SoundLastIdx= UserSetting->SetupPref.DmfSoundLastIdx;
        InitConfig.VideoLastIdx = UserSetting->SetupPref.DmfVideoLastIdx;
        InitConfig.MixLastIdx = UserSetting->SetupPref.DmfMixLastIdx;
        AppLibDCF_SetDefaultCfg(&InitConfig);
    }

    /** Initial card. */
    app_status.CardBusySlot = NO_BUSY_SLOT;
    AppLibCard_Init();
    AppUtil_PollingAllSlots();


    /** Initialize Buttons.*/
    AppButtonOp_Init();
    AppIRButtonOp_Init();

    /** Initialize asynchronous operations */
    AppLibComSvcAsyncOp_Init();

    /** Initialize Timer.*/
    AppLibComSvcTimer_Init();

#ifdef CONFIG_APP_ARD
    /*Initialize Timer for battery voltage monitor*/
    AppUtil_BatteryVoltageMonitor();
    /*gps connected and signal detect*/
    AppUtil_Gps_Detect_Monitor();
    AppUtil_AutoPowerOffInit(0);
#endif

    /** Initialize Widget.*/
    AppWidget_Init();
    AppDialog_SetDialogTable(dialog_subject_str_tbl);

    /** Initialize Jack monitor. */
    AppUtil_JackMonitor();

#ifdef CONFIG_APP_ARD
    /* Temp. Test. To be continued  */
    {   /**< Hook sensor unit test */
        extern  int SensorUT_TestAdd(void) __attribute__((weak)) ;

        if (SensorUT_TestAdd) {
            SensorUT_TestAdd();
        }
    }
#endif

    /* Add the test commands of application. */
    {
        extern int App_TestAdd(void);
        App_TestAdd();
    }
#if defined(CONFIG_APP_AMBA_LINK)
    /* The second param of  AppLibNetBase_InitAmbaLink() is used to indicate that
          hibernation is enabled or not. '1' means enabled, otherwise disabled.
          This setting should be corresponded with linux kenel image's configuration. */
    AppLibNetBase_InitAmbaLink(APPLIB_G_MMPL, 1);
    app_status.NetCtrlSessionOn = 0;

    AppLibNetIPCTest_Add(APPLIB_G_MMPL);
#endif

#ifdef CONFIG_APP_ARD
    /** Initialize the driver_id setting. */
#define DRIVER_ID_NUM_LENGTH 9
    {
        int i = 0;
        int valid = 1;
        for(i=0;i<DRIVER_ID_NUM_LENGTH;i++){
            if((UserSetting->SetupPref.driver_id[i]>='0'&&UserSetting->SetupPref.driver_id[i]<='9')||
               (UserSetting->SetupPref.driver_id[i]>='A'&&UserSetting->SetupPref.driver_id[i]<='Z')||
               (UserSetting->SetupPref.driver_id[i]=='-')){
                valid = 1;
            }else{
                valid = 0;
                break;
            }
        }

        /** Set the default driver_id as XXXXXXXXX */
        if( valid == 0 ){
            for(i=0;i<DRIVER_ID_NUM_LENGTH;i++){
                UserSetting->SetupPref.driver_id[i] = 'X';
            }
        }
    }

    {
        APP_APP_s *curapp;
        AppAppMgt_GetCurApp(&curapp);
        curapp->OnMessage(AMSG_CMD_RESET_SPLIT_TIME, UserSetting->VideoPref.video_split_rec_time, 0);
    }

    cardv_app_delay_poweroff_init();

#endif

    return 0;
}



#if defined(CONFIG_APP_AMBA_LINK)
static int AppHandler_StartNetService(void)
{
    /* net services should be intiailized or started after linux booted. */
    AppLibNetControl_Init(APPLIB_G_MMPL);

    AppLibNetFifo_PlaybackRegisterApp(OnlinePlayback);

    AppLibNetFifo_Init();
    AppLibNetFifo_EnablePlayback();

    /* stream mode should be set before starting RTSP server. */
    AppLibNetFifo_SetStreamMode(NET_STREAM_MODE_VIDEO_ONLY);
    AppLibNetFifo_StartRTSPServer();

    return 0;
}

/* param 'Token' is only meaningful when SessionStart is 1. */
static int AppHandler_NetControlSession(UINT8 SessionStart, UINT32 Token)
{
    int MsgId = 0;
    int ReturnValue = 0;
    APPLIB_JSON_OBJECT *JsonObject = NULL;
    APPLIB_JSON_STRING *JsonString = NULL;
    char *ReplyString = NULL;

    MsgId = (SessionStart == 1) ? AMBA_START_SESSION : AMBA_STOP_SESSION;

    ReturnValue = AppLibNetJsonUtility_CreateObject(&JsonObject);
    if (ReturnValue == 0) {
        if (SessionStart == 1) {
            AmbaPrintColor(GREEN,"[App Handler] AMSG_NETCTRL_SESSION_START");
            app_status.NetCtrlSessionOn = 1;

            AppLibNetJsonUtility_AddIntegerObject(JsonObject,"rval", 0);
            AppLibNetJsonUtility_AddIntegerObject(JsonObject,"msg_id", MsgId);
            AppLibNetJsonUtility_AddIntegerObject(JsonObject,"param", Token);
        } else {
            AmbaPrintColor(GREEN,"[App Handler] AMSG_NETCTRL_SESSION_STOP");
            app_status.NetCtrlSessionOn = 0;

            AppLibNetJsonUtility_AddIntegerObject(JsonObject,"rval", 0);
            AppLibNetJsonUtility_AddIntegerObject(JsonObject,"msg_id", MsgId);
        }

        AppLibNetJsonUtility_JsonObjectToJsonString(JsonObject, &JsonString);
        AppLibNetJsonUtility_GetString(JsonString, &ReplyString);
        if (ReplyString) {
            AppLibNetControl_ReplyToLnx(ReplyString, strlen(ReplyString));
        }

        AppLibNetJsonUtility_FreeJsonString(JsonString);
        AppLibNetJsonUtility_FreeJsonObject(JsonObject);
    }else {
        ReturnValue = -1;
        AppLibNetControl_ReplyErrorCode(MsgId, ERROR_NETCTRL_JSON_PACKAGE_ERROR);
    }

    return ReturnValue;
}
#endif /* CONFIG_APP_AMBA_LINK */


/**
 *  @brief To handle the message of app.
 *
 *  To handle the message.
 *
 *  @return >=0 success, <0 failure
 */
static void AppHandler_Entry(void)
{
    static int count=0;
    int AppExit = 0;
    UINT32 Param1 = 0, Param2 = 0;
    APP_APP_s *CurApp;
    APP_MESSAGE_s Msg = {0};
    int ReturnValue = 0;

    AppHandler_AppPreInit();
    AppHandler_AppInit();
    AppHandler_AppPostInit();

    while (AppExit == 0) {
        AppLibComSvcHcmgr_RcvMsg(&Msg, AMBA_KAL_WAIT_FOREVER);
        Param1 = Msg.MessageData[0];
        Param2 = Msg.MessageData[1];
        DBGMSG("[App Handler] Received msg: 0x%X (Param1 = 0x%X / Param2 = 0x%X)", Msg.MessageID, Param1, Param2);

#if defined(CONFIG_APP_AMBA_LINK)
        /* button and t_app operation should be blocked while there is a net control connection. */
        if (app_status.NetCtrlSessionOn) {
            if ((MSG_MDL_ID(Msg.MessageID) == MDL_APP_KEY_ID) ||
                ((MSG_MDL_ID(Msg.MessageID) == MDL_APP_FLOW_ID) && (MSG_APP_FLOW_TYPE(Msg.MessageID) == HMSG_APPFLOW_ID_TEST))) {
                AmbaPrint("[App Handler] button/t_app operation is blocked!");
                continue;
            }
        }
#endif
#ifdef CONFIG_APP_ARD
        cardv_app_screen_handler(LCD_SCR_TURN_OFF);
        if ((MSG_MDL_ID(Msg.MessageID) == MDL_APP_KEY_ID)&&
            (HMSG_USB_DETECT_CONNECT !=Msg.MessageID) &&(HMSG_USB_DETECT_REMOVE !=Msg.MessageID)&&(HMSG_USER_POWER_BUTTON!=Msg.MessageID)) {
            int ignore_msg =0;
            if((app_check_screen_status() == 1)||(app_check_scr_status() == 1)) {
                ignore_msg = 1;
            } 
            
            if(app_check_scr_status() == 1)
            {
                ++count;
                if(count==2)
                {
                   app_set_scr_status(0);
                   count=0;
                }
                AppUtil_AutoPowerOffInit(0);
                cardv_app_screen_handler(LCD_SCR_TURN_ON);
            }
            else
            {
                AppUtil_AutoPowerOffInit(0);
                cardv_app_screen_handler(LCD_SCR_TURN_ON);
            }

            if(ignore_msg) {
                continue;
            }
        }

        if(app_status.parkingmode_on == 1){
            int cur_app_id = -1;
            cur_app_id = AppAppMgt_GetCurApp(&CurApp);
            if(cur_app_id == APP_REC_CAM) {
                /*when parking mode trig and doing event record,any key can not respond except power*/
                if ((Msg.MessageID != HMSG_USB_DETECT_CONNECT)&&(Msg.MessageID != HMSG_USB_DETECT_REMOVE)
                    &&(MSG_MDL_ID(Msg.MessageID) == MDL_APP_KEY_ID)){
                switch (Msg.MessageID) {
                    case HMSG_USER_POWER_BUTTON:
                        break;
                    case HMSG_USER_RECORD_BUTTON:
                    case HMSG_USER_MENU_BUTTON:
                        app_status.parkingmode_on = 0;
                        break;
                    default:
                      continue;
                    }
                }
            }
        }
#endif

        /* Message handling. */
        switch (Msg.MessageID) {
        case HMSG_TIMER_CHECK:
        case HMSG_TIMER_1HZ:
        case HMSG_TIMER_2HZ:
        case HMSG_TIMER_4HZ:
        case HMSG_TIMER_10HZ:
        case HMSG_TIMER_20HZ:
        case HMSG_TIMER_5S:
        case HMSG_TIMER_30S:
            AppLibComSvcTimer_Handler(Param1);
            break;
        case HMSG_HDMI_INSERT_SET:
            if (app_status.HdmiPluginFlag == 0) {
                app_status.HdmiPluginFlag = 1;
                AppAppMgt_GetCurApp(&CurApp);
                CurApp->OnMessage(Msg.MessageID, Param1, Param2);
            }
            break;
        case HMSG_HDMI_INSERT_CLR:
            if (app_status.HdmiPluginFlag == 1) {
                app_status.HdmiPluginFlag = 0;
                AppAppMgt_GetCurApp(&CurApp);
                CurApp->OnMessage(Msg.MessageID, Param1, Param2);
            }
            break;
        case HMSG_CS_INSERT_SET:
            if (app_status.CompositePluginFlag == 0) {
                app_status.CompositePluginFlag = 1;
                AppAppMgt_GetCurApp(&CurApp);
                CurApp->OnMessage(Msg.MessageID, Param1, Param2);
            }
            break;
        case HMSG_CS_INSERT_CLR:
            if (app_status.CompositePluginFlag == 1) {
                app_status.CompositePluginFlag = 0;
                AppAppMgt_GetCurApp(&CurApp);
                CurApp->OnMessage(Msg.MessageID, Param1, Param2);
            }
            break;
#ifdef CONFIG_APP_ARD
    /*Libra use Event button(F2) to indicate AV cable insert/remove */
    case HMSG_USER_F2_BUTTON:
        if (app_status.CompositePluginFlag == 0) {
            app_status.CompositePluginFlag = 1;
            AppAppMgt_GetCurApp(&CurApp);
            CurApp->OnMessage(HMSG_CS_INSERT_SET, 0, 0);
        }else{
            app_status.CompositePluginFlag = 0;
            AppAppMgt_GetCurApp(&CurApp);
            CurApp->OnMessage(HMSG_CS_INSERT_CLR, 0, 0);
        }
        break;
#endif
        case HMSG_USB_DETECT_CONNECT:
            AmbaPrintColor(GREEN,"HMSG_USB_DETECT_CONNECT");
#ifdef CONFIG_APP_ARD
            if(UserSetting->SetupPref.DelayPowerTime!=DELAY_POWEROFF_OFF)
                cardv_app_delay_poweroff_handler_timer(0);

            app_status.PowerType = APP_POWER_ADAPTER;
            app_status.cardv_auto_encode = 1;
            AppAppMgt_GetCurApp(&CurApp);
            if(CurApp->Id!= APP_MISC_LOGO)
            AppLibComSvcHcmgr_SendMsg(AMSG_STATE_BATTERY_STATE, 2, 0);//Update gui

        /*If APP is busy,treat it as USB DC.*/
        if (app_status.UsbPluginFlag == 0) {
                if (AppAppMgt_CheckBusy() || AppAppMgt_CheckIo()){
                    BspRefBotton_jamed_handler_en(1);
            app_status.UsbPluginFlag = 1;
            app_status.UsbChargeMode = 1;
            app_status.UsbTreatAsDC = 1;
            break;
                }
            }

            if (app_status.UsbPluginFlag == 0) {
                app_status.UsbPluginFlag = 1;
                app_status.UsbChargeMode = 0;
                BspRefBotton_jamed_handler_en(1);
                if (UserSetting->SetupPref.USBMode == USB_MODE_MSC) {
                    AppUtil_UsbChargeCheckingSet(1);
                } else {
                    AppAppMgt_GetCurApp(&CurApp);
                    CurApp->OnMessage(AMSG_CMD_USB_APP_START, Param1, Param2);
                }
            }
#else
            if (app_status.UsbPluginFlag == 0) {
                app_status.UsbPluginFlag = 1;
                if (AmbaUser_CheckIsUsbSlave()) {
                    AppAppMgt_GetCurApp(&CurApp);
                    CurApp->OnMessage(AMSG_CMD_USB_APP_START, Param1, Param2);
                }
            }
#endif
            break;
        case HMSG_USB_DETECT_REMOVE:
            AmbaPrintColor(GREEN,"HMSG_USB_DETECT_REMOVE");
#ifdef CONFIG_APP_ARD
            app_status.PowerType = APP_POWER_BATTERY;
            app_status.cardv_auto_encode = 0;
            AppUtil_CheckBatteryVoltageLevel();
            AppAppMgt_GetCurApp(&CurApp);
            if(CurApp->Id!= APP_MISC_LOGO)
            AppLibComSvcHcmgr_SendMsg(AMSG_STATE_BATTERY_STATE, 2, 0);//Update gui

            if(UserSetting->SetupPref.DelayPowerTime!=DELAY_POWEROFF_OFF && AppUtil_GetSystemTestMode() == TEST_MODE_OFF )
                cardv_app_delay_poweroff_handler_timer(1);
#endif
#if 1
            if (app_status.UsbPluginFlag == 1) {
                app_status.UsbPluginFlag = 0;
                app_status.UsbChargeMode = 0;
                if (UserSetting->SetupPref.USBMode == USB_MODE_MSC) {
                    AppUtil_UsbChargeCheckingSet(0);
                } else {
                    AppAppMgt_GetCurApp(&CurApp);
                    CurApp->OnMessage(AMSG_CMD_USB_APP_STOP, Param1, Param2);
                }
            }
#else
            if (app_status.UsbPluginFlag == 1) {
                app_status.UsbPluginFlag = 0;
                if (AmbaUser_CheckIsUsbSlave()) {
                    AppAppMgt_GetCurApp(&CurApp);
                    CurApp->OnMessage(AMSG_CMD_USB_APP_STOP, Param1, Param2);
                }
            }
#endif
            break;
        case HMSG_NAND0_CARD_INSERT:
        case HMSG_NAND1_CARD_INSERT:
        case HMSG_SD0_CARD_INSERT:
        case HMSG_SD1_CARD_INSERT:
            if (app_status.UsbStorageUnmount == 0) {
                ReturnValue = AppLibCard_GetCardId(Param1);
                if ((AppLibCard_GetActiveSlot() == NULL_SLOT) && (app_status.CardBusySlot == NO_BUSY_SLOT)) {
                    AppLibComSvcAsyncOp_CardInsert(Param1);
                } else if (AppAppMgt_CheckBusy() || AppAppMgt_CheckIo() || app_status.CardBusySlot != NO_BUSY_SLOT) {
                    /** set card insert blocked flag */
                    AppLibCard_StatusSetBlock(ReturnValue, 1);
                } else {
                    /** Remove card insert blocked flag */
                    AppLibCard_StatusSetBlock(ReturnValue, 0);
                    AppLibComSvcAsyncOp_CardInsert(Param1);
                }
            } else {
                AppAppMgt_GetCurApp(&CurApp);
                CurApp->OnMessage(Msg.MessageID, Param1, Param2);
            }
            break;
        case HMSG_NAND0_CARD_REMOVE:
        case HMSG_NAND1_CARD_REMOVE:
        case HMSG_SD0_CARD_REMOVE:
        case HMSG_SD1_CARD_REMOVE:
#ifdef CONFIG_APP_ARD
            AppHandler_CardRemove (Msg);
#else
            if (app_status.UsbStorageUnmount == 0) {
                int slot = 0;
                ReturnValue = AppLibCard_Remove(Param1);
                if (app_status.CardBusySlot == Param1) {
                    AmbaPrint("[App Handler] Remove set-rooting card, reset app_status.busy_slot !");
                    app_status.CardBusySlot = NO_BUSY_SLOT;
                    /** Check card insert block */
                    ReturnValue = AppLibCard_StatusCheckHighProrityBlock();
                    if (ReturnValue >= 0 && (ReturnValue < AppLibCard_GetActiveCardId())) {
                        /** send the blocked card insert if the card is higher prority than current ative card */
                        AppLibCard_SndCardInsertMsg(ReturnValue);
                    }
                } else if (ReturnValue == CARD_REMOVE_ACTIVE) {
                    slot = AppLibCard_GetPrimarySlot();
                    if (AppAppMgt_CheckBusy() || AppAppMgt_CheckIo() || app_status.CardBusySlot != NO_BUSY_SLOT) {
                        if (slot != -1) {
                            /** set card insert blocked flag */
                            ReturnValue = AppLibCard_GetCardId(slot);
                            AppLibCard_StatusSetBlock(ReturnValue, 1);
                        }
                        Msg.MessageID = AMSG_ERROR_CARD_REMOVED;
                    } else {
                        if (slot != -1) {
                            /** remove card insert blocked flag */
                            ReturnValue = AppLibCard_GetCardId(slot);
                            AppLibCard_StatusSetBlock(ReturnValue, 0);
                            AppLibComSvcAsyncOp_CardInsert(slot);
                        }
                        Msg.MessageID = AMSG_STATE_CARD_REMOVED;
                    }
                    AppAppMgt_GetCurApp(&CurApp);
                    CurApp->OnMessage(Msg.MessageID, Param1, Param2);
                }
            } else {
                AppAppMgt_GetCurApp(&CurApp);
                CurApp->OnMessage(Msg.MessageID, Param1, Param2);
            }
#endif
            break;
        case HMSG_STORAGE_BUSY:
            {
                int slot = AppLibCard_GetSlotFromChar(Param1);
                app_status.CardBusySlot = slot;
                AppLibCard_StatusSetRefreshing(app_status.CardBusySlot, 1);
                AppAppMgt_GetCurApp(&CurApp);
                CurApp->OnMessage(Msg.MessageID, Param1, Param2);
            }
            break;
        case HMSG_STORAGE_IDLE:
            if (Param2 == 1) {
                /** Resend from current app */
                AppAppMgt_GetCurApp(&CurApp);
                CurApp->OnMessage(Msg.MessageID, Param1, Param2);
            } else if (AppLibCard_CheckInsertingCard() == 0) {
                int slot = AppLibCard_GetSlotFromChar(Param1);
                if (app_status.CardBusySlot == slot) {
                    app_status.CardBusySlot = NO_BUSY_SLOT;
                    AppLibCard_SetInsertingSlot(0xFF);
                    AppLibCard_StatusSetRefreshing(slot, 0);

                    /**Check card insert block */
                    ReturnValue = AppLibCard_StatusCheckHighProrityBlock();
                    if (ReturnValue >= 0 && (ReturnValue < AppLibCard_GetActiveCardId())) {
                    /** send the blocked card insert if the card is higher prority than current ative card */
                        AppLibCard_SndCardInsertMsg(ReturnValue);
                    } else if (slot == AppLibCard_GetPrimarySlot()) {
                        /** send the storage idle when there are no higher prority blocked slot */

                        /** Send the storage idle to the current app */
                        AppAppMgt_GetCurApp(&CurApp);
                        CurApp->OnMessage(Msg.MessageID, Param1, Param2);
                    }
                } else {
                    AmbaPrint("[App Handler] The storage idle slot %d is not the same as busy slot %d!Something wrong!", slot, app_status.CardBusySlot);
                    app_status.CardBusySlot = NO_BUSY_SLOT;
                }
            } else {
                AppLibComSvcHcmgr_SendMsgNoWait(HMSG_STORAGE_IDLE, Param1, 0);
            }
            break;
        case HMSG_STOP_APP_COMPLETE:
            AmbaPrintColor(BLUE, "[App Handler] HMSG_STOP_APP_COMPLETE");
            AppLibComSvcHcmgr_DetachHandler();
            break;
        case HMSG_EXIT_HDL:
            AmbaPrint("[App Handler] Exit handler application!");
            AppExit = 1;
            break;
        case HMSG_VA_FCAR_DEPARTURE:
        case HMSG_VA_LOW_LIGHT:
        case HMSG_VA_MD_Y:
        case HMSG_VA_MD_AE:
        case HMSG_VA_LDW:
        case HMSG_VA_FCW:
            AppAppMgt_GetCurApp(&CurApp);
            CurApp->OnMessage(Msg.MessageID, Param1, Param2);
            break;
        case HMSG_USER_POWER_BUTTON:
#ifdef CONFIG_APP_ARD
            if(AppAppMgt_CheckBusy() == 0){
                AppUtil_SwitchApp(APP_MISC_LOGO);
            }else{
                AppAppMgt_GetCurApp(&CurApp);
                CurApp->OnMessage(Msg.MessageID, Param1, Param2);
            }
            break;
#else
            // TODO: Power saving mode
            AmbaPrint("[App Handler] Power off");
            AmbaPWC_Init();
            AmbaPWC_ForcePowerDownSequence();
#endif
            break;
#ifdef CONFIG_APP_ARD
        #if defined(CONFIG_APP_AMBA_LINK)
        case AMSG_EVENT_BOSS_HIBERNATION_DONE: /* hibernation img done from linux */
        #endif
        case AMSG_CMD_SW_REBOOT:/* system reboot */
            AmbaPrint("[App Handler] AMSG_CMD_SW_REBOOT !");
            AmbaSysSoftReset();
            break;
    /*Disable SNAP2 when test mode*/
    case HMSG_USER_SNAP2_BUTTON:
    case HMSG_USER_SNAP2_BUTTON_CLR:
        if(AppUtil_GetSystemTestMode() == TEST_MODE_OFF){
            AppAppMgt_GetCurApp(&CurApp);
            CurApp->OnMessage(Msg.MessageID, Param1, Param2);
        }
        break;
    case HMSG_MUXER_END_HAS_EMPTY_TRACK:
        EmptyTrackNum++;
        break;
    case HMSG_DCF_FILE_CLOSE:
        if(EmptyTrackNum >0){
            /*Send MSG to delete the empty track file*/
            int Rval = AppLibStorageAsyncOp_SndMsg(HMSG_EMPTY_TRACK_HDLR_DEL, DCIM_HDLR, EmptyTrackNum);
            if(Rval != OK){
                AmbaPrintColor(RED,"%s:%u failure. Rval=%d", __FUNCTION__, __LINE__,Rval);
            }else{
                EmptyTrackNum = 0;
            }
        }
        AppAppMgt_GetCurApp(&CurApp);
        CurApp->OnMessage(Msg.MessageID, Param1, Param2);
        break;
#endif
#if defined(CONFIG_APP_AMBA_LINK)
        case AMSG_EVENT_BOSS_BOOTED:
            {
                extern int AmbaBoss_ChangePriority(int NewPriority);
                extern void AmbaBoss_AdjustSchedulePeriod(UINT32 Period);

                AmbaPrintColor(MAGENTA, "===> [App Handler] AMSG_EVENT_BOSS_BOOTED");
                #ifdef CONFIG_BSP_TAROKO
                // TODO: use switch control wifi on/off
                AmpNetUtility_luExecNoResponse("/usr/local/share/script/wifi_start.sh fast");
                #endif
                /* Linux booted. */
                AppHandler_StartNetService();
                AmbaBoss_ChangePriority(APP_AMBA_BOSS_PRIORITY);
                AmbaBoss_AdjustSchedulePeriod(10);

                AppAppMgt_GetCurApp(&CurApp);
                CurApp->OnMessage(Msg.MessageID, Param1, Param2);
            }
            break;
        case AMSG_NETCTRL_SESSION_START:
            AppHandler_NetControlSession(1, Param1);
            AppAppMgt_GetCurApp(&CurApp);
            CurApp->OnMessage(Msg.MessageID, Param1, Param2);
            break;
        case AMSG_NETCTRL_SESSION_STOP:
            AppHandler_NetControlSession(0, 0);
            break;
#endif //CONFIG_APP_AMBA_LINK
        default:
            AppAppMgt_GetCurApp(&CurApp);
            CurApp->OnMessage(Msg.MessageID, Param1, Param2);
            break;
        }
    }
}

/**
 *  @brief Handler Exit Implementation
 *
 *  Handler Exit Implementation
 *
 *  @return >=0 success, <0 failure
 */
static int AppHandler_Exit(void)
{
    return AppLibComSvcHcmgr_SendMsg(HMSG_EXIT_HDL, 0, 0);
}

/**
 * @brief Car Cam Handler Object
 */
APPLIB_HCMGR_HANDLER_s Handler = {
    AppHandler_Entry,
    AppHandler_Exit
};

#ifdef CONFIG_APP_ARD
/**
 * @brief Card remove message handler
 *
 * Handle of sd card, etc,. remove message
 *
 * @return >=0 success, <0 failure
 */
int AppHandler_CardRemove (APP_MESSAGE_s Msg)
{
    APP_APP_s *CurApp = NULL;
    int ReturnValue = 0;
    UINT32 Param1 = Msg.MessageData[0];
    UINT32 Param2 = Msg.MessageData[1];
    DBGMSG("[App Handler] msgid = 0x%08X, param1 = 0x%08X, param2 = 0x%08X", Msg.MessageID, Param1, Param2);
    if (app_status.UsbStorageUnmount == 0) {
        int slot = 0;
        ReturnValue = AppLibCard_Remove (Param1);
        if (app_status.CardBusySlot == Param1) {
            AmbaPrint ("[App Handler] Remove set-rooting card, reset app_status.busy_slot !");
            app_status.CardBusySlot = NO_BUSY_SLOT;
            /** Check card insert block */
            ReturnValue = AppLibCard_StatusCheckHighProrityBlock();
            if (ReturnValue >= 0 && (ReturnValue < AppLibCard_GetActiveCardId())) {
                /** send the blocked card insert if the card is higher prority than current ative card */
                AppLibCard_SndCardInsertMsg (ReturnValue);
            }
        } else if (ReturnValue == CARD_REMOVE_ACTIVE) {
            slot = AppLibCard_GetPrimarySlot();
            if (AppAppMgt_CheckBusy() || AppAppMgt_CheckIo() || app_status.CardBusySlot != NO_BUSY_SLOT) {
                if (slot != -1) {
                    /** set card insert blocked flag */
                    ReturnValue = AppLibCard_GetCardId (slot);
                    AppLibCard_StatusSetBlock (ReturnValue, 1);
                }
                Msg.MessageID = AMSG_ERROR_CARD_REMOVED;
            } else {
                if (slot != -1) {
                    /** remove card insert blocked flag */
                    ReturnValue = AppLibCard_GetCardId (slot);
                    AppLibCard_StatusSetBlock (ReturnValue, 0);
                    AppLibComSvcAsyncOp_CardInsert (slot);
                }
                Msg.MessageID = AMSG_STATE_CARD_REMOVED;
            }
            AppAppMgt_GetCurApp (&CurApp);
            CurApp->OnMessage (Msg.MessageID, Param1, Param2);
        }
    } else {
        AppAppMgt_GetCurApp (&CurApp);
        CurApp->OnMessage (Msg.MessageID, Param1, Param2);
    }

    return 0;
}
#endif



