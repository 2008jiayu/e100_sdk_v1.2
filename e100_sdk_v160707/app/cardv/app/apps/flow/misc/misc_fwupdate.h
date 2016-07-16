/**
  * @file src/app/apps/flow/misc/misc_fwupdate.h
  *
  * Header of Firmware update application
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

#ifndef APP_MISC_FWUPDATE_H_
#define APP_MISC_FWUPDATE_H_

#include <apps/apps.h>
#include <apps/flow/widget/widgetmgt.h>
#include <apps/gui/misc/gui_misc_fwupdate.h>

__BEGIN_C_PROTO__

//#define MISC_FWUPDATE_DEBUG
#if defined(MISC_FWUPDATE_DEBUG)
#define DBGMSG  AmbaPrint
#define DBGMSGc2    AmbaPrintColor
#else
#define DBGMSG(...)
#define DBGMSGc2(...)
#endif

/*************************************************************************
 * App Flags Definitions
 ************************************************************************/
#define MISC_FWUPDATE_WARNING_MSG_RUN    (0x0001)

/*************************************************************************
 * App General Definitions
 ************************************************************************/

/*************************************************************************
 * App Function Definitions
 ************************************************************************/
typedef enum _MISC_FWUPDATE_FUNC_ID_e_ {
    MISC_FWUPDATE_INIT = 0,
    MISC_FWUPDATE_START,
    MISC_FWUPDATE_START_FLG_ON,
    MISC_FWUPDATE_STOP,
    MISC_FWUPDATE_APP_READY,
    MISC_FWUPDATE_SET_APP_ENV,
    MISC_FWUPDATE_SWITCH_APP,
    MISC_FWUPDATE_DIALOG_SHOW_FWUPDATE,
    MISC_FWUPDATE_CARD_REMOVED,
    MISC_FWUPDATE_CARD_ERROR_REMOVED,
    MISC_FWUPDATE_UPDATE_FCHAN_VOUT,
    MISC_FWUPDATE_UPDATE_DCHAN_VOUT,
    MISC_FWUPDATE_CHANGE_DISPLAY,
    MISC_FWUPDATE_CHANGE_OSD,
    MISC_FWUPDATE_AUDIO_INPUT,
    MISC_FWUPDATE_AUDIO_OUTPUT,
    MISC_FWUPDATE_USB_CONNECT,
    MISC_FWUPDATE_GUI_INIT_SHOW,
    MISC_FWUPDATE_WARNING_MSG_SHOW_START,
    MISC_FWUPDATE_WARNING_MSG_SHOW_STOP
} MISC_FWUPDATE_FUNC_ID_e;

extern int misc_fwupdate_func(UINT32 funcId, UINT32 param1, UINT32 param2);

/*************************************************************************
 * App Operation Definitions
 ************************************************************************/

/*************************************************************************
 * App Status Definitions
 ************************************************************************/
typedef struct _MISC_FWUPDATE_s_ {
    int (*Func)(UINT32 funcId, UINT32 param1, UINT32 param2);
    int (*Gui)(UINT32 guiCmd, UINT32 param1, UINT32 param2);
} MISC_FWUPDATE_s;

extern MISC_FWUPDATE_s misc_fwupdate;

__END_C_PROTO__

#endif /* APP_MISC_FWUPDATE_H_ */
