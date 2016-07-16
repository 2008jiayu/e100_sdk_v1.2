/**
 * @file src/app/connected/applib/src/recorder/ApplibRecorder_StillEncRawEnc.c
 *
 * Implementation of single capture.
 *
 * History:
 *    2014/10/20 - [Annie Ting] created file
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
#include <ituner/AmbaImgCalibItuner.h>
#include <AmbaTUNE_TextHdlr.h>
#include <AmbaCache.h>



//#define DEBUG_APPLIB_PHOTO_R_ENC
#if defined(DEBUG_APPLIB_PHOTO_R_ENC)
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
static AMP_FIFO_HDLR_s *RawEncFifoHdlr = NULL;

static UINT8 RawEncNeedPreLoad = 0;
static UINT8 RawEncPreLoadDone = 0;
static UINT8 RawEncFinishFlag = 0;
/* Raw Encode buffers */
static UINT8 *RawEncScriptAddrBufRaw = NULL;
static UINT8 *RawEncScriptAddr = NULL;
static UINT8 *RawEncRawBuffAddrBufRaw = NULL;
static UINT8 *RawEncRawBuffAddr = NULL;
static UINT8 *RawEncYuvBuffAddrBufRaw = NULL;
static UINT8 *RawEncYuvBuffAddr = NULL;
static UINT8 *RawEncScrnBuffAddrBufRaw = NULL;
static UINT8 *RawEncScrnBuffAddr = NULL;
static UINT8 *RawEncThmBuffAddrBufRaw = NULL;
static UINT8 *RawEncThmBuffAddr = NULL;
static UINT8 *RawEnc3ARoiBuffAddr = NULL;
static UINT8 *RawEnc3ARoiBuffAddrBufRaw = NULL;
static UINT8 *RawEnc3AStatBuffAddr = NULL;
static UINT8 *RawEnc3AStatBuffAddrBufRaw = NULL;
static UINT8 *RawEncQvLCDBuffAddrBufRaw = NULL;
static UINT8 *RawEncQvLCDBuffAddr = NULL;
static UINT8 *RawEncQvHDMIBuffAddrBufRaw = NULL;
static UINT8 *RawEncQvHDMIBuffAddr = NULL;

/**
 *  @brief Single Capture PreProc Callback
 *
 *  Stage1: RAWCAP  -> Nothing to do
 *  Stage2: RAW2YUV  -> setup r2y idsp cfg
 *
 *  @param [in] info preproc information
 *
 *  @return 0 - success, -1 - fail
 */
UINT32 AppLibStillEnc_RawEncodePreCB(AMP_STILLENC_PREP_INFO_s *info)
{
    if (info->StageCnt == 3) {
        AmbaPrint("[Applib - StillEnc] <RawEncode> Pre-CallBack: SerialNum %d", info->JpegSerialNumber);
    }
    return 0;
}
static AMP_STILLENC_PREP_s PreRawEncCB = {.Process = AppLibStillEnc_RawEncodePreCB};

static UINT16 yuv_fno = 1;

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
UINT32 AppLibStillEnc_RawEncodePostCB(AMP_STILLENC_POSTP_INFO_s *info)
{
    static UINT8 yuvFlag = 0;

    if (info->StageCnt == 1) {
        char fn[32];
        char fn1[32];
        char mode[3] = {'w','b','\0'};
        UINT8 *LumaAddr = NULL, *ChromaAddr = NULL;
        UINT16 Pitch = 0, Width = 0, Height = 0;

        if (info->media.YuvInfo.ThmLumaAddr) {
            LumaAddr = info->media.YuvInfo.ThmLumaAddr;
            ChromaAddr = info->media.YuvInfo.ThmChromaAddr;
            Pitch = info->media.YuvInfo.ThmPitch;
            Width = info->media.YuvInfo.ThmWidth;
            Height = info->media.YuvInfo.ThmHeight;
        } else if (info->media.YuvInfo.ScrnLumaAddr) {
            LumaAddr = info->media.YuvInfo.ScrnLumaAddr;
            ChromaAddr = info->media.YuvInfo.ScrnChromaAddr;
            Pitch = info->media.YuvInfo.ScrnPitch;
            Width = info->media.YuvInfo.ScrnWidth;
            Height = info->media.YuvInfo.ScrnHeight;
        } else if (info->media.YuvInfo.LumaAddr) {
            LumaAddr = info->media.YuvInfo.LumaAddr;
            ChromaAddr = info->media.YuvInfo.ChromaAddr;
            Pitch = info->media.YuvInfo.Pitch;
            Width = info->media.YuvInfo.Width;
            Height = info->media.YuvInfo.Height;
        }

        //release yuv buffers
        if (info->media.YuvInfo.ThmLumaAddr) {
            yuvFlag |= 0x4;
        } else if (info->media.YuvInfo.ScrnLumaAddr) {
            yuvFlag |= 0x2;
        } else if (info->media.YuvInfo.LumaAddr) {
            yuvFlag |= 0x1;
        }

        if (yuvFlag == 0x7) {
            //reset yuv flag
            yuvFlag = 0x0;
            yuv_fno++;
        }
    } else {
        /**do nothing*/
    }

    return 0;
}

static AMP_STILLENC_POSTP_s PostRawEncCB = {.Process = AppLibStillEnc_RawEncodePostCB};

