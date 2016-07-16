 /**
  * @file inc/mw/recorder/ExtEnc.h
  *
  * Amba ext codec header
  *
  * Copyright (C) 2014, Ambarella, Inc.
  *
  * All rights reserved. No Part of this file may be reproduced, stored
  * in a retrieval system, or transmitted, in any form, or by any means,
  * electronic, mechanical, photocopying, recording, or otherwise,
  * without the prior consent of Ambarella, Inc.
  */

#ifndef _EXTENC_H_
#define _EXTENC_H_
/**
 * @defgroup ExtEnc
 * @brief Amba ext encode implementation
 *
 * Implementation the Amba ext encode module, include below function implementation
 *  1. Init Amba ext encode module function.
 *  2. Create Amba ext encode module function.
 *  3. Delete Amba ext encode module function.
 *  4. Get Amba ext encode module information
 *
 */

/**
 * @addtogroup ExtEnc
 * @ingroup Codec
 * @{
 */

#include <mw.h>
#include <recorder/Encode.h>

/**
 *  init config
 */
typedef struct _AMP_EXTENC_INIT_CFG_s_ {
    UINT8* MemoryPoolAddr;           /**< working buffer start address */
    UINT32 MemoryPoolSize;           /**< size of the buffer */
} AMP_EXTENC_INIT_CFG_s;


/**
 * ExtEnc DESC info
 */
typedef struct _AMBA_EXT_DESC_s_ {
    UINT32 DataSize;        /* Valid data size, from starting address */
    UINT32 pBufAddr;          /* Buffer starting address */
} AMBA_EXT_DESC_s;

/**
 * amba Ext info callback
 */
typedef int (*AMP_Extinfo_CALLBACK_f)(UINT32 *size, UINT8** ptr);

/**
 * ExtEnc handler config
 */
typedef struct AMP_EXTENC_HDLR_CFG_s_ {
    UINT8 EventDataReadySkipNum;            /**< fire "data ready" event to fifo per (1 + evenDataReadySkipNum) frames */
    UINT8 EncoderTaskPriority;              /**< Ext encoder task priority */
    UINT8 InputTaskPriority;                /**< Ext input receiver task priority */
    AMP_TASK_INFO_s MainTaskInfo;       /**< general task information */
    AMBA_KAL_TASK_t ExtTask;            /**< general task get */
    UINT16 rate;                      /**< rate of framerate of ext */
    UINT16 scale;                      /**< scale of framerate of ext */
    UINT32 time_pile;                    /**< start enc time of ext */
    UINT32 cur_time;                    /**< system current time of ext */
    UINT32 frm_cnt;                        /**< enc frame number of ext */
    AMP_CALLBACK_f cbEvent;                 /**< Event related to the encoder */
    AMP_Extinfo_CALLBACK_f cbExtGetInfo;            /**< callback for ext data get*/
    AMP_ENC_BITSBUFFER_CFG_s BitsBufCfg;       /**< buffer config */
} AMP_EXTENC_HDLR_CFG_s;

/**
 * Configure an existing Ext handler
 */
typedef struct _AMP_EXTENC_CFG_s_ {
    UINT32 EventDataReadySkipNum;           /**< fire "data ready" event to fifo per (1 + evenDataReadySkipNum) frames */
    UINT16 rate;                      /**< rate of framerate of ext */
    UINT16 scale;                      /**< scale of framerate of ext */
} AMP_EXTENC_CFG_s;

/**
 * Get ext encode module default config for initialization
 *
 * @param [out] cfg ext encode module config
 *
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpExtEnc_GetInitDefaultCfg(AMP_EXTENC_INIT_CFG_s *defInitCfg);

/**
 * Initialize ext encode module.\n
 * The function should only be invoked once.
 * User MUST invoke this function before using ext encode module.
 * The memory pool of the module will provide by user.
 *
 * @param [in] cfg ext encode module config
 *
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpExtEnc_Init(AMP_EXTENC_INIT_CFG_s *cfg);

/**
 * Get full default setting of the ext codec
 *
 * @param [out] defCfg default config
 *
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpExtEnc_GetDefaultCfg(AMP_EXTENC_HDLR_CFG_s *defCfg);

/**
 * Create an ext handler and set initial configuration
 *
 * @param [in] cfg initial configuration
 * @param [out] Hdlr ext encoder instance
 *
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpExtEnc_Create(AMP_EXTENC_HDLR_CFG_s *cfg, AMP_AVENC_HDLR_s **extencHdlr);

/**
 * Delete an ext handler
 *
 * @param [in] Hdlr ext encoder instance
 *
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpExtEnc_Delete(AMP_AVENC_HDLR_s *hdlr);

/**
 * Config ext encode handler
 *
 * @param [in] Hdlr ext encoder instance
 * @param [in] cfg encoder config
 *
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpExtEnc_Config(AMP_AVENC_HDLR_s *hdlr, AMP_EXTENC_CFG_s *cfg);

#endif /* _EXTENC_H_ */

