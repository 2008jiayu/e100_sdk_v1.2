/**
 * @file src/app/connected/applib/src/calibration/vig/ApplibCalibVig.c
 *
 * sample code for vignette calibration
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
#include <calibration/vig/ApplibCalibVig.h>
#include <common/common.h>
#include <AmbaUtility.h>
#include <imgproc/AmbaImg_AeAwb.h>

Vignette_Control_s AppVignetteControl;

//TBD: vig cal app, vig task
/***************************************************
*Site: Lens Shading Calibration by A5/A5S vignette
*Command: cal vignette
***************************************************/
#define VIGNETTE_CAL_APP_ARGC_NUM (18+MAX_VIGNETTE_NAND_TABLE_COUNT)

typedef struct Vignette_Cal_Param_s_ {
    UINT8  CalSubSiteId;         // Status should be returned to which subsite
    UINT8  Channel;              // Channel no.
    UINT8  DarkCornerSizeX;       //dark corner size.0: disable
    UINT8  DarkCornerSizeY;       //dark corner size.0: disable
    UINT8  CompensateMethod;       //< Method for compensating, 0x80: luma only, 0x40:4 channel compensate with the same gain
    UINT8  SaveFileFlagAPP;      // save flag of vignette calibration App debug dump
    UINT8  LookupShift;            // 0 for 3.7, 1 for 2.8, 2 for 1.9, 3 for 0.10, 255 for auto select
    UINT16 Threshold;               // default value 4096
    UINT16 CompensateRatio;        // 0-1024
    INT16  AeTarget;               // default value 830, -1 for manual AE info
    UINT8  FlickerMode;            // Anti flicker mode. 50 or 60
    float  ManualAgc;          // Manual AGC value, only applied to manual mode
    float  InverseShutterTime;          // Manual inverse shutter value, only applied to manual mode
    UINT16 ManualIrisIdx;         // Manual iris index, only applied to manual mode
    INT16  ManualBlackLevel;      // Manual black level value of raw data
    UINT8  CalStepStartOffset;   // starting index of vignette table to be used
    UINT8  NumCalSteps;           // 1~MAX_VIGNETTE_GAIN_TABLE_COUNT
    UINT32 CalZoomSteps[MAX_VIGNETTE_NAND_TABLE_COUNT]; //Ozoom step to be calibrated.
    UINT8  Reserved[2];
} Vignette_Cal_Param_s; //Total 19 paramters, Argv[1] ~ Argv[19], Argc should be 20



typedef struct Vignette_UnPack_Storage_s_ {
    UINT8  Enable;
    UINT8  GainShift;
    UINT8   Reserved;
    UINT8   Reserved1;
    UINT32  VignetteStrength;
    AMBA_DSP_IMG_VIN_SENSOR_GEOMETRY_s  CurrentVinSensorGeo;
    UINT32  Version;
    int     TableWidth;
    int     TableHeight;
    AMBA_DSP_IMG_VIN_SENSOR_GEOMETRY_s  CalibVinSensorGeo;
    UINT32  Reserved2;                   // Reserved for extention.
    //Original decoded vignette map
    UINT16 VignetteRedGain[VIGNETTE_MAX_SIZE_INTERNAL];
    UINT16 VignetteGreenEvenGain[VIGNETTE_MAX_SIZE_INTERNAL];
    UINT16 VignetteGreenOddGain[VIGNETTE_MAX_SIZE_INTERNAL];
    UINT16 VignetteBlueGain[VIGNETTE_MAX_SIZE_INTERNAL];
} Vignette_UnPack_Storage_s;

typedef enum _VIGNETTE_VIGSTRENGTHEFFECT_MODE_e_ {
    VIGNETTE_DefaultMode = 0,
    VIGNETTE_KeepRatioMode = 1,
} VIGNETTE_VIGSTRENGTHEFFECT_MODE_e;

static Vignette_UnPack_Storage_s UnPackVignette[CALIB_CH_NO];
static Vignette_Select_Param_s VignetteSelectParam[CALIB_CH_NO][2];//video/still mode
AMBA_DSP_IMG_VIGNETTE_CALC_INFO_s VignetteInfo[CALIB_CH_NO];


/**
 *  @brief get vignette parameter for calibration
 *
 *  get vignette parameter for calibration
 *
 *  @param [in]Channel Channel ID 
 *  @param [in]Mode video or still
 *  @param [in]Param vignette setting parameter
 *
 *  @return 0 success, -1 failure
 */
int AppLibCalibVignette_SetParameter(Vignette_Select_Param_s *Param,UINT8 Channel, UINT8 Mode)
{
    memcpy(&VignetteSelectParam[Channel][Mode], Param, sizeof(Vignette_Select_Param_s));
    return 0;
}

/**
 *  @brief set vignette parameter for calibration
 *
 *  set vignette parameter for calibration
 *
 *  @param [in]Channel Channel ID 
 *  @param [in]Mode video or still
 *  @param [out]Param vignette setting parameter
 *
 *  @return 0 success, -1 failure
 */
int AppLibCalibVignette_GetParameter(Vignette_Select_Param_s *Param,UINT8 Channel, UINT8 Mode)
{
    memcpy(Param, &VignetteSelectParam[Channel][Mode], sizeof(Vignette_Select_Param_s));
    return 0;
}

/**
 *  @brief lock vignette buffer
 *
 *  lock vignette buffer
 *
 *  @return 0 success
 */
int AppLibCalibVignette_BufferLock(void)
{
    AppVignetteControl.BufferLocked = 1;
    return 0;
}


/**
 *  @brief unlock vignette buffer
 *
 *  unlock vignette buffer
 *
 *
 *  @return 0 success
 */
int AppLibCalibVignette_BufferUnlock(void)
{
    AppVignetteControl.BufferLocked = 0;
    return 0;
}


/**
 *  @brief check the lock Flag for vignette buffer
 *
 *  check the lock Flag for vignette buffer
 *
 *
 *  @return the lock Flag
 */
int AppLibCalibVignette_BufferIsLocked(void)
{
    return AppVignetteControl.BufferLocked;
}



/**
 *  @brief get vignette calibration enable Flag
 *
 *   get vignette calibration enable Flag
 *
 *  @return enable Flag
 */
UINT8  AppLibCalibVignette_GetVignetteCalDataEnable(void)
{
    return AppVignetteControl.Enable;
}

/**
 *  @brief get vignette calibration data pointer
 *
 *  get vignette calibration data pointer
 *
 *  @param [out]
 *
 */
Vignette_Control_s* AppLibCalibVignette_GetVignetteCalData(void)
{
    return &AppVignetteControl;
}

/**
 *  @brief turn on or turn off the debug Flag for vignette
 *
 *  turn on or turn off the debug Flag for vignette
 *
 *  @param [in]Enable enable Flag to print debug message
 *
 */
void AppLibCalibVignette_DebugEnable(UINT8 Enable)
{
    AppVignetteControl.Debug = Enable;
}

/**
 *  @brief Get vignette Decompress Data address
 *
 *  Get vignette Decompress Data address
 *
 *  @param [in]CalId calibration ID
 *
 *  @return address of vignette Decompress Data
 */
AMBA_DSP_IMG_VIGNETTE_CALC_INFO_s* AppLibCalibVignette_GetVigDecompressDataAddress(void)
{
    return &VignetteInfo[0];
}

/**
 *  @brief config vigneete table and do the vigneet decompression
 *
 *   config vigneete table and do the vigneet decompression
 *
 *  @param [in]Select the sensor channel of vignette
 *  @param [in]VignetteConfig the map config of vignette
 *  @param [out]VigCoreGeo output of vigette geometry
 *
 *  @return 0 success, -1 failure
 */
