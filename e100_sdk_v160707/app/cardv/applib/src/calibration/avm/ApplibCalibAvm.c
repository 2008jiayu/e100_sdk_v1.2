/**
 * @file src/app/connected/applib/src/calibration/avm/ApplibCalibAvm.c
 *
 * sample code for surrounfing view calibration
 *
 * History:
 *    07/10/2014  Paul Hsu Created
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
#include <calibration/avm/ApplibCalibAvm.h>
#include <AmbaDSP.h>
#include <AmbaSensor.h>
#include <AmbaPrintk.h>
#include <AmbaUtility.h>
#include <common/common.h>
#include "mw/calib/AmbaCalib_Detect.h"
#include <apps/flow/disp_param/disp_param_rec.h>
#define IK2StageCompensation 1
#define Calib2StageCompensation 0

extern AMBA_KAL_BYTE_POOL_t G_MMPL;
extern AMBA_KAL_BYTE_POOL_t G_NC_MMPL;
Warp_Control_s AppWarpControl;
Warp_Storage_s WarpTable[CALIB_CH_NO];
AMBA_DSP_IMG_CALIB_WARP_INFO_s WarpInfo[CALIB_CH_NO];

AVM_Detect_Buffer DetectBuffer;
AVM_Detect_Parameter DetectParameter;
static UINT32 Raw_W, Raw_H, Raw_SIZE;

/**
 *  @brief print the error message for AVM calibration
 *
 *  print the error message for AVM calibration
 *
 *  @param [in] CalSite calibration site status
 *  @param [out] OutputStr OutputStr debug message for this function
 *
 *  @return 0
 */
int AppLibCalibWarp_PrintError(char *OutputStr, Cal_Stie_Status_s *CalSite)
{
    AmbaPrint("************************************************************");
    AmbaPrint("%s",OutputStr);
    AmbaPrint("************************************************************");
    CalSite->Status = CAL_SITE_RESET;
    return 0;
}

/**
 *  @brief get the warp enable flag
 *
 *  get the warp enable flag
 *
 *
 *  @return enable flag
 */
INT8  AppLibCalibWarp_GetCalDataEnable(void)
{
    return AppWarpControl.Enable;
}

/**
 *  @brief set the warp enable flag
 *
 *  set the warp enable flag
 *
 *  @param [in] En enable flag
 *
 *  @return 0
 */
UINT8  AppLibCalibWarp_SetCalDataEnable(UINT8 En)
{
    AppWarpControl.Enable = En;
    return 0;
}

/**
 *  @brief set the warp enable flag
 *
 *  set the warp enable flag
 *
 *
 *  @return the pointer of warp data
 */
Warp_Control_s * AppLibCalibWarp_GetWarpCalData()
{
    return &AppWarpControl;
}

/**
 *  @brief control the warp table
 *
 *  control the warp table
 *
 *  @param [in] Enable enable flag to control the warp on/off
 *  @param [in] Channel Channel
 *  @param [in] Id1 table ID 1
 *  @param [in] Id2 table ID 2
 *  @param [in] BlendRatio blend ratio for table 1 and table 2
 *  @param [in] Strength strength for the warp table
 *
 *  @return 0 success, -1 failure
 */
int AppLibCalibWarp_SelectWarpTable(UINT8 Enable, UINT8 Channel, UINT8 Id1, UINT8 Id2, int BlendRatio, INT32 Strength)
{
    int Rval = 0;
    int i;
    int Ratio1;
    AMBA_DSP_IMG_MODE_CFG_s Mode;
    AMBA_DSP_IMG_WARP_CALC_INFO_s CalcWarp = {0};
    UINT32 Config;
    UINT8 CalibMappingId1,CalibMappingId2;

    memset(&Mode, 0, sizeof(AMBA_DSP_IMG_MODE_CFG_s));

    Mode.Pipe = AMBA_DSP_IMG_PIPE_VIDEO;
    Mode.ContextId = Channel;
    Mode.BatchId = AMBA_DSP_FILTER_CVT(AMBA_DSP_VIDEO_FILTER, Channel); //Prepare idsp to channel,using AMBA_DSP_FILTER_CVT to count BatchID
    if (AmbaDSP_ImgGetWarpCompensation(&Mode, &CalcWarp) != OK) {
        AmbaPrint("Get Warp Compensation fail!!");
    }
    AmbaPrint("AppLibCalibWarp_SelectWarpTable: Enable: %d Channel: %d Id1: %d Id2: %d BlendRatio: %d Strength: %d",
              Enable, Channel, Id1, Id2, BlendRatio, Strength);
    CalibMappingId1 = AppLib_CalibTableMapping(Channel, Id1);
    CalibMappingId2 = AppLib_CalibTableMapping(Channel, Id2);
    if (Enable) {
        //blending
        if ((CalibMappingId1 != CalibMappingId2)) {
            if (AppWarpControl.WarpTable[CalibMappingId1]->WarpHeader.HorGridNum != AppWarpControl.WarpTable[CalibMappingId2]->WarpHeader.HorGridNum) {
                AmbaPrint("Horizonal Grid number mismatch");
                return -1;
            }
            if (AppWarpControl.WarpTable[CalibMappingId1]->WarpHeader.VerGridNum != AppWarpControl.WarpTable[CalibMappingId2]->WarpHeader.VerGridNum) {
                AmbaPrint("Vertical Grid number mismatch");
                return -1;
            }
            if ((BlendRatio < 0) || (BlendRatio > 65536)) {
                AmbaPrint("BlendRatio should be between 0~65536, BlendRatio = %d",BlendRatio);
                return -1;
            }
            Ratio1 = 65536 - BlendRatio;
            WarpTable[Channel].WarpHeader = AppWarpControl.WarpTable[CalibMappingId1]->WarpHeader;
            for (i = 0; i < AppWarpControl.WarpTable[CalibMappingId1]->WarpHeader.HorGridNum*AppWarpControl.WarpTable[CalibMappingId1]->WarpHeader.VerGridNum; i++) {
                WarpTable[Channel].WarpVector[i].X = (AppWarpControl.WarpTable[CalibMappingId1]->WarpVector[i].X*BlendRatio + \
                                                      AppWarpControl.WarpTable[CalibMappingId2]->WarpVector[i].X*Ratio1)>>16;
                WarpTable[Channel].WarpVector[i].Y = (AppWarpControl.WarpTable[CalibMappingId1]->WarpVector[i].Y*BlendRatio + \
                                                      AppWarpControl.WarpTable[CalibMappingId2]->WarpVector[i].Y*Ratio1)>>16;
            }
        } else if (CalibMappingId1 == CalibMappingId2) {
            WarpTable[Channel] = *(AppWarpControl.WarpTable[CalibMappingId1]);
        } else if (BlendRatio == 0) {
            WarpTable[Channel] = *(AppWarpControl.WarpTable[CalibMappingId1]);
        } else if (BlendRatio == 65536) {
            WarpTable[Channel] = *(AppWarpControl.WarpTable[CalibMappingId2]);
        }
        //decay
        if ((Strength < 0) || (Strength > 65536)) {
            AmbaPrint("Strength should be between 0~65536, Strength = %d",Strength);
            return -1;
        }
        if (Strength != 65536) {
            for (i = 0; i < AppWarpControl.WarpTable[CalibMappingId1]->WarpHeader.HorGridNum*AppWarpControl.WarpTable[CalibMappingId1]->WarpHeader.VerGridNum; i++) {
                WarpTable[Channel].WarpVector[i].X = (WarpTable[Channel].WarpVector[i].X*Strength)>>16;
                WarpTable[Channel].WarpVector[i].Y = (WarpTable[Channel].WarpVector[i].Y*Strength)>>16;
            }
        }
        Config = 0;//enable warp
        WarpInfo[Channel].Version                            = WarpTable[Channel].WarpHeader.Version;
        WarpInfo[Channel].HorGridNum                         = WarpTable[Channel].WarpHeader.HorGridNum;
        WarpInfo[Channel].VerGridNum                         = WarpTable[Channel].WarpHeader.VerGridNum;
        WarpInfo[Channel].TileWidthExp                       = WarpTable[Channel].WarpHeader.TileWidthExp;
        WarpInfo[Channel].TileHeightExp                      = WarpTable[Channel].WarpHeader.TileHeightExp;
        WarpInfo[Channel].VinSensorGeo.Width                 = WarpTable[Channel].WarpHeader.VinSensorGeo.Width;
        WarpInfo[Channel].VinSensorGeo.Height                = WarpTable[Channel].WarpHeader.VinSensorGeo.Height;
        WarpInfo[Channel].VinSensorGeo.StartX                = WarpTable[Channel].WarpHeader.VinSensorGeo.StartX;
        WarpInfo[Channel].VinSensorGeo.StartY                = WarpTable[Channel].WarpHeader.VinSensorGeo.StartY;
        WarpInfo[Channel].VinSensorGeo.HSubSample.FactorDen  = WarpTable[Channel].WarpHeader.VinSensorGeo.HSubSample.FactorDen;
        WarpInfo[Channel].VinSensorGeo.HSubSample.FactorNum  = WarpTable[Channel].WarpHeader.VinSensorGeo.HSubSample.FactorNum;
        WarpInfo[Channel].VinSensorGeo.VSubSample.FactorDen  = WarpTable[Channel].WarpHeader.VinSensorGeo.VSubSample.FactorDen;
        WarpInfo[Channel].VinSensorGeo.VSubSample.FactorNum  = WarpTable[Channel].WarpHeader.VinSensorGeo.VSubSample.FactorNum;
        WarpInfo[Channel].Enb2StageCompensation              = (WarpTable[Channel].WarpHeader.Warp2StageFlag == 1) ? Calib2StageCompensation : IK2StageCompensation;
        WarpInfo[Channel].Reserved1                          = WarpTable[Channel].WarpHeader.Reserved1;
        WarpInfo[Channel].Reserved2                          = WarpTable[Channel].WarpHeader.Reserved2;
        WarpInfo[Channel].pWarp                              = (AMBA_DSP_IMG_GRID_POINT_s*)WarpTable[Channel].WarpVector;
    } else {
        Config = AMBA_DSP_IMG_WARP_CONFIG_FORCE_DISABLE;//disable warp
    }

    AmbaPrint("version: 0x%x",WarpInfo[Channel].Version);
    AmbaPrint("horizontal grid number: %d",WarpInfo[Channel].HorGridNum);
    AmbaPrint("vertical grid number: %d",WarpInfo[Channel].VerGridNum);
    AmbaPrint("tile width: %d",WarpInfo[Channel].TileWidthExp);
    AmbaPrint("tile height: %d",WarpInfo[Channel].TileHeightExp);
    AmbaPrint("Image width: %d",WarpInfo[Channel].VinSensorGeo.Width);
    AmbaPrint("Image height: %d",WarpInfo[Channel].VinSensorGeo.Height);
    AmbaPrint("StartX: %d",WarpInfo[Channel].VinSensorGeo.StartX);
    AmbaPrint("StartY: %d",WarpInfo[Channel].VinSensorGeo.StartY);
    AmbaPrint("HSubSample FactorDen: %d",WarpInfo[Channel].VinSensorGeo.HSubSample.FactorDen);
    AmbaPrint("HSubSample FactorNum: %d",WarpInfo[Channel].VinSensorGeo.HSubSample.FactorNum);
    AmbaPrint("VSubSample FactorDen: %d",WarpInfo[Channel].VinSensorGeo.VSubSample.FactorDen);
    AmbaPrint("VSubSample FactorNum: %d",WarpInfo[Channel].VinSensorGeo.VSubSample.FactorNum);
    AmbaPrint("Warp table address: 0x%X",WarpInfo[Channel].pWarp);

    AmbaDSP_WarpCore_SetCalibWarpInfo(&Mode, &WarpInfo[Channel]);

    {
    	AMBA_DSP_IMG_VIN_SENSOR_GEOMETRY_s VinSensorGeo = {0};
    	if (AmbaDSP_WarpCore_GetVinSensorGeo(&Mode, &VinSensorGeo) != -1) {
    		if (0) AmbaPrintColor(MAGENTA, "VinSensorGeo update X:%d Y:%d W:%d H:%d, Hsub:%d/%d, VSub:%d/%d to X:%d Y:%d W:%d H:%d",
    			VinSensorGeo.StartX, VinSensorGeo.StartY, VinSensorGeo.Width, VinSensorGeo.Height,
    			VinSensorGeo.HSubSample.FactorDen, VinSensorGeo.HSubSample.FactorNum, VinSensorGeo.VSubSample.FactorDen, VinSensorGeo.VSubSample.FactorNum,
    			WarpTable[Channel].WarpHeader.VinSensorGeo.StartX, WarpTable[Channel].WarpHeader.VinSensorGeo.StartY,
    			WarpTable[Channel].WarpHeader.VinSensorGeo.Width, WarpTable[Channel].WarpHeader.VinSensorGeo.Height);

    		VinSensorGeo.StartX = WarpTable[Channel].WarpHeader.VinSensorGeo.StartX;
    		VinSensorGeo.StartY = WarpTable[Channel].WarpHeader.VinSensorGeo.StartY;
    		VinSensorGeo.Width = WarpTable[Channel].WarpHeader.VinSensorGeo.Width;
    		VinSensorGeo.Height = WarpTable[Channel].WarpHeader.VinSensorGeo.Height;
    		AmbaDSP_WarpCore_SetVinSensorGeo(&Mode, &VinSensorGeo);
    	}
    }
    AmbaDSP_WarpCore_CalcDspWarp(&Mode, Config);
    AmbaDSP_WarpCore_SetDspWarp(&Mode);
    /*set the warp info for still mode*/
    //Mode.Pipe = AMBA_DSP_IMG_PIPE_STILL; //Current B5 doesn't support STILL. Img Warp Core Context id 4 > IMG_STILL_MAX_NUM 2
    //AmbaDSP_WarpCore_SetCalibWarpInfo(&Mode, &WarpInfo[Channel]);

    return Rval;
}

/**
 *  @brief initial function for warp calibration
 *
 *  initial function for warp calibration
 *
 *  @param [in] CalObj calibration object
 *
 *  @return 0 success, -1 failure
 */
int AppLibCalibWarp_Init(Cal_Obj_s *CalObj)
{
    int i;
    UINT8 *WarpAddress = CalObj->DramShadow;
    static UINT8 WarpInitFlag = 0;

    if (WarpInitFlag == 0) {
        memset(&AppWarpControl,0,sizeof(Warp_Control_s));
        WarpInitFlag = 1;
    }
    AppWarpControl.Enable = (UINT32)WarpAddress[CAL_WARP_ENABLE];
    //AppWarpControl.Enable = get_int((INT32*)&WarpAddress[CAL_WARP_ENABLE]);
    AmbaPrint("AppWarpControl.Enable %d",AppWarpControl.Enable);


    if (AppWarpControl.Enable) {
        AppWarpControl.WarpTableCount = (UINT32)WarpAddress[CAL_WARP_TABLE_COUNT];
        //AppWarpControl.WarpTableCount = get_int((INT32*)&WarpAddress[CAL_WARP_TABLE_COUNT]);
        for (i=0; i < MAX_WARP_TABLE_COUNT; i++) {
            AppWarpControl.WarpTable[i] = (Warp_Storage_s*)&WarpAddress[CAL_WARP_TABLE(i)];
            //AppWarpControl.WarpTable[i] =(Warp_Storage_s*) &WarpAddress[CAL_WARP_TABLE(i)];
            //AppWarpControl.WarpTable[i] =(Warp_Storage_s*) (WarpAddress+CAL_WARP_TABLE(i));
        }

        for (i=0; i < CALIB_CH_NO; i++) {
        	if (AppWarpControl.WarpTable[i]->WarpHeader.Enable) {
        		AmbaPrint("[AVM] Set default WARP table!");
        		AppLibCalibWarp_SelectWarpTable(1, i, 0, 0, 0x10000, 65536);
        	}
        }
    }

    AmbaPrint("version: 0x%x",AppWarpControl.WarpTable[0]->WarpHeader.Version);

    return 0;
}

/**
 *  @brief get multi lines from the text file
 *
 *  get multi lines from the text file
 *
 *  @param [in] argc the number of the input parameter
 *  @param [in] argv value of input parameter
 *
 *  @return 0 success, <0 failure
 */
