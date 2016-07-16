/**
  * @file src/app/apps/flow/pb/pb_multi.h
  *
  * Header of video playback application
  *
  * History:
  *    2014/09/11 - [Annie Ting] created file
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
#ifndef APP_PB_MULTI_H__
#define APP_PB_MULTI_H__

#include <apps/apps.h>
#include <apps/flow/widget/widgetmgt.h>
#include <apps/gui/pb/gui_pb_multi.h>

__BEGIN_C_PROTO__

//#define PB_MULTI_DEBUG
#if defined(PB_MULTI_DEBUG)
#define DBGMSG  AmbaPrint
#define DBGMSGc2    AmbaPrintColor
#else
#define DBGMSG(...)
#define DBGMSGc2(...)
#endif


/*************************************************************************
 * App Flag Definitions
 ************************************************************************/
#define PB_MULTI_OP_BLOCKED                (0x0001)
#define PB_MULTI_OP_CONT                (0x0002)
#define PB_MULTI_WARNING_MSG_RUN    (0x0004)
#define PB_MULTI_DELETE_FILE_RUN        (0x0008)

/** Play flags */
#define PB_MULTI_PLAYBACK_NORMAL        (0x00010000)
#define PB_MULTI_PLAYBACK_ZOOM                (0x00020000)
#define PB_MULTI_PLAYBACK_ROTATE                (0x00040000)

/** OPEN_CUR parameters */
#define PB_MULTI_OPEN_RESET    (0)
#define PB_MULTI_OPEN_VIDEO_CONT    (1)

/** GET_FILE parameters */
#define GET_CURR_FILE    (0)
#define GET_NEXT_FILE    (1)
#define GET_PREV_FILE    (2)

/** Play speed definitions */
#define PBACK_SPEED_NORMAL        (0x1 <<  8)    /** 0x0100 */
#define PBACK_SPEED_MIN            (0x1 <<  2)    /** 0x0004 */
#define PBACK_SPEED_MAX            (0x1 << 14)    /** 0x4000 */

/** Rotate dictionary */
typedef enum _PB_MULTI_ROTATE_INDEX_e_ {
    ROTATE_0 = 0,
    ROTATE_90,
    ROTATE_180,
    ROTATE_270,
    ROTATE_NUM
} PB_MULTI_ROTATE_INDEX_e;


/*************************************************************************
 * App Function Definitions
 ************************************************************************/
typedef enum _PB_MULTI_FUNC_ID_e_ {
    PB_MULTI_INIT = 0,
    PB_MULTI_START,
    PB_MULTI_START_FLG_ON,
    PB_MULTI_STOP,
    PB_MULTI_APP_READY,
    PB_MULTI_SET_APP_ENV,
    PB_MULTI_START_DISP_PAGE,
    PB_MULTI_OPEN,
    PB_MULTI_PLAY,
    PB_MULTI_EOS,
    PB_MULTI_STOP_PLAYING,
    PB_MULTI_SWITCH_APP,
    PB_MULTI_GET_FILE,
    PB_MULTI_CARD_REMOVED,
    PB_MULTI_CARD_ERROR_REMOVED,
    PB_MULTI_CARD_NEW_INSERT,
    PB_MULTI_CARD_STORAGE_IDLE,
    PB_MULTI_SET_FILE_INDEX,
#ifdef CONFIG_APP_ARD
	PB_MULTI_SELECT_DELETE_FILE_MODE,
    PB_MULTI_SET_DELETE_FILE_MODE,
#endif
    PB_MULTI_DELETE_FILE_DIALOG_SHOW,
    PB_MULTI_DELETE_FILE,
    PB_MULTI_DELETE_FILE_COMPLETE,
    PB_MULTI_STATE_WIDGET_CLOSED,
    PB_MULTI_SET_SYSTEM_TYPE,
    PB_MULTI_UPDATE_FCHAN_VOUT,
    PB_MULTI_UPDATE_DCHAN_VOUT,
    PB_MULTI_CHANGE_DISPLAY,
    PB_MULTI_CHANGE_OSD,
    PB_MULTI_AUDIO_INPUT,
    PB_MULTI_AUDIO_OUTPUT,
    PB_MULTI_USB_CONNECT,
    PB_MULTI_GUI_INIT_SHOW,
    PB_MULTI_UPDATE_BAT_POWER_STATUS,
    PB_MULTI_WARNING_MSG_SHOW_START,
    PB_MULTI_WARNING_MSG_SHOW_STOP,
    PB_MULTI_DECODE_ERROR,
    PB_MULTI_OPEN_NEXT_OR_PREVIOUS
} PB_MULTI_FUNC_ID_e;

extern int pb_multi_func(UINT32 funcId, UINT32 param1, UINT32 param2);

/*************************************************************************
 * App Operation Definitions
 ************************************************************************/
typedef struct _PB_MULTI_OP_s_ {
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
} PB_MULTI_OP_s;

extern PB_MULTI_OP_s pb_multi_op;

/*************************************************************************
 * App Status Definitions
 ************************************************************************/

typedef struct _PB_MULTI_FILE_INFO_s_ {
    APPLIB_DCF_MEDIA_TYPE_e MediaRoot;
    int TotalFileNum;
    int FileCur;
} PB_MULTI_FILE_INFO_s;

typedef struct _PB_MULTI_MEDIA_INFO_s_ {
    UINT32 Width;
    UINT32 Height;
    UINT32 Frate;
    UINT32 Ftime;
    UINT32 AspectRatio;
    INT16 Rotate;
    INT16 RotateOri;
    UINT32 Zoom;
#define PB_MULTI_IZOOM_MIN        (100)
#define PB_MULTI_IZOOM_MAX        (1000)
    int ZoomStep;
    UINT8 Direction;
#define PB_MULTI_PLAY_FWD    (0x00)
#define PB_MULTI_PLAY_REV    (0x01)
    UINT8 State;
#define PB_MULTI_PLAY_PAUSED    (0x00)
#define PB_MULTI_PLAY_PLAY        (0x01)
    UINT32 Speed;
    UINT64 TotalTime;    /** in millisecond */
    UINT64 PlayTime;    /** in millisecond */
} PB_MULTI_MEDIA_INFO_s;

typedef struct _PB_MULTI_s_ {
    char CurFn[MAX_FILENAME_LENGTH];
    char FirstFn[MAX_FILENAME_LENGTH];
    UINT64 CurFileObjID;
    int CurFileType;
#define PB_MULTI_MEDIA_VIDEO        (1)
#define PB_MULTI_MEDIA_IMAGE        (2)
#define PB_MULTI_MEDIA_AUDIO        (3)
    UINT64 CurPlayTime;    /** in millisecond */
    UINT32 StillDecWaitFlag;
    PB_MULTI_FILE_INFO_s FileInfo;
    PB_MULTI_MEDIA_INFO_s MediaInfo;
#ifdef CONFIG_APP_ARD
	UINT8 DeleteFileMode;
#endif
    int (*Func)(UINT32 funcId, UINT32 param1, UINT32 param2);
    int (*Gui)(UINT32 guiCmd, UINT32 param1, UINT32 param2);
    PB_MULTI_OP_s *Op;
    int (*OpBlocked)(void);
    int (*OpCont)(void);
} PB_MULTI_s;

extern PB_MULTI_s pb_multi;

__END_C_PROTO__

#endif /* APP_PB_MULTI_H__ */
