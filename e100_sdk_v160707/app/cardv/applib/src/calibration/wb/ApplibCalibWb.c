/**
 * @file src/app/connected/applib/src/calibration/wb/ApplibCalibWB.c
 *
 * sample code for white balance calibration
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
#include <calibration/wb/ApplibCalibWb.h>
#include <AmbaDSP.h>
#include <AmbaSensor.h>
#include <imgcalib/AmbaCalib_Wb.h>
#include <AmbaPrintk.h>
#include <common/common.h>

/* Definition for WB tile config */

#define WB_COLOR_NUM     3
#define WB_TEMP_NUM      6
#define WB_LEFT          0
#define WB_RIGHT         1

/**
 *  @brief print the error message for white balance calibration
 *
 *  print the error message for white balance calibration
 *
 *  @param [in]CalSite calibration site status
 *  @param [out]OutputStr debug message for this function
 *
 *  @return 0 success, -1 failure
 */
int AppLibCalibWB_PrintError(char *OutputStr, Cal_Stie_Status_s *CalSite)
{
    AmbaPrint("************************************************************");
    AmbaPrint("%s",OutputStr);
    AmbaPrint("************************************************************");
    CalSite->Status = CAL_SITE_RESET;
    return 0;
}

/**
 *  @brief initial function for white balance calibration
 *
 *  initial function for white balance calibration
 *
 *  @param [in]CalObj calibration object
 *
 *  @return 0 success, -1 failure
 */
int AppLibCalibWB_Init(Cal_Obj_s *CalObj)
{
    UINT8  i;
    char OutputStr[256];
    WB_Detect_Info_s WBIdx;
    WB_Comp_Info_s WBCompInfo;

    memcpy(&WBCompInfo, CalObj->DramShadow, sizeof(WB_Comp_Info_s));

    for (i=0; i<WB_CAL_MAX_CH; i++) {

        WBIdx.ChID = i;

        // LCT Check
        WBIdx.Index = WB_CAL_STORE_LCT;
        if (WBCompInfo.Status[i]&WB_CAL_LCT_DONE) {
            WBIdx.CurR = WBCompInfo.OrgWB[i][0].GainR;
            WBIdx.CurG = WBCompInfo.OrgWB[i][0].GainG;
            WBIdx.CurB = WBCompInfo.OrgWB[i][0].GainB;
            WBIdx.RefR = WBCompInfo.RefWB[i][0].GainR;
            WBIdx.RefG = WBCompInfo.RefWB[i][0].GainG;
            WBIdx.RefB = WBCompInfo.RefWB[i][0].GainB;
        } else {
            WBIdx.CurR = WB_UNIT_GAIN;
            WBIdx.CurG = WB_UNIT_GAIN;
            WBIdx.CurB = WB_UNIT_GAIN;
            WBIdx.RefR = WB_UNIT_GAIN;
            WBIdx.RefG = WB_UNIT_GAIN;
            WBIdx.RefB = WB_UNIT_GAIN;
        }
        AmpCalib_CalWB(&WBIdx,OutputStr);

        // HCT Check
        WBIdx.Index = WB_CAL_STORE_HCT;
        if (WBCompInfo.Status[i]&WB_CAL_HCT_DONE) {
            WBIdx.CurR = WBCompInfo.OrgWB[i][1].GainR;
            WBIdx.CurG = WBCompInfo.OrgWB[i][1].GainG;
            WBIdx.CurB = WBCompInfo.OrgWB[i][1].GainB;
            WBIdx.RefR = WBCompInfo.RefWB[i][1].GainR;
            WBIdx.RefG = WBCompInfo.RefWB[i][1].GainG;
            WBIdx.RefB = WBCompInfo.RefWB[i][1].GainB;
        } else {
            WBIdx.CurR = WB_UNIT_GAIN;
            WBIdx.CurG = WB_UNIT_GAIN;
            WBIdx.CurB = WB_UNIT_GAIN;
            WBIdx.RefR = WB_UNIT_GAIN;
            WBIdx.RefG = WB_UNIT_GAIN;
            WBIdx.RefB = WB_UNIT_GAIN;
        }
        AmpCalib_CalWB(&WBIdx,OutputStr);

    }

    AmbaImg_Proc_Cmd(MW_IP_AMBA_AEAWBADJ_INIT, 0, 2, 0);

    return 0;
}

/**
 *  @brief check input parameter for white balance calibration
 *
 *  check input parameter for white balance calibration
 *
 *  @param [in]Argc input number
 *  @param [in]WB_Detect_Info_s parameters for white balance calibration
 *  @param [out]ParamChk check the precision of input parameter, Correct:1 / NG:0
 *
 *  @return 0 success, -1 failure
 */
