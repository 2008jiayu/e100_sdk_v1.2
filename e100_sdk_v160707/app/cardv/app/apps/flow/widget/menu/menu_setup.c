/**
  * @file src/app/apps/flow/widget/menu/connectedcam/menu_setup.c
  *
  * Implementation of Setup-related Menu Items
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
//#pragma O0
#include <apps/flow/rec/rec_cam.h>

#include <apps/apps.h>
#include <system/status.h>
#include <system/app_pref.h>
#include <system/app_util.h>
#include <apps/flow/widget/menu/widget.h>
#include <apps/gui/resource/gui_settle.h>
#include <apps/flow/widget/menu/widget.h>
#include <system/ApplibSys_Lcd.h>
#include <AmbaRTC.h>
#ifdef CONFIG_APP_ARD
#include <apps/flow/widget/menu/menu_check_box.h>
//#include "../../../../../applib/src/graphics/graphics.h"
//#include "../../../../../applib/src/graphics/inc/ApplibGraphics_MainTask.h"
//#include "../../../../../applib/inc/graphics/ApplibGraphics.h"
#endif

/*************************************************************************
 * Declaration:
 ************************************************************************/
// tab
static int menu_setup_init(void);
static int menu_setup_start(void);
static int menu_setup_stop(void);
// item
#ifdef CONFIG_APP_ARD
static int menu_setup_driver_id_init(void);
static int menu_setup_driver_id_get_tab_str(void);
static int menu_setup_driver_id_set(void);
#endif
static int menu_setup_system_type_init(void);
static int menu_setup_system_type_get_tab_str(void);
static int menu_setup_system_type_get_sel_str(int ref);
static int menu_setup_system_type_get_sel_bmp(int ref);
static int menu_setup_system_type_set(void);
static int menu_setup_system_type_sel_set(void);
static int menu_setup_dmf_mode_init(void);
static int menu_setup_dmf_mode_get_tab_str(void);
static int menu_setup_dmf_mode_get_sel_str(int ref);
static int menu_setup_dmf_mode_get_sel_bmp(int ref);
static int menu_setup_dmf_mode_set(void);
static int menu_setup_dmf_mode_sel_set(void);
static int menu_setup_time_init(void);
static int menu_setup_time_get_tab_str(void);
static int menu_setup_time_set(void);
static int menu_setup_date_time_disp_init(void);
static int menu_setup_date_time_disp_get_tab_str(void);
static int menu_setup_date_time_disp_get_sel_str(int ref);
static int menu_setup_date_time_disp_get_sel_bmp(int ref);
static int menu_setup_date_time_disp_set(void);
static int menu_setup_date_time_disp_sel_set(void);
static int menu_setup_format_init(void);
static int menu_setup_format_get_tab_str(void);
static int menu_setup_format_set(void);
static int menu_setup_default_init(void);
static int menu_setup_default_get_tab_str(void);
static int menu_setup_default_set(void);
static int menu_setup_lcd_ctrl_init(void);
static int menu_setup_lcd_ctrl_get_tab_str(void);
static int menu_setup_lcd_ctrl_get_sel_str(int ref);
static int menu_setup_lcd_ctrl_get_sel_bmp(int ref);
static int menu_setup_lcd_ctrl_set(void);
static int menu_setup_lcd_ctrl_sel_set(void);
static int menu_setup_lcd_brightness_get_cur_val(void);
static int menu_setup_lcd_brightness_get_val_str(int val);
static int menu_setup_lcd_brightness_val_set(int val);
static int menu_setup_lcd_contrast_get_cur_val(void);
static int menu_setup_lcd_contrast_get_val_str(int val);
static int menu_setup_lcd_contrast_val_set(int val);
static int menu_setup_lcd_colorbalance_get_cur_val(void);
static int menu_setup_lcd_colorbalance_get_val_str(int val);
static int menu_setup_lcd_colorbalance_val_set(int val);
static int menu_setup_poweroff_init(void);
static int menu_setup_poweroff_get_tab_str(void);
static int menu_setup_poweroff_get_sel_str(int ref);
static int menu_setup_poweroff_get_sel_bmp(int ref);
static int menu_setup_poweroff_set(void);
static int menu_setup_poweroff_sel_set(void);
static int menu_setup_powersaving_init(void);
static int menu_setup_powersaving_get_tab_str(void);
static int menu_setup_powersaving_get_sel_str(int ref);
static int menu_setup_powersaving_get_sel_bmp(int ref);
static int menu_setup_powersaving_set(void);
static int menu_setup_powersaving_sel_set(void);
static int menu_setup_hdmi_init(void);
static int menu_setup_hdmi_get_tab_str(void);
static int menu_setup_hdmi_get_sel_str(int ref);
static int menu_setup_hdmi_get_sel_bmp(int ref);
static int menu_setup_hdmi_set(void);
static int menu_setup_hdmi_sel_set(void);
static int menu_setup_flashlight_init(void);
static int menu_setup_flashlight_get_tab_str(void);
static int menu_setup_flashlight_get_sel_str(int ref);
static int menu_setup_flashlight_get_sel_bmp(int ref);
static int menu_setup_flashlight_set(void);
static int menu_setup_flashlight_sel_set(void);
static int menu_setup_usb_mode_init(void);
static int menu_setup_usb_mode_get_tab_str(void);
static int menu_setup_usb_mode_get_sel_str(int ref);
static int menu_setup_usb_mode_get_sel_bmp(int ref);
static int menu_setup_usb_mode_set(void);
static int menu_setup_usb_mode_sel_set(void);
static int menu_setup_input_dimension_mode_init(void);
static int menu_setup_input_dimension_mode_get_tab_str(void);
static int menu_setup_input_dimension_mode_get_sel_str(int ref);
static int menu_setup_input_dimension_mode_get_sel_bmp(int ref);
static int menu_setup_input_dimension_mode_set(void);
static int menu_setup_input_dimension_mode_sel_set(void);
#ifdef CONFIG_APP_ARD
static int menu_setup_input_near_far_init(void);
static int menu_setup_input_near_far_get_tab_str(void);
static int menu_setup_input_near_far_get_sel_str(int ref);
static int menu_setup_input_near_far_get_sel_bmp(int ref);
static int menu_setup_input_near_far_set(void);
static int menu_setup_input_near_far_sel_set(void);
static int menu_setup_audio_volume_init(void);
static int menu_setup_audio_volume_get_tab_str(void);
static int menu_setup_audio_volume_get_sel_str(int ref);
static int menu_setup_audio_volume_get_sel_bmp(int ref);
static int menu_setup_audio_volume_set(void);
static int menu_setup_audio_volume_sel_set(void);
static int menu_setup_audio_volume_get_cur_val(void);
static int menu_setup_audio_volume_get_val_str(int val);
static int menu_setup_audio_volume_val_set(int val);
#endif
#ifdef CONFIG_APP_ARD
static int menu_setup_delay_power_off_init(void);
static int menu_setup_delay_power_off_get_tab_str(void);
static int menu_setup_delay_power_off_get_sel_str(int ref);
static int menu_setup_delay_power_off_get_sel_bmp(int ref);
static int menu_setup_delay_power_off_set(void);
static int menu_setup_delay_power_off_sel_set(void);
#endif
static int menu_setup_output_dimension_mode_init(void);
static int menu_setup_output_dimension_mode_get_tab_str(void);
static int menu_setup_output_dimension_mode_get_sel_str(int ref);
static int menu_setup_output_dimension_mode_get_sel_bmp(int ref);
static int menu_setup_output_dimension_mode_set(void);
static int menu_setup_output_dimension_mode_sel_set(void);
static int menu_setup_wifi_init(void);
static int menu_setup_wifi_get_tab_str(void);
static int menu_setup_wifi_get_sel_str(int ref);
static int menu_setup_wifi_get_sel_bmp(int ref);
static int menu_setup_wifi_set(void);
static int menu_setup_wifi_sel_set(void);
static int menu_setup_wifi_config_init(void);
static int menu_setup_wifi_config_get_tab_str(void);
static int menu_setup_wifi_config_get_sel_str(int ref);
static int menu_setup_wifi_config_get_sel_bmp(int ref);
static int menu_setup_wifi_config_set(void);
static int menu_setup_wifi_config_sel_set(void);
static int menu_setup_version_init(void);
static int menu_setup_version_get_tab_str(void);
static int menu_setup_version_set(void);
#ifdef CONFIG_APP_ARD
static int menu_setup_language_init(void);
static int menu_setup_language_get_tab_str(void);
static int menu_setup_language_get_sel_str(int ref);
static int menu_setup_language_get_sel_bmp(int ref);
static int menu_setup_language_set(void);
static int menu_setup_language_sel_set(void);
static int menu_setup_record_mode_init(void);
static int menu_setup_record_mode_get_tab_str(void);
static int menu_setup_record_mode_get_sel_str(int ref);
static int menu_setup_record_mode_get_sel_bmp(int ref);
static int menu_setup_record_mode_set(void);
static int menu_setup_record_mode_sel_set(void);
static int menu_setup_card_volume_init(void);
static int menu_setup_card_volume_get_tab_str(void);
static int menu_setup_card_volume_set(void);
static int menu_setup_beep_sound_init(void);
static int menu_setup_beep_sound_get_tab_str(void);
static int menu_setup_beep_sound_get_sel_str(int ref);
static int menu_setup_beep_sound_get_sel_bmp(int ref);
static int menu_setup_beep_sound_set(void);
static int menu_setup_beep_sound_sel_set(void);
static int menu_setup_backlight_auto_init(void);
static int menu_setup_backlight_auto_get_tab_str(void);
static int menu_setup_backlight_auto_get_sel_str(int ref);
static int menu_setup_backlight_auto_get_sel_bmp(int ref);
static int menu_setup_backlight_auto_set(void);
static int menu_setup_backlight_auto_sel_set(void);
static CHECK_BOX_ITEM_S Setup_Stamp_Choose_Item[MENU_SETUP_STAMP_CHOOSE_NUM] = {0};
static int menu_setup_stamp_choose_init(void);
static int menu_setup_stamp_choose_get_tab_str(void);
static int menu_setup_stamp_choose_set(void);
#endif

// control
static MENU_TAB_s* menu_setup_get_tab(void);
static MENU_ITEM_s* menu_setup_get_item(UINT32 itemId);
static MENU_SEL_s* menu_setup_get_sel(UINT32 itemId, UINT32 selId);
static int menu_setup_set_sel_table(UINT32 itemId, MENU_SEL_s *selTbl);
static int menu_setup_lock_tab(void);
static int menu_setup_unlock_tab(void);
static int menu_setup_enable_item(UINT32 itemId);
static int menu_setup_disable_item(UINT32 itemId);
static int menu_setup_lock_item(UINT32 itemId);
static int menu_setup_unlock_item(UINT32 itemId);
static int menu_setup_enable_sel(UINT32 itemId, UINT32 selId);
static int menu_setup_disable_sel(UINT32 itemId, UINT32 selId);
static int menu_setup_lock_sel(UINT32 itemId, UINT32 selId);
static int menu_setup_unlock_sel(UINT32 itemId, UINT32 selId);

#ifdef CONFIG_ECL_GUI
static int menu_adas_cali_init(void);
static int menu_adas_cali_start(void);
static int menu_adas_cali_stop(void);
static int menu_adas_func_init(void);
static int menu_adas_func_start(void);
static int menu_adas_func_stop(void);
static int menu_adas_alarm_dis_init(void);
static int menu_adas_alarm_dis_start(void);
static int menu_adas_alarm_dis_stop(void);
static int menu_recorder_set_init(void);
static int menu_recorder_set_start(void);
static int menu_recorder_set_stop(void);
static int menu_version_init(void);
static int menu_version_start(void);
static int menu_version_stop(void);
static int menu_default_init(void);
static int menu_default_start(void);
static int menu_default_stop(void);
static int menu_adas_font_car_alarm_init(void);
static int menu_adas_font_car_alarm_get_tab_str(void);
static int menu_adas_font_car_alarm_set(void);
static int menu_adas_change_road_init(void);
static int menu_adas_change_road_get_tab_str(void);
static int menu_adas_change_road_set(void);
static int menu_adas_keep_dis_init(void);
static int menu_adas_keep_dis_get_tab_str(void);
static int menu_adas_keep_dis_set(void);
static int menu_adas_start_alarm_init(void);
static int menu_adas_start_alarm_get_tab_str(void);
static int menu_adas_start_alarm_set(void);
static int menu_adas_alarm_near_init(void);
static int menu_adas_alarm_near_get_tab_str(void);
static int menu_adas_alarm_near_set(void);
static int menu_adas_alarm_middle_init(void);
static int menu_adas_alarm_middle_get_tab_str(void);
static int menu_adas_alarm_middle_set(void);
static int menu_adas_alarm_far_init(void);
static int menu_adas_alarm_far_get_tab_str(void);
static int menu_adas_alarm_far_set(void);
static int menu_adas_start_cali_init(void);
static int menu_adas_start_cali_get_tab_str(void);
static int menu_adas_start_cali_set(void);
static int menu_adas_start_cali_sel_set(void);
static int menu_adas_auto_cali_init(void);
static int menu_adas_auto_cali_get_tab_str(void);
static int menu_adas_auto_cali_set(void);
static int menu_setup_chinese_init(void);
static int menu_setup_chinese_get_tab_str(void);
static int menu_setup_chinese_set(void);
static int menu_setup_english_init(void);
static int menu_setup_english_get_tab_str(void);
static int menu_setup_english_set(void);
static MENU_TAB_s* menu_adas_cali_tab(void);
static MENU_ITEM_s* menu_adas_cali_item(UINT32 itemId);
static MENU_TAB_s* menu_adas_func_tab(void);
static MENU_ITEM_s* menu_adas_func_item(UINT32 itemId);
static MENU_TAB_s* menu_adas_alarm_dis_tab(void);
static MENU_ITEM_s* menu_adas_alarm_dis_item(UINT32 itemId);
static MENU_TAB_s* menu_recorder_set_tab(void);
static MENU_ITEM_s* menu_recorder_set_item(UINT32 itemId);
static MENU_SEL_s* menu_recorder_set_get_sel(UINT32 itemId, UINT32 selId);

static MENU_TAB_s* menu_version_tab(void);
static MENU_ITEM_s* menu_version_item(UINT32 itemId);
static MENU_TAB_s* menu_default_tab(void);
static MENU_ITEM_s* menu_default_item(UINT32 itemId);
extern MENU_ITEM_s menu_video_motion_detect;
extern MENU_ITEM_s menu_video_gsensor_sensitivity;
extern MENU_ITEM_s menu_video_parkingmode_sensitivity;
extern MENU_ITEM_s menu_video_split_rec;
extern MENU_ITEM_s menu_video_flicker;
#endif

/*************************************************************************
 * Definition:
 ************************************************************************/
/** Selection */
static MENU_SEL_s menu_setup_system_type_sel_tbl[MENU_SETUP_SYSTEM_TYPE_SEL_NUM] = {
    {MENU_SETUP_SYSTEM_TYPE_NTSC, MENU_SEL_FLAGS_ENABLE,
        STR_SYSTEM_NTSC, BMP_ICN_SYSTEM_NTSC, 0,
        VOUT_SYS_NTSC, NULL},
    {MENU_SETUP_SYSTEM_TYPE_PAL, MENU_SEL_FLAGS_ENABLE,
        STR_SYSTEM_PAL, BMP_ICN_SYSTEM_PAL, 0,
        VOUT_SYS_PAL, NULL}
};

static MENU_SEL_s menu_setup_dmf_mode_sel_tbl[MENU_SETUP_DMF_MODE_SEL_NUM] = {
    {MENU_SETUP_DMF_MODE_RESET, MENU_SEL_FLAGS_ENABLE,
        STR_FILE_NUMBER_RESET, BMP_ICN_DMF_RESET, 0,
        MENU_SETUP_DMF_MODE_RESET, NULL},
    {MENU_SETUP_DMF_MODE_SERIAL, MENU_SEL_FLAGS_ENABLE,
        STR_FILE_NUMBER_SERIAL, BMP_ICN_DMF_SERIAL, 0,
        MENU_SETUP_DMF_MODE_SERIAL, NULL}
};

static MENU_SEL_s menu_setup_date_time_disp_sel_tbl[MENU_SETUP_DATE_TIME_DISPLAY_SEL_NUM] = {
    {MENU_SETUP_DATE_TIME_DISPLAY_OFF, MENU_SEL_FLAGS_ENABLE,
        0, 0, 0,
        MENU_SETUP_DATE_TIME_DISPLAY_OFF, NULL},
    {MENU_SETUP_DATE_TIME_DISPLAY_DATE, MENU_SEL_FLAGS_ENABLE,
        0, 0, 0,
        MENU_SETUP_DATE_TIME_DISPLAY_DATE, NULL},
    {MENU_SETUP_DATE_TIME_DISPLAY_TIME, MENU_SEL_FLAGS_ENABLE,
        0, 0, 0,
        MENU_SETUP_DATE_TIME_DISPLAY_TIME, NULL},
    {MENU_SETUP_DATE_TIME_DISPLAY_BOTH, MENU_SEL_FLAGS_ENABLE,
        0, 0, 0,
        MENU_SETUP_DATE_TIME_DISPLAY_BOTH, NULL}
};

#ifdef CONFIG_APP_ARD
static MENU_MAIN_ADJ_s menu_setup_audio_volume_adj = {
    8, 0, 64,
    menu_setup_audio_volume_get_cur_val,
    menu_setup_audio_volume_get_val_str,
    menu_setup_audio_volume_val_set
};
static MENU_SEL_s menu_setup_audio_volume_sel_tbl = {
    0, MENU_SEL_FLAGS_ENABLE,
    STR_AUDIO_VOLUME, BMP_ICN_VOL_0, BMP_ICN_VOL_100,
    0x40, &menu_setup_audio_volume_adj
};
#endif

static MENU_MAIN_ADJ_s menu_setup_lcd_brightness_adj = {
    5, 0, 255,
    menu_setup_lcd_brightness_get_cur_val,
    menu_setup_lcd_brightness_get_val_str,
    menu_setup_lcd_brightness_val_set
};

static MENU_MAIN_ADJ_s menu_setup_lcd_contrast_adj = {
    5, 0, 255,
    menu_setup_lcd_contrast_get_cur_val,
    menu_setup_lcd_contrast_get_val_str,
    menu_setup_lcd_contrast_val_set
};

static MENU_MAIN_ADJ_s menu_setup_lcd_colorbalance_adj = {
    5, 0, 255,
    menu_setup_lcd_colorbalance_get_cur_val,
    menu_setup_lcd_colorbalance_get_val_str,
    menu_setup_lcd_colorbalance_val_set
};

static MENU_SEL_s menu_setup_lcd_ctrl_sel_tbl[MENU_SETUP_LCD_CONTROL_SEL_NUM] = {
    {MENU_SETUP_LCD_CONTROL_BRIGHTNESS, MENU_SEL_FLAGS_ENABLE,
        STR_LCD_BRIGHTNESS, BMP_ICN_BRIGHTNESS_LOW, BMP_ICN_BRIGHTNESS_HIGH,
        65, &menu_setup_lcd_brightness_adj},
    {MENU_SETUP_LCD_CONTROL_CONTRAST, MENU_SEL_FLAGS_ENABLE,
        STR_LCD_CONTRAST, BMP_ICN_BRIGHTNESS_LOW, BMP_ICN_BRIGHTNESS_HIGH,
        65, &menu_setup_lcd_contrast_adj},
    {MENU_SETUP_LCD_CONTROL_COLOR_BALANCE, MENU_SEL_FLAGS_ENABLE,
        STR_LCD_COLORBALANCE, BMP_ICN_BRIGHTNESS_LOW, BMP_ICN_BRIGHTNESS_HIGH,
        125, &menu_setup_lcd_colorbalance_adj}
};

