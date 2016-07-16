/**
  * @file src/app/apps/flow/widget/menu/connectedcam/menu_photo.c
  *
  * Implementation of Photo-related Menu Items
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
#include <apps/apps.h>
#include <system/status.h>
#include <system/app_pref.h>
#include <system/app_util.h>
#include <apps/flow/widget/menu/widget.h>
#include <apps/gui/resource/gui_settle.h>
#include <apps/flow/widget/menu/widget.h>
#ifdef CONFIG_APP_ARD
#include <apps/flow/widget/menu/menu_check_box.h>
#include <apps/flow/rec/rec_cam.h>
#endif


/*************************************************************************
 * Declaration:
 ************************************************************************/
// tab
static int menu_photo_init(void);
static int menu_photo_start(void);
static int menu_photo_stop(void);
// item
static int menu_photo_multi_cap_init(void);
static int menu_photo_multi_cap_get_tab_str(void);
static int menu_photo_multi_cap_get_sel_str(int ref);
static int menu_photo_multi_cap_get_sel_bmp(int ref);
static int menu_photo_multi_cap_set(void);
static int menu_photo_multi_cap_sel_set(void);
static int menu_photo_cap_mode_init(void);
static int menu_photo_cap_mode_get_tab_str(void);
static int menu_photo_cap_mode_get_sel_str(int ref);
static int menu_photo_cap_mode_get_sel_bmp(int ref);
static int menu_photo_cap_mode_set(void);
static int menu_photo_cap_mode_sel_set(void);
static int menu_photo_size_init(void);
static int menu_photo_size_get_tab_str(void);
static int menu_photo_size_get_sel_str(int ref);
static int menu_photo_size_get_sel_bmp(int ref);
static int menu_photo_size_set(void);
static int menu_photo_size_sel_set(void);
static int menu_photo_quality_init(void);
static int menu_photo_quality_get_tab_str(void);
static int menu_photo_quality_get_sel_str(int ref);
static int menu_photo_quality_get_sel_bmp(int ref);
static int menu_photo_quality_set(void);
static int menu_photo_quality_sel_set(void);
static int menu_photo_selftimer_init(void);
static int menu_photo_selftimer_get_tab_str(void);
static int menu_photo_selftimer_get_sel_str(int ref);
static int menu_photo_selftimer_get_sel_bmp(int ref);
static int menu_photo_selftimer_set(void);
static int menu_photo_selftimer_sel_set(void);
static int menu_photo_time_lapse_init(void);
static int menu_photo_time_lapse_get_tab_str(void);
static int menu_photo_time_lapse_get_sel_str(int ref);
static int menu_photo_time_lapse_get_sel_bmp(int ref);
static int menu_photo_time_lapse_set(void);
static int menu_photo_time_lapse_sel_set(void);
static int menu_photo_quickview_delay_init(void);
static int menu_photo_quickview_delay_get_tab_str(void);
static int menu_photo_quickview_delay_get_sel_str(int ref);
static int menu_photo_quickview_delay_get_sel_bmp(int ref);
static int menu_photo_quickview_delay_set(void);
static int menu_photo_quickview_delay_sel_set(void);
static int menu_photo_digital_zoom_init(void);
static int menu_photo_digital_zoom_get_tab_str(void);
static int menu_photo_digital_zoom_get_sel_str(int ref);
static int menu_photo_digital_zoom_get_sel_bmp(int ref);
static int menu_photo_digital_zoom_set(void);
static int menu_photo_digital_zoom_sel_set(void);
static int menu_photo_stamp_init(void);
static int menu_photo_stamp_get_tab_str(void);
static int menu_photo_stamp_get_sel_str(int ref);
static int menu_photo_stamp_get_sel_bmp(int ref);
static int menu_photo_stamp_set(void);
static int menu_photo_stamp_sel_set(void);

#ifdef CONFIG_APP_ARD
static CHECK_BOX_ITEM_S Photo_Stamp_Choose_Item[MENU_PHOTO_STAMP_CHOOSE_NUM] = {0};
static int menu_photo_stamp_choose_init(void);
static int menu_photo_stamp_choose_get_tab_str(void);
static int menu_photo_stamp_choose_set(void);
#endif

// control
static MENU_TAB_s* menu_photo_get_tab(void);
static MENU_ITEM_s* menu_photo_get_item(UINT32 itemId);
static MENU_SEL_s* menu_photo_get_sel(UINT32 itemId, UINT32 selId);
static int menu_photo_set_sel_table(UINT32 itemId, MENU_SEL_s *selTbl);
static int menu_photo_lock_tab(void);
static int menu_photo_unlock_tab(void);
static int menu_photo_enable_item(UINT32 itemId);
static int menu_photo_disable_item(UINT32 itemId);
static int menu_photo_lock_item(UINT32 itemId);
static int menu_photo_unlock_item(UINT32 itemId);
static int menu_photo_enable_sel(UINT32 itemId, UINT32 selId);
static int menu_photo_disable_sel(UINT32 itemId, UINT32 selId);
static int menu_photo_lock_sel(UINT32 itemId, UINT32 selId);
static int menu_photo_unlock_sel(UINT32 itemId, UINT32 selId);

/*************************************************************************
 * Definition:
 ************************************************************************/
/*** Selection ***/
static MENU_SEL_s menu_photo_multi_cap_sel_tbl[MENU_PHOTO_MULTI_CAP_SEL_NUM] = {
    {MENU_PHOTO_MULTI_CAP_OFF, MENU_SEL_FLAGS_ENABLE,
        STR_PHOTO_MULTI_CAP_OFF, BMP_ICN_AUTO_POWER_OFF, 0,
        PHOTO_MULTI_CAP_OFF, NULL},
    {MENU_PHOTO_MULTI_CAP_AEB, MENU_SEL_FLAGS_ENABLE,
        STR_PHOTO_MULTI_CAP_AEB, BMP_ICN_AEB_3, 0,
        PHOTO_MULTI_CAP_AEB, NULL}
};

static MENU_SEL_s menu_photo_cap_mode_sel_tbl[MENU_PHOTO_CAP_MODE_SEL_NUM] = {
    {MENU_PHOTO_CAP_MODE_PRECISE, MENU_SEL_FLAGS_ENABLE,
        STR_PHOTO_PRECISE_QUALITY, BMP_ICN_PRECISE_QUALITY, 0,
        PHOTO_CAP_MODE_PRECISE, NULL},
    {MENU_PHOTO_CAP_MODE_PES, MENU_SEL_FLAGS_ENABLE,
        STR_PHOTO_PES, BMP_ICN_PRECISE_QUALITY_CONT, 0,
        PHOTO_CAP_MODE_PES, NULL},
    {MENU_PHOTO_CAP_MODE_BURST, MENU_SEL_FLAGS_ENABLE,
        STR_PHOTO_BURST_MODE, BMP_ICN_CONT_SUPER_FAST, 0,
        PHOTO_CAP_MODE_BURST, NULL},
    {MENU_PHOTO_CAP_MODE_PRE_CAPTURE, 0,
        STR_PHOTO_PRE_CAPTURE_MODE, BMP_ICN_PRECISE_QUALITY_CONT, 0,
        PHOTO_CAP_MODE_PRE_CAPTURE, NULL}
};

static MENU_SEL_s menu_photo_size_sel_tbl[MENU_PHOTO_SIZE_SEL_NUM] = {
    {MENU_PHOTO_SIZE_1, 0,
        STR_PHOTO_SIZE_1, BMP_ICN_PHOTO_SIZE_1, 0,
        PHOTO_SIZE_1, NULL},
    {MENU_PHOTO_SIZE_2, 0,
        STR_PHOTO_SIZE_2, BMP_ICN_PHOTO_SIZE_2, 0,
        PHOTO_SIZE_2, NULL},
    {MENU_PHOTO_SIZE_3, 0,
        STR_PHOTO_SIZE_3, BMP_ICN_PHOTO_SIZE_3, 0,
        PHOTO_SIZE_3, NULL},
    {MENU_PHOTO_SIZE_4, 0,
        STR_PHOTO_SIZE_4, BMP_ICN_PHOTO_SIZE_4, 0,
        PHOTO_SIZE_4, NULL},
    {MENU_PHOTO_SIZE_5, 0,
        STR_PHOTO_SIZE_5, BMP_ICN_PHOTO_SIZE_5, 0,
        PHOTO_SIZE_5, NULL},
    {MENU_PHOTO_SIZE_6, 0,
        STR_PHOTO_SIZE_6, BMP_ICN_PHOTO_SIZE_6, 0,
        PHOTO_SIZE_6, NULL},
    {MENU_PHOTO_SIZE_7, 0,
        STR_PHOTO_SIZE_7, BMP_ICN_PHOTO_SIZE_7, 0,
        PHOTO_SIZE_7, NULL},
    {MENU_PHOTO_SIZE_8, 0,
        STR_PHOTO_SIZE_8, BMP_ICN_PHOTO_SIZE_8, 0,
        PHOTO_SIZE_8, NULL},
    {MENU_PHOTO_SIZE_9, 0,
        STR_PHOTO_SIZE_9, BMP_ICN_PHOTO_SIZE_9, 0,
        PHOTO_SIZE_9, NULL},
    {MENU_PHOTO_SIZE_10, 0,
        STR_PHOTO_SIZE_10, BMP_ICN_PHOTO_SIZE_10, 0,
        PHOTO_SIZE_10, NULL}
};

