/**
 * @file src/app/connected/applib/src/recorder/ApplibRecorder_StillEnc.c
 *
 * Implementation of photo config Apis
 *
 * History:
 *    2013/10/04 - [Martin Lai] created file
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

#include <applib.h>
#include <recorder/StillEnc.h>
#include <imgproc/AmbaImg_Proc.h>
#include <imgproc/AmbaImg_Impl_Cmd.h>
#include <AmbaDSP_WarpCore.h>
#include <AmbaDSP_VIN.h>
#include <AmbaUtility.h>
#include <AmbaFS.h>
#include <calibration/ApplibCalibMgr.h>
#include <calibration/bpc/ApplibCalibBpc.h>
#include <calibration/vig/ApplibCalibVig.h>
#include "../AppLibTask_Priority.h"

///#define DEBUG_APPLIB_PHOTO
#if defined(DEBUG_APPLIB_PHOTO)
#define DBGMSG AmbaPrint
#define DBGMSGc(x) AmbaPrintColor(GREEN,x)
#define DBGMSGc2 AmbaPrintColor
#else
#define DBGMSG(...)
#define DBGMSGc(...)
#define DBGMSGc2(...)
#endif

static UINT8 *StillEncWorkBuf;
static void *StillEncWorkBufRaw;
static UINT32 LcdQvTurnOn = 0;
static UINT32 TVQvTurnOn = 0;

AMP_STLENC_HDLR_s *StillEncPri = NULL;
AMP_ENC_PIPE_HDLR_s *StillEncPipe = NULL;

#define STILL_BISFIFO_SIZE 32*1024*1024
UINT8 *StillBitsBuf;
void *StillBitsBufRaw;
#define STILL_DESC_SIZE 32*128
UINT8 *StillDescBuf;
void *StillDescBufRaw;

static int ApplibStillEncLiveviewInitFlag = -1;
static UINT8 ApplibStillEncLiveViewSlowShutterEnable = 1;
static int ApplibStillEncInitFlag = -1;


/**
 *  @brief Initial the photo encoder.
 *
 *  Initial the photo encoder.
 *
 *  @return >=0 success, <0 failure
 */
int AppLibStillEnc_Init(void)
{
    int ReturnValue = 0;

    DBGMSGc2(BLUE,"[Applib - StillEnc] <Init> Start");

    if (ApplibStillEncInitFlag == 0) {
        DBGMSG("[Applib - StillEnc] <Init> Still encoder already inital");
        return ReturnValue;
    }
    /* Init StillEnc module */
    {
        AMP_STILLENC_INIT_CFG_s EncInitCfg;

        AmpStillEnc_GetInitDefaultCfg(&EncInitCfg);
        if (StillEncWorkBuf == NULL) {
            ReturnValue = AmpUtil_GetAlignedPool(APPLIB_G_MMPL, (void **)&StillEncWorkBuf, &StillEncWorkBufRaw, EncInitCfg.MemoryPoolSize, 32);
            if (ReturnValue != OK) {
                AmbaPrintColor(RED, "[Applib - StillEnc] <Init> Out of memory for stillmain!!");
            }
        }
        EncInitCfg.MemoryPoolAddr = StillEncWorkBuf;
        EncInitCfg.MainTaskInfo.Priority = APPLIB_STILL_ENC_TASK_PRIORITY;
        EncInitCfg.RawCapTaskInfo.Priority = APPLIB_STILL_RAW_CAP_TASK_PRIORITY;
        EncInitCfg.RawEncTaskInfo.Priority = APPLIB_STILL_RAW_ENC_TASK_PRIORITY;
        ReturnValue = AmpStillEnc_Init(&EncInitCfg);
        if (ReturnValue != OK) {
            AmbaPrintColor(RED,"[Applib - StillEnc] <Init> failure!!");
        }
    }
    ApplibStillEncInitFlag = 0;
    DBGMSGc2(BLUE,"[Applib - StillEnc] <Init> End");
    return ReturnValue;
}

/**
 *  @brief Pipe callback function
 *
 *  Pipe callback function
 *
 *  @param [in] hdlr Handler
 *  @param [in] event Event
 *  @param [in] info Infomation
 *
 *  @return >=0 success, <0 failure
 */
static int AppLibStillEnc_PipeCallback(void *hdlr,UINT32 event, void *info)
{
    switch (event) {
    case AMP_ENC_EVENT_STATE_CHANGED:
        {
            AMP_ENC_STATE_CHANGED_INFO_s *inf = info;

            DBGMSG("[Applib - StillEnc] <PipeCallback> Pipe[%X] AMP_ENC_EVENT_STATE_CHANGED newState %X", hdlr, inf->newState);
            switch (inf->newState) {
            case AMP_ENC_PIPE_STATE_IDLE:
                AppLibComSvcHcmgr_SendMsgNoWait(HMSG_RECORDER_STATE_IDLE, 0, 0);
                break;
            case AMP_ENC_PIPE_STATE_LIVEVIEW:
                AppLibComSvcHcmgr_SendMsgNoWait(HMSG_RECORDER_STATE_LIVEVIEW, 0, 0);
                break;
            case AMP_ENC_PIPE_STATE_RECORDING:
                AppLibComSvcHcmgr_SendMsgNoWait(HMSG_RECORDER_STATE_RECORDING, 0, 0);
                break;
            case AMP_ENC_PIPE_STATE_RECORDING_PAUSED:
                AppLibComSvcHcmgr_SendMsgNoWait(HMSG_RECORDER_STATE_RECORDING_PAUSE, 0, 0);
                break;
            case AMP_ENC_PIPE_STATE_SCRIPT_PROCESSING:
                break;
            default:
                AmbaPrint("[Applib - StillEnc] <PipeCallback> Unknown event %X info: %x", event, info);
                break;
            }
        }
        break;
    }
    return 0;

}

/**
 *  @brief Codec Callback function
 *
 *  Codec Callback function
 *
 *  @param [in] hdlr Handler
 *  @param [in] event Event
 *  @param [in] info Infomation
 *
 *  @return >=0 success, <0 failure
 */
