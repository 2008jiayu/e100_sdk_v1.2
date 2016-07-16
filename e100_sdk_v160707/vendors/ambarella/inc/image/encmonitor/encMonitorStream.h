 /**
  * @file encMonitorStream.h
  *
  * Encode monitor Stream header
  *
  * Copyright (C) 2015, Ambarella, Inc.
  *
  * All rights reserved. No Part of this file may be reproduced, stored
  * in a retrieval system, or transmitted, in any form, or by any means,
  * electronic, mechanical, photocopying, recording, or otherwise,
  * without the prior consent of Ambarella, Inc.
  */

#ifndef _ENC_MONITOR_STREAM_H_
#define _ENC_MONITOR_STREAM_H_

#include <string.h>
#include "AmbaPrintk.h"
#include "AmbaDSP_VideoEnc.h"

/**
 * Runtime stream (h.264) quality control patemete
 */
typedef struct _AMBA_ENCMONITOR_RUNTIME_QUALITY_CFG_s_ {
    UINT32                              Cmd;
#define QC_BITRATE   (1<<0)
#define QC_GOP       (1<<1)
#define QC_QP        (1<<2)
#define QC_QMODEL    (1<<3)
#define QC_ROI       (1<<4)
#define QC_HQP       (1<<5)
#define QC_ZMV       (1<<6)
#define QC_FORCE_IDR (1<<7)
    UINT32 BitRate; //in bit Unit

    UINT8 Enable;
    UINT8 M;
    UINT8 N;
    UINT8 IDR;

    UINT8 QpMinI;
    UINT8 QpMaxI;
    UINT8 QpMinP;
    UINT8 QpMaxP;
    UINT8 QpMinB;
    UINT8 QpMaxB;
    UINT8 HQpMax;
    UINT8 HQpMin;
    UINT8 HQpReduce;
    UINT8 HPNumber;

    UINT8 IorIDRFrameNeedsRateControlMask;
    UINT8 QPReduceNearIDRFrame;
    UINT8 ClassNumberLimitOfQPReduceNearIDRFrame;
    UINT8 QPReduceNearIFrame;
    UINT8 ClassNumberLimitOfQPReduceNearIFrame;
    INT8 Intra16x16Bias;    /**< -64 ~ 64 */
    INT8 Intra4x4Bias;      /**< -64 ~ 64 */
    INT8 Inter16x16Bias;    /**< -64 ~ 64 */
    INT8 Inter8x8Bias;      /**< -64 ~ 64 */
    INT8 Direct16x16Bias;   /**< -64 ~ 64 */
    INT8 Direct8x8Bias;     /**< -64 ~ 64 */
    INT8 MELambdaQpOffset;  /**< 0 ~ 51 */
    INT8 AQPStrength;       /**< 0: auto, -1: inverse AQP, 1-81: fixed strength, 1 for no AQP */
    INT8 LoopFilterAlpha;   /**< -6 ~ 6 */
    INT8 LoopFilterBeta;    /**< -6 ~ 6 */
    INT8 Alpha;             /**< -6 ~ 6 */
    INT8 Beta;              /**< -6 ~ 6 */

    UINT32 *RoiBufferAddr;
    INT8 RoiDelta[3][4];
    UINT32 ZmvThres;
} AMBA_ENCMONITOR_RUNTIME_QUALITY_CFG_s;

/**
 * common handler structure for stream monitor
 */
typedef struct _AMBA_IMG_ENCODE_MONITOR_STRM_HDLR_s_ {
    void *ctx;                      /**< pointer to monitor handler */
} AMBA_IMG_ENCODE_MONITOR_STRM_HDLR_s;

/**
 * Memory query config
 */
typedef struct _AMBA_IMG_ENC_MONITOR_STRM_MEMORY_s_ {
	UINT8 MaxStreamNumber;              /**< max. number for stream monitor */
} AMBA_IMG_ENC_MONITOR_STRM_MEMORY_s;

/**
 * Module initialization config
 */
typedef struct _AMBA_IMG_ENC_MONITOR_STRM_INIT_CFG_s_ {
    UINT8 *MemoryPoolAddr;              /**< encode monitor module working pool address */
    UINT32 MemoryPoolSize;              /**< encode monitor working pool size */
    UINT8 MaxStreamNumber;              /**< max. number for stream monitor */
} AMBA_IMG_ENC_MONITOR_STRM_INIT_CFG_s;

/**
 * Current encoding info
 */
