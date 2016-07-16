/**
  * @file src/app/apps/flow/rec/connectedcam/rec_cam_func.c
  *
  *  Functions of Sport Recorder (sensor) application
  *
  * History:
  *    2013/03/24 - [Annie Ting] created file
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
 * affiliates.  In the absence of such an agreement, you agree to promptly notify and
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


#include <apps/flow/rec/rec_cam.h>
#include <system/status.h>
#include <system/app_pref.h>
#include <system/app_util.h>
#include <system/ApplibSys_Sensor.h>
//#include <storage/ApplibStorage_Dmf.h>
#include <apps/flow/widget/menu/menu.h>
#include <apps/gui/resource/gui_settle.h>
#include <apps/flow/disp_param/disp_param_rec.h>
#include <AmbaROM.h>
#include <AmbaRTC.h>
#include <graphics/stamp/ApplibGraphics_Stamp.h>
#include <apps/flow/widget/menu/menu_video.h>
#include <framework/appmaintask.h>
#include <recorder/StillEnc.h>
#include <recorder/ApplibRecorder_ExtendEnc.h>
#include "AmbaGPIO.h"

#ifdef CONFIG_APP_ARD
#include <system/ApplibSys_Lcd.h>

extern int app_get_rsnd_storage_idle_msg(void);
extern void app_clr_rsnd_storage_idle_msg(void);
extern int AppLibEmptyTrackHandler_Init(void);

 int adas_cel_sta = 1;
 int adas_cel_set = 0;
 static int record_sta = 0;


#endif
/* return 0: system idle,  return 1: system busy */
static int rec_cam_system_is_busy(void)
{
    int Status = 1;

    if((rec_cam.RecCapState == REC_CAP_STATE_PREVIEW) &&
        (!APP_CHECKFLAGS(app_rec_cam.Flags, REC_CAM_FLAGS_MUXER_BUSY)) &&
        (!APP_CHECKFLAGS(app_rec_cam.Flags, REC_CAM_FLAGS_NETFIFO_BUSY))) {
        Status = 0;

        if (app_status.CurrEncMode == APP_VIDEO_ENC_MODE) {
            AmbaPrint("[%s] AppLibVideoEnc_EraseFifo",__func__);
            AppLibVideoEnc_EraseFifo();
            AppLibExtendEnc_UnInit();
        } else if (app_status.CurrEncMode == APP_STILL_ENC_MODE){
            AmbaPrint("[%s] AppLibStillEnc_EraseFifo",__func__);
            AppLibFormatMuxExif_End();
            AppLibStillEnc_EraseFifo();
        }
    }

    return Status;
}
#if defined(CONFIG_APP_AMBA_LINK)
#include <AmbaUtility.h>
#include <net/ApplibNet_JsonUtility.h>

static int NotifyNetFifoOfAppState(int State)
{
    int ReturnValue = 0;

    if (UserSetting->VideoPref.StreamType == STREAM_TYPE_RTSP) {
        ReturnValue = AppLibNetFifo_NotifyAppStateChange(State);
        if (ReturnValue == 0){
            if (State == AMP_NETFIFO_NOTIFY_STARTENC) {
                APP_ADDFLAGS(app_rec_cam.Flags, REC_CAM_FLAGS_NETFIFO_BUSY);
            }
        }
    }

    return ReturnValue;
}

static int SendJsonString(APPLIB_JSON_OBJECT *JsonObject)
{
    APPLIB_JSON_STRING *JsonString = NULL;
    char *ReplyString = NULL;

    AppLibNetJsonUtility_JsonObjectToJsonString(JsonObject, &JsonString);
    AppLibNetJsonUtility_GetString(JsonString, &ReplyString);
    if (ReplyString) {
        AppLibNetControl_ReplyToLnx(ReplyString, strlen(ReplyString));
    }
    AppLibNetJsonUtility_FreeJsonString(JsonString);

    return 0;
}

#endif /* CONFIG_APP_AMBA_LINK */

#if defined(CONFIG_APP_STAMP)
#ifdef CONFIG_APP_ARD
#define DATA_TIME_STAMP 1
#define CARDV_AREA_STAMP 0
#define CARDV_DRIVERID  1
static UINT8 stampAreaId[3] = {0};
static AMP_AREA_s stampArea[3] = {0};  // for encodec
static AMP_AREA_s stampOsd[3] = {0};   // for osd
static int rec_cam_calculate_stamp(APP_PREF_TIME_STAMP_e stampType, UINT32 encWidth, UINT32 encHeight)
{
    UINT32 BlankLeavingW = 0, BlankLeavingH = 0;
    UINT32 StringHeight = 0;
    GUI_REC_CAM_STAMP_UPDATE_INFO_s stampUpdateInfo = {0};

    /* Initialize */
    BlankLeavingW = encWidth / 15;
    BlankLeavingH = encHeight / 15;
    if(BlankLeavingH<54)
        BlankLeavingH= 54;
    StringHeight = 54;// encWidth / 20; // TBD
    //ModifiedStringW // TBD
#if DATA_TIME_STAMP
    {
     UINT32 DateWidth = AppLibGraph_GetStringWidth(GRAPH_CH_BLEND, BOBJ_DATE_STREAM_0, StringHeight);
     UINT32 HrMinWidth = AppLibGraph_GetStringWidth(GRAPH_CH_BLEND, BOBJ_TIME_H_M_STREAM_0, StringHeight);
     UINT32 SecWidth = AppLibGraph_GetStringWidth(GRAPH_CH_BLEND, BOBJ_TIME_S_STREAM_0, StringHeight);
     UINT32 TotalWidhth = HrMinWidth + SecWidth;
     UINT32 MaxWidth = (DateWidth >= TotalWidhth) ? DateWidth : TotalWidhth;

     if (stampType&CARDV_STAMP_DATE) {
         stampUpdateInfo.GuiObjId = BOBJ_DATE_STREAM_0;
         stampUpdateInfo.Left = 0;
         stampUpdateInfo.Top = 280;
         stampUpdateInfo.Width = DateWidth;
         stampUpdateInfo.Height = StringHeight;
         rec_cam.Gui(GUI_STAMP_UPDATE_SIZE, (UINT32)stampType, (UINT32)&stampUpdateInfo);
     }
     if (stampType&CARDV_STAMP_TIME) {
        stampUpdateInfo.GuiObjId = BOBJ_TIME_H_M_STREAM_0;
        stampUpdateInfo.Left = MaxWidth - TotalWidhth;
        stampUpdateInfo.Top = 280 + StringHeight;
        stampUpdateInfo.Width = HrMinWidth;
        stampUpdateInfo.Height = StringHeight;
        rec_cam.Gui(GUI_STAMP_UPDATE_SIZE, (UINT32)stampType, (UINT32)&stampUpdateInfo);
        stampUpdateInfo.GuiObjId = BOBJ_TIME_S_STREAM_0;
        stampUpdateInfo.Left = MaxWidth - SecWidth;
        stampUpdateInfo.Top = 280 + StringHeight;
        stampUpdateInfo.Width = SecWidth;
        stampUpdateInfo.Height = StringHeight;
        rec_cam.Gui(GUI_STAMP_UPDATE_SIZE, (UINT32)stampType, (UINT32)&stampUpdateInfo);
     }
     if (stampType&CARDV_STAMP_DATE ||stampType&CARDV_STAMP_TIME) {

        /* Update position and size */
        stampArea[0].X = encWidth - BlankLeavingW - MaxWidth;
        stampArea[0].Y = encHeight - BlankLeavingH - (StringHeight<<1);
        stampArea[0].Width = (MaxWidth%32) ? (((MaxWidth>>5)+1)<<5) : MaxWidth;
        stampArea[0].Height = (StringHeight<<1);

        stampOsd[0].X = 0;
        stampOsd[0].Y = 280;
        stampOsd[0].Width = stampArea[0].Width;
        stampOsd[0].Height = StringHeight<<1;
     }
    }
#endif
#if CARDV_AREA_STAMP
    {
    UINT32 CardvArea1Width = AppLibGraph_GetStringWidth(GRAPH_CH_BLEND, BOBJ_POLICE_ADMIN_ARER1_STREAM_0, StringHeight);
    UINT32 CardvArea2Width = AppLibGraph_GetStringWidth(GRAPH_CH_BLEND, BOBJ_POLICE_ADMIN_ARER2_STREAM_0, StringHeight);
    UINT32 CardvArea3Width = AppLibGraph_GetStringWidth(GRAPH_CH_BLEND, BOBJ_POLICE_ADMIN_ARER3_STREAM_0, StringHeight);
    UINT32 CardvArea4Width = AppLibGraph_GetStringWidth(GRAPH_CH_BLEND, BOBJ_POLICE_ADMIN_ARER4_STREAM_0, StringHeight);
    UINT32 CardvArea5Width = AppLibGraph_GetStringWidth(GRAPH_CH_BLEND, BOBJ_POLICE_ADMIN_ARER5_STREAM_0, StringHeight);

    UINT32 MaxWidth = (CardvArea1Width >= CardvArea2Width) ? CardvArea1Width : CardvArea2Width;
    MaxWidth = (MaxWidth >= CardvArea3Width) ? MaxWidth : CardvArea3Width;
    MaxWidth = (MaxWidth >= CardvArea4Width) ? MaxWidth : CardvArea4Width;
    MaxWidth = (MaxWidth >= CardvArea5Width) ? MaxWidth : CardvArea5Width;


    if (stampType&CARDV_STAMP_AREA) {
        stampUpdateInfo.GuiObjId = BOBJ_CARDV_ADMIN_ARER1_STREAM_0;
        stampUpdateInfo.Left = 0;
        stampUpdateInfo.Top = 0;
        stampUpdateInfo.Width = MaxWidth;
        stampUpdateInfo.Height = StringHeight;
        rec_cam.Gui(GUI_STAMP_UPDATE_SIZE, (UINT32)stampType, (UINT32)&stampUpdateInfo);
        stampUpdateInfo.GuiObjId = BOBJ_CARDV_ADMIN_ARER2_STREAM_0;
        stampUpdateInfo.Left = 0;
        stampUpdateInfo.Top =  StringHeight;
        stampUpdateInfo.Width = MaxWidth;
        stampUpdateInfo.Height = StringHeight;
        rec_cam.Gui(GUI_STAMP_UPDATE_SIZE, (UINT32)stampType, (UINT32)&stampUpdateInfo);
        stampUpdateInfo.GuiObjId = BOBJ_CARDV_ADMIN_ARER3_STREAM_0;
        stampUpdateInfo.Left = 0;
        stampUpdateInfo.Top = StringHeight*2;
        stampUpdateInfo.Width = MaxWidth;
        stampUpdateInfo.Height = StringHeight;
        rec_cam.Gui(GUI_STAMP_UPDATE_SIZE, (UINT32)stampType, (UINT32)&stampUpdateInfo);

        stampUpdateInfo.GuiObjId = BOBJ_CARDV_ADMIN_ARER4_STREAM_0;
        stampUpdateInfo.Left = 0;
        stampUpdateInfo.Top = StringHeight*3;
        stampUpdateInfo.Width = MaxWidth;
        stampUpdateInfo.Height = StringHeight;
        rec_cam.Gui(GUI_STAMP_UPDATE_SIZE, (UINT32)stampType, (UINT32)&stampUpdateInfo);

        stampUpdateInfo.GuiObjId = BOBJ_CARDV_ADMIN_ARER5_STREAM_0;
        stampUpdateInfo.Left = 0;
        stampUpdateInfo.Top = StringHeight*4;
        stampUpdateInfo.Width = MaxWidth;
        stampUpdateInfo.Height = StringHeight;
        rec_cam.Gui(GUI_STAMP_UPDATE_SIZE, (UINT32)stampType, (UINT32)&stampUpdateInfo);

        /* Update position and size */
        stampArea[1].X = encWidth - BlankLeavingW - MaxWidth;
        stampArea[1].Y = BlankLeavingH;
        stampArea[1].Width = (MaxWidth%32) ? (((MaxWidth>>5)+1)<<5) : MaxWidth;
        stampArea[1].Height = StringHeight*5;

        stampOsd[1].X = 0;
        stampOsd[1].Y = 0;
        stampOsd[1].Width = stampArea[1].Width;
        stampOsd[1].Height = StringHeight*5;
      }

    }
#endif
#if CARDV_DRIVERID
    {
     UINT32 CardvDriveridWidth = AppLibGraph_GetStringWidth(GRAPH_CH_BLEND, BOBJ_CARDV_DRIVERID_STREAM_0, StringHeight);

     if (stampType&CARDV_STAMP_DRIVER_ID) {

        stampUpdateInfo.GuiObjId = BOBJ_CARDV_DRIVERID_STREAM_0;
        stampUpdateInfo.Left = 0;
        stampUpdateInfo.Top = 390;
        stampUpdateInfo.Width = CardvDriveridWidth;
        stampUpdateInfo.Height = StringHeight;
        rec_cam.Gui(GUI_STAMP_UPDATE_SIZE, (UINT32)stampType, (UINT32)&stampUpdateInfo);

      /* Update position and size */
         stampArea[2].X = encWidth - BlankLeavingW - CardvDriveridWidth;
         stampArea[2].Y = encHeight - BlankLeavingH;
         stampArea[2].Width = (CardvDriveridWidth%32) ? (((CardvDriveridWidth>>5)+1)<<5) : CardvDriveridWidth;
         stampArea[2].Height = StringHeight;

         stampOsd[2].X = 0;;
         stampOsd[2].Y = 390;
         stampOsd[2].Width = stampArea[2].Width;
         stampOsd[2].Height = StringHeight;
      }

    }
#endif

    return 0;
}
static int rec_cam_encode_stamp(UINT8 updateFlag)
{
    APPLIB_GRAPHIC_STAMP_BUF_CONFIG_s stampConfig = {0};
    // UserSetting->VideoStampPref.StampTime=1;
    // UserSetting->VideoStampPref.StampDate=1;
    APP_PREF_TIME_STAMP_e stampType = (UserSetting->VideoStampPref.StampDate|(UserSetting->VideoStampPref.StampTime<<1)|(UserSetting->VideoStampPref.StampDriverId<<2));

    if (stampType == CARDV_STAMP_OFF) {
        return 0;
    }

    /* Add a new stamp area*/
    if (app_status.CurrEncMode == APP_STILL_ENC_MODE) {
#ifdef CONFIG_APP_ARD
        APPLIB_GRAPHIC_STAMP_BUF_CONFIG_s stampConfig = {0};
        APPLIB_STILLENC_STAMP_SETTING_s stillStamp = {0};
#else
        APPLIB_GRAPHIC_STAMP_BUF_CONFIG_s stampConfig = {0};
        APPLIB_STILLENC_STAMP_SETTING_s stillStamp = {0};
#endif
        /* Blending */
#if DATA_TIME_STAMP
        if (stampType&CARDV_STAMP_DATE||stampType&CARDV_STAMP_TIME) {
            stampAreaId[0] = AppLibGraph_AddStampArea(stampOsd[0], ENCODE_FORMAT_YUV422);
            AppLibStamp_GetBlendBuf(stampAreaId[0], &stampConfig);
            stampConfig.OffsetX = stampArea[0].X;
            stampConfig.OffsetY = stampArea[0].Y;
            memcpy((void*)&stillStamp.StampAreaInfo[0], (void*)&stampConfig, sizeof(APPLIB_GRAPHIC_STAMP_BUF_CONFIG_s));
            stillStamp.StampAreaEn[0] = 1;
        }
#endif

#if CARDV_AREA_STAMP
        if (stampType&CARDV_STAMP_AREA) {
            stampAreaId[1] = AppLibGraph_AddStampArea(stampOsd[1], ENCODE_FORMAT_YUV422);
            AppLibStamp_GetBlendBuf(stampAreaId[1], &stampConfig);
            stampConfig.OffsetX = stampArea[1].X;
            stampConfig.OffsetY = stampArea[1].Y;
            memcpy((void*)&stillStamp.StampAreaInfo[1], (void*)&stampConfig, sizeof(APPLIB_GRAPHIC_STAMP_BUF_CONFIG_s));
            stillStamp.StampAreaEn[1] = 1;
        }
#endif
#if CARDV_DRIVERID
        if (stampType&CARDV_STAMP_DRIVER_ID) {
            stampAreaId[2] = AppLibGraph_AddStampArea(stampOsd[2], ENCODE_FORMAT_YUV422);
            AppLibStamp_GetBlendBuf(stampAreaId[2], &stampConfig);
            stampConfig.OffsetX = stampArea[2].X;
            stampConfig.OffsetY = stampArea[2].Y;
            memcpy((void*)&stillStamp.StampAreaInfo[2], (void*)&stampConfig, sizeof(APPLIB_GRAPHIC_STAMP_BUF_CONFIG_s));
            stillStamp.StampAreaEn[2] = 1;
        }
#endif
        switch (UserSetting->PhotoPref.PhotoCapMode) {
            case PHOTO_CAP_MODE_PES:
                AppLibStillEnc_SingleContCapRegisterStampCB(stillStamp);
                break;
            case PHOTO_CAP_MODE_BURST:
                AppLibStillEnc_BurstCapRegisterStampCB(stillStamp);
                break;
            case PHOTO_CAP_MODE_PRECISE:
            default:
                AppLibStillEnc_SingleCapRegisterStampCB(stillStamp);
                break;
        }
        return 0;
    }
#if DATA_TIME_STAMP
    {
        if (stampType&CARDV_STAMP_DATE||stampType&CARDV_STAMP_TIME) {
            AMP_VIDEOENC_BLEND_INFO_s BlendInfo;
            if (!updateFlag) {
               stampAreaId[0] = AppLibGraph_AddStampArea(stampOsd[0], ENCODE_FORMAT_YUV422);
            } else {
               AppLibGraph_UpdateStampArea(stampAreaId[0], stampOsd[0], ENCODE_FORMAT_YUV422);
            }
            AppLibStamp_GetBlendBuf(stampAreaId[0], &stampConfig);
            BlendInfo.Enable = 1;
            BlendInfo.BufferID = stampAreaId[0];  // Unit Test only use one blend area, we choose ID = 0
            BlendInfo.OffsetX = stampArea[0].X;
            BlendInfo.OffsetY = stampArea[0].Y;
            BlendInfo.Pitch = ((stampArea[0].Width%32) ? (((stampArea[0].Width>>5)+1)<<5) : (stampArea[0].Width));
            BlendInfo.Width = ((stampArea[0].Width%32) ? (((stampArea[0].Width>>5)+1)<<5) : (stampArea[0].Width));
            BlendInfo.Height = stampArea[0].Height;
            BlendInfo.YAddr = stampConfig.YAddr;
            BlendInfo.UVAddr = stampConfig.UVAddr;
            BlendInfo.AlphaYAddr = stampConfig.AlphaYAddr;
            BlendInfo.AlphaUVAddr = stampConfig.AlphaUVAddr;
            AppLibVideoEnc_EncodeStamp(0, &BlendInfo);
        }
    }
#endif
#if CARDV_AREA_STAMP
    {
        if (stampType&CARDV_STAMP_AREA) {
        AMP_VIDEOENC_BLEND_INFO_s BlendInfo;

        if (!updateFlag) {
             stampAreaId[1] = AppLibGraph_AddStampArea(stampOsd[1], ENCODE_FORMAT_YUV422);
         } else {
             AppLibGraph_UpdateStampArea(stampAreaId[1], stampOsd[1], ENCODE_FORMAT_YUV422);
         }

        AppLibStamp_GetBlendBuf(stampAreaId[1], &stampConfig);

        BlendInfo.Enable = 1;
        BlendInfo.BufferID = stampAreaId[1];  // Unit Test only use one blend area, we choose ID = 1
        BlendInfo.OffsetX = stampArea[1].X;
        BlendInfo.OffsetY = stampArea[1].Y;
        BlendInfo.Pitch = ((stampArea[1].Width%32) ? (((stampArea[1].Width>>5)+1)<<5) : (stampArea[1].Width));
        BlendInfo.Width = ((stampArea[1].Width%32) ? (((stampArea[1].Width>>5)+1)<<5) : (stampArea[1].Width));
        BlendInfo.Height = stampArea[1].Height;
        BlendInfo.YAddr = stampConfig.YAddr;
        BlendInfo.UVAddr = stampConfig.UVAddr;
        BlendInfo.AlphaYAddr = stampConfig.AlphaYAddr;
        BlendInfo.AlphaUVAddr = stampConfig.AlphaUVAddr;
        AppLibVideoEnc_EncodeStamp(0, &BlendInfo);
        }
    }
#endif
#if CARDV_DRIVERID
    {
        if (stampType&CARDV_STAMP_DRIVER_ID) {
        AMP_VIDEOENC_BLEND_INFO_s BlendInfo;

        if (!updateFlag) {
             stampAreaId[2] = AppLibGraph_AddStampArea(stampOsd[2], ENCODE_FORMAT_YUV422);
         } else {
             AppLibGraph_UpdateStampArea(stampAreaId[2], stampOsd[2], ENCODE_FORMAT_YUV422);
         }

        AppLibStamp_GetBlendBuf(stampAreaId[2], &stampConfig);

        BlendInfo.Enable = 1;
        BlendInfo.BufferID = stampAreaId[2];  // Unit Test only use one blend area, we choose ID = 2
        BlendInfo.OffsetX = stampArea[2].X;
        BlendInfo.OffsetY = stampArea[2].Y;
        BlendInfo.Pitch = ((stampArea[2].Width%32) ? (((stampArea[2].Width>>5)+1)<<5) : (stampArea[2].Width));
        BlendInfo.Width = ((stampArea[2].Width%32) ? (((stampArea[2].Width>>5)+1)<<5) : (stampArea[2].Width));
        BlendInfo.Height = stampArea[2].Height;
        BlendInfo.YAddr = stampConfig.YAddr;
        BlendInfo.UVAddr = stampConfig.UVAddr;
        BlendInfo.AlphaYAddr = stampConfig.AlphaYAddr;
        BlendInfo.AlphaUVAddr = stampConfig.AlphaUVAddr;
        AppLibVideoEnc_EncodeStamp(0, &BlendInfo);
        }
    }

#endif
    return 0;
}

static void rec_cam_enable_adas(void)
{
     if (UserSetting->SetupPref.ldws_mode_onoff||UserSetting->SetupPref.fcws_mode_onoff)
    {
        AppLibVideo_Ecl_ADAS_Init(95);
        AppLibVideo_Ecl_ADAS_Enable();

        AppLibVideo_Set_Radar_Calibration_Mode(UserSetting->SetupPref.adas_auto_cal_onoff);
        AppLibVideo_Set_Radar_Offset(UserSetting->SetupPref.radar_cal_offset); 

        AppLibVideo_Set_Adas_Ldws_OnOff(UserSetting->SetupPref.ldws_mode_onoff);
        AppLibVideo_Set_Adas_Fcws_OnOff(UserSetting->SetupPref.fcws_mode_onoff);

        AppLibVideo_Set_Adas_Hmws_OnOff(UserSetting->SetupPref.hmws_mode_onoff);
        AppLibVideo_Set_Adas_Fcmrs_OnOff(UserSetting->SetupPref.fcmr_mode_onoff);

        switch(UserSetting->SetupPref.adas_alarm_dis)
        {
            case 0:
                // AppLibVideo_ECL_ADAS_Set_Fcwd_ttc(1800);
                AppLibVideo_Set_Adas_Sen_Level(0);
                break;

            case 1:
                 AppLibVideo_Set_Adas_Sen_Level(1);
                 // AppLibVideo_ECL_ADAS_Set_Fcwd_ttc(1500);
                break;

            case 2:
                // AppLibVideo_ECL_ADAS_Set_Fcwd_ttc(1200);
                AppLibVideo_Set_Adas_Sen_Level(2);
                break;

            default:
                AppLibVideo_Set_Adas_Sen_Level(1);
               // AppLibVideo_ECL_ADAS_Set_Fcwd_ttc(1800);
               break;
        }
    } 
    else
    {
         AppLibVideo_Ecl_ADAS_Disable();
    }
}

static int rec_cam_setup_stamp(void)
{
    int ReturnValue = 0;

    APP_PREF_TIME_STAMP_e TimeStampType = CARDV_STAMP_OFF;
    AMBA_RTC_TIME_SPEC_u TimeSpec = {0};
    UserSetting->VideoStampPref.StampTime=1;
    UserSetting->VideoStampPref.StampDate=1;
    /* Function Check */
    TimeStampType = (UserSetting->VideoStampPref.StampDate|(UserSetting->VideoStampPref.StampTime<<1)|(UserSetting->VideoStampPref.StampDriverId<<2));

    if (TimeStampType == CARDV_STAMP_OFF) {
        AmbaPrint("<rec_cam_setup_stamp> cardv stamp off!");
        return ReturnValue;
    }

    /* Initialize */
    rec_cam.Gui(GUI_STAMP_SET_MODE, 0, 0);
    rec_cam.Gui(GUI_STAMP_HIDE, TimeStampType, 0);
    rec_cam.Gui(GUI_FLUSH, 0, 0);
#ifdef CONFIG_APP_ARD
#ifndef ENABLE_VA_STAMP
    /* Get time setting */
    AmbaRTC_GetSystemTime(AMBA_TIME_STD_TAI, &TimeSpec);

    /* Update date */
    if (TimeStampType&CARDV_STAMP_DATE) {
        GUI_REC_CAM_STAMP_DATE_s stampDate = {0};
        stampDate.Year = (TimeSpec.Calendar.Year > 2044) ? 2014 : TimeSpec.Calendar.Year;
        stampDate.Month = TimeSpec.Calendar.Month;
        stampDate.Day = TimeSpec.Calendar.Day;
        rec_cam.Gui(GUI_STAMP_UPDATE_DATE, (UINT32)&stampDate, 0);
    }

    /* Update time */
    if (TimeStampType&CARDV_STAMP_TIME) {
        GUI_REC_CAM_STAMP_TIME_s stampTime = {0};
        stampTime.Hour = TimeSpec.Calendar.Hour;
        stampTime.Minute = TimeSpec.Calendar.Minute;
        stampTime.Second = TimeSpec.Calendar.Second;
        rec_cam.Gui(GUI_STAMP_UPDATE_TIME, (UINT32)&stampTime, 0);
    }
#else
    if (TimeStampType&CARDV_STAMP_TIME)
    {
        WCHAR StampStrTimeHandM[6] = {'0','0','0','m','/','\0'};
        WCHAR StampStrTimeHandS[4] = {'0','0','0','\0'};
        UINT8 streamCount = 0;

        StampStrTimeHandM[0] = 'x';
        StampStrTimeHandM[1] = 'x';
        StampStrTimeHandM[2] = 'x';
        StampStrTimeHandM[3] = 'x';

        for (streamCount = 0; streamCount < 6; streamCount++) {
            AppLibGraph_UpdateStringContext(AppPref_GetLanguageID(), STR_OSD_BLEND_TIME_H_M, StampStrTimeHandM);
            AppLibGraph_UpdateString(GRAPH_CH_BLEND, BOBJ_TIME_H_M_STREAM_0 + streamCount, STR_OSD_BLEND_TIME_H_M);
        }

        StampStrTimeHandS[0] = 'x';
        StampStrTimeHandS[1] = 'x';
        StampStrTimeHandS[2] = 'x';

        for (streamCount = 0; streamCount < 6; streamCount++) {
            AppLibGraph_UpdateStringContext(AppPref_GetLanguageID(), STR_OSD_BLEND_TIME_S, StampStrTimeHandS);
            AppLibGraph_UpdateString(GRAPH_CH_BLEND, BOBJ_TIME_S_STREAM_0 + streamCount, STR_OSD_BLEND_TIME_S);
        }
    }

    if (TimeStampType&CARDV_STAMP_DATE)
    {
        UINT16 StampStrDate[7] = {'-','-','-','-','-','-','\0'};
        UINT8 streamCount = 0;

        for (streamCount = 0; streamCount < 6; streamCount++) {
            AppLibGraph_UpdateStringContext(AppPref_GetLanguageID(), STR_OSD_BLEND_DATE, StampStrDate);
            AppLibGraph_UpdateString(GRAPH_CH_BLEND, BOBJ_DATE_STREAM_0 + streamCount, STR_OSD_BLEND_DATE);
        }
    }
#endif
#else
    /* Get time setting */
    AmbaRTC_GetSystemTime(AMBA_TIME_STD_TAI, &TimeSpec);

    /* Update date */
    if (TimeStampType&CARDV_STAMP_DATE) {
        GUI_REC_CAM_STAMP_DATE_s stampDate = {0};
        stampDate.Year = (TimeSpec.Calendar.Year > 2044) ? 2016 : TimeSpec.Calendar.Year;
        stampDate.Month = TimeSpec.Calendar.Month;
        stampDate.Day = TimeSpec.Calendar.Day;
        rec_cam.Gui(GUI_STAMP_UPDATE_DATE, (UINT32)&stampDate, 0);
    }
    /* Update time */
    if (TimeStampType&CARDV_STAMP_TIME) {
        GUI_REC_CAM_STAMP_TIME_s stampTime = {0};
        stampTime.Hour = TimeSpec.Calendar.Hour;
        stampTime.Minute = TimeSpec.Calendar.Minute;
        stampTime.Second = TimeSpec.Calendar.Second;
        rec_cam.Gui(GUI_STAMP_UPDATE_TIME, (UINT32)&stampTime, 0);
    }
#endif

    if (TimeStampType&CARDV_STAMP_DRIVER_ID) {
        char driver_id[10] = {0};//{'Y','R','8','8','8','\0'};
        int i = 0;
        for(i=0;i<10;i++){
            driver_id[i] = UserSetting->SetupPref.driver_id[i];
        }

        rec_cam.Gui(GUI_STAMP_UPDATE_DRIVERID, &driver_id, 0);
    }

    /* Enable stamp on OSD buffer */
    rec_cam.Gui(GUI_STAMP_SHOW, TimeStampType, 0);

    /* Modify position and size */
    if (app_status.CurrEncMode == APP_STILL_ENC_MODE) {
        APPLIB_SENSOR_STILLCAP_CONFIG_s *StillCapConfigData;
        StillCapConfigData = AppLibSysSensor_GetPjpegConfig(AppLibStillEnc_GetPhotoPjpegCapMode(), AppLibStillEnc_GetPhotoPjpegConfigId());
        rec_cam_calculate_stamp(TimeStampType, StillCapConfigData->FullviewWidth, StillCapConfigData->FullviewHeight);
    } else {
        APPLIB_SENSOR_VIDEO_ENC_CONFIG_s *VideoEncConfigData;
        VideoEncConfigData = AppLibSysSensor_GetVideoConfig(AppLibVideoEnc_GetSensorVideoRes());
        rec_cam_calculate_stamp(TimeStampType, VideoEncConfigData->EncodeWidth, VideoEncConfigData->EncodeHeight);
    }
    rec_cam.Gui(GUI_FLUSH, 0, 0);

    /* Encode */
    rec_cam_encode_stamp(0);


    return ReturnValue;
}

