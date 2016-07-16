/**
 * @file src/app/connected/applib/src/applib/monitor/ApplibMonitor_SD.c
 *
 * SD card monitoring task related APIs
 *
 * History:
 *    2013/10/17 - [Martin Lai] created file
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

#include <AmbaDataType.h>
#include <AmbaKAL.h>

#include <AmbaPrintk.h>
#include <AmbaFS.h>
#include <AmbaCardManager.h>
#include <AmbaINT_Def.h>
#include <AmbaSD.h>
#include <AmbaRTSL_SD.h>
#include <AmbaSysCtrl.h>
#include <AmbaDSP.h>
#include <dcf/ApplibDcf.h>
#include <AmbaMonitor_SD.h>

//#define DEBUG_APPLIB_MONITOR_SD
#if defined(DEBUG_APPLIB_MONITOR_SD)
#define DBGMSG AmbaPrint
#define DBGMSGc(x) AmbaPrintColor(GREEN,x)
#define DBGMSGc2 AmbaPrintColor
#else
#define DBGMSG(...)
#define DBGMSGc(...)
#define DBGMSGc2(...)
#endif


#ifndef CARD_INIT_RETRY
#define CARD_INIT_RETRY             10
#endif

#define APPLIB_MONITOR_SD_STACK_SIZE  0x8000  /* this is some kind of huge ? */

static UINT8 AppLibMonitorSDStack[APPLIB_MONITOR_SD_STACK_SIZE] __attribute__((section (".bss.noinit")));

typedef struct _APPLIB_MONITOR_SD_CTRL_s_ {
    AMBA_KAL_EVENT_FLAG_t   Flag;
    int                     Running;                    /**< Runing state of thread */
    AMBA_KAL_TASK_t         Task;
} APPLIB_MONITOR_SD_CTRL_s;

static APPLIB_MONITOR_SD_CTRL_s AppLibMonitorSdCtrl;

/**
 *  @brief SD slot task
 *
 *  SD card task
 *
 *  @param [in] eid  event id
 *
 */
static void AppLibMonitorSd_SdTask(int eid)
{
    AMBA_SD_HOST *Host;
    AMBA_SD_CARD *Card;
    UINT8 Present = 0;
    int Slot = SCM_SLOT_SD0;

    DBGMSG("[Applib - SD Card Monitor] SdTask");
    Host = AmbaSD_GetHost(SD_HOST_0);
    Card = &Host->Card;

    K_ASSERT(Host != NULL);
    K_ASSERT(Card != NULL);

    Present = AmbaSD_CardINSlot(Host, Card);

    if (Card->Present != Present)
        Card->Present  = Present;
    else
        return;

    if (Card->Present) {
        eid = SCM_CARD_INSERTED;
        DBGMSG("[Applib - SD Card Monitor] SdTask - Pre AmbaSD_InitCard");
        if (0 > AmbaSD_InitCard(Host, Card, CARD_INIT_RETRY, 0)) {
            AmbaPrint("[Applib - SD Card Monitor] AmbaSD_InitCard - SD_CARD fail");
            Card->Format = FS_FAT_ERROR;
        } else {
            AmbaPrint("[Applib - SD Card Monitor] AmbaSD_InitCard - SD_CARD OK");
            AmbaFS_SdPrf2CheckMedia(SD_HOST_0);
        }
    } else {
        AmbaPrint("[Applib - SD Card Monitor] AmbaSD_InitCard - SD_CARD Ejected.");
        eid = SCM_CARD_EJECTED;
        AmbaRTSL_SDDeconfigCard(Host, Card);
        AmbaFS_SdPrf2CheckMedia(SD_HOST_0);
    }

    AmbaSCM_DispatchEvent(Slot, eid);
}

/**
 *  @brief SD2 slot task
 *
 *  SD2 card slot task
 *
 *  @param [in] eid event id
 *
 */
