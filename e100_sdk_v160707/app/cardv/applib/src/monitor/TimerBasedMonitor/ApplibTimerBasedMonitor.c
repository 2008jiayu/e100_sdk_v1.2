/**
 * @file src/app/connected/applib/src/applib/monitor/TimerBasedMonitor/ApplibTimerBasedMonitor.c
 *
 * SD card monitoring task related APIs
 *
 * History:
 *    2014/05/26 - [Chester Chuang] created file
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

#include <stdio.h>
#include <stdlib.h>
#include <AmbaPrintk.h>
#include <AmbaDataType.h>
#include <AmbaKAL.h>
#include <monitor/ApplibTimerMonitor.h>
#include <applib.h>

//#define DEBUG_APPLIB_TIMER_BASED_MONITOR
#if defined(DEBUG_APPLIB_TIMER_BASED_MONITOR)
#define DBGMSG AmbaPrint
#define DBGMSGc(x) AmbaPrintColor(GREEN,x)
#define DBGMSGc2 AmbaPrintColor
#else
#define DBGMSG(...)
#define DBGMSGc(...)
#define DBGMSGc2(...)
#endif

#define APP_TIMER_BASED_MONITOR_HANDLER_NUMBER 6
#define DEFAULT_MINIMUM_PERIOD 10
/**
 *  Timer based monitor related prototype
 */
typedef struct _TIMER_BASED_MONITOR_HANDLER_s_ {
    UINT32 Used:1;
    UINT32 Enable:1;
    UINT32 Reserved:30;
    APPLIB_TIMER_BASED_MONITOR_HANDLER_s *Handler;
} TIMER_BASED_MONITOR_HANDLER_s;

/**
 * variables for timer based monitor module
 */
typedef struct _APP_TIMER_BASED_MONITOR_CTRL_s_ {
    UINT8 Init:1;
    UINT8 Running:1;
    UINT8 reserved:6;
    UINT8 reserved1[3];
    UINT32 Count;
    UINT32 TimePeriod;

    TIMER_BASED_MONITOR_HANDLER_s Ctx[APP_TIMER_BASED_MONITOR_HANDLER_NUMBER];
    AMBA_KAL_TASK_t Task;
} APP_TIMER_BASED_MONITOR_CTRL_s;


static APP_TIMER_BASED_MONITOR_CTRL_s AppLibTimerBasedMonitorCtrl = {0};

static void AppLibTimerBasedMonitor_UpdateTimerPeriod(UINT32 period)
{
   int tmp;
   UINT32 x, y;

   if (0 == AppLibTimerBasedMonitorCtrl.TimePeriod)
       AppLibTimerBasedMonitorCtrl.TimePeriod = period;

   x = AppLibTimerBasedMonitorCtrl.TimePeriod;
   y = period;

   while (x%y!=0) { // find GCD
       tmp = y;
       y = x%y;
       x = tmp;
   }

   if (y != AppLibTimerBasedMonitorCtrl.TimePeriod) {
       if (y <= DEFAULT_MINIMUM_PERIOD) {
           AppLibTimerBasedMonitorCtrl.TimePeriod = DEFAULT_MINIMUM_PERIOD;
           AmbaPrint("[Applib - Timer Based Monitor] <Update period> force timer period as %dms", AppLibTimerBasedMonitorCtrl.TimePeriod);
       } else {
           AppLibTimerBasedMonitorCtrl.TimePeriod = y;
           AmbaPrint("[Applib - Timer Based Monitor] <Update period> update timer period as %dms", AppLibTimerBasedMonitorCtrl.TimePeriod);
       }
   } else {
       DBGMSG("[Applib - Timer Based Monitor] <Update period> period remains the same %dms", AppLibTimerBasedMonitorCtrl.TimePeriod);
   }

}

/**
 *  @brief The task function.
 *
 *  The task function.
 *
 *  @param [in] input
 *
 *  @return >=0 success, <0 failure
 */