int AppLibStillEnc_RawEncFifoCB(void *hdlr, UINT32 event, void* info)
{
    AMP_BITS_DESC_s *desc;

    AmpFifo_PeekEntry(hdlr, &desc, 0);
    DBGMSGc2(GREEN,"[Applib - StillEnc] <RawEncFifoCB>: event :0x%x hdlr : 0x%x",event);
    switch (event) {
    case AMP_FIFO_EVENT_DATA_READY:
        AmbaPrint("[Applib - StillEnc] <RawEncFifoCB>: AMP_FIFO_EVENT_DATA_READY type = %d",desc->Type);
        if (desc->Type == AMP_FIFO_TYPE_JPEG_FRAME) {
            Ituner_Ext_File_Param_s Ext_File_Param;
            char FileName[64];
            sprintf(FileName,"C:\\%04d_m.jpeg", yuv_fno);
#if 0
{
            char mdASCII[3] = {'w','+','\0'};
            AMBA_FS_FILE *jpeg = NULL;
            jpeg = AmbaFS_fopen((char const *)FileName,(char const *) mdASCII);
            AmbaFS_fwrite(desc->StartAddr, desc->Size, 1, jpeg);
            AmbaFS_FSync(jpeg);
            AmbaFS_fclose(jpeg);
}
#endif
            memset(&Ext_File_Param, 0x0, sizeof(Ituner_Ext_File_Param_s));
            Ext_File_Param.JPG_Save_Param.Address = desc->StartAddr;
            Ext_File_Param.JPG_Save_Param.Size = desc->Size;
            Ext_File_Param.JPG_Save_Param.Target_File_Path = FileName;
            AmbaTUNE_Save_Data(EXT_FILE_JPG, &Ext_File_Param);
            AmbaPrintColor(CYAN,"[Applib - StillEnc] <RawEncFifoCB>: Save JPEG DONE");
            /**Free buffer & delete fifo after jpeg write finish*/
            AppLibStillEnc_RawEncFreeBuf();
            AmpFifo_EraseAll(RawEncFifoHdlr);
            AmpFifo_Delete(RawEncFifoHdlr);
            RawEncFifoHdlr = NULL;
            RawEncFinishFlag = 0;
            /**Back to Liveview*/
            AppLibImage_UnLock3A();
            AmpStillEnc_EnableLiveviewCapture(AMP_STILL_PREPARE_TO_STILL_LIVEVIEW, 0);
            AppLibStillEnc_LiveViewSetup();
            AppLibStillEnc_LiveViewStart();
        }
        break;
    case AMP_FIFO_EVENT_DATA_EOS:
        DBGMSG("[Applib - StillEnc] <RawEncFifoCB>: AMP_FIFO_EVENT_DATA_EOS");
        break;
    default:
        AmbaPrint("[Applib - StillEnc] <RawEncFifoCB>: evnet 0x%x", event);
        break;
    }

    return 0;
}

int AppLibStillEnc_EncodeSetup(void)
{
    int ReturnValue = 0;

    if(RawEncFifoHdlr == NULL) {
        AMP_FIFO_CFG_s FifoDefCfg;
        AmpFifo_GetDefaultCfg(&FifoDefCfg);
        FifoDefCfg.hCodec = StillEncPri;
        FifoDefCfg.IsVirtual = 1;
        FifoDefCfg.NumEntries = 128;
        FifoDefCfg.cbEvent = AppLibStillEnc_RawEncFifoCB;
        ReturnValue = AmpFifo_Create(&FifoDefCfg, &RawEncFifoHdlr);
        DBGMSG("[Applib - StillEnc] <RawEncode>: RawEncFifoHdlr = 0x%x",RawEncFifoHdlr);
    }

    return ReturnValue;
}


/**
 *  To free the photo buffer after capture done
 *
 *  @return >=0 success, <0 failure
 */
int AppLibStillEnc_RawEncFreeBuf(void)
{
    int ReturnValue = 0;
    if (RawEncRawBuffAddrBufRaw) {
        if (AmbaKAL_BytePoolFree((void *)RawEncRawBuffAddrBufRaw) != OK)
            AmbaPrintColor(RED, "[Applib - StillEnc] <RawEncode> Post-CallBack: MemFree Fail Rawf!");
        RawEncRawBuffAddrBufRaw = NULL;
    }
    if (RawEncYuvBuffAddrBufRaw) {
        if (AmbaKAL_BytePoolFree((void *)RawEncYuvBuffAddrBufRaw) != OK)
            AmbaPrintColor(RED, "[Applib - StillEnc] <RawEncode> Post-CallBack: MemFree Fail YuvBuff!");
        RawEncYuvBuffAddrBufRaw = NULL;
    }
    if (RawEncScrnBuffAddrBufRaw) {
        if (AmbaKAL_BytePoolFree((void *)RawEncScrnBuffAddrBufRaw) != OK)
            AmbaPrintColor(RED, "[Applib - StillEnc] <RawEncode> Post-CallBack: MemFree Fail ScrnBuff!");
        RawEncScrnBuffAddrBufRaw = NULL;
    }
    if (RawEncThmBuffAddrBufRaw) {
        if (AmbaKAL_BytePoolFree((void *)RawEncThmBuffAddrBufRaw) != OK)
            AmbaPrintColor(RED, "[Applib - StillEnc] <RawEncode> Post-CallBack: MemFree Fail ThmBuff!");
        RawEncThmBuffAddrBufRaw = NULL;
    }
    if (RawEnc3ARoiBuffAddrBufRaw) {
        if (AmbaKAL_BytePoolFree((void *)RawEnc3ARoiBuffAddrBufRaw) != OK)
            AmbaPrintColor(RED, "[Applib - StillEnc] <RawEncode> Post-CallBack: MemFree Fail RoiBuff!");
        RawEnc3ARoiBuffAddrBufRaw = NULL;
    }
    if (RawEnc3AStatBuffAddrBufRaw) {
        if (AmbaKAL_BytePoolFree((void *)RawEnc3AStatBuffAddrBufRaw) != OK)
            AmbaPrintColor(RED, "[Applib - StillEnc] <RawEncode> Post-CallBack: MemFree Fail 3ABuff!");
        RawEnc3AStatBuffAddrBufRaw = NULL;
    }
     return ReturnValue;
}

int AppLibStillEnc_RawEncPreLoadDone(void)
{
    int ReturnValue = 0;
    if (RawEncNeedPreLoad == 0) {
        ReturnValue = -1;
        AmbaPrintColor(RED,"[ItunerRawEnc] Pre-load is not needed");
    } else {
        RawEncPreLoadDone = 1;
        AmbaPrint("[ItunerRawEnc] Pre-load is done");
    }
    return ReturnValue;
}