int ApplibCalibVignette_ConfigVignette(UINT8 Select, Vignette_Global_Map_Config_Info_s *VignetteConfig)
{
    static int First = 1;
    int CompressIndex = 0, DecompressIndex = 0;
    int NumVignetteStorage, i, j;
    static int MinGainShift = 100;
    int ActiveVignetteStorageNum = 0;
    Vignette_Pack_Storage_s *VignettePackStorage;
    AMBA_DSP_IMG_VIN_SENSOR_GEOMETRY_s VignetteGeoInfo0 = {0,0,0,0,{0,0},{0,0}};
    int GainShiftCompensateValue[2];
    int BlendOn = 0, Color;
    UINT16 *CompressVignetteMapAddress[2][4];
    UINT16 *DecompressVignetteMapAddress[4];
    UINT32 TempSum[8];
    static UINT16 Temp8U10[8];
    int BlendRatio;
    int ActiveBlendRatios[2];
    int Ch,StartCh,EndCh;
    static UINT8 IdOld = 0xff;
    AMBA_DSP_IMG_VIGNETTE_CALC_INFO_s *VignetteInfo[CALIB_CH_NO] = {0};
    AMBA_DSP_IMG_VIGNETTE_CALC_INFO_s *VignetteAddress = 0;

    VignetteAddress = AppLibCalibVignette_GetVigDecompressDataAddress();
    if (AppVignetteControl.Debug) {
        AmbaPrint("VignetteAddress = %d",(UINT32)VignetteAddress);
    }
    for (i = 0; i < CALIB_CH_NO; i++) {
        VignetteInfo[i] = &VignetteAddress[i];
        if (AppVignetteControl.Debug) {
            AmbaPrint("VignetteInfo[i] = %d VignetteInfo[i] = %d",(UINT32)VignetteInfo[i],(UINT32)&VignetteInfo[i]);
        }
    }

    if (First) {
        memset(VignetteInfo[0],0x0,(CALIB_CH_NO*sizeof(AMBA_DSP_IMG_VIGNETTE_CALC_INFO_s)));
        First = 0;
    }
    if (Select == VIG_CH_ALL) {
        StartCh = 0;
        EndCh = CALIB_CH_NO-1;
    } else {
        StartCh = Select;
        EndCh = Select;
    }

    NumVignetteStorage = VignetteConfig->NumVignetteTable;
    if ((VignetteConfig->Id[0] > VignetteConfig->NumVignetteTable) ||(VignetteConfig->Id[1] > VignetteConfig->NumVignetteTable)) {
        if (AppVignetteControl.Debug) {
            AmbaPrint("the vignette gain table ID(%d/%d) is bigger than exist table number(%d)",\
                      VignetteConfig->Id[0],VignetteConfig->Id[1],VignetteConfig->NumVignetteTable);
        }
        return -1;
    }
    for (Ch = StartCh; Ch <= EndCh; Ch++) {
        NumVignetteStorage = VignetteConfig->NumVignetteTable;
        if (NumVignetteStorage == 0) {
            if (AppVignetteControl.Debug) {
                AmbaPrint("Error! vignette storage num = 0");
            }
            return -1;
        } else if (NumVignetteStorage > VIGNETTE_STORAGE_BLEND_NUM_MAX) {
            if (AppVignetteControl.Debug) {
                AmbaPrint("Error! vignette storage num %d exceed limit!",NumVignetteStorage);
            }
            return -1;
        }
        //Check multiple vignette table geometry consistency
        for (i=0; i<2; i++) {
            VignettePackStorage = VignetteConfig->VignetteStorageAddress[VignetteConfig->Id[i]];
            BlendRatio = VignetteConfig->BlendRatio;
            if ((VignettePackStorage->Enable != 1)) {
                if (AppVignetteControl.Debug) {
                    AmbaPrint("Error! VignetteStorageAddress[%d] is not valid", i);
                }
                IdOld = 0xff;
                return -1;
            } else if (VignettePackStorage->Enable && (IdOld != VignetteConfig->Id[i])) {
                if ((VignettePackStorage->GainShift+7) < MinGainShift) {
                    MinGainShift = (VignettePackStorage->GainShift+7);
                }
                if ((VignetteGeoInfo0.HSubSample.FactorDen == 0) || (VignetteGeoInfo0.HSubSample.FactorNum== 0)||
                        (VignetteGeoInfo0.VSubSample.FactorDen == 0) || (VignetteGeoInfo0.VSubSample.FactorNum== 0)) {
                    VignetteGeoInfo0 = VignettePackStorage->CalibVinSensorGeo;
                    if ((VignetteGeoInfo0.Width == 0) || (VignetteGeoInfo0.Height == 0)) {
                        if (AppVignetteControl.Debug) {
                            AmbaPrint("Error: Vignette compensation data does not contain sensor window info");
                        }
                        IdOld = 0xff;
                        return -1;
                    }
                } else {
                    if (memcmp(&VignetteGeoInfo0,    &(VignettePackStorage->CalibVinSensorGeo),sizeof(AMBA_DSP_IMG_VIN_SENSOR_GEOMETRY_s))) {
                        if (AppVignetteControl.Debug) {
                            AmbaPrint("Error! vignette_storage[%d] geometry not match vignette_storage[0]", i);
                        }
                        IdOld = 0xff;
                        return -1;
                    }
                }

                UnPackVignette[Ch].Enable = VignettePackStorage->Enable;
                UnPackVignette[Ch].GainShift = \
                                               VignetteInfo[Ch]->GainShift = MinGainShift;
                VignetteInfo[Ch]->CalibVignetteInfo.CalibVinSensorGeo = VignettePackStorage->CalibVinSensorGeo;

                CompressVignetteMapAddress[ActiveVignetteStorageNum][0] = (UINT16 *)VignettePackStorage->VignetteRedGain;
                CompressVignetteMapAddress[ActiveVignetteStorageNum][1] = (UINT16 *)VignettePackStorage->VignetteGreenEvenGain;
                CompressVignetteMapAddress[ActiveVignetteStorageNum][2] = (UINT16 *)VignettePackStorage->VignetteGreenOddGain;
                CompressVignetteMapAddress[ActiveVignetteStorageNum][3] = (UINT16 *)VignettePackStorage->VignetteBlueGain;
                ActiveVignetteStorageNum++;
            }
            IdOld = VignetteConfig->Id[i];
        }
        IdOld = 0xff;

        if (ActiveVignetteStorageNum == 1) {    // Check Only one BlendOn value.Not need blend
            BlendOn = 0;
        }
        if (AppVignetteControl.Debug) {
            AmbaPrint("ActiveVignetteStorageNum = %d BlendOn = %d",ActiveVignetteStorageNum,BlendOn);
        }
        ActiveVignetteStorageNum = 0;
        for (i=0; i<2; i++) {
            VignettePackStorage = VignetteConfig->VignetteStorageAddress[VignetteConfig->Id[i]];

            if (VignettePackStorage->Enable && (IdOld != VignetteConfig->Id[i])) {
                GainShiftCompensateValue[ActiveVignetteStorageNum] = VignettePackStorage->GainShift - MinGainShift;
                ActiveVignetteStorageNum++;
                IdOld = VignetteConfig->Id[i];
            }
        }
        IdOld = 0xff;
        if (Select == 0 ) {
            VignettePackStorage = VignetteConfig->VignetteStorageAddress[0];
        }
        if (MinGainShift != UnPackVignette[Ch].GainShift) {
            if (AppVignetteControl.Debug) {
                AmbaPrint("Vignette MW Error: vignette tables  GainShift is different");
            }
            return -1;
        }

        UnPackVignette[Ch].CalibVinSensorGeo = VignettePackStorage->CalibVinSensorGeo;

        VignetteInfo[Ch]->CalibVignetteInfo.pVignetteRedGain       = (UINT16 *)UnPackVignette[Ch].VignetteRedGain;
        VignetteInfo[Ch]->CalibVignetteInfo.pVignetteGreenEvenGain = (UINT16 *)UnPackVignette[Ch].VignetteGreenEvenGain;
        VignetteInfo[Ch]->CalibVignetteInfo.pVignetteGreenOddGain  = (UINT16 *)UnPackVignette[Ch].VignetteGreenOddGain;
        VignetteInfo[Ch]->CalibVignetteInfo.pVignetteBlueGain      = (UINT16 *)UnPackVignette[Ch].VignetteBlueGain;

        //decode + blend compressed vignette data to G_vignette_storage
        DecompressVignetteMapAddress[0] = UnPackVignette[Ch].VignetteRedGain;
        DecompressVignetteMapAddress[1] = UnPackVignette[Ch].VignetteGreenEvenGain;
        DecompressVignetteMapAddress[2] = UnPackVignette[Ch].VignetteGreenOddGain;
        DecompressVignetteMapAddress[3] = UnPackVignette[Ch].VignetteBlueGain;


        if (BlendOn) { //Need to do vignette table blending
            ActiveVignetteStorageNum = 2;
            ActiveBlendRatios[0] = BlendRatio;
            ActiveBlendRatios[1] = MAX_BLEND_VALUE-BlendRatio;
            for (Color = 0; Color < 4; Color++) {
                CompressIndex = 0;
                for (DecompressIndex=0; DecompressIndex<VIGNETTE_MAX_SIZE_INTERNAL; DecompressIndex+=8) {
                    TempSum[0] = TempSum[1] = TempSum[2] = TempSum[3] =\
                                                           TempSum[4] = TempSum[5] = TempSum[6] = TempSum[7] = 0;
                    for (i=0; i<ActiveVignetteStorageNum; i++) {
                        AmpCalib_VigDecompress(Temp8U10,
                                               &CompressVignetteMapAddress[i][Color][CompressIndex]);
                        for (j = 0; j < 8; j++) {
                            TempSum[j] += (Temp8U10[j] >> GainShiftCompensateValue[i]) * ActiveBlendRatios[i];
                        }
                    }
                    for (j = 0; j < 8; j++) {
                        DecompressVignetteMapAddress[Color][DecompressIndex+j]
                            = (TempSum[j] >> BLEND_SHIFT);
                    }
                    CompressIndex += 5;
                }
            }
        } else {
            for (Color = 0; Color < 4; Color++) {
                CompressIndex = 0;
                for (DecompressIndex=0; DecompressIndex<VIGNETTE_MAX_SIZE_INTERNAL; DecompressIndex+=8) {
                    AmpCalib_VigDecompress(&DecompressVignetteMapAddress[Color][DecompressIndex],
                                           &CompressVignetteMapAddress[0][Color][CompressIndex]);
                    CompressIndex += 5;
                }
            }
        }
    }

    return 0;
}

/**
 *  @brief control vignette table with blend and decay ratio for multiple channel
 *
 *  control vignette table with blend and decay ratio for multiple channel
 *
 *  @param [in]Enable  Disable:0, Enable:1
 *  @param [in]Channel channel id, for different sensor input
 *  @param [in]Id1 the ID number for blending
 *  @param [in]Id2 the ID number for blending
 *  @param [in]Blend blend ratio for the two tables
 *  @param [in]VignetteLumaStrength Strength for vignette calibration data (Luma)
 *  @param [in]VignetteChromaStrength Strength for vignette calibration data (Chroma)
 *
 *  @return 0 success, -1 failure
 */
