/**
 * @file src/app/connected/applib/inc/system/ApplibSys_Sensor.h
 *
 *  Header of Sensor interface.
 *
 * History:
 *    2013/08/14 - [Martin Lai] created file
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
#ifndef APPLIB_SENSOR_H_
#define APPLIB_SENSOR_H_
/**
* @defgroup ApplibSys_Sensor
* @brief Sensor interface.
*
*
*/

/**
 * @addtogroup ApplibSys_Sensor
 * @ingroup System
 * @{
 */

#include <recorder/Encode.h>
#include <recorder/VideoEnc.h>
#include <applib.h>

__BEGIN_C_PROTO__


/*!
 * sensor photo encode window config id
 */
typedef enum _SENSOR_PHOTO_CAP_MODE_ID_e_ {
    SENSOR_PHOTO_CAP_NORMAL = 0,
    SENSOR_PHOTO_CAP_COLLAGE,
    SENSOR_PHOTO_CAP_BURST
} SENSOR_PHOTO_CAP_MODE_ID_e;

/*!
 * applib sensor video resolution ID
 */
typedef enum _APPLIB_SENSOR_VIDEO_RES_ID_e_ {
    SENSOR_VIDEO_RES_UHD_HALF = 0,
    SENSOR_VIDEO_RES_2560_1920P30,
    SENSOR_VIDEO_RES_WQHD_FULL,
    SENSOR_VIDEO_RES_WQHD_HALF,
    SENSOR_VIDEO_RES_WQHD_HALF_HDR,
    SENSOR_VIDEO_RES_1296P30,
    SENSOR_VIDEO_RES_1920_1440P60,
    SENSOR_VIDEO_RES_1920_1440P30,
    SENSOR_VIDEO_RES_TRUE_1080P_FULL,
    SENSOR_VIDEO_RES_TRUE_1080P45,
    SENSOR_VIDEO_RES_TRUE_1080P40,
    SENSOR_VIDEO_RES_TRUE_1080P_HALF,
    SENSOR_VIDEO_RES_TRUE_HDR_1080P_HALF,
    SENSOR_VIDEO_RES_TRUE_1080I,
    SENSOR_VIDEO_RES_COMP_1080P_FULL,
    SENSOR_VIDEO_RES_COMP_1080P_HALF,
    SENSOR_VIDEO_RES_COMP_1080I,
    SENSOR_VIDEO_RES_1200P60,
    SENSOR_VIDEO_RES_1200P30,
    SENSOR_VIDEO_RES_1440_1080P30,
    SENSOR_VIDEO_RES_900P30,
    SENSOR_VIDEO_RES_HD_FULL,
    SENSOR_VIDEO_RES_HD_HALF,

    SENSOR_VIDEO_RES_WVGA_FULL,
    SENSOR_VIDEO_RES_WVGA_HALF,
    SENSOR_VIDEO_RES_SDWIDE,
    SENSOR_VIDEO_RES_SD,
    SENSOR_VIDEO_RES_960P60,
    SENSOR_VIDEO_RES_960P30,
    SENSOR_VIDEO_RES_540P30,
    SENSOR_VIDEO_RES_360P30,
    SENSOR_VIDEO_RES_VGA_FULL,
    SENSOR_VIDEO_RES_VGA_HALF,
    SENSOR_VIDEO_RES_WQVGA_FULL,
    SENSOR_VIDEO_RES_WQVGA_HALF,
    SENSOR_VIDEO_RES_QVGA,
    SENSOR_VIDEO_RES_CIF,

    SENSOR_VIDEO_RES_WQHDP50,
    SENSOR_VIDEO_RES_TRUE_1080P48,
    SENSOR_VIDEO_RES_TRUE_1080P24,
    SENSOR_VIDEO_RES_TRUE_1080P15,
    SENSOR_VIDEO_RES_1200P48,
    SENSOR_VIDEO_RES_1200P24,
    SENSOR_VIDEO_RES_960P48,
    SENSOR_VIDEO_RES_960P24,
    SENSOR_VIDEO_RES_HD_P48,
    SENSOR_VIDEO_RES_HD_P24,
    SENSOR_VIDEO_RES_WVGA_P48,
    SENSOR_VIDEO_RES_WVGA_P24,
    SENSOR_VIDEO_RES_VGA_P48,
    SENSOR_VIDEO_RES_VGA_P24,

    SENSOR_VIDEO_RES_PHOTO,             /** Resolution photo mode*/

    SENSOR_VIDEO_RES_FHD_HFR_P120_P100,
    SENSOR_VIDEO_RES_FHD_HFR_P100_P100,
    SENSOR_VIDEO_RES_HD_HFR_P240_P200,
    SENSOR_VIDEO_RES_HD_HFR_P200_P200,
    SENSOR_VIDEO_RES_HD_HFR_P120_P100,
    SENSOR_VIDEO_RES_WVGA_HFR_P240_P200,
    SENSOR_VIDEO_RES_WVGA_HFR_P120_P100,
    SENSOR_VIDEO_RES_VGA_HFR_P240_P200,
    SENSOR_VIDEO_RES_VGA_HFR_P120_P100,
    SENSOR_VIDEO_RES_WQVGA_HFR_P240_P200,
    SENSOR_VIDEO_RES_WQVGA_HFR_P120_P100,
    SENSOR_VIDEO_RES_QVGA_HFR_P120_P100,

    SENSOR_VIDEO_RES_NUM
} APPLIB_SENSOR_VIDEO_RES_ID_e;
/**
 *
 *applib sensor piv config
 *
 */
