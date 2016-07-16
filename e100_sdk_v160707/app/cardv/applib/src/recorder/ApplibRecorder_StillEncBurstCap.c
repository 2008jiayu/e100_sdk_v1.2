/**
 * @file src/app/connected/applib/src/recorder/ApplibRecorder_StillEncBurstCap.c
 *
 * Implementation of burst capture.
 *
 * History:
 *    2013/12/30 - [Martin Lai] created file
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
#include <AmbaCache.h>


//#define DEBUG_APPLIB_PHOTO_BURST_CAP
#if defined(DEBUG_APPLIB_PHOTO_BURST_CAP)
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

/* Burst capture buffers */
#ifdef _STILL_BUFFER_FROM_DSP_
static UINT8 *BurstCapDstRawBuffAddr = NULL;
static UINT8 *BurstCapDstRaw3ARoiBuffAddr = NULL;
static UINT8 *BurstCapDstRaw3AStatBuffAddr = NULL;
#else
static UINT8 *BurstCapScriptAddrBufRaw = NULL;
static UINT8 *BurstCapScriptAddr = NULL;
static UINT8 *BurstCapRawBuffAddrBufRaw = NULL;
static UINT8 *BurstCapRawBuffAddr = NULL;
static UINT8 *BurstCapYuvBuffAddrBufRaw = NULL;
static UINT8 *BurstCapYuvBuffAddr = NULL;
static UINT8 *BurstCapScrnBuffAddrBufRaw = NULL;
static UINT8 *BurstCapScrnBuffAddr = NULL;
static UINT8 *BurstCapThmBuffAddrBufRaw = NULL;
static UINT8 *BurstCapThmBuffAddr = NULL;
static UINT8 *BurstCapQvLCDBuffAddrBufRaw = NULL;
static UINT8 *BurstCapQvLCDBuffAddr = NULL;
static UINT8 *BurstCapQvHDMIBuffAddrBufRaw = NULL;
static UINT8 *BurstCapQvHDMIBuffAddr = NULL;
#endif

static int BurstCapAllocateFromDSPFlag = 0;
static APPLIB_STILLENC_STAMPP_s StampBurstCapCB = {.StampInfo = {0}, .Process = NULL};

