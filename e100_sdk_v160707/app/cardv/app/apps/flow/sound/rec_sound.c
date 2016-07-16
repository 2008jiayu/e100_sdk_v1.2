/**
  * @file src/app/apps/flow/sound/rec_sound.c
  *
  * Implementation of Sport Recorder (sensor) application
  *
  * History:
  *    2014/11/26 - [QiangSu] created file
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

#include <apps/flow/sound/rec_sound.h>

/*************************************************************************
 * Declarations (static)
 ************************************************************************/
/* App structure interfaces APIs */
static int app_rec_sound_start(void);
static int app_rec_sound_stop(void);
static int app_rec_sound_on_message(UINT32 msg, UINT32 param1, UINT32 param2);

APP_APP_s app_rec_sound = {
    0,  //Id
    1,  //Tier
    0,  //Parent
    0,  //Previous
    0,  //Child
    0,  //GFlags
    0,  //Flags
    app_rec_sound_start,//start()
    app_rec_sound_stop,    //stop()
    app_rec_sound_on_message  //OnMessage()
};

/* App status */
REC_SOUND_s rec_sound = {0};

/*************************************************************************
 * Definitions (static)
 ************************************************************************/
/* App structure interface APIs */

/**
 *  @brief The application's function that handle the message.
 *
 *  The application's function that handle the message.
 *
 *  @param [in] msg Message ID
 *  @param [in] param1 first parameter
 *  @param [in] param2 second parameter
 *
 *  @return >=0 success, <0 failure
 */
