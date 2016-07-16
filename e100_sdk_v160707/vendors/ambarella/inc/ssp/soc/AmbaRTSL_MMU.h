/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaRTSL_MMU.h
 *
 *  @Copyright      :: Copyright (C) 2013 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Definitions & Constants for Memory Management Unit Control APIs
\*-------------------------------------------------------------------------------------------------------------------*/

#ifndef _AMBA_RTSL_MMU_H_
#define _AMBA_RTSL_MMU_H_

#include "AmbaRTSL_MMU_Def.h"

/*-----------------------------------------------------------------------------------------------*\
 * MMU map attribute definitions.
\*-----------------------------------------------------------------------------------------------*/
#define AMBA_MMU_MAP_ATTR_DESC_TYPE(x)      (x & 0x3)
#define AMBA_MMU_MAP_ATTR_BUFFERABLE        (0x1 << 2)          /* Bufferable memory region */
#define AMBA_MMU_MAP_ATTR_CACHEABLE         (0x1 << 3)          /* Cacheable memory region */
#define AMBA_MMU_MAP_ATTR_EXEC_NEVER        (0x1 << 4)          /* Execute Never memory region */
#define AMBA_MMU_MAP_ATTR_DOMAIN(x)         ((x & 0xf) << 5)
#define AMBA_MMU_MAP_ATTR_AP(x)             ((x & 0x7) << 9)
#define AMBA_MMU_MAP_ATTR_TEX(x)            ((x & 0x7) << 12)   /* Type Extension memory region */
#define AMBA_MMU_MAP_ATTR_SHAREABLE         (0x1 << 16)         /* Shareable memory region */
#define AMBA_MMU_MAP_ATTR_NOT_GLOBAL        (0x1 << 17)
#define AMBA_MMU_MAP_ATTR_NON_SECURE        (0x1 << 19)

#define AMBA_MMU_MAP_BUS_ATTR                   \
    (AMBA_MMU_MAP_ATTR_EXEC_NEVER         |     \
     AMBA_MMU_MAP_ATTR_AP(3)              |     \
     AMBA_MMU_MAP_ATTR_DOMAIN(0))

#define AMBA_MMU_MAP_CACHE_MEM_READ_ONLY_ATTR   \
    (AMBA_MMU_MAP_ATTR_CACHEABLE          |     \
     AMBA_MMU_MAP_ATTR_AP(2)              |     \
     AMBA_MMU_MAP_ATTR_SHAREABLE          |     \
     AMBA_MMU_MAP_ATTR_DOMAIN(0))

#define AMBA_MMU_MAP_CACHE_MEM_READ_WRITE_ATTR  \
    (AMBA_MMU_MAP_ATTR_BUFFERABLE         |     \
     AMBA_MMU_MAP_ATTR_CACHEABLE          |     \
     AMBA_MMU_MAP_ATTR_AP(3)              |     \
     AMBA_MMU_MAP_ATTR_SHAREABLE          |     \
     AMBA_MMU_MAP_ATTR_DOMAIN(0))

#define AMBA_MMU_MAP_NON_CACHE_MEM_ATTR         \
    (AMBA_MMU_MAP_ATTR_AP(3)              |     \
     AMBA_MMU_MAP_ATTR_SHAREABLE          |     \
     AMBA_MMU_MAP_ATTR_TEX(1)             |     \
     AMBA_MMU_MAP_ATTR_DOMAIN(0))

typedef enum _AMBA_MMU_MAP_ATTR_DESC_TYPE_e_ {
    AMBA_MMU_MAP_ATTR_SUPERSECTION = 0, /* consist of 16MB blocks of memory */
    AMBA_MMU_MAP_ATTR_SECTION,          /* consist of 1MB blocks of memory */
    AMBA_MMU_MAP_ATTR_LARGE_PAGE,       /* consist of 64KB blocks of memory */
    AMBA_MMU_MAP_ATTR_SMALL_PAGE        /* consist of 4KB blocks of memory */
} AMBA_MMU_MAP_ATTR_DESC_TYPE_e;

typedef union _AMBA_MMU_MAP_ATTR_u_ {
    UINT32  Data;

    struct {
        UINT32  DescType:   2;      /* [1:0]:   Small page (4KB) or Section (1MB). */
        UINT32  Buffered:   1;      /* [2]:     Memory region attribute bits. Bufferable bit */
        UINT32  Cached:     1;      /* [3]:     Memory region attribute bits. Cacheable bit. */
        UINT32  ExeNever:   1;      /* [4]:     The execute-never bit. Determines whether the region is executable. */
        UINT32  Domain:     4;      /* [8:5]:   Domain field. */
        UINT32  AP:         3;      /* [11:9]:  Access Permissions bits[2:0]. */
        UINT32  TEX:        3;      /* [14:12]: Memory region attribute bits. */
        UINT32  Reserved1:  1;      /* [15]:    Reserved */
        UINT32  Shared:     1;      /* [16]:    Determines whether the translation is for Shareable memory. */
        UINT32  NotGlobal:  1;      /* [17]:    The not global bit. Determines how the translation is marked in the TLB */
        UINT32  Reserved2:  1;      /* [18]:    Reserved */
        UINT32  NonSecure:  1;      /* [19]:    Non-secure bit. */
        UINT32  Reserved3:  12;     /* [31:20]: Reserved. */
    } Bits;
} AMBA_MMU_MAP_ATTR_u;

typedef struct _AMBA_MMU_MAP_INFO_s_ {
    UINT32  VirtAddr;
    UINT32  PhysAddr;
    UINT32  Size;
    AMBA_MMU_MAP_ATTR_u Attr;
} AMBA_MMU_MAP_INFO_s;

typedef struct _AMBA_MMU_USER_CTRL_s_ {
    void    (*MemMapInit)(void);        /* pointer to the init function of Memory Map */

    AMBA_MMU_MAP_INFO_s *pMemMapInfo;   /* pointer to the base address of Memory Map Info */
    int     MemMapNumRegion;            /* number of Memory Map regions */

    UINT8   HighVectFlag;               /* the flag of High Vector: 1 - High Vector */
} AMBA_MMU_USER_CTRL_s;

/*-----------------------------------------------------------------------------------------------*\
 * Defined in an external file: i.e. AmbaMmuUserCtrl.c
\*-----------------------------------------------------------------------------------------------*/
extern AMBA_MMU_USER_CTRL_s AmbaMmuUserCtrl;
int AmbaRTSL_MmuTlbSetup(AMBA_MMU_MAP_INFO_s *pMapInfo);

#endif  /* _AMBA_RTSL_MMU_H_ */