typedef struct _APPLIB_SENSOR_PIV_CONFIG_s_ {
    UINT16 Mode;            ///< PIV mode
    UINT16 Threshold;       ///< Time threshold
    UINT16 TileNumber;      ///< Tile dimensions
    UINT16 Reserved;        ///< Reserved
} APPLIB_SENSOR_PIV_CONFIG_s;

/**
 *
 *applib sensor vin config
 *
 */
typedef struct _APPLIB_SENSOR_VIN_CONFIG_s_ {
    int ResID;              ///< Resolution id
    UINT16 CapMode;         ///< Photo capture mode
    UINT16 PjpegConfigID;   ///< Tile dimensions
} APPLIB_SENSOR_VIN_CONFIG_s;

/**
 *
 * applib sensor video encode config
 *
 */
typedef struct _APPLIB_SENSOR_VIDEO_ENC_CONFIG_s_ {
    UINT16 CaptureWidth;                 /**<Capture Width*/
    UINT16 CaptureHeight;                /**<Capture Height*/
    UINT16 EncodeWidth;                  /**<Encode Width*/
    UINT16 EncodeHeight;                 /**<Encode Height*/
    int VAR;                             /**< Video Aspect Ratio*/
    UINT32 EncDenominator;               /**<Encode Denominator*/
    UINT32 EncNumerator;                 /**<Encode Numerator*/
    APPLIB_VIDEOENC_FRAME_MODE_e Mode;   /**<Mode*/
} APPLIB_SENSOR_VIDEO_ENC_CONFIG_s;

#define GOP_SIMPLE    0    /**< Simple GOP */
#define GOP_ADVANCED    1    /**< Hierarchical GOP */

/**
 *
 * applib sensor PIV encode config
 *
 */
typedef struct _APPLIB_SENSOR_PIV_ENC_CONFIG_s_ {
    UINT16 CaptureWidth;                 /**<Capture Width*/
    UINT16 CaptureHeight;                /**<Capture Height*/
    int VAR;                             /**< Video Aspect Ratio*/
    UINT16 ThumbnailWidth;        /**<Thumbnail Width*/
    UINT16 ThumbnailHeight;       /**<Thumbnail Height*/
    UINT16 ThumbnailActiveWidth;  /**<Thumbnail Active Width*/
    UINT16 ThumbnailActiveHeight; /**<Thumbnail Active Height*/
    UINT16 ScreennailWidth;       /**<Screennail Width,0 to disable*/
    UINT16 ScreennailHeight;      /**<Screennail Height,0 to disable*/
    UINT16 ScreennailActiveWidth;  /**<Screennail Active Width*/
    UINT16 ScreennailActiveHeight; /**<Screennail Active Height*/
    UINT8 ThumbnailQality;   /**<Thumbnail Qality*/
    UINT8 ScreennailQuality; /**<Screennail Quality*/
    UINT8 FullviewQuality;   /**<Fullview Quality*/
} APPLIB_SENSOR_PIV_ENC_CONFIG_s;

