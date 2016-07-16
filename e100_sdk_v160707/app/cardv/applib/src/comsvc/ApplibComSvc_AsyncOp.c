/**
 * @file src/app/connected/applib/src/comsvc/ApplibComSvc_AsyncOp.c
 *
 * Implementation of Async Operation - APP level
 *
 * History:
 *    2013/09/09 - [Martin Lai] created file
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
#include <wchar.h>
#include <cfs/AmpCfs.h>
#include <AmbaUtility.h>
#include "../AppLibTask_Priority.h"
#ifdef CONFIG_APP_ARD
#include <calibration/ApplibCalibCli.h>
extern int AppLibCA_UTFunc(int Argc, char *Argv[]);
extern int AppLibWarp_UTFunc(int Argc, char *Argv[]);
#endif

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

/*************************************************************************
 * Async OP definitons
 ************************************************************************/

#define ASYNC_MGR_STACK_SIZE    (0x3800)
#define ASYNC_MGR_NAME          "AppLib_Async_Operation_Manager"
#define ASYNC_MGR_MSGQUEUE_SIZE (16)

typedef struct _APP_ASYNC_MGR_MESSAGE_s_ {
    UINT32 MessageID;
    UINT32 MessageData[2];
} APP_ASYNC_MGR_MESSAGE_s;

typedef struct _ASYNC_MGR_s_ {
    UINT8 Stack[ASYNC_MGR_STACK_SIZE];  /**< Stack */
    UINT8 MsgPool[sizeof(APP_ASYNC_MGR_MESSAGE_s)*ASYNC_MGR_MSGQUEUE_SIZE];   /**< Message memory pool. */
    AMBA_KAL_TASK_t Task;               /**< Task ID */
    AMBA_KAL_MSG_QUEUE_t MsgQueue;      /**< Message queue ID */
} ASYNC_MGR_s;

/** Global instance of Async OP manager */
static ASYNC_MGR_s G_asyncmgr = {0};

#define ASYNC_FILE_OP_MAX    (ASYNC_MGR_MSGQUEUE_SIZE)
#define ASYNC_FILE_LEN_MAX    (APP_MAX_FN_SIZE)
typedef struct _APP_ASYNC_MGR_FILE_OP_ITEM_s_ {
    UINT32 DmfRootType;
    UINT32 handler;
    char SrcFn[ASYNC_FILE_LEN_MAX];
    char DstFn[ASYNC_FILE_LEN_MAX];
    int(* GetPartNum)(int, void *);
    int(* ProgramStatusReport)(int, void *);
} APP_ASYNC_MGR_FILE_OP_ITEM_s;

static APP_ASYNC_MGR_FILE_OP_ITEM_s *async_file_op_items = NULL;
static UINT8* AsyncFileOpItemMem = NULL;
static void* AsyncFileOpItemMemBufRaw = NULL;

#ifdef CONFIG_APP_ARD
static char cali_script[8][64] = {"blc.txt", \
                           "bad.txt", \
                           "ca.txt", \
                           "warp.txt", \
                           "vig.txt", \
                           "wb-golden-sample.txt", \
                           "wbh.txt", \
                           "wbl.txt"};
typedef enum _ASYNC_OP_MANUAL_CALI_e_ {
    ASYNC_OP_MANUAL_CALI_BLC = 0,
    ASYNC_OP_MANUAL_CALI_BAD_PIXEL,
    ASYNC_OP_MANUAL_CALI_CHROMA_ABERRATION,
    ASYNC_OP_MANUAL_CALI_WARP,
    ASYNC_OP_MANUAL_CALI_VIGNETTE,
    ASYNC_OP_MANUAL_CALI_WB_GOLDEN_SAMPLE,
    ASYNC_OP_MANUAL_CALI_WBH,
    ASYNC_OP_MANUAL_CALI_WBL,
} ASYNC_OP_MANUAL_CALI_e;
char* ca_cmd[5] = {"cal",
                   "ca",
                   "ca_spec",
                   "0",
                   "",};
