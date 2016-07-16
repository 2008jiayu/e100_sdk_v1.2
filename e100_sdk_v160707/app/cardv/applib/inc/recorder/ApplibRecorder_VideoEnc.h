/**
 * @file src/app/connected/applib/inc/recorder/ApplibRecorder_VideoEnc.h
 *
 * Header of video config APIs
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
#ifndef APPLIB_VIDEO_ENC_H_
#define APPLIB_VIDEO_ENC_H_
/**
* @defgroup ApplibRecorder_VideoEnc
* @brief video encode related function
*
*
*/

/**
 * @addtogroup ApplibRecorder_VideoEnc
 * @ingroup Recorder
 * @{
 */
#include <applib.h>
#include "recorder/VideoEnc.h"  // for AppLibVideoEnc_EncodeStamp
__BEGIN_C_PROTO__

/*************************************************************************
 * Video definitions
 ************************************************************************/
#define DAY_LUMA_NORMAL      (0)
#define DAY_LUMA_HDR         (1)
#define DAY_LUMA_OVERSAMPLE  (2)

#define COMPLEX_DAY_NORMAL       (0)
#define COMPLEX_NIGHT_NORMAL     (1)
#define COMPLEX_DAY_HDR          (2)
#define COMPLEX_NIGHT_HDR        (3)
#define COMPLEX_DAY_OVERSAMPLE   (4)
#define COMPLEX_NIGHT_OVERSAMPLE (5)
/*!
*applib video enc frame mode enum
*/
typedef enum _APPLIB_VIDEOENC_FRAME_MODE_e_{
    ALL_FRAME = 0x1,
    ALL_FIELD = 0x2,
    ALL_MBAFF = 0x3
} APPLIB_VIDEOENC_FRAME_MODE_e;

/**
 * video encode bit rate
 */
typedef struct _APPLIB_VIDEOENC_BITRATE_s_ {
    UINT32 Mode; /**<Mode */
    float BitRateAvg; /**<Average Bit Rate  */
    float BitRateRatioMin;/**<Min Bit Rate Ratio */
    float BitRateRatioMax;/**<Max Bit Rate Ratio  */
} APPLIB_VIDEOENC_BITRATE_s;
/**
 * video encode GOP
 */
typedef struct _APPLIB_VIDEOENC_GOP_s_ {
    UINT32 Mode; /**<Mode */
    UINT32 M;/**< M */
    UINT32 N;/**< N */
    UINT32 Idr;/**< Idr */
} APPLIB_VIDEOENC_GOP_s;

/**
 * stream info
 */
typedef struct _APPLIB_VIDEOENC_STREAM_INFO_s_ {
    int Id;
    AMP_AVENC_HDLR_s *HdlVideoEnc;
    AMP_AVENC_HDLR_s *HdlAudioEnc;
} APPLIB_VIDEOENC_STREAM_INFO_s;

/**
 * stream list
 */
typedef struct _APPLIB_VIDEOENC_STREAM_LIST_s_ {
    int StreamCount;
    UINT32 ActiveStreamID;
    APPLIB_VIDEOENC_STREAM_INFO_s StreamList[AMP_VIDEOENC_STREAM_STREAM_ID_NUM];
} APPLIB_VIDEOENC_STREAM_LIST_s;


/**
 * video encode Setting
 */
