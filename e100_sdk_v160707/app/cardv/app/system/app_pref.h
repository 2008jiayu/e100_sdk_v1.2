 /**
  * @file app/connected/app/system/app_pref.h
  *
  * Header of Application Preference
  *
  * History:
  *    2013/08/16 - [Martin Lai] created file
  *    2013/12/27 - [Hsunying Huang] modified
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

#ifndef APP_SYS_USR_PREF_H_
#define APP_SYS_USR_PREF_H_

#include <framework/appdefines.h>

__BEGIN_C_PROTO__

/*************************************************************************
 * User preference enums
 ************************************************************************/
typedef enum _APP_PREF_SYSTEM_MODE_e_ {
    APP_MODE_NONE = 0,
    APP_MODE_ENC,
    APP_MODE_DEC
} APP_PREF_SYSTEM_MODE_e;

typedef enum _APP_PREF_SELF_TIMER_e_ {
    SELF_TIMER_OFF = 0,
    SELF_TIMER_3S  = 3,
    SELF_TIMER_5S  = 5,
    SELF_TIMER_10S = 10
} APP_PREF_SELF_TIMER_e;

typedef enum _APP_PREF_STREAMING_STATUS_e_ {
    STREAMING_OFF = 0,
    STREAMING_ON
} APP_PREF_STREAMING_STATUS_e;

typedef enum _APP_PREF_STREAM_TYPE_e_ {
    STREAM_TYPE_OFF = 0,
    STREAM_TYPE_RTSP,
    STREAM_TYPE_MJPEG,
    STREAM_TYPE_UVC
} APP_PREF_STREAM_TYPE_e;

typedef enum _APP_PREF_DZOOM_e_ {
    DZOOM_OFF = 0,
    DZOOM_4X,
    DZOOM_16X,
    DZOOM_120X
} APP_PREF_DZOOM_e;

typedef enum _APP_PREF_TIME_STAMP_e_ {
    STAMP_OFF = 0,
    STAMP_DATE,
    STAMP_TIME,
    STAMP_DATE_TIME
} APP_PREF_TIME_STAMP_e;

#ifdef CONFIG_APP_ARD
typedef enum _APP_PREF_CARDV_STAMP_e_ {
    CARDV_STAMP_OFF         =   0,
    CARDV_STAMP_DATE        =   1,
    CARDV_STAMP_TIME        =   2,
    CARDV_STAMP_DRIVER_ID   =   4,
    CARDV_STAMP_AREA        =   8,
} APP_PREF_CARDV_STAMP_e;
#endif

typedef enum _APP_PREF_PHOTO_TIME_LAPSE_e_ {
    PHOTO_TIME_LAPSE_OFF = 0,
    PHOTO_TIME_LAPSE_500MS = 1,
    PHOTO_TIME_LAPSE_1S = 2,
    PHOTO_TIME_LAPSE_5S = 10
} APP_PREF_PHOTO_TIME_LAPSE_e;

typedef enum _APP_PREF_VIDOE_PLAY_OPT_e_ {
    PB_OPT_VIDEO_PLAY_ONE = 0,
    PB_OPT_VIDEO_PLAY_ALL,
    PB_OPT_VIDEO_REPEAT_ONE,
    PB_OPT_VIDEO_REPEAT_ALL
} APP_PREF_VIDOE_PLAY_OPT_e;

typedef enum _APP_PREF_PHOTO_PLAY_OPT_e_ {
    PB_OPT_PHOTO_REPEAT_OFF,
    PB_OPT_PHOTO_REPEAT_ON
} APP_PREF_PHOTO_PLAY_OPT_e;

typedef enum _APP_PREF_PHOTO_SLIDESHOW_EFFECT_e_ {
    SLIDESHOW_EFFECT_FADING = 0,
    SLIDESHOW_EFFECT_FLYING
} APP_PREF_PHOTO_SLIDESHOW_EFFECT_e;

typedef enum _APP_PREF_SOUND_PLAY_OPT_e_ {
    PB_OPT_SOUND_PLAY_ONE = 0,
    PB_OPT_SOUND_PLAY_ALL,
    PB_OPT_SOUND_REPEAT_ONE,
    PB_OPT_SOUND_REPEAT_ALL
} APP_PREF_SOUND_PLAY_OPT_e;

typedef enum _APP_PREF_SOUND_BACKGROUND_TYPE_e_ {
    PB_OPT_SOUND_BACKGROUND_LOGO = 0,
    PB_OPT_SOUND_BACKGROUND_PHOTO
} APP_PREF_SOUND_BACKGROUND_TYPE_e;