static void AppLibTimerBasedMonitor_TaskEntry(UINT32 input)
{
    int i, factor;
    UINT32 StartTime, EndTime, DiffTime, DelayTime;

    AmbaPrint("AppLibTimerBasedMonitor_Task started");

    while (1) {
        UINT32 TimePeriod = AppLibTimerBasedMonitorCtrl.TimePeriod;

        StartTime = AmbaSysTimer_GetTickCount();

        for (i=0; i<APP_TIMER_BASED_MONITOR_HANDLER_NUMBER; i++) {
            if (AppLibTimerBasedMonitorCtrl.Ctx[i].Enable) {
                factor = AppLibTimerBasedMonitorCtrl.Ctx[i].Handler->Period/TimePeriod;
                if (0 == AppLibTimerBasedMonitorCtrl.Count%factor)
                    AppLibTimerBasedMonitorCtrl.Ctx[i].Handler->TimeUpCallBack();
            }
        }

        EndTime = AmbaSysTimer_GetTickCount();
        DiffTime = SYSTIMEDIFF(StartTime, EndTime);

        DelayTime = (TimePeriod >= DiffTime)?(TimePeriod-DiffTime):0;

        if (DelayTime)
            AmbaKAL_TaskSleep(DelayTime);
        else
            AmbaPrint("[Applib - Timer Based Monitor] <Task entry> spend too much time (%dms) on callback", DiffTime);

       AppLibTimerBasedMonitorCtrl.Count++;
    }
}

/**
 *  @brief Register a timer based handler
 *
 *  Register a timer based handler.
 *
 *  @param [in] handler id
 *  @param [in] handler period
 *
 *  @return >=0 success, <0 failure
 */

int AppLibTimerBasedMonitor_RegisterHandler(APPLIB_TIMER_BASED_MONITOR_HANDLER_s *hdlr)
{
    int i;

    if (0 == AppLibTimerBasedMonitorCtrl.Init) {
        DBGMSGc("[Applib - Timer Based Monitor] <Register> monitor is not init");
        return AMP_ERROR_ILLEGAL_OPERATION;
    }
    if ((hdlr == NULL) || (hdlr->Period < DEFAULT_MINIMUM_PERIOD)|| ((0 != hdlr->Period%DEFAULT_MINIMUM_PERIOD))) {
        AmbaPrint("[Applib - Timer Based Monitor] <Register> hdlr is null or period is less than %dms", DEFAULT_MINIMUM_PERIOD);
        return AMP_ERROR_INCORRECT_PARAM_VALUE_RANGE;
    }


    for (i=0;i<APP_TIMER_BASED_MONITOR_HANDLER_NUMBER;i++) {
        /* Find empty context */
        if (0 == AppLibTimerBasedMonitorCtrl.Ctx[i].Used) {
            AppLibTimerBasedMonitorCtrl.Ctx[i].Used = 1;
            AppLibTimerBasedMonitorCtrl.Ctx[i].Handler = hdlr;
            AppLibTimerBasedMonitor_UpdateTimerPeriod(AppLibTimerBasedMonitorCtrl.Ctx[i].Handler->Period);
            break;
        } else {
            DBGMSGc("[Applib - Timer Based Monitor] <Register> handler is full (%d)", APP_TIMER_BASED_MONITOR_HANDLER_NUMBER);
            return AMP_ERROR_ILLEGAL_OPERATION;
        }
    }

    return i;
}

/**
 *  @brief Unregister a timer based handler
 *
 *  Unregister a timer based handler.
 *
 *  @param [in] handler id
 *
 *  @return >=0 success, <0 failure
 */
int AppLibTimerBasedMonitor_UnregisterHandler(UINT32 id)
{
    if (id >= APP_TIMER_BASED_MONITOR_HANDLER_NUMBER) {
        DBGMSGc("[Applib - Timer Based Monitor] <Unregister> handler id is invalid");
        return AMP_ERROR_INCORRECT_PARAM_VALUE_RANGE;
    }
    if (NULL == AppLibTimerBasedMonitorCtrl.Ctx[id].Handler->TimeUpCallBack) {
        DBGMSGc("[Applib - Timer Based Monitor] <Unregister> handler id is not in use");
        return AMP_ERROR_GENERAL_ERROR;
    }

    AppLibTimerBasedMonitorCtrl.Ctx[id].Used = 0;
    AppLibTimerBasedMonitorCtrl.Ctx[id].Enable = 0;
    AppLibTimerBasedMonitorCtrl.Ctx[id].Handler->Period = 0;
    AppLibTimerBasedMonitorCtrl.Ctx[id].Handler->TimeUpCallBack = NULL;

    return AMP_OK;
}

