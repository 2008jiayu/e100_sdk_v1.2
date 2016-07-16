/**
  * @file src/app/apps/flow/pb/connectedcam/hdmi_test_func.c
  *
  *  Functions of multi playback application
  *
  * History:
  *    2015/05/25 - [James Wang] created file
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


#include <apps/flow/pb/hdmi_test.h>
#include <system/status.h>
#include <system/app_pref.h>
#include <system/app_util.h>
#include <apps/gui/resource/gui_settle.h>
#include <apps/flow/widget/menu/widget.h>
#include <AmbaUtility.h>

static void hdmi_test_play_timer_handler(int eid)
{
    int TimeLatency = 0;
    if (eid == TIMER_UNREGISTER) {
        return;
    }

    AppLibVideoDec_GetTime(&hdmi_test.MediaInfo.PlayTime);
    TimeLatency = hdmi_test.MediaInfo.PlayTime/1000 - hdmi_test.CurPlayTime/1000;
    hdmi_test.CurPlayTime = hdmi_test.MediaInfo.PlayTime;
    if (TimeLatency == 0) {
        /**Do not update timer when time latency under 1 seconds*/
        return;
    }
    hdmi_test.Gui(GUI_PLAY_TIMER_UPDATE, (UINT32)hdmi_test.MediaInfo.PlayTime/1000, (UINT32)hdmi_test.MediaInfo.TotalTime/1000);
    hdmi_test.Gui(GUI_FLUSH, 0, 0);
}

static int hdmi_test_init(void)
{
    int ReturnValue = 0;

    return ReturnValue;
}

static int hdmi_test_start(void)
{
    int ReturnValue = 0;

    hdmi_test.FileInfo.MediaRoot = (APPLIB_DCF_MEDIA_TYPE_e)app_status.ThumbnailModeMediaRoot;
    UserSetting->SystemPref.SystemMode = APP_MODE_DEC;
    memset(&hdmi_test.MediaInfo, 0, sizeof(HDMI_TEST_MEDIA_INFO_s));

    /** Set menus */
    AppMenu_Reset();
    AppMenu_RegisterTab(MENU_SETUP);
    AppMenu_RegisterTab(MENU_PBACK);

    /** Initialize the demuxer. */
    AppLibFormat_DemuxerInit();
    /** Initialize the video/photo decoder according to media root type. */
    if (hdmi_test.FileInfo.MediaRoot == APPLIB_DCF_MEDIA_VIDEO) {
        AppLibVideoDec_Init();
    } else if (hdmi_test.FileInfo.MediaRoot == APPLIB_DCF_MEDIA_IMAGE) {
        AppLibStillSingle_Init();
    } else {
        AppLibVideoDec_Init();
        AppLibStillSingle_Init();
    }
    return ReturnValue;
}

static int hdmi_test_stop(void)
{
    int ReturnValue = 0;

    /* Stop the play timer. */
    AppLibComSvcTimer_Unregister(TIMER_10HZ, hdmi_test_play_timer_handler);
    hdmi_test.Gui(GUI_PLAY_TIMER_HIDE, 0, 0);

    /** Close the video/photo decoder according to media root type. */
    if (hdmi_test.FileInfo.MediaRoot == APPLIB_DCF_MEDIA_VIDEO) {
        AppLibVideoDec_Exit();
    } else if (hdmi_test.FileInfo.MediaRoot == APPLIB_DCF_MEDIA_IMAGE) {
        AppLibStillSingle_Deinit();
    } else {
        AppLibVideoDec_Exit();
        AppLibStillSingle_Deinit();
    }

    /* Close the menu or dialog. */
    AppWidget_Off(WIDGET_ALL, WIDGET_HIDE_SILENT);
    APP_REMOVEFLAGS(app_hdmi_test.GFlags, APP_AFLAGS_POPUP);

    /* Disable the vout. */
    AppLibDisp_DeactivateWindow(DISP_CH_FCHAN, AppLibDisp_GetWindowId(DISP_CH_FCHAN, 0));
    AppLibDisp_DeactivateWindow(DISP_CH_DCHAN, AppLibDisp_GetWindowId(DISP_CH_DCHAN, 0));
    AppLibDisp_ChanStop(DISP_CH_FCHAN);



    /* Hide GUI */
    hdmi_test.Gui(GUI_HIDE_ALL, 0, 0);
    hdmi_test.Gui(GUI_FLUSH, 0, 0);

    return ReturnValue;
}

