/**
  * @file src/app/apps/flow/widget/menu/connectedcam/menu_video.c
  *
  * Implementation of Video-related Menu Items
  *
  * History:
  *    2013/11/22 - [Martin Lai] created file
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
 * affiliates.  In the absence of such an agreement, you agree to promptly notify and
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
#include <apps/apps.h>
#include <system/status.h>
#include <system/app_pref.h>
#include <system/app_util.h>
#include <apps/flow/widget/menu/widget.h>
#include <apps/gui/resource/gui_settle.h>
#include <apps/flow/widget/menu/widget.h>
#include <apps/gui/utility/gui_utility.h>

#ifdef CONFIG_APP_ARD
#include <apps/flow/widget/menu/menu_check_box.h>
#include <apps/flow/rec/rec_cam.h>
#endif

#ifdef CONFIG_APP_ARD
#define APP_MD
#endif

/*************************************************************************
 * Declaration:
 ************************************************************************/
// tab
static int menu_video_init(void);
static int menu_video_start(void);
static int menu_video_stop(void);
// item
static int menu_video_sensor_res_init(void);
static int menu_video_sensor_res_get_tab_str(void);
static int menu_video_sensor_res_get_sel_str(int ref);
static int menu_video_sensor_res_get_sel_bmp(int ref);
static int menu_video_sensor_res_set(void);
static int menu_video_sensor_res_sel_set(void);
static int menu_video_yuv_res_init(void);
static int menu_video_yuv_res_get_tab_str(void);
static int menu_video_yuv_res_get_sel_str(int ref);
static int menu_video_yuv_res_get_sel_bmp(int ref);
static int menu_video_yuv_res_set(void);
static int menu_video_yuv_res_sel_set(void);
static int menu_video_quality_init(void);
static int menu_video_quality_get_tab_str(void);
static int menu_video_quality_get_sel_str(int ref);
static int menu_video_quality_get_sel_bmp(int ref);
static int menu_video_quality_set(void);
static int menu_video_quality_sel_set(void);
static int menu_video_selftimer_init(void);
static int menu_video_selftimer_get_tab_str(void);
static int menu_video_selftimer_get_sel_str(int ref);
static int menu_video_selftimer_get_sel_bmp(int ref);
static int menu_video_selftimer_set(void);
static int menu_video_selftimer_sel_set(void);
static int menu_video_pre_record_init(void);
static int menu_video_pre_record_get_tab_str(void);
static int menu_video_pre_record_get_sel_str(int ref);
static int menu_video_pre_record_get_sel_bmp(int ref);
static int menu_video_pre_record_set(void);
static int menu_video_pre_record_sel_set(void);
static int menu_video_time_lapse_init(void);
static int menu_video_time_lapse_get_tab_str(void);
static int menu_video_time_lapse_get_sel_str(int ref);
static int menu_video_time_lapse_get_sel_bmp(int ref);
static int menu_video_time_lapse_set(void);
static int menu_video_time_lapse_sel_set(void);
static int menu_video_dual_streams_init(void);
static int menu_video_dual_streams_get_tab_str(void);
static int menu_video_dual_streams_get_sel_str(int ref);
static int menu_video_dual_streams_get_sel_bmp(int ref);
static int menu_video_dual_streams_set(void);
static int menu_video_dual_streams_sel_set(void);

static int menu_video_streams_type_init(void);
static int menu_video_streams_type_get_tab_str(void);
static int menu_video_streams_type_get_sel_str(int ref);
static int menu_video_streams_type_get_sel_bmp(int ref);
static int menu_video_streams_type_set(void);
static int menu_video_streams_type_sel_set(void);

static int menu_video_streaming_init(void);
static int menu_video_streaming_get_tab_str(void);
static int menu_video_streaming_get_sel_str(int ref);
static int menu_video_streaming_get_sel_bmp(int ref);
static int menu_video_streaming_set(void);
static int menu_video_streaming_sel_set(void);

static int menu_video_digital_zoom_init(void);
static int menu_video_digital_zoom_get_tab_str(void);
static int menu_video_digital_zoom_get_sel_str(int ref);
static int menu_video_digital_zoom_get_sel_bmp(int ref);
static int menu_video_digital_zoom_set(void);
static int menu_video_digital_zoom_sel_set(void);
static int menu_video_stamp_init(void);
static int menu_video_stamp_get_tab_str(void);
static int menu_video_stamp_get_sel_str(int ref);
static int menu_video_stamp_get_sel_bmp(int ref);
static int menu_video_stamp_set(void);
static int menu_video_stamp_sel_set(void);
static int menu_video_rec_mode_init(void);
static int menu_video_rec_mode_get_tab_str(void);
static int menu_video_rec_mode_get_sel_str(int ref);
static int menu_video_rec_mode_get_sel_bmp(int ref);
static int menu_video_rec_mode_set(void);
static int menu_video_rec_mode_sel_set(void);
static int menu_video_adas_calibration_init(void);
static int menu_video_adas_calibration_get_tab_str(void);
static int menu_video_adas_calibration_get_sel_str(int ref);
static int menu_video_adas_calibration_get_sel_bmp(int ref);
static int menu_video_adas_calibration_set(void);
static int menu_video_adas_calibration_sel_set(void);
#ifdef CONFIG_APP_ARD
static int menu_video_motion_detect_init(void);
static int menu_video_motion_detect_get_tab_str(void);
static int menu_video_motion_detect_get_sel_str(int ref);
static int menu_video_motion_detect_get_sel_bmp(int ref);
static int menu_video_motion_detect_set(void);
static int menu_video_motion_detect_sel_set(void);

static int menu_video_gsensor_sensitivity_init(void);
static int menu_video_gsensor_sensitivity_get_tab_str(void);
static int menu_video_gsensor_sensitivity_get_sel_str(int ref);
static int menu_video_gsensor_sensitivity_get_sel_bmp(int ref);
static int menu_video_gsensor_sensitivity_set(void);
static int menu_video_gsensor_sensitivity_sel_set(void);

static int menu_video_parkingmode_sensitivity_init(void);
static int menu_video_parkingmode_sensitivity_get_tab_str(void);
static int menu_video_parkingmode_sensitivity_get_sel_str(int ref);
static int menu_video_parkingmode_sensitivity_get_sel_bmp(int ref);
static int menu_video_parkingmode_sensitivity_set(void);
static int menu_video_parkingmode_sensitivity_sel_set(void);

static int menu_video_split_rec_init(void);
static int menu_video_split_rec_get_tab_str(void);
static int menu_video_split_rec_get_sel_str(int ref);
static int menu_video_split_rec_get_sel_bmp(int ref);
static int menu_video_split_rec_set(void);
static int menu_video_split_rec_sel_set(void);

static CHECK_BOX_ITEM_S Video_Stamp_Choose_Item[MENU_VIDEO_STAMP_CHOOSE_NUM] = {0};
static int menu_video_stamp_choose_init(void);
static int menu_video_stamp_choose_get_tab_str(void);
static int menu_video_stamp_choose_set(void);

static int menu_video_micphone_setup_init(void);
static int menu_video_micphone_setup_get_tab_str(void);
static int menu_video_micphone_setup_get_sel_str(int ref);
static int menu_video_micphone_setup_get_sel_bmp(int ref);
static int menu_video_micphone_setup_set(void);
static int menu_video_micphone_setup_sel_set(void);

static int menu_video_flicker_init(void);
static int menu_video_flicker_get_tab_str(void);
static int menu_video_flicker_get_sel_str(int ref);
static int menu_video_flicker_get_sel_bmp(int ref);
static int menu_video_flicker_set(void);
static int menu_video_flicker_sel_set(void);

#endif

// control
static MENU_TAB_s* menu_video_get_tab(void);
static MENU_ITEM_s* menu_video_get_item(UINT32 itemId);
static MENU_SEL_s* menu_video_get_sel(UINT32 itemId, UINT32 selId);
static int menu_video_set_sel_table(UINT32 itemId, MENU_SEL_s *selTbl);
static int menu_video_lock_tab(void);
static int menu_video_unlock_tab(void);
static int menu_video_enable_item(UINT32 itemId);
static int menu_video_disable_item(UINT32 itemId);
static int menu_video_lock_item(UINT32 itemId);
static int menu_video_unlock_item(UINT32 itemId);
static int menu_video_enable_sel(UINT32 itemId, UINT32 selId);
static int menu_video_disable_sel(UINT32 itemId, UINT32 selId);
static int menu_video_lock_sel(UINT32 itemId, UINT32 selId);
static int menu_video_unlock_sel(UINT32 itemId, UINT32 selId);

/*************************************************************************
 * Definition:
 ************************************************************************/
/*** Selection ***/
/**
* video mres selection table is in sensor config
**/
static MENU_SEL_s menu_video_sensor_res_sel_tbl[MENU_VIDEO_SENSOR_RES_SEL_NUM] = {
    {MENU_VIDEO_SENSOR_RES_1, 0,
        STR_VIDEO_RES_1, 0, 0,
        MENU_VIDEO_SENSOR_RES_1, NULL},
    {MENU_VIDEO_SENSOR_RES_2, 0,
        STR_VIDEO_RES_2, 0, 0,
        MENU_VIDEO_SENSOR_RES_2, NULL},
    {MENU_VIDEO_SENSOR_RES_3, 0,
        STR_VIDEO_RES_3, 0, 0,
        MENU_VIDEO_SENSOR_RES_3, NULL},
    {MENU_VIDEO_SENSOR_RES_4, 0,
        STR_VIDEO_RES_4, 0, 0,
        MENU_VIDEO_SENSOR_RES_4, NULL},
    {MENU_VIDEO_SENSOR_RES_5, 0,
        STR_VIDEO_RES_5, 0, 0,
        MENU_VIDEO_SENSOR_RES_5, NULL},
    {MENU_VIDEO_SENSOR_RES_6, 0,
        STR_VIDEO_RES_6, 0, 0,
        MENU_VIDEO_SENSOR_RES_6, NULL},
    {MENU_VIDEO_SENSOR_RES_7, 0,
        STR_VIDEO_RES_7, 0, 0,
        MENU_VIDEO_SENSOR_RES_7, NULL},
    {MENU_VIDEO_SENSOR_RES_8, 0,
        STR_VIDEO_RES_8, 0, 0,
        MENU_VIDEO_SENSOR_RES_8, NULL},
    {MENU_VIDEO_SENSOR_RES_9, 0,
        STR_VIDEO_RES_9, 0, 0,
        MENU_VIDEO_SENSOR_RES_9, NULL},
    {MENU_VIDEO_SENSOR_RES_10, 0,
        STR_VIDEO_RES_10, 0, 0,
        MENU_VIDEO_SENSOR_RES_10, NULL},
    {MENU_VIDEO_SENSOR_RES_11, 0,
        STR_VIDEO_RES_11, 0, 0,
        MENU_VIDEO_SENSOR_RES_11, NULL},
    {MENU_VIDEO_SENSOR_RES_12, 0,
        STR_VIDEO_RES_12, 0, 0,
        MENU_VIDEO_SENSOR_RES_12, NULL},
    {MENU_VIDEO_SENSOR_RES_13, 0,
        STR_VIDEO_RES_13, 0, 0,
        MENU_VIDEO_SENSOR_RES_13, NULL},
    {MENU_VIDEO_SENSOR_RES_14, 0,
        STR_VIDEO_RES_14, 0, 0,
        MENU_VIDEO_SENSOR_RES_14, NULL},
    {MENU_VIDEO_SENSOR_RES_15, 0,
        STR_VIDEO_RES_15, 0, 0,
        MENU_VIDEO_SENSOR_RES_15, NULL},
    {MENU_VIDEO_SENSOR_RES_16, 0,
        STR_VIDEO_RES_16, 0, 0,
        MENU_VIDEO_SENSOR_RES_16, NULL},
    {MENU_VIDEO_SENSOR_RES_17, 0,
        STR_VIDEO_RES_17, 0, 0,
        MENU_VIDEO_SENSOR_RES_17, NULL},
    {MENU_VIDEO_SENSOR_RES_18, 0,
        STR_VIDEO_RES_18, 0, 0,
        MENU_VIDEO_SENSOR_RES_18, NULL},
    {MENU_VIDEO_SENSOR_RES_19, 0,
        STR_VIDEO_RES_19, 0, 0,
        MENU_VIDEO_SENSOR_RES_19, NULL},
    {MENU_VIDEO_SENSOR_RES_20, 0,
        STR_VIDEO_RES_20, 0, 0,
        MENU_VIDEO_SENSOR_RES_20, NULL},
    {MENU_VIDEO_SENSOR_RES_21, 0,
        STR_VIDEO_RES_21, 0, 0,
        MENU_VIDEO_SENSOR_RES_21, NULL},
    {MENU_VIDEO_SENSOR_RES_22, 0,
        STR_VIDEO_RES_22, 0, 0,
        MENU_VIDEO_SENSOR_RES_22, NULL},
    {MENU_VIDEO_SENSOR_RES_23, 0,
        STR_VIDEO_RES_23, 0, 0,
        MENU_VIDEO_SENSOR_RES_23, NULL},
    {MENU_VIDEO_SENSOR_RES_24, 0,
        STR_VIDEO_RES_24, 0, 0,
        MENU_VIDEO_SENSOR_RES_24, NULL},
    {MENU_VIDEO_SENSOR_RES_25, 0,
        STR_VIDEO_RES_25, 0, 0,
        MENU_VIDEO_SENSOR_RES_25, NULL},
    {MENU_VIDEO_SENSOR_RES_26, 0,
        STR_VIDEO_RES_26, 0, 0,
        MENU_VIDEO_SENSOR_RES_26, NULL},
    {MENU_VIDEO_SENSOR_RES_27, 0,
        STR_VIDEO_RES_27, 0, 0,
        MENU_VIDEO_SENSOR_RES_27, NULL},
    {MENU_VIDEO_SENSOR_RES_28, 0,
        STR_VIDEO_RES_28, 0, 0,
        MENU_VIDEO_SENSOR_RES_28, NULL},
    {MENU_VIDEO_SENSOR_RES_29, 0,
        STR_VIDEO_RES_29, 0, 0,
        MENU_VIDEO_SENSOR_RES_29, NULL},
    {MENU_VIDEO_SENSOR_RES_30, 0,
        STR_VIDEO_RES_30, 0, 0,
        MENU_VIDEO_SENSOR_RES_30, NULL},
    {MENU_VIDEO_SENSOR_RES_31, 0,
        STR_VIDEO_RES_31, 0, 0,
        MENU_VIDEO_SENSOR_RES_31, NULL},
    {MENU_VIDEO_SENSOR_RES_32, 0,
        STR_VIDEO_RES_32, 0, 0,
        MENU_VIDEO_SENSOR_RES_32, NULL}
};