static MENU_SEL_s menu_photo_quality_sel_tbl[MENU_PHOTO_QUALITY_SEL_NUM] = {
    {MENU_PHOTO_QUALITY_SFINE, MENU_SEL_FLAGS_ENABLE,
        STR_QUALITY_SFINE, BMP_ICN_QUALITY_SF, 0,
        PHOTO_QUALITY_SFINE, NULL},
    {MENU_PHOTO_QUALITY_FINE, MENU_SEL_FLAGS_ENABLE,
        STR_QUALITY_FINE, BMP_ICN_QUALITY_F, 0,
        PHOTO_QUALITY_FINE, NULL},
    {MENU_PHOTO_QUALITY_NORMAL, MENU_SEL_FLAGS_ENABLE,
        STR_QUALITY_NORMAL, BMP_ICN_QUALITY_N, 0,
        PHOTO_QUALITY_NORMAL, NULL},
    {MENU_PHOTO_QUALITY_AUTO, 0,
        STR_QUALITY_AUTO, 0, 0,
        PHOTO_QUALITY_AUTO, NULL}
};

#ifdef PHOTO_SELFTIMER_ENABLE
/*self timer selection table*/
static MENU_SEL_s menu_photo_selftimer_sel_tbl[MENU_PHOTO_SELFTIMER_SEL_NUM] = {
    {MENU_PHOTO_SELFTIMER, MENU_SEL_FLAGS_ENABLE,
        STR_SELF_TIMER_OFF, BMP_ICN_SELF_TIMER_OFF, 0,
        SELF_TIMER_OFF, NULL},
    {MENU_PHOTO_SELFTIMER, MENU_SEL_FLAGS_ENABLE,
        STR_SELF_TIMER_3S, BMP_ICN_SELF_TIMER_3, 0,
        SELF_TIMER_3S, NULL},
    {MENU_PHOTO_SELFTIMER, MENU_SEL_FLAGS_ENABLE,
        STR_SELF_TIMER_5S, BMP_ICN_SELF_TIMER_5, 0,
        SELF_TIMER_5S, NULL},
    {MENU_PHOTO_SELFTIMER, MENU_SEL_FLAGS_ENABLE,
        STR_SELF_TIMER_10S, BMP_ICN_SELF_TIMER_10, 0,
        SELF_TIMER_10S, NULL}
};
#else
/*self timer selection table*/
static MENU_SEL_s menu_photo_selftimer_sel_tbl[MENU_PHOTO_SELFTIMER_SEL_NUM] = {
    {MENU_PHOTO_SELFTIMER, MENU_SEL_FLAGS_ENABLE,
        STR_SELF_TIMER_OFF, 0, 0,
        SELF_TIMER_OFF, NULL},
    {MENU_PHOTO_SELFTIMER, MENU_SEL_FLAGS_ENABLE,
        STR_SELF_TIMER_3S, 0, 0,
        SELF_TIMER_3S, NULL},
    {MENU_PHOTO_SELFTIMER, MENU_SEL_FLAGS_ENABLE,
        STR_SELF_TIMER_5S, 0, 0,
        SELF_TIMER_5S, NULL},
    {MENU_PHOTO_SELFTIMER, MENU_SEL_FLAGS_ENABLE,
        STR_SELF_TIMER_10S, 0, 0,
        SELF_TIMER_10S, NULL}
};
#endif

#ifdef PHOTO_TIMELAPES_ENABLE
static MENU_SEL_s menu_photo_time_lapse_sel_tbl[MENU_PHOTO_TIME_LAPSE_SEL_NUM] = {
    {MENU_PHOTO_TIME_LAPSE_OFF, MENU_SEL_FLAGS_ENABLE,
        STR_TIME_LAPSE_OFF, BMP_ICN_VIDEO_FILM, 0,
        PHOTO_TIME_LAPSE_OFF, NULL},
    {MENU_PHOTO_TIME_LAPSE_500MS, MENU_SEL_FLAGS_ENABLE,
        STR_TIME_LAPSE_500MS, BMP_ICN_VIDEO_FILM, 0,
        PHOTO_TIME_LAPSE_500MS, NULL},
    {MENU_PHOTO_TIME_LAPSE_1S, MENU_SEL_FLAGS_ENABLE,
        STR_TIME_LAPSE_1S, BMP_ICN_VIDEO_TIME_LAPSE_1, 0,
        PHOTO_TIME_LAPSE_1S, NULL},
    {MENU_PHOTO_TIME_LAPSE_5S, MENU_SEL_FLAGS_ENABLE,
        STR_TIME_LAPSE_5S, BMP_ICN_VIDEO_TIME_LAPSE_5, 0,
        PHOTO_TIME_LAPSE_5S, NULL}
};
#else
static MENU_SEL_s menu_photo_time_lapse_sel_tbl[MENU_PHOTO_TIME_LAPSE_SEL_NUM] = {
    {MENU_PHOTO_TIME_LAPSE_OFF, MENU_SEL_FLAGS_ENABLE,
        STR_TIME_LAPSE_OFF, BMP_ICN_VIDEO_FILM, 0,
        PHOTO_TIME_LAPSE_OFF, NULL},
    {MENU_PHOTO_TIME_LAPSE_500MS, MENU_SEL_FLAGS_ENABLE,
        STR_TIME_LAPSE_500MS, BMP_ICN_VIDEO_FILM, 0,
        PHOTO_TIME_LAPSE_500MS, NULL},
    {MENU_PHOTO_TIME_LAPSE_1S, MENU_SEL_FLAGS_ENABLE,
        STR_TIME_LAPSE_1S, 0, 0,
        PHOTO_TIME_LAPSE_1S, NULL},
    {MENU_PHOTO_TIME_LAPSE_5S, MENU_SEL_FLAGS_ENABLE,
        STR_TIME_LAPSE_5S, 0, 0,
        PHOTO_TIME_LAPSE_5S, NULL}
};
#endif

/*quckview time selection table*/
static MENU_SEL_s menu_photo_quickview_delay_sel_tbl[MENU_PHOTO_QUICKVIEW_DELAY_SEL_NUM] = {
    {MENU_PHOTO_QUICKVIEW_DELAY_OFF, MENU_SEL_FLAGS_ENABLE,
        STR_QUICKVIEW_DELAY_OFF, BMP_ICN_QUICK_VIEW_OFF, 0,
        PHOTO_QUICKVIEW_DELAY_OFF, NULL},
    {MENU_PHOTO_QUICKVIEW_DELAY_1S, MENU_SEL_FLAGS_ENABLE,
        STR_QUICKVIEW_DELAY_1S, BMP_ICN_QUICK_VIEW_1, 0,
        PHOTO_QUICKVIEW_DELAY_1S, NULL},
    {MENU_PHOTO_QUICKVIEW_DELAY_2S, MENU_SEL_FLAGS_ENABLE,
        STR_QUICKVIEW_DELAY_2S, BMP_ICN_QUICK_VIEW_2, 0,
        PHOTO_QUICKVIEW_DELAY_2S, NULL},
    {MENU_PHOTO_QUICKVIEW_DELAY_3S, MENU_SEL_FLAGS_ENABLE,
        STR_QUICKVIEW_DELAY_3S, BMP_ICN_QUICK_VIEW_3, 0,
        PHOTO_QUICKVIEW_DELAY_3S, NULL},
    {MENU_PHOTO_QUICKVIEW_DELAY_4S, MENU_SEL_FLAGS_ENABLE,
        STR_QUICKVIEW_DELAY_4S, BMP_ICN_QUICK_VIEW_4, 0,
        PHOTO_QUICKVIEW_DELAY_4S, NULL},
    {MENU_PHOTO_QUICKVIEW_DELAY_5S, MENU_SEL_FLAGS_ENABLE,
        STR_QUICKVIEW_DELAY_5S, BMP_ICN_QUICK_VIEW_5, 0,
        PHOTO_QUICKVIEW_DELAY_5S, NULL},
    {MENU_PHOTO_QUICKVIEW_DELAY_CONT, MENU_SEL_FLAGS_ENABLE,
        STR_QUICKVIEW_DELAY_CONT, BMP_ICN_QUICK_VIEW_CONT, 0,
        PHOTO_QUICKVIEW_DELAY_CONT, NULL}
};

