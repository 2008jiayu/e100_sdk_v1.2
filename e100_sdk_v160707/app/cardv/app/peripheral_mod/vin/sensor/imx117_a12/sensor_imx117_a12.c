 /**
  * @file src/app/peripheral_mod/vin/sensor/imx117_a12/sensor_imx117_a12.c
  *
  *  Implementation of SONY IMX117 related settings.
  *
  * History:
  *    2013/09/18 - [Martin Lai] created file
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

#include <system/ApplibSys_Sensor.h>
#include <wchar.h>
#include <AmbaSensor.h>
#include <AmbaSensor_IMX117.h>

extern AMBA_SENSOR_OBJ_s AmbaSensor_IMX117Obj;

/** photo quality config table */
#define PHOTO_QUALITY_TABLE_SIZE    (4)
static int sensor_imx117_a12_photo_quality_table[PHOTO_QUALITY_TABLE_SIZE] = {
    95, 90, 80, 0xFF
};

/** photo encode window config table */
#define PHOTO_CONFIG_NORMAL_TABLE_SIZE        (2)
static int sensor_imx117_a12_capture_mode_normal_ar_table[PHOTO_CONFIG_NORMAL_TABLE_SIZE] = {
    VAR_4x3,
    VAR_16x9
};

static UINT32 sensor_imx117_a12_shutter_mode_normal_table[PHOTO_CONFIG_NORMAL_TABLE_SIZE] = {
    SENSOR_DEF_SHUTTER,
    SENSOR_DEF_SHUTTER
};

static APPLIB_SENSOR_STILLCAP_CONFIG_s sensor_imx117_a12_pjpeg_config_normal_table[PHOTO_CONFIG_NORMAL_TABLE_SIZE] = {
    {
        1,
        PHOTO_THUMB_W, PHOTO_THUMB_H_4x3, PHOTO_THUMB_W, PHOTO_THUMB_H_4x3,
        PHOTO_SCRN_W, PHOTO_SCRN_H_4x3, PHOTO_SCRN_W, PHOTO_SCRN_H_4x3,
        4000, 3000,
        PHOTO_QUALITY_DEFAULT, PHOTO_QUALITY_DEFAULT, PHOTO_QUALITY_DEFAULT,
        REC_JPEG_DEFAULT},
    {
        1,
        PHOTO_THUMB_W, PHOTO_THUMB_H_4x3, PHOTO_THUMB_W, PHOTO_THUMB_H_16x9,
        PHOTO_SCRN_W, PHOTO_SCRN_H_16x9, PHOTO_SCRN_W, PHOTO_SCRN_H_16x9,
        3840, 2160,
        PHOTO_QUALITY_DEFAULT, PHOTO_QUALITY_DEFAULT, PHOTO_QUALITY_DEFAULT,
        REC_JPEG_DEFAULT}
};

static APPLIB_SENSOR_STILLPREV_CONFIG_s sensor_imx117_a12_pjpeg_config_normal_liveview_table[2][PHOTO_CONFIG_NORMAL_TABLE_SIZE] = {
    {{ 1312, 984, 960, 720, 30000, 1001, VAR_4x3}, {1312, 738, 960, 540, 30000, 1001, VAR_16x9}},
    {{ 1312, 984, 960, 720, 25000, 1000, VAR_4x3}, {1312, 720, 960, 540, 25000, 1000, VAR_16x9}}
};

static int sensor_imx117_a12_pjpeg_mode_id_normal_liveview_table[2][PHOTO_CONFIG_NORMAL_TABLE_SIZE] = {
    {AMBA_SENSOR_IMX117_TYPE_2_3_MODE_3_30P, AMBA_SENSOR_IMX117_TYPE_2_3_MODE_3_30P},
    {AMBA_SENSOR_IMX117_TYPE_2_3_MODE_0_25P, AMBA_SENSOR_IMX117_TYPE_2_3_MODE_2A_25P}
};

static APPLIB_SENSOR_STILLCAP_MODE_CONFIG_s sensor_imx117_a12_still_capture_mode_table[PHOTO_CONFIG_NORMAL_TABLE_SIZE] = {
    {AMBA_SENSOR_IMX117_TYPE_2_3_MODE_0_30P, 3840, 2880},
    {AMBA_SENSOR_IMX117_TYPE_2_5_MODE_0_30P, 4096, 2160}
};

#define PHOTO_CONFIG_BURST_TABLE_SIZE    (1)
static int sensor_imx117_a12_capture_mode_burst_ar_table[PHOTO_CONFIG_BURST_TABLE_SIZE] = {
    VAR_4x3
};

static UINT32 sensor_imx117_a12_shutter_mode_burst_table[PHOTO_CONFIG_BURST_TABLE_SIZE] = {
    SENSOR_DEF_SHUTTER
};

static APPLIB_SENSOR_STILLCAP_CONFIG_s sensor_imx117_a12_pjpeg_config_burst_table[PHOTO_CONFIG_BURST_TABLE_SIZE] = {
    {
        8,
        PHOTO_THUMB_W, PHOTO_THUMB_H_4x3, PHOTO_THUMB_W, PHOTO_THUMB_H_4x3,
        PHOTO_SCRN_W, PHOTO_SCRN_H_4x3, PHOTO_SCRN_W, PHOTO_SCRN_H_4x3,
        4000, 3000,
        PHOTO_QUALITY_DEFAULT, PHOTO_QUALITY_DEFAULT, PHOTO_QUALITY_DEFAULT,
        REC_JPEG_DEFAULT}
};