int AppLibCalibWarp_GenWarpFromSpec(int Argc, char *Argv[])
{
    int I=0, J =0,   Rval;
    int IdxNumR = 0;
    int IdxNumE = 0;
    double RealWidth = 0, R = 0, Cos = 0, Sin = 0;
    INT16 TileWidthExp  =   7;
    INT16 TileHeightExp  =  7;
    INT32 StartX = 0;
    INT32 StartY = 0;
    Cal_Warp_Tool_Info_s ToolInfo = {0};//sharon
    AMBA_FS_FILE *Fid = NULL;
    AMBA_FS_FILE *FpScript = NULL;
    UINT32 Tmp;
    INT32 ExceptW = 0;
    INT32 ExceptH = 0;
    INT32 WarpHeader[32] ={0};
    int  ArgcR;
    char *ArgvR[MAX_CMD_TOKEN];
    char Buffer[256];
    char Change_line='\n';
    char Str[256];
    AMBA_FS_FILE *Fw_path =NULL,*Fr_path = NULL;
    int SensorFlag = 0;
    int RealFlag = 0;
    int ExpectFlag = 0;
    int CompensateRatioFlag = 1;
    int RawEncodeTextFlag = 0;
    int RatioRFlag = 1;
    int SensorCalFlag = 0;
    int SensorRawFlag = 0;
    Cal_Warp_Table_Info_s Info;
    char WarpHfileFn [20] = {'c',':','\\','w','a','r','p','_','b','Y','_','s','p','e','c','.','h','\0'};
    char WarpCalibFn[30] = {'c',':','\\','c','a','l','i','b','6','.','b','i','n','\0'};
    char WarpRawEncFn[30] = {'c',':','\\','w','a','r','p','R','a','w','E','n','c','o','d','e','.','b','i','n','\0'};
    char *WarpReadITunerFn;
    char *WarpWriteITunerFn;
    UINT8 WarpMaxIdxNum = 64;
    UINT8 En2StageCompensation = 1;

    void *TempWp,*TempOldWp, *TempMwWp, *TempReal,*TempExpect;
    void *TempWpBuffer,*TempMwWpBuffer,*TempOldWpBuffer,*TempRealBuffer,*TempExpectBuffer;
    UINT8 HSubSampleFactorNum=1, HSubSampleFactorDen=1, VSubSampleFactorNum=1, VSubSampleFactorDen=1;
    UINT32 WarpZoomStep = 0,WarpChannel= 0;
    AMBA_DSP_CHANNEL_ID_u Chan = {.Data = {0}, .Bits = { .VinID = 0, .SensorID = 0 } };
    UINT8 *BufferAddress;
    AMBA_SENSOR_MODE_INFO_s VinInfo;
	AMBA_SENSOR_MODE_ID_u SensorMode = {
        .Bits = {
            .Mode = 0,
            .VerticalFlip = 0,
        }
    };


    ToolInfo.Tile.TileWidth=64;
    ToolInfo.Tile.TileHeight  =64;
    ToolInfo.CAL.ImgWidth   =4608;
    ToolInfo.CAL.ImgHeight  =3456;
    ToolInfo.RatioR =100;
    ToolInfo.CompensateRatio =100;
    ToolInfo.CompensateRatioSwaX =100;
    ToolInfo.CompensateRatioSwaY =100;


    SensorMode.Bits.Mode = pAmbaCalibInfoObj[0/*AMBA_VIN_CHANNEL0*/]->AmbaVignetteMode;
    AmbaSensor_GetModeInfo(Chan, SensorMode, &VinInfo);
    StartX = VinInfo.InputInfo.PhotodiodeArray.StartX;
    StartY = VinInfo.InputInfo.PhotodiodeArray.StartY;

    AmbaPrint("START_X = %d, START_y= %d",StartX,StartY);

    RawEncodeTextFlag = atoi(Argv[3]);

    //use Big array, release in the end of this function.

    Rval = AmpUtil_GetAlignedPool(&G_NC_MMPL, &TempReal, &TempRealBuffer, WarpMaxIdxNum*sizeof(double), 32);
    BufferAddress = (UINT8*)((UINT32)TempReal) ;
    if (Rval != OK) {
        AmbaPrint("ToolInfo.Real allocate fail in %s:%u", __FUNCTION__, __LINE__);
    } else {
        ToolInfo.Real = (double *)BufferAddress;
        AmbaPrint("[AppLibCalibWarp]RawBufferfAddress (0x%08X) (%u)!", ToolInfo.Real, WarpMaxIdxNum*sizeof(double));
    }
    Rval = AmpUtil_GetAlignedPool(&G_NC_MMPL, &TempExpect, &TempExpectBuffer, WarpMaxIdxNum*sizeof(double), 32);
    BufferAddress = (UINT8*)((UINT32)TempExpect) ;
    if (Rval != OK) {
        AmbaPrint("ToolInfo.Expect allocate fail in %s:%u", __FUNCTION__, __LINE__);
    } else {
        ToolInfo.Expect = (double *)BufferAddress;
        AmbaPrint("[AppLibCalibWarp]RawBufferAddress (0x%08X) (%u)!", ToolInfo.Expect, WarpMaxIdxNum*sizeof(double));
    }

    // Read A line from the file
    AmbaPrint("Argc = %d", Argc);
    if (Argc == 7 || Argc == 5 ) {}
    else    {
        AmbaPrint(
            "t cal warp warp_spec: Generate WARP calibration table\n"
            "t cal warp warp_spec [raw encode text flag] [filename spec] [raw_input filename ] [raw_output filename] \n");
        return WARP_CALIB_INCORRECT_ARGUMENT_NUM;
    }
    if ((RawEncodeTextFlag == IDEA_WARP) || (RawEncodeTextFlag == IDEA_WARP_RAWENC) ||( RawEncodeTextFlag == DISTORTION_RAW) || \
            (RawEncodeTextFlag == SWA_XY_WARP)||(RawEncodeTextFlag ==SWA_XY_WARP_RAWENC)) {
    } else {
        AmbaPrint(
            "[raw encode text flag] = \n"
            "idea WARP ==>  0: raw encode disable , 1: raw encode enable, 99: generate the raw with distortion \n"
            "keep view angle WARPP (keep X & Y )==> 20 raw encode disable, 21:  raw encode enable \n"
            "t cal warp warp_spec [raw encode text flag] [filename spec] [raw_input filename ] [raw_output filename] \n");
        return WARP_CALIB_INCORRECT_PARAMETER;
    }
    if ((RawEncodeTextFlag == IDEA_WARP_RAWENC) ||(RawEncodeTextFlag == SWA_XY_WARP_RAWENC)) {
        if (Argc == 7) {
            WarpReadITunerFn = Argv[5];
            WarpWriteITunerFn = Argv[6];
        } else {
            AmbaPrint(
                "t cal warp warp_spec: Generate WARP calibration table\n"
                "t cal warp warp_spec [raw encode text flag] [filename spec] [raw_input filename ] [raw_output filename] \n");
            return WARP_CALIB_INCORRECT_PARAMETER;
        }
    }
    //text raw encode
    if ((RawEncodeTextFlag == IDEA_WARP_RAWENC) || (RawEncodeTextFlag == SWA_XY_WARP_RAWENC)) {
        Fr_path = AmbaFS_fopen(WarpReadITunerFn, "r");
        if (Fr_path == NULL) {
            AmbaPrint("AmbaFS_fopen %s fail.",WarpReadITunerFn);
            Rval = WARP_CALIB_OPEN_FILE_ERROR;
            return Rval;
        }
        Fw_path = AmbaFS_fopen(WarpWriteITunerFn, "w");
        if (Fw_path == NULL) {
            AmbaPrint("AmbaFS_fopen %s fail. ",WarpWriteITunerFn);
            Rval = WARP_CALIB_OPEN_FILE_ERROR;
            return Rval;
        }

        Rval = AmbaFS_fclose(Fr_path);
        if (Rval != NULL) {
            AmbaPrint("AmbaFS_fclose %s fail.",Argv[4]);
            Rval = WARP_CALIB_CLOSE_FILE_ERROR;
        }
        Rval = AmbaFS_fclose(Fw_path);
        if (Rval != NULL) {
            AmbaPrint("AmbaFS_fclose %s fail.",Argv[5]);
            Rval = WARP_CALIB_CLOSE_FILE_ERROR;
            return Rval;
        }
    }    ///text raw encode end

    //for generate 1D WARP table
    ToolInfo.Warp2StageFlag = 1; //set default value to 1
    if ((RawEncodeTextFlag == IDEA_WARP) || (RawEncodeTextFlag == IDEA_WARP_RAWENC) ||( RawEncodeTextFlag == DISTORTION_RAW) || \
            (RawEncodeTextFlag == SWA_XY_WARP)||(RawEncodeTextFlag ==SWA_XY_WARP_RAWENC)) {
        //set initial value for Info.OffSensorFlag
        Info.OffSensorFlag = 0;
        FpScript = AmbaFS_fopen(Argv[4], "r");
        if (FpScript == NULL) {
            AmbaPrint("FpScript AmbaFS_fopen %s fail.",Argv[4]);
            Rval = WARP_CALIB_OPEN_FILE_ERROR;
            return Rval;
        }
        while (1) {
            Rval = AppLib_MultiGetline(FpScript, Buffer);
            if (Rval < WARP_CALIB_OK) {
                break;
            } else {
                ArgcR = 0;
                memset(ArgvR, 0, MAX_CMD_TOKEN*sizeof(char *));
                {
                    // Parse the input string to multiple tokens
                    char *Token = strtok(Buffer, " ");
                    int NumToken = 0;
                    while (Token != NULL) {
                        ArgvR[NumToken] = Token;
                        NumToken++;
                        Token = strtok(NULL, " ");
                    }
                    ArgcR = NumToken;
                }
                if (strcmp(ArgvR[0], "TileWidth") == 0) {
                    ToolInfo.Tile.TileWidth = (INT16) atoi(ArgvR[1]);
                } else if (strcmp(ArgvR[0], "TileHeight") == 0) {
                    ToolInfo.Tile.TileHeight= (INT16) atoi(ArgvR[1]);
                } else if (strcmp(ArgvR[0], "off_sensor_calibration") == 0) {
                    SensorCalFlag = 1;
                    Info.OffSensorFlag = 1;
                    ToolInfo.CAL.ImgWidth = (INT32) atoi(ArgvR[1]);
                    ToolInfo.CAL.ImgHeight = (INT32) atoi(ArgvR[2]);
                    RealWidth = (double) atof(ArgvR[3]);
                    StartX = (INT32) atoi(ArgvR[4]);
                    StartY = (INT32) atoi(ArgvR[5]);
                    if (ArgcR == 8) {
                        ToolInfo.CAL.CenterX = (INT32) atoi(ArgvR[6]);
                        ToolInfo.CAL.CenterY = (INT32) atoi(ArgvR[7]);
                        if (ToolInfo.CAL.CenterX < (ToolInfo.CAL.ImgWidth*3>>3) || ToolInfo.CAL.CenterX > (ToolInfo.CAL.ImgWidth*5>>3)) {
                            SensorFlag = 0;
                            AmbaPrint("Error : CenterX out of the range ((ImgWidth*3/8) < CenterX < (ImgWidth*5/8))");
                        }
                        if (ToolInfo.CAL.CenterY < (ToolInfo.CAL.ImgHeight*3>>3)  || ToolInfo.CAL.CenterY > (ToolInfo.CAL.ImgHeight*5>>3)) {
                            SensorFlag = 0;
                            AmbaPrint("Error : CenterY out of the range ((ImgHeight*3/8) < CenterY < (ImgHeight*5/8))");
                        }
                    } else {
                        ToolInfo.CAL.CenterX = ToolInfo.CAL.ImgWidth>>1;
                        ToolInfo.CAL.CenterY = ToolInfo.CAL.ImgHeight>>1;
                    }
                    ToolInfo.UnitCellSize = RealWidth / ToolInfo.CAL.ImgWidth;
                    ToolInfo.Tile.HorGridNum = (ToolInfo.CAL.ImgWidth+ToolInfo.Tile.TileWidth-1)/ToolInfo.Tile.TileWidth + 1 ;
                    ToolInfo.Tile.VerGridNum = (ToolInfo.CAL.ImgHeight+ToolInfo.Tile.TileHeight-1)/ToolInfo.Tile.TileHeight+ 1 ;
                } else if (strcmp(ArgvR[0], "off_sensor_raw_image") == 0) {
                    SensorRawFlag = 1;
                    Info.OffSensorFlag = 1;
                    Info.ImgW = (INT32) atoi(ArgvR[1]);
                    Info.ImgH = (INT32) atoi(ArgvR[2]);
                    Info.StartX = (INT32) atoi(ArgvR[3]);
                    Info.StartY = (INT32) atoi(ArgvR[4]);

                } else if (strcmp(ArgvR[0], "Enable2StageWarp") == 0) {
                    ToolInfo.Warp2StageFlag = (INT8) atoi(ArgvR[1]);
                    AmbaPrint("Enable2StageWarp = %d",ToolInfo.Warp2StageFlag);
                } else if (strcmp(ArgvR[0], "sensor") == 0) {
                    SensorFlag = 1;
                    Info.ImgW = ToolInfo.CAL.ImgWidth = (INT32) atoi(ArgvR[1]);
                    Info.ImgH = ToolInfo.CAL.ImgHeight = (INT32) atoi(ArgvR[2]);
                    RealWidth = (double) atof(ArgvR[3]);
                    if (ArgcR == 6) {
                        ToolInfo.CAL.CenterX = (INT32) atoi(ArgvR[4]);
                        ToolInfo.CAL.CenterY = (INT32) atoi(ArgvR[5]);
                        AmbaPrint("center x = %d center y = %d",ToolInfo.CAL.CenterX,ToolInfo.CAL.CenterY);
                        if (ToolInfo.CAL.CenterX < (ToolInfo.CAL.ImgWidth*3>>3) || ToolInfo.CAL.CenterX > (ToolInfo.CAL.ImgWidth*5>>3)) {
                            SensorFlag = 0;
                            AmbaPrint("Error : CenterX out of the range ((ImgWidth*3/8) < CenterX < (ImgWidth*5/8))");
                        }
                        if (ToolInfo.CAL.CenterY < (ToolInfo.CAL.ImgHeight*3>>3)  || ToolInfo.CAL.CenterY > (ToolInfo.CAL.ImgHeight*5>>3)) {
                            SensorFlag = 0;
                            AmbaPrint("Error : CenterY out of the range ((ImgHeight*3/8) < CenterY < (ImgHeight*5/8))");
                        }
                    } else {
                        ToolInfo.CAL.CenterX = ToolInfo.CAL.ImgWidth>>1;
                        ToolInfo.CAL.CenterY = ToolInfo.CAL.ImgHeight>>1;
                    }
                    ToolInfo.UnitCellSize = RealWidth / ToolInfo.CAL.ImgWidth;
                    ToolInfo.Tile.HorGridNum = (ToolInfo.CAL.ImgWidth+ToolInfo.Tile.TileWidth-1)/ToolInfo.Tile.TileWidth + 1 ;
                    ToolInfo.Tile.VerGridNum = (ToolInfo.CAL.ImgHeight+ToolInfo.Tile.TileHeight-1)/ToolInfo.Tile.TileHeight+ 1 ;
                    AmbaPrint("cell size = %f HorGridNum = %d VerGridNum = %d",ToolInfo.UnitCellSize,ToolInfo.Tile.HorGridNum,ToolInfo.Tile.VerGridNum);
                    ToolInfo.Tile.AddTableGridNumW =0;
                    ToolInfo.Tile.AddTableGridNumH = 0;

                } else if (strcmp(ArgvR[0], "real") == 0) {
                    IdxNumR = ArgcR - 1;
                    for ( I=1; I<ArgcR; I++) {
                        RealFlag = 1;
                        ToolInfo.Real[I-1] = (double) atof(ArgvR[I]);
                    }
                } else if (strcmp(ArgvR[0], "expect") == 0) {
                    IdxNumE = ArgcR - 1;
                    for ( I=1; I<ArgcR; I++) {
                        ExpectFlag = 1;
                        ToolInfo.Expect[I-1] = (double) atof(ArgvR[I]);
                    }
                } else if (strcmp(ArgvR[0], "Compensate_ratio") == 0) {
                    ToolInfo.CompensateRatio = atoi(ArgvR[1]);
                    if (ToolInfo.CompensateRatio<0 || ToolInfo.CompensateRatio>200) {
                        AmbaPrint("Compensate_ratio must have be 0~200.");
                        CompensateRatioFlag = 0;
                    }
                } else if (strcmp(ArgvR[0], "Compensate_ratio_swa_y") == 0) {
                    ToolInfo.CompensateRatioSwaY = atoi(ArgvR[1]);
                    AmbaPrint("Compensate_ratio_swa_y %d",ToolInfo.CompensateRatioSwaY);
                    if (ToolInfo.CompensateRatioSwaY<0 || ToolInfo.CompensateRatioSwaY>200) {
                        AmbaPrint("Compensate_ratio_swa_y must have be 0~200.");
                        CompensateRatioFlag = 0;
                    }
                } else if (strcmp(ArgvR[0], "Compensate_ratio_swa_x") == 0) {
                    ToolInfo.CompensateRatioSwaX = atoi(ArgvR[1]);
                    AmbaPrint("Compensate_ratio_swa_x %d",ToolInfo.CompensateRatioSwaX);
                    if (ToolInfo.CompensateRatioSwaX<0 || ToolInfo.CompensateRatioSwaX>200) {
                        AmbaPrint("Compensate_ratio_swa_x must have be 0~200.");
                        CompensateRatioFlag = 0;
                    }
                } else if (strcmp(ArgvR[0], "r_ratio") == 0) {
                    ToolInfo.RatioR = atoi(ArgvR[1]);
                    if (ToolInfo.RatioR<=0 || ToolInfo.RatioR>200) {
                        AmbaPrint("r_ratio must have be 1~200.");
                        RatioRFlag = 0;
                    }
                } else if (strcmp(ArgvR[0], "zoom_step") == 0) {
                    WarpZoomStep = atoi(ArgvR[1]);
                }
            }
        }
        Rval = AmbaFS_fclose(FpScript);
        if (Rval != NULL) {
            AmbaPrint("FpScript AmbaFS_fclose fail .");
            Rval = WARP_CALIB_CLOSE_FILE_ERROR;
            return Rval;
        }
        if (Info.OffSensorFlag ) {
            SensorFlag = (SensorRawFlag & SensorCalFlag);
        }
        if ((SensorFlag == 0)||(RealFlag == 0)||(ExpectFlag == 0)||(CompensateRatioFlag == 0)||(RatioRFlag == 0)) {
            if (SensorFlag == 0)             AmbaPrint(" (off) Sensor parameter not Found or some error");
            if (RealFlag == 0)                AmbaPrint("Real warp spec parameter not Found or some error");
            if (ExpectFlag == 0)             AmbaPrint("Expect warp spec parameter not Found or some error");
            if (CompensateRatioFlag == 0)    AmbaPrint("CompensateRatio (swa_x/swa_y) parameter not Found or some error");
            if (RatioRFlag == 0)             AmbaPrint("R_ratio parameter not Found or some error");
            Rval = WARP_CALIB_INCORRECT_PARAMETER;
            return Rval;
        }
        if (IdxNumE != IdxNumR) {
            AmbaPrint("Real spec Index num (%d) != Expect spec Index num (%d)",IdxNumR,IdxNumE);
            Rval = WARP_CALIB_INCORRECT_INPUT_NUM;
            return Rval;
        }
        if ((IdxNumR > WarpMaxIdxNum) || (IdxNumE > WarpMaxIdxNum)) {
            AmbaPrint("the input real/expect parameters should be smaller than %d",WarpMaxIdxNum);
            Rval = WARP_CALIB_TOO_MUCH_INPUT_NUM;
            return Rval;
        }
        ToolInfo.IdxNum = IdxNumE;
        for (I = 0; I < ToolInfo.IdxNum;I++) {
            ToolInfo.Real[I] = ToolInfo.Real[I]/ToolInfo.UnitCellSize; //in pixel
            ToolInfo.Expect[I] = ToolInfo.Expect[I]/ToolInfo.UnitCellSize;//in pixel

        }
        AmbaPrint("ToolInfo.Expect[IdxNum-1]  %f",ToolInfo.Expect[ToolInfo.IdxNum-1] );

        Rval = AmpUtil_GetAlignedPool(&G_NC_MMPL, &TempMwWp, &TempMwWpBuffer, ToolInfo.Tile.HorGridNum * ToolInfo.Tile.VerGridNum*sizeof(AMBA_DSP_IMG_GRID_POINT_s), 32);
        BufferAddress = (UINT8*)((UINT32)TempMwWp) ;
        if (Rval != OK) {
            AmbaPrint("ToolInfo.MwWp allocate fail in %s:%u", __FUNCTION__, __LINE__);
        } else {
            ToolInfo.MwWp = (AMBA_DSP_IMG_GRID_POINT_s *)BufferAddress;
            AmbaPrint("[AppLibCalibWarp]RawBufferAddress (0x%08X) (%u)!", ToolInfo.MwWp, ToolInfo.Tile.HorGridNum * ToolInfo.Tile.VerGridNum*sizeof(AMBA_DSP_IMG_GRID_POINT_s));
        }

        R = pow((ToolInfo.CAL.ImgWidth*ToolInfo.CAL.ImgWidth + ToolInfo.CAL.ImgHeight*ToolInfo.CAL.ImgHeight), 0.5);
        if (R ==0 ) {
            AmbaPrint("[AppLibCalibWarp] ERROR! div 0 ");
            Rval = WARP_CALIB_DIV0;
            return Rval;
        }
        Cos = ToolInfo.CAL.ImgWidth/R;
        Sin = ToolInfo.CAL.ImgHeight/R;
        ExceptW = (UINT32)(ToolInfo.Expect[ToolInfo.IdxNum-1] * Cos);
        ExceptH = (UINT32)(ToolInfo.Expect[ToolInfo.IdxNum-1] * Sin);

        if ((RawEncodeTextFlag == SWA_XY_WARP)||(RawEncodeTextFlag == SWA_XY_WARP_RAWENC)) { // if gen a special warp table have be a bigger table
            ToolInfo.Tile.AddTableGridNumW =((ExceptW>>1)/ToolInfo.Tile.TileWidth )<<1 ;
            ToolInfo.Tile.AddTableGridNumH = ((ExceptH>>1)/ToolInfo.Tile.TileHeight)<<1;
            ToolInfo.Tile.HorGridNum += (ToolInfo.Tile.AddTableGridNumW )<<1;
            ToolInfo.Tile.VerGridNum  += (ToolInfo.Tile.AddTableGridNumH )<<1;
        }

        AmbaPrint("AddTableGridNumW  %d  AddTableGridNumH  %d",ToolInfo.Tile.AddTableGridNumW,ToolInfo.Tile.AddTableGridNumH);
        AmbaPrint("HorGridNum  %d, VerGridNum  %d",ToolInfo.Tile.HorGridNum,ToolInfo.Tile.VerGridNum);

        //use Big array, release in the end of this function.
        Rval = AmpUtil_GetAlignedPool(&G_NC_MMPL, &TempWp, &TempWpBuffer, ToolInfo.Tile.HorGridNum * ToolInfo.Tile.VerGridNum*sizeof(Warp_TOOL_IMG_GRID_POINT_s), 32);
        BufferAddress = (UINT8*)((UINT32)TempWp) ;
        if (Rval != OK) {
            AmbaPrint("ToolInfo.Wp allocate fail in %s:%u", __FUNCTION__, __LINE__);
            return Rval;
        } else {
            ToolInfo.Wp = (Warp_TOOL_IMG_GRID_POINT_s *)BufferAddress;
            AmbaPrint("[AppLibCalibWarp]RawBufferAddress (0x%08X) (%u)!", ToolInfo.Wp, ToolInfo.Tile.HorGridNum * ToolInfo.Tile.VerGridNum*sizeof(Warp_TOOL_IMG_GRID_POINT_s));
        }
        Rval = AmpUtil_GetAlignedPool(&G_NC_MMPL, &TempOldWp, &TempOldWpBuffer, ToolInfo.Tile.HorGridNum * ToolInfo.Tile.VerGridNum*sizeof(Warp_TOOL_IMG_GRID_POINT_s), 32);
        BufferAddress = (UINT8*)((UINT32)TempOldWp) ;
        if (Rval != OK) {
            AmbaPrint("ToolInfo.OldWp allocate fail in %s:%u", __FUNCTION__, __LINE__);
            return Rval;
        } else {
            ToolInfo.OldWp = (Warp_TOOL_IMG_GRID_POINT_s *)BufferAddress;
            AmbaPrint("[AppLibCalibWarp]RawBufferAddress (0x%08X) (%u)!", ToolInfo.OldWp, ToolInfo.Tile.HorGridNum * ToolInfo.Tile.VerGridNum*sizeof(Warp_TOOL_IMG_GRID_POINT_s));
        }
        Rval= AmbaCalib_CalWarpTable(&ToolInfo, RawEncodeTextFlag);
        if (Rval != OK) {
            AmbaPrint("[AppLibCalibWarp]Error Rval: NG!");
            Rval = WARP_CALIB_DIV0;
            return Rval;
        }
    }


    Fid = AmbaFS_fopen(WarpHfileFn, "w");
    if (Fid == NULL) {
        AmbaPrint("AmbaFS_fopen fail .");
        Rval = WARP_CALIB_OPEN_FILE_ERROR;
        return Rval;
    }
    //TileWidthExp;   /* 4:16, 5:32, 6:64, 7:128, 8:256, 9:512 */
    //TileWidthExp;   /* 4:16, 5:32, 6:64, 7:128, 8:256, 9:512 */
    switch (ToolInfo.Tile.TileWidth) {
        case 16:
            TileWidthExp = 4;
            break;
        case 32:
            TileWidthExp = 5;
            break;
        case 64:
            TileWidthExp = 6;
            break;
        case 128:
            TileWidthExp = 7;
            break;
        case 256:
            TileWidthExp = 8;
            break;
        case 512:
            TileWidthExp = 9;
            break;
        default:
            TileWidthExp = 7;//set default value to 7;
    }

    switch (ToolInfo.Tile.TileHeight) {
        case 16:
            TileHeightExp = 4;
            break;
        case 32:
            TileHeightExp = 5;
            break;
        case 64:
            TileHeightExp = 6;
            break;
        case 128:
            TileHeightExp = 7;
            break;
        case 256:
            TileHeightExp = 8;
            break;
        case 512:
            TileHeightExp = 9;
            break;
        default:
            TileHeightExp = 7;//set default value to 7;
    }

    WarpHeader[0]   = CAL_WARP_VER;
    WarpHeader[1]   = ToolInfo.Tile.HorGridNum;
    WarpHeader[2]   = ToolInfo.Tile.VerGridNum;
    WarpHeader[3]   = TileWidthExp;
    WarpHeader[4]   = TileHeightExp;
    WarpHeader[5]   = StartX;
    WarpHeader[6]   = StartY;
    WarpHeader[7]   = ToolInfo.CAL.ImgWidth;
    WarpHeader[8]   = ToolInfo.CAL.ImgHeight;
    WarpHeader[9]   = HSubSampleFactorNum;
    WarpHeader[10]  = HSubSampleFactorDen;
    WarpHeader[11]  = VSubSampleFactorNum;
    WarpHeader[12]  = VSubSampleFactorDen;
    WarpHeader[13]  = ToolInfo.Warp2StageFlag;
    WarpHeader[14]  = RESERVED2;
    WarpHeader[15]  = RESERVED3;
    //write warp header into file
    sprintf(Str, "%s",  "#define HorGridNum ");
    AmbaFS_fwrite(Str, strlen(Str), 1, Fid);
    sprintf(Str, " %5d ",  WarpHeader[1]);
    AmbaFS_fwrite(Str, strlen(Str), 1, Fid);
    AmbaFS_fwrite(&Change_line, 1, 1, Fid);
    sprintf(Str, "%s",  "#define VerGridNum ");
    AmbaFS_fwrite(Str, strlen(Str), 1, Fid);
    sprintf(Str, " %5d ",  WarpHeader[2]);
    AmbaFS_fwrite(Str, strlen(Str), 1, Fid);
    AmbaFS_fwrite(&Change_line, 1, 1, Fid);
    sprintf(Str, "%s",  "INT32 calib_warp_header_spec[64] =");
    AmbaFS_fwrite(Str, strlen(Str), 1, Fid);
    AmbaFS_fwrite(&Change_line, 1, 1, Fid);
    sprintf(Str, "%s",  "{");
    AmbaFS_fwrite(Str, strlen(Str), 1, Fid);
    AmbaFS_fwrite(&Change_line, 1, 1, Fid);
    sprintf(Str, "0x%X,",  CAL_WARP_VER);
    AmbaFS_fwrite(Str, strlen(Str), 1, Fid);
    AmbaFS_fwrite(&Change_line, 1, 1, Fid);
    for (I=1;I<32;I++) {
        sprintf(Str, " %5d, ", WarpHeader[I] );
        AmbaFS_fwrite(Str, strlen(Str), 1, Fid);
        AmbaFS_fwrite(&Change_line, 1, 1, Fid);
    }
    sprintf(Str, "%s",  "};");
    AmbaFS_fwrite(Str, strlen(Str), 1, Fid);
    AmbaFS_fwrite(&Change_line, 1, 1, Fid);
    AmbaFS_fwrite(&Change_line, 1, 1, Fid);

    //write warp table into file
    sprintf(Str, "%s",  "INT16 calib_warp_table_spec horizontal [HorGridNum * VerGridNum *2] =");
    AmbaFS_fwrite(Str, strlen(Str), 1, Fid);
    AmbaFS_fwrite(&Change_line, 1, 1, Fid);
    sprintf(Str, "%s",  "{");
    AmbaFS_fwrite(Str, strlen(Str), 1, Fid);
    AmbaFS_fwrite(&Change_line, 1, 1, Fid);
    for (J = 0; J < ToolInfo.Tile.HorGridNum * ToolInfo.Tile.VerGridNum; J++) {
        sprintf(Str, " %5d, ", ToolInfo.MwWp[J].X);
        AmbaFS_fwrite(Str, strlen(Str), 1, Fid);
        if (J % ToolInfo.Tile.HorGridNum == (ToolInfo.Tile.HorGridNum-1)) {
            AmbaFS_fwrite(&Change_line, 1, 1, Fid);
        }
    }
    sprintf(Str, "%s",  "};");
    AmbaFS_fwrite(Str, strlen(Str), 1, Fid);
    AmbaFS_fwrite(&Change_line, 1, 1, Fid);

    sprintf(Str, "%s",  "INT16 calib_warp_table_spec vertical [HorGridNum * VerGridNum *2] =");
    AmbaFS_fwrite(Str, strlen(Str), 1, Fid);
    AmbaFS_fwrite(&Change_line, 1, 1, Fid);
    sprintf(Str, "%s",  "{");
    AmbaFS_fwrite(Str, strlen(Str), 1, Fid);
    AmbaFS_fwrite(&Change_line, 1, 1, Fid);
    for (J = 0; J < ToolInfo.Tile.HorGridNum * ToolInfo.Tile.VerGridNum; J++) {
        sprintf(Str, " %5d, ", ToolInfo.MwWp[J].Y);
        AmbaFS_fwrite(Str, strlen(Str), 1, Fid);
        if (J % ToolInfo.Tile.HorGridNum == (ToolInfo.Tile.HorGridNum-1)) {
            AmbaFS_fwrite(&Change_line, 1, 1, Fid);
        }
    }
    sprintf(Str, "%s",  "};");
    AmbaFS_fwrite(Str, strlen(Str), 1, Fid);
    AmbaFS_fwrite(&Change_line, 1, 1, Fid);

    Rval = AmbaFS_fclose(Fid);

    if (Rval != NULL) {
        AmbaPrint("AmbaFS_fclose fail.");
        Rval = WARP_CALIB_CLOSE_FILE_ERROR;
        return Rval;
    }
#if 0
    slot = AMP_system_cmd(MW_GET_ACTIVE_SLOT, 0, 0);
    scardmgr_get_one_slot_info(slot, &info1);
    WarpCalibFn[0] = info1.drive_letter;
#endif
    Fid = AmbaFS_fopen(WarpCalibFn, "w");

    if (Fid == NULL) {
        AmbaPrint("AmbaFS_fopen %s fail.", WarpCalibFn);
        Rval = WARP_CALIB_OPEN_FILE_ERROR;
        return Rval;
    }
    //write for calibration data format
    J = 0;
    Tmp = WARP_ENABLE;          AmbaFS_fwrite(&Tmp, 4, 1, Fid);     J+=4;//WARP_ENABLE
    Tmp = COUNT_WARP_TABLE;     AmbaFS_fwrite(&Tmp, 4, 1, Fid);     J+=4;//WARP_TABLE_COUNT
    for (I = 16; I<32;I++ ) {
        Tmp = WarpHeader[I];    AmbaFS_fwrite(&Tmp, 2, 1, Fid);     J+=2;//Reserve[0]
    }
    Tmp = RESERVED1;            AmbaFS_fwrite(&Tmp, 4, 1, Fid);     J+=4;//Reserve[0]
    Tmp = RESERVED1;            AmbaFS_fwrite(&Tmp, 4, 1, Fid);     J+=4;//Reserve[1]
    Tmp = RESERVED1;            AmbaFS_fwrite(&Tmp, 4, 1, Fid);     J+=4;//Reserve[2]
    Tmp = RESERVED1;            AmbaFS_fwrite(&Tmp, 4, 1, Fid);     J+=4;//Reserve[3]
    Tmp = RESERVED1;            AmbaFS_fwrite(&Tmp, 4, 1, Fid);     J+=4;//Reserve[4]
    Tmp = RESERVED1;            AmbaFS_fwrite(&Tmp, 4, 1, Fid);     J+=4;//Reserve[5]
    Tmp = 1;                    AmbaFS_fwrite(&Tmp, 4, 1, Fid);     J+=4;//Warp table enable
    Tmp = CAL_WARP_VER;         AmbaFS_fwrite(&Tmp, 4, 1, Fid);     J+=4;//WARP version
    Tmp = ToolInfo.Tile.HorGridNum;          AmbaFS_fwrite(&Tmp, 4, 1, Fid);     J+=4;// Horizontal grid number
    Tmp = ToolInfo.Tile.VerGridNum;          AmbaFS_fwrite(&Tmp, 4, 1, Fid);     J+=4;//Vertical grid number
    Tmp = TileWidthExp;         AmbaFS_fwrite(&Tmp, 4, 1, Fid);     J+=4;// 4:16, 5:32, 6:64, 7:128, 8:256, 9:512
    Tmp = TileHeightExp;        AmbaFS_fwrite(&Tmp, 4, 1, Fid);     J+=4;// 4:16, 5:32, 6:64, 7:128, 8:256, 9:512
    Tmp = StartX;               AmbaFS_fwrite(&Tmp, 4, 1, Fid);     J+=4;// StartX
    Tmp = StartY;               AmbaFS_fwrite(&Tmp, 4, 1, Fid);     J+=4;// StartY
    Tmp = ToolInfo.CAL.ImgWidth;                AmbaFS_fwrite(&Tmp, 4, 1, Fid);     J+=4;// ImgWidth
    Tmp = ToolInfo.CAL.ImgHeight;           AmbaFS_fwrite(&Tmp, 4, 1, Fid);     J+=4;// ImgHeight
    Tmp = HSubSampleFactorNum;  AmbaFS_fwrite(&Tmp, 1, 1, Fid);     J+=1;// HSubSampleFactorNum
    Tmp = HSubSampleFactorDen;  AmbaFS_fwrite(&Tmp, 1, 1, Fid);     J+=1;// HSubSampleFactorDen
    Tmp = VSubSampleFactorNum;  AmbaFS_fwrite(&Tmp, 1, 1, Fid);     J+=1;// VSubSampleFactorNum
    Tmp = VSubSampleFactorDen;  AmbaFS_fwrite(&Tmp, 1, 1, Fid);     J+=1;// VSubSampleFactorDen
    Tmp = ToolInfo.Warp2StageFlag;AmbaFS_fwrite(&Tmp, 4, 1, Fid);   J+=4;// ToolInfo.Warp2StageFlag
    Tmp = RESERVED2;            AmbaFS_fwrite(&Tmp, 4, 1, Fid);     J+=4;// Reserved2
    Tmp = RESERVED3;            AmbaFS_fwrite(&Tmp, 4, 1, Fid);     J+=4;// Reserved3
    Tmp = WarpZoomStep;         AmbaFS_fwrite(&Tmp, 4, 1, Fid);     J+=4;// WarpZoomStep
    Tmp = WarpChannel;          AmbaFS_fwrite(&Tmp, 4, 1, Fid);     J+=4;// WarpChannel

    Rval = AmbaFS_fwrite(ToolInfo.MwWp, CAL_WARP_SIZE-(sizeof(Warp_Storage_Header_s)+64), 1, Fid);
    if (Rval == NULL) {
        AmbaPrint("AmbaFS_fwrite fail.");
        Rval = WARP_CALIB_CLOSE_FILE_ERROR;
        return Rval;
    }
    Rval = AmbaFS_fclose(Fid);
    if (Rval != NULL) {
        AmbaPrint("AmbaFS_fclose fail.");
        Rval = WARP_CALIB_CLOSE_FILE_ERROR;
        return Rval;
    }

    //save the warp calibration data for raw encode
    Fid = AmbaFS_fopen(WarpRawEncFn, "w");
    if (Fid == NULL) {
        AmbaPrint("AmbaFS_fopen %s fail.", WarpRawEncFn);
        Rval = WARP_CALIB_OPEN_FILE_ERROR;
        return Rval;
    }
    Rval = AmbaFS_fwrite(ToolInfo.MwWp, (ToolInfo.Tile.HorGridNum*ToolInfo.Tile.VerGridNum<<2), 1, Fid);
    if (Rval == NULL) {
        AmbaPrint("AmbaFS_fwrite fail.");
        Rval = WARP_CALIB_CLOSE_FILE_ERROR;
        return Rval;
    }
    Rval = AmbaFS_fclose(Fid);
    if (Rval != NULL) {
        AmbaPrint("AmbaFS_fclose fail.");
        Rval = WARP_CALIB_CLOSE_FILE_ERROR;
        return Rval;
    }


    {

	    AMBA_DSP_IMG_MODE_CFG_s Mode;

	    memset(&Mode, 0, sizeof(AMBA_DSP_IMG_MODE_CFG_s));

	    WarpInfo[0].Version                            = CAL_WARP_VER;
	    WarpInfo[0].HorGridNum                         = ToolInfo.Tile.HorGridNum;
	    WarpInfo[0].VerGridNum                         = ToolInfo.Tile.VerGridNum;
	    WarpInfo[0].TileWidthExp                       = TileWidthExp;
	    WarpInfo[0].TileHeightExp                      = TileHeightExp;
	    WarpInfo[0].VinSensorGeo.Width                 = ToolInfo.CAL.ImgWidth;
	    WarpInfo[0].VinSensorGeo.Height                = ToolInfo.CAL.ImgHeight;
	    WarpInfo[0].VinSensorGeo.StartX                = StartX;
	    WarpInfo[0].VinSensorGeo.StartY                = StartY;
	    WarpInfo[0].VinSensorGeo.HSubSample.FactorDen  = HSubSampleFactorDen;
	    WarpInfo[0].VinSensorGeo.HSubSample.FactorNum  = HSubSampleFactorNum;
	    WarpInfo[0].VinSensorGeo.VSubSample.FactorDen  = HSubSampleFactorDen;
	    WarpInfo[0].VinSensorGeo.VSubSample.FactorNum  = VSubSampleFactorNum;
	    WarpInfo[0].Enb2StageCompensation              = ((ToolInfo.Warp2StageFlag == 1) ? Calib2StageCompensation : IK2StageCompensation);
	    WarpInfo[0].Reserved1                          = RESERVED2;
	    WarpInfo[0].Reserved2                          = RESERVED3;
	    WarpInfo[0].pWarp                              = ToolInfo.MwWp;

	    AmbaPrint("HSubSampleFactorDen  %d",HSubSampleFactorDen);
	    AmbaPrint("HSubSampleFactorNum  %d",HSubSampleFactorNum);
	    AmbaPrint("HSubSampleFactorDen  %d",HSubSampleFactorDen);
	    AmbaPrint("VSubSampleFactorNum  %d",VSubSampleFactorNum);

	    /*set warp table*/
	    Mode.Pipe = AMBA_DSP_IMG_PIPE_VIDEO;
	    AmbaDSP_WarpCore_SetCalibWarpInfo(&Mode, &WarpInfo[0]);
	    AmbaDSP_WarpCore_CalcDspWarp(&Mode, 0);
	    AmbaDSP_WarpCore_SetDspWarp(&Mode);
	    /*set the warp info for still mode*/
	    Mode.Pipe = AMBA_DSP_IMG_PIPE_STILL;
	    AmbaDSP_WarpCore_SetCalibWarpInfo(&Mode, &WarpInfo[0]);

	}

    AmbaKAL_BytePoolFree(TempWpBuffer);
    AmbaKAL_BytePoolFree(TempOldWpBuffer);
    AmbaKAL_BytePoolFree(TempMwWpBuffer);
    AmbaKAL_BytePoolFree(TempRealBuffer);
    AmbaKAL_BytePoolFree(TempExpectBuffer);
    AmbaPrint("Warp_by_spec succeed.");
    return Rval;

}


