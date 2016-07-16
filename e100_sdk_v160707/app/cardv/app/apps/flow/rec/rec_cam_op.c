/**
  * @file src/app/apps/flow/rec/connectedcam/rec_cam_op.c
  *
  * Operations of Sport Recorder (sensor) application
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

#include <system/ApplibSys_Lcd.h>
#include <apps/flow/rec/rec_cam.h>
#include <system/app_util.h>
#include <apps/gui/resource/gui_settle.h>

#if defined(CONFIG_APP_AMBA_LINK)
#include <system/app_pref.h>
#endif

#ifdef CONFIG_APP_ARD
#include <system/app_pref.h>
#include <apps/flow/widget/menu/menu.h>
#endif




/*************************************************************************
 * App Function Declarations (static)
 ************************************************************************/
static int rec_cam_button_record(void);
static int rec_cam_button_focus(void);
static int rec_cam_button_focus_clr(void);
static int rec_cam_button_shutter(void);
static int rec_cam_button_shutter_clr(void);
static int rec_cam_button_zoom_in(void);
static int rec_cam_button_zoom_in_clr(void);
static int rec_cam_button_zoom_out(void);
static int rec_cam_button_zoom_out_clr(void);
static int rec_cam_button_up(void);
static int rec_cam_button_down(void);
static int rec_cam_button_left(void);
static int rec_cam_button_right(void);
static int rec_cam_button_set(void);
static int rec_cam_button_menu(void);
static int rec_cam_button_mode(void);
static int rec_cam_button_del(void);
static int rec_cam_button_power(void);
static int rec_cam_button_f4(void);



REC_CAM_OP_s rec_cam_op = {
    rec_cam_button_record,
    rec_cam_button_focus,
    rec_cam_button_focus_clr,
    rec_cam_button_shutter,
    rec_cam_button_shutter_clr,
    rec_cam_button_zoom_in,
    rec_cam_button_zoom_in_clr,
    rec_cam_button_zoom_out,
    rec_cam_button_zoom_out_clr,
    rec_cam_button_up,
    rec_cam_button_down,
    rec_cam_button_left,
    rec_cam_button_right,
    rec_cam_button_set,
    rec_cam_button_menu,
    rec_cam_button_mode,
    rec_cam_button_del,
    rec_cam_button_power,
    rec_cam_button_f4,
};

/**
 *  @brief The operation of Record button.
 *
 *  The operation of Record button.
 *
 *  @return >=0 success, <0 failure
 */
