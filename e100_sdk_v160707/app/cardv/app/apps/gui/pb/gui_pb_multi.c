/**
  * @file src/app/apps/gui/rec/connectedcam/gui_pb_multi.c
  *
  *  Implementation of video playback GUI display flows
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
#include <apps/gui/pb/gui_pb_multi.h>
#include <apps/gui/resource/gui_settle.h>
#include <wchar.h>
#ifdef CONFIG_APP_ARD
#include <system/app_pref.h>
#endif


#define GOBJ_MEDIA_INFO_RES     (GOBJ_FV_ICON_LEFT_1)
#define GOBJ_MEDIA_INFO_FRATE   (GOBJ_FV_ICON_LEFT_2)
#define PBACK_SPEED_NORMAL        (0x1 <<  8)    /** 0x0100 */

static int set_play_state(int state, UINT32 speed)
{
    int ReturnValue = 0;
    switch (state) {
    case GUI_FWD_NORMAL:
        AppLibGraph_UpdateBMP(GRAPH_CH_DUAL, GOBJ_PB_PLAY_STATE, BMP_ICN_WHT_PLAY_F);
        break;
    case GUI_FWD_FAST:
        AppLibGraph_UpdateBMP(GRAPH_CH_DUAL, GOBJ_PB_PLAY_STATE, BMP_ICN_WHT_PLAY_FF);
        break;
    case GUI_FWD_FAST_END:
        AppLibGraph_UpdateBMP(GRAPH_CH_DUAL, GOBJ_PB_PLAY_STATE, BMP_ICN_WHT_PLAY_FF_END);
        break;
    case GUI_FWD_SLOW:
        AppLibGraph_UpdateBMP(GRAPH_CH_DUAL, GOBJ_PB_PLAY_STATE, BMP_ICN_WHT_PLAY_SF);
        break;
    case GUI_FWD_STEP:
        AppLibGraph_UpdateBMP(GRAPH_CH_DUAL, GOBJ_PB_PLAY_STATE, BMP_ICN_WHT_PLAY_STPF);
        break;
    case GUI_REW_NORMAL:
        AppLibGraph_UpdateBMP(GRAPH_CH_DUAL, GOBJ_PB_PLAY_STATE, BMP_ICN_WHT_PLAY_R);
        break;
    case GUI_REW_FAST:
        AppLibGraph_UpdateBMP(GRAPH_CH_DUAL, GOBJ_PB_PLAY_STATE, BMP_ICN_WHT_PLAY_FRWD);
        break;
    case GUI_REW_FAST_END:
        AppLibGraph_UpdateBMP(GRAPH_CH_DUAL, GOBJ_PB_PLAY_STATE, BMP_ICN_WHT_PLAY_FRWD_END);
        break;
    case GUI_REW_SLOW:
        AppLibGraph_UpdateBMP(GRAPH_CH_DUAL, GOBJ_PB_PLAY_STATE, BMP_ICN_WHT_PLAY_SR);
        break;
    case GUI_REW_STEP:
        AppLibGraph_UpdateBMP(GRAPH_CH_DUAL, GOBJ_PB_PLAY_STATE, BMP_ICN_WHT_PLAY_STPR);
        break;
    case GUI_PAUSE:
    default:
        AppLibGraph_UpdateBMP(GRAPH_CH_DUAL, GOBJ_PB_PLAY_STATE, BMP_ICN_WHT_PLAY_P);
        break;
    }
#if CONFIG_APP_ARD
    if (speed) {
        UINT16 ratio = 0;
        UINT16 str_ratio[6] = {0};
        if (speed < PBACK_SPEED_NORMAL) {
            ratio = PBACK_SPEED_NORMAL/speed;
            str_ratio[0] = 0x0031;
            str_ratio[1] = 0x002F;
            if (ratio/10) {
                str_ratio[2] = 0x0030+(ratio/10);
                str_ratio[3] = 0x0030+(ratio%10);
                str_ratio[4] = 0x0078;
                str_ratio[5] = 0x0000;
            } else {
                str_ratio[2] = 0x0030+ratio;
                str_ratio[3] = 0x0078;
                str_ratio[4] = 0x0000;
            }
        } else {
            ratio = speed/PBACK_SPEED_NORMAL;
            if (ratio/10) {
                str_ratio[0] = 0x0030+(ratio/10);
                str_ratio[1] = 0x0030+(ratio%10);
            } else {
                str_ratio[0] = 0x0020;
                str_ratio[1] = 0x0030+ratio;
            }
            str_ratio[2] = 0x0078;
            str_ratio[3] = 0x0000;
        }
        AppLibGraph_UpdateStringContext(AppPref_GetLanguageID(), STR_SPEED_RATIO, str_ratio);
        AppLibGraph_UpdateString(GRAPH_CH_DUAL, GOBJ_PB_SPEED_RATIO, STR_SPEED_RATIO);
    } else {
           AppLibGraph_UpdateString(GRAPH_CH_DUAL, GOBJ_PB_SPEED_RATIO, STR_NULL);
    }
#endif
    return ReturnValue;
}

