/**
 * @file src/app/connected/applib/src/calibration/blc/ApplibCalibBlc.c
 *
 * sample code for black level calibration
 *
 * History:
 *    07/10/2013  Allen Chiu Created
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
#include <calibration/blc/ApplibCalibBlc.h>
#include <AmbaDSP.h>
#include <AmbaSensor.h>
#include <AmbaPrintk.h>
#include <common/common.h>
#include <AmbaFS.h>
#include <AmbaUtility.h>
#include <AmbaKAL.h>
#include <AmbaCalibInfo.h>

#define ABS(a)      (((a) < 0) ? -(a) : (a))

/**
 *  @brief initial function for black level calibration
 *
 *  initial function for black level calibration
 *
 *  @param [in] CalObj calibration object
 *
 *  @return 0 success, -1 failure
 */
int AppLibCalibBLC_Init(Cal_Obj_s *CalObj)
{
    Cal_Blc_Info_s CalBLCInfo[CALIB_CH_NO];
    char OutputStr[128];
    UINT8 Channel;

    memcpy(&CalBLCInfo[0], CalObj->DramShadow, (sizeof(Cal_Blc_Info_s)*CALIB_CH_NO));
    for (Channel = 0; Channel < CALIB_CH_NO; Channel ++) {
        if (CalBLCInfo[Channel].BlCalResult == BL_CAL_OK) {
            CalBLCInfo[Channel].BlMode = BLC_MODE_APPLY; // apply
            AmpCalib_BLCFunc(&CalBLCInfo[Channel],0, OutputStr);
    } else {
            CalBLCInfo[Channel].BlMode = BLC_MODE_RESET; // reset
            AmpCalib_BLCFunc(&CalBLCInfo[Channel],0,OutputStr);
        }
    }
    return 0;
}

/**
 *  @brief print the error message for black level calibration
 *
 *  print the error message for black level calibration
 *
 *  @param [in] CalSite calibration site status
 *  @param [out] OutputStr debug message for this function
 *
 *  @return 0 success, -1 failure
 */
int AppLibCalibBLC_PrintError(char *OutputStr, Cal_Stie_Status_s *CalSite)
{
    AmbaPrint("************************************************************");
    AmbaPrintColor(RED,"%s",OutputStr);
    AmbaPrint("************************************************************");
    CalSite->Status = CAL_SITE_RESET;

    return 0;
}
/**
 *  @brief the entry function for black level calibration
 *
 *  the entry function for black level calibration
 *
 *  @param [in] Argc number of input parameters
 *  @param [in] Argv value of input parameters
 *  @param [in] CalSite calibration site status
 *  @param [in] CalObj calibration object
 *  @param [out] OutputStr debug message for this function
 *
 *  @return 0 success, -1 failure
 */
