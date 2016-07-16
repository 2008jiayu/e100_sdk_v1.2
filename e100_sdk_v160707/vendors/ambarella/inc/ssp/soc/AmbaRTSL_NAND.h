/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaRTSL_NAND.h
 *
 *  @Copyright      :: Copyright (C) 2012 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Definitions of Run Time Support Library for NAND flash
\*-------------------------------------------------------------------------------------------------------------------*/

#ifndef _AMBA_RTSL_NAND_H_
#define _AMBA_RTSL_NAND_H_

#include "AmbaNAND_Def.h"

/*-----------------------------------------------------------------------------------------------*\
 * Indicate the partition is on which device.
\*-----------------------------------------------------------------------------------------------*/
#define PART_ON_NAND                    0

/*-----------------------------------------------------------------------------------------------*\
 * NAND device status definitions.
\*-----------------------------------------------------------------------------------------------*/
#define NAND_STS_IO0_ERROR              0x1
#define NAND_STS_IO1_ERROR              0x2
#define NAND_STS_IO5_READY              0x20
#define NAND_STS_IO6_READY              0x40
#define NAND_STS_IO7_NOWP               0x80

/*-----------------------------------------------------------------------------------------------*\
 * NAND operations error code definitions.
\*-----------------------------------------------------------------------------------------------*/
#define NAND_OP_DRIVER_ER               -1
#define NAND_OP_ERASE_ER                -2
#define NAND_OP_PROG_ER                 -3
#define NAND_OP_READ_ER                 -4
#define NAND_OP_WP_ER                   -5
#define NAND_OP_NOT_READY_ER            -6
#define NAND_OP_MARK_BBLK_ER            -7
#define NAND_OP_BCH_FAILED              -8
#define NAND_OP_BCH_CORRECTED           -9

/*-----------------------------------------------------------------------------------------------*\
 * NAND device information
\*-----------------------------------------------------------------------------------------------*/
#define NAND_MAIN_512                   512     /* Small page (512 B) */
#define NAND_MAIN_2K                    2048    /* Large page (2K B) */
#define NAND_SPARE_16                   16      /* 16 B spare size for small page */
#define NAND_SPARE_64                   64      /* 64 B spare size for large page */
#define NAND_SPARE_128                  128     /* 128 B spare size for 8-bit ECC NAND flash */
#define NAND_PPB_32                     32      /* 32 pages per block */
#define NAND_PPB_64                     64      /* 64 pages per block */
#define NAND_SPP_4                      4       /* 4 sectors per page */
#define NAND_SPP_1                      1       /* 1 sector per page */
#define NAND_SPB_32                     32      /* 32 sectors per block */
#define NAND_SPB_256                    256     /* 256 sectors per block */

#define NAND_MAIN_512_SHT               9       /* main size 512 byte */
#define NAND_MAIN_2K_SHT                11      /* main size 2k byte */
#define NAND_SPARE_16_SHT               4       /* spare size 16 byte */
#define NAND_SPARE_32_SHT               5       /* spare size 32 byte */
#define NAND_SPARE_64_SHT               6       /* spare size 64 byte */
#define NAND_SPARE_128_SHT              7       /* spare size 128 byte */
#define NAND_PPB_32_SHT                 5       /* 32 pages per block */
#define NAND_PPB_64_SHT                 6       /* 64 pages per block */
#define NAND_SPP_4_SHT                  2       /* 4 sectors per page */
#define NAND_SPB_32_SHT                 5       /* 32 sectors per block */
#define NAND_SPB_256_SHT                8       /* 256 sectors per block */

/* Define for plane mapping */

/* Device does not support copyback command */
#ifndef NAND_PLANE_MAP_0
#define NAND_PLANE_MAP_0    0
#endif
/* plane address is according the lowest block address */
#ifndef NAND_PLANE_MAP_1
#define NAND_PLANE_MAP_1    1
#endif
/* plane address is according the highest block address */
#ifndef NAND_PLANE_MAP_2
#define NAND_PLANE_MAP_2    2
#endif
/* plane address is according the lowest and highest block address */
#ifndef NAND_PLANE_MAP_3
#define NAND_PLANE_MAP_3    3
#endif

/*-----------------------------------------------------------------------------------------------*\
 * NAND Flash sector definition
\*-----------------------------------------------------------------------------------------------*/
#define NAND_SEC_SIZE                   512
#define NAND_SEC_SHT                    9

/*-----------------------------------------------------------------------------------------------*\
 * Number of ECC bits
\*-----------------------------------------------------------------------------------------------*/
#define NAND_ECC_BIT_1                  0x1
#define NAND_ECC_BIT_6                  0x6
#define NAND_ECC_BIT_8                  0x8