static int AppLibStillEnc_StillEncCallback(void *hdlr,UINT32 event, void *info)
{
    switch (event) {
    case AMP_ENC_EVENT_LIVEVIEW_RAW_READY:
        break;
    case AMP_ENC_EVENT_LIVEVIEW_DCHAN_YUV_READY:
        {
        //AMP_ENC_YUV_INFO_s *ptr = info;
        //AMP_ENC_YUV_INFO_s inf = *ptr;   // must copy to local. caller won't keep it after function exit
        //AmbaPrint("[Applib - StillEnc] <StillEncCallback> AMP_StillEnc_MSG_LIVEVIEW_DCHAN_YUV_READY addr: %X p:%d %dx%d", inf.yAddr, inf.pitch, inf.Width, inf.Height);
        }
        break;
    case AMP_ENC_EVENT_LIVEVIEW_FCHAN_YUV_READY:
        {
        //   AMP_ENC_YUV_INFO_s *ptr = info;
        //   AMP_ENC_YUV_INFO_s inf = *ptr;   // must copy to local. caller won't keep it after function exit
        //   AmbaPrint("Applib - StillEnc]: <StillEncCallback> AMP_StillEnc_MSG_LIVEVIEW_FCHAN_YUV_READY info: %X", info);
        }
        break;
    case AMP_ENC_EVENT_VCAP_2ND_YUV_READY:
        DBGMSG("[Applib - StillEnc] <StillEncCallback> AMP_ENC_EVENT_VCAP_2ND_YUV_READY");
        break;
    case AMP_ENC_EVENT_QUICKVIEW_DCHAN_YUV_READY:
        DBGMSG("[Applib - StillEnc] <StillEncCallback> AMP_ENC_EVENT_QUICKVIEW_DCHAN_YUV_READY");
        if (LcdQvTurnOn == 0) {
            AMP_DISP_WINDOW_CFG_s Window;
            AMP_DISP_INFO_s DispDev = {0};
            APPLIB_VOUT_PREVIEW_PARAM_s PreviewParam = {0};
            APPLIB_SENSOR_STILLPREV_CONFIG_s *StillLiveViewConfigData = NULL;
            UINT8 ReturnValue = 0;

            StillLiveViewConfigData = (APPLIB_SENSOR_STILLPREV_CONFIG_s *)AppLibSysSensor_GetPhotoLiveviewConfig(AppLibStillEnc_GetPhotoPjpegCapMode(), AppLibStillEnc_GetPhotoPjpegConfigId());
            PreviewParam.AspectRatio = StillLiveViewConfigData->VAR;
            LcdQvTurnOn = 1;

            ReturnValue = AppLibDisp_GetDeviceInfo(DISP_CH_DCHAN, &DispDev);
            if ((ReturnValue >= 0) && DispDev.DeviceInfo.Enable == 1) {
                PreviewParam.ChanID = DISP_CH_DCHAN;
                AppLibDisp_CalcPreviewWindowSize(&PreviewParam);
                memset(&Window, 0x0, sizeof(AMP_DISP_WINDOW_CFG_s));
                Window.Source = AMP_DISP_ENC;
                Window.SourceDesc.Enc.VinCh = AppEncChannel;
                Window.CropArea.Width = 0;
                Window.CropArea.Height = 0;
                Window.CropArea.X = 0;
                Window.CropArea.Y = 0;
                Window.TargetAreaOnPlane.Width = PreviewParam.Preview.Width;
                Window.TargetAreaOnPlane.Height = PreviewParam.Preview.Height;
                Window.TargetAreaOnPlane.X = PreviewParam.Preview.X;
                Window.TargetAreaOnPlane.Y = PreviewParam.Preview.Y;
                AppLibDisp_SetWindowConfig(DISP_CH_DCHAN, AppLibDisp_GetWindowId(DISP_CH_DCHAN, 0), &Window);
                AppLibDisp_ActivateWindow(DISP_CH_DCHAN, AppLibDisp_GetWindowId(DISP_CH_DCHAN, 0));
                AppLibDisp_FlushWindow(DISP_CH_DCHAN);
                AmbaPrint("[Applib - StillEnc] <StillEncCallback> QV_DCHAN_YUV_READY, switch to VDSRC");
            }
        }
        break;
    case AMP_ENC_EVENT_QUICKVIEW_FCHAN_YUV_READY:
        DBGMSG("[Applib - StillEnc] <StillEncCallback> AMP_ENC_EVENT_QUICKVIEW_FCHAN_YUV_READY");
        if (TVQvTurnOn == 0) {
            AMP_DISP_WINDOW_CFG_s Window;
            AMP_DISP_INFO_s DispDev = {0};
            APPLIB_VOUT_PREVIEW_PARAM_s PreviewParam = {0};
            APPLIB_SENSOR_STILLPREV_CONFIG_s *StillLiveViewConfigData = NULL;
            UINT8 ReturnValue = 0;

            StillLiveViewConfigData = (APPLIB_SENSOR_STILLPREV_CONFIG_s *)AppLibSysSensor_GetPhotoLiveviewConfig(AppLibStillEnc_GetPhotoPjpegCapMode(), AppLibStillEnc_GetPhotoPjpegConfigId());
            PreviewParam.AspectRatio = StillLiveViewConfigData->VAR;
            TVQvTurnOn = 1;

            ReturnValue = AppLibDisp_GetDeviceInfo(DISP_CH_FCHAN, &DispDev);
            if ((ReturnValue >= 0) && DispDev.DeviceInfo.Enable == 1) {
                PreviewParam.ChanID = DISP_CH_FCHAN;
                AppLibDisp_CalcPreviewWindowSize(&PreviewParam);
                memset(&Window, 0x0, sizeof(AMP_DISP_WINDOW_CFG_s));
                Window.Source = AMP_DISP_ENC;
                Window.SourceDesc.Enc.VinCh = AppEncChannel;
                Window.CropArea.Width = 0;
                Window.CropArea.Height = 0;
                Window.CropArea.X = 0;
                Window.CropArea.Y = 0;
                Window.TargetAreaOnPlane.Width = PreviewParam.Preview.Width;
                Window.TargetAreaOnPlane.Height = PreviewParam.Preview.Height;
                Window.TargetAreaOnPlane.X = PreviewParam.Preview.X;
                Window.TargetAreaOnPlane.Y = PreviewParam.Preview.Y;
                AppLibDisp_SetWindowConfig(DISP_CH_FCHAN, AppLibDisp_GetWindowId(DISP_CH_FCHAN, 0), &Window);
                AppLibDisp_ActivateWindow(DISP_CH_FCHAN, AppLibDisp_GetWindowId(DISP_CH_FCHAN, 0));
                AppLibDisp_FlushWindow(DISP_CH_FCHAN);
                AmbaPrint("[Applib - StillEnc] <StillEncCallback> QV_FCHAN_YUV_READY, switch to VDSRC");
            }
        }
        break;
    case AMP_ENC_EVENT_DATA_OVER_RUNOUT_THRESHOLD:
        DBGMSG("[Applib - StillEnc] <StillEncCallback> AMP_ENC_EVENT_DATA_OVER_RUNOUT_THRESHOLD !");
        AppLibComSvcHcmgr_SendMsgNoWait(HMSG_MEMORY_FIFO_BUFFER_RUNOUT, 0, 0);
        break;
    case AMP_ENC_EVENT_DATA_OVERRUN:
        DBGMSG("[Applib - StillEnc] <StillEncCallback> AMP_ENC_EVENT_DATA_OVERRUN !");
        break;
    case AMP_ENC_EVENT_DESC_OVERRUN:
        DBGMSG("[Applib - StillEnc] <StillEncCallback> AMP_ENC_EVENT_DESC_OVERRUN !");
        break;
    case AMP_ENC_EVENT_VCAP_YUV_READY:
        break;
    case AMP_ENC_EVENT_RAW_CAPTURE_DONE:
        AppLibComSvcHcmgr_SendMsgNoWait(HMSG_RECORDER_STATE_PHOTO_CAPTURE_COMPLETE, 0, 0);
        DBGMSG("[Applib - StillEnc] <StillEncCallback> AMP_ENC_EVENT_RAW_CAPTURE_DONE");
        break;
    case AMP_ENC_EVENT_BACK_TO_LIVEVIEW:
        DBGMSG("[Applib - StillEnc] <StillEncCallback> AMP_ENC_EVENT_BACK_TO_LIVEVIEW");
        break;
    case AMP_ENC_EVENT_BG_PROCESS_DONE:
        LcdQvTurnOn = 0;
        AppLibComSvcHcmgr_SendMsgNoWait(HMSG_RECORDER_STATE_PHOTO_BGPROC_COMPLETE, 0, 0);
        DBGMSG("[Applib - StillEnc] <StillEncCallback> AMP_ENC_EVENT_BG_PROCESS_DONE");
        break;
    default:
        AmbaPrint("[Applib - StillEnc] <StillEncCallback> Unknown event %X info: %x", event, info);
        break;
    }
    return 0;
}

/**
 *  @brief Vin event callback function
 *
 *  Vin callback function
 *
 *  @param [in] hdlr Handler
 *  @param [in] event Event
 *  @param [in] info Infomation
 *
 *  @return >=0 success, <0 failure
 */