static MENU_SEL_s menu_setup_poweroff_sel_tbl[MENU_SETUP_POWEROFF_SEL_NUM] = {
    {MENU_SETUP_POWEROFF_OFF, MENU_SEL_FLAGS_ENABLE,
        STR_POWEROFF_OFF, BMP_ICN_AUTO_POWER_OFF, 0,
        AUTO_POWEROFF_OFF, NULL},
#ifdef CONFIG_APP_ARD
    {MENU_SETUP_POWEROFF_1M, MENU_SEL_FLAGS_ENABLE,
        STR_POWEROFF_1_MIN, BMP_ICN_AUTO_POWER_OFF_1, 0,
        AUTO_POWEROFF_TIME_1M, NULL},
#endif
    {MENU_SETUP_POWEROFF_3M, MENU_SEL_FLAGS_ENABLE,
        STR_POWEROFF_3_MIN, BMP_ICN_AUTO_POWER_OFF_3, 0,
        AUTO_POWEROFF_TIME_3M, NULL},
    {MENU_SETUP_POWEROFF_5M, MENU_SEL_FLAGS_ENABLE,
        STR_POWEROFF_5_MIN, BMP_ICN_AUTO_POWER_OFF_5, 0,
        AUTO_POWEROFF_TIME_5M, NULL}
};

static MENU_SEL_s menu_setup_powersaving_sel_tbl[MENU_SETUP_POWERSAVING_SEL_NUM] = {
    {MENU_SETUP_POWERSAVING_OFF, MENU_SEL_FLAGS_ENABLE,
        STR_POWERSAVING_OFF, BMP_ICN_AUTO_POWER_OFF, 0,
        POWERSAVING_OFF, NULL},
    {MENU_SETUP_POWERSAVING_ON, MENU_SEL_FLAGS_ENABLE,
        STR_POWERSAVING_ON, BMP_ICN_AUTO_POWER_OFF, 0,
        POWERSAVING_ON, NULL}
};

#ifdef CONFIG_APP_ARD
static MENU_SEL_s menu_setup_delay_power_off_sel_tbl[MENU_SETUP_DELAY_POWER_OFF_SEL_NUM] = {
    {MENU_SETUP_DELAY_POWER_OFF_CLOSE, MENU_SEL_FLAGS_ENABLE,
        STR_DELAY_POWEROFF_OFF, BMP_ICN_AUTO_POWER_OFF, 0,
        DELAY_POWEROFF_OFF, NULL},
    {MENU_SETUP_DELAY_POWER_OFF_3_S, MENU_SEL_FLAGS_ENABLE,
        STR_DELAY_POWEROFF_SEC_3, BMP_ICN_DELAY_POWER_OFF_3S, 0,
        DELAY_POWEROFF_3S, NULL},
    {MENU_SETUP_DELAY_POWER_OFF_5_S, MENU_SEL_FLAGS_ENABLE,
        STR_DELAY_POWEROFF_SEC_5, BMP_ICN_DELAY_POWER_OFF_5S, 0,
        DELAY_POWEROFF_5S, NULL},
    {MENU_SETUP_DELAY_POWER_OFF_10_S, MENU_SEL_FLAGS_ENABLE,
        STR_DELAY_POWEROFF_SEC_10, BMP_ICN_DELAY_POWER_OFF_10S, 0,
        DELAY_POWEROFF_10S, NULL},
    {MENU_SETUP_DELAY_POWER_OFF_15_S, MENU_SEL_FLAGS_ENABLE,
        STR_DELAY_POWEROFF_SEC_15, BMP_ICN_DELAY_POWER_OFF_15S, 0,
        DELAY_POWEROFF_15S, NULL}
};
#endif


static MENU_SEL_s menu_setup_hdmi_sel_tbl[MENU_SETUP_POWERSAVING_SEL_NUM] = {
    {MENU_SETUP_HDMI_OFF, MENU_SEL_FLAGS_ENABLE,
        STR_HDMISUPPORT_OFF, BMP_ICN_HDMI_OFF, 0,
        MENU_SETUP_HDMI_OFF, NULL},
    {MENU_SETUP_HDMI_ON, MENU_SEL_FLAGS_ENABLE,
        STR_HDMISUPPORT_ON, BMP_ICN_HDMI_OFF, 0,
        MENU_SETUP_HDMI_ON, NULL}
};

static MENU_SEL_s menu_setup_flashlight_sel_tbl[MENU_SETUP_FLASHLIGHT_SEL_NUM] = {
    {MENU_SETUP_FLASHLIGHT_OFF, MENU_SEL_FLAGS_ENABLE,
        STR_FLASHLIGHT_OFF, BMP_ICN_FLASHLIGHT_OFF, 0,
        MENU_SETUP_FLASHLIGHT_OFF, NULL},
    {MENU_SETUP_FLASHLIGHT_ON, MENU_SEL_FLAGS_ENABLE,
        STR_FLASHLIGHT_ON, BMP_ICN_FLASHLIGHT_ON, 0,
        MENU_SETUP_FLASHLIGHT_ON, NULL},
    {MENU_SETUP_FLASHLIGHT_AUTO, MENU_SEL_FLAGS_ENABLE,
        STR_FLASHLIGHT_AUTO, BMP_ICN_FLASHLIGHT_AUTO, 0,
        MENU_SETUP_FLASHLIGHT_AUTO, NULL}
};

static MENU_SEL_s menu_setup_usb_mode_sel_tbl[MENU_SETUP_USB_MODE_SEL_NUM] = {
    {MENU_SETUP_USB_MODE_MSC, MENU_SEL_FLAGS_ENABLE,
        STR_USB_MASS_STORAGE, BMP_ICN_USB_MSC, 0,
        USB_MODE_MSC, NULL},
    {MENU_SETUP_USB_MODE_AMAGE, MENU_SEL_FLAGS_LOCKED,  // #ifdef CONFIG_APP_ARD
        STR_USB_IMAGE, BMP_ICN_USB_IMAGE, 0,
        USB_MODE_AMAGE, NULL},
};

static MENU_SEL_s menu_setup_input_dimension_mode_sel_tbl[MENU_SETUP_INPUT_DIMENSION_MODE_SEL_NUM] = {
    {MENU_SETUP_INPUT_2D_R, MENU_SEL_FLAGS_ENABLE,
        STR_INPUT_DIMENSION_2D_R, BMP_ICN_2D_R, 0,
        MENU_SETUP_INPUT_2D_R, NULL},
    {MENU_SETUP_INPUT_2D_L, MENU_SEL_FLAGS_ENABLE,
        STR_INPUT_DIMENSION_2D_L, BMP_ICN_2D_L, 0,
        MENU_SETUP_INPUT_2D_L, NULL},
    {MENU_SETUP_INPUT_3D, MENU_SEL_FLAGS_ENABLE,
        STR_INPUT_DIMENSION_3D, BMP_ICN_3D, 0,
        MENU_SETUP_INPUT_3D, NULL}
};

#ifdef CONFIG_APP_ARD
static MENU_SEL_s menu_setup_input_near_far_sel_tbl[MENU_SETUP_INPUT_NEAR_FAR_SEL_NUM] = {
    {MENU_SETUP_INPUT_NEAR, MENU_SEL_FLAGS_ENABLE,
        STR_INPUT_NEAR_SENSOR, BMP_ICN_2D_R, 0,
        MENU_SETUP_INPUT_NEAR, NULL},
    {MENU_SETUP_INPUT_FAR, MENU_SEL_FLAGS_ENABLE,
        STR_INPUT_FAR_SENSOR, BMP_ICN_2D_L, 0,
        MENU_SETUP_INPUT_FAR, NULL}
};
#endif

static MENU_SEL_s menu_setup_output_dimension_mode_sel_tbl[MENU_SETUP_OUTPUT_DIMENSION_MODE_SEL_NUM] = {
    {MENU_SETUP_OUTPUT_2D, MENU_SEL_FLAGS_ENABLE,
        STR_OUTPUT_DIMENSION_2D, BMP_ICN_2D, 0,
        MENU_SETUP_OUTPUT_2D, NULL},
    {MENU_SETUP_OUTPUT_2D_RIGHT, MENU_SEL_FLAGS_ENABLE,
        STR_OUTPUT_DIMENSION_2D_RIGHT, BMP_ICN_2D_R, 0,
        MENU_SETUP_OUTPUT_2D_RIGHT, NULL},
    {MENU_SETUP_OUTPUT_2D_LEFT, MENU_SEL_FLAGS_ENABLE,
        STR_OUTPUT_DIMENSION_2D_LEFT, BMP_ICN_2D_L, 0,
        MENU_SETUP_OUTPUT_2D_LEFT, NULL},
    {MENU_SETUP_OUTPUT_3D, MENU_SEL_FLAGS_ENABLE,
        STR_OUTPUT_DIMENSION_3D, BMP_ICN_3D, 0,
        MENU_SETUP_OUTPUT_3D, NULL}
};

static MENU_SEL_s menu_setup_wifi_sel_tbl[MENU_SETUP_WIFI_SEL_NUM] = {
    {MENU_SETUP_WIFI_OFF, MENU_SEL_FLAGS_ENABLE,
        STR_WIFI_OFF, BMP_ICN_WIFI_OFF, 0,
        MENU_SETUP_WIFI_OFF, NULL},
    {MENU_SETUP_WIFI_ON, MENU_SEL_FLAGS_ENABLE,
        STR_WIFI_ON, BMP_ICN_WIFI, 0,
        MENU_SETUP_WIFI_ON, NULL},
    {MENU_SETUP_WIFI_ON_AP, MENU_SEL_FLAGS_ENABLE,
        STR_WIFI_ON_AP, BMP_ICN_WIFI, 0,
        MENU_SETUP_WIFI_ON_AP, NULL}
};

static MENU_SEL_s menu_setup_wifi_config_sel_tbl[MENU_SETUP_WIFI_CONFIG_SEL_NUM] = {
    {MENU_SETUP_WIFI_CONFIG_0, MENU_SEL_FLAGS_LOCKED,
        STR_WIFI_AP_0, BMP_ICN_WIFI_AP_0, 0,
        0, NULL},
    {MENU_SETUP_WIFI_CONFIG_1, MENU_SEL_FLAGS_LOCKED,
        STR_WIFI_AP_1, BMP_ICN_WIFI_AP_1, 0,
        0, NULL},
    {MENU_SETUP_WIFI_CONFIG_2, MENU_SEL_FLAGS_LOCKED,
        STR_WIFI_AP_2, BMP_ICN_WIFI_AP_2, 0,
        0, NULL},
    {MENU_SETUP_WIFI_CONFIG_3, MENU_SEL_FLAGS_LOCKED,
        STR_WIFI_AP_3, BMP_ICN_WIFI_AP_3, 0,
        0, NULL},
    {MENU_SETUP_WIFI_CONFIG_4, MENU_SEL_FLAGS_LOCKED,
        STR_WIFI_AP_4, BMP_ICN_WIFI_AP_4, 0,
        0, NULL}
};
#ifdef CONFIG_APP_ARD
static MENU_SEL_s menu_setup_language_sel_tbl[MENU_SETUP_LANGUAGE_SEL_NUM] = {
    {MENU_SETUP_LANGUAGE_ENGLISH, MENU_SEL_FLAGS_ENABLE,
        STR_LANGUAGE_ENGLISH, BMP_ICN_LANGUAGE_ENGLISH, 0,
        MENU_SETUP_LANGUAGE_ENGLISH, NULL},
    {MENU_SETUP_LANGUAGE_CHINESE_SIMPLIFIED, MENU_SEL_FLAGS_ENABLE,
        STR_LANGUAGE_CHINESE_SIMPLIFIED, BMP_ICN_LANGUAGE_CHINESE_SIMPLIFIED, 0,
        MENU_SETUP_LANGUAGE_CHINESE_SIMPLIFIED, NULL}
};
static MENU_SEL_s menu_setup_record_mode_sel_tbl[MENU_SETUP_RECORD_MODE_SEL_NUM] = {
    {MENU_SETUP_RECORD_MODE_AUTO, MENU_SEL_FLAGS_ENABLE,
        STR_RECORD_MODE_AUTO, BMP_ICN_RECORD_AUTO, 0,
        MENU_SETUP_RECORD_MODE_AUTO, NULL},
    {MENU_SETUP_RECORD_MODE_MANUAL, MENU_SEL_FLAGS_ENABLE,
        STR_RECORD_MODE_MANUAL, BMP_ICN_RECORD_MANUAL, 0,
        MENU_SETUP_RECORD_MODE_MANUAL, NULL}
};

static MENU_SEL_s menu_setup_beep_sound_sel_tbl[MENU_SETUP_BEEP_SOUND_SEL_NUM] = {
    {MENU_SETUP_BEEP_SOUND_ON, MENU_SEL_FLAGS_ENABLE,
        STR_BEEP_TURN_ON, BMP_ICN_VOL_100_HL, 0,
        BEEP_SOUND_ON, NULL},
    {MENU_SETUP_BEEP_SOUND_OFF, MENU_SEL_FLAGS_ENABLE,
        STR_BEEP_TURN_OFF, BMP_ICN_VOL_0, 0,
        BEEP_SOUND_OFF, NULL},
};

static MENU_SEL_s menu_setup_backlight_auto_sel_tbl[MENU_SETUP_BACKLIGHT_AUTO_SEL_NUM] = {
    {MENU_SETUP_POWEROFF_OFF, MENU_SEL_FLAGS_ENABLE,
        STR_BACKLIGHTOFF_OFF, BMP_ICN_BACKLIGHT_OFF, 0,
        BACKLIGHTOFF_TIME_OFF, NULL},
    {MENU_SETUP_POWEROFF_1M, MENU_SEL_FLAGS_ENABLE,
        STR_BACKLIGHTOFF_1_MIN, BMP_ICN_BACKLIGHT_1MIN, 0,
        BACKLIGHTOFF_TIME_1_MIN, NULL},
    {MENU_SETUP_POWEROFF_3M, MENU_SEL_FLAGS_ENABLE,
        STR_BACKLIGHTOFF_3_MIN, BMP_ICN_BACKLIGHT_3MIN, 0,
        BACKLIGHTOFF_TIME_3_MIN, NULL},
    {MENU_SETUP_POWEROFF_5M, MENU_SEL_FLAGS_ENABLE,
        STR_BACKLIGHTOFF_5_MIN, BMP_ICN_BACKLIGHT_5MIN, 0,
        BACKLIGHTOFF_TIME_5_MIN, NULL}
};

#endif

static MENU_SEL_s *menu_setup_item_sel_tbls[MENU_SETUP_ITEM_NUM] = {
#ifdef CONFIG_APP_ARD
    menu_setup_language_sel_tbl,
#endif
    menu_setup_system_type_sel_tbl,
    menu_setup_dmf_mode_sel_tbl,
#ifdef CONFIG_APP_ARD
    NULL,   // driver id table
#endif
    NULL,   // time_init
#ifdef CONFIG_APP_ARD
    NULL,   // stamp choose
#endif
    menu_setup_date_time_disp_sel_tbl,
    NULL,    // format
    NULL,    // default
    menu_setup_lcd_ctrl_sel_tbl,
    menu_setup_poweroff_sel_tbl,
    menu_setup_powersaving_sel_tbl,
#ifdef CONFIG_APP_ARD
    menu_setup_delay_power_off_sel_tbl,
#endif
    menu_setup_hdmi_sel_tbl,
    menu_setup_flashlight_sel_tbl,
    menu_setup_usb_mode_sel_tbl,
    menu_setup_input_dimension_mode_sel_tbl,
#ifdef CONFIG_APP_ARD
    menu_setup_input_near_far_sel_tbl,
    &menu_setup_audio_volume_sel_tbl,
#endif
    menu_setup_output_dimension_mode_sel_tbl,
    menu_setup_wifi_sel_tbl,
    menu_setup_wifi_config_sel_tbl,
#ifdef CONFIG_APP_ARD
    menu_setup_record_mode_sel_tbl,
    NULL,
    menu_setup_beep_sound_sel_tbl,
    menu_setup_backlight_auto_sel_tbl,
    NULL,    // version
#endif
};

/*** Currently activated object id arrays ***/
static MENU_SEL_s *menu_setup_system_type_sels[MENU_SETUP_SYSTEM_TYPE_SEL_NUM];
static MENU_SEL_s *menu_setup_dmf_mode_sels[MENU_SETUP_DMF_MODE_SEL_NUM];
static MENU_SEL_s *menu_setup_date_time_disp_sels[MENU_SETUP_DATE_TIME_DISPLAY_SEL_NUM];
static MENU_SEL_s *menu_setup_lcd_ctrl_sels[MENU_SETUP_LCD_CONTROL_SEL_NUM];
static MENU_SEL_s *menu_setup_poweroff_sels[MENU_SETUP_POWEROFF_SEL_NUM];
static MENU_SEL_s *menu_setup_powersaving_sels[MENU_SETUP_POWERSAVING_SEL_NUM];
#ifdef CONFIG_APP_ARD
static MENU_SEL_s *menu_setup_delay_power_off_sels[MENU_SETUP_DELAY_POWER_OFF_SEL_NUM];
#endif
static MENU_SEL_s *menu_setup_hdmi_sels[MENU_SETUP_HDMI_SEL_NUM];
static MENU_SEL_s *menu_setup_flashlight_sels[MENU_SETUP_FLASHLIGHT_SEL_NUM];
static MENU_SEL_s *menu_setup_usb_mode_sels[MENU_SETUP_USB_MODE_SEL_NUM];
static MENU_SEL_s *menu_setup_input_dimension_mode_sels[MENU_SETUP_INPUT_DIMENSION_MODE_SEL_NUM];
#ifdef CONFIG_APP_ARD
static MENU_SEL_s *menu_setup_input_near_far_sels[MENU_SETUP_INPUT_NEAR_FAR_SEL_NUM];
static MENU_SEL_s *menu_setup_audio_volume_sels;
#endif
static MENU_SEL_s *menu_setup_output_dimension_mode_sels[MENU_SETUP_OUTPUT_DIMENSION_MODE_SEL_NUM];
static MENU_SEL_s *menu_setup_wifi_sels[MENU_SETUP_WIFI_SEL_NUM];
static MENU_SEL_s *menu_setup_wifi_config_sels[MENU_SETUP_WIFI_CONFIG_SEL_NUM];

#ifdef CONFIG_APP_ARD
static MENU_SEL_s *menu_setup_language_sels[MENU_SETUP_LANGUAGE_SEL_NUM];
static MENU_SEL_s *menu_setup_record_mode_sels[MENU_SETUP_RECORD_MODE_SEL_NUM];
static MENU_SEL_s *menu_setup_beep_sound_sels[MENU_SETUP_BEEP_SOUND_SEL_NUM];
static MENU_SEL_s *menu_setup_backlight_auto_sels[MENU_SETUP_BACKLIGHT_AUTO_SEL_NUM];
#endif
/*** Item ***/
static MENU_ITEM_s menu_setup_system_type = {
#ifdef CONFIG_APP_ARD
    MENU_SETUP_SYSTEM_TYPE, 0, //MENU_ITEM_FLAGS_ENABLE
#else
    MENU_SETUP_SYSTEM_TYPE, 0,
#endif
    0, 0,
    0, 0,
    0, 0, 0, menu_setup_system_type_sels,
    menu_setup_system_type_init,
    menu_setup_system_type_get_tab_str,
    menu_setup_system_type_get_sel_str,
    menu_setup_system_type_get_sel_bmp,
    menu_setup_system_type_set,
    menu_setup_system_type_sel_set
};
/*** Item ***/
#ifdef CONFIG_APP_ARD
static MENU_ITEM_s menu_setup_driver_id = {
    MENU_SETUP_DRIVER_ID_SETUP, MENU_ITEM_FLAGS_ENABLE,
    0, 0,
    0, STR_DRIVER_ID,
    0, 0, 0, NULL,
    menu_setup_driver_id_init,
    menu_setup_driver_id_get_tab_str,
    NULL,
    NULL,
    menu_setup_driver_id_set,
    NULL
};
#endif

