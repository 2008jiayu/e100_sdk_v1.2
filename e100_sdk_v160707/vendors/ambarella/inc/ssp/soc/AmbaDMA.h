/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaDMA.h
 *
 *  @Copyright      :: Copyright (C) 2013 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Definitions & Constants for DMA APIs.
\*-------------------------------------------------------------------------------------------------------------------*/

#ifndef _AMBA_DMA_H_
#define _AMBA_DMA_H_

#include "AmbaDMA_Def.h"

/*-----------------------------------------------------------------------------------------------*\
 * Defined in AmbaDMA.c
\*-----------------------------------------------------------------------------------------------*/
int AmbaDMA_Init(void);
int AmbaDMA_Config(AMBA_DMA_CHANNEL_ASSIGN_s* pChannelAssign);

int AmbaDMA_Transfer(AMBA_DMA_CHANNEL_e DmaChanNo, AMBA_DMA_DESC_s *pDmaDesc);
int AmbaDMA_Wait(AMBA_DMA_CHANNEL_e DmaChanNo, UINT32 TimeOut);

#endif /* _AMBA_DMA_H_ */