int AppLibCalibVignette_SelectVignetteTable(UINT8 Enable, UINT8 Channel, UINT8 Id1, UINT8 Id2, int Blend, UINT32 VignetteLumaStrength,UINT32 VignetteChromaStrength)
{
    int BlendRatio;
    Vignette_Global_Map_Config_Info_s VgmConfig;
    int i;
    UINT8 EnValue;
    AMBA_DSP_IMG_MODE_CFG_s ImgMode;
    AMBA_DSP_IMG_VIGNETTE_CALC_INFO_s *VignetteInfo[CALIB_CH_NO] = {0};
    AMBA_DSP_IMG_VIGNETTE_CALC_INFO_s *VignetteAddress = 0;
    static INT8 SelectVignetteTable[CALIB_CH_NO] = {0};//used to make sure the data is existed or empty
    INT32   Rval = -1;
    AMBA_SENSOR_STATUS_INFO_s SensorStatus = {0};
    UINT8 FlipMirror;

    AMBA_DSP_IMG_WARP_CALC_INFO_s CalcWarp = {0};

    if(AppLibCalibVignette_BufferIsLocked() == 1){
        AmbaPrint( "[VNC]Vignette Buffer is Locked, update vignette map is forbidden");
        return -1;
    }
    memset(&ImgMode, 0, sizeof(AMBA_DSP_IMG_MODE_CFG_s));
    AppLib_CalibGetDspMode(&ImgMode);
    if (AmbaDSP_ImgGetWarpCompensation(&ImgMode, &CalcWarp) != OK) {
        if (AppVignetteControl.Debug) {
            AmbaPrint("Get Warp Compensation fail!!");
        }
    }
    VignetteAddress = AppLibCalibVignette_GetVigDecompressDataAddress();
    VignetteInfo[Channel] = &VignetteAddress[Channel];

    //backup for vignette parameters
    if(Enable == VIG_UPDATE_ALL){
        for(i = 0; i < 2; i++ ){
            VignetteSelectParam[Channel][i].Enable = Enable;
            VignetteSelectParam[Channel][i].Channel = Channel;
            VignetteSelectParam[Channel][i].Id1 = Id1;
            VignetteSelectParam[Channel][i].Id2 = Id2;
            VignetteSelectParam[Channel][i].Blend = Blend;
            VignetteSelectParam[Channel][i].VignetteChromaStrength = VignetteChromaStrength;
            VignetteSelectParam[Channel][i].VignetteLumaStrength = VignetteLumaStrength;
        }
    }
    if ((Enable&VIG_UPDATE_VIN_INFO) == VIG_UPDATE_VIN_INFO) {
        if (SelectVignetteTable[Channel] == 0) {
            return 0;
        }
        Enable = VignetteSelectParam[Channel][ImgMode.Pipe].Enable;
        Channel = VignetteSelectParam[Channel][ImgMode.Pipe].Channel;
        Id1 = VignetteSelectParam[Channel][ImgMode.Pipe].Id1;
        Id2 = VignetteSelectParam[Channel][ImgMode.Pipe].Id2;
        Blend = VignetteSelectParam[Channel][ImgMode.Pipe].Blend;
        VignetteLumaStrength = VignetteSelectParam[Channel][ImgMode.Pipe].VignetteLumaStrength;
        VignetteChromaStrength = VignetteSelectParam[Channel][ImgMode.Pipe].VignetteChromaStrength;
    }
    if (AppVignetteControl.Debug) {
        AmbaPrint("Enable = %d Channel = %d Id1 = %d, id2 = %d Blend = %d VignetteLumaStrength = %d  VignetteChromaStrength = %d", Enable,Channel,Id1,Id2,Blend,VignetteLumaStrength,VignetteChromaStrength);
    }

    VgmConfig.Id[0] = AppLib_CalibTableMapping(Channel, Id1);
    VgmConfig.Id[1]  = AppLib_CalibTableMapping(Channel, Id2);
    Blend = Blend>>8; //normalize to 256
    if (Enable) {
        if (Id1 == Id2) {
            BlendRatio = 0;
        } else {
            BlendRatio = Blend;
        }
        EnValue = Enable;
    } else {
        BlendRatio = 0;
        EnValue = 0;//disable vignette Enable
    }

    VgmConfig.NumVignetteTable = MAX_VIGNETTE_GAIN_TABLE_COUNT;
    if (VignetteLumaStrength > MAX_VIG_RATIO) {
        VignetteLumaStrength = MAX_VIG_RATIO;
    }
    if (VignetteChromaStrength > MAX_VIG_RATIO) {
        VignetteChromaStrength = MAX_VIG_RATIO;
    }
    VgmConfig.BlendRatio = BlendRatio;
    for (i = 0; i < MAX_VIGNETTE_GAIN_TABLE_COUNT; i++) {
        VgmConfig.VignetteStorageAddress[i] = AppVignetteControl.GainTable[i];//uncompress data
    }
    if(((Enable&VIG_UPDATE_ALL) == VIG_UPDATE_ALL)\
        ||(Id1 != VignetteSelectParam[Channel][ImgMode.Pipe].Id1) \
        ||(Id2 != VignetteSelectParam[Channel][ImgMode.Pipe].Id2) \
        ||(Blend != VignetteSelectParam[Channel][ImgMode.Pipe].Blend)) {
    ApplibCalibVignette_ConfigVignette(Channel, &VgmConfig);
        Enable &= VIG_DEBUG_TABLE;
    }
    AmbaSensor_GetStatus(AppEncChannel, &SensorStatus); //need to add channel parameter
    FlipMirror = SensorStatus.ModeInfo.Mode.Bits.VerticalFlip;
    if(FlipMirror){//FlipMirror information should be provided by sensor driver or APP, CAL_ROTATE_180_HORZ_FLIP is only used for Imx117.
        AppLib_CalibRotateMap(VignetteInfo[Channel]->CalibVignetteInfo.pVignetteRedGain,VIGNETTE_FULL_VIEW_MAX_WIDTH, VIGNETTE_FULL_VIEW_MAX_HEIGHT, FlipMirror);
        AppLib_CalibRotateMap(VignetteInfo[Channel]->CalibVignetteInfo.pVignetteGreenEvenGain,VIGNETTE_FULL_VIEW_MAX_WIDTH, VIGNETTE_FULL_VIEW_MAX_HEIGHT, FlipMirror);
        AppLib_CalibRotateMap(VignetteInfo[Channel]->CalibVignetteInfo.pVignetteGreenOddGain,VIGNETTE_FULL_VIEW_MAX_WIDTH, VIGNETTE_FULL_VIEW_MAX_HEIGHT, FlipMirror);
        AppLib_CalibRotateMap(VignetteInfo[Channel]->CalibVignetteInfo.pVignetteBlueGain,VIGNETTE_FULL_VIEW_MAX_WIDTH, VIGNETTE_FULL_VIEW_MAX_HEIGHT, FlipMirror);
    }

    VignetteInfo[Channel]->CurrentVinSensorGeo = CalcWarp.VinSensorGeo;
    VignetteInfo[Channel]->CalibVignetteInfo.TableWidth  = VgmConfig.VignetteStorageAddress[VgmConfig.Id[0]]->TableWidth;
    VignetteInfo[Channel]->CalibVignetteInfo.TableHeight = VgmConfig.VignetteStorageAddress[VgmConfig.Id[0]]->TableHeight;
    VignetteInfo[Channel]->CalibVignetteInfo.Version = VgmConfig.VignetteStorageAddress[VgmConfig.Id[0]]->Version;
    VignetteInfo[Channel]->VigStrength = VignetteLumaStrength;
    VignetteInfo[Channel]->ChromaRatio = VignetteChromaStrength;
    VignetteInfo[Channel]->VigStrengthEffectMode = VIGNETTE_KeepRatioMode;
    VignetteInfo[Channel]->Enb = (EnValue&VgmConfig.VignetteStorageAddress[VgmConfig.Id[0]]->Enable);
    //save select parameter;
    VignetteSelectParam[Channel][ImgMode.Pipe].Enable = Enable;
    VignetteSelectParam[Channel][ImgMode.Pipe].Channel = Channel;
    VignetteSelectParam[Channel][ImgMode.Pipe].Id1 = Id1;
    VignetteSelectParam[Channel][ImgMode.Pipe].Id2 = Id2;
    VignetteSelectParam[Channel][ImgMode.Pipe].Blend = Blend;
    VignetteSelectParam[Channel][ImgMode.Pipe].VignetteChromaStrength = VignetteChromaStrength;
    VignetteSelectParam[Channel][ImgMode.Pipe].VignetteLumaStrength = VignetteLumaStrength;
    SelectVignetteTable[Channel] = 1;

    //return NG when get a null sensor information.
    if ((CalcWarp.VinSensorGeo.Width == 0) || (CalcWarp.VinSensorGeo.Height == 0) \
            ||(CalcWarp.VinSensorGeo.HSubSample.FactorNum == 0)||(CalcWarp.VinSensorGeo.HSubSample.FactorDen == 0)) {
        return NG;
    }
    if (AppVignetteControl.Debug) {
        AmbaPrint("VignetteInfo[Channel].Enb = %d",VignetteInfo[Channel]->Enb);
        AmbaPrint("VignetteInfo[Channel].GainShift = %d",VignetteInfo[Channel]->GainShift);
        AmbaPrint("VignetteInfo[Channel].VigStrength = %d",VignetteInfo[Channel]->VigStrength);
        AmbaPrint("TableWidth = %d",VignetteInfo[Channel]->CalibVignetteInfo.TableWidth);
        AmbaPrint("TableHeight = %d",VignetteInfo[Channel]->CalibVignetteInfo.TableHeight);
        AmbaPrint("Calibration Width = %d",VignetteInfo[Channel]->CalibVignetteInfo.CalibVinSensorGeo.Width);
        AmbaPrint("Calibration Height = %d",VignetteInfo[Channel]->CalibVignetteInfo.CalibVinSensorGeo.Height);
        AmbaPrint("Calibration StartX = %d",VignetteInfo[Channel]->CalibVignetteInfo.CalibVinSensorGeo.StartX);
        AmbaPrint("Calibration StartY = %d",VignetteInfo[Channel]->CalibVignetteInfo.CalibVinSensorGeo.StartY);
        AmbaPrint("Current Width = %d",VignetteInfo[Channel]->CurrentVinSensorGeo.Width);
        AmbaPrint("Current Height = %d",VignetteInfo[Channel]->CurrentVinSensorGeo.Height);
        AmbaPrint("Current StartX = %d",VignetteInfo[Channel]->CurrentVinSensorGeo.StartX);
        AmbaPrint("Current StartY = %d",VignetteInfo[Channel]->CurrentVinSensorGeo.StartY);
        AmbaPrint("HSubSample.FactorNum = %d",VignetteInfo[Channel]->CurrentVinSensorGeo.HSubSample.FactorNum);
        AmbaPrint("HSubSample.FactorDen = %d",VignetteInfo[Channel]->CurrentVinSensorGeo.HSubSample.FactorDen);
        AmbaPrint("VSubSample.FactorNum = %d",VignetteInfo[Channel]->CurrentVinSensorGeo.VSubSample.FactorNum);
        AmbaPrint("VSubSample.FactorDen = %d",VignetteInfo[Channel]->CurrentVinSensorGeo.VSubSample.FactorDen);
    }

    if ((Enable&VIG_DEBUG_TABLE) == VIG_DEBUG_TABLE ) {
        char outtext[VIGNETTE_FULL_VIEW_MAX_WIDTH * 5] = {0};

        char Rawfn[64];
        char Tmpext[1][5] = {{'.','t','x','t','\0'}};
        char Tmpfn[5][20] = {{'C',':','\\','v','i','g','c','a','l','_','g','a','i','n','_','R','\0'},
            {'C',':','\\','v','i','g','c','a','l','_','g','a','i','n','_','G','e','\0'},
            {'C',':','\\','v','i','g','c','a','l','_','g','a','i','n','_','G','o','\0'},
            {'C',':','\\','v','i','g','c','a','l','_','g','a','i','n','_','B','\0'},
            {'C',':','\\','v','i','g','c','a','l','_','i','n','f','o','\0'}
        };
        int Strlen, Gaincnter, i, j;
        AMBA_FS_FILE *Fp;

        //Vignette Map Text
        strcpy(Rawfn, Tmpfn[0]);
        Rawfn[0] = AppLib_CalibGetDriverLetter();
        //num_to_text(Rawfn ,EnterCount ,0);
        strcat(Rawfn, Tmpext[0]);
        Fp = AmbaFS_fopen(Rawfn, "w");
        Gaincnter = 0;
        for (j=0; j<VIGNETTE_FULL_VIEW_MAX_HEIGHT; j++) {
            for (i=0; i<VIGNETTE_FULL_VIEW_MAX_WIDTH; i++) {
                Strlen = sprintf(outtext,"%5d",VignetteInfo[Channel]->CalibVignetteInfo.pVignetteRedGain[Gaincnter]);
                AmbaFS_fwrite(outtext, 1, Strlen, Fp);
                Gaincnter++;
            }
            Strlen = sprintf(outtext,"\n");
            AmbaFS_fwrite(outtext,1,Strlen,Fp);
        }
        AmbaFS_fclose(Fp);
        strcpy(Rawfn, Tmpfn[1]);
        Rawfn[0] = AppLib_CalibGetDriverLetter();
        //num_to_text(Rawfn ,EnterCount ,0);
        strcat(Rawfn, Tmpext[0]);
        Fp = AmbaFS_fopen(Rawfn, "w");
        Gaincnter = 0;
        for (j=0; j<VIGNETTE_FULL_VIEW_MAX_HEIGHT; j++) {
            for (i=0; i<VIGNETTE_FULL_VIEW_MAX_WIDTH; i++) {
                Strlen = sprintf(outtext,"%5d",VignetteInfo[Channel]->CalibVignetteInfo.pVignetteGreenEvenGain[Gaincnter]);
                AmbaFS_fwrite(outtext, 1, Strlen, Fp);
                Gaincnter++;
            }
            Strlen = sprintf(outtext,"\n");
            AmbaFS_fwrite(outtext,1,Strlen,Fp);
        }
        AmbaFS_fclose(Fp);
        strcpy(Rawfn, Tmpfn[2]);
        Rawfn[0] = AppLib_CalibGetDriverLetter();
        //num_to_text(Rawfn ,EnterCount ,0);
        strcat(Rawfn, Tmpext[0]);
        Fp = AmbaFS_fopen(Rawfn, "w");
        Gaincnter = 0;
        for (j=0; j<VIGNETTE_FULL_VIEW_MAX_HEIGHT; j++) {
            for (i=0; i<VIGNETTE_FULL_VIEW_MAX_WIDTH; i++) {
                Strlen = sprintf(outtext,"%5d",VignetteInfo[Channel]->CalibVignetteInfo.pVignetteGreenOddGain[Gaincnter]);
                AmbaFS_fwrite(outtext, 1, Strlen, Fp);
                Gaincnter++;
            }
            Strlen = sprintf(outtext,"\n");
            AmbaFS_fwrite(outtext,1,Strlen,Fp);
        }
        AmbaFS_fclose(Fp);
        strcpy(Rawfn, Tmpfn[3]);
        Rawfn[0] = AppLib_CalibGetDriverLetter();
        //num_to_text(Rawfn ,EnterCount ,0);
        strcat(Rawfn, Tmpext[0]);
        Fp = AmbaFS_fopen(Rawfn, "w");
        Gaincnter = 0;
        for (j=0; j<VIGNETTE_FULL_VIEW_MAX_HEIGHT; j++) {
            for (i=0; i<VIGNETTE_FULL_VIEW_MAX_WIDTH; i++) {
                Strlen = sprintf(outtext,"%5d",VignetteInfo[Channel]->CalibVignetteInfo.pVignetteBlueGain[Gaincnter]);
                AmbaFS_fwrite(outtext, 1, Strlen, Fp);
                Gaincnter++;
            }
            Strlen = sprintf(outtext,"\n");
            AmbaFS_fwrite(outtext,1,Strlen,Fp);
        }
        AmbaFS_fclose(Fp);


        //Vignette Map Geometry + GainShift
        strcpy(Rawfn, Tmpfn[4]);
        Rawfn[0] = AppLib_CalibGetDriverLetter();
        //num_to_text(Rawfn ,EnterCount ,0);
        strcat(Rawfn, Tmpext[1]);
        Fp = AmbaFS_fopen(Rawfn, "w");
        Strlen = sprintf(outtext,"Vignette geometry startX: %d\n",VignetteInfo[Channel]->CalibVignetteInfo.CalibVinSensorGeo.StartX);
        AmbaFS_fwrite(outtext, 1, Strlen, Fp);
        Strlen = sprintf(outtext,"Vignette geometry startY: %d\n",VignetteInfo[Channel]->CalibVignetteInfo.CalibVinSensorGeo.StartY);
        AmbaFS_fwrite(outtext, 1, Strlen, Fp);
        Strlen = sprintf(outtext,"Vignette geometry Width: %d\n",VignetteInfo[Channel]->CalibVignetteInfo.CalibVinSensorGeo.Width);
        AmbaFS_fwrite(outtext, 1, Strlen, Fp);
        Strlen = sprintf(outtext,"Vignette geometry height: %d\n",VignetteInfo[Channel]->CalibVignetteInfo.CalibVinSensorGeo.Height);
        AmbaFS_fwrite(outtext, 1, Strlen, Fp);
        Strlen = sprintf(outtext,"Vignette gain shift: %d\n",VignetteInfo[Channel]->GainShift);
        AmbaFS_fwrite(outtext, 1, Strlen, Fp);
        AmbaFS_fclose(Fp);

    }

    if (ImgMode.Pipe == AMBA_DSP_IMG_PIPE_VIDEO) {
        //set for low iso mode
        ImgMode.AlgoMode = AMBA_DSP_IMG_ALGO_MODE_LISO;
        ImgMode.ContextId = AmbaImgSchdlr_GetIsoCtxIndex(Channel,ImgMode.AlgoMode);
        Rval = AmbaDSP_ImgCalcVignetteCompensation(&ImgMode, VignetteInfo[Channel]);
        if (AppVignetteControl.Debug) {
            AmbaPrint("vignette IK return value = %d",Rval);
        }
        AmbaDSP_ImgSetVignetteCompensation(&ImgMode);

/*        //set for high iso mode
        ImgMode.AlgoMode = AMBA_DSP_IMG_ALGO_MODE_HISO;
        ImgMode.ContextId = AmbaImgSchdlr_GetIsoCtxIndex(Channel,ImgMode.AlgoMode);
        Rval = AmbaDSP_ImgCalcVignetteCompensation(&ImgMode, VignetteInfo[Channel]);
        if (AppVignetteControl.Debug) {
            AmbaPrint("vignette IK return value = %d",Rval);
        }
        AmbaDSP_ImgSetVignetteCompensation(&ImgMode);
*/        
    } else {    //set for still mode
        //set for low iso mode
        ImgMode.AlgoMode = AMBA_DSP_IMG_ALGO_MODE_LISO;
        ImgMode.ContextId = 0;
        Rval = AmbaDSP_ImgCalcVignetteCompensation(&ImgMode, VignetteInfo[Channel]);
        if (AppVignetteControl.Debug) {
            AmbaPrint("vignette IK return value = %d",Rval);
        }
        AmbaDSP_ImgSetVignetteCompensation(&ImgMode);
    }
    return Rval;
}


/**
 *  @brief update vignette map
 *
 *  update vignette map
 *
 */
void AppLibCalibVignette_MapUpdate(void)
{
    AppLibCalibVignette_SelectVignetteTable(VIG_UPDATE_VIN_INFO, 0, 0, 0, 0, 0, 0);
}