static APPLIB_SENSOR_STILLPREV_CONFIG_s sensor_imx117_a12_pjpeg_config_burst_liveview_table[2][PHOTO_CONFIG_BURST_TABLE_SIZE] = {
    {{ 1312, 984, 960, 720, 30000, 1001, VAR_4x3}},
    {{ 1312, 984, 960, 720, 25000, 1000, VAR_4x3}}
};

static int sensor_imx117_a12_pjpeg_mode_id_burst_liveview_table[2][PHOTO_CONFIG_BURST_TABLE_SIZE] = {
    {AMBA_SENSOR_IMX117_TYPE_2_3_MODE_3_30P},
    {AMBA_SENSOR_IMX117_TYPE_2_3_MODE_0_30P}
};

static APPLIB_SENSOR_STILLCAP_MODE_CONFIG_s sensor_imx117_a12_still_capture_mode_burst_table[PHOTO_CONFIG_BURST_TABLE_SIZE] = {
    {AMBA_SENSOR_IMX117_TYPE_2_3_MODE_0_30P, 3840, 2880}
};
/** photo config string table */
static UINT16 sensor_imx117_a12_normal_photo_size_string_table[PHOTO_CONFIG_NORMAL_TABLE_SIZE][SENSOR_PHOTO_SIZE_STR_LEN] = {
    {'1','2','M',' ','(','4','0','0','0','x','3','0','0','0',' ','4',':','3',')','\0'},
    {'8','.','3','M',' ','(','3','8','4','0','x','2','1','6','0',' ','1','6',':','9',')','\0'}
};

static UINT16 sensor_imx117_a12_burst_photo_size_string_table[PHOTO_CONFIG_BURST_TABLE_SIZE][SENSOR_PHOTO_SIZE_STR_LEN] = {
    {'1','2','M',' ','(','4','0','0','0','x','3','0','0','0',' ','4',':','3',')','\0'}
};


#define VIDEO_RES_TABLE_SIZE    (6)
static int sensor_imx117_a12_video_res_table[VIDEO_RES_TABLE_SIZE] = {
    SENSOR_VIDEO_RES_WQHDP50,
    SENSOR_VIDEO_RES_WQHD_HALF,
    SENSOR_VIDEO_RES_TRUE_1080P_FULL,
    SENSOR_VIDEO_RES_TRUE_1080P_HALF,
    SENSOR_VIDEO_RES_FHD_HFR_P100_P100,
    SENSOR_VIDEO_RES_HD_HFR_P200_P200
};

static int sensor_video_res_table_size = VIDEO_RES_TABLE_SIZE;
static int *sensor_video_res_table = sensor_imx117_a12_video_res_table;


/**
 *  @brief Initialize the information of sensor
 *
 *  Initialize the information of sensor
 *
 *  @param [in] channel channel
 *
 *  @return >=0 success, <0 failure
 */
static int sensor_imx117_a12_init(AMBA_DSP_CHANNEL_ID_u channel)
{
    int ReturnValue = 0;

    return ReturnValue;
}

/**
 *  @brief Get the strings of photo size
 *
 *  Get the strings of photo size
 *
 *  @param [in] capMode Capture mode
 *  @param [in] pjpegConfigID Photo config id
 *
 *  @return >=0 success, <0 failure
 */
static UINT16* sensor_imx117_a12_get_photo_size_str(int capMode, int pjpegConfigID)
{
    switch (capMode) {
        case SENSOR_PHOTO_CAP_NORMAL:
            if (pjpegConfigID >= PHOTO_CONFIG_NORMAL_TABLE_SIZE) {
                return sensor_imx117_a12_normal_photo_size_string_table[0];
            } else {
                return sensor_imx117_a12_normal_photo_size_string_table[pjpegConfigID];
            }
            break;
        case SENSOR_PHOTO_CAP_BURST:
            if (pjpegConfigID >= PHOTO_CONFIG_BURST_TABLE_SIZE) {
                return sensor_imx117_a12_burst_photo_size_string_table[0];
            } else {
                return sensor_imx117_a12_burst_photo_size_string_table[pjpegConfigID];
            }
            break;
        default:
            AmbaPrintColor(RED,"Sensor imx117 doesn't support this capture mode. Cap mode is %d", capMode);
            break;
    }
}

/**
 *  @brief Get sensor mode of photo liveview mode.
 *
 *  Get sensor mode of photo liveview mode.
 *
 *  @param [in] capMode Capture mode
 *  @param [in] pjpegConfigID Photo config id
 *
 *  @return >=0 success, <0 failure
 */
static int sensor_imx117_a12_get_photo_liveview_mode_id(int capMode, int pjpegConfigID)
{
    int ReturnValue = 0;
    int VinSystem = AppLibSysVin_GetSystemType();
    switch (capMode) {
        case SENSOR_PHOTO_CAP_NORMAL:
            if (pjpegConfigID >= PHOTO_CONFIG_NORMAL_TABLE_SIZE) {
                ReturnValue = sensor_imx117_a12_pjpeg_mode_id_normal_liveview_table[VinSystem][0];
            } else {
                ReturnValue = sensor_imx117_a12_pjpeg_mode_id_normal_liveview_table[VinSystem][pjpegConfigID];
            }
            break;
        case SENSOR_PHOTO_CAP_BURST:
            if (pjpegConfigID >= PHOTO_CONFIG_BURST_TABLE_SIZE) {
                ReturnValue = sensor_imx117_a12_pjpeg_mode_id_burst_liveview_table[VinSystem][0];
            } else {
                ReturnValue = sensor_imx117_a12_pjpeg_mode_id_burst_liveview_table[VinSystem][pjpegConfigID];
            }
            break;
        default:
            ReturnValue = -1;
            AmbaPrintColor(RED,"Sensor imx117 doesn't support this capture mode. Cap mode is %d", capMode);
            break;
    }

    return ReturnValue;
}