typedef struct _APPLIB_VIDEOENC_SETTING_s_ {
    UINT8 SensorVideoRes; /**< Video Sensor Resolution */
    UINT8 YuvVideoRes; /**< YUV MODE Resolution */
    UINT8 TwoChSecStreamRes; /**< 2 channel second stream Resolution */
    UINT8 Quality; /**< Quality */
    UINT8 RecMode; /**< Record mode */
#define REC_MODE_AV (0)             /**<REC MODE AV */
#define REC_MODE_VIDEO_ONLY (1)     /**<REC MODE VIDEO_ONLY*/
    UINT8 PreRecord:4; /**< Pre Record On/Off */
#ifdef CONFIG_APP_ARD
	UINT8 PreRecordTime; /**< Pre Record Time */
#endif
#define VIDEO_PRE_RECORD_OFF    (0)        /**<VIDEO PRERECORD OFF*/
#define VIDEO_PRE_RECORD_ON     (1)        /**<VIDEO PRERECORD ON*/
    UINT8 DualStreams; /**< Dual stream on/off */
#define VIDEO_DUAL_STREAMS_OFF  (0)    /**< VIDEO DUAL STREAM OFF*/
#define VIDEO_DUAL_STREAMS_ON   (1)    /**< VIDEO DUAL STREAM ON*/
    UINT16 Split;	/**< Split file setting */
#define VIDEO_SPLIT_OFF         (0)                     /**<VIDEO SPLIT OFF*/
#define VIDEO_SPLIT_SIZE_1G          (1)                /**<VIDEO SPLIT SIZE 1G  */
#define VIDEO_SPLIT_SIZE_2G          (2)                /**<VIDEO SPLIT SIZE 2G  */
#define VIDEO_SPLIT_SIZE_4G          (3)                /**<VIDEO SPLIT SIZE 4G  */
#define VIDEO_SPLIT_SIZE_64M         (4)                /**<VIDEO SPLIT SIZE 64M */
#define VIDEO_SPLIT_SIZE_AUTO        (0xF)              /**<VIDEO SPLIT SIZE AUTO*/
#define VIDEO_SPLIT_TIME_60_SECONDS     (1<<4)          /**<VIDEO SPLIT TIME 60_SECONDS  */
#define VIDEO_SPLIT_TIME_180_SECONDS     (4<<4)          /**<VIDEO SPLIT TIME 60_SECONDS  */
#define VIDEO_SPLIT_TIME_300_SECONDS    (2<<4)          /**<VIDEO SPLIT TIME 300_SECONDS*/
#define VIDEO_SPLIT_TIME_30_MINUTES     (3<<4)          /**<VIDEO SPLIT TIME 30_MINUTES */
#define VIDEO_SPLIT_TIME_AUTO   (0xF<<4)                /**<VIDEO SPLIT TIME AUTO */
    UINT32 TimeLapse;	/**< Time lapse setting */
#define VIDEO_TIME_LAPSE_OFF    (0)      /**< VIDEO TIME LAPSE OFF*/
#define VIDEO_TIME_LAPSE_1S     (1)      /**< VIDEO TIME LAPSE 1S*/
#define VIDEO_TIME_LAPSE_2S     (2)      /**< VIDEO TIME LAPSE 2S*/
#define VIDEO_TIME_LAPSE_5S     (5)      /**< VIDEO TIME LAPSE 5S*/
#define VIDEO_TIME_LAPSE_30S    (30)     /**< VIDEO TIME LAPSE 30S*/
    UINT16 PivMode;       /**< PIV mode (skip or blanking) */
#define VIDEO_PIV_MODE_DEFAULT  (0)     /**<VIDEO PIV MODE DEFAULT */
#define VIDEO_PIV_MODE_SKIP     (1)     /**<VIDEO PIV MODE SKIP    */
#define VIDEO_PIV_MODE_BLANKING (2)     /**<VIDEO PIV MODE BLANKING*/
#define VIDEO_PIV_MODE_BOTH     (3)     /**<VIDEO PIV MODE BOTH    */
    UINT16 PivTileNumber; /**< Fixed PIV tile number */
    UINT16 PivThreshold;  /**< Fixed PIV threshold */
    UINT32 BitRateMonitor;/**Bit rate monitor on/off*/
#define BITRATE_MONITOR_OFF  (0)    /**< VIDEO DUAL STREAM OFF*/
#define BITRATE_MONITOR_ON   (1)    /**< VIDEO DUAL STREAM ON*/
    UINT32 AQPMonitor;/**AQP monitor on/off*/
#define AQP_MONITOR_OFF      (0)    /**< AQP monitor OFF*/
#define AQP_MONITOR_ON       (1)    /**< AQP monitor ON*/

} APPLIB_VIDEOENC_SETTING_s;

/*!
* applib video encode quality enum
*/
typedef enum _APPLIB_VIDEOENC_QUALITY_e_ {
    VIDEO_QUALITY_SFINE = 0,
    VIDEO_QUALITY_FINE,
    VIDEO_QUALITY_NORMAL,
    VIDEO_QUALITY_NUM
} APPLIB_VIDEOENC_QUALITY_e;

/*************************************************************************
 * Structures
 ************************************************************************/
/**
 * Extend data settings of bits buffer
 */
typedef struct _APPLIB_VIDEOENC_EXTEND_BITS_BUFFER_SETTING_s_ {
    UINT8 *BufferAddress;           /**< video bits buffer address              */
    UINT32 BufferSize;              /**< video bits buffer size                 */
    UINT32 FrameRate;               /**< video frame rate                       */
    UINT32 FrameRateScale;          /**< the time scale of video frame rate     */
} APPLIB_VIDEOENC_EXTEND_BITS_BUFFER_SETTING_s;

