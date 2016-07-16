/**
 * @file src/app/connected/applib/inc/recorder/ApplibRecorder_StillEnc.h
 *
 * Header of photo config Apis
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

#ifndef APPLIB_STILL_ENC_H_
#define APPLIB_STILL_ENC_H_
/**
* @defgroup ApplibRecorder_StillEnc
* @brief Still encode related function
*
*
*/

/**
 * @addtogroup ApplibRecorder_StillEnc
 * @ingroup Recorder
 * @{
 */
#include <applib.h>


__BEGIN_C_PROTO__


#define PHOTO_M_SHUTTER         (0x04) /**<PHOTO M SHUTTER      */

/** Size default values */
#define PHOTO_THUMB_W           (160)         /**<PHOTO THUMB_W      */
#define PHOTO_THUMB_H_4x3       (120)         /**<PHOTO THUMB_H_4x3  */
#define PHOTO_THUMB_H_3x2       (106)         /**<PHOTO THUMB_H_3x2  */
#define PHOTO_THUMB_H_16x9      (90)          /**<PHOTO THUMB_H_16x9 */
#define PHOTO_SCRN_W            (960)         /**<PHOTO SCRN_W     */
#define PHOTO_SCRN_H_4x3        (720)         /**<PHOTO SCRN_H_4x3 */
#define PHOTO_SCRN_H_3x2        (640)         /**<PHOTO SCRN_H_3x2 */
#define PHOTO_SCRN_H_16x9       (540)         /**<PHOTO SCRN_H_16x9*/
/*!
*
* app photo size id enum
*/
typedef enum _APP_PHOTO_SIZE_ID_e_ {
    PHOTO_SIZE_1 = 0,
    PHOTO_SIZE_2,
    PHOTO_SIZE_3,
    PHOTO_SIZE_4,
    PHOTO_SIZE_5,
    PHOTO_SIZE_6,
    PHOTO_SIZE_7,
    PHOTO_SIZE_8,
    PHOTO_SIZE_9,
    PHOTO_SIZE_10,
    PHOTO_SIZE_NUM
} APP_PHOTO_SIZE_ID_e;
/*!
*
*app photo multi capture mode ID enum
*/
typedef enum _APP_PHOTO_MULTI_CAP_MODE_ID_e_ {
    PHOTO_MULTI_CAP_OFF = 0,
    PHOTO_MULTI_CAP_AEB,
    PHOTO_MULTI_CAP_NUM
} APP_PHOTO_MULTI_CAP_MODE_ID_e;
/*!
*
*app photo normal capture mode ID enum
*/
typedef enum _APP_PHOTO_NORM_CAP_MODE_ID_e_ {
    PHOTO_CAP_MODE_PRECISE,
    PHOTO_CAP_MODE_PES,
    PHOTO_CAP_MODE_BURST,
    PHOTO_CAP_MODE_PRE_CAPTURE,
    PHOTO_CAP_MODE_NUM
} APP_PHOTO_NORM_CAP_MODE_ID_e;

/**
 * still encode setting
 */
