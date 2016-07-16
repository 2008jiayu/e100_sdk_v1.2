/**
 * @file src/app/connected/applib/src/va/ApplibVideoAnal_MD.c
 *
 * Implementation of VA Frontal Car Moving Depature(MD) APIs
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

#include "va/ApplibVideoAnal_MD.h"
#include <va/ambava_adas_MD_yuv.h>
#include <va/ambava_adas_MD_ae.h>
static int Applib_MD_Init = 0;
static APPLIB_ADAS_MD_METHOD_e Applib_MD_Method = APPLIB_MD_YUV;
static AMBA_MDY_CFG_t AppLib_MDYConfig = {0};
static AMBA_MDY_PAR_t AppLib_MDYParams = {0};

static AMBA_MDAE_CFG_t AppLib_MDAEConfig = {0};
static AMBA_MDAE_PAR_t AppLib_MDAEParams = {0};

static UINT8 ApplibMDYuvSrc = 0;

void AppLibVideoAnal_MD_SetROI(APPLIB_MD_CFG_t* pConfig)
{
    int i;
    Applib_MD_Method = pConfig->Method;
    switch (Applib_MD_Method){
    case APPLIB_MD_YUV:
        for ( i = 0; i < MOTION_DETECT_ROI_MAX; i++) {
            if ( (pConfig->RoiData[i].Location.H == 0) || (pConfig->RoiData[i].Location.W == 0) ) {
                continue;
            }
            AppLib_MDYConfig.RoiData[i].Location.X = pConfig->RoiData[i].Location.X;
            AppLib_MDYConfig.RoiData[i].Location.Y = pConfig->RoiData[i].Location.Y;
            AppLib_MDYConfig.RoiData[i].Location.W = pConfig->RoiData[i].Location.W;
            AppLib_MDYConfig.RoiData[i].Location.H = pConfig->RoiData[i].Location.H;
            AppLib_MDYConfig.RoiData[i].Sensitivity = 5 + 5 * ( pConfig->RoiData[i].MDSensitivity) ;
        }
        AppLib_MDYParams.Method = MDY_DEFAULT;
        AppLib_MDYParams.IsUpdate = 1;
        break;
    case APPLIB_MD_AE:
        for ( i = 0; i < MOTION_DETECT_ROI_MAX; i++) {
            if ( (pConfig->RoiData[i].Location.H == 0) || (pConfig->RoiData[i].Location.W == 0) ) {
                continue;
            }
            AppLib_MDAEConfig.RoiData[i].Location.X = pConfig->RoiData[i].Location.X;
            AppLib_MDAEConfig.RoiData[i].Location.Y = pConfig->RoiData[i].Location.Y;
            AppLib_MDAEConfig.RoiData[i].Location.W = pConfig->RoiData[i].Location.W;
            AppLib_MDAEConfig.RoiData[i].Location.H = pConfig->RoiData[i].Location.H;
            AppLib_MDAEConfig.RoiData[i].Sensitivity = 1 + pConfig->RoiData[i].MDSensitivity; /// Sensitivity is Multiplier
            AppLib_MDAEConfig.RoiData[i].Threshold = 3900;
            AppLib_MDAEConfig.RoiData[i].Update_Freq = 1;
            AppLib_MDAEConfig.RoiData[i].Update_Cnt = AppLib_MDAEConfig.RoiData[i].Update_Freq - 1;
        }
        AppLib_MDAEParams.Method = MDAE_DEFAULT;
        AppLib_MDAEParams.IsUpdate = 1;
        break;
    case APPLIB_MD_YUV_MSE:
        for ( i = 0; i < MOTION_DETECT_ROI_MAX; i++) {
            if ( (pConfig->RoiData[i].Location.H == 0) || (pConfig->RoiData[i].Location.W == 0) ) {
                continue;
            }
            AppLib_MDYConfig.RoiData[i].Location.X = pConfig->RoiData[i].Location.X;
            AppLib_MDYConfig.RoiData[i].Location.Y = pConfig->RoiData[i].Location.Y;
            AppLib_MDYConfig.RoiData[i].Location.W = pConfig->RoiData[i].Location.W;
            AppLib_MDYConfig.RoiData[i].Location.H = pConfig->RoiData[i].Location.H;
            AppLib_MDYConfig.RoiData[i].Sensitivity = 5 + 5 * (pConfig->RoiData[i].MDSensitivity) ;
        }
        AppLib_MDYParams.Method = MDY_MSE;
        AppLib_MDYParams.IsUpdate = 1;
        break;
    }
}

int AppLibVideoAnal_MD_Init(UINT8 yuvSrc, APPLIB_MD_CFG_t* pConfig)
{
    int ReturnValue = 0;
    AMP_ENC_YUV_INFO_s Img[1] = {0};
    int FrmSizeIsChange = 0;
    Applib_MD_Method = pConfig->Method;
    ApplibMDYuvSrc = yuvSrc;

    switch (Applib_MD_Method){
    case APPLIB_MD_YUV:
        if ( AppLibVideoAnal_FrmHdlr_GetFrmInfo(ApplibMDYuvSrc, Img, &FrmSizeIsChange) != OK){
        AmbaPrint("AppLibVideoAnal_FrmHdlr is not init");
        return -1;
        }

        Amba_AdasMDY_GetDefCfg(Img, &AppLib_MDYConfig);

        ReturnValue = AmpUtil_GetAlignedPool(APPLIB_G_MMPL, (void **)&(AppLib_MDYConfig.MDYBuf.pMemAlignedBase),
                                            (void **)&(AppLib_MDYConfig.MDYBuf.pMemBase), AppLib_MDYConfig.MDYBuf_Size, 32);
        if (ReturnValue != OK) {
            AmbaPrint(" AppLibVideoAnal_MD_Init Can't allocate memory.");
            return -1;
        }else {
            AmbaPrint("AmpUtil_GetAlignedPool Alignedbuf = 0x%X / size = %d  !", AppLib_MDYConfig.MDYBuf.pMemAlignedBase, AppLib_MDYConfig.MDYBuf_Size);
        }
        AppLibVideoAnal_MD_SetROI(pConfig);

        ReturnValue = Amba_AdasMDY_SetCfg(&AppLib_MDYConfig);
        AppLib_MDYParams.Method = MDY_DEFAULT;
        AppLib_MDYParams.IsUpdate = 1;
        ReturnValue = Amba_AdasMDY_Init(&AppLib_MDYParams);
        Applib_MD_Init = 1;
        break;

    case APPLIB_MD_AE:
        Amba_AdasMDAE_GetDefCfg( &AppLib_MDAEConfig);
        ReturnValue = AmpUtil_GetAlignedPool(APPLIB_G_MMPL, (void **)&(AppLib_MDAEConfig.MDAEBuf.pMemAlignedBase),
                                            (void **)&(AppLib_MDAEConfig.MDAEBuf.pMemBase), AppLib_MDAEConfig.MDAEBuf_Size, 32);
        if (ReturnValue != OK) {
            AmbaPrint(" AppLibVideoAnal_MD_Init Can't allocate memory.");
            return -1;
        }else {
            AmbaPrint("AmpUtil_GetAlignedPool Alignedbuf = 0x%X / size = %d  !", AppLib_MDAEConfig.MDAEBuf.pMemAlignedBase, AppLib_MDAEConfig.MDAEBuf_Size);
        }
        AppLibVideoAnal_MD_SetROI(pConfig);

        ReturnValue = Amba_AdasMDAE_SetCfg(&AppLib_MDAEConfig);
        AppLib_MDAEParams.Method = MDAE_DEFAULT;
        AppLib_MDAEParams.IsUpdate = 1;
        ReturnValue = Amba_AdasMDAE_Init(&AppLib_MDAEParams);
        Applib_MD_Init = 1;
        break;

    case APPLIB_MD_YUV_MSE:
        if ( AppLibVideoAnal_FrmHdlr_GetFrmInfo(ApplibMDYuvSrc, Img, &FrmSizeIsChange) != OK){
        AmbaPrint("AppLibVideoAnal_FrmHdlr is not init");
        return -1;
        }

        Amba_AdasMDY_GetDefCfg(Img, &AppLib_MDYConfig);

        ReturnValue = AmpUtil_GetAlignedPool(APPLIB_G_MMPL, (void **)&(AppLib_MDYConfig.MDYBuf.pMemAlignedBase),
                                            (void **)&(AppLib_MDYConfig.MDYBuf.pMemBase), AppLib_MDYConfig.MDYBuf_Size, 32);
        if (ReturnValue != OK) {
            AmbaPrint(" AppLibVideoAnal_MD_Init Can't allocate memory.");
            return -1;
        }else {
            AmbaPrint("AmpUtil_GetAlignedPool Alignedbuf = 0x%X / size = %d  !", AppLib_MDYConfig.MDYBuf.pMemAlignedBase, AppLib_MDYConfig.MDYBuf_Size);
        }
        AppLibVideoAnal_MD_SetROI(pConfig);

        ReturnValue = Amba_AdasMDY_SetCfg(&AppLib_MDYConfig);
        AppLib_MDYParams.Method = MDY_MSE;
        AppLib_MDYParams.IsUpdate = 1;
        ReturnValue = Amba_AdasMDY_Init(&AppLib_MDYParams);
        Applib_MD_Init = 1;
        break;
    }

    return ReturnValue;
}

int AppLibVideoAnal_MD_DeInit(void)
{
    int ReturnValue = 0;
    switch (Applib_MD_Method){
    case APPLIB_MD_YUV:
    case APPLIB_MD_YUV_MSE:
        if (Applib_MD_Init == 0) {
            AmbaPrint("AppLibVideoAnal_MD is not init \n");
            return -1;
        }
        if (AppLib_MDYConfig.MDYBuf.pMemBase != 0) {
            AmbaKAL_BytePoolFree(AppLib_MDYConfig.MDYBuf.pMemBase);
        }
        Amba_AdasMDY_Deinit();
        memset( &AppLib_MDYConfig, 0, sizeof(AMBA_MDY_CFG_t));
        memset( &AppLib_MDYParams, 0, sizeof(AMBA_MDY_PAR_t));
        Applib_MD_Init = 0;
        Applib_MD_Method = APPLIB_MD_YUV;
        break;

    case APPLIB_MD_AE:
        if (Applib_MD_Init == 0) {
            AmbaPrint("AppLibVideoAnal_MD is not init \n");
            return -1;
        }
        if (AppLib_MDAEConfig.MDAEBuf.pMemBase != 0) {
            AmbaKAL_BytePoolFree(AppLib_MDAEConfig.MDAEBuf.pMemBase);
        }
        Amba_AdasMDAE_Deinit();
        memset( &AppLib_MDAEConfig, 0, sizeof(AMBA_MDAE_CFG_t));
        memset( &AppLib_MDAEParams, 0, sizeof(AMBA_MDAE_PAR_t));
        Applib_MD_Init = 0;
        Applib_MD_Method = APPLIB_MD_YUV;
        break;
    }
    AmbaPrint("AppLibVideoAnal_MD deinit done \n");
    return ReturnValue;
}

void AppLibVideoAnal_MD_GetDef_Setting( APPLIB_MD_CFG_t* pAppLibMDConfig)
{
    pAppLibMDConfig->Method = APPLIB_MD_YUV;
    pAppLibMDConfig->RoiData[0].Location.X = 0;
    pAppLibMDConfig->RoiData[0].Location.Y = 0;
    pAppLibMDConfig->RoiData[0].Location.W = 300;
    pAppLibMDConfig->RoiData[0].Location.H = 200;
    pAppLibMDConfig->RoiData[0].MDSensitivity = ADAS_SL_MEDIUM;
    /*
    AppLibMDConfig->Method = APPLIB_MD_AE;
    pAppLibMDConfig->RoiData[0].Location.X = 0;
    pAppLibMDConfig->RoiData[0].Location.Y = 0;
    pAppLibMDConfig->RoiData[0].Location.W = 12;
    pAppLibMDConfig->RoiData[0].Location.H = 8;
    pAppLibMDConfig->RoiData[0].MDSensitivity = ADAS_SL_MEDIUM;
    */
}