UINT32 AppLibStillEnc_BurstCaptureStampCB(UINT8 type, UINT8 *yAddr, UINT8 *uvAddr, UINT16 width, UINT16 height, UINT16 pitch)
{
    UINT32 bufSize = 0;
    UINT8 *startAddrY_target, *startAddrUV_target;
    UINT8 *startAddrY_source, *startAddrUV_source;
    UINT8 *startAddrAlphaY;
    int line = 0, raw = 0;
    UINT8 *stampPositionY = NULL, *stampPositionUV = NULL;  // for cache clean use

    switch (type) {
        case 0:  // main
#ifdef CONFIG_APP_ARD        
		if(StampBurstCapCB.StampInfo.StampAreaEn[0] == 1){

            bufSize = StampBurstCapCB.StampInfo.StampAreaInfo[0].Width * StampBurstCapCB.StampInfo.StampAreaInfo[0].Height;

            for(line=0; line<StampBurstCapCB.StampInfo.StampAreaInfo[0].Height; line++) {
                stampPositionY = ((UINT8*)((UINT32)yAddr + (line+StampBurstCapCB.StampInfo.StampAreaInfo[0].OffsetY)*pitch)) + StampBurstCapCB.StampInfo.StampAreaInfo[0].OffsetX;
                stampPositionUV = ((UINT8*)((UINT32)uvAddr + (line+StampBurstCapCB.StampInfo.StampAreaInfo[0].OffsetY)*pitch)) + StampBurstCapCB.StampInfo.StampAreaInfo[0].OffsetX;

                for (raw=0; raw<StampBurstCapCB.StampInfo.StampAreaInfo[0].Width; raw++) {
                    startAddrY_target = ((UINT8*)((UINT32)yAddr + (line+StampBurstCapCB.StampInfo.StampAreaInfo[0].OffsetY)*pitch)) + StampBurstCapCB.StampInfo.StampAreaInfo[0].OffsetX + raw;
                    startAddrUV_target = ((UINT8*)((UINT32)uvAddr + (line+StampBurstCapCB.StampInfo.StampAreaInfo[0].OffsetY)*pitch)) + StampBurstCapCB.StampInfo.StampAreaInfo[0].OffsetX +raw;
                    startAddrY_source = (UINT8*)((UINT32)StampBurstCapCB.StampInfo.StampAreaInfo[0].YAddr + line * StampBurstCapCB.StampInfo.StampAreaInfo[0].Width) + raw;
                    startAddrUV_source = (UINT8*)((UINT32)StampBurstCapCB.StampInfo.StampAreaInfo[0].UVAddr + line * StampBurstCapCB.StampInfo.StampAreaInfo[0].Width) + raw;
                    startAddrAlphaY = (UINT8*)((UINT32)StampBurstCapCB.StampInfo.StampAreaInfo[0].AlphaYAddr + line * StampBurstCapCB.StampInfo.StampAreaInfo[0].Width) + raw;

                    if (*startAddrAlphaY == 0x0) {
                        *startAddrY_target = *startAddrY_source;
                        *startAddrUV_target = *startAddrUV_source;
                    }
                }
                AmbaCache_Clean(stampPositionY, StampBurstCapCB.StampInfo.StampAreaInfo[0].Width);
                AmbaCache_Clean(stampPositionUV, StampBurstCapCB.StampInfo.StampAreaInfo[0].Width);
            }
		}	
		if(StampBurstCapCB.StampInfo.StampAreaEn[1] == 1){

            bufSize = StampBurstCapCB.StampInfo.StampAreaInfo[1].Width * StampBurstCapCB.StampInfo.StampAreaInfo[1].Height;

            for(line=0; line<StampBurstCapCB.StampInfo.StampAreaInfo[1].Height; line++) {
                stampPositionY = ((UINT8*)((UINT32)yAddr + (line+StampBurstCapCB.StampInfo.StampAreaInfo[1].OffsetY)*pitch)) + StampBurstCapCB.StampInfo.StampAreaInfo[1].OffsetX;
                stampPositionUV = ((UINT8*)((UINT32)uvAddr + (line+StampBurstCapCB.StampInfo.StampAreaInfo[1].OffsetY)*pitch)) + StampBurstCapCB.StampInfo.StampAreaInfo[1].OffsetX;

                for (raw=0; raw<StampBurstCapCB.StampInfo.StampAreaInfo[1].Width; raw++) {
                    startAddrY_target = ((UINT8*)((UINT32)yAddr + (line+StampBurstCapCB.StampInfo.StampAreaInfo[1].OffsetY)*pitch)) + StampBurstCapCB.StampInfo.StampAreaInfo[1].OffsetX + raw;
                    startAddrUV_target = ((UINT8*)((UINT32)uvAddr + (line+StampBurstCapCB.StampInfo.StampAreaInfo[1].OffsetY)*pitch)) + StampBurstCapCB.StampInfo.StampAreaInfo[1].OffsetX +raw;
                    startAddrY_source = (UINT8*)((UINT32)StampBurstCapCB.StampInfo.StampAreaInfo[1].YAddr + line * StampBurstCapCB.StampInfo.StampAreaInfo[1].Width) + raw;
                    startAddrUV_source = (UINT8*)((UINT32)StampBurstCapCB.StampInfo.StampAreaInfo[1].UVAddr + line * StampBurstCapCB.StampInfo.StampAreaInfo[1].Width) + raw;
                    startAddrAlphaY = (UINT8*)((UINT32)StampBurstCapCB.StampInfo.StampAreaInfo[1].AlphaYAddr + line * StampBurstCapCB.StampInfo.StampAreaInfo[1].Width) + raw;

                    if (*startAddrAlphaY == 0x0) {
                        *startAddrY_target = *startAddrY_source;
                        *startAddrUV_target = *startAddrUV_source;
                    }
                }
                AmbaCache_Clean(stampPositionY, StampBurstCapCB.StampInfo.StampAreaInfo[1].Width);
                AmbaCache_Clean(stampPositionUV, StampBurstCapCB.StampInfo.StampAreaInfo[1].Width);
            }
		}	
		if(StampBurstCapCB.StampInfo.StampAreaEn[2] == 1){

            bufSize = StampBurstCapCB.StampInfo.StampAreaInfo[2].Width * StampBurstCapCB.StampInfo.StampAreaInfo[2].Height;

            for(line=0; line<StampBurstCapCB.StampInfo.StampAreaInfo[2].Height; line++) {
                stampPositionY = ((UINT8*)((UINT32)yAddr + (line+StampBurstCapCB.StampInfo.StampAreaInfo[2].OffsetY)*pitch)) + StampBurstCapCB.StampInfo.StampAreaInfo[2].OffsetX;
                stampPositionUV = ((UINT8*)((UINT32)uvAddr + (line+StampBurstCapCB.StampInfo.StampAreaInfo[2].OffsetY)*pitch)) + StampBurstCapCB.StampInfo.StampAreaInfo[2].OffsetX;

                for (raw=0; raw<StampBurstCapCB.StampInfo.StampAreaInfo[2].Width; raw++) {
                    startAddrY_target = ((UINT8*)((UINT32)yAddr + (line+StampBurstCapCB.StampInfo.StampAreaInfo[2].OffsetY)*pitch)) + StampBurstCapCB.StampInfo.StampAreaInfo[2].OffsetX + raw;
                    startAddrUV_target = ((UINT8*)((UINT32)uvAddr + (line+StampBurstCapCB.StampInfo.StampAreaInfo[2].OffsetY)*pitch)) + StampBurstCapCB.StampInfo.StampAreaInfo[2].OffsetX +raw;
                    startAddrY_source = (UINT8*)((UINT32)StampBurstCapCB.StampInfo.StampAreaInfo[2].YAddr + line * StampBurstCapCB.StampInfo.StampAreaInfo[2].Width) + raw;
                    startAddrUV_source = (UINT8*)((UINT32)StampBurstCapCB.StampInfo.StampAreaInfo[2].UVAddr + line * StampBurstCapCB.StampInfo.StampAreaInfo[2].Width) + raw;
                    startAddrAlphaY = (UINT8*)((UINT32)StampBurstCapCB.StampInfo.StampAreaInfo[2].AlphaYAddr + line * StampBurstCapCB.StampInfo.StampAreaInfo[2].Width) + raw;

                    if (*startAddrAlphaY == 0x0) {
                        *startAddrY_target = *startAddrY_source;
                        *startAddrUV_target = *startAddrUV_source;
                    }
                }
                AmbaCache_Clean(stampPositionY, StampBurstCapCB.StampInfo.StampAreaInfo[2].Width);
                AmbaCache_Clean(stampPositionUV, StampBurstCapCB.StampInfo.StampAreaInfo[2].Width);
            }
		}			
#else			
            bufSize = StampBurstCapCB.StampInfo.Width * StampBurstCapCB.StampInfo.Height;

            for(line=0; line<StampBurstCapCB.StampInfo.Height; line++) {
                stampPositionY = ((UINT8*)((UINT32)yAddr + (line+StampBurstCapCB.StampInfo.OffsetY)*pitch)) + StampBurstCapCB.StampInfo.OffsetX;
                stampPositionUV = ((UINT8*)((UINT32)uvAddr + (line+StampBurstCapCB.StampInfo.OffsetY)*pitch)) + StampBurstCapCB.StampInfo.OffsetX;

                for (raw=0; raw<StampBurstCapCB.StampInfo.Width; raw++) {
                    startAddrY_target = ((UINT8*)((UINT32)yAddr + (line+StampBurstCapCB.StampInfo.OffsetY)*pitch)) + StampBurstCapCB.StampInfo.OffsetX + raw;
                    startAddrUV_target = ((UINT8*)((UINT32)uvAddr + (line+StampBurstCapCB.StampInfo.OffsetY)*pitch)) + StampBurstCapCB.StampInfo.OffsetX +raw;
                    startAddrY_source = (UINT8*)((UINT32)StampBurstCapCB.StampInfo.YAddr + line * StampBurstCapCB.StampInfo.Width) + raw;
                    startAddrUV_source = (UINT8*)((UINT32)StampBurstCapCB.StampInfo.UVAddr + line * StampBurstCapCB.StampInfo.Width) + raw;
                    startAddrAlphaY = (UINT8*)((UINT32)StampBurstCapCB.StampInfo.AlphaYAddr + line * StampBurstCapCB.StampInfo.Width) + raw;

                    if (*startAddrAlphaY == 0x0) {
                        *startAddrY_target = *startAddrY_source;
                        *startAddrUV_target = *startAddrUV_source;
                    }
                }
                AmbaCache_Clean(stampPositionY, StampBurstCapCB.StampInfo.Width);
                AmbaCache_Clean(stampPositionUV, StampBurstCapCB.StampInfo.Width);
            }		
#endif			
            break;
        case 1:  // screen
            break;
        case 2:  // thumb
            break;
        default:
            break;
    }
    return 0;
}

//static UINT8 G_raw_cmpr = 0;
//static UINT8 qvDisplayCfg = 0;

/**
 *  @brief Burst Capture PreProc Callback
 *
 *  Stage1: RAWCAP  -> Nothing to do
 *  Stage2: RAW2YUV  -> setup r2y idsp cfg
 *
 *  @param [in] info preproc information
 *
 *  @return 0 - success, -1 - fail
 */
