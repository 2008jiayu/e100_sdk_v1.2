/**
 * @file app/connected/applib/src/va/ApplibVideoAnal_3aHdlr.c
 *
 * Amba VA frame 3A info handler
 *
 * History:
 *    2015/01/09 - [Bill Chou] created file
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
#include <AmbaDataType.h>
#include <AmbaKAL.h>
#include <AmbaPrint.h>
#include <AmbaPrintk.h>
#include <recorder/VideoEnc.h>


#define TRIA_TASK_NAMEE          "VA 3A adata hdlr"
#define TRIA_MSGQUEUE_SIZE (4)

static UINT8 AppTriAStack[(32<<10)];

typedef struct APPLIB_TRIA_MGRMESSAGE_t_ {
    UINT32 MessageID;
    UINT32 MessageData[2];
    AMBA_DSP_EVENT_CFA_3A_DATA_s* pData3A;
} APPLIB_TRIA_MGRMESSAGE_t;

typedef struct APPLIB_TRIA_CB_SLOT_t_ {
    APPLIB_VA_TRIAHDLR_CB Func;
    UINT32 Event;
} APPLIB_TRIA_CB_SLOT_t;

typedef struct _G_TRIA_t_ {
    UINT8 Init;
    AMBA_KAL_TASK_t Task;
    AMBA_KAL_MUTEX_t Mutex;
    AMBA_KAL_MSG_QUEUE_t MsgQueue;
    UINT8 MsgPool[sizeof(APPLIB_TRIA_MGRMESSAGE_t) * TRIA_MSGQUEUE_SIZE];
    APPLIB_TRIA_CB_SLOT_t CbFunc[APPLIB_TRIA_HDLR_MAX_CB];
} G_APPLIB_TRIA_t;

static G_APPLIB_TRIA_t G_AppLib_TRIA = { 0 };

#define APPLIB_TRIA_LOC AmbaKAL_MutexTake(&G_AppLib_TRIA.Mutex, AMBA_KAL_WAIT_FOREVER);
#define APPLIB_TRIA_UNL AmbaKAL_MutexGive(&G_AppLib_TRIA.Mutex);

int  AppLibVideoAnal_TriAHdlr_IsInit(void)
{
    return G_AppLib_TRIA.Init;
}

void VA_TriA_Maintask(UINT32 info)
{
    APPLIB_TRIA_MGRMESSAGE_t Msg;
    int ReturnValue;
    int T;
    while (1) {
        ReturnValue = AmbaKAL_MsgQueueReceive(&G_AppLib_TRIA.MsgQueue, (void *) &Msg, AMBA_KAL_WAIT_FOREVER );
        if (ReturnValue == OK) {
            //new frame coming in
            APPLIB_TRIA_LOC
            ;
            for (T = 0; T < APPLIB_TRIA_HDLR_MAX_CB; T++) {
                if (G_AppLib_TRIA.CbFunc[T].Func != NULL && G_AppLib_TRIA.CbFunc[T].Event == Msg.MessageID) {
                    G_AppLib_TRIA.CbFunc[T].Func(Msg.MessageID, Msg.pData3A);
                }
            }
            APPLIB_TRIA_UNL
            ;
        } else {
            AmbaKAL_TaskSleep(1);
        }
    }
}

int AppLibVideoAnal_TriAHdlr_GetDefCfg(APPLIB_TRIA_TASK_CFG_t* cfg)
{
    cfg->TaskPriority = APPLIB_TRIA_HDLR_DEF_PRIORITY;
    cfg->TaskStackSize = APPLIB_TRIA_HDLR_DEF_TASK_STACK_SIZE;
    return OK;
}

int AppLibVideoAnal_TriAHdlr_Init(void)
{
    int ReturnValue;
    if (G_AppLib_TRIA.Init != 0) {
        return NG;
    }

    AmbaPrint("VA 3A Handler init...... ");
    // create mqueue
    ReturnValue = AmbaKAL_MsgQueueCreate(&G_AppLib_TRIA.MsgQueue, G_AppLib_TRIA.MsgPool, sizeof(APPLIB_TRIA_MGRMESSAGE_t), TRIA_MSGQUEUE_SIZE);
    if (ReturnValue == OK) {
        AmbaPrintColor(GREEN, "[VA 3A INIT] Create Queue success");
    } else {
        AmbaPrintColor(RED, "[VA 3A INIT] Create Queue fail = %d", ReturnValue);
        return NG;
    }



    // create mutex
    ReturnValue = AmbaKAL_MutexCreate(&G_AppLib_TRIA.Mutex);
    if (ReturnValue == OK) {
        AmbaPrintColor(GREEN, "[VA 3A INIT] Create Mutex success");
    } else {
        AmbaPrintColor(RED, "[VA 3A INIT] Create Mutex fail = %d", ReturnValue);
        return NG;
    }


    AmbaPrintColor(GREEN, "[VA 3A INIT] TaskPriority %d , TaskStackSize %d", APPLIB_TRIA_HDLR_DEF_PRIORITY, APPLIB_TRIA_HDLR_DEF_TASK_STACK_SIZE);
    // create task
    ReturnValue = AmbaKAL_TaskCreate(&G_AppLib_TRIA.Task,
                              TRIA_TASK_NAMEE,
                              APPLIB_TRIA_HDLR_DEF_PRIORITY,
                              VA_TriA_Maintask,
                              0x0,
                              AppTriAStack,
                              APPLIB_TRIA_HDLR_DEF_TASK_STACK_SIZE,
                              AMBA_KAL_AUTO_START);
    if (ReturnValue == OK) {
        AmbaPrintColor(GREEN, "[VA 3A INIT] Create Task success");
    } else {
        AmbaPrintColor(RED, "[VA 3A INIT] Create Task fail = %d", ReturnValue);
        return NG;
    }
    /// register AmbaDSP Event Handler
    ReturnValue = AmbaDSP_RegisterEventHandler(AMBA_DSP_EVENT_CFA_3A_DATA_READY, (AMBA_DSP_EVENT_HANDLER_f)AppLibVideoAnal_DSP_EventHdlr_NewAE);
    if (ReturnValue != OK){
        AmbaPrint("Register Event Handler fail");
    }

    G_AppLib_TRIA.Init = 1;
    AppLibComSvcHcmgr_SendMsgNoWait(HMSG_VA_TASK3A_ON, 0, 0);
    AmbaPrint("VA 3A init done");

    return ReturnValue;
}

int AppLibVideoAnal_TriAHdlr_Register(UINT32 event,
                            APPLIB_VA_TRIAHDLR_CB func)
{
    int T;
    APPLIB_TRIA_LOC
    ;
    for (T = 0; T < APPLIB_TRIA_HDLR_MAX_CB; T++) {
        if (G_AppLib_TRIA.CbFunc[T].Func == NULL) {
            G_AppLib_TRIA.CbFunc[T].Func = func;
            G_AppLib_TRIA.CbFunc[T].Event = event;
            break;
        }
    }
    APPLIB_TRIA_UNL
    ;
    if (T == APPLIB_TRIA_HDLR_MAX_CB) {
        return NG;
    }
    return OK;
}

int AppLibVideoAnal_TriAHdlr_UnRegister(UINT32 event,
                              APPLIB_VA_TRIAHDLR_CB func)
{
    int T;
    APPLIB_TRIA_LOC
    ;
    for (T = 0; T < APPLIB_TRIA_HDLR_MAX_CB; T++) {
        if (G_AppLib_TRIA.CbFunc[T].Func == func && G_AppLib_TRIA.CbFunc[T].Event == event) {
            G_AppLib_TRIA.CbFunc[T].Func = NULL;
            G_AppLib_TRIA.CbFunc[T].Event = 0;
        }
    }
    APPLIB_TRIA_UNL
    ;
    return OK;
}

int AmbaVA_TriAHdlr_NewAE(UINT32 event,
                            AMBA_DSP_EVENT_CFA_3A_DATA_s* pData3A)
{
    int ReturnValue = 0;
    APPLIB_TRIA_MGRMESSAGE_t TempMessage = { 0 };

    TempMessage.MessageID = event;
    TempMessage.pData3A = pData3A;
    ReturnValue = AmbaKAL_MutexTake(&G_AppLib_TRIA.Mutex, AMBA_KAL_NO_WAIT);
    if (ReturnValue == OK) {
        // VA 3A task is idle or none registering cb, send msg to trigger proc
        ReturnValue = AmbaKAL_MsgQueueSend(&G_AppLib_TRIA.MsgQueue, &TempMessage, AMBA_KAL_NO_WAIT);
        AmbaKAL_MutexGive(&G_AppLib_TRIA.Mutex);
    }

    return ReturnValue;
}

int AppLibVideoAnal_DSP_EventHdlr_NewAE(AMBA_DSP_EVENT_CFA_3A_DATA_s* pData3A)
{
   int ReturnValue = 0;
    APPLIB_TRIA_MGRMESSAGE_t TempMessage = { 0 };

    TempMessage.MessageID = AMBA_DSP_EVENT_CFA_3A_DATA_READY;
    TempMessage.pData3A = pData3A;

    ReturnValue = AmbaKAL_MutexTake(&G_AppLib_TRIA.Mutex, AMBA_KAL_NO_WAIT);
    if (ReturnValue == OK) {
        // VA 3A task is idle or none registering cb, send msg to trigger proc
        ReturnValue = AmbaKAL_MsgQueueSend(&G_AppLib_TRIA.MsgQueue, &TempMessage, AMBA_KAL_NO_WAIT);
        AmbaKAL_MutexGive(&G_AppLib_TRIA.Mutex);
    }

    return ReturnValue;
}

