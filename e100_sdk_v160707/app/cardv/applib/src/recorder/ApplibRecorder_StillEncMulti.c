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

//#define DEBUG_APPLIB_PHOTO
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

AMP_STLENC_HDLR_s *StillEncPri = NULL;
AMP_ENC_PIPE_HDLR_s *StillEncPipe = NULL;

#define STILL_BISFIFO_SIZE 32*1024*1024
UINT8 *StillBitsBuf;
void *StillBitsBufRaw;
#define STILL_DESC_SIZE 32*128
UINT8 *StillDescBuf;
void *StillDescBufRaw;

static int ApplibStillEncLiveviewInitFlag = -1;

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
        EncInitCfg.MainTaskInfo.Priority = 42;
        EncInitCfg.RawCapTaskInfo.Priority = 42;
        EncInitCfg.RawEncTaskInfo.Priority = 42;
        ReturnValue = AmpStillEnc_Init(&EncInitCfg);
        if (ReturnValue != OK) {
            AmbaPrintColor(RED,"[Applib - StillEnc] <Init> failure!!");
        }
    }
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
        {
          //AMP_ENC_RAW_INFO_s *ptr = info;
          //AMP_ENC_RAW_INFO_s inf = *ptr;   // must copy to local. caller won't keep it after function exit
          //AmbaPrint("EncCB: AMP_ENC_EVENT_LIVEVIEW_RAW_READY addr: %X p:%d %dx%d", inf.RawAddr, inf.RawPitch, inf.RawWidth, inf.RawHeight);
        }
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
    case AMP_ENC_EVENT_QUICKVIEW_DCHAN_YUV_READY:
        DBGMSG("[Applib - StillEnc] <StillEncCallback> AMP_ENC_EVENT_QUICKVIEW_DCHAN_YUV_READY");
        break;
    case AMP_ENC_EVENT_QUICKVIEW_FCHAN_YUV_READY:
        DBGMSG("[Applib - StillEnc] <StillEncCallback> AMP_ENC_EVENT_QUICKVIEW_FCHAN_YUV_READY");
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
        break;
    case AMP_VIN_EVENT_CHANGED_POST:
        DBGMSG("[Applib - StillEnc] <VinSwitchCallback> AMP_VIN_EVENT_CHANGED_POST info: %X", info);

        //inform 3A LiveView type
        AppLibImage_Set3APhotoMode(1);
        AppLibImage_VinChangedPostCallbackFunc(hdlr, event, info);
        {
            int ReturnValue = 0;
            AMP_DISP_WINDOW_CFG_s Window;
            AMP_DISP_INFO_s DispDev = {0};
            APPLIB_VOUT_PREVIEW_PARAM_s PreviewParam={0};
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
    default:
        AmbaPrint("[Applib - StillEnc] <VinSwitchCallback> Unknown event %X info: %x", event, info);
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
    extern AMP_MULTI_CHAN_MAIN_WINDOW_CFG_s MultiChanMainWindow[5];
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
            VinCfg.HwCaptureWindow.Width = MultiChanMainWindow[0].MainCapWin.Width;
            VinCfg.HwCaptureWindow.Height = MultiChanMainWindow[0].MainCapWin.Height;
            VinCfg.HwCaptureWindow.X = MultiChanMainWindow[0].MainCapWin.X;
            VinCfg.HwCaptureWindow.Y = MultiChanMainWindow[0].MainCapWin.Y;
            Layout.Width = MultiChanMainWindow[0].MainWidth;
            Layout.Height = MultiChanMainWindow[0].MainHeight;

#if 0
        VinCfg.HwCaptureWindow.Width = StillLiveViewConfigData->LVCaptureWidth;
        VinCfg.HwCaptureWindow.Height = StillLiveViewConfigData->LVCaptureHeight;
        VinCfg.HwCaptureWindow.X = VinInfo.OutputInfo.RecordingPixels.StartX +
            (((VinInfo.OutputInfo.RecordingPixels.Width - VinCfg.HwCaptureWindow.Width)/2)&0xFFF8);
        VinCfg.HwCaptureWindow.Y = (VinInfo.OutputInfo.RecordingPixels.StartY +
            ((VinInfo.OutputInfo.RecordingPixels.Height - VinCfg.HwCaptureWindow.Height)/2)) & 0xFFFE;
        Layout.Width = StillLiveViewConfigData->LVMainWidth;
        Layout.Height = StillLiveViewConfigData->LVMainHeight;
#endif
        Layout.EnableSourceArea = 1;

        Layout.SourceArea = MultiChanMainWindow[0].MainCapWin;



//        Layout.EnableSourceArea = 0; // Get all capture window to main
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
        AMBA_IMG_SCHDLR_CFG_s ISCfg;
        ISCfg.MainViewID = 0;
        ISCfg.Channel = AppEncChannel;
        ISCfg.Vin = AppVinA;
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
    int ReturnValue = 0;
extern AMP_MULTI_CHAN_MAIN_WINDOW_CFG_s MultiChanMainWindow[5];
    memset(&Layout, 0x0, sizeof(AMP_VIN_LAYOUT_CFG_s));
    memset(&VinCfg, 0x0, sizeof(AMP_VIN_HDLR_CFG_s));
    memset(&VinInfo, 0x0, sizeof(AMBA_SENSOR_MODE_INFO_s));
    DBGMSGc2(BLUE,"[Applib - StillEnc] <LiveViewSetup> Start");
    if (ApplibStillEncLiveviewInitFlag < 0) {
        AppLibStillEnc_LiveViewInit();
    }

    StillLiveViewConfigData = (APPLIB_SENSOR_STILLPREV_CONFIG_s *)AppLibSysSensor_GetPhotoLiveviewConfig(AppLibStillEnc_GetPhotoPjpegCapMode(), AppLibStillEnc_GetPhotoPjpegConfigId());
    Mode.Data = AppLibSysSensor_GetPhotoLiveViewModeID(AppLibStillEnc_GetPhotoPjpegCapMode(), AppLibStillEnc_GetPhotoPjpegConfigId());
    AmbaSensor_GetModeInfo(AppEncChannel, Mode, &VinInfo);

    VinCfg.Hdlr = AppVinA;
    VinCfg.Mode = Mode;
    VinCfg.LayoutNumber = 1;
    //VinCfg.HwCaptureWindow.Width = StillLiveViewConfigData->LVCaptureWidth;
    //VinCfg.HwCaptureWindow.Height = StillLiveViewConfigData->LVCaptureHeight;
    //VinCfg.HwCaptureWindow.X = VinInfo.OutputInfo.RecordingPixels.StartX + (((VinInfo.OutputInfo.RecordingPixels.Width - VinCfg.HwCaptureWindow.Width)/2)&0xFFF8);
    //VinCfg.HwCaptureWindow.Y = (VinInfo.OutputInfo.RecordingPixels.StartY + ((VinInfo.OutputInfo.RecordingPixels.Height - VinCfg.HwCaptureWindow.Height)/2)) & 0xFFFE;
    //Layout.Width = StillLiveViewConfigData->LVMainWidth;
    //Layout.Height = StillLiveViewConfigData->LVMainHeight;
    VinCfg.HwCaptureWindow.Width = MultiChanMainWindow[0].MainCapWin.Width;
    VinCfg.HwCaptureWindow.Height = MultiChanMainWindow[0].MainCapWin.Height;
    VinCfg.HwCaptureWindow.X = MultiChanMainWindow[0].MainCapWin.X;
    VinCfg.HwCaptureWindow.Y = MultiChanMainWindow[0].MainCapWin.Y;



    Layout.Width = MultiChanMainWindow[0].MainWidth;
    Layout.Height = MultiChanMainWindow[0].MainHeight;
    Layout.EnableSourceArea = 0; // Get all capture window to main
    Layout.DzoomFactorX = 1<<16;
    Layout.DzoomFactorY = 1<<16;
    Layout.DzoomOffsetX = 0;
    Layout.DzoomOffsetY = 0;
    VinCfg.Layout = &Layout;

    MainCfg.Hdlr = StillEncPri;
    MainCfg.MainLayout.LayerNumber = 1;
    MainCfg.MainLayout.Layer = &NewLayer;
    MainCfg.MainTickPerPicture = 1001;//StillTuningMgt[SensorIdx][encID].TickPerPicture;
    MainCfg.MainTimeScale = 30000;//StillTuningMgt[SensorIdx][encID].TimeScale;

//    MainCfg.MainLayout.Width = StillLiveViewConfigData->LVMainWidth;
//    MainCfg.MainLayout.Height = StillLiveViewConfigData->LVMainHeight;
    MainCfg.MainTickPerPicture = StillLiveViewConfigData->TickPerPicture;
    MainCfg.MainTimeScale = StillLiveViewConfigData->TimeScale;
    NewLayer.EnableSourceArea = 1;
    NewLayer.EnableTargetArea = 1;
    NewLayer.SourceArea = MultiChanMainWindow[0].MainCapWin;
    NewLayer.TargetArea.Width = MultiChanMainWindow[0].MainWidth;
    NewLayer.TargetArea.Height = MultiChanMainWindow[0].MainHeight;

    MainCfg.MainLayout.Width = MultiChanMainWindow[0].MainWidth;
    MainCfg.MainLayout.Height = MultiChanMainWindow[0].MainHeight;

//    NewLayer.EnableSourceArea = 0;
//    NewLayer.EnableTargetArea = 0;

    NewLayer.LayerId = 0;
    NewLayer.SourceType = AMP_ENC_SOURCE_VIN;
    NewLayer.Source = AppVinA;
    NewLayer.SourceLayoutId = 0;

    MainCfg.DspWorkBufAddr = ApplibDspWorkAreaResvStart;
    MainCfg.DspWorkBufSize = ApplibDspWorkAreaResvSize;

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
    ReturnValue = AmpEnc_StartLiveview(StillEncPipe, 0);
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
    AppLibImage_Stop3A(StillEncPri);
    AppLibImage_StopImgSchdlr(0);
    ReturnValue = AmpEnc_StopLiveview(StillEncPipe, AMP_ENC_FUNC_FLAG_WAIT);
    if (ReturnValue != OK) {
        AmbaPrintColor(RED,"[Applib - StillEnc] <LiveViewStop> failure!!");
    } else {
        DBGMSGc2(GREEN,"[Applib - StillEnc] <LiveViewStop> success!!");
    }

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