/*************************************************************************
 * Video Setting APIs
 ************************************************************************/

/**
 *  Initial the video encoder.
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibVideoEnc_Init(void);

/**
 *  Initial the live view
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibVideoEnc_LiveViewInit(void);

/**
 *  Configure the Live view
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibVideoEnc_LiveViewSetup(void);

/**
 *  Start live view
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibVideoEnc_LiveViewStart(void);

/**
 *  Stop live view
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibVideoEnc_LiveViewStop(void);

/**
 *  change pipe setting
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibVideoEnc_PipeChange(void);

/**
 *  delete pipe and codec
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibVideoEnc_PipeDelete(void);

/**
 *  Configure the encoder's parameter
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibVideoEnc_EncodeSetup(void);

/**
 *  Start to encode
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibVideoEnc_EncodeStart(void);

/**
 *  Pause encoding
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibVideoEnc_EncodePause(void);

/**
 *  Resume encoding
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibVideoEnc_EncodeResume(void);

/**
 *  Stop encoding.
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibVideoEnc_EncodeStop(void);

/**
 *  Time Lapse encoding.
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibVideoEnc_EncodeTimeLapse(void);

/**
 *  @brief Stamp encoding.
 *
 *  Stamp encoding.
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibVideoEnc_EncodeStamp(UINT8 encodeStreamId, AMP_VIDEOENC_BLEND_INFO_s *blendInfo);

/**
 *  Set sensor video resolution ID
 *
 *  @param [in] videoResID Video resolution id
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibVideoEnc_SetSensorVideoRes(int videoResID);

/**
 *  Set YUV device video resolution ID
 *
 *  @param [in] videoResID Video resolution id
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibVideoEnc_SetYuvVideoRes(int videoResID);

/**
 *  Set encode quality setting.
 *
 *  @param [in] quality Quality
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibVideoEnc_SetQuality(int quality);

/**
 *  Set pre-record mode setting
 *
 *  @param [in] preRecord Pre-record mode
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibVideoEnc_SetPreRecord(int preRecord);

#ifdef CONFIG_APP_ARD
/**
 *  Set pre-record Time
 *
 *  @param [in] preRecord Pre-record Time
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibVideoEnc_SetPreRecordTime(int time);

#endif

/**
 *  Set time lapse setting
 *
 *  @param [in] timeLapse Time lapse
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibVideoEnc_SetTimeLapse(int timeLapse);

/**
 *  Set dual streams setting
 *
 *  @param [in] dualStreams Dual stream
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibVideoEnc_SetDualStreams(int dualStreams);

/**
 *  @brief Set split setting
 *
 *  Set split setting
 *
 *  @param [in] split enable flag
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibVideoEnc_SetSplit(int split);

/**
 *  Set PIV mode
 *
 *  @param [in] pivMode PIV mode
 *
 *  @return Zero (don't care)
 */
extern int AppLibVideoEnc_SetPivMode(UINT32 pivMode);

/**
 *  Set fixed PIV tile number
 *
 *  @param [in] pivTileNum PIV tile number (dimensions: 0xVVHH)
 *
 *  @return Zero (don't care)
 */
extern int AppLibVideoEnc_SetPivTileNumber(UINT16 pivTileNum);

/**
 *  Set fixed PIV threshold
 *
 *  @param [in] pivThreshold pivThreshold PIV threshold (milliseconds)
 *
 *  @return Zero (don't care)
 */
extern int AppLibVideoEnc_SetPivThreshold(UINT16 pivThreshold);

/**
 *  Set record mode setting
 *
 *  @param [in] recMode Record mode
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibVideoEnc_SetRecMode(UINT8 recMode);

/**
 *  Set 2-channel second stream resolution
 *
 *  @param [in] res Resolution id
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibVideoEnc_Set2chSecStreamRes(UINT8 res);

/**
 *  @brief Set video second stream width
 *
 *  Set video second stream width
 *
 *  @return >=0 success, <0 failure
 */
extern UINT32 AppLibVideoEnc_SetSecStreamW(int streamWidth);

/**
 *  @brief Set video second stream height
 *
 *  Set video second stream height
 *
 *  @return >=0 success, <0 failure
 */
extern UINT32 AppLibVideoEnc_SetSecStreamH(int streamHeight);

