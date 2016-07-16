/**
 *  @file Mp4Dmx.h
 *
 *  MP4 Demuxing Format header
 *
 *  @copyright 2013 Ambarella Corporation. All rights reserved.
 *                 No part of this file may be reproduced, stored in a retrieval system,
 *                 or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                 recording, or otherwise, without the prior consent of Ambarella Corporation.
 */
#ifndef __MP4_DMX_H__
#define __MP4_DMX_H__

#include <format/Format.h>

/**
 * @addtogroup MP4
 * @{
 */

/**
 * The configuration for initializing Mp4Dmx module
 */
typedef struct {
    UINT8 *Buffer;          /**< The work buffer of the Mp4Dmx module */
    UINT32 BufferSize;      /**< The work buffer size of the module */
    UINT8 MaxHdlr;          /**< The maximum number of Mp4Dmx handlers */
} AMP_MP4_DMX_INIT_CFG_s;

/**
 * The configuration of creating an Mp4Dmx handler
 */
typedef struct {
    UINT32 MaxIdxNum;       /**< The maximum number of the indexes of an Mp4Dmx handler */
} AMP_MP4_DMX_CFG_INFO_s;

/**
 * The configuration of Mp4Dmx handlers
 */
typedef struct {
    AMP_STREAM_HDLR_s *Stream;          /**< Stream handler */
    AMP_MP4_DMX_CFG_INFO_s DmxCfgInfo;  /**< The configuration of creating an Mp4Dmx handler */
} AMP_MP4_DMX_CFG_s;

/**
 * Get the default configuration for initializing the Mp4Dmx module.
 * @param [out] config The returned configuration
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpMp4Dmx_GetInitDefaultCfg(AMP_MP4_DMX_INIT_CFG_s *config);

/**
 * Get the required buffer size for initializing the Mp4Dmx module.
 * @param [in] maxHdlr The maximum number of Mp4Dmx handlers
 * @return The required buffer size
 */
extern UINT32 AmpMp4Dmx_GetRequiredBufferSize(UINT8 maxHdlr);

/**
 * Initialize the Mp4Dmx module.
 * @param [in] config The configuration used to initialize the module
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpMp4Dmx_Init(AMP_MP4_DMX_INIT_CFG_s *config);

/**
 * Get the default configuration of an Mp4Dmx handler.
 * @param [out] config The returned configuration
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpMp4Dmx_GetDefaultCfg(AMP_MP4_DMX_CFG_s *config);

/**
 * Create an Mp4Dmx handler.
 * @param [in] config The configuration used to create an Mp4Dmx handler
 * @param [out] hdlr The returned Mp4Dmx handler
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpMp4Dmx_Create(AMP_MP4_DMX_CFG_s *config, AMP_DMX_FORMAT_HDLR_s **hdlr);

/**
 * Delete an Mp4Dmx handler.
 * @param [in] hdlr The Mp4Dmx handler being deleted
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpMp4Dmx_Delete(AMP_DMX_FORMAT_HDLR_s *hdlr);

/**
 * Parse media data from a stream, and pack the data into a Media Info object. (See AMP_DMX_FORMAT_PARSE_FP.)
 * @param [in,out] media The returned Media Info object
 * @param [in] stream The I/O stream
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpMp4Dmx_Parse(AMP_MEDIA_INFO_s *media, AMP_STREAM_HDLR_s *stream);

/**
 * @}
 */
#endif