static MENU_ITEM_s menu_setup_dmf_mode = {
    MENU_SETUP_DMF_MODE, 0, // #ifdef CONFIG_APP_ARD
    0, 0,
    0, 0,
    0, 0, 0, menu_setup_dmf_mode_sels,
    menu_setup_dmf_mode_init,
    menu_setup_dmf_mode_get_tab_str,
    menu_setup_dmf_mode_get_sel_str,
    menu_setup_dmf_mode_get_sel_bmp,
    menu_setup_dmf_mode_set,
    menu_setup_dmf_mode_sel_set
};

static MENU_ITEM_s menu_setup_time = {
    MENU_TIME_SET, MENU_ITEM_FLAGS_ENABLE,
    0, 0,
    0, 0,
    0, 0, 0, NULL,
    menu_setup_time_init,
    menu_setup_time_get_tab_str,
    NULL,
    NULL,
    menu_setup_time_set,
    NULL
};

static MENU_ITEM_s menu_setup_date_time_disp = {
    MENU_SETUP_DATE_TIME_DISPLAY, 0,
    0, 0,
    0, 0,
    0, 0, 0, menu_setup_date_time_disp_sels,
    menu_setup_date_time_disp_init,
    menu_setup_date_time_disp_get_tab_str,
    menu_setup_date_time_disp_get_sel_str,
    menu_setup_date_time_disp_get_sel_bmp,
    menu_setup_date_time_disp_set,
    menu_setup_date_time_disp_sel_set
};

static MENU_ITEM_s menu_setup_format = {
#ifdef CONFIG_ECL_GUI	
    MENU_CARD_FORMAT, MENU_ITEM_FLAGS_ENABLE,
#else	
    MENU_SETUP_FORMAT, MENU_ITEM_FLAGS_ENABLE,
#endif    
    0, 0,
    0, 0,
    0, 0, 0, NULL,
    menu_setup_format_init,
    menu_setup_format_get_tab_str,
    NULL,
    NULL,
    menu_setup_format_set,
    NULL
};

static MENU_ITEM_s menu_setup_default = {
    menu_default, MENU_ITEM_FLAGS_ENABLE,
    0, 0,
    0, 0,
    0, 0, 0, NULL,
    menu_setup_default_init,
    menu_setup_default_get_tab_str,
    NULL,
    NULL,
    menu_setup_default_set,
    NULL
};

static MENU_ITEM_s menu_setup_lcd_ctrl = {
    MENU_SETUP_LCD_CONTROL, 0,//MENU_ITEM_FLAGS_ENABLE
    0, 0,
    0, 0,
    0, 0, 0, menu_setup_lcd_ctrl_sels,
    menu_setup_lcd_ctrl_init,
    menu_setup_lcd_ctrl_get_tab_str,
    menu_setup_lcd_ctrl_get_sel_str,
    menu_setup_lcd_ctrl_get_sel_bmp,
    menu_setup_lcd_ctrl_set,
    menu_setup_lcd_ctrl_sel_set
};

static MENU_ITEM_s menu_setup_poweroff = {
    MENU_SETUP_POWEROFF, MENU_ITEM_FLAGS_ENABLE,
    0, 0,
    0, 0,
    0, 0, 0, menu_setup_poweroff_sels,
    menu_setup_poweroff_init,
    menu_setup_poweroff_get_tab_str,
    menu_setup_poweroff_get_sel_str,
    menu_setup_poweroff_get_sel_bmp,
    menu_setup_poweroff_set,
    menu_setup_poweroff_sel_set
};

static MENU_ITEM_s menu_setup_powersaving = {
    MENU_SETUP_POWERSAVING, 0,
    0, 0,
    0, 0,
    0, 0, 0, menu_setup_powersaving_sels,
    menu_setup_powersaving_init,
    menu_setup_powersaving_get_tab_str,
    menu_setup_powersaving_get_sel_str,
    menu_setup_powersaving_get_sel_bmp,
    menu_setup_powersaving_set,
    menu_setup_powersaving_sel_set
};

#ifdef CONFIG_APP_ARD
static MENU_ITEM_s menu_setup_delay_power_off = {
    MENU_SETUP_DELAY_POWER_OFF, 0, //MENU_ITEM_FLAGS_ENABLE
    0, 0,
    0, 0,
    0, 0, 0, menu_setup_delay_power_off_sels,
    menu_setup_delay_power_off_init,
    menu_setup_delay_power_off_get_tab_str,
    menu_setup_delay_power_off_get_sel_str,
    menu_setup_delay_power_off_get_sel_bmp,
    menu_setup_delay_power_off_set,
    menu_setup_delay_power_off_sel_set
};
#endif

static MENU_ITEM_s menu_setup_hdmi = {
    MENU_SETUP_HDMI, 0,
    0, 0,
    0, 0,
    0, 0, 0, menu_setup_hdmi_sels,
    menu_setup_hdmi_init,
    menu_setup_hdmi_get_tab_str,
    menu_setup_hdmi_get_sel_str,
    menu_setup_hdmi_get_sel_bmp,
    menu_setup_hdmi_set,
    menu_setup_hdmi_sel_set
};

static MENU_ITEM_s menu_setup_flashlight = {
    MENU_SETUP_FLASHLIGHT, 0,
    0, 0,
    0, 0,
    0, 0, 0, menu_setup_flashlight_sels,
    menu_setup_flashlight_init,
    menu_setup_flashlight_get_tab_str,
    menu_setup_flashlight_get_sel_str,
    menu_setup_flashlight_get_sel_bmp,
    menu_setup_flashlight_set,
    menu_setup_flashlight_sel_set
};

static MENU_ITEM_s menu_setup_usb_mode = {
#ifdef CONFIG_APP_ARD
    MENU_SETUP_USB_MODE, 0,
#else
    MENU_SETUP_USB_MODE, MENU_ITEM_FLAGS_ENABLE,
#endif
    0, 0,
    0, 0,
    0, 0, 0, menu_setup_usb_mode_sels,
    menu_setup_usb_mode_init,
    menu_setup_usb_mode_get_tab_str,
    menu_setup_usb_mode_get_sel_str,
    menu_setup_usb_mode_get_sel_bmp,
    menu_setup_usb_mode_set,
    menu_setup_usb_mode_sel_set
};

static MENU_ITEM_s menu_setup_input_dimension_mode = {
    MENU_SETUP_INPUT_DIMENSION_MODE, 0,
    0, 0,
    0, 0,
    0, 0, 0, menu_setup_input_dimension_mode_sels,
    menu_setup_input_dimension_mode_init,
    menu_setup_input_dimension_mode_get_tab_str,
    menu_setup_input_dimension_mode_get_sel_str,
    menu_setup_input_dimension_mode_get_sel_bmp,
    menu_setup_input_dimension_mode_set,
    menu_setup_input_dimension_mode_sel_set
};

#ifdef CONFIG_APP_ARD
static MENU_ITEM_s menu_setup_input_near_far = {
    MENU_SETUP_INPUT_NEAR_FAR, 0,
    0, 0,
    0, 0,
    0, 0, 0, menu_setup_input_near_far_sels,
    menu_setup_input_near_far_init,
    menu_setup_input_near_far_get_tab_str,
    menu_setup_input_near_far_get_sel_str,
    menu_setup_input_near_far_get_sel_bmp,
    menu_setup_input_near_far_set,
    menu_setup_input_near_far_sel_set
};

static MENU_ITEM_s menu_setup_audio_volume = {
    MENU_SETUP_AUDIO_VOLUME, MENU_ITEM_FLAGS_ENABLE,//MENU_ITEM_FLAGS_ENABLE
    0, 0,
    0, 0,
    0, 0, 0, &menu_setup_audio_volume_sels,
    menu_setup_audio_volume_init,
    menu_setup_audio_volume_get_tab_str,
    menu_setup_audio_volume_get_sel_str,
    menu_setup_audio_volume_get_sel_bmp,
    menu_setup_audio_volume_set,
    menu_setup_audio_volume_sel_set
};
#endif

static MENU_ITEM_s menu_setup_output_dimension_mode = {
    MENU_SETUP_OUTPUT_DIMENSION_MODE, 0,
    0, 0,
    0, 0,
    0, 0, 0, menu_setup_output_dimension_mode_sels,
    menu_setup_output_dimension_mode_init,
    menu_setup_output_dimension_mode_get_tab_str,
    menu_setup_output_dimension_mode_get_sel_str,
    menu_setup_output_dimension_mode_get_sel_bmp,
    menu_setup_output_dimension_mode_set,
    menu_setup_output_dimension_mode_sel_set
};

static MENU_ITEM_s menu_setup_wifi = {
    MENU_SETUP_WIFI, 0,
    0, 0,
    0, 0,
    0, 0, 0, menu_setup_wifi_sels,
    menu_setup_wifi_init,
    menu_setup_wifi_get_tab_str,
    menu_setup_wifi_get_sel_str,
    menu_setup_wifi_get_sel_bmp,
    menu_setup_wifi_set,
    menu_setup_wifi_sel_set
};

static MENU_ITEM_s menu_setup_wifi_config = {
    MENU_SETUP_WIFI_CONFIG, 0,
    0, 0,
    0, 0,
    0, 0, 0, menu_setup_wifi_config_sels,
    menu_setup_wifi_config_init,
    menu_setup_wifi_config_get_tab_str,
    menu_setup_wifi_config_get_sel_str,
    menu_setup_wifi_config_get_sel_bmp,
    menu_setup_wifi_config_set,
    menu_setup_wifi_config_sel_set
};

/**
 * @brief Menu_setup item: version
 */
static MENU_ITEM_s menu_setup_version = {
    MENU_SETUP_VERSION, MENU_ITEM_FLAGS_ENABLE,
    0, 0,
    0, 0,
    0, 0, 0, NULL,
    menu_setup_version_init,
    menu_setup_version_get_tab_str,
    NULL,
    NULL,
    menu_setup_version_set,
    NULL
};
#ifdef CONFIG_APP_ARD
static MENU_ITEM_s menu_setup_language = {
    MENU_LANGUAGE_SET, MENU_ITEM_FLAGS_ENABLE,
    0, 0,
    0, 0,
    0, 0, 0, menu_setup_language_sels,
    menu_setup_language_init,
    menu_setup_language_get_tab_str,
    menu_setup_language_get_sel_str,
    menu_setup_language_get_sel_bmp,
    menu_setup_language_set,
    menu_setup_language_sel_set
};
static MENU_ITEM_s menu_setup_record_mode = {
    MENU_SETUP_RECORD_MODE, 0,
    0, 0,
    0, 0,
    0, 0, 0, menu_setup_record_mode_sels,
    menu_setup_record_mode_init,
    menu_setup_record_mode_get_tab_str,
    menu_setup_record_mode_get_sel_str,
    menu_setup_record_mode_get_sel_bmp,
    menu_setup_record_mode_set,
    menu_setup_record_mode_sel_set
};
static MENU_ITEM_s menu_setup_card_volume = {
    MENU_SETUP_CARD_VOLUME, 0,//MENU_ITEM_FLAGS_ENABLE
    0, 0,
    0, 0,
    0, 0, 0, NULL,
    menu_setup_card_volume_init,
    menu_setup_card_volume_get_tab_str,
    NULL,
    NULL,
    menu_setup_card_volume_set,
    NULL
};

static MENU_ITEM_s menu_setup_beep_sound = {
    MENU_SETUP_BEEP_SOUND, MENU_ITEM_FLAGS_ENABLE,
    0, 0,
    0, 0,
    0, 0, 0, menu_setup_beep_sound_sels,
    menu_setup_beep_sound_init,
    menu_setup_beep_sound_get_tab_str,
    menu_setup_beep_sound_get_sel_str,
    menu_setup_beep_sound_get_sel_bmp,
    menu_setup_beep_sound_set,
    menu_setup_beep_sound_sel_set
};

static MENU_ITEM_s menu_setup_backlight_auto = {
#ifdef CONFIG_ECL_GUI	
    MENU_BACKLIGHT, MENU_ITEM_FLAGS_ENABLE,
#else	
    MENU_SETUP_BACKLIGHT_AUTO, MENU_ITEM_FLAGS_ENABLE,
#endif    
    0, 0,
    0, 0,
    0, 0, 0, menu_setup_backlight_auto_sels,
    menu_setup_backlight_auto_init,
    menu_setup_backlight_auto_get_tab_str,
    menu_setup_backlight_auto_get_sel_str,
    menu_setup_backlight_auto_get_sel_bmp,
    menu_setup_backlight_auto_set,
    menu_setup_backlight_auto_sel_set
};

static MENU_ITEM_s menu_setup_stamp_choose = {
    MENU_SETUP_STAMP_CHOOSE, 0,//MENU_ITEM_FLAGS_ENABLE
    0, 0,
    0, 0,
    0, 0, 0, NULL,
    menu_setup_stamp_choose_init,
    menu_setup_stamp_choose_get_tab_str,
    NULL,
    NULL,
    menu_setup_stamp_choose_set,
    NULL
};

#endif

static MENU_ITEM_s *menu_setup_item_tbl[MENU_SETUP_ITEM_NUM] = {
#ifdef CONFIG_APP_ARD
    &menu_setup_language,
#endif
    &menu_setup_system_type,
    &menu_setup_dmf_mode,
#ifdef CONFIG_APP_ARD
    &menu_setup_driver_id,
#endif
    &menu_setup_time,
#ifdef CONFIG_APP_ARD
    &menu_setup_stamp_choose,
#endif
    &menu_setup_date_time_disp,
    &menu_setup_format,
    &menu_setup_default,
    &menu_setup_lcd_ctrl,
    &menu_setup_poweroff,
    &menu_setup_powersaving,
#ifdef CONFIG_APP_ARD
    &menu_setup_delay_power_off,
#endif
    &menu_setup_hdmi,
    &menu_setup_flashlight,
    &menu_setup_usb_mode,
    &menu_setup_input_dimension_mode,
#ifdef CONFIG_APP_ARD
    &menu_setup_input_near_far,
    &menu_setup_audio_volume,
#endif
    &menu_setup_output_dimension_mode,
    &menu_setup_wifi,
    &menu_setup_wifi_config,
#ifdef CONFIG_APP_ARD
    &menu_setup_record_mode,
    &menu_setup_card_volume,
    &menu_setup_beep_sound,
    &menu_setup_backlight_auto,
    &menu_setup_version,
#endif
};

/*** Currently activated object id arrays ***/
static MENU_ITEM_s *menu_setup_items[MENU_SETUP_ITEM_NUM];
#ifdef CONFIG_ECL_GUI
MENU_ITEM_s menu_adas_font_car_alarm = {
    menu_font_car_alarm, MENU_ITEM_FLAGS_ENABLE,//MENU_ITEM_FLAGS_ENABLE
    0, 0,
    0, 0,
    0, 0, 0, NULL,
    menu_adas_font_car_alarm_init,
    menu_adas_font_car_alarm_get_tab_str,
    NULL,
    NULL,
    menu_adas_font_car_alarm_set,
    NULL
};
MENU_ITEM_s menu_adas_change_road = {
    menu_change_road, MENU_ITEM_FLAGS_ENABLE,//MENU_ITEM_FLAGS_ENABLE
    0, 0,
    0, 0,
    0, 0, 0, NULL,
    menu_adas_change_road_init,
    menu_adas_change_road_get_tab_str,
    NULL,
    NULL,
    menu_adas_change_road_set,
    NULL
};
MENU_ITEM_s menu_adas_keep_dis = {
    menu_keep_dis, MENU_ITEM_FLAGS_ENABLE,//MENU_ITEM_FLAGS_ENABLE
    0, 0,
    0, 0,
    0, 0, 0, NULL,
    menu_adas_keep_dis_init,
    menu_adas_keep_dis_get_tab_str,
    NULL,
    NULL,
    menu_adas_keep_dis_set,
    NULL
};
MENU_ITEM_s menu_adas_start_alarm = {
    menu_start_alarm, MENU_ITEM_FLAGS_ENABLE,//MENU_ITEM_FLAGS_ENABLE
    0, 0,
    0, 0,
    0, 0, 0, NULL,
    menu_adas_start_alarm_init,
    menu_adas_start_alarm_get_tab_str,
    NULL,
    NULL,
    menu_adas_start_alarm_set,
    NULL
};

MENU_ITEM_s menu_setup_chinese = {
    menu_chinese, MENU_ITEM_FLAGS_ENABLE,//MENU_ITEM_FLAGS_ENABLE
    0, 0,
    0, 0,
    0, 0, 0, NULL,
    menu_setup_chinese_init,
    menu_setup_chinese_get_tab_str,
    NULL,
    NULL,
    menu_setup_chinese_set,
    NULL
};

MENU_ITEM_s menu_setup_english = {
    menu_english, MENU_ITEM_FLAGS_ENABLE,//MENU_ITEM_FLAGS_ENABLE
    0, 0,
    0, 0,
    0, 0, 0, NULL,
    menu_setup_english_init,
    menu_setup_english_get_tab_str,
    NULL,
    NULL,
    menu_setup_english_set,
    NULL
};

MENU_ITEM_s menu_adas_alarm_near = {
    menu_alarm_near, MENU_ITEM_FLAGS_ENABLE,//MENU_ITEM_FLAGS_ENABLE
    0, 0,
    0, 0,
    0, 0, 0, NULL,
    menu_adas_alarm_near_init,
    menu_adas_alarm_near_get_tab_str,
    NULL,
    NULL,
    menu_adas_alarm_near_set,
    NULL
};

MENU_ITEM_s menu_adas_alarm_middle = {
    menu_alarm_middle, MENU_ITEM_FLAGS_ENABLE,//MENU_ITEM_FLAGS_ENABLE
    0, 0,
    0, 0,
    0, 0, 0, NULL,
    menu_adas_alarm_middle_init,
    menu_adas_alarm_middle_get_tab_str,
    NULL,
    NULL,
    menu_adas_alarm_middle_set,
    NULL
};

MENU_ITEM_s menu_adas_alarm_far = {
    menu_alarm_far, MENU_ITEM_FLAGS_ENABLE,//MENU_ITEM_FLAGS_ENABLE
    0, 0,
    0, 0,
    0, 0, 0, NULL,
    menu_adas_alarm_far_init,
    menu_adas_alarm_far_get_tab_str,
    NULL,
    NULL,
    menu_adas_alarm_far_set,
    NULL
};

MENU_ITEM_s menu_adas_start_cali = {
    menu_adas_cali_item_start, MENU_ITEM_FLAGS_ENABLE,//MENU_ITEM_FLAGS_ENABLE
    0, 0,
    0, 0,
    0, 0, 0, NULL,
    menu_adas_start_cali_init,
    menu_adas_start_cali_get_tab_str,
    NULL,
    NULL,
    menu_adas_start_cali_set,
    NULL
};
MENU_ITEM_s menu_adas_auto_cali = {
    menu_adas_cali_item_start, MENU_ITEM_FLAGS_ENABLE,//MENU_ITEM_FLAGS_ENABLE
    0, 0,
    0, 0,
    0, 0, 0, NULL,
    menu_adas_auto_cali_init,
    menu_adas_auto_cali_get_tab_str,
    NULL,
    NULL,
    menu_adas_auto_cali_set,
    NULL
};
static MENU_ITEM_s *menu_adas_cali_item_tbl[MENU_THIRD_SEL] = {    
    &menu_adas_start_cali,
    &menu_adas_auto_cali,	
};

static MENU_ITEM_s *menu_adas_alarm_item_tbl[MENU_FOUR_SEL] = {
    &menu_adas_alarm_near,
    &menu_adas_alarm_middle,	 	
    &menu_adas_alarm_far,
};

