/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaSPI.h
 *
 *  @Copyright      :: Copyright (C) 2014 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Definitions & Constants for SPI Middleware APIs
\*-------------------------------------------------------------------------------------------------------------------*/

#ifndef _AMBA_SPI_H_
#define _AMBA_SPI_H_

#include "AmbaSPI_Def.h"

/*-----------------------------------------------------------------------------------------------*\
 * Defined in AmbaSPI.c
\*-----------------------------------------------------------------------------------------------*/
int AmbaSPI_Init(void);
int AmbaSPI_Transfer(AMBA_SPI_CHANNEL_e SpiChanNo, AMBA_SPI_CONFIG_s *pSpiConfig,
                     int DataSize, void *pTxDataBuf, void *pRxDataBuf, UINT32 TimeOut);
#endif /* _AMBA_SPI_H_ */