/**
 *  @brief get raw encode buffer
 *
 *  get raw encode buffer
 *
 *  @param [out] rawBuf raw Buffer information structure address
 *
 *  @return >=0 success, <0 failure
 */
int AppLibStillEnc_RawEncGetRawEncodeBuffer(AMBA_DSP_RAW_BUF_s *rawBuf)
{
    int ReturnValue = 0;

    if (RawEncNeedPreLoad == 0) {
        void *TempPtrBuf;
        UINT16 RawPitch = 0, RawWidth = 0, RawHeight = 0;
        UINT16 YuvWidth = 0, YuvHeight = 0, ScrnW = 0, ScrnH = 0, ThmW = 0, ThmH = 0;
        UINT16 QvLcdWidth = 0, QvLcdHeight = 0, QvHdmiWidth = 0, QvHdmiHeight = 0;
        UINT32 RawSize = 0, YuvSize = 0, ScrnSize = 0, ThmSize = 0, QvLcdSize = 0, QvHdmiSize = 0;
        UINT32 RoiSize = 0, Raw3aSize = 0;
        ITUNER_SYSTEM_s SystemInfo;
        APPLIB_SENSOR_STILLCAP_CONFIG_s *StillCapConfigData;

        APPLIB_VOUT_PREVIEW_PARAM_s PreviewParam={0};
        UINT32 QvDchanWidth = 0;
        UINT32 QvDchanHeight = 0;
        UINT32 QvFchanWidth = 0;
        UINT32 QvFchanHeight = 0;

        memset(&SystemInfo,0x0, sizeof(ITUNER_SYSTEM_s));

        AmbaTUNE_Get_SystemInfo(&SystemInfo);
        StillCapConfigData = AppLibSysSensor_GetPjpegConfig(AppLibStillEnc_GetPhotoPjpegCapMode(), AppLibStillEnc_GetPhotoPjpegConfigId());

        RawPitch = ALIGN_32(SystemInfo.RawPitch);
        RawWidth =  SystemInfo.RawWidth;
        RawHeight =  SystemInfo.RawHeight;
        RawSize = (UINT32) RawPitch*RawHeight;
        AmbaPrint("[ItunerRawEnc] <Get buffer> raw(%u %u %u)", RawWidth, RawHeight, RawPitch);

        AppLibStillEnc_GetYuvWorkingBuffer(SystemInfo.MainWidth, SystemInfo.MainHeight, RawWidth, RawHeight, &YuvWidth, &YuvHeight);
        YuvSize = (UINT32) (YuvWidth*YuvHeight*2);
        AmbaPrint("[ItunerRawEnc] <Get buffer> yuv(%u %u)", YuvWidth, YuvHeight);

        ScrnW = ALIGN_32(StillCapConfigData->ScreennailWidth);
        ScrnH = ALIGN_16(StillCapConfigData->ScreennailHeight);
        ScrnSize = (UINT32)(ScrnW*ScrnH*2);
        ScrnSize += (ScrnSize*10)/100;
        ThmW = ALIGN_32(StillCapConfigData->ThumbnailWidth);
        ThmH = ALIGN_16(StillCapConfigData->ThumbnailHeight);
        ThmSize = (UINT32)(ThmW*ThmH*2);
        ThmSize += (ThmSize*10)/100;
        AmbaPrint("[ItunerRawEnc] <Get buffer> scrn(%u %u) thm(%u %u)", ScrnW, ScrnH, \
                ThmW, ThmH);

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

        QvLcdWidth =ALIGN_32(QvDchanWidth);
        QvLcdHeight = ALIGN_16(QvDchanHeight);
        QvHdmiWidth = ALIGN_32(QvFchanWidth);
        QvHdmiHeight = ALIGN_16(QvFchanHeight);
        QvLcdSize = (UINT32) (QvLcdWidth*QvLcdHeight*2);
        QvHdmiSize = (UINT32) (QvHdmiWidth*QvHdmiHeight*2);

        RoiSize = sizeof(AMP_STILLENC_RAW2RAW_ROI_s);
        Raw3aSize = sizeof(AMBA_DSP_EVENT_STILL_CFA_3A_DATA_s);

        ReturnValue = AmpUtil_GetAlignedPool(APPLIB_G_MMPL, &TempPtrBuf, (void **)&RawEncRawBuffAddrBufRaw, RawSize, 32);
        if (ReturnValue != OK) {
            AmbaPrintColor(RED,"[Applib - StillEnc] <RawEncode> NC_DDR alloc raw fail (%u)!", RawSize);
            return -1;
        } else {
            RawEncRawBuffAddr = (UINT8*)TempPtrBuf;
            AmbaCache_Clean(RawEncRawBuffAddrBufRaw, RawSize);
            AmbaPrint("[Applib - StillEnc] <RawEncode> rawBuffAddr (0x%08X) (%u)!", RawEncRawBuffAddr, RawSize);
            //AmbaPrint("[Applib - StillEnc] <RawEncode> rawBuffAddr TempPtrBuf(0x%08X) (%u)!", SingleCapRawBuffAddrBufRaw, RawSize);
        }

        ReturnValue = AmpUtil_GetAlignedPool(APPLIB_G_MMPL, &TempPtrBuf, (void **)&RawEncYuvBuffAddrBufRaw, YuvSize, 32);
        if (ReturnValue != OK) {
            AmbaPrintColor(RED,"[Applib - StillEnc] <RawEncode> NC_DDR alloc yuv_main fail (%u)!", YuvSize);
            return -1;
        } else {
            RawEncYuvBuffAddr = (UINT8*)TempPtrBuf;
            AmbaCache_Clean(RawEncYuvBuffAddrBufRaw, YuvSize);
            AmbaPrint("[Applib - StillEnc] <RawEncode> yuvBuffAddr (0x%08X)  (%u)!", RawEncYuvBuffAddr,YuvSize);
            //AmbaPrint("[Applib - StillEnc] <RawEncode> yuvBuffAddr TempPtrBuf(0x%08X) (%u)!", SingleCapYuvBuffAddrBufRaw, RawSize);
        }

        ReturnValue = AmpUtil_GetAlignedPool(APPLIB_G_MMPL, &TempPtrBuf, (void **)&RawEncScrnBuffAddrBufRaw, ScrnSize*1, 32);
        if (ReturnValue != OK) {
            AmbaPrintColor(RED,"[Applib - StillEnc] <RawEncode> NC_DDR alloc yuv_scrn fail (%u)!", ScrnSize*1);
            return -1;
        } else {
            RawEncScrnBuffAddr = (UINT8*)TempPtrBuf;
            AmbaPrint("[Applib - StillEnc] <RawEncode> scrnBuffAddr (0x%08X)!", RawEncScrnBuffAddr);
            //AmbaPrint("[Applib - StillEnc] <RawEncode> scrnBuffAddr TempPtrBuf(0x%08X) (%u)!", SingleCapScrnBuffAddrBufRaw, RawSize);
        }

        ReturnValue = AmpUtil_GetAlignedPool(APPLIB_G_MMPL, &TempPtrBuf, (void **)&RawEncThmBuffAddrBufRaw, ThmSize*1, 32);
        if (ReturnValue != OK) {
            AmbaPrintColor(RED,"[Applib - StillEnc] <RawEncode> NC_DDR alloc yuv_thm fail (%u)!", ThmSize*1);
            return -1;
        } else {
            RawEncThmBuffAddr = (UINT8*)TempPtrBuf;
            AmbaPrint("[Applib - StillEnc] <RawEncode> thmBuffAddr (0x%08X)!", RawEncThmBuffAddr);
            //AmbaPrint("[Applib - StillEnc] <RawEncode> thmBuffAddr TempPtrBuf(0x%08X) (%u)!", SingleCapThmBuffAddrBufRaw, RawSize);
        }

        ReturnValue = AmpUtil_GetAlignedPool(APPLIB_G_MMPL, &TempPtrBuf, (void **)&RawEnc3ARoiBuffAddrBufRaw, RoiSize, 32);
        if (ReturnValue != OK) {
            AmbaPrint("[Applib - StillEnc] <RawEncode> NC_DDR alloc raw fail (%u)!", RoiSize);
        } else {
            RawEnc3ARoiBuffAddr = (UINT8*)TempPtrBuf;
            AmbaCache_Clean(RawEnc3ARoiBuffAddrBufRaw, RoiSize);
            AmbaPrint("[Applib - StillEnc] <RawEncode> raw3ARoiBuffAddr (0x%08X) (%u)!", RawEnc3ARoiBuffAddr, RoiSize);
        }


        ReturnValue = AmpUtil_GetAlignedPool(APPLIB_G_MMPL, &TempPtrBuf, (void **)&RawEnc3AStatBuffAddrBufRaw, Raw3aSize, 32);
        if (ReturnValue != OK) {
            AmbaPrint("[Applib - StillEnc] <RawEncode> NC_DDR alloc raw 3A stat fail (%u)!", Raw3aSize);
        } else {
            RawEnc3AStatBuffAddr = (UINT8*)TempPtrBuf;
            AmbaCache_Clean(RawEnc3AStatBuffAddrBufRaw, Raw3aSize);
            AmbaPrint("[Applib - StillEnc] <RawEncode> raw3AStatBuffAddr (0x%08X) (%u)!", RawEnc3AStatBuffAddr, Raw3aSize);
        }

        ReturnValue = AmpUtil_GetAlignedPool(APPLIB_G_MMPL, &TempPtrBuf, (void **)&RawEncQvLCDBuffAddrBufRaw, QvLcdSize, 32);
        if (ReturnValue != OK) {
            AmbaPrint("[Applib - StillEnc] <RawEncode> NC_DDR alloc raw fail (%u)!", QvLcdSize);
        } else {
            RawEncQvLCDBuffAddr = (UINT8*)TempPtrBuf;
            AmbaCache_Clean(RawEncQvLCDBuffAddrBufRaw, QvLcdSize);
            AmbaPrint("[Applib - StillEnc] <RawEncode> QvLCDBuffAddr (0x%08X) (%u)!", RawEncQvLCDBuffAddr, QvLcdSize);
        }


        ReturnValue = AmpUtil_GetAlignedPool(APPLIB_G_MMPL, &TempPtrBuf, (void **)&RawEncQvHDMIBuffAddrBufRaw, QvHdmiSize, 32);
        if (ReturnValue != OK) {
            AmbaPrint("[Applib - StillEnc] <RawEncode> NC_DDR alloc raw 3A stat fail (%u)!", QvHdmiSize);
        } else {
            RawEncQvHDMIBuffAddr = (UINT8*)TempPtrBuf;
            AmbaCache_Clean(RawEncQvHDMIBuffAddrBufRaw, QvHdmiSize);
            AmbaPrint("[Applib - StillEnc] <RawEncode> QvHDMIBuffAddr (0x%08X) (%u)!", RawEncQvHDMIBuffAddr, QvHdmiSize);
        }

        memset(rawBuf, 0x0, sizeof(AMBA_DSP_RAW_BUF_s));
        rawBuf->Compressed = (UINT8) SystemInfo.CompressedRaw;
        rawBuf->Window.Width = RawWidth;
        rawBuf->Window.Height = RawHeight;
        rawBuf->Pitch = RawPitch;
        rawBuf->pBaseAddr = (UINT8 *)RawEncRawBuffAddr;
        RawEncNeedPreLoad = 1;
    } else {
        ReturnValue = -1;
        AmbaPrintColor(RED,"[StillTuning] Already ask for buffer");
    }

    return ReturnValue;
}