/**
 *  @brief initialize the vignette calibration
 *
 *  initialize the vignette calibration
 *
 *  @param [in]CalObj calibration object status
 *
 *  @return 0 success, -1 failure
 */
int AppLibCalibVignette_Init(Cal_Obj_s *CalObj)
{
    int i = 0;
    UINT8 *VignetteAddress = CalObj->DramShadow;
    int BlendRatio = 0;
    static UINT8 VignetteInitFlag = 0;
    UINT32 VignetteLumaStrength,VignetteChromaStrength;
    UINT8 Channel;

    if (VignetteInitFlag == 0) {
        memset(&AppVignetteControl,0,sizeof(Vignette_Control_s));
        VignetteInitFlag = 1;
    }

    AppVignetteControl.Enable = VignetteAddress[CAL_VIGNETTE_ENABLE];
    CAL_PRINT("AppVignetteControl.Enable %d ",AppVignetteControl.Enable);
    AppVignetteControl.GainTableCount = VignetteAddress[CAL_VIGNETTE_TABLE_COUNT];

    for (i=0; i<MAX_VIGNETTE_GAIN_TABLE_COUNT; i++) {
        AppVignetteControl.GainTable[i]=(Vignette_Pack_Storage_s *)&VignetteAddress[CAL_VIGNETTE_TABLE(i)];
    }
    if (AppVignetteControl.GainTable[0]->Enable == 1) {
        VignetteLumaStrength = 65536;
        VignetteChromaStrength = 65536;

        Channel = 0;
        BlendRatio = 0;
        AppLibCalibVignette_SelectVignetteTable(VIG_UPDATE_ALL, Channel, 0, 0, BlendRatio,VignetteLumaStrength,VignetteChromaStrength);
    }
    AppLibCalibAdjust_Init();
    AppLibCalibAdjust_SetControlEnable(1);
    return 0;
}

/**
 *  @brief load parameters for vignette calibration
 *
 *  load parameters for vignette calibration
 *
 *  @param [in]Argc number of input parameters
 *  @param [in]Argv value of input parameters
 *  @param [in]CalSite calibration site status
 *  @param [in]CalObj calibration object status
 *  @param [in]VignetteCalParam input parameters for vignette calibration
 *  @param [out]VignetteCalInfo information for vignette calibration
 *  @param [out]OutputStr debug message for this function
 *
 *  @return 0 success, -1 failure
 */
int AppLibCalibVignette_LoadVignetteScript(int Argc, char *Argv[], char *OutputStr, Cal_Stie_Status_s *CalSite, Cal_Obj_s *CalObj,Vignette_Cal_Param_s *VignetteCalParam,Vignette_Cal_Info_s *VignetteCalInfo)
{
    INT16 i=0;
    int MaxCalTableId;

    if (strcmp(Argv[1], "LOAD_RAW") == 0) {
        if (Argc != 12) {
            sprintf(OutputStr,"Vignette calibration NG:LOAD_RAW witdh Height GainShift Threshold debug compensation_ratio BlackLevel DarkCornerX DarkCornerY Bayer");
            return VIG_CALIB_ERROR_LOAD_RAW_NG;
        }
    } else {
        if (Argc != VIGNETTE_CAL_APP_ARGC_NUM) {
            sprintf(OutputStr,"Vignette calibration NG: calibration paramter number(%d) != %d"
                    , Argc - 1, VIGNETTE_CAL_APP_ARGC_NUM- 1);
            return VIG_CALIB_ERROR_LOAD_NUM;
        }
    }
    /* Parse Input parameters*/
    if (strcmp(Argv[1], "LOAD_RAW") == 0) { //load raw image from SD card
        VignetteCalInfo->Width  = atoi(Argv[2]);
        VignetteCalInfo->Height     = atoi(Argv[3]);
        VignetteCalInfo->GainShift = atoi(Argv[4]);
        VignetteCalInfo->Threshold = atoi(Argv[5]);
        VignetteCalInfo->CompensateMethod = atoi(Argv[6]);
        VignetteCalInfo->CompensateRatio    = atoi(Argv[7]);
        VignetteCalInfo->BlackLevel.BlackR = atoi(Argv[8]);
        VignetteCalInfo->BlackLevel.BlackGr = atoi(Argv[8]);
        VignetteCalInfo->BlackLevel.BlackGb = atoi(Argv[8]);
        VignetteCalInfo->BlackLevel.BlackB = atoi(Argv[8]);
        VignetteCalInfo->DarkCornerSizeX = atoi(Argv[9]);
        VignetteCalInfo->DarkCornerSizeY = atoi(Argv[10]);
        VignetteCalInfo->Bayer = atoi(Argv[11]);
        if (VignetteCalInfo->GainShift == 0) {
            if (VignetteCalInfo->Threshold > (8192)) {
                VignetteCalInfo->Threshold = 8192;
            }
        } else if (VignetteCalInfo->GainShift == 1) {
            if (VignetteCalInfo->Threshold > (4096)) {
                VignetteCalInfo->Threshold = 4096;
            }
        } else if (VignetteCalInfo->GainShift == 2) {
            if (VignetteCalInfo->Threshold > (2048)) {
                VignetteCalInfo->Threshold = 2048;
            }
        } else if (VignetteCalInfo->GainShift == 3) {
            if (VignetteCalInfo->Threshold > (1024)) {
                VignetteCalInfo->Threshold = 1024;
            }
        } else if (VignetteCalInfo->GainShift == 255) {
            if (VignetteCalInfo->Threshold > (8192)) {
                VignetteCalInfo->Threshold = 8192;
            }
        } else {
            sprintf(OutputStr,"Vignette calibration NG: Unknown gain shift value(%d),it should be 0/1/2/3/255 ",VignetteCalInfo->GainShift);
            return VIG_CALIB_ERROR_GAIN_SHIFT;
        }
        return VIG_CALIB_OK;
    }   else { //
        VignetteCalParam->CalSubSiteId = (UINT8) atoi(Argv[1]);
        VignetteCalParam->Channel      = (UINT8) atoi(Argv[2]);
        VignetteCalParam->DarkCornerSizeX  = (UINT8) atoi(Argv[3]);
        VignetteCalParam->DarkCornerSizeY  = (UINT8) atoi(Argv[4]);
        VignetteCalParam->CompensateMethod = (UINT8) atoi(Argv[5]);
        VignetteCalParam->SaveFileFlagAPP = (UINT8) atoi(Argv[6]);
        VignetteCalParam->LookupShift = (UINT8) atoi(Argv[7]);
        VignetteCalParam->Threshold = (UINT16) atoi(Argv[8]);
        VignetteCalParam->CompensateRatio = (UINT16) atoi(Argv[9]);
        VignetteCalParam->AeTarget = (INT16) atoi(Argv[10]);
        VignetteCalParam->FlickerMode = (UINT8) atoi(Argv[11]);
        VignetteCalParam->ManualAgc = (float) atof(Argv[12]);
        VignetteCalParam->InverseShutterTime = (float) atof(Argv[13]);
        VignetteCalParam->ManualIrisIdx = (UINT16) atoi(Argv[14]);
        VignetteCalParam->ManualBlackLevel = (INT16) atoi(Argv[15]);

        VignetteCalParam->CalStepStartOffset = (UINT8) atoi(Argv[16]);
        VignetteCalParam->NumCalSteps = (UINT8) atoi(Argv[17]);
        for (i=0; i<MAX_VIGNETTE_GAIN_TABLE_COUNT; i++) {
            VignetteCalParam->CalZoomSteps[i] = (UINT32) atoi(Argv[18+i]);
        }
    }

    MaxCalTableId = VignetteCalParam->NumCalSteps + VignetteCalParam->CalStepStartOffset - 1;
    if (VignetteCalParam->CalSubSiteId != VignetteCalParam->CalStepStartOffset) {
        sprintf(OutputStr,"Vignette calibration NG: CalSubSiteId (%d)!= CalStepStartOffset(%d)",VignetteCalParam->CalSubSiteId,VignetteCalParam->CalStepStartOffset);
        return VIG_CALIB_ERROR_PARAM_MISMATCH;
    }

    if (MaxCalTableId >= MAX_VIGNETTE_GAIN_TABLE_COUNT) {
        sprintf(OutputStr,"Vignette calibration NG: cal_start_id (%d) + cal_step_num (%d) exceeds limit (%d)"
                , VignetteCalParam->CalStepStartOffset
                , VignetteCalParam->NumCalSteps
                , MAX_VIGNETTE_NAND_TABLE_COUNT);
        return VIG_CALIB_ERROR_PARAM_EXCEED;
    }

    for (i = 1; i<VignetteCalParam->NumCalSteps; i++ ) {
        if ((VignetteCalParam->CalZoomSteps[i]) < (VignetteCalParam->CalZoomSteps[i-1])) {
            sprintf(OutputStr,"Vignette calibration NG: vignette calibration ozoom step must be increasing");
            AmbaPrintColor(RED,"%s",OutputStr);
            return VIG_CALIB_ERROR_PARAM_NG;
        }
    }

    if (CAL_VIGNETTE_TABLE(MaxCalTableId + 1) > CAL_VIGNETTE_SIZE) {
        sprintf(OutputStr,"Vignette calibration NG: calibration data require more memory than app allocated(%d)!",CAL_VIGNETTE_SIZE);
        return VIG_CALIB_ERROR_MEMORY_ALOC;
    }

    if (VignetteCalParam->FlickerMode != 50 && VignetteCalParam->FlickerMode != 60) {
        sprintf(OutputStr,"Vignette calibration NG: vignette calibration flicker mode (%d) must be 50 or 60", VignetteCalParam->FlickerMode);
        return VIG_CALIB_ERROR_FLICKER;
    }

    AmbaPrint("[VIG_CAL PARAM] CalSubSiteId: %d", VignetteCalParam->CalSubSiteId);
    AmbaPrint("[VIG_CAL PARAM] Channel: %d", VignetteCalParam->Channel);
    AmbaPrint("[VIG_CAL PARAM] DarkCornerSizeX: %d", VignetteCalParam->DarkCornerSizeX);
    AmbaPrint("[VIG_CAL PARAM] DarkCornerSizeY: %d", VignetteCalParam->DarkCornerSizeY);
    AmbaPrint("[VIG_CAL PARAM] CompensateMethod: %d", VignetteCalParam->CompensateMethod);
    AmbaPrint("[VIG_CAL PARAM] SaveFileFlagAPP: %d", VignetteCalParam->SaveFileFlagAPP);
    AmbaPrint("[VIG_CAL PARAM] LookupShift: %d", VignetteCalParam->LookupShift);
    AmbaPrint("[VIG_CAL PARAM] Threshold: %d", VignetteCalParam->Threshold);
    AmbaPrint("[VIG_CAL PARAM] CompensateRatio: %d", VignetteCalParam->CompensateRatio);
    AmbaPrint("[VIG_CAL PARAM] AeTarget: %d", VignetteCalParam->AeTarget);
    AmbaPrint("[VIG_CAL PARAM] FlickerMode: %d", VignetteCalParam->FlickerMode);
    AmbaPrint("[VIG_CAL PARAM] ManualAgc: %f", VignetteCalParam->ManualAgc);
    AmbaPrint("[VIG_CAL PARAM] InverseShutterTime: %f", VignetteCalParam->InverseShutterTime);
    AmbaPrint("[VIG_CAL PARAM] ManualIrisIdx: %d", VignetteCalParam->ManualIrisIdx);
    AmbaPrint("[VIG_CAL PARAM] ManualBlackLevel: %d", VignetteCalParam->ManualBlackLevel);
    AmbaPrint("[VIG_CAL PARAM] CalStepStartOffset: %d", VignetteCalParam->CalStepStartOffset);
    AmbaPrint("[VIG_CAL PARAM] NumCalSteps: %d", VignetteCalParam->NumCalSteps);
    for (i = 0; i<VignetteCalParam->NumCalSteps; i++ ) {
        AmbaPrint("[VIG_CAL PARAM] ozoom_step[%d]: %d",i,VignetteCalParam->CalZoomSteps[i]);
    }

    return VIG_CALIB_OK;
}

/**
 *  @brief set 3A information to vignette calibraton
 *
 *  set 3A information to vignette calibraton
 *
 *  @param [in]CalSite calibration site status
 *  @param [in]VignetteCalParam input parameters for vignette calibration
 *  @param [in]CalBlackLevel black level for vignette calibration
 *  @param [in]CalAeInfo AE information for vignette calibration
 *  @param [out]OutputStr debug message for this function
 *
 *  @return 0 success, -1 failure
 */
