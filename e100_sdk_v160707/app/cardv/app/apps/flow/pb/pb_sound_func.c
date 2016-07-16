/**
  * @file src/app/apps/flow/pb/connectedcam/pb_sound_func.c
  *
  *  Functions of sound playback application
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

#include <apps/flow/pb/pb_sound.h>
#include <system/status.h>
#include <system/app_pref.h>
#include <system/app_util.h>
#include <apps/gui/resource/gui_settle.h>
#include <apps/flow/widget/menu/widget.h>
#include <AmbaUtility.h>

static void pb_sound_play_timer_handler(int eid)
{
    if (eid == TIMER_UNREGISTER) {
        pb_sound.MediaInfo.PlayTime = 0;
        return;
    }

    pb_sound.MediaInfo.PlayTime+=1000;
    
    //AppLibSoundDec_GetTime(&pb_sound.MediaInfo.PlayTime);
    pb_sound.Gui(GUI_PLAY_TIMER_UPDATE, (UINT32)pb_sound.MediaInfo.PlayTime/1000, (UINT32)pb_sound.MediaInfo.TotalTime/1000);
    pb_sound.Gui(GUI_FLUSH, 0, 0);
}

static int pb_sound_init(void)
{
    int ReturnValue = 0;

    pb_sound.FileInfo.MediaRoot = APPLIB_DCF_MEDIA_AUDIO;

    return ReturnValue;
}

static int pb_sound_start(void)
{
    int ReturnValue = 0;
    UserSetting->SystemPref.SystemMode = APP_MODE_DEC;
    memset(&pb_sound.MediaInfo, 0, sizeof(PB_SOUND_MEDIA_INFO_s));

    /* Init decoder. Decode black screen rather than use background source. */
    AppLibThmBasic_Init();

    /** Set menus */
    AppMenu_Reset();
    AppMenu_RegisterTab(MENU_SETUP);
    AppMenu_RegisterTab(MENU_PBACK);

    /** Initialize the demuxer. */
    AppLibFormat_DemuxerInit();

    /** Initialize the sound decoder. */
    AppLibAudioDec_SetVolume(UserSetting->AudioPref.AudioVolume);
    AppLibAudioDec_Init();

    return ReturnValue;
}

static int pb_sound_stop(void)
{
    int ReturnValue = 0;

    /* Deinit decoder. */
    AppLibThmBasic_Deinit();

    /* Stop the play timer. */
    AppLibComSvcTimer_Unregister(TIMER_1HZ, pb_sound_play_timer_handler);
    pb_sound.Gui(GUI_PLAY_TIMER_HIDE, 0, 0);

    /* Close the sound player. */
    AppLibAudioDec_Exit(); //mark wait for applib func fix

    /* Close the menu or dialog. */
    AppWidget_Off(WIDGET_ALL, WIDGET_HIDE_SILENT);
    APP_REMOVEFLAGS(app_pb_sound.GFlags, APP_AFLAGS_POPUP);

    /* Disable the vout. */
    AppLibDisp_DeactivateWindow(DISP_CH_FCHAN, AppLibDisp_GetWindowId(DISP_CH_FCHAN, 0));
    AppLibDisp_DeactivateWindow(DISP_CH_DCHAN, AppLibDisp_GetWindowId(DISP_CH_DCHAN, 0));
    AppLibDisp_ChanStop(DISP_CH_FCHAN);

    /* Hide GUI */
    pb_sound.Gui(GUI_HIDE_ALL, 0, 0);
    pb_sound.Gui(GUI_FLUSH, 0, 0);

    return ReturnValue;
}

static int pb_sound_app_ready(void)
{
    int ReturnValue = 0;

    if (!APP_CHECKFLAGS(app_pb_sound.GFlags, APP_AFLAGS_READY)) {
        APP_ADDFLAGS(app_pb_sound.GFlags, APP_AFLAGS_READY);

        // ToDo: need to remove to handler when iav completes the dsp cmd queue mechanism
        AppLibGraph_Init();
        pb_sound.Func(PB_SOUND_CHANGE_OSD, 0, 0);

        AppUtil_ReadyCheck(0);
        if (!APP_CHECKFLAGS(app_pb_sound.GFlags, APP_AFLAGS_READY)) {
            /* The system could switch the current app to other in the function "AppUtil_ReadyCheck". */
            return ReturnValue;
        }
    }

    if (APP_CHECKFLAGS(app_pb_sound.GFlags, APP_AFLAGS_READY)) {
        pb_sound.Func(PB_SOUND_START_DISP_PAGE, 0, 0);
    }

    return ReturnValue;
}

/**
 *  @brief Get the initial file infomation.
 *
 *  Get the initial file infomation.
 *
 *  @return >=0 success, <0 failure
 */
static int pb_sound_init_file_info(void)
{
    int ReturnValue = 0;

    ReturnValue = AppLibCard_CheckStatus(0);

    if (ReturnValue == 0) {
        pb_sound.FileInfo.TotalFileNum = AppLibStorageDmf_GetFileAmount(pb_sound.FileInfo.MediaRoot, DCIM_HDLR);	
    } else {
        pb_sound.FileInfo.TotalFileNum = 0;
    }
    AmbaPrintColor(GREEN, "[app_pb_sound] pb_sound.FileInfo.TotalFileNum: %d", pb_sound.FileInfo.TotalFileNum);
    if (pb_sound.FileInfo.TotalFileNum > 0) {
        int i = 0;
        UINT32 CurrFilePos = AppLibStorageDmf_GetCurrFilePos(pb_sound.FileInfo.MediaRoot, DCIM_HDLR);
        if (CurrFilePos == 0) {
            CurrFilePos = AppLibStorageDmf_GetLastFilePos(pb_sound.FileInfo.MediaRoot, DCIM_HDLR);
        }
        AppLibStorageDmf_GetFileName(pb_sound.FileInfo.MediaRoot, ".AAC", APPLIB_DCF_EXT_OBJECT_SPLIT_FILE, 0, 1,CurrFilePos, pb_sound.CurFn);
        pb_sound.CurFileObjID = CurrFilePos;
        ReturnValue = AppLibStorageDmf_GetLastFilePos(pb_sound.FileInfo.MediaRoot, DCIM_HDLR);
        for (i = (pb_sound.FileInfo.TotalFileNum - 1) ; i >= 0; i--) {
            if (ReturnValue == CurrFilePos) {
                pb_sound.FileInfo.FileCur = i;
                break;
            } else {
                ReturnValue = AppLibStorageDmf_GetPrevFilePos(pb_sound.FileInfo.MediaRoot, DCIM_HDLR);
            }
        }
        AmbaPrintColor(GREEN, "[app_pb_sound] pb_sound.FileInfo.FileCur: %d, CurrFilePos =%d", pb_sound.FileInfo.FileCur,CurrFilePos);
    } else {
        pb_sound.FileInfo.TotalFileNum = 0;
        pb_sound.FileInfo.FileCur = 0;
        pb_sound.CurFileObjID = 0;
        memset(pb_sound.CurFn, 0, MAX_FILENAME_LENGTH*sizeof(char));
    }

    return ReturnValue;
}