static void AppLibMonitorSd_Sd2Task(int eid)
{
    AMBA_SD_HOST *Host;
    AMBA_SD_CARD *Card;
    UINT8 Present = 0;
    int Slot = SCM_SLOT_SD1;

    DBGMSG("[Applib - SD Card Monitor] Sd2Task");

    Host = AmbaSD_GetHost(SD_HOST_1);
    Card = &Host->Card;

    K_ASSERT(Host != NULL);
    K_ASSERT(Card != NULL);

    Present = AmbaSD_CardINSlot(Host, Card);

    if (Card->Present != Present)
        Card->Present = Present;
    else
        return;

    if (Card->Present) {
        eid = SCM_CARD_INSERTED;

        if (0 > AmbaSD_InitCard(Host, Card, CARD_INIT_RETRY, 0)) {
            AmbaPrint("[Applib - SD Card Monitor] AmbaSD_InitCard - sd2mmc_init_Card fail");
            Card->Format = FS_FAT_ERROR;
        } else {
            AmbaPrint("[Applib - SD Card Monitor] AmbaSD_InitCard - SD2_CARD OK");
            AmbaFS_SdPrf2CheckMedia(SD_HOST_1);
        }

    } else {
        AmbaPrint("[Applib - SD Card Monitor] AmbaSD_InitCard - SD_CARD2 Ejected.");
        eid = SCM_CARD_EJECTED;
        AmbaRTSL_SDDeconfigCard(Host, Card);
        AmbaFS_SdPrf2CheckMedia(SD_HOST_1);
    }

    AmbaSCM_DispatchEvent(Slot, eid);
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
static void AppLibMonitorSd_TaskEntry(UINT32 input)
{
    UINT32 Data = 0;
    int Slot = 0, Eid = 0;

    while (AppLibMonitorSdCtrl.Running) {
        /* Wait for event from data queue */
        AmbaKAL_EventFlagTake(&AppLibMonitorSdCtrl.Flag,
                              (SCM_CARD_INSERTED | SCM_CARD_EJECTED),
                              AMBA_KAL_OR, &Data, AMBA_KAL_WAIT_FOREVER);
        Slot = Data >> 16;

        DBGMSG("[Applib - SD Card Monitor] TaskEntry Card Monitor is working");

        /* FIXME: For temporary debounce. */
        AmbaKAL_TaskSleep(50);

        if (Slot == SCM_SLOT_SD0)
            AppLibMonitorSd_SdTask(Eid);
        if (Slot == SCM_SLOT_SD1)
            AppLibMonitorSd_Sd2Task(Eid);

        AmbaKAL_EventFlagClear(&AppLibMonitorSdCtrl.Flag, Data);
    }
}

/**
 *  @brief Dispatch the SD card insert event.
 *
 *  Dispatch the SD card insert event.
 *
 *  @param [in] id slot id
 *
 */
void AppLibMonitorSd_InsertDispatch(UINT32 id)
{
    int Slot = (id == SD_HOST_0) ? SCM_SLOT_SD0 :
               (id == SD_HOST_1) ? SCM_SLOT_SD1 : -1;

    if (Slot > 0) {
        UINT32 Flags = (Slot << 16) | SCM_CARD_INSERTED;
        AmbaKAL_EventFlagGive(&AppLibMonitorSdCtrl.Flag, Flags);
    }
}

/**
 *  @brief Dispatch the SD card insert event.
 *
 *  Dispatch the SD card insert event.
 *
 *  @param [in] id slot id
 *
 */
void AppLibMonitorSd_EjectDispatch(UINT32 id)
{
    int Slot = (id == SD_HOST_0) ? SCM_SLOT_SD0 :
               (id == SD_HOST_1) ? SCM_SLOT_SD1 : -1;

    if (Slot > 0) {
        UINT32 Flags = (Slot << 16) | SCM_CARD_EJECTED;
        AmbaKAL_EventFlagGive(&AppLibMonitorSdCtrl.Flag, Flags);
    }
}

/**
 *  @brief AppLibMonitorSd_PresentCheck
 *
 *  AppLibMonitorSd_PresentCheck
 *
 */
static void AppLibMonitorSd_PresentCheck(void)
{
    UINT32  Id;
    AMBA_SD_HOST *Host;

    for (Id = 0; Id < MAX_SD_HOST; Id++) {
        Host = AmbaSD_GetHost(Id);
        if (AmbaSD_CardINSlot(Host, &Host->Card))
            AppLibMonitorSd_InsertDispatch(Id);
    }
}

/**
 *  @brief Initialize the SD CARD MONITOR task.
 *
 *  Initialize the SD CARD MONITOR task.
 *
 *  @param [in] taskPriority Task priority
 *
 *  @return >=0 success, <0 failure
 */
int AppLibMonitorSd_Init(UINT32 taskPriority)
{
    /* [20141211 JH] moved AmbaMonitor_SD.c to libcomsvc_misc.a */
    AmbaMonitor_SDInit(taskPriority, 0);
    return OK;

    #if 0
    void AmbaSD_HookCallBack(void);

    DBGMSG("[Applib - SD Card Monitor] Initialize SD MONITOR ");

    AmbaSD_SigInsertCallBack = AppLibMonitorSd_InsertDispatch;
    AmbaSD_SigEjectCallBack  = AppLibMonitorSd_EjectDispatch;
    AmbaSD_HookCallBack();

    memset(&AppLibMonitorSdCtrl, 0x0, sizeof(AppLibMonitorSdCtrl));

    if (OK != AmbaKAL_EventFlagCreate(&AppLibMonitorSdCtrl.Flag)) {
        return NG;
    }

    AppLibMonitorSd_PresentCheck();

    AppLibMonitorSdCtrl.Running = 1;

    return AmbaKAL_TaskCreate(&AppLibMonitorSdCtrl.Task,      /* pTask */
                              "AppLibMonitor_SD",             /* pTaskName */
                              taskPriority,                     /* taskPriority */
                              AppLibMonitorSd_TaskEntry,          /* void (*EntryFunction)(UINT32) */
                              0x0,                          /* EntryArg */
                              AppLibMonitorSDStack,           /* pStackBase */
                              APPLIB_MONITOR_SD_STACK_SIZE,   /* StackByteSize */
                              AMBA_KAL_AUTO_START);         /* AutoStart */
    #endif
}