int AppLibCalibVignette_CalVignette3ASetting(char *OutputStr, Cal_Stie_Status_s *CalSite,Vignette_Cal_Param_s *VignetteCalParam,Vignette_Cal_Info_s *VignetteCalInfo,AMBA_AE_INFO_s *CalAeInfo)
{
    float CurShtTime,CurAgcGain;
    UINT16 CurIrisIdx;//max_sht_idx
    AMBA_DSP_IMG_MODE_CFG_s Mode;
    AMBA_DSP_IMG_BLACK_CORRECTION_s BlackCorr;
    AMBA_3A_OP_INFO_s  AaaOpInfo = {DISABLE, DISABLE, DISABLE, DISABLE};
    AE_CONTROL_s AeCtrlInfo;
    UINT8 Sec;

    memset(&Mode, 0, sizeof(AMBA_DSP_IMG_MODE_CFG_s));
    if (VignetteCalParam->AeTarget > 0) { //Use AE to compute exposure setting

        /* Auto set the black level */
        AmbaDSP_ImgGetStaticBlackLevel(&Mode, &BlackCorr);
        AmbaPrint(  " black level: %d %d %d %d\n",
                    (int)BlackCorr.BlackR,
                    (int)BlackCorr.BlackGr,
                    (int)BlackCorr.BlackGb,
                    (int)BlackCorr.BlackB
                 );
        VignetteCalInfo->BlackLevel.BlackR  = BlackCorr.BlackR;
        VignetteCalInfo->BlackLevel.BlackGr = BlackCorr.BlackGr;
        VignetteCalInfo->BlackLevel.BlackGb = BlackCorr.BlackGb;
        VignetteCalInfo->BlackLevel.BlackB  = BlackCorr.BlackB;

        /*open calibration AE to tune lighting*/
        AmbaImg_Proc_Cmd(MW_IP_GET_MULTI_AE_CONTROL_CAPABILITY, VignetteCalParam->Channel, (UINT32)&AeCtrlInfo, 0);
        AeCtrlInfo.DefAeTarget = VignetteCalParam->AeTarget;
        AmbaImg_Proc_Cmd(MW_IP_SET_MULTI_AE_CONTROL_CAPABILITY, VignetteCalParam->Channel, (UINT32)&AeCtrlInfo, 0);

        for (Sec =5; Sec>0; Sec--) {
            AmbaPrint("Vignette Auto AE delay time : %d second", Sec);
            AmbaKAL_TaskSleep(1000);
        }
        AmbaImg_Proc_Cmd(MW_IP_GET_AE_INFO, VignetteCalParam->Channel, IP_MODE_VIDEO, (UINT32)CalAeInfo);
        if ((CalAeInfo->ShutterTime < 0.0083) && (VignetteCalParam->FlickerMode == 60 )) { // the 0.0083 = 1/120 sec
            AmbaPrint("CalAeInfo. ShutterTime : %f. The shutterTime is too short, please increase the AE target or decrease strength the light source ",CalAeInfo->ShutterTime);
            sprintf(OutputStr,"CalAeInfo. ShutterTime : %f. The shutterTime is too short, please increase the AE target or decrease strength the light source",CalAeInfo->ShutterTime);
            return VIG_CALIB_ERROR_SHUTTERTIME_TOO_SHORT;
        } else if ((CalAeInfo->ShutterTime < 0.01) && (VignetteCalParam->FlickerMode == 50 )) { // the 0.01 = 1/100 sec
            AmbaPrint("CalAeInfo. ShutterTime : %f. The shutterTime is too short, please increase the AE target or decrease strength the light source",CalAeInfo->ShutterTime);
            sprintf(OutputStr,"CalAeInfo. ShutterTime : %f. The shutterTime is too short, please increase the AE target or decrease strength the light source",CalAeInfo->ShutterTime);
            return VIG_CALIB_ERROR_SHUTTERTIME_TOO_SHORT;
        }
    } else {
        if (VignetteCalParam->ManualBlackLevel >0) {
            /* Auto set the black level */
            AmbaDSP_ImgGetStaticBlackLevel(&Mode, &BlackCorr);
            AmbaPrint(  " black level: %d %d %d %d\n",
                        (int)BlackCorr.BlackR,
                        (int)BlackCorr.BlackGr,
                        (int)BlackCorr.BlackGb,
                        (int)BlackCorr.BlackB
                     );
            VignetteCalInfo->BlackLevel.BlackR  = BlackCorr.BlackR;
            VignetteCalInfo->BlackLevel.BlackGr = BlackCorr.BlackGr;
            VignetteCalInfo->BlackLevel.BlackGb = BlackCorr.BlackGb;
            VignetteCalInfo->BlackLevel.BlackB  = BlackCorr.BlackB;
        } else {
            VignetteCalInfo->BlackLevel.BlackR  = VignetteCalParam->ManualBlackLevel;
            VignetteCalInfo->BlackLevel.BlackGr = VignetteCalParam->ManualBlackLevel;
            VignetteCalInfo->BlackLevel.BlackGb = VignetteCalParam->ManualBlackLevel;
            VignetteCalInfo->BlackLevel.BlackB  = VignetteCalParam->ManualBlackLevel;
        }
        //turn off 3A
        AaaOpInfo.AdjOp = 0;
        AaaOpInfo.AeOp = 0;
        AaaOpInfo.AwbOp = 0;
        AaaOpInfo.AfOp = 0;
        AmbaImg_Proc_Cmd(MW_IP_SET_AAA_OP_INFO, VignetteCalParam->Channel, (UINT32)&AaaOpInfo, 0); //
        AmbaPrint("disable 3A");

        CalAeInfo->ShutterTime = 1.0/VignetteCalParam->InverseShutterTime;
        CalAeInfo->AgcGain     = VignetteCalParam->ManualAgc;
        CalAeInfo->IrisIndex    = VignetteCalParam->ManualIrisIdx;
        CalAeInfo->Dgain         = WB_UNIT_GAIN;
        CalAeInfo->Flash          = 0;
        AmbaImg_Proc_Cmd(MW_IP_SET_AE_INFO, VignetteCalParam->Channel, IP_MODE_STILL, (UINT32)CalAeInfo);

        CurShtTime = CalAeInfo->ShutterTime;
        CurAgcGain = CalAeInfo->AgcGain;
        CurIrisIdx = CalAeInfo->IrisIndex;
        AmbaPrint("Vignette calibration AE - shutter index:%f agc gain:%f iris index: %d",
                  CurShtTime,CurAgcGain,CurIrisIdx);
    }

    return VIG_CALIB_OK;
}


/**
 *  @brief set zoom control for vignette calibraton
 *
 *  set zoom control for vignette calibraton
 *
 *  @param [in]ZoomStepIndex optical zoom step index
 *  @param [in]VignetteCalParam input parameters for vignette calibration
 *  @param [in]CalBlackLevel black level for vignette calibration
 *  @param [in]CalObj calibration oject status
 *  @param [out]VignetteCalInfo information for vignette calibration
 *  @param [out]OutputStr debug message for this function
 *
 *  @return 0 success, -1 failure
 */
int AppLibCalibVignette_CalVignetteZoomControl(char *OutputStr,int ZoomStepIndex,Vignette_Cal_Param_s *VignetteCalParam,Vignette_Cal_Info_s *VignetteCalInfo,AMBA_DSP_IMG_BLACK_CORRECTION_s *CalBlackLevel,Cal_Obj_s *CalObj)
{
    int CalTableId = 0;
    UINT8  *VignetteAddress = CalObj->DramShadow;

    VignetteAddress[CAL_VIGNETTE_ENABLE] = 1;
    VignetteAddress[CAL_VIGNETTE_TABLE_COUNT] = VignetteCalParam->NumCalSteps + VignetteCalParam->CalStepStartOffset;
#if 0
    lens_get_info(&lens_info);
    zoom_speed  = lens_info.zoom_max_speed/2;
    focus_speed = lens_info.focus_max_speed;
    if ((lens_info.me_capability & MECHAN_ZOOM) == MECHAN_ZOOM) {
        zoom_status_t oz_status;
        UINT16 target_zp = VignetteCalParam->CalZoomSteps[ZoomStepIndex];
        lens_get_zoom_status(&oz_status);
        if (target_zp > oz_status.zp_value) {
            lens_zoom_in(zoom_speed, target_zp - oz_status.zp_value);
        } else if (target_zp < oz_status.zp_value) {
            lens_zoom_in(zoom_speed, oz_status.zp_value - target_zp);
        }
        lens_get_zoom_status(&oz_status);
        while (oz_status.zm_action_sts != 0) { //wait zoom stop
            dly_tsk(10);
            lens_get_zoom_status(&oz_status);
        }
        //Move focus to near to make sure calibration condition is the same
        lens_get_focus_status(&focus_status);
        if (focus_status.near_pt > focus_status.focus_pt) {
            lens_focus_near(focus_speed, focus_status.near_pt - focus_status.focus_pt);
        } else if (focus_status.near_pt < focus_status.focus_pt) {
            lens_focus_far(focus_speed, focus_status.focus_pt - focus_status.near_pt);
        }
        do {
            dly_tsk(10);
            lens_get_focus_status(&focus_status);
        } while (focus_status.fs_action_sts != FS_STOP);
    }
#endif

    CalTableId = AppLib_CalibTableMapping(VignetteCalParam->Channel, (ZoomStepIndex + VignetteCalParam->CalStepStartOffset));
    VignetteCalInfo->Threshold = VignetteCalParam->Threshold;
    VignetteCalInfo->CompensateMethod = VignetteCalParam->CompensateMethod;/**< Method for compensating, 0x80: luma only, 0x40:4 channel compensate with the same gain */
    VignetteCalInfo->GainShift = VignetteCalParam->LookupShift; //0: 3.7 , 1: 2.8, 2: 1.9, 3: 0.10, 255: auto select
    VignetteCalInfo->GainTable = (Vignette_Pack_Storage_s *)&VignetteAddress[CAL_VIGNETTE_TABLE(CalTableId)];
    VignetteCalInfo->GainTable->CalInfo[0] = VignetteCalParam->NumCalSteps;// 1:total_setp_num
    VignetteCalInfo->GainTable->CalInfo[1] = VignetteCalInfo->CompensateMethod;//calibration method
    VignetteCalInfo->GainTable->ZoomStep = VignetteCalParam->CalZoomSteps[ZoomStepIndex];//CalInfo 0: zp_table  1:total_setp_num
    VignetteCalInfo->GainTable->Channel  = VignetteCalParam->Channel;
    VignetteCalInfo->GainTable->SiteId   = (ZoomStepIndex + VignetteCalParam->CalStepStartOffset);
    VignetteCalInfo->DarkCornerSizeX = VignetteCalParam->DarkCornerSizeX;
    VignetteCalInfo->DarkCornerSizeY = VignetteCalParam->DarkCornerSizeY;
    VignetteCalInfo->GainTable->Version = CAL_VIGNETTE_VER;
    VignetteCalInfo->GainTable->TableWidth  = VIGNETTE_FULL_VIEW_MAX_WIDTH;
    VignetteCalInfo->GainTable->TableHeight = VIGNETTE_FULL_VIEW_MAX_HEIGHT;
    VignetteCalInfo->GainTable->Channel = 0;
    VignetteCalInfo->CompensateRatio = VignetteCalParam->CompensateRatio; //could add to script parameter later, 1024 for fully compensated and 0 for no compensation

    if (VignetteCalInfo->GainShift == 0) {
        if (VignetteCalInfo->Threshold > (8192)) {
            VignetteCalInfo->Threshold = 8192;
        }
    } else if (VignetteCalInfo->GainShift == 1) {
        if (VignetteCalInfo->Threshold > (4096)) {
            VignetteCalInfo->Threshold = 4096;
        }
    } else if (VignetteCalInfo->GainShift == 2) {
        if (VignetteCalInfo->Threshold > (2048)) {
            VignetteCalInfo->Threshold = 2048;
        }
    } else if (VignetteCalInfo->GainShift == 3) {
        if (VignetteCalInfo->Threshold > (1024)) {
            VignetteCalInfo->Threshold = 1024;
        }
    } else if (VignetteCalInfo->GainShift == 255) {
        if (VignetteCalInfo->Threshold > (8192)) {
            VignetteCalInfo->Threshold = 8192;
        }
    } else {
        sprintf(OutputStr,"Vignette calibration NG: Unknown gain shift value(%d),it should be 0/1/2/3/255 ",VignetteCalInfo->GainShift);
        return VIG_CALIB_ERROR_GAIN_SHIFT;
    }

    return VIG_CALIB_OK;
}

#if 0
/**
 *  @brief save calibration raw image for debuging
 *
 *  save calibration raw image for debuging
 *
 *  @param [in]raw_info raw image information
 *
 *  @return 0 success, -1 failure
 */

