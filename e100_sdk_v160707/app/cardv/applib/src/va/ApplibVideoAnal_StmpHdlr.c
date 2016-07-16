/**
 * @file src/app/connected/applib/src/va/ApplibVideoAnal_StmpHdlr.c
 *
 * Implementation of VA Stamp Handler APIs
 *
 * History:
 *    2015/01/06 - [Bill Chou] created file
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

#include "va/ApplibVideoAnal_StmpHdlr.h"

static UINT32 VaEventFlag = 0;
static int LDWSTime = 0;
static int FCWSTime = 0;
static int FCMDTime = 0;
static int LLWSTime = 0;

static void AppLibVideoAnal_StmpHdlr_LDWSEventTimer(int eid)
{
    if (eid == TIMER_UNREGISTER) {
        return;
    }
    
    LDWSTime++;
    
    if (LDWSTime >= VA_STMPHDLR_KEEP_TIME) {
        LDWSTime = 0;
        VaEventFlag &= (~VA_STMPHDLR_LDWS);
        AppLibComSvcTimer_Unregister(TIMER_1HZ, AppLibVideoAnal_StmpHdlr_LDWSEventTimer);
        AppLibComSvcHcmgr_SendMsgNoWait(HMSG_VA_UPDATE, 0, 0);
    }
}

static void AppLibVideoAnal_StmpHdlr_FCWSEventTimer(int eid)
{
    if (eid == TIMER_UNREGISTER) {
        return;
    }

    FCWSTime++;
    
    if (FCWSTime >= VA_STMPHDLR_KEEP_TIME) {
        FCWSTime = 0;
        VaEventFlag &= (~VA_STMPHDLR_FCWS);
        AppLibComSvcTimer_Unregister(TIMER_1HZ, AppLibVideoAnal_StmpHdlr_FCWSEventTimer);
        AppLibComSvcHcmgr_SendMsgNoWait(HMSG_VA_UPDATE, 0, 0);
    }
}

static void AppLibVideoAnal_StmpHdlr_FCMDEventTimer(int eid)
{
    if (eid == TIMER_UNREGISTER) {
        return;
    }

    FCMDTime++;
    
    if (FCMDTime >= VA_STMPHDLR_KEEP_TIME) {
        FCMDTime = 0;
        VaEventFlag &= (~VA_STMPHDLR_FCMD);
        AppLibComSvcTimer_Unregister(TIMER_1HZ, AppLibVideoAnal_StmpHdlr_FCMDEventTimer);
        AppLibComSvcHcmgr_SendMsgNoWait(HMSG_VA_UPDATE, 0, 0);
    }
}

static void AppLibVideoAnal_StmpHdlr_LLWSEventTimer(int eid)
{
    if (eid == TIMER_UNREGISTER) {
        return;
    }

    LLWSTime++;
    
    if (LLWSTime >= VA_STMPHDLR_KEEP_TIME) {
        LLWSTime = 0;
        VaEventFlag &= (~VA_STMPHDLR_LLWS);
        AppLibComSvcTimer_Unregister(TIMER_1HZ, AppLibVideoAnal_StmpHdlr_LLWSEventTimer);
        AppLibComSvcHcmgr_SendMsgNoWait(HMSG_VA_UPDATE, 0, 0);
    }
}

int AppLibVideoAnal_StmpHdlr_AddEvent(UINT32 VaEvent)
{
    int ReturnValue = 0;

    VaEventFlag |= VaEvent;

    if (VaEvent == VA_STMPHDLR_LDWS) {
        AppLibComSvcTimer_Register(TIMER_1HZ, AppLibVideoAnal_StmpHdlr_LDWSEventTimer);
    } else if (VaEvent == VA_STMPHDLR_FCWS) {
        AppLibComSvcTimer_Register(TIMER_1HZ, AppLibVideoAnal_StmpHdlr_FCWSEventTimer);
    } else if (VaEvent == VA_STMPHDLR_FCMD) {
        AppLibComSvcTimer_Register(TIMER_1HZ, AppLibVideoAnal_StmpHdlr_FCMDEventTimer);
    } else if (VaEvent == VA_STMPHDLR_LLWS) {
        AppLibComSvcTimer_Register(TIMER_1HZ, AppLibVideoAnal_StmpHdlr_LLWSEventTimer);
    } else if (VaEvent == VA_STMPHDLR_CALIB) {
        AppLibComSvcHcmgr_SendMsgNoWait(HMSG_VA_UPDATE, 0, 0);
    }

    return ReturnValue;
}

UINT32 AppLibVideoAnal_StmpHdlr_GetEventFlag(void)
{
    return VaEventFlag;
}
 