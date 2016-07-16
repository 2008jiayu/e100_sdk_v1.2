/**
 *  @file MkvMux.h
 *
 *  MKV Muxing Format header
 *
 *  @copyright 2013 Ambarella Corporation. All rights reserved.
 *                 No part of this file may be reproduced, stored in a retrieval system,
 *                 or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                 recording, or otherwise, without the prior consent of Ambarella Corporation.
 */
#ifndef __MKV_MUX_H__
#define __MKV_MUX_H__

/**
 * @defgroup Matroska
 * @ingroup Format
 * @brief Matroska Muxing/Demuxing Format module implementation
 *
 * The implementation of Matroska Muxing/Demuxing Format module
 * Users can use MkvMux to add data into the movie and use MkvDmx to parse data from a movie.
 *
 * The MkvMux/MkvDmx module includes the following functions:
 * 1. Initiate the MkvMux/MkvDmx module
 * 2. Create MkvMux/MkvDmx handlers
 * 3. Delete MkvMux/MkvDmx handlers
 */

/**
 * @addtogroup Matroska
 * @{
 */

#include <format/Format.h>

/**
 * The configuration for initializing the MkvMux module
 */
typedef struct {
    UINT8 *Buffer;     /**< The work buffer of the MkvMux module */
    UINT32 BufferSize;    /**< The work buffer size of the module */
    UINT8 MaxHdlr;        /**< The maximum number of MkvMux handlers */
} AMP_MKV_MUX_INIT_CFG_s;

/**
 * The configuration of creating a MkvMux handler
 */
typedef struct {
    AMP_STREAM_HDLR_s *Stream; /**< Stream handler */
} AMP_MKV_MUX_CFG_s;

/**
 * Get the default configuration for initializing the MkvMux module.
 * @param [out] config The returned configuration
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpMkvMux_GetInitDefaultCfg(AMP_MKV_MUX_INIT_CFG_s *config);

/**
 * Get the required buffer size for initializing the MkvMux module.
 * @param [in] maxHdlr The maximum number of MkvMux handlers
 * @return The required buffer size
 */
extern UINT32 AmpMkvMux_GetRequiredBufferSize(UINT8 maxHdlr);

/**
 * Initialize the MkvMux module.
 * @param [in] config The configuration used to initialize the module
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpMkvMux_Init(AMP_MKV_MUX_INIT_CFG_s *config);

/**
 * Get the default configuration of a MkvMux handler.
 * @param [out] config The returned configuration
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpMkvMux_GetDefaultCfg(AMP_MKV_MUX_CFG_s *config);

/**
 * Create a MkvMux handler.
 * @param [in] config The configuration used to create a MkvMux handler
 * @param [out] hdlr The returned MkvMux handler
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpMkvMux_Create(AMP_MKV_MUX_CFG_s *config, AMP_MUX_FORMAT_HDLR_s **hdlr);

/**
 * Delete a MkvMux handler.
 * @param [in] hdlr The MkvMux handler being deleted
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpMkvMux_Delete(AMP_MUX_FORMAT_HDLR_s *hdlr);

/**
 * @}
 */

#endif