UINT32 AppLibStillEnc_BurstCapturePreCB(AMP_STILLENC_PREP_INFO_s *info)
{
    if (info->StageCnt == 2) {
        AppLibStillEnc_IdspParamSetup(info->AeIdx);
    }

    if (info->StageCnt == 3) {
        AmbaPrint("[Applib - StillEnc] <BurstCapture> Pre-CallBack: SerialNum %d", info->JpegSerialNumber);
    }
    return 0;
}
static AMP_STILLENC_PREP_s PreBurstCapCB = {.Process = AppLibStillEnc_BurstCapturePreCB};

#if defined(DEBUG_APPLIB_PHOTO_BURST_CAP)
static UINT16 raw_fno = 1;
#endif
static UINT16 yuv_fno = 1;
static UINT8 G_capcnt = 1;
/**
 *  @brief  Burst capture PostProc Callback
 *
 *  Stage1: RAWCAP  -> Dump raw
 *  Stage2: RAW2YUV -> Dump yuv
 *
 *  @param [in] info postproce information
 *
 *  @return 0 - success, -1 - fail
 */
UINT32 AppLibStillEnc_BurstCapturePostCB(AMP_STILLENC_POSTP_INFO_s *info)
{
    static UINT8 yuvFlag = 0;
    static UINT8 captureCount =0;

    if (info->StageCnt == 1) {
#if defined(DEBUG_APPLIB_PHOTO_BURST_CAP)
        char fn[64];
        WCHAR fnu[64];
        WCHAR mode[3] = {'w','b','\0'};
        AMBA_FS_FILE *raw = NULL;

        yuvFlag = 0;

        //raw ready, dump it
        sprintf(fn,"C:\\%04d.RAW", raw_fno);
        AmbaUtility_Ascii2Unicode(fn, fnu);

        raw = AmbaFS_fopen((char const *)fnu,(char const *) mode);
        AmbaPrint("[Applib - StillEnc] <BurstCapture> Post-CallBack: Dump Raw 0x%X %d %d %d  to %s Start!", \
            info->media.RawInfo.RawAddr, \
            info->media.RawInfo.RawPitch, \
            info->media.RawInfo.RawWidth, \
            info->media.RawInfo.RawHeight, fn);
        AmbaFS_fwrite(info->media.RawInfo.RawAddr, \
            info->media.RawInfo.RawPitch*info->media.RawInfo.RawHeight, 1, raw);
        AmbaFS_FSync(raw);
        AmbaFS_fclose(raw);
        raw_fno++;
#endif

        /* To fill EXIF tags. */
        {
            COMPUTE_EXIF_PARAMS_s ExifParam = {0};
            EXIF_INFO_s ExifInfo = {0};
            APPLIB_EXIF_DATA_s ExifData = {0};

            ExifParam.AeIdx = 0; //TBD
            ExifParam.Mode = IMG_EXIF_STILL;
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
            AppLibFormatMuxExif_ConfigExifTag(&ExifData);

            ExifData.TagId = APPLIB_EXIF_TAG_ISOSpeed;
            ExifData.DataType = APPLIB_TAG_TYPE_LONG;
            ExifData.DataLength = 1 * sizeof(UINT32);
            ExifData.Value = ExifInfo.IsoSpeedRating;
            AppLibFormatMuxExif_ConfigExifTag(&ExifData);

            // Exposure time
            ExifData.TagId = APPLIB_EXIF_TAG_ExposureTime;
            ExifData.DataType = APPLIB_TAG_TYPE_RATIONAL;
            ExifData.DataLength = 8 * sizeof(UINT8);
            ExifData.Value = ((UINT64)ExifInfo.ExposureTimeNum << 32) | ExifInfo.ExposureTimeDen;
            AppLibFormatMuxExif_ConfigExifTag(&ExifData);

            ApplibFormatMuxExif_IncreaseRawCount();
        }
    } else if (info->StageCnt == 2) {
        UINT8 type = 0;
        UINT8 *LumaAddr = NULL, *ChromaAddr = NULL;
        UINT16 Pitch = 0;
        UINT16 Width = 0, Height = 0;
        if (info->media.YuvInfo.ThmLumaAddr) {
            LumaAddr = info->media.YuvInfo.ThmLumaAddr;
            ChromaAddr = info->media.YuvInfo.ThmChromaAddr;
            Pitch = info->media.YuvInfo.ThmPitch;
            Width = info->media.YuvInfo.ThmWidth;
            Height = info->media.YuvInfo.ThmHeight;
            type = 2;
        } else if (info->media.YuvInfo.ScrnLumaAddr) {
            LumaAddr = info->media.YuvInfo.ScrnLumaAddr;
            ChromaAddr = info->media.YuvInfo.ScrnChromaAddr;
            Pitch = info->media.YuvInfo.ScrnPitch;
            Width = info->media.YuvInfo.ScrnWidth;
            Height = info->media.YuvInfo.ScrnHeight;
            type = 1;
        } else if (info->media.YuvInfo.LumaAddr) {
            LumaAddr = info->media.YuvInfo.LumaAddr;
            ChromaAddr = info->media.YuvInfo.ChromaAddr;
            Pitch = info->media.YuvInfo.Pitch;
            Width = info->media.YuvInfo.Width;
            Height = info->media.YuvInfo.Height;
            type = 0;
        }

        // photo stamp
        if (StampBurstCapCB.Process != NULL) {
            if ((LumaAddr != NULL) && (ChromaAddr != NULL)){
                StampBurstCapCB.Process(type, LumaAddr, ChromaAddr, Width, Height, Pitch);
            }
        }
#if defined(DEBUG_APPLIB_PHOTO_BURST_CAP)

        char fn[32];
        char fn1[32];
        WCHAR fnu[32];
        WCHAR fnu1[32];
        WCHAR mode[3] = {'w','b','\0'};
        UINT8 *LumaAddr, *ChromaAddr;
        UINT16 Pitch, Width, Height;

        if (info->media.YuvInfo.ThmLumaAddr) {
            sprintf(fn,"C:\\%04d_t.y", yuv_fno);
            sprintf(fn1,"C:\\%04d_t.uv", yuv_fno);
            LumaAddr = info->media.YuvInfo.ThmLumaAddr;
            ChromaAddr = info->media.YuvInfo.ThmChromaAddr;
            Pitch = info->media.YuvInfo.ThmPitch;
            Width = info->media.YuvInfo.ThmWidth;
            Height = info->media.YuvInfo.ThmHeight;
        } else if (info->media.YuvInfo.ScrnLumaAddr) {
            sprintf(fn,"C:\\%04d_s.y", yuv_fno);
            sprintf(fn1,"C:\\%04d_s.uv", yuv_fno);
            LumaAddr = info->media.YuvInfo.ScrnLumaAddr;
            ChromaAddr = info->media.YuvInfo.ScrnChromaAddr;
            Pitch = info->media.YuvInfo.ScrnPitch;
            Width = info->media.YuvInfo.ScrnWidth;
            Height = info->media.YuvInfo.ScrnHeight;
        } else if (info->media.YuvInfo.LumaAddr) {
            sprintf(fn,"C:\\%04d_m.y", yuv_fno);
            sprintf(fn1,"C:\\%04d_m.uv", yuv_fno);
            LumaAddr = info->media.YuvInfo.LumaAddr;
            ChromaAddr = info->media.YuvInfo.ChromaAddr;
            Pitch = info->media.YuvInfo.Pitch;
            Width = info->media.YuvInfo.Width;
            Height = info->media.YuvInfo.Height;
        }
        AmbaUtility_Ascii2Unicode(fn, fnu);
        AmbaUtility_Ascii2Unicode(fn1, fnu1);

        AmbaPrint("[Applib - StillEnc] <BurstCapture> Post-CallBack:  Dump YUV(%d) (0x%X 0x%X) %d %d %d Start!", \
            info->media.YuvInfo.DataFormat, \
            LumaAddr, ChromaAddr, Pitch, Width, Height);
        {
            AMBA_FS_FILE *y = NULL;
            AMBA_FS_FILE *uv = NULL;
            UINT16 i;
            y = AmbaFS_fopen((char const *)fnu,(char const *) mode);
            for(i=0; i<Height; i++) {
                AmbaFS_fwrite(LumaAddr, Width, 1, y);
                LumaAddr += Pitch;
            }
            AmbaFS_FSync(y);
            AmbaFS_fclose(y);

            uv = AmbaFS_fopen((char const *)fnu1,(char const *) mode);
            for(i=0; i<Height; i++) {
                AmbaFS_fwrite(ChromaAddr, Width, 1, uv);
                ChromaAddr += Pitch;
            }
            AmbaFS_FSync(uv);
            AmbaFS_fclose(uv);
        }
#endif
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
            //release raw buffers
            captureCount+=1;
            if (captureCount == G_capcnt) {
                captureCount = 0;
                if (BurstCapRawBuffAddrBufRaw) {
                    if (AmbaKAL_BytePoolFree((void *)BurstCapRawBuffAddrBufRaw) != OK)
                        AmbaPrintColor(RED, "[Applib - StillEnc] <BurstCapture> Post-CallBack: MemFree Fail raw!");
                    BurstCapRawBuffAddrBufRaw = NULL;
                }
            }
            yuv_fno++;
        }
    } else {
        //nothing to do
    }

    return 0;
}

