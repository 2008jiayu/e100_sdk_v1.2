 /**
  * @file inc/mw/utils/util.h
  *
  * TKP common utilities header
  *
  * History:
  *    2013/06/21 - [Jenghung Luo] created file
  *
  * Copyright (C) 2013, Ambarella, Inc.
  *
  * All rights reserved. No Part of this file may be reproduced, stored
  * in a retrieval system, or transmitted, in any form, or by any means,
  * electronic, mechanical, photocopying, recording, or otherwise,
  * without the prior consent of Ambarella, Inc.
  */

#ifndef _UTIL_H_
#define _UTIL_H_
/**
 * @defgroup Utility
 * @brief Utilities for applications
 *
 * Implementation the below function
 *  1. Memory pool management
 *  2. Tool to dump dsp log
 *
 */

/**
 * @addtogroup Utility
 * @{
 */
/**
 * Get aligned byte pool
 *
 * @param [in] BytePool Byte pool
 * @param [out] AlignedPool Pool address after alignment
 * @param [out] Pool Pool address before alignment
 * @param [in] Size Pool size
 * @param [in] Alignment Alignment
 *
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
int AmpUtil_GetAlignedPool(AMBA_KAL_BYTE_POOL_t *BytePool, void **AlignedPool, void **Pool, UINT32 Size, UINT32 Alignment);

/**
 * @}
 */

#endif

