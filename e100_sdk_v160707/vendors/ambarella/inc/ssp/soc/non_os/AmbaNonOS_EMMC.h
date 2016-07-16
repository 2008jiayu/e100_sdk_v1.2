/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaNonOS_EMMC.h
 *
 *  @Copyright      :: Copyright (C) 2012 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: NAND flash APIs w/o OS.
\*-------------------------------------------------------------------------------------------------------------------*/

#ifndef _AMBA_NONOS_EMMC_H_
#define _AMBA_NONOS_EMMC_H_

#define PART_MAGIC_NUM              0x8732dfe6
#define PART_HEADER_MAGIC_NUM       0xa324eb90

#define EMMC_PROG_ERR               -1
#define EMMC_PTB_GET_ERR            -2
#define EMMC_PTB_SET_ERR            -3
#define EMMC_META_SET_ERR           -4
#define EMMC_META_GET_ERR           -5

#define BOOT_BUS_WIDTH  177
#define BOOT_CONFIG     179
#define EMMC_HW_RESET   162
#define EXT_CSD_SIZE    512

#define EMMC_ACCP_USER      0
#define EMMC_ACCP_BP_1      1
#define EMMC_ACCP_BP_2      2

#define EMMC_BOOTP_USER     0x38
#define EMMC_BOOTP_BP_1     0x8
#define EMMC_BOOTP_BP_2     0x10

#define EMMC_BOOT_1BIT      0
#define EMMC_BOOT_4BIT      1
#define EMMC_BOOT_8BIT      2
#define EMMC_BOOT_HIGHSPEED 0x8
int AmbaNonOS_SdGoIdleState(AMBA_SD_HOST * pHost, AMBA_SD_CARD * pCard);
int AmbaNonOS_SdSendOpCondCmd1(AMBA_SD_HOST * pHost, AMBA_SD_CARD * pCard, UINT32 WV, UINT32 * pOcr);
int AmbaNonOS_SdAllSendCid(AMBA_SD_HOST * pHost, AMBA_SD_CARD * pCard, AMBA_SD_CID * pCid);
int AmbaNonOS_SdSendRelativeAddr(AMBA_SD_HOST * pHost, AMBA_SD_CARD * pCard, UINT16 * pRca);
int AmbaNonOS_SdSwitch(AMBA_SD_HOST * pHost, AMBA_SD_CARD * pCard, UINT8 Access, UINT8 Index, UINT8 Value, UINT8 CmdSet);
int AmbaNonOS_SdSelectCard(AMBA_SD_HOST * pHost, AMBA_SD_CARD * pCard);
int AmbaNonOS_SdSendExtCsd(AMBA_SD_HOST * pHost, AMBA_SD_CARD * pCard, AMBA_SD_EXT_CSD * pExtCsd);
int AmbaNonOS_SdSendCsd(AMBA_SD_HOST * pHost, AMBA_SD_CARD * pCard, AMBA_SD_CSD * pCsd);
int AmbaNonOS_SdSendStatus(AMBA_SD_HOST * pHost, AMBA_SD_CARD * pCard, UINT32 * Status);
int AmbaNonOS_SdBusTestR(AMBA_SD_HOST * pHost, AMBA_SD_CARD * pCard, UINT8 * pBuf, INT32 BufLen);
int AmbaNonOS_SdBusTestW(AMBA_SD_HOST * pHost, AMBA_SD_CARD * pCard, const UINT8 * pBuf, int BufLen);
int AmbaNonOS_SdSetBlockLength(AMBA_SD_HOST * pHost, AMBA_SD_CARD * pCard, int Blklen);
int AmbaNonOS_SdReadSingleBlock(AMBA_SD_HOST * pHost, AMBA_SD_CARD * pCard, UINT32 Addr, UINT8 * pBuf);
int AmbaNonOS_SdReadMultipleBlock(AMBA_SD_HOST * pHost, AMBA_SD_CARD * pCard, UINT32 Addr, int Blocks, UINT8 * pBuf);
int AmbaNonOS_SdWriteSingleBlock(AMBA_SD_HOST * pHost, AMBA_SD_CARD * pCard, UINT32 Addr, UINT8 * pBuf);
int AmbaNonOS_SdWriteMultipleBlock(AMBA_SD_HOST * pHost, AMBA_SD_CARD * pCard, UINT32 Addr, int Blocks, UINT8 * pBuf);
int AmbaNonOS_SdEraseGroupStart(AMBA_SD_HOST * pHost, AMBA_SD_CARD * pCard, UINT32 Addr);
int AmbaNonOS_SdEraseGroupEnd(AMBA_SD_HOST * pHost, AMBA_SD_CARD * pCard, UINT32 Addr);
int AmbaNonOS_SdErase(AMBA_SD_HOST * pHost, AMBA_SD_CARD * pCard);

int AmbaNonOS_SdReadSector(int Id, UINT8 *pBuf, UINT32 Sector, UINT32 Sectors);
int AmbaNonOS_SdWriteSector(int Id, UINT8 *pBuf, UINT32 Sector, UINT32 Sectors);
int AmbaNonOS_SdEraseSectror(int Id, long Sector, int Sectors);

int AmbaNonOS_EmmcSetBootConfig(UINT8 BootPartition, UINT8 AccessPartition, UINT8 BusWidth);

int AmbaNonOS_EmmcGetPartTable(AMBA_NAND_PART_TABLE_s * pTable);
int AmbaNonOS_EmmcSetPartTable(AMBA_NAND_PART_TABLE_s * pTable);
int AmbaNonOS_EmmcGetMeta(AMBA_NAND_PART_META_s * pMeta);
int AmbaNonOS_SdInitCard(AMBA_SD_HOST * pHost, AMBA_SD_CARD * pCard, int Repeat, UINT32 Fclock);

int AmbaNonOS_EmmcProgBlock(UINT8 * pRaw, UINT32 RawSize, UINT32 StartBlk, UINT32 NumBlks, int(* OutputProgress)(int, void *), void * pArg);

void AmbaNonOS_BootDeviceInit(void);

#endif /* _AMBA_NONOS_EMMC_H_ */