static int AppLibStillEnc_VinEventCallback(void *hdlr,UINT32 event, void *info)
{
    switch (event) {
    case AMP_VIN_EVENT_FRAME_READY:
        //AmbaPrint("[Applib - StillEnc] <VinEventCallback> AMP_VIN_EVENT_FRAME_READY info: %X", info);
        {
            //static int count = 0;
            //count++;
            //if (count % 30 == 0) {
            //  AmbaPrint("[Applib - StillEnc] <StillEncCallback> AMP_VIN_EVENT_FRAME_READY info: %X", info);
            //}
        }
        break;
    case AMP_VIN_EVENT_FRAME_DROPPED:
        DBGMSG("[Applib - StillEnc] <VinEventCallback> AMP_VIN_EVENT_FRAME_DROPPED info: %X", info);
        break;
    default:
        AmbaPrint("[Applib - StillEnc] <VinEventCallback> Unknown %X info: %x", event, info);
        break;
    }
    return 0;
}

/**
 *  @brief Vin switch callback function
 *
 *  Vin callback function
 *
 *  @param [in] hdlr Handler
 *  @param [in] event Event
 *  @param [in] info Infomation
 *
 *  @return >=0 success, <0 failure
 */
static int AppLibStillEnc_VinSwitchCallback(void *hdlr, UINT32 event, void *info)
{
    AMBA_DSP_IMG_MODE_CFG_s ImgMode;
    extern int ApplibCalibinitFlag;
    switch (event) {
    case AMP_VIN_EVENT_INVALID:
        DBGMSG("[Applib - StillEnc] <VinSwitchCallback> AMP_VIN_EVENT_INVALID info: %X", info);
        AppLibImage_VinInvalidCallbackFunc(hdlr, event, info);
        break;
    case AMP_VIN_EVENT_VALID:
        DBGMSG("[Applib - StillEnc] <VinSwitchCallback> AMP_VIN_EVENT_VALID info: %X", info);
        AppLibImage_VinValidCallbackFunc(hdlr, event, info);
        break;
    case AMP_VIN_EVENT_CHANGED_PRIOR:
        DBGMSG("[Applib - StillEnc] <VinSwitchCallback> AMP_VIN_EVENT_CHANGED_PRIOR info: %X", info);
        AppLibImage_VinChangedPriorCallbackFunc(hdlr, event, info);

        {
            int ReturnValue = 0;
            AMP_DISP_WINDOW_CFG_s Window;
            AMP_DISP_INFO_s DispDev = {0};
            APPLIB_VOUT_PREVIEW_PARAM_s PreviewParam = {0};
            APPLIB_SENSOR_STILLPREV_CONFIG_s *StillLiveViewConfigData = NULL;
            memset(&Window, 0x0, sizeof(AMP_DISP_WINDOW_CFG_s));
            StillLiveViewConfigData = (APPLIB_SENSOR_STILLPREV_CONFIG_s *)AppLibSysSensor_GetPhotoLiveviewConfig(AppLibStillEnc_GetPhotoPjpegCapMode(), AppLibStillEnc_GetPhotoPjpegConfigId());
            PreviewParam.AspectRatio = StillLiveViewConfigData->VAR;

            /* FCHAN Window*/
            ReturnValue = AppLibDisp_GetDeviceInfo(DISP_CH_FCHAN, &DispDev);
            if ((ReturnValue < 0) || (DispDev.DeviceInfo.Enable == 0)) {
                AmbaPrintColor(RED,"[Applib - StillEnc] FChan Disable. Disable the fchan Window");
                AppLibDisp_DeactivateWindow(DISP_CH_FCHAN, AppLibDisp_GetWindowId(DISP_CH_FCHAN, 0));
            } else {
                PreviewParam.ChanID = DISP_CH_FCHAN;
                AppLibDisp_CalcPreviewWindowSize(&PreviewParam);
                AppLibDisp_GetWindowConfig(DISP_CH_FCHAN, AppLibDisp_GetWindowId(DISP_CH_FCHAN, 0), &Window);
                Window.Source = AMP_DISP_ENC;
                Window.SourceDesc.Enc.VinCh = AppEncChannel;
                Window.CropArea.Width = 0;
                Window.CropArea.Height = 0;
                Window.CropArea.X = 0;
                Window.CropArea.Y = 0;
                Window.TargetAreaOnPlane.Width = PreviewParam.Preview.Width;
                Window.TargetAreaOnPlane.Height = PreviewParam.Preview.Height;
                Window.TargetAreaOnPlane.X = PreviewParam.Preview.X;
                Window.TargetAreaOnPlane.Y = PreviewParam.Preview.Y;
                AppLibDisp_SetWindowConfig(DISP_CH_FCHAN, AppLibDisp_GetWindowId(DISP_CH_FCHAN, 0), &Window);
                AppLibDisp_ActivateWindow(DISP_CH_FCHAN, AppLibDisp_GetWindowId(DISP_CH_FCHAN, 0));
            }

            /* DCHAN Window*/
            ReturnValue = AppLibDisp_GetDeviceInfo(DISP_CH_DCHAN, &DispDev);
            if ((ReturnValue < 0) || (DispDev.DeviceInfo.Enable == 0)) {
                AmbaPrintColor(RED,"[Applib - StillEnc] DChan Disable. Disable the Dchan Window");
                AppLibDisp_DeactivateWindow(DISP_CH_FCHAN, AppLibDisp_GetWindowId(DISP_CH_FCHAN, 0));
            } else {
                PreviewParam.ChanID = DISP_CH_DCHAN;
                AppLibDisp_CalcPreviewWindowSize(&PreviewParam);
                AppLibDisp_GetWindowConfig(DISP_CH_DCHAN, AppLibDisp_GetWindowId(DISP_CH_DCHAN, 0), &Window);
                Window.Source = AMP_DISP_ENC;
                Window.SourceDesc.Enc.VinCh = AppEncChannel;
                Window.CropArea.Width = 0;
                Window.CropArea.Height = 0;
                Window.CropArea.X = 0;
                Window.CropArea.Y = 0;
                Window.TargetAreaOnPlane.Width = PreviewParam.Preview.Width;
                Window.TargetAreaOnPlane.Height = PreviewParam.Preview.Height;
                Window.TargetAreaOnPlane.X = PreviewParam.Preview.X;
                Window.TargetAreaOnPlane.Y = PreviewParam.Preview.Y;
                AppLibDisp_SetWindowConfig(DISP_CH_DCHAN, AppLibDisp_GetWindowId(DISP_CH_DCHAN, 0), &Window);
                AppLibDisp_ActivateWindow(DISP_CH_DCHAN, AppLibDisp_GetWindowId(DISP_CH_DCHAN, 0));
            }
            AppLibDisp_FlushWindow(DISP_CH_FCHAN | DISP_CH_DCHAN);
        }

        break;
    case AMP_VIN_EVENT_CHANGED_POST:
        DBGMSG("[Applib - StillEnc] <VinSwitchCallback> AMP_VIN_EVENT_CHANGED_POST info: %X", info);
	{
            AMBA_SENSOR_STATUS_INFO_s SensorStatus = {0};// Inform 3A LV sensor mode is Hdr or not
            UINT8 IsSensorHdrMode,HdrEnable;
            AMBA_IMG_SCHDLR_UPDATE_CFG_s ISCfg;
            APPLIB_SENSOR_VIDEO_ENC_CONFIG_s *VideoEncConfigData = AppLibSysSensor_GetVideoConfig(AppLibVideoEnc_GetSensorVideoRes());
            UINT32 IQChannelCount =  AppLibSysSensor_GetIQChannelCount();

            AmbaImg_Proc_Cmd(MW_IP_GET_VIDEO_HDR_ENABLE, 0, (UINT32)&HdrEnable, 0);

            /**set HDR mode according to sensor mode*/
            AmbaSensor_GetStatus(AppEncChannel, &SensorStatus);
            IsSensorHdrMode = (SensorStatus.ModeInfo.HdrInfo.HdrType == AMBA_SENSOR_HDR_TYPE_MULTI_SLICE)? 1: 0;
            AmbaImg_Proc_Cmd(MW_IP_SET_VIDEO_HDR_ENABLE, 0/*ChNo*/, (UINT32)IsSensorHdrMode, 0);

            if (HdrEnable != IsSensorHdrMode) {
                /**if previous mode is HDR mode reload normal IQ parameter*/
                AppLibIQ_ParamInit(IQChannelCount);
            }

            ISCfg.VideoProcMode = 0;
            /* Keep the report rate lower than 60fps. */
            if ((VideoEncConfigData->EncNumerator/VideoEncConfigData->EncDenominator) >= 200) {
                ISCfg.AAAStatSampleRate= 4;
            } else if ((VideoEncConfigData->EncNumerator/VideoEncConfigData->EncDenominator) >= 100) {
                ISCfg.AAAStatSampleRate  = 2;
            } else {
                ISCfg.AAAStatSampleRate  = 1;
            }
            AppLibImage_UpdateImgSchdlr(&ISCfg, 0);

            /**App default is experss_mode*/
            AmbaImg_Proc_Cmd(MW_IP_SET_PIPE_MODE, 0/*ChNo*/, (UINT32)IP_EXPERSS_MODE, 0);

            /**App default is liso_mode*/
            AmbaImg_Proc_Cmd(MW_IP_SET_VIDEO_ALGO_MODE, 0/*ChNo*/, (UINT32)IP_MODE_LISO_VIDEO, 0);
        }
        //inform 3A LiveView type
        AppLibImage_Set3APhotoMode(1);
        AppLibImage_VinChangedPostCallbackFunc(hdlr, event, info);

        // Slow Shutter
        {
            APPLIB_SENSOR_STILLPREV_CONFIG_s *StillLiveViewConfigData;
            AE_CONTROL_s AeCtrlMode;

            StillLiveViewConfigData = (APPLIB_SENSOR_STILLPREV_CONFIG_s *)AppLibSysSensor_GetPhotoLiveviewConfig(AppLibStillEnc_GetPhotoPjpegCapMode(), AppLibStillEnc_GetPhotoPjpegConfigId());
            AmbaImg_Proc_Cmd(MW_IP_GET_MULTI_AE_CONTROL_CAPABILITY, 0, (UINT32)&AeCtrlMode,0);
            if (ApplibStillEncLiveViewSlowShutterEnable) {
                int FrameRate = 0;

                FrameRate = StillLiveViewConfigData->TimeScale / StillLiveViewConfigData->TickPerPicture;
                if (FrameRate > 30) {
                    AeCtrlMode.SlowShutter = 1;
                    AeCtrlMode.SlowShutterFps = 0;
                } else {
                    AeCtrlMode.SlowShutter = 0;
                }
            } else {
                AeCtrlMode.SlowShutter = 0;
            }
            AmbaImg_Proc_Cmd(MW_IP_SET_MULTI_AE_CONTROL_CAPABILITY, 0, (UINT32)&AeCtrlMode,0);
        }

        memset(&ImgMode, 0, sizeof(AMBA_DSP_IMG_MODE_CFG_s));
        ImgMode.Pipe      = AMBA_DSP_IMG_PIPE_VIDEO;
        ImgMode.AlgoMode  = AMBA_DSP_IMG_ALGO_MODE_LISO;
        ImgMode.BatchId   = AMBA_DSP_VIDEO_FILTER;
        ImgMode.ContextId = 0;
        ImgMode.ConfigId  = 0;
        AppLibCalib_SetDspMode(&ImgMode);

        if (ApplibCalibinitFlag == 0) {
            AppLibCalib_Init();
            ApplibCalibinitFlag = 1;
        } else{
            AppLibCalibVignette_MapUpdate();
            AppLibCalibBPC_MapUpdate(0);
        }

        break;
    default:
        AmbaPrint("[Applib - StillEnc] <VinSwitchCallback> Unknown event %X info: %x", event, info);
        break;
    }
    return 0;
}

