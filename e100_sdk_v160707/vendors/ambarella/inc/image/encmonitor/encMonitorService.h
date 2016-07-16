 /**
  * @file encMonitorService.h
  *
  * Encode monitor Service header
  *
  * Copyright (C) 2015, Ambarella, Inc.
  *
  * All rights reserved. No Part of this file may be reproduced, stored
  * in a retrieval system, or transmitted, in any form, or by any means,
  * electronic, mechanical, photocopying, recording, or otherwise,
  * without the prior consent of Ambarella, Inc.
  */

#ifndef _ENC_MONITOR_SERVICE_H_
#define _ENC_MONITOR_SERVICE_H_

#include <string.h>
#include "AmbaPrintk.h"
#include "encMonitor.h"
#include "encMonitorStream.h"

/**
 * Common handler structure for AQP monitor
 */
typedef struct _AMBA_IMG_ENC_MONITOR_AQP_HDLR_s_ {
    void *ctx;                      /**< pointer to monitor handler */
} AMBA_IMG_ENC_MONITOR_AQP_HDLR_s;

/**
 *  Amba Encode Monitor AQP control handler config
 */
typedef struct _AMBA_IMG_ENC_MONITOR_AQP_HDLR_CFG_s_ {
    AMBA_ENCMONITOR_CYCLIC_TYPE_e Type;
    UINT32 Period;
    UINT8 Priority;
    AMBA_IMG_ENCODE_MONITOR_STRM_HDLR_s *emonStrmHdlr;  /**< which stream to adjust */
    int (*AqpCB)(AMBA_IMG_ENCODE_MONITOR_STRM_HDLR_s *StrmHdlr); /**< AQP CB */
} AMBA_IMG_ENC_MONITOR_AQP_HDLR_CFG_s;

/**
 * Common handler structure for bitrate monitor
 */
typedef struct _AMBA_IMG_ENC_MONITOR_BITRATE_HDLR_s_ {
    void *ctx;                      /**< pointer to monitor handler */
} AMBA_IMG_ENC_MONITOR_BITRATE_HDLR_s;

/**
 *  Amba Encode Monitor BitRate control handler
 */
typedef struct _BITRATE_CONTROL_COMPLEXITY_HANDLER_s_ {
    int (*AdjustQpCB)(int mode, UINT8 *isQpModify, AMBA_IMG_ENCODE_MONITOR_STRM_HDLR_s *strmHdlr, AMBA_IMG_ENC_MONITOR_BITRATE_HDLR_s *hdlr);
    int (*GetDayLumaThresCB)(int mode, UINT32 *threshold); /**< Get Day Luma Threshold CB */
    int (*GetComplexityRangeCB)(int mode, UINT32 *complexMin, UINT32 *complexMid, UINT32 *complexMax); /**< Get Complexity Range CB function */
} BITRATE_CONTROL_COMPLEXITY_HANDLER_s;

/**
 *  Amba Encode Monitor BitRate control handler config
 */
typedef struct _AMBA_IMG_ENC_MONITOR_BRC_HDLR_CFG_s_ {
    AMBA_ENCMONITOR_CYCLIC_TYPE_e Type;
    UINT32 Period;
    UINT8 Priority;
    UINT8 VideoOSMode:1;
    UINT8 VideoProcMode:1;
    UINT8 VideoHdrMode:1;
    UINT8 VideoMultiChan:5;
    AMBA_IMG_ENCODE_MONITOR_STRM_HDLR_s *emonStrmHdlr;  /**< which stream to adjust */
    BITRATE_CONTROL_COMPLEXITY_HANDLER_s CmplxHdlr;
    UINT32 AverageBitrate;                              /**< Average bitrate in byte */
    UINT32 MinBitrate;              	                /**< Max. bitrate in byte */
    UINT32 MaxBitrate;                                  /**< Min. bitrate in byte */
} AMBA_IMG_ENC_MONITOR_BRC_HDLR_CFG_s;

/**
 * Memory query config
 */
typedef struct _AMBA_IMG_ENC_MONITOR_AQP_MEMORY_s_ {
	UINT8 MaxStreamNumber;              /**< max. number for stream monitor */
} AMBA_IMG_ENC_MONITOR_AQP_MEMORY_s;