static int pb_sound_start_disp_page(void)
{
    int ReturnValue = 0;

    ReturnValue = pb_sound_init_file_info();
    
    if (pb_sound.FileInfo.TotalFileNum > 0) {
        pb_sound.Func(PB_SOUND_GUI_INIT_SHOW, 0, 0);

        pb_sound.Func(PB_SOUND_GET_FILE, GET_CURR_FILE, 0);
        pb_sound.Func(PB_SOUND_OPEN, PB_SOUND_OPEN_RESET, 0);

        /* Play the sound clip. */
        pb_sound.MediaInfo.State = PB_SOUND_PLAY_PLAY;
        pb_sound.Func(PB_SOUND_PLAY, 0, 0);
        pb_sound.Gui(GUI_PLAY_STATE_UPDATE, GUI_FWD_NORMAL, 0);
        pb_sound.Gui(GUI_FLUSH, 0, 0);

    } else {
        pb_sound.Func(PB_SOUND_SWITCH_APP, 0, 0);
    }
    return ReturnValue;
}

/**
 *  @brief Get a certain file of photo
 *
 *  Get a certain file of photo
 *
 *  @param [in] param Indicate to get previous, current or next file.
 *
 *  @return >=0 success, <0 failure
 */
static int pb_sound_get_file(UINT32 param)
{
    int ReturnValue = 0;
    int i = 0;
    char TempFn[MAX_FILENAME_LENGTH] = {0};
    //APPLIB_MEDIA_INFO_s MediaInfo;

    UINT32 CurrFilePos = AppLibStorageDmf_GetCurrFilePos(pb_sound.FileInfo.MediaRoot, DCIM_HDLR);
    AmbaPrintColor(GREEN, "[app_pb_sound] pb_sound.FileInfo.FileCur: %d, CurrFilePos =%d", pb_sound.FileInfo.FileCur,CurrFilePos);
    for (i=0; i<pb_sound.FileInfo.TotalFileNum; i++) {
        switch (param) {
        case GET_PREV_FILE:
            if (pb_sound.FileInfo.FileCur == 0) {
                pb_sound.FileInfo.FileCur = pb_sound.FileInfo.TotalFileNum - 1;
                AppLibStorageDmf_GetLastFilePos(pb_sound.FileInfo.MediaRoot, DCIM_HDLR);
            } else {
                pb_sound.FileInfo.FileCur --;
                AppLibStorageDmf_GetPrevFilePos(pb_sound.FileInfo.MediaRoot, DCIM_HDLR);
            }
            break;
        case GET_CURR_FILE:
           AppLibStorageDmf_GetCurrFilePos(pb_sound.FileInfo.MediaRoot, DCIM_HDLR);
            break;
        case GET_NEXT_FILE:
        default:
            if (pb_sound.FileInfo.FileCur == (pb_sound.FileInfo.TotalFileNum - 1)) {
                pb_sound.FileInfo.FileCur = 0;
                AppLibStorageDmf_GetFirstFilePos(pb_sound.FileInfo.MediaRoot, DCIM_HDLR);
            } else {
                pb_sound.FileInfo.FileCur ++;
                AppLibStorageDmf_GetNextFilePos(pb_sound.FileInfo.MediaRoot, DCIM_HDLR);
            }
            break;
        }
        AppLibStorageDmf_GetFileName(pb_sound.FileInfo.MediaRoot, ".AAC", APPLIB_DCF_EXT_OBJECT_SPLIT_FILE, 0, 1, AppLibStorageDmf_GetCurrFilePos(pb_sound.FileInfo.MediaRoot, DCIM_HDLR), TempFn);

        //ReturnValue = AppLibFormat_GetMediaInfo(TempFn, &MediaInfo);
        //if ((ReturnValue == AMP_OK) && (MediaInfo.MediaInfoType == AMP_MEDIA_INFO_SOUND)) {
        if(1){
            pb_sound.CurFileObjID = CurrFilePos;
            strcpy(pb_sound.CurFn, TempFn);
            break;
        } else if (param == GET_CURR_FILE) {
            param = GET_NEXT_FILE;
        }
    }

    return ReturnValue;
}

/**
 * @brief Configure parameters of photo.
 *
 * @param fn - filename
 * @param info - media information
 * @return >=0 success
 *         <0 failure
 */
static int pb_sound_config_media_info(char *fn)
{
    int ReturnValue = 0;
    AMBA_FS_STAT Fstat;
    int frame_num;
#define AAC_FRAME_SIZE  (340)

    ReturnValue = AmbaFS_Stat((const char *)fn, &Fstat);
    if (Fstat.Size > 0) {
        frame_num = Fstat.Size/340;
        pb_sound.MediaInfo.TotalTime = (frame_num*1024)/48;
    }else{
        pb_sound.MediaInfo.TotalTime = 0;
    }
    
    return ReturnValue;
}

/**
 *  @brief Open the clip.
 *
 *  Open the clip.
 *
 *  @param [in] param Parameter
 *
 *  @return >=0 success, <0 failure
 */
