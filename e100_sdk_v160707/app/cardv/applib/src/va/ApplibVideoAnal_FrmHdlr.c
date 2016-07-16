/**
 * @file src/app/connected/applib/src/va/ApplibVideoAnal_FrmHdlr.c
 *
 * Amba VA frame handler
 *
 * History:
 *    2014/12/02 - [Peter Weng] created file
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

#define AppLib_VA_TASK_NAMEE          "VA frame hdlr"
#define AppLib_VA_MSGQUEUE_SIZE (4)

static UINT8 AppYuvStack[(32<<10)];

typedef struct _APPLIB_FRM_MGRMESSAGE_t_ {
    UINT32 MessageID;
    UINT32 MessageData[2];
    AMP_ENC_YUV_INFO_s YUVInfo;
} APPLIB_FRM_MGRMESSAGE_t;

typedef struct APPLIB_VA_CB_SLOT_T_ {
    APPLIB_VA_FRMHDLR_CB Func;
    UINT32 Event;
} APPLIB_VA_CB_SLOT_T;

typedef struct _G_APPLIB_VA_T_ {
    UINT8 Init;
    AMBA_KAL_TASK_t Task;
    AMBA_KAL_MUTEX_t Mutex;
    AMBA_KAL_MSG_QUEUE_t MsgQueue;
    UINT8 MsgPool[sizeof(APPLIB_FRM_MGRMESSAGE_t) * AppLib_VA_MSGQUEUE_SIZE];
    APPLIB_VA_CB_SLOT_T CbFunc[APPLIB_FRM_HDLR_MAX_CB];
} G_APPLIB_VA_T;

static G_APPLIB_VA_T G_AppLib_VA = { 0 };
static APPLIB_FRM_MGRMESSAGE_t AppLib_TempFrmMessage[APPLIB_FRM_HDLR_YUV_NUM];

static int PreviousYSize = 0;

#define AppLib_VA_LOC AmbaKAL_MutexTake(&G_AppLib_VA.Mutex, AMBA_KAL_WAIT_FOREVER);
#define AppLib_VA_UNL AmbaKAL_MutexGive(&G_AppLib_VA.Mutex);

int AppLibVideoAnal_FrmHdlr_GetFrmInfo(UINT8 yuvSrc, AMP_ENC_YUV_INFO_s* yuvInfo, int* frmSizeIsChanged)
{
    int ReturnValue = 0;
    int CheckCount = 0;

    if (G_AppLib_VA.Init == 0) {
        AmbaPrintColor(GREEN, "[VA] AppLibVideoAnal_FrmHdlr is not init.");
        return -1;
    }
    while (1) {
        ReturnValue = AmbaKAL_MutexTake(&G_AppLib_VA.Mutex, AMBA_KAL_NO_WAIT);
        if (AppLib_TempFrmMessage[yuvSrc].YUVInfo.ySize > 0) {
            if (AppLib_TempFrmMessage[yuvSrc].YUVInfo.ySize != PreviousYSize) {
                *frmSizeIsChanged = 1;
                PreviousYSize = AppLib_TempFrmMessage[yuvSrc].YUVInfo.ySize;
            }
            memcpy(yuvInfo, &AppLib_TempFrmMessage[yuvSrc].YUVInfo, sizeof(AMP_ENC_YUV_INFO_s));
            AmbaKAL_MutexGive(&G_AppLib_VA.Mutex);
            break;
        } else {
            AmbaKAL_MutexGive(&G_AppLib_VA.Mutex);
            AmbaKAL_TaskSleep(1);
        }
        CheckCount++;
        if (CheckCount > 100) {
            break;
        }
    }
    return ReturnValue;
}

int  AppLibVideoAnal_FrmHdlr_IsInit(void)
{
    return G_AppLib_VA.Init;
}

void AppLib_VA_Maintask(UINT32 info)
{
    APPLIB_FRM_MGRMESSAGE_t Msg;
    int ReturnValue;
    int T;
    while (1) {
        ReturnValue = AmbaKAL_MsgQueueReceive(&G_AppLib_VA.MsgQueue, (void *) &Msg, AMBA_KAL_WAIT_FOREVER );
        if (ReturnValue == OK) {
            //new frame coming in
            AppLib_VA_LOC
            ;
            for (T = 0; T < APPLIB_FRM_HDLR_MAX_CB; T++) {
                if (G_AppLib_VA.CbFunc[T].Func != NULL && G_AppLib_VA.CbFunc[T].Event == Msg.MessageID) {
                    G_AppLib_VA.CbFunc[T].Func(Msg.MessageID, &Msg.YUVInfo);
                }
            }
            AppLib_VA_UNL
            ;
        } else {
            AmbaKAL_TaskSleep(1);
        }
    }
}

int AppLibVideoAnal_FrmHdlr_GetDefCfg(APPLIB_YUV_TASK_CFG_t* cfg)
{
    cfg->TaskPriority = APPLIB_FRM_HDLR_DEF_PRIORITY;
    cfg->TaskStackSize = APPLIB_FRM_HDLR_DEF_TASK_STACK_SIZE;
    return OK;
}

int AppLibVideoAnal_FrmHdlr_Init(void)
{
    int ReturnValue;
    if (G_AppLib_VA.Init != 0) {
        return NG;
    }

    memset(&AppLib_TempFrmMessage, 0, sizeof(AppLib_TempFrmMessage));
    AmbaPrint("VA init...... ");
    // create mqueue
    ReturnValue = AmbaKAL_MsgQueueCreate(&G_AppLib_VA.MsgQueue, G_AppLib_VA.MsgPool, sizeof(APPLIB_FRM_MGRMESSAGE_t), AppLib_VA_MSGQUEUE_SIZE);
    if (ReturnValue == OK) {
        AmbaPrintColor(GREEN, "[VA INIT] Create Queue success");
    } else {
        AmbaPrintColor(RED, "[VA INIT] Create Queue fail = %d", ReturnValue);
        return NG;
    }
    // create mutex
    ReturnValue = AmbaKAL_MutexCreate(&G_AppLib_VA.Mutex);
    if (ReturnValue == OK) {
        AmbaPrintColor(GREEN, "[VA INIT] Create Mutex success");
    } else {
        AmbaPrintColor(RED, "[VA INIT] Create Mutex fail = %d", ReturnValue);
        return NG;
    }
    AmbaPrintColor(GREEN, "[VA INIT] TaskPriority %d , TaskStackSize %d", APPLIB_FRM_HDLR_DEF_PRIORITY, APPLIB_FRM_HDLR_DEF_TASK_STACK_SIZE);
    // create task
    ReturnValue = AmbaKAL_TaskCreate(&G_AppLib_VA.Task,
                              AppLib_VA_TASK_NAMEE,
                              APPLIB_FRM_HDLR_DEF_PRIORITY,
                              AppLib_VA_Maintask,
                              0x0,
                              AppYuvStack,
                              APPLIB_FRM_HDLR_DEF_TASK_STACK_SIZE,
                              AMBA_KAL_AUTO_START);
    if (ReturnValue == OK) {
        AmbaPrintColor(GREEN, "[VA INIT] Create Task success");
    } else {
        AmbaPrintColor(RED, "[VA INIT] Create Task fail = %d", ReturnValue);
        return NG;
    }

    G_AppLib_VA.Init = 1;
    AppLibComSvcHcmgr_SendMsgNoWait(HMSG_VA_TASKYUV_ON, 0, 0);
    AmbaPrint("VA init done");

    return OK;
}

int AppLibVideoAnal_FrmHdlr_Register(UINT8 yuvSrc, APPLIB_VA_FRMHDLR_CB func)
{
    int T;
    UINT32 event = AMP_ENC_EVENT_VCAP_2ND_YUV_READY;

    if (yuvSrc == APPLIB_FRM_HDLR_DCHAN_YUV) {
        event = AMP_ENC_EVENT_LIVEVIEW_DCHAN_YUV_READY;
    }

    AppLib_VA_LOC
    ;
    for (T = 0; T < APPLIB_FRM_HDLR_MAX_CB; T++) {
        if (G_AppLib_VA.CbFunc[T].Func == NULL) {
            G_AppLib_VA.CbFunc[T].Func = func;
            G_AppLib_VA.CbFunc[T].Event = event;
            break;
        }
    }
    AppLib_VA_UNL
    ;
    if (T == APPLIB_FRM_HDLR_MAX_CB) {
        return NG;
    }
    return OK;
}

int AppLibVideoAnal_FrmHdlr_UnRegister(UINT8 yuvSrc, APPLIB_VA_FRMHDLR_CB func)
{
    int T;
    UINT32 event = AMP_ENC_EVENT_VCAP_2ND_YUV_READY;

    if (yuvSrc == APPLIB_FRM_HDLR_DCHAN_YUV) {
        event = AMP_ENC_EVENT_LIVEVIEW_DCHAN_YUV_READY;
    }

    AppLib_VA_LOC
    ;
    for (T = 0; T < APPLIB_FRM_HDLR_MAX_CB; T++) {
        if (G_AppLib_VA.CbFunc[T].Func == func && G_AppLib_VA.CbFunc[T].Event == event) {
            G_AppLib_VA.CbFunc[T].Func = NULL;
            G_AppLib_VA.CbFunc[T].Event = 0;
        }
    }
    AppLib_VA_UNL
    ;
    return OK;
}

int AppLibVideoAnal_FrmHdlr_NewFrame(UINT32 event, AMP_ENC_YUV_INFO_s* yuvInfo)
{
    int ReturnValue = 0;
    ReturnValue = AmbaKAL_MutexTake(&G_AppLib_VA.Mutex, AMBA_KAL_NO_WAIT);
    if (ReturnValue == OK) {

        if (event == AMP_ENC_EVENT_LIVEVIEW_DCHAN_YUV_READY) {
            AppLib_TempFrmMessage[APPLIB_FRM_HDLR_DCHAN_YUV].MessageID = event;
            memcpy(&AppLib_TempFrmMessage[APPLIB_FRM_HDLR_DCHAN_YUV].YUVInfo, yuvInfo, sizeof(AMP_ENC_YUV_INFO_s));
            // VA task is idle or none registering cb, send msg to trigger proc
            ReturnValue = AmbaKAL_MsgQueueSend(&G_AppLib_VA.MsgQueue, &AppLib_TempFrmMessage[APPLIB_FRM_HDLR_DCHAN_YUV], AMBA_KAL_NO_WAIT);
        } else if (event == AMP_ENC_EVENT_VCAP_2ND_YUV_READY) {
            AppLib_TempFrmMessage[APPLIB_FRM_HDLR_2ND_YUV].MessageID = event;
            memcpy(&AppLib_TempFrmMessage[APPLIB_FRM_HDLR_2ND_YUV].YUVInfo, yuvInfo, sizeof(AMP_ENC_YUV_INFO_s));
            // VA task is idle or none registering cb, send msg to trigger proc
            ReturnValue = AmbaKAL_MsgQueueSend(&G_AppLib_VA.MsgQueue, &AppLib_TempFrmMessage[APPLIB_FRM_HDLR_2ND_YUV], AMBA_KAL_NO_WAIT);
        }
        AmbaKAL_MutexGive(&G_AppLib_VA.Mutex);
    }
    return ReturnValue;
}


