/**
  * @file src/app/apps/flow/widget/widgetmgt.h
  *
  * Header of Widget Management
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
#ifndef APP_WIDGETMGT_H_
#define APP_WIDGETMGT_H_

#include <apps/apps.h>
__BEGIN_C_PROTO__

/*************************************************************************
 * Widget Definitions
 ************************************************************************/
#define WIDGET_NONE        (0x00000000)
#define WIDGET_MENU        (0x00000001)
#define WIDGET_MENU_QUICK    (0x00000002)
#define WIDGET_MENU_ADJ        (0x00000004)
#define WIDGET_MENU_TIME    (0x00000008)
#define WIDGET_DIALOG    (0x00000010)
#define WIDGET_MENU_ADAS_CALIB    (0x00000020)
#define WIDGET_INFO_PROMPT    (0x00000040) // #ifdef CONFIG_APP_ARD 
#define WIDGET_MENU_DRIVER_ID	(0x00000080)
#define WIDGET_MENU_CKBX 	(0x00000100)
#define WIDGET_ALL        (0xFFFFFFFF)

typedef struct _WIDGET_ITEM_s_ {
    int (*on)(UINT32 param);
    int (*off)(UINT32 param);
    int (*OnMessage)(UINT32 msg, UINT32 param1, UINT32 param2);
} WIDGET_ITEM_s;

/*************************************************************************
 * Widget APIs
 ************************************************************************/
extern int AppWidget_Init(void);
extern int AppWidget_GetCur(void);

extern int AppWidget_On(UINT32 widgetId, UINT32 param);
extern int AppWidget_Off(UINT32 widgetId, UINT32 param);
#define WIDGET_HIDE_SILENT    (0x80000000)
extern int AppWidget_OnMessage(UINT32 msg, UINT32 param1, UINT32 param2);
#define WIDGET_PASSED_MSG    (10)
extern UINT32 AppWidget_GetFlags(void);
extern void AppWidget_SetPback(UINT32 value);
extern UINT32 AppWidget_GetPback(void);

__END_C_PROTO__

#endif /* APP_WIDGETMGT_H_ */