static int hdmi_test_app_ready(void)
{
    int ReturnValue = 0;

    if (!APP_CHECKFLAGS(app_hdmi_test.GFlags, APP_AFLAGS_READY)) {
        APP_ADDFLAGS(app_hdmi_test.GFlags, APP_AFLAGS_READY);
        AppUtil_ReadyCheck(0);
        if (!APP_CHECKFLAGS(app_hdmi_test.GFlags, APP_AFLAGS_READY)) {
            /* The system could switch the current app to other in the function "AppUtil_ReadyCheck". */
            return ReturnValue;
        }
    }
    // ToDo: need to remove to handler when iav completes the dsp cmd queue mechanism
    AppLibGraph_Init();
    hdmi_test.Func(HDMI_TEST_CHANGE_OSD, 0, 0);

    if (APP_CHECKFLAGS(app_hdmi_test.GFlags, APP_AFLAGS_READY)) {
        hdmi_test.Func(HDMI_TEST_START_DISP_PAGE, 0, 0);
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
static int hdmi_test_init_file_info(void)
{
    UINT64 ReturnValue = 0;

    ReturnValue = AppLibCard_CheckStatus(0);

    if (ReturnValue == 0) {
        hdmi_test.FileInfo.TotalFileNum = AppLibStorageDmf_GetFileAmount(hdmi_test.FileInfo.MediaRoot, app_status.PlaybackType);
    } else {
        hdmi_test.FileInfo.TotalFileNum = 0;
    }
    AmbaPrintColor(GREEN, "[app_hdmi_test] hdmi_test.FileInfo.TotalFileNum: %d", hdmi_test.FileInfo.TotalFileNum);
    if (hdmi_test.FileInfo.TotalFileNum > 0) {
        int i = 0;
        UINT64 CurrFilePos = 0;
        CurrFilePos = AppLibStorageDmf_GetCurrFilePos(hdmi_test.FileInfo.MediaRoot, app_status.PlaybackType);
        if (CurrFilePos == 0) {
            CurrFilePos = AppLibStorageDmf_GetLastFilePos(hdmi_test.FileInfo.MediaRoot, app_status.PlaybackType);
        }
        ReturnValue = AppLibStorageDmf_GetFileName(hdmi_test.FileInfo.MediaRoot, ".MP4", APPLIB_DCF_EXT_OBJECT_SPLIT_FILE, app_status.PlaybackType, (CUR_OBJ_IDX(CurrFilePos) + 1),CUR_OBJ(CurrFilePos), hdmi_test.CurFn);
        if (ReturnValue != 0) {
            ReturnValue = AppLibStorageDmf_GetFileName(hdmi_test.FileInfo.MediaRoot, ".JPG", APPLIB_DCF_EXT_OBJECT_SPLIT_FILE, app_status.PlaybackType, (CUR_OBJ_IDX(CurrFilePos) + 1),CUR_OBJ(CurrFilePos), hdmi_test.CurFn);
        }
        hdmi_test.CurFileObjID = CurrFilePos;
        ReturnValue = AppLibStorageDmf_GetLastFilePos(hdmi_test.FileInfo.MediaRoot, app_status.PlaybackType);
        for (i = (hdmi_test.FileInfo.TotalFileNum - 1) ; i >= 0; i--) {
            if (ReturnValue == CurrFilePos) {
                hdmi_test.FileInfo.FileCur = i;
                break;
            } else {
                ReturnValue = AppLibStorageDmf_GetPrevFilePos(hdmi_test.FileInfo.MediaRoot, app_status.PlaybackType);
            }
        }
        AmbaPrintColor(GREEN, "[app_hdmi_test] hdmi_test.FileInfo.FileCur: %d, CurrFilePos =%d , index = %d", hdmi_test.FileInfo.FileCur,CUR_OBJ(CurrFilePos), CUR_OBJ_IDX(CurrFilePos));
    } else {
        hdmi_test.FileInfo.TotalFileNum = 0;
        hdmi_test.FileInfo.FileCur = 0;
        hdmi_test.CurFileObjID = 0;
        memset(hdmi_test.CurFn, 0, MAX_FILENAME_LENGTH*sizeof(char));
    }

    return CUR_OBJ(ReturnValue);
}

static int hdmi_test_start_disp_page(int param1)
{
    int ReturnValue = 0;

    ReturnValue = hdmi_test_init_file_info();
    if (hdmi_test.FileInfo.TotalFileNum > 0) {
        hdmi_test.Func(HDMI_TEST_GET_FILE, GET_CURR_FILE, hdmi_test.FileInfo.MediaRoot);
        hdmi_test.Func(HDMI_TEST_GUI_INIT_SHOW, 0, 0);

        switch (hdmi_test.CurFileType) {
        case HDMI_TEST_MEDIA_IMAGE:
            hdmi_test.Func(HDMI_TEST_PLAY, 0, 0);/**<param2 : 0 photo : 1 video */
            break;
        case HDMI_TEST_MEDIA_VIDEO:
            strcpy(hdmi_test.FirstFn, hdmi_test.CurFn);
            hdmi_test.Func(HDMI_TEST_OPEN, HDMI_TEST_OPEN_RESET, 0);
            /* Play the multi clip. */
            hdmi_test.MediaInfo.State = HDMI_TEST_PLAY_PLAY;
            hdmi_test.Func(HDMI_TEST_PLAY, 0, 1);/**<param2 : 0 photo : 1 video */
            hdmi_test.Gui(GUI_PLAY_STATE_UPDATE, GUI_FWD_NORMAL, 0);
            break;
        default:
            break;
        }

        hdmi_test.Gui(GUI_FLUSH, 0, 0);

    } else {
        hdmi_test.Func(HDMI_TEST_SWITCH_APP, 0, 0);
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
static int hdmi_test_get_file(UINT32 param, UINT32 MediaType)
{
    int ReturnValue = -1;
    int i = 0;
    UINT64 FilePos = 0;
    char TempFn[MAX_FILENAME_LENGTH] = {0};
    APPLIB_MEDIA_INFO_s MediaInfo;

    UINT64 CurrFilePos;
    CurrFilePos = AppLibStorageDmf_GetCurrFilePos(hdmi_test.FileInfo.MediaRoot, app_status.PlaybackType);
    AmbaPrintColor(GREEN, "[app_hdmi_test] hdmi_test.FileInfo.FileCur: %d, CurrFilePos =%d", hdmi_test.FileInfo.FileCur,CurrFilePos);
    for (i=0; i<hdmi_test.FileInfo.TotalFileNum; i++) {
        switch (param) {
        case GET_PREV_FILE:
            if (hdmi_test.FileInfo.FileCur == 0) {
                hdmi_test.FileInfo.FileCur = hdmi_test.FileInfo.TotalFileNum - 1;
                FilePos = AppLibStorageDmf_GetLastFilePos(hdmi_test.FileInfo.MediaRoot, app_status.PlaybackType);
            } else {
                hdmi_test.FileInfo.FileCur --;
                FilePos = AppLibStorageDmf_GetPrevFilePos(hdmi_test.FileInfo.MediaRoot, app_status.PlaybackType);
            }
            break;
        case GET_CURR_FILE:
            FilePos = AppLibStorageDmf_GetCurrFilePos(hdmi_test.FileInfo.MediaRoot, app_status.PlaybackType);
            break;
        case GET_NEXT_FILE:
        default:
            if (hdmi_test.FileInfo.FileCur == (hdmi_test.FileInfo.TotalFileNum - 1)) {
                hdmi_test.FileInfo.FileCur = 0;
                FilePos = AppLibStorageDmf_GetFirstFilePos(hdmi_test.FileInfo.MediaRoot, app_status.PlaybackType);
            } else {
                hdmi_test.FileInfo.FileCur ++;
                FilePos = AppLibStorageDmf_GetNextFilePos(hdmi_test.FileInfo.MediaRoot, app_status.PlaybackType);
            }
            break;
        }
        switch (MediaType) {
            case APPLIB_DCF_MEDIA_VIDEO:
                ReturnValue = AppLibStorageDmf_GetFileName(hdmi_test.FileInfo.MediaRoot, ".MP4", APPLIB_DCF_EXT_OBJECT_SPLIT_FILE, app_status.PlaybackType, (CUR_OBJ_IDX(FilePos) + 1), CUR_OBJ(FilePos), TempFn);
            break;
            case APPLIB_DCF_MEDIA_IMAGE:
                ReturnValue = AppLibStorageDmf_GetFileName(hdmi_test.FileInfo.MediaRoot, ".JPG", APPLIB_DCF_EXT_OBJECT_SPLIT_FILE, app_status.PlaybackType, (CUR_OBJ_IDX(FilePos) + 1), CUR_OBJ(FilePos), TempFn);
            break;
            case APPLIB_DCF_MEDIA_AUDIO:
            break;
            case APPLIB_DCF_MEDIA_DCIM:
            default:
                ReturnValue = AppLibStorageDmf_GetFileName(hdmi_test.FileInfo.MediaRoot, ".MP4", APPLIB_DCF_EXT_OBJECT_SPLIT_FILE, app_status.PlaybackType, (CUR_OBJ_IDX(FilePos) + 1), CUR_OBJ(FilePos), TempFn);
                if (ReturnValue != 0) {
                    ReturnValue = AppLibStorageDmf_GetFileName(hdmi_test.FileInfo.MediaRoot, ".JPG", APPLIB_DCF_EXT_OBJECT_SPLIT_FILE, app_status.PlaybackType, (CUR_OBJ_IDX(FilePos) + 1), CUR_OBJ(FilePos), TempFn);
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
            strcpy(hdmi_test.CurFn, TempFn);
            hdmi_test.CurFileObjID = FilePos;
            hdmi_test.CurFileType = MediaInfo.MediaInfoType;
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
static int hdmi_test_cal_aspect_ratio(int width,int height)
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
static int hdmi_test_config_media_info(char *fn)
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
    //Width = hdmi_test.MediaInfo.Width;
    //Height = hdmi_test.MediaInfo.Height;
    //AspectRatio = hdmi_test.MediaInfo.AspectRatio;
    /// Dimension information
    hdmi_test.MediaInfo.Width = MediaInfo.MediaInfo.Movie->Track[0].Info.Video.Width;
    hdmi_test.MediaInfo.Height = MediaInfo.MediaInfo.Movie->Track[0].Info.Video.Height;
    hdmi_test.MediaInfo.AspectRatio = hdmi_test_cal_aspect_ratio(hdmi_test.MediaInfo.Width,hdmi_test.MediaInfo.Height);
    //hdmi_test.MediaInfo.Frate = info->media.movi.vid_frame_rate;
    //hdmi_test.MediaInfo.Ftime = 1000/hdmi_test.frate;
    //hdmi_test.MediaInfo.fmode = info->media.movi.mode;

    //hdmi_test.entropy_mode = info->media.movi.entropy_mode;
    /** Update total time in milliseconds */
    ReturnValue = AppLibVideoDec_SetPtsFrame(MediaInfo.MediaInfo.Movie->Track[0].FrameCount,
                MediaInfo.MediaInfo.Movie->Track[0].TimePerFrame, MediaInfo.MediaInfo.Movie->Track[0].TimeScale);
    if (ReturnValue < 0) {
        AmbaPrintColor(RED,"[app_hdmi_test] AppLibVideoDec_SetPtsFrame Failure!");
    }

    hdmi_test.MediaInfo.TotalTime = (((UINT64)(DTS) / (TimeScale)) * 1000 + (((UINT64)(DTS) % (TimeScale)) * 1000) /(TimeScale));
    AmbaPrint("[app_hdmi_test] MediaInfo.MediaInfo.Movie->Track[0].Info.Video.PixelArX = %d",MediaInfo.MediaInfo.Movie->Track[0].Info.Video.PixelArX);
    AmbaPrint("[app_hdmi_test] MediaInfo.MediaInfo.Movie->Track[0].Info.Video.PixelArY = %d",MediaInfo.MediaInfo.Movie->Track[0].Info.Video.PixelArY);
    AmbaPrint("[app_hdmi_test] MediaInfo.MediaInfo.Movie->Track[0].FrameCount = %d",MediaInfo.MediaInfo.Movie->Track[0].FrameCount);
    AmbaPrint("[app_hdmi_test] MediaInfo.MediaInfo.Movie->Track[0].TimePerFrame = %d",MediaInfo.MediaInfo.Movie->Track[0].TimePerFrame);
    AmbaPrint("[app_hdmi_test] MediaInfo.MediaInfo.Movie->Track[0].TimeScale = %d",MediaInfo.MediaInfo.Movie->Track[0].TimeScale);
    AmbaPrint("[app_hdmi_test] MediaInfo.MediaInfo.Movie->Track[0].DTS = %lld",MediaInfo.MediaInfo.Movie->Track[0].NextDTS);
    AmbaPrint("[app_hdmi_test] hdmi_test.MediaInfo.TotalTime = %lld",hdmi_test.MediaInfo.TotalTime);
    AmbaPrint("[app_hdmi_test] hdmi_test.MediaInfo.Width = %d",hdmi_test.MediaInfo.Width);
    AmbaPrint("[app_hdmi_test] hdmi_test.MediaInfo.Height = %d",hdmi_test.MediaInfo.Height);
    AmbaPrint("[app_hdmi_test] hdmi_test.MediaInfo.AspectRatio = %d",hdmi_test.MediaInfo.AspectRatio);
    //hdmi_test.Gui(GUI_MEDIA_INFO_SHOW, 0, 0);
    //hdmi_test.Gui(GUI_MEDIA_INFO_UPDATE, 0, info)

    //if ((Width != hdmi_test.MediaInfo.Width) || (Height != hdmi_test.MediaInfo.Height) || (AspectRatio != hdmi_test.MediaInfo.AspectRatio))
    {
        AMP_DISP_WINDOW_CFG_s Window;
        APPLIB_VOUT_PREVIEW_PARAM_s PreviewParam={0};
        AMP_DISP_INFO_s DispDev = {0};

        memset(&Window, 0x0, sizeof(AMP_DISP_WINDOW_CFG_s));

        PreviewParam.AspectRatio = hdmi_test.MediaInfo.AspectRatio;
        ReturnValue = AppLibDisp_GetDeviceInfo(DISP_CH_FCHAN, &DispDev);
        if ((ReturnValue < 0) || (DispDev.DeviceInfo.Enable == 0)) {
            DBGMSG("[app_hdmi_test] FChan Disable. Disable the fchan Window");
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
            DBGMSG("[app_hdmi_test] DChan Disable. Disable the Dchan Window");
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
        uni_to_asc(hdmi_test.CurFn, ft);
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
static int hdmi_test_open(UINT32 param)
{
    APPLIB_VIDEO_START_INFO_s VideoStartInfo; // Video configuration
    int ReturnValue = 0;
    WCHAR FileName[MAX_FILENAME_LENGTH];

    AmbaUtility_Ascii2Unicode(hdmi_test.CurFn, FileName);
    hdmi_test_config_media_info(hdmi_test.CurFn);

    hdmi_test.Gui(GUI_PLAY_TIMER_UPDATE, 0, 0);
    hdmi_test.Gui(GUI_PLAY_TIMER_SHOW, 0, 0);
    hdmi_test.Gui(GUI_FILENAME_UPDATE, (UINT32)FileName, GUI_PB_FN_STYLE_HYPHEN);
    hdmi_test.Gui(GUI_FILENAME_SHOW, 0, 0);
    hdmi_test.Gui(GUI_FLUSH, 0, 0);
    // Set video configuration
    AppLibVideoDec_GetStartDefaultCfg(&VideoStartInfo);
    VideoStartInfo.Filename = hdmi_test.CurFn;
    VideoStartInfo.AutoPlay = 0; // Open and pause
    if (param == HDMI_TEST_OPEN_RESET) {
        /** Update play time */
        hdmi_test.MediaInfo.PlayTime = 0;
        hdmi_test.CurPlayTime = hdmi_test.MediaInfo.PlayTime;
        hdmi_test.MediaInfo.Direction = HDMI_TEST_PLAY_FWD;
        hdmi_test.MediaInfo.State = HDMI_TEST_PLAY_PAUSED;
        hdmi_test.MediaInfo.Speed = PBACK_SPEED_NORMAL;
        ReturnValue = AppLibVideoDec_Start(&VideoStartInfo);
    } else {    /** HDMI_TEST_OPEN_VIDEO_CONT */
        /**use current setting to start decode*/
        VideoStartInfo.Direction = hdmi_test.MediaInfo.Direction;
        VideoStartInfo.ResetSpeed = 0;
        VideoStartInfo.ResetZoom = 0;
        if (hdmi_test.MediaInfo.Direction == HDMI_TEST_PLAY_REV) {
            hdmi_test.MediaInfo.PlayTime = hdmi_test.MediaInfo.TotalTime;
            VideoStartInfo.StartTime = hdmi_test.MediaInfo.TotalTime;
        } else {    /** HDMI_TEST_PLAY_FWD */
            hdmi_test.MediaInfo.PlayTime = 0;
            VideoStartInfo.StartTime = 0;
        }
        hdmi_test.CurPlayTime = VideoStartInfo.StartTime;
        ReturnValue = AppLibVideoDec_Start(&VideoStartInfo);
        ReturnValue = hdmi_test.Func(HDMI_TEST_PLAY, 0, 1);
        if (!APP_CHECKFLAGS(app_hdmi_test.GFlags, APP_AFLAGS_BUSY)) {
            /* The busy flag will be removed when the flow stop the video player. */
            /* To excute the functions that system block them when the Busy flag is enabled. */
            AppUtil_BusyCheck(0);
        }
        if (hdmi_test.MediaInfo.State == HDMI_TEST_PLAY_PLAY) {
            if (hdmi_test.MediaInfo.Speed < PBACK_SPEED_NORMAL) {
                if (hdmi_test.MediaInfo.Direction == HDMI_TEST_PLAY_REV) {
                    hdmi_test.Gui(GUI_PLAY_STATE_UPDATE, GUI_REW_SLOW, hdmi_test.MediaInfo.Speed);
                } else {
                    hdmi_test.Gui(GUI_PLAY_STATE_UPDATE, GUI_FWD_SLOW, hdmi_test.MediaInfo.Speed);
                }
            } else if (hdmi_test.MediaInfo.Speed > PBACK_SPEED_NORMAL) {
                if (hdmi_test.MediaInfo.Direction == HDMI_TEST_PLAY_REV) {
                    hdmi_test.Gui(GUI_PLAY_STATE_UPDATE, GUI_REW_FAST, hdmi_test.MediaInfo.Speed);
                } else {
                    hdmi_test.Gui(GUI_PLAY_STATE_UPDATE, GUI_FWD_FAST, hdmi_test.MediaInfo.Speed);
                }
            }
        }
    }
    if (ReturnValue >= 0) {
        AppLibComSvcTimer_Register(TIMER_10HZ, hdmi_test_play_timer_handler);
        //AppLibVideoDec_GetTime(&hdmi_test.MediaInfo.PlayTime);
        //hdmi_test.Gui(GUI_PLAY_TIMER_UPDATE, hdmi_test.MediaInfo.PlayTime/1000, hdmi_test.MediaInfo.TotalTime/1000);
        //hdmi_test.Gui(GUI_FLUSH, 0, 0);
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
static int hdmi_test_video_open_play_curr(UINT32 param)
{
    int ReturnValue = 0;
    APPLIB_VIDEO_START_MULTI_INFO_s VideoStartInfo;
    APPLIB_VIDEO_FILE_INFO_s File;

    if (hdmi_test.MediaInfo.State == HDMI_TEST_PLAY_PLAY) {
        File.Filename = hdmi_test.CurFn;
        VideoStartInfo.File = &File;
        VideoStartInfo.FileNum= 1;
        VideoStartInfo.StartTime = hdmi_test.CurPlayTime;
        VideoStartInfo.Direction = (APPLIB_VIDEO_PLAY_DIRECTION_e)hdmi_test.MediaInfo.Direction;
        VideoStartInfo.ResetSpeed = param;  // depends on param to decide reset speed or not.
        VideoStartInfo.ResetZoom = 0;
        VideoStartInfo.AutoPlay = 1;
        VideoStartInfo.ReloadFile = 0;
        AmbaPrintColor(MAGENTA,"[app_hdmi_test] Play Direction %d,StartTime %d",hdmi_test.MediaInfo.Direction,VideoStartInfo.StartTime);
        ReturnValue = AppLibVideoDec_StartMultiple(&VideoStartInfo);
        APP_ADDFLAGS(app_hdmi_test.GFlags, APP_AFLAGS_BUSY);
    } else if (hdmi_test.MediaInfo.State == HDMI_TEST_PLAY_PAUSED) {
        AppLibVideoDec_Pause();
        APP_REMOVEFLAGS(app_hdmi_test.GFlags, APP_AFLAGS_BUSY);
    } else {
        AmbaPrintColor(RED,"[app_hdmi_test] hdmi_test_open_play_curr error");
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
static int hdmi_test_photo_open_play_curr(UINT32 param)
{
    int ReturnValue = 0;
    WCHAR FileName[MAX_FILENAME_LENGTH];
    {
        APPLIB_STILL_FILE_s StillFile = {0};
        memset(&StillFile, 0, sizeof(APPLIB_STILL_FILE_s)); // Initialize StillInfo
        memcpy(StillFile.Filename, hdmi_test.CurFn, sizeof(char)*MAX_FILENAME_LENGTH); // Copy filename
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
        StillInfo.ImageRotate = (AMP_ROTATION_e)hdmi_test.MediaInfo.Rotate; // No rotation and flip

        // Show image
        AppLibStillSingle_Show(&StillInfo);

        AmbaUtility_Ascii2Unicode(hdmi_test.CurFn, FileName);
        hdmi_test.Gui(GUI_FILENAME_UPDATE, (UINT32)FileName, GUI_PB_FN_STYLE_HYPHEN);
        hdmi_test.Gui(GUI_FILENAME_SHOW, 0, 0);
        hdmi_test.Gui(GUI_FLUSH, 0, 0);
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
static int hdmi_test_do_play_eos(void)
{
    int ReturnValue = 0;
    char TempFn[MAX_FILENAME_LENGTH] = {0};

    AppLibComSvcTimer_Unregister(TIMER_10HZ, hdmi_test_play_timer_handler);
    if (hdmi_test.MediaInfo.Direction == HDMI_TEST_PLAY_REV) {
        hdmi_test.Gui(GUI_PLAY_TIMER_UPDATE, 0/1000, hdmi_test.MediaInfo.TotalTime/1000);
    } else {
        hdmi_test.Gui(GUI_PLAY_TIMER_UPDATE, hdmi_test.MediaInfo.TotalTime/1000, hdmi_test.MediaInfo.TotalTime/1000);
    }
    hdmi_test.Gui(GUI_FLUSH, 0, 0);

    switch (UserSetting->PlaybackPref.VideoPlayOpt) {
    case PB_OPT_VIDEO_PLAY_ALL:
        AmbaPrint("[app_hdmi_test] PB_OPT_VIDEO_PLAY_ALL");
        AppLibVideoDec_Stop();
        strcpy(TempFn, hdmi_test.CurFn);
        if (hdmi_test.MediaInfo.Direction == HDMI_TEST_PLAY_REV) {
            ReturnValue = hdmi_test.Func(HDMI_TEST_GET_FILE, GET_PREV_FILE, APPLIB_DCF_MEDIA_VIDEO);
        } else {
            ReturnValue = hdmi_test.Func(HDMI_TEST_GET_FILE, GET_NEXT_FILE, APPLIB_DCF_MEDIA_VIDEO);
        }
        if (ReturnValue == 0) {
            if (strcmp(hdmi_test.CurFn, hdmi_test.FirstFn) == 0) {
                APP_REMOVEFLAGS(app_hdmi_test.GFlags, APP_AFLAGS_BUSY);
                if (!APP_CHECKFLAGS(app_hdmi_test.GFlags, APP_AFLAGS_BUSY)) {
                    /* The busy flag will be removed when the flow stop the video player. */
                    /* To excute the functions that system block them when the Busy flag is enabled. */
                    AppUtil_BusyCheck(0);
                }
                if (APP_CHECKFLAGS(app_hdmi_test.GFlags, APP_AFLAGS_READY)) {
                    /* The system could switch the current app to other in the function "AppUtil_BusyCheck". */
                    hdmi_test.Func(HDMI_TEST_SWITCH_APP, 0, 0);
                }
            } else {
                    ReturnValue = hdmi_test.Func(HDMI_TEST_OPEN, HDMI_TEST_OPEN_VIDEO_CONT, 0);
            }
        }else {
            APP_REMOVEFLAGS(app_hdmi_test.GFlags, APP_AFLAGS_BUSY);
            if (!APP_CHECKFLAGS(app_hdmi_test.GFlags, APP_AFLAGS_BUSY)) {
                /* The busy flag will be removed when the flow stop the video player. */
                /* To excute the functions that system block them when the Busy flag is enabled. */
                AppUtil_BusyCheck(0);
            }
            if (APP_CHECKFLAGS(app_hdmi_test.GFlags, APP_AFLAGS_READY)) {
                /* The system could switch the current app to other in the function "AppUtil_BusyCheck". */
                hdmi_test.Func(HDMI_TEST_SWITCH_APP, 0, 0);
            };
        }
        break;
    case PB_OPT_VIDEO_REPEAT_ONE:
        AmbaPrint("[app_hdmi_test] PB_OPT_VIDEO_REPEAT_ONE");
        if (hdmi_test.MediaInfo.Direction == HDMI_TEST_PLAY_REV) {
            hdmi_test.MediaInfo.PlayTime = hdmi_test.MediaInfo.TotalTime;
            hdmi_test.CurPlayTime = hdmi_test.MediaInfo.TotalTime-1;
        } else {
            hdmi_test.MediaInfo.PlayTime = 0;
            hdmi_test.CurPlayTime = -1;
        }
        hdmi_test.Func(HDMI_TEST_PLAY, 0, 1);
        if (!APP_CHECKFLAGS(app_hdmi_test.GFlags, APP_AFLAGS_BUSY)) {
            /* The busy flag will be removed when the flow stop the video player. */
            /* To excute the functions that system block them when the Busy flag is enabled. */
            AppUtil_BusyCheck(0);
        }
        AppLibComSvcTimer_Register(TIMER_10HZ, hdmi_test_play_timer_handler);
        hdmi_test.Gui(GUI_PLAY_TIMER_UPDATE, hdmi_test.MediaInfo.PlayTime/1000, hdmi_test.MediaInfo.TotalTime/1000);
        hdmi_test.Gui(GUI_FLUSH, 0, 0);
        break;
    case PB_OPT_VIDEO_REPEAT_ALL:
        AmbaPrint("[app_hdmi_test] PB_OPT_VIDEO_REPEAT_ALL");
        AppLibVideoDec_Stop();
        if (hdmi_test.MediaInfo.Direction == HDMI_TEST_PLAY_REV) {
            ReturnValue = hdmi_test.Func(HDMI_TEST_GET_FILE, GET_PREV_FILE, APPLIB_DCF_MEDIA_VIDEO);
        } else {
            ReturnValue = hdmi_test.Func(HDMI_TEST_GET_FILE, GET_NEXT_FILE, APPLIB_DCF_MEDIA_VIDEO);
        }
        if (ReturnValue == 0) {
            if (hdmi_test.CurFileType == HDMI_TEST_MEDIA_VIDEO) {
                ReturnValue = hdmi_test.Func(HDMI_TEST_OPEN, HDMI_TEST_OPEN_VIDEO_CONT, 0);
            } else if (hdmi_test.CurFileType == HDMI_TEST_MEDIA_IMAGE) {
                ReturnValue = hdmi_test.Func(HDMI_TEST_PLAY, 0, 0);
            }
        } else {
            ReturnValue = hdmi_test.Func(HDMI_TEST_SWITCH_APP, 0, 0);
        }
        break;
    case PB_OPT_VIDEO_PLAY_ONE:
    default:
        AmbaPrint("[app_hdmi_test] PB_OPT_VIDEO_PLAY_ONE");
        AppLibVideoDec_Stop();
        APP_REMOVEFLAGS(app_hdmi_test.GFlags, APP_AFLAGS_BUSY);
        if (!APP_CHECKFLAGS(app_hdmi_test.GFlags, APP_AFLAGS_BUSY)) {
            /* The busy flag will be removed when the flow stop the video player. */
            /* To excute the functions that system block them when the Busy flag is enabled. */
            AppUtil_BusyCheck(0);
        }
        if (APP_CHECKFLAGS(app_hdmi_test.GFlags, APP_AFLAGS_READY)) {
            /* The system could switch the current app to other in the function "AppUtil_BusyCheck". */
            hdmi_test.Func(HDMI_TEST_SWITCH_APP, 0, 0);
        }
        break;
    }

    return ReturnValue;
}

static int hdmi_test_video_play_stop(void)
{
    int ReturnValue = 0;
    if (hdmi_test.CurFileType == HDMI_TEST_MEDIA_VIDEO) {
        AppLibComSvcTimer_Unregister(TIMER_10HZ, hdmi_test_play_timer_handler);
        hdmi_test.Gui(GUI_PLAY_TIMER_HIDE, 0, 0);
        hdmi_test.Gui(GUI_PLAY_STATE_HIDE, 0, 0);
        hdmi_test.Gui(GUI_PLAY_SPEED_HIDE, 0, 0);
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
static int hdmi_test_switch_app(int mode)
{
    int ReturnValue = 0;

    if ( app_status.LockDecMode && app_status.FchanDecModeOnly) {
        ReturnValue = AppUtil_SwitchApp(APP_THUMB_MOTION);
    } else if (mode == 0){
        ReturnValue = AppUtil_SwitchApp(app_hdmi_test.Previous);
    } else {
        ReturnValue = AppUtil_SwitchApp(APP_REC_CAM);
    }

    return ReturnValue;
}

int hdmi_test_card_removed(void)
{
    int ReturnValue = 0;

    if (APP_CHECKFLAGS(app_hdmi_test.GFlags, APP_AFLAGS_POPUP)) {
        APP_REMOVEFLAGS(app_hdmi_test.Flags, HDMI_TEST_DELETE_FILE_RUN);
        AppWidget_Off(WIDGET_ALL, 0);
    }
    hdmi_test.Func(HDMI_TEST_START_DISP_PAGE, 0, 0);
    hdmi_test.Func(HDMI_TEST_WARNING_MSG_SHOW_STOP, 0, 0);
    hdmi_test.Gui(GUI_CARD_UPDATE, GUI_NO_CARD, 0);
    hdmi_test.Gui(GUI_MEDIA_INFO_HIDE, 0, 0);
    hdmi_test.Gui(GUI_FILENAME_HIDE, 0, 0);
    hdmi_test.Gui(GUI_FLUSH, 0, 0);

    return ReturnValue;
}

int hdmi_test_card_error_removed(void)
{
    int ReturnValue = 0;

    APP_REMOVEFLAGS(app_hdmi_test.GFlags, APP_AFLAGS_BUSY);
    hdmi_test.Func(HDMI_TEST_CARD_REMOVED, 0, 0);

    return ReturnValue;
}

int hdmi_test_card_new_insert(int param1)
{
    int ReturnValue = 0;

    /* Remove old card.*/
    hdmi_test.Func(HDMI_TEST_CARD_REMOVED, 0, 0);
    AppLibCard_StatusSetBlock(param1, 0);
    AppLibComSvcAsyncOp_CardInsert(AppLibCard_GetSlot(param1));

    return ReturnValue;
}

int hdmi_test_card_storage_idle(void)
{
    int ReturnValue = 0;

    AppUtil_CheckCardParam(0);
    if (!APP_CHECKFLAGS(app_pb_photo.GFlags, APP_AFLAGS_READY)) {
        return ReturnValue;/**<  App switched out*/
    }

    hdmi_test.Gui(GUI_CARD_UPDATE, GUI_CARD_READY, 0);
    hdmi_test.Gui(GUI_FLUSH, 0, 0);
    hdmi_test.Func(HDMI_TEST_START_DISP_PAGE, 0, 0);

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
static int hdmi_test_dialog_del_handler(UINT32 sel, UINT32 param1, UINT32 param2)
{
    int ReturnValue = 0;

    switch (sel) {
    case DIALOG_SEL_YES:
        hdmi_test.Func(HDMI_TEST_DELETE_FILE, CUR_OBJ(hdmi_test.CurFileObjID), CUR_OBJ_IDX(hdmi_test.CurFileObjID));
        break;
    case DIALOG_SEL_NO:
    default:
        break;
    }

    return ReturnValue;
}

static int hdmi_test_delete_file_dialog_show(void)
{
    int ReturnValue = 0;

    ReturnValue = AppLibCard_CheckStatus(CARD_CHECK_DELETE);
    if (ReturnValue == 0) {
        AppDialog_SetDialog(DIALOG_TYPE_Y_N, DIALOG_SUB_DEL, hdmi_test_dialog_del_handler);
        AppWidget_On(WIDGET_DIALOG, 0);
        APP_ADDFLAGS(app_hdmi_test.GFlags, APP_AFLAGS_POPUP);
    } else if (ReturnValue == CARD_STATUS_WP_CARD) {
        hdmi_test.Func(HDMI_TEST_WARNING_MSG_SHOW_START, GUI_WARNING_CARD_PROTECTED, 0);
        AmbaPrintColor(RED,"[app_hdmi_test] WARNING_CARD_PROTECTED");
    } else {
        AmbaPrintColor(RED,"[app_hdmi_test] WARNING_CARD_Error rval = %d", ReturnValue);
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
static int hdmi_test_delete_file(UINT32 FileObjID)
{
    int ReturnValue = 0;

    if (hdmi_test.CurFileType == HDMI_TEST_MEDIA_VIDEO) {
        /* Stop player. */
        AppLibVideoDec_Stop();
    }

    AppLibComSvcAsyncOp_DmfFdel(hdmi_test.FileInfo.MediaRoot, FileObjID, app_status.PlaybackType);

    APP_ADDFLAGS(app_hdmi_test.Flags, HDMI_TEST_DELETE_FILE_RUN);
    APP_ADDFLAGS(app_hdmi_test.GFlags, APP_AFLAGS_BUSY);
    hdmi_test.Func(HDMI_TEST_WARNING_MSG_SHOW_START, GUI_WARNING_PROCESSING, 1);

    return ReturnValue;
}

static int hdmi_test_delete_file_complete(int param1, int param2)
{
    int ReturnValue = 0;

    if (APP_CHECKFLAGS(app_hdmi_test.Flags, HDMI_TEST_DELETE_FILE_RUN)) {
        APP_REMOVEFLAGS(app_hdmi_test.Flags, HDMI_TEST_DELETE_FILE_RUN);
        hdmi_test.Gui(GUI_WARNING_HIDE, 0, 0);
        hdmi_test.Gui(GUI_FLUSH, 0, 0);
        APP_REMOVEFLAGS(app_hdmi_test.GFlags, APP_AFLAGS_BUSY);
        ReturnValue = (int)param1;
        if (ReturnValue < 0) {
            AmbaPrintColor(RED,"[app_hdmi_test] Delete files failed: %d", param2);
        } else {
            //hdmi_test.Gui(GUI_ZOOM_RATIO_HIDE, 0, 0);
            //hdmi_test.Gui(GUI_FLUSH, 0, 0);
            /** page update */
            app_status.ThumbnailModeConti = 0;
            hdmi_test.Func(HDMI_TEST_START_DISP_PAGE, 0, 0);
        }
    }
    return ReturnValue;
}

static int hdmi_test_widget_closed(int param1, int param2)
{
    int ReturnValue = 0;

    if (APP_CHECKFLAGS(app_hdmi_test.GFlags, APP_AFLAGS_POPUP)) {
        APP_REMOVEFLAGS(app_hdmi_test.GFlags, APP_AFLAGS_POPUP);
    }

    return ReturnValue;
}

static int hdmi_test_set_system_type(void)
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
static int hdmi_test_update_fchan_vout(UINT32 msg)
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
        AmbaPrint("[app_hdmi_test] Vout no changed");
        return 0;
        break;
    }

    ReturnValue = AppLibDisp_SelectDevice(DISP_CH_FCHAN, DISP_ANY_DEV);
    if (APP_CHECKFLAGS(ReturnValue, DISP_FCHAN_NO_CHANGE)) {
        AmbaPrint("[app_hdmi_test] Display FCHAN has no changed");
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
                    DBGMSG("[app_hdmi_test] FChan Disable. Disable the fchan window");
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
            hdmi_test.Gui(GUI_SET_LAYOUT, 0, 0);
            hdmi_test.Gui(GUI_FLUSH, 0, 0);
        }
        hdmi_test.CurPlayTime = 0;
        AppLibVideoDec_Init();
        AppLibStillSingle_Init();
        hdmi_test.Func(HDMI_TEST_START_DISP_PAGE, 1, 0);
    }

    return ReturnValue;
}

/**
 *  @brief To change the vout of Fchan, mode and pixel format
 *
 *  To change the vout of Fchan, mode and pixel format
 *
 *  @param [in] msg Message ID
 *
 *  @return >=0 success, <0 failure
 */
static int hdmi_test_update_fchan_fixed_vout(UINT32 param1, UINT32 param2)
{
    int ReturnValue = 0;

    ReturnValue = AppLibDisp_SelectDevice(DISP_CH_FCHAN, DISP_ANY_DEV);
    if (0) {//(APP_CHECKFLAGS(ReturnValue, DISP_FCHAN_NO_CHANGE)) {
        AmbaPrint("[app_hdmi_test] Display FCHAN has no changed");
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
            AppLibDisp_ChanStop(DISP_CH_FCHAN);
            // param1 == 0: set the vout mode.
            // param1 == 1: set the pixel format.
            if (param1 == 0) {
                AppLibDisp_ConfigMode(DISP_CH_FCHAN, AppLibSysVout_GetVoutMode(param2));
            } else {
                AppLibDisp_SetupFchanPxlFmt(param2);
            }
            AppLibDisp_SetupChan(DISP_CH_FCHAN);
            AppLibDisp_ChanStart(DISP_CH_FCHAN);
            {
                AMP_DISP_WINDOW_CFG_s Window;
                AMP_DISP_INFO_s DispDev = {0};

                memset(&Window, 0, sizeof(AMP_DISP_WINDOW_CFG_s));

                ReturnValue = AppLibDisp_GetDeviceInfo(DISP_CH_FCHAN, &DispDev);
                if ((ReturnValue < 0) || (DispDev.DeviceInfo.Enable == 0)) {
                    DBGMSG("[app_hdmi_test] FChan Disable. Disable the fchan window");
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
            hdmi_test.Gui(GUI_SET_LAYOUT, 0, 0);
            hdmi_test.Gui(GUI_FLUSH, 0, 0);
        }
        hdmi_test.CurPlayTime = 0;
        AppLibVideoDec_Init();
        AppLibStillSingle_Init();
        hdmi_test.Func(HDMI_TEST_START_DISP_PAGE, 1, 0);
    }

    return ReturnValue;
}

int hdmi_test_change_display(void)
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
            DBGMSG("[app_hdmi_test] FChan Disable. Disable the fchan Window");
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
            DBGMSG("[app_hdmi_test] DChan Disable. Disable the Dchan Window");
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

int hdmi_test_change_osd(void)
{
    int ReturnValue = 0;

    /* Update graphic window*/
    AppLibGraph_SetWindowConfig(GRAPH_CH_FCHAN | GRAPH_CH_DCHAN);
    AppLibGraph_ActivateWindow(GRAPH_CH_FCHAN | GRAPH_CH_DCHAN);
    AppLibGraph_FlushWindow(GRAPH_CH_FCHAN | GRAPH_CH_DCHAN);
    hdmi_test.Gui(GUI_SET_LAYOUT, 0, 0);
    hdmi_test.Gui(GUI_FLUSH, 0, 0);

    return ReturnValue;
}

static int hdmi_test_usb_connect(void)
{
    int ReturnValue = 0;

    switch (UserSetting->SetupPref.USBMode) {
    default:
        AppUtil_SwitchApp(APP_USB_MSC);
        break;
    }

    return ReturnValue;
}

static int hdmi_test_start_show_gui(void)
{
    int ReturnValue = 0;
    int GuiParam = 0;

    hdmi_test.Gui(GUI_APP_ICON_SHOW, 0, 0);
    hdmi_test.Gui(GUI_POWER_STATE_UPDATE, app_status.PowerType, app_status.BatteryState);
    hdmi_test.Gui(GUI_POWER_STATE_SHOW, app_status.PowerType, app_status.BatteryState);
    ReturnValue = AppLibCard_CheckStatus(0);
    if (ReturnValue == CARD_STATUS_NO_CARD) {
        GuiParam = GUI_NO_CARD;
    } else {
        GuiParam = GUI_CARD_READY;
    }
    hdmi_test.Gui(GUI_CARD_UPDATE, GuiParam, 0);
    hdmi_test.Gui(GUI_CARD_SHOW, 0, 0);
    /**video playback need to display timeline gui*/
    if (hdmi_test.CurFileType== HDMI_TEST_MEDIA_VIDEO) {
        hdmi_test.Gui(GUI_PLAY_STATE_UPDATE, GUI_PAUSE, 0);
        hdmi_test.Gui(GUI_PLAY_TIMER_UPDATE, 0, hdmi_test.MediaInfo.TotalTime/1000);
        hdmi_test.Gui(GUI_PLAY_STATE_SHOW, 0, 0);
        hdmi_test.Gui(GUI_PLAY_SPEED_SHOW, 0, 0);
    }
    hdmi_test.Gui(GUI_MEDIA_INFO_HIDE, 0, 0);
    hdmi_test.Gui(GUI_FILENAME_HIDE, 0, 0);
    hdmi_test.Gui(GUI_FLUSH, 0, 0);

    return ReturnValue;
}


static int hdmi_test_update_bat_power_status(int param1)
{
    int ReturnValue = 0;

    /* Update the gui of power's status. */
    if (param1 == 0) {
        /*Hide the battery gui.*/
        hdmi_test.Gui(GUI_POWER_STATE_HIDE, GUI_HIDE_POWER_EXCEPT_DC, 0);
    } else if (param1 == 1) {
        /*Update the battery gui.*/
        hdmi_test.Gui(GUI_POWER_STATE_UPDATE, app_status.PowerType, app_status.BatteryState);
        hdmi_test.Gui(GUI_POWER_STATE_SHOW, app_status.PowerType, app_status.BatteryState);
    } else if (param1 == 2) {
        /*Reset the battery and power gui.*/
        hdmi_test.Gui(GUI_POWER_STATE_HIDE, 0, 0);
        hdmi_test.Gui(GUI_POWER_STATE_UPDATE, app_status.PowerType, app_status.BatteryState);
        hdmi_test.Gui(GUI_POWER_STATE_SHOW, app_status.PowerType, app_status.BatteryState);
    }
    hdmi_test.Gui(GUI_FLUSH, 0, 0);

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
static void hdmi_test_warning_timer_handler(int eid)
{
    static int BlinkCount;

    if (eid == TIMER_UNREGISTER) {
        BlinkCount = 0;
        return;
    }

    BlinkCount++;

    if (BlinkCount & 0x01) {
        hdmi_test.Gui(GUI_WARNING_HIDE, 0, 0);
    } else {
        hdmi_test.Gui(GUI_WARNING_SHOW, 0, 0);
    }

    if (BlinkCount >= 5) {
        APP_REMOVEFLAGS(app_hdmi_test.Flags, HDMI_TEST_WARNING_MSG_RUN);
        AppLibComSvcTimer_Unregister(TIMER_2HZ, hdmi_test_warning_timer_handler);
        hdmi_test.Gui(GUI_WARNING_HIDE, 0, 0);
    }
    hdmi_test.Gui(GUI_FLUSH, 0, 0);

}

static int hdmi_test_warning_msg_show(int enable, int param1, int param2)
{
    int ReturnValue = 0;

    if (enable) {
        if (param2) {
            hdmi_test.Gui(GUI_WARNING_UPDATE, param1, 0);
            hdmi_test.Gui(GUI_WARNING_SHOW, 0, 0);
        } else {
            if (!APP_CHECKFLAGS(app_hdmi_test.Flags, HDMI_TEST_WARNING_MSG_RUN)) {
                APP_ADDFLAGS(app_hdmi_test.Flags, HDMI_TEST_WARNING_MSG_RUN);
                hdmi_test.Gui(GUI_WARNING_UPDATE, param1, 0);
                hdmi_test.Gui(GUI_WARNING_SHOW, 0, 0);
                AppLibComSvcTimer_Register(TIMER_2HZ, hdmi_test_warning_timer_handler);
            }
        }
    } else {
        if (APP_CHECKFLAGS(app_hdmi_test.Flags, HDMI_TEST_WARNING_MSG_RUN)) {
            APP_REMOVEFLAGS(app_hdmi_test.Flags, HDMI_TEST_WARNING_MSG_RUN);
            AppLibComSvcTimer_Unregister(TIMER_2HZ, hdmi_test_warning_timer_handler);
        }
        hdmi_test.Gui(GUI_WARNING_HIDE, 0, 0);
    }
    hdmi_test.Gui(GUI_FLUSH, 0, 0);

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
int hdmi_test_func(UINT32 funcId, UINT32 param1, UINT32 param2)
{
    int ReturnValue = 0;

    switch (funcId) {
    case HDMI_TEST_INIT:
        ReturnValue = hdmi_test_init();
        break;
    case HDMI_TEST_START:
        ReturnValue = hdmi_test_start();
        break;
    case HDMI_TEST_STOP:
        ReturnValue = hdmi_test_stop();
        break;
    case HDMI_TEST_APP_READY:
        ReturnValue = hdmi_test_app_ready();
        break;
    case HDMI_TEST_START_DISP_PAGE:
        ReturnValue = hdmi_test_start_disp_page(param1);
        break;
    case HDMI_TEST_OPEN:
        ReturnValue = hdmi_test_open(param1);
        break;
    case HDMI_TEST_PLAY:
        if (param2) {
            ReturnValue = hdmi_test_video_open_play_curr(param1);
        } else {
            ReturnValue = hdmi_test_photo_open_play_curr(param1);
        }
        break;
    case HDMI_TEST_EOS:
        ReturnValue = hdmi_test_do_play_eos();
        break;
    case HDMI_TEST_STOP_PLAYING:
        ReturnValue = hdmi_test_video_play_stop();
        break;
    case HDMI_TEST_SWITCH_APP:
        ReturnValue = hdmi_test_switch_app(param1);
        break;
    case HDMI_TEST_GET_FILE:
        ReturnValue = hdmi_test_get_file(param1, param2);
        break;
    case HDMI_TEST_CARD_ERROR_REMOVED:
        ReturnValue = hdmi_test_card_error_removed();
        break;
    case HDMI_TEST_CARD_REMOVED:
        ReturnValue = hdmi_test_card_removed();
        break;
    case HDMI_TEST_CARD_NEW_INSERT:
        ReturnValue = hdmi_test_card_new_insert(param1);
        break;
    case HDMI_TEST_CARD_STORAGE_IDLE:
        ReturnValue = hdmi_test_card_storage_idle();
        break;
    case HDMI_TEST_DELETE_FILE_DIALOG_SHOW:
        ReturnValue = hdmi_test_delete_file_dialog_show();
        break;
    case HDMI_TEST_DELETE_FILE:
        ReturnValue = hdmi_test_delete_file(param1);
        break;
    case HDMI_TEST_DELETE_FILE_COMPLETE:
        ReturnValue = hdmi_test_delete_file_complete(param1, param2);
        break;
    case HDMI_TEST_STATE_WIDGET_CLOSED:
        ReturnValue = hdmi_test_widget_closed(param1, param2);
        break;
    case HDMI_TEST_SET_SYSTEM_TYPE:
        ReturnValue = hdmi_test_set_system_type();
        break;
    case HDMI_TEST_UPDATE_FCHAN_VOUT:
        ReturnValue = hdmi_test_update_fchan_vout(param1);
        break;
    case HDMI_TEST_UPDATE_FCHAN_FIXED_VOUT:
        ReturnValue = hdmi_test_update_fchan_fixed_vout(param1, param2);
        break;
    case HDMI_TEST_UPDATE_DCHAN_VOUT:
        break;
    case HDMI_TEST_CHANGE_DISPLAY:
        ReturnValue = hdmi_test_change_display();
        break;
    case HDMI_TEST_CHANGE_OSD:
        ReturnValue = hdmi_test_change_osd();
        break;
    case HDMI_TEST_USB_CONNECT:
        ReturnValue = hdmi_test_usb_connect();
        break;
    case HDMI_TEST_GUI_INIT_SHOW:
        ReturnValue = hdmi_test_start_show_gui();
        break;
    case HDMI_TEST_UPDATE_BAT_POWER_STATUS:
        ReturnValue = hdmi_test_update_bat_power_status(param1);
        break;
    case HDMI_TEST_WARNING_MSG_SHOW_START:
        ReturnValue = hdmi_test_warning_msg_show(1, param1, param2);
        break;
    case HDMI_TEST_WARNING_MSG_SHOW_STOP:
        ReturnValue = hdmi_test_warning_msg_show(0, param1, param2);
        break;
    default:
        break;
    }

    return ReturnValue;
}
