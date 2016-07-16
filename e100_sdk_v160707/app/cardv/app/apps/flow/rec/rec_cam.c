/**
  * @file src/app/apps/flow/rec/rec_cam.c
  *
  * Implementation of Sport Recorder (sensor) application
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

#ifdef CONFIG_APP_ARD
#include <apps/gui/resource/gui_settle.h>
#include <system/status.h>
#include <system/app_pref.h>
#include <apps/flow/widget/menu/menu.h>
#endif



/*************************************************************************
 * Declarations (static)
 ************************************************************************/
/* App structure interfaces APIs */
static int app_rec_cam_start(void);
static int app_rec_cam_stop(void);
static int app_rec_cam_on_message(UINT32 msg, UINT32 param1, UINT32 param2);

APP_APP_s app_rec_cam = {
    0,  //Id
    1,  //Tier
    0,  //Parent
    0,  //Previous
    0,  //Child
    0,  //GFlags
    0,  //Flags
    app_rec_cam_start,//start()
    app_rec_cam_stop,    //stop()
    app_rec_cam_on_message  //OnMessage()
};

/* App status */
REC_CAM_s rec_cam = {0};

#ifdef CONFIG_APP_ARD
extern int rec_cam_set_menu_locked(int menu_locked);
extern int rec_cam_get_menu_locked(void);
#endif

/*************************************************************************
 * Definitions (static)
 ************************************************************************/
/* App structure interface APIs */

/**
 *  @brief The application's function that handle the message.
 *
 *  The application's function that handle the message.
 *
 *  @param [in] msg Message ID
 *  @param [in] param1 first parameter
 *  @param [in] param2 second parameter
 *
 *  @return >=0 success, <0 failure
 */
