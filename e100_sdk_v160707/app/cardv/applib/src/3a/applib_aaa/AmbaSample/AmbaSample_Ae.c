/**
 * @file app/connected/applob/src/3a/applib_aaa/ambasample/AmbaSample_Ae.c
 *
 * Sample Auto Adjust algorithm
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
#include <applib.h>
#define MAX_CHANNEL_NO 2

typedef struct _SAMPLE_AE_CTRL_s_ {
    AMBA_KAL_MUTEX_t Mutex;
    float            MinRate;
    float            MaxRate;
    float            MinAgcDb;
    float            MaxAgcDb;
    UINT16           MinDgain;
    UINT16           MaxDgain;

    float            Shutter;
    float            Agc;
    UINT32           Dgain;

    AMBA_AE_INFO_s   VideoInfo;
    AMBA_AE_INFO_s   StillInfo[MAX_AEB_NUM];
    AMBA_DSP_EVENT_CFA_3A_DATA_s *pCfaData;
    AMBA_DSP_EVENT_RGB_3A_DATA_s *pRgbData;
} SAMPLE_AE_CTRL_s;

static SAMPLE_AE_CTRL_s *pSampleAeCtrl[MAX_CHANNEL_NO];
extern UINT32 IpctestChno;
/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaSample_Ae_MemInit
 *
 *  @Description:: To initialize the AE algo related memory
 *
 *  @Input      ::
 *     UINT32 chNoCount : Total Channel count
 *
 *  @Output     :: none
 *
 *  @Return     :: none
 *
\*-----------------------------------------------------------------------------------------------*/
int AmbaSample_Ae_MemInit(UINT32 chNoCount)
{
    static UINT8 InitFlg = 0;
    int RVal = 0;
    UINT32 i;

    if (InitFlg == 0) {
        for (i = 0;i < chNoCount;i++) {
            RVal = AmbaKAL_BytePoolAllocate(APPLIB_G_MMPL, (void **)&pSampleAeCtrl[i], sizeof(SAMPLE_AE_CTRL_s), 100);
            if (RVal != 0x10) {
                memset(pSampleAeCtrl[i], 0, sizeof(SAMPLE_AE_CTRL_s));
                AmbaKAL_MutexCreate(&pSampleAeCtrl[i]->Mutex);
                pSampleAeCtrl[i]->MinRate = 0;
                pSampleAeCtrl[i]->MaxRate = 1;
                pSampleAeCtrl[i]->MinAgcDb = 0;
                pSampleAeCtrl[i]->MaxAgcDb = 18;
                pSampleAeCtrl[i]->MinDgain = 4096;
                pSampleAeCtrl[i]->MaxDgain = 8192;
            }
        }
        AmbaPrint("<%s> chNoCount : %d, RVal = 0x%x , size : %d",  __FUNCTION__, i, RVal, chNoCount*sizeof(SAMPLE_AE_CTRL_s));
        InitFlg = 1;
    }

    return RVal;
}



/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AeGetVideoExposureValue
 *
 *  @Description:: Get Video Exposure value
 *
 *  @Input      ::
 *     UINT32 chNo : handler for multi-task
 *
 *  @Output     ::
 *     float *shutter : Pointer to shutter speed in second
 *     float *agc : Pointer to sensor gain in db
 *     UINT32 *dgain : Pointer to digital gain(unit gain: 4096)
 *
 *  @Return     ::
 *     INT : OK(0)/NG(-1)
 *
