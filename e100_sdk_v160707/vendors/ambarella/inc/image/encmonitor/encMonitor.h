 /**
  * @file encMonitor.h
  *
  * Encode monitor header
  *
  * Copyright (C) 2015, Ambarella, Inc.
  *
  * All rights reserved. No Part of this file may be reproduced, stored
  * in a retrieval system, or transmitted, in any form, or by any means,
  * electronic, mechanical, photocopying, recording, or otherwise,
  * without the prior consent of Ambarella, Inc.
  */

#ifndef _ENC_MONITOR_H_
#define _ENC_MONITOR_H_

#include <string.h>
#include "AmbaPrintk.h"

#define AMBA_ENCMONITOR_DEFAULT_MINIMUM_PERIOD 10 //10msec

/**
 * Error code
 */
typedef enum _AMBA_IMG_ENCMONITOR_ER_CODE_e_ {
    AMBA_ENCMONITOR_OK = 0,                          /**< OK */
    AMBA_ENCMONITOR_ERR_GENERAL = -1,                /**< General error */
    AMBA_ENCMONITOR_ERR_INPUT_PARAM_RANGE = -2,      /**< input parameter error */
    AMBA_ENCMONITOR_ERR_STATUS = -3,                 /**< status error */
} AMBA_IMG_ENCMONITOR_ER_CODE_e;

/**
 * Monitor Cyclic type
 */
typedef enum _AMBA_ENCMONITOR_CYCLIC_TYPE_e_ {
    AMBA_ENCMONITOR_TIMER = 0,           /**< Timer Base Monitor */
    AMBA_ENCMONITOR_VDSP,                /**< Vdsp Base Monitor */
    AMBA_ENCMONITOR_NONE,                /**< None */
} AMBA_ENCMONITOR_CYCLIC_TYPE_e;

/**
 * common handler structure for encode monitor
 */
typedef struct _AMBA_ENC_MONITOR_CYCLIC_HDLR_s_ {
    void *ctx;                      /**< pointer to monitor handler */
} AMBA_ENC_MONITOR_CYCLIC_HDLR_s;

/**
 *  Encode monitor handler
 */
typedef struct _AMBA_ENC_MONITOR_CYCLIC_HANDLER_s_ {
    void (*MonitorInit)(void *cbInfo);      /**< Invoke when enable handler */
    void (*TimeUpCallBack)(void *cbInfo);   /**< Invoke every time when time is up/Vdsp is coming */
    void *cbInfo;                           /**< user preference */
    UINT32 Period;                          /**< Time Period/Vdsp interval to trigger TimeUpCallCallBack() */
    UINT8 Priority;                         /**< CallBack priority (0~255), 0 is highest, shall smalle than MaxHandlrNum */
} AMBA_ENC_MONITOR_CYCLIC_HANDLER_s;

/**
 * Module initialization config
 */
typedef struct _AMBA_IMG_ENC_MONITOR_INIT_CFG_s_ {
    UINT8 *MemoryPoolAddr;              /**< encode monitor module working pool address */
    UINT32 MemoryPoolSize;              /**< encode monitor working pool size */
    UINT8 MaxTimerMonitorNumber;        /**< max. number for timer base monitor */
    UINT8 MaxVdspMonitorNumber;         /**< max. number for vdsp base monitor */
    UINT32 TimerMonitorTaskStackSize;   /**< timer base monitor stack size */
    UINT8 TimerMonitorTaskPriority;     /**< timer base monitor stack priority */
    UINT8 TimerMonitorTaskCoreSelection;/**< timer base monitor stack core selection */
    UINT32 VdspMonitorTaskStackSize;    /**< vdsp base monitor stack size */
    UINT8 VdspMonitorTaskPriority;      /**< vdsp base monitor stack priority */
    UINT8 VdspMonitorTaskCoreSelection; /**< vdsp base monitor stack core selection */
} AMBA_IMG_ENC_MONITOR_INIT_CFG_s;

/**
 * Memory query config
 */