static int rec_cam_button_record(void)
{
int ReturnValue = 0;

	if(adas_cel_sta == 0)
	{
		rec_cam.Gui(GUI_ADAS_CEL_HIDE, 0, 0);
		adas_cel_sta = 1;
		rec_cam.Gui(GUI_FLUSH, 0, 0);
        
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
	}
	else
	{
	    /* Close the menu or dialog. */
        if (AppWidget_GetCur() == WIDGET_MENU) 
        {
            AppLibGraph_Hide(GRAPH_CH_DUAL, GOBJ_SETTING_MENU_BG);
        }
	    AppWidget_Off(WIDGET_ALL, 0);
	    if (rec_cam.RecCapState == REC_CAP_STATE_PREVIEW) {
	        /* Check the card's status. */
#ifdef CONFIG_APP_ARD
	        ReturnValue = rec_cam.Func(REC_CAM_CARD_CHECK_STATUS, 0, REC_CAP_PREPARE_TO_REPLAY_MANUAL_RECORD);
#else
	        ReturnValue = rec_cam.Func(REC_CAM_CARD_CHECK_STATUS, 0, 0);
#endif
	        if (ReturnValue == 0) {
          
#ifdef CONFIG_APP_ARD
	            if(MOTION_DETECT_ON == UserSetting->MotionDetectPref.MotionDetect){
	                if (!APP_CHECKFLAGS(app_rec_cam.Flags, REC_CAM_FLAGS_MUXER_BUSY)){
	                    //rec_cam.Func(REC_CAM_MOTION_DETECT_STOP, 0, 0);
	                }
	            }
#endif
                   else
                   {
	                 /* To record the clip if the card is ready. */
	                 rec_cam.Func(REC_CAM_RECORD_START, 0, 0);
                   }
	        }
	    } else if (rec_cam.RecCapState == REC_CAP_STATE_RECORD) {

	        if (APP_CHECKFLAGS(app_rec_cam.Flags, REC_CAM_FLAGS_MUXER_BUSY)) {
	            /* Stop recording. */
	            rec_cam.Func(REC_CAM_RECORD_STOP, 0, 0);
#ifdef CONFIG_APP_ARD
	            if(MOTION_DETECT_ON == UserSetting->MotionDetectPref.MotionDetect)
	            rec_cam.Func(REC_CAM_MOTION_DETECT_STOP, 0, 0);
#endif
	        } else {
	            AmbaPrintColor(YELLOW, "[rec_cam] <button_record> Record not actually start, block record stop");
	        }
	    }
	    #if defined (CONFIG_APP_AMBA_LINK)
	    else if (rec_cam.RecCapState == REC_CAP_STATE_VF) {
	        AmbaPrintColor(YELLOW, "[rec_cam] <button_record> VF -> REC");
	        rec_cam.Func(REC_CAM_VF_SWITCH_TO_RECORD, 0, 0);
	      }
	    #endif
   }

return ReturnValue;
}

/**
 *  @brief The operation of Focus button.
 *
 *  The operation of Focus button.
 *
 *  @return >=0 success, <0 failure
 */
static int rec_cam_button_focus(void)
{
    int ReturnValue = 0;
#if 0    
    if (app_rec_cam.Child != 0) {
        AppUtil_SwitchApp(APP_REC_CAM);    // shrink from Child apps
    }
#endif    
    return ReturnValue;
}

/**
 *  @brief The operation of Focus button release.
 *
 *  The operation of Focus button release.
 *
 *  @return >=0 success, <0 failure
 */
static int rec_cam_button_focus_clr(void)
{
    int ReturnValue = 0;

    return ReturnValue;
}

/**
 *  @brief The operation of Shutter button.
 *
 *  The operation of Shutter button.
 *
 *  @return >=0 success, <0 failure
 */
