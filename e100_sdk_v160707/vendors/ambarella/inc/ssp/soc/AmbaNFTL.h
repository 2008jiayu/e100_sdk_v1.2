/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaNFTL.h
 *
 *  @Copyright      :: Copyright (C) 2012 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Definitions & Constants for NAND flash translation layer APIs.
\*-------------------------------------------------------------------------------------------------------------------*/

#ifndef _AMBA_NFTL_H_
#define _AMBA_NFTL_H_

//#define ENABLE_VERIFY_NFTL                  	1
#define NFTL_ENABLE_ICHUNK                      1

/* The max number that bad block handler can be called */
#define NFTL_BBM_HANDLER_MAX                    15

/* The max number of zone */
#define NFTL_MAX_ZONE                           256

/* Number of 32-bit word to store used sectors bit pattern */
#define MAX_BIT_PAT_32                          (NAND_SPB_256 / 32)

/* Define for nftl error codes. */
#define NFTL_FATAL_ER                           -11
#define NFTL_GENERAL_ER                         -12

/* Minimun number of translation tables in trl_blocks */
#define NFTL_TRL_TABLES                         16
#define NFTL_SS_BLK_PAGES                       1
#define NFTL_IMARK_PAGES                        1
#define NFTL_BBINFO_SIZE                        NAND_BBINFO_SIZE

/* NFTL initialize mode */
#define NFTL_MODE_NO_SAVE_TRL_TBL               0x0
#define NFTL_MODE_SAVE_TRL_TBL                  0x1

/*-----------------------------------------------------------------------------------------------*\
 * NFTL information
\*-----------------------------------------------------------------------------------------------*/
#define NFTL_UNUSED_BLK                         0xffff

#define NFTL_FROM_DEV                           0
#define NFTL_FROM_BUF                           1
#define NFTL_FROM_BBINFO                        2
#define NFTL_FROM_BBINFO_ICHUNK                 3

/*-----------------------------------------------------------------------------------------------*\
 * Definition for NAND flash translation layer
\*-----------------------------------------------------------------------------------------------*/
#define NFTL_PSECTS                             0
#define NFTL_PSECTS_NEW_BLOCK                   1
#define NFTL_BLOCKS                             2
#define NFTL_BLOCKS_PSECTS                      3
#define NFTL_BLOCKS_PSECTS_NEW_BLOCK            4

#define NFTL_DEBUG_MBUF_SIZE                    (128 * 2048)
#define NFTL_DEBUG_SBUF_SIZE                    (64 * 128)
#define NFTL_SPARE_BUF_SIZE                     (64 * 128)
#define NFTL_PAGE_BUF_SIZE                      2048
#define NFTL_BB_INFO_SIZE                       NFTL_BBINFO_SIZE
#define NFTL_CHUNK_BUF_SIZE                     (8 * 2048)

/*-----------------------------------------------------------------------------------------------*\
 * Definitions of NFTL instance.
\*-----------------------------------------------------------------------------------------------*/
#define NFTL_ID_STORAGE                         MP_Storage0
#define NFTL_ID_STORAGE2                        MP_Storage1
#define NFTL_ID_IDX                             MP_IndexForVideoRecording
#define NFTL_ID_PRF                             MP_UserSetting
#define NFTL_ID_CAL                             MP_CalibrationData
#define NFTL_MAX_INSTANCE                       AMBA_NUM_FW_MEDIA_PARTITION

/*-----------------------------------------------------------------------------------------------*\
 * The max bad blocks could be in ichunk
\*-----------------------------------------------------------------------------------------------*/
#define NFTL_ICHUNK_MAX_BB                      16

/*-----------------------------------------------------------------------------------------------*\
 * Definitions for NFTL init mark
\*-----------------------------------------------------------------------------------------------*/
#define NFTL_IMARK_MAGIC                        0x27398067
#define NFTL_IMARK_VALID                        0xa5
#define NFTL_IMARK_INVALID                      0x0
#define NFTL_IMARK_SRL                          0x11335577

/*-----------------------------------------------------------------------------------------------*\
 * Descriptor for the NFTL arguments.
\*-----------------------------------------------------------------------------------------------*/
typedef struct _AMBA_NFTL_ARGS_s_ {
    UINT16 SecNum;              /* Start sector address in a page */
    UINT16 Page;                /* Start page address in a block */
    UINT16 Lba;                 /* Start logical block address */
    UINT16 SecsStart;           /* Sectors in start page */
    UINT16 PagesStartBlk;       /* Pages in start block */
    UINT16 SecsEndBlk;          /* Last sectors in start block */
    UINT16 Blocks;              /* Blocks to be transferred */
    UINT16 PagesEnd;            /* Pages in last block */
    UINT16 SecsEndPage;         /* Sectors in last page */
} AMBA_NFTL_ARGS_s;

