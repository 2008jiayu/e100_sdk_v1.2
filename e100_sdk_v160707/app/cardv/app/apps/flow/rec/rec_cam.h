/**
  * @file src/app/apps/flow/rec/rec_cam.h
  *
  * Header of Recorder (sensor) application
  *
  * History:
  *    2013/03/24 - [Annie Ting] created file
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
#ifndef APP_REC_CAM_H__
#define APP_REC_CAM_H__

#include <apps/apps.h>
#include <apps/flow/widget/widgetmgt.h>
#include <apps/gui/rec/gui_rec_cam.h>
#ifdef CONFIG_APP_ARD
#include <system/app_util.h>
#endif
__BEGIN_C_PROTO__

#define REC_CAM_DEBUG
#if defined(REC_CAM_DEBUG)
#define DBGMSG  AmbaPrint
#define DBGMSGc2    AmbaPrintColor
#else
#define DBGMSG(...)
#define DBGMSGc2(...)
#endif


/** Define this flag, it will do pause/resume flow
      when memory runout */
//#define REC_CAM_MEM_RUNOUT_PAUSE


/**threshold value for free space check*/
#define FREESPACE_THRESHOLD 400*1024*1024 /**< default 400MB*/

/***/
#ifdef CONFIG_APP_ARD
#define MAX_PHOTO_COUNT 50
#define MAX_EVENT_FILE (50)
#else
#define MAX_PHOTO_COUNT 50
#endif
/*************************************************************************
 * App Flag Definitions
 ************************************************************************/
#define REC_CAM_FLAGS_PAUSED            (1<<0)
#define REC_CAM_FLAGS_SELFTIMER_RUN (1<<1)
#define REC_CAM_FLAGS_MUXER_BUSY     (1<<2)
#define REC_CAM_FLAGS_MUXER_OPEN     (1<<3)
#define REC_CAM_FLAGS_MUXER_ERROR     (1<<4)
#define REC_CAM_FLAGS_SHUTTER_PRESSED  (1<<5)
#define REC_CAM_FLAGS_STILL_CAPTURE (1<<6)
#define REC_CAM_FLAGS_CAPTURE_BG_PROCESS (1<<7)
#define REC_CAM_FLAGS_WARNING_MSG_RUN (1<<8)
#define REC_CAM_FLAGS_MEM_RUNOUT (1<<9)
#define REC_CAM_FLAGS_EM_RECORD  (1<<10)
#define REC_CAM_FLAGS_PES_DELAY   (1<<11)
#define REC_CAM_FLAGS_CAPTURE_ON_VF  (1<<12)
#define REC_CAM_FLAGS_NETFIFO_BUSY  (1<<13)
#define REC_CAM_FLAGS_CAPTURE_FROM_NETCTRL    (1<<14)
#define REC_CAM_FLAGS_CAPTURE_PIV    (1<<15)
#define REC_CAM_FLAGS_BLOCK_MENU    (1<<16)
#ifdef CONFIG_APP_ARD
#define REC_CAM_FLAGS_NORMAL_MUX_END    (1<<17)
#define REC_CAM_FLAGS_EVENT_MUX_END    (1<<18)
#define REC_CAM_FLAGS_APP_RECEIVED_MUX_START    (1<<19)
#endif

/*************************************************************************
 * App Function Definitions
 ************************************************************************/