static int rec_cam_button_shutter(void)
{
    int ReturnValue = 0;
    #ifndef CONFIG_ECL_GUI
    int PhotoAmount = 0;
    APP_ADDFLAGS(app_rec_cam.Flags, REC_CAM_FLAGS_SHUTTER_PRESSED);
    if (app_rec_cam.Child != 0) {
        AppUtil_SwitchApp(APP_REC_CAM);    // shrink from Child apps
    }
    /* Close the menu or dialog. */
    AppWidget_Off(WIDGET_ALL, 0);
    /**PIV can not use GetCaptureNum function*/
    PhotoAmount = AppLibStorageDmf_GetFileAmount(APPLIB_DCF_MEDIA_IMAGE,DCIM_HDLR) + rec_cam.MuxerNum;
    AmbaPrintColor(5,"[%s] Photo amount before capture %d",__func__,PhotoAmount);
    if (rec_cam.RecCapState == REC_CAP_STATE_PREVIEW) {
        /* Check the card's status. */
        ReturnValue = rec_cam.Func(REC_CAM_CARD_CHECK_STATUS, 1, 0);
        if (ReturnValue == 0) {
            /* Check the photo count. */
            PhotoAmount = PhotoAmount + AppLibStillEnc_GetCaptureNum();
            if (PhotoAmount <= MAX_PHOTO_COUNT) {
            /* Capture the photo if the card is ready. */
            ReturnValue = rec_cam.Func(REC_CAM_CAPTURE, 0, 0);
            } else {
                ReturnValue = -1;
#ifdef CONFIG_APP_ARD
                rec_cam.Gui(GUI_PHOTO_NUM_SHOW, MAX_PHOTO_COUNT, 0);
#endif
                rec_cam.Func(REC_CAM_WARNING_MSG_SHOW_START, GUI_WARNING_PHOTO_LIMIT, 0);
                AmbaPrintColor(CYAN,"[rec_cam] <button_shutter> Photo count reach limit, can not do capture");
            }
        }
    } else if (rec_cam.RecCapState == REC_CAP_STATE_RECORD) {
        if (AppLibVideoEnc_GetSensorVideoRes() == SENSOR_VIDEO_RES_TRUE_HDR_1080P_HALF ||
            AppLibVideoEnc_GetSensorVideoRes() == SENSOR_VIDEO_RES_WQHD_HALF_HDR) {
            AmbaPrintColor(CYAN,"[rec_cam] <button_shutter> HDR not support PIV. Block shutter button");
            return -1;
        }
#ifdef CONFIG_APP_ARD
        if(!APP_CHECKFLAGS(app_rec_cam.Flags, REC_CAM_FLAGS_APP_RECEIVED_MUX_START)){
            return -1;
        }
#endif

        /* Check the photo count. */
        PhotoAmount = PhotoAmount + 1;
        if (PhotoAmount <= MAX_PHOTO_COUNT) {
           rec_cam.Func(REC_CAM_CAPTURE_PIV, 0, 0);
        } else {
            ReturnValue = -1;
#ifdef CONFIG_APP_ARD
            rec_cam.Gui(GUI_PHOTO_NUM_SHOW, MAX_PHOTO_COUNT, 0);
#endif
            rec_cam.Func(REC_CAM_WARNING_MSG_SHOW_START, GUI_WARNING_PHOTO_LIMIT, 0);
            AmbaPrintColor(CYAN,"[rec_cam] <button_shutter> Photo count reach limit, can not do capture");
        }
    }
#if defined(CONFIG_APP_AMBA_LINK)
    else if (rec_cam.RecCapState == REC_CAP_STATE_VF) {
        /* Check the card's status. */
        ReturnValue = rec_cam.Func(REC_CAM_CARD_CHECK_STATUS, 1, 0);
        if (ReturnValue == 0) {
            /* Check the photo count. */
            PhotoAmount = PhotoAmount + AppLibStillEnc_GetCaptureNum();
            if (PhotoAmount <= MAX_PHOTO_COUNT) {
            AmbaPrintColor(CYAN,"[rec_cam] <button_shutter> REC_CAM_CAPTURE_ON_VF");
            ReturnValue = rec_cam.Func(REC_CAM_CAPTURE_ON_VF, 0, 0);
            } else {
                ReturnValue = -1;
#ifdef CONFIG_APP_ARD
                rec_cam.Gui(GUI_PHOTO_NUM_SHOW, MAX_PHOTO_COUNT, 0);
#endif
                rec_cam.Func(REC_CAM_WARNING_MSG_SHOW_START, GUI_WARNING_PHOTO_LIMIT, 0);
                AmbaPrintColor(CYAN,"[rec_cam] <button_shutter> Photo count reach limit, can not do capture");
            }
        }
    }
#endif
    else {
        ReturnValue = -1;
    }

    if (ReturnValue == 0) {
        /* Flag 'REC_CAM_FLAGS_BLOCK_MENU' is used to block menu while capturing.
               Set this flag when cpature starts. And this flag should be cleared when capture is done,
               including muxer finished. */
        APP_ADDFLAGS(app_rec_cam.Flags, REC_CAM_FLAGS_BLOCK_MENU);
    }
  #endif

    return ReturnValue;
}

/**
 *  @brief The operation of Shutter button release.
 *
 *  The operation of Shutter button release.
 *
 *  @return >=0 success, <0 failure
 */