static int pb_sound_open(UINT32 param)
{
    int ReturnValue = 0;
    WCHAR FileName[MAX_FILENAME_LENGTH];

    pb_sound_config_media_info(pb_sound.CurFn);

    AmbaUtility_Ascii2Unicode(pb_sound.CurFn, FileName);
    pb_sound.Gui(GUI_PLAY_TIMER_UPDATE, 0, 0);
    pb_sound.Gui(GUI_PLAY_TIMER_SHOW, 0, 0);
    pb_sound.Gui(GUI_FILENAME_UPDATE, (UINT32)FileName, GUI_PB_FN_STYLE_HYPHEN);
    pb_sound.Gui(GUI_FILENAME_SHOW, 0, 0);
    pb_sound.Gui(GUI_FLUSH, 0, 0);

    pb_sound.MediaInfo.PlayTime = 0;
    pb_sound.MediaInfo.Speed = PBACK_SPEED_NORMAL;
    AppLibAudioDec_Start(pb_sound.CurFn);
    AmbaPrintColor(GREEN, "[app_pb_sound] Play %s", pb_sound.CurFn);
            
        ReturnValue = pb_sound.Func(PB_SOUND_PLAY, 0, 0);
        if (!APP_CHECKFLAGS(app_pb_sound.GFlags, APP_AFLAGS_BUSY)) {
            /* The busy flag will be removed when the flow stop the sound player. */
            /* To excute the functions that system block them when the Busy flag is enabled. */
            AppUtil_BusyCheck(0);
        }
        if (pb_sound.MediaInfo.State == PB_SOUND_PLAY_PLAY) {
            if (pb_sound.MediaInfo.Speed < PBACK_SPEED_NORMAL) {
                if (pb_sound.MediaInfo.Direction == PB_SOUND_PLAY_REV) {
                    pb_sound.Gui(GUI_PLAY_STATE_UPDATE, GUI_REW_SLOW, pb_sound.MediaInfo.Speed);
                } else {
                    pb_sound.Gui(GUI_PLAY_STATE_UPDATE, GUI_FWD_SLOW, pb_sound.MediaInfo.Speed);
                }
            } else if (pb_sound.MediaInfo.Speed > PBACK_SPEED_NORMAL) {
                if (pb_sound.MediaInfo.Direction == PB_SOUND_PLAY_REV) {
                    pb_sound.Gui(GUI_PLAY_STATE_UPDATE, GUI_REW_FAST, pb_sound.MediaInfo.Speed);
                } else {
                    pb_sound.Gui(GUI_PLAY_STATE_UPDATE, GUI_FWD_FAST, pb_sound.MediaInfo.Speed);
                }
            }
        }

        pb_sound.Gui(GUI_PLAY_TIMER_UPDATE, pb_sound.MediaInfo.PlayTime/1000, pb_sound.MediaInfo.TotalTime/1000);
        pb_sound.Gui(GUI_FLUSH, 0, 0);
    
    if (ReturnValue >= 0) {
        AppLibComSvcTimer_Register(TIMER_1HZ, pb_sound_play_timer_handler);
        //AppLibAudioDec_GetTime(&pb_sound.MediaInfo.PlayTime);
       // pb_sound.Gui(GUI_PLAY_TIMER_UPDATE, pb_sound.MediaInfo.PlayTime/1000, pb_sound.MediaInfo.TotalTime/1000);
       // pb_sound.Gui(GUI_FLUSH, 0, 0);
    }

    return ReturnValue;
}

/**
 *  @brief Open and decode the sound.
 *
 *  Open and decode the sound.
 *
 *  @param [in] param Parameter
 *
 *  @return >=0 success, <0 failure
 */
static int pb_sound_open_play_curr(UINT32 param)
{
    int ReturnValue = 0;
    if (pb_sound.MediaInfo.State == PB_SOUND_PLAY_PLAY) {
        if (param == 0) {
            //AppLibSoundDec_Resume();
        } else {
            AmbaPrintColor(MAGENTA,"[app_pb_sound] Play Direction %d,StartTime %d",pb_sound.MediaInfo.Direction,pb_sound.MediaInfo.PlayTime);
            ReturnValue = AppLibAudioDec_Start(pb_sound.CurFn);
            }
        APP_ADDFLAGS(app_pb_sound.GFlags, APP_AFLAGS_BUSY);
    } else if (pb_sound.MediaInfo.State == PB_SOUND_PLAY_PAUSED) {
        //AppLibSoundDec_Pause();
        APP_REMOVEFLAGS(app_pb_sound.GFlags, APP_AFLAGS_BUSY);
    } else {
        AmbaPrintColor(RED,"[app_pb_sound] pb_sound_open_play_curr error");
    }

    return ReturnValue;
}


/**
 *  @brief The flow after receiving the message EOS(End of stream)
 *
 *  The flow after receiving the message EOS(End of stream)
 *
 *  @return >=0 success, <0 failure
 */
