/**
  * @file src/app/apps/gui/widget/menu/connectedcam/gui_menu_time.c
  *
  *  Implementation for Time Menu GUI flow
  *
  * History:
  *    2013/11/22 - [Martin Lai] created file
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

#include <apps/apps.h>
#include <system/app_util.h>
#include <apps/gui/widget/menu/gui_menu_adas_calib.h>
#include <apps/gui/resource/gui_settle.h>

int gui_menu_adas_calib_func(UINT32 gui_cmd, UINT32 param1, UINT32 param2)
{
    int rval = 0;

    switch (gui_cmd) {
    case GUI_HIDE_ADAS_CALIB_SKY:
        AppLibGraph_Hide(GRAPH_CH_DCHAN, GOBJ_ADAS_CALIB_SKY);
        break;
    case GUI_HIDE_ADAS_CALIB_HOOD:
        AppLibGraph_Hide(GRAPH_CH_DCHAN, GOBJ_ADAS_CALIB_HOOD);
        break;
    case GUI_SHOW_ADAS_CALIB_SKY:
        AppLibGraph_Show(GRAPH_CH_DCHAN, GOBJ_ADAS_CALIB_SKY);
        break;
    case GUI_SHOW_ADAS_CALIB_HOOD:
        AppLibGraph_Show(GRAPH_CH_DCHAN, GOBJ_ADAS_CALIB_HOOD);
        break;
    case GUI_UPDATE_ADAS_CALIB_SKY:
        AppLibGraph_UpdatePosition(GRAPH_CH_DCHAN, GOBJ_ADAS_CALIB_SKY, param1, param2);
        break;
    case GUI_UPDATE_ADAS_CALIB_HOOD:
        AppLibGraph_UpdatePosition(GRAPH_CH_DCHAN, GOBJ_ADAS_CALIB_HOOD, param1, param2);
        break;
    case GUI_HIDE_ALL:
        AppLibGraph_HideAll(GRAPH_CH_DUAL);
        break;
    case GUI_FLUSH:
        AppLibGraph_Draw(GRAPH_CH_DUAL);
        break;
    default:
        AmbaPrint("Undefined GUI command");
        rval = -1;
        break;
    }

    return rval;
}
