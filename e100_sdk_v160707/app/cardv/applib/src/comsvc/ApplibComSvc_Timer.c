/**
 * @file src/app/connected/applib/src/comsvc/ApplibComSvc_Timer.c
 *
 *  Implementation of Timers
 *
 * History:
 *    2013/07/10 - [Martin Lai] created file
 *
 *
 * Copyright (c) 2015 Ambarella, Inc.
 *
 * This file and its contents (¡°Software¡±) are protected by intellectual property rights
 * including, without limitation, U.S. and/or foreign copyrights.  This Software is also the
 * confidential and proprietary information of Ambarella, Inc. and its licensors.  You may
 * not use, reproduce, disclose, distribute, modify, or otherwise prepare derivative
 * works of this Software or any portion thereof except pursuant to a signed license
 * agreement or nondisclosure agreement with Ambarella, Inc. or its authorized
 * affiliates.	In the absence of such an agreement, you agree to promptly notify and
 * return this Software to Ambarella, Inc.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF NON-
 * INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL AMBARELLA, INC. OR ITS AFFILIATES BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; COMPUTER FAILURE OR
 * MALFUNCTION; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <applib.h>

//#define DEBUG_APPLIB_TIMER
#if defined(DEBUG_APPLIB_TIMER)
#define DBGMSG AmbaPrint
#define DBGMSGc(x) AmbaPrintColor(GREEN,x)
#define DBGMSGc2 AmbaPrintColor
#else
#define DBGMSG(...)
#define DBGMSGc(...)
#define DBGMSGc2(...)
#endif

/*************************************************************************
 * Timer structure definitons
 ************************************************************************/
/* Timer semaphore */
AMBA_KAL_SEM_t AppTimerSem = {0};

/* Timer structure */
typedef struct _APPLIB_TIMER_s_ {
    int TimerId;   /* Cyclic handler ID for the timer */
    UINT32 Valid;  /* 1:Valid timer, 0:free entry */
    int HandlerNum;
    AMBA_KAL_TIMER_t AppTimer;
    AppTimer_Handler Handler[MAX_TIMER_HANDLER];
} APPLIB_TIMER_s;

/* Timer period table */
static int timer_period_table[TIMER_NUM] = {
    10000,      /**< TIMER_CHECK 10 seconds */
    1000,       /**< TIMER_1HZ 1 second */
    500,        /**< TIMER_2HZ 500 ms */
    250,        /**< TIMER_4HZ 250 ms*/
    100,        /**< TIMER_10HZ 100 ms*/
    50,         /**< TIMER_20HZ 50ms */
    5000,       /**< TIMER_5S 5 seconds */
    30000       /* *<TIMER_30S 30 seconds*/
};
/* Timer array */
static APPLIB_TIMER_s timers[TIMER_NUM] = {0};

/*************************************************************************
 * Timer APIs
 ************************************************************************/

/**
 *  @brief Timer handler
 *
 *  Timer handler
 *
 *  @param [in] exinf information
 *
 *  @return >=0 success, <0 failure
 */
static void AppLibComSvcTimer_TimerHandler(UINT32 exinf)
{
    UINT32 Message = 0;
    Message = HMSG_COMSVC_MODULE_TIMER(exinf); /* exinf = timer id*/
    DBGMSG("[Applib - Timer] <AppLibComSvcTimer_TimerHandler> send Message: 0x%X / exinf = %d", Message, exinf);
    AppLibComSvcHcmgr_SendMsgNoWait(Message, exinf, 0);
}

/**
 *  @brief Cancel a timer
 *
 *  Cancel a timer
 *
 *  @param [in] tid timer id
 *
 *  @return >=0 success, <0 failure
 */
static int AppLibComSvcTimer_ClearTimer(int tid)
{
    int ReturnValue = 0;

    DBGMSGc2(BLUE, "[Applib - Timer] <%s> start", __FUNCTION__);

    /* Sanity check */
    if ((tid < 0) || (tid >= TIMER_NUM)) {
        return -1;
    }

    AmbaKAL_SemTake(&AppTimerSem, AMBA_KAL_WAIT_FOREVER);

    /* Stop and delete the timer */
    if (timers[tid].Valid) {
        ReturnValue = AmbaKAL_TimerStop(&timers[tid].AppTimer);
        K_ASSERT(ReturnValue == OK);

        ReturnValue = AmbaKAL_TimerDelete(&timers[tid].AppTimer);
        K_ASSERT(ReturnValue == OK);

        memset(&timers[tid], 0, sizeof(APPLIB_TIMER_s));
    }

    AmbaKAL_SemGive(&AppTimerSem);

    DBGMSGc2(BLUE, "[Applib - Timer] <%s> end",  __FUNCTION__);

    return ReturnValue;
}