typedef struct _AMBA_IMG_ENC_MONITOR_BRC_MEMORY_s_ {
	UINT8 MaxStreamNumber;              /**< max. number for stream monitor */
} AMBA_IMG_ENC_MONITOR_BRC_MEMORY_s;

/**
 * Service initialization config
 */
typedef struct _AMBA_IMG_ENC_MONITOR_AQP_INIT_CFG_s_ {
    UINT8 *MemoryPoolAddr;              /**< encode monitor module working pool address */
    UINT32 MemoryPoolSize;              /**< encode monitor working pool size */
    UINT8 MaxStreamNumber;              /**< max. number for stream monitor */
} AMBA_IMG_ENC_MONITOR_AQP_INIT_CFG_s;

typedef struct _AMBA_IMG_ENC_MONITOR_BITRATE_INIT_CFG_s_ {
    UINT8 *MemoryPoolAddr;              /**< encode monitor module working pool address */
    UINT32 MemoryPoolSize;              /**< encode monitor working pool size */
    UINT8 MaxStreamNumber;              /**< max. number for stream monitor */
} AMBA_IMG_ENC_MONITOR_BITRATE_INIT_CFG_s;

/**
 *  @brief Enable/Disable a AQP service
 *
 *  @param [in] hdlr service handler
 *  @param [in] enable enable or not
 *
 *  @return >=0 success, <0 failure
 */
extern int AmbaEncMonitorAQP_EnableService(AMBA_IMG_ENC_MONITOR_AQP_HDLR_s *hdlr, UINT32 enable);

/**
 *  @brief Register a AQP Control Service handler
 *
 *  Register a handler.
 *
 *  @param [in] cfg config
 *  @param [out] hdlr handler
 *
 * @return 0 - OK, others - AMBA_IMG_ENCMONITOR_ER_CODE_e
 * @see AMBA_IMG_ENCMONITOR_ER_CODE_e
 */
extern int AmbaEncMonitorAQP_RegisterService(AMBA_IMG_ENC_MONITOR_AQP_HDLR_CFG_s *cfg, AMBA_IMG_ENC_MONITOR_AQP_HDLR_s **hdlr);

/**
 *  @brief UnRegister a BitRate Control Service handler
 *
 *  Register a handler.
 *
 *  @param [in] cfg config
 *  @param [out] hdlr handler
 *
 * @return 0 - OK, others - AMBA_IMG_ENCMONITOR_ER_CODE_e
 * @see AMBA_IMG_ENCMONITOR_ER_CODE_e
 */
extern int AmbaEncMonitorAQP_UnRegisterService(AMBA_IMG_ENC_MONITOR_AQP_HDLR_s *hdlr);

/**
 * Get encode monitor service AQP default config for an instance
 *
 * @param [out] defaultCfg encode monitor service AQP config
 *
 * @return 0 - OK, others - AMBA_IMG_SCHDLR_ER_CODE_e
 * @see AMBA_IMG_SCHDLR_ER_CODE_e
 */
extern int AmbaEncMonitorAQP_GetDefaultCfg(AMBA_IMG_ENC_MONITOR_AQP_HDLR_CFG_s *defaultCfg);

/**
 * Get encode monitor service AQP memory usage by given paramter
 *
 * @param [in] cfg memory query config
 * @param [out] memSize total memery size of given config
 *
 * @return 0 - OK, others - AMBA_IMG_ENCMONITOR_ER_CODE_e
 * @see AMBA_IMG_ENCMONITOR_ER_CODE_e
 */
extern int AmbaEncMonitorAQP_QueryMemsize(AMBA_IMG_ENC_MONITOR_AQP_MEMORY_s *cfg, UINT32 *memSize);

/**
 * Initialize Encode Monitor AQP Control Service .
 * The function should only be invoked once.
 * User MUST invoke this function before using Encode Monitor module.
 * The memory pool of the module will provide by user.
 * please be careful that this API shall be invoke after MW_INIT
 *
 * @param [in] cfg encode monitor aqp Service config
 *
 * @return 0 - OK, others - AMBA_IMG_ENCMONITOR_ER_CODE_e
 * @see AMBA_IMG_ENCMONITOR_ER_CODE_e
 */
