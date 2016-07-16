/**
  * @file app/cardv/app/apps/gui/widget/info_prompt/guiinfo_prompt.c
  *
  *  Implementation for Info Prompt GUI flow
  *
  * History:
  *    2015/2/5 - [Emmett Xie] created file
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
#include <apps/gui/widget/info_prompt/gui_info_prompt.h>
#include <apps/gui/resource/gui_settle.h>

int gui_info_prompt_func(UINT32 guiCmd, UINT32 param1, UINT32 param2)
{
    int rval = 0;

    switch (guiCmd) {
    case GUI_RESET:
        break;
    case GUI_INFO_PROMPT_BG_SHOW:
        AppLibGraph_Show(GRAPH_CH_DUAL, GOBJ_INFO_PROMPT_BG);
        break;
    case GUI_INFO_PROMPT_BG_HIDE:
        AppLibGraph_Hide(GRAPH_CH_DUAL, GOBJ_INFO_PROMPT_BG);
        break;
    case GUI_INFO_PROMPT_BG_STR_SHOW:
        AppLibGraph_Show(GRAPH_CH_DUAL, GOBJ_INFO_PROMPT_BG_STR_ITEM0);
        AppLibGraph_Show(GRAPH_CH_DUAL, GOBJ_INFO_PROMPT_BG_STR_ITEM1);
        AppLibGraph_Show(GRAPH_CH_DUAL, GOBJ_INFO_PROMPT_BG_STR_ITEM2);
        AppLibGraph_Show(GRAPH_CH_DUAL, GOBJ_INFO_PROMPT_BG_STR_ITEM3);
        break;
    case GUI_INFO_PROMPT_BG_STR_HIDE:
        AppLibGraph_Hide(GRAPH_CH_DUAL, GOBJ_INFO_PROMPT_BG_STR_ITEM0);
        AppLibGraph_Hide(GRAPH_CH_DUAL, GOBJ_INFO_PROMPT_BG_STR_ITEM1);
        AppLibGraph_Hide(GRAPH_CH_DUAL, GOBJ_INFO_PROMPT_BG_STR_ITEM2);
        AppLibGraph_Hide(GRAPH_CH_DUAL, GOBJ_INFO_PROMPT_BG_STR_ITEM3);
        break;
    case GUI_HIDE_ALL:
        AppLibGraph_HideAll(GRAPH_CH_DUAL);
        break;
    case GUI_FLUSH:
        AppLibGraph_Draw(GRAPH_CH_DUAL);
        break;
    default:
        AmbaPrint("[gui_dialog] Undefined GUI command");
        rval = -1;
        break;
    }

    return rval;
}

