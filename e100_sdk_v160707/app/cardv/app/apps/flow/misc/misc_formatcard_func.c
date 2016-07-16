/**
  * @file src/app/apps/flow/misc/connectedcam/misc_formatcard_func.c
  *
  *  Functions of Format Card application
  *
  * History:
  *    2014/01/14 - [Martin Lai] created file
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


#include <apps/flow/misc/misc_formatcard.h>
#include <system/status.h>
#include <system/app_pref.h>
#include <system/app_util.h>
#include <apps/gui/resource/gui_settle.h>
#include <apps/flow/widget/menu/widget.h>

extern void AppUtil_Show_Data_Timer_Monitor(void);

static int misc_formatcard_start(void)
{
    int ReturnValue = 0;

    misc_formatcard.Func(MISC_FORMATCARD_DIALOG_SHOW_FORMATCARD, 0, 0);

    return ReturnValue;
}

static int misc_formatcard_stop(void)
{
    int ReturnValue = 0;

    misc_formatcard.Gui(GUI_APP_ICON_HIDE, 0, 0);
    misc_formatcard.Gui(GUI_FLUSH, 0, 0);

    return ReturnValue;
}

static int formatcard_dialog_ok_handler(UINT32 sel, UINT32 param1, UINT32 param2)
{
    int ReturnValue = 0;

    APP_REMOVEFLAGS(app_misc_formatcard.Flags, MISC_FORMATCARD_DO_FORMAT_CARD);
    switch (sel) {
    default:
    case DIALOG_SEL_OK:
        /*The process of formatting card has done.*/
        /* To excute the functions that system block them when the Busy flag is enabled. */
        AppUtil_BusyCheck(0);
        if (APP_CHECKFLAGS(app_misc_formatcard.GFlags, APP_AFLAGS_READY)) {
            extern void AppUtil_Gps_Status_Monitor(void);
#ifndef CONFIG_ECL_GUI            
            AppUtil_Gps_Detect_Monitor();
            AppUtil_Gps_Status_Monitor();
#endif
            /* The system could switch the current app to other in the function "app_util_check_busy_blocked". */
            misc_formatcard.Func(MISC_FORMATCARD_SWITCH_APP, 1, 0);
        }
        break;
    }

    return ReturnValue;
}

static int misc_formatcard_op_done(int param1, int param2)
{
    int ReturnValue = 0;

#ifdef CONFIG_APP_ARD
    AppUtil_BatteryVoltageMonitor();
#endif

    if (param1 < 0) {
        misc_formatcard.Func(MISC_FORMATCARD_OP_FAILED, 0, 0);
    } else {
        misc_formatcard.Func(MISC_FORMATCARD_OP_SUCCESS, 0, 0);
    }

    return ReturnValue;
}

static int misc_formatcard_op_success(int param1, int param2)
{
    int ReturnValue = 0;

    /** if active slot is formated, update root status by set root */
    char drive = 'A';
    drive += AppLibCard_GetActiveSlot();
    ReturnValue = AppLibStorageDmf_Refresh(drive);

    if (ReturnValue < 0) {
        AmbaPrintColor(RED, "<misc_formatcard_op_success> refresh dmf error!");
    }

    /** update card_status.format*/
    AppLibCard_CheckStatus(CARD_CHECK_RESET);
#ifdef CONFIG_APP_ARD
    // delay for show the processing
    AmbaKAL_TaskSleep(500);
#endif
    misc_formatcard.Func(MISC_FORMATCARD_WARNING_MSG_SHOW_STOP, 0, 0);
    ///** Regsister the auto poweroff check timer*/
    //app_util_auto_poweroff_init(0);
    APP_REMOVEFLAGS(app_misc_formatcard.GFlags, APP_AFLAGS_BUSY);
#ifdef CONFIG_APP_ARD
    AppLibGraph_Hide(GRAPH_CH_DUAL, GOBJ_EVENT_NUM_STR);
#endif
    ReturnValue = AppDialog_SetDialog(DIALOG_TYPE_OK, DIALOG_SUB_FORMAT_CARD_OK, formatcard_dialog_ok_handler);
    ReturnValue = AppWidget_On(WIDGET_DIALOG, 0);
    APP_ADDFLAGS(app_misc_formatcard.GFlags, APP_AFLAGS_POPUP);
    APP_REMOVEFLAGS(app_misc_formatcard.Flags, MISC_FORMATCARD_DO_FORMAT_CARD);
    return ReturnValue;
}


