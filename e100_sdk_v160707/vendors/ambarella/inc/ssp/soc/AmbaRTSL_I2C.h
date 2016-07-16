/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaRTSL_I2C.h
 *
 *  @Copyright      :: Copyright (C) 2012 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Definitions & Constants for I2C RTSL APIs
\*-------------------------------------------------------------------------------------------------------------------*/

#ifndef _AMBA_RTSL_I2C_H_
#define _AMBA_RTSL_I2C_H_

#include "AmbaI2C_Def.h"

#define AMBA_I2C_MAX_FIFO_ENTRY     63

/*-----------------------------------------------------------------------------------------------*\
 * RTSL I2C Management Structure
\*-----------------------------------------------------------------------------------------------*/
typedef struct _AMBA_RTSL_I2C_CTRL_s_ {
    int     TxDataSize;         /* Tx Data byte size */
    UINT8   *pTxDataBuf;        /* pointer to Tx Data buffer */
    int     RxDataSize;         /* Rx Data byte size */
    UINT8   *pRxDataBuf;        /* pointer to Rx Data buffer */

    UINT16  Status;             /* Tx/Rx status */
    UINT16  IntID;              /* Interrupt ID */
    void    (*pISR)(int IntID); /* pointer to the ISR */
} AMBA_RTSL_I2C_CTRL_s;

#define AMBA_I2C_16BIT_TX_DATA_MODE     0x8000  /* the flag of 16-bit TX data mode */
#define AMBA_I2C_RX_VAR_LENGTH_MODE     0x4000  /* the flag of Read Variable Length of data mode */

/*-----------------------------------------------------------------------------------------------*\
 * Defined in AmbaRTSL_I2C.c
\*-----------------------------------------------------------------------------------------------*/
extern void (*AmbaRTSL_I2cIsrCallBack)(AMBA_I2C_CHANNEL_e I2cChanNo);
extern AMBA_RTSL_I2C_CTRL_s AmbaRTSL_I2cCtrl[AMBA_NUM_I2C_CHANNEL];

#define AmbaRTSL_I2cIsrRegister(pIsr)   AmbaRTSL_I2cIsrCallBack = (pIsr)

void AmbaRTSL_I2cIsrCtrl(int I2cChanNo, UINT32 Enable);

int AmbaRTSL_I2cInit(void);
int AmbaRTSL_I2cWrite(AMBA_I2C_CHANNEL_e I2cChanNo, AMBA_I2C_SPEED_e I2cSpeed, UINT8 SlaveAddr,
                      int TxDataSize, UINT8 *pTxDataBuf);
int AmbaRTSL_I2cRead(AMBA_I2C_CHANNEL_e I2cChanNo, AMBA_I2C_SPEED_e I2cSpeed, UINT8 SlaveAddr,
                     int RxDataSize, UINT8 *pRxDataBuf);
int AmbaRTSL_I2cReadAfterWrite(AMBA_I2C_CHANNEL_e I2cChanNo, AMBA_I2C_SPEED_e I2cSpeed,
                               int TxDataSize, UINT16 *pTxDataBuf,
                               int RxDataSize, UINT8 *pRxDataBuf);

int AmbaRTSL_I2cReadVarLength(AMBA_I2C_CHANNEL_e I2cChanNo, AMBA_I2C_SPEED_e I2cSpeed,
                              UINT8 SlaveAddr, int MaxDataSize, UINT8 *pRxDataBuf);
int AmbaRTSL_I2cSetSdaCurrentSrc(AMBA_I2C_CHANNEL_e I2cChanNo, UINT32 Enable);
int AmbaRTSL_I2cGetSdaCurrentSrc(AMBA_I2C_CHANNEL_e I2cChanNo);
int AmbaRTSL_I2cSetSclCurrentSrc(AMBA_I2C_CHANNEL_e I2cChanNo, UINT32 Enable);
int AmbaRTSL_I2cGetSclCurrentSrc(AMBA_I2C_CHANNEL_e I2cChanNo);
int AmbaRTSL_I2cSetDutyCycle(AMBA_I2C_CHANNEL_e I2cChanNo, UINT32 DutyCycle);
int AmbaRTSL_I2cGetDutyCycle(AMBA_I2C_CHANNEL_e I2cChanNo);
int AmbaRTSL_I2cSetStretchScl(AMBA_I2C_CHANNEL_e I2cChanNo, UINT32 Stretch);
int AmbaRTSL_I2cGetStretchScl(AMBA_I2C_CHANNEL_e I2cChanNo);









#endif /* _AMBA_RTSL_I2C_H_ */
