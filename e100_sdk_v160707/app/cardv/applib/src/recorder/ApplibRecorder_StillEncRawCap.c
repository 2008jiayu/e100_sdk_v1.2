/**
 * @file src/app/connected/applib/src/recorder/ApplibRecorder_StillEncRawCap.c
 *
 * Implementation of single capture.
 *
 * History:
 *    2014/06/16 - [Annie Ting] created file
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
#include <imgproc/AmbaImg_Adjustment_Def.h>
#include "ApplibRecorder_StillEncUtility.h"
#include <AmbaUtility.h>

//#define DEBUG_APPLIB_PHOTO_R_CAP
#if defined(DEBUG_APPLIB_PHOTO_R_CAP)
#define DBGMSG AmbaPrint
#define DBGMSGc(x) AmbaPrintColor(GREEN,x)
#define DBGMSGc2 AmbaPrintColor
#else
#define DBGMSG(...)
#define DBGMSGc(...)
#define DBGMSGc2(...)
#endif

extern AMP_STLENC_HDLR_s *StillEncPri;
extern AMP_ENC_PIPE_HDLR_s *StillEncPipe;

/* Single capture buffers */
#ifdef _STILL_BUFFER_FROM_DSP_
static UINT8 *RawCapDstRawBuffAddr = NULL;
static UINT8 *RawCapDstRaw3ARoiBuffAddr = NULL;
static UINT8 *RawCapDstRaw3AStatBuffAddr = NULL;
#else
static UINT8 *RawCapScriptAddrBufRaw = NULL;
static UINT8 *RawCapScriptAddr = NULL;
static UINT8 *RawCapRawBuffAddrBufRaw = NULL;
static UINT8 *RawCapRawBuffAddr = NULL;
#endif

static UINT32 BufferAllocateFlag = 0;
static UINT32 SensorModeSource = 0;/**< get sensor mode for system sensor:0 or set from API :1*/
static UINT16 SensorMode;
//static UINT8 G_raw_cmpr = 0;

/**
 *  @brief Raw Capture Sensor mode setting
 *
 *
 *  @param [in] Source sensor mode for system sensor:0 or set from API :1
 *  @param [in] Mode sensor mode data
 *
 *  @return 0 - success, -1 - fail
 */
UINT32 AppLibStillEnc_RawCaptureSetSensorMode(UINT32 Source, UINT16 Mode)
{
    if (Source == 1) {
        SensorModeSource = 1;
        SensorMode = Mode;
    } else {
        SensorModeSource = 0;
    }
    DBGMSG("[Applib - StillEnc] <RawCapture> Set Sensor mode as %d Source %d",Mode,Source);

    return 0;
}



/**
 *  @brief Raw Capture PreProc Callback
 *
 *  Stage1: RAWCAP  -> Nothing to do
 *  Stage2: RAW2YUV  -> setup r2y idsp cfg
 *
 *  @param [in] info preproc information
 *
 *  @return 0 - success, -1 - fail
 */
UINT32 AppLibStillEnc_RawCapturePreCB(AMP_STILLENC_PREP_INFO_s *info)
{
    if (info->StageCnt == 1) {
        AmbaPrint("[Applib - StillEnc] <RawCapture> Pre-CallBack: Stage %d", info->State);
    }
    return 0;
}
static AMP_STILLENC_PREP_s PreRawCapCB = {.Process = AppLibStillEnc_RawCapturePreCB};


static UINT16 raw_fno = 1;

/**
 *  @brief  Raw capture PostProc Callback
 *
 *  Stage1: RAWCAP  -> Dump raw
 *  Stage2: RAW2YUV -> Dump yuv
 *
 *  @param [in] info postproce information
 *
 *  @return 0 - success, -1 - fail
 */
UINT32 AppLibStillEnc_RawCapturePostCB(AMP_STILLENC_POSTP_INFO_s *info)
{

    if (info->StageCnt == 1) {
        char fn[64];
        char mode[3] = {'w','b','\0'};
        AMBA_FS_FILE *raw = NULL;


        //raw ready, dump it for debug

        sprintf(fn,"C:\\%04d.RAW", raw_fno);

        raw = AmbaFS_fopen((char const *)fn,(char const *) mode);
        AmbaPrint("[Applib - StillEnc] <RawCapture> Post-CallBack: Dump Raw 0x%X %d %d %d  to %s Start!", \
            info->media.RawInfo.RawAddr, \
            info->media.RawInfo.RawPitch, \
            info->media.RawInfo.RawWidth, \
            info->media.RawInfo.RawHeight, fn);
        AmbaFS_fwrite(info->media.RawInfo.RawAddr, \
            info->media.RawInfo.RawPitch*info->media.RawInfo.RawHeight, 1, raw);
        //AmbaFS_FSync(raw);
        AmbaFS_fclose(raw);
        raw_fno++;
    }

    return 0;
}

