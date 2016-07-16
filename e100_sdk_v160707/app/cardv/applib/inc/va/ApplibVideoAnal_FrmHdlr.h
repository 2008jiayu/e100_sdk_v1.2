/**
 * @file src/app/connected/applib/inc/va/ApplibVideoAnal_FrmHdlr.h
 *
 * Amba fifo
 *
 * History:
 *    2014/12/4 - [cyweng] created file
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

#ifndef APPLIB_VIDEO_ANAL_FRMHDLR_H_
#define APPLIB_VIDEO_ANAL_FRMHDLR_H_

#include <recorder/Encode.h>

#define APPLIB_FRM_HDLR_MSGQUEUE_SIZE (128)
#define APPLIB_FRM_HDLR_MAX_CB  (16)
#define APPLIB_FRM_HDLR_DEF_PRIORITY  (136)
#define APPLIB_FRM_HDLR_DEF_TASK_STACK_SIZE  (32<<10)

typedef enum APPLIB_FRM_HDLR_YUV_CNT_e_ {
    APPLIB_FRM_HDLR_2ND_YUV = 0,            /* 2nd Stream */
    APPLIB_FRM_HDLR_DCHAN_YUV,          /* Dchan */

    APPLIB_FRM_HDLR_YUV_NUM             /* Number of YUV for va */
} APPLIB_FRM_HDLR_YUV_CNT_e;

typedef struct APPLIB_YUV_TASK_CFG_t_ {
    UINT8 TaskPriority;
    UINT32 TaskStackSize;
    void* TaskStack;
} APPLIB_YUV_TASK_CFG_t;

typedef int (*APPLIB_VA_FRMHDLR_CB)(UINT32, AMP_ENC_YUV_INFO_s*);

extern int AppLibVideoAnal_FrmHdlr_GetFrmInfo(UINT8 yuvSrc, AMP_ENC_YUV_INFO_s* yuvInfo, int* frmSizeIsChanged);

extern int AppLibVideoAnal_FrmHdlr_IsInit(void);

extern int AppLibVideoAnal_FrmHdlr_GetDefCfg(APPLIB_YUV_TASK_CFG_t* cfg);

extern int AppLibVideoAnal_FrmHdlr_Init(void);

extern int AppLibVideoAnal_FrmHdlr_Register(UINT8 yuvSrc, APPLIB_VA_FRMHDLR_CB func);

extern int AppLibVideoAnal_FrmHdlr_UnRegister(UINT8 yuvSrc, APPLIB_VA_FRMHDLR_CB func);

extern int AppLibVideoAnal_FrmHdlr_NewFrame(UINT32 event, AMP_ENC_YUV_INFO_s* yuvInfo);




#endif /* APPLIB_VIDEO_ANAL_FRMHDLR_H_ */