typedef struct _APPLIB_STILLENC_SETTING_s_ {
    UINT8 MultiCaptureMode; /**< Multiple photo capture mode*/
    UINT8 NormalCapMode;    /**< Normal photo capture mode */
    UINT16 SizeId;          /**< Photo size index. */
    UINT8 Quality;          /**< The value of photo quality. */
    UINT8 QualityMode;      /**< Photo quality id. */
#define PHOTO_QUALITY_SFINE     (0)   /**< PHOTO QUALITY SFINE  */
#define PHOTO_QUALITY_FINE      (1)   /**< PHOTO QUALITY FINE   */
#define PHOTO_QUALITY_NORMAL    (2)   /**< PHOTO QUALITY NORMAL */
#define PHOTO_QUALITY_AUTO      (3)   /**< PHOTO QUALITY AUTO   */
#define PHOTO_QUALITY_DEFAULT   (50)  /**< PHOTO QUALITY DEFAULT*/
    UINT16 ThumbnailWidth;  /**< The width of thumbnail. */
    UINT16 ThumbnailHeight; /**< The height of thumbnail. */
    UINT16 ScreennailWidth; /**< The width of screen-nail. */
    UINT16 ScreennailHeight;/**< The height of screen-nail. */
    UINT8 ThumbnailQuality; /**< The quality of thumbnail. */
    UINT8 ScreennailQuality;/**< The quality of screen-nail. */
    UINT32 CaptureNumber;   /**< The capture number. */
    UINT32 QuickviewDelay;  /**< Photo quick view delay setting*/
#define PHOTO_QUICKVIEW_DELAY_OFF       (0) /**< Delay Off */
#define PHOTO_QUICKVIEW_DELAY_1S        (1000) /**< Delay 1S (1000)*/
#define PHOTO_QUICKVIEW_DELAY_2S        (2000) /**< Delay 2S (2000) */
#define PHOTO_QUICKVIEW_DELAY_3S        (3000) /**< Delay 3S (3000) */
#define PHOTO_QUICKVIEW_DELAY_4S        (4000) /**< Delay 4S (4000) */
#define PHOTO_QUICKVIEW_DELAY_5S        (5000) /**< Delay 5S (5000) */
#define PHOTO_QUICKVIEW_DELAY_CONT      (600000) /**< Maximum of 10 minutes */
    UINT8 QuickView;        /**< Photo quick view on/off */
#define PHOTO_QUICKVIEW_OFF     (0x00) /**< QUICKVIEW OFF*/
#define PHOTO_QUICKVIEW_FCHAN   (0x01) /**< QUICKVIEW FCHAN*/
#define PHOTO_QUICKVIEW_DCHAN   (0x02) /**< QUICKVIEW DCHAN*/
#define PHOTO_QUICKVIEW_DUAL    (0x03) /**< QUICKVIEW DUAL*/
    UINT8 ShutterSetting;   /**< The shutter setting. */
#define DEFAULT_SHUTTER    (0)	  /**< DEFAULT SHUTTER. */
#define FORCE_R_SHUTTER    (1)    /**< Force rolling shutter. */
#define FORCE_M_SHUTTER    (2)    /**< Force m.shutter*/
    UINT8 FastAF;           /**< The fast AF mode. */
#define PHOTO_FAST_AF_OFF       (0x00) /**< PHOTO FAST AF OFF*/
#define PHOTO_FAST_AF_ON        (0x01) /**< PHOTO FAST AF ON */
    UINT8 Reserved;
} APPLIB_STILLENC_SETTING_s;
#ifdef CONFIG_APP_ARD
/**
 * still encode stamp-process information
 */
typedef struct _APPLIB_STILLENC_STAMP_AREA_SETTING_s_ {
    UINT32 OffsetX;                             /**< Offset X                           */
    UINT32 OffsetY;                             /**< Offset Y                           */
    UINT32 Width;                               /**< Width                              */
    UINT32 Height;                              /**< Height                             */
    UINT8 *YAddr;                               /**< YAddr                              */
    UINT8 *UVAddr;                              /**< UVAddr                             */
    UINT8 *AlphaYAddr;                          /**< Alpha Address for Y Channel        */
    UINT8 *AlphaUVAddr;                         /**< Alpha Address for UV Channel       */
} APPLIB_STILLENC_STAMP_AREA_SETTING_s;
typedef struct _APPLIB_STILLENC_STAMP_SETTING_s_ {
	UINT8 StampAreaEn[3];
    APPLIB_STILLENC_STAMP_AREA_SETTING_s StampAreaInfo[3];
} APPLIB_STILLENC_STAMP_SETTING_s;
#else
/**
 * still encode stamp-process information
 */
