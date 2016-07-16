/**
  * @file src/app/apps/flow/misc/misc_mvrecover.c
  *
  * Implementation of Movie Recover application
  *
  * History:
  *    2014/01/13 - [Martin Lai] created file
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

#include "misc_mvrecover.h"

/*************************************************************************
 * Declarations (static)
 ************************************************************************/
/* App structure interfaces APIs */
static int app_misc_mvrecover_start(void);
static int app_misc_mvrecover_stop(void);
static int app_misc_mvrecover_on_message(UINT32 msg, UINT32 param1, UINT32 param2);

APP_APP_s app_misc_mvrecover = {
    0,  //Id
    1,  //Tier
    0,  //Parent
    0,  //Previous
    0,  //Child
    0,  //GFlags
    0,  //Flags
    app_misc_mvrecover_start,//start()
    app_misc_mvrecover_stop,    //stop()
    app_misc_mvrecover_on_message  //OnMessage()
};

/* App status */
MISC_MVRECOVER_s misc_mvrecover = {0};

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
static int app_misc_mvrecover_on_message(UINT32 msg, UINT32 param1, UINT32 param2)
{
    int ReturnValue = 0;
    ReturnValue = AppWidget_OnMessage(msg, param1, param2);
    if (ReturnValue != WIDGET_PASSED_MSG) {
        return ReturnValue;
    }

    switch (msg) {
    case AMSG_CMD_APP_READY:
        DBGMSG("[app_misc_mvrecover] Received AMSG_CMD_APP_READY");
        misc_mvrecover.Func(MISC_MVRECOVER_APP_READY, param1, param2);
        break;
    case HMSG_EDTMGR_SUCCESS:
#ifdef CONFIG_APP_ARD
        if (APP_CHECKFLAGS(app_misc_mvrecover.Flags, MISC_MVRECOVER_THM_RECOVER)) {
            APP_REMOVEFLAGS(app_misc_mvrecover.Flags, MISC_MVRECOVER_THM_RECOVER);
            misc_mvrecover.Func(MISC_MVRECOVER_OP_SUCCESS, 0, 0);
        }else{
            misc_mvrecover.Func(MISC_MVRECOVER_OP_SUCCESS, 1, 0);        
        }
#else    
        misc_mvrecover.Func(MISC_MVRECOVER_OP_SUCCESS, 1, 0);
#endif        
        break;

    case HMSG_EDTMGR_FAIL:
        misc_mvrecover.Func(MISC_MVRECOVER_OP_FAILED, 0, 0);
        break;

    case AMSG_STATE_CARD_REMOVED:
        misc_mvrecover.Func(MISC_MVRECOVER_CARD_REMOVED, param1, param2);
        break;

    case AMSG_ERROR_CARD_REMOVED:
        misc_mvrecover.Func(MISC_MVRECOVER_CARD_ERROR_REMOVED, param1, param2);
        break;

    case HMSG_USER_MODE_BUTTON:
    case HMSG_USER_IR_MODE_BUTTON:
    case AMSG_CMD_SWITCH_APP:
        misc_mvrecover.Func(MISC_MVRECOVER_SWITCH_APP, param1, param2);
        break;

    case HMSG_HDMI_INSERT_SET:
    case HMSG_HDMI_INSERT_CLR:
    case HMSG_CS_INSERT_SET:
    case HMSG_CS_INSERT_CLR:
        if (!APP_CHECKFLAGS(app_misc_mvrecover.GFlags, APP_AFLAGS_READY)) {
            AmbaPrint("[app_misc_mvrecover] System is not ready. Jack event will be handled later");
        } else {
            misc_mvrecover.Func(MISC_MVRECOVER_UPDATE_FCHAN_VOUT, msg, 0);
        }
        break;

    case HMSG_LINEIN_IN_SET:
    case HMSG_LINEIN_IN_CLR:
        if (!APP_CHECKFLAGS(app_misc_mvrecover.GFlags, APP_AFLAGS_READY)) {
            AmbaPrint("[app_misc_mvrecover] System is not ready. Jack event will be handled later");
        } else {
            misc_mvrecover.Func(MISC_MVRECOVER_AUDIO_INPUT, param1, param2);
        }
        break;

    case HMSG_HP_IN_SET:
    case HMSG_HP_IN_CLR:
    case HMSG_LINEOUT_IN_SET:
    case HMSG_LINEOUT_IN_CLR:
        if (!APP_CHECKFLAGS(app_misc_mvrecover.GFlags, APP_AFLAGS_READY)) {
            AmbaPrint("[app_misc_mvrecover] System is not ready. Jack event will be handled later");
        } else {
            misc_mvrecover.Func(MISC_MVRECOVER_AUDIO_OUTPUT, param1, param2);
        }
        break;

    case AMSG_CMD_USB_APP_START:
    case HMSG_USB_DETECT_CONNECT:
        if (APP_CHECKFLAGS(app_misc_mvrecover.GFlags, APP_AFLAGS_READY) &&
            !APP_CHECKFLAGS(app_misc_mvrecover.GFlags, APP_AFLAGS_BUSY)) {
            misc_mvrecover.Func(MISC_MVRECOVER_USB_CONNECT, param1, param2);
        }
        break;
        
#ifdef CONFIG_APP_ARD        
#if defined(CONFIG_APP_AMBA_LINK)
    case AMSG_EVENT_BOSS_BOOTED:
    	AmbaKAL_TaskSleep(500);
        AppLibComSvcHcmgr_SendMsg(AMSG_CMD_APP_READY, 0, 0);    	
        break;
#endif        
#endif
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
static int app_misc_mvrecover_start(void)
{
    int ReturnValue = 0;
    /* Set app function and operate sets */
    misc_mvrecover.Func = misc_mvrecover_func;
    misc_mvrecover.Gui = gui_misc_mvrecover_func;

    if (!APP_CHECKFLAGS(app_misc_mvrecover.GFlags, APP_AFLAGS_INIT)) {
        APP_ADDFLAGS(app_misc_mvrecover.GFlags, APP_AFLAGS_INIT);
        misc_mvrecover.Func(MISC_MVRECOVER_INIT, 0, 0);
    }

    if (APP_CHECKFLAGS(app_misc_mvrecover.GFlags, APP_AFLAGS_START)) {
        misc_mvrecover.Func(MISC_MVRECOVER_START_FLG_ON, 0, 0);
    } else {
        APP_ADDFLAGS(app_misc_mvrecover.GFlags, APP_AFLAGS_START);
        misc_mvrecover.Func(MISC_MVRECOVER_SET_APP_ENV, 0, 0);

        misc_mvrecover.Func(MISC_MVRECOVER_START, 0, 0);

        misc_mvrecover.Func(MISC_MVRECOVER_CHANGE_DISPLAY, 0, 0);
#ifdef CONFIG_APP_ARD        
#if defined(CONFIG_APP_AMBA_LINK)
	if (AppLibNetBase_GetBootStatus() == 1) {
	        AppLibComSvcHcmgr_SendMsg(AMSG_CMD_APP_READY, 0, 0);
	}	
#else
        AppLibComSvcHcmgr_SendMsg(AMSG_CMD_APP_READY, 0, 0);
#endif
#else
        AppLibComSvcHcmgr_SendMsg(AMSG_CMD_APP_READY, 0, 0);
#endif
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
static int app_misc_mvrecover_stop(void)
{
    int ReturnValue = 0;

    misc_mvrecover.Func(MISC_MVRECOVER_STOP, 0, 0);

    return ReturnValue;
}