/**
 * Generic StillEnc ImgSchdlr callback
 *
 * @param [in] hdlr  The event belongs to which vin
 * @param [in] event The event
 * @param [in] info Information the event brings
 *
 * @return 0
 */
static int AppLibStillEnc_ImgSchdlrCallback(void *hdlr,UINT32 event, void *info)
{
    switch (event) {
        case AMBA_IMG_SCHDLR_EVENT_CFA_STAT_READY:
        {
            AMBA_IMG_SCHDLR_META_CFG_s *Meta = (AMBA_IMG_SCHDLR_META_CFG_s *)info;
            AppLibImage_CfaHandler(hdlr, event, (void *)Meta);
        }
            break;
        case AMBA_IMG_SCHDLR_EVENT_RGB_STAT_READY:
        {
            AMBA_IMG_SCHDLR_META_CFG_s *Meta = (AMBA_IMG_SCHDLR_META_CFG_s *)info;
            AppLibImage_VDspRgbHandler(hdlr, event, (void *)Meta);
        }
            break;
        default:
            AmbaPrint("[Applib - StillEnc] <ImgSchdlrCallback> Unknown %X info: %x", event, info);
            break;
    }
    return 0;
}

/**
 *  @brief Initial the live view
 *
 *  Initial the live view
 *
 *  @return >=0 success, <0 failure
 */
