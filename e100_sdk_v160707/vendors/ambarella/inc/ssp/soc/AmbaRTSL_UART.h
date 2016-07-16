/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaRTSL_UART.h
 *
 *  @Copyright      :: Copyright (C) 2012 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Definitions & Constants for UART RTSL APIs
\*-------------------------------------------------------------------------------------------------------------------*/

#ifndef _AMBA_RTSL_UART_H_
#define _AMBA_RTSL_UART_H_

#include "AmbaUART_Def.h"

/*-----------------------------------------------------------------------------------------------*\
 * RTSL UART Management Structure
\*-----------------------------------------------------------------------------------------------*/
typedef struct _AMBA_RTSL_UART_RX_BUF_CTRL_s_ {
    volatile int DataSize;          /* Ring buffer Byte size */

    int     ReadPtr;                /* Read pointer to the Ring Buffer */
    int     WritePtr;               /* Write pointer to the Ring Buffer */

    int     MaxRingBufSize;         /* maximum receive ring-buffer size in Byte */
    UINT8   *pRingBuf;              /* pointer to the receive ring-buffer */
} AMBA_RTSL_UART_RX_BUF_CTRL_s;

typedef struct _AMBA_RTSL_UART_CTRL_s_ {
    volatile int TxDataSize;        /* Tx Data byte size */
    UINT8   *pTxDataBuf;            /* pointer to Tx Data buffer */
    AMBA_UART_FLOW_CTRL_e FlowCtrl; /* flow control */

    /* only valid for non-Dma mode */
    UINT16  IntID;                  /* Interrupt ID */
    void    (*pISR)(int IntID);     /* pointer to the ISR */

    AMBA_RTSL_UART_RX_BUF_CTRL_s    RxBufCtrl;
} AMBA_RTSL_UART_CTRL_s;

/*-----------------------------------------------------------------------------------------------*\
 * Defined in AmbaRTSL_UART.c
\*-----------------------------------------------------------------------------------------------*/
extern void (*_AmbaRTSL_UartRxIsrCallBack)(AMBA_UART_CHANNEL_e UartChanNo);
extern void (*_AmbaRTSL_UartTxIsrCallBack)(AMBA_UART_CHANNEL_e UartChanNo);
extern AMBA_RTSL_UART_CTRL_s _AmbaRTSL_UartCtrl[];

#define AmbaRTSL_UartRxIsrRegister(pISR)    _AmbaRTSL_UartRxIsrCallBack = (pISR)
#define AmbaRTSL_UartTxIsrRegister(pISR)    _AmbaRTSL_UartTxIsrCallBack = (pISR)

int AmbaRTSL_UartInit(void);

int AmbaRTSL_UartConfig(AMBA_UART_CHANNEL_e UartChanNo, AMBA_UART_CONFIG_s *pUartConfig);
int AmbaRTSL_UartWrite(AMBA_UART_CHANNEL_e UartChanNo, int TxDataSize, UINT8 *pTxDataBuf);
int AmbaRTSL_UartRead(AMBA_UART_CHANNEL_e UartChanNo, int RxDataSize, UINT8 *pRxDataBuf);
UINT8 AmbaRTSL_UartFetch1ByteFromRxRingBuf(AMBA_RTSL_UART_RX_BUF_CTRL_s *pRxBufCtrl);
int AmbaRTSL_UartClearRxRingBuf(AMBA_UART_CHANNEL_e UartChanNo);
int AmbaRTSL_UartGetTxDmaChan(AMBA_UART_CHANNEL_e UartChanNo);
int AmbaRTSL_UartGetRxDmaChan(AMBA_UART_CHANNEL_e UartChanNo);
#endif /* _AMBA_RTSL_UART_H_ */
