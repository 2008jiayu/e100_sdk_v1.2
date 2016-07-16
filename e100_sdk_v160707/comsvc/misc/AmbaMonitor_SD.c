/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaMonitor_SD.c
 *
 *  @Copyright      :: Copyright (C) 2012 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Bare SD Card Monitor.
 *
 *  @History        ::
 *      Date        Name        Comments
 *      09/17/2012  Evan Chen   Created
 *
\*-------------------------------------------------------------------------------------------------------------------*/

#include "AmbaDataType.h"
#include "AmbaKAL.h"

#include "AmbaPrintk.h"
#include "AmbaFS.h"
#include "AmbaCardManager.h"
#include "AmbaINT_Def.h"
#include "AmbaSD.h"

//#include "AmbaDCF.h"
//#include "AmbaSVC.h"
#include "AmbaSysCtrl.h"
#include "AmbaDSP.h"

#ifdef ENABLE_DBG_SD_MONITOR
#define DBGMSG AmbaPrint
#else
#define DBGMSG(...)
#endif

#ifndef CARD_INIT_RETRY
#define CARD_INIT_RETRY             10
#endif

#define AMBA_MONITOR_SD_STACK_SIZE  0x8000  /* this is some kind of huge ? */

extern void AmbaRTSL_SDDeconfigCard(AMBA_SD_HOST * pHost, AMBA_SD_CARD * pCard);
extern void AmbaFS_SdPrf2CheckMedia(int Id);

static UINT8 AmbaMonitorSDStack[AMBA_MONITOR_SD_STACK_SIZE] __attribute__((section (".bss.noinit")));

typedef struct _AMBA_MONITOR_SD_CTRL_s_ {
    AMBA_KAL_EVENT_FLAG_t   Flag;
    int                     Running;                    /**< Runing state of thread */
    AMBA_KAL_TASK_t         Task;
} AMBA_MONITOR_SD_CTRL_s;

static AMBA_MONITOR_SD_CTRL_s AmbaMonitorSdCtrl;

static void SDTask(int Eid)
{
    AMBA_SD_HOST *pHost;
    AMBA_SD_CARD *pCard;
    UINT8 Present;
    int Slot = SCM_SLOT_SD0;
    int SD_Instance = 0;

    pHost = AmbaSD_GetHost(SD_HOST_0);
    pCard = &pHost->Card;

    K_ASSERT(pHost != NULL);
    K_ASSERT(pCard != NULL);

    Present = AmbaSD_CardINSlot(pHost, pCard);

    if (pCard->Present != Present)
        pCard->Present  = Present;
    else
        return;

    if (pCard->Present) {
        AmbaPrint("SCM_SLOT_SD inserted");
        Eid = SCM_CARD_INSERTED;
        if (0 > AmbaSD_InitCard(pHost, pCard, CARD_INIT_RETRY, 0)) {
            AmbaPrint("AmbaSD_InitCard - SD_CARD fail");
            pCard->Format[SD_Instance] = FS_FAT_ERROR;
        } else {
            AmbaPrint("AmbaSD_InitCard - SD_CARD OK");
            AmbaFS_SdPrf2CheckMedia(SD_HOST_0);
        }
    } else {
        AmbaPrint("SCM_SLOT_SD0 ejected");
        Eid = SCM_CARD_EJECTED;
        AmbaRTSL_SDDeconfigCard(pHost, pCard);
        AmbaFS_FreeFileInstance(Slot);
        AmbaFS_SdPrf2CheckMedia(SD_HOST_0);
        /* Reset SDPhy Delay Ctrl */
        AmbaSD_DelayCtrlAdjustPhy(0, 0, 1, 0);
    }

    AmbaSCM_DispatchEvent(Slot, Eid);
}