int AppLibCalibBLC_Func(int Argc, char *Argv[], char *OutputStr, Cal_Stie_Status_s *CalSite, Cal_Obj_s *CalObj)
{
    Cal_Blc_Info_s CalBLCInfo[CALIB_CH_NO];
    UINT8 DbValue;
    AMBA_SENSOR_MODE_ID_u SensorMode = { .Bits = {   .Mode = 0,  .VerticalFlip = 0,  } };

    UINT8 ParamChk = 0;
    INT16 BLCThreshold[CALIB_CH_NO];
    AMBA_DSP_IMG_BLACK_CORRECTION_s BLCVar = { 0 };

    INT16 BlcVarTmp = 0;

    AMBA_DSP_IMG_VIN_SENSOR_GEOMETRY_s Geometry= {0};
    AMBA_SENSOR_MODE_INFO_s VinInfo;
    int Rval;
    UINT32 RawBufferAddress;
    //UINT8 Flag = 0x1;
    UINT32 RawPitch;
    int Channel;
    AMBA_AE_INFO_s StillAeInfo[MAX_AEB_NUM], CurStillAeInfo[MAX_AEB_NUM];
    AMBA_AE_INFO_s  VideoAeInfo = {0},  CurVideoAeInfo = {0};
    AMBA_3A_OP_INFO_s NoAAAFunc = {DISABLE, DISABLE, DISABLE, DISABLE}, CurAAAFunc = {0};
    UINT8 ChNo = 0;

    memcpy(&CalBLCInfo[0], CalObj->DramShadow, (sizeof(Cal_Blc_Info_s)*CALIB_CH_NO));
    Channel =(int) atoi(Argv[1]);    
    memset(&CalBLCInfo[Channel], 0, sizeof(Cal_Blc_Info_s));
    BLCThreshold[Channel] = 0;
    if ((Channel < 0) || (Channel >= CALIB_CH_NO)) {
        ParamChk = 0;
        AmbaPrint("Channel number out of range, Channel = %d and it should be between 0 ~ %d",Channel, (CALIB_CH_NO-1));
        sprintf(OutputStr,"Channel number out of range, Channel = %d and it should be between 0 ~ %d",Channel, (CALIB_CH_NO-1));
    }
    //bug, need to change VIN here. add it in the future........
        
    if (Argc >= 3) {
        CalBLCInfo[Channel].BlMode = (UINT8)(atoi(Argv[2]));
        if (CalBLCInfo[Channel].BlMode == BLC_MODE_TEST) {
            ParamChk = 1;
        } else {
            CalBLCInfo[Channel].BlMode = BLC_MODE_CALIBRATION;
            if (Argc >= 8) {
                ParamChk = 1;
                CalBLCInfo[Channel].BlStd.BlackR = (INT16)(atoi(Argv[3]));
                CalBLCInfo[Channel].BlStd.BlackGr = (INT16)(atoi(Argv[4]));
                CalBLCInfo[Channel].BlStd.BlackGb = (INT16)(atoi(Argv[5]));
                CalBLCInfo[Channel].BlStd.BlackB = (INT16)(atoi(Argv[6]));
                BLCThreshold[Channel] = (INT16)(atoi(Argv[7]));
            } else {
                ParamChk = 0;
                AmbaPrint("Script should be: BLC channel mode BlackR BlackGr BlackGb BlackGb Threshold");                
                sprintf(OutputStr,"BLC Calibration fail: BLC Parameter Wrong!");
            }
        }
    } else {
        ParamChk = 0;
    }
    if (ParamChk == 0) {
        sprintf(OutputStr,"BLC Calibration fail: BLC Parameter Wrong!");
        AppLibCalibBLC_PrintError(OutputStr,CalSite);
        return BL_CALIB_ERROR_PARAMETER;
    } else {        
        AmbaPrint("*****BLC Calibration Info*****");        
        AmbaPrint("Channel No = %d",Channel);        
        AmbaPrint("BLC mode = %d",CalBLCInfo[Channel].BlMode);        
        AmbaPrint("Reference black R = %d",CalBLCInfo[Channel].BlStd.BlackR);        
        AmbaPrint("Reference black GR = %d",CalBLCInfo[Channel].BlStd.BlackGr);        
        AmbaPrint("Reference black GB = %d",CalBLCInfo[Channel].BlStd.BlackGb);        
        AmbaPrint("Reference black B = %d",CalBLCInfo[Channel].BlStd.BlackB);        
        AmbaPrint("Threshold = %d",BLCThreshold[Channel]);        
        AmbaPrint("******************************");            
    }


    /* allocate raw buffer address */
    SensorMode.Bits.Mode = pAmbaCalibInfoObj[0/*AMBA_VIN_CHANNEL0*/]->AmbaVignetteMode;
    SensorMode.Bits.VerticalFlip = 0;
    AmbaSensor_GetModeInfo(AppEncChannel, SensorMode, &VinInfo);
    CalBLCInfo[Channel].Bayer = VinInfo.OutputInfo.CfaPattern;

    Geometry.Width = VinInfo.OutputInfo.RecordingPixels.Width;
    Geometry.Height = VinInfo.OutputInfo.RecordingPixels.Height;
    RawPitch = ALIGN_32(Geometry.Width * 2);
//backup current info
    AmbaImg_Proc_Cmd(MW_IP_GET_AAA_OP_INFO, (UINT32)ChNo, (UINT32)&CurAAAFunc, 0);
    AmbaImg_Proc_Cmd(MW_IP_GET_AE_INFO, (UINT32)ChNo, IP_MODE_STILL,  (UINT32)CurStillAeInfo);
    AmbaImg_Proc_Cmd(MW_IP_GET_AE_INFO, (UINT32)ChNo, IP_MODE_VIDEO,  (UINT32)&CurVideoAeInfo);

//Stillmode
    for (DbValue = 0; DbValue < 7 ; ++DbValue) {
        AmbaKAL_TaskSleep(100);
        AmbaImg_Proc_Cmd(MW_IP_SET_AAA_OP_INFO, (UINT32)ChNo, (UINT32)&NoAAAFunc, 0);

        AmbaImg_Proc_Cmd(MW_IP_GET_AE_INFO, (UINT32)ChNo, IP_MODE_STILL,  (UINT32)StillAeInfo);
        for (int i=0; i<MAX_AEB_NUM; ++i) {
            StillAeInfo[i].ShutterTime = (float)1/30;
            StillAeInfo[i].AgcGain      = (float)pow(2.0, (double)DbValue);
            StillAeInfo[i].Dgain         = WB_UNIT_GAIN;
            StillAeInfo[i].Flash          = 0;
            StillAeInfo[i].IrisIndex    = 0;
        }
        AmbaImg_Proc_Cmd(MW_IP_SET_AE_INFO, (UINT32)ChNo, IP_MODE_STILL,  (UINT32)StillAeInfo);
        AmbaPrintColor(RED, "BLC: ShutterTime: %f AgcGain: %f Dgain: %d", StillAeInfo[0].ShutterTime, StillAeInfo[0].AgcGain, StillAeInfo[0].Dgain);
        //AppLib_CalibSetExposureValue(AeInfo.ShutterTime, AeInfo.AgcGain, AeInfo.Dgain);
        AmbaKAL_TaskSleep(100);

        CalBLCInfo[Channel].SrcRawWidth = VinInfo.OutputInfo.RecordingPixels.Width;
        CalBLCInfo[Channel].SrcRawHeight = VinInfo.OutputInfo.RecordingPixels.Height;
        //raw capture
        AmbaPrint("start to do raw capture");
        RawPitch = ALIGN_32(VinInfo.OutputInfo.RecordingPixels.Width * 2);
        //ItunerRawCapCtrl.SensorMode.Bits.Mode = VinInfo.Mode.Bits.Mode;
        //ItunerRawCapCtrl.SensorMode.Bits.VerticalFlip = VinInfo.Mode.Bits.VerticalFlip;
        //ItunerRawCapCtrl.RawBuff.Raw.RawAddr = (UINT8 *)RawBufferAddress;
        //ItunerRawCapCtrl.RawBuff.Raw.RawPitch = RawPitch; // Basic DMA transfer unit is 32-byte, which is 16-pixels, so pad to 16pixels boundary here;
        //ItunerRawCapCtrl.RawBuff.Raw.RawWidth = VinInfo.OutputInfo.RecordingPixels.Width;
        //ItunerRawCapCtrl.RawBuff.Raw.RawHeight = VinInfo.OutputInfo.RecordingPixels.Height;
        //Rval = AmpUT_ItunerRawCapture(Flag, ItunerRawCapCtrl);

        AppLibStillEnc_RawCaptureSetSensorMode(1,pAmbaCalibInfoObj[0/*AMBA_VIN_CHANNEL0*/]->AmbaVignetteMode);
        AppLibStillEnc_CaptureRaw(&RawBufferAddress);
        AmbaKAL_TaskSleep(500);

        AmbaPrint("end of raw capture");
        AmbaKAL_TaskSleep(500);

        CalBLCInfo[Channel].SrcRawAddr = RawBufferAddress;
        Rval = AmpCalib_BLCFunc(&CalBLCInfo[Channel],&CalBLCInfo[Channel].BlStill[DbValue],OutputStr);
        {
            char Fn[32];
            AMBA_FS_FILE *Fp = NULL;

            sprintf(Fn, "c:\\blccal_still_%d.raw", DbValue);
			Fn[0] = AppLib_CalibGetDriverLetter();
            AmbaPrint("AppLibCalibBlc_SaveRawImage");
            //raw ready, dump it

            Fp = AmbaFS_fopen(Fn,"wb");
            AmbaPrint("[AmpVig_UT]Dump Raw 0x%X width: %d height: %d  ", \
                      CalBLCInfo[Channel].SrcRawAddr, \
                      VinInfo.OutputInfo.RecordingPixels.Width, \
                      VinInfo.OutputInfo.RecordingPixels.Height);
            AmbaFS_fwrite((UINT16 *)RawBufferAddress, RawPitch*VinInfo.OutputInfo.RecordingPixels.Height, 1, Fp);
            AmbaFS_fclose(Fp);
        }
        if (Rval < BL_CALIB_OK) {
            AppLibCalibBLC_PrintError(OutputStr,CalSite);
            return Rval;
        }
    }

//Videomode
    {
        AMBA_SENSOR_STATUS_INFO_s SensorStatus;
        AmbaSensor_GetStatus(AppEncChannel, &SensorStatus);
        SensorMode.Bits.Mode= SensorStatus.ModeInfo.Mode.Bits.Mode;
        AmbaSensor_GetModeInfo(AppEncChannel, SensorMode, &VinInfo);
    }

    for (DbValue = 0; DbValue < 7 ; ++DbValue) {

        AmbaKAL_TaskSleep(500);
        AmbaImg_Proc_Cmd(MW_IP_SET_AAA_OP_INFO, (UINT32)ChNo, (UINT32)&NoAAAFunc, 0);

        AmbaImg_Proc_Cmd(MW_IP_GET_AE_INFO, (UINT32)ChNo, IP_MODE_VIDEO,  (UINT32)&VideoAeInfo);
        VideoAeInfo.ShutterTime = (float)1/30;
        VideoAeInfo.AgcGain      = (float)pow(2.0, (double)DbValue);
        VideoAeInfo.Dgain         = WB_UNIT_GAIN;
        VideoAeInfo.Flash          = 0;
        VideoAeInfo.IrisIndex    = 0;
        AmbaImg_Proc_Cmd(MW_IP_SET_AE_INFO, (UINT32)ChNo, IP_MODE_VIDEO,  (UINT32)&VideoAeInfo);


        AmbaPrintColor(RED, "BLC: ShutterTime: %f AgcGain: %f Dgain: %d", VideoAeInfo.ShutterTime, VideoAeInfo.AgcGain, VideoAeInfo.Dgain);
        AmbaKAL_TaskSleep(500);
        //AmbaImg_Proc_Cmd(MW_IP_SET_AE_INFO, (UINT32)ChNo, IP_MODE_VIDEO, (UINT32)&AeInfo);
        //AmbaImg_Proc_Cmd(MW_IP_SET_AE_INFO, (UINT32)ChNo, IP_MODE_STILL, (UINT32)&AeInfo);
        //AppLib_CalibSetExposureValue(AeInfo.ShutterTime, AeInfo.AgcGain, AeInfo.Dgain);

        CalBLCInfo[Channel].SrcRawWidth = VinInfo.OutputInfo.RecordingPixels.Width;
        CalBLCInfo[Channel].SrcRawHeight = VinInfo.OutputInfo.RecordingPixels.Height;
        //raw capture
        AmbaPrint("start to do raw capture");
        RawPitch = ALIGN_32(VinInfo.OutputInfo.RecordingPixels.Width * 2);
        //ItunerRawCapCtrl.SensorMode.Bits.Mode = VinInfo.Mode.Bits.Mode;
        //ItunerRawCapCtrl.SensorMode.Bits.VerticalFlip = VinInfo.Mode.Bits.VerticalFlip;
        //ItunerRawCapCtrl.RawBuff.Raw.RawAddr = (UINT8 *)RawBufferAddress;
        //ItunerRawCapCtrl.RawBuff.Raw.RawPitch = RawPitch;// Basic DMA transfer unit is 32-byte, which is 16-pixels, so pad to 16pixels boundary here;
        //ItunerRawCapCtrl.RawBuff.Raw.RawWidth = VinInfo.OutputInfo.RecordingPixels.Width;
        //ItunerRawCapCtrl.RawBuff.Raw.RawHeight = VinInfo.OutputInfo.RecordingPixels.Height;
        //Rval = AmpUT_ItunerRawCapture(Flag, ItunerRawCapCtrl);

        AppLibStillEnc_RawCaptureSetSensorMode(1,pAmbaCalibInfoObj[0/*AMBA_VIN_CHANNEL0*/]->AmbaVignetteMode);
        AppLibStillEnc_CaptureRaw(&RawBufferAddress);
        AmbaKAL_TaskSleep(500);
        AmbaPrint("end of raw capture");


        CalBLCInfo[Channel].SrcRawAddr = RawBufferAddress;
        Rval = AmpCalib_BLCFunc(&CalBLCInfo[Channel],&CalBLCInfo[Channel].BlVideo[DbValue],OutputStr);
        {
            char Fn[32];
            AMBA_FS_FILE *Fp = NULL;

            sprintf(Fn, "c:\\blccal_video_%d.raw", DbValue);
			Fn[0] = AppLib_CalibGetDriverLetter();

            AmbaPrint("AppLibCalibBlc_SaveRawImage");
            //raw ready, dump it
            Fp = AmbaFS_fopen(Fn, "wb");
            AmbaPrint("[AmpVig_UT]Dump Raw 0x%X width: %d height: %d  ", \
                      CalBLCInfo[Channel].SrcRawAddr, \
                      VinInfo.OutputInfo.RecordingPixels.Width, \
                      VinInfo.OutputInfo.RecordingPixels.Height);
            AmbaFS_fwrite((UINT16 *)RawBufferAddress, RawPitch*VinInfo.OutputInfo.RecordingPixels.Height, 1, Fp);
            AmbaFS_fclose(Fp);
        }

        if (Rval < BL_CALIB_OK) {
            AppLibCalibBLC_PrintError(OutputStr,CalSite);
            return Rval;
        }
    }


//restore backup info
    AmbaImg_Proc_Cmd(MW_IP_SET_AAA_OP_INFO, (UINT32)ChNo, (UINT32)&CurAAAFunc, 0);
    AmbaImg_Proc_Cmd(MW_IP_SET_AE_INFO, (UINT32)ChNo, IP_MODE_STILL,  (UINT32)CurStillAeInfo);
    AmbaImg_Proc_Cmd(MW_IP_SET_AE_INFO, (UINT32)ChNo, IP_MODE_VIDEO,  (UINT32)&CurVideoAeInfo);


    if (Rval == BL_CALIB_OK) {

        if (CalBLCInfo[Channel].BlMode == 0) {
            sprintf(OutputStr, "[OK]BLC - Test - %d, %d, %d, %d",
                    CalBLCInfo[Channel].BlVideo[0].BlackR,
                    CalBLCInfo[Channel].BlVideo[0].BlackGr,
                    CalBLCInfo[Channel].BlVideo[0].BlackGb,
                    CalBLCInfo[Channel].BlVideo[0].BlackB);
        } else {
            BLCVar.BlackR = (CalBLCInfo[Channel].BlVideo[0].BlackR - CalBLCInfo[Channel].BlStd.BlackR);
            BLCVar.BlackGr = (CalBLCInfo[Channel].BlVideo[0].BlackGr - CalBLCInfo[Channel].BlStd.BlackGr);
            BLCVar.BlackGb = (CalBLCInfo[Channel].BlVideo[0].BlackGb - CalBLCInfo[Channel].BlStd.BlackGb);
            BLCVar.BlackB = (CalBLCInfo[Channel].BlVideo[0].BlackB - CalBLCInfo[Channel].BlStd.BlackB);

            BlcVarTmp = BLCVar.BlackR;
            if ( ABS(BlcVarTmp) < ABS(BLCVar.BlackGr) ) {
                BlcVarTmp = BLCVar.BlackGr;
            }
            if ( ABS(BlcVarTmp) < ABS(BLCVar.BlackGb) ) {
                BlcVarTmp = BLCVar.BlackGb;
            }
            if ( ABS(BlcVarTmp) < ABS(BLCVar.BlackB) ) {
                BlcVarTmp = BLCVar.BlackB;
            }
            BlcVarTmp = ABS(BlcVarTmp);

            if (BlcVarTmp <= BLCThreshold[Channel]) {
                CalSite->Version = CAL_BLC_VER;
                CalSite->Status = CAL_SITE_DONE;
                CalSite->SubSiteStatus[0] = CAL_SITE_DONE;
                sprintf(OutputStr, "[OK]BLC");
                CalBLCInfo[Channel].BlMode = BLC_MODE_APPLY;

                AmpCalib_BLCFunc(&CalBLCInfo[Channel],0,OutputStr);
                CalBLCInfo[Channel].BlCalResult = BL_CAL_OK;
                memcpy((CalObj->DramShadow+Channel*(sizeof(Cal_Blc_Info_s))), &CalBLCInfo[Channel], sizeof(Cal_Blc_Info_s));
            } else {
                CalSite->Status = CAL_SITE_RESET;
                sprintf(OutputStr,
                        "[NG]BLC: over threshold BL_Cur/BL_Std/BL_Thr: %d, %d, %d, %d/ %d, %d, %d, %d/ %d",
                        CalBLCInfo[Channel].BlVideo[0].BlackR,
                        CalBLCInfo[Channel].BlVideo[0].BlackGr,
                        CalBLCInfo[Channel].BlVideo[0].BlackGb,
                        CalBLCInfo[Channel].BlVideo[0].BlackB,
                        CalBLCInfo[Channel].BlStd.BlackR,
                        CalBLCInfo[Channel].BlStd.BlackGr,
                        CalBLCInfo[Channel].BlStd.BlackGb,
                        CalBLCInfo[Channel].BlStd.BlackB,
                        BLCThreshold[Channel]);
                AppLibCalibBLC_PrintError(OutputStr,CalSite);
                Rval = BL_CALIB_ERROR_OVER_THRESHOLD;
            }
        }
    } else {
        CalSite->Status = CAL_SITE_RESET;
        sprintf(OutputStr, "[NG]BLC: Overflow or Unknown bayer pattern!");
    }

    return Rval;
}