int AppLibStillEnc_LiveViewInit(void)
{
    int ReturnValue = 0;
    APPLIB_SENSOR_STILLPREV_CONFIG_s *StillLiveViewConfigData;
    StillLiveViewConfigData = (APPLIB_SENSOR_STILLPREV_CONFIG_s *)AppLibSysSensor_GetPhotoLiveviewConfig(AppLibStillEnc_GetPhotoPjpegCapMode(), AppLibStillEnc_GetPhotoPjpegConfigId());
    DBGMSGc2(BLUE,"[Applib - StillEnc] <LiveViewInit> Start");
    /* Config the vin. */
    {
        AMP_VIN_CFG_s VinCfg = {0};
        AMP_VIN_LAYOUT_CFG_s Layout;
        AMBA_SENSOR_MODE_ID_u Mode = {0};
        AMBA_SENSOR_MODE_INFO_s VinInfo;
        memset(&Layout, 0x0, sizeof(AMP_VIN_LAYOUT_CFG_s));
        memset(&VinInfo, 0x0, sizeof(AMBA_SENSOR_MODE_INFO_s));

        Mode.Data = AppLibSysSensor_GetPhotoLiveViewModeID(AppLibStillEnc_GetPhotoPjpegCapMode(), AppLibStillEnc_GetPhotoPjpegConfigId());

        AmbaSensor_GetModeInfo(AppEncChannel, Mode, &VinInfo);

        VinCfg.Mode = Mode;
        VinCfg.Channel = AppEncChannel;
        VinCfg.LayoutNumber = 1;
        VinCfg.HwCaptureWindow.Width = StillLiveViewConfigData->LVCaptureWidth;
        VinCfg.HwCaptureWindow.Height = StillLiveViewConfigData->LVCaptureHeight;
        VinCfg.HwCaptureWindow.X = VinInfo.OutputInfo.RecordingPixels.StartX +
            (((VinInfo.OutputInfo.RecordingPixels.Width - VinCfg.HwCaptureWindow.Width)/2)&0xFFF8);
        VinCfg.HwCaptureWindow.Y = (VinInfo.OutputInfo.RecordingPixels.StartY +
            ((VinInfo.OutputInfo.RecordingPixels.Height - VinCfg.HwCaptureWindow.Height)/2)) & 0xFFFE;
        Layout.Width = StillLiveViewConfigData->LVMainWidth;
        Layout.Height = StillLiveViewConfigData->LVMainHeight;
        Layout.EnableSourceArea = 0; // Get all capture window to main
        Layout.DzoomFactorX = 1<<16;
        Layout.DzoomFactorY = 1<<16;
        Layout.DzoomOffsetX = 0;
        Layout.DzoomOffsetY = 0;
        VinCfg.Layout = &Layout;
        VinCfg.cbEvent = AppLibStillEnc_VinEventCallback;
        VinCfg.cbSwitch = AppLibStillEnc_VinSwitchCallback;
        AppLibSysVin_Config(&VinCfg);
        DBGMSGc2(GREEN,"[Applib - StillEnc] AmpVin_ConfigHandler ReturnValue = %d",ReturnValue);
    }

    /* Create still encoder objects */
    if (StillEncPri == NULL) {
        AMP_STILLENC_HDLR_CFG_s EncCfg;
        AMP_VIDEOENC_LAYER_DESC_s Layer = {0, 0, 0, AMP_ENC_SOURCE_VIN, 0, 0, {0,0,0,0},{0,0,0,0}};
        memset(&EncCfg, 0x0, sizeof(AMP_STILLENC_HDLR_CFG_s));
        EncCfg.MainLayout.Layer = &Layer;
        AmpStillEnc_GetDefaultCfg(&EncCfg);

        /* Assign callback */
        EncCfg.cbEvent = AppLibStillEnc_StillEncCallback;

        /* Assign main Layout */
        EncCfg.MainLayout.Width = StillLiveViewConfigData->LVMainWidth;
        EncCfg.MainLayout.Height = StillLiveViewConfigData->LVMainHeight;
        EncCfg.MainLayout.LayerNumber = 1;
        EncCfg.Interlace = 0;
        EncCfg.MainTimeScale = StillLiveViewConfigData->TimeScale;
        EncCfg.MainTickPerPicture = StillLiveViewConfigData->TickPerPicture;

        EncCfg.DspWorkBufAddr = ApplibDspWorkAreaResvStart;
        EncCfg.DspWorkBufSize = ApplibDspWorkAreaResvSize;
#if 0
        // Assign bitstream/descriptor buffer
        ReturnValue = AmpUtil_GetAlignedPool(APPLIB_G_MMPL, (void **)&StillBitsBuf, &StillBitsBufRaw, STILL_BISFIFO_SIZE, 32);
        if (ReturnValue != OK) {
            AmbaPrintColor(RED, "[Applib - StillEnc] Out of cached memory for bitsFifo!!");
        }
        StillBitsBuf = (UINT8 *)ALIGN_32((UINT32)StillBitsBuf);
        EncCfg.BitsBufCfg.BitsBufSize = STILL_BISFIFO_SIZE;
        ReturnValue = AmpUtil_GetAlignedPool(APPLIB_G_MMPL, (void **)&StillDescBuf, &StillDescBufRaw, STILL_DESC_SIZE, 32);
        if (ReturnValue != OK) {
            AmbaPrintColor(RED, "[Applib - StillEnc] Out of cached memory for DescFifo!!");
        }
        StillDescBuf = (UINT8 *)ALIGN_32((UINT32)StillDescBuf);
        EncCfg.BitsBufCfg.DescBufSize = STILL_DESC_SIZE;
#else
        AppLibRecorderMemMgr_BufAllocate();
        AppLibRecorderMemMgr_GetBufAddr(&StillBitsBuf, &StillDescBuf);
        AppLibRecorderMemMgr_GetBufSize(&EncCfg.BitsBufCfg.BitsBufSize, &EncCfg.BitsBufCfg.DescBufSize);
#endif
        EncCfg.BitsBufCfg.BitsBufAddr = StillBitsBuf;


        EncCfg.BitsBufCfg.DescBufAddr = StillDescBuf;

        EncCfg.BitsBufCfg.BitsRunoutThreshold = STILL_BISFIFO_SIZE - 4*1024*1024; // leave 4MB
        DBGMSG("[Applib - StillEnc] Bits 0x%X size %x Desc 0x%X size %d", StillBitsBuf, STILL_BISFIFO_SIZE, StillDescBuf, STILL_DESC_SIZE);

        Layer.SourceType = AMP_ENC_SOURCE_VIN;
        Layer.Source = AppVinA;
        Layer.SourceLayoutId = 0;
        Layer.EnableSourceArea = 0;  // No source cropping
        Layer.EnableTargetArea = 0;  // No target pip
        DBGMSG("[Applib - StillEnc] Enc create %d %d %d %d", EncCfg.MainLayout.Width, EncCfg.MainLayout.Height, \
        EncCfg.MainTimeScale, EncCfg.MainTickPerPicture);
        ReturnValue = AmpStillEnc_Create(&EncCfg, &StillEncPri);
        if (ReturnValue != OK) {
            AmbaPrintColor(RED,"[Applib - StillEnc] <AmpStillEnc_Create> failure!!");
        }
    }
    // Register pipeline
    if (StillEncPipe == NULL) {
        AMP_ENC_PIPE_CFG_s PipeCfg;
        memset(&PipeCfg, 0x0, sizeof(AMP_ENC_PIPE_CFG_s));

        // Register pipeline
        AmpEnc_GetDefaultCfg(&PipeCfg);
        PipeCfg.encoder[0] = StillEncPri;
        PipeCfg.numEncoder = 1;
        PipeCfg.cbEvent = AppLibStillEnc_PipeCallback;
        PipeCfg.type = AMP_ENC_STILL_PIPE;
        ReturnValue = AmpEnc_Create(&PipeCfg, &StillEncPipe);
        if (ReturnValue != OK) {
            AmbaPrintColor(RED,"[Applib - StillEnc] <AmpEnc_Create> failure!!");
        }

        ReturnValue = AmpEnc_Add(StillEncPipe);
        if (ReturnValue != OK) {
            AmbaPrintColor(RED,"[Applib - StillEnc] <AmpEnc_Add> failure!!");
        }
    }
    {
        AMBA_IMG_SCHDLR_CFG_s ISCfg = {0};
        ISCfg.MainViewID = 0;
        ISCfg.Channel = AppEncChannel;
        ISCfg.Vin = AppVinA;
        ISCfg.cbEvent = AppLibStillEnc_ImgSchdlrCallback;
        ReturnValue = AppLibImage_CreateImgSchdlr(&ISCfg, 0);
        if (ReturnValue != OK) {
            AmbaPrintColor(RED,"[Applib - StillEnc] <AppLibImage_CreateImgSchdlr> Out of NC memory for bitsFifo!!");
        }
    }

    ApplibStillEncLiveviewInitFlag = 0;
    DBGMSGc2(BLUE,"[Applib - StillEnc] <LiveViewInit> End");
    return 0;
}

/**
 *  @brief Configure the live view
 *
 *  Configure the live view
 *
 *  @return >=0 success, <0 failure
 */
