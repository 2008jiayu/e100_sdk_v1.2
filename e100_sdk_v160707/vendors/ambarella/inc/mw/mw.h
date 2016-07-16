 /**
  * @file inc/mw/mw.h
  *
  * Basic MW definitions
  *
  * Copyright (C) 2013, Ambarella, Inc.
  *
  * All rights reserved. No Part of this file may be reproduced, stored
  * in a retrieval system, or transmitted, in any form, or by any means,
  * electronic, mechanical, photocopying, recording, or otherwise,
  * without the prior consent of Ambarella, Inc.
  */
/**
 * @mainpage Ambarella A9 SDK
 *
 * @par Introduction
 * The Ambarella A9 SDK
 *
 */

/**
 * @defgroup Initialization
 * @brief Head of Middleware of A9SDK
 *
 * Middleware level API A9SDK.\n
 * Middleware utilizes SSP (System Support Package) with more convenient APIs which can make customers easy and fast
 * develop product softwares. Initialization APIs are put here.
 */

/**
 * @defgroup FlowPipeline
 * @brief Function (encode/decode...) flow pipelines
 *
 * Flow pipeline manages a group of codecs which take actions together (sharing the same state)
 */
/**
 * @defgroup Codec
 * @brief Encoder/Decoder engines
 *
 * Basic unit of encoder/decoder implementation. Each codec can do a specific function such as video encoding, audio encoding, or video decoding...,etc.
 */

/**
  * @defgroup  DataPipeline
  * @brief Data pipelines.
  *
  * Data pipeline manages the data for Stream FIFO ...,etc.
  */


/**
 * @addtogroup Initialization
 * @{
 */

#ifndef _MW_H_
#define _MW_H_

#include "AmbaSSP.h"
#include <common/common.h>
#include "AmbaDSP_ImgDef.h"
#include "AmbaDSP_ImgUtility.h"
#include "w_char.h"
//#include "AmbaSensor.h"

#define AMP_VIN_EVENT_START_NUM         (0x01000000)
#define AMP_ENC_EVENT_START_NUM         (0x02000000)
#define AMP_DEC_EVENT_START_NUM         (0x03000000)
#define AMP_FIFO_EVENT_START_NUM        (0x04000000)
#define AMP_DISPLAY_EVENT_START_NUM     (0x05000000)
#define AMP_FROMAT_EVENT_START_NUM      (0x06000000)
#define AMP_STREAM_EVENT_START_NUM      (0x07000000)
#define AMP_IMG_EVENT_START_NUM         (0x08000000)
#define AMP_IMGALGO_EVENT_START_NUM     (0x09000000)
#define AMP_DFC_EVENT_START_NUM         (0x0A000000)
#define AMP_CALIB_EVENT_START_NUM       (0x0B000000)
#define AMP_CFS_EVENT_START_NUM         (0x0C000000)
#define AMP_GRAPHICS_EVENT_START_NUM    (0x0D000000)
#define AMP_TRANSCODER_EVENT_START_NUM  (0x0E000000)

#define FATAL_ERROR(er, msg)    if(er!=AMP_OK){AmbaPrint("FATAL ERROR : %s", msg);}

typedef struct _AMP_MW_INIT_CFG_s_{
    UINT8 *DspDbgLogDataArea;       ///< align 32
    UINT32 SizeDspDbgLogDataArea;
    UINT8 *DspWorkingArea;          ///< align 32
    UINT32 SizeDspWorkArea;
    UINT8 *ImgKernelWorkArea;       ///< align 32
    UINT32 SizeImgKernelWorkArea;
    UINT16 NumImgKernelPipe;
    UINT16 NumImgKernelBufNum;
    AMBA_DSP_IMG_PIPE_INFO_s *ImgKernelPipeInfo;
    void *ImgKernelDefaultTable;
    UINT32 DspIntTaskPriority;
    UINT32 DspIntTaskCoreSelectionBitMap;
    UINT32 DspCommTaskPriority;
    UINT32 DspCommTaskCoreSelectionBitMap;
    UINT32 DspCmdTaskPriority;
    UINT32 DspCmdTaskCoreSelectionBitMap;
    UINT32 DspMsgTaskPriority;
    UINT32 DspMsgTaskCoreSelectionBitMap;
    UINT8  EnSeamless;              ///< Whether seamless mode is enabled. 1: Seamless enabled. 0: Seamless disabled
}AMP_MW_INIT_CFG_s;

/**
 * Get MiddleWare default initial configuration
 *
 * @param [out] cfg Default built-in init config
 *
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpMW_GetDefaultInitCfg(AMP_MW_INIT_CFG_s *cfg);

/**
 * Initialize MiddleWare
 *
 * @param [in] cfg init config
 *
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpMW_Init(AMP_MW_INIT_CFG_s *cfg);

/**
 *
 * Set dsp working area information
 *
 * @param [in] buf buffer address
 * @param [in] size buffer size
 *
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpResource_SetDspWorkArea(UINT8 *buf, UINT32 size);

/**
 * Get dsp background process mode
 *
 * @return dsp background process mode
 */
extern int AmpResource_GetDspBackgroundProcMode(void);

/**
 * Get Total Iso config number of certain pipe in ImageKernel
 *
 * @param [in] pipe pipeline
 *
 * @return total iso config number
 */
extern int AmpResource_GetIKIsoConfigNumber(AMBA_DSP_IMG_PIPE_e pipe);

/**
 * Get vout seamless feature enable status
 *
 * @return 0 - disable, 1 - enable
 */
extern int AmpResource_GetVoutSeamlessEnable(void);

/**
 * Get the value to which seamless common LCD buffer pitch should align.
 *
 * @param [out] Video process Mode
 *
 * @return 0 - OK, others - AMP_ER_CODE_e
 */
extern int AmpResource_GetVout0SeamlessBufferPitchAlign(UINT32 *PitchAlign);

/**
 * Get flow pipe svn version
 *
 * @return SVN version
 */
extern int AmpMW_GetFlowPipeVer(void);

/**
 * get data pipe svn version
 *
 * @return SVN version
 */
extern int AmpMW_GetDataPipeVer(void);


/**
 * @}
 */

#endif
