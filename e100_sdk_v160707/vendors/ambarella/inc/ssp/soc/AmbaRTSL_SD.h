/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaRTSL_SD.h
 *
 *  @Copyright      :: Copyright (C) 2012 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Definitions of Run Time Support Library for SD flash
\*-------------------------------------------------------------------------------------------------------------------*/

#ifndef _AMBA_RTSL_SD_H_
#define _AMBA_RTSL_SD_H_

#include "AmbaSD_Def.h"

/*----------------------------------------------------------------------------*\
 * @defgroup SD SD/MMC Protocol Stack
 *
 * The SD/MMC Protocol Stack module provides an implementation of the SD
 * Memoery, SD IO, MMC protocols. It translates the commands into host
 * controller specific commands and work with upper-level drivers such as
 * block devices (for file-system) and NICs, etc.
\*----------------------------------------------------------------------------*/

/* ------------------------------------------------------------------------ */
/* Protocol definitions                                                     */
/* ------------------------------------------------------------------------ */

/*      Command            name                  type argument           resp*/
/* class 1 */
#define SD_CMD0   0  /* GO_IDLE_STATE         bc   [31:0]             -   */
#define SD_CMD1   1  /* SEND_OP_COND          bcr  [31:0] OCR         R3  */
#define SD_CMD2   2  /* ALL_SEND_CID          bcr                     R2  */
#define SD_CMD3   3  /* SET_RELATIVE_ADDR     ac   [31:16] RCA        R1  */
#define SD_CMD4   4  /* SET_DSR               bc   [31:16] RCA            */
#define SD_CMD5   5
#define SD_CMD6   6  /* SWITCH                ac                      R1b */
#define SD_CMD7   7  /* SELECT_CARD           ac   [31:16] RCA        R1  */
#define SD_CMD8   8  /* SEND_EXT_CSD          adtc                    R1  */
#define SD_CMD9   9  /* SEND_CSD              ac   [31:16] RCA        R2  */
#define SD_CMD10 10  /* SEND_CID              ac   [31:16] RCA        R2  */
#define SD_CMD11 11  /* READ_DAT_UNTIL_STOP   adtc [31:0] dadr        R1  */
#define SD_CMD12 12  /* STOP_TRANSMISSION     ac                      R1b */
#define SD_CMD13 13  /* SEND_STATUS          ac   [31:16] RCA        R1  */
#define SD_CMD14 14  /* BUSTEST_R             adtc                    R1  */
#define SD_CMD15 15  /* GO_INACTIVE_STATE     ac   [31:16] RCA            */
/* class 2 */
#define SD_CMD16 16  /* SET_BLOCKLEN          ac   [31:0] block len   R1  */
#define SD_CMD17 17  /* READ_SINGLE_BLOCK     adtc [31:0] data addr   R1  */
#define SD_CMD18 18  /* READ_MULTIPLE_BLOCK   adtc [31:0] data addr   R1  */
#define SD_CMD19 19  /* BUSTEST_W             adtc                    R1  */
/* class 3 */
#define SD_CMD20 20  /* WRITE_DAT_UNTIL_STOP  adtc [31:0] data addr   R1  */
/* class 4 */
#define SD_CMD23 23  /* SET_BLOCK_COUNT       adtc [31:0] data addr   R1  */
#define SD_CMD24 24  /* WRITE_BLOCK           adtc [31:0] data addr   R1  */
#define SD_CMD25 25  /* WRITE_MULTIPLE_BLOCK  adtc                    R1  */
#define SD_CMD26 26  /* PROGRAM_CID           adtc                    R1  */
#define SD_CMD27 27  /* PROGRAM_CSD           adtc                    R1  */
/* class 6 */
#define SD_CMD28 28  /* SET_WRITE_PROT        ac   [31:0] data addr   R1b */
#define SD_CMD29 29  /* CLR_WRITE_PROT        ac   [31:0] data addr   R1b */
#define SD_CMD30 30  /* SEND_WRITE_PROT       adtc [31:0] wpdata addr R1  */
/* class 5 */
#define SD_CMD32 32  /* ERASE_WR_BLK_START    ac   [31:0] data addr   R1  */
#define SD_CMD33 33  /* ERASE_WR_BLK_END      ac   [31:0] data addr   R1  */
#define SD_CMD35 35  /* ERASE_GROUP_START     ac   [31:0] data addr   R1  */
#define SD_CMD36 36  /* ERASE_GROUP_END       ac   [31:0] data addr   R1  */
/* #define SD_CMD37 37 */
#define SD_CMD38 38  /* ERASE                 ac                      R1b */
/* class 9 */
#define SD_CMD39 39  /* FAST_IO               ac   <Complex>          R4  */
#define SD_CMD40 40  /* GO_IRQ_STATE          bcr                     R5  */
/* class 7 */
#define SD_CMD42 42  /* LOCK_UNLOCK           adtc                    R1b */
/* class 8 */
#define SD_CMD55 55  /* APP_CMD               ac   [31:16] RCA        R1  */
#define SD_CMD56 56  /* GEN_CMD               adtc [0] RD/WR          R1b */
/* --- */
#define SD_CMD52 52  /* IO_RW_DIRECT */
#define SD_CMD53 53  /* IO_RW_EXTENDED */

/**
 * SD Memory Card Application Specific Commands.
 */
/*      Command            name                  type argument           resp*/
#define SD_ACMD6   6 /* SET_BUS_WIDTH         ac   [1:0] bus width    R1  */
#define SD_ACMD13 13 /* SD_STATUS             adtc                    R1  */
#define SD_ACMD22 22 /* SEND_NUM_WR_BLOCKS    adtc                    R1  */
#define SD_ACMD23 23 /* SET_WR_BLK_ERASE_COUNT ac                     R1  */
#define SD_ACMD41 41 /* SD_SEND_OP_COND       bcr                     R3  */
#define SD_ACMD42 42 /* SET_CLR_CARD_DETECT   ac                      R1  */
#define SD_ACMD51 51 /* SEND_SCR              adtc                    R1  */

/*
  SD/MMC status in R1
  Type
    e : error bit
    s : status bit
    r : detected and set for the actual command response
    x : detected and set during command execution. the host must poll
            the card by sending status command in order to read these bits.
  Clear condition
    a : according to the card state
    b : always related to the previous command. Reception of
            a valid command will clear it (with a delay of one command)
    c : clear by read
 */

#define R1_OUT_OF_RANGE         0x80000000  /* er, c */
#define R1_ADDRESS_ERROR        0x40000000  /* erx, c */
#define R1_BLOCK_LEN_ERROR      0x20000000  /* er, c */
#define R1_ERASE_SEQ_ERROR      0x10000000  /* er, c */
#define R1_ERASE_PARAM          0x08000000  /* ex, c */
#define R1_WP_VIOLATION         0x04000000  /* erx, c */
#define R1_CARD_IS_LOCKED       0x02000000  /* sx, a */
#define R1_LOCK_UNLOCK_FAILED   0x01000000  /* erx, c */
#define R1_COM_CRC_ERROR        0x00800000  /* er, b */
#define R1_ILLEGAL_COMMAND      0x00400000  /* er, b */
#define R1_CARD_ECC_FAILED      0x00200000  /* ex, c */
#define R1_CC_ERROR             0x00100000  /* erx, c */
#define R1_ERROR                0x00080000  /* erx, c */
#define R1_UNDERRUN             0x00040000  /* ex, c */
#define R1_OVERRUN              0x00020000  /* ex, c */
#define R1_CID_CSD_OVERWRITE    0x00010000  /* erx, c, CID/CSD overwrite */
#define R1_WP_ERASE_SKIP        0x00008000  /* sx, c */
#define R1_CARD_ECC_DISABLED    0x00004000  /* sx, a */
#define R1_ERASE_RESET          0x00002000  /* sr, c */
#define R1_STATUS(x)            (x & 0xffffe000)
#define R1_CURRENT_STATE(x)     ((x & 0x00001e00) >> 9) /* sx, b (4 bits) */
#define R1_READY_FOR_DATA       0x00000100  /* sx, a */
#define R1_APP_CMD              0x00000080  /* sr, c */

/* SD/MMC error code */
#define SD_ERR_R1_OUT_OF_RANGE          -200
#define SD_ERR_R1_ADDRESS_ERROR         -201
#define SD_ERR_R1_BLOCK_LEN_ERROR       -202
#define SD_ERR_R1_ERASE_SEQ_ERROR       -203
#define SD_ERR_R1_ERASE_PARAM           -204
#define SD_ERR_R1_WP_VIOLATION          -205
#define SD_ERR_R1_CARD_IS_LOCKED        -206
#define SD_ERR_R1_LOCK_UNLOCK_FAILED    -207
#define SD_ERR_R1_COM_CRC_ERROR         -208
#define SD_ERR_R1_ILLEGAL_COMMAND       -209
#define SD_ERR_R1_CARD_ECC_FAILED       -210
#define SD_ERR_R1_CC_ERROR              -211
#define SD_ERR_R1_ERROR                 -212
#define SD_ERR_R1_UNDERRUN              -213
#define SD_ERR_R1_OVERRUN               -214
#define SD_ERR_R1_CID_CSD_OVERWRITE     -215
#define SD_ERR_R1_WP_ERASE_SKIP         -216
#define SD_ERR_R1_CARD_ECC_DISABLED     -217
#define SD_ERR_R1_ERASE_RESET           -218

/**
 * Voltage constants.
 */