/**
 *  @brief Get sensor mode of photo High frame mode.
 *
 *  Get sensor mode of photo High frame mode.
 *
 *  @param [in] capMode Capture mode
 *  @param [in] pjpegConfigID Photo config id
 *
 *  @return >=0 success, <0 failure
 */
static int sensor_imx117_a12_get_photo_hfr_mode_id(int capMode, int pjpegConfigID)
{
    int ReturnValue = -1;

    return ReturnValue;
}

/**
 *  @brief Get sensor mode of photo High frame preflash mode.
 *
 *  Get sensor mode of photo High frame mode.
 *
 *  @param [in] capMode Capture mode
 *  @param [in] pjpegConfigID Photo config id
 *
 *  @return >=0 success, <0 failure
 */
static int sensor_imx117_a12_get_photo_preflash_hfr_mode_id(int capMode, int pjpegConfigID)
{
    int ReturnValue = -1;

    return ReturnValue;
}

/**
 *  @brief Get sensor mode of photo normal capture mode.
 *
 *  Get sensor mode of photo normal capture mode.
 *
 *  @param [in] capMode Capture mode
 *  @param [in] pjpegConfigID Photo config id
 *
 *  @return >=0 success, <0 failure
 */
static APPLIB_SENSOR_STILLCAP_MODE_CONFIG_s* sensor_imx117_a12_get_still_capture_mode_config(int capMode, int pjpegConfigID)
{
    APPLIB_SENSOR_STILLCAP_MODE_CONFIG_s *ReturnValue = NULL;

    switch (capMode) {
        case SENSOR_PHOTO_CAP_NORMAL:
            if (pjpegConfigID >= PHOTO_CONFIG_NORMAL_TABLE_SIZE) {
                ReturnValue = &sensor_imx117_a12_still_capture_mode_table[0];
            } else {
                ReturnValue = &sensor_imx117_a12_still_capture_mode_table[pjpegConfigID];
            }
            break;
        case SENSOR_PHOTO_CAP_BURST:
            if (pjpegConfigID >= PHOTO_CONFIG_BURST_TABLE_SIZE) {
                ReturnValue = &sensor_imx117_a12_still_capture_mode_burst_table[0];
            } else {
                ReturnValue = &sensor_imx117_a12_still_capture_mode_burst_table[pjpegConfigID];
            }
            break;
        default:
            ReturnValue = NULL;
            AmbaPrintColor(RED,"Sensor imx117 doesn't support this capture mode. Cap mode is %d, pjpegConfigID is %d", capMode, pjpegConfigID);
            K_ASSERT(0);
            break;
    }

    return ReturnValue;
}

/**
 *  @brief Get sensor mode of photo OB mode.
 *
 *  Get sensor mode of photo normal capture mode.
 *
 *  @param [in] capMode Capture mode
 *  @param [in] pjpegConfigID Photo config id
 *
 *  @return >=0 success, <0 failure
 */
static int sensor_imx117_a12_get_still_capture_ob_mode_id(int capMode, int pjpegConfigID)
{
    int ReturnValue = -1;

    return ReturnValue;
}


/**
 *  @brief Get aspect ratio of photo mode.
 *
 *  Get aspect ratio of photo mode.
 *
 *  @param [in] capMode Capture mode
 *  @param [in] pjpegConfigID Photo config id
 *
 *  @return >=0 success, <0 failure
 */
static int sensor_imx117_a12_get_capture_mode_ar(int capMode, int pjpegConfigID)
{
    int ReturnValue = -1;
    switch (capMode) {
        case SENSOR_PHOTO_CAP_NORMAL:
        if (pjpegConfigID >= PHOTO_CONFIG_NORMAL_TABLE_SIZE) {
            ReturnValue = sensor_imx117_a12_capture_mode_normal_ar_table[0];
        } else {
            ReturnValue = sensor_imx117_a12_capture_mode_normal_ar_table[pjpegConfigID];
        }
        break;
    case SENSOR_PHOTO_CAP_BURST:
        if (pjpegConfigID >= PHOTO_CONFIG_BURST_TABLE_SIZE) {
            ReturnValue = sensor_imx117_a12_capture_mode_burst_ar_table[0];
        } else {
            ReturnValue = sensor_imx117_a12_capture_mode_burst_ar_table[pjpegConfigID];
        }
        break;
    default:
        ReturnValue = -1;
        AmbaPrintColor(RED,"Sensor imx117 doesn't support this capture mode. Cap mode is %d", capMode);
        break;
    }
    return ReturnValue;
}


/**
 *  @brief Get the preview window size
 *
 *  Get the preview window size
 *
 *  @param [in] capMode Capture mode
 *  @param [in] pjpegConfigID Photo config id
 *  @param [in] video_plane_dev video plane of device
 *  @param [out] width Width
 *  @param [out] height Height
 *
 *  @return >=0 success, <0 failure
 */
static int sensor_imx117_a12_get_preview_window(int capMode, int pjpegConfigID, int *width, int *height)
{
    int ReturnValue = 0;

    return ReturnValue;
}

