/**
  * @file src/app/apps/flow/rec/sound/rec_sound_func.c
  *
  *  Functions of Sport Recorder (sensor) application
  *
  * History:
  *    2014/11/26 - [QiangSu] created file
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


#include <apps/flow/sound/rec_sound.h>
#include <system/status.h>
#include <system/app_pref.h>
#include <system/app_util.h>
#include <system/ApplibSys_Sensor.h>
#include <storage/ApplibStorage_DmfDateTime.h>
#include <apps/flow/widget/menu/menu.h>
#include <apps/gui/resource/gui_settle.h>
#include <apps/flow/disp_param/disp_param_rec.h>
#include <AmbaROM.h>
#include <AmbaRTC.h>
#include <graphics/stamp/ApplibGraphics_Stamp.h>
#include <apps/flow/widget/menu/menu_video.h>

#if defined(CONFIG_APP_CONNECTED_AMBA_LINK)
#include <AmbaUtility.h>


#define NotifyNetFifoOfAppState(state) \
    do {    \
        if (UserSetting->VideoPref.StreamType == STREAM_TYPE_RTSP) {    \
            AppLibNetFifo_NotifyAppStateChange(state);    \
        }    \
    } while(0)
#endif /* CONFIG_APP_CONNECTED_AMBA_LINK */

static int rec_sound_init(void)
{
    int ReturnValue = 0;

    /** Initialize the audio recorder. */
    AppLibAudioEnc_Init();
    
    rec_sound.RecState = REC_SOUND_STATE_RESET;

    return ReturnValue;
}

static int rec_sound_start(void)
{
    int ReturnValue = 0;

    UserSetting->SystemPref.SystemMode = APP_MODE_ENC;

    /* Init decoder. Decode black screen rather than use background source. */
    AppLibThmBasic_Init();

    /** Set menus */
    AppMenu_Reset();
    AppMenu_RegisterTab(MENU_SETUP);
    //AppMenu_RegisterTab(MENU_IMAGE);
    AppMenu_RegisterTab(MENU_PHOTO);
    AppMenu_RegisterTab(MENU_VIDEO);

    rec_sound.Func(REC_SOUND_CHANGE_DISPLAY, 0, 0);

    AppLibAudioEnc_EncodeSetup();
    AppLibAudioEnc_Mux_Setup();
    
    /** set free space threshold*/
    AppLibCard_SetThreahold(FREESPACE_THRESHOLD); /**<set card check threshold*/
    AppLibMonitorStorage_SetThreshold(FREESPACE_THRESHOLD);/**<set storage monitor threshold*/
    /** inital storage async op task*/
    AppLibStorageAsyncOp_Init();

    /*register loop enc handler*/
    //AppLibLoopEnc_Init();

    /** set split setting to 60 sec.*/
    //AppLibVideoEnc_SetSplit(VIDEO_SPLIT_TIME_60_SECONDS);
    
    AppLibComSvcHcmgr_SendMsg(AMSG_CMD_APP_READY, 0, 0);   //test message

    return ReturnValue;
}

static int rec_sound_stop(void)
{
    int ReturnValue = 0;

    /* Deinit decoder. */
    AppLibThmBasic_Deinit();

    /* Stop the warning message, because the warning could need to be updated. */
    rec_sound.Func(REC_SOUND_WARNING_MSG_SHOW_STOP, 0, 0);

    APP_REMOVEFLAGS(app_rec_sound.Flags, REC_SOUND_FLAGS_MUXER_ERROR);
    AmbaKAL_TaskSleep(50);

    /** Stop the liveview. */
   // AppLibVideoEnc_LiveViewStop();

    /* Close the menu or dialog. */
    AppWidget_Off(WIDGET_ALL, WIDGET_HIDE_SILENT);
    APP_REMOVEFLAGS(app_rec_sound.GFlags, APP_AFLAGS_POPUP);
    AmbaKAL_TaskSleep(50);

    /* Disable the vout. */
    AppLibDisp_DeactivateWindow(DISP_CH_FCHAN, AppLibDisp_GetWindowId(DISP_CH_FCHAN, 0));
    AppLibDisp_DeactivateWindow(DISP_CH_DCHAN, AppLibDisp_GetWindowId(DISP_CH_DCHAN, 0));
    AppLibDisp_ChanStop(DISP_CH_FCHAN);
    AmbaKAL_TaskSleep(50);

    /** Hide GUI */
    rec_sound.Gui(GUI_HIDE_ALL, 0, 0);
    rec_sound.Gui(GUI_FLUSH, 0, 0);
    AmbaKAL_TaskSleep(50);

    return ReturnValue;
}

