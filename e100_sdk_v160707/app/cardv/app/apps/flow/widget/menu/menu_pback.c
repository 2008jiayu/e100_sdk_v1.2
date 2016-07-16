/**
  * @file src/app/apps/flow/widget/menu/connectedcam/menu_pback.c
  *
  *  Implementation of Playback-related Menu Items
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

/*************************************************************************
 * Declaration:
 ************************************************************************/
//tab
static int menu_pback_init(void);
static int menu_pback_start(void);
static int menu_pback_stop(void);
//item
static int menu_pback_file_prot_init(void);
static int menu_pback_file_prot_get_tab_str(void);
static int menu_pback_file_prot_get_sel_str(int ref);
static int menu_pback_file_prot_get_sel_bmp(int ref);
static int menu_pback_file_prot_set(void);
static int menu_pback_file_prot_sel_set(void);
static int menu_pback_video_play_opt_init(void);
static int menu_pback_video_play_opt_get_tab_str(void);
static int menu_pback_video_play_opt_get_sel_str(int ref);
static int menu_pback_video_play_opt_get_sel_bmp(int ref);
static int menu_pback_video_play_opt_set(void);
static int menu_pback_video_play_opt_sel_set(void);
static int menu_pback_photo_play_opt_init(void);
static int menu_pback_photo_play_opt_get_tab_str(void);
static int menu_pback_photo_play_opt_get_sel_str(int ref);
static int menu_pback_photo_play_opt_get_sel_bmp(int ref);
static int menu_pback_photo_play_opt_set(void);
static int menu_pback_photo_play_opt_sel_set(void);
static int menu_pback_slideshow_init(void);
static int menu_pback_slideshow_get_tab_str(void);
static int menu_pback_slideshow_get_sel_str(int ref);
static int menu_pback_slideshow_get_sel_bmp(int ref);
static int menu_pback_slideshow_set(void);
static int menu_pback_slideshow_sel_set(void);
static int menu_pback_slide_show_effect_init(void);
static int menu_pback_slide_show_effect_get_tab_str(void);
static int menu_pback_slide_show_effect_get_sel_str(int ref);
static int menu_pback_slide_show_effect_get_sel_bmp(int ref);
static int menu_pback_slide_show_effect_set(void);
static int menu_pback_slide_show_effect_sel_set(void);
static int menu_pback_real_mov_edt_init(void);
static int menu_pback_real_mov_edt_get_tab_str(void);
static int menu_pback_real_mov_edt_get_sel_str(int ref);
static int menu_pback_real_mov_edt_get_sel_bmp(int ref);
static int menu_pback_real_mov_edt_set(void);
static int menu_pback_real_mov_edt_sel_set(void);
static int menu_pback_delete_file_init(void);
static int menu_pback_delete_file_get_tab_str(void);
static int menu_pback_delete_file_get_sel_str(int ref);
static int menu_pback_delete_file_get_sel_bmp(int ref);
static int menu_pback_delete_file_set(void);
static int menu_pback_delete_file_sel_set(void);
static int menu_pback_postproc_init(void);
static int menu_pback_postproc_get_tab_str(void);
static int menu_pback_postproc_get_sel_str(int ref);
static int menu_pback_postproc_get_sel_bmp(int ref);
static int menu_pback_postproc_set(void);
static int menu_pback_postproc_sel_set(void);
static int menu_pback_postproc_save_init(void);
static int menu_pback_postproc_save_get_tab_str(void);
static int menu_pback_postproc_save_get_sel_str(int ref);
static int menu_pback_postproc_save_get_sel_bmp(int ref);
static int menu_pback_postproc_save_set(void);
static int menu_pback_postproc_save_sel_set(void);
static int menu_pback_sound_play_opt_init(void);
static int menu_pback_sound_play_opt_get_tab_str(void);
static int menu_pback_sound_play_opt_get_sel_str(int ref);
static int menu_pback_sound_play_opt_get_sel_bmp(int ref);
static int menu_pback_sound_play_opt_set(void);
static int menu_pback_sound_play_opt_sel_set(void);
static int menu_pback_sound_background_init(void);
static int menu_pback_sound_background_get_tab_str(void);
static int menu_pback_sound_background_get_sel_str(int ref);
static int menu_pback_sound_background_get_sel_bmp(int ref);
static int menu_pback_sound_background_set(void);
static int menu_pback_sound_background_sel_set(void);
//////////////////////////////////////////////////////////////
static int menu_pback_delete_this_file_init(void);
static int menu_pback_delete_this_file_get_tab_str(void);
static int menu_pback_delete_this_file_get_sel_str(int ref);
static int menu_pback_delete_this_file_get_sel_bmp(int ref);
static int menu_pback_delete_this_file_set(void);
static int menu_pback_delete_this_file_sel_set(void);

static int menu_pback_delete_all_file_init(void);
static int menu_pback_delete_all_file_get_tab_str(void);
static int menu_pback_delete_all_file_get_sel_str(int ref);
static int menu_pback_delete_all_file_get_sel_bmp(int ref);
static int menu_pback_delete_all_file_set(void);
static int menu_pback_delete_all_file_sel_set(void);


////////////////////////////////////////////////////////////
// control
static MENU_TAB_s* menu_pback_get_tab(void);
static MENU_ITEM_s* menu_pback_get_item(UINT32 itemId);
static MENU_SEL_s* menu_pback_get_sel(UINT32 itemId, UINT32 selId);
static int menu_pback_set_sel_table(UINT32 itemId, MENU_SEL_s *selTbl);
static int menu_pback_lock_tab(void);
static int menu_pback_unlock_tab(void);
static int menu_pback_enable_item(UINT32 itemId);
static int menu_pback_disable_item(UINT32 itemId);
static int menu_pback_lock_item(UINT32 itemId);
static int menu_pback_unlock_item(UINT32 itemId);
static int menu_pback_enable_sel(UINT32 itemId, UINT32 selId);
static int menu_pback_disable_sel(UINT32 itemId, UINT32 selId);
static int menu_pback_lock_sel(UINT32 itemId, UINT32 selId);
static int menu_pback_unlock_sel(UINT32 itemId, UINT32 selId);

/*************************************************************************
 * Definition:
 ************************************************************************/
/*** Selection ***/
static MENU_SEL_s menu_pback_file_prot_sel_tbl[MENU_PBACK_FILE_PROTECTION_SEL_NUM] = {
    {MENU_PBACK_FILE_PROTECTION_MULTI, MENU_SEL_FLAGS_ENABLE,
        0, 0, 0,
        MENU_PBACK_FILE_PROTECTION_MULTI, NULL},
    {MENU_PBACK_FILE_PROTECTION_ALL, MENU_SEL_FLAGS_ENABLE,
        0, 0, 0,
        MENU_PBACK_FILE_PROTECTION_ALL, NULL},
    {MENU_PBACK_FILE_PROTECTION_UNPROTECT_ALL, MENU_SEL_FLAGS_ENABLE,
        0, 0, 0,
        MENU_PBACK_FILE_PROTECTION_UNPROTECT_ALL, NULL}
};

static MENU_SEL_s menu_pback_video_play_opt_sel_tbl[MENU_PBACK_VIDEO_PLAY_OPTION_SEL_NUM] = {
    {MENU_PBACK_VIDEO_PLAY_OPTION_ONE, MENU_SEL_FLAGS_ENABLE,
        STR_VIDEO_PLAY_ONE, BMP_ICN_VIDEO_PLAY_ONE, 0,
        PB_OPT_VIDEO_PLAY_ONE, NULL},
    {MENU_PBACK_VIDEO_PLAY_OPTION_ALL, MENU_SEL_FLAGS_ENABLE,
        STR_VIDEO_PLAY_ALL, BMP_ICN_VIDEO_PLAY_ALL, 0,
        PB_OPT_VIDEO_PLAY_ALL, NULL},
#if 0        
    {MENU_PBACK_VIDEO_PLAY_OPTION_REPEAT_ONE, MENU_SEL_FLAGS_ENABLE,
        STR_VIDEO_REPEAT_ONE, BMP_ICN_VIDEO_REPEAT_ONE, 0,
        PB_OPT_VIDEO_REPEAT_ONE, NULL},
    {MENU_PBACK_VIDEO_PLAY_OPTION_REPEAT_ALL, MENU_SEL_FLAGS_ENABLE,
        STR_VIDEO_REPEAT_ALL, BMP_ICN_VIDEO_REPEAT_ALL, 0,
        PB_OPT_VIDEO_REPEAT_ALL, NULL}
#endif
};

