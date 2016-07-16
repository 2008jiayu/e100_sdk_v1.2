/**
 * @file src/app/connected/applib/src/applib/monitor/ApplibMonitor_BitrateHdlr.c
 *
 * Implementation of bitrate control handler
 *
 * History:
 *    2014/07/23 - [Chester Chuang] created file
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

#include <stdio.h>
#include <stdlib.h>
#include <AmbaDataType.h>
#include <AmbaKAL.h>
#include <imgproc/AmbaImg_Impl_Cmd.h>
#include <applib.h>
#include <monitor/ApplibTimerMonitor.h>
#include <AmbaDSP_Event.h>

typedef struct _AmbaBitRateControl_s_ {
#define AMBA_BRCHDLR_MAX_STREAM_NUM 4
    UINT8 TuneCurrStep[AMBA_BRCHDLR_MAX_STREAM_NUM];
    UINT8 TunePrevStep[AMBA_BRCHDLR_MAX_STREAM_NUM];
    UINT8 Reserved[2];
    BITRATE_CONTROL_LUMA_HANDLER_s LumaHdlr[AMBA_BRCHDLR_MAX_STREAM_NUM];
    BITRATE_CONTROL_DZOOM_HANDLER_s DzoomHdlr[AMBA_BRCHDLR_MAX_STREAM_NUM];
    BITRATE_CONTROL_COMPLEXITY_HANDLER_s CplxHdlr[AMBA_BRCHDLR_MAX_STREAM_NUM];
} AmbaBitRateControl_s;

static double BrcSlope_H[AMBA_BRCHDLR_MAX_STREAM_NUM] = {0};
static double BrcSlope_L[AMBA_BRCHDLR_MAX_STREAM_NUM] = {0};
static UINT8 brc_handler_init = 0;
static AmbaBitRateControl_s G_brc_ctrl;
static UINT8 Day = 1;
static UINT8 DayCnt = 0;
static UINT8 BrcDebugMode = 0;
#define DAYCNT_THRESHOLD 5

static void AmbaBitRateControl_init(void)
{
    if (brc_handler_init) {
        return;
    } else {
        memset(&G_brc_ctrl, 0x0, sizeof(AmbaBitRateControl_s));
        brc_handler_init = 1;
        return;
    }
}

static UINT32 AmbaBitRateControl_GetNewVBR(UINT8 bitRateStep, UINT8 streamId)
{
    UINT32 Rval = 0;
    UINT32 AverBitRate = 0, MinBitRate = 0, MaxBitRate = 0;
    AMP_VIDEOENC_BITSTREAM_CFG_s CurrConfig;

    extern AMP_AVENC_HDLR_s *VideoEncPri;
    //extern AMP_AVENC_HDLR_s *VideoEncSec;

    AmpVideoEnc_GetBitstreamConfig(VideoEncPri, &CurrConfig); //FIXME: Pri? Sec?

    AverBitRate = CurrConfig.Spec.H264Cfg.BitRateControl.AverageBitrate;
    if (VIDEOENC_SMART_VBR == CurrConfig.Spec.H264Cfg.BitRateControl.BrcMode) {
        MinBitRate = CurrConfig.Spec.H264Cfg.BitRateControl.MinBitrate;
        MaxBitRate = CurrConfig.Spec.H264Cfg.BitRateControl.MaxBitrate;
    }

    if ((MinBitRate == AverBitRate) && (bitRateStep <= 2)) {
        switch (bitRateStep) {
        case 0:
            Rval = (AverBitRate/100)*90;
            break;
        case 1:
            Rval = (AverBitRate/100)*95;
            break;
        case 2:
            Rval = AverBitRate;
            break;
        }
    } else if ((MaxBitRate == AverBitRate) && (bitRateStep >= 2)) {
        switch (bitRateStep) {
        case 2:
            Rval = AverBitRate;
            break;
        case 3:
            Rval = (AverBitRate/100)*105;
            break;
        case 4:
            Rval = (AverBitRate/100)*110;
            break;
        default:
            Rval = (AverBitRate/100)*(100+5*(bitRateStep-2));
            break;
        }
    } else {
        switch (bitRateStep) {
        case 0:
            Rval = MinBitRate;
            break;
        case 1:
            Rval = (AverBitRate + MinBitRate)>>1;
            break;
        case 2:
            Rval = AverBitRate;
            break;
        case 3:
            Rval = (AverBitRate + MaxBitRate)>>1;
            break;
        case 4:
            Rval = MaxBitRate;
            break;
        default:
            Rval = MaxBitRate*(100+5*(bitRateStep-4))/100;
            break;
        }
    }

    AmbaPrint("[Applib - BitRate Hdlr] <New VBR> min=%d aver=%d max=%d step=%d rval=%d",\
            MinBitRate, AverBitRate, MaxBitRate, bitRateStep, Rval);
    return Rval;
}

void AmbaBitRateControl_SetDebugMode(UINT8 debugMode)
{
    BrcDebugMode = debugMode;
    AmbaPrint("[Applib - BitRate Hdlr] <Set DebugMode> debug mode = %d", BrcDebugMode);
}

void AmbaBitRateControl_RegisterComplexityHandler(UINT8 streamId, BITRATE_CONTROL_COMPLEXITY_HANDLER_s *hdlr)
{
    if (0 == brc_handler_init) AmbaBitRateControl_init();
    //FIXME: check streamID is valid
    memcpy(&G_brc_ctrl.CplxHdlr[streamId], hdlr, sizeof(BITRATE_CONTROL_COMPLEXITY_HANDLER_s));
}

void AmbaBitRateControl_RegisterLumaHandler(UINT8 streamId, BITRATE_CONTROL_LUMA_HANDLER_s *hdlr)
{
    if (0 == brc_handler_init) AmbaBitRateControl_init();
    //FIXME: check streamID is valid
    memcpy(&G_brc_ctrl.LumaHdlr[streamId], hdlr, sizeof(BITRATE_CONTROL_LUMA_HANDLER_s));
}
void AmbaBitRateControl_RegisterDzoomHandler(UINT8 streamId, BITRATE_CONTROL_DZOOM_HANDLER_s *hdlr)
{
    if (0 == brc_handler_init) AmbaBitRateControl_init();
    //FIXME: check streamID is valid
    memcpy(&G_brc_ctrl.DzoomHdlr[streamId], hdlr, sizeof(BITRATE_CONTROL_DZOOM_HANDLER_s));
}

void AmbaBitRateControl_ComplexityHandler(UINT32 *targetBitRate, UINT32 currBitRate, UINT8 streamId)
{
    extern AMP_AVENC_HDLR_s *VideoEncPri;
    //extern AMP_AVENC_HDLR_s *VideoEncSec;
    AMP_VIDEOENC_BITSTREAM_CFG_s CurrConfig;
    AMBA_DSP_EVENT_RGB_3A_DATA_s RGBstat;
    UINT32 MinBitRate, AverBitRate, MaxBitRate;
    UINT8 IsVhdr, IsOverSample;
    UINT8 LumaMode, MultiMode, Region;
    UINT32 WindowSize, TileNum;
    UINT32 FV2SumRaw = 0, LVSumRaw = 0, FV2Sum, LVSum;
    UINT32 FV2Min, FV2Mid, FV2Max;

    /* Step 1. Sanity check */
    if (NULL == G_brc_ctrl.CplxHdlr[streamId].GetComplexityRangeCB ||\
       NULL == G_brc_ctrl.CplxHdlr[streamId].GetDayLumaThresCB ||\
       NULL == G_brc_ctrl.CplxHdlr[streamId].GetPipeModeCB) {
       *targetBitRate = 0;
       if (BrcDebugMode || (G_brc_ctrl.CplxHdlr[streamId].DebugPattern & 0x1))
           AmbaPrint("[Applib - BitRate Hdlr] <Complex hdlr> Please hook incorrectCB");

       return;
    }

    /* Step 2. Get App settings */
    AmpVideoEnc_GetBitstreamConfig(VideoEncPri, &CurrConfig);
    AverBitRate = CurrConfig.Spec.H264Cfg.BitRateControl.AverageBitrate;
    if (VIDEOENC_SMART_VBR == CurrConfig.Spec.H264Cfg.BitRateControl.BrcMode) {
        MinBitRate = CurrConfig.Spec.H264Cfg.BitRateControl.MinBitrate;
        MaxBitRate = CurrConfig.Spec.H264Cfg.BitRateControl.MaxBitrate;
    }

    /* Step 3. Get current pipeline setting */
    G_brc_ctrl.CplxHdlr[streamId].GetPipeModeCB(&IsVhdr, &IsOverSample);
    //FIXME
    //AMP_img_sensor_cmd(MW_IMG_PIPELINE_INFO, 1, &pipe);
    //if (pipe.pipeline == PIPELINE_OVERSMP) isOverSample = 1; //double confirm with MW
    LumaMode = (IsVhdr)?1:((IsOverSample)?2:0);

    /* Step 4. Get current statistics */
    AmbaImg_Proc_Cmd(MW_IP_GET_RGB_3A_STAT, 0, (UINT32)&RGBstat, 0);
    WindowSize = RGBstat.Header.AfTileActiveWidth*RGBstat.Header.AfTileActiveHeight;
    // FIXME:multi_mode = AMP_img_algo_cmd(MW_IA_GET_MULTI_CHAN_MODE, 0, 0);
     MultiMode = 0;

    /* Step 5. Calculate AF stat */
    if (1 == MultiMode) {
        UINT8 x, y, ColNum, RowNum, TileOfst;
        ColNum = RGBstat.Header.AfTileNumRow/2;
        RowNum = RGBstat.Header.AfTileNumRow;

        TileNum = ColNum*RowNum;
        TileOfst = (0 == streamId)?0:ColNum;

        if (TileNum) {
            for (x=0;x<RowNum;x++) {
                for (y=0;y<ColNum;y++) {
                    FV2SumRaw += RGBstat.Af[x*(ColNum*2)+y+TileOfst].SumFV2;
                    LVSumRaw += RGBstat.Af[x*(ColNum*2)+y+TileOfst].SumFY;
                }
            }
            // FIXME: should manually refine below adjustment
            FV2SumRaw = FV2SumRaw*3; //*2*1.5
            LVSumRaw = LVSumRaw*3;

            WindowSize = (TileNum*2)*WindowSize;
        } else {
            WindowSize = 0;
        }

        if (BrcDebugMode || (G_brc_ctrl.CplxHdlr[streamId].DebugPattern & 0x1)) {
            AmbaPrint("[Multi Cplx] (%d): Sz(%d %d) Ofst(%d) (FV:%d LV:%d)", streamId, RowNum, ColNum, TileOfst,\
                FV2SumRaw, LVSumRaw);
        }
    } else {
        if (IsVhdr) {
            UINT8 x, y;
            UINT8 ColNum = RGBstat.Header.AfTileNumCol/2;
            UINT8 RowNum = RGBstat.Header.AfTileNumRow;
            TileNum = ColNum*RowNum;

            if (TileNum) {
                for (x=0;x<RowNum;x++) {
                    for (y=0;y<ColNum;y++) {
                        FV2SumRaw += RGBstat.Af[x*RowNum + ColNum + y].SumFV2;
                        LVSumRaw += RGBstat.Af[x*RowNum + ColNum + y].SumFY;
                    }
                }
                WindowSize*= TileNum;
            } else {
                WindowSize = 0;
            }

            if (BrcDebugMode || (G_brc_ctrl.CplxHdlr[streamId].DebugPattern & 0x1)) {
                AmbaPrint("[Single VHDR] (%d): Sz(%d %d) (FV:%d LV:%d)", streamId, RowNum, ColNum, FV2SumRaw, LVSumRaw);
            }
        } else {
            UINT8 i;
            TileNum = RGBstat.Header.AfTileNumCol*RGBstat.Header.AfTileNumRow;

            if (TileNum) {
                for (i=0;i<TileNum;i++) {
                    FV2SumRaw += RGBstat.Af[i].SumFV2;
                    LVSumRaw += RGBstat.Af[i].SumFY;
                }
                WindowSize*= TileNum;
            } else {
                WindowSize = 0;
            }

            if (BrcDebugMode || (G_brc_ctrl.CplxHdlr[streamId].DebugPattern & 0x1)) {
                AmbaPrint("[Single ] (%d): Sz(%d %d) (FV:%d LV:%d)", streamId, RGBstat.Header.AfTileNumRow,\
                    RGBstat.Header.AfTileNumCol, FV2SumRaw, LVSumRaw);
            }
        }
    }

    /* Print Af statistics if needed */
    if (BrcDebugMode || (G_brc_ctrl.CplxHdlr[streamId].DebugPattern & 0x1)) {
        char LineBuf[128];
        UINT8 x, y;
        AmbaPrint("---  FV2 statistics   ---\n");
        for (x=0;x<RGBstat.Header.AfTileNumRow;x++) {
            for (y=0;y<RGBstat.Header.AfTileNumRow;y++) {
                sprintf(&LineBuf[6*x], "%6d", (int)&(RGBstat.Af[0]).SumFV2);
            }
            sprintf(&LineBuf[6*y], "%c", '\0');
            AmbaPrint("%s", LineBuf);
            AmbaKAL_TaskSleep(10);
        }
        AmbaPrint("\n");
        AmbaPrint("---  Y statistics   ---\n");
        for (x=0;x<RGBstat.Header.AfTileNumRow;x++) {
            for (y=0;y<RGBstat.Header.AfTileNumRow;y++) {
                sprintf(&LineBuf[6*x], "%6d", (int)&(RGBstat.Af[0]).SumFY);
            }
            sprintf(&LineBuf[6*y], "%c", '\0');
            AmbaPrint("%s", LineBuf);
            AmbaKAL_TaskSleep(10);
        }
        AmbaPrint("\n");
    }

    /* Step 6. Normalize AF stat */
    if (WindowSize) {
        UINT64 AdjFV2Sum = (UINT64)FV2SumRaw<<20; // empirical value
        UINT64 AdjLVSum = (UINT64)LVSumRaw<<20;
        FV2Sum = (UINT32)(AdjFV2Sum/WindowSize);
        LVSum = (UINT32)(AdjLVSum/WindowSize);
    } else {
        FV2Sum = 100000;
        LVSum = 100000;
    }

    /* Step 7. Decide threshold */
    {
        UINT8 DayTmp;
        UINT32 LumaThreshold;

        G_brc_ctrl.CplxHdlr[streamId].GetDayLumaThresCB(LumaMode, &LumaThreshold);
        DayTmp = (LVSum > LumaThreshold)?1:0;
        if (DayTmp != Day) {
            DayCnt++;
            if (DayCnt >= DAYCNT_THRESHOLD) {
                Day = DayTmp;
                DayCnt = 0;
            }
        } else {
            DayCnt = 0;
        }

        if (Day) {
            if (1 == LumaMode)
                G_brc_ctrl.CplxHdlr[streamId].GetComplexityRangeCB(2, &FV2Min, &FV2Mid, &FV2Max);
            else if ((2 == LumaMode) || (0/*multi_mode = mulit*/))
                G_brc_ctrl.CplxHdlr[streamId].GetComplexityRangeCB(4, &FV2Min, &FV2Mid, &FV2Max);
            else
                G_brc_ctrl.CplxHdlr[streamId].GetComplexityRangeCB(0, &FV2Min, &FV2Mid, &FV2Max);
        } else {
            if (1 == LumaMode)
                G_brc_ctrl.CplxHdlr[streamId].GetComplexityRangeCB(3, &FV2Min, &FV2Mid, &FV2Max);
            else if ((2 == LumaMode) || (0/*multi_mode = mulit*/))
                G_brc_ctrl.CplxHdlr[streamId].GetComplexityRangeCB(5, &FV2Min, &FV2Mid, &FV2Max);
            else
                G_brc_ctrl.CplxHdlr[streamId].GetComplexityRangeCB(1, &FV2Min, &FV2Mid, &FV2Max);
        }

        if (0 == BrcSlope_H[streamId])
            BrcSlope_H[streamId] = (1.0*(MaxBitRate-AverBitRate)/(FV2Max-FV2Mid));
        if (0 == BrcSlope_L[streamId])
            BrcSlope_L[streamId] = (1.0*(AverBitRate-MinBitRate)/(FV2Mid-FV2Min));
    }

    /* Step 8. Target bitrate calculation */
    if (FV2Sum <= FV2Min) {
        Region = 1;
        *targetBitRate = MinBitRate;
    } else if (FV2Sum < FV2Mid) {
        Region = 2;
        *targetBitRate = MinBitRate + (UINT32)(1.0*(FV2Sum-FV2Min)*BrcSlope_L[streamId]);
    } else if (FV2Sum < FV2Max) {
        Region = 3;
        *targetBitRate = AverBitRate + (UINT32)(1.0*(FV2Sum-FV2Mid)*BrcSlope_H[streamId]);
    } else {
        Region = 4;
        *targetBitRate = MaxBitRate;
    }

    AmbaPrint("[%s %d][%d][R%d %d]: Fv2(%d %d) Lv(%d %d)", (Day)?"DAY":"NITE", LumaMode, streamId, Region,\
        WindowSize, FV2Sum, FV2SumRaw, LVSum, LVSumRaw);
    AmbaPrint("[Cplx]: bitrate %d -> %d range(%d~%d) \n", currBitRate, *targetBitRate, MinBitRate, MaxBitRate);
}

