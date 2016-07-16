/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaNonOS_NOR.h
 *
 *  @Copyright      :: Copyright (C) 2012 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: NOR flash APIs w/o OS.
\*-------------------------------------------------------------------------------------------------------------------*/

#ifndef _AMBA_NONOS_NOR_H_
#define _AMBA_NONOS_NOR_H_

#define PART_MAGIC_NUM              0x8732dfe6
#define PART_HEADER_MAGIC_NUM       0xa324eb90

#define NOR_PROG_ERR               -1
#define NOR_PTB_GET_ERR            -2
#define NOR_PTB_SET_ERR            -3
#define NOR_ERASE_ERR              -4
#define NOR_META_GET_ERR           -5
#define NOR_META_SET_ERR           -6

void AmbaNonOS_NorGetDevParam(AMBA_NOR_PART_TABLE_s *pTable);
int AmbaNonOS_NorProgBlock(UINT8 *pRaw, UINT32 RawSize, UINT32 StartBlk, UINT32 NumBlks,
                            int (*OutputProgress)(int, void *), void *pArg);
int AmbaNonOS_NorGetPartTable(AMBA_NOR_PART_TABLE_s *pTable);
int AmbaNonOS_NorSetPartTable(AMBA_NOR_PART_TABLE_s *pTable);
void AmbaNonOS_NORSoftReset(void);
void AmbaNonOS_NORClearStatus(void);
int AmbaNonOS_NorReadStatus(UINT8 StatusNum,  UINT8 * Status);
void AmbaNonOS_NorWriteEnable(void);
void AmbaNonOS_NorWriteDisable(void);
int AmbaNonOS_NorErase(UINT32 Offset, UINT32 ByteCount);
int AmbaNonOS_NorRead(UINT32 Offset, UINT32 ByteCount, UINT8 *pDataBuf);
int AmbaNonOS_NorProgram(UINT32 Offset, UINT32 ByteCount, UINT8 *pDataBuf);
int AmbaNonOS_NorIsInit(void);
int AmbaNonOS_NorInit(const AMBA_NOR_DEV_INFO_s *pNandDevInfo);
void AmbaNonOS_BootDeviceInit(void);
int AmbaNonOS_NorSetMeta(void);
int AmbaNonOS_NorGetMeta(AMBA_NAND_PART_META_s *pMeta);

#endif /* _AMBA_NONOS_NOR_H_ */
