/**
 * @file inc/mw/player/videodec.h
 *
 * Amba video decoder header
 *
 * Copyright (C) 2013, Ambarella, Inc.
 *
 * All rights reserved. No Part of this file may be reproduced, stored
 * in a retrieval system, or transmitted, in any form, or by any means,
 * electronic, mechanical, photocopying, recording, or otherwise,
 * without the prior consent of Ambarella, Inc.
 */

#ifndef VIDEODEC_H_
#define VIDEODEC_H_

#include <AmbaDSP_VOUT.h>
#include "Decode.h"

/**
 * @defgroup VideoDec
 * @brief Amba video decoder implementation
 * 
 * Video decode module provide functions on video decode.\n
 * In most case, user control video decode via decode pipe.\n
 * Some function like GetInfo, SetEos, DumpFrame could run directly.\n
 * Please check function description for detail.
 *
 */

/**
 * @addtogroup VideoDec
 * @ingroup Codec
 * @{ 
 */

/**
 * transcoding Setting
 */
typedef struct _AMP_VIDEO_TRANSCODE_s_ {
    /** Enable transcode */
    UINT8 Enable;

    /** Data format: 0 - H.264 1 - Motion JPEG */
    UINT8 DataFmt;

    /** target width */
    UINT32 TargetWidth;

    /** target height */
    UINT32 TargetHeight;

    /** frame rate convert. 0 - no change */
    UINT32 FrameRateConvert;

    /** number of ticks per second*/
    UINT32 TimeScale;

    /** number of ticks per frame*/
    UINT32 TickPerPicture;

    /** buffer for raw file as file write buffer */
    char* RawBuffer;

    /** size of raw buffer */
    UINT32 RawBufferSize;

    /** is interlaced fields*/
    UINT8 Interlaced;
    UINT32 GopM;
    UINT32 GopN;
    UINT32 GopIdrInt;

} AMP_VIDEO_TRANSCODE_s;

/**
 * decode video initial config
 */
typedef struct _AMP_VIDEODEC_FEATURE_CFG_s_ {
    UINT8 EnablePbcapture; ///< if support to capture a video frame while play the video. default on
    UINT32 MaxVoutWidth;    ///< max supported vout width
    UINT32 MaxVoutHeight;   ///< max supported vout height
    UINT32 MaxVideoBitRate; ///< max supported bit rate
    UINT8 GopM;             ///< M of gop setting for max resolution
    UINT8 GopN;             ///< N of gop setting for max resolution
    UINT8 TrickplayBW;        ///< If support backware playback
    UINT8 TrickplayFWIPOnly; ///< If support fordware playback with I/P frame only
    UINT8 TrickplayFWIOnly;  ///< If support fordware playback with I frame only
    UINT8 TrickplayBWIPOnly; ///< If support backware playback with I/P frame only
    UINT8 TrickplayBWIOnly;  ///< If support backware playback with I frame only
    UINT8 SupportWideAngleVideo;       ///< If support wideangle
    AMP_VIDEO_TRANSCODE_s Transcode;   ///< transcode setting.
} AMP_VIDEODEC_FEATURE_CFG_s;

typedef struct _AMP_VIDEODEC_CFG_s_ {
    /** back ground color */
    AMP_YUV_COLOR_s BgColor;

    /** callback fuunction when config update */
    AMP_CALLBACK_f CbCfgUpdated;

    /** callback function on event */
    AMP_CALLBACK_f CbCodecEvent;

    /** buffer for raw file as file reading buffer */
    char* RawBuffer;

    /** size of raw buffer */
    UINT32 RawBufferSize;

    /** buffer for internal descriptor */
    char* DescBuffer;

    /** number of decsriptor */
    UINT32 NumDescBuffer;

    /** feature support list */
    AMP_VIDEODEC_FEATURE_CFG_s Feature;

    /** DSP working area address, 0 = no change */
    UINT8 *DspWorkBufAddr;

    /** DSP working area size, 0 = no change */
    UINT32 DspWorkBufSize;

    /** Handler group ID. Handlers with the same ID belong to the same pipe. */
    UINT8 HandlerGroupID;
} AMP_VIDEODEC_CFG_s;

typedef struct _AMP_VIDEODEC_INIT_CFG_s_ {
    UINT32 MaxHdlr;
    AMP_TASK_INFO_s TaskInfo;
    UINT8 *Buf;
    UINT32 BufSize;
} AMP_VIDEODEC_INIT_CFG_s;

