/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaRootTask.c
 *
 *  @Copyright      :: Copyright (C) 2012 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Root software task related APIs
\*-------------------------------------------------------------------------------------------------------------------*/

#include <stdio.h>
#include <string.h>

#include "AmbaDataType.h"
#include "AmbaKAL.h"
#include "AmbaSysCtrl.h"

AMBA_KAL_TASK_t _AmbaRootTask;
UINT8 _AmbaRootTaskStack[4096]__attribute__((section(".bss.noinit")))__attribute__((used));
UINT32 _AmbaRootTaskStackSize = sizeof(_AmbaRootTaskStack);

AMBA_KAL_EVENT_FLAG_t AmbaSysStatusFlag;    /* System status flags */

static VOID_FUNCTION _IdleTaskCallbackFunction = NULL;  /* pointer to the Callback Function */
static UINT16 _IdleTaskIntervalTime = 500;              /* Interval Time in ms */

/*---------------------------------------------------------------------------*\
 * Defined in AmbaSysInit.c
\*---------------------------------------------------------------------------*/
void AmbaSysInitPostOS(void);

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaRootTaskEntry
 *
 *  @Description:: Root Task entry
 *
 *  @Input      ::
 *      EntryArg: Task entry Argument
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
void AmbaRootTaskEntry(UINT32 EntryArg)
{
    /* create the System Status Flags */
    if (AmbaKAL_EventFlagCreate(&AmbaSysStatusFlag) != OK)
        return;     /* something wrong ! */

    AmbaSysInitPostOS();    /* Post-OS Initializations */
    while (1) {     /* loop forever */
        AmbaWaitSysStatus(AMBA_SYS_STATUS_IDLE_TIME_OUT_FLAG,
                          AMBA_KAL_OR_CLEAR, _IdleTaskIntervalTime);

        if (_IdleTaskCallbackFunction != NULL)
            _IdleTaskCallbackFunction();    /* invoke User provided idle function */
    }
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaIdleTaskCallbackFunctionRegister
 *
 *  @Description:: Register Callback Function for the Idle Task
 *
 *  @Input      ::
 *      pCallbackFunction: pointer to the Callback Function
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
void AmbaIdleTaskCallbackFunctionRegister(VOID_FUNCTION pCallbackFunction)
{
    _IdleTaskCallbackFunction = pCallbackFunction;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaIdleTaskIntervalTimeSet
 *
 *  @Description:: Set the Interval time the Idle Task to call user defined Callback function
 *
 *  @Input      ::
 *      IntervalTime: Interval time in ms
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
void AmbaIdleTaskIntervalTimeSet(UINT16 IntervalTime)
{
    _IdleTaskIntervalTime = IntervalTime;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaSetSysStatus
 *
 *  @Description:: Set system status flags
 *
 *  @Input      ::
 *      SysStatusFlag: System status flags
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
int AmbaSetSysStatus(UINT32 SysStatusFlag)
{
    return AmbaKAL_EventFlagGive(&AmbaSysStatusFlag, SysStatusFlag);
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaClearSysStatus
 *
 *  @Description:: Clear system status flags
 *
 *  @Input      ::
 *      SysStatusFlag: System status flags
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
int AmbaClearSysStatus(UINT32 SysStatusFlag)
{
    return AmbaKAL_EventFlagClear(&AmbaSysStatusFlag, SysStatusFlag);
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaCheckSysStatus
 *
 *  @Description:: Check system status flags
 *
 *  @Input      ::
 *      SysStatusFlag: System status flags
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *       UINT32 : Current system status flags
\*-----------------------------------------------------------------------------------------------*/
UINT32 AmbaCheckSysStatus(UINT32 SysStatusFlag)
{
    UINT32 ActualFlags = 0;

    if (AmbaKAL_EventFlagTake(&AmbaSysStatusFlag, SysStatusFlag, AMBA_KAL_OR,
                              &ActualFlags, AMBA_KAL_NO_WAIT) == OK) {
        ActualFlags &= SysStatusFlag;
    }

    return ActualFlags;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaWaitSysStatus
 *
 *  @Description:: Check system status flags
 *
 *  @Input      ::
 *      SysStatusFlag: System status flags
 *      Options:       Options for this operation (AND, OR, CLEAR, etc.)
 *      Timeout:       Wait option (timer-ticks)
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *       UINT32 : Current system status flags
\*-----------------------------------------------------------------------------------------------*/
UINT32 AmbaWaitSysStatus(UINT32 SysStatusFlag, UINT32 Options, UINT32 Timeout)
{
    UINT32 ActualFlags = 0;

    if (AmbaKAL_EventFlagTake(&AmbaSysStatusFlag, SysStatusFlag, Options,
                              &ActualFlags, Timeout) == OK) {
        ActualFlags &= SysStatusFlag;
    }

    return ActualFlags;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaResetAllSysStatus
 *
 *  @Description:: Reset All system status flags
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
int AmbaResetAllSysStatus(void)
{
    return AmbaKAL_EventFlagClear(&AmbaSysStatusFlag, 0xffffffff);
}
