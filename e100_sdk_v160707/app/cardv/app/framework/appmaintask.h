 /**
  * @file src/app/framework/appmaintask.h
  *
  * Header of DemoApp entry for testing.
  *
  * History:
  *    2013/08/20 - [Martin Lai] created file
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


#ifndef APPMAINTASK_H_
#define APPMAINTASK_H_

#include <stdio.h>
#include <string.h>
#include <AmbaDataType.h>
#include <AmbaRTSL_GPIO.h>
#include <AmbaKAL.h>


#define APP_DSP_CMD_PREPARE_TASK_PRIORITY 6
#define APP_DSP_ARM_CMD_TASK_PRIORITY 9
#define APP_DSP_MSG_DISPATCH_TASK_PRIORITY 11
#define APP_DSP_STILLRAW_MONITOR_TASK_PRIORITY 57
#define APP_FIFO_TASK_PRIORITY 25
#define APP_BOOT_MGR_PRIORITY 70
#define APP_CFS_SCHDLR_PRIORITY 73
#define APP_BUTTON_OP_PRIORITY 122
#define APP_IR_BUTTON_OP_PRIORITY 125
#define APP_DCF_PRIORITY 160
#define APP_AMBA_SHELL_PRIORITY 190
#define APP_AMBA_PRINT_PRIORITY 202
#define APP_AMBA_BOSS_PRIORITY 100

#endif /* APPMAINTASK_H_ */

