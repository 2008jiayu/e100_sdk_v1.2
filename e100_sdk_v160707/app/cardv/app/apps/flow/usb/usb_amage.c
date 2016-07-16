/**
  * @file src/app/apps/flow/usb/usb_amage.c
  *
  * Implementation of USB MTP class for Amage
  *
  * History:
  *    2013/12/02 - [Martin Lai] created file
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

#include <apps/flow/usb/usb_amage.h>

/*************************************************************************
 * Declarations (static)
 ************************************************************************/
/* App structure interfaces APIs */
static int app_usb_amage_start(void);
static int app_usb_amage_stop(void);
static int app_usb_amage_on_message(UINT32 msg, UINT32 param1, UINT32 param2);

APP_APP_s app_usb_amage = {
    0,  //Id
    1,  //Tier
    0,  //Parent
    0,  //Previous
    0,  //Child
    0,  //GFlags
    0,  //Flags
    app_usb_amage_start,//start()
    app_usb_amage_stop,    //stop()
    app_usb_amage_on_message  //OnMessage()
};

/* App status */
USB_AMAGE_s usb_amage = {0};

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
static int app_usb_amage_on_message(UINT32 msg, UINT32 param1, UINT32 param2)
{
    int ReturnValue = 0;

    DBGMSG("[app_usb_amage] Received msg: 0x%X (param1 = 0x%X / param2 = 0x%X)", msg, param1, param2);

    switch (msg) {
    case AMSG_CMD_USB_APP_STOP:
    case HMSG_USB_DETECT_REMOVE:
        usb_amage.Func(USB_AMAGE_DETECT_REMOVE, 0, 0);
        break;
    case HMSG_USER_MODE_BUTTON:
    case HMSG_USER_IR_MODE_BUTTON:
        if (!APP_CHECKFLAGS(app_usb_amage.GFlags, APP_AFLAGS_READY)) {
            AmbaPrint("[app_usb_amage] System is not ready. Jack event will be handled later");
        } else {
            usb_amage.Func(USB_AMAGE_CHANGE_ENC_MODE, 0, 0);
        }
        break;
    case HMSG_USER_SNAP2_BUTTON:
    case HMSG_USER_IR_SNAP2_BUTTON:
        if (!APP_CHECKFLAGS(app_usb_amage.GFlags, APP_AFLAGS_READY)) {
            AmbaPrint("[app_usb_amage] System is not ready. Jack event will be handled later");
        } else {
            usb_amage.Func(USB_AMAGE_STILL_CAPTURE, 0, 0);
        }
        break;
    case HMSG_HDMI_INSERT_SET:
    case HMSG_HDMI_INSERT_CLR:
        AmbaPrint("[app_usb_amage] HMSG_HDMI_INSERT_SET");
        if (!APP_CHECKFLAGS(app_usb_amage.GFlags, APP_AFLAGS_READY)) {
            AmbaPrint("[app_usb_amage] System is not ready. Jack event will be handled later");
        } else {
            ReturnValue = usb_amage.Func(USB_AMAGE_UPDATE_FCHAN_VOUT, msg, param1);
        }
        break;
    case HMSG_MUXER_END:
        ReturnValue = usb_amage.Func(USB_AMAGE_MUXER_END, msg, param1);
        break;
    default:
        {
            APP_APP_s *curapp;
            AppAppMgt_GetApp(app_usb_amage.Parent, &curapp);
            curapp->OnMessage(msg, param1, param2);
        }
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
static int app_usb_amage_start(void)
    {
    int ReturnValue = 0;

    /* Set app function and operate sets */
    usb_amage.Func = usb_amage_func;

    if (!APP_CHECKFLAGS(app_usb_amage.GFlags, APP_AFLAGS_INIT)) {
        APP_ADDFLAGS(app_usb_amage.GFlags, APP_AFLAGS_INIT);
        usb_amage.Func(USB_AMAGE_INIT, 0, 0);
    }

    if (APP_CHECKFLAGS(app_usb_amage.GFlags, APP_AFLAGS_START)) {
        usb_amage.Func(USB_AMAGE_START_FLG_ON, 0, 0);
    } else {
        APP_ADDFLAGS(app_usb_amage.GFlags, APP_AFLAGS_START);
        if (!APP_CHECKFLAGS(app_usb_amage.GFlags, APP_AFLAGS_READY)) {
            APP_ADDFLAGS(app_usb_amage.GFlags, APP_AFLAGS_READY);
        }
        usb_amage.Func(USB_AMAGE_SET_APP_ENV, 0, 0);

        usb_amage.Func(USB_AMAGE_START, 0, 0);
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
static int app_usb_amage_stop(void)
{
    int ReturnValue = 0;

    usb_amage.Func(USB_AMAGE_STOP, 0, 0);

    return ReturnValue;
}