/**
 *  @brief to generate the top view warp table
 *
 *  to generate the top view warp table
 *
 *  @param [in] argc the number of the input parameter
 *  @param [in] argv value of input parameter
 *  @param [in] ToolInfo structure for warp tool information
 *  @param [in] Point structure for top view points
 *  @param [in] Debugflag debug flag
 *  @param [in] ChId channel ID
 *
 *  @return 0 success, <0 failure
 */
int AppLibCalibWarp_GenTopViewTable(int Argc, char *Argv[], Cal_Warp_Tool_Info_s* ToolInfo, Top_View_Point_s* Point, UINT8 Debugflag,UINT8 ChId)
{
    INT32 I=0, J =0,  Rval = -1;
    INT16 TileWidthExp  =   7;
    INT16 TileHeightExp  =  7;
    AMBA_FS_FILE *Fid = NULL;
    UINT32 Tmp;
    INT32 WarpHeader[32] ={0};
    char Change_line='\n';
    char Str[256];
    void *TempWp,*TempOldWp,*TempMwWp;
    void *TempWpBuffer,*TempMwWpBuffer,*TempOldWpBuffer;
    UINT8 HSubSampleFactorNum=1, HSubSampleFactorDen=1, VSubSampleFactorNum=1, VSubSampleFactorDen=1;
    UINT32 WarpZoomStep = 0,WarpChannel= 0;
    Top_View_Hmatrix_s Matrix = {0};
    UINT8 *BufferAddress;
    char TopViewWarpHfileFn [20] ={'c',':','\\','\0'};
    char TopViewWarpCalibFn[30] = {'c',':','\\','\0'};
    char TopViewWarpRawEncFn[30] = {'c',':','\\','\0'};
    char RawEncFn[30] = {'_','R','a','w','E','n','c','o','d','e','\0'};
    char BinFn[5] = {'.','b','i','n','\0'};
    char HFn[3] = {'.','h','\0'};
    char FrontFn[3] = {'_','F','\0'};
    char BackFn[3] = {'_','B','\0'};
    char LeftFn[3] = {'_','L','\0'};
    char RightFn[3] = {'_','R','\0'};
    char MainFn[3] = {'M','\0'};
    char WayFn[3] = {0};

    if (ChId == Front) {
        strcat(WayFn, FrontFn);
    } else if (ChId == Back) {
        strcat(WayFn, BackFn);
    } else if (ChId == Left) {
        strcat(WayFn, LeftFn);
    } else if (ChId == Right) {
        strcat(WayFn, RightFn);
    } else if (ChId == MainFront) {
        strcat(WayFn, FrontFn);
        strcat(WayFn, MainFn);
    } else if (ChId == MainBack) {
        strcat(WayFn, BackFn);
        strcat(WayFn, MainFn);
    } else if (ChId == MainLeft) {
        strcat(WayFn, LeftFn);
        strcat(WayFn, MainFn);
    } else if (ChId == MainRight) {
        strcat(WayFn, RightFn);
        strcat(WayFn, MainFn);
    }

    strcat(TopViewWarpHfileFn, Argv[5]);
    strcat(TopViewWarpHfileFn, WayFn);
    strcat(TopViewWarpHfileFn, HFn);

    strcat(TopViewWarpCalibFn, Argv[5]);
    strcat(TopViewWarpCalibFn, WayFn);
    strcat(TopViewWarpCalibFn, BinFn);

    strcat(TopViewWarpRawEncFn, Argv[5]);
    strcat(TopViewWarpRawEncFn, WayFn);
    strcat(TopViewWarpRawEncFn, RawEncFn);
    strcat(TopViewWarpRawEncFn, BinFn);

    ToolInfo->Tile.AddTableGridNumW = 0;
    ToolInfo->Tile.AddTableGridNumH = 0;

    if (ToolInfo->ROI[ChId].ImgWidth == 0) {
        ToolInfo->ROI[ChId].StartX = ToolInfo->CAL.StartX;
        ToolInfo->ROI[ChId].StartY = ToolInfo->CAL.StartY;
        ToolInfo->ROI[ChId].ImgWidth= ToolInfo->CAL.ImgWidth;
        ToolInfo->ROI[ChId].ImgHeight= ToolInfo->CAL.ImgHeight;
    }

    if (ToolInfo->ROI[ChId].OpticalCenterX != 0) {
        ToolInfo->CAL.CenterX = ToolInfo->ROI[ChId].OpticalCenterX;
        ToolInfo->CAL.CenterY = ToolInfo->ROI[ChId].OpticalCenterY;
    }

    ToolInfo->Tile.HorGridNum = (ToolInfo->ROI[ChId].ImgWidth+ToolInfo->Tile.TileWidth-1)/ToolInfo->Tile.TileWidth + 1 ;
    ToolInfo->Tile.VerGridNum = (ToolInfo->ROI[ChId].ImgHeight+ToolInfo->Tile.TileHeight-1)/ToolInfo->Tile.TileHeight + 1 ;
    ToolInfo->Tile.AddTableGridNumW += (ToolInfo->Tile.HorGridNum>>2);
    ToolInfo->Tile.AddTableGridNumH += (ToolInfo->Tile.VerGridNum>>2);

    AmbaPrint("[ROI]cell size = %f  HorGridNum = %d VerGridNum = %d",ToolInfo->UnitCellSize,ToolInfo->Tile.HorGridNum,ToolInfo->Tile.VerGridNum);
    //use Big array, release in the end of this function.
    Rval = AmpUtil_GetAlignedPool(&G_NC_MMPL, &TempMwWp, &TempMwWpBuffer, ToolInfo->Tile.HorGridNum * ToolInfo->Tile.VerGridNum*sizeof(AMBA_DSP_IMG_GRID_POINT_s), 32);
    BufferAddress = (UINT8*)((UINT32)TempMwWp) ;
    if (Rval != OK) {
        AmbaPrint("ToolInfo->MwWp allocate fail in %s:%u", __FUNCTION__, __LINE__);
    } else {
        ToolInfo->MwWp = (AMBA_DSP_IMG_GRID_POINT_s *)BufferAddress;
        AmbaPrint("[AmpLibCalibWarp]rawBufferAddress (0x%08X) (%u)!", ToolInfo->MwWp, ToolInfo->Tile.HorGridNum * ToolInfo->Tile.VerGridNum*sizeof(AMBA_DSP_IMG_GRID_POINT_s));
    }

    for (J=0; J <4 ;J++) {
        Point->Output[J].X = Point->Output[J].X*ToolInfo->UnitCellSize;
        Point->Output[J].Y = Point->Output[J].Y*ToolInfo->UnitCellSize;
    }

    ToolInfo->Tile.HorGridNum += (ToolInfo->Tile.AddTableGridNumW )<<1;
    ToolInfo->Tile.VerGridNum += (ToolInfo->Tile.AddTableGridNumH )<<1;
    AmbaPrint("[ROI] AddTableGridNumW  %d, AddTableGridNumH  %d",ToolInfo->Tile.AddTableGridNumW,ToolInfo->Tile.AddTableGridNumH);
    AmbaPrint("[ROI] HorGridNum  %d, VerGridNum  %d",ToolInfo->Tile.HorGridNum,ToolInfo->Tile.VerGridNum);
    // To find how many grid we need. the grid inform need to generate according to the ROI information.

    //use Big array, release in the end of this function.
    Rval = AmpUtil_GetAlignedPool(&G_NC_MMPL, &TempWp, &TempWpBuffer, ToolInfo->Tile.HorGridNum * ToolInfo->Tile.VerGridNum*sizeof(Warp_TOOL_IMG_GRID_POINT_s), 32);
    BufferAddress = (UINT8*)((UINT32)TempWp) ;
    if (Rval != OK) {
        AmbaPrint("ToolInfo->Wp allocate fail in %s:%u", __FUNCTION__, __LINE__);
    } else {
        ToolInfo->Wp = (Warp_TOOL_IMG_GRID_POINT_s *)BufferAddress;
        AmbaPrint("[AmpLibCalibWarp]Wp rawBufferAddress (0x%08X) (%u)!", ToolInfo->Wp, ToolInfo->Tile.HorGridNum * ToolInfo->Tile.VerGridNum*sizeof(Warp_TOOL_IMG_GRID_POINT_s));
    }
    Rval = AmpUtil_GetAlignedPool(&G_NC_MMPL, &TempOldWp, &TempOldWpBuffer, ToolInfo->Tile.HorGridNum * ToolInfo->Tile.VerGridNum*sizeof(Warp_TOOL_IMG_GRID_POINT_s), 32);
    BufferAddress = (UINT8*)((UINT32)TempOldWp) ;
    if (Rval != OK) {
        AmbaPrint("ToolInfo->OldWp allocate fail in %s:%u", __FUNCTION__, __LINE__);
    } else {
        ToolInfo->OldWp = (Warp_TOOL_IMG_GRID_POINT_s *)BufferAddress;
        AmbaPrint("[AppLibCalibWarp]OldWp rawBufferAddress (0x%08X) (%u)!", ToolInfo->OldWp, ToolInfo->Tile.HorGridNum * ToolInfo->Tile.VerGridNum*sizeof(Warp_TOOL_IMG_GRID_POINT_s));
    }

    //Start to calculate the table
    Rval = AmbaCalib_CalTopViewTable(ToolInfo,Point, Matrix, Debugflag,ChId);

    if (Rval != OK) {
        AmbaPrint("[AppLibCalibWarp]Error Rval: NG!");
        Rval = WARP_CALIB_DIV0;
        return Rval;
    }

    //Start to write the table into h/bin files.
    Fid = AmbaFS_fopen(TopViewWarpHfileFn, "w");
    if (Fid == NULL) {
        AmbaPrint("AmbaFS_fopen fail .");
        Rval = WARP_CALIB_OPEN_FILE_ERROR;
        return Rval;
    }

    //TileWidthExp;   4:16, 5:32, 6:64, 7:128, 8:256, 9:512
    //TileWidthExp;   4:16, 5:32, 6:64, 7:128, 8:256, 9:512
    switch (ToolInfo->Tile.TileWidth) {
        case 16:
            TileWidthExp = 4;
            break;
        case 32:
            TileWidthExp = 5;
            break;
        case 64:
            TileWidthExp = 6;
            break;
        case 128:
            TileWidthExp = 7;
            break;
        case 256:
            TileWidthExp = 8;
            break;
        case 512:
            TileWidthExp = 9;
            break;
        default:
            TileWidthExp = 7;//set default value to 7;
    }

    switch (ToolInfo->Tile.TileHeight) {
        case 16:
            TileHeightExp = 4;
            break;
        case 32:
            TileHeightExp = 5;
            break;
        case 64:
            TileHeightExp = 6;
            break;
        case 128:
            TileHeightExp = 7;
            break;
        case 256:
            TileHeightExp = 8;
            break;
        case 512:
            TileHeightExp = 9;
            break;
        default:
            TileHeightExp = 7;//set default value to 7;
    }

    WarpHeader[0]   = CAL_WARP_VER;
    WarpHeader[1]   = ToolInfo->Tile.HorGridNum;
    WarpHeader[2]   = ToolInfo->Tile.VerGridNum;
    WarpHeader[3]   = TileWidthExp;
    WarpHeader[4]   = TileHeightExp;
    WarpHeader[5]   = ToolInfo->ROI[ChId].StartX;
    WarpHeader[6]   = ToolInfo->ROI[ChId].StartY;
    WarpHeader[7]   = ToolInfo->ROI[ChId].ImgWidth;
    WarpHeader[8]   = ToolInfo->ROI[ChId].ImgHeight;
    WarpHeader[9]   = HSubSampleFactorNum;
    WarpHeader[10]  = HSubSampleFactorDen;
    WarpHeader[11]  = VSubSampleFactorNum;
    WarpHeader[12]  = VSubSampleFactorDen;
    WarpHeader[13]  = ToolInfo.Warp2StageFlag;
    WarpHeader[14]  = RESERVED2;
    WarpHeader[15]  = RESERVED3;
    WarpHeader[16]   = ToolInfo->BirdEyeINFO.Birdeye.StartX;
    WarpHeader[17]   = ToolInfo->BirdEyeINFO.Birdeye.StartY;
    WarpHeader[18]   = ToolInfo->BirdEyeINFO.Birdeye.Width;
    WarpHeader[19]   = ToolInfo->BirdEyeINFO.Birdeye.Height;
    WarpHeader[20]   = ToolInfo->BirdEyeINFO.Car.StartX;
    WarpHeader[21]   = ToolInfo->BirdEyeINFO.Car.StartY;
    WarpHeader[22]   = ToolInfo->BirdEyeINFO.Car.Width;
    WarpHeader[23]   = ToolInfo->BirdEyeINFO.Car.Height;
    WarpHeader[24]   = ToolInfo->BirdEyeINFO.BlendPoint[0].X;
    WarpHeader[25]   = ToolInfo->BirdEyeINFO.BlendPoint[0].Y;
    WarpHeader[26]  = ToolInfo->BirdEyeINFO.BlendPoint[1].X;
    WarpHeader[27]  = ToolInfo->BirdEyeINFO.BlendPoint[1].Y;
    WarpHeader[28]  = ToolInfo->BirdEyeINFO.BlendPoint[2].X;
    WarpHeader[29]  = ToolInfo->BirdEyeINFO.BlendPoint[2].Y;
    WarpHeader[30]  = ToolInfo->BirdEyeINFO.BlendPoint[3].X;
    WarpHeader[31]  = ToolInfo->BirdEyeINFO.BlendPoint[3].Y;
    //write warp header into file
    sprintf(Str, "%s",  "#define HorGridNum ");
    AmbaFS_fwrite(Str, strlen(Str), 1, Fid);
    sprintf(Str, " %5d ",  WarpHeader[1]);
    AmbaFS_fwrite(Str, strlen(Str), 1, Fid);
    AmbaFS_fwrite(&Change_line, 1, 1, Fid);
    sprintf(Str, "%s",  "#define VerGridNum ");
    AmbaFS_fwrite(Str, strlen(Str), 1, Fid);
    sprintf(Str, " %5d ",  WarpHeader[2]);
    AmbaFS_fwrite(Str, strlen(Str), 1, Fid);
    AmbaFS_fwrite(&Change_line, 1, 1, Fid);
    sprintf(Str, "%s",  "INT32 calib_warp_header_spec[64] =");
    AmbaFS_fwrite(Str, strlen(Str), 1, Fid);
    AmbaFS_fwrite(&Change_line, 1, 1, Fid);
    sprintf(Str, "%s",  "{");
    AmbaFS_fwrite(Str, strlen(Str), 1, Fid);
    AmbaFS_fwrite(&Change_line, 1, 1, Fid);
    sprintf(Str, "0x%X,",  CAL_WARP_VER);
    AmbaFS_fwrite(Str, strlen(Str), 1, Fid);
    AmbaFS_fwrite(&Change_line, 1, 1, Fid);
    for (I=1;I<32;I++) {
        sprintf(Str, " %5d, ", WarpHeader[I] );
        AmbaFS_fwrite(Str, strlen(Str), 1, Fid);
        AmbaFS_fwrite(&Change_line, 1, 1, Fid);
    }
    sprintf(Str, "%s",  "};");
    AmbaFS_fwrite(Str, strlen(Str), 1, Fid);
    AmbaFS_fwrite(&Change_line, 1, 1, Fid);
    AmbaFS_fwrite(&Change_line, 1, 1, Fid);

    //write warp table into file
    sprintf(Str, "%s",  "INT16 calib_warp_table_spec horizontal [HorGridNum * VerGridNum *2] =");
    AmbaFS_fwrite(Str, strlen(Str), 1, Fid);
    AmbaFS_fwrite(&Change_line, 1, 1, Fid);
    sprintf(Str, "%s",  "{");
    AmbaFS_fwrite(Str, strlen(Str), 1, Fid);
    AmbaFS_fwrite(&Change_line, 1, 1, Fid);
    for (J = 0; J < ToolInfo->Tile.HorGridNum *ToolInfo-> Tile.VerGridNum; J++) {
        sprintf(Str, " %5d, ", ToolInfo->MwWp[J].X);
        AmbaFS_fwrite(Str, strlen(Str), 1, Fid);
        if (J % ToolInfo->Tile.HorGridNum == (ToolInfo->Tile.HorGridNum-1)) {
            AmbaFS_fwrite(&Change_line, 1, 1, Fid);
        }
    }
    sprintf(Str, "%s",  "};");
    AmbaFS_fwrite(Str, strlen(Str), 1, Fid);
    AmbaFS_fwrite(&Change_line, 1, 1, Fid);

    sprintf(Str, "%s",  "INT16 calib_warp_table_spec vertical [HorGridNum * VerGridNum *2] =");
    AmbaFS_fwrite(Str, strlen(Str), 1, Fid);
    AmbaFS_fwrite(&Change_line, 1, 1, Fid);
    sprintf(Str, "%s",  "{");
    AmbaFS_fwrite(Str, strlen(Str), 1, Fid);
    AmbaFS_fwrite(&Change_line, 1, 1, Fid);
    for (J = 0; J < ToolInfo->Tile.HorGridNum * ToolInfo->Tile.VerGridNum; J++) {
        sprintf(Str, " %5d, ", ToolInfo->MwWp[J].Y);
        AmbaFS_fwrite(Str, strlen(Str), 1, Fid);
        if (J % ToolInfo->Tile.HorGridNum == (ToolInfo->Tile.HorGridNum-1)) {
            AmbaFS_fwrite(&Change_line, 1, 1, Fid);
        }
    }
    sprintf(Str, "%s",  "};");
    AmbaFS_fwrite(Str, strlen(Str), 1, Fid);
    AmbaFS_fwrite(&Change_line, 1, 1, Fid);

    Rval = AmbaFS_fclose(Fid);

    if (Rval != NULL) {
        AmbaPrint("AmbaFS_fclose fail.");
        Rval = WARP_CALIB_CLOSE_FILE_ERROR;
        return Rval;
    }


    Fid = AmbaFS_fopen(TopViewWarpCalibFn, "w");

    if (Fid == NULL) {
        AmbaPrint("AmbaFS_fopen %s fail.", TopViewWarpCalibFn);
        Rval = WARP_CALIB_OPEN_FILE_ERROR;
        return Rval;
    }
    //write for calibration data format
    J = 0;
    Tmp = WARP_ENABLE;          AmbaFS_fwrite(&Tmp, 4, 1, Fid);     J+=4;//WARP_ENABLE
    Tmp = COUNT_WARP_TABLE;     AmbaFS_fwrite(&Tmp, 4, 1, Fid);     J+=4;//WARP_TABLE_COUNT
    for (I = 16; I<32;I++ ) {
        Tmp = WarpHeader[I];    AmbaFS_fwrite(&Tmp, 2, 1, Fid);     J+=2;//Reserve[0]
    }
    Tmp = RESERVED1;            AmbaFS_fwrite(&Tmp, 4, 1, Fid);     J+=4;//Reserve[0]
    Tmp = RESERVED1;            AmbaFS_fwrite(&Tmp, 4, 1, Fid);     J+=4;//Reserve[1]
    Tmp = RESERVED1;            AmbaFS_fwrite(&Tmp, 4, 1, Fid);     J+=4;//Reserve[2]
    Tmp = RESERVED1;            AmbaFS_fwrite(&Tmp, 4, 1, Fid);     J+=4;//Reserve[3]
    Tmp = RESERVED1;            AmbaFS_fwrite(&Tmp, 4, 1, Fid);     J+=4;//Reserve[4]
    Tmp = RESERVED1;            AmbaFS_fwrite(&Tmp, 4, 1, Fid);     J+=4;//Reserve[5]
    Tmp = 1;                    AmbaFS_fwrite(&Tmp, 4, 1, Fid);     J+=4;//Warp table enable
    Tmp = CAL_WARP_VER;         AmbaFS_fwrite(&Tmp, 4, 1, Fid);     J+=4;//WARP version
    Tmp = ToolInfo->Tile.HorGridNum;          AmbaFS_fwrite(&Tmp, 4, 1, Fid);     J+=4;// Horizontal grid number
    Tmp = ToolInfo->Tile.VerGridNum;          AmbaFS_fwrite(&Tmp, 4, 1, Fid);     J+=4;//Vertical grid number
    Tmp = TileWidthExp;         AmbaFS_fwrite(&Tmp, 4, 1, Fid);     J+=4;// 4:16, 5:32, 6:64, 7:128, 8:256, 9:512
    Tmp = TileHeightExp;        AmbaFS_fwrite(&Tmp, 4, 1, Fid);     J+=4;// 4:16, 5:32, 6:64, 7:128, 8:256, 9:512
    Tmp = ToolInfo->ROI[ChId].StartX;               AmbaFS_fwrite(&Tmp, 4, 1, Fid);     J+=4;// StartX
    Tmp = ToolInfo->ROI[ChId].StartY;               AmbaFS_fwrite(&Tmp, 4, 1, Fid);     J+=4;// StartY
    Tmp = ToolInfo->ROI[ChId].ImgWidth;                AmbaFS_fwrite(&Tmp, 4, 1, Fid);     J+=4;// ImgWidth
    Tmp = ToolInfo->ROI[ChId].ImgHeight;           AmbaFS_fwrite(&Tmp, 4, 1, Fid);     J+=4;// ImgHeight
    Tmp = HSubSampleFactorNum;  AmbaFS_fwrite(&Tmp, 1, 1, Fid);     J+=1;// HSubSampleFactorNum
    Tmp = HSubSampleFactorDen;  AmbaFS_fwrite(&Tmp, 1, 1, Fid);     J+=1;// HSubSampleFactorDen
    Tmp = VSubSampleFactorNum;  AmbaFS_fwrite(&Tmp, 1, 1, Fid);     J+=1;// VSubSampleFactorNum
    Tmp = VSubSampleFactorDen;  AmbaFS_fwrite(&Tmp, 1, 1, Fid);     J+=1;// VSubSampleFactorDen
    Tmp = ToolInfo.Warp2StageFlag;AmbaFS_fwrite(&Tmp, 4, 1, Fid);   J+=4;// ToolInfo.Warp2StageFlag
    Tmp = RESERVED2;            AmbaFS_fwrite(&Tmp, 4, 1, Fid);     J+=4;// Reserved2
    Tmp = RESERVED3;            AmbaFS_fwrite(&Tmp, 4, 1, Fid);     J+=4;// Reserved3
    Tmp = WarpZoomStep;         AmbaFS_fwrite(&Tmp, 4, 1, Fid);     J+=4;// WarpZoomStep
    Tmp = WarpChannel;          AmbaFS_fwrite(&Tmp, 4, 1, Fid);     J+=4;// WarpChannel

    Rval = AmbaFS_fwrite(ToolInfo->MwWp, CAL_WARP_SIZE-(sizeof(Warp_Storage_Header_s)+64), 1, Fid); // header of topview table: 64bytes
    if (Rval == NULL) {
        //AmbaPrint("AmbaFS_fwrite fail.");
        Rval = WARP_CALIB_CLOSE_FILE_ERROR;
        return Rval;
    }
    Rval = AmbaFS_fclose(Fid);
    if (Rval != NULL) {
        //AmbaPrint("AmbaFS_fclose fail.");
        Rval = WARP_CALIB_CLOSE_FILE_ERROR;
        return Rval;
    } else {
        AmbaPrint("Generate file %s. \n",TopViewWarpCalibFn);
    }

   //save the warp calibration data for raw encode
    Fid = AmbaFS_fopen(TopViewWarpRawEncFn, "w");

    if (Fid == NULL) {
        AmbaPrint("AmbaFS_fopen %s fail.", TopViewWarpRawEncFn);
        Rval = WARP_CALIB_OPEN_FILE_ERROR;
        return Rval;
    }
    AmbaPrint("sizeof(ToolInfo->MwWp  )   %d  sizeof (ToolInfo->Tile.HorGridNum*ToolInfo->Tile.VerGridNum<<2)  %d",sizeof(ToolInfo->MwWp),ToolInfo->Tile.HorGridNum*ToolInfo->Tile.VerGridNum<<2);
    Rval = AmbaFS_fwrite(ToolInfo->MwWp, (ToolInfo->Tile.HorGridNum*ToolInfo->Tile.VerGridNum<<2), 1, Fid);
    if (Rval == NULL) {
        AmbaPrint("AmbaFS_fwrite fail.");
        Rval = WARP_CALIB_CLOSE_FILE_ERROR;
        return Rval;
    }
    Rval = AmbaFS_fclose(Fid);
    if (Rval != NULL) {
        AmbaPrint("AmbaFS_fclose fail.");
        Rval = WARP_CALIB_CLOSE_FILE_ERROR;
        return Rval;
    }

    AmbaKAL_BytePoolFree(TempWpBuffer);
    AmbaKAL_BytePoolFree(TempOldWpBuffer);
    AmbaKAL_BytePoolFree(TempMwWpBuffer);

  return 0;
}


