/**
 * @file src/app/connected/applib/src/va/ApplibVideoAnal_LLWS.c
 *
 * Implementation of Low Light Warning System(LLWS) AppLib
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

#include "va/ApplibVideoAnal_LLWS.h"
#include "va/ApplibVideoAnal_StmpHdlr.h"
#include <va/ambava_adas_LLWS.h>

static int ApplibLlwsInit = 0;
static AMP_LLWS_CFG_t ApplibLlwsConfig = {0};
static AMP_LLWS_PAR_t ApplibLlwsParams = {0};


void AppLibVideoAnal_LLWS_GetDef_Setting( APPLIB_LLWS_CFG_t* pConfig, APPLIB_LLWS_PAR_t* pParams)
{
    pConfig->LLWSSensitivity = ADAS_SL_MEDIUM;
    pParams->HoodLevel = DEFAULT_HOODLEVEL;
    pParams->HorizonLevel = DEFAULT_HORIZLEVEL;
}

int AppLibVideoAnal_LLWS_Init(APPLIB_LLWS_CFG_t config, APPLIB_LLWS_PAR_t params)
{
    int ReturnValue = 0;

    if (ApplibLlwsInit) {
        AmbaPrint("LLWS is already init");
        return -1;
    }

    ApplibLlwsConfig.LLWSSensitivity = config.LLWSSensitivity;
    ApplibLlwsParams.HoodLevel = params.HoodLevel;
    ApplibLlwsParams.HorizonLevel = params.HorizonLevel;
    ReturnValue = Amba_AdasLLWS_SetCfg(&ApplibLlwsConfig);
    ReturnValue = Amba_AdasLLWS_Init(&ApplibLlwsParams);
    ApplibLlwsInit = 1;
    return ReturnValue;
}

int AppLibVideoAnal_LLWS_DeInit(void)
{
    int ReturnValue = 0;
    if (ApplibLlwsInit == 0) {
        AmbaPrint("AppLibVideoAnal_LLWS is not init \n");
        return -1;
    }
    memset( &ApplibLlwsConfig, 0, sizeof(AMP_LLWS_CFG_t));
    memset( &ApplibLlwsParams, 0, sizeof(AMP_LLWS_PAR_t));
    ApplibLlwsInit = 0;
    AmbaPrint("AppLibVideoAnal_LLWS deinit done \n");
    return ReturnValue;
}

int AppLibVideoAnal_LLWS_SetCfg(APPLIB_LLWS_CFG_t* pConfig)
{
    int ReturnValue = 0;
    ApplibLlwsConfig.LLWSSensitivity = pConfig->LLWSSensitivity;
    ReturnValue = Amba_AdasLLWS_SetCfg(&ApplibLlwsConfig);
    return ReturnValue;
}

int AppLibVideoAnal_LLWS_SetPar(APPLIB_LLWS_PAR_t* pParams)
{
    int ReturnValue = 0;
    ApplibLlwsParams.HoodLevel = pParams->HoodLevel;
    ApplibLlwsParams.HorizonLevel = pParams->HorizonLevel;
    ApplibLlwsParams.IsUpdate = 1;
    return ReturnValue;
}


int AppLibVideoAnal_LLWS_Process(UINT32 event, AMBA_DSP_EVENT_CFA_3A_DATA_s* pData)
{
    int ReturnValue = 0;
    int Llws_Event = 0;
    ReturnValue = Amba_AdasLLWS_Proc(pData, &ApplibLlwsParams, &Llws_Event);

    if (Llws_Event == AMBA_LLWS_LOW_LIGHT) {
        AppLibVideoAnal_StmpHdlr_AddEvent(VA_STMPHDLR_LLWS);
        AppLibComSvcHcmgr_SendMsgNoWait(HMSG_VA_LOW_LIGHT, 0, 0);
        //AmbaKAL_TaskSleep(3000);
    }

    return ReturnValue;
}

int AppLibVideoAnal_LLWS_Enable(void)
{
    int ReturnValue = 0;
    if ( !(AppLibVideoAnal_TriAHdlr_IsInit()) ) {
        AmbaPrint("AppLibVideoAnal_TriAHdlr is not init \n");
        return -1;
    }
    if ( ApplibLlwsInit == 0 ) {
        AmbaPrint("Applib_Llws is not init \n");
        return -1;
    }
    ApplibLlwsConfig.IsEnabled = 1;
    ReturnValue = Amba_AdasLLWS_SetCfg(&ApplibLlwsConfig);
    ReturnValue = AppLibVideoAnal_TriAHdlr_Register(AMBA_DSP_EVENT_CFA_3A_DATA_READY, AppLibVideoAnal_LLWS_Process);
    return ReturnValue;
}

int AppLibVideoAnal_LLWS_Disable(void)
{
    int ReturnValue = 0;
    if ( !(AppLibVideoAnal_TriAHdlr_IsInit()) ) {
        AmbaPrint("AppLibVideoAnal_TriAHdlr is not init \n");
        return -1;
    }
    ReturnValue = AppLibVideoAnal_TriAHdlr_UnRegister(AMBA_DSP_EVENT_CFA_3A_DATA_READY, AppLibVideoAnal_LLWS_Process);
    ApplibLlwsConfig.IsEnabled = 0;
    return ReturnValue;

}

