/**
 *  @file ExifMux.h
 *
 *  Exif Muxing Format header
 *
 *  @copyright 2013 Ambarella Corporation. All rights reserved.
 *   No part of this file may be reproduced, stored in a retrieval system,
 *   or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *   recording, or otherwise, without the prior consent of Ambarella Corporation.
 */
#ifndef __EXIF_MUX_H__
#define __EXIF_MUX_H__

#include <format/Format.h>

/**
 * @addtogroup EXIF
 * @{
 */

/**
 * The configuration for initializing the ExifMux module
 */
typedef struct {
    UINT8 *Buffer;          /**< The work buffer of the ExifMux module */
    UINT32 BufferSize;      /**< The size of the work buffer */
    UINT32 ExifHeadSize;    /**< The header size of the module */
    UINT8 MaxHdlr;          /**< The maximum number of ExifMux handlers */
} AMP_EXIF_MUX_INIT_CFG_s;

/**
 * Exif tag configuration
 */
typedef struct {
    UINT8 *Data;        /**< Data */
    UINT32 Value;       /**< Value */
    UINT32 Count;       /**< Count */
    UINT16 Tag;         /**< Tag */
    UINT16 Type;        /**< Tag type */
    UINT8 Set;          /**< Set */
} AMP_CFG_TAG_s;

/**
 * Exif tag information
 */
typedef struct {
    UINT16 Ifd0Tags;            /**< The number of Ifd0 tags */
    UINT16 ExifIfdTags;         /**< The number of ExifIfd tags */
    UINT16 IntIfdTags;          /**< The number of IntIfd tags */
    UINT16 Ifd1Tags;            /**< The number of Ifd1 tags */
    UINT16 GpsIfdTags;          /**< The number of GPSIfd tags */
    AMP_CFG_TAG_s Ifd0[IFD0_TOTAL_TAGS];    /**< Ifd0 tags (See AMP_CFG_TAG_s.) */
    AMP_CFG_TAG_s ExifIfd[EXIF_TOTAL_TAGS]; /**< ExtIfd tags (See AMP_CFG_TAG_s.) */
    AMP_CFG_TAG_s IntIfd[IntIFD_TOTAL_TAGS];/**< IntIfd (See AMP_CFG_TAG_s.) */
    AMP_CFG_TAG_s Ifd1[IFD1_TOTAL_TAGS];    /**< Ifd1 tags (See AMP_CFG_TAG_s.) */
    AMP_CFG_TAG_s GpsIfd[GPS_TOTAL_TAGS];   /**< GPSIfd tags (See AMP_CFG_TAG_s.) */
} AMP_CFG_TAG_INFO_s;

/**
 * The configuration of Exif muxer handlers
 */
typedef struct {
    AMP_STREAM_HDLR_s *Stream;          /**< Stream handler */
    AMP_CFG_TAG_INFO_s SetTagInfo;      /**< Tag information */
    UINT8 Endian;                       /**< The value indicating that the Exif header is big endian or little endian (big endian: 0x00, little endian: 0x01) */
} AMP_EXIF_MUX_CFG_s;

/**
 * Get the default configuration for initializing the ExifMux module.
 * @param [out] config The returned configuration
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpExifMux_GetInitDefaultCfg(AMP_EXIF_MUX_INIT_CFG_s *config);

/**
 * Get the required buffer size for initializing the ExifMux module.
 * @param [in] maxHdlr The maximum number of ExifMux handlers
 * @param [in] headSize The size of the Exif header
 * @return The required buffer size
 */
extern UINT32 AmpExifMux_GetRequiredBufferSize(UINT8 maxHdlr, UINT32 headSize);

/**
 * Initialize the ExifMux module.
 * @param [in] config The configuration used to initialize the module
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 * */
extern int AmpExifMux_Init(AMP_EXIF_MUX_INIT_CFG_s *config);

/**
 * Get the default configuration of an ExifMux handler.
 * @param [out] config The returned configuration
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpExifMux_GetDefaultCfg(AMP_EXIF_MUX_CFG_s *config);

/**
 * Create an ExifMux handler.
 * @param [in] config The configuration used to create an ExifMux handler
 * @param [out] hdlr The returned ExifMux handler
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpExifMux_Create(AMP_EXIF_MUX_CFG_s *config, AMP_MUX_FORMAT_HDLR_s **hdlr);

/**
 * Delete an ExifMux handler.
 * @param [in] hdlr The ExifMux handler being deleted
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpExifMux_Delete(AMP_MUX_FORMAT_HDLR_s *hdlr);

/**
 * @}
 */

#endif /* __EXIF_MUX_H__ */
