/**
  * @file src/app/apps/flow/pb/connectedcam/pb_video_func.c
  *
  *  Functions of video playback application
  *
  * History:
  *    2013/08/09 - [Martin Lai] created file
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

#include <apps/flow/pb/pb_video.h>
#include <system/status.h>
#include <system/app_pref.h>
#include <system/app_util.h>
#include <apps/gui/resource/gui_settle.h>
#include <apps/flow/widget/menu/widget.h>
#include <AmbaUtility.h>

static void pb_video_play_timer_handler(int eid)
{
    if (eid == TIMER_UNREGISTER) {
        return;
    }

    AppLibVideoDec_GetTime(&pb_video.MediaInfo.PlayTime);
    pb_video.Gui(GUI_PLAY_TIMER_UPDATE, (UINT32)pb_video.MediaInfo.PlayTime/1000, (UINT32)pb_video.MediaInfo.TotalTime/1000);
    pb_video.Gui(GUI_FLUSH, 0, 0);
}

static int pb_video_init(void)
{
    int ReturnValue = 0;

    pb_video.FileInfo.MediaRoot = APPLIB_DCF_MEDIA_VIDEO;

    return ReturnValue;
}

static int pb_video_start(void)
{
    int ReturnValue = 0;
    UserSetting->SystemPref.SystemMode = APP_MODE_DEC;
    memset(&pb_video.MediaInfo, 0, sizeof(PB_VIDEO_MEDIA_INFO_s));

    /** Set menus */
    AppMenu_Reset();
    #ifndef CONFIG_ECL_GUI
    AppMenu_RegisterTab(MENU_SETUP);
    #endif
    AppMenu_RegisterTab(MENU_PBACK);

    /** Initialize the demuxer. */
    AppLibFormat_DemuxerInit();

    /** Initialize the video decoder. */
    AppLibVideoDec_Init();

    return ReturnValue;
}

static int pb_video_stop(void)
{
    int ReturnValue = 0;

    /* Stop the play timer. */
    AppLibComSvcTimer_Unregister(TIMER_1HZ, pb_video_play_timer_handler);
    pb_video.Gui(GUI_PLAY_TIMER_HIDE, 0, 0);

    /* Close the video player. */
    //pLibVideoDec_Exit(); mark wait for applib func fix

    /* Close the menu or dialog. */
    AppWidget_Off(WIDGET_ALL, WIDGET_HIDE_SILENT);
    APP_REMOVEFLAGS(app_pb_video.GFlags, APP_AFLAGS_POPUP);

    /* Disable the vout. */
    AppLibDisp_DeactivateWindow(DISP_CH_FCHAN, AppLibDisp_GetWindowId(DISP_CH_FCHAN, 0));
    AppLibDisp_DeactivateWindow(DISP_CH_DCHAN, AppLibDisp_GetWindowId(DISP_CH_DCHAN, 0));
    AppLibDisp_ChanStop(DISP_CH_FCHAN);

    /* Hide GUI */
    pb_video.Gui(GUI_HIDE_ALL, 0, 0);
    pb_video.Gui(GUI_FLUSH, 0, 0);

    return ReturnValue;
}

