/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaMmuMap.c
 *
 *  @Copyright      :: Copyright (C) 2015 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Mapping the physical address of RAM to logical address with caching/uncaching attribute
\*-------------------------------------------------------------------------------------------------------------------*/

#include <stdio.h>

#include "AmbaCortexA9.h"
#include "AmbaDataType.h"
#include "AmbaRTSL_MMU.h"

/*-----------------------------------------------------------------------------------------------*\
 * MMU RAM address region ID
\*-----------------------------------------------------------------------------------------------*/
typedef enum _AMBA_MMU_RAM_REGION_e_ {
    AMBA_MMU_RAM_REGION_RESV,                /* system reserv */
    AMBA_MMU_RAM_REGION_ROM,                /* ROM */
    AMBA_MMU_RAM_REGION_DSP_BUF,            /* DSP Buffers */
    AMBA_MMU_RAM_REGION_RW_DATA,            /* Read/Write Data */

    AMBA_MMU_RAM_REGION_DSP_UCODE,          /* DSP uCode */

    AMBA_MMU_RAM_REGION_CACHED_HEAP,        /* Cacheable Memory Heap */
    AMBA_MMU_RAM_REGION_NONCACHED_HEAP,     /* Non-cacheable Memory Heap */

    AMBA_MMU_RAM_REGION_HIGH_VECT,          /* High Vector Region */

    AMBA_MMU_RAM_REGION_LINUX,              /* Linux */

    AMBA_MMU_NUM_RAM_REGION                 /* Number of DRAM memory map regions */
} AMBA_MMU_RAM_REGION_e;

/*-----------------------------------------------------------------------------------------------*\
 * MMU RAM map table
\*-----------------------------------------------------------------------------------------------*/
static AMBA_MMU_MAP_INFO_s MMU_RamMapInfo[AMBA_MMU_NUM_RAM_REGION];

