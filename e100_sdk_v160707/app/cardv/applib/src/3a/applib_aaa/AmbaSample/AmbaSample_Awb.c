/**
 * @file app/connected/applob/src/3a/applib_aaa/ambasample/AmbaSample_AWB.h
 *
 * Sample Auto White Balance (AWB) algorithm
 *
 * History:
 *    2013/03/12 - [Jyh-Jiun Li] created file
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
#include <string.h>
#include <math.h>

#include <AmbaDataType.h>
#include <AmbaPrintk.h>

#include <AmbaDSP_ImgFilter.h>
#include <AmbaDSP_Img3aStatistics.h>

#include <imgproc/AmbaImg_Proc.h>
#include <imgproc/AmbaImg_AeAwb.h>
#include "AmbaSample_AeAwbAdj.h"
#include <imgproc/AmbaImg_Impl_Cmd.h>


#define MAX_CHANNEL_NO 2
#define MAX_AWB_TILE_SIZE 1024
#define WB_SKIP_FRAMES 1

typedef struct _SAMPLE_AWB_CTRL_s_ {
    UINT32    TilesRAvg[MAX_AWB_TILE_SIZE];
    UINT32    TilesGAvg[MAX_AWB_TILE_SIZE];
    UINT32    TilesBAvg[MAX_AWB_TILE_SIZE];
    UINT8     SkipFrames;
    AMBA_DSP_EVENT_CFA_3A_DATA_s *pCfaData;
    AMBA_DSP_EVENT_RGB_3A_DATA_s *pRgbData;
}SAMPLE_AWB_CTRL_s;


SAMPLE_AWB_CTRL_s *pSampleAwbCtrl[MAX_CHANNEL_NO];
extern AMBA_KAL_BYTE_POOL_t G_MMPL;

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaSample_Awb_MemInit
 *
 *  @Description:: To initialize the AWB algo related memory
 *
 *  @Input      ::
 *     UINT32 chNoCount : Total Channel count
 *
 *  @Output     :: none
 *
 *  @Return     :: none
 *
\*-----------------------------------------------------------------------------------------------*/
int AmbaSample_Awb_MemInit(UINT32 chNoCount)
{
    static UINT8 InitFlg = 0;
    int RVal = 0;
    UINT32 i;

    if (InitFlg == 0) {
        for (i = 0;i < chNoCount;i++) {
            RVal = AmbaKAL_BytePoolAllocate(&G_MMPL, (void **)&pSampleAwbCtrl[i], sizeof(SAMPLE_AWB_CTRL_s), 100);
            if (RVal != 0x10) {
                memset(pSampleAwbCtrl[i], 0, sizeof(SAMPLE_AWB_CTRL_s));
                pSampleAwbCtrl[i]->SkipFrames = WB_SKIP_FRAMES;

            }
        }
        AmbaPrint("<%s> chNoCount : %d, RVal = 0x%x , size : %d",  __FUNCTION__, i, RVal, chNoCount*sizeof(SAMPLE_AWB_CTRL_s));
        InitFlg = 1;
    }

    return RVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaSample_AwbInit
 *
 *  @Description:: To initialize the AWB algorithm related settings
 *
 *  @Input      ::
 *     UINT32 chNo : handler for multi-task
 *
 *  @Output     :: none
 *
 *  @Return     :: none
 *
\*-----------------------------------------------------------------------------------------------*/
void AmbaSample_AwbInit(UINT32 chNo)
{
    static UINT8 InitFlg = 0;
    UINT32 MaxChNo = 0;
    AMBA_DSP_IMG_WB_GAIN_s InitWBGain = {4096, 4096, 4096};

    if (InitFlg == 0) { //FIXME
        AmbaImg_Proc_Cmd(MW_IP_GET_TOTAL_CH_COUNT, (UINT32)&MaxChNo, 0, 0);
        AmbaSample_Awb_MemInit(MaxChNo);
        InitFlg = 1;
    }
    AmbaImg_Proc_Cmd(MW_IP_GET_WB_GAIN, chNo, (UINT32)&InitWBGain, 0);
    AmbaImg_Proc_Cmd(MW_IP_SET_WB_GAIN, chNo, (UINT32)&InitWBGain, 0);
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaSample_Compute_Awb_Tile_Value
 *
 *  @Description:: Compute Awb tile value
 *
 *  @Input      :: none
 *
 *  @Output     ::
 *     UINT16 *awbTileRow : Pointer to Awb tile row number
 *     UINT16 *awbTileCol : Pointer to Awb tile col number
 *     UINT32 *awbTilesRAvg : Pointer to Awb Red tile row average
 *     UINT32 *awbTilesGAvg : Pointer to Awb Green tile row average
 *     UINT32 *awbTilesBAvg : Pointer to Awb Blue tile row average
 *
 *  @Return     ::
 *     INT : OK(0)/NG(-1)
 *
\*-----------------------------------------------------------------------------------------------*/
int AmbaSample_Compute_Awb_Tile_Value(UINT32 chNo, UINT16 *awbTileRow,UINT16 *awbTileCol,UINT32 *awbTilesRAvg,UINT32 *awbTilesGAvg,UINT32 *awbTilesBAvg)
{
    int                             Rval = 0;
    long long                       TilePixel;
    UINT16                          TileRow;
    UINT16                          TileCol;
    int                             i,j;
    UINT32                          CurTileNo;
    long long                       TilePixelOffset;
    long long                       Tmp;


    AmbaImg_Proc_Cmd(MW_IP_GET_CFA_3A_STAT_ADDR, (UINT32)chNo, (UINT32)&pSampleAwbCtrl[chNo]->pCfaData, 0);
    TilePixel = (pSampleAwbCtrl[chNo]->pCfaData->Header.AwbTileWidth * pSampleAwbCtrl[chNo]->pCfaData->Header.AwbTileHeight) / 4;

    if (TilePixel == 0) {
        return -1;
    }

    TileRow = pSampleAwbCtrl[chNo]->pCfaData->Header.AwbTileNumRow;
    TileCol = pSampleAwbCtrl[chNo]->pCfaData->Header.AwbTileNumCol;
    for (i = 0; i < TileRow; i ++) {
        for (j = 0; j < TileCol; j ++) {

            CurTileNo = i*TileCol + j;
            TilePixelOffset = (pSampleAwbCtrl[chNo]->pCfaData->Awb[CurTileNo].CountMin + pSampleAwbCtrl[chNo]->pCfaData->Awb[CurTileNo].CountMax) / 4;

            if (TilePixelOffset > 0) {
                TilePixelOffset = TilePixel - TilePixelOffset;
            } else {
                TilePixelOffset = TilePixel;
            }
            if (TilePixelOffset > 0) {

                Tmp = ((long long)pSampleAwbCtrl[chNo]->pCfaData->Awb[CurTileNo].SumG << pSampleAwbCtrl[chNo]->pCfaData->Header.AwbRgbShift) / TilePixelOffset;
                awbTilesGAvg[CurTileNo] = Tmp;
                Tmp = ((long long)pSampleAwbCtrl[chNo]->pCfaData->Awb[CurTileNo].SumR << pSampleAwbCtrl[chNo]->pCfaData->Header.AwbRgbShift) / TilePixelOffset;
                awbTilesRAvg[CurTileNo] = Tmp;
                Tmp = ((long long)pSampleAwbCtrl[chNo]->pCfaData->Awb[CurTileNo].SumB << pSampleAwbCtrl[chNo]->pCfaData->Header.AwbRgbShift) / TilePixelOffset;
                awbTilesBAvg[CurTileNo] = Tmp;
            } else {

                awbTilesGAvg[CurTileNo] = 0;
                awbTilesRAvg[CurTileNo] = 0;
                awbTilesBAvg[CurTileNo] = 0;
            }
        }
    }

    *awbTileRow = TileRow;
    *awbTileCol = TileCol;
    return Rval;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AWB_GrayWorldMode
 *
 *  @Description:: Calculate the WB gain value.
 *
 *  @Input      ::
 *     UINT32 chNo : handler for multi-task
 *
 *  @Output     ::
 *     AMBA_DSP_IMG_WB_GAIN_s *awbGain : Pointer to wb gain
 *
 *  @Return     ::
 *      UINT8   : 0(Whites) / 1(NoWhites)
\*-----------------------------------------------------------------------------------------------*/
static UINT8 AWB_GrayWorldMode(UINT32 chNo, AMBA_DSP_IMG_WB_GAIN_s *awbGain)
{
    UINT16                        AwbTileRow;
    UINT16                        AwbTileCol;

    UINT32                        i;
    UINT8                         AwbNoWhites = 1;
    long long                     RSum, BSum, Cnt;


    AmbaSample_Compute_Awb_Tile_Value(chNo, &AwbTileRow, &AwbTileCol, pSampleAwbCtrl[chNo]->TilesRAvg, pSampleAwbCtrl[chNo]->TilesGAvg, pSampleAwbCtrl[chNo]->TilesBAvg);

    RSum = 0;
    BSum = 0;
    Cnt = 0;
    for (i = 0; i < AwbTileRow * AwbTileCol; i++) {
        if (pSampleAwbCtrl[chNo]->TilesRAvg[i] > 0 && pSampleAwbCtrl[chNo]->TilesGAvg[i] > 0 && pSampleAwbCtrl[chNo]->TilesBAvg[i] > 0) {
            RSum += pSampleAwbCtrl[chNo]->TilesGAvg[i]* WB_UNIT_GAIN / pSampleAwbCtrl[chNo]->TilesRAvg[i];
            BSum += pSampleAwbCtrl[chNo]->TilesGAvg[i]* WB_UNIT_GAIN / pSampleAwbCtrl[chNo]->TilesBAvg[i];
            Cnt ++;
        }
    }

    if (Cnt > 0 ) {
        awbGain->GainR = RSum / Cnt;
        awbGain->GainG = WB_UNIT_GAIN;
        awbGain->GainB = BSum / Cnt;
        AwbNoWhites = 0;
    }
    return (AwbNoWhites);
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaSample_AwbControl
 *
 *  @Description:: AWB algorithm control API
 *
 *  @Input      ::
 *     UINT32 chNo : handler for multi-task
 *
 *  @Output     ::
 *    aaaVideoStatus: Pointer to Video Aaa status
 *    aaaStillStatus: Pointer to Still Aaa status
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
void AmbaSample_AwbControl(UINT32 chNo, AMBA_3A_STATUS_s *aaaVideoStatus, AMBA_3A_STATUS_s *aaaStillStatus)
{
    UINT8                       AwbNoWhites;
    AMBA_DSP_IMG_WB_GAIN_s      CurAwbGain, NextAwbGain;


    if (pSampleAwbCtrl[chNo]->SkipFrames > 0) {
        pSampleAwbCtrl[chNo]->SkipFrames--;
        return;
    }

    AmbaImg_Proc_Cmd(MW_IP_GET_WB_GAIN, chNo, (UINT32)&CurAwbGain, 0);

    NextAwbGain = CurAwbGain;
    AwbNoWhites = AWB_GrayWorldMode(chNo, &NextAwbGain);

    if (AwbNoWhites) {
        return;
    }
    NextAwbGain.GainR = (NextAwbGain.GainR * 4 + CurAwbGain.GainR * 60) / 64;
    NextAwbGain.GainB = (NextAwbGain.GainB * 4 + CurAwbGain.GainB * 60) / 64;

    aaaStillStatus->Awb = AMBA_LOCK;
    AmbaImg_Proc_Cmd(MW_IP_SET_WB_GAIN, chNo, (UINT32)&NextAwbGain, 0);

    pSampleAwbCtrl[chNo]->SkipFrames = WB_SKIP_FRAMES;
    return;
}