/**
 *  @brief Get the quality setting of photo mode.
 *
 *  Get the quality setting of photo mode.
 *
 *  @param [in] qualityMode The quality mode
 *
 *  @return Quality setting
 */
static int sensor_imx117_a12_get_photo_quality_config(int qualityMode)
{
    return sensor_imx117_a12_photo_quality_table[qualityMode];
}

/**
 *  @brief Get the photo liveview config.
 *
 *  Get the photo liveview config.
 *
 *  @param [in] capMode Capture mode
 *  @param [in] pjpegConfigID Photo config id
 *
 *  @return >=0 success, <0 failure
 */
static APPLIB_SENSOR_STILLPREV_CONFIG_s* sensor_imx117_a12_get_photo_liveview_config(int capMode, int pjpegConfigID)
{
    APPLIB_SENSOR_STILLPREV_CONFIG_s *ReturnValue = NULL;
    int VinSystem = AppLibSysVin_GetSystemType();
    switch (capMode) {
        case SENSOR_PHOTO_CAP_NORMAL:
            if (pjpegConfigID >= PHOTO_CONFIG_NORMAL_TABLE_SIZE) {
                ReturnValue = &sensor_imx117_a12_pjpeg_config_normal_liveview_table[VinSystem][0];
            } else {
                ReturnValue = &sensor_imx117_a12_pjpeg_config_normal_liveview_table[VinSystem][pjpegConfigID];
            }
            break;
        case SENSOR_PHOTO_CAP_BURST:
            if (pjpegConfigID >= PHOTO_CONFIG_BURST_TABLE_SIZE) {
                ReturnValue = &sensor_imx117_a12_pjpeg_config_burst_liveview_table[VinSystem][0];
            } else {
                ReturnValue = &sensor_imx117_a12_pjpeg_config_burst_liveview_table[VinSystem][pjpegConfigID];
            }
            break;
        default:
            ReturnValue = NULL;
            AmbaPrintColor(RED,"Sensor imx117 doesn't support this capture mode. Cap mode is %d", capMode);
            break;
    }
    return ReturnValue;
}

/**
 *  @brief Get the photo jpge config
 *
 *  Get the photo jpge config
 *
 *  @param [in] capMode Capture mode
 *  @param [in] pjpegConfigID Photo config id
 *
 *  @return >=0 success, <0 failure
 */
static APPLIB_SENSOR_STILLCAP_CONFIG_s* sensor_imx117_a12_get_pjpeg_config(int capMode, int pjpegConfigID)
{
    APPLIB_SENSOR_STILLCAP_CONFIG_s *ReturnValue = NULL;
    switch (capMode) {
        case SENSOR_PHOTO_CAP_NORMAL:
            if (pjpegConfigID >= PHOTO_CONFIG_NORMAL_TABLE_SIZE) {
                ReturnValue = &sensor_imx117_a12_pjpeg_config_normal_table[0];
            } else {
                ReturnValue = &sensor_imx117_a12_pjpeg_config_normal_table[pjpegConfigID];
            }
            break;
        case SENSOR_PHOTO_CAP_BURST:
            if (pjpegConfigID >= PHOTO_CONFIG_BURST_TABLE_SIZE) {
                ReturnValue = &sensor_imx117_a12_pjpeg_config_burst_table[0];
            } else {
                ReturnValue = &sensor_imx117_a12_pjpeg_config_burst_table[pjpegConfigID];
            }
            break;
        default:
            ReturnValue = NULL;
            AmbaPrintColor(RED,"Sensor imx117 doesn't support this capture mode. Cap mode is %d", capMode);
            break;
    }
    return ReturnValue;
}


/**
 *  @brief Get the shutter mode under photo capture mode
 *
 *  Get the shutter mode under photo capture mode
 *
 *  @param [in] capMode Capture mode
 *  @param [in] pjpegConfigID Photo config id
 *
 *  @return >=0 success, <0 failure
 */
static UINT32 sensor_imx117_a12_get_shutter_mode(int capMode, int pjpegConfigID)
{
    UINT32 ReturnValue = 0;
    switch (capMode) {
        case SENSOR_PHOTO_CAP_NORMAL:
            if (pjpegConfigID >= PHOTO_CONFIG_NORMAL_TABLE_SIZE) {
                ReturnValue = sensor_imx117_a12_shutter_mode_normal_table[0];
            } else {
                ReturnValue = sensor_imx117_a12_shutter_mode_normal_table[pjpegConfigID];
            }
            break;
        case SENSOR_PHOTO_CAP_BURST:
            if (pjpegConfigID >= PHOTO_CONFIG_BURST_TABLE_SIZE) {
                ReturnValue = sensor_imx117_a12_shutter_mode_burst_table[0];
            } else {
                ReturnValue = sensor_imx117_a12_shutter_mode_burst_table[pjpegConfigID];
            }
            break;
        default:
            ReturnValue = -1;
            AmbaPrintColor(RED,"Sensor imx117 doesn't support this capture mode. Cap mode is %d", capMode);
            break;
    }
    return ReturnValue;
}


/**
 *  @brief Get maximum shutter time
 *
 *  Get maximum shutter time
 *
 *  @param [in] capMode Capture mode
 *  @param [in] pjpegConfigID Photo config id
 *
 *  @return >=0 success, <0 failure
 */
static int sensor_imx117_a12_get_max_shutter_time(int capMode, int pjpegConfigID)
{
    int ReturnValue = 0;

    return ReturnValue;
}


