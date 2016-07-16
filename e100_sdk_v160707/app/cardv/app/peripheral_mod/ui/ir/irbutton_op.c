/**
  * @file src/app/peripheral_mod/ui/ir/irbutton_op.c
  *
  * Implementation of IR Button Operation - APP level
  *
  * History:
  *    2013/12/24 - [Martin Lai] created file
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
#include "irbutton_op.h"
#include <wchar.h>
#include <AmbaIR.h>
#include <AmbaIrDecode.h>
#include <AmbaSysCtrl.h>
#include <framework/apphmi.h>
#include <framework/appmaintask.h>

//#define DEBUG_IR_BUTTON_OP
#if defined(DEBUG_IR_BUTTON_OP)
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
//#define IR_BUTTON_MGR_PRIORITY      (125)
#define IR_BUTTON_MGR_STACK_SIZE    (0x3800)
#define IR_BUTTON_MGR_NAME          "App_IR_Button_Operation_Manager"

/**
 * Map of a button to infrared receiver.
 */
typedef struct _IR_BUTTON_TABLE_s_ {
    INT32    bid;   ///< Button ID
    UINT32  uid;
} IR_BUTTON_TABLE_s;

static IR_BUTTON_TABLE_s G_ir_btoc_PANASONIC[] =
{
    { POWER_BUTTON,             0x100bcbd }, /* POWER */
    { IR_MUTE_BUTTON,           0x1004c4d }, /* MUTE(LCD reverse) */

    { IR_REG_DOWN_BUTTON,   0x100d2d3 }, /* VOLUME DOWN */
    { IR_REG_UP_BUTTON,        0x1005253 }, /* VOLUME UP */
    { IR_ZOOM_OUT_BUTTON,   0x100e2e3 }, /* CHANNEL DOWN(TELE) */
    { IR_ZOOM_IN_BUTTON,      0x1006263 }, /* CHANNEL UP(WIDE) */
    { IR_ZOOM_IN_BUTTON,      0x1004140 }, /* C1(TELE) */
    { IR_ZOOM_OUT_BUTTON,   0x1005c5d }, /* C2(WIDE) */

    { IR_UP_BUTTON,               0x1002c2d }, /* UP */
    { IR_DOWN_BUTTON,          0x100acad }, /* DOWN */
    { IR_LEFT_BUTTON,            0x1008485 }, /* LEFT */
    { IR_RIGHT_BUTTON,          0x1000405 }, /* RIGHT */
    { IR_SET_BUTTON,             0x100a0a1 }, /* ENTER */

    { IR_MODE_BUTTON,           0x1007e7f }, /* BASS(MODE) */
    { IR_MODE_BUTTON,           0x100f0f1 }, /* Sleep Timer(MODE) */
    { IR_SNAP2_BUTTON,          0x1000809 }, /* NUMBER 1(shutter) */
    { IR_SNAP1_BUTTON,          0x1008889 }, /* NUMBER 2(focus) */
    { IR_RECORD_BUTTON,        0x1004849 }, /* NUMBER 3(record) */

    { IR_BASS_LEFT_BUTTON,   0x1007273 }, /* BASS LEFT */
    { IR_N4_BUTTON,               0x100c8c9 }, /* NUMBER 4 */
    { IR_N5_BUTTON,               0x1002829 }, /* NUMBER 5 */
    { IR_N6_BUTTON,               0x100a8a9 }, /* NUMBER 6 */

    { IR_MENU_BUTTON,          0x100f2f3 }, /* BASS RIGHT */
    { IR_N7_BUTTON,              0x1006869 }, /* NUMBER 7 */
    { IR_N8_BUTTON,              0x100e8e9 }, /* NUMBER 8 */
    { IR_N9_BUTTON,              0x1001819 }, /* NUMBER 9 */

    { IR_AUDIO_BUTTON,         0x100e0e1 }, /* AUDIO */
    { IR_HUNDRED_BUTTON,     0x1003938 }, /* NUMBER 100 */
    { IR_N0_BUTTON,              0x1009899 }, /* NUMBER 0 */
    { IR_RETURN_BUTTON,       0x100eced }, /* TOGGLE */

    { IR_DEL_BUTTON,             0x100b0b1 }, /* SETUP(DEL) */
    { IR_IMAGE_BUTTON,         0x1006061 }, /* IMAGE(CS MODE) */
    { IR_FOUR_THREE_BUTTON, 0x1006c6d }, /* 4:3 */
    { IR_STANDARD_BUTTON,    0x1003031 }, /* STANDARD(CP MODE) */
    { IR_INDEX_BUTTON,          0x100feff }, /* INDEX */
    { IR_DISPLAY_BUTTON,       0x1009c9d }, /* DISPLAY */
    { IR_MTS_BUTTON,            0x100cccd }, /* MTS:Black controller N:Gray controller */
    { IR_MTS_BUTTON,            0x1009293 }, /* MTS:Gray controller */
    { -1,                               0x0000000}
};

