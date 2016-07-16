/**
 *  @file MovDmx.h
 *
 *  Mov Demuxing Format header
 *
 *  @copyright 2013 Ambarella Corporation. All rights reserved.
 *                 No part of this file may be reproduced, stored in a retrieval system,
 *                 or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                 recording, or otherwise, without the prior consent of Ambarella Corporation.
 */
#ifndef __MOV_DMX_H__
#define __MOV_DMX_H__

/**
 * @addtogroup MOV
 * @{
 */
#include <format/Format.h>

/**
 * The configuration for initializing the MovDmx module
 */
typedef struct {
    UINT8 *Buffer;      /**< The work buffer of the MovDmx module */
    UINT32 BufferSize;  /**< The work buffer size of the module */
    UINT8 MaxHdlr;      /**< The maximum number of MovDmx handlers */
} AMP_MOV_DMX_INIT_CFG_s;

/**
 * The configuration of creating an MovDmx handler
 */
typedef struct {
    UINT32 MaxIdxNum;   /**< The maximum number of the indexes of an MovDmx handler */
} AMP_MOV_DMX_CFG_INFO_s;

/**
 * The configuration of MovDmx handlers
 */
typedef struct {
    AMP_STREAM_HDLR_s *Stream;          /**< Stream handler */
    AMP_MOV_DMX_CFG_INFO_s DmxCfgInfo;  /**< The configuration of creating an MovDmx handler */
} AMP_MOV_DMX_CFG_s;

/**
 * Get the default configuration for initializing the MovDmx module.
 * @param [out] config The returned configuration
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpMovDmx_GetInitDefaultCfg(AMP_MOV_DMX_INIT_CFG_s *config);

/**
 * Get the required buffer size for initializing the MovDmx module.
 * @param [in] maxHdlr The maximum number of MovDmx handlers
 * @return The required buffer size
 */
extern UINT32 AmpMovDmx_GetRequiredBufferSize(UINT8 maxHdlr);

/**
 * Initialize the MovDmx module.
 * @param [in] config The configuration used to initialize the module
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpMovDmx_Init(AMP_MOV_DMX_INIT_CFG_s *config);

/**
 * Get the default configuration of an MovDmx handler.
 * @param [out] config The returned configuration
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpMovDmx_GetDefaultCfg(AMP_MOV_DMX_CFG_s *config);

/**
 * Create an MovDmx handler.
 * @param [in] config The configuration used to create an MovDmx handler
 * @param [out] hdlr The returned MovDmx handler
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpMovDmx_Create(AMP_MOV_DMX_CFG_s *config, AMP_DMX_FORMAT_HDLR_s **hdlr);

/**
 * Delete an MovDmx handler.
 * @param [in] hdlr The MovDmx handler being deleted
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpMovDmx_Delete(AMP_DMX_FORMAT_HDLR_s *hdlr);

/**
 * Parse media data from a stream, and pack the data into a Media Info object. (See AMP_DMX_FORMAT_PARSE_FP.)
 * @param [in,out] media The returned Media Info object
 * @param [in] stream The I/O stream
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpMovDmx_Parse(AMP_MEDIA_INFO_s *media, AMP_STREAM_HDLR_s *stream);

/**
 * @}
 */
#endif