typedef enum _REC_CAM_FUNC_ID_e_ {
    REC_CAM_INIT,
    REC_CAM_START,
    REC_CAM_START_FLAG_ON,
    REC_CAM_STOP,
    REC_CAM_SET_APP_ENV,
    REC_CAM_LIVEVIEW_STATE,
    REC_CAM_LIVEVIEW_POST_ACTION,
    REC_CAM_SELFTIMER_START,
    REC_CAM_SELFTIMER_STOP,
    REC_CAM_FOCUS,
    REC_CAM_PREVIEW,
    REC_CAM_CAPTURE,
    REC_CAM_CAPTURE_PIV,
    REC_CAM_CAPTURE_COMPLETE,
    REC_CAM_CAPTURE_BG_PROCESS_DONE,
    REC_CAM_PIV,
    REC_CAM_RECORD_START,
    REC_CAM_RECORD_STOP,
    REC_CAM_RECORD_PAUSE,
    REC_CAM_RECORD_RESUME,
    REC_CAM_RECORD_AUTO_START,
    REC_CAM_RECORD_PRE_RECORD,
    REC_CAM_RECORD_EMERGENCY_START,
    REC_CAM_MUXER_START,
    REC_CAM_MUXER_OPEN,
    REC_CAM_MUXER_END,
    REC_CAM_MUXER_REACH_LIMIT,
    REC_CAM_EVENTRECORD_START,
    REC_CAM_MUXER_END_EVENTRECORD,
    REC_CAM_MUXER_REACH_LIMIT_EVENTRECORD,
    REC_CAM_MUXER_STREAM_ERROR,
    REC_CAM_ERROR_MEMORY_RUNOUT,
    REC_CAM_ERROR_STORAGE_RUNOUT,
    REC_CAM_ERROR_STORAGE_IO,
    REC_CAM_ERROR_LOOP_ENC_ERR,
    REC_CAM_LOOP_ENC_DONE,
    REC_CAM_EDTMGR_DONE,
    REC_CAM_EDTMGR_FAIL,
    REC_CAM_EM_RECORD_RETURN,
    REC_CAM_SWITCH_APP,
    REC_CAM_SET_VIDEO_RES,
    REC_CAM_SET_VIDEO_QUALITY,
    REC_CAM_SET_VIDEO_PRE_RECORD,
    REC_CAM_SET_VIDEO_TIME_LAPSE,
    REC_CAM_SET_VIDEO_DUAL_STREAMS,
    REC_CAM_SET_VIDEO_RECORD_MODE,
    REC_CAM_SET_PHOTO_SIZE,
    REC_CAM_SET_PHOTO_QUALITY,
    REC_CAM_SET_ENC_MODE,
    REC_CAM_SET_DMF_MODE,
    REC_CAM_SET_SELFTIMER,
    REC_CAM_CARD_REMOVED,
    REC_CAM_CARD_ERROR_REMOVED,
    REC_CAM_CARD_NEW_INSERT,
    REC_CAM_CARD_STORAGE_IDLE,
    REC_CAM_CARD_STORAGE_BUSY,
    REC_CAM_CARD_CHECK_STATUS,
    REC_CAM_CARD_FULL_HANDLE,
    REC_CAM_CARD_FULL_HANDLE_EVENT,
    REC_CAM_SET_FILE_INDEX,
    REC_CAM_FILE_ID_UPDATE,
    REC_CAM_WIDGET_CLOSED,
    REC_CAM_SET_SYSTEM_TYPE,
    REC_CAM_UPDATE_FCHAN_VOUT,
    REC_CAM_UPDATE_DCHAN_VOUT,
    REC_CAM_CHANGE_DISPLAY,
    REC_CAM_CHANGE_OSD,
    REC_CAM_AUDIO_INPUT,
    REC_CAM_AUDIO_OUTPUT,
    REC_CAM_USB_CONNECT,
    REC_CAM_GUI_INIT_SHOW,
    REC_CAM_UPDATE_BAT_POWER_STATUS,
    REC_CAM_WARNING_MSG_SHOW_START,
    REC_CAM_WARNING_MSG_SHOW_STOP,
    REC_CAM_CARD_FMT_NONOPTIMUM,
    REC_CAM_ADAS_EVENT,
    REC_CAM_ADAS_FUNCTION_INIT,
    REC_CAM_ADAS_UPDATE_PARAM,
#ifdef CONFIG_APP_ARD
    REC_CAM_MOTION_DETECT_START,
    REC_CAM_MOTION_DETECT_STOP,
    REC_CAM_MOTION_DETECT_RECORD,
    REC_CAM_MIC_SWITCH,
    REC_CAM_ADAS_CEL,
    REC_CAM_VIDEO_SET_GSENSOR_SENSITIVITY,
    REC_CAM_SET_SPLIT_TIME,
    REC_CAR_VIDEO_EVENT_FILE_NUM_UPDATE,
    REC_CAR_PHOTO_FILE_NUM_UPDATE,
    REC_CAM_MUXER_GENERAL_ERROR,
    REC_CAM_MUXER_IO_ERROR_EVENTRECORD,
    REC_CAM_MUXER_OPEN_EVENTRECORD,
#endif
#if defined(CONFIG_APP_AMBA_LINK)
    REC_CAM_BOSS_BOOTED,
    REC_CAM_VF_START,
    REC_CAM_VF_STOP,
    REC_CAM_VF_SWITCH_TO_RECORD,
    REC_CAM_CAPTURE_ON_VF,
    REC_CAM_NETFIFO_EVENT_START,
    REC_CAM_NETFIFO_EVENT_STOP,
    REC_CAM_NETCTRL_CAPTURE_DONE,
#endif
    REC_CAM_RECORD_LED_START,
    REC_CAM_CALIBRATION_ICN,

} REC_CAM_FUNC_ID_e;

extern int rec_cam_func(UINT32 funcId, UINT32 param1, UINT32 param2);

/*************************************************************************
 * App Operation Definitions
 ************************************************************************/
typedef struct _REC_CAM_OP_s_ {
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
    int (*ButtonF4)(void);
} REC_CAM_OP_s;

extern REC_CAM_OP_s rec_cam_op;

