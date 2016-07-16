/**
  * @file src/app/apps/flow/pb/connectedcam/pb_multi_func.c
  *
  *  Functions of multi playback application
  *
  * History:
  *    2014/09/10 - [Annie Ting] created file
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


#include <apps/flow/pb/pb_multi.h>
#include <system/status.h>
#include <system/app_pref.h>
#include <system/app_util.h>
#include <apps/gui/resource/gui_settle.h>
#include <apps/flow/widget/menu/widget.h>
#include <AmbaUtility.h>
#include <system/ApplibSys_Lcd.h>

#ifdef CONFIG_APP_ARD
/*Record the previous playback media type*/
static AMP_MEDIA_INFO_TYPE_e PrePBType = AMP_MEDIA_INFO_MAX;
#endif

static void pb_multi_play_timer_handler(int eid)
{
    int TimeLatency = 0;
    if (eid == TIMER_UNREGISTER) {
        return;
    }

    AppLibVideoDec_GetTime(&pb_multi.MediaInfo.PlayTime);
    TimeLatency = pb_multi.MediaInfo.PlayTime/1000 - pb_multi.CurPlayTime/1000;
    pb_multi.CurPlayTime = pb_multi.MediaInfo.PlayTime;

    if (TimeLatency == 0) {
        /**Do not update timer when time latency under 1 seconds*/
        return;
    }
    pb_multi.Gui(GUI_PLAY_TIMER_UPDATE, (UINT32)pb_multi.MediaInfo.PlayTime/1000, (UINT32)pb_multi.MediaInfo.TotalTime/1000);
    pb_multi.Gui(GUI_FLUSH, 0, 0);
}

static int pb_multi_init(void)
{
    int ReturnValue = 0;

    return ReturnValue;
}

static int pb_multi_start(void)
{
    int ReturnValue = 0;

    pb_multi.FileInfo.MediaRoot = (APPLIB_DCF_MEDIA_TYPE_e)app_status.ThumbnailModeMediaRoot;
    pb_multi.StillDecWaitFlag = 0;
    UserSetting->SystemPref.SystemMode = APP_MODE_DEC;
    memset(&pb_multi.MediaInfo, 0, sizeof(PB_MULTI_MEDIA_INFO_s));

    /** Set menus */
    //AppMenu_Reset();
    // AppMenu_RegisterTab(MENU_SETUP);
    //AppMenu_RegisterTab(MENU_PBACK);

#ifdef CONFIG_APP_ARD
    // AppMenu_LockItem(MENU_SETUP, MENU_SETUP_FORMAT);
    //AppMenu_LockItem(MENU_PBACK, MENU_PBACK_DELETE_FILE);
    // AppMenu_LockItem(MENU_SETUP, MENU_SETUP_SYSTEM_TYPE);
#endif

    /** Initialize the demuxer. */
    AppLibFormat_DemuxerInit();
#ifdef CONFIG_APP_ARD
    AppLibAudioDec_SetVolume(UserSetting->AudioPref.AudioVolume);
#endif
    /** Initialize the video/photo decoder according to media root type. */
    if (pb_multi.FileInfo.MediaRoot == APPLIB_DCF_MEDIA_VIDEO) {
        AppLibVideoDec_Init();
    } else if (pb_multi.FileInfo.MediaRoot == APPLIB_DCF_MEDIA_IMAGE) {
        AppLibStillSingle_Init();
    } else {
        AppLibVideoDec_Init();
        AppLibStillSingle_Init();
    }
    return ReturnValue;
}

static int pb_multi_stop(void)
{
    int ReturnValue = 0;

    /* Stop the play timer. */
    AppLibComSvcTimer_Unregister(TIMER_10HZ, pb_multi_play_timer_handler);
    pb_multi.Gui(GUI_PLAY_TIMER_HIDE, 0, 0);

    /** Close the video/photo decoder according to media root type. */
    if (pb_multi.FileInfo.MediaRoot == APPLIB_DCF_MEDIA_VIDEO) {
        AppLibVideoDec_Exit();
    } else if (pb_multi.FileInfo.MediaRoot == APPLIB_DCF_MEDIA_IMAGE) {
        AppLibStillSingle_Deinit();
    } else {
        AppLibVideoDec_Exit();
        AppLibStillSingle_Deinit();
    }

    /* Close the menu or dialog. */
    AppWidget_Off(WIDGET_ALL, WIDGET_HIDE_SILENT);
    APP_REMOVEFLAGS(app_pb_multi.GFlags, APP_AFLAGS_POPUP);

#ifdef CONFIG_APP_ARD
            if(AppLibDisp_GetRotate(DISP_CH_DCHAN) == AMP_ROTATE_90){
                AMP_DISP_WINDOW_CFG_s Window;
                AMP_DISP_INFO_s DispDev = {0};
                memset(&Window, 0, sizeof(AMP_DISP_WINDOW_CFG_s));
                ReturnValue = AppLibDisp_GetDeviceInfo(DISP_CH_DCHAN, &DispDev);
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
                Window.SourceDesc.Dec.Rotate= AMP_ROTATE_0;
                AppLibDisp_SetWindowConfig(DISP_CH_DCHAN, AppLibDisp_GetWindowId(DISP_CH_DCHAN, 0), &Window);
                AppLibDisp_ActivateWindow(DISP_CH_DCHAN, AppLibDisp_GetWindowId(DISP_CH_DCHAN, 0));
                AppLibDisp_FlushWindow(DISP_CH_DCHAN);
            }
            PrePBType = AMP_MEDIA_INFO_MAX;
#endif

    /* Disable the vout. */
    AppLibDisp_DeactivateWindow(DISP_CH_FCHAN, AppLibDisp_GetWindowId(DISP_CH_FCHAN, 0));
    AppLibDisp_DeactivateWindow(DISP_CH_DCHAN, AppLibDisp_GetWindowId(DISP_CH_DCHAN, 0));
    AppLibDisp_ChanStop(DISP_CH_FCHAN);

#ifdef CONFIG_APP_ARD
    //AppMenu_UnlockItem(MENU_SETUP, MENU_SETUP_FORMAT);
    //AppMenu_UnlockItem(MENU_PBACK, MENU_PBACK_DELETE_FILE);
    //AppMenu_UnlockItem(MENU_SETUP, MENU_SETUP_SYSTEM_TYPE);
#endif

    /* Hide GUI */
    pb_multi.Gui(GUI_HIDE_ALL, 0, 0);
    pb_multi.Gui(GUI_FLUSH, 0, 0);

    return ReturnValue;
}

static int pb_multi_app_ready(void)
{
    int ReturnValue = 0;

    if (!APP_CHECKFLAGS(app_pb_multi.GFlags, APP_AFLAGS_READY)) {
        APP_ADDFLAGS(app_pb_multi.GFlags, APP_AFLAGS_READY);
        AppUtil_ReadyCheck(0);
        if (!APP_CHECKFLAGS(app_pb_multi.GFlags, APP_AFLAGS_READY)) {
            /* The system could switch the current app to other in the function "AppUtil_ReadyCheck". */
            return ReturnValue;
        }
    }
    // ToDo: need to remove to handler when iav completes the dsp cmd queue mechanism
    AppLibGraph_Init();
    pb_multi.Func(PB_MULTI_CHANGE_OSD, 0, 0);

    if (APP_CHECKFLAGS(app_pb_multi.GFlags, APP_AFLAGS_READY)) {
        pb_multi.Func(PB_MULTI_START_DISP_PAGE, 0, 0);
    }

#ifdef CONFIG_APP_ARD
    AppLibSysLcd_SetBacklight_Directly(0, 1);
    cardv_app_screen_unlock();
#endif

    return ReturnValue;
}

/**
 *  @brief Get the initial file infomation.
 *
 *  Get the initial file infomation.
 *
 *  @return >=0 success, <0 failure
 */
static int pb_multi_init_file_info(void)
{
    UINT64 ReturnValue = 0;

    ReturnValue = AppLibCard_CheckStatus(0);

    if (ReturnValue == 0) {
        pb_multi.FileInfo.TotalFileNum = AppLibStorageDmf_GetFileAmount(pb_multi.FileInfo.MediaRoot, app_status.PlaybackType);
    } else {
        pb_multi.FileInfo.TotalFileNum = 0;
    }
    AmbaPrintColor(GREEN, "[app_pb_multi] pb_multi.FileInfo.TotalFileNum: %d", pb_multi.FileInfo.TotalFileNum);
    if (pb_multi.FileInfo.TotalFileNum > 0) {
        int i = 0;
        UINT64 CurrFilePos = 0;
        CurrFilePos = AppLibStorageDmf_GetCurrFilePos(pb_multi.FileInfo.MediaRoot, app_status.PlaybackType);
        if (CurrFilePos == 0) {
            CurrFilePos = AppLibStorageDmf_GetLastFilePos(pb_multi.FileInfo.MediaRoot, app_status.PlaybackType);
        }
        ReturnValue = AppLibStorageDmf_GetFileName(pb_multi.FileInfo.MediaRoot, ".MP4", APPLIB_DCF_EXT_OBJECT_SPLIT_FILE, app_status.PlaybackType, (CUR_OBJ_IDX(CurrFilePos) + 1),CUR_OBJ(CurrFilePos), pb_multi.CurFn);
        if (ReturnValue != 0) {
            ReturnValue = AppLibStorageDmf_GetFileName(pb_multi.FileInfo.MediaRoot, ".JPG", APPLIB_DCF_EXT_OBJECT_SPLIT_FILE, app_status.PlaybackType, (CUR_OBJ_IDX(CurrFilePos) + 1),CUR_OBJ(CurrFilePos), pb_multi.CurFn);
        }
        pb_multi.CurFileObjID = CurrFilePos;
        ReturnValue = AppLibStorageDmf_GetLastFilePos(pb_multi.FileInfo.MediaRoot, app_status.PlaybackType);
        for (i = (pb_multi.FileInfo.TotalFileNum - 1) ; i >= 0; i--) {
            if (ReturnValue == CurrFilePos) {
                pb_multi.FileInfo.FileCur = i;
                break;
            } else {
                ReturnValue = AppLibStorageDmf_GetPrevFilePos(pb_multi.FileInfo.MediaRoot, app_status.PlaybackType);
            }
        }
        AmbaPrintColor(GREEN, "[app_pb_multi] pb_multi.FileInfo.FileCur: %d, CurrFilePos =%d , index = %d", pb_multi.FileInfo.FileCur,CUR_OBJ(CurrFilePos), CUR_OBJ_IDX(CurrFilePos));
    } else {
        pb_multi.FileInfo.TotalFileNum = 0;
        pb_multi.FileInfo.FileCur = 0;
        pb_multi.CurFileObjID = 0;
        memset(pb_multi.CurFn, 0, MAX_FILENAME_LENGTH*sizeof(char));
    }

    return CUR_OBJ(ReturnValue);
}

