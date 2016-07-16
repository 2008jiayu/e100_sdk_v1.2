/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaRTSL_NOR.h
 *
 *  @Copyright      :: Copyright (C) 2012 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Definitions of Run Time Support Library for NAND flash
\*-------------------------------------------------------------------------------------------------------------------*/

#ifndef _AMBA_RTSL_NOR_H_
#define _AMBA_RTSL_NOR_H_

#include "AmbaNOR_Def.h"
#include "AmbaRTSL_SPI.h"

/*-----------------------------------------------------------------------------------------------*\
 * NOR operations error code definitions.
\*-----------------------------------------------------------------------------------------------*/
#define NOR_OP_DRIVER_ER               -1
#define NOR_OP_ERASE_ER                -2
#define NOR_OP_PROG_ER                 -3
#define NOR_OP_READ_ER                 -4
#define NOR_OP_WP_ER                   -5
#define NOR_OP_NOT_READY_ER            -6

/*-----------------------------------------------------------------------------------------------*\
 * NOR device information.
\*-----------------------------------------------------------------------------------------------*/
typedef struct _AMBA_NOR_DEV_s_ {
    AMBA_NOR_DEV_INFO_s *pNorInfo;

    char    Name[128];                  /**< Name */
    UINT8   Present;                    /**< Device is present */
    int Format[2];                  /**< File system format ID for STG */
    UINT8   Status1;                     /**< Status Register1 of NOR devices */ 
    UINT8   Status2;                     /**< Status Register2 of NOR devices */
    UINT8   Config;                     /**< Config Register of NOR devices */    
    UINT8   ExtAddr_Flag;
    
    /** Device logic info */
    struct {
        UINT8  ManufactuerID;      /* Manufactuer ID */
        UINT8  DeviceID;        /* Device ID */
        UINT32  TotalByteSize;      /* Total number of bytes */
        UINT16  PageSize;   /* Program Page size in Byte  */
        UINT32  EraseBlockSize; /* Erase Block Size in Byte */
        UINT16  NumBlocks;    /* Number of Erase Block Regions */
        UINT8  LSBFirst;
        UINT32  Frequency;      /* Total number of bytes */
    } DevLogicInfo;

    /** Device Cmd info */
    struct {
        UINT8  Read;
        UINT8  Program;
        UINT8  WriteReg;
        UINT8  ReadID;
        UINT8  ResetEnable;
        UINT8  Reset;
        UINT8  ReadStatus0;
        UINT8  ReadStatus1;
        UINT8  ClearStatus;
        UINT8  WriteEnable;
        UINT8  WriteDisable;
        UINT8  EraseBlk;
        UINT8  EraseChip;
        UINT8  ReadDummyCycle;
    } DevCmdIndex;
} AMBA_NOR_DEV_s;

/*-----------------------------------------------------------------------------------------------*\
 * NOR cmd type definitions.
\*-----------------------------------------------------------------------------------------------*/
#define NOR_SPI_SEND_CMD    0x1
#define NOR_SPI_READ_REG   0x2
#define NOR_SPI_WRITE_DATA   0x3
#define NOR_SPI_READ_DATA   0x4
#define NOR_SPI_READ_DATA_DTR   0x5

/*-----------------------------------------------------------------------------------------------*\
 * NOR host controller.
\*-----------------------------------------------------------------------------------------------*/
typedef struct _AMBA_NOR_HOST_s_ {
    AMBA_NOR_DEV_s Device;       /**< The NAND device chip(s) */
    AMBA_SPI_CHANNEL_e SpiChanNo;
    INT8    Chip;
    UINT8           * pBuffer;  
} AMBA_NOR_HOST_s;

/*-----------------------------------------------------------------------------------------------*\
 * Data structure describing partitions in NOR flash.
\*-----------------------------------------------------------------------------------------------*/
typedef struct _AMBA_NOR_FW_PART_s_ {
    UINT32  StartBlk[TOTAL_FW_PARTS];           /* Start block */
    UINT32  NumBlks[TOTAL_FW_PARTS];            /* Number of blocks */
} AMBA_NOR_FW_PART_s;

/*-----------------------------------------------------------------------------------------------*\
 * Data structure describing partitions in NAND flash.
\*-----------------------------------------------------------------------------------------------*/
typedef struct _AMBA_NOR_MEDIA_PART_s_ {
    UINT32  StartBlk[ AMBA_NUM_FW_MEDIA_PARTITION];           /* Start block */
    UINT32  NumBlks[ AMBA_NUM_FW_MEDIA_PARTITION];            /* Number of blocks */
} AMBA_NOR_MEDIA_PART_s;

/*-----------------------------------------------------------------------------------------------*\
 * Defined in AmbaRTSL_NOR.c (RTSL for NOR flash)
\*-----------------------------------------------------------------------------------------------*/
extern void (*AmbaRTSL_NORIsrDoneCallBack)(void);

void AmbaRTSL_NorSpiStop(void);
int AmbaRTSL_NorInit(void);
void AmbaRTSL_NorReset(void);
void AmbaRTSL_NorResetEnable(void);

void AmbaRTSL_NorWaitInt(void);
void AmbaRTSL_NorSPIConfig(void);
void AmbaRTSL_NorSetDefaultSPIClk(void);
void AmbaRTSL_NorSpiWriteNoDMA(UINT32 Offset, UINT32 ByteCount, UINT8 *pDataBuf);
void AmbaRTSL_NorSpiWriteDMA(UINT32 Offset, UINT32 ByteCount, UINT8 *pDataBuf);
void AmbaRTSL_NorSpiReadNoDMA(UINT32 From, UINT32 ByteCount);
void AmbaRTSL_NorSpiReadDMA(UINT32 From, UINT32 ByteCount, UINT8 *pDataBuf);
void AmbaRTSL_ResetFIFO(AMBA_NOR_HOST_s *pHost);
void AmbaRTSL_NorEraseChip(void);
void AmbaRTSL_NorEraseBlock(UINT32 Offset, UINT8 AddrSize);
void AmbaRTSL_NorReadFIFO(UINT32 RxDataSize, UINT8 *pRxDataBuf);
void AmbaRTSL_NorCalFWPartInfo(AMBA_NOR_DEV_s *pDev);

void AmbaRTSL_NorWriteEnable(void);
void AmbaRTSL_NorWriteDisable(void);

void AmbaRTSL_NorClearStatus(void);
void AmbaRTSL_NorReadStatusCmd(UINT8 StatusNum);
void AmbaRTSL_NorSetExtAddr(void);
void AmbaRTSL_NorReadIDCmd(void);
AMBA_NOR_HOST_s *AmbaRTSL_NorGetHost(void);
AMBA_NOR_DEV_s *AmbaRTSL_NorGetDev(void);
int AmbaRTSL_NorSetDevInfo(AMBA_NOR_DEV_s *pDev, const AMBA_NOR_DEV_INFO_s *pNorDB);

#endif /* _AMBA_RTSL_NAND_H_ */