#define SD_VDD_145_150  0x00000001  /**< VDD voltage 1.45 - 1.50 */
#define SD_VDD_150_155  0x00000002  /**< VDD voltage 1.50 - 1.55 */
#define SD_VDD_155_160  0x00000004  /**< VDD voltage 1.55 - 1.60 */
#define SD_VDD_160_165  0x00000008  /**< VDD voltage 1.60 - 1.65 */
#define SD_VDD_165_170  0x00000010  /**< VDD voltage 1.65 - 1.70 */
#define SD_VDD_17_18        0x00000020  /**< VDD voltage 1.7 - 1.8 */
#define SD_VDD_18_19        0x00000040  /**< VDD voltage 1.8 - 1.9 */
#define SD_VDD_19_20        0x00000080  /**< VDD voltage 1.9 - 2.0 */
#define SD_VDD_20_21        0x00000100  /**< VDD voltage 2.0 ~ 2.1 */
#define SD_VDD_21_22        0x00000200  /**< VDD voltage 2.1 ~ 2.2 */
#define SD_VDD_22_23        0x00000400  /**< VDD voltage 2.2 ~ 2.3 */
#define SD_VDD_23_24        0x00000800  /**< VDD voltage 2.3 ~ 2.4 */
#define SD_VDD_24_25        0x00001000  /**< VDD voltage 2.4 ~ 2.5 */
#define SD_VDD_25_26        0x00002000  /**< VDD voltage 2.5 ~ 2.6 */
#define SD_VDD_26_27        0x00004000  /**< VDD voltage 2.6 ~ 2.7 */
#define SD_VDD_27_28        0x00008000  /**< VDD voltage 2.7 ~ 2.8 */
#define SD_VDD_28_29        0x00010000  /**< VDD voltage 2.8 ~ 2.9 */
#define SD_VDD_29_30        0x00020000  /**< VDD voltage 2.9 ~ 3.0 */
#define SD_VDD_30_31        0x00040000  /**< VDD voltage 3.0 ~ 3.1 */
#define SD_VDD_31_32        0x00080000  /**< VDD voltage 3.1 ~ 3.2 */
#define SD_VDD_32_33        0x00100000  /**< VDD voltage 3.2 ~ 3.3 */
#define SD_VDD_33_34        0x00200000  /**< VDD voltage 3.3 ~ 3.4 */
#define SD_VDD_34_35        0x00400000  /**< VDD voltage 3.4 ~ 3.5 */
#define SD_VDD_35_36        0x00800000  /**< VDD voltage 3.5 ~ 3.6 */
#define SD_CARD_BUSY        0x80000000  /**< Card Power up status bit */

/**
 * Additional def. for SDIO CMD5 op cond (R4) result.
 */
#define SDIO_OP_COND_C      0x80000000
#define SDIO_OP_COND_NIO(x) (((x) & 0x70000000) >> 28)
#define SDIO_OP_COND_MP     0x08000000

/**
 * CSD field definitions
 */
#define CSD_STRUCT_VER_1_0  0  /**< Valid for system specification 1.0 - 1.2 */
#define CSD_STRUCT_VER_1_1  1  /**< Valid for system specification 1.4 - 2.2 */
#define CSD_STRUCT_VER_1_2  2  /**< Valid for system specification 3.1       */

#define CSD_SPEC_VER_0      0  /**< Implements system specification 1.0 - 1.2 */
#define CSD_SPEC_VER_1      1  /**< Implements system specification 1.4 */
#define CSD_SPEC_VER_2      2  /**< Implements system specification 2.0 - 2.2 */
#define CSD_SPEC_VER_3      3  /**< Implements system specification 3.1 */

/* DSR1 filed definitions */
#define DSR1_SWITCH_ON_500_200  0x1
#define DSR1_SWITCH_ON_100_50   0x2
#define DSR1_SWITCH_ON_20_10    0x4
#define DSR1_SWITCH_ON_5_2      0x8

/* DSR2 filed definitions */
#define DSR2_PEAK_RISE_1_2_500      0x1
#define DSR2_PEAK_RISE_5_10_100     0x2
#define DSR2_PEAK_RISE_20_50_20     0x4
#define DSR2_PEAK_RISE_100_200_5    0x8

/* ======================================================================== */
/* ------------------------------------------------------------------------ */
/* CARD definitions                                                         */
/* ------------------------------------------------------------------------ */
/* ======================================================================== */


#define SD_UPDATE_STATUS_ERROR      (-300)
#define SD_ALLOCATE_BUFFER_ERROR    (-301)

/* ======================================================================== */
/* ------------------------------------------------------------------------ */
/* SDIO definitions                                                         */
/* ------------------------------------------------------------------------ */
/* ======================================================================== */

#define MAX_SDIO_TUPLE_BODY_LEN     255

/************************************************/
/* CCCR (Card Common Control Registers).    */
/* This is for FN0.             */
/************************************************/
#define CCCR_SDIO_REV_REG           0x00
#   define CCCR_FORMAT_VERSION(reg)    (((reg) & 0x0f))
#   define SDIO_SPEC_REVISION(reg)     (((reg) & 0xf0 >> 4))
#define SD_SPEC_REV_REG             0x01
#   define SD_PHYS_SPEC_VERSION(reg)  (((reg) & 0x0f))
#define IO_ENABLE_REG               0x02
#   define IOE(x)                      (0x1U << (x))
#   define IOE_1                       IOE(1)
#   define IOE_2                       IOE(2)
#   define IOE_3                       IOE(3)
#   define IOE_4                       IOE(4)
#   define IOE_5                       IOE(5)
#   define IOE_6                       IOE(6)
#   define IOE_7                       IOE(7)
#define IO_READY_REG                0x03
#   define IOR(x)                      (0x1U << (x))
#   define IOR_1                       IOR(1)
#   define IOR_2                       IOR(2)
#   define IOR_3                       IOR(3)
#   define IOR_4                       IOR(4)
#   define IOR_5                       IOR(5)
#   define IOR_6                       IOR(6)
#   define IOR_7                       IOR(7)
#define INT_ENABLE_REG              0x04
#   define IENM                        0x1
#   define IEN(x)                      (0x1U << (x))
#   define IEN_1                       IEN(1)
#   define IEN_2                       IEN(2)
#   define IEN_3                       IEN(3)
#   define IEN_4                       IEN(4)
#   define IEN_5                       IEN(5)
#   define IEN_6                       IEN(6)
#   define IEN_7                       IEN(7)
#define INT_PENDING_REG             0x05
#   define INT(x)                      (0x1U << (x))
#   define INT_1                       INT(1)
#   define INT_2                       INT(2)
#   define INT_3                       INT(3)
#   define INT_4                       INT(4)
#   define INT_5                       INT(5)
#   define INT_6                       INT(6)
#   define INT_7                       INT(7)
#define IO_ABORT_REG                0x06
#   define AS(x)                       ((x) & 0x7)
#   define AS_1                        AS(1)
#   define AS_2                        AS(2)
#   define AS_3                        AS(3)
#   define AS_4                        AS(4)
#   define AS_5                        AS(5)
#   define AS_6                        AS(6)
#   define AS_7                        AS(7)
#   define RES                         (0x1U << 3)
#define BUS_INTERFACE_CONTROL_REG   0x07
#   define BUS_WIDTH(reg)              ((reg) & 0x3)
#   define CD_DISABLE                  (0x1U << 7)
#define CARD_CAPABILITY_REG         0x08
#   define SDC                         (0x1U << 0)
#   define SMB                         (0x1U << 1)
#   define SRW                         (0x1U << 2)
#   define SBS                         (0x1U << 3)
#   define S4MI                        (0x1U << 4)
#   define E4MI                        (0x1U << 5)
#   define LSC                         (0x1U << 6)
#   define S4BLS                       (0x1U << 7)
#define COMMON_CIS_POINTER_0_REG    0x09
#define COMMON_CIS_POINTER_1_REG    0x0a
#define COMMON_CIS_POINTER_2_REG    0x0b
#define BUS_SUSPEND_REG             0x0c
#   define BUS_STATUS                  (0x1U << 0)
#   define BUS_REL_REQ_STATUS          (0x1U << 1)
#define FUNCTION_SELECT_REG         0x0d
#define    FS(x)                       ((x) & 0xf)
#   define FS_1                        FS(1)
#   define FS_2                        FS(2)
#   define FS_3                        FS(3)
#   define FS_4                        FS(4)
#   define FS_5                        FS(5)
#   define FS_6                        FS(6)
#   define FS_7                        FS(7)
#   define FS_MEM                      FS(8)
#   define DF                          (0x1U << 7)
#define EXEC_FLAGS_REG              0x0e
#   define EXM                         0x0
#   define EX(x)                       (0x1U << (x))
#   define EX_1                        EX(1)
#   define EX_2                        EX(2)
#   define EX_3                        EX(3)
#   define EX_4                        EX(4)
#   define EX_5                        EX(5)
#   define EX_6                        EX(6)
#   define EX_7                        EX(7)
#define READY_FLAGS_REG             0x0f
#   define RFM                         0x0
#   define RF(x)                       (0x1U << (x))
#   define RF_1                        RF(1)
#   define RF_2                        RF(2)
#   define RF_3                        RF(3)
#   define RF_4                        RF(4)
#   define RF_5                        RF(5)
#   define RF_6                        RF(6)
#   define RF_7                        IEN(7)
#if 0   /* Defined below in FBR */
#define FN0_BLOCK_SIZE_0_REG        0x10
#define FN0_BLOCK_SIZE_1_REG        0x11
#endif
#define POWER_CONTROL_REG           0x12
#   define SMPC             (0x1U << 0)
#   define EMPC             (0x1U << 1)