typedef struct _IR_BUTTON_MGR_s_ {
    UINT8 Stack[IR_BUTTON_MGR_STACK_SIZE];  /**< Stack */
    AMBA_KAL_TASK_t Task;               /**< Task ID */
    INT32 LastButtonId;
    UINT32 LastButtonIdTime;
    IR_PROTOCOL_ID_e IrProtocol;
} IR_BUTTON_MGR_s;

/** Global instance of Button OP manager */
static IR_BUTTON_MGR_s G_IrButtonMgr = {0};
#define BUTTON_MSG(bid, offset)        (((offset) == 0) ? HMSG_KEY2_BUTTON(bid) : HMSG_KEY2_BUTTON_RELEASE(bid))
/*************************************************************************
 * Button OP APIs - Task
 ************************************************************************/
static AMBA_IR_DECODE_CTRL_s AppLibIrDecodeCtrl;
#define IR_MAX_NUM_RAW_EVENTS  512  /* maximum number of IR raw events */
static UINT16 AppLibIrRawEvents[IR_MAX_NUM_RAW_EVENTS];
#define IR_BUTTON_TIME_OUT  (500)

/**
 *  @brief IR button manager task
 *
 *  IR button manager task
 *
 *  @param [in] info information
 *
 *  @return >=0 success, <0 failure
 */
static void IRButtonOp_MgrTask(UINT32 info)
{
#ifdef CONFIG_IRDA // add for 'Aries' since 'Aries' doesn't have IR

    int ReturnValue = 0;
    UINT32 IrDataSize = 0;
    UINT16 *IrDataBuf = NULL;
    DBGMSG("[App - IR Button] <MgrTask> IR Button manager ready");

    while (1) {
//        AmbaKAL_TaskSleep(500);
        AmbaWaitSysStatus(AMBA_SYS_STATUS_IR_DATA_AVAIL_FLAG, AMBA_KAL_AND_CLEAR, 100);
        /*Read the IR information. */
        ReturnValue = AmbaIR_Read(&IrDataSize, &IrDataBuf);
        if (ReturnValue == NG) {
            AmbaPrint("[App - IR Button] <MgrTask> AmbaIR_Read failure.");
        }
        DBGMSGc2(GREEN,"[App - IR Button] <MgrTask> IrDataSize= %d, IrDataBuf = 0x%x, *IrDataBuf = 0x%x",IrDataSize, IrDataBuf, *IrDataBuf);
        {
            AMBA_IR_DECODE_CTRL_s *IrDecCtrl = &AppLibIrDecodeCtrl;
            UINT32 ScanCode = 0;
            int i = 0;
#ifndef DEBUG_IR_BUTTON_OP
            /* Release Ir button. */
            if (G_IrButtonMgr.LastButtonId >= 0) {
                if (( AmbaSysTimer_GetTickCount() - G_IrButtonMgr.LastButtonIdTime) >= IR_BUTTON_TIME_OUT) {
                    //AmbaPrint("[App - IR Button] IR_BUTTON_TIME_OUT");
                    AppLibComSvcHcmgr_SendMsgNoWait(BUTTON_MSG(G_IrButtonMgr.LastButtonId, HMSG_KEY_BUTTON_ID_RELEASE), 0, HMSG_KEY_BUTTON_ID_RELEASE);
                    G_IrButtonMgr.LastButtonId = -1;
                    G_IrButtonMgr.LastButtonIdTime = AmbaSysTimer_GetTickCount();
                }
            }
#endif
            if (IrDataBuf == NULL || IrDataSize == 0) {
                DBGMSG("[App - IR Button] <MgrTask> IrDataBuf == NULL || IrDataSize == 0");
            } else {
                if (IrDecCtrl->IrEventEnd + IrDataSize >= IrDecCtrl->IrEventBufSize) {
                    IrDecCtrl->IrEventEnd = 0;
                }

                memcpy(&IrDecCtrl->pIrEventBuf[IrDecCtrl->IrEventEnd], IrDataBuf, IrDataSize * sizeof(UINT16));
                IrDecCtrl->IrEventEnd += IrDataSize;

                DBGMSGc2(MAGENTA,"[App - IR Button] <MgrTask> IrDecCtrl->IrEventBufSize= %d, IrDataBuf = 0x%x, IrDecCtrl->IrEventEnd = 0x%x",IrDecCtrl->IrEventBufSize, IrDecCtrl->pIrEventBuf, IrDecCtrl->IrEventEnd);


                switch (G_IrButtonMgr.IrProtocol) {
                default:
                case IR_PROTOCOL_PANASONIC:
                    ReturnValue = AmbaIrDecode_Panasonic(IrDecCtrl, &ScanCode);
                    break;
                }

                if (ReturnValue == NG) {
                    DBGMSG("[App - IR Button] <MgrTask> ReturnValue = NG ");
                    //if (G_IrButtonMgr.LastButtonId != 0) {
                        //AppLibComSvcHcmgr_SendMsgNoWait(BUTTON_MSG(G_IrButtonMgr.LastButtonId, HMSG_KEY_BUTTON_ID_RELEASE), 0, HMSG_KEY_BUTTON_ID_RELEASE);
                    //}
                } else {
                    DBGMSGc2(YELLOW, "[App - IR Button] <MgrTask> ScanCode = 0x%x",ScanCode);
#ifndef DEBUG_IR_BUTTON_OP
                    IrDecCtrl->IrEventEnd = 0;
                    i = 0;
                    while (1) {
                        if (G_ir_btoc_PANASONIC[i].bid == -1) {
                            break;
                        } else if (G_ir_btoc_PANASONIC[i].uid == ScanCode) {
                            if (G_IrButtonMgr.LastButtonId == G_ir_btoc_PANASONIC[i].bid) {
                                G_IrButtonMgr.LastButtonIdTime = AmbaSysTimer_GetTickCount();
                            } else if (G_IrButtonMgr.LastButtonId < 0) {
                                AppLibComSvcHcmgr_SendMsgNoWait(BUTTON_MSG(G_ir_btoc_PANASONIC[i].bid, 0), 0, 0);
                                G_IrButtonMgr.LastButtonId = G_ir_btoc_PANASONIC[i].bid;
                                G_IrButtonMgr.LastButtonIdTime = AmbaSysTimer_GetTickCount();
                            } else {
                                AppLibComSvcHcmgr_SendMsgNoWait(BUTTON_MSG(G_IrButtonMgr.LastButtonId, HMSG_KEY_BUTTON_ID_RELEASE), 0, HMSG_KEY_BUTTON_ID_RELEASE);
                                AppLibComSvcHcmgr_SendMsgNoWait(BUTTON_MSG(G_ir_btoc_PANASONIC[i].bid, 0), 0, 0);
                                G_IrButtonMgr.LastButtonId = G_ir_btoc_PANASONIC[i].bid;
                                G_IrButtonMgr.LastButtonIdTime = AmbaSysTimer_GetTickCount();
                            }
                            break;
                        } else {
                            i++;
                        }
                    }
#endif
                }
            }
        }
    }
#endif
}