/**
 *  @brief The timer handler that can show the record time.
 *
 *  The timer handler that can show the record time.
 *
 *  @param[in] eid timer id.
 *
 */
static void rec_sound_rec_timer_handler(int eid)
{
    if (eid == TIMER_UNREGISTER) {
        return;
    }

    //AmbaPrint("[app_rec_sound] Record Time: %d", rec_sound.RecTime);
    if (rec_sound.RecState == REC_SOUND_STATE_RECORD) {
        rec_sound.RecTime++;
        rec_sound.Gui(GUI_REC_TIMER_UPDATE, rec_sound.RecTime, 0);
        rec_sound.Gui(GUI_FLUSH, 0, 0);
    }
}

static int rec_sound_record_start(void)
{
    int ReturnValue = 0;

    if (rec_sound.RecState != REC_SOUND_STATE_RECORD) {     
	    if (APP_CHECKFLAGS(app_rec_sound.Flags, REC_SOUND_FLAGS_MUXER_BUSY))
	        return ReturnValue;

	    DBGMSG("[app_rec_sound] REC_SOUND_RECORD_START");

	    /* Register the timer to show the record time. */
	    rec_sound.RecTime = 0;
	    AppLibComSvcTimer_Register(TIMER_1HZ, rec_sound_rec_timer_handler);

	    /* Start audio encoding. */
	    AppLibAudioEnc_EncodeStart();

	    /* Enable the storage monitor.*/
	    AppLibMonitorStorage_Enable(1);
	    AppLibMonitorStorage_EnableMsg(1);

	    rec_sound.RecState = REC_SOUND_STATE_RECORD;

	    APP_ADDFLAGS(app_rec_sound.GFlags, APP_AFLAGS_BUSY);
	    APP_ADDFLAGS(app_rec_sound.Flags, REC_SOUND_FLAGS_MUXER_BUSY);

	    /* Update the gui. */
	    rec_sound.Gui(GUI_REC_STATE_UPDATE, GUI_REC_START, 0);
	    rec_sound.Gui(GUI_REC_STATE_SHOW, 0, 0);
	    rec_sound.Gui(GUI_REC_TIMER_UPDATE, rec_sound.RecTime, 0);
	    rec_sound.Gui(GUI_REC_TIMER_SHOW, 0, 0);
	    rec_sound.Gui(GUI_FLUSH, 0, 0);

	    //UserSetting->VideoPref.UnsavingData = 1;
	    //AppPref_Save();
    }

    return ReturnValue;
}

static int rec_sound_record_pause(void)
{
    int ReturnValue = 0;

    return ReturnValue;
}

static int rec_sound_record_resume(void)
{
    int ReturnValue = 0;

    return ReturnValue;
}

