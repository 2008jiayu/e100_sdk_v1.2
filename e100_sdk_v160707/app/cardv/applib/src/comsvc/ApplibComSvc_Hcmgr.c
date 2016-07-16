/**
 * @file src/app/connected/applib/src/comsvc/ApplibComSvc_Hcmgr.c
 *
 * Host Control Manager implementation
 *
 * History:
 *    2013/09/12 - [Martin Lai] created file
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
#include "../AppLibTask_Priority.h"

//#define DEBUG_APPLIB_HCMGR
#if defined(DEBUG_APPLIB_HCMGR)
#define DBGMSG AmbaPrint
#define DBGMSGc(x) AmbaPrintColor(GREEN,x)
#define DBGMSGc2 AmbaPrintColor
#else
#define DBGMSG(...)
#define DBGMSGc(...)
#define DBGMSGc2(...)
#endif

#define HCMGR_STACK_SIZE        0x12000
#define HCMGR_NAME              "AppLib_Host_Control_Manager"
#define HCMGR_MSGQUEUE_SIZE     128
#define HCMGR_AVAILABLE         0x1

typedef struct _APPLIB_HCMGR_s_ {
    UINT8 Stack[HCMGR_STACK_SIZE];                              /**< Stack */
    UINT8 MsgPool[sizeof(APP_MESSAGE_s)*HCMGR_MSGQUEUE_SIZE];   /**< Message memory pool. */
    AMBA_KAL_TASK_t Task;                                       /**< Task ID */
    AMBA_KAL_MSG_QUEUE_t MsgQueue;                              /**< Message queue ID */
    AMBA_KAL_MUTEX_t Mutex;                                     /**< Mutex ID*/
    AMBA_KAL_EVENT_FLAG_t EventFlag;                            /**< Flag ID*/
    APPLIB_HCMGR_HANDLER_s *handler;                                     /**< Handler function pointer*/
} APPLIB_HCMGR_s;

/**
 * @brief Global instance of host control manager
 */
static APPLIB_HCMGR_s AppHcmgr = {0};

/**
 *  @brief Attach handler function
 *
 *  Attach handler function
 *
 *  @param[in] handler handler function pointer
 *
 *  @return >=0 success, <0 failure
 *  @see link to other function
 */
int AppLibComSvcHcmgr_AttachHandler(APPLIB_HCMGR_HANDLER_s *handler)
{
    AmbaKAL_MutexTake(&AppHcmgr.Mutex, AMBA_KAL_WAIT_FOREVER);

    K_ASSERT(handler != NULL);
    AppHcmgr.handler = handler;

    AmbaKAL_MutexGive(&AppHcmgr.Mutex);

    return 0;
}

/**
 *  @brief Detach handler function
 *
 *   Detach handler function
 *
 *  @return >=0 success, <0 failure
 *  @see link to other function
 */
int AppLibComSvcHcmgr_DetachHandler(void)
{
    AmbaKAL_MutexTake(&AppHcmgr.Mutex, AMBA_KAL_WAIT_FOREVER);

    K_ASSERT(AppHcmgr.handler != NULL);
    AppHcmgr.handler->HandlerExit();

    AmbaKAL_MutexGive(&AppHcmgr.Mutex);

    return 0;
}

/**
 *  @brief Reset handler function pointer
 *
 *  Reset handler function pointer
 *
 *  @return >=0 success, <0 failure
 */
int AppLibComSvcHcmgr_ResetHandler(void)
{
    K_ASSERT(AppHcmgr.handler != NULL);
    AppHcmgr.handler = NULL;

    return 0;
}

/**
 *  @brief Send message function
 *
 *  Send message function
 *
 *  @param [in] msg Message object
 *  @param [in] waitOption Wait option
 *
 *  @return >=0 success, <0 failure
 */
int AppLibComSvcHcmgr_SndMsg(APP_MESSAGE_s *msg, UINT32 waitOption)
{
    int ReturnValue = 0;

    ReturnValue = AmbaKAL_MsgQueueSend(&AppHcmgr.MsgQueue, msg, waitOption);
    //AmbaPrint("SndMsg.MessageID = 0x%x ReturnValue = %d", msg->MessageID, ReturnValue);

    return ReturnValue;
}

/**
 *  @brief Receive message function
 *
 *  Receive message function
 *
 *  @param [in] msg Message object
 *  @param [in] waitOption Wait option
 *
 *  @return >=0 success, <0 failure
 */
int AppLibComSvcHcmgr_RcvMsg(APP_MESSAGE_s *msg, UINT32 waitOption)
{
    int ReturnValue = 0;

    ReturnValue = AmbaKAL_MsgQueueReceive(&AppHcmgr.MsgQueue, (void *)msg, waitOption);
    //AmbaPrint("RcvMsg.MessageID = 0x%x ReturnValue = %d", msg->MessageID, ReturnValue);

    return ReturnValue;
}