/*-----------------------------------------------------------------------------------------------*\
 * Descriptor for the NFTL bad block management.
\*-----------------------------------------------------------------------------------------------*/
typedef struct _AMBA_NFTL_BBM_s_ {
    AMBA_NFTL_ARGS_s    Arg[NFTL_BBM_HANDLER_MAX];  /* Backup for the original request argument */

    UINT8   *pBuf;          /* Backup the original buffer pointer */
    UINT8   Condition;      /* Current NFTL write condition */
    UINT32  Pba;            /* Backup the original PBA */
    UINT32  *pPat;          /* Backup the original bit patern pointer */
    UINT32  Count;          /* Number of bad blk handler be called in the same wirte condition */
    UINT32  TotalCnt;       /* Total number of bad blk handler be called */
    UINT32  FailWrCnt;      /* Number of failed write operation */
    UINT32  FailRdCnt;      /* Number of failed read operation */
    UINT32  FailErCnt;      /* Number of failed erase operation */
} AMBA_NFTL_BBM_s;

/*-----------------------------------------------------------------------------------------------*\
 * Descriptor for the NFTL bad block managerment status.
\*-----------------------------------------------------------------------------------------------*/
typedef struct _AMBA_NFTL_BBM_STATUS_s_ {
    UINT32 NumBB;           /* Number of bad blocks */
    UINT32 *pBBInfo;        /* Bad block information */
    UINT32 TotalCnt;        /* Total number of bad blk handler be called */
    UINT32 FailWrCnt;       /* Number of failed write operation */
    UINT32 FailRdCnt;       /* Number of failed read operation */
    UINT32 FailErCnt;       /* Number of failed erase operation */
} AMBA_NFTL_BBM_STATUS_s;

/*-----------------------------------------------------------------------------------------------*\
 * Descriptor for the NFTL init mark to indicate which table chunk is valid
\*-----------------------------------------------------------------------------------------------*/
typedef struct _AMBA_NFTL_INIT_MARK_s_ {
    UINT32  InitNum;        /* Init number */
    UINT32  DeinitNum;      /* Deinit number */
    UINT32  Valid;          /* Indicate if this imark is valid */
    UINT32  Signature;      /* imark signature */
} AMBA_NFTL_INIT_MARK_s;

/*-----------------------------------------------------------------------------------------------*\
 * Descriptor for the NFTL table chunk to store the init tables and imark
\*-----------------------------------------------------------------------------------------------*/
typedef struct _AMBA_NFTL_TABLE_CHUNK_s_ {
    UINT8   TableRdy;               /* Flag to indicate translation table ready */
    UINT8   TChunkRdy;              /* Flag to indicate valid tchunk ready */
    UINT32  PagesPerInfo;           /* Pages per tchunk info */
    UINT32  PagesPerChunk;          /* Pages per tchunk */

    UINT32  StartTablePage;         /* Start page of tanslation table in tchunk_buf */
    UINT32  TablePages;             /* Pages of tanslation table in tchunk_buf */
    UINT32  StartBBInfoPage;        /* Start page of bad block info in tchunk_buf */
    UINT32  BBInfoPages;            /* Pages of bad block info in tchunk_buf */
    UINT32  StartSBlkPage;          /* Start page of start block per zone in tchunk_buf */
    UINT32  SBlkPages;              /* Pages of start block per zone in tchunk_buf */
    UINT32  StartIMarkPage;         /* Start page of init mark in tchunk_buf */
    UINT32  IMarkPages;             /* Pages of init mark in tchunk_buf */

    UINT8   *pChunkBuf;             /* Pointer to tchunk buf */
    UINT8   *pChunkBufRaw;          /* Raw buffer used for rel_mpl */
    UINT16  *pTrlTable;             /* Pointer to translation table */
    UINT32  *pBBInfo;               /* Pointer to bad block info table */
    UINT32  *pSBlk;                 /* Pointer to start block per zone table */
    AMBA_NFTL_INIT_MARK_s *pIMark;  /* Pointer to imark */
} AMBA_NFTL_TABLE_CHUNK_s;

/*-----------------------------------------------------------------------------------------------*\
 * Descriptor for the NFTL init chunk to store the table chunks
\*-----------------------------------------------------------------------------------------------*/
typedef struct _AMBA_NFTL_INIT_CHUNK_s_ {
    UINT8               Init;                       /* Init flag */
    UINT32              StartBlock;                 /* Start block of init chunk */
    UINT32              Blocks;                     /* Blocks of init chunk */
    UINT32              BBInfo[NFTL_ICHUNK_MAX_BB]; /* Bad block information of init chunk */
    UINT32              NumBB;                      /* Number of bad blocks of init chunk */
    UINT32              TChunkAddr;                 /* Table chunk address */
    UINT32              NumTChunks;                 /* Number of table chunks in init chunk */
    AMBA_NFTL_TABLE_CHUNK_s   TChunk;               /* Table chunk */
} AMBA_NFTL_INIT_CHUNK_s;

