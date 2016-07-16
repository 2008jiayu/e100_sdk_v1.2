/**
 * @file src/app/connected/applib/src/format/ApplibMuxerManager.c
 *
 * Exif Muxer manager implementation (for sport cam APP only)
 *
 * History:
 *    2014/01/20 - [Martin Lai] created file
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
#include <format/Muxer.h>
#include <wchar.h>
#include "../AppLibTask_Priority.h"

//#define DEBUG_APPLIB_MUX_MGR
#if defined(DEBUG_APPLIB_MUX_MGR)
#define DBGMSG AmbaPrint
#define DBGMSGc(x) AmbaPrintColor(GREEN,x)
#define DBGMSGc2 AmbaPrintColor
#else
#define DBGMSG(...)
#define DBGMSGc(...)
#define DBGMSGc2(...)
#endif

#define MUX_MGR_STACK_SIZE    (0x8000)
#define MUX_MGR_NAME          "AppLib_Muxer_Manager"
#define MUX_MGR_MSGQUEUE_SIZE (128)

typedef struct _APPLIB_MUX_MGRMESSAGE_s_ {
    UINT32 MessageID;
    UINT32 MessageData[2];
} APPLIB_MUX_MGRMESSAGE_s;



#define APPLIB_MAX_MUXER_HANDLER    8
typedef struct _MUX_MGR_s_ {
    UINT8 Stack[MUX_MGR_STACK_SIZE];  /**< Stack */
    UINT8 MsgPool[sizeof(APPLIB_MUX_MGRMESSAGE_s)*MUX_MGR_MSGQUEUE_SIZE];   /**< Message memory pool. */
    AMBA_KAL_TASK_t Task;               /**< Task ID */
    AMBA_KAL_MSG_QUEUE_t MsgQueue;      /**< Message queue ID */
    AMBA_KAL_SEM_t Sem;      /**< Flag ID*/
    APPLIB_MUX_MGR_HANDLER_s Handler[APPLIB_MAX_MUXER_HANDLER];
} MUX_MGR_s;

/** Global instance of manager */
static MUX_MGR_s G_muxmgr = {0};
static int ApplibFormatMuxerMgrInitFlag = -1;
static int MuxerHandlerCont = 0;
static UINT16 StillDataReadyNum = 0;
/**
 *  @brief Send message to muxer manager.
 *
 *  Send message to muxer manager.
 *
 *  @param [in] msg Message ID
 *  @param [in] param1 first parameter
 *  @param [in] param2 second parameter
 *
 *  @return >=0 success, <0 failure
 */
