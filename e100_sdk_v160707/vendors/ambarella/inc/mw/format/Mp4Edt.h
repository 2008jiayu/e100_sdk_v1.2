/**
 *  @file Mp4Edt.h
 *
 *  MP4 Editing format module header
 *
 *  @copyright 2013 Ambarella Corporation. All rights reserved.
 *                 No part of this file may be reproduced, stored in a retrieval system,
 *                 or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                 recording, or otherwise, without the prior consent of Ambarella Corporation.
 */
#ifndef __MP4_EDT_H__
#define __MP4_EDT_H__

/**
 * @addtogroup MP4
 * @{
 */

#include <format/Editor.h>

/**
 * The configuration for initializing Mp4Edt module
 */
typedef struct {
    UINT8 *Buffer;      /**< The work buffer of the Mp4Edt module */
    UINT32 BufferSize;  /**< The work buffer size of the module */
    UINT8 MaxHdlr;      /**< The maximum number of Mp4Edt handlers */
} AMP_MP4_EDT_INIT_CFG_s;

/**
 * The configuration of Mp4Edt handlers
 */
typedef struct {
    AMP_INDEX_HDLR_s *Index;        /**< Index handler */
    AMP_CALLBACK_f OnEvent;         /**< The event callback returning an execution result */
    BOOL8 EnableMoovHead;           /**< The flag to enable the flow of putting MOOV before MDAT */
} AMP_MP4_EDT_CFG_s;

/**
* Get the required buffer size for initializing the Mp4Edt module.
* @param [in] maxHdlr The maximum number of Mp4Edt handlers
* @return The required buffer size
*/
extern UINT32 AmpMp4Edt_GetRequiredBufferSize(UINT8 maxHdlr);

/**
 * Get the default configuration for initializing the Mp4Edt module.
 * @param [out] config The returned configuration
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpMp4Edt_GetInitDefaultCfg(AMP_MP4_EDT_INIT_CFG_s *config);

/**
 * Initialize the Mp4Edt module.
 * @param [in] config The configuration used to initialize the module
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpMp4Edt_Init(AMP_MP4_EDT_INIT_CFG_s *config);

/**
 * Get the default configuration of a Mp4Edt handler.
 * @param [out] config The returned configuration
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpMp4Edt_GetDefaultCfg(AMP_MP4_EDT_CFG_s *config);

/**
 * Create an Mp4Edt handler.
 * @param [in] config The configuration used to create an Mp4Edt handler
 * @param [out] hdlr The returned Mp4Edt handler
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpMp4Edt_Create(AMP_MP4_EDT_CFG_s *config, AMP_EDT_FORMAT_HDLR_s **hdlr);

/**
 * Delete a Mp4Edt handler.
 * @param [in] hdlr The Mp4Edt handler being deleted
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpMp4Edt_Delete(AMP_EDT_FORMAT_HDLR_s *hdlr);

/**
 * @}
 */

#endif /* __MP4_EDT_H__ */
