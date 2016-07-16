/**
 *  @file Mem.h
 *
 *  encode common function header
 *
 *  @copyright 2013 Ambarella Corporation. All rights reserved.
 *                 No part of this file may be reproduced, stored in a retrieval system,
 *                 or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                 recording, or otherwise, without the prior consent of Ambarella Corporation.
 */
#ifndef __MEM_H__
#define __MEM_H__

/**
 * @addtogroup Index
 * @{
 */

#include <index/Index.h>

/**
 *  The configuration for initializing the MEM Index module
 */
typedef struct {
    UINT8 *Buffer;      /**< The work buffer of the MEM Index module */
    UINT32 BufferSize;  /**< The size of the work buffer */
    UINT8 MaxHdlr;      /**< The maximum number of MEM Index handlers held in the MEM Index module */
} AMP_MEM_IDX_INIT_CFG_s;

/**
 * The configuration of MEM indexes
 */
typedef struct {
    UINT8 Resv[4];  /**< Reserved */
} AMP_MEM_IDX_CFG_s;

/**
 * Get the required buffer size for initializing the MEM Index module.
 * @param [in] maxHdlr The maximum number of MEM Index handlers
 * @param [in] memSize The size of memory buffer
 * @return The required buffer size
 */
UINT32 AmpMemIdx_GetRequiredBufferSize(UINT8 maxHdlr, UINT32 memSize);

/**
 * Get the default configuration for initializing the MEM Index module.
 * @param [out] config The returned configuration
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpMemIdx_GetInitDefaultCfg(AMP_MEM_IDX_INIT_CFG_s *config);

/**
 * Initialize the MEM Index module.
 * @param [in] config The configuration used to initialize the MEM Index module
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpMemIdx_Init(AMP_MEM_IDX_INIT_CFG_s *config);

/**
 * Get the default configuration for creating an MEM Index handler.
 * @param [out] config The returned configuration
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpMemIdx_GetDefaultCfg(AMP_MEM_IDX_CFG_s *config);

/**
 * Create an MEM Index handler.
 * @param [in] config The configuration used to create an MEM index handler
 * @param [out] hdlr The returned MEM index handler
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpMemIdx_Create(AMP_MEM_IDX_CFG_s *config, AMP_INDEX_HDLR_s **hdlr);

/**
 * Delete an MEM Index handler.
 * @param [in] hdlr The MEM index handler being deleted
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpMemIdx_Delete(AMP_INDEX_HDLR_s *hdlr);

/**
 * @}
 */
#endif/* __MEM_H__ */

