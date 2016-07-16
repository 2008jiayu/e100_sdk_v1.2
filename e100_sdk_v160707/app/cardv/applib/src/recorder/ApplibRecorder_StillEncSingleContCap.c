/**
 * @file src/app/connected/applib/src/recorder/ApplibRecorder_StillEncSingleContCap.c
 *
 * Implementation of photo single continuous capture.
 *
 * History:
 *    2013/12/24 - [Martin Lai] created file
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
#include <AmbaCache.h>

#include <imgproc/AmbaImg_Adjustment_Def.h>
#include "ApplibRecorder_StillEncUtility.h"
#include <AmbaDSP_Img3aStatistics.h>
//#define DEBUG_APPLIB_PHOTO_S_CONT_CAP
#if defined(DEBUG_APPLIB_PHOTO_S_CONT_CAP)
#define DBGMSG AmbaPrint
#define DBGMSGc(x) AmbaPrintColor(GREEN,x)
#define DBGMSGc2 AmbaPrintColor
#else
#define DBGMSG(...)
#define DBGMSGc(...)
#define DBGMSGc2(...)
#endif

const UINT16 TileNumber = 1;
static UINT32 G_iso = 1;
//static UINT8 G_raw_cmpr = 0;
//static UINT8 qvDisplayCfg = 0;


extern AMP_STLENC_HDLR_s *StillEncPri;
extern AMP_ENC_PIPE_HDLR_s *StillEncPipe;

/* Single capture buffers */
#ifdef _STILL_BUFFER_FROM_DSP_
static UINT8 *SingleCapDstRawBuffAddr = NULL;
static UINT8 *SingleCapDstRaw3ARoiBuffAddr = NULL;
static UINT8 *SingleCapDstRaw3AStatBuffAddr = NULL;
#else
static UINT8 *SingleCapContScriptAddrBufRaw = NULL;
static UINT8 *SingleCapContScriptAddr = NULL;
static UINT8 *SingleCapContRawBuffAddrBufRaw = NULL;
static UINT8 *SingleCapContRawBuffAddr = NULL;
static UINT8 *SingleCapContYuvBuffAddrBufRaw = NULL;
static UINT8 *SingleCapContYuvBuffAddr = NULL;
static UINT8 *SingleCapContScrnBuffAddrBufRaw = NULL;
static UINT8 *SingleCapContScrnBuffAddr = NULL;
static UINT8 *SingleCapContThmBuffAddrBufRaw = NULL;
static UINT8 *SingleCapContThmBuffAddr = NULL;
static UINT8 *SingleCapContQvLCDBuffAddrBufRaw = NULL;
static UINT8 *SingleCapContQvLCDBuffAddr = NULL;
static UINT8 *SingleCapContQvHDMIBuffAddrBufRaw = NULL;
static UINT8 *SingleCapContQvHDMIBuffAddr = NULL;
#endif

static UINT8 *SingleCapContRaw3ARoiBuffAddr = NULL;
static UINT8 *SingleCapContRaw3ARoiBuffAddrBufRaw = NULL;
static UINT8 *SingleCapContRaw3AStatBuffAddr = NULL;
static UINT8 *SingleCapContRaw3AStatBuffAddrBufRaw = NULL;

static int SingleCapContAllocateFromDSPFlag = 0;
static APPLIB_STILLENC_STAMPP_s StampSingleContCapCB = {.StampInfo = {0}, .Process = NULL};

UINT32 AppLibStillEnc_SingleContCaptureStampCB(UINT8 type, UINT8 *yAddr, UINT8 *uvAddr, UINT16 width, UINT16 height, UINT16 pitch)
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
		if(StampSingleContCapCB.StampInfo.StampAreaEn[0] == 1){

            bufSize = StampSingleContCapCB.StampInfo.StampAreaInfo[0].Width * StampSingleContCapCB.StampInfo.StampAreaInfo[0].Height;

            for(line=0; line<StampSingleContCapCB.StampInfo.StampAreaInfo[0].Height; line++) {
                stampPositionY = ((UINT8*)((UINT32)yAddr + (line+StampSingleContCapCB.StampInfo.StampAreaInfo[0].OffsetY)*pitch)) + StampSingleContCapCB.StampInfo.StampAreaInfo[0].OffsetX;
                stampPositionUV = ((UINT8*)((UINT32)uvAddr + (line+StampSingleContCapCB.StampInfo.StampAreaInfo[0].OffsetY)*pitch)) + StampSingleContCapCB.StampInfo.StampAreaInfo[0].OffsetX;

                for (raw=0; raw<StampSingleContCapCB.StampInfo.StampAreaInfo[0].Width; raw++) {
                    startAddrY_target = ((UINT8*)((UINT32)yAddr + (line+StampSingleContCapCB.StampInfo.StampAreaInfo[0].OffsetY)*pitch)) + StampSingleContCapCB.StampInfo.StampAreaInfo[0].OffsetX + raw;
                    startAddrUV_target = ((UINT8*)((UINT32)uvAddr + (line+StampSingleContCapCB.StampInfo.StampAreaInfo[0].OffsetY)*pitch)) + StampSingleContCapCB.StampInfo.StampAreaInfo[0].OffsetX +raw;
                    startAddrY_source = (UINT8*)((UINT32)StampSingleContCapCB.StampInfo.StampAreaInfo[0].YAddr + line * StampSingleContCapCB.StampInfo.StampAreaInfo[0].Width) + raw;
                    startAddrUV_source = (UINT8*)((UINT32)StampSingleContCapCB.StampInfo.StampAreaInfo[0].UVAddr + line * StampSingleContCapCB.StampInfo.StampAreaInfo[0].Width) + raw;
                    startAddrAlphaY = (UINT8*)((UINT32)StampSingleContCapCB.StampInfo.StampAreaInfo[0].AlphaYAddr + line * StampSingleContCapCB.StampInfo.StampAreaInfo[0].Width) + raw;

                    if (*startAddrAlphaY == 0x0) {
                        *startAddrY_target = *startAddrY_source;
                        *startAddrUV_target = *startAddrUV_source;
                    }
                }
                AmbaCache_Clean(stampPositionY, StampSingleContCapCB.StampInfo.StampAreaInfo[0].Width);
                AmbaCache_Clean(stampPositionUV, StampSingleContCapCB.StampInfo.StampAreaInfo[0].Width);
            }
		}	
		if(StampSingleContCapCB.StampInfo.StampAreaEn[1] == 1){

            bufSize = StampSingleContCapCB.StampInfo.StampAreaInfo[1].Width * StampSingleContCapCB.StampInfo.StampAreaInfo[1].Height;

            for(line=0; line<StampSingleContCapCB.StampInfo.StampAreaInfo[1].Height; line++) {
                stampPositionY = ((UINT8*)((UINT32)yAddr + (line+StampSingleContCapCB.StampInfo.StampAreaInfo[1].OffsetY)*pitch)) + StampSingleContCapCB.StampInfo.StampAreaInfo[1].OffsetX;
                stampPositionUV = ((UINT8*)((UINT32)uvAddr + (line+StampSingleContCapCB.StampInfo.StampAreaInfo[1].OffsetY)*pitch)) + StampSingleContCapCB.StampInfo.StampAreaInfo[1].OffsetX;

                for (raw=0; raw<StampSingleContCapCB.StampInfo.StampAreaInfo[1].Width; raw++) {
                    startAddrY_target = ((UINT8*)((UINT32)yAddr + (line+StampSingleContCapCB.StampInfo.StampAreaInfo[1].OffsetY)*pitch)) + StampSingleContCapCB.StampInfo.StampAreaInfo[1].OffsetX + raw;
                    startAddrUV_target = ((UINT8*)((UINT32)uvAddr + (line+StampSingleContCapCB.StampInfo.StampAreaInfo[1].OffsetY)*pitch)) + StampSingleContCapCB.StampInfo.StampAreaInfo[1].OffsetX +raw;
                    startAddrY_source = (UINT8*)((UINT32)StampSingleContCapCB.StampInfo.StampAreaInfo[1].YAddr + line * StampSingleContCapCB.StampInfo.StampAreaInfo[1].Width) + raw;
                    startAddrUV_source = (UINT8*)((UINT32)StampSingleContCapCB.StampInfo.StampAreaInfo[1].UVAddr + line * StampSingleContCapCB.StampInfo.StampAreaInfo[1].Width) + raw;
                    startAddrAlphaY = (UINT8*)((UINT32)StampSingleContCapCB.StampInfo.StampAreaInfo[1].AlphaYAddr + line * StampSingleContCapCB.StampInfo.StampAreaInfo[1].Width) + raw;

                    if (*startAddrAlphaY == 0x0) {
                        *startAddrY_target = *startAddrY_source;
                        *startAddrUV_target = *startAddrUV_source;
                    }
                }
                AmbaCache_Clean(stampPositionY, StampSingleContCapCB.StampInfo.StampAreaInfo[1].Width);
                AmbaCache_Clean(stampPositionUV, StampSingleContCapCB.StampInfo.StampAreaInfo[1].Width);
            }
		}	
		if(StampSingleContCapCB.StampInfo.StampAreaEn[2] == 1){

            bufSize = StampSingleContCapCB.StampInfo.StampAreaInfo[2].Width * StampSingleContCapCB.StampInfo.StampAreaInfo[2].Height;

            for(line=0; line<StampSingleContCapCB.StampInfo.StampAreaInfo[2].Height; line++) {
                stampPositionY = ((UINT8*)((UINT32)yAddr + (line+StampSingleContCapCB.StampInfo.StampAreaInfo[2].OffsetY)*pitch)) + StampSingleContCapCB.StampInfo.StampAreaInfo[2].OffsetX;
                stampPositionUV = ((UINT8*)((UINT32)uvAddr + (line+StampSingleContCapCB.StampInfo.StampAreaInfo[2].OffsetY)*pitch)) + StampSingleContCapCB.StampInfo.StampAreaInfo[2].OffsetX;

                for (raw=0; raw<StampSingleContCapCB.StampInfo.StampAreaInfo[2].Width; raw++) {
                    startAddrY_target = ((UINT8*)((UINT32)yAddr + (line+StampSingleContCapCB.StampInfo.StampAreaInfo[2].OffsetY)*pitch)) + StampSingleContCapCB.StampInfo.StampAreaInfo[2].OffsetX + raw;
                    startAddrUV_target = ((UINT8*)((UINT32)uvAddr + (line+StampSingleContCapCB.StampInfo.StampAreaInfo[2].OffsetY)*pitch)) + StampSingleContCapCB.StampInfo.StampAreaInfo[2].OffsetX +raw;
                    startAddrY_source = (UINT8*)((UINT32)StampSingleContCapCB.StampInfo.StampAreaInfo[2].YAddr + line * StampSingleContCapCB.StampInfo.StampAreaInfo[2].Width) + raw;
                    startAddrUV_source = (UINT8*)((UINT32)StampSingleContCapCB.StampInfo.StampAreaInfo[2].UVAddr + line * StampSingleContCapCB.StampInfo.StampAreaInfo[2].Width) + raw;
                    startAddrAlphaY = (UINT8*)((UINT32)StampSingleContCapCB.StampInfo.StampAreaInfo[2].AlphaYAddr + line * StampSingleContCapCB.StampInfo.StampAreaInfo[2].Width) + raw;

                    if (*startAddrAlphaY == 0x0) {
                        *startAddrY_target = *startAddrY_source;
                        *startAddrUV_target = *startAddrUV_source;
                    }
                }
                AmbaCache_Clean(stampPositionY, StampSingleContCapCB.StampInfo.StampAreaInfo[2].Width);
                AmbaCache_Clean(stampPositionUV, StampSingleContCapCB.StampInfo.StampAreaInfo[2].Width);
            }
		}			
