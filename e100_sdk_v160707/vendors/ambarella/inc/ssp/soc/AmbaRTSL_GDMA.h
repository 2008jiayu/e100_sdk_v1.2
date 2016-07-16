/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaRTSL_GDMA.h
 *
 *  @Copyright      :: Copyright (C) 2013 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Definitions of Chip Support Library for Graphics DMA
\*-------------------------------------------------------------------------------------------------------------------*/

#ifndef _AMBA_RTSL_GDMA_H_
#define _AMBA_RTSL_GDMA_H_

#include "AmbaGDMA_Def.h"

/* GDMA OPCODE */
typedef enum _AMBA_GDMA_OPCODE_e_ {
    GDMA_OPCODE_LINEAR_COPY = 0,
    GDMA_OPCODE_2D_COPY_ONE_SRC,
    GDMA_OPCODE_2D_COPY_TWO_SRC,
    GDMA_OPCODE_ALPHA_WITH_PREMUL,
    GDMA_OPCODE_RUN_LEN_DECODE,
    GDMA_OPCODE_ALPHA_WITHOUT_PREMUL,
} AMBA_GDMA_OPCODE_e;

/*-----------------------------------------------------------------------------------------------*\
 * Defined in AmbaRTSL_GDMA.c
\*-----------------------------------------------------------------------------------------------*/
extern void (*AmbaRTSL_GdmaIsrCallBack)(void);

#define AmbaRTSL_GdmaIsrRegister(pIsr)  AmbaRTSL_GdmaIsrCallBack = (pIsr)

void AmbaRTSL_GdmaInit(void);
int AmbaRTSL_GdmaGetNumAvailableInstance(void);
int AmbaRTSL_GdmaLinearCopy(AMBA_GDMA_LINEAR_CTRL_s *pLinearCopy);
int AmbaRTSL_GdmaBlockCopy(AMBA_GDMA_BLOCK_CTRL_s *pBlockCopy);
int AmbaRTSL_GdmaColorKeying(AMBA_GDMA_BLOCK_CTRL_s *pComposite, UINT32 TransparentColor);
int AmbaRTSL_GdmaAlphaBlending(AMBA_GDMA_BLOCK_CTRL_s *pComposite, UINT8 SrcMultiplyFlag, UINT8 Alpha);

#endif /* _AMBA_RTSL_GDMA_H_ */
