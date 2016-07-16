/**
  * @file applib/src/graphics/DirectDraw/ApplibGraphics_DD.c
  *
  * ApplibGraphics_DD source code
  *
  * History:
  *    2013/11/15 - [Eric Yen] created file
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
#include "graphics/DirectDraw/ApplibGraphics_DirectDraw.h"
#include "graphics/ApplibGraphics.h"

#define DIRECTDRAW_PRIORITY  128
#define APPLIB_DIRECTDRAW_TASK_MSG_NUM    (64) // < Maximum number of Vout messages in Vout message pool.
#define DIRECTDRAW_TASK_STACK_SIZE  (0x4000) // < Size of the stack used by a Still Task (16kB).
APPLIB_DIRECTDRAW_TASK_RESOURCE_s APPLIB_G_DD = { 0 };

static void *ddMsgPoolRaw = NULL;
static void *ddStackRaw = NULL;
static void *ddStack = NULL;

//#define DEBUG_APPLIB_DIRECTDRAW
#if defined(DEBUG_APPLIB_DIRECTDRAW)
#define DBGMSG AmbaPrint
#define DBGMSGc(x) AmbaPrintColor(GREEN,x)
#define DBGMSGc2 AmbaPrintColor
#else
#define DBGMSG(...)
#define DBGMSGc(...)
#define DBGMSGc2(...)
#endif

int AppLibDirectDraw_SendMsg(const APPLIB_DIRECTDRAW_TASK_MSG_s *ddMsg, const UINT32 timeout)
{
    return AmpMsgQueue_Send(&APPLIB_G_DD.ddMsgQueue, ddMsg, timeout);
}

static void _AppLibDirectDraw_Task(UINT32 entryArg)
{
    int Rval;                                 // Function call return value
    APPLIB_DIRECTDRAW_TASK_MSG_s ddMsg;       // Message

    while (1) {
        // Clean message
        SET_ZERO(ddMsg);

        // Receive decode message.
        Rval = AmpMsgQueue_Receive(&APPLIB_G_DD.ddMsgQueue, &ddMsg, AMBA_KAL_WAIT_FOREVER);
        if (Rval != AMP_MQ_SUCCESS) {
            DBGMSGc2(RED,"[AppLib - Graphics] <_AppLibDirectDraw_Task> Receive MQ error %d", Rval);
        }

        switch (ddMsg.MessageType) {
            case APPLIB_DIRECTDRAW_TASK_MSG_DRAW:
                if (ddMsg.Refresh != NULL) {
                    ddMsg.Refresh(ddMsg.Parameter);
                } else {
                    DBGMSG("[AppLib - Graphics] <_AppLibDirectDraw_Task> no refresh function");
                }
                break;
            case APPLIB_DIRECTDRAW_TASK_MSG_HIDE:
                if (ddMsg.Clean != NULL) {
                    ddMsg.Clean(ddMsg.Parameter);
                } else {
                    DBGMSG("[AppLib - Graphics] <_AppLibDirectDraw_Task> no clean function");
                }
                break;
            default:
                break;
        }
    }
}

int AppLibDirectDraw_TaskInit(void)
{
    int Rval = 0;

    DBGMSG("[AppLib - Graphics] <AppLibDirectDraw_TaskInit> Start");

    /** Step 1: Create message pool for message queue */
    if (ddMsgPoolRaw == NULL) {
        Rval = AmpUtil_GetAlignedPool(
                APPLIB_G_MMPL,
                (void**) &(APPLIB_G_DD.ddMsgPool),
                &ddMsgPoolRaw,
                (sizeof(APPLIB_DIRECTDRAW_TASK_MSG_s) * APPLIB_DIRECTDRAW_TASK_MSG_NUM),
                1 << 5 // Align 32
            );
        if (Rval != AMP_OK) {
            DBGMSGc2(RED, "[AppLib - Graphics] <AppLibDirectDraw_TaskInit> %s:%u Failed to allocate memory (%d).", __FUNCTION__, __LINE__, Rval);
            goto ReturnError;
        }
    }

    /** Step 2: Create message queue */
    if (AmpMsgQueue_IsCreated(&APPLIB_G_DD.ddMsgQueue) == 0) {
        Rval = AmpMsgQueue_Create(
                &(APPLIB_G_DD.ddMsgQueue),
                APPLIB_G_DD.ddMsgPool,
                sizeof(APPLIB_DIRECTDRAW_TASK_MSG_s),
                APPLIB_DIRECTDRAW_TASK_MSG_NUM
            );
        if (Rval != AMP_MQ_SUCCESS) {
            DBGMSGc2(RED, "[AppLib - Graphics] <AppLibDirectDraw_TaskInit> %s:%u Failed to create message queue (%d).", __FUNCTION__, __LINE__, Rval);
            goto ReturnError;
        }
    }

    /** Step 3: Create stack for task */
    if (ddStackRaw == NULL) {
        Rval = AmpUtil_GetAlignedPool(
                APPLIB_G_MMPL,
                &ddStack,
                &ddStackRaw,
                DIRECTDRAW_TASK_STACK_SIZE,
                1 << 5 // Align 32
            );
        if (Rval != AMP_OK) {
            DBGMSGc2(RED, "[AppLib - Graphics] <AppLibDirectDraw_TaskInit> %s:%u Failed to allocate memory (%d).", __FUNCTION__, __LINE__, Rval);
            goto ReturnError;
        }
    }

    /** Step 4: Create task */
    Rval = AmbaKAL_TaskCreate(
            &(APPLIB_G_DD.ddTask),      // pTask
            "AppLib_DirectDrawTask",    // pTaskName
            DIRECTDRAW_PRIORITY,        // Priority
            _AppLibDirectDraw_Task,     // void (*EntryFunction)(UINT32)
            0x0,                        // EntryArg
            ddStack,                    // pStackBase
            DIRECTDRAW_TASK_STACK_SIZE, // StackByteSize
            AMBA_KAL_AUTO_START         // AutoStart
        );
    if (Rval != AMP_OK) {
        DBGMSGc2(RED, "[AppLib - Graphics] <AppLibDirectDraw_TaskInit> %s:%u Failed to create task (%d).", __FUNCTION__, __LINE__, Rval);
        goto ReturnError;
    }

    /** Step 5: Set flag */
    APPLIB_G_DD.IsInit = 1;

    DBGMSG("[AppLib - Graphics] <AppLibDirectDraw_TaskInit> End");
    return 0; // Success

