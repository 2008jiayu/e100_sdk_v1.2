/**
  * @file src/app/apps/flow/usb/usb_msc.h
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
#ifndef APP_USB_MSC_H__
#define APP_USB_MSC_H__

#include <apps/apps.h>
#include <apps/gui/usb/gui_usb_msc.h>

__BEGIN_C_PROTO__

//#define USB_MSC_DEBUG
#if defined(USB_MSC_DEBUG)
#define DBGMSG  AmbaPrint
#define DBGMSGc2    AmbaPrintColor
#else
#define DBGMSG(...)
#define DBGMSGc2(...)
#endif


/*************************************************************************
 * App Flag Definitions
 ************************************************************************/


/*************************************************************************
 * App Function Definitions
 ************************************************************************/
typedef enum _USB_MSC_FUNC_ID_e_ {
    USB_MSC_INIT = 0,
    USB_MSC_START,
    USB_MSC_START_FLG_ON,
    USB_MSC_STOP,
    USB_MSC_APP_READY,
    USB_MSC_CHANGE_DISPLAY,
    USB_MSC_CHANGE_OSD,
    USB_MSC_SET_APP_ENV,
    USB_MSC_DETECT_REMOVE,
    USB_MSC_UPDATE_FCHAN_VOUT,
    USB_MSC_UPDATE_DCHAN_VOUT,
    USB_MSC_AUDIO_INPUT,
    USB_MSC_AUDIO_OUTPUT,
    USB_MSC_GUI_INIT_SHOW,
    USB_MSC_CARD_INSERT,
    USB_MSC_CARD_REMOVE
} USB_MSC_FUNC_ID_e;

extern int usb_msc_func(UINT32 funcId, UINT32 param1, UINT32 param2);


/*************************************************************************
 * App Status Definitions
 ************************************************************************/
typedef struct _USB_MSC_s_ {
    int (*Func)(UINT32 funcId, UINT32 param1, UINT32 param2);
    int (*Gui)(UINT32 guiCmd, UINT32 param1, UINT32 param2);
} USB_MSC_s;

extern USB_MSC_s usb_msc;

__END_C_PROTO__

#endif /* APP_USB_MSC_H__ */
