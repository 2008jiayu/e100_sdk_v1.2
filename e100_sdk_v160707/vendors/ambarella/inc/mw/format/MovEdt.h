/**
 *  @file MovEdt.h
 *
 *  MOV Editing format module header
 *
 *  @copyright 2015 Ambarella Corporation. All rights reserved.
 *                 No part of this file may be reproduced, stored in a retrieval system,
 *                 or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                 recording, or otherwise, without the prior consent of Ambarella Corporation.
 */
#ifndef __MOV_EDT_H__
#define __MOV_EDT_H__

/**
 * @addtogroup MOV
 * @{
 */

#include <format/Editor.h>

/**
 * The configuration for initializing MovEdt module
 */
typedef struct {
    UINT8 *Buffer;      /**< The work buffer of the MovEdt module */
    UINT32 BufferSize;  /**< The work buffer size of the module */
    UINT8 MaxHdlr;      /**< The maximum number of MovEdt handlers */
} AMP_MOV_EDT_INIT_CFG_s;

/**
 * The configuration of MovEdt handlers
 */
typedef struct {
    AMP_INDEX_HDLR_s *Index;        /**< Index handler */
    AMP_CALLBACK_f OnEvent;         /**< The event callback returning an execution result */
    BOOL8 EnableMoovHead;           /**< The flag to enable the flow of putting MOOV before MDAT */
} AMP_MOV_EDT_CFG_s;

/**
* Get the required buffer size for initializing the MovEdt module.
* @param [in] maxHdlr The maximum number of MovEdt handlers
* @return The required buffer size
*/
extern UINT32 AmpMovEdt_GetRequiredBufferSize(UINT8 maxHdlr);

/**
 * Get the default configuration for initializing the MovEdt module.
 * @param [out] config The returned configuration
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpMovEdt_GetInitDefaultCfg(AMP_MOV_EDT_INIT_CFG_s *config);

/**
 * Initialize the MovEdt module.
 * @param [in] config The configuration used to initialize the module
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpMovEdt_Init(AMP_MOV_EDT_INIT_CFG_s *config);

/**
 * Get the default configuration of a MovEdt handler.
 * @param [out] config The returned configuration
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpMovEdt_GetDefaultCfg(AMP_MOV_EDT_CFG_s *config);

/**
 * Create an MovEdt handler.
 * @param [in] config The configuration used to create an MovEdt handler
 * @param [out] hdlr The returned MovEdt handler
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpMovEdt_Create(AMP_MOV_EDT_CFG_s *config, AMP_EDT_FORMAT_HDLR_s **hdlr);

/**
 * Delete a MovEdt handler.
 * @param [in] hdlr The MovEdt handler being deleted
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpMovEdt_Delete(AMP_EDT_FORMAT_HDLR_s *hdlr);

/**
 * @}
 */

#endif /* __MOV_EDT_H__ */