static MENU_ITEM_s *menu_adas_func_item_tbl[MENU_FIVE_SEL] = {
    &menu_adas_font_car_alarm,
    &menu_adas_change_road,	 	
    &menu_adas_keep_dis,
    &menu_adas_start_alarm,
};
static MENU_ITEM_s *menu_recorder_set_item_tbl[MENU_TEN_SEL] = {
    &menu_setup_format,	
    &menu_setup_time,		
    &menu_setup_language,    
    &menu_video_motion_detect,
    &menu_video_gsensor_sensitivity,
    &menu_video_split_rec,
    &menu_video_flicker,
    &menu_setup_backlight_auto, 
    &menu_setup_audio_volume,
   //&menu_video_parkingmode_sensitivity,
};
static MENU_ITEM_s *menu_language_item_tbl[MENU_THIRD_SEL] = {
    &menu_setup_english,			
    &menu_setup_chinese,	
};	

static MENU_ITEM_s *menu_default_item_tbl[MENU_SECOND_SEL] = {
    &menu_setup_default,	
};	
static MENU_ITEM_s *menu_version_item_tbl[MENU_SECOND_SEL] = {
    &menu_setup_version,	
};
static MENU_ITEM_s *menu_adas_cali_items[MENU_SECOND_SEL];
static MENU_ITEM_s *menu_adas_func_items[MENU_FIVE_SEL];
static MENU_ITEM_s *menu_adas_alarm_dis_items[MENU_FOUR_SEL];
static MENU_ITEM_s *menu_recorder_set_items[MENU_SEL_NUM];
static MENU_ITEM_s *menu_language_items[MENU_THIRD_SEL];
static MENU_ITEM_s *menu_default_items[MENU_SECOND_SEL];
static MENU_ITEM_s *menu_version_items[MENU_SECOND_SEL];
/*** Tab ***/
static MENU_TAB_s menu_adas_cali = {
    MENU_ADAS_CALI, 1,
    0, 0,
    BMP_MENU_TABLE_CALIBRATION, BMP_MENU_TABLE_CALIBRATION_HL,
    menu_adas_cali_items,
    menu_adas_cali_init,
    menu_adas_cali_start,
    menu_adas_cali_stop
};
static MENU_TAB_s menu_adas_func = {
    MENU_ADAS_FUNC, 1,
    0, 0,
        BMP_MENU_TABLE_FUNCTIONS, BMP_MENU_TABLE_FUNCTIONS_HL,
    menu_adas_func_items,
    menu_adas_func_init,
    menu_adas_func_start,
    menu_adas_func_stop
};
static MENU_TAB_s menu_adas_alarm_dis = {
    MENU_ADAS_ALARM_DIS, 1,
    0, 0,
    BMP_MENU_TABLE_WARING, BMP_MENU_TABLE_WARING_HL,
    menu_adas_alarm_dis_items,
    menu_adas_alarm_dis_init,
    menu_adas_alarm_dis_start,
    menu_adas_alarm_dis_stop
};
static MENU_TAB_s menu_recorder_set = {
    MENU_RECORDER_SETTTING, 1,
    0, 0,
    BMP_MENU_TABLE_SETTING, BMP_MENU_TABLE_SETTING_HL,
    menu_recorder_set_items,
    menu_recorder_set_init,
    menu_recorder_set_start,
    menu_recorder_set_stop
};
static MENU_TAB_s menu_version = {
    MENU_VERSION, 1,
    0, 0,
    BMP_MENU_TABLE_LANGUAGE, BMP_MENU_TABLE_LANGUAGE_HL,
    menu_version_items,
    menu_version_init,
    menu_version_start,
    menu_version_stop
};
static MENU_TAB_s menu_sys_default = {
    MENU_DEFAULT, 1,
    0, 0,
    BMP_MENU_TABLE_DEFAULT, BMP_MENU_TABLE_DEFAULT_HL,
    menu_default_items,
    menu_default_init,
    menu_default_start,
    menu_default_stop
};

MENU_TAB_CTRL_s menu_adas_cali_ctrl = {
    menu_adas_cali_tab,
    menu_adas_cali_item,
};

MENU_TAB_CTRL_s menu_adas_func_ctrl = {
    menu_adas_func_tab,
    menu_adas_func_item,
};

MENU_TAB_CTRL_s menu_adas_alarm_dis_ctrl = {
    menu_adas_alarm_dis_tab,
    menu_adas_alarm_dis_item,
};

MENU_TAB_CTRL_s menu_recorder_set_ctrl = {
    menu_recorder_set_tab,
    menu_recorder_set_item,
    menu_recorder_set_get_sel,
};

MENU_TAB_CTRL_s menu_version_ctrl = {
    menu_version_tab,
    menu_version_item,
};

MENU_TAB_CTRL_s menu_default_ctrl = {
    menu_default_tab,
    menu_default_item,
};
#endif

/*** Tab ***/
static MENU_TAB_s menu_setup = {
    MENU_SETUP, 1,
    0, 0,
    BMP_MENU_TAB_SETUP, BMP_MENU_TAB_SETUP_HL,
    menu_setup_items,
    menu_setup_init,
    menu_setup_start,
    menu_setup_stop
};

MENU_TAB_CTRL_s menu_setup_ctrl = {
    menu_setup_get_tab,
    menu_setup_get_item,
    menu_setup_get_sel,
    menu_setup_set_sel_table,
    menu_setup_lock_tab,
    menu_setup_unlock_tab,
    menu_setup_enable_item,
    menu_setup_disable_item,
    menu_setup_lock_item,
    menu_setup_unlock_item,
    menu_setup_enable_sel,
    menu_setup_disable_sel,
    menu_setup_lock_sel,
    menu_setup_unlock_sel
};

//static struct tm tm_bak;
#ifdef CONFIG_ECL_GUI
static int menu_adas_cali_init(void)
{
    int i = 0;
    UINT32 cur_item_id = 0;

    APP_ADDFLAGS(menu_adas_cali.Flags, MENU_TAB_FLAGS_INIT);
    if (menu_adas_cali.ItemNum > 0) {
        cur_item_id = menu_adas_cali_items[menu_adas_cali.ItemCur]->Id;
    }
    menu_adas_cali.ItemNum = 0;
    menu_adas_cali.ItemCur = 0;
    for (i=0; i<MENU_THIRD_SEL; i++) {
        if (APP_CHECKFLAGS(menu_adas_cali_item_tbl[i]->Flags, MENU_ITEM_FLAGS_ENABLE)) {
            menu_adas_cali_items[menu_adas_cali.ItemNum] = menu_adas_cali_item_tbl[i];
            menu_adas_cali.ItemNum++;
        }
    }
    return 0;
}

static int menu_adas_cali_start(void)
{
   return 0;
}

static int menu_adas_cali_stop(void)
{
   return 0;
}

static int menu_adas_func_init(void)
{
    int i = 0;
    UINT32 cur_item_id = 0;

    APP_ADDFLAGS(menu_adas_func.Flags, MENU_TAB_FLAGS_INIT);
    if (menu_adas_func.ItemNum > 0) {
        cur_item_id = menu_adas_func_items[menu_adas_func.ItemCur]->Id;
    }
    menu_adas_func.ItemNum = 0;
    menu_adas_func.ItemCur = 0;
    for (i=0; i<MENU_FIVE_SEL; i++) {
        if (APP_CHECKFLAGS(menu_adas_func_item_tbl[i]->Flags, MENU_ITEM_FLAGS_ENABLE)) {
            menu_adas_func_items[menu_adas_func.ItemNum] = menu_adas_func_item_tbl[i];
            if (cur_item_id == menu_adas_func_item_tbl[i]->Id) {
                menu_adas_func.ItemCur = menu_adas_func.ItemNum;
            }
            menu_adas_func.ItemNum++;
        }
    }
    return 0;
}

static int menu_adas_func_start(void)
{
   return 0;
}

static int menu_adas_func_stop(void)
{
   return 0;
}

static int menu_adas_alarm_dis_init(void)
{
    int i = 0;
    UINT32 cur_item_id = 0;

    APP_ADDFLAGS(menu_adas_alarm_dis.Flags, MENU_TAB_FLAGS_INIT);
    //if (menu_adas_alarm_dis.ItemNum > 0) {
        //cur_item_id = menu_adas_alarm_dis_items[menu_adas_alarm_dis.ItemCur]->Id;
    //}
    menu_adas_alarm_dis.ItemNum = 0;
    menu_adas_alarm_dis.ItemCur = 0;
    for (i=0; i<MENU_FOUR_SEL; i++) {
        if (APP_CHECKFLAGS(menu_adas_alarm_item_tbl[i]->Flags, MENU_ITEM_FLAGS_ENABLE)) {
            menu_adas_alarm_dis_items[menu_adas_alarm_dis.ItemNum] = menu_adas_alarm_item_tbl[i];
            if (UserSetting->SetupPref.adas_alarm_dis == menu_adas_alarm_item_tbl[i]->Id) {
                menu_adas_alarm_dis.ItemCur = menu_adas_alarm_dis.ItemNum;
            }
            menu_adas_alarm_dis.ItemNum++;
        }
    }
    return 0;
}

static int menu_adas_alarm_dis_start(void)
{
   return 0;
}

static int menu_adas_alarm_dis_stop(void)
{
   return 0;
}

static int menu_recorder_set_init(void)
{
    int i = 0;
    UINT32 cur_item_id = 0;

    APP_ADDFLAGS(menu_recorder_set.Flags, MENU_TAB_FLAGS_INIT);
    if (menu_recorder_set.ItemNum > 0) {
        cur_item_id = menu_recorder_set_items[menu_recorder_set.ItemCur]->Id;
    }
    menu_recorder_set.ItemNum = 0;
    menu_recorder_set.ItemCur = 0;
    for (i=0; i<MENU_TEN_SEL; i++) {
        if (APP_CHECKFLAGS(menu_recorder_set_item_tbl[i]->Flags, MENU_ITEM_FLAGS_ENABLE)) {
            menu_recorder_set_items[menu_recorder_set.ItemNum] = menu_recorder_set_item_tbl[i];
            if (cur_item_id == menu_recorder_set_item_tbl[i]->Id) {
                menu_recorder_set.ItemCur = menu_recorder_set.ItemNum;
            }
            menu_recorder_set.ItemNum++;
        }
    }
    return 0;
}

static int menu_recorder_set_start(void)
{
   return 0;
}

static int menu_recorder_set_stop(void)
{
   return 0;
}

static int menu_version_init(void)
{
    int i = 0;
    UINT32 cur_item_id = 0;

    APP_ADDFLAGS(menu_version.Flags, MENU_TAB_FLAGS_INIT);
    if (menu_version.ItemNum > 0) {
        cur_item_id = menu_version_items[menu_version.ItemCur]->Id;
    }
    menu_version.ItemNum = 0;
    menu_version.ItemCur = 0;
    for (i=0; i<MENU_SECOND_SEL; i++) {
        if (APP_CHECKFLAGS(menu_version_item_tbl[i]->Flags, MENU_ITEM_FLAGS_ENABLE)) {
            menu_version_items[menu_version.ItemNum] = menu_version_item_tbl[i];
            if (cur_item_id == menu_version_item_tbl[i]->Id) {
                menu_version.ItemCur = menu_version.ItemNum;
            }
            menu_version.ItemNum++;
        }
    }
    return 0;
}

static int menu_version_start(void)
{
   return 0;
}

static int menu_version_stop(void)
{
   return 0;
}

static int menu_default_init(void)
{

    int i = 0;
    UINT32 cur_item_id = 0;

    APP_ADDFLAGS(menu_sys_default.Flags, MENU_TAB_FLAGS_INIT);
    if (menu_sys_default.ItemNum > 0) {
        cur_item_id = menu_default_items[menu_sys_default.ItemCur]->Id;
    }
    menu_sys_default.ItemNum = 0;
    menu_sys_default.ItemCur = 0;
    for (i=0; i<MENU_SECOND_SEL; i++) {
        if (APP_CHECKFLAGS(menu_default_item_tbl[i]->Flags, MENU_ITEM_FLAGS_ENABLE)) {
            menu_default_items[menu_sys_default.ItemNum] = menu_default_item_tbl[i];
            if (cur_item_id == menu_default_item_tbl[i]->Id) {
                menu_sys_default.ItemCur = menu_sys_default.ItemNum;
            }
            menu_sys_default.ItemNum++;
        }
    }
    return 0;
}

static int menu_default_start(void)
{
   return 0;
}

static int menu_default_stop(void)
{
   return 0;
}

#endif

/*** APIs ***/
// tab
static int menu_setup_init(void)
{
    int i = 0;
    UINT32 cur_item_id = 0;

#if defined (FLASHLIGHT)
    menu_setup_enable_item(MENU_SETUP_FLASHLIGHT);
#endif

    menu_setup_disable_item(MENU_SETUP_POWEROFF);
    menu_setup_disable_item(MENU_SETUP_LCD_CONTROL);

#if defined(APP_HDMI_TEST)
    menu_setup_enable_item(MENU_SETUP_SYSTEM_TYPE);
#endif

#ifdef CONFIG_APP_ARD
    menu_setup_enable_item(MENU_SETUP_SYSTEM_TYPE);
    menu_setup_enable_item(MENU_SETUP_POWEROFF);
    menu_setup_enable_item(MENU_SETUP_VERSION);
    menu_setup_disable_item(MENU_SETUP_AUDIO_VOLUME);
#endif

    APP_ADDFLAGS(menu_setup.Flags, MENU_TAB_FLAGS_INIT);
    if (menu_setup.ItemNum > 0) {
        cur_item_id = menu_setup_items[menu_setup.ItemCur]->Id;
    }
    menu_setup.ItemNum = 0;
    menu_setup.ItemCur = 0;
    for (i=0; i<MENU_SETUP_ITEM_NUM; i++) {
        if (APP_CHECKFLAGS(menu_setup_item_tbl[i]->Flags, MENU_ITEM_FLAGS_ENABLE)) {
            menu_setup_items[menu_setup.ItemNum] = menu_setup_item_tbl[i];
            if (cur_item_id == menu_setup_item_tbl[i]->Id) {
                menu_setup.ItemCur = menu_setup.ItemNum;
            }
            menu_setup.ItemNum++;
        }
    }

    return 0;
}

static int menu_setup_start(void)
{
    return 0;
}

static int menu_setup_stop(void)
{
    return 0;
}
// item
#ifdef CONFIG_APP_ARD
#define DRIVER_ID_NUM_LENGTH 9
static int menu_setup_driver_id_init(void)
{
    int i = 0;
    char* str_temp = NULL;

    str_temp = app_widget_menu_driver_id_get_num_string();
    for(i =0;i< DRIVER_ID_NUM_LENGTH; i++){
        str_temp[i] = UserSetting->SetupPref.driver_id[i];
    }

    return 0;
}

static int menu_setup_driver_id_get_tab_str(void)
{
    return STR_DRIVER_ID_SETTEING;
}

static int menu_setup_driver_id_set(void)
{
    app_widget_menu_driver_id_set_item(MENU_SETUP, MENU_SETUP_DRIVER_ID_SETUP);
    AppWidget_On(WIDGET_MENU_DRIVER_ID, 0);
    return 0;
}
#endif

// item
static int menu_setup_system_type_init(void)
{
    int i = 0;

    APP_ADDFLAGS(menu_setup_system_type.Flags, MENU_ITEM_FLAGS_INIT);
    menu_setup_system_type.SelSaved = 0;
    menu_setup_system_type.SelNum = 0;
    menu_setup_system_type.SelCur = 0;
    for (i=0; i<MENU_SETUP_SYSTEM_TYPE_SEL_NUM; i++) {
        if (APP_CHECKFLAGS((menu_setup_item_sel_tbls[MENU_SETUP_SYSTEM_TYPE]+i)->Flags, MENU_SEL_FLAGS_ENABLE)) {
            menu_setup_system_type_sels[menu_setup_system_type.SelNum] = menu_setup_item_sel_tbls[MENU_SETUP_SYSTEM_TYPE]+i;
            if (menu_setup_system_type_sels[menu_setup_system_type.SelNum]->Val == UserSetting->SetupPref.VoutSystem) {
                menu_setup_system_type.SelSaved = menu_setup_system_type.SelNum;
                menu_setup_system_type.SelCur = menu_setup_system_type.SelNum;
            }
            menu_setup_system_type.SelNum++;
        }
    }

    return 0;
}

static int menu_setup_system_type_get_tab_str(void)
{
    return menu_setup_system_type_sels[menu_setup_system_type.SelSaved]->Str;
}

static int menu_setup_system_type_get_sel_str(int ref)
{
    return menu_setup_system_type_sels[ref]->Str;
}

static int menu_setup_system_type_get_sel_bmp(int ref)
{
    return menu_setup_system_type_sels[ref]->Bmp;
}

static int menu_setup_system_type_set(void)
{
    AppMenuQuick_SetItem(MENU_SETUP, MENU_SETUP_SYSTEM_TYPE);
    AppWidget_On(WIDGET_MENU_QUICK, 0);
    return 0;
}

static int menu_setup_system_type_sel_set(void)
{
    if (menu_setup_system_type.SelSaved != menu_setup_system_type.SelCur) {
        menu_setup_system_type.SelSaved = menu_setup_system_type.SelCur;
        UserSetting->SetupPref.VinSystem = menu_setup_system_type_sels[menu_setup_system_type.SelCur]->Val;
        AppLibSysVin_SetSystemType(menu_setup_system_type_sels[menu_setup_system_type.SelCur]->Val);
        UserSetting->SetupPref.VoutSystem = menu_setup_system_type_sels[menu_setup_system_type.SelCur]->Val;
        AppLibSysVout_SetSystemType(menu_setup_system_type_sels[menu_setup_system_type.SelCur]->Val);
        {
        /* Send the message to the current app. */
            APP_APP_s *curapp;
            AppAppMgt_GetCurApp(&curapp);
            curapp->OnMessage(AMSG_CMD_SET_SYSTEM_TYPE,
            menu_setup_system_type_sels[menu_setup_system_type.SelCur]->Val,
            menu_setup_system_type_sels[menu_setup_system_type.SelCur]->Val);
        }
    }

    return 0;
}


static int menu_setup_dmf_mode_init(void)
{
    int i = 0;

    APP_ADDFLAGS(menu_setup_dmf_mode.Flags, MENU_ITEM_FLAGS_INIT);
    menu_setup_dmf_mode.SelSaved = 0;
    menu_setup_dmf_mode.SelNum = 0;
    menu_setup_dmf_mode.SelCur = 0;
    for (i=0; i<MENU_SETUP_DMF_MODE_SEL_NUM; i++) {
        if (APP_CHECKFLAGS((menu_setup_item_sel_tbls[MENU_SETUP_DMF_MODE]+i)->Flags, MENU_SEL_FLAGS_ENABLE)) {
            menu_setup_dmf_mode_sels[menu_setup_dmf_mode.SelNum] = menu_setup_item_sel_tbls[MENU_SETUP_DMF_MODE]+i;
            if (menu_setup_dmf_mode_sels[menu_setup_dmf_mode.SelNum]->Val == UserSetting->SetupPref.DMFMode) {
                menu_setup_dmf_mode.SelSaved = menu_setup_dmf_mode.SelNum;
                menu_setup_dmf_mode.SelCur = menu_setup_dmf_mode.SelNum;
            }
            menu_setup_dmf_mode.SelNum++;
        }
    }

    return 0;
}

static int menu_setup_dmf_mode_get_tab_str(void)
{
    return menu_setup_dmf_mode_sels[menu_setup_dmf_mode.SelSaved]->Str;
}

static int menu_setup_dmf_mode_get_sel_str(int ref)
{
    return menu_setup_dmf_mode_sels[ref]->Str;
}

static int menu_setup_dmf_mode_get_sel_bmp(int ref)
{
    return menu_setup_dmf_mode_sels[ref]->Bmp;
}

static int menu_setup_dmf_mode_set(void)
{
    AppMenuQuick_SetItem(MENU_SETUP, MENU_SETUP_DMF_MODE);
    AppWidget_On(WIDGET_MENU_QUICK, 0);
    return 0;
}