/**
 *  @brief Get the resolution id of video record mode that this sensor support
 *
 *  Get the resolution id of video record mode that this sensor support
 *
 *  @param [in] resRef The index that this sensor support
 *
 *  @return >=0 Resolution id, <0 failure
 */
static int sensor_imx117_a12_get_videoResID(int resRef)
{
    if ((resRef >= 0) || (resRef < sensor_video_res_table_size)) {
        return sensor_video_res_table[resRef];
    } else {
        AmbaPrint("[App] Sensor imx117 Invalid resolution reference");
        return -1;
    }
}

/**
 *  @brief Get the strings of video recording mode.
 *
 *  Get the strings of video recording mode.
 *
 *  @param [in] videoResID Resolution id
 *
 *  @return strings index
 */
static UINT16* sensor_imx117_a12_get_video_res_str(int videoResID)
{
    if (AppLibSysVin_GetSystemType() == VIN_SYS_PAL) {
        extern UINT16 sensor_video_res_string_table_pal[][SENSOR_VIDEO_RES_STR_LEN];
        return sensor_video_res_string_table_pal[videoResID];
    } else {
        extern UINT16 sensor_video_res_string_table_ntsc[][SENSOR_VIDEO_RES_STR_LEN];
        return sensor_video_res_string_table_ntsc[videoResID];
    }
}

/**
 *  @brief Get sensor mode of video recording mode
 *
 *  Get sensor mode of video recording mode
 *
 *  @param [in] vin_config Vin config
 *
 *  @return >=0 VinMode, <0 failure
 */
static int sensor_imx117_a12_get_vinMode(APPLIB_SENSOR_VIN_CONFIG_s *vin_config)
{
    int ReturnValue = -1;
    int VinSystem = AppLibSysVin_GetSystemType();

    switch (vin_config->ResID) {
    case SENSOR_VIDEO_RES_UHD_HALF:
    case SENSOR_VIDEO_RES_WQHD_HALF:
    case SENSOR_VIDEO_RES_TRUE_1080P_HALF:
        if (VinSystem == VIN_SYS_PAL) {
            ReturnValue = AMBA_SENSOR_IMX117_TYPE_2_5_MODE_0_25P;
        } else {
            ReturnValue = AMBA_SENSOR_IMX117_TYPE_2_5_MODE_0_30P;
        }
        break;
    case SENSOR_VIDEO_RES_WQHD_FULL:
    case SENSOR_VIDEO_RES_TRUE_1080P_FULL:
    case SENSOR_VIDEO_RES_HD_FULL:
        if (VinSystem == VIN_SYS_PAL) {
            ReturnValue = AMBA_SENSOR_IMX117_TYPE_2_5_MODE_1_50P;
        } else {
            ReturnValue = AMBA_SENSOR_IMX117_TYPE_2_5_MODE_1_60P;
        }
        break;
    case SENSOR_VIDEO_RES_1920_1440P60:
        if (VinSystem == VIN_SYS_PAL) {
           ReturnValue = AMBA_SENSOR_IMX117_TYPE_2_3_MODE_2A_50P;
        } else {
           ReturnValue = AMBA_SENSOR_IMX117_TYPE_2_3_MODE_2A_60P;
        }
        break;
    case SENSOR_VIDEO_RES_1920_1440P30:
    case SENSOR_VIDEO_RES_1440_1080P30:
        if (VinSystem == VIN_SYS_PAL) {
           ReturnValue = AMBA_SENSOR_IMX117_TYPE_2_3_MODE_0_25P;
        } else {
           ReturnValue = AMBA_SENSOR_IMX117_TYPE_2_3_MODE_0_30P;
        }
        break;
    case SENSOR_VIDEO_RES_FHD_HFR_P120_P100:
        if (VinSystem == VIN_SYS_PAL) {
            ReturnValue = AMBA_SENSOR_IMX117_TYPE_4_4_MODE_1_100P;
        } else {
            ReturnValue = AMBA_SENSOR_IMX117_TYPE_4_4_MODE_1_120P;
        }
        break;
    case SENSOR_VIDEO_RES_HD_HFR_P240_P200:
        if (VinSystem == VIN_SYS_PAL) {
            ReturnValue = AMBA_SENSOR_IMX117_TYPE_2_3_MODE_4A_200P;
        } else {
            ReturnValue = AMBA_SENSOR_IMX117_TYPE_2_3_MODE_4A_240P;
        }
        break;
    case SENSOR_VIDEO_RES_WQHDP50:
        if (VinSystem == VIN_SYS_PAL) {
            ReturnValue = AMBA_SENSOR_IMX117_TYPE_2_5_MODE_1_50P;
        } else {
            ReturnValue = AMBA_SENSOR_IMX117_TYPE_2_5_MODE_1_50P;
        }
        break;
    case SENSOR_VIDEO_RES_FHD_HFR_P100_P100:
        if (VinSystem == VIN_SYS_PAL) {
            ReturnValue = AMBA_SENSOR_IMX117_TYPE_2_3_MODE_10_100P;
        } else {
            ReturnValue = AMBA_SENSOR_IMX117_TYPE_2_3_MODE_10_100P;
        }
        break;
    case SENSOR_VIDEO_RES_HD_HFR_P200_P200:
        if (VinSystem == VIN_SYS_PAL) {
            ReturnValue = AMBA_SENSOR_IMX117_TYPE_2_3_MODE_4A_200P;
        } else {
            ReturnValue = AMBA_SENSOR_IMX117_TYPE_2_3_MODE_4A_200P;
        }
        break;
    case SENSOR_VIDEO_RES_PHOTO:
        ReturnValue = sensor_imx117_a12_get_photo_liveview_mode_id(vin_config->CapMode, vin_config->PjpegConfigID);
        break;
    default:
        AmbaPrintColor(RED,"[App] Sensor imx117 doesn't support this resolution %d ", vin_config->ResID);
        K_ASSERT(0);
        break;
    }

    AmbaPrintColor(BLUE, "Vin Mode = %d", ReturnValue);

    return ReturnValue;
}

