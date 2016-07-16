/**
 *  @file File.h
 *
 *  FILE stream header
 *
 *  @copyright 2013 Ambarella Corporation. All rights reserved.
 *                 No part of this file may be reproduced, stored in a retrieval system,
 *                 or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                 recording, or otherwise, without the prior consent of Ambarella Corporation.
 */
#ifndef __FILE_H__
#define __FILE_H__
/**
 * @defgroup FileStream
 * @ingroup Stream
 * @brief Stream data from/to file
 *
 * The File Stream module includes the following functions:
 * 1. Initiate the File Stream module
 * 2. Create file streams
 * 3. Delete file streams
 *
 */

/**
 * @addtogroup FileStream
 * @{
 */

#include <stream/Stream.h>

/**
 * The configuration for initializing the File Stream module
 */
typedef struct {
    UINT8 *Buffer;          /**< The work buffer of the File Stream module */
    UINT32 BufferSize;      /**< The size of the work buffer */
    UINT8 MaxHdlr;          /**< The maximum number of handlers held in the File Stream module */
} AMP_FILE_STREAM_INIT_CFG_s;

/**
 * The async parameters of the File Stream module
 */
typedef struct {
    UINT8 MaxBank;     /**< The maximum number of banks that the handler can use */
} AMP_FILE_STREAM_ASYNC_PARAM_s;

/**
 * The configuration for initializing a file stream handler
 */
typedef struct {
    UINT32 Alignment;       /**< The alignment of file size */
    UINT32 BytesToSync;     /**< The number of bytes to sync FAT */
    AMP_FILE_STREAM_ASYNC_PARAM_s AsyncParam;   /**< The parameters of async mode */
    BOOL8 Async;            /**< The flag to enable async mode (The handler runs in async mode.) */
    BOOL8 LowPriority;      /**< The flag to enable low priority (The handler runs in low priority mode.)  */
} AMP_FILE_STREAM_CFG_s;

/**
 * Get the default configuration for initializing the File Stream module.
 * @param [out] config The returned configuration
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpFileStream_GetInitDefaultCfg(AMP_FILE_STREAM_INIT_CFG_s *config);

/**
 * Get the required buffer size for initializing the File Stream module.
 * @param [in] maxHdlr The maximum number of handlers held in the File Stream module
 * @return The required buffer size
 */
extern UINT32 AmpFileStream_GetRequiredBufferSize(UINT8 maxHdlr);

/**
 * Initialize the File Stream module.
 * @param [in] config The configuration used to initialize the File Stream module
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpFileStream_Init(AMP_FILE_STREAM_INIT_CFG_s *config);

/**
 * Get the default configuration of a file stream handler.
 * @param [out] config The returned configuration
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpFileStream_GetDefaultCfg(AMP_FILE_STREAM_CFG_s *config);

/**
 * Create a file stream handler.
 * @param [in] config The configuration used to create a file stream handler
 * @param [out] hdlr The returned handler
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpFileStream_Create(AMP_FILE_STREAM_CFG_s *config, AMP_STREAM_HDLR_s **hdlr);

/**
 * Delete a file stream handler.
 * @param [in] hdlr The handler of a file stream being removed
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpFileStream_Delete(AMP_STREAM_HDLR_s *hdlr);

#endif