typedef enum _APP_PREF_AUTO_POWEROFF_e_ {
    AUTO_POWEROFF_OFF = 0,
#ifdef CONFIG_APP_ARD
    AUTO_POWEROFF_TIME_1M = 60,
#endif
    AUTO_POWEROFF_TIME_3M = 180,
    AUTO_POWEROFF_TIME_5M = 300
} APP_PREF_AUTO_POWEROFF_e;

typedef enum _APP_PREF_POWER_SAVING_e_ {
    POWERSAVING_OFF = 0,
    POWERSAVING_ON
} APP_PREF_POWER_SAVING_e;

typedef enum _APP_PREF_HDMI_SUPPORT_e_ {
    HDMI_SUPPORT_OFF = 0,
    HDMI_SUPPORT_ON
} APP_PREF_HDMI_SUPPORT_e;

typedef enum _APP_PREF_USB_MODE_e_ {
    USB_MODE_MSC = 0,
    USB_MODE_AMAGE
} APP_PREF_USB_MODE_e;

typedef enum _APP_PREF_WIFI_STATUS_e_ {
    WIFI_OFF = 0,
    WIFI_ON,
    WIFI_AP
} APP_PREF_WIFI_STATUS_e;

typedef enum _APP_PREF_RET_STATUS_e_ {
    PREF_RET_STATUS_LOAD_NORMAL = 0,
    PREF_RET_STATUS_LOAD_RESET,
} APP_PREF_RET_STATUS_e;

typedef enum _APP_PREF_ADAS_STATUS_e_ {
    ADAS_OFF = 0,
    ADAS_ON
} APP_PREF_ADAS_STATUS_e;

typedef enum _APP_PREF_ADAS_ORBIT_WARNING_STATUS_e_ {
    ADAS_ORBIT_WARNING_ON = 0,
    ADAS_ORBIT_WARNING_OFF
} APP_PREF_ADAS_ORBIT_WARNING_STATUS_e;

typedef enum _APP_PREF_ADAS_BEFORE_ALARM_STATUS_e_ {
    ADAS_BEFORE_ALARM_OFF = 0,
    ADAS_BEFORE_ALARM_NEAR,
    ADAS_BEFORE_ALARM_MIDDLE,
    ADAS_BEFORE_ALARM_FAR    
} APP_PREF_ADAS_BEFORE_ALARM_STATUS_e;

#ifdef CONFIG_APP_ARD
typedef enum _APP_PREF_MANUAL_CALI_e_ {
    MANUAL_CALI_BLC = 0,
    MANUAL_CALI_BAD_PIXEL,
    MANUAL_CALI_CHROMA_ABERRATION,
    MANUAL_CALI_WARP,
    MANUAL_CALI_VIGNETTE,
    MANUAL_CALI_WB_GOLDEN_SAMPLE,
    MANUAL_CALI_WBH,
    MANUAL_CALI_WBL,
} APP_PREF_MANUAL_CALI_e;

typedef enum _APP_PREF_TEST_MODE_e_ {
    TEST_MODE_OFF = 0,
    TEST_MODE_AUTO_POWER_OFF = 0xAA,
    TEST_MODE_AUTO_REBOOT = 0x6B,
    TEST_MODE_LOOP_REC = 0xA6,
} APP_PREF_TEST_MODE_e;

typedef enum _APP_PREF_RECORD_MODE_e_ {
    RECORD_MODE_AUTO = 0,
    RECORD_MODE_MANUAL = 1
} APP_PREF_RECORD_MODE_e;

typedef enum _APP_PREF_MOTION_DETECT_e_ {
    MOTION_DETECT_OFF = 0,
    MOTION_DETECT_ON
} APP_PREF_MOTION_DETECT_e;

typedef enum _APP_PREF_GSENSOR_SETTING_e_ {
    GSENSOR_SENSITIVITY_OFF = 0,
    GSENSOR_SENSITIVITY_HIGH = 1,
    GSENSOR_SENSITIVITY_MEDIUM = 2,
    GSENSOR_SENSITIVITY_LOW = 3
} APP_PREF_GSENSOR_SETTING_e;

typedef enum _APP_PREF_VIDEO_SPLIT_REC_TIME_e_ {
    VIDEO_SPLIT_REC_1_MIN = 0,
    //VIDEO_SPLIT_REC_2_MIN,
    VIDEO_SPLIT_REC_3_MIN,
    VIDEO_SPLIT_REC_5_MIN,
    VIDEO_SPLIT_REC_OFF,
} APP_PREF_VIDEO_SPLIT_REC_TIME_e;

