/**
  * @file src/app/apps/flow/pb/connectedcam/pb_photo_op.c
  *
  * Operations of photo playback application
  *
  * History:
  *    2013/11/08 - [Martin Lai] created file
  *
  *
 * Copyright (c) 2015 Ambarella, Inc.
 *
 * This file and its contents (��Software��) are protected by intellectual property rights
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
#include <system/app_util.h>

/*************************************************************************
 * App Function Declarations (static)
 ************************************************************************/
static int pb_photo_button_record(void);
static int pb_photo_button_focus(void);
static int pb_photo_button_focus_clr(void);
static int pb_photo_button_shutter(void);
static int pb_photo_button_shutter_clr(void);
static int pb_photo_button_zoom_in(void);
static int pb_photo_button_zoom_in_clr(void);
static int pb_photo_button_zoom_out(void);
static int pb_photo_button_zoom_out_clr(void);
static int pb_photo_button_up(void);
static int pb_photo_button_down(void);
static int pb_photo_button_left(void);
static int pb_photo_button_right(void);
static int pb_photo_button_set(void);
static int pb_photo_button_menu(void);
static int pb_photo_button_mode(void);
static int pb_photo_button_del(void);
static int pb_photo_button_power(void);

PB_PHOTO_OP_s pb_photo_op = {
    pb_photo_button_record,
    pb_photo_button_focus,
    pb_photo_button_focus_clr,
    pb_photo_button_shutter,
    pb_photo_button_shutter_clr,
    pb_photo_button_zoom_in,
    pb_photo_button_zoom_in_clr,
    pb_photo_button_zoom_out,
    pb_photo_button_zoom_out_clr,
    pb_photo_button_up,
    pb_photo_button_down,
    pb_photo_button_left,
    pb_photo_button_right,
    pb_photo_button_set,
    pb_photo_button_menu,
    pb_photo_button_mode,
    pb_photo_button_del,
    pb_photo_button_power
};

/**
 *  @brief The operation of Record button.
 *
 *  The operation of Record button.
 *
 *  @return >=0 success, <0 failure
 */
static int pb_photo_button_record(void)
{
    int ReturnValue = 0;

    return ReturnValue;
}

/**
 *  @brief The operation of Focus button.
 *
 *  The operation of Focus button.
 *
 *  @return >=0 success, <0 failure
 */
static int pb_photo_button_focus(void)
{
    int ReturnValue = 0;

    return ReturnValue;
}

/**
 *  @brief The operation of Focus button release.
 *
 *  The operation of Focus button release.
 *
 *  @return >=0 success, <0 failure
 */
static int pb_photo_button_focus_clr(void)
{
    int ReturnValue = 0;

    return ReturnValue;
}

/**
 *  @brief The operation of Shutter button.
 *
 *  The operation of Shutter button.
 *
 *  @return >=0 success, <0 failure
 */
static int pb_photo_button_shutter(void)
    {
    int ReturnValue = 0;

    return ReturnValue;
}

/**
 *  @brief The operation of Shutter button release.
 *
 *  The operation of Shutter button release.
 *
 *  @return >=0 success, <0 failure
 */
