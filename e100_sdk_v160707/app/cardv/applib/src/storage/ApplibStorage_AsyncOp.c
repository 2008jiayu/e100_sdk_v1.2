/**
 * @file src/app/connected/applib/src/recorder/ApplibStorage_AsyncOp
 *
 * Implementation of Storage handle async operation
 *
 * History:
 *    2014/05/12 - [Annie Ting] created file
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
#include <AmbaAudio.h>
#include <AmbaUtility.h>
#include <recorder/Encode.h>
#include <recorder/AudioEnc.h>
#include <fifo/Fifo.h>
#include "../AppLibTask_Priority.h"

//#define DEBUG_APPLIB_ASYNC_OP
#if defined(DEBUG_APPLIB_ASYNC_OP)
#define DBGMSG AmbaPrint
#define DBGMSGc(x) AmbaPrintColor(GREEN,x)
#define DBGMSGc2 AmbaPrintColor
#else
#define DBGMSG(...)
#define DBGMSGc(...)
#define DBGMSGc2(...)
#endif

#define ASYNC_OP_MGR_STACK_SIZE    (0x8000)
#define ASYNC_OP_MGR_NAME          "AppLib_Storage_Async_Operation_Manager"
#define ASYNC_OP_MGR_MSGQUEUE_SIZE (128)

#define APPLIB_MAX_STORAGE_OP_HANDLER   4

typedef struct _APPLIB_STORAGE_OP_MGRMESSAGE_s_ {
    UINT32 MessageID;
    UINT32 MessageData[2];
} APPLIB_STORAGE_OP_MGRMESSAGE_s;


typedef struct _APPLIB_STORAGE_OP_CFG_s_ {
    UINT8 Stack[ASYNC_OP_MGR_STACK_SIZE];  /**< Stack */
    UINT8 MsgPool[sizeof(APPLIB_STORAGE_OP_MGRMESSAGE_s)*ASYNC_OP_MGR_MSGQUEUE_SIZE];
    AMBA_KAL_TASK_t Task;               /**< Task ID */
    AMBA_KAL_MSG_QUEUE_t MsgQueue;      /**< Message queue ID */
    APPLIB_ENCODE_HANDLER_s Handler[APPLIB_MAX_STORAGE_OP_HANDLER];
} APPLIB_STORAGE_OP_CFG_s;


/** Global instance of manager */
static APPLIB_STORAGE_OP_CFG_s G_StorageAsyncOpMgr = {0};
static int ApplibStorageAsyncOpInitFlag = -1;
static int ApplibAsyncOpHandlerCount = 0;


/**
 *  @brief Send message to Storage Async Op Mgr.
 *
 *  Send message to  Storage Async Op Mgr.
 *
 *  @param [in] msg Message ID
 *  @param [in] param1 first parameter
 *  @param [in] param2 second parameter
 *
 *  @return >=0 success, <0 failure
 */
int AppLibStorageAsyncOp_SndMsg(UINT32 msg, UINT32 param1, UINT32 param2)
{
    int ReturnValue = 0;
    APPLIB_STORAGE_OP_MGRMESSAGE_s TempMessage = {0};

    TempMessage.MessageID = msg;
    TempMessage.MessageData[0] = param1;
    TempMessage.MessageData[1] = param2;

    ReturnValue = AmbaKAL_MsgQueueSend(&G_StorageAsyncOpMgr.MsgQueue, &TempMessage, AMBA_KAL_NO_WAIT);
    //AmbaPrint("SndMsg.MessageID = 0x%x ReturnValue = %d", msg->MessageID, ReturnValue);

    return ReturnValue;
}


/**
 *  @brief  Receive message function
 *
 *  Receive message function
 *
 *  @param [in] msg message object
 *  @param [in] waitOption Wait option
 *
 *  @return >=0 success, <0 failure
 */
static int AppLibStorageAsyncOp_RcvMsg(APPLIB_STORAGE_OP_MGRMESSAGE_s *msg, UINT32 waitOption)
{
    int ReturnValue = 0;

    ReturnValue = AmbaKAL_MsgQueueReceive(&G_StorageAsyncOpMgr.MsgQueue, (void *)msg, waitOption);
    AmbaPrint("RcvMsg.MessageID = 0x%x ReturnValue = %d", msg->MessageID, ReturnValue);

    return ReturnValue;
}

/**
 *  @brief  Register handler for different cmd
 *
 *  Receive message function
 *
 *  @param [in] Handler  encoder handler
 *
 *  @return >=0 success, <0 failure
 */