/**
 *
 * applib sensor still preview config
 *
 */
typedef struct _APPLIB_SENSOR_STILLPREV_CONFIG_s_ {
    UINT16 LVCaptureWidth;  /**<Live View Capture Width*/
    UINT16 LVCaptureHeight; /**<Live View Capture Height*/
    UINT16 LVMainWidth;     /**<Live View Main Width*/
    UINT16 LVMainHeight;    /**<Live View Main Height*/
    UINT32 TimeScale;       /**<Time Scale*/
    UINT32 TickPerPicture;  /**<TickPerPicture*/
    int VAR;                /**<Video Aspect Ratio*/
} APPLIB_SENSOR_STILLPREV_CONFIG_s;

/**
 *
 * applib sensor still capture config
 *
 */
typedef struct _APPLIB_SENSOR_STILLCAP_CONFIG_s_ {
    UINT32 CaptureNumber;         /**<Capture Number*/
    UINT16 ThumbnailWidth;        /**<Thumbnail Width*/
    UINT16 ThumbnailHeight;       /**<Thumbnail Height*/
    UINT16 ThumbnailActiveWidth;  /**<Thumbnail Active Width*/
    UINT16 ThumbnailActiveHeight; /**<Thumbnail Active Height*/
    UINT16 ScreennailWidth;       /**<Screennail Width,0 to disable*/
    UINT16 ScreennailHeight;      /**<Screennail Height,0 to disable*/
    UINT16 ScreennailActiveWidth;  /**<Screennail Active Width*/
    UINT16 ScreennailActiveHeight; /**<Screennail Active Height*/
    UINT16 FullviewWidth;    /**<Fullview Width*/
    UINT16 FullviewHeight;   /**<Fullview Height*/
    UINT8 ThumbnailQality;   /**<Thumbnail Qality*/
    UINT8 ScreennailQuality; /**<Screennail Quality*/
    UINT8 FullviewQuality;   /**<Fullview Quality*/
    UINT8 Jpeg3dFlag;        /**<Jpeg 3d Flag*/
#define REC_JPEG_DEFAULT         0 /**<REC_JPEG_DEFAULT        */
#define REC_JPEG_3D_LEFT_ONLY    1 /**<REC_JPEG_3D_LEFT_ONLY   */
#define REC_JPEG_3D_RIGHT_ONLY   2 /**<REC_JPEG_3D_RIGHT_ONLY  */
#define REC_JPEG_3D_L_R_SEPERATE 3 /**<REC_JPEG_3D_L_R_SEPERATE*/
} APPLIB_SENSOR_STILLCAP_CONFIG_s;

/**
 *
 * applib sensor still capture mode config
 *
 */
typedef struct _APPLIB_SENSOR_STILLCAP_MODE_CONFIG_s_ {
    int ModeId;             /**<Sensor mode ID*/
    UINT16 CaptureWidth;    /**<Capture window width*/
    UINT16 CaptureHeight;   /**<Capture window height*/
} APPLIB_SENSOR_STILLCAP_MODE_CONFIG_s;


/*************************************************************************
 * Sensor structure
 ************************************************************************/
/**
 *  This data structure describes the interface of a sensor
 */
