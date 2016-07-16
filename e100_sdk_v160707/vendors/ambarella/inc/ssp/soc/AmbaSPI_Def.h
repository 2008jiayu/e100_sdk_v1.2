/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaSPI_Def.h
 *
 *  @Copyright      :: Copyright (C) 2012 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Common Definitions & Constants for SPI APIs
\*-------------------------------------------------------------------------------------------------------------------*/

#ifndef _AMBA_SPI_DEF_H_
#define _AMBA_SPI_DEF_H_

typedef enum _AMBA_SPI_CHANNEL_e_ {
    AMBA_SPI_MASTER = 0,
    AMBA_SPI_MASTER1,
    AMBA_SPI_SLAVE,

    AMBA_NUM_SPI_CHANNEL            /* Total Number of SPI Channels (2 Masters, 1 Slave) */
} AMBA_SPI_CHANNEL_e;

/* SPI mode number: The combinations of clock polarity and phases */
typedef enum _AMBA_SPI_PROTOCOL_MODE_e_ {
    AMBA_SPI_CPOL_LOW_CPHA_LOW = 0, /* the leading (first) clock edge is rising edge, and data is sampled on the leading (first) clock edge */
    AMBA_SPI_CPOL_LOW_CPHA_HIGH,    /* the leading (first) clock edge is rising edge, and data is sampled on the trailing (second) clock edge */
    AMBA_SPI_CPOL_HIGH_CPHA_LOW,    /* the leading (first) clock edge is fallng edge, and data is sampled on the leading (first) clock edge */
    AMBA_SPI_CPOL_HIGH_CPHA_HIGH    /* the leading (first) clock edge is fallng edge, and data is sampled on the trailing (second) clock edge */
} AMBA_SPI_PROTOCOL_MODE_e;

/* SPI slave select polarity */
typedef enum _AMBA_SPI_CHIP_SELECT_POL_e_ {
    AMBA_SPI_CHIP_SELECT_ACTIVE_LOW = 0,        /* Slave select is active low */
    AMBA_SPI_CHIP_SELECT_ACTIVE_HIGH            /* Slave select is active high */
} AMBA_SPI_CHIP_SELECT_POL_e;

typedef struct _AMBA_SPI_CONFIG_s_ {
    AMBA_SPI_PROTOCOL_MODE_e    ProtocolMode;   /* SPI Protocol mode */
    AMBA_SPI_CHIP_SELECT_POL_e  CsPolarity;     /* Slave select polarity */
    UINT8                       SlaveID;        /* Slave ID (for AMBA_SPI_MASTER only) or GPIO-pin number (for AMBA_SPI_SLAVE only) */
    UINT8                       DataFrameSize;  /* Data Frame Size in Bit */
    UINT32                      BaudRate;       /* Transfer BaudRate in Hz */
    UINT8                       LsbFirst;       /* Transfer LSB first */
} AMBA_SPI_CONFIG_s;

#endif /* _AMBA_SPI_DEF_H_ */
