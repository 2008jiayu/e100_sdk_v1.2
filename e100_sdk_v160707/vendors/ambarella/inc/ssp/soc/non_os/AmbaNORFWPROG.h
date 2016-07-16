/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaNORFWPROG.h
 *
 *  @Copyright      :: Copyright (C) 2012 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Definitions & Constants for NAND flash firmware program utilities.
\*-------------------------------------------------------------------------------------------------------------------*/

#ifndef _AMBA_NORFWPROG_H_
#define _AMBA_NORFWPROG_H_

#if 0
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
#endif
#define AMBOOT_BST_FIXED_SIZE	(2048)

/*-----------------------------------------------------------------------------------------------*\
 * SPINOR_LENGTH_REG
\*-----------------------------------------------------------------------------------------------*/
typedef union _AMBA_NOR_IMGLEN_REG_u_ {
    UINT32  Data;

    struct {
        UINT32  ImageLen:             16;     /* [15:0] length of boot Image */
        UINT32  Clock_Div :            6;      /* [21:16] Dummy Cycle length */
        UINT32  DummyLen:            5;      /* [26:22] Dummy Cycle length */
        UINT32  AddrLen:           3;      /* [29:27] Address length */
        UINT32  CmdLen:          2;      /* [31:30] Command length */
    } Bits;
} AMBA_NOR_IMGLEN_REG_u;

/*-----------------------------------------------------------------------------------------------*\
 * SPINOR_CTRL_REG
\*-----------------------------------------------------------------------------------------------*/
typedef union _AMBA_NOR_DTR_REG_u_ {
    UINT32  Data;

    struct {
        UINT32  DataReadn:          1;      /* [0] Data Part Read Mode */
        UINT32  DataWriteen:            1;      /* [1] Data Part Write Mode */
        UINT32  Reserved:            7;      /* [8:2] */
        UINT32  RxLane:         1;      /* [9] RxLANE count */
        UINT32  NumDataLane:            2;      /* [11:10] DataLANE count */
        UINT32  NumAddrLane:            2;      /* [13:12] AddrLANE count */
        UINT32  NumCmdLane:         2;      /* [15:14] CmdLANE count */
        UINT32  Reserved1:          8 ;     /* [23:16] */
        UINT32  Reserved2:           1;      /* [24] LSB & MSB First  (ignored)*/
        UINT32  Reserved3:          3;      /* [27:25] */
        UINT32  DataDTR:            1;      /* [28] Data Double Transfer Rate */
        UINT32  DummyDTR:           1;      /* [29] Dummy Double Transfer Rate */
        UINT32  AddressDTR:         1;      /* [30] Address DTR MODE */
        UINT32  CmdDTR:         1;      /* [31] Cmd DTR MODE */
    } Bits;
}AMBA_NOR_DTR_REG_u;

/*-----------------------------------------------------------------------------------------------*\
 * SPINOR_CFG_REG
\*-----------------------------------------------------------------------------------------------*/
typedef union _AMBA_NOR_FLOWCTR_REG_u_ {
    UINT32  Data;

    struct {
        UINT32  RxSampleDelay:          5;      /* [4:0] Adjust RX sampling Data Phase */
        UINT32  Reserved:          5;      /* [9:5] */
        UINT32  Reserved1:         8;      /* [17:10] ignored */
        UINT32  ChipSelect:            8;      /* [25:18] CEN for multiple device*/
        UINT32  HoldSwitchphase:             1;      /* [26] Clock will remain in standby mode*/
        UINT32  Reserved2:             1;      /* [27]  ignored*/
        UINT32  Hold:           3;      /* [30:28] For Flow control purpose */
        UINT32  FlowControl:            1;      /* [31] Flow control enable*/
    } Bits;
} AMBA_NOR_FLOWCTR_REG_u;

