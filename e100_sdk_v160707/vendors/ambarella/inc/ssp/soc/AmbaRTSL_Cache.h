/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaRTSL_Cache.h
 *
 *  @Copyright      :: Copyright (C) 2013 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Definitions & Constants for System Control Coprocessor - CP15 Related APIs
\*-------------------------------------------------------------------------------------------------------------------*/

#ifndef _AMBA_RTSL_CACHE_H_
#define _AMBA_RTSL_CACHE_H_

//#include <intrinsics.h>
#include "AmbaCache_Def.h"

/*-----------------------------------------------------------------------------------------------*\
 * Defined in AmbaRTSL_Cache.c
\*-----------------------------------------------------------------------------------------------*/
extern int (*_AmbaRTSL_CacheEnterCriticalSectionCallBack)(UINT32 *pProcessorStatusRegister);
extern int (*_AmbaRTSL_CacheExitCriticalSectionCallBack)(UINT32 ProcessorStatusRegister);

#define AmbaRTSL_CacheEnterCriticalSectionRegister(pFunc)   _AmbaRTSL_CacheEnterCriticalSectionCallBack = (pFunc)
#define AmbaRTSL_CacheExitCriticalSectionRegister(pFunc)    _AmbaRTSL_CacheExitCriticalSectionCallBack = (pFunc)

void AmbaRTSL_CacheInvalidateEntireDataCache(void);
void AmbaRTSL_CacheInvalidateEntireL1DataCache(void);
void AmbaRTSL_CacheInvalidateDataCacheRange(void *pAddr, UINT32 Size);
void AmbaRTSL_CacheCleanEntireDataCache(void);
void AmbaRTSL_CacheCleanDataCacheRange(void *pAddr, UINT32 Size);
void AmbaRTSL_CacheFlushEntireDataCache(void);
void AmbaRTSL_CacheFlushDataCacheRange(void *pAddr, UINT32 Size);

int AmbaRTSL_CacheLockInstCacheRange(void *pAddr, UINT32 Size);
int AmbaRTSL_CacheLockDataCacheRange(void *pAddr, UINT32 Size);
void AmbaRTSL_CacheUnlockInstCache(void);
void AmbaRTSL_CacheUnlockDataCache(void);

void AmbaRTSL_CacheEnableL2CC(void);

#endif  /* _AMBA_RTSL_CACHE_H_ */