static AMP_STILLENC_POSTP_s PostBurstCapCB = {.Process = AppLibStillEnc_BurstCapturePostCB};

/**
 * Next frame sensor preparation
 *
 * @param [in] vin channel
 * @param [in] ae index
 *
 * @return 0 - success, -1 - fail
 */
UINT32  AppLibStillEnc_BurstCapSensorPrep(AMBA_DSP_CHANNEL_ID_u vinChan,UINT32 aeIdx)
{
    UINT32 rval = 0;
    UINT32 imgIpChNo = 0;
    AMBA_AE_INFO_s stillAeInfo[MAX_AEB_NUM];
    UINT32 gainFactor = 0;
    UINT32 aGainCtrl = 0;
    UINT32 dGainCtrl = 0;
    UINT32 shutterCtrl = 0;
    AMBA_SENSOR_MODE_INFO_s SensorModeInfo;
    AMBA_SENSOR_MODE_ID_u StillModeId;
    APPLIB_SENSOR_STILLCAP_MODE_CONFIG_s *pMode = NULL;
    UINT8 exposureFrames = 0;
    aeIdx = 0;/////temp

    AmbaImg_Proc_Cmd(MW_IP_GET_AE_INFO, imgIpChNo, IP_MODE_STILL, (UINT32)stillAeInfo);

    pMode = AppLibSysSensor_GetStillCaptureModeConfig(AppLibStillEnc_GetPhotoPjpegCapMode(), AppLibStillEnc_GetPhotoPjpegConfigId());
    memset(&StillModeId, 0x0, sizeof(AMBA_SENSOR_MODE_ID_u));
    StillModeId.Data = pMode->ModeId;
    AmbaSensor_GetModeInfo(vinChan, StillModeId, &SensorModeInfo);

    AmbaSensor_ConvertGainFactor(vinChan, stillAeInfo[aeIdx].AgcGain, &gainFactor, &aGainCtrl, &dGainCtrl);
    AmbaSensor_SetAnalogGainCtrl(vinChan, aGainCtrl);
    AmbaSensor_SetDigitalGainCtrl(vinChan, dGainCtrl);
    AmbaSensor_ConvertShutterSpeed(vinChan, stillAeInfo[aeIdx].ShutterTime, &shutterCtrl);
    exposureFrames = (shutterCtrl/SensorModeInfo.FrameLengthLines);
    exposureFrames = (shutterCtrl%SensorModeInfo.FrameLengthLines)? exposureFrames+1: exposureFrames;
    AmbaSensor_SetSlowShutterCtrl(vinChan, exposureFrames);
    AmbaSensor_SetShutterCtrl(vinChan, shutterCtrl);
    //AmbaPrintColor(6,"aeidx (%d) agc gain=%f, shutter=%f",
                //   aeIdx,stillAeInfo[aeIdx].AgcGain,stillAeInfo[aeIdx].ShutterTime);
    return rval;
}