static int pb_photo_button_shutter_clr(void)
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
static int pb_photo_button_zoom_in(void)
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
static int pb_photo_button_zoom_in_clr(void)
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
static int pb_photo_button_zoom_out(void)
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
static int pb_photo_button_zoom_out_clr(void)
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
static int pb_photo_button_up(void)
{
    int ReturnValue = 0;

    if (APP_CHECKFLAGS(app_pb_photo.GFlags, APP_AFLAGS_BUSY)) {
        APP_ADDFLAGS(app_pb_photo.Flags, PB_PHOTO_OP_BLOCKED);
        pb_photo.OpBlocked = pb_photo_button_up;
    } else {
        if (pb_photo.FileInfo.TotalFileNum > 0) {
            ReturnValue = pb_photo.Func(PB_PHOTO_SWITCH_APP, 0, 0);
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
static int pb_photo_button_down(void)
{
    int ReturnValue = 0;

    return ReturnValue;
}

/**
 *  @brief The operation of Left button.
 *
 *  Left button operation
 *
 *  @return >=0 success, <0 failure
 */
static int pb_photo_button_left(void)
{
    int ReturnValue = 0;

    if (APP_CHECKFLAGS(app_pb_photo.GFlags, APP_AFLAGS_BUSY)) {
        ReturnValue = 0;
    } else {
        if (pb_photo.FileInfo.TotalFileNum > 0) {
            ReturnValue = pb_photo.Func(PB_PHOTO_GET_FILE, GET_PREV_FILE, 0);
            if (ReturnValue == 0) {
                pb_photo.Func(PB_PHOTO_PLAY, 0, 0);
            } else {
                ReturnValue = pb_photo.Func(PB_PHOTO_SWITCH_APP, 0, 0);
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
static int pb_photo_button_right(void)
{
    int ReturnValue = 0;

    if (APP_CHECKFLAGS(app_pb_photo.GFlags, APP_AFLAGS_BUSY)) {
        ReturnValue = 0;
    } else {
        if (pb_photo.FileInfo.TotalFileNum > 0) {
            ReturnValue = pb_photo.Func(PB_PHOTO_GET_FILE, GET_NEXT_FILE, 0);
            if (ReturnValue == 0) {
                pb_photo.Func(PB_PHOTO_PLAY, 0, 0);
            } else {
                ReturnValue = pb_photo.Func(PB_PHOTO_SWITCH_APP, 0, 0);
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
static int pb_photo_button_set(void)
{
    int ReturnValue = 0;

    return ReturnValue;
}

/**
 *  @brief The operation of Menu button.
 *
 *  The operation of Menu button.
 *
 *  @return >=0 success, <0 failure
 */
static int pb_photo_button_menu(void)
{
    int ReturnValue = 0;
    if (APP_CHECKFLAGS(app_pb_photo.GFlags, APP_AFLAGS_BUSY)) {
        APP_ADDFLAGS(app_pb_photo.Flags, PB_PHOTO_OP_BLOCKED);
        pb_photo.OpBlocked = pb_photo_button_menu;
    } else {
        if (!APP_CHECKFLAGS(app_pb_photo.GFlags, APP_AFLAGS_POPUP)) {
            pb_photo.Func(PB_PHOTO_WARNING_MSG_SHOW_STOP, 0, 0);
            APP_ADDFLAGS(app_pb_photo.GFlags, APP_AFLAGS_POPUP);
            ReturnValue = AppWidget_On(WIDGET_MENU, 0);
        }
    }
    return ReturnValue;
}

/**
 *  @brief The operation of Mode button.
 *
 *  The operation of Mode button.
 *
 *  @return >=0 success, <0 failure
 */
static int pb_photo_button_mode(void)
{
    int ReturnValue = 0;
    pb_photo.Func(PB_PHOTO_SWITCH_APP, 0, 0);
    return ReturnValue;
}

/**
 *  @brief The operation of Delete button.
 *
 *  The operation of Delete button.
 *
 *  @return >=0 success, <0 failure
 */
static int pb_photo_button_del(void)
{
    int ReturnValue = 0;
    if (APP_CHECKFLAGS(app_pb_photo.GFlags, APP_AFLAGS_BUSY)) {
        APP_ADDFLAGS(app_pb_photo.Flags, PB_PHOTO_OP_BLOCKED);
        pb_photo.OpBlocked = pb_photo_button_del;
    } else {
        if (pb_photo.FileInfo.TotalFileNum > 0) {
            pb_photo.Func(PB_PHOTO_DELETE_FILE_DIALOG_SHOW, 0, 0);
        }
    }
    return ReturnValue;
}

/**
 *  @brief The operation of Power button.
 *
 *  The operation of Power button.
 *
 *  @return >=0 success, <0 failure
 */
static int pb_photo_button_power(void)
{
    int ReturnValue = 0;

    return ReturnValue;
}
