/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaFwLoader.c
 *
 *  @Copyright      :: Copyright (C) 2012 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Simple card manager APIs.
\*-------------------------------------------------------------------------------------------------------------------*/

#include <stdio.h>
#include <ctype.h>
#include <string.h>

#include "AmbaDataType.h"
#include "AmbaKAL.h"
#include "AmbaNAND_PartitionTable.h"
#include "AmbaNAND.h"
#include "AmbaFwLoader.h"
#include "AmbaRTSL_MMU.h"
#include "AmbaRTSL_Cache.h"
#include "AmbaPrintk.h"

#include "AmbaSysCtrl.h"
#include "AmbaEMMC_Def.h"

#ifdef ENABLE_DEBUG_MSG_FW_LOADER
#define DBGMSG AmbaPrint
#else
#define DBGMSG(...)
#endif

#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))

extern void *__ddr_region0_ro_start, *__ddr_region0_ro_end;
extern void *__ddr_region1_ro_start, *__ddr_region1_ro_end;
extern void *__ddr_region2_ro_start, *__ddr_region2_ro_end;
extern void *__ddr_region3_ro_start, *__ddr_region3_ro_end;
extern void *__ddr_region4_ro_start, *__ddr_region4_ro_end;
extern void *__ddr_region5_ro_start, *__ddr_region5_ro_end;

extern void *__ddr_region0_rw_start, *__ddr_region0_rw_end;
extern void *__ddr_region1_rw_start, *__ddr_region1_rw_end;
extern void *__ddr_region2_rw_start, *__ddr_region2_rw_end;
extern void *__ddr_region3_rw_start, *__ddr_region3_rw_end;
extern void *__ddr_region4_rw_start, *__ddr_region4_rw_end;
extern void *__ddr_region5_rw_start, *__ddr_region5_rw_end;

static AMBA_FW_REGION_s Regions[] = {

    {
        /* .pName           = */ "Region0",
        /* .ImageOffset     = */ (UINT32)&__ddr_region0_ro_start,
        /* .RoSectionBase   = */ (UINT32)&__ddr_region0_ro_start,
        /* .RoSectionEnd    = */ (UINT32)&__ddr_region0_ro_end,
        /* .RoSectionLength = */ (UINT32)0,
        /* .RwSectionBase   = */ (UINT32)&__ddr_region0_rw_start,
        /* .RwSectionEnd    = */ (UINT32)&__ddr_region0_rw_end,
        /* .RwSectionLength = */ (UINT32)0
    },
    {
        /* .pName           = */ "Region1",
        /* .ImageOffset     = */ (UINT32)&__ddr_region0_ro_start,
        /* .RoSectionBase   = */ (UINT32)&__ddr_region1_ro_start,
        /* .RoSectionEnd    = */ (UINT32)&__ddr_region1_ro_end,
        /* .RoSectionLength = */ (UINT32)0,
        /* .RwSectionBase   = */ (UINT32)&__ddr_region1_rw_start,
        /* .RwSectionEnd    = */ (UINT32)&__ddr_region1_rw_end,
        /* .RwSectionLength = */ (UINT32)0,

    },
    {
        /* .pName           = */ "Region2",
        /* .ImageOffset     = */ (UINT32)&__ddr_region0_ro_start,
        /* .RoSectionBase   = */ (UINT32)&__ddr_region2_ro_start,
        /* .RoSectionEnd    = */ (UINT32)&__ddr_region2_ro_end,
        /* .RoSectionLength = */ (UINT32)0,
        /* .RwSectionBase   = */ (UINT32)&__ddr_region2_rw_start,
        /* .RwSectionEnd    = */ (UINT32)&__ddr_region2_rw_end,
        /* .RwSectionLength = */ (UINT32)0,
    },
    {
        /* .pName           = */ "Region3",
        /* .ImageOffset     = */ (UINT32)&__ddr_region0_ro_start,
        /* .RoSectionBase   = */ (UINT32)&__ddr_region3_ro_start,
        /* .RoSectionEnd    = */ (UINT32)&__ddr_region3_ro_end,
        /* .RoSectionLength = */ (UINT32)0,
        /* .RwSectionBase   = */ (UINT32)&__ddr_region3_rw_start,
        /* .RwSectionEnd    = */ (UINT32)&__ddr_region3_rw_end,
        /* .RwSectionLength = */ (UINT32)0,
    },
    {
        /* .pName           = */ "Region4",
        /* .ImageOffset     = */ (UINT32)&__ddr_region0_ro_start,
        /* .RoSectionBase   = */ (UINT32)&__ddr_region4_ro_start,
        /* .RoSectionEnd    = */ (UINT32)&__ddr_region4_ro_end,
        /* .RoSectionLength = */ (UINT32)0,
        /* .RwSectionBase   = */ (UINT32)&__ddr_region4_rw_start,
        /* .RwSectionEnd    = */ (UINT32)&__ddr_region4_rw_end,
        /* .RwSectionLength = */ (UINT32)0,
    },
    {
        /* .pName           = */ "Region5",
        /* .ImageOffset     = */ (UINT32)&__ddr_region0_ro_start,
        /* .RoSectionBase   = */ (UINT32)&__ddr_region5_ro_start,
        /* .RoSectionEnd    = */ (UINT32)&__ddr_region5_ro_end,
        /* .RoSectionLength = */ (UINT32)0,
        /* .RwSectionBase   = */ (UINT32)&__ddr_region5_rw_start,
        /* .RwSectionEnd    = */ (UINT32)&__ddr_region5_rw_end,
        /* .RwSectionLength = */ (UINT32)0,
    },
};
UINT32 AmbaSysGetBootDevice(void);
static void FwloaderSetupMMU(UINT32 Base, UINT32 Length, int WriteEnable)
{
    AMBA_MMU_MAP_INFO_s MapInfo;
    MapInfo.VirtAddr  = AmbaRTSL_MmuPhysToVirt(Base);
    MapInfo.PhysAddr  = AmbaRTSL_MmuVirtToPhys(Base);
    MapInfo.Size      = Length;
    MapInfo.Attr.Data = AMBA_MMU_MAP_ATTR_SMALL_PAGE;
    MapInfo.Attr.Data |= (WriteEnable) ? AMBA_MMU_MAP_CACHE_MEM_READ_WRITE_ATTR :
                         AMBA_MMU_MAP_CACHE_MEM_READ_ONLY_ATTR;

    AmbaRTSL_MmuTlbSetup(&MapInfo);
}