/**
 *  @brief Initialize the IR button task.
 *
 *  Initialize the IR button task.
 *
 *  @return >=0 success, <0 failure
 */
int AppIRButtonOp_Init(void)
{
    int ReturnValue = 0;
#ifdef CONFIG_IRDA // add for 'Aries' since 'Aries' doesn't have IR
    DBGMSG("[App - IR Button] <Init> start");

    /* Clear G_IrButtonMgr */
    memset(&G_IrButtonMgr, 0, sizeof(IR_BUTTON_MGR_s));
    G_IrButtonMgr.IrProtocol = IR_PROTOCOL_PANASONIC;
    AmbaIR_Init();
    memset(&AppLibIrDecodeCtrl, 0x0, sizeof(AMBA_IR_DECODE_CTRL_s));
    AppLibIrDecodeCtrl.pIrEventBuf = AppLibIrRawEvents;
    AppLibIrDecodeCtrl.IrEventBufSize = GetArraySize(AppLibIrRawEvents);


    AmbaIR_Enable();

    /* Create IR button Manager task*/
    ReturnValue = AmbaKAL_TaskCreate(&G_IrButtonMgr.Task, /* pTask */
        IR_BUTTON_MGR_NAME, /* pTaskName */
        APP_IR_BUTTON_OP_PRIORITY, /* Priority */
        IRButtonOp_MgrTask, /* void (*EntryFunction)(UINT32) */
        0x0, /* EntryArg */
        (void *) G_IrButtonMgr.Stack, /* pStackBase */
        IR_BUTTON_MGR_STACK_SIZE, /* StackByteSize */
        AMBA_KAL_AUTO_START); /* AutoStart */
    if (ReturnValue != OK) {
        AmbaPrintColor(RED, "[App - IR Button] <Init> Create task fail = %d", ReturnValue);
    }

    DBGMSG("[App - IR Button] <Init> end: ReturnValue = %d", ReturnValue);
#endif
    return ReturnValue;
}


