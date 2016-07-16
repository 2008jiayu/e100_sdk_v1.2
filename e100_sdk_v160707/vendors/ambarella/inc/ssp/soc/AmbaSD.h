/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaSD.h
 *
 *  @Copyright      :: Copyright (C) 2012 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Ambarella SD control APIs
\*-------------------------------------------------------------------------------------------------------------------*/

#ifndef _AMBA_SD_H_
#define _AMBA_SD_H_

#include "AmbaSD_Def.h"
#include "AmbaGPIO_Def.h"
/*-----------------------------------------------------------------------------------------------*\
 * Defined in AmbaSD.c (MW for SD)
\*-----------------------------------------------------------------------------------------------*/

/**
 * Initialize a card attached to the SD/MMC host controller. This function
 * goes through an indentification and status gathering process for each of
 * the MMC, SD memory, SDIO, or multi-function cards as specified in the
 * respective protocol specifications.
 *
 * This function should be called by a task that is set-up to perform
 * initialization procedures. The sequence should be:
 * 1. Host controller driver sets a host event (card insertion)
 * 2. The event wakes up (or notifies) a task that an initialization is
 *    required.
 * 3. The initialization task uses its context to perform the initialization.
 *
 * @param host - The SDHC.
 * @param repeat - The numbers of times to repeat SD initialization.
 * @param fclock - The specific clock to be initialized.
 * @returns - 0 successful, < 0 failure
 * @see scm
 */

int AmbaSD_InitCard(AMBA_SD_HOST *pHost, AMBA_SD_CARD *pCard, int Repeat, UINT32 Fclock);

INT32 AmbaSD_SetTimeout(UINT32 ID, UINT32 Type, UINT32 Val);
INT32 AmbaSD_GetTimeout(UINT32 ID, UINT32 Type, UINT32* pVal);

#define SD_CMD_TIMEOUT  0x1
#define SD_DATA_TIMEOUT 0x2