typedef struct _AMBA_IMG_ENC_MONITOR_MEMORY_s_ {
    UINT8 MaxTimerMonitorNumber;        /**< max. number for timer base monitor */
    UINT8 MaxVdspMonitorNumber;         /**< max. number for vdsp base monitor */
    UINT32 TimerMonitorTaskStackSize;   /**< timer base monitor stack size */
    UINT32 VdspMonitorTaskStackSize;    /**< vdsp base monitor stack size */
} AMBA_IMG_ENC_MONITOR_MEMORY_s;

/**
 *  @brief Register a handler
 *
 *  Register a handler.
 *
 *  @param [in] type monitor type
 *  @param [in] hdlr handler body
 *  @param [out] cyclicHdlr handler
 *
 * @return 0 - OK, others - AMBA_IMG_ENCMONITOR_ER_CODE_e
 * @see AMBA_IMG_ENCMONITOR_ER_CODE_e
 */
extern int AmbaEncMonitorCyclic_Register(AMBA_ENCMONITOR_CYCLIC_TYPE_e type, AMBA_ENC_MONITOR_CYCLIC_HANDLER_s *hdlr, AMBA_ENC_MONITOR_CYCLIC_HDLR_s **cyclicHdlr);
    
/**
 *  @brief Unregister a handler
 *
 *  @param [in] type monitor type
 *  @param [in] cyclicHdlr handler
 *
 *  @return >=0 success, <0 failure
 */
extern int AmbaEncMonitorCyclic_Unregister(AMBA_ENC_MONITOR_CYCLIC_HDLR_s *cyclicHdlr);

/**
 *  @brief Enable/Disable a handler
 *
 *  @param [in] cyclicHdlr handler
 *  @param [in] enable enable or not
 *
 *  @return >=0 success, <0 failure
 */
extern int AmbaEncMonitorCyclic_Enable(AMBA_ENC_MONITOR_CYCLIC_HDLR_s *cyclicHdlr, UINT32 enable);

/**
 *  @brief Set handler period
 *
 *  @param [in] cyclicHdlr handler
 *  @param [in] period handler period
 *
 * @return 0 - OK, others - AMBA_IMG_ENCMONITOR_ER_CODE_e
 * @see AMBA_IMG_ENCMONITOR_ER_CODE_e
 */
extern int AmbaEncMonitorCyclic_SetHandlerPeriod(AMBA_ENC_MONITOR_CYCLIC_HDLR_s *cyclicHdlr, UINT32 period);

/**
 *  @brief Get handler period
 *
 *  @param [in] cyclicHdlr handler
 *  @param [out] period handler period
 *
 * @return 0 - OK, others - AMBA_IMG_ENCMONITOR_ER_CODE_e
 * @see AMBA_IMG_ENCMONITOR_ER_CODE_e
 */
extern int AmbaEncMonitorCyclic_GetHandlerPeriod(AMBA_ENC_MONITOR_CYCLIC_HDLR_s *cyclicHdlr, UINT32 *period);

/**
 * Get encode monitor memory usage by given paramter
 *
 * @param [in] cfg memory query config
 * @param [out] memSize total memery size of given config
 *
 * @return 0 - OK, others - AMBA_IMG_ENCMONITOR_ER_CODE_e
 * @see AMBA_IMG_ENCMONITOR_ER_CODE_e
 */
extern int AmbaEncMonitor_QueryMemsize(AMBA_IMG_ENC_MONITOR_MEMORY_s *cfg, UINT32 *memSize);

/**
 * Get encode monitor default config for initializing
 *
 * @param [out] cfg image scheduler module config
 *
 * @return 0 - OK, others - AMBA_IMG_ENCMONITOR_ER_CODE_e
 * @see AMBA_IMG_ENCMONITOR_ER_CODE_e
 */
extern int AmbaEncMonitor_GetInitDefaultCfg(AMBA_IMG_ENC_MONITOR_INIT_CFG_s *defInitCfg);

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
extern int AmbaEncMonitor_Init(AMBA_IMG_ENC_MONITOR_INIT_CFG_s *cfg);

#endif /* _ENC_MONITOR_H_ */