typedef struct _APPLIB_STILLENC_STAMP_SETTING_s_ {
    UINT32 OffsetX;                             /**< Offset X                           */
    UINT32 OffsetY;                             /**< Offset Y                           */
    UINT32 Width;                               /**< Width                              */
    UINT32 Height;                              /**< Height                             */
    UINT8 *YAddr;                               /**< YAddr                              */
    UINT8 *UVAddr;                              /**< UVAddr                             */
    UINT8 *AlphaYAddr;                          /**< Alpha Address for Y Channel        */
    UINT8 *AlphaUVAddr;                         /**< Alpha Address for UV Channel       */
} APPLIB_STILLENC_STAMP_SETTING_s;
#endif
/**
 *  This data structure describes the interface of MW preproc handler
 */
typedef struct _APPLIB_STILLENC_STAMPP_s_ {
    APPLIB_STILLENC_STAMP_SETTING_s StampInfo;
    UINT32 (*Process)(UINT8 type, UINT8 *yAddr, UINT8 *uvAddr, UINT16 width, UINT16 height, UINT16 pitch);
} APPLIB_STILLENC_STAMPP_s;

/**
 *  Initial the photo encoder.
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibStillEnc_Init(void);

/**
 *  Initial the live view
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibStillEnc_LiveViewInit(void);

/**
 *  Configure the Live view
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibStillEnc_LiveViewSetup(void);

/**
 *  Start live view
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibStillEnc_LiveViewStart(void);

/**
 *  Stop live view
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibStillEnc_LiveViewStop(void);

/**
 *  De-initial live view
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibStillEnc_LiveViewDeInit(void);

/**
 *  @brief delete still encode codec & pipe
 *
 *  delete still encode codec & pipe
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibStillEnc_DeletePipe(void);

/**
 *  To capture the photo with single capture mode
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibStillEnc_CaptureSingle(void);

/**
 *  @brief  Registersingle capture StampProc Callback
 *
 *  @param [in] info stamp information
 *
 *  @return 0 - success, -1 - fail
 */
extern int AppLibStillEnc_SingleCapRegisterStampCB(APPLIB_STILLENC_STAMP_SETTING_s stampSetting);

/**
 *  To free the photo buffer after capture done
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibStillEnc_SingleCapFreeBuf(void);

/**
 *  To capture the photo raw file with single capture mode
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibStillEnc_CaptureRaw(UINT32* RawBufAddr);

/**
 *  @brief encode raw
 *
 *  encode raw
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibStillEnc_RawEncode(void);

/**
 *  @brief get raw encode buffer
 *
 *  get raw encode buffer
 *
 *  @param [out] rawBuf raw Buffer information structure address
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibStillEnc_RawEncGetRawEncodeBuffer(AMBA_DSP_RAW_BUF_s *rawBuf);

/**
 *  To free the photo raw buffer after captureraw done
 *
 *  @return >=0 success, <0 failure
 */
extern UINT32 AppLibStillEnc_RawCapFreeBuf(void);

extern int AppLibStillEnc_RawEncPreLoadDone(void);

/**
 *  To free the photo buffer and remove fifo entry after raw encode done
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibStillEnc_RawEncFreeBuf(void);

/**
 *  @brief Raw Capture Sensor mode setting
 *
 *
 *  @param [in] Source sensor mode for system sensor:0 or set from API :1
 *  @param [in] Mode sensor mode data
 *
 *  @return 0 - success, -1 - fail
 */

extern UINT32 AppLibStillEnc_RawCaptureSetSensorMode(UINT32 Source, UINT16 Mode);


/**
 *  To capture the photo with the continuous mode.
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibStillEnc_CaptureSingleCont(void);

/**
 *  @brief  Register single capture StampProc Callback
 *
 *  @param [in] info stamp information
 *
 *  @return 0 - success, -1 - fail
 */
extern int AppLibStillEnc_SingleContCapRegisterStampCB(APPLIB_STILLENC_STAMP_SETTING_s stampSetting);