static int rec_cam_button_shutter_clr(void)
{
    int ReturnValue = 0;
    if((app_check_scr_status() == 0)&&(app_check_menu_scr_status()==1))
    {
        AppLibSysLcd_SetBacklight_Directly(LCD_CH_DCHAN, 0);
        app_set_scr_status(1);
    }
   
    APP_REMOVEFLAGS(app_rec_cam.Flags, REC_CAM_FLAGS_SHUTTER_PRESSED);

    return ReturnValue;
}

/**
 *  @brief The operation of Zoom_in button.
 *
 *  The operation of Zoom_in button.
 *
 *  @return >=0 success, <0 failure
 */
static int rec_cam_button_zoom_in(void)
{
    int ReturnValue = 0;

    return ReturnValue;
}

/**
 *  @brief The operation of Zoom_in button release.
 *
 *  The operation of Zoom_in button release.
 *
 *  @return >=0 success, <0 failure
 */
static int rec_cam_button_zoom_in_clr(void)
{
    int ReturnValue = 0;

    return ReturnValue;
}

/**
 *  @brief The operation of Zoom_out button.
 *
 *  The operation of Zoom_out button.
 *
 *  @return >=0 success, <0 failure
 */
static int rec_cam_button_zoom_out(void)
{
    int ReturnValue = 0;
    return ReturnValue;
}

/**
 *  @brief The operation of Zoom_out button release.
 *
 *  The operation of Zoom_out button release.
 *
 *  @return >=0 success, <0 failure
 */
static int rec_cam_button_zoom_out_clr(void)
{
    int ReturnValue = 0;

    return ReturnValue;
}

/**
 *  @brief The operation of Up button.
 *
 *  The operation of Up button.
 *
 *  @return >=0 success, <0 failure
 */
static int rec_cam_button_up(void)
{
    int ReturnValue = 0;

    return ReturnValue;
}

/**
 *  @brief The operation of Down button.
 *
 *  The operation of Down button.
 *
 *  @return >=0 success, <0 failure
 */
static int rec_cam_button_down(void)
{
    int ReturnValue = 0;

    return ReturnValue;
}

/**
 *  @brief The operation of Left button.
 *
 *  Left button operation
 *
 *  @return >=0 success, <0 failure
 */
static int rec_cam_button_left(void)
{
    int ReturnValue = 0;
	/*if(adas_cel_sta == 0)
		{
			//adas_cel_set = (UserSetting->VideoPref.Adas_cel_set-45)/4
			//if(adas_cel_set==0)
			//rec_cam.Gui(GUI_ADAS_CEL_HIDE, 0, 0);

			//if(adas_cel_set >= 1)
			rec_cam.Gui(GUI_ADAS_CEL_SET_HIDE, adas_cel_set, 0);

			adas_cel_set--;	
			if(adas_cel_set <= 0)
			adas_cel_set = 0;
			
			
			rec_cam.Gui(GUI_ADAS_CEL_SET_SHOW, adas_cel_set, 0);
			rec_cam.Gui(GUI_FLUSH, 0, 0);
			//AmbaPrintColor(CYAN,"####rec_cam_button_left!!!adas_cel_set===%d####\n",adas_cel_set);
			//AmbaPrintColor(CYAN,"####rec_cam_button_left!!!UserSetting->VideoPref.Adas_cel_set===%d####\n",UserSetting->VideoPref.Adas_cel_set);
		}
	//else
		//{
#ifdef CONFIG_APP_ARD
    rec_cam.Func(REC_CAM_MIC_SWITCH, 0, 0);
#endif
		//}*/
   
#ifdef CONFIG_APP_ARD
    rec_cam.Func(REC_CAM_MIC_SWITCH, 0, 0);
#endif
   
    return ReturnValue;
}

/**
 *  @brief The operation of Right button.
 *
 *  The operation of Right button.
 *
 *  @return >=0 success, <0 failure
 */