static int misc_formatcard_op_failed(int param1, int param2)
{
    int ReturnValue = 0;

    misc_formatcard.Func(MISC_FORMATCARD_WARNING_MSG_SHOW_STOP, 0, 0);
    AmbaPrintColor(RED,"[app_misc_formatcard] Format card failed: %d", param2);
    /** update card_status.format*/
    ReturnValue = AppLibCard_CheckStatus(CARD_CHECK_RESET);

    ///** Regsister the auto poweroff check timer*/
    //app_util_auto_poweroff_init(0);
    APP_REMOVEFLAGS(app_misc_formatcard.GFlags, APP_AFLAGS_BUSY);
    ReturnValue = AppDialog_SetDialog(DIALOG_TYPE_OK, DIALOG_SUB_FORMAT_CARD_FAILED, formatcard_dialog_ok_handler);
    ReturnValue = AppWidget_On(WIDGET_DIALOG, 0);
    APP_ADDFLAGS(app_misc_formatcard.GFlags, APP_AFLAGS_POPUP);

    return ReturnValue;
}

static int misc_formatcard_card_removed(void)
{
    int ReturnValue = 0;

    misc_formatcard.Func(MISC_FORMATCARD_WARNING_MSG_SHOW_STOP, 0, 0);
    if (APP_CHECKFLAGS(app_misc_formatcard.GFlags, APP_AFLAGS_READY)) {
        APP_REMOVEFLAGS(app_misc_formatcard.GFlags, APP_AFLAGS_BUSY);
        if (APP_CHECKFLAGS(app_misc_formatcard.Flags, MISC_FORMATCARD_DO_FORMAT_CARD) == 0) {
            AppUtil_SwitchApp(AppUtil_GetStartApp(0));
        }
    }

    return ReturnValue;
}

static int formatcard_dialog_format_caution_handler(UINT32 sel, UINT32 param1, UINT32 param2)
{
    int ReturnValue = 0;

#ifdef CONFIG_APP_ARD
    ReturnValue = AppLibCard_CheckStatus(CARD_CHECK_DELETE);
    if ((sel == DIALOG_SEL_YES)
        && ((ReturnValue == CARD_STATUS_NO_CARD) || (ReturnValue == CARD_STATUS_WP_CARD))) {
        AmbaPrintColor(RED,"[app_misc_formatcard] <%s> WARNING_CARD_PROTECTED or CARD_STATUS_NO_CARD, err = %d", __FUNCTION__, ReturnValue);
        sel = DIALOG_SEL_NO;    // to run cancel process below
    }
#endif

    switch (sel) {
    case DIALOG_SEL_YES:
        APP_ADDFLAGS(app_misc_formatcard.GFlags, APP_AFLAGS_BUSY);
        APP_ADDFLAGS(app_misc_formatcard.Flags, MISC_FORMATCARD_DO_FORMAT_CARD);
        misc_formatcard.Func(MISC_FORMATCARD_WARNING_MSG_SHOW_START, GUI_WARNING_PROCESSING, 1);
        /** Unregister all timers: slide show, self timer...     */
        AppLibComSvcTimer_UnregisterAll();
#if 0
        /** Stop the player */
        if ((UserSetting->SystemPref.SystemMode == APP_MODE_DEC) ||
        ((UserSetting->SystemPref.SystemMode == APP_MODE_ENC_PHOTO) && (app_status.previous_mode == PREVIOUS_PHOTO_ENCODE_MODE))|| ///< Photo mode quickplay.
        ((UserSetting->SystemPref.SystemMode == APP_MODE_ENC_VIDEO) && (app_status.previous_mode == PREVIOUS_VIDEO_ENCODE_MODE))) {  ///< Video mode quickplay.
            APP_APP_s *parent_app;

            AppAppMgt_GetCurApp(app_misc_formatcard.parent, &parent_app);
            ReturnValue = parent_app->OnMessage(AMSG_CMD_STOP_PLAYING, 0, 0);
        }
#endif
        ReturnValue = AppLibCard_GetActiveSlot();
        ReturnValue = AppLibComSvcAsyncOp_CardFormat(ReturnValue);
        break;
    default:
        case DIALOG_SEL_NO:
        /** To give up formatting card.*/
        misc_formatcard.Func(MISC_FORMATCARD_SWITCH_APP, 0, 0);
        break;
    }

    return ReturnValue;
}


static int formatcard_dialog_format_handler(UINT32 sel, UINT32 param1, UINT32 param2)
{
    int ReturnValue = 0;

    switch (sel) {
    case DIALOG_SEL_YES:
        //ReturnValue = AppDialog_SetDialog(DIALOG_TYPE_Y_N, DIALOG_SUB_FORMAT_CARD_CAUTION, formatcard_dialog_format_caution_handler);
        //ReturnValue = AppWidget_On(WIDGET_DIALOG, 0);
        //APP_ADDFLAGS(app_misc_formatcard.GFlags, APP_AFLAGS_POPUP);
        AppLibComSvcTimer_UnregisterAll();
        AppUtil_Show_Data_Timer_Monitor();
        ReturnValue = AppLibCard_GetActiveSlot();
        ReturnValue = AppLibComSvcAsyncOp_CardFormat(ReturnValue);        
        break;
    case DIALOG_SEL_NO:
    default:
        /** To give up formatting card.*/
        misc_formatcard.Func(MISC_FORMATCARD_SWITCH_APP, 0, 0);
        break;
    }

    return ReturnValue;
}

