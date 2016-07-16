/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaRTSL_MMU_Def.h
 *
 *  @Copyright      :: Copyright (C) 2013 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Common definitions & constants for MMU RTSL Control APIs
\*-------------------------------------------------------------------------------------------------------------------*/

#ifndef _AMBA_RTSL_MMU_DEF_H_
#define _AMBA_RTSL_MMU_DEF_H_

#define AMBA_TLB_DESC_AP_1_0(x)             ((x) & 0x3)         /* Access Permissions Bit[1:0] */
#define AMBA_TLB_DESC_AP_2(x)               (((x) >> 2) & 0x1)  /* Access Permissions Bit[2] */

#define AMBA_TLB_GET_L1_PAGE_NUM(x)         ((x) >> 20)                 /* 0 ~ 4095. Section Number. */
#define AMBA_TLB_GET_L2_LARGE_PAGE_NUM(x)   (((x) & 0x000f0000) >> 16)  /* 0 ~ 15 */
#define AMBA_TLB_GET_L2_SMALL_PAGE_NUM(x)   (((x) & 0x000ff000) >> 12)  /* 0 ~ 255 */

/*-----------------------------------------------------------------------------------------------*\
 *  Virtual Memory System Architecture version 7 (VMSAv7) first-level TLB descriptor formats
\*-----------------------------------------------------------------------------------------------*/
typedef enum _AMBA_TLB_L1_DESC_FORMAT_e_ {
    AMBA_TLB_L1_DESC_FAULT = 0,
    AMBA_TLB_L1_DESC_PAGE_TABLE,
    AMBA_TLB_L1_DESC_SECTION,
    AMBA_TLB_L1_DESC_RESERVED
} AMBA_TLB_L1_DESC_FORMAT_e;

#define AMBA_TLB_L1_PAGE_TBL_BASE(x)        ((x) >> 10)
#define AMBA_TLB_L1_PAGE_TBL_BASE_MASK      0xfffffc00
#define AMBA_TLB_L1_SECTION_BASE(x)         ((x) >> 20)
#define AMBA_TLB_L1_SECTION_BASE_MASK       0xfff00000
#define AMBA_TLB_L1_SUPERSECTION_BASE(x)    ((x) >> 24)
#define AMBA_TLB_L1_SUPERSECTION_BASE_MASK  0x000000ffff000000ULL

