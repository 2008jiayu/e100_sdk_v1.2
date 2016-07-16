/**
 * @file src/app/connected/applib/src/applib/monitor/ApplibMonitor_Bitrate.c
 *
 * Implementation of bitrate control monitor
 *
 * History:
 *    2014/05/29 - [Chester Chuang] created file
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
#include <applib.h>

//#define DEBUG_APPLIB_MONITOR_BITRATE
#if defined(DEBUG_APPLIB_MONITOR_BITRATE)
#define DBGMSG AmbaPrint
#define DBGMSGc(x) AmbaPrintColor(GREEN,x)
#define DBGMSGc2 AmbaPrintColor
#else
#define DBGMSG(...)
#define DBGMSGc(...)
#define DBGMSGc2(...)
#endif

typedef struct _Bitrate_monitor_param_s_ {
    UINT8 Mode;
    UINT8 StreamId;
    UINT8 Reserved[2];
    UINT32 AverBitRate;
    UINT32 MaxBitRate;
    UINT32 MinBitRate;
} Bitrate_monitor_param_s;

static Bitrate_monitor_config_s BrcInfo[3];
static UINT32 FrameFactor[3];
static UINT32 FrameAverageBitRate[3]; // store last average bitrate when frame rate is changed due to bandwidth shortage
static UINT32 FrameRoofBitRate[3];
static AMP_AVENC_HDLR_s* VideoEncodeHdlr[3];

static UINT32 BitRateMonitorPrevTime[3];
static UINT8 BitRateMonitorSkip = 1;

static APP_BRC_AQP_CB_s *AppLibMonitor_BitRateAQP_cb = {NULL};
static APP_BRC_FORCE_IDR_CB_s *AppLibMonitor_Force_IDR_cb = {NULL};

/**
 *  Get smoothed target bit rate
 *
 *  @param [averBitRate] average bit rate
 *  @param [targetBitRate] target bit rate
 *
 *  @return 0 - OK.
 */
static int AppLibMonitorBitRate_GetSmoothTargetBitRate(UINT32 averBitRate, UINT32 *targetBitRate)
{
    UINT32 UnitBitRate, SmoothFactor;

    UnitBitRate = (UINT32) 5*(averBitRate/100);
    SmoothFactor = (*targetBitRate)/UnitBitRate;

    //AmbaPrint("Unit(%d) factor(%d) old(%d) new(%d)", UnitBitRate, SmoothFactor, *targetBitRate, SmoothFactor*UnitBitRate);

    *targetBitRate = (SmoothFactor)?(SmoothFactor*UnitBitRate):(UnitBitRate);


    return 0;
}

/**
 *  Run time change frame rate if needed
 *
 *  @param [index] stream index
 *  @param [currBitRate] current target bit rate
 *  @param [targetBitRate] new target bit rate
 *  @param [maxFactor] max factor
 *
 *  @return 1 - bit rate has changed. 0 - bit rate remains the same
 */
static int AppLibMonitorBitRate_RunTimeBitRateControl(UINT8 index, UINT32 averBitRate, UINT32 currBitRate, UINT32 targetBitRate, UINT32 maxFactor)
{
    int rval = 0;

    AMP_VIDEOENC_RUNTIME_QUALITY_CFG_s NewConfig;

    DBGMSG("[Applib - Bitrate Monitor] <Bit rate control> TargetBrate(%d) CurrBrate(%d)", targetBitRate, currBitRate);
    // Decide whether to change bit rate
    if (targetBitRate && (targetBitRate != currBitRate)) {
        if (FrameRoofBitRate[index] && FrameRoofBitRate[index]<targetBitRate) {
            NewConfig.BitRate = (UINT32) (((UINT64)FrameRoofBitRate[index]<<6)/maxFactor)>>6;
        } else {
            NewConfig.BitRate = targetBitRate;
        }

        AppLibMonitorBitRate_GetSmoothTargetBitRate(averBitRate, &NewConfig.BitRate);
        NewConfig.Cmd = RC_BITRATE;
        AmpVideoEnc_SetRuntimeQuality(VideoEncodeHdlr[index], &NewConfig);
        FrameRoofBitRate[index] = NewConfig.BitRate;

        AmbaPrint("[Applib - Bitrate Monitor] <Control> current %d target %d", currBitRate, NewConfig.BitRate);
        rval = 1;
    }

    return rval;
}

