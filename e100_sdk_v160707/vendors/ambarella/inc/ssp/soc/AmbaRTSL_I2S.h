/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaRTSL_I2S.c
 *
 *  @Copyright      :: Copyright (C) 2012 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Definitions & Constants for SPI control APIs
\*-------------------------------------------------------------------------------------------------------------------*/

#ifndef _AMBA_RTSL_I2S_H_
#define _AMBA_RTSL_I2S_H_

#include "AmbaI2S_Def.h"

/*-----------------------------------------------------------------------------------------------*\
 * RTSL I2S Management Structure
\*-----------------------------------------------------------------------------------------------*/
typedef struct _AMBA_RTSL_I2S_CTRL_s_ {
    void    *pTxDmaAddr;          /* DMA address for Tx */
    void    *pRxDmaAddr;          /* DMA address for Rx */
} AMBA_RTSL_I2S_CTRL_s;

/*-----------------------------------------------------------------------------------------------*\
 * Defined in AmbaRTSL_I2S.c
\*-----------------------------------------------------------------------------------------------*/
extern AMBA_RTSL_I2S_CTRL_s AmbaRTSL_I2sCtrl[AMBA_NUM_I2S_CHANNEL];

void AmbaRTSL_I2sInit(void);
int  AmbaRTSL_I2sConfig(AMBA_I2S_CHANNEL_e I2sChanNo, AMBA_I2S_CTRL_INFO_s *Ctrl);
int  AmbaRTSL_I2sTxCtrl(AMBA_I2S_CHANNEL_e I2sChanNo, UINT32 EnableFlag);
int  AmbaRTSL_I2sRxCtrl(AMBA_I2S_CHANNEL_e I2sChanNo, UINT32 EnableFlag);
int  AmbaRTSL_I2sTxFifoReset(AMBA_I2S_CHANNEL_e I2sChanNo);
int  AmbaRTSL_I2sRxFifoReset(AMBA_I2S_CHANNEL_e I2sChanNo);
int  AmbaRTSL_I2sSetOverSampingRate(AMBA_I2S_CHANNEL_e I2sChanNo, AMBA_I2S_CLK_FREQ_e OverSampleIdx);

void AmbaRTSL_I2sEnableFdma(void);

#endif /* _AMBA_RTSL_I2S_H_ */