static int pb_sound_do_play_eos(void)
{
    int ReturnValue = 0;
    char TempFn[MAX_FILENAME_LENGTH] = {0};

    AppLibComSvcTimer_Unregister(TIMER_1HZ, pb_sound_play_timer_handler);
    if (pb_sound.MediaInfo.Direction == PB_SOUND_PLAY_REV) {
        pb_sound.Gui(GUI_PLAY_TIMER_UPDATE, 0/1000, pb_sound.MediaInfo.TotalTime/1000);
    } else {
        pb_sound.Gui(GUI_PLAY_TIMER_UPDATE, pb_sound.MediaInfo.TotalTime/1000, pb_sound.MediaInfo.TotalTime/1000);
    }
    pb_sound.Gui(GUI_FLUSH, 0, 0);

    switch (UserSetting->PlaybackPref.SoundPlayOpt) {
    case PB_OPT_SOUND_PLAY_ALL:
        AmbaPrint("[app_pb_sound] PB_OPT_SOUND_PLAY_ALL");
        AppLibAudioDec_Stop();
        strcpy(TempFn, pb_sound.CurFn);
        if (pb_sound.MediaInfo.Direction == PB_SOUND_PLAY_REV) {
            ReturnValue = pb_sound.Func(PB_SOUND_GET_FILE, GET_PREV_FILE, 0);
        } else {
            ReturnValue = pb_sound.Func(PB_SOUND_GET_FILE, GET_NEXT_FILE, 0);
        }
        if (strcmp(pb_sound.CurFn, pb_sound.FirstFn) == 0) {
            pb_sound.MediaInfo.State = PB_SOUND_PLAY_PAUSED;
            pb_sound.Func(PB_SOUND_PLAY, 0, 0);
            if (!APP_CHECKFLAGS(app_pb_sound.GFlags, APP_AFLAGS_BUSY)) {
                /* The busy flag will be removed when the flow stop the sound player. */
                /* To excute the functions that system block them when the Busy flag is enabled. */
                AppUtil_BusyCheck(0);
            }
            if (APP_CHECKFLAGS(app_pb_sound.GFlags, APP_AFLAGS_READY)) {
                /* The system could switch the current app to other in the function "AppUtil_BusyCheck". */
                pb_sound.Func(PB_SOUND_SWITCH_APP, 0, 0);
            }
        } else {
            //            ReturnValue = app_format_get_media_info2(pb_sound.CurFn, &media_info);
            if (ReturnValue == 0) {
            //                pb_sound.Gui(GUI_MEDIA_INFO_UPDATE, 0, &media_info);
                ReturnValue = pb_sound.Func(PB_SOUND_OPEN, PB_SOUND_OPEN_SOUND_CONT, 0);
            } else {
                ReturnValue = pb_sound.Func(PB_SOUND_SWITCH_APP, 0, 0);
            }
        }
        break;
    case PB_OPT_SOUND_REPEAT_ONE:
        AmbaPrint("[app_pb_sound] PB_OPT_SOUND_REPEAT_ONE");
        if (pb_sound.MediaInfo.Direction == PB_SOUND_PLAY_REV) {
            pb_sound.MediaInfo.PlayTime = pb_sound.MediaInfo.TotalTime;
            //ReturnValue = app_pback_sound_search_time(pb_sound.MediaInfo.TotalTime);
        } else {
            pb_sound.MediaInfo.PlayTime = 0;
            //ReturnValue = app_pback_sound_search_time(0);
        }
        pb_sound.Func(PB_SOUND_PLAY, 0, 0);
        if (!APP_CHECKFLAGS(app_pb_sound.GFlags, APP_AFLAGS_BUSY)) {
            /* The busy flag will be removed when the flow stop the sound player. */
            /* To excute the functions that system block them when the Busy flag is enabled. */
            AppUtil_BusyCheck(0);
        }
        pb_sound.Gui(GUI_PLAY_TIMER_UPDATE, pb_sound.MediaInfo.PlayTime/1000, pb_sound.MediaInfo.TotalTime/1000);
        pb_sound.Gui(GUI_FLUSH, 0, 0);
        break;
    case PB_OPT_SOUND_REPEAT_ALL:
        AmbaPrint("[app_pb_sound] PB_OPT_SOUND_REPEAT_ALL");
        AppLibAudioDec_Stop();
        if (pb_sound.MediaInfo.Direction == PB_SOUND_PLAY_REV) {
            ReturnValue = pb_sound.Func(PB_SOUND_GET_FILE, GET_PREV_FILE, 0);
        } else {
            ReturnValue = pb_sound.Func(PB_SOUND_GET_FILE, GET_NEXT_FILE, 0);
        }
        //ReturnValue = app_format_get_media_info2(pb_sound.CurFn, &media_info);
        if (ReturnValue == 0) {
            //pb_sound.Gui(GUI_MEDIA_INFO_UPDATE, 0, &media_info);
            ReturnValue = pb_sound.Func(PB_SOUND_OPEN, PB_SOUND_OPEN_SOUND_CONT, 0);
        } else {
            ReturnValue = pb_sound.Func(PB_SOUND_SWITCH_APP, 0, 0);
        }
        break;
    case PB_OPT_SOUND_PLAY_ONE:
    default:
        AmbaPrint("[app_pb_sound] PB_OPT_SOUND_PLAY_ONE");
        pb_sound.MediaInfo.State = PB_SOUND_PLAY_PAUSED;
        ReturnValue = pb_sound.Func(PB_SOUND_PLAY, 0, 0);
        AppLibAudioDec_Stop();
        if (!APP_CHECKFLAGS(app_pb_sound.GFlags, APP_AFLAGS_BUSY)) {
            /* The busy flag will be removed when the flow stop the sound player. */
            /* To excute the functions that system block them when the Busy flag is enabled. */
            AppUtil_BusyCheck(0);
        }
        if (APP_CHECKFLAGS(app_pb_sound.GFlags, APP_AFLAGS_READY)) {
            /* The system could switch the current app to other in the function "AppUtil_BusyCheck". */
            pb_sound.Func(PB_SOUND_SWITCH_APP, 0, 0);
        }
        break;
    }

    return ReturnValue;
}

/**
 * @brief The applications switching function.
 *
 * @return >=0 success
 *         <0 failure
 */
static int pb_sound_switch_app(void)
{
    int ReturnValue = 0;

    ReturnValue = AppUtil_SwitchApp(APP_THUMB_MOTION);
    
    return ReturnValue;
}

int pb_sound_card_removed(void)
{
    int ReturnValue = 0;

    if (APP_CHECKFLAGS(app_pb_sound.GFlags, APP_AFLAGS_POPUP)) {
        APP_REMOVEFLAGS(app_pb_sound.Flags, PB_SOUND_DELETE_FILE_RUN);
        AppWidget_Off(WIDGET_ALL, 0);
    }
    pb_sound.Func(PB_SOUND_START_DISP_PAGE, 0, 0);
    pb_sound.Func(PB_SOUND_WARNING_MSG_SHOW_STOP, 0, 0);
    pb_sound.Gui(GUI_CARD_UPDATE, GUI_NO_CARD, 0);
    pb_sound.Gui(GUI_MEDIA_INFO_HIDE, 0, 0);
    pb_sound.Gui(GUI_FILENAME_HIDE, 0, 0);
    pb_sound.Gui(GUI_FLUSH, 0, 0);

    return ReturnValue;
}