int AppLibVideoAnal_MDY_Process(UINT32 event, AMP_ENC_YUV_INFO_s* img)
{
    int ReturnValue = 0;
    int MD_Event = 0;
    ReturnValue = Amba_AdasMDY_Proc(img, &AppLib_MDYParams, &MD_Event);

    if (MD_Event == AMBA_MDY_MOVE_DET) {
        AppLibComSvcHcmgr_SendMsgNoWait(HMSG_VA_MD_Y, 0, 0);
        AmbaPrintColor(RED, "AMBA_MDY_MOVE_DET \n");
        AmbaKAL_TaskSleep(3000);
    }

    //AmbaPrintColor(RED, "End \n");
    return ReturnValue;
}

int AppLibVideoAnal_MDAE_Process(UINT32 event, AMBA_DSP_EVENT_CFA_3A_DATA_s* pData)
{
#if 0 //def CONFIG_APP_ARD
        static UINT32 LastAlarm = 0;
        int ReturnValue = 0;
        int MD_Event = 0;
        UINT32 CurTime = AmbaKAL_GetTickCount();
        ReturnValue = Amba_AdasMDAE_Proc(pData, &AppLib_MDAEParams, &MD_Event);

#define MDY_MIN_ALARM_DISTANCE  (5000)
        if (MD_Event == AMBA_MDAE_MOVE_DET) {
            if (((CurTime - LastAlarm)>MDY_MIN_ALARM_DISTANCE) || (0 == LastAlarm)) {
                LastAlarm = CurTime;
                AppLibComSvcHcmgr_SendMsgNoWait(HMSG_VA_MD_AE, 0, 0);
                AmbaPrintColor(RED, "AMBA_MDAE_MOVE_DET.... \n");
            }
        }
#else
    int ReturnValue = 0;
    int MD_Event = 0;
    ReturnValue = Amba_AdasMDAE_Proc(pData, &AppLib_MDAEParams, &MD_Event);

    if (MD_Event == AMBA_MDAE_MOVE_DET) {
        AppLibComSvcHcmgr_SendMsgNoWait(HMSG_VA_MD_AE, 0, 0);
        AmbaPrintColor(RED, "AMBA_MDAE_MOVE_DET.... \n");
     //   AmbaKAL_TaskSleep(3000);
    }
#endif

        //AmbaPrintColor(RED, "End \n");
        return ReturnValue;
}