typedef struct _APPLIB_SENSOR_s_ {
    /** Module ID */
    UINT32 ID;
    /** Module name */
    char Name[80];
    /** Module capability */
    UINT8 SysCapacity:2;
#define SENSOR_SYS_NTSC (0x01)     /**<SENSOR_SYS_NTSC (0x01)*/
#define SENSOR_SYS_PAL  (0x02)     /**<SENSOR_SYS_PAL  (0x02)*/
    UINT8 DzoomCapacity:1; /**<Dzoom Capacity*/
#define SENSOR_DZOOM_DISABLE    (0)     /**<SENSOR_DZOOM_DISABLE    (0)*/
#define SENSOR_DZOOM_ENABLE     (1)     /**<SENSOR_DZOOM_ENABLE     (1)*/
    UINT8 ThreeDCapacity:1; /**<3D Capacity*/
#define SENSOR_3D_DISABLE       (0)    /**<SENSOR_3D_DISABLE       (0)*/
#define SENSOR_3D_ENABLE        (1)    /**<SENSOR_3D_ENABLE        (1)*/
    UINT8 Rotate:2;          /**<*Rotate*/
#define SENSOR_ROTATE_0         (0) /**<SENSOR_ROTATE_0         (0)*/
#define SENSOR_ROTATE_90        (1) /**<SENSOR_ROTATE_90        (1)*/
#define SENSOR_ROTATE_180       (2) /**<SENSOR_ROTATE_180       (2)*/
#define SENSOR_ROTATE_270       (3) /**<SENSOR_ROTATE_270       (3)*/
    UINT8 Reserved:2;/**<Reserved*/
    UINT8 VideoResNum; /**< Module supported video resolution number */
    UINT8 PjpegConfigNormalNum; /**<Photo jpeg Config Normal Num*/
    UINT8 PjpegConfigCollageNum;/**<Photo jpeg Config Collage Num*/
    UINT8 PjpegConfigBurstNum;  /**<Photo jpeg Config Burst Num*/
    UINT16 Reserved1;           /**<Reserved1*/
    /** Config maximum info */
    UINT16 PhotoMaxVcapWidth;  /**<Photo Max Vcap Width*/
    UINT16 PhotoMaxVcapHeight; /**<Photo Max Vcap Height*/
    UINT16 PhotoMaxEncWeight;  /**<Photo Max Encode Weight*/
    UINT16 PhotoMaxEncHeight;  /**<Photo Max Encode Height*/
    UINT16 PhotoMaxPrevWidth;  /**<Photo Max Prev Width*/
    UINT16 PhotoMaxPrevHeight; /**<Photo Max Prev Height*/
    /**Config IQ Parameter Channel Count*/
    UINT32 IQChannelCount;
    /** Module init interface */
    int (*Init)(AMBA_DSP_CHANNEL_ID_u channel);
    /** Module parameter get interface */
    int (*GetVideoResID)(int resRef);/**<Get Video Resolution ID*/
    int (*GetPhotoLiveviewModeID)(int capMode, int pjpegConfigID); /**<Get Photo Liveview Mode ID*/
    int (*GetPhotoHfrModeID)(int capMode, int pjpegConfigID); /**<Get Photo Hfr Mode ID*/
    int (*GetPhotoPreflashHfrModeID)(int capMode, int pjpegConfigID); /**<Get Photo Pre flash Hfr Mode ID*/
    APPLIB_SENSOR_STILLCAP_MODE_CONFIG_s* (*GetStillCaptureModeConfig)(int capMode, int pjpegConfigID); /**<Get Still Capture Mode ID*/
    int (*GetStillCaptureObModeID)(int capMode, int pjpegConfigID); /**<Get Still Capture Object Mode ID*/
    UINT16* (*GetVideoResString)(int videoResID); /**<Get Video Resolution String*/
#define SENSOR_VIDEO_RES_STR_LEN        (25)  /**<SENSOR_VIDEO_RES_STR_LEN*/
    UINT16* (*GetPhotoSizeString)(int capMode, int pjpegConfigID); /**<Get Photo Size String*/
#define SENSOR_PHOTO_SIZE_STR_LEN       (30) /**<SENSOR_PHOTO_SIZE_STR_LEN*/
    int (*GetVinMode)(APPLIB_SENSOR_VIN_CONFIG_s *vinConfig); /**<Get Vin Mode*/
    int (*GetCaptureModeAR)(int capMode, int pjpegConfigID); /**< Get Capture Mode AR*/
    int (*GetPreviewWindow)(int capMode, int pjpegConfigID, int *width, int *height); /**< Get Preview Window*/
    int (*GetPhotoQualityConfig)(int qualityMode); /**< Get Photo Quality Config*/
    APPLIB_SENSOR_STILLPREV_CONFIG_s* (*GetPhotoLiveviewConfig)(int capMode, int pjpegConfigID); /**< Get Photo Live view Config*/
    APPLIB_SENSOR_STILLCAP_CONFIG_s* (*GetPjpegConfig)(int capMode, int pjpegConfigID); /**<Get Photo jpeg Config*/
    APPLIB_SENSOR_VIDEO_ENC_CONFIG_s* (*GetVideoConfig)(int videoResID); /**<Get Video Config*/
    APPLIB_VIDEOENC_BITRATE_s* (*GetVideoBiteRate)(int videoResID, int videoQuality); /**<Get Video Bite Rate*/
    APPLIB_VIDEOENC_GOP_s* (*GetVideoGOP)(int videoResID); /**<Get Video GOP*/
    int (*CheckVideoRes)(int videoResID); /**< Check Video Resolution*/
    APPLIB_SENSOR_PIV_CONFIG_s* (*GetPIVConfig)(int videoResID); /**<Get PIV Config*/
    int (*GetPIVSize)(int videoResID, APPLIB_SENSOR_PIV_ENC_CONFIG_s *PIVCapConfig); /**<Get PIV Size*/
    UINT32 (*GetShutterMode)(int capMode, int pjpegConfigID); /**<Get Shutter Mode*/
#define SENSOR_DEF_SHUTTER      (0)  /**<SENSOR_DEF_SHUTTER*/
#define SENSOR_R_SHUTTER        (1)  /**<SENSOR_R_SHUTTER  */
#define SENSOR_M_SHUTTER        (2)  /**<SENSOR_M_SHUTTER  */
    int (*GetMaxShutterTime)(int capMode, int pjpegConfigID); /**<Get Max Shutter Time*/
    UINT32* (*Get3dDzoomTable)(int videoResID, int var);      /**<Get 3d Dzoom Table*/
    UINT32 (*Get3dDzoomMaxRatio)(int videoResID, int var);    /**<Get 3d Dzoom Max Ratio*/
    int (*Get3dDzoomTotalStep)(int videoResID, int var);      /**<Get 3d Dzoom Total Step*/
} APPLIB_SENSOR_s;

