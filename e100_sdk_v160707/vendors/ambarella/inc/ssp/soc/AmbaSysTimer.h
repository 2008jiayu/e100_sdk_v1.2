/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaSysTimer.h
 *
 *  @Copyright      :: Copyright (C) 2013 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Common Definitions & Constants for System Timer APIs
\*-------------------------------------------------------------------------------------------------------------------*/

#ifndef _AMBA_SYS_TIMER_H_
#define _AMBA_SYS_TIMER_H_

/*-----------------------------------------------------------------------------------------------*\
 * Defined in AmbaSysTimer.c
\*-----------------------------------------------------------------------------------------------*/
int AmbaSysTimer_Init(void);
int AmbaSysTimer_Take(UINT32 TimeOut);
int AmbaSysTimer_Give(void);

int AmbaSysTimer_Start(UINT32 Frequency, UINT32 NumTicks, VOID_FUNCTION TimeOutIsrEntry);
int AmbaSysTimer_Wait(UINT32 Frequency, UINT32 NumTicks);
UINT32 AmbaSysTimer_GetTickCount(void);

#endif  /* _AMBA_SYS_TIMER_H_ */