static void SD2Task(int Eid)
{
    AMBA_SD_HOST *pHost;
    AMBA_SD_CARD *pCard;
    UINT8 Present;
    int Slot = SCM_SLOT_SD1;
    int SD_Instance = 0;

    pHost = AmbaSD_GetHost(SD_HOST_1);
    pCard = &pHost->Card;

    K_ASSERT(pHost != NULL);
    K_ASSERT(pCard != NULL);

    Present = AmbaSD_CardINSlot(pHost, pCard);

    if (pCard->Present != Present)
        pCard->Present = Present;
    else
        return;

    if (pCard->Present) {
        Eid = SCM_CARD_INSERTED;

        if (0 > AmbaSD_InitCard(pHost, pCard, CARD_INIT_RETRY, 0)) {
            AmbaPrint("sd2mmc_init_Card fail");
            pCard->Format[SD_Instance] = FS_FAT_ERROR;
        } else {
            AmbaFS_SdPrf2CheckMedia(SD_HOST_1);
        }

    } else {
        Eid = SCM_CARD_EJECTED;
        AmbaRTSL_SDDeconfigCard(pHost, pCard);
        AmbaFS_FreeFileInstance(Slot);
        AmbaFS_SdPrf2CheckMedia(SD_HOST_1);
    }

    AmbaSCM_DispatchEvent(Slot, Eid);
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: Monitor_SDTaskEntry
 *
 *  @Description:: The task function.
 *
 *  @Input      ::
 *
 *
 *  @Output     :: none
 *
 *  @Return     :: none
 *
\*-----------------------------------------------------------------------------------------------*/
static void Monitor_SDTaskEntry(UINT32 Input)
{
    UINT32 Data = 0;
    int Slot, Eid = 0;

    while (AmbaMonitorSdCtrl.Running) {
        /* Wait for event from data queue */
        AmbaKAL_EventFlagTake(&AmbaMonitorSdCtrl.Flag,
                              (SCM_CARD_INSERTED | SCM_CARD_EJECTED),
                              AMBA_KAL_OR, &Data, AMBA_KAL_WAIT_FOREVER);
        Slot = Data >> 16;

        DBGMSG("Card Monitor is working");

        /* FIXME: For temporary debounce. */
        AmbaKAL_TaskSleep(50);

        if (Slot == SCM_SLOT_SD0)
            SDTask(Eid);
        if (Slot == SCM_SLOT_SD1)
            SD2Task(Eid);

        AmbaKAL_EventFlagClear(&AmbaMonitorSdCtrl.Flag, Data);
    }
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaMonitor_SDInsertDispatch
 *
 *  @Description:: Dispatch the NAND insert event.
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static void AmbaMonitor_SDInsertDispatch(UINT32 Id)
{
    int Slot = (Id == SD_HOST_0) ? SCM_SLOT_SD0 :
               (Id == SD_HOST_1) ? SCM_SLOT_SD1 : -1;

    if (Slot > 0) {
        UINT32 Flags = (Slot << 16) | SCM_CARD_INSERTED;
        AmbaKAL_EventFlagGive(&AmbaMonitorSdCtrl.Flag, Flags);
    }
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaMonitor_SDEjectDispatch
 *
 *  @Description:: Dispatch the NAND insert event.
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static void AmbaMonitor_SDEjectDispatch(UINT32 Id)
{
    int Slot = (Id == SD_HOST_0) ? SCM_SLOT_SD0 :
               (Id == SD_HOST_1) ? SCM_SLOT_SD1 : -1;

    if (Slot > 0) {
        UINT32 Flags = (Slot << 16) | SCM_CARD_EJECTED;
        AmbaKAL_EventFlagGive(&AmbaMonitorSdCtrl.Flag, Flags);
    }
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: SDPresentCheck
 *
 *  @Description::
 *
 *  @Input      ::
 *
 *
 *  @Output     :: none
 *
 *  @Return     :: none
 *
\*-----------------------------------------------------------------------------------------------*/
static void SDPresentCheck(void)
{
    UINT32  Id;
    AMBA_SD_HOST *pHost;

    for (Id = 0; Id < MAX_SD_HOST; Id++) {
        pHost = AmbaSD_GetHost(Id);
        if (AmbaSD_CardINSlot(pHost, &pHost->Card))
            AmbaMonitor_SDInsertDispatch(Id);
    }
}

/**
 * SD monitor init function
 *
 * @param Priority [in] - monitor task priority
 * @param CoreSelection [in] - core option for smp, Bit[0] = 1 means core #0 is selected
 * @return
 */
int AmbaMonitor_SDInit(UINT32 Priority, UINT32 CoreSelection)
{
    void AmbaSD_HookCallBack(void);
    int Rval = 0;

    DBGMSG("Initialize SD MONITOR ");

    AmbaSD_SigInsertCallBack = AmbaMonitor_SDInsertDispatch;
    AmbaSD_SigEjectCallBack  = AmbaMonitor_SDEjectDispatch;
    AmbaSD_HookCallBack();

    memset(&AmbaMonitorSdCtrl, 0x0, sizeof(AmbaMonitorSdCtrl));

    if (OK != AmbaKAL_EventFlagCreate(&AmbaMonitorSdCtrl.Flag)) {
        return NG;
    }

    SDPresentCheck();

    AmbaMonitorSdCtrl.Running = 1;

    Rval = AmbaKAL_TaskCreate(&AmbaMonitorSdCtrl.Task,      /* pTask */
                              "AmbaMonitor_SD",             /* pTaskName */
                              Priority,                     /* Priority */
                              Monitor_SDTaskEntry,          /* void (*EntryFunction)(UINT32) */
                              0x0,                          /* EntryArg */
                              AmbaMonitorSDStack,           /* pStackBase */
                              AMBA_MONITOR_SD_STACK_SIZE,   /* StackByteSize */
                              AMBA_KAL_AUTO_START);         /* AutoStart */

#if 0
    AmbaKAL_TaskSmpCoreExclusionSet(&AmbaMonitorSdCtrl.Task, CoreExclusiveBitMap);
#endif

    return Rval;
}