static int AppLibCalibWB_ParamCheck(int Argc,char *Argv[], WB_Detect_Info_s *WBIdx, char *OutputStr)
{
    int Rval = WB_CALIB_ERROR_PARAMETER;
    UINT8 CheckVig = 0;


    if (((WBIdx->Index == WB_CAL_TEST)||((WBIdx->Index == WB_CAL_LCT) ||(WBIdx->Index == WB_CAL_HCT)))&&(Argc==9)) {
        CheckVig = atoi(Argv[8]);
        AmbaPrint("[WB_CAL PARAM] Type: %d", atoi(Argv[1]));
        AmbaPrint("[WB_CAL PARAM] Channel ID: %d", atoi(Argv[2]));
        AmbaPrint("[WB_CAL PARAM] Flicker Mode: %d", atoi(Argv[3]));
        AmbaPrint("[WB_CAL PARAM] Ref R: %d", atoi(Argv[4]));
        AmbaPrint("[WB_CAL PARAM] Ref B: %d", atoi(Argv[5]));
        AmbaPrint("[WB_CAL PARAM] Threshold R: %d", atoi(Argv[6]));
        AmbaPrint("[WB_CAL PARAM] Threshold B: %d", atoi(Argv[7]));
        AmbaPrint("[WB_CAL PARAM] Vignette Check Flag: %d", atoi(Argv[8]));
        Rval = WB_CALIB_OK;
    } else {
        sprintf(OutputStr,"WB Cal Parameter Wrong!");
        AmbaPrint("WB [type] [ChID] [flicker_mode] [R_avg] [B_avg] [R_thd] [B_thd][CheckVig]");
        Rval = WB_CALIB_ERROR_PARAMETER;
        return Rval;
    }
    if (CheckVig == 1) {
        if (AppLibCalibVignette_GetVignetteCalDataEnable() != 1) {
            sprintf(OutputStr,"Please do vignette calibration before WB calibration!");
            Rval = WB_CALIB_ERROR_NO_VIG;
        }
    }
    return Rval;
}

/**
 *  @brief check the WB gain is suitable or not?
 *
 *  check the WB gain is suitable or not?
 *
 *  @param [in]WBIdx the WB cal-mode information
 *  @param [in]ThR threshold for R gain
 *  @param [in]TBB threshold for B gain
 *  @param [in]ThColor threshold for the color difference between two sensor
 *  @param [in]SideR right sensor or left sensor
 *  @param [out]ParamChk check the precision of input parameter, Correct:1 / NG:0
 *  @param [out]OutputStr output debug message
 *
 *  @return 0 success, -1 failure
 */
static int AppLibCalibWB_CheckGainResult(WB_Detect_Info_s *WBIdx, UINT16 ThR, UINT16 ThB, char *OutputStr)
{
    UINT16 DeltaR = 0;
    UINT16 DeltaB = 0;

    if ((WBIdx->Index == WB_CAL_LCT)||(WBIdx->Index == WB_CAL_HCT)) {

        if (WBIdx->RefR > WBIdx->CurR) {
            DeltaR = WBIdx->RefR - WBIdx->CurR;
        } else {
            DeltaR = WBIdx->CurR - WBIdx->RefR;
        }

        if (WBIdx->RefB > WBIdx->CurB) {
            DeltaB = WBIdx->RefB - WBIdx->CurB;
        } else {
            DeltaB = WBIdx->CurB - WBIdx->RefB;
        }

        if ((DeltaR > ThR) || (DeltaB > ThB)) {
            sprintf(OutputStr,"WB calibration NG: Difference:(R:%d/B:%d) over Threshold(R:%d/B:%d)",DeltaR,DeltaB,ThR,ThB);
            return WB_CALIB_ERROR_DIFF_OVER_THRESHOLD;
        }

    }

    return WB_CALIB_OK;
}

/**
 *  @brief the entry function for white calibration function
 *
 *  the entry function for white calibration function
 *
 *  @param [in]Argc input number for WB calibration
 *  @param [in]Argv input value for WB calibration
 *  @param [in]CalSite calibration site status
 *  @param [in]CalObj calibration object
 *  @param [out]OutputStr output debug message
 *
 *  @return 0 success, -1 failure
 */