/**
 *  @brief Enable/Disable a timer based handler
 *
 *  Enable/Disable a timer based handler.
 *
 *  @param [in] handler id
 *  @param [in] enable/disable
 *
 *  @return >=0 success, <0 failure
 */
int AppLibTimerBasedMonitor_EnableHandler(UINT32 id, UINT32 enable)
{
    int i, control, ReturnValue = AMP_OK;

    if (id >= APP_TIMER_BASED_MONITOR_HANDLER_NUMBER) {
        AmbaPrint("[Applib - Timer Based Monitor] <Enable> handler id is invalid");
        return AMP_ERROR_INCORRECT_PARAM_VALUE_RANGE;
    }
    if (NULL == AppLibTimerBasedMonitorCtrl.Ctx[id].Handler->TimeUpCallBack) {
        AmbaPrint("[Applib - Timer Based Monitor] <Enable> handler id is not in use");
        return AMP_ERROR_GENERAL_ERROR;
    }
    if (AppLibTimerBasedMonitorCtrl.Ctx[id].Enable == enable) {
        AmbaPrint("[Applib - Timer Based Monitor] <Enable> handler is already %s", enable?"enable":"disable");
        return AMP_OK;
    }

    AppLibTimerBasedMonitorCtrl.Ctx[id].Enable = enable;

    /* Enable given handler */
    if (1 == enable) {
        if (0 == AppLibTimerBasedMonitorCtrl.Running) {
            AppLibTimerBasedMonitorCtrl.Running = 1;
            /* invoke monitor init CB if exists */
            if (AppLibTimerBasedMonitorCtrl.Ctx[id].Handler->MonitorInit)
                AppLibTimerBasedMonitorCtrl.Ctx[id].Handler->MonitorInit();

            ReturnValue = AmbaKAL_TaskResume(&AppLibTimerBasedMonitorCtrl.Task);
            AmbaPrint("[Applib - Timer Based Monitor] <Enable> Task Resume ");
        }
    }

    /* Disable given handler */
    if (0 == enable) {
        control = 0;
        for (i=0; i<APP_TIMER_BASED_MONITOR_HANDLER_NUMBER; i++) {
            // If any handler is enable, do not suspend task
            if (1 == AppLibTimerBasedMonitorCtrl.Ctx[i].Enable) {
                control = 1;
                break;
            }
        }

        // all handler is disable, suspend task
        if (0 == control) {
            AppLibTimerBasedMonitorCtrl.Running = 0;
            ReturnValue = AmbaKAL_TaskSuspend(&AppLibTimerBasedMonitorCtrl.Task);
        }
    }

    return ReturnValue;
}

/**
 *  @brief Release Timer based Monitor
 *
 *  Release Timer based Monitor.
 *
 *  @return >=0 success, <0 failure
 */
int AppLibTimerBasedMonitor_Release(void)
{
    int ReturnValue = AMP_OK;

    if (0 == AppLibTimerBasedMonitorCtrl.Init) {
        AmbaPrint("[Applib - Timer Based Monitor] <Release> task is not init");
        return AMP_ERROR_GENERAL_ERROR;
    }

    /* delete task */
    ReturnValue = AmbaKAL_TaskTerminate(&AppLibTimerBasedMonitorCtrl.Task);
    ReturnValue = AmbaKAL_TaskDelete(&AppLibTimerBasedMonitorCtrl.Task);

    /* reset status */
    AppLibTimerBasedMonitorCtrl.Init = 0;

    return ReturnValue;
}

/**
 *  @brief Set handler period based on id
 *
 *  Set handler period based on id.
 *
 *  @param [in] handler id
 *  @param [in] handler period
 *
 *  @return >=0 success, <0 failure
 */