static int set_timer(UINT32 playTime, UINT32 totalTime)
{
    int hours = 0, minutes = 0, seconds = 0;
   // UINT16 str_time[10] = {0};
   UINT16 str_time[6] = {0};
#if 0
    if (((playTime-((playTime/1000)*1000)) > 750) ||
        ((playTime-((playTime/1000)*1000)) < 250)) {
        playTime += 500;
    }

    playTime /= 1000;
    totalTime /= 1000;
#endif
    /** Update play time */
    hours = playTime/3600;
    minutes = (playTime-(hours*3600))/60;
    seconds = playTime-((hours*3600)+(minutes*60));
#if 0
    str_time[0] = 0x0030 + (hours/100);
    str_time[1] = 0x0030 + ((hours%100)/10);
    str_time[2] = 0x0030+(hours%10);

    str_time[3] = 0x003A;

    str_time[4] = 0x0030+(minutes/10);
    str_time[5] = 0x0030+(minutes%10);

    str_time[6] = 0x003A;

    str_time[7] = 0x0030+(seconds/10);
    str_time[8] = 0x0030+(seconds%10);

    str_time[9] = 0x0000;
#endif
//////////////////////////////////////////////////////

    str_time[0] = 0x0030+(minutes/10);
    str_time[1] = 0x0030+(minutes%10);

    str_time[2] = 0x003A;

    str_time[3] = 0x0030+(seconds/10);
    str_time[4] = 0x0030+(seconds%10);

    str_time[5] = 0x0000;
//////////////////////////////////////////////////////


#ifdef CONFIG_APP_ARD
    AppLibGraph_UpdateStringContext(AppPref_GetLanguageID(), STR_PLAY_TIME, str_time);
#else
    AppLibGraph_UpdateStringContext(0, STR_PLAY_TIME, str_time);
#endif
    AppLibGraph_UpdateString(GRAPH_CH_DUAL, GOBJ_PB_PLAY_TIME, STR_PLAY_TIME);


    /**total time*/
    hours = totalTime/3600;
    minutes = (totalTime-(hours*3600))/60;
    seconds = totalTime-((hours*3600)+(minutes*60));
#if 0
    str_time[0] = 0x0030 + (hours/100);
    str_time[1] = 0x0030 + ((hours%100)/10);
    str_time[2] = 0x0030+(hours%10);

    str_time[3] = 0x003A;

    str_time[4] = 0x0030+(minutes/10);
    str_time[5] = 0x0030+(minutes%10);

    str_time[6] = 0x003A;

    str_time[7] = 0x0030+(seconds/10);
    str_time[8] = 0x0030+(seconds%10);

    str_time[9] = 0x0000;
#endif
//////////////////////////////////////////////////////
    str_time[0] = 0x0030+(minutes/10);
    str_time[1] = 0x0030+(minutes%10);

    str_time[2] = 0x003A;

    str_time[3] = 0x0030+(seconds/10);
    str_time[4] = 0x0030+(seconds%10);

    str_time[5] = 0x0000;

/////////////////////////////////////////////////////


#ifdef CONFIG_APP_ARD
    AppLibGraph_UpdateStringContext(AppPref_GetLanguageID(), STR_TOTAL_TIME, str_time);
#else
    AppLibGraph_UpdateStringContext(0, STR_TOTAL_TIME, str_time);
#endif
    AppLibGraph_UpdateString(GRAPH_CH_DUAL, GOBJ_PB_TOTAL_TIME, STR_TOTAL_TIME);

    return 0;
}

