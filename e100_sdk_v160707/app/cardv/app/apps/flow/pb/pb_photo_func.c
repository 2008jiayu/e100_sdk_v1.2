/**
  * @file src/app/apps/flow/pb/connectedcam/pb_photo_func.c
  *
  *  Functions of photo playback application
  *
  * History:
  *    2013/11/08 - [Martin Lai] created file
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


#include <apps/flow/pb/pb_photo.h>
#include <system/status.h>
#include <system/app_pref.h>
#include <system/app_util.h>
#include <apps/gui/resource/gui_settle.h>
#include <apps/flow/widget/menu/widget.h>
#include <AmbaUtility.h>

static int pb_photo_init(void)
{
    int ReturnValue = 0;

    pb_photo.FileInfo.MediaRoot = APPLIB_DCF_MEDIA_IMAGE;

    return ReturnValue;
}

static int pb_photo_start(void)
{
    int ReturnValue = 0;

    UserSetting->SystemPref.SystemMode = APP_MODE_DEC;

    /** Initialize the photo decoder. */
    AppLibFormat_DemuxerInit();
    AppLibStillSingle_Init();

    /** Set menus */
    AppMenu_Reset();
    AppMenu_RegisterTab(MENU_SETUP);
    AppMenu_RegisterTab(MENU_PBACK);

    return ReturnValue;
}

static int pb_photo_stop(void)
{
    int ReturnValue = 0;

    /* Close the photo player. */
    AppLibStillSingle_Deinit();

    /* Close the menu or dialog. */
    AppWidget_Off(WIDGET_ALL, WIDGET_HIDE_SILENT);
    APP_REMOVEFLAGS(app_pb_photo.GFlags, APP_AFLAGS_POPUP);

    /* Disable the vout. */
    AppLibDisp_DeactivateWindow(DISP_CH_FCHAN, AppLibDisp_GetWindowId(DISP_CH_FCHAN, 0));
    AppLibDisp_DeactivateWindow(DISP_CH_DCHAN, AppLibDisp_GetWindowId(DISP_CH_DCHAN, 0));
    AppLibDisp_ChanStop(DISP_CH_FCHAN);

    /* Hide GUI */
    pb_photo.Gui(GUI_HIDE_ALL, 0, 0);
    pb_photo.Gui(GUI_FLUSH, 0, 0);

    return ReturnValue;
}