static MENU_SEL_s menu_video_yuv_res_sel_tbl[MENU_VIDEO_YUV_RES_SEL_NUM] = {
    {MENU_VIDEO_YUV_RES_1, 0,
        STR_VIDEO_RES_1, 0, 0,
        MENU_VIDEO_YUV_RES_1, NULL},
    {MENU_VIDEO_YUV_RES_2, 0,
        STR_VIDEO_RES_2, 0, 0,
        MENU_VIDEO_YUV_RES_2, NULL},
    {MENU_VIDEO_YUV_RES_3, 0,
        STR_VIDEO_RES_3, 0, 0,
        MENU_VIDEO_YUV_RES_3, NULL},
    {MENU_VIDEO_YUV_RES_4, 0,
        STR_VIDEO_RES_4, 0, 0,
        MENU_VIDEO_YUV_RES_4, NULL},
    {MENU_VIDEO_YUV_RES_5, 0,
        STR_VIDEO_RES_5, 0, 0,
        MENU_VIDEO_YUV_RES_5, NULL},
    {MENU_VIDEO_YUV_RES_6, 0,
        STR_VIDEO_RES_6, 0, 0,
        MENU_VIDEO_YUV_RES_6, NULL},
    {MENU_VIDEO_YUV_RES_7, 0,
        STR_VIDEO_RES_7, 0, 0,
        MENU_VIDEO_YUV_RES_7, NULL},
    {MENU_VIDEO_YUV_RES_8, 0,
        STR_VIDEO_RES_8, 0, 0,
        MENU_VIDEO_YUV_RES_8, NULL},
    {MENU_VIDEO_YUV_RES_9, 0,
        STR_VIDEO_RES_9, 0, 0,
        MENU_VIDEO_YUV_RES_9, NULL},
    {MENU_VIDEO_YUV_RES_10, 0,
        STR_VIDEO_RES_10, 0, 0,
        MENU_VIDEO_YUV_RES_10, NULL},
    {MENU_VIDEO_YUV_RES_11, 0,
        STR_VIDEO_RES_11, 0, 0,
        MENU_VIDEO_YUV_RES_11, NULL},
    {MENU_VIDEO_YUV_RES_12, 0,
        STR_VIDEO_RES_12, 0, 0,
        MENU_VIDEO_YUV_RES_12, NULL},
    {MENU_VIDEO_YUV_RES_13, 0,
        STR_VIDEO_RES_13, 0, 0,
        MENU_VIDEO_YUV_RES_13, NULL},
    {MENU_VIDEO_YUV_RES_14, 0,
        STR_VIDEO_RES_14, 0, 0,
        MENU_VIDEO_YUV_RES_14, NULL},
    {MENU_VIDEO_YUV_RES_15, 0,
        STR_VIDEO_RES_15, 0, 0,
        MENU_VIDEO_YUV_RES_15, NULL},
    {MENU_VIDEO_YUV_RES_16, 0,
        STR_VIDEO_RES_16, 0, 0,
        MENU_VIDEO_YUV_RES_16, NULL},
    {MENU_VIDEO_YUV_RES_17, 0,
        STR_VIDEO_RES_17, 0, 0,
        MENU_VIDEO_YUV_RES_17, NULL},
    {MENU_VIDEO_YUV_RES_18, 0,
        STR_VIDEO_RES_18, 0, 0,
        MENU_VIDEO_YUV_RES_18, NULL},
    {MENU_VIDEO_YUV_RES_19, 0,
        STR_VIDEO_RES_19, 0, 0,
        MENU_VIDEO_YUV_RES_19, NULL},
    {MENU_VIDEO_YUV_RES_20, 0,
        STR_VIDEO_RES_20, 0, 0,
        MENU_VIDEO_YUV_RES_20, NULL},
    {MENU_VIDEO_YUV_RES_21, 0,
        STR_VIDEO_RES_21, 0, 0,
        MENU_VIDEO_YUV_RES_21, NULL},
    {MENU_VIDEO_YUV_RES_22, 0,
        STR_VIDEO_RES_22, 0, 0,
        MENU_VIDEO_YUV_RES_22, NULL},
    {MENU_VIDEO_YUV_RES_23, 0,
        STR_VIDEO_RES_23, 0, 0,
        MENU_VIDEO_YUV_RES_23, NULL},
    {MENU_VIDEO_YUV_RES_24, 0,
        STR_VIDEO_RES_24, 0, 0,
        MENU_VIDEO_YUV_RES_24, NULL},
    {MENU_VIDEO_YUV_RES_25, 0,
        STR_VIDEO_RES_25, 0, 0,
        MENU_VIDEO_YUV_RES_25, NULL},
    {MENU_VIDEO_YUV_RES_26, 0,
        STR_VIDEO_RES_26, 0, 0,
        MENU_VIDEO_YUV_RES_26, NULL},
    {MENU_VIDEO_YUV_RES_27, 0,
        STR_VIDEO_RES_27, 0, 0,
        MENU_VIDEO_YUV_RES_27, NULL},
    {MENU_VIDEO_YUV_RES_28, 0,
        STR_VIDEO_RES_28, 0, 0,
        MENU_VIDEO_YUV_RES_28, NULL},
    {MENU_VIDEO_YUV_RES_29, 0,
        STR_VIDEO_RES_29, 0, 0,
        MENU_VIDEO_YUV_RES_29, NULL},
    {MENU_VIDEO_YUV_RES_30, 0,
        STR_VIDEO_RES_30, 0, 0,
        MENU_VIDEO_YUV_RES_30, NULL},
    {MENU_VIDEO_YUV_RES_31, 0,
        STR_VIDEO_RES_31, 0, 0,
        MENU_VIDEO_YUV_RES_31, NULL},
    {MENU_VIDEO_YUV_RES_32, 0,
        STR_VIDEO_RES_32, 0, 0,
        MENU_VIDEO_YUV_RES_32, NULL}
};

static MENU_SEL_s menu_video_quality_sel_tbl[MENU_VIDEO_QUALITY_SEL_NUM] = {
    {MENU_VIDEO_QUALITY_SFINE, MENU_SEL_FLAGS_ENABLE,
        STR_QUALITY_SFINE, BMP_ICN_QUALITY_SF, 0,
        VIDEO_QUALITY_SFINE, NULL},
    {MENU_VIDEO_QUALITY_FINE, MENU_SEL_FLAGS_ENABLE,
        STR_QUALITY_FINE, BMP_ICN_QUALITY_F, 0,
        VIDEO_QUALITY_FINE, NULL},
    {MENU_VIDEO_QUALITY_NORMAL, MENU_SEL_FLAGS_ENABLE,
        STR_QUALITY_NORMAL, BMP_ICN_QUALITY_N, 0,
        VIDEO_QUALITY_NORMAL, NULL}
};

#ifdef VIDEO_TIMELAPES_ENABLE
static MENU_SEL_s menu_video_selftimer_sel_tbl[MENU_VIDEO_SELFTIMER_SEL_NUM] = {
    {MENU_VIDEO_SELFTIMER, MENU_SEL_FLAGS_ENABLE,
        STR_SELF_TIMER_OFF, BMP_ICN_SELF_TIMER_OFF, 0,
        SELF_TIMER_OFF, NULL},
    {MENU_VIDEO_SELFTIMER, MENU_SEL_FLAGS_ENABLE,
        STR_SELF_TIMER_3S, BMP_ICN_SELF_TIMER_3, 0,
        SELF_TIMER_3S, NULL},
    {MENU_VIDEO_SELFTIMER, MENU_SEL_FLAGS_ENABLE,
        STR_SELF_TIMER_5S, BMP_ICN_SELF_TIMER_5, 0,
        SELF_TIMER_5S, NULL},
    {MENU_VIDEO_SELFTIMER, MENU_SEL_FLAGS_ENABLE,
        STR_SELF_TIMER_10S, BMP_ICN_SELF_TIMER_10, 0,
        SELF_TIMER_10S, NULL}
};
#else
static MENU_SEL_s menu_video_selftimer_sel_tbl[MENU_VIDEO_SELFTIMER_SEL_NUM] = {
    {MENU_VIDEO_SELFTIMER, MENU_SEL_FLAGS_ENABLE,
        STR_SELF_TIMER_OFF, 0, 0,
        SELF_TIMER_OFF, NULL},
    {MENU_VIDEO_SELFTIMER, MENU_SEL_FLAGS_ENABLE,
        STR_SELF_TIMER_3S, 0, 0,
        SELF_TIMER_3S, NULL},
    {MENU_VIDEO_SELFTIMER, MENU_SEL_FLAGS_ENABLE,
        STR_SELF_TIMER_5S, 0, 0,
        SELF_TIMER_5S, NULL},
    {MENU_VIDEO_SELFTIMER, MENU_SEL_FLAGS_ENABLE,
        STR_SELF_TIMER_10S, 0, 0,
        SELF_TIMER_10S, NULL}
};
#endif

static MENU_SEL_s menu_video_pre_record_sel_tbl[MENU_VIDEO_PRE_RECORD_SEL_NUM] = {
    {MENU_VIDEO_PRE_RECORD_OFF, MENU_SEL_FLAGS_ENABLE,
        STR_PRE_RECORD_OFF, BMP_ICN_VIDEO_FILM, 0,
        VIDEO_PRE_RECORD_OFF, NULL},
    {MENU_VIDEO_PRE_RECORD_ON, MENU_SEL_FLAGS_ENABLE,
        STR_PRE_RECORD_ON, BMP_ICN_VIDEO_PRE_RECORD, 0,
        VIDEO_PRE_RECORD_ON, NULL}
};

#ifdef VIDEO_TIMELAPES_ENABLE
static MENU_SEL_s menu_video_time_lapse_sel_tbl[MENU_VIDEO_TIME_LAPSE_SEL_NUM] = {
    {MENU_VIDEO_TIME_LAPSE_OFF, MENU_SEL_FLAGS_ENABLE,
        STR_TIME_LAPSE_OFF, BMP_ICN_VIDEO_FILM, 0,
        VIDEO_TIME_LAPSE_OFF, NULL},
    {MENU_VIDEO_TIME_LAPSE_2S, MENU_SEL_FLAGS_ENABLE,
        STR_TIME_LAPSE_2S, BMP_ICN_VIDEO_TIME_LAPSE_2, 0,
        VIDEO_TIME_LAPSE_2S, NULL}
};
#else
static MENU_SEL_s menu_video_time_lapse_sel_tbl[MENU_VIDEO_TIME_LAPSE_SEL_NUM] = {
    {MENU_VIDEO_TIME_LAPSE_OFF, MENU_SEL_FLAGS_ENABLE,
        STR_TIME_LAPSE_OFF, BMP_ICN_VIDEO_FILM, 0,
        VIDEO_TIME_LAPSE_OFF, NULL},
    {MENU_VIDEO_TIME_LAPSE_2S, MENU_SEL_FLAGS_ENABLE,
        STR_TIME_LAPSE_2S, 0, 0,
        VIDEO_TIME_LAPSE_2S, NULL}
};
#endif

static MENU_SEL_s menu_video_dual_streams_sel_tbl[MENU_VIDEO_DUAL_STREAMS_SEL_NUM] = {
    {MENU_VIDEO_DUAL_STREAMS_OFF, MENU_SEL_FLAGS_ENABLE,
        STR_DUAL_STREAMS_OFF, BMP_ICN_VIDEO_FILM, 0,
        VIDEO_DUAL_STREAMS_OFF, NULL},
    {MENU_VIDEO_DUAL_STREAMS_ON, MENU_SEL_FLAGS_ENABLE,
        STR_DUAL_STREAMS_ON, BMP_ICN_VIDEO_DUAL, 0,
        VIDEO_DUAL_STREAMS_ON, NULL}
};

static MENU_SEL_s menu_video_streams_type_sel_tbl[MENU_VIDEO_STREAMS_TYPE_SEL_NUM] = {
    {MENU_VIDEO_STREAMS_TYPE_OFF, MENU_SEL_FLAGS_ENABLE,
        STR_STREAM_TYPE_OFF, BMP_ICN_STREAMING_OFF, 0,
        STREAM_TYPE_OFF, NULL},
#if defined(CONFIG_APP_AMBA_LINK)
    {MENU_VIDEO_STREAMS_TYPE_RTSP, MENU_SEL_FLAGS_ENABLE,
        STR_STREAM_TYPE_RTSP, BMP_ICN_STREAMING_RTSP, 0,
        STREAM_TYPE_RTSP, NULL},
#endif
#if defined(CONFIG_AMBA_STREAMING)
    {MENU_VIDEO_STREAMS_TYPE_MJPG, MENU_SEL_FLAGS_ENABLE,
        STR_STREAM_TYPE_MJPEG, BMP_ICN_STREAMING_MJPG, 0,
        STREAM_TYPE_MJPEG, NULL},
    {MENU_VIDEO_STREAMS_TYPE_UVC_MJPG, MENU_SEL_FLAGS_ENABLE,
        STR_STREAM_TYPE_UVC, BMP_ICN_STREAMING_MJPG, 0,
        STREAM_TYPE_UVC, NULL},
    {MENU_VIDEO_STREAMS_TYPE_HLS, MENU_SEL_FLAGS_ENABLE,
        STR_STREAM_TYPE_HLS, BMP_ICN_STREAMING_HLS, 0,
        STREAM_TYPE_HLS, NULL}
#endif
};

static MENU_SEL_s menu_video_streaming_sel_tbl[MENU_VIDEO_STREAMING_SEL_NUM] = {
    {MENU_VIDEO_STREAMING_OFF, MENU_SEL_FLAGS_ENABLE,
        STR_STREAMING_OFF, BMP_ICN_VIDEO_FILM, 0,
        STREAMING_OFF, NULL},
    {MENU_VIDEO_STREAMING_ON, MENU_SEL_FLAGS_ENABLE,
        STR_STREAMING_ON, BMP_ICN_VIDEO_DUAL, 0,
        STREAMING_ON, NULL}
};

static MENU_SEL_s menu_video_digital_zoom_sel_tbl[MENU_VIDEO_DIGITAL_ZOOM_SEL_NUM] = {
    {MENU_VIDEO_DIGITAL_ZOOM_OFF, MENU_SEL_FLAGS_ENABLE,
        STR_DIGITAL_ZOOM_OFF, BMP_ICN_DIGITAL_ZOOM_OFF, 0,
        DZOOM_OFF, NULL},
    {MENU_VIDEO_DIGITAL_ZOOM_ON, MENU_SEL_FLAGS_ENABLE,
        STR_DIGITAL_ZOOM_ON, BMP_ICN_DIGITAL_ZOOM_4X, 0,
        DZOOM_4X, NULL},
    {MENU_VIDEO_DIGITAL_ZOOM_NUMX, 0,
        STR_DIGITAL_ZOOM_120X, BMP_ICN_DIGITAL_ZOOM_120X, 0,
        DZOOM_120X, NULL}
};

static MENU_SEL_s menu_video_stamp_sel_tbl[MENU_VIDEO_STAMP_SEL_NUM] = {
    {MENU_VIDEO_STAMP_OFF, MENU_SEL_FLAGS_ENABLE,
        STR_STAMP_OFF, BMP_ICN_STAMP_OFF, 0,
        STAMP_OFF, NULL},
    {MENU_VIDEO_STAMP_DATE, MENU_SEL_FLAGS_ENABLE,
        STR_STAMP_DATE, BMP_ICN_STAMP_DATE, 0,
        STAMP_DATE, NULL},
    {MENU_VIDEO_STAMP_TIME, MENU_SEL_FLAGS_ENABLE,
        STR_STAMP_TIME, BMP_ICN_STAMP_TIME, 0,
        STAMP_TIME, NULL},
    {MENU_VIDEO_STAMP_BOTH, MENU_SEL_FLAGS_ENABLE,
        STR_STAMP_DATE_TIME, BMP_ICN_STAMP_DATE_TIME, 0,
        STAMP_DATE_TIME, NULL}
};