static int pb_video_app_ready(void)
{
    int ReturnValue = 0;

    if (!APP_CHECKFLAGS(app_pb_video.GFlags, APP_AFLAGS_READY)) {
        APP_ADDFLAGS(app_pb_video.GFlags, APP_AFLAGS_READY);

        // ToDo: need to remove to handler when iav completes the dsp cmd queue mechanism
        AppLibGraph_Init();
        pb_video.Func(PB_VIDEO_CHANGE_OSD, 0, 0);

        AppUtil_ReadyCheck(0);
        if (!APP_CHECKFLAGS(app_pb_video.GFlags, APP_AFLAGS_READY)) {
            /* The system could switch the current app to other in the function "AppUtil_ReadyCheck". */
            return ReturnValue;
        }
    }

    if (APP_CHECKFLAGS(app_pb_video.GFlags, APP_AFLAGS_READY)) {
        pb_video.Func(PB_VIDEO_START_DISP_PAGE, 0, 0);
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
static int pb_video_init_file_info(void)
{
    UINT64 ReturnValue = 0;

    ReturnValue = AppLibCard_CheckStatus(0);

    if (ReturnValue == 0) {
        pb_video.FileInfo.TotalFileNum = AppLibStorageDmf_GetFileAmount(pb_video.FileInfo.MediaRoot, DCIM_HDLR);
    } else {
        pb_video.FileInfo.TotalFileNum = 0;
    }
    AmbaPrintColor(GREEN, "[app_pb_video] pb_video.FileInfo.TotalFileNum: %d", pb_video.FileInfo.TotalFileNum);
    if (pb_video.FileInfo.TotalFileNum > 0) {
        int i = 0;
        UINT64 CurrFilePos = AppLibStorageDmf_GetCurrFilePos(pb_video.FileInfo.MediaRoot, DCIM_HDLR);
        if (CurrFilePos == 0) {
            CurrFilePos = AppLibStorageDmf_GetLastFilePos(pb_video.FileInfo.MediaRoot, DCIM_HDLR);
        }
        AppLibStorageDmf_GetFileName(pb_video.FileInfo.MediaRoot, ".MP4", APPLIB_DCF_EXT_OBJECT_SPLIT_FILE, DCIM_HDLR, (CUR_OBJ_IDX(CurrFilePos) + 1), CUR_OBJ(CurrFilePos), pb_video.CurFn);
        pb_video.CurFileObjID = CurrFilePos;
        ReturnValue = AppLibStorageDmf_GetLastFilePos(pb_video.FileInfo.MediaRoot, DCIM_HDLR);
        for (i = (pb_video.FileInfo.TotalFileNum - 1) ; i >= 0; i--) {
            if (ReturnValue == CurrFilePos) {
                pb_video.FileInfo.FileCur = i;
                break;
            } else {
                ReturnValue = AppLibStorageDmf_GetPrevFilePos(pb_video.FileInfo.MediaRoot, DCIM_HDLR);
            }
        }
        AmbaPrintColor(GREEN, "[app_pb_video] pb_video.FileInfo.FileCur: %d, CurrFilePos =0x%llx", pb_video.FileInfo.FileCur,CurrFilePos);
    } else {
        pb_video.FileInfo.TotalFileNum = 0;
        pb_video.FileInfo.FileCur = 0;
        pb_video.CurFileObjID = 0;
        memset(pb_video.CurFn, 0, MAX_FILENAME_LENGTH*sizeof(char));
    }

    return CUR_OBJ(ReturnValue);
}

static int pb_video_start_disp_page(void)
{
    int ReturnValue = 0;

    ReturnValue = pb_video_init_file_info();
    if (pb_video.FileInfo.TotalFileNum > 0) {
        pb_video.Func(PB_VIDEO_GUI_INIT_SHOW, 0, 0);

        pb_video.Func(PB_VIDEO_GET_FILE, GET_CURR_FILE, 0);
        pb_video.Func(PB_VIDEO_OPEN, PB_VIDEO_OPEN_RESET, 0);

        /* Play the video clip. */
        pb_video.MediaInfo.State = PB_VIDEO_PLAY_PLAY;
        pb_video.Func(PB_VIDEO_PLAY, 0, 0);
        pb_video.Gui(GUI_PLAY_STATE_UPDATE, GUI_FWD_NORMAL, 0);
        pb_video.Gui(GUI_FLUSH, 0, 0);

    } else {
        pb_video.Func(PB_VIDEO_SWITCH_APP, 0, 0);
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
static int pb_video_get_file(UINT32 param)
{
    int ReturnValue = 0;
    int i = 0;
    char TempFn[MAX_FILENAME_LENGTH] = {0};
    APPLIB_MEDIA_INFO_s MediaInfo;

    UINT64 CurrFilePos = AppLibStorageDmf_GetCurrFilePos(pb_video.FileInfo.MediaRoot, DCIM_HDLR);
    AmbaPrintColor(GREEN, "[app_pb_video] pb_video.FileInfo.FileCur: %d, CurrFilePos =%d", pb_video.FileInfo.FileCur,CurrFilePos);
    for (i=0; i<pb_video.FileInfo.TotalFileNum; i++) {
        switch (param) {
        case GET_PREV_FILE:
            if (pb_video.FileInfo.FileCur == 0) {
                pb_video.FileInfo.FileCur = pb_video.FileInfo.TotalFileNum - 1;
                AppLibStorageDmf_GetLastFilePos(pb_video.FileInfo.MediaRoot, DCIM_HDLR);
            } else {
                pb_video.FileInfo.FileCur --;
                AppLibStorageDmf_GetPrevFilePos(pb_video.FileInfo.MediaRoot, DCIM_HDLR);
            }
            break;
        case GET_CURR_FILE:
           AppLibStorageDmf_GetCurrFilePos(pb_video.FileInfo.MediaRoot, DCIM_HDLR);
            break;
        case GET_NEXT_FILE:
        default:
            if (pb_video.FileInfo.FileCur == (pb_video.FileInfo.TotalFileNum - 1)) {
                pb_video.FileInfo.FileCur = 0;
                AppLibStorageDmf_GetFirstFilePos(pb_video.FileInfo.MediaRoot, DCIM_HDLR);
            } else {
                pb_video.FileInfo.FileCur ++;
                AppLibStorageDmf_GetNextFilePos(pb_video.FileInfo.MediaRoot, DCIM_HDLR);
            }
            break;
        }
        CurrFilePos = AppLibStorageDmf_GetCurrFilePos(pb_video.FileInfo.MediaRoot, DCIM_HDLR);
        AppLibStorageDmf_GetFileName(pb_video.FileInfo.MediaRoot, ".MP4", APPLIB_DCF_EXT_OBJECT_SPLIT_FILE, DCIM_HDLR, (CUR_OBJ_IDX(CurrFilePos) + 1), CUR_OBJ(CurrFilePos), TempFn);

        ReturnValue = AppLibFormat_GetMediaInfo(TempFn, &MediaInfo);
        if ((ReturnValue == AMP_OK) && (MediaInfo.MediaInfoType == AMP_MEDIA_INFO_MOVIE)) {
            pb_video.CurFileObjID = CurrFilePos;
            strcpy(pb_video.CurFn, TempFn);
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
static int pb_video_cal_aspect_ratio(int width,int height)
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
static int pb_video_config_media_info(char *fn)
{
    int ReturnValue = 0;
    //int Width = 0, Height = 0, AspectRatio = 0;
    UINT32 DTS, TimeScale;
    APPLIB_MEDIA_INFO_s MediaInfo;
    ReturnValue = AppLibFormat_GetMediaInfo(fn, &MediaInfo);
    if ((ReturnValue != AMP_OK) || (MediaInfo.MediaInfoType != AMP_MEDIA_INFO_MOVIE))
        return -1;
    DTS = MediaInfo.MediaInfo.Movie->Track[0].NextDTS;
    TimeScale = MediaInfo.MediaInfo.Movie->Track[0].TimeScale;
    //Width = pb_video.MediaInfo.Width;
    //Height = pb_video.MediaInfo.Height;
    //AspectRatio = pb_video.MediaInfo.AspectRatio;
    /// Dimension information
    pb_video.MediaInfo.Width = MediaInfo.MediaInfo.Movie->Track[0].Info.Video.Width;
    pb_video.MediaInfo.Height = MediaInfo.MediaInfo.Movie->Track[0].Info.Video.Height;
    pb_video.MediaInfo.AspectRatio = pb_video_cal_aspect_ratio(pb_video.MediaInfo.Width,pb_video.MediaInfo.Height);
    //pb_video.MediaInfo.Frate = info->media.movi.vid_frame_rate;
    //pb_video.MediaInfo.Ftime = 1000/pb_video.frate;
    //pb_video.MediaInfo.fmode = info->media.movi.mode;

    //pb_video.entropy_mode = info->media.movi.entropy_mode;
    /** Update total time in milliseconds */
    ReturnValue = AppLibVideoDec_SetPtsFrame(MediaInfo.MediaInfo.Movie->Track[0].FrameCount,
                MediaInfo.MediaInfo.Movie->Track[0].TimePerFrame, MediaInfo.MediaInfo.Movie->Track[0].TimeScale);
    if (ReturnValue < 0) {
        AmbaPrintColor(RED,"[app_pb_video] AppLibVideoDec_SetPtsFrame Failure!");
    }

    pb_video.MediaInfo.TotalTime = (((UINT64)(DTS) / (TimeScale)) * 1000 + (((UINT64)(DTS) % (TimeScale)) * 1000) /(TimeScale));
    AmbaPrint("[app_pb_video] MediaInfo.MediaInfo.Movie->Track[0].Info.Video.PixelArX = %d",MediaInfo.MediaInfo.Movie->Track[0].Info.Video.PixelArX);
    AmbaPrint("[app_pb_video] MediaInfo.MediaInfo.Movie->Track[0].Info.Video.PixelArY = %d",MediaInfo.MediaInfo.Movie->Track[0].Info.Video.PixelArY);
    AmbaPrint("[app_pb_video] MediaInfo.MediaInfo.Movie->Track[0].FrameCount = %d",MediaInfo.MediaInfo.Movie->Track[0].FrameCount);
    AmbaPrint("[app_pb_video] MediaInfo.MediaInfo.Movie->Track[0].TimePerFrame = %d",MediaInfo.MediaInfo.Movie->Track[0].TimePerFrame);
    AmbaPrint("[app_pb_video] MediaInfo.MediaInfo.Movie->Track[0].TimeScale = %d",MediaInfo.MediaInfo.Movie->Track[0].TimeScale);
    AmbaPrint("[app_pb_video] MediaInfo.MediaInfo.Movie->Track[0].DTS = %lld",MediaInfo.MediaInfo.Movie->Track[0].NextDTS);
    AmbaPrint("[app_pb_video] pb_video.MediaInfo.TotalTime = %lld",pb_video.MediaInfo.TotalTime);
    AmbaPrint("[app_pb_video] pb_video.MediaInfo.Width = %d",pb_video.MediaInfo.Width);
    AmbaPrint("[app_pb_video] pb_video.MediaInfo.Height = %d",pb_video.MediaInfo.Height);
    AmbaPrint("[app_pb_video] pb_video.MediaInfo.AspectRatio = %d",pb_video.MediaInfo.AspectRatio);
    //pb_video.Gui(GUI_MEDIA_INFO_SHOW, 0, 0);
    //pb_video.Gui(GUI_MEDIA_INFO_UPDATE, 0, info)

    //if ((Width != pb_video.MediaInfo.Width) || (Height != pb_video.MediaInfo.Height) || (AspectRatio != pb_video.MediaInfo.AspectRatio))
    {
        AMP_DISP_WINDOW_CFG_s Window;
        APPLIB_VOUT_PREVIEW_PARAM_s PreviewParam={0};
        AMP_DISP_INFO_s DispDev = {0};

        memset(&Window, 0x0, sizeof(AMP_DISP_WINDOW_CFG_s));

        PreviewParam.AspectRatio = pb_video.MediaInfo.AspectRatio;
        ReturnValue = AppLibDisp_GetDeviceInfo(DISP_CH_FCHAN, &DispDev);
        if ((ReturnValue < 0) || (DispDev.DeviceInfo.Enable == 0)) {
            DBGMSG("[app_pb_video] FChan Disable. Disable the fchan Window");
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
            DBGMSG("[app_pb_video] DChan Disable. Disable the Dchan Window");
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
            AppLibDisp_SetWindowConfig(DISP_CH_DCHAN, AppLibDisp_GetWindowId(DISP_CH_DCHAN, 0), &Window);
            AppLibDisp_ActivateWindow(DISP_CH_DCHAN, AppLibDisp_GetWindowId(DISP_CH_DCHAN, 0));
        }
        AppLibDisp_FlushWindow(DISP_CH_FCHAN | DISP_CH_DCHAN);
    }
#if 0
    {
        char ft[64];
        uni_to_asc(pb_video.CurFn, ft);
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
static int pb_video_open(UINT32 param)
{
    APPLIB_VIDEO_START_INFO_s VideoStartInfo; // Video configuration
    int ReturnValue = 0;
    WCHAR FileName[MAX_FILENAME_LENGTH];

    pb_video_config_media_info(pb_video.CurFn);

    AmbaUtility_Ascii2Unicode(pb_video.CurFn, FileName);

    pb_video.Gui(GUI_PLAY_TIMER_UPDATE, 0, 0);
    pb_video.Gui(GUI_PLAY_TIMER_SHOW, 0, 0);
    pb_video.Gui(GUI_FILENAME_UPDATE, (UINT32)FileName, GUI_PB_FN_STYLE_HYPHEN);
    pb_video.Gui(GUI_FILENAME_SHOW, 0, 0);
    pb_video.Gui(GUI_FLUSH, 0, 0);
    // Set video configuration
    AppLibVideoDec_GetStartDefaultCfg(&VideoStartInfo);
    VideoStartInfo.Filename = pb_video.CurFn;
    VideoStartInfo.AutoPlay = 0; // Open and pause
    if (param == PB_VIDEO_OPEN_RESET) {
        /** Update play time */
        pb_video.MediaInfo.PlayTime = 0;
        pb_video.MediaInfo.Direction = PB_VIDEO_PLAY_FWD;
        pb_video.MediaInfo.State = PB_VIDEO_PLAY_PAUSED;
        pb_video.MediaInfo.Speed = PBACK_SPEED_NORMAL;
        ReturnValue = AppLibVideoDec_Start(&VideoStartInfo);
    } else {    /** PB_VIDEO_OPEN_VIDEO_CONT */
        ReturnValue = AppLibVideoDec_Start(&VideoStartInfo);
        if (pb_video.MediaInfo.Direction == PB_VIDEO_PLAY_REV) {
            pb_video.MediaInfo.PlayTime = pb_video.MediaInfo.TotalTime;
            //ReturnValue = app_pback_video_search_time(pb_video.MediaInfo.TotalTime);
        } else {    /** PB_VIDEO_PLAY_FWD */
            pb_video.MediaInfo.PlayTime = 0;
        }
        ReturnValue = pb_video.Func(PB_VIDEO_PLAY, 0, 0);
        if (!APP_CHECKFLAGS(app_pb_video.GFlags, APP_AFLAGS_BUSY)) {
            /* The busy flag will be removed when the flow stop the video player. */
            /* To excute the functions that system block them when the Busy flag is enabled. */
            AppUtil_BusyCheck(0);
        }
        if (pb_video.MediaInfo.State == PB_VIDEO_PLAY_PLAY) {
            if (pb_video.MediaInfo.Speed < PBACK_SPEED_NORMAL) {
                if (pb_video.MediaInfo.Direction == PB_VIDEO_PLAY_REV) {
                    pb_video.Gui(GUI_PLAY_STATE_UPDATE, GUI_REW_SLOW, pb_video.MediaInfo.Speed);
                } else {
                    pb_video.Gui(GUI_PLAY_STATE_UPDATE, GUI_FWD_SLOW, pb_video.MediaInfo.Speed);
                }
            } else if (pb_video.MediaInfo.Speed > PBACK_SPEED_NORMAL) {
                if (pb_video.MediaInfo.Direction == PB_VIDEO_PLAY_REV) {
                    pb_video.Gui(GUI_PLAY_STATE_UPDATE, GUI_REW_FAST, pb_video.MediaInfo.Speed);
                } else {
                    pb_video.Gui(GUI_PLAY_STATE_UPDATE, GUI_FWD_FAST, pb_video.MediaInfo.Speed);
                }
            }
        }

        pb_video.Gui(GUI_PLAY_TIMER_UPDATE, pb_video.MediaInfo.PlayTime/1000, pb_video.MediaInfo.TotalTime/1000);
        pb_video.Gui(GUI_FLUSH, 0, 0);
    }
    if (ReturnValue >= 0) {
        AppLibComSvcTimer_Register(TIMER_1HZ, pb_video_play_timer_handler);
        AppLibVideoDec_GetTime(&pb_video.MediaInfo.PlayTime);
        pb_video.Gui(GUI_PLAY_TIMER_UPDATE, pb_video.MediaInfo.PlayTime/1000, pb_video.MediaInfo.TotalTime/1000);
        pb_video.Gui(GUI_FLUSH, 0, 0);
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
static int pb_video_open_play_curr(UINT32 param)
{
    int ReturnValue = 0;
    APPLIB_VIDEO_START_INFO_s VideoStartInfo;
    if (pb_video.MediaInfo.State == PB_VIDEO_PLAY_PLAY) {
        VideoStartInfo.Filename = pb_video.CurFn;
        VideoStartInfo.StartTime = pb_video.MediaInfo.PlayTime;
        VideoStartInfo.Direction = (APPLIB_VIDEO_PLAY_DIRECTION_e)pb_video.MediaInfo.Direction;
        VideoStartInfo.ResetSpeed = 0;
        VideoStartInfo.ResetZoom = 0;
        VideoStartInfo.AutoPlay = 1;
        AmbaPrintColor(MAGENTA,"[app_pb_video] Play Direction %d,StartTime %d",pb_video.MediaInfo.Direction,pb_video.MediaInfo.PlayTime);
        ReturnValue = AppLibVideoDec_Start(&VideoStartInfo);
        APP_ADDFLAGS(app_pb_video.GFlags, APP_AFLAGS_BUSY);
    } else if (pb_video.MediaInfo.State == PB_VIDEO_PLAY_PAUSED) {
        AppLibVideoDec_Pause();
        APP_REMOVEFLAGS(app_pb_video.GFlags, APP_AFLAGS_BUSY);
    } else {
        AmbaPrintColor(RED,"[app_pb_video] pb_video_open_play_curr error");
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
static int pb_video_do_play_eos(void)
{
    int ReturnValue = 0;
    char TempFn[MAX_FILENAME_LENGTH] = {0};

    AppLibComSvcTimer_Unregister(TIMER_1HZ, pb_video_play_timer_handler);
    if (pb_video.MediaInfo.Direction == PB_VIDEO_PLAY_REV) {
        pb_video.Gui(GUI_PLAY_TIMER_UPDATE, 0/1000, pb_video.MediaInfo.TotalTime/1000);
    } else {
        pb_video.Gui(GUI_PLAY_TIMER_UPDATE, pb_video.MediaInfo.TotalTime/1000, pb_video.MediaInfo.TotalTime/1000);
    }
    pb_video.Gui(GUI_FLUSH, 0, 0);

    switch (UserSetting->PlaybackPref.VideoPlayOpt) {
    case PB_OPT_VIDEO_PLAY_ALL:
        AmbaPrint("[app_pb_video] PB_OPT_VIDEO_PLAY_ALL");
        AppLibVideoDec_Stop();
        strcpy(TempFn, pb_video.CurFn);
        if (pb_video.MediaInfo.Direction == PB_VIDEO_PLAY_REV) {
            ReturnValue = pb_video.Func(PB_VIDEO_GET_FILE, GET_PREV_FILE, 0);
        } else {
            ReturnValue = pb_video.Func(PB_VIDEO_GET_FILE, GET_NEXT_FILE, 0);
        }
        if (strcmp(pb_video.CurFn, pb_video.FirstFn) == 0) {
            pb_video.MediaInfo.State = PB_VIDEO_PLAY_PAUSED;
            pb_video.Func(PB_VIDEO_PLAY, 0, 0);
            if (!APP_CHECKFLAGS(app_pb_video.GFlags, APP_AFLAGS_BUSY)) {
                /* The busy flag will be removed when the flow stop the video player. */
                /* To excute the functions that system block them when the Busy flag is enabled. */
                AppUtil_BusyCheck(0);
            }
            if (APP_CHECKFLAGS(app_pb_video.GFlags, APP_AFLAGS_READY)) {
                /* The system could switch the current app to other in the function "AppUtil_BusyCheck". */
                pb_video.Func(PB_VIDEO_SWITCH_APP, 0, 0);
            }
        } else {
            //            ReturnValue = app_format_get_media_info2(pb_video.CurFn, &media_info);
            if (ReturnValue == 0) {
            //                pb_video.Gui(GUI_MEDIA_INFO_UPDATE, 0, &media_info);
                ReturnValue = pb_video.Func(PB_VIDEO_OPEN, PB_VIDEO_OPEN_VIDEO_CONT, 0);
            } else {
                ReturnValue = pb_video.Func(PB_VIDEO_SWITCH_APP, 0, 0);
            }
        }
        break;
    case PB_OPT_VIDEO_REPEAT_ONE:
        AmbaPrint("[app_pb_video] PB_OPT_VIDEO_REPEAT_ONE");
        if (pb_video.MediaInfo.Direction == PB_VIDEO_PLAY_REV) {
            pb_video.MediaInfo.PlayTime = pb_video.MediaInfo.TotalTime;
            //ReturnValue = app_pback_video_search_time(pb_video.MediaInfo.TotalTime);
        } else {
            pb_video.MediaInfo.PlayTime = 0;
            //ReturnValue = app_pback_video_search_time(0);
        }
        pb_video.Func(PB_VIDEO_PLAY, 0, 0);
        if (!APP_CHECKFLAGS(app_pb_video.GFlags, APP_AFLAGS_BUSY)) {
            /* The busy flag will be removed when the flow stop the video player. */
            /* To excute the functions that system block them when the Busy flag is enabled. */
            AppUtil_BusyCheck(0);
        }
        pb_video.Gui(GUI_PLAY_TIMER_UPDATE, pb_video.MediaInfo.PlayTime/1000, pb_video.MediaInfo.TotalTime/1000);
        pb_video.Gui(GUI_FLUSH, 0, 0);
        break;
    case PB_OPT_VIDEO_REPEAT_ALL:
        AmbaPrint("[app_pb_video] PB_OPT_VIDEO_REPEAT_ALL");
        AppLibVideoDec_Stop();
        if (pb_video.MediaInfo.Direction == PB_VIDEO_PLAY_REV) {
            ReturnValue = pb_video.Func(PB_VIDEO_GET_FILE, GET_PREV_FILE, 0);
        } else {
            ReturnValue = pb_video.Func(PB_VIDEO_GET_FILE, GET_NEXT_FILE, 0);
        }
        //ReturnValue = app_format_get_media_info2(pb_video.CurFn, &media_info);
        if (ReturnValue == 0) {
            //pb_video.Gui(GUI_MEDIA_INFO_UPDATE, 0, &media_info);
            ReturnValue = pb_video.Func(PB_VIDEO_OPEN, PB_VIDEO_OPEN_VIDEO_CONT, 0);
        } else {
            ReturnValue = pb_video.Func(PB_VIDEO_SWITCH_APP, 0, 0);
        }
        break;
    case PB_OPT_VIDEO_PLAY_ONE:
    default:
        AmbaPrint("[app_pb_video] PB_OPT_VIDEO_PLAY_ONE");
        pb_video.MediaInfo.State = PB_VIDEO_PLAY_PAUSED;
        ReturnValue = pb_video.Func(PB_VIDEO_PLAY, 0, 0);
        AppLibVideoDec_Stop();
        if (!APP_CHECKFLAGS(app_pb_video.GFlags, APP_AFLAGS_BUSY)) {
            /* The busy flag will be removed when the flow stop the video player. */
            /* To excute the functions that system block them when the Busy flag is enabled. */
            AppUtil_BusyCheck(0);
        }
        if (APP_CHECKFLAGS(app_pb_video.GFlags, APP_AFLAGS_READY)) {
            /* The system could switch the current app to other in the function "AppUtil_BusyCheck". */
            pb_video.Func(PB_VIDEO_SWITCH_APP, 0, 0);
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
static int pb_video_switch_app(void)
{
    int ReturnValue = 0;

    if ( app_status.LockDecMode && app_status.FchanDecModeOnly) {
        ReturnValue = AppUtil_SwitchApp(APP_THUMB_MOTION);
    } else {
        ReturnValue = AppUtil_SwitchApp(app_pb_video.Previous);
    }

    return ReturnValue;
}

int pb_video_card_removed(void)
{
    int ReturnValue = 0;

    if (APP_CHECKFLAGS(app_pb_video.GFlags, APP_AFLAGS_POPUP)) {
        APP_REMOVEFLAGS(app_pb_video.Flags, PB_VIDEO_DELETE_FILE_RUN);
        AppWidget_Off(WIDGET_ALL, 0);
    }
    pb_video.Func(PB_VIDEO_START_DISP_PAGE, 0, 0);
    pb_video.Func(PB_VIDEO_WARNING_MSG_SHOW_STOP, 0, 0);
    pb_video.Gui(GUI_CARD_UPDATE, GUI_NO_CARD, 0);
    pb_video.Gui(GUI_MEDIA_INFO_HIDE, 0, 0);
    pb_video.Gui(GUI_FILENAME_HIDE, 0, 0);
    pb_video.Gui(GUI_FLUSH, 0, 0);

    return ReturnValue;
}

int pb_video_card_error_removed(void)
{
    int ReturnValue = 0;

    APP_REMOVEFLAGS(app_pb_video.GFlags, APP_AFLAGS_BUSY);
    pb_video.Func(PB_VIDEO_CARD_REMOVED, 0, 0);

    return ReturnValue;
}

int pb_video_card_new_insert(int param1)
{
    int ReturnValue = 0;

    /* Remove old card.*/
    pb_video.Func(PB_VIDEO_CARD_REMOVED, 0, 0);
    AppLibCard_StatusSetBlock(param1, 0);
    AppLibComSvcAsyncOp_CardInsert(AppLibCard_GetSlot(param1));

    return ReturnValue;
}

int pb_video_card_storage_idle(void)
{
    int ReturnValue = 0;

    pb_video.Func(PB_VIDEO_SET_FILE_INDEX, 0, 0);

    AppUtil_CheckCardParam(0);
    if (!APP_CHECKFLAGS(app_pb_photo.GFlags, APP_AFLAGS_READY)) {
        return ReturnValue;/**<  App switched out*/
    }

    pb_video.Gui(GUI_CARD_UPDATE, GUI_CARD_READY, 0);
    pb_video.Gui(GUI_FLUSH, 0, 0);
    pb_video.Func(PB_VIDEO_START_DISP_PAGE, 0, 0);

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
static int pb_video_dialog_del_handler(UINT32 sel, UINT32 param1, UINT32 param2)
{
    int ReturnValue = 0;

    switch (sel) {
    case DIALOG_SEL_YES:
        pb_video.Func(PB_VIDEO_DELETE_FILE, CUR_OBJ(pb_video.CurFileObjID), CUR_OBJ_IDX(pb_video.CurFileObjID));
        break;
    case DIALOG_SEL_NO:
    default:
        break;
    }

    return ReturnValue;
}

static int pb_video_delete_file_dialog_show(void)
{
    int ReturnValue = 0;

    ReturnValue = AppLibCard_CheckStatus(CARD_CHECK_DELETE);
    if (ReturnValue == 0) {
        AppDialog_SetDialog(DIALOG_TYPE_Y_N, DIALOG_SUB_DEL, pb_video_dialog_del_handler);
        AppWidget_On(WIDGET_DIALOG, 0);
        APP_ADDFLAGS(app_pb_video.GFlags, APP_AFLAGS_POPUP);
    } else if (ReturnValue == CARD_STATUS_WP_CARD) {
        pb_video.Func(PB_VIDEO_WARNING_MSG_SHOW_START, GUI_WARNING_CARD_PROTECTED, 0);
        AmbaPrintColor(RED,"[app_pb_video] WARNING_CARD_PROTECTED");
    } else {
        AmbaPrintColor(RED,"[app_pb_video] WARNING_CARD_Error rval = %d", ReturnValue);
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
static int pb_video_delete_file(UINT32 FileObjID)
{
    int ReturnValue = 0;

    /* Stop player. */
    AppLibVideoDec_Stop();

    AppLibComSvcAsyncOp_DmfFdel(pb_video.FileInfo.MediaRoot, FileObjID, app_status.PlaybackType);

    APP_ADDFLAGS(app_pb_video.Flags, PB_VIDEO_DELETE_FILE_RUN);
    APP_ADDFLAGS(app_pb_video.GFlags, APP_AFLAGS_BUSY);
    pb_video.Func(PB_VIDEO_WARNING_MSG_SHOW_START, GUI_WARNING_PROCESSING, 1);

    return ReturnValue;
}

static int pb_video_delete_file_complete(int param1, int param2)
{
    int ReturnValue = 0;

    if (APP_CHECKFLAGS(app_pb_video.Flags, PB_VIDEO_DELETE_FILE_RUN)) {
        APP_REMOVEFLAGS(app_pb_video.Flags, PB_VIDEO_DELETE_FILE_RUN);
        pb_video.Gui(GUI_WARNING_HIDE, 0, 0);
        pb_video.Gui(GUI_FLUSH, 0, 0);
        APP_REMOVEFLAGS(app_pb_video.GFlags, APP_AFLAGS_BUSY);
        ReturnValue = (int)param1;
        if (ReturnValue < 0) {
            AmbaPrintColor(RED,"[app_pb_video] Delete files failed: %d", param2);
        } else {
            //pb_video.Gui(GUI_ZOOM_RATIO_HIDE, 0, 0);
            //pb_video.Gui(GUI_FLUSH, 0, 0);
            /** page update */
            app_status.ThumbnailModeConti = 0;
            pb_video.Func(PB_VIDEO_START_DISP_PAGE, 0, 0);
        }
    }
    return ReturnValue;
}

static int pb_video_widget_closed(int param1, int param2)
{
    int ReturnValue = 0;

    if (APP_CHECKFLAGS(app_pb_video.GFlags, APP_AFLAGS_POPUP)) {
        APP_REMOVEFLAGS(app_pb_video.GFlags, APP_AFLAGS_POPUP);
    }

    return ReturnValue;
}

static int pb_video_set_system_type(void)
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
static int pb_video_update_fchan_vout(UINT32 msg)
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
        AmbaPrint("[app_pb_video] Vout no changed");
        return 0;
        break;
    }
    ReturnValue = AppLibDisp_SelectDevice(DISP_CH_FCHAN, DISP_ANY_DEV);
    if (APP_CHECKFLAGS(ReturnValue, DISP_FCHAN_NO_CHANGE)) {
        AmbaPrint("[app_pb_video] Display FCHAN has no changed");
    } else {
        AppLibVideoDec_Exit();
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
                    DBGMSG("[app_pb_video] FChan Disable. Disable the fchan window");
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
                }
            }
            AppLibGraph_SetWindowConfig(GRAPH_CH_FCHAN);
            AppLibGraph_ActivateWindow(GRAPH_CH_FCHAN);
            AppLibGraph_FlushWindow(GRAPH_CH_FCHAN);
            pb_video.Gui(GUI_SET_LAYOUT, 0, 0);
            pb_video.Gui(GUI_FLUSH, 0, 0);
        }
        AppLibVideoDec_Init();
        pb_video.Func(PB_VIDEO_START_DISP_PAGE, 0, 0);
    }

    return ReturnValue;
}

int pb_video_change_display(void)
{
    int ReturnValue = 0;

    AppLibDisp_SelectDevice(DISP_CH_FCHAN | DISP_CH_DCHAN, DISP_ANY_DEV);
    AppLibDisp_ConfigMode(DISP_CH_FCHAN | DISP_CH_DCHAN, AppLibSysVout_GetVoutMode(VOUT_DISP_MODE_1080P));
    AppLibDisp_SetupChan(DISP_CH_FCHAN | DISP_CH_DCHAN);
    AppLibDisp_ChanStart(DISP_CH_FCHAN | DISP_CH_DCHAN);
    {
        AMP_DISP_WINDOW_CFG_s Window;
        AMP_DISP_INFO_s DispDev = {0};

        memset(&Window, 0, sizeof(AMP_DISP_WINDOW_CFG_s));

        ReturnValue = AppLibDisp_GetDeviceInfo(DISP_CH_FCHAN, &DispDev);
        if ((ReturnValue < 0) || (DispDev.DeviceInfo.Enable == 0)) {
            DBGMSG("[app_pb_video] FChan Disable. Disable the fchan Window");
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
            DBGMSG("[app_pb_video] DChan Disable. Disable the Dchan Window");
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

int pb_video_change_osd(void)
{
    int ReturnValue = 0;

    /* Update graphic window*/
    AppLibGraph_SetWindowConfig(GRAPH_CH_FCHAN | GRAPH_CH_DCHAN);
    AppLibGraph_ActivateWindow(GRAPH_CH_FCHAN | GRAPH_CH_DCHAN);
    AppLibGraph_FlushWindow(GRAPH_CH_FCHAN | GRAPH_CH_DCHAN);
    pb_video.Gui(GUI_SET_LAYOUT, 0, 0);
    pb_video.Gui(GUI_FLUSH, 0, 0);

    return ReturnValue;
}

static int pb_video_usb_connect(void)
{
    int ReturnValue = 0;

    switch (UserSetting->SetupPref.USBMode) {
    default:
        AppUtil_SwitchApp(APP_USB_MSC);
        break;
    }

    return ReturnValue;
}

static int pb_video_start_show_gui(void)
{
    int ReturnValue = 0;
    int GuiParam = 0;

    // pb_video.Gui(GUI_APP_ICON_SHOW, 0, 0);
    pb_video.Gui(GUI_POWER_STATE_UPDATE, app_status.PowerType, app_status.BatteryState);
    pb_video.Gui(GUI_POWER_STATE_SHOW, app_status.PowerType, app_status.BatteryState);
    ReturnValue = AppLibCard_CheckStatus(0);
    if (ReturnValue == CARD_STATUS_NO_CARD) {
        GuiParam = GUI_NO_CARD;
    } else {
        GuiParam = GUI_CARD_READY;
    }
    pb_video.Gui(GUI_CARD_UPDATE, GuiParam, 0);
    pb_video.Gui(GUI_CARD_SHOW, 0, 0);
    pb_video.Gui(GUI_PLAY_STATE_UPDATE, GUI_PAUSE, 0);
    pb_video.Gui(GUI_PLAY_TIMER_UPDATE, 0, pb_video.MediaInfo.TotalTime/1000);
    pb_video.Gui(GUI_PLAY_STATE_SHOW, 0, 0);
    pb_video.Gui(GUI_MEDIA_INFO_HIDE, 0, 0);
    pb_video.Gui(GUI_FILENAME_HIDE, 0, 0);
    pb_video.Gui(GUI_FLUSH, 0, 0);

    return ReturnValue;
}


static int pb_video_update_bat_power_status(int param1)
{
    int ReturnValue = 0;

    /* Update the gui of power's status. */
    if (param1 == 0) {
        /*Hide the battery gui.*/
        pb_video.Gui(GUI_POWER_STATE_HIDE, GUI_HIDE_POWER_EXCEPT_DC, 0);
    } else if (param1 == 1) {
        /*Update the battery gui.*/
        pb_video.Gui(GUI_POWER_STATE_UPDATE, app_status.PowerType, app_status.BatteryState);
        pb_video.Gui(GUI_POWER_STATE_SHOW, app_status.PowerType, app_status.BatteryState);
    } else if (param1 == 2) {
        /*Reset the battery and power gui.*/
        pb_video.Gui(GUI_POWER_STATE_HIDE, 0, 0);
        pb_video.Gui(GUI_POWER_STATE_UPDATE, app_status.PowerType, app_status.BatteryState);
        pb_video.Gui(GUI_POWER_STATE_SHOW, app_status.PowerType, app_status.BatteryState);
    }
    pb_video.Gui(GUI_FLUSH, 0, 0);

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
static void pb_video_warning_timer_handler(int eid)
{
    static int BlinkCount;

    if (eid == TIMER_UNREGISTER) {
        BlinkCount = 0;
        return;
    }

    BlinkCount++;

    if (BlinkCount & 0x01) {
        pb_video.Gui(GUI_WARNING_HIDE, 0, 0);
    } else {
        pb_video.Gui(GUI_WARNING_SHOW, 0, 0);
    }

    if (BlinkCount >= 5) {
        APP_REMOVEFLAGS(app_pb_video.Flags, PB_VIDEO_WARNING_MSG_RUN);
        AppLibComSvcTimer_Unregister(TIMER_2HZ, pb_video_warning_timer_handler);
        pb_video.Gui(GUI_WARNING_HIDE, 0, 0);
    }
    pb_video.Gui(GUI_FLUSH, 0, 0);

}

static int pb_video_warning_msg_show(int enable, int param1, int param2)
{
    int ReturnValue = 0;

    if (enable) {
        if (param2) {
            pb_video.Gui(GUI_WARNING_UPDATE, param1, 0);
            pb_video.Gui(GUI_WARNING_SHOW, 0, 0);
        } else {
            if (!APP_CHECKFLAGS(app_pb_video.Flags, PB_VIDEO_WARNING_MSG_RUN)) {
                APP_ADDFLAGS(app_pb_video.Flags, PB_VIDEO_WARNING_MSG_RUN);
                pb_video.Gui(GUI_WARNING_UPDATE, param1, 0);
                pb_video.Gui(GUI_WARNING_SHOW, 0, 0);
                AppLibComSvcTimer_Register(TIMER_2HZ, pb_video_warning_timer_handler);
            }
        }
    } else {
        if (APP_CHECKFLAGS(app_pb_video.Flags, PB_VIDEO_WARNING_MSG_RUN)) {
            APP_REMOVEFLAGS(app_pb_video.Flags, PB_VIDEO_WARNING_MSG_RUN);
            AppLibComSvcTimer_Unregister(TIMER_2HZ, pb_video_warning_timer_handler);
        }
        pb_video.Gui(GUI_WARNING_HIDE, 0, 0);
    }
    pb_video.Gui(GUI_FLUSH, 0, 0);

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
int pb_video_func(UINT32 funcId, UINT32 param1, UINT32 param2)
{
    int ReturnValue = 0;

    switch (funcId) {
    case PB_VIDEO_INIT:
        ReturnValue = pb_video_init();
        break;
    case PB_VIDEO_START:
        ReturnValue = pb_video_start();
        break;
    case PB_VIDEO_STOP:
        ReturnValue = pb_video_stop();
        break;
    case PB_VIDEO_APP_READY:
        ReturnValue = pb_video_app_ready();
        break;
    case PB_VIDEO_START_DISP_PAGE:
        ReturnValue = pb_video_start_disp_page();
        break;
    case PB_VIDEO_OPEN:
        ReturnValue = pb_video_open(param1);
        break;
    case PB_VIDEO_PLAY:
        ReturnValue = pb_video_open_play_curr(param1);
        break;
    case PB_VIDEO_EOS:
        ReturnValue = pb_video_do_play_eos();
        break;
    case PB_VIDEO_SWITCH_APP:
        ReturnValue = pb_video_switch_app();
        break;
    case PB_VIDEO_GET_FILE:
        ReturnValue = pb_video_get_file(param1);
        break;
    case PB_VIDEO_CARD_ERROR_REMOVED:
        ReturnValue = pb_video_card_error_removed();
        break;
    case PB_VIDEO_CARD_REMOVED:
        ReturnValue = pb_video_card_removed();
        break;
    case PB_VIDEO_CARD_NEW_INSERT:
        ReturnValue = pb_video_card_new_insert(param1);
        break;
    case PB_VIDEO_CARD_STORAGE_IDLE:
        ReturnValue = pb_video_card_storage_idle();
        break;
    case PB_VIDEO_DELETE_FILE_DIALOG_SHOW:
        ReturnValue = pb_video_delete_file_dialog_show();
        break;
    case PB_VIDEO_DELETE_FILE:
        ReturnValue = pb_video_delete_file(param1);
        break;
    case PB_VIDEO_DELETE_FILE_COMPLETE:
        ReturnValue = pb_video_delete_file_complete(param1, param2);
        break;
    case PB_VIDEO_STATE_WIDGET_CLOSED:
        ReturnValue = pb_video_widget_closed(param1, param2);
        break;
    case PB_VIDEO_SET_SYSTEM_TYPE:
        ReturnValue = pb_video_set_system_type();
        break;
    case PB_VIDEO_UPDATE_FCHAN_VOUT:
        ReturnValue = pb_video_update_fchan_vout(param1);
        break;
    case PB_VIDEO_UPDATE_DCHAN_VOUT:
        break;
    case PB_VIDEO_CHANGE_DISPLAY:
        ReturnValue = pb_video_change_display();
        break;
    case PB_VIDEO_CHANGE_OSD:
        ReturnValue = pb_video_change_osd();
        break;
    case PB_VIDEO_USB_CONNECT:
        ReturnValue = pb_video_usb_connect();
        break;
    case PB_VIDEO_GUI_INIT_SHOW:
        ReturnValue = pb_video_start_show_gui();
        break;
    case PB_VIDEO_UPDATE_BAT_POWER_STATUS:
        ReturnValue = pb_video_update_bat_power_status(param1);
        break;
    case PB_VIDEO_WARNING_MSG_SHOW_START:
        ReturnValue = pb_video_warning_msg_show(1, param1, param2);
        break;
    case PB_VIDEO_WARNING_MSG_SHOW_STOP:
        ReturnValue = pb_video_warning_msg_show(0, param1, param2);
        break;
    default:
        break;
    }

    return ReturnValue;
}
