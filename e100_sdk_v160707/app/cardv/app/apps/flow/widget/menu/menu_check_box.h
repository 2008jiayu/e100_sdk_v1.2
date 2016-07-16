/**
 * system/src/app/apps/flow/widget/menu/menu_check_box.h
 *
 * Header of Check Box Menu
 *
 * History:
 *	  2014/01/10 - [David Li] created file
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

#ifndef __APP_WIDGET_MENU_CHECK_BOX_H__
#define __APP_WIDGET_MENU_CHECK_BOX_H__

#include <apps/flow/widget/widgetmgt.h>

__BEGIN_C_PROTO__


/*************************************************************************
 * Check Box Menu definitions
 ************************************************************************/
typedef int (*Check_Box_Set_Func)(int);

typedef struct _CHECK_BOX_ITEM_S_ {
	int item_id;
	int string_id;
	int check_or_not;
} CHECK_BOX_ITEM_S;

/*************************************************************************
 * Check Box Menu APIs for widget management
 ************************************************************************/
extern WIDGET_ITEM_s* app_menu_ckbx_get_widget(void);

/*************************************************************************
 * Public Check Box Menu Widget APIs
 ************************************************************************/
extern int app_widget_menu_check_box_set_item(UINT32 tab_id, UINT32 item_id);

extern int app_widget_check_box_init_item( int total_item, CHECK_BOX_ITEM_S * p_item ,Check_Box_Set_Func set_func);

__END_C_PROTO__

#endif /* __APP_WIDGET_MENU_CHECK_BOX_H__ */

