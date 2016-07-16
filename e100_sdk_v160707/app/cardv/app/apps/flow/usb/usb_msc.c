/**
  * @file src/app/apps/flow/usb/usb_msc.c
  *
  * Implementation of USB MSC class
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

#include <apps/flow/usb/usb_msc.h>

/*************************************************************************
 * Declarations (static)
 ************************************************************************/
/* App structure interfaces APIs */
static int app_usb_msc_start(void);
static int app_usb_msc_stop(void);
static int app_usb_msc_on_message(UINT32 msg, UINT32 param1, UINT32 param2);

APP_APP_s app_usb_msc = {
    0,  //Id
    1,  //Tier
    0,  //Parent
    0,  //Previous
    0,  //Child
    0,  //GFlags
    0,  //Flags
    app_usb_msc_start,//start()
    app_usb_msc_stop,    //stop()
    app_usb_msc_on_message  //OnMessage()
};

/* App status */
USB_MSC_s usb_msc = {0};

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
static int app_usb_msc_on_message(UINT32 msg, UINT32 param1, UINT32 param2)
{
    int ReturnValue = 0;

    switch (msg) {
    case AMSG_CMD_APP_READY:
        DBGMSG("[app_usb_msc] Received AMSG_CMD_APP_READY");
        usb_msc.Func(USB_MSC_APP_READY, 0, 0);
        break;
    case AMSG_CMD_USB_APP_STOP:
    case HMSG_USB_DETECT_REMOVE:
        usb_msc.Func(USB_MSC_DETECT_REMOVE, 0, 0);
        break;
    case HMSG_HDMI_INSERT_SET:
    case HMSG_HDMI_INSERT_CLR:
    case HMSG_CS_INSERT_SET:
    case HMSG_CS_INSERT_CLR:
        if (!APP_CHECKFLAGS(app_usb_msc.GFlags, APP_AFLAGS_READY)) {
            AmbaPrint("[app_usb_msc] System is not ready. Jack event will be handled later");
        } else {
            usb_msc.Func(USB_MSC_UPDATE_FCHAN_VOUT, msg, 0);
        }
        break;
    case HMSG_LINEIN_IN_SET:
    case HMSG_LINEIN_IN_CLR:
        if (!APP_CHECKFLAGS(app_usb_msc.GFlags, APP_AFLAGS_READY)) {
            AmbaPrint("[app_usb_msc] System is not ready. Jack event will be handled later");
        } else {
            usb_msc.Func(USB_MSC_AUDIO_INPUT, 0, 0);
        }
        break;
    case HMSG_HP_IN_SET:
    case HMSG_HP_IN_CLR:
    case HMSG_LINEOUT_IN_SET:
    case HMSG_LINEOUT_IN_CLR:
        if (!APP_CHECKFLAGS(app_usb_msc.GFlags, APP_AFLAGS_READY)) {
            AmbaPrint("[app_usb_msc] System is not ready. Jack event will be handled later");
        } else {
            usb_msc.Func(USB_MSC_AUDIO_OUTPUT, 0, 0);
        }
        break;
    case HMSG_NAND0_CARD_INSERT:
    case HMSG_NAND1_CARD_INSERT:
    case HMSG_SD0_CARD_INSERT:
    case HMSG_SD1_CARD_INSERT:
        if (!APP_CHECKFLAGS(app_usb_msc.GFlags, APP_AFLAGS_READY)) {
            AmbaPrint("[app_usb_msc] System is not ready. Card insert event will be handled later");
        } else {
            usb_msc.Func(USB_MSC_CARD_INSERT, 0, 0);
        }
        break;
    case HMSG_NAND0_CARD_REMOVE:
    case HMSG_NAND1_CARD_REMOVE:
    case HMSG_SD0_CARD_REMOVE:
    case HMSG_SD1_CARD_REMOVE:
        if (!APP_CHECKFLAGS(app_usb_msc.GFlags, APP_AFLAGS_READY)) {
            AmbaPrint("[app_usb_msc] System is not ready. Card remove event will be handled later");
        } else {
            usb_msc.Func(USB_MSC_CARD_REMOVE, 0, 0);
        }
        break;
    default:
        break;
    }

    return ReturnValue;
}

/**
 *  @brief The start flow of video playback application.
 *
 *  The start flow of photo playback application.
 *
 *  @return >=0 success, <0 failure
 */
static int app_usb_msc_start(void)
    {
    int ReturnValue = 0;

    /* Set app function and operate sets */
    usb_msc.Func = usb_msc_func;
    usb_msc.Gui = gui_usb_msc_func;

    if (!APP_CHECKFLAGS(app_usb_msc.GFlags, APP_AFLAGS_INIT)) {
        APP_ADDFLAGS(app_usb_msc.GFlags, APP_AFLAGS_INIT);
        usb_msc.Func(USB_MSC_INIT, 0, 0);
    }

    if (APP_CHECKFLAGS(app_usb_msc.GFlags, APP_AFLAGS_START)) {
        usb_msc.Func(USB_MSC_START_FLG_ON, 0, 0);
    } else {
        APP_ADDFLAGS(app_usb_msc.GFlags, APP_AFLAGS_START);
        usb_msc.Func(USB_MSC_SET_APP_ENV, 0, 0);

        usb_msc.Func(USB_MSC_START, 0, 0);

        usb_msc.Func(USB_MSC_CHANGE_DISPLAY, 0, 0);
        AppLibComSvcHcmgr_SendMsg(AMSG_CMD_APP_READY, 0, 0);
    }

    return ReturnValue;
}

/**
 *  @brief The stop flow of video playback application.
 *
 *  The stop flow of photo playback application.
 *
 *  @return >=0 success, <0 failure
 */
static int app_usb_msc_stop(void)
{
    int ReturnValue = 0;

    if (!APP_CHECKFLAGS(app_usb_msc.GFlags, APP_AFLAGS_READY)) {
        usb_msc.Func(USB_MSC_STOP, 0, 0);
    }

    return ReturnValue;
}
