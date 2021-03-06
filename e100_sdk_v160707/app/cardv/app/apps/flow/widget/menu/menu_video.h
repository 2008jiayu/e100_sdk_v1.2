/**
  * @file src/app/apps/flow/widget/menu/connectedcam/menu_video.h
  *
  * Header of Video-related Menu Items
  *
  * History:
  *    2013/11/22 - [Martin Lai] created file
  *
  *
 * Copyright (c) 2015 Ambarella, Inc.
 *
 * This file and its contents (��Software��) are protected by intellectual property rights
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
#ifndef APP_WIDGET_MENU_VIDEO_H_
#define APP_WIDGET_MENU_VIDEO_H_

__BEGIN_C_PROTO__

/*************************************************************************
 * Video menu definitions
 ************************************************************************/
typedef enum _MENU_VIDEO_ITEM_ID_e_ {
    MENU_VIDEO_SENSOR_RES = 0,
    MENU_VIDEO_YUV_RES,
    MENU_VIDEO_QUALITY,
    MENU_VIDEO_SELFTIMER,
    MENU_VIDEO_PRE_RECORD,
    MENU_VIDEO_TIME_LAPSE,
    MENU_VIDEO_DUAL_STREAMS,
    MENU_VIDEO_STREAMS_TYPE,
    MENU_VIDEO_STREAMING,
    MENU_VIDEO_DIGITAL_ZOOM,
    MENU_VIDEO_STAMP,
    MENU_VIDEO_REC_MODE,
    //MENU_VIDEO_ADAS,
    MENU_VIDEO_ADAS_CALIB,
#ifdef CONFIG_APP_ARD
    MENU_VIDEO_MOTION_DETECT,
    MENU_VIDEO_GSENSOR_SENSITIVITY,
    MENU_VIDEO_PARKINGMODE_SENSITIVITY,
    MENU_VIDEO_SPLIT_REC_MODE,
    MENU_VIDEO_STAMP_CHOOSE,
    MENU_VIDEO_MICPHONE_SETUP,
    MENU_VIDEO_FLICKER,
#endif
    MENU_VIDEO_ITEM_NUM
} MENU_VIDEO_ITEM_ID_e;

/**
* video mres selection table is in sensor config
**/
typedef enum _MENU_VIDEO_SENSOR_RES_SEL_ID_e_ {
    MENU_VIDEO_SENSOR_RES_1 = 0,
    MENU_VIDEO_SENSOR_RES_2,
    MENU_VIDEO_SENSOR_RES_3,
    MENU_VIDEO_SENSOR_RES_4,
    MENU_VIDEO_SENSOR_RES_5,
    MENU_VIDEO_SENSOR_RES_6,
    MENU_VIDEO_SENSOR_RES_7,
    MENU_VIDEO_SENSOR_RES_8,
    MENU_VIDEO_SENSOR_RES_9,
    MENU_VIDEO_SENSOR_RES_10,
    MENU_VIDEO_SENSOR_RES_11,
    MENU_VIDEO_SENSOR_RES_12,
    MENU_VIDEO_SENSOR_RES_13,
    MENU_VIDEO_SENSOR_RES_14,
    MENU_VIDEO_SENSOR_RES_15,
    MENU_VIDEO_SENSOR_RES_16,
    MENU_VIDEO_SENSOR_RES_17,
    MENU_VIDEO_SENSOR_RES_18,
    MENU_VIDEO_SENSOR_RES_19,
    MENU_VIDEO_SENSOR_RES_20,
    MENU_VIDEO_SENSOR_RES_21,
    MENU_VIDEO_SENSOR_RES_22,
    MENU_VIDEO_SENSOR_RES_23,
    MENU_VIDEO_SENSOR_RES_24,
    MENU_VIDEO_SENSOR_RES_25,
    MENU_VIDEO_SENSOR_RES_26,
    MENU_VIDEO_SENSOR_RES_27,
    MENU_VIDEO_SENSOR_RES_28,
    MENU_VIDEO_SENSOR_RES_29,
    MENU_VIDEO_SENSOR_RES_30,
    MENU_VIDEO_SENSOR_RES_31,
    MENU_VIDEO_SENSOR_RES_32,
    MENU_VIDEO_SENSOR_RES_SEL_NUM
} MENU_VIDEO_SENSOR_RES_SEL_ID_e;