typedef union _AMBA_TLB_VMSAV7_L1_DESC_s_ {
    UINT32  Data;

    struct {
        UINT32  DescType:               2;      /* [1:0]:   0b00 = Invalid or fault entry */
        UINT32  Reserved:               30;     /* [31:2]:  IGNORE */
    } Fault;

    struct {
        UINT32  DescType:               2;      /* [1:0]:   0b01 = Page table descriptor */
        UINT32  Reserved:               1;      /* [2]:     SBZ */
        UINT32  NonSecure:              1;      /* [3]:     Non-secure bit. */
        UINT32  Reserved1:              1;      /* [4]:     SBZ */
        UINT32  Domain:                 4;      /* [8:5]:   Domain field. */
        UINT32  Reserved2:              1;      /* [9]:     IMP */
        UINT32  BaseAddr:               22;     /* [31:10]: Page table base address, bits [31:10]. */
    } PageTbl;

    struct {
        UINT32  DescType:               2;      /* [1:0]:   0b10 = Section or Supersection descriptor for the associated MVA. Bit [18]. */
        UINT32  Buffered:               1;      /* [2]:     Memory region attribute bits. Bufferable bit */
        UINT32  Cached:                 1;      /* [3]:     Memory region attribute bits. Cacheable bit. */
        UINT32  ExeNever:               1;      /* [4]:     The execute-never bit. Determines whether the region is executable. */
        UINT32  Domain:                 4;      /* [8:5]:   Domain field. */
        UINT32  Reserved:               1;      /* [9]:     IMP */
        UINT32  AP:                     2;      /* [11:10]: Access Permissions bits[1:0]. */
        UINT32  TEX:                    3;      /* [14:12]: Memory region attribute bits. */
        UINT32  AP2:                    1;      /* [15]:    Access Permissions bit[2]. */
        UINT32  Shared:                 1;      /* [16]:    Determines whether the translation is for Shareable memory. */
        UINT32  NotGlobal:              1;      /* [17]:    The not global bit. Determines how the translation is marked in the TLB */
        UINT32  SectType:               1;      /* [18]:    0 = section */
        UINT32  NonSecure:              1;      /* [19]:    Non-secure bit. */
        UINT32  BaseAddr:               12;     /* [31:20]: Section base address, PA[31:20]. */
    } Section;

    struct {
        UINT32  DescType:               2;      /* [1:0]:   0b10 = Section or Supersection descriptor for the associated MVA. Bit [18]. */
        UINT32  Buffered:               1;      /* [2]:     Bufferable bit */
        UINT32  Cached:                 1;      /* [3]:     Cacheable bit. */
        UINT32  ExeNever:               1;      /* [4]:     The execute-never bit. Determines whether the region is executable. */
        UINT32  ExtBaseAddrHi:          4;      /* [8:5]:   Extended base address PA[39:36]. */
        UINT32  Reserved1:              1;      /* [9]:     IMP */
        UINT32  AP:                     2;      /* [11:10]: Access Permissions bits[1:0]. */
        UINT32  TEX:                    3;      /* [14:12]: Memory region attribute bits. */
        UINT32  AP2:                    1;      /* [15]:    Access Permissions bit[2]. */
        UINT32  Shared:                 1;      /* [16]:    Determines whether the translation is for Shareable memory. */
        UINT32  NotGlobal:              1;      /* [17]:    The not global bit. Determines how the translation is marked in the TLB */
        UINT32  SectType:               1;      /* [18]:    1 = Super section. */
        UINT32  NonSecure:              1;      /* [19]:    Non-secure bit. */
        UINT32  ExtBaseAddrLo:          4;      /* [23:20]: Extended base address PA[35:32]. */
        UINT32  BaseAddr:               8;      /* [31:24]: Section base address, PA[31:24]. */
    } SuperSect;

    struct {
        UINT32  DescType:               2;      /* [1:0]:   0b11 = Reserved. In VMSAv7, descriptors with bits [1:0] == 0b11 generate Translation faults. */
        UINT32  Reserved:               30;     /* [31:2]:  Reserved. */
    } Reserved;
} AMBA_TLB_VMSAV7_L1_DESC_s;

/*-----------------------------------------------------------------------------------------------*\
 *  Virtual Memory System Architecture version 7 (VMSAv7) second-level TLB descriptor formats
\*-----------------------------------------------------------------------------------------------*/
typedef enum _AMBA_TLB_L2_DESC_FORMAT_e_ {
    AMBA_TLB_L2_DESC_FAULT = 0,
    AMBA_TLB_L2_DESC_LARGE_PAGE,
    AMBA_TLB_L2_DESC_SMALL_PAGE
} AMBA_TLB_L2_DESC_FORMAT_e;

#define AMBA_TLB_L2_SMALL_PAGE_BASE(x)      ((x) >> 12)
#define AMBA_TLB_L2_SMALL_PAGE_BASE_MASK    0xfffff000
#define AMBA_TLB_L2_LARGE_PAGE_BASE(x)      ((x) >> 16)
#define AMBA_TLB_L2_LARGE_PAGE_BASE_MASK    0xffff0000