/***********************************************/
/* FBR (Function Basic Registers).         */
/* This is for FN1 ~ FN7.              */
/***********************************************/
#define FN_CSA_REG(x)           (0x100 * (x) + 0x00)
#define FN_EFIC_REG(x)          (0x100 * (x) + 0x01)
#define FN_POWER_REG(x)         (0x100 * (x) + 0x02)
#define FN_CIS_POINTER_0_REG(x)   (0x100 * (x) + 0x09)
#define FN_CIS_POINTER_1_REG(x)   (0x100 * (x) + 0x0a)
#define FN_CIS_POINTER_2_REG(x)   (0x100 * (x) + 0x0b)
#define FN_CSA_POINTER_0_REG(x)   (0x100 * (x) + 0x0c)
#define FN_CSA_POINTER_1_REG(x)   (0x100 * (x) + 0x0d)
#define FN_CSA_POINTER_2_REG(x)   (0x100 * (x) + 0x0e)
#define FN_CSA_DAT_REG(x)         (0x100 * (x) + 0x0f)
#define FN_BLOCK_SIZE_0_REG(x)    (0x100 * (x) + 0x10)
#define FN_BLOCK_SIZE_1_REG(x)    (0x100 * (x) + 0x11)

/*      Function 0  -- duplicate, see the CCRC section */
#define FN0_CIS_POINTER_0_REG   FN_CIS_POINTER_0_REG(0)
#define FN0_CIS_POINTER_1_REG   FN_CIS_POINTER_1_REG(0)
#define FN0_CIS_POINTER_2_REG   FN_CIS_POINTER_2_REG(0)
#define FN0_BLOCK_SIZE_0_REG    FN_BLOCK_SIZE_0_REG(0)
#define FN0_BLOCK_SIZE_1_REG    FN_BLOCK_SIZE_1_REG(0)
/*      Function 1       */
#define FN1_CSA_REG             FN_CSA_REG(1)
#define FN1_EFIC_REG            FN_EFIC_REG(1)
#define FN1_POWER_REG           FN_POWER_REG(1)
#define FN1_CIS_POINTER_0_REG   FN_CIS_POINTER_0_REG(1)
#define FN1_CIS_POINTER_1_REG   FN_CIS_POINTER_1_REG(1)
#define FN1_CIS_POINTER_2_REG   FN_CIS_POINTER_2_REG(1)
#define FN1_CSA_POINTER_0_REG   FN_CSA_POINTER_0_REG(1)
#define FN1_CSA_POINTER_1_REG   FN_CSA_POINTER_1_REG(1)
#define FN1_CSA_POINTER_2_REG   FN_CSA_POINTER_2_REG(1)
#define FN1_CSA_DAT_REG         FN_CSA_DAT_REG(1)
#define FN1_BLOCK_SIZE_0_REG    FN_BLOCK_SIZE_0_REG(1)
#define FN1_BLOCK_SIZE_1_REG    FN_BLOCK_SIZE_1_REG(1)
/*      Function 2       */
#define FN2_CSA_REG             FN_CSA_REG(2)
#define FN2_EFIC_REG            FN_EFIC_REG(2)
#define FN2_POWER_REG           FN_POWER_REG(2)
#define FN2_CIS_POINTER_0_REG   FN_CIS_POINTER_0_REG(2)
#define FN2_CIS_POINTER_1_REG   FN_CIS_POINTER_1_REG(2)
#define FN2_CIS_POINTER_2_REG   FN_CIS_POINTER_2_REG(2)
#define FN2_CSA_POINTER_0_REG   FN_CSA_POINTER_0_REG(2)
#define FN2_CSA_POINTER_1_REG   FN_CSA_POINTER_1_REG(2)
#define FN2_CSA_POINTER_2_REG   FN_CSA_POINTER_2_REG(2)
#define FN2_CSA_DAT_REG         FN_CSA_DAT_REG(2)
#define FN2_BLOCK_SIZE_0_REG    FN_BLOCK_SIZE_0_REG(2)
#define FN2_BLOCK_SIZE_1_REG    FN_BLOCK_SIZE_1_REG(2)
/*      Function 3       */
#define FN3_CSA_REG             FN_CSA_REG(3)
#define FN3_EFIC_REG            FN_EFIC_REG(3)
#define FN3_POWER_REG           FN_POWER_REG(3)
#define FN3_CIS_POINTER_0_REG   FN_CIS_POINTER_0_REG(3)(3)
#define FN3_CIS_POINTER_1_REG   FN_CIS_POINTER_1_REG(3)
#define FN3_CIS_POINTER_2_REG   FN_CIS_POINTER_2_REG(3)
#define FN3_CSA_POINTER_0_REG   FN_CSA_POINTER_0_REG(3)
#define FN3_CSA_POINTER_1_REG   FN_CSA_POINTER_1_REG(3)
#define FN3_CSA_POINTER_2_REG   FN_CSA_POINTER_2_REG(3)
#define FN3_CSA_DAT_REG         FN_CSA_DAT_REG(3)
#define FN3_BLOCK_SIZE_0_REG    FN_BLOCK_SIZE_0_REG(3)
#define FN3_BLOCK_SIZE_1_REG    FN_BLOCK_SIZE_1_REG(3)
/*      Function 4       */
#define FN4_CSA_REG             FN_CSA_REG(4)
#define FN4_EFIC_REG            FN_EFIC_REG(4)
#define FN4_POWER_REG           FN_POWER_REG(4)
#define FN4_CIS_POINTER_0_REG   FN_CIS_POINTER_0_REG(4)
#define FN4_CIS_POINTER_1_REG   FN_CIS_POINTER_1_REG(4)
#define FN4_CIS_POINTER_2_REG   FN_CIS_POINTER_2_REG(4)
#define FN4_CSA_POINTER_0_REG   FN_CSA_POINTER_0_REG(4)
#define FN4_CSA_POINTER_1_REG   FN_CSA_POINTER_1_REG(4)
#define FN4_CSA_POINTER_2_REG   FN_CSA_POINTER_2_REG(4)
#define FN4_CSA_DAT_REG         FN_CSA_DAT_REG(4)
#define FN4_BLOCK_SIZE_0_REG    FN_BLOCK_SIZE_0_REG(4)
#define FN4_BLOCK_SIZE_1_REG    FN_BLOCK_SIZE_1_REG(4)
/*      Function 5       */
#define FN5_CSA_REG             FN_CSA_REG(5)
#define FN5_EFIC_REG            FN_EFIC_REG(5)
#define FN5_POWER_REG           FN_POWER_REG(5)
#define FN5_CIS_POINTER_0_REG   FN_CIS_POINTER_0_REG(5)
#define FN5_CIS_POINTER_1_REG   FN_CIS_POINTER_1_REG(5)
#define FN5_CIS_POINTER_2_REG   FN_CIS_POINTER_2_REG(5)
#define FN5_CSA_POINTER_0_REG   FN_CSA_POINTER_0_REG(5)
#define FN5_CSA_POINTER_1_REG   FN_CSA_POINTER_1_REG(5)
#define FN5_CSA_POINTER_2_REG   FN_CSA_POINTER_2_REG(5)
#define FN5_CSA_DAT_REG         FN_CSA_DAT_REG(5)
#define FN5_BLOCK_SIZE_0_REG    FN_BLOCK_SIZE_0_REG(5)
#define FN5_BLOCK_SIZE_1_REG    FN_BLOCK_SIZE_1_REG(5)
/*      Function 6       */
#define FN6_CSA_REG             FN_CSA_REG(6)
#define FN6_EFIC_REG            FN_EFIC_REG(6)
#define FN6_POWER_REG           FN_POWER_REG(6)
#define FN6_CIS_POINTER_0_REG   FN_CIS_POINTER_0_REG(6)
#define FN6_CIS_POINTER_1_REG   FN_CIS_POINTER_1_REG(6)
#define FN6_CIS_POINTER_2_REG   FN_CIS_POINTER_2_REG(6)
#define FN6_CSA_POINTER_0_REG   FN_CSA_POINTER_0_REG(6)
#define FN6_CSA_POINTER_1_REG   FN_CSA_POINTER_1_REG(6)
#define FN6_CSA_POINTER_2_REG   FN_CSA_POINTER_2_REG(6)
#define FN6_CSA_DAT_REG         FN_CSA_DAT_REG(6)
#define FN6_BLOCK_SIZE_0_REG    FN_BLOCK_SIZE_0_REG(6)
#define FN6_BLOCK_SIZE_1_REG    FN_BLOCK_SIZE_1_REG(6)
/*      Function 7       */
#define FN7_CSA_REG             FN_CSA_REG(7)
#define FN7_EFIC_REG            FN_EFIC_REG(7)
#define FN7_POWER_REG           FN_POWER_REG(7)
#define FN7_CIS_POINTER_0_REG   FN_CIS_POINTER_0_REG(7)
#define FN7_CIS_POINTER_1_REG   FN_CIS_POINTER_1_REG(7)
#define FN7_CIS_POINTER_2_REG   FN_CIS_POINTER_2_REG(7)
#define FN7_CSA_POINTER_0_REG   FN_CSA_POINTER_0_REG(7)
#define FN7_CSA_POINTER_1_REG   FN_CSA_POINTER_1_REG(7)
#define FN7_CSA_POINTER_2_REG   FN_CSA_POINTER_2_REG(7)
#define FN7_CSA_DAT_REG         FN_CSA_DAT_REG(7)
#define FN7_BLOCK_SIZE_0_REG    FN_BLOCK_SIZE_0_REG(7)
#define FN7_BLOCK_SIZE_1_REG    FN_BLOCK_SIZE_1_REG(7)

