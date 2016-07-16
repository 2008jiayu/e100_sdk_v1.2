/**
 * @file src/app/connected/applib/src/recorder/AppLibEmptyTrackHandler
 *
 * Implementation of empty track clip Handle
 *
 * History:
 *    2015/09/9 - [QiangSu] created file
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
#include <AmbaUtility.h>
#include <fifo/Fifo.h>

//#define DEBUG_APPLIB_EMPTYTRACK
#if defined(DEBUG_APPLIB_EMPTYTRACK)
#define DBGMSG AmbaPrint
#define DBGMSGc(x) AmbaPrintColor(GREEN,x)
#define DBGMSGc2 AmbaPrintColor
#else
#define DBGMSG(...)
#define DBGMSGc(...)
#define DBGMSGc2(...)
#endif

#define APPLIB_EMPTYTRACKHDLR_MSGQUEUE_SIZE (8)
typedef struct _APPLIB_EMPTYTRACKHDLR_MGRMESSAGE_s_ {
    char FullName[APP_MAX_FN_SIZE];
} APPLIB_EMPTYTRACKHDLR_MGRMESSAGE_s;

typedef struct _APPLIB_EMPTYTRACKHDLR_CFG_s_ {
    UINT8 MsgPool[sizeof(APPLIB_EMPTYTRACKHDLR_MGRMESSAGE_s)*APPLIB_EMPTYTRACKHDLR_MSGQUEUE_SIZE];
    AMBA_KAL_MSG_QUEUE_t MsgQueue;      /**< Message queue ID */
} APPLIB_EMPTYTRACKHDLR_CFG_s;

static char* DebugMsg = "<EmptyTrackHandler>";
static APPLIB_EMPTYTRACKHDLR_CFG_s G_EmptyTrackHdlr = {0};
static int AppLibEmptyTrackHandlerInitFlag = -1;

int AppLibEmptyTrackHandler_AddFile(char* filename)
{
    int ReturnValue = 0;
    APPLIB_EMPTYTRACKHDLR_MGRMESSAGE_s TempMessage = {0};

    if(filename !=NULL){
        strncpy(TempMessage.FullName, filename, APP_MAX_FN_SIZE);
        ReturnValue = AmbaKAL_MsgQueueSend(&G_EmptyTrackHdlr.MsgQueue, &TempMessage, AMBA_KAL_NO_WAIT);
    }else{
        ReturnValue = -1;
    }

    return ReturnValue;
}

static int AppLibEmptyTrackHandler_RcvMsg(APPLIB_EMPTYTRACKHDLR_MGRMESSAGE_s *msg, UINT32 waitOption)
{
    int ReturnValue = 0;
    ReturnValue = AmbaKAL_MsgQueueReceive(&G_EmptyTrackHdlr.MsgQueue, (void *)msg, waitOption);
    return ReturnValue;
}

static int AppLibEmptyTrackHandler_DeleteFile(UINT32 param1, UINT32 param2)
{
    int ReturnValue = 0;
    int EmptyTrackFileObjId = AMP_ERROR_GENERAL_ERROR;
    INT32 FileNumToDel = (INT32)param2;
    APPLIB_EMPTYTRACKHDLR_MGRMESSAGE_s Msg = {0};
    char TmpFileName[APP_MAX_FN_SIZE];

    AmbaPrintColor(GREEN, "%s:start,FileNumToDel=%d",DebugMsg,FileNumToDel);
    while(1){

        /*No file need to delete*/
        if(FileNumToDel == 0){
            break;
        }
        if(AppLibEmptyTrackHandler_RcvMsg(&Msg, 1000) != AMP_OK){
            break;
        }

        EmptyTrackFileObjId = AppLibDCF_GetObjIdByName(Msg.FullName);
        if(AMP_ERROR_GENERAL_ERROR != EmptyTrackFileObjId){
            ReturnValue = AppLibStorageDmf_GetFileName(APPLIB_DCF_MEDIA_VIDEO, ".MP4", APPLIB_DCF_EXT_OBJECT_SPLIT_FILE, param1, 0, CUR_OBJ(EmptyTrackFileObjId), TmpFileName);
            if (ReturnValue < 0) {
                AmbaPrintColor(RED, "%s:file not exist",DebugMsg);
            }else{
                if (strcmp(Msg.FullName, TmpFileName) == 0) {
                    if(AppLibStorageDmf_DeleteFile(APPLIB_DCF_MEDIA_VIDEO,EmptyTrackFileObjId, param1) < 0){
                        ReturnValue = -1;
                        AmbaPrintColor(RED, "%s:del:%s fail",DebugMsg,Msg.FullName);
                    }else{
                        ReturnValue = 0;
                        FileNumToDel--;
                        AmbaPrintColor(GREEN, "%s:del:%s ok",DebugMsg,Msg.FullName);
                    }
                }else{
                    ReturnValue = -1;
                    AmbaPrintColor(RED, "%s:file not match",DebugMsg);
                }
            }
        }else{
            AmbaPrintColor(RED, "%s:ObjId Err",DebugMsg);
            ReturnValue = -1;
        }
    }

    if(FileNumToDel != 0){
        AmbaPrintColor(RED, "%s:FileNumToDel Err",DebugMsg);
    }else{
        AmbaPrintColor(GREEN, "%s:Done",DebugMsg);
    }

    return ReturnValue;
}

int AppLibEmptyTrackHandler_Init(void)
{
    int ReturnValue = 0;
    APPLIB_ENCODE_HANDLER_s EmptyTrackHandler = {0};

    if (AppLibEmptyTrackHandlerInitFlag == 0){
        return 0;
    }

    DBGMSG("<AppLibEmptyTrackHandler_Init> start");

    /* Create message queue */
    ReturnValue = AmbaKAL_MsgQueueCreate(&G_EmptyTrackHdlr.MsgQueue, G_EmptyTrackHdlr.MsgPool, sizeof(APPLIB_EMPTYTRACKHDLR_MGRMESSAGE_s), APPLIB_EMPTYTRACKHDLR_MSGQUEUE_SIZE);
    if (ReturnValue == OK) {
        DBGMSGc2(GREEN, "%s: Create Queue success = %d", DebugMsg,ReturnValue);
    } else {
        AmbaPrintColor(RED, "%s: Create Queue fail = %d", DebugMsg,ReturnValue);
    }

    /**register handler*/
    EmptyTrackHandler.FuncSearch = NULL;
    EmptyTrackHandler.FuncHandle = &AppLibEmptyTrackHandler_DeleteFile;
    EmptyTrackHandler.FuncReturn = NULL;
    EmptyTrackHandler.Command = HMSG_EMPTY_TRACK_HDLR_DEL;
    AppLibStorageAsyncOp_RegHandler(&EmptyTrackHandler);

    AppLibEmptyTrackHandlerInitFlag = 0;
    DBGMSG("<AppLibEmptyTrackHandlerInit> end: ReturnValue = %d", ReturnValue);

    return ReturnValue;
}