int AppLibCalibWB_Func(int Argc, char *Argv[], char *OutputStr, Cal_Stie_Status_s *CalSite, Cal_Obj_s *CalObj)
{
    int Rval = -1;
    WB_Detect_Info_s WBIdx;
    UINT8 i;
    UINT8 AEFlickerMode;
    UINT16 ThR = 0;
    UINT16 ThB = 0;
    WB_Comp_Info_s WBCompInfo;
    char Buffer[64];

    WBIdx.Index = (UINT8)(atoi(Argv[1]));

    if ((WBIdx.Index != WB_CAL_TEST) &&
            (WBIdx.Index != WB_CAL_LCT)  &&
            (WBIdx.Index != WB_CAL_HCT)  &&
            (WBIdx.Index != WB_CAL_RESET)) {
        WBIdx.Index = WB_CAL_INVALID;
    }

    if (WBIdx.Index == WB_CAL_RESET) {

        for (i=0; i<WB_CAL_MAX_CH; i++) {
            WBIdx.ChID = i;
            Rval = AmpCalib_CalWB(&WBIdx,OutputStr);
            WBCompInfo.OrgWB[i][0].GainR = WB_UNIT_GAIN;
            WBCompInfo.OrgWB[i][0].GainG = WB_UNIT_GAIN;
            WBCompInfo.OrgWB[i][0].GainB = WB_UNIT_GAIN;
            WBCompInfo.RefWB[i][0].GainR = WB_UNIT_GAIN;
            WBCompInfo.RefWB[i][0].GainG = WB_UNIT_GAIN;
            WBCompInfo.RefWB[i][0].GainB = WB_UNIT_GAIN;
            WBCompInfo.OrgWB[i][1].GainR = WB_UNIT_GAIN;
            WBCompInfo.OrgWB[i][1].GainG = WB_UNIT_GAIN;
            WBCompInfo.OrgWB[i][1].GainB = WB_UNIT_GAIN;
            WBCompInfo.RefWB[i][1].GainR = WB_UNIT_GAIN;
            WBCompInfo.RefWB[i][1].GainG = WB_UNIT_GAIN;
            WBCompInfo.RefWB[i][1].GainB = WB_UNIT_GAIN;
            WBCompInfo.Status[i] = WB_CAL_LHCT_RESET;
        }

        memcpy(CalObj->DramShadow, &WBCompInfo, sizeof(WB_Comp_Info_s));

        return Rval;
    }

    WBIdx.ChID = (UINT8)(atoi(Argv[2]));

    AEFlickerMode = (UINT8)(atoi(Argv[3]));

    Rval = AppLibCalibWB_ParamCheck(Argc, Argv, &WBIdx, OutputStr);
    if (Rval < 0) {
        AppLibCalibWB_PrintError(OutputStr,CalSite);
        return Rval;
    }

    if (AEFlickerMode==60) {
        WBIdx.FlickerMode = ANTI_FLICKER_60HZ;
    } else { //(AEFlickerMode==50)
        WBIdx.FlickerMode = ANTI_FLICKER_50HZ;
    }

    if (WBIdx.Index != WB_CAL_TEST) {
        WBIdx.RefR = (UINT16)(atoi(Argv[4]));
        WBIdx.RefG = WB_UNIT_GAIN;
        WBIdx.RefB = (UINT16)(atoi(Argv[5]));
        ThR = (UINT16)(atoi(Argv[6]));
        ThB = (UINT16)(atoi(Argv[7]));
    }

    switch (WBIdx.Index) {

        case WB_CAL_TEST:
        case WB_CAL_LCT:
        case WB_CAL_HCT:
            Rval = AmpCalib_CalWB(&WBIdx,OutputStr);
            if (Rval<WB_CALIB_OK) {
                AppLibCalibWB_PrintError(OutputStr,CalSite);
            }

            // check threshold
            if ((WBIdx.Index==WB_CAL_LCT)||(WBIdx.Index==WB_CAL_HCT)) {
                Rval = AppLibCalibWB_CheckGainResult( &WBIdx, ThR, ThB, OutputStr);
                if (Rval<WB_CALIB_OK) {
                    AppLibCalibWB_PrintError(OutputStr,CalSite);
                }
            }
            break;

        case WB_CAL_INVALID:
        default:
            sprintf(OutputStr,"Wrong WB_CAL Mode!");
            AppLibCalibWB_PrintError(OutputStr,CalSite);
            Rval =  WB_CALIB_ERROR_INCORRECT_INDEX;
            break;
    }

    if (Rval == WB_CALIB_OK) {

        if (WBIdx.Index == WB_CAL_TEST) {
            sprintf(OutputStr, "[OK]WB Calibration: (Test)Ravg/Bavg:%d/%d", WBIdx.CurR, WBIdx.CurB);
        } else {
            CalSite->Version = CAL_WB_VER;
            memcpy(&WBCompInfo, CalObj->DramShadow, sizeof(WB_Comp_Info_s));

            if (WBIdx.Index == WB_CAL_LCT) {

                WBIdx.Index = WB_CAL_STORE_LCT;

                if (AmpCalib_CalWB(&WBIdx,OutputStr) == WB_CALIB_OK) {
                    AmbaPrint("R = %d, B = %d", WBIdx.CurR, WBIdx.CurB);
                    CalSite->SubSiteStatus[1] = CAL_SITE_DONE;

                    if (CalSite->SubSiteStatus[2] != CAL_SITE_DONE) {
                        AmbaPrint("[CAL]No WB(HCT)calibration!,WB(LCT)finish!");
                    }

                    CalSite->Status = CAL_SITE_DONE;

                    WBCompInfo.OrgWB[WBIdx.ChID][0].GainR = WBIdx.CurR;
                    WBCompInfo.OrgWB[WBIdx.ChID][0].GainG = WBIdx.CurG;
                    WBCompInfo.OrgWB[WBIdx.ChID][0].GainB = WBIdx.CurB;
                    WBCompInfo.RefWB[WBIdx.ChID][0].GainR = WBIdx.RefR;
                    WBCompInfo.RefWB[WBIdx.ChID][0].GainG = WBIdx.RefG;
                    WBCompInfo.RefWB[WBIdx.ChID][0].GainB = WBIdx.RefB;
                    WBCompInfo.Status[WBIdx.ChID] = (WBCompInfo.Status[WBIdx.ChID]|WB_CAL_LCT_DONE);
                    memcpy(CalObj->DramShadow, &WBCompInfo, sizeof(WB_Comp_Info_s));
                    sprintf(OutputStr, "[OK]WB Calibration(LCT)");
                }

            } else if (WBIdx.Index == WB_CAL_HCT) {

                WBIdx.Index = WB_CAL_STORE_HCT;

                if (AmpCalib_CalWB(&WBIdx,OutputStr) == WB_CALIB_OK) {

                    AmbaPrint("R = %d, B = %d", WBIdx.CurR, WBIdx.CurB);
                    CalSite->SubSiteStatus[2] = CAL_SITE_DONE;

                    if (CalSite->SubSiteStatus[1] != CAL_SITE_DONE) {
                        AmbaPrint("[CAL]No WB(LCT)calibration!,WB(HCT)finish!");
                    }

                    CalSite->Status = CAL_SITE_DONE;

                    WBCompInfo.OrgWB[WBIdx.ChID][1].GainR = WBIdx.CurR;
                    WBCompInfo.OrgWB[WBIdx.ChID][1].GainG = WBIdx.CurG;
                    WBCompInfo.OrgWB[WBIdx.ChID][1].GainB = WBIdx.CurB;
                    WBCompInfo.RefWB[WBIdx.ChID][1].GainR = WBIdx.RefR;
                    WBCompInfo.RefWB[WBIdx.ChID][1].GainG = WBIdx.RefG;
                    WBCompInfo.RefWB[WBIdx.ChID][1].GainB = WBIdx.RefB;
                    WBCompInfo.Status[WBIdx.ChID] = (WBCompInfo.Status[WBIdx.ChID]|WB_CAL_HCT_DONE);
                    memcpy(CalObj->DramShadow, &WBCompInfo, sizeof(WB_Comp_Info_s));
                    sprintf(OutputStr, "[OK]WB Calibration(HCT)");

                }
            }

        }

    } else {
        if (WBIdx.Index == WB_CAL_TEST) {
            sprintf(OutputStr, "[NG]WB Calibration: Test");
        } else if (WBIdx.Index == WB_CAL_LCT) {
            CalSite->Status = CAL_SITE_RESET;
            CalSite->SubSiteStatus[1] = CAL_SITE_RESET;
            sprintf(Buffer, "[NG]WB Calibration: (LCT)Ravg/Bavg:%d/%d",
                    WBIdx.CurR, WBIdx.CurB);
            strcat(OutputStr,Buffer);
        } else if (WBIdx.Index == WB_CAL_HCT) {
            CalSite->Status = CAL_SITE_RESET;
            CalSite->SubSiteStatus[2] = CAL_SITE_RESET;
            sprintf(Buffer, "[NG]WB Calibration: (HCT)Ravg/Bavg:%d/%d",
                    WBIdx.CurR, WBIdx.CurB);
            strcat(OutputStr,Buffer);
        }
    }

    return Rval;
}