/***********************************************/
/* FBR bits definitions. This is for FN1 ~ FN7 */
/***********************************************/
/* FN_CSA_REG */
#define FN_IODEV_INTERFACE_CODE(reg)    ((reg) & 0xf)
#   define NO_SDIO_FUNC         0x0
#   define SDIO_UART            0x1
#   define SDIO_TYPEA_BLUETOOTH 0x2
#   define SDIO_TYPEB_BLUETOOTH 0x3
#   define SDIO_GPS             0x4
#   define SDIO_CAMERA          0x5
#   define SDIO_PHS             0x6
#   define SDIO_WLAN            0x7
/* Reference interface code in FN_EFIC_REG */
#   define SDIO_OTHERS          0xf
#define FN_SUPPORTS_CSA         (0x1U << 6)
#define FN_CSA_ENABLE           (0x1U << 7)


/*******************************************/
/* Misc definition for SDIO        */
/*******************************************/

/* Misc. helper definitions */
#define FN(x)                     (x)
#define FN0                       FN(0)
#define FN1                       FN(1)
#define FN2                       FN(2)
#define FN3                       FN(3)
#define FN4                       FN(4)
#define FN5                       FN(5)
#define FN6                       FN(6)
#define FN7                       FN(7)

/* IO_RW_DIRECT response(R5) flags */
#define R5_COM_CRC_ERROR    0x80
#define R5_ILLEGAL_COMMAND  0x40

/* bit 5:4 IO_CURRENT_STATE */
#define R5_CURRENT_STATE(x)     ((x & 0x30) >> 4)
#define R5_IO_CUR_STA_DIS   0x0
#define R5_IO_CUR_STA_CMD   0x1
#define R5_IO_CUR_STA_TRN   0x2
#define R5_IO_CUR_STA_RFU   0x3

#define R5_ERROR            0x08
#define R5_FUNC_NUM_ERROR   0x02
#define R5_OUT_OF_RANGE     0x01

/* Modified R6 response status */
#define MR6_COM_CRC_ERROR   0x8000
#define MR6_ILLEGAL_COMMAND 0x4000
#define MR6_ERROR           0x2000

/* SDIO error code */
#define ERR_SDIO_R5_COM_CRC_ERROR   -230
#define ERR_SDIO_R5_ILLEGAL_COMMAND -231
#define ERR_SDIO_R5_ERROR           -232
#define ERR_SDIO_R5_FUNC_NUM_ERROR  -233
#define ERR_SDIO_R5_OUT_OF_RANGE    -234
#define ERR_SDIO_MR6_COM_CRC_ERROR  -240
#define ERR_SDIO_MR6_ILLEGAL_COMMAND    -241
#define ERR_SDIO_MR6_ERROR          -242

/* SDIO CISTPL_FUNCE tuple for function 0 */
typedef struct _AMBA_SDIO_FN0_FUNCE_ {
    UINT8   Type;
    UINT16  BlkSize;
    UINT8   MaxTranSpeed;
}  AMBA_SDIO_FN0_FUNCE;

/* SDIO CISTPL_FUNCE tuple for function 1 ~ 7 */
typedef struct _AMBA_SDIO_FNx_FUNCE_ {
    UINT8   Type;
    UINT8   FuncInfo;
    UINT8   StdIORev;
    UINT32  CardPsn;
    UINT32  CsaSize;
    UINT8   CsaProperty;
    UINT16  MaxBlkSize;
    UINT32  Ocr;
    UINT8   OPMinPwr;
    UINT8   OPAvgPwr;
    UINT8   OPMaxPwr;
    UINT8   SBMinPwr;
    UINT8   SBAvgPwr;
    UINT8   SBMaxPwr;
    UINT16  MinBW;
    UINT16  OptBWw;
    UINT16  EnableTimeoutVal;
    UINT16  SPAvgPwr33v;
    UINT16  SPMaxPwr33v;
    UINT16  HPAvgPwr33v;
    UINT16  HPMaxPwr33v;
    UINT16  LPAvgPwr33v;
    UINT16  LPMaxPwr33v;
}  AMBA_SDIO_FNx_FUNCE;

typedef void (*SDIOIrqHdlr_f)(AMBA_SD_HOST *, void *);

/* Getter macros */
#define SDHOST_CMD(pHost)   ((pHost)->pMrq->pCmd)
#define SDHOST_MRQ(pHost)   ((pHost)->pMrq)
#define SDHOST_DAT(pHost)   ((pHost)->pMrq->pData)
#define SDHOST_STP(pHost)   ((pHost)->pMrq->pStop)
#define SDHOST_REG(pHost)   (pHost->pAmba_SDReg)

#define CHK_CARD_SLOTS(id)          \
    {                                   \
       K_ASSERT((id) < MAX_SD_HOST_ID);   \
    }

#define GET_SD_HOST_ID(id)  ((id) & 0x1)
#define GET_SD_CARD_ID(id)  (((id) >> 1) & 0x1)

/* ======================================================================== */
/* ------------------------------------------------------------------------ */
/* The protocol stack                                                       */
/* ------------------------------------------------------------------------ */
/* ======================================================================== */

#define SD_ERR_NONE      0  /**< No error */
#define SD_ERR_TIMEOUT  -1  /**< Timeout in SDHC I/O */
#define SD_ERR_BADCRC   -2  /**< CRC error in SDHC I/O */
#define SD_ERR_FAILED   -3  /**< Command failure in SDHC I/O */
#define SD_ERR_INVALID  -4  /**< Invalid cmd/arg in SDHC I/O */
#define SD_ERR_UNUSABLE -5  /**< Unusable card */
#define SD_ERR_ISR_TIMEOUT      -10 /**< Can't wait SDHC ISR and timeout in sd driver. */
#define SD_ERR_DLINE_TIMEOUT    -11 /**< Can't wait SDHC dat line ready and timeout in sd driver. */
#define SD_ERR_CLINE_TIMEOUT    -12 /**< Can't wait SDHC cmd line ready and timeout in sd driver. */
#define SD_ERR_NO_CARD          -13
#define SD_ERR_CHECK_PATTERN    -14 /**< CMD19 check pattern mismatch */

/* New command8 argument bits field definition in SD 2.0 spec */
#define SD_CHECK_PATTERN    0xaa    /* Recommanded value in SD 2.0 */
#define SD_HIGH_VOLTAGE     0x01    /* 2.7~3.6V */
#define SD_LOW_VOLTAGE      0x02    /* Reserved for Low Voltage Range */

/* HCS & CCS bit shift position in OCR */
#define HCS_SHIFT_BIT       30
#define CCS_SHIFT_BIT       30
#define XPC_SHIFT_BIT       28
#define S18R_SHIFT_BIT      24

/*-----------------------------------------------------------------------------------------------*\
 * Defined in AmbaRTSL_SD.h
\*-----------------------------------------------------------------------------------------------*/
/**
 * Switches the mode of operation of the selected card or modifies the
 * EXT_CSD registers (CMD6).
 *
 * @param host - The SDHC.
 * @param card - The card.
 * @param access - Access type.
 * @param index - Index to EXT_CSD.
 * @param value - Value to modify.
 * @param cmd_set - Command set.
 * @returns - 0 successful, < 0 failed.
 */
INT32 AmbaRTSL_SD_switch(AMBA_SD_HOST *pHost,
                         AMBA_SD_CARD *pCard,
                         UINT8 Access, UINT8 Index, UINT8 Value, UINT8 CmdSet);
#define ACCESS_COMMAND_SET  0x0
#define ACCESS_SET_BITS     0x1
#define ACCESS_CLEAR_BITS   0x2
#define ACCESS_WRITE_BYTE   0x3

/**
 * CMD0 for set SD card into PreIdle state with version larger than 4.4
 */
void AmbaRTSL_SDGoPreIdleState(AMBA_SD_HOST *pHost, AMBA_SD_CARD *pCard);

/**
 * CMD6 for SD card with version larger than 1.11.
 */
void AmbaRTSL_SDSwitchFunc(AMBA_SD_HOST *pHost,
                           AMBA_SD_CARD *pCard,
                           UINT8 Mode, AMBA_SD_SW_FUNC_SWITCH *pSwitchReq,
                           AMBA_SD_SW_FUNC_STATUS *pSW);
#define MODE_CHECK_FUNC     0x0
#define MODE_SWITCH_FUNC    0x1
#define CURRENT_FUNC        0xf
#define DEFAULT_FUNC        0x0
#define CMD_SYS_ECOMMERCE   0x1
#define ACCESS_HIGH_SPEED   0x1

/**
 * Select a card - send the RCA to the card specified to the SD/MMC bus (CMD7).
 *
 * @param host - The SDHC.
 * @param card - The card to select on.
 * @returns - 0 suceessful, < 0 failure.
 */
void AmbaRTSL_SDSelectCard(AMBA_SD_HOST *pHost,
                           AMBA_SD_CARD *pCard);

