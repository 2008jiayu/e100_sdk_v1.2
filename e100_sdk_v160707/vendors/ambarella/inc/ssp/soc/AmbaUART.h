/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaUART.h
 *
 *  @Copyright      :: Copyright (C) 2012 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Definitions & Constants for UART Middleware APIs
\*-------------------------------------------------------------------------------------------------------------------*/

#ifndef _AMBA_UART_H_
#define _AMBA_UART_H_

#include "AmbaUART_Def.h"

/*-----------------------------------------------------------------------------------------------*\
 * Defined in AmbaUART.c
\*-----------------------------------------------------------------------------------------------*/
int AmbaUART_Config(AMBA_UART_CHANNEL_e UartChanNo, AMBA_UART_CONFIG_s *pUartConfig);

int AmbaUART_Write(AMBA_UART_CHANNEL_e UartChanNo, int TxDataSize, UINT8 *pTxDataBuf, UINT32 TimeOut);
int AmbaUART_Read(AMBA_UART_CHANNEL_e UartChanNo, int RxDataSize, UINT8 *pRxDataBuf, UINT32 TimeOut);
int AmbaUART_ClearRxRingBuf(AMBA_UART_CHANNEL_e UartChanNo);

#endif /* _AMBA_UART_H_ */