int AppLibCalibVignette_SaveRawImage(postproc_info_t *raw_info)
{
    static int enter_count = 0;
    //Save calibration RAW data here
    AMBA_FS_FILE *fid;
    char raw_fn[64];
    char temp_fn[30] = {'c',':','\\','v','i','g','c','a','l','_','\0'};
    char fmode[2] = {'w','\0'};
    char tmp_ext[5] = {'.','r','a','w','\0'};
    int slot;
    scardmgr_slot_info_t info;
    ID tskid;
    int uc_mode;

    enter_count++;
    slot = AMP_system_cmd(MW_GET_ACTIVE_SLOT, 0, 0);
    scardmgr_get_one_slot_info(slot, &info);
    temp_fn[0] = info.drive_letter;
    AmbaPrint(PURPLE,"SD card root: %c",info.drive_letter);
    get_tid(&tskid);
    uc_mode = ff_get_uc_for_this_task();
#ifdef ENABLE_UC
    ff_set_uc_for_task(tskid,FF_UC_ALWAYS);
#endif
    mstrcpy(raw_fn, temp_fn);
    num_to_text(raw_fn ,enter_count ,0);
    mstrcat(raw_fn, tmp_ext);
    fid = AmbaFS_fopen(raw_fn, fmode);
    AmbaFS_fwrite( raw_info->src_raw_addr[0], sizeof(UINT8),
                   raw_info->src_raw_pitch * raw_info->src_raw_height,
                   fid);
    AmbaFS_fclose(fid);
#ifdef  ENABLE_UC
    ff_set_uc_for_task(tskid,uc_mode);
#endif
    return 0;
}
#endif


/**
 *  @brief save raw image to SD card for debuging
 *
 *  save calibration raw image for debuging
 *
 *  @param [in]VignetteCalInfo information for vignette calibration
 *
 *  @return 0 success, -1 failure
 */
int AppLibCalibVignette_SaveRawImage(Vignette_Cal_Info_s *VignetteCalInfo)
{
    char Fn[16] = {'c',':','\\','v','i','g','c','a','l','.','r','a','w','\0'};
    AMBA_FS_FILE *Fp = NULL;
    UINT32 RawPitch;

    AmbaPrint("AppLibCalibVignette_SaveRawImage");
    //raw ready, dump it
    Fn[0] = AppLib_CalibGetDriverLetter();
    RawPitch = ((VignetteCalInfo->GainTable->RawSensorGeo.Width+15)>>4)<<4;

    Fp = AmbaFS_fopen(Fn, "w");
    AmbaPrint("[AppLibCalibVignette]Dump Raw 0x%X width: %d height: %d  ", \
              VignetteCalInfo->RawAddr, \
              RawPitch, \
              VignetteCalInfo->GainTable->RawSensorGeo.Height);
    AmbaFS_fwrite(VignetteCalInfo->RawAddr, \
                  RawPitch*VignetteCalInfo->GainTable->RawSensorGeo.Height*2, 1, Fp);
    AmbaFS_fclose(Fp);

    return 0;
}


/**
 *  @brief Raw capture for vignette calibration
 *
 *  Raw capture for vignette calibration
 *
 *  @param [in]VignetteCalParam input parameters for vignette calibration
 *  @param [in]VignetteCalInfo information for vignette calibration
 *  @param [in]VinInfo Vin sensor information
 *  @param [out]OutputStr debug message for this function
 *
 *  @return 0 success, -1 failure
 */
int AppLibCalibVignette_RawCapture(Vignette_Cal_Param_s *VignetteCalParam,Vignette_Cal_Info_s *VignetteCalInfo,char *OutputStr,AMBA_SENSOR_MODE_INFO_s VinInfo)
{
    int Rval;
    UINT32 RawAddr;
    //fill in the sensor geometry for Raw image
    VignetteCalInfo->GainTable->RawSensorGeo.Width  = VinInfo.OutputInfo.RecordingPixels.Width;
    VignetteCalInfo->GainTable->RawSensorGeo.Height = VinInfo.OutputInfo.RecordingPixels.Height;
    VignetteCalInfo->GainTable->RawSensorGeo.StartX = VinInfo.InputInfo.PhotodiodeArray.StartX;
    VignetteCalInfo->GainTable->RawSensorGeo.StartY = VinInfo.InputInfo.PhotodiodeArray.StartY;

    VignetteCalInfo->GainTable->RawSensorGeo.HSubSample.FactorDen= VinInfo.InputInfo.HSubsample.FactorDen;
    VignetteCalInfo->GainTable->RawSensorGeo.HSubSample.FactorNum= VinInfo.InputInfo.HSubsample.FactorNum;
    VignetteCalInfo->GainTable->RawSensorGeo.VSubSample.FactorDen= VinInfo.InputInfo.VSubsample.FactorDen;
    VignetteCalInfo->GainTable->RawSensorGeo.VSubSample.FactorNum= VinInfo.InputInfo.VSubsample.FactorNum;
    //fill in the sensor geometry for OB window
    VignetteCalInfo->GainTable->OBVinSensorGeo.Width  = pAmbaCalibInfoObj[VignetteCalParam->Channel]->AmbaCalibWidth;
    VignetteCalInfo->GainTable->OBVinSensorGeo.Height  = pAmbaCalibInfoObj[VignetteCalParam->Channel]->AmbaCalibHeight;
    VignetteCalInfo->GainTable->OBVinSensorGeo.StartX = VinInfo.InputInfo.PhotodiodeArray.StartX -\
                                                           (VinInfo.OutputInfo.RecordingPixels.StartX * VinInfo.InputInfo.HSubsample.FactorDen \
                                                            /VinInfo.InputInfo.HSubsample.FactorNum);
    VignetteCalInfo->GainTable->OBVinSensorGeo.StartY = VinInfo.InputInfo.PhotodiodeArray.StartY -\
                                                           (VinInfo.OutputInfo.RecordingPixels.StartY * VinInfo.InputInfo.VSubsample.FactorDen \
                                                            /VinInfo.InputInfo.VSubsample.FactorNum);
    VignetteCalInfo->GainTable->OBVinSensorGeo.HSubSample.FactorNum = VinInfo.InputInfo.HSubsample.FactorNum;
    VignetteCalInfo->GainTable->OBVinSensorGeo.HSubSample.FactorDen = VinInfo.InputInfo.HSubsample.FactorDen;
    VignetteCalInfo->GainTable->OBVinSensorGeo.VSubSample.FactorNum = VinInfo.InputInfo.VSubsample.FactorNum;
    VignetteCalInfo->GainTable->OBVinSensorGeo.VSubSample.FactorDen = VinInfo.InputInfo.VSubsample.FactorDen;
    VignetteCalInfo->GainTable->OBOffsetX = VinInfo.OutputInfo.RecordingPixels.StartX;
    VignetteCalInfo->GainTable->OBOffsetY = VinInfo.OutputInfo.RecordingPixels.StartY;
    VignetteCalInfo->GainTable->OBEnable = AmpCalib_GetOpticalBlackFlag();
    if (AmpCalib_GetOpticalBlackFlag() == 1) {
        memcpy(&(VignetteCalInfo->GainTable->CalibVinSensorGeo),&(VignetteCalInfo->GainTable->OBVinSensorGeo),sizeof(AMBA_DSP_IMG_VIN_SENSOR_GEOMETRY_s));
    } else {
        memcpy(&(VignetteCalInfo->GainTable->CalibVinSensorGeo),&(VignetteCalInfo->GainTable->RawSensorGeo),sizeof(AMBA_DSP_IMG_VIN_SENSOR_GEOMETRY_s));
    }
    VignetteCalInfo->Width = VignetteCalInfo->GainTable->CalibVinSensorGeo.Width;
    VignetteCalInfo->Height= VignetteCalInfo->GainTable->CalibVinSensorGeo.Height;
    VignetteCalInfo->Bayer= VinInfo.OutputInfo.CfaPattern;

    AmbaPrint("Raw Width = %d",VignetteCalInfo->GainTable->RawSensorGeo.Width);
    AmbaPrint("Raw Height = %d",VignetteCalInfo->GainTable->RawSensorGeo.Height);
    AmbaPrint("Raw StartX = %d",VignetteCalInfo->GainTable->RawSensorGeo.StartX);
    AmbaPrint("Raw StartY = %d",VignetteCalInfo->GainTable->RawSensorGeo.StartY);
    AmbaPrint("OB Width = %d",VignetteCalInfo->GainTable->OBVinSensorGeo.Width);
    AmbaPrint("OB Height = %d",VignetteCalInfo->GainTable->OBVinSensorGeo.Height);
    AmbaPrint("OB StartX = %d",VignetteCalInfo->GainTable->OBVinSensorGeo.StartX);
    AmbaPrint("OB StartY = %d",VignetteCalInfo->GainTable->OBVinSensorGeo.StartY);

    AmbaPrint("HSubSample.FactorNum = %d",VignetteCalInfo->GainTable->RawSensorGeo.HSubSample.FactorNum);
    AmbaPrint("HSubSample.FactorDen = %d",VignetteCalInfo->GainTable->RawSensorGeo.HSubSample.FactorDen);
    AmbaPrint("VSubSample.FactorNum = %d",VignetteCalInfo->GainTable->RawSensorGeo.VSubSample.FactorNum);
    AmbaPrint("VSubSample.FactorDen = %d",VignetteCalInfo->GainTable->RawSensorGeo.VSubSample.FactorDen);



    AmbaPrint("Calibration Width = %d",VignetteCalInfo->GainTable->CalibVinSensorGeo.Width);
    AmbaPrint("Calibration Height = %d",VignetteCalInfo->GainTable->CalibVinSensorGeo.Height);
    AmbaPrint("Calibration StartX = %d",VignetteCalInfo->GainTable->CalibVinSensorGeo.StartX);
    AmbaPrint("Calibration StartY = %d",VignetteCalInfo->GainTable->CalibVinSensorGeo.StartY);
    AmbaPrint("HSubSample.FactorNum = %d",VignetteCalInfo->GainTable->CalibVinSensorGeo.HSubSample.FactorNum);
    AmbaPrint("HSubSample.FactorDen = %d",VignetteCalInfo->GainTable->CalibVinSensorGeo.HSubSample.FactorDen);
    AmbaPrint("VSubSample.FactorNum = %d",VignetteCalInfo->GainTable->CalibVinSensorGeo.VSubSample.FactorNum);
    AmbaPrint("VSubSample.FactorDen = %d",VignetteCalInfo->GainTable->CalibVinSensorGeo.VSubSample.FactorDen);

    //raw capture
    AmbaPrint("start to do raw capture");
    //ItunerRawCapCtrl.SensorMode.Bits.Mode = VinInfo.Mode.Bits.Mode;
    //ItunerRawCapCtrl.SensorMode.Bits.VerticalFlip = VinInfo.Mode.Bits.VerticalFlip;
    //ItunerRawCapCtrl.RawBuff.Raw.RawAddr = (UINT8 *)VignetteCalInfo->RawAddr;
    //ItunerRawCapCtrl.RawBuff.Raw.RawPitch = ((VignetteCalInfo->GainTable->CalibVinSensorGeo.Width+15)/16)*16*2; // Basic DMA transfer unit is 32-byte, which is 16-pixels, so pad to 16pixels boundary here;
    //ItunerRawCapCtrl.RawBuff.Raw.RawWidth = VignetteCalInfo->GainTable->CalibVinSensorGeo.Width;
    //ItunerRawCapCtrl.RawBuff.Raw.RawHeight = VignetteCalInfo->GainTable->CalibVinSensorGeo.Height;
    //Rval = AmpUT_ItunerRawCapture(Flag, ItunerRawCapCtrl);

    AppLibStillEnc_RawCaptureSetSensorMode(1,pAmbaCalibInfoObj[VignetteCalParam->Channel]->AmbaVignetteMode);
    Rval = AppLibStillEnc_CaptureRaw(&RawAddr);
    AmbaKAL_TaskSleep(1000);
    VignetteCalInfo->RawAddr = (UINT16 *) RawAddr;
    AmbaPrintColor(RED,"RawAddr = %x",RawAddr);
    AmbaPrintColor(RED,"VignetteCalInfo->RawAddr = %x",VignetteCalInfo->RawAddr);
    AmbaPrint("end of raw capture");
    if (VignetteCalParam->SaveFileFlagAPP & 0x1) {
        AppLibCalibVignette_SaveRawImage(VignetteCalInfo);
    }

#ifdef GR_GB_MISMATCH
    /* Replace Gr and Gb with (Gr+Gb)/2 before calibration */
    app_grgb_average(VignetteCalInfo);
#endif

    //set the size to check dark corner.
    AmbaPrint("dark corner size X = %d Y = %d",VignetteCalInfo->DarkCornerSizeX,VignetteCalInfo->DarkCornerSizeY);
    return Rval;

}

/**
 *  @brief save ituner information for vignette calibration
 *
 *  save ituner information for vignette calibration
 *
 *  @param [in]VignetteCalInfo information for vignette calibration
 *  @param [in]Channel sensor ID
 *  @param [out]OutputStr debug message for this function
 *
 *  @return 0 success, -1 failure
 */