/**
 * The card sends its EXT_CSD register as a block of data (CMD8).
 *
 * @param host - The SDHC.
 * @param card - The card to use.
 * @param ext_csd - Write-back variable of the EXT_CSD register.
 * @returns - 0 suceessful, < 0 failure.
 */
void AmbaRTSL_SDSendExtCsd(AMBA_SD_HOST *pHost,
                           AMBA_SD_CARD *pCard,
                           AMBA_SD_EXT_CSD *pExtCsd);

/**
 * Commad8 is defined to initialize SD memory cards compilant to the SD ver2.00 (NEW CMD8).
 *
 * @param host - The SDHC.
 * @param card - The card to use.
 * @returns - 0 suceessful, < 0 failure.
 */
void AmbaRTSL_SDSendIFCond(AMBA_SD_HOST *pHost, AMBA_SD_CARD *pCard);

/**
 * Addressed card sends its card-specific data (CSD) on the CMD line (CMD9).
 *
 * @param host - The SDHC.
 * @param card - The card to use.
 * @param csd - Write-back variable of the CSD.
 * @returns - 0 suceessful, < 0 failure.
 */
void AmbaRTSL_SDSendCsd(AMBA_SD_HOST *pHost,
                        AMBA_SD_CARD *pCard,
                        AMBA_SD_CSD *pCsd);

/**
 * Addressed card sends its card identification (CID) on the CMD line (CMD10).
 *
 * @param host - The SDHC.
 * @param card - The card to use.
 * @param cid - Write-back variable of the CID.
 * @returns - 0 suceessful, < 0 failure.
 */
void AmbaRTSL_SDSendCid(AMBA_SD_HOST *pHost,
                        AMBA_SD_CARD *pCard,
                        AMBA_SD_CID *pCid);

/**
 * Reads data stream from the card, starting at the given address, until a
 * STOP_TRANSMISSION follows (CMD11).
 *
 * @param host - The SDHC.
 * @param card - The card to use.
 * @param addr - The data address of target memory.
 * @param buf - Pointer to the data buffer on host memory.
 * @returns - 0 suceessful, < 0 failure.
 */
void AmbaRTSL_SDReadDataUntilStop(AMBA_SD_HOST *pHost,
                                  AMBA_SD_CARD *pCard,
                                  UINT32 Addr, UINT8 *pBuf);

/**
 * Do singal voltage switch sequence for spec 3.x(CMD11).
 *
 * @param host - The SDHC.
 * @param card - The card to use.
 * @returns - 0 suceessful, < 0 failure.
 */
void AmbaRTSL_SDVoltageSwitch(AMBA_SD_HOST *pHost,
                              AMBA_SD_CARD *pCard);
#define CLK_SWITCH_DLY_MS       10 /* 5ms in spec define */
#define CMDLINE_SWITCH_DLY_MS   5  /* 1ms in spec define */

/**
 * Forces the card to stop transmission (CMD12).
 *
 * @param host - The SDHC.
 * @param card - The card to use.
 * @returns - 0 successful, < 0 failed
 */
void AmbaRTSL_SDStopTransmission(AMBA_SD_HOST *pHost,
                                 AMBA_SD_CARD *pCard);

/**
 * A host reads the reversed bus testing pattern from a card (CMD14).
 *
 * @param host - The SDHC.
 * @param card - The card to use.
 * @param buf - Pointer to the data buffer on host memory.
 * @param buflen - Length of data buffer.
 * @returns - 0 suceessful, < 0 failure.
 */
void AmbaRTSL_SDBusTestR(AMBA_SD_HOST *pHost,
                         AMBA_SD_CARD *pCard,
                         UINT8 *pBuf, INT32 BufLen);

/**
 * Sets the card to inactive state in order to protect the card stack
 * against communication breakdowns (CMD15).
 *
 * @param host - The SDHC.
 * @param card - The card to use.
 * @returns - 0 suceessful, < 0 failure.
 */
void AmbaRTSL_SDGOInactiveState(AMBA_SD_HOST *pHost,
                                AMBA_SD_CARD *pCard);

/**
 * Sets the block length (in bytes) for all following block commands (read
 * and write). Default block length is specified in the CSD. Supported only
 * if partial block RD/WR operation are allowed in CSD (CMD16).
 *
 * @param host - The SDHC.
 * @param card - The card to use.
 * @param blklen - Block length.
 * @returns - 0 successful, < 0 failed
 */
INT32 AmbaRTSL_SDSetBlockLen(AMBA_SD_HOST *pHost,
                             AMBA_SD_CARD *pCard,
                             INT32 BlkLen);
/**
 * Reads a block of the size selected by the SET_BLOCKLEN command (CMD17).
 *
 * @param host - The SDHC.
 * @param card - The card to use.
 * @param addr - The data address of target memory.
 * @param buf - Pointer to the data buffer on host memory.
 * @returns - 0 successful, < 0 failed
 */
void AmbaRTSL_SDReadSingleBlock(AMBA_SD_HOST *pHost,
                                AMBA_SD_CARD *pCard,
                                UINT32 Addr, UINT8 *pBuf);

/**
 * Continuously transfers data blocks from card to host until interrupted
 * by a STOP_TRANSMISSION command (CMD18).
 *
 * @param host - The SDHC.
 * @param card - The card to use.
 * @param addr - The data address of target memory.
 * @param blocks - The number of blocks.
 * @param buf - Pointer to the data buffer on host memory.
 * @returns - 0 successful, < 0 failed
 */
void AmbaRTSL_SDReadMultipleBlock(AMBA_SD_HOST *pHost,
                                  AMBA_SD_CARD *pCard,
                                  UINT32 Addr, INT32 Blocks, UINT8 *pBuf);

/**
 * A host sends the bus test data pattern to a card (CMD19).
 *
 * @param host - The SDHC.
 * @param card - The card to use.
 * @param buf - Pointer to the data buffer on host memory.
 * @param buflen - Length of data buffer.
 * @returns - 0 suceessful, < 0 failure.
 */
void AmbaRTSL_SDBusTestW(AMBA_SD_HOST *pHost,
                         AMBA_SD_CARD *pCard,
                         const UINT8 *pBuf, INT32 BufLen);

/**
 * A host sends the bus test data pattern to a card for spec 3.x (CMD19).
 *
 * @param host - The SDHC.
 * @param card - The card to use.
 * @param buf - Pointer to the data buffer on host memory, used by SW mode.
 * @returns - 0 suceessful, < 0 failure.
 */
void AmbaRTSL_SDSendTuningPattern(AMBA_SD_HOST *pHost,
                                  AMBA_SD_CARD *pCard,
                                  UINT8 *pBuf);
#define TUNING_BLOCK_SIZE   64

/**
 * Writes a data stream from the host, starting at the given address,
 * until a STOP_TRANSMISSION follows (CMD20).
 *
 * @param host - The SDHC.
 * @param card - The card to use.
 * @param addr - The data address of target memory.
 * @param buf - Pointer to the data buffer on host memory.
 * @returns - 0 suceessful, < 0 failure.
 */
INT32 AmbaRTSL_SDWriteDataUntilStop(AMBA_SD_HOST *pHost,
                                    AMBA_SD_CARD *pCard,
                                    UINT32 Addr, UINT8 *pBuf);

/**
 * Defines to optimize card operation to support Speed Class recording
 * for spec 3.x (CMD20).
 *
 * @param host - The SDHC.
 * @param card - The card to use.
 * @param ssc - Speed Class Control.
 * @returns - 0 suceessful, < 0 failure.
 */
INT32 AmbaRTSL_SDSpeedClassControl(AMBA_SD_HOST *pHost,
                                   AMBA_SD_CARD *pCard,
                                   UINT16 Scc);
#define SCC_START_RECORDING     0x0
#define SCC_CREATE_DIR          0x1
#define SCC_END_REC_WITHOUT_MOVE    0x2
#define SCC_END_REC_WITH_MOVE       0x3

INT32 AmbaRTSL_SDXCSpeedClassCtrl(INT32 Slot, UINT16 Scc);
#define SCC_ERR_UNSUPPORT_SLOT      -1  /**< Not a SD slot */
#define SCC_ERR_UNSUPPORT_CMD       -2  /**< Card doesn't support SSC */


/**
 * Defines the number of blocks which are going to be transferred in the
 * immediately succeeding multiple block read or write command. If the
 * argument is all 0s, the subsequent read/write operation will be
 * open-ended (CMD23).
 *
 * @param host - The SDHC.
 * @param card - The card to use.
 * @param count - Number of blocks.
 * @returns - 0 successful, < 0 failed
 */
void AmbaRTSL_SDSetBlockCount(AMBA_SD_HOST *pHost,
                              AMBA_SD_CARD *pCard,
                              INT32 Count);

/**
 * Writes a block of the size selected by the SET_BLOCKLEN command (CMD24).
 *
 * @param host - The SDHC.
 * @param card - The card to use.
 * @param addr - The data address of target memory.
 * @param buf - Pointer to the data buffer on host memory.
 * @returns - 0 successful, < 0 failed
 */
void AmbaRTSL_SDWriteBlock(AMBA_SD_HOST *pHost,
                           AMBA_SD_CARD *pCard,
                           UINT32 Addr, const UINT8 *pBuf);