int AmbaSD_GoIdleState(AMBA_SD_HOST *pHost, AMBA_SD_CARD *pCard);
int AmbaSD_GoPreIdleState(AMBA_SD_HOST *pHost, AMBA_SD_CARD *pCard);
int AmbaSD_SendOpCondCmd1(AMBA_SD_HOST *pHost, AMBA_SD_CARD *pCard, UINT32 WV, UINT32 *pOcr);
int AmbaSD_AllSendCid(AMBA_SD_HOST *pHost, AMBA_SD_CARD *pCard, AMBA_SD_CID *pCid);
int AmbaSD_SendRelativeAddr(AMBA_SD_HOST *pHost, AMBA_SD_CARD *pCard, UINT16 *Rca);
int AmbaSD_SetDsr(AMBA_SD_HOST *pHost, AMBA_SD_CARD *pCard, UINT16 Dsr);
int AmbaSD_IOSendOPCondition(AMBA_SD_HOST *pHost, AMBA_SD_CARD *pCard, UINT32 WV, UINT8 *c, UINT8 *pNio, UINT8 *m, UINT32 *pOcr);
int AmbaSD_Switch(AMBA_SD_HOST *pHost, AMBA_SD_CARD *pCard, UINT8 Access, UINT8 Index, UINT8 Value, UINT8 CmdSet);
int AmbaSD_SwitchFunc(AMBA_SD_HOST *pHost, AMBA_SD_CARD *pCard, UINT8 Mode, AMBA_SD_SW_FUNC_SWITCH *pSwitchReq, AMBA_SD_SW_FUNC_STATUS *pSW);
int AmbaSD_SelectCard(AMBA_SD_HOST *pHost, AMBA_SD_CARD *pCard);
int AmbaSD_SendIFCond(AMBA_SD_HOST *pHost, AMBA_SD_CARD *pCard);
int AmbaSD_SendExtCsd(AMBA_SD_HOST *pHost, AMBA_SD_CARD *pCard, AMBA_SD_EXT_CSD *pExtCsd);
int AmbaSD_SendCsd(AMBA_SD_HOST *pHost, AMBA_SD_CARD *pCard, AMBA_SD_CSD *pCsd);
int AmbaSD_SendCid(AMBA_SD_HOST *pHost, AMBA_SD_CARD *pCard, AMBA_SD_CID *pCid);
int AmbaSD_VoltageSwitch(AMBA_SD_HOST *pHost, AMBA_SD_CARD *pCard);
int AmbaSD_ReadDataUntilStop(AMBA_SD_HOST *pHost, AMBA_SD_CARD *pCard, UINT32 Addr, UINT8 *buf);
int AmbaSD_StopTransmission(AMBA_SD_HOST *pHost, AMBA_SD_CARD *pCard);
int AmbaSD_SendStatus(AMBA_SD_HOST * pHost, AMBA_SD_CARD * pCard, UINT32 * Status);
int AmbaSD_BusTestR(AMBA_SD_HOST * pHost, AMBA_SD_CARD * pCard, UINT8 * pBuf, INT32 BufLen);
int AmbaSD_GOInactiveState(AMBA_SD_HOST *pHost, AMBA_SD_CARD *pCard);
int AmbaSD_SetBlockLength(AMBA_SD_HOST *pHost, AMBA_SD_CARD *pCard, int Blklen);
int AmbaSD_ReadSingleBlock(AMBA_SD_HOST *pHost, AMBA_SD_CARD *pCard, UINT32 Addr, UINT8 *pBuf);
int AmbaSD_ReadMultipleBlock(AMBA_SD_HOST *pHost, AMBA_SD_CARD *pCard, UINT32 Addr, int Blocks, UINT8 *pBuf);
int AmbaSD_BusTestW(AMBA_SD_HOST * pHost, AMBA_SD_CARD *pCard, const UINT8 *pBuf, int BufLen);
int AmbaSD_SendTuningPattern(AMBA_SD_HOST *pHost, AMBA_SD_CARD *pCard, UINT8 *pBuf);
int AmbaSD_SpeedClassCtrl(AMBA_SD_HOST *pHost, AMBA_SD_CARD *pCard, UINT16 Scc);
int AmbaSD_WriteUntilStop(AMBA_SD_HOST *pHost, AMBA_SD_CARD *pCard, UINT32 Addr, UINT8 *pBuf);
int AmbaSD_SetBlockCount(AMBA_SD_HOST *pHost, AMBA_SD_CARD *pCard, int Count);
int AmbaSD_WriteSingleBlock(AMBA_SD_HOST *pHost, AMBA_SD_CARD *pCard, UINT32 Addr, UINT8 *pBuf);
int AmbaSD_WriteMultipleBlock(AMBA_SD_HOST *pHost, AMBA_SD_CARD *pCard, UINT32 Addr, int Blocks, UINT8 *pBuf);
int AmbaSD_ProgramCsd(AMBA_SD_HOST *pHost, AMBA_SD_CARD *pCard);
int AmbaSD_SetWriteProt(AMBA_SD_HOST *pHost, AMBA_SD_CARD *pCard, UINT32 Addr);
int AmbaSD_ClearWriteProt(AMBA_SD_HOST *pHost, AMBA_SD_CARD *pCard, UINT32 Addr);
int AmbaSD_SendWriteProt(AMBA_SD_HOST *pHost, AMBA_SD_CARD *pCard, UINT32 Addr, UINT32 *pProt);
int AmbaSD_EraseWEBlkStart(AMBA_SD_HOST *pHost, AMBA_SD_CARD *pCard, UINT32 Addr);
int AmbaSD_EraseWRBlkEnd(AMBA_SD_HOST *pHost, AMBA_SD_CARD *pCard, UINT32 Addr);
int AmbaSD_EraseGroupStart(AMBA_SD_HOST *pHost, AMBA_SD_CARD *pCard, UINT32 Addr);
int AmbaSD_EraseGroupEnd(AMBA_SD_HOST *pHost, AMBA_SD_CARD *pCard, UINT32 Addr);
int AmbaSD_Erase(AMBA_SD_HOST *pHost, AMBA_SD_CARD *pCard);
int AmbaSD_LockUnlock(AMBA_SD_HOST *pHost, AMBA_SD_CARD *pCard, AMBA_SD_LOCK  *pLock);
int AmbaSD_SdioRWDirect(AMBA_SD_HOST *pHost, AMBA_SD_CARD *pCard, int RW, UINT8 Func, UINT32 Reg, UINT8 *pVal);
int AmbaSD_SdioRWExtended(AMBA_SD_HOST *pHost, AMBA_SD_CARD *pCard, int RW, UINT8 Func, UINT8 BlockMode, UINT8 Opcode, UINT32 Reg, UINT32 Count, UINT32 BlockSize, UINT8 *pDat);