char* warp_cmd[5] = {"cal",
                     "warp",
                     "warp_spec",
                     "0",
                     "",};
#endif
/*************************************************************************
 * Async OP APIs - Task
 ************************************************************************/

/**
 *  @brief Get empty item for file operation
 *
 *  Get empty item for file operation
 *
 *  @return >=0 valid item, <0 failure
 */
static int AppLibComSvcAsyncOp_FileOpGetEmptyItem(void)
{
    int i = 0;
    for (i=0; i<ASYNC_FILE_OP_MAX; i++) {
        if (async_file_op_items[i].SrcFn[0] == '\0') {
            return i;
        }
    }
    DBGMSG("[Applib - AsyncOp] No empty file op item found");
    return -1;
}


/**
 *  @brief Send message to async task
 *
 *  Send message to async task
 *
 *  @param [in] msg Message ID
 *  @param [in] param1 first parameter
 *  @param [in] param2 second parameter
 *
 *  @return >=0 success, <0 failure
 */
static int AppLibComSvcAsyncOp_SndMsg(UINT32 msg, UINT32 param1, UINT32 param2)
{
    int ReturnValue = 0;
    APP_ASYNC_MGR_MESSAGE_s TempMessage = {0};

    TempMessage.MessageID = msg;
    TempMessage.MessageData[0] = param1;
    TempMessage.MessageData[1] = param2;

    ReturnValue = AmbaKAL_MsgQueueSend(&G_asyncmgr.MsgQueue, &TempMessage, AMBA_KAL_NO_WAIT);
    //DBGMSG("SndMsg.MessageID = 0x%x ReturnValue = %d", msg->MessageID, ReturnValue);

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
static int AppLibComSvcAsyncOp_RcvMsg(APP_ASYNC_MGR_MESSAGE_s *msg, UINT32 waitOption)
{
    int ReturnValue = 0;

    ReturnValue = AmbaKAL_MsgQueueReceive(&G_asyncmgr.MsgQueue, (void *)msg, waitOption);
    //DBGMSG("RcvMsg.MessageID = 0x%x ReturnValue = %d", msg->MessageID, ReturnValue);

    return ReturnValue;
}


/**
 *  @brief Task of Async operation.
 *
 *  Task of Async operation.
 *
 *  @param [in] info information
 *
 */
static void AppLibComSvcAsyncOp_MgrTask(UINT32 info)
{
    int ReturnValue = 0;
    UINT32 Param1 = 0, Param2 = 0;
    APP_ASYNC_MGR_MESSAGE_s Msg = {0};
    int ReturnInfo = 0;
    DBGMSG("[Applib - AsyncOp] Async manager ready");

    while (1) {
        AppLibComSvcAsyncOp_RcvMsg(&Msg, AMBA_KAL_WAIT_FOREVER);
        Param1 = Msg.MessageData[0];
        Param2 = Msg.MessageData[1];
        DBGMSG("[Applib - AsyncOp] Received msg: 0x%X (Param1 = 0x%X / Param2 = 0x%X)", Msg.MessageID, Param1, Param2);

        ReturnInfo = 0;

        switch (Msg.MessageID) {
        case ASYNC_MGR_CMD_SHUTDOWN:
            AmbaPrintColor(RED, "[Applib - AsyncOp]  The function \"Shutdwon\" is not ready. ");
            break;
        case ASYNC_MGR_CMD_CARD_FORMAT:
            ReturnValue = AppLibCard_Format(Param1);
#ifdef CONFIG_APP_ARD
            {
            UINT8 Redo;
            for(Redo=0;Redo<3;Redo++){
                if(ReturnValue < 0){
                    AmbaKAL_TaskSleep(500);
                    ReturnValue = AppLibCard_Format(Param1);
                    if(ReturnValue>=0){
                        AmbaPrintColor(GREEN,"retry format OK");
                    }
                }else{
                    break;
                }
            }
            }
#endif
            ReturnInfo = Param1;
            break;
        case ASYNC_MGR_CMD_CARD_INSERT:
            ReturnValue = AppLibCard_Insert(Param1);
            ReturnInfo = Param1;
            break;
        case ASYNC_MGR_CMD_FILE_COPY:
            AmbaPrintColor(RED, "[Applib - AsyncOp]  The function \"Copy file\" is not ready. ");
            memset(&async_file_op_items[Param1], 0, sizeof(APP_ASYNC_MGR_FILE_OP_ITEM_s));
            break;
        case ASYNC_MGR_CMD_FILE_MOVE:
            ReturnValue = AmpCFS_Move(async_file_op_items[Param1].SrcFn, async_file_op_items[Param1].DstFn);
            memset(&async_file_op_items[Param1], 0, sizeof(APP_ASYNC_MGR_FILE_OP_ITEM_s));
            break;
        case ASYNC_MGR_CMD_FILE_DEL:
            ReturnValue = AmpCFS_remove(async_file_op_items[Param1].SrcFn);
            memset(&async_file_op_items[Param1], 0, sizeof(APP_ASYNC_MGR_FILE_OP_ITEM_s));
            break;
        case ASYNC_MGR_CMD_DMF_FCOPY:
            AmbaPrintColor(RED, "[Applib - AsyncOp]  The function \"Firmware Update\" is not ready. ");
            memset(&async_file_op_items[Param1], 0, sizeof(APP_ASYNC_MGR_FILE_OP_ITEM_s));
            break;
        case ASYNC_MGR_CMD_DMF_FMOVE:
            ReturnValue = AmpCFS_Move(async_file_op_items[Param1].SrcFn, async_file_op_items[Param1].DstFn);
            memset(&async_file_op_items[Param1], 0, sizeof(APP_ASYNC_MGR_FILE_OP_ITEM_s));
            break;
        case ASYNC_MGR_CMD_DMF_FDEL:
            ReturnValue = AppLibStorageDmf_DeleteFile(async_file_op_items[Param1].DmfRootType,Param2, async_file_op_items[Param1].handler);
            memset(&async_file_op_items[Param1], 0, sizeof(APP_ASYNC_MGR_FILE_OP_ITEM_s));
            break;
        case ASYNC_MGR_CMD_DMF_FAST_FDEL_ALL:
#ifdef CONFIG_APP_ARD
			ReturnValue = AppLibStorageDmf_DeleteFileAll(Param1,Param2);
			memset(&async_file_op_items[Param1], 0, sizeof(APP_ASYNC_MGR_FILE_OP_ITEM_s));
#else
			AmbaPrintColor(RED, "[Applib - AsyncOp]  The function \"Delete all file\" is not ready. ");
#endif
            break;
        case ASYNC_MGR_CMD_CALIB_LOAD_DATA:
            AmbaPrintColor(RED, "[Applib - AsyncOp]  The function \"Load calibration data\" is not ready. ");
            break;
#ifdef CONFIG_APP_ARD
        case ASYNC_MGR_CMD_DO_CALIBRATION:
        {
            int ReturnValue = -1;
            char Drive = 'A';
            char Script[64] = "I:\\factory_mode\\";

            Drive = AppLibCard_GetActiveDrive();
            Script[0] = (char)Drive;
            strcat(Script, cali_script[Param1]);

            switch(Param1) {
                case ASYNC_OP_MANUAL_CALI_BLC:
                case ASYNC_OP_MANUAL_CALI_BAD_PIXEL:
                case ASYNC_OP_MANUAL_CALI_VIGNETTE:
                case ASYNC_OP_MANUAL_CALI_WB_GOLDEN_SAMPLE:
                case ASYNC_OP_MANUAL_CALI_WBH:
                case ASYNC_OP_MANUAL_CALI_WBL:
                    ReturnValue = AppLib_CalibPathIf(Script);
                    if(-1 == ReturnValue) {
                        AmbaPrint("%s, %s, %d", __FILE__, __func__, __LINE__);
                    }
                    break;
                case ASYNC_OP_MANUAL_CALI_CHROMA_ABERRATION:
                    ca_cmd[4] = Script;
                    AppLibCA_UTFunc(5, ca_cmd);
                    break;
                case ASYNC_OP_MANUAL_CALI_WARP:
                    warp_cmd[4] = Script;
                    AppLibWarp_UTFunc(5, warp_cmd);
                    break;
                default:
                    AmbaPrint("%s, %s, %d", __FILE__, __func__, __LINE__);
                    break;
            }
            break;
        }
#endif
        default:
            break;
        }

        DBGMSG("[Async] msg 0x%X is done (ReturnValue = %d / ReturnInfo = %d)", Msg.MessageID, ReturnValue, ReturnInfo);

        AppLibComSvcHcmgr_SendMsg(ASYNC_MGR_MSG_OP_DONE(Msg.MessageID), ReturnValue, ReturnInfo);

    }
}


/**
 *  @brief Initialization of Async operation.
 *
 *   Initialization of Async operation.
 *
 *  @return >=0 success, <0 failure
 */
int AppLibComSvcAsyncOp_Init(void)
{
    int ReturnValue = 0;

    DBGMSG("[Applib - AsyncOp] <AppLibComSvcAsyncOp_init> start");

    /* Clear G_asyncmgr */
    memset(&G_asyncmgr, 0, sizeof(ASYNC_MGR_s));

    /* Create App message queue */
    ReturnValue = AmbaKAL_MsgQueueCreate(&G_asyncmgr.MsgQueue, G_asyncmgr.MsgPool, sizeof(APP_ASYNC_MGR_MESSAGE_s), ASYNC_MGR_MSGQUEUE_SIZE);
    if (ReturnValue == OK) {
        DBGMSGc2(GREEN, "[Applib - AsyncOp]Create Queue success = %d", ReturnValue);
    } else {
        AmbaPrintColor(RED, "[Applib - AsyncOp]Create Queue fail = %d", ReturnValue);
    }
    /* Create Host Control Manager task*/
    ReturnValue = AmbaKAL_TaskCreate(&G_asyncmgr.Task, /* pTask */
        ASYNC_MGR_NAME, /* pTaskName */
        APPLIB_COMSVC_ASYNC_OP_TASK_PRIORITY, /* Priority */
        AppLibComSvcAsyncOp_MgrTask, /* void (*EntryFunction)(UINT32) */
        0x0, /* EntryArg */
        (void *) G_asyncmgr.Stack, /* pStackBase */
        ASYNC_MGR_STACK_SIZE, /* StackByteSize */
        AMBA_KAL_AUTO_START); /* AutoStart */
    if (ReturnValue != OK) {
        AmbaPrintColor(RED, "[Applib - AsyncOp]Create task fail = %d", ReturnValue);
    }

    ReturnValue = AmpUtil_GetAlignedPool(APPLIB_G_MMPL, (void **)&AsyncFileOpItemMem, &AsyncFileOpItemMemBufRaw, ASYNC_FILE_OP_MAX*sizeof(APP_ASYNC_MGR_FILE_OP_ITEM_s), 32);
    if (ReturnValue < 0 ) {
        AmbaPrintColor(RED, "%s:%u", __FUNCTION__, __LINE__);
        return ReturnValue;
    }
    memset(AsyncFileOpItemMem, 0, ASYNC_FILE_OP_MAX*sizeof(APP_ASYNC_MGR_FILE_OP_ITEM_s));
    async_file_op_items = (APP_ASYNC_MGR_FILE_OP_ITEM_s *)AsyncFileOpItemMem;

    DBGMSG("[Applib - AsyncOp] <AppLibComSvcAsyncOp_init> end: ReturnValue = %d", ReturnValue);

    return ReturnValue;
}


/*************************************************************************
 * Async OP APIs - Command entry
 ************************************************************************/

/**
 *  @brief To shut down system
 *
 *  To shut down system.
 *
 *  @return >=0 success, <0 failure
 */
int AppLibComSvcAsyncOp_Shutdown(void)
{
    return AppLibComSvcAsyncOp_SndMsg(ASYNC_MGR_CMD_SHUTDOWN, 0, 0);
}

/**
 *  @brief To format card
 *
 *  To format card.
 *
 *  @param [in] slot Card slot id
 *
 *  @return >=0 success, <0 failure
 */
int AppLibComSvcAsyncOp_CardFormat(int slot)
{
    return AppLibComSvcAsyncOp_SndMsg(ASYNC_MGR_CMD_CARD_FORMAT, slot, 0);
}

/**
 *  @brief To insert card
 *
 *  To insert card.
 *
 *  @param [in] slot Card slot id
 *
 *  @return >=0 success, <0 failure
 */
int AppLibComSvcAsyncOp_CardInsert(int slot)
{
    return AppLibComSvcAsyncOp_SndMsg(ASYNC_MGR_CMD_CARD_INSERT, slot, 0);
}

/**
 *  @brief To copy the file
 *
 *  To copy the file
 *
 *  @param [in] srcFn Source file name
 *  @param [in] dstFn destination file name
 *
 *  @return >=0 success, <0 failure
 */
int AppLibComSvcAsyncOp_FileCopy(char *srcFn, char *dstFn)
{
    int ReturnValue = -1;
    ReturnValue = AppLibComSvcAsyncOp_FileOpGetEmptyItem();
    if (ReturnValue >= 0) {
        strcpy(async_file_op_items[ReturnValue].SrcFn, srcFn);
        strcpy(async_file_op_items[ReturnValue].DstFn, dstFn);
        AppLibComSvcAsyncOp_SndMsg(ASYNC_MGR_CMD_FILE_COPY, ReturnValue, 0);
    }
    return ReturnValue;
}

/**
 *  @brief To move the file
 *
 *  To move the file
 *
 *  @param [in] srcFn Source file name
 *  @param [in] dstFn Destination file name
 *
 *  @return >=0 success, <0 failure
 */
int AppLibComSvcAsyncOp_FileMove(char *srcFn, char *dstFn)
{
    int ReturnValue = -1;
    ReturnValue = AppLibComSvcAsyncOp_FileOpGetEmptyItem();
    if (ReturnValue >= 0) {
        strcpy(async_file_op_items[ReturnValue].SrcFn, srcFn);
        strcpy(async_file_op_items[ReturnValue].DstFn, dstFn);
        AppLibComSvcAsyncOp_SndMsg(ASYNC_MGR_CMD_FILE_MOVE, ReturnValue , 0);
    }
    return ReturnValue;
}

/**
 *  @brief To delete the file
 *
 *  To delete the file
 *
 *  @param [in] filename File name
 *
 *  @return >=0 success, <0 failure
 */
int AppLibComSvcAsyncOp_FileDel(char *filename)
{
    int ReturnValue = -1;
    ReturnValue = AppLibComSvcAsyncOp_FileOpGetEmptyItem();
    if (ReturnValue >= 0) {
        strcpy(async_file_op_items[ReturnValue].SrcFn, filename);
        AppLibComSvcAsyncOp_SndMsg(ASYNC_MGR_CMD_FILE_DEL, ReturnValue, 0);
    }
    return ReturnValue;
}

/**
 *  @brief To copy file with DMF rule
 *
 *  To copy file with DMF rule
 *
 *  @param [in] dmfRootType Media root type
 *  @param [in] srcFn source file name
 *  @param [in] dstFn destination file name
 *
 *  @return >=0 success, <0 failure
 */
int AppLibComSvcAsyncOp_DmfFcopy(int dmfRootType, char *srcFn, char *dstFn)
{
    int ReturnValue = -1;
    ReturnValue = AppLibComSvcAsyncOp_FileOpGetEmptyItem();
    if (ReturnValue >= 0) {
        async_file_op_items[ReturnValue].DmfRootType = dmfRootType;
        strcpy(async_file_op_items[ReturnValue].SrcFn, srcFn);
        strcpy(async_file_op_items[ReturnValue].DstFn, dstFn);
        AppLibComSvcAsyncOp_SndMsg(ASYNC_MGR_CMD_DMF_FCOPY, ReturnValue, 0);
    }
    return ReturnValue;
}


/**
 *  @brief Move file with DMF rule
 *
 *  Move file with DMF rule
 *
 *  @param [in] dmfRootType Media root type
 *  @param [in] srcFn source filename
 *  @param [in] dstFn destination filename
 *
 *  @return >=0 success, <0 failure
 */
int AppLibComSvcAsyncOp_DmfFmove(int dmfRootType, char *srcFn, char *dstFn)
{
    int ReturnValue = -1;
    ReturnValue = AppLibComSvcAsyncOp_FileOpGetEmptyItem();
    if (ReturnValue >= 0) {
        async_file_op_items[ReturnValue].DmfRootType = dmfRootType;
        strcpy(async_file_op_items[ReturnValue].SrcFn, srcFn);
        strcpy(async_file_op_items[ReturnValue].DstFn, dstFn);
        AppLibComSvcAsyncOp_SndMsg(ASYNC_MGR_CMD_DMF_FMOVE, ReturnValue, 0);
    }
    return ReturnValue;
}

/**
 *  @brief Delete file with DMF rule
 *
 *  Delete file with DMF rule
 *
 *  @param [in] dmfRootType Media root type
 *  @param [in] filename  filename
 *
 *  @return >=0 success, <0 failure
 */
int AppLibComSvcAsyncOp_DmfFdel(int dmfRootType, UINT32 FileObjID, UINT32 handler)
{
    int ReturnValue = -1;
    ReturnValue = AppLibComSvcAsyncOp_FileOpGetEmptyItem();
    if (ReturnValue >= 0) {
        async_file_op_items[ReturnValue].DmfRootType = dmfRootType;
        async_file_op_items[ReturnValue].handler= handler;
        AppLibComSvcAsyncOp_SndMsg(ASYNC_MGR_CMD_DMF_FDEL, ReturnValue, FileObjID);
    }
    return ReturnValue;
}

/**
 *  @brief Delete all files with DMF rule
 *
 *  Delete all files with DMF rule
 *
 *  @param [in] dmfRootType Media root type
 *  @param [in] param Parameter
 *
 *  @return >=0 success, <0 failure
 */
int AppLibComSvcAsyncOp_DmfFastFdelAll(int dmfRootType, UINT32 param)
{
    return AppLibComSvcAsyncOp_SndMsg(ASYNC_MGR_CMD_DMF_FAST_FDEL_ALL, dmfRootType, param);
}

/**
 *  @brief Load calibration data
 *
 *  Load calibration data
 *
 *  @param [in] stage Stage ID
 *
 *  @return >=0 success, <0 failure
 */
int AppLibComSvcAsyncOp_CalibLoadData(int stage)
{
    return AppLibComSvcAsyncOp_SndMsg(ASYNC_MGR_CMD_CALIB_LOAD_DATA, stage, 0);
}

#ifdef CONFIG_APP_ARD
/**
 *  @brief do calibration
 *
 *  do calibration
 *
 *  @param
 *
 *  @return >=0 success, <0 failure
 */
int AppLibComSvcAsyncOp_DoCalibration(int CaliId)
{
    return AppLibComSvcAsyncOp_SndMsg(ASYNC_MGR_CMD_DO_CALIBRATION, CaliId, 0);
}
#endif

