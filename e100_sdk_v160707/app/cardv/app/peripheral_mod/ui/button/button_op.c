/**
  * @file src/app/peripheral_mod/ui/button/button_op.c
  *
  * Implementation of Button Operation - APP level
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
 * affiliates.  In the absence of such an agreement, you agree to promptly notify and
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
#include "button_op.h"
#include <wchar.h>
#include <framework/apphmi.h>
#include <framework/appmaintask.h>
#if defined(CONFIG_APP_ARD)
#include <apps/flow/widget/widgetmgt.h>
#endif

//#define DEBUG_DEMOLIB_BUTTON_OP
#if defined(DEBUG_DEMOLIB_BUTTON_OP)
#define DBGMSG AmbaPrint
#define DBGMSGc(x) AmbaPrintColor(GREEN,x)
#define DBGMSGc2 AmbaPrintColor
#else
#define DBGMSG(...)
#define DBGMSGc(...)
#define DBGMSGc2(...)
#endif

/*************************************************************************
 * Button OP definitons
 ************************************************************************/
#define BUTTON_MGR_STACK_SIZE    (0x3800)
#define BUTTON_MGR_NAME          "App_Button_Operation_Manager"
#define BUTTON_MGR_MSGQUEUE_SIZE (16)

typedef struct _APP_BUTTON_MGR_MESSAGE_s_ {
    UINT32 MessageID;
    UINT32 MessageData[2];
} APP_BUTTON_MGR_MESSAGE_s;

typedef struct _BUTTON_MGR_s_ {
    UINT8 Stack[BUTTON_MGR_STACK_SIZE];  /**< Stack */
    UINT8 MsgPool[sizeof(APP_BUTTON_MGR_MESSAGE_s)*BUTTON_MGR_MSGQUEUE_SIZE];   /**< Message memory pool. */
    AMBA_KAL_TASK_t Task;               /**< Task ID */
    AMBA_KAL_MSG_QUEUE_t MsgQueue;      /**< Message queue ID */
} BUTTON_MGR_s;

/** Global instance of Button OP manager */
static BUTTON_MGR_s G_buttonmgr = {0};
#define BUTTON_MSG(bid, offset)        (((offset) == 0) ? HMSG_KEY2_BUTTON(bid) : HMSG_KEY2_BUTTON_RELEASE(bid))

/**
 *  @brief Send the message
 *
 *  Send the message
 *
 *  @param [in] msg Meassage
 *  @param [in] param1 Parameter 1
 *  @param [in] param2 Parameter 2
 *
 *  @return >=0 success, <0 failure
 */
static int ButtonOp_SndMsg(UINT32 msg, UINT32 param1, UINT32 param2)
{
    int ReturnValue = 0;
    APP_BUTTON_MGR_MESSAGE_s t_msg = {0};

    t_msg.MessageID = msg;
    t_msg.MessageData[0] = param1;
    t_msg.MessageData[1] = param2;

    ReturnValue = AmbaKAL_MsgQueueSend(&G_buttonmgr.MsgQueue, &t_msg, AMBA_KAL_NO_WAIT);
    //AmbaPrint("SndMsg.MessageID = 0x%x ReturnValue = %d", msg->MessageID, ReturnValue);

    return ReturnValue;
}

/**
 *  @brief Receive the message
 *
 *  Receive the message
 *
 *  @param [in] msg Message
 *  @param [in] waitOption Wait option
 *
 *  @return >=0 success, <0 failure
 */
static int ButtonOp_RcvMsg(APP_BUTTON_MGR_MESSAGE_s *msg, UINT32 waitOption)
{
    int ReturnValue = 0;

    ReturnValue = AmbaKAL_MsgQueueReceive(&G_buttonmgr.MsgQueue, (void *)msg, waitOption);
    //AmbaPrint("RcvMsg.MessageID = 0x%x ReturnValue = %d", msg->MessageID, ReturnValue);

    return ReturnValue;
}
#if defined(CONFIG_APP_ARD)
static int app_remap_key_msg_handle(UINT32 is_release_msg,UINT32 btn_bid);
#endif
/**
 *  @brief Button manager task
 *
 *  Button manager task
 *
 *  @param [in] info information
 *
 *  @return >=0 success, <0 failure
 */