static AMP_STILLENC_POSTP_s PostRawCapCB = {.Process = AppLibStillEnc_RawCapturePostCB};

/**
 *  @brief Free Buf Space
 *
 *
 *  @param [in] info preproc information
 *
 *  @return 0 - success, -1 - fail
 */
UINT32 AppLibStillEnc_RawCapFreeBuf(void)
{
    if (BufferAllocateFlag == 1) {
        if (RawCapRawBuffAddrBufRaw) {
            if (AmbaKAL_BytePoolFree((void *)RawCapRawBuffAddrBufRaw) != OK)
                AmbaPrintColor(RED, "[Applib - StillEnc] <RawCapture> RawCapFreeBuf: MemFree Fail raw!");
                RawCapRawBuffAddrBufRaw = NULL;
        }
        BufferAllocateFlag = 0;
    }
    return 0;
}

/**
 *  @brief simple capture(ie rawcap + raw2yuv + yuv2jpeg)
 *
 *  simple capture(ie rawcap + raw2yuv + yuv2jpeg)
 *
 *  @param [in] iso iso mode
 *  @param [in] cmpr compressed raw or not
 *  @param [in] targetSize targetSize jpeg target Size in Kbyte unit
 *  @param [in] encodeLoop encodeloop re-encode number
 *  @param [out] raw buffer memory addr
 *
 *  @return >=0 success, <0 failure
 */