static int misc_formatcard_dialog_show_formatcard(void)
{
    int ReturnValue = 0;

    ReturnValue = AppLibCard_CheckStatus(CARD_CHECK_DELETE);

    if ((ReturnValue == 0) || (ReturnValue == CARD_STATUS_UNFORMAT_CARD) || (ReturnValue == CARD_STATUS_INVALID_CARD) || (ReturnValue == CARD_STATUS_INVALID_FORMAT_CARD) ||
    (ReturnValue == CARD_STATUS_NOT_ENOUGH_SPACE)) {
        if (app_status.CardFmtParam) {
            app_status.CardFmtParam = 0;
            ReturnValue = AppDialog_SetDialog(DIALOG_TYPE_Y_N, DIALOG_SUB_FORMAT_CARD_NO_OPTIMUM, formatcard_dialog_format_handler);
        } else {
            ReturnValue = AppDialog_SetDialog(DIALOG_TYPE_Y_N, DIALOG_SUB_FORMAT_CARD_YES_OR_NO, formatcard_dialog_format_handler);
        }
        AmbaPrintColor(GREEN,"[app_misc_formatcard] CARD_OK");
        ReturnValue = AppWidget_On(WIDGET_DIALOG, 0);
        APP_ADDFLAGS(app_misc_formatcard.GFlags, APP_AFLAGS_POPUP);
    } else {
        if (ReturnValue == CARD_STATUS_NO_CARD) {
            AmbaPrintColor(RED,"[app_misc_formatcard] WARNING_NO_CARD");
        } else if (ReturnValue == CARD_STATUS_WP_CARD) {
            misc_formatcard.Func(MISC_FORMATCARD_WARNING_MSG_SHOW_START, GUI_WARNING_CARD_PROTECTED, 0);
            AmbaPrintColor(RED,"[app_misc_formatcard] WARNING_CARD_PROTECTED");
        } else {
            AmbaPrintColor(RED,"[app_misc_formatcard] WARNING_CARD_Error %d",ReturnValue);
        }
        app_status.CardFmtParam = 0;
        /** Can not format card.*/
        misc_formatcard.Func(MISC_FORMATCARD_SWITCH_APP, 0, 0);
    }

    return ReturnValue;
}

static int misc_formatcard_widget_closed(void)
{
    int ReturnValue = 0;

    if (APP_CHECKFLAGS(app_misc_formatcard.GFlags, APP_AFLAGS_POPUP)) {
        APP_REMOVEFLAGS(app_misc_formatcard.GFlags, APP_AFLAGS_POPUP);
    }

    return ReturnValue;
}

/**
 * @brief The applications switching function.
 *
 * @return >=0 success
 *         <0 failure
 */