static int rec_cam_button_right(void)
{
    int ReturnValue = 0;

	#ifdef CONFIG_ECL_GUI
 
    if (rec_cam.RecCapState == REC_CAP_STATE_RECORD) 
    {
        rec_cam.Func(REC_CAM_EVENTRECORD_START, 0, 0);
    }
    else
    {
        AmbaPrint("[app_rec_cam] Block eventrecord when system is not in recording");
        return 0;
    }

    
    #endif
    
    return ReturnValue;
}

/**
 *  @brief The operation of Set button.
 *
 *  The operation of Set button.
 *
 *  @return >=0 success, <0 failure
 */
static int rec_cam_button_set(void)
{
    int ReturnValue = 0;
    return ReturnValue;
}

#ifdef CONFIG_APP_ARD
static int rec_menu_locked = 0;
int rec_cam_set_menu_locked(int menu_locked) {
    rec_menu_locked = menu_locked;
}
int rec_cam_get_menu_locked(void) {
    return rec_menu_locked;
}
#endif

/**
 *  @brief The operation of Menu button.
 *
 *  The operation of Menu button.
 *
 *  @return >=0 success, <0 failure
 */
static int rec_cam_button_menu(void)
{
    int ReturnValue = 0;
#ifdef CONFIG_APP_ARD
    char Drive = 'A';
    char FactoryModePath[]="I:\\factory_mode";
    AMBA_FS_STAT Fstat;
#endif
    if(adas_cel_sta==1)
    {
        if (APP_CHECKFLAGS(app_rec_cam.Flags, REC_CAM_FLAGS_BLOCK_MENU)) {
        AmbaPrint("%s, %s, %d", __FILE__, __func__, __LINE__);
        DBGMSG("[rec_cam] <button_menu> Block menu.");
        return -1;
       }
        
        if (!APP_CHECKFLAGS(app_rec_cam.GFlags, APP_AFLAGS_POPUP)) {
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
                #ifdef CONFIG_APP_ARD
                if(MOTION_DETECT_ON == UserSetting->MotionDetectPref.MotionDetect)
                rec_cam.Func(REC_CAM_MOTION_DETECT_STOP, 0, 0);
                #endif
                /** record stop at menu open*/
                if(rec_cam.RecCapState == REC_CAP_STATE_RECORD)		
                {
                     rec_cam.Func(REC_CAM_RECORD_STOP, 0, 0);
                }			 
            }
            rec_cam.Func(REC_CAM_WARNING_MSG_SHOW_STOP, 0, 0);
            APP_ADDFLAGS(app_rec_cam.GFlags, APP_AFLAGS_POPUP);

#ifndef CONFIG_APP_ARD
            Drive = AppLibCard_GetActiveDrive();
            FactoryModePath[0] = Drive;
            AmbaPrint("%s, %s, %d, %s", __FILE__, __func__, __LINE__, FactoryModePath);

            AmbaFS_Stat((const char *)FactoryModePath, &Fstat);
            if (ATTR_DIR == Fstat.Attr) {
                AppWidget_Off(WIDGET_ALL, WIDGET_HIDE_SILENT);
                AppMenu_Reset();
                AppMenu_RegisterTab(MENU_FACTORY);
            }

            if (0 != AppLibDCF_GetOpenedFiles()) {
                AppMenu_LockTab(MENU_ADAS);
                AppMenu_LockTab(MENU_VIDEO);
                AppMenu_LockTab(MENU_SETUP);
                rec_cam_set_menu_locked(1);
            }
#endif
            #ifdef CONFIG_ECL_GUI
            //rec_cam.Gui(GUI_MIC_ICON_HIDE, UserSetting->VideoPref.MicMute, 0);
            //rec_cam.Gui(GUI_CARD_HIDE, 0, 0);
            //rec_cam.Gui(GUI_POWER_STATE_HIDE, 0, 0);
            if(ADAS_ON == UserSetting->VAPref.AdasDetection)
            {
                AppLibVideo_Ecl_ADAS_Disable();
            }
            #endif
            ReturnValue = AppWidget_On(WIDGET_MENU, 0);
        }
    } 
    return ReturnValue;
}