typedef struct _AMP_VIDEODEC_FADING_EFFECT_s_ {
    /** fading start time in ms*/
    UINT32 StartTime;

    /** fading duration in ms, 0 to disable*/
    UINT32 Duration;

    /** fading start matrix */
    UINT16 StartMatrix[9];
    INT16 StartYOffset;
    INT16 StartUOffset;
    INT16 StartVOffset;

    /** fading end matrix */
    UINT16 EndMatrix[9];
    INT16 EndYOffset;
    INT16 EndUOffset;
    INT16 EndVOffset;
} AMP_VIDEODEC_FADING_EFFECT_s;

/**
 * To get the default value of initial config for video decode module
 * 
 * @param [out] cfg - default module config
 * 
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpVideoDec_GetInitDefaultCfg(AMP_VIDEODEC_INIT_CFG_s *cfg);

/**
 * To set the initial config for video decode module
 *
 * @param [in] cfg - module config
 *
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpVideoDec_Init(AMP_VIDEODEC_INIT_CFG_s *cfg);

/**
 * To get an available handler group ID
 *
 * @param [out] GroupID - codec handler
 *
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpVideoDec_GetFreeHdlrGroupID(UINT8 *GroupID);

/**
 * To get Video decode codec default config
 *
 * @param [out] cfg - default codec config
 *
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpVideoDec_GetDefaultCfg(AMP_VIDEODEC_CFG_s *cfg);

/**
 * To create Video decode codec
 *
 * @param [in] cfg - codec config
 * @param [out] hdlr - video codec handler
 *
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpVideoDec_Create(AMP_VIDEODEC_CFG_s *cfg,
                              AMP_AVDEC_HDLR_s **hdlr);

/**
 * To delete codec
 *
 * @param [in] hdlr - codec handler
 *
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpVideoDec_Delete(AMP_AVDEC_HDLR_s *hdlr);

/**
 * To dump current frame to YUV buffer
 *
 * @param [in] hdlr - codec handler
 * @param [in] buf - buffer info to store yuv data
 *
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpVideoDec_DumpFrame(AMP_AVDEC_HDLR_s *hdlr,
                                 AMP_YUV_BUFFER_s *buf);

/**
 * To get current time
 *
 * @param [in] Hdlr - codec handler
 * @param [out] Time - current time in ms
 *
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpVideoDec_GetTime(AMP_AVDEC_HDLR_s *Hdlr,
                               UINT64 *Time);

/**
 * To set EOS pts
 *
 * @param [in] Hdlr - codec handler
 * @param [in] EosFileTime - EOS time (of file).
 * @param [in] DeltaFileTime - EOS delta. It's time (of file) per frame.
 * @param [in] FileTimeScale - time (of file) per second.
 *
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpVideoDec_SetEosPts(AMP_AVDEC_HDLR_s *Hdlr,
                                 UINT64 EosFileTime,
                                 UINT32 DeltaFileTime,
                                 UINT32 FileTimeScale);

/**
 * To get transcode codec
 *
 * @param [in] hdlr - codec handler
 * @param [out] xcodeHdlr - transcode codec
 *
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpVideoDec_GetXCodec(AMP_AVDEC_HDLR_s *hdlr,
                                 AMP_AVDEC_HDLR_s **xcodeHdlr);

/**
 * To set the fading effect on video playback
 *
 * @param [in] hdlr - codec handler
 * @param [in] numFadingCfg - number of fading effect, max value is 2 to support fade in and fade out
 * @param [in] fadingCfg - fading parameters
 *
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpVideoDec_SetFadingEffect(AMP_AVDEC_HDLR_s *hdlr,
                                       UINT8 numFadingCfg,
                                       AMP_VIDEODEC_FADING_EFFECT_s *fadingCfg);

#if 0
/** 
 * template 
 * get the config
 * 
 * @param idx 
 * @param pParamStructure 
 * 
 * @return 
 */
extern int AmpVideoDec_GetXXXCfg(AMP_VIDDEC_HDLR_s *hdlr,
        AMP_VIDEODEC_XXX_CFG *pParamStructure);

/** 
 * template 
 * set the config 
 * 
 * @param idx 
 * @param pParamStructure 
 * 
 * @return 
 */
extern int AmpVideoDec_SetXXXCfg(AMP_VIDDEC_HDLR_s *hdlr,
        AMP_VIDEODEC_XXX_CFG *pParamStructure);

/**
 * @param videoDecHdlr [in] corelated video hdlr 
 * 
 * @return 
 */
extern int AmpVideoDec_GetInfo(AMP_VIDDEC_HDLR_s *hdlr,
        AMP_VIDEODEC_INFO_s *info);
#endif
/**
 * @}
 */

#endif /* VIDEODEC_H_ */

