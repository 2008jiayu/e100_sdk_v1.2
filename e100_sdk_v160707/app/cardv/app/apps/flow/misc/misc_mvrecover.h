/**
  * @file src/app/apps/flow/misc/misc_mvrecover.h
  *
  * Header of Movie Recover application
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
#ifndef APP_MISC_MVRECOVER_H__
#define APP_MISC_MVRECOVER_H__

#include <apps/apps.h>
#include <apps/flow/widget/widgetmgt.h>
#include <apps/gui/misc/gui_misc_mvrecover.h>

__BEGIN_C_PROTO__

//#define MISC_MVRECOVER_DEBUG
#if defined(MISC_MVRECOVER_DEBUG)
#define DBGMSG  AmbaPrint
#define DBGMSGc2    AmbaPrintColor
#else
#define DBGMSG(...)
#define DBGMSGc2(...)
#endif


/*************************************************************************
 * App Flag Definitions
 ************************************************************************/
#define MISC_MVRECOVER_WARNING_MSG_RUN    (0x0004)
#ifdef CONFIG_APP_ARD
#define MISC_MVRECOVER_THM_RECOVER    (0x0008)
#endif


/*************************************************************************
 * App Function Definitions
 ************************************************************************/
typedef enum _MISC_MVRECOVER_FUNC_ID_e_ {
    MISC_MVRECOVER_INIT = 0,
    MISC_MVRECOVER_START,
    MISC_MVRECOVER_START_FLG_ON,
    MISC_MVRECOVER_STOP,
    MISC_MVRECOVER_APP_READY,
    MISC_MVRECOVER_SET_APP_ENV,
    MISC_MVRECOVER_SWITCH_APP,
    MISC_MVRECOVER_DIALOG_SHOW_MVRECOVER,
    MISC_MVRECOVER_OP_SUCCESS,
    MISC_MVRECOVER_OP_FAILED,
    MISC_MVRECOVER_CARD_REMOVED,
    MISC_MVRECOVER_CARD_ERROR_REMOVED,
    MISC_MVRECOVER_UPDATE_FCHAN_VOUT,
    MISC_MVRECOVER_UPDATE_DCHAN_VOUT,
    MISC_MVRECOVER_CHANGE_DISPLAY,
    MISC_MVRECOVER_CHANGE_OSD,
    MISC_MVRECOVER_AUDIO_INPUT,
    MISC_MVRECOVER_AUDIO_OUTPUT,
    MISC_MVRECOVER_USB_CONNECT,
    MISC_MVRECOVER_GUI_INIT_SHOW,
    MISC_MVRECOVER_WARNING_MSG_SHOW_START,
    MISC_MVRECOVER_WARNING_MSG_SHOW_STOP
} MISC_MVRECOVER_FUNC_ID_e;

extern int misc_mvrecover_func(UINT32 funcId, UINT32 param1, UINT32 param2);

/*************************************************************************
 * App Status Definitions
 ************************************************************************/

typedef struct _MISC_MVRECOVER_s_ {
    int (*Func)(UINT32 funcId, UINT32 param1, UINT32 param2);
    int (*Gui)(UINT32 guiCmd, UINT32 param1, UINT32 param2);
} MISC_MVRECOVER_s;

extern MISC_MVRECOVER_s misc_mvrecover;

__END_C_PROTO__

#endif /* APP_MISC_MVRECOVER_H__ */
