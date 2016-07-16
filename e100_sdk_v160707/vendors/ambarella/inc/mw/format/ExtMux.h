/**
 *  @file ExtMux.h
 *
 *  External Muxing Format header
 *
 *  @copyright 2013 Ambarella Corporation. All rights reserved.
 *                 No part of this file may be reproduced, stored in a retrieval system,
 *                 or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                 recording, or otherwise, without the prior consent of Ambarella Corporation.
 */
#ifndef __EXT_MUX_H__
#define __EXT_MUX_H__

/**
 * @addtogroup EXT
 * @{
 */

#include <format/Format.h>

/**
 * The configuration for initializing the ExtMux module
 */
typedef struct {
    UINT8 *Buffer;          /**< The work buffer of the ExtMux module */
    UINT32 BufferSize;      /**< The work buffer size of the module */
    UINT8 MaxHdlr;          /**< The maximum number of ExtMux handlers */
} AMP_EXT_MUX_INIT_CFG_s;

/**
 * The configuration of ExtMux handlers
 */
typedef struct {
    AMP_STREAM_HDLR_s *Stream;          /**< Stream handler */
    union {
        AMP_MUX_PARAM_MOVIE_s Movie;    /**< The muxing parameters of a movie */
        AMP_MUX_PARAM_SOUND_s Sound;    /**< The muxing parameters of a sound */
        AMP_MUX_PARAM_IMAGE_s Image;    /**< The muxing parameters of an image */
    } Param;
    UINT8 MediaType;                    /**< Media type (the media type of a file being muxed, for example, movie and sound) */
} AMP_EXT_MUX_CFG_s;

/**
 * Get the default configuration for initializing the ExtMux module.
 * @param [out] config The returned configuration
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpExtMux_GetInitDefaultCfg(AMP_EXT_MUX_INIT_CFG_s *config);

/**
 * Get the required buffer size for initializing the ExtMux module.
 * @param [in] maxHdlr The maximum number of ExtMux handlers
 * @return The required buffer size
 */
extern UINT32 AmpExtMux_GetRequiredBufferSize(UINT8 maxHdlr);

/**
 * Initialize the ExtMux module.
 * @param [in] config The configuration used to initialize the module
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpExtMux_Init(AMP_EXT_MUX_INIT_CFG_s *config);

/**
 * Get the default configuration of an ExtMux handler.
 * @param [in] mediaType The media type (See AMP_MEDIA_INFO_TYPE_e.)
 * @param [out] config The returned configuration
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpExtMux_GetDefaultCfg(UINT8 mediaType, AMP_EXT_MUX_CFG_s *config);

/**
 * Create an ExtMux handler.
 * @param [in] config The configuration used to create an ExtMux handler
 * @param [out] hdlr The returned ExtMux handler
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpExtMux_Create(AMP_EXT_MUX_CFG_s *config, AMP_MUX_FORMAT_HDLR_s **hdlr);

/**
 * Delete an ExtMux handler.
 * @param [in] hdlr The ExtMux handler being deleted
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpExtMux_Delete(AMP_MUX_FORMAT_HDLR_s *hdlr);

/**
 * @}
 */

#endif

