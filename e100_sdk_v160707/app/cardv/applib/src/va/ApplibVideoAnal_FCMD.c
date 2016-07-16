/**
 * @file src/app/connected/applib/src/va/ApplibVideoAnal_FCMD.c
 *
 * Implementation of VA Frontal Car Moving Depature(FCMD) APIs
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

#include "va/ApplibVideoAnal_FCMD.h"
#include "va/ApplibVideoAnal_StmpHdlr.h"
#include <va/ambava_adas_FCMD.h>
#include <gps.h>

static int ApplibFcmdInit = 0;
static AMP_FCMD_CFG_t ApplibFcmdConfig = {0};
static AMP_FCMD_PAR_t ApplibFcmdParams = {0};
static gps_data_t* ApplibFcmdGpsData = NULL;
static UINT8 ApplibFcmdYuvSrc = 0;

int AppLibVideoAnal_FCMD_Init(UINT8 yuvSrc, APPLIB_FCMD_CFG_t pConfig, APPLIB_FCMD_PAR_t pParams)
{
    int ReturnValue = 0;
    AMP_ENC_YUV_INFO_s Img[1] = {0};
    int FrmSizeIsChange = 0;

    ApplibFcmdYuvSrc = yuvSrc;

    if ( AppLibVideoAnal_FrmHdlr_GetFrmInfo(ApplibFcmdYuvSrc ,Img, &FrmSizeIsChange) != OK){
        AmbaPrint("AppLibVideoAnal_FrmHdlr is not init");
        return -1;
    }
    if (ApplibFcmdInit) {
        AmbaPrint("FCMD is already init");
        return -1;
    }
    if ( Amba_AdasFCMD_GetDef_Cfg_Par(Img, &ApplibFcmdConfig, &ApplibFcmdParams) != OK){
        AmbaPrint("Amba_AdasFCMD_GetDef_Cfg_Par failed");
        return -1;
    }
    ReturnValue = AmpUtil_GetAlignedPool(APPLIB_G_MMPL, (void **)&ApplibFcmdConfig.Buf.pMemAlignedBase, (void **)&ApplibFcmdConfig.Buf.pMemBase, ApplibFcmdConfig.Buf_Size, 32);
    if (ReturnValue != OK) {
        AmbaPrint(" cardvws_process_FCMD Can't allocate memory.");
        return -1;
    }else {
        AmbaPrint("AmpUtil_GetAlignedPool Alignedbuf = 0x%X / size = %d  !", ApplibFcmdConfig.Buf.pMemAlignedBase, ApplibFcmdConfig.Buf_Size);
    }

    /** Set Config */
    ApplibFcmdConfig.IsEnabled = 1;

    /** Set Paramss */
    ApplibFcmdParams.HoodLevel = pParams.HoodLevel;
    ApplibFcmdParams.HorizonLevel = pParams.HorizonLevel;
    ApplibFcmdParams.IsUpdate = 1;
    ReturnValue = Amba_AdasFCMD_SetCfg(&ApplibFcmdConfig);

    ApplibFcmdConfig.FCMDSensitivity = pConfig.FCMDSensitivity;
    ReturnValue = Amba_AdasFCMD_SetCfg(&ApplibFcmdConfig);
    if ( ReturnValue != OK){
        AmbaPrint("Amba_AdasFCMD_SetCfg failed");
        return 0;
    }

    AmbaPrint("Amba_AdasFCMD_Init (%d , %d)", Img[0].width, Img[0].height);
    ReturnValue = Amba_AdasFCMD_Init(Img, &ApplibFcmdParams);
    if (ReturnValue != OK){
        AmbaPrint("Amba_AdasFCMD_Init failed");
        return -1;
    }
    ApplibFcmdInit = 1;
    return ReturnValue;
}

int AppLibVideoAnal_FCMD_DeInit(void)
{
    int ReturnValue = 0;
    if (ApplibFcmdInit == 0) {
        AmbaPrint("AppLibVideoAnal_FCMD is not init \n");
        return -1;
    }
    if (ApplibFcmdConfig.Buf.pMemBase != 0) {
        AmbaKAL_BytePoolFree(ApplibFcmdConfig.Buf.pMemBase);
    }
    Amba_AdasFCMD_Deinit();
    ApplibFcmdInit = 0;
    memset( &ApplibFcmdConfig, 0, sizeof(AMP_FCMD_CFG_t));
    memset( &ApplibFcmdParams, 0, sizeof(AMP_FCMD_PAR_t));
    AmbaPrint("AppLibVideoAnal_FCMD deinit done \n");
    return ReturnValue;
}