int AppLibVideoAnal_MD_Enable(void)
{
    int ReturnValue = 0;

    switch (Applib_MD_Method){
    case APPLIB_MD_YUV:
    case APPLIB_MD_YUV_MSE:
        if ( !(AppLibVideoAnal_FrmHdlr_IsInit()) ) {
        AmbaPrint("AppLibVideoAnal_FrmHdlr is not init \n");
        return -1;
        }
        if (Applib_MD_Init == 0) {
            AmbaPrint("AppLibVideoAnal_MD is not init \n");
            return -1;
        }
        ReturnValue = AppLibVideoAnal_FrmHdlr_Register(ApplibMDYuvSrc, AppLibVideoAnal_MDY_Process);
        break;

    case APPLIB_MD_AE:
        if ( !(AppLibVideoAnal_TriAHdlr_IsInit()) ) {
            AmbaPrint("AppLibVideoAnal_TriAHdlr is not init \n");
            return -1;
        }
        if ( Applib_MD_Init == 0 ) {
            AmbaPrint("AppLibVideoAnal_MD is not init \n");
            return -1;
        }
        ReturnValue = AppLibVideoAnal_TriAHdlr_Register(AMBA_DSP_EVENT_CFA_3A_DATA_READY, AppLibVideoAnal_MDAE_Process);
        break;
    }
    return ReturnValue;
}

int AppLibVideoAnal_MD_Disable(void)
{
    int ReturnValue = 0;

    switch (Applib_MD_Method){
    case APPLIB_MD_YUV:
    case APPLIB_MD_YUV_MSE:
        if ( !(AppLibVideoAnal_FrmHdlr_IsInit()) ) {
        AmbaPrint("AppLibVideoAnal_FrmHdlr is not init \n");
        return -1;
        }
        ReturnValue = AppLibVideoAnal_FrmHdlr_UnRegister(ApplibMDYuvSrc, AppLibVideoAnal_MDY_Process);
        break;

    case APPLIB_MD_AE:
        if ( !(AppLibVideoAnal_TriAHdlr_IsInit()) ) {
            AmbaPrint("AppLibVideoAnal_TriAHdlr is not init \n");
            return -1;
        }
        ReturnValue = AppLibVideoAnal_TriAHdlr_UnRegister(AMBA_DSP_EVENT_CFA_3A_DATA_READY, AppLibVideoAnal_MDAE_Process);
        break;
    }
    return ReturnValue;
}





