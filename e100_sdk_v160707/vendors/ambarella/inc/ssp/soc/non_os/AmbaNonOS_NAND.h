/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaNonOS_NAND.h
 *
 *  @Copyright      :: Copyright (C) 2012 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: NAND flash APIs w/o OS.
\*-------------------------------------------------------------------------------------------------------------------*/

#ifndef _AMBA_NONOS_NAND_H_
#define _AMBA_NONOS_NAND_H_

#define PART_MAGIC_NUM              0x8732dfe6
#define PART_HEADER_MAGIC_NUM       0xa324eb90

#define NAND_PROG_ERR               -1
#define NAND_PTB_GET_ERR            -2
#define NAND_PTB_SET_ERR            -3
#define NAND_META_SET_ERR           -4
#define NAND_META_GET_ERR           -5

int     AmbaNonOS_NandLoadPartition(UINT32 PartID);
void    AmbaNonOS_NandGetDevParam(AMBA_NAND_PART_TABLE_s *pTable);
void    AmbaNonOS_NandOutputBadBlock(UINT32 Block, int Type);
int     AmbaNonOS_NandProgBlock(UINT8 *pRaw, UINT32 RawSize, UINT32 StartBlk, UINT32 NumBlks,
                                int (*OutputProgress)(int, void *), void *pArg);
int     AmbaNonOS_NandGetMeta(AMBA_NAND_PART_META_s *pMeta);
int     AmbaNonOS_NandSetMeta(void);
int     AmbaNonOS_NandGetPartTable(AMBA_NAND_PART_TABLE_s *pTable);
int     AmbaNonOS_NandSetPartTable(AMBA_NAND_PART_TABLE_s *pTable);
int     AmbaNonOS_NandErase(UINT32 Block);
int     AmbaNonOS_NandRead(UINT32 Block, UINT32 StartPage, UINT32 Pages, UINT8 *pMain,
                           UINT8 *pSpare, UINT32 Area);
int     AmbaNonOS_NandProgram(UINT32 Block, UINT32 StartPage, UINT32 Pages, UINT8 *pMain,
                              UINT8 *pSpare, UINT32 Area);
void    AmbaNonOS_NandReadStatus(UINT32 *Status);
void    AmbaNonOS_NandReset(void);
void    AmbaNonOS_NandReadID(AMBA_NAND_DEV_s *pDev);
int     AmbaNonOS_NandIsInit(void);
int     AmbaNonOS_NandInit(const AMBA_NAND_DEV_INFO_s *pNandDevInfo);

#endif /* _AMBA_NONOS_NAND_H_ */

