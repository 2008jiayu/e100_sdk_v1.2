/**
 * @file src/app/connected/applib/src/recorder/ApplibRecorder_VideoEnc.c
 *
 * Implementation of video config APIs
 *
 * History:
 *    2013/08/16 - [Martin Lai] created file
 *
 *
 * Copyright (c) 2015 Ambarella, Inc.
 *
 * This file and its contents (“Software”) are protected by intellectual property rights
 * including, without limitation, U.S. and/or foreign copyrights.  This Software is also the
 * confidential and proprietary information of Ambarella, Inc. and its licensors.  You may
 * not use, reproduce, disclose, distribute, modify, or otherwise prepare derivative
 * works of this Software or any portion thereof except pursuant to a signed license
 * agreement or nondisclosure agreement with Ambarella, Inc. or its authorized
 * affiliates.  In the absence of such an agreement, you agree to promptly notify and
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
#include <calibration/ApplibCalibMgr.h>
#include "../AppLibTask_Priority.h"

//#include <cfs/AmpCfs.h>
#include <imgproc/AmbaImg_Proc.h>
#include <imgproc/AmbaImg_Impl_Cmd.h>
#include <recorder/StillEnc.h>
#include "ApplibRecorder_StillEncUtility.h"
#include <AmbaUtility.h>
#include <calibration/bpc/ApplibCalibBpc.h>
#include <calibration/vig/ApplibCalibVig.h>
#include "encmonitor/encMonitor.h"
#include "encmonitor/encMonitorService.h"
#include "encmonitor/encMonitorStream.h"
#include <recorder/ApplibRecorder_ExtendEnc.h>
#include "va/ApplibVideoAnal_FrmHdlr.h"
#include <3a/iqparam/ApplibIQParamHandler.h>

//#define DEBUG_APPLIB_VIDEO
#if defined(DEBUG_APPLIB_VIDEO)
#define DBGMSG AmbaPrint
#define DBGMSGc(x) AmbaPrintColor(GREEN,x)
#define DBGMSGc2 AmbaPrintColor
#else
#define DBGMSG(...)
#define DBGMSGc(...)
#define DBGMSGc2(...)
#endif

/**
 * video encode bit stream setting
 */
typedef struct _APPLIB_VIDEOENC_STREAM_SETTING_s_ {
    UINT16 VinRes;/**< Vin resolution */
    UINT16 EncodeRes;/**< Encode resolution */
    UINT8 Quality;/**< Quality */
    UINT8 RecordMode;/**< Record Mode */
    UINT16 SplitMode;/**< Split Mode */
    APPLIB_VIDEOENC_GOP_s Gop;/**< GOP Setting */
} APPLIB_VIDEOENC_STREAM_SETTING_s;
/*!
* applib video encode stream setting ID enum
*/
typedef enum _APPLIB_VIDEOENC_STREAM_SETTING_ID_e_ {
    PRIMARY_PRESET_1 = 0,
    PRIMARY_PRESET_2,
    PRIMARY_STREAMOUT,
    SECONDARY_PRESET_1,
    SECONDARY_PRESET_2,
    SECONDARY_STREAMOUT,
    THIRD_STREAMOUT,
    VIDEO_STREAM_SETTINGS_NUM
} APPLIB_VIDEOENC_STREAM_SETTING_ID_e;

static APPLIB_VIDEOENC_SETTING_s ApplibVideoEncVideoSetting = {0};
static APPLIB_VIDEOENC_STREAM_SETTING_s ApplibVideoEncVideoStreamSettingTable[VIDEO_STREAM_SETTINGS_NUM] = {
    {SENSOR_VIDEO_RES_TRUE_1080P_HALF, SENSOR_VIDEO_RES_TRUE_1080P_HALF, VIDEO_QUALITY_SFINE, REC_MODE_AV, 0, {GOP_SIMPLE,3,15,60}}, //PRIMARY_PRESET_1
    {SENSOR_VIDEO_RES_TRUE_1080P_HALF, SENSOR_VIDEO_RES_TRUE_1080P_HALF, VIDEO_QUALITY_SFINE, REC_MODE_AV, 0, {GOP_SIMPLE,1,8,32}}, //PRIMARY_PRESET_2
    {SENSOR_VIDEO_RES_WVGA_HALF, SENSOR_VIDEO_RES_WVGA_HALF, VIDEO_QUALITY_SFINE, REC_MODE_VIDEO_ONLY, 0, {GOP_SIMPLE,1,8,8}}, //PRIMARY_STREAMOUT
    {SENSOR_VIDEO_RES_WQVGA_HALF, SENSOR_VIDEO_RES_WQVGA_HALF, VIDEO_QUALITY_SFINE, REC_MODE_AV, 0, {GOP_SIMPLE,3,15,60}}, //SECONDARY_PRESET_1
    {SENSOR_VIDEO_RES_WQVGA_HALF, SENSOR_VIDEO_RES_WQVGA_HALF, VIDEO_QUALITY_SFINE, REC_MODE_AV, 0, {GOP_SIMPLE,1,8,32}}, //SECONDARY_PRESET_2
    {SENSOR_VIDEO_RES_WQVGA_HALF, SENSOR_VIDEO_RES_WQVGA_HALF, VIDEO_QUALITY_SFINE, REC_MODE_VIDEO_ONLY, 0, {GOP_SIMPLE,1,8,16}}, //SECONDARY_STREAMOUT
    {SENSOR_VIDEO_RES_WQVGA_HALF, SENSOR_VIDEO_RES_WQVGA_HALF, VIDEO_QUALITY_SFINE, REC_MODE_VIDEO_ONLY, 0, {GOP_SIMPLE,1,8,16}}, //THIRD_STREAMOUT
};



#define VIDENC_BITSFIFO_SIZE 32*1024*1024
UINT8 *H264EncBitsBuf = NULL;
void *H264EncBitsBufRaw = NULL;
static UINT8 *MjpgEncBitsBuf = NULL;
#define VIDENC_DESC_SIZE 40*3000
UINT8 *H264DescBuf = NULL;
void *H264DescBufRaw = NULL;
UINT8 *StillEncWorkBuf = NULL;
void *StillEncWorkBufRaw = NULL;
static UINT8 *MjpgDescBuf = NULL;
#define ENCMEMMSIZE 18000
static UINT8 *AppLibVideoEncMem;
static void *AppLibVideoEncMemBufRaw;

#define SEC_STREAM_WIDTH   1280
#define SEC_STREAM_HEIGHT  720
#define SEC_STREAM_TIMESCALE 30000
#define SEC_STREAM_TICK 1001
#define SEC_STREAM_GOP_M 1
#define SEC_STREAM_GOP_N 8
#define SEC_STREAM_GOP_IDR 8
#define SEC_STREAM_BITRATE 4E6

static UINT32 SecStreamWidth = SEC_STREAM_WIDTH;
static UINT32 SecStreamHeight = SEC_STREAM_HEIGHT;
static UINT32 SecStreamTimeScale = SEC_STREAM_TIMESCALE;
static UINT32 SecStreamTick = SEC_STREAM_TICK;
static UINT32 SecStreamGopM = SEC_STREAM_GOP_M;
static UINT32 SecStreamGopN = SEC_STREAM_GOP_N;
static UINT32 SecStreamGopIDR = SEC_STREAM_GOP_IDR;
static UINT32 SecStreamBitRate = SEC_STREAM_BITRATE;

AMP_AVENC_HDLR_s *VideoEncPri = NULL; // Primary stream codec
AMP_AVENC_HDLR_s *VideoEncSec = NULL; // Secondary stream codec
AMP_STLENC_HDLR_s *StillEncPriPIV = NULL; // Primary stream codec
static AMP_ENC_PIPE_HDLR_s *VideoEncPipe = NULL;
static AMP_ENC_PIPE_HDLR_s *StillEncPipePIV = NULL;
static UINT32 InitZoomX = 1<<16;
static UINT32 InitZoomY = 1<<16;
static UINT32 G_iso = 1;

static UINT8 *PIVCapScriptAddrBufRaw = NULL;
static UINT8 *PIVCapScriptAddr = NULL;
static UINT8 *PIVCapRawBuffAddr = NULL;
static UINT8 *PIVCapYuvBuffAddr = NULL;
static UINT8 *PIVCapScrnBuffAddr = NULL;
static UINT8 *PIVCapThmBuffAddr = NULL;
static UINT8 *PIVCapQvLCDBuffAddrBufRaw = NULL;
static UINT8 *PIVCapQvLCDBuffAddr = NULL;
static UINT8 *PIVCapQvHDMIBuffAddrBufRaw = NULL;
static UINT8 *PIVCapQvHDMIBuffAddr = NULL;

static UINT8 *EncMonitorCyclicWorkBuf = NULL;  // Encode monitor Cyclic working buffer
static UINT8 *EncMonitorCyclicWorkBufRaw = NULL;  // Encode monitor Cyclic working buffer
static UINT8 *EncMonitorServiceWorkBuf = NULL; // BitRateMonitorControl working buffer
static UINT8 *EncMonitorServiceWorkBufRaw = NULL; // BitRateMonitorControl working buffer
static UINT8 *EncMonitorServiceAqpWorkBuf = NULL; // AQpMonitorControl working buffer
static UINT8 *EncMonitorServiceAqpWorkBufRaw = NULL; // AQpMonitorControl working buffer


static UINT8 *EncMonitorStrmWorkBuf = NULL;    // Encode monitor stream working buffer
static UINT8 *EncMonitorStrmWorkBufRaw = NULL;    // Encode monitor stream working buffer

AMBA_IMG_ENC_MONITOR_BITRATE_HDLR_s *BrcHdlrPri = NULL;  // Pri Stream BitRateMonitorControl instance
AMBA_IMG_ENC_MONITOR_BITRATE_HDLR_s *BrcHdlrSec = NULL;  // Sec Stream BitRateMonitorControl instance
AMBA_IMG_ENC_MONITOR_AQP_HDLR_s     *AqpHdlrPri = NULL;  // Pri Stream AqpMonitorControl instance
AMBA_IMG_ENC_MONITOR_AQP_HDLR_s     *AqpHdlrSec = NULL;  // Sec Stream AqpMonitorControl instance

static AMBA_IMG_ENCODE_MONITOR_STRM_HDLR_s *EncMonitorStrmHdlrPri = NULL;  // Pri Stream in encode monitor instance
static AMBA_IMG_ENCODE_MONITOR_STRM_HDLR_s *EncMonitorStrmHdlrSec = NULL;  // Sec Stream in encode monitor instance

static int ApplibVideoEncLiveviewInitFlag = -1;
static UINT8 ApplibVideoEncSlowShutterEnable = 1;
static int ApplibVideoEncInitFlag = -1;
int ApplibCalibinitFlag = 0;
static int ApplibVideoEncInitIqParamFlag = 0;
static int ApplibVideoEncIsHdrIqParamFlag = 0;


#define APPLIB_RECORDER_MEMMGR  (1)
/**
 *  @brief Initial the video encoder.
 *
 *  Initial the video encoder.
 *
 *  @return >=0 success, <0 failure
 */