/**
 * Continuously writes blocks of data until 'Stop Tran' token is sent
 * (instead of 'Start Block') (CMD25).
 *
 * @param host - The SDHC.
 * @param card - The card to use.
 * @param addr - The data address of target memory.
 * @param blocks - The number of blocks.
 * @param buf - Pointer to the data buffer on host memory.
 * @returns - 0 successful, < 0 failed
 */
INT32 AmbaRTSL_DWriteMultipleBlock(AMBA_SD_HOST *pHost,
                                   AMBA_SD_CARD *pCard,
                                   UINT32 Addr, INT32 Blocks, const UINT8 *pBuf);

/**
 * Programming of the card identification register. This command shall be
 * issued only once. The card contains hardware to prevent this operation
 * after the first programming. Normally this command is reserved for the
 * manufacturer (CMD26).
 *
 * @param host - The SDHC.
 * @param card - The card to use.
 * @returns - 0 successful, < 0 failed
 */
INT32 AmbaRTSL_SDProgramCid(AMBA_SD_HOST *pHost,
                            AMBA_SD_CARD *pCard);

/**
 * Programming of the programmable bits of the CSD (CMD27).
 *
 * @param host - The SDHC.
 * @param card - The card to use.
 * @returns - 0 successful, < 0 failed
 */
void AmbaRTSL_SDProgramCsd(AMBA_SD_HOST *pHost,
                           AMBA_SD_CARD *pCard);

/**
 * If the card has write protection features, this command sets the write
 * protection bit of the addressed group. The properties of write protection
 * are coded in the card specific data (WP_GRP_SIZE) (CMD28).
 *
 * @param host - The SDHC.
 * @param card - The card to use.
 * @param addr - The data address of the group.
 * @returns - 0 successful, < 0 failed
 */
void AmbaRTSL_SDSetWriteProt(AMBA_SD_HOST *pHost,
                             AMBA_SD_CARD *pCard,
                             UINT32 Addr);

/**
 * If the card has write protection deatures, this command clears the write
 * protection bit of the addressed group (CMD29).
 *
 * @param host - The SDHC.
 * @param card - The card to use.
 * @param addr - The data address of the group.
 * @returns - 0 successful, < 0 failed
 */
void AmbaRTSL_SDClearWriteProt(AMBA_SD_HOST *pHost,
                               AMBA_SD_CARD *pCard,
                               UINT32 Addr);


/**
 * If the card has write protection features, this command asks the card to
 * send the status of the write protection bits (CMD30).
 *
 * @param host - The SDHC.
 * @param card - The card to use.
 * @param addr - The protected data address.
 * @param prot - Write-back variable of prot sent.
 * @returns - 0 successful, < 0 failed
 */
void AmbaRTSL_SDSendWriteProt(AMBA_SD_HOST *pHost,
                              AMBA_SD_CARD *pCard,
                              UINT32 Addr, UINT32 *pProt);

/**
 * Sets the addresse of the first write block to be erased (CMD32).
 * This is an SD memory command.
 *
 * @param host - The SDHC.
 * @param card - The card to use.
 * @param addr - The data address of the write block.
 * @returns - 0 successful, < 0 failed
 */
INT32 AmbaRTSL_SDEraseWriteBlkStart(AMBA_SD_HOST *pHost,
                                    AMBA_SD_CARD *pCard,
                                    UINT32 addr);

/**
 * Sets the address of the last write block of the continuous range
 * to be erased (CMD33).
 * This is an SD memory command.
 *
 * @param host - The SDHC.
 * @param card - The card to use.
 * @param addr - The data address of the write block.
 * @returns - 0 successful, < 0 failed
 */
INT32 AmbaRTSL_SDEraseWriteBlkEnd(AMBA_SD_HOST *pHost,
                                  AMBA_SD_CARD *pCard,
                                  UINT32 addr);

/**
 * Sets the addresse of the first group to be erased (CMD35).
 * This is an MMC command.
 *
 * @param host - The SDHC.
 * @param card - The card to use.
 * @param addr - The data address of the group.
 * @returns - 0 successful, < 0 failed
 */
void AmbaRTSL_SDEraseGroupStart(AMBA_SD_HOST *pHost,
                                AMBA_SD_CARD *pCard,
                                UINT32 addr);

/**
 * Sets the address of the last group continuous range to be erased (CMD36).
 * This is an MMC command.
 *
 * @param host - The SDHC.
 * @param card - The card to use.
 * @param addr - The data address of the group.
 * @returns - 0 successful, < 0 failed
 */
void AmbaRTSL_SDEraseGroupEnd(AMBA_SD_HOST *pHost,
                              AMBA_SD_CARD *pCard,
                              UINT32 Addr);

/**
 * Erases all previously selected write blocks (CMD38).
 *
 * @param host - The SDHC.
 * @param card - The card to use.
 * @returns - 0 successful, < 0 failed
 */
void AmbaRTSL_SDErase(AMBA_SD_HOST *pHost,
                      AMBA_SD_CARD *pCard);

/**
 * Used to write and read 8 bit (register) data fields. The command addresses
 * a card and a register and provides the data for writing if the write flag
 * is set. The R4 response contains data read from the addressed register.
 * This command accesses application dependent registers which are not defined
 * in the MultiMediaCard standard (CMD39).
 *
 * @param host - The SDHC.
 * @param card - The card to use.
 * @param flag - Register write flag.
 * @param addr - Register address.
 * @param data - Pointer to register data to read/write.
 * @returns - 0 successful, < 0 failed
 */
INT32 AmbaRTSL_SDFastIO(AMBA_SD_HOST *pHost,
                        AMBA_SD_CARD *pCard,
                        UINT8 Flag, UINT8 Addr, UINT8 *pData);

/**
 * Ses the system into interrupt mode (CMD40).
 *
 * @param host - The SDHC.
 * @param card - The card to use.
 * @returns - 0 successful, < 0 failed
 */
INT32 AmbaRTSL_SDGOIrqState(AMBA_SD_HOST *pHost,
                            AMBA_SD_CARD *pCard);
/**
 * Defines the data bus width ('00' = 1 bit or '10' = 4 bit bus) to be used
 * for data transfer. The allowed data bus width are given in SCR register
 * (ACMD6).
 *
 * @param host - The SDHC.
 * @param card - The card to use.
 * @param width - Data bus width.
 * @returns - 0 successful, < 0 failed
 */
void AmbaRTSL_SDSetBusWidth(AMBA_SD_HOST *pHost,
                            AMBA_SD_CARD *pCard,
                            INT32 Width);
#define SD_BUS_WIDTH_1_BIT 0
#define SD_BUS_WIDTH_4_BIT 2

/**
 * Send the number of the well written (without errors) blocks. Responds with
 * 32-bit + CRC data block (ACMD22).
 *
 * @param host - The SDHC.
 * @param card - The card to use.
 * @param blocks - The number of blocks.
 * @returns - 0 successful, < 0 failed

 */
void AmbaRTSL_SDSendNumWRBlocks(AMBA_SD_HOST *pHost,
                                AMBA_SD_CARD *pCard,
                                INT32 Blocks);

/**
 * Activates the card's initialization process (ACMD41).
 *
 * @param host - The SDHC.
 * @param card - The card to use.
 * @param wv - Working voltage argument.
 * @param ocr - Write-back register of the Operating Condition Register.
 * @returns - 0 successful, < 0 failed
 */
void AmbaRTSL_SDSendOPCondAcmd41(AMBA_SD_HOST *pHost,
                                 AMBA_SD_CARD *pCard,
                                 UINT32 wv, UINT32 *pOcr);
typedef struct {
        UINT8   StructureSpec;
        UINT8   EraseBus;
        UINT8   SDSpec3;
        UINT8   CmdSupport;
        UINT32  Bot32;
} ScrDat;

/*-----------------------------------------------------------------------------------------------*\
 * Defined in AmbaRTSL_SD.c (RTSL for SD)
\*-----------------------------------------------------------------------------------------------*/
extern void (*AmbaRTSL_SDInsertCallBack)(UINT32);
extern void (*AmbaRTSL_SDEjectCallBack)(UINT32);
extern void (*AmbaRTSL_SDIsrCallBack)(int);
AMBA_SD_HOST* AmbaRTSL_SdGetHost(int);
UINT16 AmbaRTSL_SDGetPostscaler(int HostId);
void AmbaRTSL_SDSetPostscaler(int HostId, UINT16 Postscaler);
INT32 AmbaRTSL_SDGetWriteProtect(AMBA_SD_HOST * pHost, AMBA_SD_CARD * pCard);
INT32 AmbaRTSL_SDGetBusStatus(AMBA_SD_HOST * pHost, AMBA_SD_CARD * pCard);
INT32 AmbaRTSL_SDCardINSlot(AMBA_SD_HOST *pHost, AMBA_SD_CARD *pCard);
void  AmbaRTSL_SDSendCmd(AMBA_SD_HOST * pHost, AMBA_SD_CARD * pCard, AMBA_SD_REQUEST * pMrq);
void  AmbaRTSL_SDReadData(AMBA_SD_HOST * pHost, AMBA_SD_CARD * pCard, AMBA_SD_REQUEST * pMrq);
INT32 AmbaRTSL_SDSuspend(AMBA_SD_HOST * pHost, AMBA_SD_CARD * pCard);
void  AmbaRTSL_SDWriteData(AMBA_SD_HOST * pHost, AMBA_SD_CARD * pCard, AMBA_SD_REQUEST * pMrq);
void  AmbaRTSL_SDCmdDone(AMBA_SD_HOST * pHost);
void  AmbaRTSL_SDDataDone(AMBA_SD_HOST * pHost, AMBA_SD_CARD * pCard, AMBA_SD_REQUEST * pMrq);
void  AmbaRTSL_SDAbort(AMBA_SD_HOST * pHost);
int   AmbaRTSL_SDInit(INT32 ID, UINT32 MaxFreq);
INT32 AmbaRTSL_SDResume(AMBA_SD_HOST *pHost, AMBA_SD_CARD *pCard);
void  AmbaRTSL_SDDeconfigCard(AMBA_SD_HOST * pHost, AMBA_SD_CARD * pCard);
void  AmbaRTSL_SDSetIos(AMBA_SD_HOST * pHost, AMBA_SD_IOS * pIos, AMBA_SD_CARD * pCard);
void  AmbaRTSL_SDHandleCoreFreqChange(void);
void  AmbaRTSL_SDSetIsrTarget(int HostId, int CpuId);