static int rec_cam_update_stamp(void)
{
    int ReturnValue = 0;

    APP_PREF_TIME_STAMP_e TimeStampType = CARDV_STAMP_OFF;
    AMBA_RTC_TIME_SPEC_u TimeSpec = {0};
    /* Function Check */
    TimeStampType = (UserSetting->VideoStampPref.StampDate|(UserSetting->VideoStampPref.StampTime<<1)|(UserSetting->VideoStampPref.StampDriverId<<2));


    if (TimeStampType == CARDV_STAMP_OFF) {
        return ReturnValue;
    }
    /* Get time setting */
    AmbaRTC_GetSystemTime(AMBA_TIME_STD_TAI, &TimeSpec);

    /* Update date */
    if (TimeStampType&CARDV_STAMP_DATE) {
        GUI_REC_CAM_STAMP_DATE_s stampDate = {0};
        stampDate.Year = (TimeSpec.Calendar.Year > 2044) ? 2014 : TimeSpec.Calendar.Year;
        stampDate.Month = TimeSpec.Calendar.Month;
        stampDate.Day = TimeSpec.Calendar.Day;
        rec_cam.Gui(GUI_STAMP_UPDATE_DATE, (UINT32)&stampDate, 0);
    }

    /* Update time */
    if (TimeStampType&CARDV_STAMP_TIME) {
        GUI_REC_CAM_STAMP_TIME_s stampTime = {0};
        stampTime.Hour = TimeSpec.Calendar.Hour;
        stampTime.Minute = TimeSpec.Calendar.Minute;
        stampTime.Second = TimeSpec.Calendar.Second;
        rec_cam.Gui(GUI_STAMP_UPDATE_TIME, (UINT32)&stampTime, 0);
    }

    if (TimeStampType&CARDV_STAMP_DRIVER_ID) {
        char driver_id[10] = {0};//{'Y','R','8','8','8','\0'};
        int i = 0;
        for(i=0;i<10;i++){
            driver_id[i] = UserSetting->SetupPref.driver_id[i];
        }

        rec_cam.Gui(GUI_STAMP_UPDATE_DRIVERID, &driver_id, 0);
    }

    /* Enable stamp on OSD buffer */
    rec_cam.Gui(GUI_STAMP_SHOW, TimeStampType, 0);

    /* Modify position and size */
    if (app_status.CurrEncMode == APP_VIDEO_ENC_MODE) {
        APPLIB_SENSOR_VIDEO_ENC_CONFIG_s *VideoEncConfigData;
        VideoEncConfigData = AppLibSysSensor_GetVideoConfig(AppLibVideoEnc_GetSensorVideoRes());
        rec_cam_calculate_stamp(TimeStampType, VideoEncConfigData->EncodeWidth, VideoEncConfigData->EncodeHeight);
    }

    return 0;
}

static int rec_cam_stop_stamp(void)
{
    int ReturnValue = 0;

    APP_PREF_TIME_STAMP_e TimeStampType = CARDV_STAMP_OFF;
    AMP_VIDEOENC_BLEND_INFO_s BlendInfo;

    TimeStampType = (UserSetting->VideoStampPref.StampDate|(UserSetting->VideoStampPref.StampTime<<1)|(UserSetting->VideoStampPref.StampDriverId<<2));

    /* Function Check */
    if (TimeStampType == CARDV_STAMP_OFF) {
        AmbaPrint("<rec_cam_stop_stamp> stamp off!");
        return ReturnValue;
    }
    if (TimeStampType&CARDV_STAMP_DATE||TimeStampType&CARDV_STAMP_TIME) {
        BlendInfo.Enable = 0;
        BlendInfo.BufferID = stampAreaId[0];  // Unit Test only use one blend area, we choose ID = 0
        AppLibVideoEnc_EncodeStamp(0, &BlendInfo);
    }
    if (TimeStampType&CARDV_STAMP_AREA) {
        BlendInfo.Enable = 0;
        BlendInfo.BufferID = stampAreaId[1];  // Unit Test only use one blend area, we choose ID = 1
        AppLibVideoEnc_EncodeStamp(0, &BlendInfo);
    }
    if (TimeStampType&CARDV_STAMP_DRIVER_ID) {
        BlendInfo.Enable = 0;
        BlendInfo.BufferID = stampAreaId[2];  // Unit Test only use one blend area, we choose ID = 2
        AppLibVideoEnc_EncodeStamp(0, &BlendInfo);
    }

    rec_cam.Gui(GUI_STAMP_HIDE, TimeStampType, 0);
    rec_cam.Gui(GUI_FLUSH, 0, 0);

    return 0;
}

#else
static UINT8 stampAreaId = 0;
static AMP_AREA_s stampArea = {0};  // for encodec
static AMP_AREA_s stampOsd = {0};   // for osd
static int rec_cam_calculate_stamp(APP_PREF_TIME_STAMP_e stampType, UINT32 encWidth, UINT32 encHeight)
{
    UINT32 BlankLeavingW = 0, BlankLeavingH = 0;
    UINT32 StringHeight = 0;
    GUI_REC_CAM_STAMP_UPDATE_INFO_s stampUpdateInfo = {0};

    /* Initialize */
    BlankLeavingW = encWidth / 15;
    BlankLeavingH = encHeight / 15;
    StringHeight = 54;// encWidth / 20; // TBD
    //ModifiedStringW // TBD

    /* Date */
    if (stampType == STAMP_DATE) {
        UINT32 DateWidth = AppLibGraph_GetStringWidth(GRAPH_CH_BLEND, BOBJ_DATE_STREAM_0, StringHeight);

        stampUpdateInfo.GuiObjId = BOBJ_DATE_STREAM_0;
        stampUpdateInfo.Left = 0;
        stampUpdateInfo.Top = 230;
        stampUpdateInfo.Width = DateWidth;
        stampUpdateInfo.Height = StringHeight;
        rec_cam.Gui(GUI_STAMP_UPDATE_SIZE, (UINT32)stampType, (UINT32)&stampUpdateInfo);

        /* Update position and size */
        stampArea.X = encWidth - BlankLeavingW - DateWidth;
        stampArea.Y = encHeight - BlankLeavingH - StringHeight;
        stampArea.Width = (DateWidth%32) ? (((DateWidth>>5)+1)<<5) : DateWidth;
        stampArea.Height = StringHeight;

        stampOsd.X = 0;
        stampOsd.Y = 258;
        stampOsd.Width = stampArea.Width;
        stampOsd.Height = StringHeight;
    } else if (stampType == STAMP_TIME){
        UINT32 HrMinWidth = AppLibGraph_GetStringWidth(GRAPH_CH_BLEND, BOBJ_TIME_H_M_STREAM_0, StringHeight);
        UINT32 SecWidth = AppLibGraph_GetStringWidth(GRAPH_CH_BLEND, BOBJ_TIME_S_STREAM_0, StringHeight);
        UINT32 TotalWidth = 0;

        stampUpdateInfo.GuiObjId = BOBJ_TIME_H_M_STREAM_0;
        stampUpdateInfo.Left = 0;
        stampUpdateInfo.Top = 230;
        stampUpdateInfo.Width = HrMinWidth;
        stampUpdateInfo.Height = StringHeight;
        rec_cam.Gui(GUI_STAMP_UPDATE_SIZE, (UINT32)stampType,(UINT32)&stampUpdateInfo);
        stampUpdateInfo.GuiObjId = BOBJ_TIME_S_STREAM_0;
        stampUpdateInfo.Left = HrMinWidth;
        stampUpdateInfo.Top = 230;
        stampUpdateInfo.Width = SecWidth;
        stampUpdateInfo.Height = StringHeight;
        rec_cam.Gui(GUI_STAMP_UPDATE_SIZE, (UINT32)stampType, (UINT32)&stampUpdateInfo);

        /* Update position and size */
        stampArea.X = encWidth - BlankLeavingW - SecWidth - HrMinWidth;
        stampArea.Y = encHeight - BlankLeavingH - StringHeight;
        TotalWidth = HrMinWidth + SecWidth;
        stampArea.Width = (TotalWidth%32) ? (((TotalWidth>>5)+1)<<5) : TotalWidth;
        stampArea.Height = StringHeight;

        stampOsd.X = 0;
        stampOsd.Y = 258;
        stampOsd.Width = stampArea.Width;
        stampOsd.Height = StringHeight;
    } else {
        UINT32 DateWidth = AppLibGraph_GetStringWidth(GRAPH_CH_BLEND, BOBJ_DATE_STREAM_0, StringHeight);
        UINT32 HrMinWidth = AppLibGraph_GetStringWidth(GRAPH_CH_BLEND, BOBJ_TIME_H_M_STREAM_0, StringHeight);
        UINT32 SecWidth = AppLibGraph_GetStringWidth(GRAPH_CH_BLEND, BOBJ_TIME_S_STREAM_0, StringHeight);
        UINT32 TotalWidhth = HrMinWidth + SecWidth;
        UINT32 MaxWidth = (DateWidth >= TotalWidhth) ? DateWidth : TotalWidhth;

        stampUpdateInfo.GuiObjId = BOBJ_DATE_STREAM_0;
        stampUpdateInfo.Left = 0;
        stampUpdateInfo.Top = 230;
        stampUpdateInfo.Width = DateWidth;
        stampUpdateInfo.Height = StringHeight;
        rec_cam.Gui(GUI_STAMP_UPDATE_SIZE, stampType, (UINT32)&stampUpdateInfo);
        stampUpdateInfo.GuiObjId = BOBJ_TIME_H_M_STREAM_0;
        stampUpdateInfo.Left = MaxWidth - TotalWidhth;
        stampUpdateInfo.Top = 230 + StringHeight;
        stampUpdateInfo.Width = HrMinWidth;
        stampUpdateInfo.Height = StringHeight;
        rec_cam.Gui(GUI_STAMP_UPDATE_SIZE, stampType, (UINT32)&stampUpdateInfo);
        stampUpdateInfo.GuiObjId = BOBJ_TIME_S_STREAM_0;
        stampUpdateInfo.Left = MaxWidth - SecWidth;
        stampUpdateInfo.Top = 230 + StringHeight;
        stampUpdateInfo.Width = SecWidth;
        stampUpdateInfo.Height = StringHeight;
        rec_cam.Gui(GUI_STAMP_UPDATE_SIZE, stampType, (UINT32)&stampUpdateInfo);

        /* Update position and size */
        stampArea.X = encWidth - BlankLeavingW - MaxWidth;
        stampArea.Y = encHeight - BlankLeavingH - (StringHeight<<1);
        stampArea.Width = (MaxWidth%32) ? (((MaxWidth>>5)+1)<<5) : MaxWidth;
        stampArea.Height = (StringHeight<<1);

        stampOsd.X = 0;
        stampOsd.Y = 258;
        stampOsd.Width = stampArea.Width;
        stampOsd.Height = StringHeight<<1;
    }

    return 0;
}

static int rec_cam_encode_stamp(UINT8 updateFlag)
{
    APPLIB_GRAPHIC_STAMP_BUF_CONFIG_s stampConfig = {0};
    /* Add a new stamp area*/
    if (app_status.CurrEncMode == APP_STILL_ENC_MODE) {
        stampAreaId = AppLibGraph_AddStampArea(stampOsd, ENCODE_FORMAT_YUV422);
    } else {
        if (!updateFlag) {
            stampAreaId = AppLibGraph_AddStampArea(stampOsd, ENCODE_FORMAT_YUV422);
        } else {
            AppLibGraph_UpdateStampArea(stampAreaId, stampOsd, ENCODE_FORMAT_YUV422);
        }
    }

    AppLibStamp_GetBlendBuf(stampAreaId, &stampConfig);

    /* Encode */
    if (app_status.CurrEncMode == APP_STILL_ENC_MODE) {
        APPLIB_GRAPHIC_STAMP_BUF_CONFIG_s stampConfig = {0};
        APPLIB_STILLENC_STAMP_SETTING_s stillStamp = {0};

        /* Blending */
        stampAreaId = AppLibGraph_AddStampArea(stampOsd, ENCODE_FORMAT_YUV422);
        AppLibStamp_GetBlendBuf(stampAreaId, &stampConfig);
        stampConfig.OffsetX = stampArea.X;
        stampConfig.OffsetY = stampArea.Y;
        memcpy((void*)&stillStamp, (void*)&stampConfig, sizeof(APPLIB_STILLENC_STAMP_SETTING_s));
        switch (UserSetting->PhotoPref.PhotoCapMode) {
            case PHOTO_CAP_MODE_PES:
                AppLibStillEnc_SingleContCapRegisterStampCB(stillStamp);
                break;
            case PHOTO_CAP_MODE_BURST:
                AppLibStillEnc_BurstCapRegisterStampCB(stillStamp);
                break;
            case PHOTO_CAP_MODE_PRECISE:
            default:
                AppLibStillEnc_SingleCapRegisterStampCB(stillStamp);
                break;
        }
    } else {
        AMP_VIDEOENC_BLEND_INFO_s BlendInfo;
        BlendInfo.Enable = 1;
        BlendInfo.BufferID = stampAreaId;  // Unit Test only use one blend area, we choose ID = 0
        BlendInfo.OffsetX = stampArea.X;
        BlendInfo.OffsetY = stampArea.Y;
        BlendInfo.Pitch = ((stampArea.Width%32) ? (((stampArea.Width>>5)+1)<<5) : (stampArea.Width));
        BlendInfo.Width = ((stampArea.Width%32) ? (((stampArea.Width>>5)+1)<<5) : (stampArea.Width));
        BlendInfo.Height = stampArea.Height;
        BlendInfo.YAddr = stampConfig.YAddr;
        BlendInfo.UVAddr = stampConfig.UVAddr;
        BlendInfo.AlphaYAddr = stampConfig.AlphaYAddr;
        BlendInfo.AlphaUVAddr = stampConfig.AlphaUVAddr;
        AppLibVideoEnc_EncodeStamp(0, &BlendInfo);
    }
    return 0;
}
static int rec_cam_setup_stamp(void)
{
    int ReturnValue = 0;

    APP_PREF_TIME_STAMP_e TimeStampType = STAMP_OFF;
    AMBA_RTC_TIME_SPEC_u TimeSpec = {0};

    /* Function Check */
    if (app_status.CurrEncMode == APP_STILL_ENC_MODE) {
        TimeStampType = UserSetting->PhotoPref.PhotoTimeStamp;
    } else if (app_status.CurrEncMode == APP_VIDEO_ENC_MODE) {
        TimeStampType = UserSetting->VideoPref.VideoDateTimeStamp;
    }
    if (TimeStampType == STAMP_OFF) {
        AmbaPrint("<rec_cam_setup_stamp> stamp off!");
        return ReturnValue;
    }

    /* Initialize */
    rec_cam.Gui(GUI_STAMP_SET_MODE, 0, 0);
    rec_cam.Gui(GUI_STAMP_HIDE, TimeStampType, 0);
    rec_cam.Gui(GUI_FLUSH, 0, 0);

    /* Get time setting */
    AmbaRTC_GetSystemTime(AMBA_TIME_STD_TAI, &TimeSpec);

    /* Update date */
    if ((TimeStampType == STAMP_DATE_TIME) || (TimeStampType == STAMP_DATE)) {
        GUI_REC_CAM_STAMP_DATE_s stampDate = {0};
        stampDate.Year = (TimeSpec.Calendar.Year > 2044) ? 2014 : TimeSpec.Calendar.Year;
        stampDate.Month = TimeSpec.Calendar.Month;
        stampDate.Day = TimeSpec.Calendar.Day;
        rec_cam.Gui(GUI_STAMP_UPDATE_DATE, (UINT32)&stampDate, 0);
    }

    /* Update time */
    if ((TimeStampType == STAMP_DATE_TIME) || (TimeStampType == STAMP_TIME)) {
        GUI_REC_CAM_STAMP_TIME_s stampTime = {0};
        stampTime.Hour = TimeSpec.Calendar.Hour;
        stampTime.Minute = TimeSpec.Calendar.Minute;
        stampTime.Second = TimeSpec.Calendar.Second;
        rec_cam.Gui(GUI_STAMP_UPDATE_TIME, (UINT32)&stampTime, 0);
    }

    /* Enable stamp on OSD buffer */
    rec_cam.Gui(GUI_STAMP_SHOW, TimeStampType, 0);

    /* Modify position and size */
    if (app_status.CurrEncMode == APP_STILL_ENC_MODE) {
        APPLIB_SENSOR_STILLCAP_CONFIG_s *StillCapConfigData;
        StillCapConfigData = AppLibSysSensor_GetPjpegConfig(AppLibStillEnc_GetPhotoPjpegCapMode(), AppLibStillEnc_GetPhotoPjpegConfigId());
        rec_cam_calculate_stamp(TimeStampType, StillCapConfigData->FullviewWidth, StillCapConfigData->FullviewHeight);
    } else {
        APPLIB_SENSOR_VIDEO_ENC_CONFIG_s *VideoEncConfigData;
        VideoEncConfigData = AppLibSysSensor_GetVideoConfig(AppLibVideoEnc_GetSensorVideoRes());
        rec_cam_calculate_stamp(TimeStampType, VideoEncConfigData->EncodeWidth, VideoEncConfigData->EncodeHeight);
    }
    rec_cam.Gui(GUI_FLUSH, 0, 0);

    /* Encode */
    rec_cam_encode_stamp(0);


    return ReturnValue;
}

static int rec_cam_update_stamp(void)
{
    int ReturnValue = 0;

    APP_PREF_TIME_STAMP_e TimeStampType = STAMP_OFF;
    AMBA_RTC_TIME_SPEC_u TimeSpec = {0};
    /* Function Check */
    if (app_status.CurrEncMode == APP_VIDEO_ENC_MODE) {
        TimeStampType = UserSetting->VideoPref.VideoDateTimeStamp;
    }

    if (TimeStampType == STAMP_OFF) {
        AmbaPrint("<rec_cam_update_stamp> stamp off!");
        return ReturnValue;
    }
    /* Get time setting */
    AmbaRTC_GetSystemTime(AMBA_TIME_STD_TAI, &TimeSpec);

    /* Update date */
    if ((TimeStampType == STAMP_DATE_TIME) || (TimeStampType == STAMP_DATE)) {
        GUI_REC_CAM_STAMP_DATE_s stampDate = {0};
        stampDate.Year = (TimeSpec.Calendar.Year > 2044) ? 2014 : TimeSpec.Calendar.Year;
        stampDate.Month = TimeSpec.Calendar.Month;
        stampDate.Day = TimeSpec.Calendar.Day;
        rec_cam.Gui(GUI_STAMP_UPDATE_DATE, (UINT32)&stampDate, 0);
    }

    /* Update time */
    if ((TimeStampType == STAMP_DATE_TIME) || (TimeStampType == STAMP_TIME)) {
        GUI_REC_CAM_STAMP_TIME_s stampTime = {0};
        stampTime.Hour = TimeSpec.Calendar.Hour;
        stampTime.Minute = TimeSpec.Calendar.Minute;
        stampTime.Second = TimeSpec.Calendar.Second;
        rec_cam.Gui(GUI_STAMP_UPDATE_TIME, (UINT32)&stampTime, 0);
    }

    /* Enable stamp on OSD buffer */
    rec_cam.Gui(GUI_STAMP_SHOW, TimeStampType, 0);

    /* Modify position and size */
    if (app_status.CurrEncMode == APP_VIDEO_ENC_MODE) {
        APPLIB_SENSOR_VIDEO_ENC_CONFIG_s *VideoEncConfigData;
        VideoEncConfigData = AppLibSysSensor_GetVideoConfig(AppLibVideoEnc_GetSensorVideoRes());
        rec_cam_calculate_stamp(TimeStampType, VideoEncConfigData->EncodeWidth, VideoEncConfigData->EncodeHeight);
    }

    return 0;
}

static int rec_cam_stop_stamp(void)
{
    int ReturnValue = 0;

    APP_PREF_TIME_STAMP_e TimeStampType = STAMP_OFF;
    AMP_VIDEOENC_BLEND_INFO_s BlendInfo;

    if (app_status.CurrEncMode == APP_VIDEO_ENC_MODE) {
        TimeStampType = UserSetting->VideoPref.VideoDateTimeStamp;
    }

    /* Function Check */
    if (TimeStampType == STAMP_OFF) {
        AmbaPrint("<rec_cam_stop_stamp> stamp off!");
        return ReturnValue;
    }
    BlendInfo.Enable = 0;
    BlendInfo.BufferID = stampAreaId;  // Unit Test only use one blend area, we choose ID = 0
    AppLibVideoEnc_EncodeStamp(0, &BlendInfo);

    rec_cam.Gui(GUI_STAMP_HIDE, TimeStampType, 0);
    rec_cam.Gui(GUI_FLUSH, 0, 0);

    return 0;
}
#endif
#endif

static int rec_cam_init(void)
{
    int ReturnValue = 0;

    /** Initialize the vin. */
    AppLibSysVin_Init();

    /** Initialize the video recorder. */
    AppLibVideoEnc_Init();

    /** Initialize the still recorder. */
    AppLibStillEnc_Init();

    /** Initialize the audio recorder. */
    AppLibAudioEnc_Init();
    /** Initialize the extend recorder. */
    AppLibExtendEnc_Init();
    rec_cam.QuickViewFileType = MEDIA_TYPE_UNKNOWN;
    rec_cam.RecCapState = REC_CAP_STATE_RESET;
    #if defined(CONFIG_APP_AMBA_LINK)
    NotifyNetFifoOfAppState(AMP_NETFIFO_NOTIFY_RELEASE);
    #endif

#ifdef CONFIG_APP_ARD
    rec_cam.PrepareToReplayRecord = REC_CAP_PREPARE_TO_REPLAY_NONE;
#endif
    return ReturnValue;
}

static int rec_cam_muxer_end_eventrecord(void)
{
    int ReturnValue = 0;
#ifdef CONFIG_APP_ARD
#ifndef CONFIG_APP_EVENT_OVERLAP
    AppLibFormatMuxMp4_Close_EventRecord();
    rec_cam.Func(REC_CAM_MUXER_END, 0, 0);
#endif
#endif
#ifdef CONFIG_APP_ARD
    if (!APP_CHECKFLAGS(app_rec_cam.Flags, REC_CAM_FLAGS_MUXER_ERROR)) {
        APP_REMOVEFLAGS(app_rec_cam.Flags, REC_CAM_FLAGS_MUXER_ERROR);
        UserSetting->VideoPref.UnsavingEvent = 0;
        AppPref_Save();
    }

    {
    int number;
    number = AppLibStorageDmf_GetFileAmount(APPLIB_DCF_MEDIA_VIDEO, EVENTRECORD_HDLR);
    if(number < MAX_EVENT_FILE)
        rec_cam.Gui(GUI_EVENT_NUM_HIDE,0,0);
    }
    rec_cam.Gui(GUI_EVENT_ICON_HIDE, 0, 0);
    rec_cam.Gui(GUI_FLUSH, 0, 0);
#ifndef CONFIG_APP_EVENT_OVERLAP
    if((app_status.parkingmode_on == 1)&&(app_status.PowerType == APP_POWER_BATTERY))
        AppLibComSvcHcmgr_SendMsg(HMSG_USER_POWER_BUTTON, 0, 0);
    if((app_status.parkingmode_on == 1)&&(app_status.PowerType == APP_POWER_ADAPTER)){
        app_status.parkingmode_on = 0;
        AppLibComSvcHcmgr_SendMsg(HMSG_USER_RECORD_BUTTON, 0, 0);
       }
#endif
#endif
    return ReturnValue;
}
#ifdef CONFIG_APP_ARD
static int rec_cam_muxer_open_eventrecord(void)
{
    int ReturnValue = 0;
    UserSetting->VideoPref.UnsavingEvent = 1;
    AppPref_Save();

    return ReturnValue;
}
#endif
static int rec_cam_muxer_reach_limit_eventrecord(int param1)
{
    int ReturnValue = 0;
#ifdef CONFIG_APP_ARD
#ifndef CONFIG_APP_EVENT_OVERLAP
    AppLibFormatMuxMp4_Event_End();
#endif
#endif
    return ReturnValue;
}