\*-----------------------------------------------------------------------------------------------*/
int AeGetVideoExposureValue(UINT32 chNo, float *shutter, float *agc, UINT32 *dgain)
{
    if (IpctestChno != 0xFFFFFFFF)
    chNo = 0;
    AmbaKAL_MutexTake(&pSampleAeCtrl[chNo]->Mutex, AMBA_KAL_WAIT_FOREVER);
    *shutter = pSampleAeCtrl[chNo]->Shutter;
    *agc = pSampleAeCtrl[chNo]->Agc;
    *dgain = pSampleAeCtrl[chNo]->Dgain;
    AmbaKAL_MutexGive(&pSampleAeCtrl[chNo]->Mutex);
    //AmbaPrint("<%s>  %s ",  __FUNCTION__, actTask->tx_thread_name);
    return (0);
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AeSetVideoExposureValue
 *
 *  @Description:: Set Video Exposure value
 *
 *  @Input      ::
 *     UINT32 chNo : handler for multi-task
 *     float shutter : shutter speed in second
 *     float agc : sensor gain in db
 *     UINT32 dgain : digital gain(unit gain: 4096)
 *
 *  @Output     ::
 *     none
 *
 *  @Return     ::
 *     INT : OK(0)/NG(-1)
 *
\*-----------------------------------------------------------------------------------------------*/
static int AeSetVideoExposureValue(UINT32 chNo, float shutter, float agc, UINT32 dgain)
{
    //AMBA_DSP_CHANNEL_ID_u Chan = { .Bits = { .VinID = 0, .SensorID = 0 } };
    int    Rval = 0;
    if (IpctestChno != 0xFFFFFFFF)
    chNo = 0;
    AmbaKAL_MutexTake(&pSampleAeCtrl[chNo]->Mutex, AMBA_KAL_WAIT_FOREVER);
    pSampleAeCtrl[chNo]->Shutter = shutter;
    pSampleAeCtrl[chNo]->Agc = agc;
    pSampleAeCtrl[chNo]->Dgain = dgain;

    //AmbaSensor_SetGainFactor(Chan, agc);
    //AmbaSensor_SetEshutterSpeed(Chan, shutter);
    //Test
    if (IpctestChno != 0xFFFFFFFF)
        chNo = 0;
    //
    AmbaImg_Proc_Cmd(MW_IP_SET_DGAIN, (UINT32)chNo, (UINT32)dgain, 0);
    AmbaKAL_MutexGive(&pSampleAeCtrl[chNo]->Mutex);
    return Rval;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaSample_AeInit
 *
 *  @Description:: To initialize the AE algo related settings
 *
 *  @Input      ::
 *     UINT32 chNo : handler for multi-task
 *
 *  @Output     :: none
 *
 *  @Return     :: none
 *
\*-----------------------------------------------------------------------------------------------*/
void AmbaSample_AeInit(UINT32 chNo)
{
    static UINT8 InitFlg = 0;
    UINT32 MaxChNo = 0;
    UINT32    FrameRate = 60;
    //UINT16    FlickerHz = 60;

    if (InitFlg == 0) { //FIXME
        AmbaImg_Proc_Cmd(MW_IP_GET_TOTAL_CH_COUNT, (UINT32)&MaxChNo, 0, 0);
        //AmbaSample_Ae_MemInit(MaxChNo);
        AmbaSample_Ae_MemInit(2); //Test
        InitFlg = 1;
    }
    AmbaImg_Proc_Cmd(MW_IP_GET_FRAME_RATE, (UINT32)chNo, (UINT32)&FrameRate, 0);
    pSampleAeCtrl[chNo]->MinRate = 1.001 / 2000;
    pSampleAeCtrl[chNo]->MaxRate = 1.001 / (float)FrameRate;
    pSampleAeCtrl[chNo]->MaxAgcDb = 18;
    pSampleAeCtrl[chNo]->MinAgcDb = 0;
    pSampleAeCtrl[chNo]->MinDgain = 4096;
    pSampleAeCtrl[chNo]->MaxDgain = 8192;

    AeSetVideoExposureValue(chNo, pSampleAeCtrl[chNo]->MaxRate, pSampleAeCtrl[chNo]->MinAgcDb, pSampleAeCtrl[chNo]->MinDgain);
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaSample_Compute_Ae_Tile_Value
 *
 *  @Description:: Compute Ae tile value
 *
 *  @Input      :: none
 *
 *  @Output     ::
 *     UINT16 *aeTileRow : Pointer to Ae tile row number
 *     UINT16 *aeTileCol : Pointer to Ae tile col number
 *     UINT16 *aeTilesAvg : Pointer to Ae tile average
 *
 *  @Return     ::
 *     INT : OK(0)/NG(-1)
 *
\*-----------------------------------------------------------------------------------------------*/
int AmbaSample_Compute_Ae_Tile_Value(UINT32 chNo, UINT16 *aeTileRow,UINT16 *aeTileCol,UINT16 *aeTilesAvg)
{
    int                 Rval = 0;

    long long           TilePixel;
    UINT16              TileRow;
    UINT16              TileCol;
    int                 i,j;
    UINT32              CurTileNo;
    long long           TilePixelOffset;
    long long           Tmp;
    UINT32              DGain, GlobalDGain;
    //Test
    if (IpctestChno == 0xFFFFFFFF) {
    AmbaImg_Proc_Cmd(MW_IP_GET_DGAIN, (UINT32)chNo, (UINT32)&DGain, 0);
    AmbaImg_Proc_Cmd(MW_IP_GET_GLOBAL_DGAIN, (UINT32)chNo, (UINT32)&GlobalDGain, 0);
    AmbaImg_Proc_Cmd(MW_IP_GET_CFA_3A_STAT_ADDR, (UINT32)chNo, (UINT32)&pSampleAeCtrl[chNo]->pCfaData, 0);
    } else {
         AmbaImg_Proc_Cmd(MW_IP_GET_DGAIN, 0, (UINT32)&DGain, 0);
         AmbaImg_Proc_Cmd(MW_IP_GET_GLOBAL_DGAIN, 0, (UINT32)&GlobalDGain, 0);
         AmbaImg_Proc_Cmd(MW_IP_GET_CFA_3A_STAT_ADDR, 0, (UINT32)&pSampleAeCtrl[chNo]->pCfaData, 0);
    }
    //



    TilePixel = pSampleAeCtrl[chNo]->pCfaData->Header.AeTileWidth * pSampleAeCtrl[chNo]->pCfaData->Header.AeTileHeight;
    if (TilePixel == 0) {
        return -1;
    }
    TileRow = pSampleAeCtrl[chNo]->pCfaData->Header.AeTileNumRow;
    TileCol = pSampleAeCtrl[chNo]->pCfaData->Header.AeTileNumCol;

    for (i = 0; i < TileRow; i ++) {
        for (j = 0; j < TileCol; j ++) {
            CurTileNo = i*TileCol + j;
            TilePixelOffset = pSampleAeCtrl[chNo]->pCfaData->Ae[CurTileNo].CountMin + pSampleAeCtrl[chNo]->pCfaData->Ae[CurTileNo].CountMax;
            if (TilePixelOffset > 0) {
                TilePixelOffset = TilePixel - TilePixelOffset;
            } else {
                TilePixelOffset = TilePixel;
            }
            if (TilePixelOffset > 0) {
                Tmp = ((long long)pSampleAeCtrl[chNo]->pCfaData->Ae[CurTileNo].LinY << pSampleAeCtrl[chNo]->pCfaData->Header.AeLinearYShift) / TilePixelOffset;
            } else {
                Tmp = 0;
            }
            Tmp = Tmp * DGain / WB_UNIT_GAIN;
            Tmp = Tmp * GlobalDGain / WB_UNIT_GAIN;

            if (Tmp > 16383)
                Tmp = 16383;
            aeTilesAvg[CurTileNo] = Tmp;
        }
    }

    *aeTileRow = TileRow;
    *aeTileCol = TileCol;
    return Rval;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaSample_Average
 *
 *  @Description:: Calculate average of Ae statistics
 *
 *  @Input      ::
 *     UINT32 chNo : handler for multi-task
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *     Average of AE statistics
 *
\*-----------------------------------------------------------------------------------------------*/
static UINT32 AmbaSample_Average(UINT32 chNo)
{
    UINT16 i;
    UINT16 AeTileRow;
    UINT16 AeTileCol;
    UINT16 AeTilesAvg[96];
    UINT32 AeAvg;
    UINT32 Count;

    AmbaSample_Compute_Ae_Tile_Value(chNo, &AeTileRow,&AeTileCol,AeTilesAvg);

    AeAvg = 0;
    Count = 0;
    for (i = 0; i < (AeTileRow * AeTileCol); i++) {
        AeAvg += AeTilesAvg[i];
        Count ++;
    }
    if (Count > 0) {
        return (AeAvg / Count);
    }
    return 0;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaSample_AeControl
 *
 *  @Description:: AE algo control API
 *
 *  @Input      ::
 *     UINT32 chNo : handler for multi-task
 *
 *  @Output     ::
 *    aaaVideoStatus: Pointer to Video Aaa status
 *    aaaVideoStatus: Pointer to Still Aaa status
 *
 *  @Return     :: none
 *
\*-----------------------------------------------------------------------------------------------*/
void AmbaSample_AeControl(UINT32 chNo, AMBA_3A_STATUS_s *aaaVideoStatus, AMBA_3A_STATUS_s *aaaStillStatus)
{
    static UINT8     AeSkipFrames = 0;
    UINT16           AETarget, AETargetH, AETargetL, i;
    UINT32           AeAverage;
    float            AeShutter;
    float            AeAgc;
    UINT32           AeDgain;

    if (AeSkipFrames > 0) {
        AeSkipFrames--;
        return;
    }


    if (chNo != 0) {
        AETarget = 1000;
    } else {
        AETarget = 2600;
    }
    AETargetH = AETarget * 11 / 10;
    AETargetL = AETarget * 9 / 10;

    AeGetVideoExposureValue(chNo, &AeShutter, &AeAgc, &AeDgain);
    //AmbaPrint("--- Pre AE : %f %f %d ---", AeShutter, AeAgc, AeDgain);
    AeAverage = AmbaSample_Average(chNo);
    //AmbaPrint("--- AeAverage : %d ---", AeAverage);

    aaaVideoStatus->Ae = AMBA_PROCESSING;
    if (AeAverage > AETargetH) {

        if (AeDgain > pSampleAeCtrl[chNo]->MinDgain) {
            AeDgain -= 32;
        } else if (AeAgc > pSampleAeCtrl[chNo]->MinAgcDb) {
            AeAgc -= 0.046;
        } else if (AeShutter > pSampleAeCtrl[chNo]->MinRate) {
            AeShutter -= 0.0000625;
        }
    } else if (AeAverage < AETargetL) {

        if (AeShutter < pSampleAeCtrl[chNo]->MaxRate) {
            AeShutter += 0.0000625;
        } else if (AeAgc < pSampleAeCtrl[chNo]->MaxAgcDb) {
            AeAgc += 0.046;
        } else if (AeDgain < pSampleAeCtrl[chNo]->MaxDgain) {
           AeDgain += 32;
        }
    } else {
        aaaVideoStatus->Ae = AMBA_LOCK;
        return;
    }

    if (AeShutter < pSampleAeCtrl[chNo]->MinRate) {
        AeShutter = pSampleAeCtrl[chNo]->MinRate;
    } else if (AeShutter > pSampleAeCtrl[chNo]->MaxRate) {
        AeShutter = pSampleAeCtrl[chNo]->MaxRate;
    }

    if (AeAgc < pSampleAeCtrl[chNo]->MinAgcDb) {
        AeAgc = pSampleAeCtrl[chNo]->MinAgcDb;
    } else if (AeAgc > pSampleAeCtrl[chNo]->MaxAgcDb) {
        AeAgc = pSampleAeCtrl[chNo]->MaxAgcDb;
    }

    if (AeDgain < pSampleAeCtrl[chNo]->MinDgain) {
        AeDgain = pSampleAeCtrl[chNo]->MinDgain;
    } else if (AeDgain > pSampleAeCtrl[chNo]->MaxDgain) {
        AeDgain = pSampleAeCtrl[chNo]->MaxDgain;
    }
    //AmbaPrint("---chNo : %d, AETarget : %4d, next AE : %f %f %d ---", chNo, AETarget, AeShutter, AeAgc, AeDgain);
    memset(&pSampleAeCtrl[chNo]->VideoInfo, 0, sizeof(pSampleAeCtrl[chNo]->VideoInfo));
    memset(pSampleAeCtrl[chNo]->StillInfo, 0, sizeof(pSampleAeCtrl[chNo]->StillInfo));

    pSampleAeCtrl[chNo]->VideoInfo.ShutterTime = AeShutter;
    pSampleAeCtrl[chNo]->VideoInfo.AgcGain = AeAgc;
    pSampleAeCtrl[chNo]->VideoInfo.Dgain = AeDgain;
    pSampleAeCtrl[chNo]->VideoInfo.Flash = 0;
    pSampleAeCtrl[chNo]->VideoInfo.IrisIndex = 0;

    for (i = 0; i<MAX_AEB_NUM; i++) {
        pSampleAeCtrl[chNo]->StillInfo[i] = pSampleAeCtrl[chNo]->VideoInfo;
    }

    AmbaImg_Proc_Cmd(MW_IP_SET_AE_INFO, (UINT32)chNo, IP_MODE_VIDEO, (UINT32)&pSampleAeCtrl[chNo]->VideoInfo);
    AmbaImg_Proc_Cmd(MW_IP_SET_AE_INFO, (UINT32)chNo, IP_MODE_STILL, (UINT32)pSampleAeCtrl[chNo]->StillInfo);

    AeSetVideoExposureValue(chNo, AeShutter, AeAgc, AeDgain);
}