/**
 *  @brief the update function for black level calibration
 *
 *  the update function for black level calibration
 *
 *  @param [in] CalSite calibration site status
 *  @param [in] CalObj calibration object
 *
 *  @return 0 success, -1 failure
 */
int AppLibCalibBLC_Upgrade(Cal_Obj_s *CalObj, Cal_Stie_Status_s* CalSite)
{
    if (CalObj->Version != CalSite->Version) {
        // This API is an example to handle calibration data upgrade
        AmbaPrint("[CAL] Site %s Version mismatch (FW:0x%08X, NAND:0x%08X)", CalObj->Name, CalObj->Version, CalSite->Version);
    }
    // The default behavior is to do-nothing when Version mismatch
    return 0;
}

/**
 *  @brief the unit test function for black level calibration
 *
 *  the unit test function for black level calibration
 *
 *  @param [in] Argc number of input parameters
 *  @param [in] Argv value of input parameters
 *
 *  @return 0 success, -1 failure
 */
int AppLibBLC_UTFunc(int Argc, char *Argv[])
{
    Cal_Obj_s           *CalObj;
    int Rval = -1;
    int i;

    CalObj = AppLib_CalibGetObj(CAL_BLC_ID);
    if ((strcmp(Argv[2], "test") == 0)) {
        //register calibration site
        AppLib_CalibSiteInit();
        Rval = 0;
    } else if ((strcmp(Argv[2], "init") == 0)) {
        AppLibCalibBLC_Init(CalObj);
        Rval = 0;
    } else if ((strcmp(Argv[2], "info") == 0)) {
        Cal_Blc_Info_s CalBLCInfo[CALIB_CH_NO];
        UINT8 Channel;

        memcpy(&CalBLCInfo[0], CalObj->DramShadow, (sizeof(Cal_Blc_Info_s)*CALIB_CH_NO));
        for (Channel = 0; Channel < CALIB_CH_NO; Channel ++) {
            AmbaPrint("Channel %d BLC Result = %d ",Channel,CalBLCInfo[Channel].BlCalResult);
            if (CalBLCInfo[Channel].BlCalResult == BL_CAL_OK) {
                AmbaPrint("Standard BlackR = %d BlackGr = %d BlackGb = %d BlackB = %d",CalBLCInfo[Channel].BlStd.BlackR, \
                    CalBLCInfo[Channel].BlStd.BlackGr,CalBLCInfo[Channel].BlStd.BlackGb,CalBLCInfo[Channel].BlStd.BlackB);  

                AmbaPrint("*********************************************************************************");
                for(i = 0; i < 10; i++){
                    AmbaPrint("Video Gain[%d] BlackR = %d BlackGr = %d BlackGb = %d BlackB = %d",(int)pow(2.0, (double)i),CalBLCInfo[Channel].BlVideo[i].BlackR, \
                        CalBLCInfo[Channel].BlVideo[i].BlackGr,CalBLCInfo[Channel].BlVideo[i].BlackGb,CalBLCInfo[Channel].BlVideo[i].BlackB);
                }
                AmbaPrint("*********************************************************************************");
                AmbaPrint("*********************************************************************************");
                for(i = 0; i < 10; i++){
                    AmbaPrint("Still Gain[%d] BlackR = %d BlackGr = %d BlackGb = %d BlackB = %d",(int)pow(2.0, (double)i),CalBLCInfo[Channel].BlVideo[i].BlackR, \
                        CalBLCInfo[Channel].BlVideo[i].BlackGr,CalBLCInfo[Channel].BlVideo[i].BlackGb,CalBLCInfo[Channel].BlVideo[i].BlackB);
                }
                AmbaPrint("*********************************************************************************");
                
            }
        }
        return 0;
    }

    if (Rval == -1) {
        AmbaPrint("t cal blc init : re-init blc");
        AmbaPrint("t cal blc info : BLC information");
    }


    return Rval;
}