/**
 *  @brief to generate the top view warp table
 *
 *  to generate the top view warp table
 *
 *  @param [in] argc the number of the input parameter
 *  @param [in] argv value of input parameter
 *
 *  @return 0 success, <0 failure
 */
int AppLibCalibWarp_GenTopViewWarpFromSpec(int Argc, char * Argv [ ])
{
    INT32 I=0, Rval;
    INT32 IdxNumR = 0;
    INT32 IdxNumE = 0;
    Cal_Warp_Tool_Info_s ToolInfo = {0};//sharon
    AMBA_FS_FILE *FpScript = NULL;
    //double Threshold =0;
    //double UnitCellSize = 0.001348; // unit in mm
    INT32  ArgcR;
    char *ArgvR[MAX_CMD_TOKEN];
    char Buffer[256];
    Warp_TOOL_IMG_GRID_POINT_s BirdEyeCalPoint[8];
    //AMBA_FS_FILE *Fw_path,*Fr_path;
    UINT8 DetectFlag = 1;
    UINT8 TargetFlag = 1;
    UINT8 ChId = 0;
    UINT8 CellSizeFlag = 0;
    UINT8 RealFlag = 0;
    UINT8 ExpectFlag = 0;
    UINT8 ROIFlag[8] = 0; // ROIFlag[Ch]=1; ch:F/B/L/R/MF/MB/ML/MR
    UINT8 RawPointFlag[8] = {0}; //every ch have 4 points  F:0x1111 /L:0x1111 /R:0x1111 /B:0x1111
    UINT8 TargetPointFlag[8] = {0}; //every ch have 4 points  F:0x1111 /L:0x1111 /R:0x1111 /B:0x1111
    UINT8 BirdEyeFlag[4] = {0}; //[0]Birdeye:0x0001/[0]car:0x0010/[1]blend_point: 0x1111/[2]birdeye_point0-3:0x1111/[3]birdeye_point4-7:0x1111
    //UINT8 data_flag = 0;
    //UINT8 table_id = 0;
    UINT8 BEP1 = 0;
    INT32 CalNumX=0, CalDenX = 0;
    INT32 CalNumY=0, CalDenY = 0;

    UINT8 WarpMaxIdxNum = 64;
    void *TempReal,*TempExpect;
    void *TempRealBuffer,*TempExpectBuffer;
    Top_View_Point_s Point[8] = {0};//  1.raw  2. input (dewarp point ) 3. output(top view) (main)*2
    UINT8 DebugFlag = 0;
    AMBA_DSP_CHANNEL_ID_u Chan = {.Data = {0}, .Bits = { .VinID = 0, .SensorID = 0 } };
    UINT8 *BufferAddress;
    AMBA_SENSOR_MODE_INFO_s VinInfo;
    AMBA_SENSOR_MODE_ID_u SensorMode = {
                                            .Bits = {
                                                .Mode = 0,
                                                .VerticalFlip = 0,
                                            }
                                        };

    //Initial the parameters
    ToolInfo.Tile.TileWidth = ToolInfo.Tile.TileHeight = 64;
    ToolInfo.RatioR = ToolInfo.CompensateRatio =100;
    ToolInfo.CompensateRatioSwaX = ToolInfo.CompensateRatioSwaY =100;

    //SensorMode.Bits.Mode = AmbaVignetteMode1;
    AmbaSensor_GetModeInfo(Chan, SensorMode, &VinInfo);
    ToolInfo.CAL.StartX = VinInfo.OutputInfo.RecordingPixels.StartX;
    ToolInfo.CAL.StartY = VinInfo.OutputInfo.RecordingPixels.StartY;
    ToolInfo.CAL.ImgWidth   =1920;
    ToolInfo.CAL.ImgHeight  =1080;
    AmbaPrint("[CAL] START(x,y):(%d,%d) (W,H):(%d,%d)",ToolInfo.CAL.StartX,ToolInfo.CAL.StartY,ToolInfo.CAL.ImgWidth,ToolInfo.CAL.ImgHeight );

    ToolInfo.Threshold = (double) atof(Argv[3]);

    //use Big array, release in the end of this function.
    Rval = AmpUtil_GetAlignedPool(&G_NC_MMPL, &TempReal,&TempRealBuffer, WarpMaxIdxNum*sizeof(double), 32);
    BufferAddress = (UINT8*)((UINT32)TempRealBuffer) ;
    if (Rval != OK) {
        AmbaPrint("allocate fail in %s:%u", __FUNCTION__, __LINE__);
    } else {
        ToolInfo.Real = (double *)BufferAddress;
        AmbaPrint("[AppLibCalibWarp]RawBufferAddress (0x%08X) (%u)!", ToolInfo.Real, WarpMaxIdxNum*sizeof(double));
    }
    Rval = AmpUtil_GetAlignedPool(&G_NC_MMPL, &TempExpect, &TempExpectBuffer, WarpMaxIdxNum*sizeof(double),32);
    BufferAddress = (UINT8*)((UINT32)TempExpectBuffer) ;
    if (Rval != OK) {
        AmbaPrint("allocate fail in %s:%u", __FUNCTION__, __LINE__);
    } else {
        ToolInfo.Expect = (double *)BufferAddress;
        AmbaPrint("[AppLibCalibWarp]RawBufferAddress (0x%08X) (%u)!", ToolInfo.Expect, WarpMaxIdxNum*sizeof(double));
    }


    // Read A line from the file
    AmbaPrint("Argc = %d", Argc);
    if (Argc == 6 ) {
    } else {
        AmbaPrint(
        "t cal warp top_view: Generate top_view WARP calibration table\n"
        "t cal warp top_view [Threshold] [filename spec] [top view calibration output filename]\n"
        "[Threshold]: 0;\n"
        "[filename spec]: the path of the script \n");
        return WARP_CALIB_INCORRECT_ARGUMENT_NUM;
    }

    //for generate 1D WARP table
    FpScript = AmbaFS_fopen(Argv[4], "r");
    if (FpScript == NULL) {
        AmbaPrint("FpScript AmbaFS_fopen %s fail.",Argv[4]);
        Rval = WARP_CALIB_OPEN_FILE_ERROR;
        return Rval;
    }
    while (1) {
        Rval = AppLib_MultiGetline(FpScript, Buffer);
        if (Rval < WARP_CALIB_OK) {
            break;
        } else {
            ArgcR = 0;
            memset(ArgvR, 0, MAX_CMD_TOKEN*sizeof(char *));
            { // Parse the input string to multiple tokens
                char *Token = strtok(Buffer, " ");
                int NumToken = 0;
                while (Token != NULL) {
                    ArgvR[NumToken] = Token;
                    NumToken++;
                    Token = strtok(NULL, " ");
                }
                ArgcR = NumToken;
            }
            if (strcmp(ArgvR[0], "TileWidth") == 0) {
                ToolInfo.Tile.TileWidth = (INT16) atoi(ArgvR[1]);
            } else if (strcmp(ArgvR[0], "TileHeight") == 0) {
                ToolInfo.Tile.TileHeight = (INT16) atoi(ArgvR[1]);
            } else if (strcmp(ArgvR[0], "target_point") == 0) {
                if (strcmp(ArgvR[1], "Front") == 0) {
                    ChId = Front;
                } else if (strcmp(ArgvR[1], "Left") == 0) {
                    ChId = Left;
                } else if (strcmp(ArgvR[1], "Right") == 0) {
                    ChId = Right;
                } else if (strcmp(ArgvR[1], "Back") == 0) {
                    ChId = Back;
                } else if (strcmp(ArgvR[1], "MainFront") == 0) {
                    ChId = MainFront;
                } else if (strcmp(ArgvR[1], "MainLeft") == 0) {
                    ChId = MainLeft;
                } else if (strcmp(ArgvR[1], "MainRight") == 0) {
                    ChId = MainRight;
                } else if (strcmp(ArgvR[1], "MainBack") == 0) {
                    ChId = MainBack;
                } else {
                    AmbaPrint("Parameter error target_point");
                    return -1;
                }
                TargetPointFlag[ChId] |= (1<<atoi(ArgvR[2]));
                Point[ChId].Output[atoi(ArgvR[2])].X = (double) atoi(ArgvR[3]);
                Point[ChId].Output[atoi(ArgvR[2])].Y = (double) atoi(ArgvR[4]);
                TargetFlag = 0;
            } else if (strcmp(ArgvR[0], "raw_point") == 0) {
                if (strcmp(ArgvR[1], "Front") == 0) {
                    ChId = Front;
                } else if (strcmp(ArgvR[1], "Left") == 0) {
                    ChId = Left;
                } else if (strcmp(ArgvR[1], "Right") == 0) {
                    ChId = Right;
                } else if (strcmp(ArgvR[1], "Back") == 0) {
                    ChId = Back;
                } else if (strcmp(ArgvR[1], "MainFront") == 0) {
                    ChId = MainFront;
                } else if (strcmp(ArgvR[1], "MainLeft") == 0) {
                    ChId = MainLeft;
                } else if (strcmp(ArgvR[1], "MainRight") == 0) {
                    ChId = MainRight;
                } else if (strcmp(ArgvR[1], "MainBack") == 0) {
                    ChId = MainBack;
                } else {
                    AmbaPrint("Parameter error raw_point");
                    return -1;
                }
                DetectFlag = 0;
                RawPointFlag[ChId] |=(1<<atoi(ArgvR[2]));
                Point[ChId].Raw[atoi(ArgvR[2])].X = (double) atoi(ArgvR[3]);
                Point[ChId].Raw[atoi(ArgvR[2])].Y = (double) atoi(ArgvR[4]);
            } else if (strcmp(ArgvR[0], "birdeye_point") == 0) {
                if (atoi(ArgvR[1])<4) {
                    BirdEyeFlag[2] |= (1<<atoi(ArgvR[1]));
                } else if (atoi(ArgvR[1])<8) {
                    BirdEyeFlag[3] |= (1<<(atoi(ArgvR[1])-4));
                } else {
                    AmbaPrint("Error : birdeye_point parameter error.");
                }
                BirdEyeCalPoint[atoi(ArgvR[1])].X = atoi(ArgvR[2]);
                BirdEyeCalPoint[atoi(ArgvR[1])].Y = atoi(ArgvR[3]);
            } else if (strcmp(ArgvR[0], "birdeye") == 0) {
                BirdEyeFlag[0] |= 1;
                ToolInfo.BirdEyeINFO.Birdeye.StartX = (INT32) atoi(ArgvR[1]);
                ToolInfo.BirdEyeINFO.Birdeye.StartY = (INT32) atoi(ArgvR[2]);
                ToolInfo.BirdEyeINFO.Birdeye.Width = (INT32) atoi(ArgvR[3]);
                ToolInfo.BirdEyeINFO.Birdeye.Height = (INT32) atoi(ArgvR[4]);
            } else if (strcmp(ArgvR[0], "car") == 0) {
                BirdEyeFlag[0] |= 2;
                ToolInfo.BirdEyeINFO.Car.StartX = (INT32) atoi(ArgvR[1]);
                ToolInfo.BirdEyeINFO.Car.StartY = (INT32) atoi(ArgvR[2]);
                ToolInfo.BirdEyeINFO.Car.Width = (INT32) atoi(ArgvR[3]);
                ToolInfo.BirdEyeINFO.Car.Height = (INT32) atoi(ArgvR[4]);
            } else if (strcmp(ArgvR[0], "blend_point") == 0) {
                BirdEyeFlag[1] |= (1<<atoi(ArgvR[1]));
                ToolInfo.BirdEyeINFO.BlendPoint[atoi(ArgvR[1])].X = (INT32) atoi(ArgvR[2]);
                ToolInfo.BirdEyeINFO.BlendPoint[atoi(ArgvR[1])].Y = (INT32) atoi(ArgvR[3]);
            } else if (strcmp(ArgvR[0], "debug") == 0) {
                DebugFlag = 1;
            } else if (strcmp(ArgvR[0], "cell_size") == 0) {
                CellSizeFlag = 1 ;
                ToolInfo.UnitCellSize = (double) atof(ArgvR[1]);
            } else if (strcmp(ArgvR[0], "ROI") == 0) {
                if (strcmp(ArgvR[1], "Front") == 0) {
                    ChId = Front;
                } else if (strcmp(ArgvR[1], "Back") == 0) {
                    ChId = Back;
                } else if (strcmp(ArgvR[1], "Left") == 0) {
                    ChId = Left;
                } else if (strcmp(ArgvR[1], "Right") == 0) {
                    ChId = Right;
                } else if (strcmp(ArgvR[1], "MainFront") == 0) {
                    ChId = MainFront;
                } else if (strcmp(ArgvR[1], "MainLeft") == 0) {
                    ChId = MainLeft;
                } else if (strcmp(ArgvR[1], "MainRight") == 0) {
                    ChId = MainRight;
                } else if (strcmp(ArgvR[1], "MainBack") == 0) {
                    ChId = MainBack;
                }
                ToolInfo.ROI[ChId].StartX = (INT32) atoi(ArgvR[2]);
                ToolInfo.ROI[ChId].StartY = (INT32) atoi(ArgvR[3]);
                ToolInfo.ROI[ChId].ImgWidth  = (INT32) atoi(ArgvR[4]);
                ToolInfo.ROI[ChId].ImgHeight = (INT32) atoi(ArgvR[5]);
                ToolInfo.ROI[ChId].OpticalCenterX= (INT32) atoi(ArgvR[6]);
                ToolInfo.ROI[ChId].OpticalCenterY= (INT32) atoi(ArgvR[7]);
                ToolInfo.ROI[ChId].ShiftX = ToolInfo.ROI[ChId].StartX - ToolInfo.CAL.StartX;
                ToolInfo.ROI[ChId].ShiftY = ToolInfo.ROI[ChId].StartY -  ToolInfo.CAL.StartY;
                if ((ToolInfo.ROI[ChId].ShiftX<0) || (ToolInfo.ROI[ChId].ShiftY <0 )|| ((ToolInfo.ROI[ChId].ImgWidth+ToolInfo.ROI[ChId].StartX) > (ToolInfo.CAL.ImgWidth+ ToolInfo.CAL.StartX)) || \
                    ((ToolInfo.ROI[ChId].ImgHeight+ToolInfo.ROI[ChId].StartY) > (ToolInfo.CAL.ImgHeight+ ToolInfo.CAL.StartY))) {
                    AmbaPrint("the ROI have to be included with calibration window");
                    AmbaPrint("[ROI] start X/Y : %d, %d Width %d Height %d",ToolInfo.ROI[ChId].StartX,ToolInfo.ROI[ChId].StartY,ToolInfo.ROI[ChId].ImgWidth,ToolInfo.ROI[ChId].ImgHeight);
                    AmbaPrint("[CAL] start X/Y : %d, %d Width %d Height %d",ToolInfo.CAL.StartX,ToolInfo.CAL.StartY,ToolInfo.CAL.ImgWidth,ToolInfo.CAL.ImgHeight);

                    Rval = WARP_CALIB_INCORRECT_PARAMETER;
                    return Rval;
                }
                ROIFlag[ChId] = 1;
            } else if (strcmp(ArgvR[0], "real") == 0) {
                IdxNumR = ArgcR - 1;
                for ( I=1; I<ArgcR; I++) {
                    RealFlag = 1;
                    ToolInfo.Real[I-1] = (double) atof(ArgvR[I]);
                }
            } else if (strcmp(ArgvR[0], "expect") == 0) {
                IdxNumE = ArgcR - 1;
                for ( I=1; I<ArgcR; I++) {
                    ExpectFlag = 1;
                    ToolInfo.Expect[I-1] = (double) atof(ArgvR[I]);
                }
            }
        }
    }
    Rval = AmbaFS_fclose(FpScript);
    if (Rval != NULL) {
        AmbaPrint("FpScript AmbaFS_fclose fail .");
        Rval = WARP_CALIB_CLOSE_FILE_ERROR;
        return Rval;
    }

    if ((CellSizeFlag == 0)||(RealFlag == 0)||(ExpectFlag == 0)||(BirdEyeFlag[0] != 3)||(BirdEyeFlag[1] != 15)) {
        if (CellSizeFlag == 0) {
            AmbaPrint(" cell_size parameter not Found or some error");
        }
        if (RealFlag == 0) {
            AmbaPrint("Real warp spec parameter not Found or some error");
        }
        if (ExpectFlag == 0) {
            AmbaPrint("Expect warp spec parameter not Found or some error");
        }
        if (BirdEyeFlag[0] != 3) {
            AmbaPrint("BirdEye Information parameters not Found or some error");
        }
        if (BirdEyeFlag[1] != 15) {
            AmbaPrint("Blend points parameters not Found or some error");
        }
        Rval = WARP_CALIB_INCORRECT_PARAMETER;
        return Rval;
    }
    if (DetectFlag) {
        //start detect=================
        if (1) {
            //pre
            AppLibCalibAVM_CalDetectParameter();
            AppLibCalibAVM_CalDetectBuffer();
            AmpCalib_AVMGetBuffer(&DetectBuffer, &DetectParameter);

            UINT16 *RawBuffer;
            char DFn[32];
            AVM_Corner ResultCorner[4];

            //From SDcard
            AMBA_MEM_CTRL_s MemRawBuffer;
            sprintf(DFn, "c:\\0001.raw");
            AppLibCalibAVM_ReadRawImg(&MemRawBuffer, DFn);
            RawBuffer = (UINT16 *)MemRawBuffer.pMemAlignedBase;

            AmpCalib_AVMMarkDetection(RawBuffer, Raw_W, Raw_H, ResultCorner);
            for (int i=0 ; i<4; ++i) {
                AmbaPrint("TopLeft x: %d y: %d TopRight x: %d y: %d \nBottomLeft x: %d y: %d BottomRight x: %d y: %d \n",  ResultCorner[i].Corner[0].x, ResultCorner[i].Corner[0].y,
                    ResultCorner[i].Corner[1].x, ResultCorner[i].Corner[1].y,  ResultCorner[i].Corner[2].x, ResultCorner[i].Corner[2].y,  ResultCorner[i].Corner[3].x, ResultCorner[i].Corner[3].y);
            }

            //Write Raw Image
            sprintf(DFn, "c:\\out_all.raw");
            AppLibCalibAVM_WriteRawImg(RawBuffer, DFn);

            //Free Raw Image Buffer
            AmbaKAL_BytePoolFree(MemRawBuffer.pMemBase);

            for (int i=0 ; i<4; ++i) {
              for (int j=0; j<4; ++j) {
            	 Point[i].Raw[j].X = ResultCorner[i].Corner[j].x;
            	 Point[i].Raw[j].Y = ResultCorner[i].Corner[j].y;
              }
            }
            AppLibCalibAVM_CalDetectBufferFree();
        }
        //================
    } else {
        for (I=0; I<8; I++) {
            if (ROIFlag[I] == 1) {
                if (RawPointFlag[I] != 15) {
                    AmbaPrint(" ERROR! There are no raw points! ch: %d ",I);
                    Rval = WARP_CALIB_INCORRECT_PARAMETER;
                    return Rval;
                }
            }
        }
    }

    if (TargetFlag) { // get the target points by bird info
        for (I=2; I<4; I++) {
            if (BirdEyeFlag[I] != 15) {
                AmbaPrint(" ERROR! wrong Birdeye points, can't get target points!");
                Rval = WARP_CALIB_INCORRECT_PARAMETER;
                return Rval;
            }
        }
        BirdEyeFlag[0] = 15;
    } else { // get the target points by user
        for (I=0; I<8; I++) {
            if (ROIFlag[I] == 1) {
                if (TargetPointFlag[I] != 15) {
                    AmbaPrint(" ERROR! There are no target points! ch: %d ",I);
                    Rval = WARP_CALIB_INCORRECT_PARAMETER;
                    return Rval;
                }
            }
        }
    }

    if (IdxNumE != IdxNumR) {
        AmbaPrint("Real spec Index num (%d) != Expect spec Index num (%d)",IdxNumR,IdxNumE);
        Rval = WARP_CALIB_INCORRECT_INPUT_NUM;
        return Rval;
    }
    if ((IdxNumR > WarpMaxIdxNum) || (IdxNumE > WarpMaxIdxNum)) {
        AmbaPrint("the input real/expect parameters should be smaller than %d",WarpMaxIdxNum);
        Rval = WARP_CALIB_TOO_MUCH_INPUT_NUM;
        return Rval;
    }

    // Set the birdeye cal point to point every ch point (F/B/L/R)
    if (BirdEyeFlag[0] == 15) {
        //Front
        CalNumX = ToolInfo.BirdEyeINFO.Birdeye.StartX;
        CalDenX = ToolInfo.BirdEyeINFO.Birdeye.Width;
        CalNumY=  ToolInfo.BirdEyeINFO.Birdeye.StartY;
        CalDenY = ToolInfo.BirdEyeINFO.Car.StartY - ToolInfo.BirdEyeINFO.Birdeye.StartY;
        for (I = 0;I<4;I++) {
            BEP1 = I;
            Point[Front].Output[I].X = (double)(BirdEyeCalPoint[BEP1].X - CalNumX)* ToolInfo.ROI[Front].ImgWidth/CalDenX + ToolInfo.ROI[Front].ShiftX;
            Point[Front].Output[I].Y = (double)(BirdEyeCalPoint[BEP1].Y - CalNumY)* ToolInfo.ROI[Front].ImgHeight/CalDenY + ToolInfo.ROI[Front].ShiftY;
            if (DebugFlag) {
                AmbaPrint("[Front] target point %d, X %f Y %f ",I,Point[Front].Output[I].X,Point[Front].Output[I].Y);
            }
        }
        //Left
        CalNumX = ToolInfo.BirdEyeINFO.Birdeye.StartY+ToolInfo.BirdEyeINFO.Birdeye.Height;
        CalDenX = ToolInfo.BirdEyeINFO.Birdeye.Height;
        CalNumY=  ToolInfo.BirdEyeINFO.Birdeye.StartX;
        CalDenY = ToolInfo.BirdEyeINFO.Car.StartX - ToolInfo.BirdEyeINFO.Birdeye.StartX;
        for (I = 0;I<4;I++) {
            switch (I) {
                case 0 :
                    BEP1 = 6;
                    break;
                case 1 :
                    BEP1 = 0;
                    break;
                case 2 :
                    BEP1 = 4;
                    break;
                case 3 :
                    BEP1 = 2;
                    break;
            }
            Point[Left].Output[I].X = (double)(CalNumX - BirdEyeCalPoint[BEP1].Y)* ToolInfo.ROI[Left].ImgWidth/CalDenX + ToolInfo.ROI[Left].ShiftX;
            Point[Left].Output[I].Y = (double)(BirdEyeCalPoint[BEP1].X - CalNumY)* ToolInfo.ROI[Left].ImgHeight/CalDenY + ToolInfo.ROI[Left].ShiftY;
            if (DebugFlag) {
                AmbaPrint("[Left] target point %d, X %f Y %f  ",I,Point[Left].Output[I].X,Point[Left].Output[I].Y);
            }
        }
        //Right
        CalNumX = ToolInfo.BirdEyeINFO.Birdeye.StartY;
        CalDenX = ToolInfo.BirdEyeINFO.Birdeye.Height;
        CalNumY=  ToolInfo.BirdEyeINFO.Birdeye.Width + ToolInfo.BirdEyeINFO.Birdeye.StartX;
        CalDenY = (ToolInfo.BirdEyeINFO.Birdeye.Width + ToolInfo.BirdEyeINFO.Birdeye.StartX)- (ToolInfo.BirdEyeINFO.Car.Width + ToolInfo.BirdEyeINFO.Car.StartX);
        for (I = 0;I<4;I++) {
            switch (I) {
                case 0 :
                    BEP1 = 1;
                    break;
                case 1 :
                    BEP1 = 7;
                    break;
                case 2 :
                    BEP1 = 3;
                    break;
                case 3 :
                    BEP1 = 5;
                    break;
            }
            Point[Right].Output[I].X = (double)(BirdEyeCalPoint[BEP1].Y - CalNumX)* ToolInfo.ROI[Right].ImgWidth/CalDenX + ToolInfo.ROI[Right].ShiftX;
            Point[Right].Output[I].Y = (double)(CalNumY - BirdEyeCalPoint[BEP1].X)* ToolInfo.ROI[Right].ImgHeight/CalDenY + ToolInfo.ROI[Right].ShiftY;
            if (DebugFlag) {
                AmbaPrint("[Right] target point %d, X %f Y %f ",I,Point[Right].Output[I].X,Point[Right].Output[I].Y);
            }
        }
        //Back
        CalNumX = ToolInfo.BirdEyeINFO.Birdeye.StartX;
        CalDenX = ToolInfo.BirdEyeINFO.Birdeye.Width;
        CalNumY=  ToolInfo.BirdEyeINFO.Car.Height+ ToolInfo.BirdEyeINFO.Car.StartY;
        CalDenY = (ToolInfo.BirdEyeINFO.Birdeye.Height+ ToolInfo.BirdEyeINFO.Birdeye.StartY)-(ToolInfo.BirdEyeINFO.Car.Height+ ToolInfo.BirdEyeINFO.Car.StartY);
        for (I = 0;I<4;I++) {
            switch (I) {
                case 0 :
                    BEP1 = 4;
                    break;
                case 1 :
                    BEP1 = 5;
                    break;
                case 2 :
                    BEP1 = 6;
                    break;
                case 3 :
                    BEP1 = 7;
                    break;
            }
            Point[Back].Output[I].X = (double)(BirdEyeCalPoint[BEP1].X - CalNumX)* ToolInfo.ROI[Back].ImgWidth/CalDenX + ToolInfo.ROI[Back].ShiftX;
            Point[Back].Output[I].Y = (double)(BirdEyeCalPoint[BEP1].Y - CalNumY)* ToolInfo.ROI[Back].ImgHeight/CalDenY + ToolInfo.ROI[Back].ShiftY;
            if (DebugFlag) {
                AmbaPrint("[Back] target point %d, X %f Y %f  ",I,Point[Back].Output[I].X,Point[Back].Output[I].Y);
            }
        }
    }

    ToolInfo.IdxNum = IdxNumE;

    for (I = 0; I < ToolInfo.IdxNum;I++) {
        ToolInfo.Real[I] = ToolInfo.Real[I]/ToolInfo.UnitCellSize; //in pixel
        ToolInfo.Expect[I] = ToolInfo.Expect[I]/ToolInfo.UnitCellSize;//in pixel
    }

   //auto-calculate the F/B/L/R top view warp table
   for (ChId = 0; ChId<8; ChId++) {
        if (ROIFlag[ChId] == 0) {
            continue;
        } else {
            AmbaPrint("AppLibCalibWarp_GenTopViewTable ChId %d",ChId);
            Rval= AppLibCalibWarp_GenTopViewTable(Argc,Argv,&ToolInfo, &Point[ChId], DebugFlag,ChId);
            if (Rval!= OK) {
                AmbaPrint("AppLibCalibWarp_GenTopViewTable  return error %d",Rval);
                return Rval;
            }
        }
    }

    AmbaKAL_BytePoolFree(TempRealBuffer);
    AmbaKAL_BytePoolFree(TempExpectBuffer);
    AmbaPrint("Warp_by_spec succeed.");
    return Rval;

}

