/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaDMA_Def.h
 *
 *  @Copyright      :: Copyright (C) 2013 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Definitions & Constants for DMA APIs.
\*-------------------------------------------------------------------------------------------------------------------*/

#ifndef _AMBA_DMA_DEF_H_
#define _AMBA_DMA_DEF_H_

#define AMBA_MAX_DMA_DATA_TRANSFER_SIZE     (4*1024*1024L - 1)      /* maximum DMA data transfer size */

/*-----------------------------------------------------------------------------------------------*\
 * Number of DMA channels.
\*-----------------------------------------------------------------------------------------------*/
typedef enum _AMBA_DMA_CHANNEL_e_ {
    AMBA_DMA_CHANNEL0 = 0,                  /* DMA Channel-0  */
    AMBA_DMA_CHANNEL1,                      /* DMA Channel-1 */
    AMBA_DMA_CHANNEL2,                      /* DMA Channel-2 */
    AMBA_DMA_CHANNEL3,                      /* DMA Channel-3 */
    AMBA_DMA_CHANNEL4,                      /* DMA Channel-4 */
    AMBA_DMA_CHANNEL5,                      /* DMA Channel-5 */
    AMBA_DMA_CHANNEL6,                      /* DMA Channel-6 */
    AMBA_DMA_CHANNEL7,                      /* DMA Channel-7 */

    AMBA_NUM_DMA_CHANNEL,

    AMBA_DMA_CHANNEL_FDMA                   = AMBA_DMA_CHANNEL0,  /* FDMA Channel */

    AMBA_DMA_CHANNEL_SSI0_SPI_NOR_TX_REQ    = AMBA_DMA_CHANNEL0,  /* DMA channel: SSI0 and SPI NOR Tx Request */
    AMBA_DMA_CHANNEL_SSI0_SPI_NOR_RX_REQ    = AMBA_DMA_CHANNEL1,  /* DMA channel: SSI0 and SPI NOR Rx Request */

    AMBA_DMA_CHANNEL_SSI1_TX_ACK            = AMBA_DMA_CHANNEL2,  /* DMA channel: SSI1 Tx Acknowledge */
    AMBA_DMA_CHANNEL_SSI1_RX_ACK            = AMBA_DMA_CHANNEL3,  /* DMA channel: SSI1 Rx Acknowledge */

    AMBA_DMA_CHANNEL_SSI0_UART_TX_ACK       = AMBA_DMA_CHANNEL4,  /* DMA channel: SSI0 and UART Tx Acknowledge */
    AMBA_DMA_CHANNEL_SSI0_UART_RX_ACK       = AMBA_DMA_CHANNEL5,  /* DMA channel: SSI1 and UART Rx Acknowledge */
    AMBA_DMA_CHANNEL_PRIMARY_I2S_RX         = AMBA_DMA_CHANNEL6,  /* DMA channel: Primary I2S Rx */
    AMBA_DMA_CHANNEL_PRIMARY_I2S_TX         = AMBA_DMA_CHANNEL7   /* DMA channel: Primary I2S Tx */
} AMBA_DMA_CHANNEL_e;

/*-----------------------------------------------------------------------------------------------*\
 * Definitions of AMBA DMA Bus Data Transfer Size.
\*-----------------------------------------------------------------------------------------------*/
typedef enum _AMBA_DMA_BUS_DATA_SIZE_e_ {
    AMBA_DMA_BUS_DATA_1BYTE = 0,    /* Bus Transfer Size = 1 Byte */
    AMBA_DMA_BUS_DATA_2BYTE,        /* Bus Transfer Size = Halfword (2 Byte) */
    AMBA_DMA_BUS_DATA_4BYTE,        /* Bus Transfer Size = 1 Word (4 Byte) */
    AMBA_DMA_BUS_DATA_8BYTE         /* Bus Transfer Size = Double word (8 Byte) */
} AMBA_DMA_BUS_DATA_SIZE_e;