extern AMBA_DSP_CHANNEL_ID_u AppEncChannel; /**< channel ID AppEncChannel*/


/**
 *  Remove the Sensor input device
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibSysSensor_Remove(void);

/**
 *  Attach the Sensor input device and enable the device control.
 *
 *  @param [in] dev informations of device
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibSysSensor_Attach(APPLIB_SENSOR_s *dev);

/**
 *  Clean sensor configuration
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibSysSensor_PreInit(void);

/**
 *  Initialize the sensor
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibSysSensor_Init(void);

/**
 *  Check the system capacity (NTSC or PAL)
 *
 *  @param [in] cap The system mode
 *
 *  @return 1 Support, 0 Not support
 */
extern int AppLibSysSensor_CheckSysCap(UINT32 cap);

/**
 *  Check the capacity of dzoom function
 *
 *  @return 1 Support, 0 Not support
 */
extern int AppLibSysSensor_CheckDzoomCap(void);

/**
 *  Check the capacity of 3D function
 *
 *  @return 1 Support, 0 Not support
 */
extern int AppLibSysSensor_Check3dCap(void);

/**
 *  Check the rotation of sensor
 *
 *  @return The rotation of sensor
 */
extern int AppLibSysSensor_CheckRotate(void);

/**
 *  Get the number of video resolution
 *
 *  @return The number of video resolution
 */
extern int AppLibSysSensor_GetVideoResNum(void);

/**
 *  Get the number of photo size
 *
 *  @param [in] capMode Capture mode
 *
 *  @return The number of photo size
 */
extern int AppLibSysSensor_GetPjpegConfigNum(int capMode);

/**
 *  Get the video resolution id by index
 *
 *  @param [in] resRef Reference index
 *
 *  @return The video resolution id
 */
extern int AppLibSysSensor_GetVideoResID(int resRef);

/**
 *  Get the sensor mode of photo live view
 *
 *  @param [in] capMode Capture mode
 *  @param [in] pjpegConfigID Photo size index
 *
 *  @return The sensor mode
 */
extern int AppLibSysSensor_GetPhotoLiveViewModeID(int capMode, int pjpegConfigID);

/**
 *  Get the sensor mode of photo HFR mode
 *
 *  @param [in] capMode Capture mode
 *  @param [in] pjpegConfigID Photo size index
 *
 *  @return The sensor mode
 */