/**
 *  @brief merge warp table
 *
 *  merge warp table
 *
 *  @param [in] argc the number of the input parameter
 *  @param [in] argv value of input parameter
 *
 *  @return 0 success, <0 failure
 */
int AppLibCalibWarp_WarpMergeBinFile(int Argc, char *Argv[])
{
    INT I;

    AMBA_FS_FILE *Fp = 0,*Fid = 0;
    char WarpFileName[30] = {'c',':','\\','c','a','l','i','b','6','.','b','i','n','\0'};
    char CaFileName[30] = {'c',':','\\','c','a','l','i','b','1','3','.','b','i','n','\0'};
    char AscFname[64];
    char FrontFn[3] = {'_','F','\0'};
    char BackFn[3] = {'_','B','\0'};
    char LeftFn[3] = {'_','L','\0'};
    char RightFn[3] = {'_','R','\0'};
    char MainFn[3] = {'M','\0'};
    UINT32 Tmp = 0;
    UINT32 NvdSize = 0;
    UINT32 Offset;
    UINT8 *Buffer;
    UINT8 Reserved = 0;
    UINT32 TableCount;
    INT32 Rval;
    UINT32 Misalign;
    void *BufferAddress;
    UINT8 HeaderOffset = 32;


    Rval = AmbaKAL_BytePoolAllocate(&G_NC_MMPL, &BufferAddress, sizeof(Warp_Storage_s)+32, 100);
    Misalign = ((UINT32)BufferAddress) % 32;
    Buffer = (UINT8*)((UINT32)BufferAddress + (32-Misalign)) ;
    if (Rval != OK) {
        AmbaPrint("allocate fail in %s:%u", __FUNCTION__, __LINE__);
        return Rval;
    } else {
        AmbaPrint("[AppLibCalibWarp]Buffer (0x%08X) (%d)!", Buffer, (sizeof(Warp_Storage_s)+32));
    }
    if (strcmp(Argv[3], "sur_warp") == 0) {
        HeaderOffset = 64;
        Fp = AmbaFS_fopen(WarpFileName, "w");

        if (Fp == NULL) {
            AmbaPrint("AmbaFS_fopen %s fail.", WarpFileName);
            Rval = WARP_CALIB_OPEN_FILE_ERROR;
            return Rval;
        }
    } else if (strcmp(Argv[3], "warp") == 0) {
        Fp = AmbaFS_fopen(WarpFileName, "w");

        if (Fp == NULL) {
            AmbaPrint("AmbaFS_fopen %s fail.", WarpFileName);
            Rval = WARP_CALIB_OPEN_FILE_ERROR;
            return Rval;
        }
    } else if (strcmp(Argv[3], "ca") == 0) {
        Fp = AmbaFS_fopen(CaFileName, "w");

        if (Fp == NULL) {
            AmbaPrint("AmbaFS_fopen %s fail.", CaFileName);
            Rval = WARP_CALIB_OPEN_FILE_ERROR;
            return Rval;
        }
    } else {
        AmbaPrintColor(RED,"unknow format");
        return -1;
    }
    if (Fp == NULL) {
        AmbaPrint("Can't open the file %s\n", Argv[3]);
        return -1;
    }

    if (Argc >= 5) {
        if (strcmp(Argv[4], "repeat") == 0) {
            AmbaPrintColor(RED,"repeat");
            Tmp = 1;
            AmbaFS_fwrite(&Tmp,4,1,Fp);//enable
            TableCount = Tmp = atoi(Argv[5]);
            AmbaFS_fwrite(&Tmp,4,1,Fp);//table count
            Reserved = 0;
            I = HeaderOffset - 8;  // enable & table count = 8 bytes
            for (I; I >0; I--) {
                AmbaFS_fwrite(&Reserved,1,1,Fp);//Reserved
            }

            Fid = AmbaFS_fopen(Argv[6], "r");
            if (Fid == NULL) {
                AmbaPrint("Can't open the file %s\n", Argv[6]);
                return -1;
            }

            AmbaFS_fseek(Fid, HeaderOffset, AMBA_FS_SEEK_START);// 32 bytes for normal warp 64 bytes for surround view warp
            AmbaFS_fread(Buffer,sizeof(Warp_Storage_s),1,Fid);
            AmbaFS_fclose(Fid);

            for (I = 0; I < TableCount; I++) {
                AmbaFS_fwrite(Buffer,sizeof(Warp_Storage_s),1,Fp);

            }

            //fill zero to match the WARP size.
            Offset = TableCount*sizeof(Warp_Storage_s)+HeaderOffset;

            if ((strcmp(Argv[3], "sur_warp") == 0)||(strcmp(Argv[3], "warp") == 0)) {
                NvdSize = CAL_WARP_SIZE;
            } else if (strcmp(Argv[3], "ca") == 0) {
                NvdSize = CAL_CA_SIZE;
            }
            AmbaPrintColor(BLUE,"Offset = %d NvdSize = %d",Offset,NvdSize);
            /*
            for (i = Offset; i < NvdSize; i++) {
            AmbaFS_fwrite(&Reserved,1,1,Fp);
            }
            */
            Reserved = 0;
            AmbaFS_fwrite(&Reserved,1,(NvdSize-Offset),Fp);
            Rval = 0;
        }
        else if (strcmp(Argv[4], "merge") == 0) {
            AmbaPrintColor(RED,"merge");
            if (strcmp(Argv[3], "sur_warp") == 0) {

                Tmp = 1;
                AmbaFS_fwrite(&Tmp,4,1,Fp); //enable
                TableCount = atoi(Argv[6]);
                AmbaFS_fwrite(&TableCount,4,1,Fp);  //table count

                for (I = 0; I < TableCount; I++) {
                    char WayFn[3] = {0};

                    if (I == Front) {
                        strcat(WayFn, FrontFn);
                    } else if (I == Back) {
                        strcat(WayFn, BackFn);
                    } else if (I == Left) {
                        strcat(WayFn, LeftFn);
                    } else if (I == Right) {
                        strcat(WayFn, RightFn);
                    } else if (I == MainFront) {
                        strcat(WayFn, FrontFn);
                        strcat(WayFn, MainFn);
                    } else if (I == MainBack) {
                        strcat(WayFn, BackFn);
                        strcat(WayFn, MainFn);
                    } else if (I == MainLeft) {
                        strcat(WayFn, LeftFn);
                        strcat(WayFn, MainFn);
                    } else if (I == MainRight) {
                        strcat(WayFn, RightFn);
                        strcat(WayFn, MainFn);
                    }

                    sprintf(AscFname, "%s%s.bin", Argv[5], WayFn);
                    AmbaPrint("Open the file %s\n", AscFname);
                    Fid = AmbaFS_fopen(AscFname, "r");
                    if (Fid == NULL) {
                        AmbaPrint("Can't open the file %s finish merge.\n", AscFname);
                        AmbaFS_fclose(Fid);
                        break;
                    }
                    //set the birdview info
                    if (I == 0) {
                        AmbaFS_fseek(Fid, 0x0008, AMBA_FS_SEEK_START);
                        AmbaFS_fread(Buffer,0x0038,1,Fid);
                        AmbaFS_fwrite(Buffer,0x0038,1,Fp);
                    }

                    AmbaFS_fseek(Fid, HeaderOffset, AMBA_FS_SEEK_START);
                    AmbaFS_fread(Buffer,sizeof(Warp_Storage_s),1,Fid);
                    AmbaFS_fwrite(Buffer,sizeof(Warp_Storage_s),1,Fp);
                    AmbaFS_fclose(Fid);
                }
                //fill zero to match the WARP size.
                Offset = TableCount*sizeof(Warp_Storage_s)+HeaderOffset;
                NvdSize = CAL_WARP_SIZE;
                Reserved = 0;
                AmbaFS_fwrite(&Reserved,1,(NvdSize-Offset),Fp);

                Rval = 0;

            } else {
                Tmp = 1;
                AmbaFS_fwrite(&Tmp,4,1,Fp); //enable
                TableCount = atoi(Argv[6]);
                AmbaFS_fwrite(&TableCount,4,1,Fp);  //table count
                Reserved = 0;
                AmbaFS_fwrite(&Reserved,1,24,Fp);//Reserved

                for (I = 0; I < TableCount; I++) {
                    sprintf(AscFname, "%s%d.bin", Argv[5], I);

                    Fid = AmbaFS_fopen(AscFname, "r");
                    if (Fid == NULL) {
                        AmbaPrint("Can't open the file %s\n", AscFname);
                        return -1;
                    }
                    AmbaFS_fseek(Fid, HeaderOffset, AMBA_FS_SEEK_START);
                    AmbaFS_fread(Buffer,sizeof(Warp_Storage_s),1,Fid);
                    AmbaFS_fwrite(Buffer,sizeof(Warp_Storage_s),1,Fp);
                    AmbaFS_fclose(Fid);
                }
                //fill zero to match the WARP size.
                Offset = TableCount*sizeof(Warp_Storage_s)+HeaderOffset;

                if (strcmp(Argv[3], "warp") == 0) {
                    NvdSize = CAL_WARP_SIZE;
                }
                else if (strcmp(Argv[3], "ca") == 0) {
                    NvdSize = CAL_CA_SIZE;
                }
                /*
                for (i = Offset; i < NvdSize; i++) {
                AmbaFS_fwrite(&Reserved,1,1,Fp);
                }
                */
                Reserved = 0;
                AmbaFS_fwrite(&Reserved,1,(NvdSize-Offset),Fp);

                Rval = 0;

            }

        } else {
            AmbaPrintColor(RED,"else");
            Tmp = 1;
            AmbaFS_fwrite(&Tmp,4,1,Fp);//enable
            Tmp = Argc-4;
            AmbaFS_fwrite(&Tmp,4,1,Fp);//table count
            Reserved = 0;
            AmbaFS_fwrite(&Reserved,1,24,Fp);//Reserved

            for (I = 0; I <= (Argc-5); I++) {

                Fid = AmbaFS_fopen(Argv[4+I], "r");
                if (Fid == NULL) {
                    AmbaPrint("Can't open the file %s\n", Argv[4+I]);
                    return -1;
                }
                AmbaFS_fseek(Fid, HeaderOffset, AMBA_FS_SEEK_START);
                AmbaFS_fread(Buffer,sizeof(Warp_Storage_s),1,Fid);
                AmbaFS_fwrite(Buffer,sizeof(Warp_Storage_s),1,Fp);
                AmbaFS_fclose(Fid);
            }
            //fill zero to match the WARP size.
            Offset = (Argc-4)*sizeof(Warp_Storage_s)+8;

            if (strcmp(Argv[3], "warp") == 0) {
                NvdSize = CAL_WARP_SIZE;
            } else if (strcmp(Argv[3], "ca") == 0) {
                NvdSize = CAL_CA_SIZE;
            }
            //  AmbaPrintColor(BLUE,"Offset = %d NvdSize = %d",Offset,NvdSize);
            Reserved = 0;
            AmbaFS_fwrite(&Reserved,1,(NvdSize-Offset),Fp);
            Rval = 0;
        }
    } else {
        Rval = -1;
    }

    AmbaKAL_BytePoolFree(BufferAddress);
    AmbaFS_fclose(Fp);
    if (Rval == -1) {
        AmbaPrint("t cal warp merge_bin [ca/warp] [filename] [filename] [filename] ....  : combine ca/warp binary for amba calibration"
                  "t cal warp merge_bin [ca/warp] merge [filename] [num]: the file name is warp0.bin warp1.bin .... warp7.bin [filename]= driver:warp [num]= 8. "
                  "t cal warp merge_bin sur_warp merge [filename] [num]: the file name is warp_F.bin warp_B.bin ..... "
        		 );
    }

    return Rval;
}