typedef struct _AMBA_IMG_ENC_MONITOR_ENCODING_INFO_s_ {
    AMBA_DSP_H264ENC_REALTIME_QUALITY_s QCfg;
    UINT32 TotalFrames;         /**< Total encoded frames number since encode start */
    UINT64 TotalBytes;          /**< Total encoded bytes since encode start */
} AMBA_IMG_ENC_MONITOR_ENCODING_INFO_s;

/**
 *  @brief Register a stream to collection data
 *
 *  Register a handler.
 *
 *  @param [in] type monitor type
 *  @param [out] emonStrmHdlr handler
 *
 * @return 0 - OK, others - AMBA_IMG_ENCMONITOR_ER_CODE_e
 * @see AMBA_IMG_ENCMONITOR_ER_CODE_e
 */
extern int AmbaEncMonitor_StreamRegister(AMBA_DSP_EVENT_VIDEO_ENC_STATUS_s stream, AMBA_IMG_ENCODE_MONITOR_STRM_HDLR_s **emonStrmHdlr);

/**
 *  @brief Unregister a stream to collection data
 *
 *  @param [in emonStrmHdlr handler
 *
 * @return 0 - OK, others - AMBA_IMG_ENCMONITOR_ER_CODE_e
 * @see AMBA_IMG_ENCMONITOR_ER_CODE_e
 */
extern int AmbaEncMonitor_StreamUnregister(AMBA_IMG_ENCODE_MONITOR_STRM_HDLR_s *emonStrmHdlr);

/**
 *  @brief Enable/Disable a stream handler
 *
 *  @param [in] emonStrmHdlr handler
 *  @param [in] enable enable or not
 *
 *  @return >=0 success, <0 failure
 */
extern int AmbaEncMonitor_EnableStreamHandler(AMBA_IMG_ENCODE_MONITOR_STRM_HDLR_s *emonStrmHdlr, UINT32 enable);

/**
 * Get encode monitor memory usage by given paramter
 *
 * @param [in] cfg memory query config
 * @param [out] memSize total memery size of given config
 *
 * @return 0 - OK, others - AMBA_IMG_ENCMONITOR_ER_CODE_e
 * @see AMBA_IMG_ENCMONITOR_ER_CODE_e
 */
extern int AmbaEncMonitorStream_QueryMemsize(AMBA_IMG_ENC_MONITOR_STRM_MEMORY_s *cfg, UINT32 *memSize);

/**
 * Get encode monitor default config for initializing
 *
 * @param [out] cfg image scheduler module config
 *
 * @return 0 - OK, others - AMBA_IMG_ENCMONITOR_ER_CODE_e
 * @see AMBA_IMG_ENCMONITOR_ER_CODE_e
 */
extern int AmbaEncMonitorStream_GetInitDefaultCfg(AMBA_IMG_ENC_MONITOR_STRM_INIT_CFG_s *defInitCfg);

/**
 * Initialize Encode Monitor module.
 * The function should only be invoked once.
 * User MUST invoke this function before using Encode Monitor module.
 * The memory pool of the module will provide by user.
 * please be careful that this API shall be invoke after MW_INIT
 *
 * @param [in] cfg encode monitor module config
 *
 * @return 0 - OK, others - AMBA_IMG_ENCMONITOR_ER_CODE_e
 * @see AMBA_IMG_ENCMONITOR_ER_CODE_e
 */
extern int AmbaEncMonitorStream_Init(AMBA_IMG_ENC_MONITOR_STRM_INIT_CFG_s *cfg);

/**
 * Runtime control quality for certain stream
 *
 * @param [in] hdlr stream handler
 * @param [in] cfg runtime control configuration
 *
 * @return 0 - OK, others - AMBA_IMG_ENCMONITOR_ER_CODE_e
 * @see AMBA_IMG_ENCMONITOR_ER_CODE_e
 */
extern int AmbaEncMonitor_SetRuntimeQuality(AMBA_IMG_ENCODE_MONITOR_STRM_HDLR_s *strmHdlr, AMBA_ENCMONITOR_RUNTIME_QUALITY_CFG_s *cfg);

/**
 * Runtime control quality for certain stream
 *
 * @param [in] hdlr stream handler
 * @param [in] current encoding info
 *
 * @return 0 - OK, others - AMBA_IMG_ENCMONITOR_ER_CODE_e
 * @see AMBA_IMG_ENCMONITOR_ER_CODE_e
 */
extern int AmbaEncMonitor_GetCurrentEncodingInfo(AMBA_IMG_ENCODE_MONITOR_STRM_HDLR_s *emonStrmHdlr, AMBA_IMG_ENC_MONITOR_ENCODING_INFO_s *encInfo);

#endif /* _ENC_MONITOR_STREAM_H_ */
