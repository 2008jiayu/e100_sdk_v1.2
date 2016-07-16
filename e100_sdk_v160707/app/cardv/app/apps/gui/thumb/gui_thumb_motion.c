/**
  * @file src/app/apps/gui/pb/connectedcam/gui_thumb_motion.c
  *
  *  Implementation of photo playback GUI display flows
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
#include <apps/gui/thumb/gui_thumb_motion.h>
#include <apps/gui/resource/gui_settle.h>
#include <wchar.h>
#ifdef CONFIG_APP_ARD
#include <system/app_pref.h>
#endif


#define GOBJ_MEDIA_INFO_RES        (GOBJ_FV_ICON_LEFT_1)
#define GOBJ_MEDIA_INFO_FRATE    (GOBJ_FV_ICON_LEFT_2)


static int set_filename(WCHAR *fn, GUI_UTILITY_FILENAME_STYLE_e style)
{
    UINT16 GuiFilename[GUI_FILENAME_SIZE] = {0};

    AppGuiUtil_GetFilenameStrings((WCHAR*)GuiFilename, fn, style);
#ifdef CONFIG_APP_ARD
    AppLibGraph_UpdateStringContext(AppPref_GetLanguageID(), STR_FILENAME, GuiFilename);
#else
    AppLibGraph_UpdateStringContext(0, STR_FILENAME, GuiFilename);
#endif
    AppLibGraph_UpdateString(GRAPH_CH_DUAL, GOBJ_THUMB_FILENAME, STR_FILENAME);

    return 0;
}

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
 *  @param [in] param1 first parameter
 *  @param [in] param2 second parameter
 *
 *  @return >=0 success, <0 failure
 */
static int TabHighlight = -1;

int gui_thumb_motion_func(UINT32 guiCmd, UINT32 param1, UINT32 param2)
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
        AppLibGraph_UpdateBMP(GRAPH_CH_DUAL, GOBJ_APP_ICON, BMP_BTN_MODE_THUMBNAIL);
        AppLibGraph_Show(GRAPH_CH_DUAL, GOBJ_APP_ICON);
        break;
    case GUI_APP_ICON_HIDE:
        AppLibGraph_Hide(GRAPH_CH_DUAL, GOBJ_APP_ICON);
        break;
    case GUI_FRAME_SHOW:
        AppLibGraph_Show(GRAPH_CH_DCHAN, GOBJ_THUMB_SEP_UP);
        //AppLibGraph_Show(GRAPH_CH_DCHAN, GOBJ_THUMB_SEP_LEFT);
        //AppLibGraph_Show(GRAPH_CH_DCHAN, GOBJ_THUMB_GLOW_LEFT);
        //AppLibGraph_Show(GRAPH_CH_DCHAN, GOBJ_THUMB_GLOW_RIGHT);
        break;
    case GUI_FRAME_HIDE:
        AppLibGraph_Hide(GRAPH_CH_DCHAN, GOBJ_THUMB_SEP_UP);
        //AppLibGraph_Hide(GRAPH_CH_DCHAN, GOBJ_THUMB_SEP_LEFT);
        //AppLibGraph_Hide(GRAPH_CH_DCHAN, GOBJ_THUMB_GLOW_LEFT);
        //AppLibGraph_Hide(GRAPH_CH_DCHAN, GOBJ_THUMB_GLOW_RIGHT);
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
    case GUI_WARNING_SHOW:
        AppLibGraph_Show(GRAPH_CH_DUAL, GOBJ_WARNING);
        break;
    case GUI_WARNING_HIDE:
        AppLibGraph_Hide(GRAPH_CH_DUAL, GOBJ_WARNING);
        break;
    case GUI_WARNING_UPDATE:
        AppLibGraph_UpdateString(GRAPH_CH_DUAL, GOBJ_WARNING, GuiWarningTable[param1].str);
#ifdef CONFIG_APP_ARD
        //if(param2 == 1) {
        AppLibGraph_UpdateColor(GRAPH_CH_DUAL, GOBJ_WARNING, COLOR_RED, COLOR_TEXT_BORDER );
        //} else {
        //    AppLibGraph_UpdateColor(GRAPH_CH_DUAL, GOBJ_WARNING, COLOR_LIGHTGRAY, COLOR_TEXT_BORDER);//COLOR_TEXT_BORDER, COLOR_WHITE);
        //}
#endif
        break;
    case GUI_TAB_SHOW:
