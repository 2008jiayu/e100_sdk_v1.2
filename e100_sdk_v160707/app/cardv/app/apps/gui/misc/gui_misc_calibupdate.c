/**
  * @file src/app/apps/gui/misc/connectedcam/gui_misc_calibupdate.c
  *
  *  Implementation of Firmware Update GUI display flows
  *
  * History:
  *    2014/06/26 - [Annie Ting] created file
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

#include <apps/gui/misc/gui_misc_calibupdate.h>
#include <wchar.h>
#include <apps/gui/resource/gui_settle.h>

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
int gui_misc_calibupdate_func(UINT32 guiCmd, UINT32 param1, UINT32 param2)
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
    default:
        AmbaPrint("[gui_misc_calibupdate] Undefined GUI command");
        ReturnValue = -1;
        break;
    }

    return ReturnValue;
}
