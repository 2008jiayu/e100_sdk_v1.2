/**
  * @file src/app/apps/flow/usb/usb_amage.h
  *
  * Header of USB MTP class for Amage
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
#ifndef APP_USB_AMAGE_H__
#define APP_USB_AMAGE_H__

#include <apps/apps.h>

__BEGIN_C_PROTO__

//#define USB_AMAGE_DEBUG
#if defined(USB_AMAGE_DEBUG)
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
typedef enum _USB_AMAGE_FUNC_ID_e_ {
    USB_AMAGE_INIT = 0,
    USB_AMAGE_START,
    USB_AMAGE_START_FLG_ON,
    USB_AMAGE_STOP,
    USB_AMAGE_DETECT_REMOVE,
    USB_AMAGE_SET_APP_ENV,
    USB_AMAGE_CHANGE_DISPLAY,
    USB_AMAGE_CHANGE_ENC_MODE,
    USB_AMAGE_UPDATE_FCHAN_VOUT,
    USB_AMAGE_STILL_CAPTURE,
    USB_AMAGE_MUXER_END
} USB_AMAGE_FUNC_ID_e;

extern int usb_amage_func(UINT32 funcId, UINT32 param1, UINT32 param2);

#define USB_AMAGE_FLAGS_CAPTURE            (1<<0)

/*************************************************************************
 * App Status Definitions
 ************************************************************************/
typedef struct _USB_AMAGE_s_ {
    UINT32 CurMode;
#define USB_AMAGE_VIDEO_MODE   0
#define USB_AMAGE_STILL_MODE    1
    int (*Func)(UINT32 funcId, UINT32 param1, UINT32 param2);
    int (*Gui)(UINT32 guiCmd, UINT32 param1, UINT32 param2);
} USB_AMAGE_s;

extern USB_AMAGE_s usb_amage;

__END_C_PROTO__

#endif /* APP_USB_AMAGE_H__ */
