/**
  * @file src/app/apps/flow/pb/connectedcam/hdmi_test_op.c
  *
  * Operations of multi playback application
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
#include <system/app_util.h>
#include <system/status.h>
#include <wchar.h>

/*************************************************************************
 * App Function Declarations (static)
 ************************************************************************/
static int hdmi_test_button_record(void);
static int hdmi_test_button_focus(void);
static int hdmi_test_button_focus_clr(void);
static int hdmi_test_button_shutter(void);
static int hdmi_test_button_shutter_clr(void);
static int hdmi_test_button_zoom_in(void);
static int hdmi_test_button_zoom_in_clr(void);
static int hdmi_test_button_zoom_out(void);
static int hdmi_test_button_zoom_out_clr(void);
static int hdmi_test_button_up(void);
static int hdmi_test_button_down(void);
static int hdmi_test_button_left(void);
static int hdmi_test_button_right(void);
static int hdmi_test_button_set(void);
static int hdmi_test_button_menu(void);
static int hdmi_test_button_n4(void);
static int hdmi_test_button_n5(void);
static int hdmi_test_button_n6(void);
static int hdmi_test_button_n7(void);
static int hdmi_test_button_n8(void);
static int hdmi_test_button_n9(void);
static int hdmi_test_button_n0(void);
static int hdmi_test_button_n100(void);
static int hdmi_test_button_return(void);
static int hdmi_test_button_display(void);
static int hdmi_test_button_standard(void);
static int hdmi_test_button_image(void);
static int hdmi_test_button_mode(void);
static int hdmi_test_button_del(void);
static int hdmi_test_button_power(void);

HDMI_TEST_OP_s hdmi_test_op = {
    hdmi_test_button_record,
    hdmi_test_button_focus,
    hdmi_test_button_focus_clr,
    hdmi_test_button_shutter,
    hdmi_test_button_shutter_clr,
    hdmi_test_button_zoom_in,
    hdmi_test_button_zoom_in_clr,
    hdmi_test_button_zoom_out,
    hdmi_test_button_zoom_out_clr,
    hdmi_test_button_up,
    hdmi_test_button_down,
    hdmi_test_button_left,
    hdmi_test_button_right,
    hdmi_test_button_set,
    hdmi_test_button_menu,
    hdmi_test_button_n4,
    hdmi_test_button_n5,
    hdmi_test_button_n6,
    hdmi_test_button_n7,
    hdmi_test_button_n8,
    hdmi_test_button_n9,
    hdmi_test_button_n0,
    hdmi_test_button_n100,
    hdmi_test_button_return,
    hdmi_test_button_display,
    hdmi_test_button_standard,
    hdmi_test_button_image,
    hdmi_test_button_mode,
    hdmi_test_button_del,
    hdmi_test_button_power
};

/**
 *  @brief The operation of Record button.
 *
 *  The operation of Record button(N3).
 *
 *  @return >=0 success, <0 failure
 */
static int hdmi_test_button_record(void)
{
    int ReturnValue = 0;
    AmbaPrint("[Fixed vout mode] Display mode: 1080I");
    hdmi_test.Func(HDMI_TEST_UPDATE_FCHAN_FIXED_VOUT, 0, VOUT_DISP_MODE_1080I);

    return ReturnValue;
}

/**
 *  @brief The operation of Focus button.
 *
 *  The operation of Focus button(N2).
 *
 *  @return >=0 success, <0 failure
 */
static int hdmi_test_button_focus(void)
{
    int ReturnValue = 0;
    AmbaPrint("[Fixed vout mode] Display mode: 1080P half");
    hdmi_test.Func(HDMI_TEST_UPDATE_FCHAN_FIXED_VOUT, 0, VOUT_DISP_MODE_1080P_HALF);

    return ReturnValue;
}

/**
 *  @brief The operation of Focus button release.
 *
 *  The operation of Focus button release.
 *
 *  @return >=0 success, <0 failure
 */