/* SD RTSL command layer */
void AmbaRTSL_SDGoIdleState(AMBA_SD_HOST * pHost, AMBA_SD_CARD * pCard);
void AmbaRTSL_SDSendStatus(AMBA_SD_HOST * pHost, AMBA_SD_CARD * pCard, UINT32 * Status);
void AmbaRTSL_SDSendStatusDone(AMBA_SD_HOST * pHost, AMBA_SD_CARD * pCard, UINT32 * Status);
void AmbaRTSL_SDSendScr(AMBA_SD_HOST * pHost, AMBA_SD_CARD * pCard, AMBA_SD_SCR * pScr);
void AmbaRTSL_SDSendScrDone(AMBA_SD_HOST * pHost, AMBA_SD_CARD * pCard, AMBA_SD_SCR * pScr);
void AmbaRTSL_SDSendAppCmd(AMBA_SD_HOST * pHost, AMBA_SD_CARD * pCard);
void AmbaRTSL_SDSendAppCmdDone(AMBA_SD_HOST * pHost, AMBA_SD_CARD * pCard);
void AmbaRTSL_SDSetClrCardDetect(AMBA_SD_HOST * pHost, AMBA_SD_CARD * pCard, int SetCD);
void AmbaRTSL_SDSetClrCardDetectDone(AMBA_SD_HOST * pHost, AMBA_SD_CARD * pCard, int SetCD);

void AmbaRTSL_SDSendOPCondAcmd41(AMBA_SD_HOST * pHost, AMBA_SD_CARD * pCard, UINT32 WV, UINT32 * pOcr);
void AmbaRTSL_SDSendOPCondAcmd41Done(AMBA_SD_HOST * pHost, AMBA_SD_CARD * pCard, UINT32 WV, UINT32 * pOcr);
void AmbaRTSL_SDSendNumWRBlocks(AMBA_SD_HOST * pHost, AMBA_SD_CARD * pCard, int Blocks);
void AmbaRTSL_SDSendNumWRBlocksDone(AMBA_SD_HOST * pHost, AMBA_SD_CARD * pCard, int Blocks);
void AmbaRTSL_SDSendSDStatus(AMBA_SD_HOST * pHost, AMBA_SD_CARD * pCard, AMBA_SD_SD_STATUS * pStatus);
void AmbaRTSL_SDSendSDStatusDone(AMBA_SD_HOST * pHost, AMBA_SD_CARD * pCard, AMBA_SD_SD_STATUS * pStatus);

void AmbaRTSL_SDSetBusWidth(AMBA_SD_HOST * pHost, AMBA_SD_CARD * pCard, int Width);
void AmbaRTSL_SDSetBusWidthDone(AMBA_SD_HOST * pHost, AMBA_SD_CARD * pCard, int Width);

int AmbaRTSL_SDioRWDirect(AMBA_SD_HOST * pHost, AMBA_SD_CARD * pCard, int rw, UINT8 Func, UINT32 reg, UINT8 * val);
#define SD_SDIO_REG_READ        0   /**< SDIO: Read register */
#define SD_SDIO_REG_WRITE       1   /**< SDIO: Write register */
#define SD_SDIO_REG_WRITE_READ  2   /**< SDIO: Read after write */
int AmbaRTSL_SDioRWDirectDone(AMBA_SD_HOST * pHost, AMBA_SD_CARD * pCard, int rw, UINT8 Func, UINT32 reg, UINT8 * val);

int AmbaRTSL_SdioRWExtended(AMBA_SD_HOST * pHost, AMBA_SD_CARD * pCard, int rw, UINT8 Func, UINT8 BlockMode, UINT8 OpCode, UINT32 Reg, UINT32 Count, UINT32 BlockSize, UINT8 * pDat);
/* R/W flag */
#define SD_SDIO_EXT_READ        0
#define SD_SDIO_EXT_WRITE       1

/* Block mode */
#define SD_SDIO_BYTE_MODE       0
#define SD_SDIO_BLOCK_MODE      1

/* OP code */
#define SD_SDIO_FIXED_ADDR      0
#define SD_SDIO_INCRE_ADDR      1

#define SD_SDIO_MEM_READ    0   /**< SDIO: Read memory */
#define SD_SDIO_MEM_WRITE   1   /**< SDIO: Write memory */
#define SD_SDIO_BYTE_MODE   0   /**< SDIO: blockmode = byte mode */
#define SD_SDIO_BLOCK_MODE  1   /**< SDIO: blockmode = block mode */
#define SD_SDIO_FIXED_ADDR  0   /**< SDIO: opcode = fixed address */
#define SD_SDIO_INCR_ADDR   1   /**< SDIO: opcode = incremental addr */
void AmbaRTSL_SdioRWExtendedDone(AMBA_SD_HOST * pHost, AMBA_SD_CARD * pCard, int rw, UINT8 Func, UINT8 BlockMode, UINT8 OpCode, UINT32 Reg, UINT32 Count, UINT32 BlockSize, UINT8 * pDat);

void AmbaRTSL_SDLockUnlock(AMBA_SD_HOST * pHost, AMBA_SD_CARD * pCard, AMBA_SD_LOCK * pLock);
void AmbaRTSL_SDLockUnlockDone(AMBA_SD_HOST * pHost, AMBA_SD_CARD * pCard, AMBA_SD_LOCK * pLock);

void AmbaRTSL_SDEraseWRBlkEnd(AMBA_SD_HOST * pHost, AMBA_SD_CARD * pCard, UINT32 Addr);
void AmbaRTSL_SDEraseWRBlkEndDone(AMBA_SD_HOST * pHost, AMBA_SD_CARD * pCard, UINT32 Addr);

void AmbaRTSL_SDEraseGroupStart(AMBA_SD_HOST * pHost, AMBA_SD_CARD * pCard, UINT32 Addr);
void AmbaRTSL_SDEraseGroupStartDone(AMBA_SD_HOST * pHost, AMBA_SD_CARD * pCard, UINT32 addr);
void AmbaRTSL_SDEraseGroupEnd(AMBA_SD_HOST * pHost, AMBA_SD_CARD * pCard, UINT32 Addr);
void AmbaRTSL_SDEraseGroupEndDone(AMBA_SD_HOST * pHost, AMBA_SD_CARD * pCard, UINT32 Addr);

void AmbaRTSL_SDErase(AMBA_SD_HOST * pHost, AMBA_SD_CARD * pCard);
void AmbaRTSL_SDEraseDone(AMBA_SD_HOST * pHost, AMBA_SD_CARD * pCard);

void AmbaRTSL_SDEraseWEBlkStart(AMBA_SD_HOST * pHost, AMBA_SD_CARD * pCard, UINT32 Addr);
void AmbaRTSL_SDEraseWEBlkStartDone(AMBA_SD_HOST * pHost, AMBA_SD_CARD * pCard, UINT32 Addr);

void AmbaRTSL_SDSendWriteProt(AMBA_SD_HOST * pHost, AMBA_SD_CARD * pCard, UINT32 Addr, UINT32 * pProt);
void AmbaRTSL_SDSendWriteProtDone(AMBA_SD_HOST * pHost, AMBA_SD_CARD * pCard, UINT32 Addr, UINT32 * pProt);

int AmbaSD_AllSendCid(AMBA_SD_HOST * pHost, AMBA_SD_CARD * pCard, AMBA_SD_CID * pCid);

void AmbaRTSL_SDSendRelativeAddr(AMBA_SD_HOST * pHost, AMBA_SD_CARD * pCard, UINT16 * Rca);
void AmbaRTSL_SDSendRelativeAddrDone(AMBA_SD_HOST * pHost, AMBA_SD_CARD * pCard, UINT16 * pRca);

void AmbaRTSL_SDSetDsr(AMBA_SD_HOST * pHost, AMBA_SD_CARD * pCard, UINT16 Dsr);
void AmbaRTSL_SDSetDsrDone(AMBA_SD_HOST * pHost, AMBA_SD_CARD * pCard, UINT16 Dsr);

void AmbaRTSL_SDIOSendOPCondition(AMBA_SD_HOST * pHost, AMBA_SD_CARD * pCard, UINT32 WV, UINT8 * c, UINT8 * pNio, UINT8 * m, UINT32 * pOcr);
void AmbaRTSL_SDIOSendOPConditionDone(AMBA_SD_HOST * pHost, AMBA_SD_CARD * pCard, UINT32 WV, UINT8 * c, UINT8 * pNio, UINT8 * m, UINT32 * pOcr);