static int app_rec_sound_on_message(UINT32 msg, UINT32 param1, UINT32 param2)
{
    int ReturnValue = 0;
    DBGMSG("[app_rec_sound] Received msg: 0x%X (param1 = 0x%X / param2 = 0x%X)", msg, param1, param2);

    ReturnValue = AppWidget_OnMessage(msg, param1, param2);
    if (ReturnValue != WIDGET_PASSED_MSG) {
        return ReturnValue;
    }
    switch (msg) {
   case AMSG_CMD_APP_READY:
   	DBGMSG("[app_rec_sound] Received AMSG_CMD_APP_READY");
    
    if (!APP_CHECKFLAGS(app_rec_sound.GFlags, APP_AFLAGS_READY)) {
   	//rec_sound.Func(REC_SOUND_APP_READY, 0, 0);
   	APP_ADDFLAGS(app_rec_sound.GFlags, APP_AFLAGS_READY);   
   	
        // ToDo: need to remove to handler when iav completes the dsp cmd queue mechanism
         AppLibGraph_Init();
         rec_sound.Func(REC_SOUND_CHANGE_OSD, 0, 0);
        /* To show the gui of current application. */
        rec_sound.Func(REC_SOUND_GUI_INIT_SHOW, 0, 0);
        
        ReturnValue = rec_sound.Func(REC_SOUND_CARD_CHECK_STATUS, 0, 0);
        if (ReturnValue == 0) {
            /* To record the clip if the card is ready. */
            rec_sound.Func(REC_SOUND_RECORD_START, 0, 0);
        }

        }
   	break;    
    case HMSG_USER_RECORD_BUTTON:
    case HMSG_USER_IR_RECORD_BUTTON:
        if (!APP_CHECKFLAGS(app_rec_sound.GFlags, APP_AFLAGS_READY)) {
            break;    // not ready
        }
        ReturnValue = rec_sound.Op->ButtonRecord();
        break;
    case HMSG_USER_UP_BUTTON:
    case HMSG_USER_IR_UP_BUTTON:
        if (!APP_CHECKFLAGS(app_rec_sound.GFlags, APP_AFLAGS_READY)) {
            break;    // not ready
        }
        ReturnValue = rec_sound.Op->ButtonUp();
        break;
    case HMSG_USER_DOWN_BUTTON:
    case HMSG_USER_IR_DOWN_BUTTON:
        if (!APP_CHECKFLAGS(app_rec_sound.GFlags, APP_AFLAGS_READY)) {
            break;    // not ready
        }
        ReturnValue = rec_sound.Op->ButtonDown();
        break;
    case HMSG_USER_LEFT_BUTTON:
    case HMSG_USER_IR_LEFT_BUTTON:
        if (!APP_CHECKFLAGS(app_rec_sound.GFlags, APP_AFLAGS_READY)) {
            break;    // not ready
        }
        ReturnValue = rec_sound.Op->ButtonLeft();
        break;
    case HMSG_USER_RIGHT_BUTTON:
    case HMSG_USER_IR_RIGHT_BUTTON:
        if (!APP_CHECKFLAGS(app_rec_sound.GFlags, APP_AFLAGS_READY)) {
            break;    // not ready
        }
        ReturnValue = rec_sound.Op->ButtonRight();
        break;
    case HMSG_USER_SET_BUTTON:
    case HMSG_USER_IR_SET_BUTTON:
        if (!APP_CHECKFLAGS(app_rec_sound.GFlags, APP_AFLAGS_READY)) {
            break;    // not ready
        }
        ReturnValue = rec_sound.Op->ButtonSet();
        break;
    case HMSG_USER_MENU_BUTTON:
    case HMSG_USER_IR_MENU_BUTTON:
        if (!APP_CHECKFLAGS(app_rec_sound.GFlags, APP_AFLAGS_READY)) {
            break;    // not ready
        }
        ReturnValue = rec_sound.Op->ButtonMenu();
        break;
    case HMSG_USER_MODE_BUTTON:
    case HMSG_USER_IR_MODE_BUTTON:
        ReturnValue = rec_sound.Op->ButtonMode();
        break;
    case HMSG_USER_POWER_BUTTON:
        if (!APP_CHECKFLAGS(app_rec_sound.GFlags, APP_AFLAGS_READY)) {
            break;    // not ready
        }
        ReturnValue = rec_sound.Op->ButtonPower();
        break;
    case HMSG_USER_F1_BUTTON:
        if (!APP_CHECKFLAGS(app_rec_sound.GFlags, APP_AFLAGS_READY)) {
            break;    // not ready
        }
        if (rec_sound.RecState == REC_SOUND_STATE_RECORD) {
            /* Stop recording. */
            rec_sound.Func(REC_SOUND_RECORD_STOP, 0, 0);
        }     
        break;        
    case AMSG_CMD_SWITCH_APP:
        //ReturnValue = rec_sound.Func(REC_SOUND_SWITCH_APP, param1, param2);
        break;
    case HMSG_MUXER_END:
        DBGMSG("[app_rec_sound] Received HMSG_MUXER_END");
        ReturnValue = rec_sound.Func(REC_SOUND_MUXER_END, param1, param2);
        AppUtil_SwitchMode(APP_REC_CAM);
        break;
    case HMSG_MUXER_REACH_LIMIT:
        DBGMSG("[app_rec_sound] Received HMSG_MUXER_REACH_LIMIT");
        if (!APP_CHECKFLAGS(app_rec_sound.GFlags, APP_AFLAGS_READY)) {
            break;    // not ready
        }
        ReturnValue = rec_sound.Func(REC_SOUND_MUXER_REACH_LIMIT, param1, param2);
        break;
    case HMSG_DCF_FILE_CLOSE:
        ReturnValue = rec_sound.Func(REC_SOUND_FILE_ID_UPDATE, param1, param2);
        DBGMSG("[app_rec_sound] Received HMSG_DCF_FILE_CLOSE, file object ID = %d",param1);
        break;
    case HMSG_MEMORY_FIFO_BUFFER_RUNOUT:
        DBGMSG("[app_rec_sound] Received HMSG_MEMORY_FIFO_BUFFER_RUNOUT");
        if (!APP_CHECKFLAGS(app_rec_sound.GFlags, APP_AFLAGS_READY)) {
            break;    // not ready
        }
        ReturnValue = rec_sound.Func(REC_SOUND_ERROR_MEMORY_RUNOUT, param1, param2);
        break;
    case HMSG_MUXER_IO_ERROR:
        DBGMSG("[app_rec_sound] Received HMSG_MUXER_IO_ERROR");
        if (!APP_CHECKFLAGS(app_rec_sound.GFlags, APP_AFLAGS_READY)) {
            break;    // not ready
        }
        ReturnValue = rec_sound.Func(REC_SOUND_ERROR_MEMORY_RUNOUT, param1, param2);
        break;
    case HMSG_MUXER_FIFO_ERROR:
        DBGMSG("[app_rec_sound] Received AMP_MUXER_EVENT_FIFO_ERROR");
        if (!APP_CHECKFLAGS(app_rec_sound.GFlags, APP_AFLAGS_READY)) {
            break;    // not ready
        }
        ReturnValue = rec_sound.Func(REC_SOUND_ERROR_MEMORY_RUNOUT, param1, param2);
        break;
    case AMSG_ERROR_CARD_REMOVED:
        if (APP_CHECKFLAGS(app_rec_sound.GFlags, APP_AFLAGS_READY)) {
            ReturnValue = rec_sound.Func(REC_SOUND_CARD_ERROR_REMOVED, param1, param2);
        }
        break;
    case AMSG_STATE_CARD_REMOVED:
        if (APP_CHECKFLAGS(app_rec_sound.GFlags, APP_AFLAGS_READY)) {
            ReturnValue = rec_sound.Func(REC_SOUND_CARD_REMOVED, param1, param2);
        }
        break;
    case HMSG_STORAGE_BUSY:
        if (APP_CHECKFLAGS(app_rec_sound.GFlags, APP_AFLAGS_READY)) {
            ReturnValue = rec_sound.Func(REC_SOUND_CARD_STORAGE_BUSY, param1, param2);
        }
        break;
    case HMSG_STORAGE_IDLE:
        if (APP_CHECKFLAGS(app_rec_sound.GFlags, APP_AFLAGS_READY)) {
            ReturnValue = rec_sound.Func(REC_SOUND_CARD_STORAGE_IDLE, param1, param2);
        }
        break;
    case HMSG_STORAGE_RUNOUT:
        DBGMSG("[app_rec_sound] Received HMSG_STORAGE_RUNOUT");
        if (!APP_CHECKFLAGS(app_rec_sound.GFlags, APP_AFLAGS_READY)) {
            break;    // not ready
        }
        ReturnValue = rec_sound.Func(REC_SOUND_ERROR_STORAGE_RUNOUT, param1, param2);
        break;
    case HMSG_STORAGE_REACH_FILE_LIMIT:
    case HMSG_STORAGE_REACH_FILE_NUMBER:
        break;
    case HMSG_LOOP_ENC_DONE:
        ReturnValue = rec_sound.Func(REC_SOUND_LOOP_ENC_DONE, 0, 0);
        break;
    case HMSG_LOOP_ENC_ERROR:
        ReturnValue = rec_sound.Func(REC_SOUND_ERROR_LOOP_ENC_ERR, param1, 0);
        break;
    case HMSG_HDMI_INSERT_SET:
    case HMSG_HDMI_INSERT_CLR:
    case HMSG_CS_INSERT_SET:
    case HMSG_CS_INSERT_CLR:
        if (!APP_CHECKFLAGS(app_rec_sound.GFlags, APP_AFLAGS_READY)) {
            AmbaPrint("[app_rec_sound] System is not ready. Jack event will be handled later");
        } else {
            ReturnValue = rec_sound.Func(REC_SOUND_UPDATE_FCHAN_VOUT, msg, param1);
        }
        break;
    case HMSG_LINEIN_IN_SET:
    case HMSG_LINEIN_IN_CLR:
        if (!APP_CHECKFLAGS(app_rec_sound.GFlags, APP_AFLAGS_READY)) {
            AmbaPrint("[app_rec_sound] System is not ready. Jack event will be handled later");
        } else {
            ReturnValue = rec_sound.Func(REC_SOUND_AUDIO_INPUT, param1, param2);
        }
        break;
    case HMSG_HP_IN_SET:
    case HMSG_HP_IN_CLR:
    case HMSG_LINEOUT_IN_SET:
    case HMSG_LINEOUT_IN_CLR:
        if (!APP_CHECKFLAGS(app_rec_sound.GFlags, APP_AFLAGS_READY)) {
            AmbaPrint("[app_rec_sound] System is not ready. Jack event will be handled later");
        } else {
            ReturnValue = rec_sound.Func(REC_SOUND_AUDIO_OUTPUT, param1, param2);
        }
        break;
    case AMSG_CMD_USB_APP_START:
    case HMSG_USB_DETECT_CONNECT:
        if (APP_CHECKFLAGS(app_rec_sound.GFlags, APP_AFLAGS_READY) &&
            !APP_CHECKFLAGS(app_rec_sound.GFlags, APP_AFLAGS_BUSY)) {
            ReturnValue = rec_sound.Func(REC_SOUND_USB_CONNECT, param1, param2);
        }
        break;
    case AMSG_STATE_WIDGET_CLOSED:
        ReturnValue = rec_sound.Func(REC_SOUND_WIDGET_CLOSED, param1, param2);
        break;
        case AMSG_STATE_BATTERY_STATE:
            rec_sound.Func(REC_SOUND_UPDATE_BAT_POWER_STATUS, param1, param2);
        break;      
    default:
        break;
    }

    return ReturnValue;
}