/**
 *  @brief Get video config
 *
 *  Get video config
 *
 *  @param [in] videoResID Video resolution id
 *
 *  @return >=0 The address of config file, <0 failure
 */
static APPLIB_SENSOR_VIDEO_ENC_CONFIG_s* sensor_imx117_a12_get_video_config(int videoResID)
{
    if (AppLibSysVin_GetSystemType() == VIN_SYS_PAL) {
        extern APPLIB_SENSOR_VIDEO_ENC_CONFIG_s SensorVideoEncConfigTablePAL[];
        switch (videoResID) {
        case SENSOR_VIDEO_RES_UHD_HALF:
        case SENSOR_VIDEO_RES_WQHD_HALF:
        case SENSOR_VIDEO_RES_TRUE_1080P_FULL:
        case SENSOR_VIDEO_RES_HD_FULL:
            SensorVideoEncConfigTablePAL[videoResID].CaptureWidth = 3840;
            SensorVideoEncConfigTablePAL[videoResID].CaptureHeight = 2160;
            break;
        case SENSOR_VIDEO_RES_WQHD_FULL:
        case SENSOR_VIDEO_RES_WQHDP50:
            SensorVideoEncConfigTablePAL[videoResID].CaptureWidth = 3840;
            SensorVideoEncConfigTablePAL[videoResID].CaptureHeight = 2160;
            break;
        case SENSOR_VIDEO_RES_TRUE_1080P_HALF:
            SensorVideoEncConfigTablePAL[videoResID].CaptureWidth = 3840;
            SensorVideoEncConfigTablePAL[videoResID].CaptureHeight = 2160;
            break;
        case SENSOR_VIDEO_RES_1920_1440P60:
            SensorVideoEncConfigTablePAL[videoResID].CaptureWidth = 1920;
            SensorVideoEncConfigTablePAL[videoResID].CaptureHeight = 1440;
            break;
        case SENSOR_VIDEO_RES_1920_1440P30:
        case SENSOR_VIDEO_RES_1440_1080P30:
            SensorVideoEncConfigTablePAL[videoResID].CaptureWidth = 4000;
            SensorVideoEncConfigTablePAL[videoResID].CaptureHeight = 3000;
            break;
        case SENSOR_VIDEO_RES_FHD_HFR_P120_P100:
        case SENSOR_VIDEO_RES_FHD_HFR_P100_P100:
            SensorVideoEncConfigTablePAL[videoResID].CaptureWidth = 1920;
            SensorVideoEncConfigTablePAL[videoResID].CaptureHeight = 1080;
            break;
        case SENSOR_VIDEO_RES_HD_HFR_P240_P200:
        case SENSOR_VIDEO_RES_HD_HFR_P200_P200:
            SensorVideoEncConfigTablePAL[videoResID].CaptureWidth = 1920;
            SensorVideoEncConfigTablePAL[videoResID].CaptureHeight = 720;
            break;
        default:
            break;
        }
        return &SensorVideoEncConfigTablePAL[videoResID];
    } else {
        extern APPLIB_SENSOR_VIDEO_ENC_CONFIG_s SensorVideoEncConfigTableNTSC[];
        switch (videoResID) {
        case SENSOR_VIDEO_RES_UHD_HALF:
        case SENSOR_VIDEO_RES_WQHD_HALF:
        case SENSOR_VIDEO_RES_TRUE_1080P_FULL:
        case SENSOR_VIDEO_RES_HD_FULL:
            SensorVideoEncConfigTableNTSC[videoResID].CaptureWidth = 3840;
            SensorVideoEncConfigTableNTSC[videoResID].CaptureHeight = 2160;
            break;
        case SENSOR_VIDEO_RES_WQHD_FULL:
        case SENSOR_VIDEO_RES_WQHDP50:
            SensorVideoEncConfigTableNTSC[videoResID].CaptureWidth = 3840;
            SensorVideoEncConfigTableNTSC[videoResID].CaptureHeight = 2160;
            break;
        case SENSOR_VIDEO_RES_TRUE_1080P_HALF:
            SensorVideoEncConfigTableNTSC[videoResID].CaptureWidth = 3840;
            SensorVideoEncConfigTableNTSC[videoResID].CaptureHeight = 2160;
            break;
        case SENSOR_VIDEO_RES_1920_1440P60:
            SensorVideoEncConfigTableNTSC[videoResID].CaptureWidth = 1920;
            SensorVideoEncConfigTableNTSC[videoResID].CaptureHeight = 1440;
            break;
        case SENSOR_VIDEO_RES_1920_1440P30:
        case SENSOR_VIDEO_RES_1440_1080P30:
            SensorVideoEncConfigTableNTSC[videoResID].CaptureWidth = 4000;
            SensorVideoEncConfigTableNTSC[videoResID].CaptureHeight = 3000;
            break;
        case SENSOR_VIDEO_RES_FHD_HFR_P120_P100:
        case SENSOR_VIDEO_RES_FHD_HFR_P100_P100:
            SensorVideoEncConfigTableNTSC[videoResID].CaptureWidth = 1920;
            SensorVideoEncConfigTableNTSC[videoResID].CaptureHeight = 1080;
            break;
        case SENSOR_VIDEO_RES_HD_HFR_P240_P200:
        case SENSOR_VIDEO_RES_HD_HFR_P200_P200:
            SensorVideoEncConfigTableNTSC[videoResID].CaptureWidth = 1920;
            SensorVideoEncConfigTableNTSC[videoResID].CaptureHeight = 720;
            break;
        default:
            break;
        }
        return &SensorVideoEncConfigTableNTSC[videoResID];
    }
}