void AmbaRTSL_SDSwitch(AMBA_SD_HOST * pHost, AMBA_SD_CARD * pCard, UINT8 Access, UINT8 Index, UINT8 Value, UINT8 CmdSet);
void AmbaRTSL_SDSwitchDone(AMBA_SD_HOST * pHost, AMBA_SD_CARD * pCard, UINT8 Access, UINT8 Index, UINT8 Value, UINT8 CmdSet);

int AmbaSD_SwitchFunc(AMBA_SD_HOST * pHost, AMBA_SD_CARD * pCard, UINT8 Mode, AMBA_SD_SW_FUNC_SWITCH * pSwitchReq, AMBA_SD_SW_FUNC_STATUS * pSW);

void AmbaRTSL_SDClearWriteProt(AMBA_SD_HOST * pHost, AMBA_SD_CARD * pCard, UINT32 Addr);
void AmbaRTSL_SDClearWriteProtDone(AMBA_SD_HOST * pHost, AMBA_SD_CARD * pCard, UINT32 Addr);

void AmbaRTSL_SDProgramCsd(AMBA_SD_HOST * pHost, AMBA_SD_CARD * pCard);
void AmbaRTSL_SDProgramCsdDone(AMBA_SD_HOST * pHost, AMBA_SD_CARD * pCard);

void AmbaRTSL_SDWriteMultipleBlock(AMBA_SD_HOST * pHost, AMBA_SD_CARD * pCard, UINT32 Addr, int Blocks, const UINT8 * pBuf);
void AmbaRTSL_SDWriteMultipleBlockDone(AMBA_SD_HOST * pHost, AMBA_SD_CARD * pCard, UINT32 Addr, int Blocks, const UINT8 * pBuf);

void AmbaRTSL_SDWriteBlockDone(AMBA_SD_HOST * pHost, AMBA_SD_CARD * pCard, UINT32 Addr, const UINT8 * pBuf);

void AmbaRTSL_SDSpeedClassCtrl(AMBA_SD_HOST * pHost, AMBA_SD_CARD * pCard, UINT16 Scc);
void AmbaRTSL_SDSpeedClassCtrlDone(AMBA_SD_HOST * pHost, AMBA_SD_CARD * pCard, UINT16 Scc);

int AmbaRTSL_SDSendTuningPatternDone(AMBA_SD_HOST * pHost, AMBA_SD_CARD * pCard, UINT8 * pBuf);

void AmbaRTSL_SDBusTestWDone(AMBA_SD_HOST * pHost, AMBA_SD_CARD * pCard, const UINT8 * pBuf, int BufLen);

void AmbaRTSL_SDReadSingleBlockDone(AMBA_SD_HOST * pHost, AMBA_SD_CARD * pCard, UINT32 Addr, UINT8 * pBuf);

void AmbaRTSL_SDSetWriteProt(AMBA_SD_HOST * pHost, AMBA_SD_CARD * pCard, UINT32 Addr);
void AmbaRTSL_SDSetWriteProtDone(AMBA_SD_HOST * pHost, AMBA_SD_CARD * pCard, UINT32 Addr);

void AmbaRTSL_SDSetBlockCount(AMBA_SD_HOST * pHost, AMBA_SD_CARD * pCard, int Count);
void AmbaRTSL_SDSetBlockCountDone(AMBA_SD_HOST * pHost, AMBA_SD_CARD * pCard, int Count);

void AmbaRTSL_SDSetBlockLength(AMBA_SD_HOST * pHost, AMBA_SD_CARD * pCard, int Blklen);
void AmbaRTSL_SDSetBlockLengthDone(AMBA_SD_HOST * pHost, AMBA_SD_CARD * pCard, int Blklen);

void AmbaRTSL_SDSelectCard(AMBA_SD_HOST * pHost, AMBA_SD_CARD * pCard);
void AmbaRTSL_SDSelectCardDone(AMBA_SD_HOST * pHost, AMBA_SD_CARD * pCard);

void AmbaRTSL_SDSendCsd(AMBA_SD_HOST * pHost, AMBA_SD_CARD * pCard, AMBA_SD_CSD * pCsd);
void AmbaRTSL_SDSendCsdDone(AMBA_SD_HOST * pHost, AMBA_SD_CARD * pCard, AMBA_SD_CSD * pCsd);

void AmbaRTSL_SDSendCidDone(AMBA_SD_HOST * pHost, AMBA_SD_CARD * pCard, AMBA_SD_CID * pCid);
void AmbaRTSL_SDReadMultipleBlockDone(AMBA_SD_HOST * pHost, AMBA_SD_CARD * pCard, UINT32 Addr, int Blocks, UINT8 * pBuf);

void AmbaRTSL_SDGOInactiveState(AMBA_SD_HOST * pHost, AMBA_SD_CARD * pCard);
void AmbaRTSL_SDGOInactiveStateDone(AMBA_SD_HOST * pHost, AMBA_SD_CARD * pCard);

void AmbaRTSL_SDBusTestR(AMBA_SD_HOST * pHost, AMBA_SD_CARD * pCard, UINT8 * pBuf, INT32 BufLen);
void AmbaRTSL_SDBusTestRDone(AMBA_SD_HOST * pHost, AMBA_SD_CARD * pCard, UINT8 * pBuf, INT32 BufLen);

void AmbaRTSL_SDStopTransmission(AMBA_SD_HOST * pHost, AMBA_SD_CARD * pCard);
void AmbaRTSL_SDStopTransmissionDone(AMBA_SD_HOST * pHost, AMBA_SD_CARD * pCard);

int AmbaRTSL_SDVoltageSwitchDone(AMBA_SD_HOST * pHost, AMBA_SD_CARD * pCard);
void AmbaRTSL_SDVoltageSwitch(AMBA_SD_HOST * pHost, AMBA_SD_CARD * pCard);

void AmbaRTSL_SDAllSendCid(AMBA_SD_HOST * pHost, AMBA_SD_CARD * pCard, AMBA_SD_CID * pCid);
void AmbaRTSL_SDAllSendCidDone(AMBA_SD_HOST * pHost, AMBA_SD_CARD * pCard, AMBA_SD_CID * pCid);

void AmbaRTSL_SDSwitchFunc(AMBA_SD_HOST * pHost, AMBA_SD_CARD * pCard, UINT8 mode, AMBA_SD_SW_FUNC_SWITCH * pSwitchReq, AMBA_SD_SW_FUNC_STATUS * pSW);
void AmbaRTSL_SDSwitchFuncDone(AMBA_SD_HOST * pHost, AMBA_SD_CARD * pCard, UINT8 mode, AMBA_SD_SW_FUNC_SWITCH * pSwitchReq, AMBA_SD_SW_FUNC_STATUS * pSW);

void AmbaRTSL_SDSendIFCond(AMBA_SD_HOST * pHost, AMBA_SD_CARD * pCard);
int AmbaRTSL_SDSendIFCondDone(AMBA_SD_HOST * pHost, AMBA_SD_CARD * pCard);

void AmbaRTSL_SDSendExtCsdDone(AMBA_SD_HOST * pHost, AMBA_SD_CARD * pCard, AMBA_SD_EXT_CSD * pExtCsd);

void  AmbaRTSL_SDSendOpCondCmd1(AMBA_SD_HOST *pHost, AMBA_SD_CARD *pCard, UINT32 WV, UINT32 *pOcr);
void AmbaRTSL_SDSendOpCondCmd1Done(UINT32 * pResp, UINT32 * pOcr);

void AmbaRTSL_SDSetWRBlkEraseCount(AMBA_SD_HOST *pHost, AMBA_SD_CARD *pCard, INT32 Blocks);
void AmbaRTSL_SDPhySetConfig(AMBA_SD_PHY_PARAM_e Type, UINT8 Value);
UINT8 AmbaRTSL_SDPhyGetConfig(AMBA_SD_PHY_PARAM_e Type);
void AmbaRTSL_SDPhySetDelayChainSel(UINT8 Value);
UINT8 AmbaRTSL_SDPhyGetDelayChainSel(void);
void AmbaRTSL_SDPhySetDutySel(UINT8 Value);
UINT8 AmbaRTSL_SDPhyGetDutySel(AMBA_SD_PHY_PARAM_e Type);
void AmbaRTSL_SDPhySetSbc(AMBA_SD_PHY_SBC_e Type, UINT16 Value);
UINT32 AmbaRTSL_SDPhyGetSbc(void);
void AmbaRTSL_SDPhySetSel(AMBA_SD_PHY_SEL_e Type, UINT8 Value);
UINT8 AmbaRTSL_SDPhyGetSel(AMBA_SD_PHY_SEL_e Type);
UINT8 AmbaRTSL_SDPhyGetVShift(void);
void AmbaRTSL_SDPhy0Reset(void);
void AmbaRTSL_SDPhySelReset(void);
void AmbaRTSL_SDSetRdLatencyCtrl(AMBA_SD_HOST *pHost, UINT8 DelayCycle);
UINT32 AmbaRTSL_SDGetRdLatencyCtrl(AMBA_SD_HOST *pHost);
void AmbaRTSL_SDSetHiSpdEnable(AMBA_SD_HOST *pHost, UINT8 HiSpdEnable);
UINT8 AmbaRTSL_SDGetEnableDDR(AMBA_SD_HOST *pHost);
void AmbaRTSL_SDSetEnableDDR(AMBA_SD_HOST *pHost, UINT8 EnableDDR);

#endif  /* _AMBA_RTSL_SD_H_ */