static void ButtonOp_MgrTask(UINT32 info)
{
    UINT32 param1 = 0, param2 = 0;
    APP_BUTTON_MGR_MESSAGE_s Msg = {0};
    AmbaPrint("[Button Handler] Button manager ready");

    while (1) {
        ButtonOp_RcvMsg(&Msg, AMBA_KAL_WAIT_FOREVER);
        param1 = Msg.MessageData[0];
        param2 = Msg.MessageData[1];
        AmbaPrint("[Button Handler] Received msg: 0x%X (param1 = 0x%X / param2 = 0x%X)", Msg.MessageID, param1, param2);
        switch (Msg.MessageID) {
        default:
#if defined(CONFIG_APP_ARD)
            app_remap_key_msg_handle(Msg.MessageID,param1);
#else
            AppLibComSvcHcmgr_SendMsgNoWait(BUTTON_MSG(param1, Msg.MessageID), 0, 0);
#endif
            break;
        }
    }

    DBGMSG("[Button] msg 0x%X is done (ReturnValue = %d / retInfo = %d)", Msg.MessageID, ReturnValue, retInfo);
}


/**
 *  @brief Initialize the button operation
 *
 *  Initialize the button operation
 *
 *  @return >=0 success, <0 failure
 */
int AppButtonOp_Init(void)
{
    int ReturnValue = 0;

    DBGMSG("[DemoLib - Button] <AppButtonOp_init> start");

    /* Clear G_buttonmgr */
    memset(&G_buttonmgr, 0, sizeof(BUTTON_MGR_s));

    /* Create App message queue */
    ReturnValue = AmbaKAL_MsgQueueCreate(&G_buttonmgr.MsgQueue, G_buttonmgr.MsgPool, sizeof(APP_BUTTON_MGR_MESSAGE_s), BUTTON_MGR_MSGQUEUE_SIZE);
    if (ReturnValue == OK) {
        DBGMSGc2(GREEN, "[DemoLib - Button]Create Queue success = %d", ReturnValue);
    } else {
        AmbaPrintColor(RED, "[DemoLib - Button]Create Queue fail = %d", ReturnValue);
    }
    /* Create Host Control Manager task*/
    ReturnValue = AmbaKAL_TaskCreate(&G_buttonmgr.Task, /* pTask */
        BUTTON_MGR_NAME, /* pTaskName */
        APP_BUTTON_OP_PRIORITY, /* Priority */
        ButtonOp_MgrTask, /* void (*EntryFunction)(UINT32) */
        0x0, /* EntryArg */
        (void *) G_buttonmgr.Stack, /* pStackBase */
        BUTTON_MGR_STACK_SIZE, /* StackByteSize */
        AMBA_KAL_AUTO_START); /* AutoStart */
    if (ReturnValue != OK) {
        AmbaPrintColor(RED, "[DemoLib - Button]Create task fail = %d", ReturnValue);
    }

    DBGMSG("[DemoLib - Button] <AppButtonOp_init> end: ReturnValue = %d", ReturnValue);

    return ReturnValue;
}


/**
 *  @brief Update button status
 *
 *  Update button status
 *
 *  @param [in] buttonId Button id
 *  @param [in] event Event
 *
 *  @return >=0 success, <0 failure
 */
int AppButtonOp_UpdateStatus(UINT32 buttonId, UINT32 event)
{
    ButtonOp_SndMsg(event, buttonId, 0);
    return 0;
}