int pb_sound_card_error_removed(void)
{
    int ReturnValue = 0;

    APP_REMOVEFLAGS(app_pb_sound.GFlags, APP_AFLAGS_BUSY);
    pb_sound.Func(PB_SOUND_CARD_REMOVED, 0, 0);

    return ReturnValue;
}

int pb_sound_card_new_insert(int param1)
{
    int ReturnValue = 0;

    /* Remove old card.*/
    pb_sound.Func(PB_SOUND_CARD_REMOVED, 0, 0);
    AppLibCard_StatusSetBlock(param1, 0);
    AppLibComSvcAsyncOp_CardInsert(AppLibCard_GetSlot(param1));

    return ReturnValue;
}

int pb_sound_card_storage_idle(void)
{
    int ReturnValue = 0;

    pb_sound.Func(PB_SOUND_SET_FILE_INDEX, 0, 0);

    AppUtil_CheckCardParam(0);
    if (!APP_CHECKFLAGS(app_pb_photo.GFlags, APP_AFLAGS_READY)) {
        return ReturnValue;/**<  App switched out*/
    }

    pb_sound.Gui(GUI_CARD_UPDATE, GUI_CARD_READY, 0);
    pb_sound.Gui(GUI_FLUSH, 0, 0);
    pb_sound.Func(PB_SOUND_START_DISP_PAGE, 0, 0);

    return ReturnValue;
}

/**
 *  @brief The dialog of deletion function
 *
 *  The dialog of deletion function
 *
 *  @param [in] sel Select Yes or No
 *  @param [in] param1 First parameter
 *  @param [in] param2 Second parameter
 *
 *  @return >=0 success, <0 failure
 */
static int pb_sound_dialog_del_handler(UINT32 sel, UINT32 param1, UINT32 param2)
{
    int ReturnValue = 0;

    switch (sel) {
    case DIALOG_SEL_YES:
        pb_sound.Func(PB_SOUND_DELETE_FILE, (UINT32)pb_sound.CurFileObjID, 0);
        break;
    case DIALOG_SEL_NO:
    default:
        break;
    }

    return ReturnValue;
}

static int pb_sound_delete_file_dialog_show(void)
{
    int ReturnValue = 0;

    ReturnValue = AppLibCard_CheckStatus(CARD_CHECK_DELETE);
    if (ReturnValue == 0) {
        AppDialog_SetDialog(DIALOG_TYPE_Y_N, DIALOG_SUB_DEL, pb_sound_dialog_del_handler);
        AppWidget_On(WIDGET_DIALOG, 0);
        APP_ADDFLAGS(app_pb_sound.GFlags, APP_AFLAGS_POPUP);
    } else if (ReturnValue == CARD_STATUS_WP_CARD) {
        pb_sound.Func(PB_SOUND_WARNING_MSG_SHOW_START, GUI_WARNING_CARD_PROTECTED, 0);
        AmbaPrintColor(RED,"[app_pb_sound] WARNING_CARD_PROTECTED");
    } else {
        AmbaPrintColor(RED,"[app_pb_sound] WARNING_CARD_Error rval = %d", ReturnValue);
    }

    return ReturnValue;
}

/**
 *  @brief The deletion function
 *
 *  The deletion function
 *
 *  @param [in] fn filename
 *
 *  @return >=0 success, <0 failure
 */
static int pb_sound_delete_file(UINT32 FileObjID)
{
    int ReturnValue = 0;

    /* Stop player. */
    AppLibAudioDec_Stop();

    AppLibComSvcAsyncOp_DmfFdel(pb_sound.FileInfo.MediaRoot, FileObjID, app_status.PlaybackType);

    APP_ADDFLAGS(app_pb_sound.Flags, PB_SOUND_DELETE_FILE_RUN);
    APP_ADDFLAGS(app_pb_sound.GFlags, APP_AFLAGS_BUSY);
    pb_sound.Func(PB_SOUND_WARNING_MSG_SHOW_START, GUI_WARNING_PROCESSING, 1);

    return ReturnValue;
}

static int pb_sound_delete_file_complete(int param1, int param2)
{
    int ReturnValue = 0;

    if (APP_CHECKFLAGS(app_pb_sound.Flags, PB_SOUND_DELETE_FILE_RUN)) {
        APP_REMOVEFLAGS(app_pb_sound.Flags, PB_SOUND_DELETE_FILE_RUN);
        pb_sound.Gui(GUI_WARNING_HIDE, 0, 0);
        pb_sound.Gui(GUI_FLUSH, 0, 0);
        APP_REMOVEFLAGS(app_pb_sound.GFlags, APP_AFLAGS_BUSY);
        ReturnValue = (int)param1;
        if (ReturnValue < 0) {
            AmbaPrintColor(RED,"[app_pb_sound] Delete files failed: %d", param2);
        } else {
            //pb_sound.Gui(GUI_ZOOM_RATIO_HIDE, 0, 0);
            //pb_sound.Gui(GUI_FLUSH, 0, 0);
            /** page update */
            app_status.ThumbnailModeConti = 0;
            pb_sound.Func(PB_SOUND_START_DISP_PAGE, 0, 0);
        }
    }
    return ReturnValue;
}

static int pb_sound_widget_closed(int param1, int param2)
{
    int ReturnValue = 0;

    if (APP_CHECKFLAGS(app_pb_sound.GFlags, APP_AFLAGS_POPUP)) {
        APP_REMOVEFLAGS(app_pb_sound.GFlags, APP_AFLAGS_POPUP);
    }

    return ReturnValue;
}

static int pb_sound_set_system_type(void)
{
    int ReturnValue = 0;

    return ReturnValue;
}

/**
 *  @brief To change the vout of Fchan
 *
 *  To change the vout of Fchan
 *
 *  @param [in] msg Message ID
 *
 *  @return >=0 success, <0 failure
 */