/**
 *  @brief Get the bit rate control table under video recording mode
 *
 *  Get the bit rate control table under video recording mode
 *
 *  @param [in] videoResID Video resolution id
 *  @param [in] videoQuality
 *
 *  @return >=0  bit rate table, <0 failure
 */
static APPLIB_VIDEOENC_BITRATE_s* sensor_imx117_a12_get_video_brc(int videoResID, int videoQuality)
{
    extern APPLIB_VIDEOENC_BITRATE_s SensorVideoBitRateTable[][VIDEO_QUALITY_NUM];
    return &SensorVideoBitRateTable[videoResID][videoQuality];
}

/**
 *  @brief Get the GOP table under video recording mode
 *
 *  Get the GOP table under video recording mode
 *
 *  @param [in] videoResID Video resolution id
 *
 *  @return >=0 Gop Table, <0 failure
 */
static APPLIB_VIDEOENC_GOP_s* sensor_imx117_a12_get_video_gop(int videoResID)
{
    extern APPLIB_VIDEOENC_GOP_s SensorVideoGOPTable[];
    return &SensorVideoGOPTable[videoResID];
}

/**
 *  @brief Check the video resolution valid in this sensor setting.
 *
 *  Check the video resolution valid in this sensor setting.
 *
 *  @param [in] videoResID Video resolution id
 *
 *  @return >=0 success, <0 failure
 */
static int sensor_imx117_a12_check_video_res(int videoResID)
{
    int i = 0;
    for (i=0; i<sensor_video_res_table_size; i++) {
        if (videoResID == sensor_video_res_table[i]) {
            return sensor_video_res_table[i];
        }
    }
    AmbaPrint("Not supported video resolution. Return default video resolution");
    return sensor_video_res_table[0];
}

/**
 *  @brief Get the PIV config table
 *
 *  Get the PIV config table
 *
 *  @param [in] videoResID Video resolution id
 *
 *  @return  PIV config table
 */
static APPLIB_SENSOR_PIV_CONFIG_s* sensor_imx117_a12_get_piv_config(int videoResID)
{
    extern APPLIB_SENSOR_PIV_CONFIG_s SensorPIVConfigTable[];
    return &SensorPIVConfigTable[videoResID];
}

/**
 *  @brief Get PIV Size config
 *
 *  Get PIV Size config
 *
 *  @param [in] videoResID Video resolution id
 *  @param [out] PIVCapConfig  PIV capture size config
 *
 *  @return  0 success <0 failure
 */
static int sensor_imx117_a12_get_piv_size(int videoResID, APPLIB_SENSOR_PIV_ENC_CONFIG_s *PIVCapConfig)
{
    int ReturnValue = 0;
    extern APPLIB_SENSOR_VIDEO_ENC_CONFIG_s SensorVideoEncConfigTableNTSC[];

        if (SensorVideoEncConfigTableNTSC[videoResID].VAR == VAR_16x9) {
        PIVCapConfig->VAR = VAR_16x9;
        PIVCapConfig->ScreennailWidth = PHOTO_SCRN_W;
        PIVCapConfig->ScreennailHeight = PHOTO_SCRN_H_16x9;
        PIVCapConfig->ScreennailActiveWidth = PHOTO_SCRN_W;
        PIVCapConfig->ScreennailActiveHeight = PHOTO_SCRN_H_16x9;
        PIVCapConfig->ThumbnailWidth = PHOTO_THUMB_W;
        PIVCapConfig->ThumbnailHeight = PHOTO_THUMB_H_4x3;
        PIVCapConfig->ThumbnailActiveWidth = PHOTO_THUMB_W;
        PIVCapConfig->ThumbnailActiveHeight = PHOTO_THUMB_H_16x9;
        PIVCapConfig->ThumbnailQality = PHOTO_QUALITY_DEFAULT;
        PIVCapConfig->FullviewQuality = PHOTO_QUALITY_DEFAULT;
        PIVCapConfig->ScreennailQuality = PHOTO_QUALITY_DEFAULT;
    } else if (SensorVideoEncConfigTableNTSC[videoResID].VAR == VAR_4x3) {
        PIVCapConfig->VAR = VAR_4x3;
        PIVCapConfig->ScreennailWidth = PHOTO_SCRN_W;
        PIVCapConfig->ScreennailHeight = PHOTO_SCRN_H_4x3;
        PIVCapConfig->ScreennailActiveWidth = PHOTO_SCRN_W;
        PIVCapConfig->ScreennailActiveHeight = PHOTO_SCRN_H_4x3;
        PIVCapConfig->ThumbnailWidth = PHOTO_THUMB_W;
        PIVCapConfig->ThumbnailHeight = PHOTO_THUMB_H_4x3;
        PIVCapConfig->ThumbnailActiveWidth = PHOTO_THUMB_W;
        PIVCapConfig->ThumbnailActiveHeight = PHOTO_THUMB_H_4x3;
        PIVCapConfig->ThumbnailQality = PHOTO_QUALITY_DEFAULT;
        PIVCapConfig->FullviewQuality = PHOTO_QUALITY_DEFAULT;
        PIVCapConfig->ScreennailQuality = PHOTO_QUALITY_DEFAULT;
    }

    switch (videoResID) {
        case SENSOR_VIDEO_RES_UHD_HALF:
        case SENSOR_VIDEO_RES_WQHD_FULL:
        case SENSOR_VIDEO_RES_WQHDP50:
        case SENSOR_VIDEO_RES_WQHD_HALF:
        case SENSOR_VIDEO_RES_TRUE_1080P_FULL:
        case SENSOR_VIDEO_RES_TRUE_1080P_HALF:
        case SENSOR_VIDEO_RES_HD_FULL:
            PIVCapConfig->CaptureWidth = 3840;
            PIVCapConfig->CaptureHeight = 2160;
            break;
        case SENSOR_VIDEO_RES_1920_1440P60:
            PIVCapConfig->CaptureWidth = 1920;
            PIVCapConfig->CaptureHeight = 1440;
            break;
        case SENSOR_VIDEO_RES_1920_1440P30:
        case SENSOR_VIDEO_RES_1440_1080P30:
            PIVCapConfig->CaptureWidth = 4000;
            PIVCapConfig->CaptureHeight = 3000;
            break;
        case SENSOR_VIDEO_RES_FHD_HFR_P120_P100:
        case SENSOR_VIDEO_RES_FHD_HFR_P100_P100:
            PIVCapConfig->CaptureWidth = 1920;
            PIVCapConfig->CaptureHeight = 1080;
            break;
        case SENSOR_VIDEO_RES_HD_HFR_P240_P200:
        case SENSOR_VIDEO_RES_HD_HFR_P200_P200:
            PIVCapConfig->CaptureWidth = 1920;
            PIVCapConfig->CaptureHeight = 720;
            break;
        default:
            break;
        }

    return ReturnValue;
}


