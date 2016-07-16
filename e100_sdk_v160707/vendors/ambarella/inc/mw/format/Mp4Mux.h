/**
 *  @file Mp4Mux.h
 *
 *  Mp4 Muxing Format header
 *
 *  @copyright 2013 Ambarella Corporation. All rights reserved.
 *                 No part of this file may be reproduced, stored in a retrieval system,
 *                 or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                 recording, or otherwise, without the prior consent of Ambarella Corporation.
 */
#ifndef __MP4_MUX_H__
#define __MP4_MUX_H__

/**
 * @defgroup MP4
 * @ingroup Format
 * @brief MP4 Muxing/Demuxing Format implementation
 *
 * The implementation of MP4 Muxing/Demuxing/Editing Format module
 * Users can use Mp4Mux to add data into the movie, use Mp4Dmx to parse data from a movie, and use Mp4Edt to edit a movie.
 *
 * The Mp4Mux/Mp4Dmx/Mp4Edt module includes the following functions:
 * 1. Initiate the Mp4Mux/Mp4Dmx/Mp4Edt module
 * 2. Create Mp4Mux/Mp4Dmx/Mp4Edt handlers
 * 3. Delete Mp4Mux/Mp4Dmx/Mp4Edt handlers
 */

#include <format/Format.h>

/**
 * @addtogroup MP4
 * @{
 */

/**
 * The configuration for initializing Mp4Mux module
 */
typedef struct {
    UINT8 *Buffer;      /**< The work buffer of the Mp4Mux module */
    UINT32 BufferSize;  /**< The work buffer size of the module */
    UINT8 MaxHdlr;      /**< The maximum number of Mp4Mux handlers */
} AMP_MP4_MUX_INIT_CFG_s;

/**
 * The configuration of Mp4Mux handlers
 */
typedef struct {
    AMP_STREAM_HDLR_s *Stream;                  /**< Stream handler */
    AMP_INDEX_HDLR_s *Index;                    /**< Index handler */
    UINT32 MaxIdxNum;                           /**< The maximum number of the cached index of an Mp4Mux handler */
    UINT32 TrickRecDivisor;                     /**< The divisor factor to decide media fps for High frame rate. if fps = 120, TrickRecDivisor = 4 => container = 30fps */
    AMP_ISO_PUT_MOOV_FP PutMoov;                /**< The callback of MOOV putting function */
    AMP_ISO_GET_MOOV_SIZE_FP GetMoovSize;       /**< The callback of getting MOOV size function */
    BOOL8 EnableMoovHead;                       /**< The flag to enable the flow of putting MOOV before MDAT */
    BOOL8 EnableCO64;                 /**< The flag to support large mdat offset(64bit file) */
} AMP_MP4_MUX_CFG_s;

/**
* Get the required buffer size for initializing the Mp4Mux module.
* @param [in] maxHdlr The maximum number of Mp4Mux handlers
* @return The required buffer size
*/
extern UINT32 AmpMp4Mux_GetRequiredBufferSize(UINT8 maxHdlr);

/**
 * Get the default configuration for initializing the Mp4Mux module.
 * @param [out] config The returned configuration
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpMp4Mux_GetInitDefaultCfg(AMP_MP4_MUX_INIT_CFG_s *config);

/**
 * Initialize the Mp4Mux module.
 * @param [in] config The configuration used to initialize the module
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpMp4Mux_Init(AMP_MP4_MUX_INIT_CFG_s *config);

/**
 * Get the default configuration of a Mp4Mux handler.
 * @param [out] config The returned configuration
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpMp4Mux_GetDefaultCfg(AMP_MP4_MUX_CFG_s *config);

/**
 * Create an Mp4Mux handler.
 * @param [in] config The configuration used to create an Mp4Mux handler
 * @param [out] hdlr The returned Mp4Mux handler
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpMp4Mux_Create(AMP_MP4_MUX_CFG_s *config, AMP_MUX_FORMAT_HDLR_s **hdlr);

/**
 * Delete a Mp4Mux handler.
 * @param [in] hdlr The Mp4Mux handler being deleted
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpMp4Mux_Delete(AMP_MUX_FORMAT_HDLR_s *hdlr);

/**
 * @}
 */


#endif