int AppLibVideoAnal_FCMD_GetDef_Setting( APPLIB_FCMD_CFG_t* pApplibFcmdConfig, APPLIB_FCMD_PAR_t* pApplibFcmdParams)
{
    int ReturnValue = 0;
    
    pApplibFcmdConfig->FCMDSensitivity      = ADAS_SL_MEDIUM;
    pApplibFcmdParams->HoodLevel            = DEFAULT_HOODLEVEL;
    pApplibFcmdParams->HorizonLevel         = DEFAULT_HORIZLEVEL;

    return ReturnValue;
}

int AppLibVideoAnal_FCMD_SetCfg(APPLIB_FCMD_CFG_t* pConfig)
{
    int ReturnValue = 0;
    ApplibFcmdConfig.FCMDSensitivity = pConfig->FCMDSensitivity;
    ReturnValue = Amba_AdasFCMD_SetCfg(&ApplibFcmdConfig);
    return ReturnValue;
}

int AppLibVideoAnal_FCMD_SetPar( APPLIB_FCMD_PAR_t* pParams)
{
    int ReturnValue = 0;
    ApplibFcmdParams.HoodLevel = pParams->HoodLevel;
    ApplibFcmdParams.HorizonLevel = pParams->HorizonLevel;
    ApplibFcmdParams.IsUpdate = 1;
    return ReturnValue;
}

int AppLibVideoAnal_FCMD_Process(UINT32 event, AMP_ENC_YUV_INFO_s* img)
{
    int ReturnValue = 0;
    AMBA_ADAS_FCMD_EVENT_e Fcmd_Event;
    AMBA_ADAS_GPS_INFO_s GpsInfo = {0};
    AMBA_ADAS_AUX_DATA_s AuxData = {0};
    gps_data_t GpsData = {0};
#ifdef CONFIG_APP_ARD
#ifdef APP_GPS_VALID
    ApplibFcmdGpsData = AppLibSysGps_GetData();

    if (ApplibFcmdGpsData->status == 0) {
        return ReturnValue;
    } else {
        GpsInfo.Speed = ApplibFcmdGpsData->fix.speed*MPS_TO_KMPH;
        GpsInfo.Bearing = ApplibFcmdGpsData->fix.track;
    }
#else
        GpsInfo.Speed = 0;
        GpsInfo.Bearing = 0;
#endif
#else
    ApplibFcmdGpsData = AppLibSysGps_GetData();

    if (ApplibFcmdGpsData->status == 0) {
        return ReturnValue;
    } else {
        GpsInfo.Speed = ApplibFcmdGpsData->fix.speed*MPS_TO_KMPH;
        GpsInfo.Bearing = ApplibFcmdGpsData->fix.track;
    }
#endif

    ApplibFcmdParams.Aux_data.pGpsInfo = &GpsInfo;

    ReturnValue = Amba_AdasFCMD_Proc(img, &ApplibFcmdParams, &Fcmd_Event);

    if (Fcmd_Event == AMBA_FCMD_MOVE) {
        AppLibVideoAnal_StmpHdlr_AddEvent(VA_STMPHDLR_FCMD);
        AppLibComSvcHcmgr_SendMsgNoWait(HMSG_VA_FCAR_DEPARTURE, 0, 0);
        AmbaPrintColor(RED, "Frontal Vehicle Move \n");
        //AmbaKAL_TaskSleep(3000);
    }

    //AmbaPrintColor(RED, "End \n");
    return ReturnValue;
}

int AppLibVideoAnal_FCMD_Enable(void)
{
    int ReturnValue = 0;

    if ( !(AppLibVideoAnal_FrmHdlr_IsInit()) ) {
        AmbaPrint("AppLibVideoAnal_FrmHdlr is not init \n");
        return -1;
    }
    if (ApplibFcmdInit == 0) {
        AmbaPrint("AppLibVideoAnal_FCMD is not init \n");
        return -1;
    }
    ApplibFcmdConfig.IsEnabled = 1;
    ReturnValue = AppLibVideoAnal_FrmHdlr_Register(ApplibFcmdYuvSrc, AppLibVideoAnal_FCMD_Process);
    return ReturnValue;
}

int AppLibVideoAnal_FCMD_Disable(void)
{
    int ReturnValue = 0;
    if ( !(AppLibVideoAnal_FrmHdlr_IsInit()) ) {
        AmbaPrint("AppLibVideoAnal_FrmHdlr is not init \n");
        return -1;
    }
    ReturnValue = AppLibVideoAnal_FrmHdlr_UnRegister(ApplibFcmdYuvSrc, AppLibVideoAnal_FCMD_Process);
    ApplibFcmdConfig.IsEnabled = 0;
    return ReturnValue;
}