typedef enum _APP_PREF_VIDEO_STAMP_e_ {
    STAMP_NOT_CHOOSE = 0,
    STAMP_CHOOSE,
} APP_PREF_VIDEO_STAMP_e;

typedef enum _APP_PREF_MIC_MUTE_e_ {
    MIC_MUTE_OFF = 0,
    MIC_MUTE_ON
} APP_PREF_MIC_MUTE_e;

typedef enum _APP_PREF_BACKLIGHTOFF_TIME_e_ {
    BACKLIGHTOFF_TIME_OFF = 0,
    BACKLIGHTOFF_TIME_1_MIN = 60,
    BACKLIGHTOFF_TIME_3_MIN = 180,
    BACKLIGHTOFF_TIME_5_MIN = 300,
} APP_PREF_BACKLIGHTOFF_TIME_e;

typedef enum _APP_PREF_BEEP_SOUND_e_ {
    BEEP_SOUND_ON = 0,
    BEEP_SOUND_OFF,
} APP_PREF_BEEP_SOUND_e;
#endif

#ifdef CONFIG_APP_ARD
typedef enum _APP_PREF_ANTI_FLICK_TYPE_e_ {
    ANTI_FLICKER_AUTO   = 0,
    ANTI_FLICKER_60HZ   = 1,
    ANTI_FLICKER_50HZ   = 2,
    ANTI_FLICKER_NO_60HZ = 10,
    ANTI_FLICKER_NO_50HZ = 20
} APP_PREF_ANTI_FLICKER_TYPE_e;
#endif

#ifdef CONFIG_APP_ARD
typedef enum _APP_PREF_DELAY_POWEROFF_e_ {
    DELAY_POWEROFF_OFF = 0,
    DELAY_POWEROFF_3S = 3,
    DELAY_POWEROFF_5S = 5,
    DELAY_POWEROFF_10S = 10,
    DELAY_POWEROFF_15S = 15,
} APP_PREF_DELAY_POWEROFF_e;
#endif

#ifdef CONFIG_APP_ARD
typedef enum _APP_PREF_LANGUAGE_SEL_ID_e_ {
    LANGUAGE_ENGLISH = 0,
    LANGUAGE_CHINESE_SIMPLIFIED,
    LANGUAGE_SEL_NUM
} APP_PREF_LANGUAGE_SEL_ID_e;
#endif

/*************************************************************************
 * User preference structures
 ************************************************************************/
typedef struct _APP_PREF_USER_SYSTEM_s_ {
    UINT16 SystemVersion;
#define GEN_VER(m,n,p)      ((((m) & 0xF) << 12) | (((n) & 0x3F) << 6) | ((p) & 0x3F))
#define GET_VER_MAJOR(ver)  (((ver) >> 12) & 0xF)
#define GET_VER_MINOR(ver)  (((ver) >> 6) & 0x3F)
#define GET_VER_PATCH(ver)  ((ver) & 0x3F)
    APP_PREF_SYSTEM_MODE_e SystemMode:8;
    UINT8 Reserved1;
} APP_PREF_USER_SYSTEM_s;

typedef struct _APP_PREF_USER_VIDEO_s_ {
    UINT8 SensorVideoRes;
    UINT8 SensorVideoResSecond;
    UINT8 YUVVideoRes;
    UINT8 VideoQuality;
    APP_PREF_SELF_TIMER_e VideoSelftimer:8;
    UINT8 PreRecord;
    UINT8 TimeLapse;
    UINT8 DualStreams;
    APP_PREF_STREAMING_STATUS_e Streaming:8;
    APP_PREF_STREAM_TYPE_e StreamType:8;
    APP_PREF_DZOOM_e VideoDZoom:8;
    APP_PREF_TIME_STAMP_e VideoDateTimeStamp:8;
    UINT8 StillPriorityEN;
    UINT8 UnsavingData;
#ifdef CONFIG_APP_ARD
    UINT8 MicMute;
    APP_PREF_VIDEO_SPLIT_REC_TIME_e video_split_rec_time:8;
    UINT8 parkingmode_sensitivity:8;
    UINT8 UnsavingEvent;
	UINT8 Adas_cel_set;
    //UINT8 Reserved2;
    UINT8 Reserved3;
	
#else
    UINT8 Reserved1;
    UINT8 Reserved2;
#endif
} APP_PREF_USER_VIDEO_s;