#ifndef SUPPORT_PHY_ADDR
/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MMU_RamMapTranslate
 *
 *  @Description:: MMU Memory Map Translation for Virtual Address RTOS.
 *                 Add some more operations to avoid the IAR optimization.
 *                 IAR optimization will cause linking error.
 *
 *  @Input      ::
 *      Addr:  address to be translated
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static UINT32 MMU_RamMapTranslate(UINT32 Addr)
{
    UINT32 HighAddr, LowAddr;

    HighAddr    = (Addr & 0xF0000000) - (AMBA_CORTEX_A9_DRAM_VIRT_BASE_ADDR & 0xF0000000);
    LowAddr     = (Addr & 0x0FFFFFFF) - (AMBA_CORTEX_A9_DRAM_VIRT_BASE_ADDR & 0x0FFFFFFF);

    return (HighAddr | LowAddr);
}
#endif

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MMU_RamMapDoAlign
 *
 *  @Description:: Align the end of the reion.
 *
 *  @Input      ::
 *      Addr:  address to be aligned.
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static inline UINT32 MMU_RamMapDoAlign(UINT32 Addr, UINT32 Align)
{
    return ((Addr & (Align - 1)) ? ((Addr & ~(Align - 1)) + Align) : Addr);
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MMU_RamMapInit
 *
 *  @Description:: MMU Memory Map initializations for DRAM
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static void MMU_RamMapInit(void)
{
    extern void *__ddr_resv_start, *__ddr_resv_end;

    extern void *__ddr_region0_ro_start, *__ddr_region0_ro_end;
    extern void *__ddr_region0_rw_start;
#if 0
    extern void *__ddr_region1_ro_start, *__ddr_region1_ro_end;
    extern void *__ddr_region1_rw_start, *__ddr_region1_rw_end;

    extern void *__ddr_region2_ro_start, *__ddr_region2_ro_end;
    extern void *__ddr_region2_rw_start, *__ddr_region2_rw_end;

    extern void *__ddr_region3_ro_start, *__ddr_region3_ro_end;
    extern void *__ddr_region3_rw_start, *__ddr_region3_rw_end;

    extern void *__ddr_region4_ro_start, *__ddr_region4_ro_end;
    extern void *__ddr_region4_rw_start, *__ddr_region4_rw_end;

    extern void *__ddr_region5_ro_start, *__ddr_region5_ro_end;
    extern void *__ddr_region5_rw_start, *__ddr_region5_rw_end;
#endif
    extern void *__bss_end;
    extern void *__dsp_buf_start, *__dsp_buf_end;

    extern void *__ucode_start, *__ucode_end;
    extern void *__non_cache_heap_start, *__non_cache_heap_end;
    extern void *__cache_heap_start, *__cache_heap_end;
    extern void *__linux_start, *__linux_end;

    UINT32 SizeOfLinux;

    AMBA_MMU_MAP_INFO_s *pMapInfo;

#ifndef SUPPORT_PHY_ADDR
    /* Non-cheable Memory: System resv */
    pMapInfo            = &MMU_RamMapInfo[AMBA_MMU_RAM_REGION_RESV];
    pMapInfo->PhysAddr  = MMU_RamMapTranslate((UINT32) &__ddr_resv_start);                 /* Physical Address */
    pMapInfo->VirtAddr  = (UINT32) &__ddr_resv_start;                                      /* Virtual Address */
    pMapInfo->Size      = MMU_RamMapDoAlign((UINT32) &__ddr_resv_end, 0x1000) - pMapInfo->VirtAddr;                   /* Size in Bytes */
    pMapInfo->Attr.Data = AMBA_MMU_MAP_ATTR_SMALL_PAGE | AMBA_MMU_MAP_NON_CACHE_MEM_ATTR;

    /* Cacheable Memory: ROM */
    pMapInfo            = &MMU_RamMapInfo[AMBA_MMU_RAM_REGION_ROM];
    pMapInfo->PhysAddr  = MMU_RamMapTranslate((UINT32) &__ddr_region0_ro_start);               /* Physical Address */
    pMapInfo->VirtAddr  = (UINT32) &__ddr_region0_ro_start;                                    /* Virtual Address */
    pMapInfo->Size      = MMU_RamMapDoAlign((UINT32) &__ddr_region0_ro_end, 0x1000) - pMapInfo->VirtAddr;            /* Size in Bytes */
    pMapInfo->Attr.Data = AMBA_MMU_MAP_ATTR_SMALL_PAGE  | AMBA_MMU_MAP_CACHE_MEM_READ_ONLY_ATTR;

    /* Non-cheable Memory: DSP Buffers */
    pMapInfo            = &MMU_RamMapInfo[AMBA_MMU_RAM_REGION_DSP_BUF];
    pMapInfo->PhysAddr  = MMU_RamMapTranslate((UINT32) &__dsp_buf_start);                 /* Physical Address */
    pMapInfo->VirtAddr  = (UINT32) &__dsp_buf_start;                                      /* Virtual Address */
    pMapInfo->Size      = MMU_RamMapDoAlign((UINT32) &__dsp_buf_end, 0x1000) - pMapInfo->VirtAddr;                    /* Size in Bytes */
    pMapInfo->Attr.Data = AMBA_MMU_MAP_ATTR_SMALL_PAGE | AMBA_MMU_MAP_NON_CACHE_MEM_ATTR;

    /* Cacheable Memory: Read/Write */
    pMapInfo            = &MMU_RamMapInfo[AMBA_MMU_RAM_REGION_RW_DATA];
    pMapInfo->PhysAddr  = MMU_RamMapTranslate((UINT32) &__ddr_region0_rw_start);                 /* Physical Address */
    pMapInfo->VirtAddr  = (UINT32) &__ddr_region0_rw_start;                                      /* Virtual Address */
    pMapInfo->Size      = MMU_RamMapDoAlign((UINT32) &__bss_end, 0x1000) - pMapInfo->VirtAddr;                       /* Size in Bytes */
    pMapInfo->Attr.Data = AMBA_MMU_MAP_ATTR_SMALL_PAGE | AMBA_MMU_MAP_CACHE_MEM_READ_WRITE_ATTR;

    /* Non-cacheable memory: DSP uCodes */
    pMapInfo            = &MMU_RamMapInfo[AMBA_MMU_RAM_REGION_DSP_UCODE];
    pMapInfo->PhysAddr  = MMU_RamMapTranslate((UINT32) &__ucode_start);         /* Physical Address */
    pMapInfo->VirtAddr  = (UINT32) &__ucode_start;                              /* Virtual Address */
    pMapInfo->Size      = MMU_RamMapDoAlign((UINT32) &__ucode_end, 0x100000) - pMapInfo->VirtAddr;      /* Size in Bytes */
    pMapInfo->Attr.Data = AMBA_MMU_MAP_ATTR_SECTION | AMBA_MMU_MAP_NON_CACHE_MEM_ATTR;

    /* Non-cacheable memory: Linux area */
    pMapInfo            = &MMU_RamMapInfo[AMBA_MMU_RAM_REGION_LINUX];
    pMapInfo->PhysAddr  = MMU_RamMapTranslate((UINT32) &__linux_start);               /* Physical Address */
    pMapInfo->VirtAddr  = (UINT32) &__linux_start;                                  /* Virtual Address */
    pMapInfo->Size      = MMU_RamMapDoAlign((UINT32) &__linux_end, 0x100000) - pMapInfo->VirtAddr;            /* Size in Bytes */
    SizeOfLinux         = pMapInfo->Size;
    pMapInfo->Attr.Data = AMBA_MMU_MAP_ATTR_SECTION | AMBA_MMU_MAP_NON_CACHE_MEM_ATTR;

    /* Non-cacheable memory: heap  */
    pMapInfo            = &MMU_RamMapInfo[AMBA_MMU_RAM_REGION_NONCACHED_HEAP];
    pMapInfo->PhysAddr  = MMU_RamMapTranslate((UINT32) &__non_cache_heap_start);      /* Physical Address */
    pMapInfo->VirtAddr  = (UINT32) &__non_cache_heap_start;                           /* Virtual Address */
    pMapInfo->Size      = MMU_RamMapDoAlign((UINT32) &__non_cache_heap_end, 0x100000) - pMapInfo->VirtAddr;   /* Size in Bytes */
    pMapInfo->Attr.Data = AMBA_MMU_MAP_ATTR_SECTION  | AMBA_MMU_MAP_NON_CACHE_MEM_ATTR;

    /* Cacheable memory: heap  */
    pMapInfo            = &MMU_RamMapInfo[AMBA_MMU_RAM_REGION_CACHED_HEAP];
    pMapInfo->PhysAddr  = MMU_RamMapTranslate((UINT32) &__cache_heap_start);         /* Physical Address */
    pMapInfo->VirtAddr  = (UINT32) &__cache_heap_start;                              /* Virtual Address */
    pMapInfo->Size      = MMU_RamMapDoAlign((UINT32) &__cache_heap_end, 0x100000) - pMapInfo->VirtAddr;             /* Size in Bytes */
    pMapInfo->Attr.Data = AMBA_MMU_MAP_ATTR_SECTION  | AMBA_MMU_MAP_CACHE_MEM_READ_WRITE_ATTR;

    /* Remap the high vector to __ddr_region0_ro_start */
    pMapInfo            = &MMU_RamMapInfo[AMBA_MMU_RAM_REGION_HIGH_VECT];
    pMapInfo->PhysAddr  = MMU_RamMapTranslate((UINT32) &__ddr_region0_ro_start);                    /* Physical Address */
    pMapInfo->VirtAddr  = 0xffff0000;                                                   /* Virtual address: High Vector Base Address must be 0xffff0000 */
    pMapInfo->Size      = 0x10000;                                                      /* size in Bytes */
    pMapInfo->Attr.Data = AMBA_MMU_MAP_ATTR_SMALL_PAGE | AMBA_MMU_MAP_CACHE_MEM_READ_ONLY_ATTR;
#else
    /* Non-cheable Memory: System resv */
    pMapInfo            = &MMU_RamMapInfo[AMBA_MMU_RAM_REGION_RESV];
    pMapInfo->PhysAddr  = MMU_RamMapTranslate((UINT32) &__ddr_resv_start);                 /* Physical Address */
    pMapInfo->VirtAddr  = (UINT32) &__ddr_resv_start;                                      /* Virtual Address */
    pMapInfo->Size      = (UINT32) &__ddr_resv_start - (UINT32) &__ddr_resv_end;           /* Size in Bytes */
    pMapInfo->Attr.Data = AMBA_MMU_MAP_ATTR_SMALL_PAGE | AMBA_MMU_MAP_NON_CACHE_MEM_ATTR;

    /* Cacheable Memory: ROM */
    pMapInfo            = &MMU_RamMapInfo[AMBA_MMU_RAM_REGION_ROM];
    pMapInfo->PhysAddr  = (UINT32) &__ddr_region0_ro_start;    /* Physical Address */
    pMapInfo->VirtAddr  = (UINT32) &__ddr_region0_ro_start;    /* Virtual Address */
    pMapInfo->Size      = ((UINT32) &__ddr_region0_ro_end) - pMapInfo->PhysAddr;    /* Size in Bytes */
    pMapInfo->Attr.Data = AMBA_MMU_MAP_ATTR_SMALL_PAGE  | AMBA_MMU_MAP_CACHE_MEM_READ_WRITE_ATTR;   /* ??? */

    /* Non-cheable Memory: DSP Buffers */
    pMapInfo            = &MMU_RamMapInfo[AMBA_MMU_RAM_REGION_DSP_BUF];
    pMapInfo->PhysAddr  = (UINT32) &__dsp_buf_start;      /* Physical Address */
    pMapInfo->VirtAddr  = (UINT32) &__dsp_buf_start;      /* Virtual Address */
    pMapInfo->Size      = ((UINT32) &__dsp_buf_end) - pMapInfo->PhysAddr;      /* Size in Bytes */
    pMapInfo->Attr.Data = AMBA_MMU_MAP_ATTR_SMALL_PAGE  | AMBA_MMU_MAP_NON_CACHE_MEM_ATTR;

    /* Cacheable Memory: Read/Write */
    pMapInfo            = &MMU_RamMapInfo[AMBA_MMU_RAM_REGION_RW_DATA];
    pMapInfo->PhysAddr  = (UINT32) &__ddr_region0_rw_start;      /* Physical Address */
    pMapInfo->VirtAddr  = (UINT32) &__ddr_region0_rw_start;      /* Virtual Address */
    pMapInfo->Size      = ((UINT32) &__ddr_region0_rw_end) - pMapInfo->PhysAddr;  /* Size in Bytes */
    pMapInfo->Attr.Data = AMBA_MMU_MAP_ATTR_SMALL_PAGE  | AMBA_MMU_MAP_CACHE_MEM_READ_WRITE_ATTR;

    /* Non-cacheable memory: DSP uCodes */
    pMapInfo            = &MMU_RamMapInfo[AMBA_MMU_RAM_REGION_DSP_UCODE];
    pMapInfo->PhysAddr  = (UINT32) &__ucode_start;  /* Physical Address */
    pMapInfo->VirtAddr  = (UINT32) &__ucode_start;  /* Virtual Address */
    pMapInfo->Size      = ((UINT32) &__ucode_end) - pMapInfo->PhysAddr;  /* Size in Bytes */
    pMapInfo->Attr.Data = AMBA_MMU_MAP_ATTR_SECTION  | AMBA_MMU_MAP_NON_CACHE_MEM_ATTR;

    /* Non-cacheable memory: Linux area */
    pMapInfo            = &MMU_RamMapInfo[AMBA_MMU_RAM_REGION_LINUX];
    pMapInfo->PhysAddr  = (UINT32) &__linux_start;    /* Physical Address */
    pMapInfo->VirtAddr  = (UINT32) &__linux_start;    /* Virtual Address */
    pMapInfo->Size      = ((UINT32) &__linux_end) - pMapInfo->PhysAddr;    /* Size in Bytes */
    pMapInfo->Attr.Data = AMBA_MMU_MAP_ATTR_SECTION  | AMBA_MMU_MAP_NON_CACHE_MEM_ATTR;

    /* Non-cacheable memory: heap  */
    pMapInfo            = &MMU_RamMapInfo[AMBA_MMU_RAM_REGION_NONCACHED_HEAP];
    pMapInfo->PhysAddr  = (UINT32) &__non_cache_heap_start;  /* Physical Address */
    pMapInfo->VirtAddr  = (UINT32) &__non_cache_heap_start;  /* Virtual Address */
    pMapInfo->Size      = ((UINT32) &__non_cache_heap_end) - pMapInfo->PhysAddr;    /* Size in Bytes */
    pMapInfo->Attr.Data = AMBA_MMU_MAP_ATTR_SECTION  | AMBA_MMU_MAP_NON_CACHE_MEM_ATTR;

    /* Cacheable memory: heap  */
    pMapInfo            = &MMU_RamMapInfo[AMBA_MMU_RAM_REGION_CACHED_HEAP];
    pMapInfo->PhysAddr  = (UINT32) &__cache_heap_start;  /* Physical Address */
    pMapInfo->VirtAddr  = (UINT32) &__cache_heap_start;  /* Virtual Address */
    pMapInfo->Size      = ((UINT32) &__cache_heap_end) - pMapInfo->PhysAddr + 1;  /* Size in Bytes */
    pMapInfo->Attr.Data = AMBA_MMU_MAP_ATTR_SECTION  | AMBA_MMU_MAP_CACHE_MEM_READ_WRITE_ATTR;

    /* Remap the high vector to __ddr_region0_ro_start */
    pMapInfo            = &MMU_RamMapInfo[AMBA_MMU_RAM_REGION_HIGH_VECT];
    pMapInfo->PhysAddr  = (UINT32) &__ddr_region0_ro_start;         /* Physical address */
    pMapInfo->VirtAddr  = 0xffff0000;                   /* Virtual address: High Vector Base Address must be 0xffff0000 */
    pMapInfo->Size      = 0x4000;                       /* size in Bytes */
    pMapInfo->Attr.Data = AMBA_MMU_MAP_ATTR_SMALL_PAGE  | AMBA_MMU_MAP_CACHE_MEM_READ_ONLY_ATTR;
#endif
}

int AmbaMMU_CheckCached(UINT32 startAddr, UINT32 size)
{
    AMBA_MMU_MAP_INFO_s *pMapInfo = &MMU_RamMapInfo[AMBA_MMU_RAM_REGION_CACHED_HEAP];
    return ((startAddr >= pMapInfo->VirtAddr) && (( startAddr + size)<=(pMapInfo->VirtAddr+pMapInfo->Size)) )? 1 : 0;
}

AMBA_MMU_USER_CTRL_s AmbaMmuUserCtrl = {
    .MemMapInit      = MMU_RamMapInit,              /* pointer to the init function of Memory Map */

    .pMemMapInfo     = MMU_RamMapInfo,              /* pointer to the base address of Memory Map Info */
    .MemMapNumRegion = AMBA_MMU_NUM_RAM_REGION,     /* number of Memory Map regions */

    .HighVectFlag    = 1,                           /* the flag of High Vector: 1 - High Vector */
};