#ifdef CONFIG_APP_ARD
static int pb_multi_start_disp_page(int param1, int param2)
#else
static int pb_multi_start_disp_page(int param1)
#endif
{
    int ReturnValue = 0;

    ReturnValue = pb_multi_init_file_info();
#ifdef CONFIG_APP_ARD
    // if param2 == 1, force to NOT enter into playback state, just in thumbnail view.
    if ((pb_multi.FileInfo.TotalFileNum > 0) && (param2 == 0)){
#else
    if (pb_multi.FileInfo.TotalFileNum > 0) {
#endif
        pb_multi.Func(PB_MULTI_GET_FILE, GET_CURR_FILE, pb_multi.FileInfo.MediaRoot);
        pb_multi.Func(PB_MULTI_GUI_INIT_SHOW, 0, 0);
        switch (pb_multi.CurFileType) {
        case PB_MULTI_MEDIA_IMAGE:
            pb_multi.Func(PB_MULTI_PLAY, 0, 0);/**<param2 : 0 photo : 1 video */
            break;
        case PB_MULTI_MEDIA_VIDEO:
            strcpy(pb_multi.FirstFn, pb_multi.CurFn);
            pb_multi.Func(PB_MULTI_OPEN, PB_MULTI_OPEN_RESET, 0);
            /* Play the multi clip. */
            pb_multi.MediaInfo.State = PB_MULTI_PLAY_PLAY;
            pb_multi.Func(PB_MULTI_PLAY, 0, 1);/**<param2 : 0 photo : 1 video */
            pb_multi.Gui(GUI_PLAY_STATE_UPDATE, GUI_FWD_NORMAL, 0);
            break;
        default:
            break;
        }

        pb_multi.Gui(GUI_FLUSH, 0, 0);

    } else {
        pb_multi.Func(PB_MULTI_SWITCH_APP, 0, 0);
    }

    return ReturnValue;
}

static int pb_multi_open_next_previous_file(int param1, int param2)
{
    int ReturnValue = 0;
    pb_multi.Func(PB_MULTI_GET_FILE, param1, pb_multi.FileInfo.MediaRoot);
    switch (pb_multi.CurFileType) {
    case PB_MULTI_MEDIA_IMAGE:
        pb_multi.Func(PB_MULTI_PLAY, 0, 0);/**<param2 : 0 photo : 1 video */
        break;
    case PB_MULTI_MEDIA_VIDEO:
        strcpy(pb_multi.FirstFn, pb_multi.CurFn);
        AppLibVideoDec_Stop();
        pb_multi.Func(PB_MULTI_GUI_INIT_SHOW, 0, 0);
        pb_multi.Func(PB_MULTI_OPEN, PB_MULTI_OPEN_RESET, 0);
        /* Play the multi clip. */
        pb_multi.MediaInfo.State = PB_MULTI_PLAY_PLAY;
        pb_multi.Func(PB_MULTI_PLAY, 0, 1);/**<param2 : 0 photo : 1 video */
        pb_multi.Gui(GUI_PLAY_STATE_UPDATE, GUI_FWD_NORMAL, 0);
        break;
    default:
        break;
    }
    pb_multi.Gui(GUI_FLUSH, 0, 0);

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
static int pb_multi_get_file(UINT32 param, UINT32 MediaType)
{
    int ReturnValue = -1;
    int i = 0;
    UINT64 FilePos = 0;
    char TempFn[MAX_FILENAME_LENGTH] = {0};
    APPLIB_MEDIA_INFO_s MediaInfo;

    UINT64 CurrFilePos;
    CurrFilePos = AppLibStorageDmf_GetCurrFilePos(pb_multi.FileInfo.MediaRoot, app_status.PlaybackType);
    AmbaPrintColor(GREEN, "[app_pb_multi] pb_multi.FileInfo.FileCur: %d, CurrFilePos =%d", pb_multi.FileInfo.FileCur,CurrFilePos);

    for (i=0; i<pb_multi.FileInfo.TotalFileNum; i++) {
        switch (param) {
        case GET_PREV_FILE:
            if (pb_multi.FileInfo.FileCur == 0) {
                pb_multi.FileInfo.FileCur = pb_multi.FileInfo.TotalFileNum - 1;
                FilePos = AppLibStorageDmf_GetLastFilePos(pb_multi.FileInfo.MediaRoot, app_status.PlaybackType);
            } else {
                pb_multi.FileInfo.FileCur --;
                FilePos = AppLibStorageDmf_GetPrevFilePos(pb_multi.FileInfo.MediaRoot, app_status.PlaybackType);
            }
            break;
        case GET_CURR_FILE:
            FilePos = AppLibStorageDmf_GetCurrFilePos(pb_multi.FileInfo.MediaRoot, app_status.PlaybackType);
            break;
        case GET_NEXT_FILE:
        default:
            if (pb_multi.FileInfo.FileCur == (pb_multi.FileInfo.TotalFileNum - 1)) {
                pb_multi.FileInfo.FileCur = 0;
                FilePos = AppLibStorageDmf_GetFirstFilePos(pb_multi.FileInfo.MediaRoot, app_status.PlaybackType);
            } else {
                pb_multi.FileInfo.FileCur ++;
                FilePos = AppLibStorageDmf_GetNextFilePos(pb_multi.FileInfo.MediaRoot, app_status.PlaybackType);
            }
            break;
        }
        switch (MediaType) {
            case APPLIB_DCF_MEDIA_VIDEO:
                ReturnValue = AppLibStorageDmf_GetFileName(pb_multi.FileInfo.MediaRoot, ".MP4", APPLIB_DCF_EXT_OBJECT_SPLIT_FILE, app_status.PlaybackType, (CUR_OBJ_IDX(FilePos) + 1), CUR_OBJ(FilePos), TempFn);
            break;
            case APPLIB_DCF_MEDIA_IMAGE:
                ReturnValue = AppLibStorageDmf_GetFileName(pb_multi.FileInfo.MediaRoot, ".JPG", APPLIB_DCF_EXT_OBJECT_SPLIT_FILE, app_status.PlaybackType, (CUR_OBJ_IDX(FilePos) + 1), CUR_OBJ(FilePos), TempFn);
            break;
            case APPLIB_DCF_MEDIA_AUDIO:
            break;
            case APPLIB_DCF_MEDIA_DCIM:
            default:
                ReturnValue = AppLibStorageDmf_GetFileName(pb_multi.FileInfo.MediaRoot, ".MP4", APPLIB_DCF_EXT_OBJECT_SPLIT_FILE, app_status.PlaybackType, (CUR_OBJ_IDX(FilePos) + 1), CUR_OBJ(FilePos), TempFn);
                if (ReturnValue != 0) {
                    ReturnValue = AppLibStorageDmf_GetFileName(pb_multi.FileInfo.MediaRoot, ".JPG", APPLIB_DCF_EXT_OBJECT_SPLIT_FILE, app_status.PlaybackType, (CUR_OBJ_IDX(FilePos) + 1), CUR_OBJ(FilePos), TempFn);
                }
            break;
        }

        /**if return valuw < 0 means next file is not match require file type, search for next file*/
        if (ReturnValue < 0) {
            if (param == GET_CURR_FILE) {
                param = GET_NEXT_FILE;
            }
            continue;
        }

        ReturnValue = AppLibFormat_GetMediaInfo(TempFn, &MediaInfo);
        if (ReturnValue == AMP_OK) {
            strcpy(pb_multi.CurFn, TempFn);
            pb_multi.CurFileObjID = FilePos;
            pb_multi.CurFileType = MediaInfo.MediaInfoType;
            break;
        } else if (param == GET_CURR_FILE) {
            param = GET_NEXT_FILE;
        }
    }

    return ReturnValue;
}
/**
 * @brief caculate the aspect ratio for display
 *
 * @param width  resolution width
 * @param height resolution height
 * @return aspect ratio
 */
static int pb_multi_cal_aspect_ratio(int width,int height)
{
    int ReturnValue = 0;
    int Temp1,Temp2,Gcd;
    Gcd = height;
    Temp1 = width;
    while (Temp1 % Gcd != 0) {
        Temp2 = Gcd;
        Gcd = Temp1 % Gcd;
        Temp1 = Temp2;
    }
    Temp1 = width/Gcd;
    Temp2 = height/Gcd;
    ReturnValue = Temp1 << 8 | Temp2;
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
static int pb_multi_config_media_info(char *fn)
{
    int ReturnValue = 0;
    //int Width = 0, Height = 0, AspectRatio = 0;
    UINT32 DTS, TimeScale;
    APPLIB_MEDIA_INFO_s MediaInfo;

    ReturnValue = AppLibFormat_GetMediaInfo(fn, &MediaInfo);
    if (ReturnValue != AMP_OK) {
        return -1;
    }

    if (MediaInfo.MediaInfoType == AMP_MEDIA_INFO_MOVIE) {
        DTS = MediaInfo.MediaInfo.Movie->Track[0].NextDTS;
        TimeScale = MediaInfo.MediaInfo.Movie->Track[0].TimeScale;
        /// Dimension information
        pb_multi.MediaInfo.Width = MediaInfo.MediaInfo.Movie->Track[0].Info.Video.Width;
        pb_multi.MediaInfo.Height = MediaInfo.MediaInfo.Movie->Track[0].Info.Video.Height;
        pb_multi.MediaInfo.AspectRatio = pb_multi_cal_aspect_ratio(pb_multi.MediaInfo.Width,pb_multi.MediaInfo.Height);

        /** Update total time in milliseconds */
        ReturnValue = AppLibVideoDec_SetPtsFrame(MediaInfo.MediaInfo.Movie->Track[0].FrameCount,
                    MediaInfo.MediaInfo.Movie->Track[0].TimePerFrame, MediaInfo.MediaInfo.Movie->Track[0].TimeScale);
        if (ReturnValue < 0) {
            AmbaPrintColor(RED,"[app_pb_multi] AppLibVideoDec_SetPtsFrame Failure!");
        }

        pb_multi.MediaInfo.TotalTime = (((UINT64)(DTS) / (TimeScale)) * 1000 + (((UINT64)(DTS) % (TimeScale)) * 1000) /(TimeScale));
        AmbaPrint("[app_pb_multi] MediaInfo.MediaInfo.Movie->Track[0].Info.Video.PixelArX = %d",MediaInfo.MediaInfo.Movie->Track[0].Info.Video.PixelArX);
        AmbaPrint("[app_pb_multi] MediaInfo.MediaInfo.Movie->Track[0].Info.Video.PixelArY = %d",MediaInfo.MediaInfo.Movie->Track[0].Info.Video.PixelArY);
        AmbaPrint("[app_pb_multi] MediaInfo.MediaInfo.Movie->Track[0].FrameCount = %d",MediaInfo.MediaInfo.Movie->Track[0].FrameCount);
        AmbaPrint("[app_pb_multi] MediaInfo.MediaInfo.Movie->Track[0].TimePerFrame = %d",MediaInfo.MediaInfo.Movie->Track[0].TimePerFrame);
        AmbaPrint("[app_pb_multi] MediaInfo.MediaInfo.Movie->Track[0].TimeScale = %d",MediaInfo.MediaInfo.Movie->Track[0].TimeScale);
        AmbaPrint("[app_pb_multi] MediaInfo.MediaInfo.Movie->Track[0].DTS = %lld",MediaInfo.MediaInfo.Movie->Track[0].NextDTS);
        AmbaPrint("[app_pb_multi] pb_multi.MediaInfo.TotalTime = %lld",pb_multi.MediaInfo.TotalTime);
        AmbaPrint("[app_pb_multi] pb_multi.MediaInfo.Width = %d",pb_multi.MediaInfo.Width);
        AmbaPrint("[app_pb_multi] pb_multi.MediaInfo.Height = %d",pb_multi.MediaInfo.Height);
        AmbaPrint("[app_pb_multi] pb_multi.MediaInfo.AspectRatio = %d",pb_multi.MediaInfo.AspectRatio);

    } else if (MediaInfo.MediaInfoType == AMP_MEDIA_INFO_IMAGE) {
#ifdef CONFIG_APP_ARD
        //Just return if it's IMAGE,system will re-config vout after the image showed completely.
        PrePBType = MediaInfo.MediaInfoType;
        return 0;
#else
        pb_multi.MediaInfo.AspectRatio = VAR_4x3;//pb_multi_cal_aspect_ratio(MediaInfo.MediaInfo.Image->Frame[0].Width,MediaInfo.MediaInfo.Image->Frame[0].Height);
#endif
    }

#ifdef CONFIG_APP_ARD
    if (PrePBType != MediaInfo.MediaInfoType || PrePBType == AMP_MEDIA_INFO_MOVIE) {
        if (AppLibDisp_GetRotate(DISP_CH_DCHAN) == AMP_ROTATE_90) {
            AppLibDisp_DeactivateWindow(DISP_CH_DCHAN, AppLibDisp_GetWindowId(DISP_CH_DCHAN, 0));
            AppLibDisp_FlushWindow(DISP_CH_DCHAN);
        }
    }
    PrePBType = MediaInfo.MediaInfoType;
#endif

    {
        AMP_DISP_WINDOW_CFG_s Window;
        APPLIB_VOUT_PREVIEW_PARAM_s PreviewParam={0};
        AMP_DISP_INFO_s DispDev = {0};

        memset(&Window, 0x0, sizeof(AMP_DISP_WINDOW_CFG_s));

        PreviewParam.AspectRatio = pb_multi.MediaInfo.AspectRatio;
        ReturnValue = AppLibDisp_GetDeviceInfo(DISP_CH_FCHAN, &DispDev);
        if ((ReturnValue < 0) || (DispDev.DeviceInfo.Enable == 0)) {
            DBGMSG("[app_pb_multi] FChan Disable. Disable the fchan Window");
            AppLibDisp_DeactivateWindow(DISP_CH_FCHAN, AppLibDisp_GetWindowId(DISP_CH_FCHAN, 0));
        } else {
            /** FCHAN Window*/
            PreviewParam.ChanID = DISP_CH_FCHAN;
            AppLibDisp_CalcPreviewWindowSize(&PreviewParam);
            AppLibDisp_GetWindowConfig(DISP_CH_FCHAN, AppLibDisp_GetWindowId(DISP_CH_FCHAN, 0), &Window);
            Window.Source = AMP_DISP_DEC;
            Window.CropArea.Width = 0;
            Window.CropArea.Height = 0;
            Window.CropArea.X = 0;
            Window.CropArea.Y = 0;
            Window.TargetAreaOnPlane.Width = PreviewParam.Preview.Width;
            Window.TargetAreaOnPlane.Height = PreviewParam.Preview.Height;
            Window.TargetAreaOnPlane.X = PreviewParam.Preview.X;
            Window.TargetAreaOnPlane.Y = PreviewParam.Preview.Y;
            AppLibDisp_SetWindowConfig(DISP_CH_FCHAN, AppLibDisp_GetWindowId(DISP_CH_FCHAN, 0), &Window);
            AppLibDisp_ActivateWindow(DISP_CH_FCHAN, AppLibDisp_GetWindowId(DISP_CH_FCHAN, 0));
        }

        ReturnValue = AppLibDisp_GetDeviceInfo(DISP_CH_DCHAN, &DispDev);
        if ((ReturnValue < 0) || (DispDev.DeviceInfo.Enable == 0)) {
            DBGMSG("[app_pb_multi] DChan Disable. Disable the Dchan Window");
            AppLibDisp_DeactivateWindow(DISP_CH_DCHAN, AppLibDisp_GetWindowId(DISP_CH_DCHAN, 0));
        } else {
            /** DCHAN Window*/
            PreviewParam.ChanID = DISP_CH_DCHAN;
            AppLibDisp_CalcPreviewWindowSize(&PreviewParam);
            AppLibDisp_GetWindowConfig(DISP_CH_DCHAN, AppLibDisp_GetWindowId(DISP_CH_DCHAN, 0), &Window);
            Window.Source = AMP_DISP_DEC;
            Window.CropArea.Width = 0;
            Window.CropArea.Height = 0;
            Window.CropArea.X = 0;
            Window.CropArea.Y = 0;
            Window.TargetAreaOnPlane.Width = PreviewParam.Preview.Width;
            Window.TargetAreaOnPlane.Height = PreviewParam.Preview.Height;
            Window.TargetAreaOnPlane.X = PreviewParam.Preview.X;
            Window.TargetAreaOnPlane.Y = PreviewParam.Preview.Y;
#ifdef CONFIG_APP_ARD
            if(AppLibDisp_GetRotate(DISP_CH_DCHAN) == AMP_ROTATE_90){
                Window.SourceDesc.Dec.Rotate= AMP_ROTATE_90;
            }
#endif
            AppLibDisp_SetWindowConfig(DISP_CH_DCHAN, AppLibDisp_GetWindowId(DISP_CH_DCHAN, 0), &Window);
            AppLibDisp_ActivateWindow(DISP_CH_DCHAN, AppLibDisp_GetWindowId(DISP_CH_DCHAN, 0));
        }
        AppLibDisp_FlushWindow(DISP_CH_FCHAN | DISP_CH_DCHAN);
    }
#if 0
    {
        char ft[64];
        uni_to_asc(pb_multi.CurFn, ft);
        AmbaPrint("Config current file: %s", ft);
    }
#endif
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
static int pb_multi_open(UINT32 param)
{
    APPLIB_VIDEO_START_INFO_s VideoStartInfo; // Video configuration
    int ReturnValue = 0;
    WCHAR FileName[MAX_FILENAME_LENGTH];

    AmbaUtility_Ascii2Unicode(pb_multi.CurFn, FileName);
    pb_multi_config_media_info(pb_multi.CurFn);

    pb_multi.Gui(GUI_PLAY_TIMER_UPDATE, 0, 0);
    pb_multi.Gui(GUI_PLAY_TIMER_SHOW, 0, 0);
    pb_multi.Gui(GUI_FILENAME_UPDATE, (UINT32)FileName, GUI_PB_FN_STYLE_HYPHEN);
    pb_multi.Gui(GUI_FILENAME_SHOW, 0, 0);
    pb_multi.Gui(GUI_FLUSH, 0, 0);
    // Set video configuration
    AppLibVideoDec_GetStartDefaultCfg(&VideoStartInfo);
    VideoStartInfo.Filename = pb_multi.CurFn;
#ifdef CONFIG_APP_ARD
    VideoStartInfo.AutoPlay = 1; // Open and pause
    APP_ADDFLAGS(app_pb_multi.GFlags, APP_AFLAGS_BUSY);
#else
    VideoStartInfo.AutoPlay = 0; // Open and pause
#endif
    if (param == PB_MULTI_OPEN_RESET) {
        /** Update play time */
        pb_multi.MediaInfo.PlayTime = 0;
        pb_multi.CurPlayTime = pb_multi.MediaInfo.PlayTime;
        pb_multi.MediaInfo.Direction = PB_MULTI_PLAY_FWD;
        pb_multi.MediaInfo.State = PB_MULTI_PLAY_PAUSED;
        pb_multi.MediaInfo.Speed = PBACK_SPEED_NORMAL;
        ReturnValue = AppLibVideoDec_Start(&VideoStartInfo);
    } else {    /** PB_MULTI_OPEN_VIDEO_CONT */
        /**use current setting to start decode*/
        VideoStartInfo.Direction = pb_multi.MediaInfo.Direction;
        VideoStartInfo.ResetSpeed = 0;
        VideoStartInfo.ResetZoom = 0;
        if (pb_multi.MediaInfo.Direction == PB_MULTI_PLAY_REV) {
            pb_multi.MediaInfo.PlayTime = pb_multi.MediaInfo.TotalTime;
            VideoStartInfo.StartTime = pb_multi.MediaInfo.TotalTime;
        } else {    /** PB_MULTI_PLAY_FWD */
            pb_multi.MediaInfo.PlayTime = 0;
            VideoStartInfo.StartTime = 0;
        }
        pb_multi.CurPlayTime = VideoStartInfo.StartTime;
        ReturnValue = AppLibVideoDec_Start(&VideoStartInfo);
        ReturnValue = pb_multi.Func(PB_MULTI_PLAY, 0, 1);
        if (!APP_CHECKFLAGS(app_pb_multi.GFlags, APP_AFLAGS_BUSY)) {
            /* The busy flag will be removed when the flow stop the video player. */
            /* To excute the functions that system block them when the Busy flag is enabled. */
            AppUtil_BusyCheck(0);
        }
        if (pb_multi.MediaInfo.State == PB_MULTI_PLAY_PLAY) {
            if (pb_multi.MediaInfo.Speed < PBACK_SPEED_NORMAL) {
                if (pb_multi.MediaInfo.Direction == PB_MULTI_PLAY_REV) {
                    pb_multi.Gui(GUI_PLAY_STATE_UPDATE, GUI_REW_SLOW, pb_multi.MediaInfo.Speed);
                } else {
                    pb_multi.Gui(GUI_PLAY_STATE_UPDATE, GUI_FWD_SLOW, pb_multi.MediaInfo.Speed);
                }
            } else if (pb_multi.MediaInfo.Speed > PBACK_SPEED_NORMAL) {
                if (pb_multi.MediaInfo.Direction == PB_MULTI_PLAY_REV) {
                    pb_multi.Gui(GUI_PLAY_STATE_UPDATE, GUI_REW_FAST, pb_multi.MediaInfo.Speed);
                } else {
                    pb_multi.Gui(GUI_PLAY_STATE_UPDATE, GUI_FWD_FAST, pb_multi.MediaInfo.Speed);
                }
            }
        }
    }
    if (ReturnValue >= 0) {
        AppLibComSvcTimer_Register(TIMER_10HZ, pb_multi_play_timer_handler);
        //AppLibVideoDec_GetTime(&pb_multi.MediaInfo.PlayTime);
        //pb_multi.Gui(GUI_PLAY_TIMER_UPDATE, pb_multi.MediaInfo.PlayTime/1000, pb_multi.MediaInfo.TotalTime/1000);
        //pb_multi.Gui(GUI_FLUSH, 0, 0);
    }

    return ReturnValue;
}

/**
 *  @brief Open and decode the video.
 *
 *  Open and decode the video.
 *
 *  @param [in] param Parameter
 *
 *  @return >=0 success, <0 failure
 */
static int pb_multi_video_open_play_curr(UINT32 param)
{
    int ReturnValue = 0;
    APPLIB_VIDEO_START_MULTI_INFO_s VideoStartInfo;
    APPLIB_VIDEO_FILE_INFO_s File;
    if (pb_multi.MediaInfo.State == PB_MULTI_PLAY_PLAY) {
        File.Filename = pb_multi.CurFn;
        VideoStartInfo.File = &File;
        VideoStartInfo.FileNum= 1;
        VideoStartInfo.StartTime = pb_multi.CurPlayTime;
        VideoStartInfo.Direction = (APPLIB_VIDEO_PLAY_DIRECTION_e)pb_multi.MediaInfo.Direction;
        VideoStartInfo.ResetSpeed = param;  // depends on param to decide reset speed or not.
        VideoStartInfo.ResetZoom = 0;
        VideoStartInfo.AutoPlay = 1;
        VideoStartInfo.ReloadFile = 0;
        AmbaPrintColor(MAGENTA,"[app_pb_multi] Play Direction %d,StartTime %d",pb_multi.MediaInfo.Direction,VideoStartInfo.StartTime);
        ReturnValue = AppLibVideoDec_StartMultiple(&VideoStartInfo);
        APP_ADDFLAGS(app_pb_multi.GFlags, APP_AFLAGS_BUSY);
    } else if (pb_multi.MediaInfo.State == PB_MULTI_PLAY_PAUSED) {
        AppLibVideoDec_Pause();
        APP_REMOVEFLAGS(app_pb_multi.GFlags, APP_AFLAGS_BUSY);
    } else {
        AmbaPrintColor(RED,"[app_pb_multi] pb_multi_open_play_curr error");
    }

    return ReturnValue;
}

/**
 *  @brief Open and decode the photo.
 *
 *  Open and decode the photo.
 *
 *  @param [in] param Parameter
 *
 *  @return >=0 success, <0 failure
 */
static int pb_multi_photo_open_play_curr(UINT32 param)
{
    int ReturnValue = 0;
    WCHAR FileName[MAX_FILENAME_LENGTH];
#ifdef CONFIG_APP_ARD
    UINT32 WaitEventID = 0;
#endif
    pb_multi_config_media_info(pb_multi.CurFn);

    {
        APPLIB_STILL_FILE_s StillFile = {0};
        memset(&StillFile, 0, sizeof(APPLIB_STILL_FILE_s)); // Initialize StillInfo
        memcpy(StillFile.Filename, pb_multi.CurFn, sizeof(char)*MAX_FILENAME_LENGTH); // Copy filename
        StillFile.FileSource = 0; // Decode from full image
        AppLibStillSingle_Load(&StillFile);
    }
    {
        APPLIB_STILL_SINGLE_s StillInfo = {0};
        AMP_AREA_s lcdWindow = {
            .X = 0,
            .Y = 0,
            .Width = 10000,
            .Height = 10000
        };
        AMP_AREA_s lcdPip = {
            .X = 0,
            .Y = 0,
            .Width = 0, // No PIP window
            .Height = 0 // No PIP window
        };
        AMP_AREA_s tvWindow = {
            .X = 0,
            .Y = 0,
            .Width = 10000,
            .Height = 10000
        };
        AMP_AREA_s tvPip = {
            .X = 0,
            .Y = 0,
            .Width = 0, // No PIP window
            .Height = 0 // No PIP window
        };
        memset(&StillInfo, 0, sizeof(APPLIB_STILL_SINGLE_s)); // Initialize StillInfo

        // Confugure StillInfo settings
        StillInfo.AreaDchanDisplayMain = lcdWindow;
        StillInfo.AreaDchanPIP = lcdPip;
        StillInfo.AreaFchanDisplayMain = tvWindow;
        StillInfo.AreaFchanPIP = tvPip;
        StillInfo.ImageShiftX = 0; // No shifting
        StillInfo.ImageShiftY = 0; // No shifting
        StillInfo.MagFactor = 100; // No magnification
        StillInfo.ImageRotate = (AMP_ROTATION_e)pb_multi.MediaInfo.Rotate; // No rotation and flip
        StillInfo.OutputWaitEventID = &pb_multi.StillDecWaitFlag;
#ifdef CONFIG_APP_ARD
        // Show image
        ReturnValue = AppLibStillSingle_Show(&StillInfo);
#else
        AppLibStillSingle_Show(&StillInfo);
#endif

        AmbaUtility_Ascii2Unicode(pb_multi.CurFn, FileName);
        pb_multi.Gui(GUI_FILENAME_UPDATE, (UINT32)FileName, GUI_PB_FN_STYLE_HYPHEN);
        pb_multi.Gui(GUI_FILENAME_SHOW, 0, 0);
        pb_multi.Gui(GUI_FLUSH, 0, 0);
#ifdef CONFIG_APP_ARD
        if(ReturnValue == OK){
            AppLibStillSingle_WaitShow(pb_multi.StillDecWaitFlag);
            /* In AppLibStillSingle_Load(), vout video setup will be changed by configure DSP directly.
            That means MW display module is not aware of that change.
            So, display setting should be updated after AppLibStillSingle_Load() for synchronization. */
            pb_multi.Func(PB_MULTI_CHANGE_DISPLAY, 0, 0);
        }
#endif
    }

#ifndef CONFIG_APP_ARD
    // when photo, doesn't support repeat all and play all
    if (ReturnValue == 0) {
        if((UserSetting->PlaybackPref.VideoPlayOpt == PB_OPT_VIDEO_PLAY_ALL) ||(UserSetting->PlaybackPref.VideoPlayOpt == PB_OPT_VIDEO_REPEAT_ALL)){
        UINT32 WaitEventID = 0;
        AppLibStillSingle_WaitShow(WaitEventID);
        AmbaKAL_TaskSleep(1000);
        AppLibComSvcHcmgr_SendMsg(HMSG_PLAYER_PLY_EOS, 0, 0);
    }
    }
#endif

    return ReturnValue;
}

/**
 *  @brief The flow after receiving the message EOS(End of stream)
 *
 *  The flow after receiving the message EOS(End of stream)
 *
 *  @return >=0 success, <0 failure
 */
static int pb_multi_do_play_eos(void)
{
    int ReturnValue = 0;
    char TempFn[MAX_FILENAME_LENGTH] = {0};

    AppLibComSvcTimer_Unregister(TIMER_10HZ, pb_multi_play_timer_handler);
    if (pb_multi.MediaInfo.Direction == PB_MULTI_PLAY_REV) {
        pb_multi.Gui(GUI_PLAY_TIMER_UPDATE, 0/1000, pb_multi.MediaInfo.TotalTime/1000);
    } else {
        pb_multi.Gui(GUI_PLAY_TIMER_UPDATE, pb_multi.MediaInfo.TotalTime/1000, pb_multi.MediaInfo.TotalTime/1000);
    }
    pb_multi.Gui(GUI_FLUSH, 0, 0);

    switch (UserSetting->PlaybackPref.VideoPlayOpt) {
    case PB_OPT_VIDEO_PLAY_ALL:
        AmbaPrint("[app_pb_multi] PB_OPT_VIDEO_PLAY_ALL");
        AppLibVideoDec_Stop();
        strcpy(TempFn, pb_multi.CurFn);
        if (pb_multi.MediaInfo.Direction == PB_MULTI_PLAY_REV) {
            ReturnValue = pb_multi.Func(PB_MULTI_GET_FILE, GET_PREV_FILE, APPLIB_DCF_MEDIA_VIDEO);
        } else {
            ReturnValue = pb_multi.Func(PB_MULTI_GET_FILE, GET_NEXT_FILE, APPLIB_DCF_MEDIA_VIDEO);
        }
        if (ReturnValue == 0) {
            if (strcmp(pb_multi.CurFn, pb_multi.FirstFn) == 0) {
                APP_REMOVEFLAGS(app_pb_multi.GFlags, APP_AFLAGS_BUSY);
                if (!APP_CHECKFLAGS(app_pb_multi.GFlags, APP_AFLAGS_BUSY)) {
                    /* The busy flag will be removed when the flow stop the video player. */
                    /* To excute the functions that system block them when the Busy flag is enabled. */
                    AppUtil_BusyCheck(0);
                }
                if (APP_CHECKFLAGS(app_pb_multi.GFlags, APP_AFLAGS_READY)) {
                    /* The system could switch the current app to other in the function "AppUtil_BusyCheck". */
                    pb_multi.Func(PB_MULTI_SWITCH_APP, 0, 0);
                }
            } else {
                ReturnValue = pb_multi.Func(PB_MULTI_OPEN, PB_MULTI_OPEN_VIDEO_CONT, 0);
            }
        }else {
            APP_REMOVEFLAGS(app_pb_multi.GFlags, APP_AFLAGS_BUSY);
            if (!APP_CHECKFLAGS(app_pb_multi.GFlags, APP_AFLAGS_BUSY)) {
                /* The busy flag will be removed when the flow stop the video player. */
                /* To excute the functions that system block them when the Busy flag is enabled. */
                AppUtil_BusyCheck(0);
            }
            if (APP_CHECKFLAGS(app_pb_multi.GFlags, APP_AFLAGS_READY)) {
                /* The system could switch the current app to other in the function "AppUtil_BusyCheck". */
                pb_multi.Func(PB_MULTI_SWITCH_APP, 0, 0);
            };
        }
        break;
    case PB_OPT_VIDEO_REPEAT_ONE:
        AmbaPrint("[app_pb_multi] PB_OPT_VIDEO_REPEAT_ONE");
        if (pb_multi.MediaInfo.Direction == PB_MULTI_PLAY_REV) {
            pb_multi.MediaInfo.PlayTime = pb_multi.MediaInfo.TotalTime;
            pb_multi.CurPlayTime = pb_multi.MediaInfo.TotalTime-1;
        } else {
            pb_multi.MediaInfo.PlayTime = 0;
#ifdef CONFIG_APP_ARD
            pb_multi.CurPlayTime = 0;
#else
            pb_multi.CurPlayTime = -1;
#endif
        }
#ifdef CONFIG_APP_ARD
        pb_multi_video_open_play_curr(0);
#else
        pb_multi.Func(PB_MULTI_PLAY, 0, 1);
#endif
        if (!APP_CHECKFLAGS(app_pb_multi.GFlags, APP_AFLAGS_BUSY)) {
            /* The busy flag will be removed when the flow stop the video player. */
            /* To excute the functions that system block them when the Busy flag is enabled. */
            AppUtil_BusyCheck(0);
        }
        AppLibComSvcTimer_Register(TIMER_10HZ, pb_multi_play_timer_handler);
        pb_multi.Gui(GUI_PLAY_TIMER_UPDATE, pb_multi.MediaInfo.PlayTime/1000, pb_multi.MediaInfo.TotalTime/1000);
        pb_multi.Gui(GUI_FLUSH, 0, 0);
        break;
    case PB_OPT_VIDEO_REPEAT_ALL:
        AmbaPrint("[app_pb_multi] PB_OPT_VIDEO_REPEAT_ALL");
        AppLibVideoDec_Stop();
        if (pb_multi.MediaInfo.Direction == PB_MULTI_PLAY_REV) {
            ReturnValue = pb_multi.Func(PB_MULTI_GET_FILE, GET_PREV_FILE, APPLIB_DCF_MEDIA_VIDEO);
        } else {
            ReturnValue = pb_multi.Func(PB_MULTI_GET_FILE, GET_NEXT_FILE, APPLIB_DCF_MEDIA_VIDEO);
        }
        if (ReturnValue == 0) {
            if (pb_multi.CurFileType == PB_MULTI_MEDIA_VIDEO) {
                ReturnValue = pb_multi.Func(PB_MULTI_OPEN, PB_MULTI_OPEN_VIDEO_CONT, 0);
            } else if (pb_multi.CurFileType == PB_MULTI_MEDIA_IMAGE) {
                ReturnValue = pb_multi.Func(PB_MULTI_PLAY, 0, 0);
            }
        } else {
            ReturnValue = pb_multi.Func(PB_MULTI_SWITCH_APP, 0, 0);
        }
        break;
    case PB_OPT_VIDEO_PLAY_ONE:
    default:
        AmbaPrint("[app_pb_multi] PB_OPT_VIDEO_PLAY_ONE");
        AppLibVideoDec_Stop();
        APP_REMOVEFLAGS(app_pb_multi.GFlags, APP_AFLAGS_BUSY);
        if (!APP_CHECKFLAGS(app_pb_multi.GFlags, APP_AFLAGS_BUSY)) {
            /* The busy flag will be removed when the flow stop the video player. */
            /* To excute the functions that system block them when the Busy flag is enabled. */
            AppUtil_BusyCheck(0);
        }
        if (APP_CHECKFLAGS(app_pb_multi.GFlags, APP_AFLAGS_READY)) {
#ifdef CONFIG_APP_ARD
    AppLibVideoDec_Stop();
#endif
            /* The system could switch the current app to other in the function "AppUtil_BusyCheck". */
            pb_multi.Func(PB_MULTI_SWITCH_APP, 0, 0);
        }
        break;
    }

    return ReturnValue;
}

static int pb_multi_video_play_stop(void)
{
    int ReturnValue = 0;
    if (pb_multi.CurFileType == PB_MULTI_MEDIA_VIDEO) {
        AppLibComSvcTimer_Unregister(TIMER_10HZ, pb_multi_play_timer_handler);
        pb_multi.Gui(GUI_PLAY_TIMER_HIDE, 0, 0);
        pb_multi.Gui(GUI_PLAY_STATE_HIDE, 0, 0);
        pb_multi.Gui(GUI_PLAY_SPEED_HIDE, 0, 0);
        AppLibVideoDec_PlayEOS();
        AppLibVideoDec_Stop();
    } else {
        //AppLibStillSingle_Stop();
    }

    return ReturnValue;
}



/**
 * @brief The applications switching function.
 *
 * @return >=0 success
 *         <0 failure
 */
static int pb_multi_switch_app(int mode)
{
    int ReturnValue = 0;

    if ( app_status.LockDecMode && app_status.FchanDecModeOnly) {
        ReturnValue = AppUtil_SwitchApp(APP_THUMB_MOTION);
    } else if (mode == 0){
        ReturnValue = AppUtil_SwitchApp(app_pb_multi.Previous);
    } else {
        ReturnValue = AppUtil_SwitchApp(APP_REC_CAM); /* CONFIG_APP_ARD */
    }

    return ReturnValue;
}

int pb_multi_card_removed(void)
{
    int ReturnValue = 0;

#ifdef CONFIG_APP_ARD
    // A running pipe cannot be removed! Must call AmpDemuxer_Stop() before AmpDemuxer_Remove().
    pb_multi.Func(PB_MULTI_STOP_PLAYING, 0, 0);
#endif

    if (APP_CHECKFLAGS(app_pb_multi.GFlags, APP_AFLAGS_POPUP)) {
        APP_REMOVEFLAGS(app_pb_multi.Flags, PB_MULTI_DELETE_FILE_RUN);
        AppWidget_Off(WIDGET_ALL, 0);
    }
    pb_multi.Func(PB_MULTI_START_DISP_PAGE, 0, 0);
    pb_multi.Func(PB_MULTI_WARNING_MSG_SHOW_STOP, 0, 0);
    pb_multi.Gui(GUI_CARD_UPDATE, GUI_NO_CARD, 0);
    pb_multi.Gui(GUI_MEDIA_INFO_HIDE, 0, 0);
    pb_multi.Gui(GUI_FILENAME_HIDE, 0, 0);
    pb_multi.Gui(GUI_FLUSH, 0, 0);

    return ReturnValue;
}

int pb_multi_card_error_removed(void)
{
    int ReturnValue = 0;

    APP_REMOVEFLAGS(app_pb_multi.GFlags, APP_AFLAGS_BUSY);
    pb_multi.Func(PB_MULTI_CARD_REMOVED, 0, 0);

    return ReturnValue;
}

int pb_multi_card_new_insert(int param1)
{
    int ReturnValue = 0;

    /* Remove old card.*/
    pb_multi.Func(PB_MULTI_CARD_REMOVED, 0, 0);
    AppLibCard_StatusSetBlock(param1, 0);
    AppLibComSvcAsyncOp_CardInsert(AppLibCard_GetSlot(param1));

    return ReturnValue;
}

int pb_multi_card_storage_idle(void)
{
    int ReturnValue = 0;

    AppUtil_CheckCardParam(0);
    if (!APP_CHECKFLAGS(app_pb_photo.GFlags, APP_AFLAGS_READY)) {
        return ReturnValue;/**<  App switched out*/
    }

    pb_multi.Gui(GUI_CARD_UPDATE, GUI_CARD_READY, 0);
    pb_multi.Gui(GUI_FLUSH, 0, 0);
    pb_multi.Func(PB_MULTI_START_DISP_PAGE, 0, 0);

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

static int pb_multi_set_delete_file_mode(int param)
{
    int ReturnValue = 0;

    APP_ADDFLAGS(app_pb_multi.Flags, PB_MULTI_DELETE_FILE_RUN);
    pb_multi.DeleteFileMode = param;

    return ReturnValue;
}

static int pb_multi_select_delete_file_mode(void)
{
    int ReturnValue = 0;

    ReturnValue = AppLibCard_CheckStatus(CARD_CHECK_DELETE);
    if (ReturnValue == 0) {
        AppMenuQuick_SetItem(MENU_PBACK, MENU_PBACK_DELETE_FILE);
        ReturnValue = AppWidget_On(WIDGET_MENU_QUICK, 0);
        APP_ADDFLAGS(app_pb_multi.GFlags, APP_AFLAGS_POPUP);
    } else if (ReturnValue == CARD_STATUS_WP_CARD) {
        pb_multi.Func(PB_MULTI_WARNING_MSG_SHOW_START, GUI_WARNING_CARD_PROTECTED, 0);
        AmbaPrintColor(RED,"[app_pb_multi] WARNING_CARD_PROTECTED");
    } else {
        AmbaPrintColor(RED,"[app_pb_multi] WARNING_CARD_ERROR ReturnValue = %d", ReturnValue);
    }

    return ReturnValue;
}

static int pb_multi_dialog_del_handler(UINT32 sel, UINT32 param1, UINT32 param2)
{
    int ReturnValue = 0;

    switch (sel) {
    case DIALOG_SEL_YES:
#ifdef CONFIG_APP_ARD
        pb_multi.Func(PB_MULTI_DELETE_FILE, CUR_OBJ(pb_multi.CurFileObjID), 0);
#else
        pb_multi.Func(PB_MULTI_DELETE_FILE, CUR_OBJ(pb_multi.CurFileObjID), CUR_OBJ_IDX(pb_multi.CurFileObjID));
#endif
        break;
    case DIALOG_SEL_NO:
    default:
        break;
    }

    return ReturnValue;
}

#ifdef CONFIG_APP_ARD
static int pb_multi_dialog_del_all_handler(UINT32 sel, UINT32 param1, UINT32 param2)
{
    int ReturnValue = 0;

    switch (sel) {
    case DIALOG_SEL_YES:
        pb_multi.Func(PB_MULTI_DELETE_FILE, app_status.PlaybackType, 1);
        break;
    case DIALOG_SEL_NO:
    default:
        break;
    }

    return ReturnValue;
}
#endif


static int pb_multi_delete_file_dialog_show(void)
{
    int ReturnValue = 0;
#ifdef CONFIG_APP_ARD
    if (pb_multi.DeleteFileMode) {
        ReturnValue = AppDialog_SetDialog(DIALOG_TYPE_Y_N, DIALOG_SUB_DEL_ALL, pb_multi_dialog_del_all_handler);
    } else {
        ReturnValue = AppDialog_SetDialog(DIALOG_TYPE_Y_N, DIALOG_SUB_DEL, pb_multi_dialog_del_handler);
    }
    ReturnValue = AppWidget_On(WIDGET_DIALOG, 0);
    APP_ADDFLAGS(app_pb_multi.GFlags, APP_AFLAGS_POPUP);
#else
    ReturnValue = AppLibCard_CheckStatus(CARD_CHECK_DELETE);
    if (ReturnValue == 0) {
        AppDialog_SetDialog(DIALOG_TYPE_Y_N, DIALOG_SUB_DEL, pb_multi_dialog_del_handler);
        AppWidget_On(WIDGET_DIALOG, 0);
        APP_ADDFLAGS(app_pb_multi.GFlags, APP_AFLAGS_POPUP);
    } else if (ReturnValue == CARD_STATUS_WP_CARD) {
        pb_multi.Func(PB_MULTI_WARNING_MSG_SHOW_START, GUI_WARNING_CARD_PROTECTED, 0);
        AmbaPrintColor(RED,"[app_pb_multi] WARNING_CARD_PROTECTED");
    } else {
        AmbaPrintColor(RED,"[app_pb_multi] WARNING_CARD_Error rval = %d", ReturnValue);
    }
#endif
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

#ifdef CONFIG_APP_ARD
static int pb_multi_delete_file(UINT32 FileObjID,UINT32 param)
#else
static int pb_multi_delete_file(UINT32 FileObjID)
#endif
{
    int ReturnValue = 0;
#ifdef CONFIG_APP_ARD
    DBGMSG("[app_pb_multi] pb_multi_delete_file, FileObjID = 0x%08X, param = %d", FileObjID, param);
    pb_multi.Func(PB_MULTI_STOP_PLAYING, 0, 0);

    if (param) {
        AppLibComSvcAsyncOp_DmfFastFdelAll(pb_multi.FileInfo.MediaRoot, 0);
    } else {
        AppLibComSvcAsyncOp_DmfFdel(pb_multi.FileInfo.MediaRoot, FileObjID, app_status.PlaybackType);
    }

    APP_ADDFLAGS(app_pb_multi.GFlags, APP_AFLAGS_BUSY);

    pb_multi.Func(PB_MULTI_WARNING_MSG_SHOW_START, GUI_WARNING_PROCESSING, 1);
    pb_multi.Func(PB_MULTI_SWITCH_APP, 0, 0);
#else
    if (pb_multi.CurFileType == PB_MULTI_MEDIA_VIDEO) {
        /* Stop player. */
        AppLibVideoDec_Stop();
    }

    AppLibComSvcAsyncOp_DmfFdel(pb_multi.FileInfo.MediaRoot, FileObjID, app_status.PlaybackType);

    APP_ADDFLAGS(app_pb_multi.Flags, PB_MULTI_DELETE_FILE_RUN);
    APP_ADDFLAGS(app_pb_multi.GFlags, APP_AFLAGS_BUSY);
    pb_multi.Func(PB_MULTI_WARNING_MSG_SHOW_START, GUI_WARNING_PROCESSING, 1);
#endif
    return ReturnValue;
}

static int pb_multi_delete_file_complete(int param1, int param2)
{
    int ReturnValue = 0;
#ifdef CONFIG_APP_ARD
    APP_REMOVEFLAGS(app_pb_multi.Flags, PB_MULTI_DELETE_FILE_RUN);
    APP_REMOVEFLAGS(app_pb_multi.GFlags, APP_AFLAGS_BUSY);
    pb_multi.Gui(GUI_WARNING_HIDE, 0, 0);
    pb_multi.Gui(GUI_FLUSH, 0, 0);
    if (((int)param1) < 0) {
        DBGMSG("[app_pb_multi] Received AMSG_MGR_MSG_OP_FAILED");
        AmbaPrintColor(RED,"[app_pb_multi] Delete files failed: %d", param2);
    } else {
        DBGMSG("[app_pb_multi] Received AMSG_MGR_MSG_OP_DONE");
        /** page update */
        // param2 == 1, just force the system into thumbnail view, instead of playback state.
        pb_multi.Func(PB_MULTI_START_DISP_PAGE, 0, 1);
    }

#else
    if (APP_CHECKFLAGS(app_pb_multi.Flags, PB_MULTI_DELETE_FILE_RUN)) {
        APP_REMOVEFLAGS(app_pb_multi.Flags, PB_MULTI_DELETE_FILE_RUN);
        pb_multi.Gui(GUI_WARNING_HIDE, 0, 0);
        pb_multi.Gui(GUI_FLUSH, 0, 0);
        APP_REMOVEFLAGS(app_pb_multi.GFlags, APP_AFLAGS_BUSY);
        ReturnValue = (int)param1;
        if (ReturnValue < 0) {
            AmbaPrintColor(RED,"[app_pb_multi] Delete files failed: %d", param2);
        } else {
            //pb_multi.Gui(GUI_ZOOM_RATIO_HIDE, 0, 0);
            //pb_multi.Gui(GUI_FLUSH, 0, 0);
            /** page update */
            app_status.ThumbnailModeConti = 0;
            pb_multi.Func(PB_MULTI_START_DISP_PAGE, 0, 0);
        }
    }

#endif
    return ReturnValue;
}

static int pb_multi_widget_closed(int param1, int param2)
{
    int ReturnValue = 0;

    if (APP_CHECKFLAGS(app_pb_multi.GFlags, APP_AFLAGS_POPUP)) {
        APP_REMOVEFLAGS(app_pb_multi.GFlags, APP_AFLAGS_POPUP);
#ifdef CONFIG_APP_ARD
        if (APP_CHECKFLAGS(app_pb_multi.Flags, PB_MULTI_DELETE_FILE_RUN)) {
            APP_REMOVEFLAGS(app_pb_multi.Flags, PB_MULTI_DELETE_FILE_RUN);
            pb_multi.Func(PB_MULTI_DELETE_FILE_DIALOG_SHOW, param1, param2);
        }
#endif
    }

#ifdef CONFIG_APP_ARD
    {
        WCHAR FileName[MAX_FILENAME_LENGTH];
        AmbaUtility_Ascii2Unicode(pb_multi.CurFn, FileName);
        pb_multi.Gui(GUI_FILENAME_UPDATE, (UINT32)FileName, GUI_PB_FN_STYLE_HYPHEN);
        pb_multi.Gui(GUI_FILENAME_SHOW, 0, 0);
        pb_multi.Gui(GUI_FLUSH, 0, 0);
    }
#endif

    return ReturnValue;
}

static int pb_multi_set_system_type(void)
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
static int pb_multi_update_fchan_vout(UINT32 msg)
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
        AmbaPrint("[app_pb_multi] Vout no changed");
        return 0;
        break;
    }
    ReturnValue = AppLibDisp_SelectDevice(DISP_CH_FCHAN, DISP_ANY_DEV);
    if (APP_CHECKFLAGS(ReturnValue, DISP_FCHAN_NO_CHANGE)) {
        AmbaPrint("[app_pb_multi] Display FCHAN has no changed");
    } else {
        AppLibVideoDec_Stop();
        AppLibVideoDec_Exit();
        AppLibStillSingle_Deinit();
        if (APP_CHECKFLAGS(ReturnValue, DISP_FCHAN_NO_DEVICE)) {
            AppLibDisp_ChanStop(DISP_CH_FCHAN);
            AppLibDisp_DeactivateWindow(DISP_CH_FCHAN, AppLibDisp_GetWindowId(DISP_CH_FCHAN, 0));
            AppLibDisp_FlushWindow(DISP_CH_FCHAN);
            app_status.LockDecMode = 0;
        } else {
            AppLibDisp_ConfigMode(DISP_CH_FCHAN, AppLibSysVout_GetVoutMode(VOUT_DISP_MODE_1080P));
            AppLibDisp_SetupChan(DISP_CH_FCHAN);
            AppLibDisp_ChanStart(DISP_CH_FCHAN);
            {
                AMP_DISP_WINDOW_CFG_s Window;
                AMP_DISP_INFO_s DispDev = {0};

                memset(&Window, 0, sizeof(AMP_DISP_WINDOW_CFG_s));

                ReturnValue = AppLibDisp_GetDeviceInfo(DISP_CH_FCHAN, &DispDev);
                if ((ReturnValue < 0) || (DispDev.DeviceInfo.Enable == 0)) {
                    DBGMSG("[app_pb_multi] FChan Disable. Disable the fchan window");
                    AppLibDisp_DeactivateWindow(DISP_CH_FCHAN, AppLibDisp_GetWindowId(DISP_CH_FCHAN, 0));
                    AppLibDisp_FlushWindow(DISP_CH_FCHAN);
                    app_status.LockDecMode = 0;
                } else {
                    /** FCHAN window*/
#ifdef CONFIG_APP_ARD
                    AppLibGraph_EnableDraw(DISP_CH_FCHAN);
#endif
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
            pb_multi.Gui(GUI_SET_LAYOUT, 0, 0);
            pb_multi.Gui(GUI_FLUSH, 0, 0);
        }
        pb_multi.CurPlayTime = 0;
        AppLibVideoDec_Init();
        AppLibStillSingle_Init();
        pb_multi.Func(PB_MULTI_START_DISP_PAGE, 1, 0);
    }

    return ReturnValue;
}

int pb_multi_change_display(void)
{
    int ReturnValue = 0;

#ifdef CONFIG_APP_ARD
    AppLibSysLcd_SetBacklight_Directly(0, 0);
    cardv_app_screen_lock();
#endif

    AppLibDisp_SelectDevice(DISP_CH_FCHAN | DISP_CH_DCHAN, DISP_ANY_DEV);
#ifdef CONFIG_APP_ARD
    AppLibDisp_DeactivateWindow(DISP_CH_DCHAN, AppLibDisp_GetWindowId(DISP_CH_DCHAN, 0));
    AppLibDisp_ChanStop(DISP_CH_DCHAN);
    AppLibDisp_ConfigMode(DISP_CH_FCHAN , AppLibSysVout_GetVoutMode(VOUT_DISP_MODE_1080P));
    if(UserSetting->SetupPref.VoutSystem == 0 ) {
        AppLibDisp_ConfigMode( DISP_CH_DCHAN, 0 );
    } else {
        AppLibDisp_ConfigMode( DISP_CH_DCHAN, 1);
    }
#else
    AppLibDisp_ConfigMode(DISP_CH_FCHAN | DISP_CH_DCHAN, AppLibSysVout_GetVoutMode(VOUT_DISP_MODE_1080P));
#endif
    AppLibDisp_SetupChan(DISP_CH_FCHAN | DISP_CH_DCHAN);
    AppLibDisp_ChanStart(DISP_CH_FCHAN | DISP_CH_DCHAN);
    {
        AMP_DISP_WINDOW_CFG_s Window;
        AMP_DISP_INFO_s DispDev = {0};

        memset(&Window, 0, sizeof(AMP_DISP_WINDOW_CFG_s));

        ReturnValue = AppLibDisp_GetDeviceInfo(DISP_CH_FCHAN, &DispDev);
        if ((ReturnValue < 0) || (DispDev.DeviceInfo.Enable == 0)) {
            DBGMSG("[app_pb_multi] FChan Disable. Disable the fchan Window");
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
            DBGMSG("[app_pb_multi] DChan Disable. Disable the Dchan Window");
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
#ifdef CONFIG_APP_ARD
            if(AppLibDisp_GetRotate(DISP_CH_DCHAN) == AMP_ROTATE_90){
                Window.SourceDesc.Dec.Rotate= AMP_ROTATE_90;
            }
#endif
            AppLibDisp_SetWindowConfig(DISP_CH_DCHAN, AppLibDisp_GetWindowId(DISP_CH_DCHAN, 0), &Window);
            AppLibDisp_ActivateWindow(DISP_CH_DCHAN, AppLibDisp_GetWindowId(DISP_CH_DCHAN, 0));
        }
        AppLibDisp_FlushWindow(DISP_CH_FCHAN | DISP_CH_DCHAN);
    }
    return ReturnValue;
}

int pb_multi_change_osd(void)
{
    int ReturnValue = 0;
    /* Update graphic window*/
    AppLibGraph_SetWindowConfig(GRAPH_CH_FCHAN | GRAPH_CH_DCHAN);
    AppLibGraph_ActivateWindow(GRAPH_CH_FCHAN | GRAPH_CH_DCHAN);
    AppLibGraph_FlushWindow(GRAPH_CH_FCHAN | GRAPH_CH_DCHAN);
    pb_multi.Gui(GUI_SET_LAYOUT, 0, 0);
    pb_multi.Gui(GUI_FLUSH, 0, 0);

    return ReturnValue;
}

static int pb_multi_usb_connect(void)
{
    int ReturnValue = 0;

    switch (UserSetting->SetupPref.USBMode) {
    default:
        AppUtil_SwitchApp(APP_USB_MSC);
        break;
    }

    return ReturnValue;
}

static int pb_multi_start_show_gui(void)
{
    int ReturnValue = 0;
    int GuiParam = 0;

    pb_multi.Gui(GUI_APP_ICON_SHOW, 0, 0);
    pb_multi.Gui(GUI_POWER_STATE_UPDATE, app_status.PowerType, app_status.BatteryState);
    pb_multi.Gui(GUI_POWER_STATE_SHOW, app_status.PowerType, app_status.BatteryState);
    ReturnValue = AppLibCard_CheckStatus(0);
    if (ReturnValue == CARD_STATUS_NO_CARD) {
        GuiParam = GUI_NO_CARD;
    } else {
        GuiParam = GUI_CARD_READY;
    }
    pb_multi.Gui(GUI_CARD_UPDATE, GuiParam, 0);
    pb_multi.Gui(GUI_CARD_SHOW, 0, 0);
    // *video playback need to display timeline gui
    if (pb_multi.CurFileType== PB_MULTI_MEDIA_VIDEO) {
        pb_multi.Gui(GUI_PLAY_STATE_UPDATE, GUI_PAUSE, 0);
        pb_multi.Gui(GUI_PLAY_TIMER_UPDATE, 0, pb_multi.MediaInfo.TotalTime/1000);
        pb_multi.Gui(GUI_PLAY_STATE_SHOW, 0, 0);
        pb_multi.Gui(GUI_PLAY_SPEED_SHOW, 0, 0);
    }
    pb_multi.Gui(GUI_MEDIA_INFO_HIDE, 0, 0);
    pb_multi.Gui(GUI_FILENAME_HIDE, 0, 0);
    pb_multi.Gui(GUI_FLUSH, 0, 0);

    return ReturnValue;
}


static int pb_multi_update_bat_power_status(int param1)
{
    int ReturnValue = 0;

    /* Update the gui of power's status. */
    if (param1 == 0) {
        /*Hide the battery gui.*/
        pb_multi.Gui(GUI_POWER_STATE_HIDE, GUI_HIDE_POWER_EXCEPT_DC, 0);
    } else if (param1 == 1) {
        /*Update the battery gui.*/
        pb_multi.Gui(GUI_POWER_STATE_UPDATE, app_status.PowerType, app_status.BatteryState);
        pb_multi.Gui(GUI_POWER_STATE_SHOW, app_status.PowerType, app_status.BatteryState);
    } else if (param1 == 2) {
        /*Reset the battery and power gui.*/
        pb_multi.Gui(GUI_POWER_STATE_HIDE, 0, 0);
        pb_multi.Gui(GUI_POWER_STATE_UPDATE, app_status.PowerType, app_status.BatteryState);
        pb_multi.Gui(GUI_POWER_STATE_SHOW, app_status.PowerType, app_status.BatteryState);
    }
    pb_multi.Gui(GUI_FLUSH, 0, 0);

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
static void pb_multi_warning_timer_handler(int eid)
{
    static int BlinkCount;

    if (eid == TIMER_UNREGISTER) {
        BlinkCount = 0;
        return;
    }

    BlinkCount++;

    if (BlinkCount & 0x01) {
        pb_multi.Gui(GUI_WARNING_HIDE, 0, 0);
    } else {
        pb_multi.Gui(GUI_WARNING_SHOW, 0, 0);
    }

    if (BlinkCount >= 5) {
        APP_REMOVEFLAGS(app_pb_multi.Flags, PB_MULTI_WARNING_MSG_RUN);
        AppLibComSvcTimer_Unregister(TIMER_2HZ, pb_multi_warning_timer_handler);
        pb_multi.Gui(GUI_WARNING_HIDE, 0, 0);
    }
    pb_multi.Gui(GUI_FLUSH, 0, 0);

}

static int pb_multi_warning_msg_show(int enable, int param1, int param2)
{
    int ReturnValue = 0;

    if (enable) {
        if (param2) {
            pb_multi.Gui(GUI_WARNING_UPDATE, param1, 0);
            pb_multi.Gui(GUI_WARNING_SHOW, 0, 0);
        } else {
            if (!APP_CHECKFLAGS(app_pb_multi.Flags, PB_MULTI_WARNING_MSG_RUN)) {
                APP_ADDFLAGS(app_pb_multi.Flags, PB_MULTI_WARNING_MSG_RUN);
                pb_multi.Gui(GUI_WARNING_UPDATE, param1, 0);
                pb_multi.Gui(GUI_WARNING_SHOW, 0, 0);
                AppLibComSvcTimer_Register(TIMER_2HZ, pb_multi_warning_timer_handler);
            }
        }
    } else {
        if (APP_CHECKFLAGS(app_pb_multi.Flags, PB_MULTI_WARNING_MSG_RUN)) {
            APP_REMOVEFLAGS(app_pb_multi.Flags, PB_MULTI_WARNING_MSG_RUN);
            AppLibComSvcTimer_Unregister(TIMER_2HZ, pb_multi_warning_timer_handler);
        }
        pb_multi.Gui(GUI_WARNING_HIDE, 0, 0);
    }
    pb_multi.Gui(GUI_FLUSH, 0, 0);

    return ReturnValue;
}

static int pb_multi_decode_error(void)
{
    int ReturnValue = 0;

    if (pb_multi.CurFileType == PB_MULTI_MEDIA_VIDEO) {
        AppLibVideoDec_DecodeErrorHandling();
        AppLibComSvcTimer_Unregister(TIMER_10HZ, pb_multi_play_timer_handler);
        AmbaPrintColor(RED,"\r\n#####\r\nError: File %s is corrupted!\r\n#####", pb_multi.CurFn);

        APP_REMOVEFLAGS(app_pb_multi.GFlags, APP_AFLAGS_BUSY);
        ReturnValue = pb_multi.Func(PB_MULTI_SWITCH_APP, 0, 0);
    }


    return ReturnValue;
}

/**
 *  @brief The functions of video playback application
 *
 *  The functions of video playback application
 *
 *  @param[in] funcId Function id
 *  @param[in] param1 first parameter
 *  @param[in] param2 second parameter
 *
 *  @return >=0 success, <0 failure
 */
int pb_multi_func(UINT32 funcId, UINT32 param1, UINT32 param2)
{
    int ReturnValue = 0;

    switch (funcId) {
    case PB_MULTI_INIT:
        ReturnValue = pb_multi_init();
        break;
    case PB_MULTI_START:
        ReturnValue = pb_multi_start();
        break;
    case PB_MULTI_STOP:
        ReturnValue = pb_multi_stop();
        break;
    case PB_MULTI_APP_READY:
        ReturnValue = pb_multi_app_ready();
        break;
    case PB_MULTI_START_DISP_PAGE:
#ifdef CONFIG_APP_ARD
        ReturnValue = pb_multi_start_disp_page(param1, param2);
#else
        ReturnValue = pb_multi_start_disp_page(param1);
#endif
        break;
    case PB_MULTI_OPEN_NEXT_OR_PREVIOUS:
        ReturnValue = pb_multi_open_next_previous_file(param1, param2);

    case PB_MULTI_OPEN:
        ReturnValue = pb_multi_open(param1);
        break;
    case PB_MULTI_PLAY:
        if (param2) {
            
#ifdef CONFIG_APP_ARD
            ReturnValue = pb_multi_video_open_play_curr(param1);
#endif
        } else {
            ReturnValue = pb_multi_photo_open_play_curr(param1);
        }
        break;
    case PB_MULTI_EOS:
        ReturnValue = pb_multi_do_play_eos();
        break;
    case PB_MULTI_STOP_PLAYING:
        ReturnValue = pb_multi_video_play_stop();
        break;
    case PB_MULTI_SWITCH_APP:
        ReturnValue = pb_multi_switch_app(param1);
        break;
    case PB_MULTI_GET_FILE:
        ReturnValue = pb_multi_get_file(param1, param2);
        break;
    case PB_MULTI_CARD_ERROR_REMOVED:
        ReturnValue = pb_multi_card_error_removed();
        break;
    case PB_MULTI_CARD_REMOVED:
        ReturnValue = pb_multi_card_removed();
        break;
    case PB_MULTI_CARD_NEW_INSERT:
        ReturnValue = pb_multi_card_new_insert(param1);
        break;
    case PB_MULTI_CARD_STORAGE_IDLE:
        ReturnValue = pb_multi_card_storage_idle();
        break;
#ifdef CONFIG_APP_ARD
    case PB_MULTI_SET_DELETE_FILE_MODE:
        ReturnValue = pb_multi_set_delete_file_mode(param2);
        break;
    case PB_MULTI_SELECT_DELETE_FILE_MODE:
        ReturnValue = pb_multi_select_delete_file_mode();
        break;
#endif
    case PB_MULTI_DELETE_FILE_DIALOG_SHOW:
        ReturnValue = pb_multi_delete_file_dialog_show();
        break;
    case PB_MULTI_DELETE_FILE:
#ifdef CONFIG_APP_ARD
        ReturnValue = pb_multi_delete_file(param1,param2);
#else
        ReturnValue = pb_multi_delete_file(param1);
#endif
        break;
    case PB_MULTI_DELETE_FILE_COMPLETE:
        ReturnValue = pb_multi_delete_file_complete(param1, param2);
        break;
    case PB_MULTI_STATE_WIDGET_CLOSED:
        ReturnValue = pb_multi_widget_closed(param1, param2);
        break;
    case PB_MULTI_SET_SYSTEM_TYPE:
        ReturnValue = pb_multi_set_system_type();
        break;
    case PB_MULTI_UPDATE_FCHAN_VOUT:
        ReturnValue = pb_multi_update_fchan_vout(param1);
        break;
    case PB_MULTI_UPDATE_DCHAN_VOUT:
        break;
    case PB_MULTI_CHANGE_DISPLAY:
        ReturnValue = pb_multi_change_display();
        break;
    case PB_MULTI_CHANGE_OSD:
        ReturnValue = pb_multi_change_osd();
        break;
    case PB_MULTI_USB_CONNECT:
        ReturnValue = pb_multi_usb_connect();
        break;
    case PB_MULTI_GUI_INIT_SHOW:
        ReturnValue = pb_multi_start_show_gui();
        break;
    case PB_MULTI_UPDATE_BAT_POWER_STATUS:
        ReturnValue = pb_multi_update_bat_power_status(param1);
        break;
    case PB_MULTI_WARNING_MSG_SHOW_START:
        ReturnValue = pb_multi_warning_msg_show(1, param1, param2);
        break;
    case PB_MULTI_WARNING_MSG_SHOW_STOP:
        ReturnValue = pb_multi_warning_msg_show(0, param1, param2);
        break;
    case PB_MULTI_DECODE_ERROR:
        ReturnValue = pb_multi_decode_error();
        break;
    default:
        break;
    }

    return ReturnValue;
}