int AppLibStorageAsyncOp_RegHandler(APPLIB_ENCODE_HANDLER_s *Handler)
{
    int ReturnValue = 0;
    if (ApplibAsyncOpHandlerCount < APPLIB_MAX_STORAGE_OP_HANDLER) {
        memcpy(&G_StorageAsyncOpMgr.Handler[ApplibAsyncOpHandlerCount], Handler, sizeof(APPLIB_ENCODE_HANDLER_s));
        ApplibAsyncOpHandlerCount ++;
    } else {
        AmbaPrintColor(GREEN,"[Applib-Recorder]Register Handler Error, Handler runout");
        ReturnValue = -1;
    }
    return ReturnValue;
}


/**
 *  @brief Task of Loop Encoder manager.
 *
 *  Task of Loop Encoder manager.
 *
 *  @param [in] info information
 *
 */
static void AppLibStorageAsyncOp_MgrTask(UINT32 info)
{

    APPLIB_STORAGE_OP_MGRMESSAGE_s Msg = {0};
    int i = 0;
    DBGMSG("[Applib - LoopEnc] Manager ready");

    while (1) {
        AppLibStorageAsyncOp_RcvMsg(&Msg, AMBA_KAL_WAIT_FOREVER);
        DBGMSG("[Applib - LoopEnc] Received msg: 0x%X (Param1 = 0x%X / Param2 = 0x%X)", Msg.MessageID, Msg.MessageData[0], Msg.MessageData[1]);
        for (i = 0; i < APPLIB_MAX_STORAGE_OP_HANDLER; i++) {
            if (G_StorageAsyncOpMgr.Handler[i].Command == Msg.MessageID) {
                        if (G_StorageAsyncOpMgr.Handler[i].FuncSearch != NULL) {
                            DBGMSG("[Applib - LoopEnc] [Applib - LoopEnc] call function FuncSearch");
                            G_StorageAsyncOpMgr.Handler[i].FuncSearch(Msg.MessageData[0],Msg.MessageData[1]);
                        }
                        if (G_StorageAsyncOpMgr.Handler[i].FuncHandle != NULL) {
                            DBGMSG("[Applib - LoopEnc] [Applib - LoopEnc] call function FuncHandle");
                            G_StorageAsyncOpMgr.Handler[i].FuncHandle(Msg.MessageData[0],Msg.MessageData[1]);
                        }
                        if (G_StorageAsyncOpMgr.Handler[i].FuncReturn != NULL) {
                            DBGMSG("[Applib - LoopEnc] [Applib - LoopEnc] call function FuncReturn");
                            G_StorageAsyncOpMgr.Handler[i].FuncReturn();
                        }
                break;
            }
        }
    }
}

/**
 *  @brief Initialization of Loop Encoder manager.
 *
 *   Initialization of Loop Encoder manager.
 *
 *  @return >=0 success, <0 failure
 */
int AppLibStorageAsyncOp_Init(void)
{
    int ReturnValue = 0;

    if (ApplibStorageAsyncOpInitFlag == 0)
        return 0;
    DBGMSG("[Applib - AsyncOp] <AppLibStorageAsyncOp_Init> start");

    /* Clear G_muxmgr */
    memset(&G_StorageAsyncOpMgr, 0, sizeof(APPLIB_STORAGE_OP_CFG_s));

    /* Create message queue */
    ReturnValue = AmbaKAL_MsgQueueCreate(&G_StorageAsyncOpMgr.MsgQueue, G_StorageAsyncOpMgr.MsgPool, sizeof(APPLIB_STORAGE_OP_MGRMESSAGE_s), ASYNC_OP_MGR_MSGQUEUE_SIZE);
    if (ReturnValue == OK) {
        DBGMSGc2(GREEN, "[Applib - LoopEnc] Create Queue success = %d", ReturnValue);
    } else {
        AmbaPrintColor(RED, "[Applib - LoopEnc] Create Queue fail = %d", ReturnValue);
    }


    /* Create Host Control Manager task*/
    ReturnValue = AmbaKAL_TaskCreate(&G_StorageAsyncOpMgr.Task, /* pTask */
        ASYNC_OP_MGR_NAME, /* pTaskName */
        APPLIB_STORAGE_ASYNC_OP_TASK_PRIORITY, /* Priority */
        AppLibStorageAsyncOp_MgrTask, /* void (*EntryFunction)(UINT32) */
        0x0, /* EntryArg */
        (void *) G_StorageAsyncOpMgr.Stack, /* pStackBase */
        ASYNC_OP_MGR_STACK_SIZE, /* StackByteSize */
        AMBA_KAL_AUTO_START); /* AutoStart */

    if (ReturnValue != OK) {
        AmbaPrintColor(RED, "[Applib - LoopEnc] Create task fail = %d", ReturnValue);
    }

    ApplibStorageAsyncOpInitFlag = 0;
    DBGMSG("[Applib - LoopEnc] <AppLibStorageAsyncOp_Init> end: ReturnValue = %d", ReturnValue);

    return ReturnValue;
}


