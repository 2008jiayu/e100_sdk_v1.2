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

#ifndef _AMBA_NOR_PARTITION_TABLE_H_
#define _AMBA_NOR_PARTITION_TABLE_H_

#include "AmbaNOR_Def.h"

/*-----------------------------------------------------------------------------------------------*\
 * Blocks reserved for NAND bad blocks table.
\*-----------------------------------------------------------------------------------------------*/
#define BOOT_CMD_LINE_SIZE              1024

#define NOR_PTB_SIZE                        4096
#define NOR_PTB_PAD_SIZE                    \
    (NOR_PTB_SIZE - AMBA_NUM_FW_PARTITION * sizeof(AMBA_NOR_PART_s) - sizeof(AMBA_NOR_PARAM_s))

#define FW_MODEL_NAME_SIZE              128
#define FW_PROG_MAGIC_SIZE              4
#define PART_NAME_LEN                   32

/*-----------------------------------------------------------------------------------------------*\
 * Flash partition entry. This structure describes a partition allocated
 * on the embedded flash device for either:
\*-----------------------------------------------------------------------------------------------*/
typedef struct _AMBA_NOR_PART_s_ {
    UINT32  Crc32;      /* CRC32 checksum of image  */
    UINT32  VerNum;     /* Version number   */
    UINT32  VerDate;    /* Version date     */
    UINT32  ImgLen;     /* Lengh of image in the partition  */
    UINT32  MemAddr;    /* Starting address to copy to RAM  */
    UINT32  Flag;       /* Special properties of this partition */
    UINT32  Magic;      /* Magic number */
} __attribute__((packed)) AMBA_NOR_PART_s;

/* These are flags set on a firmware partition table entry */
#define PART_LOAD		0x0		/**< Load partition data */
#define PART_NO_LOAD		0x1		/**< Don't load part data */
#define PART_COMPRESSED		0x2		/**< Data is not compressed */
#define PART_READONLY		0x4		/**< Data is RO */
#define PART_INCLUDE_OOB	0x8		/**< Data include oob info*/
#define PART_ERASE_APARTS	0x10		/**< Erase Android partitions */


/*-----------------------------------------------------------------------------------------------*\
 * Properties of the network device
\*-----------------------------------------------------------------------------------------------*/
typedef struct _AMBA_NOR_NET_DEV_s_ {
    UINT8   Mac[6];     /* MAC address                  */
    UINT8   Rsv[2];
    UINT32  IP;         /* Bootloader's LAN IP address  */
    UINT32  Mask;       /* Bootloader's LAN mask        */
    UINT32  Gateway;    /* Bootloader's LAN gateway     */
} AMBA_NOR_NET_DEV_s;

/*-----------------------------------------------------------------------------------------------*\
 * Properties of the target device that is stored in the flash.
\*-----------------------------------------------------------------------------------------------*/
typedef struct _AMBA_NOR_PARAM_s_ {
    char    SN[32];                         /* Serial number */
    UINT8   UsbDlMode;                      /* USB download mode */
    UINT8   AutoBoot;                       /* Automatic boot */
    UINT8   Rsv[2];
    char    CmdLine[BOOT_CMD_LINE_SIZE];    /* Boot command line options */
    UINT32  SplashID;

    /* This section contains networking related settings */
    AMBA_NOR_NET_DEV_s Wifi[2];

    UINT32  Magic;                          /* Magic number */
} __attribute__((packed)) AMBA_NOR_PARAM_s;

/*-----------------------------------------------------------------------------------------------*\
 *  The partition table is a region in flash where meta data about
 *  different partitions are stored.
\*-----------------------------------------------------------------------------------------------*/
typedef struct _AMBA_NOR_PART_TABLE_s_ {
    AMBA_NOR_PART_s    Part[AMBA_NUM_FW_PARTITION]; /**< Partitions */
    AMBA_NOR_PARAM_s   Param;              /**< Device properties */
    UINT8               Rsv[NOR_PTB_PAD_SIZE];  /**< Padding to 2048 bytes */
} __attribute__((packed)) AMBA_NOR_PART_TABLE_s;

#define PART_MAGIC_NUM              0x8732dfe6

/*-----------------------------------------------------------------------------------------------*\
 * Defined in AmbaNAND_PartitionTable.c
\*-----------------------------------------------------------------------------------------------*/
int AmbaNOR_GetPtb(AMBA_NOR_PART_TABLE_s * pTable);
int AmbaNOR_SetPtb(AMBA_NOR_PART_TABLE_s * pTable);

#endif