static int pb_photo_app_ready(void)
{
    int ReturnValue = 0;

    if (!APP_CHECKFLAGS(app_pb_photo.GFlags, APP_AFLAGS_READY)) {
        APP_ADDFLAGS(app_pb_photo.GFlags, APP_AFLAGS_READY);

        // ToDo: need to remove to handler when iav completes the dsp cmd queue mechanism
        AppLibGraph_Init();
        pb_photo.Func(PB_PHOTO_CHANGE_OSD, 0, 0);

        AppUtil_ReadyCheck(0);
        if (!APP_CHECKFLAGS(app_pb_photo.GFlags, APP_AFLAGS_READY)) {
            /* The system could switch the current app to other in the function "AppUtil_ReadyCheck". */
            return ReturnValue;
        }
    }

    if (APP_CHECKFLAGS(app_pb_photo.GFlags, APP_AFLAGS_READY)) {
        pb_photo.Func(PB_PHOTO_START_DISP_PAGE, 0, 0);
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
static int pb_photo_init_file_info(void)
{
    UINT64 ReturnValue = 0;

    ReturnValue = AppLibCard_CheckStatus(0);

    if (ReturnValue == 0) {
        pb_photo.FileInfo.TotalFileNum = AppLibStorageDmf_GetFileAmount(pb_photo.FileInfo.MediaRoot, DCIM_HDLR);
    } else {
        pb_photo.FileInfo.TotalFileNum = 0;
    }
    AmbaPrintColor(GREEN, "[app_pb_photo] pb_photo.FileInfo.TotalFileNum: %d", pb_photo.FileInfo.TotalFileNum);
    if (pb_photo.FileInfo.TotalFileNum > 0) {
        int i = 0;
        UINT64 CurrFilePos = AppLibStorageDmf_GetCurrFilePos(pb_photo.FileInfo.MediaRoot, DCIM_HDLR);
        if (CurrFilePos == 0) {
            CurrFilePos = AppLibStorageDmf_GetLastFilePos(pb_photo.FileInfo.MediaRoot, DCIM_HDLR);
        }
        AppLibStorageDmf_GetFileName(pb_photo.FileInfo.MediaRoot,".JPG", APPLIB_DCF_EXT_OBJECT_SPLIT_FILE, DCIM_HDLR, (CUR_OBJ_IDX(CurrFilePos) + 1), CUR_OBJ(CurrFilePos), pb_photo.CurFn);
        pb_photo.CurFileObjID = CurrFilePos;
        ReturnValue = AppLibStorageDmf_GetLastFilePos(pb_photo.FileInfo.MediaRoot, DCIM_HDLR);
        for (i = (pb_photo.FileInfo.TotalFileNum - 1) ; i >= 0; i--) {
            if (ReturnValue == CurrFilePos) {
                pb_photo.FileInfo.FileCur = i;
                break;
            } else {
                ReturnValue = AppLibStorageDmf_GetPrevFilePos(pb_photo.FileInfo.MediaRoot, DCIM_HDLR);
            }
        }
        AmbaPrintColor(GREEN, "[app_pb_photo] pb_photo.FileInfo.FileCur: %d, CurrFilePos =%lld", pb_photo.FileInfo.FileCur,CurrFilePos);
    } else {
        pb_photo.FileInfo.TotalFileNum = 0;
        pb_photo.FileInfo.FileCur = 0;
        pb_photo.CurFileObjID = 0;
        memset(pb_photo.CurFn, 0, MAX_FILENAME_LENGTH*sizeof(char));
    }

    return CUR_OBJ(ReturnValue);
}


/**
 *  @brief Start to show the image.
 *
 *  Start to show the image.
 *
 *  @return >=0 success, <0 failure
 */
static int pb_photo_start_disp_page(void)
{
    int ReturnValue = 0;

    ReturnValue = pb_photo_init_file_info();
    if (pb_photo.FileInfo.TotalFileNum > 0) {
        pb_photo.Func(PB_PHOTO_GUI_INIT_SHOW, 0, 0);
        pb_photo.Func(PB_PHOTO_GET_FILE, GET_CURR_FILE, 0);
        pb_photo.Func(PB_PHOTO_PLAY, 0, 0);
    } else {
        pb_photo.Func(PB_PHOTO_SWITCH_APP, 0, 0);
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
static int pb_photo_get_file(UINT32 param)
{
    int ReturnValue = 0;
    int i = 0;
    char TempFn[MAX_FILENAME_LENGTH] = {0};
    APPLIB_MEDIA_INFO_s MediaInfo;
    UINT64 CurrFilePos = AppLibStorageDmf_GetCurrFilePos(pb_photo.FileInfo.MediaRoot, DCIM_HDLR);
    AmbaPrintColor(GREEN, "[app_pb_photo] pb_photo.FileInfo.FileCur: %d, CurrFilePos =%d", pb_photo.FileInfo.FileCur,CurrFilePos);

    for (i=0; i<pb_photo.FileInfo.TotalFileNum; i++) {
        switch (param) {
        case GET_PREV_FILE:
            if (pb_photo.FileInfo.FileCur == 0) {
                pb_photo.FileInfo.FileCur = pb_photo.FileInfo.TotalFileNum - 1;
                AppLibStorageDmf_GetLastFilePos(pb_photo.FileInfo.MediaRoot, DCIM_HDLR);
            } else {
                pb_photo.FileInfo.FileCur --;
                AppLibStorageDmf_GetPrevFilePos(pb_photo.FileInfo.MediaRoot, DCIM_HDLR);
            }
            break;
        case GET_CURR_FILE:
           AppLibStorageDmf_GetCurrFilePos(pb_photo.FileInfo.MediaRoot, DCIM_HDLR);
            break;
        case GET_NEXT_FILE:
        default:
            if (pb_photo.FileInfo.FileCur == (pb_photo.FileInfo.TotalFileNum - 1)) {
                pb_photo.FileInfo.FileCur = 0;
                AppLibStorageDmf_GetFirstFilePos(pb_photo.FileInfo.MediaRoot, DCIM_HDLR);
            } else {
                pb_photo.FileInfo.FileCur ++;
                AppLibStorageDmf_GetNextFilePos(pb_photo.FileInfo.MediaRoot, DCIM_HDLR);
            }
            break;
        }
        CurrFilePos = AppLibStorageDmf_GetCurrFilePos(pb_photo.FileInfo.MediaRoot, DCIM_HDLR);
        AppLibStorageDmf_GetFileName(pb_photo.FileInfo.MediaRoot, ".JPG", APPLIB_DCF_EXT_OBJECT_SPLIT_FILE, DCIM_HDLR, (CUR_OBJ_IDX(CurrFilePos) + 1), CUR_OBJ(CurrFilePos), TempFn);
        ReturnValue = AppLibFormat_GetMediaInfo(TempFn, &MediaInfo);
        if ((ReturnValue == AMP_OK) && (MediaInfo.MediaInfoType == AMP_MEDIA_INFO_IMAGE)) {
            strcpy(pb_photo.CurFn, TempFn);
            pb_photo.CurFileObjID = CurrFilePos;
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
static int pb_photo_config_media_info(char *fn)//, dec_media_info_t *info)
{
    int ReturnValue = 0;
#if 0
    /// Dimension information
    pb_photo.MediaInfo.Width = info->media.img.width;
    pb_photo.MediaInfo.Height = info->media.img.height;
    /// ISO information
    pb_photo.MediaInfo.Iso = info->media.img.exif_info->iso_speed_rating;
    /// PB zoom status
    pb_photo.MediaInfo.Zoom = PB_PHOTO_IZOOM_MIN;
    pb_photo.MediaInfo.ZoomStep = info->media.img.height/32;
    pb_photo.MediaInfo.Zoom_step = (pb_photo.MediaInfo.Zoom_step+3)&0xFFFFFFFC;    ///< 4 align
    APP_REMOVEFLAGS(app_pb_photo.Flags, PB_PHOTO_PLAYBACK_ZOOM);
//#define PHOTO_AR_DEFAULT    (0x100)
//    app_pback_photo_set_ar(PHOTO_AR_DEFAULT);
    /// Orientation status
    switch (info->media.img.orientation) {
    case JPEG_ROTATE_90_DEGREE:
        pb_photo.MediaInfo.Rotate = ROTATE_90;
        break;
    case JPEG_ROTATE_180_DEGREE:
        pb_photo.MediaInfo.Rotate = ROTATE_180;
        break;
    case JPEG_ROTATE_270_DEGREE:
        pb_photo.MediaInfo.Rotate = ROTATE_270;
        break;
    case JPEG_ROTATE_0_DEGREE:
    default:
        pb_photo.MediaInfo.Rotate = ROTATE_0;
        break;
    }
    pb_photo.MediaInfo.RotateOri = pb_photo.MediaInfo.Rotate;
    APP_REMOVEFLAGS(app_pb_photo.Flags, PB_PHOTO_PLAYBACK_ROTATE);

    {
        char ft[64];
        uni_to_asc(pb_photo.CurFn, ft);
        AmbaPrint("Config current file: %s", ft);
    }
#endif
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
static int pb_photo_open_play_curr(UINT32 param)
{
    int ReturnValue = 0;
    WCHAR FileName[MAX_FILENAME_LENGTH];
    pb_photo_config_media_info(pb_photo.CurFn);
    {
        APPLIB_STILL_FILE_s StillFile = {0};
        memset(&StillFile, 0, sizeof(APPLIB_STILL_FILE_s)); // Initialize StillInfo
        memcpy(StillFile.Filename, pb_photo.CurFn, sizeof(char)*MAX_FILENAME_LENGTH); // Copy filename
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
        StillInfo.ImageRotate = (AMP_ROTATION_e)pb_photo.MediaInfo.Rotate; // No rotation and flip

        // Show image
        AppLibStillSingle_Show(&StillInfo);
        AmbaUtility_Ascii2Unicode(pb_photo.CurFn, FileName);
        pb_photo.Gui(GUI_FILENAME_UPDATE, (UINT32)FileName, GUI_PB_FN_STYLE_HYPHEN);
        pb_photo.Gui(GUI_FILENAME_SHOW, 0, 0);
        pb_photo.Gui(GUI_FLUSH, 0, 0);
    }
    return ReturnValue;
}

/**
 * @brief The applications switching function.
 *
 * @return >=0 success
 *         <0 failure
 */
static int pb_photo_switch_app(void)
{
    int ReturnValue = 0;

    if ( app_status.LockDecMode && app_status.FchanDecModeOnly) {
        ReturnValue = AppUtil_SwitchApp(APP_THUMB_MOTION);
    } else {
        ReturnValue = AppUtil_SwitchApp(app_pb_photo.Previous);
    }

    return ReturnValue;
}

int pb_photo_card_storage_idle(void)
{
    int ReturnValue = 0;

    pb_photo.Func(PB_PHOTO_SET_FILE_INDEX, 0, 0);

    AppUtil_CheckCardParam(0);

    if (!APP_CHECKFLAGS(app_pb_photo.GFlags, APP_AFLAGS_READY)) {
        return ReturnValue;/**<  App switched out*/
    }

    pb_photo.Gui(GUI_CARD_UPDATE, GUI_CARD_READY, 0);
    pb_photo.Gui(GUI_FLUSH, 0, 0);
    pb_photo.Func(PB_PHOTO_START_DISP_PAGE, 0, 0);

    return ReturnValue;
}

int pb_photo_card_new_insert(int param1)
{
    int ReturnValue = 0;

    /* Remove old card.*/
    pb_photo.Func(PB_PHOTO_CARD_REMOVED, 0, 0);
    AppLibCard_StatusSetBlock(param1, 0);
    AppLibComSvcAsyncOp_CardInsert(AppLibCard_GetSlot(param1));

    return ReturnValue;
}

int pb_photo_card_removed(void)
{
    int ReturnValue = 0;

    if (APP_CHECKFLAGS(app_pb_photo.GFlags, APP_AFLAGS_POPUP)) {
        APP_REMOVEFLAGS(app_pb_photo.Flags, PB_PHOTO_DELETE_FILE_RUN);
        AppWidget_Off(WIDGET_ALL, 0);
    }
    pb_photo.Func(PB_PHOTO_START_DISP_PAGE, 0, 0);
    pb_photo.Func(PB_PHOTO_WARNING_MSG_SHOW_STOP, 0, 0);
    pb_photo.Gui(GUI_CARD_UPDATE, GUI_NO_CARD, 0);
    pb_photo.Gui(GUI_MEDIA_INFO_HIDE, 0, 0);
    pb_photo.Gui(GUI_FILENAME_HIDE, 0, 0);
    pb_photo.Gui(GUI_FLUSH, 0, 0);

    return ReturnValue;
}

int pb_photo_card_error_removed(void)
{
    int ReturnValue = 0;

    APP_REMOVEFLAGS(app_pb_photo.GFlags, APP_AFLAGS_BUSY);
    pb_photo.Func(PB_PHOTO_CARD_REMOVED, 0, 0);

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
static int pb_photo_dialog_del_handler(UINT32 sel, UINT32 param1, UINT32 param2)
{
    int ReturnValue = 0;

    switch (sel) {
    case DIALOG_SEL_YES:
        pb_photo.Func(PB_PHOTO_DELETE_FILE, CUR_OBJ(pb_photo.CurFileObjID), CUR_OBJ_IDX(pb_photo.CurFileObjID));
        break;
    case DIALOG_SEL_NO:
    default:
        break;
    }

    return ReturnValue;
}

static int pb_photo_delete_file_dialog_show(void)
{
    int ReturnValue = 0;

    ReturnValue = AppLibCard_CheckStatus(CARD_CHECK_DELETE);
    if (ReturnValue == 0) {
        AppDialog_SetDialog(DIALOG_TYPE_Y_N, DIALOG_SUB_DEL, pb_photo_dialog_del_handler);
        AppWidget_On(WIDGET_DIALOG, 0);
        APP_ADDFLAGS(app_pb_photo.GFlags, APP_AFLAGS_POPUP);
    } else if (ReturnValue == CARD_STATUS_WP_CARD) {
        pb_photo.Func(PB_PHOTO_WARNING_MSG_SHOW_START, GUI_WARNING_CARD_PROTECTED, 0);
        AmbaPrintColor(RED,"[app_pb_photo] WARNING_CARD_PROTECTED");
    } else {
        AmbaPrintColor(RED,"[app_pb_photo] WARNING_CARD_Error rval = %d", ReturnValue);
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
static int pb_photo_delete_file(UINT32 FileObjID)
{
    int ReturnValue = 0;

    AppLibComSvcAsyncOp_DmfFdel(pb_photo.FileInfo.MediaRoot, FileObjID, app_status.PlaybackType);

    APP_ADDFLAGS(app_pb_photo.Flags, PB_PHOTO_DELETE_FILE_RUN);
    APP_ADDFLAGS(app_pb_photo.GFlags, APP_AFLAGS_BUSY);
    pb_photo.Func(PB_PHOTO_WARNING_MSG_SHOW_START, GUI_WARNING_PROCESSING, 1);

    return ReturnValue;
}

static int ph_photo_delete_file_complete(int param1, int param2)
{
    int ReturnValue = 0;
    if (APP_CHECKFLAGS(app_pb_photo.Flags, PB_PHOTO_DELETE_FILE_RUN)) {
        APP_REMOVEFLAGS(app_pb_photo.Flags, PB_PHOTO_DELETE_FILE_RUN);
        APP_REMOVEFLAGS(app_pb_photo.GFlags, APP_AFLAGS_BUSY);
        pb_photo.Gui(GUI_WARNING_HIDE, 0, 0);
        pb_photo.Gui(GUI_FLUSH, 0, 0);
        ReturnValue = (int)param1;
        if (ReturnValue < 0) {
            AmbaPrintColor(RED,"[app_pb_photo] Delete files failed: %d", param2);
        } else {
            /** page update */
            //pb_photo.Gui(GUI_ZOOM_RATIO_HIDE, 0, 0);
            //pb_photo.Gui(GUI_FLUSH, 0, 0);
            app_status.ThumbnailModeConti = 0;

            pb_photo.Func(PB_PHOTO_START_DISP_PAGE, 0, 0);
        }
    }
    return ReturnValue;
}

static int pb_photo_widget_closed(void)
{
    int ReturnValue = 0;

    if (APP_CHECKFLAGS(app_pb_photo.GFlags, APP_AFLAGS_POPUP)) {
        APP_REMOVEFLAGS(app_pb_photo.GFlags, APP_AFLAGS_POPUP);
    }

    return ReturnValue;
}

static int pb_photo_set_system_type(void)
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
static int pb_photo_update_fchan_vout(UINT32 msg)
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
        AmbaPrint("[app_pb_photo] Vout no changed");
        return 0;
        break;
    }
    ReturnValue = AppLibDisp_SelectDevice(DISP_CH_FCHAN, DISP_ANY_DEV);
    if (APP_CHECKFLAGS(ReturnValue, DISP_FCHAN_NO_CHANGE)) {
        AmbaPrint("[app_pb_photo] Display FCHAN has no changed");
    } else {
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
                    DBGMSG("[app_pb_photo] FChan Disable. Disable the fchan window");
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
            pb_photo.Gui(GUI_SET_LAYOUT, 0, 0);
            pb_photo.Gui(GUI_FLUSH, 0, 0);
        }

        AppLibStillSingle_Init();
        pb_photo.Func(PB_PHOTO_START_DISP_PAGE, 0, 0);
    }

    return ReturnValue;
}

int pb_photo_change_display(void)
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
            DBGMSG("[app_pb_photo] FChan Disable. Disable the fchan window");
            AppLibDisp_DeactivateWindow(DISP_CH_FCHAN, AppLibDisp_GetWindowId(DISP_CH_FCHAN, 0));
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
        }

        ReturnValue = AppLibDisp_GetDeviceInfo(DISP_CH_DCHAN, &DispDev);
        if ((ReturnValue < 0) || (DispDev.DeviceInfo.Enable == 0)) {
            DBGMSG("[app_pb_photo] DChan Disable. Disable the Dchan window");
            AppLibDisp_DeactivateWindow(DISP_CH_DCHAN, AppLibDisp_GetWindowId(DISP_CH_DCHAN, 0));
        } else {
            /** DCHAN window*/
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

int pb_photo_change_osd(void)
{
    int ReturnValue = 0;

    /* Update graphic window*/
    AppLibGraph_SetWindowConfig(GRAPH_CH_FCHAN | GRAPH_CH_DCHAN);
    AppLibGraph_ActivateWindow(GRAPH_CH_FCHAN | GRAPH_CH_DCHAN);
    AppLibGraph_FlushWindow(GRAPH_CH_FCHAN | GRAPH_CH_DCHAN);

    pb_photo.Gui(GUI_SET_LAYOUT, 0, 0);
    pb_photo.Gui(GUI_FLUSH, 0, 0);

    return ReturnValue;
}

int pb_photo_usb_connect(void)
{
    int ReturnValue = 0;

    switch (UserSetting->SetupPref.USBMode) {
    default:
        AppUtil_SwitchApp(APP_USB_MSC);
        break;
    }

    return ReturnValue;
}

/**
 *  @brief Show gui of app
 *
 *  Show gui of app
 *
 *  @return >=0 success, <0 failure
 */
static int pb_photo_start_show_gui(void)
{
    int ReturnValue = 0;
    int GuiParam = 0;

    pb_photo.Gui(GUI_APP_ICON_HIDE, 0, 0);
    pb_photo.Gui(GUI_POWER_STATE_UPDATE, app_status.PowerType, app_status.BatteryState);
    pb_photo.Gui(GUI_POWER_STATE_SHOW, app_status.PowerType, app_status.BatteryState);
    ReturnValue = AppLibCard_CheckStatus(0);
    if (ReturnValue == CARD_STATUS_NO_CARD) {
        GuiParam = GUI_NO_CARD;
    } else {
        GuiParam = GUI_CARD_READY;
    }
    pb_photo.Gui(GUI_CARD_UPDATE, GuiParam, 0);
    pb_photo.Gui(GUI_CARD_SHOW, 0, 0);
    pb_photo.Gui(GUI_MEDIA_INFO_HIDE, 0, 0);
    pb_photo.Gui(GUI_FILENAME_HIDE, 0, 0);
    pb_photo.Gui(GUI_FLUSH, 0, 0);
    return ReturnValue;
}


static int pb_photo_update_bat_power_status(int param1)
{
    int ReturnValue = 0;

    /* Update the gui of power's status. */
    if (param1 == 0) {
        /*Hide the battery gui.*/
        pb_photo.Gui(GUI_POWER_STATE_HIDE, GUI_HIDE_POWER_EXCEPT_DC, 0);
    } else if (param1 == 1) {
        /*Update the battery gui.*/
        pb_photo.Gui(GUI_POWER_STATE_UPDATE, app_status.PowerType, app_status.BatteryState);
        pb_photo.Gui(GUI_POWER_STATE_SHOW, app_status.PowerType, app_status.BatteryState);
    } else if (param1 == 2) {
        /*Reset the battery and power gui.*/
        pb_photo.Gui(GUI_POWER_STATE_HIDE, 0, 0);
        pb_photo.Gui(GUI_POWER_STATE_UPDATE, app_status.PowerType, app_status.BatteryState);
        pb_photo.Gui(GUI_POWER_STATE_SHOW, app_status.PowerType, app_status.BatteryState);
    }
    pb_photo.Gui(GUI_FLUSH, 0, 0);

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
static void pb_photo_warning_timer_handler(int eid)
{
    static int BlinkCount;

    if (eid == TIMER_UNREGISTER) {
        BlinkCount = 0;
        return;
    }

    BlinkCount++;

    if (BlinkCount & 0x01) {
        pb_photo.Gui(GUI_WARNING_HIDE, 0, 0);
    } else {
        pb_photo.Gui(GUI_WARNING_SHOW, 0, 0);
    }

    if (BlinkCount >= 5) {
        APP_REMOVEFLAGS(app_pb_photo.Flags, PB_PHOTO_WARNING_MSG_RUN);
        AppLibComSvcTimer_Unregister(TIMER_2HZ, pb_photo_warning_timer_handler);
        pb_photo.Gui(GUI_WARNING_HIDE, 0, 0);
    }
    pb_photo.Gui(GUI_FLUSH, 0, 0);

}

static int pb_photo_warning_msg_show(int enable, int param1, int param2)
{
    int ReturnValue = 0;

    if (enable) {
        if (param2) {
            pb_photo.Gui(GUI_WARNING_UPDATE, param1, 0);
            pb_photo.Gui(GUI_WARNING_SHOW, 0, 0);
        } else {
            if (!APP_CHECKFLAGS(app_pb_photo.Flags, PB_PHOTO_WARNING_MSG_RUN)) {
                APP_ADDFLAGS(app_pb_photo.Flags, PB_PHOTO_WARNING_MSG_RUN);
                pb_photo.Gui(GUI_WARNING_UPDATE, param1, 0);
                pb_photo.Gui(GUI_WARNING_SHOW, 0, 0);
                AppLibComSvcTimer_Register(TIMER_2HZ, pb_photo_warning_timer_handler);
            }
        }
    } else {
        if (APP_CHECKFLAGS(app_pb_photo.Flags, PB_PHOTO_WARNING_MSG_RUN)) {
            APP_REMOVEFLAGS(app_pb_photo.Flags, PB_PHOTO_WARNING_MSG_RUN);
            AppLibComSvcTimer_Unregister(TIMER_2HZ, pb_photo_warning_timer_handler);
            pb_photo.Gui(GUI_WARNING_HIDE, 0, 0);
        } else {
            pb_photo.Gui(GUI_WARNING_HIDE, 0, 0);
        }
    }
    pb_photo.Gui(GUI_FLUSH, 0, 0);

    return ReturnValue;
}

/**
 *  @brief The functions of application
 *
 *  The functions of application
 *
 *  @param[in] funcId Function id
 *  @param[in] param1 first parameter
 *  @param[in] param2 second parameter
 *
 *  @return >=0 success, <0 failure
 */
int pb_photo_func(UINT32 funcId, UINT32 param1, UINT32 param2)
{
    int ReturnValue = 0;

    switch (funcId) {
    case PB_PHOTO_INIT:
        ReturnValue = pb_photo_init();
        break;
    case PB_PHOTO_START:
        ReturnValue = pb_photo_start();
        break;
    case PB_PHOTO_STOP:
        ReturnValue = pb_photo_stop();
        break;
    case PB_PHOTO_APP_READY:
        ReturnValue = pb_photo_app_ready();
        break;
    case PB_PHOTO_START_DISP_PAGE:
        ReturnValue = pb_photo_start_disp_page();
        break;
    case PB_PHOTO_PLAY:
        ReturnValue = pb_photo_open_play_curr(param1);
        break;
    case PB_PHOTO_SWITCH_APP:
        ReturnValue = pb_photo_switch_app();
        break;
    case PB_PHOTO_GET_FILE:
        ReturnValue = pb_photo_get_file(param1);
        break;
    case PB_PHOTO_CARD_STORAGE_IDLE:
        ReturnValue = pb_photo_card_storage_idle();
        break;
    case PB_PHOTO_CARD_NEW_INSERT:
        ReturnValue = pb_photo_card_new_insert(param1);
        break;
    case PB_PHOTO_CARD_REMOVED:
        ReturnValue = pb_photo_card_removed();
        break;
    case PB_PHOTO_CARD_ERROR_REMOVED:
        ReturnValue = pb_photo_card_error_removed();
        break;
    case PB_PHOTO_DELETE_FILE_DIALOG_SHOW:
        ReturnValue = pb_photo_delete_file_dialog_show();
        break;
    case PB_PHOTO_DELETE_FILE:
        ReturnValue = pb_photo_delete_file(param1);
        break;
    case PB_PHOTO_DELETE_FILE_COMPLETE:
        ReturnValue = ph_photo_delete_file_complete(param1, param2);
        break;
    case PB_PHOTO_STATE_WIDGET_CLOSED:
        ReturnValue = pb_photo_widget_closed();
        break;
    case PB_PHOTO_SET_SYSTEM_TYPE:
        ReturnValue = pb_photo_set_system_type();
        break;
    case PB_PHOTO_UPDATE_FCHAN_VOUT:
        ReturnValue = pb_photo_update_fchan_vout(param1);
        break;
    case PB_PHOTO_UPDATE_DCHAN_VOUT:
        break;
    case PB_PHOTO_CHANGE_DISPLAY:
        ReturnValue = pb_photo_change_display();
        break;
    case PB_PHOTO_CHANGE_OSD:
        ReturnValue = pb_photo_change_osd();
        break;
    case PB_PHOTO_USB_CONNECT:
        ReturnValue = pb_photo_usb_connect();
        break;
    case PB_PHOTO_GUI_INIT_SHOW:
        ReturnValue = pb_photo_start_show_gui();
        break;
    case PB_PHOTO_UPDATE_BAT_POWER_STATUS:
        ReturnValue = pb_photo_update_bat_power_status(param1);
        break;
    case PB_PHOTO_WARNING_MSG_SHOW_START:
        ReturnValue = pb_photo_warning_msg_show(1, param1, param2);
        break;
    case PB_PHOTO_WARNING_MSG_SHOW_STOP:
        ReturnValue = pb_photo_warning_msg_show(0, param1, param2);
        break;
    default:
        break;
    }

    return ReturnValue;
}