/**
 *  Run time change frame rate according to bandwidth if needed
 *
 *  @param [index] stream index
 *  @param [currBitRate] current target bit rate
 *  @param [targetBitRate] new target bit rate
 *  @parem [info] current bit rate info
 *  @param [maxFactor] max factor
 *
 *  @return 1 - bit rate has changed. 0 - bit rate remains the same
 */
static int AppLibMonitorBitrate_RunTimeBandWidthControl(UINT8 index, UINT32 currBitRate, UINT32 targetBitRate, Bitrate_monitor_param_s *info, UINT32 maxFactor)
{
    int rval = 0;
    AMP_VIDEOENC_RUNTIME_QUALITY_CFG_s NewConfig;

    if (targetBitRate && (targetBitRate != info->AverBitRate)) {
        if (BrcInfo[index].EnablaFrateChg) { // Frame rate change is enable, change it!
            UINT8 TmpFrameFactor;
            AMP_VIDEOENC_RUNTIME_FRAMERATE_CFG_s FrameRateCfg;
            AmbaPrint("[Applib - Bitrate Monitor] <Bandwidth> BW(%d) AvgBrate(%d) CurrBrate(%d)", targetBitRate, info->AverBitRate, currBitRate);

            if (targetBitRate < info->AverBitRate) {
                /* AverBitrate is larger than BandWidth target, should lower frame rate*/
                TmpFrameFactor = (UINT8) currBitRate/targetBitRate;

                if (TmpFrameFactor >= BrcInfo[index].FrateDownFactorThres) {
                    TmpFrameFactor *= FrameFactor[index];
                    if (TmpFrameFactor <= BrcInfo[index].FrateDownFactorMax) {
                        FrameFactor[index] = TmpFrameFactor;
                    }
                }
            } else {
                /* AverBitrate is smaller than BandWidth target, should increase frame rate*/
                TmpFrameFactor = (UINT8) targetBitRate/currBitRate;
                if (TmpFrameFactor >= BrcInfo[index].FrateUpFactorThres) {
                    TmpFrameFactor = FrameFactor[index]/TmpFrameFactor;
                    FrameFactor[index] = TmpFrameFactor;
                }

            }

        // In A9, we set down factor(divisor) to SSP
        // FIXME: Do we need AMP_recorder_cmd(MW_REC_SET_AUDIO_MUXER_MSG_FRAME_THRESH,0,1); ??
            FrameRateCfg.Hdlr = VideoEncodeHdlr[index];
            FrameRateCfg.Divisor = FrameFactor[index];
            AmpVideoEnc_SetRuntimeFrameRate(1, &FrameRateCfg);
            NewConfig.BitRate = targetBitRate * FrameFactor[index];
            NewConfig.Cmd = RC_BITRATE;
            FrameAverageBitRate[index] = NewConfig.BitRate;
            FrameRoofBitRate[index] = NewConfig.BitRate*((int)((info->MaxBitRate*1.0/info->AverBitRate)*(1<<6)) & 0x1FF);
            AmpVideoEnc_SetRuntimeQuality(VideoEncodeHdlr[index], &NewConfig);
            rval = 1;

        } else { // Frame rate change is disable, change bit rate instead
            UINT8 ChangeRate;
            UINT32 AverBitRateTarget = (UINT32) (((UINT64)targetBitRate<<6)/maxFactor)>>6;

            if (targetBitRate > info->MaxBitRate) {
                if (0 == FrameAverageBitRate[index]) {
                    // need not changed, keep going
                } else if (FrameAverageBitRate[index]) {
                    AverBitRateTarget = info->AverBitRate;
                    FrameRoofBitRate[index] = targetBitRate;
                    ChangeRate = 1;
                }
            } else if (targetBitRate < info->MaxBitRate) {
                // should change to low bitrate
                if (0 == FrameRoofBitRate[index]) {
                    FrameRoofBitRate[index] = targetBitRate;
                    ChangeRate = 1;
                } else if (FrameAverageBitRate[index] && \
                        (FrameAverageBitRate[index] < AverBitRateTarget || FrameRoofBitRate[index] > AverBitRateTarget)) {
                    FrameRoofBitRate[index] = targetBitRate;
                    ChangeRate = 1;
                }
            }

            if (ChangeRate) {
                FrameAverageBitRate[index] = NewConfig.BitRate = AverBitRateTarget;
                NewConfig.Cmd = RC_BITRATE;
                AmpVideoEnc_SetRuntimeQuality(VideoEncodeHdlr[index], &NewConfig);
                rval = 1;
            }

        }
    }

    return rval;
}