/**
 *  @brief The operation of Mode button.
 *
 *  The operation of Mode button.
 *
 *  @return >=0 success, <0 failure
 */
static int rec_cam_button_mode(void)
{
    int ReturnValue = 0;

    if(adas_cel_sta==0)
    {
         return ReturnValue;
    }
    
#ifdef CONFIG_APP_ARD
    if(rec_cam.RecCapState == REC_CAP_STATE_RECORD) {
        AmbaPrint("Is Recording, key mode no responding");
        rec_cam.Func(REC_CAM_WARNING_MSG_SHOW_START, GUI_WARNING_MODE_BUSY, 0);
    }
    else if (APP_CHECKFLAGS(app_rec_cam.GFlags, APP_AFLAGS_POPUP)) 
    {
        /* Don't Switch mode when menu is on. */
        AmbaPrint("Block switch mode, APP_AFLAGS_POPUP");
        return -1;
    } 
    else if(!APP_CHECKFLAGS(app_rec_cam.Flags, REC_CAM_FLAGS_MUXER_BUSY))
    {
#endif
        /*block change mode at capture on vf ,due to capture on vf will remove busy flag at change video to stil
        so flow will be interrupt if allow switch model*/
        if (!APP_CHECKFLAGS(app_rec_cam.Flags,REC_CAM_FLAGS_CAPTURE_ON_VF)) {
            /* Switch mode. */
            AppUtil_SwitchMode(0);
        } 
        else
        {
            AmbaPrint("[app_rec_cam] Block switch mode at capture on VF");
#ifdef CONFIG_APP_ARD
            rec_cam.Func(REC_CAM_WARNING_MSG_SHOW_START, GUI_WARNING_MODE_BUSY, 0);
#endif
        }
#ifdef CONFIG_APP_ARD
    }
    else
    {
        AmbaPrint("[app_rec_cam] Block switch mode");
        rec_cam.Func(REC_CAM_WARNING_MSG_SHOW_START, GUI_WARNING_MODE_BUSY, 0);
    }
#endif
 

return ReturnValue;
}

/**
 *  @brief The operation of Delete button.
 *
 *  The operation of Delete button.
 *
 *  @return >=0 success, <0 failure
 */
static int rec_cam_button_del(void)
{
    int ReturnValue = 0;
	#ifdef CONFIG_APP_ARD
         rec_cam.Func(REC_CAM_ADAS_CEL, 0, 0);
    #endif
    return ReturnValue;
}

/**
 *  @brief The operation of Power button.
 *
 *  The operation of Power button.
 *
 *  @return >=0 success, <0 failure
 */
static int rec_cam_button_power(void)
{
    int ReturnValue = 0;
#ifdef CONFIG_APP_ARD
    AppWidget_SetPback(WIDGET_ALL);
    rec_cam.Func(REC_CAM_RECORD_STOP, 0, 0);   
    AppUtil_SwitchApp(APP_MISC_LOGO);
#endif
    return ReturnValue;
}

/**
 *  @brief The operation of F4 button.
 *
 *  The operation of F4 button.
 *
 *  @return >=0 success, <0 failure
 */
static int rec_cam_button_f4(void)
{
    int ReturnValue = 0;
#ifndef CONFIG_ECL_GUI
    if (rec_cam.RecCapState == REC_CAP_STATE_RECORD) {
        rec_cam.Func(REC_CAM_EVENTRECORD_START, 0, 0);
    }
    else{
        AmbaPrint("[app_rec_cam] Block eventrecord when system is not in recording");
        return 0;
    }
#endif
    return ReturnValue;
}

