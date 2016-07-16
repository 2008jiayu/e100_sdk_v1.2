/**
 *  @file ExtDmx.h
 *
 *  External Demuxing Format header
 *
 *  @copyright 2013 Ambarella Corporation. All rights reserved.
 *                 No part of this file may be reproduced, stored in a retrieval system,
 *                 or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                 recording, or otherwise, without the prior consent of Ambarella Corporation.
 */
#ifndef __EXT_DMX_H__
#define __EXT_DMX_H__

/**
 * @addtogroup EXT
 * @{
 */

#include <format/Format.h>

/**
 * The configuration for initializing the ExtDmx module
 */
typedef struct {
    UINT8 *Buffer;      /**< The work buffer of the ExtDmx module */
    UINT32 BufferSize;  /**< The work buffer size of the module */
    UINT8 MaxHdlr;      /**< The maximum number of ExtDmx handlers */
} AMP_EXT_DMX_INIT_CFG_s;

/**
 * The configuration of ExtDmx handlers
 */
typedef struct {
    AMP_STREAM_HDLR_s *Stream;      /**< Stream handler (the stream handler of a file being demuxed) */
    UINT8 MediaType;                /**< Media type (the media type of a file being demuxed, for example, movie and sound) */
} AMP_EXT_DMX_CFG_s;

/**
 * Get the default configuration for initializing the ExtDmx module.
 * @param [out] config The returned configuration
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpExtDmx_GetInitDefaultCfg(AMP_EXT_DMX_INIT_CFG_s *config);

/**
 * Get the required buffer size for initializing the ExtDmx module.
 * @param [in] maxHdlr The maximum number of ExtDmx handlers
 * @return The required buffer size
 */
extern UINT32 AmpExtDmx_GetRequiredBufferSize(UINT8 maxHdlr);

/**
 * Initialize the ExtDmx module.
 * @param [in] config The configuration used to initialize the module
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpExtDmx_Init(AMP_EXT_DMX_INIT_CFG_s *config);

/**
 * Get the default configuration of an ExtDmx handler.
 * @param [in] mediaType The media type (See AMP_MEDIA_INFO_TYPE_e.)
 * @param [out] config The returned configuration
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpExtDmx_GetDefaultCfg(UINT8 mediaType, AMP_EXT_DMX_CFG_s *config);

/**
 * Create an ExtDmx handler.
 * @param [in] config The configuration used to create an ExtDmx handler
 * @param [out] hdlr The returned ExtDmx handler
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpExtDmx_Create(AMP_EXT_DMX_CFG_s *config, AMP_DMX_FORMAT_HDLR_s **hdlr);

/**
 * Delete an ExtDmx handler.
 * @param [in] hdlr The ExtDmx handler being deleted
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpExtDmx_Delete(AMP_DMX_FORMAT_HDLR_s *hdlr);

/**
 * Parse media data from a stream, and pack the data into a Media Info object. (See AMP_DMX_FORMAT_PARSE_FP.)
 * @param [in,out] media The returned Media Info object
 * @param [in] stream The I/O stream
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpExtDmx_Parse(AMP_MEDIA_INFO_s *media, AMP_STREAM_HDLR_s *stream);

/**
 * @}
 */

#endif