static int hdmi_test_button_focus_clr(void)
{
    int ReturnValue = 0;

    return ReturnValue;
}

/**
 *  @brief The operation of Shutter button.
 *
 *  The operation of Shutter button(N1).
 *
 *  @return >=0 success, <0 failure
 */
static int hdmi_test_button_shutter(void)
    {
    int ReturnValue = 0;

    AmbaPrint("[Fixed vout mode] Display mode: 1080P");
    hdmi_test.Func(HDMI_TEST_UPDATE_FCHAN_FIXED_VOUT, 0, VOUT_DISP_MODE_1080P);
    return ReturnValue;
}

/**
 *  @brief The operation of Shutter button release.
 *
 *  The operation of Shutter button release.
 *
 *  @return >=0 success, <0 failure
 */
static int hdmi_test_button_shutter_clr(void)
{
    int ReturnValue = 0;


    return ReturnValue;
}

/**
 *  @brief The operation of Zoom_in button.
 *
 *  The operation of Zoom_in button.
 *
 *  @return >=0 success, <0 failure
 */
static int hdmi_test_button_zoom_in(void)
{
    int ReturnValue = 0;

    return ReturnValue;
}

/**
 *  @brief The operation of Zoom_in button release.
 *
 *  The operation of Zoom_in button release.
 *
 *  @return >=0 success, <0 failure
 */
static int hdmi_test_button_zoom_in_clr(void)
{
    int ReturnValue = 0;

    return ReturnValue;
}

/**
 *  @brief The operation of Zoom_out button.
 *
 *  The operation of Zoom_out button.
 *
 *  @return >=0 success, <0 failure
 */
static int hdmi_test_button_zoom_out(void)
{
    int ReturnValue = 0;
    return ReturnValue;
}

/**
 *  @brief The operation of Zoom_out button release.
 *
 *  The operation of Zoom_out button release.
 *
 *  @return >=0 success, <0 failure
 */
static int hdmi_test_button_zoom_out_clr(void)
{
    int ReturnValue = 0;

    return ReturnValue;
}

/**
 *  @brief The operation of Up button.
 *
 *  The operation of Up button.
 *
 *  @return >=0 success, <0 failure
 */
static int hdmi_test_button_up(void)
{
    int ReturnValue = 0;

    if (APP_CHECKFLAGS(app_hdmi_test.GFlags, APP_AFLAGS_BUSY)) {
        /** video : pause */
        if (hdmi_test.CurFileType == HDMI_TEST_MEDIA_VIDEO) {
            hdmi_test.MediaInfo.State = HDMI_TEST_PLAY_PAUSED;
            hdmi_test.Func(HDMI_TEST_PLAY, 0, 1);
            if (!APP_CHECKFLAGS(app_hdmi_test.GFlags, APP_AFLAGS_BUSY)) {
                /* The busy flag will be removed when the flow stop the video player. */
                /* To excute the functions that system block them when the Busy flag is enabled. */
                AppUtil_BusyCheck(0);
            }
            if (APP_CHECKFLAGS(app_hdmi_test.GFlags, APP_AFLAGS_READY)) {
                /* The system could switch the current app to other in the function "AppUtil_BusyCheck". */
                hdmi_test.Gui(GUI_PLAY_STATE_UPDATE, GUI_PAUSE, 0);
                hdmi_test.Gui(GUI_FLUSH, 0, 0);
            }
        } else {
            APP_ADDFLAGS(app_hdmi_test.Flags, HDMI_TEST_OP_BLOCKED);
            hdmi_test.OpBlocked = hdmi_test_button_up;
        }
    } else {
        if (hdmi_test.FileInfo.TotalFileNum > 0) {
            if (hdmi_test.CurFileType == HDMI_TEST_MEDIA_VIDEO) {
                ReturnValue = AppLibVideoDec_Step();
                if (hdmi_test.MediaInfo.Direction == HDMI_TEST_PLAY_REV) {
                    hdmi_test.Gui(GUI_PLAY_STATE_UPDATE, GUI_REW_STEP, 0);
                } else {
                    hdmi_test.Gui(GUI_PLAY_STATE_UPDATE, GUI_FWD_STEP, 0);
                }
                hdmi_test.Gui(GUI_FLUSH, 0, 0);
                AmbaKAL_TaskSleep(200);
                hdmi_test.Gui(GUI_PLAY_STATE_UPDATE, GUI_PAUSE, 0);
                hdmi_test.Gui(GUI_FLUSH, 0, 0);
            } else {
                ReturnValue = hdmi_test.Func(HDMI_TEST_SWITCH_APP, 0, 0);
            }
        }
    }

    return ReturnValue;
}