/**
 *  @brief Host Control Manager Main Loop
 *
 *  Host Control Manager Main Loop
 *
 *  @param [in] info information
 *
 */
static void AppLibComSvcHcmgr_Task(UINT32 info)
{
    if (AppHcmgr.handler != NULL) {
        AppHcmgr.handler->HandlerMain();
    } else {
        AmbaPrintColor(RED, "[Applib - Hcmgr] The handler is not registered.");
    }
}

/**
 *  @brief Host Control Manager pre-initial function
 *
 *  Host Control Manager pre-initial function
 *
 *  @return >=0 success, <0 failure
 */
int AppLibComSvcHcmgr_PreInit(void)
{
    /* Reset the AppHcmgr parameters. */
    memset(&AppHcmgr, 0x0, sizeof(AppHcmgr));
    return 0;
}

/**
 *  @brief Host Control Manager initial function
 *
 *  Host Control Manager initial function
 *
 *  @return >=0 success, <0 failure
 */
int AppLibComSvcHcmgr_Init(void)
{
    int ReturnValue = 0;
    DBGMSG("[Applib - Hcmgr] App_hcmgr_init");

    /* Create App message queue */
    ReturnValue = AmbaKAL_MsgQueueCreate(&AppHcmgr.MsgQueue, AppHcmgr.MsgPool, sizeof(APP_MESSAGE_s), HCMGR_MSGQUEUE_SIZE);
    if (ReturnValue == OK) {
        DBGMSGc2(GREEN, "[Applib - Hcmgr] Create Queue success = %d", ReturnValue);
    } else {
        AmbaPrintColor(RED, "[Applib - Hcmgr] Create Queue fail = %d", ReturnValue);
    }

    /* Create mutex */
    ReturnValue = AmbaKAL_MutexCreate(&AppHcmgr.Mutex);
    if (ReturnValue == OK) {
        DBGMSGc2(GREEN, "[Applib - Hcmgr]Create Mutex success = %d", ReturnValue);
    } else {
        AmbaPrintColor(RED, "[Applib - Hcmgr]Create Mutex fail = %d", ReturnValue);
    }

    /* Create flag */
    ReturnValue = AmbaKAL_EventFlagCreate(&AppHcmgr.EventFlag);
    if (ReturnValue == OK) {
        DBGMSGc2(GREEN, "[Applib - Hcmgr]Create flag success = %d", ReturnValue);
    } else {
        AmbaPrintColor(RED, "[Applib - Hcmgr]Create flag fail = %d", ReturnValue);
    }
    AmbaKAL_EventFlagGive(&AppHcmgr.EventFlag, HCMGR_AVAILABLE);

    /* Create Host Control Manager task*/
    ReturnValue = AmbaKAL_TaskCreate(&AppHcmgr.Task, /* pTask */
            HCMGR_NAME, /* pTaskName */
            APPLIB_HCMGR_TASK_PRIORITY, /* Priority */
            AppLibComSvcHcmgr_Task, /* void (*EntryFunction)(UINT32) */
            0x0, /* EntryArg */
            (void *) AppHcmgr.Stack, /* pStackBase */
            HCMGR_STACK_SIZE, /* StackByteSize */
            AMBA_KAL_AUTO_START); /* AutoStart */
    if (ReturnValue != OK) {
        AmbaPrintColor(RED, "[Applib - Hcmgr]Create task fail = %d", ReturnValue);
    }

    return 0;
}

/**
 *  @brief Send the message to message queue with "WAIT_FOREVER" flag
 *
 *  Send the message to message queue with "WAIT_FOREVER" flag
 *
 *  @param [in] msg Message ID
 *  @param [in] param1 First parameter
 *  @param [in] param2 Second parameter
 *
 *  @return >=0 success, <0 failure
 */
int AppLibComSvcHcmgr_SendMsg(UINT32 msg, UINT32 param1, UINT32 param2)
{
    APP_MESSAGE_s TempMessage = {0};

    TempMessage.MessageID = msg;
    TempMessage.MessageData[0] = param1;
    TempMessage.MessageData[1] = param2;

    return AppLibComSvcHcmgr_SndMsg(&TempMessage, AMBA_KAL_WAIT_FOREVER);
}

/**
 *  @brief Send the message to message queue with "NO_WAIT" flag
 *
 *  Send the message to message queue with "NO_WAIT" flag
 *
 *  @param [in] msg Message ID
 *  @param [in] param1 First parameter
 *  @param [in] param2 Second parameter
 *
 *  @return >=0 success, <0 failure
 */
int AppLibComSvcHcmgr_SendMsgNoWait(UINT32 msg, UINT32 param1, UINT32 param2)
{
    APP_MESSAGE_s TempMessage = {0};

    TempMessage.MessageID = msg;
    TempMessage.MessageData[0] = param1;
    TempMessage.MessageData[1] = param2;

    return AppLibComSvcHcmgr_SndMsg(&TempMessage, AMBA_KAL_NO_WAIT);
}

