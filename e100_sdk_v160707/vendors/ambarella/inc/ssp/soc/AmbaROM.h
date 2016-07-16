/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaROM.h
 *
 *  @Copyright      :: Copyright (C) 2012 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Definitions & Constants for Read Only Memory for NAND APIs
\*-------------------------------------------------------------------------------------------------------------------*/

#ifndef _AMBA_ROM_H_
#define _AMBA_ROM_H_

/*-----------------------------------------------------------------------------------------------*\
 * ROM driver definitions.
\*-----------------------------------------------------------------------------------------------*/

typedef enum _AMBA_ROM_DEVICE_e_ {
    AMBA_ROM_DEVICE_NAND = 0,
    AMBA_ROM_DEVICE_NOR,
    AMBA_ROM_DEVICE_SM,
} AMBA_ROM_DEVICE_e;

typedef enum _AMBA_ROM_REGION_e_ {
    AMBA_ROM_DSP_UCODE = 0,     /* for DSP uCode */
    AMBA_ROM_SYS_DATA,          /* for System ROM Data */

    AMBA_NUM_ROM_REGION         /* Total number of Regions in the ROM */
} AMBA_ROM_REGION_e;

#define ROM_BB_MAX                  128
#define ROM_PAGE_SIZE               2048
#define ROM_HEADER_MAX_SIZE         1048576     /* 1 MB for ROM header */
#define ROM_INODE_MAX               600
#define ROM_NAND                    0
#define ROM_NOR                     1
#define ROM_SM                      2
#define ROM_RETRY                   1

#define ALIGN_TO_4BITS              3
#define G_ROM_HEADER_ADDR           0xc0100000

#define RF_INODE_DIR_TYPE           0
#define RF_INODE_FILE_TYPE          1

#ifdef _AMBA_ROM_IMPL_

/*-----------------------------------------------------------------------------------------------*\
 * Meta data used for ROM.
\*-----------------------------------------------------------------------------------------------*/
#define ROM_META_SIZE               2048
#define ROM_META_MAGIC              0x66fc328a
#define ROM_HEADER_MAGIC            0x2387ab76

typedef struct _AMBA_ROM_META_ {
    unsigned int  FileCount;
    unsigned int  Magic;
    char    Padding[2040];
} AMBA_ROM_META_s;

/*-----------------------------------------------------------------------------------------------*\
 * Header internal to ROM.
\*-----------------------------------------------------------------------------------------------*/
typedef struct _AMBA_ROM_HEADER_ {
    char    Name[116];
    unsigned int  Offset;
    unsigned int  Size;
    unsigned int  Magic;
} AMBA_ROM_HEADER_s;

/*-----------------------------------------------------------------------------------------------*\
 * Used by the AmbaRomGen parser and runtime.
\*-----------------------------------------------------------------------------------------------*/
typedef struct _AMBA_ROM_LIST_ {
    char *pFile;
    char *pAlias;
    unsigned int Size;
    unsigned int Offset;
    unsigned int Padding;
    struct _AMBA_ROM_LIST_ *pNext;
} AMBA_ROM_LIST_s;

/*-----------------------------------------------------------------------------------------------*\
 * Used by the AmbaRomGen parser.
\*-----------------------------------------------------------------------------------------------*/
typedef struct _AMBA_ROM_PARSED_ {
    char *pTop;
    struct _AMBA_ROM_LIST_ *pList;
} AMBA_ROM_PARSED_s;

/*-----------------------------------------------------------------------------------------------*\
 * ROM run-time node that is kept in DRAM.
\*-----------------------------------------------------------------------------------------------*/
typedef struct _AMBA_ROM_NODE_ {
    AMBA_ROM_HEADER_s *pHdr;

    struct {
        int     Valid;
        UINT32  StartBlock;
        UINT32  StartPage;
    } RomPos;

    UINT32  Requests;                   /* Number of requests */
} AMBA_ROM_NODE_s;

#ifndef _AMBA_HOST_UTIL_
/*-----------------------------------------------------------------------------------------------*\
 * Object data used for ROM.
\*-----------------------------------------------------------------------------------------------*/
typedef struct _AMBA_ROM_OBJ_ {
    struct {
        UINT32 StartBlock;
        UINT32 StartPage;
        UINT32 MetaPages;
        UINT32 HeaderPages;
        UINT32 RomBlocks;
    } RomPos;                       /* Rom partition position */

    struct {
        UINT32 BlockSize;
        UINT32 PageSize;
        UINT32 PagesPerBlock;
    } RomInfo;

    AMBA_ROM_HEADER_s *pHdr;        /* Aligned pointer to rom headers */
    AMBA_ROM_NODE_s   *pTable;      /* Table of files */
    UINT8           *pBuf;
    UINT32          Count;          /* Count of files */
    UINT8           Type;           /* NAND or Nor*/
    UINT8           Init;

    AMBA_KAL_SEM_t    MutexSem;
} AMBA_ROM_OBJ_s;

#endif
#endif

#ifndef _AMBA_HOST_UTIL_
/*-----------------------------------------------------------------------------------------------*\
 * ROM info object data.
\*-----------------------------------------------------------------------------------------------*/
typedef struct _AMBA_ROM_INFO_ {
    char    Name[16];
    UINT32  TotalBlocks;
    UINT32  FreeBlocks;
    UINT32  BlockSize;
    UINT32  SectorsPerBlock;
} AMBA_ROM_INFO_s;

/*-----------------------------------------------------------------------------------------------*\
 * This structure is used to represent a tree-structure of the ROM.
\*-----------------------------------------------------------------------------------------------*/
typedef struct _AMBA_ROM_INODE_ {
    struct _AMBA_ROM_INODE_ *pParent;       /* Parent directory */
    struct _AMBA_ROM_INODE_ *pChild;        /* Child list */
    struct _AMBA_ROM_INODE_ *pSibling;      /* Next sibling */

    UINT32  Type;
#define AMBA_ROMFS_NAME_LENGTH      128
    char    Name[AMBA_ROMFS_NAME_LENGTH];   /* Name */
    int     Index;                          /* Index */
} AMBA_ROM_INODE_s;

int AmbaROM_GetCounts(UINT32 RomID);
int AmbaROM_FileExists(UINT32 RomID, const char *pFile);
int AmbaROM_GetName(UINT32 RomID, int Index, char *pName, UINT32 Len);
int AmbaROM_GetIndex(UINT32 RomID, const char *pFile);
int AmbaROM_GetSize(UINT32 RomID, const char *pFile, int Index);
int AmbaROM_LoadByName(UINT32 RomID, const char *pFile, UINT8 *pPtr, UINT32 Len, UINT32 Fpos);
int AmbaROM_LoadByIndex(UINT32 RomID, int Index, UINT8 *pPtr, UINT32 Len, UINT32 Fpos);
void AmbaROM_GetVolInfo(UINT32 RomID, AMBA_ROM_INFO_s *pVolInfo);
int AmbaROM_IsInit(UINT32 RomID);

int AmbaROM_Init(AMBA_KAL_BYTE_POOL_t *pCachedHeap, UINT32 RomDevice);

#endif /* _AMBA_HOST_UTIL_ */
#endif /* _AMBA_ROM_H_ */