static int pb_sound_update_fchan_vout(UINT32 msg)
{
    int ReturnValue = 0;

    switch (msg) {
    case HMSG_HDMI_INSERT_SET:
    case HMSG_HDMI_INSERT_CLR:
        AppLibSysVout_SetJackHDMI(app_status.HdmiPluginFlag);
        break;
    case HMSG_CS_INSERT_SET:
    case HMSG_CS_INSERT_CLR:
        AppLibSysVout_SetJackCs(app_status.CompositePluginFlag);
        break;
    default:
        AmbaPrint("[app_pb_sound] Vout no changed");
        return 0;
        break;
    }
    ReturnValue = AppLibDisp_SelectDevice(DISP_CH_FCHAN, DISP_ANY_DEV);
    if (APP_CHECKFLAGS(ReturnValue, DISP_FCHAN_NO_CHANGE)) {
        AmbaPrint("[app_pb_sound] Display FCHAN has no changed");
    } else {
        AppLibAudioDec_Exit();
        if (APP_CHECKFLAGS(ReturnValue, DISP_FCHAN_NO_DEVICE)) {
            AppLibDisp_ChanStop(DISP_CH_FCHAN);
            AppLibDisp_DeactivateWindow(DISP_CH_FCHAN, AppLibDisp_GetWindowId(DISP_CH_FCHAN, 0));
            AppLibDisp_FlushWindow(DISP_CH_FCHAN);
            app_status.LockDecMode = 0;
        } else {
            AppLibDisp_ConfigMode(DISP_CH_FCHAN, AppLibSysVout_GetVoutMode(VOUT_DISP_MODE_2160P_HALF));
            AppLibDisp_SetupChan(DISP_CH_FCHAN);
            AppLibDisp_ChanStart(DISP_CH_FCHAN);
            {
                AMP_DISP_WINDOW_CFG_s Window;
                AMP_DISP_INFO_s DispDev = {0};

                memset(&Window, 0, sizeof(AMP_DISP_WINDOW_CFG_s));

                ReturnValue = AppLibDisp_GetDeviceInfo(DISP_CH_FCHAN, &DispDev);
                if ((ReturnValue < 0) || (DispDev.DeviceInfo.Enable == 0)) {
                    DBGMSG("[app_pb_sound] FChan Disable. Disable the fchan window");
                    AppLibDisp_DeactivateWindow(DISP_CH_FCHAN, AppLibDisp_GetWindowId(DISP_CH_FCHAN, 0));
                    AppLibDisp_FlushWindow(DISP_CH_FCHAN);
                    app_status.LockDecMode = 0;
                } else {
                    /** FCHAN window*/
                    AppLibDisp_GetWindowConfig(DISP_CH_FCHAN, AppLibDisp_GetWindowId(DISP_CH_FCHAN, 0), &Window);
                    Window.Source = AMP_DISP_DEC;
                    Window.SourceDesc.Dec.DecHdlr = 0;
                    Window.CropArea.Width = 0;
                    Window.CropArea.Height = 0;
                    Window.CropArea.X = 0;
                    Window.CropArea.Y = 0;
                    Window.TargetAreaOnPlane.Width = DispDev.DeviceInfo.VoutWidth;
                    Window.TargetAreaOnPlane.Height = DispDev.DeviceInfo.VoutHeight;//  interlance should be consider in MW
                    Window.TargetAreaOnPlane.X = 0;
                    Window.TargetAreaOnPlane.Y = 0;
                    AppLibDisp_SetWindowConfig(DISP_CH_FCHAN, AppLibDisp_GetWindowId(DISP_CH_FCHAN, 0), &Window);
                    AppLibDisp_ActivateWindow(DISP_CH_FCHAN, AppLibDisp_GetWindowId(DISP_CH_FCHAN, 0));
                    AppLibDisp_FlushWindow(DISP_CH_FCHAN);
                    if (app_status.FchanDecModeOnly == 1) {
                        app_status.LockDecMode = 1;
                    }
                }
            }
            AppLibGraph_SetWindowConfig(GRAPH_CH_FCHAN);
            AppLibGraph_ActivateWindow(GRAPH_CH_FCHAN);
            AppLibGraph_FlushWindow(GRAPH_CH_FCHAN);
            pb_sound.Gui(GUI_SET_LAYOUT, 0, 0);
            pb_sound.Gui(GUI_FLUSH, 0, 0);
        }
        AppLibAudioDec_Init();
        pb_sound.Func(PB_SOUND_START_DISP_PAGE, 0, 0);
    }

    return ReturnValue;
}

