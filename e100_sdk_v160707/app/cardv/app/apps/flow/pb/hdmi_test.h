/**
  * @file src/app/apps/flow/pb/hdmi_test.h
  *
  * Header of video playback application
  *
  * History:
  *    2015/05/25 - [James Wang] created file
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
#ifndef APP_HDMI_TEST_H__
#define APP_HDMI_TEST_H__

#include <apps/apps.h>
#include <apps/flow/widget/widgetmgt.h>
#include <apps/gui/pb/gui_hdmi_test.h>

__BEGIN_C_PROTO__

//#define HDMI_TEST_DEBUG
#if defined(HDMI_TEST_DEBUG)
#define DBGMSG  AmbaPrint
#define DBGMSGc2    AmbaPrintColor
#else
#define DBGMSG(...)
#define DBGMSGc2(...)
#endif


/*************************************************************************
 * App Flag Definitions
 ************************************************************************/
#define HDMI_TEST_OP_BLOCKED                (0x0001)
#define HDMI_TEST_OP_CONT                (0x0002)
#define HDMI_TEST_WARNING_MSG_RUN    (0x0004)
#define HDMI_TEST_DELETE_FILE_RUN        (0x0008)

/** Play flags */
#define HDMI_TEST_PLAYBACK_NORMAL        (0x00010000)
#define HDMI_TEST_PLAYBACK_ZOOM                (0x00020000)
#define HDMI_TEST_PLAYBACK_ROTATE                (0x00040000)

/** OPEN_CUR parameters */
#define HDMI_TEST_OPEN_RESET    (0)
#define HDMI_TEST_OPEN_VIDEO_CONT    (1)

/** GET_FILE parameters */
#define GET_CURR_FILE    (0)
#define GET_NEXT_FILE    (1)
#define GET_PREV_FILE    (2)

/** Play speed definitions */
#define PBACK_SPEED_NORMAL        (0x1 <<  8)    /** 0x0100 */
#define PBACK_SPEED_MIN            (0x1 <<  2)    /** 0x0004 */
#define PBACK_SPEED_MAX            (0x1 << 14)    /** 0x4000 */

/** Rotate dictionary */
typedef enum _HDMI_TEST_ROTATE_INDEX_e_ {
    ROTATE_0 = 0,
    ROTATE_90,
    ROTATE_180,
    ROTATE_270,
    ROTATE_NUM
} HDMI_TEST_ROTATE_INDEX_e;


/*************************************************************************
 * App Function Definitions
 ************************************************************************/
typedef enum _HDMI_TEST_FUNC_ID_e_ {
    HDMI_TEST_INIT = 0,
    HDMI_TEST_START,
    HDMI_TEST_START_FLG_ON,
    HDMI_TEST_STOP,
    HDMI_TEST_APP_READY,
    HDMI_TEST_SET_APP_ENV,
    HDMI_TEST_START_DISP_PAGE,
    HDMI_TEST_OPEN,
    HDMI_TEST_PLAY,
    HDMI_TEST_EOS,
    HDMI_TEST_STOP_PLAYING,
    HDMI_TEST_SWITCH_APP,
    HDMI_TEST_GET_FILE,
    HDMI_TEST_CARD_REMOVED,
    HDMI_TEST_CARD_ERROR_REMOVED,
    HDMI_TEST_CARD_NEW_INSERT,
    HDMI_TEST_CARD_STORAGE_IDLE,
    HDMI_TEST_SET_FILE_INDEX,
    HDMI_TEST_DELETE_FILE_DIALOG_SHOW,
    HDMI_TEST_DELETE_FILE,
    HDMI_TEST_DELETE_FILE_COMPLETE,
    HDMI_TEST_STATE_WIDGET_CLOSED,
    HDMI_TEST_SET_SYSTEM_TYPE,
    HDMI_TEST_UPDATE_FCHAN_VOUT,
    HDMI_TEST_UPDATE_FCHAN_FIXED_VOUT,
    HDMI_TEST_UPDATE_DCHAN_VOUT,
    HDMI_TEST_CHANGE_DISPLAY,
    HDMI_TEST_CHANGE_OSD,
    HDMI_TEST_AUDIO_INPUT,
    HDMI_TEST_AUDIO_OUTPUT,
    HDMI_TEST_USB_CONNECT,
    HDMI_TEST_GUI_INIT_SHOW,
    HDMI_TEST_UPDATE_BAT_POWER_STATUS,
    HDMI_TEST_WARNING_MSG_SHOW_START,
    HDMI_TEST_WARNING_MSG_SHOW_STOP
} HDMI_TEST_FUNC_ID_e;