extern int AppLibSysSensor_GetPhotoHfrModeID(int capMode, int pjpegConfigID);

/**
 *  Get the sensor mode of photo HFR pre-flash mode
 *
 *  @param [in] capMode Capture mode
 *  @param [in] pjpegConfigID Photo size index
 *
 *  @return The sensor mode
 */
extern int AppLibSysSensor_GetPhotoPreflashHfrModeID(int capMode, int pjpegConfigID);

/**
 *  Get the sensor mode of photo capture mode
 *
 *  @param [in] capMode Capture mode
 *  @param [in] pjpegConfigID Photo size index
 *
 *  @return The sensor mode config
 */
extern APPLIB_SENSOR_STILLCAP_MODE_CONFIG_s* AppLibSysSensor_GetStillCaptureModeConfig(int capMode, int pjpegConfigID);


/**
 *  Get the sensor mode of photo OB mode
 *
 *  @param [in] capMode Capture mode
 *  @param [in] pjpegConfigID Photo size index
 *
 *  @return The sensor mode
 */
extern int AppLibSysSensor_GetStillCaptureObModeID(int capMode, int pjpegConfigID);

/**
 *  Get the strings of video resolution
 *
 *  @param [in] videoResID Video resolution index
 *
 *  @return The strings of video resolution
 */
extern UINT16* AppLibSysSensor_GetVideoResStr(int videoResID);

/**
 *  Get the strings of photo size
 *
 *  @param [in] capMode Capture mode
 *  @param [in] pjpegConfigID Photo size index
 *
 *  @return The strings of photo size
 */
extern UINT16* AppLibSysSensor_GetPhotoSizeStr(int capMode, int pjpegConfigID);

/**
 *  Get the vin mode
 *
 *  @param [in] vinConfig The vin configuration
 *
 *  @return The vin mode
 */
extern int AppLibSysSensor_GetVinMode(APPLIB_SENSOR_VIN_CONFIG_s *vinConfig);

/**
 *  Get the aspect ratio of capture mode
 *
 *  @param [in] capMode Capture mode
 *  @param [in] pjpegConfigID Photo size index
 *
 *  @return The aspect ratio of capture mode
 */
extern int AppLibSysSensor_GetCaptureModeAR(int capMode, int pjpegConfigID);

/**
 *  Get the size of preview window
 *
 *  @param [in] capMode Capture mode
 *  @param [in] pjpegConfigID Photo size index
 *  @param [out] width Width
 *  @param [out] height Height
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibSysSensor_GetPreviewWindow(int capMode, int pjpegConfigID, int *width, int *height);

/**
 *  Get the quality value of photo
 *
 *  @param [in] qualityMode Quality mode index
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibSysSensor_GetPhotoQualityConfig(int qualityMode);

/**
 *  Get the photo live view configuration
 *
 *  @param [in] capMode Capture mode
 *  @param [in] pjpegConfigID Photo size index
 *
 *  @return The photo live view configuration
 */
extern APPLIB_SENSOR_STILLPREV_CONFIG_s* AppLibSysSensor_GetPhotoLiveviewConfig(int capMode, int pjpegConfigID);

/**
 *  Get the photo capture configuration
 *
 *  @param [in] capMode Capture mode
 *  @param [in] pjpegConfigID Photo size index
 *
 *  @return The photo capture configuration
 */
extern APPLIB_SENSOR_STILLCAP_CONFIG_s* AppLibSysSensor_GetPjpegConfig(int capMode, int pjpegConfigID);

/**
 *  Get the video configuration
 *
 *  @param [in] videoResID Video resolution index
 *
 *  @return The video configuration
 */
extern APPLIB_SENSOR_VIDEO_ENC_CONFIG_s* AppLibSysSensor_GetVideoConfig(int videoResID);

/**
 *  Get the video bit rate
 *
 *  @param [in] videoResID Video resolution index
 *  @param [in] videoQuality Video quality index
 *
 *  @return The video bit rate
 */
extern APPLIB_VIDEOENC_BITRATE_s* AppLibSysSensor_GetVideoBitRate(int videoResID, int videoQuality);

/**
 *  Get the GOP of video
 *
 *  @param [in] videoResID Video resolution index
 *
 *  @return The GOP of video
 */
