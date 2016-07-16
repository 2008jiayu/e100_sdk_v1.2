/**
 *  @file DummyEnc.h
 *
 *  @copyright 2013 Ambarella Corporation. All rights reserved.
 *                 No part of this file may be reproduced, stored in a retrieval system,
 *                 or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                 recording, or otherwise, without the prior consent of Ambarella Corporation.
 */

#ifndef DUMMYENC_H_
#define DUMMYENC_H_

#include <mw.h>
#include <recorder/Encode.h>
#include <fifo/Fifo.h>
#include "AmbaDataType.h"
#include "AmbaKAL.h"
#include "AmbaPrintk.h"

#define DUMMY_MAX_FILENAME_LENGTH (64)

/**
 * @defgroup DummyEnc
 * @brief Sample Encode implementation
 *
 * Implementation the sample encode module, include below function implementation
 *  1. Create sample encode module function.
 *  2. Delete sample encode module function.
 *
 */

/**
 * @addtogroup DummyEnc
 * @ingroup Codec
 * @{
 */

/**
 * encode video config
 */
typedef struct _AMP_DUMMYENC_CFG_s_ {
    AMP_YUV_COLOR_s bgColor;  ///< back ground color
    AMP_CALLBACK_f cbCfgUpdated; ///< callback when config updated
    AMP_CALLBACK_f cbCodecEvent; ///< callback event
    WCHAR rawFn[DUMMY_MAX_FILENAME_LENGTH]; ///< raw data file name
    WCHAR idxFn[DUMMY_MAX_FILENAME_LENGTH]; ///< index data file name
    UINT8 idrItvl; ///< IDR interval
    char* rawBuffer; ///< raw data buffer address
    UINT32 rawBufferSize; ///< raw data buffer size
    char* descBuffer;  ///< descriptor buffer
    UINT32 numDescBuffer; ///< descriptor buffer entry
    UINT32 frmDly;  ///< frame sent delay
    UINT32 memRunOutAlarmThr; ///< runout memory threshold
} AMP_DUMMYENC_CFG_s;

/**
 * get full default setting of the module
 *
 * @param cfg
 *
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpDummyEnc_GetDefaultCfg(AMP_DUMMYENC_CFG_s *cfg);

/**
 * To set the initial config for video encode module
 *
 * @param cfg
 *
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpDummyEnc_Create(AMP_DUMMYENC_CFG_s *cfg,
                                 AMP_AVENC_HDLR_s **hdlr);

/**
 *
 *
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpDummyEnc_Delete(AMP_AVENC_HDLR_s *hdlr);

/**
 * @}
 */

#endif /* DUMMYENC_H_ */