#ifdef CONFIG_APP_ARD
        AppLibGraph_Show(GRAPH_CH_DCHAN, GOBJ_THUMB_2_TAB_BASE1);
        AppLibGraph_Show(GRAPH_CH_DCHAN, GOBJ_THUMB_2_TAB_BASE2);

#else
        AppLibGraph_Show(GRAPH_CH_DCHAN, GOBJ_THUMB_3_TAB_BASE);
#endif
        if (TabHighlight > 0) {
            AppLibGraph_Show(GRAPH_CH_DCHAN, TabHighlight);
        }
        break;
    case GUI_TAB_HIDE:
#ifdef CONFIG_APP_ARD
        AppLibGraph_Hide(GRAPH_CH_DCHAN, GOBJ_THUMB_2_TAB_BASE1);
        AppLibGraph_Hide(GRAPH_CH_DCHAN, GOBJ_THUMB_2_TAB_BASE2);

#else
        AppLibGraph_Hide(GRAPH_CH_DCHAN, GOBJ_THUMB_3_TAB_BASE);
#endif
        if (TabHighlight > 0) {
            AppLibGraph_Hide(GRAPH_CH_DCHAN, TabHighlight);
        }
        break;
    case GUI_TAB_UPDATE:
        if (TabHighlight > 0) {
            AppLibGraph_Hide(GRAPH_CH_DCHAN, TabHighlight);
        }
#ifdef CONFIG_APP_ARD
        TabHighlight = GOBJ_THUMB_2_TAB_HL_2 - param1;
#else
        TabHighlight = GOBJ_THUMB_3_TAB_HL_1 + param1;
#endif

#ifdef CONFIG_APP_ARD
        if(param1)
        AppLibGraph_UpdateBMP(GRAPH_CH_DUAL, TabHighlight, BMP_ICN_THUMB_HL_VIDEO_EMERGENCY);
        else
        AppLibGraph_UpdateBMP(GRAPH_CH_DUAL, TabHighlight, BMP_ICN_THUMB_HL_MIX);
#endif

        if (TabHighlight > 0) {
            AppLibGraph_Show(GRAPH_CH_DCHAN, TabHighlight);
        }
        break;
    case GUI_DEL_SHOW:
    case GUI_DEL_HIDE:
    case GUI_DEL_UPDATE:
        break;
    case GUI_PROTECT_SHOW:
    case GUI_PROTECT_HIDE:
    case GUI_PROTECT_UPDATE:
        break;
    case GUI_FILENAME_SHOW:
        AppLibGraph_Show(GRAPH_CH_DUAL, GOBJ_THUMB_FILENAME);
        break;
    case GUI_FILENAME_HIDE:
        AppLibGraph_Hide(GRAPH_CH_DUAL, GOBJ_THUMB_FILENAME);
        break;
    case GUI_THUMB_SELECT_SHOW:
        AppLibGraph_Show(GRAPH_CH_DUAL, GOBJ_THUMB_ITEM_0 + param1);
        break;
    case GUI_THUMB_SELECT_HIDE:
        AppLibGraph_Hide(GRAPH_CH_DUAL, GOBJ_THUMB_ITEM_0 + param1);
        break;		
    case GUI_MODE_NAME_SHOW:
        AppLibGraph_Show(GRAPH_CH_DUAL,GOBJ_THUMB_MODE_NAME);
        break;
    case GUI_MODE_NAME_HIDE:
        AppLibGraph_Hide(GRAPH_CH_DUAL,GOBJ_THUMB_MODE_NAME);
        break;
    case GUI_MODE_NAME_UPDATE:
	  if(param1)	
            AppLibGraph_UpdateString(GRAPH_CH_DUAL, GOBJ_THUMB_MODE_NAME,STR_THUMB_MOTION_EVENT);
	  else
            AppLibGraph_UpdateString(GRAPH_CH_DUAL, GOBJ_THUMB_MODE_NAME,STR_THUMB_MOTION_NORMAL);	  	
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
    case GUI_PHOTO_SIZE_SHOW:
    case GUI_PHOTO_SIZE_HIDE:
    case GUI_PHOTO_SIZE_UPDATE:
    case GUI_VIDEO_SENSOR_RES_SHOW:
    case GUI_VIDEO_SENSOR_RES_HIDE:
    case GUI_VIDEO_SENSOR_RES_UPDATE:
        break;
    default:
        ReturnValue = -1;
        break;
    }

    return ReturnValue;
}
