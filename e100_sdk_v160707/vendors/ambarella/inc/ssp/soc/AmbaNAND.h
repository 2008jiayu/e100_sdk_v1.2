/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaNAND.h
 *
 *  @Copyright      :: Copyright (C) 2012 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Definitions & Constants for NAND flash APIs.
\*-------------------------------------------------------------------------------------------------------------------*/

#ifndef _AMBA_NAND_H_
#define _AMBA_NAND_H_

#include "AmbaNAND_Def.h"

/*-----------------------------------------------------------------------------------------------*\
 * Use to indicate the nand is 512 byte page or 2k byte page nand.
\*-----------------------------------------------------------------------------------------------*/
#define NAND_TYPE_NONE              0
#define NAND_TYPE_512               1
#define NAND_TYPE_2K                2

/*-----------------------------------------------------------------------------------------------*\
 * NAND Command
\*-----------------------------------------------------------------------------------------------*/
#define NAND_CMD_RESET              0x0
#define NAND_CMD_COPYBACK           0x1
#define NAND_CMD_ERASE              0x2
#define NAND_CMD_READ_ID            0x3
#define NAND_CMD_READ_STATUS        0x4
#define NAND_CMD_READ               0x5
#define NAND_CMD_PROGRAM            0x6

/*-----------------------------------------------------------------------------------------------*\
 * Data structure of NAND operations
\*-----------------------------------------------------------------------------------------------*/
typedef struct _AMBA_NAND_OP_s_ {
    AMBA_KAL_EVENT_FLAG_t   EventFlag;          /* Event Flags */
    AMBA_KAL_MUTEX_t        Mutex;              /* Mutex */
    UINT8                   *pPageBuf;          /* Pointer to the aligned page buffer address */
    UINT8                   *pRawPageBuf;       /* Pointer to the actual page buffer address */
    UINT8                   *pSpareBuf;         /* Pointer to the aligned spare buffer address */
    UINT8                   *pRawSpareBuf;      /* Pointer to the actual spare buffer address */

#ifdef ENABLE_VERIFY_NFTL
    UINT8                   *pDebugPage;        /* Pointer to the aligned page debug buffer address */
    UINT8                   *pRawDebugPage;     /* Pointer to the actual page debug buffer address */
    UINT8                   *pDebugSpare;       /* Pointer to the aligned spare debug buffer address */
    UINT8                   *pRawDebugSpare;    /* Pointer to the actual spare debug buffer address */
#endif
} AMBA_NAND_OP_s;

/*-----------------------------------------------------------------------------------------------*\
 * Defined in AmbaNAND.c (MW for NAND flash)
\*-----------------------------------------------------------------------------------------------*/
extern AMBA_NAND_OP_s g_NandOP;

int  AmbaNAND_GetPartInfo(UINT32 PartId, UINT32 * pStartBlk, UINT32 * pNumBlks);
int  AmbaNAND_SetPartInfo(UINT32 PartId, UINT32 StartBlk, UINT32 NumBlks);

int  AmbaNAND_LoadPartition(UINT32 PartID);
int AmbaNAND_LoadPartitionData(int Id, UINT32 ImageOffset, UINT8 * pDst, UINT32 Len);
int  AmbaNAND_WritePartition(UINT8 * pBuf, UINT32 Size, UINT32 PartID);
int  AmbaNAND_ErasePartition(int PartID);

int  AmbaNAND_Copyback(UINT32 BlockFrom, UINT32 Page, UINT32 BlockTo, UINT32 TimeOut);
int  AmbaNAND_Erase(UINT32 Block, UINT32 TimeOut);
int  AmbaNAND_Read(UINT32 Block, UINT32 StartPage, UINT32 Pages, UINT8 *pMain,
                   UINT8 *pSpare, UINT32 Area, UINT32 TimeOut);
int  AmbaNAND_ReadSector(UINT32 Block, UINT32 Page, UINT32 Sec, UINT32 Secs,
                         UINT8 *pBuf, UINT32 Area, UINT32 TimeOut);
int  AmbaNAND_Program(UINT32 Block, UINT32 StartPage, UINT32 Pages, UINT8 *pMain,
                      UINT8 *pSpare, UINT32 Area, UINT32 TimeOut);
void AmbaNAND_ReadStatus(UINT32 *Status, UINT32 TimeOut);
int  AmbaNAND_MarkBlock(UINT32 Block);
int  AmbaNAND_Reset(UINT32 TimeOut);
int  AmbaNAND_IsInit(void);
int  AmbaNAND_Init(const AMBA_NAND_DEV_INFO_s *pNandDevInfo);
void AmbaNAND_CalcNftlPartition(UINT32 MPStartBlk);
void AmbaNAND_InitNftlPart(void);

UINT16  AmbaNAND_GetMainSize(void);
UINT16  AmbaNAND_GetPagesPerBlock(void);

#endif /* _AMBA_NAND_H_ */
