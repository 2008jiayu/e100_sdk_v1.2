/**
 *  @file ExifDmx.h
 *
 *  Exif Demuxing Format header
 *
 *  @copyright 2013 Ambarella Corporation. All rights reserved.
 *   No part of this file may be reproduced, stored in a retrieval system,
 *   or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *   recording, or otherwise, without the prior consent of Ambarella Corporation.
 */
#ifndef __EXIF_DMX_H__
#define __EXIF_DMX_H__

#include <format/Format.h>

/**
 * @addtogroup EXIF
 * @{
 */

/**
 * The configuration for initializing the ExifDmx module
 */
typedef struct {
    UINT8 *Buffer;          /**< The work buffer of the ExifDmx module */
    UINT32 BufferSize;      /**< The size of the work buffer */
    UINT8 MaxHdlr;          /**< The maximum number of ExifDmx handlers */
} AMP_EXIF_DMX_INIT_CFG_s;

/**
 * The configuration of ExifDmx handlers
 */
typedef struct {
    AMP_STREAM_HDLR_s *Stream;  /**< Stream handler */
} AMP_EXIF_DMX_CFG_s;

/**
 * Get the default configuration for initializing the ExifDmx module.
 * @param [out] config The returned configuration
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpExifDmx_GetInitDefaultCfg(AMP_EXIF_DMX_INIT_CFG_s *config);

/**
 * Get the required buffer size for initializing the ExifDmx module.
 * @param [in] maxHdlr The maximum number of ExifDmx handlers
 * @return The required buffer size
 */
extern UINT32 AmpExifDmx_GetRequiredBufferSize(UINT8 maxHdlr);

/**
 * Initialize the ExifDmx module.
 * @param [in] config The configuration used to initialize the module
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpExifDmx_Init(AMP_EXIF_DMX_INIT_CFG_s *config);

/**
 * Get the default configuration of an ExifDmx handler.
 * @param [out] config The returned configuration
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpExifDmx_GetDefaultCfg(AMP_EXIF_DMX_CFG_s *config);

/**
 * Create an ExifDmx handler.
 * @param [in] config The configuration used to create an ExifDmx handler
 * @param [out] hdlr The returned ExifDmx handler
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpExifDmx_Create(AMP_EXIF_DMX_CFG_s *config, AMP_DMX_FORMAT_HDLR_s **hdlr);

/**
 * Delete an ExifDmx handler.
 * @param [in] hdlr The ExifDmx handler being deleted
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpExifDmx_Delete(AMP_DMX_FORMAT_HDLR_s *hdlr);

/**
 * Parse media data from a I/O stream, and pack the data into a Media Info object. (See AMP_DMX_FORMAT_PARSE_FP.)
 * @param [in,out] media The returned Media Info object
 * @param [in] stream The I/O stream
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpExifDmx_Parse(AMP_MEDIA_INFO_s *media, AMP_STREAM_HDLR_s *stream);

/**
 * @}
 */

#endif /* __EXIF_DMX_H__ */
