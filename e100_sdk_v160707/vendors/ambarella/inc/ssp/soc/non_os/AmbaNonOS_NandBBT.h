/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaNonOS_NandBBT.c
 *
 *  @Copyright      :: Copyright (C) 2012 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: NAND flash bad block table APIs.
\*-------------------------------------------------------------------------------------------------------------------*/
#ifndef _AMBA_NONOS_NAND_BBT_H_
#define _AMBA_NONOS_NAND_BBT_H_

/*-----------------------------------------------------------------------------------------------*\
 * Descriptor for the bad block table marker and the descriptor for
 * the pattern which identifies good and bad blocks.
 *-----------------------------------------------------------------------------------------------*/
typedef struct _AMBA_NAND_BBT_DESC_ {
    UINT32  Options;
    UINT32   Block;
    UINT8   PatternOffset;
    UINT8   PatternLen;
    UINT8   MaxSearchBlks;
    UINT8   *pPattern;
} AMBA_NAND_BBT_DESC_s;

int     AmbaNonOS_NandBbtScan(void);
int     AmbaNonOS_NandBbtExist(void);
int     AmbaNonOS_NandBbtUpdate(UINT32 BadBlock, UINT32 GoodBlock, UINT32 Type);
int     AmbaNonOS_NandBbtIsBadBlock(UINT32 Block);
void    AmbaNonOS_NandBbtShow(void);
int     AmbaNonOS_NandBbtErase(void);

#endif /* _AMBA_NONOS_NAND_BBT_H_ */

