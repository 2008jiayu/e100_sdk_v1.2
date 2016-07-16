 /**
  * @file inc/mw/recorder/TranscoderEnc.h
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

#ifndef _TRANSCODERENC_H_
#define _TRANSCODERENC_H_
/**
 * @defgroup TranscoderEnc
 * @brief Amba transcoder encode implementation
 *
 * Implementation the Amba Transcoder encode module, include below function implementation
 *  1. Init Amba Transcoder encode module function.
 *  2. Create Amba Transcoder encode module function.
 *  3. Delete Amba Transcoder encode module function.
 *  4. Get Amba Transcoder encode module information
 *
 */

/**
 * @addtogroup TranscoderEnc
 * @ingroup Codec
 * @{
 */

#include <mw.h>
#include <recorder/Encode.h>

/**
 *  init config
 */
typedef struct _AMP_TRANSCODERENC_INIT_CFG_s_ {
    UINT8* MemoryPoolAddr;           /**< working buffer start address */
    UINT32 MemoryPoolSize;           /**< size of the buffer */
} AMP_TRANSCODERENC_INIT_CFG_s;


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
 * TranscoderEnc handler config
 */
typedef struct AMP_TRANSCODERENC_HDLR_CFG_s_ {
    UINT8 EventDataReadySkipNum;            /**< fire "data ready" event to fifo per (1 + evenDataReadySkipNum) frames */
    AMP_CALLBACK_f cbEvent;                 /**< Event related to the encoder */
    AMP_Transcoder_CALLBACK_f cbTranscoder;            /**< callback for transcode data */
    AMP_ENC_BITSBUFFER_CFG_s BitsBufCfg;       /**< buffer config */
    void *src_fifo;        /**< address of fifo of original data*/
    UINT32 SrcNumEntries;     /**< Number of entries of fifo of original data*/
    UINT8 reserve[3];
} AMP_TRANSCODERENC_HDLR_CFG_s;

/**
 * Get Transcoder encode module default config for initialization
 *
 * @param [out] cfg Transcoder encode module config
 *
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpTranscoderEnc_GetInitDefaultCfg(AMP_TRANSCODERENC_INIT_CFG_s *defInitCfg);

/**
 * Initialize Transcoder encode module.\n
 * The function should only be invoked once.
 * User MUST invoke this function before using Transcoder encode module.
 * The memory pool of the module will provide by user.
 *
 * @param [in] cfg Transcoder encode module config
 *
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpTranscoderEnc_Init(AMP_TRANSCODERENC_INIT_CFG_s *cfg);

/**
 * Get full default setting of the Transcoder codec
 *
 * @param [out] defCfg default config
 *
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpTranscoderEnc_GetDefaultCfg(AMP_TRANSCODERENC_HDLR_CFG_s *defCfg);

/**
 * Create an Transcoder handler and set initial configuration
 *
 * @param [in] cfg initial configuration
 * @param [out] Hdlr Transcoder encoder instance
 *
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpTranscoderEnc_Create(AMP_TRANSCODERENC_HDLR_CFG_s *cfg, AMP_AVENC_HDLR_s **extencHdlr);

/**
 * Delete an Transcoder handler
 *
 * @param [in] Hdlr Transcoder encoder instance
 *
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpTranscoderEnc_Delete(AMP_AVENC_HDLR_s *hdlr);


#endif /* _TRANSCODERENC_H_ */
