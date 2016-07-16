/**
  * @file src/app/apps/flow/misc/misc_logo.c
  *
  * Implementation Logo application
  *
  * History:
  *    2014/11/27 - SuQiang created file
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

#include "misc_logo.h"
#include <system/status.h>
#include "../../../system/app_util.h"
#include "../../../system/app_pref.h"


/*************************************************************************
 * Declarations (static)
 ************************************************************************/
/* App structure interfaces APIs */
static int app_misc_logo_start(void);
static int app_misc_logo_stop(void);
static int app_misc_logo_on_message(UINT32 msg, UINT32 param1, UINT32 param2);

APP_APP_s app_misc_logo = {
    0,    //id
    1,    //tier
    0,    //parent
    0,    //previous
    0,    //child
    0,    //GFlags
    0,    //flags
    app_misc_logo_start,    //start()
    app_misc_logo_stop,    //stop()
    app_misc_logo_on_message    //on_message()
};
/* App status */
misc_logo_t misc_logo = {0};

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
 static int app_misc_logo_on_message(UINT32 msg, UINT32 param1, UINT32 param2)
{
    int ReturnValue = 0;

    switch (msg) {
    case HMSG_STORAGE_IDLE:
        AppUtil_SetRsndStorageIdleMsg(1);
        break;
   case AMSG_CMD_APP_READY:
        //AmbaPrintColor(GREEN,"show power OFF LOGO");
        misc_logo.Func(MISC_LOGO_SHOW_LOGO, 0, 0);

        if(app_status.logo_type == LOGO_ON){
            AppLibAudioDec_Beep(BEEP_POWER_ON,0);
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
static int app_misc_logo_start(void)
{
    int ReturnValue = 0;

    misc_logo.Func = misc_logo_func;
    AmbaPrintColor(GREEN,"==%s==",__FUNCTION__);

    if (!APP_CHECKFLAGS(app_misc_logo.GFlags, APP_AFLAGS_INIT)) {
        APP_ADDFLAGS(app_misc_logo.GFlags, APP_AFLAGS_INIT);
    }

    if (!APP_CHECKFLAGS(app_misc_logo.GFlags, APP_AFLAGS_START)) {
        APP_ADDFLAGS(app_misc_logo.GFlags, APP_AFLAGS_START);
        misc_logo.Func(MISC_LOGO_START, 0, 0);
    }

    if (!APP_CHECKFLAGS(app_misc_logo.GFlags, APP_AFLAGS_READY)) {
        APP_ADDFLAGS(app_misc_logo.GFlags, APP_AFLAGS_READY);
    }

    AppLibComSvcHcmgr_SendMsg(AMSG_CMD_APP_READY, 0, 0);   //test message

    return ReturnValue;
}

/**
 *  @brief The stop flow of application.
 *
 *  The stop flow of application.
 *
 *  @return >=0 success, <0 failure
 */
static int app_misc_logo_stop(void)
{
    int ReturnValue = 0;

    AmbaPrintColor(GREEN,"==%s==",__FUNCTION__);
    ReturnValue = misc_logo.Func(MISC_LOGO_STOP, 0, 0);
    app_status.logo_type = LOGO_OFF;

    return ReturnValue;
}