static MENU_SEL_s menu_photo_digital_zoom_sel_tbl[MENU_PHOTO_DIGITAL_ZOOM_SEL_NUM] = {
    {MENU_PHOTO_DIGITAL_ZOOM_OFF, MENU_SEL_FLAGS_ENABLE,
        STR_DIGITAL_ZOOM_OFF, BMP_ICN_DIGITAL_ZOOM_OFF, 0,
        DZOOM_OFF, NULL},
    {MENU_PHOTO_DIGITAL_ZOOM_ON, MENU_SEL_FLAGS_ENABLE,
        STR_DIGITAL_ZOOM_4X, BMP_ICN_DIGITAL_ZOOM_4X, 0,
        DZOOM_4X, NULL},
    {MENU_PHOTO_DIGITAL_ZOOM_NUMX, MENU_SEL_FLAGS_ENABLE,
        STR_DIGITAL_ZOOM_16X, BMP_ICN_DIGITAL_ZOOM_16X, 0,
        DZOOM_16X, NULL}
};

static MENU_SEL_s menu_photo_stamp_sel_tbl[MENU_PHOTO_STAMP_SEL_NUM] = {
    {MENU_PHOTO_STAMP_OFF, MENU_SEL_FLAGS_ENABLE,
        STR_STAMP_OFF, BMP_ICN_STAMP_OFF, 0,
        STAMP_OFF, NULL},
    {MENU_PHOTO_STAMP_DATE, MENU_SEL_FLAGS_ENABLE,
        STR_STAMP_DATE, BMP_ICN_STAMP_DATE, 0,
        STAMP_DATE, NULL},
    {MENU_PHOTO_STAMP_TIME, MENU_SEL_FLAGS_ENABLE,
        STR_STAMP_TIME, BMP_ICN_STAMP_TIME, 0,
        STAMP_TIME, NULL},
    {MENU_PHOTO_STAMP_BOTH, MENU_SEL_FLAGS_ENABLE,
        STR_STAMP_DATE_TIME, BMP_ICN_STAMP_DATE_TIME, 0,
        STAMP_DATE_TIME, NULL}
};

static MENU_SEL_s *menu_photo_item_sel_tbls[MENU_PHOTO_ITEM_NUM] = {
    menu_photo_multi_cap_sel_tbl,
    menu_photo_cap_mode_sel_tbl,
    menu_photo_size_sel_tbl,
    menu_photo_quality_sel_tbl,
    menu_photo_selftimer_sel_tbl,
    menu_photo_time_lapse_sel_tbl,
    menu_photo_quickview_delay_sel_tbl,
    menu_photo_digital_zoom_sel_tbl,
    menu_photo_stamp_sel_tbl,
#ifdef CONFIG_APP_ARD
    NULL,
#endif
};

/*** Currently activated object Id arrays ***/
static MENU_SEL_s *menu_photo_multi_cap_sels[MENU_PHOTO_MULTI_CAP_SEL_NUM];
static MENU_SEL_s *menu_photo_cap_mode_sels[MENU_PHOTO_CAP_MODE_SEL_NUM];
static MENU_SEL_s *menu_photo_size_sels[MENU_PHOTO_SIZE_SEL_NUM];
static MENU_SEL_s *menu_photo_quality_sels[MENU_PHOTO_QUALITY_SEL_NUM];
static MENU_SEL_s *menu_photo_selftimer_sels[MENU_PHOTO_SELFTIMER_SEL_NUM];
static MENU_SEL_s *menu_photo_time_lapse_sels[MENU_PHOTO_TIME_LAPSE_SEL_NUM];
static MENU_SEL_s *menu_photo_quickview_delay_sels[MENU_PHOTO_QUICKVIEW_DELAY_SEL_NUM];
static MENU_SEL_s *menu_photo_digital_zoom_sels[MENU_PHOTO_DIGITAL_ZOOM_SEL_NUM];
static MENU_SEL_s *menu_photo_stamp_sels[MENU_PHOTO_STAMP_SEL_NUM];

/*** Item ***/
static MENU_ITEM_s menu_photo_multi_cap = {
    MENU_PHOTO_MULTI_CAP, 0,
    0, 0,
    0, 0,
    0, 0, 0, menu_photo_multi_cap_sels,
    menu_photo_multi_cap_init,
    menu_photo_multi_cap_get_tab_str,
    menu_photo_multi_cap_get_sel_str,
    menu_photo_multi_cap_get_sel_bmp,
    menu_photo_multi_cap_set,
    menu_photo_multi_cap_sel_set
};

static MENU_ITEM_s menu_photo_cap_mode = {
#ifdef CONFIG_APP_ARD
    MENU_PHOTO_CAP_MODE, 0, /* disable */
#else
    MENU_PHOTO_CAP_MODE, MENU_ITEM_FLAGS_ENABLE,
#endif
    0, 0,
    0, 0,
    0, 0, 0, menu_photo_cap_mode_sels,
    menu_photo_cap_mode_init,
    menu_photo_cap_mode_get_tab_str,
    menu_photo_cap_mode_get_sel_str,
    menu_photo_cap_mode_get_sel_bmp,
    menu_photo_cap_mode_set,
    menu_photo_cap_mode_sel_set
};

static MENU_ITEM_s menu_photo_size = {
    MENU_PHOTO_SIZE, MENU_ITEM_FLAGS_ENABLE,
    0, 0,
    0, 0,
    0, 0, 0, menu_photo_size_sels,
    menu_photo_size_init,
    menu_photo_size_get_tab_str,
    menu_photo_size_get_sel_str,
    menu_photo_size_get_sel_bmp,
    menu_photo_size_set,
    menu_photo_size_sel_set
};

static MENU_ITEM_s menu_photo_quality = {
    MENU_PHOTO_QUALITY, MENU_ITEM_FLAGS_ENABLE,
    0, 0,
    0, 0,
    0, 0, 0, menu_photo_quality_sels,
    menu_photo_quality_init,
    menu_photo_quality_get_tab_str,
    menu_photo_quality_get_sel_str,
    menu_photo_quality_get_sel_bmp,
    menu_photo_quality_set,
    menu_photo_quality_sel_set
};

static MENU_ITEM_s menu_photo_selftimer = {
    MENU_PHOTO_SELFTIMER, 0,
    0, 0,
    0, 0,
    0, 0, 0, menu_photo_selftimer_sels,
    menu_photo_selftimer_init,
    menu_photo_selftimer_get_tab_str,
    menu_photo_selftimer_get_sel_str,
    menu_photo_selftimer_get_sel_bmp,
    menu_photo_selftimer_set,
    menu_photo_selftimer_sel_set
};

static MENU_ITEM_s menu_photo_time_lapse = {
    MENU_PHOTO_TIME_LAPSE, 0,
    0, 0,
    0, 0,
    0, 0, 0, menu_photo_time_lapse_sels,
    menu_photo_time_lapse_init,
    menu_photo_time_lapse_get_tab_str,
    menu_photo_time_lapse_get_sel_str,
    menu_photo_time_lapse_get_sel_bmp,
    menu_photo_time_lapse_set,
    menu_photo_time_lapse_sel_set
};

static MENU_ITEM_s menu_photo_quickview_delay = {
    MENU_PHOTO_QUICKVIEW_DELAY, 0,
    0, 0,
    0, 0,
    0, 0, 0, menu_photo_quickview_delay_sels,
    menu_photo_quickview_delay_init,
    menu_photo_quickview_delay_get_tab_str,
    menu_photo_quickview_delay_get_sel_str,
    menu_photo_quickview_delay_get_sel_bmp,
    menu_photo_quickview_delay_set,
    menu_photo_quickview_delay_sel_set
};