extern int hdmi_test_func(UINT32 funcId, UINT32 param1, UINT32 param2);

/*************************************************************************
 * App Operation Definitions
 ************************************************************************/
typedef struct _HDMI_TEST_OP_s_ {
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
    int (*ButtonN4)(void);
    int (*ButtonN5)(void);
    int (*ButtonN6)(void);
    int (*ButtonN7)(void);
    int (*ButtonN8)(void);
    int (*ButtonN9)(void);
    int (*ButtonN0)(void);
    int (*ButtonN100)(void);
    int (*ButtonReturn)(void);
    int (*ButtonDisplay)(void);
    int (*ButtonStandard)(void);
    int (*ButtonImage)(void);
    int (*ButtonMode)(void);
    int (*ButtonDel)(void);
    int (*ButtonPower)(void);
} HDMI_TEST_OP_s;

extern HDMI_TEST_OP_s hdmi_test_op;

/*************************************************************************
 * App Status Definitions
 ************************************************************************/

typedef struct _HDMI_TEST_FILE_INFO_s_ {
    APPLIB_DCF_MEDIA_TYPE_e MediaRoot;
    int TotalFileNum;
    int FileCur;
} HDMI_TEST_FILE_INFO_s;

typedef struct _HDMI_TEST_MEDIA_INFO_s_ {
    UINT32 Width;
    UINT32 Height;
    UINT32 Frate;
    UINT32 Ftime;
    UINT32 AspectRatio;
    INT16 Rotate;
    INT16 RotateOri;
    UINT32 Zoom;
#define HDMI_TEST_IZOOM_MIN        (100)
#define HDMI_TEST_IZOOM_MAX        (1000)
    int ZoomStep;
    UINT8 Direction;
#define HDMI_TEST_PLAY_FWD    (0x00)
#define HDMI_TEST_PLAY_REV    (0x01)
    UINT8 State;
#define HDMI_TEST_PLAY_PAUSED    (0x00)
#define HDMI_TEST_PLAY_PLAY        (0x01)
    UINT32 Speed;
    UINT64 TotalTime;    /** in millisecond */
    UINT64 PlayTime;    /** in millisecond */
} HDMI_TEST_MEDIA_INFO_s;

typedef struct _HDMI_TEST_s_ {
    char CurFn[MAX_FILENAME_LENGTH];
    char FirstFn[MAX_FILENAME_LENGTH];
    UINT64 CurFileObjID;
    int CurFileType;
#define HDMI_TEST_MEDIA_VIDEO        (1)
#define HDMI_TEST_MEDIA_IMAGE        (2)
#define HDMI_TEST_MEDIA_AUDIO        (3)
    UINT64 CurPlayTime;    /** in millisecond */
    HDMI_TEST_FILE_INFO_s FileInfo;
    HDMI_TEST_MEDIA_INFO_s MediaInfo;
    int (*Func)(UINT32 funcId, UINT32 param1, UINT32 param2);
    int (*Gui)(UINT32 guiCmd, UINT32 param1, UINT32 param2);
    HDMI_TEST_OP_s *Op;
    int (*OpBlocked)(void);
    int (*OpCont)(void);
} HDMI_TEST_s;

extern HDMI_TEST_s hdmi_test;

__END_C_PROTO__

#endif /* APP_HDMI_TEST_H__ */
