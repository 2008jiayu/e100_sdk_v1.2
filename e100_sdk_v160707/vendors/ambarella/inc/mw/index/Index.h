/**
 *  @file Index.h
 *
 *  encode common function header
 *
 *  @copyright 2013 Ambarella Corporation. All rights reserved.
 *                 No part of this file may be reproduced, stored in a retrieval system,
 *                 or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                 recording, or otherwise, without the prior consent of Ambarella Corporation.
 */
#ifndef __INDEX_H__
#define __INDEX_H__
/**
 * @defgroup Index
 * @brief Index utilities for the Format module
 *
 * Index utilities for the Format module
 * When muxing/editing, some muxing/editing formats need temporal spaces to keep intermediate data for indexing the media.
 * Index module provides the API and interface to access the index spaces.
 */

/**
 * @addtogroup Index
 * @{
 */

#include <mw.h>

/**
 *  Index mode
 */
typedef enum {
    AMP_INDEX_MODE_WRITE = 0,       /**< Write only mode */
    AMP_INDEX_MODE_READ_INDEX = 1,  /**< Read index mode (read index data from an Index file) */
    AMP_INDEX_MODE_READ_STREAM = 2, /**< Read stream mode (read index data from a file stream) */
    AMP_INDEX_MODE_MAX              /**< Max value (used to check the range of the index mode) */
} AMP_INDEX_MODE_s;

/**
 *  The configuration for initializing the Index module
 */
typedef struct {
    void *Buffer;       /**< The work buffer of the Index module */
    UINT32 BufferSize;  /**< The size of the work buffer */
    UINT8 MaxHdlr;      /**< The maximum number of Index handlers held in Index */
} AMP_INDEX_INIT_CFG_s;

/**
 * Index device information
 */
typedef struct {
    UINT32 SectorSize;  /**< The size of a device sector */
    UINT32 PageSize;    /**< The size of a device page */
    UINT32 BlockSize;   /**< The size of a device block */
} AMP_INDEX_DEV_INFO_s;

/**
 * The Index handler
 */
typedef struct {
    struct _AMP_INDEX_s_ *Func;     /**< The functions used to operate an Index file (See AMP_INDEX_s.) */
} AMP_INDEX_HDLR_s;

/**
 *  The interface of Index functions
 */
typedef struct _AMP_INDEX_s_ {
    int (*Open)(AMP_INDEX_HDLR_s *, char *, INT32);             /** The interface to open an Index file (*pHdlr, *Url, Size) */
    int (*Close)(AMP_INDEX_HDLR_s *, BOOL8);                     /** The interface to close an Index file (*pHdlr, bRemove) */
    int (*Read)(AMP_INDEX_HDLR_s *, UINT32, UINT32, UINT8 *);    /** The interface to read data from an Index file (*pHdlr, nOffset, Size, *Buffer) */
    int (*Write)(AMP_INDEX_HDLR_s *, UINT32, UINT32, UINT8 *);   /** The interface to write data into an Index file (*pHdlr, nOffset, Size, *Buffer) */
    int (*Sync)(AMP_INDEX_HDLR_s *);                             /** The interface to flush an Index file (*pHdlr) */
    int (*Info)(AMP_INDEX_DEV_INFO_s *);                         /** The interface to obtain the information of a device where Index files place (*pInfo) */
    int (*Func)(UINT32, UINT32, UINT32);                         /** The interface to invoke other functions with a command code (nCmd, nParam1, nParam2)*/
} AMP_INDEX_s;

/**
 * Get the default configuration for initializing the Index module.
 * @param [out] config The returned configuration
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpIndex_GetInitDefaultCfg(AMP_INDEX_INIT_CFG_s *config);

/**
 * Get the required buffer size for initializing the Index module.
 * @param [in] maxHdlr The maximum number of Index handlers
 * @param [in] memSize The memory size of the index I/O buffer
 * @return The required buffer size
 */
extern UINT32 AmpIndex_GetRequiredBufferSize(UINT8 maxHdlr, UINT32 memSize);

/**
 * Initialize the Index module.
 * @param [in] config The configuration used to initialize the Index module
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpIndex_Init(AMP_INDEX_INIT_CFG_s *config);

/**
 * Allocate an Index buffer.
 * @param [in] size The size of an Index I/O buffer
 * @return The Index buffer address
 */
extern UINT8 *AmpIndex_AllocateBuffer(UINT32 size);

/**
 * Release an Index buffer.
 * @param [in] buffer The Index I/O buffer being released
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpIndex_ReleaseBuffer(UINT8 *buffer);

/**
 * @}
 */
#endif /* __INDEX_H__ */