/*-----------------------------------------------------------------------------------------------*\
 * Number of BCH bytes and BCH start byte in spare area
\*-----------------------------------------------------------------------------------------------*/
#define NAND_SPARE_BCH_START            6
#define NAND_SPARE_BCH6_COUNT           10
#define NAND_SPARE_BCH8_COUNT           13

#define NAND_OUT_OF_SPACE               0x1
#define NAND_CAP_CACHE_CB               0x1
#define NAND_CAP_CACHE_PROG             0x2

/*-----------------------------------------------------------------------------------------------*\
 * NAND Flash programmable command word options
\*-----------------------------------------------------------------------------------------------*/
#define RESET_ALL_CMD_WORD              0
#define RESET_PROG_CMD_WORD             1
#define RESET_READ_CMD_WORD             2
#define SET_PROG_CMD_WORD               3
#define SET_READ_CMD_WORD               4

/*-----------------------------------------------------------------------------------------------*\
 * Initial bad block        : page 0|1 is not 0xff.
 * Late developed bad block : Page 2|3 is not 0xff.
\*-----------------------------------------------------------------------------------------------*/
#define INIT_BAD_BLOCK_PAGES            2
#define LATE_BAD_BLOCK_PAGES            2
#define BAD_BLOCK_PAGES                 (INIT_BAD_BLOCK_PAGES + LATE_BAD_BLOCK_PAGES)

/*-----------------------------------------------------------------------------------------------*\
 * Ambarella bad block marker. Used to mark the late developed bad block by Ambarella nand software.
\*-----------------------------------------------------------------------------------------------*/
#define AMBA_BAD_BLOCK_MARK             0x0

#define NAND_GOOD_BLOCK                 0x0
#define NAND_INIT_BAD_BLOCK             0x1
#define NAND_LATE_BAD_BLOCK             0x2
#define NAND_OTHER_BAD_BLOCK            0x4

/*-----------------------------------------------------------------------------------------------*\
 * Definitions for NFTL partitions config.
\*-----------------------------------------------------------------------------------------------*/
#define NAND_TRL_TABLES                 16
#define NAND_SS_BLK_PAGES               1
#define NAND_IMARK_PAGES                1
#define NAND_BBINFO_SIZE                (4 * 1024)

#define STG1_ZONE_THRESHOLD             1000
#define STG1_RSV_BLOCKS_PER_ZONET       24
#define STG1_MIN_RSV_BLOCKS_PER_ZONE    24
#define STG1_TRL_TABLES                 NAND_TRL_TABLES

#define STG2_ZONE_THRESHOLD             1000
#define STG2_RSV_BLOCKS_PER_ZONET       24
#define STG2_MIN_RSV_BLOCKS_PER_ZONE    24
#define STG2_TRL_TABLES                 NAND_TRL_TABLES

#define IDX_ZONE_THRESHOLD              1000
#define IDX_RSV_BLOCKS_PER_ZONET        24
#define IDX_MIN_RSV_BLOCKS_PER_ZONE     24
#define IDX_TRL_TABLES                  NAND_TRL_TABLES

#define PRF_ZONE_THRESHOLD              1000
#define PRF_RSV_BLOCKS_PER_ZONET        24
#define PRF_MIN_RSV_BLOCKS_PER_ZONE     5
#define PRF_TRL_TABLES                  0

#define CAL_ZONE_THRESHOLD              1000
#define CAL_RSV_BLOCKS_PER_ZONET        24
#define CAL_MIN_RSV_BLOCKS_PER_ZONE     5
#define CAL_TRL_TABLES                  0