/**
 *  To free the photo buffer after capture done
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibStillEnc_SingleCapContFreeBuf(void);

extern UINT32 AppLibStillEnc_SetShotCount(UINT32 shotCount);
/**
 *  To capture the photo with the burst mode.
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibStillEnc_CaptureBurst(void);

/**
 *  @brief  Register single capture StampProc Callback
 *
 *  @param [in] info stamp information
 *
 *  @return 0 - success, -1 - fail
 */
extern int AppLibStillEnc_BurstCapRegisterStampCB(APPLIB_STILLENC_STAMP_SETTING_s stampSetting);

/**
 *  To free the photo buffer after capture done
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibStillEnc_BurstCapFreeBuf(void);

/**
 *  Set multiple frames capture mode
 *
 *  @param [in] capMode Capture mode
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibStillEnc_SetMultiCapMode(int capMode);

/**
 *  Set normal capture mode
 *
 *  @param [in] capMode Capture mode
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibStillEnc_SetNormCapMode(int capMode);

/**
 *  Set the photo size ID
 *
 *  @param [in] size Photo size ID
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibStillEnc_SetSizeID(int size);

/**
 *  Set the photo quality mode
 *
 *  @param [in] qualityMode Quality mode
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibStillEnc_SetQualityMode(int qualityMode);

/**
 *  Set the size of thumbnail
 *
 *  @param [in] width Width
 *  @param [in] height Height
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibStillEnc_SetPhotoThumbnailSize(UINT16 width, UINT16 height);

/**
 *  Set the size of screen-nail
 *
 *  @param [in] width Width
 *  @param [in] height Height
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibStillEnc_SetPhotoScreennailSize(UINT16 width, UINT16 height);

/**
 *  Set the quality of thumbnail
 *
 *  @param [in] quality Quality
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibStillEnc_SetThumbnailQuality(UINT8 quality);

/**
 *  Set the quality of screen-nail
 *
 *  @param [in] quality Quality
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibStillEnc_SetScreennailQuality(UINT8 quality);

/**
 *  Set the setting of quick view
 *
 *  @param [in] qv The setting of quick view
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibStillEnc_SetQuickview(int qv);

/**
 *  @brief Set the setting of quick view delay
 *
 *  Set the setting of quick view delay
 *
 *  @param [in] qvDelay The setting of quick view delay
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibStillEnc_SetQuickviewDelay(int qvDelay);

/**
 *  @brief Set the setting of fast AF
 *
 *  Set the setting of fast AF
 *
 *  @param [in] enable Enable flag
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibStillEnc_SetFastAf(int enable);

/**
 *  Set the shutter mode
 *
 *  @param [in] mode The shutter mode
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibStillEnc_SetShutterMode(int mode);

/**
 *  Set the capture number
 *
 *  @param [in] num Capture number
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibStillEnc_SetCaptureNum(int num);

/**
 *  Set slow shutter for liveview
 *
 *  @param [in] enable Enable the slow shutter.
 *
 *  @return >=0 success, <0 failure
 */
extern UINT32 AppLibStillEnc_SetEnalbeLiveViewSlowShutter(UINT8 enable);

/**
 *  Get the photo module setting
 *
 *  @param [out] setting The photo module setting
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibStillEnc_GetSetting(APPLIB_STILLENC_SETTING_s *setting);

/**
 *  Get the multiple frames capture mode
 *
 *  @return The multiple frames capture mode
 */
extern int AppLibStillEnc_GetMultiCapMode(void);

/**
 *  Get the normal capture mode
 *
 *  @return The normal capture mode
 */
extern int AppLibStillEnc_GetNormCapMode(void);

/**
 *  The photo size ID
 *
 *  @return The photo size ID
 */
extern int AppLibStillEnc_GetSizeID(void);

/**
 *  Get the photo quality mode
 *
 *  @return The photo quality mode
 */
extern int AppLibStillEnc_GetQuality(void);