static MENU_SEL_s menu_pback_photo_play_opt_sel_tbl[MENU_PBACK_PHOTO_PLAY_OPTION_SEL_NUM] = {
    {MENU_PBACK_PHOTO_PLAY_OPTION_NO_REPEAT, MENU_SEL_FLAGS_ENABLE,
        STR_PHOTO_REPEAT_OFF, BMP_ICN_REPEAT_OFF, 0,
        PB_OPT_PHOTO_REPEAT_OFF, NULL},
    {MENU_PBACK_PHOTO_PLAY_OPTION_REPEAT, MENU_SEL_FLAGS_ENABLE,
        STR_PHOTO_REPEAT_ON, BMP_ICN_REPEAT_ON, 0,
        PB_OPT_PHOTO_REPEAT_ON, NULL}
};

static MENU_SEL_s menu_pback_slideshow_sel_tbl[MENU_PBACK_SLIDESHOW_SEL_NUM] = {
    {MENU_PBACK_SLIDESHOW_OFF, MENU_SEL_FLAGS_ENABLE,
        0, BMP_ICN_SLIDE_SHOW_OFF, 0,
        MENU_PBACK_SLIDESHOW_OFF, NULL},
    {MENU_PBACK_SLIDESHOW_ON, MENU_SEL_FLAGS_ENABLE,
        0, BMP_ICN_SLIDE_SHOW_ON, 0,
        MENU_PBACK_SLIDESHOW_ON, NULL}
};

static MENU_SEL_s menu_pback_slide_show_effect_sel_tbl[MENU_PBACK_SLIDE_SHOW_EFFECT_SEL_NUM] = {
    {MENU_PBACK_SLIDE_SHOW_EFFECT_FADING, MENU_SEL_FLAGS_ENABLE,
        STR_SLIDE_SHOW_FADING, BMP_ICN_SLIDE_SHOW_FADE_IN, 0,
        SLIDESHOW_EFFECT_FADING, NULL},
    {MENU_PBACK_SLIDE_SHOW_EFFECT_FLYING, MENU_SEL_FLAGS_ENABLE,
        STR_SLIDE_SHOW_FLYING, BMP_ICN_SLIDE_SHOW_FLY_IN, 0,
        SLIDESHOW_EFFECT_FLYING, NULL}
};

static MENU_SEL_s menu_pback_real_mov_edt_sel_tbl[MENU_PBACK_REAL_MOVIE_EDIT_SEL_NUM] = {
    {MENU_PBACK_REAL_MOVIE_EDIT_CROP, MENU_SEL_FLAGS_ENABLE,
        STR_VIDEO_EDIT_CROP, 0, 0,
        VIDEO_EDIT_MODE_CROP, NULL},
    {MENU_PBACK_REAL_MOVIE_EDIT_PARTIAL_DEL, MENU_SEL_FLAGS_ENABLE,
        STR_VIDEO_EDIT_PARTIAL_DELETE, 0, 0,
        VIDEO_EDIT_MODE_PARTIAL_DEL, NULL},
    {MENU_PBACK_REAL_MOVIE_EDIT_DIVIDE, MENU_SEL_FLAGS_ENABLE,
        STR_VIDEO_EDIT_DIVIDE, 0, 0,
        VIDEO_EDIT_MODE_DIVIDE, NULL},
    {MENU_PBACK_REAL_MOVIE_EDIT_MERGE, MENU_SEL_FLAGS_ENABLE,
        STR_VIDEO_EDIT_MERGE, 0, 0,
        VIDEO_EDIT_MODE_MERGE, NULL}
};

