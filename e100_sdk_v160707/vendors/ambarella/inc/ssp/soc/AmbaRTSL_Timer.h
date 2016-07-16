/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaRTSL_Timer.c
 *
 *  @Copyright      :: Copyright (C) 2013 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Definitions & Constants for Timer control APIs
\*-------------------------------------------------------------------------------------------------------------------*/

#ifndef _AMBA_RTSL_TIMER_H_
#define _AMBA_RTSL_TIMER_H_

#include "AmbaTimer_Def.h"

/*-----------------------------------------------------------------------------------------------*\
 * RTSL Timber Management Structure
\*-----------------------------------------------------------------------------------------------*/
typedef struct _AMBA_RTSL_TIMER_CTRL_s_ {
    int     IntID;   /* Interrupt ID */
    void    (*pISR)(int IntID); /* pointer to the group ISR */
} AMBA_RTSL_TIMER_CTRL_s;

/*-----------------------------------------------------------------------------------------------*\
 * Defined in AmbaRTSL_Timer.c
\*-----------------------------------------------------------------------------------------------*/
extern void (*_AmbaRTSL_TimerIsrCallBack)(AMBA_TIMER_ID_e TimerID);
extern UINT32 AmbaKAL_TimerTickCounter;

#define AmbaRTSL_TimerIsrRegister(pIsr)     _AmbaRTSL_TimerIsrCallBack = (pIsr)

#define AmbaRTSL_TimerGetTickCount()        AmbaKAL_TimerTickCounter
#define AmbaRTSL_TimerResetTickCounter()    AmbaKAL_TimerTickCounter = 0

int AmbaRTSL_TimerInit(void);
int AmbaRTSL_TimerSet(AMBA_TIMER_ID_e TimerID, AMBA_TIMER_CONFIG_s *pTimerConfig);
int AmbaRTSL_TimerGet(AMBA_TIMER_ID_e TimerID, AMBA_TIMER_CONFIG_s *pTimerConfig);
void AmbaRTSL_TimerHandleCoreFreqChange(void);

#endif /* _AMBA_RTSL_TIMER_H_ */
