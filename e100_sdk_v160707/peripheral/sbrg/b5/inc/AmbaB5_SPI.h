/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaB5_SPI.h
 *
 *  @Copyright      :: Copyright (C) 2013 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Definitions & Constants for B5 SPI APIs
\*-------------------------------------------------------------------------------------------------------------------*/

#ifndef _AMBA_B5_SPI_H_
#define _AMBA_B5_SPI_H_

/* SPI protocol modes */
typedef enum _AMBA_B5_SPI_PROTOCOL_MODE_e_ {
    AMBA_B5_SPI_CPOL_LOW_CPHA_LOW = 0,              /* Motorola SPI, clock polarity = low, clock phase = low */
    AMBA_B5_SPI_CPOL_LOW_CPHA_HIGH,                 /* Motorola SPI, clock polarity = low, clock phase = high */
    AMBA_B5_SPI_CPOL_HIGH_CPHA_LOW,                 /* Motorola SPI, clock polarity = high, clock phase = low */
    AMBA_B5_SPI_CPOL_HIGH_CPHA_HIGH                 /* Motorola SPI, clock polarity = high, clock phase = high */
} AMBA_B5_SPI_PROTOCOL_MODE_e;

/* SPI slave select polarity */
typedef enum _AMBA_B5_SPI_CHIP_SELECT_POL_e_ {
    AMBA_B5_SPI_CHIP_SELECT_ACTIVE_LOW = 0,         /* Slave select is active low */
    AMBA_B5_SPI_CHIP_SELECT_ACTIVE_HIGH             /* Slave select is active high */
} AMBA_B5_SPI_CHIP_SELECT_POL_e;

typedef struct _AMBA_B5_SPI_CONFIG_s_ {
    AMBA_B5_SPI_PROTOCOL_MODE_e     ProtocolMode;   /* SPI Protocol mode */
    AMBA_B5_SPI_CHIP_SELECT_POL_e   CsPolarity;     /* Slave select polarity */
    UINT8                           DataFrameSize;  /* Data frame size in number of bits */
    UINT32                          BaudRate;       /* Transfer BaudRate in Hz */
    UINT32                          NumDataFrames;  /* Number of data frames for read-only operation mode */
} AMBA_B5_SPI_CONFIG_s;

/*-----------------------------------------------------------------------------------------------*\
 * Defined in AmbaB5_SPI.c
\*-----------------------------------------------------------------------------------------------*/
int AmbaB5_SpiInit(void);
int AmbaB5_SpiTransfer(AMBA_B5_CHANNEL_s *pB5Chan, AMBA_B5_CHANNEL_e RxChan, AMBA_B5_SPI_CONFIG_s *pSpiConfig, UINT32 DataSize,
                       UINT32 *pTxDataBuf, UINT32 *pRxDataBuf);

#endif /* _AMBA_B5_SPI_H_ */
