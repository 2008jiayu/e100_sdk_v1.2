/**
  * @file src/app/apps/gui/resource/connectedcam/gui_settle.h
  *
  * Header for GUI object
  *
  * History:
  *    2013/09/23 - [Martin Lai] created file
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
#ifndef APP_GUI_SETTLE_H_
#define APP_GUI_SETTLE_H_

#include "gui_resource.h"
#include "gui_table.h"

__BEGIN_C_PROTO__


/*************************************************************************
 * GUI layout size define
 ************************************************************************/


/*************************************************************************
 * GUI layout settings
 ************************************************************************/
typedef enum _GUI_LAYOUT_ID_e_ {
    GUI_LAYOUT_960x480 = 0,
    GUI_LAYOUT_480x960,
    GUI_LAYOUT_1920x135_TV,
    GUI_LAYOUT_960x540_TV,
    GUI_LAYOUT_BLEND,
    GUI_LAYOUT_NUM
} GUI_LAYOUT_ID_e;

/* Warning message id */
typedef enum _GUI_WARNING_ID_e_ {
    GUI_WARNING_PROCESSING = 0,
    GUI_WARNING_LOADING,
    GUI_WARNING_RECOVERING,
    GUI_WARNING_NO_FILES,
    GUI_WARNING_MEM_RUNOUT,
    GUI_WARNING_NO_CARD,
    GUI_WARNING_CARD_PROTECTED,
    GUI_WARNING_CARD_FULL,
    GUI_WARNING_CARD_VOLUME_LIMIT,
    GUI_WARNING_FILE_LIMIT,
    GUI_WARNING_PHOTO_LIMIT,
#ifdef CONFIG_APP_ARD
    GUI_WARNING_PHOTO_LIMIT_2,
#endif
    GUI_WARNING_INDEX_FILE_LIMIT,
    GUI_WARNING_FILE_INDEX_LIMIT,
    GUI_WARNING_FILE_AMOUNT_LIMIT,
    GUI_WARNING_STORAGE_IO_ERROR,
    GUI_WARNING_PRE_RECORDING_NOT_PERMIT,
    GUI_WARNING_CANT_GET_AP,
    GUI_WARNING_PIV_BUSY,
    GUI_WARNING_PIV_DISALLOWED,
    GUI_WARNING_PIV_ERROR,
    GUI_WARNING_LDWS_EVENT,
    GUI_WARNING_FCWS_EVENT,
    GUI_WARNING_FCMD_EVENT,
    GUI_WARNING_LLWS_EVENT,
    GUI_WARNING_MD_EVENT,
#ifdef CONFIG_APP_ARD
    GUI_WARNING_LOW_VOLTAGE,
    GUI_WARNING_EVENT_FULL,
    GUI_WARNING_EVENT_FULL_2,
    GUI_WARNING_VIDEO_CLOSE,
    GUI_WARNING_MODE_BUSY,
    GUI_WARNING_CARD_UNFORMATED,
    GUI_WARNING_SYSTEM_TIME_ABNORMAL,
    GUI_WARNING_COMMUNICATIONS_TEST,
    GUI_WARNING_SYSREM_UPGRATE_PROCESSING,
    GUI_WARNING_SYSREM_UPGRATE_SUCCESS,
#endif
    GUI_WARNING_MSG_NUM
} GUI_WARNING_ID_e;

typedef struct _GUI_WARNING_DATA_s_ {
    UINT32 id;
    UINT32 str;
    UINT32 bmp;
} GUI_WARNING_DATA_s;

extern GUI_WARNING_DATA_s GuiWarningTable[];

extern int Gui_Resource_Fchan_Id;
extern int Gui_Resource_Dchan_Id;
extern int Gui_Resource_Blend_Id;
extern APPLIB_GRAPHIC_UIOBJ_s **Gui_Table_Fchan;
extern APPLIB_GRAPHIC_UIOBJ_s **Gui_Table_Dchan;
extern APPLIB_GRAPHIC_UIOBJ_s **Gui_Table_Blend;

extern int AppGui_Init(void);

__END_C_PROTO__

#endif /* APP_GUI_SETTLE_H_ */
