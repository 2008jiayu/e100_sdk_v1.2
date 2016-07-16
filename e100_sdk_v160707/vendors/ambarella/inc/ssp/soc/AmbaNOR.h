/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaNOR.h
 *
 *  @Copyright      :: Copyright (C) 2012 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Definitions & Constants for NAND flash APIs.
\*-------------------------------------------------------------------------------------------------------------------*/

#ifndef _AMBA_NOR_H_
#define _AMBA_NOR_H_

#include "AmbaNOR_Def.h"

/*-----------------------------------------------------------------------------------------------*\
 * the buffer size must align to 32 and smaller than the max size of DMA
\*-----------------------------------------------------------------------------------------------*/
#define AMBA_SPINOR_DMA_BUF_SIZE    4096

/*-----------------------------------------------------------------------------------------------*\
 * The flag of Nor interrupt definitions.
\*-----------------------------------------------------------------------------------------------*/
#define NOR_TRANS_DONE_FLAG     0x1 
#define NOR_SPIDMA_DONE_FLAG        0x2

/*-----------------------------------------------------------------------------------------------*\
 *  Timeout value for CMD and DataTrans 
\*-----------------------------------------------------------------------------------------------*/
#define NOR_CMD_TIMEOUT_VALUE   1000
#define NOR_TRAN_TIMEOUT_VALUE   10000

/*-----------------------------------------------------------------------------------------------*\
 * Data structure of NOR operations
\*-----------------------------------------------------------------------------------------------*/
typedef struct _AMBA_NOR_OP_s_ {
    AMBA_KAL_EVENT_FLAG_t   EventFlag;          /* Event Flags */
    AMBA_KAL_MUTEX_t        Mutex;              /* Mutex */
    UINT8                   *pPageBuf;          /* Pointer to the aligned page buffer address */
} AMBA_NOR_OP_s;

/*-----------------------------------------------------------------------------------------------*\
 * Defined in AmbaNOR.c (MW for NOR flash)
\*-----------------------------------------------------------------------------------------------*/
int AmbaNOR_SoftReset(void);
void AmbaNOR_TranDoneSet(void);

int AmbaNOR_WriteEnable(void);
int AmbaNOR_WriteDisable(void);

int AmbaNOR_ClearStatus(void);
int AmbaNOR_ReadStatus(UINT8 StatusNum,  UINT8 * Status);

int AmbaNOR_Erase(UINT32 Offset, UINT32 ByteCount);
int AmbaNOR_Readbyte(UINT32 Offset, UINT32 ByteCount, UINT8 *pDataBuf);
int AmbaNOR_Program(UINT32 Offset, UINT32 ByteCount, UINT8 *pDataBuf);
int AmbaNOR_Init(const AMBA_NOR_DEV_INFO_s *pNorDB);
int AmbaNOR_ProgBlock(UINT8 *pRaw, UINT32 RawSize, UINT32 StartBlk, UINT32 NumBlks);
int AmbaNOR_MediaPartRead(int ID, UINT8 *pBuf, UINT32 Sector, UINT32 Sectors);
int AmbaNOR_MediaPartWrite(int ID, UINT8 *pBuf, UINT32 Sector, UINT32 Sectors);
int AmbaNOR_WritePartition(UINT8 *pRaw, UINT32 RawSize, UINT32 PartID);
UINT32 AmbaNOR_MediaPartGetTotalSector(int ID);
int AmbaNorFwUpdaterSD_SetMagicCode(void);
int AmbaNorFwUpdaterSD_ClearMagicCode(void);
int AmbaNorFwUpdaterSD_GetMagicCode(UINT8 *pBuf);
void AmbaNOR_SetRebootClkSetting(void);
#endif /* _AMBA_NAND_H_ */
