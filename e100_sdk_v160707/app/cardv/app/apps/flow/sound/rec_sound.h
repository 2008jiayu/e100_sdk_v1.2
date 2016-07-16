/**
  * @file src/app/apps/flow/sound/rec_sound.h
  *
  * Header of CAR Recorder (sensor) application
  *
  * History:
  *    2014/11/26 - [QiangSu] created file
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
#ifndef APP_REC_SOUND_H__
#define APP_REC_SOUND_H__

#include "../../apps.h"
#include "../widget/widgetmgt.h"
#include "../../gui/sound/gui_rec_sound.h"
#include <system/app_util.h>

__BEGIN_C_PROTO__

#define REC_SOUND_DEBUG
#if defined(REC_SOUND_DEBUG)
#define DBGMSG  AmbaPrint
#define DBGMSGc2    AmbaPrintColor
#else
#define DBGMSG(...)
#define DBGMSGc2(...)
#endif


/** Define this flag, it will do pause/resume flow
      when memory runout */
//#define REC_SOUND_MEM_RUNOUT_PAUSE


/**threshold value for free space check*/
#define FREESPACE_THRESHOLD 400*1024*1024 /**< default 400MB*/

/*************************************************************************
 * App Flag Definitions
 ************************************************************************/
#define REC_SOUND_FLAGS_PAUSED            (1<<0)
#define REC_SOUND_FLAGS_SELFTIMER_RUN (1<<1)
#define REC_SOUND_FLAGS_MUXER_BUSY     (1<<2)
#define REC_SOUND_FLAGS_MUXER_ERROR     (1<<3)
#define REC_SOUND_FLAGS_WARNING_MSG_RUN (1<<4)
#define REC_SOUND_FLAGS_MEM_RUNOUT (1<<5)


/*************************************************************************
 * App Function Definitions
 ************************************************************************/
typedef enum _REC_SOUND_FUNC_ID_e_ {
    REC_SOUND_INIT,
    REC_SOUND_START,
    REC_SOUND_START_FLAG_ON,
    REC_SOUND_STOP,
    REC_SOUND_APP_READY,
    REC_SOUND_SET_APP_ENV,
    REC_SOUND_RECORD_START,
    REC_SOUND_RECORD_STOP,
    REC_SOUND_RECORD_PAUSE,
    REC_SOUND_RECORD_RESUME,
    REC_SOUND_RECORD_AUTO_START,
    REC_SOUND_MUXER_END,
    REC_SOUND_MUXER_REACH_LIMIT,
    REC_SOUND_ERROR_MEMORY_RUNOUT,
    REC_SOUND_ERROR_STORAGE_RUNOUT,
    REC_SOUND_ERROR_STORAGE_IO,
    REC_SOUND_ERROR_LOOP_ENC_ERR,
    REC_SOUND_LOOP_ENC_DONE,
    REC_SOUND_SWITCH_APP,
    REC_SOUND_SET_SELFTIMER,
    REC_SOUND_CARD_REMOVED,
    REC_SOUND_CARD_ERROR_REMOVED,
    REC_SOUND_CARD_NEW_INSERT,
    REC_SOUND_CARD_STORAGE_IDLE,
    REC_SOUND_CARD_STORAGE_BUSY,
    REC_SOUND_CARD_CHECK_STATUS,
    REC_SOUND_CARD_FULL_HANDLE,
    REC_SOUND_SET_FILE_INDEX,
    REC_SOUND_FILE_ID_UPDATE,
    REC_SOUND_WIDGET_CLOSED,
    REC_SOUND_SET_SYSTEM_TYPE,
    REC_SOUND_UPDATE_FCHAN_VOUT,
    REC_SOUND_UPDATE_DCHAN_VOUT,
    REC_SOUND_CHANGE_DISPLAY,
    REC_SOUND_CHANGE_OSD,
    REC_SOUND_AUDIO_INPUT,
    REC_SOUND_AUDIO_OUTPUT,
    REC_SOUND_USB_CONNECT,
    REC_SOUND_GUI_INIT_SHOW,
    REC_SOUND_UPDATE_BAT_POWER_STATUS,
    REC_SOUND_WARNING_MSG_SHOW_START,
    REC_SOUND_WARNING_MSG_SHOW_STOP,
} REC_SOUND_FUNC_ID_e;

extern int rec_sound_func(UINT32 funcId, UINT32 param1, UINT32 param2);

/*************************************************************************
 * App Operation Definitions
 ************************************************************************/
typedef struct _REC_SOUND_OP_s_ {
    int (*ButtonRecord)(void);
    int (*ButtonFocus)(void);
    int (*ButtonFocusClr)(void);
    int (*ButtonShutter)(void);
    int (*ButtonShutterClr)(void);
    int (*ButtonZoomIn)(void);
    int (*ButtonZoomInClr)(void);
    int (*ButtonZoomOut)(void);
    int (*ButtonZoomOutClr)(void);
    int (*ButtonUp)(void);
    int (*ButtonDown)(void);
    int (*ButtonLeft)(void);
    int (*ButtonRight)(void);
    int (*ButtonSet)(void);
    int (*ButtonMenu)(void);
    int (*ButtonMode)(void);
    int (*ButtonDel)(void);
    int (*ButtonPower)(void);
} REC_SOUND_OP_s;

extern REC_SOUND_OP_s rec_sound_op;

/*************************************************************************
 * App Status Definitions
 ************************************************************************/
typedef struct _REC_SOUND_s_ {
    UINT8 RecState;
#define REC_SOUND_STATE_IDLE   (0x00)
#define REC_SOUND_STATE_RECORD    (0x01)
#define REC_SOUND_STATE_RESET     (0xFF)
    int RecTime;
    int (*Func)(UINT32 funcId, UINT32 param1, UINT32 param2);
    int (*Gui)(UINT32 guiCmd, UINT32 param1, UINT32 param2);
    REC_SOUND_OP_s *Op;
} REC_SOUND_s;

extern REC_SOUND_s rec_sound;

__END_C_PROTO__

#endif /* APP_REC_SOUND_H__ */