void AmbaBitRateControl_LumaHandler(UINT32 *targetBitRate, UINT32 currBitRate, UINT8 streamId)
{
    UINT8 *BitrateTunePrevStep = 0, *BitrateTuneCurrStep = 0;
    UINT16 CurrLevel;

    // FIXME: streamID
    if (streamId >= AMBA_BRCHDLR_MAX_STREAM_NUM) {
        AmbaPrint("%s: wrong stream id(%d)", __func__, streamId);
        *targetBitRate = 0;
        return;
    }
    // FIXME: curr lv
    CurrLevel = 0;
    if (0/*AMP_img_algo_cmd(MW_IA_GET_CURR_LV, &CurrLevel, 0) < 0*/) {
        AmbaPrint("%s: cannot get current lv", __func__);
        *targetBitRate = 0;
        return;
    }


    BitrateTunePrevStep = &G_brc_ctrl.TunePrevStep[streamId];
    BitrateTuneCurrStep = &G_brc_ctrl.TuneCurrStep[streamId];

    if (CurrLevel < G_brc_ctrl.LumaHdlr[streamId].LowLumaThres) {
        if (currBitRate < AmbaBitRateControl_GetNewVBR(2, streamId)) {
            *targetBitRate = AmbaBitRateControl_GetNewVBR((*BitrateTuneCurrStep)+1, streamId);
            if (*BitrateTunePrevStep == 255) *BitrateTunePrevStep = *BitrateTuneCurrStep;
            (*BitrateTuneCurrStep)++;
        }
    } else if (CurrLevel < G_brc_ctrl.LumaHdlr[streamId].LumaThres) {
        if (currBitRate < AmbaBitRateControl_GetNewVBR(3, streamId)) {
            *targetBitRate = AmbaBitRateControl_GetNewVBR((*BitrateTuneCurrStep)+1, streamId);
            if (*BitrateTunePrevStep == 255) *BitrateTunePrevStep = *BitrateTuneCurrStep;
            (*BitrateTuneCurrStep)++;
        }
    } else if (*BitrateTunePrevStep != 255) {
        *BitrateTuneCurrStep = *BitrateTunePrevStep; // scale back to previous bitrate
        *BitrateTunePrevStep = 255;
    }
}

void AmbaBitRateControl_DzoomHandler(UINT32 *targetBitRate, UINT32 currBitRate, UINT8 streamId)
{
    UINT8 *BitrateTunePrevStep = 0, *BitrateTuneCurrStep = 0;

    // FIXME: streamID
    if (streamId >= AMBA_BRCHDLR_MAX_STREAM_NUM) {
        AmbaPrint("%s: wrong stream id(%d)", __func__, streamId);
        *targetBitRate = 0;
        return;
    }

    BitrateTunePrevStep = &G_brc_ctrl.TunePrevStep[streamId];
    BitrateTuneCurrStep = &G_brc_ctrl.TuneCurrStep[streamId];

    if (currBitRate < AmbaBitRateControl_GetNewVBR(2, streamId)) {
        *targetBitRate = AmbaBitRateControl_GetNewVBR((*BitrateTuneCurrStep)+1, streamId);
        if (*BitrateTunePrevStep == 255) *BitrateTunePrevStep = *BitrateTuneCurrStep;
        (*BitrateTuneCurrStep)++;

    } else if (*BitrateTunePrevStep != 255) {
        *BitrateTuneCurrStep = *BitrateTunePrevStep; // scale back to previous bitrate
        *BitrateTunePrevStep = 255;
    }
}
