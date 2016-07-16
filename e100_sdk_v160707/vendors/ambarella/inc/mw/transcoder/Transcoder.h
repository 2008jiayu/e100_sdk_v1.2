 /**
  * @file inc/mw/transcoder/Transcoder.h
  *
  * Amba transcoder codec header
  *
  * Copyright (C) 2014, Ambarella, Inc.
  *
  * All rights reserved. No Part of this file may be reproduced, stored
  * in a retrieval system, or transmitted, in any form, or by any means,
  * electronic, mechanical, photocopying, recording, or otherwise,
  * without the prior consent of Ambarella, Inc.
  */

#ifndef _TRANSCODER_H_
#define _TRANSCODER_H_
/**
 * @defgroup Transcoder
 * @brief Amba transcoder encode implementation
 *
 * Implementation the Amba Transcoder encode module, include below function implementation
 *  1. Init Amba Transcoder module function.
 *  2. Create Amba Transcoder module function.
 *  3. Delete Amba Transcoder module function.
 *  4. Start/Stop/Pause/Resume Transcoder
 *  5. Get Amba Transcoder module information
 *
 */

/**
 * @addtogroup Transcoder
 * @ingroup Codec
 * @{
 */

#include <mw.h>
#include <AmbaPrintk.h>
#include "AmbaSensor.h"

/**
 * Transcoder callback event
 */
typedef enum _AMP_TRANSCODER_EVENT_e_ {
    // Codec event
    AMP_TRANSCODER_EVENT_DATA_OVER_RUNOUT_THRESHOLD = AMP_TRANSCODER_EVENT_START_NUM, /**< buffer data over runout threshold */
    AMP_TRANSCODER_EVENT_DATA_OVERRUN,             /**< buffer data overrun */
    AMP_TRANSCODER_EVENT_DESC_OVERRUN,             /**< buffer data overrun */
    // Pipe event
    AMP_TRANSCODER_EVENT_STATE_CHANGED             /**< event state changed. codec function may be async function and the event is for user to know the processing is done. */
} AMP_TRANSCODER_EVENT_e;

/**
 * bitstream buffer config
 */
typedef struct AMP_TRANSCODER_BITSBUF_CFG_s {
    UINT8  *BitsBufAddr;            /**< Bitstream buffer address */
    UINT32  BitsBufSize;            /**< Bitstream buffer size. Must be multiple of 16384 */
    UINT8  *DescBufAddr;            /**< Descriptor buffer address */
    UINT32  DescBufSize;            /**< Descriptor buffer size */
    UINT32  BitsRunoutThreshold;    /**< indicate Bitstream's capacity is alomost full, Byte Unit */
} AMP_TRANSCODER_BITSBUF_CFG_s;


/**
 * Transcoder pipeline state
 */
typedef enum _AMP_TRANSCODER_STATE_e_ {
    AMP_TRANSCODER_STATE_IDLE,                /**< idle state */
    AMP_TRANSCODER_STATE_TRANSCODING,           /**< TRANSCODE TRANSCODING state */
    AMP_TRANSCODER_STATE_UNKNOWN = 0xFF
} AMP_TRANSCODER_STATE_e;

/**
 *  init config
 */
typedef struct _AMP_TRANSCODER_INIT_CFG_s_ {
    UINT8* MemoryPoolAddr;           /**< working buffer start address */
    UINT32 MemoryPoolSize;           /**< size of the buffer */
} AMP_TRANSCODER_INIT_CFG_s;


/**
 * TranscoderEnc DESC info
 */
typedef struct _AMBA_TRANSCODER_DESC_s_ {
    UINT32 DataSize;        /* Valid data size, from starting address */
    UINT32 pBufAddr;          /* Buffer starting address */
} AMBA_TRANSCODER_DESC_s;

/**
 * amba TRANSCODER callback
 */
typedef int (*AMP_Transcoder_CALLBACK_f)(UINT32 ori_size, UINT8* ori_data, UINT32 *transcode_size, UINT8** transcode_data);

/**
 * Transcoder handler config
 */
typedef struct AMP_TRANSCODER_HDLR_CFG_s_ {
    UINT8 EventDataReadySkipNum;            /**< fire "data ready" event to fifo per (1 + evenDataReadySkipNum) frames */
    AMP_CALLBACK_f cbEvent;                 /**< Event related to the encoder */
    AMP_Transcoder_CALLBACK_f cbTranscoder;            /**< callback for transcode data */
    AMP_TRANSCODER_BITSBUF_CFG_s BitsBufCfg;       /**< buffer config */
    void *src_fifo;        /**< address of fifo of original data*/
    UINT32 SrcNumEntries;     /**< Number of entries of fifo of original data*/
    AMP_TRANSCODER_STATE_e state; /**< pipeline state */
    UINT8 reserve[3];
} AMP_TRANSCODER_HDLR_CFG_s;

/**
 * Get Transcoder module default config for initialization
 *
 * @param [out] cfg Transcoder encode module config
 *
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpTranscoder_GetInitDefaultCfg(AMP_TRANSCODER_INIT_CFG_s *defInitCfg);

/**
 * Initialize Transcoder module.\n
 * The function should only be invoked once.
 * User MUST invoke this function before using Transcoder encode module.
 * The memory pool of the module will provide by user.
 *
 * @param [in] cfg Transcoder encode module config
 *
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpTranscoder_Init(AMP_TRANSCODER_INIT_CFG_s *cfg);

/**
 * Get full default setting of the Transcoder codec
 *
 * @param [out] defCfg default config
 *
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpTranscoder_GetDefaultCfg(AMP_TRANSCODER_HDLR_CFG_s *defCfg);

/**
 * Create an Transcoder handler and set initial configuration
 *
 * @param [in] cfg initial configuration
 * @param [out] transcoderHdlr Transcoder instance
 *
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpTranscoder_Create(AMP_TRANSCODER_HDLR_CFG_s *cfg, void **transcoderHdlr);

/**
 * Delete an Transcoder handler
 *
 * @param [in] Hdlr Transcoder instance
 *
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpTranscoder_Delete(void *hdlr);

/**
 * Standard codec operation: open
 *
 * @param [in] Hdlr instance
 *
 * @return 0 - success, -1 - fail
 */
extern int AmpTranscoder_Open(void *hdlr);

/**
 * Standard codec operation: close
 *
 * @param [in] Hdlr encoder instance
 *
 * @return 0 - success, -1 - fail
 */
extern int AmpTranscoder_Close(void *hdlr);

/**
 * Standard codec operation: encode start
 *
 * @param [in] Hdlr encoder instance
 *
 * @return 0 - success, -1 - fail
 */
extern int AmpTranscoder_Start(void *hdlr);

/**
 * Standard codec operation: encode stop
 *
 * @param [in] Hdlr encoder instance
 *
 * @return 0 - success, -1 - fail
 */
extern int AmpTranscoder_Stop(void *hdlr);


#endif /* _TRANSCODER_H_ */
