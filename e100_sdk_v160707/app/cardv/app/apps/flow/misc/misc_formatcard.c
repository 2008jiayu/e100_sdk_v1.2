/**
  * @file src/app/apps/flow/misc/misc_formatcard.c
  *
  * Implementation of Format Card application
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

#include "misc_formatcard.h"

/*************************************************************************
 * Declarations (static)
 ************************************************************************/
/* App structure interfaces APIs */
static int app_misc_formatcard_start(void);
static int app_misc_formatcard_stop(void);
static int app_misc_formatcard_on_message(UINT32 msg, UINT32 param1, UINT32 param2);

APP_APP_s app_misc_formatcard = {
    0,  //Id
    2,  //Tier
    0,  //Parent
    0,  //Previous
    0,  //Child
    APP_AFLAGS_OVERLAP,  //GFlags
    0,  //Flags
    app_misc_formatcard_start,//start()
    app_misc_formatcard_stop,    //stop()
    app_misc_formatcard_on_message  //OnMessage()
};

/* App status */
MISC_FORMATCARD_s misc_formatcard = {0};

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
static int app_misc_formatcard_on_message(UINT32 msg, UINT32 param1, UINT32 param2)
{
    int ReturnValue = 0;
    APP_APP_s *parent_app;

    ReturnValue = AppWidget_OnMessage(msg, param1, param2);
    if (ReturnValue != WIDGET_PASSED_MSG) {
        return ReturnValue;
    }
    switch (msg) {
    case ASYNC_MGR_MSG_CARD_FORMAT_DONE:
        DBGMSG("[app_misc_formatcard] Received ASYNC_MGR_MSG_CARD_FORMAT_DONE");
        ReturnValue = misc_formatcard.Func(MISC_FORMATCARD_OP_DONE, param1, param2);
        break;
        
    case AMSG_STATE_CARD_REMOVED:
    case AMSG_ERROR_CARD_REMOVED:
        DBGMSG("[app_misc_formatcard] Received AMSG_STATE_CARD_REMOVED");
        if (APP_CHECKFLAGS(app_misc_formatcard.GFlags, APP_AFLAGS_READY)) {
            ReturnValue = AppAppMgt_GetApp(app_misc_formatcard.Parent,&parent_app);
            ReturnValue = parent_app->OnMessage(msg, param1, param2);
        }
        ReturnValue = misc_formatcard.Func(MISC_FORMATCARD_CARD_REMOVED, param1, param2);
        break;

    case AMSG_STATE_WIDGET_CLOSED:
        ReturnValue = misc_formatcard.Func(MISC_FORMATCARD_STATE_WIDGET_CLOSED, param1, param2);
        break;
    case HMSG_USER_ZOOM_IN_BUTTON:
    case HMSG_USER_IR_ZOOM_IN_BUTTON:
    case HMSG_USER_ZOOM_OUT_BUTTON:
    case HMSG_USER_IR_ZOOM_OUT_BUTTON:
    case HMSG_USER_ZOOM_IN_BUTTON_CLR:
    case HMSG_USER_IR_ZOOM_IN_BUTTON_CLR:
    case HMSG_USER_ZOOM_OUT_BUTTON_CLR:
    case HMSG_USER_IR_ZOOM_OUT_BUTTON_CLR:
    case HMSG_USER_UP_BUTTON:
    case HMSG_USER_IR_UP_BUTTON:
    case HMSG_USER_DOWN_BUTTON:
    case HMSG_USER_IR_DOWN_BUTTON:
    case HMSG_USER_LEFT_BUTTON:
    case HMSG_USER_IR_LEFT_BUTTON:
    case HMSG_USER_RIGHT_BUTTON:
    case HMSG_USER_IR_RIGHT_BUTTON:
    case HMSG_USER_SET_BUTTON:
    case HMSG_USER_IR_SET_BUTTON:
    case HMSG_USER_MENU_BUTTON:
    case HMSG_USER_IR_MENU_BUTTON:
    case HMSG_USER_DEL_BUTTON:
    case HMSG_USER_IR_DEL_BUTTON:
        break;

    case HMSG_USER_SNAP1_BUTTON:
    case HMSG_USER_IR_SNAP1_BUTTON:
    case HMSG_USER_SNAP1_BUTTON_CLR:
    case HMSG_USER_IR_SNAP1_BUTTON_CLR:
    case HMSG_USER_SNAP2_BUTTON:
    case HMSG_USER_IR_SNAP2_BUTTON:
    case HMSG_USER_SNAP2_BUTTON_CLR:
    case HMSG_USER_IR_SNAP2_BUTTON_CLR:
    case HMSG_USER_RECORD_BUTTON:
    case HMSG_USER_IR_RECORD_BUTTON:
    case HMSG_USER_MODE_BUTTON:
    case HMSG_USER_IR_MODE_BUTTON:
    case HMSG_USER_POWER_BUTTON:
        if (APP_CHECKFLAGS(app_misc_formatcard.Flags, MISC_FORMATCARD_DO_FORMAT_CARD)) {
            break;
        }

    default:
        ReturnValue = AppAppMgt_GetApp(app_misc_formatcard.Parent,&parent_app);
        ReturnValue = parent_app->OnMessage(msg, param1, param2);
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
static int app_misc_formatcard_start(void)
    {
    int ReturnValue = 0;

    /* Set app function and operate sets */
    misc_formatcard.Func = misc_formatcard_func;
    misc_formatcard.Gui = gui_misc_formatcard_func;

    if (!APP_CHECKFLAGS(app_misc_formatcard.GFlags, APP_AFLAGS_INIT)) {
        APP_ADDFLAGS(app_misc_formatcard.GFlags, APP_AFLAGS_INIT);
        misc_formatcard.Func(MISC_FORMATCARD_INIT, 0, 0);
    }

    if (APP_CHECKFLAGS(app_misc_formatcard.GFlags, APP_AFLAGS_START)) {
        misc_formatcard.Func(MISC_FORMATCARD_START_FLG_ON, 0, 0);
    } else {
        APP_ADDFLAGS(app_misc_formatcard.GFlags, APP_AFLAGS_START);
        if (!APP_CHECKFLAGS(app_misc_formatcard.GFlags, APP_AFLAGS_READY)) {
            APP_ADDFLAGS(app_misc_formatcard.GFlags, APP_AFLAGS_READY);
        }

        misc_formatcard.Func(MISC_FORMATCARD_START, 0, 0);
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
static int app_misc_formatcard_stop(void)
{
    int ReturnValue = 0;

    misc_formatcard.Func(MISC_FORMATCARD_STOP, 0, 0);

    return ReturnValue;
}