/**
 *  @brief The operation of Down button.
 *
 *  The operation of Down button.
 *
 *  @return >=0 success, <0 failure
 */
static int hdmi_test_button_down(void)
{
    int ReturnValue = 0;
    if (hdmi_test.CurFileType == HDMI_TEST_MEDIA_VIDEO) {
        if (APP_CHECKFLAGS(app_hdmi_test.GFlags, APP_AFLAGS_BUSY)) {
            strcpy(hdmi_test.FirstFn, hdmi_test.CurFn);
            if (hdmi_test.MediaInfo.Direction == HDMI_TEST_PLAY_FWD) {
                if (hdmi_test.CurPlayTime <= 0) {
                    AmbaPrint("[hdmi_test] <button_down> Current play time equal 0. Can not do backward play.");
                    return ReturnValue;
                }
                hdmi_test.MediaInfo.Direction = HDMI_TEST_PLAY_REV;
            } else {
                if (hdmi_test.CurPlayTime >= hdmi_test.MediaInfo.TotalTime) {
                    AmbaPrint("[hdmi_test] <button_down> Current play time equal total time. Can not do forward play.");
                    return ReturnValue;
                }
                hdmi_test.MediaInfo.Direction = HDMI_TEST_PLAY_FWD;
            }
            hdmi_test.MediaInfo.State = HDMI_TEST_PLAY_PLAY;
            hdmi_test.MediaInfo.Speed = PBACK_SPEED_NORMAL;
            ReturnValue = hdmi_test.Func(HDMI_TEST_PLAY, 0, 1);
            if (hdmi_test.MediaInfo.Direction == HDMI_TEST_PLAY_REV) {
                hdmi_test.Gui(GUI_PLAY_STATE_UPDATE, GUI_REW_NORMAL, 0);
            } else {
                hdmi_test.Gui(GUI_PLAY_STATE_UPDATE, GUI_FWD_NORMAL, 0);
            }
            hdmi_test.Gui(GUI_FLUSH, 0, 0);
        } else {
            if (hdmi_test.FileInfo.TotalFileNum > 0) {
                strcpy(hdmi_test.FirstFn, hdmi_test.CurFn);
                if (hdmi_test.MediaInfo.Direction == HDMI_TEST_PLAY_FWD) {
                    hdmi_test.MediaInfo.Direction = HDMI_TEST_PLAY_REV;
                } else {
                    hdmi_test.MediaInfo.Direction = HDMI_TEST_PLAY_FWD;
                }
                hdmi_test.MediaInfo.State = HDMI_TEST_PLAY_PLAY;
                hdmi_test.MediaInfo.Speed = PBACK_SPEED_NORMAL;
                hdmi_test.Func(HDMI_TEST_PLAY, 0, 1);
                if (hdmi_test.MediaInfo.Direction == HDMI_TEST_PLAY_REV) {
                    hdmi_test.Gui(GUI_PLAY_STATE_UPDATE, GUI_REW_NORMAL, 0);
                } else {
                    hdmi_test.Gui(GUI_PLAY_STATE_UPDATE, GUI_FWD_NORMAL, 0);
                }
                hdmi_test.Gui(GUI_FLUSH, 0, 0);
            }
        }
    }
    return ReturnValue;
}