static int AppLibFormatMuxMgr_SndMsg(UINT32 msg, UINT32 param1, UINT32 param2)
{
    int ReturnValue = 0;
    APPLIB_MUX_MGRMESSAGE_s TempMessage = {0};

    TempMessage.MessageID = msg;
    TempMessage.MessageData[0] = param1;
    TempMessage.MessageData[1] = param2;

    ReturnValue = AmbaKAL_MsgQueueSend(&G_muxmgr.MsgQueue, &TempMessage, AMBA_KAL_NO_WAIT);
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
static int AppLibFormatMuxMgr_RcvMsg(APPLIB_MUX_MGRMESSAGE_s *msg, UINT32 waitOption)
{
    int ReturnValue = 0;

    ReturnValue = AmbaKAL_MsgQueueReceive(&G_muxmgr.MsgQueue, (void *)msg, waitOption);
    //AmbaPrint("RcvMsg.MessageID = 0x%x ReturnValue = %d", msg->MessageID, ReturnValue);

    return ReturnValue;
}


/**
 *  @brief Register Muxer Handler
 *
 *  Register Muxer Handler
 *
 *  @param [in] muxerHandler Muxer Handler.
 *
 *  @return >=0 success, <0 failure
 */
int AppLibFormatMuxMgr_RegMuxHandler(APPLIB_MUX_MGR_HANDLER_s *muxerHandler)
{
    int ReturnValue = -1;
    for (int i=0;i < MuxerHandlerCont; i++) {
        if (muxerHandler->Type == G_muxmgr.Handler[i].Type) {
            if (G_muxmgr.Handler[i].Used == 1) {
                AmbaPrintColor(RED, "[Applib - MuxMgr] Handler for type %d already register",G_muxmgr.Handler[i].Type);
            } else {
                G_muxmgr.Handler[i].Used = 1;
            }
            return ReturnValue;
        }
    }

    if (MuxerHandlerCont < APPLIB_MAX_MUXER_HANDLER) {
        memcpy(&G_muxmgr.Handler[MuxerHandlerCont], muxerHandler, sizeof(APPLIB_MUX_MGR_HANDLER_s));
        MuxerHandlerCont++;
    } else {
        AmbaPrintColor(RED,"[Applib - MuxMgr] <RegMuxHandler> Error: MuxerHandlerCont = %d (Should be less than %d)", MuxerHandlerCont, APPLIB_MAX_MUXER_HANDLER);
    }

    return ReturnValue;
}


/**
 *  @brief Unregister Muxer Handler
 *
 *  Unregister Muxer Handler
 *
 *  @param [in] muxerHandler Muxer Handler.
 *
 *  @return >=0 success, <0 failure
 */
int AppLibFormatMuxMgr_UnRegMuxHandler(APPLIB_MUX_MGR_HANDLER_s *muxerHandler)
{
    int ReturnValue = -1;
    for (int i=0;i < MuxerHandlerCont; i++) {
        if (muxerHandler->Type == G_muxmgr.Handler[i].Type && G_muxmgr.Handler[i].Used == 1) {
                AmbaPrintColor(RED, "[Applib - MuxMgr] Unregister handler for type %d ",G_muxmgr.Handler[i].Type);
                G_muxmgr.Handler[i].Used = 0;
                return ReturnValue;
        }
    }

    AmbaPrintColor(RED,"[Applib - MuxMgr] <RegMuxHandler> Handler for type %d is not register", muxerHandler->Type);

    return ReturnValue;
}


/**
 *  @brief Initialize the FIFO of muxer.
 *
 *  Initialize the FIFO of muxer.
 *
 *  @param [in] muxerType Muxer type
 *
 *  @return >=0 success, <0 failure
 */
int AppLibFormatMuxMgr_MuxInit(UINT32 muxerType)
{
    int ReturnValue = -1;
    DBGMSG("[Applib - MuxMgr] AppLibFormatMuxMgr_MuxInit");
    for (int i = 0; i < MuxerHandlerCont; i++) {
        if (G_muxmgr.Handler[i].Type == muxerType && G_muxmgr.Handler[i].Used == 1) {
            if (G_muxmgr.Handler[i].MuxerInit != NULL) {
                ReturnValue = G_muxmgr.Handler[i].MuxerInit();
		   if (ReturnValue != OK) {
                    AmbaPrintColor(RED, "[Applib - MuxMgr] AppLibFormatMuxMgr_MuxInit Fail");
                } else {
                    return ReturnValue;
                }
            } else {
                    AmbaPrintColor(RED, "[Applib - MuxMgr] AppLibFormatMuxMgr_MuxInit = NULL");
            }
        }
    }
    AmbaPrintColor(RED, "[Applib - MuxMgr] Muxer handler for type %d = NULL",muxerType);
    return ReturnValue;
}

/**
 *  @brief Open the muxer.
 *
 *  Open the muxer.
 *
 *  @param [in] muxerType Muxer type
 *
 *  @return >=0 success, <0 failure
 */
int AppLibFormatMuxMgr_MuxOpen(UINT32 muxerType)
{
    int ReturnValue = -1;
    DBGMSG("[Applib - MuxMgr] AppLibFormatMuxMgr_MuxOpen");
    for (int i = 0; i < MuxerHandlerCont; i++) {
        if (G_muxmgr.Handler[i].Type == muxerType && G_muxmgr.Handler[i].Used == 1) {
            if (G_muxmgr.Handler[i].MuxerOpen != NULL) {
                ReturnValue = G_muxmgr.Handler[i].MuxerOpen();
                if (G_muxmgr.Handler[i].Type == STILL_MUXER_HANDLER || G_muxmgr.Handler[i].Type == PIV_MUXER_HANDLER) {
                    StillDataReadyNum = G_muxmgr.Handler[i].DataReadyNum;
                }
		   if (ReturnValue != OK) {
                    switch (ReturnValue) {
                        case STORAGE_DMF_MKDIR_FAIL:
                        case STORAGE_DMF_UNREACHABLE:
                        case STORAGE_DMF_DIR_IDX_REACH_LIMIT:
                        case STORAGE_DMF_RTC_IDX_REACH_LIMIT:
                        case STORAGE_DMF_INCORRECT_EXT_FILE_TYPE:
                        case STORAGE_DMF_DIR_NAME_ILLEGAL:
                            AppLibComSvcHcmgr_SendMsgNoWait(HMSG_MUXER_IO_ERROR, 0, 0);
                            break;
                    }
                    AmbaPrintColor(RED, "[Applib - MuxMgr] AppLibFormatMuxMgr_MuxOpen Fail. ReturnValue: %d", ReturnValue);
                } else {
                    return ReturnValue;
                }
            } else {
                    AmbaPrintColor(RED, "[Applib - MuxMgr] AppLibFormatMuxMgr_MuxOpen = NULL");
            }
        }
    }
    AmbaPrintColor(RED, "[Applib - MuxMgr] Muxer handler for type %d = NULL",muxerType);
    return ReturnValue;
}

/**
 *  @brief Close the muxer.
 *
 *  Close the muxer.
 *
 *  @param [in] muxerType Muxer type
 *
 *  @return >=0 success, <0 failure
 */
int AppLibFormatMuxMgr_MuxClose(UINT32 muxerType)
{
    int ReturnValue = -1;
    DBGMSG("[Applib - MuxMgr] AppLibFormatMuxMgr_MuxClose");
    for (int i = 0; i < MuxerHandlerCont; i++) {
        if (G_muxmgr.Handler[i].Type == muxerType && G_muxmgr.Handler[i].Used == 1) {
            if (G_muxmgr.Handler[i].MuxerClose != NULL) {
                ReturnValue = G_muxmgr.Handler[i].MuxerClose();
                if (ReturnValue != OK) {
                    AmbaPrintColor(RED, "[Applib - MuxMgr] AppLibFormatMuxMgr_MuxClose Fail");
                } else {
                    return ReturnValue;
                }
            } else {
                AmbaPrintColor(RED, "[Applib - MuxMgr] AppLibFormatMuxMgr_MuxClose = NULL");
            }
        }
    }
    AmbaPrintColor(RED, "[Applib - MuxMgr] Muxer handler for type %d = NULL",muxerType);
    return ReturnValue;
}


#define MUXMGR_AVAILABLE         0x1
#define JPEG_MAX_NUM    3
static UINT32 MuxerCounter = 0;

/**
 *  @brief Task of muxer manager.
 *
 *  Task of muxer manager.
 *
 *  @param [in] info information
 *
 */
static void AppLibFormatMuxMgr_MgrTask(UINT32 info)
{
    int ReturnValue = 0;
    APPLIB_MUX_MGRMESSAGE_s Msg = {0};
    DBGMSG("[Applib - MuxMgr] Manager ready");

    while (1) {
        AppLibFormatMuxMgr_RcvMsg(&Msg, AMBA_KAL_WAIT_FOREVER);
        DBGMSG("[Applib - MuxMgr] Received msg: 0x%X (Param1 = 0x%X / Param2 = 0x%X)", Msg.MessageID, Msg.MessageData[0], Msg.MessageData[1]);

        switch (Msg.MessageID) {
        case APPLIB_MUX_MGR_EVENT_STILL_DATA_READY:
            if (MuxerCounter == 0) {
                ReturnValue = AppLibFormatMuxMgr_MuxOpen(Msg.MessageData[1]);
            }
            ReturnValue = AmpMuxer_OnDataReady((AMP_FIFO_HDLR_s *)Msg.MessageData[0]);
            MuxerCounter ++;
            DBGMSGc2(GREEN,"[Applib - MuxMgr] msg 0x%X is doing (ReturnValue = %d / Msg.MessageData[0] = 0x%x)", Msg.MessageID, ReturnValue, Msg.MessageData[0]);
            if (MuxerCounter == StillDataReadyNum) {
                AmbaKAL_SemTake(&G_muxmgr.Sem, AMBA_KAL_WAIT_FOREVER);
                MuxerCounter = 0;
            }
            DBGMSG("[Applib - MuxMgr] msg 0x%X is done (ReturnValue = %d / Msg.MessageData[0] = 0x%x)", Msg.MessageID, ReturnValue, Msg.MessageData[0]);
            break;
        case APPLIB_MUX_MGR_EVENT_VIDEO_DATA_READY:
            ReturnValue = AppLibFormatMuxMgr_MuxOpen(VIDEO_MUXER_HANDLER);
            ReturnValue = AmpMuxer_OnDataReady((AMP_FIFO_HDLR_s *)Msg.MessageData[0]);
            DBGMSG("[Applib - MuxMgr] msg 0x%X is done (ReturnValue = %d / Msg.MessageData[0] = 0x%x)", Msg.MessageID, ReturnValue, Msg.MessageData[0]);
            break;
        case APPLIB_MUX_MGR_EVENT_VIDEO_DATA_READY_EVENTRECORD:
            ReturnValue = AppLibFormatMuxMgr_MuxOpen(VIDEO_EVENTRECORD_MUXER_HANDLER);
            ReturnValue = AmpMuxer_OnDataReady((AMP_FIFO_HDLR_s *)Msg.MessageData[0]);
            DBGMSG("[Applib - MuxMgr] msg 0x%X is done (ReturnValue = %d / Msg.MessageData[0] = 0x%x)", Msg.MessageID, ReturnValue, Msg.MessageData[0]);
            break;
        case APPLIB_MUX_MGR_EVENT_VIDEO_DATA_EOS:
            ReturnValue = AmpMuxer_OnEOS((AMP_FIFO_HDLR_s *)Msg.MessageData[0]);
            break;
        default:
            break;
        }
    }
}


/**
 *  @brief Initialization of muxer manager.
 *
 *   Initialization of muxer manager.
 *
 *  @return >=0 success, <0 failure
 */
int AppLibFormatMuxMgr_Init(void)
{
    int ReturnValue = 0;

    if (ApplibFormatMuxerMgrInitFlag == 0)
        return 0;
    DBGMSG("[Applib - AsyncOp] <AppLibFormatMuxMgr_init> start");

    /* Clear G_muxmgr */
    memset(&G_muxmgr, 0, sizeof(MUX_MGR_s));

    /* Create message queue */
    ReturnValue = AmbaKAL_MsgQueueCreate(&G_muxmgr.MsgQueue, G_muxmgr.MsgPool, sizeof(APPLIB_MUX_MGRMESSAGE_s), MUX_MGR_MSGQUEUE_SIZE);
    if (ReturnValue == OK) {
        DBGMSGc2(GREEN, "[Applib - MuxMgr] Create Queue success = %d", ReturnValue);
    } else {
        AmbaPrintColor(RED, "[Applib - MuxMgr] Create Queue fail = %d", ReturnValue);
    }

    /* Create semaphore.*/
    ReturnValue = AmbaKAL_SemCreate(&G_muxmgr.Sem, 0);
    if (ReturnValue == OK) {
        DBGMSGc2(GREEN, "[Applib - MuxMgr] Create semaphore success = %d", ReturnValue);
    } else {
        AmbaPrintColor(RED, "[Applib - MuxMgr] Create semaphore fail = %d", ReturnValue);
    }

    /* Create Host Control Manager task*/
    ReturnValue = AmbaKAL_TaskCreate(&G_muxmgr.Task, /* pTask */
        MUX_MGR_NAME, /* pTaskName */
        APPLIB_MUXER_MGR_TASK_PRIORITY, /* Priority */
        AppLibFormatMuxMgr_MgrTask, /* void (*EntryFunction)(UINT32) */
        0x0, /* EntryArg */
        (void *) G_muxmgr.Stack, /* pStackBase */
        MUX_MGR_STACK_SIZE, /* StackByteSize */
        AMBA_KAL_AUTO_START); /* AutoStart */
    if (ReturnValue != OK) {
        AmbaPrintColor(RED, "[Applib - MuxMgr] Create task fail = %d", ReturnValue);
    }
    ApplibFormatMuxerMgrInitFlag = 0;
    DBGMSG("[Applib - MuxMgr] <AppLibFormatMuxMgr_init> end: ReturnValue = %d", ReturnValue);

    return ReturnValue;
}

/**
 *  @brief Receive the data ready event
 *
 *  Receive the data ready event
 *
 *  @param [in] handler Handler
 *  @param [in] info Information
 *
 *  @return >=0 success, <0 failure
 */
int AppLibFormatMuxMgr_DataReady(void* handler, void* info, UINT32 Type)
{
    int ReturnValue = -1;
    if (Type == STILL_MUXER_HANDLER || Type == PIV_MUXER_HANDLER) {
        AppLibFormatMuxMgr_SndMsg(APPLIB_MUX_MGR_EVENT_STILL_DATA_READY, (UINT32)handler, Type);
    } else if (Type == VIDEO_MUXER_HANDLER){
        //AmbaPrint("[DEBUG]AppLibFormatMuxMgr_DataReady video");
        AppLibFormatMuxMgr_SndMsg(APPLIB_MUX_MGR_EVENT_VIDEO_DATA_READY, (UINT32)handler, (UINT32)info);
    } else {
        AppLibFormatMuxMgr_SndMsg(APPLIB_MUX_MGR_EVENT_VIDEO_DATA_READY_EVENTRECORD, (UINT32)handler, (UINT32)info);
    }
    return ReturnValue;
}

/**
 *  @brief Receive the data EOS event
 *
 *  Receive the data EOS event
 *
 *  @param [in] handler Handler
 *  @param [in] info Information
 *
 *  @return >=0 success, <0 failure
 */
int AppLibFormatMuxMgr_DataEos(void* handler, void* info)
{
    int ReturnValue = -1;
    AppLibFormatMuxMgr_SndMsg(APPLIB_MUX_MGR_EVENT_VIDEO_DATA_EOS, (UINT32)handler, (UINT32)info);
    return ReturnValue;
}

/**
 *  @brief Muxer start
 *
 *  Muxer start
 *
 *  @param [in] handler FIFO handler
 *  @param [in] info information
 *
 *  @return >=0 success, <0 failure
 */
int AppLibFormatMuxMgr_MuxStart(void* handler, void* info)
{
    int ReturnValue = -1;
    //AppLibFormatMuxMgr_SndMsg(APPLIB_MUX_MGR_EVENT_START, (UINT32)handler, (UINT32)info);
    return ReturnValue;
}

/**
 *  @brief remove semaphore after muxer close
 *
 *  remove semaphore after muxer close
 *
 *
 *  @return >=0 success, <0 failure
 */
int AppLibFormatMuxMgr_MuxEnd()
{
    int ReturnValue = -1;
    //AmbaKAL_EventFlagGive(&G_muxmgr.EventFlag, MUXMGR_AVAILABLE);
    AmbaKAL_SemGive(&G_muxmgr.Sem);
    //AppLibFormatMuxMgr_SndMsg(APPLIB_MUX_MGR_EVENT_STOP, (UINT32)handler, (UINT32)info);
    return ReturnValue;
}

