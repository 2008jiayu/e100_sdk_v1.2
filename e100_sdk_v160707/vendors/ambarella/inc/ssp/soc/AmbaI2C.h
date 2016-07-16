/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaI2C.h
 *
 *  @Copyright      :: Copyright (C) 2012 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Definitions & Constants for I2C Middleware APIs
\*-------------------------------------------------------------------------------------------------------------------*/

#ifndef _AMBA_I2C_H_
#define _AMBA_I2C_H_

#include "AmbaI2C_Def.h"

/*-----------------------------------------------------------------------------------------------*\
 * Defined in AmbaI2C.c
\*-----------------------------------------------------------------------------------------------*/
int AmbaI2C_Init(void);
int AmbaI2C_Write(AMBA_I2C_CHANNEL_e I2cChanNo, AMBA_I2C_SPEED_e I2cSpeed, UINT8 SlaveAddr,
                  int TxDataSize, UINT8 *pTxDataBuf, UINT32 TimeOut);
int AmbaI2C_Read(AMBA_I2C_CHANNEL_e I2cChanNo, AMBA_I2C_SPEED_e I2cSpeed, UINT8 SlaveAddr,
                 int RxDataSize, UINT8 *pRxDataBuf, UINT32 TimeOut);
int AmbaI2C_ReadAfterWrite(AMBA_I2C_CHANNEL_e I2cChanNo, AMBA_I2C_SPEED_e I2cSpeed,
                           int TxDataSize, UINT16 *pTxDataBuf,
                           int RxDataSize, UINT8 *pRxDataBuf, UINT32 TimeOut);

int AmbaI2C_ReadVarLength(AMBA_I2C_CHANNEL_e I2cChanNo, AMBA_I2C_SPEED_e I2cSpeed, UINT8 SlaveAddr,
                          int MaxDataSize, UINT8 *pRxDataBuf, UINT32 TimeOut);

#endif /* _AMBA_I2C_H_ */