static int app_rec_cam_on_message(UINT32 msg, UINT32 param1, UINT32 param2)
{
    int ReturnValue = 0;
    DBGMSG("[app_rec_cam] Received msg: 0x%X (param1 = 0x%X / param2 = 0x%X)", msg, param1, param2);

    ReturnValue = AppWidget_OnMessage(msg, param1, param2);
    if (ReturnValue != WIDGET_PASSED_MSG) {
        return ReturnValue;
    }
    switch (msg) {
    case HMSG_RECORDER_STATE_LIVEVIEW:
        DBGMSG("[app_rec_cam] Received HMSG_RECORDER_STATE_LIVEVIEW");
        ReturnValue = rec_cam.Func(REC_CAM_LIVEVIEW_STATE, 0, 0);
        break;
    case HMSG_RECORDER_STATE_PHOTO_CAPTURE_COMPLETE:
        ReturnValue = rec_cam.Func(REC_CAM_CAPTURE_COMPLETE, 0, 0);
        break;
    case HMSG_RECORDER_STATE_PHOTO_BGPROC_COMPLETE:
        ReturnValue = rec_cam.Func(REC_CAM_CAPTURE_BG_PROCESS_DONE, 0, 0);
        break;
    case HMSG_USER_SNAP1_BUTTON:
    case HMSG_USER_IR_SNAP1_BUTTON:
        if (!APP_CHECKFLAGS(app_rec_cam.GFlags, APP_AFLAGS_READY)) {
            break;    // not ready
        }
        ReturnValue = rec_cam.Op->ButtonFocus();
        break;
    case HMSG_USER_SNAP1_BUTTON_CLR:
    case HMSG_USER_IR_SNAP1_BUTTON_CLR:
        if (!APP_CHECKFLAGS(app_rec_cam.GFlags, APP_AFLAGS_READY)) {
            break;    // not ready
        }
        ReturnValue = rec_cam.Op->ButtonFocusClr();
        break;
    case HMSG_USER_SNAP2_BUTTON:
    case HMSG_USER_IR_SNAP2_BUTTON:
        if (!APP_CHECKFLAGS(app_rec_cam.GFlags, APP_AFLAGS_READY)) {
            break;    // not ready
        }
        ReturnValue = rec_cam.Op->ButtonShutter();
        break;
    case HMSG_USER_SNAP2_BUTTON_CLR:
    case HMSG_USER_IR_SNAP2_BUTTON_CLR:
        if (!APP_CHECKFLAGS(app_rec_cam.GFlags, APP_AFLAGS_READY)) {
            break;    // not ready
        }
        ReturnValue = rec_cam.Op->ButtonShutterClr();
        break;
    case HMSG_USER_RECORD_BUTTON:
    case HMSG_USER_IR_RECORD_BUTTON:
        if (!APP_CHECKFLAGS(app_rec_cam.GFlags, APP_AFLAGS_READY)) {
            break;    // not ready
        }
        ReturnValue = rec_cam.Op->ButtonRecord();
        break;
    case HMSG_USER_ZOOM_IN_BUTTON:
    case HMSG_USER_IR_ZOOM_IN_BUTTON:
        if (!APP_CHECKFLAGS(app_rec_cam.GFlags, APP_AFLAGS_READY)) {
            break;    // not ready
        }
        ReturnValue = rec_cam.Op->ButtonZoomIn();
        break;
    case HMSG_USER_ZOOM_OUT_BUTTON:
    case HMSG_USER_IR_ZOOM_OUT_BUTTON:
        if (!APP_CHECKFLAGS(app_rec_cam.GFlags, APP_AFLAGS_READY)) {
            break;    // not ready
        }
        ReturnValue = rec_cam.Op->ButtonZoomOut();
        break;
    case HMSG_USER_ZOOM_IN_BUTTON_CLR:
    case HMSG_USER_IR_ZOOM_IN_BUTTON_CLR:
        if (!APP_CHECKFLAGS(app_rec_cam.GFlags, APP_AFLAGS_READY)) {
            break;    // not ready
        }
        ReturnValue = rec_cam.Op->ButtonZoomInClr();
        break;
    case HMSG_USER_ZOOM_OUT_BUTTON_CLR:
    case HMSG_USER_IR_ZOOM_OUT_BUTTON_CLR:
        if (!APP_CHECKFLAGS(app_rec_cam.GFlags, APP_AFLAGS_READY)) {
            break;    // not ready
        }
        ReturnValue = rec_cam.Op->ButtonZoomOutClr();
        break;
    case HMSG_USER_UP_BUTTON:
    case HMSG_USER_IR_UP_BUTTON:
        if (!APP_CHECKFLAGS(app_rec_cam.GFlags, APP_AFLAGS_READY)) {
            break;    // not ready
        }
        ReturnValue = rec_cam.Op->ButtonUp();
        break;
    case HMSG_USER_DOWN_BUTTON:
    case HMSG_USER_IR_DOWN_BUTTON:
        if (!APP_CHECKFLAGS(app_rec_cam.GFlags, APP_AFLAGS_READY)) {
            break;    // not ready
        }
#ifdef CONFIG_BSP_ORTHRUS
        if (rec_cam.RecCapState == REC_CAP_STATE_RECORD){   /*Is Recording...*/
            ReturnValue = rec_cam.Op->ButtonF4();
        }else{
            ReturnValue = rec_cam.Op->ButtonDown();
        }
#else
        ReturnValue = rec_cam.Op->ButtonDown();
#endif
        break;
    case HMSG_USER_LEFT_BUTTON:
    case HMSG_USER_IR_LEFT_BUTTON:
        if (!APP_CHECKFLAGS(app_rec_cam.GFlags, APP_AFLAGS_READY)) {
            break;    // not ready
        }
        ReturnValue = rec_cam.Op->ButtonLeft();
        break;
    case HMSG_USER_RIGHT_BUTTON:
    case HMSG_USER_IR_RIGHT_BUTTON:
        if (!APP_CHECKFLAGS(app_rec_cam.GFlags, APP_AFLAGS_READY)) {
            break;    // not ready
        }
        ReturnValue = rec_cam.Op->ButtonRight();
        break;
    case HMSG_USER_SET_BUTTON:
    case HMSG_USER_IR_SET_BUTTON:
        if (!APP_CHECKFLAGS(app_rec_cam.GFlags, APP_AFLAGS_READY)) {
            break;    // not ready
        }
        ReturnValue = rec_cam.Op->ButtonSet();
        break;
#ifdef CONFIG_APP_ARD
    case AMSG_CMD_EVENT_RECORD:
#endif
    case HMSG_USER_F4_BUTTON:
    case HMSG_USER_IR_N4_BUTTON:
        if (!APP_CHECKFLAGS(app_rec_cam.GFlags, APP_AFLAGS_READY)) {
            break;    // not ready
        }
        #ifndef CONFIG_ECL_GUI  
        ReturnValue = rec_cam.Op->ButtonF4();
        #else
         ReturnValue = rec_cam.Op->ButtonRight();
        #endif
        break;
        
    case HMSG_USER_MENU_BUTTON:
    case HMSG_USER_IR_MENU_BUTTON:
#ifdef CONFIG_APP_ARD
        if (APP_CHECKFLAGS(app_rec_cam.GFlags, APP_AFLAGS_POPUP)) {
            APP_REMOVEFLAGS(app_rec_cam.GFlags, APP_AFLAGS_POPUP);
        }
#endif
        if (!APP_CHECKFLAGS(app_rec_cam.GFlags, APP_AFLAGS_READY)) {
            break;    // not ready
        }

        ReturnValue = rec_cam.Op->ButtonMenu();
#ifdef CONFIG_APP_ARD
    AppLibAudioDec_Beep(BEEP_OPTONE,0);
#endif
        break;
    case HMSG_USER_MODE_BUTTON:
    case HMSG_USER_IR_MODE_BUTTON:
#ifdef CONFIG_APP_ARD
    if (!APP_CHECKFLAGS(app_rec_cam.GFlags, APP_AFLAGS_READY))
        break;

    AppLibAudioDec_Beep(BEEP_OPTONE,0);
#endif
        ReturnValue = rec_cam.Op->ButtonMode();
        break;
    case HMSG_USER_DEL_BUTTON:
    case HMSG_USER_IR_DEL_BUTTON:
        if (!APP_CHECKFLAGS(app_rec_cam.GFlags, APP_AFLAGS_READY)) {
            break;    // not ready
        }
        ReturnValue = rec_cam.Op->ButtonDel();
        break;
    case HMSG_USER_POWER_BUTTON:
        if (!APP_CHECKFLAGS(app_rec_cam.GFlags, APP_AFLAGS_READY)) {
            break;    // not ready
        }
        ReturnValue = rec_cam.Op->ButtonPower();
        break;
#ifdef CONFIG_APP_ARD
    case HMSG_USER_F1_BUTTON:
        if (!APP_CHECKFLAGS(app_rec_cam.GFlags, APP_AFLAGS_READY)) {
            break;    // not ready
        }
        //check card full
        {
            int CardStatus = 0;
            CardStatus = AppLibCard_CheckStatus(CARD_CHECK_WRITE);
            if (CardStatus == CARD_STATUS_NO_CARD) {
                rec_cam.Func(REC_CAM_WARNING_MSG_SHOW_START, GUI_WARNING_NO_CARD, 0);
                AmbaPrintColor(RED,"[app_rec_cam] Card error = %d  No Card", CardStatus);
                break;
            } else if (CardStatus == CARD_STATUS_WP_CARD) {
                rec_cam.Func(REC_CAM_WARNING_MSG_SHOW_START, GUI_WARNING_CARD_PROTECTED, 0);
                AmbaPrintColor(RED,"[app_rec_cam] Card error = %d Write Protection Card", CardStatus);
                break;
            } else if (CardStatus == CARD_STATUS_NOT_ENOUGH_SPACE) {
                rec_cam.Func(REC_CAM_WARNING_MSG_SHOW_START, GUI_WARNING_CARD_FULL, 0);
                if (param1 == 0) {// to handle card full situation
                rec_cam.Func(REC_CAM_CARD_FULL_HANDLE, 0, 0);
                }
                AmbaPrintColor(RED,"[app_rec_cam] Card error = %d CARD_STATUS_NOT_ENOUGH_SPACE, param1 = %d (0 to do full handle)", CardStatus, param1);
                break;
            } else {
                if (CardStatus != CARD_STATUS_CHECK_PASS ) {
                    AmbaPrintColor(RED,"[app_rec_cam] Card error = %d", CardStatus);
                    break;
                }
            }
        }
        ReturnValue = AppUtil_SwitchMode(APP_REC_SOUND);
        break;
#endif
    case AMSG_CMD_SWITCH_APP:
        ReturnValue = rec_cam.Func(REC_CAM_SWITCH_APP, param1, param2);
        break;
    case HMSG_MUXER_START:
        DBGMSG("[app_rec_cam] Received HMSG_MUXER_START");
#ifdef CONFIG_APP_ARD
        APP_ADDFLAGS(app_rec_cam.Flags, REC_CAM_FLAGS_APP_RECEIVED_MUX_START);
#endif
        ReturnValue = rec_cam.Func(REC_CAM_MUXER_START, param1, param2);
        break;
    case HMSG_MUXER_END:
        DBGMSG("[app_rec_cam] Received HMSG_MUXER_END");
        ReturnValue = rec_cam.Func(REC_CAM_MUXER_END, param1, param2);
        break;
    case HMSG_MUXER_OPEN:
        DBGMSG("[app_rec_cam] Received HMSG_MUXER_OPEN");
        ReturnValue = rec_cam.Func(REC_CAM_MUXER_OPEN, param1, param2);
        break;
    case HMSG_MUXER_END_EVENTRECORD:
        DBGMSG("[app_rec_cam] Received HMSG_MUXER_END_EVENTRECORD");
        ReturnValue = rec_cam.Func(REC_CAM_MUXER_END_EVENTRECORD, param1, param2);
        break;
#ifdef CONFIG_APP_ARD
    case HMSG_MUXER_OPEN_EVENTRECORD:
        ReturnValue = rec_cam.Func(REC_CAM_MUXER_OPEN_EVENTRECORD, param1, param2);
        break;
#ifdef CONFIG_APP_EVENT_OVERLAP
    case HMSG_MUXER_FORMAT_CHANGED:
        if(CURRENT_FORMAT_NORMAL == param1) {
            UserSetting->VideoPref.UnsavingData = 0;
            UserSetting->VideoPref.UnsavingData = UserSetting->VideoPref.UnsavingData | 1;
        } else if(CURRENT_FORMAT_EVENT == param1) {
            UserSetting->VideoPref.UnsavingData = 0;
            UserSetting->VideoPref.UnsavingData = UserSetting->VideoPref.UnsavingData | (1<<1);
        }
        AppPref_Save();
        break;
#endif
#endif
    case HMSG_MUXER_REACH_LIMIT:
        DBGMSG("[app_rec_cam] Received HMSG_MUXER_REACH_LIMIT");
        if (!APP_CHECKFLAGS(app_rec_cam.GFlags, APP_AFLAGS_READY)) {
            break;    // not ready
        }
        ReturnValue = rec_cam.Func(REC_CAM_MUXER_REACH_LIMIT, param1, param2);
        break;
    case HMSG_MUXER_REACH_LIMIT_EVENTRECORD:
        DBGMSG("[app_rec_cam] Received HMSG_MUXER_REACH_LIMIT_EVENTRECORD");
        if (!APP_CHECKFLAGS(app_rec_cam.GFlags, APP_AFLAGS_READY)) {
            break;    // not ready
        }
        ReturnValue = rec_cam.Func(REC_CAM_MUXER_REACH_LIMIT_EVENTRECORD, param1, param2);
        break;
#ifdef CONFIG_APP_ARD
    case HMSG_MUXER_IO_ERROR_EVENTRECORD:
        if (!APP_CHECKFLAGS(app_rec_cam.GFlags, APP_AFLAGS_READY)) {
            break;    // not ready
        }
        ReturnValue = rec_cam.Func(REC_CAM_MUXER_IO_ERROR_EVENTRECORD, param1, param2);
        break;
#endif
    case HMSG_DCF_FILE_CLOSE:
#ifdef CONFIG_APP_ARD
        if(0 == param2) {
            if(1 == rec_cam_get_menu_locked()) {
                AppMenu_UnlockTab(MENU_VIDEO);
                AppMenu_UnlockTab(MENU_SETUP);
                rec_cam_set_menu_locked(0);
            }
            if (AppWidget_GetCur() == WIDGET_MENU) {
                //AppMenu_ReflushItem();
            }
        }
#endif

        ReturnValue = rec_cam.Func(REC_CAM_FILE_ID_UPDATE, param1, param2);
        DBGMSG("[app_rec_cam] Received HMSG_DCF_FILE_CLOSE, file object ID = %d",param1);
        break;
    case HMSG_MEMORY_FIFO_BUFFER_RUNOUT:
        DBGMSG("[app_rec_cam] Received HMSG_MEMORY_FIFO_BUFFER_RUNOUT");
        if (!APP_CHECKFLAGS(app_rec_cam.GFlags, APP_AFLAGS_READY)) {
            break;    // not ready
        }
        ReturnValue = rec_cam.Func(REC_CAM_ERROR_MEMORY_RUNOUT, param1, param2);
        break;
    case HMSG_MUXER_IO_ERROR:
        DBGMSG("[app_rec_cam] Received HMSG_MUXER_IO_ERROR");
        if (!APP_CHECKFLAGS(app_rec_cam.GFlags, APP_AFLAGS_READY)) {
            break;    // not ready
        }
        ReturnValue = rec_cam.Func(REC_CAM_MUXER_STREAM_ERROR, param1, param2);
        break;
#ifdef CONFIG_APP_ARD
    case HMSG_MUXER_GENERAL_ERROR:
        if (!APP_CHECKFLAGS(app_rec_cam.GFlags, APP_AFLAGS_READY)) {
            break;    // not ready
        }
        ReturnValue = rec_cam.Func(REC_CAM_MUXER_GENERAL_ERROR, param1, param2);
        break;
#endif
    case HMSG_MUXER_FIFO_ERROR:
        DBGMSG("[app_rec_cam] Received AMP_MUXER_EVENT_FIFO_ERROR");
        if (!APP_CHECKFLAGS(app_rec_cam.GFlags, APP_AFLAGS_READY)) {
            break;    // not ready
        }
        ReturnValue = rec_cam.Func(REC_CAM_ERROR_MEMORY_RUNOUT, param1, param2);
        break;
    case AMSG_ERROR_CARD_REMOVED:
        if (APP_CHECKFLAGS(app_rec_cam.GFlags, APP_AFLAGS_READY)) {
            ReturnValue = rec_cam.Func(REC_CAM_CARD_ERROR_REMOVED, param1, param2);
        }
        break;
    case AMSG_STATE_CARD_REMOVED:
        if (APP_CHECKFLAGS(app_rec_cam.GFlags, APP_AFLAGS_READY)) {
            ReturnValue = rec_cam.Func(REC_CAM_CARD_REMOVED, param1, param2);
        }
        break;
    case HMSG_STORAGE_BUSY:
        if (APP_CHECKFLAGS(app_rec_cam.GFlags, APP_AFLAGS_READY)) {
            ReturnValue = rec_cam.Func(REC_CAM_CARD_STORAGE_BUSY, param1, param2);
        }
        break;
    case HMSG_STORAGE_IDLE:
        if (APP_CHECKFLAGS(app_rec_cam.GFlags, APP_AFLAGS_READY)) {
            ReturnValue = rec_cam.Func(REC_CAM_CARD_STORAGE_IDLE, param1, param2);
        }
        break;
    case HMSG_STORAGE_RUNOUT:
        DBGMSG("[app_rec_cam] Received HMSG_STORAGE_RUNOUT");
        if (!APP_CHECKFLAGS(app_rec_cam.GFlags, APP_AFLAGS_READY)) {
            break;    // not ready
        }
        ReturnValue = rec_cam.Func(REC_CAM_ERROR_STORAGE_RUNOUT, param1, param2);
        break;
    case HMSG_STORAGE_REACH_FILE_LIMIT:
    case HMSG_STORAGE_REACH_FILE_NUMBER:
        break;
    case HMSG_LOOP_ENC_DONE:
        ReturnValue = rec_cam.Func(REC_CAM_LOOP_ENC_DONE, 0, 0);
        break;
    case HMSG_LOOP_ENC_ERROR:
        ReturnValue = rec_cam.Func(REC_CAM_ERROR_LOOP_ENC_ERR, param1, 0);
        break;
    case HMSG_EDTMGR_SUCCESS:
        AmbaPrintColor(5,"[app_rec_cam]HMSG_EDTMGR_SUCCESS");
        ReturnValue = rec_cam.Func(REC_CAM_EDTMGR_DONE, 0, 0);
        break;
    case HMSG_EDTMGR_FAIL:
        AmbaPrintColor(5,"[app_rec_cam]HMSG_EDTMGR_FAIL");
        ReturnValue = rec_cam.Func(REC_CAM_EDTMGR_FAIL, 0, 0);
        break;
    case HMSG_EM_RECORD_RETURN:
        AmbaPrintColor(5,"HMSG_EM_RECORD_RETURN");
        ReturnValue = rec_cam.Func(REC_CAM_EM_RECORD_RETURN, param1, 0);
        break;
    case HMSG_HDMI_INSERT_SET:
    case HMSG_HDMI_INSERT_CLR:
    case HMSG_CS_INSERT_SET:
    case HMSG_CS_INSERT_CLR:
        if (!APP_CHECKFLAGS(app_rec_cam.GFlags, APP_AFLAGS_READY)) {
            AmbaPrint("[app_rec_cam] System is not ready. Jack event will be handled later");
        } else {
            ReturnValue = rec_cam.Func(REC_CAM_UPDATE_FCHAN_VOUT, msg, param1);
        }
        break;
    case HMSG_LINEIN_IN_SET:
    case HMSG_LINEIN_IN_CLR:
        if (!APP_CHECKFLAGS(app_rec_cam.GFlags, APP_AFLAGS_READY)) {
            AmbaPrint("[app_rec_cam] System is not ready. Jack event will be handled later");
        } else {
            ReturnValue = rec_cam.Func(REC_CAM_AUDIO_INPUT, param1, param2);
        }
        break;
    case HMSG_HP_IN_SET:
    case HMSG_HP_IN_CLR:
    case HMSG_LINEOUT_IN_SET:
    case HMSG_LINEOUT_IN_CLR:
        if (!APP_CHECKFLAGS(app_rec_cam.GFlags, APP_AFLAGS_READY)) {
            AmbaPrint("[app_rec_cam] System is not ready. Jack event will be handled later");
        } else {
            ReturnValue = rec_cam.Func(REC_CAM_AUDIO_OUTPUT, param1, param2);
        }
        break;
#ifdef CONFIG_APP_ARD
    case HMSG_VA_MD_Y:
    case HMSG_VA_MD_AE:
        ReturnValue = rec_cam.Func(REC_CAM_MOTION_DETECT_RECORD, msg, param1);
        break;
#endif
    case HMSG_VA_FCAR_DEPARTURE:
    case HMSG_VA_LOW_LIGHT:
#ifndef CONFIG_APP_ARD
    case HMSG_VA_MD_Y:
    case HMSG_VA_MD_AE:
#endif
    case HMSG_VA_LDW:
    case HMSG_VA_FCW:
    case HMSG_VA_UPDATE:
    case HMSG_VA_CLIBRATION_DONE:
        ReturnValue = rec_cam.Func(REC_CAM_ADAS_EVENT, msg, param1);
        break;
    case AMSG_CMD_USB_APP_START:
    case HMSG_USB_DETECT_CONNECT:
        ReturnValue = rec_cam.Func(REC_CAM_USB_CONNECT, param1, param2);
        break;
    case AMSG_CMD_SET_VIDEO_RES:
        ReturnValue = rec_cam.Func(REC_CAM_SET_VIDEO_RES, param1, param2);
        break;
    case AMSG_CMD_SET_VIDEO_QUALITY:
        ReturnValue = rec_cam.Func(REC_CAM_SET_VIDEO_QUALITY, param1, param2);
        break;
    case AMSG_CMD_SET_VIDEO_PRE_RECORD:
        ReturnValue = rec_cam.Func(REC_CAM_SET_VIDEO_PRE_RECORD, param1, param2);
        break;
    case AMSG_CMD_SET_VIDEO_TIME_LAPSE:
        ReturnValue = rec_cam.Func(REC_CAM_SET_VIDEO_TIME_LAPSE, param1, param2);
        break;
    case AMSG_CMD_SET_VIDEO_DUAL_STREAMS:
        ReturnValue = rec_cam.Func(REC_CAM_SET_VIDEO_DUAL_STREAMS, param1, param2);
        break;
    case AMSG_CMD_SET_PHOTO_SIZE:
        ReturnValue = rec_cam.Func(REC_CAM_SET_PHOTO_SIZE, param1, param2);
        break;
    case AMSG_CMD_SET_PHOTO_QUALITY:
        ReturnValue = rec_cam.Func(REC_CAM_SET_PHOTO_QUALITY, param1, param2);
        break;
    case AMSG_CMD_SET_RECORD_MODE:
        ReturnValue = rec_cam.Func(REC_CAM_SET_ENC_MODE, param1, param2);
        break;
    case AMSG_CMD_SET_APP_ENC_MODE:
        ReturnValue = rec_cam.Func(REC_CAM_SET_ENC_MODE, param1, param2);
        break;
    case AMSG_CMD_SET_DMF_MODE:
        DBGMSG("[app_rec_cam]Received AMSG_CMD_SET_DMF_MODE");
        ReturnValue = rec_cam.Func(REC_CAM_SET_DMF_MODE, param1, param2);
        break;
    case AMSG_CMD_CARD_FMT_NONOPTIMUM:
        ReturnValue = rec_cam.Func(REC_CAM_CARD_FMT_NONOPTIMUM, param1, param2);
        break;
    case AMSG_CMD_CARD_MOVIE_RECOVER:
#if defined(CONFIG_APP_AMBA_LINK)
        /*Stop VF to do busy check, busy check will enable movie recover*/
        if (rec_cam.RecCapState == REC_CAP_STATE_VF) {
            rec_cam.Func(REC_CAM_VF_STOP, 0, 0);
        }
#endif
        break;
    case AMSG_STATE_WIDGET_CLOSED:
        ReturnValue = rec_cam.Func(REC_CAM_WIDGET_CLOSED, param1, param2);
        break;
#ifdef CONFIG_APP_ARD
        case AMSG_STATE_BATTERY_STATE:
            rec_cam.Func(REC_CAM_UPDATE_BAT_POWER_STATUS, param1, param2);
        break;
        
        case AMSG_CMD_SET_SYSTEM_TYPE:
        ReturnValue = rec_cam.Func(REC_CAM_SET_SYSTEM_TYPE, param1, param2);
        break;
        case AMSG_CMD_SET_VIDEO_GSENSOR_SENSITIVITY:
        ReturnValue = rec_cam.Func(REC_CAM_VIDEO_SET_GSENSOR_SENSITIVITY, param1, param2);
        break;
        case AMSG_CMD_RESET_SPLIT_TIME:
        ReturnValue = rec_cam.Func(REC_CAM_SET_SPLIT_TIME, param1, param2);
        break;
        case AMSG_CMD_MOTION_RECORD_STOP:
        ReturnValue = rec_cam.Func(REC_CAM_RECORD_STOP, param1, param2);
        break;
#endif
#if defined(CONFIG_APP_AMBA_LINK)
    case AMSG_EVENT_BOSS_BOOTED:
        if (APP_CHECKFLAGS(app_rec_cam.GFlags, APP_AFLAGS_READY)) {
            /* Handling the case that HMSG_RECORDER_STATE_LIVEVIEW comes before AMSG_CMD_BOSS_BOOTED. */
            ReturnValue = rec_cam.Func(REC_CAM_BOSS_BOOTED, param1, param2);
        }
        break;
    case AMSG_NETFIFO_EVENT_START:
        ReturnValue = rec_cam.Func(REC_CAM_NETFIFO_EVENT_START, 0, 0);
        break;
    case AMSG_NETFIFO_EVENT_STOP:
        ReturnValue = rec_cam.Func(REC_CAM_NETFIFO_EVENT_STOP, 0, 0);
        break;
    case AMSG_NETCTRL_SESSION_START:
        ReturnValue = rec_cam.NetCtrl->NetCtrlRefreshPrefTable();
        break;
    case AMSG_NETCTRL_VF_RESET:
        ReturnValue = rec_cam.NetCtrl->NetCtrlVFReset();
        break;
    case AMSG_NETCTRL_VF_STOP:
        ReturnValue = rec_cam.NetCtrl->NetCtrlVFStop();
        break;
    case AMSG_NETCTRL_VIDEO_RECORD_START:
        ReturnValue = rec_cam.NetCtrl->NetCtrlRecordStart();
        break;
    case AMSG_NETCTRL_VIDEO_RECORD_STOP:
        ReturnValue = rec_cam.NetCtrl->NetCtrlRecordStop();
        break;
    case AMSG_NETCTRL_VIDEO_GET_RECORD_TIME:
        ReturnValue = rec_cam.NetCtrl->NetCtrlGetRecordTime();
        break;
    case AMSG_NETCTRL_PHOTO_TAKE_PHOTO:
        ReturnValue = rec_cam.NetCtrl->NetCtrlCapture();
        break;
    case AMSG_NETCTRL_PHOTO_CONTINUE_CAPTURE_STOP:
        ReturnValue = rec_cam.NetCtrl->NetCtrlContinueCaptureStop();
        break;
    case AMSG_NETCTRL_SYS_GET_SETTING_ALL:
        ReturnValue = rec_cam.NetCtrl->NetCtrlGetAllCurSetting();
        break;
    case AMSG_NETCTRL_SYS_GET_SINGLE_SETTING_OPTION:
        ReturnValue = rec_cam.NetCtrl->NetCtrlGetSettingOptions(param1, param2);
        break;
    case AMSG_NETCTRL_SYS_GET_SETTING:
        ReturnValue = rec_cam.NetCtrl->NetCtrlGetSetting(param1, param2);
        break;
    case AMSG_NETCTRL_SYS_SET_SETTING:
        ReturnValue = rec_cam.NetCtrl->NetCtrlSetSetting(param1, param2);
        break;
    case AMSG_NETCTRL_SYS_GET_NUMB_FILES:
        ReturnValue = rec_cam.NetCtrl->NetCtrlGetNumbFiles(param1, param2);
        break;
    case AMSG_NETCTRL_SYS_GET_DEVICE_INFO:
        ReturnValue = rec_cam.NetCtrl->NetCtrlGetDeviceInfo();
        break;
    case AMSG_NETCTRL_MEDIA_GET_THUMB:
        ReturnValue = rec_cam.NetCtrl->NetCtrlGetThumb(param1, param2);
        break;
    case AMSG_NETCTRL_MEDIA_GET_MEDIAINFO:
         ReturnValue = rec_cam.NetCtrl->NetCtrlGetMediaInfo(param1, param2);
        break;
    case AMSG_NETCTRL_CUSTOM_CMD:
        ReturnValue = rec_cam.NetCtrl->NetCtrlCustomCmd(param1, param2);
        break;
    case AMSG_NETCTRL_SYS_FORMAT:
        ReturnValue = rec_cam.NetCtrl->NetCtrlFormat(param1, param2);
        break;
    case ASYNC_MGR_MSG_CARD_FORMAT_DONE:
        ReturnValue = rec_cam.NetCtrl->NetCtrlFormatDone(param1, param2);
        break;
    case AMSG_NETCTRL_SYS_GET_SPACE:
        ReturnValue = rec_cam.NetCtrl->NetCtrlGetSpace(param1, param2);
        break;
    case HMSG_USER_VF_START:
        ReturnValue = rec_cam.Func(REC_CAM_VF_START, msg, 0);
        break;
    case HMSG_USER_VF_STOP:
        ReturnValue = rec_cam.Func(REC_CAM_VF_STOP, msg, 0);
        break;
    case HMSG_USER_VF_SWITCH_TO_RECORD:
        ReturnValue = rec_cam.Func(REC_CAM_VF_SWITCH_TO_RECORD, msg, 0);
        break;
    case HMSG_USER_PIRNT_REC_CAP_STATE:
        AmbaPrint("rec_cam.RecCapState = %d",rec_cam.RecCapState);
        break;
#endif
    default:
        break;
    }
    return ReturnValue;
}

