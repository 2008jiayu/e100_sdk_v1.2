 /**
  * @file util.c
  *
  * SDK common utilities
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

#include "AmbaDataType.h"
#include "AmbaKAL.h"
#include "util.h"
#include "AmbaPrintk.h"
#include "AmbaUART.h"
#include "AmbaFS.h"
#include "AmbaUtility.h"


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
int AmpUtil_GetAlignedPool(AMBA_KAL_BYTE_POOL_t *BytePool, void **AlignedPool, void **Pool, UINT32 Size, UINT32 Alignment)
{
#define CLINE 32
    int Er;
    UINT32 MisAlign;

    if (Alignment < CLINE)
        Alignment = CLINE;

    Size += Alignment << 1;

    Er = AmbaKAL_BytePoolAllocate(BytePool, Pool, Size, 500);
    if (Er == OK) {
        MisAlign =  (UINT32)(*Pool) & (Alignment - 1);
        *AlignedPool = (void *)((UINT32)(*Pool) + (Alignment - MisAlign));
    }

    return Er;

}