#if defined(CONFIG_APP_AMBA_LINK)
typedef struct _REC_CAM_NETCTRL_s_ {
    int (*NetCtrlRecordStart)(void);
    int (*NetCtrlRecordStop)(void);
    int (*NetCtrlGetRecordTime)(void);
    int (*NetCtrlCapture)(void);
    int (*NetCtrlContinueCaptureStop)(void);
    int (*NetCtrlRefreshPrefTable)(void);
    int (*NetCtrlGetAllCurSetting)(void);
    int (*NetCtrlGetSettingOptions)(UINT32 Param1, UINT32 Param2);
    int (*NetCtrlGetSetting)(UINT32 Param1, UINT32 Param2);
    int (*NetCtrlSetSetting)(UINT32 Param1, UINT32 Param2);
    int (*NetCtrlGetNumbFiles)(UINT32 Param1, UINT32 Param2);
    int (*NetCtrlGetDeviceInfo)(void);
    int (*NetCtrlVFStop)(void);
    int (*NetCtrlVFReset)(void);
    int (*NetCtrlGetThumb)(UINT32 Param1, UINT32 Param2);
    int (*NetCtrlGetMediaInfo)(UINT32 Param1, UINT32 Param2);
    int (*NetCtrlFormat)(UINT32 Param1, UINT32 Param2);
    int (*NetCtrlFormatDone)(UINT32 Param1, UINT32 Param2);
    int (*NetCtrlGetSpace)(UINT32 Param1, UINT32 Param2);
    int (*NetCtrlCustomCmd)(UINT32 Param1, UINT32 Param2);
} REC_CAM_NETCTRL_s;

extern REC_CAM_NETCTRL_s rec_cam_netctrl_op;

#define APP_NETCTRL_FLAGS_VF_DISABLE    (1<<0)
#define APP_NETCTRL_FLAGS_VF_RESET_DONE (1<<1)
#define APP_NETCTRL_FLAGS_VF_STOP_DONE  (1<<2)



#endif


/*************************************************************************
 * App Status Definitions
 ************************************************************************/
typedef struct _REC_CAM_s_ {
    UINT8 RecCapState;
#define REC_CAP_STATE_PREVIEW   (0x00)
#define REC_CAP_STATE_RECORD    (0x01)
#define REC_CAP_STATE_PRE_RECORD    (0x02)
#define REC_CAP_STATE_FOCUS     (0x03)
#define REC_CAP_STATE_CAPTURE   (0x04)
#define REC_CAP_STATE_VF   (0x05)
#define REC_CAP_STATE_TRANSIT_TO_VF   (0x06)
#define REC_CAP_STATE_RESET     (0xFF)
    UINT8 RecCurrMode;
    UINT8 RecNextMode;
#define REC_CAP_MODE_STOP        (0x00)
#define REC_CAP_MODE_VIEWFINDER    (0x01)
#define REC_CAP_MODE_RECORD        (0x02)
#define REC_CAP_MODE_CAPTURE        (0x03)
    UINT8 SelfTimerType;
#define SELF_TIMER_TYPE_PHOTO   (0)
#define SELF_TIMER_TYPE_VIDEO   (1)
    INT32 MuxerNum;
    int RecTime;
    int SelfTimerTime;
    int TimeLapseTime;
#ifdef CONFIG_APP_ARD
#define REC_CAP_MOTION_DETECT_STOP    (0x00)
#define REC_CAP_MOTION_DETECT_START   (0x01)
    int MotionDetectStatus;
    int MotionDetectRecordRemainTime;
#endif
    int QuickViewFileType;
#define MEDIA_TYPE_UNKNOWN  (0x00)
#define MEDIA_TYPE_VIDEO  (0x01)
#define MEDIA_TYPE_PHOTO  (0x02)
    int (*Func)(UINT32 funcId, UINT32 param1, UINT32 param2);
    int (*Gui)(UINT32 guiCmd, UINT32 param1, UINT32 param2);
    REC_CAM_OP_s *Op;
#if defined(CONFIG_APP_AMBA_LINK)
    UINT32 NetCtrlFlags;
    REC_CAM_NETCTRL_s *NetCtrl;
#endif
#ifdef CONFIG_APP_ARD
#define REC_CAP_PREPARE_TO_REPLAY_NONE              (0x00)
#define REC_CAP_PREPARE_TO_REPLAY_MANUAL_RECORD     (0x01)
#define REC_CAP_PREPARE_TO_REPLAY_AUTO_RECORD       (0x02)
    int PrepareToReplayRecord;
#endif
} REC_CAM_s;

extern REC_CAM_s rec_cam;
extern int adas_cel_sta;
extern int adas_cel_set;



__END_C_PROTO__

#endif /* APP_REC_CAM_H__ */
