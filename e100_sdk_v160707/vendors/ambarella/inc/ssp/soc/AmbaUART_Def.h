/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaUART_Def.h
 *
 *  @Copyright      :: Copyright (C) 2012 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Common Definitions & Constants for UART APIs
\*-------------------------------------------------------------------------------------------------------------------*/

#ifndef _AMBA_UART_DEF_H_
#define _AMBA_UART_DEF_H_

typedef enum _AMBA_UART_CHANNEL_e_ {
    AMBA_UART_CHANNEL1 = 0,
    AMBA_UART_CHANNEL0,

    AMBA_NUM_UART_CHANNEL
} AMBA_UART_CHANNEL_e;

typedef enum _AMBA_UART_DATA_BIT_e_ {
    AMBA_UART_DATA_5_BIT = 0,
    AMBA_UART_DATA_6_BIT,
    AMBA_UART_DATA_7_BIT,
    AMBA_UART_DATA_8_BIT,
} AMBA_UART_DATA_BIT_e;

typedef enum _AMBA_UART_PARITY_e_ {
    AMBA_UART_PARITY_NONE = 0,
    AMBA_UART_PARITY_EVEN,
    AMBA_UART_PARITY_ODD
} AMBA_UART_PARITY_e;

typedef enum _AMBA_UART_STOP_e_ {
    AMBA_UART_STOP_1_BIT = 0,
    AMBA_UART_STOP_1D5_BIT,
    AMBA_UART_STOP_2_BIT,
} AMBA_UART_STOP_e;

typedef enum _AMBA_UART_FLOW_CTRL_e_ {
    AMBA_UART_FLOW_CTRL_NONE = 0,
    AMBA_UART_FLOW_CTRL_HARDWARE,
    AMBA_UART_FLOW_CTRL_SOFTWARE,
} AMBA_UART_FLOW_CTRL_e;

typedef struct _AMBA_UART_CONFIG_s_ {
    UINT32  BaudRate;                   /* Baud Rate */

    AMBA_UART_DATA_BIT_e    DataBit;    /* number of data bits */
    AMBA_UART_PARITY_e      Parity;     /* parity */
    AMBA_UART_STOP_e        StopBit;    /* number of stop bits */
    AMBA_UART_FLOW_CTRL_e   FlowCtrl;   /* flow control */

    int     MaxRxRingBufSize;           /* maximum receive ring-buffer size in Byte */
    UINT8   *pRxRingBuf;                /* pointer to the receive ring-buffer */

    /* only valid for non-Dma mode */
    void    (*RxEventHandler)(int RxByteSize);  /* pointer to the Data Receiving Event Handler */
} AMBA_UART_CONFIG_s;

#endif /* _AMBA_UART_DEF_H_ */