/*-----------------------------------------------------------------------------------------------*\
 *  SPINOR_CMD_REG
\*-----------------------------------------------------------------------------------------------*/
typedef union _AMBA_NOR_CMD_REG_u_ {
    UINT32  Data;

    struct {
        UINT32  Cmd0:           8;      /* [7:0] Command 0 for SPI Device*/
        UINT32  Cmd1:           8;      /* [15:8] Command 1 for SPI Device */
        UINT32  Cmd2:           8;      /* [23:16] Command 2 for SPI Device*/
        UINT32  Reserved:           8;      /* [31:24] */
    } Bits;
} AMBA_NOR_CMD_REG_u;

/*-----------------------------------------------------------------------------------------------*\
 *  SPINOR_ADDRESS_HI_Register
\*-----------------------------------------------------------------------------------------------*/
typedef union _AMBA_NOR_ADDRHI_REG_u_ {
    UINT32  Data;

    struct {
        UINT32  Addr4:           8;      /* [7:0] Address 4 Field for SPI Device*/
        UINT32  Addr5:           8;      /* [15:8] Address 5 Field for SPI Device */
        UINT32  Addr6:           8;      /* [23:16] Address 6 Field for SPI Device */
        UINT32  Reserved:           8;      /* [31:24] */
    } Bits;
} AMBA_NOR_ADDRHI_REG_u;

/*-----------------------------------------------------------------------------------------------*\
 *  SPINOR_ADDRESS_LOW_Register
\*-----------------------------------------------------------------------------------------------*/
typedef union _AMBA_NOR_ADDRLOW_REG_u_ {
    UINT32  Data;

    struct {
        UINT32  Addr0:           8;      /* [7:0] Address 0 Field for SPI Device */
        UINT32  Addr1:           8;      /* [15:8] Address 1 Field for SPI Device */
        UINT32  Addr2:           8;      /* [23:16] Address 2 Field for SPI Device */
        UINT32  Addr3:           8;      /* [31:24] Address 3 Field for SPI Device */
    } Bits;
} AMBA_NOR_ADDRLOW_REG_u;

/*-----------------------------------------------------------------------------------------------*\
 * The boot image header is a128-byte block and only first 24 bytes are used.
 * Each 4 bytes is corresponding to the layout (register definition) of
 * registers 0x0~0x14, respectively except for the data length field, clock
 * divder field, and those controlled by boot options. Please refer to document
 * for detailed information.
\*-----------------------------------------------------------------------------------------------*/
typedef struct _AMBA_NOR_FW_HEADER_s_ {
    volatile AMBA_NOR_IMGLEN_REG_u            Img;     /* 0x0: NOR flash the format of command  */
    volatile AMBA_NOR_DTR_REG_u                 DTR;       /* 0x4: NOR flash DTR mode control register  */
    volatile AMBA_NOR_FLOWCTR_REG_u            Flow;        /* 0x8: NOR flash flowcontrol */
    volatile AMBA_NOR_CMD_REG_u        Cmd;        /* 0xc: NOR flash command for SPI device*/
    volatile AMBA_NOR_ADDRHI_REG_u        AddrHi;        /* 0x10: NOR flash Address 6~4 Field for SPI Command */
    volatile AMBA_NOR_ADDRLOW_REG_u        AddrLow;        /* 0x14: NOR flash Address 3~0 Field for SPI Command */
    UINT32 Reserved[26]; /* 0x18 - 0x80 */
} AMBA_NOR_FW_HEADER_s;


/*-----------------------------------------------------------------------------------------------*\
 * Defined in AmbaFWPROG.c
 *-----------------------------------------------------------------------------------------------*/
int AmbaNonOS_NorGetPartTable(AMBA_NOR_PART_TABLE_s *pTable);
int AmbaNonOS_NorSetPartTable(AMBA_NOR_PART_TABLE_s *pTable);
int AmbaFWPROG_ErasePart(int PartID);
int AmbaFWPROG_NorPart(int PartID, UINT8 *pImage, UINT32 Len);
const char *GetPartitionName(UINT32 PartID);

#endif /* _AMBA_RTSL_NAND_H_ */