static MENU_SEL_s menu_video_rec_mode_sel_tbl[MENU_VIDEO_REC_MODE_SEL_NUM] = {
    {MENU_VIDEO_REC_MODE_VIDEO_MODE, MENU_SEL_FLAGS_ENABLE,
        STR_VIDEO_MODE, BMP_ICN_VIDEO_FILM, 0,
        MENU_VIDEO_REC_MODE_VIDEO_MODE, NULL},
    {MENU_VIDEO_REC_MODE_STILL_MODE, MENU_SEL_FLAGS_ENABLE,
        STR_PHOTO_MODE, BMP_ICN_PRECISE_QUALITY, 0,
        MENU_VIDEO_REC_MODE_STILL_MODE, NULL}
};


static MENU_SEL_s menu_video_adas_calibration_sel_tbl[MENU_VIDEO_ADAS_CALIBRATION_SEL_NUM] = {
    {MENU_VIDEO_ADAS_CALIBRATION_SET, MENU_SEL_FLAGS_ENABLE,
        STR_ADAS_CALIBRATION, BMP_ICN_AUTO_POWER_OFF, 0,
        ADAS_ON, NULL}
};
#ifdef CONFIG_APP_ARD
static MENU_SEL_s menu_video_motion_detect_sel_tbl[MENU_VIDEO_MOTION_DETECT_SEL_NUM] = {
    {MENU_VIDEO_MOTION_DETECT_OFF, MENU_SEL_FLAGS_ENABLE,
        STR_MOTION_DETECT_OFF, BMP_ICN_MOTION_DETECT_OFF, 0,
        MOTION_DETECT_OFF, NULL},
    {MENU_VIDEO_MOTION_DETECT_ON, MENU_SEL_FLAGS_ENABLE,
        STR_MOTION_DETECT_ON, BMP_ICN_MOTION_DETECT_ON, 0,
        MOTION_DETECT_ON, NULL}
};

static MENU_SEL_s menu_video_gsensor_sensitivity_sel_tbl[MENU_VIDEO_GSENSOR_SENSITIVITY_SEL_NUM] = {
    {MENU_VIDEO_GSENSOR_OFF, MENU_SEL_FLAGS_ENABLE,
        STR_G_SENSOR_SENSORTIVITY_OFF, BMP_ICN_G_SENSOR_OFF, 0,
        MENU_VIDEO_GSENSOR_OFF, NULL},
    {MENU_VIDEO_GSENSOR_SENSITIVITY_H, MENU_SEL_FLAGS_ENABLE,
        STR_G_SENSOR_SENSORTIVITY_HIGH, BMP_ICN_G_SENSOR_HIGH, 0,
        MENU_VIDEO_GSENSOR_SENSITIVITY_H, NULL},
    {MENU_VIDEO_GSENSOR_SENSITIVITY_M, MENU_SEL_FLAGS_ENABLE,
        STR_G_SENSOR_SENSORTIVITY_MEDIUM, BMP_ICN_G_SENSOR_MEDIUM, 0,
        MENU_VIDEO_GSENSOR_SENSITIVITY_M, NULL},
    {MENU_VIDEO_GSENSOR_SENSITIVITY_L, MENU_SEL_FLAGS_ENABLE,
        STR_G_SENSOR_SENSORTIVITY_LOW, BMP_ICN_G_SENSOR_LOW, 0,
        MENU_VIDEO_GSENSOR_SENSITIVITY_L, NULL},
};

static MENU_SEL_s menu_video_parkingmode_sensitivity_sel_tbl[MENU_VIDEO_PARKINGMODE_SENSITIVITY_SEL_NUM] = {
    {MENU_VIDEO_PARKINGMODE_OFF, MENU_SEL_FLAGS_ENABLE,
        STR_PARKING_MODE_OFF, BMP_ICN_PARKING_MODE_OFF, 0,
        MENU_VIDEO_PARKINGMODE_OFF, NULL},
    {MENU_VIDEO_PARKINGMODE_ON, MENU_SEL_FLAGS_ENABLE,
        STR_PARKING_MODE_ON, BMP_ICN_PARKING_MODE_ON, 0,
        MENU_VIDEO_PARKINGMODE_ON, NULL},
};

static MENU_SEL_s menu_video_split_rec_sel_tbl[MENU_VIDEO_SPLIT_REC_MODE_SEL_NUM] = {
    {MENU_VIDEO_SPLIT_REC_MODE_1MIN, MENU_SEL_FLAGS_ENABLE,
        STR_SPLIT_RECORD_1_MIN, BMP_ICN_LOOP_RECORD_1_MIN_SHORT, 0,
        MENU_VIDEO_SPLIT_REC_MODE_1MIN, NULL},
    //{MENU_VIDEO_SPLIT_REC_MODE_2MIN, 0,
       // STR_SPLIT_RECORD_2_MIN, BMP_ICN_LOOP_RECORD_2_MIN_SHORT, 0,
       // MENU_VIDEO_SPLIT_REC_MODE_2MIN, NULL},
    {MENU_VIDEO_SPLIT_REC_MODE_3MIN, MENU_SEL_FLAGS_ENABLE,
        STR_SPLIT_RECORD_3_MIN, BMP_ICN_LOOP_RECORD_3_MIN_SHORT, 0,
        MENU_VIDEO_SPLIT_REC_MODE_3MIN, NULL},
    {MENU_VIDEO_SPLIT_REC_MODE_5MIN, MENU_SEL_FLAGS_ENABLE,
        STR_SPLIT_RECORD_5_MIN, BMP_ICN_LOOP_RECORD_5_MIN_SHORT, 0,
        MENU_VIDEO_SPLIT_REC_MODE_5MIN, NULL},
    //{MENU_VIDEO_SPLIT_REC_MODE_OFF, MENU_SEL_FLAGS_ENABLE,
        //STR_SPLIT_RECORD_OFF, BMP_ICN_AUTO_POWER_OFF, 0,
        //MENU_VIDEO_SPLIT_REC_MODE_OFF,NULL},
};

static MENU_SEL_s menu_video_micphone_setup_sel_tbl[MENU_VIDEO_MICPHONE_SETUP_SEL_NUM] = {
    {MENU_VIDEO_MICPHONE_SETUP_OFF, MENU_SEL_FLAGS_ENABLE,
        STR_MIC_MUTE_OFF, BMP_MIC_MUTE_ON, 0,
        MIC_MUTE_OFF, NULL},
    {MENU_VIDEO_MICPHONE_SETUP_ON, MENU_SEL_FLAGS_ENABLE,
        STR_MIC_MUTE_ON, BMP_MIC_MUTE_OFF, 0,
        MIC_MUTE_ON, NULL}
};

static MENU_SEL_s menu_video_flicker_sel_tbl[MENU_VIDEO_FLICKER_SEL_NUM] = {
    {MENU_VIDEO_FLICKER_AUTO, 0,    // hide it
        STR_FLICKER_AUTO, BMP_ICN_FLICKER_AUTO, 0,
        ANTI_FLICKER_AUTO, NULL},
    {MENU_VIDEO_FLICKER_60HZ, MENU_SEL_FLAGS_ENABLE,
        STR_FLICKER_60HZ, BMP_ICN_FLICKER_60HZ, 0,
        ANTI_FLICKER_60HZ, NULL},
    {MENU_VIDEO_FLICKER_50HZ, MENU_SEL_FLAGS_ENABLE,
        STR_FLICKER_50HZ, BMP_ICN_FLICKER_50HZ, 0,
        ANTI_FLICKER_50HZ, NULL}
};

#endif

static MENU_SEL_s *menu_video_item_sel_tbls[MENU_VIDEO_ITEM_NUM] = {
    menu_video_sensor_res_sel_tbl,
    menu_video_yuv_res_sel_tbl,
    menu_video_quality_sel_tbl,
    menu_video_selftimer_sel_tbl,
    menu_video_pre_record_sel_tbl,
    menu_video_time_lapse_sel_tbl,
    menu_video_dual_streams_sel_tbl,
    menu_video_streams_type_sel_tbl,
    menu_video_streaming_sel_tbl,
    menu_video_digital_zoom_sel_tbl,
    menu_video_stamp_sel_tbl,
    menu_video_rec_mode_sel_tbl,
    //menu_video_adas_sel_tbl,
    menu_video_adas_calibration_sel_tbl,
#ifdef CONFIG_APP_ARD
    menu_video_motion_detect_sel_tbl,
    menu_video_gsensor_sensitivity_sel_tbl,
    menu_video_parkingmode_sensitivity_sel_tbl,
    menu_video_split_rec_sel_tbl,
    NULL,
    menu_video_micphone_setup_sel_tbl,
    menu_video_flicker_sel_tbl,
#endif
};

/*** Currently activated object id arrays ***/
static MENU_SEL_s *menu_video_sensor_res_sels[MENU_VIDEO_SENSOR_RES_SEL_NUM];
static MENU_SEL_s *menu_video_yuv_res_sels[MENU_VIDEO_YUV_RES_SEL_NUM];
static MENU_SEL_s *menu_video_quality_sels[MENU_VIDEO_QUALITY_SEL_NUM];
static MENU_SEL_s *menu_video_selftimer_sels[MENU_VIDEO_SELFTIMER_SEL_NUM];
static MENU_SEL_s *menu_video_pre_record_sels[MENU_VIDEO_PRE_RECORD_SEL_NUM];
static MENU_SEL_s *menu_video_time_lapse_sels[MENU_VIDEO_TIME_LAPSE_SEL_NUM];
static MENU_SEL_s *menu_video_dual_streams_sels[MENU_VIDEO_DUAL_STREAMS_SEL_NUM];
static MENU_SEL_s *menu_video_streams_type_sels[MENU_VIDEO_STREAMS_TYPE_SEL_NUM];
static MENU_SEL_s *menu_video_streaming_sels[MENU_VIDEO_STREAMING_SEL_NUM];
static MENU_SEL_s *menu_video_digital_zoom_sels[MENU_VIDEO_DIGITAL_ZOOM_SEL_NUM];
static MENU_SEL_s *menu_video_stamp_sels[MENU_VIDEO_STAMP_SEL_NUM];
static MENU_SEL_s *menu_video_rec_mode_sels[MENU_VIDEO_REC_MODE_SEL_NUM];
static MENU_SEL_s *menu_video_adas_calibration_sels[MENU_VIDEO_ADAS_CALIBRATION_SEL_NUM];
#ifdef CONFIG_APP_ARD
static MENU_SEL_s *menu_video_motion_detect_sels[MENU_VIDEO_MOTION_DETECT_SEL_NUM];
static MENU_SEL_s *menu_video_gsensor_sensitivity_sels[MENU_VIDEO_GSENSOR_SENSITIVITY_SEL_NUM];
static MENU_SEL_s *menu_video_parkingmode_sensitivity_sels[MENU_VIDEO_PARKINGMODE_SENSITIVITY_SEL_NUM];
static MENU_SEL_s *menu_video_split_rec_sels[MENU_VIDEO_SPLIT_REC_MODE_SEL_NUM];
static MENU_SEL_s *menu_video_micphone_setup_sels[MENU_VIDEO_MICPHONE_SETUP_SEL_NUM];
static MENU_SEL_s *menu_video_flicker_sels[MENU_VIDEO_FLICKER_SEL_NUM];
#endif


/*** Item ***/
static MENU_ITEM_s menu_video_sensor_res = {
    MENU_VIDEO_SENSOR_RES, 0,//MENU_ITEM_FLAGS_ENABLE,
    0, 0,
    0, 0,
    0, 0, 0, menu_video_sensor_res_sels,
    menu_video_sensor_res_init,
    menu_video_sensor_res_get_tab_str,
    menu_video_sensor_res_get_sel_str,
    menu_video_sensor_res_get_sel_bmp,
    menu_video_sensor_res_set,
    menu_video_sensor_res_sel_set
};

static MENU_ITEM_s menu_video_yuv_res = {
    MENU_VIDEO_YUV_RES, 0,
    0, 0,
    0, 0,
    0, 0, 0, menu_video_yuv_res_sels,
    menu_video_yuv_res_init,
    menu_video_yuv_res_get_tab_str,
    menu_video_yuv_res_get_sel_str,
    menu_video_yuv_res_get_sel_bmp,
    menu_video_yuv_res_set,
    menu_video_yuv_res_sel_set
};

static MENU_ITEM_s menu_video_quality = {
    MENU_VIDEO_QUALITY, 0,   //MENU_ITEM_FLAGS_ENABLE
    0, 0,
    0, 0,
    0, 0, 0, menu_video_quality_sels,
    menu_video_quality_init,
    menu_video_quality_get_tab_str,
    menu_video_quality_get_sel_str,
    menu_video_quality_get_sel_bmp,
    menu_video_quality_set,
    menu_video_quality_sel_set
};

static MENU_ITEM_s menu_video_selftimer = {
    MENU_VIDEO_SELFTIMER, 0,
    0, 0,
    0, 0,
    0, 0, 0, menu_video_selftimer_sels,
    menu_video_selftimer_init,
    menu_video_selftimer_get_tab_str,
    menu_video_selftimer_get_sel_str,
    menu_video_selftimer_get_sel_bmp,
    menu_video_selftimer_set,
    menu_video_selftimer_sel_set
};

static MENU_ITEM_s menu_video_pre_record = {
#ifdef CONFIG_APP_ARD
    MENU_VIDEO_PRE_RECORD, 0,
#else
    MENU_VIDEO_PRE_RECORD, 0,
#endif
    0, 0,
    0, 0,
    0, 0, 0, menu_video_pre_record_sels,
    menu_video_pre_record_init,
    menu_video_pre_record_get_tab_str,
    menu_video_pre_record_get_sel_str,
    menu_video_pre_record_get_sel_bmp,
    menu_video_pre_record_set,
    menu_video_pre_record_sel_set
};

static MENU_ITEM_s menu_video_time_lapse = {
    MENU_VIDEO_TIME_LAPSE, 0,
    0, 0,
    0, 0,
    0, 0, 0, menu_video_time_lapse_sels,
    menu_video_time_lapse_init,
    menu_video_time_lapse_get_tab_str,
    menu_video_time_lapse_get_sel_str,
    menu_video_time_lapse_get_sel_bmp,
    menu_video_time_lapse_set,
    menu_video_time_lapse_sel_set
};

static MENU_ITEM_s menu_video_dual_streams = {
    MENU_VIDEO_DUAL_STREAMS, 0,
    0, 0,
    0, 0,
    0, 0, 0, menu_video_dual_streams_sels,
    menu_video_dual_streams_init,
    menu_video_dual_streams_get_tab_str,
    menu_video_dual_streams_get_sel_str,
    menu_video_dual_streams_get_sel_bmp,
    menu_video_dual_streams_set,
    menu_video_dual_streams_sel_set
};