/**
 *  register AQP callback,
 *
 *  @param [aqpCb] function pointer of AQP callback
 *
 *  @return 0 - success
 */
int AppLibRegisterAQPCallBack(APP_BRC_AQP_CB_s *aqpCb)
{
    AppLibMonitor_BitRateAQP_cb = aqpCb;
    return 0;
}

/**
 *  register Force Idr callback,
 *
 *  @param [forceIdrCb] function pointer of Force Idr callback
 *
 *  @return 0 - success
 */
int AppLibRegisterForceIdrCallBack(APP_BRC_FORCE_IDR_CB_s *foceIdrCb)
{
    AppLibMonitor_Force_IDR_cb = foceIdrCb;
    return 0;
}

/**
 *  @brief Config bit rate monitor
 *
 *  Config bit rate monitor
 *
 *  @return >=0 success, <0 failure
 */
int AppLibMonitorBitrate_Config(UINT8 mode, Bitrate_monitor_config_s *config)
{
    UINT8 i;

    // FIXME: how to discriminate stream ID
    if (0 == config->StreamId) { // Primary
        i = 0;
    } else if (1 == config->StreamId) { // Secondary
        i = 1;
    } else if (2 == config->StreamId) {
        i = 2;
    } else {
        AmbaPrintColor(RED,"[Applib - Bitrate Monitor] <config> incorrect stream ID (%d)", config->StreamId);
        return -1;
    }

    BrcInfo[i].Enable = config->Enable;
    BrcInfo[i].StreamId = config->StreamId;

    if (mode & APPLIB_BRCMON_CONFIG_BANDWIDTH) {
        BrcInfo[i].EnablaFrateChg = config->EnablaFrateChg;
        BrcInfo[i].EnableBWChk = config->EnableBWChk;
        BrcInfo[i].FrateUpFactorThres = config->FrateUpFactorThres;
        BrcInfo[i].FrateDownFactorThres = config->FrateDownFactorThres;
        BrcInfo[i].FrateDownFactorMax = config->FrateDownFactorMax;
        BrcInfo[i].BandwidthCb = config->BandwidthCb;
    }
    if (mode & APPLIB_BRCMON_CONFIG_DZOOM) {
        BrcInfo[i].EnableDzoomChk = config->EnableDzoomChk;
        BrcInfo[i].DzoomFactorThres = config->DzoomFactorThres;

        if (NULL == config->DzoomCb) {
            extern  __attribute__((weak)) void AmbaBitRateControl_DzoomHandler(UINT32 *targetBitRate, UINT32 currBitRate, UINT8 streamId);
            //BITRATE_CONTROL_DZOOM_HANDLER_s DzoomHdlr = {0};
            //DzoomHdlr.HandlerCB = AmbaBitRateControl_DzoomHandler;
            //DzoomHdlr.DzoomFactorThres = config->DzoomFactorThres;
            //DzoomHdlr.DebugPattern = config->Debug;
            //FIXME: Hook dzoom relate CB
            BrcInfo[i].DzoomCb = AmbaBitRateControl_DzoomHandler;
        } else {
            BrcInfo[i].DzoomCb = config->DzoomCb;
        }
    }
    if (mode & APPLIB_BRCMON_CONFIG_LUMA) {
        BrcInfo[i].EnableLumaChk = config->EnableLumaChk;
        BrcInfo[i].LumaThres = config->LumaThres;
        BrcInfo[i].LumaLowThres = config->LumaLowThres;

        if (NULL == config->LumaCb) {
            extern  __attribute__((weak)) void AmbaBitRateControl_LumaHandler(UINT32 *targetBitRate, UINT32 currBitRate, UINT8 streamId);
            //BITRATE_CONTROL_LUMA_HANDLER_s LumaHdlr = {0};
            //LumaHdlr.HandlerCB = AmbaBitRateControl_LumaHandler;
            //LumaHdlr.LumaThres = config->LumaThres;
            //LumaHdlr.LowLumaThres = config->LumaLowThres;
            //LumaHdlr.DebugPattern = config->Debug;
            //FIXME: Hook luma relate CB
            BrcInfo[i].LumaCb = AmbaBitRateControl_LumaHandler;
        } else {
            BrcInfo[i].LumaCb = config->LumaCb;
        }
    }

    if (mode & APPLIB_BRCMON_CONFIG_CUSTOM) {
        extern  __attribute__((weak)) void AmbaBitRateControl_ComplexityHandler(UINT32 *targetBitRate, UINT32 currBitRate, UINT8 streamId);
        BrcInfo[i].EnableCustomChk = config->EnableCustomChk;
        BrcInfo[i].CustomCb = config->CustomCb;
        if (config->CustomCb == AmbaBitRateControl_ComplexityHandler) {
            //BITRATE_CONTROL_COMPLEXITY_HANDLER_s CplxHdlr = {0};
            //CplxHdlr.HandlerCB = config->CustomCb;
            //CplxHdlr.GetDayLumaThresCB = config->SceneGetDayLumaThresCb;
            //CplxHdlr.GetComplexityRangeCB = config->SceneGetRangeCb;
            //CplxHdlr.GetPipeModeCB = config->SceneGetPipeModeCb;
            //CplxHdlr.DebugPattern = config->Debug;
            //FIXME: Hook cplx relate CB
            BrcInfo[i].CustomCb = AmbaBitRateControl_ComplexityHandler;
        } else {
            BrcInfo[i].CustomCb = config->CustomCb;
        }
    }

    return 0;
}