typedef struct _APP_PREF_USER_PHOTO_s_ {
    UINT8 PhotoMultiCap:3;
    UINT8 PhotoCapMode:3;
    UINT8 PhotoFastAf:2;
    UINT8 PhotoSize;
    UINT8 PhotoQuality;
    APP_PREF_SELF_TIMER_e PhotoSelftimer:8;
    UINT32 QuickviewDelay;
    APP_PREF_DZOOM_e PhotoDZoom:8;
    APP_PREF_TIME_STAMP_e PhotoTimeStamp:8;
    UINT16 TimeLapse;
} APP_PREF_USER_PHOTO_s;

typedef struct _APP_PREF_USER_IMAGE_s_ {
    UINT8 Flicker;
    UINT8 SlowShutter;
    UINT8 SceneMode;
    UINT8 WhiteBalance;
    UINT32 WbRedGain;
    UINT32 WbGreenGain;
    UINT32 WbBlueGain;
    UINT8 DigitalEffect;
    UINT8 Contrast;
    UINT8 Sharpness;
    UINT8 MeterMode;
    UINT8 ISO;
    UINT8 AFSetting;
    UINT8 ISSetting;
    UINT8 EVBiasIndex;
    UINT8 BLC;
    UINT8 Reserved;
    UINT8 Reserved1;
    UINT8 Reserved2;
    UINT32 Reserved3;
} APP_PREF_USER_IMAGE_s;

typedef struct _APP_PREF_USER_AUDIO_s_ {
    UINT8 AudioVolume;
    UINT8 Reserved;
    UINT8 Reserved1;
    UINT8 Reserved2;
} APP_PREF_USER_AUDIO_s;

typedef struct _APP_PREF_USER_PLAYBACK_s_ {
    UINT8 FileProtect;
    APP_PREF_VIDOE_PLAY_OPT_e VideoPlayOpt:8;
    APP_PREF_PHOTO_PLAY_OPT_e PhotoPlayOpt:8;
    UINT8 SlideshowMode;
    APP_PREF_PHOTO_SLIDESHOW_EFFECT_e SlideshowEffectType:8;
    UINT8 RealMovieEdit;
    UINT8 DeleteFile;
    UINT8 PostProc;
    UINT8 PostProcSave;
    UINT8 UnsavingData;
    UINT8 EdtrFunc;
    APP_PREF_SOUND_PLAY_OPT_e SoundPlayOpt:8;
    APP_PREF_SOUND_BACKGROUND_TYPE_e BackgroundSoundType:8;
#ifdef CONFIG_APP_ARD
    struct _DCIM_PLAY_OPT_S_ {
        APP_PREF_VIDOE_PLAY_OPT_e VideoPlayOpt:8;
        APP_PREF_PHOTO_PLAY_OPT_e PhotoPlayOpt:8;
        APP_PREF_SOUND_PLAY_OPT_e SoundPlayOpt:8;
    } DcimPlayOpt;
#endif
    UINT32 Reserved1;
} APP_PREF_USER_PLAYBACK_s;

typedef struct _APP_PREF_USER_SETUP_s_ {
    UINT8 VinSystem;
    UINT8 VoutSystem;
    UINT8 DMFMode;
    APP_PREF_HDMI_SUPPORT_e EnableHDMI:8;
    UINT32 DmfPhotoLastIdx;    /**< last recorded photo object id*/
    UINT32 DmfSoundLastIdx;    /**< last recorded sound object id*/
    UINT32 DmfVideoLastIdx;    /**< last recorded video object id*/
    UINT32 DmfMixLastIdx;    /**< last recorded dcim object id */
    UINT8 LcdBrightness;           /** Default: 65 = 0x41 */
    UINT8 LcdContrast;             /** Default: 65 = 0x41 */
    UINT8 LcdColorBalance;         /** Default: 125 = 0x7D */
    APP_PREF_USB_MODE_e USBMode:8;
#ifdef CONFIG_APP_ARD
    APP_PREF_AUTO_POWEROFF_e AutoPoweroff:16;
#else
    APP_PREF_AUTO_POWEROFF_e AutoPoweroff:8;
#endif
    APP_PREF_POWER_SAVING_e PowerSaving:8;
    UINT8 InputDimension;
    UINT8 OutputDimension;
    APP_PREF_WIFI_STATUS_e Wifi:8;
#ifdef CONFIG_APP_ARD
    UINT8 SensorNearFar;
    UINT8 SensorSwitch;
    APP_PREF_LANGUAGE_SEL_ID_e LanguageID:8;
    UINT8 RecordMode;
    APP_PREF_TEST_MODE_e test_mode;
    APP_PREF_BEEP_SOUND_e beep_sound:8;
    APP_PREF_BACKLIGHTOFF_TIME_e backlightoff_time;
    UINT16 driver_id[10];
    APP_PREF_ANTI_FLICKER_TYPE_e  anti_flicker_type;
    APP_PREF_DELAY_POWEROFF_e DelayPowerTime:8;
    UINT8 ldws_mode_onoff;
    UINT8 fcws_mode_onoff;
    UINT8 hmws_mode_onoff;
    UINT8 fcmr_mode_onoff;	
    UINT8 adas_alarm_dis;
    UINT8 adas_auto_cal_onoff;
    float radar_cal_offset;
#else
    UINT8 Reserved1;
#endif
    UINT16 Reserved2;
} APP_PREF_USER_SETUP_s;