static MENU_ITEM_s menu_video_streams_type = {
    MENU_VIDEO_STREAMS_TYPE, 0,
    0, 0,
    0, 0,
    0, 0, 0, menu_video_streams_type_sels,
    menu_video_streams_type_init,
    menu_video_streams_type_get_tab_str,
    menu_video_streams_type_get_sel_str,
    menu_video_streams_type_get_sel_bmp,
    menu_video_streams_type_set,
    menu_video_streams_type_sel_set
};

static MENU_ITEM_s menu_video_streaming = {
    MENU_VIDEO_STREAMING, 0,
    0, 0,
    0, 0,
    0, 0, 0, menu_video_streaming_sels,
    menu_video_streaming_init,
    menu_video_streaming_get_tab_str,
    menu_video_streaming_get_sel_str,
    menu_video_streaming_get_sel_bmp,
    menu_video_streaming_set,
    menu_video_streaming_sel_set
};

static MENU_ITEM_s menu_video_digital_zoom = {
    MENU_VIDEO_DIGITAL_ZOOM, 0,
    0, 0,
    0, 0,
    0, 0, 0, menu_video_digital_zoom_sels,
    menu_video_digital_zoom_init,
    menu_video_digital_zoom_get_tab_str,
    menu_video_digital_zoom_get_sel_str,
    menu_video_digital_zoom_get_sel_bmp,
    menu_video_digital_zoom_set,
    menu_video_digital_zoom_sel_set
};

static MENU_ITEM_s menu_video_stamp = {
    MENU_VIDEO_STAMP, 0,
    0, 0,
    0, 0,
    0, 0, 0, menu_video_stamp_sels,
    menu_video_stamp_init,
    menu_video_stamp_get_tab_str,
    menu_video_stamp_get_sel_str,
    menu_video_stamp_get_sel_bmp,
    menu_video_stamp_set,
    menu_video_stamp_sel_set
};

static MENU_ITEM_s menu_video_rec_mode = {
#ifdef CONFIG_APP_ARD
    MENU_VIDEO_REC_MODE, 0,
#else
    MENU_VIDEO_REC_MODE, 0,//MENU_ITEM_FLAGS_ENABLE
#endif
    0, 0,
    0, 0,
    0, 0, 0, menu_video_rec_mode_sels,
    menu_video_rec_mode_init,
    menu_video_rec_mode_get_tab_str,
    menu_video_rec_mode_get_sel_str,
    menu_video_rec_mode_get_sel_bmp,
    menu_video_rec_mode_set,
    menu_video_rec_mode_sel_set
};

static MENU_ITEM_s menu_video_adas_calibration = {
    MENU_VIDEO_ADAS_CALIB, 0,  //MENU_ITEM_FLAGS_ENABLE
    0, 0,
    0, 0,
    0, 0, 0, menu_video_adas_calibration_sels,
    menu_video_adas_calibration_init,
    menu_video_adas_calibration_get_tab_str,
    menu_video_adas_calibration_get_sel_str,
    menu_video_adas_calibration_get_sel_bmp,
    menu_video_adas_calibration_set,
    menu_video_adas_calibration_sel_set
};

#ifdef CONFIG_APP_ARD
MENU_ITEM_s menu_video_motion_detect = {
#ifdef CONFIG_ECL_GUI	
    MENU_MOTION_DAT, MENU_ITEM_FLAGS_ENABLE,
#else
    MENU_VIDEO_MOTION_DETECT, MENU_ITEM_FLAGS_ENABLE,
#endif
    0, 0,
    0, 0,
    0, 0, 0, menu_video_motion_detect_sels,
    menu_video_motion_detect_init,
    menu_video_motion_detect_get_tab_str,
    menu_video_motion_detect_get_sel_str,
    menu_video_motion_detect_get_sel_bmp,
    menu_video_motion_detect_set,
    menu_video_motion_detect_sel_set
};

MENU_ITEM_s menu_video_gsensor_sensitivity = {
#ifdef CONFIG_ECL_GUI	
    MENU_GSENSOR, MENU_ITEM_FLAGS_ENABLE,
#else
    MENU_VIDEO_GSENSOR_SENSITIVITY, MENU_ITEM_FLAGS_ENABLE,
#endif    
    0, 0,
    0, 0,
    0, 0, 0, menu_video_gsensor_sensitivity_sels,
    menu_video_gsensor_sensitivity_init,
    menu_video_gsensor_sensitivity_get_tab_str,
    menu_video_gsensor_sensitivity_get_sel_str,
    menu_video_gsensor_sensitivity_get_sel_bmp,
    menu_video_gsensor_sensitivity_set,
    menu_video_gsensor_sensitivity_sel_set
};

MENU_ITEM_s menu_video_parkingmode_sensitivity = {
#ifdef CONFIG_ECL_GUI	
    MENU_PARKING_CON, MENU_ITEM_FLAGS_ENABLE,
#else	
    MENU_VIDEO_PARKINGMODE_SENSITIVITY, MENU_ITEM_FLAGS_ENABLE,
#endif    
    0, 0,
    0, 0,
    0, 0, 0, menu_video_parkingmode_sensitivity_sels,
    menu_video_parkingmode_sensitivity_init,
    menu_video_parkingmode_sensitivity_get_tab_str,
    menu_video_parkingmode_sensitivity_get_sel_str,
    menu_video_parkingmode_sensitivity_get_sel_bmp,
    menu_video_parkingmode_sensitivity_set,
    menu_video_parkingmode_sensitivity_sel_set
};

MENU_ITEM_s menu_video_split_rec = {
#ifdef CONFIG_ECL_GUI	
    MENU_SPLIT_REC, MENU_ITEM_FLAGS_ENABLE,
#else	
    MENU_VIDEO_SPLIT_REC_MODE, MENU_ITEM_FLAGS_ENABLE,
#endif    
    0, 0,
    0, 0,
    0, 0, 0, menu_video_split_rec_sels,
    menu_video_split_rec_init,
    menu_video_split_rec_get_tab_str,
    menu_video_split_rec_get_sel_str,
    menu_video_split_rec_get_sel_bmp,
    menu_video_split_rec_set,
    menu_video_split_rec_sel_set
};

static MENU_ITEM_s menu_video_stamp_choose = {
    MENU_VIDEO_STAMP_CHOOSE, 0,     // disbale stamp menu in video tab
    0, 0,
    0, 0,
    0, 0, 0, NULL,
    menu_video_stamp_choose_init,
    menu_video_stamp_choose_get_tab_str,
    NULL,
    NULL,
    menu_video_stamp_choose_set,
    NULL
};

static MENU_ITEM_s menu_video_micphone_setup = {
    MENU_VIDEO_MICPHONE_SETUP, MENU_ITEM_FLAGS_ENABLE,
    0, 0,
    0, 0,
    0, 0, 0, menu_video_micphone_setup_sels,
    menu_video_micphone_setup_init,
    menu_video_micphone_setup_get_tab_str,
    menu_video_micphone_setup_get_sel_str,
    menu_video_micphone_setup_get_sel_bmp,
    menu_video_micphone_setup_set,
    menu_video_micphone_setup_sel_set
};

MENU_ITEM_s menu_video_flicker = {
#ifdef CONFIG_ECL_GUI	
    MENU_FLICKER, MENU_ITEM_FLAGS_ENABLE,
#else	
    MENU_VIDEO_FLICKER, MENU_ITEM_FLAGS_ENABLE,
#endif    
    0, 0,
    0, 0,
    0, 0, 0, menu_video_flicker_sels,
    menu_video_flicker_init,
    menu_video_flicker_get_tab_str,
    menu_video_flicker_get_sel_str,
    menu_video_flicker_get_sel_bmp,
    menu_video_flicker_set,
    menu_video_flicker_sel_set
};

#endif

static MENU_ITEM_s *menu_video_item_tbl[MENU_VIDEO_ITEM_NUM] = {
    &menu_video_sensor_res,
    &menu_video_yuv_res,
    &menu_video_quality,
    &menu_video_selftimer,
    &menu_video_pre_record,
    &menu_video_time_lapse,
    &menu_video_dual_streams,
    &menu_video_streams_type,
    &menu_video_streaming,
    &menu_video_digital_zoom,
    &menu_video_stamp,
    &menu_video_rec_mode,
    //&menu_video_adas,
    &menu_video_adas_calibration,
#ifdef CONFIG_APP_ARD
    &menu_video_motion_detect,
    &menu_video_gsensor_sensitivity,
    &menu_video_parkingmode_sensitivity,
    &menu_video_split_rec,
    &menu_video_stamp_choose,
    &menu_video_micphone_setup,
    &menu_video_flicker,
#endif
};

/*** Currently activated object id arrays ***/
static MENU_ITEM_s *menu_video_items[MENU_VIDEO_ITEM_NUM];

/*** Tab ***/
static MENU_TAB_s menu_video = {
    MENU_VIDEO, MENU_TAB_FLAGS_ENABLE,
    0, 0,
    BMP_MENU_TAB_VIDEO, BMP_MENU_TAB_VIDEO_HL,
    menu_video_items,
    menu_video_init,
    menu_video_start,
    menu_video_stop
};

MENU_TAB_CTRL_s menu_video_ctrl = {
    menu_video_get_tab,
    menu_video_get_item,
    menu_video_get_sel,
    menu_video_set_sel_table,
    menu_video_lock_tab,
    menu_video_unlock_tab,
    menu_video_enable_item,
    menu_video_disable_item,
    menu_video_lock_item,
    menu_video_unlock_item,
    menu_video_enable_sel,
    menu_video_disable_sel,
    menu_video_lock_sel,
    menu_video_unlock_sel
};

/*** APIs ***/

// tab
static int menu_video_init(void)
{
    int i = 0;
    UINT32 cur_item_id = 0;

//#if defined(CONFIG_APP_CONNECTED_STAMP)
//    menu_video_enable_item(MENU_VIDEO_STAMP);
//#endif

#if defined APP_MD
#ifdef CONFIG_APP_ARD
    if(app_status.CurrEncMode == APP_VIDEO_ENC_MODE) {
        menu_video_enable_item(MENU_VIDEO_MOTION_DETECT);
    } else {
        menu_video_disable_item(MENU_VIDEO_MOTION_DETECT);
    }
#else
    menu_video_enable_item(MENU_VIDEO_MOTION_DETECT);
#endif
#else
    menu_video_disable_item(MENU_VIDEO_MOTION_DETECT);
    UserSetting->MotionDetectPref.MotionDetect = MOTION_DETECT_OFF;
#endif

    APP_ADDFLAGS(menu_video.Flags, MENU_TAB_FLAGS_INIT);

    if (menu_video.ItemNum > 0) {
        cur_item_id = menu_video_items[menu_video.ItemCur]->Id;
    }
    menu_video.ItemNum = 0;
    menu_video.ItemCur = 0;
    for (i=0; i<MENU_VIDEO_ITEM_NUM; i++) {
        if (APP_CHECKFLAGS(menu_video_item_tbl[i]->Flags, MENU_ITEM_FLAGS_ENABLE)) {
            menu_video_items[menu_video.ItemNum] = menu_video_item_tbl[i];
            if (cur_item_id == menu_video_item_tbl[i]->Id) {
                menu_video.ItemCur = menu_video.ItemNum;
            }
            menu_video.ItemNum++;
        }
    }

    return 0;
}

static int menu_video_start(void)
{
    return 0;
}

static int menu_video_stop(void)
{
    return 0;
}

// item
static int menu_video_sensor_res_init(void)
{
    int i = 0;
    int res_num = 0;
    UINT16 *Str;

    APP_ADDFLAGS(menu_video_sensor_res.Flags, MENU_ITEM_FLAGS_INIT);

    res_num = AppLibSysSensor_GetVideoResNum();
    for (i=0; i<MENU_VIDEO_SENSOR_RES_SEL_NUM; i++) {
        if (i < res_num) {
            APP_ADDFLAGS((menu_video_item_sel_tbls[MENU_VIDEO_SENSOR_RES]+i)->Flags, MENU_SEL_FLAGS_ENABLE);
            (menu_video_item_sel_tbls[MENU_VIDEO_SENSOR_RES]+i)->Val = AppLibSysSensor_GetVideoResID(i);
            Str = AppLibSysSensor_GetVideoResStr((menu_video_item_sel_tbls[MENU_VIDEO_SENSOR_RES]+i)->Val);
#ifdef CONFIG_APP_ARD
            AppLibGraph_UpdateStringContext(AppPref_GetLanguageID(), (menu_video_item_sel_tbls[MENU_VIDEO_SENSOR_RES]+i)->Str, Str);
#else
            AppLibGraph_UpdateStringContext(0, (menu_video_item_sel_tbls[MENU_VIDEO_SENSOR_RES]+i)->Str, Str);
#endif
            (menu_video_item_sel_tbls[MENU_VIDEO_SENSOR_RES]+i)->Bmp = AppGuiUtil_GetVideoResolutionBitmapSizeId((menu_video_item_sel_tbls[MENU_VIDEO_SENSOR_RES]+i)->Val);
            if ((menu_video_item_sel_tbls[MENU_VIDEO_SENSOR_RES]+i)->Val == SENSOR_VIDEO_RES_PHOTO) {
                (menu_video_item_sel_tbls[MENU_VIDEO_SENSOR_RES]+i)->Bmp = BMP_ICN_PRECISE_QUALITY_CONT;
            }

        } else {
            APP_REMOVEFLAGS((menu_video_item_sel_tbls[MENU_VIDEO_SENSOR_RES]+i)->Flags, MENU_SEL_FLAGS_ENABLE);
        }

    }

    menu_video_sensor_res.SelSaved = 0;
    menu_video_sensor_res.SelNum = 0;
    menu_video_sensor_res.SelCur = 0;
    for (i=0; i<MENU_VIDEO_SENSOR_RES_SEL_NUM; i++) {
        if (APP_CHECKFLAGS((menu_video_item_sel_tbls[MENU_VIDEO_SENSOR_RES]+i)->Flags, MENU_SEL_FLAGS_ENABLE)) {
            menu_video_sensor_res_sels[menu_video_sensor_res.SelNum] = menu_video_item_sel_tbls[MENU_VIDEO_SENSOR_RES]+i;
            if (menu_video_sensor_res_sels[menu_video_sensor_res.SelNum]->Val == UserSetting->VideoPref.SensorVideoRes) {
                menu_video_sensor_res.SelSaved = menu_video_sensor_res.SelNum;
                menu_video_sensor_res.SelCur = menu_video_sensor_res.SelNum;
            }
            menu_video_sensor_res.SelNum++;
        }
    }
    return 0;
}

static int menu_video_sensor_res_get_tab_str(void)
{
#ifdef CONFIG_APP_ARD
    return STR_VIDEO_RESOLUTION_SETTING;
#else
    return menu_video_sensor_res_sels[menu_video_sensor_res.SelSaved]->Str;
#endif
}

static int menu_video_sensor_res_get_sel_str(int ref)
{
    return menu_video_sensor_res_sels[ref]->Str;
}

static int menu_video_sensor_res_get_sel_bmp(int ref)
{
    return menu_video_sensor_res_sels[ref]->Bmp;
}