int AppLibCalibVignette_SaveItunerFile(Vignette_Cal_Info_s *VignetteCalInfo,char *OutputStr, UINT8 Channel)
{
    char FnVignetteGainTable [20] = {'c',':','\\','v','i','g','0','0','.','b','i','n','\0'};
    AMBA_FS_FILE *Fp = NULL;

    UINT32 TableSize;
	FnVignetteGainTable[0] = AppLib_CalibGetDriverLetter();

    Fp = AmbaFS_fopen(FnVignetteGainTable, "w");

    if (Fp == NULL) {
        sprintf(OutputStr,"AmbaFS_fopen %s FnVignetteGainTable  fail", FnVignetteGainTable);
        return VIG_CALIB_ERROR_SAVE_ITUNER_FILE;
    }

    TableSize = VIGNETTE_FULL_VIEW_MAX_WIDTH*VIGNETTE_FULL_VIEW_MAX_HEIGHT<<1;
    AmbaFS_fwrite(UnPackVignette[Channel].VignetteRedGain,TableSize , 1, Fp);
    AmbaFS_fwrite(UnPackVignette[Channel].VignetteGreenEvenGain,TableSize , 1, Fp);
    AmbaFS_fwrite(UnPackVignette[Channel].VignetteGreenOddGain,TableSize , 1, Fp);
    AmbaFS_fwrite(UnPackVignette[Channel].VignetteBlueGain,TableSize , 1, Fp);

    AmbaFS_fclose(Fp);
    return 0;

}

/**
 *  @brief calculate vignette table from raw image
 *
 *  calculate vignette table from raw image
 *
 *  @param [in]CalObj calibration object
 *  @param [in]VignetteCalInfo information for vignette calibration
 *  @param [out]OutputStr debug message for this function
 *
 *  @return 0 success, -1 failure
 */
int AppLibCalibVignette_CalcVignetteFromRawImage(Cal_Obj_s *CalObj,Vignette_Cal_Info_s *VignetteCalInfo,char *OutputStr)
{
    UINT8 *VignetteAddress = CalObj->DramShadow;
    UINT16 MaxShade=0;
    int Rval;
    char VignetteRaw [20] = {'c',':','\\','v','i','g','.','r','a','w','\0'};
    AMBA_FS_FILE *FidRaw;
    AMBA_SENSOR_MODE_INFO_s VinInfo;
    void *TempPtr,*TempBufferRel,*TempGridBuffer,*TempGridBufferRel;
    AMBA_SENSOR_MODE_ID_u SensorMode;

    memset(&SensorMode, 0, sizeof(SensorMode));
    AppLibCalibVignette_BufferLock();
    //set the dram address to save the vignette table.
    VignetteCalInfo->GainTable =(Vignette_Pack_Storage_s *) &VignetteAddress[CAL_VIGNETTE_TABLE(0)];
    VignetteCalInfo->GainTable->CalInfo[0] = 1;//CalInfo 0: zp_table  1:total_setp_num
    VignetteCalInfo->GainTable->CalInfo[1] = VignetteCalInfo->CompensateMethod;//calibration method
    VignetteCalInfo->GainTable->ZoomStep = 0;
    VignetteCalInfo->GainTable->Channel  = 0;
    VignetteCalInfo->GainTable->SiteId   = 0;
    VignetteCalInfo->GainTable->Version = CAL_VIGNETTE_VER;
    VignetteCalInfo->GainTable->TableWidth  = VIGNETTE_FULL_VIEW_MAX_WIDTH;
    VignetteCalInfo->GainTable->TableHeight = VIGNETTE_FULL_VIEW_MAX_HEIGHT;


    /* allocate raw buffer address */
    //bug.......unknown sensor mode
	SensorMode.Bits.Mode = pAmbaCalibInfoObj[0]->AmbaVignetteMode;
    AmbaSensor_GetModeInfo(AppEncChannel, SensorMode, &VinInfo);
    
    //fill in the sensor geometry for Raw image
    VignetteCalInfo->GainTable->RawSensorGeo.Width  = VignetteCalInfo->Width;
    VignetteCalInfo->GainTable->RawSensorGeo.Height = VignetteCalInfo->Height;
    VignetteCalInfo->GainTable->RawSensorGeo.StartX = 0;
    VignetteCalInfo->GainTable->RawSensorGeo.StartY = 0;

    VignetteCalInfo->GainTable->RawSensorGeo.HSubSample.FactorDen= 1;
    VignetteCalInfo->GainTable->RawSensorGeo.HSubSample.FactorNum= 1;
    VignetteCalInfo->GainTable->RawSensorGeo.VSubSample.FactorDen= 1;
    VignetteCalInfo->GainTable->RawSensorGeo.VSubSample.FactorNum= 1;
    //fill in the sensor geometry for OB window
    VignetteCalInfo->GainTable->OBVinSensorGeo.Width  = VignetteCalInfo->Width;
    VignetteCalInfo->GainTable->OBVinSensorGeo.Height  = VignetteCalInfo->Height;
    VignetteCalInfo->GainTable->OBVinSensorGeo.StartX = 0;
    VignetteCalInfo->GainTable->OBVinSensorGeo.StartY = 0;
    VignetteCalInfo->GainTable->OBVinSensorGeo.HSubSample.FactorNum = 1;
    VignetteCalInfo->GainTable->OBVinSensorGeo.HSubSample.FactorDen = 1;
    VignetteCalInfo->GainTable->OBVinSensorGeo.VSubSample.FactorNum = 1;
    VignetteCalInfo->GainTable->OBVinSensorGeo.VSubSample.FactorDen = 1;
    VignetteCalInfo->GainTable->OBOffsetX = 0;
    VignetteCalInfo->GainTable->OBOffsetY = 0;
    VignetteCalInfo->GainTable->OBEnable = 0;
    if (AmpCalib_GetOpticalBlackFlag() == 1) {
        memcpy(&(VignetteCalInfo->GainTable->CalibVinSensorGeo),&(VignetteCalInfo->GainTable->OBVinSensorGeo),sizeof(AMBA_DSP_IMG_VIN_SENSOR_GEOMETRY_s));
    } else {
        memcpy(&(VignetteCalInfo->GainTable->CalibVinSensorGeo),&(VignetteCalInfo->GainTable->RawSensorGeo),sizeof(AMBA_DSP_IMG_VIN_SENSOR_GEOMETRY_s));
    }
    Rval = AmpUtil_GetAlignedPool(APPLIB_G_MMPL, &TempPtr, &TempBufferRel, (VignetteCalInfo->GainTable->RawSensorGeo.Width*VignetteCalInfo->GainTable->RawSensorGeo.Height)<<1, 32);
    if (Rval != OK) {
        AmbaPrint("[AppLibCalibVignette]allocate raw buffer fail (%u)!", (VignetteCalInfo->GainTable->RawSensorGeo.Width*VignetteCalInfo->GainTable->RawSensorGeo.Height)<<1);
    } else {
        VignetteCalInfo->RawAddr = (UINT16*)TempPtr;
        AmbaPrint("[AppLibCalibVignette]rawBuffAddr (0x%08X) (%u)!",\
        VignetteCalInfo->RawAddr, (VignetteCalInfo->GainTable->RawSensorGeo.Width*VignetteCalInfo->GainTable->RawSensorGeo.Height)<<1);
    }

    AmbaPrint("doing vignette calibration by raw capture");
	VignetteRaw[0] = AppLib_CalibGetDriverLetter();
    FidRaw = AmbaFS_fopen(VignetteRaw, "r");
    if (FidRaw == NULL) {
        AmbaPrint("Vignette calibration NG:open raw image fail");
        return VIG_CALIB_ERROR_OPEN_RAW_ERROR;
    }
    AmbaFS_fread(VignetteCalInfo->RawAddr, sizeof(UINT32), ((VignetteCalInfo->GainTable->RawSensorGeo.Width* VignetteCalInfo->GainTable->RawSensorGeo.Height)>>1 ), FidRaw);
    AmbaPrint("RAW data addr:0x%8x Width %d Height(%d)",VignetteCalInfo->RawAddr,VignetteCalInfo->Width,VignetteCalInfo->Height);
    AmbaFS_fclose(FidRaw);

    Rval = AmpUtil_GetAlignedPool(APPLIB_G_MMPL, &TempGridBuffer, &TempGridBufferRel, (4*VIGNETTE_CAL_SIZE*sizeof(UINT32)), 32);
    if (Rval != OK) {
        AmbaPrint("[AppLibCalibVignette]allocate buffer fail (%u)!", (4*VIGNETTE_CAL_SIZE*sizeof(UINT32)));
    } else {
        VignetteCalInfo->VigGridsAddr = (UINT32)TempGridBuffer;
        AmbaPrint("[AppLibCalibVignette]rawBuffAddr (0x%08X) (%u)!", VignetteCalInfo->VigGridsAddr, (4*VIGNETTE_CAL_SIZE*sizeof(UINT32)));
    }
    VignetteCalInfo->VigGridsAddr  = (UINT32)TempGridBuffer;
    //do vignette calibration
    Rval = AmpCalib_VigCalFromRaw(VignetteCalInfo,OutputStr);
    AmbaKAL_BytePoolFree((void *) TempGridBufferRel);
    if (Rval < 0) {
        AmbaKAL_BytePoolFree(TempBufferRel);//release memory
        return Rval;
    }
    AppLibCalibVignette_BufferUnlock();
    AppLibCalibVignette_SelectVignetteTable((VIG_UPDATE_ALL|VIG_DEBUG_TABLE), 0, 0, 0, 0,65536,65536);
    Rval = AppLibCalibVignette_SaveItunerFile(VignetteCalInfo,OutputStr,0);
    if (Rval < 0) {
        AmbaKAL_BytePoolFree(TempBufferRel);//release memory
        return Rval;
    }
    if (Rval == 0) {
        if (VignetteCalInfo->MaxMinRatio > MaxShade) {
            MaxShade = VignetteCalInfo->MaxMinRatio;
            AmbaPrint("Vignette calibration OK: Max shading ratio(%d)", MaxShade);
        }
    } else {
        AmbaKAL_BytePoolFree(TempBufferRel);
        return VIG_CALIB_ERROR_EXCEED_THRESHOLD;
    }
    //bug
    AmbaKAL_BytePoolFree(TempBufferRel);
    AmbaKAL_BytePoolFree(TempGridBufferRel);

    VignetteAddress[CAL_VIGNETTE_ENABLE] = 1;
    VignetteAddress[CAL_VIGNETTE_TABLE_COUNT] = 1;
    return 0;
}

/**
 *  @brief print the error message for vignette calibration
 *
 *  print the error message for vignette calibration
 *
 *  @param [in]CalObj calibration object
 *  @param [out]OutputStr debug message for this function
 *
 *  @return 0 success, -1 failure
 */
int AppLibCalibVignette_PrintError(char *OutputStr, Cal_Stie_Status_s *CalSite,Vignette_Cal_Param_s *VignetteCalParam)
{
    AmbaPrint("************************************************************");
    AmbaPrint("%s",OutputStr);
    AmbaPrint("************************************************************");
    CalSite->Status = CAL_SITE_RESET;
    CalSite->SubSiteStatus[VignetteCalParam->CalSubSiteId] = CAL_SITE_RESET;
    return 0;
}

/**
 *  @brief the entry function to do vignette calibration
 *
 *  the entry function to do vignette calibration
 *
 *  @param [in]Argc number of input parameters
 *  @param [in]Argv value of input parameters
 *  @param [in]CalSite calibration site status
 *  @param [in]CalObj calibration object
 *  @param [out]OutputStr debug message for this function
 *
 *  @return 0 success, -1 failure
 */
