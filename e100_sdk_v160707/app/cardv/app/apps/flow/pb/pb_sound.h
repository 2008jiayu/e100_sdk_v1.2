/**
  * @file src/app/apps/flow/pb/pb_sound.h
  *
  * Header of sound playback application
  *
  * History:
  *    2013/07/09 - [Martin Lai] created file
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
#ifndef APP_PB_SOUND_H__
#define APP_PB_SOUND_H__

#include <apps/apps.h>
#include <apps/flow/widget/widgetmgt.h>
#include <apps/gui/pb/gui_pb_sound.h>

__BEGIN_C_PROTO__

//#define PB_SOUND_DEBUG
#if defined(PB_SOUND_DEBUG)
#define DBGMSG  AmbaPrint
#define DBGMSGc2    AmbaPrintColor
#else
#define DBGMSG(...)
#define DBGMSGc2(...)
#endif


/*************************************************************************
 * App Flag Definitions
 ************************************************************************/
#define PB_SOUND_OP_BLOCKED                (0x0001)
#define PB_SOUND_OP_CONT                (0x0002)
#define PB_SOUND_WARNING_MSG_RUN    (0x0004)
#define PB_SOUND_DELETE_FILE_RUN        (0x0008)

/** Play flags */
#define PB_SOUND_PLAYBACK_NORMAL        (0x00010000)
#define PB_SOUND_PLAYBACK_ZOOM                (0x00020000)
#define PB_SOUND_PLAYBACK_ROTATE                (0x00040000)

/** OPEN_CUR parameters */
#define PB_SOUND_OPEN_RESET    (0)
#define PB_SOUND_OPEN_SOUND_CONT    (1)

/** GET_FILE parameters */
#define GET_CURR_FILE    (0)
#define GET_NEXT_FILE    (1)
#define GET_PREV_FILE    (2)

/** Play speed definitions */
#define PBACK_SPEED_NORMAL        (0x1 <<  8)    /** 0x0100 */
#define PBACK_SPEED_MIN            (0x1 <<  2)    /** 0x0004 */
#define PBACK_SPEED_MAX            (0x1 << 14)    /** 0x4000 */


/*************************************************************************
 * App Function Definitions
 ************************************************************************/
typedef enum _PB_SOUND_FUNC_ID_e_ {
    PB_SOUND_INIT = 0,
    PB_SOUND_START,
    PB_SOUND_START_FLG_ON,
    PB_SOUND_STOP,
    PB_SOUND_APP_READY,
    PB_SOUND_SET_APP_ENV,
    PB_SOUND_START_DISP_PAGE,
    PB_SOUND_OPEN,
    PB_SOUND_PLAY,
    PB_SOUND_EOS,
    PB_SOUND_STOP_PLAYING,
    PB_SOUND_SWITCH_APP,
    PB_SOUND_GET_FILE,
    PB_SOUND_CARD_REMOVED,
    PB_SOUND_CARD_ERROR_REMOVED,
    PB_SOUND_CARD_NEW_INSERT,
    PB_SOUND_CARD_STORAGE_IDLE,
    PB_SOUND_SET_FILE_INDEX,
    PB_SOUND_DELETE_FILE_DIALOG_SHOW,
    PB_SOUND_DELETE_FILE,
    PB_SOUND_DELETE_FILE_COMPLETE,
    PB_SOUND_STATE_WIDGET_CLOSED,
    PB_SOUND_SET_SYSTEM_TYPE,
    PB_SOUND_UPDATE_FCHAN_VOUT,
    PB_SOUND_UPDATE_DCHAN_VOUT,
    PB_SOUND_CHANGE_DISPLAY,
    PB_SOUND_CHANGE_OSD,
    PB_SOUND_AUDIO_INPUT,
    PB_SOUND_AUDIO_OUTPUT,
    PB_SOUND_USB_CONNECT,
    PB_SOUND_GUI_INIT_SHOW,
    PB_SOUND_UPDATE_BAT_POWER_STATUS,
    PB_SOUND_WARNING_MSG_SHOW_START,
    PB_SOUND_WARNING_MSG_SHOW_STOP
} PB_SOUND_FUNC_ID_e;

extern int pb_sound_func(UINT32 funcId, UINT32 param1, UINT32 param2);

/*************************************************************************
 * App Operation Definitions
 ************************************************************************/
typedef struct _PB_SOUND_OP_s_ {
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
} PB_SOUND_OP_s;

extern PB_SOUND_OP_s pb_sound_op;

/*************************************************************************
 * App Status Definitions
 ************************************************************************/

typedef struct _PB_SOUND_FILE_INFO_s_ {
    APPLIB_DCF_MEDIA_TYPE_e MediaRoot;
    int TotalFileNum;
    int FileCur;
} PB_SOUND_FILE_INFO_s;

typedef struct _PB_SOUND_MEDIA_INFO_s_ {
    UINT32 Width;
    UINT32 Height;
    UINT32 Frate;
    UINT32 Ftime;
    UINT32 AspectRatio;
    INT16 Rotate;
    INT16 RotateOri;
    UINT32 Zoom;
#define PB_SOUND_IZOOM_MIN        (100)
#define PB_SOUND_IZOOM_MAX        (1000)
    int ZoomStep;
    UINT8 Direction;
#define PB_SOUND_PLAY_FWD    (0x00)
#define PB_SOUND_PLAY_REV    (0x01)
    UINT8 State;
#define PB_SOUND_PLAY_PAUSED    (0x00)
#define PB_SOUND_PLAY_PLAY        (0x01)
    UINT32 Speed;
    UINT64 TotalTime;    /** in millisecond */
    UINT64 PlayTime;    /** in millisecond */
} PB_SOUND_MEDIA_INFO_s;

typedef struct _PB_SOUND_s_ {
    char CurFn[MAX_FILENAME_LENGTH];
    char FirstFn[MAX_FILENAME_LENGTH];
    UINT32 CurFileObjID;
    PB_SOUND_FILE_INFO_s FileInfo;
    PB_SOUND_MEDIA_INFO_s MediaInfo;
    int (*Func)(UINT32 funcId, UINT32 param1, UINT32 param2);
    int (*Gui)(UINT32 guiCmd, UINT32 param1, UINT32 param2);
    PB_SOUND_OP_s *Op;
    int (*OpBlocked)(void);
    int (*OpCont)(void);
} PB_SOUND_s;

extern PB_SOUND_s pb_sound;

__END_C_PROTO__

#endif /* APP_PB_SOUND_H__ */
