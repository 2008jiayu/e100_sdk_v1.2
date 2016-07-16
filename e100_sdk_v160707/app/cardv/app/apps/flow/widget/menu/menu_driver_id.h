/**
 * system/src/app/apps/flow/widget/menu/menu_driver_id.h
 *
 * Header of Driver Id Menu
 *
 * History:
 *	  2010/08/11 - [Jili Kuang] created file
 *
 *
 * Copyright (c) 2015 Ambarella, Inc.
 *
 * This file and its contents (��Software��) are protected by intellectual property rights
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

#ifndef __APP_WIDGET_MENU_DRIVER_ID_H__
#define __APP_WIDGET_MENU_DRIVER_ID_H__

#include <apps/flow/widget/widgetmgt.h>

__BEGIN_C_PROTO__

#define DRIVER_ID_NUM_LENGTH 9

/*************************************************************************
 * Driver Id Menu definitions
 ************************************************************************/

/*************************************************************************
 * Driver Id Menu APIs for widget management
 ************************************************************************/
extern WIDGET_ITEM_s* app_menu_driver_id_get_widget(void);

/*************************************************************************
 * Public Driver Id Menu Widget APIs
 ************************************************************************/
extern int app_widget_menu_driver_id_set_item(UINT32 tab_id, UINT32 item_id);

extern char* app_widget_menu_driver_id_get_num_string(void);

__END_C_PROTO__

#endif /* __APP_WIDGET_MENU_DRIVER_ID_H__ */