static MENU_ITEM_s menu_photo_digital_zoom = {
    MENU_PHOTO_DIGITAL_ZOOM, 0,
    0, 0,
    0, 0,
    0, 0, 0, menu_photo_digital_zoom_sels,
    menu_photo_digital_zoom_init,
    menu_photo_digital_zoom_get_tab_str,
    menu_photo_digital_zoom_get_sel_str,
    menu_photo_digital_zoom_get_sel_bmp,
    menu_photo_digital_zoom_set,
    menu_photo_digital_zoom_sel_set
};

static MENU_ITEM_s menu_photo_stamp = {
    MENU_PHOTO_STAMP, 0,
    0, 0,
    0, 0,
    0, 0, 0, menu_photo_stamp_sels,
    menu_photo_stamp_init,
    menu_photo_stamp_get_tab_str,
    menu_photo_stamp_get_sel_str,
    menu_photo_stamp_get_sel_bmp,
    menu_photo_stamp_set,
    menu_photo_stamp_sel_set
};

#ifdef CONFIG_APP_ARD
static MENU_ITEM_s menu_photo_stamp_choose = {
    MENU_PHOTO_STAMP_CHOOSE, 0,     // disbale stamp menu in photo tab
    0, 0,
    0, 0,
    0, 0, 0, NULL,
    menu_photo_stamp_choose_init,
    menu_photo_stamp_choose_get_tab_str,
    NULL,
    NULL,
    menu_photo_stamp_choose_set,
    NULL
};
#endif


static MENU_ITEM_s *menu_photo_item_tbl[MENU_PHOTO_ITEM_NUM] = {
    &menu_photo_multi_cap,
    &menu_photo_cap_mode,
    &menu_photo_size,
    &menu_photo_quality,
    &menu_photo_selftimer,
    &menu_photo_time_lapse,
    &menu_photo_quickview_delay,
    &menu_photo_digital_zoom,
    &menu_photo_stamp,
#ifdef CONFIG_APP_ARD
    &menu_photo_stamp_choose,
#endif
};

/*** Currently activated object Id arrays ***/
static MENU_ITEM_s *menu_photo_items[MENU_PHOTO_ITEM_NUM];

/*** Tab ***/
static MENU_TAB_s menu_photo = {
    MENU_PHOTO, MENU_TAB_FLAGS_ENABLE,
    0, 0,
    BMP_MENU_TAB_PHOTO, BMP_MENU_TAB_PHOTO_HL,
    menu_photo_items,
    menu_photo_init,
    menu_photo_start,
    menu_photo_stop
};


MENU_TAB_CTRL_s menu_photo_ctrl = {
    menu_photo_get_tab,
    menu_photo_get_item,
    menu_photo_get_sel,
    menu_photo_set_sel_table,
    menu_photo_lock_tab,
    menu_photo_unlock_tab,
    menu_photo_enable_item,
    menu_photo_disable_item,
    menu_photo_lock_item,
    menu_photo_unlock_item,
    menu_photo_enable_sel,
    menu_photo_disable_sel,
    menu_photo_lock_sel,
    menu_photo_unlock_sel
};

/*** APIs ***/
// tab
static int menu_photo_init(void)
{
    int i = 0;
    UINT32 cur_item_id = 0;

    /** Check dzoom availability */
    if (AppLibSysSensor_CheckDzoomCap()) {
        menu_photo_enable_item(MENU_PHOTO_DIGITAL_ZOOM);
    }

//#if defined(CONFIG_APP_CONNECTED_STAMP)
//    menu_photo_enable_item(MENU_PHOTO_STAMP);
//    menu_photo_unlock_item(MENU_PHOTO_STAMP);
//#endif

    /** multi capture has higher priority to capture mode,
    when there has multi capture, lock the capture mode menu item */
    if (AppLibStillEnc_GetMultiCapMode() == PHOTO_MULTI_CAP_OFF) {
        AppMenu_UnlockItem(MENU_PHOTO, MENU_PHOTO_CAP_MODE);
    } else {
        AppMenu_LockItem(MENU_PHOTO, MENU_PHOTO_CAP_MODE);
    }

    APP_ADDFLAGS(menu_photo.Flags, MENU_TAB_FLAGS_INIT);

    if (menu_photo.ItemNum > 0) {
        cur_item_id = menu_photo_items[menu_photo.ItemCur]->Id;
    }
    menu_photo.ItemNum = 0;
    menu_photo.ItemCur = 0;
    for (i=0; i<MENU_PHOTO_ITEM_NUM; i++) {
        if (APP_CHECKFLAGS(menu_photo_item_tbl[i]->Flags, MENU_ITEM_FLAGS_ENABLE)) {
            menu_photo_items[menu_photo.ItemNum] = menu_photo_item_tbl[i];
            if (cur_item_id == menu_photo_item_tbl[i]->Id) {
                menu_photo.ItemCur = menu_photo.ItemNum;
            }
            menu_photo.ItemNum++;
        }
    }

    return 0;
}

static int menu_photo_start(void)
{
    return 0;
}

static int menu_photo_stop(void)
{
    return 0;
}

// item
static int menu_photo_multi_cap_init(void)
{
    int i = 0;

    APP_ADDFLAGS(menu_photo_multi_cap.Flags, MENU_ITEM_FLAGS_INIT);
    menu_photo_multi_cap.SelSaved = 0;
    menu_photo_multi_cap.SelNum = 0;
    menu_photo_multi_cap.SelCur = 0;
    for (i=0; i<MENU_PHOTO_MULTI_CAP_SEL_NUM; i++) {
        if (APP_CHECKFLAGS((menu_photo_item_sel_tbls[MENU_PHOTO_MULTI_CAP]+i)->Flags, MENU_SEL_FLAGS_ENABLE)) {
            menu_photo_multi_cap_sels[menu_photo_multi_cap.SelNum] = menu_photo_item_sel_tbls[MENU_PHOTO_MULTI_CAP]+i;
            if (menu_photo_multi_cap_sels[menu_photo_multi_cap.SelNum]->Val == UserSetting->PhotoPref.PhotoMultiCap) {
                menu_photo_multi_cap.SelSaved = menu_photo_multi_cap.SelNum;
                menu_photo_multi_cap.SelCur = menu_photo_multi_cap.SelNum;
            }
            menu_photo_multi_cap.SelNum++;
        }
    }

    return 0;
}

static int menu_photo_multi_cap_get_tab_str(void)
{
    return menu_photo_multi_cap_sels[menu_photo_multi_cap.SelSaved]->Str;
}

static int menu_photo_multi_cap_get_sel_str(int ref)
{
    return menu_photo_multi_cap_sels[ref]->Str;
}

static int menu_photo_multi_cap_get_sel_bmp(int ref)
{
    return menu_photo_multi_cap_sels[ref]->Bmp;
}

static int menu_photo_multi_cap_set(void)
{
    AppMenuQuick_SetItem(MENU_PHOTO, MENU_PHOTO_MULTI_CAP);
    AppWidget_On(WIDGET_MENU_QUICK, 0);
    return 0;
}

static int menu_photo_multi_cap_sel_set(void)
{
    int pjpeg_config_num = 0;
    if (menu_photo_multi_cap.SelSaved != menu_photo_multi_cap.SelCur) {
        menu_photo_multi_cap.SelSaved = menu_photo_multi_cap.SelCur;
        UserSetting->PhotoPref.PhotoMultiCap = menu_photo_multi_cap.Sels[menu_photo_multi_cap.SelCur]->Val;
        AppLibStillEnc_SetMultiCapMode(menu_photo_multi_cap.Sels[menu_photo_multi_cap.SelCur]->Val);
        pjpeg_config_num = AppLibSysSensor_GetPjpegConfigNum(AppLibStillEnc_GetPhotoPjpegCapMode());
        if (AppLibStillEnc_GetPhotoPjpegConfigId() >= pjpeg_config_num) {
            UserSetting->PhotoPref.PhotoSize = 0;
            menu_photo_size.SelSaved = 0;
            menu_photo_size.SelCur = 0;
            AppLibStillEnc_SetSizeID(UserSetting->PhotoPref.PhotoSize);
        }
        /** multi capture has higher priority to capture mode,
            when there has multi capture, lock the capture mode menu item */
        if (AppLibStillEnc_GetMultiCapMode() == PHOTO_MULTI_CAP_OFF) {
            AppLibStillEnc_SetNormCapMode(AppLibStillEnc_GetNormCapMode());
            AppMenu_UnlockItem(MENU_PHOTO, MENU_PHOTO_CAP_MODE);
        } else {
            AppLibStillEnc_SetMultiCapMode(AppLibStillEnc_GetMultiCapMode());
            AppMenu_LockItem(MENU_PHOTO, MENU_PHOTO_CAP_MODE);
        }
        //app_util_menu_lock_item_self_timer(0);
        {
            /* Send the message to the current app. */
            APP_APP_s *curapp;
            AppAppMgt_GetCurApp(&curapp);
            curapp->OnMessage(AMSG_CMD_SET_PHOTO_CAPTURE_MODE, 0, 0);
        }
    }
    return 0;
}