/**
 *  @brief Config a timer
 *
 *  Config a timer
 *
 *  @param [in] tid timer id
 *
 *  @return >=0 success, <0 failure
 */
static int AppLibComSvcTimer_SetTimer(int tid)
{
    int ReturnValue = 0;

    DBGMSGc2(BLUE, "[Applib - Timer] <%s> start", __FUNCTION__);

    /* Sanity check */
    if ((tid < 0) || (tid >= TIMER_NUM)) {
        return -1;
    }

    AmbaKAL_SemTake(&AppTimerSem, AMBA_KAL_WAIT_FOREVER);
    /* If this is an old timer set before, delete it first */
    if (timers[tid].Valid) {
        ReturnValue = AmbaKAL_TimerStop(&timers[tid].AppTimer);
        K_ASSERT(ReturnValue == OK);

        ReturnValue = AmbaKAL_TimerDelete(&timers[tid].AppTimer);
        K_ASSERT(ReturnValue == OK);
    }

    /* Create a timer hander */
    timers[tid].TimerId = AmbaKAL_TimerCreate(&timers[tid].AppTimer, AMBA_KAL_AUTO_START,
                                &AppLibComSvcTimer_TimerHandler, tid,
                                timer_period_table[tid], timer_period_table[tid]);
    if (timers[tid].TimerId < 0) {
        DBGMSGc2(RED, "[Applib - Timer] <%s> Failure", __FUNCTION__);
        K_ASSERT(timers[tid].TimerId >= 0);
        ReturnValue = -1;
    } else {
        timers[tid].Valid = 1;
        ReturnValue = 0;
    }
    AmbaKAL_SemGive(&AppTimerSem);

    DBGMSGc2(BLUE, "[Applib - Timer] <%s> end",  __FUNCTION__);

    return ReturnValue;
}

/**
 *  @brief The initialization of timer library.
 *
 *  The initialization of timer library.
 *
 *  @return >=0 success, <0 failure
 */
int AppLibComSvcTimer_Init(void)
{
    DBGMSGc2(BLUE, "[Applib - Timer] <%s> start",  __FUNCTION__);

    /* Initialize timers obj */
    AmbaKAL_SemCreate(&AppTimerSem, 1);

    // TIMER_CHECK is used for auto-poweroff feature
    // Should not be unregistered in any cases
    memset(timers, 0, sizeof(APPLIB_TIMER_s)*TIMER_NUM);

    DBGMSGc2(BLUE, "[Applib - Timer] <%s> end",  __FUNCTION__);

    return 0;
}

/**
 *  @brief This API unregister all timers, except the auto power off timer.
 *
 *  This API unregister all timers, except the auto power off timer.
 *
 *  @return >=0 success, <0 failure
 */
int AppLibComSvcTimer_UnregisterAll(void)
{
    int i = 0, j = 0;

    DBGMSGc2(BLUE, "[Applib - Timer] <%s> start",  __FUNCTION__);

    /* TIMER_CHECK = 0 is the auto-poweroff timer */
    for (i=1; i<TIMER_NUM; i++) {
        for (j=0; j<MAX_TIMER_HANDLER; j++) {
            timers[i].Handler[j] = NULL;
        }
        timers[i].HandlerNum = 0;
        AppLibComSvcTimer_ClearTimer(i);
    }

    DBGMSGc2(BLUE, "[Applib - Timer] <%s> end",  __FUNCTION__);

    return 0;
}

/**
 *  @brief Register a timer
 *
 *  Register a timer
 *
 *  @param [in] tid Timer id
 *  @param [in] handler Timer handler
 *
 *  @return >=0 success, <0 failure
 */