/**
 *  @brief Set video second stream Time Scale
 *
 *  Set video second stream Time Scale
 *
 *  @return >=0 success, <0 failure
 */
extern UINT32 AppLibVideoEnc_SetSecStreamTimeScale(int timeScale);

/**
 *  @brief Set video second stream Tick
 *
 *  Set video second stream Tick
 *
 *  @return >=0 success, <0 failure
 */
extern UINT32 AppLibVideoEnc_SetSecStreamTick(int tick);

/**
 *  @brief Set video second stream gop M
 *
 *  Set video second stream gop M
 *
 *  @return >=0 success, <0 failure
 */
extern UINT32 AppLibVideoEnc_SetSecStreamGopM(int gopM);

/**
 *  @brief Set video second stream gop N
 *
 *  Set video second stream  gop N
 *
 *  @return >=0 success, <0 failure
 */
extern UINT32 AppLibVideoEnc_SetSecStreamGopN(int gopN);

/**
 *  @brief Set video second stream gop IDR
 *
 *  Set video second stream  gop IDR
 *
 *  @return >=0 success, <0 failure
 */
extern UINT32 AppLibVideoEnc_SetSecStreamGopIDR(int gopIDR);

/**
 *  @brief Set video second stream Bit Rate
 *
 *  Set video second stream  gop Bit Rate
 *
 *  @return >=0 success, <0 failure
 */
extern UINT32 AppLibVideoEnc_SetSecStreamBitRate(int bitRate);


/**
 *  @brief Set slow shutter
 *
 *  Set slow shutter
 *
 *  @return >=0 success, <0 failure
 */
extern UINT32 AppLibVideoEnc_SetEnalbeSlowShutter(UINT8 enable);


/**
 *  @brief Get current Bit Rate
 *
 *  Get current Bit Rate
 *
 *  @param [in] streamID stream id
 *
 *  @return current bit rate
 */
extern UINT32 AppLibvideoEnc_GetCurAvgBitrate(int streamID);

/**
 *  Get 2-channel second stream resolution
 *
 *  @return 2-channel second stream resolution
 */
extern int AppLibVideoEnc_Get2chSecStreamRes(void);

/**
 *  Get the video encoding setting.
 *
 *  @param [out] setting  The video encoding setting.
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibVideoEnc_GetSetting(APPLIB_VIDEOENC_SETTING_s *setting);

/**
 *  Get the setting of sensor resolution.
 *
 *  @return The setting of sensor resolution.
 */
extern int AppLibVideoEnc_GetSensorVideoRes(void);

/**
 *  Get the setting of YUV resolution.
 *
 *  @return The setting of YUV resolution.
 */
extern int AppLibVideoEnc_GetYuvVideoRes(void);

/**
 *  Get the setting of video quality.
 *
 *  @return The setting of video quality.
 */
extern int AppLibVideoEnc_GetQuality(void);

/**
 *  Get the pre-record setting.
 *
 *  @return The pre-record setting.
 */
extern int AppLibVideoEnc_GetPreRecord(void);

#ifdef CONFIG_APP_ARD
/**
 *  Get the pre-record Time.
 *
 *  @return The pre-record Time.
 */
extern int AppLibVideoEnc_GetPreRecordTime(void);
#endif

/**
 *  Get the setting of time lapse.
 *
 *  @return The setting of time lapse.
 */
extern int AppLibVideoEnc_GetTimeLapse(void);

/**
 *  Get dual stream setting.
 *
 *  @return The dual stream setting.
 */
extern int AppLibVideoEnc_GetDualStreams(void);

/**
 *  Get the setting of split file.
 *
 *  @return The setting of split file.
 */
extern int AppLibVideoEnc_GetSplit(void);

/**
 *  Get the setting of split file.
 *
 *  @return The setting of split file.
 */
extern int AppLibVideoEnc_GetSplitTimeSize(UINT32 *splitTime, UINT64 *splitSize);

/**
 *  Get assigned PIV mode
 *
 *  @return PIV mode
 */
extern int AppLibVideoEnc_GetPivMode(void);

/**
 *  Get assigned PIV tile number
 *
 *  @return  PIV tile number (dimensions: 0xVVHH)
 */
extern int AppLibVideoEnc_GetPivTileNumber(void);

/**
 *  Get assigned PIV threshold
 *
 *  @return PIV threshold (milliseconds)
 */
extern int AppLibVideoEnc_GetPivThreshold(void);

