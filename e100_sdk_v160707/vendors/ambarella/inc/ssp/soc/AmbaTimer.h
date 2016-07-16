/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaTimer.h
 *
 *  @Copyright      :: Copyright (C) 2013 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Definitions & Constants for Timer Control APIs
\*-------------------------------------------------------------------------------------------------------------------*/

#ifndef _AMBA_TIMER_H_
#define _AMBA_TIMER_H_

#include "AmbaTimer_Def.h"

/*-----------------------------------------------------------------------------------------------*\
 * Defined in AmbaTimer.c
\*-----------------------------------------------------------------------------------------------*/
int AmbaTimer_Init(void);
int AmbaTimer_Set(AMBA_TIMER_CONFIG_s *pTimerConfig, UINT32 TimeOut);
int AmbaTimer_Get(int TimerIdx, AMBA_TIMER_CONFIG_s *pTimerConfig);
int AmbaTimer_Stop(int TimerIdx);
int AmbaTimer_Wait(int TimerIdx);

UINT32 AmbaTimer_GetSysTickCount(void);

#endif /* _AMBA_TIMER_H_ */