int AppLibTimerBasedMonitor_SetHandlerPeriod(UINT32 id, UINT32 period) {

    int ReturnValue = AMP_OK;
    if (id>=APP_TIMER_BASED_MONITOR_HANDLER_NUMBER) {
        AmbaPrint("[Applib - Timer Based Monitor] <Set period> handler id is invalid");
        return AMP_ERROR_INCORRECT_PARAM_VALUE_RANGE;
    }
    if (NULL == AppLibTimerBasedMonitorCtrl.Ctx[id].Handler->TimeUpCallBack) {
        AmbaPrint("[Applib - Timer Based Monitor] <Set period> handler id is not in use");
        return AMP_ERROR_GENERAL_ERROR;
    }
    if ((period < DEFAULT_MINIMUM_PERIOD) || (period%DEFAULT_MINIMUM_PERIOD!=0)) {
        AmbaPrint("[Applib - Timer Based Monitor] <Set period> hdlr is null or period is less than 10ms");
        return AMP_ERROR_INCORRECT_PARAM_VALUE_RANGE;
    }
    if (1 == AppLibTimerBasedMonitorCtrl.Ctx[id].Enable) {
        AmbaPrint("[Applib - Timer Based Monitor] <Set period> Do not run time change period");
        return AMP_ERROR_GENERAL_ERROR;
    }

    AppLibTimerBasedMonitorCtrl.Ctx[id].Handler->Period = period;

    return ReturnValue;
}

/**
 *  @brief Get handler period based on id
 *
 *  Get handler period based on id.
 *
 *  @param [in] handler id
 *
 *  @return >=0 period, <0 failure
 */
int AppLibTimerBasedMonitor_GetHandlerPeriod(UINT32 id) {

    if (id >= APP_TIMER_BASED_MONITOR_HANDLER_NUMBER) {
        AmbaPrint("[Applib - Timer Based Monitor] <Get period> handler id is invalid");
        return AMP_ERROR_INCORRECT_PARAM_VALUE_RANGE;
    }
    if (NULL == AppLibTimerBasedMonitorCtrl.Ctx[id].Handler->TimeUpCallBack) {
        AmbaPrint("[Applib - Timer Based Monitor] <Get period> handler id is not in use");
        return AMP_ERROR_GENERAL_ERROR;
    }

    return AppLibTimerBasedMonitorCtrl.Ctx[id].Handler->Period;
}

/**
 *  @brief Initialize Timer based Monitor
 *
 *  Initialize the Timer based Monitor
 *
 *  @param [in] taskPriority priority of the collection task
 *  @param [in] pStack stack for the task
 *  @param [in] stackSize size of the stack
 *
 *  @return >=0 success, <0 failure
 */
int AppLibTimerBasedMonitor_Init(UINT32 taskPriority, void *pStack, UINT32 stackSize)
{

    int ReturnValue = AMP_ERROR_GENERAL_ERROR;
    DBGMSGc2(GREEN, "[Applib - Timer Based Monitor] <Init> taskPriority = %d stack = 0x%X size = 0x%X", \
              taskPriority, pStack, stackSize);

    if (1 == AppLibTimerBasedMonitorCtrl.Init) {
        AmbaPrint("[Applib - Timer Based Monitor] <Init> task is already init");
        goto _DONE;
    }

    memset(pStack, 0x0, stackSize);
    memset(&AppLibTimerBasedMonitorCtrl, 0x0, sizeof(AppLibTimerBasedMonitorCtrl));

    /* Create the task */
    ReturnValue = AmbaKAL_TaskCreate(&AppLibTimerBasedMonitorCtrl.Task, /* pTask */
                              "AppMonitor_Timer",                       /* pTaskName */
                              taskPriority,                             /* Priority */
                              AppLibTimerBasedMonitor_TaskEntry,        /* void (*EntryFunction)(UINT32) */
                              0x0,                                      /* entryArg */
                              pStack,                                     /* pStackBase */
                              stackSize,                                /* Stack size in bytes */
                              AMBA_KAL_DO_NOT_START);                   /* Do not auto start */



    if (OK != ReturnValue) {
        AmbaPrintColor(RED,"[Applib - Storage Monitor] <Init> TaskCreate failure");
        goto _DONE;
    }

    /* Init settings */
    AppLibTimerBasedMonitorCtrl.Init = 1;
    AppLibTimerBasedMonitorCtrl.Running = 0;
    AppLibTimerBasedMonitorCtrl.Count = 0;
    AppLibTimerBasedMonitorCtrl.TimePeriod = 0;

_DONE:
    return ReturnValue;
}
