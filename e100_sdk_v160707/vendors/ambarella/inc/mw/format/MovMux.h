/**
 *  @file MovMux.h
 *
 *  Mov Muxing Format header
 *
 *  @copyright 2013 Ambarella Corporation. All rights reserved.
 *                 No part of this file may be reproduced, stored in a retrieval system,
 *                 or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                 recording, or otherwise, without the prior consent of Ambarella Corporation.
 */
#ifndef __MOV_MUX_H__
#define __MOV_MUX_H__

/**
 * @defgroup MOV
 * @ingroup Format
 * @brief MOV mux/demux flow implementation
 *
 * The implementation of MOV Muxing/Demuxing/Editing Format module
 * Users can use MovMux to add data into the movie, use MovDmx to parse data from a movie, and use MovEdt to edit a movie..
 *
 * The MovMux/MovDmx module includes the following functions:
 * 1. Initiate the MovMux/MovDmx/MovEdt module
 * 2. Create MovMux/MovDmx/MovEdt handlers
 * 3. Delete MovMux/MovDmx/MovEdt handlers
 */

#include <format/Format.h>

/**
 * @addtogroup MOV
 * @{
 */

/**
 * The configuration for initializing the MovMux module
 */
typedef struct {
    UINT8 *Buffer;      /**< The work buffer of the MovMux module */
    UINT32 BufferSize;  /**< The work buffer size of the module */
    UINT8 MaxHdlr;      /**< The maximum number of MovMux handlers */
} AMP_MOV_MUX_INIT_CFG_s;

/**
 * The configuration of MovMux handlers
 */
typedef struct {
    AMP_STREAM_HDLR_s *Stream;                  /**< Stream handler */
    AMP_INDEX_HDLR_s *Index;                    /**< Index handler */
    UINT32 MaxIdxNum;                           /**< The maximum number of the cached index of the MovMux handler */
    UINT32 TrickRecDivisor;                     /**< The divisor factor to decide media fps for High frame rate. if fps = 120, TrickRecDivisor = 4 => container = 30fps */
    AMP_ISO_PUT_MOOV_FP PutMoov;                /**< The callback of MOOV putting function */
    AMP_ISO_GET_MOOV_SIZE_FP GetMoovSize;       /**< The callback of getting MOOV size function */
    BOOL8 EnableMoovHead;                       /**< The flag to enable the flow of putting MOOV before MDAT */
    BOOL8 EnableCO64;                 /**< The flag to support large mdat offset(64bit file) */
} AMP_MOV_MUX_CFG_s;

/**
* Get the required buffer size for initializing the MovMux module.
* @param [in] maxHdlr The maximum number of MovMux handlers
* @return The required buffer size
*/
extern UINT32 AmpMovMux_GetRequiredBufferSize(UINT8 maxHdlr);

/**
 * Get the default configuration for initializing the MovMux module.
 * @param [out] config The returned configuration
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpMovMux_GetInitDefaultCfg(AMP_MOV_MUX_INIT_CFG_s *config);

/**
 * Initialize the MOV muxer module.
 * @param [in] config The configuration used to initialize the module
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpMovMux_Init(AMP_MOV_MUX_INIT_CFG_s *config);

/**
 * Get the default configuration of a MovMux handler.
 * @param [out] config The returned configuration
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpMovMux_GetDefaultCfg(AMP_MOV_MUX_CFG_s *config);

/**
 * Create a MovMux handler.
 * @param [in] config The configuration used to create a MovMux handler
 * @param [out] hdlr The returned MovMux handler
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpMovMux_Create(AMP_MOV_MUX_CFG_s *config, AMP_MUX_FORMAT_HDLR_s **hdlr);

/**
 * Delete an MovMux handler.
 * @param [in] hdlr The MovMux handler being deleted
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpMovMux_Delete(AMP_MUX_FORMAT_HDLR_s *hdlr);

/**
 * @}
 */
#endif