/**
 *  @brief The operation of Left button.
 *
 *  Left button operation
 *
 *  @return >=0 success, <0 failure
 */
static int hdmi_test_button_left(void)
{
    int ReturnValue = 0;

    if (APP_CHECKFLAGS(app_hdmi_test.GFlags, APP_AFLAGS_BUSY)) {
        if (hdmi_test.CurFileType == HDMI_TEST_MEDIA_VIDEO) {
            hdmi_test.MediaInfo.Speed >>= 1;
            AppLibVideoDec_SpeedDown(&hdmi_test.MediaInfo.Speed);
            if (hdmi_test.MediaInfo.Direction == HDMI_TEST_PLAY_REV) {
                if (hdmi_test.MediaInfo.Speed == PBACK_SPEED_NORMAL) {
                    hdmi_test.Gui(GUI_PLAY_STATE_UPDATE, GUI_REW_NORMAL, 0);
                } else if (hdmi_test.MediaInfo.Speed > PBACK_SPEED_NORMAL) {
                    hdmi_test.Gui(GUI_PLAY_STATE_UPDATE, GUI_REW_FAST, hdmi_test.MediaInfo.Speed);
                } else {
                    hdmi_test.Gui(GUI_PLAY_STATE_UPDATE, GUI_REW_SLOW, hdmi_test.MediaInfo.Speed);
                }
            } else {
                if (hdmi_test.MediaInfo.Speed == PBACK_SPEED_NORMAL) {
                    hdmi_test.Gui(GUI_PLAY_STATE_UPDATE, GUI_FWD_NORMAL, 0);
                } else if (hdmi_test.MediaInfo.Speed > PBACK_SPEED_NORMAL) {
                    hdmi_test.Gui(GUI_PLAY_STATE_UPDATE, GUI_FWD_FAST, hdmi_test.MediaInfo.Speed);
                } else {
                    hdmi_test.Gui(GUI_PLAY_STATE_UPDATE, GUI_FWD_SLOW, hdmi_test.MediaInfo.Speed);
                }
            }
            hdmi_test.Gui(GUI_FLUSH, 0, 0);
        }
    } else {
        if (hdmi_test.FileInfo.TotalFileNum > 0) {
            if (ReturnValue == 0) {
            hdmi_test.Func(HDMI_TEST_STOP_PLAYING, 0, 0);
            ReturnValue = hdmi_test.Func(HDMI_TEST_GET_FILE, GET_PREV_FILE, hdmi_test.FileInfo.MediaRoot);
                if (hdmi_test.CurFileType == HDMI_TEST_MEDIA_VIDEO) {
                    strcpy(hdmi_test.FirstFn, hdmi_test.CurFn);
                    hdmi_test.Gui(GUI_PLAY_STATE_UPDATE, GUI_PAUSE, 0);
                    hdmi_test.Gui(GUI_PLAY_TIMER_UPDATE, 0, hdmi_test.MediaInfo.TotalTime/1000);
                    hdmi_test.Gui(GUI_PLAY_STATE_SHOW, 0, 0);
                    hdmi_test.Gui(GUI_PLAY_SPEED_SHOW, 0, 0);
                    ReturnValue = hdmi_test.Func(HDMI_TEST_OPEN, HDMI_TEST_OPEN_RESET, 0);
                    hdmi_test.MediaInfo.State = HDMI_TEST_PLAY_PLAY;
                    hdmi_test.Func(HDMI_TEST_PLAY, 0, 1);/**<param2 : 0 photo : 1 video */
                    hdmi_test.Gui(GUI_PLAY_STATE_UPDATE, GUI_FWD_NORMAL, 0);
                } else if (hdmi_test.CurFileType == HDMI_TEST_MEDIA_IMAGE) {
                    hdmi_test.Func(HDMI_TEST_PLAY, 0, 0);
                }
             } else {
                ReturnValue = hdmi_test.Func(HDMI_TEST_SWITCH_APP, 0, 0);
            }

        }
    }

    return ReturnValue;
}

