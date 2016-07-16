/**
 * @file app/connected/applib/inc/va/ApplibVideoAnal_3aHdlr.h
 *
 * header of VA frame 3A info handler
 *
 * History:
 *    2015/01/09 - [Bill Chou] created file
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

#ifndef APPLIB_VIDEO_ANAL_3AHDLR_H_
#define APPLIB_VIDEO_ANAL_3AHDLR_H_

#include <recorder/Encode.h>
#include "AmbaDSP_Event.h"

#define APPLIB_TRIA_HDLR_MSGQUEUE_SIZE (128)
#define APPLIB_TRIA_HDLR_MAX_CB  (16)
#define APPLIB_TRIA_HDLR_DEF_PRIORITY  (135)
#define APPLIB_TRIA_HDLR_DEF_TASK_STACK_SIZE  (32<<10)


typedef struct APPLIB_TRIA_TASK_CFG_t_ {
    UINT8 TaskPriority;
    UINT32 TaskStackSize;
    void* TaskStack;
} APPLIB_TRIA_TASK_CFG_t;

typedef int (*APPLIB_VA_TRIAHDLR_CB)(UINT32, AMBA_DSP_EVENT_CFA_3A_DATA_s*);

extern int AppLibVideoAnal_TriAHdlr_IsInit(void);

extern int AppLibVideoAnal_TriAHdlr_GetDefCfg(APPLIB_TRIA_TASK_CFG_t* cfg);

extern int AppLibVideoAnal_TriAHdlr_Init(void);

extern int AppLibVideoAnal_TriAHdlr_Register(UINT32 event, APPLIB_VA_TRIAHDLR_CB func);

extern int AppLibVideoAnal_TriAHdlr_UnRegister(UINT32 event, APPLIB_VA_TRIAHDLR_CB func);

extern int AppLibVideoAnal_TriAHdlr_NewFrame(UINT32, AMBA_DSP_EVENT_CFA_3A_DATA_s* pData3A);

extern int AppLibVideoAnal_DSP_EventHdlr_NewAE(AMBA_DSP_EVENT_CFA_3A_DATA_s* pData3A);


#endif /* APPLIB_VIDEO_ANAL_3AHDLR_H_ */