/**
 *  @brief the upgrade function for warp calibration
 *
 *  the upgrade function for warp calibration
 *
 *  @param [in] CalSite calibration site status
 *  @param [in] CalObj calibration object
 *
 *  @return 0 success, <0 failure
 */
int AppLibCalibWarp_Upgrade(Cal_Obj_s *CalObj, Cal_Stie_Status_s* CalSite)
{
    if (CalObj->Version != CalSite->Version) {
        // This API is an example to handle calibration data upgrade
        AmbaPrint("[CAL] Site %s Version mismatch (FW:0x%08X, NAND:0x%08X)", CalObj->Name, CalObj->Version, CalSite->Version);
    }
    // The default behavior is to do-nothing when Version mismatch
    return 0;

}

/**
 *  @brief the entry function for Warp calibration function
 *
 *  the entry function for Warp calibration function
 *
 *  @param [in] Argc input number for WB calibration
 *  @param [in] Argv input value for WB calibration
 *  @param [in] CalSite calibration site status
 *  @param [in] CalObj calibration object
 *  @param [out] OutputStr output debug message
 *
 *  @return 0 success, <0 failure
 */
int AppLibCalibWarp_Func(int Argc, char *Argv[], char *OutputStr, Cal_Stie_Status_s *CalSite, Cal_Obj_s *CalObj)
{
    return 0;
}

