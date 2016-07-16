/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaNAND_PartitionTable.h
 *
 *  @Copyright      :: Copyright (C) 2012 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Definitions of Run Time Support Library for SD flash
\*-------------------------------------------------------------------------------------------------------------------*/

#ifndef _AMBA_NAND_PARTITION_TABLE_H_
#define _AMBA_NAND_PARTITION_TABLE_H_

#include "AmbaNAND_Def.h"

/*-----------------------------------------------------------------------------------------------*\
 * Blocks reserved for NAND bad blocks table.
\*-----------------------------------------------------------------------------------------------*/
#define NAND_BBT_RSV_BLOCKS             2

#define BOOT_CMD_LINE_SIZE              1024

#define PTB_SIZE                        4096
#define PTB_PAD_SIZE                    \
    (PTB_SIZE - AMBA_NUM_NAND_PARTITION * sizeof(AMBA_NAND_PART_s) - sizeof(AMBA_NAND_PARAM_s))

#define FW_MODEL_NAME_SIZE              128
#define FW_PROG_MAGIC_SIZE              4
#define PART_NAME_LEN                   32
#define PLOAD_REGION_NUM                6
#define PTB_META_ACTURAL_LEN            \
    ((sizeof(UINT32) * (PLOAD_REGION_NUM * 4 + 2)) + (sizeof(UINT32) * 2 + PART_NAME_LEN + sizeof(UINT32)) * AMBA_NUM_NAND_PARTITION + \
     sizeof(UINT32) + sizeof(UINT32) + FW_MODEL_NAME_SIZE + FW_PROG_MAGIC_SIZE + sizeof(UINT32) * TOTAL_FW_PARTS)

#define PTB_META_SIZE                   2048
#define PTB_META_PAD_SIZE               (PTB_META_SIZE - PTB_META_ACTURAL_LEN)
#define PTB_META_MAGIC                  0x33219fbd

/*-----------------------------------------------------------------------------------------------*\
 * Flash partition entry. This structure describes a partition allocated
 * on the embedded flash device for either:
\*-----------------------------------------------------------------------------------------------*/
typedef struct _AMBA_NAND_PART_s_ {
    UINT32  Crc32;      /* CRC32 checksum of image  */
    UINT32  VerNum;     /* Version number   */
    UINT32  VerDate;    /* Version date     */
    UINT32  ImgLen;     /* Lengh of image in the partition  */
    UINT32  MemAddr;    /* Starting address to copy to RAM  */
    UINT32  Flag;       /* Special properties of this partition */
    UINT32  Magic;      /* Magic number */
} __attribute__((packed)) AMBA_NAND_PART_s;

/*-----------------------------------------------------------------------------------------------*\
 * Properties of the network device
\*-----------------------------------------------------------------------------------------------*/
typedef struct _AMBA_NET_DEV_s_ {
    UINT8   Mac[6];     /* MAC address                  */
    UINT8   Rsv[2];
    UINT32  IP;         /* Bootloader's LAN IP address  */
    UINT32  Mask;       /* Bootloader's LAN mask        */
    UINT32  Gateway;    /* Bootloader's LAN gateway     */
} AMBA_NET_DEV_s;

/*-----------------------------------------------------------------------------------------------*\
 * Properties of the target device that is stored in the flash.
\*-----------------------------------------------------------------------------------------------*/
typedef struct _AMBA_NAND_PARAM_s_ {
    char    SN[32];                         /* Serial number */
    UINT8   UsbDlMode;                      /* USB download mode */
    UINT8   AutoBoot;                       /* Automatic boot */
    UINT8   Rsv[2];
    char    CmdLine[BOOT_CMD_LINE_SIZE];    /* Boot command line options */
    UINT32  SplashID;

    /* This section contains networking related settings */
    AMBA_NET_DEV_s Wifi[2];

    UINT32  Magic;                          /* Magic number */
} __attribute__((packed)) AMBA_NAND_PARAM_s;

/*-----------------------------------------------------------------------------------------------*\
 *  The partition table is a region in flash where meta data about
 *  different partitions are stored.
\*-----------------------------------------------------------------------------------------------*/
typedef struct _AMBA_NAND_PART_TABLE_s_ {
    AMBA_NAND_PART_s    Part[AMBA_NUM_NAND_PARTITION]; /**< Partitions */
    AMBA_NAND_PARAM_s   Param;              /**< Device properties */
    UINT8               Rsv[PTB_PAD_SIZE];  /**< Padding to 2048 bytes */
} __attribute__((packed)) AMBA_NAND_PART_TABLE_s;

typedef struct _AMBA_PLOAD_PARTITION_s_ {
    UINT32  RegionRoStart[PLOAD_REGION_NUM];
    UINT32  RegionRwStart[PLOAD_REGION_NUM];
    UINT32  RegionRoSize[PLOAD_REGION_NUM];
    UINT32  RegionRwSize[PLOAD_REGION_NUM];
    UINT32  DspBufStart;
    UINT32  DspBufSize;
} __attribute__((packed)) AMBA_PLOAD_PARTITION_s;

/*-----------------------------------------------------------------------------------------------*\
 * The meta data table is a region in flash after partition table.
 * The data need by dual boot are stored.
\*-----------------------------------------------------------------------------------------------*/
typedef struct _AMBA_NAND_PART_META_s_ {
    struct {
        UINT32  StartBlk;
        UINT32  NumBlks;
        char    Name[PART_NAME_LEN];
    } PartInfo[AMBA_NUM_NAND_PARTITION];

#define PART_MAGIC_NUM              0x8732dfe6
    UINT32  Magic;              /**< Magic number */
    UINT32  PartDev[AMBA_NUM_NAND_PARTITION];
    UINT8   ModelName[FW_MODEL_NAME_SIZE];

    AMBA_PLOAD_PARTITION_s PloadInfo;

    /* This meta crc32 doesn't include itself. */
    /* It's only calc data before this field.  */
    UINT32  Crc32;

    int     FwProgStatus[TOTAL_FW_PARTS];
    UINT8   FwProgMagic[FW_PROG_MAGIC_SIZE];

    UINT8   Rsv[PTB_META_PAD_SIZE];
} __attribute__((packed)) AMBA_NAND_PART_META_s;

/*-----------------------------------------------------------------------------------------------*\
 * Defined in AmbaNAND_PartitionTable.c
\*-----------------------------------------------------------------------------------------------*/
int AmbaNAND_GetPtb(AMBA_NAND_PART_TABLE_s * pTable);
int AmbaNAND_SetPtb(AMBA_NAND_PART_TABLE_s * pTable);
int AmbaNAND_GetMeta(AMBA_NAND_PART_META_s *pMeta);
int AmbaNAND_SetMeta(AMBA_NAND_PART_META_s *pMeta);

#endif
