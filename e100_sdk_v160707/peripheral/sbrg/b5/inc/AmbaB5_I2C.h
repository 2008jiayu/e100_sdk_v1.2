/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaB5_I2C.h
 *
 *  @Copyright      :: Copyright (C) 2013 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Definitions & Constants for B5 I2C APIs
\*-------------------------------------------------------------------------------------------------------------------*/

#ifndef _AMBA_B5_I2C_H_
#define _AMBA_B5_I2C_H_

#define AMBA_B5_I2C_MAX_FIFO_ENTRY  63

/* I2C channel */
typedef enum _AMBA_B5_I2C_CHANNEL_e_ {
    AMBA_B5_I2C_CHANNEL0 = 0,           /* I2C Channel-0 */
    AMBA_B5_I2C_CHANNEL1,               /* I2C Channel-1 */

    AMBA_B5_NUM_I2C_CHANNEL             /* Total Number of I2C Channels */
} AMBA_B5_I2C_CHANNEL_e;

typedef enum _AMBA_B5_I2C_SPEED_e_ {
    AMBA_B5_I2C_SPEED_STANDARD,         /* I2C Standard speed: 100Kbps */
    AMBA_B5_I2C_SPEED_FAST,             /* I2C Fast speed: 400Kbps */
    AMBA_B5_I2C_SPEED_FAST_PLUS,        /* I2C Fast-mode Plus speed: 1Mbps */
    AMBA_B5_I2C_SPEED_HIGH              /* I2C High-speed mode: 3.4Mbps */
} AMBA_B5_I2C_SPEED_e;

typedef struct _AMBA_B5_I2C_TRANSACTION_s_ {
    UINT8   SlaveAddr;                  /* I2C Slave Address */
    int     DataSize;                   /* Data buffer size */
    UINT8   *pDataBuf;                  /* Data buffer base address */
} AMBA_B5_I2C_TRANSACTION_s;

/*-----------------------------------------------------------------------------------------------*\
 * Defined in AmbaB5_I2C.c
\*-----------------------------------------------------------------------------------------------*/
int AmbaB5_I2cInit(void);

int AmbaB5_I2cWrite(
    AMBA_B5_CHANNEL_s *pB5Chan, AMBA_B5_I2C_CHANNEL_e I2cChanNo, AMBA_B5_I2C_SPEED_e I2cSpeed, UINT8 SlaveAddr,
    int TxDataSize, UINT8 *pTxDataBuf);

int AmbaB5_I2cBurstWrite(
    AMBA_B5_CHANNEL_s *pB5Chan, AMBA_B5_I2C_CHANNEL_e I2cChanNo, AMBA_B5_I2C_SPEED_e I2cSpeed, UINT8 SlaveAddr,
    int TxDataSize, UINT8 *pTxDataBuf);

int AmbaB5_I2cRead(
    AMBA_B5_CHANNEL_s *pB5Chan, AMBA_B5_I2C_CHANNEL_e I2cChanNo, AMBA_B5_I2C_SPEED_e I2cSpeed, UINT8 SlaveAddr,
    int RxDataSize, UINT8 *pRxDataBuf, AMBA_B5_CHANNEL_e RxChan);

int AmbaB5_I2cReadAfterWrite(
    AMBA_B5_CHANNEL_s *pB5Chan, AMBA_B5_I2C_CHANNEL_e I2cChanNo, AMBA_B5_I2C_SPEED_e I2cSpeed,
    int NumTxTransaction, AMBA_B5_I2C_TRANSACTION_s *pTxTransaction, AMBA_B5_I2C_TRANSACTION_s *pRxTransaction, AMBA_B5_CHANNEL_e RxChan);

#endif /* _AMBA_B5_I2C_H_ */