static int menu_setup_dmf_mode_sel_set(void)
{
    if (menu_setup_dmf_mode.SelSaved != menu_setup_dmf_mode.SelCur) {
        menu_setup_dmf_mode.SelSaved = menu_setup_dmf_mode.SelCur;
        UserSetting->SetupPref.DMFMode = menu_setup_dmf_mode.Sels[menu_setup_dmf_mode.SelCur]->Val;
        if (UserSetting->SetupPref.DMFMode == DMF_MODE_RESET) {
            UserSetting->SetupPref.DmfMixLastIdx = 0;
        }
        {
            /* Send the message to the current app. */
            APP_APP_s *curapp;
            AppAppMgt_GetCurApp(&curapp);
            curapp->OnMessage(AMSG_CMD_SET_DMF_MODE, menu_setup_dmf_mode.Sels[menu_setup_dmf_mode.SelCur]->Val, 0);
        }
    }
    return 0;
}

static int menu_setup_time_init(void)
{
    menu_setup_time.SelNum = 5;
    return 0;
}

static int menu_setup_time_get_tab_str(void)
{
    char str_time[17] = {0};
    UINT16 str_time_uni[17] = {0};

    /** Correct RTC time */
    {
        AMBA_RTC_TIME_SPEC_u TimeSpec = {0};
        AmbaRTC_GetSystemTime(AMBA_TIME_STD_TAI, &TimeSpec);
#if 0
        AmbaPrint("TimeSpec.Calendar.Year = %d",TimeSpec.Calendar.Year);
        AmbaPrint("TimeSpec.Calendar.Month = %d",TimeSpec.Calendar.Month);
        AmbaPrint("TimeSpec.Calendar.DayOfMonth = %d",TimeSpec.Calendar.DayOfMonth);
        AmbaPrint("TimeSpec.Calendar.DayOfWeek = %d",TimeSpec.Calendar.DayOfWeek);
        AmbaPrint("TimeSpec.Calendar.Hour = %d",TimeSpec.Calendar.Hour);
        AmbaPrint("TimeSpec.Calendar.Minute = %d",TimeSpec.Calendar.Minute);
        AmbaPrint("TimeSpec.Calendar.Second = %d",TimeSpec.Calendar.Second);
#endif
        sprintf(str_time, "%04d/%02d/%02d %02d:%02d", TimeSpec.Calendar.Year, TimeSpec.Calendar.Month, TimeSpec.Calendar.Day, TimeSpec.Calendar.Hour, TimeSpec.Calendar.Minute);
    }

    AppUtil_AsciiToUnicode(str_time, str_time_uni);
#ifdef CONFIG_APP_ARD
    AppLibGraph_UpdateStringContext(AppPref_GetLanguageID(), STR_TIME_VALUE, str_time_uni);
#else
    AppLibGraph_UpdateStringContext(0, STR_TIME_VALUE, str_time_uni);
#endif

#ifdef CONFIG_APP_ARD
    return STR_TIME_SETUP;
#else
    return STR_TIME_VALUE;
#endif
}

static int menu_setup_time_set(void)
{
    AppMenuTime_SetItem(MENU_RECORDER_SETTTING, MENU_TIME_SET);
    AppWidget_On(WIDGET_MENU_TIME, 0);
    return 0;
}

static int menu_setup_date_time_disp_init(void)
{
    int i = 0;

    APP_ADDFLAGS(menu_setup_date_time_disp.Flags, MENU_ITEM_FLAGS_INIT);
    menu_setup_date_time_disp.SelSaved = 0;
    menu_setup_date_time_disp.SelNum = 0;
    menu_setup_date_time_disp.SelCur = 0;
    for (i=0; i<MENU_SETUP_DATE_TIME_DISPLAY_SEL_NUM; i++) {
        if (APP_CHECKFLAGS((menu_setup_item_sel_tbls[MENU_SETUP_DATE_TIME_DISPLAY]+i)->Flags, MENU_SEL_FLAGS_ENABLE)) {
            menu_setup_date_time_disp_sels[menu_setup_date_time_disp.SelNum] = menu_setup_item_sel_tbls[MENU_SETUP_DATE_TIME_DISPLAY]+i;
            if (menu_setup_date_time_disp_sels[menu_setup_date_time_disp.SelNum]->Val == 0/*UserSetting->date_time_display*/) {
                menu_setup_date_time_disp.SelSaved = menu_setup_date_time_disp.SelNum;
                menu_setup_date_time_disp.SelCur = menu_setup_date_time_disp.SelNum;
            }
            menu_setup_date_time_disp.SelNum++;
        }
    }

    return 0;
}

static int menu_setup_date_time_disp_get_tab_str(void)
{
    return menu_setup_date_time_disp_sels[menu_setup_date_time_disp.SelSaved]->Str;
}

static int menu_setup_date_time_disp_get_sel_str(int ref)
{
    return menu_setup_date_time_disp_sels[ref]->Str;
}

static int menu_setup_date_time_disp_get_sel_bmp(int ref)
{
    return menu_setup_date_time_disp_sels[ref]->Bmp;
}

static int menu_setup_date_time_disp_set(void)
{
    AppMenuQuick_SetItem(MENU_SETUP, MENU_SETUP_DATE_TIME_DISPLAY);
    return 0;
}

static int menu_setup_date_time_disp_sel_set(void)
{
    return 0;
}

static int menu_setup_format_init(void)
{
    return 0;
}

static int menu_setup_format_get_tab_str(void)
{
    return STR_FORMAT;
}

static int menu_setup_format_set(void)
{
    int rval = 0;

    rval = AppLibCard_CheckStatus(CARD_CHECK_DELETE);
#ifdef CONFIG_APP_ARD
    if (0 != AppLibDCF_GetOpenedFiles()) {
        AmbaPrintColor(RED,"There is file opened");
    }
#endif

    if (rval == CARD_STATUS_NO_CARD) {
        AmbaPrintColor(RED,"WARNING_NO_CARD");
    } else {
        rval = AppUtil_SwitchApp(APP_MISC_FORMATCARD);
    }

    return rval;
}

static int menu_setup_default_init(void)
{
    return 0;
}

static int menu_setup_default_get_tab_str(void)
{
    return STR_DEFAULT_SETTING;
}

static int menu_setup_default_set(void)
{
    int rval = 0;

    rval = AppUtil_SwitchApp(APP_MISC_DEFSETTING);

    return rval;
}

static int menu_setup_lcd_ctrl_init(void)
{
    int i = 0;

    APP_ADDFLAGS(menu_setup_lcd_ctrl.Flags, MENU_ITEM_FLAGS_INIT);
    menu_setup_lcd_ctrl_sel_tbl[MENU_SETUP_LCD_CONTROL_BRIGHTNESS].Val = UserSetting->SetupPref.LcdBrightness;
    menu_setup_lcd_ctrl_sel_tbl[MENU_SETUP_LCD_CONTROL_CONTRAST].Val = UserSetting->SetupPref.LcdContrast;
    menu_setup_lcd_ctrl_sel_tbl[MENU_SETUP_LCD_CONTROL_COLOR_BALANCE].Val = UserSetting->SetupPref.LcdColorBalance;
    for (i=0; i<MENU_SETUP_LCD_CONTROL_SEL_NUM; i++) {
        menu_setup_lcd_ctrl_sels[i] = menu_setup_item_sel_tbls[MENU_SETUP_LCD_CONTROL]+i;
    }
    menu_setup_lcd_ctrl.SelNum = MENU_SETUP_LCD_CONTROL_SEL_NUM;
    return 0;
}

static int menu_setup_lcd_ctrl_get_tab_str(void)
{
    return STR_LCD_CONTROL;
}

static int menu_setup_lcd_ctrl_get_sel_str(int ref)
{
    return menu_setup_lcd_ctrl_sels[ref]->Str;
}

static int menu_setup_lcd_ctrl_get_sel_bmp(int ref)
{
    return menu_setup_lcd_ctrl_sels[ref]->Bmp;
}

static int menu_setup_lcd_ctrl_set(void)
{
    AppMenuQuick_SetItem(MENU_SETUP, MENU_SETUP_LCD_CONTROL);
    AppWidget_On(WIDGET_MENU_QUICK, 0);
    return 0;
}

static int menu_setup_lcd_ctrl_sel_set(void)
{
    menu_setup_lcd_ctrl.SelSaved = menu_setup_lcd_ctrl.SelCur;
    AppMenuAdj_SetSel(MENU_SETUP, MENU_SETUP_LCD_CONTROL, menu_setup_lcd_ctrl_sels[menu_setup_lcd_ctrl.SelCur]->Id);
    AppWidget_On(WIDGET_MENU_ADJ, 0);
    return 0;
}

static int menu_setup_lcd_brightness_get_cur_val(void)
{
    return UserSetting->SetupPref.LcdBrightness;
}

static int menu_setup_lcd_brightness_get_val_str(int val)
{
    char Str[4] = {0};
    UINT16 str_madj_value[4] = {0};

    sprintf(Str, "%3d", val);
    //gui_ascii_to_unicode(Str, str_madj_value);
#ifdef CONFIG_APP_ARD
    AppLibGraph_UpdateStringContext(AppPref_GetLanguageID(), STR_ADJUST_VALUE, str_madj_value);
#else
    AppLibGraph_UpdateStringContext(0, STR_ADJUST_VALUE, str_madj_value);
#endif

    return STR_ADJUST_VALUE;
}

static int menu_setup_lcd_brightness_val_set(int val)
{
    UserSetting->SetupPref.LcdBrightness = val;
    return AppLibSysLcd_SetBrightness(LCD_CH_DCHAN, val, 0);
}

static int menu_setup_lcd_contrast_get_cur_val(void)
{
    return UserSetting->SetupPref.LcdContrast;
}

static int menu_setup_lcd_contrast_get_val_str(int val)
{
    char Str[4] = {0};
    UINT16 str_madj_value[4] = {0};

    sprintf(Str, "%3d", val);
    //gui_ascii_to_unicode(Str, str_madj_value);
#ifdef CONFIG_APP_ARD
    AppLibGraph_UpdateStringContext(AppPref_GetLanguageID(), STR_ADJUST_VALUE, str_madj_value);
#else
    AppLibGraph_UpdateStringContext(0, STR_ADJUST_VALUE, str_madj_value);
#endif

    return STR_ADJUST_VALUE;
}

static int menu_setup_lcd_contrast_val_set(int val)
{
    UserSetting->SetupPref.LcdContrast = val;
    return AppLibSysLcd_SetContrast(LCD_CH_DCHAN, val, 0);
}

static int menu_setup_lcd_colorbalance_get_cur_val(void)
{
    return UserSetting->SetupPref.LcdColorBalance;
}

static int menu_setup_lcd_colorbalance_get_val_str(int val)
{
    char Str[4] = {0};
    UINT16 str_madj_value[4] = {0};

    sprintf(Str, "%3d", val);
    //gui_ascii_to_unicode(Str, str_madj_value);
#ifdef CONFIG_APP_ARD
    AppLibGraph_UpdateStringContext(AppPref_GetLanguageID(), STR_ADJUST_VALUE, str_madj_value);
#else
    AppLibGraph_UpdateStringContext(0, STR_ADJUST_VALUE, str_madj_value);
#endif
    return STR_ADJUST_VALUE;
}

static int menu_setup_lcd_colorbalance_val_set(int Val)
{
    UserSetting->SetupPref.LcdColorBalance = Val;
    return 0;//AppLibSysLcd_SetColorBalance(LCD_CH_DCHAN, Val);
}

static int menu_setup_poweroff_init(void)
{
    int i = 0;

    APP_ADDFLAGS(menu_setup_poweroff.Flags, MENU_ITEM_FLAGS_INIT);
    menu_setup_poweroff.SelSaved = 0;
    menu_setup_poweroff.SelNum = 0;
    menu_setup_poweroff.SelCur = 0;
    for (i=0; i<MENU_SETUP_POWEROFF_SEL_NUM; i++) {
        if (APP_CHECKFLAGS((menu_setup_item_sel_tbls[MENU_SETUP_POWEROFF]+i)->Flags, MENU_SEL_FLAGS_ENABLE)) {
            menu_setup_poweroff_sels[menu_setup_poweroff.SelNum] = menu_setup_item_sel_tbls[MENU_SETUP_POWEROFF]+i;
            if (menu_setup_poweroff_sels[menu_setup_poweroff.SelNum]->Val == UserSetting->SetupPref.AutoPoweroff) {
                menu_setup_poweroff.SelSaved = menu_setup_poweroff.SelNum;
                menu_setup_poweroff.SelCur = menu_setup_poweroff.SelNum;
            }
            menu_setup_poweroff.SelNum++;
        }
    }

    return 0;
}

static int menu_setup_poweroff_get_tab_str(void)
{
    return menu_setup_poweroff_sels[menu_setup_poweroff.SelSaved]->Str;
}

static int menu_setup_poweroff_get_sel_str(int ref)
{
    return menu_setup_poweroff_sels[ref]->Str;
}

static int menu_setup_poweroff_get_sel_bmp(int ref)
{
    return menu_setup_poweroff_sels[ref]->Bmp;
}

static int menu_setup_poweroff_set(void)
{
    AppMenuQuick_SetItem(MENU_SETUP, MENU_SETUP_POWEROFF);
    AppWidget_On(WIDGET_MENU_QUICK, 0);
    return 0;
}

static int menu_setup_poweroff_sel_set(void)
{
#ifdef CONFIG_APP_ARD
    if (menu_setup_poweroff.SelSaved != menu_setup_poweroff.SelCur) {
        menu_setup_poweroff.SelSaved = menu_setup_poweroff.SelCur;
        UserSetting->SetupPref.AutoPoweroff = (APP_PREF_AUTO_POWEROFF_e)menu_setup_poweroff_sels[menu_setup_poweroff.SelCur]->Val;
        AppUtil_AutoPowerOffInit(0);
    }
#endif
    return 0;
}

static int menu_setup_powersaving_init(void)
{
    int i = 0;

    APP_ADDFLAGS(menu_setup_powersaving.Flags, MENU_ITEM_FLAGS_INIT);
    menu_setup_powersaving.SelSaved = 0;
    menu_setup_powersaving.SelNum = 0;
    menu_setup_powersaving.SelCur = 0;
    for (i=0; i<MENU_SETUP_POWERSAVING_SEL_NUM; i++) {
        if (APP_CHECKFLAGS((menu_setup_item_sel_tbls[MENU_SETUP_POWERSAVING]+i)->Flags, MENU_SEL_FLAGS_ENABLE)) {
            menu_setup_powersaving_sels[menu_setup_powersaving.SelNum] = menu_setup_item_sel_tbls[MENU_SETUP_POWERSAVING]+i;
            if (menu_setup_powersaving_sels[menu_setup_powersaving.SelNum]->Val == UserSetting->SetupPref.PowerSaving) {
                menu_setup_powersaving.SelSaved = menu_setup_powersaving.SelNum;
                menu_setup_powersaving.SelCur = menu_setup_powersaving.SelNum;
            }
            menu_setup_powersaving.SelNum++;
        }
    }

    return 0;
}

static int menu_setup_powersaving_get_tab_str(void)
{
    return menu_setup_powersaving_sels[menu_setup_powersaving.SelSaved]->Str;
}

static int menu_setup_powersaving_get_sel_str(int ref)
{
    return menu_setup_powersaving_sels[ref]->Str;
}

static int menu_setup_powersaving_get_sel_bmp(int ref)
{
    return menu_setup_powersaving_sels[ref]->Bmp;
}

static int menu_setup_powersaving_set(void)
{
    AppMenuQuick_SetItem(MENU_SETUP, MENU_SETUP_POWERSAVING);
    AppWidget_On(WIDGET_MENU_QUICK, 0);
    return 0;
}

static int menu_setup_powersaving_sel_set(void)
{
    UserSetting->SetupPref.PowerSaving = (APP_PREF_POWER_SAVING_e)menu_setup_powersaving_sels[menu_setup_powersaving.SelCur]->Val;
    menu_setup_powersaving.SelSaved = menu_setup_powersaving.SelCur;
    //app_util_powersaving_init(0);
    return 0;
}

#ifdef CONFIG_APP_ARD
static int menu_setup_delay_power_off_init(void)
{
    int i = 0;

    APP_ADDFLAGS(menu_setup_delay_power_off.Flags, MENU_ITEM_FLAGS_INIT);
    menu_setup_delay_power_off.SelSaved = 0;
    menu_setup_delay_power_off.SelNum = 0;
    menu_setup_delay_power_off.SelCur = 0;
    for (i=0; i<MENU_SETUP_DELAY_POWER_OFF_SEL_NUM; i++) {
        if (APP_CHECKFLAGS((menu_setup_item_sel_tbls[MENU_SETUP_DELAY_POWER_OFF]+i)->Flags, MENU_SEL_FLAGS_ENABLE)) {
            menu_setup_delay_power_off_sels[menu_setup_delay_power_off.SelNum] = menu_setup_item_sel_tbls[MENU_SETUP_DELAY_POWER_OFF]+i;
            if (menu_setup_delay_power_off_sels[menu_setup_delay_power_off.SelNum]->Val == UserSetting->SetupPref.DelayPowerTime) {
                menu_setup_delay_power_off.SelSaved = menu_setup_delay_power_off.SelNum;
                menu_setup_delay_power_off.SelCur = menu_setup_delay_power_off.SelNum;
            }
            menu_setup_delay_power_off.SelNum++;
        }
    }

    return 0;
}

static int menu_setup_delay_power_off_get_tab_str(void)
{
    return STR_DELAY_POWER_OFF_SET;
}

static int menu_setup_delay_power_off_get_sel_str(int ref)
{
    return menu_setup_delay_power_off_sels[ref]->Str;
}

static int menu_setup_delay_power_off_get_sel_bmp(int ref)
{
    return menu_setup_delay_power_off_sels[ref]->Bmp;
}

static int menu_setup_delay_power_off_set(void)
{
    AppMenuQuick_SetItem(MENU_SETUP, MENU_SETUP_DELAY_POWER_OFF);
    AppWidget_On(WIDGET_MENU_QUICK, 0);
    return 0;
}

static int menu_setup_delay_power_off_sel_set(void)
{
    UserSetting->SetupPref.DelayPowerTime = menu_setup_delay_power_off_sels[menu_setup_delay_power_off.SelCur]->Val;
    menu_setup_delay_power_off.SelSaved = menu_setup_delay_power_off.SelCur;
    cardv_app_delay_poweroff_init();

    return 0;
}
#endif


static int menu_setup_hdmi_init(void)
{
    int i = 0;

    APP_ADDFLAGS(menu_setup_hdmi.Flags, MENU_ITEM_FLAGS_INIT);
    menu_setup_hdmi.SelSaved = 0;
    menu_setup_hdmi.SelNum = 0;
    menu_setup_hdmi.SelCur = 0;
    for (i=0; i<MENU_SETUP_HDMI_SEL_NUM; i++) {
        if (APP_CHECKFLAGS((menu_setup_item_sel_tbls[MENU_SETUP_HDMI]+i)->Flags, MENU_SEL_FLAGS_ENABLE)) {
            menu_setup_hdmi_sels[menu_setup_hdmi.SelNum] = menu_setup_item_sel_tbls[MENU_SETUP_HDMI]+i;
            if (menu_setup_hdmi_sels[menu_setup_hdmi.SelNum]->Val == UserSetting->SetupPref.EnableHDMI) {
                menu_setup_hdmi.SelSaved = menu_setup_hdmi.SelNum;
                menu_setup_hdmi.SelCur = menu_setup_hdmi.SelNum;
            }
            menu_setup_hdmi.SelNum++;
        }
    }

    return 0;
}

static int menu_setup_hdmi_get_tab_str(void)
{
    return menu_setup_hdmi_sels[menu_setup_hdmi.SelSaved]->Str;
}

