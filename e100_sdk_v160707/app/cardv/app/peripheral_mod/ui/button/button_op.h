/**
  * @file src/app/peripheral_mod/ui/button/button_op.h
  *
  * Header of Button Operation - APP level
  *
  * History:
  *    2013/09/09 - [Martin Lai] created file
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

#ifndef APP_BUTTON_OP_H_
#define APP_BUTTON_OP_H_

#include <applib.h>

__BEGIN_C_PROTO__

/*************************************************************************
 * Button OP definitons
 ************************************************************************/
#if defined(CONFIG_APP_ARD)
typedef enum {
	BTN_IS_MENU_UP_DOWN_MODE = 0,
	BTN_IS_MENU_LEFT_RIGHT_MODE,
	BTN_IS_MENU_RIGHT_LEFT_MODE,
	BTN_IS_MENU_UP_RIGHT_MODE,
	BTN_IS_REC_MODE,
	BTN_IS_PHOTO_MODE,
	BTN_IS_PB_MODE,
	BTN_MODE_NUM
}UI_BUTTON_MODE;

typedef struct {
	UINT32 bid;
	UINT32 new_bid[BTN_MODE_NUM];
}KEY_REMAP_TBL;

extern void app_button_set_special_mode(UI_BUTTON_MODE mode);
#endif


/*************************************************************************
 * Button OP APIs - Command entry
 ************************************************************************/
extern int AppButtonOp_Init(void);
extern int AppButtonOp_UpdateStatus(UINT32 buttonId, UINT32 event);

__END_C_PROTO__

#endif /* APP_BUTTON_OP_H_ */