/**
 *  @brief raw encode (ie raw2yuv + yuv2jpeg)
 *
 *  raw encode (ie raw2yuv + yuv2jpeg)
 *
 *  @param [in] targetSize targetSize jpeg target Size in Kbyte unit
 *  @param [in] encodeLoop encodeloop re-encode number
 *
 *  @return >=0 success, <0 failure
 */
int AppLibStillEnc_RawEncode(void)
{
    int ReturnValue;
    void *TempPtrBuf;
    UINT16 RawPitch = 0, RawWidth = 0, RawHeight = 0;
    UINT16 YuvWidth = 0, YuvHeight = 0, ScrnW = 0, ScrnH = 0, ThmW = 0, ThmH = 0;
    UINT16 QvLcdWidth = 0, QvLcdHeight = 0, QvHdmiWidth = 0, QvHdmiHeight = 0;
    UINT32 RawSize = 0, YuvSize = 0, QvLcdSize = 0, QvHdmiSize = 0;
    UINT8 *StageAddr = NULL;
    UINT32 TotalScriptSize = 0, TotalStageNum = 0;
    AMP_SENC_SCRPT_GENCFG_s *GenScrpt = NULL;
    AMP_SENC_SCRPT_RAW2YUV_s *Raw2YuvScrpt = NULL;
    AMP_SENC_SCRPT_RAW2RAW_s *Raw2RawScrpt = NULL;
    AMP_SENC_SCRPT_YUV2JPG_s *Yuv2JpgScrpt = NULL;
    AMP_SCRPT_CONFIG_s Scrpt;
    ITUNER_SYSTEM_s SystemInfo;
    ITUNER_INFO_s ItunerInfo;
    APPLIB_SENSOR_STILLCAP_CONFIG_s *StillCapConfigData;

    APPLIB_VOUT_PREVIEW_PARAM_s PreviewParam={0};
    UINT32 QvDchanWidth = 0;
    UINT32 QvDchanHeight = 0;
    UINT32 QvFchanWidth = 0;
    UINT32 QvFchanHeight = 0;

    memset(&SystemInfo,0x0, sizeof(ITUNER_SYSTEM_s));
    memset(&ItunerInfo,0x0, sizeof(ITUNER_INFO_s));
    AmbaTUNE_Get_SystemInfo(&SystemInfo);
    AmbaTUNE_Get_ItunerInfo(&ItunerInfo);

    RawPitch = ALIGN_32(SystemInfo.RawPitch);
    RawWidth =  SystemInfo.RawWidth;
    RawHeight =  SystemInfo.RawHeight;
    RawSize = RawPitch*RawHeight;
    AmbaPrint("[Applib - StillEnc] <RawEncode> raw(%u %u %u)", RawPitch, RawWidth, RawHeight);

    AmpEnc_StopLiveview(StillEncPipe, AMP_ENC_FUNC_FLAG_WAIT);
    AppLibStillEnc_EncodeSetup();

    if (RawEncNeedPreLoad == 1) {
        if (RawEncPreLoadDone == 0) {
            AmbaPrintColor(RED,"[ItunerRawEnc] Please make sure pre-load is done");
        }
    } else {
        AMBA_DSP_RAW_BUF_s RawBuf = {0};
        AppLibStillEnc_RawEncGetRawEncodeBuffer(&RawBuf);
        {   //Load raw file
            Ituner_Ext_File_Param_s Ituner_Ext_File_Param = {0};
            Ituner_Ext_File_Param.Raw_Load_Param.Address = RawBuf.pBaseAddr;
            Ituner_Ext_File_Param.Raw_Load_Param.Max_Size = RawPitch*RawHeight;
            AmbaTUNE_Load_Data(EXT_FILE_RAW, &Ituner_Ext_File_Param);
            AmbaPrint("[ItunerRawEnc] Load Raw Done(0x%X %d)", RawBuf.pBaseAddr, RawPitch*RawHeight);
            AmbaCache_Clean(Ituner_Ext_File_Param.Raw_Load_Param.Address, Ituner_Ext_File_Param.Raw_Load_Param.Max_Size);
        }
    }


    AppLibStillEnc_GetYuvWorkingBuffer(SystemInfo.RawWidth, SystemInfo.RawHeight, RawWidth, RawHeight, &YuvWidth, &YuvHeight);
    YuvSize = YuvWidth*YuvHeight*2;

    AmbaPrint("[Applib - StillEnc] <RawEncode> yuv(%u %u %u)!", YuvWidth, YuvHeight, YuvSize);

    StillCapConfigData = AppLibSysSensor_GetPjpegConfig(AppLibStillEnc_GetPhotoPjpegCapMode(), AppLibStillEnc_GetPhotoPjpegConfigId());

    ScrnW = ALIGN_32(StillCapConfigData->ScreennailWidth);
    ScrnH = StillCapConfigData->ScreennailHeight ;
    ThmW = ALIGN_32(StillCapConfigData->ThumbnailWidth);
    ThmH = StillCapConfigData->ThumbnailHeight;
    AmbaPrint("[Applib - StillEnc] <RawEncode> scrn(%d %d) thm(%d %d)!", \
        ScrnW, ScrnH, \
        ThmW, ThmH);

    /* Set the size of Fchan preview window.*/
    PreviewParam.AspectRatio = AppLibSysSensor_GetCaptureModeAR(AppLibStillEnc_GetPhotoPjpegCapMode(), AppLibStillEnc_GetPhotoPjpegConfigId());
    PreviewParam.ChanID = DISP_CH_FCHAN;
    AppLibDisp_CalcPreviewWindowSize(&PreviewParam);
    QvFchanWidth = PreviewParam.Preview.Width;
    QvFchanHeight = PreviewParam.Preview.Height;

    /* Set the size of Dchan preview window.*/
    PreviewParam.AspectRatio = AppLibSysSensor_GetCaptureModeAR(AppLibStillEnc_GetPhotoPjpegCapMode(), AppLibStillEnc_GetPhotoPjpegConfigId());
    PreviewParam.ChanID = DISP_CH_DCHAN;
    AppLibDisp_CalcPreviewWindowSize(&PreviewParam);
    QvDchanWidth = PreviewParam.Preview.Width;
    QvDchanHeight = PreviewParam.Preview.Height;

    QvLcdWidth =ALIGN_32(QvDchanWidth);
    QvLcdHeight = QvDchanHeight;
    QvHdmiWidth = ALIGN_32(QvFchanWidth);
    QvHdmiHeight = QvFchanHeight;
    QvLcdSize = (UINT32) (QvLcdWidth*QvLcdHeight*2);
    QvHdmiSize = (UINT32) (QvHdmiWidth*QvHdmiHeight*2);

    ReturnValue = AmpUtil_GetAlignedPool(APPLIB_G_MMPL, &TempPtrBuf, (void **)&RawEncScriptAddrBufRaw, 128*10, 32); //TBD
    if (ReturnValue != OK) {
        AmbaPrintColor(RED,"[Applib - StillEnc] <RawEncode> NC_DDR alloc scriptAddr fail (%u)!", 128*10);
        return -1;
    } else {
        RawEncScriptAddr = (UINT8*)TempPtrBuf;
        AmbaPrint("[Applib - StillEnc] <RawEncode> scriptAddr (0x%08X) (%d)!", RawEncScriptAddr, 128*10);
        //AmbaPrint("[Applib - StillEnc] <RawEncode> scriptAddr TempPtrBuf(0x%08X) (%u)!", SingleCapScriptAddrBufRaw, 128*10);
    }

    /* Step3. fill script */
    //general config
    StageAddr = RawEncScriptAddr;
    GenScrpt = (AMP_SENC_SCRPT_GENCFG_s *)StageAddr;
    memset(GenScrpt, 0x0, sizeof(AMP_SENC_SCRPT_GENCFG_s));
    GenScrpt->Cmd = SENC_GENCFG;
    GenScrpt->RawEncRepeat = 0;
    GenScrpt->RawToCap = 1;
    if (ItunerInfo.TuningAlgoMode.AlgoMode == 0) {
        GenScrpt->StillProcMode = 2;
    } else if (ItunerInfo.TuningAlgoMode.AlgoMode == 1) {
        GenScrpt->StillProcMode = 1;
    } else {
        GenScrpt->StillProcMode = 0;
    }

    GenScrpt->QVConfig.DisableLCDQV = 0;
    GenScrpt->QVConfig.DisableHDMIQV = 0;
    GenScrpt->QVConfig.LCDDataFormat = AMP_YUV_422;
    GenScrpt->QVConfig.LCDLumaAddr = RawEncQvLCDBuffAddr;
    GenScrpt->QVConfig.LCDChromaAddr = RawEncQvLCDBuffAddr + QvLcdSize/2;
    GenScrpt->QVConfig.LCDWidth = QvLcdWidth;
    GenScrpt->QVConfig.LCDHeight = QvLcdHeight;
    GenScrpt->QVConfig.HDMIDataFormat = AMP_YUV_422;
    GenScrpt->QVConfig.HDMILumaAddr = RawEncQvHDMIBuffAddr;
    GenScrpt->QVConfig.HDMIChromaAddr = RawEncQvHDMIBuffAddr + QvHdmiSize/2;
    GenScrpt->QVConfig.HDMIWidth = QvHdmiWidth;
    GenScrpt->QVConfig.HDMIHeight = QvHdmiHeight;
    GenScrpt->b2LVCfg = AMP_ENC_SCRPT_B2LV_NONE;

    GenScrpt->ScrnEnable = 1;
    GenScrpt->ThmEnable = 1;
    GenScrpt->PreProc = &PreRawEncCB;
    GenScrpt->PostProc = &PostRawEncCB;

    GenScrpt->MainBuf.ColorFmt = AMP_YUV_422;
    GenScrpt->RawDataBits = SystemInfo.RawResolution;
    GenScrpt->RawBayerPattern = SystemInfo.RawBayer;
    GenScrpt->MainBuf.Width = GenScrpt->MainBuf.Pitch = YuvWidth;
    GenScrpt->MainBuf.Height = YuvHeight;
    GenScrpt->MainBuf.LumaAddr = RawEncYuvBuffAddr;
    GenScrpt->MainBuf.ChromaAddr = RawEncYuvBuffAddr + YuvSize/2;;
    GenScrpt->MainBuf.AOI.X = 0;
    GenScrpt->MainBuf.AOI.Y = 0;
    GenScrpt->MainBuf.AOI.Width = SystemInfo.MainWidth;
    GenScrpt->MainBuf.AOI.Height = SystemInfo.MainHeight;

    GenScrpt->ScrnBuf.ColorFmt = AMP_YUV_422;
    GenScrpt->ScrnBuf.Width = GenScrpt->ScrnBuf.Pitch = ScrnW;
    GenScrpt->ScrnBuf.Height = ScrnH;
    GenScrpt->ScrnBuf.LumaAddr = RawEncScrnBuffAddr;
    GenScrpt->ScrnBuf.ChromaAddr = 0;
    GenScrpt->ScrnBuf.AOI.X = 0;
    GenScrpt->ScrnBuf.AOI.Y = 0;
    GenScrpt->ScrnBuf.AOI.Width = StillCapConfigData->ScreennailActiveWidth;
    GenScrpt->ScrnBuf.AOI.Height = StillCapConfigData->ScreennailActiveHeight;
    GenScrpt->ScrnWidth = StillCapConfigData->ScreennailWidth;
    GenScrpt->ScrnHeight = StillCapConfigData->ScreennailHeight;

    GenScrpt->ThmBuf.ColorFmt = AMP_YUV_422;
    GenScrpt->ThmBuf.Width = GenScrpt->ThmBuf.Pitch = ThmW;
    GenScrpt->ThmBuf.Height = ThmH;
    GenScrpt->ThmBuf.LumaAddr = RawEncThmBuffAddr;
    GenScrpt->ThmBuf.ChromaAddr = 0;
    GenScrpt->ThmBuf.AOI.X = 0;
    GenScrpt->ThmBuf.AOI.Y = 0;
    GenScrpt->ThmBuf.AOI.Width = StillCapConfigData->ThumbnailActiveWidth;
    GenScrpt->ThmBuf.AOI.Height = StillCapConfigData->ThumbnailActiveHeight;
    GenScrpt->ThmWidth = StillCapConfigData->ThumbnailWidth;
    GenScrpt->ThmHeight = StillCapConfigData->ThumbnailHeight;

    AppLibStillEnc_initJpegDqt(ApplibJpegQTable[0], AppLibStillEnc_GetQuality());
    AppLibStillEnc_initJpegDqt(ApplibJpegQTable[1], AppLibStillEnc_GetQuality());
    AppLibStillEnc_initJpegDqt(ApplibJpegQTable[2], AppLibStillEnc_GetQuality());
    GenScrpt->BrcCtrl.Tolerance = 0;
    GenScrpt->BrcCtrl.MaxEncLoop = 0;
    GenScrpt->BrcCtrl.JpgBrcCB = NULL;
    GenScrpt->BrcCtrl.TargetBitRate = 0;
    GenScrpt->BrcCtrl.MainQTAddr = ApplibJpegQTable[0];
    GenScrpt->BrcCtrl.ThmQTAddr = ApplibJpegQTable[1];
    GenScrpt->BrcCtrl.ScrnQTAddr = ApplibJpegQTable[2];

    TotalScriptSize += ALIGN_128(sizeof(AMP_SENC_SCRPT_GENCFG_s));
    AmbaPrint("[Applib - StillEnc] <RawEncode> Stage #%d  0x%08X", TotalStageNum, StageAddr);
    TotalStageNum ++;

    if (SystemInfo.EnableRaw2Raw == 1) {
        StageAddr = RawEncScriptAddr + TotalScriptSize;
        Raw2RawScrpt = (AMP_SENC_SCRPT_RAW2RAW_s *)StageAddr;
        memset(Raw2RawScrpt, 0x0, sizeof(AMP_SENC_SCRPT_RAW2RAW_s));
        Raw2RawScrpt->Cmd = SENC_RAW2RAW;
        Raw2RawScrpt->SrcRawType = (SystemInfo.CompressedRaw)? AMP_STILLENC_RAW_COMPR: AMP_STILLENC_RAW_UNCOMPR;
        Raw2RawScrpt->SrcRawBuf.Buf = RawEncRawBuffAddr;
        Raw2RawScrpt->SrcRawBuf.Width = RawWidth;
        Raw2RawScrpt->SrcRawBuf.Height = RawHeight;
        Raw2RawScrpt->SrcRawBuf.Pitch = RawPitch;
        Raw2RawScrpt->TileNumber = 1;//FIXME: temp hack as full frame
        if (Raw2RawScrpt->TileNumber == 1) { //full frame
            AMP_STILLENC_RAW2RAW_ROI_s *roi = (AMP_STILLENC_RAW2RAW_ROI_s *)RawEnc3ARoiBuffAddr;
            roi->RoiColStart = 0;
            roi->RoiRowStart = 0;
            roi->RoiWidth = RawWidth;
            roi->RoiHeight = RawHeight;
        } else if (Raw2RawScrpt->TileNumber == 3) {
            //TBD
        }
        Raw2RawScrpt->TileListAddr = (UINT32)RawEnc3ARoiBuffAddr;
        Raw2RawScrpt->Raw3AStatAddr = (UINT32)RawEnc3AStatBuffAddr;
        Raw2RawScrpt->Raw3AStatSize = sizeof(AMBA_DSP_EVENT_STILL_CFA_3A_DATA_s);
        TotalScriptSize += ALIGN_128(sizeof(AMP_SENC_SCRPT_RAW2RAW_s));
        AmbaPrint("[Applib - StillEnc] <SingleCaptureCont> Stage #%d  0x%08X", TotalStageNum, StageAddr);
        TotalStageNum ++;

    } else {
        //raw2yuv config
        StageAddr = RawEncScriptAddr + TotalScriptSize;
        Raw2YuvScrpt = (AMP_SENC_SCRPT_RAW2YUV_s *)StageAddr;
        memset(Raw2YuvScrpt, 0x0, sizeof(AMP_SENC_SCRPT_RAW2YUV_s));
        Raw2YuvScrpt->Cmd = SENC_RAW2YUV;
        Raw2YuvScrpt->RawType = (SystemInfo.CompressedRaw)?AMP_STILLENC_RAW_COMPR: AMP_STILLENC_RAW_UNCOMPR;
        Raw2YuvScrpt->RawBuf.Buf = RawEncRawBuffAddr;
        Raw2YuvScrpt->RawBuf.Width = RawWidth;
        Raw2YuvScrpt->RawBuf.Height = RawHeight;
        Raw2YuvScrpt->RawBuf.Pitch = RawPitch;
        Raw2YuvScrpt->RawBufRule = AMP_ENC_SCRPT_BUFF_FIXED;
        Raw2YuvScrpt->RingBufSize = 0;
        Raw2YuvScrpt->YuvBufRule = AMP_ENC_SCRPT_BUFF_FIXED;
        Raw2YuvScrpt->YuvRingBufSize = 0;
        TotalScriptSize += ALIGN_128(sizeof(AMP_SENC_SCRPT_RAW2YUV_s));
        AmbaPrint("[Applib - StillEnc] <RawEncode> Stage #%d  0x%08X", TotalStageNum, StageAddr);
        TotalStageNum ++;

        //yuv2jpg config
        StageAddr = RawEncScriptAddr + TotalScriptSize;
        Yuv2JpgScrpt = (AMP_SENC_SCRPT_YUV2JPG_s *)StageAddr;
        memset(Yuv2JpgScrpt, 0x0, sizeof(AMP_SENC_SCRPT_YUV2JPG_s));
        Yuv2JpgScrpt->Cmd = SENC_YUV2JPG;
        Yuv2JpgScrpt->YuvBufRule = AMP_ENC_SCRPT_BUFF_FIXED;
        Yuv2JpgScrpt->YuvRingBufSize = 0;
        TotalScriptSize += ALIGN_128(sizeof(AMP_SENC_SCRPT_YUV2JPG_s));
        AmbaPrint("[Applib - StillEnc] <RawEncode> Stage #%d  0x%08X", TotalStageNum, StageAddr);
        TotalStageNum ++;
    }

    //script config
    Scrpt.mode = AMP_SCRPT_MODE_STILL;
    Scrpt.StepPreproc = NULL;
    Scrpt.StepPostproc = NULL;
    Scrpt.ScriptStartAddr = (UINT32)RawEncScriptAddr;
    Scrpt.ScriptTotalSize = TotalScriptSize;
    Scrpt.ScriptStageNum = TotalStageNum;
    AmbaPrint("[Applib - StillEnc] <RawEncode> Scrpt addr 0x%X, Sz %uByte, stg %d", Scrpt.ScriptStartAddr, Scrpt.ScriptTotalSize, Scrpt.ScriptStageNum);

    //set still idsp param
    {
        AMBA_DSP_IMG_CTX_INFO_s DestCtx, SrcCtx;
        AMBA_DSP_IMG_CFG_INFO_s CfgInfo;
        AMBA_ITUNER_PROC_INFO_s ProcInfo;
        AMBA_DSP_IMG_WARP_CALC_INFO_s CalcWarp;
        AMBA_DSP_IMG_OUT_WIN_INFO_s ImgOutputWin = {0};
        AMBA_DSP_IMG_SIZE_INFO_s SizeInfo;

        //Initialize the context of ImageKernel of still
        DestCtx.Pipe = AMBA_DSP_IMG_PIPE_STILL;
        DestCtx.CtxId = 0;
        SrcCtx.CtxId = 0;

        //Initialize the configure of ImageKernel of Still
        CfgInfo.Pipe = AMBA_DSP_IMG_PIPE_STILL;
        CfgInfo.CfgId = 0;
        AmbaDSP_ImgInitCfg(&CfgInfo, ItunerInfo.TuningAlgoMode.AlgoMode);

        AmbaPrint("[Applib - StillEnc] <RawEncode> AlgoMode(%d %d %d %d %d %d)", \
                     ItunerInfo.TuningAlgoMode.Pipe,
                     ItunerInfo.TuningAlgoMode.AlgoMode,
                     ItunerInfo.TuningAlgoMode.FuncMode,
                     ItunerInfo.TuningAlgoMode.BatchId,
                     ItunerInfo.TuningAlgoMode.ConfigId,
                     ItunerInfo.TuningAlgoMode.ContextId);

        ImgOutputWin.MainWinDim.Width = SystemInfo.MainWidth;
        ImgOutputWin.MainWinDim.Height = SystemInfo.MainHeight;
        ImgOutputWin.ScreennailDim.Width = StillCapConfigData->ScreennailActiveWidth;
        ImgOutputWin.ScreennailDim.Height = StillCapConfigData->ScreennailActiveHeight;
        ImgOutputWin.ThumbnailDim.Width = StillCapConfigData->ThumbnailActiveWidth;
        ImgOutputWin.ThumbnailDim.Height = StillCapConfigData->ThumbnailActiveHeight;
        ImgOutputWin.PrevWinDim[0].Width = QvLcdWidth;
        ImgOutputWin.PrevWinDim[0].Height = QvLcdHeight;
        ImgOutputWin.PrevWinDim[1].Width  = QvHdmiWidth;
        ImgOutputWin.PrevWinDim[1].Height = QvHdmiHeight;

        AmbaDSP_WarpCore_SetOutputWin(&ItunerInfo.TuningAlgoMode, &ImgOutputWin);

        memset(&ProcInfo, 0, sizeof(AMBA_ITUNER_PROC_INFO_s));
        ProcInfo.HisoBatchId = AMBA_DSP_STILL_HISO_FILTER;
        AmbaTUNE_Execute_IDSP(&ItunerInfo.TuningAlgoMode, &ProcInfo);

        memset(&CalcWarp, 0x0, sizeof(AMBA_DSP_IMG_WARP_CALC_INFO_s));
        AmbaDSP_ImgGetWarpCompensation(&ItunerInfo.TuningAlgoMode, &CalcWarp);

        memset(&SizeInfo, 0, sizeof(AMBA_DSP_IMG_SIZE_INFO_s));
        SizeInfo.WidthIn     = ((CalcWarp.ActWinCrop.RightBotX - CalcWarp.ActWinCrop.LeftTopX + 0xFFFF)>>16);
        SizeInfo.HeightIn    = ((CalcWarp.ActWinCrop.RightBotY - CalcWarp.ActWinCrop.LeftTopY + 0xFFFF)>>16);
        SizeInfo.WidthMain   = SystemInfo.MainWidth;
        SizeInfo.HeightMain  = SystemInfo.MainHeight;
        SizeInfo.WidthPrevA  = QvLcdWidth;
        SizeInfo.HeightPrevA = QvLcdHeight;
        SizeInfo.WidthPrevB  = QvHdmiWidth;
        SizeInfo.HeightPrevB = QvHdmiHeight;
        SizeInfo.WidthScrn = ScrnW;
        SizeInfo.HeightScrn = ScrnH;
        AmbaDSP_ImgSetSizeInfo(&ItunerInfo.TuningAlgoMode, &SizeInfo);

        AmbaDSP_ImgPostExeCfg(&ItunerInfo.TuningAlgoMode, AMBA_DSP_IMG_CFG_EXE_FULLCOPY);
    }

    /* Step4. execute script */
    ReturnValue = AmpEnc_RunScript(StillEncPipe, &Scrpt, AMP_ENC_FUNC_FLAG_NONE);
    if (ReturnValue < 0) {
        AmbaPrintColor(RED,"[Applib - StillEnc] <RawEncode> AmpEnc_RunScript Fail ");
    }

    /* Step4. release script */
    AmbaPrint("[Applib - StillEnc] <RawEncode> [0x%08X] memFree", RawEncScriptAddrBufRaw);
    if (AmbaKAL_BytePoolFree((void *)RawEncScriptAddrBufRaw) != OK) {
        AmbaPrintColor(RED,"[Applib - StillEnc] <RawEncode> memFree Fail (scrpt)");
    }
    RawEncScriptAddrBufRaw = NULL;

    AmbaPrint("[Applib - StillEnc] <RawEncode> memFree Done");
    RawEncFinishFlag = 1;
    while (RawEncFinishFlag == 1) {
        AmbaKAL_TaskSleep(100);
    }
    RawEncNeedPreLoad = 0;
    RawEncPreLoadDone = 0;
    return 0;
}