/**
 *  Get record mode
 *
 *  @return Record mode
 */
extern int AppLibVideoEnc_GetRecMode(void);

/**
 *  Get video buffer size
 *
 *  @return Video buffer size
 */
extern int AppLibVideoEnc_GetVideoBufSize(void);

/**
 *  @brief Get video second stream width
 *
 *  Get video second stream width
 *
 *  @return second stream width
 */
extern UINT32 AppLibVideoEnc_GetSecStreamW(void);
/**
 *  @brief Get video second stream height
 *
 *  Get video second stream height
 *
 *  @return second stream height
 */
extern UINT32 AppLibVideoEnc_GetSecStreamH(void);

/**
 *  @brief Get video second stream Time Scale
 *
 *  Get video second stream Time Scale
 *
 *  @return second stream Time Scale
 */
extern UINT32 AppLibVideoEnc_GetSecStreamTimeScale(void);

/**
 *  @brief Get video second stream Tick
 *
 *  Get video second stream Tick
 *
 *  @return second stream Tick
 */
extern UINT32 AppLibVideoEnc_GetSecStreamTick(void);

/**
 *  @brief Get video second stream gop M
 *
 *  Get video second stream gop M
 *
 *  @return second stream gop M
 */
extern UINT32 AppLibVideoEnc_GetSecStreamGopM(void);

/**
 *  @brief Get video second stream gop N
 *
 *  Get video second stream  gop N
 *
 *  @return second stream  gop N
 */
extern UINT32 AppLibVideoEnc_GetSecStreamGopN(void);

/**
 *  @brief Get video second stream gop IDR
 *
 *  Get video second stream  gop IDR
 *
 *  @return second stream  gop IDR
 */
extern UINT32 AppLibVideoEnc_GetSecStreamGopIDR(void);

/**
 *  @brief Get video second stream Bit Rate
 *
 *  Get video second stream  gop Bit Rate
 *
 *  @return second stream  gop Bit Rate
 */
extern UINT32 AppLibVideoEnc_GetSecStreamBitRate(void);

/**
 *  @brief Get slow shutter setting.
 *
 *  Get slow shutter setting.
 *
 *  @return Slow shutter setting
 */
extern UINT8 AppLibVideoEnc_GetEnalbeSlowShutter(void);

/**
 *  @brief Set extend encode module enable/disable status
 *
 *  Set extend encode module enable/disable status
 *
 *  @return success
 */
extern int AppLibVideoEnc_ExtendEnc_SetEnableStatus(UINT8 enableExtendEnc);

/**
 *  @brief Get extend encode module enable/disable status
 *
 *  Get extend encode module enable/disable status
 *
 *  @return 0 disable, >1 enable
 */
extern UINT8 AppLibVideoEnc_ExtendEnc_GetEnableStatus(void);

/**
 *  @brief Set video bits buffer frequency for extend data (unit: ms)
 *
 *  Set video bits buffer frequency for extend data
 *  And the unit is ms
 *
 *  @return success
 */
extern int AppLibVideoEnc_ExtendEnc_SetFrequency(UINT16 period);

/**
 *  @brief Get video bits buffer information for extend data
 *
 *  Get video bits buffer information for extend data
 *
 *  @return video bits buffer information
 */
extern int AppLibVideoEnc_ExtendEnc_GetConfigure(APPLIB_VIDEOENC_EXTEND_BITS_BUFFER_SETTING_s *ExtDataSettings);

/**
 *  To free the QV buffer after capture done
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibVideoEnc_PIVFreeBuf(void);

/**
 *  @brief PIV capture
 *
 *  PIV capture
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibVideoEnc_CapturePIV(void);


/**
 *  @brief Inital still encode codec for PIV
 *
 *  Inital still encode codec for PIV
 *
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibVideoEnc_PIVInit(void);

/**
 *  @brief Get avaliable stream list
 *
 *  Get avaliable stream list
 *
 *
 *  @return =0 success, <0 failure
 */
extern int AppLibVideoEnc_GetValidStream(APPLIB_VIDEOENC_STREAM_LIST_s *pStreamList);

/**
 *  @brief Erase data in fifo
 *
 *  Erase data in fifo
 *
 *
 *  @return =0 success, <0 failure
 */
extern int AppLibVideoEnc_EraseFifo(void);
__END_C_PROTO__
/**
 * @}
 */
#endif /* APPLIB_VIDEO_ENC_H_ */