/**
 *  @brief The operation of Right button.
 *
 *  The operation of Right button.
 *
 *  @return >=0 success, <0 failure
 */
static int hdmi_test_button_right(void)
{
        int ReturnValue = 0;

        if (APP_CHECKFLAGS(app_hdmi_test.GFlags, APP_AFLAGS_BUSY)) {
            if (hdmi_test.CurFileType == HDMI_TEST_MEDIA_VIDEO) {
                hdmi_test.MediaInfo.Speed <<= 1;
                AppLibVideoDec_SpeedUp(&hdmi_test.MediaInfo.Speed);
                if (hdmi_test.MediaInfo.Direction == HDMI_TEST_PLAY_REV) {
                    if (hdmi_test.MediaInfo.Speed == PBACK_SPEED_NORMAL) {
                        hdmi_test.Gui(GUI_PLAY_STATE_UPDATE, GUI_REW_NORMAL, 0);
                    } else if (hdmi_test.MediaInfo.Speed > PBACK_SPEED_NORMAL) {
                        hdmi_test.Gui(GUI_PLAY_STATE_UPDATE, GUI_REW_FAST, hdmi_test.MediaInfo.Speed);
                    } else {
                        hdmi_test.Gui(GUI_PLAY_STATE_UPDATE, GUI_REW_SLOW, hdmi_test.MediaInfo.Speed);
                    }
                } else {
                    if (hdmi_test.MediaInfo.Speed == PBACK_SPEED_NORMAL) {
                        hdmi_test.Gui(GUI_PLAY_STATE_UPDATE, GUI_FWD_NORMAL, 0);
                    } else if (hdmi_test.MediaInfo.Speed > PBACK_SPEED_NORMAL) {
                        hdmi_test.Gui(GUI_PLAY_STATE_UPDATE, GUI_FWD_FAST, hdmi_test.MediaInfo.Speed);
                    } else {
                        hdmi_test.Gui(GUI_PLAY_STATE_UPDATE, GUI_FWD_SLOW, hdmi_test.MediaInfo.Speed);
                    }
                }
                hdmi_test.Gui(GUI_FLUSH, 0, 0);
            }
        } else {
            if (hdmi_test.FileInfo.TotalFileNum > 0) {
                hdmi_test.Func(HDMI_TEST_STOP_PLAYING, 0, 0);
                ReturnValue = hdmi_test.Func(HDMI_TEST_GET_FILE, GET_NEXT_FILE, hdmi_test.FileInfo.MediaRoot);
                if (ReturnValue == 0) {
                    if (hdmi_test.CurFileType == HDMI_TEST_MEDIA_VIDEO) {
                        strcpy(hdmi_test.FirstFn, hdmi_test.CurFn);
                        hdmi_test.Gui(GUI_PLAY_STATE_UPDATE, GUI_PAUSE, 0);
                        hdmi_test.Gui(GUI_PLAY_TIMER_UPDATE, 0, hdmi_test.MediaInfo.TotalTime/1000);
                        hdmi_test.Gui(GUI_PLAY_STATE_SHOW, 0, 0);
                        hdmi_test.Gui(GUI_PLAY_SPEED_SHOW, 0, 0);
                        ReturnValue = hdmi_test.Func(HDMI_TEST_OPEN, HDMI_TEST_OPEN_RESET, 0);
                        hdmi_test.MediaInfo.State = HDMI_TEST_PLAY_PLAY;
                        hdmi_test.Func(HDMI_TEST_PLAY, 0, 1);/**<param2 : 0 photo : 1 video */
                        hdmi_test.Gui(GUI_PLAY_STATE_UPDATE, GUI_FWD_NORMAL, 0);
                    } else if (hdmi_test.CurFileType == HDMI_TEST_MEDIA_IMAGE) {
                        hdmi_test.Func(HDMI_TEST_PLAY, 0, 0);

                    }
                } else {
                    ReturnValue = hdmi_test.Func(HDMI_TEST_SWITCH_APP, 0, 0);
                }
            }
        }

        return ReturnValue;
}