int AppLibStillEnc_LiveViewSetup(void)
{
    AMP_VIN_RUNTIME_CFG_s VinCfg = {0};
    AMP_STILLENC_MAIN_CFG_s MainCfg = {0};
    AMP_VIDEOENC_LAYER_DESC_s NewLayer = {0, 0, 0, AMP_ENC_SOURCE_VIN, 0, 0, {0,0,0,0},{0,0,0,0}};
    AMBA_SENSOR_MODE_ID_u Mode = {0};
    AMBA_SENSOR_MODE_INFO_s VinInfo;
    AMP_VIN_LAYOUT_CFG_s Layout;
    APPLIB_SENSOR_STILLPREV_CONFIG_s *StillLiveViewConfigData;

    UINT32 DdrClk = 0;
    UINT16 CustomMaxCortexFreq = 0;
    UINT16 CustomMaxIdspFreq = 0;
    UINT16 CustomMaxCoreFreq = 0;

    int ReturnValue = 0;
    memset(&Layout, 0x0, sizeof(AMP_VIN_LAYOUT_CFG_s));
    memset(&VinCfg, 0x0, sizeof(AMP_VIN_RUNTIME_CFG_s));
    memset(&VinInfo, 0x0, sizeof(AMBA_SENSOR_MODE_INFO_s));
    memset(&MainCfg, 0x0, sizeof(AMP_STILLENC_MAIN_CFG_s));
    DBGMSGc2(BLUE,"[Applib - StillEnc] <LiveViewSetup> Start");
    if (ApplibStillEncLiveviewInitFlag < 0) {
        AppLibStillEnc_LiveViewInit();
    }

    StillLiveViewConfigData = (APPLIB_SENSOR_STILLPREV_CONFIG_s *)AppLibSysSensor_GetPhotoLiveviewConfig(AppLibStillEnc_GetPhotoPjpegCapMode(), AppLibStillEnc_GetPhotoPjpegConfigId());
    Mode.Data= AppLibSysSensor_GetPhotoLiveViewModeID(AppLibStillEnc_GetPhotoPjpegCapMode(), AppLibStillEnc_GetPhotoPjpegConfigId());
    AmbaSensor_GetModeInfo(AppEncChannel, Mode, &VinInfo);

    VinCfg.Hdlr = AppVinA;
    VinCfg.Mode = Mode;
    VinCfg.LayoutNumber = 1;
    VinCfg.HwCaptureWindow.Width = StillLiveViewConfigData->LVCaptureWidth;
    VinCfg.HwCaptureWindow.Height = StillLiveViewConfigData->LVCaptureHeight;
    VinCfg.HwCaptureWindow.X = VinInfo.OutputInfo.RecordingPixels.StartX + (((VinInfo.OutputInfo.RecordingPixels.Width - VinCfg.HwCaptureWindow.Width)/2)&0xFFF8);
    VinCfg.HwCaptureWindow.Y = VinInfo.OutputInfo.RecordingPixels.StartY + (((VinInfo.OutputInfo.RecordingPixels.Height - VinCfg.HwCaptureWindow.Height)/2) & 0xFFFE);

    Layout.Width = StillLiveViewConfigData->LVMainWidth;
    Layout.Height = StillLiveViewConfigData->LVMainHeight;
    Layout.EnableSourceArea = 0; // Get all capture window to main
    Layout.DzoomFactorX = 1<<16;
    Layout.DzoomFactorY = 1<<16;
    Layout.DzoomOffsetX = 0;
    Layout.DzoomOffsetY = 0;
    VinCfg.Layout = &Layout;

    MainCfg.Hdlr = StillEncPri;
    MainCfg.MainLayout.LayerNumber = 1;
    MainCfg.MainLayout.Layer = &NewLayer;
    MainCfg.MainLayout.Width = StillLiveViewConfigData->LVMainWidth;
    MainCfg.MainLayout.Height = StillLiveViewConfigData->LVMainHeight;
    MainCfg.Interlace = 0;
    MainCfg.MainTickPerPicture = StillLiveViewConfigData->TickPerPicture;
    MainCfg.MainTimeScale = StillLiveViewConfigData->TimeScale;
    NewLayer.EnableSourceArea = 0;
    NewLayer.EnableTargetArea = 0;
    NewLayer.LayerId = 0;
    NewLayer.SourceType = AMP_ENC_SOURCE_VIN;
    NewLayer.Source = AppVinA;
    NewLayer.SourceLayoutId = 0;

    MainCfg.DspWorkBufAddr = ApplibDspWorkAreaResvStart;
    MainCfg.DspWorkBufSize = ApplibDspWorkAreaResvSize;
    MainCfg.LiveViewProcMode = 0;
    MainCfg.LiveViewAlgoMode = 0;

    {
        extern UINT32 AmbaPLL_GetDdrClk(void);
        DdrClk = AmbaPLL_GetDdrClk();
    }
    if (DdrClk == 396000000) {
        CustomMaxCortexFreq = 504;
        CustomMaxIdspFreq = 276;
        CustomMaxCoreFreq = 288;
    } else if (DdrClk == 600000000) {
        CustomMaxCortexFreq = 800;
        CustomMaxIdspFreq = 504;
        CustomMaxCoreFreq = 396;
    }

    MainCfg.SysFreq.ArmCortexFreq = AMP_SYSTEM_FREQ_KEEPCURRENT;
    MainCfg.SysFreq.IdspFreq = AMP_SYSTEM_FREQ_POWERSAVING;
    MainCfg.SysFreq.CoreFreq = AMP_SYSTEM_FREQ_PERFORMANCE;
    MainCfg.SysFreq.MaxArmCortexFreq = CustomMaxCortexFreq;
    MainCfg.SysFreq.MaxIdspFreq = CustomMaxIdspFreq;
    MainCfg.SysFreq.MaxCoreFreq = CustomMaxCoreFreq;

    if (MainCfg.LiveViewProcMode == 0) {
        if (StillLiveViewConfigData->LVCaptureWidth > 1920 || \
            StillLiveViewConfigData->LVCaptureHeight > 1920)
            MainCfg.LiveViewOSMode = 1;
   }


    ReturnValue = AmpStillEnc_ConfigVinMain(1, &VinCfg, &MainCfg);
    if (ReturnValue != OK) {
        AmbaPrintColor(RED,"[Applib - StillEnc] <AmpStillEnc_ConfigVinMain> failure!!");
    }
    DBGMSGc2(BLUE,"[Applib - StillEnc] <LiveViewSetup> End");
    return 0;
}

/**
 *  @brief Start live view
 *
 *  Start live view
 *
 *  @return >=0 success, <0 failure
 */
int AppLibStillEnc_LiveViewStart(void)
{
    int ReturnValue = 0;
    DBGMSGc2(BLUE,"[Applib - StillEnc] <LiveViewStart> Start");
    ReturnValue = AmpEnc_StartLiveview(StillEncPipe, AMP_ENC_FUNC_FLAG_WAIT);
    if (ReturnValue != OK) {
        AmbaPrintColor(RED,"[Applib - StillEnc] <LiveViewStart> failure!!");
    }
    return ReturnValue;
}

/**
 *  @brief Stop live view
 *
 *  Stop live view
 *
 *  @return >=0 success, <0 failure
 */
int AppLibStillEnc_LiveViewStop(void)
{
    int ReturnValue = 0;
    DBGMSGc2(BLUE,"[Applib - StillEnc] <LiveViewStop> End");
    AppLibImage_StopImgSchdlr(0);
    AppLibImage_Stop3A(StillEncPri);
    ReturnValue = AmpEnc_StopLiveview(StillEncPipe, AMP_ENC_FUNC_FLAG_WAIT);
    if (ReturnValue != OK) {
        AmbaPrintColor(RED,"[Applib - StillEnc] <LiveViewStop> failure!!");
    } else {
        DBGMSGc2(GREEN,"[Applib - StillEnc] <LiveViewStop> success!!");
    }
    return ReturnValue;
}

/**
 *  @brief De-initial live view
 *
 *  De-initial live view
 *
 *  @return >=0 success, <0 failure
 */
int AppLibStillEnc_LiveViewDeInit(void)
{
    int ReturnValue = 0;

    AppLibImage_DeleteImgSchdlr(0);

#if 0
    if (StillEncPipe != NULL) {
        ReturnValue = AmpEnc_Delete(StillEncPipe);
        if (ReturnValue == OK) {
            StillEncPipe = NULL;
        } else {
            AmbaPrintColor(RED,"[Applib - StillEnc] <AmpVideoEnc_Delete> failure!!");
        }
    }

    if (StillEncPri != NULL) {
        //ReturnValue = AmpStillEnc_Delete(StillEncPri);
        if (ReturnValue == OK) {
            StillEncPri = NULL;
        } else {
            AmbaPrintColor(RED,"[Applib - StillEnc] <AmpVideoEnc_Delete> VideoEncPri failure!!");
        }
    }

    if (AppVinA != NULL) {
        ReturnValue = AmpVin_Delete(AppVinA);
        if (ReturnValue == OK) {
            AppVinA = NULL;
        } else {
            AmbaPrintColor(RED,"[Applib - StillEnc] <AmpVin_Delete> failure!!");
        }
    }
#endif
    ApplibStillEncLiveviewInitFlag = -1;

    return ReturnValue;
}

/**
 *  @brief delete still encode codec & pipe
 *
 *  delete still encode codec & pipe
 *
 *  @return >=0 success, <0 failure
 */