/**
 *  @brief the upgrade function for white calibration
 *
 *  the upgrade function for white calibration
 *
 *  @param [in]CalSite calibration site status
 *  @param [in]CalObj calibration object
 *
 *  @return 0 success, -1 failure
 */
int AppLibCalibWB_Upgrade(Cal_Obj_s *CalObj, Cal_Stie_Status_s* CalSite)
{
    if (CalObj->Version != CalSite->Version) {
        // This API is an example to handle calibration data upgrade
        AmbaPrint("[CAL] Site %s Version mismatch (FW:0x%08X, NAND:0x%08X)", CalObj->Name, CalObj->Version, CalSite->Version);
    }

    // The default behavior is to do-nothing when Version mismatch
    return 0;
}

/**
 *  @brief the unit test function for white balance calibration
 *
 *  the unit test function for white balance calibration
 *
 *  @param [in]env environment
 *  @param [in]argc the number of the input parameter
 *  @param [in]argv value of input parameter
 *
 *  @return 0 success, -1 failure
 */
int AppLibWB_UTFunc(int Argc, char *Argv[])
{
    Cal_Obj_s           *CalObj;
    UINT16        ChIdx;
    int Rval = -1;

    CalObj = AppLib_CalibGetObj(CAL_WB_ID);
    if ((strcmp(Argv[2], "test") == 0)) {
        //register calibration site
        AppLib_CalibSiteInit();
        Rval = 0;
    } else if ((strcmp(Argv[2], "init") == 0)) {
        AppLibCalibWB_Init(CalObj);
        Rval = 0;
    } else if ((strcmp(Argv[2], "info") == 0)) {
        WB_Comp_Info_s WBCompInfo;

        memcpy(&WBCompInfo, CalObj->DramShadow, sizeof(WB_Comp_Info_s));

        ChIdx = (UINT16)(atoi(Argv[3]));

        AmbaPrint("Ch-%d WBCompInfo.Status: %d", ChIdx, WBCompInfo.Status[ChIdx]);
        if (WBCompInfo.Status[ChIdx] & WB_CAL_LCT_DONE) {
            AmbaPrint("Ch-%d WB_Cal LCT: Done", ChIdx);
        } else {
            AmbaPrint("Ch-%d WB_Cal LCT: None", ChIdx);
        }

        if (WBCompInfo.Status[ChIdx] & WB_CAL_HCT_DONE) {
            AmbaPrint("Ch-%d WB_Cal HCT: Done", ChIdx);
        } else {
            AmbaPrint("Ch-%d WB_Cal HCT: None", ChIdx);
        }

        AmbaPrint("Ch-%d WB_Cal LCT Src Avg-R: %d", ChIdx, WBCompInfo.OrgWB[ChIdx][0].GainR);
        AmbaPrint("Ch-%d WB_Cal LCT Src Avg-G: %d", ChIdx, WBCompInfo.OrgWB[ChIdx][0].GainG);
        AmbaPrint("Ch-%d WB_Cal LCT Src Avg-B: %d", ChIdx, WBCompInfo.OrgWB[ChIdx][0].GainB);
        AmbaPrint("Ch-%d WB_Cal HCT Src Avg-R: %d", ChIdx, WBCompInfo.OrgWB[ChIdx][1].GainR);
        AmbaPrint("Ch-%d WB_Cal HCT Src Avg-G: %d", ChIdx, WBCompInfo.OrgWB[ChIdx][1].GainG);
        AmbaPrint("Ch-%d WB_Cal HCT Src Avg-B: %d", ChIdx, WBCompInfo.OrgWB[ChIdx][1].GainB);
        AmbaPrint("Ch-%d WB_Cal LCT Tgt Avg-R: %d", ChIdx, WBCompInfo.RefWB[ChIdx][0].GainR);
        AmbaPrint("Ch-%d WB_Cal LCT Tgt Avg-G: %d", ChIdx, WBCompInfo.RefWB[ChIdx][0].GainG);
        AmbaPrint("Ch-%d WB_Cal LCT Tgt Avg-B: %d", ChIdx, WBCompInfo.RefWB[ChIdx][0].GainB);
        AmbaPrint("Ch-%d WB_Cal HCT Tgt Avg-R: %d", ChIdx, WBCompInfo.RefWB[ChIdx][1].GainR);
        AmbaPrint("Ch-%d WB_Cal HCT Tgt Avg-G: %d", ChIdx, WBCompInfo.RefWB[ChIdx][1].GainG);
        AmbaPrint("Ch-%d WB_Cal HCT Tgt Avg-B: %d", ChIdx, WBCompInfo.RefWB[ChIdx][1].GainB);

        Rval = 0;
    }

    if (Rval == -1) {
        AmbaPrint("t cal wb init : re-init wb");
        AmbaPrint("t cal wb info : wb information");
    }

    return Rval;
}


