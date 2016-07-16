/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaI2S.h
 *
 *  @Copyright      :: Copyright (C) 2013 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Definitions & Constants for I2C Middleware APIs
\*-------------------------------------------------------------------------------------------------------------------*/

#ifndef _AMBA_I2S_H_
#define _AMBA_I2S_H_

#include "AmbaI2S_Def.h"

/*-----------------------------------------------------------------------------------------------*\
 * Defined in AmbaI2S.c
\*-----------------------------------------------------------------------------------------------*/
int  AmbaI2S_Config(AMBA_I2S_CHANNEL_e I2sChanNo, AMBA_I2S_CTRL_INFO_s *Ctrl);
int  AmbaI2S_TxCtrl(AMBA_I2S_CHANNEL_e I2sChanNo, UINT32 EnableFlag);
int  AmbaI2S_RxCtrl(AMBA_I2S_CHANNEL_e I2sChanNo, UINT32 EnableFlag);
void *AmbaI2S_GetTxDmaAddress(AMBA_I2S_CHANNEL_e I2sChanNo);
void *AmbaI2S_GetRxDmaAddress(AMBA_I2S_CHANNEL_e I2sChanNo);
int  AmbaI2S_TxFifoReset(AMBA_I2S_CHANNEL_e I2sChanNo);
int  AmbaI2S_RxFifoReset(AMBA_I2S_CHANNEL_e I2sChanNo);
int  AmbaI2S_SetOverSampingRate(AMBA_I2S_CHANNEL_e I2sChanNo, AMBA_I2S_CLK_FREQ_e OverSampleIdx);

#endif /* _AMBA_I2S_H_ */