static int menu_video_sensor_res_set(void)
{
    AppMenuQuick_SetItem(MENU_VIDEO, MENU_VIDEO_SENSOR_RES);
    AppWidget_On(WIDGET_MENU_QUICK, 0);
    return 0;
}

static int menu_video_sensor_res_sel_set(void)
{
    if (menu_video_sensor_res.SelSaved != menu_video_sensor_res.SelCur) {
        menu_video_sensor_res.SelSaved = menu_video_sensor_res.SelCur;
        UserSetting->VideoPref.SensorVideoRes = menu_video_sensor_res.Sels[menu_video_sensor_res.SelCur]->Val;
        //app_util_video_sensor_res_pref_reset(0);
        //app_util_menu_lock_item_dual_stream_prerecord(0);
        //app_util_menu_lock_item_stamp(0);
        //app_util_menu_lock_item_time_lapse(0);
        {
            /* Send the message to the current app. */
            APP_APP_s *curapp;
            AppAppMgt_GetCurApp(&curapp);
            curapp->OnMessage(AMSG_CMD_SET_VIDEO_RES, menu_video_sensor_res.Sels[menu_video_sensor_res.SelCur]->Val, 0);
        }
    }
    return 0;
}

static int menu_video_yuv_res_init(void)
{
    int i = 0;
    int res_num = 1;
    //WCHAR *Str;

    APP_ADDFLAGS(menu_video_yuv_res.Flags, MENU_ITEM_FLAGS_INIT);

    //res_num = app_yuv_get_video_res_num();
    for (i=0; i<MENU_VIDEO_YUV_RES_SEL_NUM; i++) {
        if (i < res_num) {
            APP_ADDFLAGS((menu_video_item_sel_tbls[MENU_VIDEO_YUV_RES]+i)->Flags, MENU_SEL_FLAGS_ENABLE);
            //(menu_video_item_sel_tbls[MENU_VIDEO_YUV_RES]+i)->Val = app_yuv_get_video_res_id(i);
            //Str = app_yuv_get_video_res_str((menu_video_item_sel_tbls[MENU_VIDEO_YUV_RES]+i)->Val);
            /*{
                char str_t[YUV_VIDEO_RES_STR_LEN];
                uni_to_asc(Str, str_t);
                AmbaPrint("Update STR_VIDEO_YUV_RES_%d with %s", i, str_t);
            }*/
        } else {
            APP_REMOVEFLAGS((menu_video_item_sel_tbls[MENU_VIDEO_YUV_RES]+i)->Flags, MENU_SEL_FLAGS_ENABLE);
        }
    }

    menu_video_yuv_res.SelSaved = 0;
    menu_video_yuv_res.SelNum = 0;
    menu_video_yuv_res.SelCur = 0;
    for (i=0; i<MENU_VIDEO_YUV_RES_SEL_NUM; i++) {
        if (APP_CHECKFLAGS((menu_video_item_sel_tbls[MENU_VIDEO_YUV_RES]+i)->Flags, MENU_SEL_FLAGS_ENABLE)) {
            menu_video_yuv_res_sels[menu_video_yuv_res.SelNum] = menu_video_item_sel_tbls[MENU_VIDEO_YUV_RES]+i;
            if (menu_video_yuv_res_sels[menu_video_yuv_res.SelNum]->Val == UserSetting->VideoPref.YUVVideoRes) {
                menu_video_yuv_res.SelSaved = menu_video_yuv_res.SelNum;
                menu_video_yuv_res.SelCur = menu_video_yuv_res.SelNum;
            }
            menu_video_yuv_res.SelNum++;
        }
    }
    return 0;
}

static int menu_video_yuv_res_get_tab_str(void)
{
#ifdef CONFIG_APP_ARD
    return STR_VIDEO_RESOLUTION_SETTING;
#else
    return menu_video_yuv_res_sels[menu_video_yuv_res.SelSaved]->Str;
#endif
}

static int menu_video_yuv_res_get_sel_str(int ref)
{
    return menu_video_yuv_res_sels[ref]->Str;
}

static int menu_video_yuv_res_get_sel_bmp(int ref)
{
    return menu_video_yuv_res_sels[ref]->Bmp;
}

static int menu_video_yuv_res_set(void)
{
    AppMenuQuick_SetItem(MENU_VIDEO, MENU_VIDEO_YUV_RES);
    AppWidget_On(WIDGET_MENU_QUICK, 0);
    return 0;
}

static int menu_video_yuv_res_sel_set(void)
{
    if (menu_video_yuv_res.SelSaved != menu_video_yuv_res.SelCur) {
        menu_video_yuv_res.SelSaved = menu_video_yuv_res.SelCur;
        UserSetting->VideoPref.YUVVideoRes = menu_video_yuv_res.Sels[menu_video_yuv_res.SelCur]->Val;
        AppLibVideoEnc_SetYuvVideoRes(menu_video_yuv_res.Sels[menu_video_yuv_res.SelCur]->Val);
        {
            /* Send the message to the current app. */
            APP_APP_s *curapp;
            AppAppMgt_GetCurApp(&curapp);
            curapp->OnMessage(AMSG_CMD_SET_VIDEO_RES, menu_video_yuv_res.Sels[menu_video_yuv_res.SelCur]->Val, 0);
        }
    }
    return 0;
}

static int menu_video_quality_init(void)
{
    int i = 0;

    APP_ADDFLAGS(menu_video_quality.Flags, MENU_ITEM_FLAGS_INIT);
    menu_video_quality.SelSaved = 0;
    menu_video_quality.SelNum = 0;
    menu_video_quality.SelCur = 0;
    for (i=0; i<MENU_VIDEO_QUALITY_SEL_NUM; i++) {
        if (APP_CHECKFLAGS((menu_video_item_sel_tbls[MENU_VIDEO_QUALITY]+i)->Flags, MENU_SEL_FLAGS_ENABLE)) {
            menu_video_quality_sels[menu_video_quality.SelNum] = menu_video_item_sel_tbls[MENU_VIDEO_QUALITY]+i;
            if (menu_video_quality_sels[menu_video_quality.SelNum]->Val == UserSetting->VideoPref.VideoQuality) {
                menu_video_quality.SelSaved = menu_video_quality.SelNum;
                menu_video_quality.SelCur = menu_video_quality.SelNum;
            }
            menu_video_quality.SelNum++;
        }
    }

    return 0;
}

static int menu_video_quality_get_tab_str(void)
{
    return menu_video_quality_sels[menu_video_quality.SelSaved]->Str;
}

static int menu_video_quality_get_sel_str(int ref)
{
    return menu_video_quality_sels[ref]->Str;
}

static int menu_video_quality_get_sel_bmp(int ref)
{
    return menu_video_quality_sels[ref]->Bmp;
}

static int menu_video_quality_set(void)
{
    AppMenuQuick_SetItem(MENU_VIDEO, MENU_VIDEO_QUALITY);
    AppWidget_On(WIDGET_MENU_QUICK, 0);
    return 0;
}

static int menu_video_quality_sel_set(void)
{
    if (menu_video_quality.SelSaved != menu_video_quality.SelCur) {
        menu_video_quality.SelSaved = menu_video_quality.SelCur;
        UserSetting->VideoPref.VideoQuality = menu_video_quality.Sels[menu_video_quality.SelCur]->Val;
        {
            /* Send the message to the current app. */
            APP_APP_s *curapp;
            AppAppMgt_GetCurApp(&curapp);
            curapp->OnMessage(AMSG_CMD_SET_VIDEO_QUALITY, menu_video_quality.Sels[menu_video_quality.SelCur]->Val, 0);
        }
    }
    return 0;
}

static int menu_video_selftimer_init(void)
{
    int i = 0;

    APP_ADDFLAGS(menu_video_selftimer.Flags, MENU_ITEM_FLAGS_INIT);
    menu_video_selftimer.SelSaved = 0;
    menu_video_selftimer.SelNum = 0;
    menu_video_selftimer.SelCur = 0;
    for (i=0; i<MENU_VIDEO_SELFTIMER_SEL_NUM; i++) {
        if (APP_CHECKFLAGS((menu_video_item_sel_tbls[MENU_VIDEO_SELFTIMER]+i)->Flags, MENU_SEL_FLAGS_ENABLE)) {
            menu_video_selftimer_sels[menu_video_selftimer.SelNum] = menu_video_item_sel_tbls[MENU_VIDEO_SELFTIMER]+i;
            if (menu_video_selftimer_sels[menu_video_selftimer.SelNum]->Val == UserSetting->VideoPref.VideoSelftimer) {
                menu_video_selftimer.SelSaved = menu_video_selftimer.SelNum;
                menu_video_selftimer.SelCur = menu_video_selftimer.SelNum;
            }
            menu_video_selftimer.SelNum++;
        }
    }

    return 0;
}

static int menu_video_selftimer_get_tab_str(void)
{
    return menu_video_selftimer_sels[menu_video_selftimer.SelSaved]->Str;
}

static int menu_video_selftimer_get_sel_str(int ref)
{
    return menu_video_selftimer_sels[ref]->Str;
}

static int menu_video_selftimer_get_sel_bmp(int ref)
{
    return menu_video_selftimer_sels[ref]->Bmp;
}

static int menu_video_selftimer_set(void)
{
    AppMenuQuick_SetItem(MENU_VIDEO, MENU_VIDEO_SELFTIMER);
    AppWidget_On(WIDGET_MENU_QUICK, 0);
    return 0;
}

static int menu_video_selftimer_sel_set(void)
{
    if (menu_video_selftimer.SelSaved != menu_video_selftimer.SelCur) {
        menu_video_selftimer.SelSaved = menu_video_selftimer.SelCur;
        UserSetting->VideoPref.VideoSelftimer = (APP_PREF_SELF_TIMER_e)menu_video_selftimer.Sels[menu_video_selftimer.SelCur]->Val;
        {
            /* Send the message to the current app. */
            APP_APP_s *curapp;
            AppAppMgt_GetCurApp(&curapp);
            curapp->OnMessage(AMSG_CMD_SET_SELFTIMER, menu_video_selftimer.Sels[menu_video_selftimer.SelCur]->Val, 0);
        }
    }
    return 0;
}

static int menu_video_pre_record_init(void)
{
    int i = 0;

    APP_ADDFLAGS(menu_video_pre_record.Flags, MENU_ITEM_FLAGS_INIT);
    menu_video_pre_record.SelSaved = 0;
    menu_video_pre_record.SelNum = 0;
    menu_video_pre_record.SelCur = 0;
    for (i=0; i<MENU_VIDEO_PRE_RECORD_SEL_NUM; i++) {
        if (APP_CHECKFLAGS((menu_video_item_sel_tbls[MENU_VIDEO_PRE_RECORD]+i)->Flags, MENU_SEL_FLAGS_ENABLE)) {
            menu_video_pre_record_sels[menu_video_pre_record.SelNum] = menu_video_item_sel_tbls[MENU_VIDEO_PRE_RECORD]+i;
            if (menu_video_pre_record_sels[menu_video_pre_record.SelNum]->Val == UserSetting->VideoPref.PreRecord) {
                menu_video_pre_record.SelSaved = menu_video_pre_record.SelNum;
                menu_video_pre_record.SelCur = menu_video_pre_record.SelNum;
            }
            menu_video_pre_record.SelNum++;
        }
    }

    return 0;
}

static int menu_video_pre_record_get_tab_str(void)
{
    return menu_video_pre_record_sels[menu_video_pre_record.SelSaved]->Str;
}

static int menu_video_pre_record_get_sel_str(int ref)
{
    return menu_video_pre_record_sels[ref]->Str;
}

static int menu_video_pre_record_get_sel_bmp(int ref)
{
    return menu_video_pre_record_sels[ref]->Bmp;
}

static int menu_video_pre_record_set(void)
{
    AppMenuQuick_SetItem(MENU_VIDEO, MENU_VIDEO_PRE_RECORD);
    AppWidget_On(WIDGET_MENU_QUICK, 0);
    return 0;
}

static int menu_video_pre_record_sel_set(void)
{
    if (menu_video_pre_record.SelSaved != menu_video_pre_record.SelCur) {
        menu_video_pre_record.SelSaved = menu_video_pre_record.SelCur;
        UserSetting->VideoPref.PreRecord = menu_video_pre_record.Sels[menu_video_pre_record.SelCur]->Val;
        //app_util_menu_lock_item_time_lapse(0);
        {
            /* Send the message to the current app. */
            APP_APP_s *curapp;
            AppAppMgt_GetCurApp(&curapp);
            curapp->OnMessage(AMSG_CMD_SET_VIDEO_PRE_RECORD, menu_video_pre_record.Sels[menu_video_pre_record.SelCur]->Val, 0);
        }
    }
    return 0;
}

static int menu_video_time_lapse_init(void)
{
    int i = 0;

    APP_ADDFLAGS(menu_video_time_lapse.Flags, MENU_ITEM_FLAGS_INIT);
    menu_video_time_lapse.SelSaved = 0;
    menu_video_time_lapse.SelNum = 0;
    menu_video_time_lapse.SelCur = 0;
    for (i=0; i<MENU_VIDEO_TIME_LAPSE_SEL_NUM; i++) {
        if (APP_CHECKFLAGS((menu_video_item_sel_tbls[MENU_VIDEO_TIME_LAPSE]+i)->Flags, MENU_SEL_FLAGS_ENABLE)) {
            menu_video_time_lapse_sels[menu_video_time_lapse.SelNum] = menu_video_item_sel_tbls[MENU_VIDEO_TIME_LAPSE]+i;
            if (menu_video_time_lapse_sels[menu_video_time_lapse.SelNum]->Val == UserSetting->VideoPref.TimeLapse) {
                menu_video_time_lapse.SelSaved = menu_video_time_lapse.SelNum;
                menu_video_time_lapse.SelCur = menu_video_time_lapse.SelNum;
            }
            menu_video_time_lapse.SelNum++;
        }
    }

    return 0;
}

static int menu_video_time_lapse_get_tab_str(void)
{
    return menu_video_time_lapse_sels[menu_video_time_lapse.SelSaved]->Str;
}

static int menu_video_time_lapse_get_sel_str(int ref)
{
    return menu_video_time_lapse_sels[ref]->Str;
}

static int menu_video_time_lapse_get_sel_bmp(int ref)
{
    return menu_video_time_lapse_sels[ref]->Bmp;
}

static int menu_video_time_lapse_set(void)
{
    AppMenuQuick_SetItem(MENU_VIDEO, MENU_VIDEO_TIME_LAPSE);
    AppWidget_On(WIDGET_MENU_QUICK, 0);
    return 0;
}

static int menu_video_time_lapse_sel_set(void)
{
    if (menu_video_time_lapse.SelSaved != menu_video_time_lapse.SelCur) {
        menu_video_time_lapse.SelSaved = menu_video_time_lapse.SelCur;
        UserSetting->VideoPref.TimeLapse = menu_video_time_lapse.Sels[menu_video_time_lapse.SelCur]->Val;
        //app_util_menu_lock_item_dual_stream_prerecord(0);
        //app_util_menu_lock_item_scene_sshutter(0);
        {
            /* Send the message to the current app. */
            APP_APP_s *curapp;
            AppAppMgt_GetCurApp(&curapp);
            curapp->OnMessage(AMSG_CMD_SET_VIDEO_TIME_LAPSE, menu_video_time_lapse.Sels[menu_video_time_lapse.SelCur]->Val, 0);
        }
    }
    return 0;
}

