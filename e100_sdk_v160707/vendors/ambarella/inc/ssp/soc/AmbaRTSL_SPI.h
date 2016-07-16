/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaRTSL_SPI.c
 *
 *  @Copyright      :: Copyright (C) 2014 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Definitions & Constants for SPI control APIs
\*-------------------------------------------------------------------------------------------------------------------*/

#ifndef _AMBA_RTSL_SPI_H_
#define _AMBA_RTSL_SPI_H_

#include "AmbaSPI_Def.h"
#include "AmbaDMA_Def.h"

/*-----------------------------------------------------------------------------------------------*\
 * RTSL SPI Management Structure
\*-----------------------------------------------------------------------------------------------*/
typedef struct _AMBA_RTSL_SPI_CTRL_s_ {
    UINT16  SlaveSelect;        /* Slave select */
    UINT8   MaxNumSlaveSelect;  /* maximum Number of Slave Selects */
    UINT8   MaxNumFifoEntry;    /* maximum Number of FIFO entries */

    int     TxDataSize;         /* Tx Data byte size */
    void    *pTxDataBuf;        /* pointer to Tx Data buffer */
    int     RxDataSize;         /* Rx Data byte size */
    void    *pRxDataBuf;        /* pointer to Rx Data buffer */

    UINT16  Status;             /* Tx/Rx status */
    UINT16  IntID;              /* Interrupt ID */
    void    (*pISR)(int IntID); /* pointer to the ISR */
} AMBA_RTSL_SPI_CTRL_s;

/*-----------------------------------------------------------------------------------------------*\
 * Defined in AmbaRTSL_SPI.c
\*-----------------------------------------------------------------------------------------------*/
extern void (*AmbaRTSL_SpiIsrCallBack)(AMBA_SPI_CHANNEL_e SpiChanNo);

#define AmbaRTSL_SpiIsrRegister(pIsr)   AmbaRTSL_SpiIsrCallBack = (pIsr)

int AmbaRTSL_SpiInit(void);
int AmbaRTSL_SpiTransfer(AMBA_SPI_CHANNEL_e SpiChanNo, AMBA_SPI_CONFIG_s *pSpiConfig, int DataSize,
                         void *pTxDataBuf, void *pRxDataBuf, UINT8 EnableDma);

void AmbaRTSL_SpiStop(AMBA_SPI_CHANNEL_e SpiChanNo);
int AmbaRTSL_SpiGetTxDmaChan(AMBA_SPI_CHANNEL_e SpiChanNo);
int AmbaRTSL_SpiGetRxDmaChan(AMBA_SPI_CHANNEL_e SpiChanNo);

#endif /* _AMBA_RTSL_SPI_H_ */
