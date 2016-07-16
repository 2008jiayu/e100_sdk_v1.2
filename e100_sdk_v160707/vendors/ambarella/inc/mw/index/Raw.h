/**
 *  @file Raw.h
 *
 *  encode common function header
 *
 *  @copyright 2013 Ambarella Corporation. All rights reserved.
 *                 No part of this file may be reproduced, stored in a retrieval system,
 *                 or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                 recording, or otherwise, without the prior consent of Ambarella Corporation.
 */
#ifndef __RAW_H__
#define __RAW_H__

/**
 * @addtogroup Index
 * @{
 */

#include <index/Index.h>
#include <AmbaNAND_Def.h>

#define AMP_RAW_IDX_SECTOR_SIZE 512 /**< The sector size of the NAND */

#define AMP_RAW_IDX_OP_ERASE_ALL    0x00000001      /**< The opcode of the function to erase all Raw Index partitions */

/**
 * The device type
 */
typedef enum {
    AMP_RAW_IDX_DEV_TYPE_NAND,  /**< NAND */
    AMP_RAW_IDX_DEV_TYPE_EMMC   /**< EMMC */
} AMP_RAW_IDX_DEV_TYPE_e;

/**
 * The information of the NAND device
 */
typedef struct {
    AMBA_NAND_DEV_INFO_s Info;  /**< NAND information (see AMBA_NAND_DEV_INFO_s.) */
    UINT8 Mode;                 /**< NAND initiation mode (NFTL_MODE_NO_SAVE_TRL_TBL or NFTL_MODE_SAVE_TRL_TBL) */
} AMP_RAW_IDX_NAND_INFO_s;

/**
 * The information of the EMMC device
 */
typedef struct {
    UINT8 Resv[4];
} AMP_RAW_IDX_EMMC_INFO_s;

/**
 * The configuration for initializing the Raw Index module
 */
typedef struct {
    UINT8 *Buffer;                  /**< The work buffer of the Raw Index module */
    UINT32 BufferSize;              /**< The size of the work buffer */
    union {
        AMP_RAW_IDX_NAND_INFO_s Nand;   /**< NAND information */
        AMP_RAW_IDX_EMMC_INFO_s Emmc;   /**< EMMC information */
    } DevInfo;
    UINT8 DevType;                  /**< The device type (see AMP_RAW_IDX_DEV_TYPE_e) */
    UINT8 PartId;                   /**< The partition Id */
    UINT8 MaxHdlr;                  /**< The maximum number of Raw Index handlers held in the Raw Index module */
} AMP_RAW_IDX_INIT_CFG_s;

/**
 * The configuration of Raw indexes
 */
typedef struct {
    UINT8 Resv[4];  /**< Reserved */
} AMP_RAW_IDX_CFG_s;

/**
 * Get the required buffer size for initializing the Raw Index module.
 * @param [in] maxHdlr The maximum number of RAW Index handlers
 * @return The required buffer size
 */
UINT32 AmpRawIdx_GetRequiredBufferSize(UINT8 maxHdlr);

/**
 * Get the default configuration for initializing the Raw Index module.
 * @param [out] config The returned configuration
 * @param [in] DevType The device type (see AMP_RAW_IDX_DEV_TYPE_e)
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpRawIdx_GetInitDefaultCfg(AMP_RAW_IDX_INIT_CFG_s *config, UINT8 DevType);

/**
 * Initialize the Raw Index module.
 * @param [in] config The configuration used to initialize the Raw Index module
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpRawIdx_Init(AMP_RAW_IDX_INIT_CFG_s *config);

/**
 * Get the default configuration for creating a Raw Index handler.
 * @param [out] config The returned configuration
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpRawIdx_GetDefaultCfg(AMP_RAW_IDX_CFG_s *config);

/**
 * Create a Raw Index handler.
 * @param [in] config The configuration used to create a Raw Index handler
 * @param [out] hdlr The returned Raw index handler
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpRawIdx_Create(AMP_RAW_IDX_CFG_s *config, AMP_INDEX_HDLR_s **hdlr);

/**
 * Delete a Raw Index handler.
 * @param [in] hdlr The Raw Index handler being deleted
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpRawIdx_Delete(AMP_INDEX_HDLR_s *hdlr);

/**
 * @}
 */
#endif/* __RAW_H__ */