/**
 *  @brief The operation of Set button.
 *
 *  The operation of Set button.
 *
 *  @return >=0 success, <0 failure
 */
static int hdmi_test_button_set(void)
{
    int ReturnValue = 0;
    APPLIB_VIDEO_START_MULTI_INFO_s VideoStartInfo;
    APPLIB_VIDEO_FILE_INFO_s File;

    if (hdmi_test.CurFileType == HDMI_TEST_MEDIA_VIDEO) {
        if (APP_CHECKFLAGS(app_hdmi_test.GFlags, APP_AFLAGS_BUSY)) {
            if (hdmi_test.MediaInfo.Speed == PBACK_SPEED_NORMAL) {
                hdmi_test.MediaInfo.State = HDMI_TEST_PLAY_PAUSED;
                APP_REMOVEFLAGS(app_hdmi_test.GFlags, APP_AFLAGS_BUSY);
                AppLibVideoDec_Stop();
                ReturnValue = hdmi_test.Func(HDMI_TEST_SWITCH_APP, 0, 0);
            } else {
                hdmi_test.MediaInfo.Speed = PBACK_SPEED_NORMAL;
                File.Filename = hdmi_test.CurFn;
                VideoStartInfo.File = &File;
                VideoStartInfo.FileNum= 1;
                VideoStartInfo.AutoPlay = 1;
                VideoStartInfo.Direction = (APPLIB_VIDEO_PLAY_DIRECTION_e)hdmi_test.MediaInfo.Direction;
                VideoStartInfo.ResetSpeed = 1;
                VideoStartInfo.ResetZoom = 0;
                VideoStartInfo.StartTime = hdmi_test.MediaInfo.PlayTime;
                VideoStartInfo.ReloadFile = 0;
                ReturnValue = AppLibVideoDec_StartMultiple(&VideoStartInfo);
                if (hdmi_test.MediaInfo.Direction == HDMI_TEST_PLAY_REV) {
                    hdmi_test.Gui(GUI_PLAY_STATE_UPDATE, GUI_REW_NORMAL, 0);
                } else {
                    hdmi_test.Gui(GUI_PLAY_STATE_UPDATE, GUI_FWD_NORMAL, 0);
                }
                hdmi_test.Gui(GUI_FLUSH, 0, 0);
            }
        } else {
            if (hdmi_test.FileInfo.TotalFileNum > 0) {
                strcpy(hdmi_test.FirstFn, hdmi_test.CurFn);
                hdmi_test.MediaInfo.State = HDMI_TEST_PLAY_PLAY;
                hdmi_test.MediaInfo.Speed = PBACK_SPEED_NORMAL;
                ReturnValue = hdmi_test.Func(HDMI_TEST_PLAY, 1, 1);
                if (hdmi_test.MediaInfo.Direction == HDMI_TEST_PLAY_REV) {
                    hdmi_test.Gui(GUI_PLAY_STATE_UPDATE, GUI_REW_NORMAL, 0);
                } else {
                    hdmi_test.Gui(GUI_PLAY_STATE_UPDATE, GUI_FWD_NORMAL, 0);
                }
                hdmi_test.Gui(GUI_FLUSH, 0, 0);
            }
        }
    }
    return ReturnValue;
}

/**
 *  @brief The operation of Menu button.
 *
 *  The operation of Menu button.
 *
 *  @return >=0 success, <0 failure
 */