int AppLibVideoEnc_Init(void)
{
    int ReturnValue = 0;

    if (ApplibVideoEncInitFlag == 0) {
        DBGMSG("[Applib - VideoEnc] <Init> Video encoder already inital");
        return ReturnValue;
    }
    /* Init VIDEOENC module */
    {
        AMP_VIDEOENC_INIT_CFG_s EncInitCfg;
        memset(&EncInitCfg, 0x0, sizeof(AMP_VIDEOENC_INIT_CFG_s));
        ReturnValue = AmpUtil_GetAlignedPool(APPLIB_G_MMPL, (void **)&AppLibVideoEncMem, &AppLibVideoEncMemBufRaw, ENCMEMMSIZE, 32);
        if (ReturnValue != OK) {
            AmbaPrintColor(RED, "[Applib - VideoEnc] <Init> Out of memory for enc!!");
        }
        DBGMSG("[Applib - VideoEnc] <Init> ReturnValue = %d, MemoryPoolSize = 0x%X MemoryPoolAddr=0x%X,",ReturnValue, ENCMEMMSIZE, AppLibVideoEncMem);
        AppLibVideoEncMem = (UINT8 *)ALIGN_32((UINT32)AppLibVideoEncMem);

        AmpVideoEnc_GetInitDefaultCfg(&EncInitCfg);
        EncInitCfg.MemoryPoolAddr = AppLibVideoEncMem;
        EncInitCfg.MemoryPoolSize = ENCMEMMSIZE;
        EncInitCfg.TaskInfo.StackSize = 8192;
        EncInitCfg.TaskInfo.Priority = APPLIB_VIDEO_ENC_TASK_PRIORITY;
        ReturnValue = AmpVideoEnc_Init(&EncInitCfg);
        if (ReturnValue != OK) {
            AmbaPrintColor(RED,"AmpVideoEnc_Init failure!!");
        }
    }
#if APPLIB_RECORDER_MEMMGR

   AppLibRecorderMemMgr_BufAllocate();
   AppLibRecorderMemMgr_GetBufAddr(&H264EncBitsBuf, &H264DescBuf);
   /* AppLibComSvcMemMgr_FIFOBufAllocate();
    ReturnValue = AmpUtil_GetAlignedPool(APPLIB_FIFO_MMPL, (void **)&H264EncBitsBuf, &H264EncBitsBufRaw, VIDENC_BITSFIFO_SIZE, 32);
    if (ReturnValue != OK) {
        AmbaPrintColor(RED,"[Applib - VideoEnc] <Init> Out of cached memory for bitsFifo!!");
        return ReturnValue;
    }

    ReturnValue = AmpUtil_GetAlignedPool(APPLIB_FIFO_MMPL, (void **)&H264DescBuf, &H264DescBufRaw, VIDENC_DESC_SIZE, 32);
    if (ReturnValue != OK) {
        AmbaPrintColor(RED,"[Applib - VideoEnc] <Init> Out of cached memory for bitsFifo!!");
    }

    AmbaPrint("[Applib - VideoEnc] <Init>  H264EncBitsBuf=  0x%x", H264EncBitsBuf);
    AmbaPrint("[Applib - VideoEnc] <Init>  H264DescBuf=  0x%x", H264DescBuf);*/
#else
    /* Allocate bitstream buffer */
    {
        extern UINT8 *ApplibDspWorkAreaResvLimit;
        ReturnValue = AmpUtil_GetAlignedPool(APPLIB_G_MMPL, (void **)&H264EncBitsBuf, &H264EncBitsBufRaw, VIDENC_BITSFIFO_SIZE, 32);
        if (ReturnValue != OK) {
            AmbaPrintColor(RED,"[Applib - VideoEnc] <Init> Out of cached memory for bitsFifo!!");
            return ReturnValue;
        }
        H264EncBitsBuf = (UINT8 *)ALIGN_32((UINT32)H264EncBitsBuf);

        ReturnValue = AmpUtil_GetAlignedPool(APPLIB_G_MMPL, (void **)&H264DescBuf, &H264DescBufRaw, VIDENC_DESC_SIZE, 32);
        if (ReturnValue != OK) {
            AmbaPrintColor(RED,"[Applib - VideoEnc] <Init> Out of cached memory for bitsFifo!!");
        }
        H264DescBuf = (UINT8 *)ALIGN_32((UINT32)H264DescBuf);

        // This is an example how to use DSP working memory when APP knows these memory area is not used.
        // We steal 25MB here
        // MjpgDescBuf = ApplibDspWorkAreaResvLimit + 1 - 1*1024*1024;
        // MjpgEncBitsBuf = MjpgDescBuf - VIDENC_BITSFIFO_SIZE;

    }
#endif


    /**Initialize EncMonitor Cyclic module*/
    {
        UINT32 MemSize = 0;
        AMBA_IMG_ENC_MONITOR_MEMORY_s MonitorCyclicQueryCfg;
        AMBA_IMG_ENC_MONITOR_INIT_CFG_s MonitorCyclicCfg;
        memset(&MonitorCyclicQueryCfg, 0x0, sizeof(AMBA_IMG_ENC_MONITOR_MEMORY_s));
        memset(&MonitorCyclicCfg, 0x0, sizeof(AMBA_IMG_ENC_MONITOR_INIT_CFG_s));

        AmbaEncMonitor_GetInitDefaultCfg(&MonitorCyclicCfg);
        MonitorCyclicQueryCfg.MaxTimerMonitorNumber = MonitorCyclicCfg.MaxTimerMonitorNumber;
        MonitorCyclicQueryCfg.MaxVdspMonitorNumber = MonitorCyclicCfg.MaxVdspMonitorNumber;
        MonitorCyclicQueryCfg.TimerMonitorTaskStackSize = MonitorCyclicCfg.TimerMonitorTaskStackSize;
        MonitorCyclicQueryCfg.VdspMonitorTaskStackSize = MonitorCyclicCfg.VdspMonitorTaskStackSize;
        AmbaEncMonitor_QueryMemsize(&MonitorCyclicQueryCfg, &MemSize);
        ReturnValue = AmpUtil_GetAlignedPool(APPLIB_G_MMPL, (void **)&EncMonitorCyclicWorkBuf, (void **)&EncMonitorCyclicWorkBufRaw , MemSize , 32);
        if (ReturnValue != OK) {
            AmbaPrint("[Applib - VideoEnc] <Init> Out of memory for encMonitorCyclic!!");
        }
        MonitorCyclicCfg.MemoryPoolSize = MemSize;
        MonitorCyclicCfg.MemoryPoolAddr = EncMonitorCyclicWorkBuf;
        MonitorCyclicCfg.VdspMonitorTaskPriority = APPLIB_ENC_MONITOR_VDSP_TASK_PRIORITY;
        MonitorCyclicCfg.TimerMonitorTaskPriority = APPLIB_ENC_MONITOR_TIMER_TASK_PRIORITY;
        //MonitorCyclicCfg.TimerMonitorTaskPriority = ;
        ReturnValue = AmbaEncMonitor_Init(&MonitorCyclicCfg);
        if (ReturnValue != AMP_OK) {
            AmbaPrint("[Applib - VideoEnc] <Init> AmbaEncMonitor_Init Fail!");
        }
    }

    /** Initialize EncMonitor Stream module*/
    {
        UINT32 MemSize = 0;
        AMBA_IMG_ENC_MONITOR_STRM_MEMORY_s MonitorStrmQueryCfg;
        AMBA_IMG_ENC_MONITOR_STRM_INIT_CFG_s MonitorStrmCfg;
        memset(&MonitorStrmQueryCfg, 0x0, sizeof(AMBA_IMG_ENC_MONITOR_STRM_MEMORY_s));
        memset(&MonitorStrmCfg, 0x0, sizeof(AMBA_IMG_ENC_MONITOR_STRM_INIT_CFG_s));

        MonitorStrmCfg.MaxStreamNumber = MonitorStrmQueryCfg.MaxStreamNumber = 2; // dual stream
        AmbaEncMonitorStream_QueryMemsize(&MonitorStrmQueryCfg, &MemSize);

        ReturnValue = AmpUtil_GetAlignedPool(APPLIB_G_MMPL, (void **)&EncMonitorStrmWorkBuf, (void **)&EncMonitorStrmWorkBufRaw, MemSize, 32);
        if (ReturnValue != OK) {
            AmbaPrint("[Applib - VideoEnc] <Init> Out of memory for encMonitorStream!!");
        }
        MonitorStrmCfg.MemoryPoolSize = MemSize;
        MonitorStrmCfg.MemoryPoolAddr = EncMonitorStrmWorkBuf;
        ReturnValue = AmbaEncMonitorStream_Init(&MonitorStrmCfg);
        if (ReturnValue != AMP_OK) {
            AmbaPrint("[Applib - VideoEnc] <Init> AmbaEncMonitorStream_Init Fail!");
        }
    }
    /** Initialize EncMonitor BitRate Service module*/
    {
        UINT32 MemSize = 0;
        AMBA_IMG_ENC_MONITOR_BRC_MEMORY_s MonitorBRateQueryCfg;
        AMBA_IMG_ENC_MONITOR_BITRATE_INIT_CFG_s MonitorBRateCfg;
        memset(&MonitorBRateQueryCfg, 0x0, sizeof(AMBA_IMG_ENC_MONITOR_BRC_MEMORY_s));
        memset(&MonitorBRateCfg, 0x0, sizeof(AMBA_IMG_ENC_MONITOR_BITRATE_INIT_CFG_s));

        MonitorBRateCfg.MaxStreamNumber = MonitorBRateQueryCfg.MaxStreamNumber = 2; // dual stream
        AmbaEncMonitorBRC_QueryMemsize(&MonitorBRateQueryCfg, &MemSize);

        ReturnValue = AmpUtil_GetAlignedPool(APPLIB_G_MMPL, (void **)&EncMonitorServiceWorkBuf, (void **)&EncMonitorServiceWorkBufRaw, MemSize, 32);
        if (ReturnValue != OK) {
            AmbaPrint("[Applib - VideoEnc] <Init> Out of memory for encMonitorService!!");
        }

        MonitorBRateCfg.MemoryPoolSize = MemSize;
        MonitorBRateCfg.MemoryPoolAddr = EncMonitorServiceWorkBuf;
        ReturnValue = AmbaEncMonitorBRC_init(&MonitorBRateCfg);
        if (ReturnValue != AMP_OK) {
            AmbaPrint("[Applib - VideoEnc] <Init> AmbaEncMonitorBRC_init Fail!");
        }
    }
    // Initialize EncMonitor AQP Service module
    {
        UINT32 MemSize = 0;
        AMBA_IMG_ENC_MONITOR_AQP_MEMORY_s MonitorAqpQueryCfg;
        AMBA_IMG_ENC_MONITOR_AQP_INIT_CFG_s MonitorAqpCfg;
        memset(&MonitorAqpQueryCfg, 0x0, sizeof(AMBA_IMG_ENC_MONITOR_AQP_MEMORY_s));
        memset(&MonitorAqpCfg, 0x0, sizeof(AMBA_IMG_ENC_MONITOR_AQP_INIT_CFG_s));

        MonitorAqpCfg.MaxStreamNumber = MonitorAqpQueryCfg.MaxStreamNumber = 2; // dual stream
        AmbaEncMonitorAQP_QueryMemsize(&MonitorAqpQueryCfg, &MemSize);

        ReturnValue = AmpUtil_GetAlignedPool(APPLIB_G_MMPL, (void **)&EncMonitorServiceAqpWorkBuf, (void **)&EncMonitorServiceAqpWorkBufRaw, MemSize, 32);
        if (ReturnValue != OK) {
            AmbaPrintColor(RED,"[Applib - VideoEnc] <Init> Out of memory for encMonitorAqpService!!MemSize=%ld",MemSize);
        }

        MonitorAqpCfg.MemoryPoolSize = MemSize;
        MonitorAqpCfg.MemoryPoolAddr = EncMonitorServiceAqpWorkBuf;
        ReturnValue= AmbaEncMonitorAQP_init(&MonitorAqpCfg);
        if (ReturnValue != AMP_OK) {
            AmbaPrintColor(RED,"[Applib - VideoEnc] <Init> AmbaEncMonitorAQP_init Fail!");
        }
    }


    ApplibVideoEncInitFlag = 0;
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
static int AppLibVideoEnc_PipeCallback(void *hdlr,UINT32 event, void *info)
{
    switch (event) {
    case AMP_ENC_EVENT_STATE_CHANGED:
        {
            AMP_ENC_STATE_CHANGED_INFO_s *Inf = info;

            AmbaPrint("[Applib - VideoEnc] <PipeCallback> Pipe[%X] AMP_ENC_EVENT_STATE_CHANGED newState %X", hdlr, Inf->newState);
            switch (Inf->newState) {
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
                AmbaPrint("[Applib - VideoEnc] <PipeCallback> Unknown event %X info: %x", event, info);
                break;
            }
        }
        break;
    }
    return 0;

}

//static int yuv_counter = 0;

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
static int AppLibVideoEnc_VideoEncCallback(void *hdlr,UINT32 event, void *info)
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
        DBGMSG("[Applib - VideoEnc] <VideoEncCallback> AMP_ENC_EVENT_LIVEVIEW_DCHAN_YUV_READY !");
        AppLibVideoAnal_FrmHdlr_NewFrame(AMP_ENC_EVENT_LIVEVIEW_DCHAN_YUV_READY, (AMP_ENC_YUV_INFO_s* )info);
        {
            //AMP_ENC_YUV_INFO_s *ptr = info;
            //AMP_ENC_YUV_INFO_s inf = *ptr;   // must copy to local. caller won't keep it after function exit
#if 0
            extern int mode_swiych;
            if (mode_swiych) {
                yuv_counter++;
                if (yuv_counter == 200) {
                    #define Y_FN  L"C:\\PREV.Y"
                    #define UV_FN  L"C:\\PREV.UV"
                    AMP_CFS_FILE_s *pFile;
                    AMP_CFS_FILE_PARAM_s cfsParam;


                    AmpCFS_GetFileParam(&cfsParam);
                    cfsParam.Mode = AMP_CFS_FILE_MODE_WRITE_ONLY;
                    w_strcpy(cfsParam.Filename, Y_FN);
                    pFile = AmpCFS_fopen(&cfsParam);
                    AmpCFS_fwrite(inf.yAddr, 1,inf.ySize, pFile);
                    AmpCFS_fclose(pFile);
                    AmpCFS_GetFileParam(&cfsParam);
                    cfsParam.Mode = AMP_CFS_FILE_MODE_WRITE_ONLY;
                    w_strcpy(cfsParam.Filename, UV_FN);
                    pFile = AmpCFS_fopen(&cfsParam);
                    if (inf.colorFmt == AMP_YUV_422) {
                        AmbaPrintColor(GREEN,"~~~~~~~~~~~~~~~~~~~~AMP_YUV_422 @@@@@@@");
                        AmpCFS_fwrite((inf.yAddr+inf.ySize), 1,(inf.pitch * inf.height), pFile);
                    } else {
                        AmbaPrintColor(GREEN,"~~~~~~~~~~~~~~~~~~~~AMP_YUV_420 @@@@@@@");
                        AmpCFS_fwrite((inf.yAddr+inf.ySize), 1,(inf.pitch * inf.height)/2, pFile);
                    }
                    AmpCFS_fclose(pFile);
if (0) {
    char yFileName[20] ={'c',':','\\','1','.','y'};
    char uvFileName[20] ={'c',':','\\','1','.','u','v'};
    char tmp[10] = {0};
    WCHAR Fn[32];
    AMBA_FS_FILE *Fid;
    AmbaUtility_Ascii2Unicode(FileName,Fn);
    Fid = AmbaFS_fopen((char*)Fn,"w");
    if (Fid <= NULL) {
        AmbaPrint("fopen %s fail.",FileName);
        return -1;
    }
    if (inf.colorFmt == AMP_YUV_422) {
        AmbaPrintColor(GREEN,"~~~~~~~~~~~~~~~~~~~~AMP_YUV_422 @@@@@@@");
        AmbaFS_fwrite(CalObj->DramShadow,CalObj->Size, 1, Fid);
        AmpCFS_fwrite(inf.yAddr, 1,(inf.pitch * inf.height), pFile);
    } else {
        AmbaPrintColor(GREEN,"~~~~~~~~~~~~~~~~~~~~AMP_YUV_420 @@@@@@@");
        AmbaFS_fwrite(CalObj->DramShadow,CalObj->Size, 1, Fid);
        AmpCFS_fwrite((inf.yAddr+inf.ySize), 1,(inf.pitch * inf.height)/2, pFile);
    }
    AmbaFS_fclose(Fid);
                    }
                    AmbaPrint("inf.pitch = %d", inf.pitch);
                    AmbaPrint("inf.width = %d", inf.width);
                    AmbaPrint("inf.height = %d", inf.height);
                    AmbaPrint("inf.ySize = %d", inf.ySize);

                }
            }
#endif
        //AmbaPrint("[Applib - VideoEnc] <VideoEncCallback> AMP_VIDEOENC_MSG_LIVEVIEW_DCHAN_YUV_READY addr: %X p:%d %dx%d", inf.yAddr, inf.pitch, inf.Width, inf.Height);
        }
        break;
    case AMP_ENC_EVENT_LIVEVIEW_FCHAN_YUV_READY:
        {
        //   AMP_ENC_YUV_INFO_s *ptr = info;
        //   AMP_ENC_YUV_INFO_s inf = *ptr;   // must copy to local. caller won't keep it after function exit
        //   AmbaPrint("Applib - VideoEnc]: <VideoEncCallback> AMP_VIDEOENC_MSG_LIVEVIEW_FCHAN_YUV_READY info: %X", info);
        }
        break;
    case AMP_ENC_EVENT_VCAP_2ND_YUV_READY:
        DBGMSG("[Applib - VideoEnc] <VideoEncCallback> AMP_ENC_EVENT_VCAP_2ND_YUV_READY !");
        AppLibVideoAnal_FrmHdlr_NewFrame(AMP_ENC_EVENT_VCAP_2ND_YUV_READY, (AMP_ENC_YUV_INFO_s* )info);
        break;
    case AMP_ENC_EVENT_DATA_OVER_RUNOUT_THRESHOLD:
        AmbaPrint("[Applib - VideoEnc] <VideoEncCallback> AMP_ENC_EVENT_DATA_OVER_RUNOUT_THRESHOLD !");
        AppLibComSvcHcmgr_SendMsgNoWait(HMSG_MEMORY_FIFO_BUFFER_RUNOUT, 0, 0);
        break;
    case AMP_ENC_EVENT_DATA_OVERRUN:
        DBGMSG("[Applib - VideoEnc] <VideoEncCallback> AMP_ENC_EVENT_DATA_OVERRUN !");
        break;
    case AMP_ENC_EVENT_QUICKVIEW_DCHAN_YUV_READY:
        DBGMSG("[Applib - VideoEnc] <VideoEncCallback> AMP_ENC_EVENT_QUICKVIEW_DCHAN_YUV_READY");
        break;
    case AMP_ENC_EVENT_QUICKVIEW_FCHAN_YUV_READY:
        DBGMSG("[Applib - VideoEnc] <VideoEncCallback> AMP_ENC_EVENT_QUICKVIEW_FCHAN_YUV_READY");
        break;
    case AMP_ENC_EVENT_VCAP_YUV_READY:
        DBGMSG("[Applib - VideoEnc] <VideoEncCallback> AMP_ENC_EVENT_VCAP_YUV_READY");
        break;
    case AMP_ENC_EVENT_VCAP_ME1_Y_READY:
            DBGMSG("[Applib - VideoEnc] <VideoEncCallback>AMP_ENC_EVENT_VCAP_ME1_Y_READY");
            break;
    case AMP_ENC_EVENT_HYBRID_YUV_READY:
            DBGMSG("[Applib - VideoEnc] <VideoEncCallback>AMP_ENC_EVENT_HYBRID_YUV_READY ");
            break;
    case AMP_ENC_EVENT_DESC_OVERRUN:      /**< buffer data overrun */
        AmbaPrint("[Applib - VideoEnc] <VideoEncCallback> AMP_ENC_EVENT_DESC_OVERRUN");
        break;
    case AMP_ENC_EVENT_RAW_CAPTURE_DONE:
        AmbaPrint("[Applib - StillEnc] <StillEncCallback> AMP_ENC_EVENT_RAW_CAPTURE_DONE");
        break;
    case AMP_ENC_EVENT_BG_PROCESS_DONE:
        AppLibComSvcHcmgr_SendMsgNoWait(HMSG_RECORDER_STATE_PHOTO_BGPROC_COMPLETE, 0, 0);
        AmbaPrint("[Applib - StillEnc] <StillEncCallback> AMP_ENC_EVENT_BG_PROCESS_DONE");
        break;
    case AMP_ENC_EVENT_DATA_FULLNESS_NOTIFY:
        DBGMSG("[Applib - VideoEnc] <VideoEncCallback>AMP_ENC_EVENT_DATA_FULLNESS_NOTIFY  %d %",(UINT32)info);
        break;
    default:
        AmbaPrint("[Applib - VideoEnc] <VideoEncCallback> Unknown event %X info: %x", event, info);
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
static int AppLibVideoEnc_VinEventCallback(void *hdlr,UINT32 event, void *info)
{
    switch (event) {
    case AMP_VIN_EVENT_FRAME_READY:
        //AmbaPrint("[Applib - VideoEnc] <VinEventCallback> AMP_VIN_EVENT_FRAME_READY info: %X", info);
        {
            //static int count = 0;
            //count++;
            //if (count % 30 == 0) {
            //  AmbaPrint("[Applib - VideoEnc] <VinEventCallback> AMP_VIN_EVENT_FRAME_READY info: %X", info);
            //}
        }
        break;
    case AMP_VIN_EVENT_FRAME_DROPPED:
        DBGMSG("[Applib - VideoEnc] <VinEventCallback> AMP_VIN_EVENT_FRAME_DROPPED info: %X", info);
        break;
    default:
        AmbaPrint("[Applib - VideoEnc] <VinEventCallback> Unknown %X info: %x", event, info);
        break;
    }
    return 0;
}
static UINT32 AppLibVideoEnc_FrameRateIntConvert(UINT32 OriFrameRate)
{
    UINT32 FrameRateInt = OriFrameRate;

    if (OriFrameRate==29 || OriFrameRate==59 || OriFrameRate==119 || OriFrameRate==239) {
        FrameRateInt++;
    }

    return FrameRateInt;
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
static int AppLibVideoEnc_VinSwitchCallback(void *hdlr, UINT32 event, void *info)
{
    AMBA_DSP_IMG_MODE_CFG_s ImgMode;
    switch (event) {
    case AMP_VIN_EVENT_INVALID:
        DBGMSG("[Applib - VideoEnc] <VinSwitchCallback> AMP_VIN_EVENT_INVALID info: %X", info);
        AppLibImage_VinInvalidCallbackFunc(hdlr, event, info);
        break;
    case AMP_VIN_EVENT_VALID:
        DBGMSG("[Applib - VideoEnc] <VinSwitchCallback> AMP_VIN_EVENT_VALID info: %X", info);
        AppLibImage_VinValidCallbackFunc(hdlr, event, info);
        break;
    case AMP_VIN_EVENT_CHANGED_PRIOR:
        DBGMSG("[Applib - VideoEnc] <VinSwitchCallback> AMP_VIN_EVENT_CHANGED_PRIOR info: %X", info);
        AppLibImage_VinChangedPriorCallbackFunc(hdlr, event, info);

        {
            int ReturnValue = 0;
            AMP_DISP_WINDOW_CFG_s Window;
            AMP_DISP_INFO_s DispDev = {0};
            APPLIB_VOUT_PREVIEW_PARAM_s PreviewParam={0};
            APPLIB_SENSOR_VIDEO_ENC_CONFIG_s *VideoEncConfigData = NULL;
            memset(&Window, 0x0, sizeof(AMP_DISP_WINDOW_CFG_s));
            VideoEncConfigData = AppLibSysSensor_GetVideoConfig(AppLibVideoEnc_GetSensorVideoRes());
            PreviewParam.AspectRatio = VideoEncConfigData->VAR;

            /* vout configure is moved from event 'AMP_VIN_EVENT_CHANGED_POST' to here.
                      Because A12 MW need to calculate HAL freq before vin_changed_post. */

            /* FCHAN Window*/
            ReturnValue = AppLibDisp_GetDeviceInfo(DISP_CH_FCHAN, &DispDev);
            if ((ReturnValue < 0) || (DispDev.DeviceInfo.Enable == 0)) {
                DBGMSGc2(RED,"[Applib - VideoEnc] FChan Disable. Disable the fchan Window");
                AppLibDisp_DeactivateWindow(DISP_CH_FCHAN, AppLibDisp_GetWindowId(DISP_CH_FCHAN, 0));
            } else {
                PreviewParam.ChanID = DISP_CH_FCHAN;
                AppLibDisp_CalcPreviewWindowSize(&PreviewParam);
                AppLibDisp_GetWindowConfig(DISP_CH_FCHAN, AppLibDisp_GetWindowId(DISP_CH_FCHAN, 0), &Window);
                Window.Source = AMP_DISP_ENC;
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

            /** DCHAN Window*/
            ReturnValue = AppLibDisp_GetDeviceInfo(DISP_CH_DCHAN, &DispDev);
            if ((ReturnValue < 0) || (DispDev.DeviceInfo.Enable == 0)) {
                DBGMSGc2(RED,"[Applib - VideoEnc] DChan Disable. Disable the Dchan Window");
                AppLibDisp_DeactivateWindow(DISP_CH_DCHAN, AppLibDisp_GetWindowId(DISP_CH_DCHAN, 0));
            } else {
                PreviewParam.ChanID = DISP_CH_DCHAN;
                AppLibDisp_CalcPreviewWindowSize(&PreviewParam);
                AppLibDisp_GetWindowConfig(DISP_CH_DCHAN, AppLibDisp_GetWindowId(DISP_CH_DCHAN, 0), &Window);
                Window.Source = AMP_DISP_ENC;
                Window.CropArea.Width = 0;
                Window.CropArea.Height = 0;
                Window.CropArea.X = 0;
                Window.CropArea.Y = 0;
                Window.TargetAreaOnPlane.Width = PreviewParam.Preview.Width;
                Window.TargetAreaOnPlane.Height = PreviewParam.Preview.Height;
                Window.TargetAreaOnPlane.X = PreviewParam.Preview.X;
                Window.TargetAreaOnPlane.Y = PreviewParam.Preview.Y;
#ifdef CONFIG_APP_ARD
                if(AppLibDisp_GetRotate(DISP_CH_DCHAN) == AMP_ROTATE_90){
                    Window.SourceDesc.Enc.Rotate=AMP_ROTATE_90;
                }else{
                    Window.SourceDesc.Enc.Rotate=AMP_ROTATE_0;
                }
#endif
                AppLibDisp_SetWindowConfig(DISP_CH_DCHAN, AppLibDisp_GetWindowId(DISP_CH_DCHAN, 0), &Window);
                AppLibDisp_ActivateWindow(DISP_CH_DCHAN, AppLibDisp_GetWindowId(DISP_CH_DCHAN, 0));
            }
            AppLibDisp_FlushWindow(DISP_CH_FCHAN | DISP_CH_DCHAN);
        }
        break;
    case AMP_VIN_EVENT_CHANGED_POST:
        DBGMSG("[Applib - VideoEnc] <VinSwitchCallback> AMP_VIN_EVENT_CHANGED_POST info: %X", info);


        { // Load IQ params
            UINT8 IsSensorHdrMode, HdrEnable;
            AMBA_SENSOR_STATUS_INFO_s SensorStatus = {0};
            AMBA_IMG_SCHDLR_UPDATE_CFG_s ISCfg;
            LIVEVIEW_INFO_s LiveViewInfo = {0};
            APPLIB_SENSOR_VIDEO_ENC_CONFIG_s *VideoEncConfigData = AppLibSysSensor_GetVideoConfig(AppLibVideoEnc_GetSensorVideoRes());
            AMBA_SENSOR_STATUS_INFO_s SsrStatus = {0};
            AMBA_SENSOR_INPUT_INFO_s *InputInfo = &SsrStatus.ModeInfo.InputInfo;
            UINT32 FrameRate = 0, FrameRatex1000 = 0, IQChannelCount;
            IQChannelCount =  AppLibSysSensor_GetIQChannelCount();

        AmbaImg_Proc_Cmd(MW_IP_GET_VIDEO_HDR_ENABLE, 0, (UINT32)&HdrEnable, 0);

        /**Enable HDR Hybrid liso mode*/
             if (AppLibVideoEnc_GetSensorVideoRes() == SENSOR_VIDEO_RES_TRUE_HDR_1080P_HALF ||
                AppLibVideoEnc_GetSensorVideoRes() == SENSOR_VIDEO_RES_WQHD_HALF_HDR) {
                if (HdrEnable == 0) {
                    /**previous is not HDR mode set ApplibVideoEncInitIqParamFlag = 0 to reload hdr iq parameter*/
                    ApplibVideoEncInitIqParamFlag = 0;
                }
                HdrEnable = 1;
             } else {
                HdrEnable = 0;
             }

            // Inform 3A LV sensor mode is Hdr or not
            AmbaSensor_GetStatus(AppEncChannel, &SensorStatus);
            IsSensorHdrMode = (SensorStatus.ModeInfo.HdrInfo.HdrType == AMBA_SENSOR_HDR_TYPE_MULTI_SLICE)? 1: 0;

            AmbaImg_Proc_Cmd(MW_IP_SET_VIDEO_HDR_ENABLE, 0/*ChNo*/, (UINT32)IsSensorHdrMode, 0);

            if (ApplibVideoEncInitIqParamFlag == 0 || IsSensorHdrMode != ApplibVideoEncIsHdrIqParamFlag) {

                AppLibIQ_ParamInit(IQChannelCount);

                ApplibVideoEncInitIqParamFlag = 1;
                ApplibVideoEncIsHdrIqParamFlag = IsSensorHdrMode;
            }

            if (HdrEnable) {
                ISCfg.VideoProcMode = 1;
                if (IsSensorHdrMode) {
                ISCfg.VideoProcMode |= 0x10;
                }
            } else {
                ISCfg.VideoProcMode = 0;
            }


            /* Keep the report rate lower than 60fps. */
            if ((VideoEncConfigData->EncNumerator/VideoEncConfigData->EncDenominator) >= 200) {
                ISCfg.AAAStatSampleRate= 4;
            } else if ((VideoEncConfigData->EncNumerator/VideoEncConfigData->EncDenominator) >= 100) {
                ISCfg.AAAStatSampleRate  = 2;
            } else {
                ISCfg.AAAStatSampleRate  = 1;
            }
            AppLibImage_UpdateImgSchdlr(&ISCfg, 0);

            if (HdrEnable) {
                /**HDR use hybrid mode*/
                AmbaImg_Proc_Cmd(MW_IP_SET_PIPE_MODE, 0/*ChNo*/, (UINT32)IP_HYBRID_MODE, 0);
            } else {
                /**App default is experss_mode*/
                AmbaImg_Proc_Cmd(MW_IP_SET_PIPE_MODE, 0/*ChNo*/, (UINT32)IP_EXPERSS_MODE, 0);
            }
            /**App default is liso_mode*/
            AmbaImg_Proc_Cmd(MW_IP_SET_VIDEO_ALGO_MODE, 0/*ChNo*/, (UINT32)IP_MODE_LISO_VIDEO, 0);

            //inform 3A LiveView type
            AppLibImage_Set3APhotoMode(0);
            //inform 3A LiveView info

            if (HdrEnable) {
                /**HDR use hybrid mode*/
                LiveViewInfo.OverSamplingEnable = 1; //HybridMode use OverSampling
            } else {
                LiveViewInfo.OverSamplingEnable = 0;
            }
            LiveViewInfo.MainW = VideoEncConfigData->EncodeWidth;
            LiveViewInfo.MainH = VideoEncConfigData->EncodeHeight;
            LiveViewInfo.FrameRateInt = VideoEncConfigData->EncNumerator/VideoEncConfigData->EncDenominator;

            LiveViewInfo.FrameRateInt = AppLibVideoEnc_FrameRateIntConvert(LiveViewInfo.FrameRateInt);
            AmbaSensor_GetStatus(AppEncChannel, &SsrStatus);
            LiveViewInfo.BinningHNum = InputInfo->HSubsample.FactorNum;
            LiveViewInfo.BinningHDen = InputInfo->HSubsample.FactorDen;
            LiveViewInfo.BinningVNum = InputInfo->VSubsample.FactorNum;
            LiveViewInfo.BinningVDen = InputInfo->VSubsample.FactorDen;
            AmbaImg_Proc_Cmd(MW_IP_SET_LIVEVIEW_INFO, 0/*ChIndex*/, (UINT32)&LiveViewInfo, 0);
            //inform 3A FrameRate info

            FrameRate = VideoEncConfigData->EncNumerator/VideoEncConfigData->EncDenominator;
            FrameRatex1000 = VideoEncConfigData->EncNumerator*1000/VideoEncConfigData->EncDenominator;
            FrameRate = AppLibVideoEnc_FrameRateIntConvert(FrameRate);
            AmbaImg_Proc_Cmd(MW_IP_SET_FRAME_RATE, 0/*ChIndex*/, FrameRate, FrameRatex1000);
        }
        AppLibImage_VinChangedPostCallbackFunc(hdlr, event, info);

        // Slow Shutter
        {
            APPLIB_SENSOR_VIDEO_ENC_CONFIG_s *VideoEncConfigData;
            AE_CONTROL_s AeCtrlMode;

            VideoEncConfigData = AppLibSysSensor_GetVideoConfig(AppLibVideoEnc_GetSensorVideoRes());
            AmbaImg_Proc_Cmd(MW_IP_GET_MULTI_AE_CONTROL_CAPABILITY, 0, (UINT32)&AeCtrlMode,0);
            if (ApplibVideoEncSlowShutterEnable) {
                int FrameRate = 0;

                FrameRate = VideoEncConfigData->EncNumerator / VideoEncConfigData->EncDenominator;
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

        /** Load the calibration setting*/
        /** FIXME: Calib Init will cost about 120ms. To avoid vin lost issue,
         *         let it be called before update display window size.
         */
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
        AmbaPrint("[Applib - VideoEnc] <VinSwitchCallback> Unknown event %X info: %x", event, info);
        break;
    }
    return 0;
}

/**
 * Generic VideoEnc ImgSchdlr callback
 *
 * @param [in] hdlr  The event belongs to which vin
 * @param [in] event The event
 * @param [in] info Information the event brings
 *
 * @return 0
 */
static int AppLibVideoEnc_ImgSchdlrCallback(void *hdlr,UINT32 event, void *info)
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
        case AMBA_IMG_SCHDLR_EVENT_MAIN_CFA_HIST_READY:
            break;
        case AMBA_IMG_SCHDLR_EVENT_HDR_CFA_HIST_READY:
            break;
        default:
            AmbaPrint("[Applib - VideoEnc] <ImgSchdlrCallback> Unknown %X info: %x", event, info);
            break;
    }
    return 0;
}

/**
 *  @brief PIV Capture PreProc Callback
 *
 *  Stage1: RAWCAP  -> Nothing to do
 *  Stage2: RAW2YUV  -> setup r2y idsp cfg
 *
 *  @param [in] info preproc information
 *
 *  @return 0 - success, -1 - fail
 */
UINT32 AppLibVideoEnc_PIVCapturePreCB(AMP_STILLENC_PREP_INFO_s *info)
{

    if (info->StageCnt == 3) {
        AmbaPrint("[Applib - VideoEnc] <VideoEncPIV> Pre-CallBack: SerialNum %d", info->JpegSerialNumber);
        /* set still idsp param */
        if (G_iso != 2){ // comes from AmbaSample_AdjPivControl()
            AMBA_DSP_IMG_MODE_CFG_s ImgMode;
            ADJ_STILL_CONTROL_s AdjPivCtrl;
            float BaseStillBlcShtTime = 60.0;
            UINT16 ShutterIndex = 0;
            AMBA_AE_INFO_s VideoAeInfo;
            AMBA_DSP_IMG_WB_GAIN_s VideoWbGain = {WB_UNIT_GAIN,WB_UNIT_GAIN,WB_UNIT_GAIN,WB_UNIT_GAIN,WB_UNIT_GAIN};
            AMBA_SENSOR_STATUS_INFO_s SensorStatus = {0};
            AMBA_DSP_IMG_SENSOR_INFO_s SensorInfo = {0};

            memset(&ImgMode, 0x0, sizeof(AMBA_DSP_IMG_MODE_CFG_s));
            ImgMode.Pipe = AMBA_DSP_IMG_PIPE_STILL;
            ImgMode.AlgoMode = (G_iso==1)? AMBA_DSP_IMG_ALGO_MODE_LISO: AMBA_DSP_IMG_ALGO_MODE_HISO;
            ImgMode.FuncMode = AMBA_DSP_IMG_FUNC_MODE_PIV;
            ImgMode.ContextId = 0; //TBD
            ImgMode.BatchId = AMBA_DSP_STILL_LISO_FILTER;

            /* Run Adj compute */
            memset(&AdjPivCtrl, 0x0, sizeof(ADJ_STILL_CONTROL_s));
            AmbaImg_Proc_Cmd(MW_IP_GET_AE_INFO, 0/*TBD*/, IP_MODE_VIDEO, (UINT32)&VideoAeInfo);
            AmbaImg_Proc_Cmd(MW_IP_GET_PIPE_WB_GAIN, 0/*TBD*/, IP_MODE_VIDEO, (UINT32)&VideoWbGain);

            ShutterIndex = (UINT16)(log2(BaseStillBlcShtTime/VideoAeInfo.ShutterTime) * 128);
            AdjPivCtrl.StillMode = (G_iso==1)? IP_MODE_LISO_STILL: IP_MODE_HISO_STILL;
            AdjPivCtrl.ShIndex = ShutterIndex;//stillAeInfo[aeIndx].ShutterIndex;
            AdjPivCtrl.EvIndex = VideoAeInfo.EvIndex;
            AdjPivCtrl.NfIndex = VideoAeInfo.NfIndex;
            AdjPivCtrl.WbGain = VideoWbGain;
            AdjPivCtrl.DZoomStep = 0;
            AdjPivCtrl.FlashMode = 0;
            AdjPivCtrl.LutNo = 0; //TBD

            AmbaPrintColor(GREEN, "[%s], WbGain, GainR : %5d, GainG : %5d,  GainB : %5d",
                __FUNCTION__, VideoWbGain.GainR, VideoWbGain.GainG, VideoWbGain.GainB);
            AmbaPrintColor(GREEN, "PIV, StillMode : %d, ShIndex : %5d, EvIndex : %5d, NfIndex : %5d",
                    AdjPivCtrl.StillMode, AdjPivCtrl.ShIndex, AdjPivCtrl.EvIndex, AdjPivCtrl.NfIndex);

            AmbaImg_Proc_Cmd(MW_IP_ADJ_STILL_CONTROL, 0/*TBD*/ , (UINT32)&AdjPivCtrl , 0);
            AmbaImg_Proc_Cmd(MW_IP_SET_STILL_PIPE_CTRL_PARAMS, 0/*TBD*/ , (UINT32)&ImgMode , 0);

            AmbaDSP_ImgSetWbGain(&ImgMode, &VideoWbGain);
            if (AppLibVideoEnc_GetSensorVideoRes() == SENSOR_VIDEO_RES_TRUE_HDR_1080P_HALF ||
                AppLibVideoEnc_GetSensorVideoRes() == SENSOR_VIDEO_RES_WQHD_HALF_HDR) {
                    AmbaSensor_GetStatus(AppEncChannel, &SensorStatus);
                    SensorInfo.SensorPattern = (UINT8)SensorStatus.ModeInfo.OutputInfo.CfaPattern;
                    SensorInfo.SensorResolution = (UINT8)SensorStatus.ModeInfo.OutputInfo.NumDataBits;
                    AmbaDSP_ImgSetVinSensorInfo(&ImgMode, &SensorInfo);
            }

        }
    }

    return 0;
}
static AMP_STILLENC_PREP_s PIVPreCapCB = {.Process = AppLibVideoEnc_PIVCapturePreCB};

/**
 *  @brief  single capture PostProc Callback
 *
 *  Stage1: RAWCAP  -> Dump raw
 *  Stage2: RAW2YUV -> Dump yuv
 *
 *  @param [in] info postproce information
 *
 *  @return 0 - success, -1 - fail
 */
UINT32 AppLibVideoEnc_PIVCapturePostCB(AMP_STILLENC_POSTP_INFO_s *info)
{
    if (info->StageCnt == 1) {

#ifdef CONFIG_APP_ARD
       /* To fill EXIF tags. */
       {
        COMPUTE_EXIF_PARAMS_s ExifParam = {0};
        EXIF_INFO_s ExifInfo = {0};
        APPLIB_EXIF_DATA_s ExifData = {0};

        ExifParam.AeIdx = 0; //TBD
        ExifParam.Mode = IMG_EXIF_PIV;
        Amba_Img_Exif_Compute_AAA_Exif(&ExifParam);
        Amba_Img_Exif_Get_Exif_Info(ExifParam.AeIdx, &ExifInfo);

        //AmbaPrint("[AmpUT][Still Exif]");
        //AmbaPrint("======== AE ========");
        //AmbaPrint("ExpTime    : %u/%u sec", ExifInfo.ExposureTimeNum, ExifInfo.ExposureTimeDen);
        //AmbaPrint("ShtSpeed   : %u/%u", ExifInfo.ShutterSpeedNum, ExifInfo.ShutterSpeedDen);
        //AmbaPrint("ISO        : %d", ExifInfo.IsoSpeedRating);
        //AmbaPrint("Flash      : %d", ExifInfo.Flash);
        //AmbaPrint("MeterMode  : %d", ExifInfo.MeteringMode);
        //AmbaPrint("Sensing    : %d", ExifInfo.SensingMethod);
        //AmbaPrint("ExpMode    : %d", ExifInfo.ExposureMode);
        //AmbaPrint("LightSource: %d", ExifInfo.LightSource);
        //AmbaPrint("======== AWB =======");
        //AmbaPrint("WB         : %d", ExifInfo.WhiteBalance);
        //AmbaPrint("EVBias     : %u/%u", ExifInfo.ExposureBiasValueNum, ExifInfo.ExposureBiasValueDen);
        //AmbaPrint("ColorSpace : %d", ExifInfo.ColorSpace);
        //AmbaPrint("====================");

        // ISO speed
        ExifData.TagId = APPLIB_EXIF_TAG_ISOSpeedRatings;
        ExifData.DataType = APPLIB_TAG_TYPE_SHORT;
        ExifData.DataLength = 1 * sizeof(UINT16);
        ExifData.Value = ExifInfo.IsoSpeedRating;
        AppLibFormatMuxExifPiv_ConfigExifTag(&ExifData);

        ExifData.TagId = APPLIB_EXIF_TAG_ISOSpeed;
        ExifData.DataType = APPLIB_TAG_TYPE_LONG;
        ExifData.DataLength = 1 * sizeof(UINT32);
        ExifData.Value = ExifInfo.IsoSpeedRating;
        AppLibFormatMuxExifPiv_ConfigExifTag(&ExifData);

        // Exposure time
        ExifData.TagId = APPLIB_EXIF_TAG_ExposureTime;
        ExifData.DataType = APPLIB_TAG_TYPE_RATIONAL;
        ExifData.DataLength = 8 * sizeof(UINT8);
        ExifData.Value = ((UINT64)ExifInfo.ExposureTimeNum << 32) | ExifInfo.ExposureTimeDen;
        AppLibFormatMuxExifPiv_ConfigExifTag(&ExifData);

        ApplibFormatMuxExifPiv_IncreaseRawCount();
       }
#endif

        //Do nothing
        /**Direct use video encode yuv data, so can not dump raw and yuv at this callback*/
    } else if (info->StageCnt == 2) {

    } else {
        /**do nothing*/
    }

    return 0;
}

static AMP_STILLENC_POSTP_s PIVPostCapCB = {.Process = AppLibVideoEnc_PIVCapturePostCB};

UINT32 AppLibVideoEnc_PIVRawCapCB(AMP_STILLENC_RAWCAP_FLOW_CTRL_s *ctrl)
{
    AMP_STILLENC_RAWCAP_DSP_CTRL_s dspCtrl;
    memset(&dspCtrl, 0x0, sizeof(AMP_STILLENC_RAWCAP_DSP_CTRL_s));

    dspCtrl.VidSkip = 0; //TBD
    dspCtrl.RawCapNum = ctrl->TotalRawToCap;/* determine rawCapNum */
    dspCtrl.StillProc = G_iso;
    AmpStillEnc_StartRawCapture(StillEncPriPIV, &dspCtrl);

    return 0;
}

static int AppLibVideoEnc_GetDayLumaThresholdCB(int mode, UINT32 *threshold)
{
    //AmbaPrintColor(BLUE, "Day Luma Mode %d", mode);
    switch (mode) {
    case DAY_LUMA_NORMAL:
        *threshold = 124900;
        break;
    case DAY_LUMA_HDR:
        *threshold = 181550;
        break;
    case DAY_LUMA_OVERSAMPLE:
        *threshold = 259400;
        break;
    default:
        *threshold = 124900;
        break;
    }

    return 0;
}

static int AppLibVideoEnc_GetSceneComplexityRangeCB(int mode, UINT32 *complexMin, UINT32 *complexMid, UINT32 *complexMax)
{
    //AmbaPrintColor(BLUE, "Complex Mode: %d", mode);
    switch (mode) {
    case COMPLEX_DAY_NORMAL:
        *complexMin = 31000; *complexMid = 52900; *complexMax = 66200;
        break;
    case COMPLEX_NIGHT_NORMAL:
        *complexMin = 18700; *complexMid = 32600; *complexMax = 40800;
        break;
    case COMPLEX_DAY_HDR:
        *complexMin = 24800; *complexMid = 40000; *complexMax = 50000;
        break;
    case COMPLEX_NIGHT_HDR:
        *complexMin = 13600; *complexMid = 26000; *complexMax = 32500;
        break;
    case COMPLEX_DAY_OVERSAMPLE:
        *complexMin = 29100; *complexMid = 46900; *complexMax = 58700;
        break;
    case COMPLEX_NIGHT_OVERSAMPLE:
        *complexMin = 16900; *complexMid = 30400; *complexMax = 38000;
        break;
    default:
        *complexMin = 31000; *complexMid = 52900; *complexMax = 66200;
        break;
    }
    return 0;
}

static UINT8 VideoEncPriQpIsZero = 0;
static int VideoEncPriCurrQpMinI = 0;
static int VideoEncPriCurrQpMaxI = 0;
static int VideoEncPriCurrQpMinP = 0;
static int VideoEncPriCurrQpMaxP = 0;
static int VideoEncPriCurrQpMinB = 0;
static int VideoEncPriCurrQpMaxB = 0;

static UINT8 VideoEncSecQpIsZero = 0;
static int VideoEncSecCurrQpMinI = 0;
static int VideoEncSecCurrQpMaxI = 0;
static int VideoEncSecCurrQpMinP = 0;
static int VideoEncSecCurrQpMaxP = 0;
static int VideoEncSecCurrQpMinB = 0;
static int VideoEncSecCurrQpMaxB = 0;
#define BITRATE_UNDER_SPEC (1)
#define BITRATE_FIT_SPEC   (2)
#define BITRATE_OVER_SPEC  (3)
static int AppLibVideoEnc_QpAdjustmentCB(int mode, UINT8 *isQpModify, AMBA_IMG_ENCODE_MONITOR_STRM_HDLR_s *strmHdlr, AMBA_IMG_ENC_MONITOR_BITRATE_HDLR_s *hdlr)
{
    AMBA_ENCMONITOR_RUNTIME_QUALITY_CFG_s Cfg = {0};
    int *QpMinI = NULL, *QpMaxI = NULL;
    int *QpMinP = NULL, *QpMaxP = NULL;
    int *QpMinB = NULL, *QpMaxB = NULL;
    UINT8 IsFound = 1, *QpIsZero = NULL;

    *isQpModify = 0;
    if ((hdlr) == BrcHdlrPri) {
        QpMinI = &VideoEncPriCurrQpMinI;
        QpMaxI = &VideoEncPriCurrQpMaxI;
        QpMinP = &VideoEncPriCurrQpMinP;
        QpMaxP = &VideoEncPriCurrQpMaxP;
        QpMinB = &VideoEncPriCurrQpMinB;
        QpMaxB = &VideoEncPriCurrQpMaxB;
        QpIsZero = &VideoEncPriQpIsZero;
    } else if ((hdlr) == BrcHdlrSec) {
        QpMinI = &VideoEncSecCurrQpMinI;
        QpMaxI = &VideoEncSecCurrQpMaxI;
        QpMinP = &VideoEncSecCurrQpMinP;
        QpMaxP = &VideoEncSecCurrQpMaxP;
        QpMinB = &VideoEncSecCurrQpMinB;
        QpMaxB = &VideoEncSecCurrQpMaxB;
        QpIsZero = &VideoEncSecQpIsZero;
    } else {
        IsFound = 0;
    }

    /*
     * If bit rate is under SPEC, that means scene may be too simple that needs to change QP.
     * Below is SAMPLE code to show how to change QP settings to increase bit rate.
     *
     */
    if (IsFound == 1) {
        switch (mode) {
        case BITRATE_UNDER_SPEC:
            if ((*QpMinI == 0) && (*QpMinP == 0)) {
                if (*QpIsZero == 0) {
                    *QpIsZero = 1;
                    AmbaPrintColor(RED, "[AmpUT] <QP_adjust> MinQP of hdlr(0x%X) already set as 0", hdlr);
                }
            } else {
                *QpMinI = ((*QpMinI - 1) >= 0)? (*QpMinI - 1): 0;
                *QpMinP = ((*QpMinP - 1) >= 0)? (*QpMinP - 1): 0;
                *QpMinB = ((*QpMinB - 1) >= 0)? (*QpMinB - 1): 0;
                Cfg.Cmd |= QC_QP;
                Cfg.QpMinI = (UINT8) *QpMinI;
                Cfg.QpMaxI = (UINT8) *QpMaxI;
                Cfg.QpMinP = (UINT8) *QpMinP;
                Cfg.QpMaxP = (UINT8) *QpMaxP;
                Cfg.QpMinB = (UINT8) *QpMinB;
                Cfg.QpMaxB = (UINT8) *QpMaxB;
                AmbaEncMonitor_SetRuntimeQuality(strmHdlr, &Cfg);
                *isQpModify = 1;
            }
            break;
        case BITRATE_FIT_SPEC:
            break;
        case BITRATE_OVER_SPEC:
            break;
        default:
            break;
        }
    } else {
        AmbaPrint("Unknown brc hdlr 0x%X", hdlr);
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
int AppLibVideoEnc_LiveViewInit(void)
{
    int ReturnValue = 0;
    AMBA_SENSOR_MODE_INFO_s VinInfo;
    AMBA_SENSOR_MODE_ID_u Mode = {0};
    APPLIB_SENSOR_VIDEO_ENC_CONFIG_s *VideoEncConfigData;
    APPLIB_SENSOR_VIN_CONFIG_s VinConfigData;
    UINT8 HdrEnable;


    memset(&VinInfo, 0x0, sizeof(AMBA_SENSOR_MODE_INFO_s));

    DBGMSG("[Applib - VideoEnc] <LiveViewInit> Start");
    VideoEncConfigData = AppLibSysSensor_GetVideoConfig(AppLibVideoEnc_GetSensorVideoRes());
    VinConfigData.ResID = AppLibVideoEnc_GetSensorVideoRes();
    Mode.Data = AppLibSysSensor_GetVinMode(&VinConfigData);
    AmbaSensor_GetModeInfo(AppEncChannel, Mode, &VinInfo);
    {
        DBGMSG("[Applib - VideoEnc] <LiveViewInit> VinInfo.MinFrameRate.NumUnitsInTick = %d",VinInfo.MinFrameRate.NumUnitsInTick);
        DBGMSG("[Applib - VideoEnc] <LiveViewInit> VinInfo.MinFrameRate.TimeScale = %d",VinInfo.MinFrameRate.TimeScale);
        DBGMSG("[Applib - VideoEnc] <LiveViewInit> VinInfo.FrameTime.FrameRate.NumUnitsInTick = %d",VinInfo.FrameTime.FrameRate.NumUnitsInTick);
        DBGMSG("[Applib - VideoEnc] <LiveViewInit> VinInfo.FrameTime.FrameRate.TimeScale = %d",VinInfo.FrameTime.FrameRate.TimeScale);
    }

    /**Enable HDR Hybrid liso mode*/
    if (AppLibVideoEnc_GetSensorVideoRes() == SENSOR_VIDEO_RES_TRUE_HDR_1080P_HALF ||
        AppLibVideoEnc_GetSensorVideoRes() == SENSOR_VIDEO_RES_WQHD_HALF_HDR) {
       HdrEnable = 1;
    } else {
       HdrEnable = 0;
    }
    /* Create VIN instance */
    {
        AMP_VIN_CFG_s VinCfg = {0};
        AMP_VIN_LAYOUT_CFG_s Layout[2]; /**< Dualstream from same vin/vcapwindow */
        memset(&Layout, 0x0, 2*sizeof(AMP_VIN_LAYOUT_CFG_s));
        VinCfg.Mode = Mode;
        VinCfg.Channel = AppEncChannel;
        VinCfg.LayoutNumber = 2;
        VinCfg.HwCaptureWindow.Width = VideoEncConfigData->CaptureWidth;
        VinCfg.HwCaptureWindow.Height = VideoEncConfigData->CaptureHeight;
        VinCfg.HwCaptureWindow.X = VinInfo.OutputInfo.RecordingPixels.StartX +
            (((VinInfo.OutputInfo.RecordingPixels.Width - VinCfg.HwCaptureWindow.Width)/2)&0xFFF8);
        VinCfg.HwCaptureWindow.Y = (VinInfo.OutputInfo.RecordingPixels.StartY +
            ((VinInfo.OutputInfo.RecordingPixels.Height - VinCfg.HwCaptureWindow.Height)/2)) & 0xFFFE;
        Layout[0].Width = VideoEncConfigData->EncodeWidth;
        Layout[0].Height = VideoEncConfigData->EncodeHeight;
        Layout[0].EnableSourceArea = 0; /**< Get all capture window to main */
        Layout[0].DzoomFactorX = InitZoomX;
        Layout[0].DzoomFactorY = InitZoomY;
        Layout[0].DzoomOffsetX = 0;
        Layout[0].DzoomOffsetY = 0;
        Layout[0].MainviewReportRate  = 1;
        Layout[1].Width = SecStreamWidth;
        Layout[1].Height = SecStreamHeight;
        Layout[1].EnableSourceArea = 0;/**< Get all capture window to main */
        Layout[1].DzoomFactorX = InitZoomX;
        Layout[1].DzoomFactorY = InitZoomY;
        Layout[1].DzoomOffsetX = 0;
        Layout[1].DzoomOffsetY = 0;
        VinCfg.Layout = &Layout[0];
        VinCfg.cbEvent = AppLibVideoEnc_VinEventCallback;
        VinCfg.cbSwitch= AppLibVideoEnc_VinSwitchCallback;
        ReturnValue = AppLibSysVin_Config(&VinCfg);
        DBGMSGc2(GREEN,"[Applib - VideoEnc] <LiveViewInit> AmpVin_ConfigHandler ReturnValue = %d",ReturnValue);
        if (ReturnValue != OK) {
            /**AppLibSysVin_Config always return failure. So disable debug message*/
            DBGMSGc2(RED,"[Applib - VideoEnc] <LiveViewInit> AmpVin_Create failure!!");
        }
    }
    /**encode monitor default set as enable*/
    ApplibVideoEncVideoSetting.BitRateMonitor = BITRATE_MONITOR_ON;
    ApplibVideoEncVideoSetting.AQPMonitor = AQP_MONITOR_ON;
    /* Register encode monitor instance*/
    if (ApplibVideoEncVideoSetting.BitRateMonitor == BITRATE_MONITOR_ON || ApplibVideoEncVideoSetting.AQPMonitor == AQP_MONITOR_ON) {

        AMBA_DSP_EVENT_VIDEO_ENC_STATUS_s Stream;

        memset(&Stream, 0x0, sizeof(AMBA_DSP_EVENT_VIDEO_ENC_STATUS_s));

        Stream.StreamId = 0; //Pri
        Stream.ChannelId = 0; //TBD
        if (EncMonitorStrmHdlrPri == NULL) {
            ReturnValue = AmbaEncMonitor_StreamRegister(Stream, &EncMonitorStrmHdlrPri);
            if (ReturnValue != OK) {
                AmbaPrintColor(RED,"[Applib - VideoEnc] <LiveViewInit> Pri AmbaEncMonitor_StreamRegister failure!!");
            }
        }
        if (AppLibVideoEnc_GetDualStreams()) {
            Stream.StreamId = 1; //Sec
            Stream.ChannelId = 0; //TBD
            if (EncMonitorStrmHdlrSec == NULL) {
                ReturnValue = AmbaEncMonitor_StreamRegister(Stream, &EncMonitorStrmHdlrSec);
                if (ReturnValue != OK) {
                    AmbaPrintColor(RED,"[Applib - VideoEnc] <LiveViewInit> Sec AmbaEncMonitor_StreamRegister failure!!");
                }
            }
        }
   }

    /* Create video encoder objects */
    {
        AMP_VIDEOENC_HDLR_CFG_s EncCfg;
        AMP_VIDEOENC_LAYER_DESC_s EncLayer = {0, 0, 0, AMP_ENC_SOURCE_VIN, 0, 0, {0,0,0,0},{0,0,0,0}};
        memset(&EncCfg, 0x0, sizeof(AMP_VIDEOENC_HDLR_CFG_s));
        EncCfg.MainLayout.Layer = &EncLayer;
        AmpVideoEnc_GetDefaultCfg(&EncCfg);
        /* Assign callback */
        EncCfg.cbEvent = AppLibVideoEnc_VideoEncCallback;
        /* Assign DSP working memory address. */
        {
            extern UINT32 ApplibDspWorkAreaResvSize;
            EncCfg.DspWorkBufAddr = ApplibDspWorkAreaResvStart;
            EncCfg.DspWorkBufSize = ApplibDspWorkAreaResvSize;
        }
        if (VideoEncPri == NULL) {
            /* Assign main layout */
            EncCfg.MainLayout.Width = VideoEncConfigData->EncodeWidth;
            EncCfg.MainLayout.Height = VideoEncConfigData->EncodeHeight;
            EncCfg.MainLayout.LayerNumber = 1;
            EncCfg.MainTimeScale = VideoEncConfigData->EncNumerator;
            EncCfg.MainTickPerPicture = VideoEncConfigData->EncDenominator;
            EncCfg.SysFreq.ArmCortexFreq = AMP_SYSTEM_FREQ_KEEPCURRENT;
            EncCfg.SysFreq.IdspFreq = AMP_SYSTEM_FREQ_POWERSAVING;
            EncCfg.SysFreq.CoreFreq = AMP_SYSTEM_FREQ_PERFORMANCE;
            EncLayer.SourceType = AMP_ENC_SOURCE_VIN;
            EncLayer.Source = AppVinA;
            EncLayer.SourceLayoutId = 0;
            EncLayer.EnableSourceArea = 0; /**< No source cropping */
            EncLayer.EnableTargetArea = 0; /**< No target pip */
            EncCfg.EventDataReadySkipNum = 0; /**< File data ready every frame */
            EncCfg.StreamId = AMP_VIDEOENC_STREAM_PRIMARY;
            /**HDR Mode use hybrid low iso*/
            if (HdrEnable) {
                EncCfg.LiveViewProcMode = 1;
                EncCfg.LiveViewAlgoMode = 1;
            } else {
                /**Express mode*/
                EncCfg.LiveViewProcMode = 0;
                EncCfg.LiveViewAlgoMode = 0;
            }
            if ((VideoEncConfigData->EncNumerator/VideoEncConfigData->EncDenominator) >= 100 ||EncCfg.LiveViewProcMode == 1) {
            /* Display the OverSampling function in the High Frame Rate setting. */
                EncCfg.LiveViewOSMode = 0;
            } else if (EncCfg.LiveViewProcMode == 0) {
                if (VideoEncConfigData->CaptureWidth > 1920 || \
                    VideoEncConfigData->EncodeWidth > 1920)
                    EncCfg.LiveViewOSMode = 1;
            }
            EncCfg.LiveViewOSMode = 0;
            if (VinInfo.HdrInfo.HdrType == AMBA_SENSOR_HDR_TYPE_MULTI_SLICE) {
                EncCfg.LiveViewHdrMode = 1;
            } else {
                EncCfg.LiveViewHdrMode = 0;
            }

           /* Create primary stream handler */
            ReturnValue = AmpVideoEnc_Create(&EncCfg, &VideoEncPri); /**< Don't have to worry about h.264 spec settings when liveview */
            if (ReturnValue != OK) {
                AmbaPrintColor(RED,"[Applib - VideoEnc] <LiveViewInit> AmpVideoEnc_Create failure!!");
            }
        }
        if (VideoEncSec == NULL && AppLibVideoEnc_GetDualStreams() ) {
            /* Assign Secondary main layout */
            EncCfg.MainLayout.Width = SecStreamWidth;
            EncCfg.MainLayout.Height = SecStreamHeight;
            EncCfg.MainLayout.LayerNumber = 1;
            EncCfg.MainTimeScale = SecStreamTimeScale;
            EncCfg.MainTickPerPicture = SecStreamTick;
            EncCfg.SysFreq.ArmCortexFreq = AMP_SYSTEM_FREQ_KEEPCURRENT;
            EncCfg.SysFreq.IdspFreq = AMP_SYSTEM_FREQ_POWERSAVING;
            EncCfg.SysFreq.CoreFreq = AMP_SYSTEM_FREQ_PERFORMANCE;
            EncLayer.SourceType = AMP_ENC_SOURCE_VIN;
            EncLayer.Source = AppVinA;
            EncLayer.EnableSourceArea = 0;  // No source cropping
            EncLayer.EnableTargetArea = 0;  // No target pip
            EncLayer.SourceLayoutId = 1; // from 2nd layout of VideoEncVinA
            EncCfg.EventDataReadySkipNum = 0;  // File data ready every frame
            EncCfg.StreamId = AMP_VIDEOENC_STREAM_SECONDARY;
            EncCfg.LiveViewProcMode = 0;
            EncCfg.LiveViewAlgoMode = 0;
            EncCfg.LiveViewOSMode = 0;
            EncCfg.LiveViewHdrMode = 0;
            /* Create secondary stream handler */
            ReturnValue = AmpVideoEnc_Create(&EncCfg, &VideoEncSec); // Don't have to worry about h.264 spec settings when liveview
            if (ReturnValue != OK) {
                AmbaPrintColor(RED,"[Applib - VideoEnc] <LiveViewInit> AmpVideoEnc_Create failure!!");
            }
        }
    }
    /* Register pipeline */
    if (VideoEncPipe == NULL) {
        extern AMP_AVENC_HDLR_s *AudioEncPriHdlr;
        extern AMP_AVENC_HDLR_s *AudioEncSecHdlr;
        AMP_ENC_PIPE_CFG_s PipeCfg;
        int RecMode = REC_MODE_AV;

        memset(&PipeCfg, 0x0, sizeof(AMP_ENC_PIPE_CFG_s));
        AmpEnc_GetDefaultCfg(&PipeCfg);

        /* To add EXTEND codec. */
        if  (AppLibExtendEnc_GetEnableStatus()) {
            extern AMP_AVENC_HDLR_s *VideoEncExtendHdlr;
            if (VideoEncExtendHdlr != NULL) {
                PipeCfg.encoder[PipeCfg.numEncoder++] = VideoEncExtendHdlr;
            }
        }

        /* To get recode mode. */
        RecMode = AppLibVideoEnc_GetRecMode();

        /*************************************************************************************
         * [NOTE] AV enc need to add audio encoder before video encoder                      *
         *        to ensure the mw av encode state correct                                   *
         *        Please double confirm the settings in AppLibVideoEnc_PipeChange() !!!!!    *
         *************************************************************************************/
        /* To add AUDIO codec. */
        if (RecMode == REC_MODE_AV) {
            extern AMP_AVENC_HDLR_s *AudioEncPriHdlr;
            AppLibAudioEnc_Setup();
            PipeCfg.encoder[PipeCfg.numEncoder++] = AudioEncPriHdlr;

            if ( AppLibVideoEnc_GetDualStreams() && AppLibAudioEnc_GetDualStreams() ) {
                extern AMP_AVENC_HDLR_s *AudioEncSecHdlr;
                PipeCfg.encoder[PipeCfg.numEncoder++] = AudioEncSecHdlr;
            }
        }

        /* To add VIDEO codec. */
        PipeCfg.encoder[PipeCfg.numEncoder++] = VideoEncPri;
        if (AppLibVideoEnc_GetDualStreams()) {
            PipeCfg.encoder[PipeCfg.numEncoder++] = VideoEncSec;
        }

        PipeCfg.cbEvent = AppLibVideoEnc_PipeCallback;
        PipeCfg.type = AMP_ENC_AV_PIPE;
        ReturnValue = AmpEnc_Create(&PipeCfg, &VideoEncPipe);
        if (ReturnValue != OK) {
            AmbaPrintColor(RED,"[Applib - VideoEnc] <LiveViewInit> AmpEnc_Create failure!!");
        }

        ReturnValue = AmpEnc_Add(VideoEncPipe);
        if (ReturnValue != OK) {
            AmbaPrintColor(RED,"[Applib - VideoEnc] <LiveViewInit> AmpEnc_Add failure!!");
        }
    }

    if (ReturnValue == OK)
        ApplibVideoEncLiveviewInitFlag = 0;
    return ReturnValue;
}

/**
 *  @brief Configure the live view
 *
 *  Configure the live view
 *
 *  @return >=0 success, <0 failure
 */
int AppLibVideoEnc_LiveViewSetup(void)
{
    int ReturnValue = 0;
    AMP_VIN_RUNTIME_CFG_s VinCfg = {0};
    AMP_VIDEOENC_MAIN_CFG_s MainCfg[2] = {0};
    UINT32 NumMainCfg = 0;
    AMP_VIDEOENC_LAYER_DESC_s NewPriLayer = {0, 0, 0, AMP_ENC_SOURCE_VIN, 0, 0, {0,0,0,0},{0,0,0,0}};
    AMP_VIDEOENC_LAYER_DESC_s NewSecLayer = {0, 0, 0, AMP_ENC_SOURCE_VIN, 0, 0, {0,0,0,0},{0,0,0,0}};
    AMBA_SENSOR_MODE_INFO_s VinInfo;
    AMP_VIN_LAYOUT_CFG_s Layout[2]; /**<  Dualstream from same vin/vcapwindow */
    AMBA_SENSOR_MODE_ID_u Mode = {0};
    UINT8 HdrEnable;

    UINT32 DdrClk = 0;
    UINT16 CustomMaxCortexFreq = 0;
    UINT16 CustomMaxIdspFreq = 0;
    UINT16 CustomMaxCoreFreq = 0;

    APPLIB_SENSOR_VIDEO_ENC_CONFIG_s *VideoEncConfigData;
    APPLIB_SENSOR_VIN_CONFIG_s VinConfigData;
    AMP_VIDEOENC_BITSTREAM_CFG_s BitsCfg;
    APPLIB_SENSOR_PIV_ENC_CONFIG_s PIVCapConfig;
    memset(&VinInfo, 0x0, sizeof(AMBA_SENSOR_MODE_INFO_s));
    memset(&Layout, 0x0, 2*sizeof(AMP_VIN_LAYOUT_CFG_s));
    memset(&MainCfg, 0x0, 2*sizeof(AMP_VIDEOENC_MAIN_CFG_s));
    DBGMSG("[Applib - VideoEnc] <LiveViewSetup> Start");

    if (ApplibVideoEncLiveviewInitFlag < 0) {
        AppLibVideoEnc_LiveViewInit();
    }

    /**Enable HDR Hybrid liso mode*/
    if (AppLibVideoEnc_GetSensorVideoRes() == SENSOR_VIDEO_RES_TRUE_HDR_1080P_HALF ||
        AppLibVideoEnc_GetSensorVideoRes() == SENSOR_VIDEO_RES_WQHD_HALF_HDR) {
       HdrEnable = 1;
    } else {
       HdrEnable = 0;
    }

    VideoEncConfigData = AppLibSysSensor_GetVideoConfig(AppLibVideoEnc_GetSensorVideoRes());
    VinConfigData.ResID = AppLibVideoEnc_GetSensorVideoRes();
    Mode.Data = AppLibSysSensor_GetVinMode(&VinConfigData);
    AmbaSensor_GetModeInfo(AppEncChannel, Mode, &VinInfo);

    {
        AMBA_IMG_SCHDLR_CFG_s ISCfg = {0};

        AppLibImage_DeleteImgSchdlr(0);

        ISCfg.MainViewID = 0;
        ISCfg.Channel = AppEncChannel;
        ISCfg.Vin = AppVinA;
        ISCfg.cbEvent = AppLibVideoEnc_ImgSchdlrCallback;
        if (HdrEnable) {
            ISCfg.VideoProcMode = 1;
            if (VinInfo.HdrInfo.HdrType == AMBA_SENSOR_HDR_TYPE_MULTI_SLICE) {
                ISCfg.VideoProcMode |= 0x10;
            }
        }
        /* Keep the report rate lower than 60fps. */
            if ((VideoEncConfigData->EncNumerator/VideoEncConfigData->EncDenominator) >= 200) {
                ISCfg.AAAStatSampleRate= 4;
            } else if ((VideoEncConfigData->EncNumerator/VideoEncConfigData->EncDenominator) >= 100) {
                ISCfg.AAAStatSampleRate  = 2;
            } else {
                ISCfg.AAAStatSampleRate  = 1;
            }
        ReturnValue = AppLibImage_CreateImgSchdlr(&ISCfg, 0);
        if (ReturnValue != OK) {
            AmbaPrintColor(RED,"[Applib - VideoEnc] <LiveViewInit> CreateImgSchdlr failure!!");
        }
    }



    if (0) {
        AmbaPrint("[Applib - VideoEnc] <LiveViewSetup> VinInfo.MinFrameRate.NumUnitsInTick = %d",VinInfo.MinFrameRate.NumUnitsInTick);
        AmbaPrint("[Applib - VideoEnc] <LiveViewSetup> VinInfo.MinFrameRate.TimeScale = %d",VinInfo.MinFrameRate.TimeScale);
        AmbaPrint("[Applib - VideoEnc] <LiveViewSetup> VinInfo.FrameTime.FrameRate.NumUnitsInTick = %d",VinInfo.FrameTime.FrameRate.NumUnitsInTick);
        AmbaPrint("[Applib - VideoEnc] <LiveViewSetup> VinInfo.FrameTime.FrameRate.TimeScale = %d",VinInfo.FrameTime.FrameRate.TimeScale);
        AmbaPrint("[Applib - VideoEnc] <LiveViewSetup> VinInfo.OutputInfo.OutputHeight = %d",VinInfo.OutputInfo.OutputHeight);
        AmbaPrint("[Applib - VideoEnc] <LiveViewSetup> VinInfo.OutputInfo.OutputWidth = %d",VinInfo.OutputInfo.OutputWidth);
    }

    VinCfg.Hdlr = AppVinA;
    VinCfg.Mode = Mode;
    VinCfg.LayoutNumber = 2;
    Layout[0].ActiveArea.Width = VinCfg.HwCaptureWindow.Width = VideoEncConfigData->CaptureWidth;
    Layout[0].ActiveArea.Height = VinCfg.HwCaptureWindow.Height = VideoEncConfigData->CaptureHeight;
    Layout[0].ActiveArea.X = VinCfg.HwCaptureWindow.X = VinInfo.OutputInfo.RecordingPixels.StartX +
        (((VinInfo.OutputInfo.RecordingPixels.Width - VinCfg.HwCaptureWindow.Width)/2)&0xFFF8);
    Layout[0].ActiveArea.Y = VinCfg.HwCaptureWindow.Y = VinInfo.OutputInfo.RecordingPixels.StartY +
        (((VinInfo.OutputInfo.RecordingPixels.Height - VinCfg.HwCaptureWindow.Height)/2) & 0xFFFE);
    Layout[0].EnableOBArea = 0;
    Layout[0].Width = VideoEncConfigData->EncodeWidth;
    Layout[0].Height = VideoEncConfigData->EncodeHeight;
    Layout[0].EnableSourceArea = 0; /**< Get all capture window to main */
    Layout[0].DzoomFactorX = InitZoomX;
    Layout[0].DzoomFactorY = InitZoomY;
    Layout[0].DzoomOffsetX = 0;
    Layout[0].DzoomOffsetY = 0;
    /* Keep the report rate lower than 60fps. */
    if ((VideoEncConfigData->EncNumerator/VideoEncConfigData->EncDenominator) >= 200) {
        Layout[0].MainviewReportRate  = 4;
    } else if ((VideoEncConfigData->EncNumerator/VideoEncConfigData->EncDenominator) >= 100) {
        Layout[0].MainviewReportRate  = 2;
    } else {
        Layout[0].MainviewReportRate  = 1;
    }
    Layout[1].Width = SecStreamWidth;
    Layout[1].Height = SecStreamHeight;
    Layout[1].EnableSourceArea = 0; /**< Get all capture window to main */
    Layout[1].DzoomFactorX = InitZoomX;
    Layout[1].DzoomFactorY = InitZoomY;
    Layout[1].DzoomOffsetX = 0;
    Layout[1].DzoomOffsetY = 0;
    VinCfg.Layout = &Layout[0];

    if (0) {
        AmbaPrint("[Applib - VideoEnc] <LiveViewSetup> VinCfg.HwCaptureWindow.Width = %d",VinCfg.HwCaptureWindow.Width);
        AmbaPrint("[Applib - VideoEnc] <LiveViewSetup> VinCfg.HwCaptureWindow.Height = %d",VinCfg.HwCaptureWindow.Height);
        AmbaPrint("[Applib - VideoEnc] <LiveViewSetup> VinCfg.HwCaptureWindow.X = %d",VinCfg.HwCaptureWindow.X);
        AmbaPrint("[Applib - VideoEnc] <LiveViewSetup> VinCfg.HwCaptureWindow.Y = %d",VinCfg.HwCaptureWindow.Y);
    }

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
    MainCfg[0].Hdlr = VideoEncPri;
    MainCfg[0].MainLayout.LayerNumber = 1;
    MainCfg[0].MainLayout.Layer = &NewPriLayer;
    MainCfg[0].MainLayout.Width = VideoEncConfigData->EncodeWidth;
    MainCfg[0].MainLayout.Height = VideoEncConfigData->EncodeHeight;
    MainCfg[0].Interlace = 0;
    MainCfg[0].MainTickPerPicture = VideoEncConfigData->EncDenominator;
    MainCfg[0].MainTimeScale = VideoEncConfigData->EncNumerator;
    MainCfg[0].SysFreq.ArmCortexFreq = AMP_SYSTEM_FREQ_KEEPCURRENT;
    MainCfg[0].SysFreq.IdspFreq = AMP_SYSTEM_FREQ_POWERSAVING;
    MainCfg[0].SysFreq.CoreFreq = AMP_SYSTEM_FREQ_PERFORMANCE;
    MainCfg[0].SysFreq.MaxArmCortexFreq = CustomMaxCortexFreq;
    MainCfg[0].SysFreq.MaxIdspFreq = CustomMaxIdspFreq;
    MainCfg[0].SysFreq.MaxCoreFreq = CustomMaxCoreFreq;

    {
        extern UINT32 ApplibDspWorkAreaResvSize;
        MainCfg[0].DspWorkBufAddr = ApplibDspWorkAreaResvStart;
        MainCfg[0].DspWorkBufSize = ApplibDspWorkAreaResvSize;
    }
    /**HDR Mode use hybrid low iso*/
    if (HdrEnable) {
        MainCfg[0].LiveViewProcMode = 1;
        MainCfg[0].LiveViewAlgoMode = 1;
    } else {
        /**Express mode*/
        MainCfg[0].LiveViewProcMode = 0;
        MainCfg[0].LiveViewAlgoMode = 0;
    }

    if ((MainCfg[0].MainTimeScale/MainCfg[0].MainTickPerPicture) >= 100 ||MainCfg[0].LiveViewProcMode == 1) {
        /* Display the OverSampling function in the High Frame Rate setting. */
        MainCfg[0].LiveViewOSMode = 0;
    } else if (MainCfg[0].LiveViewProcMode == 0) {
        if (( VideoEncConfigData->EncodeWidth > 1280 && VideoEncConfigData->CaptureWidth > 1920 ) || \
            VideoEncConfigData->EncodeWidth > 1920)
            MainCfg[0].LiveViewOSMode = 1;
    }
     if (VinInfo.HdrInfo.HdrType == AMBA_SENSOR_HDR_TYPE_MULTI_SLICE) {
         MainCfg[0].LiveViewHdrMode = 1;
     } else {
         MainCfg[0].LiveViewHdrMode = 0;
     }

    AmpVideoEnc_GetBitstreamConfig(VideoEncPri, &BitsCfg);
    /* Set piv capture size */
    AppLibSysSensor_GetPIVSize(AppLibVideoEnc_GetSensorVideoRes(), &PIVCapConfig);
    BitsCfg.PIVMaxWidth = PIVCapConfig.CaptureWidth;

    BitsCfg.Spec.H264Cfg.Interlace = MainCfg[0].Interlace;
    BitsCfg.Spec.H264Cfg.TimeScale = MainCfg[0].MainTimeScale;
    BitsCfg.Spec.H264Cfg.TickPerPicture = MainCfg[0].MainTickPerPicture;
    AmpVideoEnc_SetBitstreamConfig(VideoEncPri, &BitsCfg);

    NewPriLayer.EnableSourceArea = 0;
    NewPriLayer.EnableTargetArea = 0;
    NewPriLayer.LayerId = 0;
    NewPriLayer.SourceType = AMP_ENC_SOURCE_VIN;
    NewPriLayer.Source = AppVinA;
    NewPriLayer.SourceLayoutId = 0;

    if (AppLibVideoEnc_GetDualStreams()) {

        if (VideoEncConfigData->EncNumerator % AppLibVideoEnc_GetSecStreamTimeScale() != 0) {
            AmbaPrintColor(RED,"[Applib - VideoEnc] <LiveViewSetup> Second stream frame rate is not the divisor of main stream frame rate.");
        }

        MainCfg[1].Hdlr = VideoEncSec;
        MainCfg[1].MainLayout.LayerNumber = 1;
        MainCfg[1].MainLayout.Layer = &NewSecLayer;
        MainCfg[1].MainLayout.Width = SecStreamWidth;
        MainCfg[1].MainLayout.Height = SecStreamHeight;
        MainCfg[1].Interlace = 0;
        MainCfg[1].MainTickPerPicture = SecStreamTick;
        MainCfg[1].MainTimeScale = SecStreamTimeScale;
        MainCfg[1].SysFreq.ArmCortexFreq = AMP_SYSTEM_FREQ_KEEPCURRENT;
        MainCfg[1].SysFreq.IdspFreq = AMP_SYSTEM_FREQ_POWERSAVING;
        MainCfg[1].SysFreq.CoreFreq = AMP_SYSTEM_FREQ_PERFORMANCE;
        MainCfg[1].SysFreq.MaxArmCortexFreq = CustomMaxCortexFreq;
        MainCfg[1].SysFreq.MaxIdspFreq = CustomMaxIdspFreq;
        MainCfg[1].SysFreq.MaxCoreFreq = CustomMaxCoreFreq;

        /**HDR Mode use hybrid low iso*/
        if (HdrEnable) {
            MainCfg[1].LiveViewProcMode = 1;
            MainCfg[1].LiveViewAlgoMode = 1;
        } else {
            /**Express mode*/
            MainCfg[1].LiveViewProcMode = 0;
            MainCfg[1].LiveViewAlgoMode = 0;
        }

        if (VinInfo.HdrInfo.HdrType == AMBA_SENSOR_HDR_TYPE_MULTI_SLICE) {
             MainCfg[1].LiveViewHdrMode = 1;
        } else {
             MainCfg[1].LiveViewHdrMode = 0;
        }

       if ((MainCfg[1].MainTimeScale/MainCfg[0].MainTickPerPicture) >= 100 ||MainCfg[0].LiveViewProcMode == 1) {
            /* Display the OverSampling function in the High Frame Rate setting. */
            MainCfg[1].LiveViewOSMode = 0;
       } else if (MainCfg[1].LiveViewProcMode == 0) {
            if (( VideoEncConfigData->EncodeWidth > 1280 && VideoEncConfigData->CaptureWidth > 1920 ) || \
                VideoEncConfigData->EncodeWidth > 1920)
                MainCfg[1].LiveViewOSMode = 1;
       }
        AmpVideoEnc_GetBitstreamConfig(VideoEncSec, &BitsCfg);
        /* Set piv capture size */
        AppLibSysSensor_GetPIVSize(AppLibVideoEnc_GetSensorVideoRes(), &PIVCapConfig);
        BitsCfg.PIVMaxWidth = PIVCapConfig.CaptureWidth;

        BitsCfg.Spec.H264Cfg.Interlace = MainCfg[1].Interlace;
        BitsCfg.Spec.H264Cfg.TimeScale = MainCfg[1].MainTimeScale;
        BitsCfg.Spec.H264Cfg.TickPerPicture = MainCfg[1].MainTickPerPicture;
        AmpVideoEnc_SetBitstreamConfig(VideoEncSec, &BitsCfg);

       NumMainCfg = 2;
    } else {
        MainCfg[1].MainLayout.Width = 0;
        MainCfg[1].MainLayout.Height = 0;
        NumMainCfg = 1;
        DBGMSG("[Applib - VideoEnc] <LiveViewSetup> Disable dual stream");
    }
    MainCfg[1].DspWorkBufAddr = 0; /**< Don't want to change dsp buffer */
    MainCfg[1].DspWorkBufSize = 0; /**< Don't want to change dsp buffer */
    NewSecLayer.EnableSourceArea = 0;
    NewSecLayer.EnableTargetArea = 0;
    NewSecLayer.LayerId = 0;
    NewSecLayer.SourceType = AMP_ENC_SOURCE_VIN;
    NewSecLayer.Source = AppVinA;
    NewSecLayer.SourceLayoutId = 1;
#if 0
    // Set capture in Dzoom
    {
        AMP_IMG_DZOOM_VIN_INVALID_INFO_s dzoomVinInvalidInfo;
        dzoomVinInvalidInfo.CapW = VideoEncConfigData->CaptureWidth;
        dzoomVinInvalidInfo.CapH = VideoEncConfigData->CaptureHeight;

        AmpImgDzoom_StopDzoom(dzoomHdlr);
        AmpImgDzoom_ResetStatus(dzoomHdlr);
        AmpImgDzoom_ChangeResolutionHandler(dzoomHdlr, &dzoomVinInvalidInfo);
    }
#endif
    ReturnValue = AmpVideoEnc_ConfigVinMain(1, &VinCfg, NumMainCfg, &MainCfg[0]);
    if (ReturnValue != OK) {
        AmbaPrintColor(RED,"[Applib - VideoEnc] <LiveViewSetup> ConfigVinMain failure!!");
    }
    return ReturnValue;
}

/**
 *  @brief Start live view
 *
 *  Start live view
 *
 *  @return >=0 success, <0 failure
 */
int AppLibVideoEnc_LiveViewStart(void)
{
    int ReturnValue = 0;
    ReturnValue = AmpEnc_StartLiveview(VideoEncPipe, AMP_ENC_FUNC_FLAG_WAIT);
    if (ReturnValue != OK) {
        AmbaPrintColor(RED,"[Applib - VideoEnc] <LiveViewStart> failure!!");
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
int AppLibVideoEnc_LiveViewStop(void)
{
    int ReturnValue = 0;

    AppLibImage_StopImgSchdlr(0);
    AppLibImage_Stop3A(VideoEncPri);
    ReturnValue = AmpEnc_StopLiveview(VideoEncPipe, AMP_ENC_FUNC_FLAG_WAIT);
    if (ReturnValue != OK) {
        AmbaPrintColor(RED,"[Applib - VideoEnc] <LiveViewStop> failure!!");
    }
    AppLibImage_DeleteImgSchdlr(0);

     /**delete still encode pipe, stillenc doesn't use piv still pipe*/
     if (StillEncPipePIV != NULL) {
        ReturnValue = AmpEnc_Delete(StillEncPipePIV);
        if (ReturnValue == OK) {
            StillEncPipePIV = NULL;
        } else {
            AmbaPrintColor(RED,"[Applib - VideoEnc] <LiveViewStop> failure!!");
        }
    }

    if (StillEncPriPIV != NULL) {
        ReturnValue = AmpStillEnc_Delete(StillEncPriPIV);
        if (ReturnValue == OK) {
            StillEncPriPIV = NULL;
        } else {
            AmbaPrintColor(RED,"[Applib - VideoEnc] <LiveViewStop> VideoEncPri failure!!");
        }
    }

    /**unregister encode moniter*/
        /*
    if (BrcHdlrPri != NULL) {
        AmbaEncMonitorBRC_UnRegisterService(BrcHdlrPri);
        BrcHdlrPri = NULL;
    }

    if (BrcHdlrSec != NULL) {
        AmbaEncMonitorBRC_UnRegisterService(BrcHdlrSec);
        BrcHdlrSec = NULL;
    }

    if (EncMonitorStrmHdlrPri != NULL) {
        AmbaEncMonitor_StreamUnregister(EncMonitorStrmHdlrPri);
        EncMonitorStrmHdlrPri = NULL;
    }

    if (EncMonitorStrmHdlrSec != NULL) {
        AmbaEncMonitor_StreamUnregister(EncMonitorStrmHdlrSec);
        EncMonitorStrmHdlrSec = NULL;
    }*/
#if 0
    if (VideoEncPipe != NULL) {
        ReturnValue = AmpEnc_Delete(VideoEncPipe);
        if (ReturnValue == OK) {
            VideoEncPipe = NULL;
        } else {
            AmbaPrintColor(RED,"[Applib - VideoEnc] <AmpVideoEnc_Delete> failure!!");
        }
    }

    if (VideoEncPri != NULL) {
        ReturnValue = AmpVideoEnc_Delete(VideoEncPri);
        if (ReturnValue == OK) {
            VideoEncPri = NULL;
        } else {
            AmbaPrintColor(RED,"[Applib - VideoEnc] <AmpVideoEnc_Delete> VideoEncPri failure!!");
        }
    }
    if (VideoEncSec != NULL) {
        ReturnValue = AmpVideoEnc_Delete(VideoEncSec);
        if (ReturnValue == OK) {
            VideoEncSec = NULL;
        } else {
            AmbaPrintColor(RED,"[Applib - VideoEnc] <AmpVideoEnc_Delete> VideoEncSec failure!!");
        }
    }

    if (AppVinA != NULL) {
        ReturnValue = AmpVin_Delete(AppVinA);
        if (ReturnValue == OK) {
            AppVinA = NULL;
        } else {
            AmbaPrintColor(RED,"[Applib - VideoEnc] <AmpVin_Delete> failure!!");
        }
    }
#endif
    ApplibVideoEncLiveviewInitFlag = -1;

    return ReturnValue;
}

/**
 *  @brief change pipe setting
 *
 *  change pipe setting
 *
 *  @return >=0 success, <0 failure
 */
int AppLibVideoEnc_PipeChange(void)
{
    int ReturnValue = 0;
    int RecMode = REC_MODE_AV;
    AMP_ENC_PIPE_CFG_s PipeCfg;

    if (VideoEncPipe != NULL) {
        ReturnValue = AmpEnc_Delete(VideoEncPipe);
        if (ReturnValue != OK) {
            AmbaPrintColor(RED,"[Applib - VideoEnc] <PipeChange> Delete pipe fail !");
            return ReturnValue;
        }
    }

    memset(&PipeCfg, 0x0, sizeof(AMP_ENC_PIPE_CFG_s));
    AmpEnc_GetDefaultCfg(&PipeCfg);

    /* To add EXTEND codec. */
    if ( AppLibExtendEnc_GetEnableStatus() ) {
        extern AMP_AVENC_HDLR_s *VideoEncExtendHdlr;
        if ( VideoEncExtendHdlr != NULL ) {
            PipeCfg.encoder[PipeCfg.numEncoder++] = VideoEncExtendHdlr;
        }
    }

    /* To get recode mode. */
    RecMode = AppLibVideoEnc_GetRecMode();

    /*************************************************************************************
     * [NOTE] AV enc need to add audio encoder before video encoder                      *
     *        to ensure the mw av encode state correct                                   *
     *        Please double confirm the settings in AppLibVideoEnc_LiveViewInit() !!!!!  *
     *************************************************************************************/
    /* To add AUDIO codec. */
    if ( RecMode == REC_MODE_AV ) {
        extern AMP_AVENC_HDLR_s *AudioEncPriHdlr;
        AppLibAudioEnc_Setup();
        PipeCfg.encoder[PipeCfg.numEncoder++] = AudioEncPriHdlr;

        if ( AppLibVideoEnc_GetDualStreams() && AppLibAudioEnc_GetDualStreams() ) {
            extern AMP_AVENC_HDLR_s *AudioEncSecHdlr;
            PipeCfg.encoder[PipeCfg.numEncoder++] = AudioEncSecHdlr;
        }
    }

    /* To add VIDEO codec. */
    PipeCfg.encoder[PipeCfg.numEncoder++] = VideoEncPri;
    if (AppLibVideoEnc_GetDualStreams()) {
        PipeCfg.encoder[PipeCfg.numEncoder++] = VideoEncSec;
    }

    /* To create pipe. */
    PipeCfg.cbEvent = AppLibVideoEnc_PipeCallback;
    PipeCfg.type = AMP_ENC_AV_PIPE;
    ReturnValue = AmpEnc_Create(&PipeCfg, &VideoEncPipe);
    if (ReturnValue != OK) {
        AmbaPrintColor(RED,"[Applib - VideoEnc] <LiveViewInit> AmpEnc_Create failure!!");
        return ReturnValue;
    }

    ReturnValue = AmpEnc_Add(VideoEncPipe);
    if (ReturnValue != OK) {
        AmbaPrintColor(RED,"[Applib - VideoEnc] <LiveViewInit> AmpEnc_Add failure!!");
            return ReturnValue;
    }
    return ReturnValue;

}

int AppLibVideoEnc_PipeDelete(void)
{
    int ReturnValue = 0;
    if (VideoEncPipe != NULL) {
        ReturnValue = AmpEnc_Delete(VideoEncPipe);
        if (ReturnValue != OK) {
            AmbaPrintColor(RED,"[Applib - VideoEnc] <PipeDelete> Delete pipe fail !");
            return ReturnValue;
        }
        VideoEncPipe = NULL;
    }

    if (VideoEncPri != NULL) {
        ReturnValue = AmpVideoEnc_Delete(VideoEncPri);
        if (ReturnValue != OK) {
            AmbaPrintColor(RED,"[Applib - VideoEnc] <PipeDelete> Delete VideoEncPri fail !");
            return ReturnValue;
        }
        VideoEncPri = NULL;
    }

    if (VideoEncSec != NULL) {
        ReturnValue = AmpVideoEnc_Delete(VideoEncSec);
        if (ReturnValue != OK) {
            AmbaPrintColor(RED,"[Applib - VideoEnc] <PipeDelete> Delete VideoEncSec fail !");
            return ReturnValue;
        }
        VideoEncSec = NULL;
    }
    return ReturnValue;
}

static AMP_ROTATION_e VencRotation = AMP_ROTATE_0;
static UINT8 EncPriSpecH264 = 1;
static UINT8 EncSecSpecH264 = 1;
static UINT8 EncIBeat = 0;
static UINT8 MjpegQuantMatrix[128] = {
    0x10, 0x0B, 0x0C, 0x0E, 0x0C, 0x0A, 0x10, 0x0E,
    0x0D, 0x0E, 0x12, 0x11, 0x10, 0x13, 0x18, 0x28,
    0x1A, 0x18, 0x16, 0x16, 0x18, 0x31, 0x23, 0x25,
    0x1D, 0x28, 0x3A, 0x33, 0x3D, 0x3C, 0x39, 0x33,
    0x38, 0x37, 0x40, 0x48, 0x5C, 0x4E, 0x40, 0x44,
    0x57, 0x45, 0x37, 0x38, 0x50, 0x6D, 0x51, 0x57,
    0x5F, 0x62, 0x67, 0x68, 0x67, 0x3E, 0x4D, 0x71,
    0x79, 0x70, 0x64, 0x78, 0x5C, 0x65, 0x67, 0x63,
    0x11, 0x12, 0x12, 0x18, 0x15, 0x18, 0x2F, 0x1A,
    0x1A, 0x2F, 0x63, 0x42, 0x38, 0x42, 0x63, 0x63,
    0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63,
    0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63,
    0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63,
    0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63,
    0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63,
    0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63
};
static UINT32 AppLibVideoEnc_CalBitsBufThreshold(void)
{
    UINT32 VideoThreshold = 0;
    int RemainTime = 2;/**Default remain 2s times for threshold */
    APPLIB_VIDEOENC_BITRATE_s *VideoEncBitRateData = AppLibSysSensor_GetVideoBitRate(AppLibVideoEnc_GetSensorVideoRes(), AppLibVideoEnc_GetQuality());

    VideoThreshold = VideoEncBitRateData->BitRateAvg*RemainTime/8;
    AmbaPrint("[%s] Bits buffer threshold is %d MByte",__func__,VideoThreshold);

    return VideoThreshold;
}

//int AmpUT_VideoEnc_AqpPriStream(AMBA_IMG_ENCODE_MONITOR_STRM_HDLR_s *StrmHdlr)
static int AppLibVideoEnc_AqpPriStream(AMBA_IMG_ENCODE_MONITOR_STRM_HDLR_s *StrmHdlr)
{
    ADJ_AQP_INFO_s AqpInfo;

    memset(&AqpInfo, 0x0, sizeof(ADJ_AQP_INFO_s));
    AmbaImg_Proc_Cmd(MW_IP_GET_ADJ_AQP_INFO, 0/*chNo*/, (UINT32)&AqpInfo, 0);
    if (AqpInfo.UpdateFlg) {
        AMBA_ENCMONITOR_RUNTIME_QUALITY_CFG_s Cfg;

        memset(&Cfg, 0, sizeof(AMBA_ENCMONITOR_RUNTIME_QUALITY_CFG_s));
        Cfg.Cmd |= QC_QMODEL;
        Cfg.AQPStrength = AqpInfo.AQPParams.Value[0];
        Cfg.Intra16x16Bias = AqpInfo.AQPParams.Value[1];
        Cfg.Intra4x4Bias = AqpInfo.AQPParams.Value[2];
        Cfg.Inter16x16Bias = AqpInfo.AQPParams.Value[3];
        Cfg.Inter8x8Bias = AqpInfo.AQPParams.Value[4];
        Cfg.Direct16x16Bias = AqpInfo.AQPParams.Value[5];
        Cfg.Direct8x8Bias = AqpInfo.AQPParams.Value[6];
        Cfg.MELambdaQpOffset = AqpInfo.AQPParams.Value[7];
        Cfg.Alpha = AqpInfo.AQPParams.Value[8];
        Cfg.Beta = AqpInfo.AQPParams.Value[9];
        AmbaEncMonitor_SetRuntimeQuality(StrmHdlr, &Cfg);

        AqpInfo.UpdateFlg = 0;
        AmbaImg_Proc_Cmd(MW_IP_SET_ADJ_AQP_INFO, 0/*chNo*/, (UINT32)&AqpInfo, 0);
    }

    DBGMSGc(GREEN,"[Applib - VideoEnc] <%s> !!",__func__);

    return 0;
}

//int AmpUT_VideoEnc_AqpSecStream(AMBA_IMG_ENCODE_MONITOR_STRM_HDLR_s *StrmHdlr)
static int AppLibVideoEnc_AqpSecStream(AMBA_IMG_ENCODE_MONITOR_STRM_HDLR_s *StrmHdlr)
{
    ADJ_AQP_INFO_s AqpInfo;

    memset(&AqpInfo, 0x0, sizeof(ADJ_AQP_INFO_s));
    AmbaImg_Proc_Cmd(MW_IP_GET_ADJ_AQP_INFO, 0/*chNo*/, (UINT32)&AqpInfo, 0);
    if (AqpInfo.UpdateFlg) {
        AMBA_ENCMONITOR_RUNTIME_QUALITY_CFG_s Cfg;

        memset(&Cfg, 0, sizeof(AMBA_ENCMONITOR_RUNTIME_QUALITY_CFG_s));
        Cfg.Cmd |= QC_QMODEL;
        Cfg.AQPStrength = AqpInfo.AQPParams.Value[0];
        Cfg.Intra16x16Bias = AqpInfo.AQPParams.Value[1];
        Cfg.Intra4x4Bias = AqpInfo.AQPParams.Value[2];
        Cfg.Inter16x16Bias = AqpInfo.AQPParams.Value[3];
        Cfg.Inter8x8Bias = AqpInfo.AQPParams.Value[4];
        Cfg.Direct16x16Bias = AqpInfo.AQPParams.Value[5];
        Cfg.Direct8x8Bias = AqpInfo.AQPParams.Value[6];
        Cfg.MELambdaQpOffset = AqpInfo.AQPParams.Value[7];
        Cfg.Alpha = AqpInfo.AQPParams.Value[8];
        Cfg.Beta = AqpInfo.AQPParams.Value[9];
        AmbaEncMonitor_SetRuntimeQuality(StrmHdlr, &Cfg);

        AqpInfo.UpdateFlg = 0;
        AmbaImg_Proc_Cmd(MW_IP_SET_ADJ_AQP_INFO, 0/*chNo*/, (UINT32)&AqpInfo, 0);

    }

    DBGMSGc(GREEN,"[Applib - VideoEnc] <%s> !!",__func__);

    return 0;
}


/**
 *  @brief Configure the encoder's parameter
 *
 *  Configure the encoder's parameter
 *
 *  @return >=0 success, <0 failure
 */
int AppLibVideoEnc_EncodeSetup(void)
{
    //
    // Note: For A9, Bitstream-specific configs can be assigned here (before encode start), or when codec instance creation.
    //       But for A7L/A12 family, it is better to assign them when codec instance creation.
    //
    UINT8 HdrEnable;
    {
        AMP_VIDEOENC_H264_CFG_s *H264Cfg = NULL;
        AMP_VIDEOENC_MJPEG_CFG_s *MjpegCfg = NULL;
        AMP_VIDEOENC_H264_HEADER_INFO_s HeaderInfo;
        AMP_VIDEOENC_BITSTREAM_CFG_s BitsCfg;

        APPLIB_VIDEOENC_GOP_s *VideoEncGOPData = AppLibSysSensor_GetVideoGOP(AppLibVideoEnc_GetSensorVideoRes());
        APPLIB_VIDEOENC_BITRATE_s *VideoEncBitRateData = AppLibSysSensor_GetVideoBitRate(AppLibVideoEnc_GetSensorVideoRes(), AppLibVideoEnc_GetQuality());
        APPLIB_SENSOR_VIDEO_ENC_CONFIG_s *VideoEncConfigData = AppLibSysSensor_GetVideoConfig(AppLibVideoEnc_GetSensorVideoRes());

        /**Enable HDR Hybrid liso mode*/
        if (AppLibVideoEnc_GetSensorVideoRes() == SENSOR_VIDEO_RES_TRUE_HDR_1080P_HALF ||
            AppLibVideoEnc_GetSensorVideoRes() == SENSOR_VIDEO_RES_WQHD_HALF_HDR) {
           HdrEnable = 1;
        } else {
           HdrEnable = 0;
        }

        memset(&HeaderInfo, 0x0, sizeof(AMP_VIDEOENC_H264_HEADER_INFO_s));
        memset(&BitsCfg, 0x0, sizeof(AMP_VIDEOENC_BITSTREAM_CFG_s));
        BitsCfg.Rotation = VencRotation;
        BitsCfg.TimeLapse = (AppLibVideoEnc_GetTimeLapse() == 0) ? 0:1 ;
        BitsCfg.VideoThumbnail = 0;
        /* Assign bitstream - specific configs */
        if (EncPriSpecH264) {
            BitsCfg.StreamSpec = AMP_VIDEOENC_CODER_AVCC;
            H264Cfg = &BitsCfg.Spec.H264Cfg;
            H264Cfg->GopM = VideoEncGOPData->M;
            H264Cfg->GopN = VideoEncGOPData->N;
            H264Cfg->GopIDR = VideoEncGOPData->Idr;
            H264Cfg->GopHierarchical = VideoEncGOPData->Mode;
            H264Cfg->QPControl.QpMinI = 14;
            H264Cfg->QPControl.QpMaxI = 51;
            H264Cfg->QPControl.QpMinP = 17;
            H264Cfg->QPControl.QpMaxP = 51;
            H264Cfg->QPControl.QpMinB = 21;
            H264Cfg->QPControl.QpMaxB = 51;
            H264Cfg->Cabac = 1;
            H264Cfg->QualityControl.LoopFilterEnable = 1;
            H264Cfg->QualityControl.LoopFilterAlpha = 0;
            H264Cfg->QualityControl.LoopFilterBeta = 0;
            H264Cfg->StartFromBFrame = (H264Cfg->GopM > 1);
            H264Cfg->Interlace = 0;
            H264Cfg->AuDelimiterType = 1;
            H264Cfg->QualityLevel = 0; // Suggested value: 1080P: 0x94, 1008i: 0x9B, 720P: 0x9A
            H264Cfg->StopMethod = AMP_VIDEOENC_STOP_NEXT_IP;
            H264Cfg->TimeScale = VideoEncConfigData->EncNumerator;
            H264Cfg->TickPerPicture = VideoEncConfigData->EncDenominator;
            HeaderInfo.GopM = VideoEncGOPData->M;
            HeaderInfo.Width = VideoEncConfigData->EncodeWidth;
            HeaderInfo.Height = VideoEncConfigData->EncodeHeight;
            HeaderInfo.Interlace = 0;
            HeaderInfo.Rotation = VencRotation;

            /* Use default SPS/VUI */
            AmpVideoEnc_GetDefaultH264Header(&HeaderInfo, &H264Cfg->SPS, &H264Cfg->VUI);
            H264Cfg->VUI.video_full_range_flag = 0; //follow old spec.

            /* Bitrate control */
            H264Cfg->BitRateControl.BrcMode = VideoEncBitRateData->Mode;
            H264Cfg->BitRateControl.AverageBitrate = (UINT32)(VideoEncBitRateData->BitRateAvg* 1E6);
            if (VideoEncBitRateData->Mode == VIDEOENC_SMART_VBR) {
                //H264Cfg->BitRateControl.Brc.SmartVbrConfig.MaxBitrate = (UINT32)(VideoEncBitRateData->BitRateAvg * VideoEncBitRateData->BitRateRatioMax * 1E6);
                //H264Cfg->BitRateControl.Brc.SmartVbrConfig.MinBitrate = (UINT32)(VideoEncBitRateData->BitRateAvg * VideoEncBitRateData->BitRateRatioMin * 1E6);
            }
            if (0) {
                AmbaPrint("[Applib - VideoEnc] <EncodeSetup> H264Cfg->BitRateControl.BrcMode = %d",H264Cfg->BitRateControl.BrcMode);
                AmbaPrint("[Applib - VideoEnc] <EncodeSetup> H264Cfg->BitRateControl.AverageBitrate = %d",H264Cfg->BitRateControl.AverageBitrate);
            }
            H264Cfg->QualityControl.IBeatMode = EncIBeat;

        } else {
            BitsCfg.StreamSpec = AMP_VIDEOENC_CODER_MJPEG;
            MjpegCfg = &BitsCfg.Spec.MjpgCfg;

            MjpegCfg->FrameRateDivisionFactor = 1;
            MjpegCfg->QuantMatrixAddr = MjpegQuantMatrix;

        }
        AmpVideoEnc_SetBitstreamConfig(VideoEncPri, &BitsCfg);

        if (AppLibVideoEnc_GetDualStreams()) {
            if (EncSecSpecH264) {
                BitsCfg.StreamSpec = AMP_VIDEOENC_CODER_AVCC;
                H264Cfg = &BitsCfg.Spec.H264Cfg;
                H264Cfg->GopM = SecStreamGopM;
                H264Cfg->GopN = SecStreamGopN;
                H264Cfg->GopIDR = SecStreamGopIDR;
                H264Cfg->GopHierarchical = 0;
                H264Cfg->QPControl.QpMinI = 14;
                H264Cfg->QPControl.QpMaxI = 51;
                H264Cfg->QPControl.QpMinP = 17;
                H264Cfg->QPControl.QpMaxP = 51;
                H264Cfg->QPControl.QpMinB = 21;
                H264Cfg->QPControl.QpMaxB = 51;
                H264Cfg->Cabac = 1;
                H264Cfg->QualityControl.LoopFilterEnable = 1;
                H264Cfg->QualityControl.LoopFilterAlpha = 0;
                H264Cfg->QualityControl.LoopFilterBeta = 0;
                H264Cfg->StartFromBFrame = (H264Cfg->GopM > 1);
                H264Cfg->Interlace = 0;
                H264Cfg->TimeScale = SecStreamTimeScale;
                H264Cfg->TickPerPicture = SecStreamTick;
                H264Cfg->AuDelimiterType = 1;
                H264Cfg->QualityLevel = 0;
                H264Cfg->StopMethod = AMP_VIDEOENC_STOP_NEXT_IP;

                HeaderInfo.GopM = H264Cfg->GopM;
                HeaderInfo.Width = SecStreamWidth;
                HeaderInfo.Height = SecStreamHeight;
                HeaderInfo.Interlace = 0;
                HeaderInfo.Rotation = VencRotation;

                /* Get default SPS/VUI */
                AmpVideoEnc_GetDefaultH264Header(&HeaderInfo, &H264Cfg->SPS, &H264Cfg->VUI);
                H264Cfg->SPS.level_idc = 42;
                H264Cfg->VUI.video_full_range_flag = 0; //follow old spec.

                /* Bitrate control */
                H264Cfg->BitRateControl.BrcMode = VIDEOENC_SMART_VBR;
                H264Cfg->BitRateControl.AverageBitrate = SecStreamBitRate;
                H264Cfg->QualityControl.IBeatMode = EncIBeat;
            } else {
                BitsCfg.StreamSpec = AMP_VIDEOENC_CODER_MJPEG;
                MjpegCfg = &BitsCfg.Spec.MjpgCfg;

                MjpegCfg->FrameRateDivisionFactor = 1;
                MjpegCfg->QuantMatrixAddr = MjpegQuantMatrix;
            }
            AmpVideoEnc_SetBitstreamConfig(VideoEncSec, &BitsCfg);

        }

        // BitRate monitor
        if (ApplibVideoEncVideoSetting.BitRateMonitor == BITRATE_MONITOR_ON) {

            AMBA_IMG_ENC_MONITOR_BRC_HDLR_CFG_s BrcCfg;
            APPLIB_VIDEOENC_BITRATE_s *VideoEncBitRateData = AppLibSysSensor_GetVideoBitRate(AppLibVideoEnc_GetSensorVideoRes(), AppLibVideoEnc_GetQuality());
            AMBA_SENSOR_MODE_INFO_s VinInfo;
            APPLIB_SENSOR_VIN_CONFIG_s VinConfigData;
            AMBA_SENSOR_MODE_ID_u Mode = {0};

            memset(&BrcCfg, 0x0, sizeof(AMBA_IMG_ENC_MONITOR_BRC_HDLR_CFG_s));
            AmbaEncMonitorBRC_GetDefaultCfg(&BrcCfg);
            VinConfigData.ResID = AppLibVideoEnc_GetSensorVideoRes();
            Mode.Data = AppLibSysSensor_GetVinMode(&VinConfigData);

            AmbaSensor_GetModeInfo(AppEncChannel, Mode, &VinInfo);
            BrcCfg.Period = 3000; // 3sec
            BrcCfg.CmplxHdlr.GetDayLumaThresCB = AppLibVideoEnc_GetDayLumaThresholdCB;
            BrcCfg.CmplxHdlr.GetComplexityRangeCB = AppLibVideoEnc_GetSceneComplexityRangeCB;
            BrcCfg.CmplxHdlr.AdjustQpCB = AppLibVideoEnc_QpAdjustmentCB;
            BrcCfg.AverageBitrate = (UINT32)VideoEncBitRateData->BitRateAvg*1000*1000;
            BrcCfg.MaxBitrate = (UINT32)BrcCfg.AverageBitrate*VideoEncBitRateData->BitRateRatioMax;
            BrcCfg.MinBitrate = (UINT32)BrcCfg.AverageBitrate*VideoEncBitRateData->BitRateRatioMin;
            AmbaPrint("[Applib - VideoEnc] <LiveViewSetup> pri AverageBitrate = %d, MaxBitrate = %d, MinBitrate = %d",BrcCfg.AverageBitrate,BrcCfg.MaxBitrate,BrcCfg.MinBitrate);
            BrcCfg.emonStrmHdlr = EncMonitorStrmHdlrPri;
            /**HDR Mode use hybrid low iso*/
            if (HdrEnable) {
               BrcCfg.VideoProcMode = 1;
            } else {
               BrcCfg.VideoProcMode = 0;
            }

            if ((VideoEncConfigData->EncNumerator/VideoEncConfigData->EncDenominator) >= 100 ||BrcCfg.VideoProcMode == 1) {
            /* Display the OverSampling function in the High Frame Rate setting. */
                BrcCfg.VideoOSMode = 0;
            } else if (BrcCfg.VideoProcMode == 0) {
                if (VideoEncConfigData->CaptureWidth > 1920 || \
                    VideoEncConfigData->EncodeWidth > 1920)
                    BrcCfg.VideoOSMode = 1;
            }
            BrcCfg.VideoHdrMode = (VinInfo.HdrInfo.HdrType == AMBA_SENSOR_HDR_TYPE_MULTI_SLICE) ? 1:0;
            /**if monitor is already register, unregister before register with new config*/
            if (BrcHdlrPri != NULL) {
                AmbaEncMonitorBRC_UnRegisterService(BrcHdlrPri);
            }
            AmbaEncMonitorBRC_RegisterService(&BrcCfg, &BrcHdlrPri);

            if (AppLibVideoEnc_GetDualStreams()) {
                BrcCfg.AverageBitrate = (UINT32)SecStreamBitRate;
                BrcCfg.MaxBitrate = (UINT32)SecStreamBitRate*VideoEncBitRateData->BitRateRatioMax;
                BrcCfg.MinBitrate = (UINT32)SecStreamBitRate*VideoEncBitRateData->BitRateRatioMin;
                AmbaPrint("[Applib - VideoEnc] <LiveViewSetup>  sec AverageBitrate = %d, MaxBitrate = %d, MinBitrate = %d",BrcCfg.AverageBitrate,BrcCfg.MaxBitrate,BrcCfg.MinBitrate);
                BrcCfg.emonStrmHdlr = EncMonitorStrmHdlrSec;
                /**if monitor is already register, unregister before register with new config*/
                if (BrcHdlrSec != NULL) {
                    AmbaEncMonitorBRC_UnRegisterService(BrcHdlrSec);
                }
                AmbaEncMonitorBRC_RegisterService(&BrcCfg, &BrcHdlrSec);
            }
        }
        // AQP monitor
        if (ApplibVideoEncVideoSetting.AQPMonitor == AQP_MONITOR_ON) {
            AMBA_IMG_ENC_MONITOR_AQP_HDLR_CFG_s AqpCfg;
            int Res = 0;

            memset(&AqpCfg, 0x0, sizeof(AMBA_IMG_ENC_MONITOR_AQP_HDLR_CFG_s));
            AmbaEncMonitorAQP_GetDefaultCfg(&AqpCfg);

            if (AqpHdlrPri == NULL) {
                AqpCfg.Period = 1000; // 1sec
                AqpCfg.emonStrmHdlr = EncMonitorStrmHdlrPri;
                AqpCfg.AqpCB = AppLibVideoEnc_AqpPriStream;
                Res = AmbaEncMonitorAQP_RegisterService(&AqpCfg, &AqpHdlrPri);
                AmbaPrintColor(RED,"[Applib - VideoEnc] <LiveViewSetup>  Pri AmbaEncMonitorAQP_RegisterService %d",Res);
            }

            if ((AppLibVideoEnc_GetDualStreams()) && AqpHdlrSec == NULL) {
                AqpCfg.AqpCB = AppLibVideoEnc_AqpSecStream;
                AqpCfg.emonStrmHdlr = EncMonitorStrmHdlrSec;
                AmbaEncMonitorAQP_RegisterService(&AqpCfg, &AqpHdlrSec);
                AmbaPrintColor(RED,"[Applib - VideoEnc] <LiveViewSetup>  Sec AmbaEncMonitorAQP_RegisterService %d",Res);
            }
        }


        //
        // Setup AQP
        //
        #if 0
        { // retrieve current QP settings for QP_adjustment
            AMP_VIDEOENC_BITSTREAM_CFG_s CurrentCfg;
            if (EncPriSpecH264) {
                VideoEncPriQpIsZero = 0;
                AmpVideoEnc_GetBitstreamConfig(VideoEncPri, &CurrentCfg);
                VideoEncPriCurrQpMinI = CurrentCfg.Spec.H264Cfg.QPControl.QpMinI;
                VideoEncPriCurrQpMaxI = CurrentCfg.Spec.H264Cfg.QPControl.QpMaxI;
                VideoEncPriCurrQpMinP = CurrentCfg.Spec.H264Cfg.QPControl.QpMinP;
                VideoEncPriCurrQpMaxP = CurrentCfg.Spec.H264Cfg.QPControl.QpMaxP;
                VideoEncPriCurrQpMinB = CurrentCfg.Spec.H264Cfg.QPControl.QpMinB;
                VideoEncPriCurrQpMaxB = CurrentCfg.Spec.H264Cfg.QPControl.QpMaxB;
            }

            if (EncSecSpecH264 && (AppLibVideoEnc_GetDualStreams())) {
                VideoEncSecQpIsZero = 0;
                AmpVideoEnc_GetBitstreamConfig(VideoEncSec, &CurrentCfg);
                VideoEncSecCurrQpMinI = CurrentCfg.Spec.H264Cfg.QPControl.QpMinI;
                VideoEncSecCurrQpMaxI = CurrentCfg.Spec.H264Cfg.QPControl.QpMaxI;
                VideoEncSecCurrQpMinP = CurrentCfg.Spec.H264Cfg.QPControl.QpMinP;
                VideoEncSecCurrQpMaxP = CurrentCfg.Spec.H264Cfg.QPControl.QpMaxP;
                VideoEncSecCurrQpMinB = CurrentCfg.Spec.H264Cfg.QPControl.QpMinB;
                VideoEncSecCurrQpMaxB = CurrentCfg.Spec.H264Cfg.QPControl.QpMaxB;
            }
        }
        #else
        { // retrieve current QP settings for QP_adjustment
            AMP_VIDEOENC_BITSTREAM_CFG_s CurrentCfg;
            if (EncPriSpecH264 && VideoEncPri) {
                VideoEncPriQpIsZero = 0;
                AmpVideoEnc_GetBitstreamConfig(VideoEncPri, &CurrentCfg);
                VideoEncPriCurrQpMinI = CurrentCfg.Spec.H264Cfg.QPControl.QpMinI;
                VideoEncPriCurrQpMaxI = CurrentCfg.Spec.H264Cfg.QPControl.QpMaxI;
                VideoEncPriCurrQpMinP = CurrentCfg.Spec.H264Cfg.QPControl.QpMinP;
                VideoEncPriCurrQpMaxP = CurrentCfg.Spec.H264Cfg.QPControl.QpMaxP;
                VideoEncPriCurrQpMinB = CurrentCfg.Spec.H264Cfg.QPControl.QpMinB;
                VideoEncPriCurrQpMaxB = CurrentCfg.Spec.H264Cfg.QPControl.QpMaxB;
            }

            //if (EncSecSpecH264 && VideoEncSec && (EncDualStream || EncDualHDStream)) {
            if (EncSecSpecH264 && VideoEncSec && (AppLibVideoEnc_GetDualStreams())) {
                VideoEncSecQpIsZero = 0;
                AmpVideoEnc_GetBitstreamConfig(VideoEncSec, &CurrentCfg);
                VideoEncSecCurrQpMinI = CurrentCfg.Spec.H264Cfg.QPControl.QpMinI;
                VideoEncSecCurrQpMaxI = CurrentCfg.Spec.H264Cfg.QPControl.QpMaxI;
                VideoEncSecCurrQpMinP = CurrentCfg.Spec.H264Cfg.QPControl.QpMinP;
                VideoEncSecCurrQpMaxP = CurrentCfg.Spec.H264Cfg.QPControl.QpMaxP;
                VideoEncSecCurrQpMinB = CurrentCfg.Spec.H264Cfg.QPControl.QpMinB;
                VideoEncSecCurrQpMaxB = CurrentCfg.Spec.H264Cfg.QPControl.QpMaxB;
            }
        }

        {  // renew AQP table
            AMP_VIDEOENC_BITSTREAM_CFG_s CurrentCfg;
            if (EncPriSpecH264 && VideoEncPri && AqpHdlrPri) {
                ADJ_AQP_INFO_s AqpInfo = {0};
                AMBA_IMG_ENC_MONITOR_ENCODING_INFO_s EncodeInfo = {0};
                AmpVideoEnc_GetBitstreamConfig(VideoEncPri, &CurrentCfg);

                /* Check ADJ data base first */
                AmbaImg_Proc_Cmd(MW_IP_GET_ADJ_AQP_INFO, 0/*chNo*/, (UINT32)&AqpInfo, 0);

                if (AqpInfo.UpdateFlg) {
                    AMBA_ENCMONITOR_RUNTIME_QUALITY_CFG_s Cfg = {0};

                    Cfg.Cmd |= QC_QMODEL;
                    Cfg.AQPStrength = AqpInfo.AQPParams.Value[0];
                    Cfg.Intra16x16Bias = AqpInfo.AQPParams.Value[1];
                    Cfg.Intra4x4Bias = AqpInfo.AQPParams.Value[2];
                    Cfg.Inter16x16Bias = AqpInfo.AQPParams.Value[3];
                    Cfg.Inter8x8Bias = AqpInfo.AQPParams.Value[4];
                    Cfg.Direct16x16Bias = AqpInfo.AQPParams.Value[5];
                    Cfg.Direct8x8Bias = AqpInfo.AQPParams.Value[6];
                    Cfg.MELambdaQpOffset = AqpInfo.AQPParams.Value[7];
                    Cfg.Alpha = AqpInfo.AQPParams.Value[8];
                    Cfg.Beta = AqpInfo.AQPParams.Value[9];
                    AmbaEncMonitor_SetRuntimeQuality(EncMonitorStrmHdlrPri, &Cfg);

                    AqpInfo.UpdateFlg = 0;
                    AmbaImg_Proc_Cmd(MW_IP_SET_ADJ_AQP_INFO, 0/*chNo*/, (UINT32)&AqpInfo, 0);
                    AmbaEncMonitor_GetCurrentEncodingInfo(EncMonitorStrmHdlrPri, &EncodeInfo);
                    CurrentCfg.Spec.H264Cfg.QualityControl.AutoQpStrength = EncodeInfo.QCfg.QualityModelConfig.AQPStrength;
                    CurrentCfg.Spec.H264Cfg.QualityControl.Intra16x16Bias = EncodeInfo.QCfg.QualityModelConfig.Intra16by16Bias;
                    CurrentCfg.Spec.H264Cfg.QualityControl.Intra4x4Bias = EncodeInfo.QCfg.QualityModelConfig.Intra4by4Bias;
                    CurrentCfg.Spec.H264Cfg.QualityControl.Inter16x16Bias = EncodeInfo.QCfg.QualityModelConfig.Inter16by16Bias;
                    CurrentCfg.Spec.H264Cfg.QualityControl.Inter8x8Bias = EncodeInfo.QCfg.QualityModelConfig.Inter8by8Bias;
                    CurrentCfg.Spec.H264Cfg.QualityControl.Direct16x16Bias = EncodeInfo.QCfg.QualityModelConfig.Direct16by16Bias;
                    CurrentCfg.Spec.H264Cfg.QualityControl.Direct8x8Bias = EncodeInfo.QCfg.QualityModelConfig.Direct8by8Bias;
                    CurrentCfg.Spec.H264Cfg.QualityControl.MELambdaQpOffset = EncodeInfo.QCfg.QualityModelConfig.MeLambdaQPOffset;
                    CurrentCfg.Spec.H264Cfg.QualityControl.LoopFilterAlpha = EncodeInfo.QCfg.QualityModelConfig.Alpha;
                    CurrentCfg.Spec.H264Cfg.QualityControl.LoopFilterBeta = EncodeInfo.QCfg.QualityModelConfig.Beta;
                    AmpVideoEnc_SetBitstreamConfig(VideoEncPri, &CurrentCfg);
                } else {
                    //Using resident data in StrmHdlr
                    AmbaEncMonitor_GetCurrentEncodingInfo(EncMonitorStrmHdlrPri, &EncodeInfo);
                    CurrentCfg.Spec.H264Cfg.QualityControl.AutoQpStrength = EncodeInfo.QCfg.QualityModelConfig.AQPStrength;
                    CurrentCfg.Spec.H264Cfg.QualityControl.Intra16x16Bias = EncodeInfo.QCfg.QualityModelConfig.Intra16by16Bias;
                    CurrentCfg.Spec.H264Cfg.QualityControl.Intra4x4Bias = EncodeInfo.QCfg.QualityModelConfig.Intra4by4Bias;
                    CurrentCfg.Spec.H264Cfg.QualityControl.Inter16x16Bias = EncodeInfo.QCfg.QualityModelConfig.Inter16by16Bias;
                    CurrentCfg.Spec.H264Cfg.QualityControl.Inter8x8Bias = EncodeInfo.QCfg.QualityModelConfig.Inter8by8Bias;
                    CurrentCfg.Spec.H264Cfg.QualityControl.Direct16x16Bias = EncodeInfo.QCfg.QualityModelConfig.Direct16by16Bias;
                    CurrentCfg.Spec.H264Cfg.QualityControl.Direct8x8Bias = EncodeInfo.QCfg.QualityModelConfig.Direct8by8Bias;
                    CurrentCfg.Spec.H264Cfg.QualityControl.MELambdaQpOffset = EncodeInfo.QCfg.QualityModelConfig.MeLambdaQPOffset;
                    CurrentCfg.Spec.H264Cfg.QualityControl.LoopFilterAlpha = EncodeInfo.QCfg.QualityModelConfig.Alpha;
                    CurrentCfg.Spec.H264Cfg.QualityControl.LoopFilterBeta = EncodeInfo.QCfg.QualityModelConfig.Beta;
                    AmpVideoEnc_SetBitstreamConfig(VideoEncPri, &CurrentCfg);
                }
            }

            if (EncSecSpecH264 && VideoEncSec && AqpHdlrSec) {
                ADJ_AQP_INFO_s AqpInfo = {0};
                AMBA_IMG_ENC_MONITOR_ENCODING_INFO_s EncodeInfo = {0};
                AmpVideoEnc_GetBitstreamConfig(VideoEncSec, &CurrentCfg);

                /* Check ADJ data base first */
                AmbaImg_Proc_Cmd(MW_IP_GET_ADJ_AQP_INFO, 0/*chNo*/, (UINT32)&AqpInfo, 0);

                if (AqpInfo.UpdateFlg) {
                    AMBA_ENCMONITOR_RUNTIME_QUALITY_CFG_s Cfg = {0};

                    Cfg.Cmd |= QC_QMODEL;
                    Cfg.AQPStrength = AqpInfo.AQPParams.Value[0];
                    Cfg.Intra16x16Bias = AqpInfo.AQPParams.Value[1];
                    Cfg.Intra4x4Bias = AqpInfo.AQPParams.Value[2];
                    Cfg.Inter16x16Bias = AqpInfo.AQPParams.Value[3];
                    Cfg.Inter8x8Bias = AqpInfo.AQPParams.Value[4];
                    Cfg.Direct16x16Bias = AqpInfo.AQPParams.Value[5];
                    Cfg.Direct8x8Bias = AqpInfo.AQPParams.Value[6];
                    Cfg.MELambdaQpOffset = AqpInfo.AQPParams.Value[7];
                    Cfg.Alpha = AqpInfo.AQPParams.Value[8];
                    Cfg.Beta = AqpInfo.AQPParams.Value[9];
                    AmbaEncMonitor_SetRuntimeQuality(EncMonitorStrmHdlrSec, &Cfg);

                    AqpInfo.UpdateFlg = 0;
                    AmbaImg_Proc_Cmd(MW_IP_SET_ADJ_AQP_INFO, 0/*chNo*/, (UINT32)&AqpInfo, 0);
                    AmbaEncMonitor_GetCurrentEncodingInfo(EncMonitorStrmHdlrSec, &EncodeInfo);
                    CurrentCfg.Spec.H264Cfg.QualityControl.AutoQpStrength = EncodeInfo.QCfg.QualityModelConfig.AQPStrength;
                    CurrentCfg.Spec.H264Cfg.QualityControl.Intra16x16Bias = EncodeInfo.QCfg.QualityModelConfig.Intra16by16Bias;
                    CurrentCfg.Spec.H264Cfg.QualityControl.Intra4x4Bias = EncodeInfo.QCfg.QualityModelConfig.Intra4by4Bias;
                    CurrentCfg.Spec.H264Cfg.QualityControl.Inter16x16Bias = EncodeInfo.QCfg.QualityModelConfig.Inter16by16Bias;
                    CurrentCfg.Spec.H264Cfg.QualityControl.Inter8x8Bias = EncodeInfo.QCfg.QualityModelConfig.Inter8by8Bias;
                    CurrentCfg.Spec.H264Cfg.QualityControl.Direct16x16Bias = EncodeInfo.QCfg.QualityModelConfig.Direct16by16Bias;
                    CurrentCfg.Spec.H264Cfg.QualityControl.Direct8x8Bias = EncodeInfo.QCfg.QualityModelConfig.Direct8by8Bias;
                    CurrentCfg.Spec.H264Cfg.QualityControl.MELambdaQpOffset = EncodeInfo.QCfg.QualityModelConfig.MeLambdaQPOffset;
                    CurrentCfg.Spec.H264Cfg.QualityControl.LoopFilterAlpha = EncodeInfo.QCfg.QualityModelConfig.Alpha;
                    CurrentCfg.Spec.H264Cfg.QualityControl.LoopFilterBeta = EncodeInfo.QCfg.QualityModelConfig.Beta;
                    AmpVideoEnc_SetBitstreamConfig(VideoEncSec, &CurrentCfg);
                } else {
                    //Using resident data in StrmHdlr
                    AmbaEncMonitor_GetCurrentEncodingInfo(EncMonitorStrmHdlrSec, &EncodeInfo);
                    CurrentCfg.Spec.H264Cfg.QualityControl.AutoQpStrength = EncodeInfo.QCfg.QualityModelConfig.AQPStrength;
                    CurrentCfg.Spec.H264Cfg.QualityControl.Intra16x16Bias = EncodeInfo.QCfg.QualityModelConfig.Intra16by16Bias;
                    CurrentCfg.Spec.H264Cfg.QualityControl.Intra4x4Bias = EncodeInfo.QCfg.QualityModelConfig.Intra4by4Bias;
                    CurrentCfg.Spec.H264Cfg.QualityControl.Inter16x16Bias = EncodeInfo.QCfg.QualityModelConfig.Inter16by16Bias;
                    CurrentCfg.Spec.H264Cfg.QualityControl.Inter8x8Bias = EncodeInfo.QCfg.QualityModelConfig.Inter8by8Bias;
                    CurrentCfg.Spec.H264Cfg.QualityControl.Direct16x16Bias = EncodeInfo.QCfg.QualityModelConfig.Direct16by16Bias;
                    CurrentCfg.Spec.H264Cfg.QualityControl.Direct8x8Bias = EncodeInfo.QCfg.QualityModelConfig.Direct8by8Bias;
                    CurrentCfg.Spec.H264Cfg.QualityControl.MELambdaQpOffset = EncodeInfo.QCfg.QualityModelConfig.MeLambdaQPOffset;
                    CurrentCfg.Spec.H264Cfg.QualityControl.LoopFilterAlpha = EncodeInfo.QCfg.QualityModelConfig.Alpha;
                    CurrentCfg.Spec.H264Cfg.QualityControl.LoopFilterBeta = EncodeInfo.QCfg.QualityModelConfig.Beta;
                    AmpVideoEnc_SetBitstreamConfig(VideoEncSec, &CurrentCfg);
                }
            }
        }
        #endif




        //
        // Setup bitstream buffer.
        //
        // Rule: H.264 and MJPEG can't use the same bitstream/descriptor buffer. Same Spec uses the same buffer. No matter it is primary or secondary
        // Note: Since buffer allocation depends on the above rule, it is better to assign bitstream buffer before encode start.
        //       Otherwise you have to know what you are going to encode when codec instance creation
        {
            AMP_ENC_BITSBUFFER_CFG_s  BitsBufCfg = {0};
            UINT32 VidencBitsFifoSize = 0;
            UINT32 VidencDescSize = 0;
            UINT32 BufThreshold = AppLibVideoEnc_CalBitsBufThreshold();
#if APPLIB_RECORDER_MEMMGR
            AppLibRecorderMemMgr_GetBufSize(&VidencBitsFifoSize, &VidencDescSize);
#else
            VidencBitsFifoSize =  VIDENC_BITSFIFO_SIZE;
            VidencDescSize =  VIDENC_DESC_SIZE;
#endif
            if (EncPriSpecH264) {
                BitsBufCfg.BitsBufAddr = H264EncBitsBuf;
                BitsBufCfg.BitsBufSize = VidencBitsFifoSize;
                BitsBufCfg.DescBufAddr = H264DescBuf;
                BitsBufCfg.DescBufSize = VidencDescSize;
                BitsBufCfg.BitsRunoutThreshold = VidencBitsFifoSize - BufThreshold*1024*1024; /**< leave 4MB */
            } else {
                BitsBufCfg.BitsBufAddr = MjpgEncBitsBuf;
                BitsBufCfg.BitsBufSize = VidencBitsFifoSize;
                BitsBufCfg.DescBufAddr = MjpgDescBuf;
                BitsBufCfg.DescBufSize = VidencDescSize;
                BitsBufCfg.BitsRunoutThreshold = VidencBitsFifoSize - BufThreshold*1024*1024; /**< leave 4MB */
            }
            AmpVideoEnc_SetBitstreamBuffer(VideoEncPri, &BitsBufCfg);

            if (AppLibVideoEnc_GetDualStreams()) {
                if (EncSecSpecH264) {
                    BitsBufCfg.BitsBufAddr = H264EncBitsBuf;
                    BitsBufCfg.BitsBufSize = VidencBitsFifoSize;
                    BitsBufCfg.DescBufAddr = H264DescBuf;
                    BitsBufCfg.DescBufSize = VidencDescSize;
                    BitsBufCfg.BitsRunoutThreshold = VidencBitsFifoSize - BufThreshold*1024*1024; /**< leave 4MB */
                } else {
                    BitsBufCfg.BitsBufAddr = MjpgEncBitsBuf;
                    BitsBufCfg.BitsBufSize = VidencBitsFifoSize;
                    BitsBufCfg.DescBufAddr = MjpgDescBuf;
                    BitsBufCfg.DescBufSize = VidencDescSize;
                    BitsBufCfg.BitsRunoutThreshold = VidencBitsFifoSize - BufThreshold*1024*1024; /**< leave 4MB */
                }
                AmpVideoEnc_SetBitstreamBuffer(VideoEncSec, &BitsBufCfg);
            }

            #if APPLIB_RECORDER_MEMMGR /* CONFIG_APP_ARD */
            if (EncSecSpecH264) {
                AmbaPrint("[Applib - VideoEnc] <EncodeSetup> H.264 Bits 0x%x size %d Desc 0x%x size %d", H264EncBitsBuf, VidencBitsFifoSize, H264DescBuf, VidencDescSize);
            } else {
                AmbaPrint("[Applib - VideoEnc] <EncodeSetup> MJPEG Bits 0x%x size %d Desc 0x%x size %d", MjpgEncBitsBuf, VidencBitsFifoSize, MjpgDescBuf, VidencDescSize);
            }
            #else
            if (EncSecSpecH264) {
                AmbaPrint("[Applib - VideoEnc] <EncodeSetup> H.264 Bits 0x%x size %d Desc 0x%x size %d", H264EncBitsBuf, VIDENC_BITSFIFO_SIZE, H264DescBuf, VIDENC_DESC_SIZE);
            } else {
                AmbaPrint("[Applib - VideoEnc] <EncodeSetup> MJPEG Bits 0x%x size %d Desc 0x%x size %d", MjpgEncBitsBuf, VIDENC_BITSFIFO_SIZE, MjpgDescBuf, VIDENC_DESC_SIZE);
            }
            #endif
        }
    }
    return 0;
}

/**
 *  @brief Start to encode
 *
 *  Start to encode
 *
 *  @return >=0 success, <0 failure
 */
int AppLibVideoEnc_EncodeStart(void)
{
    int ReturnValue = 0;

    #if 0
    /**enable encode monitor before encode start*/
    if (ApplibVideoEncVideoSetting.BitRateMonitor == BITRATE_MONITOR_ON) {
        AmbaEncMonitor_EnableStreamHandler(EncMonitorStrmHdlrPri, 1);
        AmbaEncMonitorBRC_EnableService(BrcHdlrPri, 1);
        if (AppLibVideoEnc_GetDualStreams()) {
            AmbaEncMonitor_EnableStreamHandler(EncMonitorStrmHdlrSec, 1);
            AmbaEncMonitorBRC_EnableService(BrcHdlrSec, 1);
        }
    }
    #else
    /**enable encode monitor before encode start*/
    if (ApplibVideoEncVideoSetting.BitRateMonitor == BITRATE_MONITOR_ON || ApplibVideoEncVideoSetting.AQPMonitor == AQP_MONITOR_ON) {
        AmbaEncMonitor_EnableStreamHandler(EncMonitorStrmHdlrPri, 1);

        //BitRate
        if (ApplibVideoEncVideoSetting.BitRateMonitor == BITRATE_MONITOR_ON){
            AmbaEncMonitorBRC_EnableService(BrcHdlrPri, 1);
        }
        //AQP
        if (ApplibVideoEncVideoSetting.AQPMonitor == AQP_MONITOR_ON) {
            AmbaEncMonitorAQP_EnableService(AqpHdlrPri, 1);
        }

        if (AppLibVideoEnc_GetDualStreams()) {
            AmbaEncMonitor_EnableStreamHandler(EncMonitorStrmHdlrSec, 1);

            //BitRate
            if (ApplibVideoEncVideoSetting.BitRateMonitor == BITRATE_MONITOR_ON){
                AmbaEncMonitorBRC_EnableService(BrcHdlrSec, 1);
            }
            //AQP
            if (ApplibVideoEncVideoSetting.AQPMonitor == AQP_MONITOR_ON){
                AmbaEncMonitorAQP_EnableService(AqpHdlrSec, 1);
            }
        }
    }
    #endif

    ReturnValue = AmpEnc_StartRecord(VideoEncPipe, 0);

    return ReturnValue;
}

/**
 *  @brief Pause encoding
 *
 *  Pause encoding
 *
 *  @return >=0 success, <0 failure
 */
int AppLibVideoEnc_EncodePause(void)
{
    int ReturnValue = 0;

    ReturnValue = AmpEnc_PauseRecord(VideoEncPipe, 0);
    #if 0
    /**stop encode monitor after pause record*/
    if (ApplibVideoEncVideoSetting.BitRateMonitor == BITRATE_MONITOR_ON) {
        AmbaEncMonitorBRC_EnableService(BrcHdlrPri, 0);
        if (AppLibVideoEnc_GetDualStreams()) {
            AmbaEncMonitorBRC_EnableService(BrcHdlrSec, 0);
        }
    }
    #else
    /**enable encode monitor before encode start*/
    if (ApplibVideoEncVideoSetting.BitRateMonitor == BITRATE_MONITOR_ON || ApplibVideoEncVideoSetting.AQPMonitor == AQP_MONITOR_ON) {

        //BitRate
        if (ApplibVideoEncVideoSetting.BitRateMonitor == BITRATE_MONITOR_ON){
            AmbaEncMonitorBRC_EnableService(BrcHdlrPri, 0);
        }
        //AQP
        if (ApplibVideoEncVideoSetting.AQPMonitor == AQP_MONITOR_ON) {
            AmbaEncMonitorAQP_EnableService(AqpHdlrPri, 0);
        }

        if (AppLibVideoEnc_GetDualStreams()) {

            //BitRate
            if (ApplibVideoEncVideoSetting.BitRateMonitor == BITRATE_MONITOR_ON){
                AmbaEncMonitorBRC_EnableService(BrcHdlrSec, 0);
            }
            //AQP
            if (ApplibVideoEncVideoSetting.AQPMonitor == AQP_MONITOR_ON){
                AmbaEncMonitorAQP_EnableService(AqpHdlrSec, 0);
            }
        }
    }

    #endif

    return ReturnValue;
}

/**
 *  @brief Resume encoding
 *
 *  Resume encoding
 *
 *  @return >=0 success, <0 failure
 */
int AppLibVideoEnc_EncodeResume(void)
{
    int ReturnValue = 0;

    ReturnValue = AmpEnc_ResumeRecord(VideoEncPipe, 0);
    #if 0
    /**enable encode monitor after encode resume*/
    if (ApplibVideoEncVideoSetting.BitRateMonitor == BITRATE_MONITOR_ON) {
        AmbaEncMonitorBRC_EnableService(BrcHdlrPri, 1);
        if (AppLibVideoEnc_GetDualStreams()) {
            AmbaEncMonitorBRC_EnableService(BrcHdlrSec, 1);
        }
    }
    #else
    if (ApplibVideoEncVideoSetting.BitRateMonitor == BITRATE_MONITOR_ON || ApplibVideoEncVideoSetting.AQPMonitor == AQP_MONITOR_ON) {

        //BitRate
        if (ApplibVideoEncVideoSetting.BitRateMonitor == BITRATE_MONITOR_ON){
            AmbaEncMonitorBRC_EnableService(BrcHdlrPri, 1);
        }
        //AQP
        if (ApplibVideoEncVideoSetting.AQPMonitor == AQP_MONITOR_ON) {
            AmbaEncMonitorAQP_EnableService(AqpHdlrPri, 1);
        }

        if (AppLibVideoEnc_GetDualStreams()) {

            //BitRate
            if (ApplibVideoEncVideoSetting.BitRateMonitor == BITRATE_MONITOR_ON){
                AmbaEncMonitorBRC_EnableService(BrcHdlrSec, 1);
            }
            //AQP
            if (ApplibVideoEncVideoSetting.AQPMonitor == AQP_MONITOR_ON){
                AmbaEncMonitorAQP_EnableService(AqpHdlrSec, 1);
            }
        }
    }

    #endif

    return ReturnValue;
}

/**
 *  @brief Stop encoding.
 *
 *  Stop encoding.
 *
 *  @return >=0 success, <0 failure
 */
int AppLibVideoEnc_EncodeStop(void)
{
    int ReturnValue = 0;

    ReturnValue = AmpEnc_StopRecord(VideoEncPipe, AMP_ENC_FUNC_FLAG_WAIT);

    #if 0
    /**disable encode monitor after encode stop*/
    if (ApplibVideoEncVideoSetting.BitRateMonitor == BITRATE_MONITOR_ON) {
        AmbaEncMonitor_EnableStreamHandler(EncMonitorStrmHdlrPri, 0);
        AmbaEncMonitorBRC_EnableService(BrcHdlrPri, 0);
        if (AppLibVideoEnc_GetDualStreams()) {
            AmbaEncMonitor_EnableStreamHandler(EncMonitorStrmHdlrSec, 0);
            AmbaEncMonitorBRC_EnableService(BrcHdlrSec, 0);
        }
    }
    #else
    if (ApplibVideoEncVideoSetting.BitRateMonitor == BITRATE_MONITOR_ON || ApplibVideoEncVideoSetting.AQPMonitor == AQP_MONITOR_ON) {
        AmbaEncMonitor_EnableStreamHandler(EncMonitorStrmHdlrPri, 0);

        //BitRate
        if (ApplibVideoEncVideoSetting.BitRateMonitor == BITRATE_MONITOR_ON){
            AmbaEncMonitorBRC_EnableService(BrcHdlrPri, 0);
        }
        //AQP
        if (ApplibVideoEncVideoSetting.AQPMonitor == AQP_MONITOR_ON) {
            AmbaEncMonitorAQP_EnableService(AqpHdlrPri, 0);
        }

        if (AppLibVideoEnc_GetDualStreams()) {
            AmbaEncMonitor_EnableStreamHandler(EncMonitorStrmHdlrSec, 0);

            //BitRate
            if (ApplibVideoEncVideoSetting.BitRateMonitor == BITRATE_MONITOR_ON){
                AmbaEncMonitorBRC_EnableService(BrcHdlrSec, 0);
            }
            //AQP
            if (ApplibVideoEncVideoSetting.AQPMonitor == AQP_MONITOR_ON){
                AmbaEncMonitorAQP_EnableService(AqpHdlrSec, 0);
            }
        }
    }

    #endif

    return ReturnValue;
}

/**
 *  @brief Time Lapse encoding.
 *
 *  Time Lapse encoding.
 *
 *  @return >=0 success, <0 failure
 */
int AppLibVideoEnc_EncodeTimeLapse(void)
{
    int ReturnValue = 0;

    ReturnValue = AmpVideoEnc_CaptureTimeLapsedFrame(AppVinA);

    return ReturnValue;
}

/**
 *  @brief Stamp encoding.
 *
 *  Stamp encoding.
 *
 *  @return >=0 success, <0 failure
 */
int AppLibVideoEnc_EncodeStamp(UINT8 encodeStreamId, AMP_VIDEOENC_BLEND_INFO_s *blendInfo)
{
    if (encodeStreamId == 0) {
        AmpVideoEnc_SetEncodeBlend(VideoEncPri, blendInfo);
    }
    if (encodeStreamId == 1) {
        AmpVideoEnc_SetEncodeBlend(VideoEncSec, blendInfo);
    }

    return 0;
}

/**
 *  To free the QV buffer after capture done
 *
 *  @return >=0 success, <0 failure
 */
int AppLibVideoEnc_PIVFreeBuf(void)
{
    int ReturnValue = 0;

    if (PIVCapQvLCDBuffAddrBufRaw) {
        if (AmbaKAL_BytePoolFree((void *)PIVCapQvLCDBuffAddrBufRaw) != OK)
            AmbaPrintColor(RED, "[Applib - StillEnc] <BurstCapture> Post-CallBack: MemFree Fail QvLCDBuff!");
        PIVCapQvLCDBuffAddrBufRaw = NULL;
    }
    if (PIVCapQvHDMIBuffAddrBufRaw) {
        if (AmbaKAL_BytePoolFree((void *)PIVCapQvHDMIBuffAddrBufRaw) != OK)
            AmbaPrintColor(RED, "[Applib - StillEnc] <BurstCapture> Post-CallBack: MemFree Fail QvHDMI!");
        PIVCapQvHDMIBuffAddrBufRaw = NULL;
    }
     return ReturnValue;
}



/**
 * @brief simple PIV(ie rawcap from mem + raw2yuv + yuv2jpeg)
 *
 * @param [in] pivCtrl PIV control information
 * @param [in] iso iso mode
 * @param [in] cmpr compressed raw or not
 * @param [in] targetSize jpeg target Size in Kbyte unit
 * @param [in] encodeloop re-encode number
 *
 * @return 0 - success, -1 - fail
 */
int AppLibVideoEnc_PIV(AMP_VIDEOENC_PIV_CTRL_s pivCtrl, UINT32 iso, UINT32 cmpr, UINT32 targetSize, UINT8 encodeLoop)
{
    int Er = 0;
    UINT16 RawPitch = 0, RawWidth = 0, RawHeight = 0;
    UINT16 YuvWidth = 0, YuvHeight = 0, ScrnW = 0, ScrnH = 0, ThmW = 0, ThmH = 0;
    UINT32 RawSize = 0, YuvSize = 0, ScrnSize = 0, ThmSize = 0;
    UINT32 QvLCDSize = 0, QvHDMISize = 0;
    UINT8 *StageAddr = NULL;
    UINT32 TotalScriptSize = 0, TotalStageNum = 0;
    AMP_SENC_SCRPT_GENCFG_s *GenScrpt;
    AMP_SENC_SCRPT_RAWCAP_s *RawCapScrpt;
    AMP_SENC_SCRPT_RAW2YUV_s *Raw2YvuScrpt;
    AMP_SENC_SCRPT_YUV2JPG_s *Yuv2JpgScrpt;
    AMP_SCRPT_CONFIG_s Scrpt;
    UINT16 ScrnWidth, ScrnHeight, ThmWidth, ThmHeight;
    UINT16 ScrnWidthAct, ScrnHeightAct, ThmWidthAct, ThmHeightAct;
    UINT16 PivCaptureWidth,PivCaptureHeight,PivMainWidth,PivMainHeight;
    UINT16 QvLCDW,QvLCDH,QvHDMIW,QvHDMIH;
    UINT8 DspBackgroundProcMode = AmpResource_GetDspBackgroundProcMode();

    AMBA_SENSOR_MODE_ID_u Mode = {0};
    APPLIB_SENSOR_PIV_ENC_CONFIG_s PIVCapConfig;
    APPLIB_VOUT_PREVIEW_PARAM_s PreviewParam={0};
    UINT32 QvDchanWidth = 0;
    UINT32 QvDchanHeight = 0;
    UINT32 QvFchanWidth = 0;
    UINT32 QvFchanHeight = 0;
    APPLIB_SENSOR_STILLCAP_MODE_CONFIG_s *pMode = NULL;

    G_iso = iso;

    /* fill script and run */
    /* Get sensor mode.*/
    pMode = AppLibSysSensor_GetStillCaptureModeConfig(AppLibStillEnc_GetPhotoPjpegCapMode(), AppLibStillEnc_GetPhotoPjpegConfigId());
    Mode.Data = pMode->ModeId;
    AppLibSysSensor_GetPIVSize(AppLibVideoEnc_GetSensorVideoRes(), &PIVCapConfig);

    /* Set the size of Fchan preview window.*/
    PreviewParam.AspectRatio = AppLibSysSensor_GetCaptureModeAR(AppLibStillEnc_GetPhotoPjpegCapMode(), AppLibStillEnc_GetPhotoPjpegConfigId());
    PreviewParam.ChanID = DISP_CH_FCHAN;
    AppLibDisp_CalcPreviewWindowSize(&PreviewParam);
    QvFchanWidth = PreviewParam.Preview.Width;
    QvFchanHeight = PreviewParam.Preview.Height;

    /* Set the size of Dchan preview window.*/
    PreviewParam.ChanID = DISP_CH_DCHAN;
    AppLibDisp_CalcPreviewWindowSize(&PreviewParam);
    QvDchanWidth = PreviewParam.Preview.Width;
    QvDchanHeight = PreviewParam.Preview.Height;

    /*fill the capture size */
    PivCaptureWidth = pivCtrl.CaptureWidth;
    PivCaptureHeight = pivCtrl.CaptureHeight;
    PivMainWidth = pivCtrl.MainWidth;
    PivMainHeight = pivCtrl.MainHeight;
    ScrnWidth = PIVCapConfig.ScreennailWidth;
    ScrnHeight = PIVCapConfig.ScreennailHeight;
    ThmWidth = PIVCapConfig.ThumbnailWidth;
    ThmHeight = PIVCapConfig.ThumbnailHeight;
    ScrnWidthAct = PIVCapConfig.ScreennailActiveWidth;
    ScrnHeightAct = PIVCapConfig.ScreennailActiveHeight;
    ThmWidthAct = PIVCapConfig.ThumbnailActiveWidth;
    ThmHeightAct = PIVCapConfig.ThumbnailActiveHeight;
    QvLCDW = ALIGN_32(QvDchanWidth);
    QvLCDH = QvDchanHeight;
    QvHDMIW = ALIGN_32(QvFchanWidth);
    QvHDMIH = QvFchanHeight;

    /* Step1. calc raw and yuv buffer memory */
    RawPitch = (cmpr)? \
        AMP_COMPRESSED_RAW_WIDTH(PivCaptureWidth): \
        PivCaptureWidth*2;
    RawPitch = ALIGN_32(RawPitch);
    RawWidth =  PivCaptureWidth;
    RawHeight = PivCaptureHeight;
    RawSize = RawPitch*RawHeight;
    AmbaPrint("[Applib - VideoEnc]raw(%u %u %u)", RawPitch, RawWidth, RawHeight);

    //FastMode need 16_align enc_height
    if (G_iso == 2) {
        //DSP lib need 32ALign for Width and 16_Align for height in buffer allocation
        YuvWidth = ALIGN_32(PivMainWidth);
        YuvHeight = PivMainHeight;
        YuvSize = YuvWidth*YuvHeight*2;
        AmbaPrint("[Applib - VideoEnc]yuv(%u %u %u)!", YuvWidth, YuvHeight, YuvSize);
        ScrnW = ALIGN_32(ScrnWidth);
        ScrnH = ScrnHeight;
        ScrnSize = ScrnW*ScrnH*2;
        ThmW = ALIGN_32(ThmWidth);
        ThmH = ThmHeight;
        ThmSize = ThmW*ThmH*2;
        AmbaPrint("[Applib - VideoEnc]scrn(%d %d %u) thm(%d %d %u)!", \
            ScrnW, ScrnH, ScrnSize, \
            ThmW, ThmH, ThmSize);
    } else {
        //DSP lib need 32ALign for Width and 16_Align for height in buffer allocation
        YuvWidth = ALIGN_32(PivMainWidth);
        YuvHeight = PivMainHeight;
        YuvSize = YuvWidth*YuvHeight*2;
        YuvSize += (YuvSize*10)/100;
        AmbaPrint("[Applib - VideoEnc]yuv(%u %u %u)!", YuvWidth, YuvHeight, YuvSize);
        ScrnW = ALIGN_32(ScrnWidth);
        ScrnH = ScrnHeight;
        ScrnSize = ScrnW*ScrnH*2;
        ScrnSize += (ScrnSize*10)/100;
        ThmW = ALIGN_32(ThmWidth);
        ThmH = ThmHeight;
        ThmSize = ThmW*ThmH*2;
        ThmSize += (ThmSize*10)/100;
        AmbaPrint("[Applib - VideoEnc]scrn(%d %d %u) thm(%d %d %u)!", \
            ScrnW, ScrnH, ScrnSize, \
            ThmW, ThmH, ThmSize);
    }
    /* QV need 16_Align */
    QvLCDSize = QvLCDW*QvLCDH*2;
    QvLCDSize += (QvLCDSize*15)/100;
    QvHDMISize = QvHDMIW*QvHDMIH*2;
    QvHDMISize += (QvHDMISize*15)/100;

    AmbaPrint("[Applib - VideoEnc]qvLCD(%u) qvHDMI(%u)!", QvLCDSize, QvHDMISize);

    /* Step2. allocate raw and yuv/scrn/thm buffer address, script address */
    //In Mode_0 PIV, APP do not need to provide buffer memory
    PIVCapRawBuffAddr = 0;
    PIVCapYuvBuffAddr = 0;
    PIVCapScrnBuffAddr = 0;
    PIVCapThmBuffAddr = 0;


    Er = AmpUtil_GetAlignedPool(APPLIB_G_MMPL, (void **)&PIVCapQvLCDBuffAddr, (void **)&PIVCapQvLCDBuffAddrBufRaw, QvLCDSize*1, 32);
    if (Er != OK) {
        AmbaPrint("[Applib - VideoEnc]NC_DDR alloc yuv_lcd fail (%u)!", QvLCDSize*1);
    } else {
        AmbaPrint("[Applib - VideoEnc]qvLCDBuffaddr (0x%08X)!", PIVCapQvLCDBuffAddr);
    }

    Er = AmpUtil_GetAlignedPool(APPLIB_G_MMPL, (void **)&PIVCapQvHDMIBuffAddr, (void **)&PIVCapQvHDMIBuffAddrBufRaw, QvHDMISize*1, 32);
    if (Er != OK) {
        AmbaPrint("[Applib - VideoEnc]NC_DDR alloc yuv_hdmi fail (%u)!", QvHDMISize*1);
    } else {
        AmbaPrint("[Applib - VideoEnc]qvHDMIBuffaddr (0x%08X)!", PIVCapQvHDMIBuffAddr);
    }

    Er = AmpUtil_GetAlignedPool(APPLIB_G_MMPL, (void **)&PIVCapScriptAddr, (void **)&PIVCapScriptAddrBufRaw, 128*10, 32); //TBD
    if (Er != OK) {
        AmbaPrint("[Applib - VideoEnc]NC_DDR alloc scriptAddr fail (%u)!", 128*10);
    } else {
        AmbaPrint("[Applib - VideoEnc]scriptAddr (0x%08X) (%d)!", PIVCapScriptAddr, 128*10);
    }

    /* Step3. fill script */
    //general config
    StageAddr = PIVCapScriptAddr;
    GenScrpt = (AMP_SENC_SCRPT_GENCFG_s *)StageAddr;
    memset(GenScrpt, 0x0, sizeof(AMP_SENC_SCRPT_GENCFG_s));
    GenScrpt->Cmd = SENC_GENCFG;
    GenScrpt->RawEncRepeat = 0;
    GenScrpt->RawToCap = 1;
    GenScrpt->StillProcMode = G_iso;

    if (DspBackgroundProcMode) {
        GenScrpt->QVConfig.DisableLCDQV = 1;
        GenScrpt->QVConfig.DisableHDMIQV = 1;
    } else {
        //Mode_0 need QV working buffer
        GenScrpt->QVConfig.DisableLCDQV = 1;
        GenScrpt->QVConfig.DisableHDMIQV = 1;
    }


    GenScrpt->QVConfig.LCDDataFormat = AMP_YUV_422;
    GenScrpt->QVConfig.LCDLumaAddr = PIVCapQvLCDBuffAddr;
    GenScrpt->QVConfig.LCDChromaAddr = PIVCapQvLCDBuffAddr + QvLCDSize/2;
    GenScrpt->QVConfig.LCDWidth = QvLCDW;
    GenScrpt->QVConfig.LCDHeight = QvLCDH;
    GenScrpt->QVConfig.HDMIDataFormat = AMP_YUV_422;
    GenScrpt->QVConfig.HDMILumaAddr = PIVCapQvHDMIBuffAddr;
    GenScrpt->QVConfig.HDMIChromaAddr = PIVCapQvHDMIBuffAddr + QvHDMISize/2;
    GenScrpt->QVConfig.HDMIWidth = QvHDMIW;
    GenScrpt->QVConfig.HDMIHeight = QvHDMIH;

    GenScrpt->b2LVCfg = AMP_ENC_SCRPT_B2LV_NONE;
    GenScrpt->ScrnEnable = 1;
    GenScrpt->ThmEnable = 1;

    GenScrpt->MainBuf.ColorFmt = AMP_YUV_420;
    GenScrpt->MainBuf.Width = GenScrpt->MainBuf.Pitch = YuvWidth;
    GenScrpt->MainBuf.Height = YuvHeight;
    GenScrpt->MainBuf.LumaAddr = PIVCapYuvBuffAddr;
    GenScrpt->MainBuf.ChromaAddr = 0; // Behind Luma
    GenScrpt->MainBuf.AOI.X = 0;
    GenScrpt->MainBuf.AOI.Y = 0;
    GenScrpt->MainBuf.AOI.Width = PivMainWidth;
    GenScrpt->MainBuf.AOI.Height = PivMainHeight;

    GenScrpt->ScrnBuf.ColorFmt = AMP_YUV_420;
    GenScrpt->ScrnBuf.Width = GenScrpt->ScrnBuf.Pitch = ScrnW;
    GenScrpt->ScrnBuf.Height = ScrnH;
    GenScrpt->ScrnBuf.LumaAddr = PIVCapScrnBuffAddr;
    GenScrpt->ScrnBuf.ChromaAddr = 0; // Behind Luma
    GenScrpt->ScrnBuf.AOI.X = 0;
    GenScrpt->ScrnBuf.AOI.Y = 0;
    GenScrpt->ScrnBuf.AOI.Height = ScrnHeightAct;
    GenScrpt->ScrnBuf.AOI.Width = ScrnWidthAct;
    GenScrpt->ScrnWidth = ScrnWidth;
    GenScrpt->ScrnHeight = ScrnHeight;

    GenScrpt->ThmBuf.ColorFmt = AMP_YUV_420;
    GenScrpt->ThmBuf.Width = GenScrpt->ThmBuf.Pitch = ThmW;
    GenScrpt->ThmBuf.Height = ThmH;
    GenScrpt->ThmBuf.LumaAddr = PIVCapThmBuffAddr;
    GenScrpt->ThmBuf.ChromaAddr = 0; // Behind Luma
    GenScrpt->ThmBuf.AOI.X = 0;
    GenScrpt->ThmBuf.AOI.Y = 0;
    GenScrpt->ThmBuf.AOI.Width = ThmWidthAct;
    GenScrpt->ThmBuf.AOI.Height = ThmHeightAct;
    GenScrpt->ThmWidth = ThmWidth;
    GenScrpt->ThmHeight = ThmHeight;

    if (targetSize) {
        AmbaPrint("[Applib - VideoEnc]Target Size %u Kbyte", targetSize);
        AppLibStillEnc_initJpegDqt(ApplibJpegQTable[0], -1);
        AppLibStillEnc_initJpegDqt(ApplibJpegQTable[1], -1);
        AppLibStillEnc_initJpegDqt(ApplibJpegQTable[2], -1);
        GenScrpt->BrcCtrl.Tolerance = 10;
        GenScrpt->BrcCtrl.MaxEncLoop = encodeLoop;
        GenScrpt->BrcCtrl.JpgBrcCB = AppLibStillEnc_jpegBRCPredictCB;
        GenScrpt->BrcCtrl.TargetBitRate = \
           (((targetSize<<13)/PivMainWidth)<<12)/PivMainHeight;
    } else {
        AppLibStillEnc_initJpegDqt(ApplibJpegQTable[0], 95);
        AppLibStillEnc_initJpegDqt(ApplibJpegQTable[1], 95);
        AppLibStillEnc_initJpegDqt(ApplibJpegQTable[2], 95);
        GenScrpt->BrcCtrl.Tolerance = 0;
        GenScrpt->BrcCtrl.MaxEncLoop = 0;
        GenScrpt->BrcCtrl.JpgBrcCB = NULL;
        GenScrpt->BrcCtrl.TargetBitRate = 0;
    }
    GenScrpt->BrcCtrl.MainQTAddr = ApplibJpegQTable[0];
    GenScrpt->BrcCtrl.ThmQTAddr = ApplibJpegQTable[1];
    GenScrpt->BrcCtrl.ScrnQTAddr = ApplibJpegQTable[2];

    GenScrpt->PostProc = &PIVPostCapCB;
    GenScrpt->PreProc = &PIVPreCapCB;
    TotalScriptSize += ALIGN_128(sizeof(AMP_SENC_SCRPT_GENCFG_s));
    TotalStageNum ++;
    AmbaPrint("[Applib - VideoEnc]Stage_0 0x%08X", StageAddr);

    //raw cap config
    StageAddr = PIVCapScriptAddr + TotalScriptSize;
    RawCapScrpt = (AMP_SENC_SCRPT_RAWCAP_s *)StageAddr;
    memset(RawCapScrpt, 0x0, sizeof(AMP_SENC_SCRPT_RAWCAP_s));
    RawCapScrpt->Cmd = SENC_RAWCAP;
    RawCapScrpt->SrcType = AMP_ENC_SOURCE_VIN;
    RawCapScrpt->ShType = AMBA_SENSOR_ESHUTTER_TYPE_ROLLING;
    RawCapScrpt->SensorMode = pivCtrl.SensorMode;
    RawCapScrpt->FvRawCapArea.VcapWidth = PivCaptureWidth;
    RawCapScrpt->FvRawCapArea.VcapHeight = PivCaptureHeight;
    RawCapScrpt->FvRawCapArea.EffectArea.X = RawCapScrpt->FvRawCapArea.EffectArea.Y = 0;
    RawCapScrpt->FvRawCapArea.EffectArea.Width = RawCapScrpt->FvRawCapArea.VcapWidth;
    RawCapScrpt->FvRawCapArea.EffectArea.Height = RawCapScrpt->FvRawCapArea.VcapHeight;
    RawCapScrpt->FvRawType = (cmpr)? AMP_STILLENC_RAW_COMPR: AMP_STILLENC_RAW_UNCOMPR;
    RawCapScrpt->FvRawBuf.Buf = PIVCapRawBuffAddr;
    RawCapScrpt->FvRawBuf.Width = RawWidth;
    RawCapScrpt->FvRawBuf.Height = RawHeight;
    RawCapScrpt->FvRawBuf.Pitch = RawPitch;
    RawCapScrpt->FvBufRule = AMP_ENC_SCRPT_BUFF_FIXED;
    RawCapScrpt->FvRingBufSize = RawSize*1;
    RawCapScrpt->CapCB.RawCapCB = AppLibVideoEnc_PIVRawCapCB;
    TotalScriptSize += ALIGN_128(sizeof(AMP_SENC_SCRPT_RAWCAP_s));
    TotalStageNum ++;
    AmbaPrint("[Applib - VideoEnc]Stage_1 0x%X", StageAddr);

    //raw2yuv config
    StageAddr = PIVCapScriptAddr + TotalScriptSize;
    Raw2YvuScrpt = (AMP_SENC_SCRPT_RAW2YUV_s *)StageAddr;
    memset(Raw2YvuScrpt, 0x0, sizeof(AMP_SENC_SCRPT_RAW2YUV_s));
    Raw2YvuScrpt->Cmd = SENC_RAW2YUV;
    Raw2YvuScrpt->RawType = RawCapScrpt->FvRawType;
    Raw2YvuScrpt->RawBuf.Buf = RawCapScrpt->FvRawBuf.Buf;
    Raw2YvuScrpt->RawBuf.Width = RawCapScrpt->FvRawBuf.Width;
    Raw2YvuScrpt->RawBuf.Height = RawCapScrpt->FvRawBuf.Height;
    Raw2YvuScrpt->RawBuf.Pitch = RawCapScrpt->FvRawBuf.Pitch;
    Raw2YvuScrpt->RawBufRule = RawCapScrpt->FvBufRule;
    Raw2YvuScrpt->RingBufSize = 0;
    Raw2YvuScrpt->YuvBufRule = AMP_ENC_SCRPT_BUFF_FIXED;
    Raw2YvuScrpt->YuvRingBufSize = 0;
    TotalScriptSize += ALIGN_128(sizeof(AMP_SENC_SCRPT_RAW2YUV_s));
    TotalStageNum ++;
    AmbaPrint("[Applib - VideoEnc]Stage_2 0x%X", StageAddr);

    //yuv2jpg config
    StageAddr = PIVCapScriptAddr + TotalScriptSize;
    Yuv2JpgScrpt = (AMP_SENC_SCRPT_YUV2JPG_s *)StageAddr;
    memset(Yuv2JpgScrpt, 0x0, sizeof(AMP_SENC_SCRPT_YUV2JPG_s));
    Yuv2JpgScrpt->Cmd = SENC_YUV2JPG;
    Yuv2JpgScrpt->YuvBufRule = AMP_ENC_SCRPT_BUFF_FIXED;
    Yuv2JpgScrpt->YuvRingBufSize = 0;
    TotalScriptSize += ALIGN_128(sizeof(AMP_SENC_SCRPT_YUV2JPG_s));
    TotalStageNum ++;
    AmbaPrint("[Applib - VideoEnc]Stage_3 0x%X", StageAddr);

    //script config
    Scrpt.mode = AMP_SCRPT_MODE_STILL;
    Scrpt.StepPreproc = NULL;
    Scrpt.StepPostproc = NULL;
    Scrpt.ScriptStartAddr = (UINT32)PIVCapScriptAddr;
    Scrpt.ScriptTotalSize = TotalScriptSize;
    Scrpt.ScriptStageNum = TotalStageNum;
    AmbaPrint("[Applib - VideoEnc]Scrpt addr 0x%X, Sz %uByte, stg %d", Scrpt.ScriptStartAddr, Scrpt.ScriptTotalSize, Scrpt.ScriptStageNum);

    /* Step4. execute script */
#ifdef CONFIG_APP_ARD
    AmpEnc_RunScript(StillEncPipePIV, &Scrpt, AMP_ENC_FUNC_FLAG_WAIT);
#else
    AmpEnc_RunScript(StillEncPipePIV, &Scrpt, AMP_ENC_FUNC_FLAG_NONE);
#endif

    /* Step4. release script */
    AmbaPrint("[0x%08X] memFree", PIVCapScriptAddr);
    if (AmbaKAL_BytePoolFree((void *)PIVCapScriptAddrBufRaw) != OK)
        AmbaPrint("memFree Fail (scrpt)");
    PIVCapScriptAddrBufRaw = NULL;

    AmbaPrint("memFree Done");

    return 0;
}

/**
 *  @brief PIV capture
 *
 *  PIV capture
 *
 *
 *  @return >=0 success, <0 failure
 */
int AppLibVideoEnc_CapturePIV(void)
{
    int ReturnValue = 0;
    /**PIV can set capture width/heigth , set as video setting now*/
    AMP_VIDEOENC_PIV_CTRL_s PivCtrl;
    APPLIB_SENSOR_VIN_CONFIG_s VinConfigData;
    APPLIB_SENSOR_PIV_ENC_CONFIG_s PIVCapConfig;

    memset(&VinConfigData, 0x0, sizeof(APPLIB_SENSOR_VIN_CONFIG_s));

    VinConfigData.ResID = AppLibVideoEnc_GetSensorVideoRes();
    AppLibSysSensor_GetPIVSize(VinConfigData.ResID, &PIVCapConfig);

    PivCtrl.SensorMode.Data = AppLibSysSensor_GetVinMode(&VinConfigData);
    PivCtrl.CaptureWidth = PIVCapConfig.CaptureWidth;
    PivCtrl.CaptureHeight = PIVCapConfig.CaptureHeight;
    PivCtrl.MainWidth = PIVCapConfig.CaptureWidth;
    PivCtrl.MainHeight = PIVCapConfig.CaptureHeight;
    PivCtrl.AspectRatio = PIVCapConfig.VAR;
    ReturnValue = AppLibVideoEnc_PIV(PivCtrl, 1, 1, 0, 0);
    return ReturnValue;
}

int AppLibVideoEnc_PIVInit(void)
{

    int ReturnValue = 0;

    if (StillEncPriPIV == NULL) { //no codec have been created
        AMP_STILLENC_HDLR_CFG_s EncCfg;
        AMP_VIDEOENC_LAYER_DESC_s Elayer;

        memset(&EncCfg, 0x0, sizeof(AMP_STILLENC_HDLR_CFG_s));
        memset(&Elayer, 0x0, sizeof(AMP_VIDEOENC_LAYER_DESC_s));
        EncCfg.MainLayout.Layer = &Elayer;
        AmpStillEnc_GetDefaultCfg(&EncCfg);

        // Assign callback
        EncCfg.cbEvent = AppLibVideoEnc_VideoEncCallback;
        // A12 express PIV will use H264 BS
        EncCfg.BitsBufCfg.BitsBufAddr = H264EncBitsBuf;
        EncCfg.BitsBufCfg.BitsBufSize = VIDENC_BITSFIFO_SIZE;
        EncCfg.BitsBufCfg.DescBufAddr = H264DescBuf;
        EncCfg.BitsBufCfg.DescBufSize = VIDENC_DESC_SIZE;
        EncCfg.BitsBufCfg.BitsRunoutThreshold = VIDENC_BITSFIFO_SIZE - 4*1024*1024; /**< leave 4MB */
        AmbaPrint("Bits 0x%X size %x Desc 0x%X size %d", H264EncBitsBuf, VIDENC_BITSFIFO_SIZE, H264DescBuf, VIDENC_DESC_SIZE);
        ReturnValue = AmpStillEnc_Create(&EncCfg, &StillEncPriPIV);
        if (ReturnValue != OK) {
            AmbaPrintColor(RED,"[Applib - VideoEnc] <AmpStillEnc_Create> failure!!");
        }

    }

    if (StillEncPipePIV == NULL) { //no pipeline have been created
        AMP_ENC_PIPE_CFG_s pipeCfg;

        AmpEnc_GetDefaultCfg(&pipeCfg);
        pipeCfg.encoder[0] = StillEncPriPIV;
        pipeCfg.numEncoder = 1;
        pipeCfg.cbEvent = AppLibVideoEnc_PipeCallback;
        pipeCfg.type = AMP_ENC_STILL_PIPE;
        ReturnValue = AmpEnc_Create(&pipeCfg, &StillEncPipePIV);
        if (ReturnValue != OK) {
            AmbaPrintColor(RED,"[Applib - VideoEnc] <AmpEnc_Create> failure!!");
        }
        AmpEnc_Add(StillEncPipePIV);
    }
    return ReturnValue;
}
/**
 *  @brief Set sensor video resolution ID
 *
 *  Set sensor video resolution ID
 *
 *  @param [in] videoResID Video resolution id
 *
 *  @return >=0 success, <0 failure
 */
int AppLibVideoEnc_SetSensorVideoRes(int videoResID)
{
    ApplibVideoEncVideoSetting.SensorVideoRes = videoResID;
    return 0;
}

/**
 *  @brief Set YUV device video resolution ID
 *
 *  Set YUV device video resolution ID
 *
 *  @param [in] videoResID Video resolution id
 *
 *  @return >=0 success, <0 failure
 */
int AppLibVideoEnc_SetYuvVideoRes(int videoResID)
{
    ApplibVideoEncVideoSetting.YuvVideoRes = videoResID;
    return 0;
}

/**
 *  @brief Set encode quality.
 *
 *  Set encode quality.
 *
 *  @param [in] quality Quality
 *
 *  @return >=0 success, <0 failure
 */
int AppLibVideoEnc_SetQuality(int quality)
{
    ApplibVideoEncVideoSetting.Quality = quality;
    return 0;
}

/**
 *  @brief Set pre-record mode
 *
 *  Set pre-record mode
 *
 *  @param [in] preRecord Pre-record mode
 *
 *  @return >=0 success, <0 failure
 */
int AppLibVideoEnc_SetPreRecord(int preRecord)
{
    ApplibVideoEncVideoSetting.PreRecord = preRecord;
    return 0;
}

#ifdef CONFIG_APP_ARD
/**
 *  @brief Set pre-record Time
 *
 *  Set pre-record Time
 *
 *  @param [in] preRecord Pre-record Time
 *
 *  @return >=0 success, <0 failure
 */
int AppLibVideoEnc_SetPreRecordTime(int time)
{
    ApplibVideoEncVideoSetting.PreRecordTime = time;
    return 0;
}

#endif

/**
 *  @brief Set time lapse function
 *
 *  Set time lapse function
 *
 *  @param [in] timeLapse Time lapse
 *
 *  @return >=0 success, <0 failure
 */
int AppLibVideoEnc_SetTimeLapse(int timeLapse)
{
    ApplibVideoEncVideoSetting.TimeLapse = timeLapse;
    return 0;
}

/**
 *  @brief Set dual streams function
 *
 *  Set dual streams function
 *
 *  @param [in] dualStreams Dual stream
 *
 *  @return >=0 success, <0 failure
 */
int AppLibVideoEnc_SetDualStreams(int dualStreams)
{
    ApplibVideoEncVideoSetting.DualStreams = dualStreams;
    return 0;
}

/**
 *  @brief Set split function
 *
 *  Set split function
 *
 *  @param [in] split enable flag
 *
 *  @return >=0 success, <0 failure
 */
int AppLibVideoEnc_SetSplit(int split)
{
    ApplibVideoEncVideoSetting.Split = split;
    return 0;
}

/**
 *  @brief Set PIV mode
 *
 *  Set PIV mode
 *
 *  @param [in] pivMode PIV mode
 *
 *  @return Zero (don't care)
 */
int AppLibVideoEnc_SetPivMode(UINT32 pivMode)
{
    ApplibVideoEncVideoSetting.PivMode = pivMode;
    return 0;
}

/**
 *  @brief Set fixed PIV tile number
 *
 *  Set fixed PIV tile number
 *
 *  @param [in] pivTileNum PIV tile number (dimensions: 0xVVHH)
 *
 *  @return Zero (don't care)
 */
int AppLibVideoEnc_SetPivTileNumber(UINT16 pivTileNum)
{
    ApplibVideoEncVideoSetting.PivTileNumber = pivTileNum;
    return 0;
}

/**
 *  @brief Set fixed PIV threshold
 *
 *  Set fixed PIV threshold
 *
 *  @param [in] pivThreshold pivThreshold PIV threshold (milliseconds)
 *
 *  @return Zero (don't care)
 */
int AppLibVideoEnc_SetPivThreshold(UINT16 pivThreshold)
{
    ApplibVideoEncVideoSetting.PivThreshold = pivThreshold;
    return 0;
}

/**
 *  @brief Set record mode setting
 *
 *  Set record mode setting
 *
 *  @param [in] recMode Record mode
 *
 *  @return >=0 success, <0 failure
 */
int AppLibVideoEnc_SetRecMode(UINT8 recMode)
{
    ApplibVideoEncVideoSetting.RecMode = recMode;
    return 0;
}

/**
 *  @brief Set main stream bitrate
 *
 *  Set main stream bitrate
 *
 *  @param [in] bitrate bitrate value
 *
 *  @return >=0 success, <0 failure
 */
int AppLibVideoEnc_SetPriStreamBitRate(int bitrate)
{
    extern APPLIB_VIDEOENC_BITRATE_s SensorVideoBitRateTable[][VIDEO_QUALITY_NUM];
    int VideoRes =AppLibVideoEnc_GetSensorVideoRes();
    int VideoQuality = AppLibVideoEnc_GetQuality();
    SensorVideoBitRateTable[VideoRes][VideoQuality].BitRateAvg = (float)bitrate;
    AmbaPrintColor(CYAN,"[Applib - VideoEnc] <SetPriBitRate> Set Pri Stream Bitrate to %d M",bitrate);
    return 0;
}

/**
 *  @brief Set 2-channel second stream resolution
 *
 *  Set 2-channel second stream resolution
 *
 *  @param [in] res Resolution id
 *
 *  @return >=0 success, <0 failure
 */
int AppLibVideoEnc_Set2chSecStreamRes(UINT8 res)
{
    ApplibVideoEncVideoSetting.TwoChSecStreamRes = res;
    return 0;
}

/**
 *  @brief Set video second stream width
 *
 *  Set video second stream width
 *
 *  @return >=0 success, <0 failure
 */
UINT32 AppLibVideoEnc_SetSecStreamW(int streamWidth)
{
    SecStreamWidth = streamWidth;
    return 0;
}

/**
 *  @brief Set video second stream height
 *
 *  Set video second stream height
 *
 *  @return >=0 success, <0 failure
 */
UINT32 AppLibVideoEnc_SetSecStreamH(int streamHeight)
{
    SecStreamHeight = streamHeight;
    return 0;
}

/**
 *  @brief Set video second stream Time Scale
 *
 *  Set video second stream Time Scale
 *
 *  @return >=0 success, <0 failure
 */
UINT32 AppLibVideoEnc_SetSecStreamTimeScale(int timeScale)
{
    SecStreamTimeScale = timeScale;
    return 0;
}


/**
 *  @brief Set video second stream Tick
 *
 *  Set video second stream Tick
 *
 *  @return >=0 success, <0 failure
 */
UINT32 AppLibVideoEnc_SetSecStreamTick(int tick)
{
    SecStreamTick = tick;
    return 0;
}

/**
 *  @brief Set video second stream gop M
 *
 *  Set video second stream gop M
 *
 *  @return >=0 success, <0 failure
 */
UINT32 AppLibVideoEnc_SetSecStreamGopM(int gopM)
{
    SecStreamGopM = gopM;
    return 0;
}

/**
 *  @brief Set video second stream gop N
 *
 *  Set video second stream  gop N
 *
 *  @return >=0 success, <0 failure
 */
UINT32 AppLibVideoEnc_SetSecStreamGopN(int gopN)
{
    SecStreamGopN = gopN;
    return 0;
}


/**
 *  @brief Set video second stream gop IDR
 *
 *  Set video second stream  gop IDR
 *
 *  @return >=0 success, <0 failure
 */
UINT32 AppLibVideoEnc_SetSecStreamGopIDR(int gopIDR)
{
    SecStreamGopIDR = gopIDR;
    return 0;
}

/**
 *  @brief Set video second stream Bit Rate
 *
 *  Set video second stream  gop Bit Rate
 *
 *  @return >=0 success, <0 failure
 */
UINT32 AppLibVideoEnc_SetSecStreamBitRate(int bitRate)
{
    SecStreamBitRate = bitRate;
    return 0;
}

/**
 *  @brief Set slow shutter
 *
 *  Set slow shutter
 *
 *  @return >=0 success, <0 failure
 */
UINT32 AppLibVideoEnc_SetEnalbeSlowShutter(UINT8 enable)
{
    ApplibVideoEncSlowShutterEnable = enable;
    return 0;
}

/**
 *  @brief Get current Bit Rate
 *
 *  Get current Bit Rate
 *
 *  @param [in] streamID stream id
 *
 *  @return current bit rate
 */
UINT32 AppLibvideoEnc_GetCurAvgBitrate(int streamID)
{
    static UINT32 CurPriFrameCount = 0;
    static UINT64 CurPriFrameBytes = 0;
    static UINT32 CurSecFrameCount = 0;
    static UINT64 CurSecFrameBytes = 0;
    AMBA_IMG_ENC_MONITOR_ENCODING_INFO_s EncInfo = {0};
    UINT32 TotalBytes = 0;
    UINT32 TotalFrameCount = 0;
    UINT32 AvgBitrate = 0;
    UINT32 FrameRate = 0;

    if (streamID == AMP_VIDEOENC_STREAM_PRIMARY) {
        APPLIB_SENSOR_VIDEO_ENC_CONFIG_s *VideoEncConfigData = AppLibSysSensor_GetVideoConfig(AppLibVideoEnc_GetSensorVideoRes());

        AmbaEncMonitor_GetCurrentEncodingInfo(EncMonitorStrmHdlrPri, &EncInfo);
        TotalBytes = EncInfo.TotalBytes - CurPriFrameBytes;
        CurPriFrameBytes = EncInfo.TotalBytes;
        TotalFrameCount = EncInfo.TotalFrames - CurPriFrameCount;
        CurPriFrameCount = EncInfo.TotalFrames;
        FrameRate = VideoEncConfigData->EncNumerator/1000;
    } else {
        AmbaEncMonitor_GetCurrentEncodingInfo(EncMonitorStrmHdlrSec, &EncInfo);
        TotalBytes = EncInfo.TotalBytes - CurSecFrameBytes;
        CurSecFrameBytes = EncInfo.TotalBytes;
        TotalFrameCount = EncInfo.TotalFrames - CurSecFrameCount;
        CurSecFrameCount = EncInfo.TotalFrames;
        FrameRate = AppLibVideoEnc_GetSecStreamTimeScale()/1000;
    }
    AvgBitrate = TotalBytes/TotalFrameCount * FrameRate*8;///bits

    AmbaPrintColor(CYAN,"[Applib - VideoEnc] <GetCurAvgBitrate> Stream #%d current bitrate = %d",streamID,AvgBitrate);

    return AvgBitrate;

}

/**
 *  @brief Get 2-channel second stream resolution
 *
 *  Get 2-channel second stream resolution
 *
 *  @return 2-channel second stream resolution
 */
int AppLibVideoEnc_Get2chSecStreamRes(void)
{
    return ApplibVideoEncVideoSetting.TwoChSecStreamRes;
}

/**
 *  @brief Get the video encoding setting.
 *
 *  Get the video encoding setting.
 *
 *  @param [out] setting  The video encoding setting.
 *
 *  @return >=0 success, <0 failure
 */
int AppLibVideoEnc_GetSetting(APPLIB_VIDEOENC_SETTING_s *setting)
{
    memcpy(setting, &ApplibVideoEncVideoSetting, sizeof(APPLIB_VIDEOENC_SETTING_s));
    return 0;
}

/**
 *  @brief Get the setting of sensor resolution.
 *
 *  Get the setting of sensor resolution.
 *
 *  @return The setting of sensor resolution.
 */
int AppLibVideoEnc_GetSensorVideoRes(void)
{
    return ApplibVideoEncVideoSetting.SensorVideoRes;
}

/**
 *  @brief Get the setting of YUV resolution.
 *
 *  Get the setting of YUV resolution.
 *
 *  @return The setting of YUV resolution.
 */
int AppLibVideoEnc_GetYuvVideoRes(void)
{
    return ApplibVideoEncVideoSetting.YuvVideoRes;
}

/**
 *  @brief Get the setting of video quality.
 *
 *  Get the setting of video quality.
 *
 *  @return The setting of video quality.
 */
int AppLibVideoEnc_GetQuality(void)
{
    return ApplibVideoEncVideoSetting.Quality;
}

/**
 *  @brief Get the pre-record setting.
 *
 *  Get the pre-record setting.
 *
 *  @return The pre-record setting.
 */
int AppLibVideoEnc_GetPreRecord(void)
{
    return ApplibVideoEncVideoSetting.PreRecord;
}

#ifdef CONFIG_APP_ARD
/**
 *  @brief Get the pre-record time.
 *
 *  Get the pre-record Time.
 *
 *  @return The pre-record time.
 */
int AppLibVideoEnc_GetPreRecordTime(void)
{
    return ApplibVideoEncVideoSetting.PreRecordTime;
}

#endif

/**
 *  @brief Get the setting of time lapse.
 *
 *  Get the setting of time lapse.
 *
 *  @return The setting of time lapse.
 */
int AppLibVideoEnc_GetTimeLapse(void)
{
    return ApplibVideoEncVideoSetting.TimeLapse;
}

/**
 *  @brief Get dual stream setting.
 *
 *  Get dual stream setting.
 *
 *  @return The dual stream setting.
 */
int AppLibVideoEnc_GetDualStreams(void)
{
    return ApplibVideoEncVideoSetting.DualStreams;
}

/**
 *  @brief Get the setting of split file.
 *
 *  Get the setting of split file.
 *
 *  @return The setting of split file.
 */
int AppLibVideoEnc_GetSplit(void)
{
    return ApplibVideoEncVideoSetting.Split;
}

/**
 *  Get the setting of split file.
 *
 *  @return The setting of split file.
 */
int AppLibVideoEnc_GetSplitTimeSize(UINT32 *splitTime, UINT64 *splitSize)
{
    switch (ApplibVideoEncVideoSetting.Split) {
    case VIDEO_SPLIT_OFF:
        *splitSize = 0;
        break;
    case VIDEO_SPLIT_SIZE_1G:
        *splitSize = 950 * 1024 * 1024;
        break;
    case VIDEO_SPLIT_SIZE_2G:
        *splitSize = 1900 * 1024 * 1024;
        break;
    case VIDEO_SPLIT_SIZE_4G:
        *splitSize = (UINT64)3750 * (UINT64)1024 * (UINT64)1024;
        break;
    case VIDEO_SPLIT_SIZE_64M:
        *splitSize = 64 * 1024 * 1024;
        break;
    case VIDEO_SPLIT_SIZE_AUTO:
    default:
        *splitSize = (UINT64)3750 * (UINT64)1024 * (UINT64)1024;
        break;
    }

    switch (ApplibVideoEncVideoSetting.Split) {
    case VIDEO_SPLIT_OFF:
        *splitTime = 0;
        break;
    case VIDEO_SPLIT_TIME_60_SECONDS:
        *splitTime = 60 * 1000;
        break;
    case VIDEO_SPLIT_TIME_180_SECONDS:
        *splitTime = 180 * 1000;
        break;		
    case VIDEO_SPLIT_TIME_300_SECONDS:
        *splitTime = 300 * 1000;
        break;
    case VIDEO_SPLIT_TIME_30_MINUTES:
        *splitTime = 30 * 60 * 1000;
        break;
    case VIDEO_SPLIT_TIME_AUTO:
    default:
        {
            APPLIB_VIDEOENC_BITRATE_s *VideoEncBitRateData = AppLibSysSensor_GetVideoBitRate(AppLibVideoEnc_GetSensorVideoRes(), AppLibVideoEnc_GetQuality());
            UINT32 BitRateMax = (UINT32)(VideoEncBitRateData->BitRateAvg * VideoEncBitRateData->BitRateRatioMax * 1E6);
            if (BitRateMax > 20 * 1E6) {
                *splitTime = 10 * 60 * 1000;
            } else {
                *splitTime = 30 * 60 * 1000;
            }
        }
        break;
    }
    return 0;
}

/**
 *  @brief Get assigned PIV mode
 *
 *  Get assigned PIV mode
 *
 *  @return PIV mode
 */
int AppLibVideoEnc_GetPivMode(void)
{
    return ApplibVideoEncVideoSetting.PivMode;
}

/**
 *  @brief Get assigned PIV tile number
 *
 *  Get assigned PIV tile number
 *
 *  @return  PIV tile number (dimensions: 0xVVHH)
 */
int AppLibVideoEnc_GetPivTileNumber(void)
{
    return ApplibVideoEncVideoSetting.PivTileNumber;
}

/**
 *  @brief Get assigned PIV threshold
 *
 *  Get assigned PIV threshold
 *
 *  @return PIV threshold (milliseconds)
 */
int AppLibVideoEnc_GetPivThreshold(void)
{
    return ApplibVideoEncVideoSetting.PivThreshold;
}

/**
 *  @brief Get record mode
 *
 *  Get record mode
 *
 *  @return Record mode
 */
int AppLibVideoEnc_GetRecMode(void)
{
    return ApplibVideoEncVideoSetting.RecMode;
}

/**
 *  @brief Get video second stream width
 *
 *  Get video second stream width
 *
 *  @return second stream width
 */
UINT32 AppLibVideoEnc_GetSecStreamW(void)
{
    return SecStreamWidth;
}

/**
 *  @brief Get video second stream height
 *
 *  Get video second stream height
 *
 *  @return second stream height
 */
UINT32 AppLibVideoEnc_GetSecStreamH(void)
{
    return SecStreamHeight;
}

/**
 *  @brief Get video second stream Time Scale
 *
 *  Get video second stream Time Scale
 *
 *  @return second stream Time Scale
 */
UINT32 AppLibVideoEnc_GetSecStreamTimeScale(void)
{
    return SecStreamTimeScale;
}


/**
 *  @brief Get video second stream Tick
 *
 *  Get video second stream Tick
 *
 *  @return second stream Tick
 */
UINT32 AppLibVideoEnc_GetSecStreamTick(void)
{
    return SecStreamTick;
}

/**
 *  @brief Get video second stream gop M
 *
 *  Get video second stream gop M
 *
 *  @return second stream gop M
 */
UINT32 AppLibVideoEnc_GetSecStreamGopM(void)
{
    return SecStreamGopM;
}

/**
 *  @brief Get video second stream gop N
 *
 *  Get video second stream  gop N
 *
 *  @return second stream  gop N
 */
UINT32 AppLibVideoEnc_GetSecStreamGopN(void)
{
    return SecStreamGopN;
}


/**
 *  @brief Get video second stream gop IDR
 *
 *  Get video second stream  gop IDR
 *
 *  @return second stream  gop IDR
 */
UINT32 AppLibVideoEnc_GetSecStreamGopIDR(void)
{
    return SecStreamGopIDR;
}


/**
 *  @brief Get video second stream Bit Rate
 *
 *  Get video second stream  gop Bit Rate
 *
 *  @return second stream  gop Bit Rate
 */
UINT32 AppLibVideoEnc_GetSecStreamBitRate(void)
{
    return SecStreamBitRate;
}

/**
 *  @brief Get slow shutter setting.
 *
 *  Get slow shutter setting.
 *
 *  @return Slow shutter setting
 */
UINT8 AppLibVideoEnc_GetEnalbeSlowShutter(void)
{
    return ApplibVideoEncSlowShutterEnable;
}

/**
 *  @brief Get the encoding stream setting
 *
 *  Get the encoding stream setting
 *
 *  @param [in] idx index
 *
 *  @return Stream's setting
 */
APPLIB_VIDEOENC_STREAM_SETTING_s *AppLibVideoEnc_GetStreamSetting(int idx)
{
    if (idx >= VIDEO_STREAM_SETTINGS_NUM) {
        AmbaPrintColor(RED,"[Applib - VideoEnc] <GetStreamSetting> %s: Invalid stream idx!(%d/%d)",idx,VIDEO_STREAM_SETTINGS_NUM);
        return NULL;
    }

    return &(ApplibVideoEncVideoStreamSettingTable[idx]);
}

int AppLibVideoEnc_GetValidStream(APPLIB_VIDEOENC_STREAM_LIST_s *pStreamList)
{
    int Count =0;
    APPLIB_VIDEOENC_STREAM_INFO_s *pStream = NULL;

    if (!pStreamList) {
        return -1;
    }

    pStream = &(pStreamList->StreamList[0]);
    if (VideoEncPri) {
        extern AMP_AVENC_HDLR_s *AudioEncPriHdlr;
        pStream->Id = AMP_VIDEOENC_STREAM_PRIMARY;
        pStream->HdlVideoEnc = VideoEncPri;
        if (AppLibVideoEnc_GetRecMode() == REC_MODE_AV) {
            pStream->HdlAudioEnc = AudioEncPriHdlr;
        } else {
            pStream->HdlAudioEnc = NULL;
        }

        Count++;
        pStream++;
    }

    if (AppLibVideoEnc_GetDualStreams()) {
        if (VideoEncSec) {
            extern AMP_AVENC_HDLR_s *AudioEncPriHdlr;
            extern AMP_AVENC_HDLR_s *AudioEncSecHdlr;
            pStream->Id = AMP_VIDEOENC_STREAM_SECONDARY;
            pStream->HdlVideoEnc = VideoEncSec;
            if (AppLibVideoEnc_GetRecMode() == REC_MODE_AV) {
                if (AppLibAudioEnc_GetDualStreams()) {
                pStream->HdlAudioEnc= AudioEncSecHdlr;
                } else {
                    pStream->HdlAudioEnc = AudioEncPriHdlr;
                }
            } else {
                pStream->HdlAudioEnc = NULL;
            }

            Count++;
            pStream++;
        }
    }

    pStreamList->StreamCount = Count;

    return 0;
}

int AppLibVideoEnc_EraseFifoCB(void *hdlr, UINT32 event, void* info)
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

int AppLibVideoEnc_EraseFifo(void)
{
    AMP_FIFO_CFG_s FifoDefCfg;
    AMP_FIFO_HDLR_s *TempVideoPriFifoHdlr = NULL;
    memset(&FifoDefCfg, 0x0, sizeof(AMP_FIFO_CFG_s));
    AmbaPrintColor(CYAN, "AppLibVideoEnc_EraseFifo");

    // For video encode
    AmpFifo_GetDefaultCfg(&FifoDefCfg);
    FifoDefCfg.hCodec = VideoEncPri;
    AmbaPrintColor(CYAN, "VideoEncPri codec 0x%X", VideoEncPri);
    FifoDefCfg.IsVirtual = 1;
    FifoDefCfg.NumEntries = 1024;
    FifoDefCfg.cbEvent = AppLibVideoEnc_EraseFifoCB;
    AmpFifo_Create(&FifoDefCfg, &TempVideoPriFifoHdlr);
    AmpFifo_EraseAll(TempVideoPriFifoHdlr);
    AmpFifo_Delete(TempVideoPriFifoHdlr);
    DBGMSG("[Applib - Format] <EraseFIFO> Erase Video Primary");

    if (AppLibVideoEnc_GetRecMode() == REC_MODE_AV) {
        extern AMP_AVENC_HDLR_s *AudioEncPriHdlr;
        AMP_FIFO_HDLR_s *TempAudioPriFifoHdlr = NULL;
        AMP_FIFO_CFG_s FifoDefCfg;
        AmpFifo_GetDefaultCfg(&FifoDefCfg);
        FifoDefCfg.hCodec = AudioEncPriHdlr;
        AmbaPrintColor(CYAN, "AudioEncPriHdlr codec 0x%X", AudioEncPriHdlr);
        FifoDefCfg.IsVirtual = 1;
        FifoDefCfg.NumEntries = 1024;
        FifoDefCfg.cbEvent = AppLibVideoEnc_EraseFifoCB;
        AmpFifo_Create(&FifoDefCfg, &TempAudioPriFifoHdlr);
        AmpFifo_EraseAll(TempAudioPriFifoHdlr);
        AmpFifo_Delete(TempAudioPriFifoHdlr);
        DBGMSG("[Applib - Format] <EraseFIFO> Erase Audio Primary");
    }

    /* Create a virtual fifo for secondary stream. */
    if (AppLibVideoEnc_GetDualStreams()) {
        AMP_FIFO_HDLR_s *TempVideoSecFifoHdlr = NULL;
        AMP_FIFO_CFG_s FifoDefCfg;
        AmpFifo_GetDefaultCfg(&FifoDefCfg);
        FifoDefCfg.hCodec = VideoEncSec;
        AmbaPrintColor(CYAN, "VideoEncSec codec 0x%X", VideoEncSec);
        FifoDefCfg.IsVirtual = 1;
        FifoDefCfg.NumEntries = 1024;
        FifoDefCfg.cbEvent = AppLibVideoEnc_EraseFifoCB;
        AmpFifo_Create(&FifoDefCfg, &TempVideoSecFifoHdlr);
        AmpFifo_EraseAll(TempVideoSecFifoHdlr);
        AmpFifo_Delete(TempVideoSecFifoHdlr);
        DBGMSG("[Applib - Format] <EraseFIFO> Erase Video Secondary");

        if (AppLibVideoEnc_GetRecMode() == REC_MODE_AV) {
            extern AMP_AVENC_HDLR_s *AudioEncPriHdlr;
            extern AMP_AVENC_HDLR_s *AudioEncSecHdlr;
            AMP_FIFO_HDLR_s *TempAudioSecFifoHdlr = NULL;
            AMP_FIFO_CFG_s FifoDefCfg;
            AmpFifo_GetDefaultCfg(&FifoDefCfg);
            if (AppLibAudioEnc_GetDualStreams()) {
                FifoDefCfg.hCodec = AudioEncSecHdlr;
                AmbaPrintColor(CYAN, "AudioEncSecHdlr codec 0x%X", AudioEncSecHdlr);
            } else {
                FifoDefCfg.hCodec = AudioEncPriHdlr;
                AmbaPrintColor(CYAN, "AudioEncPriHdlr codec 0x%X", AudioEncPriHdlr);
            }
            FifoDefCfg.IsVirtual = 1;
            FifoDefCfg.NumEntries = 1024;
            FifoDefCfg.cbEvent = AppLibVideoEnc_EraseFifoCB;
            AmpFifo_Create(&FifoDefCfg, &TempAudioSecFifoHdlr);
            AmpFifo_EraseAll(TempAudioSecFifoHdlr);
            AmpFifo_Delete(TempAudioSecFifoHdlr);
            DBGMSG("[Applib - Format] <EraseFIFO> Erase Audio Secondary");
        }
    }

    return 0;
}