#if defined(CONFIG_APP_ARD)
/*button remap table*/
#if defined(CONFIG_BSP_ARIES)
#define REMAPPED_KEY_NUM  (5)
static KEY_REMAP_TBL remap_tbl[REMAPPED_KEY_NUM] = {
                  /* BTN_IS_MENU_UP_DOWN_MODE, BTN_IS_MENU_LEFT_RIGHT_MODE, BTN_IS_MENU_RIGHT_LEFT_MODE, BTN_IS_MENU_UP_RIGHT_MODE, BTN_IS_REC_MODE, BTN_IS_PHOTO_MODE, BTN_IS_PB_MODE*/
    {UP_BUTTON,     {UP_BUTTON,                  LEFT_BUTTON,                  LEFT_BUTTON,                 UP_BUTTON,                  LEFT_BUTTON,     LEFT_BUTTON,         LEFT_BUTTON}},
    {DOWN_BUTTON,   {DOWN_BUTTON,                RIGHT_BUTTON,                 RIGHT_BUTTON,                DOWN_BUTTON,                RIGHT_BUTTON,    RIGHT_BUTTON,        RIGHT_BUTTON}},
    {MENU_BUTTON,   {MENU_BUTTON,                MENU_BUTTON,                  MENU_BUTTON,                 MENU_BUTTON,                MENU_BUTTON,     MENU_BUTTON,         MENU_BUTTON}},
    {RECORD_BUTTON, {SET_BUTTON,                 SET_BUTTON,                   SET_BUTTON,                  SET_BUTTON,                 RECORD_BUTTON,   SET_BUTTON,          SET_BUTTON}},
    {MODE_BUTTON,   {RIGHT_BUTTON,               MODE_BUTTON,                  MODE_BUTTON,                 RIGHT_BUTTON,               MODE_BUTTON,     MODE_BUTTON,         MODE_BUTTON}},
};
#elif defined(CONFIG_BSP_ORTHRUS)   /*5Key ORTHRUS @20150917*/
#define REMAPPED_KEY_NUM  (5)
static KEY_REMAP_TBL remap_tbl[REMAPPED_KEY_NUM] = {
                     /* MENU_UP_DOWN,     MENU_LEFT_RIGHT,   MENU_RIGHT_LEFT,   MENU_UP_RIGHT,  REC_MODE,       PHOTO_MODE,     PB_MODE*/
    {UP_BUTTON,     {UP_BUTTON,         LEFT_BUTTON,            LEFT_BUTTON,            UP_BUTTON,          LEFT_BUTTON,        LEFT_BUTTON,        LEFT_BUTTON}},
    {DOWN_BUTTON,   {DOWN_BUTTON,       RIGHT_BUTTON,           RIGHT_BUTTON,           DOWN_BUTTON,        DOWN_BUTTON,        RIGHT_BUTTON,       RIGHT_BUTTON}},
    {MENU_BUTTON,   {MENU_BUTTON,       MENU_BUTTON,            MENU_BUTTON,            MENU_BUTTON,        MENU_BUTTON,        MENU_BUTTON,        MENU_BUTTON}},
    {RECORD_BUTTON, {SET_BUTTON,        SET_BUTTON,             SET_BUTTON,             RIGHT_BUTTON,       RECORD_BUTTON,      SNAP2_BUTTON,       SET_BUTTON}},
    {MODE_BUTTON,   {RIGHT_BUTTON,      MODE_BUTTON,            MODE_BUTTON,            MODE_BUTTON,        MODE_BUTTON,        MODE_BUTTON,        MODE_BUTTON}},
};
#elif defined(CONFIG_BSP_GOAT)
#define REMAPPED_KEY_NUM  (5)
static KEY_REMAP_TBL remap_tbl[REMAPPED_KEY_NUM] = {
                  /* BTN_IS_MENU_UP_DOWN_MODE, BTN_IS_MENU_LEFT_RIGHT_MODE, BTN_IS_MENU_RIGHT_LEFT_MODE, BTN_IS_MENU_UP_RIGHT_MODE, BTN_IS_REC_MODE, BTN_IS_PHOTO_MODE, BTN_IS_PB_MODE*/
    {UP_BUTTON,     {UP_BUTTON,                  LEFT_BUTTON,                  LEFT_BUTTON,                 UP_BUTTON,                  LEFT_BUTTON,     LEFT_BUTTON,         LEFT_BUTTON}},
    {DOWN_BUTTON,   {DOWN_BUTTON,                RIGHT_BUTTON,                 RIGHT_BUTTON,                DOWN_BUTTON,                RIGHT_BUTTON,    RIGHT_BUTTON,        RIGHT_BUTTON}},
    {MENU_BUTTON,   {MENU_BUTTON,                MENU_BUTTON,                  MENU_BUTTON,                 MENU_BUTTON,                MENU_BUTTON,     MENU_BUTTON,         MENU_BUTTON}},
    {RECORD_BUTTON, {SET_BUTTON,                 SET_BUTTON,                   SET_BUTTON,                  SET_BUTTON,                 RECORD_BUTTON,   SET_BUTTON,          SET_BUTTON}},
    {MODE_BUTTON,   {RIGHT_BUTTON,               MODE_BUTTON,                  MODE_BUTTON,                 RIGHT_BUTTON,               MODE_BUTTON,     MODE_BUTTON,         MODE_BUTTON}},
};
#else
#define REMAPPED_KEY_NUM  (0)
static KEY_REMAP_TBL remap_tbl[1] = {{0}};
#endif

