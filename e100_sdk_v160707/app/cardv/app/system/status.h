/**
  * @file app/connected/app/system/status.h
  *
  * Header of Application Status
  *
  * History:
  *    2013/08/16 - [Martin Lai] created file
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
#ifndef APP_STATUS_H_
#define APP_STATUS_H_

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <AmbaDataType.h>

/* Application preference ID */
#define APP_PREF_USER   (0)

__BEGIN_C_PROTO__

/*************************************************************************
 * App system status definitons
 ************************************************************************/
typedef struct _APP_STATUS_s_ {
    UINT8 Type:3;
#define APP_TYPE_DSC    (0)
#define APP_TYPE_DV     (1)
    UINT8 CurrEncMode:3;
#define APP_VIDEO_ENC_MODE    (0)
#define APP_STILL_ENC_MODE     (1)
    UINT8 FchanDecModeOnly:1;
    UINT8 LockDecMode:1;
    UINT8 EncRotate;
    UINT8 EncFlip;
    UINT8 DispRotate;
#define APP_DISP_ROTATE_NONE            (0)
#define APP_DISP_ROTATE_90_DEGREE       (1)
#define APP_DISP_ROTATE_180_DEGREE      (2)
#define APP_DISP_ROTATE_270_DEGREE      (3)
    UINT8 PowerType;
#define APP_POWER_UNKNOWN    (0)
#define APP_POWER_ADAPTER    (1)
#define APP_POWER_BATTERY    (2)
    UINT8 BatteryState;
#define APP_BATTERY_NONE    (0)
#define APP_BATTERY_EMPTY    (1)
#define APP_BATTERY_0    (2)
#define APP_BATTERY_1    (3)
#define APP_BATTERY_2    (4)
#define APP_BATTERY_3    (5)
    UINT8 PowerButtonTimer;
#define POWER_TIMER_DISABLE        (0)
#define POWER_TIMER_ENABLE         (1)
    /* UINT8 partition start */
    UINT8 HdmiPluginFlag:1;
    UINT8 CompositePluginFlag:1;
    UINT8 LineinPluginFlag:1;
    UINT8 LineoutPluginFlag:1;
    UINT8 Headphone_pluginFlag:1;
    UINT8 UsbPluginFlag:1;
    UINT8 UsbChargeMode:1;
    UINT8 UsbStorageUnmount:1;
    /* UINT8 partition end */
    UINT8 ThumbnailModeMediaRoot;
    UINT8 ThumbnailModeConti;
#define THUMB_MODE_INIT         (0)  ///The initial value of parameter.
#define THUMB_MODE_CONT         (1)  ///The system will keep the parameters and decoded data of thumbnail mode.
#define THUMB_MODE_RESET    (1<<1)  ///The flow will reset the thumbnail mode and re-decode all thumbanils.
#define THUMB_MODE_DIRECT    (1<<2)  ///There is no animation between switching the adv thumbanil mode and playback mode.
#define THUMB_MODE_DIRECT_VIDEO    (1<<3)  ///The system will playback clip the after switching to playback mode from thumbnail mode.
#define THUMB_MODE_DIRECT_AUDIO    (1<<4)///The system will playback audio the after switching to playback mode from thumbnail mode.
#define THUMB_MODE_INSERT_NEW_ACTIVE_CARD    (1<<5)
    UINT8 ThumbnailTvDisplayMode;
    /* UINT8 partition start */
    UINT8 ThumbnailShiftDirection:1;
#define THUMB_MODE_SHIFT_COL (0)
#define THUMB_MODE_SHIFT_ROW (1)
    UINT8 DecodeModeFlag:1;
    UINT8 DecodeModeLock:1;
    UINT8 VideoEditMode:5;
#define VIDEO_EDIT_MODE_CROP     (1)
#define VIDEO_EDIT_MODE_PARTIAL_DEL (2)
#define VIDEO_EDIT_MODE_DIVIDE  (3)
#define VIDEO_EDIT_MODE_MERGE   (4)
    /* UINT8 partition end */
    UINT8 InitFlags;    ///< Initialization Flags
#define APP_INIT_FLAGS_LOGO_LOADED  (1)
#define APP_INIT_FLAGS_EXTLIB_INIT  (1<<1)
#define APP_INIT_FLAGS_ENCODE_INIT  (1<<2)
#define APP_INIT_FLAGS_DECODE_INIT  (1<<3)
#define APP_INIT_FLAGS_DECODE_APP_INIT  (1<<4)
    /* UINT8 partition start */
    UINT8 MvRecover:1;
    UINT8 NetCtrlSessionOn:1;   // 1: there's a net session, otherwise 0
    UINT8 FwUpdate:1;
    UINT8 CalibRework:1;
    UINT8 CardFmtParam:4;
#define APP_CARD_FMT_OPTIMUM (0x00)
#define APP_CARD_FMT_NONOPTIMUM (0x01)
#define APP_CARD_FMT_UNKNOWN    (0x02)
    /* UINT8 partition end */
    /* UINT8 partition start */
    UINT8 LoopTest:2;
    UINT8 Profiling:2;
    UINT8 DebugDump:2;
    UINT8 DebugYuvDump:2;
    /* UINT8 partition end */
    UINT8 CardBusySlot;
#define NO_BUSY_SLOT    (0xFF)
    UINT16 AppSwitchBlocked;
    UINT8 LcdDisplayFlag;
    UINT8 LcdMirror;
    UINT16 FixedVoutMode;
#define VOUT_DISP_MODE_AUTO        (0xFF)
    UINT8 PlaybackType;
#define DCIM_HDLR            (0)
#define EVENTRECORD_HDLR           (1)
#ifdef CONFIG_APP_ARD
    UINT8 logo_type;
#define LOGO_ON            (0)
#define LOGO_OFF           (1)
    UINT8 cardv_auto_encode;
    UINT8    parkingmode_on;
    int gps_status;
#define GPS_DISCONNECT     (-1)
#define GPS_NO_SIGNAL      (0)
#define GPS_SIGNAL_1       (1)
#define GPS_SIGNAL_2       (2)
#define GPS_SIGNAL_3       (3)
#define GPS_SIGNAL_4       (4)
    UINT8 UsbTreatAsDC;
    UINT8 anti_flicker_type;
    UINT8 thumb_motion_tab_switched;
#define THUMB_MOTION_TAB_SWITCHED_YES   (1)
#define THUMB_MOTION_TAB_SWITCHED_NO    (0)
#endif
} APP_STATUS_s;

extern APP_STATUS_s app_status;

__END_C_PROTO__

#endif /* APP_STATUS_H_ */
