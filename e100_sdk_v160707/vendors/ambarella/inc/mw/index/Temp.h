/**
 *  @file Temp.h
 *
 *  encode common function header
 *
 *  @copyright 2013 Ambarella Corporation. All rights reserved.
 *                 No part of this file may be reproduced, stored in a retrieval system,
 *                 or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                 recording, or otherwise, without the prior consent of Ambarella Corporation.
 */
#ifndef __TEMP_H__
#define __TEMP_H__

/**
 * @addtogroup Index
 * @{
 */

#include <index/Index.h>

#define AMP_TEMP_IDX_MAX_FNEXT_LENGTH    8               /**< The maximum length of a file extension */

/**
 *  The configuration for initializing the Temp Index module
 */
typedef struct {
    char Ext[AMP_TEMP_IDX_MAX_FNEXT_LENGTH];   /**< The extension name of an Index file */
    UINT8 *Buffer;                              /**< The work buffer of the Temp Index module */
    UINT32 BufferSize;                          /**< The size of the work buffer */
    UINT8 MaxHdlr;                              /**< The maximum number of Temp Index handlers held in the Temp Index module */
} AMP_TEMP_IDX_INIT_CFG_s;

/**
 * The configuration of Temp indexes
 */
typedef struct {
    UINT8 Resv[4];  /**< Reserved */
} AMP_TEMP_IDX_CFG_s;

/**
 * Get the required buffer size for initializing the Temp Index module.
 * @param [in] maxHdlr The maximum number of Temp Index handlers
 * @return The required buffer size
 */
UINT32 AmpTempIdx_GetRequiredBufferSize(UINT8 maxHdlr);

/**
 * Get the default configuration for initializing the Temp Index module.
 * @param [out] config The returned configuration
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpTempIdx_GetInitDefaultCfg(AMP_TEMP_IDX_INIT_CFG_s *config);

/**
 * Initialize the Temp Index module.
 * @param [in] config The configuration used to initialize the Temp Index module
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpTempIdx_Init(AMP_TEMP_IDX_INIT_CFG_s *config);

/**
 * Get the default configuration for creating a Temp Index handler.
 * @param [out] config The returned configuration
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpTempIdx_GetDefaultCfg(AMP_TEMP_IDX_CFG_s *config);

/**
 * Create a Temp Index handler.
 * @param [in] config The configuration used to create a Temp Index handler
 * @param [out] hdlr The returned Temp Index handler
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpTempIdx_Create(AMP_TEMP_IDX_CFG_s *config, AMP_INDEX_HDLR_s **hdlr);

/**
 * Delete a Temp Index handler.
 * @param [in] hdlr The Temp Index handler being deleted
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpTempIdx_Delete(AMP_INDEX_HDLR_s *hdlr);

/**
 * @}
 */
#endif/* __TEMP_H__ */

