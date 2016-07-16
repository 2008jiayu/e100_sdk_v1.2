/**
 * @file src/app/connected/applib/src/applib/monitor/ApplibMonitor_HDMI.c
 *
 * HDMI monitoring task related APIs
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
#include <AmbaHDMI.h>
#include <applibhmi.h>
#include <applib.h>

//#define DEBUG_APPLIB_MONITOR_HDMI
#if defined(DEBUG_APPLIB_MONITOR_HDMI)
#define DBGMSG AmbaPrint
#define DBGMSGc(x) AmbaPrintColor(GREEN,x)
#define DBGMSGc2 AmbaPrintColor
#else
#define DBGMSG(...)
#define DBGMSGc(...)
#define DBGMSGc2(...)
#endif

#define HDMI_MONITOR_MODE_POOLING   (1)
#define HDMI_MONITOR_MODE_INTERRUPT (0)
#define HDMI_MONITOR_MSG(enable)        ((enable == AMBA_HDMI_CABLE_DETECT_NONE) ? HMSG_HDMI_INSERT_CLR : HMSG_HDMI_INSERT_SET)

static UINT8 AppMonitorHdmiCtrlStack[4096] __attribute__((section (".bss.noinit")));  /* HDMI task Stack */

typedef struct _APP_MONITOR_HDMI_CTRL_s_ {
    AMBA_KAL_TASK_t     Task;
} APP_MONITOR_HDMI_CTRL_s;

static APP_MONITOR_HDMI_CTRL_s AmbaMonitorHdmiCtrl __attribute__((section (".bss.noinit")));

/**
 *  @brief Hdmi monitor Main Loop
 *
 *  Hdmi monitor Main Loop
 *
 *  @param [in] entryArg information
 *
 */
static void Monitor_HdmiTaskEntry(UINT32 entryArg)
{
    AMBA_HDMI_CABLE_DETECT_e PrevCableState = AMBA_HDMI_CABLE_DETECT_NONE;
    AMBA_HDMI_SINK_INFO_s SinkInfo;
    UINT32 Count = 0;
    int ReturnValue = 0;
    DBGMSGc2(GREEN,"[Applib -HDMI Monitor] <TaskEntry> Start");
    while (1) {
#if HDMI_MONITOR_MODE_INTERRUPT
        UINT32 ActualFlags;
        AmbaKAL_EventFlagTake(&AmbaHDMI_Ctrl.EventFlag,
                              (1 << AMBA_HDMI_EVENT_SINK_DISCONNECTED) |
                              (1 << AMBA_HDMI_EVENT_SINK_POWERED_OFF)  |
                              (1 << AMBA_HDMI_EVENT_SINK_POWERED_ON),
                              AMBA_KAL_OR_CLEAR, &ActualFlags, AMBA_KAL_WAIT_FOREVER);
        DBGMSGc2(GREEN,"[Applib -HDMI Monitor] <Monitor_HdmiTaskEntry> SinkInfo.CableState = %d",SinkInfo.CableState);
        /* De-Bouncing */
        for (Count = 0; Count < 10; Count ++) {
            if (AmbaHDMI_GetSinkInfo(&SinkInfo) != OK)
                break;

            if (PrevCableState != SinkInfo.CableState) {
                PrevCableState = SinkInfo.CableState;
                Count = 0;
            }

            AmbaKAL_TaskSleep(30);
        }

        if (Count == 10) {
            AmbaPrintColor(GREEN,"[Applib - HDMI Monitor] <TaskEntry>SinkInfo.CableState = %d",SinkInfo.CableState);
            AppLibComSvcHcmgr_SendMsg(HDMI_MONITOR_MSG(SinkInfo.CableState), (UINT32)SinkInfo.pVideoInfo, (UINT32)SinkInfo.pAudioInfo);
        }
#elif HDMI_MONITOR_MODE_POOLING
        AmbaKAL_TaskSleep(1000);
        ReturnValue = AmbaHDMI_GetSinkInfo(&SinkInfo);
        DBGMSGc2(GREEN,"[Applib - HDMI Monitor] <TaskEntry> AmbaHDMI_GetSinkInfo ReturnValue = %d, SinkInfo.CableState = %d", ReturnValue, SinkInfo.CableState);
        if ((ReturnValue == OK) && (PrevCableState != SinkInfo.CableState)) {
            for (Count = 0; Count < 10; Count ++) {
                if (AmbaHDMI_GetSinkInfo(&SinkInfo) != OK)
                    break;

                if (PrevCableState != SinkInfo.CableState) {
                    PrevCableState = SinkInfo.CableState;
                    Count = 0;
                }
                AmbaKAL_TaskSleep(30);
            }

            if (Count == 10) {
#if 0
                int i ;
                for (i =0; i<AMBA_NUM_VIDEO_ID;i++) {
                    AmbaPrintColor(MAGENTA,"SinkInfo.pVideoInfo[%d] = %d",i,SinkInfo.pVideoInfo[i]);
                }
#endif
                DBGMSGc2(GREEN,"[Applib - HDMI Monitor] <TaskEntry> SinkInfo.CableState = %d",SinkInfo.CableState);
                AppLibComSvcHcmgr_SendMsg(HDMI_MONITOR_MSG(SinkInfo.CableState), (UINT32)SinkInfo.pVideoInfo, (UINT32)SinkInfo.pAudioInfo);
            }
        }
#endif

    }
}

/**
 *  @brief Initialize the HDMI Hot-Plug Monitor
 *
 *  Initialize the HDMI Hot-Plug Monitor
 *
 *  @param [in] taskPriority priority of the collection task
 *
 *  @return >=0 success, <0 failure
 */
int AppLibMonitorHdmi_Init(UINT32 taskPriority)
{
    /* Set zero as default when first init */
    int ReturnValue = 0;
    CEA861_SOURCE_PRODUCT_DESCRIPTION_s pSourceProduct = {0};
    memset(&AmbaMonitorHdmiCtrl, 0, sizeof(APP_MONITOR_HDMI_CTRL_s));
    AmbaHDMI_Init(&pSourceProduct);
    DBGMSGc2(GREEN, "[Applib -HDMI Monitor] <Init> taskPriority = %d", taskPriority);

    memset(&AppMonitorHdmiCtrlStack, 0,  sizeof(AppMonitorHdmiCtrlStack));
    /* Create the task */
    ReturnValue = AmbaKAL_TaskCreate(&AmbaMonitorHdmiCtrl.Task,        /* pTask */
                              "AppLib Monitor HDMI",               /* pTaskName */
                              taskPriority,                     /* Priority */
                              Monitor_HdmiTaskEntry,            /* void (*EntryFunction)(UINT32) */
                              0x0,                              /* entryArg */
                              AppMonitorHdmiCtrlStack,         /* pStackBase */
                              sizeof(AppMonitorHdmiCtrlStack), /* StackByteSize */
                              AMBA_KAL_AUTO_START);             /* AutoStart */
    if (ReturnValue != OK) {
        AmbaPrintColor(RED,"[Applib - HDMI Monitor] <Init> TaskCreate failure");
    }
    return ReturnValue;
}