static int hdmi_test_button_menu(void)
{
    int ReturnValue = 0;

    if (!APP_CHECKFLAGS(app_hdmi_test.GFlags, APP_AFLAGS_POPUP)) {
        hdmi_test.Func(HDMI_TEST_WARNING_MSG_SHOW_STOP, 0, 0);
        APP_ADDFLAGS(app_hdmi_test.GFlags, APP_AFLAGS_POPUP);
        ReturnValue = AppWidget_On(WIDGET_MENU, 0);
    }

    return ReturnValue;
}

/**
 *  @brief The operation of N4 button.
 *
 *  The operation of N4 button.
 *
 *  @return >=0 success, <0 failure
 */
static int hdmi_test_button_n4(void)
{
    int ReturnValue = 0;
    AmbaPrint("[Fixed vout mode] Display mode: 1080P24");
    hdmi_test.Func(HDMI_TEST_UPDATE_FCHAN_FIXED_VOUT, 0, VOUT_DISP_MODE_1080P24);
    return ReturnValue;
}

/**
 *  @brief The operation of N5 button.
 *
 *  The operation of N5 button.
 *
 *  @return >=0 success, <0 failure
 */
static int hdmi_test_button_n5(void)
{
    int ReturnValue = 0;
    AmbaPrint("[Fixed vout mode] Display mode: 720P");
    hdmi_test.Func(HDMI_TEST_UPDATE_FCHAN_FIXED_VOUT, 0, VOUT_DISP_MODE_720P);
    return ReturnValue;
}

/**
 *  @brief The operation of N6 button.
 *
 *  The operation of N6 button.
 *
 *  @return >=0 success, <0 failure
 */
static int hdmi_test_button_n6(void)
{
    int ReturnValue = 0;
    AmbaPrint("[Fixed vout mode] Display mode: 720P half");
    hdmi_test.Func(HDMI_TEST_UPDATE_FCHAN_FIXED_VOUT, 0, VOUT_DISP_MODE_720P_HALF);
    return ReturnValue;
}

/**
 *  @brief The operation of N7 button.
 *
 *  The operation of N7 button.
 *
 *  @return >=0 success, <0 failure
 */
static int hdmi_test_button_n7(void)
{
    int ReturnValue = 0;
    AmbaPrint("[Fixed vout mode] Display mode: 720P24");
    hdmi_test.Func(HDMI_TEST_UPDATE_FCHAN_FIXED_VOUT, 0, VOUT_DISP_MODE_720P24);
    return ReturnValue;
}

/**
 *  @brief The operation of N8 button.
 *
 *  The operation of N8 button.
 *
 *  @return >=0 success, <0 failure
 */
static int hdmi_test_button_n8(void)
{
    int ReturnValue = 0;
    AmbaPrint("[Fixed vout mode] Display mode: SDP");
    hdmi_test.Func(HDMI_TEST_UPDATE_FCHAN_FIXED_VOUT, 0, VOUT_DISP_MODE_SDP);
    return ReturnValue;
}

/**
 *  @brief The operation of N9 button.
 *
 *  The operation of N9 button.
 *
 *  @return >=0 success, <0 failure
 */
static int hdmi_test_button_n9(void)
{
    int ReturnValue = 0;
    AmbaPrint("[Fixed vout mode] Display mode: SDI");
    hdmi_test.Func(HDMI_TEST_UPDATE_FCHAN_FIXED_VOUT, 0, VOUT_DISP_MODE_SDI);
    return ReturnValue;
}

/**
 *  @brief The operation of N0 button.
 *
 *  The operation of N0 button.
 *
 *  @return >=0 success, <0 failure
 */
static int hdmi_test_button_n0(void)
{
    int ReturnValue = 0;
    AmbaPrint("[HDMI test] Set YUV444 mode");
    hdmi_test.Func(HDMI_TEST_UPDATE_FCHAN_FIXED_VOUT, 1, AMBA_DSP_VOUT_HDMI_YCC444_8B);
    return ReturnValue;
}