typedef struct _APP_PREF_USER_VA_s_ {
    APP_PREF_ADAS_STATUS_e AdasDetection:8;
    UINT8 AdasCalibSky;     /** Default: 55% = 0x37 */
    UINT8 AdasCalibHood;    /** Default: 85% = 0x55 */
    APP_PREF_ADAS_ORBIT_WARNING_STATUS_e AdasOrbitWarning:8;
    APP_PREF_ADAS_BEFORE_ALARM_STATUS_e AdasBeforeAlarm:8;
} APP_PREF_USER_VA_s;

#ifdef CONFIG_APP_ARD
typedef struct _APP_PREF_USER_FACTORY_s_ {
    APP_PREF_MANUAL_CALI_e ManualCali:8;
    UINT8 GsensorTest;
    UINT8 FourGTest;
    UINT8 GpsTest;
    UINT32 Reserved1;
} APP_PREF_USER_FACTORY_s;

typedef struct _APP_PREF_USER_MOTION_DETECT_s_ {
    APP_PREF_MOTION_DETECT_e MotionDetect:8;
    UINT8 Reserved1;
    UINT8 Reserved2;
    UINT8 Reserved3;
} APP_PREF_USER_MOTION_DETECT_s;

typedef struct _APP_PREF_USER_GSENSOR_SETTING_s_ {
    APP_PREF_GSENSOR_SETTING_e Gsensor_sensitivity:8;
    UINT8 Reserved1;
    UINT8 Reserved2;
    UINT8 Reserved3;
} APP_PREF_USER_GSENSOR_SETTING_s;

typedef struct _APP_PREF_USER_VIDEO_STAMP_s_ {
    APP_PREF_VIDEO_STAMP_e StampDriverId:8;
    APP_PREF_VIDEO_STAMP_e StampDate:8;
    APP_PREF_VIDEO_STAMP_e StampTime:8;
    UINT8 Reserved1;
} APP_PREF_USER_VIDEO_STAMP_s;
#endif

typedef struct _APP_PREF_USER_s_ {
    /** System */
    APP_PREF_USER_SYSTEM_s SystemPref;
    /** Video */
    APP_PREF_USER_VIDEO_s VideoPref;
    /** Photo */
    APP_PREF_USER_PHOTO_s PhotoPref;
    /** Image */
    APP_PREF_USER_IMAGE_s ImagePref;
    /** Audio*/
    APP_PREF_USER_AUDIO_s AudioPref;
    /** Playback */
    APP_PREF_USER_PLAYBACK_s PlaybackPref;
    /** VA */
    APP_PREF_USER_VA_s VAPref;
#ifdef CONFIG_APP_ARD
    /** Factory */
    APP_PREF_USER_FACTORY_s FactoryPref;
    /** Motion detect option */
    APP_PREF_USER_MOTION_DETECT_s MotionDetectPref;
    /** Gsensor sentivity**/
    APP_PREF_USER_GSENSOR_SETTING_s GSensorSentivityPref;
    /** Video Stamp Setting**/
    APP_PREF_USER_VIDEO_STAMP_s VideoStampPref;

    /** Setup */
    APP_PREF_USER_SETUP_s SetupPref;
#endif
} APP_PREF_USER_s;

extern APP_PREF_USER_s *UserSetting;

/*************************************************************************
 * User preference API
 ************************************************************************/
extern INT8 AppPref_InitPref(void);
extern APP_PREF_RET_STATUS_e AppPref_Load(void);
extern INT8 AppPref_Save(void);
#ifdef CONFIG_APP_ARD
extern UINT32 AppPref_GetLanguageID(VOID);
#endif

__END_C_PROTO__

#endif /* APP_SYS_USR_PREF_H_ */