/*-----------------------------------------------------------------------------------------------*\
 * Definitions of AMBA DMA Bus Request Size for Burst Operation.
\*-----------------------------------------------------------------------------------------------*/
typedef enum _AMBA_DMA_BUS_REQUEST_SIZE_e_ {
    AMBA_DMA_BUS_BLOCK_8BYTE = 0,
    AMBA_DMA_BUS_BLOCK_16BYTE,
    AMBA_DMA_BUS_BLOCK_32BYTE,
    AMBA_DMA_BUS_BLOCK_64BYTE,
    AMBA_DMA_BUS_BLOCK_128BYTE,
    AMBA_DMA_BUS_BLOCK_256BYTE,
    AMBA_DMA_BUS_BLOCK_512BYTE,
    AMBA_DMA_BUS_BLOCK_1024BYTE
} AMBA_DMA_BUS_REQUEST_SIZE_e;

/*-----------------------------------------------------------------------------------------------*\
 * Definitions of DMA descriptor control
\*-----------------------------------------------------------------------------------------------*/
typedef union _AMBA_DMA_DESC_CTRL_s_ {
    UINT32  Data;

    struct {
        UINT32  StopOnError:    1;  /* [0]: 1=DMA channel will be disabled when an error occurs */
        UINT32  IrqOnError:     1;  /* [1]: 1=DMA channel will signal an interrupt when an error occurs */
        UINT32  IrqOnDone:      1;  /* [2]: 1=DMA channel will signal an interrupt if operation ends without an error */
        UINT32  Reserved0:      13; /* [15:3]: Reserved */
        UINT32  BusBlockSize:   3;  /* [18:16]: Bus Transaction Block size (AMBA_DMA_BUS_BLOCK_SIZE_e) */
        UINT32  BusDataSize:    2;  /* [20:19]: Bus Data Transfer size (AMBA_DMA_BUS_BLOCK_SIZE_e) */
        UINT32  NoBusAddrInc:   1;  /* [21]: 1=No bus address increment during DMA operation */
        UINT32  ReadMem:        1;  /* [22]: 1=DMA read from memory, 0=DMA read from AHB */
        UINT32  WriteMem:       1;  /* [23]: 1=DMA write memory, 0=DMA write to AHB */
        UINT32  EndOfChain:     1;  /* [24]: 1 = End of chain flag. DMA will stop after this descriptor */
        UINT32  Reserved1:      7;  /* [31:25]: */
    } Bits;
} AMBA_DMA_DESC_CTRL_s;

/*-----------------------------------------------------------------------------------------------*\
 * Definitions of DMA descriptor format
\*-----------------------------------------------------------------------------------------------*/
typedef struct _AMBA_DMA_DESC_s_ {
    void    *pSrcAddr;              /* pointer to the source Base Address */
    void    *pDestAddr;             /* pointer to destination Base Address */
    void    *pNextDesc;             /* pointer to next descriptor address */
    UINT32  *pStatus;               /* pointer to status: 2 Words */
    UINT32  DataSize;               /* Transfer byte count, max value = (4MB - 1) bytes */
    AMBA_DMA_DESC_CTRL_s Ctrl;      /* Descriptor's attribute */
} AMBA_DMA_DESC_s;

/*-----------------------------------------------------------------------------------------------*\
 * Definitions of DMA configuration/channel assignment
\*-----------------------------------------------------------------------------------------------*/
typedef enum _AMBA_DMA_CHANNEL01_ASSIGN_e_ {
    AMBA_DMA_NOR_SPI,
    AMBA_DMA_SPI0
} AMBA_DMA_CHANNEL01_ASSIGN_e;

typedef enum _AMBA_DMA_CHANNEL45_ASSIGN_e_ {
    AMBA_DMA_UART,
    AMBA_DMA_SPI_SLAVE
} AMBA_DMA_CHANNEL45_ASSIGN_e;

typedef struct _AMBA_DMA_CHANNEL_ASSIGN_s_ {
    AMBA_DMA_CHANNEL01_ASSIGN_e   Channel01;
    AMBA_DMA_CHANNEL45_ASSIGN_e   Channel45;
} AMBA_DMA_CHANNEL_ASSIGN_s;

#endif /* _AMBA_DMA_DEF_H_ */
