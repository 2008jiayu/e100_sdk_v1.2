/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaRTSL_DMA.h
 *
 *  @Copyright      :: Copyright (C) 2013 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Definitions of Run Time Support Library for DMA
\*-------------------------------------------------------------------------------------------------------------------*/

#ifndef _AMBA_RTSL_DMA_H_
#define _AMBA_RTSL_DMA_H_

#include "AmbaDMA_Def.h"

/*-----------------------------------------------------------------------------------------------*\
 * Defined in AmbaRTSL_DMA.c
\*-----------------------------------------------------------------------------------------------*/
extern void (*_AmbaRTSL_DmaIsrCallBack)(AMBA_DMA_CHANNEL_e DmaChanNo);

#define AmbaRTSL_DmaIsrRegister(pIsr)   _AmbaRTSL_DmaIsrCallBack = (pIsr)

void AmbaRTSL_DmaInit(void);
void AmbaRTSL_DmaConfig(AMBA_DMA_CHANNEL_ASSIGN_s* pChannelAssign);
int  AmbaRTSL_DmaTransfer(AMBA_DMA_CHANNEL_e DmaChanNo, AMBA_DMA_DESC_s *pDmaDesc);
int AmbaRTSL_FdmaTransfer(AMBA_DMA_DESC_s *pDmaMainDesc, AMBA_DMA_DESC_s *pDmaSpareDesc);    /* TODO */	
int  AmbaRTSL_FDmaParseStatus(void);
void AmbaRTSL_FDmaWaitInt(void);


int  AmbaRTSL_DmaStop(AMBA_DMA_CHANNEL_e DmaChanNo);
int  AmbaRTSL_DmaGetTransferByteCount(AMBA_DMA_CHANNEL_e DmaChanNo);

#endif /* _AMBA_RTSL_DMA_H_ */