int AppLibStillEnc_RawCapture(UINT32 iso, UINT32 cmpr, UINT32 targetSize, UINT8 encodeLoop)
{
    int ReturnValue;
    void *TempPtrBuf;
    UINT16 RawPitch = 0, RawWidth = 0, RawHeight = 0;
    UINT32 RawSize = 0;
    UINT8 *SstageAddr = NULL;
    UINT32 TotalScriptSize = 0, TotalStageNum = 0;
    AMP_SENC_SCRPT_GENCFG_s *GenScrpt;
    AMP_SENC_SCRPT_RAWCAP_s *RawCapScrpt;
    AMP_SCRPT_CONFIG_s Scrpt;

    APPLIB_VOUT_PREVIEW_PARAM_s PreviewParam={0};

    AMBA_SENSOR_MODE_ID_u Mode = {0};
    AMBA_SENSOR_MODE_INFO_s VinInfo;
    APPLIB_SENSOR_STILLCAP_MODE_CONFIG_s *pMode = NULL;

    /* Get sensor mode.*/
    if (SensorModeSource) {
        Mode.Data = SensorMode;
        DBGMSG("[Applib - StillEnc] <RawCapture> Sensor mode = %d", Mode.Data);
        AmbaSensor_GetModeInfo(AppEncChannel, Mode, &VinInfo);
        RawWidth =  VinInfo.OutputInfo.RecordingPixels.Width;
        RawHeight = VinInfo.OutputInfo.RecordingPixels.Height;
    } else {
        pMode = AppLibSysSensor_GetStillCaptureModeConfig(AppLibStillEnc_GetPhotoPjpegCapMode(), AppLibStillEnc_GetPhotoPjpegConfigId());
        Mode.Data = pMode->ModeId;
        DBGMSG("[Applib - StillEnc] <RawCapture> Sensor mode = %d", Mode.Data);
        RawWidth =  pMode->CaptureWidth;
        RawHeight = pMode->CaptureHeight;
    }

    /* Set the size of Fchan preview window.*/
    PreviewParam.AspectRatio = AppLibSysSensor_GetCaptureModeAR(AppLibStillEnc_GetPhotoPjpegCapMode(), AppLibStillEnc_GetPhotoPjpegConfigId());
    PreviewParam.ChanID = DISP_CH_FCHAN;
    AppLibDisp_CalcPreviewWindowSize(&PreviewParam);

    /* Set the size of Dchan preview window.*/
    PreviewParam.ChanID = DISP_CH_DCHAN;
    AppLibDisp_CalcPreviewWindowSize(&PreviewParam);

//    G_raw_cmpr = cmpr;

    /* Step1. calc raw buffer memory */
    RawPitch = (cmpr)? \
        AMP_COMPRESSED_RAW_WIDTH(RawWidth): \
        RawWidth*2;
    RawPitch = ALIGN_32(RawPitch);
    RawSize = RawPitch*RawHeight;
    AmbaPrint("[Applib - StillEnc] <RawCapture> raw(%u %u %u)", RawPitch, RawWidth, RawHeight);


    /* Step2. allocate raw buffer address, script address */

    if (BufferAllocateFlag == 0) {

#ifdef _STILL_BUFFER_FROM_DSP_
        {
            UINT8 *dspWorkAddr;
            UINT32 dspWorkSize;
            UINT8 *bufAddr;
            int rt = 0;

            rt = AppLibStillEnc_DspWorkCalculate(&dspWorkAddr, &dspWorkSize);
            if (rt == -1) return -1;

            bufAddr = dspWorkAddr + dspWorkSize;
            RawCapRawBuffAddr = bufAddr;
            bufAddr += RawSize;
            AmbaPrint("[Applib - StillEnc] _STILL_BUFFER_FROM_DSP_<RawCapture> rawBuffAddr (0x%08X) (%u)!", RawCapRawBuffAddr, RawSize);
        }
#else
        ReturnValue = AmpUtil_GetAlignedPool(APPLIB_G_MMPL, &TempPtrBuf, (void **)&RawCapRawBuffAddrBufRaw, RawSize, 32);
        if (ReturnValue != OK) {
            AmbaPrintColor(RED,"[Applib - StillEnc] <RawCapture> NC_DDR alloc raw fail (%u)!", RawSize);
            return -1;
        } else {
            RawCapRawBuffAddr = (UINT8*)TempPtrBuf;
            AmbaPrint("[Applib - StillEnc] <RawCapture> rawBuffAddr (0x%08X) (%u)!", RawCapRawBuffAddr, RawSize);
            //AmbaPrint("[Applib - StillEnc] <RawCapture> rawBuffAddr TempPtrBuf(0x%08X) (%u)!", TempPtrBufRaw, RawSize);
        }
#endif
        BufferAllocateFlag = 1;
    }
        ReturnValue = AmpUtil_GetAlignedPool(APPLIB_G_MMPL, &TempPtrBuf, (void **)&RawCapScriptAddrBufRaw, 128*10, 32); //TBD
        if (ReturnValue != OK) {
            AmbaPrintColor(RED,"[Applib - StillEnc] <RawCapture> NC_DDR alloc scriptAddr fail (%u)!", 128*10);
            return -1;
        } else {
            RawCapScriptAddr = (UINT8*)TempPtrBuf;
            AmbaPrint("[Applib - StillEnc] <RawCapture> scriptAddr (0x%08X) (%d)!", RawCapScriptAddr, 128*10);
            //AmbaPrint("[Applib - StillEnc] <RawCapture> scriptAddr TempPtrBuf(0x%08X) (%u)!", RawCapScriptAddrBufRaw, 128*10);
        }
    /* Step3. fill script */
    //general config
    SstageAddr = RawCapScriptAddr;
    GenScrpt = (AMP_SENC_SCRPT_GENCFG_s *)SstageAddr;
    memset(GenScrpt, 0x0, sizeof(AMP_SENC_SCRPT_GENCFG_s));
    GenScrpt->Cmd = SENC_GENCFG;
    GenScrpt->RawEncRepeat = 0;
    GenScrpt->RawToCap = 1;
    GenScrpt->StillProcMode = iso;

    GenScrpt->QVConfig.DisableLCDQV = 1;
    GenScrpt->QVConfig.DisableHDMIQV = 1;
    GenScrpt->b2LVCfg = AMP_ENC_SCRPT_B2LV_NONE;
    //AutoBackToLiveview = (GenScrpt->b2LVCfg)? 1: 0;
    GenScrpt->ScrnEnable = 0;
    GenScrpt->ThmEnable = 0;
    GenScrpt->PreProc = &PreRawCapCB;
    GenScrpt->PostProc = &PostRawCapCB;
    TotalScriptSize += ALIGN_128(sizeof(AMP_SENC_SCRPT_GENCFG_s));
    AmbaPrint("[Applib - StillEnc] <RawCapture> Stage #%d  0x%08X", TotalStageNum, SstageAddr);
    TotalStageNum ++;


    //raw cap config
    SstageAddr = RawCapScriptAddr + TotalScriptSize;
    RawCapScrpt = (AMP_SENC_SCRPT_RAWCAP_s *)SstageAddr;
    memset(RawCapScrpt, 0x0, sizeof(AMP_SENC_SCRPT_RAWCAP_s));
    RawCapScrpt->Cmd = SENC_RAWCAP;
    RawCapScrpt->SrcType = AMP_ENC_SOURCE_VIN;
    RawCapScrpt->ShType = AMBA_SENSOR_ESHUTTER_TYPE_ROLLING;
    RawCapScrpt->SensorMode = Mode;
    RawCapScrpt->FvRawCapArea.VcapWidth = RawWidth;
    RawCapScrpt->FvRawCapArea.VcapHeight = RawHeight;
    RawCapScrpt->FvRawCapArea.EffectArea.X = RawCapScrpt->FvRawCapArea.EffectArea.Y = 0;
    RawCapScrpt->FvRawCapArea.EffectArea.Width = RawCapScrpt->FvRawCapArea.VcapWidth;
    RawCapScrpt->FvRawCapArea.EffectArea.Height = RawCapScrpt->FvRawCapArea.VcapHeight;
    RawCapScrpt->FvRawType = (cmpr)? AMP_STILLENC_RAW_COMPR: AMP_STILLENC_RAW_UNCOMPR;
    RawCapScrpt->FvRawBuf.Buf = RawCapRawBuffAddr;
    RawCapScrpt->FvRawBuf.Width = RawWidth;
    RawCapScrpt->FvRawBuf.Height = RawHeight;
    RawCapScrpt->FvRawBuf.Pitch = RawPitch;
    RawCapScrpt->FvBufRule = AMP_ENC_SCRPT_BUFF_FIXED;
    RawCapScrpt->FvRingBufSize = RawSize*1;
    RawCapScrpt->CapCB.RawCapCB = AppLibStillEnc_RawCapCB;
    TotalScriptSize += ALIGN_128(sizeof(AMP_SENC_SCRPT_RAWCAP_s));
    AmbaPrint("[Applib - StillEnc] <RawCapture> Stage #%d  0x%08X", TotalStageNum, SstageAddr);
    TotalStageNum ++;


    //script config
    Scrpt.mode = AMP_SCRPT_MODE_STILL;
    Scrpt.StepPreproc = NULL;
    Scrpt.StepPostproc = NULL;
    Scrpt.ScriptStartAddr = (UINT32)RawCapScriptAddr;
    Scrpt.ScriptTotalSize = TotalScriptSize;
    Scrpt.ScriptStageNum = TotalStageNum;
    AmbaPrint("[Applib - StillEnc] <RawCapture> Scrpt addr 0x%X, Sz %uByte, stg %d", Scrpt.ScriptStartAddr, Scrpt.ScriptTotalSize, Scrpt.ScriptStageNum);

    AppLibImage_EnableImgSchdlr(0,0);
    /* Step4. execute script */
    ReturnValue = AmpEnc_RunScript(StillEncPipe, &Scrpt, AMP_ENC_FUNC_FLAG_NONE);
    if (ReturnValue < 0) {
        AmbaPrintColor(RED,"[Applib - StillEnc] <RawCapture> AmpEnc_RunScript Fail ");
    }

    /* Step4. release script */
    AmbaPrint("[Applib - StillEnc] <RawCapture> [0x%08X] memFree", RawCapScriptAddrBufRaw);
    if (AmbaKAL_BytePoolFree((void *)RawCapScriptAddrBufRaw) != OK) {
        AmbaPrintColor(RED,"[Applib - StillEnc] <RawCapture> memFree Fail (scrpt)");
    }
    RawCapScriptAddrBufRaw = NULL;

    AmbaPrint("[Applib - StillEnc] <RawCapture> memFree Done");


    return 0;
}

/**
 *  @brief To capture the photo with single capture mode
 *
 *  To capture the photo with single capture mode
 *
 *  @return >=0 success, <0 failure
 */
int AppLibStillEnc_CaptureRaw(UINT32* RawBufAddr)
{
    AppLibImage_Lock3A();
    AppLibStillEnc_RawCapture(1, 0, 0, 0);
    *RawBufAddr =(UINT32) RawCapRawBuffAddr;
    AmbaPrintColor(5,"AppLibStillEnc_CaptureRaw addr = %08x",RawBufAddr);

    return 0;
}