extern int AmbaEncMonitorAQP_init(AMBA_IMG_ENC_MONITOR_AQP_INIT_CFG_s *cfg);

/**
 *  @brief Enable/Disable a Brc service
 *
 *  @param [in] hdlr service handler
 *  @param [in] enable enable or not
 *
 *  @return >=0 success, <0 failure
 */
extern int AmbaEncMonitorBRC_EnableService(AMBA_IMG_ENC_MONITOR_BITRATE_HDLR_s *hdlr, UINT32 enable);

/**
 *  @brief Register a BitRate Control Service handler
 *
 *  Register a handler.
 *
 *  @param [in] cfg config
 *  @param [out] hdlr handler
 *
 * @return 0 - OK, others - AMBA_IMG_ENCMONITOR_ER_CODE_e
 * @see AMBA_IMG_ENCMONITOR_ER_CODE_e
 */
extern int AmbaEncMonitorBRC_RegisterService(AMBA_IMG_ENC_MONITOR_BRC_HDLR_CFG_s *cfg, AMBA_IMG_ENC_MONITOR_BITRATE_HDLR_s **hdlr);

/**
 *  @brief UnRegister a BitRate Control Service handler
 *
 *  Register a handler.
 *
 *  @param [in] cfg config
 *  @param [out] hdlr handler
 *
 * @return 0 - OK, others - AMBA_IMG_ENCMONITOR_ER_CODE_e
 * @see AMBA_IMG_ENCMONITOR_ER_CODE_e
 */
extern int AmbaEncMonitorBRC_UnRegisterService(AMBA_IMG_ENC_MONITOR_BITRATE_HDLR_s *hdlr);

/**
 *  Run-time change bit rate settings
 *
 *  @param [in] cfg config
 *  @param [out] hdlr handler
 *
 * @return 0 - OK, others - AMBA_IMG_ENCMONITOR_ER_CODE_e
 * @see AMBA_IMG_ENCMONITOR_ER_CODE_e
 */
extern int AmbaEncMonitorBRC_RunTimeBitRateChange(AMBA_IMG_ENC_MONITOR_BRC_HDLR_CFG_s *cfg, AMBA_IMG_ENC_MONITOR_BITRATE_HDLR_s **hdlr);

/**
 * Get encode monitor service BitRate default config for an instance
 *
 * @param [out] defaultCfg encode monitor service BitRate config
 *
 * @return 0 - OK, others - AMBA_IMG_SCHDLR_ER_CODE_e
 * @see AMBA_IMG_SCHDLR_ER_CODE_e
 */
extern int AmbaEncMonitorBRC_GetDefaultCfg(AMBA_IMG_ENC_MONITOR_BRC_HDLR_CFG_s *defaultCfg);

/**
 * Get encode monitor memory usage by given paramter
 *
 * @param [in] cfg memory query config
 * @param [out] memSize total memery size of given config
 *
 * @return 0 - OK, others - AMBA_IMG_ENCMONITOR_ER_CODE_e
 * @see AMBA_IMG_ENCMONITOR_ER_CODE_e
 */
extern int AmbaEncMonitorBRC_QueryMemsize(AMBA_IMG_ENC_MONITOR_BRC_MEMORY_s *cfg, UINT32 *memSize);

/**
 * Initialize Encode Monitor BitRate Control Service .
 * The function should only be invoked once.
 * User MUST invoke this function before using Encode Monitor module.
 * The memory pool of the module will provide by user.
 * please be careful that this API shall be invoke after MW_INIT
 *
 * @param [in] cfg encode monitor BitRate Service config
 *
 * @return 0 - OK, others - AMBA_IMG_ENCMONITOR_ER_CODE_e
 * @see AMBA_IMG_ENCMONITOR_ER_CODE_e
 */
extern int AmbaEncMonitorBRC_init(AMBA_IMG_ENC_MONITOR_BITRATE_INIT_CFG_s *cfg);

#endif /* _ENC_MONITOR_SERVICE_H_ */

