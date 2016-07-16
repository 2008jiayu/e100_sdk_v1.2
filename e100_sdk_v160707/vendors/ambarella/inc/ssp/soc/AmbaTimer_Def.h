/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaTimer_Def.h
 *
 *  @Copyright      :: Copyright (C) 2013 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Common Definitions & Constants for A12 Timer APIs
\*-------------------------------------------------------------------------------------------------------------------*/

#ifndef _AMBA_TIMER_DEF_H_
#define _AMBA_TIMER_DEF_H_

/*-----------------------------------------------------------------------------------------------*\
 * A12 Interval Timers
\*-----------------------------------------------------------------------------------------------*/
typedef enum _AMBA_TIMER_ID_e_ {
    AMBA_TIMER0 = 0,
    AMBA_TIMER1,
    AMBA_TIMER2,
    AMBA_TIMER3,
    AMBA_TIMER4,
    AMBA_TIMER5,
    AMBA_TIMER6,
    AMBA_TIMER7,

    AMBA_NUM_TIMER,
    AMBA_TIMER_SYS_TICK = AMBA_NUM_TIMER - 1,       /* Reserved the Last Timer for OS tick */
    AMBA_TIMER_SYS_PROFILE = AMBA_NUM_TIMER - 5,    /* Reserved the timer for System Profiling */
} AMBA_TIMER_ID_e;

typedef void (*AMBA_TIMER_ISR_f)(AMBA_TIMER_ID_e, UINT32);

/*-----------------------------------------------------------------------------------------------*\
 * Interval Timer Configuration Structure
\*-----------------------------------------------------------------------------------------------*/
typedef struct _AMBA_TIMER_CONFIG_s_ {
    UINT32  CounterVal;     /* The nearest expiry time in microseconds. Set it zero to stop timer immediately. */
    UINT32  IntervalVal;    /* Reload counter value when timer expires. Set it zero to stop timer on expiration. */
    AMBA_TIMER_ISR_f ExpirationFunc;    /* function to be called on timer expiration (ISR) */
    UINT32  ExpirationArg;              /* Optional argument of the timer expiration function */
} AMBA_TIMER_CONFIG_s;

#endif /* _AMBA_TIMER_DEF_H_ */