int AppLibStillEnc_DeletePipe(void)
{
    int ReturnValue = 0;

    /**delete still encode pipe, video encode will create PIV still encode pipe*/
    if (StillEncPipe != NULL) {
        ReturnValue = AmpEnc_Delete(StillEncPipe);
        if (ReturnValue == OK) {
            StillEncPipe = NULL;
        } else {
            AmbaPrintColor(RED,"[Applib - StillEnc] <LiveViewStop> failure!!");
        }
    }

    if (StillEncPri != NULL) {
        ReturnValue = AmpStillEnc_Delete(StillEncPri);
        if (ReturnValue == OK) {
            StillEncPri = NULL;
        } else {
            AmbaPrintColor(RED,"[Applib - StillEnc] <LiveViewStop> VideoEncPri failure!!");
        }
    }

    return ReturnValue;
}

static int AppLibStillEncPjpegConfigId = -1;
static int AppLibStillEncPjpegCaptureMode = -1;
static APPLIB_STILLENC_SETTING_s photoSetting = {0};
//==========================================================================
// Photo Setting APIs
//==========================================================================


/**
 *  @brief Set multiple frames capture mode
 *
 *  Set multiple frames capture mode
 *
 *  @param [in] capMode Capture mode
 *
 *  @return >=0 success, <0 failure
 */
int AppLibStillEnc_SetMultiCapMode(int capMode)
{
    photoSetting.MultiCaptureMode = capMode;

    switch (photoSetting.MultiCaptureMode) {
    case PHOTO_MULTI_CAP_AEB:
        AppLibStillEncPjpegCaptureMode = SENSOR_PHOTO_CAP_NORMAL;
        AppLibStillEncPjpegConfigId = photoSetting.SizeId;
        break;
    default:
        AppLibStillEncPjpegCaptureMode = SENSOR_PHOTO_CAP_NORMAL;
        AppLibStillEncPjpegConfigId = 0;    // 0 should be default photo config
        break;
    }

    return 0;
}

/**
 *  @brief Set normal capture mode
 *
 *  Set normal capture mode
 *
 *  @param [in] capMode Capture mode
 *
 *  @return >=0 success, <0 failure
 */
int AppLibStillEnc_SetNormCapMode(int capMode)
{
    photoSetting.NormalCapMode = capMode;

    if (photoSetting.MultiCaptureMode == PHOTO_MULTI_CAP_OFF) {
        switch (photoSetting.NormalCapMode) {
        case PHOTO_CAP_MODE_PRECISE:
        case PHOTO_CAP_MODE_PES:
        case PHOTO_CAP_MODE_PRE_CAPTURE:
            AppLibStillEncPjpegCaptureMode = SENSOR_PHOTO_CAP_NORMAL;
            AppLibStillEncPjpegConfigId = photoSetting.SizeId;
            break;
        case PHOTO_CAP_MODE_BURST:
            AppLibStillEncPjpegCaptureMode = SENSOR_PHOTO_CAP_BURST;
            AppLibStillEncPjpegConfigId = photoSetting.SizeId;
            break;
        default:
            AppLibStillEncPjpegCaptureMode = SENSOR_PHOTO_CAP_NORMAL;
            AppLibStillEncPjpegConfigId = 0;// 0 should be default photo config
            break;
        }
    }

    return 0;
}

/**
 *  @brief Set the photo size ID
 *
 *  Set the photo size ID
 *
 *  @param [in] size Photo size ID
 *
 *  @return >=0 success, <0 failure
 */
int AppLibStillEnc_SetSizeID(int size)
{
    photoSetting.SizeId = size;
    AppLibStillEncPjpegConfigId = photoSetting.SizeId;
    return 0;
}

/**
 *  @brief Set the photo quality mode
 *
 *  Set the photo quality mode
 *
 *  @param [in] qualityMode Quality mode
 *
 *  @return >=0 success, <0 failure
 */
int AppLibStillEnc_SetQualityMode(int qualityMode)
{
    photoSetting.QualityMode = qualityMode;
    photoSetting.Quality = AppLibSysSensor_GetPhotoQualityConfig(photoSetting.QualityMode);
    return 0;
}

/**
 *  @brief Set the size of thumbnail
 *
 *  Set the size of thumbnail
 *
 *  @param [in] width Width
 *  @param [in] height Height
 *
 *  @return >=0 success, <0 failure
 */
int AppLibStillEnc_SetPhotoThumbnailSize(UINT16 width, UINT16 height)
{
    photoSetting.ThumbnailWidth = width;
    photoSetting.ThumbnailHeight = height;
    return 0;
}

/**
 *  @brief Set the size of screen-nail
 *
 *  Set the size of screen-nail
 *
 *  @param [in] width Width
 *  @param [in] height Height
 *
 *  @return >=0 success, <0 failure
 */
int AppLibStillEnc_SetPhotoScreennailSize(UINT16 width, UINT16 height)
{
    photoSetting.ScreennailWidth = width;
    photoSetting.ScreennailHeight = height;
    return 0;
}

/**
 *  @brief Set the quality of thumbnail
 *
 *  Set the quality of thumbnail
 *
 *  @param [in] quality Quality
 *
 *  @return >=0 success, <0 failure
 */
int AppLibStillEnc_SetThumbnailQuality(UINT8 quality)
{
    photoSetting.ThumbnailQuality = quality;
    return 0;
}

/**
 *  @brief Set the quality of screen-nail
 *
 *  Set the quality of screen-nail
 *
 *  @param [in] quality Quality
 *
 *  @return >=0 success, <0 failure
 */
int AppLibStillEnc_SetScreennailQuality(UINT8 quality)
{
    photoSetting.ScreennailQuality = quality;
    return 0;
}

/**
 *  @brief Set the setting of quick view
 *
 *  Set the setting of quick view
 *
 *  @param [in] qv The setting of quick view
 *
 *  @return >=0 success, <0 failure
 */
int AppLibStillEnc_SetQuickview(int qv)
{
    photoSetting.QuickView = qv;
    return 0;
}

/**
 *  @brief Set the setting of quick view delay
 *
 *  Set the setting of quick view delay
 *
 *  @param [in] qvDelay The setting of quick view delay
 *
 *  @return >=0 success, <0 failure
 */
int AppLibStillEnc_SetQuickviewDelay(int qvDelay)
{
    photoSetting.QuickviewDelay = qvDelay;
    return 0;
}

/**
 *  @brief Set the setting of fast AF
 *
 *  Set the setting of fast AF
 *
 *  @param [in] enable Enable flag
 *
 *  @return >=0 success, <0 failure
 */
int AppLibStillEnc_SetFastAf(int enable)
{
    photoSetting.FastAF = enable;
    return 0;
}

/**
 *  @brief Set the shutter mode
 *
 *  Set the shutter mode
 *
 *  @param [in] mode The shutter mode
 *
 *  @return >=0 success, <0 failure
 */
int AppLibStillEnc_SetShutterMode(int mode)
{
    photoSetting.ShutterSetting = mode;
    return 0;
}

/**
 *  @brief Set the capture number
 *
 *  Set the capture number
 *
 *  @param [in] num Capture number
 *
 *  @return >=0 success, <0 failure
 */
int AppLibStillEnc_SetCaptureNum(int num)
{
    photoSetting.CaptureNumber = num;
    return 0;
}

/**
 *  Set slow shutter for liveview
 *
 *  @param [in] enable Enable the slow shutter.
 *
 *  @return >=0 success, <0 failure
 */
UINT32 AppLibStillEnc_SetEnalbeLiveViewSlowShutter(UINT8 enable)
{
    ApplibStillEncLiveViewSlowShutterEnable = enable;
    return 0;
}

/**
 *  @brief Get the photo module setting
 *
 *  Get the photo module setting
 *
 *  @param [out] setting The photo module setting
 *
 *  @return >=0 success, <0 failure
 */
