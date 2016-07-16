/**
 *  @file DummyMux.h
 *
 *  DUMMY Muxing Format header
 *
 *  @copyright 2013 Ambarella Corporation. All rights reserved.
 *                 No part of this file may be reproduced, stored in a retrieval system,
 *                 or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                 recording, or otherwise, without the prior consent of Ambarella Corporation.
 */
#ifndef __DUMMY_MUX_H__
#define __DUMMY_MUX_H__

#include <format/Format.h>


/**
 * @defgroup DUMMY
 * @ingroup Format
 * @brief Dummy muxing format implementation
 *
 * Implementation the dummy muxing format
 * DummyMux is a muxing format without any container information, i.e., raw data only.
 *
 */

/**
 * @addtogroup DUMMY
 * @{
 */

/**
 * The configuration for initializing the DummyMux module
 */
typedef struct {
    UINT8 *Buffer;          /**< The work buffer of the DummyMux module */
    UINT32 BufferSize;      /**< The size of the work buffer */
    UINT8 MaxHdlr;          /**< The maximum number of DummyMux handlers */
} AMP_DUMMY_MUX_INIT_CFG_s;

/**
 * The configuration of DummyMux handlers
 */
typedef struct {
    AMP_STREAM_HDLR_s *Stream;          /**< Stream handler */
    union {
        AMP_MUX_PARAM_MOVIE_s Movie;    /**< The muxing parameters of a movie (See AMP_MUX_PARAM_MOVIE_s.) */
        AMP_MUX_PARAM_SOUND_s Sound;    /**< The muxing parameters of a sound (See AMP_MUX_PARAM_SOUND_s.) */
        AMP_MUX_PARAM_IMAGE_s Image;    /**< The muxing parameters of an image (See AMP_MUX_PARAM_IMAGE_s.) */
    } Param;
    UINT8 MediaType;                    /**< Media type (See AMP_MEDIA_TRACK_TYPE_e.) */
} AMP_DUMMY_MUX_CFG_s;

/**
 * Get the default configuration of the DummyMux module.
 * @param [out] config The returned configuration
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpDummyMux_GetInitDefaultCfg(AMP_DUMMY_MUX_INIT_CFG_s *config);

/**
 * Get the required buffer size of the DummyMux module.
 * @param [in] maxHdlr The maximum number of DummyMux handlers
 * @return The required buffer size
 * @see AMP_ER_CODE_e
 */
extern UINT32 AmpDummyMux_GetRequiredBufSize(UINT8 maxHdlr);

/**
 * Initialize the DummyMux module.
 * @param [in] config The configuration used to initialize the module
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpDummyMux_Init(AMP_DUMMY_MUX_INIT_CFG_s *config);

/**
 * Get the default configuration of a DummyMux handler.
 * @param [in] mediaType The media type (See AMP_MEDIA_INFO_TYPE_e.)
 * @param [out] config The returned configuration
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpDummyMux_GetDefaultCfg(UINT8 mediaType, AMP_DUMMY_MUX_CFG_s *cnfig);

/**
 * Create a DummyMux handler.
 * @param [in] config The configuration used to create a DummyMux handler
 * @param [out] hdlr The returned DummyMux handler
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpDummyMux_Create(AMP_DUMMY_MUX_CFG_s *config, AMP_MUX_FORMAT_HDLR_s **hdlr);

/**
 * Delete a DummyMux handler.
 * @param [in] hdlr The DummyMux handler being deleted
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpDummyMux_Delete(AMP_MUX_FORMAT_HDLR_s *hdlr);
/**
 * @}
 */

#endif