/**
 *  @brief Register the sensor's information.
 *
 *  Register the sensor's information.
 *
 *  @return >=0 success, <0 failure
 */
int AppSensor_register_imx117_a12(void)
{
    APPLIB_SENSOR_s Dev = {0};
    char DevName[] = {'i','m','x','1','1','7','_','a','1','2','\0'};

    Dev.ID = 0;
    strcpy(Dev.Name, DevName);
    Dev.SysCapacity = SENSOR_SYS_NTSC | SENSOR_SYS_PAL;
    Dev.DzoomCapacity = 0;
    Dev.Rotate = SENSOR_ROTATE_0;
    Dev.VideoResNum = sensor_video_res_table_size;
    Dev.PjpegConfigNormalNum = PHOTO_CONFIG_NORMAL_TABLE_SIZE;
    Dev.PjpegConfigCollageNum = 0;
    Dev.PjpegConfigBurstNum = PHOTO_CONFIG_BURST_TABLE_SIZE;
    Dev.PhotoMaxVcapWidth = 4000;
    Dev.PhotoMaxVcapHeight = 3000;
    Dev.PhotoMaxEncWeight = 4608;
    Dev.PhotoMaxEncHeight = 3456;
    Dev.PhotoMaxPrevWidth = 1332;
    Dev.PhotoMaxPrevHeight = 1000;

    Dev.Init = sensor_imx117_a12_init;
    Dev.GetVideoResID = sensor_imx117_a12_get_videoResID;
    Dev.GetPhotoLiveviewModeID = sensor_imx117_a12_get_photo_liveview_mode_id;
    Dev.GetPhotoHfrModeID = sensor_imx117_a12_get_photo_hfr_mode_id;
    Dev.GetPhotoPreflashHfrModeID = sensor_imx117_a12_get_photo_preflash_hfr_mode_id;
    Dev.GetStillCaptureModeConfig =sensor_imx117_a12_get_still_capture_mode_config;
    Dev.GetStillCaptureObModeID =sensor_imx117_a12_get_still_capture_ob_mode_id;
    Dev.GetVideoResString = sensor_imx117_a12_get_video_res_str;
    Dev.GetPhotoSizeString = sensor_imx117_a12_get_photo_size_str;
    Dev.GetVinMode = sensor_imx117_a12_get_vinMode;
    Dev.GetCaptureModeAR = sensor_imx117_a12_get_capture_mode_ar;
    Dev.GetPreviewWindow = sensor_imx117_a12_get_preview_window;
    Dev.GetPhotoQualityConfig = sensor_imx117_a12_get_photo_quality_config;
    Dev.GetPhotoLiveviewConfig = sensor_imx117_a12_get_photo_liveview_config;
    Dev.GetPjpegConfig = sensor_imx117_a12_get_pjpeg_config;
    Dev.GetVideoConfig = sensor_imx117_a12_get_video_config;
    Dev.GetVideoBiteRate = sensor_imx117_a12_get_video_brc;
    Dev.GetVideoGOP = sensor_imx117_a12_get_video_gop;
    Dev.CheckVideoRes = sensor_imx117_a12_check_video_res;
    Dev.GetPIVConfig = sensor_imx117_a12_get_piv_config;
    Dev.GetPIVSize = sensor_imx117_a12_get_piv_size;
    Dev.GetShutterMode = sensor_imx117_a12_get_shutter_mode;
    Dev.GetMaxShutterTime = sensor_imx117_a12_get_max_shutter_time;

    AppLibSysSensor_Attach(&Dev);

    return 0;
}