int AppLibStillEnc_BurstCapFreeBuf(void)
{
    int ReturnValue = 0;
    if (BurstCapYuvBuffAddrBufRaw) {
        if (AmbaKAL_BytePoolFree((void *)BurstCapYuvBuffAddrBufRaw) != OK)
            AmbaPrintColor(RED, "[Applib - StillEnc] <BurstCapture> Post-CallBack: MemFree Fail YuvBuff!");
        BurstCapYuvBuffAddrBufRaw = NULL;
        memset((void*)&StampBurstCapCB.StampInfo, 0x0, sizeof(APPLIB_STILLENC_STAMP_SETTING_s));
        StampBurstCapCB.Process = NULL;
    }
    if (BurstCapScrnBuffAddrBufRaw) {
        if (AmbaKAL_BytePoolFree((void *)BurstCapScrnBuffAddrBufRaw) != OK)
            AmbaPrintColor(RED, "[Applib - StillEnc] <BurstCapture> Post-CallBack: MemFree Fail ScrnBuff!");
        BurstCapScrnBuffAddrBufRaw = NULL;
    }
    if (BurstCapThmBuffAddrBufRaw) {
        if (AmbaKAL_BytePoolFree((void *)BurstCapThmBuffAddrBufRaw) != OK)
            AmbaPrintColor(RED, "[Applib - StillEnc] <BurstCapture> Post-CallBack: MemFree Fail ThmBuff!");
        BurstCapThmBuffAddrBufRaw = NULL;
    }
    if (BurstCapQvLCDBuffAddrBufRaw) {
        if (AmbaKAL_BytePoolFree((void *)BurstCapQvLCDBuffAddrBufRaw) != OK)
            AmbaPrintColor(RED, "[Applib - StillEnc] <BurstCapture> Post-CallBack: MemFree Fail QvLCDBuff!");
        BurstCapQvLCDBuffAddrBufRaw = NULL;
    }
    if (BurstCapQvHDMIBuffAddrBufRaw) {
        if (AmbaKAL_BytePoolFree((void *)BurstCapQvHDMIBuffAddrBufRaw) != OK)
            AmbaPrintColor(RED, "[Applib - StillEnc] <BurstCapture> Post-CallBack: MemFree Fail QvHDMI!");
        BurstCapQvHDMIBuffAddrBufRaw = NULL;
    }
    if (BurstCapAllocateFromDSPFlag) {
        AppLibComSvcMemMgr_FreeDSPMemory();
        BurstCapAllocateFromDSPFlag = 0;
    }
     return ReturnValue;
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
 *
 *  @return >=0 success, <0 failure
 */
int AppLibStillEnc_BurstCapture(UINT32 iso, UINT32 cmpr, UINT32 targetSize, UINT8 encodeLoop)
{
    int ReturnValue;
    void *TempPtrBuf;
    UINT16 RawPitch = 0, RawWidth = 0, RawHeight = 0;
    UINT16 YuvWidth = 0, YuvHeight = 0, ScrnW = 0, ScrnH = 0, ThmW = 0, ThmH = 0;
    UINT32 RawSize = 0, YuvSize = 0, ScrnSize = 0, ThmSize = 0;
    UINT16 QvLCDW = 0, QvLCDH = 0, QvHDMIW = 0, QvHDMIH = 0;
    UINT32 QvLCDSize = 0, QvHDMISize = 0;
    UINT8 *SstageAddr = NULL;
    UINT32 TotalScriptSize = 0, TotalStageNum = 0;
    AMP_SENC_SCRPT_GENCFG_s *GenScrpt;
    AMP_SENC_SCRPT_RAWCAP_s *RawCapScrpt;
    AMP_SENC_SCRPT_RAW2YUV_s *Raw2YuvScrpt;
    AMP_SENC_SCRPT_YUV2JPG_s *Yuv2JpgScrpt;
    AMP_SCRPT_CONFIG_s Scrpt;

    APPLIB_VOUT_PREVIEW_PARAM_s PreviewParam={0};
    UINT32 QvDchanWidth = 0;
    UINT32 QvDchanHeight = 0;
    UINT32 QvFchanWidth = 0;
    UINT32 QvFchanHeight = 0;

    APPLIB_SENSOR_STILLCAP_CONFIG_s *StillCapConfigData;
    AMBA_SENSOR_MODE_ID_u Mode = {0};
    APPLIB_SENSOR_STILLCAP_MODE_CONFIG_s *pMode = NULL;

    /* Get sensor mode.*/
    StillCapConfigData = AppLibSysSensor_GetPjpegConfig(AppLibStillEnc_GetPhotoPjpegCapMode(), AppLibStillEnc_GetPhotoPjpegConfigId());
    pMode = AppLibSysSensor_GetStillCaptureModeConfig(AppLibStillEnc_GetPhotoPjpegCapMode(), AppLibStillEnc_GetPhotoPjpegConfigId());
    Mode.Data = pMode->ModeId;

    G_capcnt = StillCapConfigData->CaptureNumber;
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

//    G_raw_cmpr = cmpr;

    /* Step1. calc raw and yuv buffer memory */
    RawWidth =  pMode->CaptureWidth;
    RawHeight =  pMode->CaptureHeight;
    RawPitch = (cmpr)? \
        AMP_COMPRESSED_RAW_WIDTH(RawWidth): \
        RawWidth*2;
    RawSize = RawPitch*RawHeight;
    AmbaPrint("[Applib - StillEnc] <BurstCapture> raw(%u %u %u)", RawPitch, RawWidth, RawHeight);

    AppLibStillEnc_GetYuvWorkingBuffer(StillCapConfigData->FullviewWidth, StillCapConfigData->FullviewHeight, RawWidth, RawHeight, &YuvWidth, &YuvHeight);
    ScrnW = ALIGN_32(StillCapConfigData->ScreennailWidth);
    ScrnH = StillCapConfigData->ScreennailHeight;
    ThmW = ALIGN_32(StillCapConfigData->ThumbnailWidth);
    ThmH = StillCapConfigData->ThumbnailHeight;
    YuvSize = YuvWidth*YuvHeight*2;
    ScrnSize = ScrnW*ScrnH*2;
    ThmSize = ThmW*ThmH*2;

    if (iso != 2) {
        //DSP lib need 32ALign for Width and 16_Align for height in buffer allocation
        AmbaPrint("[Applib - StillEnc] <BurstCapture> yuv(%u %u %u)!", YuvWidth, YuvHeight, YuvSize);
        ScrnSize += (ScrnSize*10)/100;
        ThmSize += (ThmSize*10)/100;
        AmbaPrint("[Applib - StillEnc] <BurstCapture> scrn(%d %d %u) thm(%d %d %u)!", \
            ScrnW, ScrnH, ScrnSize, \
            ThmW, ThmH, ThmSize);
    }

    /* QV need 16_Align */
    QvLCDW = QvDchanWidth;
    QvLCDH = QvDchanHeight;
    QvHDMIW = QvFchanWidth;
    QvHDMIH = QvFchanHeight;
    QvLCDSize = QvLCDW*QvLCDH*2;
    QvLCDSize += (QvLCDSize*10)/100;
    QvHDMISize = QvHDMIW*QvHDMIH*2;
    QvHDMISize += (QvHDMISize*10)/100;
    AmbaPrint("[Applib - StillEnc] <BurstCapture> qvLCD(%u) qvHDMI(%u)!", QvLCDSize, QvHDMISize);

    /* Step2. allocate raw and yuv/scrn/thm buffer address, script address */
    /* allocate for QV show buffer before working buffer */
    AppLibStillEnc_QvShowBufferAllocate(QvLCDSize, QvHDMISize);

#ifdef _STILL_BUFFER_FROM_DSP_
    {
        UINT8 *dspWorkAddr;
        UINT32 dspWorkSize;
        UINT8 *bufAddr;
        int rt = 0;

        rt = AppLibStillEnc_DspWorkCalculate(&dspWorkAddr, &dspWorkSize);
        if (rt == -1) return -1;

        bufAddr = dspWorkAddr + dspWorkSize;
        BurstCapRawBuffAddr = bufAddr;
        bufAddr += RawSize*StillCapConfigData->CaptureNumber;
        AmbaPrint("[Applib - StillEnc] <BurstCapture> rawBuffAddr (0x%08X) (%u)!", BurstCapRawBuffAddr, RawSize);

        BurstCapYuvBuffAddr = bufAddr;
        bufAddr += YuvSize;
        AmbaPrint("[Applib - StillEnc] <BurstCapture> yuvBuffAddr (0x%08X)!", BurstCapYuvBuffAddr);

        BurstCapScrnBuffAddr = bufAddr;
        bufAddr += ScrnSize;
        AmbaPrint("[Applib - StillEnc] <BurstCapture> scrnBuffAddr (0x%08X)!", BurstCapScrnBuffAddr);

        BurstCapThmBuffAddr = bufAddr;
        bufAddr += ThmSize;
        AmbaPrint("[Applib - StillEnc] <BurstCapture> thmBuffAddr (0x%08X)!", BurstCapThmBuffAddr);

        BurstCapQvLCDBuffAddr = bufAddr;
        bufAddr += QvLCDSize;
        AmbaPrint("[Applib - StillEnc] <BurstCapture> qvLCDBuffaddr (0x%08X)!", BurstCapQvLCDBuffAddr);

        BurstCapQvHDMIBuffAddr = bufAddr;
        bufAddr += QvHDMISize;

        AmbaPrint("[Applib - StillEnc] <BurstCapture> qvHDMIBuffaddr (0x%08X)!", BurstCapQvHDMIBuffAddr);
    }
#else
    ReturnValue = AmpUtil_GetAlignedPool(APPLIB_G_MMPL, &TempPtrBuf, (void **)&BurstCapRawBuffAddrBufRaw, RawSize*StillCapConfigData->CaptureNumber, 32);
    if (ReturnValue != OK) {
    /**Allocate DSP memory if NC memory not encough*/
        AppLibComSvcMemMgr_AllocateDSPMemory(&BurstCapRawBuffAddr,RawSize*StillCapConfigData->CaptureNumber);
        BurstCapAllocateFromDSPFlag = 1;
        AmbaPrint("[Applib - StillEnc] <BurstCapture> NC_DDR alloc raw fail Allocate DSP MEM instead(%u)!", RawSize*StillCapConfigData->CaptureNumber);
    } else {
        BurstCapRawBuffAddr = (UINT8*)TempPtrBuf;
        AmbaPrint("[Applib - StillEnc] <BurstCapture> rawBuffAddr (0x%08X) (%u)!", BurstCapRawBuffAddr, RawSize*StillCapConfigData->CaptureNumber);
    }

    ReturnValue = AmpUtil_GetAlignedPool(APPLIB_G_MMPL, &TempPtrBuf, (void **)&BurstCapYuvBuffAddrBufRaw, YuvSize, 32);
    if (ReturnValue != OK) {
        AppLibComSvcMemMgr_AllocateDSPMemory(&BurstCapYuvBuffAddr,YuvSize);
        BurstCapAllocateFromDSPFlag = 1;
        AmbaPrint("[Applib - StillEnc] <BurstCapture> NC_DDR alloc yuv_main fail Allocate DSP MEM instead(%u)!", YuvSize);
    } else {
        BurstCapYuvBuffAddr = (UINT8*)TempPtrBuf;
        AmbaPrint("[Applib - StillEnc] <BurstCapture> yuvBuffAddr (0x%08X)!", BurstCapYuvBuffAddr);
    }

    ReturnValue = AmpUtil_GetAlignedPool(APPLIB_G_MMPL, &TempPtrBuf, (void **)&BurstCapScrnBuffAddrBufRaw, ScrnSize, 32);
    if (ReturnValue != OK) {
        AppLibComSvcMemMgr_AllocateDSPMemory(&BurstCapScrnBuffAddr,ScrnSize);
        BurstCapAllocateFromDSPFlag = 1;
        AmbaPrint("[Applib - StillEnc] <BurstCapture> NC_DDR alloc yuv_scrn failAllocate DSP MEM instead (%u)!", ScrnSize);
    } else {
        BurstCapScrnBuffAddr = (UINT8*)TempPtrBuf;
        AmbaPrint("[Applib - StillEnc] <BurstCapture> scrnBuffAddr (0x%08X)!", BurstCapScrnBuffAddr);
    }

    ReturnValue = AmpUtil_GetAlignedPool(APPLIB_G_MMPL, &TempPtrBuf, (void **)&BurstCapThmBuffAddrBufRaw, ThmSize, 32);
    if (ReturnValue != OK) {
        AppLibComSvcMemMgr_AllocateDSPMemory(&BurstCapScrnBuffAddr,ScrnSize);
        BurstCapAllocateFromDSPFlag = 1;
        AmbaPrint("[Applib - StillEnc] <BurstCapture> NC_DDR alloc yuv_thm fail Allocate DSP MEM instead(%u)!", ThmSize);
    } else {
        BurstCapThmBuffAddr = (UINT8*)TempPtrBuf;
        AmbaPrint("[Applib - StillEnc] <BurstCapture> thmBuffAddr (0x%08X)!", BurstCapThmBuffAddr);
    }

    ReturnValue = AmpUtil_GetAlignedPool(APPLIB_G_MMPL, &TempPtrBuf, (void **)&BurstCapQvLCDBuffAddrBufRaw, QvLCDSize*StillCapConfigData->CaptureNumber, 32);
    if (ReturnValue != OK) {
        AppLibComSvcMemMgr_AllocateDSPMemory(&BurstCapQvLCDBuffAddr,QvLCDSize*StillCapConfigData->CaptureNumber);
        BurstCapAllocateFromDSPFlag = 1;
        AmbaPrint("[Applib - StillEnc] <BurstCapture> NC_DDR alloc yuv_lcd fail Allocate DSP MEM instead(%u)!", QvLCDSize*StillCapConfigData->CaptureNumber);
   	} else {
        BurstCapQvLCDBuffAddr = (UINT8*)TempPtrBuf;
        AmbaPrint("[Applib - StillEnc] <BurstCapture> qvLCDBuffaddr (0x%08X)!", BurstCapQvLCDBuffAddr);
    }

    ReturnValue = AmpUtil_GetAlignedPool(APPLIB_G_MMPL, &TempPtrBuf, (void **)&BurstCapQvHDMIBuffAddrBufRaw, QvHDMISize*StillCapConfigData->CaptureNumber, 32);
    if (ReturnValue != OK) {
        AppLibComSvcMemMgr_AllocateDSPMemory(&BurstCapQvHDMIBuffAddr,QvHDMISize*StillCapConfigData->CaptureNumber);
        BurstCapAllocateFromDSPFlag = 1;
        AmbaPrint("[Applib - StillEnc] <BurstCapture> NC_DDR alloc yuv_hdmi fail Allocate DSP MEM instead(%u)!", QvHDMISize*StillCapConfigData->CaptureNumber);
    } else {
        BurstCapQvHDMIBuffAddr = (UINT8*)TempPtrBuf;
        AmbaPrint("[Applib - StillEnc] <BurstCapture> qvHDMIBuffaddr (0x%08X)!", BurstCapQvHDMIBuffAddr);
    }
#endif

    ReturnValue = AmpUtil_GetAlignedPool(APPLIB_G_MMPL, &TempPtrBuf, (void **)&BurstCapScriptAddrBufRaw, 128*10, 32);
    if (ReturnValue != OK) {
        AmbaPrint("[Applib - StillEnc] <BurstCapture> NC_DDR alloc scriptAddr fail (%u)!", 128*10);
    } else {
        BurstCapScriptAddr = (UINT8*)TempPtrBuf;
        AmbaPrint("[Applib - StillEnc] <BurstCapture> scriptAddr (0x%08X) (%d)!", BurstCapScriptAddr, 128*10);
    }

    /* Step3. fill script */
    //general config
    SstageAddr = BurstCapScriptAddr;
    GenScrpt = (AMP_SENC_SCRPT_GENCFG_s *)SstageAddr;
    memset(GenScrpt, 0x0, sizeof(AMP_SENC_SCRPT_GENCFG_s));
    GenScrpt->Cmd = SENC_GENCFG;
    GenScrpt->RawEncRepeat = 1;      // More than one raw, need to do rawenc repeatedly.
    GenScrpt->RawEncRepeatStage = 2; // Repeat from R2Y
    GenScrpt->RawToCap = StillCapConfigData->CaptureNumber;
    GenScrpt->StillProcMode = iso;
    GenScrpt->CapProfEnable = 0;
    GenScrpt->CapProfShow = 0;

    GenScrpt->QVConfig.DisableLCDQV = 0;//(qvDisplayCfg == 0)? 1: 0;
    GenScrpt->QVConfig.DisableHDMIQV = 1;//(qvDisplayCfg == 0)? 1: 0;
    GenScrpt->QVConfig.LCDDataFormat = AMP_YUV_422;
    GenScrpt->QVConfig.LCDLumaAddr = BurstCapQvLCDBuffAddr;
    GenScrpt->QVConfig.LCDChromaAddr = BurstCapQvLCDBuffAddr + QvLCDSize/2;
    GenScrpt->QVConfig.LCDWidth = QvDchanWidth;
    GenScrpt->QVConfig.LCDHeight = QvDchanHeight;
    GenScrpt->QVConfig.HDMIDataFormat = AMP_YUV_422;
    GenScrpt->QVConfig.HDMILumaAddr = BurstCapQvHDMIBuffAddr;
    GenScrpt->QVConfig.HDMIChromaAddr = BurstCapQvHDMIBuffAddr + QvHDMISize/2;
    GenScrpt->QVConfig.HDMIWidth = QvFchanWidth;
    GenScrpt->QVConfig.HDMIHeight = QvFchanHeight;
    GenScrpt->b2LVCfg = AMP_ENC_SCRPT_B2LV_NONE;
    //AutoBackToLiveview = (GenScrpt->b2LVCfg)? 1: 0;
    GenScrpt->ScrnEnable = 1;
    GenScrpt->ThmEnable = 1;
    GenScrpt->PreProc = &PreBurstCapCB;
    GenScrpt->PostProc = &PostBurstCapCB;

    GenScrpt->MainBuf.ColorFmt = AMP_YUV_422;
    GenScrpt->MainBuf.Width = GenScrpt->MainBuf.Pitch = YuvWidth;
    GenScrpt->MainBuf.Height = YuvHeight;
    GenScrpt->MainBuf.LumaAddr = BurstCapYuvBuffAddr;
    GenScrpt->MainBuf.ChromaAddr = BurstCapYuvBuffAddr + YuvSize/2;
    GenScrpt->MainBuf.AOI.X = 0;
    GenScrpt->MainBuf.AOI.Y = 0;
    GenScrpt->MainBuf.AOI.Width = StillCapConfigData->FullviewWidth;
    GenScrpt->MainBuf.AOI.Height = StillCapConfigData->FullviewHeight;

    GenScrpt->ScrnBuf.ColorFmt = AMP_YUV_422;
    GenScrpt->ScrnBuf.Width = GenScrpt->ScrnBuf.Pitch = ScrnW;
    GenScrpt->ScrnBuf.Height = ScrnH;
    GenScrpt->ScrnBuf.LumaAddr = BurstCapYuvBuffAddr;
    GenScrpt->ScrnBuf.ChromaAddr = BurstCapYuvBuffAddr + ScrnSize/2;
    GenScrpt->ScrnBuf.AOI.X = 0;
    GenScrpt->ScrnBuf.AOI.Y = 0;
    GenScrpt->ScrnBuf.AOI.Width = StillCapConfigData->ScreennailActiveWidth;
    GenScrpt->ScrnBuf.AOI.Height = StillCapConfigData->ScreennailActiveHeight;
    GenScrpt->ScrnWidth = StillCapConfigData->ScreennailWidth;
    GenScrpt->ScrnHeight = StillCapConfigData->ScreennailHeight;

    GenScrpt->ThmBuf.ColorFmt = AMP_YUV_422;
    GenScrpt->ThmBuf.Width = GenScrpt->ThmBuf.Pitch = ThmW;
    GenScrpt->ThmBuf.Height = ThmH;
    GenScrpt->ThmBuf.LumaAddr = BurstCapThmBuffAddr;
    GenScrpt->ThmBuf.ChromaAddr = BurstCapThmBuffAddr + ThmSize/2;
    GenScrpt->ThmBuf.AOI.X = 0;
    GenScrpt->ThmBuf.AOI.Y = 0;
    GenScrpt->ThmBuf.AOI.Width = StillCapConfigData->ThumbnailActiveWidth;
    GenScrpt->ThmBuf.AOI.Height = StillCapConfigData->ThumbnailActiveHeight;
    GenScrpt->ThmWidth = StillCapConfigData->ThumbnailWidth;
    GenScrpt->ThmHeight = StillCapConfigData->ThumbnailHeight;

    if (targetSize) {
        AmbaPrint("[Applib - StillEnc] <BurstCapture> Target Size %u Kbyte", targetSize);
        AppLibStillEnc_initJpegDqt(ApplibJpegQTable[0], -1);
        AppLibStillEnc_initJpegDqt(ApplibJpegQTable[1], -1);
        AppLibStillEnc_initJpegDqt(ApplibJpegQTable[2], -1);
        GenScrpt->BrcCtrl.Tolerance = 10;
        GenScrpt->BrcCtrl.MaxEncLoop = encodeLoop;
        GenScrpt->BrcCtrl.JpgBrcCB = AppLibStillEnc_jpegBRCPredictCB;
        GenScrpt->BrcCtrl.TargetBitRate = \
           (((targetSize<<13)/StillCapConfigData->FullviewWidth)<<12)/StillCapConfigData->FullviewHeight;
        } else {
            AppLibStillEnc_initJpegDqt(ApplibJpegQTable[0], AppLibStillEnc_GetQuality());
            AppLibStillEnc_initJpegDqt(ApplibJpegQTable[1], AppLibStillEnc_GetQuality());
            AppLibStillEnc_initJpegDqt(ApplibJpegQTable[2], AppLibStillEnc_GetQuality());
            GenScrpt->BrcCtrl.Tolerance = 0;
            GenScrpt->BrcCtrl.MaxEncLoop = 0;
            GenScrpt->BrcCtrl.JpgBrcCB = NULL;
            GenScrpt->BrcCtrl.TargetBitRate = 0;
        }
    GenScrpt->BrcCtrl.MainQTAddr = ApplibJpegQTable[0];
    GenScrpt->BrcCtrl.ThmQTAddr = ApplibJpegQTable[1];
    GenScrpt->BrcCtrl.ScrnQTAddr = ApplibJpegQTable[2];

    TotalScriptSize += ALIGN_128(sizeof(AMP_SENC_SCRPT_GENCFG_s));
    AmbaPrint("[Applib - StillEnc] <BurstCapture> Stage #%d  0x%08X", TotalStageNum, SstageAddr);
    TotalStageNum ++;


    //raw cap config
    SstageAddr = BurstCapScriptAddr + TotalScriptSize;
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
    RawCapScrpt->FvRawBuf.Buf = BurstCapRawBuffAddr;
    RawCapScrpt->FvRawBuf.Width = RawWidth;
    RawCapScrpt->FvRawBuf.Height = RawHeight;
    RawCapScrpt->FvRawBuf.Pitch = RawPitch;
    RawCapScrpt->FvBufRule = AMP_ENC_SCRPT_BUFF_RING;
    RawCapScrpt->FvRingBufSize = RawSize*StillCapConfigData->CaptureNumber;
    RawCapScrpt->CapCB.RawCapCB = AppLibStillEnc_RawCapCB;
    RawCapScrpt->CapCB.RawCapMultiCB = AppLibStillEnc_MultiRawCapCB;
    RawCapScrpt->CapCB.RawCapMultiPrepCB =  AppLibStillEnc_BurstCapSensorPrep;
    TotalScriptSize += ALIGN_128(sizeof(AMP_SENC_SCRPT_RAWCAP_s));
    AmbaPrint("[Applib - StillEnc] <BurstCapture> Stage #%d  0x%08X", TotalStageNum, SstageAddr);
    TotalStageNum ++;


    //raw2yuv config
    SstageAddr = BurstCapScriptAddr + TotalScriptSize;
    Raw2YuvScrpt = (AMP_SENC_SCRPT_RAW2YUV_s *)SstageAddr;
    memset(Raw2YuvScrpt, 0x0, sizeof(AMP_SENC_SCRPT_RAW2YUV_s));
    Raw2YuvScrpt->Cmd = SENC_RAW2YUV;
    Raw2YuvScrpt->RawType = RawCapScrpt->FvRawType;
    Raw2YuvScrpt->RawBuf.Buf = RawCapScrpt->FvRawBuf.Buf;
    Raw2YuvScrpt->RawBuf.Width = RawCapScrpt->FvRawBuf.Width;
    Raw2YuvScrpt->RawBuf.Height = RawCapScrpt->FvRawBuf.Height;
    Raw2YuvScrpt->RawBuf.Pitch = RawCapScrpt->FvRawBuf.Pitch;
    Raw2YuvScrpt->RawBufRule = RawCapScrpt->FvBufRule;
    Raw2YuvScrpt->RingBufSize = 0;
    Raw2YuvScrpt->YuvBufRule = AMP_ENC_SCRPT_BUFF_FIXED;
    Raw2YuvScrpt->YuvRingBufSize = 0;
    TotalScriptSize += ALIGN_128(sizeof(AMP_SENC_SCRPT_RAW2YUV_s));
    AmbaPrint("[Applib - StillEnc] <BurstCapture> Stage #%d  0x%08X", TotalStageNum, SstageAddr);
    TotalStageNum ++;

    //yuv2jpg config
    SstageAddr = BurstCapScriptAddr + TotalScriptSize;
    Yuv2JpgScrpt = (AMP_SENC_SCRPT_YUV2JPG_s *)SstageAddr;
    memset(Yuv2JpgScrpt, 0x0, sizeof(AMP_SENC_SCRPT_YUV2JPG_s));
    Yuv2JpgScrpt->Cmd = SENC_YUV2JPG;
    Yuv2JpgScrpt->YuvBufRule = AMP_ENC_SCRPT_BUFF_FIXED;
    Yuv2JpgScrpt->YuvRingBufSize = 0;
    TotalScriptSize += ALIGN_128(sizeof(AMP_SENC_SCRPT_YUV2JPG_s));
    AmbaPrint("[Applib - StillEnc] <BurstCapture> Stage #%d  0x%08X", TotalStageNum, SstageAddr);
    TotalStageNum ++;


    //script config
    Scrpt.mode = AMP_SCRPT_MODE_STILL;
    Scrpt.StepPreproc = NULL;
    Scrpt.StepPostproc = NULL;
    Scrpt.ScriptStartAddr = (UINT32)BurstCapScriptAddr;
    Scrpt.ScriptTotalSize = TotalScriptSize;
    Scrpt.ScriptStageNum = TotalStageNum;
    AmbaPrint("[Applib - StillEnc] <BurstCapture> Scrpt addr 0x%X, Sz %uByte, stg %d", Scrpt.ScriptStartAddr, Scrpt.ScriptTotalSize, Scrpt.ScriptStageNum);

    AppLibImage_EnableImgSchdlr(0,0);
    /* Step4. execute script */
    AmpEnc_RunScript(StillEncPipe, &Scrpt, AMP_ENC_FUNC_FLAG_NONE);

    /* Step4. release script */
    AmbaPrint("[Applib - StillEnc] <BurstCapture> [0x%08X] memFree", BurstCapScriptAddrBufRaw);
    if (AmbaKAL_BytePoolFree((void *)BurstCapScriptAddrBufRaw) != OK) {
        AmbaPrintColor(RED,"[Applib - StillEnc] <BurstCapture> memFree Fail (scrpt)");
    }
    BurstCapScriptAddrBufRaw = NULL;

    AmbaPrint("[Applib - StillEnc] <BurstCapture> memFree Done");


    return 0;
}


/**
 *  @brief To capture the photo with the burst mode.
 *
 *  To capture the photo with the burst mode.
 *
 *  @return >=0 success, <0 failure
 */
int AppLibStillEnc_CaptureBurst(void)
{
    return AppLibStillEnc_BurstCapture(1, 1, 0, 0);
}

/**
 *  @brief  Register single capture StampProc Callback
 *
 *  @param [in] info stamp information
 *
 *  @return 0 - success, -1 - fail
 */
int AppLibStillEnc_BurstCapRegisterStampCB(APPLIB_STILLENC_STAMP_SETTING_s stampSetting)
{
    StampBurstCapCB.StampInfo = stampSetting;
    StampBurstCapCB.Process = AppLibStillEnc_BurstCaptureStampCB;
    return 0;
}