int pb_sound_change_display(void)
{
    int ReturnValue = 0;

    AppLibDisp_SelectDevice(DISP_CH_FCHAN | DISP_CH_DCHAN, DISP_ANY_DEV);
    AppLibDisp_ConfigMode(DISP_CH_FCHAN | DISP_CH_DCHAN, AppLibSysVout_GetVoutMode(VOUT_DISP_MODE_1080P));
    AppLibDisp_SetupChan(DISP_CH_FCHAN | DISP_CH_DCHAN);
    AppLibDisp_ChanStart(DISP_CH_FCHAN | DISP_CH_DCHAN);
    {
        AMP_DISP_WINDOW_CFG_s Window;
        AMP_DISP_INFO_s DispDev = {0};

        memset(&Window, 0x0, sizeof(AMP_DISP_WINDOW_CFG_s));

        ReturnValue = AppLibDisp_GetDeviceInfo(DISP_CH_FCHAN, &DispDev);
        if ((ReturnValue < 0) || (DispDev.DeviceInfo.Enable == 0)) {
            DBGMSG("[app_usb_msc] FChan Disable. Disable the fchan Window");
            AppLibDisp_DeactivateWindow(DISP_CH_FCHAN, AppLibDisp_GetWindowId(DISP_CH_FCHAN, 0));
        } else {
            /** FCHAN Window*/
            AppLibDisp_GetWindowConfig(DISP_CH_FCHAN, AppLibDisp_GetWindowId(DISP_CH_FCHAN, 0), &Window);
            Window.Source = AMP_DISP_DEC;
            Window.SourceDesc.Dec.DecHdlr = 0;
            Window.CropArea.Width = 0;
            Window.CropArea.Height = 0;
            Window.CropArea.X = 0;
            Window.CropArea.Y = 0;
            Window.TargetAreaOnPlane.Width = DispDev.DeviceInfo.VoutWidth;
            Window.TargetAreaOnPlane.Height = DispDev.DeviceInfo.VoutHeight;
            Window.TargetAreaOnPlane.X = 0;
            Window.TargetAreaOnPlane.Y = 0;
            AppLibDisp_SetWindowConfig(DISP_CH_FCHAN, AppLibDisp_GetWindowId(DISP_CH_FCHAN, 0), &Window);
            AppLibDisp_ActivateWindow(DISP_CH_FCHAN, AppLibDisp_GetWindowId(DISP_CH_FCHAN, 0));
        }

        ReturnValue = AppLibDisp_GetDeviceInfo(DISP_CH_DCHAN, &DispDev);
        if ((ReturnValue < 0) || (DispDev.DeviceInfo.Enable == 0)) {
            DBGMSG("[app_usb_msc] DChan Disable. Disable the Dchan Window");
            AppLibDisp_DeactivateWindow(DISP_CH_DCHAN, AppLibDisp_GetWindowId(DISP_CH_DCHAN, 0));
        } else {
            /** DCHAN Window*/
            AppLibDisp_GetWindowConfig(DISP_CH_DCHAN, AppLibDisp_GetWindowId(DISP_CH_DCHAN, 0), &Window);
            Window.Source = AMP_DISP_DEC;
            Window.SourceDesc.Dec.DecHdlr = 0;
            Window.CropArea.Width = 0;
            Window.CropArea.Height = 0;
            Window.CropArea.X = 0;
            Window.CropArea.Y = 0;
            Window.TargetAreaOnPlane.Width = DispDev.DeviceInfo.VoutWidth;
            Window.TargetAreaOnPlane.Height = DispDev.DeviceInfo.VoutHeight;
            Window.TargetAreaOnPlane.X = 0;
            Window.TargetAreaOnPlane.Y = 0;
            AppLibDisp_SetWindowConfig(DISP_CH_DCHAN, AppLibDisp_GetWindowId(DISP_CH_DCHAN, 0), &Window);
            AppLibDisp_ActivateWindow(DISP_CH_DCHAN, AppLibDisp_GetWindowId(DISP_CH_DCHAN, 0));
        }
        AppLibDisp_FlushWindow(DISP_CH_FCHAN | DISP_CH_DCHAN);
    }

    return ReturnValue;
}


int pb_sound_change_osd(void)
{
    int ReturnValue = 0;

    /* Update graphic window*/
    AppLibGraph_SetWindowConfig(GRAPH_CH_FCHAN | GRAPH_CH_DCHAN);
    AppLibGraph_ActivateWindow(GRAPH_CH_FCHAN | GRAPH_CH_DCHAN);
    AppLibGraph_FlushWindow(GRAPH_CH_FCHAN | GRAPH_CH_DCHAN);
    pb_sound.Gui(GUI_SET_LAYOUT, 0, 0);
    pb_sound.Gui(GUI_FLUSH, 0, 0);

    return ReturnValue;
}

static int pb_sound_usb_connect(void)
{
    int ReturnValue = 0;

    switch (UserSetting->SetupPref.USBMode) {
    default:
        AppUtil_SwitchApp(APP_USB_MSC);
        break;
    }

    return ReturnValue;
}

static int pb_sound_start_show_gui(void)
{
    int ReturnValue = 0;
    int GuiParam = 0;

    /* Clean vout buffer */
    AppLibThmBasic_ClearScreen();

    pb_sound.Gui(GUI_APP_ICON_SHOW, 0, 0);
    pb_sound.Gui(GUI_POWER_STATE_UPDATE, app_status.PowerType, app_status.BatteryState);
    pb_sound.Gui(GUI_POWER_STATE_SHOW, app_status.PowerType, app_status.BatteryState);
    ReturnValue = AppLibCard_CheckStatus(0);
    if (ReturnValue == CARD_STATUS_NO_CARD) {
        GuiParam = GUI_NO_CARD;
    } else {
        GuiParam = GUI_CARD_READY;
    }
    pb_sound.Gui(GUI_CARD_UPDATE, GuiParam, 0);
    pb_sound.Gui(GUI_CARD_SHOW, 0, 0);
    pb_sound.Gui(GUI_PLAY_STATE_UPDATE, GUI_PAUSE, 0);
    pb_sound.Gui(GUI_PLAY_TIMER_UPDATE, 0, pb_sound.MediaInfo.TotalTime/1000);
    // pb_sound.Gui(GUI_PLAY_STATE_SHOW, 0, 0);
    pb_sound.Gui(GUI_MEDIA_INFO_HIDE, 0, 0);
    pb_sound.Gui(GUI_FILENAME_HIDE, 0, 0);
    pb_sound.Gui(GUI_FLUSH, 0, 0);

    return ReturnValue;
}


static int pb_sound_update_bat_power_status(int param1)
{
    int ReturnValue = 0;

    /* Update the gui of power's status. */
    if (param1 == 0) {
        /*Hide the battery gui.*/
        pb_sound.Gui(GUI_POWER_STATE_HIDE, GUI_HIDE_POWER_EXCEPT_DC, 0);
    } else if (param1 == 1) {
        /*Update the battery gui.*/
        pb_sound.Gui(GUI_POWER_STATE_UPDATE, app_status.PowerType, app_status.BatteryState);
        pb_sound.Gui(GUI_POWER_STATE_SHOW, app_status.PowerType, app_status.BatteryState);
    } else if (param1 == 2) {
        /*Reset the battery and power gui.*/
        pb_sound.Gui(GUI_POWER_STATE_HIDE, 0, 0);
        pb_sound.Gui(GUI_POWER_STATE_UPDATE, app_status.PowerType, app_status.BatteryState);
        pb_sound.Gui(GUI_POWER_STATE_SHOW, app_status.PowerType, app_status.BatteryState);
    }
    pb_sound.Gui(GUI_FLUSH, 0, 0);

    return ReturnValue;
}

/**
 *  @brief The timer of warning message.
 *
 *  The timer of warning message.
 *
 *  @param [in] eid event id
 *
 *  @return >=0 success, <0 failure
 */