static int menu_photo_cap_mode_init(void)
{
    int i = 0;

#ifndef CONFIG_APP_ARD
    if (AppLibSysSensor_GetPjpegConfigNum(SENSOR_PHOTO_CAP_BURST)) {
        menu_photo_enable_sel(MENU_PHOTO_CAP_MODE, MENU_PHOTO_CAP_MODE_BURST);
    }
#endif

    /** check for 3D */
    /** lock unused capture mode and photo size */
    for (i = MENU_PHOTO_CAP_MODE_BURST; i<menu_photo_cap_mode.SelNum; i++) {
        if (AppLibSysVin_GetDimension() == INPUT_3D) {
            menu_photo_lock_sel(MENU_PHOTO_CAP_MODE, i);
        } else {
            menu_photo_unlock_sel(MENU_PHOTO_CAP_MODE, i);
        }
    }

    APP_ADDFLAGS(menu_photo_cap_mode.Flags, MENU_ITEM_FLAGS_INIT);
    menu_photo_cap_mode.SelSaved = 0;
    menu_photo_cap_mode.SelNum = 0;
    menu_photo_cap_mode.SelCur = 0;
    for (i=0; i<MENU_PHOTO_CAP_MODE_SEL_NUM; i++) {
        if (APP_CHECKFLAGS((menu_photo_item_sel_tbls[MENU_PHOTO_CAP_MODE]+i)->Flags, MENU_SEL_FLAGS_ENABLE)) {
            menu_photo_cap_mode_sels[menu_photo_cap_mode.SelNum] = menu_photo_item_sel_tbls[MENU_PHOTO_CAP_MODE]+i;
            if (menu_photo_cap_mode_sels[menu_photo_cap_mode.SelNum]->Val == UserSetting->PhotoPref.PhotoCapMode) {
                menu_photo_cap_mode.SelSaved = menu_photo_cap_mode.SelNum;
                menu_photo_cap_mode.SelCur = menu_photo_cap_mode.SelNum;
            }
            menu_photo_cap_mode.SelNum++;
        }
    }

    return 0;
}

static int menu_photo_cap_mode_get_tab_str(void)
{
    return menu_photo_cap_mode_sels[menu_photo_cap_mode.SelSaved]->Str;
}

static int menu_photo_cap_mode_get_sel_str(int ref)
{
    return menu_photo_cap_mode_sels[ref]->Str;
}

static int menu_photo_cap_mode_get_sel_bmp(int ref)
{
    return menu_photo_cap_mode_sels[ref]->Bmp;
}

static int menu_photo_cap_mode_set(void)
{
    AppMenuQuick_SetItem(MENU_PHOTO, MENU_PHOTO_CAP_MODE);
    AppWidget_On(WIDGET_MENU_QUICK, 0);
    return 0;
}

static int menu_photo_cap_mode_sel_set(void)
{
    int pjpeg_config_num = 0;
    if (menu_photo_cap_mode.SelSaved != menu_photo_cap_mode.SelCur) {
        menu_photo_cap_mode.SelSaved = menu_photo_cap_mode.SelCur;
        UserSetting->PhotoPref.PhotoCapMode = menu_photo_cap_mode.Sels[menu_photo_cap_mode.SelCur]->Val;
        AppLibStillEnc_SetNormCapMode(menu_photo_cap_mode.Sels[menu_photo_cap_mode.SelCur]->Val);
        pjpeg_config_num = AppLibSysSensor_GetPjpegConfigNum(AppLibStillEnc_GetPhotoPjpegCapMode());
        if (AppLibStillEnc_GetPhotoPjpegConfigId() >= pjpeg_config_num) {
            UserSetting->PhotoPref.PhotoSize = 0;
            menu_photo_size.SelSaved = 0;
            menu_photo_size.SelCur = 0;
            AppLibStillEnc_SetSizeID(UserSetting->PhotoPref.PhotoSize);
        }
        /** multi capture has higher priority to capture mode,
        when there has multi capture, lock the capture mode menu item */
        if (AppLibStillEnc_GetMultiCapMode() == PHOTO_MULTI_CAP_OFF) {
            AppLibStillEnc_SetNormCapMode(AppLibStillEnc_GetNormCapMode());
            AppMenu_UnlockItem(MENU_PHOTO, MENU_PHOTO_CAP_MODE);
        } else {
            AppLibStillEnc_SetMultiCapMode(AppLibStillEnc_GetMultiCapMode());
            AppMenu_LockItem(MENU_PHOTO, MENU_PHOTO_CAP_MODE);
        }
        //app_util_menu_lock_item_self_timer(0);
        {
            /* Send the message to the current app. */
            APP_APP_s *curapp;
            AppAppMgt_GetCurApp(&curapp);
            curapp->OnMessage(AMSG_CMD_SET_PHOTO_CAPTURE_MODE, 0, 0);
        }
    }
    return 0;
}

static int menu_photo_size_init(void)
{
    int i = 0;
    int pjpeg_config_num = 0;
    UINT16 *Str;

    pjpeg_config_num = AppLibSysSensor_GetPjpegConfigNum(AppLibStillEnc_GetPhotoPjpegCapMode());
    for (i=0; i<MENU_PHOTO_SIZE_SEL_NUM; i++) {
        if (i < pjpeg_config_num) {
            if ((menu_photo_item_sel_tbls[MENU_PHOTO_SIZE]+i)->Val < 0) {
                APP_REMOVEFLAGS((menu_photo_item_sel_tbls[MENU_PHOTO_SIZE]+i)->Flags, MENU_SEL_FLAGS_ENABLE);
            } else {
                APP_ADDFLAGS((menu_photo_item_sel_tbls[MENU_PHOTO_SIZE]+i)->Flags, MENU_SEL_FLAGS_ENABLE);
            }
            Str = AppLibSysSensor_GetPhotoSizeStr(AppLibStillEnc_GetPhotoPjpegCapMode(), i);
#ifdef CONFIG_APP_ARD
            AppLibGraph_UpdateStringContext(AppPref_GetLanguageID(), (menu_photo_item_sel_tbls[MENU_PHOTO_SIZE]+i)->Str, Str);
#else
            AppLibGraph_UpdateStringContext(0, (menu_photo_item_sel_tbls[MENU_PHOTO_SIZE]+i)->Str, Str);
#endif

        } else {
            APP_REMOVEFLAGS((menu_photo_item_sel_tbls[MENU_PHOTO_SIZE]+i)->Flags, MENU_SEL_FLAGS_ENABLE);
        }
    }

    /** check for 3D */
    for (i=0; i<MENU_PHOTO_SIZE_SEL_NUM; i++) {
        if (i < pjpeg_config_num) {
            if (AppLibSysVin_GetDimension() == INPUT_3D) {
                if ((AppLibSysSensor_GetCaptureModeAR(AppLibStillEnc_GetPhotoPjpegCapMode(), i)) != VAR_16x9) {
                    menu_photo_lock_sel(MENU_PHOTO_SIZE, i);
                }
            } else {
                if (APP_ADDFLAGS((menu_photo_item_sel_tbls[MENU_PHOTO_SIZE]+i)->Flags, MENU_SEL_FLAGS_LOCKED)) {
                    menu_photo_unlock_sel(MENU_PHOTO_SIZE, i);
                }
            }
        }
    }

    APP_ADDFLAGS(menu_photo_size.Flags, MENU_ITEM_FLAGS_INIT);
    menu_photo_size.SelSaved = 0;
    menu_photo_size.SelNum = 0;
    menu_photo_size.SelCur = 0;
    for (i=0; i<MENU_PHOTO_SIZE_SEL_NUM; i++) {
        if (APP_CHECKFLAGS((menu_photo_item_sel_tbls[MENU_PHOTO_SIZE]+i)->Flags, MENU_SEL_FLAGS_ENABLE)) {
            menu_photo_size_sels[menu_photo_size.SelNum] = menu_photo_item_sel_tbls[MENU_PHOTO_SIZE]+i;
            if (menu_photo_size_sels[menu_photo_size.SelNum]->Val == UserSetting->PhotoPref.PhotoSize) {
                menu_photo_size.SelSaved = menu_photo_size.SelNum;
                menu_photo_size.SelCur = menu_photo_size.SelNum;
            }
            menu_photo_size.SelNum++;
        }
    }

    return 0;
}