ReturnError:
    AppLibDirectDraw_DeinitTask();
    DBGMSG("[AppLib - Graphics] <AppLibDirectDraw_TaskInit> End");
    return -1; // Error
}

int AppLibDirectDraw_DeinitTask(void)
{
    int ReturnValue = 0;    // Success
    int Rval = 0;           // Function call return

    DBGMSG("[AppLib - Graphics] <AppLibDirectDraw_DeinitTask> Start");

    /** Step 1: Reset flag */
    APPLIB_G_DD.IsInit = 0;

    /** Step 2: Delete task */
    Rval = AmbaKAL_TaskTerminate(&(APPLIB_G_DD.ddTask));
    // TX_THREAD_ERROR: The task is not created.
    if ((Rval != AMP_OK) && (Rval != TX_THREAD_ERROR)) { // TODO: Wait DSP definition of TX_THREAD_ERROR
        DBGMSG("[AppLib - Graphics] <AppLibDirectDraw_DeinitTask> %s:%u Failed to terminate task (%d).", __FUNCTION__, __LINE__, Rval);
        ReturnValue = -1; // Error
    }
    Rval = AmbaKAL_TaskDelete(&(APPLIB_G_DD.ddTask));
    // TX_THREAD_ERROR: The task is not created.
    // TX_DELETE_ERROR: The task is not terminated.
    if ((Rval != AMP_OK) && (Rval != TX_THREAD_ERROR) && (Rval != TX_DELETE_ERROR)) { // TODO: Wait DSP definition of TX_THREAD_ERROR and TX_DELETE_ERROR
        DBGMSG("[AppLib - Graphics] <AppLibDirectDraw_DeinitTask> %s:%u Failed to delete task (%d).", __FUNCTION__, __LINE__, Rval);
        ReturnValue = -1; // Error
    }

    /** Step 3: Release stack */
    if (ddStackRaw != NULL) {
        Rval = AmbaKAL_BytePoolFree(ddStackRaw);
        if (Rval != AMP_OK) {
            DBGMSG("[AppLib - Graphics] <AppLibDirectDraw_DeinitTask> %s:%u Failed to release stack (%d).", __FUNCTION__, __LINE__, Rval);
            ReturnValue = -1; // Error
        }
        ddStackRaw = NULL;
        ddStack = NULL;
    }

    /** Step 4: Delete message queue */
    if (AmpMsgQueue_IsCreated(&APPLIB_G_DD.ddMsgQueue)) {
        Rval = AmpMsgQueue_Delete(&APPLIB_G_DD.ddMsgQueue);
        if (Rval != AMP_MQ_SUCCESS) {
            DBGMSG("[AppLib - Graphics] <AppLibDirectDraw_DeinitTask> %s:%u Failed to delete message queue (%d).", __FUNCTION__, __LINE__, Rval);
            ReturnValue = -1; // Error
        }
    }

    /** Step 5: Release message pool */
    if (ddMsgPoolRaw != NULL) {
        if (AmbaKAL_BytePoolFree(ddMsgPoolRaw) != AMP_OK) {
            DBGMSG("[AppLib - Graphics] <AppLibDirectDraw_DeinitTask> %s:%u Failed to release message pool.", __FUNCTION__, __LINE__);
            ReturnValue = -1; // Error
        }
        ddMsgPoolRaw = NULL;
        APPLIB_G_DD.ddMsgPool = NULL;
    }

    DBGMSG("[AppLib - Graphics] <AppLibDirectDraw_DeinitTask> End");
    return ReturnValue;
}