static MENU_SEL_s menu_pback_delete_file_sel_tbl[MENU_PBACK_DELETE_FILE_SEL_NUM] = {
    {MENU_PBACK_DELETE_FILE_ONE, MENU_SEL_FLAGS_ENABLE,
        STR_DEL_ONE, BMP_ICN_DEL_ONE, 0,
        MENU_PBACK_DELETE_FILE_ONE, NULL},
#ifdef CONFIG_APP_ARD
	{MENU_PBACK_DELETE_FILE_ALL, MENU_SEL_FLAGS_ENABLE,
#else
    {MENU_PBACK_DELETE_FILE_ALL, 0,
#endif
        STR_DEL_ALL, BMP_ICN_DEL_ALL, 0,
        MENU_PBACK_DELETE_FILE_ALL, NULL}
};

static MENU_SEL_s menu_pback_postproc_sel_tbl[MENU_PBACK_POSTPROC_SEL_NUM] = {
    {MENU_PBACK_POSTPROC_RER, 0,
        STR_RED_EYE_REMOVAL, BMP_ICN_AUTO_POWER_OFF, 0,
        MENU_PBACK_POSTPROC_RER, NULL},
    {MENU_PBACK_POSTPROC_DL, 0,
        STR_DYNAMIC_LIGHTING, BMP_ICN_AUTO_POWER_OFF, 0,
        MENU_PBACK_POSTPROC_DL, NULL}
};

static MENU_SEL_s menu_pback_postproc_save_sel_tbl[MENU_PBACK_POSTPROC_SAVE_SEL_NUM] = {
    {MENU_PBACK_POSTPROC_SAVE_SAVE, MENU_SEL_FLAGS_ENABLE,
        STR_SAVE_NEW_FILE, BMP_ICN_AUTO_POWER_OFF, 0,
        MENU_PBACK_POSTPROC_SAVE_SAVE, NULL},
    {MENU_PBACK_POSTPROC_SAVE_REPLACE, MENU_SEL_FLAGS_ENABLE,
        STR_REPLACE_OLD_FILE, BMP_ICN_AUTO_POWER_OFF, 0,
        MENU_PBACK_POSTPROC_SAVE_REPLACE, NULL}
};

static MENU_SEL_s menu_pback_sound_play_opt_sel_tbl[MENU_PBACK_SOUND_PLAY_OPTION_SEL_NUM] = {
    {MENU_PBACK_SOUND_PLAY_OPTION_ONE, MENU_SEL_FLAGS_ENABLE,
        STR_SOUND_PLAY_ONE, BMP_ICN_VIDEO_PLAY_ONE, 0,
        PB_OPT_SOUND_PLAY_ONE, NULL},
    {MENU_PBACK_SOUND_PLAY_OPTION_ALL, MENU_SEL_FLAGS_ENABLE,
        STR_SOUND_PLAY_ALL, BMP_ICN_VIDEO_PLAY_ALL, 0,
        PB_OPT_SOUND_PLAY_ALL, NULL},
    {MENU_PBACK_SOUND_PLAY_OPTION_REPEAT_ONE, MENU_SEL_FLAGS_ENABLE,
        STR_SOUND_REPEAT_ONE, BMP_ICN_VIDEO_REPEAT_ONE, 0,
        PB_OPT_SOUND_REPEAT_ONE, NULL},
    {MENU_PBACK_SOUND_PLAY_OPTION_REPEAT_ALL, MENU_SEL_FLAGS_ENABLE,
        STR_SOUND_REPEAT_ALL, BMP_ICN_VIDEO_REPEAT_ALL, 0,
        PB_OPT_SOUND_REPEAT_ALL, NULL}
};

static MENU_SEL_s menu_pback_sound_background_sel_tbl[MENU_PBACK_SOUND_BACKGROUND_SEL_NUM] = {
    {MENU_PBACK_SOUND_BACKGROUND_LOGO, MENU_SEL_FLAGS_ENABLE,
        STR_SOUND_BACKGROUND_LOGO, BMP_ICN_SLIDE_SHOW_FADE_IN, 0,
        PB_OPT_SOUND_BACKGROUND_LOGO, NULL},
    {MENU_PBACK_SOUND_BACKGROUND_PHOTO, MENU_SEL_FLAGS_ENABLE,
        STR_SOUND_BACKGROUND_PHOTO, BMP_ICN_SLIDE_SHOW_FADE_IN, 0,
        PB_OPT_SOUND_BACKGROUND_PHOTO, NULL}
};

static MENU_SEL_s menu_pback_delete_this_file_sel_tbl[MENU_PBACK_DELETE_FILE_SEL_NUM] = {
    {MENU_PBACK_DELETE_FILE_ONE, MENU_SEL_FLAGS_ENABLE,
        STR_DEL_ONE, BMP_ICN_DEL_ONE, 0,
        MENU_PBACK_DELETE_FILE_ONE, NULL},
#ifdef CONFIG_APP_ARD
	{MENU_PBACK_DELETE_FILE_ALL, MENU_SEL_FLAGS_ENABLE,
#else
    {MENU_PBACK_DELETE_FILE_ALL, 0,
#endif
        STR_DEL_ALL, BMP_ICN_DEL_ALL, 0,
        MENU_PBACK_DELETE_FILE_ALL, NULL}
};
static MENU_SEL_s menu_pback_delete_all_file_sel_tbl[MENU_PBACK_DELETE_FILE_SEL_NUM] = {
    {MENU_PBACK_DELETE_FILE_ONE, MENU_SEL_FLAGS_ENABLE,
        STR_DEL_ONE, BMP_ICN_DEL_ONE, 0,
        MENU_PBACK_DELETE_FILE_ONE, NULL},
#ifdef CONFIG_APP_ARD
	{MENU_PBACK_DELETE_FILE_ALL, MENU_SEL_FLAGS_ENABLE,
#else
    {MENU_PBACK_DELETE_FILE_ALL, 0,
#endif
        STR_DEL_ALL, BMP_ICN_DEL_ALL, 0,
        MENU_PBACK_DELETE_FILE_ALL, NULL}
};


static MENU_SEL_s *menu_pback_item_sel_tbls[MENU_PBACK_ITEM_NUM] = {
    menu_pback_file_prot_sel_tbl,
    menu_pback_video_play_opt_sel_tbl,
    menu_pback_photo_play_opt_sel_tbl,
    menu_pback_slideshow_sel_tbl,
    menu_pback_slide_show_effect_sel_tbl,
    menu_pback_real_mov_edt_sel_tbl,
    menu_pback_delete_file_sel_tbl,
    menu_pback_postproc_sel_tbl,
    menu_pback_postproc_save_sel_tbl,
    menu_pback_sound_play_opt_sel_tbl,
    menu_pback_sound_background_sel_tbl,
    ////////////////////////////////////
    menu_pback_delete_this_file_sel_tbl,
    menu_pback_delete_all_file_sel_tbl,
};

/*** Currently activated object Id arrays ***/
static MENU_SEL_s *menu_pback_file_prot_sels[MENU_PBACK_FILE_PROTECTION_SEL_NUM];
static MENU_SEL_s *menu_pback_video_play_opt_sels[MENU_PBACK_VIDEO_PLAY_OPTION_SEL_NUM];
static MENU_SEL_s *menu_pback_photo_play_opt_sels[MENU_PBACK_PHOTO_PLAY_OPTION_SEL_NUM];
static MENU_SEL_s *menu_pback_slideshow_sels[MENU_PBACK_SLIDESHOW_SEL_NUM];
static MENU_SEL_s *menu_pback_slide_show_effect_sels[MENU_PBACK_SLIDE_SHOW_EFFECT_SEL_NUM];
static MENU_SEL_s *menu_pback_real_mov_edt_sels[MENU_PBACK_REAL_MOVIE_EDIT_SEL_NUM];
static MENU_SEL_s *menu_pback_delete_file_sels[MENU_PBACK_DELETE_FILE_SEL_NUM];
static MENU_SEL_s *menu_pback_postproc_sels[MENU_PBACK_POSTPROC_SEL_NUM];
static MENU_SEL_s *menu_pback_postproc_save_sels[MENU_PBACK_POSTPROC_SAVE_SEL_NUM];
static MENU_SEL_s *menu_pback_sound_play_opt_sels[MENU_PBACK_SOUND_PLAY_OPTION_SEL_NUM];
static MENU_SEL_s *menu_pback_sound_background_sels[MENU_PBACK_SOUND_BACKGROUND_SEL_NUM];
////////////////////////////////////////////////////

static MENU_SEL_s *menu_pback_delete_this_file_sels[MENU_PBACK_DELETE_FILE_SEL_NUM];
static MENU_SEL_s *menu_pback_delete_all_file_sels[MENU_PBACK_DELETE_FILE_SEL_NUM];

///////////////////////////////////////////////////
/*** Item ***/
static MENU_ITEM_s menu_pback_file_prot = {
    MENU_PBACK_FILE_PROTECTION, 0,
    0, 0,
    0, 0,
    0, 0, 0, menu_pback_file_prot_sels,
    menu_pback_file_prot_init,
    menu_pback_file_prot_get_tab_str,
    menu_pback_file_prot_get_sel_str,
    menu_pback_file_prot_get_sel_bmp,
    menu_pback_file_prot_set,
    menu_pback_file_prot_sel_set
};

static MENU_ITEM_s menu_pback_video_play_opt = {
    MENU_PBACK_VIDEO_PLAY_OPTION, MENU_ITEM_FLAGS_ENABLE,
    0, 0,
    0, 0,
    0, 0, 0, menu_pback_video_play_opt_sels,
    menu_pback_video_play_opt_init,
    menu_pback_video_play_opt_get_tab_str,
    menu_pback_video_play_opt_get_sel_str,
    menu_pback_video_play_opt_get_sel_bmp,
    menu_pback_video_play_opt_set,
    menu_pback_video_play_opt_sel_set
};

static MENU_ITEM_s menu_pback_photo_play_opt = {
    MENU_PBACK_PHOTO_PLAY_OPTION, 0,
    0, 0,
    0, 0,
    0, 0, 0, menu_pback_photo_play_opt_sels,
    menu_pback_photo_play_opt_init,
    menu_pback_photo_play_opt_get_tab_str,
    menu_pback_photo_play_opt_get_sel_str,
    menu_pback_photo_play_opt_get_sel_bmp,
    menu_pback_photo_play_opt_set,
    menu_pback_photo_play_opt_sel_set
};

static MENU_ITEM_s menu_pback_slideshow = {
    MENU_PBACK_SLIDESHOW, 0,
    0, 0,
    0, 0,
    0, 0, 0, menu_pback_slideshow_sels,
    menu_pback_slideshow_init,
    menu_pback_slideshow_get_tab_str,
    menu_pback_slideshow_get_sel_str,
    menu_pback_slideshow_get_sel_bmp,
    menu_pback_slideshow_set,
    menu_pback_slideshow_sel_set
};

static MENU_ITEM_s menu_pback_slide_show_effect = {
    MENU_PBACK_SLIDE_SHOW_EFFECT, 0,
    0, 0,
    0, 0,
    0, 0, 0, menu_pback_slide_show_effect_sels,
    menu_pback_slide_show_effect_init,
    menu_pback_slide_show_effect_get_tab_str,
    menu_pback_slide_show_effect_get_sel_str,
    menu_pback_slide_show_effect_get_sel_bmp,
    menu_pback_slide_show_effect_set,
    menu_pback_slide_show_effect_sel_set
};

static MENU_ITEM_s menu_pback_real_mov_edt = {
    MENU_PBACK_REAL_MOVIE_EDIT, 0,
    0, 0,
    0, 0,
    0, 0, 0, menu_pback_real_mov_edt_sels,
    menu_pback_real_mov_edt_init,
    menu_pback_real_mov_edt_get_tab_str,
    menu_pback_real_mov_edt_get_sel_str,
    menu_pback_real_mov_edt_get_sel_bmp,
    menu_pback_real_mov_edt_set,
    menu_pback_real_mov_edt_sel_set
};

static MENU_ITEM_s menu_pback_delete_file = {
    MENU_PBACK_DELETE_FILE, MENU_ITEM_FLAGS_ENABLE,
    0, 0,
    0, 0,
    0, 0, 0, menu_pback_delete_file_sels,
    menu_pback_delete_file_init,
    menu_pback_delete_file_get_tab_str,
    menu_pback_delete_file_get_sel_str,
    menu_pback_delete_file_get_sel_bmp,
    menu_pback_delete_file_set,
    menu_pback_delete_file_sel_set
};

static MENU_ITEM_s menu_pback_postproc = {
    MENU_PBACK_POSTPROC, 0,
    0, 0,
    0, 0,
    0, 0, 0, menu_pback_postproc_sels,
    menu_pback_postproc_init,
    menu_pback_postproc_get_tab_str,
    menu_pback_postproc_get_sel_str,
    menu_pback_postproc_get_sel_bmp,
    menu_pback_postproc_set,
    menu_pback_postproc_sel_set
};

static MENU_ITEM_s menu_pback_postproc_save = {
    MENU_PBACK_POSTPROC_SAVE, 0,
    0, 0,
    0, 0,
    0, 0, 0, menu_pback_postproc_save_sels,
    menu_pback_postproc_save_init,
    menu_pback_postproc_save_get_tab_str,
    menu_pback_postproc_save_get_sel_str,
    menu_pback_postproc_save_get_sel_bmp,
    menu_pback_postproc_save_set,
    menu_pback_postproc_save_sel_set
};

static MENU_ITEM_s menu_pback_sound_play_opt = {
    MENU_PBACK_SOUND_PLAY_OPTION, 0,
    0, 0,
    0, 0,
    0, 0, 0, menu_pback_sound_play_opt_sels,
    menu_pback_sound_play_opt_init,
    menu_pback_sound_play_opt_get_tab_str,
    menu_pback_sound_play_opt_get_sel_str,
    menu_pback_sound_play_opt_get_sel_bmp,
    menu_pback_sound_play_opt_set,
    menu_pback_sound_play_opt_sel_set
};

static MENU_ITEM_s menu_pback_sound_background = {
    MENU_PBACK_SOUND_BACKGROUND, 0,
    0, 0,
    0, 0,
    0, 0, 0, menu_pback_sound_background_sels,
    menu_pback_sound_background_init,
    menu_pback_sound_background_get_tab_str,
    menu_pback_sound_background_get_sel_str,
    menu_pback_sound_background_get_sel_bmp,
    menu_pback_sound_background_set,
    menu_pback_sound_background_sel_set
};

static MENU_ITEM_s menu_pback_delete_this_file = {
    MENU_PBACK_DELETE_THIS_FILE, MENU_ITEM_FLAGS_ENABLE,
    0, 0,
    0, 0,
    0, 0, 0, menu_pback_delete_this_file_sels,
    menu_pback_delete_this_file_init,
    menu_pback_delete_this_file_get_tab_str,
    menu_pback_delete_this_file_get_sel_str,
    menu_pback_delete_this_file_get_sel_bmp,
    menu_pback_delete_this_file_set,
    menu_pback_delete_this_file_sel_set
};
 
static MENU_ITEM_s menu_pback_delete_all_file = {
    MENU_PBACK_DELETE_ALL_FILE, MENU_ITEM_FLAGS_ENABLE,
    0, 0,
    0, 0,
    0, 0, 0, menu_pback_delete_all_file_sels,
    menu_pback_delete_all_file_init,
    menu_pback_delete_all_file_get_tab_str,
    menu_pback_delete_all_file_get_sel_str,
    menu_pback_delete_all_file_get_sel_bmp,
    menu_pback_delete_all_file_set,
    menu_pback_delete_all_file_sel_set
};

static MENU_ITEM_s *menu_pback_item_tbl[MENU_PBACK_ITEM_NUM] = {
    &menu_pback_file_prot,
    &menu_pback_video_play_opt,
    &menu_pback_photo_play_opt,
    &menu_pback_slideshow,
    &menu_pback_slide_show_effect,
    &menu_pback_real_mov_edt,
    &menu_pback_delete_file,
    &menu_pback_postproc,
    &menu_pback_postproc_save,
    &menu_pback_sound_play_opt,
    &menu_pback_sound_background,
    /////////////////////////////
    &menu_pback_delete_this_file,
    &menu_pback_delete_all_file,
};

/*** Currently activated object Id arrays ***/
static MENU_ITEM_s *menu_pback_items[MENU_PBACK_ITEM_NUM];

/*** Tab ***/
static MENU_TAB_s menu_pback = {
    MENU_ECL_PBACK, MENU_TAB_FLAGS_ENABLE,
    0, 0,
    BMP_MENU_TAB_PLAY, BMP_MENU_TAB_PLAY_HL,
    menu_pback_items,
    menu_pback_init,
    menu_pback_start,
    menu_pback_stop
};

MENU_TAB_CTRL_s menu_pback_ctrl = {
    menu_pback_get_tab,
    menu_pback_get_item,
    menu_pback_get_sel,
    menu_pback_set_sel_table,
    menu_pback_lock_tab,
    menu_pback_unlock_tab,
    menu_pback_enable_item,
    menu_pback_disable_item,
    menu_pback_lock_item,
    menu_pback_unlock_item,
    menu_pback_enable_sel,
    menu_pback_disable_sel,
    menu_pback_lock_sel,
    menu_pback_unlock_sel
};

/*** APIs ***/
// tab
static int menu_pback_init(void)
{
    int i = 0;
    UINT32 cur_item_id = 0;

    // Playback
    APP_ADDFLAGS(menu_pback.Flags, MENU_TAB_FLAGS_INIT);
    if (menu_pback.ItemNum > 0) {
        cur_item_id = menu_pback_items[menu_pback.ItemCur]->Id;
    }

    menu_pback_disable_item(MENU_PBACK_PHOTO_PLAY_OPTION);
    menu_pback_disable_item(MENU_PBACK_SLIDESHOW);
    menu_pback_disable_item(MENU_PBACK_SLIDE_SHOW_EFFECT);
    
    menu_pback_disable_item(MENU_PBACK_VIDEO_PLAY_OPTION);
    menu_pback_disable_item(MENU_PBACK_DELETE_FILE);
#ifdef CONFIG_APP_ARD
	//menu_pback_enable_item(MENU_PBACK_DELETE_FILE);
#endif

    menu_pback.ItemNum = 0;
    menu_pback.ItemCur = 0;
    for (i=0; i<MENU_PBACK_ITEM_NUM; i++) {
        if (APP_CHECKFLAGS(menu_pback_item_tbl[i]->Flags, MENU_ITEM_FLAGS_ENABLE)) {
            menu_pback_items[menu_pback.ItemNum] = menu_pback_item_tbl[i];
            if (cur_item_id == menu_pback_item_tbl[i]->Id) {
                menu_pback.ItemCur = menu_pback.ItemNum;
            }
            menu_pback.ItemNum++;
        }
    }

    return 0;
}

static int menu_pback_start(void)
{
    return 0;
}

static int menu_pback_stop(void)
{
    return 0;
}

// item
static int menu_pback_file_prot_init(void)
{
    int i = 0;

    APP_ADDFLAGS(menu_pback_file_prot.Flags, MENU_ITEM_FLAGS_INIT);
    menu_pback_file_prot.SelNum = 0;
    menu_pback_file_prot.SelCur = 0;
    for (i=0; i<MENU_PBACK_FILE_PROTECTION_SEL_NUM; i++) {
        if (APP_CHECKFLAGS((menu_pback_item_sel_tbls[MENU_PBACK_FILE_PROTECTION]+i)->Flags, MENU_SEL_FLAGS_ENABLE)) {
            menu_pback_file_prot_sels[menu_pback_file_prot.SelNum] = menu_pback_item_sel_tbls[MENU_PBACK_FILE_PROTECTION]+i;
            menu_pback_file_prot.SelNum++;
        }
    }

    return 0;
}

static int menu_pback_file_prot_get_tab_str(void)
{

    return 0;
}

static int menu_pback_file_prot_get_sel_str(int ref)
{
    return menu_pback_file_prot_sels[ref]->Str;
}

static int menu_pback_file_prot_get_sel_bmp(int ref)
{
    return menu_pback_file_prot_sels[ref]->Bmp;
}

static int menu_pback_file_prot_set(void)
{
    //if (sys_status.card_act > 0) {
        AppMenuQuick_SetItem(MENU_PBACK, MENU_PBACK_FILE_PROTECTION);
        menu_pback_file_prot.SelCur = 0;
        //curapp->switch_to(APP_THUMB_PICKUP);
    //}
    return 0;
}

static int menu_pback_file_prot_sel_set(void)
{
    return 0;
}

static int menu_pback_video_play_opt_init(void)
{
    int i = 0;

    APP_ADDFLAGS(menu_pback_video_play_opt.Flags, MENU_ITEM_FLAGS_INIT);
    menu_pback_video_play_opt.SelSaved = 0;
    menu_pback_video_play_opt.SelNum = 0;
    menu_pback_video_play_opt.SelCur = 0;
    for (i=0; i<MENU_PBACK_VIDEO_PLAY_OPTION_SEL_NUM; i++) {
        if (APP_CHECKFLAGS((menu_pback_item_sel_tbls[MENU_PBACK_VIDEO_PLAY_OPTION]+i)->Flags, MENU_SEL_FLAGS_ENABLE)) {
            menu_pback_video_play_opt_sels[menu_pback_video_play_opt.SelNum] = menu_pback_item_sel_tbls[MENU_PBACK_VIDEO_PLAY_OPTION]+i;
            if (menu_pback_video_play_opt_sels[menu_pback_video_play_opt.SelNum]->Val == UserSetting->PlaybackPref.VideoPlayOpt) {
            menu_pback_video_play_opt.SelSaved = menu_pback_video_play_opt.SelNum;
            menu_pback_video_play_opt.SelCur = menu_pback_video_play_opt.SelNum;
        }
            menu_pback_video_play_opt.SelNum++;
        }
    }

    return 0;
}

static int menu_pback_video_play_opt_get_tab_str(void)
{
    return menu_pback_video_play_opt_sels[menu_pback_video_play_opt.SelSaved]->Str;
}

static int menu_pback_video_play_opt_get_sel_str(int ref)
{
    return menu_pback_video_play_opt_sels[ref]->Str;
}

static int menu_pback_video_play_opt_get_sel_bmp(int ref)
{
    return menu_pback_video_play_opt_sels[ref]->Bmp;
}

static int menu_pback_video_play_opt_set(void)
{
    AppMenuQuick_SetItem(MENU_ECL_PBACK, MENU_PBACK_VIDEO_PLAY_OPTION);
    AppWidget_On(WIDGET_MENU_QUICK, 0);
    return 0;
}

static int menu_pback_video_play_opt_sel_set(void)
{
    int pre_dis;
    if (menu_pback_video_play_opt.SelSaved != menu_pback_video_play_opt.SelCur) {
        menu_pback_video_play_opt.SelSaved = menu_pback_video_play_opt.SelCur;
	 pre_dis = UserSetting->PlaybackPref.VideoPlayOpt;	
        UserSetting->PlaybackPref.VideoPlayOpt = (APP_PREF_VIDOE_PLAY_OPT_e)menu_pback_video_play_opt_sels[menu_pback_video_play_opt.SelCur]->Val;
	 record_item_sel(UserSetting->PlaybackPref.VideoPlayOpt,pre_dis);	
    }
    return 0;
}

static int menu_pback_photo_play_opt_init(void)
{
    int i = 0;

    APP_ADDFLAGS(menu_pback_photo_play_opt.Flags, MENU_ITEM_FLAGS_INIT);
    menu_pback_photo_play_opt.SelSaved = 0;
    menu_pback_photo_play_opt.SelNum = 0;
    menu_pback_photo_play_opt.SelCur = 0;
    for (i=0; i<MENU_PBACK_PHOTO_PLAY_OPTION_SEL_NUM; i++) {
        if (APP_CHECKFLAGS((menu_pback_item_sel_tbls[MENU_PBACK_PHOTO_PLAY_OPTION]+i)->Flags, MENU_SEL_FLAGS_ENABLE)) {
            menu_pback_photo_play_opt_sels[menu_pback_photo_play_opt.SelNum] = menu_pback_item_sel_tbls[MENU_PBACK_PHOTO_PLAY_OPTION]+i;
            if (menu_pback_photo_play_opt_sels[menu_pback_photo_play_opt.SelNum]->Val == UserSetting->PlaybackPref.PhotoPlayOpt) {
                menu_pback_photo_play_opt.SelSaved = menu_pback_photo_play_opt.SelNum;
                menu_pback_photo_play_opt.SelCur = menu_pback_photo_play_opt.SelNum;
            }
            menu_pback_photo_play_opt.SelNum++;
        }
    }

    return 0;
}

static int menu_pback_photo_play_opt_get_tab_str(void)
{
    return menu_pback_photo_play_opt_sels[menu_pback_photo_play_opt.SelSaved]->Str;
}

static int menu_pback_photo_play_opt_get_sel_str(int ref)
{
    return menu_pback_photo_play_opt_sels[ref]->Str;
}

static int menu_pback_photo_play_opt_get_sel_bmp(int ref)
{
    return menu_pback_photo_play_opt_sels[ref]->Bmp;
}

static int menu_pback_photo_play_opt_set(void)
{
    AppMenuQuick_SetItem(MENU_PBACK, MENU_PBACK_PHOTO_PLAY_OPTION);
    AppWidget_On(WIDGET_MENU_QUICK, 0);
    return 0;
}

static int menu_pback_photo_play_opt_sel_set(void)
{
    UserSetting->PlaybackPref.PhotoPlayOpt = (APP_PREF_PHOTO_PLAY_OPT_e)menu_pback_photo_play_opt_sels[menu_pback_photo_play_opt.SelCur]->Val;
    menu_pback_photo_play_opt.SelSaved = menu_pback_photo_play_opt.SelCur;
    return 0;
}

static int menu_pback_slideshow_init(void)
{
    int i = 0;

    APP_ADDFLAGS(menu_pback_slideshow.Flags, MENU_ITEM_FLAGS_INIT);
    menu_pback_slideshow.SelSaved = 0;
    menu_pback_slideshow.SelNum = 0;
    menu_pback_slideshow.SelCur = 0;
    for (i=0; i<MENU_PBACK_SLIDESHOW_SEL_NUM; i++) {
        if (APP_CHECKFLAGS((menu_pback_item_sel_tbls[MENU_PBACK_SLIDESHOW]+i)->Flags, MENU_SEL_FLAGS_ENABLE)) {
            menu_pback_slideshow_sels[menu_pback_slideshow.SelNum] = menu_pback_item_sel_tbls[MENU_PBACK_SLIDESHOW]+i;
            if (menu_pback_slideshow_sels[menu_pback_slideshow.SelNum]->Val == 0/*UserSetting->slideshow*/) {
                menu_pback_slideshow.SelSaved = menu_pback_slideshow.SelNum;
                menu_pback_slideshow.SelCur = menu_pback_slideshow.SelNum;
            }
            menu_pback_slideshow.SelNum++;
        }
    }

    return 0;
}

static int menu_pback_slideshow_get_tab_str(void)
{
    return menu_pback_slideshow_sels[menu_pback_slideshow.SelSaved]->Str;
}

static int menu_pback_slideshow_get_sel_str(int ref)
{
    return menu_pback_slideshow_sels[ref]->Str;
}

static int menu_pback_slideshow_get_sel_bmp(int ref)
{
    return menu_pback_slideshow_sels[ref]->Bmp;
}

static int menu_pback_slideshow_set(void)
{
    AppMenuQuick_SetItem(MENU_SETUP, MENU_PBACK_SLIDESHOW);
    return 0;
}

static int menu_pback_slideshow_sel_set(void)
{
    if (menu_pback_slideshow.SelSaved != menu_pback_slideshow.SelCur) {
        menu_pback_slideshow.SelSaved = menu_pback_slideshow.SelCur;
        UserSetting->PlaybackPref.SlideshowMode = menu_pback_slideshow_sels[menu_pback_slideshow.SelCur]->Val;
    }
    return 0;
}

static int menu_pback_slide_show_effect_init(void)
{
    int i = 0;

    APP_ADDFLAGS(menu_pback_slide_show_effect.Flags, MENU_ITEM_FLAGS_INIT);
    menu_pback_slide_show_effect.SelSaved = 0;
    menu_pback_slide_show_effect.SelNum = 0;
    menu_pback_slide_show_effect.SelCur = 0;
    for (i=0; i<MENU_PBACK_SLIDE_SHOW_EFFECT_SEL_NUM; i++) {
        if (APP_CHECKFLAGS((menu_pback_item_sel_tbls[MENU_PBACK_SLIDE_SHOW_EFFECT]+i)->Flags, MENU_SEL_FLAGS_ENABLE)) {
            menu_pback_slide_show_effect_sels[menu_pback_slide_show_effect.SelNum] = menu_pback_item_sel_tbls[MENU_PBACK_SLIDE_SHOW_EFFECT]+i;
            if (menu_pback_slide_show_effect_sels[menu_pback_slide_show_effect.SelNum]->Val == UserSetting->PlaybackPref.SlideshowEffectType) {
                menu_pback_slide_show_effect.SelSaved = menu_pback_slide_show_effect.SelNum;
                menu_pback_slide_show_effect.SelCur = menu_pback_slide_show_effect.SelNum;
            }
            menu_pback_slide_show_effect.SelNum++;
        }
    }

    return 0;
}

static int menu_pback_slide_show_effect_get_tab_str(void)
{
    return menu_pback_slide_show_effect_sels[menu_pback_slide_show_effect.SelSaved]->Str;
}

static int menu_pback_slide_show_effect_get_sel_str(int ref)
{
    return menu_pback_slide_show_effect_sels[ref]->Str;
}

static int menu_pback_slide_show_effect_get_sel_bmp(int ref)
{
    return menu_pback_slide_show_effect_sels[ref]->Bmp;
}

static int menu_pback_slide_show_effect_set(void)
{
    AppMenuQuick_SetItem(MENU_PBACK, MENU_PBACK_SLIDE_SHOW_EFFECT);
    AppWidget_On(WIDGET_MENU_QUICK, 0);
    return 0;
}

static int menu_pback_slide_show_effect_sel_set(void)
{
    UserSetting->PlaybackPref.SlideshowEffectType = (APP_PREF_PHOTO_SLIDESHOW_EFFECT_e)menu_pback_slide_show_effect_sels[menu_pback_slide_show_effect.SelCur]->Val;
    menu_pback_slide_show_effect.SelSaved = menu_pback_slide_show_effect.SelCur;
    return 0;
}

static int menu_pback_real_mov_edt_init(void)
{
    int i = 0;

    APP_ADDFLAGS(menu_pback_real_mov_edt.Flags, MENU_ITEM_FLAGS_INIT);
    menu_pback_real_mov_edt.SelNum = 0;
    menu_pback_real_mov_edt.SelCur = 0;
    for (i=0; i<MENU_PBACK_REAL_MOVIE_EDIT_SEL_NUM; i++) {
        if (APP_CHECKFLAGS((menu_pback_item_sel_tbls[MENU_PBACK_REAL_MOVIE_EDIT]+i)->Flags, MENU_SEL_FLAGS_ENABLE)) {
            menu_pback_real_mov_edt_sels[menu_pback_real_mov_edt.SelNum] = menu_pback_item_sel_tbls[MENU_PBACK_REAL_MOVIE_EDIT]+i;
            menu_pback_real_mov_edt.SelNum++;
        }
    }

    return 0;
}

static int menu_pback_real_mov_edt_get_tab_str(void)
{
    return STR_VIDEO_EDIT;
}

static int menu_pback_real_mov_edt_get_sel_str(int ref)
{
    return menu_pback_real_mov_edt_sels[ref]->Str;
}

static int menu_pback_real_mov_edt_get_sel_bmp(int ref)
{
    return menu_pback_real_mov_edt_sels[ref]->Bmp;
}

static int menu_pback_real_mov_edt_set(void)
{
    AppMenuQuick_SetItem(MENU_PBACK, MENU_PBACK_REAL_MOVIE_EDIT);
    AppWidget_On(WIDGET_MENU_QUICK, 0);

    return 0;
}

static int menu_pback_real_mov_edt_sel_set(void)
{
    app_status.VideoEditMode = menu_pback_real_mov_edt_sels[menu_pback_real_mov_edt.SelCur]->Val;
    {
        /* Send the message to the current app. */
        APP_APP_s *curapp;
        AppAppMgt_GetCurApp(&curapp);
        curapp->OnMessage(AMSG_CMD_SET_VIDEO_EDITOR, 1, 0);
    }

    return 0;
}

static int menu_pback_delete_file_init(void)
{
    int i = 0;

    APP_ADDFLAGS(menu_pback_delete_file.Flags, MENU_ITEM_FLAGS_INIT);
    menu_pback_delete_file.SelNum = 0;
    menu_pback_delete_file.SelCur = 0;
    for (i=0; i<MENU_PBACK_DELETE_FILE_SEL_NUM; i++) {
        if (APP_CHECKFLAGS((menu_pback_item_sel_tbls[MENU_PBACK_DELETE_FILE]+i)->Flags, MENU_SEL_FLAGS_ENABLE)) {
            menu_pback_delete_file_sels[menu_pback_delete_file.SelNum] = menu_pback_item_sel_tbls[MENU_PBACK_DELETE_FILE]+i;
            menu_pback_delete_file.SelNum++;
        }
    }

    return 0;
}

static int menu_pback_delete_file_get_tab_str(void)
{
    return STR_DEL_FILE;
}

static int menu_pback_delete_file_get_sel_str(int ref)
{
    return menu_pback_delete_file_sels[ref]->Str;
}

static int menu_pback_delete_file_get_sel_bmp(int ref)
{
    return menu_pback_delete_file_sels[ref]->Bmp;
}

static int menu_pback_delete_file_set(void)
{
    ///* Send the message to the current app. */
    //APP_APP_s *curapp;
    //AppAppMgt_GetCurApp(&curapp);
    //curapp->OnMessage(AMSG_CMD_SET_DELETE_FILE, 1, menu_pback_delete_file_sels[menu_pback_delete_file.SelCur]->Val);
    AppMenuQuick_SetItem(MENU_ECL_PBACK, MENU_PBACK_DELETE_FILE);
    AppWidget_On(WIDGET_MENU_QUICK, 0);
    return 0;
}

static int menu_pback_delete_file_sel_set(void)
{
    /* Send the message to the current app. */
    APP_APP_s *curapp;
    AppAppMgt_GetCurApp(&curapp);
    curapp->OnMessage(AMSG_CMD_SET_DELETE_FILE, 0,menu_pback_delete_file_sels[menu_pback_delete_file.SelCur]->Val);	
    AppWidget_Off(WIDGET_ALL, 0);
    if(AppWidget_GetPback() == WIDGET_MENU)
        AppWidget_SetPback(WIDGET_NONE);            
    return 0;
}

static int menu_pback_postproc_init(void)
{
    int i = 0;

#if defined(ENABLE_REDEYE_REMOVAL)
    menu_pback_enable_sel(MENU_PBACK_POSTPROC, MENU_PBACK_POSTPROC_RER);
#endif
#if defined(ENABLE_DYNAMIC_LIGHTING)
    menu_pback_enable_sel(MENU_PBACK_POSTPROC, MENU_PBACK_POSTPROC_DL);
#endif

    APP_ADDFLAGS(menu_pback_postproc.Flags, MENU_ITEM_FLAGS_INIT);
    menu_pback_postproc.SelNum = 0;
    menu_pback_postproc.SelCur = 0;
    for (i=0; i<MENU_PBACK_POSTPROC_SEL_NUM; i++) {
        if (APP_CHECKFLAGS((menu_pback_item_sel_tbls[MENU_PBACK_POSTPROC]+i)->Flags, MENU_SEL_FLAGS_ENABLE)) {
            menu_pback_postproc_sels[menu_pback_postproc.SelNum] = menu_pback_item_sel_tbls[MENU_PBACK_POSTPROC]+i;
            menu_pback_postproc.SelNum++;
        }
    }

    return 0;
}

static int menu_pback_postproc_get_tab_str(void)
{
    return STR_IMAGE_PROCESSING;
}

static int menu_pback_postproc_get_sel_str(int ref)
{
    return menu_pback_postproc_sels[ref]->Str;
}

static int menu_pback_postproc_get_sel_bmp(int ref)
{
    return menu_pback_postproc_sels[ref]->Bmp;
}

static int menu_pback_postproc_set(void)
{
    AppMenuQuick_SetItem(MENU_PBACK, MENU_PBACK_POSTPROC);
    AppWidget_On(WIDGET_MENU_QUICK, 0);
    return 0;
}

static int menu_pback_postproc_sel_set(void)
{
    if (menu_pback_postproc_sels[menu_pback_postproc.SelCur]->Val == MENU_PBACK_POSTPROC_RER) {
        /* Send the message to the current app. */
        APP_APP_s *curapp;
        AppAppMgt_GetCurApp(&curapp);
        //curapp->OnMessage(AMSG_CMD_DO_REDEYE_REMOVAL, 0, 0);
    } else if (menu_pback_postproc_sels[menu_pback_postproc.SelCur]->Val == MENU_PBACK_POSTPROC_DL) {
        /* Send the message to the current app. */
        APP_APP_s *curapp;
        AppAppMgt_GetCurApp(&curapp);
        //curapp->OnMessage(AMSG_CMD_DO_DYNAMIC_LIGHTING, 0, 0);
    }
    AppWidget_Off(WIDGET_ALL, 0);
    return 0;
}

static int menu_pback_postproc_save_init(void)
{
    int i = 0;

    APP_ADDFLAGS(menu_pback_postproc_save.Flags, MENU_ITEM_FLAGS_INIT);
    menu_pback_postproc_save.SelNum = 0;
    menu_pback_postproc_save.SelCur = 0;
    for (i=0; i<MENU_PBACK_POSTPROC_SAVE_SEL_NUM; i++) {
        if (APP_CHECKFLAGS((menu_pback_item_sel_tbls[MENU_PBACK_POSTPROC_SAVE]+i)->Flags, MENU_SEL_FLAGS_ENABLE)) {
            menu_pback_postproc_save_sels[menu_pback_postproc_save.SelNum] = menu_pback_item_sel_tbls[MENU_PBACK_POSTPROC_SAVE]+i;
            menu_pback_postproc_save.SelNum++;
        }
    }

    return 0;
}

static int menu_pback_postproc_save_get_tab_str(void)
{
    return STR_RED_EYE_REMOVAL;
}

static int menu_pback_postproc_save_get_sel_str(int ref)
{
    return menu_pback_postproc_save_sels[ref]->Str;
}

static int menu_pback_postproc_save_get_sel_bmp(int ref)
{
    return menu_pback_postproc_save_sels[ref]->Bmp;
}

static int menu_pback_postproc_save_set(void)
{
    /* Send the message to the current app. */
    APP_APP_s *curapp;
    AppAppMgt_GetCurApp(&curapp);
    //curapp->OnMessage(AMSG_CMD_SET_REDEYE_REMOVAL_SAVE, 1, menu_pback_postproc_save_sels[menu_pback_postproc_save.SelCur]->Val);
    return 0;
}

static int menu_pback_postproc_save_sel_set(void)
{
    /* Send the message to the current app. */
    APP_APP_s *curapp;
    AppAppMgt_GetCurApp(&curapp);
    //curapp->OnMessage(AMSG_CMD_SET_REDEYE_REMOVAL_SAVE, 0, menu_pback_postproc_save_sels[menu_pback_postproc_save.SelCur]->Val);
    AppWidget_Off(WIDGET_ALL, 0);
    return 0;
}

static int menu_pback_sound_play_opt_init(void)
{
    int i = 0;

    APP_ADDFLAGS(menu_pback_sound_play_opt.Flags, MENU_ITEM_FLAGS_INIT);
    menu_pback_sound_play_opt.SelSaved = 0;
    menu_pback_sound_play_opt.SelNum = 0;
    menu_pback_sound_play_opt.SelCur = 0;
    for (i=0; i<MENU_PBACK_SOUND_PLAY_OPTION_SEL_NUM; i++) {
        if (APP_CHECKFLAGS((menu_pback_item_sel_tbls[MENU_PBACK_SOUND_PLAY_OPTION]+i)->Flags, MENU_SEL_FLAGS_ENABLE)) {
            menu_pback_sound_play_opt_sels[menu_pback_sound_play_opt.SelNum] = menu_pback_item_sel_tbls[MENU_PBACK_SOUND_PLAY_OPTION]+i;
            if (menu_pback_sound_play_opt_sels[menu_pback_sound_play_opt.SelNum]->Val == UserSetting->PlaybackPref.SoundPlayOpt) {
                menu_pback_sound_play_opt.SelSaved = menu_pback_sound_play_opt.SelNum;
                menu_pback_sound_play_opt.SelCur = menu_pback_sound_play_opt.SelNum;
            }
            menu_pback_sound_play_opt.SelNum++;
        }
    }

    return 0;
}

static int menu_pback_sound_play_opt_get_tab_str(void)
{
    return menu_pback_sound_play_opt_sels[menu_pback_sound_play_opt.SelSaved]->Str;
}

static int menu_pback_sound_play_opt_get_sel_str(int ref)
{
    return menu_pback_sound_play_opt_sels[ref]->Str;
}

static int menu_pback_sound_play_opt_get_sel_bmp(int ref)
{
    return menu_pback_sound_play_opt_sels[ref]->Bmp;
}

static int menu_pback_sound_play_opt_set(void)
{
    AppMenuQuick_SetItem(MENU_PBACK, MENU_PBACK_SOUND_PLAY_OPTION);
    AppWidget_On(WIDGET_MENU_QUICK, 0);
    return 0;
}

static int menu_pback_sound_play_opt_sel_set(void)
{
    if (menu_pback_sound_play_opt.SelSaved != menu_pback_sound_play_opt.SelCur) {
        menu_pback_sound_play_opt.SelSaved = menu_pback_sound_play_opt.SelCur;
        UserSetting->PlaybackPref.SoundPlayOpt = (APP_PREF_SOUND_PLAY_OPT_e)menu_pback_sound_play_opt_sels[menu_pback_sound_play_opt.SelCur]->Val;
    }
    return 0;
}

static int menu_pback_sound_background_init(void)
{
    int i = 0;

    APP_ADDFLAGS(menu_pback_sound_background.Flags, MENU_ITEM_FLAGS_INIT);
    menu_pback_sound_background.SelSaved = 0;
    menu_pback_sound_background.SelNum = 0;
    menu_pback_sound_background.SelCur = 0;
    for (i=0; i<MENU_PBACK_SOUND_BACKGROUND_SEL_NUM; i++) {
        if (APP_CHECKFLAGS((menu_pback_item_sel_tbls[MENU_PBACK_SOUND_BACKGROUND]+i)->Flags, MENU_SEL_FLAGS_ENABLE)) {
            menu_pback_sound_background_sels[menu_pback_sound_background.SelNum] = menu_pback_item_sel_tbls[MENU_PBACK_SOUND_BACKGROUND]+i;
            if (menu_pback_sound_background_sels[menu_pback_sound_background.SelNum]->Val == UserSetting->PlaybackPref.BackgroundSoundType) {
                menu_pback_sound_background.SelSaved = menu_pback_sound_background.SelNum;
                menu_pback_sound_background.SelCur = menu_pback_sound_background.SelNum;
            }
            menu_pback_sound_background.SelNum++;
        }
    }

    return 0;
}

static int menu_pback_sound_background_get_tab_str(void)
{
    return menu_pback_sound_background_sels[menu_pback_sound_background.SelSaved]->Str;
}

static int menu_pback_sound_background_get_sel_str(int ref)
{
    return menu_pback_sound_background_sels[ref]->Str;
}

static int menu_pback_sound_background_get_sel_bmp(int ref)
{
    return menu_pback_sound_background_sels[ref]->Bmp;
}

static int menu_pback_sound_background_set(void)
{
    AppMenuQuick_SetItem(MENU_PBACK, MENU_PBACK_SOUND_BACKGROUND);
    AppWidget_On(WIDGET_MENU_QUICK, 0);
    return 0;
}

static int menu_pback_sound_background_sel_set(void)
{
    int param = 0;

    if (UserSetting->PlaybackPref.BackgroundSoundType != menu_pback_sound_background_sels[menu_pback_sound_background.SelCur]->Val) {
        param = 1;
    }
    UserSetting->PlaybackPref.BackgroundSoundType = (APP_PREF_SOUND_BACKGROUND_TYPE_e)menu_pback_sound_background_sels[menu_pback_sound_background.SelCur]->Val;
    menu_pback_sound_background.SelSaved = menu_pback_sound_background.SelCur;
    if (param) {
        /* Re-open background, if sound_background_type is changed*/
        APP_APP_s *curapp;
        AppAppMgt_GetCurApp(&curapp);
        //curapp->OnMessage(AMSG_CMD_RESET_SOUND_APP_BG, param, 0);
    }
    return 0;
}

// control
static MENU_TAB_s* menu_pback_get_tab(void)
{
    return &menu_pback;
}

static MENU_ITEM_s* menu_pback_get_item(UINT32 itemId)
{
    return menu_pback_item_tbl[itemId];
}

static MENU_SEL_s* menu_pback_get_sel(UINT32 itemId, UINT32 selId)
{
    return &menu_pback_item_sel_tbls[itemId][selId];
}

static int menu_pback_set_sel_table(UINT32 itemId, MENU_SEL_s *selTbl)
{
    menu_pback_item_sel_tbls[itemId] = selTbl;
    APP_REMOVEFLAGS(menu_pback_item_tbl[itemId]->Flags, MENU_ITEM_FLAGS_INIT);
    return 0;
}

static int menu_pback_lock_tab(void)
{
    APP_ADDFLAGS(menu_pback.Flags, MENU_TAB_FLAGS_LOCKED);
    return 0;
}

static int menu_pback_unlock_tab(void)
{
    APP_REMOVEFLAGS(menu_pback.Flags, MENU_TAB_FLAGS_LOCKED);
    return 0;
}

static int menu_pback_enable_item(UINT32 itemId)
{
    if (!APP_CHECKFLAGS(menu_pback_item_tbl[itemId]->Flags, MENU_ITEM_FLAGS_ENABLE)) {
        APP_ADDFLAGS(menu_pback_item_tbl[itemId]->Flags, MENU_ITEM_FLAGS_ENABLE);
        APP_REMOVEFLAGS(menu_pback.Flags, MENU_TAB_FLAGS_INIT);
    }
    return 0;
}

static int menu_pback_disable_item(UINT32 itemId)
{
    if (APP_CHECKFLAGS(menu_pback_item_tbl[itemId]->Flags, MENU_ITEM_FLAGS_ENABLE)) {
        APP_REMOVEFLAGS(menu_pback_item_tbl[itemId]->Flags, MENU_ITEM_FLAGS_ENABLE);
        APP_REMOVEFLAGS(menu_pback.Flags, MENU_TAB_FLAGS_INIT);
    }
    return 0;
}

static int menu_pback_lock_item(UINT32 itemId)
{
    APP_ADDFLAGS(menu_pback_item_tbl[itemId]->Flags, MENU_ITEM_FLAGS_LOCKED);
    return 0;
}

static int menu_pback_unlock_item(UINT32 itemId)
{
    APP_REMOVEFLAGS(menu_pback_item_tbl[itemId]->Flags, MENU_ITEM_FLAGS_LOCKED);
    return 0;
}

static int menu_pback_enable_sel(UINT32 itemId, UINT32 selId)
{
    if (!APP_CHECKFLAGS((menu_pback_item_sel_tbls[itemId]+selId)->Flags, MENU_SEL_FLAGS_ENABLE)) {
        APP_ADDFLAGS((menu_pback_item_sel_tbls[itemId]+selId)->Flags, MENU_SEL_FLAGS_ENABLE);
        APP_REMOVEFLAGS(menu_pback_item_tbl[itemId]->Flags, MENU_ITEM_FLAGS_INIT);
    }
    return 0;
}

static int menu_pback_disable_sel(UINT32 itemId, UINT32 selId)
{
    if (APP_CHECKFLAGS((menu_pback_item_sel_tbls[itemId]+selId)->Flags, MENU_SEL_FLAGS_ENABLE)) {
        APP_REMOVEFLAGS((menu_pback_item_sel_tbls[itemId]+selId)->Flags, MENU_SEL_FLAGS_ENABLE);
        APP_REMOVEFLAGS(menu_pback_item_tbl[itemId]->Flags, MENU_ITEM_FLAGS_INIT);
    }
    return 0;
}

static int menu_pback_lock_sel(UINT32 itemId, UINT32 selId)
{
    APP_ADDFLAGS((menu_pback_item_sel_tbls[itemId]+selId)->Flags, MENU_SEL_FLAGS_LOCKED);
    return 0;
}

static int menu_pback_unlock_sel(UINT32 itemId, UINT32 selId)
{
    APP_REMOVEFLAGS((menu_pback_item_sel_tbls[itemId]+selId)->Flags, MENU_SEL_FLAGS_LOCKED);
    return 0;
}


////////////////////////////////////////////////////////
static int menu_pback_delete_this_file_init(void)
{
    int i = 0;

    APP_ADDFLAGS(menu_pback_delete_this_file.Flags, MENU_ITEM_FLAGS_INIT);
    menu_pback_delete_this_file.SelNum = 0;
    menu_pback_delete_this_file.SelCur = 0;
    for (i=0; i<MENU_PBACK_DELETE_FILE_SEL_NUM; i++) {
        if (APP_CHECKFLAGS((menu_pback_item_sel_tbls[MENU_PBACK_DELETE_THIS_FILE]+i)->Flags, MENU_SEL_FLAGS_ENABLE)) {
            menu_pback_delete_this_file_sels[menu_pback_delete_this_file.SelNum] = menu_pback_item_sel_tbls[MENU_PBACK_DELETE_THIS_FILE]+i;
            menu_pback_delete_this_file.SelNum++;
        }
    }

    return 0;
}

static int menu_pback_delete_this_file_get_tab_str(void)
{
    return STR_DEL_ONE;
}

static int menu_pback_delete_this_file_get_sel_str(int ref)
{
    return STR_DEL_FILE;
}

static int menu_pback_delete_this_file_get_sel_bmp(int ref)
{
    return BMP_0_NULL;
}

static int menu_pback_delete_this_file_set(void)
{
    
   // AppMenuQuick_SetItem(MENU_ECL_PBACK, MENU_PBACK_DELETE_FILE);
   // AppWidget_On(WIDGET_MENU_QUICK, 0);
   #if 1 
    APP_APP_s *curapp;
    AppAppMgt_GetCurApp(&curapp);
    curapp->OnMessage(AMSG_CMD_SET_DELETE_FILE, 0,0);	
    AppWidget_Off(WIDGET_ALL, 0);
    if(AppWidget_GetPback() == WIDGET_MENU)
        AppWidget_SetPback(WIDGET_NONE);      
    #endif
    return 0;
    
}
static int menu_pback_delete_this_file_sel_set(void)
{
#if 0
    /* Send the message to the current app. */
    APP_APP_s *curapp;
    AppAppMgt_GetCurApp(&curapp);
    curapp->OnMessage(AMSG_CMD_SET_DELETE_FILE, 0,menu_pback_delete_file_sels[menu_pback_delete_file.SelCur]->Val);	
    
    AppWidget_Off(WIDGET_ALL, 0);
    if(AppWidget_GetPback() == WIDGET_MENU)
        AppWidget_SetPback(WIDGET_NONE);   
#endif

    return 0;
}

static int menu_pback_delete_all_file_init(void)
{
    int i = 0;

    APP_ADDFLAGS(menu_pback_delete_this_file.Flags, MENU_ITEM_FLAGS_INIT);
    menu_pback_delete_this_file.SelNum = 0;
    menu_pback_delete_this_file.SelCur = 0;
    for (i=0; i<MENU_PBACK_DELETE_FILE_SEL_NUM; i++) {
        if (APP_CHECKFLAGS((menu_pback_item_sel_tbls[MENU_PBACK_DELETE_THIS_FILE]+i)->Flags, MENU_SEL_FLAGS_ENABLE)) {
            menu_pback_delete_this_file_sels[menu_pback_delete_this_file.SelNum] = menu_pback_item_sel_tbls[MENU_PBACK_DELETE_THIS_FILE]+i;
            menu_pback_delete_this_file.SelNum++;
        }
    }

    return 0;
}

static int menu_pback_delete_all_file_get_tab_str(void)
{
    return STR_DEL_ALL;
}

static int menu_pback_delete_all_file_get_sel_str(int ref)
{
    return STR_DEL_FILE;
}

static int menu_pback_delete_all_file_get_sel_bmp(int ref)
{
    return BMP_0_NULL;
}

static int menu_pback_delete_all_file_set(void)
{
    
   // AppMenuQuick_SetItem(MENU_ECL_PBACK, MENU_PBACK_DELETE_FILE);
   // AppWidget_On(WIDGET_MENU_QUICK, 0);
 
    APP_APP_s *curapp;
    AppAppMgt_GetCurApp(&curapp);
    curapp->OnMessage(AMSG_CMD_SET_DELETE_FILE, 0,1);	
    AppWidget_Off(WIDGET_ALL, 0);
    if(AppWidget_GetPback() == WIDGET_MENU)
        AppWidget_SetPback(WIDGET_NONE);    
    return 0;
    
}
static int menu_pback_delete_all_file_sel_set(void)
{
#if 0
    /* Send the message to the current app. */
    APP_APP_s *curapp;
    AppAppMgt_GetCurApp(&curapp);
    curapp->OnMessage(AMSG_CMD_SET_DELETE_FILE, 0,menu_pback_delete_file_sels[menu_pback_delete_file.SelCur]->Val);	
    
    AppWidget_Off(WIDGET_ALL, 0);
    if(AppWidget_GetPback() == WIDGET_MENU)
        AppWidget_SetPback(WIDGET_NONE);   
#endif

    return 0;
}



