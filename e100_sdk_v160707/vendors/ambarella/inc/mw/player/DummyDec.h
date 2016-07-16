/**
 * @file inc/mw/player/dummydec.h
 *
 * Amba dummy decoder header
 *
 * Copyright (C) 2013, Ambarella, Inc.
 *
 * All rights reserved. No Part of this file may be reproduced, stored
 * in a retrieval system, or transmitted, in any form, or by any means,
 * electronic, mechanical, photocopying, recording, or otherwise,
 * without the prior consent of Ambarella, Inc.
 */

#ifndef DUMMYDEC_H_
#define DUMMYDEC_H_

#include <mw.h>
#include <player/Decode.h>
#include <fifo/Fifo.h>
#include "AmbaDataType.h"
#include "AmbaKAL.h"
#include "AmbaPrintk.h"

/**
 * @defgroup DummyDec
 * @brief Dummy decoder function sets
 *
 * Dummy decoder is a decoder that only consume frame without doing anything.\n
 * Used for testing.
 */

/**
 * @addtogroup DummyDec
 * @ingroup Codec
 * @{ 
 */

typedef struct _AMP_DUMMYDEC_CFG_s_ {
#define DUMMY_MAX_FILENAME_LENGTH (64)
    AMP_YUV_COLOR_s BgColor;        ///< background color
    AMP_CALLBACK_f CbCfgUpdated;    ///< config updated cb function
    AMP_CALLBACK_f CbCodecEvent;    ///< codec state cb function
    char* RawBuffer;                ///< raw buffer for coded data
    UINT32 RawBufferSize;           ///< size of raw buffer
    char* DescBuffer;               ///< desc buffer
    UINT32 NumDescBuffer;           ///< size of desc buffer
} AMP_DUMMYDEC_CFG_s;

typedef struct _AMP_DUMMYDEC_HDLR_s_ {
    void *CodecCtx;                         ///< pointer to codec context
    AMP_AVDEC_CODEC_s *CodecOpFunc;  ///< pointer to codec operation functions
} AMP_DUMMYDEC_HDLR_s;

/**
 * To get default config of a dummy video decode codec
 *
 * @param [out] cfg - codec config
 *
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpDummyDec_GetDefaultCfg(AMP_DUMMYDEC_CFG_s *cfg);

/**
 * To create a dummy video decode codec
 *
 * @param [in] cfg - codec config
 * @param [out] hdlr - video codec handler
 *
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpDummyDec_Create(AMP_DUMMYDEC_CFG_s *cfg,
                              AMP_AVDEC_HDLR_s **hdlr);

/**
 * To delete a dummy video decode codec
 *
 * @param [in] hdlr - video codec handler
 *
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpDummyDec_Delete(AMP_AVDEC_HDLR_s *hdlr);

/**
 * @}
 */

#endif /* DUMMYDEC_H_ */
