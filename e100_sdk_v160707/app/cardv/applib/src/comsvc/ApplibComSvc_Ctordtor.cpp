/*
 * cppMm.cpp
 *
 *  Created on: 2014/12/18
 *      Author: cyweng
 */

#ifdef __cplusplus
extern "C"{
#endif
#include <AmbaDataType.h>
#include <AmbaKAL.h>
#ifdef __cplusplus
}
#endif

extern AMBA_KAL_BYTE_POOL_t G_MMPL;
extern AMBA_KAL_BYTE_POOL_t G_NC_MMPL;
/*
unsigned char * ambamalloc(unsigned int size)
{
	void *pmem;
	int ercd;

	ercd = AmbaKAL_BytePoolAllocate(&G_MMPL, (void **) &pmem, size, 100);
	if (ercd != OK)
		return NULL;

	memset(pmem, 0x0, size);

	return (unsigned char *)pmem;	
}

void ambafree(unsigned char * pmem)
{
	AmbaKAL_BytePoolFree(pmem);
}
*/

void *operator new(size_t isize) throw()
{
	void *pmem;
	int ercd;

	ercd = AmbaKAL_BytePoolAllocate(&G_MMPL, (void **) &pmem, isize, 100);
	if (ercd != OK)
		return NULL;

	memset(pmem, 0x0, isize);

	return pmem;
}

void *operator new[](size_t isize) throw()
{
	void *pmem;
	int ercd;

	ercd = AmbaKAL_BytePoolAllocate(&G_MMPL, (void **) &pmem, isize, 100);
	if (ercd != OK)
		return NULL;

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