#define STATUS_BAR_HL_TOTAL_W_0    (260)
#define STATUS_BAR_HL_TOTAL_H_0    (5)
#ifdef CONFIG_APP_ARD
#define STATUS_BAR_IDX_INIT_X_0     (150)
#define STATUS_BAR_IDX_INIT_Y_0      (490)
#define STATUS_BAR_IDX_INIT_X_0_LCD    (228)
#define STATUS_BAR_IDX_INIT_Y_0_LCD    (427)
#else
#define STATUS_BAR_IDX_INIT_X_0    (320)
#define STATUS_BAR_IDX_INIT_Y_0    (432)
#endif

static int set_status_bar(UINT32 playTime, UINT32 totalTime)
{
    UINT16 GuiSbarHLw = 0;
    UINT16 GuiSbarHLh = 0;
    UINT16 GuiSbarIdxX = 0;
    UINT16 GuiSbarIdxY = 0;

    AmbaPrintColor(GREEN,"[gui_pb_multi] set_status_bar Play Time: %d Total Time: %d", playTime, totalTime);

    if (totalTime == 0) {
        totalTime = 1;
    }
    GuiSbarHLw = (playTime*STATUS_BAR_HL_TOTAL_W_0)/totalTime;
    GuiSbarHLh = STATUS_BAR_HL_TOTAL_H_0;
    AppLibGraph_UpdateSize(GRAPH_CH_FCHAN, GOBJ_PB_STATUS_BAR_HL, GuiSbarHLw, GuiSbarHLh, 0);
    GuiSbarIdxX = STATUS_BAR_IDX_INIT_X_0 + GuiSbarHLw;
    GuiSbarIdxY = STATUS_BAR_IDX_INIT_Y_0;
    AppLibGraph_UpdatePosition(GRAPH_CH_FCHAN, GOBJ_PB_STATUS_BAR_IDX, GuiSbarIdxX, GuiSbarIdxY);

    GuiSbarHLw = (playTime*STATUS_BAR_HL_TOTAL_W_0)/totalTime;
    GuiSbarHLh = STATUS_BAR_HL_TOTAL_H_0;
    AppLibGraph_UpdateSize(GRAPH_CH_DCHAN, GOBJ_PB_STATUS_BAR_HL, GuiSbarHLw, GuiSbarHLh, 0);
#ifdef CONFIG_APP_ARD
    GuiSbarIdxX = STATUS_BAR_IDX_INIT_X_0_LCD + GuiSbarHLw;
    GuiSbarIdxY = STATUS_BAR_IDX_INIT_Y_0_LCD;
#else
    GuiSbarIdxX = STATUS_BAR_IDX_INIT_X_0 + GuiSbarHLw;
    GuiSbarIdxY = STATUS_BAR_IDX_INIT_Y_0;
#endif
    AppLibGraph_UpdatePosition(GRAPH_CH_DCHAN, GOBJ_PB_STATUS_BAR_IDX, GuiSbarIdxX, GuiSbarIdxY);

    return 0;
}

static int set_filename(WCHAR *fn, GUI_UTILITY_FILENAME_STYLE_e style)
{
    UINT16 GuiFilename[GUI_FILENAME_SIZE] = {0};

    AppGuiUtil_GetFilenameStrings((WCHAR*)GuiFilename, fn, style);
#ifdef CONFIG_APP_ARD
    AppLibGraph_UpdateStringContext(AppPref_GetLanguageID(), STR_FILENAME, GuiFilename);
#else
    AppLibGraph_UpdateStringContext(0, STR_FILENAME, GuiFilename);
#endif
    AppLibGraph_UpdateString(GRAPH_CH_DUAL, GOBJ_PB_FILENAME, STR_FILENAME);

    return 0;
}
#if 0
static int set_zoom_ratio(UINT32 zoom)
{
    UINT16 str_ratio[6] = {0};

    str_ratio[0] = 0x0078;
    if (zoom/1000) {
        str_ratio[1] = 0x0030+(zoom/1000);
    } else {
        str_ratio[1] = 0x0020;
    }
    str_ratio[2] = 0x0030+((zoom/100)%10);
    str_ratio[3] = 0x002E;
    str_ratio[4] = 0x0030+((zoom/10)%10);
    str_ratio[5] = 0x0000;
    AppLibGraph_UpdateStringContext(0, STR_ZOOM_RATIO, str_ratio);
    AppLibGraph_UpdateString(GRAPH_CH_DUAL, GOBJ_PB_ZOOM_RATIO, STR_ZOOM_RATIO);

    return 0;
}
#endif
#if 0
static int set_media_info(dec_media_info_t *media_info)
{
    switch (media_info->type) {
    case MEDIA_TYPE_MOV:
        AppLibGraph_UpdateBMP(GRAPH_CH_DUAL, GOBJ_MEDIA_INFO_RES,
                               gutil_get_video_res_bmp(media_info->media.movi.vid_width,
                                                       media_info->media.movi.vid_height));
        AppLibGraph_UpdateBMP(GRAPH_CH_DUAL, GOBJ_MEDIA_INFO_FRATE,
                               gutil_get_video_ar_frate_bmp(media_info->media.movi.aspect_ratio,
                                                            media_info->media.movi.mode,
                                                            media_info->media.movi.vid_frame_rate));
        break;
    case MEDIA_TYPE_IMG:
        AppLibGraph_UpdateBMP(GRAPH_CH_DUAL, GOBJ_MEDIA_INFO_RES, BMP_0_NULL);
        AppLibGraph_UpdateBMP(GRAPH_CH_DUAL, GOBJ_MEDIA_INFO_FRATE, BMP_0_NULL);
        break;
    default:
        AppLibGraph_UpdateBMP(GRAPH_CH_DUAL, GOBJ_MEDIA_INFO_RES, BMP_0_NULL);
        AppLibGraph_UpdateBMP(GRAPH_CH_DUAL, GOBJ_MEDIA_INFO_FRATE, BMP_0_NULL);
        break;
    }
    return 0;
}
#endif