typedef enum _MENU_VIDEO_YUV_RES_SEL_ID_e_ {
    MENU_VIDEO_YUV_RES_1 = 0,
    MENU_VIDEO_YUV_RES_2,
    MENU_VIDEO_YUV_RES_3,
    MENU_VIDEO_YUV_RES_4,
    MENU_VIDEO_YUV_RES_5,
    MENU_VIDEO_YUV_RES_6,
    MENU_VIDEO_YUV_RES_7,
    MENU_VIDEO_YUV_RES_8,
    MENU_VIDEO_YUV_RES_9,
    MENU_VIDEO_YUV_RES_10,
    MENU_VIDEO_YUV_RES_11,
    MENU_VIDEO_YUV_RES_12,
    MENU_VIDEO_YUV_RES_13,
    MENU_VIDEO_YUV_RES_14,
    MENU_VIDEO_YUV_RES_15,
    MENU_VIDEO_YUV_RES_16,
    MENU_VIDEO_YUV_RES_17,
    MENU_VIDEO_YUV_RES_18,
    MENU_VIDEO_YUV_RES_19,
    MENU_VIDEO_YUV_RES_20,
    MENU_VIDEO_YUV_RES_21,
    MENU_VIDEO_YUV_RES_22,
    MENU_VIDEO_YUV_RES_23,
    MENU_VIDEO_YUV_RES_24,
    MENU_VIDEO_YUV_RES_25,
    MENU_VIDEO_YUV_RES_26,
    MENU_VIDEO_YUV_RES_27,
    MENU_VIDEO_YUV_RES_28,
    MENU_VIDEO_YUV_RES_29,
    MENU_VIDEO_YUV_RES_30,
    MENU_VIDEO_YUV_RES_31,
    MENU_VIDEO_YUV_RES_32,
    MENU_VIDEO_YUV_RES_SEL_NUM
} MENU_VIDEO_YUV_RES_SEL_ID_e;

typedef enum _MENU_VIDEO_QUALITY_SEL_ID_e_ {
    MENU_VIDEO_QUALITY_SFINE = 0,
    MENU_VIDEO_QUALITY_FINE,
    MENU_VIDEO_QUALITY_NORMAL,
    MENU_VIDEO_QUALITY_SEL_NUM
} MENU_VIDEO_QUALITY_SEL_ID_e;

typedef enum _MENU_VIDEO_SELFTIMER_SEL_ID_e_ {
    MENU_VIDEO_SELFTIMER_OFF = 0,
    MENU_VIDEO_SELFTIMER_3S,
    MENU_VIDEO_SELFTIMER_5S,
    MENU_VIDEO_SELFTIMER_10S,
    MENU_VIDEO_SELFTIMER_SEL_NUM
} MENU_VIDEO_SELFTIMER_SEL_ID_e;

typedef enum _MENU_VIDEO_PRE_RECORD_SEL_ID_e_ {
    MENU_VIDEO_PRE_RECORD_OFF = 0,
    MENU_VIDEO_PRE_RECORD_ON,
    MENU_VIDEO_PRE_RECORD_SEL_NUM
} MENU_VIDEO_PRE_RECORD_SEL_ID_e;

typedef enum _MENU_VIDEO_TIME_LAPSE_SEL_ID_e_ {
    MENU_VIDEO_TIME_LAPSE_OFF = 0,
    MENU_VIDEO_TIME_LAPSE_2S,
    MENU_VIDEO_TIME_LAPSE_SEL_NUM
} MENU_VIDEO_TIME_LAPSE_SEL_ID_e;

typedef enum _MENU_VIDEO_DUAL_STREAMS_SEL_ID_e_ {
    MENU_VIDEO_DUAL_STREAMS_OFF = 0,
    MENU_VIDEO_DUAL_STREAMS_ON,
    MENU_VIDEO_DUAL_STREAMS_SEL_NUM
} MENU_VIDEO_DUAL_STREAMS_SEL_ID_e;

typedef enum _MENU_VIDEO_STREAMS_TYPE_SEL_ID_e_ {
#if defined(CONFIG_AMBA_STREAMING) || defined(CONFIG_APP_AMBA_LINK)
    MENU_VIDEO_STREAMS_TYPE_OFF = 0,
#if defined(CONFIG_SUPPORT_BOSS) || defined(CONFIG_APP_AMBA_LINK)
    MENU_VIDEO_STREAMS_TYPE_RTSP,
#endif
    MENU_VIDEO_STREAMS_TYPE_MJPG,
    MENU_VIDEO_STREAMS_TYPE_UVC_MJPG,
    MENU_VIDEO_STREAMS_TYPE_HLS,
    MENU_VIDEO_STREAMS_TYPE_SEL_NUM
#else
    MENU_VIDEO_STREAMS_TYPE_OFF = 0,
    MENU_VIDEO_STREAMS_TYPE_MJPG,
    MENU_VIDEO_STREAMS_TYPE_SEL_NUM
#endif
} MENU_VIDEO_STREAMS_TYPE_SEL_ID_e;

typedef enum _MENU_VIDEO_STREAMING_SEL_ID_e_ {
    MENU_VIDEO_STREAMING_OFF = 0,
    MENU_VIDEO_STREAMING_ON,
    MENU_VIDEO_STREAMING_SEL_NUM
} MENU_VIDEO_STREAMING_SEL_ID_e;