static int menu_setup_hdmi_get_sel_str(int ref)
{
    return menu_setup_hdmi_sels[ref]->Str;
}

static int menu_setup_hdmi_get_sel_bmp(int ref)
{
    return menu_setup_hdmi_sels[ref]->Bmp;
}

static int menu_setup_hdmi_set(void)
{
    AppMenuQuick_SetItem(MENU_SETUP, MENU_SETUP_HDMI);
    AppWidget_On(WIDGET_MENU_QUICK, 0);
    return 0;
}

static int menu_setup_hdmi_sel_set(void)
{
    menu_setup_hdmi.SelSaved = menu_setup_hdmi.SelCur;

    if ( UserSetting->SetupPref.EnableHDMI != menu_setup_hdmi_sels[menu_setup_hdmi.SelCur]->Val) {
        UserSetting->SetupPref.EnableHDMI = (APP_PREF_HDMI_SUPPORT_e)menu_setup_hdmi_sels[menu_setup_hdmi.SelCur]->Val;
        {
            /* Send the message to the current app. */
            APP_APP_s *curapp;
            AppAppMgt_GetCurApp(&curapp);
            //curapp->OnMessage(AMSG_CMD_RESET_HDMI_PREVIEW, UserSetting->SetupPref.EnableHDMI, 0);
        }

        if (app_status.HdmiPluginFlag == 1) {
            app_status.HdmiPluginFlag = 0;
            AppLibSysVout_SetJackHDMI(0);
            AppLibComSvcHcmgr_SendMsg(HMSG_HDMI_INSERT_SET, 0, 0);
        }
    }
    return 0;
}

#if defined (FLASHLIGHT)
static int menu_setup_flashlight_init(void)
{
    int i = 0;

    APP_ADDFLAGS(menu_setup_flashlight.Flags, MENU_ITEM_FLAGS_INIT);
    menu_setup_flashlight.SelSaved = 0;
    menu_setup_flashlight.SelNum = 0;
    menu_setup_flashlight.SelCur = 0;
    for (i=0; i<MENU_SETUP_FLASHLIGHT_SEL_NUM; i++) {
    if (APP_CHECKFLAGS((menu_setup_item_sel_tbls[MENU_SETUP_FLASHLIGHT]+i)->Flags, MENU_SEL_FLAGS_ENABLE)) {
        menu_setup_flashlight_sels[menu_setup_flashlight.SelNum] = menu_setup_item_sel_tbls[MENU_SETUP_FLASHLIGHT]+i;
        if (menu_setup_flashlight_sels[menu_setup_flashlight.SelNum]->Val == UserSetting->SetupPref.Flashlight) {
            menu_setup_flashlight.SelSaved = menu_setup_flashlight.SelNum;
            menu_setup_flashlight.SelCur = menu_setup_flashlight.SelNum;
        }
        menu_setup_flashlight.SelNum++;
        }
    }

    return 0;
}

static int menu_setup_flashlight_get_tab_str(void)
{
    return menu_setup_flashlight_sels[menu_setup_flashlight.SelSaved]->Str;
}

static int menu_setup_flashlight_get_sel_str(int ref)
{
    return menu_setup_flashlight_sels[ref]->Str;
}

static int menu_setup_flashlight_get_sel_bmp(int ref)
{
    return menu_setup_flashlight_sels[ref]->Bmp;
}

static int menu_setup_flashlight_set(void)
{
    AppMenuQuick_SetItem(MENU_SETUP, MENU_SETUP_FLASHLIGHT);
    AppWidget_On(WIDGET_MENU_QUICK, 0);
    return 0;
}

static int menu_setup_flashlight_sel_set(void)
{
    if (menu_setup_flashlight.SelSaved != menu_setup_flashlight.SelCur) {
        menu_setup_flashlight.SelSaved = menu_setup_flashlight.SelCur;
        UserSetting->SetupPref.Flashlight = menu_setup_flashlight_sels[menu_setup_flashlight.SelCur]->Val;
        app_image_setting_set_flashlight(menu_setup_flashlight_sels[menu_setup_flashlight.SelCur]->Val);
        app_image_setup_flashlight();
        {
            /* Send the message to the current app. */
            APP_APP_s *curapp;
            AppAppMgt_GetCurApp(&curapp);
            curapp->OnMessage(AMSG_CMD_SET_FLASHLIGHT, UserSetting->SetupPref.Flashlight, 0);
        }
    }
    return 0;
}
#else
static int menu_setup_flashlight_init(void)
{
    return 0;
}

static int menu_setup_flashlight_get_tab_str(void)
{
    return 0;
}

static int menu_setup_flashlight_get_sel_str(int ref)
{
    return 0;
}

static int menu_setup_flashlight_get_sel_bmp(int ref)
{
    return 0;
}

static int menu_setup_flashlight_set(void)
{
    return 0;
}

static int menu_setup_flashlight_sel_set(void)
{
    return 0;
}

#endif

static int menu_setup_usb_mode_init(void)
{
    int i = 0;

    APP_ADDFLAGS(menu_setup_usb_mode.Flags, MENU_ITEM_FLAGS_INIT);
    menu_setup_usb_mode.SelSaved = 0;
    menu_setup_usb_mode.SelNum = 0;
    menu_setup_usb_mode.SelCur = 0;
    for (i=0; i<MENU_SETUP_USB_MODE_SEL_NUM; i++) {
        if (APP_CHECKFLAGS((menu_setup_item_sel_tbls[MENU_SETUP_USB_MODE]+i)->Flags, MENU_SEL_FLAGS_ENABLE)) {
            menu_setup_usb_mode_sels[menu_setup_usb_mode.SelNum] = menu_setup_item_sel_tbls[MENU_SETUP_USB_MODE]+i;
            if (menu_setup_usb_mode_sels[menu_setup_usb_mode.SelNum]->Val == UserSetting->SetupPref.USBMode) {
                menu_setup_usb_mode.SelSaved = menu_setup_usb_mode.SelNum;
                menu_setup_usb_mode.SelCur = menu_setup_usb_mode.SelNum;
            }
            menu_setup_usb_mode.SelNum++;
        }
    }

    return 0;
}

static int menu_setup_usb_mode_get_tab_str(void)
{
    return menu_setup_usb_mode_sels[menu_setup_usb_mode.SelSaved]->Str;
}

static int menu_setup_usb_mode_get_sel_str(int ref)
{
    return menu_setup_usb_mode_sels[ref]->Str;
}

static int menu_setup_usb_mode_get_sel_bmp(int ref)
{
    return menu_setup_usb_mode_sels[ref]->Bmp;
}

static int menu_setup_usb_mode_set(void)
{
    AppMenuQuick_SetItem(MENU_SETUP, MENU_SETUP_USB_MODE);
    AppWidget_On(WIDGET_MENU_QUICK, 0);
    return 0;
}

static int menu_setup_usb_mode_sel_set(void)
{
    if (menu_setup_usb_mode.SelSaved != menu_setup_usb_mode.SelCur) {
        menu_setup_usb_mode.SelSaved = menu_setup_usb_mode.SelCur;
        UserSetting->SetupPref.USBMode = (APP_PREF_USB_MODE_e)menu_setup_usb_mode_sels[menu_setup_usb_mode.SelCur]->Val;

        AppLibComSvcHcmgr_SendMsg(AMSG_CMD_SET_USB_MODE, 0, 0);
    }
    return 0;
}

static int menu_setup_input_dimension_mode_init(void)
{
    int i = 0;

    APP_ADDFLAGS(menu_setup_input_dimension_mode.Flags, MENU_ITEM_FLAGS_INIT);
    menu_setup_input_dimension_mode.SelSaved = 0;
    menu_setup_input_dimension_mode.SelNum = 0;
    menu_setup_input_dimension_mode.SelCur = 0;
    for (i=0; i<MENU_SETUP_INPUT_DIMENSION_MODE_SEL_NUM; i++) {
        if (APP_CHECKFLAGS((menu_setup_item_sel_tbls[MENU_SETUP_INPUT_DIMENSION_MODE]+i)->Flags, MENU_SEL_FLAGS_ENABLE)) {
            menu_setup_input_dimension_mode_sels[menu_setup_input_dimension_mode.SelNum] = menu_setup_item_sel_tbls[MENU_SETUP_INPUT_DIMENSION_MODE]+i;
            if (menu_setup_input_dimension_mode_sels[menu_setup_input_dimension_mode.SelNum]->Val == UserSetting->SetupPref.InputDimension) {
                menu_setup_input_dimension_mode.SelSaved = menu_setup_input_dimension_mode.SelNum;
                menu_setup_input_dimension_mode.SelCur = menu_setup_input_dimension_mode.SelNum;
            }
            menu_setup_input_dimension_mode.SelNum++;
        }
    }

    return 0;
}
static int menu_setup_input_dimension_mode_get_tab_str(void)
{
    return menu_setup_input_dimension_mode_sels[menu_setup_input_dimension_mode.SelSaved]->Str;
}
static int menu_setup_input_dimension_mode_get_sel_str(int ref)
{
    return menu_setup_input_dimension_mode_sels[ref]->Str;
}
static int menu_setup_input_dimension_mode_get_sel_bmp(int ref)
{
    return menu_setup_input_dimension_mode_sels[ref]->Bmp;
}
static int menu_setup_input_dimension_mode_set(void)
{
    AppMenuQuick_SetItem(MENU_SETUP, MENU_SETUP_INPUT_DIMENSION_MODE);
    AppWidget_On(WIDGET_MENU_QUICK, 0);
    return 0;
}
static int menu_setup_input_dimension_mode_sel_set(void)
{
     return 0;
}

#ifdef CONFIG_APP_ARD
static int menu_setup_input_near_far_init(void)
{
    int i = 0;

    APP_ADDFLAGS(menu_setup_input_near_far.Flags, MENU_ITEM_FLAGS_INIT);
    menu_setup_input_near_far.SelSaved = 0;
    menu_setup_input_near_far.SelNum = 0;
    menu_setup_input_near_far.SelCur = 0;
    for (i=0; i<MENU_SETUP_INPUT_NEAR_FAR_SEL_NUM; i++) {
        if (APP_CHECKFLAGS((menu_setup_item_sel_tbls[MENU_SETUP_INPUT_NEAR_FAR]+i)->Flags, MENU_SEL_FLAGS_ENABLE)) {
            menu_setup_input_near_far_sels[menu_setup_input_near_far.SelNum] = menu_setup_item_sel_tbls[MENU_SETUP_INPUT_NEAR_FAR]+i;
            if (menu_setup_input_near_far_sels[menu_setup_input_near_far.SelNum]->Val == UserSetting->SetupPref.SensorNearFar) {
                menu_setup_input_near_far.SelSaved = menu_setup_input_near_far.SelNum;
                menu_setup_input_near_far.SelCur = menu_setup_input_near_far.SelNum;
            }
            menu_setup_input_near_far.SelNum++;
        }
    }

    return 0;
}
static int menu_setup_input_near_far_get_tab_str(void)
{
    return menu_setup_input_near_far_sels[menu_setup_input_near_far.SelSaved]->Str;
}
static int menu_setup_input_near_far_get_sel_str(int ref)
{
    return menu_setup_input_near_far_sels[ref]->Str;
}
static int menu_setup_input_near_far_get_sel_bmp(int ref)
{
    return menu_setup_input_near_far_sels[ref]->Bmp;
}
static int menu_setup_input_near_far_set(void)
{
    AppMenuQuick_SetItem(MENU_SETUP, MENU_SETUP_INPUT_NEAR_FAR);
    AppWidget_On(WIDGET_MENU_QUICK, 0);
    return 0;
}
static int menu_setup_input_near_far_sel_set(void)
{
    if (menu_setup_input_near_far.SelSaved != menu_setup_input_near_far.SelCur) {
        menu_setup_input_near_far.SelSaved = menu_setup_input_near_far.SelCur;
        UserSetting->SetupPref.SensorNearFar = menu_setup_input_near_far.Sels[menu_setup_input_near_far.SelCur]->Val;
        UserSetting->SetupPref.SensorSwitch = 1;
        {
            /* Send the message to the current app. */
            APP_APP_s *curapp;
            AppAppMgt_GetCurApp(&curapp);
            curapp->OnMessage(AMSG_CMD_SENSOR_NEAR_FAR_SWITCH, menu_setup_input_near_far.Sels[menu_setup_input_near_far.SelCur]->Val, 0);
        }
    }

     return 0;
}

static int menu_setup_audio_volume_get_cur_val(void)
{
    return UserSetting->AudioPref.AudioVolume;
}

static int menu_setup_audio_volume_get_val_str(int val)
{
    char str[4] = {0};
    UINT16 str_madj_value[4] = {0};

    sprintf(str, "%3d", val);
#ifdef CONFIG_APP_ARD
    AppLibGraph_UpdateStringContext(AppPref_GetLanguageID(), STR_ADJUST_VALUE, str_madj_value);
#else
    AppLibGraph_UpdateStringContext(0, STR_ADJUST_VALUE, str_madj_value);
#endif
    return STR_ADJUST_VALUE;
}

static int menu_setup_audio_volume_val_set(int val)
{
    UserSetting->AudioPref.AudioVolume = (UINT8)val;
    return AppLibAudioDec_SetVolume(UserSetting->AudioPref.AudioVolume);
}

static int menu_setup_audio_volume_init(void)
{
    APP_ADDFLAGS(menu_setup_audio_volume.Flags, MENU_ITEM_FLAGS_INIT);
    menu_setup_audio_volume_sel_tbl.Val = UserSetting->AudioPref.AudioVolume;
    menu_setup_audio_volume_sels = menu_setup_item_sel_tbls[MENU_SETUP_AUDIO_VOLUME];
    menu_setup_audio_volume.SelNum= 1;
    return 0;
}

static int menu_setup_audio_volume_get_tab_str(void)
{
    return STR_AUDIO_VOLUME;
}

static int menu_setup_audio_volume_get_sel_str(int ref)
{
    return menu_setup_audio_volume_sels->Str;
}

static int menu_setup_audio_volume_get_sel_bmp(int ref)
{
    return menu_setup_audio_volume_sels->Bmp;
}

static int menu_setup_audio_volume_set(void)
{
    AppMenuAdj_SetSel(MENU_RECORDER_SETTTING, MENU_SETUP_AUDIO_VOLUME,menu_setup_audio_volume_sels->Id);
    AppWidget_On(WIDGET_MENU_ADJ, 0);
    return 0;
}

static int menu_setup_audio_volume_sel_set(void)
{
    return 0;
}
#endif

static int menu_setup_output_dimension_mode_init(void)
{
    return 0;
}
static int menu_setup_output_dimension_mode_get_tab_str(void)
{
    return menu_setup_output_dimension_mode_sels[menu_setup_output_dimension_mode.SelSaved]->Str;
}
static int menu_setup_output_dimension_mode_get_sel_str(int ref)
{
    return menu_setup_output_dimension_mode_sels[ref]->Str;
}
static int menu_setup_output_dimension_mode_get_sel_bmp(int ref)
{
    return menu_setup_output_dimension_mode_sels[ref]->Bmp;
}
static int menu_setup_output_dimension_mode_set(void)
{
    AppMenuQuick_SetItem(MENU_SETUP, MENU_SETUP_OUTPUT_DIMENSION_MODE);
    AppWidget_On(WIDGET_MENU_QUICK, 0);
    return 0;
}
static int menu_setup_output_dimension_mode_sel_set(void)
{
    if (menu_setup_output_dimension_mode.SelSaved != menu_setup_output_dimension_mode.SelCur) {
        menu_setup_output_dimension_mode.SelSaved = menu_setup_output_dimension_mode.SelCur;
        UserSetting->SetupPref.OutputDimension = menu_setup_output_dimension_mode_sels[menu_setup_output_dimension_mode.SelCur]->Val;
        {
            /* Send the message to the current app. */
            APP_APP_s *curapp;
            AppAppMgt_GetCurApp(&curapp);
            //curapp->OnMessage(AMSG_CMD_SET_OUTPUT_DIMENSION, 0, 0);
        }
    }
    return 0;
}

static int menu_setup_wifi_init(void)
{
    return 0;
}

static int menu_setup_wifi_get_tab_str(void)
{
    return 0;
}

static int menu_setup_wifi_get_sel_str(int ref)
{
    return 0;
}

static int menu_setup_wifi_get_sel_bmp(int ref)
{
    return 0;
}

static int menu_setup_wifi_set(void)
{
    return 0;
}

static int menu_setup_wifi_sel_set(void)
{
    return 0;
}

static int menu_setup_wifi_config_init(void)
{
    return 0;
}

static int menu_setup_wifi_config_get_tab_str(void)
{
    return 0;
}

static int menu_setup_wifi_config_get_sel_str(int ref)
{
    return 0;
}

static int menu_setup_wifi_config_get_sel_bmp(int ref)
{
    return 0;
}

static int menu_setup_wifi_config_set(void)
{
    return 0;
}

static int menu_setup_wifi_config_sel_set(void)
{
    return 0;
}

/**
 * @brief Initialization function of menu_setup_version
 * @return success or not
 */
static int menu_setup_version_init(void)
{
#if 0
    char *ver_br_name, *ver_br_svn;

    // output
    const int output_str_len = 64;
    UINT16 output_str[output_str_len] = {0};

    // branch version
    const int uni_ver_len = 14; // strlen(': SVN r') + strlen('0123456')
    UINT16 uni_ver[uni_ver_len];

    // branch name
    int uni_name_len = output_str_len - uni_ver_len;

    // tmp
    int tmp_len = 0;
    UINT16 ver_uni_tmp[16];

    APP_ADDFLAGS(menu_setup_version.Flags, MENU_ITEM_FLAGS_INIT);

    // Get SVN info
    ver_br_name = get_branch_name("prkapp");
    ver_br_svn = get_branch_ver("prkapp");

    // Get SVN number
    gui_ascii_to_unicode(": SVN r", uni_ver);
    gui_ascii_to_unicode(ver_br_svn, ver_uni_tmp);
    w_strcat(uni_ver, ver_uni_tmp);

    // Generate string
    tmp_len = gui_ascii_to_unicode(ver_br_name, ver_uni_tmp);

    if (tmp_len < uni_name_len) {
        gui_ascii_to_unicode(ver_br_name, output_str);
    } else {
        memcpy(output_str, ver_uni_tmp, (uni_name_len-1)*sizeof(UINT16));
        output_str[uni_name_len] = 0;
        AmbaPrint("Warning! SVN branch: %s", ver_br_name);
    }

    gui_ascii_to_unicode(": SVN r", ver_uni_tmp);
    w_strcat(output_str, ver_uni_tmp);
    gui_ascii_to_unicode(ver_br_svn, ver_uni_tmp);
    w_strcat(output_str, ver_uni_tmp);

    // Update string
    AppLibGraph_UpdateStringContext(0, STR_SVN_VERSION, output_str);
#endif
    return 0;
}

/**
 * @brief Function to get string Id to display on main menu
 * @return int String ID
 */
static int menu_setup_version_get_tab_str(void)
{
    return STR_VERSION;
}
#ifdef CONFIG_APP_ARD
extern const char *pAmbaVer_LinkVer_Date;
#endif

/**
 * @brief Dummy function to handle SET operation
 * @return Don't care
 */
