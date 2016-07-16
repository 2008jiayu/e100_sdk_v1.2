/**
  * @file src/app/apps/flow/pb/pb_photo.c
  *
  * Implementation of photo playback application
  *
  * History:
  *    2013/11/08 - [Martin Lai] created file
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

#include <apps/flow/pb/pb_photo.h>

/*************************************************************************
 * Declarations (static)
 ************************************************************************/
/* App structure interfaces APIs */
static int app_pb_photo_start(void);
static int app_pb_photo_stop(void);
static int app_pb_photo_on_message(UINT32 msg, UINT32 param1, UINT32 param2);

APP_APP_s app_pb_photo = {
    0,  //Id
    1,  //Tier
    0,  //Parent
    0,  //Previous
    0,  //Child
    0,  //GFlags
    0,  //Flags
    app_pb_photo_start,//start()
    app_pb_photo_stop,    //stop()
    app_pb_photo_on_message  //OnMessage()
};

/* App status */
PB_PHOTO_s pb_photo = {0};

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
static int app_pb_photo_on_message(UINT32 msg, UINT32 param1, UINT32 param2)
{
    int ReturnValue = 0;
    ReturnValue = AppWidget_OnMessage(msg, param1, param2);
    if (ReturnValue != WIDGET_PASSED_MSG) {
        return ReturnValue;
    }
    switch (msg) {
    case AMSG_CMD_APP_READY:
        DBGMSG("[app_pb_photo] Received AMSG_CMD_APP_READY");
        ReturnValue = pb_photo.Func(PB_PHOTO_APP_READY, 0, 0);
        break;
    case HMSG_USER_SNAP1_BUTTON:
    case HMSG_USER_IR_SNAP1_BUTTON:
        if (!APP_CHECKFLAGS(app_pb_photo.GFlags, APP_AFLAGS_READY)) {
            break;    // not ready
        }
        ReturnValue = pb_photo.Op->ButtonFocus();
        break;
    case HMSG_USER_SNAP1_BUTTON_CLR:
    case HMSG_USER_IR_SNAP1_BUTTON_CLR:
        if (!APP_CHECKFLAGS(app_pb_photo.GFlags, APP_AFLAGS_READY)) {
            break;    // not ready
        }
        ReturnValue = pb_photo.Op->ButtonFocusClr();
        break;
    case HMSG_USER_SNAP2_BUTTON:
    case HMSG_USER_IR_SNAP2_BUTTON:
        if (!APP_CHECKFLAGS(app_pb_photo.GFlags, APP_AFLAGS_READY)) {
            break;    // not ready
        }
        ReturnValue = pb_photo.Op->ButtonShutter();
        break;
    case HMSG_USER_SNAP2_BUTTON_CLR:
    case HMSG_USER_IR_SNAP2_BUTTON_CLR:
        if (!APP_CHECKFLAGS(app_pb_photo.GFlags, APP_AFLAGS_READY)) {
            break;    // not ready
        }
        ReturnValue = pb_photo.Op->ButtonShutterClr();
        break;
    case HMSG_USER_RECORD_BUTTON:
    case HMSG_USER_IR_RECORD_BUTTON:
        if (!APP_CHECKFLAGS(app_pb_photo.GFlags, APP_AFLAGS_READY)) {
            break;    // not ready
        }
        ReturnValue = pb_photo.Op->ButtonRecord();
        break;
    case HMSG_USER_ZOOM_IN_BUTTON:
    case HMSG_USER_IR_ZOOM_IN_BUTTON:
        if (!APP_CHECKFLAGS(app_pb_photo.GFlags, APP_AFLAGS_READY)) {
            break;    // not ready
        }
        ReturnValue = pb_photo.Op->ButtonZoomIn();
        break;
    case HMSG_USER_ZOOM_OUT_BUTTON:
    case HMSG_USER_IR_ZOOM_OUT_BUTTON:
        if (!APP_CHECKFLAGS(app_pb_photo.GFlags, APP_AFLAGS_READY)) {
            break;    // not ready
        }
        ReturnValue = pb_photo.Op->ButtonZoomOut();
        break;
    case HMSG_USER_ZOOM_IN_BUTTON_CLR:
    case HMSG_USER_IR_ZOOM_IN_BUTTON_CLR:
        if (!APP_CHECKFLAGS(app_pb_photo.GFlags, APP_AFLAGS_READY)) {
            break;    // not ready
        }
        ReturnValue = pb_photo.Op->ButtonZoomInClr();
        break;
    case HMSG_USER_ZOOM_OUT_BUTTON_CLR:
    case HMSG_USER_IR_ZOOM_OUT_BUTTON_CLR:
        if (!APP_CHECKFLAGS(app_pb_photo.GFlags, APP_AFLAGS_READY)) {
            break;    // not ready
        }
        ReturnValue = pb_photo.Op->ButtonZoomOutClr();
        break;
    case HMSG_USER_UP_BUTTON:
    case HMSG_USER_IR_UP_BUTTON:
        if (!APP_CHECKFLAGS(app_pb_photo.GFlags, APP_AFLAGS_READY)) {
            break;    // not ready
        }
        ReturnValue = pb_photo.Op->ButtonUp();
        break;
    case HMSG_USER_DOWN_BUTTON:
    case HMSG_USER_IR_DOWN_BUTTON:
        if (!APP_CHECKFLAGS(app_pb_photo.GFlags, APP_AFLAGS_READY)) {
            break;    // not ready
        }
        ReturnValue = pb_photo.Op->ButtonDown();
        break;
    case HMSG_USER_LEFT_BUTTON:
    case HMSG_USER_IR_LEFT_BUTTON:
        if (!APP_CHECKFLAGS(app_pb_photo.GFlags, APP_AFLAGS_READY)) {
            break;    // not ready
        }
        ReturnValue = pb_photo.Op->ButtonLeft();
        break;
    case HMSG_USER_RIGHT_BUTTON:
    case HMSG_USER_IR_RIGHT_BUTTON:
        if (!APP_CHECKFLAGS(app_pb_photo.GFlags, APP_AFLAGS_READY)) {
            break;    // not ready
        }
        ReturnValue = pb_photo.Op->ButtonRight();
        break;
    case HMSG_USER_SET_BUTTON:
    case HMSG_USER_IR_SET_BUTTON:
        if (!APP_CHECKFLAGS(app_pb_photo.GFlags, APP_AFLAGS_READY)) {
            break;    // not ready
        }
        ReturnValue = pb_photo.Op->ButtonSet();
        break;
    case HMSG_USER_MENU_BUTTON:
    case HMSG_USER_IR_MENU_BUTTON:
        if (!APP_CHECKFLAGS(app_pb_photo.GFlags, APP_AFLAGS_READY)) {
            break;    // not ready
        }
        ReturnValue = pb_photo.Op->ButtonMenu();
        break;
    case HMSG_USER_MODE_BUTTON:
    case HMSG_USER_IR_MODE_BUTTON:
        ReturnValue = pb_photo.Op->ButtonMode();
        break;
    case HMSG_USER_DEL_BUTTON:
    case HMSG_USER_IR_DEL_BUTTON:
        if (!APP_CHECKFLAGS(app_pb_photo.GFlags, APP_AFLAGS_READY)) {
            break;    // not ready
        }
        ReturnValue = pb_photo.Op->ButtonDel();
        break;
    case HMSG_USER_POWER_BUTTON:
        if (!APP_CHECKFLAGS(app_pb_photo.GFlags, APP_AFLAGS_READY)) {
            break;    // not ready
        }
        ReturnValue = pb_photo.Op->ButtonPower();
        break;
    case AMSG_ERROR_CARD_REMOVED:
        if (APP_CHECKFLAGS(app_pb_photo.GFlags, APP_AFLAGS_READY)) {
            ReturnValue = pb_photo.Func(PB_PHOTO_CARD_ERROR_REMOVED, param1, param2);
        }
        break;
    case AMSG_STATE_CARD_REMOVED:
        if (APP_CHECKFLAGS(app_pb_photo.GFlags, APP_AFLAGS_READY)) {
            ReturnValue = pb_photo.Func(PB_PHOTO_CARD_REMOVED, param1, param2);
        }
        break;
    case HMSG_STORAGE_IDLE:
        if (APP_CHECKFLAGS(app_pb_photo.GFlags, APP_AFLAGS_READY)) {
            ReturnValue = pb_photo.Func(PB_PHOTO_CARD_STORAGE_IDLE, param1, param2);
        }
        break;
    case AMSG_CMD_CARD_UPDATE_ACTIVE_CARD:
        ReturnValue = pb_photo.Func(PB_PHOTO_CARD_NEW_INSERT, param1, param2);
        break;
    case AMSG_CMD_SWITCH_APP:
        break;
    case HMSG_HDMI_INSERT_SET:
    case HMSG_HDMI_INSERT_CLR:
    case HMSG_CS_INSERT_SET:
    case HMSG_CS_INSERT_CLR:
        if (!APP_CHECKFLAGS(app_pb_photo.GFlags, APP_AFLAGS_READY)) {
            AmbaPrint("[app_pb_photo] System is not ready. Jack event will be handled later");
        } else {
            ReturnValue = pb_photo.Func(PB_PHOTO_UPDATE_FCHAN_VOUT, msg, 0);
        }
        break;
    case HMSG_LINEIN_IN_SET:
    case HMSG_LINEIN_IN_CLR:
        if (!APP_CHECKFLAGS(app_pb_photo.GFlags, APP_AFLAGS_READY)) {
            AmbaPrint("[app_pb_photo] System is not ready. Jack event will be handled later");
        } else {
            ReturnValue = pb_photo.Func(PB_PHOTO_AUDIO_INPUT, 0, 0);
        }
        break;
    case HMSG_HP_IN_SET:
    case HMSG_HP_IN_CLR:
    case HMSG_LINEOUT_IN_SET:
    case HMSG_LINEOUT_IN_CLR:
        if (!APP_CHECKFLAGS(app_pb_photo.GFlags, APP_AFLAGS_READY)) {
            AmbaPrint("[app_pb_photo] System is not ready. Jack event will be handled later");
        } else {
            ReturnValue = pb_photo.Func(PB_PHOTO_AUDIO_OUTPUT, 0, 0);
        }
        break;
    case AMSG_CMD_USB_APP_START:
    case HMSG_USB_DETECT_CONNECT:
        if (APP_CHECKFLAGS(app_pb_photo.GFlags, APP_AFLAGS_READY) &&
            !APP_CHECKFLAGS(app_pb_photo.GFlags, APP_AFLAGS_BUSY)) {
            ReturnValue = pb_photo.Func(PB_PHOTO_USB_CONNECT, 0, 0);
        }
        break;
    case AMSG_CMD_STOP_PLAYING:
        ReturnValue = pb_photo.Func(PB_PHOTO_STOP_PLAYING, param1, param2);
        break;
    case AMSG_STATE_WIDGET_CLOSED:
        ReturnValue = pb_photo.Func(PB_PHOTO_STATE_WIDGET_CLOSED, 0, 0);
        break;
    case ASYNC_MGR_MSG_DMF_FDEL_DONE:
        DBGMSG("[app_pb_photo] Received ASYNC_MGR_MSG_DMF_FDEL_DONE");
        ReturnValue = pb_photo.Func(PB_PHOTO_DELETE_FILE_COMPLETE, param1, param2);
        break;
    default:
        break;
    }

    return ReturnValue;
}