typedef union _AMBA_TLB_VMSAV7_L2_DESC_s_ {
    UINT32  Data;

    struct {
        UINT32  DescType:               2;      /* [1:0]:   0b00 = Invalid or fault entry */
        UINT32  Reserved:               30;     /* [31:2]:  IGNORE */
    } Fault;

    struct {
        UINT32  DescType:               2;      /* [1:0]:   0b01 = Large page descriptor. */
        UINT32  Buffered:               1;      /* [2]:     Memory region attribute bits. Bufferable bit */
        UINT32  Cached:                 1;      /* [3]:     Memory region attribute bits. Cacheable bit. */
        UINT32  AP:                     2;      /* [5:4]:   Access Permissions bits[1:0]. */
        UINT32  Reserved1:              3;      /* [8:6]:   SBZ. */
        UINT32  AP2:                    1;      /* [9]:     Access Permissions bit[2]. */
        UINT32  Shared:                 1;      /* [10]:    Determines whether the translation is for Shareable memory. */
        UINT32  NotGlobal:              1;      /* [11]:    The not global bit. Used in the TLB matching process. */
        UINT32  TEX:                    3;      /* [14:12]: Memory region attribute bits. */
        UINT32  ExeNever:               1;      /* [15]:    The execute-never bit. Determines whether the region is executable. */
        UINT32  BaseAddr:               16;     /* [31:16]: Small page base address, PA[31:12]. */
    } LargePage;

    struct {
        UINT32  DescType:               2;      /* [1:0]:   0b1X = Small page descriptor. Bit[0]: 0 = Executable, 1 = Execute-Never. */
        UINT32  Buffered:               1;      /* [2]:     Memory region attribute bits. Bufferable bit */
        UINT32  Cached:                 1;      /* [3]:     Memory region attribute bits. Cacheable bit. */
        UINT32  AP:                     2;      /* [5:4]:   Access Permissions bits[1:0]. */
        UINT32  TEX:                    3;      /* [8:6]:   Memory region attribute bits. */
        UINT32  AP2:                    1;      /* [9]:     Access Permissions bit[2]. */
        UINT32  Shared:                 1;      /* [10]:    Determines whether the translation is for Shareable memory. */
        UINT32  NotGlobal:              1;      /* [11]:    The not global bit. Used in the TLB matching process. */
        UINT32  BaseAddr:               20;     /* [31:12]: Small page base address, PA[31:12]. */
    } SmallPage;
} AMBA_TLB_VMSAV7_L2_DESC_s;

#define AMBA_NUM_TLB_L1_ENTRY           (1 << 12)   /* Available section base address, PA[31:20] */
#define AMBA_NUM_TLB_L2_SMALL_PAGE      (1 << 8)
#define AMBA_NUM_TLB_L2_ENTRY           (1 << 29) / (AMBA_NUM_TLB_L2_SMALL_PAGE * 4096)

typedef struct _AMBA_MMU_PAGE_TABLE_s_ {
    AMBA_TLB_VMSAV7_L1_DESC_s L1Desc[AMBA_NUM_TLB_L1_ENTRY];                                /* Occupy 16KB of L1 table (maps 1MB) */
    AMBA_TLB_VMSAV7_L2_DESC_s L2Desc[AMBA_NUM_TLB_L2_ENTRY][AMBA_NUM_TLB_L2_SMALL_PAGE];    /* Occupy AMBA_TLB_NUM_L2_ENTRIES of 1KB of L2 table (maps 4KB) */
    UINT32  L2DescCnt;                                                                      /* L2 allocation count */
} AMBA_MMU_PAGE_TABLE_s;

/*-----------------------------------------------------------------------------------------------*\
 * Defined in AmbaRTSL_MMU.c
\*-----------------------------------------------------------------------------------------------*/
void AmbaRTSL_MmuInit(void);
void AmbaRTSL_MmuEnable(void);
void AmbaRTSL_MmuDisable(void);
UINT32 AmbaRTSL_MmuVirtToPhys(UINT32 Virt);
UINT32 AmbaRTSL_MmuPhysToVirt(UINT32 Phys);
UINT32 AmbaRTSL_MmuTlbAddrGet(UINT32 *pMmuTblAddr);

#endif  /* _AMBA_RTSL_MMU_DEF_H_ */