/*-----------------------------------------------------------------------------------------------*\
 * Descriptor for the NFTL object
\*-----------------------------------------------------------------------------------------------*/
typedef struct _AMBA_NFTL_OBJ_s_ {
    UINT32          Init;           /*Indicate the trial initialization */
    int             Mode;
    UINT32          ID;             /* ID for instances */

    UINT8   *pMainBuf;              /* Pointer to page data aligned buffer */
    UINT8   *pSpareBuf;             /* Pointer to spare data aligned buffer */
    UINT8   *pMainBufRaw;           /* Pointer to page data buffer */
    UINT8   *pSpareBufRaw;          /* Pointer to spare data buffer */
    UINT32  *pBBInfo;               /* Pointer to bad block information aligned buffer */
    UINT32  *pBBInfoRaw;            /* Pointer to bad block information buffer */

    UINT32  TotalBlks;              /* Total blocks in current partition, not include trl_blocks */
    UINT32  TotalSecs;              /* Total avaiable logical sectors */
    UINT32  TotalZones;             /* Total zones the partition */
    UINT32  PageType;               /* 2k page size or 512 byte page size */

    /* Required members to do translation */
    UINT16  LBlksPerZone;           /* Logical blocks per zone */
    UINT16  PBlksPerZone;           /* Physical blocks per zone */
    UINT16  StartBlkPart;           /* Start block of partition */
    UINT16  MaxZones;

    AMBA_NFTL_INIT_CHUNK_s  IChunk; /* Init chunk */

    UINT32  NumBB;                  /* Number of bad blocks */
    UINT32  SBlk[NFTL_MAX_ZONE];    /* Start block in a zone */
    UINT16  *pTrlTable;             /* Pointer to translation table */
    UINT16  *pPhyTable;             /* pointer to physical to logical table */


    AMBA_NFTL_BBM_s BBM;            /* Bad block managerment object */

    UINT8   WrProtect;              /* Partition is write protected or not? */

#ifdef ENABLE_VERIFY_NFTL
    UINT8   *pMainDebug;            /* Pointer to page data aligned buffer for debugging */
    UINT8   *pSpareDebug;           /* Pointer to spare data aligned buffer for debugging */
    UINT8   *pMainDebugRaw;         /* Pointer to page data buffer */
    UINT8   *pSpareDebugRaw;        /* Pointer to spare data buffer */
#endif
} AMBA_NFTL_OBJ_s;

/*-----------------------------------------------------------------------------------------------*\
 * Descriptor for the NFTL status
\*-----------------------------------------------------------------------------------------------*/
typedef struct _AMBA_NFTL_STATUS_s_ {
    UINT32 StartIChunkBlk;
    UINT32 IChunkBlks;
    UINT32 StartFtlBlk;
    UINT32 FtlBlocks;
    UINT16 LBlksPerZone;
    UINT16 PBlksPerZone;
    UINT32 NumBB;
    UINT32 TotalSecs;
    UINT32 TotalZones;
    UINT32 WrProtect;
} AMBA_NFTL_STATUS_s;

/*-----------------------------------------------------------------------------------------------*\
 * Descriptor for the report of NFTL unused sectors
\*-----------------------------------------------------------------------------------------------*/
typedef struct _AMBA_NFTL_USPAT_RPT_s_ {
    UINT32 Block;
    UINT32 Page;
    UINT32 Sector;
} AMBA_NFTL_USPAT_RPT_s;

/*-----------------------------------------------------------------------------------------------*\
 * Defined in AmbaNFTL.c (NAND flash translation layer MW)
\*-----------------------------------------------------------------------------------------------*/
int AmbaNFTL_InitLock(int ID);
int AmbaNFTL_DeinitLock(int ID);
int AmbaNFTL_IsWrProtect(int ID);
int AmbaNFTL_IsInit(int ID);
int AmbaNFTL_Init(int ID, int Mode);
int AmbaNFTL_Deinit(int ID);
int AmbaNFTL_ErasePart(int ID);
int AmbaNFTL_SyncTChunk(int ID);
int AmbaNFTL_Read(int ID, UINT8 *pBuf, UINT32 Sec, UINT32 Secs);
int AmbaNFTL_Write(int ID, UINT8 *pBuf, UINT32 Sec, UINT32 Secs);
int AmbaNFTL_EraseLogicalBlocks(int ID, UINT32 StartBlock, int Blocks);
int AmbaNFTL_GetTotalSecs(int ID, UINT32 *pTotalSecs);
int AmbaNFTL_GetStatus(int ID, AMBA_NFTL_STATUS_s *pStatus);
int AmbaNFTL_ReclaimBadBlks(int ID, int BBType);
int AmbaNFTL_CheckTrlTable(int ID, AMBA_NFTL_USPAT_RPT_s *pRpt, int Rpts);
int AmbaNFTL_GetBBMStatus(int ID, AMBA_NFTL_BBM_STATUS_s *pBBMStatus);

#endif  /* _AMBA_NFTL_H_ */

