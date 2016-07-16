/**
 * @file src/app/connected/applib/src/recorder/ApplibRecorder_StillEncUtility.c
 *
 * Implementation utility of still capture.
 *
 * History:
 *    2013/12/27 - [Martin Lai] created file
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
#include <calibration/bpc/ApplibCalibBpc.h>
#include <calibration/vig/ApplibCalibVig.h>
#include "ApplibRecorder_StillEncUtility.h"
#define DEBUG_APPLIB_PHOTO_UTILITY
#if defined(DEBUG_APPLIB_PHOTO_UTILITY)
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

/*
 * JPEG QTable
 */
static UINT8 StdJpegQTable[128] = {
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

UINT8 __attribute__((aligned(32))) ApplibJpegQTable[3][128] = {
    [0] = { //Main
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
    },
    [1] = { //Thm
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
    },
    [2] = { //Scrn
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
    }
};



/* Initial JPEG DQT */
void AppLibStillEnc_initJpegDqt(UINT8 *qTable, int quality)
{
    int i, scale, temp;

    /** for jpeg brc; return the quantization table*/
    if (quality==-1) {
        memcpy(qTable, StdJpegQTable, 128);
        return;
    }

    if (quality <= 0) {
        scale = 5000;
    } else if (quality >= 100) {
        scale = 0;
    } else if (quality < 50) {
        scale = (5000/quality);
    } else {
        scale = (200-quality*2);
    }

    for (i=0; i<128; i++) {
        temp = ((long) StdJpegQTable[i] * scale + 50L) / 100L;
        /* limit the values to the valid range */
        if (temp <= 0L) temp = 1L;
        if (temp > 255L) temp = 255L; /* max quantizer needed for baseline */
        qTable[i] = temp;
    }
}

static UINT8 sensorCurve[36];
static UINT16 *linearFv2;
static UINT16 *linearBpp;
const UINT32 a7Imx117Fv2SumLiso = 19644;
const UINT32 a7Imx117Fv2SumHiso = 19650;
const UINT16 a7Imx117AFTileNum = 40;
const UINT16 a7Imx117AFActiveWidth = 448;
const UINT16 a7Imx117AFActiveHeight = 816;


static UINT8 AppLibStillEnc_InterpolatedQFromCurve(UINT8 number, UINT8 *curveX, UINT16 *curveY, UINT16 Bpp)
{
    UINT32 tmp;
    UINT8 pq, i, idx;

    idx = 0;
    for (i=0; i<number; i++) {
        if (curveY[i] >= Bpp) {
            idx = i;
            i = 0xfe; //leave loop
        }
    }

    if (i == number) { //out of curve upper bound
        pq = curveX[number -1];
    } else if (idx == 0) { //out of curve lower bound
        pq = curveX[0];
    } else {
        tmp = curveY[idx] - curveY[idx-1];
        pq = curveX[idx] - (((UINT32)(curveY[idx]-Bpp)*(UINT32)(curveX[idx]-curveX[idx-1])  + (tmp>>1)) / tmp);
    }

    return pq;
}

static UINT8 AppLibStillEnc_PredictQPerComplex(UINT32 fv2, UINT8 curveNum, UINT8 *curveQ, UINT16 *curveNBpp, UINT16 targetBpp)
{
    UINT8 pq; /* predicted q*/
    UINT32 bpp, tmp; /** bit per pixel in q = 85*/
    UINT16 nbpp; /* normalized bit per pixel*/

    tmp = linearFv2[1] - linearFv2[0];
    if (fv2 > linearFv2[0]) {
        bpp = linearBpp[0] + (((UINT32)(fv2 - linearFv2[0])*(UINT32)(linearBpp[1] - linearBpp[0]) + (tmp>>1)) / tmp);
        nbpp = (targetBpp<<10)/bpp; /** 10-bit resolution*/
    } else { /** fv2 < linear_fv2_x[0]*/
        bpp = linearBpp[0] - (((UINT32)(linearFv2[0] - fv2)*(UINT32)(linearBpp[1] - linearBpp[0]) + (tmp>>1)) / tmp);
        nbpp = (targetBpp<<10)/bpp; /** 10-bit resolution*/
    }
    AmbaPrint("[Applib - StillEnc] <StillEncUtility> PredictQPerComplex: bpp %u, nbpp %u", bpp, nbpp);
    pq = AppLibStillEnc_InterpolatedQFromCurve(curveNum, curveQ, curveNBpp, nbpp);

    return pq;
}

UINT32 AppLibStillEnc_jpegBRCPredictCB(UINT16 targetBpp, UINT32 stillProc, UINT8* predictQ, UINT8 *curveNum, UINT32 *curveAddr)
{
    UINT32 afDataFv2Sum = 0;
    UINT16 activeWidth, activeHeight, tileNum;

    UINT8 sensorCurveQ[12] = {50, 60, 65, 70, 75, 80, 85, 90, 93, 96, 98, 100};
    /** normalized bit per pixel with 6.10 fix format; normalized at q = 85*/
    UINT16 sensorCurveNBpp[12] = {481, 553, 610, 666, 737, 850, 1024, 1321, 1618, 2294, 3021, 4690};

#if 1 //refer to MMT9F001
    /** normalized fv2 with 20-bit_resolution*/
    UINT16 sensorFv2Hiso[2] = {799, 6642};
    UINT16 sensorFv2Normal[2] = {701, 7806};
    UINT16 sensorFv2Liso[2] = {726, 7343};
    /** bit per pixel with 12-bit resolution*/
    UINT16 sensorBppHiso[2] = {3771, 6960};
    UINT16 sensorBppNormal[2] = {6192, 11302};
    UINT16 sensorBppLiso[2] = {3957, 7926};
#else
    /** normalized fv2 with 20-bit_resolution*/
    UINT16 sensorFv2Hiso[2] = {3713, 15506};
    UINT16 sensorFv2Normal[2] = {3713, 15506};
    UINT16 sensorFv2Liso[2] = {3713, 15506};
    /** bit per pixel with 12-bit resolution*/
    UINT16 sensorBppHiso[2] = {2847, 5788};
    UINT16 sensorBppNormal[2] = {4185, 8509};
    UINT16 sensorBppLiso[2] = {2847, 5788};
#endif

    memcpy(&sensorCurve[0], sensorCurveQ, 12);
    memcpy(&sensorCurve[12], (UINT8 *)sensorCurveNBpp, 24);

    /* get af data*/
#if 0
{
    UINT32 loopCnt = 0
    af_tiles_value_t afData;
    af_tiles_t afInfo;

    //AMP_img_sensor_cmd(MW_IMG_GET_AF_STAT_INFO, 0, &afInfo);
    activeWidth = afInfo.active_width;
    activeHeight = afInfo.active_height;
    if (activeWidth == 0 || activeHeight == 0) {
        AmbaPrintColor(BLUE, "JPGBRC: The active width %u or height %u is zero!!!", activeWidth, activeHeight);
        activeWidth = 448;
        activeHeight = 816;
    }

    //AMP_img_algo_cmd(MW_IA_GET_AF_TILES_VALUE, &afData, 0);
    tileNum = afData.tiles_no;
    for (loopCnt = 0; loopCnt < tileNum; loopCnt++) { /* accumulate fv2 */
        afDataFv2Sum += afData.fv2[loopCnt];
    }
}
#else
    activeWidth = a7Imx117AFActiveWidth;
    activeHeight = a7Imx117AFActiveHeight;

    if (stillProc == 0) //HISO
        afDataFv2Sum = a7Imx117Fv2SumHiso;
    else
        afDataFv2Sum = a7Imx117Fv2SumLiso;
    tileNum = a7Imx117AFTileNum;

#endif

    if (tileNum == 0) {
        AmbaPrintColor(BLUE, "[Applib - StillEnc] <StillEncUtility> JPGBRC: The tiles number is zero!!!");
        afDataFv2Sum = 500;
    } else {
        afDataFv2Sum /= tileNum; /* average fv2*/
    }

    /** normalize fv2*/
    afDataFv2Sum = (((afDataFv2Sum<<10)/activeWidth)<<10)/activeHeight;

    *curveAddr = (UINT32)sensorCurve;
    *curveNum = 12;
    if (stillProc == 0) {
        linearFv2 = sensorFv2Hiso;
        linearBpp = sensorBppHiso;
    } else if (stillProc == 1) {
        linearFv2 = sensorFv2Liso;
        linearBpp = sensorBppLiso;
    } else if (stillProc == 2) {
        linearFv2 = sensorFv2Normal;
        linearBpp = sensorBppNormal;
    } else {
        AmbaPrint("[Applib - StillEnc] <StillEncUtility> JPGBRC: Unknown stillProc %d", stillProc);
        linearFv2 = sensorFv2Liso;
        linearBpp = sensorBppLiso;
    }

    /** calculate tarbet bit per pixel with 8-bit resolution*/
    *predictQ = AppLibStillEnc_PredictQPerComplex(afDataFv2Sum,
        *curveNum, &sensorCurve[0], (UINT16 *)&sensorCurve[12], targetBpp); /** predict quality*/
    AmbaPrintColor(GREEN, "[Applib - StillEnc] <StillEncUtility> JPGBRC: tbpp %u, fv2 %u, q %u", targetBpp, afDataFv2Sum, *predictQ);

    return 0;
}

/* Deliver CFA stat to 3A and do still post WB calculation */
UINT32 AppLibStillEnc_PostWBCalculation(AMBA_DSP_EVENT_STILL_CFA_3A_DATA_s *cfaStat)
{
    UINT32 ImgChipNo = 0;
    int speed = 0;
    // Provide cfa stat for 3A
    AmbaImg_Proc_Cmd(MW_IP_SET_CFA_3A_STAT, ImgChipNo, (UINT32) cfaStat, 0);
    // Ask 3A to do still post WB calculation
    AmbaImg_Proc_Cmd(MW_IP_AMBA_POST_STILL, ImgChipNo, (UINT32) speed, 0);
    return OK;
}



static UINT32 G_iso = 1;
//static UINT8 qvLcdShowBuffIndex = 0;
//static UINT8 qvTvShowBuffIndex = 0;
static UINT8 *s_qvLcdShowBuffAddrOdd = NULL;
static UINT8 *s_qvLcdShowBuffAddrEven = NULL;
static UINT8 *s_qvTvShowBuffAddrOdd = NULL;
static UINT8 *s_qvTvShowBuffAddrEven = NULL;
#ifdef _STILL_BUFFER_FROM_DSP_
static UINT8 G_raw_cmpr = 0;
static UINT8 G_capcnt = 1;
#endif


#if 0
int AppLibStillEnc_IsoConfigSet(AMBA_DSP_IMG_ALGO_MODE_e isoMode)
{
    extern STILL_ISO_CONFIG_s AppLibStillEnc_StillIsoConfig;
    extern STILL_HISO_NORMAL_CONFIG_s AppLibStillEnc_StillHisoNormalConfig;
    extern STILL_HISO_CONFIG_s AppLibStillEnc_StillHisoConfig;
    extern const UINT8 StillCc3DConfig[];
    extern const UINT8 StillCcRegConfig[];
    extern const UINT8 StillSecCcConfig[];
    extern UINT8 AppLibStillEnc_StillThreeDTable[];
    extern UINT8 AppLibStillEnc_StillHisoThreeDTable[];
    AMBA_DSP_IMG_MODE_CFG_s ImgMode;
    AMBA_DSP_IMG_CFG_INFO_s CfgInfo;
    AMBA_DSP_IMG_CTX_INFO_s DestCtx, SrcCtx;

    /* Initialize the context of ImageKernel of still */
    DestCtx.Pipe = AMBA_DSP_IMG_PIPE_STILL;
    DestCtx.CtxId = 0;
    SrcCtx.CtxId = 0;
    AmbaDSP_ImgInitCtx(0, 0, DestCtx, SrcCtx);
    AmbaPrint("%s: AmbaDSP_ImgInitCtx done", __FUNCTION__);

    /* Initialize the configure of ImageKernel of Still */
    CfgInfo.Pipe = AMBA_DSP_IMG_PIPE_STILL;
    CfgInfo.CfgId = 0;
    if (isoMode == AMBA_DSP_IMG_ALGO_MODE_LISO)
        AmbaDSP_ImgInitCfg(CfgInfo, AMBA_DSP_IMG_ALGO_MODE_LISO);
    else
        AmbaDSP_ImgInitCfg(CfgInfo, AMBA_DSP_IMG_ALGO_MODE_HISO);
    AmbaPrint("%s: AmbaDSP_ImgInitCfg done", __FUNCTION__);

    /* Setup mode of ImageKernel */
    /*
     * batch rule for normal filter:
     * LISO : batch_id = LISO
     * HISO : batch_id = LISO, some idsp_cmds need to issue again with batch_id = Hiso
    */
    memset(&ImgMode, 0, sizeof(AMBA_DSP_IMG_MODE_CFG_s));
    ImgMode.Pipe      = AMBA_DSP_IMG_PIPE_STILL;
    if (isoMode == AMBA_DSP_IMG_ALGO_MODE_LISO)
        ImgMode.AlgoMode  = AMBA_DSP_IMG_ALGO_MODE_LISO;
    else
        ImgMode.AlgoMode  = AMBA_DSP_IMG_ALGO_MODE_HISO;

    ImgMode.BatchId   = AMBA_DSP_VIDEO_FILTER;
    ImgMode.ContextId = 0;
    ImgMode.ConfigId  = 0;

    /* Setup related windows size */
    {
        AppLibStillEnc_StillIsoConfig.WarpCalcInfo.Data.VinSensorGeo.Width    = stillEncMgt[SensorIdx][StillModeIdx].CaptureWidth;
        AppLibStillEnc_StillIsoConfig.WarpCalcInfo.Data.VinSensorGeo.Height   = stillEncMgt[SensorIdx][StillModeIdx].CaptureHeight;
        AppLibStillEnc_StillIsoConfig.WarpCalcInfo.Data.MainWinDim.Width      = stillEncMgt[SensorIdx][StillModeIdx].MainWidth;
        AppLibStillEnc_StillIsoConfig.WarpCalcInfo.Data.MainWinDim.Height     = stillEncMgt[SensorIdx][StillModeIdx].MainHeight;
        AppLibStillEnc_StillIsoConfig.WarpCalcInfo.Data.R2rOutWinDim.Width    = stillEncMgt[SensorIdx][StillModeIdx].CaptureWidth;
        AppLibStillEnc_StillIsoConfig.WarpCalcInfo.Data.R2rOutWinDim.Height   = stillEncMgt[SensorIdx][StillModeIdx].CaptureHeight;
        AppLibStillEnc_StillIsoConfig.WarpCalcInfo.Data.DmyWinGeo.Width       = stillEncMgt[SensorIdx][StillModeIdx].CaptureWidth;
        AppLibStillEnc_StillIsoConfig.WarpCalcInfo.Data.DmyWinGeo.Height      = stillEncMgt[SensorIdx][StillModeIdx].CaptureHeight;
        AppLibStillEnc_StillIsoConfig.WarpCalcInfo.Data.CfaWinDim.Width       = stillEncMgt[SensorIdx][StillModeIdx].CaptureWidth;
        AppLibStillEnc_StillIsoConfig.WarpCalcInfo.Data.CfaWinDim.Height      = stillEncMgt[SensorIdx][StillModeIdx].CaptureWidth;
        AppLibStillEnc_StillIsoConfig.WarpCalcInfo.Data.ActWinCrop.RightBotX  = stillEncMgt[SensorIdx][StillModeIdx].CaptureWidth << 16;
        AppLibStillEnc_StillIsoConfig.WarpCalcInfo.Data.ActWinCrop.RightBotY  = stillEncMgt[SensorIdx][StillModeIdx].CaptureHeight << 16;
    }

    /* Prepare filters, same as Amba_Img_Set_Still_Pipe_Ctrl_Params() */
    if (AppLibStillEnc_StillIsoConfig.AeStatInfo.Enable)
        AmbaDSP_Img3aSetAeStatInfo(&ImgMode, &(AppLibStillEnc_StillIsoConfig.AeStatInfo.Data));

    if (AppLibStillEnc_StillIsoConfig.AfStatInfo.Enable)
        AmbaDSP_Img3aSetAfStatInfo(&ImgMode, &(AppLibStillEnc_StillIsoConfig.AfStatInfo.Data));

    if (AppLibStillEnc_StillIsoConfig.AwbStatInfo.Enable)
        AmbaDSP_Img3aSetAwbStatInfo(&ImgMode, &(AppLibStillEnc_StillIsoConfig.AwbStatInfo.Data));

    if (AppLibStillEnc_StillIsoConfig.SensorInfo.Enable)
        AmbaDSP_ImgSetVinSensorInfo(&ImgMode, &(AppLibStillEnc_StillIsoConfig.SensorInfo.Data));

    if (AppLibStillEnc_StillIsoConfig.SbpCorr.Enable)
        AmbaDSP_ImgSetStaticBadPixelCorrection(&ImgMode, &(AppLibStillEnc_StillIsoConfig.SbpCorr.Data));

    if (AppLibStillEnc_StillIsoConfig.VignetteCalcInfo.Enable) {
        AmbaDSP_ImgCalcVignetteCompensation(&ImgMode, &(AppLibStillEnc_StillIsoConfig.VignetteCalcInfo.Data));
        AmbaDSP_ImgSetVignetteCompensation(&ImgMode);
    }

    if (AppLibStillEnc_StillIsoConfig.CACalInfo.Enable) {
        AmbaDSP_ImgCalcCawarpCompensation(&ImgMode, &(AppLibStillEnc_StillIsoConfig.CACalInfo.Data));
        AmbaDSP_ImgSetCawarpCompensation(&ImgMode);
    }

    if (isoMode == AMBA_DSP_IMG_ALGO_MODE_LISO) {
        //LISO normal filter
        if (AppLibStillEnc_StillIsoConfig.StaticBlackLevelInfo.Enable)
            AmbaDSP_ImgSetStaticBlackLevel(&ImgMode, &(AppLibStillEnc_StillIsoConfig.StaticBlackLevelInfo.Data));

//        if (AppLibStillEnc_StillIsoConfig.VerticalFlipCorrectionInfo.Enable)
//          AmbaDSP_ImgSetDynamicBadPixelCorrection(&ImgMode, &(AppLibStillEnc_StillIsoConfig.VerticalFlipCorrectionInfo.Data));

        if (AppLibStillEnc_StillIsoConfig.CfaLeakageFilterInfo.Enable)
            AmbaDSP_ImgSetCfaLeakageFilter(&ImgMode, &(AppLibStillEnc_StillIsoConfig.CfaLeakageFilterInfo.Data));

        if (AppLibStillEnc_StillIsoConfig.CfaNoiseFilterInfo.Enable)
            AmbaDSP_ImgSetCfaNoiseFilter(&ImgMode, &(AppLibStillEnc_StillIsoConfig.CfaNoiseFilterInfo.Data));

        if (AppLibStillEnc_StillIsoConfig.AntiAliasingInfo.Enable)
            AmbaDSP_ImgSetAntiAliasing(&ImgMode, AppLibStillEnc_StillIsoConfig.AntiAliasingInfo.Data);

        if (AppLibStillEnc_StillIsoConfig.WbGainInfo.Enable)
            AppLibStillEnc_Img3ASetWbGain(ImgMode, AppLibStillEnc_StillIsoConfig.WbGainInfo.GlobalGain, &(AppLibStillEnc_StillIsoConfig.WbGainInfo.Data));

        if (AppLibStillEnc_StillIsoConfig.DgainSaturationLevelInfo.Enable)
            AmbaDSP_ImgSetDgainSaturationLevel(&ImgMode, &(AppLibStillEnc_StillIsoConfig.DgainSaturationLevelInfo.Data));

        if (AppLibStillEnc_StillIsoConfig.LocalExposureInfo.Enable)
            AmbaDSP_ImgSetLocalExposure(&ImgMode, &(AppLibStillEnc_StillIsoConfig.LocalExposureInfo.Data));

        if (AppLibStillEnc_StillIsoConfig.ColorCorrectionRegInfo.Enable) {
            AppLibStillEnc_StillIsoConfig.ColorCorrectionRegInfo.Data.RegSettingAddr = (UINT32)StillCcRegConfig;
            AmbaDSP_ImgSetColorCorrectionReg(&ImgMode, &(AppLibStillEnc_StillIsoConfig.ColorCorrectionRegInfo.Data));
        }

        if (AppLibStillEnc_StillIsoConfig.ColorCorrectionInfo.Enable) {
            AppLibStillEnc_StillIsoConfig.ColorCorrectionInfo.Data.MatrixThreeDTableAddr = (UINT32)StillCc3DConfig;
            //AppLibStillEnc_StillIsoConfig.ColorCorrectionInfo.Data.SecCcAddr = (UINT32)StillSecCcConfig;
            AmbaDSP_ImgSetColorCorrection(&ImgMode, &(AppLibStillEnc_StillIsoConfig.ColorCorrectionInfo.Data));
        }

        if (AppLibStillEnc_StillIsoConfig.ToneCurveInfo.Enable)
            AmbaDSP_ImgSetToneCurve(&ImgMode, &(AppLibStillEnc_StillIsoConfig.ToneCurveInfo.Data));

        if (AppLibStillEnc_StillIsoConfig.Rgb2YuvInfo.Enable)
            AmbaDSP_ImgSetRgbToYuvMatrix(&ImgMode, &(AppLibStillEnc_StillIsoConfig.Rgb2YuvInfo.Data));

        if (AppLibStillEnc_StillIsoConfig.ChromaScaleInfo.Enable)
            AmbaDSP_ImgSetChromaScale(&ImgMode, &(AppLibStillEnc_StillIsoConfig.ChromaScaleInfo.Data));

        if (AppLibStillEnc_StillIsoConfig.ChromaMedianFilterInfor.Enable)
            AmbaDSP_ImgSetChromaMedianFilter(&ImgMode, &(AppLibStillEnc_StillIsoConfig.ChromaMedianFilterInfor.Data));

        if (AppLibStillEnc_StillIsoConfig.DemosaicInfo.Enable)
            AmbaDSP_ImgSetDemosaic(&ImgMode, &(AppLibStillEnc_StillIsoConfig.DemosaicInfo.Data));

        if (AppLibStillEnc_StillIsoConfig.GbGrMismatch.Enable)
            AmbaDSP_ImgSetGbGrMismatch(&ImgMode, &(AppLibStillEnc_StillIsoConfig.GbGrMismatch.Data));

        if (AppLibStillEnc_StillIsoConfig.SharpenAOrSpatialFilterInfo.Enable)
            AmbaDSP_ImgSet1stLumaProcessingMode(&ImgMode, AppLibStillEnc_StillIsoConfig.SharpenAOrSpatialFilterInfo.Data);

        if (AppLibStillEnc_StillIsoConfig.CdnrInfo.Enable)
            AmbaDSP_ImgSetColorDependentNoiseReduction(&ImgMode, &(AppLibStillEnc_StillIsoConfig.CdnrInfo.Data));

        if (AppLibStillEnc_StillIsoConfig.AsfInfo.Enable) {
            AppLibStillEnc_StillIsoConfig.AsfInfo.Data.Adapt.ThreeD.pTable = AppLibStillEnc_StillThreeDTable;
            AmbaDSP_ImgSetAdvanceSpatialFilter(&ImgMode, &(AppLibStillEnc_StillIsoConfig.AsfInfo.Data));
        }

        if (AppLibStillEnc_StillIsoConfig.ChromaFilterInfo.Enable)
            AmbaDSP_ImgSetChromaFilter(&ImgMode, &(AppLibStillEnc_StillIsoConfig.ChromaFilterInfo.Data));

        //LISO only
        if (AppLibStillEnc_StillIsoConfig.SharpenBoth.Enable) {
            AppLibStillEnc_StillIsoConfig.SharpenBoth.Data.ThreeD.pTable = AppLibStillEnc_StillThreeDTable;
            AmbaDSP_ImgSetFinalSharpenNoiseBoth(&ImgMode, &(AppLibStillEnc_StillIsoConfig.SharpenBoth.Data));
        }

        if (AppLibStillEnc_StillIsoConfig.SharpenNoise.Enable)
            AmbaDSP_ImgSetFinalSharpenNoiseNoise(&ImgMode, &(AppLibStillEnc_StillIsoConfig.SharpenNoise.Data));

        if (AppLibStillEnc_StillIsoConfig.SharpenBFir.Enable)
            AmbaDSP_ImgSetFinalSharpenNoiseSharpenFir(&ImgMode, &(AppLibStillEnc_StillIsoConfig.SharpenBFir.Data));

        if (AppLibStillEnc_StillIsoConfig.SharpenBCoringInfo.Enable)
            AmbaDSP_ImgSetFinalSharpenNoiseSharpenCoring(&ImgMode, &(AppLibStillEnc_StillIsoConfig.SharpenBCoringInfo.Data));

        if (AppLibStillEnc_StillIsoConfig.SharpBLevelOverallInfo.Enable)
            AmbaDSP_ImgSetFinalSharpenNoiseSharpenCoringIndexScale(&ImgMode, &(AppLibStillEnc_StillIsoConfig.SharpBLevelOverallInfo.Data));

        if (AppLibStillEnc_StillIsoConfig.SharpBLevelMinInfo.Enable)
            AmbaDSP_ImgSetFinalSharpenNoiseSharpenMinCoringResult(&ImgMode, &(AppLibStillEnc_StillIsoConfig.SharpBLevelMinInfo.Data));

        if (AppLibStillEnc_StillIsoConfig.SharpenBScaleCoring.Enable)
            AmbaDSP_ImgSetFinalSharpenNoiseSharpenScaleCoring(&ImgMode, &(AppLibStillEnc_StillIsoConfig.SharpenBScaleCoring.Data));
    } else {
        AMBA_DSP_IMG_MODE_CFG_s modeHisoBatch;
        //HISO normal filter
        if (AppLibStillEnc_StillHisoNormalConfig.StaticBlackLevelInfo.Enable)
            AmbaDSP_ImgSetStaticBlackLevel(&ImgMode, &(AppLibStillEnc_StillHisoNormalConfig.StaticBlackLevelInfo.Data));

//        if (AppLibStillEnc_StillHisoNormalConfig.VerticalFlipCorrectionInfo.Enable)
//            AmbaDSP_ImgSetDynamicBadPixelCorrection(&ImgMode, &(AppLibStillEnc_StillHisoNormalConfig.VerticalFlipCorrectionInfo.Data));

        if (AppLibStillEnc_StillHisoNormalConfig.CfaLeakageFilterInfo.Enable)
            AmbaDSP_ImgSetCfaLeakageFilter(&ImgMode, &(AppLibStillEnc_StillHisoNormalConfig.CfaLeakageFilterInfo.Data));

        if (AppLibStillEnc_StillHisoNormalConfig.CfaNoiseFilterInfo.Enable)
            AmbaDSP_ImgSetCfaNoiseFilter(&ImgMode, &(AppLibStillEnc_StillHisoNormalConfig.CfaNoiseFilterInfo.Data));

        if (AppLibStillEnc_StillHisoNormalConfig.AntiAliasingInfo.Enable)
            AmbaDSP_ImgSetAntiAliasing(&ImgMode, AppLibStillEnc_StillHisoNormalConfig.AntiAliasingInfo.Data);

        if (AppLibStillEnc_StillHisoNormalConfig.WbGainInfo.Enable)
            AppLibStillEnc_Img3ASetWbGain(ImgMode, AppLibStillEnc_StillHisoNormalConfig.WbGainInfo.GlobalGain, &(AppLibStillEnc_StillHisoNormalConfig.WbGainInfo.Data));

        if (AppLibStillEnc_StillHisoNormalConfig.DgainSaturationLevelInfo.Enable)
            AmbaDSP_ImgSetDgainSaturationLevel(&ImgMode, &(AppLibStillEnc_StillHisoNormalConfig.DgainSaturationLevelInfo.Data));

        if (AppLibStillEnc_StillHisoNormalConfig.LocalExposureInfo.Enable)
            AmbaDSP_ImgSetLocalExposure(&ImgMode, &(AppLibStillEnc_StillHisoNormalConfig.LocalExposureInfo.Data));

        if (AppLibStillEnc_StillHisoNormalConfig.ColorCorrectionRegInfo.Enable) {
            AppLibStillEnc_StillHisoNormalConfig.ColorCorrectionRegInfo.Data.RegSettingAddr = (UINT32)StillCcRegConfig;
            AmbaDSP_ImgSetColorCorrectionReg(&ImgMode, &(AppLibStillEnc_StillHisoNormalConfig.ColorCorrectionRegInfo.Data));
        }

        if (AppLibStillEnc_StillHisoNormalConfig.ColorCorrectionInfo.Enable) {
            AppLibStillEnc_StillHisoNormalConfig.ColorCorrectionInfo.Data.MatrixThreeDTableAddr = (UINT32)StillCc3DConfig;
            //AppLibStillEnc_StillHisoNormalConfig.ColorCorrectionInfo.Data.SecCcAddr = (UINT32)StillSecCcConfig;
            AmbaDSP_ImgSetColorCorrection(&ImgMode, &(AppLibStillEnc_StillHisoNormalConfig.ColorCorrectionInfo.Data));
        }

        if (AppLibStillEnc_StillHisoNormalConfig.ToneCurveInfo.Enable)
            AmbaDSP_ImgSetToneCurve(&ImgMode, &(AppLibStillEnc_StillHisoNormalConfig.ToneCurveInfo.Data));

        if (AppLibStillEnc_StillHisoNormalConfig.Rgb2YuvInfo.Enable)
            AmbaDSP_ImgSetRgbToYuvMatrix(&ImgMode, &(AppLibStillEnc_StillHisoNormalConfig.Rgb2YuvInfo.Data));

        if (AppLibStillEnc_StillHisoNormalConfig.ChromaScaleInfo.Enable)
            AmbaDSP_ImgSetChromaScale(&ImgMode, &(AppLibStillEnc_StillHisoNormalConfig.ChromaScaleInfo.Data));

        if (AppLibStillEnc_StillHisoNormalConfig.ChromaMedianFilterInfor.Enable)
            AmbaDSP_ImgSetChromaMedianFilter(&ImgMode, &(AppLibStillEnc_StillHisoNormalConfig.ChromaMedianFilterInfor.Data));

        if (AppLibStillEnc_StillHisoNormalConfig.DemosaicInfo.Enable)
            AmbaDSP_ImgSetDemosaic(&ImgMode, &(AppLibStillEnc_StillHisoNormalConfig.DemosaicInfo.Data));

        if (AppLibStillEnc_StillHisoNormalConfig.GbGrMismatch.Enable)
            AmbaDSP_ImgSetGbGrMismatch(&ImgMode, &(AppLibStillEnc_StillHisoNormalConfig.GbGrMismatch.Data));

        if (AppLibStillEnc_StillHisoNormalConfig.SharpenAOrSpatialFilterInfo.Enable)
            AmbaDSP_ImgSet1stLumaProcessingMode(&ImgMode, AppLibStillEnc_StillHisoNormalConfig.SharpenAOrSpatialFilterInfo.Data);

        if (AppLibStillEnc_StillHisoNormalConfig.CdnrInfo.Enable)
            AmbaDSP_ImgSetColorDependentNoiseReduction(&ImgMode, &(AppLibStillEnc_StillHisoNormalConfig.CdnrInfo.Data));

        if (AppLibStillEnc_StillHisoNormalConfig.AsfInfo.Enable) {
            AppLibStillEnc_StillHisoNormalConfig.AsfInfo.Data.Adapt.ThreeD.pTable = AppLibStillEnc_StillThreeDTable;
            AmbaDSP_ImgSetAdvanceSpatialFilter(&ImgMode, &(AppLibStillEnc_StillHisoNormalConfig.AsfInfo.Data));
        }

        if (AppLibStillEnc_StillHisoNormalConfig.ChromaFilterInfo.Enable)
            AmbaDSP_ImgSetChromaFilter(&ImgMode, &(AppLibStillEnc_StillHisoNormalConfig.ChromaFilterInfo.Data));

        //HISO only
        if (AppLibStillEnc_StillHisoConfig.AntiAliasing.Enable)
            AmbaDSP_ImgSetHighIsoAntiAliasing(&ImgMode, AppLibStillEnc_StillHisoConfig.AntiAliasing.Strength);

        if (AppLibStillEnc_StillHisoConfig.CfaLeakageFilter.Enable)
            AmbaDSP_ImgSetHighIsoCfaLeakageFilter(&ImgMode, &(AppLibStillEnc_StillHisoConfig.CfaLeakageFilter.Data));

        if (AppLibStillEnc_StillHisoConfig.DynamicBadPixelCorrection.Enable)
            AmbaDSP_ImgSetHighIsoDynamicBadPixelCorrection(&ImgMode, &(AppLibStillEnc_StillHisoConfig.DynamicBadPixelCorrection.Data));

        if (AppLibStillEnc_StillHisoConfig.CfaNoiseFilter.Enable)
            AmbaDSP_ImgSetHighIsoCfaNoiseFilter(&ImgMode, &(AppLibStillEnc_StillHisoConfig.CfaNoiseFilter.Data));

        if (AppLibStillEnc_StillHisoConfig.GbGrMismatch.Enable)
            AmbaDSP_ImgSetHighIsoGbGrMismatch(&ImgMode, &(AppLibStillEnc_StillHisoConfig.GbGrMismatch.Data));

        if (AppLibStillEnc_StillHisoConfig.Demosaic.Enable)
            AmbaDSP_ImgSetHighIsoDemosaic(&ImgMode, &(AppLibStillEnc_StillHisoConfig.Demosaic.Data));

        if (AppLibStillEnc_StillHisoConfig.ChromaMedianFilter.Enable)
            AmbaDSP_ImgSetHighIsoChromaMedianFilter(&ImgMode, &(AppLibStillEnc_StillHisoConfig.ChromaMedianFilter.Data));

        if (AppLibStillEnc_StillHisoConfig.ColorDependentNoiseReduction.Enable)
            AmbaDSP_ImgSetHighIsoColorDependentNoiseReduction(&ImgMode, &(AppLibStillEnc_StillHisoConfig.ColorDependentNoiseReduction.Data));

        if (AppLibStillEnc_StillHisoConfig.DeferColorCorrection.Enable)
            AmbaDSP_ImgSetHighIsoDeferColorCorrection(&ImgMode, &(AppLibStillEnc_StillHisoConfig.DeferColorCorrection.Data));

        //Hiso Asf
        if (AppLibStillEnc_StillHisoConfig.AdvanceSpatialFilter.Enable) {
            AppLibStillEnc_StillHisoConfig.AdvanceSpatialFilter.Data.Adapt.ThreeD.pTable = AppLibStillEnc_StillHisoThreeDTable;
            AmbaDSP_ImgSetHighIsoAdvanceSpatialFilter(&ImgMode, &(AppLibStillEnc_StillHisoConfig.AdvanceSpatialFilter.Data));
        }

        if (AppLibStillEnc_StillHisoConfig.HighAdvanceSpatialFilter.Enable) {
            AppLibStillEnc_StillHisoConfig.HighAdvanceSpatialFilter.Data.Adapt.ThreeD.pTable = AppLibStillEnc_StillHisoThreeDTable;
            AmbaDSP_ImgSetHighIsoHighAdvanceSpatialFilter(&ImgMode, &(AppLibStillEnc_StillHisoConfig.HighAdvanceSpatialFilter.Data));
        }

        if (AppLibStillEnc_StillHisoConfig.LowAdvanceSpatialFilter.Enable) {
            AppLibStillEnc_StillHisoConfig.LowAdvanceSpatialFilter.Data.Adapt.ThreeD.pTable = AppLibStillEnc_StillHisoThreeDTable;
            AmbaDSP_ImgSetHighIsoLowAdvanceSpatialFilter(&ImgMode, &(AppLibStillEnc_StillHisoConfig.LowAdvanceSpatialFilter.Data));
        }

        if (AppLibStillEnc_StillHisoConfig.Med1AdvanceSpatialFilter.Enable) {
            AppLibStillEnc_StillHisoConfig.Med1AdvanceSpatialFilter.Data.Adapt.ThreeD.pTable = AppLibStillEnc_StillHisoThreeDTable;
            AmbaDSP_ImgSetHighIsoMed1AdvanceSpatialFilter(&ImgMode, &(AppLibStillEnc_StillHisoConfig.Med1AdvanceSpatialFilter.Data));
        }

        if (AppLibStillEnc_StillHisoConfig.Med2AdvanceSpatialFilter.Enable) {
            AppLibStillEnc_StillHisoConfig.Med2AdvanceSpatialFilter.Data.Adapt.ThreeD.pTable = AppLibStillEnc_StillHisoThreeDTable;
            AmbaDSP_ImgSetHighIsoMed2AdvanceSpatialFilter(&ImgMode, &(AppLibStillEnc_StillHisoConfig.Med2AdvanceSpatialFilter.Data));
        }

        if (AppLibStillEnc_StillHisoConfig.Li2ndAdvanceSpatialFilter.Enable) {
            AppLibStillEnc_StillHisoConfig.Li2ndAdvanceSpatialFilter.Data.Adapt.ThreeD.pTable = AppLibStillEnc_StillHisoThreeDTable;
            AmbaDSP_ImgSetHighIsoLi2ndAdvanceSpatialFilter(&ImgMode, &(AppLibStillEnc_StillHisoConfig.Li2ndAdvanceSpatialFilter.Data));
        }

        if (AppLibStillEnc_StillHisoConfig.ChromaAdvanceSpatialFilter.Enable) {
          // JH  AppLibStillEnc_StillHisoConfig.ChromaAdvanceSpatialFilter.Data.Adapt.ThreeD.pTable = AppLibStillEnc_StillHisoThreeDTable;
            AmbaDSP_ImgSetHighIsoChromaAdvanceSpatialFilter(&ImgMode, &(AppLibStillEnc_StillHisoConfig.ChromaAdvanceSpatialFilter.Data));
        }

        //Hiso Sharpen hi_high_
        if (AppLibStillEnc_StillHisoConfig.HighSharpenNoiseBoth.Enable) {
            AppLibStillEnc_StillHisoConfig.HighSharpenNoiseBoth.Data.ThreeD.pTable = AppLibStillEnc_StillHisoThreeDTable;
            AmbaDSP_ImgSetHighIsoHighSharpenNoiseBoth(&ImgMode, &(AppLibStillEnc_StillHisoConfig.HighSharpenNoiseBoth.Data));
        }

        if (AppLibStillEnc_StillHisoConfig.HighSharpenNoiseNoise.Enable)
            AmbaDSP_ImgSetHighIsoHighSharpenNoiseNoise(&ImgMode, &(AppLibStillEnc_StillHisoConfig.HighSharpenNoiseNoise.Data));

        if (AppLibStillEnc_StillHisoConfig.HighSharpenNoiseSharpenFir.Enable)
            AmbaDSP_ImgSetHighIsoHighSharpenNoiseSharpenFir(&ImgMode, &(AppLibStillEnc_StillHisoConfig.HighSharpenNoiseSharpenFir.Data));

        if (AppLibStillEnc_StillHisoConfig.HighSharpenNoiseSharpenCoring.Enable)
            AmbaDSP_ImgSetHighIsoHighSharpenNoiseSharpenCoring(&ImgMode, &(AppLibStillEnc_StillHisoConfig.HighSharpenNoiseSharpenCoring.Data));

        if (AppLibStillEnc_StillHisoConfig.HighSharpenNoiseSharpenCoringIndexScale.Enable)
            AmbaDSP_ImgSetHighIsoHighSharpenNoiseSharpenCoringIndexScale(&ImgMode, &(AppLibStillEnc_StillHisoConfig.HighSharpenNoiseSharpenCoringIndexScale.Data));

        if (AppLibStillEnc_StillHisoConfig.HighSharpenNoiseSharpenMinCoringResult.Enable)
            AmbaDSP_ImgSetHighIsoHighSharpenNoiseSharpenMinCoringResult(&ImgMode, &(AppLibStillEnc_StillHisoConfig.HighSharpenNoiseSharpenMinCoringResult.Data));

        if (AppLibStillEnc_StillHisoConfig.HighSharpenNoiseSharpenScaleCoring.Enable)
            AmbaDSP_ImgSetHighIsoHighSharpenNoiseSharpenScaleCoring(&ImgMode, &(AppLibStillEnc_StillHisoConfig.HighSharpenNoiseSharpenScaleCoring.Data));

        //Hiso Sharpen hi_med_
        if (AppLibStillEnc_StillHisoConfig.MedSharpenNoiseBoth.Enable) {
            AppLibStillEnc_StillHisoConfig.MedSharpenNoiseBoth.Data.ThreeD.pTable = AppLibStillEnc_StillHisoThreeDTable;
            AmbaDSP_ImgSetHighIsoMedSharpenNoiseBoth(&ImgMode, &(AppLibStillEnc_StillHisoConfig.MedSharpenNoiseBoth.Data));
        }

        if (AppLibStillEnc_StillHisoConfig.MedSharpenNoiseNoise.Enable)
            AmbaDSP_ImgSetHighIsoMedSharpenNoiseNoise(&ImgMode, &(AppLibStillEnc_StillHisoConfig.MedSharpenNoiseNoise.Data));

        if (AppLibStillEnc_StillHisoConfig.MedSharpenNoiseSharpenFir.Enable)
            AmbaDSP_ImgSetHighIsoMedSharpenNoiseSharpenFir(&ImgMode, &(AppLibStillEnc_StillHisoConfig.MedSharpenNoiseSharpenFir.Data));

        if (AppLibStillEnc_StillHisoConfig.MedSharpenNoiseSharpenCoring.Enable)
            AmbaDSP_ImgSetHighIsoMedSharpenNoiseSharpenCoring(&ImgMode, &(AppLibStillEnc_StillHisoConfig.MedSharpenNoiseSharpenCoring.Data));

        if (AppLibStillEnc_StillHisoConfig.MedSharpenNoiseSharpenCoringIndexScale.Enable)
            AmbaDSP_ImgSetHighIsoMedSharpenNoiseSharpenCoringIndexScale(&ImgMode, &(AppLibStillEnc_StillHisoConfig.MedSharpenNoiseSharpenCoringIndexScale.Data));

        if (AppLibStillEnc_StillHisoConfig.MedSharpenNoiseSharpenMinCoringResult.Enable)
            AmbaDSP_ImgSetHighIsoMedSharpenNoiseSharpenMinCoringResult(&ImgMode, &(AppLibStillEnc_StillHisoConfig.MedSharpenNoiseSharpenMinCoringResult.Data));

        if (AppLibStillEnc_StillHisoConfig.MedSharpenNoiseSharpenScaleCoring.Enable)
            AmbaDSP_ImgSetHighIsoMedSharpenNoiseSharpenScaleCoring(&ImgMode, &(AppLibStillEnc_StillHisoConfig.MedSharpenNoiseSharpenScaleCoring.Data));

        //Hiso Sharpen hili_
        if (AppLibStillEnc_StillHisoConfig.Liso1SharpenNoiseBoth.Enable) {
            AppLibStillEnc_StillHisoConfig.Liso1SharpenNoiseBoth.Data.ThreeD.pTable = AppLibStillEnc_StillHisoThreeDTable;
            AmbaDSP_ImgSetHighIsoLiso1SharpenNoiseBoth(&ImgMode, &(AppLibStillEnc_StillHisoConfig.Liso1SharpenNoiseBoth.Data));
        }

        if (AppLibStillEnc_StillHisoConfig.Liso1SharpenNoiseNoise.Enable)
            AmbaDSP_ImgSetHighIsoLiso1SharpenNoiseNoise(&ImgMode, &(AppLibStillEnc_StillHisoConfig.Liso1SharpenNoiseNoise.Data));

        if (AppLibStillEnc_StillHisoConfig.Liso1SharpenNoiseSharpenFir.Enable)
            AmbaDSP_ImgSetHighIsoLiso1SharpenNoiseSharpenFir(&ImgMode, &(AppLibStillEnc_StillHisoConfig.Liso1SharpenNoiseSharpenFir.Data));

        if (AppLibStillEnc_StillHisoConfig.Liso1SharpenNoiseSharpenCoring.Enable)
            AmbaDSP_ImgSetHighIsoLiso1SharpenNoiseSharpenCoring(&ImgMode, &(AppLibStillEnc_StillHisoConfig.Liso1SharpenNoiseSharpenCoring.Data));

        if (AppLibStillEnc_StillHisoConfig.Liso1SharpenNoiseSharpenCoringIndexScale.Enable)
            AmbaDSP_ImgSetHighIsoLiso1SharpenNoiseSharpenCoringIndexScale(&ImgMode, &(AppLibStillEnc_StillHisoConfig.Liso1SharpenNoiseSharpenCoringIndexScale.Data));

        if (AppLibStillEnc_StillHisoConfig.Liso1SharpenNoiseSharpenMinCoringResult.Enable)
            AmbaDSP_ImgSetHighIsoLiso1SharpenNoiseSharpenMinCoringResult(&ImgMode, &(AppLibStillEnc_StillHisoConfig.Liso1SharpenNoiseSharpenMinCoringResult.Data));

        if (AppLibStillEnc_StillHisoConfig.Liso1SharpenNoiseSharpenScaleCoring.Enable)
            AmbaDSP_ImgSetHighIsoLiso1SharpenNoiseSharpenScaleCoring(&ImgMode, &(AppLibStillEnc_StillHisoConfig.Liso1SharpenNoiseSharpenScaleCoring.Data));

        //Hiso Sharpen li_2nd_
        if (AppLibStillEnc_StillHisoConfig.Liso2SharpenNoiseBoth.Enable) {
            AppLibStillEnc_StillHisoConfig.Liso2SharpenNoiseBoth.Data.ThreeD.pTable = AppLibStillEnc_StillHisoThreeDTable;
            AmbaDSP_ImgSetHighIsoLiso2SharpenNoiseBoth(&ImgMode, &(AppLibStillEnc_StillHisoConfig.Liso2SharpenNoiseBoth.Data));
        }

        if (AppLibStillEnc_StillHisoConfig.Liso2SharpenNoiseNoise.Enable)
            AmbaDSP_ImgSetHighIsoLiso2SharpenNoiseNoise(&ImgMode, &(AppLibStillEnc_StillHisoConfig.Liso2SharpenNoiseNoise.Data));

        if (AppLibStillEnc_StillHisoConfig.Liso2SharpenNoiseSharpenFir.Enable)
            AmbaDSP_ImgSetHighIsoLiso2SharpenNoiseSharpenFir(&ImgMode, &(AppLibStillEnc_StillHisoConfig.Liso2SharpenNoiseSharpenFir.Data));

        if (AppLibStillEnc_StillHisoConfig.Liso2SharpenNoiseSharpenCoring.Enable)
            AmbaDSP_ImgSetHighIsoLiso2SharpenNoiseSharpenCoring(&ImgMode, &(AppLibStillEnc_StillHisoConfig.Liso2SharpenNoiseSharpenCoring.Data));

        if (AppLibStillEnc_StillHisoConfig.Liso2SharpenNoiseSharpenCoringIndexScale.Enable)
            AmbaDSP_ImgSetHighIsoLiso2SharpenNoiseSharpenCoringIndexScale(&ImgMode, &(AppLibStillEnc_StillHisoConfig.Liso2SharpenNoiseSharpenCoringIndexScale.Data));

        if (AppLibStillEnc_StillHisoConfig.Liso2SharpenNoiseSharpenMinCoringResult.Enable)
            AmbaDSP_ImgSetHighIsoLiso2SharpenNoiseSharpenMinCoringResult(&ImgMode, &(AppLibStillEnc_StillHisoConfig.Liso2SharpenNoiseSharpenMinCoringResult.Data));

        if (AppLibStillEnc_StillHisoConfig.Liso2SharpenNoiseSharpenScaleCoring.Enable)
            AmbaDSP_ImgSetHighIsoLiso2SharpenNoiseSharpenScaleCoring(&ImgMode, &(AppLibStillEnc_StillHisoConfig.Liso2SharpenNoiseSharpenScaleCoring.Data));

        //Chroma
        if (AppLibStillEnc_StillHisoConfig.ChromaFilterHigh.Enable)
            AmbaDSP_ImgHighIsoSetChromaFilterHigh(&ImgMode, &(AppLibStillEnc_StillHisoConfig.ChromaFilterHigh.Data));

        if (AppLibStillEnc_StillHisoConfig.ChromaFilterLowVeryLow.Enable)
            AmbaDSP_ImgHighIsoSetChromaFilterLowVeryLow(&ImgMode, &(AppLibStillEnc_StillHisoConfig.ChromaFilterLowVeryLow.Data));

        if (AppLibStillEnc_StillHisoConfig.ChromaFilterPre.Enable)
            AmbaDSP_ImgHighIsoSetChromaFilterPre(&ImgMode, &(AppLibStillEnc_StillHisoConfig.ChromaFilterPre.Data));

        if (AppLibStillEnc_StillHisoConfig.ChromaFilterMed.Enable)
            AmbaDSP_ImgHighIsoSetChromaFilterMed(&ImgMode, &(AppLibStillEnc_StillHisoConfig.ChromaFilterMed.Data));

        if (AppLibStillEnc_StillHisoConfig.ChromaFilterLow.Enable)
            AmbaDSP_ImgHighIsoSetChromaFilterLow(&ImgMode, &(AppLibStillEnc_StillHisoConfig.ChromaFilterLow.Data));

        if (AppLibStillEnc_StillHisoConfig.ChromaFilterVeryLow.Enable)
            AmbaDSP_ImgHighIsoSetChromaFilterVeryLow(&ImgMode, &(AppLibStillEnc_StillHisoConfig.ChromaFilterVeryLow.Data));

        //Combine
        if (AppLibStillEnc_StillHisoConfig.ChromaFilterMedCombine.Enable) {
            AppLibStillEnc_StillHisoConfig.ChromaFilterMedCombine.Data.ThreeD.pTable = AppLibStillEnc_StillHisoThreeDTable;
            AmbaDSP_ImgHighIsoSetChromaFilterMedCombine(&ImgMode, &(AppLibStillEnc_StillHisoConfig.ChromaFilterMedCombine.Data));
        }

        if (AppLibStillEnc_StillHisoConfig.ChromaFilterLowCombine.Enable) {
            AppLibStillEnc_StillHisoConfig.ChromaFilterLowCombine.Data.ThreeD.pTable = AppLibStillEnc_StillHisoThreeDTable;
            AmbaDSP_ImgHighIsoSetChromaFilterLowCombine(&ImgMode, &(AppLibStillEnc_StillHisoConfig.ChromaFilterLowCombine.Data));
        }

        if (AppLibStillEnc_StillHisoConfig.ChromaFilterVeryLowCombine.Enable) {
            AppLibStillEnc_StillHisoConfig.ChromaFilterVeryLowCombine.Data.ThreeD.pTable = AppLibStillEnc_StillHisoThreeDTable;
            AmbaDSP_ImgHighIsoSetChromaFilterVeryLowCombine(&ImgMode, &(AppLibStillEnc_StillHisoConfig.ChromaFilterVeryLowCombine.Data));
        }

        if (AppLibStillEnc_StillHisoConfig.LumaNoiseCombine.Enable) {
            AppLibStillEnc_StillHisoConfig.LumaNoiseCombine.Data.ThreeD.pTable = AppLibStillEnc_StillHisoThreeDTable;
            AmbaDSP_ImgHighIsoSetLumaNoiseCombine(&ImgMode, &(AppLibStillEnc_StillHisoConfig.LumaNoiseCombine.Data));
        }

        if (AppLibStillEnc_StillHisoConfig.LowASFCombine.Enable) {
            AppLibStillEnc_StillHisoConfig.LowASFCombine.Data.ThreeD.pTable = AppLibStillEnc_StillHisoThreeDTable;
            AmbaDSP_ImgHighIsoSetLowASFCombine(&ImgMode, &(AppLibStillEnc_StillHisoConfig.LowASFCombine.Data));
        }

        if (AppLibStillEnc_StillHisoConfig.HighIsoCombine.Enable) {
            AppLibStillEnc_StillHisoConfig.HighIsoCombine.Data.ThreeD.pTable = AppLibStillEnc_StillHisoThreeDTable;
            AmbaDSP_ImgHighIsoSetHighIsoCombine(&ImgMode, &(AppLibStillEnc_StillHisoConfig.HighIsoCombine.Data));
        }

        if (AppLibStillEnc_StillHisoConfig.HighIsoFreqRecover.Enable)
            AmbaDSP_ImgHighIsoSetHighIsoFreqRecover(&ImgMode, &(AppLibStillEnc_StillHisoConfig.HighIsoFreqRecover.Data));

        if (AppLibStillEnc_StillHisoConfig.HighIsoLumaBlend.Enable) {
            AmbaDSP_ImgHighIsoSetHighIsoLumaBlend(&ImgMode, &(AppLibStillEnc_StillHisoConfig.HighIsoLumaBlend.Data));

            if (AppLibStillEnc_StillHisoConfig.HighIsoLumaBlend.Data.Enable) {
                if (AppLibStillEnc_StillHisoConfig.HighIsoBlendLumaLevel.Enable)
                    AmbaDSP_ImgHighIsoSetHighIsoBlendLumaLevel(&ImgMode, &(AppLibStillEnc_StillHisoConfig.HighIsoBlendLumaLevel.Data));
            }
        }

        //to hiso batch
        modeHisoBatch = ImgMode;
        modeHisoBatch.BatchId = AMBA_DSP_VIDEO_FILTER;

        AmbaDSP_ImgSetStaticBadPixelCorrection(&modeHisoBatch, &(AppLibStillEnc_StillIsoConfig.SbpCorr.Data));
        AmbaDSP_ImgSetVignetteCompensation(&modeHisoBatch);
        AmbaDSP_ImgSetWarpCompensation(&modeHisoBatch);
        AmbaDSP_ImgSetCawarpCompensation(&modeHisoBatch);
    }

    return OK;
}

#endif

int AppLibStillEnc_SetStillWB(UINT32 chipNo, AMBA_DSP_IMG_MODE_CFG_s *imgMode)
{
    AMBA_DSP_IMG_WB_GAIN_s StillWbGain;
    AMBA_AE_INFO_s StillAeInfo[MAX_AEB_NUM];

    AmbaImg_Proc_Cmd(MW_IP_GET_AE_INFO, chipNo, IP_MODE_STILL, (UINT32)StillAeInfo);
    AmbaImg_Proc_Cmd(MW_IP_GET_PIPE_WB_GAIN, chipNo, IP_MODE_STILL, (UINT32)&StillWbGain);
    StillWbGain.AeGain = StillAeInfo[0].Dgain;

    DBGMSGc2(GREEN,"StillWbGain: GainR(%u) GainG(%u) GainB(%u) AEGain(%u) GlobalD(%u)",\
            StillWbGain.GainR, StillWbGain.GainG, StillWbGain.GainB, StillWbGain.AeGain, StillWbGain.GlobalDGain);
    return AmbaDSP_ImgSetWbGain(imgMode, &StillWbGain);
}

int AppLibStillEnc_SetStillSensorInfo(AMBA_DSP_IMG_MODE_CFG_s *imgMode)
{
    AMBA_DSP_IMG_SENSOR_INFO_s SensorInfo;
    AMBA_SENSOR_STATUS_INFO_s SensorStatus = {0};
    AmbaSensor_GetStatus(AppEncChannel, &SensorStatus);
    memset(&SensorInfo, 0x0, sizeof(SensorInfo));
    SensorInfo.SensorPattern = (UINT8)SensorStatus.ModeInfo.OutputInfo.CfaPattern;
    return AmbaDSP_ImgSetVinSensorInfo(imgMode, &SensorInfo);
}

/* CB for raw capture */
UINT32 AppLibStillEnc_RawCapCB(AMP_STILLENC_RAWCAP_FLOW_CTRL_s *ctrl)
{
    int rval = 0;
    //UINT32 OBMode = 0;
    AMBA_DSP_WINDOW_s vinCapture;

    AMBA_SENSOR_STATUS_INFO_s sensorStatus;
    AMBA_SENSOR_AREA_INFO_s *recPixel = &sensorStatus.ModeInfo.OutputInfo.RecordingPixels;
    AMBA_SENSOR_INPUT_INFO_s *inputInfo = &sensorStatus.ModeInfo.InputInfo;
    AMP_STILLENC_RAWCAP_DSP_CTRL_s dspCtrl;
    AMBA_AE_INFO_s stillAeInfo[MAX_AEB_NUM];
    AMBA_DSP_IMG_WB_GAIN_s wbGain;
    APPLIB_SENSOR_STILLCAP_CONFIG_s *StillCapConfigData;
    AMBA_SENSOR_MODE_INFO_s SensorModeInfo;
    AMBA_SENSOR_MODE_ID_u Mode = {0};
    UINT32 gainFactor = 0;
    UINT32 aGainCtrl = 0;
    UINT32 dGainCtrl = 0;
    UINT32 shutterCtrl = 0;
    UINT32 imgIpChNo = 0;
    UINT8 exposureFrames = 0;

    APPLIB_VOUT_PREVIEW_PARAM_s PreviewParam={0};
    UINT32 DchanWidth = 0;
    UINT32 DchanHeight = 0;
    UINT32 FchanWidth = 0;
    UINT32 FchanHeight = 0;
    APPLIB_SENSOR_STILLCAP_MODE_CONFIG_s *pMode = NULL;

    AmbaPrint("[Applib - StillEnc] <StillEncUtility> RawCapCB start");
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


    /* Stop LiveView capture */
    rval = AmpStillEnc_EnableLiveviewCapture(AMP_STILL_PREPARE_TO_STILL_CAPTURE, 0);
    if (rval == NG) {
        AmbaPrintColor(RED,"[Applib - StillEnc] <StillEncUtility> Cannot Stop Liveview");
        return NG;
    } else {
        AmbaPrint("[Applib - StillEnc] <StillEncUtility> Liveview Stop");
    }

    rval = AmbaImg_Proc_Cmd(MW_IP_GET_AE_INFO, imgIpChNo, IP_MODE_STILL, (UINT32)stillAeInfo);

    if (rval < 0) {
        AmbaPrintColor(RED, "[Applib - StillEnc] <StillEncUtility> RawCapCB MW_IP_GET_AE_INFO failure");
        return -1;
    }

#if 1
    AmbaPrintColor(GREEN, "[Applib - StillEnc] <StillEncUtility> RawCapCB: AE info[%d]: EV:%d NF:%d ShIdx:%d AgcIdx:%d IrisIdx:%d Dgain:%d ISO:%d Flash:%d Mode:%d ShTime:%f AgcGain:%f", \
        ctrl->AeIdx, \
        stillAeInfo[ctrl->AeIdx].EvIndex, stillAeInfo[ctrl->AeIdx].NfIndex, \
        stillAeInfo[ctrl->AeIdx].ShutterIndex, stillAeInfo[ctrl->AeIdx].AgcIndex, \
        stillAeInfo[ctrl->AeIdx].IrisIndex, stillAeInfo[ctrl->AeIdx].Dgain, \
        stillAeInfo[ctrl->AeIdx].IsoValue, stillAeInfo[ctrl->AeIdx].Flash, \
        stillAeInfo[ctrl->AeIdx].Mode, \
        stillAeInfo[ctrl->AeIdx].ShutterTime, stillAeInfo[ctrl->AeIdx].AgcGain);
#endif

    rval = AmbaImg_Proc_Cmd(MW_IP_GET_WB_GAIN, imgIpChNo, (UINT32)&wbGain, 0);
    if (rval < 0) {
        AmbaPrintColor(RED, "[Applib - StillEnc] <StillEncUtility> RawCapCB MW_IP_GET_AE_INFO failure");
        return -1;
    }
    /* check OB mode, TBD */
    //ctrl->ob(&OBMode);

    dspCtrl.VinChan = ctrl->VinChan;

    AmbaPrint("[Applib - StillEnc] <StillEncUtility> [vinChan] %d", dspCtrl.VinChan);

    /* Get vidSkip from sensor */
    dspCtrl.VidSkip = 0; //TBD

    /* determine rawCapNum */
    if (1/* fast capture mode */) {
        //APP should make sure we have enough memory in this burst capture
        dspCtrl.RawCapNum = ctrl->TotalRawToCap;
    } else {
        //Slow capture mode
        //TBS
    }

    /* Program sensor */
    AmbaSensor_GetModeInfo(ctrl->VinChan, Mode, &SensorModeInfo);
    AmbaSensor_Config(ctrl->VinChan, ctrl->SensorMode, ctrl->ShType);
    AmbaSensor_GetStatus(ctrl->VinChan, &sensorStatus);
    AmbaSensor_ConvertGainFactor(ctrl->VinChan, stillAeInfo[ctrl->AeIdx].AgcGain, &gainFactor, &aGainCtrl, &dGainCtrl);
    AmbaSensor_SetAnalogGainCtrl(ctrl->VinChan, aGainCtrl);
    AmbaSensor_SetDigitalGainCtrl(ctrl->VinChan, dGainCtrl);
    AmbaSensor_ConvertShutterSpeed(ctrl->VinChan, stillAeInfo[ctrl->AeIdx].ShutterTime, &shutterCtrl);
    exposureFrames = (shutterCtrl/SensorModeInfo.FrameLengthLines);
    exposureFrames = (shutterCtrl%SensorModeInfo.FrameLengthLines)? exposureFrames+1: exposureFrames;
    AmbaSensor_SetSlowShutterCtrl(ctrl->VinChan, exposureFrames);
    AmbaSensor_SetShutterCtrl(ctrl->VinChan, shutterCtrl);
    AmbaPrint("[Applib - StillEnc] <StillEncUtility> RawCapCB: (sensor) %d %d", ctrl->SensorMode.Bits.Mode, ctrl->ShType);

    /* Program Vin */
    vinCapture.Width = ctrl->VcapWidth;//scrptRawCap->fvRawCapArea.effectArea.width;
    vinCapture.Height = ctrl->VcapHeight;//scrptRawCap->fvRawCapArea.effectArea.height;
    vinCapture.OffsetX = (recPixel->StartX + ((recPixel->Width - vinCapture.Width)/2)) & 0xFFFE;
    vinCapture.OffsetY = recPixel->StartY + (((recPixel->Height - vinCapture.Height)/2) & 0xFFFE);
    AmbaPrint("[Applib - StillEnc] <StillEncUtility> RawCapCB: (vin) %d %d %d %d", \
    vinCapture.Width, vinCapture.Height, \
    vinCapture.OffsetX, vinCapture.OffsetY);
    AmbaDSP_VinCaptureConfig(0, &vinCapture);

    /* set still idsp param */
    {
        extern void  Amba_Img_Set_Still_Pipe_Ctrl_Params(UINT32 chNo, AMBA_DSP_IMG_MODE_CFG_s *mode);

        /* Run Adj compute */
        if (G_iso == 1) {
            ADJ_STILL_CONTROL_s adjStillCtrl;
            AMBA_DSP_IMG_MODE_CFG_s imgMode;
            AMBA_DSP_IMG_CTX_INFO_s DestCtx, SrcCtx;
            AMBA_DSP_IMG_CFG_INFO_s CfgInfo;
            /* Prepare IK ctx */
            /* Initialize the context of ImageKernel of still */
            DestCtx.Pipe = AMBA_DSP_IMG_PIPE_STILL;
            DestCtx.CtxId = 0;
            SrcCtx.CtxId = 0;
            AmbaDSP_ImgInitCtx(0, 0, &DestCtx, &SrcCtx);

            /* Initialize the configure of ImageKernel of Still */
            CfgInfo.Pipe = AMBA_DSP_IMG_PIPE_STILL;
            CfgInfo.CfgId = 0;
            if (G_iso == 1) AmbaDSP_ImgInitCfg(&CfgInfo, AMBA_DSP_IMG_ALGO_MODE_LISO);
            else AmbaDSP_ImgInitCfg(&CfgInfo, AMBA_DSP_IMG_ALGO_MODE_HISO);

            memset(&adjStillCtrl, 0x0, sizeof(ADJ_STILL_CONTROL_s));
            adjStillCtrl.StillMode = IP_MODE_LISO_STILL; //temp set as 0, since HISO adj is not ready
            adjStillCtrl.ShIndex = stillAeInfo[ctrl->AeIdx].ShutterIndex;
            adjStillCtrl.EvIndex = stillAeInfo[ctrl->AeIdx].EvIndex;
            adjStillCtrl.NfIndex = stillAeInfo[ctrl->AeIdx].NfIndex;
            adjStillCtrl.WbGain = wbGain;
            adjStillCtrl.DZoomStep = 0;
            adjStillCtrl.FlashMode = 0;
            adjStillCtrl.LutNo = 0;
            AmbaPrint("[ADJ] id %d, shidx %d, evidx %d, nfidx %d, wb(%u %u %u %u %u)", \
                ctrl->VinChan.Bits.SensorID, adjStillCtrl.ShIndex, \
                adjStillCtrl.EvIndex, adjStillCtrl.NfIndex, \
                adjStillCtrl.WbGain.GainR, adjStillCtrl.WbGain.GainG, \
                adjStillCtrl.WbGain.GainB, adjStillCtrl.WbGain.AeGain, \
                adjStillCtrl.WbGain.GlobalDGain);
            AmbaImg_Proc_Cmd(MW_IP_ADJ_STILL_CONTROL, imgIpChNo , (UINT32)&adjStillCtrl , 0);
            memset(&imgMode, 0, sizeof(AMBA_DSP_IMG_MODE_CFG_s));
            imgMode.Pipe = AMBA_DSP_IMG_PIPE_STILL;
            imgMode.AlgoMode = (G_iso == 1)? AMBA_DSP_IMG_ALGO_MODE_LISO: AMBA_DSP_IMG_ALGO_MODE_HISO;
            imgMode.BatchId   = AMBA_DSP_VIDEO_FILTER;
            imgMode.ContextId = 0;
            imgMode.ConfigId  = 0;
            Amba_Img_Set_Still_Pipe_Ctrl_Params(imgIpChNo, &imgMode);
            AppLibStillEnc_SetStillWB(imgIpChNo, &imgMode);
			AppLibStillEnc_SetStillSensorInfo(&imgMode);
        } else  if (G_iso == 0) {
            //AppLibStillEnc_IsoConfigSet(AMBA_DSP_IMG_ALGO_MODE_HISO);
        }

        dspCtrl.StillProc = G_iso;
    }

    /* calc warp and execute ISO config */
    {
        AMBA_DSP_IMG_MODE_CFG_s ImgMode;
        AMBA_DSP_IMG_SIZE_INFO_s SizeInfo;
        AMBA_DSP_IMG_WARP_CALC_INFO_s CalcWarp = {0};
        double ZoomRatio;
        UINT16 CapW, CapH, EncW, EncH, ScrnW, ScrnH, ThmW, ThmH;
        AMBA_DSP_IMG_VIN_SENSOR_GEOMETRY_s VinGeo;
        AMBA_DSP_IMG_OUT_WIN_INFO_s                 ImgOutputWin;
        AMBA_DSP_IMG_SENSOR_FRAME_TIMING_s          ImgSensorFrameTiming;
        AMBA_DSP_IMG_DZOOM_INFO_s                   ImgDzoom;
        UINT16 offsetX = 0;

        memset(&ImgMode, 0, sizeof(AMBA_DSP_IMG_MODE_CFG_s));
        ImgMode.Pipe = AMBA_DSP_IMG_PIPE_STILL;
        if (G_iso == 0) {
            ImgMode.AlgoMode  = AMBA_DSP_IMG_ALGO_MODE_HISO;
        } else if (G_iso == 1) {
            ImgMode.AlgoMode  = AMBA_DSP_IMG_ALGO_MODE_LISO;
        } else {
            ImgMode.AlgoMode  = AMBA_DSP_IMG_ALGO_MODE_FAST;
        }
        ImgMode.BatchId   = AMBA_DSP_VIDEO_FILTER;
        ImgMode.ContextId = 0;
        ImgMode.ConfigId  = 0;


        /* Dzoom */
        CapW = pMode->CaptureWidth;
        CapH = pMode->CaptureHeight;
        EncW = StillCapConfigData->FullviewWidth;
        EncH = StillCapConfigData->FullviewHeight;
        ScrnW = StillCapConfigData->ScreennailWidth;
        ScrnH = StillCapConfigData->ScreennailHeight;
        ThmW = StillCapConfigData->ThumbnailWidth;
        ThmH = StillCapConfigData->ThumbnailHeight;

        memset(&VinGeo, 0, sizeof(VinGeo));
        offsetX = ((recPixel->StartX + (((recPixel->Width - CapW)/2)&0xFFF8)) - recPixel->StartX)* \
                    inputInfo->HSubsample.FactorDen/inputInfo->HSubsample.FactorNum;

        VinGeo.StartX = inputInfo->PhotodiodeArray.StartX + offsetX;
        VinGeo.StartY = inputInfo->PhotodiodeArray.StartY + \
            (((recPixel->StartY + ((recPixel->Height - CapH)/2)) & 0xFFFE) - recPixel->StartY)* \
                    inputInfo->VSubsample.FactorDen/inputInfo->VSubsample.FactorNum;
        VinGeo.Width  = CapW;
        VinGeo.Height = CapH;
        VinGeo.HSubSample.FactorNum = inputInfo->HSubsample.FactorNum;
        VinGeo.HSubSample.FactorDen = inputInfo->HSubsample.FactorDen;
        VinGeo.VSubSample.FactorNum = inputInfo->VSubsample.FactorNum;
        VinGeo.VSubSample.FactorDen = inputInfo->VSubsample.FactorDen;
        AmbaDSP_WarpCore_SetVinSensorGeo(&ImgMode, &VinGeo);

        memset(&ImgSensorFrameTiming, 0, sizeof(ImgSensorFrameTiming));
        ImgSensorFrameTiming.TimeScale        = sensorStatus.ModeInfo.FrameTime.FrameRate.TimeScale;
        ImgSensorFrameTiming.NumUnitsInTick   = sensorStatus.ModeInfo.FrameTime.FrameRate.NumUnitsInTick;
        ImgSensorFrameTiming.FrameLengthLines = sensorStatus.ModeInfo.FrameLengthLines;
        AmbaDSP_WarpCore_SetSensorFrameTiming(&ImgMode, &ImgSensorFrameTiming);

        memset(&ImgOutputWin, 0, sizeof(ImgOutputWin));
        ImgOutputWin.MainWinDim.Width  = EncW;
        ImgOutputWin.MainWinDim.Height = EncH;
        ImgOutputWin.ScreennailDim.Width  = ScrnW;
        ImgOutputWin.ScreennailDim.Height = ScrnH;
        ImgOutputWin.ThumbnailDim.Width  = ThmW;
        ImgOutputWin.ThumbnailDim.Height = ThmH;
        ImgOutputWin.PrevWinDim[0].Width  = DchanWidth;
        ImgOutputWin.PrevWinDim[0].Height = DchanHeight;
        ImgOutputWin.PrevWinDim[1].Width  = FchanWidth;
        ImgOutputWin.PrevWinDim[1].Height = FchanHeight;
        AmbaDSP_WarpCore_SetOutputWin(&ImgMode, &ImgOutputWin);


        /* Dzoom don't care, TBD */
        ZoomRatio = (double) 100 / 100;

        memset(&ImgDzoom, 0, sizeof(ImgDzoom));
        ImgDzoom.ZoomX = (UINT32)(ZoomRatio * 65536);
        ImgDzoom.ZoomY = (UINT32)(ZoomRatio * 65536);
        AmbaDSP_WarpCore_SetDzoomFactor(&ImgMode, &ImgDzoom);

        AmbaDSP_WarpCore_CalcDspWarp(&ImgMode, 0);
        AmbaDSP_WarpCore_CalcDspCawarp(&ImgMode, 0);

        AmbaDSP_WarpCore_SetDspWarp(&ImgMode);
        AmbaDSP_WarpCore_SetDspCawarp(&ImgMode);
        //update bad pixel and vignette map.
        AppLibCalib_SetDspMode(&ImgMode);
        AppLibCalibAdjust_StillVignetteStrength(imgIpChNo);
        AppLibCalibVignette_MapUpdate();
        AppLibCalibBPC_MapUpdate(0);

        memset(&CalcWarp, 0, sizeof(CalcWarp));
        if (AmbaDSP_ImgGetWarpCompensation(&ImgMode, &CalcWarp) != OK)
            AmbaPrintColor(RED, "[Applib - StillEnc] <StillEncUtility> RawCapCB: Get Warp Compensation fail!!");

        memset(&SizeInfo, 0, sizeof(SizeInfo));
        SizeInfo.WidthIn     = ((CalcWarp.ActWinCrop.RightBotX - CalcWarp.ActWinCrop.LeftTopX + 0xFFFF)>>16);
        SizeInfo.HeightIn    = ((CalcWarp.ActWinCrop.RightBotY - CalcWarp.ActWinCrop.LeftTopY + 0xFFFF)>>16);
        SizeInfo.WidthMain   = StillCapConfigData->FullviewWidth;
        SizeInfo.HeightMain  = StillCapConfigData->FullviewHeight;
        SizeInfo.WidthPrevA = DchanWidth;
        SizeInfo.HeightPrevA = DchanHeight;
        SizeInfo.WidthPrevB = FchanWidth;
        SizeInfo.HeightPrevB = FchanHeight;
        SizeInfo.WidthScrn = ScrnW;
        SizeInfo.HeightScrn = ScrnH;

        AmbaDSP_ImgSetSizeInfo(&ImgMode, &SizeInfo);
        AmbaDSP_ImgPostExeCfg(&ImgMode, (AMBA_DSP_IMG_CONFIG_EXECUTE_MODE_e)0);
    }

    AmpStillEnc_StartRawCapture(StillEncPri, &dspCtrl);

    return 0;
}

/** CB for multi raw capture */
UINT32 AppLibStillEnc_MultiRawCapCB(AMP_STILLENC_RAWCAP_FLOW_CTRL_s *ctrl)
{
    AMP_STILLENC_RAWCAP_DSP_CTRL_s DspCtrl;

    DspCtrl.RawCapNum = ctrl->TotalRawToCap;
    DspCtrl.VinChan = ctrl->VinChan;
    DspCtrl.StillProc = G_iso;
    DspCtrl.VidSkip = 1;

    /* check OB mode, TBD */
    //ctrl->ob(&OBMode);

    // do we need to update and IDSP filter ??

    AmpStillEnc_StartFollowingRawCapture(StillEncPri, &DspCtrl);
    return OK;
}

#ifdef _STILL_BUFFER_FROM_DSP_
static UINT32 AppLibStillEnc_DspWorkCalculate(UINT8 **addr, UINT32 *size)
{
    int ReturnValue = 0;
    UINT32 totalSize = 0;
    APPLIB_SENSOR_STILLCAP_CONFIG_s *StillCapConfigData;
    AMBA_SENSOR_MODE_ID_u Mode = {0};

    APPLIB_VOUT_PREVIEW_PARAM_s PreviewParam={0};
    UINT32 QvDchanWidth = 0;
    UINT32 QvDchanHeight = 0;
    UINT32 QvFchanWidth = 0;
    UINT32 QvFchanHeight = 0;
    APPLIB_SENSOR_STILLCAP_MODE_CONFIG_s *pMode = NULL;

    StillCapConfigData = AppLibSysSensor_GetPjpegConfig(AppLibStillEnc_GetPhotoPjpegCapMode(), AppLibStillEnc_GetPhotoPjpegConfigId());
    pMode = AppLibSysSensor_GetStillCaptureModeConfig(AppLibStillEnc_GetPhotoPjpegCapMode(), AppLibStillEnc_GetPhotoPjpegConfigId());
    Mode.Data = pMode->ModeId;

    PreviewParam.AspectRatio = AppLibSysSensor_GetCaptureModeAR(AppLibStillEnc_GetPhotoPjpegCapMode(), AppLibStillEnc_GetPhotoPjpegConfigId());
    PreviewParam.ChanID = DISP_CH_FCHAN;
    AppLibDisp_CalcPreviewWindowSize(&PreviewParam);
    QvFchanWidth = PreviewParam.Preview.Width;
    QvFchanHeight = PreviewParam.Preview.Height;

    PreviewParam.ChanID = DISP_CH_DCHAN;
    AppLibDisp_CalcPreviewWindowSize(&PreviewParam);
    QvDchanWidth = PreviewParam.Preview.Width;
    QvDchanHeight = PreviewParam.Preview.Height;



    if (0) {//CaptureMode == STILL_CAPTURE_NONE) {
        //LiveView only
        (*addr) = ApplibDspWorkAreaResvStart;
        (*size) = ApplibDspWorkAreaResvSize;
    } else if (0) {//CaptureMode == STILL_CAPTURE_YUV2JPG) {
        //allocate YUV buffer
        UINT16 YuvWidth = 0, YuvHeight = 0, ScrnW = 0, ScrnH = 0, ThmW = 0, ThmH = 0;
        UINT32 YuvSize = 0, ScrnSize = 0, ThmSize = 0;

        //FastMode need 16_align enc_height
        if (G_iso == 2) {
            //DSP lib need 32ALign for Width and 16_Align for height in buffer allocation
            YuvWidth = ALIGN_32(StillCapConfigData->FullviewWidth);
            YuvHeight = ALIGN_16(StillCapConfigData->FullviewHeight);
            YuvSize = YuvWidth*YuvHeight*2;
            ScrnW = ALIGN_32(StillCapConfigData->ScreennailWidth);
            ScrnH = ALIGN_16(StillCapConfigData->ScreennailHeight);
            ScrnSize = ScrnW*ScrnH*2;
            ThmW = ALIGN_32(StillCapConfigData->ThumbnailWidth);
            ThmH = ALIGN_16(StillCapConfigData->ThumbnailHeight);
            ThmSize = ThmW*ThmH*2;
        } else {
            //DSP lib need 32ALign for Width and 16_Align for height in buffer allocation
            YuvWidth = ALIGN_32(StillCapConfigData->FullviewWidth);
            YuvHeight = ALIGN_16(StillCapConfigData->FullviewHeight);
            YuvSize = YuvWidth*YuvHeight*2;
            YuvSize += (YuvSize*10)/100;
            ScrnW = ALIGN_32(StillCapConfigData->ScreennailWidth);
            ScrnH = ALIGN_16(StillCapConfigData->ScreennailHeight);
            ScrnSize = ScrnW*ScrnH*2;
            ScrnSize += (ScrnSize*10)/100;
            ThmW = ALIGN_32(StillCapConfigData->ThumbnailWidth);
            ThmH = ALIGN_16(StillCapConfigData->ThumbnailHeight);
            ThmSize = ThmW*ThmH*2;
            ThmSize += (ThmSize*10)/100;
        }

        totalSize = YuvSize*1 + ScrnSize*1 + ThmSize*1;
        if (totalSize > ApplibDspWorkAreaResvSize) {
            AmbaPrint("[Applib - StillEnc] <StillEncUtility> DspWork_Calculate: Memory shortage (%u %u)", totalSize, ApplibDspWorkAreaResvSize);
            (*addr) = ApplibDspWorkAreaResvStart;
            (*size) = ApplibDspWorkAreaResvSize;
            ReturnValue = -1;
        } else {
            (*addr) = ApplibDspWorkAreaResvStart;
            (*size) = ApplibDspWorkAreaResvSize - totalSize;
        }
    } else if (1) {
        //CaptureMode == STILL_CAPTURE_SINGLE_SHOT ||
        // CaptureMode == STILL_CAPTURE_RAW2YUV) {
        //allocate (Raw + YUV) buffer
        UINT16 RawPitch = 0, RawHeight = 0;
        UINT16 YuvWidth = 0, YuvHeight = 0, ScrnW = 0, ScrnH = 0, ThmW = 0, ThmH = 0;
        UINT32 RawSize = 0, YuvSize = 0, ScrnSize = 0, ThmSize = 0;
        UINT16 QvLCDW = 0, QvLCDH = 0, QvHDMIW = 0, QvHDMIH = 0;
        UINT32 QvLCDSize = 0, QvHDMISize = 0;

        RawPitch = (G_raw_cmpr)? \
            AMP_COMPRESSED_RAW_WIDTH(pMode->CaptureWidth): \
            pMode->CaptureWidth*2;
        RawHeight =  pMode->CaptureHeight;

        RawSize = RawPitch*RawHeight;

        //FastMode need 16_align enc_height
        if (G_iso == 2) {
            //DSP lib need 32ALign for Width and 16_Align for height in buffer allocation
            YuvWidth = ALIGN_32(StillCapConfigData->FullviewWidth);
            YuvHeight = ALIGN_16(StillCapConfigData->FullviewHeight);
            YuvSize = YuvWidth*YuvHeight*2;
            ScrnW = ALIGN_32(StillCapConfigData->ScreennailWidth);
            ScrnH = ALIGN_16(StillCapConfigData->ScreennailHeight);
            ScrnSize = ScrnW*ScrnH*2;
            ThmW = ALIGN_32(StillCapConfigData->ThumbnailWidth);
            ThmH = ALIGN_16(StillCapConfigData->ThumbnailHeight);
            ThmSize = ThmW*ThmH*2;
        } else {
            //DSP lib need 32ALign for Width and 16_Align for height in buffer allocation
            YuvWidth = ALIGN_32(StillCapConfigData->FullviewWidth);
            YuvHeight = ALIGN_16(StillCapConfigData->FullviewHeight);
            YuvSize = YuvWidth*YuvHeight*2;
            YuvSize += (YuvSize*10)/100;
            ScrnW = ALIGN_32(StillCapConfigData->ScreennailWidth);
            ScrnH = ALIGN_16(StillCapConfigData->ScreennailHeight);
            ScrnSize = ScrnW*ScrnH*2;
            ScrnSize += (ScrnSize*10)/100;
            ThmW = ALIGN_32(StillCapConfigData->ThumbnailWidth);
            ThmH = ALIGN_16(StillCapConfigData->ThumbnailHeight);
            ThmSize = ThmW*ThmH*2;
            ThmSize += (ThmSize*10)/100;
        }

        /* QV need 16_Align */
        QvLCDW = ALIGN_32(QvDchanWidth);
        QvLCDH = ALIGN_16(QvDchanHeight);
        QvHDMIW = ALIGN_32(QvFchanWidth);
        QvHDMIH = ALIGN_16(QvFchanHeight);
        QvLCDSize = QvLCDW*QvLCDH*2;
        QvLCDSize += (QvLCDSize*10)/100;
        QvHDMISize = QvHDMIW*QvHDMIH*2;
        QvHDMISize += (QvHDMISize*10)/100;

//        if (CaptureMode == STILL_CAPTURE_RAW2YUV) QvLCDSize = QvHDMISize = 0;

        totalSize = RawSize*G_capcnt + YuvSize*1 + ScrnSize*1 + ThmSize*1 + QvLCDSize*1 + QvHDMISize*1;
        if (totalSize > ApplibDspWorkAreaResvSize) {
            AmbaPrint("[Applib - StillEnc] <StillEncUtility> DspWork_Calculate: Memory shortage (%u %u)", totalSize, ApplibDspWorkAreaResvSize);
            (*addr) = ApplibDspWorkAreaResvStart;
            (*size) = ApplibDspWorkAreaResvSize;
            ReturnValue = -1;
        } else {
            (*addr) = ApplibDspWorkAreaResvStart;
            (*size) = ApplibDspWorkAreaResvSize - totalSize;
        }
    } else if (0) {//CaptureMode == STILL_CAPTURE_BURST_SHOT) {
        //allocate (Raw*CapCnt + YUV) buffer
        UINT16 RawPitch = 0, RawHeight = 0;
        UINT16 YuvWidth = 0, YuvHeight = 0, ScrnW = 0, ScrnH = 0, ThmW = 0, ThmH = 0;
        UINT32 RawSize = 0, YuvSize = 0, ScrnSize = 0, ThmSize = 0;
        UINT16 QvLCDW = 0, QvLCDH = 0, QvHDMIW = 0, QvHDMIH = 0;
        UINT32 QvLCDSize = 0, QvHDMISize = 0;

        RawPitch = (G_raw_cmpr)? \
            AMP_COMPRESSED_RAW_WIDTH(pMode->CaptureWidth): \
            pMode->CaptureWidth*2;
        RawHeight =  pMode->CaptureHeight;
        RawSize = RawPitch*RawHeight;

        //FastMode need 16_align enc_height
        if (G_iso == 2) {
            //DSP lib need 32ALign for Width and 16_Align for height in buffer allocation
            YuvWidth = ALIGN_32(StillCapConfigData->FullviewWidth);
            YuvHeight = ALIGN_16(StillCapConfigData->FullviewHeight);
            YuvSize = YuvWidth*YuvHeight*2;
            ScrnW = ALIGN_32(StillCapConfigData->ScreennailWidth);
            ScrnH = ALIGN_16(StillCapConfigData->ScreennailHeight);
            ScrnSize = ScrnW*ScrnH*2;
            ThmW = ALIGN_32(StillCapConfigData->ThumbnailWidth);
            ThmH = ALIGN_16(StillCapConfigData->ThumbnailHeight);
            ThmSize = ThmW*ThmH*2;
        } else {
            //DSP lib need 32ALign for Width and 16_Align for height in buffer allocation
            YuvWidth = ALIGN_32(StillCapConfigData->FullviewWidth);
            YuvHeight = ALIGN_16(StillCapConfigData->FullviewHeight);
            YuvSize = YuvWidth*YuvHeight*2;
            YuvSize += (YuvSize*10)/100;
            ScrnW = ALIGN_32(StillCapConfigData->ScreennailWidth);
            ScrnH = ALIGN_16(StillCapConfigData->ScreennailHeight);
            ScrnSize = ScrnW*ScrnH*2;
            ScrnSize += (ScrnSize*10)/100;
            ThmW = ALIGN_32(StillCapConfigData->ThumbnailWidth);
            ThmH = ALIGN_16(StillCapConfigData->ThumbnailHeight);
            ThmSize = ThmW*ThmH*2;
            ThmSize += (ThmSize*10)/100;
        }

        /* QV need 16_Align */
        QvLCDW = ALIGN_32(QvDchanWidth);
        QvLCDH = ALIGN_16(QvDchanHeight);
        QvHDMIW = ALIGN_32(QvFchanWidth);
        QvHDMIH = ALIGN_16(QvFchanHeight);
        QvLCDSize = QvLCDW*QvLCDH*2;
        QvLCDSize += (QvLCDSize*10)/100;
        QvHDMISize = QvHDMIW*QvHDMIH*2;
        QvHDMISize += (QvHDMISize*10)/100;

        totalSize = RawSize*G_capcnt + YuvSize*1 + ScrnSize*1 + ThmSize*1 + QvLCDSize*1 + QvHDMISize*1;
        if (totalSize > (ApplibDspWorkAreaResvSize-180*1024*1024)) {
            // 20131017 Chester: Per Edgar, empirically dsp may need at least 180MB to works well
            AmbaPrint("[Applib - StillEnc] <StillEncUtility> DspWork_Calculate: Memory shortage (%u %u)", totalSize, ApplibDspWorkAreaResvSize);
            (*addr) = ApplibDspWorkAreaResvStart;
            (*size) = ApplibDspWorkAreaResvSize;
            ReturnValue = -1;
        } else {
            (*addr) = ApplibDspWorkAreaResvStart;
            (*size) = ApplibDspWorkAreaResvSize - totalSize;
        }
    } else if (0) {//CaptureMode == STILL_CAPTURE_AEB) {
        //allocate (Raw*3 + YUV) buffer

    }
    AmbaPrint("[Applib - StillEnc] <StillEncUtility> DspWork_Calculate: Addr 0x%X, Sz %u, req %d", *addr, *size, totalSize);
    return ReturnValue;
}
#endif

/**
 * UnitTest: QuickView show buffer allocation
 *
 * @param [in] LCD qv buffer size
 * @param [in] HDMI qv buffer size
 *
 * @return 0 - success, -1 - fail
 */
int AppLibStillEnc_QvShowBufferAllocate(UINT32 QvLCDSize, UINT32 QvHDMISize)
{
    int ReturnValue;
    void *TempPtrBuf, *TempPtrBufRaw;

    if (s_qvLcdShowBuffAddrOdd == NULL) {
        ReturnValue = AmpUtil_GetAlignedPool(APPLIB_G_MMPL, &TempPtrBuf, &TempPtrBufRaw, QvLCDSize, 32);
        if (ReturnValue != OK) {
            AmbaPrintColor(RED,"[Applib - StillEnc] <StillEncUtility> QvShowBufferAllocate: NC_DDR alloc yuv_lcd_odd fail (%u)!", QvLCDSize);
            return ReturnValue;
        } else {
            s_qvLcdShowBuffAddrOdd = (UINT8*)TempPtrBuf;
            AmbaPrint("[Applib - StillEnc] <StillEncUtility> QvShowBufferAllocate: qvLCDBuffaddrOdd (0x%08X)!", s_qvLcdShowBuffAddrOdd);
        }
    }
    if (s_qvLcdShowBuffAddrEven == NULL) {
        ReturnValue = AmpUtil_GetAlignedPool(APPLIB_G_MMPL, &TempPtrBuf, &TempPtrBufRaw, QvLCDSize, 32);
        if (ReturnValue != OK) {
            AmbaPrintColor(RED,"[Applib - StillEnc] <StillEncUtility> QvShowBufferAllocate: NC_DDR alloc yuv_lcd_even fail (%u)!", QvLCDSize);
            return ReturnValue;
        } else {
            s_qvLcdShowBuffAddrEven = (UINT8*)TempPtrBuf;
            AmbaPrint("[Applib - StillEnc] <StillEncUtility> QvShowBufferAllocate: qvLCDBuffaddrEven (0x%08X)!", s_qvLcdShowBuffAddrEven);
        }
    }

    if (s_qvTvShowBuffAddrOdd == NULL) {
        ReturnValue = AmpUtil_GetAlignedPool(APPLIB_G_MMPL, &TempPtrBuf, &TempPtrBufRaw, QvHDMISize, 32);
        if (ReturnValue != OK) {
            AmbaPrintColor(RED,"[Applib - StillEnc] <StillEncUtility> QvShowBufferAllocate: NC_DDR alloc yuv_tv_odd fail (%u)!", QvHDMISize);
            return ReturnValue;
        } else {
            s_qvTvShowBuffAddrOdd = (UINT8*)TempPtrBuf;
            AmbaPrint("[Applib - StillEnc] <StillEncUtility> QvShowBufferAllocate: qvTVBuffaddrOdd (0x%08X)!", s_qvTvShowBuffAddrOdd);
        }
    }
    if (s_qvTvShowBuffAddrEven == NULL) {
        ReturnValue = AmpUtil_GetAlignedPool(APPLIB_G_MMPL, &TempPtrBuf, &TempPtrBufRaw, QvHDMISize, 32);
        if (ReturnValue != OK) {
            AmbaPrintColor(RED,"[Applib - StillEnc] <StillEncUtility> QvShowBufferAllocate: NC_DDR alloc yuv_tv_even fail (%u)!", QvHDMISize);
            return ReturnValue;
        } else {
            s_qvTvShowBuffAddrEven = (UINT8*)TempPtrBuf;
            AmbaPrint("[Applib - StillEnc] <StillEncUtility> QvShowBufferAllocate: qvTVBuffaddrEvev (0x%08X)!", s_qvTvShowBuffAddrEven);
        }
    }

    return ReturnValue;
}
