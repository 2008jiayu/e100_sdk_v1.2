/**
 * @file src/app/connected/applib/src/monitor/ApplibMonitor_Storage.c
 *
 * STORAGE monitoring task related APIs
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


#include <stdio.h>
#include <stdlib.h>
#include <AmbaDataType.h>
#include <AmbaKAL.h>
#include <applibhmi.h>
#include <applib.h>
#include <cfs/AmpCfs.h>

//#define DEBUG_APPLIB_MONITOR_STORAGE
#if defined(DEBUG_APPLIB_MONITOR_STORAGE)
#define DBGMSG AmbaPrint
#define DBGMSGc(x) AmbaPrintColor(GREEN,x)
#define DBGMSGc2 AmbaPrintColor
#else
#define DBGMSG(...)
#define DBGMSGc(...)
#define DBGMSGc2(...)
#endif

#define STORAGE_MONITOR_SIZE_LIMITATION   (150 * 1024 * 1024)/**< Default value 150MB */


static UINT8 AppMonitorStorageCtrlStack[4096] __attribute__((section (".bss.noinit")));  /**< Storage task Stack */

typedef struct _APP_MONITOR_STORAGE_CTRL_s_ {
    AMBA_KAL_TASK_t Task;
    UINT8 EnableDetectFlag;
    UINT8 EnableMsg;
    UINT16 MsgFlag;
    UINT32 StorageMonSize;
} APP_MONITOR_STORAGE_CTRL_s;

static APP_MONITOR_STORAGE_CTRL_s AmbaMonitorStorageCtrl __attribute__((section (".bss.noinit")));

/**
 *  @brief Storage Space monitor Main Loop
 *
 *  Hdmi monitor Main Loop
 *
 *  @param [in] entryArg information
 *
 */
static void Monitor_StorageTaskEntry(UINT32 entryArg)
{
    int ReturnValue = 0;
    DBGMSGc2(GREEN,"[Applib - Storage Monitor] <TaskEntry> Start");
    while (1) {
#ifdef CONFIG_APP_ARD
        AmbaKAL_TaskSleep(5000);
#else
        AmbaKAL_TaskSleep(1000);
#endif        
#if 0
        {
            int Slot = AppLibCard_GetActiveSlot();
            ReturnValue = AmbaFS_CheckLeftSpace(Slot, STORAGE_MONITOR_SIZE_LIMITATION );
            DBGMSGc2(GREEN,"[Applib - Storage Monitor] <TaskEntry> ReturnValue = %d", ReturnValue);
            if (ReturnValue < 0) {
                if (AmbaMonitorStorageCtrl.EnableMsg) {
                    AppLibComSvcHcmgr_SendMsg(HMSG_STORAGE_RUNOUT, 0, 0);
                }
            }
        }
#else
        if (AmbaMonitorStorageCtrl.EnableDetectFlag) {
            char Drive = 'C';
            UINT64 Size = 0;
            AMP_CFS_DEVINF DevIn;
            Drive = AppLibCard_GetActiveDrive();
#ifdef CONFIG_APP_ARD
            ReturnValue = AmbaFS_GetDev(Drive, &DevIn);
#else
            ReturnValue = AmpCFS_GetDev(Drive, &DevIn);
#endif            
            if (ReturnValue < 0) {
                AmbaPrintColor(RED,"[Applib - Storage Monitor] <TaskEntry> AmpCFS_GetDev Fail ReturnValue = %d", ReturnValue);
            } else {
                DBGMSGc2(GREEN,"[Applib - Storage Monitor] <TaskEntry>  DevIn.Ucl = %d",  DevIn.Ucl);
                DBGMSGc2(GREEN,"[Applib - Storage Monitor] <TaskEntry>  DevIn.Bps = %d",  DevIn.Bps);
                DBGMSGc2(GREEN,"[Applib - Storage Monitor] <TaskEntry> DevIn.Spc = %d", DevIn.Spc);
            }
            Size = (UINT64)DevIn.Ucl * DevIn.Bps * DevIn.Spc;
            DBGMSGc2(GREEN,"[Applib - Storage Monitor] <TaskEntry> size = %d", Size);
            if (Size <= (AmbaMonitorStorageCtrl.StorageMonSize )) {
                if (AmbaMonitorStorageCtrl.EnableMsg) {
                    if (AmbaMonitorStorageCtrl.MsgFlag == 0) {
                        AppLibComSvcHcmgr_SendMsg(HMSG_STORAGE_RUNOUT, 0, 0);
                        AmbaMonitorStorageCtrl.MsgFlag = 1;
                    }
                }
            }
        }
#endif
    }
}

/**
 *  @brief Initialize the Storage Monitor
 *
 *  Initialize the Storage Monitor
 *
 *  @param [in] taskPriority priority of the collection task
 *
 *  @return >=0 success, <0 failure
 */
int AppLibMonitorStorage_Init(UINT32 taskPriority)
{
    /* Set zero as default when first init */
    int ReturnValue = 0;
    DBGMSGc2(GREEN, "[Applib - Storage Monitor] <Init> taskPriority = %d", taskPriority);

    memset(&AppMonitorStorageCtrlStack, 0,  sizeof(AppMonitorStorageCtrlStack));
    AmbaMonitorStorageCtrl.EnableMsg = 0;
    AmbaMonitorStorageCtrl.MsgFlag = 0;
    AmbaMonitorStorageCtrl.EnableDetectFlag = 0;
    AmbaMonitorStorageCtrl.StorageMonSize = STORAGE_MONITOR_SIZE_LIMITATION;
    /* Create the task */
    ReturnValue = AmbaKAL_TaskCreate(&AmbaMonitorStorageCtrl.Task,        /* pTask */
                              "AppLib Monitor Storage",               /* pTaskName */
                              taskPriority,                     /* Priority */
                              Monitor_StorageTaskEntry,            /* void (*EntryFunction)(UINT32) */
                              0x0,                              /* entryArg */
                              AppMonitorStorageCtrlStack,         /* pStackBase */
                              sizeof(AppMonitorStorageCtrlStack), /* StackByteSize */
                              AMBA_KAL_AUTO_START);             /* AutoStart */
    if (ReturnValue != OK) {
        AmbaPrintColor(RED,"[Applib - Storage Monitor] <Init> TaskCreate failure");
    }
    return ReturnValue;
}

/**
 *  @brief Enable monitor's flow to send message.
 *
 *  Enable monitor's flow to send message.
 *
 *  @param [in] enable Enable flag
 *
 *  @return >=0 success, <0 failure
 */
int AppLibMonitorStorage_EnableMsg(UINT32 enable)
{
    AmbaMonitorStorageCtrl.EnableMsg = enable;
    if (enable == 1) {
        AmbaMonitorStorageCtrl.MsgFlag = 0;
    }
    return 0;
}

/**
 *  @brief Enable monitor's flow to detect storage space.
 *
 *  Enable monitor's flow to detect storage space.
 *
 *  @param [in] enable Enable flag
 *
 *  @return >=0 success, <0 failure
 */
int AppLibMonitorStorage_Enable(UINT32 enable)
{
    AmbaMonitorStorageCtrl.EnableDetectFlag = enable;

    return 0;
}


/**
 *  @brief set free space threshold
 *
 *  set free space check threshold
 *
 *  @param [in] threshold threshold size kbyte
 *
 *  @return >=0 success, <0 failure
 */
int AppLibMonitorStorage_SetThreshold(UINT32 threshold)
{
    AmbaMonitorStorageCtrl.StorageMonSize = threshold;

    return 0;
}



