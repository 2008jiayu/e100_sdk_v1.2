/**
 *  @file MkvEdt.h
 *
 *  MKV Editing format module header
 *
 *  @copyright 2014 Ambarella Corporation. All rights reserved.
 *                 No part of this file may be reproduced, stored in a retrieval system,
 *                 or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                 recording, or otherwise, without the prior consent of Ambarella Corporation.
 */
#ifndef __MKV_EDT_H__
#define __MKV_EDT_H__

/**
 * @addtogroup MKV
 * @{
 */

#include <format/Editor.h>

/**
 * The configuration for initializing MkvEdt module
 */
typedef struct {
    UINT8 *Buffer;      /**< The work buffer of the MkvEdt module */
    UINT32 BufferSize;  /**< The work buffer size of the module */
    UINT8 MaxHdlr;      /**< The maximum number of MkvEdt handlers */
} AMP_MKV_EDT_INIT_CFG_s;

/**
 * The configuration of MkvEdt handlers
 */
typedef struct {
    AMP_CALLBACK_f OnEvent;         /**< The event callback returning an execution result */
} AMP_MKV_EDT_CFG_s;

/**
* Get the required buffer size for initializing the MkvEdt module.
* @param [in] maxHdlr The maximum number of MkvEdt handlers
* @return The required buffer size
*/
extern UINT32 AmpMkvEdt_GetRequiredBufferSize(UINT8 maxHdlr);

/**
 * Get the default configuration for initializing the MkvEdt module.
 * @param [out] config The returned configuration
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpMkvEdt_GetInitDefaultCfg(AMP_MKV_EDT_INIT_CFG_s *config);

/**
 * Initialize the MkvEdt module.
 * @param [in] config The configuration used to initialize the module
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpMkvEdt_Init(AMP_MKV_EDT_INIT_CFG_s *config);

/**
 * Get the default configuration of a MkvEdt handler.
 * @param [out] config The returned configuration
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpMkvEdt_GetDefaultCfg(AMP_MKV_EDT_CFG_s *config);

/**
 * Create an MkvEdt handler.
 * @param [in] config The configuration used to create an MkvEdt handler
 * @param [out] hdlr The returned MkvEdt handler
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpMkvEdt_Create(AMP_MKV_EDT_CFG_s *config, AMP_EDT_FORMAT_HDLR_s **hdlr);

/**
 * Delete a MkvEdt handler.
 * @param [in] hdlr The MkvEdt handler being deleted
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpMkvEdt_Delete(AMP_EDT_FORMAT_HDLR_s *hdlr);

/**
 * @}
 */

#endif /* __MKV_EDT_H__ */