/**
 *  @brief The start flow of recoder application.
 *
 *  The start flow of recoder application.
 *
 *  @return >=0 success, <0 failure
 */
static int app_rec_sound_start(void)
{
    int ReturnValue = 0;

    /* Set app function and operate sets */
    rec_sound.Func = rec_sound_func;
    rec_sound.Gui = gui_rec_sound_func;
    rec_sound.Op = &rec_sound_op;
    AmbaPrintColor(GREEN,"==%s==",__FUNCTION__);		

    if (!APP_CHECKFLAGS(app_rec_sound.GFlags, APP_AFLAGS_INIT)) {
        APP_ADDFLAGS(app_rec_sound.GFlags, APP_AFLAGS_INIT);
        rec_sound.Func(REC_SOUND_INIT, 0, 0);
    }

    APP_ADDFLAGS(app_rec_sound.GFlags, APP_AFLAGS_START);
    rec_sound.Func(REC_SOUND_START, 0, 0);

    return ReturnValue;
}

/**
 *  @brief The stop flow of recoder application.
 *
 *  The stop flow of recoder application.
 *
 *  @return >=0 success, <0 failure
 */
static int app_rec_sound_stop(void)
{
    int ReturnValue = 0;

    if (!APP_CHECKFLAGS(app_rec_sound.GFlags, APP_AFLAGS_READY)) {
        rec_sound.Func(REC_SOUND_STOP, 0, 0);
    }

    return ReturnValue;
}
