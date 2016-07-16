 /**
  * @file inc/mw/transcoder/DecTranscoder.h
  *
  * MW internal transcoder codec header
  *
  * Copyright (C) 2014, Ambarella, Inc.
  *
  * All rights reserved. No Part of this file may be reproduced, stored
  * in a retrieval system, or transmitted, in any form, or by any means,
  * electronic, mechanical, photocopying, recording, or otherwise,
  * without the prior consent of Ambarella, Inc.
  */

#ifndef _DEC_TRANSCODER_H_
#define _DEC_TRANSCODER_H_

#include <fifo/Fifo.h>
#include <player/VideoDec.h>
#include <stdio.h>
#include <string.h>
#include <mw.h>
#include <AmbaFS.h>
#include <AmbaKAL.h>
#include <AmbaDSP_EventInfo.h>

/**
 * @addtogroup Transcoder
 * @ingroup Codec
 * @{
 */

typedef struct _AMP_DEC_TRANSCODER_INIT_CFG_s_{
    /** task info for transcoder task */
    AMP_TASK_INFO_s TaskInfo;
    /** working buffer size */
    UINT32 WorkingBufSize;
    /** working buffer address */
    UINT8* WorkingBuf;
}AMP_DEC_TRANSCODER_INIT_CFG_s;

typedef struct _AMP_DEC_TRANSCODER_CB_INFO_s_{
    AMP_BITS_DESC_s SrcData;
    UINT8* SrcDataBase;
    UINT8* SrcDataLimit;
    AMP_FIFO_HDLR_s* DstFifo;  
}AMP_DEC_TRANSCODER_CB_INFO_s;

typedef struct _AMP_DEC_TRANSCODER_CFG_s_{
    /** callback function for transcode */
    AMP_CALLBACK_f CbTranscode;

    /** buffer for raw file as file reading buffer */
    UINT8* RawBuffer;

    /** size of raw buffer */
    UINT32 RawBufferSize;

    /** target codec of trancode result, null to skip frame sent flow*/
    AMP_AVDEC_HDLR_s* DstCodec;

    /** fifo type of target codec*/
    UINT8 DstCodecType;

    /** number of src fifo entries*/
    UINT32 SrcFifoNumEntries;

    /** number of dst fifo entries*/
    UINT32 DstFifoNumEntries;
}AMP_DEC_TRANSCODER_CFG_s;

/**
 * To get the default value of initial config for video decode transcoder module
 * 
 * @param [out] cfg - default module config
 * 
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpDecTranscoder_GetInitDefaultCfg(AMP_DEC_TRANSCODER_INIT_CFG_s *cfg);

/**
 * To set the initial config for video decode module
 *
 * @param [in] cfg - module config
 *
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpDecTranscoder_Init(AMP_DEC_TRANSCODER_INIT_CFG_s *cfg);

/**
 * To get Video decode transcoder codec default config
 *
 * @param [out] cfg - default codec config
 *
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpDecTranscoder_GetDefaultCfg(AMP_DEC_TRANSCODER_CFG_s *cfg);

/**
 * To create Video decode transcoder codec
 *
 * @param [in] cfg - codec config
 * @param [out] hdlr - video codec handler
 *
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpDecTranscoder_Create(AMP_DEC_TRANSCODER_CFG_s *cfg,
                                   AMP_AVDEC_HDLR_s **hdlr);

/**
 * To delete transcoder codec
 *
 * @param [in] hdlr - codec handler
 *
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpDecTranscoder_Delete(AMP_AVDEC_HDLR_s *hdlr);

/**
 * @}
 */

#endif // __DEC_TRANSCODER_H_
