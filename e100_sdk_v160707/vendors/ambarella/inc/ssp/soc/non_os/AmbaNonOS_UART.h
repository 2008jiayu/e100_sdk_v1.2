/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaNonOS_UART.h
 *
 *  @Copyright      :: Copyright (C) 2012 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: UART APIs w/o OS.
\*-------------------------------------------------------------------------------------------------------------------*/

#ifndef _AMBA_NONOS_UART_H_
#define _AMBA_NONOS_UART_H_

#define AMBA_PRINTF_TARGET_CHAMELEON        0
#define AMBA_PRINTF_TARGET_UART             1

void    AmbaNonOS_UartInit(void);
void    AmbaNonOS_UartPutChar(char Char);
char    AmbaNonOS_UartGetChar(void);
int     AmbaNonOS_UartPutString(const char *pString, UINT32 Length);
int     AmbaNonOS_UartGetString(char *pString, int MaxLength);
int     AmbaNonOS_UartPrintf(const char *pFormatString, ...);
int     AmbaNonOS_UartWaitEscape(UINT32 Time);

#endif /* _AMBA_NONOS_UART_H_ */