static int rec_sound_record_stop(void)
{
    int ReturnValue = 0;

    if (rec_sound.RecState == REC_SOUND_STATE_RECORD) {
        DBGMSG("[app_rec_sound] REC_SOUND_RECORD_STOP");
        if (APP_CHECKFLAGS(app_rec_sound.GFlags, APP_AFLAGS_POPUP)) {
            AppWidget_Off(WIDGET_ALL, 0);
        }
        /* Stop the storage monitor.*/
        AppLibMonitorStorage_EnableMsg(0);
        AppLibMonitorStorage_Enable(0);

        /* Stop encoding. */
        AppLibAudioEnc_EncodeStop();
        rec_sound.RecState = REC_SOUND_STATE_RESET;

        /* Don't call NotifyNetFifoOfAppState(AMP_NETFIFO_NOTIFY_STOPENC) here, or exception happened!*/
        //NotifyNetFifoOfAppState(AMP_NETFIFO_NOTIFY_STOPENC);
        if (APP_CHECKFLAGS(app_rec_sound.Flags, REC_SOUND_FLAGS_MUXER_BUSY)){
            APP_REMOVEFLAGS(app_rec_sound.Flags, REC_SOUND_FLAGS_MUXER_BUSY);
        }

        /* Stop the timer that show the gui of recording time because of stopping recording. */
        AppLibComSvcTimer_Unregister(TIMER_1HZ, rec_sound_rec_timer_handler);
        /* Update the gui. */
        rec_sound.Gui(GUI_REC_STATE_HIDE, 0, 0);
        rec_sound.Gui(GUI_REC_TIMER_HIDE, 0, 0);
        rec_sound.Gui(GUI_FLUSH, 0, 0);
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
static int rec_sound_record_auto_start(void)
{
    int ReturnValue = 0;
    if (rec_sound.RecState != REC_SOUND_STATE_RECORD) {
        /** Check the card's status. */
        ReturnValue = rec_sound.Func(REC_SOUND_CARD_CHECK_STATUS, 0, 0);
        if (ReturnValue == 0) {
            /** To record the clip if the card is ready. */
            rec_sound.Func(REC_SOUND_RECORD_START, 0, 0);
        }
    }
    return ReturnValue;
}

static int rec_sound_muxer_end(void)
{
    int ReturnValue = 0;
    APP_REMOVEFLAGS(app_rec_sound.Flags, REC_SOUND_FLAGS_MUXER_BUSY);
    APP_REMOVEFLAGS(app_rec_sound.GFlags, APP_AFLAGS_BUSY);
    return ReturnValue;
}

static int rec_sound_muxer_reach_limit(int param1)
{
    int ReturnValue = 0;

    if (param1) {
        /* Reach the limitation of file, but the setting of split file is off. Stop recording. */
        if (rec_sound.RecState == REC_SOUND_STATE_RECORD) {
            /* Stop recording if the audio and video data buffer is full. */
            rec_sound.Func(REC_SOUND_RECORD_STOP, 0, 0);
        }
    } else {
    }

    return ReturnValue;
}

static int rec_sound_error_memory_runout(void)
{
    int ReturnValue = 0;
    if (rec_sound.RecState == REC_SOUND_STATE_RECORD) {

        /* Stop/Pause recording if the audio and video data buffer is full. */
#if defined(REC_SOUND_MEM_RUNOUT_PAUSE)
        AmbaPrintColor(GREEN,"[app_rec_sound] REC_SOUND_MEM_RUNOUT_PAUSE");
        rec_sound.Func(REC_SOUND_RECORD_PAUSE, 0, 0);
#else
        APP_ADDFLAGS(app_rec_sound.Flags, REC_SOUND_FLAGS_MEM_RUNOUT);
        rec_sound.Func(REC_SOUND_RECORD_STOP, 0, 0);
#endif
    }
    return ReturnValue;
}

static int rec_sound_error_storage_runout(void)
{
    int ReturnValue = 0;

    if (rec_sound.RecState == REC_SOUND_STATE_RECORD) {
        /**call card full handle to do loop enc*/
        rec_sound.Func(REC_SOUND_CARD_FULL_HANDLE, 0, 0);
    }
    return ReturnValue;
}


static int rec_sound_error_storage_io(void)
{
    int ReturnValue = 0;

    if (rec_sound.RecState == REC_SOUND_STATE_RECORD) {
        /* Stop recording if the audio and video data buffer is full. */
        rec_sound.Func(REC_SOUND_RECORD_STOP, 0, 0);
    }

    return ReturnValue;
}

static int rec_sound_error_loop_enc_err(int err_type)
{
    int ReturnValue = 0;
    /**start send storage runout msg after loop enc function*/
    AppLibMonitorStorage_EnableMsg(1);
    if (err_type) {
        AmbaPrint("[app_rec_sound] Loop Enc Delete File Fail ");
    } else {
        if (rec_sound.RecState == REC_SOUND_STATE_RECORD ) {
            rec_sound.Func(REC_SOUND_RECORD_STOP, 0, 0);
            AmbaPrint("[app_rec_sound] Loop Enc Search First File Fail Stop Record ");
        } else {
            AmbaPrint("[app_rec_sound] Loop Enc Search First File Fail");
        }
    }
    return ReturnValue;
}

static int rec_sound_loop_enc_done(void)
{
    int ReturnValue = 0;
    AmbaPrintColor(GREEN,"[app_rec_sound] Loop Enc Done");
    /**start send storage runout msg after loop enc function*/
    AppLibMonitorStorage_EnableMsg(1);
    if (rec_sound.RecState != REC_SOUND_STATE_RECORD ) {
        rec_sound.Func(REC_SOUND_RECORD_AUTO_START, 0, 0);
    }
    return ReturnValue;
}

static int rec_sound_switch_app(void)
{
    int ReturnValue = 0;

    if (rec_sound.RecState == REC_SOUND_STATE_RECORD) {
        ReturnValue = rec_sound.Func(REC_SOUND_RECORD_STOP, 0, 0);
    }

    return ReturnValue;
}

static int rec_sound_card_removed(void)
{
    int ReturnValue = 0;

    if (rec_sound.RecState == REC_SOUND_STATE_RECORD) {
        /* Stop recording when the card be removed during recording. */
        rec_sound.Func(REC_SOUND_RECORD_STOP, 0, 0);
    }

    /* Stop the warning message, because the warning could need to be updated.. */
    rec_sound.Func(REC_SOUND_WARNING_MSG_SHOW_STOP, 0, 0);

    /* Update the gui of card's status. */
    rec_sound.Gui(GUI_CARD_UPDATE, GUI_NO_CARD, 0);
    rec_sound.Gui(GUI_FLUSH, 0, 0);

    return ReturnValue;
}

static int rec_sound_card_error_removed(void)
{
    int ReturnValue = 0;

    APP_REMOVEFLAGS(app_rec_sound.GFlags, APP_AFLAGS_BUSY);
    APP_ADDFLAGS(app_rec_sound.Flags, REC_SOUND_FLAGS_MUXER_ERROR);
    rec_sound.Func(REC_SOUND_CARD_REMOVED, 0, 0);

    return ReturnValue;
}

static int rec_sound_card_new_insert(void)
{
    int ReturnValue = 0;

    /* Stop the warning message, because the warning could need to be updated.. */
    rec_sound.Func(REC_SOUND_WARNING_MSG_SHOW_STOP, 0, 0);

    /* Update the gui of card's status. */
    rec_sound.Gui(GUI_CARD_UPDATE, GUI_NO_CARD, 0);
    rec_sound.Gui(GUI_FLUSH, 0, 0);

    return ReturnValue;
}

static int rec_sound_card_storage_idle(void)
{
    int ReturnValue = 0;

    rec_sound.Func(REC_SOUND_SET_FILE_INDEX, 0, 0);

    AppUtil_CheckCardParam(0);
    if (!APP_CHECKFLAGS(app_rec_sound.GFlags, APP_AFLAGS_READY)) {
        return ReturnValue;/**<  App switched out*/
    }

    /* Update the gui of card's status. */
    rec_sound.Gui(GUI_CARD_UPDATE, GUI_CARD_READY, 0);
    rec_sound.Gui(GUI_FLUSH, 0, 0);

    /** card ready call auto record to start record*/
    rec_sound.Func(REC_SOUND_RECORD_AUTO_START, 0, 0);

    return ReturnValue;
}

static int rec_sound_card_storage_busy(void)
{
    int ReturnValue = 0;

    /* Update the gui of card's status. */
    rec_sound.Gui(GUI_CARD_UPDATE, GUI_CARD_REFRESHING, 0);
    rec_sound.Gui(GUI_FLUSH, 0, 0);

    return ReturnValue;
}

static int rec_sound_card_check_status(void)
{
    int ReturnValue = 0;

    ReturnValue = AppLibCard_CheckStatus(CARD_CHECK_WRITE);
    if (ReturnValue == CARD_STATUS_NO_CARD) {
        rec_sound.Func(REC_SOUND_WARNING_MSG_SHOW_START, GUI_WARNING_NO_CARD, 0);
        AmbaPrintColor(RED,"[app_rec_sound] Card error = %d  No Card", ReturnValue);
    } else if (ReturnValue == CARD_STATUS_WP_CARD) {
        rec_sound.Func(REC_SOUND_WARNING_MSG_SHOW_START, GUI_WARNING_CARD_PROTECTED, 0);
        AmbaPrintColor(RED,"[app_rec_sound] Card error = %d Write Protection Card", ReturnValue);
    } else if (ReturnValue == CARD_STATUS_NOT_ENOUGH_SPACE) {
        rec_sound.Func(REC_SOUND_CARD_FULL_HANDLE, 0, 0);
        AmbaPrintColor(RED,"[app_rec_sound] Card error = %d CARD_STATUS_NOT_ENOUGH_SPACE", ReturnValue);
    } else {
        AmbaPrintColor(RED,"[app_rec_sound] Card error = %d", ReturnValue);
    }

    return ReturnValue;
}

static int rec_sound_card_full_handle(void)
{
    int ReturnValue = 0;
#ifdef CONFIG_APP_ARD
	AppLibMonitorStorage_EnableMsg(1);
	rec_sound.Func(REC_SOUND_RECORD_STOP, 0, 0);
	rec_sound.Func(REC_SOUND_WARNING_MSG_SHOW_START, GUI_WARNING_CARD_FULL, 0);
#else
    /**disable storage runout msg send*/
    AppLibMonitorStorage_EnableMsg(0);
    AmbaPrintColor(GREEN,"[app_rec_sound] SEND MSG APPLIB_LOOP_ENC_START");
    /**send msg to start loop enc*/
    ReturnValue = AppLibStorageAsyncOp_SndMsg(HMSG_LOOP_ENC_START, DCIM_HDLR, 0);
#endif
    return ReturnValue;

}

static int rec_sound_file_id_update(UINT32 FileID)
{
    int ReturnValue = 0;
    /**update last id for serial mode if new filw close*/
    if (FileID > UserSetting->SetupPref.DmfMixLastIdx || UserSetting->SetupPref.DMFMode == DMF_MODE_RESET) {
        UserSetting->SetupPref.DmfMixLastIdx = FileID;
    }
    return ReturnValue;
}


/**
 *  @brief handle widget close msg
 *
 *  remove pop up flag and call auto record function to start record
 *
 *  @return =0 success
 *
 */
static int rec_sound_widget_closed(void)
{
    int ReturnValue = 0;

    if (APP_CHECKFLAGS(app_rec_sound.GFlags, APP_AFLAGS_POPUP)) {
        APP_REMOVEFLAGS(app_rec_sound.GFlags, APP_AFLAGS_POPUP);
        /** after menu close call auto record function to start record*/
        AmbaPrintColor(YELLOW,"[rec_sound] <widget_closed> record auto start");
        rec_sound.Func(REC_SOUND_RECORD_AUTO_START, 0, 0);
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
static int rec_sound_set_system_type(void)
{
    int ReturnValue = 0;

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
static int rec_sound_update_fchan_vout(UINT32 msg)
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
        return 0;
        break;
    }
    ReturnValue = AppLibDisp_SelectDevice(DISP_CH_FCHAN, DISP_ANY_DEV);
    if (APP_CHECKFLAGS(ReturnValue, DISP_FCHAN_NO_CHANGE)) {
        AmbaPrint("[app_rec_sound] Display FCHAN has no changed");
    } else {
        if (APP_CHECKFLAGS(ReturnValue, DISP_FCHAN_NO_DEVICE)) {
            AppLibDisp_ChanStop(DISP_CH_FCHAN);
            AppLibDisp_DeactivateWindow(DISP_CH_FCHAN, AppLibDisp_GetWindowId(DISP_CH_FCHAN, 0));
            AppLibDisp_FlushWindow(DISP_CH_FCHAN);
            app_status.LockDecMode = 0;
        } else {
            AppLibDisp_ConfigMode(DISP_CH_FCHAN, AppLibSysVout_GetVoutMode(VOUT_DISP_MODE_2160P_HALF));
            AppLibDisp_SetupChan(DISP_CH_FCHAN);
            AppLibDisp_ChanStart(DISP_CH_FCHAN);
            {
                AMP_DISP_WINDOW_CFG_s Window;
                AMP_DISP_INFO_s DispDev = {0};

                memset(&Window, 0, sizeof(AMP_DISP_WINDOW_CFG_s));

                ReturnValue = AppLibDisp_GetDeviceInfo(DISP_CH_FCHAN, &DispDev);
                if ((ReturnValue < 0) || (DispDev.DeviceInfo.Enable == 0)) {
                    DBGMSG("[app_thumb_motion] FChan Disable. Disable the fchan window");
                    AppLibDisp_DeactivateWindow(DISP_CH_FCHAN, AppLibDisp_GetWindowId(DISP_CH_FCHAN, 0));
                    AppLibDisp_FlushWindow(DISP_CH_FCHAN);
                    app_status.LockDecMode = 0;
                } else {
                    /** FCHAN window*/
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
            rec_sound.Gui(GUI_SET_LAYOUT, 0, 0);
            rec_sound.Func(REC_SOUND_GUI_INIT_SHOW, 0, 0);
            rec_sound.Gui(GUI_FLUSH, 0, 0);
        }
    }

    return ReturnValue;
}

static int rec_sound_change_display(void)
{
    int ReturnValue = 0;

    AppLibDisp_SelectDevice(DISP_CH_FCHAN | DISP_CH_DCHAN, DISP_ANY_DEV);
    AppLibDisp_ConfigMode(DISP_CH_FCHAN | DISP_CH_DCHAN, AppLibSysVout_GetVoutMode(VOUT_DISP_MODE_1080P));
    AppLibDisp_SetupChan(DISP_CH_FCHAN | DISP_CH_DCHAN);
    AppLibDisp_ChanStart(DISP_CH_FCHAN | DISP_CH_DCHAN);
    {
        AMP_DISP_WINDOW_CFG_s Window;
        AMP_DISP_INFO_s DispDev = {0};

        memset(&Window, 0, sizeof(AMP_DISP_WINDOW_CFG_s));

        ReturnValue = AppLibDisp_GetDeviceInfo(DISP_CH_FCHAN, &DispDev);
        if ((ReturnValue < 0) || (DispDev.DeviceInfo.Enable == 0)) {
            DBGMSG("[app_rec_sound] FChan Disable. Disable the fchan Window");
            AppLibDisp_DeactivateWindow(DISP_CH_FCHAN, AppLibDisp_GetWindowId(DISP_CH_FCHAN, 0));
        } else {
            /** FCHAN window*/
            AppLibDisp_GetWindowConfig(DISP_CH_FCHAN, AppLibDisp_GetWindowId(DISP_CH_FCHAN, 0), &Window);
            Window.Source = AMP_DISP_DEC;
            Window.SourceDesc.Dec.DecHdlr = 0;
            Window.CropArea.Width = 0;
            Window.CropArea.Height = 0;
            Window.CropArea.X = 0;
            Window.CropArea.Y = 0;
            Window.TargetAreaOnPlane.Width = DispDev.DeviceInfo.VoutWidth;
            Window.TargetAreaOnPlane.Height = DispDev.DeviceInfo.VoutHeight;//  interlance should be consider in MW
            Window.TargetAreaOnPlane.X = 0;
            Window.TargetAreaOnPlane.Y = 0;
            AppLibDisp_SetWindowConfig(DISP_CH_FCHAN, AppLibDisp_GetWindowId(DISP_CH_FCHAN, 0), &Window);
            AppLibDisp_ActivateWindow(DISP_CH_FCHAN, AppLibDisp_GetWindowId(DISP_CH_FCHAN, 0));
        }

        ReturnValue = AppLibDisp_GetDeviceInfo(DISP_CH_DCHAN, &DispDev);
        if ((ReturnValue < 0) || (DispDev.DeviceInfo.Enable == 0)) {
            DBGMSG("[app_rec_sound] DChan Disable. Disable the Dchan Window");
            AppLibDisp_DeactivateWindow(DISP_CH_DCHAN, AppLibDisp_GetWindowId(DISP_CH_DCHAN, 0));
        } else {
            /** DCHAN window*/
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

    return ReturnValue;
}

static int rec_sound_change_osd(void)
{
    int ReturnValue = 0;

    // ToDo: need to remove to handler when iav completes the dsp cmd queue mechanism
    /* Update graphic window*/
    AppLibGraph_SetWindowConfig(GRAPH_CH_FCHAN | GRAPH_CH_DCHAN);
    AppLibGraph_ActivateWindow(GRAPH_CH_FCHAN | GRAPH_CH_DCHAN);
    AppLibGraph_FlushWindow(GRAPH_CH_FCHAN | GRAPH_CH_DCHAN);
    rec_sound.Gui(GUI_SET_LAYOUT, 0, 0);
    rec_sound.Gui(GUI_FLUSH, 0, 0);

    return ReturnValue;
}

static int rec_sound_usb_connect(void)
{
    int ReturnValue = 0;

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
static int rec_sound_start_show_gui(void)
{
    int ReturnValue = 0;
    int GuiParam = 0;

    /* Clean vout buffer */
    AppLibThmBasic_ClearScreen();

    // check encode status
    rec_sound.Gui(GUI_APP_SOUND_ICON_SHOW, 0, 0);
    // show power status
    rec_sound.Gui(GUI_POWER_STATE_UPDATE, app_status.PowerType, app_status.BatteryState);
    rec_sound.Gui(GUI_POWER_STATE_SHOW, app_status.PowerType, app_status.BatteryState);

    // show SD card status
    ReturnValue = AppLibCard_CheckStatus(0);
    if (ReturnValue == CARD_STATUS_NO_CARD) {
        GuiParam = GUI_NO_CARD;
    } else {
        GuiParam = GUI_CARD_READY;
    }
    rec_sound.Gui(GUI_CARD_UPDATE, GuiParam, 0);
    rec_sound.Gui(GUI_CARD_SHOW, 0, 0);

    // draw
    rec_sound.Gui(GUI_FLUSH, 0, 0);
    return ReturnValue;
}

static int rec_sound_update_bat_power_status(int param1)
{
    int ReturnValue = 0;

    /* Update the gui of power's status. */
    if (param1 == 0) {
        /*Hide the battery gui.*/
        rec_sound.Gui(GUI_POWER_STATE_HIDE, GUI_HIDE_POWER_EXCEPT_DC, 0);
    } else if (param1 == 1) {
        /*Update the battery gui.*/
        rec_sound.Gui(GUI_POWER_STATE_UPDATE, app_status.PowerType, app_status.BatteryState);
        rec_sound.Gui(GUI_POWER_STATE_SHOW, app_status.PowerType, app_status.BatteryState);
    } else if (param1 == 2) {
        /*Reset the battery and power gui.*/
        rec_sound.Gui(GUI_POWER_STATE_HIDE, 0, 0);
        rec_sound.Gui(GUI_POWER_STATE_UPDATE, app_status.PowerType, app_status.BatteryState);
        rec_sound.Gui(GUI_POWER_STATE_SHOW, app_status.PowerType, app_status.BatteryState);
    }
    rec_sound.Gui(GUI_FLUSH, 0, 0);

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
static void rec_sound_warning_timer_handler(int eid)
{
    static int blink_count = 0;

    if (eid == TIMER_UNREGISTER) {
        blink_count = 0;
        return;
    }

    blink_count++;

    if (blink_count & 0x01) {
        if (blink_count >= 5) {
            APP_REMOVEFLAGS(app_rec_sound.Flags, REC_SOUND_FLAGS_WARNING_MSG_RUN);
            AppLibComSvcTimer_Unregister(TIMER_2HZ, rec_sound_warning_timer_handler);
        }
        rec_sound.Gui(GUI_WARNING_HIDE, 0, 0);
    } else {
        rec_sound.Gui(GUI_WARNING_SHOW, 0, 0);
    }

    rec_sound.Gui(GUI_FLUSH, 0, 0);

}


static int rec_sound_warning_msg_show(int enable, int param1, int param2)
{
    int ReturnValue = 0;

    if (enable) {
        /* To show the warning message. */
        if (param2) {
            rec_sound.Gui(GUI_WARNING_UPDATE, param1, 0);
            rec_sound.Gui(GUI_WARNING_SHOW, 0, 0);
        } else if (!APP_CHECKFLAGS(app_rec_sound.Flags, REC_SOUND_FLAGS_WARNING_MSG_RUN)) {
            APP_ADDFLAGS(app_rec_sound.Flags, REC_SOUND_FLAGS_WARNING_MSG_RUN);
            rec_sound.Gui(GUI_WARNING_UPDATE, param1, 0);
            rec_sound.Gui(GUI_WARNING_SHOW, 0, 0);

            AppLibComSvcTimer_Register(TIMER_2HZ, rec_sound_warning_timer_handler);
        }
    } else {
        /* To disable the warning message. */
        if (APP_CHECKFLAGS(app_rec_sound.Flags, REC_SOUND_FLAGS_WARNING_MSG_RUN)) {
            APP_REMOVEFLAGS(app_rec_sound.Flags, REC_SOUND_FLAGS_WARNING_MSG_RUN);
            AppLibComSvcTimer_Unregister(TIMER_2HZ, rec_sound_warning_timer_handler);
        }
        rec_sound.Gui(GUI_WARNING_HIDE, 0, 0);
    }
    rec_sound.Gui(GUI_FLUSH, 0, 0);

    return ReturnValue;
}

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
int rec_sound_func(UINT32 funcId, UINT32 param1, UINT32 param2)
{
    int ReturnValue = 0;

    switch (funcId) {
    case REC_SOUND_INIT:
        ReturnValue = rec_sound_init();
        break;
    case REC_SOUND_START:
        ReturnValue = rec_sound_start();
        break;
    case REC_SOUND_STOP:
        ReturnValue = rec_sound_stop();
        break;
    case REC_SOUND_RECORD_START:
        ReturnValue = rec_sound_record_start();
        break;
    case REC_SOUND_RECORD_PAUSE:
        ReturnValue = rec_sound_record_pause();
        break;
    case REC_SOUND_RECORD_RESUME:
        ReturnValue = rec_sound_record_resume();
        break;
    case REC_SOUND_RECORD_STOP:
        ReturnValue = rec_sound_record_stop();
        break;
    case REC_SOUND_RECORD_AUTO_START:
        ReturnValue = rec_sound_record_auto_start();
        break;
    case REC_SOUND_MUXER_END:
        ReturnValue = rec_sound_muxer_end();
        break;
    case REC_SOUND_MUXER_REACH_LIMIT:
        ReturnValue = rec_sound_muxer_reach_limit(param1);
        break;
    case REC_SOUND_ERROR_MEMORY_RUNOUT:
        ReturnValue = rec_sound_error_memory_runout();
        break;
    case REC_SOUND_ERROR_STORAGE_RUNOUT:
        ReturnValue = rec_sound_error_storage_runout();
        break;
    case REC_SOUND_ERROR_STORAGE_IO:
        ReturnValue = rec_sound_error_storage_io();
        break;
    case REC_SOUND_ERROR_LOOP_ENC_ERR:
        ReturnValue = rec_sound_error_loop_enc_err(param1);
        break;
    case REC_SOUND_LOOP_ENC_DONE:
        ReturnValue = rec_sound_loop_enc_done();
        break;
    case REC_SOUND_SWITCH_APP:
        ReturnValue = rec_sound_switch_app();
        break;
    case REC_SOUND_CARD_REMOVED:
        ReturnValue = rec_sound_card_removed();
        break;
    case REC_SOUND_CARD_ERROR_REMOVED:
        ReturnValue = rec_sound_card_error_removed();
        break;
    case REC_SOUND_CARD_NEW_INSERT:
        ReturnValue = rec_sound_card_new_insert();
        break;
    case REC_SOUND_CARD_STORAGE_IDLE:
        ReturnValue = rec_sound_card_storage_idle();
        break;
    case REC_SOUND_CARD_STORAGE_BUSY:
        ReturnValue = rec_sound_card_storage_busy();
        break;
    case REC_SOUND_CARD_CHECK_STATUS:
        ReturnValue = rec_sound_card_check_status();
        break;
    case REC_SOUND_CARD_FULL_HANDLE:
        ReturnValue = rec_sound_card_full_handle();
        break;
    case REC_SOUND_FILE_ID_UPDATE:
        ReturnValue = rec_sound_file_id_update(param1);
        break;
    case REC_SOUND_WIDGET_CLOSED:
        ReturnValue = rec_sound_widget_closed();
        break;
    case REC_SOUND_SET_SYSTEM_TYPE:
        ReturnValue = rec_sound_set_system_type();
        break;
    case REC_SOUND_UPDATE_FCHAN_VOUT:
        ReturnValue = rec_sound_update_fchan_vout(param1);
        break;
    case REC_SOUND_UPDATE_DCHAN_VOUT:
        break;
    case REC_SOUND_CHANGE_DISPLAY:
        ReturnValue = rec_sound_change_display();
        break;
    case REC_SOUND_CHANGE_OSD:
        ReturnValue = rec_sound_change_osd();
        break;
    case REC_SOUND_USB_CONNECT:
        ReturnValue = rec_sound_usb_connect();
        break;
    case REC_SOUND_GUI_INIT_SHOW:
        ReturnValue = rec_sound_start_show_gui();
        break;
    case REC_SOUND_UPDATE_BAT_POWER_STATUS:
        ReturnValue = rec_sound_update_bat_power_status(param1);
        break;
    case REC_SOUND_WARNING_MSG_SHOW_START:
        ReturnValue = rec_sound_warning_msg_show( 1, param1, param2);
        break;
    case REC_SOUND_WARNING_MSG_SHOW_STOP:
        ReturnValue = rec_sound_warning_msg_show( 0, param1, param2);
        break;
    default:
        break;
    }

    return ReturnValue;
}
