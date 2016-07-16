/**
 *  @file MkvDmx.h
 *
 *  MKV Demuxing Format header
 *
 *  @copyright 2014 Ambarella Corporation. All rights reserved.
 *                 No part of this file may be reproduced, stored in a retrieval system,
 *                 or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                 recording, or otherwise, without the prior consent of Ambarella Corporation.
 */
#ifndef __MKV_DMX_H__
#define __MKV_DMX_H__

#include <format/Format.h>

/**
 * @addtogroup MKV
 * @{
 */

/**
 * The configuration for initializing MkvDmx module
 */
typedef struct {
    UINT8 *Buffer;          /**< The work buffer of the MkvDmx module */
    UINT32 BufferSize;      /**< The work buffer size of the module */
    UINT8 MaxHdlr;          /**< The maximum number of MkvDmx handlers */
} AMP_MKV_DMX_INIT_CFG_s;

/**
 * The configuration of MkvDmx handlers
 */
typedef struct {
    AMP_STREAM_HDLR_s *Stream;          /**< Stream handler */
} AMP_MKV_DMX_CFG_s;

/**
 * Get the default configuration for initializing the MkvDmx module.
 * @param [out] config The returned configuration
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpMkvDmx_GetInitDefaultCfg(AMP_MKV_DMX_INIT_CFG_s *config);

/**
 * Get the required buffer size for initializing the MkvDmx module.
 * @param [in] maxHdlr The maximum number of MkvDmx handlers
 * @return The required buffer size
 */
extern UINT32 AmpMkvDmx_GetRequiredBufferSize(UINT8 maxHdlr);

/**
 * Initialize the MkvDmx module.
 * @param [in] config The configuration used to initialize the module
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpMkvDmx_Init(AMP_MKV_DMX_INIT_CFG_s *config);

/**
 * Get the default configuration of a MkvDmx handler.
 * @param [out] config The returned configuration
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpMkvDmx_GetDefaultCfg(AMP_MKV_DMX_CFG_s *config);

/**
 * Create a MkvDmx handler.
 * @param [in] config The configuration used to create a MkvDmx handler
 * @param [out] hdlr The returned MkvDmx handler
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpMkvDmx_Create(AMP_MKV_DMX_CFG_s *config, AMP_DMX_FORMAT_HDLR_s **hdlr);

/**
 * Delete a MkvDmx handler.
 * @param [in] hdlr The MkvDmx handler being deleted
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpMkvDmx_Delete(AMP_DMX_FORMAT_HDLR_s *hdlr);

/**
 * Parse media data from a stream, and pack the data into a Media Info object. (See AMP_DMX_FORMAT_PARSE_FP.)
 * @param [in,out] media The returned Media Info object
 * @param [in] stream The I/O stream
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpMkvDmx_Parse(AMP_MEDIA_INFO_s *media, AMP_STREAM_HDLR_s *stream);

/**
 * @}
 */
#endif