/*-----------------------------------------------------------------------------------------------*\
 * NAND device information.
\*-----------------------------------------------------------------------------------------------*/
typedef struct _AMBA_NAND_DEV_s_ {
    AMBA_NAND_DEV_INFO_s *pNandInfo;

    UINT32  Ctrl;

    UINT32  ID;

    char    Name[128];                  /**< Name */
    UINT8   Present;                    /**< Device is present */
    int     Format[2];                  /**< File system format ID for STG */
    UINT32  Status;                     /**< Status of NAND devices */
    UINT64  DsmDWAddr64;                /**< 64 bits Address of DSM dummy write block */

    /** Device logic info */
    struct {
        UINT16  Cap;
        UINT16  PlaneMap;
        UINT16  Intlve;                 /**< Number of interleave */
        UINT16  MainSize;               /**< Main area size */
        UINT16  SpareSize;              /**< Spare area size */
        UINT16  PageSize;               /**< Page size */
        UINT16  SectorSize;             /**< Sector size */
        UINT16  PagesPerBlock;          /**< Pages per block */
        UINT16  SectorsPerPage;         /**< Sectors per page */
        UINT16  SectorsPerBlock;        /**< Sectors per block */
        UINT16  BlocksPerPlane;         /**< Blocks per plane */
        UINT16  TotalBlocks;            /**< Total blocks */
        UINT16  TotalPlanes;            /**< Total planes */
        UINT16  TotalZones;             /**< Total zones */
    } DevLogicInfo;

    /** Device partition info */
    struct {
        INT32   StartBlock;             /**< Start block of device partition */
        INT32   StartTrlTbl;            /**< Starting block of nftl init tbl */
        INT32   TrlBlocks;              /**< Blocks used for translation tbl */
        INT32   PagesPerTchunk;         /**< Pages of tchunk */
        INT32   PartStartBlk;           /**< Starting block of ftl partition */
        INT32   FtlBlocks;              /**< Total blocks in ftl partition */
        INT32   TotalZones;             /**< Total zones in ftl partition */
        INT32   TotalBlocks;            /**< Total blocks in partition */
        INT32   PBlksPerZone;           /**< Physical blocks per zone */
        INT32   LBlksPerZone;           /**< Logical blocks per zone */
        INT32   RBlksPerZone;           /**< Reserved blocks per zone */
    } DevPartInfo[AMBA_NUM_FW_MEDIA_PARTITION];

    /** Chip(s) timing parameters */
    struct {
        UINT32  Tim0;
        UINT32  Tim1;
        UINT32  Tim2;
        UINT32  Tim3;
        UINT32  Tim4;
        UINT32  Tim5;
    } Timing;
} AMBA_NAND_DEV_s;

/*-----------------------------------------------------------------------------------------------*\
 * NAND host controller.
\*-----------------------------------------------------------------------------------------------*/
typedef struct _AMBA_NAND_HOST_s_ {
    AMBA_NAND_DEV_s Device;       /**< The NAND device chip(s) */

    UINT32 LastCmd;

    UINT8   *pDWriteMain;
    UINT8   *pDWriteSpare;
} AMBA_NAND_HOST_s;

/*-----------------------------------------------------------------------------------------------*\
 * NAND with page 512 byte spare area layout. It follows the Samsung's
 * NAND spare area definition and with some private definition field.
\*-----------------------------------------------------------------------------------------------*/
typedef struct _NAND_SPARE_LAYOUT_SMALL_s_ {
    UINT8   Lsn[3];     /* Logical sector number */
    UINT16  Rsv1;       /* Reserved */
    UINT8   BI;         /* Bad block information */
    UINT8   Ecc[3];     /* Ecc code for main area data */
    UINT16  LsnEcc;     /* Ecc code for lsn data */
    UINT32  UspBlk;     /* Used sectors pattern in a block */
    UINT8   Rsv2;       /* Reserved */
} __attribute__((packed)) NAND_SPARE_LAYOUT_SMALL_s;

/*-----------------------------------------------------------------------------------------------*\
 * NAND with page 2K byte spare area layout. It follows the Samsung's
 * NAND spare area definition and with some private definition field.
\*-----------------------------------------------------------------------------------------------*/
typedef struct _NAND_SPARE_LAYOUT_LARGE_s_ {
    UINT8   BI;         /* Bad block information */
    UINT8   Rsv1;       /* Reserved */
    UINT8   Lsn[3];     /* Logical sector number */
    UINT8   Pub;        /* Page used bit */
    UINT16  Rsv2;       /* Reserved */
    UINT8   Ecc[3];     /* Ecc code for main area data */
    UINT16  LsnEcc;     /* Ecc code for lsn data */
    UINT8   Rsv3[3];    /* Used sectors bit pattern */
} __attribute__((packed)) NAND_SPARE_LAYOUT_LARGE_s;

#if 0
/*-----------------------------------------------------------------------------------------------*\
 * For 6-bit ECC.
 * NAND with page 2K byte spare area layout. It follows the Samsung's
 * NAND spare area definition and with some private definition field.
\*-----------------------------------------------------------------------------------------------*/
typedef struct nand_big_spare_e6b_s {
    UINT8   BI;         /* Bad block information */
    UINT8   Rsv1;       /* Reserved */
    UINT8   LSN[3];     /* Logical sector number */
    UINT8   PUB;        /* Page used bit */
    UINT8   Ecc[10];    /* 6-bit ECC protects 512 B Main and the first 6 B spare */
} nand_big_spare_e6b_t;