static int menu_video_dual_streams_init(void)
{
    int i = 0;

    APP_ADDFLAGS(menu_video_dual_streams.Flags, MENU_ITEM_FLAGS_INIT);
    menu_video_dual_streams.SelSaved = 0;
    menu_video_dual_streams.SelNum = 0;
    menu_video_dual_streams.SelCur = 0;
    for (i=0; i<MENU_VIDEO_DUAL_STREAMS_SEL_NUM; i++) {
        if (APP_CHECKFLAGS((menu_video_item_sel_tbls[MENU_VIDEO_DUAL_STREAMS]+i)->Flags, MENU_SEL_FLAGS_ENABLE)) {
            menu_video_dual_streams_sels[menu_video_dual_streams.SelNum] = menu_video_item_sel_tbls[MENU_VIDEO_DUAL_STREAMS]+i;
            if (menu_video_dual_streams_sels[menu_video_dual_streams.SelNum]->Val == UserSetting->VideoPref.DualStreams) {
                menu_video_dual_streams.SelSaved = menu_video_dual_streams.SelNum;
                menu_video_dual_streams.SelCur = menu_video_dual_streams.SelNum;
            }
            menu_video_dual_streams.SelNum++;
        }
    }

    return 0;
}

static int menu_video_dual_streams_get_tab_str(void)
{
    if (APP_CHECKFLAGS(menu_video_dual_streams.Flags, MENU_ITEM_FLAGS_LOCKED)) {
        return STR_DUAL_STREAMS_OFF;
    } else {
        return menu_video_dual_streams_sels[menu_video_dual_streams.SelSaved]->Str;
    }
}

static int menu_video_dual_streams_get_sel_str(int ref)
{
    return menu_video_dual_streams_sels[ref]->Str;
}

static int menu_video_dual_streams_get_sel_bmp(int ref)
{
    return menu_video_dual_streams_sels[ref]->Bmp;
}

static int menu_video_dual_streams_set(void)
{
    AppMenuQuick_SetItem(MENU_VIDEO, MENU_VIDEO_DUAL_STREAMS);
    AppWidget_On(WIDGET_MENU_QUICK, 0);
    return 0;
}

static int menu_video_dual_streams_sel_set(void)
{
    if (menu_video_dual_streams.SelSaved != menu_video_dual_streams.SelCur) {
        menu_video_dual_streams.SelSaved = menu_video_dual_streams.SelCur;
        UserSetting->VideoPref.DualStreams = menu_video_dual_streams.Sels[menu_video_dual_streams.SelCur]->Val;
        //app_util_video_sensor_res_pref_reset(0);
        //app_util_menu_lock_item_time_lapse(0);
        //app_util_menu_lock_item_stamp(0);
        {
            /* Send the message to the current app. */
            APP_APP_s *curapp;
            AppAppMgt_GetCurApp(&curapp);
            curapp->OnMessage(AMSG_CMD_SET_VIDEO_DUAL_STREAMS, menu_video_dual_streams.Sels[menu_video_dual_streams.SelCur]->Val, 0);
        }
    }
    return 0;
}

static int menu_video_streams_type_init(void)
{
    int i = 0;

    APP_ADDFLAGS(menu_video_streams_type.Flags, MENU_ITEM_FLAGS_INIT);
    menu_video_streams_type.SelSaved = 0;
    menu_video_streams_type.SelNum = 0;
    menu_video_streams_type.SelCur = 0;
    for (i=0; i<MENU_VIDEO_STREAMS_TYPE_SEL_NUM; i++) {
        if (APP_CHECKFLAGS((menu_video_item_sel_tbls[MENU_VIDEO_STREAMS_TYPE]+i)->Flags, MENU_SEL_FLAGS_ENABLE)) {
            menu_video_streams_type_sels[menu_video_streams_type.SelNum] = menu_video_item_sel_tbls[MENU_VIDEO_STREAMS_TYPE]+i;
            if (menu_video_streams_type_sels[menu_video_streams_type.SelNum]->Val == UserSetting->VideoPref.StreamType) {
                menu_video_streams_type.SelSaved = menu_video_streams_type.SelNum;
                menu_video_streams_type.SelCur = menu_video_streams_type.SelNum;
            }
            menu_video_streams_type.SelNum++;
        }
    }

    return 0;
}

static int menu_video_streams_type_get_tab_str(void)
{
#if defined(CONFIG_AMBA_STREAMING)
    if (APP_CHECKFLAGS(menu_video_streams_type.Flags, MENU_ITEM_FLAGS_LOCKED) &&
        (UserSetting->SetupPref.USBMode == USB_MODE_MSC)) {
        return STR_STREAM_TYPE_OFF;
    } else
#endif
    return menu_video_streams_type_sels[menu_video_streams_type.SelSaved]->Str;
}

static int menu_video_streams_type_get_sel_str(int ref)
{
    return menu_video_streams_type_sels[ref]->Str;
}

static int menu_video_streams_type_get_sel_bmp(int ref)
{
    return menu_video_streams_type_sels[ref]->Bmp;
}

static int menu_video_streams_type_set(void)
{
    AppMenuQuick_SetItem(MENU_VIDEO, MENU_VIDEO_STREAMS_TYPE);
    AppWidget_On(WIDGET_MENU_QUICK, 0);
    return 0;
}

static int menu_video_streams_type_sel_set(void)
{
    if (menu_video_streams_type.SelSaved != menu_video_streams_type.SelCur) {
        menu_video_streams_type.SelSaved = menu_video_streams_type.SelCur;
        UserSetting->VideoPref.StreamType = (APP_PREF_STREAM_TYPE_e)menu_video_streams_type.Sels[menu_video_streams_type.SelCur]->Val;
        //app_rec_setting_set_streaming_type(UserSetting->VideoPref.StreamType);
#if defined(CONFIG_AMBA_STREAMING)
        if (UserSetting->VideoPref.StreamType == SPORT_DV_STREAM_OFF) {
            app_video_set_streaming_mode(VIDEO_STREAMING_OFF);
            if (app_video_setting_get_sensor_video_res() != UserSetting->VideoPref.SensorVideoRes) {
                /* Send the message to the current app. */
                APP_APP_s *curapp;
                AppAppMgt_GetCurApp(&curapp);
                curapp->OnMessage(AMSG_CMD_SET_VIDEO_RES, UserSetting->VideoPref.SensorVideoRes, 0);
            }
        }
#endif
        //app_util_video_sensor_res_pref_reset(0);
        //app_util_menu_lock_item_dual_stream_prerecord(0);
        //app_util_menu_lock_item_time_lapse(0);

    }
    return 0;
}

static int menu_video_streaming_init(void)
{
    int i = 0;

    APP_ADDFLAGS(menu_video_streaming.Flags, MENU_ITEM_FLAGS_INIT);
    menu_video_streaming.SelSaved = 0;
    menu_video_streaming.SelNum = 0;
    menu_video_streaming.SelCur = 0;
    for (i=0; i<MENU_VIDEO_STREAMING_SEL_NUM; i++) {
        if (APP_CHECKFLAGS((menu_video_item_sel_tbls[MENU_VIDEO_STREAMING]+i)->Flags, MENU_SEL_FLAGS_ENABLE)) {
            menu_video_streaming_sels[menu_video_streaming.SelNum] = menu_video_item_sel_tbls[MENU_VIDEO_STREAMING]+i;
            if (menu_video_streaming_sels[menu_video_streaming.SelNum]->Val == UserSetting->VideoPref.Streaming) {
                menu_video_streaming.SelSaved = menu_video_streaming.SelNum;
                menu_video_streaming.SelCur = menu_video_streaming.SelNum;
            }
            menu_video_streaming.SelNum++;
        }
    }

    return 0;
}

static int menu_video_streaming_get_tab_str(void)
{
    return menu_video_streaming_sels[menu_video_streaming.SelSaved]->Str;
}

static int menu_video_streaming_get_sel_str(int ref)
{
    return menu_video_streaming_sels[ref]->Str;
}

static int menu_video_streaming_get_sel_bmp(int ref)
{
    return menu_video_streaming_sels[ref]->Bmp;
}

static int menu_video_streaming_set(void)
{
    AppMenuQuick_SetItem(MENU_VIDEO, MENU_VIDEO_STREAMING);
    AppWidget_On(WIDGET_MENU_QUICK, 0);
    return 0;
}

static int menu_video_streaming_sel_set(void)
{
    if (menu_video_streaming.SelSaved != menu_video_streaming.SelCur) {
        menu_video_streaming.SelSaved = menu_video_streaming.SelCur;
        UserSetting->VideoPref.Streaming = (APP_PREF_STREAMING_STATUS_e)menu_video_streaming.Sels[menu_video_streaming.SelCur]->Val;
        //app_util_video_sensor_res_pref_reset(0);
        //app_util_menu_lock_item_time_lapse(0);
        //app_util_menu_lock_item_stamp(0);
#if defined(CONFIG_AMBA_STREAMING)
        if (UserSetting->VideoPref.Streaming) {
            app_video_set_streaming_mode(VIDEO_STREAMING_OUT_3RD);
        } else {
            app_video_set_streaming_mode(VIDEO_STREAMING_OFF);
        }
        app_rec_setup_file_format(0);
        //AmbaPrintColor(RED,"UserSetting->streaming = %d",UserSetting->streaming);
        {
            /* Send the message to the current app. */
            APP_APP_s *curapp;
            AppAppMgt_GetCurApp(&curapp);
            curapp->OnMessage(AMSG_CMD_SET_STREAMING, menu_video_streaming.Sels[menu_video_streaming.SelCur]->Val, 0);
        }
#endif
    }
    return 0;
}

static int menu_video_digital_zoom_init(void)
{
    int i = 0;

    APP_ADDFLAGS(menu_video_digital_zoom.Flags, MENU_ITEM_FLAGS_INIT);
    menu_video_digital_zoom.SelSaved = 0;
    menu_video_digital_zoom.SelNum = 0;
    menu_video_digital_zoom.SelCur = 0;
    for (i=0; i<MENU_VIDEO_DIGITAL_ZOOM_SEL_NUM; i++) {
        if (APP_CHECKFLAGS((menu_video_item_sel_tbls[MENU_VIDEO_DIGITAL_ZOOM]+i)->Flags, MENU_SEL_FLAGS_ENABLE)) {
            menu_video_digital_zoom_sels[menu_video_digital_zoom.SelNum] = menu_video_item_sel_tbls[MENU_VIDEO_DIGITAL_ZOOM]+i;
            if (menu_video_digital_zoom_sels[menu_video_digital_zoom.SelNum]->Val == UserSetting->VideoPref.VideoDZoom) {
                menu_video_digital_zoom.SelSaved = menu_video_digital_zoom.SelNum;
                menu_video_digital_zoom.SelCur = menu_video_digital_zoom.SelNum;
            }
            menu_video_digital_zoom.SelNum++;
        }
    }

    return 0;
}

static int menu_video_digital_zoom_get_tab_str(void)
{
    return menu_video_digital_zoom_sels[menu_video_digital_zoom.SelSaved]->Str;
}

static int menu_video_digital_zoom_get_sel_str(int ref)
{
    return menu_video_digital_zoom_sels[ref]->Str;
}

static int menu_video_digital_zoom_get_sel_bmp(int ref)
{
    return menu_video_digital_zoom_sels[ref]->Bmp;
}

static int menu_video_digital_zoom_set(void)
{
    AppMenuQuick_SetItem(MENU_VIDEO, MENU_VIDEO_DIGITAL_ZOOM);
    AppWidget_On(WIDGET_MENU_QUICK, 0);
    return 0;
}

static int menu_video_digital_zoom_sel_set(void)
{
    if (menu_video_digital_zoom.SelSaved != menu_video_digital_zoom.SelCur) {
        menu_video_digital_zoom.SelSaved = menu_video_digital_zoom.SelCur;
        UserSetting->VideoPref.VideoDZoom = (APP_PREF_DZOOM_e)menu_video_digital_zoom.Sels[menu_video_digital_zoom.SelCur]->Val;
        //app_dzoom_setting_set_mode(menu_video_digital_zoom.Sels[menu_video_digital_zoom.SelCur]->Val);
        {
            /* Send the message to the current app. */
            APP_APP_s *curapp;
            AppAppMgt_GetCurApp(&curapp);
            curapp->OnMessage(AMSG_CMD_SET_DIGITAL_ZOOM, menu_video_digital_zoom.Sels[menu_video_digital_zoom.SelCur]->Val, 0);
        }
    }
    return 0;
}

static int menu_video_stamp_init(void)
{
    int i = 0;

    APP_ADDFLAGS(menu_video_stamp.Flags, MENU_ITEM_FLAGS_INIT);
    menu_video_stamp.SelSaved = 0;
    menu_video_stamp.SelNum = 0;
    menu_video_stamp.SelCur = 0;
    for (i=0; i<MENU_VIDEO_STAMP_SEL_NUM; i++) {
        if (APP_CHECKFLAGS((menu_video_item_sel_tbls[MENU_VIDEO_STAMP]+i)->Flags, MENU_SEL_FLAGS_ENABLE)) {
            menu_video_stamp_sels[menu_video_stamp.SelNum] = menu_video_item_sel_tbls[MENU_VIDEO_STAMP]+i;
            if (menu_video_stamp_sels[menu_video_stamp.SelNum]->Val == UserSetting->VideoPref.VideoDateTimeStamp) {
                menu_video_stamp.SelSaved = menu_video_stamp.SelNum;
                menu_video_stamp.SelCur = menu_video_stamp.SelNum;
            }
            menu_video_stamp.SelNum++;
        }
    }

    return 0;
}

static int menu_video_stamp_get_tab_str(void)
{
    return menu_video_stamp_sels[menu_video_stamp.SelSaved]->Str;
}

static int menu_video_stamp_get_sel_str(int ref)
{
    return menu_video_stamp_sels[ref]->Str;
}

static int menu_video_stamp_get_sel_bmp(int ref)
{
    return menu_video_stamp_sels[ref]->Bmp;
}

static int menu_video_stamp_set(void)
{
    AppMenuQuick_SetItem(MENU_VIDEO, MENU_VIDEO_STAMP);
    AppWidget_On(WIDGET_MENU_QUICK, 0);
    return 0;
}

static int menu_video_stamp_sel_set(void)
{
    if (menu_video_stamp.SelSaved != menu_video_stamp.SelCur) {
        menu_video_stamp.SelSaved = menu_video_stamp.SelCur;
        UserSetting->VideoPref.VideoDateTimeStamp = (APP_PREF_TIME_STAMP_e)menu_video_stamp_sels[menu_video_stamp.SelCur]->Val;
    }
    //app_util_menu_lock_item_dual_stream_prerecord(0);

    return 0;
}