static int menu_setup_version_set(void)
{
#ifdef CONFIG_APP_ARD
    // UINT16 OutputStr0[64] = {'S', 'D', 'K', ' ', 'v', 'e', 'r', 's', 'i', 'o', 'n', ' ', 'i', 's', ':', '\0'};
    UINT16 OutputStr0[64] = {'E', '1', '0', '0', '_', 'F', 'W', '_', 'V', 'e', 'r', 's', 'i', 'o', 'n',':', '\0'};
    //UINT16 OutputStr1[64] = {'\0'};
    
    UINT16 OutputStr1[64] = {'V','1','.','2','.','0','_','1','6','0','7','1','5','_','0','1','\0'};
    UINT16 *PtrStrDst;
    const char *PtrStrSrc;
#if 0
    PtrStrDst = OutputStr1;
    PtrStrSrc=pAmbaVer_LinkVer_Date;

    //Omit week info
    while (*PtrStrSrc != ' ')
        PtrStrSrc++;

    PtrStrSrc++;
    while (*PtrStrSrc) {
        if(*PtrStrSrc == 'C')
            PtrStrSrc += 4;

        *PtrStrDst++ = *PtrStrSrc++;
    }

    *PtrStrDst = '\0';
#endif
    AppLibGraph_UpdateStringContext(AppPref_GetLanguageID(), STR_INFO_PROMPT_CONTENT_ITEM0, OutputStr0);
    AppLibGraph_UpdateString(GRAPH_CH_DUAL, GOBJ_INFO_PROMPT_BG_STR_ITEM0, STR_INFO_PROMPT_CONTENT_ITEM0);

    AppLibGraph_UpdateStringContext(AppPref_GetLanguageID(), STR_INFO_PROMPT_CONTENT_ITEM1, OutputStr1);
    AppLibGraph_UpdateString(GRAPH_CH_DUAL, GOBJ_INFO_PROMPT_BG_STR_ITEM1, STR_INFO_PROMPT_CONTENT_ITEM1);

    AppLibGraph_UpdateStringContext(AppPref_GetLanguageID(), STR_INFO_PROMPT_CONTENT_ITEM2, (UINT16 *)"");
    AppLibGraph_UpdateString(GRAPH_CH_DUAL, GOBJ_INFO_PROMPT_BG_STR_ITEM2, STR_INFO_PROMPT_CONTENT_ITEM2);

    AppLibGraph_UpdateStringContext(AppPref_GetLanguageID(), STR_INFO_PROMPT_CONTENT_ITEM3, (UINT16 *)"");
    AppLibGraph_UpdateString(GRAPH_CH_DUAL, GOBJ_INFO_PROMPT_BG_STR_ITEM3, STR_INFO_PROMPT_CONTENT_ITEM3);

    AppWidget_On(WIDGET_INFO_PROMPT, 0);

#endif

    return 0;
}
#ifdef CONFIG_APP_ARD
static int menu_setup_language_init(void)
{
    int i = 0;

    APP_ADDFLAGS(menu_setup_language.Flags, MENU_ITEM_FLAGS_INIT);
    menu_setup_language.SelSaved = 0;
    menu_setup_language.SelNum = 0;
    menu_setup_language.SelCur = 0;
    for (i=0; i<MENU_SETUP_LANGUAGE_SEL_NUM; i++) {
        if (APP_CHECKFLAGS((menu_setup_item_sel_tbls[MENU_SETUP_LANGUAGE]+i)->Flags, MENU_SEL_FLAGS_ENABLE)) {
            menu_setup_language_sels[menu_setup_language.SelNum] = menu_setup_item_sel_tbls[MENU_SETUP_LANGUAGE]+i;
            if (menu_setup_language_sels[menu_setup_language.SelNum]->Val == UserSetting->SetupPref.LanguageID) {
                menu_setup_language.SelSaved = menu_setup_language.SelNum;
                menu_setup_language.SelCur = menu_setup_language.SelNum;
            }
            menu_setup_language.SelNum++;
        }
    }

    return 0;
}
static int menu_setup_language_get_tab_str(void)
{
    return STR_LANGUAGE_SETTING;
}
static int menu_setup_language_get_sel_str(int ref)
{
    return menu_setup_language_sels[ref]->Str;
}
static int menu_setup_language_get_sel_bmp(int ref)
{
    return menu_setup_language_sels[ref]->Bmp;
}
static int menu_setup_language_set(void)
{
    AppMenuQuick_SetItem(MENU_RECORDER_SETTTING, MENU_LANGUAGE_SET);
    AppWidget_On(WIDGET_MENU_QUICK, 0);
    return 0;
}

#ifdef CONFIG_APP_ARD
extern GRAPH_CHAN_s* AppLibGraph_GetChannelData(GRAPHICS_CHAN_e canvasChn);
extern BOOLEAN_e _AppLibGraph_String_Load(GRAPHICS_CHAN_e canvasChn, UINT32 langIdx, GRAPH_CHAN_s *ChannelData);
#endif
static int menu_setup_language_sel_set(void)
{
     int pre_dis;
    if (menu_setup_language.SelSaved != menu_setup_language.SelCur) {
        menu_setup_language.SelSaved = menu_setup_language.SelCur;
        pre_dis = UserSetting->SetupPref.LanguageID;		
        UserSetting->SetupPref.LanguageID = menu_setup_language.Sels[menu_setup_language.SelCur]->Val;
	 record_item_sel(UserSetting->SetupPref.LanguageID,pre_dis);
#ifdef CONFIG_APP_ARD
       _AppLibGraph_String_Load(GRAPHICS_CHANNEL_D, UserSetting->SetupPref.LanguageID, AppLibGraph_GetChannelData(GRAPHICS_CHANNEL_D));
       _AppLibGraph_String_Load(GRAPHICS_CHANNEL_F, UserSetting->SetupPref.LanguageID, AppLibGraph_GetChannelData(GRAPHICS_CHANNEL_F));
       _AppLibGraph_String_Load(GRAPHICS_CHANNEL_BLEND, UserSetting->SetupPref.LanguageID, AppLibGraph_GetChannelData(GRAPHICS_CHANNEL_BLEND));
#endif
    }

     return 0;
}

static int menu_setup_record_mode_init(void)
{
    int i = 0;

    APP_ADDFLAGS(menu_setup_record_mode.Flags, MENU_ITEM_FLAGS_INIT);
    menu_setup_record_mode.SelSaved = 0;
    menu_setup_record_mode.SelNum = 0;
    menu_setup_record_mode.SelCur = 0;
    for (i=0; i<MENU_SETUP_RECORD_MODE_SEL_NUM; i++) {
        if (APP_CHECKFLAGS((menu_setup_item_sel_tbls[MENU_SETUP_RECORD_MODE]+i)->Flags, MENU_SEL_FLAGS_ENABLE)) {
            menu_setup_record_mode_sels[menu_setup_record_mode.SelNum] = menu_setup_item_sel_tbls[MENU_SETUP_RECORD_MODE]+i;
            if (menu_setup_record_mode_sels[menu_setup_record_mode.SelNum]->Val == UserSetting->SetupPref.RecordMode) {
                menu_setup_record_mode.SelSaved = menu_setup_record_mode.SelNum;
                menu_setup_record_mode.SelCur = menu_setup_record_mode.SelNum;
            }
            menu_setup_record_mode.SelNum++;
        }
    }

    return 0;
}
static int menu_setup_record_mode_get_tab_str(void)
{
#ifdef CONFIG_APP_ARD
    return STR_RECORD_MODE;
#else
    return menu_setup_record_mode_sels[menu_setup_record_mode.SelSaved]->Str;
#endif
}
static int menu_setup_record_mode_get_sel_str(int ref)
{
    return menu_setup_record_mode_sels[ref]->Str;
}
static int menu_setup_record_mode_get_sel_bmp(int ref)
{
    return menu_setup_record_mode_sels[ref]->Bmp;
}
static int menu_setup_record_mode_set(void)
{
    AppMenuQuick_SetItem(MENU_SETUP, MENU_SETUP_RECORD_MODE);
    AppWidget_On(WIDGET_MENU_QUICK, 0);
    return 0;
}
static int menu_setup_record_mode_sel_set(void)
{
    if (menu_setup_record_mode.SelSaved != menu_setup_record_mode.SelCur) {
        menu_setup_record_mode.SelSaved = menu_setup_record_mode.SelCur;
        UserSetting->SetupPref.RecordMode = menu_setup_record_mode.Sels[menu_setup_record_mode.SelCur]->Val;
        app_status.cardv_auto_encode = (UserSetting->SetupPref.RecordMode == RECORD_MODE_AUTO)?1:0;
        {
            /* Send the message to the current app. */
            //APP_APP_s *curapp;
            //AppAppMgt_GetCurApp(&curapp);
            //curapp->OnMessage(AMSG_CMD_SET_RECORD_MODE, menu_setup_record_mode.Sels[menu_setup_record_mode.SelCur]->Val, 0);
        }
    }

     return 0;
}

static int menu_setup_card_volume_init(void)
{

    return 0;
}

/**
 * @brief Function to get string Id to display on main menu
 * @return int String ID
 */
static int menu_setup_card_volume_get_tab_str(void)
{
    return STR_CARD_VOLUME;
}

static void uint64_to_wstr(UINT64 val, WCHAR *wstr)
{
    WCHAR *pwstr, wstr_buff[64] = {'\0'};
    UINT64 quotient, remainder, tmp;

    pwstr = wstr_buff;
    quotient = val;

    while(quotient){
        //Caculate remainder
        tmp = quotient;
        quotient = quotient/10;
        remainder = tmp - 10*quotient;

        *pwstr = '0' + remainder;
        pwstr++;
    }

    pwstr--;
    while(pwstr>=wstr_buff) {
        *wstr++ = *pwstr--;
    }

    *wstr = '\0';
}

void convert_volume_to_str(UINT64 Volume, UINT16* OutputStr)
{
    UINT16 *Puint16 = NULL;
    WCHAR WConvertStr[64] = {'\0'}, *PWstr = NULL;
    UINT64 quotient = 0, tmp = 0;

    Puint16 = OutputStr;
    while(*Puint16 != '\0')
        Puint16++;

    //Get MB part
    tmp = Volume;
    quotient = Volume/(1024*1024);

    if (quotient) {
        WConvertStr[0] = '\0';
        uint64_to_wstr(quotient, WConvertStr);

        for(PWstr=WConvertStr; *PWstr!='\0'; PWstr++, Puint16++)
            *Puint16 = *PWstr;
        *Puint16++ = 'M';
        *Puint16++ = 'B';
    } else {
        *Puint16++ = '0';
        *Puint16++ = 'M';
        *Puint16++ = 'B';
    }

    *Puint16 = '\0';

}

/**
 * @brief Dummy function to handle SET operation
 * @return Don't care
 */
static int menu_setup_card_volume_set(void)
{
    UINT16 OutputStr0[64] = {'C', 'a', 'r', 'd', ' ', 'v', 'o', 'l', 'u', 'm', 'e', ' ', 'i', 's', ':', '\0'};
    UINT16 OutputStr1[64] = {'T', 'o', 't', 'a', 'l', ':', '\0'};
    UINT16 OutputStr2[64] = {'U', 's', 'e', 'd', ':', '\0'};
    UINT16 OutputStr3[64] = {'R', 'e', 's', 'e', 'r', 'v', 'e', 'd', ':', '\0'};

    UINT16 OutputStr0_chs[64] = { 0x5361,0x5bb9,0x91cf,0x662f,0xff1a,'\0'};
    UINT16 OutputStr1_chs[64] = { 0x5361,0x603b,0x91cf,0xff1a,'\0'};
    UINT16 OutputStr2_chs[64] = { 0x4f7f,0x7528,0x91cf,0xff1a,'\0'};
    UINT16 OutputStr3_chs[64] = { 0x5269,0x4f59,0x91cf,0xff1a,'\0'};

    UINT64 TotalSize = 0, FreeSize = 0, UsedSize = 0;
    AMP_CFS_DEVINF DevIn;
    char Drive = 'A';
    int ReturnValue = 0;

    Drive = AppLibCard_GetActiveDrive();
    ReturnValue = AmpCFS_GetDev(Drive, &DevIn);

    if (ReturnValue < 0) {
        TotalSize = 0;
        FreeSize = 0;
        AmbaPrint("!!!Fail to get storage info");
    } else {
        TotalSize = (UINT64)DevIn.Cls * DevIn.Bps * DevIn.Spc;
        FreeSize = (UINT64)DevIn.Ucl * DevIn.Bps * DevIn.Spc;
        UsedSize = TotalSize - FreeSize;
    }
    AmbaPrint("!!!TotalSize %llu, FreeSize %llu", TotalSize, FreeSize);

    if(UserSetting->SetupPref.LanguageID==1){
        /* update card info */
        AppLibGraph_UpdateStringContext(AppPref_GetLanguageID(), STR_INFO_PROMPT_CONTENT_ITEM0, OutputStr0_chs);
        AppLibGraph_UpdateString(GRAPH_CH_DUAL, GOBJ_INFO_PROMPT_BG_STR_ITEM0, STR_INFO_PROMPT_CONTENT_ITEM0);


        convert_volume_to_str(TotalSize, OutputStr1_chs);
        AppLibGraph_UpdateStringContext(AppPref_GetLanguageID(), STR_INFO_PROMPT_CONTENT_ITEM1, OutputStr1_chs);
        AppLibGraph_UpdateString(GRAPH_CH_DUAL, GOBJ_INFO_PROMPT_BG_STR_ITEM1, STR_INFO_PROMPT_CONTENT_ITEM1);

        convert_volume_to_str(UsedSize, OutputStr2_chs);
        AppLibGraph_UpdateStringContext(AppPref_GetLanguageID(), STR_INFO_PROMPT_CONTENT_ITEM2, OutputStr2_chs);
        AppLibGraph_UpdateString(GRAPH_CH_DUAL, GOBJ_INFO_PROMPT_BG_STR_ITEM2, STR_INFO_PROMPT_CONTENT_ITEM2);

        convert_volume_to_str(FreeSize, OutputStr3_chs);
        AppLibGraph_UpdateStringContext(AppPref_GetLanguageID(), STR_INFO_PROMPT_CONTENT_ITEM3, OutputStr3_chs);
        AppLibGraph_UpdateString(GRAPH_CH_DUAL, GOBJ_INFO_PROMPT_BG_STR_ITEM3, STR_INFO_PROMPT_CONTENT_ITEM3);
    } else {
        /* update card info */
        AppLibGraph_UpdateStringContext(AppPref_GetLanguageID(), STR_INFO_PROMPT_CONTENT_ITEM0, OutputStr0);
        AppLibGraph_UpdateString(GRAPH_CH_DUAL, GOBJ_INFO_PROMPT_BG_STR_ITEM0, STR_INFO_PROMPT_CONTENT_ITEM0);


        convert_volume_to_str(TotalSize, OutputStr1);
        AppLibGraph_UpdateStringContext(AppPref_GetLanguageID(), STR_INFO_PROMPT_CONTENT_ITEM1, OutputStr1);
        AppLibGraph_UpdateString(GRAPH_CH_DUAL, GOBJ_INFO_PROMPT_BG_STR_ITEM1, STR_INFO_PROMPT_CONTENT_ITEM1);

        convert_volume_to_str(UsedSize, OutputStr2);
        AppLibGraph_UpdateStringContext(AppPref_GetLanguageID(), STR_INFO_PROMPT_CONTENT_ITEM2, OutputStr2);
        AppLibGraph_UpdateString(GRAPH_CH_DUAL, GOBJ_INFO_PROMPT_BG_STR_ITEM2, STR_INFO_PROMPT_CONTENT_ITEM2);

        convert_volume_to_str(FreeSize, OutputStr3);
        AppLibGraph_UpdateStringContext(AppPref_GetLanguageID(), STR_INFO_PROMPT_CONTENT_ITEM3, OutputStr3);
        AppLibGraph_UpdateString(GRAPH_CH_DUAL, GOBJ_INFO_PROMPT_BG_STR_ITEM3, STR_INFO_PROMPT_CONTENT_ITEM3);
    }
    /* show info */
    AppWidget_On(WIDGET_INFO_PROMPT, 0);

    return 0;
}

static int menu_setup_beep_sound_init(void)
{
    int i = 0;

    APP_ADDFLAGS(menu_setup_beep_sound.Flags, MENU_ITEM_FLAGS_INIT);
    menu_setup_beep_sound.SelSaved = 0;
    menu_setup_beep_sound.SelNum = 0;
    menu_setup_beep_sound.SelCur = 0;
    for (i=0; i<MENU_SETUP_BEEP_SOUND_SEL_NUM; i++) {
        if (APP_CHECKFLAGS((menu_setup_item_sel_tbls[MENU_SETUP_BEEP_SOUND]+i)->Flags, MENU_SEL_FLAGS_ENABLE)) {
            menu_setup_beep_sound_sels[menu_setup_beep_sound.SelNum] = menu_setup_item_sel_tbls[MENU_SETUP_BEEP_SOUND]+i;
            if (menu_setup_beep_sound_sels[menu_setup_beep_sound.SelNum]->Val == UserSetting->SetupPref.beep_sound ) {
                menu_setup_beep_sound.SelSaved = menu_setup_beep_sound.SelNum;
                menu_setup_beep_sound.SelCur = menu_setup_beep_sound.SelNum;
            }
            menu_setup_beep_sound.SelNum++;
        }
    }

    return 0;
}

static int menu_setup_beep_sound_get_tab_str(void)
{
    return STR_BEEP_ON_OFF_SET;
}

static int menu_setup_beep_sound_get_sel_str(int ref)
{
    return menu_setup_beep_sound_sels[ref]->Str;
}

static int menu_setup_beep_sound_get_sel_bmp(int ref)
{
    return menu_setup_beep_sound_sels[ref]->Bmp;
}

static int menu_setup_beep_sound_set(void)
{
//#if defined(SZ_ARD_APP_EN)
//    app_widget_menu_quick_set_item_title_str(STR_BEEP_ON_OFF_SET);
//#endif
    AppMenuQuick_SetItem(MENU_SETUP, MENU_SETUP_BEEP_SOUND);
    AppWidget_On(WIDGET_MENU_QUICK, 0);
    return 0;
}

static int menu_setup_beep_sound_sel_set(void)
{
    UINT8 enable;
    if (menu_setup_beep_sound.SelSaved != menu_setup_beep_sound.SelCur) {
        menu_setup_beep_sound.SelSaved = menu_setup_beep_sound.SelCur;
        UserSetting->SetupPref.beep_sound = menu_setup_beep_sound_sels[menu_setup_beep_sound.SelCur]->Val;
        enable = (UserSetting->SetupPref.beep_sound == BEEP_SOUND_ON)?1:0;
        AppLibAudioDec_Beep_Enable(enable);
    }

    return 0;
}

static int menu_setup_backlight_auto_init(void)
{
    int i = 0;

    APP_ADDFLAGS(menu_setup_backlight_auto.Flags, MENU_ITEM_FLAGS_INIT);
    menu_setup_backlight_auto.SelSaved = 0;
    menu_setup_backlight_auto.SelNum = 0;
    menu_setup_backlight_auto.SelCur = 0;
    for (i=0; i<MENU_SETUP_BACKLIGHT_AUTO_SEL_NUM; i++) {
        if (APP_CHECKFLAGS((menu_setup_item_sel_tbls[MENU_SETUP_BACKLIGHT_AUTO]+i)->Flags, MENU_SEL_FLAGS_ENABLE)) {
            menu_setup_backlight_auto_sels[menu_setup_backlight_auto.SelNum] = menu_setup_item_sel_tbls[MENU_SETUP_BACKLIGHT_AUTO]+i;
            if (menu_setup_backlight_auto_sels[menu_setup_backlight_auto.SelNum]->Val == UserSetting->SetupPref.backlightoff_time) {
                menu_setup_backlight_auto.SelSaved = menu_setup_backlight_auto.SelNum;
                menu_setup_backlight_auto.SelCur = menu_setup_backlight_auto.SelNum;
            }
            menu_setup_backlight_auto.SelNum++;
        }
    }

    return 0;
}

static int menu_setup_backlight_auto_get_tab_str(void)
{
#if CONFIG_APP_ARD
    return STR_BACKLIGHTOFF_SET;
#else
    return menu_setup_backlight_auto_sels[menu_setup_backlight_auto.sel_saved]->Str;
#endif
}

static int menu_setup_backlight_auto_get_sel_str(int ref)
{
    return menu_setup_backlight_auto_sels[ref]->Str;
}

