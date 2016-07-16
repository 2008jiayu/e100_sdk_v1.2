/**
  * @file src/app/apps/flow/misc/misc_fwupdate.c
  *
  * Implementation of Firmware update application
  *
  * History:
  *    2014/03/20 - [Martin Lai] created file
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

#include "misc_fwupdate.h"

/*************************************************************************
 * Declarations (static)
 ************************************************************************/
/* App structure interfaces APIs */
static int app_misc_fwupdate_start(void);
static int app_misc_fwupdate_stop(void);
static int app_misc_fwupdate_on_message(UINT32 msg, UINT32 param1, UINT32 param2);

APP_APP_s app_misc_fwupdate = {
    0,    //id
    1,    //tier
    0,    //parent
    0,    //previous
    0,    //child
    0,    //GFlags
    0,    //flags
    app_misc_fwupdate_start,    //start()
    app_misc_fwupdate_stop,    //stop()
    app_misc_fwupdate_on_message    //on_message()
};

MISC_FWUPDATE_s misc_fwupdate = {0};

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
static int app_misc_fwupdate_on_message(UINT32 msg, UINT32 param1, UINT32 param2)
{
    int ReturnValue = 0;

    ReturnValue = AppWidget_OnMessage(msg, param1, param2);
    if (ReturnValue != WIDGET_PASSED_MSG) {
        return ReturnValue;
    }

    switch (msg) {
    case AMSG_ERROR_CARD_REMOVED:
        misc_fwupdate.Func(MISC_FWUPDATE_CARD_ERROR_REMOVED, param1, param2);
        AmbaPrint("[app_misc_fwupdate] Illegal operation: Remove card during IO/busy");
        break;
    case AMSG_STATE_CARD_REMOVED:
        misc_fwupdate.Func(MISC_FWUPDATE_CARD_REMOVED, param1, param2);
        break;
    case HMSG_HDMI_INSERT_SET:
    case HMSG_HDMI_INSERT_CLR:
    case HMSG_CS_INSERT_SET:
    case HMSG_CS_INSERT_CLR:
        if (!APP_CHECKFLAGS(app_misc_fwupdate.GFlags, APP_AFLAGS_READY)) {
            AmbaPrint("[app_misc_fwupdate] System is not ready. Jack event will be handled later");
        } else if (APP_CHECKFLAGS(app_misc_fwupdate.GFlags, APP_AFLAGS_BUSY)) {
            AmbaPrint("[app_misc_fwupdate]System is busy.  Jack event will be handled later.");
        } else {
            misc_fwupdate.Func(MISC_FWUPDATE_UPDATE_FCHAN_VOUT, msg, param1);
        }
        break;
    case HMSG_LINEIN_IN_SET:
    case HMSG_LINEIN_IN_CLR:
        if (!APP_CHECKFLAGS(app_misc_fwupdate.GFlags, APP_AFLAGS_READY)) {
            AmbaPrint("[app_misc_fwupdate] System is not ready. Audio jack event will be handled later");
        } else {
            misc_fwupdate.Func(MISC_FWUPDATE_AUDIO_INPUT, param1, param2);
        }
        break;
    case HMSG_HP_IN_SET:
    case HMSG_HP_IN_CLR:
    case HMSG_LINEOUT_IN_SET:
    case HMSG_LINEOUT_IN_CLR:
        if (!APP_CHECKFLAGS(app_misc_fwupdate.GFlags, APP_AFLAGS_READY)) {
            AmbaPrint("[app_misc_fwupdate] System is not ready. Audio jack event will be handled later");
        } else {
            misc_fwupdate.Func(MISC_FWUPDATE_AUDIO_OUTPUT, param1, param2);
        }
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
static int app_misc_fwupdate_start(void)
{
    int ReturnValue = 0;

    misc_fwupdate.Func = misc_fwupdate_func;
    misc_fwupdate.Gui = gui_misc_fwupdate_func;

    if (!APP_CHECKFLAGS(app_misc_fwupdate.GFlags, APP_AFLAGS_INIT)) {
        APP_ADDFLAGS(app_misc_fwupdate.GFlags, APP_AFLAGS_INIT);
        /** Initialize application structure */
    }

    if (!APP_CHECKFLAGS(app_misc_fwupdate.GFlags, APP_AFLAGS_START)) {
        APP_ADDFLAGS(app_misc_fwupdate.GFlags, APP_AFLAGS_START);
        APP_ADDFLAGS(app_misc_fwupdate.GFlags, APP_AFLAGS_READY);

        misc_fwupdate.Func(MISC_FWUPDATE_START, 0, 0);
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
static int app_misc_fwupdate_stop(void)
{
    int ReturnValue = 0;

    ReturnValue = misc_fwupdate.Func(MISC_FWUPDATE_STOP, 0, 0);

    return ReturnValue;
}
