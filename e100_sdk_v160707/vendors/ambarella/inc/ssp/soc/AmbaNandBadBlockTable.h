/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaNandBadBlockTable.h
 *
 *  @Copyright      :: Copyright (C) 2012 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Definitions & Constants for NAND flash bad block table APIs
\*-------------------------------------------------------------------------------------------------------------------*/

#ifndef _AMBA_NAND_BBT_H_
#define _AMBA_NAND_BBT_H_

/*-----------------------------------------------------------------------------------------------*\
 * Descriptor for the bad block table marker and the descriptor for
 * the pattern which identifies good and bad blocks.
\*-----------------------------------------------------------------------------------------------*/
typedef struct _AMBA_NAND_BBT_DESC_s_ {
    UINT32  Options;
    UINT32  Block;
    UINT8   PatternOffset;
    UINT8   PatternLen;
    UINT8   MaxSearchBlks;
    UINT8   *pPattern;
} AMBA_NAND_BBT_DESC_s;

/*-----------------------------------------------------------------------------------------------*\
 * Defined in AmbaNandBBT.c (MW for NAND Bad blocks table service)
\*-----------------------------------------------------------------------------------------------*/
int  AmbaNandBBT_Scan(void);
int  AmbaNandBBT_Exist(void);
int  AmbaNandBBT_Update(UINT32 BadBlock, UINT32 GoodBlock, UINT32 Type);
int  AmbaNandBBT_IsBadBlock(UINT32 Block);
void AmbaNandBBT_Show(void);
int  AmbaNandBBT_Erase(void);

#endif  /* _AMBA_NAND_BBT_H_ */