static UI_BUTTON_MODE m_pre_button_mode = BTN_MODE_NUM;
static UI_BUTTON_MODE m_special_button_mode = BTN_MODE_NUM;
void app_button_set_special_mode(UI_BUTTON_MODE mode)
{
    m_special_button_mode = mode;
}

static UINT32 app_button_remap_impl(UI_BUTTON_MODE new_mode,UINT32 bid,UINT32 is_release_msg)
{
    UINT32 new_bid = bid;
    UI_BUTTON_MODE mode = new_mode;
    int i;

        /*For button CLR msg, its mode should be previous mode.*/
        if((is_release_msg)&&(m_pre_button_mode != mode)) {
            mode = m_pre_button_mode;
        }

        if(mode <BTN_MODE_NUM) {
            for(i=0;i<REMAPPED_KEY_NUM;i++) {
                if(remap_tbl[i].bid == bid) {
                    new_bid = remap_tbl[i].new_bid[mode];
                    break;
                }
            }
        } else {
            AmbaPrintColor(RED,"btn mode invalide");
        }

        m_pre_button_mode = new_mode;

    return new_bid;
}

static UI_BUTTON_MODE app_get_button_mode(void)
{
    UI_BUTTON_MODE mode = BTN_MODE_NUM;

    int widget_id = AppWidget_GetCur();

    switch (widget_id) {
    case WIDGET_MENU:
        mode = BTN_IS_MENU_UP_DOWN_MODE;
        break;
    case WIDGET_MENU_QUICK:
        mode = BTN_IS_MENU_LEFT_RIGHT_MODE;
        break;
    case WIDGET_MENU_ADJ:
        mode = BTN_IS_MENU_LEFT_RIGHT_MODE;
        break;
    case WIDGET_MENU_TIME:
        if(m_special_button_mode != BTN_MODE_NUM) {
            mode = m_special_button_mode;
        } else {
            mode = BTN_IS_MENU_UP_RIGHT_MODE;
        }
        break;
    case WIDGET_DIALOG:
        mode = BTN_IS_MENU_LEFT_RIGHT_MODE;
        break;
#if defined(CONFIG_APP_ARD)
    case WIDGET_MENU_CKBX:
        mode = BTN_IS_MENU_UP_DOWN_MODE;
        break;
#if defined(CONFIG_BSP_ORTHRUS)
    case WIDGET_MENU_DRIVER_ID:
        mode = BTN_IS_MENU_UP_RIGHT_MODE;
#else
    case WIDGET_MENU_DRIVER_ID:
        mode = BTN_IS_MENU_UP_DOWN_MODE;
#endif
        break;
#endif
    case WIDGET_NONE:
        {
            int cur_app_id = -1;
            APP_APP_s *CurApp;
            cur_app_id = AppAppMgt_GetCurApp(&CurApp);
            if(cur_app_id == APP_REC_CAM) {
                mode = BTN_IS_REC_MODE;
            }else if(cur_app_id == APP_THUMB_MOTION) {
                mode = BTN_IS_PB_MODE;
            }else if(cur_app_id == APP_PB_MULTI) {
                mode = BTN_IS_PB_MODE;
            }else if(cur_app_id == APP_MISC_POLICE_ID) {
                mode = BTN_IS_MENU_UP_DOWN_MODE;
            }else {
                mode = BTN_IS_MENU_UP_DOWN_MODE;
            }
        }
        break;
    default:
        AmbaPrint("Unknown widget!!!");
        break;
    }

    return mode;
}

static int app_remap_key_msg_handle(UINT32 is_release_msg,UINT32 btn_bid)
{
    UINT32 new_bid;
    UI_BUTTON_MODE bt_mode;


    bt_mode = app_get_button_mode();

    new_bid = app_button_remap_impl(bt_mode,btn_bid,is_release_msg);

    AppLibComSvcHcmgr_SendMsgNoWait(BUTTON_MSG(new_bid, is_release_msg), 0, 0);

    return 0;
}
#endif