static int rec_cam_eventrecord_start(void)
{
    int ReturnValue = 0;
    int PreEventTime = 10;//to set how many seconds before event to record

#ifdef CONFIG_APP_ARD
    int number = AppLibStorageDmf_GetFileObjAmount(APPLIB_DCF_MEDIA_VIDEO, EVENTRECORD_HDLR);
    if(number >= MAX_EVENT_FILE){
        rec_cam.Gui(GUI_EVENT_NUM_SHOW, MAX_EVENT_FILE, 0);
        AmbaPrintColor(RED,"Event file amount > MAX_EVENT_FILE(%d)!,skip Event REC!",MAX_EVENT_FILE);
        if(app_status.parkingmode_on == 1) {
            rec_cam.Func(REC_CAM_WARNING_MSG_SHOW_START, GUI_WARNING_EVENT_FULL, 1);
            AppLibAudioDec_Beep(BEEP_ERROR,0);
            AmbaKAL_TaskSleep(500);
            rec_cam.Func(REC_CAM_WARNING_MSG_SHOW_STOP, GUI_WARNING_EVENT_FULL, 0);
            AppLibAudioDec_Beep(BEEP_ERROR,0);
            AmbaKAL_TaskSleep(500);
            rec_cam.Func(REC_CAM_WARNING_MSG_SHOW_START, GUI_WARNING_EVENT_FULL, 1);
            AppLibAudioDec_Beep(BEEP_ERROR,0);
            AmbaKAL_TaskSleep(500);
            rec_cam.Func(REC_CAM_WARNING_MSG_SHOW_STOP, GUI_WARNING_EVENT_FULL, 0);
            AmbaKAL_TaskSleep(500);
            rec_cam.Func(REC_CAM_WARNING_MSG_SHOW_START, GUI_WARNING_EVENT_FULL, 1);
            AmbaKAL_TaskSleep(500);
            AppLibComSvcHcmgr_SendMsg(HMSG_USER_POWER_BUTTON, 0, 0);
        } else {
            rec_cam.Func(REC_CAM_WARNING_MSG_SHOW_START, GUI_WARNING_EVENT_FULL, 0);
        }
        return ReturnValue;
    }
#endif

#ifdef CONFIG_APP_ARD
#ifndef CONFIG_APP_EVENT_OVERLAP
    AppLibFormat_SetSplit_EventRecord(30000);//30 secs
#endif
#endif

#ifdef CONFIG_APP_ARD
#ifdef CONFIG_APP_EVENT_OVERLAP
    if (AppLibFormatMuxMp4_GetEventStatus()==0){
#else
    if (AppLibFormat_GetEventStatus()==0){
#endif
#endif
#ifdef CONFIG_APP_ARD
#ifndef CONFIG_APP_EVENT_OVERLAP
        rec_cam.MuxerNum ++;
#endif
#else
        rec_cam.MuxerNum ++;
#endif

#ifdef CONFIG_APP_ARD
        #ifndef CONFIG_ECL_GUI
        rec_cam.Gui(GUI_EVENT_NUM_SHOW, number + 1, 0);
        rec_cam.Func(REC_CAR_VIDEO_EVENT_FILE_NUM_UPDATE,0,0);
        rec_cam.Gui(GUI_EVENT_ICON_SHOW, 0, 0);
        rec_cam.Gui(GUI_FLUSH, 0, 0);
        #else
        rec_cam.Gui(GUI_EVENT_ICON_SHOW, 0, 0);
        rec_cam.Func(REC_CAR_VIDEO_EVENT_FILE_NUM_UPDATE,0,0);
        rec_cam.Gui(GUI_FLUSH, 0, 0);
        #endif

        AppLibAudioDec_Beep(BEEP_ERROR,0);

        if(MOTION_DETECT_ON == UserSetting->MotionDetectPref.MotionDetect) {
             if((rec_cam.MotionDetectStatus == REC_CAP_MOTION_DETECT_START)&&(rec_cam.MotionDetectRecordRemainTime != 0)){
#ifdef CONFIG_APP_EVENT_OVERLAP
                rec_cam.MotionDetectRecordRemainTime +=60;
#else
                rec_cam.MotionDetectRecordRemainTime +=30;
#endif
              }
         }
#endif
#ifdef CONFIG_APP_ARD
    }
#endif
#ifdef CONFIG_APP_ARD
#ifdef CONFIG_APP_EVENT_OVERLAP
    AppLibFormatMuxMp4_EventRecord_event();
#else
    AppLibFormatMuxMp4_StartOnRecording_EventRecord(PreEventTime);
#endif
#else
    AppLibFormatMuxMp4_StartOnRecording_EventRecord(PreEventTime);
#endif

    return ReturnValue;
}
#ifdef CONFIG_APP_ARD
static void rec_cam_gps_status_timer_handler(int eid)
{
    static int gps_status_pre = 0xff;
	
    if (eid == TIMER_UNREGISTER) {
        return;
    }
    if(gps_status_pre != app_status.gps_status){
    rec_cam.Gui(GUI_GPS_STATUS_ICON_SHOW, app_status.gps_status, 0);
    rec_cam.Gui(GUI_FLUSH, 0, 0);
    gps_status_pre = app_status.gps_status ;
    }
}
#if 1

static void rec_cam_show_data_timer_handler(int eid)
{
    rec_cam.Gui(GUI_UPDATE_DATE_TIME,0, 0);
    rec_cam.Gui(GUI_DATE_TIME_SHOW, 0, 0);
    rec_cam.Gui(GUI_FLUSH, 0, 0);	
    if(UserSetting->SetupPref.adas_auto_cal_onoff==1)
    {
        if(AppLibVideo_Get_Radar_Offset_e()<=0.1f)
        {
            UserSetting->SetupPref.adas_auto_cal_onoff=0;
            UserSetting->SetupPref.radar_cal_offset=AppLibVideo_Get_Radar_Offset();
            AppPref_Save();
            rec_cam_func(REC_CAM_CALIBRATION_ICN,UserSetting->SetupPref.adas_auto_cal_onoff,0);
        }
    }

}
#endif	


#endif

static void rec_cam_record_led_timer_handler(int eid)
{
    static int record_led_status = 0;
    
    if (eid == TIMER_UNREGISTER) {
        AmbaGPIO_ConfigOutput(GPIO_PIN_10, AMBA_GPIO_LEVEL_HIGH);
        return;
    }
    if(record_led_status == 0)
    {
        record_led_status = 1;
        AmbaGPIO_ConfigOutput(GPIO_PIN_10,AMBA_GPIO_LEVEL_LOW);
    }
    else
    {
        record_led_status = 0;
        AmbaGPIO_ConfigOutput(GPIO_PIN_10, AMBA_GPIO_LEVEL_HIGH);
    }

}

static int rec_cam_record_led(int param)
{
    int ReturnValue = 0;
    if(param==1)
    {
        AppLibComSvcTimer_Register(TIMER_2HZ, rec_cam_record_led_timer_handler);
    }
    else
    {
        AppLibComSvcTimer_Unregister(TIMER_2HZ, rec_cam_record_led_timer_handler);
    }
    return ReturnValue;
}

static int rec_cam_start(void)
{
    int ReturnValue = 0;
    UserSetting->SystemPref.SystemMode = APP_MODE_ENC;
    /** Set menus */
    AppMenu_Reset();

#ifndef CONFIG_ECL_GUI
    if (app_status.CurrEncMode == APP_VIDEO_ENC_MODE)
        AppMenu_RegisterTab(MENU_VIDEO);
    else
        AppMenu_RegisterTab(MENU_PHOTO);
#else
    AppMenu_RegisterTab(MENU_ADAS_CALI);    
    AppMenu_RegisterTab(MENU_ADAS_FUNC);
    AppMenu_RegisterTab(MENU_ADAS_ALARM_DIS);
    AppMenu_RegisterTab(MENU_RECORDER_SETTTING);    
    AppMenu_RegisterTab(MENU_VERSION);
    AppMenu_RegisterTab(MENU_DEFAULT);	
   /* AppMenu_RegisterTab(MENU_ADAS);
    AppMenu_RegisterTab(MENU_SETUP);
    AppMenu_RegisterTab(MENU_VIDEO); */   
#endif
    
    #if defined (CONFIG_APP_AMBA_LINK)
    //AppMenu_EnableItem(MENU_VIDEO, MENU_VIDEO_STREAMS_TYPE);
    #endif
    #ifdef CONFIG_ECL_GUI
    AppLibVideoEnc_SetSecStreamW(1280);
    AppLibVideoEnc_SetSecStreamH(720);
    #endif
    // ToDo: need to remove to handler when iav completes the dsp cmd queue mechanism
   
    AppLibGraph_Init();
    
    /** Start the liveview. */
    if (app_status.CurrEncMode == APP_VIDEO_ENC_MODE) {
        /**Calculate second stream timescale by main stream time scale*/
        APPLIB_SENSOR_VIDEO_ENC_CONFIG_s *VideoEncConfigData;
        VideoEncConfigData = AppLibSysSensor_GetVideoConfig(AppLibVideoEnc_GetSensorVideoRes());
        if ((VideoEncConfigData->EncNumerator % 25000) ==0) {
            AppLibVideoEnc_SetSecStreamTimeScale(25000);
            AppLibVideoEnc_SetSecStreamTick(1000);
        } else {
            AppLibVideoEnc_SetSecStreamTimeScale(30000);
            AppLibVideoEnc_SetSecStreamTick(1001);
        }

        /* Video preview. */
        AppLibVideoEnc_LiveViewSetup();
        rec_cam.Func(REC_CAM_CHANGE_DISPLAY, 0, 0);
        AppLibVideoEnc_LiveViewStart();
    } else {
        /* Photo preview. */
        AppLibStillEnc_LiveViewSetup();
        rec_cam.Func(REC_CAM_CHANGE_DISPLAY, 0, 0);
        AppLibStillEnc_LiveViewStart();
    }

    /** set free space threshold*/
    AppLibCard_SetThreahold(FREESPACE_THRESHOLD); /**<set card check threshold*/
    AppLibMonitorStorage_SetThreshold(FREESPACE_THRESHOLD);/**<set storage monitor threshold*/
    /** inital storage async op task*/
    AppLibStorageAsyncOp_Init();

    /*register loop enc handler*/
    AppLibLoopEnc_Init();
#ifdef CONFIG_APP_ARD
    AppLibEmptyTrackHandler_Init();
#endif

    /** init frame handler task for ADAS */
    AppLibVideoAnal_FrmHdlr_Init();
    AppLibVideoAnal_TriAHdlr_Init();

    /** set motion detect record time.*/
#ifdef CONFIG_APP_ARD
     rec_cam.MotionDetectRecordRemainTime = 0;
#endif

    /** set split setting to 60 sec.*/
#ifdef CONFIG_APP_ARD
    if(UserSetting->VideoPref.video_split_rec_time == VIDEO_SPLIT_REC_1_MIN ) {
        AppLibVideoEnc_SetSplit(VIDEO_SPLIT_TIME_60_SECONDS);
    } 
    else if (UserSetting->VideoPref.video_split_rec_time == VIDEO_SPLIT_REC_3_MIN ) {
        AppLibVideoEnc_SetSplit(VIDEO_SPLIT_TIME_180_SECONDS);
    } else if (UserSetting->VideoPref.video_split_rec_time == VIDEO_SPLIT_REC_5_MIN ) {
        AppLibVideoEnc_SetSplit(VIDEO_SPLIT_TIME_300_SECONDS);
    } else if (UserSetting->VideoPref.video_split_rec_time == VIDEO_SPLIT_REC_OFF ) {
        AppLibVideoEnc_SetSplit(VIDEO_SPLIT_OFF);
    }
#if defined CONFIG_G_SENSOR_ST_LIS3DE||defined CONFIG_G_SENSOR_MIRA_DA380
    rec_cam.Func(REC_CAM_VIDEO_SET_GSENSOR_SENSITIVITY, UserSetting->GSensorSentivityPref.Gsensor_sensitivity, 0);
#endif
    if(MOTION_DETECT_ON == UserSetting->MotionDetectPref.MotionDetect) {
        APPLIB_MD_CFG_t Config = {0};
        Config.Method = APPLIB_MD_AE;
        Config.RoiData[0].Location.X = 0;
        Config.RoiData[0].Location.Y = 0;
        Config.RoiData[0].Location.W = 12;
        Config.RoiData[0].Location.H = 8;
        Config.RoiData[0].MDSensitivity = ADAS_SL_MEDIUM;
        ReturnValue = AppLibVideoAnal_MD_Init(0, &Config);
        AmbaPrintColor(YELLOW, "--------AppLibVideoAnal_MD_Init return = %d", ReturnValue);
#ifdef CONFIG_APP_ARD
        rec_cam.MotionDetectStatus = REC_CAP_MOTION_DETECT_STOP;
        /** set motion detect record time.*/
        rec_cam.MotionDetectRecordRemainTime = 0;
#endif
    }

    if(AppUtil_GetRsndStorageIdleMsg()) {
        AppUtil_SetRsndStorageIdleMsg(0);
        rec_cam.Func(REC_CAM_CARD_STORAGE_IDLE, 0, 0);
    }
#else
    AppLibVideoEnc_SetSplit(VIDEO_SPLIT_TIME_60_SECONDS);
#endif

#ifdef CONFIG_ECL_GUI
    
    rec_cam_enable_adas();
#endif
    return ReturnValue;
}

static int rec_cam_stop(void)
{
    int ReturnValue = 0;

    if (APP_CHECKFLAGS(app_rec_cam.Flags, REC_CAM_FLAGS_SELFTIMER_RUN)) {
        ReturnValue = rec_cam.Func(REC_CAM_SELFTIMER_STOP, 0, 0);
    }

    /* Stop the warning message, because the warning could need to be updated. */
    rec_cam.Func(REC_CAM_WARNING_MSG_SHOW_STOP, 0, 0);

    APP_REMOVEFLAGS(app_rec_cam.Flags, REC_CAM_FLAGS_MUXER_ERROR);
    AppLibVideo_Ecl_ADAS_Disable();
    /** Stop the liveview. */
    if (app_status.CurrEncMode == APP_VIDEO_ENC_MODE) {
        AppLibVideoEnc_LiveViewStop();
    } else {
        AppLibStillEnc_LiveViewStop();
        AppLibStillEnc_LiveViewDeInit();
#ifdef CONFIG_APP_ARD
        /**delete still encode pipe when change to video mode*/
        AppLibStillEnc_DeletePipe();
#endif
    }
#ifdef CONFIG_APP_ARD
    if(MOTION_DETECT_ON == UserSetting->MotionDetectPref.MotionDetect) {
        ReturnValue = AppLibVideoAnal_MD_Disable();
        AmbaPrintColor(YELLOW, "--------AppLibVideoAnal_MD_Disable return = %d", ReturnValue);

        ReturnValue = AppLibVideoAnal_MD_DeInit();
        AmbaPrintColor(YELLOW, "--------AppLibVideoAnal_MD_DeInit return = %d", ReturnValue);
    }
#endif

    /* Close the menu or dialog. */
    AppWidget_Off(WIDGET_ALL, WIDGET_HIDE_SILENT);
    APP_REMOVEFLAGS(app_rec_cam.GFlags, APP_AFLAGS_POPUP);

    /* Disable the vout. */
    AppLibDisp_DeactivateWindow(DISP_CH_FCHAN, AppLibDisp_GetWindowId(DISP_CH_FCHAN, 0));
    AppLibDisp_DeactivateWindow(DISP_CH_DCHAN, AppLibDisp_GetWindowId(DISP_CH_DCHAN, 0));
    AppLibDisp_ChanStop(DISP_CH_FCHAN);

#ifndef CONFIG_APP_ARD
    AppLibComSvcTimer_Unregister(TIMER_1HZ, rec_cam_gps_status_timer_handler);
    app_status.CurrEncMode = APP_VIDEO_ENC_MODE;
#endif

#ifdef CONFIG_ECL_GUI
    AmbaPrint("rec_cam_show_data_timer_handler Unregister\n");
    AppLibComSvcTimer_Unregister(TIMER_1HZ, rec_cam_show_data_timer_handler);
    rec_cam.Gui(GUI_DATE_TIME_HIDE, 0, 0);
#endif

    /** Hide GUI */
    rec_cam.Gui(GUI_HIDE_ALL, 0, 0);
    rec_cam.Gui(GUI_FLUSH, 0, 0);

    return ReturnValue;
}

static int rec_cam_do_liveview_state(void)
{
    int ReturnValue = 0;

    rec_cam.RecCapState = REC_CAP_STATE_PREVIEW;
    if (APP_CHECKFLAGS(app_rec_cam.GFlags, APP_AFLAGS_READY)) {
        /* Application is ready. */
        if (APP_CHECKFLAGS(app_rec_cam.GFlags, APP_AFLAGS_BUSY)) {
            /* Application is busy. */
            if (APP_CHECKFLAGS(app_rec_cam.Flags, REC_CAM_FLAGS_STILL_CAPTURE)) {
                /* Capture complete. */
                APP_REMOVEFLAGS(app_rec_cam.Flags, REC_CAM_FLAGS_STILL_CAPTURE);
                /* The application is busy, if the muxer is not idle mode.*/
                #if defined(CONFIG_APP_AMBA_LINK)||defined(CONFIG_APP_ARD)
                if (!rec_cam_system_is_busy()) {
                #else
                if (!APP_CHECKFLAGS(app_rec_cam.Flags, REC_CAM_FLAGS_MUXER_BUSY)) {
                #endif
                    APP_REMOVEFLAGS(app_rec_cam.GFlags, APP_AFLAGS_BUSY);
                    APP_REMOVEFLAGS(app_rec_cam.Flags, REC_CAM_FLAGS_BLOCK_MENU);
#if defined(CONFIG_APP_AMBA_LINK)
                    if (APP_CHECKFLAGS(app_rec_cam.Flags,REC_CAM_FLAGS_CAPTURE_FROM_NETCTRL)) {
                        rec_cam.Func(REC_CAM_NETCTRL_CAPTURE_DONE, 0, 0);
                    }

                    if (APP_CHECKFLAGS(app_rec_cam.Flags,REC_CAM_FLAGS_CAPTURE_ON_VF)) {
                        DBGMSGc2(CYAN,"[rec_cam] <do_liveview_state> clear REC_CAR_DV_FLAGS_CAPTURE_ON_VF");
                        APP_REMOVEFLAGS(app_rec_cam.Flags,REC_CAM_FLAGS_CAPTURE_ON_VF);
                        app_status.CurrEncMode = APP_VIDEO_ENC_MODE;
                    }
#endif
                    /* To excute the functions that system block them when the Busy flag is enabled. */
                    AppUtil_BusyCheck(0);
                    if (!APP_CHECKFLAGS(app_rec_cam.GFlags, APP_AFLAGS_READY)) {
                        return ReturnValue;/**<  App switched out*/
                    }
                }
            } else {
                /* Stop recording clip. */
                #if defined(CONFIG_APP_AMBA_LINK)||defined(CONFIG_APP_ARD)
                if (!rec_cam_system_is_busy()) {
                #else
                if (!APP_CHECKFLAGS(app_rec_cam.Flags, REC_CAM_FLAGS_MUXER_BUSY)) {
                #endif
                    APP_REMOVEFLAGS(app_rec_cam.GFlags, APP_AFLAGS_BUSY);

                    #if defined(CONFIG_APP_AMBA_LINK)
                    if (APP_CHECKFLAGS(rec_cam.NetCtrlFlags, APP_NETCTRL_FLAGS_VF_STOP_DONE)) {
                        APP_REMOVEFLAGS(rec_cam.NetCtrlFlags, APP_NETCTRL_FLAGS_VF_STOP_DONE);
                        AppLibNetControl_ReplyErrorCode(AMBA_STOP_VF, 0);
                    }
                    #endif

                    /* To excute the functions that system block them when the Busy flag is enabled. */
                    AppUtil_BusyCheck(0);
                    if (!APP_CHECKFLAGS(app_rec_cam.GFlags, APP_AFLAGS_READY)) {
                        return ReturnValue;/**<  App switched out*/
                    }
                }
            }
        }
#ifdef CONFIG_APP_ARD
        rec_cam.Func(REC_CAM_GUI_INIT_SHOW, 0, 0);
#endif
    } else {
#ifdef CONFIG_SENSOR_AR0230
        int VideoRes;
#endif
        APP_ADDFLAGS(app_rec_cam.GFlags, APP_AFLAGS_READY);
        /* Enable Anti flicker. */
#ifdef CONFIG_APP_ARD
#ifdef CONFIG_SENSOR_AR0230
        VideoRes = AppLibVideoEnc_GetSensorVideoRes();
        /**if resolution is HDR set flicker mode to */
        if (VideoRes == SENSOR_VIDEO_RES_TRUE_HDR_1080P_HALF) {
            AppLibImage_SetAntiFlickerMode(ANTI_FLICKER_NO_50HZ);
        } else {
            AppLibImage_SetAntiFlickerMode(UserSetting->ImagePref.Flicker);
        }
#else
        AppLibImage_EnableAntiFlicker(1, app_status.anti_flicker_type);
#endif
#else
        AppLibImage_EnableAntiFlicker(1);
#endif

        rec_cam.Func(REC_CAM_CHANGE_OSD, 0, 0);

        /* To excute the functions that system block them when the Ready flag is not enabled. */
        AppUtil_ReadyCheck(0);
        if (!APP_CHECKFLAGS(app_rec_cam.GFlags, APP_AFLAGS_READY)) {
            return ReturnValue;/**<  App switched out*/
        }
        /* To show the gui of current application. */
        rec_cam.Func(REC_CAM_GUI_INIT_SHOW, 0, 0);
    }

    rec_cam.Func(REC_CAM_LIVEVIEW_POST_ACTION, 0, 0);

#ifdef CONFIG_APP_ARD
    if(app_status.cardv_auto_encode == 1){
        APP_APP_s *CurApp;
        if(APP_REC_CAM == AppAppMgt_GetCurApp(&CurApp)) {
            rec_cam.Func(REC_CAM_RECORD_AUTO_START, 0, 0);
        }
    }
    if (!APP_CHECKFLAGS(app_rec_cam.GFlags, APP_AFLAGS_POPUP)) {
        if(rec_cam.RecCapState == REC_CAP_STATE_PREVIEW)
        rec_cam.Func(REC_CAM_MOTION_DETECT_START, 0, 0);
    }

#endif

    return ReturnValue;
}

/**
 *  @brief do actions which need to be call at liveview state
 *  call auto record if encode mode is video
 *
 *  @return =0 success
 */
static int rec_cam_liveview_post_action(void)
{
    int ReturnValue = 0;
#ifdef CONFIG_APP_ARD
    rec_cam.Func(REC_CAM_ADAS_FUNCTION_INIT, 0, 0);
#else
    rec_cam.Func(REC_CAM_ADAS_FUNCTION_INIT, 0, 0);
#endif

#if defined(CONFIG_APP_AMBA_LINK)
    if (AppLibNetBase_GetBootStatus() == 0) {
        //DBGMSGc2(GREEN,"[rec_cam] <liveview_post_action> AMBA Link not booted yet!");
        return 0;
    }

    if (APP_CHECKFLAGS(app_rec_cam.GFlags, APP_AFLAGS_BUSY)) {
        return 0;
    }

    if (app_status.CurrEncMode == APP_VIDEO_ENC_MODE) {
        if (APP_CHECKFLAGS(app_rec_cam.Flags,REC_CAM_FLAGS_CAPTURE_ON_VF)) {
            DBGMSGc2(CYAN, "[rec_cam] <liveview_post_action> REC_CAM_FLAGS_CAPTURE_ON_VF");
            /**Wait photo preview to enter preview state*/
            rec_cam.RecCapState = REC_CAP_STATE_RESET;
            AppLibVideoEnc_LiveViewStop();
            AppLibStillEnc_LiveViewSetup();
            AppLibStillEnc_LiveViewStart();
            app_status.CurrEncMode = APP_STILL_ENC_MODE;
        } else {
            if (UserSetting->VideoPref.StreamType == STREAM_TYPE_RTSP) {
                DBGMSGc2(GREEN, "[rec_car_dv] <liveview_post_action> REC_CAR_DV_VF_START");
                rec_cam.Func(REC_CAM_VF_START, 0, 0);
            }
        }
    } else {
        /* APP_STILL_ENC_MODE */
        if (APP_CHECKFLAGS(app_rec_cam.Flags,REC_CAM_FLAGS_CAPTURE_ON_VF)) {
            DBGMSGc2(CYAN, "[rec_cam] <liveview_post_action> REC_CAM_CAPTURE");
            rec_cam.Func(REC_CAM_CAPTURE, 0, 0);
        }
    }

#else
    if (app_status.CurrEncMode == APP_VIDEO_ENC_MODE) {
        if (!APP_CHECKFLAGS(app_rec_cam.Flags, REC_CAM_FLAGS_MUXER_BUSY)) {
#ifdef CONFIG_APP_ARD
            if(app_status.cardv_auto_encode == 1)
#endif
#ifdef CONFIG_APP_ARD
            {
                APP_APP_s *CurApp;
                if(APP_REC_CAM == AppAppMgt_GetCurApp(&CurApp)) {
                    rec_cam.Func(REC_CAM_RECORD_AUTO_START, 0, 0);
                }
            }
#else
            rec_cam.Func(REC_CAM_RECORD_AUTO_START, 0, 0);
#endif
        }
    }
#endif /* CONFIG_APP_AMBA_LINK */

    return ReturnValue;
}

/**
 *  @brief The timer handler of selftimer
 *
 *  To countdown and show the gui.
 *
 *  @param [in] eid Event id
 *
 *  @return >=0 success, <0 failure
 */
static void rec_cam_selftimer_handler(int eid)
{
    if (eid == TIMER_UNREGISTER) {
        return;
    }

    if (--rec_cam.SelfTimerTime > 0) {
        rec_cam.Gui(GUI_SELFTIMER_COUNTDOWN_UPDATE, rec_cam.SelfTimerTime, 0);
        rec_cam.Gui(GUI_SELFTIMER_COUNTDOWN_SHOW, 0, 0);
    } else {
        AppLibComSvcTimer_Unregister(TIMER_1HZ, rec_cam_selftimer_handler);
        rec_cam.Gui(GUI_SELFTIMER_COUNTDOWN_HIDE, 0, 0);
        if (rec_cam.SelfTimerType == SELF_TIMER_TYPE_PHOTO) {
            rec_cam.Func(REC_CAM_CAPTURE, 0, 0);
        } else if (rec_cam.SelfTimerType == SELF_TIMER_TYPE_VIDEO) {
            rec_cam.Func(REC_CAM_RECORD_START, 0, 0);
        }
    }
    rec_cam.Gui(GUI_FLUSH, 0, 0);
}


static int rec_cam_selftimer(int enable, int param1, int param2)
{
    int ReturnValue = 0;

    if (enable) {
        rec_cam.SelfTimerTime = param1;
        rec_cam.SelfTimerType = param2;
        APP_ADDFLAGS(app_rec_cam.Flags, REC_CAM_FLAGS_SELFTIMER_RUN);

        /* Register the timer for self timer. */
        AppLibComSvcTimer_Register(TIMER_1HZ, rec_cam_selftimer_handler);

        /* Update the gui. */
        rec_cam.Gui(GUI_SELFTIMER_COUNTDOWN_UPDATE, rec_cam.SelfTimerTime, 0);
        rec_cam.Gui(GUI_SELFTIMER_COUNTDOWN_SHOW, 0, 0);
        rec_cam.Gui(GUI_FLUSH, 0, 0);
    } else {
        if(APP_CHECKFLAGS(app_rec_cam.Flags, REC_CAM_FLAGS_SELFTIMER_RUN)) {
            APP_REMOVEFLAGS(app_rec_cam.Flags, REC_CAM_FLAGS_SELFTIMER_RUN);
            /* Unregister the timer for self timer. */
            AppLibComSvcTimer_Unregister(TIMER_1HZ, rec_cam_selftimer_handler);

            /* Update the gui. */
            rec_cam.Gui(GUI_SELFTIMER_COUNTDOWN_HIDE, 0, 0);
            rec_cam.Gui(GUI_FLUSH, 0, 0);
        }
    }
    return ReturnValue;
}

#define Single_JPEG_SIZE    (5*1024)    /**< 5MB */
static UINT32  SingleCapContCount = 0;/**<Count single capture number for 3A*/
/**
 *  @brief The timer handler that can show the record time.
 *
 *  The timer handler that can show the record time.
 *
 *  @param[in] eid timer id.
 */
static void rec_cam_capture_cont_timer_handler(int eid)
{
    static int CapCount = 0;
    static UINT32 Cap_num = 0;
    int PhotoAmount = 0;

    if (eid == TIMER_UNREGISTER) {
        return;
    }
    PhotoAmount = AppLibStorageDmf_GetFileAmount(APPLIB_DCF_MEDIA_IMAGE,DCIM_HDLR) + AppLibStillEnc_GetCaptureNum() + \
        rec_cam.MuxerNum;
    if (APP_CHECKFLAGS(app_rec_cam.Flags, REC_CAM_FLAGS_MUXER_OPEN)) {
        /**if muxer open flag is on, represent there is a dcf object is created but file is writing.
             The fileamount function and muxernum both count this file, so minus one to correct the file amount*/
        PhotoAmount --;
    }
    if (APP_CHECKFLAGS(app_rec_cam.Flags, REC_CAM_FLAGS_SHUTTER_PRESSED) &&
        (AppLibCard_GetFreeSpace(AppLibCard_GetActiveDrive()) > (rec_cam.MuxerNum * Single_JPEG_SIZE))
        && PhotoAmount <= MAX_PHOTO_COUNT) {
        if ( CapCount == (UserSetting->PhotoPref.TimeLapse - 1)) {
            if (!APP_CHECKFLAGS(app_rec_cam.Flags, REC_CAM_FLAGS_CAPTURE_BG_PROCESS)) {
                AppLibStillEnc_CaptureSingleCont();
                APP_ADDFLAGS(app_rec_cam.Flags, REC_CAM_FLAGS_CAPTURE_BG_PROCESS);
                Cap_num ++;
                DBGMSGc2(GREEN, "[app_rec_cam] cont_cap: %d", Cap_num);
                rec_cam.MuxerNum ++;
            } else {
                APP_ADDFLAGS(app_rec_cam.Flags, REC_CAM_FLAGS_PES_DELAY);
                AppLibComSvcTimer_Unregister(TIMER_2HZ, rec_cam_capture_cont_timer_handler);
                Cap_num = 0;
                AmbaPrintColor(GREEN, "[app_rec_cam] Previous capture not finish yet, wait until it finish");
            }
            CapCount = 0;
        } else {
            CapCount++;
        }
    } else {
        if (PhotoAmount >= MAX_PHOTO_COUNT) {
#ifdef CONFIG_APP_ARD
            rec_cam.Gui(GUI_PHOTO_NUM_SHOW, MAX_PHOTO_COUNT, 0);
#endif
            rec_cam.Func(REC_CAM_WARNING_MSG_SHOW_START, GUI_WARNING_PHOTO_LIMIT, 0);
            AmbaPrintColor(CYAN,"[rec_cam] BG_PROCESS_DONE: Photo count reach limit, can not do capture");
        }
        AppLibComSvcTimer_Unregister(TIMER_2HZ, rec_cam_capture_cont_timer_handler);
        DBGMSGc2(GREEN, "[app_rec_cam] Stop cont_cap: %d", Cap_num);
        Cap_num = 0;
        CapCount = 0;
        if (!APP_CHECKFLAGS(app_rec_cam.Flags, REC_CAM_FLAGS_CAPTURE_BG_PROCESS)) {
            SingleCapContCount = 0;/**<reset capture count*/
            AppLibImage_UnLock3A();
            AmpStillEnc_EnableLiveviewCapture(AMP_STILL_STOP_LIVEVIEW, 0);  // Still codec return to idle state
            AppLibStillEnc_SingleCapContFreeBuf();/**<free buff*/
            /*Start the liveview after stoping capturing photo.*/
#if defined(CONFIG_APP_AMBA_LINK)
            if (APP_CHECKFLAGS(app_rec_cam.Flags, REC_CAM_FLAGS_CAPTURE_ON_VF)) {
                AppLibStillEnc_LiveViewDeInit();
                AppLibVideoEnc_LiveViewSetup();
                AppLibVideoEnc_LiveViewStart();
            } else
#endif
            {
                AppLibStillEnc_LiveViewSetup();
                AppLibStillEnc_LiveViewStart();
            }
        } else {
            APP_ADDFLAGS(app_rec_cam.Flags, REC_CAM_FLAGS_PES_DELAY);
        }
    }
}

static int rec_cam_capture(void)
{
    int ReturnValue = 0;

    if (app_status.CurrEncMode != APP_STILL_ENC_MODE) {
        DBGMSG("[rec_cam] <capture> It is the photo preview mode now.");
        return -1;
    }
    DBGMSGc2(GREEN,"[rec_cam] <capture> REC_CAM_CAPTURE");
    if (rec_cam.RecCapState != REC_CAP_STATE_PREVIEW)
        return ReturnValue;

    if (APP_CHECKFLAGS(app_rec_cam.Flags, REC_CAM_FLAGS_MUXER_BUSY)) {
        AmbaPrintColor(GREEN,"[app_rec_cam] Capture block due to previous capture is not finish");
        return ReturnValue;
    }

    /* Initialize and start the muxer. */
    AppLibFormat_MuxerInit();
    AppLibFormatMuxExif_Start();
    AppLibImage_Lock3A();

    /* Enable stamp */
    #if defined(CONFIG_APP_STAMP)
    rec_cam_setup_stamp();
    #endif
    switch (UserSetting->PhotoPref.PhotoCapMode) {
    case PHOTO_CAP_MODE_PES:
            DBGMSGc2(GREEN,"[app_rec_cam] CAPTURE: PHOTO_CAP_MODE_PES");
            if (UserSetting->PhotoPref.TimeLapse != PHOTO_TIME_LAPSE_OFF) {
                AppLibComSvcTimer_Register(TIMER_2HZ, rec_cam_capture_cont_timer_handler);
            }
            AppLibStillEnc_CaptureSingleCont();
            rec_cam.MuxerNum += AppLibStillEnc_GetCaptureNum();
        break;
    case PHOTO_CAP_MODE_BURST:
            DBGMSGc2(GREEN,"[app_rec_cam] CAPTURE: PHOTO_CAP_MODE_BURST");
            AppLibStillEnc_CaptureBurst();
            rec_cam.MuxerNum += AppLibStillEnc_GetCaptureNum();
        break;
    case PHOTO_CAP_MODE_PRECISE:
    default:
            DBGMSGc2(GREEN,"[app_rec_cam] CAPTURE: PHOTO_CAP_MODE_PRECISE");
            AppLibStillEnc_CaptureSingle();
            rec_cam.MuxerNum += AppLibStillEnc_GetCaptureNum();
        break;
    }
    APP_ADDFLAGS(app_rec_cam.GFlags, APP_AFLAGS_BUSY);
    APP_ADDFLAGS(app_rec_cam.Flags, REC_CAM_FLAGS_MUXER_BUSY);
    APP_ADDFLAGS(app_rec_cam.Flags, REC_CAM_FLAGS_STILL_CAPTURE);
    APP_ADDFLAGS(app_rec_cam.Flags, REC_CAM_FLAGS_CAPTURE_BG_PROCESS);
    rec_cam.RecCapState = REC_CAP_STATE_CAPTURE;

    return ReturnValue;
}

static int rec_cam_capture_piv(void)
{
    int ReturnValue = 0;
     /* Initialize and start the muxer. */
    AppLibFormat_MuxerInit();
    AppLibVideoEnc_PIVInit();
    if (!APP_CHECKFLAGS(app_rec_cam.Flags, REC_CAM_FLAGS_CAPTURE_PIV)) {
        AppLibFormatMuxExifPIV_Start();//Testing
        ReturnValue = AppLibVideoEnc_CapturePIV();
        rec_cam.MuxerNum += 1;
    } else {
        AmbaPrintColor(GREEN,"[app_rec_cam] PIV Capture block due to previous capture is not finish");
        return -1;
    }
    APP_ADDFLAGS(app_rec_cam.Flags, REC_CAM_FLAGS_CAPTURE_BG_PROCESS);
    APP_ADDFLAGS(app_rec_cam.Flags, REC_CAM_FLAGS_CAPTURE_PIV);

    return ReturnValue;
}

static int rec_cam_capture_complete(void)
{
    int ReturnValue = 0;

    switch (UserSetting->PhotoPref.PhotoCapMode) {
    case PHOTO_CAP_MODE_PES:
        AmbaPrintColor(GREEN,"[app_rec_cam] CAPTURE_COMPLETE: PHOTO_CAP_MODE_PES");
        break;
    case PHOTO_CAP_MODE_BURST:
        AmbaPrintColor(GREEN,"[app_rec_cam] CAPTURE_COMPLETE: PHOTO_CAP_MODE_BURST");
        break;
    case PHOTO_CAP_MODE_PRECISE:
    default:
        AmbaPrintColor(GREEN,"[app_rec_cam] CAPTURE_COMPLETE: PHOTO_CAP_MODE_PRECISE");
        break;
    }

#ifdef CONFIG_APP_ARD
    rec_cam.Func(REC_CAR_PHOTO_FILE_NUM_UPDATE,0,0);
#endif
    return ReturnValue;
}

static int rec_cam_capture_bg_process_done(void)
{
    int ReturnValue = 0;
    int PhotoAmount = 0;
    APP_REMOVEFLAGS(app_rec_cam.Flags, REC_CAM_FLAGS_CAPTURE_BG_PROCESS);

    if (app_status.CurrEncMode != APP_STILL_ENC_MODE) {
        /** if not in still mode, do not start liveview*/
        return ReturnValue;
    }
    PhotoAmount = AppLibStorageDmf_GetFileAmount(APPLIB_DCF_MEDIA_IMAGE,DCIM_HDLR) + AppLibStillEnc_GetCaptureNum() + \
        rec_cam.MuxerNum;

    if (APP_CHECKFLAGS(app_rec_cam.Flags, REC_CAM_FLAGS_MUXER_OPEN)) {
        /**if muxer open flag is on, represent there is a dcf object is created but file is writing.
             The fileamount function and muxernum both count this file, so minus one to correct the file amount*/
        PhotoAmount --;
    }
    switch (UserSetting->PhotoPref.PhotoCapMode) {
    case PHOTO_CAP_MODE_PES:
        AmbaPrintColor(GREEN,"[app_rec_cam] BG_PROCESS_DONE: PHOTO_CAP_MODE_PES");
        /**Restart liveview when timer unregister to avoid the bg prosess done come before shutter release will not restart live view*/
        if (APP_CHECKFLAGS(app_rec_cam.Flags, REC_CAM_FLAGS_SHUTTER_PRESSED)) {
            SingleCapContCount++;
            AppLibStillEnc_SetShotCount(SingleCapContCount);
            if (UserSetting->PhotoPref.TimeLapse == PHOTO_TIME_LAPSE_OFF) {
                AppLibStillEnc_CaptureSingleCont();
                APP_ADDFLAGS(app_rec_cam.Flags, REC_CAM_FLAGS_CAPTURE_BG_PROCESS);
                rec_cam.MuxerNum ++;
            } else if (APP_CHECKFLAGS(app_rec_cam.Flags, REC_CAM_FLAGS_PES_DELAY)){
                AppLibComSvcTimer_Register(TIMER_2HZ, rec_cam_capture_cont_timer_handler);
                AppLibStillEnc_CaptureSingleCont();
                APP_ADDFLAGS(app_rec_cam.Flags, REC_CAM_FLAGS_CAPTURE_BG_PROCESS);
                APP_REMOVEFLAGS(app_rec_cam.Flags, REC_CAM_FLAGS_PES_DELAY);
                rec_cam.MuxerNum ++;
            }
        } else if (APP_CHECKFLAGS(app_rec_cam.Flags, REC_CAM_FLAGS_PES_DELAY) || UserSetting->PhotoPref.TimeLapse == PHOTO_TIME_LAPSE_OFF) {
            SingleCapContCount = 0;/**<reset capture count*/
             AppLibImage_UnLock3A();
            AmpStillEnc_EnableLiveviewCapture(AMP_STILL_STOP_LIVEVIEW, 0);  // Still codec return to idle state
            AppLibStillEnc_SingleCapContFreeBuf();/**<free buff*/
            /*Start the liveview after stoping capturing photo.*/
#if defined(CONFIG_APP_AMBA_LINK)
            if (APP_CHECKFLAGS(app_rec_cam.Flags, REC_CAM_FLAGS_CAPTURE_ON_VF)) {
                AppLibStillEnc_LiveViewDeInit();
                AppLibVideoEnc_LiveViewSetup();
                AppLibVideoEnc_LiveViewStart();
            } else
#endif
            {
                AppLibStillEnc_LiveViewSetup();
                AppLibStillEnc_LiveViewStart();
            }
            APP_REMOVEFLAGS(app_rec_cam.Flags, REC_CAM_FLAGS_PES_DELAY);
        } else if (PhotoAmount >= MAX_PHOTO_COUNT) {
#ifdef CONFIG_APP_ARD
            rec_cam.Gui(GUI_PHOTO_NUM_SHOW, MAX_PHOTO_COUNT, 0);
#endif
            rec_cam.Func(REC_CAM_WARNING_MSG_SHOW_START, GUI_WARNING_PHOTO_LIMIT, 0);
            AmbaPrintColor(CYAN,"[rec_cam] BG_PROCESS_DONE: Photo count reach limit, can not do capture");
        }
        break;
    case PHOTO_CAP_MODE_BURST:
        AmbaPrintColor(GREEN,"[app_rec_cam] BG_PROCESS_DONE: PHOTO_CAP_MODE_BURST");
        AppLibImage_UnLock3A();
        AmpStillEnc_EnableLiveviewCapture(AMP_STILL_STOP_LIVEVIEW, 0);  // Still codec return to idle state
        AppLibStillEnc_BurstCapFreeBuf();/**<free buff*/
#if defined(CONFIG_APP_AMBA_LINK)
        if (APP_CHECKFLAGS(app_rec_cam.Flags, REC_CAM_FLAGS_CAPTURE_ON_VF)) {
            AppLibStillEnc_LiveViewDeInit();
            AppLibVideoEnc_LiveViewSetup();
            AppLibVideoEnc_LiveViewStart();
        } else
#endif
        {
            AppLibStillEnc_LiveViewSetup();
            AppLibStillEnc_LiveViewStart();
        }
        break;
    case PHOTO_CAP_MODE_PRECISE:
    default:
        AmbaPrintColor(GREEN,"[app_rec_cam] BG_PROCESS_DONE: PHOTO_CAP_MODE_PRECISE");
        AppLibImage_UnLock3A();
        AmpStillEnc_EnableLiveviewCapture(AMP_STILL_STOP_LIVEVIEW, 0);  // Still codec return to idle state
        AppLibStillEnc_SingleCapFreeBuf();/**<free buff*/
#if defined(CONFIG_APP_AMBA_LINK)
        if (APP_CHECKFLAGS(app_rec_cam.Flags, REC_CAM_FLAGS_CAPTURE_ON_VF)) {
            AppLibStillEnc_LiveViewDeInit();
            AppLibVideoEnc_LiveViewSetup();
            AppLibVideoEnc_LiveViewStart();
        } else
#endif
        {
            AppLibStillEnc_LiveViewSetup();
            AppLibStillEnc_LiveViewStart();
        }
        break;
    }

#if defined(CONFIG_APP_STAMP)
    rec_cam_stop_stamp();
#endif

    return ReturnValue;
}


static void rec_cam_rec_check_event_folder(void)
{
#ifdef CONFIG_APP_ARD
    int number = AppLibStorageDmf_GetFileObjAmount(APPLIB_DCF_MEDIA_VIDEO, EVENTRECORD_HDLR);
#else
    int number = AppLibStorageDmf_GetFileAmount(APPLIB_DCF_MEDIA_VIDEO, EVENTRECORD_HDLR);
#endif
#ifdef CONFIG_APP_ARD
    if (number>MAX_EVENT_FILE){
#else
    if (number>10){
#endif
        rec_cam.Func(REC_CAM_CARD_FULL_HANDLE_EVENT, 0, 0);
    }
}

/**
 *  @brief The timer handler that can show the record time.
 *
 *  The timer handler that can show the record time.
 *
 *  @param[in] eid timer id.
 *
 */
static void rec_cam_rec_timer_handler(int eid)
{
    if (eid == TIMER_UNREGISTER) {
        rec_cam.Func(REC_CAM_RECORD_LED_START, 0, 0);
        return;
    }
    if (rec_cam.RecCapState == REC_CAP_STATE_RECORD) {
        rec_cam.RecTime++;
        if (AppLibVideoEnc_GetTimeLapse() == VIDEO_TIME_LAPSE_OFF) {
            rec_cam.Gui(GUI_REC_TIMER_UPDATE, rec_cam.RecTime, 0);
        } else {
            rec_cam.TimeLapseTime--;
            if (rec_cam.TimeLapseTime == 0) {
                AppLibVideoEnc_EncodeTimeLapse();
                rec_cam.TimeLapseTime = AppLibVideoEnc_GetTimeLapse();
                rec_cam.Gui(GUI_REC_TIMER_UPDATE, rec_cam.RecTime, 0);
            }
        }
#ifdef CONFIG_APP_ARD
        if((AppLibVideoEnc_GetPreRecord() == 1)&&(rec_cam.RecTime == 46)){
             if (APP_CHECKFLAGS(app_rec_cam.Flags, REC_CAM_FLAGS_MUXER_BUSY)) {
                  /* Stop recording. */
                  rec_cam.Func(REC_CAM_RECORD_STOP, 0, 0);
                } else {
                 AmbaPrintColor(YELLOW, "[rec_cam] <button_record> Record not actually start, block record stop");
              }
        }

        if(MOTION_DETECT_ON == UserSetting->MotionDetectPref.MotionDetect) {
             if((rec_cam.MotionDetectRecordRemainTime!=0)&&(--rec_cam.MotionDetectRecordRemainTime == 0))
                   /* Stop recording. */
                  AppLibComSvcHcmgr_SendMsg(AMSG_CMD_MOTION_RECORD_STOP, 0, 0);
         }

        /*For test mode*/
        if (rec_cam.RecCapState == REC_CAP_STATE_RECORD) {
            if(AppUtil_CheckSystemTestModeAutoPowerOff()){
                if(rec_cam.RecTime >= 5){
                    AppUtil_SystemTestModePowerOff();
                }
            }
        }
#endif
        #if defined(CONFIG_APP_STAMP)
        rec_cam_update_stamp();
        #endif
        rec_cam.Gui(GUI_FLUSH, 0, 0);
        #if defined(CONFIG_APP_STAMP)
        rec_cam_encode_stamp(1);
        #endif
#ifdef CONFIG_APP_ARD
    if((app_status.parkingmode_on == 1)&&(rec_cam.RecTime == 10)){
#ifdef CONFIG_APP_EVENT_OVERLAP
        AppLibFormatMuxMp4_EventParkingMode_Status(1);
#endif
        AppLibComSvcHcmgr_SendMsg(AMSG_CMD_EVENT_RECORD, 0, 0);
    }
#ifdef CONFIG_APP_EVENT_OVERLAP
    if((app_status.parkingmode_on == 1)&&(app_status.PowerType == APP_POWER_BATTERY)&&(rec_cam.RecTime==61)){
        AppLibComSvcHcmgr_SendMsg(HMSG_USER_POWER_BUTTON, 0, 0);
     }
    if((app_status.parkingmode_on == 1)&&(app_status.PowerType == APP_POWER_ADAPTER)&&(rec_cam.RecTime==61)){
        app_status.parkingmode_on = 0;
        AppLibComSvcHcmgr_SendMsg(HMSG_USER_RECORD_BUTTON, 0, 0);
       }
#endif
#endif
    }
    #if defined(CONFIG_APP_AMBA_LINK)
    else if (rec_cam.RecCapState == REC_CAP_STATE_VF) {
        rec_cam.RecTime++;
        rec_cam.Gui(GUI_REC_TIMER_UPDATE, rec_cam.RecTime, 0);

        #if defined(CONFIG_APP_STAMP)
        rec_cam_update_stamp();
        #endif

        rec_cam.Gui(GUI_FLUSH, 0, 0);

        #if defined(CONFIG_APP_STAMP)
        rec_cam_encode_stamp(1);
        #endif
    }
    #endif
#ifdef CONFIG_ECL_GUI
    if (rec_cam.RecCapState == REC_CAP_STATE_RECORD) {
        rec_cam_rec_check_event_folder();
    }
#endif
}

static int rec_cam_record_start(void)
{
    int ReturnValue = 0;
    if (app_status.CurrEncMode != APP_VIDEO_ENC_MODE) {
        DBGMSG("[app_rec_cam] It is the photo preview mode now.");
        return ReturnValue;
    }
    if (APP_CHECKFLAGS(app_rec_cam.Flags, REC_CAM_FLAGS_MUXER_BUSY))
        return ReturnValue;

    DBGMSG("[app_rec_cam] REC_CAM_RECORD_START");
#ifdef CONFIG_APP_ARD
    AppUtil_BatteryVoltagePrint();

    if(app_status.parkingmode_on == 1){
        int CardStatus = 0;
        int number = 0;

        // show SD card status
        CardStatus = AppLibCard_CheckStatus(CARD_CHECK_WRITE);
        AmbaPrintColor(RED,"rec_cam_record_start Card error = %d  No Card", CardStatus);
        if (CardStatus == CARD_STATUS_NO_CARD) {
            app_status.parkingmode_on = 0;
            app_status.cardv_auto_encode = 0;
            rec_cam.Func(REC_CAM_WARNING_MSG_SHOW_START, GUI_WARNING_NO_CARD, 1);
            AppLibAudioDec_Beep(BEEP_ERROR,0);
            AmbaKAL_TaskSleep(500);
            rec_cam.Func(REC_CAM_WARNING_MSG_SHOW_STOP, GUI_WARNING_NO_CARD, 0);
            AppLibAudioDec_Beep(BEEP_ERROR,0);
            AmbaKAL_TaskSleep(500);
            rec_cam.Func(REC_CAM_WARNING_MSG_SHOW_START, GUI_WARNING_NO_CARD, 1);
            AppLibAudioDec_Beep(BEEP_ERROR,0);
            AmbaKAL_TaskSleep(500);
            rec_cam.Func(REC_CAM_WARNING_MSG_SHOW_STOP, GUI_WARNING_NO_CARD, 0);
            AmbaKAL_TaskSleep(500);
            rec_cam.Func(REC_CAM_WARNING_MSG_SHOW_START, GUI_WARNING_NO_CARD, 1);
            AmbaKAL_TaskSleep(500);
            AmbaPrintColor(YELLOW,"No Card Insert");
            AppLibComSvcHcmgr_SendMsg(HMSG_USER_POWER_BUTTON, 0, 0);
            return ReturnValue;
        }

        number = AppLibStorageDmf_GetFileObjAmount(APPLIB_DCF_MEDIA_VIDEO, EVENTRECORD_HDLR);
        if(number >= MAX_EVENT_FILE){
            rec_cam.Gui(GUI_EVENT_NUM_SHOW, MAX_EVENT_FILE, 0);
            app_status.parkingmode_on = 0;
            app_status.cardv_auto_encode = 0;
            rec_cam.Func(REC_CAM_WARNING_MSG_SHOW_START, GUI_WARNING_EVENT_FULL, 1);
            AppLibAudioDec_Beep(BEEP_ERROR,0);
            AmbaKAL_TaskSleep(500);
            rec_cam.Func(REC_CAM_WARNING_MSG_SHOW_STOP, GUI_WARNING_EVENT_FULL, 0);
            AppLibAudioDec_Beep(BEEP_ERROR,0);
            AmbaKAL_TaskSleep(500);
            rec_cam.Func(REC_CAM_WARNING_MSG_SHOW_START, GUI_WARNING_EVENT_FULL, 1);
            AppLibAudioDec_Beep(BEEP_ERROR,0);
            AmbaKAL_TaskSleep(500);
            rec_cam.Func(REC_CAM_WARNING_MSG_SHOW_STOP, GUI_WARNING_EVENT_FULL, 0);
            AmbaKAL_TaskSleep(500);
            rec_cam.Func(REC_CAM_WARNING_MSG_SHOW_START, GUI_WARNING_EVENT_FULL, 1);
            AmbaKAL_TaskSleep(500);
            AmbaPrintColor(RED,"Event file amount > MAX_EVENT_FILE(%d)!,skip Event REC!",MAX_EVENT_FILE);
            AppLibComSvcHcmgr_SendMsg(HMSG_USER_POWER_BUTTON, 0, 0);
            return ReturnValue;
        }
    }
#endif

    /* Set time lapse. */
    rec_cam.TimeLapseTime = AppLibVideoEnc_GetTimeLapse();

    /* Register the timer to show the record time. */
    rec_cam.RecTime = 0;
    AppLibComSvcTimer_Register(TIMER_1HZ, rec_cam_rec_timer_handler);
    rec_cam.Func(REC_CAM_RECORD_LED_START, 1, 0);


    /* Setup the encode setting. */
    AppLibVideoEnc_EncodeSetup();
#ifdef CONFIG_APP_ARD
        /* Enable stamp */
    #if defined(CONFIG_APP_STAMP)
        rec_cam_setup_stamp();
    #endif
#endif

    /* Initialize and start the muxer. */
    AppLibFormat_MuxerInit();
    AppLibFormatMuxMp4_Start();

#ifdef CONFIG_APP_ARD
    if(UserSetting->VideoPref.video_split_rec_time != VIDEO_SPLIT_REC_OFF )
        AppLibFormatMuxMp4_SetAutoSplitFileType(1);/**set the auto split file type*/
    else
        AppLibFormatMuxMp4_SetAutoSplitFileType(0);/**set the auto split file type*/
#else
    AppLibFormatMuxMp4_SetAutoSplitFileType(1);/**set the auto split file type*/
#endif

#ifdef CONFIG_APP_ARD
    APP_REMOVEFLAGS(app_rec_cam.Flags, REC_CAM_FLAGS_APP_RECEIVED_MUX_START);
    AppLibAudioEnc_Mute(UserSetting->VideoPref.MicMute);
#endif

    /* Start video encoding. */
    AppLibVideoEnc_EncodeStart();

    /* Enable the storage monitor.*/
    AppLibMonitorStorage_Enable(1);
    AppLibMonitorStorage_EnableMsg(1);

#ifndef CONFIG_APP_ARD
    /* Enable stamp */
    #if defined(CONFIG_APP_STAMP)
    rec_cam_setup_stamp();
    #endif
#endif
#ifdef CONFIG_APP_ARD
#ifdef CONFIG_APP_EVENT_OVERLAP
    AppLibPrecMux_setprecend(0);
#endif
#endif

#ifndef CONFIG_ECL_GUI
    if (UserSetting->VAPref.AdasDetection == ADAS_ON) {
        // ReturnValue = rec_cam.Func(REC_CAM_ADAS_UPDATE_PARAM, 0, 0);
        // //AppLibVideoAnal_FCMD_Enable();
        // //AppLibVideoAnal_LLWS_Enable();
        // AppLibVideoAnal_ADAS_Enable();
        // rec_cam.Gui(GUI_ADAS_STAMP_UPDATE, 0, 0);
        // rec_cam.Gui(GUI_ADAS_STAMP_SHOW, 0, 0);
    }
#endif
    
    rec_cam.RecCapState = REC_CAP_STATE_RECORD;
    #if defined(CONFIG_APP_AMBA_LINK)
    NotifyNetFifoOfAppState(AMP_NETFIFO_NOTIFY_STARTENC);
    #endif

    APP_ADDFLAGS(app_rec_cam.GFlags, APP_AFLAGS_BUSY);

    /* Update the gui. */
    rec_cam.Gui(GUI_REC_STATE_UPDATE, GUI_REC_START, 0);
    rec_cam.Gui(GUI_REC_STATE_SHOW, 0, 0);
    rec_cam.Gui(GUI_REC_TIMER_UPDATE, rec_cam.RecTime, 0);
    rec_cam.Gui(GUI_REC_TIMER_SHOW, 0, 0);
#ifdef CONFIG_APP_ARD
    if(app_status.parkingmode_on == 1){
        rec_cam.Func(REC_CAR_VIDEO_EVENT_FILE_NUM_UPDATE,0,0);
        rec_cam.Gui(GUI_EVENT_ICON_SHOW, 0, 0);
    }
#endif
    rec_cam.Gui(GUI_FLUSH, 0, 0);

    return ReturnValue;
}

static int rec_cam_record_pause(void)
{
    int ReturnValue = 0;

    /* Pause encoding */
    AppLibVideoEnc_EncodePause();
    APP_ADDFLAGS(app_rec_cam.Flags, REC_CAM_FLAGS_PAUSED);

    /* Stop the timer that show the gui of recording time because of recording pause. */
    AppLibComSvcTimer_Unregister(TIMER_1HZ, rec_cam_rec_timer_handler);

    /* Update the gui. */
    rec_cam.Gui(GUI_REC_STATE_UPDATE, GUI_REC_PAUSED, 0);
    rec_cam.Gui(GUI_REC_TIMER_UPDATE, rec_cam.RecTime, 0);
    rec_cam.Gui(GUI_FLUSH, 0, 0);

    return ReturnValue;
}

static int rec_cam_record_resume(void)
{
    int ReturnValue = 0;

    /* Resume encoding */
    AppLibVideoEnc_EncodeResume();
    APP_REMOVEFLAGS(app_rec_cam.Flags, REC_CAM_FLAGS_PAUSED);
    /* Start the timer that show the gui of recording time because the flow resume the recording. */
    AppLibComSvcTimer_Register(TIMER_1HZ, rec_cam_rec_timer_handler);
    rec_cam.Func(REC_CAM_RECORD_LED_START, 1, 0);

    /* Update the gui. */
    rec_cam.Gui(GUI_REC_STATE_UPDATE, GUI_REC_START, 0);
    rec_cam.Gui(GUI_REC_TIMER_UPDATE, rec_cam.RecTime, 0);
    rec_cam.Gui(GUI_FLUSH, 0, 0);

    return ReturnValue;
}

static int rec_cam_record_stop(void)
{
    int ReturnValue = 0;
    if ((rec_cam.RecCapState == REC_CAP_STATE_RECORD) || (rec_cam.RecCapState == REC_CAP_STATE_PRE_RECORD)) {
        DBGMSG("[app_rec_cam] REC_CAM_RECORD_STOP");
        if (APP_CHECKFLAGS(app_rec_cam.GFlags, APP_AFLAGS_POPUP)) {
            AppWidget_Off(WIDGET_ALL, 0);
        }
        /* Stop the storage monitor.*/
        AppLibMonitorStorage_EnableMsg(0);
        AppLibMonitorStorage_Enable(0);

#ifdef CONFIG_APP_ARD
        rec_cam.MotionDetectRecordRemainTime = 0;
#ifdef CONFIG_APP_EVENT_OVERLAP
        AppLibPrecMux_setprecend(1);
        AppLibFormatMuxMp4_EventStop();
        AppLibFormatMuxMp4_EventParkingMode_Status(0);
#endif
#endif
//#ifdef CONFIG_APP_ARD
//        /*Set duration as MAX to avoid trigger "REACH_LIMIT" at the same time when stop encode*/
//        AppLibFormatMuxMp4_SetMaxDurationMax();
//#endif

        /* Stop encoding. */
        AppLibVideoEnc_EncodeStop();
        #if defined(CONFIG_APP_STAMP)
        rec_cam_stop_stamp();
        #endif
        if (UserSetting->VAPref.AdasDetection == ADAS_ON) {
            AppLibVideo_Ecl_ADAS_Disable();
            AppLibVideoAnal_FCMD_Disable();
            AppLibVideoAnal_LLWS_Disable();
            AppLibVideoAnal_ADAS_Disable();
            rec_cam.Gui(GUI_ADAS_STAMP_HIDE, 0, 0);
        }

        rec_cam.RecCapState = REC_CAP_STATE_RESET;

        /* Stop the timer that show the gui of recording time because of stopping recording. */
        AppLibComSvcTimer_Unregister(TIMER_1HZ, rec_cam_rec_timer_handler);
        /* Update the gui. */
        rec_cam.Gui(GUI_REC_STATE_HIDE, 0, 0);
        rec_cam.Gui(GUI_REC_TIMER_HIDE, 0, 0);
#ifdef CONFIG_APP_ARD
        rec_cam.Gui(GUI_EVENT_ICON_HIDE, 0, 0);
        {
        int number;
        number = AppLibStorageDmf_GetFileAmount(APPLIB_DCF_MEDIA_VIDEO, EVENTRECORD_HDLR);
        if(number < MAX_EVENT_FILE)
        rec_cam.Gui(GUI_EVENT_NUM_HIDE,0,0);
        }
#endif
        rec_cam.Gui(GUI_FLUSH, 0, 0);
    }

    return ReturnValue;
}

/**
*  @brief:add auto record function to check card status and start record
*
*record stop after memory runout, mode change, menu open, the record should be auto restart after
*before start record, card status should be recheck
*
*  @return =0 success
*/
static int rec_cam_record_auto_start(void)
{
    int ReturnValue = 0;
#ifdef CONFIG_APP_ARD
    int number = AppLibStorageDmf_GetFileObjAmount(APPLIB_DCF_MEDIA_VIDEO, EVENTRECORD_HDLR);

    if (APP_CHECKFLAGS(app_rec_cam.GFlags, APP_AFLAGS_POPUP)) {
        /* Don't start VF when menu is on. */
        DBGMSGc2(GREEN, "[rec_cam] <record_auto_start> APP_AFLAGS_POPUP");
        return -1;
    }
#ifdef CONFIG_APP_ARD
        if(app_status.parkingmode_on == 1){
            int CardStatus = 0;
            // show SD card status
            CardStatus = AppLibCard_CheckStatus(CARD_CHECK_WRITE);
            AmbaPrintColor(RED,"rec_cam_record_auto_start Card error = %d  No Card", CardStatus);
            if (CardStatus == CARD_STATUS_NO_CARD) {
                app_status.parkingmode_on = 0;
                app_status.cardv_auto_encode = 0;
                rec_cam.Func(REC_CAM_WARNING_MSG_SHOW_START, GUI_WARNING_NO_CARD, 1);
                AppLibAudioDec_Beep(BEEP_ERROR,0);
                AmbaKAL_TaskSleep(500);
                rec_cam.Func(REC_CAM_WARNING_MSG_SHOW_STOP, GUI_WARNING_NO_CARD, 0);
                AppLibAudioDec_Beep(BEEP_ERROR,0);
                AmbaKAL_TaskSleep(500);
                rec_cam.Func(REC_CAM_WARNING_MSG_SHOW_START, GUI_WARNING_NO_CARD, 1);
                AppLibAudioDec_Beep(BEEP_ERROR,0);
                AmbaKAL_TaskSleep(500);
                rec_cam.Func(REC_CAM_WARNING_MSG_SHOW_STOP, GUI_WARNING_NO_CARD, 0);
                AmbaKAL_TaskSleep(500);
                rec_cam.Func(REC_CAM_WARNING_MSG_SHOW_START, GUI_WARNING_NO_CARD, 1);
                AmbaKAL_TaskSleep(500);
                AmbaPrintColor(GREEN,"No Card Insert");
                AppLibComSvcHcmgr_SendMsg(HMSG_USER_POWER_BUTTON, 0, 0);
                return ReturnValue;
            }

            if(number >= MAX_EVENT_FILE){
                rec_cam.Gui(GUI_EVENT_NUM_SHOW, MAX_EVENT_FILE, 0);
                rec_cam.Gui(GUI_FLUSH, 0, 0);
                app_status.parkingmode_on = 0;
                app_status.cardv_auto_encode = 0;
                rec_cam.Func(REC_CAM_WARNING_MSG_SHOW_START, GUI_WARNING_EVENT_FULL, 1);
                AppLibAudioDec_Beep(BEEP_ERROR,0);
                AmbaKAL_TaskSleep(500);
                rec_cam.Func(REC_CAM_WARNING_MSG_SHOW_STOP, GUI_WARNING_EVENT_FULL, 0);
                AppLibAudioDec_Beep(BEEP_ERROR,0);
                AmbaKAL_TaskSleep(500);
                rec_cam.Func(REC_CAM_WARNING_MSG_SHOW_START, GUI_WARNING_EVENT_FULL, 1);
                AppLibAudioDec_Beep(BEEP_ERROR,0);
                AmbaKAL_TaskSleep(500);
                rec_cam.Func(REC_CAM_WARNING_MSG_SHOW_STOP, GUI_WARNING_EVENT_FULL, 0);
                AmbaKAL_TaskSleep(500);
                rec_cam.Func(REC_CAM_WARNING_MSG_SHOW_START, GUI_WARNING_EVENT_FULL, 1);
                AmbaKAL_TaskSleep(500);
                AmbaPrintColor(RED,"Event file amount > MAX_EVENT_FILE(%d)!,skip Event REC!",MAX_EVENT_FILE);
                AppLibComSvcHcmgr_SendMsg(HMSG_USER_POWER_BUTTON, 0, 0);
                return ReturnValue;
            }
        }
#endif
    if (rec_cam.RecCapState == REC_CAP_STATE_PREVIEW && app_status.CurrEncMode == APP_VIDEO_ENC_MODE) {
        /** Check the card's status. */
#ifdef CONFIG_APP_ARD
        ReturnValue = rec_cam.Func(REC_CAM_CARD_CHECK_STATUS, 0, REC_CAP_PREPARE_TO_REPLAY_AUTO_RECORD);
#else
        ReturnValue = rec_cam.Func(REC_CAM_CARD_CHECK_STATUS, 0, 0);
#endif
        if (ReturnValue == 0) {
#ifdef CONFIG_APP_ARD
        if(AppUtil_CheckSystemTestModeAutoPowerOff() == 0){
            //if ((UserSetting->VideoPref.UnsavingData != 0) || (UserSetting->VideoPref.UnsavingEvent != 0)) {
            if (UserSetting->VideoPref.UnsavingData != 0){
                AmbaPrint("%s, %d", __func__, __LINE__);
                if (0 == AppLibDCF_GetOpenedFiles()) {
                    AppUtil_CheckCardParam(0);
                    AmbaPrint("%s, %d", __func__, __LINE__);
                    return ReturnValue;
                }
            }
        }
#endif
            /** To record the clip if the card is ready. */
            rec_cam.Func(REC_CAM_RECORD_START, 0, 0);
#ifdef CONFIG_APP_ARD
            /** for baterry,camera boot and do not auto record, for dc, camera boot and auto record,but later run on manul mode */
            app_status.cardv_auto_encode = 0;
#endif
        }
    }
#ifdef CONFIG_APP_ARD
    /*Enable auto encode when test mode is on(burn-in test)*/
    if(AppUtil_CheckSystemTestModeAutoPowerOff()){
        app_status.cardv_auto_encode = 1;
    }
#endif

#endif

    return ReturnValue;
}
static int rec_cam_muxer_open(int MuxerType)
{
    int ReturnValue = 0;

    if (MuxerType == 0) {
        /**video muxer open add muxer busy flag and turn on unsaving data flag*/
    APP_ADDFLAGS(app_rec_cam.Flags, REC_CAM_FLAGS_MUXER_BUSY);
    UserSetting->VideoPref.UnsavingData = 1;
    AppPref_Save();
    } else {
        /**exif muxer open add muxer open flag for photo amount counting*/
        APP_ADDFLAGS(app_rec_cam.Flags, REC_CAM_FLAGS_MUXER_OPEN);
    }

    return ReturnValue;
}

static int rec_cam_muxer_end(void)
{
    int ReturnValue = 0;

    if (app_status.CurrEncMode == APP_VIDEO_ENC_MODE) {
        /*Video muxer end*/
        if (rec_cam.MuxerNum == 0) {
            APP_REMOVEFLAGS(app_rec_cam.Flags, REC_CAM_FLAGS_MUXER_OPEN);
            /* Close the mp4 muxer. */
            AppLibFormatMuxMp4_Close();
            /* Clear the movie recovery flag. */
            if (!APP_CHECKFLAGS(app_rec_cam.Flags, REC_CAM_FLAGS_MUXER_ERROR)) {
                APP_REMOVEFLAGS(app_rec_cam.Flags, REC_CAM_FLAGS_MUXER_ERROR);
                UserSetting->VideoPref.UnsavingData = 0;
#ifdef CONFIG_APP_ARD
#ifdef CONFIG_APP_EVENT_OVERLAP
                UserSetting->VideoPref.UnsavingEvent = 0;
#endif
#endif
                AppPref_Save();
            }
            /* Remove the flag.*/
            APP_REMOVEFLAGS(app_rec_cam.Flags, REC_CAM_FLAGS_MUXER_BUSY);
            #if defined(CONFIG_APP_AMBA_LINK)||defined(CONFIG_APP_ARD)
            if (!rec_cam_system_is_busy()) {
            #else
            if (rec_cam.RecCapState == REC_CAP_STATE_PREVIEW) {
            #endif
                /* The system should be idle if the muxer is idle and the state is preview state. */
                APP_REMOVEFLAGS(app_rec_cam.GFlags, APP_AFLAGS_BUSY);

                /* To excute the functions that system block them when the Busy flag is enabled. */
                AppUtil_BusyCheck(0);
                if (!APP_CHECKFLAGS(app_rec_cam.GFlags, APP_AFLAGS_READY)) {
                    return ReturnValue;/**<  App switched out and break current application's flow. */
                }

                    #if defined (CONFIG_APP_AMBA_LINK)
                    if (UserSetting->VideoPref.StreamType == STREAM_TYPE_RTSP) {
                        DBGMSGc2(GREEN, "[rec_cam] <muxer_end> REC_CAM_VF_START");
                        rec_cam.Func(REC_CAM_VF_START, 0, 0);
                    }
                    #else
#ifndef CONFIG_APP_ARD
                    rec_cam.Func(REC_CAM_RECORD_AUTO_START, 0, 0);
#else
                    if (APP_CHECKFLAGS(app_rec_cam.Flags, REC_CAM_FLAGS_MEM_RUNOUT)) {
                        AmbaPrintColor(YELLOW, "[rec_cam]REC auto due to memory runout");
                        rec_cam.Func(REC_CAM_RECORD_AUTO_START, 0, 0);
                        APP_REMOVEFLAGS(app_rec_cam.Flags, REC_CAM_FLAGS_MEM_RUNOUT);
                    }
#endif
                    #endif
            }
            }
            else if(!APP_CHECKFLAGS(app_rec_cam.Flags, REC_CAM_FLAGS_CAPTURE_PIV))
            {
                rec_cam.MuxerNum --;
                APP_REMOVEFLAGS(app_rec_cam.Flags, REC_CAM_FLAGS_MUXER_OPEN);
            }
            else {

            /*PIV muxer end*/
            AppLibFormatMuxExifPIV_Close();
            AppLibFormatMuxMgr_MuxEnd();
            AppLibVideoEnc_PIVFreeBuf();
            rec_cam.MuxerNum --;
            APP_REMOVEFLAGS(app_rec_cam.Flags, REC_CAM_FLAGS_MUXER_OPEN);
            APP_REMOVEFLAGS(app_rec_cam.Flags, REC_CAM_FLAGS_CAPTURE_PIV);
            APP_REMOVEFLAGS(app_rec_cam.Flags, REC_CAM_FLAGS_BLOCK_MENU);
            DBGMSGc2(GREEN, "[rec_cam] <muxer_end> PIV Done");
        }
    } else {
        AppLibFormatMuxExif_Close();
        AppLibFormatMuxMgr_MuxEnd();
        rec_cam.MuxerNum --;
        APP_REMOVEFLAGS(app_rec_cam.Flags, REC_CAM_FLAGS_MUXER_OPEN);
        if (rec_cam.MuxerNum == 0) {
            APP_REMOVEFLAGS(app_rec_cam.Flags, REC_CAM_FLAGS_MUXER_BUSY);
            if (!APP_CHECKFLAGS(app_rec_cam.Flags, REC_CAM_FLAGS_STILL_CAPTURE)) {
#if defined(CONFIG_APP_AMBA_LINK)||defined(CONFIG_APP_ARD)
                if (!rec_cam_system_is_busy()) {
#else
                if (rec_cam.RecCapState == REC_CAP_STATE_PREVIEW) {
#endif
                    /* The application is idle, if the muxer is idle after caputuring completed.*/
                    APP_REMOVEFLAGS(app_rec_cam.GFlags, APP_AFLAGS_BUSY);
                    APP_REMOVEFLAGS(app_rec_cam.Flags, REC_CAM_FLAGS_BLOCK_MENU);

#if defined(CONFIG_APP_AMBA_LINK)
                    if (APP_CHECKFLAGS(app_rec_cam.Flags,REC_CAM_FLAGS_CAPTURE_FROM_NETCTRL)) {
                        rec_cam.Func(REC_CAM_NETCTRL_CAPTURE_DONE, 0, 0);
                    }
                    if (APP_CHECKFLAGS(app_rec_cam.Flags,REC_CAM_FLAGS_CAPTURE_ON_VF)) {
                        APP_REMOVEFLAGS(app_rec_cam.Flags,REC_CAM_FLAGS_CAPTURE_ON_VF);
                        app_status.CurrEncMode = APP_VIDEO_ENC_MODE;
                        if (UserSetting->VideoPref.StreamType == STREAM_TYPE_RTSP) {
                            DBGMSGc2(GREEN, "[rec_cam] <muxer_end> REC_CAM_VF_START 2");
                            rec_cam.Func(REC_CAM_VF_START, 0, 0);
                        }
                    }
#endif

                    /* To excute the functions that system block them when the Busy flag is enabled. */
                    AppUtil_BusyCheck(0);
                    if (!APP_CHECKFLAGS(app_rec_cam.GFlags, APP_AFLAGS_READY)) {
                        return ReturnValue;/**<  App switched out and break current application's flow. */
                    }
                }
            }
        }
    }
    AmbaPrintColor(GREEN,"[app_rec_cam] REC_CAM_MUXER_END mux.num%d",rec_cam.MuxerNum);

    return ReturnValue;
}

static int rec_cam_muxer_reach_limit(int param1)
{
    int ReturnValue = 0;

    if (param1) {
        /* Reach the limitation of file, but the setting of split file is off. Stop recording. */
        if ((rec_cam.RecCapState == REC_CAP_STATE_RECORD) || (rec_cam.RecCapState == REC_CAP_STATE_PRE_RECORD)) {
            /* Stop recording if the audio and video data buffer is full. */
            rec_cam.Func(REC_CAM_RECORD_STOP, 0, 0);
        }
    }

    return ReturnValue;
}

static int rec_cam_muxer_stream_error(void)
{
    int ReturnValue = 0;

    if (app_status.CurrEncMode == APP_VIDEO_ENC_MODE) {
        if (rec_cam.RecCapState == REC_CAP_STATE_RECORD) {
            rec_cam.Func(REC_CAM_RECORD_STOP, 0, 0);
        }
        /* Close the mp4 muxer. */
        AppLibFormatMuxMp4_StreamError();
#ifdef CONFIG_APP_ARD
#ifndef CONFIG_APP_EVENT_OVERLAP
        UserSetting->VideoPref.UnsavingData = 1;
#endif
#endif
        /* Remove the flag.*/
        APP_REMOVEFLAGS(app_rec_cam.Flags, REC_CAM_FLAGS_MUXER_BUSY);
        #if defined(CONFIG_APP_AMBA_LINK)||defined(CONFIG_APP_ARD)
        if (!rec_cam_system_is_busy()) {
        #else
        if (rec_cam.RecCapState == REC_CAP_STATE_PREVIEW) {
        #endif
            /* The system should be idle if the muxer is idle and the state is preview state. */
            APP_REMOVEFLAGS(app_rec_cam.GFlags, APP_AFLAGS_BUSY);

            /* To excute the functions that system block them when the Busy flag is enabled. */
            AppUtil_BusyCheck(0);
            if (!APP_CHECKFLAGS(app_rec_cam.GFlags, APP_AFLAGS_READY)) {
                return ReturnValue;/**<  App switched out and break current application's flow. */
            }

                #if defined (CONFIG_APP_AMBA_LINK)
                if (UserSetting->VideoPref.StreamType == STREAM_TYPE_RTSP) {
                    DBGMSGc2(GREEN, "[rec_cam] <muxer_end> REC_CAM_VF_START");
                    rec_cam.Func(REC_CAM_VF_START, 0, 0);
                }
                #else
#ifdef CONFIG_APP_ARD
                if(app_status.cardv_auto_encode == 1)
#endif
                rec_cam.Func(REC_CAM_RECORD_AUTO_START, 0, 0);

                #endif

        }
    } else {
        rec_cam.MuxerNum --;
        if (rec_cam.MuxerNum == 0) {
            /* The system will close the EXIF muxer automatically. The application flow does not need to close it. */
            APP_REMOVEFLAGS(app_rec_cam.Flags, REC_CAM_FLAGS_MUXER_BUSY);
            if (!APP_CHECKFLAGS(app_rec_cam.Flags, REC_CAM_FLAGS_STILL_CAPTURE)) {

#if defined(CONFIG_APP_AMBA_LINK)||defined(CONFIG_APP_ARD)
                if (!rec_cam_system_is_busy()) {
#else
                if (rec_cam.RecCapState == REC_CAP_STATE_PREVIEW) {
#endif
                    /* The application is idle, if the muxer is idle after caputuring completed.*/
                    APP_REMOVEFLAGS(app_rec_cam.GFlags, APP_AFLAGS_BUSY);
                    APP_REMOVEFLAGS(app_rec_cam.Flags, REC_CAM_FLAGS_BLOCK_MENU);

#if defined(CONFIG_APP_AMBA_LINK)
                    if (APP_CHECKFLAGS(app_rec_cam.Flags,REC_CAM_FLAGS_CAPTURE_ON_VF)) {
                        APP_REMOVEFLAGS(app_rec_cam.Flags,REC_CAM_FLAGS_CAPTURE_ON_VF);
                        app_status.CurrEncMode = APP_VIDEO_ENC_MODE;
                        if (UserSetting->VideoPref.StreamType == STREAM_TYPE_RTSP) {
                            DBGMSGc2(GREEN, "[rec_cam] <muxer_end> REC_CAM_VF_START 2");
                            rec_cam.Func(REC_CAM_VF_START, 0, 0);
                        }
                    }
#endif

                    /* To excute the functions that system block them when the Busy flag is enabled. */
                    AppUtil_BusyCheck(0);
                    if (!APP_CHECKFLAGS(app_rec_cam.GFlags, APP_AFLAGS_READY)) {
                        return ReturnValue;/**<  App switched out and break current application's flow. */
                    }
                }
            }
        }
    }
    AmbaPrintColor(GREEN,"[app_rec_cam] REC_CAM_MUXER_END mux.num%d",rec_cam.MuxerNum);

    return ReturnValue;
}


static int rec_cam_error_memory_runout(void)
{
    int ReturnValue = 0;
    if ((rec_cam.RecCapState == REC_CAP_STATE_RECORD) || (rec_cam.RecCapState == REC_CAP_STATE_PRE_RECORD)) {

        /* Stop/Pause recording if the audio and video data buffer is full. */
#if defined(REC_CAM_MEM_RUNOUT_PAUSE)
        AmbaPrintColor(GREEN,"[app_rec_cam] REC_CAM_MEM_RUNOUT_PAUSE");
        rec_cam.Func(REC_CAM_RECORD_PAUSE, 0, 0);
#else
        APP_ADDFLAGS(app_rec_cam.Flags, REC_CAM_FLAGS_MEM_RUNOUT);
        rec_cam.Func(REC_CAM_RECORD_STOP, 0, 0);
#ifdef CONFIG_APP_ARD
        rec_cam.Func(REC_CAM_WARNING_MSG_SHOW_START, GUI_WARNING_MEM_RUNOUT, 0);
#endif
#endif
    }

    return ReturnValue;
}

static int rec_cam_error_storage_runout(void)
{
    int ReturnValue = 0;

    if ((rec_cam.RecCapState == REC_CAP_STATE_RECORD) || (rec_cam.RecCapState == REC_CAP_STATE_PRE_RECORD)) {
        /**call card full handle to do loop enc*/
        rec_cam.Func(REC_CAM_CARD_FULL_HANDLE, 0, 0);
    }
    return ReturnValue;
}


static int rec_cam_error_storage_io(void)
{
    int ReturnValue = 0;

    if ((rec_cam.RecCapState == REC_CAP_STATE_RECORD) || (rec_cam.RecCapState == REC_CAP_STATE_PRE_RECORD)) {
        /* Stop recording if the audio and video data buffer is full. */
        rec_cam.Func(REC_CAM_RECORD_STOP, 0, 0);
    }

    return ReturnValue;
}

static int rec_cam_error_loop_enc_err(int err_type)
{
    int ReturnValue = 0;

#ifdef CONFIG_APP_ARD
    if (rec_cam.PrepareToReplayRecord != REC_CAP_PREPARE_TO_REPLAY_NONE) {
        rec_cam.PrepareToReplayRecord = REC_CAP_PREPARE_TO_REPLAY_NONE;
    }
#endif

    /**start send storage runout msg after loop enc function*/
    AppLibMonitorStorage_EnableMsg(1);
    if (err_type) {
        AmbaPrint("[app_rec_cam] Loop Enc Delete File Fail ");
    } else {
        if (rec_cam.RecCapState == REC_CAP_STATE_RECORD ) {
            rec_cam.Func(REC_CAM_RECORD_STOP, 0, 0);
            AmbaPrint("[app_rec_cam] Loop Enc Search First File Fail Stop Record ");
        } else {
            AmbaPrint("[app_rec_cam] Loop Enc Search First File Fail");
        }
    }
    return ReturnValue;
}

static int rec_cam_loop_enc_done(void)
{
    int ReturnValue = 0;
    AmbaPrintColor(GREEN,"[app_rec_cam] Loop Enc Done");
    /**start send storage runout msg after loop enc function*/
    AppLibMonitorStorage_EnableMsg(1);
#ifdef CONFIG_APP_ARD
    /** It was fail to start recording last time becase of shortage of card volume. Now resume. */
    if (rec_cam.PrepareToReplayRecord == REC_CAP_PREPARE_TO_REPLAY_AUTO_RECORD) {
        rec_cam.PrepareToReplayRecord = REC_CAP_PREPARE_TO_REPLAY_NONE;
        DBGMSGc2(GREEN, "[rec_cam] <%s> resume auto record after card full handle.", __func__);
        ReturnValue = rec_cam.Func(REC_CAM_RECORD_AUTO_START, 0, 0);
        return ReturnValue;
    }
    else if (rec_cam.PrepareToReplayRecord == REC_CAP_PREPARE_TO_REPLAY_MANUAL_RECORD) {
        rec_cam.PrepareToReplayRecord = REC_CAP_PREPARE_TO_REPLAY_NONE;
        DBGMSGc2(GREEN, "[rec_cam] <%s> resume manual record after card full handle.", __func__);
        ReturnValue = rec_cam.Func(REC_CAM_RECORD_START, 0, 0);
        return ReturnValue;
    }
#endif
    if (rec_cam.RecCapState == REC_CAP_STATE_PREVIEW ) {
#ifdef CONFIG_APP_ARD
    if(app_status.cardv_auto_encode == 1)
#endif
        rec_cam.Func(REC_CAM_RECORD_AUTO_START, 0, 0);
    }
    #if defined(CONFIG_APP_AMBA_LINK)
     else if (rec_cam.RecCapState == REC_CAP_STATE_VF) {
        rec_cam.Func(REC_CAM_VF_SWITCH_TO_RECORD, 0, 0);
    }
    #endif
    return ReturnValue;
}

static int rec_cam_switch_app(void)
{
    int ReturnValue = 0;

    if ((rec_cam.RecCapState == REC_CAP_STATE_RECORD) || (rec_cam.RecCapState == REC_CAP_STATE_PRE_RECORD)) {
        ReturnValue = rec_cam.Func(REC_CAM_RECORD_STOP, 0, 0);
    }
    #if defined(CONFIG_APP_AMBA_LINK)
    else if (rec_cam.RecCapState == REC_CAP_STATE_VF) {
        ReturnValue = rec_cam.Func(REC_CAM_VF_STOP, 0, 0);
    }
    #endif

    return ReturnValue;
}


/**
 *  @brief To update the video encoding resolution.
 *
 *  To update the video encoding resolution.
 *
 *  @param [in] videoRes Video resolution id
 *  @param [in] guiFlush The flag that update the gui.
 *
 *  @return >=0 success, <0 failure
 */
static int rec_cam_set_videoRes(UINT32 videoRes, UINT32 guiFlush)
{
    int ReturnValue = 0;

    AppLibVideoEnc_SetSensorVideoRes(videoRes);
    /** do not change to video only at HFR mode
    if (videoRes >= SENSOR_VIDEO_RES_FHD_HFR_P120_P100 && AppLibVideoEnc_GetRecMode() == REC_MODE_AV) {
        AppLibVideoEnc_SetRecMode(REC_MODE_VIDEO_ONLY);
        ReturnValue = ApplibVideoEnc_PipeChange();
    } else if (videoRes < SENSOR_VIDEO_RES_FHD_HFR_P120_P100 && AppLibVideoEnc_GetRecMode() == REC_MODE_VIDEO_ONLY) {
        AppLibVideoEnc_SetRecMode(REC_MODE_AV);
        ReturnValue = ApplibVideoEnc_PipeChange();
    }
    */
    if (app_status.CurrEncMode == APP_VIDEO_ENC_MODE) {
        /**Calculate second stream timescale by main stream time scale*/
        APPLIB_SENSOR_VIDEO_ENC_CONFIG_s *VideoEncConfigData;
        VideoEncConfigData = AppLibSysSensor_GetVideoConfig(AppLibVideoEnc_GetSensorVideoRes());
        if ((VideoEncConfigData->EncNumerator % 25000) ==0) {
            AppLibVideoEnc_SetSecStreamTimeScale(25000);
            AppLibVideoEnc_SetSecStreamTick(1000);
        } else {
            AppLibVideoEnc_SetSecStreamTimeScale(30000);
            AppLibVideoEnc_SetSecStreamTick(1001);
        }
        AppLibVideoEnc_LiveViewSetup();
    }

    /** reflush the menu */
    if (AppWidget_GetCur() == WIDGET_MENU) {
        //AppMenu_ReflushItem();
    }

    rec_cam.Gui(GUI_VIDEO_SENSOR_RES_UPDATE, UserSetting->VideoPref.SensorVideoRes, 0);

#ifdef CONFIG_APP_ARD
    if( UserSetting->VideoPref.SensorVideoRes == SENSOR_VIDEO_RES_TRUE_HDR_1080P_HALF ||
        UserSetting->VideoPref.SensorVideoRes == SENSOR_VIDEO_RES_WQHD_HALF_HDR) {
        #ifndef CONFIG_ECL_GUI
        rec_cam.Gui(GUI_HDR_ICON_SHOW,0,0);
        #endif

        #ifdef CONFIG_SENSOR_AR0230
        AppLibImage_SetAntiFlickerMode(ANTI_FLICKER_NO_50HZ);
        #else
        AppLibImage_EnableAntiFlicker(1, app_status.anti_flicker_type);
        #endif
    } else {
        
         #ifndef CONFIG_ECL_GUI
         rec_cam.Gui(GUI_HDR_ICON_HIDE,0,0);
         #endif
        AppLibImage_EnableAntiFlicker(1, app_status.anti_flicker_type);
    }
#endif

    if (guiFlush) {
        rec_cam.Gui(GUI_FLUSH, 0, 0);
    }

    return ReturnValue;
}

/**
 *  @brief To update the video encoding quality.
 *
 *  To update the video encoding quality.
 *
 *  @param [in] videoRes Quality id
 *  @param [in] guiFlush The flag that update the gui.
 *
 *  @return >=0 success, <0 failure
 */
static int rec_cam_set_video_quality(UINT32 quality, UINT32 guiFlush)
{
    int ReturnValue = 0;

    AppLibVideoEnc_SetQuality(quality);
    switch (quality) {
    case VIDEO_QUALITY_SFINE:
        ReturnValue = GUI_SFINE;
        break;
    case VIDEO_QUALITY_FINE:
        ReturnValue = GUI_FINE;
        break;
    case VIDEO_QUALITY_NORMAL:
    default:
        ReturnValue = GUI_NORMAL;
        break;
    }
    rec_cam.Gui(GUI_VIDEO_QUALITY_UPDATE, ReturnValue, 0);
    if (guiFlush) {
        rec_cam.Gui(GUI_FLUSH, 0, 0);
    }

    return ReturnValue;
}

/**
 *  @brief To update the photo caputre size.
 *
 *  To update the photo caputre size.
 *
 *  @param [in] size Photo size id
 *  @param [in] guiFlush The flag that update the gui.
 *
 *  @return >=0 success, <0 failure
 */
static int rec_cam_set_photo_size(UINT32 size, UINT32 guiFlush)
{
    int ReturnValue = 0;

    AppLibStillEnc_SetSizeID(size);

    if (app_status.CurrEncMode == APP_STILL_ENC_MODE) {
        AppLibStillEnc_LiveViewSetup();
    }

    rec_cam.Gui(GUI_PHOTO_SIZE_UPDATE, size, 0);
    if (guiFlush) {
        rec_cam.Gui(GUI_FLUSH, 0, 0);
    }
    return ReturnValue;
}

/**
 *  @brief To update the photo quality.
 *
 *  To update the photo quality.
 *
 *  @param [in] videoRes Quality id
 *  @param [in] guiFlush The flag that update the gui.
 *
 *  @return >=0 success, <0 failure
 */
static int rec_cam_set_photo_quality(UINT32 quality, UINT32 guiFlush)
{
    int ReturnValue = 0;

    AppLibStillEnc_SetQualityMode(quality);

    /* Update gui. */
    switch (quality) {
    case PHOTO_QUALITY_SFINE:
        ReturnValue = GUI_SFINE;
        break;
    case PHOTO_QUALITY_FINE:
        ReturnValue = GUI_FINE;
        break;
    case PHOTO_QUALITY_NORMAL:
        default:
        ReturnValue = GUI_NORMAL;
        break;
    }
    rec_cam.Gui(GUI_PHOTO_QUALITY_UPDATE, ReturnValue, 0);
    if (guiFlush) {
        rec_cam.Gui(GUI_FLUSH, 0, 0);
    }

    return ReturnValue;
}

/**
 *  @brief Update the gui of record mode.
 *
 *  Update the gui of record mode.
 *
 *  @return >=0 success, <0 failure
 */
static int rec_cam_rec_mode_gui_update(void)
{
    int ReturnValue = 0;

    if (UserSetting->VideoPref.TimeLapse == VIDEO_TIME_LAPSE_OFF) {
        if (UserSetting->VideoPref.DualStreams) {
            rec_cam.Gui(GUI_REC_MODE_UPDATE, GUI_MODE_DUAL_STREAMS, 0);
        } else {
            rec_cam.Gui(GUI_REC_MODE_UPDATE, GUI_MODE_DEFAULT, 0);
        }
    } else {
        switch (UserSetting->VideoPref.TimeLapse) {
        case VIDEO_TIME_LAPSE_2S:
            rec_cam.Gui(GUI_REC_MODE_UPDATE, GUI_MODE_TIME_LAPSE_2S, 0);
            break;
        default:
        case VIDEO_TIME_LAPSE_OFF:
            rec_cam.Gui(GUI_REC_MODE_UPDATE, GUI_MODE_DEFAULT, 0);
            break;
        }
    }

    return ReturnValue;
}


/**
 *  @brief To update the time lapse setting.
 *
 *  To update the time lapse setting.
 *
 *  @param [in] timeLapse Time Lapse setting
 *  @param [in] guiFlush The flag that update the gui.
 *
 *  @return >=0 success, <0 failure
 */
static int rec_cam_set_time_lapse(UINT32 timeLapse, UINT32 guiFlush)
{
    int ReturnValue = 0;

    ReturnValue = AppLibVideoEnc_GetSensorVideoRes();
    if ((ReturnValue > SENSOR_VIDEO_RES_PHOTO) && (ReturnValue < SENSOR_VIDEO_RES_NUM)) {
        /* HFR mode doesn't support timeLapse. */
        AmbaPrint("[app_rec_cam] HFR mode doesn't support timeLapse.");
        timeLapse = VIDEO_TIME_LAPSE_OFF;
    }

    switch (timeLapse) {
    case VIDEO_TIME_LAPSE_2S:
        ReturnValue = AppLibVideoEnc_SetTimeLapse(timeLapse);
        break;
    case VIDEO_TIME_LAPSE_OFF:
        ReturnValue = AppLibVideoEnc_SetTimeLapse(timeLapse);
        break;
    default:
        break;
    }

    /* Update gui. */
    rec_cam_rec_mode_gui_update();
    if (guiFlush) {
        rec_cam.Gui(GUI_FLUSH, 0, 0);
    }

    return ReturnValue;
}

#ifdef CONFIG_APP_ARD
/**
 *  set pre record mode
 *
 *  @param [in] enable pre record
 *
 *
 *  @return >=0 success, <0 failure
 */
static int rec_cam_set_video_prerecord(UINT32 prerecord_en)
{
    int ReturnValue = 0;

    AppLibVideoEnc_SetPreRecord(prerecord_en);

    return ReturnValue;
}

#endif

/**
 *  @brief To update the self timer setting.
 *
 *  To update the self timer setting.
 *
 *  @param [in] selftimer Self timer setting
 *  @param [in] guiFlush The flag that update the gui.
 *
 *  @return >=0 success, <0 failure
 */
static int rec_cam_set_selftimer(UINT32 selftimer, UINT32 guiFlush)
{
    int ReturnValue = 0;

    /* Update gui. */
    switch (selftimer) {
    case SELF_TIMER_3S:
        ReturnValue = GUI_SELFTIMER_3S;
        break;

    case SELF_TIMER_5S:
        ReturnValue = GUI_SELFTIMER_5S;
        break;

    case SELF_TIMER_10S:
        ReturnValue = GUI_SELFTIMER_10S;
        break;

    case SELF_TIMER_OFF:
    default:
        ReturnValue = GUI_SELFTIMER_OFF;
        break;
    }
    rec_cam.Gui(GUI_SELFTIMER_UPDATE, ReturnValue, 0);
    if (guiFlush) {
        rec_cam.Gui(GUI_FLUSH, 0, 0);
    }

    return ReturnValue;
}

static int rec_cam_set_enc_mode(int param1)
{
    int ReturnValue = 0;
    AppMenu_Reset();
#ifndef CONFIG_ECL_GUI
    /** Set menus */
    //AppMenu_RegisterTab(MENU_SETUP);
    if (param1 == APP_VIDEO_ENC_MODE)
        AppMenu_RegisterTab(MENU_VIDEO);
    else
        AppMenu_RegisterTab(MENU_PHOTO);
#else
    //AppMenu_RegisterTab(MENU_ADAS);
    //AppMenu_RegisterTab(MENU_SETUP);
    //AppMenu_RegisterTab(MENU_VIDEO);
#endif

    if (param1 == app_status.CurrEncMode)
        return ReturnValue;/**<  Break the flow if the mode is the same as current mode. */

    /** Start the liveview. */
    if (param1 == APP_STILL_ENC_MODE) {
#ifdef CONFIG_APP_ARD
            if(MOTION_DETECT_ON == UserSetting->MotionDetectPref.MotionDetect) {
                ReturnValue = AppLibVideoAnal_MD_Disable();
                AmbaPrintColor(YELLOW, "--------AppLibVideoAnal_MD_Disable return = %d", ReturnValue);

                ReturnValue = AppLibVideoAnal_MD_DeInit();
                AmbaPrintColor(YELLOW, "--------AppLibVideoAnal_MD_DeInit return = %d", ReturnValue);
                AmbaKAL_TaskSleep(400);
            }
#endif
        /* Stop video preivew. */
        AppLibVideoEnc_LiveViewStop();
        /* Start still preivew. */
        AppLibStillEnc_LiveViewSetup();
        AppLibStillEnc_LiveViewStart();
        app_status.CurrEncMode = APP_STILL_ENC_MODE;
    } else {
        /* Stop still preivew. */
        AppLibStillEnc_LiveViewStop();
        AppLibStillEnc_LiveViewDeInit();
        /**delete still encode pipe when change to video mode*/
        AppLibStillEnc_DeletePipe();
        /* Start video preivew. */
        AppLibVideoEnc_LiveViewSetup();
        AppLibVideoEnc_LiveViewStart();
        app_status.CurrEncMode = APP_VIDEO_ENC_MODE;
    }

    /** Update the GUI. */
    if (app_status.CurrEncMode == APP_VIDEO_ENC_MODE) {
        #ifndef CONFIG_ECL_GUI
        rec_cam.Gui(GUI_APP_VIDEO_ICON_SHOW, 0, 0);
#ifdef CONFIG_APP_ARD
        rec_cam.Func(REC_CAR_VIDEO_EVENT_FILE_NUM_UPDATE,0,0);
#endif
        #endif
    } else {
        rec_cam.Gui(GUI_APP_PHOTO_ICON_SHOW, 0, 0);
#ifdef CONFIG_APP_ARD
        rec_cam.Func(REC_CAR_PHOTO_FILE_NUM_UPDATE,0,0);
#endif
    }
    rec_cam.Gui(GUI_FLUSH, 0, 0);

    return ReturnValue;
}

static int rec_cam_set_dmf_mode(int param1)
{
    int ReturnValue = 0;
    AppLibStorageDmf_SetFileNumberMode(APPLIB_DCF_MEDIA_VIDEO, (APPLIB_DCF_NUMBER_MODE_e)param1, DCIM_HDLR);
    return ReturnValue;
}

static int rec_cam_card_removed(void)
{
    int ReturnValue = 0;

#ifdef CONFIG_APP_ARD
    if (APP_CHECKFLAGS(app_rec_cam.GFlags, APP_AFLAGS_POPUP) && (AppWidget_GetCur() == WIDGET_DIALOG)) {
        APP_APP_s *CurApp;
        int app_id = AppAppMgt_GetCurApp(&CurApp);
        if ((app_id == APP_MISC_FORMATCARD) || (app_id == APP_MISC_MVRECOVER)) {
            APP_REMOVEFLAGS(app_rec_cam.GFlags, APP_AFLAGS_POPUP);
            AppWidget_Off(WIDGET_ALL, 0);
        }
    }
#endif

    if (rec_cam.RecCapState == REC_CAP_STATE_RECORD) {
        /* Stop recording when the card be removed during recording. */
        rec_cam.Func(REC_CAM_RECORD_STOP, 0, 0);
    }

    /* Stop the warning message, because the warning could need to be updated.. */
    rec_cam.Func(REC_CAM_WARNING_MSG_SHOW_STOP, 0, 0);

    /* Stop the self timer because the system can not capture or record. */
    rec_cam.Func(REC_CAM_SELFTIMER_STOP, 0, 0);

    /* Reset the file type of quick view function. */
    rec_cam.QuickViewFileType = MEDIA_TYPE_UNKNOWN;

#ifdef CONFIG_APP_ARD
    rec_cam.Gui(GUI_PHOTO_NUM_HIDE, 0, 0);
#endif

    /* Update the gui of card's status. */
    rec_cam.Gui(GUI_CARD_UPDATE, GUI_NO_CARD, 0);
    rec_cam.Gui(GUI_FLUSH, 0, 0);

    return ReturnValue;
}

static int rec_cam_card_error_removed(void)
{
    int ReturnValue = 0;

    if (rec_cam.RecCapState == REC_CAP_STATE_VF) {
        // do nothing
    } else if (rec_cam.RecCapState == REC_CAP_STATE_RECORD){
        /* Update the flags */
        APP_REMOVEFLAGS(app_rec_cam.GFlags, APP_AFLAGS_BUSY);
        APP_ADDFLAGS(app_rec_cam.Flags, REC_CAM_FLAGS_MUXER_ERROR);

#ifdef CONFIG_APP_ARD
#ifdef CONFIG_APP_EVENT_OVERLAP
        if(1 == AppLibFormatMuxMp4_GetEventStatus()) {
            UserSetting->VideoPref.UnsavingData = 0;
            UserSetting->VideoPref.UnsavingEvent = 1;
        } else {
            UserSetting->VideoPref.UnsavingData = 1;
            UserSetting->VideoPref.UnsavingEvent = 0;
        }
        AppPref_Save();
#endif
#endif
        /* Stop recording when the card be removed during recording. */
        rec_cam.Func(REC_CAM_RECORD_STOP, 0, 0);

        /* Stop the warning message, because the warning could need to be updated.. */
        rec_cam.Func(REC_CAM_WARNING_MSG_SHOW_STOP, 0, 0);

        /* Stop the self timer because the system can not capture or record. */
        rec_cam.Func(REC_CAM_SELFTIMER_STOP, 0, 0);

        /* Reset the file type of quick view function. */
        rec_cam.QuickViewFileType = MEDIA_TYPE_UNKNOWN;

    }

    if(app_status.parkingmode_on==1)
    {
        app_status.parkingmode_on=0;
    }

    /* Update the gui of card's status. */
    rec_cam.Gui(GUI_CARD_UPDATE, GUI_NO_CARD, 0);
    rec_cam.Gui(GUI_FLUSH, 0, 0);

    return ReturnValue;
}

static int rec_cam_card_new_insert(void)
{
    int ReturnValue = 0;

    /* Stop the warning message, because the warning could need to be updated.. */
    rec_cam.Func(REC_CAM_WARNING_MSG_SHOW_STOP, 0, 0);

    /* Stop the self timer because the system can not capture or record. */
    rec_cam.Func(REC_CAM_SELFTIMER_STOP, 0, 0);

    /* Reset the file type of quick view function. */
    rec_cam.QuickViewFileType = MEDIA_TYPE_UNKNOWN;

    /* Update the gui of card's status. */
    rec_cam.Gui(GUI_CARD_UPDATE, GUI_NO_CARD, 0);
    rec_cam.Gui(GUI_FLUSH, 0, 0);

    return ReturnValue;
}

static int rec_cam_card_storage_idle(void)
{
    int ReturnValue = 0;

    rec_cam.Func(REC_CAM_SET_FILE_INDEX, 0, 0);

    AppUtil_CheckCardParam(0);
    if (!APP_CHECKFLAGS(app_rec_cam.GFlags, APP_AFLAGS_READY)) {
        return ReturnValue;/**<  App switched out*/
    }

#ifdef CONFIG_APP_ARD
    if (app_status.CurrEncMode == APP_VIDEO_ENC_MODE) {
        rec_cam.Func(REC_CAR_VIDEO_EVENT_FILE_NUM_UPDATE,0,0);
    } else if (app_status.CurrEncMode == APP_STILL_ENC_MODE){
        rec_cam.Func(REC_CAR_PHOTO_FILE_NUM_UPDATE,0,0);
    }
#endif

    /* Update the gui of card's status. */
    rec_cam.Gui(GUI_CARD_UPDATE, GUI_CARD_READY, 0);
    rec_cam.Gui(GUI_FLUSH, 0, 0);

    /** card ready call auto record to start record*/
#ifdef CONFIG_APP_ARD
    if(app_status.cardv_auto_encode == 1){
#endif
#ifdef CONFIG_APP_ARD
        {
            APP_APP_s *CurApp;
            if(APP_REC_CAM == AppAppMgt_GetCurApp(&CurApp)) {
                rec_cam.Func(REC_CAM_RECORD_AUTO_START, 0, 0);
            }
        }
#else
        rec_cam.Func(REC_CAM_RECORD_AUTO_START, 0, 0);
#endif
#ifdef CONFIG_APP_ARD
    } else {
        rec_cam.Func(REC_CAM_PREVIEW, 0, 0);
    }
#endif
    return ReturnValue;
}

static int rec_cam_card_storage_busy(void)
{
    int ReturnValue = 0;

    /* Update the gui of card's status. */
    rec_cam.Gui(GUI_CARD_UPDATE, GUI_CARD_REFRESHING, 0);
    rec_cam.Gui(GUI_FLUSH, 0, 0);

    return ReturnValue;
}

#ifdef CONFIG_APP_ARD
static int rec_cam_card_check_status(int param1, int param2)
#else
static int rec_cam_card_check_status(int param1)
#endif
{
    int ReturnValue = 0;

    ReturnValue = AppLibCard_CheckStatus(CARD_CHECK_WRITE);
    if (ReturnValue == CARD_STATUS_NO_CARD) {
        AmbaPrintColor(RED,"[app_rec_cam] Card error = %d  No Card", ReturnValue);
        rec_cam.Func(REC_CAM_WARNING_MSG_SHOW_START, GUI_WARNING_NO_CARD, 0);
#ifdef CONFIG_APP_ARD
        if(app_status.parkingmode_on == 1){
          int CardStatus = 0;
          int number = 0;
          // show SD card status
          CardStatus = AppLibCard_CheckStatus(CARD_CHECK_WRITE);
          AmbaPrintColor(RED,"rec_cam_record_start Card error = %d  No Card", CardStatus);
          if (CardStatus == CARD_STATUS_NO_CARD) {
             app_status.parkingmode_on = 0;
             app_status.cardv_auto_encode = 0;
             rec_cam.Func(REC_CAM_WARNING_MSG_SHOW_START, GUI_WARNING_NO_CARD, 1);
             AppLibAudioDec_Beep(BEEP_ERROR,0);
             AmbaKAL_TaskSleep(500);
             rec_cam.Func(REC_CAM_WARNING_MSG_SHOW_STOP, GUI_WARNING_NO_CARD, 0);
             AppLibAudioDec_Beep(BEEP_ERROR,0);
             AmbaKAL_TaskSleep(500);
             rec_cam.Func(REC_CAM_WARNING_MSG_SHOW_START, GUI_WARNING_NO_CARD, 1);
             AppLibAudioDec_Beep(BEEP_ERROR,0);
             AmbaKAL_TaskSleep(500);
             rec_cam.Func(REC_CAM_WARNING_MSG_SHOW_STOP, GUI_WARNING_NO_CARD, 0);
             AmbaKAL_TaskSleep(500);
             rec_cam.Func(REC_CAM_WARNING_MSG_SHOW_START, GUI_WARNING_NO_CARD, 1);
             AmbaKAL_TaskSleep(500);
             AmbaPrintColor(YELLOW,"No Card Insert");
             AppLibComSvcHcmgr_SendMsg(HMSG_USER_POWER_BUTTON, 0, 0);
             return ReturnValue;
           }
         }
#endif

    } else if (ReturnValue == CARD_STATUS_WP_CARD) {
        rec_cam.Func(REC_CAM_WARNING_MSG_SHOW_START, GUI_WARNING_CARD_PROTECTED, 0);
        AmbaPrintColor(RED,"[app_rec_cam] Card error = %d Write Protection Card", ReturnValue);
    } else if (ReturnValue == CARD_STATUS_NOT_ENOUGH_SPACE) {
#ifdef CONFIG_APP_ARD
        if ((param1 == 0)
            && (AppLibVideoEnc_GetSplit() != VIDEO_SPLIT_OFF))  /**< do full handle for loop encoding */
        {
            rec_cam.PrepareToReplayRecord = param2; // auto record or manual record.
            rec_cam.Func(REC_CAM_CARD_FULL_HANDLE, 0, 0);
        } else {
            rec_cam.PrepareToReplayRecord = REC_CAP_PREPARE_TO_REPLAY_NONE;
            rec_cam.Func(REC_CAM_WARNING_MSG_SHOW_START, GUI_WARNING_CARD_FULL, 0);
        }
#else
        rec_cam.Func(REC_CAM_WARNING_MSG_SHOW_START, GUI_WARNING_CARD_FULL, 0);
        if (param1 == 0) {/**< video card caheck, do loop encode , photo do noting*/
            rec_cam.Func(REC_CAM_CARD_FULL_HANDLE, 0, 0);
        }
#endif
        AmbaPrintColor(RED,"[app_rec_cam] Card error = %d CARD_STATUS_NOT_ENOUGH_SPACE, param1(%d), 0 to do full handle.", ReturnValue, param1);
    } else {
        if (ReturnValue != CARD_STATUS_CHECK_PASS ) {
            AmbaPrintColor(RED,"[app_rec_cam] Card error = %d", ReturnValue);

#ifdef CONFIG_APP_ARD
            if(CARD_STATUS_UNFORMAT_CARD == ReturnValue) {
                rec_cam.Func(REC_CAM_WARNING_MSG_SHOW_START, GUI_WARNING_CARD_UNFORMATED, 0);
            }
#endif
        }
    }

    return ReturnValue;
}

static int rec_cam_card_full_handle(void)
{
    int ReturnValue = 0;
#ifndef CONFIG_ECL_GUI
    /*Only stop recording if split is off*/
    if(AppLibVideoEnc_GetSplit() == VIDEO_SPLIT_OFF) {
        /* not start full handle, so cancel replay */
        rec_cam.PrepareToReplayRecord = REC_CAP_PREPARE_TO_REPLAY_NONE;
        AppLibMonitorStorage_EnableMsg(1);
        if (APP_CHECKFLAGS(app_rec_cam.Flags, REC_CAM_FLAGS_MUXER_BUSY)) {
            /* Stop recording. */
            rec_cam.Func(REC_CAM_RECORD_STOP, 0, 0);
            rec_cam.Func(REC_CAM_WARNING_MSG_SHOW_START, GUI_WARNING_CARD_FULL, 0);
        }
        return ReturnValue;
    }
#endif
    /**disable storage runout msg send*/
    AppLibMonitorStorage_EnableMsg(0);
    AmbaPrintColor(GREEN,"[app_rec_cam] SEND MSG APPLIB_LOOP_ENC_START");
    /**send msg to start loop enc*/
    ReturnValue = AppLibStorageAsyncOp_SndMsg(HMSG_LOOP_ENC_START, DCIM_HDLR, 0);
    return ReturnValue;

}

static int rec_cam_card_full_handle_event(void)
{
    int ReturnValue = 0;
#ifndef CONFIG_ECL_GUI
    AppLibMonitorStorage_EnableMsg(1);
    if (APP_CHECKFLAGS(app_rec_cam.Flags, REC_CAM_FLAGS_MUXER_BUSY)) {
        /* Stop recording. */
        rec_cam.Func(REC_CAM_RECORD_STOP, 0, 0);
        rec_cam.Func(REC_CAM_WARNING_MSG_SHOW_START, GUI_WARNING_CARD_FULL, 0);
    }
#else
    /**disable storage runout msg send*/
    AppLibMonitorStorage_EnableMsg(0);
    AmbaPrintColor(GREEN,"[app_rec_cam] SEND MSG APPLIB_LOOP_ENC_START for Event Record");
    /**send msg to start loop enc*/
    ReturnValue = AppLibStorageAsyncOp_SndMsg(HMSG_LOOP_ENC_START, EVENTRECORD_HDLR, 0);
#endif
    return ReturnValue;
}

static int rec_cam_file_id_update(UINT32 FileID)
{
    int ReturnValue = 0;
    /**update last id for serial mode if new filw close*/
    if (FileID > UserSetting->SetupPref.DmfMixLastIdx || UserSetting->SetupPref.DMFMode == DMF_MODE_RESET) {
        UserSetting->SetupPref.DmfMixLastIdx = FileID;
    }
    return ReturnValue;
}
void record_sta_set(int sta)
{
    record_sta = sta;
}

/**
 *  @brief handle widget close msg
 *
 *  remove pop up flag and call auto record function to start record
 *
 *  @return =0 success
 *
 */
static int rec_cam_widget_closed(void)
{
    int ReturnValue = 0;
    app_set_menu_scr_status(1);
    if (APP_CHECKFLAGS(app_rec_cam.GFlags, APP_AFLAGS_POPUP)) {
        APP_REMOVEFLAGS(app_rec_cam.GFlags, APP_AFLAGS_POPUP);
        /** after menu close call auto record function to start record*/
#if defined (CONFIG_APP_AMBA_LINK)
        if (UserSetting->VideoPref.StreamType == STREAM_TYPE_RTSP) {
            if ((app_status.CurrEncMode == APP_VIDEO_ENC_MODE) &&
                ((!APP_CHECKFLAGS(app_rec_cam.GFlags, APP_AFLAGS_BUSY)))) {
                DBGMSGc2(GREEN, "[rec_cam] <widget_closed> REC_CAM_VF_START");
                rec_cam.Func(REC_CAM_VF_START, 0, 0);
            }
        }
        #else
        AmbaPrintColor(YELLOW,"[rec_cam] <widget_closed> record auto start");
#ifdef CONFIG_APP_ARD
        if (AppWidget_GetCur() == WIDGET_DIALOG) {
            // Skip auto start if still show some dialogs.
            return 0;
        }
        #ifdef CONFIG_ECL_GUI
        rec_cam.Func(REC_CAM_GUI_INIT_SHOW, 0, 0);//show OSD again
        
        rec_cam_enable_adas();
        #endif

        if(MOTION_DETECT_ON == UserSetting->MotionDetectPref.MotionDetect)
        {
            rec_cam.Func(REC_CAM_MOTION_DETECT_START, 0, 0);
           // rec_cam.Func(REC_CAM_RECORD_START, 0, 0);
        } 
        else
        {
            if(record_sta==0)
            rec_cam.Func(REC_CAM_RECORD_AUTO_START, 0, 0);
            else
            record_sta=0;
            
        }

#else
        rec_cam.Func(REC_CAM_RECORD_AUTO_START, 0, 0);
#endif
        #endif
    }
    return ReturnValue;
}

/**
 *  @brief Switch NTSC and PAL mode
 *
 *  Switch NTSC and PAL mode
 *
 *  @return >=0 success, <0 failure
 */
static int rec_cam_set_system_type(void)
{
    int ReturnValue = 0;

#ifdef CONFIG_APP_ARD
#if defined(CONFIG_APP_AMBA_LINK)
    if (rec_cam.RecCapState == REC_CAP_STATE_VF) {
        /** stop view finder at menu open*/
        rec_cam.Func(REC_CAM_VF_STOP, 0, 0);
    } else if (rec_cam.RecCapState == REC_CAP_STATE_RECORD)
#endif
    {
        /** record stop at menu open*/
        rec_cam.Func(REC_CAM_RECORD_STOP, 0, 0);
    }
    /** Start the liveview. */
    if (app_status.CurrEncMode == APP_STILL_ENC_MODE) {
        /* Start still preivew. */
        AppLibStillEnc_LiveViewSetup();
        app_status.CurrEncMode = APP_STILL_ENC_MODE;
    } else {
        /* Start video preivew. */
        AppLibVideoEnc_LiveViewSetup();
        app_status.CurrEncMode = APP_VIDEO_ENC_MODE;
        // show video resolution
        rec_cam.Gui(GUI_VIDEO_SENSOR_RES_UPDATE, UserSetting->VideoPref.SensorVideoRes, 0);
        rec_cam.Gui(GUI_VIDEO_SENSOR_RES_SHOW, 0, 0);
        rec_cam.Gui(GUI_FLUSH, 0, 0);
    }
#endif
    return ReturnValue;
}

/**
 *  @brief Update the Vout setting of FCHAN
 *
 *  Update the Vout setting of FCHAN
 *
 *  @param [in] msg Message id
 *
 *  @return >=0 success, <0 failure
 */
static int rec_cam_update_fchan_vout(UINT32 msg)
{
    int ReturnValue = 0;
#ifdef CONFIG_APP_ARD
    AMP_DISP_WINDOW_CFG_s Window;
    AMP_DISP_INFO_s DispDev = { 0 };
#endif

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
        return 0;
        break;
    }
    ReturnValue = AppLibDisp_SelectDevice(DISP_CH_FCHAN, DISP_ANY_DEV);
    if (APP_CHECKFLAGS(ReturnValue, DISP_FCHAN_NO_CHANGE)) {
        AmbaPrint("[app_rec_cam] Display FCHAN has no changed");
    } else {
        if (APP_CHECKFLAGS(ReturnValue, DISP_FCHAN_NO_DEVICE)) {
            AppLibGraph_DisableDraw(GRAPH_CH_FCHAN);
            AppLibDisp_ChanStop(DISP_CH_FCHAN);
            AppLibDisp_DeactivateWindow(DISP_CH_FCHAN, AppLibDisp_GetWindowId(DISP_CH_FCHAN, 0));
            AppLibDisp_FlushWindow(DISP_CH_FCHAN);
            app_status.LockDecMode = 0;
        } else {
#ifdef CONFIG_APP_ARD
        if (app_status.FchanDecModeOnly == 1) {
            app_status.LockDecMode = 1;
            AppUtil_SwitchApp(APP_THUMB_MOTION);
            return ReturnValue;
        } else {
            app_status.LockDecMode = 0;

        if (APP_CHECKFLAGS(app_rec_cam.Flags, REC_CAM_FLAGS_SELFTIMER_RUN)) {
            rec_cam.Func(REC_CAM_SELFTIMER_STOP, 0, 0);
            rec_cam.Func(REC_CAM_PREVIEW, 0, 0);
        }

        #if defined(CONFIG_APP_AMBA_LINK)
        if (rec_cam.RecCapState == REC_CAP_STATE_VF) {
            /** stop view finder at menu open*/
            rec_cam.Func(REC_CAM_VF_STOP, 0, 0);
        } else if (rec_cam.RecCapState == REC_CAP_STATE_RECORD)
        #endif
        {
            /** record stop at menu open*/
            rec_cam.Func(REC_CAM_RECORD_STOP, 0, 0);
        }

            AppLibDisp_ConfigMode(DISP_CH_FCHAN, AppLibSysVout_GetVoutMode(VOUT_DISP_MODE_2160P_HALF));
            AppLibDisp_SetupChan(DISP_CH_FCHAN);
            AppLibDisp_ChanStart(DISP_CH_FCHAN);
            {
                AMP_DISP_WINDOW_CFG_s Window;
                AMP_DISP_INFO_s DispDev = {0};

                memset(&Window, 0, sizeof(AMP_DISP_WINDOW_CFG_s));

                ReturnValue = AppLibDisp_GetDeviceInfo(DISP_CH_FCHAN, &DispDev);
                if ((ReturnValue < 0) || (DispDev.DeviceInfo.Enable == 0)) {
                    DBGMSG("[rec_cam] FChan Disable. Disable the fchan window");
                    AppLibDisp_DeactivateWindow(DISP_CH_FCHAN, AppLibDisp_GetWindowId(DISP_CH_FCHAN, 0));
                    AppLibGraph_DeactivateWindow(GRAPH_CH_FCHAN);
                    AppLibDisp_FlushWindow(DISP_CH_FCHAN);
                    app_status.LockDecMode = 0;
                } else {
                    /** FCHAN window*/
                    AppLibDisp_GetWindowConfig(DISP_CH_FCHAN, AppLibDisp_GetWindowId(DISP_CH_FCHAN, 0), &Window);
                    Window.Source = AMP_DISP_ENC;
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
                    AppLibGraph_SetWindowConfig(GRAPH_CH_FCHAN);
                    AppLibGraph_ActivateWindow(GRAPH_CH_FCHAN);
                    AppLibDisp_FlushWindow(DISP_CH_FCHAN);

                    rec_cam.Gui(GUI_SET_LAYOUT, 0, 0);
                    rec_cam.Gui(GUI_FLUSH, 0, 0);

                }
                AppLibVideoEnc_LiveViewSetup();
            }

        }
#else
        if (app_status.FchanDecModeOnly == 1) {
            app_status.LockDecMode = 1;
            AppUtil_SwitchApp(APP_THUMB_MOTION);
            return ReturnValue;
        } else {
            app_status.LockDecMode = 0;
        }
#endif
        }
    }

    return ReturnValue;
}
/**
 *  @brief Update the Vout setting of DCHAN
 *
 *  Update the Vout setting of DCHAN
 *
 *  @param [in] msg Message id
 *
 *  @return >=0 success, <0 failure
 */
static int rec_cam_update_dchan_vout(UINT32 msg)
{
    int ReturnValue = 0;
if(0){
    ReturnValue = AppLibDisp_SelectDevice(DISP_CH_DCHAN, DISP_ANY_DEV);
    if (APP_CHECKFLAGS(ReturnValue, DISP_DCHAN_NO_DEVICE)) {
        /* no device then stop chan, disable draw, and stop activate window*/
        AppLibGraph_DisableDraw(GRAPH_CH_DCHAN);
        AppLibDisp_ChanStop(DISP_CH_DCHAN);
        AppLibDisp_DeactivateWindow(DISP_CH_DCHAN, AppLibDisp_GetWindowId(DISP_CH_DCHAN, 0));
        AppLibDisp_FlushWindow(DISP_CH_DCHAN);
        app_status.LockDecMode = 0;
    } else {
        /*MW will ignore setup chan,if chan is already start. so it needs to stop chan first*/
        AppLibDisp_ChanStop(DISP_CH_DCHAN);
        AppLibDisp_ConfigMode(DISP_CH_DCHAN, AppLibSysVout_GetVoutMode(VOUT_DISP_MODE_1080P));
        AppLibDisp_SetupChan(DISP_CH_DCHAN);
        AppLibDisp_ChanStart(DISP_CH_DCHAN);
        {
            AMP_DISP_WINDOW_CFG_s Window;
            APPLIB_VOUT_PREVIEW_PARAM_s PreviewParam = {0};
            AMP_DISP_INFO_s DispDev = {0};

            memset(&Window, 0, sizeof(AMP_DISP_WINDOW_CFG_s));

            ReturnValue = AppLibDisp_GetDeviceInfo(DISP_CH_DCHAN, &DispDev);
            if ((ReturnValue < 0) || (DispDev.DeviceInfo.Enable == 0)) {
                 DBGMSG("[app_rec_connected_cam] DChan Disable. Disable the Dchan window");
                 AppLibGraph_DisableDraw(GRAPH_CH_DCHAN);
                 AppLibDisp_DeactivateWindow(DISP_CH_DCHAN, AppLibDisp_GetWindowId(DISP_CH_DCHAN, 0));
                 AppLibGraph_DeactivateWindow(GRAPH_CH_DCHAN);
                 AppLibDisp_FlushWindow(DISP_CH_DCHAN);
                 app_status.LockDecMode = 0;
            } else {
                /**Reset DCHAN window*/
                PreviewParam.ChanID = DISP_CH_DCHAN;
                AppLibDisp_CalcPreviewWindowSize(&PreviewParam);
                AppLibDisp_GetWindowConfig(DISP_CH_DCHAN, AppLibDisp_GetWindowId(DISP_CH_DCHAN, 0), &Window);
                Window.Source = AMP_DISP_ENC;
                Window.CropArea.Width = 0;
                Window.CropArea.Height = 0;
                Window.CropArea.X = 0;
                Window.CropArea.Y = 0;
                Window.TargetAreaOnPlane.Width = PreviewParam.Preview.Width;
                Window.TargetAreaOnPlane.Height = PreviewParam.Preview.Height;
                Window.TargetAreaOnPlane.X = PreviewParam.Preview.X;
                Window.TargetAreaOnPlane.Y = PreviewParam.Preview.Y;
                AppLibDisp_SetWindowConfig(DISP_CH_DCHAN, AppLibDisp_GetWindowId(DISP_CH_DCHAN, 0), &Window);
                AppLibDisp_ActivateWindow(DISP_CH_DCHAN, AppLibDisp_GetWindowId(DISP_CH_DCHAN, 0));
                AppLibGraph_SetWindowConfig(GRAPH_CH_DCHAN);
                AppLibGraph_ActivateWindow(GRAPH_CH_DCHAN);
                AppLibGraph_EnableDraw(GRAPH_CH_DCHAN);
                AppLibDisp_FlushWindow(DISP_CH_DCHAN);

            }
        }
    }

    rec_cam.Gui(GUI_FLUSH, 0, 0);
}
    return ReturnValue;
}
static int rec_cam_change_display(void)
{
    int ReturnValue = 0;

    /** Setup the display. */
    AppLibDisp_SelectDevice(DISP_CH_FCHAN | DISP_CH_DCHAN, DISP_ANY_DEV);
    AppLibDisp_ConfigMode(DISP_CH_FCHAN | DISP_CH_DCHAN, AppLibSysVout_GetVoutMode(VOUT_DISP_MODE_1080P));
    AppLibDisp_SetupChan(DISP_CH_FCHAN | DISP_CH_DCHAN);
    AppLibDisp_ChanStart(DISP_CH_FCHAN | DISP_CH_DCHAN);
    /** Setup the Window. */
    {
        AMP_DISP_WINDOW_CFG_s Window;
        APPLIB_VOUT_PREVIEW_PARAM_s PreviewParam = {0};
        AMP_DISP_INFO_s DispDev = {0};

        memset(&Window, 0x0, sizeof(AMP_DISP_WINDOW_CFG_s));

        if (app_status.CurrEncMode == APP_VIDEO_ENC_MODE) {
            APPLIB_SENSOR_VIDEO_ENC_CONFIG_s *VideoEncConfigData = NULL;
            VideoEncConfigData = AppLibSysSensor_GetVideoConfig(AppLibVideoEnc_GetSensorVideoRes());
            PreviewParam.AspectRatio = VideoEncConfigData->VAR;
        } else {
            APPLIB_SENSOR_STILLPREV_CONFIG_s *StillLiveViewConfigData = NULL;
            StillLiveViewConfigData = (APPLIB_SENSOR_STILLPREV_CONFIG_s *)AppLibSysSensor_GetPhotoLiveviewConfig(AppLibStillEnc_GetPhotoPjpegCapMode(), AppLibStillEnc_GetPhotoPjpegConfigId());
            PreviewParam.AspectRatio = StillLiveViewConfigData->VAR;
        }

        ReturnValue = AppLibDisp_GetDeviceInfo(DISP_CH_FCHAN, &DispDev);
        if ((ReturnValue < 0) || (DispDev.DeviceInfo.Enable == 0)) {
            DBGMSG("[app_rec_cam] FChan Disable. Disable the fchan Window");
            AppLibDisp_DeactivateWindow(DISP_CH_FCHAN, AppLibDisp_GetWindowId(DISP_CH_FCHAN, 0));
            AppLibGraph_DisableDraw(GRAPH_CH_FCHAN);
        } else {
            /** FCHAN Window*/
            PreviewParam.ChanID = DISP_CH_FCHAN;
            AppLibDisp_CalcPreviewWindowSize(&PreviewParam);
            AppLibDisp_GetWindowConfig(DISP_CH_FCHAN, AppLibDisp_GetWindowId(DISP_CH_FCHAN, 0), &Window);
            Window.Source = AMP_DISP_ENC;
            Window.CropArea.Width = 0;
            Window.CropArea.Height = 0;
            Window.CropArea.X = 0;
            Window.CropArea.Y = 0;
            Window.TargetAreaOnPlane.Width = PreviewParam.Preview.Width;
            Window.TargetAreaOnPlane.Height = PreviewParam.Preview.Height;
            Window.TargetAreaOnPlane.X = PreviewParam.Preview.X;
            Window.TargetAreaOnPlane.Y = PreviewParam.Preview.Y;
            AppLibDisp_SetWindowConfig(DISP_CH_FCHAN, AppLibDisp_GetWindowId(DISP_CH_FCHAN, 0), &Window);
            AppLibDisp_ActivateWindow(DISP_CH_FCHAN, AppLibDisp_GetWindowId(DISP_CH_FCHAN, 0));
            AppLibGraph_EnableDraw(GRAPH_CH_FCHAN);
        }

        ReturnValue = AppLibDisp_GetDeviceInfo(DISP_CH_DCHAN, &DispDev);
        if ((ReturnValue < 0) || (DispDev.DeviceInfo.Enable == 0)) {
            DBGMSG("[app_rec_cam] DChan Disable. Disable the Dchan Window");
            AppLibDisp_DeactivateWindow(DISP_CH_DCHAN, AppLibDisp_GetWindowId(DISP_CH_DCHAN, 0));
            AppLibGraph_DisableDraw(GRAPH_CH_DCHAN);
        } else {
            /** DCHAN Window*/
            PreviewParam.ChanID = DISP_CH_DCHAN;
            AppLibDisp_CalcPreviewWindowSize(&PreviewParam);
            AppLibDisp_GetWindowConfig(DISP_CH_DCHAN, AppLibDisp_GetWindowId(DISP_CH_DCHAN, 0), &Window);
            Window.Source = AMP_DISP_ENC;
            Window.CropArea.Width = 0;
            Window.CropArea.Height = 0;
            Window.CropArea.X = 0;
            Window.CropArea.Y = 0;
            Window.TargetAreaOnPlane.Width = PreviewParam.Preview.Width;
            Window.TargetAreaOnPlane.Height = PreviewParam.Preview.Height;
            Window.TargetAreaOnPlane.X = PreviewParam.Preview.X;
            Window.TargetAreaOnPlane.Y = PreviewParam.Preview.Y;
            AppLibDisp_SetWindowConfig(DISP_CH_DCHAN, AppLibDisp_GetWindowId(DISP_CH_DCHAN, 0), &Window);
            AppLibDisp_ActivateWindow(DISP_CH_DCHAN, AppLibDisp_GetWindowId(DISP_CH_DCHAN, 0));
            AppLibGraph_EnableDraw(GRAPH_CH_DCHAN);
        }
        AppLibDisp_FlushWindow(DISP_CH_FCHAN | DISP_CH_DCHAN);
    }

    return ReturnValue;
}

static int rec_cam_change_osd(void)
{
    int ReturnValue = 0;

    // ToDo: need to remove to handler when iav completes the dsp cmd queue mechanism
    /* Update graphic window*/
    AppLibGraph_SetWindowConfig(GRAPH_CH_FCHAN | GRAPH_CH_DCHAN);
    AppLibGraph_ActivateWindow(GRAPH_CH_FCHAN | GRAPH_CH_DCHAN);
#ifdef CONFIG_APP_ARD
    if(AppLibDisp_GetRotate(DISP_CH_DCHAN) == AMP_ROTATE_90) {
        rec_cam.Gui(GUI_SET_LAYOUT, 0, 0);
        rec_cam.Gui(GUI_FLUSH, 0, 0);
        AppLibGraph_FlushWindow(GRAPH_CH_FCHAN | GRAPH_CH_DCHAN);
    } else {
#endif
    AppLibGraph_FlushWindow(GRAPH_CH_FCHAN | GRAPH_CH_DCHAN);
    rec_cam.Gui(GUI_SET_LAYOUT, 0, 0);
    rec_cam.Gui(GUI_FLUSH, 0, 0);
#ifdef CONFIG_APP_ARD
    }
#endif
    return ReturnValue;
}

static int rec_cam_usb_connect(void)
{
    int ReturnValue = 0;
#if defined(CONFIG_APP_AMBA_LINK)
    if (rec_cam.RecCapState == REC_CAP_STATE_VF) {
        rec_cam.Func(REC_CAM_VF_STOP, 0, 0);
    }
#endif

    if (!APP_CHECKFLAGS(app_rec_cam.GFlags, APP_AFLAGS_READY) ||
        APP_CHECKFLAGS(app_rec_cam.GFlags, APP_AFLAGS_BUSY)) {
        return ReturnValue;
    }

    /* The flow after connecting the USB cable. */
    switch (UserSetting->SetupPref.USBMode) {
    case USB_MODE_AMAGE:
        AppAppMgt_SwitchApp(APP_USB_AMAGE);
        break;
    case USB_MODE_MSC:
    default:
        AppAppMgt_SwitchApp(APP_USB_MSC);
        break;
    }

    return ReturnValue;
}

/**
 *  @brief To show the gui of current application
 *
 *  To show the gui of current application
 *
 *  @return >=0 success, <0 failure
 */
static int rec_cam_start_show_gui(void)
{
    int ReturnValue = 0;
    int GuiParam = 0;
     AmbaPrint("rec_cam_start_show_gui:%d\n",UserSetting->MotionDetectPref.MotionDetect);
    if(UserSetting->MotionDetectPref.MotionDetect==MOTION_DETECT_ON)
    {
        rec_cam.Gui(GUI_DETECT_MOTION_ICON_SHOW, 0, 0);
    }
    else
    {
        rec_cam.Gui(GUI_DETECT_MOTION_ICON_HIDE, 0, 0);
    }
    
    /*parkingmonitor is removed*/
#if 0
    if(UserSetting->VideoPref.parkingmode_sensitivity==MENU_VIDEO_PARKINGMODE_ON)
    {
        rec_cam.Gui(GUI_PARKING_MNITOR_ICON_SHOW, 0, 0);
    }
    else
    {
        rec_cam.Gui(GUI_PARKING_MNITOR_ICON_HIDE, 0, 0);
    }
#endif

    if(UserSetting->SetupPref.adas_auto_cal_onoff==1)
    {
        rec_cam.Gui(GUI_CALIBRATION_ICON_SHOW, 0, 0);
    }
    else
    {
        rec_cam.Gui(GUI_CALIBRATION_ICON_HIDE, 0, 0);
    }
    // check encode status
    #ifndef CONFIG_ECL_GUI
    if (app_status.CurrEncMode == APP_VIDEO_ENC_MODE) {
        rec_cam.Gui(GUI_APP_VIDEO_ICON_SHOW, 0, 0);
    } else {
        rec_cam.Gui(GUI_APP_PHOTO_ICON_SHOW, 0, 0);
    }
    #endif
    // show power status
    rec_cam.Gui(GUI_POWER_STATE_UPDATE, app_status.PowerType, app_status.BatteryState);
    rec_cam.Gui(GUI_POWER_STATE_SHOW, app_status.PowerType, app_status.BatteryState);

    // show SD card status
    ReturnValue = AppLibCard_CheckStatus(0);
    if (ReturnValue == CARD_STATUS_NO_CARD) {
        GuiParam = GUI_NO_CARD;
    } else {
        GuiParam = GUI_CARD_READY;
    }
    rec_cam.Gui(GUI_CARD_UPDATE, GuiParam, 0);
    rec_cam.Gui(GUI_CARD_SHOW, 0, 0);

#ifdef CONFIG_APP_ARD
    if (app_status.CurrEncMode == APP_VIDEO_ENC_MODE) {
    rec_cam.Gui(GUI_PHOTO_SIZE_HIDE, 0, 0);
    rec_cam.Gui(GUI_PHOTO_QUALITY_HIDE, 0, 0);
    rec_cam.Gui(GUI_CAP_MODE_HIDE, 0, 0);
#endif

    #ifndef CONFIG_ECL_GUI
    // show selftimer
    rec_cam.Gui(GUI_SELFTIMER_UPDATE, UserSetting->VideoPref.VideoSelftimer, 0);
    rec_cam.Gui(GUI_SELFTIMER_SHOW, 0, 0);

    // show video resolution
    rec_cam.Gui(GUI_VIDEO_SENSOR_RES_UPDATE, UserSetting->VideoPref.SensorVideoRes, 0);
    rec_cam.Gui(GUI_VIDEO_SENSOR_RES_SHOW, 0, 0);

    // show quality
    rec_cam.Gui(GUI_VIDEO_QUALITY_UPDATE, UserSetting->VideoPref.VideoQuality, 0);
    rec_cam.Gui(GUI_VIDEO_QUALITY_SHOW, 0, 0);

    // show mode
    rec_cam.Gui(GUI_REC_MODE_UPDATE, 0, 0);
    rec_cam.Gui(GUI_REC_MODE_SHOW, 0, 0);

    rec_cam.Gui(GUI_SPLIT_TIME_ICON_SHOW,0,0);

    if( UserSetting->VideoPref.SensorVideoRes == SENSOR_VIDEO_RES_TRUE_HDR_1080P_HALF ||
        UserSetting->VideoPref.SensorVideoRes == SENSOR_VIDEO_RES_WQHD_HALF_HDR) {
        rec_cam.Gui(GUI_HDR_ICON_SHOW,0,0);
    } else {
        rec_cam.Gui(GUI_HDR_ICON_HIDE,0,0);
    }
 #endif
    }

#ifdef CONFIG_APP_ARD
    if (app_status.CurrEncMode == APP_STILL_ENC_MODE) {
    rec_cam.Gui(GUI_SELFTIMER_HIDE, 0, 0);
    rec_cam.Gui(GUI_VIDEO_SENSOR_RES_HIDE, 0, 0);
    rec_cam.Gui(GUI_VIDEO_QUALITY_HIDE, 0, 0);
    rec_cam.Gui(GUI_SPLIT_TIME_ICON_HIDE,0,0);
    rec_cam.Gui(GUI_HDR_ICON_HIDE,0,0);
#endif
#if 0
    // show photo resolution
    rec_cam.Gui(GUI_PHOTO_SIZE_UPDATE, UserSetting->PhotoPref.PhotoSize, 0);
    rec_cam.Gui(GUI_PHOTO_SIZE_SHOW, 0, 0);
    // show quality
    rec_cam.Gui(GUI_PHOTO_QUALITY_UPDATE, UserSetting->PhotoPref.PhotoQuality, 0);
    rec_cam.Gui(GUI_PHOTO_QUALITY_SHOW, 0, 0);
    rec_cam.Gui(GUI_CAP_MODE_UPDATE, UserSetting->PhotoPref.PhotoCapMode, 0);
    rec_cam.Gui(GUI_CAP_MODE_SHOW, 0, 0);
 #endif
#ifdef CONFIG_APP_ARD
    rec_cam.Gui(GUI_EVENT_NUM_HIDE,0,0);
    }
#endif

#ifdef CONFIG_APP_ARD
    if (app_status.CurrEncMode == APP_VIDEO_ENC_MODE) {
        rec_cam.Gui(GUI_MIC_ICON_SHOW, UserSetting->VideoPref.MicMute, 0);
    } else {
        rec_cam.Gui(GUI_MIC_ICON_HIDE, UserSetting->VideoPref.MicMute, 0);
    }
#endif
    // draw
    rec_cam.Gui(GUI_FLUSH, 0, 0);
#ifdef CONFIG_APP_ARD
    AppLibSysLcd_SetBrightness(LCD_CH_DCHAN,1,0);
    //AppLibSysLcd_SetBacklight(LCD_CH_DCHAN, 1);
#endif
    
#ifndef CONFIG_ECL_GUI
    rec_cam.Gui(GUI_GPS_STATUS_ICON_SHOW, app_status.gps_status, 0);
    AppLibComSvcTimer_Register(TIMER_1HZ, rec_cam_gps_status_timer_handler);
#endif

#ifdef CONFIG_ECL_GUI
    if (app_status.CurrEncMode == APP_VIDEO_ENC_MODE) {
        rec_cam.Func(REC_CAR_VIDEO_EVENT_FILE_NUM_UPDATE,0,0);
    } else if (app_status.CurrEncMode == APP_STILL_ENC_MODE){
        rec_cam.Func(REC_CAR_PHOTO_FILE_NUM_UPDATE,0,0);
    }
    AppLibComSvcTimer_Register(TIMER_1HZ, rec_cam_show_data_timer_handler);	
#endif

    return ReturnValue;
}

static int rec_cam_calibration_icn(UINT32 param1)
{
    int ReturnValue = 0;
    if(param1==1)
    {
        rec_cam.Gui(GUI_CALIBRATION_ICON_SHOW,0, 0);
    }
    else
    {
        rec_cam.Gui(GUI_CALIBRATION_ICON_HIDE, 0, 0);
    }
        
    return ReturnValue;
}

static int rec_cam_update_bat_power_status(int param1)
{
    int ReturnValue = 0;

    /* Update the gui of power's status. */
    if (param1 == 0) {
        /*Hide the battery gui.*/
        rec_cam.Gui(GUI_POWER_STATE_HIDE, GUI_HIDE_POWER_EXCEPT_DC, 0);
    } else if (param1 == 1) {
        /*Update the battery gui.*/
        rec_cam.Gui(GUI_POWER_STATE_UPDATE, app_status.PowerType, app_status.BatteryState);
        rec_cam.Gui(GUI_POWER_STATE_SHOW, app_status.PowerType, app_status.BatteryState);
    } else if (param1 == 2) {
        /*Reset the battery and power gui.*/
        rec_cam.Gui(GUI_POWER_STATE_HIDE, 0, 0);
        rec_cam.Gui(GUI_POWER_STATE_UPDATE, app_status.PowerType, app_status.BatteryState);
        rec_cam.Gui(GUI_POWER_STATE_SHOW, app_status.PowerType, app_status.BatteryState);
    }
    rec_cam.Gui(GUI_FLUSH, 0, 0);

    return ReturnValue;
}

/**
 *  @brief The timer handler of warning message.
 *
 *  To show and hide the warning message.
 *
 *  @param [in] eid Event id
 *
 *  @return >=0 success, <0 failure
 */
static void rec_cam_warning_timer_handler(int eid)
{
    static int blink_count = 0;

    if (eid == TIMER_UNREGISTER) {
        blink_count = 0;
        return;
    }

    blink_count++;
    if (blink_count & 0x01) {
        if (blink_count >= 5) {
            if (!APP_CHECKFLAGS(app_rec_cam.Flags, REC_CAM_FLAGS_MEM_RUNOUT)) {
                APP_REMOVEFLAGS(app_rec_cam.Flags, REC_CAM_FLAGS_WARNING_MSG_RUN);
                AppLibComSvcTimer_Unregister(TIMER_2HZ, rec_cam_warning_timer_handler);
            }
        }

        rec_cam.Gui(GUI_WARNING_HIDE, 0, 0);
    } else {
        rec_cam.Gui(GUI_WARNING_SHOW, 0, 0);
    }

    rec_cam.Gui(GUI_FLUSH, 0, 0);

}

static int rec_cam_warning_msg_show(int enable, int param1, int param2)
{
    int ReturnValue = 0;

    if (enable) {
        /* To show the warning message. */
        if (param2) {
            rec_cam.Gui(GUI_WARNING_UPDATE, param1, 0);
            rec_cam.Gui(GUI_WARNING_SHOW, 0, 0);
        } else if (!APP_CHECKFLAGS(app_rec_cam.Flags, REC_CAM_FLAGS_WARNING_MSG_RUN)) {
            APP_ADDFLAGS(app_rec_cam.Flags, REC_CAM_FLAGS_WARNING_MSG_RUN);
            rec_cam.Gui(GUI_WARNING_UPDATE, param1, 0);
            rec_cam.Gui(GUI_WARNING_SHOW, 0, 0);

            AppLibComSvcTimer_Register(TIMER_2HZ, rec_cam_warning_timer_handler);
        }
    } else {
        /* To disable the warning message. */
        if (APP_CHECKFLAGS(app_rec_cam.Flags, REC_CAM_FLAGS_WARNING_MSG_RUN)) {
            APP_REMOVEFLAGS(app_rec_cam.Flags, REC_CAM_FLAGS_WARNING_MSG_RUN);
            AppLibComSvcTimer_Unregister(TIMER_2HZ, rec_cam_warning_timer_handler);
        }
        rec_cam.Gui(GUI_WARNING_HIDE, 0, 0);
    }
    rec_cam.Gui(GUI_FLUSH, 0, 0);

    return ReturnValue;
}

static int rec_cam_card_fmt_nonoptimum(void)
{
    int ReturnValue = 0;

#if defined(CONFIG_APP_AMBA_LINK)
    if (rec_cam.RecCapState == REC_CAP_STATE_VF) {
        rec_cam.Func(REC_CAM_VF_STOP, 0, 0);
    }
    APP_ADDFLAGS(app_rec_cam.GFlags, APP_AFLAGS_POPUP);
#endif

    return ReturnValue;
}

static int rec_cam_adas_event_handler(UINT32 msg)
{
    int ReturnValue = 0;

    return ReturnValue;
}

static int rec_cam_adas_function_init(void)
{
    int ReturnValue = 0;
  
    return ReturnValue;
}

static int rec_cam_adas_update_param(void)
{
    int ReturnValue = 0;

    return ReturnValue;
}

#ifdef CONFIG_APP_ARD
static int motion_detect_start_cd = 0;
static void rec_cam_motion_detect_start_timer_cd(int eid)
{
    int ReturnValue = 0;
    if (eid == TIMER_UNREGISTER) {
        return;
    }
    if(--motion_detect_start_cd == 0){
      AppLibComSvcTimer_Unregister(TIMER_1HZ, rec_cam_motion_detect_start_timer_cd);
      ReturnValue = AppLibVideoAnal_MD_Enable();
      AmbaPrintColor(YELLOW, "--------AppLibVideoAnal_MD_Enable return = %d", ReturnValue);

    }
}
static int rec_cam_motion_detect_start(void)
{
    if(rec_cam.MotionDetectStatus == REC_CAP_MOTION_DETECT_STOP){
    rec_cam.MotionDetectStatus = REC_CAP_MOTION_DETECT_START;
    motion_detect_start_cd = 5;
    rec_cam.MotionDetectRecordRemainTime = 0;
    AppLibComSvcTimer_Register(TIMER_1HZ, rec_cam_motion_detect_start_timer_cd);
    }
    return 0;
}
static int rec_cam_motion_detect_stop(void)
{
    int ReturnValue = 0;
    rec_cam.MotionDetectStatus = REC_CAP_MOTION_DETECT_STOP;
    motion_detect_start_cd = 0;
    AppLibComSvcTimer_Unregister(TIMER_1HZ, rec_cam_motion_detect_start_timer_cd);
    rec_cam.MotionDetectRecordRemainTime = 0;
    ReturnValue = AppLibVideoAnal_MD_Disable();
    AmbaPrintColor(YELLOW, "--------AppLibVideoAnal_MD_Disable return = %d", ReturnValue);
    return 0;
}
static int rec_cam_motion_detect_record_handler(void)
{
#ifdef CONFIG_APP_ARD
#ifdef CONFIG_APP_EVENT_OVERLAP
    if (AppLibFormatMuxMp4_GetEventStatus()==0)
#else
    if (AppLibFormat_GetEventStatus()==0)
#endif
#endif
    rec_cam.MotionDetectRecordRemainTime = 10;
    if (rec_cam.RecCapState == REC_CAP_STATE_PREVIEW ) {
        rec_cam.Func(REC_CAM_RECORD_AUTO_START, 0, 0);
    }
    return 0;
}

static int rec_cam_video_set_gsensor_sensitivity(int mode)
{

    AppLibSysGyro_Set_Event_Sensitivity(mode);
    if(mode==1||mode==2||mode==3)
    AppLibSysGyro_Detect_Task_En(1);
    else
    AppLibSysGyro_Detect_Task_En(0);
    return 0;
}

#endif

#if defined(CONFIG_APP_AMBA_LINK)
static int rec_cam_boos_booted(void)
{
    if (APP_CHECKFLAGS(app_rec_cam.GFlags, APP_AFLAGS_POPUP)) {
        /* Don't start VF when menu is on. */
        DBGMSGc2(GREEN, "[rec_cam] <boos_booted> APP_AFLAGS_POPUP");
        return 0;
    }

    if (UserSetting->VideoPref.StreamType != STREAM_TYPE_RTSP){
        return 0;
    }

    if (rec_cam.RecCapState == REC_CAP_STATE_PREVIEW) {
        if (app_status.CurrEncMode == APP_VIDEO_ENC_MODE) {
            /* start view finder */
            if (UserSetting->VideoPref.StreamType == STREAM_TYPE_RTSP) {
                rec_cam.Func(REC_CAM_VF_START, 0, 0);
            }
        }
    } else if (rec_cam.RecCapState == REC_CAP_STATE_RECORD) {
        NotifyNetFifoOfAppState(AMP_NETFIFO_NOTIFY_STARTENC);
    } else {
        ;
    }

    return 0;
}

static int rec_cam_vf_start(void)
{
    int ReturnValue = 0;

    if (AppLibNetBase_GetBootStatus() == 0) {
        DBGMSGc2(GREEN,"[rec_cam] <vf_start> AMBA Link not booted yet! Do nothing!");
        return 0;
    }

    if (rec_cam.RecCapState != REC_CAP_STATE_PREVIEW) {
        DBGMSGc2(GREEN,"[rec_cam] <vf_start> RecCapState is not REC_CAP_STATE_PREVIEW");
        return -1;
    }

    if (APP_CHECKFLAGS(app_rec_cam.GFlags, APP_AFLAGS_POPUP)) {
        /* Don't start VF when menu is on. */
        DBGMSGc2(GREEN, "[rec_cam] <vf_start> APP_AFLAGS_POPUP");
        return -1;
    }

    /* This check condition is for the case that view finder is stopped through netCtrol command AMSG_NETCTRL_VF_STOP.
          REC_CAM_FLAGS_VF_DISABLE flag will be set when receiving AMSG_NETCTRL_VF_STOP. */
    if (APP_CHECKFLAGS(rec_cam.NetCtrlFlags, APP_NETCTRL_FLAGS_VF_DISABLE)) {
        DBGMSGc2(GREEN,"[rec_cam] <vf_start> VF function is disabled through netCtrl");
        return -1;
    }

    /* Setup the encode setting. */
    AppLibVideoEnc_EncodeSetup();

    ReturnValue = NotifyNetFifoOfAppState(AMP_NETFIFO_NOTIFY_STARTENC);
    if (ReturnValue == 0) {
        rec_cam.RecCapState = REC_CAP_STATE_TRANSIT_TO_VF;

        /* AppLibVideoEnc_EncodeStart() should be postponed until
            message 'AMSG_NETFIFO_EVENT_START' is received. */
    }

#ifdef CONFIG_APP_ARD
    #if defined(CONFIG_APP_STAMP)
        rec_cam_setup_stamp();
    #endif
#endif
    return 0;
}

static int rec_cam_vf_stop(void)
{
    if (AppLibNetBase_GetBootStatus() == 0) {
        DBGMSGc2(GREEN,"[rec_cam] <vf_stop> AMBA Link not booted yet! Do nothing!");
        return 0;
    }

    if (rec_cam.RecCapState != REC_CAP_STATE_VF) {
        DBGMSGc2(RED,"[rec_cam] <vf_stop> RecCapState is not REC_CAP_STATE_VF");
        return -1;
    }
    DBGMSGc2(GREEN,"[rec_cam] <vf_stop> EncodeStop");

#if defined(CONFIG_APP_STAMP)
    rec_cam_stop_stamp();
#endif
    /* Start video encoding. */
    AppLibVideoEnc_EncodeStop();
    rec_cam.RecCapState = REC_CAP_STATE_RESET;

    /* Stop the timer when stopping view finder. */
    AppLibComSvcTimer_Unregister(TIMER_1HZ, rec_cam_rec_timer_handler);

    /* Update the gui. */
    rec_cam.Gui(GUI_REC_STATE_HIDE, 0, 0);
    rec_cam.Gui(GUI_REC_TIMER_HIDE, 0, 0);
    rec_cam.Gui(GUI_FLUSH, 0, 0);

    return 0;
}

static int rec_cam_vf_switch_to_record(void)
{
    int ReturnValue = 0;

    if (AppLibNetBase_GetBootStatus() == 0) {
        DBGMSGc2(GREEN,"[rec_cam] <vf_switch_to_record> AMBA Link not booted yet! Do nothing!");
        return -1;
    }

    if (rec_cam.RecCapState != REC_CAP_STATE_VF) {
        DBGMSGc2(RED,"[rec_cam] <vf_switch_to_record> RecCapState is not REC_CAP_STATE_VF");
        return -1;
    }

    /* Check the card's status. */
    ReturnValue = rec_cam.Func(REC_CAM_CARD_CHECK_STATUS, 0, 0);
    if (ReturnValue != 0) {
        return -1;
    }

    /* Set time lapse. */
    rec_cam.TimeLapseTime = AppLibVideoEnc_GetTimeLapse();

#ifdef CONFIG_APP_ARD
    if(AppLibVideoEnc_GetPreRecord() == 1){
        /* Register the timer to show the record time. */
         if(rec_cam.RecTime >= 15)
             rec_cam.RecTime = 15;
         AppLibVideoEnc_SetPreRecordTime(rec_cam.RecTime);
    }else{
        rec_cam.RecTime = 0;
    }
#else
    /* Register the timer to show the record time. */
    rec_cam.RecTime = 0;
#endif

    /* Initialize and start the muxer. */
    AppLibFormat_MuxerInit();
    AppLibFormatMuxMp4_StartOnRecording();
#ifdef CONFIG_APP_ARD
    if(UserSetting->VideoPref.video_split_rec_time!=VIDEO_SPLIT_REC_OFF)
#endif
    AppLibFormatMuxMp4_SetAutoSplitFileType(1);/**set the auto split file type*/
#ifdef CONFIG_APP_ARD
    else
        AppLibFormatMuxMp4_SetAutoSplitFileType(0);/**disable the auto split file type*/
#endif

    /* Enable the storage monitor.*/
    AppLibMonitorStorage_Enable(1);
    AppLibMonitorStorage_EnableMsg(1);

#ifndef CONFIG_APP_ARD
    /* Enable stamp */
    #if defined(CONFIG_APP_STAMP)
    rec_cam_setup_stamp();
    #endif
#endif


    rec_cam.RecCapState = REC_CAP_STATE_RECORD;

    APP_ADDFLAGS(app_rec_cam.GFlags, APP_AFLAGS_BUSY);

    /* Update the gui. */
    rec_cam.Gui(GUI_REC_STATE_UPDATE, GUI_REC_START, 0);
    rec_cam.Gui(GUI_REC_STATE_SHOW, 0, 0);
    rec_cam.Gui(GUI_REC_TIMER_UPDATE, rec_cam.RecTime, 0);
    rec_cam.Gui(GUI_REC_TIMER_SHOW, 0, 0);
    rec_cam.Gui(GUI_FLUSH, 0, 0);

    return 0;
}

static int rec_cam_capture_on_vf(void)
{
    if (rec_cam.RecCapState != REC_CAP_STATE_VF) {
        return -1;
    }

    APP_ADDFLAGS(app_rec_cam.Flags,REC_CAM_FLAGS_CAPTURE_ON_VF);
    rec_cam.Func(REC_CAM_VF_STOP, 0, 0);

    return 0;
}

static int rec_cam_netfifo_event_start(void)
{
    int ReturnValue = 0;

    AmbaPrintColor(GREEN,"[rec_cam] <netfifo_event_start> AMSG_NETFIFO_EVENT_START");

    /* Virtual fifo of net stream has been created when AMSG_NETFIFO_EVENT_START is received. */
    if (rec_cam.RecCapState == REC_CAP_STATE_TRANSIT_TO_VF) {
        AmbaPrintColor(GREEN,"[rec_cam] <netfifo_event_start> EncodeStart");

        /* Set time lapse. */
        rec_cam.TimeLapseTime = AppLibVideoEnc_GetTimeLapse();

        /* Register the timer to show the view finder time. */
        rec_cam.RecTime = 0;

        AppLibComSvcTimer_Register(TIMER_1HZ, rec_cam_rec_timer_handler);
        rec_cam.Func(REC_CAM_RECORD_LED_START, 1, 0);

        /* Start video encoding. */
        AppLibVideoEnc_EncodeStart();

        APP_ADDFLAGS(app_rec_cam.GFlags, APP_AFLAGS_BUSY);
        rec_cam.RecCapState = REC_CAP_STATE_VF;

        if (APP_CHECKFLAGS(rec_cam.NetCtrlFlags, APP_NETCTRL_FLAGS_VF_RESET_DONE)) {
            APP_REMOVEFLAGS(rec_cam.NetCtrlFlags, APP_NETCTRL_FLAGS_VF_RESET_DONE);
            AppLibNetControl_ReplyErrorCode(AMBA_RESET_VF, 0);
        }

        /* Update the gui. */
        rec_cam.Gui(GUI_REC_STATE_UPDATE, GUI_REC_PRE_RECORD, 0);
        rec_cam.Gui(GUI_REC_STATE_SHOW, 0, 0);
        rec_cam.Gui(GUI_REC_TIMER_UPDATE, rec_cam.RecTime, 0);
        rec_cam.Gui(GUI_REC_TIMER_SHOW, 0, 0);
        rec_cam.Gui(GUI_FLUSH, 0, 0);
        if (APP_CHECKFLAGS(app_rec_cam.Flags, REC_CAM_FLAGS_MEM_RUNOUT)) {
        AmbaPrintColor(YELLOW, "[rec_cam] <record> VF -> REC auto due to memory runout");
        rec_cam.Func(REC_CAM_VF_SWITCH_TO_RECORD, 0, 0);
        APP_REMOVEFLAGS(app_rec_cam.Flags, REC_CAM_FLAGS_MEM_RUNOUT);
        }
#ifndef CONFIG_APP_ARD
        /* Enable stamp */
        #if defined(CONFIG_APP_STAMP)
        rec_cam_setup_stamp();
        #endif
#endif

    }

    return ReturnValue;
}

static int rec_cam_netfifo_event_stop(void)
{
    int ReturnValue = 0;

    AmbaPrintColor(GREEN,"[rec_cam] <netfifo_event_stop> AMSG_NETFIFO_EVENT_STOP");

    APP_REMOVEFLAGS(app_rec_cam.Flags, REC_CAM_FLAGS_NETFIFO_BUSY);
    if (!rec_cam_system_is_busy()) {
        APP_REMOVEFLAGS(app_rec_cam.GFlags, APP_AFLAGS_BUSY);
        if (APP_CHECKFLAGS(rec_cam.NetCtrlFlags, APP_NETCTRL_FLAGS_VF_STOP_DONE)) {
            APP_REMOVEFLAGS(rec_cam.NetCtrlFlags, APP_NETCTRL_FLAGS_VF_STOP_DONE);
            AppLibNetControl_ReplyErrorCode(AMBA_STOP_VF, 0);
        }

        /* To excute the functions that system block them when the Busy flag is enabled. */
        AppUtil_BusyCheck(0);
        if (!APP_CHECKFLAGS(app_rec_cam.GFlags, APP_AFLAGS_READY)) {
            return ReturnValue;/**<  App switched out*/
        }

        if (app_status.CurrEncMode == APP_VIDEO_ENC_MODE) {
            if (APP_CHECKFLAGS(app_rec_cam.Flags,REC_CAM_FLAGS_CAPTURE_ON_VF)) {
                DBGMSGc2(CYAN, "[rec_cam] <netfifo_event_stop> switch liveivew");
                AppLibVideoEnc_LiveViewStop();
                AppLibStillEnc_LiveViewSetup();
                AppLibStillEnc_LiveViewStart();
                app_status.CurrEncMode = APP_STILL_ENC_MODE;
            } else {
                if (UserSetting->VideoPref.StreamType == STREAM_TYPE_RTSP) {
                        DBGMSGc2(GREEN, "[rec_cam] <netfifo_event_stop> REC_CAM_VF_START");
                        rec_cam.Func(REC_CAM_VF_START, 0, 0);
                }
            }
        }
    }

    return ReturnValue;
}

static int rec_cam_netctrl_capture_done(void)
{
    UINT64 FileObjID = 0;
    char CurFn[APP_MAX_FN_SIZE] = {0};
    APPLIB_JSON_OBJECT *JsonObject = NULL;
    int ErrorCode = 0;
    int ReturnValue = 0;

    APP_REMOVEFLAGS(app_rec_cam.Flags,REC_CAM_FLAGS_CAPTURE_FROM_NETCTRL);
    if (UserSetting->PhotoPref.PhotoCapMode == PHOTO_CAP_MODE_PRECISE) {
        ErrorCode = ERROR_NETCTRL_UNKNOWN_ERROR;
        FileObjID = AppLibStorageDmf_GetLastFilePos(APPLIB_DCF_MEDIA_IMAGE, DCIM_HDLR);
        if (FileObjID > 0) {
            ReturnValue = AppLibStorageDmf_GetFileName(APPLIB_DCF_MEDIA_IMAGE, ".JPG", APPLIB_DCF_EXT_OBJECT_SPLIT_FILE, DCIM_HDLR, 0, FileObjID, CurFn);
            if (ReturnValue == 0) {
                AmbaPrint("[rec_cam] <netctrl_take_photo_done> CurFn: %s", CurFn);
                ErrorCode = 0;
            }
        }

        if (AppLibNetJsonUtility_CreateObject(&JsonObject) == 0) {
            AppLibNetJsonUtility_AddIntegerObject(JsonObject,"rval", ErrorCode);
            AppLibNetJsonUtility_AddIntegerObject(JsonObject,"msg_id", AMBA_TAKE_PHOTO);
            if (ErrorCode == 0) {
                AppLibNetJsonUtility_AddStringObject(JsonObject,"param", CurFn);
            }

            SendJsonString(JsonObject);
            AppLibNetJsonUtility_FreeJsonObject(JsonObject);
        } else {
            AppLibNetControl_ReplyErrorCode(AMBA_TAKE_PHOTO, ERROR_NETCTRL_UNKNOWN_ERROR);
        }

    }
    else if (UserSetting->PhotoPref.PhotoCapMode == PHOTO_CAP_MODE_BURST) {
        ErrorCode = ERROR_NETCTRL_UNKNOWN_ERROR;
        FileObjID = AppLibStorageDmf_GetLastFilePos(APPLIB_DCF_MEDIA_IMAGE, DCIM_HDLR);
        if (FileObjID > 0) {
            ReturnValue = AppLibStorageDmf_GetFileName(APPLIB_DCF_MEDIA_IMAGE, ".JPG", APPLIB_DCF_EXT_OBJECT_SPLIT_FILE, DCIM_HDLR, 0, FileObjID, CurFn);
            if (ReturnValue == 0) {
                AmbaPrint("[rec_cam] <netctrl_take_photo_done> CurFn: %s", CurFn);
                ErrorCode = 0;
            }
        }

        if (AppLibNetJsonUtility_CreateObject(&JsonObject) == 0) {
            AppLibNetJsonUtility_AddIntegerObject(JsonObject,"msg_id",AMBA_NOTIFICATION);
            AppLibNetJsonUtility_AddStringObject(JsonObject,"type", "continue_burst_complete");

            if (ErrorCode == 0) {
                AppLibNetJsonUtility_AddStringObject(JsonObject,"param", CurFn);
            }

            SendJsonString(JsonObject);
            AppLibNetJsonUtility_FreeJsonObject(JsonObject);
        } else {
            AppLibNetControl_ReplyErrorCode(AMBA_TAKE_PHOTO, ERROR_NETCTRL_UNKNOWN_ERROR);
        }
    } else if (UserSetting->PhotoPref.PhotoCapMode == PHOTO_CAP_MODE_PES) {
        AppLibNetControl_ReplyErrorCode(AMBA_CONTINUE_CAPTURE_STOP, 0);
    } else {
        return -1;
    }

    return ReturnValue;
}
#endif
/**
 *  @brief The functions of recorder application
 *
 *  The functions of recorder application
 *
 *  @param[in] funcId Function id
 *  @param[in] param1 First parameter
 *  @param[in] param2 Second parameter
 *
 *  @return >=0 success, <0 failure
 */
int rec_cam_func(UINT32 funcId, UINT32 param1, UINT32 param2)
{
    int ReturnValue = 0;

    switch (funcId) {
    case REC_CAM_INIT:
        ReturnValue = rec_cam_init();
        break;
    case REC_CAM_START:
        ReturnValue = rec_cam_start();
        break;
    case REC_CAM_STOP:
        ReturnValue = rec_cam_stop();
        break;
    case REC_CAM_LIVEVIEW_STATE:
        ReturnValue = rec_cam_do_liveview_state();
        break;
    case REC_CAM_LIVEVIEW_POST_ACTION:
        ReturnValue = rec_cam_liveview_post_action();
        break;
    case REC_CAM_SELFTIMER_START:
        ReturnValue = rec_cam_selftimer( 1, param1, param2);
        break;
    case REC_CAM_SELFTIMER_STOP:
        ReturnValue = rec_cam_selftimer( 0, param1, param2);
        break;
    case REC_CAM_CAPTURE:
        ReturnValue = rec_cam_capture();
        break;
    case REC_CAM_CAPTURE_PIV:
        ReturnValue = rec_cam_capture_piv();
        break;
    case REC_CAM_CAPTURE_COMPLETE:
        ReturnValue = rec_cam_capture_complete();
        break;
    case REC_CAM_CAPTURE_BG_PROCESS_DONE:
        ReturnValue = rec_cam_capture_bg_process_done();
        break;
    case REC_CAM_RECORD_START:
        ReturnValue = rec_cam_record_start();
        break;
    case REC_CAM_RECORD_PAUSE:
        ReturnValue = rec_cam_record_pause();
        break;
    case REC_CAM_RECORD_RESUME:
        ReturnValue = rec_cam_record_resume();
        break;
    case REC_CAM_RECORD_STOP:
        ReturnValue = rec_cam_record_stop();
        break;
    case REC_CAM_RECORD_AUTO_START:
        ReturnValue = rec_cam_record_auto_start();
        break;
    case REC_CAM_RECORD_LED_START:
        ReturnValue = rec_cam_record_led(param1);
        break;
    case REC_CAM_MUXER_OPEN:
        ReturnValue = rec_cam_muxer_open(param1);
        break;
    case REC_CAM_MUXER_END:
        ReturnValue = rec_cam_muxer_end();
        break;
    case REC_CAM_MUXER_END_EVENTRECORD:
        ReturnValue = rec_cam_muxer_end_eventrecord();
        break;
#ifdef CONFIG_APP_ARD
    case REC_CAM_MUXER_OPEN_EVENTRECORD:
        ReturnValue = rec_cam_muxer_open_eventrecord();
        break;
#endif
    case REC_CAM_MUXER_REACH_LIMIT:
        ReturnValue = rec_cam_muxer_reach_limit(param1);
        break;
    case REC_CAM_MUXER_REACH_LIMIT_EVENTRECORD:
        ReturnValue = rec_cam_muxer_reach_limit_eventrecord(param1);
        break;
#ifdef CONFIG_APP_ARD
    case REC_CAM_MUXER_IO_ERROR_EVENTRECORD:
        UserSetting->VideoPref.UnsavingEvent = 1;
#ifndef CONFIG_APP_EVENT_OVERLAP
        AppLibFormatMuxMp4_Close_EventRecord();
#endif
        rec_cam.Func(REC_CAM_MUXER_END, 0, 0);
        break;
#endif
    case REC_CAM_EVENTRECORD_START:
        ReturnValue = rec_cam_eventrecord_start();
        break;
    case REC_CAM_MUXER_STREAM_ERROR:
#ifdef CONFIG_APP_ARD
    case REC_CAM_MUXER_GENERAL_ERROR:
        if(REC_CAM_MUXER_GENERAL_ERROR == funcId) {
            AmbaPrint("$$REC_CAM_MUXER_GENERAL_ERROR");
        }
#endif
        ReturnValue = rec_cam_muxer_stream_error();
        break;
    case REC_CAM_ERROR_MEMORY_RUNOUT:
        ReturnValue = rec_cam_error_memory_runout();
        break;
    case REC_CAM_ERROR_STORAGE_RUNOUT:
        ReturnValue = rec_cam_error_storage_runout();
        break;
    case REC_CAM_ERROR_STORAGE_IO:
        ReturnValue = rec_cam_error_storage_io();
        break;
    case REC_CAM_ERROR_LOOP_ENC_ERR:
        ReturnValue = rec_cam_error_loop_enc_err(param1);
        break;
    case REC_CAM_LOOP_ENC_DONE:
        ReturnValue = rec_cam_loop_enc_done();
        break;
    case REC_CAM_SWITCH_APP:
        ReturnValue = rec_cam_switch_app();
        break;
    case REC_CAM_SET_VIDEO_RES:
        ReturnValue = rec_cam_set_videoRes(param1, 1);
        break;
    case REC_CAM_SET_VIDEO_QUALITY:
        ReturnValue = rec_cam_set_video_quality(param1, 1);
        break;
    case REC_CAM_SET_VIDEO_PRE_RECORD:
#ifdef CONFIG_APP_ARD
        ReturnValue = rec_cam_set_video_prerecord(param1);
#endif
        break;
    case REC_CAM_SET_VIDEO_TIME_LAPSE:
        ReturnValue = rec_cam_set_time_lapse(param1, 1);
        break;
    case REC_CAM_SET_VIDEO_DUAL_STREAMS:
        break;
    case REC_CAM_SET_VIDEO_RECORD_MODE:
        break;
    case REC_CAM_SET_PHOTO_SIZE:
        ReturnValue = rec_cam_set_photo_size(param1, param2);
        break;
    case REC_CAM_SET_PHOTO_QUALITY:
        ReturnValue = rec_cam_set_photo_quality(param1, 1);
        break;
    case REC_CAM_SET_SELFTIMER:
        ReturnValue = rec_cam_set_selftimer(param1, 1);
        break;
    case REC_CAM_SET_ENC_MODE:
        ReturnValue = rec_cam_set_enc_mode(param1);
        break;
    case REC_CAM_SET_DMF_MODE:
        ReturnValue = rec_cam_set_dmf_mode(param1);
        break;
    case REC_CAM_CARD_REMOVED:
        ReturnValue = rec_cam_card_removed();
        break;
    case REC_CAM_CARD_ERROR_REMOVED:
        ReturnValue = rec_cam_card_error_removed();
        break;
    case REC_CAM_CARD_NEW_INSERT:
        ReturnValue = rec_cam_card_new_insert();
        break;
    case REC_CAM_CARD_STORAGE_IDLE:
        ReturnValue = rec_cam_card_storage_idle();
        break;
    case REC_CAM_CARD_STORAGE_BUSY:
        ReturnValue = rec_cam_card_storage_busy();
        break;
    case REC_CAM_CARD_CHECK_STATUS:
        /**param1 0: video if space not enough will do loop encode 1: photo if space not enough do nothing*/
        #ifdef CONFIG_APP_ARD
        ReturnValue = rec_cam_card_check_status(param1, param2);
        #else
        ReturnValue = rec_cam_card_check_status(param1);
        #endif
        break;
    case REC_CAM_CARD_FULL_HANDLE:
        ReturnValue = rec_cam_card_full_handle();
        break;
    case REC_CAM_CARD_FULL_HANDLE_EVENT:
        ReturnValue = rec_cam_card_full_handle_event();
        break;
    case REC_CAM_FILE_ID_UPDATE:
        ReturnValue = rec_cam_file_id_update(param1);
        break;
    case REC_CAM_WIDGET_CLOSED:
        ReturnValue = rec_cam_widget_closed();
        break;
    case REC_CAM_SET_SYSTEM_TYPE:
        ReturnValue = rec_cam_set_system_type();
        break;
    case REC_CAM_UPDATE_FCHAN_VOUT:
        ReturnValue = rec_cam_update_fchan_vout(param1);
        break;
    case REC_CAM_UPDATE_DCHAN_VOUT:
      ReturnValue = rec_cam_update_dchan_vout(param1);
        break;
    case REC_CAM_CHANGE_DISPLAY:
        ReturnValue = rec_cam_change_display();
        break;
    case REC_CAM_CHANGE_OSD:
        ReturnValue = rec_cam_change_osd();
        break;
    case REC_CAM_USB_CONNECT:
        ReturnValue = rec_cam_usb_connect();
        break;
    case REC_CAM_GUI_INIT_SHOW:
        ReturnValue = rec_cam_start_show_gui();
        break;
    case REC_CAM_UPDATE_BAT_POWER_STATUS:
        ReturnValue = rec_cam_update_bat_power_status(param1);
        break;
    case REC_CAM_WARNING_MSG_SHOW_START:
        ReturnValue = rec_cam_warning_msg_show( 1, param1, param2);
        break;
    case REC_CAM_WARNING_MSG_SHOW_STOP:
        ReturnValue = rec_cam_warning_msg_show( 0, param1, param2);
        break;
    case REC_CAM_CARD_FMT_NONOPTIMUM:
        ReturnValue = rec_cam_card_fmt_nonoptimum();
        break;
    case REC_CAM_ADAS_EVENT:
        ReturnValue = rec_cam_adas_event_handler(param1);
        break;
    case REC_CAM_ADAS_FUNCTION_INIT:
        ReturnValue = rec_cam_adas_function_init();
        break;
    case REC_CAM_ADAS_UPDATE_PARAM:
        ReturnValue = rec_cam_adas_update_param();
        break;
#ifdef CONFIG_APP_ARD
    case REC_CAM_MOTION_DETECT_START:
        ReturnValue = rec_cam_motion_detect_start();
        break;
     case REC_CAM_MOTION_DETECT_STOP:
        ReturnValue = rec_cam_motion_detect_stop();
        break;
    case REC_CAM_MOTION_DETECT_RECORD:
        ReturnValue = rec_cam_motion_detect_record_handler();
        break;
    case REC_CAM_MIC_SWITCH:
#ifdef CONFIG_APP_ARD
        if (app_status.CurrEncMode == APP_VIDEO_ENC_MODE) {
            UserSetting->VideoPref.MicMute = (UserSetting->VideoPref.MicMute)? 0: 1;
            rec_cam.Gui(GUI_MIC_ICON_SHOW, UserSetting->VideoPref.MicMute, 0);
            rec_cam.Gui(GUI_FLUSH, 0, 0);
        }
#else
         UserSetting->VideoPref.MicMute = (UserSetting->VideoPref.MicMute)? 0: 1;
         rec_cam.Gui(GUI_MIC_ICON_SHOW, UserSetting->VideoPref.MicMute, 0);
         rec_cam.Gui(GUI_FLUSH, 0, 0);
#endif
         AppLibAudioEnc_Mute(UserSetting->VideoPref.MicMute);
         break;
	case REC_CAM_ADAS_CEL:
		if(adas_cel_sta)
			{
				rec_cam.Gui(GUI_ADAS_CEL_SHOW, 0, 0);
                if(UserSetting->VideoPref.Adas_cel_set<76||UserSetting->VideoPref.Adas_cel_set>94)
                {
                     UserSetting->VideoPref.Adas_cel_set=90;
                }  

				adas_cel_set = (UserSetting->VideoPref.Adas_cel_set-76)/2;
				//rec_cam.Gui(GUI_ADAS_CEL_SET_SHOW, adas_cel_set, 0);
				//if((adas_cel_set<=0||(adas_cel_set>10))
				//adas_cel_set = 0;
				AmbaPrintColor(CYAN,"####REC_CAM_ADAS_CEL!! adas_cel_sta = 1!adas_cel_set===%d####\n",adas_cel_set);
				adas_cel_sta = 0;
			}
		/*else
			{
				
				//if(adas_cel_set==0)
				//rec_cam.Gui(GUI_ADAS_CEL_HIDE, 0, 0);

				//if(adas_cel_set >= 1)
				rec_cam.Gui(GUI_ADAS_CEL_SET_HIDE, adas_cel_set, 0);
				
				adas_cel_set++;	
				if(adas_cel_set >= 10)
				adas_cel_set = 9;
				rec_cam.Gui(GUI_ADAS_CEL_SET_SHOW, adas_cel_set, 0);
				AmbaPrintColor(CYAN,"####REC_CAM_ADAS_CEL!!!adas_cel_set===%d####\n",adas_cel_set);
				//adas_cel_sta = 1;
		      }*/

        rec_cam.Gui(GUI_FLUSH, 0, 0);

		break;
    case REC_CAM_VIDEO_SET_GSENSOR_SENSITIVITY:
         rec_cam_video_set_gsensor_sensitivity(param1);
         break;
    case REC_CAM_SET_SPLIT_TIME:
        /* if (app_status.CurrEncMode == APP_VIDEO_ENC_MODE) {
         rec_cam.Gui(GUI_SPLIT_TIME_ICON_SHOW,0,0);
         rec_cam.Gui(GUI_FLUSH, 0, 0);
         }*/
          if(UserSetting->VideoPref.video_split_rec_time == VIDEO_SPLIT_REC_1_MIN ) {
                AppLibVideoEnc_SetSplit(VIDEO_SPLIT_TIME_60_SECONDS);
            } 
            else if (UserSetting->VideoPref.video_split_rec_time == VIDEO_SPLIT_REC_3_MIN ) {
                AppLibVideoEnc_SetSplit(VIDEO_SPLIT_TIME_180_SECONDS);
            } else if (UserSetting->VideoPref.video_split_rec_time == VIDEO_SPLIT_REC_5_MIN ) {
                AppLibVideoEnc_SetSplit(VIDEO_SPLIT_TIME_300_SECONDS);
            } else if (UserSetting->VideoPref.video_split_rec_time == VIDEO_SPLIT_REC_OFF ) {
                AppLibVideoEnc_SetSplit(VIDEO_SPLIT_OFF);
            }
         break;
    case REC_CAR_VIDEO_EVENT_FILE_NUM_UPDATE:
    {
        int number;
#ifdef CONFIG_APP_ARD
        number = AppLibStorageDmf_GetFileObjAmount(APPLIB_DCF_MEDIA_VIDEO, EVENTRECORD_HDLR);
#else
        number = AppLibStorageDmf_GetFileAmount(APPLIB_DCF_MEDIA_VIDEO, EVENTRECORD_HDLR);
#endif
        if(number >= MAX_EVENT_FILE ){
            rec_cam.Func(REC_CAM_WARNING_MSG_SHOW_START, GUI_WARNING_EVENT_FULL, 0);
            rec_cam.Gui(GUI_EVENT_NUM_SHOW, number, 0);
            rec_cam.Gui(GUI_FLUSH, 0, 0);
        } else {
#ifndef CONFIG_APP_ARD
            rec_cam.Func(REC_CAM_WARNING_MSG_SHOW_STOP, 0, 0);
#endif
        }
        break;
    }

    case REC_CAR_PHOTO_FILE_NUM_UPDATE:
    {
        int PhotoAmount;
        PhotoAmount = AppLibStorageDmf_GetFileAmount(APPLIB_DCF_MEDIA_IMAGE,DCIM_HDLR);

        if (PhotoAmount >= MAX_PHOTO_COUNT) {
            rec_cam.Func(REC_CAM_WARNING_MSG_SHOW_START, GUI_WARNING_PHOTO_LIMIT, 0);
            AmbaPrintColor(CYAN,"[rec_cam] REC_CAR_PHOTO_FILE_NUM_UPDATE: Photo count reach limit, can not do capture any more.");
        } else {
            rec_cam.Func(REC_CAM_WARNING_MSG_SHOW_STOP, 0, 0);
        }
#ifdef CONFIG_APP_ARD
        if(-1 == PhotoAmount) {
            PhotoAmount = 0;
        }
        if (AppLibCard_CheckStatus(CARD_CHECK_WRITE) == CARD_STATUS_NO_CARD) {
            break;
        }
#endif
        rec_cam.Gui(GUI_PHOTO_NUM_SHOW, PhotoAmount, 0);
        rec_cam.Gui(GUI_FLUSH, 0, 0);
        break;
    }
#endif

#if defined(CONFIG_APP_AMBA_LINK)
    case REC_CAM_BOSS_BOOTED:
        ReturnValue = rec_cam_boos_booted();
        break;
    case REC_CAM_VF_START:
        ReturnValue = rec_cam_vf_start();
        break;
    case REC_CAM_VF_STOP:
        ReturnValue = rec_cam_vf_stop();
        break;
    case REC_CAM_CAPTURE_ON_VF:
        ReturnValue = rec_cam_capture_on_vf();
        break;
    case REC_CAM_VF_SWITCH_TO_RECORD:
        ReturnValue = rec_cam_vf_switch_to_record();
        break;
    case REC_CAM_NETFIFO_EVENT_START:
        ReturnValue = rec_cam_netfifo_event_start();
        break;
    case REC_CAM_NETFIFO_EVENT_STOP:
        ReturnValue = rec_cam_netfifo_event_stop();
        break;
    case REC_CAM_NETCTRL_CAPTURE_DONE:
        ReturnValue = rec_cam_netctrl_capture_done();
        break;
#endif
    case REC_CAM_CALIBRATION_ICN:
        ReturnValue = rec_cam_calibration_icn(param1);
        break;
    default:
        break;
    }

    return ReturnValue;
}

#ifdef CONFIG_APP_ARD
void AppUtil_Gps_Status_Monitor(void)
{
    AppLibComSvcTimer_Register(TIMER_1HZ, rec_cam_gps_status_timer_handler);
}

void AppUtil_Show_Data_Timer_Monitor(void)
{
    AppLibComSvcTimer_Register(TIMER_1HZ, rec_cam_show_data_timer_handler);
}

#endif

