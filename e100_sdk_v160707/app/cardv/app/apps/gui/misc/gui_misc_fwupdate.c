/**
  * @file src/app/apps/gui/misc/connectedcam/gui_misc_fwupdate.c
  *
  *  Implementation of Firmware Update GUI display flows
  *
  * History:
  *    2014/03/20 - [Martin Lai] created file
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

#include <apps/gui/misc/gui_misc_fwupdate.h>
#include <wchar.h>
#include <apps/gui/resource/gui_settle.h>
#ifdef CONFIG_APP_ARD
#include <system/app_pref.h>
#endif

/**
 * @brief Show the progress of each partition during updating firmware.
 *
 * @param percentage - The percentage of progress.
 */
static int set_fwupdate_ratio(int percentage)
{
#if 0
    UINT16 str_ratio[5] = {0};

    if (percentage/100) {
        str_ratio[0] = 0x0030+(percentage/100);
    } else {
        str_ratio[0] = 0x0020;    //SPACE
    }
    if (percentage/10) {
        str_ratio[1] = 0x0030+((percentage/10)%10);
    } else {
        str_ratio[1] = 0x0020;    //SPACE
    }
    str_ratio[2] = 0x0030+(percentage%10);
    str_ratio[3] = 0x0025;    //%
    str_ratio[4] = 0x0000;

    AppLibGraph_UpdateStringContext(0, STR_FWUPDATE_RATIO, str_ratio);
    AppLibGraph_UpdateString(GRAPH_CH_DUAL, GOBJ_FWUPDATE_RATIO, STR_FWUPDATE_RATIO);
#endif
    return 0;
}

/**
 * @brief Show the rest of partitions during updating firmware.
 *
 * @param present - The present updating partition.
 * @param totalPart - The total partitions.
 */
static int set_fwupdate_stage(int present, int totalPart)
{
    UINT16 str_stage[10] = {'s','t','a','g','e',' ','0','/','6','\0'};

    str_stage[6] = 0x0030+present;
    str_stage[8] = 0x0030+totalPart;

#ifdef CONFIG_APP_ARD
    AppLibGraph_UpdateStringContext(AppPref_GetLanguageID(), STR_FWUPDATE_STAGE, str_stage);
#else
    AppLibGraph_UpdateStringContext(0, STR_FWUPDATE_STAGE, str_stage);
#endif
    AppLibGraph_UpdateString(GRAPH_CH_DUAL, GOBJ_FWUPDATE_STAGE, STR_FWUPDATE_STAGE);

    return 0;
}

/**
 *  @brief The GUI functions of application
 *
 *  The GUI functions of application
 *
 *  @param [in] guiCmd Command ID
 *  @param [in] param1 First parameter
 *  @param [in] param2 Second parameter
 *
 *  @return >=0 success, <0 failure
 */
int gui_misc_fwupdate_func(UINT32 guiCmd, UINT32 param1, UINT32 param2)
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
    case GUI_FWUPDATE_RATIO_SHOW:
        AppLibGraph_Show(GRAPH_CH_DUAL, GOBJ_FWUPDATE_RATIO);
        break;
    case GUI_FWUPDATE_RATIO_UPDATE:
        set_fwupdate_ratio(param1);
        break;
    case GUI_FWUPDATE_RATIO_HIDE:
        AppLibGraph_Hide(GRAPH_CH_DUAL, GOBJ_FWUPDATE_RATIO);
        break;
    case GUI_FWUPDATE_STAGE_SHOW:
        AppLibGraph_Show(GRAPH_CH_DUAL, GOBJ_FWUPDATE_STAGE);
        break;
    case GUI_FWUPDATE_STAGE_UPDATE:
        set_fwupdate_stage(param1, param2);
        break;
    case GUI_FWUPDATE_STAGE_HIDE:
        AppLibGraph_Hide(GRAPH_CH_DUAL, GOBJ_FWUPDATE_STAGE);
        break;
    default:
        AmbaPrint("[gui_misc_fwupdate] Undefined GUI command");
        ReturnValue = -1;
        break;
    }

    return ReturnValue;
}