/**
 *  @brief The start flow of application.
 *
 *  The start flow of application.
 *
 *  @return >=0 success, <0 failure
 */
static int app_pb_photo_start(void)
    {
    int ReturnValue = 0;

    /* Set app function and operate sets */
    pb_photo.Func = pb_photo_func;
    pb_photo.Gui = gui_pb_photo_func;
    pb_photo.Op = &pb_photo_op;

    if (!APP_CHECKFLAGS(app_pb_photo.GFlags, APP_AFLAGS_INIT)) {
        APP_ADDFLAGS(app_pb_photo.GFlags, APP_AFLAGS_INIT);
        pb_photo.Func(PB_PHOTO_INIT, 0, 0);
    }

    if (APP_CHECKFLAGS(app_pb_photo.GFlags, APP_AFLAGS_START)) {
        pb_photo.Func(PB_PHOTO_START_FLG_ON, 0, 0);
    } else {
        APP_ADDFLAGS(app_pb_photo.GFlags, APP_AFLAGS_START);
        pb_photo.Func(PB_PHOTO_SET_APP_ENV, 0, 0);

        pb_photo.Func(PB_PHOTO_START, 0, 0);

        pb_photo.Func(PB_PHOTO_CHANGE_DISPLAY, 0, 0);
        AppLibComSvcHcmgr_SendMsg(AMSG_CMD_APP_READY, 0, 0);
    }

    return ReturnValue;
}

/**
 *  @brief The stop flow of application.
 *
 *  The stop flow of application.
 *
 *  @return >=0 success, <0 failure
 */
static int app_pb_photo_stop(void)
{
    int ReturnValue = 0;

    if (!APP_CHECKFLAGS(app_pb_photo.GFlags, APP_AFLAGS_READY)) {
        pb_photo.Func(PB_PHOTO_STOP, 0, 0);
    }

    return ReturnValue;
}