/*-----------------------------------------------------------------------------------------------*\
 * For 8-bit ECC. Spare area should be 64 Bytes / 512 Bytes.
 * NAND with page 2K byte spare area layout. It follows the Samsung's
 * NAND spare area definition and with some private definition field.
\*-----------------------------------------------------------------------------------------------*/
typedef struct nand_big_spare_e8b_s {
    u8 bi;      /* bad block information */
    u8 rsv;     /* reserved area */
    u8 lbn[2];  /* logical block number */
    u8 lsn;     /* local sector number */
    u8 usp1;    /* used sectors bit pattern */
    u8 ecc[13]; /* 8-bit ECC protects 512 B Main and the first 6 B spare */
    u8 rsv2[13];    /* reserved area 2 */
} nand_big_spare_e8b_t;
#endif

/*-----------------------------------------------------------------------------------------------*\
 * Data structure describing partitions in NAND flash.
\*-----------------------------------------------------------------------------------------------*/
typedef struct _AMBA_NAND_FW_PART_s_ {
    UINT32  StartBlk[TOTAL_FW_PARTS];           /* Start block */
    UINT32  NumBlks[TOTAL_FW_PARTS];            /* Number of blocks */
} AMBA_NAND_FW_PART_s;

/*-----------------------------------------------------------------------------------------------*\
 * Data structure describing partitions in NAND flash.
\*-----------------------------------------------------------------------------------------------*/
typedef struct _AMBA_NAND_NFTL_PART_s_ {
    UINT32  StartBlk[AMBA_NUM_FW_MEDIA_PARTITION];           /* Start block */
    UINT32  NumBlks[AMBA_NUM_FW_MEDIA_PARTITION];            /* Number of blocks */
    UINT32  RsvBlk[AMBA_NUM_FW_MEDIA_PARTITION];             /* Number of reserved blocks */
    UINT32  NumZones[AMBA_NUM_FW_MEDIA_PARTITION];           /* Number of zones */
    UINT32  NumTrlBlks[AMBA_NUM_FW_MEDIA_PARTITION];         /* Number of translation tables in trl_blocks */
} AMBA_NAND_NFTL_PART_s;

/*-----------------------------------------------------------------------------------------------*\
 * Defined in AmbaRTSL_NAND.c (RTSL for NAND flash)
\*-----------------------------------------------------------------------------------------------*/

void    AmbaRTSL_NandCalFWPartInfo(AMBA_NAND_DEV_s *pDev);
void    AmbaRTSL_NandCalNftlPart(AMBA_NAND_DEV_s *pDev);
void    AmbaRTSL_NandWaitInt(void);
AMBA_NAND_HOST_s *AmbaRTSL_NandGetHost(void);
AMBA_NAND_DEV_s *AmbaRTSL_NandGetDev(void);
void    AmbaRTSL_NandReset(void);
void    AmbaRTSL_NandReadIDCmd(AMBA_NAND_DEV_s *pDev);
void    AmbaRTSL_NandReadIDResult(AMBA_NAND_DEV_s *pDev);
void    AmbaRTSL_NandReadStatusCmd(AMBA_NAND_DEV_s *pDev);
UINT32  AmbaRTSL_NandReadStatusResult(void);
void    AmbaRTSL_NandCopyback(AMBA_NAND_HOST_s *pHost, UINT32 AddrHi, UINT32 Addr, UINT32 Dest);
void    AmbaRTSL_NandErase(AMBA_NAND_DEV_s *pDev, UINT32 AddrHi, UINT32 Addr);
void    AmbaRTSL_NandRead(AMBA_NAND_HOST_s *pHost, UINT32 AddrHi, UINT32 Addr,
                          UINT8 *pMain, UINT8 *pSpare, UINT32 MainLen, UINT32 SpareLen, UINT32 Area);
void    AmbaRTSL_NandProgram(AMBA_NAND_HOST_s *pHost, UINT32 AddrHi, UINT32 Addr,
                             UINT8 *pMain, UINT8 *pSpare, UINT32 MainLen, UINT32 SpareLen, UINT32 Area);
void    AmbaRTSL_NandSetTiming(AMBA_NAND_DEV_s *pDev);
void    AmbaRTSL_NandSetCmdWord(int Operation, UINT32 CmdWord1, UINT32 CmdWord2);
void    AmbaRTSL_NandGetRsvBlks(int NumBlk, int ZoneT, int RsvPerZone, int MinRsv,
                                int *pZones, int *pRsvPerZone);
void    AmbaRTSL_NandHandleCoreFreqChange(void);
int     AmbaRTSL_NandInit(AMBA_NAND_HOST_s *pHost, AMBA_NAND_DEV_s *pDev);
int     AmbaRTSL_NandSetDevInfo(AMBA_NAND_DEV_s *pDev, const AMBA_NAND_DEV_INFO_s *pNandDevInfo);
void    AmbaRTSL_NandSetIsrTarget(int CpuId);
void    AmbaRTSL_NandIsrCtrl(UINT32 Enable);

#endif /* _AMBA_RTSL_NAND_H_ */