static void FlushTlb(void)
{
    register unsigned int x;

    x = 0x0;
    __asm__ volatile("mcr   p15, 0, %0, c8, c6, 0": : "r" (x));
}

int AmbaFwLoader_Load(UINT32 Id)
{
    int rval;
    AMBA_FW_REGION_s *pRegion = &Regions[Id];
    int (*LoadFunc)(int PartID, UINT32 ImageOffset, UINT8 * pDst, UINT32 Length) = NULL;

    if ((pRegion->RoSectionLength == 0) && (pRegion->RwSectionLength == 0))
        return OK;

    if (AmbaSysGetBootDevice() == BOOT_FROM_NAND)
        LoadFunc = AmbaNAND_LoadPartitionData;
    else if (AmbaSysGetBootDevice() == BOOT_FROM_EMMC)
        LoadFunc = AmbaEMMC_LoadPartitionData;
    else
        return NG;

    AmbaPrint("[%s] region: %s, ImgOffset: 0x%08x",
              __func__, pRegion->pName, pRegion->ImageOffset);
    AmbaPrint("[%s] RoBase: 0x%08x, RoLength: 0x%08x",
              __func__, pRegion->RoSectionBase, pRegion->RoSectionLength);
    AmbaPrint("[%s] RwBase: 0x%08x, RwLength: 0x%08x",
              __func__, pRegion->RwSectionBase, pRegion->RwSectionLength);

    FwloaderSetupMMU(pRegion->RoSectionBase, pRegion->RoSectionLength, 1);
    FwloaderSetupMMU(pRegion->RwSectionBase, pRegion->RwSectionLength, 1);

    __disable_interrupt();

    AmbaRTSL_CacheCleanEntireDataCache();
    FlushTlb();

    __DSB();

    __enable_interrupt();       /* Renable interrupt */


    rval = LoadFunc(AMBA_PARTITION_SYS_SOFTWARE,
                    pRegion->RoSectionBase - pRegion->ImageOffset,
                    (UINT8 *)pRegion->RoSectionBase, pRegion->RoSectionLength);
    if (rval != pRegion->RoSectionLength) {
        rval = NG;
        goto Done;
    }

    rval = LoadFunc(AMBA_PARTITION_SYS_SOFTWARE,
                    pRegion->RwSectionBase - pRegion->ImageOffset,
                    (UINT8 *)pRegion->RwSectionBase, pRegion->RwSectionLength);
    if (rval != pRegion->RwSectionLength)
        rval = NG;
Done:

    if (rval != OK)
        AmbaPrint("[%s] pload: load failed", __func__);

    return OK;
}

int AmbaFwLoader_Finish(void)
{
    AMBA_FW_REGION_s *pRegion;
    int i = 0;

    for (i = 1; i < ARRAY_SIZE(Regions); i++) {
        pRegion = &Regions[i];
        FwloaderSetupMMU(pRegion->RoSectionBase, pRegion->RoSectionLength, 0);
    }

    __disable_interrupt();

    FlushTlb();
    AmbaRTSL_CacheCleanEntireDataCache();

    __DSB();

    __enable_interrupt();       /* Renable interrupt */

    return OK;
}

int AmbaFwLoader_FlushCache(void)
{
    __disable_interrupt();

    FlushTlb();
    AmbaRTSL_CacheCleanEntireDataCache();

    __DSB();

    __enable_interrupt();       /* Renable interrupt */

    return OK;
}

int AmbaFwLoader_Init(void)
{
    AMBA_FW_REGION_s *pRegion;
    int i;

    for (i = 1; i < ARRAY_SIZE(Regions); i++) {
        pRegion = &Regions[i];
        pRegion->RoSectionLength = pRegion->RoSectionEnd - pRegion->RoSectionBase;
        pRegion->RwSectionLength = pRegion->RwSectionEnd - pRegion->RwSectionBase;
    }

#if 0
    __disable_interrupt();

    pRegion = &Regions[0];
    FwloaderSetupMMU(pRegion->RoSectionBase, pRegion->RoSectionLength, 1);
    FwloaderSetupMMU(pRegion->RwSectionBase, pRegion->RwSectionLength, 1);

    FlushTlb();
    AmbaRTSL_CacheCleanEntireDataCache();
    __DSB();

    __enable_interrupt();       /* Renable interrupt */
#endif

    for (i = 1; i < ARRAY_SIZE(Regions); i++) {
        pRegion = &Regions[i];
        FwloaderSetupMMU(pRegion->RoSectionBase, pRegion->RoSectionLength, 1);
        FwloaderSetupMMU(pRegion->RwSectionBase, pRegion->RwSectionLength, 1);
    }

    __disable_interrupt();

    FlushTlb();
    AmbaRTSL_CacheCleanEntireDataCache();

    __DSB();

    __enable_interrupt();       /* Renable interrupt */

    return OK;
}

