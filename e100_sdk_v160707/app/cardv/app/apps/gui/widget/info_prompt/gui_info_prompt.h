/**
  * @file app/cardv/app/apps/gui/widget/info_prompt/guiinfo_prompt.h
  *
  *  Header for Info Prompt GUI flow
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

#ifndef APP_GUI_WIDGET_INFO_PROMT_H_
#define APP_GUI_WIDGET_INFO_PROMT_H_
      
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <AmbaDataType.h>
#include <applib.h>
      
__BEGIN_C_PROTO__

/*************************************************************************
* Info prompt GUI definitions
************************************************************************/
typedef enum _GUI_INFO_PROMPT_CMD_ID_e_ {
  GUI_FLUSH = 0,
  GUI_HIDE_ALL,
  GUI_RESET,
  GUI_INFO_PROMPT_BG_SHOW,
  GUI_INFO_PROMPT_BG_HIDE,
  GUI_INFO_PROMPT_BG_STR_SHOW,
  GUI_INFO_PROMPT_BG_STR_HIDE,
} GUI_INFO_PROMPT_CMD_ID_e;
/*************************************************************************
* Info prompt Widget GUI functions
************************************************************************/
extern int gui_info_prompt_func(UINT32 guiCmd, UINT32 param1, UINT32 param2);

__END_C_PROTO__
      
#endif /* APP_GUI_WIDGET_INFO_PROMT_H_ */