int AppLibStillEnc_GetSetting(APPLIB_STILLENC_SETTING_s *setting)
{
    memcpy(setting, &photoSetting, sizeof(APPLIB_STILLENC_SETTING_s));
    return 0;
}

/**
 *  @brief Get the multiple frames capture mode
 *
 *  Get the multiple frames capture mode
 *
 *  @return The multiple frames capture mode
 */
int AppLibStillEnc_GetMultiCapMode(void)
{
    return photoSetting.MultiCaptureMode;
}

/**
 *  @brief Get the normal capture mode
 *
 *  Get the normal capture mode
 *
 *  @return The normal capture mode
 */
int AppLibStillEnc_GetNormCapMode(void)
{
    return photoSetting.NormalCapMode;
}

/**
 *  @brief Get the photo size ID
 *
 *  The photo size ID
 *
 *  @return The photo size ID
 */
int AppLibStillEnc_GetSizeID(void)
{
    return photoSetting.SizeId;
}

/**
 *  @brief Get the photo quality mode
 *
 *  Get the photo quality mode
 *
 *  @return The photo quality mode
 */
int AppLibStillEnc_GetQuality(void)
{
    return photoSetting.Quality;
}

/**
 *  @brief Get the size of thumbnail
 *
 *  Get the size of thumbnail
 *
 *  @param [out] width Width
 *  @param [out] height Height
 *
 *  @return >=0 success, <0 failure
 */
int AppLibStillEnc_GetPhotoThumbnailSize(UINT16 *width, UINT16 *height)
{
    *width = photoSetting.ThumbnailWidth;
    *height = photoSetting.ThumbnailHeight;
    return 0;
}

/**
 *  @brief Get the size of screen-nail
 *
 *  Get the size of screen-nail
 *
 *  @param [out] width Width
 *  @param [out] height Height
 *
 *  @return >=0 success, <0 failure
 */
int AppLibStillEnc_GetPhotoScreennailSize(UINT16 *width, UINT16 *height)
{
    *width = photoSetting.ScreennailWidth;
    *height = photoSetting.ScreennailHeight;
    return 0;
}

/**
 *  @brief Get the quality of thumbnail
 *
 *  Get the quality of thumbnail
 *
 *  @return The quality of thumbnail
 */
int AppLibStillEnc_GetThumbnailQuality(void)
{
    return photoSetting.ThumbnailQuality;
}

/**
 *  @brief Get the quality of screen-nail
 *
 *  Get the quality of screen-nail
 *
 *  @return The quality of screen-nail
 */
int AppLibStillEnc_GetScreennailQuality(void)
{
    return photoSetting.ScreennailQuality;
}

/**
 *  @brief Get the setting of quick view
 *
 *  Get the setting of quick view
 *
 *  @return The setting of quick view
 */
int AppLibStillEnc_GetQuickview(void)
{
    return photoSetting.QuickView;
}

/**
 *  @brief Get the setting of quick view delay
 *
 *  Get the setting of quick view delay
 *
 *  @return The setting of quick view delay
 */
int AppLibStillEnc_GetQuickviewDelay(void)
{
    return photoSetting.QuickviewDelay;
}

/**
 *  @brief Get the setting of fast AF
 *
 *  Get the setting of fast AF
 *
 *  @return The setting of fast AF
 */
int AppLibStillEnc_GetFastAf(void)
{
    return photoSetting.FastAF;
}

/**
 *  @brief Get the shutter mode
 *
 *  Get the shutter mode
 *
 *  @return The shutter mode
 */
int AppLibStillEnc_GetShutterMode(void)
{
    return photoSetting.ShutterSetting;
}

/**
 *  @brief Get the capture number
 *
 *  Get the capture number
 *
 *  @return The capture number
 */
int AppLibStillEnc_GetCaptureNum(void)
{
    APPLIB_SENSOR_STILLCAP_CONFIG_s *PjpegConfig;
    PjpegConfig = AppLibSysSensor_GetPjpegConfig(AppLibStillEncPjpegCaptureMode, AppLibStillEncPjpegConfigId);
    return PjpegConfig->CaptureNumber;
}

/**
 *  @brief Get slow shutter setting.
 *
 *  Get slow shutter setting.
 *
 *  @return Slow shutter setting
 */
UINT8 AppLibStillEnc_GetEnalbeLiveViewSlowShutter(void)
{
    return ApplibStillEncLiveViewSlowShutterEnable;
}

int AppLibStillEnc_GetYuvWorkingBuffer(UINT16 MainWidth, UINT16 MainHeight, UINT16 RawWidth, UINT16 RawHeight, UINT16 *BufWidth, UINT16 *BufHeight)
{
    *BufWidth = MAX(RawWidth, MainWidth);
    *BufWidth = ALIGN_32(*BufWidth*28/25);
    *BufHeight = MAX(RawHeight, MainHeight);
    *BufHeight = MAX(ALIGN_32(*BufHeight*27/25) + ALIGN_32((11*MainHeight+9)/10-MainHeight),
        (11*(*BufHeight+9))/10);

    return 0;
}


/**
 *  @brief Get the current capture mode
 *
 *  Get the current capture mode
 *
 *  @return The current capture mode
 */
int AppLibStillEnc_GetPhotoPjpegCapMode(void)
{
    return AppLibStillEncPjpegCaptureMode;
}

/**
 *  @brief Get the current capture size id
 *
 *  Get the current capture size id
 *
 *  @return The current capture size id
 */
int AppLibStillEnc_GetPhotoPjpegConfigId(void)
{
    return AppLibStillEncPjpegConfigId;
}

int AppLibStillEnc_EraseFifoCB(void *hdlr, UINT32 event, void* info)
{
    switch (event) {
    case AMP_FIFO_EVENT_DATA_READY:
        //AmbaPrintColor(GREEN,"[Applib - Format] <Mp4Mux_VideoFifoCB>: AMP_FIFO_EVENT_DATA_READY hdlr = 0x%x",hdlr);
        break;
    case AMP_FIFO_EVENT_DATA_EOS:
        //AmbaPrintColor(YELLOW,"[Applib - Format] <Mp4Mux_VideoFifoCB>: AMP_FIFO_EVENT_DATA_EOS hdlr = 0x%x",hdlr);
        break;
    default:
        AmbaPrint("[Applib - Format] <Mp4Mux_VideoFifoCB>: evnet 0x%x", event);
        break;
    }

    return 0;
}

/**
 *  @brief Erase data in fifo
 *
 *  Erase data in fifo
 *
 *
 *  @return =0 success, <0 failure
 */
int AppLibStillEnc_EraseFifo(void)
{
    int ReturnValue = 0;
    AMP_FIFO_CFG_s FifoDefCfg = {0};
    AMP_FIFO_HDLR_s *TempStillPriFifoHdlr = NULL;

    memset(&FifoDefCfg, 0x0, sizeof(AMP_FIFO_CFG_s));
    AmbaPrintColor(CYAN, "AppLibStillEnc_EraseFifo");

    AmpFifo_GetDefaultCfg(&FifoDefCfg);
    FifoDefCfg.hCodec = StillEncPri;
    AmbaPrintColor(CYAN, "StillEncPri codec 0x%X", StillEncPri);
    FifoDefCfg.IsVirtual = 1;
    FifoDefCfg.NumEntries = 1024;
    FifoDefCfg.cbEvent = AppLibStillEnc_EraseFifoCB;
    AmpFifo_Create(&FifoDefCfg, &TempStillPriFifoHdlr);
    AmpFifo_EraseAll(TempStillPriFifoHdlr);
    AmpFifo_Delete(TempStillPriFifoHdlr);
    DBGMSG("[Applib - Format] <EraseFIFO> Erase Still Primary");

    if (ApplibStillEncLiveviewInitFlag < 0) {
        AppLibStillEnc_DeletePipe();
    }
    return ReturnValue;
}