static int misc_formatcard_switch_app(int param)
{
    int ReturnValue = 0;

    if (param) {
        /** The process of formatting card has done.*/
        ReturnValue = AppLibDisp_GetDeviceID(DISP_CH_FCHAN);
        if ((ReturnValue != AMP_DSIP_NONE) && app_status.FchanDecModeOnly) {
            app_status.LockDecMode = 1;
            /** Switch to the thumbnail mode if fchan and app_status.FchanDecModeOnly are enabled.*/
            AppUtil_SwitchApp(APP_THUMB_MOTION);
        } else {
            app_status.LockDecMode = 0;
            AppUtil_SwitchApp(AppUtil_GetStartApp(0));
        }
    } else {
        /** Doesn't format card.*/
        if (app_misc_formatcard.Parent != APP_MAIN) {
            AppUtil_SwitchApp(app_misc_formatcard.Parent);
        } else {
            ReturnValue = AppLibDisp_GetDeviceID(DISP_CH_FCHAN);
            if ((ReturnValue != AMP_DSIP_NONE) && app_status.FchanDecModeOnly) {
                app_status.LockDecMode = 1;
                /** Switch to the thumbnail mode if fchan and app_status.FchanDecModeOnly are enabled.*/
                AppUtil_SwitchApp(APP_THUMB_MOTION);
            } else {
                app_status.LockDecMode = 0;
                AppUtil_SwitchApp(AppUtil_GetStartApp(0));
            }
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
static void misc_formatcard_warning_timer_handler(int eid)
{
    static int BlinkCount;

    if (eid == TIMER_UNREGISTER) {
        BlinkCount = 0;
        return;
    }

    BlinkCount++;

    if (BlinkCount & 0x01) {
        misc_formatcard.Gui(GUI_WARNING_HIDE, 0, 0);
    } else {
        misc_formatcard.Gui(GUI_WARNING_SHOW, 0, 0);
    }

    if (BlinkCount >= 5) {
        APP_REMOVEFLAGS(app_misc_formatcard.Flags, MISC_FORMATCARD_WARNING_MSG_RUN);
        AppLibComSvcTimer_Unregister(TIMER_2HZ, misc_formatcard_warning_timer_handler);
        misc_formatcard.Gui(GUI_WARNING_HIDE, 0, 0);
        /** Can not format card.*/
        misc_formatcard.Func(MISC_FORMATCARD_SWITCH_APP, 0, 0);
    }
    misc_formatcard.Gui(GUI_FLUSH, 0, 0);

}



static int misc_formatcard_warning_msg_show(int enable, int param1, int param2)
{
    int ReturnValue = 0;

    if (enable) {
        if (param1 == MISC_FORMATCARD_CARD_PROTECTED) {
            param1 = GUI_WARNING_CARD_PROTECTED;
        }
        if (param2) {
            misc_formatcard.Gui(GUI_WARNING_UPDATE, param1, 0);
            misc_formatcard.Gui(GUI_WARNING_SHOW, 0, 0);
        } else {
            if (!APP_CHECKFLAGS(app_misc_formatcard.Flags, MISC_FORMATCARD_WARNING_MSG_RUN)) {
                APP_ADDFLAGS(app_misc_formatcard.Flags, MISC_FORMATCARD_WARNING_MSG_RUN);
                misc_formatcard.Gui(GUI_WARNING_UPDATE, param1, 0);
                misc_formatcard.Gui(GUI_WARNING_SHOW, 0, 0);
                AppLibComSvcTimer_Register(TIMER_2HZ, misc_formatcard_warning_timer_handler);
            }
        }
    } else {
        if (APP_CHECKFLAGS(app_misc_formatcard.Flags, MISC_FORMATCARD_WARNING_MSG_RUN)) {
            APP_REMOVEFLAGS(app_misc_formatcard.Flags, MISC_FORMATCARD_WARNING_MSG_RUN);
            AppLibComSvcTimer_Unregister(TIMER_2HZ, misc_formatcard_warning_timer_handler);
        }
        misc_formatcard.Gui(GUI_WARNING_HIDE, 0, 0);
    }
    misc_formatcard.Gui(GUI_FLUSH, 0, 0);

    return ReturnValue;
}


/**
 *  @brief The functions of application
 *
 *  The functions of application
 *
 *  @param[in] funcId Function id
 *  @param[in] param1 first parameter
 *  @param[in] param2 second parameter
 *
 *  @return >=0 success, <0 failure
 */
int misc_formatcard_func(UINT32 funcId, UINT32 param1, UINT32 param2)
{
    int ReturnValue = 0;

    switch (funcId) {
    case MISC_FORMATCARD_INIT:
        break;
    case MISC_FORMATCARD_START:
        ReturnValue = misc_formatcard_start();
        break;
    case MISC_FORMATCARD_STOP:
        ReturnValue = misc_formatcard_stop();
        break;
    case MISC_FORMATCARD_OP_DONE:
        ReturnValue = misc_formatcard_op_done(param1, param2);
        break;
    case MISC_FORMATCARD_OP_SUCCESS:
        ReturnValue = misc_formatcard_op_success(param1, param2);
        break;
    case MISC_FORMATCARD_OP_FAILED:
        ReturnValue = misc_formatcard_op_failed(param1, param2);
        break;
    case MISC_FORMATCARD_SWITCH_APP:
        ReturnValue = misc_formatcard_switch_app(param1);
        break;
    case MISC_FORMATCARD_CARD_REMOVED:
        ReturnValue = misc_formatcard_card_removed();
        break;
    case MISC_FORMATCARD_DIALOG_SHOW_FORMATCARD:
        ReturnValue = misc_formatcard_dialog_show_formatcard();
        break;
    case MISC_FORMATCARD_STATE_WIDGET_CLOSED:
        ReturnValue = misc_formatcard_widget_closed();
        break;
    case MISC_FORMATCARD_WARNING_MSG_SHOW_START:
        ReturnValue = misc_formatcard_warning_msg_show( 1, param1, param2);
        break;
    case MISC_FORMATCARD_WARNING_MSG_SHOW_STOP:
        ReturnValue = misc_formatcard_warning_msg_show( 0, param1, param2);
        break;
    default:
        break;
    }

    return ReturnValue;
}