static int menu_setup_backlight_auto_get_sel_bmp(int ref)
{
    return menu_setup_backlight_auto_sels[ref]->Bmp;
}

static int menu_setup_backlight_auto_set(void)
{
    AppMenuQuick_SetItem(MENU_RECORDER_SETTTING, MENU_BACKLIGHT);
    AppWidget_On(WIDGET_MENU_QUICK, 0);
    return 0;
}

static int menu_setup_backlight_auto_sel_set(void)
{
    int pre_dis;

    pre_dis = UserSetting->SetupPref.backlightoff_time;
    switch(UserSetting->SetupPref.backlightoff_time)
    {
        case BACKLIGHTOFF_TIME_OFF:
          pre_dis = 0;
	      break;	
        case BACKLIGHTOFF_TIME_1_MIN:
          pre_dis = 1;
		break;	
        case BACKLIGHTOFF_TIME_3_MIN:
          pre_dis = 2;
		break;	
        case BACKLIGHTOFF_TIME_5_MIN:
          pre_dis = 3;
		break;	
	 default:
	 	break;    
    }
    UserSetting->SetupPref.backlightoff_time = menu_setup_backlight_auto_sels[menu_setup_backlight_auto.SelCur]->Val;
    menu_setup_backlight_auto.SelSaved = menu_setup_backlight_auto.SelCur;
    AmbaPrint("backlight = %d",UserSetting->SetupPref.backlightoff_time);
    switch(UserSetting->SetupPref.backlightoff_time)
    {
        case BACKLIGHTOFF_TIME_OFF:
            record_item_sel(0,pre_dis);
		break;	
        case BACKLIGHTOFF_TIME_1_MIN:
            record_item_sel(1,pre_dis);
		break;	
        case BACKLIGHTOFF_TIME_3_MIN:
            record_item_sel(2,pre_dis);
		break;	
        case BACKLIGHTOFF_TIME_5_MIN:
            record_item_sel(3,pre_dis);
		break;	
	 default:
	 	break;
    }
    //if(UserSetting->SetupPref.backlightoff_time == BACKLIGHTOFF_TIME_OFF)		
    if(BACKLIGHTOFF_TIME_OFF == UserSetting->SetupPref.backlightoff_time) {
        cardv_app_screen_handler_timer(0);
    }else {
        cardv_app_screen_handler_timer(1);
    }

    return 0;
}

static int SetupStampItemSet(int item_idx)
{
    UserSetting->VideoStampPref.StampDate = Setup_Stamp_Choose_Item[MENU_SETUP_STAMP_CHOOSE_DATE].check_or_not;
    UserSetting->VideoStampPref.StampTime = Setup_Stamp_Choose_Item[MENU_SETUP_STAMP_CHOOSE_TIME].check_or_not;
    UserSetting->VideoStampPref.StampDriverId = Setup_Stamp_Choose_Item[MENU_SETUP_STAMP_CHOOSE_DRIVERID].check_or_not;
    return 0;
}

static int menu_setup_stamp_choose_init(void)
{
    Setup_Stamp_Choose_Item[MENU_SETUP_STAMP_CHOOSE_DATE].item_id = MENU_SETUP_STAMP_CHOOSE_DATE;
    Setup_Stamp_Choose_Item[MENU_SETUP_STAMP_CHOOSE_DATE].string_id = STR_STAMP_DATE;
    Setup_Stamp_Choose_Item[MENU_SETUP_STAMP_CHOOSE_DATE].check_or_not= UserSetting->VideoStampPref.StampDate;

    Setup_Stamp_Choose_Item[MENU_SETUP_STAMP_CHOOSE_TIME].item_id = MENU_SETUP_STAMP_CHOOSE_TIME;
    Setup_Stamp_Choose_Item[MENU_SETUP_STAMP_CHOOSE_TIME].string_id = STR_STAMP_TIME;
    Setup_Stamp_Choose_Item[MENU_SETUP_STAMP_CHOOSE_TIME].check_or_not= UserSetting->VideoStampPref.StampTime;

    Setup_Stamp_Choose_Item[MENU_SETUP_STAMP_CHOOSE_DRIVERID].item_id = MENU_SETUP_STAMP_CHOOSE_DRIVERID;
    Setup_Stamp_Choose_Item[MENU_SETUP_STAMP_CHOOSE_DRIVERID].string_id = STR_STAMP_DRIVER_ID;
    Setup_Stamp_Choose_Item[MENU_SETUP_STAMP_CHOOSE_DRIVERID].check_or_not= UserSetting->VideoStampPref.StampDriverId;

    return 0;
}

static int menu_setup_stamp_choose_get_tab_str(void)
{
    return STR_STAMP_SETTING;
}

static int menu_setup_stamp_choose_set(void)
{
    app_widget_check_box_init_item( MENU_SETUP_STAMP_CHOOSE_NUM, &Setup_Stamp_Choose_Item[0], &SetupStampItemSet);
    app_widget_menu_check_box_set_item(MENU_SETUP, MENU_SETUP_STAMP_CHOOSE);
    AppWidget_On(WIDGET_MENU_CKBX, 0);

    return 0;
}

#endif

#ifdef CONFIG_ECL_GUI
static int menu_adas_font_car_alarm_init(void)
{
    return 0;
}

static int menu_adas_font_car_alarm_get_tab_str(void)
{
   return STR_MENU_FONT_CAR;
}

static int menu_adas_font_car_alarm_set(void)
{
    UserSetting->SetupPref.ldws_mode_onoff = !UserSetting->SetupPref.ldws_mode_onoff;
    Adas_Functions_sel(0,UserSetting->SetupPref.ldws_mode_onoff);
    AppPref_Save();   	
    return 0;
}

static int menu_adas_change_road_init(void)
{
    return 0;
}

static int menu_adas_change_road_get_tab_str(void)
{
   return STR_MENU_CHANGE_ROAD;
}

static int menu_adas_change_road_set(void)
{
    UserSetting->SetupPref.fcws_mode_onoff = !UserSetting->SetupPref.fcws_mode_onoff;
    Adas_Functions_sel(1,UserSetting->SetupPref.fcws_mode_onoff);	
    AppPref_Save();   	
    return 0;
}

static int menu_adas_keep_dis_init(void)
{
    return 0;
}

static int menu_adas_keep_dis_get_tab_str(void)
{
   return STR_MENU_KEEP_DIS;
}

static int menu_adas_keep_dis_set(void)
{
    UserSetting->SetupPref.hmws_mode_onoff = !UserSetting->SetupPref.hmws_mode_onoff;
    Adas_Functions_sel(2,UserSetting->SetupPref.hmws_mode_onoff);	
    AppPref_Save();   	
    return 0;
}

static int menu_adas_start_alarm_init(void)
{
    return 0;
}

static int menu_adas_start_alarm_get_tab_str(void)
{
   return STR_MENU_LAUNCH_WARNING;
}

static int menu_adas_start_alarm_set(void)
{
    UserSetting->SetupPref.fcmr_mode_onoff = !UserSetting->SetupPref.fcmr_mode_onoff;
    Adas_Functions_sel(3,UserSetting->SetupPref.fcmr_mode_onoff);	
    AppPref_Save();   	
    return 0;
}

static int menu_adas_alarm_near_init(void)
{
    return 0;
}

static int menu_adas_alarm_near_get_tab_str(void)
{
   return STR_ADAS_ALARM_NEAR;
}

static int menu_adas_alarm_near_set(void)
{
	int pre_sel_dis;
	
	/*if(UserSetting->SetupPref.adas_alarm_dis == 1.2)
	{
	  	pre_sel_dis = 1;
	}	
	if(UserSetting->SetupPref.adas_alarm_dis == 1.5)
	{
	  	pre_sel_dis = 2;
	}	
	if(UserSetting->SetupPref.adas_alarm_dis == 1.0)
	{
	  	pre_sel_dis = 0;		 
	}*/
       pre_sel_dis = UserSetting->SetupPref.adas_alarm_dis;			
       UserSetting->SetupPref.adas_alarm_dis = 0;		
       language_sel(0,pre_sel_dis);
	AppPref_Save();   	   
    return 0;
}

static int menu_adas_alarm_middle_init(void)
{
    return 0;
}

static int menu_adas_alarm_middle_get_tab_str(void)
{
   return STR_ADAS_ALARM_MIDDLE;
}

static int menu_adas_alarm_middle_set(void)
{
	int pre_sel_dis;
	
	/*if(UserSetting->SetupPref.adas_alarm_dis == 1.2)
	{
	  	pre_sel_dis = 1;
	}	
	if(UserSetting->SetupPref.adas_alarm_dis == 1.5)
	{
	  	pre_sel_dis = 2;
	}	
	if(UserSetting->SetupPref.adas_alarm_dis == 1.0)	
	{
	  	pre_sel_dis = 0;		 
	}*/	
       pre_sel_dis = UserSetting->SetupPref.adas_alarm_dis;		
       UserSetting->SetupPref.adas_alarm_dis = 1;		
       language_sel(1,pre_sel_dis);
	AppPref_Save();   	   
    return 0;
}

static int menu_adas_start_cali_init(void)
{
    
    return 0;

}

static int menu_adas_start_cali_get_tab_str(void)
{
   return STR_MENU_ADAS_M_CALI;
}


static int menu_adas_start_cali_set(void)
{
    APP_APP_s *curapp;
    extern record_sta_set(int sta);
    record_sta_set(1);
    AppWidget_Off(WIDGET_ALL, 0);   	   	
    AppAppMgt_GetCurApp(&curapp);
    curapp->OnMessage(HMSG_USER_DEL_BUTTON, 0,0);

    return 0;
}
static int menu_adas_start_cali_sel_set(void)
{
	
    return 0;
}

static int menu_adas_auto_cali_init(void)
{
    Adas_Functions_sel(1,UserSetting->SetupPref.adas_auto_cal_onoff);   
    return 0;
}

static int menu_adas_auto_cali_get_tab_str(void)
{
   return STR_MENU_ADAS_AUTO_CALI;
}

static int menu_adas_auto_cali_set(void)
{ 
    if(UserSetting->SetupPref.adas_auto_cal_onoff==0)
      {
          UserSetting->SetupPref.adas_auto_cal_onoff=1;
          UserSetting->SetupPref.radar_cal_offset=0;
      }
      else
      {
          UserSetting->SetupPref.adas_auto_cal_onoff=0;
      }
      AppLibVideo_Set_Radar_Calibration_Mode( UserSetting->SetupPref.adas_auto_cal_onoff);
      AppPref_Save(); 
      Adas_Functions_sel(1,UserSetting->SetupPref.adas_auto_cal_onoff);   
      rec_cam_func(REC_CAM_CALIBRATION_ICN,UserSetting->SetupPref.adas_auto_cal_onoff,0);
      return 0;
}



static int menu_adas_alarm_far_init(void)
{
    return 0;
}

static int menu_adas_alarm_far_get_tab_str(void)
{
   return STR_ADAS_ALARM_FAR;
}

static int menu_adas_alarm_far_set(void)
{
	int pre_sel_dis;
	
	/*if(UserSetting->SetupPref.adas_alarm_dis == 1.2)
	{
	  	pre_sel_dis = 1;
	}	
	if(UserSetting->SetupPref.adas_alarm_dis == 1.5)
	{
	  	pre_sel_dis = 2;
	}	
	if(UserSetting->SetupPref.adas_alarm_dis == 1.0)		
	{
	  	pre_sel_dis = 0;		 
	}*/	 
       pre_sel_dis = UserSetting->SetupPref.adas_alarm_dis;	
       UserSetting->SetupPref.adas_alarm_dis = 2;		
       language_sel(2,pre_sel_dis);
	AppPref_Save();   
    return 0;
}

static int menu_setup_chinese_init(void)
{
    return 0;
}

static int menu_setup_chinese_get_tab_str(void)
{
   return STR_LANGUAGE_CHINESE_SIMPLIFIED;
}

static int menu_setup_chinese_set(void)
{
      int pre_sel;

	 pre_sel = UserSetting->SetupPref.LanguageID;
       UserSetting->SetupPref.LanguageID = MENU_SETUP_LANGUAGE_CHINESE_SIMPLIFIED;
	language_sel(MENU_SETUP_LANGUAGE_CHINESE_SIMPLIFIED,pre_sel);	
#ifdef CONFIG_APP_ARD
       _AppLibGraph_String_Load(GRAPHICS_CHANNEL_D, UserSetting->SetupPref.LanguageID, AppLibGraph_GetChannelData(GRAPHICS_CHANNEL_D));
       _AppLibGraph_String_Load(GRAPHICS_CHANNEL_F, UserSetting->SetupPref.LanguageID, AppLibGraph_GetChannelData(GRAPHICS_CHANNEL_F));
       _AppLibGraph_String_Load(GRAPHICS_CHANNEL_BLEND, UserSetting->SetupPref.LanguageID, AppLibGraph_GetChannelData(GRAPHICS_CHANNEL_BLEND));
#endif
    return 0;
}

static int menu_setup_english_init(void)
{
    return 0;
}

static int menu_setup_english_get_tab_str(void)
{
   return STR_LANGUAGE_ENGLISH;
}

static int menu_setup_english_set(void)
{
       int pre_sel;

	 pre_sel = UserSetting->SetupPref.LanguageID;
       UserSetting->SetupPref.LanguageID = MENU_SETUP_LANGUAGE_ENGLISH;
	language_sel(MENU_SETUP_LANGUAGE_ENGLISH,pre_sel);	
#ifdef CONFIG_APP_ARD
       _AppLibGraph_String_Load(GRAPHICS_CHANNEL_D, UserSetting->SetupPref.LanguageID, AppLibGraph_GetChannelData(GRAPHICS_CHANNEL_D));
       _AppLibGraph_String_Load(GRAPHICS_CHANNEL_F, UserSetting->SetupPref.LanguageID, AppLibGraph_GetChannelData(GRAPHICS_CHANNEL_F));
       _AppLibGraph_String_Load(GRAPHICS_CHANNEL_BLEND, UserSetting->SetupPref.LanguageID, AppLibGraph_GetChannelData(GRAPHICS_CHANNEL_BLEND));
#endif
    return 0;
}
#endif

#ifdef CONFIG_ECL_GUI	
static MENU_TAB_s* menu_adas_cali_tab(void)
{
    return &menu_adas_cali;
}

static MENU_ITEM_s* menu_adas_cali_item(UINT32 itemId)
{
    return menu_adas_cali_item_tbl[itemId];
}
static MENU_TAB_s* menu_adas_func_tab(void)
{
    return &menu_adas_func;
}

static MENU_ITEM_s* menu_adas_func_item(UINT32 itemId)
{
    return menu_adas_func_item_tbl[itemId];
}
static MENU_TAB_s* menu_adas_alarm_dis_tab(void)
{
    return &menu_adas_alarm_dis;
}

static MENU_ITEM_s* menu_adas_alarm_dis_item(UINT32 itemId)
{
    return menu_adas_alarm_item_tbl[itemId];
}
static MENU_TAB_s* menu_recorder_set_tab(void)
{
    return &menu_recorder_set;
}

static MENU_ITEM_s* menu_recorder_set_item(UINT32 itemId)
{
   return menu_recorder_set_item_tbl[itemId];
}
static MENU_SEL_s* menu_recorder_set_get_sel(UINT32 itemId, UINT32 selId)
{
    return &menu_setup_item_sel_tbls[itemId][selId];
}

static MENU_TAB_s* menu_version_tab(void)
{
    return &menu_version;
}

static MENU_ITEM_s* menu_version_item(UINT32 itemId)
{
    return menu_version_item_tbl[itemId];
}
static MENU_TAB_s* menu_default_tab(void)
{
    return &menu_sys_default;
}

static MENU_ITEM_s* menu_default_item(UINT32 itemId)
{
    return menu_default_item_tbl[itemId];
}

#endif
// control

static MENU_TAB_s* menu_setup_get_tab(void)
{
    return &menu_setup;
}

static MENU_ITEM_s* menu_setup_get_item(UINT32 itemId)
{
    return menu_setup_item_tbl[itemId];
}

static MENU_SEL_s* menu_setup_get_sel(UINT32 itemId, UINT32 selId)
{
    return &menu_setup_item_sel_tbls[itemId][selId];
}

static int menu_setup_set_sel_table(UINT32 itemId, MENU_SEL_s *selTbl)
{
    menu_setup_item_sel_tbls[itemId] = selTbl;
    APP_REMOVEFLAGS(menu_setup_item_tbl[itemId]->Flags, MENU_ITEM_FLAGS_INIT);
    return 0;
}

static int menu_setup_lock_tab(void)
{
    APP_ADDFLAGS(menu_setup.Flags, MENU_TAB_FLAGS_LOCKED);
    return 0;
}

static int menu_setup_unlock_tab(void)
{
    APP_REMOVEFLAGS(menu_setup.Flags, MENU_TAB_FLAGS_LOCKED);
    return 0;
}

static int menu_setup_enable_item(UINT32 itemId)
{
    if (!APP_CHECKFLAGS(menu_setup_item_tbl[itemId]->Flags, MENU_ITEM_FLAGS_ENABLE)) {
        APP_ADDFLAGS(menu_setup_item_tbl[itemId]->Flags, MENU_ITEM_FLAGS_ENABLE);
        APP_REMOVEFLAGS(menu_setup.Flags, MENU_TAB_FLAGS_INIT);
    }
    return 0;
}

static int menu_setup_disable_item(UINT32 itemId)
{
    if (APP_CHECKFLAGS(menu_setup_item_tbl[itemId]->Flags, MENU_ITEM_FLAGS_ENABLE)) {
        APP_REMOVEFLAGS(menu_setup_item_tbl[itemId]->Flags, MENU_ITEM_FLAGS_ENABLE);
        APP_REMOVEFLAGS(menu_setup.Flags, MENU_TAB_FLAGS_INIT);
    }
    return 0;
}

static int menu_setup_lock_item(UINT32 itemId)
{
    APP_ADDFLAGS(menu_setup_item_tbl[itemId]->Flags, MENU_ITEM_FLAGS_LOCKED);
    return 0;
}

static int menu_setup_unlock_item(UINT32 itemId)
{
    APP_REMOVEFLAGS(menu_setup_item_tbl[itemId]->Flags, MENU_ITEM_FLAGS_LOCKED);
    return 0;
}

static int menu_setup_enable_sel(UINT32 itemId, UINT32 selId)
{
    if (!APP_CHECKFLAGS((menu_setup_item_sel_tbls[itemId]+selId)->Flags, MENU_SEL_FLAGS_ENABLE)) {
        APP_ADDFLAGS((menu_setup_item_sel_tbls[itemId]+selId)->Flags, MENU_SEL_FLAGS_ENABLE);
        APP_REMOVEFLAGS(menu_setup_item_tbl[itemId]->Flags, MENU_ITEM_FLAGS_INIT);
    }
    return 0;
}

static int menu_setup_disable_sel(UINT32 itemId, UINT32 selId)
{
    if (APP_CHECKFLAGS((menu_setup_item_sel_tbls[itemId]+selId)->Flags, MENU_SEL_FLAGS_ENABLE)) {
        APP_REMOVEFLAGS((menu_setup_item_sel_tbls[itemId]+selId)->Flags, MENU_SEL_FLAGS_ENABLE);
        APP_REMOVEFLAGS(menu_setup_item_tbl[itemId]->Flags, MENU_ITEM_FLAGS_INIT);
    }
    return 0;
}

static int menu_setup_lock_sel(UINT32 itemId, UINT32 selId)
{
    APP_ADDFLAGS((menu_setup_item_sel_tbls[itemId]+selId)->Flags, MENU_SEL_FLAGS_LOCKED);
    return 0;
}

static int menu_setup_unlock_sel(UINT32 itemId, UINT32 selId)
{
    APP_REMOVEFLAGS((menu_setup_item_sel_tbls[itemId]+selId)->Flags, MENU_SEL_FLAGS_LOCKED);
    return 0;
}