/**
 *  @brief the unit test function for warp calibration
 *
 *  the unit test function for warp calibration
 *
 *  @param [in] env environment
 *  @param [in] Argc input number for WB calibration
 *  @param [in] Argv input value for WB calibration
 *
 *  @return 0 success, <0 failure
 */
int AppLibWarp_UTFunc(int Argc, char *Argv[])
{
    Cal_Obj_s           *CalObj;
    int Rval = -1;
    CalObj = AppLib_CalibGetObj(CAL_WARP_ID);
    if ((strcmp(Argv[2], "test") == 0)) {
        //register calibration site
        AppLib_CalibSiteInit();
        Rval = 0;
    } else if ((strcmp(Argv[2], "reset") == 0)) {
        AppLib_CalibMemReset(CAL_WARP_ID);
        Rval = 0;
    } else if ((strcmp(Argv[2], "init") == 0)) {
        AppLibCalibWarp_Init(CalObj);
        Rval = 0;
    } else if ((strcmp(Argv[2], "warp_spec") == 0)) {
        AppLibCalibWarp_GenWarpFromSpec(Argc,Argv);
        Rval = 0;
    } else if ((strcmp(Argv[2], "top_view") == 0)) {
        AppLibCalibWarp_GenTopViewWarpFromSpec(Argc,Argv);
        Rval = 0;
    } else if ((strcmp(Argv[2], "select") == 0)) {
        UINT8 Enable      = atoi(Argv[3]);
        UINT8 Channel     = atoi(Argv[4]);
        UINT8 Id1         = atoi(Argv[5]);
        UINT8 Id2         = atoi(Argv[6]);
        int BlendRatio    = atoi(Argv[7]);
        int Strength    = atoi(Argv[8]);
        Rval = AppLibCalibWarp_SelectWarpTable(Enable, Channel, Id1, Id2, BlendRatio, Strength);
    } else if ((strcmp(Argv[2], "merge_bin") == 0)) {
        AppLibCalibWarp_WarpMergeBinFile(Argc,Argv);
    } else if ((strcmp(Argv[2], "detect") == 0)) {
        AppLibCalibAVM_CalDetectParameter();
        AppLibCalibAVM_CalDetectBuffer();
        AmpCalib_AVMGetBuffer(&DetectBuffer, &DetectParameter);
        AppLibCalibAVM_DetectMark();
        AppLibCalibAVM_CalDetectBufferFree();
    }



    if (Rval == -1) {
        AmbaPrint("t cal warp init : re-init warp\n");
    }

    return Rval;
}


/**
 *  @brief read the raw image
 *
 *  read the raw image
 *
 *  @param [in] MemRawBuffer raw buffer address
 *  @param [in] Filename file name for the raw image
 *
 *  @return 0 success, <0 failure
 */
