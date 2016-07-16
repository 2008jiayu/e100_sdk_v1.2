/**
  * @file src/app/apps/flow/misc/misc_calibupdate.h
  *
  * Header of Calibration update application
  *
  * History:
  *    2014/06/25 - [Annie Ting] created file
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

#ifndef APP_MISC_CALIBUPDATE_H_
#define APP_MISC_CALIBUPDATE_H_

#include <apps/apps.h>
#include <apps/flow/widget/widgetmgt.h>
#include <apps/gui/misc/gui_misc_calibupdate.h>

__BEGIN_C_PROTO__

//#define MISC_CALIBUPDATE_DEBUG
#if defined(MISC_CALIBUPDATE_DEBUG)
#define DBGMSG  AmbaPrint
#define DBGMSGc2    AmbaPrintColor
#else
#define DBGMSG(...)
#define DBGMSGc2(...)
#endif

/*************************************************************************
 * App Flags Definitions
 ************************************************************************/
#define MISC_CALIBUPDATE_WARNING_MSG_RUN    (0x0001)

/*************************************************************************
 * App General Definitions
 ************************************************************************/

/*************************************************************************
 * App Function Definitions
 ************************************************************************/
typedef enum _MISC_CALIBUPDATE_FUNC_ID_e_ {
    MISC_CALIBUPDATE_INIT = 0,
    MISC_CALIBUPDATE_START,
    MISC_CALIBUPDATE_START_FLG_ON,
    MISC_CALIBUPDATE_STOP,
    MISC_CALIBUPDATE_APP_READY,
    MISC_CALIBUPDATE_SET_APP_ENV,
    MISC_CALIBUPDATE_SWITCH_APP,
    MISC_CALIBUPDATE_DIALOG_SHOW_CALIBUPDATE,
    MISC_CALIBUPDATE_OP_DONE,
    MISC_CALIBUPDATE_OP_SUCCESS,
    MISC_CALIBUPDATE_OP_FAILED,
    MISC_CALIBUPDATE_CARD_REMOVED,
    MISC_CALIBUPDATE_CARD_ERROR_REMOVED,
    MISC_CALIBUPDATE_UPDATE_FCHAN_VOUT,
    MISC_CALIBUPDATE_UPDATE_DCHAN_VOUT,
    MISC_CALIBUPDATE_CHANGE_DISPLAY,
    MISC_CALIBUPDATE_CHANGE_OSD,
    MISC_CALIBUPDATE_AUDIO_INPUT,
    MISC_CALIBUPDATE_AUDIO_OUTPUT,
    MISC_CALIBUPDATE_USB_CONNECT,
    MISC_CALIBUPDATE_GUI_INIT_SHOW,
    MISC_CALIBUPDATE_WARNING_MSG_SHOW_START,
    MISC_CALIBUPDATE_WARNING_MSG_SHOW_STOP
} MISC_CALIBUPDATE_FUNC_ID_e;

extern int misc_calibupdate_func(UINT32 funcId, UINT32 param1, UINT32 param2);

/*************************************************************************
 * App Operation Definitions
 ************************************************************************/

/*************************************************************************
 * App Status Definitions
 ************************************************************************/
typedef struct _MISC_CALIBUPDATE_s_ {
    int (*Func)(UINT32 funcId, UINT32 param1, UINT32 param2);
    int (*Gui)(UINT32 guiCmd, UINT32 param1, UINT32 param2);
} MISC_CALIBUPDATE_s;

extern MISC_CALIBUPDATE_s misc_calibupdate;

__END_C_PROTO__

#endif /* APP_MISC_CALIBUPDATE_H_ */