/**
 *  @brief The start flow of recoder application.
 *
 *  The start flow of recoder application.
 *
 *  @return >=0 success, <0 failure
 */
static int app_rec_cam_start(void)
{
    int ReturnValue = 0;
#ifdef CONFIG_APP_ARD
    AppUtil_BatteryVoltagePrint();
#endif
    /* Set app function and operate sets */
    rec_cam.Func = rec_cam_func;
    rec_cam.Gui = gui_rec_cam_func;
    rec_cam.Op = &rec_cam_op;

    #if defined(CONFIG_APP_AMBA_LINK)
    rec_cam.NetCtrlFlags = 0;
    rec_cam.NetCtrl = &rec_cam_netctrl_op;
    #endif

    if (!APP_CHECKFLAGS(app_rec_cam.GFlags, APP_AFLAGS_INIT)) {
        APP_ADDFLAGS(app_rec_cam.GFlags, APP_AFLAGS_INIT);
        rec_cam.Func(REC_CAM_INIT, 0, 0);
    }

    if (APP_CHECKFLAGS(app_rec_cam.GFlags, APP_AFLAGS_START)) {
        rec_cam.Func(REC_CAM_START_FLAG_ON, 0, 0);
    } else {
        APP_ADDFLAGS(app_rec_cam.GFlags, APP_AFLAGS_START);

        rec_cam.Func(REC_CAM_SET_APP_ENV, 0, 0);

        rec_cam.Func(REC_CAM_START, 0, 0);
    }

    return ReturnValue;
}

/**
 *  @brief The stop flow of recoder application.
 *
 *  The stop flow of recoder application.
 *
 *  @return >=0 success, <0 failure
 */
static int app_rec_cam_stop(void)
{
    int ReturnValue = 0;

    if (!APP_CHECKFLAGS(app_rec_cam.GFlags, APP_AFLAGS_READY)) {
        rec_cam.Func(REC_CAM_STOP, 0, 0);
    }

    return ReturnValue;
}