/**
 *  Get the size of thumbnail
 *
 *  @param [out] width Width
 *  @param [out] height Height
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibStillEnc_GetPhotoThumbnailSize(UINT16 *width, UINT16 *height);

/**
 *  Get the size of screen-nail
 *
 *  @param [out] width Width
 *  @param [out] height Height
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibStillEnc_GetPhotoScreennailSize(UINT16 *width, UINT16 *height);

/**
 *  Get the quality of thumbnail
 *
 *  @return The quality of thumbnail
 */
extern int AppLibStillEnc_GetThumbnailQuality(void);

/**
 *  Get the quality of screen-nail
 *
 *  @return The quality of screen-nail
 */
extern int AppLibStillEnc_GetScreennailQuality(void);

/**
 *  Get the setting of quick view
 *
 *  @return The setting of quick view
 */
extern int AppLibStillEnc_GetQuickview(void);

/**
 *  Get the setting of quick view delay
 *
 *  @return The setting of quick view delay
 */
extern int AppLibStillEnc_GetQuickviewDelay(void);
/**
 *  Get the setting of fast AF
 *
 *  @return The setting of quick view delay
 */
extern int AppLibStillEnc_GetFastAf(void);

/**
 *  Get the shutter mode
 *
 *  @return The shutter mode
 */
extern int AppLibStillEnc_GetShutterMode(void);

/**
 *  Get the capture number
 *
 *  @return The capture number
 */
extern int AppLibStillEnc_GetCaptureNum(void);

/**
 *  @brief Get slow shutter setting of liveview.
 *
 *  Get slow shutter setting.
 *
 *  @return Slow shutter setting
 */
extern UINT8 AppLibStillEnc_GetEnalbeLiveViewSlowShutter(void);


extern int AppLibStillEnc_GetYuvWorkingBuffer(UINT16 MainWidth, UINT16 MainHeight, UINT16 RawWidth, UINT16 RawHeight, UINT16 *BufWidth, UINT16 *BufHeight);


/**
 *  Get the current capture mode
 *
 *  @return The current capture mode
 */
extern int AppLibStillEnc_GetPhotoPjpegCapMode(void);

/**
 *  Get the current capture size id
 *
 *  @return The current capture size id
 */
extern int AppLibStillEnc_GetPhotoPjpegConfigId(void);



/**
 * UnitTest: Still IDSP parameters setup should be done any time but before doing R2Y
 *
 * @param [in] aeIdx procMode can be class as follow:
 * 0 means Allproc: all IDSP params will be setup up at one time
 * 1 means Preproc: fast-calculate IDSP params can be setup before still capture
 * 2 means Postproc:Slow-calculate IDSP params can be setup after raw capture is done
 * User can setup IDSP either by Allproc or Preproc + Postproc, but make sure at correct timing
 *
 * @return 0 - success, -1 - fail
 */
extern UINT32 AppLibStillEnc_IdspParamSetup(UINT8 aeIdx);

/**
 *  Initial JPEG DQT
 *
 *  @param [in] quality
 *  @param [out] qTable
 */
extern void AppLibStillEnc_initJpegDqt(UINT8 *qTable, int quality);
/**
 *
 * Set Still WB
 *
 * @param [in] chipNo
 * @param [in] imgMode
 *
 * @return 0 - success, -1 - fail
 */
extern int AppLibStillEnc_SetStillWB(UINT32 chipNo, AMBA_DSP_IMG_MODE_CFG_s *imgMode);

extern int AppLibStillEnc_SetStillSensorInfo(AMBA_DSP_IMG_MODE_CFG_s *imgMode);

/**
 *  @brief Erase data in fifo
 *
 *  Erase data in fifo
 *
 *
 *  @return =0 success, <0 failure
 */
extern int AppLibStillEnc_EraseFifo(void);

__END_C_PROTO__
/**
 * @}
 */
#endif /* APPLIB_STILL_ENC_H_ */