int AppLibCalibVignette_Func(int Argc, char *Argv[], char *OutputStr, Cal_Stie_Status_s *CalSite, Cal_Obj_s *CalObj)
{
    int Rval;
    Vignette_Cal_Info_s *VignetteCalInfo;
    //AMBA_3A_OP_INFO_s cur_aaa_func;
    AMBA_AE_INFO_s CalAeInfo;
    UINT16 MaxShade=0;
    Vignette_Cal_Param_s VignetteCalParam;
    int i;
    AMBA_DSP_IMG_BLACK_CORRECTION_s CalBlackLevel;
    void *TempBufferPtr,*TempGridBuffer;
    void *TempBufferPtrRel,*TempGridBufferRel;
//    UINT8 *RawBufferaddr;
    AMBA_SENSOR_MODE_INFO_s VinInfo;
    AMBA_3A_OP_INFO_s  AaaOpInfoBackup = {DISABLE, DISABLE, DISABLE, DISABLE};
    AMBA_SENSOR_MODE_ID_u SensorMode;

    AppLibCalibAdjust_SetControlEnable(0);
    memset(&SensorMode, 0, sizeof(SensorMode));
    AppLibCalibVignette_BufferLock();
    Rval = AmpUtil_GetAlignedPool(APPLIB_G_MMPL, &TempBufferPtr, &TempBufferPtrRel, sizeof(Vignette_Cal_Info_s), 32);
    if (Rval != OK) {
        AmbaPrint("[AppLibCalibVignette]allocate VigCalBuffer fail (%u)!", sizeof(Vignette_Cal_Info_s));
        return Rval;
    } else {
        VignetteCalInfo = (Vignette_Cal_Info_s*)TempBufferPtr;
        AmbaPrint("[AppLibCalibVignette]VigCalBufferAddr (0x%08X) (%u)!", VignetteCalInfo, sizeof(Vignette_Cal_Info_s));
    }

    Rval = AppLibCalibVignette_LoadVignetteScript(Argc, Argv, OutputStr, CalSite, CalObj, &VignetteCalParam,VignetteCalInfo);
    if (Rval < 0) {
        AppLibCalibVignette_PrintError(OutputStr,CalSite,&VignetteCalParam);
        AmbaKAL_BytePoolFree(TempBufferPtrRel);
        return Rval;
    }
    //load raw image from the SD card, and doing vignette with the raw image
    if (strcmp(Argv[1], "LOAD_RAW") == 0) {
        Rval = AppLibCalibVignette_CalcVignetteFromRawImage(CalObj,VignetteCalInfo,OutputStr);
        if (Rval < 0) {
            AmbaKAL_BytePoolFree(TempBufferPtrRel);
            AppLibCalibVignette_PrintError(OutputStr,CalSite,&VignetteCalParam);
            return Rval;
        }
        sprintf(OutputStr,"Vignette calibration OK: Max shading ratio(%d)", VignetteCalInfo->MaxMinRatio);
        AmbaPrint("%s",OutputStr);
        CalSite->Status = CAL_SITE_DONE;
        CalSite->SubSiteStatus[VignetteCalParam.CalSubSiteId] = CAL_SITE_DONE;
        CalSite->Version = CAL_VIGNETTE_VER;
        AmbaPrint("version = %x",CalSite->Version);
        return Rval;
    }
    //config 3A setting
    //backup AAA_OP_INFO
    AmbaImg_Proc_Cmd(MW_IP_GET_AAA_OP_INFO, VignetteCalParam.Channel, (UINT32)&AaaOpInfoBackup, 0);

    /* allocate raw buffer address */


    SensorMode.Bits.Mode = pAmbaCalibInfoObj[VignetteCalParam.Channel]->AmbaVignetteMode;
    AmbaSensor_GetModeInfo(AppEncChannel, SensorMode, &VinInfo);

    for (i = 0; i<VignetteCalParam.NumCalSteps; i++) {
        //zoom control for vignette calibration
        Rval = AppLibCalibVignette_CalVignetteZoomControl(OutputStr,i,&VignetteCalParam,VignetteCalInfo,&CalBlackLevel,CalObj);
        if (Rval < 0) {
//            AmbaKAL_BytePoolFree(TempPtrRel);
            AmbaKAL_BytePoolFree(TempBufferPtrRel);
            AppLibCalibVignette_PrintError(OutputStr,CalSite,&VignetteCalParam);
            AmbaImg_Proc_Cmd(MW_IP_SET_AAA_OP_INFO, VignetteCalParam.Channel, (UINT32)&AaaOpInfoBackup, 0);
            return Rval;
        }
        //Set still capture to full view calibration mode
        //Set ae info to calibration result

        AmbaPrint("doing vignette calibration by raw capture");
#if 0
        VignetteCalInfo->RawAddr =(UINT16 *)RawBufferaddr;
#endif
        Rval = AppLibCalibVignette_CalVignette3ASetting(OutputStr, CalSite,&VignetteCalParam,VignetteCalInfo,&CalAeInfo);
        if (Rval < 0) {
//            AmbaKAL_BytePoolFree(TempPtrRel);
            AmbaKAL_BytePoolFree(TempBufferPtrRel);
            AppLibCalibVignette_PrintError(OutputStr,CalSite,&VignetteCalParam);
            AmbaImg_Proc_Cmd(MW_IP_SET_AAA_OP_INFO, VignetteCalParam.Channel, (UINT32)&AaaOpInfoBackup, 0);
            return Rval;
        }

        Rval = AppLibCalibVignette_RawCapture(&VignetteCalParam,VignetteCalInfo,OutputStr,VinInfo);
        if (Rval < 0) {
//            AmbaKAL_BytePoolFree(TempPtrRel);
            AmbaKAL_BytePoolFree(TempBufferPtrRel);
            AppLibCalibVignette_PrintError(OutputStr,CalSite,&VignetteCalParam);
            AmbaImg_Proc_Cmd(MW_IP_SET_AAA_OP_INFO, VignetteCalParam.Channel, (UINT32)&AaaOpInfoBackup, 0);
            return Rval;
        }

        //get buffer for AmpCalib_VigCalFromRaw
        Rval = AmpUtil_GetAlignedPool(APPLIB_G_MMPL, &TempGridBuffer, &TempGridBufferRel, (4*VIGNETTE_CAL_SIZE*sizeof(UINT32)), 32);
        if (Rval != OK) {
            AmbaKAL_BytePoolFree(TempBufferPtrRel);
//            AmbaKAL_BytePoolFree(TempPtrRel);
            AmbaKAL_BytePoolFree(TempGridBufferRel);
            AmbaPrint("[AppLibCalibVignette]allocate raw fail (%u)!", (4*VIGNETTE_CAL_SIZE*sizeof(UINT32)));
        } else {
            VignetteCalInfo->VigGridsAddr = (UINT32)TempGridBuffer;
            AmbaPrint("[AppLibCalibVignette]rawBuffAddr (0x%08X) (%u)!", VignetteCalInfo->VigGridsAddr, (4*VIGNETTE_CAL_SIZE*sizeof(UINT32)));
        }
        Rval = AmpCalib_VigCalFromRaw(VignetteCalInfo,OutputStr);

        if (Rval < 0) {
            AmbaKAL_BytePoolFree(TempBufferPtrRel);
            AppLibCalibVignette_PrintError(OutputStr,CalSite,&VignetteCalParam);
            AmbaKAL_BytePoolFree(TempGridBufferRel);
            AmbaImg_Proc_Cmd(MW_IP_SET_AAA_OP_INFO, VignetteCalParam.Channel, (UINT32)&AaaOpInfoBackup, 0);
            return Rval;
        }
        AppLibCalibVignette_BufferUnlock();
        if (VignetteCalParam.SaveFileFlagAPP & 0x1) {
            //bug, testing
            AppLibCalibVignette_SelectVignetteTable((VIG_UPDATE_ALL|VIG_DEBUG_TABLE), VignetteCalParam.Channel, i, i, 0,65536,65536);
            Rval = AppLibCalibVignette_SaveItunerFile(VignetteCalInfo,OutputStr,VignetteCalParam.Channel);
            if (Rval < 0) {
                AmbaKAL_BytePoolFree(TempBufferPtrRel);
                AppLibCalibVignette_PrintError(OutputStr,CalSite,&VignetteCalParam);
                AmbaKAL_BytePoolFree(TempGridBufferRel);
                AmbaImg_Proc_Cmd(MW_IP_SET_AAA_OP_INFO, VignetteCalParam.Channel, (UINT32)&AaaOpInfoBackup, 0);
                return Rval;
            }
        }
        if (Rval == 0) {
            if (VignetteCalInfo->MaxMinRatio > MaxShade) {
                MaxShade = VignetteCalInfo->MaxMinRatio;
                AmbaKAL_BytePoolFree(TempGridBufferRel);
            }
        } else {
            sprintf(OutputStr,"Vignette calibration NG: Max shading exceed Threshold!");
            AmbaKAL_BytePoolFree(TempBufferPtrRel);
            AmbaKAL_BytePoolFree(TempGridBufferRel);
            AppLibCalibVignette_PrintError(OutputStr,CalSite,&VignetteCalParam);
            AmbaImg_Proc_Cmd(MW_IP_SET_AAA_OP_INFO, VignetteCalParam.Channel, (UINT32)&AaaOpInfoBackup, 0);

            return VIG_CALIB_ERROR_EXCEED_THRESHOLD;
        }

    }
    //release buffer
    AmbaKAL_BytePoolFree(TempBufferPtrRel);
    sprintf(OutputStr,"Vignette calibration OK: Max shading ratio(%d)", MaxShade);
    AmbaPrint("%s",OutputStr);
    CalSite->Status = CAL_SITE_DONE;
    CalSite->Version = CAL_VIGNETTE_VER;
    CalSite->SubSiteStatus[VignetteCalParam.CalSubSiteId] = CAL_SITE_DONE;
    AmbaPrint("version = %x",CalSite->Version);
    AmbaImg_Proc_Cmd(MW_IP_SET_AAA_OP_INFO, VignetteCalParam.Channel, (UINT32)&AaaOpInfoBackup, 0);
    return Rval;
}


/**
 *  @brief the unit test function for vignette calibration
 *
 *  the unit test function for vignette calibration
 *
 *  @param [in] env: environment
 *  @param [in]Argc number of input parameters
 *  @param [in]Argv value of input parameters
 *
 *  @return 0 success, -1 failure
 */
int AppLibVignette_UTFunc(int Argc, char *Argv[])
{
    Cal_Obj_s           *CalObj;
    int Rval = -1;

    CalObj = AppLib_CalibGetObj(CAL_VIGNETTE_ID);
    if ((strcmp(Argv[2], "test") == 0)) {
        //register calibration site
        AppLib_CalibSiteInit();
        Rval = 0;
    } else if ((strcmp(Argv[2], "init") == 0)) {
        AppLibCalibVignette_Init(CalObj);
        Rval = 0;
    } else if ((strcmp(Argv[2], "select") == 0)) {
        int Enable      = atoi(Argv[3]);
        int Channel     = atoi(Argv[4]);
        int Id1         = atoi(Argv[5]);
        int Id2         = atoi(Argv[6]);
        int BlendRatio  = atoi(Argv[7]);
        int VignetteLumaStrength    = atoi(Argv[8]);
        int VignetteChromaStrength = atoi(Argv[9]);
		if(Argc == 10){
            if (Enable) {
                Enable = VIG_UPDATE_ALL;
            }
        Rval = AppLibCalibVignette_SelectVignetteTable(Enable, Channel, Id1, Id2, BlendRatio, VignetteLumaStrength,VignetteChromaStrength);
		}
    } else if ((strcmp(Argv[2], "debug") == 0)) {
        int Enable = atoi(Argv[3]);
        AppLibCalibVignette_DebugEnable(Enable);
        Rval = 0;
    } else if(strcmp(Argv[2], "info") == 0) {
        int i;
        if (AppVignetteControl.Enable) {
            AmbaPrint("enable = %d , VignetteTableCount = %d", AppVignetteControl.Enable, AppVignetteControl.GainTableCount);
            for(i = 0; i < MAX_VIGNETTE_NAND_TABLE_COUNT; i++) {
			AmbaPrint("---------- storage #%d ----------", i);
			AmbaPrint("enable = %d, gain_shift = %d", AppVignetteControl.GainTable[i]->Enable, AppVignetteControl.GainTable[i]->GainShift);
			AmbaPrint("[swin_info] start_x = %u, start_y = %u, width = %u, height = %u",
                                AppVignetteControl.GainTable[i]->CalibVinSensorGeo.StartX, AppVignetteControl.GainTable[i]->CalibVinSensorGeo.StartY,
                                AppVignetteControl.GainTable[i]->CalibVinSensorGeo.Width, AppVignetteControl.GainTable[i]->CalibVinSensorGeo.Height);
			AmbaPrint("Addrees of gain tables (R, Ge, Go, B) = (0x%X, 0x%X, 0x%X, 0x%X)", 
				AppVignetteControl.GainTable[i]->VignetteRedGain,
				AppVignetteControl.GainTable[i]->VignetteGreenEvenGain,
				AppVignetteControl.GainTable[i]->VignetteGreenOddGain,
        			AppVignetteControl.GainTable[i]->VignetteBlueGain);
            }
        } else {
            AmbaPrint("NO Vignette calibration");
        }
    } else if(strcmp(Argv[2], "ctl") == 0) {
        if((strcmp(Argv[3], "debug") == 0)){
            int En = atoi(Argv[4]);                
            AppLibCalibAdjust_Debug((UINT8) En);
        }else{
            int CtlEn = atoi(Argv[3]);                
            AppLibCalibAdjust_SetControlEnable(CtlEn);
        }
        Rval = 0;
    }

    if (Rval == -1) {
        AmbaPrint("t cal vnc init : re-init vignette");
        AmbaPrint("t cal vnc select Enable Channel Id1 Id2 BlendRatio VignetteLumaStrength VignetteChromaStrength: select vignette");
        AmbaPrint("t cal vnc debug enable :enable debug Flag to print message");
        AmbaPrint("t cal vnc info :vignette calibration information");
    }


    return Rval;
}
