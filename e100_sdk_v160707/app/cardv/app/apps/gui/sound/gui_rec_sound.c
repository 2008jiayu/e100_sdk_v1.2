/**
  * @file src/app/apps/gui/sound/gui_rec_sound.c
  *
  *  Implementation of Sport Recorder (sensor) GUI display flowsx
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
#include <apps/gui/sound/gui_rec_sound.h>
#include <apps/gui/resource/gui_resource.h>
#include <apps/gui/resource/gui_settle.h>
#include <wchar.h>

static int set_rec_state(int state)
{
    switch (state) {
    case GUI_REC_START:
        AppLibGraph_UpdateBMP(GRAPH_CH_DUAL, GOBJ_REC_STATE, BMP_ICN_VIDEO_REC);
        break;
    case GUI_REC_PAUSED:
        AppLibGraph_UpdateBMP(GRAPH_CH_DUAL, GOBJ_REC_STATE, BMP_ICN_VIDEO_REC_PAUSE);
        break;
    default:
        AppLibGraph_UpdateBMP(GRAPH_CH_DUAL, GOBJ_REC_STATE, 0);
        break;
    }
    return 0;
}

static int set_rec_timer(UINT32 time)
{
    WCHAR RecStrTimer[10] = {'0','0','0',':','0','0',':','0','0','\0'};
    int Hours = 0, Minutes = 0, Seconds = 0;
    Hours = time/3600;
    Minutes = (time-(Hours*3600))/60;
    Seconds = time-((Hours*3600)+(Minutes*60));

    RecStrTimer[0] = 0x0030 + (Hours/100);
    RecStrTimer[1] = 0x0030 + ((Hours%100)/10);
    RecStrTimer[2] = 0x0030 + (Hours%10);

    RecStrTimer[3] = 0x003A;

    RecStrTimer[4] = 0x0030 + (Minutes/10);
    RecStrTimer[5] = 0x0030 + (Minutes%10);

    RecStrTimer[6] = 0x003A;

    RecStrTimer[7] = 0x0030 + (Seconds/10);
    RecStrTimer[8] = 0x0030 + (Seconds%10);

    RecStrTimer[9] = 0x0000;
#ifdef CONFIG_APP_ARD
    AppLibGraph_UpdateStringContext(AppPref_GetLanguageID(), STR_REC_TIME, RecStrTimer);
#else
    AppLibGraph_UpdateStringContext(0, STR_REC_TIME, RecStrTimer);
#endif

    AppLibGraph_UpdateString(GRAPH_CH_DUAL, GOBJ_REC_TIME, STR_REC_TIME);

    return 0;
}
/**
 *  @brief The GUI functions of recorder application
 *
 *  The GUI functions of recorder application
 *
 *  @param [in] guiCmd Command ID
 *  @param [in] param1 First parameter
 *  @param [in] param2 Second parameter
 *
 *  @return >=0 success, <0 failure
 */
int gui_rec_sound_func(UINT32 guiCmd, UINT32 param1, UINT32 param2)
{
    int ReturnValue = 0;

    switch (guiCmd) {
    case GUI_FLUSH:
        AppLibGraph_Draw(GRAPH_CH_DUAL);
        AppLibGraph_Draw(GRAPH_CH_BLEND);
        break;
    case GUI_HIDE_ALL:
        AppLibGraph_HideAll(GRAPH_CH_DUAL);
        break;
    case GUI_SET_LAYOUT:
        AppLibGraph_SetGUILayout(GRAPH_CH_DCHAN, Gui_Resource_Dchan_Id, Gui_Table_Dchan, 0);
        AppLibGraph_SetGUILayout(GRAPH_CH_FCHAN, Gui_Resource_Fchan_Id, Gui_Table_Fchan, 0);
        AppLibGraph_SetGUILayout(GRAPH_CH_BLEND, Gui_Resource_Blend_Id, Gui_Table_Blend, 0);
        break;
    case GUI_APP_SOUND_ICON_SHOW:
        AppLibGraph_UpdateBMP(GRAPH_CH_DUAL, GOBJ_APP_ICON, BMP_BTN_MIC_ON);
        AppLibGraph_Show(GRAPH_CH_DUAL, GOBJ_APP_ICON);
        break;
    case GUI_APP_ICON_HIDE:
        AppLibGraph_Hide(GRAPH_CH_DUAL, GOBJ_APP_ICON);
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
        AppLibGraph_Show(GRAPH_CH_DCHAN, GOBJ_WARNING);
        break;
    case GUI_WARNING_HIDE:
        AppLibGraph_Hide(GRAPH_CH_DCHAN, GOBJ_WARNING);
        break;
    case GUI_WARNING_UPDATE:
        AppLibGraph_UpdateString(GRAPH_CH_DCHAN, GOBJ_WARNING, GuiWarningTable[param1].str);
#ifdef CONFIG_APP_ARD
        AppLibGraph_UpdateColor(GRAPH_CH_DUAL, GOBJ_WARNING, COLOR_RED, COLOR_TEXT_BORDER );
#endif
        break;
    case GUI_REC_STATE_SHOW:
        AppLibGraph_Show(GRAPH_CH_DUAL, GOBJ_REC_STATE);
        break;
    case GUI_REC_STATE_HIDE:
        AppLibGraph_Hide(GRAPH_CH_DUAL, GOBJ_REC_STATE);
        break;
    case GUI_REC_STATE_UPDATE:
        set_rec_state(param1);
        break;
    case GUI_REC_TIMER_SHOW:
        AppLibGraph_Show(GRAPH_CH_DUAL, GOBJ_REC_TIME);
        break;
    case GUI_REC_TIMER_HIDE:
        AppLibGraph_Hide(GRAPH_CH_DUAL, GOBJ_REC_TIME);
        break;
    case GUI_REC_TIMER_UPDATE:
        set_rec_timer(param1);
        break;
    default:
        ReturnValue = -1;
        break;
    }

    return ReturnValue;
}
