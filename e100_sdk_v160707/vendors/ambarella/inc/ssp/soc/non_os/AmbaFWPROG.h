/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaFWPROG.h
 *
 *  @Copyright      :: Copyright (C) 2012 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Definitions & Constants for NAND flash firmware program utilities.
\*-------------------------------------------------------------------------------------------------------------------*/

#ifndef _AMBA_FWPROG_H_
#define _AMBA_FWPROG_H_

#define PART_MAGIC_NUM              0x8732dfe6
#define PART_HEADER_MAGIC_NUM       0xa324eb90

/* These are flags set on a firmware partition table entry */
#define PART_LOAD                   0x0     /* Load partition data */
#define PART_NO_LOAD                0x1     /* Don't load part data */
#define PART_COMPRESSED             0x2     /* Data is not compressed */
#define PART_READONLY               0x4     /* Data is RO */

/* Error codes */
#define FLPROG_OK                   0
#define FLPROG_ERR_MAGIC            -1
#define FLPROG_ERR_LENGTH           -2
#define FLPROG_ERR_CRC              -3
#define FLPROG_ERR_VER_NUM          -4
#define FLPROG_ERR_VER_DATE         -5
#define FLPROG_ERR_PROG_IMG         -6
#define FLPROG_ERR_FIRM_FILE        -9
#define FLPROG_ERR_FIRM_FLAG        -10
#define FLPROG_ERR_NO_MEM           -11
#define FLPROG_ERR_FIFO_OPEN        -12
#define FLPROG_ERR_FIFO_READ        -13
#define FLPROG_ERR_PAYLOAD          -14
#define FLPROG_ERR_ILLEGAL_HDR      -15
#define FLPROG_ERR_EXTRAS_MAGIC     -16
#define FLPROG_ERR_PREPROCESS       -17
#define FLPROG_ERR_POSTPROCESS      -18
#define FLPROG_ERR_NOT_READY        0x00001000

/*-----------------------------------------------------------------------------------------------*\
 * This is the header for a flash image partition.
 *-----------------------------------------------------------------------------------------------*/
typedef struct _AMBA_IMG_HEADER_ {
    UINT32  Crc32;              /* CRC32 Checksum       */
    UINT32  VerNum;             /* Version number       */
    UINT32  VerDate;            /* Version date         */
    UINT32  ImgLen;             /* Image length         */
    UINT32  MemAddr;            /* Location to be loaded into memory */
    UINT32  Flag;               /* Flag of partition    */
    UINT32  Magic;              /* The magic number     */
    UINT32  Reserved[57];
} AMBA_IMG_HEADER;

/*-----------------------------------------------------------------------------------------------*\
 * The following data structure is used by the memfwprog program to output
 * the flash programming results to a memory area.
 *-----------------------------------------------------------------------------------------------*/

#define FWPROG_RESULT_FLAG_LEN_MASK     0x00ffffff
#define FWPROG_RESULT_FLAG_CODE_MASK    0xff000000

#define FWPROG_RESULT_MAKE(Code, Len) \
    ((Code) << 24) | ((Len) & FWPROG_RESULT_FLAG_LEN_MASK)

typedef struct _AMBA_FWPROG_RESULT_ {
    UINT32  Magic;
#define FWPROG_RESULT_MAGIC 0xb0329ac3

    UINT32  BadBlockInfo;
#define BST_BAD_BLK_OVER    0x00000001
#define BLD_BAD_BLK_OVER    0x00000002
#define HAL_BAD_BLK_OVER    0x00000004
#define PBA_BAD_BLK_OVER    0x00000008
#define PRI_BAD_BLK_OVER    0x00000010
#define SEC_BAD_BLK_OVER    0x00000020
#define BAK_BAD_BLK_OVER    0x00000040
#define RMD_BAD_BLK_OVER    0x00000080
#define ROM_BAD_BLK_OVER    0x00000100
#define DSP_BAD_BLK_OVER    0x00000200

    UINT32  Flag[AMBA_NUM_FW_PARTITION];
} __attribute__((packed)) AMBA_FWPROG_RESULT_s;

#define AMBA_FWPROG_RESULT_ADDR  0x000ffe00

/*-----------------------------------------------------------------------------------------------*\
 * Defined in AmbaFWPROG.c
 *-----------------------------------------------------------------------------------------------*/
int AmbaNonOS_NandGetMeta(AMBA_NAND_PART_META_s *pMeta);
int AmbaNonOS_NandSetMeta(void);
int AmbaNonOS_NandGetPartTable(AMBA_NAND_PART_TABLE_s *pTable);
int AmbaNonOS_NandSetPartTable(AMBA_NAND_PART_TABLE_s *pTable);
int AmbaFWPROG_EraseEmmcPart(int PartID);
int AmbaFWPROG_EraseNandPart(int PartID);
int AmbaFWPROG_NandPart(int PartID, UINT8 *pImage, UINT32 Len);
int AmbaFWPROG_EmmcPart(int PartID, UINT8 *pImage, UINT32 Len);
const char *GetPartitionName(UINT32 PartID);

#endif /* _AMBA_RTSL_NAND_H_ */