static int menu_video_rec_mode_init(void)
{
    int i = 0;

    APP_ADDFLAGS(menu_video_rec_mode.Flags, MENU_ITEM_FLAGS_INIT);
    menu_video_rec_mode.SelSaved = 0;
    menu_video_rec_mode.SelNum = 0;
    menu_video_rec_mode.SelCur = 0;


    for (i=0; i<MENU_VIDEO_REC_MODE_SEL_NUM; i++) {
        if (APP_CHECKFLAGS((menu_video_item_sel_tbls[MENU_VIDEO_REC_MODE]+i)->Flags, MENU_SEL_FLAGS_ENABLE)) {
            menu_video_rec_mode_sels[menu_video_rec_mode.SelNum] = menu_video_item_sel_tbls[MENU_VIDEO_REC_MODE]+i;
            if (menu_video_rec_mode_sels[menu_video_rec_mode.SelNum]->Val == app_status.CurrEncMode) {
                menu_video_rec_mode.SelSaved = menu_video_rec_mode.SelNum;
                menu_video_rec_mode.SelCur = menu_video_rec_mode.SelNum;
            }
            menu_video_rec_mode.SelNum++;
        }
    }

    return 0;
}

static int menu_video_rec_mode_get_tab_str(void)
{
    return menu_video_rec_mode_sels[menu_video_rec_mode.SelSaved]->Str;
}

static int menu_video_rec_mode_get_sel_str(int ref)
{
    return menu_video_rec_mode_sels[ref]->Str;
}

static int menu_video_rec_mode_get_sel_bmp(int ref)
{
    return menu_video_rec_mode_sels[ref]->Bmp;
}

static int menu_video_rec_mode_set(void)
{
    AppMenuQuick_SetItem(MENU_VIDEO, MENU_VIDEO_REC_MODE);
    AppWidget_On(WIDGET_MENU_QUICK, 0);
    return 0;
}

static int menu_video_rec_mode_sel_set(void)
{
    if (menu_video_rec_mode.SelSaved != menu_video_rec_mode.SelCur) {
        menu_video_rec_mode.SelSaved = menu_video_rec_mode.SelCur;
        {
            /* Send the message to the current app. */
            APP_APP_s *curapp;
            AppAppMgt_GetCurApp(&curapp);
            curapp->OnMessage(AMSG_CMD_SET_RECORD_MODE, menu_video_rec_mode.Sels[menu_video_rec_mode.SelCur]->Val, 0);
        }
    }
    return 0;
}

static int menu_video_adas_calibration_init(void)
{
    int i = 0;

    APP_ADDFLAGS(menu_video_adas_calibration.Flags, MENU_ITEM_FLAGS_INIT);
    menu_video_adas_calibration.SelSaved = 0;
    menu_video_adas_calibration.SelNum = 0;
    menu_video_adas_calibration.SelCur = 0;

    for (i=0; i<MENU_VIDEO_ADAS_CALIBRATION_SEL_NUM; i++) {
        if (APP_CHECKFLAGS((menu_video_item_sel_tbls[MENU_VIDEO_ADAS_CALIB]+i)->Flags, MENU_SEL_FLAGS_ENABLE)) {
            menu_video_adas_calibration_sels[menu_video_adas_calibration.SelNum] = menu_video_item_sel_tbls[MENU_VIDEO_ADAS_CALIB]+i;
            menu_video_adas_calibration.SelNum++;
        }
    }

    return 0;
}

static int menu_video_adas_calibration_get_tab_str(void)
{
    return menu_video_adas_calibration_sels[menu_video_adas_calibration.SelSaved]->Str;
}

static int menu_video_adas_calibration_get_sel_str(int ref)
{
    return menu_video_adas_calibration_sels[ref]->Str;
}

static int menu_video_adas_calibration_get_sel_bmp(int ref)
{
    return menu_video_adas_calibration_sels[ref]->Bmp;
}

static int menu_video_adas_calibration_set(void)
{
    AppMenuQuick_SetItem(MENU_VIDEO, MENU_VIDEO_ADAS_CALIB);
    AppWidget_On(WIDGET_MENU_ADAS_CALIB, 0);
    return 0;
}

static int menu_video_adas_calibration_sel_set(void)
{
    return 0;
}

#ifdef CONFIG_APP_ARD
static int menu_video_motion_detect_init(void)
{
    int i = 0;

    APP_ADDFLAGS(menu_video_motion_detect.Flags, MENU_ITEM_FLAGS_INIT);
    menu_video_motion_detect.SelSaved = 0;
    menu_video_motion_detect.SelNum = 0;
    menu_video_motion_detect.SelCur = 0;


    for (i=0; i<MENU_VIDEO_MOTION_DETECT_SEL_NUM; i++) {
        if (APP_CHECKFLAGS((menu_video_item_sel_tbls[MENU_VIDEO_MOTION_DETECT]+i)->Flags, MENU_SEL_FLAGS_ENABLE)) {
            menu_video_motion_detect_sels[menu_video_motion_detect.SelNum] = menu_video_item_sel_tbls[MENU_VIDEO_MOTION_DETECT]+i;
            if (menu_video_motion_detect_sels[menu_video_motion_detect.SelNum]->Val == UserSetting->MotionDetectPref.MotionDetect) {
                menu_video_motion_detect.SelSaved = menu_video_motion_detect.SelNum;
                menu_video_motion_detect.SelCur = menu_video_motion_detect.SelNum;
            }
            menu_video_motion_detect.SelNum++;
        }
    }

    return 0;
}

static int menu_video_motion_detect_get_tab_str(void)
{
    return menu_video_motion_detect_sels[menu_video_motion_detect.SelSaved]->Str;
}

static int menu_video_motion_detect_get_sel_str(int ref)
{
    return menu_video_motion_detect_sels[ref]->Str;
}

static int menu_video_motion_detect_get_sel_bmp(int ref)
{
    return menu_video_motion_detect_sels[ref]->Bmp;
}

static int menu_video_motion_detect_set(void)
{
    AppMenuQuick_SetItem(MENU_RECORDER_SETTTING, MENU_MOTION_DAT);
    AppWidget_On(WIDGET_MENU_QUICK, 0);
    return 0;
}

static int menu_video_motion_detect_sel_set(void)
{
    int Rval = 0;
    APPLIB_MD_CFG_t Config = {0};
    int pre_dis;	

    if (menu_video_motion_detect.SelSaved != menu_video_motion_detect.SelCur) {
        menu_video_motion_detect.SelSaved = menu_video_motion_detect.SelCur;
        pre_dis = UserSetting->MotionDetectPref.MotionDetect;		
        UserSetting->MotionDetectPref.MotionDetect = menu_video_motion_detect.Sels[menu_video_motion_detect.SelCur]->Val;
        record_item_sel(UserSetting->MotionDetectPref.MotionDetect,pre_dis);
        show_icn(UserSetting->MotionDetectPref.MotionDetect,0);//show detect motion icn

        if(MOTION_DETECT_ON == UserSetting->MotionDetectPref.MotionDetect) {
            Config.Method = APPLIB_MD_AE;
            Config.RoiData[0].Location.X = 0;
            Config.RoiData[0].Location.Y = 0;
            Config.RoiData[0].Location.W = 12;
            Config.RoiData[0].Location.H = 8;
            Config.RoiData[0].MDSensitivity = ADAS_SL_MEDIUM;
            Rval = AppLibVideoAnal_MD_Init(0, &Config);

            rec_cam.MotionDetectStatus = REC_CAP_MOTION_DETECT_STOP;
            AmbaPrintColor(YELLOW, "--------AppLibVideoAnal_MD_Init return = %d", Rval);
        } else {
            Rval = AppLibVideoAnal_MD_Disable();
            AmbaPrintColor(YELLOW, "--------AppLibVideoAnal_MD_Disable return = %d", Rval);

            Rval = AppLibVideoAnal_MD_DeInit();
            AmbaPrintColor(YELLOW, "--------AppLibVideoAnal_MD_DeInit return = %d", Rval);
        }
    }
    return 0;
}

static int menu_video_gsensor_sensitivity_init(void)
{
    int i = 0;

    APP_ADDFLAGS(menu_video_gsensor_sensitivity.Flags, MENU_ITEM_FLAGS_INIT);
    menu_video_gsensor_sensitivity.SelSaved = 0;
    menu_video_gsensor_sensitivity.SelNum = 0;
    menu_video_gsensor_sensitivity.SelCur = 0;
    for (i=0; i<MENU_VIDEO_GSENSOR_SENSITIVITY_SEL_NUM; i++) {
        if (APP_CHECKFLAGS((menu_video_item_sel_tbls[MENU_VIDEO_GSENSOR_SENSITIVITY]+i)->Flags, MENU_SEL_FLAGS_ENABLE)) {
            menu_video_gsensor_sensitivity_sels[menu_video_gsensor_sensitivity.SelNum] = menu_video_item_sel_tbls[MENU_VIDEO_GSENSOR_SENSITIVITY]+i;
            if (menu_video_gsensor_sensitivity_sels[menu_video_gsensor_sensitivity.SelNum]->Val == UserSetting->GSensorSentivityPref.Gsensor_sensitivity) {
                menu_video_gsensor_sensitivity.SelSaved = menu_video_gsensor_sensitivity.SelNum;
                menu_video_gsensor_sensitivity.SelCur = menu_video_gsensor_sensitivity.SelNum;
            }
            menu_video_gsensor_sensitivity.SelNum++;
        }
    }

    return 0;
}

static int menu_video_gsensor_sensitivity_get_tab_str(void)
{
    return STR_G_SENSOR_SENSORTIVITY_SETTING;
}

static int menu_video_gsensor_sensitivity_get_sel_str(int ref)
{
    return menu_video_gsensor_sensitivity_sels[ref]->Str;
}

static int menu_video_gsensor_sensitivity_get_sel_bmp(int ref)
{
    return menu_video_gsensor_sensitivity_sels[ref]->Bmp;
}

static int menu_video_gsensor_sensitivity_set(void)
{
    //app_widget_menu_quick_set_item_title_str(STR_G_SENSOR_SENSORTIVITY_SETTING);

    AppMenuQuick_SetItem(MENU_RECORDER_SETTTING, MENU_GSENSOR);
    AppWidget_On(WIDGET_MENU_QUICK, 0);

    return 0;
}

static int menu_video_gsensor_sensitivity_sel_set(void)
{
    int pre_dis;
	
    if (menu_video_gsensor_sensitivity.SelSaved != menu_video_gsensor_sensitivity.SelCur) {
        menu_video_gsensor_sensitivity.SelSaved = menu_video_gsensor_sensitivity.SelCur;
	  pre_dis = UserSetting->GSensorSentivityPref.Gsensor_sensitivity;	
        UserSetting->GSensorSentivityPref.Gsensor_sensitivity = menu_video_gsensor_sensitivity.Sels[menu_video_gsensor_sensitivity.SelCur]->Val;
        record_item_sel(UserSetting->GSensorSentivityPref.Gsensor_sensitivity,pre_dis);	  	
        AppLibComSvcHcmgr_SendMsg(AMSG_CMD_SET_VIDEO_GSENSOR_SENSITIVITY, menu_video_gsensor_sensitivity.Sels[menu_video_gsensor_sensitivity.SelCur]->Val, 0);

    }
    return 0;
}

static int menu_video_parkingmode_sensitivity_init(void)
{
    int i = 0;

    APP_ADDFLAGS(menu_video_parkingmode_sensitivity.Flags, MENU_ITEM_FLAGS_INIT);
    menu_video_parkingmode_sensitivity.SelSaved = 0;
    menu_video_parkingmode_sensitivity.SelNum = 0;
    menu_video_parkingmode_sensitivity.SelCur = 0;
    for (i=0; i<MENU_VIDEO_PARKINGMODE_SENSITIVITY_SEL_NUM; i++) {
        if (APP_CHECKFLAGS((menu_video_item_sel_tbls[MENU_VIDEO_PARKINGMODE_SENSITIVITY]+i)->Flags, MENU_SEL_FLAGS_ENABLE)) {
            menu_video_parkingmode_sensitivity_sels[menu_video_parkingmode_sensitivity.SelNum] = menu_video_item_sel_tbls[MENU_VIDEO_PARKINGMODE_SENSITIVITY]+i;
            if (menu_video_parkingmode_sensitivity_sels[menu_video_parkingmode_sensitivity.SelNum]->Val == UserSetting->VideoPref.parkingmode_sensitivity) {
                menu_video_parkingmode_sensitivity.SelSaved = menu_video_parkingmode_sensitivity.SelNum;
                menu_video_parkingmode_sensitivity.SelCur = menu_video_parkingmode_sensitivity.SelNum;
            }
            menu_video_parkingmode_sensitivity.SelNum++;
        }
    }

    return 0;
}

static int menu_video_parkingmode_sensitivity_get_tab_str(void)
{
    return STR_PARKING_MODE;
}

static int menu_video_parkingmode_sensitivity_get_sel_str(int ref)
{
    return menu_video_parkingmode_sensitivity_sels[ref]->Str;
}

static int menu_video_parkingmode_sensitivity_get_sel_bmp(int ref)
{
    return menu_video_parkingmode_sensitivity_sels[ref]->Bmp;
}

static int menu_video_parkingmode_sensitivity_set(void)
{
//  app_widget_menu_quick_set_item_title_str(STR_PARKING_MODE,menu_video_parkingmode_sensitivity.sel_num);

    AppMenuQuick_SetItem(MENU_RECORDER_SETTTING, MENU_PARKING_CON);
    AppWidget_On(WIDGET_MENU_QUICK, 0);
    return 0;
}

static int menu_video_parkingmode_sensitivity_sel_set(void)
{
    int pre_dis;

    if (menu_video_parkingmode_sensitivity.SelSaved != menu_video_parkingmode_sensitivity.SelCur) {
        menu_video_parkingmode_sensitivity.SelSaved = menu_video_parkingmode_sensitivity.SelCur;
        pre_dis = UserSetting->VideoPref.parkingmode_sensitivity;		
        UserSetting->VideoPref.parkingmode_sensitivity = menu_video_parkingmode_sensitivity.Sels[menu_video_parkingmode_sensitivity.SelCur]->Val;
        record_item_sel(UserSetting->VideoPref.parkingmode_sensitivity,pre_dis);	
    }
    return 0;
}



static int menu_video_split_rec_init(void)
{
    int i = 0;

    APP_ADDFLAGS(menu_video_split_rec.Flags, MENU_ITEM_FLAGS_INIT);
    menu_video_split_rec.SelSaved = 0;
    menu_video_split_rec.SelNum = 0;
    menu_video_split_rec.SelCur = 0;
    for (i=0; i<MENU_VIDEO_SPLIT_REC_MODE_SEL_NUM; i++) {
        if (APP_CHECKFLAGS((menu_video_item_sel_tbls[MENU_VIDEO_SPLIT_REC_MODE]+i)->Flags, MENU_SEL_FLAGS_ENABLE)) {
            menu_video_split_rec_sels[menu_video_split_rec.SelNum] = menu_video_item_sel_tbls[MENU_VIDEO_SPLIT_REC_MODE]+i;
            if (menu_video_split_rec_sels[menu_video_split_rec.SelNum]->Val == UserSetting->VideoPref.video_split_rec_time) {
                menu_video_split_rec.SelSaved = menu_video_split_rec.SelNum;
                menu_video_split_rec.SelCur = menu_video_split_rec.SelNum;
            }
            menu_video_split_rec.SelNum++;
        }
    }

    return 0;
}