#else			
            bufSize = StampSingleContCapCB.StampInfo.Width * StampSingleContCapCB.StampInfo.Height;

            for(line=0; line<StampSingleContCapCB.StampInfo.Height; line++) {
                stampPositionY = ((UINT8*)((UINT32)yAddr + (line+StampSingleContCapCB.StampInfo.OffsetY)*pitch)) + StampSingleContCapCB.StampInfo.OffsetX;
                stampPositionUV = ((UINT8*)((UINT32)uvAddr + (line+StampSingleContCapCB.StampInfo.OffsetY)*pitch)) + StampSingleContCapCB.StampInfo.OffsetX;

                for (raw=0; raw<StampSingleContCapCB.StampInfo.Width; raw++) {
                    startAddrY_target = ((UINT8*)((UINT32)yAddr + (line+StampSingleContCapCB.StampInfo.OffsetY)*pitch)) + StampSingleContCapCB.StampInfo.OffsetX + raw;
                    startAddrUV_target = ((UINT8*)((UINT32)uvAddr + (line+StampSingleContCapCB.StampInfo.OffsetY)*pitch)) + StampSingleContCapCB.StampInfo.OffsetX +raw;
                    startAddrY_source = (UINT8*)((UINT32)StampSingleContCapCB.StampInfo.YAddr + line * StampSingleContCapCB.StampInfo.Width) + raw;
                    startAddrUV_source = (UINT8*)((UINT32)StampSingleContCapCB.StampInfo.UVAddr + line * StampSingleContCapCB.StampInfo.Width) + raw;
                    startAddrAlphaY = (UINT8*)((UINT32)StampSingleContCapCB.StampInfo.AlphaYAddr + line * StampSingleContCapCB.StampInfo.Width) + raw;

                    if (*startAddrAlphaY == 0x0) {
                        *startAddrY_target = *startAddrY_source;
                        *startAddrUV_target = *startAddrUV_source;
                    }
                }
                AmbaCache_Clean(stampPositionY, StampSingleContCapCB.StampInfo.Width);
                AmbaCache_Clean(stampPositionUV, StampSingleContCapCB.StampInfo.Width);
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

UINT32 AppLibStillEnc_SetShotCount(UINT32 shotCount)
{
    UINT32 ChNo = 0;
    /* According to A7L, set cont shot count number from second capture */
    AmbaImg_Proc_Cmd(MW_IP_SET_CONTI_SHOTCOUNT, ChNo, shotCount, 0);
    return 0;
}

/* CB for raw2raw idsp setting  */
UINT32 AppLibStillEnc_Raw2RawIdspCfgCB(UINT16 index)
{
    AMBA_DSP_IMG_MODE_CFG_s ImgMode;
    AMBA_DSP_IMG_BLACK_CORRECTION_s StaticBlackLevel = {0};
    AMBA_DSP_IMG_AAA_STAT_INFO_s AaaStatInfo = {0};
    AMBA_DSP_IMG_CFA_LEAKAGE_FILTER_s CfaLeakage = {0};
    AMBA_DSP_IMG_DGAIN_SATURATION_s DgainSaturation = {0};
    AMBA_DSP_IMG_DBP_CORRECTION_s DbpCorr = {0};
    AMBA_DSP_IMG_SBP_CORRECTION_s SbpCorr = {0};
    AMBA_DSP_IMG_ANTI_ALIASING_s AntiAliasing = {0};

    AMBA_DSP_IMG_AE_STAT_INFO_s AeStatInfo;
    AMBA_DSP_IMG_AF_STAT_INFO_s AfStatInfo;
    AMBA_DSP_IMG_AWB_STAT_INFO_s AwbStatInfo;
    AMBA_SENSOR_MODE_ID_u Mode = {0};
    APPLIB_SENSOR_STILLCAP_MODE_CONFIG_s *pMode = NULL;

    AmbaPrint("[Applib - StillEnc] <SingleCaptureCont> raw2raw idsp CB %d", index);


    /* Get sensor mode.*/
    pMode = AppLibSysSensor_GetStillCaptureModeConfig(AppLibStillEnc_GetPhotoPjpegCapMode(), AppLibStillEnc_GetPhotoPjpegConfigId());
    Mode.Data = pMode->ModeId;

    /* Setup mode of ImageKernel */
    memset(&ImgMode, 0, sizeof(AMBA_DSP_IMG_MODE_CFG_s));
    ImgMode.Pipe      = AMBA_DSP_IMG_PIPE_STILL;
    ImgMode.AlgoMode  = AMBA_DSP_IMG_ALGO_MODE_LISO;
    ImgMode.BatchId   = AMBA_DSP_RAW_TO_RAW_FILTER;
    ImgMode.FuncMode = AMBA_DSP_IMG_FUNC_MODE_RAW2RAW;
    ImgMode.ContextId = 0;
    ImgMode.ConfigId  = 0;

    if (TileNumber == 1) { //full frame
        UINT16 RawW = pMode->CaptureWidth;
        UINT16 RawH = pMode->CaptureHeight;

        if (index == 0) {
            AmbaDSP_ImgGetStaticBlackLevel(&ImgMode, &StaticBlackLevel);
            AmbaDSP_ImgSetStaticBlackLevel(&ImgMode, &StaticBlackLevel);

            AmbaDSP_Img3aGetAaaStatInfo(&ImgMode, &AaaStatInfo);
            AaaStatInfo.AeTileWidth = RawW/AaaStatInfo.AeTileNumCol;
            AaaStatInfo.AeTileHeight = RawH/AaaStatInfo.AeTileNumRow;
            AaaStatInfo.AfTileWidth = RawW/AaaStatInfo.AfTileNumCol;
            AaaStatInfo.AfTileHeight = RawH/AaaStatInfo.AfTileNumRow;
            AaaStatInfo.AfTileActiveWidth = AaaStatInfo.AfTileWidth;
            AaaStatInfo.AfTileActiveHeight = AaaStatInfo.AfTileHeight;
            AaaStatInfo.AwbTileWidth = RawW/AaaStatInfo.AwbTileNumCol;
            AaaStatInfo.AwbTileHeight = RawH/AaaStatInfo.AwbTileNumRow;
            AaaStatInfo.AwbTileActiveWidth = AaaStatInfo.AwbTileWidth;
            AaaStatInfo.AwbTileActiveHeight = AaaStatInfo.AwbTileHeight;
            AmbaDSP_Img3aSetAaaStatInfo(&ImgMode, &AaaStatInfo);

            AmbaDSP_ImgSetCawarpCompensation(&ImgMode);
            AmbaDSP_ImgSetVignetteCompensation(&ImgMode);

            AmbaDSP_ImgGetCfaLeakageFilter(&ImgMode,&CfaLeakage);
            AmbaDSP_ImgSetCfaLeakageFilter(&ImgMode,&CfaLeakage);

            AmbaDSP_ImgGetDgainSaturationLevel(&ImgMode,&DgainSaturation);
            AmbaDSP_ImgSetDgainSaturationLevel(&ImgMode,&DgainSaturation);

            AmbaDSP_ImgGetDynamicBadPixelCorrection(&ImgMode,&DbpCorr);
            AmbaDSP_ImgSetDynamicBadPixelCorrection(&ImgMode,&DbpCorr);

            AmbaDSP_ImgGetStaticBadPixelCorrection(&ImgMode,&SbpCorr);
            AmbaDSP_ImgSetStaticBadPixelCorrection(&ImgMode,&SbpCorr);

            AmbaDSP_ImgGetAntiAliasing(&ImgMode, &AntiAliasing);
            AmbaDSP_ImgSetAntiAliasing(&ImgMode, &AntiAliasing);
        }

    } else if (TileNumber == 3) { //multi roi
        if (index == 0) {
            AeStatInfo.AeTileNumCol = 1;
            AeStatInfo.AeTileNumRow = 1;
            AeStatInfo.AeTileColStart = 0;
            AeStatInfo.AeTileRowStart = 0;
            AeStatInfo.AeTileWidth = 128 / AeStatInfo.AeTileNumCol;
            AeStatInfo.AeTileHeight = 128 / AeStatInfo.AeTileNumRow;
            AeStatInfo.AePixMinValue = 0;
            AeStatInfo.AePixMaxValue = 16100;

            AwbStatInfo.AwbTileNumCol = 1;
            AwbStatInfo.AwbTileNumRow = 1;
            AwbStatInfo.AwbTileColStart = 0;
            AwbStatInfo.AwbTileRowStart = 0;
            AwbStatInfo.AwbTileWidth = 128 / AwbStatInfo.AwbTileNumCol;
            AwbStatInfo.AwbTileHeight = 128 / AwbStatInfo.AwbTileNumRow;
            AwbStatInfo.AwbTileActiveWidth = AwbStatInfo.AwbTileWidth;
            AwbStatInfo.AwbTileActiveHeight = AwbStatInfo.AwbTileHeight;
            AwbStatInfo.AwbPixMinValue = 0;
            AwbStatInfo.AwbPixMaxValue = 16100;

            AfStatInfo.AfTileNumCol = 1;
            AfStatInfo.AfTileNumRow = 1;
            AfStatInfo.AfTileColStart = 0;
            AfStatInfo.AfTileRowStart = 0;
            AfStatInfo.AfTileWidth = 128 / AfStatInfo.AfTileNumCol;
            AfStatInfo.AfTileHeight = 128 / AfStatInfo.AfTileNumRow;
            AfStatInfo.AfTileActiveWidth = AfStatInfo.AfTileWidth;
            AfStatInfo.AfTileActiveHeight = AfStatInfo.AfTileHeight;
        } else if (index == 1) {
            AeStatInfo.AeTileNumCol = 4;
            AeStatInfo.AeTileNumRow = 4;
            AeStatInfo.AeTileColStart = 0;
            AeStatInfo.AeTileRowStart = 0;
            AeStatInfo.AeTileWidth = 512 / AeStatInfo.AeTileNumCol;
            AeStatInfo.AeTileHeight = 512 / AeStatInfo.AeTileNumRow;
            AeStatInfo.AePixMinValue = 0;
            AeStatInfo.AePixMaxValue = 16100;

            AwbStatInfo.AwbTileNumCol = 4;
            AwbStatInfo.AwbTileNumRow = 4;
            AwbStatInfo.AwbTileColStart = 0;
            AwbStatInfo.AwbTileRowStart = 0;
            AwbStatInfo.AwbTileWidth = 512 / AwbStatInfo.AwbTileNumCol;
            AwbStatInfo.AwbTileHeight = 512 / AwbStatInfo.AwbTileNumRow;
            AwbStatInfo.AwbTileActiveWidth = AwbStatInfo.AwbTileWidth;
            AwbStatInfo.AwbTileActiveHeight = AwbStatInfo.AwbTileHeight;
            AwbStatInfo.AwbPixMinValue = 0;
            AwbStatInfo.AwbPixMaxValue = 16100;

            AfStatInfo.AfTileNumCol = 4;
            AfStatInfo.AfTileNumRow = 4;
            AfStatInfo.AfTileColStart = 0;
            AfStatInfo.AfTileRowStart = 0;
            AfStatInfo.AfTileWidth = 512 / AfStatInfo.AfTileNumCol;
            AfStatInfo.AfTileHeight = 512 / AfStatInfo.AfTileNumRow;
            AfStatInfo.AfTileActiveWidth = AfStatInfo.AfTileWidth;
            AfStatInfo.AfTileActiveHeight = AfStatInfo.AfTileHeight;
        } else if (index == 2) {
            AeStatInfo.AeTileNumCol = 2;
            AeStatInfo.AeTileNumRow = 2;
            AeStatInfo.AeTileColStart = 0;
            AeStatInfo.AeTileRowStart = 0;
            AeStatInfo.AeTileWidth = 256 / AeStatInfo.AeTileNumCol;
            AeStatInfo.AeTileHeight = 256 / AeStatInfo.AeTileNumRow;
            AeStatInfo.AePixMinValue = 0;
            AeStatInfo.AePixMaxValue = 16100;

            AwbStatInfo.AwbTileNumCol = 2;
            AwbStatInfo.AwbTileNumRow = 2;
            AwbStatInfo.AwbTileColStart = 0;
            AwbStatInfo.AwbTileRowStart = 0;
            AwbStatInfo.AwbTileWidth = 256 / AwbStatInfo.AwbTileNumCol;
            AwbStatInfo.AwbTileHeight = 256 / AwbStatInfo.AwbTileNumRow;
            AwbStatInfo.AwbTileActiveWidth = AwbStatInfo.AwbTileWidth;
            AwbStatInfo.AwbTileActiveHeight = AwbStatInfo.AwbTileHeight;
            AwbStatInfo.AwbPixMinValue = 0;
            AwbStatInfo.AwbPixMaxValue = 16100;

            AfStatInfo.AfTileNumCol = 2;
            AfStatInfo.AfTileNumRow = 2;
            AfStatInfo.AfTileColStart = 0;
            AfStatInfo.AfTileRowStart = 0;
            AfStatInfo.AfTileWidth = 256 / AfStatInfo.AfTileNumCol;
            AfStatInfo.AfTileHeight = 256 / AfStatInfo.AfTileNumRow;
            AfStatInfo.AfTileActiveWidth = AfStatInfo.AfTileWidth;
            AfStatInfo.AfTileActiveHeight = AfStatInfo.AfTileHeight;
        }

        AmbaDSP_Img3aSetAeStatInfo(&ImgMode, &AeStatInfo);
        AmbaDSP_Img3aSetAfStatInfo(&ImgMode, &AfStatInfo);
        AmbaDSP_Img3aSetAwbStatInfo(&ImgMode, &AwbStatInfo);
    } else if (TileNumber == 0) {
        //TBD
    }

    return OK;
}

/**
 * UnitTest: Still IDSP parameters setup should be done any time but before doing R2Y
 *
 * @param [in] : (TBD) procMode can be class as follow:
 * 0 means Allproc: all IDSP params will be setup up at one time
 * 1 means Preproc: fast-calculate IDSP params can be setup before still capture
 * 2 means Postproc:Slow-calculate IDSP params can be setup after raw capture is done
 * User can setup IDSP either by Allproc or Preproc + Postproc, but make sure at correct timing
 *
 * @return 0 - success, -1 - fail
 */
UINT32 AppLibStillEnc_IdspParamSetup(UINT8 aeIdx)
{

    AMBA_DSP_IMG_MODE_CFG_s imgMode;
    AMBA_DSP_IMG_SIZE_INFO_s sizeInfo;
    AMBA_DSP_IMG_WARP_CALC_INFO_s calcWarp = {0};
    double zoomRatio;
    UINT16 capW, capH, encW, encH, ScrnW, ScrnH, ThmW, ThmH;
    AMBA_DSP_IMG_VIN_SENSOR_GEOMETRY_s vinGeo;
    AMBA_DSP_IMG_OUT_WIN_INFO_s                 imgOutputWin;
    AMBA_DSP_IMG_DZOOM_INFO_s                   imgDzoom;
    UINT32 imgIpChNo = 0;
    AMBA_SENSOR_STATUS_INFO_s sensorStatus;
    AMBA_SENSOR_AREA_INFO_s *recPixel = &sensorStatus.ModeInfo.OutputInfo.RecordingPixels;
    AMBA_SENSOR_INPUT_INFO_s *inputInfo = &sensorStatus.ModeInfo.InputInfo;
    UINT16 offsetX = 0;
    UINT32 DchanWidth = 0;
    UINT32 DchanHeight = 0;
    UINT32 FchanWidth = 0;
    UINT32 FchanHeight = 0;
    extern void Amba_Img_Set_Still_Pipe_Ctrl_Params(UINT32 chNo, AMBA_DSP_IMG_MODE_CFG_s* mode);

    /* Run Adj compute @ LISO */
    if (G_iso == 1) {
        AMBA_DSP_IMG_CTX_INFO_s destCtx, srcCtx;
        AMBA_DSP_IMG_CFG_INFO_s cfgInfo;
        ADJ_STILL_CONTROL_s adjStillCtrl;
        AMBA_DSP_IMG_WB_GAIN_s wbGain;
        AMBA_AE_INFO_s stillAeInfo[MAX_AEB_NUM];

        /* Prepare IK ctx */
        /* Initialize the context of ImageKernel of still */
        destCtx.Pipe = AMBA_DSP_IMG_PIPE_STILL;
        destCtx.CtxId = 0;
        srcCtx.CtxId = 0;
        AmbaDSP_ImgInitCtx(0, 0, &destCtx, &srcCtx);

        /* Initialize the configure of ImageKernel of Still */
        cfgInfo.Pipe = AMBA_DSP_IMG_PIPE_STILL;
        cfgInfo.CfgId = 0;
        if (G_iso == 1) AmbaDSP_ImgInitCfg(&cfgInfo, AMBA_DSP_IMG_ALGO_MODE_LISO);
        else AmbaDSP_ImgInitCfg(&cfgInfo, AMBA_DSP_IMG_ALGO_MODE_HISO);
        AmbaImg_Proc_Cmd(MW_IP_GET_AE_INFO, imgIpChNo, IP_MODE_STILL, (UINT32)stillAeInfo);
        AmbaImg_Proc_Cmd(MW_IP_GET_WB_GAIN, imgIpChNo, (UINT32)&wbGain, 0);

        memset(&adjStillCtrl, 0x0, sizeof(ADJ_STILL_CONTROL_s));
        adjStillCtrl.StillMode = 0; //temp set as 0, since HISO adj is not ready
        adjStillCtrl.ShIndex = stillAeInfo[aeIdx].ShutterIndex;
        adjStillCtrl.EvIndex = stillAeInfo[aeIdx].EvIndex;
        adjStillCtrl.NfIndex = stillAeInfo[aeIdx].NfIndex;
        adjStillCtrl.WbGain = wbGain;
        adjStillCtrl.DZoomStep = 0;
        adjStillCtrl.FlashMode = 0;
        adjStillCtrl.LutNo = 0;
        AmbaPrint("[ADJ] Chnlid %d, shidx %d, evidx %d, nfidx %d, wb(%u %u %u %u %u)", \
            imgIpChNo, adjStillCtrl.ShIndex, \
            adjStillCtrl.EvIndex, adjStillCtrl.NfIndex, \
            adjStillCtrl.WbGain.GainR, adjStillCtrl.WbGain.GainG, \
            adjStillCtrl.WbGain.GainB, adjStillCtrl.WbGain.AeGain, \
            adjStillCtrl.WbGain.GlobalDGain);
        AmbaImg_Proc_Cmd(MW_IP_ADJ_STILL_CONTROL, (UINT32)imgIpChNo , (UINT32)&adjStillCtrl , 0);


        memset(&imgMode, 0, sizeof(AMBA_DSP_IMG_MODE_CFG_s));
        imgMode.Pipe = AMBA_DSP_IMG_PIPE_STILL;
        imgMode.AlgoMode = (G_iso == 1)? AMBA_DSP_IMG_ALGO_MODE_LISO: AMBA_DSP_IMG_ALGO_MODE_HISO;
        imgMode.BatchId   = AMBA_DSP_VIDEO_FILTER;
        imgMode.ContextId = 0;
        imgMode.ConfigId  = 0;
        Amba_Img_Set_Still_Pipe_Ctrl_Params((UINT32)imgIpChNo, &imgMode);

        AppLibStillEnc_SetStillWB(imgIpChNo, &imgMode);
		AppLibStillEnc_SetStillSensorInfo(&imgMode);
    } else if (G_iso == 0) //Applib_IsoConfigSet(AMBA_DSP_IMG_ALGO_MODE_HISO);

    memset(&imgMode, 0, sizeof(AMBA_DSP_IMG_MODE_CFG_s));
    imgMode.Pipe = AMBA_DSP_IMG_PIPE_STILL;
    if (G_iso == 0) {
        imgMode.AlgoMode  = AMBA_DSP_IMG_ALGO_MODE_HISO;
    } else if (G_iso == 1) {
        imgMode.AlgoMode  = AMBA_DSP_IMG_ALGO_MODE_LISO;
    } else {
        imgMode.AlgoMode  = AMBA_DSP_IMG_ALGO_MODE_FAST;
    }
    imgMode.BatchId   = AMBA_DSP_VIDEO_FILTER;
    imgMode.ContextId = 0;
    imgMode.ConfigId  = 0;
    {

        APPLIB_SENSOR_STILLCAP_CONFIG_s *StillCapConfigData;
        AMBA_SENSOR_MODE_ID_u Mode = {0};
        APPLIB_VOUT_PREVIEW_PARAM_s PreviewParam={0};
        APPLIB_SENSOR_STILLCAP_MODE_CONFIG_s *pMode = NULL;

        /* Get sensor mode.*/
        StillCapConfigData = AppLibSysSensor_GetPjpegConfig(AppLibStillEnc_GetPhotoPjpegCapMode(), AppLibStillEnc_GetPhotoPjpegConfigId());
        pMode = AppLibSysSensor_GetStillCaptureModeConfig(AppLibStillEnc_GetPhotoPjpegCapMode(), AppLibStillEnc_GetPhotoPjpegConfigId());
        Mode.Data = pMode->ModeId;

        PreviewParam.AspectRatio = AppLibSysSensor_GetCaptureModeAR(AppLibStillEnc_GetPhotoPjpegCapMode(), AppLibStillEnc_GetPhotoPjpegConfigId());
        PreviewParam.ChanID = DISP_CH_FCHAN;
        AppLibDisp_CalcPreviewWindowSize(&PreviewParam);
        FchanWidth = PreviewParam.Preview.Width;
        FchanHeight = PreviewParam.Preview.Height;

        PreviewParam.ChanID = DISP_CH_DCHAN;
        AppLibDisp_CalcPreviewWindowSize(&PreviewParam);
        DchanWidth = PreviewParam.Preview.Width;
        DchanHeight = PreviewParam.Preview.Height;


        /* Dzoom */
        capW = pMode->CaptureWidth;
        capH = pMode->CaptureHeight;
        encW = StillCapConfigData->FullviewWidth;
        encH = StillCapConfigData->FullviewHeight;
        ScrnW = StillCapConfigData->ScreennailWidth;
        ScrnH = StillCapConfigData->ScreennailHeight;
        ThmW = StillCapConfigData->ThumbnailWidth;
        ThmH = StillCapConfigData->ThumbnailHeight;
    }

    memset(&sensorStatus, 0x0, sizeof(AMBA_SENSOR_STATUS_INFO_s));
    AmbaSensor_GetStatus(AppEncChannel, &sensorStatus);



    memset(&vinGeo, 0, sizeof(vinGeo));
    vinGeo.Width  = capW;
    vinGeo.Height = capH;
    offsetX = ((recPixel->StartX + (((recPixel->Width - capW)/2)&0xFFF8)) - recPixel->StartX)* \
                inputInfo->HSubsample.FactorDen/inputInfo->HSubsample.FactorNum;
    offsetX = ALIGN_8(offsetX);
    vinGeo.StartX = inputInfo->PhotodiodeArray.StartX + offsetX;
    vinGeo.StartY = inputInfo->PhotodiodeArray.StartY + \
        (((recPixel->StartY + ((recPixel->Height - capH)/2)) & 0xFFFE) - recPixel->StartY)* \
                inputInfo->VSubsample.FactorDen/inputInfo->VSubsample.FactorNum;
    vinGeo.HSubSample.FactorDen = inputInfo->HSubsample.FactorDen;
    vinGeo.HSubSample.FactorNum = inputInfo->HSubsample.FactorNum;
    vinGeo.VSubSample.FactorDen = inputInfo->VSubsample.FactorDen;
    vinGeo.VSubSample.FactorNum = inputInfo->VSubsample.FactorNum;
    AmbaDSP_WarpCore_SetVinSensorGeo(&imgMode, &vinGeo);

    memset(&imgOutputWin, 0, sizeof(imgOutputWin));
    imgOutputWin.MainWinDim.Width  = encW;
    imgOutputWin.MainWinDim.Height = encH;
    imgOutputWin.ScreennailDim.Width  = ScrnW;
    imgOutputWin.ScreennailDim.Height = ScrnH;
    imgOutputWin.ThumbnailDim.Width  = ThmW;
    imgOutputWin.ThumbnailDim.Height = ThmH;
    imgOutputWin.PrevWinDim[0].Width  = DchanWidth;
    imgOutputWin.PrevWinDim[0].Height = DchanHeight;
    imgOutputWin.PrevWinDim[1].Width  = FchanWidth;
    imgOutputWin.PrevWinDim[1].Height = FchanHeight;
    AmbaDSP_WarpCore_SetOutputWin(&imgMode, &imgOutputWin);

    /* Dzoom don't care, TBD */
    zoomRatio = (double) 100 / 100;

    memset(&imgDzoom, 0, sizeof(imgDzoom));
    imgDzoom.ZoomX = (UINT32)(zoomRatio * 65536);
    imgDzoom.ZoomY = (UINT32)(zoomRatio * 65536);
    AmbaDSP_WarpCore_SetDzoomFactor(&imgMode, &imgDzoom);

    AmbaDSP_WarpCore_CalcDspWarp(&imgMode, 0);
    AmbaDSP_WarpCore_CalcDspCawarp(&imgMode, 0);

    AmbaDSP_WarpCore_SetDspWarp(&imgMode);
    AmbaDSP_WarpCore_SetDspCawarp(&imgMode);

    memset(&calcWarp, 0, sizeof(calcWarp));
    if (AmbaDSP_ImgGetWarpCompensation(&imgMode, &calcWarp) != OK)
        AmbaPrintColor(RED, "[UT_StillEnc_StillIdspParamSetup] Get Warp Compensation fail!!");


    {

        APPLIB_SENSOR_STILLCAP_CONFIG_s *StillCapConfigData;
        AMBA_SENSOR_MODE_ID_u Mode = {0};
        APPLIB_VOUT_PREVIEW_PARAM_s PreviewParam={0};
        APPLIB_SENSOR_STILLCAP_MODE_CONFIG_s *pMode = NULL;
        /* Get sensor mode.*/
        StillCapConfigData = AppLibSysSensor_GetPjpegConfig(AppLibStillEnc_GetPhotoPjpegCapMode(), AppLibStillEnc_GetPhotoPjpegConfigId());
        pMode = AppLibSysSensor_GetStillCaptureModeConfig(AppLibStillEnc_GetPhotoPjpegCapMode(), AppLibStillEnc_GetPhotoPjpegConfigId());
        Mode.Data = pMode->ModeId;

        /* Set the size of Fchan preview window.*/
        PreviewParam.AspectRatio = AppLibSysSensor_GetCaptureModeAR(AppLibStillEnc_GetPhotoPjpegCapMode(), AppLibStillEnc_GetPhotoPjpegConfigId());
        PreviewParam.ChanID = DISP_CH_FCHAN;
        AppLibDisp_CalcPreviewWindowSize(&PreviewParam);


        memset(&sizeInfo, 0, sizeof(sizeInfo));
        sizeInfo.WidthIn     = ((calcWarp.ActWinCrop.RightBotX - calcWarp.ActWinCrop.LeftTopX + 0xFFFF)>>16);
        sizeInfo.HeightIn    = ((calcWarp.ActWinCrop.RightBotY - calcWarp.ActWinCrop.LeftTopY + 0xFFFF)>>16);
        sizeInfo.WidthMain   = StillCapConfigData->FullviewWidth;
        sizeInfo.HeightMain  = StillCapConfigData->FullviewHeight;
        /* Set the size of Dchan preview window.*/
        PreviewParam.ChanID = DISP_CH_DCHAN;
        AppLibDisp_CalcPreviewWindowSize(&PreviewParam);
        sizeInfo.WidthPrevA = PreviewParam.Preview.Width;
        sizeInfo.HeightPrevA = PreviewParam.Preview.Height;

        PreviewParam.ChanID = DISP_CH_FCHAN;
        AppLibDisp_CalcPreviewWindowSize(&PreviewParam);
        sizeInfo.WidthPrevB = PreviewParam.Preview.Width;
        sizeInfo.HeightPrevB = PreviewParam.Preview.Height;
        sizeInfo.WidthScrn = ScrnW;
        sizeInfo.HeightScrn = ScrnH;
        AmbaDSP_ImgSetSizeInfo(&imgMode, &sizeInfo);
        AmbaDSP_ImgPostExeCfg(&imgMode, (AMBA_DSP_IMG_CONFIG_EXECUTE_MODE_e)0);
        AmbaPrint("[UT_StillEnc_StillIdspParamSetup] Done!!!");
    }
    return 0;
}

/**
 * Still post IDSP parameters setup should be invoke after post WB calculation
 *
 * @return 0 - success, -1 - fail
 */
UINT32 AppLibStillEnc_PostIdspParamSetup(UINT8 aeIdx)
{
    extern void Amba_Img_Set_Still_Pipe_Ctrl_Params(UINT32 chNo, AMBA_DSP_IMG_MODE_CFG_s* mode);
    UINT32 ImgChipNo = 0;
    AMBA_DSP_IMG_MODE_CFG_s ImgMode;
    ADJ_STILL_CONTROL_s AdjStillCtrl;
    AMBA_AE_INFO_s StillAeInfo[MAX_AEB_NUM];
    AMBA_DSP_IMG_WB_GAIN_s WbGain;

    memset(&ImgMode, 0, sizeof(AMBA_DSP_IMG_MODE_CFG_s));
    ImgMode.Pipe = AMBA_DSP_IMG_PIPE_STILL;
    ImgMode.AlgoMode = (G_iso == 1)?AMBA_DSP_IMG_ALGO_MODE_LISO:AMBA_DSP_IMG_ALGO_MODE_HISO;
    ImgMode.BatchId   = AMBA_DSP_VIDEO_FILTER;
    ImgMode.ContextId = 0;
    ImgMode.ConfigId  = 0;
    AmbaImg_Proc_Cmd(MW_IP_GET_AE_INFO, ImgChipNo, IP_MODE_STILL, (UINT32)StillAeInfo);
    AmbaImg_Proc_Cmd(MW_IP_GET_WB_GAIN, ImgChipNo, (UINT32)&WbGain, 0);

    if (0/*TBD*/) {
        memset(&AdjStillCtrl, 0x0, sizeof(ADJ_STILL_CONTROL_s));
        AdjStillCtrl.StillMode = 0; //temp set as 0, since HISO adj is not ready
        AdjStillCtrl.ShIndex = StillAeInfo[aeIdx].ShutterIndex;
        AdjStillCtrl.EvIndex = StillAeInfo[aeIdx].EvIndex;
        AdjStillCtrl.NfIndex = StillAeInfo[aeIdx].NfIndex;
        AdjStillCtrl.WbGain = WbGain;
        AdjStillCtrl.DZoomStep = 0;
        AdjStillCtrl.FlashMode = 0;
        AdjStillCtrl.LutNo = 0;
        AmbaImg_Proc_Cmd(MW_IP_ADJ_STILL_CONTROL, (UINT32)ImgChipNo , (UINT32)&AdjStillCtrl , 0);
    }

    if (1/*TBD*/) {
        Amba_Img_Set_Still_Pipe_Ctrl_Params((UINT32)ImgChipNo, &ImgMode);
    }
    AppLibStillEnc_SetStillWB(ImgChipNo, &ImgMode);
    AppLibStillEnc_SetStillSensorInfo(&ImgMode);
    AmbaDSP_ImgPostExeCfg(&ImgMode, AMBA_DSP_IMG_CFG_EXE_FULLCOPY);
    AmbaPrint("[Applib - StillEnc] <SingleCaptureCont> StillEnc_PostIdspParamSetup Done!!!");
    return OK;
}

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
UINT32 AppLibStillEnc_SingleCaptureContPreCB(AMP_STILLENC_PREP_INFO_s *info)
{
    //AmbaPrint("[Applib - StillEnc] <SingleCaptureCont> Pre-CallBack: SerialNum %d", info->JpegSerialNumber);
    if (info->StageCnt == 1) {
    } else if (info->StageCnt == 2) {
        AppLibStillEnc_Raw2RawIdspCfgCB(info->CfaIndex);
    } else if (info->StageCnt == 3) {
        AppLibStillEnc_PostIdspParamSetup(info->AeIdx/*FIXME: shot count*/);
    }

    return 0;
}
static AMP_STILLENC_PREP_s PreSingleCapContCB = {.Process = AppLibStillEnc_SingleCaptureContPreCB};


static UINT16 raw_fno = 1;
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
UINT32 AppLibStillEnc_SingleCaptureContPostCB(AMP_STILLENC_POSTP_INFO_s *info)
{
    static UINT8 yuvFlag = 0;

    if (info->StageCnt == 1) {
#if defined(DEBUG_APPLIB_PHOTO_S_CONT_CAP)
        char fn[64];
        char mode[3] = {'w','b','\0'};
        AMBA_FS_FILE *raw = NULL;

        //raw ready, dump it
        sprintf(fn,"C:\\%04d.RAW", raw_fno);


        raw = AmbaFS_fopen((char const *)fn,(char const *) mode);
        AmbaPrint("[Applib - StillEnc] <SingleCaptureCont> Post-CallBack: Dump Raw 0x%X %d %d %d  to %s Start!", \
            info->media.RawInfo.RawAddr, \
            info->media.RawInfo.RawPitch, \
            info->media.RawInfo.RawWidth, \
            info->media.RawInfo.RawHeight, fn);
        AmbaFS_fwrite(info->media.RawInfo.RawAddr, \
            info->media.RawInfo.RawPitch*info->media.RawInfo.RawHeight, 1, raw);
        //AmbaFS_FSync(raw);
        AmbaFS_fclose(raw);
#endif
        yuvFlag = 0;
        raw_fno++;

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
		AppLibStillEnc_PostWBCalculation(info->media.CfaStatInfo);
        AmbaPrint("[Applib - StillEnc] raw2RawPPCB %d", info->CfaIndex);
        if (info->CfaIndex == TileNumber-1) {
            // whole CFA done, release buffer
            if (SingleCapContRaw3ARoiBuffAddrBufRaw) {
                if (AmbaKAL_BytePoolFree((void *)SingleCapContRaw3ARoiBuffAddrBufRaw) != OK)
                    AmbaPrint("[Applib - StillEnc] <SingleCaptureCont> memFree Fail raw3AROI!");
                SingleCapContRaw3ARoiBuffAddrBufRaw = NULL;
                SingleCapContRaw3ARoiBuffAddr = NULL;

            }
            if (SingleCapContRaw3AStatBuffAddrBufRaw) {
                if (AmbaKAL_BytePoolFree((void *)SingleCapContRaw3AStatBuffAddrBufRaw) != OK)
                    AmbaPrint("[Applib - StillEnc] <SingleCaptureCont> memFree Fail raw3AStat!");
                SingleCapContRaw3AStatBuffAddrBufRaw = NULL;
                SingleCapContRaw3AStatBuffAddr = NULL;
            }
        }

    } else if (info->StageCnt == 3) {

        char fn[32];
        char fn1[32];
        UINT8 *LumaAddr = NULL, *ChromaAddr = NULL;
        UINT16 Pitch = 0, Width = 0, Height = 0;

        UINT8 type = 0;

        if (info->media.YuvInfo.ThmLumaAddr) {
            sprintf(fn,"C:\\%04d_t.y", yuv_fno);
            sprintf(fn1,"C:\\%04d_t.uv", yuv_fno);
            LumaAddr = info->media.YuvInfo.ThmLumaAddr;
            ChromaAddr = info->media.YuvInfo.ThmChromaAddr;
            Pitch = info->media.YuvInfo.ThmPitch;
            Width = info->media.YuvInfo.ThmWidth;
            Height = info->media.YuvInfo.ThmHeight;
            type = 2;
        } else if (info->media.YuvInfo.ScrnLumaAddr) {
            sprintf(fn,"C:\\%04d_s.y", yuv_fno);
            sprintf(fn1,"C:\\%04d_s.uv", yuv_fno);
            LumaAddr = info->media.YuvInfo.ScrnLumaAddr;
            ChromaAddr = info->media.YuvInfo.ScrnChromaAddr;
            Pitch = info->media.YuvInfo.ScrnPitch;
            Width = info->media.YuvInfo.ScrnWidth;
            Height = info->media.YuvInfo.ScrnHeight;
            type = 1;
        } else if (info->media.YuvInfo.LumaAddr) {
            sprintf(fn,"C:\\%04d_m.y", yuv_fno);
            sprintf(fn1,"C:\\%04d_m.uv", yuv_fno);
            LumaAddr = info->media.YuvInfo.LumaAddr;
            ChromaAddr = info->media.YuvInfo.ChromaAddr;
            Pitch = info->media.YuvInfo.Pitch;
            Width = info->media.YuvInfo.Width;
            Height = info->media.YuvInfo.Height;
            type = 0;
        }

        // photo stamp
        if (StampSingleContCapCB.Process != NULL) {
            if ((LumaAddr != NULL) && (ChromaAddr != NULL)) {
                StampSingleContCapCB.Process(type, LumaAddr, ChromaAddr, Width, Height, Pitch);
            }
        }

        AmbaPrint("[Applib - StillEnc] <SingleCaptureCont> Post-CallBack:  Dump YUV(%d) (0x%X 0x%X) %d %d %d Start!", \
            info->media.YuvInfo.DataFormat, \
            LumaAddr, ChromaAddr, Pitch, Width, Height);
#if defined(DEBUG_APPLIB_PHOTO_S_CONT_CAP)
        {
        AMBA_FS_FILE *y = NULL;
        AMBA_FS_FILE *uv = NULL;
        char mode[3] = {'w','b','\0'};
        UINT16 i;

        y = AmbaFS_fopen((char const *)fn,(char const *) mode);
        for(i=0; i<Height; i++) {
            AmbaFS_fwrite(LumaAddr, Width, 1, y);
            LumaAddr += Pitch;
        }
        AmbaFS_FSync(y);
        AmbaFS_fclose(y);

        uv = AmbaFS_fopen((char const *)fn1,(char const *) mode);
        for(i=0; i<Height; i++) {
            AmbaFS_fwrite(ChromaAddr, Width, 1, uv);
            ChromaAddr += Pitch;
        }
        //AmbaFS_FSync(uv);
        AmbaFS_fclose(uv);
        //AmbaPrint("[Amp_UT] Dump YUV Done!");
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
            if (SingleCapContRawBuffAddrBufRaw) {
                if (AmbaKAL_BytePoolFree((void *)SingleCapContRawBuffAddrBufRaw) != OK)
                    AmbaPrintColor(RED, "[Applib - StillEnc] <SingleCaptureCont> Post-CallBack: MemFree Fail raw!");
                SingleCapContRawBuffAddrBufRaw = NULL;
                SingleCapContRawBuffAddr = NULL;
            }

            yuv_fno++;
        }
    } else {
        //nothing to do
    }

    return 0;
}


static AMP_STILLENC_POSTP_s PostSingleCapContCB = {.Process = AppLibStillEnc_SingleCaptureContPostCB};

int AppLibStillEnc_SingleCapContFreeBuf(void)
{
    int ReturnValue = 0;
    if (SingleCapContYuvBuffAddrBufRaw) {
        if (AmbaKAL_BytePoolFree((void *)SingleCapContYuvBuffAddrBufRaw) != OK)
            AmbaPrintColor(RED, "[Applib - StillEnc] <SingleCaptureCont> Post-CallBack: MemFree Fail YuvBuff!");
        SingleCapContYuvBuffAddrBufRaw = NULL;
        SingleCapContYuvBuffAddr = NULL;
        StampSingleContCapCB.Process = NULL;
    }
    if (SingleCapContRaw3ARoiBuffAddrBufRaw) {
        if (AmbaKAL_BytePoolFree((void *)SingleCapContRaw3ARoiBuffAddrBufRaw) != OK)
            AmbaPrintColor(RED, "[Applib - StillEnc] <SingleCaptureCont> Post-CallBack: MemFree Fail YuvBuff!");
        SingleCapContRaw3ARoiBuffAddrBufRaw = NULL;
        SingleCapContRaw3ARoiBuffAddr = NULL;
    }
    if (SingleCapContRaw3AStatBuffAddrBufRaw) {
        if (AmbaKAL_BytePoolFree((void *)SingleCapContRaw3AStatBuffAddrBufRaw) != OK)
            AmbaPrintColor(RED, "[Applib - StillEnc] <SingleCaptureCont> Post-CallBack: MemFree Fail YuvBuff!");
        SingleCapContRaw3AStatBuffAddrBufRaw = NULL;
        SingleCapContRaw3AStatBuffAddr = NULL;
    }
    if (SingleCapContScrnBuffAddrBufRaw) {
        if (AmbaKAL_BytePoolFree((void *)SingleCapContScrnBuffAddrBufRaw) != OK)
            AmbaPrintColor(RED, "[Applib - StillEnc] <SingleCaptureCont> Post-CallBack: MemFree Fail ScrnBuff!");
        SingleCapContScrnBuffAddrBufRaw = NULL;
        SingleCapContScrnBuffAddr = NULL;
    }
    if (SingleCapContThmBuffAddrBufRaw) {
        if (AmbaKAL_BytePoolFree((void *)SingleCapContThmBuffAddrBufRaw) != OK)
            AmbaPrintColor(RED, "[Applib - StillEnc] <SingleCaptureCont> Post-CallBack: MemFree Fail ThmBuff!");
        SingleCapContThmBuffAddrBufRaw = NULL;
        SingleCapContThmBuffAddr = NULL;
    }
    if (SingleCapContQvLCDBuffAddrBufRaw) {
        if (AmbaKAL_BytePoolFree((void *)SingleCapContQvLCDBuffAddrBufRaw) != OK)
            AmbaPrintColor(RED, "[Applib - StillEnc] <SingleCaptureCont> Post-CallBack: MemFree Fail QvLCDBuff!");
        SingleCapContQvLCDBuffAddrBufRaw = NULL;
        SingleCapContQvLCDBuffAddr = NULL;
    }
    if (SingleCapContQvHDMIBuffAddrBufRaw) {
        if (AmbaKAL_BytePoolFree((void *)SingleCapContQvHDMIBuffAddrBufRaw) != OK)
            AmbaPrintColor(RED, "[Applib - StillEnc] <SingleCaptureCont> Post-CallBack: MemFree Fail QvHDMI!");
        SingleCapContQvHDMIBuffAddrBufRaw = NULL;
        SingleCapContQvHDMIBuffAddr = NULL;
    }

    if (SingleCapContAllocateFromDSPFlag) {
        AppLibComSvcMemMgr_FreeDSPMemory();
        SingleCapContAllocateFromDSPFlag = 0;
        SingleCapContRaw3ARoiBuffAddr = NULL;
        SingleCapContYuvBuffAddr = NULL;
        SingleCapContRaw3AStatBuffAddr = NULL;
        SingleCapContScrnBuffAddr = NULL;
        SingleCapContThmBuffAddr = NULL;
        SingleCapContQvHDMIBuffAddr = NULL;
        SingleCapContQvLCDBuffAddr = NULL;
        StampSingleContCapCB.Process = NULL;
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
int AppLibStillEnc_SingleContCapture(UINT32 iso, UINT32 cmpr, UINT32 targetSize, UINT8 encodeLoop)
{
    int ReturnValue;
    void *TempPtrBuf;
    UINT16 RawPitch = 0, RawWidth = 0, RawHeight = 0;
    UINT16 YuvWidth = 0, YuvHeight = 0, ScrnW = 0, ScrnH = 0, ThmW = 0, ThmH = 0;
    UINT32 RawSize = 0, YuvSize = 0, ScrnSize = 0, ThmSize = 0;
    UINT16 QvLCDW = 0, QvLCDH = 0, QvHDMIW = 0, QvHDMIH = 0;
    UINT32 QvLCDSize = 0, QvHDMISize = 0, roiSize = 0, raw3AStatSize = 0;

    UINT8 *SstageAddr = NULL;
    UINT32 TotalScriptSize = 0, TotalStageNum = 0;
    AMP_SENC_SCRPT_GENCFG_s *GenScrpt;
    AMP_SENC_SCRPT_RAW2RAW_s *Raw2RawScrpt;

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
    AmbaPrint("[Applib - StillEnc] <SingleCaptureCont> raw(%u %u %u)", RawPitch, RawWidth, RawHeight);

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

    QvLCDW = QvDchanWidth;
    QvLCDH = QvDchanHeight;
    QvHDMIW = QvFchanWidth;
    QvHDMIH = QvFchanHeight;
    QvLCDSize = QvLCDW*QvLCDH*2;
    QvLCDSize += (QvLCDSize*10)/100;
    QvHDMISize = QvHDMIW*QvHDMIH*2;
    QvHDMISize += (QvHDMISize*10)/100;
    AmbaPrint("[Applib - StillEnc] <SingleCaptureCont> qvLCD(%u) qvHDMI(%u)!", QvLCDSize, QvHDMISize);

    roiSize = TileNumber*sizeof(AMBA_DSP_EVENT_STILL_CFA_3A_DATA_s);
    raw3AStatSize = sizeof(AMBA_DSP_EVENT_STILL_CFA_3A_DATA_s);

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
        SingleCapContRawBuffAddr = bufAddr;
        bufAddr += RawSize;
        AmbaPrint("[Applib - StillEnc] <SingleCaptureCont> rawBuffAddr (0x%08X) (%u)!", SingleCapContRawBuffAddr, RawSize);

        SingleCapContRaw3ARoiBuffAddr = bufAddr;
        bufAddr += roiSize;
        AmbaPrint("UT_singleCaptureCont]raw3AStatBuffAddr (0x%08X) (%u)!", SingleCapContRaw3ARoiBuffAddr, roiSize);

        SingleCapContRaw3AStatBuffAddr = bufAddr;
        bufAddr += raw3AStatSize;
        AmbaPrint("UT_singleCaptureCont]raw3AStatBuffAddr (0x%08X) (%u)!", SingleCapContRaw3AStatBuffAddr, raw3AStatSize);

        SingleCapContYuvBuffAddr = bufAddr;
        bufAddr += YuvSize;
        AmbaPrint("[Applib - StillEnc] <SingleCaptureCont> yuvBuffAddr (0x%08X)!", SingleCapContYuvBuffAddr);

        SingleCapContScrnBuffAddr = bufAddr;
        bufAddr += ScrnSize;
        AmbaPrint("[Applib - StillEnc] <SingleCaptureCont> scrnBuffAddr (0x%08X)!", SingleCapContScrnBuffAddr);

        SingleCapContThmBuffAddr = bufAddr;
        bufAddr += ThmSize;
        AmbaPrint("[Applib - StillEnc] <SingleCaptureCont> thmBuffAddr (0x%08X)!", SingleCapContThmBuffAddr);

        SingleCapContQvLCDBuffAddr = bufAddr;
        bufAddr += QvLCDSize;
        AmbaPrint("[Applib - StillEnc] <SingleCaptureCont> qvLCDBuffaddr (0x%08X)!", SingleCapContQvLCDBuffAddr);

        SingleCapContQvHDMIBuffAddr = bufAddr;
        AmbaPrint("[Applib - StillEnc] <SingleCaptureCont> qvHDMIBuffaddr (0x%08X)!", SingleCapContQvHDMIBuffAddr);
    }
#else
    if (SingleCapContRawBuffAddr == NULL) {
        ReturnValue = AmpUtil_GetAlignedPool(APPLIB_G_MMPL, &TempPtrBuf, (void **)&SingleCapContRawBuffAddrBufRaw, RawSize, 32);
        if (ReturnValue != OK) {
            /**Allocate DSP memory if NC memory not encough*/
            AppLibComSvcMemMgr_AllocateDSPMemory(&SingleCapContRawBuffAddr,RawSize);
            SingleCapContAllocateFromDSPFlag = 1;
            AmbaPrint("[Applib - StillEnc] <SingleCaptureCont> NC_DDR alloc raw fail Allocate DSP MEM instead(%u)!", RawSize);
        } else {
            SingleCapContRawBuffAddr = (UINT8*)TempPtrBuf;
            AmbaPrint("[Applib - StillEnc] <SingleCaptureCont> rawBuffAddr (0x%08X) (%u)!", SingleCapContRawBuffAddr, RawSize);
        }
    } else {
        AmbaPrint("[Applib - StillEnc] <SingleCaptureCont> already allocate rawBuffAddr (0x%08X) (%u)!", SingleCapContRawBuffAddr, RawSize);
    }

    if (SingleCapContRaw3ARoiBuffAddr == NULL) {
        ReturnValue = AmpUtil_GetAlignedPool(APPLIB_G_MMPL, &TempPtrBuf, (void **)&SingleCapContRaw3ARoiBuffAddrBufRaw, roiSize, 32);
        if (ReturnValue != OK) {
            AppLibComSvcMemMgr_AllocateDSPMemory(&SingleCapContRaw3ARoiBuffAddr,roiSize);
            SingleCapContAllocateFromDSPFlag = 1;
            AmbaPrint("[Applib - StillEnc] <SingleCaptureCont> NC_DDR alloc raw fail Allocate DSP MEM instead(%u)!", roiSize);
        } else {
            SingleCapContRaw3ARoiBuffAddr = (UINT8*)TempPtrBuf;
            AmbaPrint("[Applib - StillEnc] <SingleCaptureCont> SingleCapContRaw3ARoiBuffAddr (0x%08X) (%u)!", SingleCapContRaw3ARoiBuffAddr, roiSize);
        }
    } else {
        AmbaPrint("[Applib - StillEnc] <SingleCaptureCont> already allocate SingleCapContRaw3ARoiBuffAddr (0x%08X) (%u)!", SingleCapContRaw3ARoiBuffAddr, roiSize);
    }

    if (SingleCapContRaw3AStatBuffAddr == NULL) {
        ReturnValue = AmpUtil_GetAlignedPool(APPLIB_G_MMPL, &TempPtrBuf, (void **)&SingleCapContRaw3AStatBuffAddrBufRaw, raw3AStatSize, 32);
        if (ReturnValue != OK) {
            AppLibComSvcMemMgr_AllocateDSPMemory(&SingleCapContRaw3AStatBuffAddr,raw3AStatSize);
            SingleCapContAllocateFromDSPFlag = 1;
            AmbaPrint("[Applib - StillEnc] <SingleCaptureCont> NC_DDR alloc raw 3A stat fail Allocate DSP MEM instead(%u)!", raw3AStatSize);
        } else {
            SingleCapContRaw3AStatBuffAddr = (UINT8*)TempPtrBuf;
            AmbaPrint("[Applib - StillEnc] <SingleCaptureCont> raw3AStatBuffAddr (0x%08X) (%u)!", SingleCapContRaw3AStatBuffAddr, raw3AStatSize);
        }
    } else {
        AmbaPrint("[Applib - StillEnc] <SingleCaptureCont>  already allocate raw3AStatBuffAddr (0x%08X) (%u)!", SingleCapContRaw3AStatBuffAddr, raw3AStatSize);
    }

    if (SingleCapContYuvBuffAddr == NULL) {
        ReturnValue = AmpUtil_GetAlignedPool(APPLIB_G_MMPL, &TempPtrBuf, (void **)&SingleCapContYuvBuffAddrBufRaw, YuvSize, 32);
        if (ReturnValue != OK) {
            AppLibComSvcMemMgr_AllocateDSPMemory(&SingleCapContYuvBuffAddr,YuvSize);
            SingleCapContAllocateFromDSPFlag = 1;
            AmbaPrint("[Applib - StillEnc] <SingleCaptureCont> NC_DDR alloc yuv_main fail Allocate DSP MEM instead(%u)!", YuvSize);
        } else {
            SingleCapContYuvBuffAddr = (UINT8*)TempPtrBuf;
            AmbaPrint("[Applib - StillEnc] <SingleCaptureCont> yuvBuffAddr (0x%08X)!", SingleCapContYuvBuffAddr);
        }
    } else {
       AmbaPrint("[Applib - StillEnc] <SingleCaptureCont> already allocate yuvBuffAddr (0x%08X)!", SingleCapContYuvBuffAddr);
    }

    if (SingleCapContScrnBuffAddr == NULL) {
        ReturnValue = AmpUtil_GetAlignedPool(APPLIB_G_MMPL, &TempPtrBuf, (void **)&SingleCapContScrnBuffAddrBufRaw, ScrnSize*1, 32);
        if (ReturnValue != OK) {
            AppLibComSvcMemMgr_AllocateDSPMemory(&SingleCapContScrnBuffAddr,ScrnSize);
            SingleCapContAllocateFromDSPFlag = 1;
            AmbaPrint("[Applib - StillEnc] <SingleCaptureCont> NC_DDR alloc yuv_scrn fail Allocate DSP MEM instead(%u)!", ScrnSize*1);
        } else {
            SingleCapContScrnBuffAddr = (UINT8*)TempPtrBuf;
            AmbaPrint("[Applib - StillEnc] <SingleCaptureCont> scrnBuffAddr (0x%08X)!", SingleCapContScrnBuffAddr);
        }
    } else {
        AmbaPrint("[Applib - StillEnc] <SingleCaptureCont> already allocate scrnBuffAddr (0x%08X)!", SingleCapContScrnBuffAddr);
    }

    if (SingleCapContThmBuffAddr == NULL) {
        ReturnValue = AmpUtil_GetAlignedPool(APPLIB_G_MMPL, &TempPtrBuf, (void **)&SingleCapContThmBuffAddrBufRaw, ThmSize*1, 32);
        if (ReturnValue != OK) {
            AppLibComSvcMemMgr_AllocateDSPMemory(&SingleCapContThmBuffAddr,ThmSize);
            SingleCapContAllocateFromDSPFlag = 1;
            AmbaPrint("[Applib - StillEnc] <SingleCaptureCont> NC_DDR alloc yuv_thm fail Allocate DSP MEM instead(%u)!", ThmSize*1);
        } else {
            SingleCapContThmBuffAddr = (UINT8*)TempPtrBuf;
            AmbaPrint("[Applib - StillEnc] <SingleCaptureCont> thmBuffAddr (0x%08X)!", SingleCapContThmBuffAddr);
        }
    } else {
        AmbaPrint("[Applib - StillEnc] <SingleCaptureCont> already allocate thmBuffAddr (0x%08X)!", SingleCapContThmBuffAddr);
    }

    if (SingleCapContQvLCDBuffAddr == NULL) {
        ReturnValue = AmpUtil_GetAlignedPool(APPLIB_G_MMPL, &TempPtrBuf, (void **)&SingleCapContQvLCDBuffAddrBufRaw, QvLCDSize*1, 32);
        if (ReturnValue != OK) {
            AppLibComSvcMemMgr_AllocateDSPMemory(&SingleCapContQvLCDBuffAddr,QvLCDSize);
            SingleCapContAllocateFromDSPFlag = 1;
            AmbaPrint("[Applib - StillEnc] <SingleCaptureCont> NC_DDR alloc yuv_lcd fail Allocate DSP MEM instead(%u)!", QvLCDSize*1);
        } else {
            SingleCapContQvLCDBuffAddr = (UINT8*)TempPtrBuf;
            AmbaPrint("[Applib - StillEnc] <SingleCaptureCont> qvLCDBuffaddr (0x%08X)!", SingleCapContQvLCDBuffAddr);
        }
    } else {
        AmbaPrint("[Applib - StillEnc] <SingleCaptureCont> already allocate qvLCDBuffaddr (0x%08X)!", SingleCapContQvLCDBuffAddr);
    }

    if (SingleCapContQvHDMIBuffAddr == NULL) {
        ReturnValue = AmpUtil_GetAlignedPool(APPLIB_G_MMPL, &TempPtrBuf, (void **)&SingleCapContQvHDMIBuffAddrBufRaw, QvHDMISize*1, 32);
        if (ReturnValue != OK) {
            AppLibComSvcMemMgr_AllocateDSPMemory(&SingleCapContQvHDMIBuffAddr,QvHDMISize);
            SingleCapContAllocateFromDSPFlag = 1;
            AmbaPrint("[Applib - StillEnc] <SingleCaptureCont> NC_DDR alloc yuv_hdmi fail Allocate DSP MEM instead(%u)!", QvHDMISize*1);
        } else {
            SingleCapContQvHDMIBuffAddr = (UINT8*)TempPtrBuf;
            AmbaPrint("[Applib - StillEnc] <SingleCaptureCont> qvHDMIBuffaddr (0x%08X)!", SingleCapContQvHDMIBuffAddr);
        }
    } else {
        AmbaPrint("[Applib - StillEnc] <SingleCaptureCont> already allocate qvHDMIBuffaddr (0x%08X)!", SingleCapContQvHDMIBuffAddr);
    }
#endif

    ReturnValue = AmpUtil_GetAlignedPool(APPLIB_G_MMPL, &TempPtrBuf, (void **)&SingleCapContScriptAddrBufRaw, 128*10, 32); //TBD
    if (ReturnValue != OK) {
        AmbaPrint("[Applib - StillEnc] <SingleCaptureCont> NC_DDR alloc scriptAddr fail (%u)!", 128*10);
    } else {
        SingleCapContScriptAddr = (UINT8*)TempPtrBuf;
        AmbaPrint("[Applib - StillEnc] <SingleCaptureCont> scriptAddr (0x%08X) (%d)!", SingleCapContScriptAddr, 128*10);
    }

    /* Step3. fill script */
    //general config
    SstageAddr = SingleCapContScriptAddr;
    GenScrpt = (AMP_SENC_SCRPT_GENCFG_s *)SstageAddr;
    memset(GenScrpt, 0x0, sizeof(AMP_SENC_SCRPT_GENCFG_s));
    GenScrpt->Cmd = SENC_GENCFG;
    GenScrpt->RawEncRepeat = 0;
    GenScrpt->RawToCap = 1;
    GenScrpt->StillProcMode = iso;

    GenScrpt->QVConfig.DisableLCDQV = 0;//(qvDisplayCfg == 0)? 1: 0;
    GenScrpt->QVConfig.DisableHDMIQV = 1;//(qvDisplayCfg == 0)? 1: 0;
    GenScrpt->QVConfig.LCDDataFormat = AMP_YUV_422;
    GenScrpt->QVConfig.LCDLumaAddr = SingleCapContQvLCDBuffAddr;
    GenScrpt->QVConfig.LCDChromaAddr = SingleCapContQvLCDBuffAddr + QvLCDSize/2;
    GenScrpt->QVConfig.LCDWidth = QvDchanWidth;
    GenScrpt->QVConfig.LCDHeight = QvDchanHeight;
    GenScrpt->QVConfig.HDMIDataFormat = AMP_YUV_422;
    GenScrpt->QVConfig.HDMILumaAddr = SingleCapContQvHDMIBuffAddr;
    GenScrpt->QVConfig.HDMIChromaAddr = SingleCapContQvHDMIBuffAddr + QvHDMISize/2;
    GenScrpt->QVConfig.HDMIWidth = QvFchanWidth;
    GenScrpt->QVConfig.HDMIHeight = QvFchanHeight;
    GenScrpt->b2LVCfg = AMP_ENC_SCRPT_B2LV_NONE;
    //AutoBackToLiveview = (GenScrpt->b2LVCfg)? 1: 0;
    GenScrpt->ScrnEnable = 1;
    GenScrpt->ThmEnable = 1;
    GenScrpt->PreProc = &PreSingleCapContCB;
    GenScrpt->PostProc = &PostSingleCapContCB;

    GenScrpt->MainBuf.ColorFmt = AMP_YUV_422;
    GenScrpt->MainBuf.Width = GenScrpt->MainBuf.Pitch = YuvWidth;
    GenScrpt->MainBuf.Height = YuvHeight;
    GenScrpt->MainBuf.LumaAddr = SingleCapContYuvBuffAddr;
    GenScrpt->MainBuf.ChromaAddr = SingleCapContYuvBuffAddr + YuvSize/2;
    GenScrpt->MainBuf.AOI.X = 0;
    GenScrpt->MainBuf.AOI.Y = 0;
    GenScrpt->MainBuf.AOI.Width = StillCapConfigData->FullviewWidth;
    GenScrpt->MainBuf.AOI.Height = StillCapConfigData->FullviewHeight;

    GenScrpt->ScrnBuf.ColorFmt = AMP_YUV_422;
    GenScrpt->ScrnBuf.Width = GenScrpt->ScrnBuf.Pitch = ScrnW;
    GenScrpt->ScrnBuf.Height = ScrnH;
    GenScrpt->ScrnBuf.LumaAddr = SingleCapContScrnBuffAddr;
    GenScrpt->ScrnBuf.ChromaAddr = SingleCapContScrnBuffAddr + ScrnSize/2;
    GenScrpt->ScrnBuf.AOI.X = 0;
    GenScrpt->ScrnBuf.AOI.Y = 0;
    GenScrpt->ScrnBuf.AOI.Width = StillCapConfigData->ScreennailActiveWidth;
    GenScrpt->ScrnBuf.AOI.Height = StillCapConfigData->ScreennailActiveHeight;
    GenScrpt->ScrnWidth = StillCapConfigData->ScreennailWidth;
    GenScrpt->ScrnHeight = StillCapConfigData->ScreennailHeight;

    GenScrpt->ThmBuf.ColorFmt = AMP_YUV_422;
    GenScrpt->ThmBuf.Width = GenScrpt->ThmBuf.Pitch = ThmW;
    GenScrpt->ThmBuf.Height = ThmH;
    GenScrpt->ThmBuf.LumaAddr = SingleCapContThmBuffAddr;
    GenScrpt->ThmBuf.ChromaAddr = SingleCapContThmBuffAddr + ThmSize/2;
    GenScrpt->ThmBuf.AOI.X = 0;
    GenScrpt->ThmBuf.AOI.Y = 0;
    GenScrpt->ThmBuf.AOI.Width = StillCapConfigData->ThumbnailActiveWidth;
    GenScrpt->ThmBuf.AOI.Height = StillCapConfigData->ThumbnailActiveHeight;
    GenScrpt->ThmWidth = StillCapConfigData->ThumbnailWidth;
    GenScrpt->ThmHeight = StillCapConfigData->ThumbnailHeight;

    if (targetSize) {
        AmbaPrint("[Applib - StillEnc] <SingleCaptureCont> Target Size %u Kbyte", targetSize);
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
    AmbaPrint("[Applib - StillEnc] <SingleCaptureCont> Stage #%d  0x%08X", TotalStageNum, SstageAddr);
    TotalStageNum ++;


    //raw cap config
    SstageAddr = SingleCapContScriptAddr + TotalScriptSize;
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
    RawCapScrpt->FvRawBuf.Buf = SingleCapContRawBuffAddr;
    RawCapScrpt->FvRawBuf.Width = RawWidth;
    RawCapScrpt->FvRawBuf.Height = RawHeight;
    RawCapScrpt->FvRawBuf.Pitch = RawPitch;
    RawCapScrpt->FvBufRule = AMP_ENC_SCRPT_BUFF_FIXED;
    RawCapScrpt->FvRingBufSize = RawSize*1;
    RawCapScrpt->CapCB.RawCapCB = AppLibStillEnc_RawCapCB;
    TotalScriptSize += ALIGN_128(sizeof(AMP_SENC_SCRPT_RAWCAP_s));
    AmbaPrint("[Applib - StillEnc] <SingleCaptureCont> Stage #%d  0x%08X", TotalStageNum, SstageAddr);
    TotalStageNum ++;

    //raw2raw config
    SstageAddr = SingleCapContScriptAddr + TotalScriptSize;
    Raw2RawScrpt = (AMP_SENC_SCRPT_RAW2RAW_s *)SstageAddr;
    memset(Raw2RawScrpt, 0x0, sizeof(AMP_SENC_SCRPT_RAW2RAW_s));
    Raw2RawScrpt->Cmd = SENC_RAW2RAW;
    Raw2RawScrpt->SrcRawType = (cmpr)? AMP_STILLENC_RAW_COMPR: AMP_STILLENC_RAW_UNCOMPR;
    Raw2RawScrpt->SrcRawBuf.Buf = SingleCapContRawBuffAddr;
    Raw2RawScrpt->SrcRawBuf.Width = RawWidth;
    Raw2RawScrpt->SrcRawBuf.Height = RawHeight;
    Raw2RawScrpt->SrcRawBuf.Pitch = RawPitch;
    Raw2RawScrpt->TileNumber = TileNumber;
    if (TileNumber == 1) { //full frame
        AMP_STILLENC_RAW2RAW_ROI_s *roi = (AMP_STILLENC_RAW2RAW_ROI_s *)SingleCapContRaw3ARoiBuffAddr;
        roi->RoiColStart = 0;
        roi->RoiRowStart = 0;
        roi->RoiWidth = RawWidth;
        roi->RoiHeight = RawHeight;
    } else if (TileNumber == 3) {
        AMP_STILLENC_RAW2RAW_ROI_s *roi = (AMP_STILLENC_RAW2RAW_ROI_s *)SingleCapContRaw3ARoiBuffAddr;
        roi[0].RoiColStart = 64;
        roi[0].RoiRowStart = 64;
        roi[0].RoiWidth = 128;
        roi[0].RoiHeight = 128;

        roi[1].RoiColStart = 2368;
        roi[1].RoiRowStart = 64;
        roi[1].RoiWidth = 512;
        roi[1].RoiHeight = 512;

        roi[2].RoiColStart = 128;
        roi[2].RoiRowStart = 128;
        roi[2].RoiWidth = 256;
        roi[2].RoiHeight = 256;
    }
    Raw2RawScrpt->TileListAddr = (UINT32)SingleCapContRaw3ARoiBuffAddr;
    Raw2RawScrpt->Raw3AStatAddr = (UINT32)SingleCapContRaw3AStatBuffAddr;
    Raw2RawScrpt->Raw3AStatSize = sizeof(AMBA_DSP_EVENT_STILL_CFA_3A_DATA_s);
    TotalScriptSize += ALIGN_128(sizeof(AMP_SENC_SCRPT_RAW2RAW_s));
    AmbaPrint("[Applib - StillEnc] <SingleCaptureCont> Stage #%d  0x%08X", TotalStageNum, SstageAddr);
    TotalStageNum ++;

    //raw2yuv config
    SstageAddr = SingleCapContScriptAddr + TotalScriptSize;
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
    AmbaPrint("[Applib - StillEnc] <SingleCaptureCont> Stage #%d  0x%08X", TotalStageNum, SstageAddr);
    TotalStageNum ++;

    //yuv2jpg config
    SstageAddr = SingleCapContScriptAddr + TotalScriptSize;
    Yuv2JpgScrpt = (AMP_SENC_SCRPT_YUV2JPG_s *)SstageAddr;
    memset(Yuv2JpgScrpt, 0x0, sizeof(AMP_SENC_SCRPT_YUV2JPG_s));
    Yuv2JpgScrpt->Cmd = SENC_YUV2JPG;
    Yuv2JpgScrpt->YuvBufRule = AMP_ENC_SCRPT_BUFF_FIXED;
    Yuv2JpgScrpt->YuvRingBufSize = 0;
    TotalScriptSize += ALIGN_128(sizeof(AMP_SENC_SCRPT_YUV2JPG_s));
    AmbaPrint("[Applib - StillEnc] <SingleCaptureCont> Stage #%d  0x%08X", TotalStageNum, SstageAddr);
    TotalStageNum ++;

    //script config
    Scrpt.mode = AMP_SCRPT_MODE_STILL;
    Scrpt.StepPreproc = NULL;
    Scrpt.StepPostproc = NULL;
    Scrpt.ScriptStartAddr = (UINT32)SingleCapContScriptAddr;
    Scrpt.ScriptTotalSize = TotalScriptSize;
    Scrpt.ScriptStageNum = TotalStageNum;
    AmbaPrint("[Applib - StillEnc] <SingleCaptureCont> Scrpt addr 0x%X, Sz %uByte, stg %d", Scrpt.ScriptStartAddr, Scrpt.ScriptTotalSize, Scrpt.ScriptStageNum);

    AppLibImage_EnableImgSchdlr(0,0);
    /* Step4. execute script */
    AmpEnc_RunScript(StillEncPipe, &Scrpt, AMP_ENC_FUNC_FLAG_NONE);

    /* Step4. release script */
    AmbaPrint("[Applib - StillEnc] <SingleCaptureCont> [0x%08X] memFree", SingleCapContScriptAddrBufRaw);
    if (AmbaKAL_BytePoolFree((void *)SingleCapContScriptAddrBufRaw) != OK) {
        AmbaPrintColor(RED,"[Applib - StillEnc] <SingleCaptureCont> memFree Fail (scrpt)");
    }
    SingleCapContScriptAddrBufRaw = NULL;

    AmbaPrint("[Applib - StillEnc] <SingleCaptureCont> memFree Done");

    return 0;
}


/**
 *  @brief To capture the photo with the continuous mode.
 *
 *  To capture the photo with the continuous mode.
 *
 *  @return >=0 success, <0 failure
 */
int AppLibStillEnc_CaptureSingleCont(void)
{
    return AppLibStillEnc_SingleContCapture(1, 1, 0, 0);
}

/**
 *  @brief  Register single capture StampProc Callback
 *
 *  @param [in] info stamp information
 *
 *  @return 0 - success, -1 - fail
 */
int AppLibStillEnc_SingleContCapRegisterStampCB(APPLIB_STILLENC_STAMP_SETTING_s stampSetting)
{
    StampSingleContCapCB.StampInfo = stampSetting;
    StampSingleContCapCB.Process = AppLibStillEnc_SingleContCaptureStampCB;
    return 0;
}