/**
 *  @brief The GUI functions of video playback application
 *
 *  The GUI functions of video playback application
 *
 *  @param [in] guiCmd Command ID
 *  @param [in] param1 First parameter
 *  @param [in] param2 Second parameter
 *
 *  @return >=0 success, <0 failure
 */
int gui_pb_multi_func(UINT32 guiCmd, UINT32 param1, UINT32 param2)
{
    int ReturnValue = 0;

    switch (guiCmd) {
    case GUI_FLUSH:
        AppLibGraph_Draw(GRAPH_CH_DUAL);
        break;
    case GUI_HIDE_ALL:
        AppLibGraph_HideAll(GRAPH_CH_DUAL);
        break;

    case GUI_SET_LAYOUT:
        AppLibGraph_SetGUILayout(GRAPH_CH_DCHAN, Gui_Resource_Dchan_Id, Gui_Table_Dchan, 0);
        AppLibGraph_SetGUILayout(GRAPH_CH_FCHAN, Gui_Resource_Fchan_Id, Gui_Table_Fchan, 0);
        break;

    case GUI_APP_ICON_SHOW:
        //AppLibGraph_UpdateBMP(GRAPH_CH_DUAL, GOBJ_APP_ICON, BMP_BTN_MODE_PLAY);
        //AppLibGraph_Show(GRAPH_CH_DUAL, GOBJ_APP_ICON);
        break;

    case GUI_APP_ICON_HIDE:
        //AppLibGraph_Hide(GRAPH_CH_DUAL, GOBJ_APP_ICON);
        break;

    case GUI_POWER_STATE_SHOW:
        AppGuiUtil_PowerIconShow(param1, param2);
        break;

    case GUI_POWER_STATE_HIDE:
        AppGuiUtil_PowerIconHide(param1, param2);
        break;

    case GUI_POWER_STATE_UPDATE:
        AppGuiUtil_PowerIconUpdate(param1, param2);
        break;
    case GUI_CARD_SHOW:
        AppGuiUtil_CardIconShow(param1, param2);
        break;
    case GUI_CARD_HIDE:
        AppGuiUtil_CardIconHide(param1, param2);
        break;
    case GUI_CARD_UPDATE:
        AppGuiUtil_CardIconUpdate(param1);
        break;
    case GUI_PLAY_STATE_SHOW:
        AppLibGraph_Show(GRAPH_CH_DUAL, GOBJ_PB_STATUS_BAR_BASE);
        AppLibGraph_Show(GRAPH_CH_DUAL, GOBJ_PB_STATUS_BAR);
        //AppLibGraph_Show(GRAPH_CH_DUAL, GOBJ_PB_STATUS_BAR_HL);
        AppLibGraph_Show(GRAPH_CH_DUAL, GOBJ_PB_STATUS_BAR_IDX);
        AppLibGraph_Show(GRAPH_CH_DUAL, GOBJ_PB_PLAY_STATE);
#ifdef CONFIG_APP_ARD
        AppLibGraph_Show(GRAPH_CH_DUAL, GOBJ_PB_SPEED_RATIO);
#endif
        break;
    case GUI_PLAY_STATE_HIDE:
        AppLibGraph_Hide(GRAPH_CH_DUAL, GOBJ_PB_STATUS_BAR_BASE);
        AppLibGraph_Hide(GRAPH_CH_DUAL, GOBJ_PB_STATUS_BAR);
        //AppLibGraph_Hide(GRAPH_CH_DUAL, GOBJ_PB_STATUS_BAR_HL);
        AppLibGraph_Hide(GRAPH_CH_DUAL, GOBJ_PB_STATUS_BAR_IDX);
        AppLibGraph_Hide(GRAPH_CH_DUAL, GOBJ_PB_PLAY_STATE);
#ifdef CONFIG_APP_ARD
        AppLibGraph_Hide(GRAPH_CH_DUAL, GOBJ_PB_SPEED_RATIO);
#endif
        break;
    case GUI_PLAY_STATE_UPDATE:
        set_play_state(param1, param2);
        break;
    case GUI_PLAY_TIMER_SHOW:
        AppLibGraph_Show(GRAPH_CH_DUAL, GOBJ_PB_PLAY_TIME);
        AppLibGraph_Show(GRAPH_CH_DUAL, GOBJ_PB_TOTAL_TIME);
        AppLibGraph_Show(GRAPH_CH_DUAL, GOBJ_PB_PLAY_TOTAL_TIME_SEP);
        break;
    case GUI_PLAY_TIMER_HIDE:
        AppLibGraph_Hide(GRAPH_CH_DUAL, GOBJ_PB_PLAY_TIME);
        AppLibGraph_Hide(GRAPH_CH_DUAL, GOBJ_PB_TOTAL_TIME);
        AppLibGraph_Hide(GRAPH_CH_DUAL, GOBJ_PB_PLAY_TOTAL_TIME_SEP);
        break;
    case GUI_PLAY_TIMER_UPDATE:
        set_status_bar(param1, param2);
        set_timer(param1, param2);
        break;
    case GUI_PLAY_SPEED_SHOW:
        AppLibGraph_Show(GRAPH_CH_DUAL, GOBJ_PB_SPEED_RATIO);
        break;
    case GUI_PLAY_SPEED_HIDE:
        AppLibGraph_Hide(GRAPH_CH_DUAL, GOBJ_PB_SPEED_RATIO);
        break;
    case GUI_FILENAME_SHOW:
        AppLibGraph_Show(GRAPH_CH_DUAL, GOBJ_PB_FILENAME);
        break;
    case GUI_FILENAME_HIDE:
        AppLibGraph_Hide(GRAPH_CH_DUAL, GOBJ_PB_FILENAME);
        break;
    case GUI_FILENAME_UPDATE:
        set_filename((WCHAR *)param1, (GUI_UTILITY_FILENAME_STYLE_e)param2);
        break;
    case GUI_MEDIA_INFO_SHOW:
        AppLibGraph_Show(GRAPH_CH_FCHAN, GOBJ_MEDIA_INFO_RES);
        AppLibGraph_Show(GRAPH_CH_FCHAN, GOBJ_MEDIA_INFO_FRATE);
        break;
    case GUI_MEDIA_INFO_HIDE:
        AppLibGraph_Hide(GRAPH_CH_FCHAN, GOBJ_MEDIA_INFO_RES);
        AppLibGraph_Hide(GRAPH_CH_FCHAN, GOBJ_MEDIA_INFO_FRATE);
        break;
    case GUI_MEDIA_INFO_UPDATE:
        //set_media_info(param2);
        break;
    case GUI_ZOOMBAR_SHOW:
    case GUI_ZOOMBAR_HIDE:
    case GUI_ZOOMBAR_UPDATE:
        break;
    case GUI_WARNING_SHOW:
        AppLibGraph_Show(GRAPH_CH_DUAL, GOBJ_WARNING);
        break;
    case GUI_WARNING_HIDE:
        AppLibGraph_Hide(GRAPH_CH_DUAL, GOBJ_WARNING);
        break;
    case GUI_WARNING_UPDATE:
        AppLibGraph_UpdateString(GRAPH_CH_DUAL, GOBJ_WARNING, GuiWarningTable[param1].str);
#ifdef CONFIG_APP_ARD
        AppLibGraph_UpdateColor(GRAPH_CH_DUAL, GOBJ_WARNING, COLOR_RED, COLOR_TEXT_BORDER );
#endif
        break;
    default:
        ReturnValue = -1;
        break;
    }

    return ReturnValue;
}