int AppLibComSvcTimer_Register(int tid, AppTimer_Handler handler)
{
    int i = 0, Found = 0;
    APPLIB_TIMER_s *CurrentTimer;

    DBGMSGc2(BLUE, "[Applib - Timer] <%s> start: tid = %d / Handler = 0x%X",  __FUNCTION__, tid, Handler);

    if (tid >= TIMER_NUM) {
        // no such timer
        DBGMSG("[Applib - Timer] <%s> No timer id %d",  __FUNCTION__, tid);
        return -1;
    }

    if (handler == NULL) {
        // NULL Handler
        DBGMSG("[Applib - Timer] <%s>Timer handler is NULL",  __FUNCTION__);
        return -1;
    }

    CurrentTimer = &(timers[tid]);

    Found = 0;
    for (i=0; i<MAX_TIMER_HANDLER; i++) {
        if (handler == CurrentTimer->Handler[i]) {
            // Handler already exists
            DBGMSG("[Applib - Timer] <%s>This timer handler has already been registered", __FUNCTION__);
            Found = 1;
            break;
        }
    }

    if (!Found) {
        for (i=0; i<MAX_TIMER_HANDLER; i++) {
        if (CurrentTimer->Handler[i] == NULL) {
            // Empty slot Found. Insert Handler
            CurrentTimer->Handler[i] = handler;
            CurrentTimer->HandlerNum++;
            if (!CurrentTimer->Valid) {
                DBGMSG("[Applib - Timer] <%s>Timer id %d is inValid. Create timer id %d",  __FUNCTION__, tid, tid);
                AppLibComSvcTimer_SetTimer(tid);
            }
                break;
            }
        }
    }

    DBGMSGc2(BLUE, "[Applib - Timer] < __FUNCTION__> end",  __FUNCTION__);

    return 0;
}

/**
 *  @brief Unregister a timer
 *
 *  Unregister a timer
 *
 *  @param [in] tid Timer id
 *  @param [in] handler Timer Handler
 *
 *  @return >=0 success, <0 failure
 */
int AppLibComSvcTimer_Unregister(int tid, AppTimer_Handler handler)
{
    int i = 0, Found = 0;
    APPLIB_TIMER_s *CurrentTimer;

    DBGMSGc2(BLUE, "[Applib - Timer] <%s> start: tid = %d / Handler = 0x%X",  __FUNCTION__, tid, handler);

    if (tid >= TIMER_NUM) {
        // No such timer
        DBGMSG("[Applib - Timer] <%s>No timer id %d",  __FUNCTION__, tid);
        return -1;
    }

    if (handler == NULL) {
        // NULL Handler
        DBGMSG("[Applib - Timer] <%s>Timer handler is NULL",  __FUNCTION__);
        return -1;
    }

    CurrentTimer = &(timers[tid]);

    if (CurrentTimer->HandlerNum == 0) {
        // No Handlers in timer
        DBGMSG("[Applib - Timer] <%s>No timer handler is registered",  __FUNCTION__);
        return -1;
    }

    Found = 0;
    for (i=0; i<MAX_TIMER_HANDLER; i++) {
        if (handler == CurrentTimer->Handler[i]) {
            Found = 1;
            CurrentTimer->Handler[i] = NULL;
            CurrentTimer->HandlerNum--;
            handler(TIMER_UNREGISTER);
            break;
        }
    }

    if (Found) {
        if (CurrentTimer->HandlerNum == 0) {
            // all Handlers removed in timer. clear timer
            DBGMSG("[Applib - Timer] <%s>No timer handler under timer id %d. Delete timer id %d",  __FUNCTION__, tid, tid);
            AppLibComSvcTimer_ClearTimer(tid);
        }
    }

    DBGMSGc2(BLUE, "[Applib - Timer] <%s> end",  __FUNCTION__);

    return 0;
}

/**
 *  @brief To handle the timers.
 *
 *  To handle the timers.
 *
 *  @param [in] tid timer id
 *
 *  @return >=0 success, <0 failure
 */
int AppLibComSvcTimer_Handler(int tid)
{
    int i = 0;
    APPLIB_TIMER_s *CurrentTimer;

    DBGMSGc2(BLUE, "[Applib - Timer] <%s> start: tid = %d",  __FUNCTION__, tid);

    if (tid >= TIMER_NUM) {
        // no such timer
        DBGMSG("[Applib - Timer] <%s>No timer id %d",  __FUNCTION__, tid);
        return -1;
    }

    CurrentTimer = &(timers[tid]);

    if (!CurrentTimer->Valid) {
        // timer is not Valid
        DBGMSG("[Applib - Timer] <%s>Timer id %d is invalid",  __FUNCTION__, tid);
        return -1;
    }

    if (CurrentTimer->HandlerNum == 0) {
        // no Handlers in timer
        DBGMSG("[Applib - Timer] <%s>No timer handler is registered",  __FUNCTION__);
        return -1;
    }

    for (i=0; i<MAX_TIMER_HANDLER; i++) {
        if (CurrentTimer->Handler[i] != NULL) {
            DBGMSG("[Applib - Timer] <%s> Execute handler 0x%X",  __FUNCTION__, CurrentTimer->Handler[i]);
            CurrentTimer->Handler[i](TIMER_TICK);
        }
    }

    DBGMSGc2(BLUE, "[Applib - Timer] <%s> end",  __FUNCTION__);

    return 0;
}