extern APPLIB_VIDEOENC_GOP_s* AppLibSysSensor_GetVideoGOP(int videoResID);

/**
 *  Get the maximum size of video capture
 *
 *  @param [out] width Width
 *  @param [out] height Height
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibSysSensor_GetPhotoMaxVcapSize(int *width, int *height);

/**
 *  Get the maximum size of photo encoding size
 *
 *  @param [out] width Width
 *  @param [out] height Height
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibSysSensor_GetPhotoMaxEncSize(int *width, int *height);

/**
 *  @brief Get IQ Channel Count
 *
 *  Get IQ Channel Count
 *
 *
 *  @return  IQ Channel Count
 */
extern UINT32 AppLibSysSensor_GetIQChannelCount(void);

/**
 *  Check the video resolution valid in this sensor setting.
 *
 *  @param [in] videoResID Video resolution index
 *
 *  @return >=0 Valid, <0 failure
 */
extern int AppLibSysSensor_CheckVideoRes(int videoResID);

/**
 *  Get the maximum size of photo preview size
 *
 *  @param [out] width Width
 *  @param [out] height Height
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibSysSensor_GetPhotoMaxPreviewSize(int *width, int *height);

/**
 *  Get PIV configuration of a specific video resolution.
 *
 *  @param videoResID Video resolution ID
 *
 *  @return APPLIB_SENSOR_PIV_CONFIG_s* Address of the requested PIV configuration
 *
 */
extern APPLIB_SENSOR_PIV_CONFIG_s* AppLibSysSensor_GetPIVConfig(int videoResID);

/**
 *  @brief Get the PIV config
 *
 *  @param [in] videoResID Video resolution id
 *  @param [out] pIVCapConfig PIV config
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibSysSensor_GetPIVSize(int videoResID, APPLIB_SENSOR_PIV_ENC_CONFIG_s *pIVCapConfig);

/**
 *  Get 3D dzoom table
 *
 *  @param [in] videoResID Video resolution id
 *  @param [in] var Parameter
 *
 *  @return 3D dzoom table
 */
extern UINT32* AppLibSysSensor_Get3dDzoomTable(int videoResID, int var);

/**
 *  Get the maximum ratio of 3D dzoom
 *
 *  @param [in] videoResID Video resolution id
 *  @param [in] var Parameter
 *
 *  @return The maximum ratio of 3D dzoom
 */
extern UINT32 AppLibSysSensor_Get3dDzoomMaxRatio(int videoResID, int var);

/**
 *  Get the total step of 3D dzoom
 *
 *  @param [in] videoResID Video resolution id
 *  @param [in] var Parameter
 *
 *  @return The total step of 3D dzoom
 */
extern int AppLibSysSensor_Get3dDzoomTotalStep(int videoResID, int var);

/**
 *  Get the shutter mode under photo capture mode
 *
 *  @param [in] capMode Capture mode
 *  @param [in] pjpegConfigID Photo size index
 *
 *  @return The shutter mode
 */
extern UINT32 AppLibSysSensor_GetShutterMode(int capMode, int pjpegConfigID);

/**
 *  Get maximum shutter time
 *
 *  @param [in] capMode Capture mode
 *  @param [in] pjpegConfigID Photo size index
 *
 *  @return The maximum shutter time
 */
extern int AppLibSysSensor_GetMaxShutterTime(int capMode, int pjpegConfigID);

/**
 *  Get the video resolution index
 *
 *  @param [in] resRef Reference video resolution index
 *
 *  @return >=0 The video resolution index, <0 failure
 */
extern int AppLibSysSensor_GetVideoResIdx(UINT16 *resRef);

/**
 *  Get the index of photo size
 *
 *  @param [in] capMode Capture mode
 *  @param [in] photoSize Reference photo size index
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibSysSensor_GetPhotoSizeID(int capMode, UINT16* photoSize);

/**
 *  @brief Set IQ Channel Count
 *
 *  Set IQ Channel Count
 *
 *  @param [in] IQChannelCount IQ Channel Count
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibSysSensor_SetIQChannelCount(UINT32 IQChannelCount);

__END_C_PROTO__
/**
 * @}
 */
#endif /* APPLIB_SENSOR_H_ */