/* Application CMD */
int AmbaSD_SetBusWidth(AMBA_SD_HOST *pHost, AMBA_SD_CARD *pCard, int Width);
int AmbaSD_SendSDStatus(AMBA_SD_HOST *pHost, AMBA_SD_CARD *pCard, AMBA_SD_SD_STATUS *pStatus);
int AmbaSD_SendNumWRBlocks(AMBA_SD_HOST *pHost, AMBA_SD_CARD *pCard, int Blocks);
int AmbaSD_SetWRBlkEraseCount(AMBA_SD_HOST * pHost, AMBA_SD_CARD *pCard, int Blocks);
int AmbaSD_SendOPCondAcmd41(AMBA_SD_HOST * pHost, AMBA_SD_CARD * pCard, UINT32 WV, UINT32 * pOcr);
int AmbaSD_SetClrCardDetect(AMBA_SD_HOST *pHost, AMBA_SD_CARD *pCard, int SetCD);
int AmbaSD_SendScr(AMBA_SD_HOST * pHost, AMBA_SD_CARD * pCard, AMBA_SD_SCR * pScr);

int AmbaSD_EraseSectror(int Id, long Sector, int Sectors);
int AmbaSD_ReadSector(int Id, UINT8 * pBuf, UINT32 Sector, UINT32 Sectors);
int AmbaSD_WriteSector(int Id, UINT8 * pBuf, UINT32 Sector, UINT32 Sectors);
INT32 AmbaSD_CardINSlot(AMBA_SD_HOST * pHost, AMBA_SD_CARD * pCard);
AMBA_SD_HOST *AmbaSD_GetHost(int Id);
int AmbaSD_Init(UINT32 HostNumber, UINT32 MaxFreq);
void AmbaSD_CardEject(UINT32 Id);
void AmbaSD_FwContainterCheck(int Id, UINT32 * pSector, UINT32 Sectors);
UINT32 AmbaSD_GetStorageSectorCount(int Id);
void AmbaSD_SetDrivingStrength(int Id, AMBA_SD_DRIVING_STRENGTH_TYPE_e Type, AMBA_SD_DRIVING_STRENGTH_VALUE_e Driving);
AMBA_SD_DRIVING_STRENGTH_VALUE_e AmbaSD_GetPinDrivingStrength(AMBA_GPIO_PIN_ID_e GpioPinID);
int AmbaSD_DelayCtrlAdjustPhy(UINT32 RoundDelay, UINT32 ClkPeriod, UINT8 HighSpeed, UINT8 DdrMode);
void AmbaSD_DelayCtrlReset(void);
void AmbaSD_SetEmmcBusWidth(UINT8 Width);
#define EMMC_BUSWIDTH_1BIT  0x0
#define EMMC_BUSWIDTH_4BIT  0x1
#define EMMC_BUSWIDTH_8BIT  0x2

extern void (*AmbaSD_SigInsertCallBack)(UINT32);
extern void (*AmbaSD_SigEjectCallBack)(UINT32);
extern void (*AmbaSD_FSSetScm1CallBack)(void);
extern void (*AmbaSD_FSSetScm2CallBack)(void);

#endif /* _AMBA_SD_H_ */
