/**
  * @file src/app/apps/flow/rec/sound/rec_sound_op.c
  *
  * Operations of Sport Recorder (sensor) application
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
#include <system/app_util.h>

#if defined(CONFIG_APP_CONNECTED_AMBA_LINK)
#include <system/app_pref.h>
#endif

/*************************************************************************
 * App Function Declarations (static)
 ************************************************************************/
static int rec_sound_button_record(void);
static int rec_sound_button_focus(void);
static int rec_sound_button_focus_clr(void);
static int rec_sound_button_shutter(void);
static int rec_sound_button_shutter_clr(void);
static int rec_sound_button_zoom_in(void);
static int rec_sound_button_zoom_in_clr(void);
static int rec_sound_button_zoom_out(void);
static int rec_sound_button_zoom_out_clr(void);
static int rec_sound_button_up(void);
static int rec_sound_button_down(void);
static int rec_sound_button_left(void);
static int rec_sound_button_right(void);
static int rec_sound_button_set(void);
static int rec_sound_button_menu(void);
static int rec_sound_button_mode(void);
static int rec_sound_button_del(void);
static int rec_sound_button_power(void);

REC_SOUND_OP_s rec_sound_op = {
    rec_sound_button_record,
    rec_sound_button_focus,
    rec_sound_button_focus_clr,
    rec_sound_button_shutter,
    rec_sound_button_shutter_clr,
    rec_sound_button_zoom_in,
    rec_sound_button_zoom_in_clr,
    rec_sound_button_zoom_out,
    rec_sound_button_zoom_out_clr,
    rec_sound_button_up,
    rec_sound_button_down,
    rec_sound_button_left,
    rec_sound_button_right,
    rec_sound_button_set,
    rec_sound_button_menu,
    rec_sound_button_mode,
    rec_sound_button_del,
    rec_sound_button_power
};

/**
 *  @brief The operation of Record button.
 *
 *  The operation of Record button.
 *
 *  @return >=0 success, <0 failure
 */
static int rec_sound_button_record(void)
{
    int ReturnValue = 0;

    /* Close the menu or dialog. */
    AppWidget_Off(WIDGET_ALL, 0);
    if (rec_sound.RecState == REC_SOUND_STATE_RECORD) {
        /* Stop recording. */
        rec_sound.Func(REC_SOUND_RECORD_STOP, 0, 0);
    } else {
        /* Check the card's status. */
        ReturnValue = rec_sound.Func(REC_SOUND_CARD_CHECK_STATUS, 0, 0);
        if (ReturnValue == 0) {
            /* To record the clip if the card is ready. */
            rec_sound.Func(REC_SOUND_RECORD_START, 0, 0);
        }
    }
    return ReturnValue;
}

/**
 *  @brief The operation of Focus button.
 *
 *  The operation of Focus button.
 *
 *  @return >=0 success, <0 failure
 */
static int rec_sound_button_focus(void)
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
static int rec_sound_button_focus_clr(void)
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
static int rec_sound_button_shutter(void)
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
static int rec_sound_button_shutter_clr(void)
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
static int rec_sound_button_zoom_in(void)
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
static int rec_sound_button_zoom_in_clr(void)
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
static int rec_sound_button_zoom_out(void)
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
static int rec_sound_button_zoom_out_clr(void)
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
static int rec_sound_button_up(void)
{
    int ReturnValue = 0;

    return ReturnValue;
}

/**
 *  @brief The operation of Down button.
 *
 *  The operation of Down button.
 *
 *  @return >=0 success, <0 failure
 */
static int rec_sound_button_down(void)
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
static int rec_sound_button_left(void)
{
    int ReturnValue = 0;

    return ReturnValue;
}

/**
 *  @brief The operation of Right button.
 *
 *  The operation of Right button.
 *
 *  @return >=0 success, <0 failure
 */
static int rec_sound_button_right(void)
{
    int ReturnValue = 0;

    return ReturnValue;
}

/**
 *  @brief The operation of Set button.
 *
 *  The operation of Set button.
 *
 *  @return >=0 success, <0 failure
 */
static int rec_sound_button_set(void)
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
static int rec_sound_button_menu(void)
{
    int ReturnValue = 0;
    if (!APP_CHECKFLAGS(app_rec_sound.GFlags, APP_AFLAGS_POPUP)) {
        /** record stop at menu open*/
        rec_sound.Func(REC_SOUND_RECORD_STOP, 0, 0);
        rec_sound.Func(REC_SOUND_WARNING_MSG_SHOW_STOP, 0, 0);
        APP_ADDFLAGS(app_rec_sound.GFlags, APP_AFLAGS_POPUP);
        ReturnValue = AppWidget_On(WIDGET_MENU, 0);
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
static int rec_sound_button_mode(void)
{
    int ReturnValue = 0;
    /* Switch mode. */
    AppUtil_SwitchMode(APP_PB_SOUND);

    return ReturnValue;
}

/**
 *  @brief The operation of Delete button.
 *
 *  The operation of Delete button.
 *
 *  @return >=0 success, <0 failure
 */
static int rec_sound_button_del(void)
{
    int ReturnValue = 0;

    return ReturnValue;
}

/**
 *  @brief The operation of Power button.
 *
 *  The operation of Power button.
 *
 *  @return >=0 success, <0 failure
 */
static int rec_sound_button_power(void)
{
    int ReturnValue = 0;

    return ReturnValue;
}