static void pb_sound_warning_timer_handler(int eid)
{
    static int BlinkCount;

    if (eid == TIMER_UNREGISTER) {
        BlinkCount = 0;
        return;
    }

    BlinkCount++;

    if (BlinkCount & 0x01) {
        pb_sound.Gui(GUI_WARNING_HIDE, 0, 0);
    } else {
        pb_sound.Gui(GUI_WARNING_SHOW, 0, 0);
    }

    if (BlinkCount >= 5) {
        APP_REMOVEFLAGS(app_pb_sound.Flags, PB_SOUND_WARNING_MSG_RUN);
        AppLibComSvcTimer_Unregister(TIMER_2HZ, pb_sound_warning_timer_handler);
        pb_sound.Gui(GUI_WARNING_HIDE, 0, 0);
    }
    pb_sound.Gui(GUI_FLUSH, 0, 0);

}

static int pb_sound_warning_msg_show(int enable, int param1, int param2)
{
    int ReturnValue = 0;

    if (enable) {
        if (param2) {
            pb_sound.Gui(GUI_WARNING_UPDATE, param1, 0);
            pb_sound.Gui(GUI_WARNING_SHOW, 0, 0);
        } else {
            if (!APP_CHECKFLAGS(app_pb_sound.Flags, PB_SOUND_WARNING_MSG_RUN)) {
                APP_ADDFLAGS(app_pb_sound.Flags, PB_SOUND_WARNING_MSG_RUN);
                pb_sound.Gui(GUI_WARNING_UPDATE, param1, 0);
                pb_sound.Gui(GUI_WARNING_SHOW, 0, 0);
                AppLibComSvcTimer_Register(TIMER_2HZ, pb_sound_warning_timer_handler);
            }
        }
    } else {
        if (APP_CHECKFLAGS(app_pb_sound.Flags, PB_SOUND_WARNING_MSG_RUN)) {
            APP_REMOVEFLAGS(app_pb_sound.Flags, PB_SOUND_WARNING_MSG_RUN);
            AppLibComSvcTimer_Unregister(TIMER_2HZ, pb_sound_warning_timer_handler);
        }
        pb_sound.Gui(GUI_WARNING_HIDE, 0, 0);
    }
    pb_sound.Gui(GUI_FLUSH, 0, 0);

    return ReturnValue;
}

/**
 *  @brief The functions of sound playback application
 *
 *  The functions of sound playback application
 *
 *  @param[in] funcId Function id
 *  @param[in] param1 first parameter
 *  @param[in] param2 second parameter
 *
 *  @return >=0 success, <0 failure
 */
int pb_sound_func(UINT32 funcId, UINT32 param1, UINT32 param2)
{
    int ReturnValue = 0;

    switch (funcId) {
    case PB_SOUND_INIT:
        ReturnValue = pb_sound_init();
        break;
    case PB_SOUND_START:
        ReturnValue = pb_sound_start();
        break;
    case PB_SOUND_STOP:
        ReturnValue = pb_sound_stop();
        break;
    case PB_SOUND_APP_READY:
        ReturnValue = pb_sound_app_ready();
        break;
    case PB_SOUND_START_DISP_PAGE:
        ReturnValue = pb_sound_start_disp_page();
        break;
    case PB_SOUND_OPEN:
        ReturnValue = pb_sound_open(param1);
        break;
    case PB_SOUND_PLAY:
        ReturnValue = pb_sound_open_play_curr(param1);
        break;
    case PB_SOUND_EOS:
        ReturnValue = pb_sound_do_play_eos();
        break;
    case PB_SOUND_SWITCH_APP:
        ReturnValue = pb_sound_switch_app();
        break;
    case PB_SOUND_GET_FILE:
        ReturnValue = pb_sound_get_file(param1);
        break;
    case PB_SOUND_CARD_ERROR_REMOVED:
        ReturnValue = pb_sound_card_error_removed();
        break;
    case PB_SOUND_CARD_REMOVED:
        ReturnValue = pb_sound_card_removed();
        break;
    case PB_SOUND_CARD_NEW_INSERT:
        ReturnValue = pb_sound_card_new_insert(param1);
        break;
    case PB_SOUND_CARD_STORAGE_IDLE:
        ReturnValue = pb_sound_card_storage_idle();
        break;
    case PB_SOUND_DELETE_FILE_DIALOG_SHOW:
        ReturnValue = pb_sound_delete_file_dialog_show();
        break;
    case PB_SOUND_DELETE_FILE:
        ReturnValue = pb_sound_delete_file(param1);
        break;
    case PB_SOUND_DELETE_FILE_COMPLETE:
        ReturnValue = pb_sound_delete_file_complete(param1, param2);
        break;
    case PB_SOUND_STATE_WIDGET_CLOSED:
        ReturnValue = pb_sound_widget_closed(param1, param2);
        break;
    case PB_SOUND_SET_SYSTEM_TYPE:
        ReturnValue = pb_sound_set_system_type();
        break;
    case PB_SOUND_UPDATE_FCHAN_VOUT:
        ReturnValue = pb_sound_update_fchan_vout(param1);
        break;
    case PB_SOUND_UPDATE_DCHAN_VOUT:
        break;
    case PB_SOUND_CHANGE_DISPLAY:
        ReturnValue = pb_sound_change_display();
        break;
    case PB_SOUND_CHANGE_OSD:
        ReturnValue = pb_sound_change_osd();
        break;
    case PB_SOUND_USB_CONNECT:
        ReturnValue = pb_sound_usb_connect();
        break;
    case PB_SOUND_GUI_INIT_SHOW:
        ReturnValue = pb_sound_start_show_gui();
        break;
    case PB_SOUND_UPDATE_BAT_POWER_STATUS:
        ReturnValue = pb_sound_update_bat_power_status(param1);
        break;
    case PB_SOUND_WARNING_MSG_SHOW_START:
        ReturnValue = pb_sound_warning_msg_show(1, param1, param2);
        break;
    case PB_SOUND_WARNING_MSG_SHOW_STOP:
        ReturnValue = pb_sound_warning_msg_show(0, param1, param2);
        break;
    default:
        break;
    }

    return ReturnValue;
}