/**
 *  @brief Bit rate monitor initialization callback
 *
 *  Bit rate monitor initialization callback
 */
void AppLibMonitorBitrate_InitCB(void)
{
    UINT8 i = 0;
    UINT32 initTime;

    /* Init bit rate monitor settings*/
    for (i=0;i<3;i++) {
        FrameFactor[i] = 1;
        FrameAverageBitRate[i] = 0;
        FrameRoofBitRate[i] = 0;
    }

    /* Let default skip = 1*/
    BitRateMonitorSkip = 1;

    /* Record current time */
    initTime = AmbaSysTimer_GetTickCount();
    BitRateMonitorPrevTime[0] = BitRateMonitorPrevTime[1] = BitRateMonitorPrevTime[2] = initTime;

    /* Get video encode handler */
    {   //Per Martin, this should be a workaround to get video encode handler
        extern AMP_AVENC_HDLR_s *VideoEncPri;
        extern AMP_AVENC_HDLR_s *VideoEncSec;
        VideoEncodeHdlr[0] = VideoEncPri;
        VideoEncodeHdlr[1] = VideoEncSec;
        VideoEncodeHdlr[2] = NULL;
    }

    AmbaPrint("[Applib - Bitrate Monitor] <Init> Bitrate monitor init, skip %d time %d", BitRateMonitorSkip, initTime);
}

/**
 *  @brief Bit rate monitor time up callback
 *
 *  Bit rate monitor time up callback
 */