/**
 *  @brief The operation of N100 button.
 *
 *  The operation of N100 button.
 *
 *  @return >=0 success, <0 failure
 */
static int hdmi_test_button_n100(void)
{
    int ReturnValue = 0;
    AmbaPrint("[HDMI test] Set RGB mode");
    hdmi_test.Func(HDMI_TEST_UPDATE_FCHAN_FIXED_VOUT, 1, AMBA_DSP_VOUT_HDMI_RGB444_8B);
    return ReturnValue;
}

/**
 *  @brief The operation of Return button.
 *
 *  The operation of Return button.
 *
 *  @return >=0 success, <0 failure
 */
static int hdmi_test_button_return(void)
{
    int ReturnValue = 0;
    AmbaPrint("[HDMI test] Set YUV422 mode");
    hdmi_test.Func(HDMI_TEST_UPDATE_FCHAN_FIXED_VOUT, 1, AMBA_DSP_VOUT_HDMI_YCC422_12B);
    return ReturnValue;
}

/**
 *  @brief The operation of Display button.
 *
 *  The operation of Display button.
 *
 *  @return >=0 success, <0 failure
 */
static int hdmi_test_button_display(void)
{
    int ReturnValue = 0;
    AmbaPrint("[HDMI test] Set 3D vout mode: Side-by-side");
    return ReturnValue;
}

/**
 *  @brief The operation of Standard button.
 *
 *  The operation of Standard button.
 *
 *  @return >=0 success, <0 failure
 */
static int hdmi_test_button_standard(void)
{
    int ReturnValue = 0;
    AmbaPrint("[HDMI test] Set 3D vout mode: Frame-packing");
    return ReturnValue;
}

/**
 *  @brief The operation of Image button.
 *
 *  The operation of Image button.
 *
 *  @return >=0 success, <0 failure
 */
static int hdmi_test_button_image(void)
{
    int ReturnValue = 0;
    AmbaPrint("[HDMI test] Set 3D vout mode: Top-and-bottom");
    return ReturnValue;
}

/**
 *  @brief The operation of Mode button.
 *
 *  The operation of Mode button.
 *
 *  @return >=0 success, <0 failure
 */
static int hdmi_test_button_mode(void)
{
    int ReturnValue = 0;
    //Disable this function when running HDMI test application.
    #if 0
    if (hdmi_test.CurFileType == HDMI_TEST_MEDIA_VIDEO) {
        APP_REMOVEFLAGS(app_hdmi_test.GFlags, APP_AFLAGS_BUSY);
        AppLibVideoDec_Stop();
    }
    hdmi_test.Func(HDMI_TEST_SWITCH_APP, 1, 0);
    #endif
    return ReturnValue;
}

/**
 *  @brief The operation of Delete button.
 *
 *  The operation of Delete button.
 *
 *  @return >=0 success, <0 failure
 */
static int hdmi_test_button_del(void)
{
    int ReturnValue = 0;
    //Disable this function when running HDMI test application.
    #if 0
    if (APP_CHECKFLAGS(app_hdmi_test.GFlags, APP_AFLAGS_BUSY)) {
        APP_ADDFLAGS(app_hdmi_test.Flags, HDMI_TEST_OP_BLOCKED);
        hdmi_test.OpBlocked = hdmi_test_button_del;
    } else {
        if (hdmi_test.FileInfo.TotalFileNum > 0) {
            hdmi_test.Func(HDMI_TEST_DELETE_FILE_DIALOG_SHOW, 0, 0);
        }
    }
    #endif
    return ReturnValue;
}

/**
 *  @brief The operation of Power button.
 *
 *  The operation of Power button.
 *
 *  @return >=0 success, <0 failure
 */
static int hdmi_test_button_power(void)
{
    int ReturnValue = 0;

    return ReturnValue;
}