static int menu_photo_size_get_tab_str(void)
{
#ifdef CONFIG_APP_ARD
    return STR_PHOTO_RESOLUTION;
#else
    return menu_photo_size_sels[menu_photo_size.SelSaved]->Str;
#endif
}

static int menu_photo_size_get_sel_str(int ref)
{
    return menu_photo_size_sels[ref]->Str;
}

static int menu_photo_size_get_sel_bmp(int ref)
{
    return menu_photo_size_sels[ref]->Bmp;
}

static int menu_photo_size_set(void)
{
    AppMenuQuick_SetItem(MENU_PHOTO, MENU_PHOTO_SIZE);
    AppWidget_On(WIDGET_MENU_QUICK, 0);
    return 0;
}

static int menu_photo_size_sel_set(void)
{
    if (menu_photo_size.SelSaved != menu_photo_size.SelCur) {
        menu_photo_size.SelSaved = menu_photo_size.SelCur;
        UserSetting->PhotoPref.PhotoSize = menu_photo_size.Sels[menu_photo_size.SelCur]->Val;
        AppLibStillEnc_SetSizeID(menu_photo_size.Sels[menu_photo_size.SelCur]->Val);
        {
            /* Send the message to the current app. */
            APP_APP_s *curapp;
            AppAppMgt_GetCurApp(&curapp);
            curapp->OnMessage(AMSG_CMD_SET_PHOTO_SIZE, menu_photo_size.Sels[menu_photo_size.SelCur]->Val, 0);
        }
    }
    return 0;
}

static int menu_photo_quality_init(void)
{
    int i = 0;

    APP_ADDFLAGS(menu_photo_quality.Flags, MENU_ITEM_FLAGS_INIT);
    menu_photo_quality.SelSaved = 0;
    menu_photo_quality.SelNum = 0;
    menu_photo_quality.SelCur = 0;
    for (i=0; i<MENU_PHOTO_QUALITY_SEL_NUM; i++) {
        if (APP_CHECKFLAGS((menu_photo_item_sel_tbls[MENU_PHOTO_QUALITY]+i)->Flags, MENU_SEL_FLAGS_ENABLE)) {
            menu_photo_quality_sels[menu_photo_quality.SelNum] = menu_photo_item_sel_tbls[MENU_PHOTO_QUALITY]+i;
            if (menu_photo_quality_sels[menu_photo_quality.SelNum]->Val == UserSetting->PhotoPref.PhotoQuality) {
                menu_photo_quality.SelSaved = menu_photo_quality.SelNum;
                menu_photo_quality.SelCur = menu_photo_quality.SelNum;
            }
            menu_photo_quality.SelNum++;
        }
    }

    return 0;
}

static int menu_photo_quality_get_tab_str(void)
{
    return menu_photo_quality_sels[menu_photo_quality.SelSaved]->Str;
}

static int menu_photo_quality_get_sel_str(int ref)
{
    return menu_photo_quality_sels[ref]->Str;
}

static int menu_photo_quality_get_sel_bmp(int ref)
{
    return menu_photo_quality_sels[ref]->Bmp;
}

static int menu_photo_quality_set(void)
{
    AppMenuQuick_SetItem(MENU_PHOTO, MENU_PHOTO_QUALITY);
    AppWidget_On(WIDGET_MENU_QUICK, 0);
    return 0;
}

static int menu_photo_quality_sel_set(void)
{
    if (menu_photo_quality.SelSaved != menu_photo_quality.SelCur) {
        menu_photo_quality.SelSaved = menu_photo_quality.SelCur;
        UserSetting->PhotoPref.PhotoQuality = menu_photo_quality.Sels[menu_photo_quality.SelCur]->Val;
        AppLibStillEnc_SetQualityMode(menu_photo_quality.Sels[menu_photo_quality.SelCur]->Val);
        {
            /* Send the message to the current app. */
            APP_APP_s *curapp;
            AppAppMgt_GetCurApp(&curapp);
            curapp->OnMessage(AMSG_CMD_SET_PHOTO_QUALITY, menu_photo_quality.Sels[menu_photo_quality.SelCur]->Val, 0);
        }
    }
    return 0;
}

static int menu_photo_selftimer_init(void)
{
    int i = 0;

    APP_ADDFLAGS(menu_photo_selftimer.Flags, MENU_ITEM_FLAGS_INIT);
    menu_photo_selftimer.SelSaved = 0;
    menu_photo_selftimer.SelNum = 0;
    menu_photo_selftimer.SelCur = 0;
    for (i=0; i<MENU_PHOTO_SELFTIMER_SEL_NUM; i++) {
        if (APP_CHECKFLAGS((menu_photo_item_sel_tbls[MENU_PHOTO_SELFTIMER]+i)->Flags, MENU_SEL_FLAGS_ENABLE)) {
            menu_photo_selftimer_sels[menu_photo_selftimer.SelNum] = menu_photo_item_sel_tbls[MENU_PHOTO_SELFTIMER]+i;
            if (menu_photo_selftimer_sels[menu_photo_selftimer.SelNum]->Val == UserSetting->PhotoPref.PhotoSelftimer) {
                menu_photo_selftimer.SelSaved = menu_photo_selftimer.SelNum;
                menu_photo_selftimer.SelCur = menu_photo_selftimer.SelNum;
            }
            menu_photo_selftimer.SelNum++;
        }
    }

    return 0;
}

static int menu_photo_selftimer_get_tab_str(void)
{
    return menu_photo_selftimer_sels[menu_photo_selftimer.SelSaved]->Str;
}

static int menu_photo_selftimer_get_sel_str(int ref)
{
    return menu_photo_selftimer_sels[ref]->Str;
}

static int menu_photo_selftimer_get_sel_bmp(int ref)
{
    return menu_photo_selftimer_sels[ref]->Bmp;
}

static int menu_photo_selftimer_set(void)
{
    AppMenuQuick_SetItem(MENU_PHOTO, MENU_PHOTO_SELFTIMER);
    AppWidget_On(WIDGET_MENU_QUICK, 0);
    return 0;
}

static int menu_photo_selftimer_sel_set(void)
{
    if (menu_photo_selftimer.SelSaved != menu_photo_selftimer.SelCur) {
        menu_photo_selftimer.SelSaved = menu_photo_selftimer.SelCur;
        UserSetting->PhotoPref.PhotoSelftimer = (APP_PREF_SELF_TIMER_e)menu_photo_selftimer.Sels[menu_photo_selftimer.SelCur]->Val;
        {
            /* Send the message to the current app. */
            APP_APP_s *curapp;
            AppAppMgt_GetCurApp(&curapp);
            curapp->OnMessage(AMSG_CMD_SET_SELFTIMER, menu_photo_selftimer.Sels[menu_photo_selftimer.SelCur]->Val, 0);
        }
    }
    return 0;
}

static int menu_photo_time_lapse_init(void)
{
    int i = 0;

    APP_ADDFLAGS(menu_photo_time_lapse.Flags, MENU_ITEM_FLAGS_INIT);
    menu_photo_time_lapse.SelSaved = 0;
    menu_photo_time_lapse.SelNum = 0;
    menu_photo_time_lapse.SelCur = 0;
    for (i=0; i<MENU_PHOTO_TIME_LAPSE_SEL_NUM; i++) {
        if (APP_CHECKFLAGS((menu_photo_item_sel_tbls[MENU_PHOTO_TIME_LAPSE]+i)->Flags, MENU_SEL_FLAGS_ENABLE)) {
            menu_photo_time_lapse_sels[menu_photo_time_lapse.SelNum] = menu_photo_item_sel_tbls[MENU_PHOTO_TIME_LAPSE]+i;
            if (menu_photo_time_lapse_sels[menu_photo_time_lapse.SelNum]->Val == UserSetting->PhotoPref.TimeLapse) {
                menu_photo_time_lapse.SelSaved = menu_photo_time_lapse.SelNum;
                menu_photo_time_lapse.SelCur = menu_photo_time_lapse.SelNum;
            }
            menu_photo_time_lapse.SelNum++;
        }
    }

    return 0;
}

static int menu_photo_time_lapse_get_tab_str(void)
{
    return menu_photo_time_lapse_sels[menu_photo_time_lapse.SelSaved]->Str;
}

