/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaGDMA.h
 *
 *  @Copyright      :: Copyright (C) 2012 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Definitions & Constants for 2D Graphics DMA Middleware APIs
\*-------------------------------------------------------------------------------------------------------------------*/

#ifndef _AMBA_GDMA_H_
#define _AMBA_GDMA_H_

#include "AmbaGDMA_Def.h"

/*-----------------------------------------------------------------------------------------------*\
 * Defined in AmbaGDMA.c
\*-----------------------------------------------------------------------------------------------*/
int AmbaGDMA_Init(void);

int AmbaGDMA_CheckStatus(void);
int AmbaGDMA_WaitCompletion(UINT32 TimeOut);

int AmbaGDMA_LinearCopy(AMBA_GDMA_LINEAR_CTRL_s *pLinearCtrl, UINT32 TimeOut);
int AmbaGDMA_BlockCopy(AMBA_GDMA_BLOCK_CTRL_s *pBlockCtrl, UINT32 TimeOut);
int AmbaGDMA_ColorKeying(AMBA_GDMA_BLOCK_CTRL_s *pBlockCtrl, UINT32 TransparentColor, UINT32 TimeOut);
int AmbaGDMA_AlphaBlending(AMBA_GDMA_BLOCK_CTRL_s *pBlockCtrl, UINT8 SrcMultiplyFlag, UINT8 Alpha, UINT32 TimeOut);

#endif  /* _AMBA_GDMA_H_ */