static int menu_video_split_rec_get_tab_str(void)
{
    return STR_SPLIT_RECORD;
}

static int menu_video_split_rec_get_sel_str(int ref)
{
    return menu_video_split_rec_sels[ref]->Str;
}

static int menu_video_split_rec_get_sel_bmp(int ref)
{
    return menu_video_split_rec_sels[ref]->Bmp;
}

static int menu_video_split_rec_set(void)
{
    AppMenuQuick_SetItem(MENU_RECORDER_SETTTING, MENU_SPLIT_REC);
    AppWidget_On(WIDGET_MENU_QUICK, 0);
    return 0;
}

static int menu_video_split_rec_sel_set(void)
{
    int pre_dis;

    if (menu_video_split_rec.SelSaved != menu_video_split_rec.SelCur) {
        menu_video_split_rec.SelSaved = menu_video_split_rec.SelCur;
	 pre_dis = UserSetting->VideoPref.video_split_rec_time;	
        UserSetting->VideoPref.video_split_rec_time = menu_video_split_rec.Sels[menu_video_split_rec.SelCur]->Val;
        record_item_sel(UserSetting->VideoPref.video_split_rec_time,pre_dis);				
    }

    {
        APP_APP_s *curapp;
        AppAppMgt_GetCurApp(&curapp);
        curapp->OnMessage(AMSG_CMD_RESET_SPLIT_TIME, UserSetting->VideoPref.video_split_rec_time, 0);
    }
    return 0;
}

static int VideoStampItemSet(int item_idx)
{
    UserSetting->VideoStampPref.StampDate = Video_Stamp_Choose_Item[MENU_VIDEO_STAMP_CHOOSE_DATE].check_or_not;
    UserSetting->VideoStampPref.StampTime = Video_Stamp_Choose_Item[MENU_VIDEO_STAMP_CHOOSE_TIME].check_or_not;
    UserSetting->VideoStampPref.StampDriverId = Video_Stamp_Choose_Item[MENU_VIDEO_STAMP_CHOOSE_DRIVERID].check_or_not;
    return 0;
}

static int menu_video_stamp_choose_init(void)
{
    Video_Stamp_Choose_Item[MENU_VIDEO_STAMP_CHOOSE_DATE].item_id = MENU_VIDEO_STAMP_CHOOSE_DATE;
    Video_Stamp_Choose_Item[MENU_VIDEO_STAMP_CHOOSE_DATE].string_id = STR_STAMP_DATE;
    Video_Stamp_Choose_Item[MENU_VIDEO_STAMP_CHOOSE_DATE].check_or_not= UserSetting->VideoStampPref.StampDate;

    Video_Stamp_Choose_Item[MENU_VIDEO_STAMP_CHOOSE_TIME].item_id = MENU_VIDEO_STAMP_CHOOSE_TIME;
    Video_Stamp_Choose_Item[MENU_VIDEO_STAMP_CHOOSE_TIME].string_id = STR_STAMP_TIME;
    Video_Stamp_Choose_Item[MENU_VIDEO_STAMP_CHOOSE_TIME].check_or_not= UserSetting->VideoStampPref.StampTime;

    //Video_Stamp_Choose_Item[MENU_VIDEO_STAMP_CHOOSE_GPS].item_id = MENU_SETUP_STAMP_CHOOSE_GPS;
    //Video_Stamp_Choose_Item[MENU_VIDEO_STAMP_CHOOSE_GPS].string_id = STR_STAMP_GPS;
    //Video_Stamp_Choose_Item[MENU_VIDEO_STAMP_CHOOSE_GPS].check_or_not= 1;

    //Video_Stamp_Choose_Item[MENU_VIDEO_STAMP_CHOOSE_GSESNOR].item_id = MENU_SETUP_STAMP_CHOOSE_GSESNOR;
    //Video_Stamp_Choose_Item[MENU_VIDEO_STAMP_CHOOSE_GSESNOR].string_id = STR_STAMP_G_SENSOR;
    //Video_Stamp_Choose_Item[MENU_VIDEO_STAMP_CHOOSE_GSESNOR].check_or_not= 0;

    Video_Stamp_Choose_Item[MENU_VIDEO_STAMP_CHOOSE_DRIVERID].item_id = MENU_VIDEO_STAMP_CHOOSE_DRIVERID;
    Video_Stamp_Choose_Item[MENU_VIDEO_STAMP_CHOOSE_DRIVERID].string_id = STR_STAMP_DRIVER_ID;
    Video_Stamp_Choose_Item[MENU_VIDEO_STAMP_CHOOSE_DRIVERID].check_or_not= UserSetting->VideoStampPref.StampDriverId;

    return 0;
}

static int menu_video_stamp_choose_get_tab_str(void)
{
    return STR_STAMP_SETTING;
}

static int menu_video_stamp_choose_set(void)
{
    app_widget_check_box_init_item( MENU_VIDEO_STAMP_CHOOSE_NUM, &Video_Stamp_Choose_Item[0], &VideoStampItemSet);
    app_widget_menu_check_box_set_item(MENU_VIDEO, MENU_VIDEO_STAMP_CHOOSE);
    AppWidget_On(WIDGET_MENU_CKBX, 0);

    return 0;
}

static int menu_video_micphone_setup_init(void)
{
    int i = 0;

    APP_ADDFLAGS(menu_video_micphone_setup.Flags, MENU_ITEM_FLAGS_INIT);
    menu_video_micphone_setup.SelSaved = 0;
    menu_video_micphone_setup.SelNum = 0;
    menu_video_micphone_setup.SelCur = 0;


    for (i=0; i<MENU_VIDEO_MICPHONE_SETUP_SEL_NUM; i++) {
        if (APP_CHECKFLAGS((menu_video_item_sel_tbls[MENU_VIDEO_MICPHONE_SETUP]+i)->Flags, MENU_SEL_FLAGS_ENABLE)) {
            menu_video_micphone_setup_sels[menu_video_micphone_setup.SelNum] = menu_video_item_sel_tbls[MENU_VIDEO_MICPHONE_SETUP]+i;
            if (menu_video_micphone_setup_sels[menu_video_micphone_setup.SelNum]->Val == UserSetting->VideoPref.MicMute) {
                menu_video_micphone_setup.SelSaved = menu_video_micphone_setup.SelNum;
                menu_video_micphone_setup.SelCur = menu_video_micphone_setup.SelNum;
            }
            menu_video_micphone_setup.SelNum++;
        }
    }

    return 0;
}

static int menu_video_micphone_setup_get_tab_str(void)
{
    return menu_video_micphone_setup_sels[menu_video_micphone_setup.SelSaved]->Str;
}

static int menu_video_micphone_setup_get_sel_str(int ref)
{
    return menu_video_micphone_setup_sels[ref]->Str;
}

static int menu_video_micphone_setup_get_sel_bmp(int ref)
{
    return menu_video_micphone_setup_sels[ref]->Bmp;
}

static int menu_video_micphone_setup_set(void)
{
    AppMenuQuick_SetItem(MENU_VIDEO, MENU_VIDEO_MICPHONE_SETUP);
    AppWidget_On(WIDGET_MENU_QUICK, 0);
    return 0;
}

static int menu_video_micphone_setup_sel_set(void)
{
    int Rval = 0;
    APPLIB_MD_CFG_t Config = {0};

    if (menu_video_micphone_setup.SelSaved != menu_video_micphone_setup.SelCur) {
        menu_video_micphone_setup.SelSaved = menu_video_micphone_setup.SelCur;
        UserSetting->VideoPref.MicMute = menu_video_micphone_setup.Sels[menu_video_micphone_setup.SelCur]->Val;

        rec_cam.Gui(GUI_MIC_ICON_SHOW, UserSetting->VideoPref.MicMute, 0);
        rec_cam.Gui(GUI_FLUSH, 0, 0);
        AppLibAudioEnc_Mute(UserSetting->VideoPref.MicMute);
    }
    return 0;
}

static int menu_video_flicker_init(void)
{
    int i = 0;

    APP_ADDFLAGS(menu_video_flicker.Flags, MENU_ITEM_FLAGS_INIT);
    menu_video_flicker.SelSaved = 0;
    menu_video_flicker.SelNum = 0;
    menu_video_flicker.SelCur = 0;


    for (i=0; i<MENU_VIDEO_FLICKER_SEL_NUM; i++) {
        if (APP_CHECKFLAGS((menu_video_item_sel_tbls[MENU_VIDEO_FLICKER]+i)->Flags, MENU_SEL_FLAGS_ENABLE)) {
            menu_video_flicker_sels[menu_video_flicker.SelNum] = menu_video_item_sel_tbls[MENU_VIDEO_FLICKER]+i;
            if (menu_video_flicker_sels[menu_video_flicker.SelNum]->Val == UserSetting->ImagePref.Flicker) {
                menu_video_flicker.SelSaved = menu_video_flicker.SelNum;
                menu_video_flicker.SelCur = menu_video_flicker.SelNum;
            }
            menu_video_flicker.SelNum++;
        }
    }

    return 0;
}

static int menu_video_flicker_get_tab_str(void)
{
    return menu_video_flicker_sels[menu_video_flicker.SelSaved]->Str;
}

static int menu_video_flicker_get_sel_str(int ref)
{
    return menu_video_flicker_sels[ref]->Str;
}

static int menu_video_flicker_get_sel_bmp(int ref)
{
    return menu_video_flicker_sels[ref]->Bmp;
}

static int menu_video_flicker_set(void)
{
    AppMenuQuick_SetItem(MENU_RECORDER_SETTTING, MENU_FLICKER);
    AppWidget_On(WIDGET_MENU_QUICK, 0);
    return 0;
}

static int menu_video_flicker_sel_set(void)
{
    int Rval = 0;
    APPLIB_MD_CFG_t Config = {0};
#ifdef CONFIG_SENSOR_AR0230
    int VideoRes;
#endif
    int pre_dis;

    if (menu_video_flicker.SelSaved != menu_video_flicker.SelCur) {
        menu_video_flicker.SelSaved = menu_video_flicker.SelCur;
        pre_dis = UserSetting->ImagePref.Flicker - 1;		
        UserSetting->ImagePref.Flicker = menu_video_flicker.Sels[menu_video_flicker.SelCur]->Val;
        record_item_sel(UserSetting->ImagePref.Flicker-1 ,pre_dis);						
        app_status.anti_flicker_type = UserSetting->ImagePref.Flicker;
#ifdef CONFIG_SENSOR_AR0230
        VideoRes = AppLibVideoEnc_GetSensorVideoRes();
        /**if resolution is HDR set flicker mode to */
        if (VideoRes == SENSOR_VIDEO_RES_TRUE_HDR_1080P_HALF) {
            AppLibImage_SetAntiFlickerMode(ANTI_FLICKER_NO_50HZ);
        } else {
            AppLibImage_SetAntiFlickerMode(UserSetting->ImagePref.Flicker);
        }
#else
        AppLibImage_EnableAntiFlicker(1, UserSetting->ImagePref.Flicker);
#endif
    }
    return 0;
}

#endif

// control
static MENU_TAB_s* menu_video_get_tab(void)
{
    return &menu_video;
}

static MENU_ITEM_s* menu_video_get_item(UINT32 itemId)
{
    return menu_video_item_tbl[itemId];
}

static MENU_SEL_s* menu_video_get_sel(UINT32 itemId, UINT32 selId)
{
    return &menu_video_item_sel_tbls[itemId][selId];
}

static int menu_video_set_sel_table(UINT32 itemId, MENU_SEL_s *selTbl)
{
    menu_video_item_sel_tbls[itemId] = selTbl;
    APP_REMOVEFLAGS(menu_video_item_tbl[itemId]->Flags, MENU_ITEM_FLAGS_INIT);
    return 0;
}

static int menu_video_lock_tab(void)
{
    APP_ADDFLAGS(menu_video.Flags, MENU_TAB_FLAGS_LOCKED);
    return 0;
}

static int menu_video_unlock_tab(void)
{
    APP_REMOVEFLAGS(menu_video.Flags, MENU_TAB_FLAGS_LOCKED);
    return 0;
}

static int menu_video_enable_item(UINT32 itemId)
{
    if (!APP_CHECKFLAGS(menu_video_item_tbl[itemId]->Flags, MENU_ITEM_FLAGS_ENABLE)) {
        APP_ADDFLAGS(menu_video_item_tbl[itemId]->Flags, MENU_ITEM_FLAGS_ENABLE);
        APP_REMOVEFLAGS(menu_video.Flags, MENU_TAB_FLAGS_INIT);
    }
    return 0;
}

static int menu_video_disable_item(UINT32 itemId)
{
    if (APP_CHECKFLAGS(menu_video_item_tbl[itemId]->Flags, MENU_ITEM_FLAGS_ENABLE)) {
        APP_REMOVEFLAGS(menu_video_item_tbl[itemId]->Flags, MENU_ITEM_FLAGS_ENABLE);
        APP_REMOVEFLAGS(menu_video.Flags, MENU_TAB_FLAGS_INIT);
    }
    return 0;
}

static int menu_video_lock_item(UINT32 itemId)
{
    APP_ADDFLAGS(menu_video_item_tbl[itemId]->Flags, MENU_ITEM_FLAGS_LOCKED);
    return 0;
}

static int menu_video_unlock_item(UINT32 itemId)
{
    APP_REMOVEFLAGS(menu_video_item_tbl[itemId]->Flags, MENU_ITEM_FLAGS_LOCKED);
    return 0;
}

static int menu_video_enable_sel(UINT32 itemId, UINT32 selId)
{
    if (!APP_CHECKFLAGS((menu_video_item_sel_tbls[itemId]+selId)->Flags, MENU_SEL_FLAGS_ENABLE)) {
        APP_ADDFLAGS((menu_video_item_sel_tbls[itemId]+selId)->Flags, MENU_SEL_FLAGS_ENABLE);
        APP_REMOVEFLAGS(menu_video_item_tbl[itemId]->Flags, MENU_ITEM_FLAGS_INIT);
    }
    return 0;
}

static int menu_video_disable_sel(UINT32 itemId, UINT32 selId)
{
    if (APP_CHECKFLAGS((menu_video_item_sel_tbls[itemId]+selId)->Flags, MENU_SEL_FLAGS_ENABLE)) {
        APP_REMOVEFLAGS((menu_video_item_sel_tbls[itemId]+selId)->Flags, MENU_SEL_FLAGS_ENABLE);
        APP_REMOVEFLAGS(menu_video_item_tbl[itemId]->Flags, MENU_ITEM_FLAGS_INIT);
    }
    return 0;
}

static int menu_video_lock_sel(UINT32 itemId, UINT32 selId)
{
    APP_ADDFLAGS((menu_video_item_sel_tbls[itemId]+selId)->Flags, MENU_SEL_FLAGS_LOCKED);
    return 0;
}

static int menu_video_unlock_sel(UINT32 itemId, UINT32 selId)
{
    APP_REMOVEFLAGS((menu_video_item_sel_tbls[itemId]+selId)->Flags, MENU_SEL_FLAGS_LOCKED);
    return 0;
}