static int menu_photo_time_lapse_get_sel_str(int ref)
{
    return menu_photo_time_lapse_sels[ref]->Str;
}

static int menu_photo_time_lapse_get_sel_bmp(int ref)
{
    return menu_photo_time_lapse_sels[ref]->Bmp;
}

static int menu_photo_time_lapse_set(void)
{
    AppMenuQuick_SetItem(MENU_PHOTO, MENU_PHOTO_TIME_LAPSE);
    AppWidget_On(WIDGET_MENU_QUICK, 0);
    return 0;
}

static int menu_photo_time_lapse_sel_set(void)
{
    if (menu_photo_time_lapse.SelSaved != menu_photo_time_lapse.SelCur) {
        menu_photo_time_lapse.SelSaved = menu_photo_time_lapse.SelCur;
        UserSetting->PhotoPref.TimeLapse = menu_photo_time_lapse.Sels[menu_photo_time_lapse.SelCur]->Val;
        //app_util_menu_lock_item_dual_stream_prerecord(0);
        //app_util_menu_lock_item_scene_sshutter(0);
        {
            /* Send the message to the current app. */
            APP_APP_s *curapp;
            AppAppMgt_GetCurApp(&curapp);
            curapp->OnMessage(AMSG_CMD_SET_PHOTO_TIME_LAPSE, menu_photo_time_lapse.Sels[menu_photo_time_lapse.SelCur]->Val, 0);
        }
    }
    return 0;
}

static int menu_photo_quickview_delay_init(void)
{
    int i = 0;

    APP_ADDFLAGS(menu_photo_quickview_delay.Flags, MENU_ITEM_FLAGS_INIT);
    menu_photo_quickview_delay.SelSaved = 0;
    menu_photo_quickview_delay.SelNum = 0;
    menu_photo_quickview_delay.SelCur = 0;
    for (i=0; i<MENU_PHOTO_QUICKVIEW_DELAY_SEL_NUM; i++) {
        if (APP_CHECKFLAGS((menu_photo_item_sel_tbls[MENU_PHOTO_QUICKVIEW_DELAY]+i)->Flags, MENU_SEL_FLAGS_ENABLE)) {
            menu_photo_quickview_delay_sels[menu_photo_quickview_delay.SelNum] = menu_photo_item_sel_tbls[MENU_PHOTO_QUICKVIEW_DELAY]+i;
            if (menu_photo_quickview_delay_sels[menu_photo_quickview_delay.SelNum]->Val == UserSetting->PhotoPref.QuickviewDelay) {
                menu_photo_quickview_delay.SelSaved = menu_photo_quickview_delay.SelNum;
                menu_photo_quickview_delay.SelCur = menu_photo_quickview_delay.SelNum;
            }
            menu_photo_quickview_delay.SelNum++;
        }
    }

    return 0;
}

static int menu_photo_quickview_delay_get_tab_str(void)
{
    return menu_photo_quickview_delay_sels[menu_photo_quickview_delay.SelSaved]->Str;
}

static int menu_photo_quickview_delay_get_sel_str(int ref)
{
    return menu_photo_quickview_delay_sels[ref]->Str;
}

static int menu_photo_quickview_delay_get_sel_bmp(int ref)
{
    return menu_photo_quickview_delay_sels[ref]->Bmp;
}

static int menu_photo_quickview_delay_set(void)
{
    AppMenuQuick_SetItem(MENU_PHOTO, MENU_PHOTO_QUICKVIEW_DELAY);
    AppWidget_On(WIDGET_MENU_QUICK, 0);
    return 0;
}

static int menu_photo_quickview_delay_sel_set(void)
{
    if (menu_photo_quickview_delay.SelSaved != menu_photo_quickview_delay.SelCur) {
        menu_photo_quickview_delay.SelSaved = menu_photo_quickview_delay.SelCur;
        UserSetting->PhotoPref.QuickviewDelay = menu_photo_quickview_delay.Sels[menu_photo_quickview_delay.SelCur]->Val;
        AppLibStillEnc_SetQuickviewDelay(menu_photo_quickview_delay.Sels[menu_photo_quickview_delay.SelCur]->Val);
        {
            /* Send the message to the current app. */
            APP_APP_s *curapp;
            AppAppMgt_GetCurApp(&curapp);
            curapp->OnMessage(AMSG_CMD_SET_PHOTO_QUICKVIEW_DELAY, menu_photo_quickview_delay.Sels[menu_photo_quickview_delay.SelCur]->Val, 0);
        }
    }
    return 0;
}

static int menu_photo_digital_zoom_init(void)
{
    int i = 0;

    APP_ADDFLAGS(menu_photo_digital_zoom.Flags, MENU_ITEM_FLAGS_INIT);
    menu_photo_digital_zoom.SelSaved = 0;
    menu_photo_digital_zoom.SelNum = 0;
    menu_photo_digital_zoom.SelCur = 0;
    for (i=0; i<MENU_PHOTO_DIGITAL_ZOOM_SEL_NUM; i++) {
        if (APP_CHECKFLAGS((menu_photo_item_sel_tbls[MENU_PHOTO_DIGITAL_ZOOM]+i)->Flags, MENU_SEL_FLAGS_ENABLE)) {
            menu_photo_digital_zoom_sels[menu_photo_digital_zoom.SelNum] = menu_photo_item_sel_tbls[MENU_PHOTO_DIGITAL_ZOOM]+i;
            if (menu_photo_digital_zoom_sels[menu_photo_digital_zoom.SelNum]->Val == UserSetting->PhotoPref.PhotoDZoom) {
                menu_photo_digital_zoom.SelSaved = menu_photo_digital_zoom.SelNum;
                menu_photo_digital_zoom.SelCur = menu_photo_digital_zoom.SelNum;
            }
            menu_photo_digital_zoom.SelNum++;
        }
    }

    return 0;
}

static int menu_photo_digital_zoom_get_tab_str(void)
{
    return menu_photo_digital_zoom_sels[menu_photo_digital_zoom.SelSaved]->Str;
}

static int menu_photo_digital_zoom_get_sel_str(int ref)
{
    return menu_photo_digital_zoom_sels[ref]->Str;
}

static int menu_photo_digital_zoom_get_sel_bmp(int ref)
{
    return menu_photo_digital_zoom_sels[ref]->Bmp;
}

static int menu_photo_digital_zoom_set(void)
{
    AppMenuQuick_SetItem(MENU_PHOTO, MENU_PHOTO_DIGITAL_ZOOM);
    AppWidget_On(WIDGET_MENU_QUICK, 0);
    return 0;
}

static int menu_photo_digital_zoom_sel_set(void)
{
    if (menu_photo_digital_zoom.SelSaved != menu_photo_digital_zoom.SelCur) {
        menu_photo_digital_zoom.SelSaved = menu_photo_digital_zoom.SelCur;
        UserSetting->PhotoPref.PhotoDZoom = (APP_PREF_DZOOM_e)menu_photo_digital_zoom.Sels[menu_photo_digital_zoom.SelCur]->Val;
        {
            /* Send the message to the current app. */
            APP_APP_s *curapp;
            AppAppMgt_GetCurApp(&curapp);
            curapp->OnMessage(AMSG_CMD_SET_DIGITAL_ZOOM, menu_photo_digital_zoom.Sels[menu_photo_digital_zoom.SelCur]->Val, 0);
        }
    }
    return 0;
}

static int menu_photo_stamp_init(void)
{
    int i = 0;

    APP_ADDFLAGS(menu_photo_stamp.Flags, MENU_ITEM_FLAGS_INIT);
    menu_photo_stamp.SelSaved = 0;
    menu_photo_stamp.SelNum = 0;
    menu_photo_stamp.SelCur = 0;
    for (i=0; i<MENU_PHOTO_STAMP_SEL_NUM; i++) {
        if (APP_CHECKFLAGS((menu_photo_item_sel_tbls[MENU_PHOTO_STAMP]+i)->Flags, MENU_SEL_FLAGS_ENABLE)) {
            menu_photo_stamp_sels[menu_photo_stamp.SelNum] = menu_photo_item_sel_tbls[MENU_PHOTO_STAMP]+i;
            if (menu_photo_stamp_sels[menu_photo_stamp.SelNum]->Val == UserSetting->PhotoPref.PhotoTimeStamp) {
                menu_photo_stamp.SelSaved = menu_photo_stamp.SelNum;
                menu_photo_stamp.SelCur = menu_photo_stamp.SelNum;
            }
            menu_photo_stamp.SelNum++;
        }
    }

    return 0;
}