typedef enum _MENU_VIDEO_DIGIAL_ZOOM_SEL_ID_e_ {
    MENU_VIDEO_DIGITAL_ZOOM_OFF = 0,
    MENU_VIDEO_DIGITAL_ZOOM_ON,
    MENU_VIDEO_DIGITAL_ZOOM_NUMX,
    MENU_VIDEO_DIGITAL_ZOOM_SEL_NUM
} MENU_VIDEO_DIGIAL_ZOOM_SEL_ID_e;

typedef enum _MENU_VIDEO_STAMP_SEL_ID_e_ {
    MENU_VIDEO_STAMP_OFF = 0,
    MENU_VIDEO_STAMP_DATE,
    MENU_VIDEO_STAMP_TIME,
    MENU_VIDEO_STAMP_BOTH,
    MENU_VIDEO_STAMP_SEL_NUM
} MENU_VIDEO_STAMP_SEL_ID_e;

typedef enum _MENU_VIDEO_REC_MODE_SEL_ID_e_ {
    MENU_VIDEO_REC_MODE_VIDEO_MODE = 0,
    MENU_VIDEO_REC_MODE_STILL_MODE,
    MENU_VIDEO_REC_MODE_SEL_NUM
} MENU_VIDEO_REC_MODE_SEL_ID_e;

typedef enum _MENU_VIDEO_ADAS_CALIBRATION_SEL_ID_e_ {
    MENU_VIDEO_ADAS_CALIBRATION_SET = 0,
    MENU_VIDEO_ADAS_CALIBRATION_SEL_NUM
} MENU_VIDEO_ADAS_CALIBRATION_SEL_ID_e;

#ifdef CONFIG_APP_ARD
typedef enum _MENU_VIDEO_MOTION_DETECT_SEL_ID_e_ {
    MENU_VIDEO_MOTION_DETECT_OFF = 0,
    MENU_VIDEO_MOTION_DETECT_ON,
    MENU_VIDEO_MOTION_DETECT_SEL_NUM
} MENU_VIDEO_MOTION_DETECT_SEL_ID_e;

typedef enum _MENU_VIDEO_GSENSOR_SENSITIVITY_SEL_ID_e_ {
    MENU_VIDEO_GSENSOR_OFF = 0,
    MENU_VIDEO_GSENSOR_SENSITIVITY_H,
    MENU_VIDEO_GSENSOR_SENSITIVITY_M,
    MENU_VIDEO_GSENSOR_SENSITIVITY_L,
    MENU_VIDEO_GSENSOR_SENSITIVITY_SEL_NUM
}MENU_VIDEO_GSENSOR_SENSITIVITY_SEL_ID_e;

enum menu_video_parkingmode_sensitivity_sel_id_e {
    MENU_VIDEO_PARKINGMODE_OFF = 0,
    MENU_VIDEO_PARKINGMODE_ON,
    MENU_VIDEO_PARKINGMODE_SENSITIVITY_SEL_NUM
};

typedef enum _MENU_VIDEO_SPLIT_REC_MODE_SEL_ID_e_ {
    MENU_VIDEO_SPLIT_REC_MODE_1MIN = 0,
    //MENU_VIDEO_SPLIT_REC_MODE_2MIN,
    MENU_VIDEO_SPLIT_REC_MODE_3MIN,
    MENU_VIDEO_SPLIT_REC_MODE_5MIN,
    //MENU_VIDEO_SPLIT_REC_MODE_OFF,
    MENU_VIDEO_SPLIT_REC_MODE_SEL_NUM
} MENU_VIDEO_SPLIT_REC_MODE_SEL_ID_e;

enum menu_video_stamp_choose_sel_id_e {
    MENU_VIDEO_STAMP_CHOOSE_DATE = 0,
    MENU_VIDEO_STAMP_CHOOSE_TIME,
    //MENU_VIDEO_STAMP_CHOOSE_GPS,
    //MENU_VIDEO_STAMP_CHOOSE_GSESNOR,
    MENU_VIDEO_STAMP_CHOOSE_DRIVERID,
    MENU_VIDEO_STAMP_CHOOSE_NUM,
};

typedef enum _MENU_VIDEO_MICPHONE_SETUP_SEL_ID_e_ {
    MENU_VIDEO_MICPHONE_SETUP_OFF = 0,
    MENU_VIDEO_MICPHONE_SETUP_ON,
    MENU_VIDEO_MICPHONE_SETUP_SEL_NUM
} MENU_VIDEO_MICPHONE_SETUP_SEL_ID_e;

typedef enum _MENU_VIDEO_FLICKER_SEL_ID_e_ {
    MENU_VIDEO_FLICKER_AUTO = 0,
    MENU_VIDEO_FLICKER_60HZ,
    MENU_VIDEO_FLICKER_50HZ,
    MENU_VIDEO_FLICKER_SEL_NUM
} MENU_VIDEO_FLICKER_SEL_ID_e;
#endif

__END_C_PROTO__

#endif /* APP_WIDGET_MENU_VIDEO_H_ */