void AppLibMonitorBitrate_TimeUpCB(void)
{
    UINT8 i;
    UINT32 period, currBitRate;
    int isModify = 0;
    Bitrate_monitor_param_s bitRate;
    AMP_VIDEOENC_BITSTREAM_CFG_s currConfig;
    AMP_AVENC_HDLR_s *videoEncHdlr;

    if (AppLibMonitor_BitRateAQP_cb) AppLibMonitor_BitRateAQP_cb->BitRate_AQP_cb();
    if (AppLibMonitor_Force_IDR_cb) AppLibMonitor_Force_IDR_cb->BitRate_Force_IDR_cb();

    if (0 == BitRateMonitorSkip) {
        for (i=0;i<3;i++) {
            if (BrcInfo[i].Enable) {
                UINT8 isCBR = 0;
                UINT32 maxFactor;
                UINT32 dzoomTarget = 0 , lumaTarget = 0, bandwidthTarget = 0, customTarget = 0;
                AMP_VIDEOENC_ENCODING_INFO_s encInfo;

                videoEncHdlr = VideoEncodeHdlr[i];

                period = SYSTIMEDIFF(BitRateMonitorPrevTime[i], AmbaSysTimer_GetTickCount());
                if (0 == period) goto _DONE;

            /* Step 1. Get current encode info from video codec*/
                AmpVideoEnc_GetEncodingInfo(videoEncHdlr,&encInfo);
                DBGMSG("[Applib - Bitrate Monitor] <TimeUp> Total frames(%d)  Average bitrate(%d) TotalBytes(%lld)", i, encInfo.TotalFrames, encInfo.AverageBitrate, encInfo.TotalBytes);

                currBitRate = encInfo.AverageBitrate*1000;
                if (0 == currBitRate) goto _DONE;

                AmpVideoEnc_GetBitstreamConfig(videoEncHdlr, &currConfig);

                // FIXME: we should care simple_rc only
                if (AMP_VIDEOENC_CODER_AVCC == currConfig.StreamSpec) {
                    bitRate.StreamId = BrcInfo[i].StreamId;
                    bitRate.AverBitRate = currConfig.Spec.H264Cfg.BitRateControl.AverageBitrate;
                    bitRate.Mode = currConfig.Spec.H264Cfg.BitRateControl.BrcMode;
                    if (VIDEOENC_SMART_VBR == currConfig.Spec.H264Cfg.BitRateControl.BrcMode) {
                        bitRate.MaxBitRate = currConfig.Spec.H264Cfg.BitRateControl.MaxBitrate;
                        bitRate.MinBitRate = currConfig.Spec.H264Cfg.BitRateControl.MinBitrate;
                    }
                } else if (AMP_VIDEOENC_CODER_MJPEG == currConfig.StreamSpec) {
                    DBGMSGc("[Applib - Bitrate Monitor] <TimeUp> MJPEG has no bitrate control,,,");
                    goto _DONE;
                }

                DBGMSG("[Applib - Bitrate Monitor] <TimeUp> Target bit rate(max:%d min:%d aver:%d) mode(%d)", \
                    bitrate.MaxBitRate, bitrate.MinBitRate, bitrate.AverBitRate, bitRate.Mode);

                if (VIDEOENC_CBR == bitRate.Mode) isCBR = 1;

                maxFactor = (UINT32) ((bitRate.MaxBitRate*1.0/bitRate.AverBitRate)/(1<<6))&0x1FF;
                if (0 == maxFactor) {
                    AmbaPrint("[Applib - Bitrate Monitor] <TimeUp> WARNING for bitrate info[%d] MaxBrate(%d) MinBrate(%d) AverBrate(%d)", \
                        i, bitRate.MaxBitRate, bitRate.MinBitRate, bitRate.AverBitRate);
                    goto _DONE;
                }

            /*  Step 2. Check callback usage and invoke it to get target bit rate  */
                if (BrcInfo[i].EnableDzoomChk && BrcInfo[i].DzoomCb)
                    BrcInfo[i].DzoomCb(&dzoomTarget, currBitRate, BrcInfo[i].StreamId);
                if (BrcInfo[i].EnableLumaChk && BrcInfo[i].LumaCb)
                    BrcInfo[i].LumaCb(&lumaTarget, currBitRate, BrcInfo[i].StreamId);
                if (BrcInfo[i].EnableBWChk && BrcInfo[i].BandwidthCb)
                    BrcInfo[i].BandwidthCb(&bandwidthTarget, currBitRate, BrcInfo[i].StreamId);
                if (BrcInfo[i].EnableCustomChk && BrcInfo[i].CustomCb)
                    BrcInfo[i].CustomCb(&customTarget, currBitRate, BrcInfo[i].StreamId);


           /*  Step 3. Compare target bit rate and current bit rate to decide whether to run time change bit rate  */
                isModify = AppLibMonitorBitrate_RunTimeBandWidthControl(i, currBitRate, bandwidthTarget, &bitRate, maxFactor);
                if (isModify) goto _DONE;

                if (isCBR) goto _DONE;

                /* Custom */
                isModify = AppLibMonitorBitRate_RunTimeBitRateControl(i, bitRate.AverBitRate, currBitRate, customTarget, maxFactor);
                if (isModify) goto _DONE;

                /* Dzoom */
                isModify = AppLibMonitorBitRate_RunTimeBitRateControl(i, bitRate.AverBitRate, currBitRate, dzoomTarget, maxFactor);
                if (isModify) goto _DONE;

                /* Luma */
                isModify = AppLibMonitorBitRate_RunTimeBitRateControl(i, bitRate.AverBitRate, currBitRate, lumaTarget, maxFactor);
                if (isModify) goto _DONE;

            }
       _DONE:
            if (isModify) {
                BitRateMonitorPrevTime[i] = AmbaSysTimer_GetTickCount();
                AmpVideoEnc_ResetEncodingInfo(VideoEncodeHdlr[i]); // reset encoding info to get new average bitrate
                AmbaPrint("[Applib - Bitrate Monitor] <TimeUp> bit rate is changed");
            } else {
                DBGMSGc("[Applib - Bitrate Monitor] <TimeUp> spec %d period %dms currBrate %d",\
                        currConfig.StreamSpec, period, currBitRate);
            }
        }
    } else {
        BitRateMonitorSkip--;
        DBGMSGc("[Applib - Bitrate Monitor] <TimeUp> skip(%d)", BitRateMonitorSkip);
    }
    return;
}