static int menu_photo_stamp_get_tab_str(void)
{
    if (APP_CHECKFLAGS(menu_photo_stamp.Flags, MENU_ITEM_FLAGS_LOCKED)) {
        return STR_STAMP_OFF;
    } else {
        return menu_photo_stamp_sels[menu_photo_stamp.SelSaved]->Str;
    }
}

static int menu_photo_stamp_get_sel_str(int ref)
{
    return menu_photo_stamp_sels[ref]->Str;
}

static int menu_photo_stamp_get_sel_bmp(int ref)
{
    return menu_photo_stamp_sels[ref]->Bmp;
}

static int menu_photo_stamp_set(void)
{
    AppMenuQuick_SetItem(MENU_PHOTO, MENU_PHOTO_STAMP);
    AppWidget_On(WIDGET_MENU_QUICK, 0);
    return 0;
}

static int menu_photo_stamp_sel_set(void)
{
    if (menu_photo_stamp.SelSaved != menu_photo_stamp.SelCur) {
        menu_photo_stamp.SelSaved = menu_photo_stamp.SelCur;
        UserSetting->PhotoPref.PhotoTimeStamp = (APP_PREF_TIME_STAMP_e)menu_photo_stamp_sels[menu_photo_stamp.SelCur]->Val;
    }

    return 0;
}

#ifdef CONFIG_APP_ARD
static int PhotoStampItemSet(int item_idx)
{
    UserSetting->VideoStampPref.StampDate = Photo_Stamp_Choose_Item[MENU_PHOTO_STAMP_CHOOSE_DATE].check_or_not;
    UserSetting->VideoStampPref.StampTime = Photo_Stamp_Choose_Item[MENU_PHOTO_STAMP_CHOOSE_TIME].check_or_not;
    UserSetting->VideoStampPref.StampDriverId = Photo_Stamp_Choose_Item[MENU_PHOTO_STAMP_CHOOSE_DRIVERID].check_or_not;
    return 0;
}

static int menu_photo_stamp_choose_init(void)
{
    Photo_Stamp_Choose_Item[MENU_PHOTO_STAMP_CHOOSE_DATE].item_id = MENU_PHOTO_STAMP_CHOOSE_DATE;
    Photo_Stamp_Choose_Item[MENU_PHOTO_STAMP_CHOOSE_DATE].string_id = STR_STAMP_DATE;
    Photo_Stamp_Choose_Item[MENU_PHOTO_STAMP_CHOOSE_DATE].check_or_not= UserSetting->VideoStampPref.StampDate;

    Photo_Stamp_Choose_Item[MENU_PHOTO_STAMP_CHOOSE_TIME].item_id = MENU_PHOTO_STAMP_CHOOSE_TIME;
    Photo_Stamp_Choose_Item[MENU_PHOTO_STAMP_CHOOSE_TIME].string_id = STR_STAMP_TIME;
    Photo_Stamp_Choose_Item[MENU_PHOTO_STAMP_CHOOSE_TIME].check_or_not= UserSetting->VideoStampPref.StampTime;

    Photo_Stamp_Choose_Item[MENU_PHOTO_STAMP_CHOOSE_DRIVERID].item_id = MENU_PHOTO_STAMP_CHOOSE_DRIVERID;
    Photo_Stamp_Choose_Item[MENU_PHOTO_STAMP_CHOOSE_DRIVERID].string_id = STR_STAMP_DRIVER_ID;
    Photo_Stamp_Choose_Item[MENU_PHOTO_STAMP_CHOOSE_DRIVERID].check_or_not= UserSetting->VideoStampPref.StampDriverId;

    return 0;
}

static int menu_photo_stamp_choose_get_tab_str(void)
{
    return STR_STAMP_SETTING;
}

static int menu_photo_stamp_choose_set(void)
{
    app_widget_check_box_init_item( MENU_PHOTO_STAMP_CHOOSE_NUM, &Photo_Stamp_Choose_Item[0], &PhotoStampItemSet);
    app_widget_menu_check_box_set_item(MENU_PHOTO, MENU_PHOTO_STAMP_CHOOSE);
    AppWidget_On(WIDGET_MENU_CKBX, 0);

    return 0;
}
#endif

// control
static MENU_TAB_s* menu_photo_get_tab(void)
{
    return &menu_photo;
}

static MENU_ITEM_s* menu_photo_get_item(UINT32 itemId)
{
    return menu_photo_item_tbl[itemId];
}

static MENU_SEL_s* menu_photo_get_sel(UINT32 itemId, UINT32 selId)
{
    return &menu_photo_item_sel_tbls[itemId][selId];
}

static int menu_photo_set_sel_table(UINT32 itemId, MENU_SEL_s *selTbl)
{
    menu_photo_item_sel_tbls[itemId] = selTbl;
    APP_REMOVEFLAGS(menu_photo_item_tbl[itemId]->Flags, MENU_ITEM_FLAGS_INIT);
    return 0;
}

static int menu_photo_lock_tab(void)
{
    APP_ADDFLAGS(menu_photo.Flags, MENU_TAB_FLAGS_LOCKED);
    return 0;
}

static int menu_photo_unlock_tab(void)
{
    APP_REMOVEFLAGS(menu_photo.Flags, MENU_TAB_FLAGS_LOCKED);
    return 0;
}

static int menu_photo_enable_item(UINT32 itemId)
{
    if (!APP_CHECKFLAGS(menu_photo_item_tbl[itemId]->Flags, MENU_ITEM_FLAGS_ENABLE)) {
        APP_ADDFLAGS(menu_photo_item_tbl[itemId]->Flags, MENU_ITEM_FLAGS_ENABLE);
        APP_REMOVEFLAGS(menu_photo.Flags, MENU_TAB_FLAGS_INIT);
    }
    return 0;
}

static int menu_photo_disable_item(UINT32 itemId)
{
    if (APP_CHECKFLAGS(menu_photo_item_tbl[itemId]->Flags, MENU_ITEM_FLAGS_ENABLE)) {
        APP_REMOVEFLAGS(menu_photo_item_tbl[itemId]->Flags, MENU_ITEM_FLAGS_ENABLE);
        APP_REMOVEFLAGS(menu_photo.Flags, MENU_TAB_FLAGS_INIT);
    }
    return 0;
}

static int menu_photo_lock_item(UINT32 itemId)
{
    APP_ADDFLAGS(menu_photo_item_tbl[itemId]->Flags, MENU_ITEM_FLAGS_LOCKED);
    return 0;
}

static int menu_photo_unlock_item(UINT32 itemId)
{
    APP_REMOVEFLAGS(menu_photo_item_tbl[itemId]->Flags, MENU_ITEM_FLAGS_LOCKED);
    return 0;
}

static int menu_photo_enable_sel(UINT32 itemId, UINT32 selId)
{
    if (!APP_CHECKFLAGS((menu_photo_item_sel_tbls[itemId]+selId)->Flags, MENU_SEL_FLAGS_ENABLE)) {
        APP_ADDFLAGS((menu_photo_item_sel_tbls[itemId]+selId)->Flags, MENU_SEL_FLAGS_ENABLE);
        APP_REMOVEFLAGS(menu_photo_item_tbl[itemId]->Flags, MENU_ITEM_FLAGS_INIT);
    }
    return 0;
}

static int menu_photo_disable_sel(UINT32 itemId, UINT32 selId)
{
    if (APP_CHECKFLAGS((menu_photo_item_sel_tbls[itemId]+selId)->Flags, MENU_SEL_FLAGS_ENABLE)) {
        APP_REMOVEFLAGS((menu_photo_item_sel_tbls[itemId]+selId)->Flags, MENU_SEL_FLAGS_ENABLE);
        APP_REMOVEFLAGS(menu_photo_item_tbl[itemId]->Flags, MENU_ITEM_FLAGS_INIT);
    }
    return 0;
}

static int menu_photo_lock_sel(UINT32 itemId, UINT32 selId)
{
    APP_ADDFLAGS((menu_photo_item_sel_tbls[itemId]+selId)->Flags, MENU_SEL_FLAGS_LOCKED);
    return 0;
}

static int menu_photo_unlock_sel(UINT32 itemId, UINT32 selId)
{
    APP_REMOVEFLAGS((menu_photo_item_sel_tbls[itemId]+selId)->Flags, MENU_SEL_FLAGS_LOCKED);
    return 0;
}
