/**
 *  @file AmbaCtorDtor.cpp
 *  
 *  **History**
 *      |Date       |Name        |Comments       |
 *      |-----------|------------|---------------|
 *      |2015/04/16 |cyweng      |Created        |
 *
 *  @copyright 2013 Ambarella Corporation. All rights reserved.
 *                 No part of this file may be reproduced, stored in a retrieval system,
 *                 or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                 recording, or otherwise, without the prior consent of Ambarella Corporation.
 */

#ifdef __cplusplus
extern "C"{
#endif
#include <AmbaDataType.h>
#include <AmbaKAL.h>
#ifdef __cplusplus
}
#endif

static AMBA_KAL_BYTE_POOL_t *pAmbaCtorDtorMpl = NULL;

#ifdef __cplusplus
extern "C"{
#endif
void AmbaCtorDtor_Init(AMBA_KAL_BYTE_POOL_t *pMpl)
{
    pAmbaCtorDtorMpl = pMpl;
}
#ifdef __cplusplus
}
#endif

void *operator new(size_t isize) throw()
{
	void *pmem;
	int ercd;

	ercd = AmbaKAL_BytePoolAllocate(pAmbaCtorDtorMpl, (void **) &pmem, isize, 100);
	if (ercd != OK) {
		return NULL;
    }

	memset(pmem, 0x0, isize);

	return pmem;
}

void *operator new[](size_t isize) throw()
{
	void *pmem;
	int ercd;

	ercd = AmbaKAL_BytePoolAllocate(pAmbaCtorDtorMpl, (void **) &pmem, isize, 100);
	if (ercd != OK) {
		return NULL;
    }

	memset(pmem, 0x0, isize);

	return pmem;
}

void operator delete(void *pmem) throw()
{
	AmbaKAL_BytePoolFree(pmem);
}

void operator delete[](void *pmem) throw()
{
	AmbaKAL_BytePoolFree(pmem);
}