static UINT32 Raw_W, Raw_H, Raw_SIZE;
int AppLibCalibAVM_ReadRawImg(AMBA_MEM_CTRL_s *MemRawBuffer, const char *Filename)
{

    AMBA_FS_FILE *FidRaw = NULL;
    FidRaw = AmbaFS_fopen(Filename, "r");
    if (FidRaw == NULL) {
        AmbaPrint("CalibDetect NG: open raw img fail");
        return -1;
    }

    AmbaFS_fseek(FidRaw, 0, AMBA_FS_SEEK_END);
    int FileSize = AmbaFS_ftell(FidRaw);
    AmbaFS_fseek(FidRaw, 0, AMBA_FS_SEEK_START);

    if (FileSize == 24576000) {
        Raw_W = 4096;
        Raw_H = 3000;
    } else if (FileSize == 4147200) {
        Raw_W = 1920;
        Raw_H = 1080;
    } else if (FileSize == 16588800) {
        Raw_W = 7680;
        Raw_H = 1080;
    } else {
        Raw_W = (int)(4*sqrt(FileSize/24));
        Raw_H = (int)(3*sqrt(FileSize/24));
        //Raw_W = (int)16*sqrt(filesize/288);
        //Raw_H = (int)9*sqrt(filesize/288);
    }
    Raw_SIZE = FileSize/2;
    AmbaPrint("Raw_W:%d Raw_H:%d", Raw_W, Raw_H);

    if (AmpUtil_GetAlignedPool(&G_MMPL, &MemRawBuffer->pMemAlignedBase, &MemRawBuffer->pMemBase, sizeof(UINT16) * Raw_SIZE, 32) ) {
        AmbaPrint("Error callocing the MemRawBuffer");
    }
    AmbaFS_fread(MemRawBuffer->pMemAlignedBase, sizeof(UINT16), Raw_SIZE, FidRaw);
    AmbaFS_fclose(FidRaw);

    return 0;
}

/**
 *  @brief write the raw image
 *
 *  write the raw image
 *
 *  @param [in] RawBuffer raw buffer address
 *  @param [in] Filename file name for the raw image
 *
 *  @return 0 success, <0 failure
 */
int AppLibCalibAVM_WriteRawImg(UINT16 *RawBuffer, const char *Filename)
{
    AMBA_FS_FILE *FidRaw = NULL;

    FidRaw = AmbaFS_fopen(Filename, "wb");
    if (FidRaw == NULL) {
        AmbaPrint("CalibDetect NG: open to write raw img fail");
        return -1;
    }
    AmbaFS_fwrite(RawBuffer, sizeof(UINT16), Raw_SIZE, FidRaw);
    AmbaFS_fclose(FidRaw);

    return 0;
}

/**
 *  @brief detect the mark in the raw image
 *
 *  detect the mark in the raw image
 *
 *
 *  @return 0 success, <0 failure
 */
int AppLibCalibAVM_DetectMark()
{
    UINT16 *RawBuffer;
    char DFn[32];
    AVM_Corner ResultCorner[4];

    //From SDcard
    AMBA_MEM_CTRL_s MemRawBuffer;
    sprintf(DFn, "c:\\0001.raw");
    AppLibCalibAVM_ReadRawImg(&MemRawBuffer, DFn);
    RawBuffer = (UINT16 *)MemRawBuffer.pMemAlignedBase;

    AmpCalib_AVMMarkDetection(RawBuffer, Raw_W, Raw_H, ResultCorner);
    for (int i=0 ; i<4; ++i) {
        AmbaPrint("TopLeft x: %d y: %d TopRight x: %d y: %d \nBottomLeft x: %d y: %d BottomRight x: %d y: %d \n",  ResultCorner[i].Corner[0].x, ResultCorner[i].Corner[0].y,
            ResultCorner[i].Corner[1].x, ResultCorner[i].Corner[1].y,  ResultCorner[i].Corner[2].x, ResultCorner[i].Corner[2].y,  ResultCorner[i].Corner[3].x, ResultCorner[i].Corner[3].y);
    }

    //Write Raw Image
    sprintf(DFn, "c:\\out_all.raw");
    AppLibCalibAVM_WriteRawImg(RawBuffer, DFn);

    //Free Raw Image Buffer
    AmbaKAL_BytePoolFree(MemRawBuffer.pMemBase);

    return 0;
/*
#define FromSD 0
#if FromSD
    //From SDcard
    AMBA_MEM_CTRL_s MemRawBuffer;
    //sprintf(Fn, "c:\\raw\\detect_%d.raw", i);
    sprintf(Fn, "c:\\raw\\detect_all.raw");
    AppLibCalibAVM_ReadRawImg(&MemRawBuffer, Fn);
    RawBuffer = (UINT16 *)MemRawBuffer.pMemAlignedBase;
#else
    //From Memory
    if (AppLibStillEnc_GetRawBufferAddr(&RawAddr) >= 0) {
    	RawBuffer = (UINT16 *)RawAddr;
    } else
    	return -1;
#endif
    //Detection Mark
    AmbaPrint("Start detection function");
    for (int i=0; i<4; ++i) {
        AmpCalib_AVMMarkDetection(RawBuffer, Raw_W, Raw_H, GuessCorner[i],&ResultCorner[i]);
        AmbaPrint("TopLeft x: %d y: %d TopRight x: %d y: %d \nBottomLeft x: %d y: %d BottomRight x: %d y: %d \n",  ResultCorner[i].Corner[0].x, ResultCorner[i].Corner[0].y,
                  ResultCorner[i].Corner[1].x, ResultCorner[i].Corner[1].y,  ResultCorner[i].Corner[2].x, ResultCorner[i].Corner[2].y,  ResultCorner[i].Corner[3].x, ResultCorner[i].Corner[3].y);
    }
    //Write Raw Image
    sprintf(Fn, "c:\\out_all.raw");
    AppLibCalibAVM_WriteRawImg(RawBuffer, Fn);
#if FromSD
    //Free Raw Image Buffer
    AmbaKAL_BytePoolFree(MemRawBuffer.pMemBase);
#endif
    return 0;
*/

}

/**
 *  @brief test function for write chart map
 *
 *  test function for write chart map
 *
 *  @param [in] Buffer buffer address
 *  @param [in] Filename file name for the raw image
 *
 */
void AmpCalib_AVMWriteCharMapTest(UINT8 *Buffer, const char *Filename)
{
    AMBA_FS_FILE *Fid = AmbaFS_fopen(Filename, "w");

    AMBA_MEM_CTRL_s MemTmp;
    if (AmpUtil_GetAlignedPool(&G_MMPL, &MemTmp.pMemAlignedBase, &MemTmp.pMemBase, sizeof(UINT16) * 288*1080, 32) ) {
        AmbaPrint("Error callocing the outputcharmap.");
    }
    UINT16 *Tmp = (UINT16 *)MemTmp.pMemAlignedBase;
    for (int i=0; i<288*1080; ++i) {
        Tmp[i] = (UINT16)Buffer[i]*255;
    }

    AmbaFS_fwrite(Tmp, sizeof(UINT16), 288*1080, Fid);
    AmbaFS_fclose(Fid);
    AmbaKAL_BytePoolFree(MemTmp.pMemBase);
}

AMP_AREA_s VoutBirdView     = {0, 0, 600, 869};             //BirdView place and size in VoutWindow
AMP_AREA_s VoutCar              = {150, 150, 300, 569};     //Car place and size in VoutWindow
AMP_AREA_s AVMBirdView      = {0, 0, 1500, 2050};         //BirdView place and size in real world
AMP_AREA_s AVMCar               = {460, 475, 580, 1100};   //Car place and size in real world
extern AMP_MULTI_CHAN_VOUT_WINDOW_CFG_s MultiChanVoutWindowTV[MAX_VOUT_WINDOW_VIEW];


/**
 *  @brief generate multiple Vout window
 *
 *  generate multiple Vout window
 *
 *  @return 0 success, <0 failure
 *
 */
int AppLibCalibAVM_GenMultiVoutWin () {

    int VoutCarHeight = ceil ( (double) AVMCar.Height / AVMCar.Width * VoutCar.Width );
    AmbaPrint("VoutCarHeight: %d\n", VoutCarHeight);
    if (VoutCarHeight != VoutCar.Height) {
        AmbaPrint("VoutCarHeight/VoutCarWidth ratio is not same as AVMCarHeight/AVMCarWidth, should be %d\n", VoutCarHeight);
        return -1;
    }

    int NewF, NewL, NewR, NewB;
    NewF = TRUNCATE_16(VoutCar.Y - VoutBirdView.Y);
    NewB = TRUNCATE_16(VoutBirdView.Y + VoutBirdView.Height - VoutCar.Y - VoutCar.Height);
    NewL = TRUNCATE_16(VoutCar.X - VoutBirdView.X);
    NewR = TRUNCATE_16(VoutBirdView.X + VoutBirdView.Width - VoutCar.X - VoutCar.Width);
    double NewFRatio= (double)NewF/(NewF+NewB);

    AMP_AREA_s NewVoutBirdView = VoutBirdView;
    NewVoutBirdView.Width =  NewL + NewR + VoutCar.Width;
    NewVoutBirdView.Height = TRUNCATE_16(VoutBirdView.Height);

    NewF = (int)((NewVoutBirdView.Height - VoutCar.Height) * NewFRatio);
    NewB = NewVoutBirdView.Height - VoutCar.Height - NewF;

        AmbaPrint("test:%d %d\n", NewL, NewR);

    AMP_AREA_s NewVoutCar = VoutCar;
    NewVoutCar.X = NewVoutBirdView.X + NewL;
    NewVoutCar.Y = NewVoutBirdView.Y + NewF;

    AMP_AREA_s FrontVout, BackVout, LeftVout, RightVout;
    FrontVout.X = NewVoutBirdView.X;
    FrontVout.Y = NewVoutBirdView.Y;
    FrontVout.Width = NewL + NewVoutCar.Width + NewR;
    FrontVout.Height = NewF;

    BackVout.X = NewVoutBirdView.X;
    BackVout.Y = NewVoutBirdView.Y + NewF + NewVoutCar.Height;
    BackVout.Width = NewL + NewVoutCar.Width + NewR;
    BackVout.Height = NewB;

    LeftVout.X = FrontVout.X;
    LeftVout.Y = FrontVout.Y;
    LeftVout.Width = NewL;
    LeftVout.Height = NewF + NewVoutCar.Height + NewB;

    RightVout.X = NewVoutBirdView.X + NewL + NewVoutCar.Width;
    RightVout.Y = FrontVout.Y;
    RightVout.Width = NewR;
    RightVout.Height = NewF + NewVoutCar.Height + NewB;

        AmbaPrint("Front X: %d Y: %d Width: %d Height: %d Back X: %d Y: %d Width: %d Height: %d Left X: %d Y: %d Width: %d Height: %d Right X: %d Y: %d Width: %d Height: %d\n",
            FrontVout.X, FrontVout.Y, FrontVout.Width, FrontVout.Height,BackVout.X, BackVout.Y, BackVout.Width,BackVout.Height,
            LeftVout.X, LeftVout.Y, LeftVout.Width, LeftVout.Height, RightVout.X, RightVout.Y, RightVout.Width, RightVout.Height);

   memcpy( &MultiChanVoutWindowTV[0].DisplayWin,  &FrontVout, sizeof(AMP_AREA_s));
   memcpy( &MultiChanVoutWindowTV[1].DisplayWin,  &BackVout, sizeof(AMP_AREA_s));
   memcpy( &MultiChanVoutWindowTV[2].DisplayWin,  &LeftVout, sizeof(AMP_AREA_s));
   memcpy( &MultiChanVoutWindowTV[3].DisplayWin,  &RightVout, sizeof(AMP_AREA_s));

   MultiChanVoutWindowTV[0].BlendTable.Width  = FrontVout.Width;
   MultiChanVoutWindowTV[0].BlendTable.Height = FrontVout.Height;
   MultiChanVoutWindowTV[0].BlendTable.Pitch   = ALIGN_16(FrontVout.Width);
   MultiChanVoutWindowTV[1].BlendTable.Width  = BackVout.Width;
   MultiChanVoutWindowTV[1].BlendTable.Height = BackVout.Height;
   MultiChanVoutWindowTV[1].BlendTable.Pitch   = ALIGN_16(BackVout.Width);
   MultiChanVoutWindowTV[2].BlendTable.Width  = LeftVout.Width;
   MultiChanVoutWindowTV[2].BlendTable.Height = LeftVout.Height;
   MultiChanVoutWindowTV[2].BlendTable.Pitch   = ALIGN_16(LeftVout.Width);
   MultiChanVoutWindowTV[3].BlendTable.Width  = RightVout.Width;
   MultiChanVoutWindowTV[3].BlendTable.Height = RightVout.Height;
   MultiChanVoutWindowTV[3].BlendTable.Pitch   = ALIGN_16(RightVout.Width);
    return 0;

}

/**
 *  @brief allocate the detection buffer
 *
 *  allocate the detection buffer
 *
 *
 *  @return 0 success, <0 failure
 */
int AppLibCalibAVM_CalDetectBuffer() {
    AmpUtil_GetAlignedPool(&G_MMPL, &DetectBuffer.Memkernel.pMemAlignedBase, &DetectBuffer.Memkernel.pMemBase, sizeof(float) * 1 + 2 * (int)ceil(2.5 * DetectParameter.Sigma), 32);
    AmpUtil_GetAlignedPool(&G_MMPL, &DetectBuffer.Memtempim.pMemAlignedBase, &DetectBuffer.Memtempim.pMemBase, sizeof(float) * CLIP_SIZE, 32);
    AmpUtil_GetAlignedPool(&G_MMPL, &DetectBuffer.Memsmoothedim.pMemAlignedBase, &DetectBuffer.Memsmoothedim.pMemBase, sizeof(UINT16) * CLIP_SIZE, 32);
    AmpUtil_GetAlignedPool(&G_MMPL, &DetectBuffer.MemMagnitude.pMemAlignedBase, &DetectBuffer.MemMagnitude.pMemBase, sizeof(INT16) * CLIP_SIZE, 32);
    AmpUtil_GetAlignedPool(&G_MMPL, &DetectBuffer.MemDeltaX.pMemAlignedBase, &DetectBuffer.MemDeltaX.pMemBase, sizeof(INT16) * CLIP_SIZE, 32);
    AmpUtil_GetAlignedPool(&G_MMPL, &DetectBuffer.MemDeltaY.pMemAlignedBase, &DetectBuffer.MemDeltaY.pMemBase, sizeof(INT16) * CLIP_SIZE, 32);
    AmpUtil_GetAlignedPool(&G_MMPL, &DetectBuffer.MemHistBuffer.pMemAlignedBase, &DetectBuffer.MemHistBuffer.pMemBase, sizeof(int) * HIST_BOUND, 32);
    AmpUtil_GetAlignedPool(&G_MMPL, &DetectBuffer.MemNms.pMemAlignedBase, &DetectBuffer.MemNms.pMemBase, sizeof(UINT8) * CLIP_SIZE, 32);
    AmpUtil_GetAlignedPool(&G_MMPL, &DetectBuffer.MemEdge.pMemAlignedBase, &DetectBuffer.MemEdge.pMemBase, sizeof(UINT8) * CLIP_SIZE, 32);
    AmpUtil_GetAlignedPool(&G_MMPL, &DetectBuffer.MemTmp.pMemAlignedBase, &DetectBuffer.MemTmp.pMemBase, sizeof(UINT16) * CLIP_SIZE, 32);
    AmpUtil_GetAlignedPool(&G_MMPL, &DetectBuffer.MemContourMap.pMemAlignedBase, &DetectBuffer.MemContourMap.pMemBase, sizeof(INT16) * CLIP_SIZE, 32);
    AmpUtil_GetAlignedPool(&G_MMPL, &DetectBuffer.MemClipImage.pMemAlignedBase, &DetectBuffer.MemClipImage.pMemBase, sizeof(UINT8) * CLIP_SIZE, 32);
    return 0;
}

/**
 *  @brief free the detection buffer
 *
 *  free the detection buffer
 *
 *
 *  @return 0 success, <0 failure
 */
int AppLibCalibAVM_CalDetectBufferFree() {
    AmbaKAL_BytePoolFree(DetectBuffer.Memkernel.pMemBase);
    AmbaKAL_BytePoolFree(DetectBuffer.Memtempim.pMemBase);
    AmbaKAL_BytePoolFree(DetectBuffer.Memsmoothedim.pMemBase);
    AmbaKAL_BytePoolFree(DetectBuffer.MemMagnitude.pMemBase);
    AmbaKAL_BytePoolFree(DetectBuffer.MemDeltaX.pMemBase);
    AmbaKAL_BytePoolFree(DetectBuffer.MemDeltaY.pMemBase);
    AmbaKAL_BytePoolFree(DetectBuffer.MemHistBuffer.pMemBase);
    AmbaKAL_BytePoolFree(DetectBuffer.MemNms.pMemBase);
    AmbaKAL_BytePoolFree(DetectBuffer.MemEdge.pMemBase);
    AmbaKAL_BytePoolFree(DetectBuffer.MemTmp.pMemBase);
    AmbaKAL_BytePoolFree(DetectBuffer.MemContourMap.pMemBase);
    AmbaKAL_BytePoolFree(DetectBuffer.MemClipImage.pMemBase);
    return 0;
}

/**
 *  @brief initial the detection parameter
 *
 *  initial the detection parameter
 *
 *
 *  @return 0 success, <0 failure
 */
int AppLibCalibAVM_CalDetectParameter() {
    //canny
    DetectParameter.Sigma = 2.0;//3
    DetectParameter.ThresholdLow = 0.7; //0
    DetectParameter.ThresholdHigh = 0.7;//0.95;//0.995;
    //0.5 0.5 best for top two corner

    DetectParameter.CornerNumber = 4;
    //Front 600 10
    DetectParameter.ThresholdArea = 600; //500
    DetectParameter.ThresholdMinusColor = 10; //10


}

