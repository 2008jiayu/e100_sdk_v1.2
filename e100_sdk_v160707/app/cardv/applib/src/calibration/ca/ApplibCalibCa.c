/**
 * @file src/app/connected/applib/src/calibration/ca/ApplibCalibCA.c
 *
 * sample code for chromatic abbreation calibration
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
#include <calibration/ca/ApplibCalibCa.h>




#define MAX_CMD_TOKEN  128
#define RESERVED1   0
#define RESERVED2   0
#define ABS(a)      (((a) < 0) ? -(a) : (a))
#define MAX_SC_TOKEN 64
#define CANumFracBits 4
#define IDEA_CA   0
#define IDEA_CA_RAWENC   (IDEA_CA+1)

#define MAX_CMD_TOKEN_THIS 2048

CA_Control_s AppCAControl;
CA_Storage_s CATable[CALIB_CH_NO];
AMBA_DSP_IMG_CALIB_CAWARP_INFO_s CAInfo[CALIB_CH_NO];

/**
 *  @brief  turn on or turn off the debug flag for CA
 *
 *   turn on or turn off the debug flag for CA
 *
 *  @param [in]Enable enable flag to print debug message
 *
 *  @return 0 success, -1 failure
 */
void AppLibCalibCA_DebugEnable(UINT8 Enable)
{
    AppCAControl.Debug = Enable;
}

/**
 *  @brief  print the error message for CA calibration
 *
 *   print the error message for CA calibration
 *
 *  @param [in]CalSite calibration site status
 *  @param [out]OutputStr debug message for this function
 *
 *  @return 0 success, -1 failure
 */
INT32   AppLibCalibCA_PrintError(char *OutputStr, Cal_Stie_Status_s *CalSite)
{
    AmbaPrint("************************************************************");
    AmbaPrint("%s",OutputStr);
    AmbaPrint("************************************************************");
    CalSite->Status = CAL_SITE_RESET;
    return 0;
}


/**
 *  @brief get the CA enable flag
 *
 *   get the CA enable flag
 *
 *
 *  @return enable flag
 */
UINT8  AppLibCalibCA_GetCalDataEnable(void)
{
    return AppCAControl.Enable;
}

/**
 *  @brief set the CA enable flag
 *
 *  set the CA enable flag
 *
 *  @param [in]En enable flag
 *
 *  @return 0 success, -1 failure
 */
UINT8  AppLibCalibCA_SetCalDataEnable(UINT8 En)
{
    AppCAControl.Enable = En;
    return 0;
}

/**
 *  @brief get the CA data address
 *
 *  get the CA data address
 *
 *  @param [out]CACalData return the pointer of CA data
 *
 */
void AppLibCalibCA_GetCACalData(CA_Control_s *CACalData)
{
    *CACalData = AppCAControl;
}

/**
 *  @brief set the CA table
 *
 *  set the CA table
 *
 *  @param [in]Enable enable flag
 *
 *  @return 0 success, -1 failure
 */int AppLibCalibCA_SetCA(UINT32 Enable)
{
    int Rval = 0;
    AMBA_DSP_IMG_MODE_CFG_s ImgMode;
    UINT32 Config;

    memset(&ImgMode, 0, sizeof(AMBA_DSP_IMG_MODE_CFG_s));
    AppLib_CalibGetDspMode(&ImgMode);

    if (Enable == 1){
        Config = 0;//enable CA
    } else {
        Config = AMBA_DSP_IMG_WARP_CONFIG_FORCE_DISABLE;//disable CA
    }

    /*set the CA table for video mode*/
    ImgMode.Pipe = AMBA_DSP_IMG_PIPE_VIDEO;
    AmbaDSP_WarpCore_CalcDspCawarp(&ImgMode, Config);
    AmbaDSP_WarpCore_SetDspCawarp(&ImgMode);

    /*set the CA table for still mode*/
    ImgMode.Pipe = AMBA_DSP_IMG_PIPE_STILL;
    ImgMode.BatchId = AMBA_DSP_NO_BATCH_FILTER;
    AmbaDSP_WarpCore_CalcDspCawarp(&ImgMode, Config);
    AmbaDSP_WarpCore_SetDspCawarp(&ImgMode);
    return Rval;
}

/**
 *  @brief control the CA table
 *
 *  control the CA table
 *
 *  @param [in]Enable enable flag to control the CA on/off
 *  @param [in]Channe channel no
 *  @param [in]Id1 table ID 1
 *  @param [in]Id2 table ID 2
 *  @param [in]BlendRatio blend ratio for table 1 and table 2
 *  @param [in]Strength Strength for the CA table
 *
 *  @return 0 success, -1 failure
 */
INT32 AppLibCalibCA_SelectCATable(UINT8 Enable, UINT8 Channel, UINT8 Id1, UINT8 Id2, int BlendRatio, INT32 Strength)
{

    INT32 Rval = 0;
    INT32 i;
    INT32 Ratio1;
    AMBA_DSP_IMG_MODE_CFG_s ImgMode;
    UINT32 Config;
    UINT8 CalibMappingId1,CalibMappingId2;

    memset(&ImgMode, 0, sizeof(AMBA_DSP_IMG_MODE_CFG_s));
    AppLib_CalibGetDspMode(&ImgMode);
    if (AppCAControl.Debug) {
    AmbaPrint("AppLibCalibCA_SelectCATable: Enable: %d Channel: %d Id1: %d Id:2 %d BlendRatio: %d Strength: %d",
              Enable, Channel, Id1, Id2, BlendRatio, Strength);
    }
    CalibMappingId1 = AppLib_CalibTableMapping(Channel, Id1);
    CalibMappingId2 = AppLib_CalibTableMapping(Channel, Id2);
    if (Enable) {
        //blending
        if ((CalibMappingId1 != CalibMappingId2)) {
            if (AppCAControl.CATable[CalibMappingId1]->CAHeader.HorGridNum != AppCAControl.CATable[CalibMappingId2]->CAHeader.HorGridNum) {
                if (AppCAControl.Debug) {
                AmbaPrint("Horizonal Grid number mismatch");
                }
                return -1;
            }
            if (AppCAControl.CATable[CalibMappingId1]->CAHeader.VerGridNum != AppCAControl.CATable[CalibMappingId2]->CAHeader.VerGridNum) {
                if (AppCAControl.Debug) {
                AmbaPrint("Vertical Grid number mismatch");
                }
                return -1;
            }
            if ((BlendRatio < 0) || (BlendRatio > 65536)) {
                if (AppCAControl.Debug) {
                AmbaPrint("BlendRatio should be between 0~65536, BlendRatio = %d",BlendRatio);
                }
                return -1;
            }
            Ratio1 = 65536 - BlendRatio;
            CATable[Channel].CAHeader = AppCAControl.CATable[CalibMappingId1]->CAHeader;
            for (i = 0; i < AppCAControl.CATable[CalibMappingId1]->CAHeader.HorGridNum*AppCAControl.CATable[CalibMappingId1]->CAHeader.VerGridNum; i++) {
                CATable[Channel].CAVector[i].X = (AppCAControl.CATable[CalibMappingId1]->CAVector[i].X*BlendRatio + \
                                                  AppCAControl.CATable[CalibMappingId2]->CAVector[i].X*Ratio1)>>16;
                CATable[Channel].CAVector[i].Y = (AppCAControl.CATable[CalibMappingId1]->CAVector[i].Y*BlendRatio + \
                                                  AppCAControl.CATable[CalibMappingId2]->CAVector[i].Y*Ratio1)>>16;
            }
        } else if (CalibMappingId1 == CalibMappingId2) {
            CATable[Channel] = *(AppCAControl.CATable[CalibMappingId1]);
        } else if (BlendRatio == 0) {
            CATable[Channel] = *(AppCAControl.CATable[CalibMappingId1]);
        } else if (BlendRatio == 65536) {
            CATable[Channel] = *(AppCAControl.CATable[CalibMappingId2]);
        }

        //decay
        if ((Strength < 0) || (Strength > 65536)) {
            if (AppCAControl.Debug) {
            AmbaPrint("Strength should be between 0~65536, Strength = %d",Strength);
            }
            return -1;
        }
        if (Strength != 65536) {
            for (i = 0; i < AppCAControl.CATable[CalibMappingId1]->CAHeader.HorGridNum*AppCAControl.CATable[CalibMappingId1]->CAHeader.VerGridNum; i++) {
                CATable[Channel].CAVector[i].X = (CATable[Channel].CAVector[i].X*Strength)>>16;
                CATable[Channel].CAVector[i].Y = (CATable[Channel].CAVector[i].Y*Strength)>>16;
            }
        }
        Config                                    = 0;
        CAInfo[Channel].Version                            = CATable[Channel].CAHeader.Version;
        CAInfo[Channel].HorGridNum                         = CATable[Channel].CAHeader.HorGridNum;
        CAInfo[Channel].VerGridNum                         = CATable[Channel].CAHeader.VerGridNum;
        CAInfo[Channel].TileWidthExp                       = CATable[Channel].CAHeader.TileWidthExp;
        CAInfo[Channel].TileHeightExp                      = CATable[Channel].CAHeader.TileHeightExp;
        CAInfo[Channel].VinSensorGeo.Width                 = CATable[Channel].CAHeader.VinSensorGeo.Width;
        CAInfo[Channel].VinSensorGeo.Height                = CATable[Channel].CAHeader.VinSensorGeo.Height;
        CAInfo[Channel].VinSensorGeo.StartX                = CATable[Channel].CAHeader.VinSensorGeo.StartX;
        CAInfo[Channel].VinSensorGeo.StartY                = CATable[Channel].CAHeader.VinSensorGeo.StartY;
        CAInfo[Channel].VinSensorGeo.HSubSample.FactorDen  = CATable[Channel].CAHeader.VinSensorGeo.HSubSample.FactorDen;
        CAInfo[Channel].VinSensorGeo.HSubSample.FactorNum  = CATable[Channel].CAHeader.VinSensorGeo.HSubSample.FactorNum;
        CAInfo[Channel].VinSensorGeo.VSubSample.FactorDen  = CATable[Channel].CAHeader.VinSensorGeo.VSubSample.FactorDen;
        CAInfo[Channel].VinSensorGeo.VSubSample.FactorNum  = CATable[Channel].CAHeader.VinSensorGeo.VSubSample.FactorNum;
        CAInfo[Channel].RedScaleFactor                     = CATable[Channel].CAHeader.RedScaleFactor;
        CAInfo[Channel].BlueScaleFactor                    = CATable[Channel].CAHeader.BlueScaleFactor;
        CAInfo[Channel].Reserved                           = CATable[Channel].CAHeader.Reserved;
        CAInfo[Channel].Reserved1                          = CATable[Channel].CAHeader.Reserved1;
        CAInfo[Channel].pCaWarp                            = (AMBA_DSP_IMG_GRID_POINT_s*)CATable[Channel].CAVector;

    } else {
        Config                                    = AMBA_DSP_IMG_WARP_CONFIG_FORCE_DISABLE;
    }
	if (AppCAControl.Debug) {
		AmbaPrint("version: 0x%X",CAInfo[Channel].Version);
		AmbaPrint("horizontal grid number: %d",CAInfo[Channel].HorGridNum);
		AmbaPrint("vertical grid number: %d",CAInfo[Channel].VerGridNum);
		AmbaPrint("tile width: %d",CAInfo[Channel].TileWidthExp);
		AmbaPrint("tile height: %d",CAInfo[Channel].TileHeightExp);
		AmbaPrint("Image width: %d",CAInfo[Channel].VinSensorGeo.Width);
		AmbaPrint("Image height: %d",CAInfo[Channel].VinSensorGeo.Height);
		AmbaPrint("StartX: %d",CAInfo[Channel].VinSensorGeo.StartX);
		AmbaPrint("StartY: %d",CAInfo[Channel].VinSensorGeo.StartY);
		AmbaPrint("HSubSample FactorDen: %d",CAInfo[Channel].VinSensorGeo.HSubSample.FactorDen);
		AmbaPrint("HSubSample FactorNum: %d",CAInfo[Channel].VinSensorGeo.HSubSample.FactorNum);
		AmbaPrint("VSubSample FactorDen: %d",CAInfo[Channel].VinSensorGeo.VSubSample.FactorDen);
		AmbaPrint("VSubSample FactorNum: %d",CAInfo[Channel].VinSensorGeo.VSubSample.FactorNum);
		AmbaPrint("RedScaleFactor: %d",CAInfo[Channel].RedScaleFactor);
		AmbaPrint("BlueScaleFactor: %d",CAInfo[Channel].BlueScaleFactor);
		AmbaPrint("CA table address: 0x%X",CAInfo[Channel].pCaWarp);
	}
    ImgMode.Pipe = AMBA_DSP_IMG_PIPE_VIDEO;
    AmbaDSP_WarpCore_SetCalibCawarpInfo(&ImgMode,&CAInfo[Channel]);
    AmbaDSP_WarpCore_CalcDspCawarp(&ImgMode, Config);
    AmbaDSP_WarpCore_SetDspCawarp(&ImgMode);

    ImgMode.Pipe = AMBA_DSP_IMG_PIPE_STILL;
    AmbaDSP_WarpCore_SetCalibCawarpInfo(&ImgMode,&CAInfo[Channel]);
    AmbaDSP_WarpCore_CalcDspCawarp(&ImgMode, Config);
    AmbaDSP_WarpCore_SetDspCawarp(&ImgMode);    
    return Rval;
}

/**
 *  @brief initial function for CA calibration
 *
 *  initial function for CA calibration
 *
 *  @param [in]CalObj calibration object
 *
 *  @return 0 success, -1 failure
 */
INT32 AppLibCalibCA_Init(Cal_Obj_s *CalObj)
{
    INT32 i;
    UINT8 *CA_Addr = CalObj->DramShadow;
    static UINT8 CAInitFlag = 0;

    if (CAInitFlag == 0)   {
        memset(&AppCAControl,0,sizeof(CA_Control_s));
        CAInitFlag = 1;
    }
    AppCAControl.Enable = (INT32)CA_Addr[CAL_CA_ENABLE];
    CAL_PRINT("AppCAControl.Enable %d",AppCAControl.Enable);
    if (AppCAControl.Enable == 1) {
        AppCAControl.CATableCount = CA_Addr[CAL_CA_TABLE_COUNT];
        for (i=0; i < MAX_CA_TABLE_COUNT; i++) {
            AppCAControl.CATable[i] = (CA_Storage_s*)&CA_Addr[CAL_CA_TABLE(i)];
        }
        if (AppCAControl.CATable[0]->CAHeader.Enable == 1) {
            AppLibCalibCA_SelectCATable(1, 0, 0, 0, 0x10000, 65536);
        }
    }
    return 0;
}


/**
 *  @brief generate CA calibration table from lens specification
 *
 *  generate CA calibration table from lens specification
 *
 *  @param [in]argc the number of the input parameter
 *  @param [in]argv value of input parameter
 *
 *  @return 0 success, -1 failure
 */
INT32 AppLibCalibCA_GenCAFromSpec(INT32 argc, char *argv[])
{
    char Str[256];
    //char buftext[2048];
    //char *argv_ca[MAX_CMD_TOKEN_THIS];
    INT32 IdxNum = 11;
    INT32 IdxNumE = 0;
    INT32 IdxNumR = 0;
    INT32 IdxNumB = 0;
    UINT8 CAMaxIdxNum = 64;
    char ChangeLine='\n';
    INT32  ArgcR;
    INT32 i = 0;
    INT32 j = 0;
    char *ArgvR[MAX_CMD_TOKEN];
    char Buffer[256];
    double *RedLine,*BlueLine;
    INT16 *CalibCATableBySpec;
    INT32 Method = 0;
    INT32 CompensateRatio = 0;
    INT32 RRatio = 0;
    INT32 ZoomStep = 0;
    INT32 CAZoomStep = 0,CAChannelId = 0;
    INT32 MethodFlag = 0;
    INT32 CompensateRatioFlag = 0;
    INT32 RRatioFlag = 0;
    INT32 RedLineFlag = 0;
    INT32 BlueLineFlag = 0;
    INT32 SensorFlag = 0;
    INT32 ExpectFlag = 0;
    INT32 SensorCalFlag = 0;
    INT32 SensorRawFlag = 0;
    INT32 OffSensorFlag = 0;
    INT32 RawEncodeTextFlag = 0;
    AMBA_FS_FILE *fp_r = NULL, *fp = NULL,*Fid =NULL;
    INT32 X, Y, f;
    char TestCAFn[30] = {'c',':','\\','c','a','l','i','b','1','3','.','b','i','n','\0'};
    char CSHfileFn[30] = {'c',':','\\','c','a','_','t','a','b','l','e','.','h','\0'};
    char CARawEncFn[30] = {'c',':','\\','C','A','_','R','a','w','E','n','c','o','d','e','.','b','i','n','\0'};
    UINT32 Tmp;
    Cal_CA_Entry_s *Wp;
    AMBA_DSP_IMG_GRID_POINT_s *CATable;
    extern double pow(double X, double Y);  /* For calculating gain_magnitude */
    double x_pr = 0, y_pr =0,  r = 0, cos = 0, sin = 0, a = 0, b = 0,red_offset = 0,blue_offset = 0;
    double UnitCellSize = 0.001348; // unit in mm
    double CrossProductSum = 0, SquareSumR = 0, SquareSumB = 0, RedHorShift, RedVerShift, BlueHorShift, BlueVerShift;
    double SquareSumAvg = 0, CrossProductSumAvgR = 0, CrossProductSumAvgB = 0;
    INT16 RedScaleFactor = 0, BlueScaleFactor = 0;
    double ExceedScaleFactor = -256.0;
    INT32 StartX = 0;       //INT32
    INT32 StartY = 0;       //INT32
    INT32 ImgWidth = 0; //INT32
    INT32 ImgHeight = 0;    //INT32
    INT32 HorGridNum = 0;
    INT32 VerGridNum = 0;
    INT16 TileWidthExp  =   7;
    INT16 TileHeightExp  =  7;
    INT16 TileWidth  =  128;//TileWidthExp;   /* 4:16, 5:32, 6:64, 7:128, 8:256, 9:512 */
    INT16 TileHeight  = 128;//TileWidthExp;   /* 4:16, 5:32, 6:64, 7:128, 8:256, 9:512 */
    INT32 CenterX = 0;
    INT32 CenterY = 0;
    double CARealWidth = 0;
    INT32 CalibCAHeaderSpec[20] = {0};
    double *Expect;
    INT32 Rval = 0;
    void *TempExpect,*TempRedLine,*TempBlueLine,*TempCATableBySpec,*TempWp,*TempCATable;
    void *TempExpectRel,*TempRedLineRel,*TempBlueLineRel,*TempCATableBySpecRel,*TempWpRel,*TempCATableRel;
    UINT16 HSubSampleFactorNum=1, HSubSampleFactorDen=1, VSubSampleFactorNum=1, VSubSampleFactorDen=1;
    UINT8 *BufferAddress;
    AMBA_SENSOR_MODE_INFO_s VinInfo;
    AMBA_SENSOR_MODE_ID_u SensorMode = {
        .Bits = {
            .Mode = 0,
            .VerticalFlip = 0,
        }
    };



    SensorMode.Bits.Mode = pAmbaCalibInfoObj[0/*AMBA_VIN_CHANNEL0*/]->AmbaVignetteMode;
    AmbaSensor_GetModeInfo(AppEncChannel, SensorMode, &VinInfo);
    if (AmpCalib_GetOpticalBlackFlag() == 1) {
        StartX = VinInfo.InputInfo.PhotodiodeArray.StartX - (VinInfo.OutputInfo.RecordingPixels.StartX * VinInfo.InputInfo.HSubsample.FactorDen \
                                                                                /VinInfo.InputInfo.HSubsample.FactorNum);
        StartY = VinInfo.InputInfo.PhotodiodeArray.StartY -  (VinInfo.OutputInfo.RecordingPixels.StartY * VinInfo.InputInfo.VSubsample.FactorDen \
                                                                                /VinInfo.InputInfo.VSubsample.FactorNum);
    } else {
    StartX = VinInfo.InputInfo.PhotodiodeArray.StartX;
    StartY = VinInfo.InputInfo.PhotodiodeArray.StartY;
    }


    AmbaPrint("START_X = %d, START_y= %d",StartX,StartY);

    Rval = AmpUtil_GetAlignedPool(APPLIB_G_MMPL, &TempExpect, &TempExpectRel, (CAMaxIdxNum*sizeof(double)), 32);
    BufferAddress = (UINT8*)((UINT32)TempExpect) ;
    if (Rval != OK) {
        AmbaPrint("allocate fail in %s:%u", __FUNCTION__, __LINE__);
        return CA_CALIB_CREATE_BUFFER_ERROR;
    } else {
        Expect = (double *)BufferAddress;
        AmbaPrint("[AppLibCalibCA]Expect buffer (0x%08X) (%u)!", Expect, (CAMaxIdxNum*sizeof(double)));
    }
    Rval = AmpUtil_GetAlignedPool(APPLIB_G_MMPL, &TempRedLine, &TempRedLineRel, (CAMaxIdxNum*sizeof(double)), 32);
    BufferAddress = (UINT8*)((UINT32)TempRedLine) ;
    if (Rval != OK) {
        AmbaPrint("allocate fail in %s:%u", __FUNCTION__, __LINE__);
        return CA_CALIB_CREATE_BUFFER_ERROR;
    } else {
        RedLine = (double *)BufferAddress;
        AmbaPrint("[AppLibCalibCA]RedLine buffer (0x%08X) (%u)!", RedLine, (CAMaxIdxNum*sizeof(double)));
    }
    Rval = AmpUtil_GetAlignedPool(APPLIB_G_MMPL, &TempBlueLine, &TempBlueLineRel, (CAMaxIdxNum*sizeof(double)), 32);
    BufferAddress = (UINT8*)((UINT32)TempBlueLine) ;
    if (Rval != OK) {
        AmbaPrint("allocate fail in %s:%u", __FUNCTION__, __LINE__);
        return CA_CALIB_CREATE_BUFFER_ERROR;
    } else {
        BlueLine = (double *)BufferAddress;
        AmbaPrint("[AppLibCalibCA]BlueLine buffer (0x%08X) (%u)!", BlueLine, (CAMaxIdxNum*sizeof(double)));
    }


    if (argc == 7 || argc == 5) {
    } else {
        AmbaPrint(
            "t cal gen_ca: Generate CA calibration table\n"
            "t cal gen_ca [raw encode text flag] [filename spec] [raw_input filename ] [raw_output filename] \n");
        AmbaPrint("incorrect argument number");
        return CA_CALIB_INCORRECT_ARGUMENT_NUM;
    }
    RawEncodeTextFlag = atoi(argv[3]);
    if ((RawEncodeTextFlag == IDEA_CA) || (RawEncodeTextFlag == IDEA_CA_RAWENC)) {
    } else {
        AmbaPrint(
            "[raw encode text flag] = 0 or 1 \n"
            "t cal gen_ca [raw encode text flag] [filename spec] [raw_input filename ] [raw_output filename] \n");
        AmbaPrint("incorrect parameter");
        return CA_CALIB_INCORRECT_PARAMETER;
    }
    if (RawEncodeTextFlag) {
        if (argc == 7)   {
        } else {
            AmbaPrint(
                "t cal gen_ca: Generate CA calibration table\n"
                "t cal gen_ca [raw encode text flag] [filename spec] [raw_input filename ] [raw_output filename] \n");
            AmbaPrint("incorrect parameter 1");
            return CA_CALIB_INCORRECT_PARAMETER;
        }
    }
    // Read a line from the file
    fp_r = AmbaFS_fopen(argv[4], "r");
    if (fp_r == NULL) {
        AmbaPrint("AmbaFS_fopen %s fail.",argv[4]);
        Rval = CA_CALIB_OPEN_FILE_ERROR;
        return Rval;
    }

    while (1) {
        Rval = AppLib_MultiGetline(fp_r, Buffer);
        if (Rval == -1) {
            Rval = CA_CALIB_READSCRIPT_ERROR;
            break;
        } else {
            ArgcR = 0;
            memset(ArgvR, 0, MAX_SC_TOKEN*sizeof(char *));
            {
                // Parse the input string to multiple tokens
                char *Token = strtok(Buffer, " ");
                INT32 NumToken = 0;
                while (Token != NULL) {
                    ArgvR[NumToken] = Token;
                    NumToken++;
                    Token = strtok(NULL, " ");
                }
                ArgcR = NumToken;
            }
            if (strcmp(ArgvR[0], "method") == 0) {
                Method = atoi(ArgvR[1]);
                MethodFlag = 1;
            } else if (strcmp(ArgvR[0], "Compensate_ratio") == 0) {
                CompensateRatio = atoi(ArgvR[1]);
                if (CompensateRatio<0 || CompensateRatio>200) {
                    AmbaPrint("Compensate_ratio must be between 0~200.");
                    return CA_CALIB_COMPENSATE_RATIO_ERROR;
                } else {
                    CompensateRatioFlag = 1;
                }
            } else if (strcmp(ArgvR[0], "r_ratio") == 0) {
                RRatio = atoi(ArgvR[1]);
                if (RRatio<=0 || RRatio>200) {
                    AmbaPrint("r_ratio must be between 1~200.");
                    return CA_CALIB_RRATIO_ERROR;
                } else {
                    RRatioFlag = 1;
                }
            } else if (strcmp(ArgvR[0], "redline") == 0) {
                IdxNumR = ArgcR-1;
                for ( i=1; i<ArgcR; i++) {
                    RedLine[i-1] = (double) atof(ArgvR[i]);
                }
                RedLineFlag = 1;
            } else if (strcmp(ArgvR[0], "blueline") == 0) {
                IdxNumB = ArgcR-1;
                for ( i=1; i<ArgcR; i++) {
                    BlueLine[i-1] = (double) atof(ArgvR[i]);
                }
                BlueLineFlag = 1;
            } else if (strcmp(ArgvR[0], "off_sensor_calibration") == 0) {
                SensorCalFlag = 1;
                OffSensorFlag = 1;
                ImgWidth = (INT32) atoi(ArgvR[1]);
                ImgHeight = (INT32) atoi(ArgvR[2]);
                CARealWidth = (double) atof(ArgvR[3]);
                StartX = (INT32) atoi(ArgvR[4]);
                StartY = (INT32) atoi(ArgvR[5]);
                if (ArgcR == 8) {
                    CenterX = (INT32) atoi(ArgvR[6]);
                    CenterY = (INT32) atoi(ArgvR[7]);
                    if (CenterX < (ImgWidth*3>>3) || CenterX > (ImgWidth*5>>3)) {
                        SensorFlag = 0;
                        AmbaPrint("Error : CENTER_X out of the range ((IMG_W*3/8) < CENTER_X < (IMG_W*5/8))");
                        return CA_CALIB_CENTER_POSITION_ERROR;
                    }
                    if (CenterY < (ImgHeight*3>>3)  || CenterY > (ImgHeight*5>>3)) {
                        SensorFlag = 0;
                        AmbaPrint("Error : CENTER_Y out of the range ((IMG_H*3/8) < CENTER_Y < (IMG_H*5/8))");
                        return CA_CALIB_CENTER_POSITION_ERROR;
                    }
                } else {
                    CenterX = ImgWidth>>1;
                    CenterY = ImgHeight>>1;
                }
                UnitCellSize = CARealWidth / ImgWidth;
                HorGridNum  = (ImgWidth+TileWidth-1)/TileWidth + 1 ;
                VerGridNum  = (ImgHeight+TileHeight-1)/TileHeight + 1 ;
            } else if (strcmp(ArgvR[0], "off_sensor_raw_image") == 0) {
                SensorRawFlag = 1;
                OffSensorFlag = 1;
            } else if (strcmp(ArgvR[0], "sensor") == 0) {
                SensorFlag = 1;
                ImgWidth = (INT32) atoi(ArgvR[1]);
                ImgHeight = (INT32) atoi(ArgvR[2]);
                CARealWidth = (double) atof(ArgvR[3]);
                if (ArgcR == 6) {
                    CenterX = (INT32) atoi(ArgvR[4]);
                    CenterY = (INT32) atoi(ArgvR[5]);
                    if (CenterX < (ImgWidth*3>>3) || CenterX > (ImgWidth*5>>3)) {
                        SensorFlag = 0;
                        AmbaPrint("Error : CenterX out of the range ((ImgWidth*3/8) < CenterX < (ImgWidth*5/8))");
                        return CA_CALIB_CENTER_POSITION_ERROR;
                    }
                    if (CenterY < (ImgHeight*3>>3)  || CenterY > (ImgHeight*5>>3)) {
                        SensorFlag = 0;
                        AmbaPrint("Error : CenterY out of the range ((ImgHeight*3/8) < CenterY < (ImgHeight*5/8))");
                        return CA_CALIB_CENTER_POSITION_ERROR;
                    }
                } else {
                    CenterX = ImgWidth>>1;
                    CenterY = ImgHeight>>1;
                }
                UnitCellSize = CARealWidth / ImgWidth;
                HorGridNum  = (ImgWidth+TileWidth-1)/TileWidth + 1 ;
                VerGridNum  = (ImgHeight+TileHeight-1)/TileHeight + 1 ;
            } else if (strcmp(ArgvR[0], "expect") == 0) {
                IdxNumE = ArgcR-1;
                for ( i=1; i<ArgcR; i++) {
                    ExpectFlag = 1;
                    Expect[i-1] = (double) atof(ArgvR[i]);
                }
            } else if (strcmp(ArgvR[0], "zoom_step") == 0) {
                CAZoomStep = atoi(ArgvR[1]);
            } else  if (strcmp(ArgvR[0], "distance") == 0) {
            } else  if (strcmp(ArgvR[0], "aperture") == 0) {
            }
        }
    }
    Rval = AmbaFS_fclose(fp_r);
    if (Rval != 0) {
        AmbaPrint("AmbaFS_fclose fail.");
        Rval = CA_CALIB_CLOSE_FILE_ERROR;
        return Rval;
    }

    if (OffSensorFlag ) {
        SensorFlag = (SensorRawFlag & SensorCalFlag);
    }

    if ((MethodFlag == 0)||(RRatioFlag == 0)||(RedLineFlag == 0)||(BlueLineFlag == 0)||(SensorFlag == 0)) {
        if (SensorFlag == 0)            AmbaPrint("Sensor parameter not found or some error");
        if (MethodFlag == 0)            AmbaPrint("Method parameter not found or some error");
        if (CompensateRatioFlag == 0)   AmbaPrint("CompensateRatio parameter not found or some error");
        if (RRatioFlag == 0)            AmbaPrint("R_ratio parameter not found or some error");
        if (ExpectFlag == 0)            AmbaPrint("Expect parameter not found or some error");
        if (RedLineFlag == 0)           AmbaPrint("Redline_flag parameter not found or some error");
        if (BlueLineFlag == 0)          AmbaPrint("Blueline parameter not found or some error");

        Rval = CA_CALIB_INCORRECT_PARAMETER;
        return Rval;
    }

    //write ca data to binary file

    Rval = AmpUtil_GetAlignedPool(APPLIB_G_MMPL, &TempCATableBySpec, &TempCATableBySpecRel, (HorGridNum*VerGridNum*2*sizeof(INT16)), 32);
    BufferAddress = (UINT8*)((UINT32)TempCATableBySpec) ;
    if (Rval != OK) {
        AmbaPrint("allocate fail in %s:%u", __FUNCTION__, __LINE__);
        return CA_CALIB_CREATE_BUFFER_ERROR;
    } else {
        CalibCATableBySpec = (INT16 *)BufferAddress;
        AmbaPrint("[AppLibCalibCA]CalibCATableBySpec buffer (0x%08X) (%u)!", CalibCATableBySpec, (HorGridNum*VerGridNum*2*sizeof(INT16)));
    }
    Rval = AmpUtil_GetAlignedPool(APPLIB_G_MMPL, &TempWp, &TempWpRel, (HorGridNum * VerGridNum*sizeof(Cal_CA_Entry_s)), 32);
    BufferAddress = (UINT8*)((UINT32)TempWp) ;
    if (Rval != OK) {
        AmbaPrint("allocate fail in %s:%u", __FUNCTION__, __LINE__);
        return CA_CALIB_CREATE_BUFFER_ERROR;
    } else {
        Wp = (Cal_CA_Entry_s *)BufferAddress;
        AmbaPrint("[AppLibCalibCA]Wp buffer (0x%08X) (%u)!", Wp, (HorGridNum * VerGridNum*sizeof(Cal_CA_Entry_s)));
    }
    Rval = AmpUtil_GetAlignedPool(APPLIB_G_MMPL, &TempCATable, &TempCATableRel, (HorGridNum * VerGridNum*sizeof(AMBA_DSP_IMG_GRID_POINT_s)), 32);
    BufferAddress = (UINT8*)((UINT32)TempCATable) ;
    if (Rval != OK) {
        AmbaPrint("allocate fail in %s:%u", __FUNCTION__, __LINE__);
        return CA_CALIB_CREATE_BUFFER_ERROR;
    } else {
        CATable = (AMBA_DSP_IMG_GRID_POINT_s *)BufferAddress;
        AmbaPrint("[AppLibCalibCA]CATable buffer (0x%08X) (%u)!", CATable, (HorGridNum * VerGridNum*sizeof(AMBA_DSP_IMG_GRID_POINT_s)));
    }

    if (IdxNumB != IdxNumR) {
        AmbaPrint("RED LINE index num (%d) != BLUE LINE spec index num (%d)",IdxNumR,IdxNumB);
        Rval = CA_CALIB_INCORRECT_INPUT_NUM;
        return Rval;
    }
    if (IdxNumE != IdxNumR) {
        AmbaPrint("RED LINE index num (%d) != Expect parameter spec index num (%d)",IdxNumR,IdxNumE);
        Rval = CA_CALIB_INCORRECT_INPUT_NUM;
        return Rval;
    }
    IdxNum = IdxNumB;
    if (ZoomStep>14 || Method>5) {
        AmbaPrint("Illegal params of Method or ZoomStep");
        Rval = CA_CALIB_INCORRECT_PARAMETER;
        return Rval;
    }
    Rval = 0;

    /*** CA Header params in order as in bin file ***/

    AmbaPrint("Select Method #%d for generating table, Decay-ratio = %d, r-ratio = %d", Method, CompensateRatio, RRatio);
    //Method 0:auto, 1:BLUE only, 2:RED only, 3:BLUE table(scale to RED), 4:RED table(scale to Blue), 5:AVERAGE table
    for (Y=0; Y<VerGridNum; Y++) {
        for (X=0; X<HorGridNum; X++) {
            x_pr = X*TileWidth - CenterX;
            y_pr = Y*TileHeight - CenterY;
            RedHorShift  = RedVerShift = BlueHorShift = BlueVerShift = red_offset = blue_offset = 0;

            r = pow((x_pr*x_pr + y_pr*y_pr), 0.5);
            cos = x_pr/r;
            sin = y_pr/r;
            r = r * RRatio / 100;
            f = 0;
            for (i=0; i<IdxNum-1; i++) {
                a = Expect[i]/UnitCellSize; // in pixel
                b = Expect[i+1]/UnitCellSize;   // in pixel
                //now the ZoomStep always = 0
                red_offset = ((RedLine[i+1])/UnitCellSize) - ((RedLine[i])/UnitCellSize);
                blue_offset = ((BlueLine[i+1])/UnitCellSize) - ((BlueLine[i])/UnitCellSize);
                if ((a <= r) && (b >= r)) {
                    // found interval
                    red_offset = ((RedLine[i])/UnitCellSize) + red_offset*(r-a)/(b-a); //inter-OFFSET of (X,Y)
                    blue_offset = ((BlueLine[i])/UnitCellSize) + blue_offset*(r-a)/(b-a); //inter-OFFSET of (X,Y)
                    f = 1;
                    break;  // leave loop
                }
            }
            if (r > b) {
                // found interval
                red_offset = ((RedLine[i])/UnitCellSize) + red_offset*(r-a)/(b-a); //inter-OFFSET of (X,Y)
                blue_offset = ((BlueLine[i])/UnitCellSize) + blue_offset*(r-a)/(b-a); //inter-OFFSET of (X,Y)
                f = 1;
            }
            if (!f) {
                AmbaPrint("Not found interval for r %f, b %f, X %d, Y %d, i %d", r,b,X,Y, i);
                Rval = CA_CALIB_INTERVAL_NOT_FOUND;
                return Rval;
            }

            Wp[Y*HorGridNum+X].RedHor  = RedHorShift  = (-1.0)*red_offset*cos*(1<<CANumFracBits);
            Wp[Y*HorGridNum+X].RedVer  = RedVerShift  = (-1.0)*red_offset*sin*(1<<CANumFracBits);
            Wp[Y*HorGridNum+X].BlueHor = BlueHorShift = (-1.0)*blue_offset*cos*(1<<CANumFracBits);
            Wp[Y*HorGridNum+X].BlueVer = BlueVerShift = (-1.0)*blue_offset*sin*(1<<CANumFracBits);

            CrossProductSum += RedHorShift * BlueHorShift;
            CrossProductSum += RedVerShift * BlueVerShift;
            SquareSumR += RedHorShift * RedHorShift;
            SquareSumR += RedVerShift * RedVerShift;
            SquareSumB += BlueHorShift * BlueHorShift;
            SquareSumB += BlueVerShift * BlueVerShift;

            if ( Method==0 || Method==5 ) {
                double ShiftHorAvg = (RedHorShift + BlueHorShift) /2;
                double ShiftVerAvg = (RedVerShift + BlueVerShift) /2;
                CrossProductSumAvgR += RedHorShift * ShiftHorAvg;
                CrossProductSumAvgR += RedVerShift * ShiftVerAvg;
                CrossProductSumAvgB += BlueHorShift * ShiftHorAvg;
                CrossProductSumAvgB += BlueVerShift * ShiftVerAvg;
                SquareSumAvg += pow(ShiftHorAvg, 2);
                SquareSumAvg += pow(ShiftVerAvg, 2);
            }
        } //for (X)
    } //for (Y)
    //TABLE

    BlueHorShift = BlueVerShift = RedHorShift = RedVerShift = i = 0;

    /*** Auto Method ***/
    if (Method == 0 ) {
        double DistBo, DistRo, DistBetaR, DistBetaB, DistAvg, DistAvgTmp, DistTmp;
        double BetaR = (SquareSumB != 0)?(CrossProductSum / SquareSumB):1.0, BetaB = (SquareSumR != 0)?(CrossProductSum / SquareSumR):1.0;
        double BetaAvgR = (SquareSumAvg != 0)?(CrossProductSumAvgR / SquareSumAvg):1.0, BetaAvgB = (SquareSumAvg != 0)?(CrossProductSumAvgB / SquareSumAvg):1.0;
        INT32 MethodAuto = 0;
        DistBo= DistRo= DistBetaR= DistBetaB= DistAvg= DistAvgTmp= 0;

        /***  Compute distortion of each Method ***/
        for (Y=0; Y<VerGridNum; Y++) {
            for (X=0; X<HorGridNum; X++) {
                RedHorShift = Wp[Y*HorGridNum+X].RedHor;
                RedVerShift = Wp[Y*HorGridNum+X].RedVer;
                BlueHorShift = Wp[Y*HorGridNum+X].BlueHor;
                BlueVerShift = Wp[Y*HorGridNum+X].BlueVer;

                //BLUE only
                DistBo += ( pow(RedHorShift,2) + pow(RedVerShift,2) );
                //RED only
                DistRo += ( pow(BlueHorShift,2) + pow(BlueVerShift,2) );
                //BLUE & beta_R
                DistBetaR += ( pow(BlueHorShift*BetaR - RedHorShift, 2) + pow(BlueVerShift*BetaR  - RedVerShift, 2) );
                //RED & beta_B
                DistBetaB += ( pow(RedHorShift*BetaB - BlueHorShift, 2) + pow(RedVerShift*BetaB - BlueVerShift, 2) );
                //Average
                DistAvgTmp = 0;
                DistAvgTmp += pow( ((RedHorShift+BlueHorShift)/2) * BetaAvgR - RedHorShift, 2);
                DistAvgTmp += pow( ((RedVerShift+BlueVerShift)/2) * BetaAvgR - RedVerShift, 2);
                DistAvgTmp += pow( ((RedHorShift+BlueHorShift)/2) * BetaAvgB - BlueHorShift, 2);
                DistAvgTmp += pow( ((RedVerShift+BlueVerShift)/2) * BetaAvgB - BlueVerShift, 2);
                DistAvg += DistAvgTmp;
            }
        }
        //Method 0:auto,     1:BLUE only,    2:RED only,
        //       3:BLUE table(scale to RED), 4:RED table(scale to Blue), 5:AVERAGE table
        /***  Compare distortion of each Method ***/
        DistTmp = DistBo;
        MethodAuto = 1;
        if ( DistRo < DistTmp) {
            DistTmp = DistRo;
            MethodAuto = 2;
        }
        if ( DistBetaR < DistTmp) {
            DistTmp = DistBetaR;
            MethodAuto = 3;
        }
        if ( DistBetaB < DistTmp) {
            DistTmp = DistBetaB;
            MethodAuto = 4;
        }
        if ( DistAvg < DistTmp) {
            DistTmp = DistAvg;
            MethodAuto = 5;
        }
        Method = MethodAuto;
        AmbaPrint("AUTO SELECT Method#: %d", Method);
    }

    /*** Compute Beta ***/
    //Method 0:auto, 1:BLUE only, 2:RED only,
    //       3:BLUE table(scale to RED), 4:RED table(scale to Blue), 5:AVERAGE table
    switch (Method) {
        case 1: // BLUE only, RED=0
            RedScaleFactor = 0;
            BlueScaleFactor = -256;
            break;
        case 2: // RED only, BLUE=0
            RedScaleFactor = -256;
            BlueScaleFactor = 0;
            break;
        case 3: // BLUE table(scale to RED)
            if (SquareSumB != 0) {
                RedScaleFactor = (INT32)(-256.0 * (CrossProductSum / SquareSumB));
            }
            BlueScaleFactor = -256;
            break;
        case 4: // RED table(scale to Blue)
            if (SquareSumR != 0) {
                BlueScaleFactor = (INT32)(-256.0 * (CrossProductSum / SquareSumR));
            }
            RedScaleFactor = -256;
            break;
        case 5: // AVERAGE table
            RedScaleFactor = (INT32)(-256.0 * (CrossProductSumAvgR / SquareSumAvg));
            BlueScaleFactor = (INT32)(-256.0 * (CrossProductSumAvgB / SquareSumAvg));
            break;
        default: {
            AmbaPrint("Wrong value for Method request.");
            return CA_CALIB_UNDEFINED_METHOD;
        }
    }
    AmbaPrintColor(RED,"1 RedScaleFactor = %d BlueScaleFactor",RedScaleFactor,BlueScaleFactor);
    //compute corresponding table of select Method
    if (RedScaleFactor < -256 || BlueScaleFactor < -256 || RedScaleFactor > 255 || BlueScaleFactor > 255) {

        if (ABS(RedScaleFactor) == ABS(BlueScaleFactor)) {
            ExceedScaleFactor = (double) (RedScaleFactor > 0)? RedScaleFactor: BlueScaleFactor;
            RedScaleFactor = (INT32)( (RedScaleFactor /ExceedScaleFactor) * -256 );
            BlueScaleFactor = (INT32)( (BlueScaleFactor /ExceedScaleFactor ) * -256);

            if (RedScaleFactor == 256) {
                RedScaleFactor = 255;
            } else if (BlueScaleFactor == 256) {
                BlueScaleFactor = 255;
            }
        } else {
            if ((RedScaleFactor < -256 || RedScaleFactor > 255) && (BlueScaleFactor < -256 || BlueScaleFactor > 255)) {
                ExceedScaleFactor = (double) (ABS(RedScaleFactor) >  ABS(BlueScaleFactor))? RedScaleFactor: BlueScaleFactor;
            } else {
                ExceedScaleFactor = (double) (RedScaleFactor < -256 ||  RedScaleFactor > 255)? RedScaleFactor: BlueScaleFactor;
            }
            RedScaleFactor = (INT32)( (RedScaleFactor /ExceedScaleFactor) * -256 );
            BlueScaleFactor = (INT32)( (BlueScaleFactor /ExceedScaleFactor ) * -256);
        }
    }
    ExceedScaleFactor = ExceedScaleFactor/(-256);
    AmbaPrintColor(RED,"1 RedScaleFactor = %d BlueScaleFactor",RedScaleFactor,BlueScaleFactor);

    BlueHorShift = BlueVerShift = RedHorShift = RedVerShift = i = 0;
    for (Y=0; Y<VerGridNum; Y++) {
        for (X=0; X<HorGridNum; X++) {
            RedHorShift = Wp[Y*HorGridNum+X].RedHor;
            RedVerShift = Wp[Y*HorGridNum+X].RedVer;
            BlueHorShift = Wp[Y*HorGridNum+X].BlueHor;
            BlueVerShift = Wp[Y*HorGridNum+X].BlueVer;

            if ( Method == 1 || Method == 3 ) {
                CATable[Y*HorGridNum+X].X = (INT32)(BlueHorShift * ExceedScaleFactor * CompensateRatio / 100);
                CATable[Y*HorGridNum+X].Y = (INT32)(BlueVerShift * ExceedScaleFactor * CompensateRatio / 100);
            } else if ( Method == 2 || Method == 4 ) {
                CATable[Y*HorGridNum+X].X = (INT32)(RedHorShift * ExceedScaleFactor * CompensateRatio / 100);
                CATable[Y*HorGridNum+X].Y = (INT32)(RedVerShift * ExceedScaleFactor * CompensateRatio / 100);
            } else if ( Method == 5 ) {
                CATable[Y*HorGridNum+X].X = (INT32)((RedHorShift+BlueHorShift)/2 * ExceedScaleFactor * CompensateRatio / 100);
                CATable[Y*HorGridNum+X].Y = (INT32)((RedVerShift+BlueVerShift)/2 * ExceedScaleFactor * CompensateRatio / 100);
            }
            CATable[Y*HorGridNum+X].X = ( CATable[Y*HorGridNum+X].X > 256 )? 256: ( CATable[Y*HorGridNum+X].X < -256 )? -256: CATable[Y*HorGridNum+X].X;
            CATable[Y*HorGridNum+X].Y = ( CATable[Y*HorGridNum+X].Y > 64 )? 64: ( CATable[Y*HorGridNum+X].Y < -64 )? -64: CATable[Y*HorGridNum+X].Y;
        }
    }
    //TileWidthExp;   /* 4:16, 5:32, 6:64, 7:128, 8:256, 9:512 */
    //TileWidthExp;   /* 4:16, 5:32, 6:64, 7:128, 8:256, 9:512 */
    switch (TileWidth) {
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

    switch (TileHeight) {
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

    //write the ca header & table
    CalibCAHeaderSpec[0] = CAL_CA_VER;
    CalibCAHeaderSpec[1] = HorGridNum;
    CalibCAHeaderSpec[2] = VerGridNum;
    CalibCAHeaderSpec[3] = TileWidthExp;
    CalibCAHeaderSpec[4] = TileHeightExp;
    CalibCAHeaderSpec[5] = StartX;
    CalibCAHeaderSpec[6] = StartY;
    CalibCAHeaderSpec[7] = ImgWidth;
    CalibCAHeaderSpec[8] = ImgHeight;
    CalibCAHeaderSpec[9] = HSubSampleFactorNum;
    CalibCAHeaderSpec[10] = HSubSampleFactorDen;
    CalibCAHeaderSpec[11] = VSubSampleFactorNum;
    CalibCAHeaderSpec[12] = VSubSampleFactorDen;
    CalibCAHeaderSpec[13] = RedScaleFactor;
    CalibCAHeaderSpec[14] = BlueScaleFactor;
    CalibCAHeaderSpec[15] = RESERVED1;
    CalibCAHeaderSpec[16] = RESERVED1;


    AmbaPrint("version: 0x%X",CAL_CA_VER);
    AmbaPrint("horizontal grid number: %d",HorGridNum);
    AmbaPrint("vertical grid number: %d",VerGridNum);
    AmbaPrint("tile width: %d",TileWidthExp);
    AmbaPrint("tile height: %d",TileHeightExp);
    AmbaPrint("Image width: %d",ImgWidth);
    AmbaPrint("Image height: %d",ImgHeight);
    AmbaPrint("StartX: %d",StartX);
    AmbaPrint("StartY: %d",StartY);
    AmbaPrint("HSubSample FactorDen: %d",HSubSampleFactorNum);
    AmbaPrint("HSubSample FactorNum: %d",HSubSampleFactorDen);
    AmbaPrint("VSubSample FactorDen: %d",VSubSampleFactorNum);
    AmbaPrint("VSubSample FactorNum: %d",VSubSampleFactorDen);
    AmbaPrint("RedScaleFactor: %d",RedScaleFactor);
    AmbaPrint("BlueScaleFactor: %d",BlueScaleFactor);



    for (j = 0; j < HorGridNum * VerGridNum; j++) {
        CalibCATableBySpec[2*j] = CATable[j].X;
        CalibCATableBySpec[2*j+1] = CATable[j].Y;
    }

    /***     FILE STORE       ****/
    // calibration binary file
    TestCAFn[0] = AppLib_CalibGetDriverLetter();
    fp = AmbaFS_fopen(TestCAFn, "w");
    if (fp == NULL) {
        AmbaPrint("AmbaFS_fopen %s TestCAFn  fail", TestCAFn);
        return CA_CALIB_OPEN_FILE_ERROR;
    }

    j = 0;
    Tmp = 1;                    AmbaFS_fwrite(&Tmp, 4, 1, fp);      j+=4;//CA_ENABLE
    Tmp = 1;                    AmbaFS_fwrite(&Tmp, 4, 1, fp);      j+=4;//CA_TABLE_COUNT
    Tmp = RESERVED1;            AmbaFS_fwrite(&Tmp, 4, 1, fp);      j+=4;//Reserved[0]
    Tmp = RESERVED1;            AmbaFS_fwrite(&Tmp, 4, 1, fp);      j+=4;//Reserved[1]
    Tmp = RESERVED1;            AmbaFS_fwrite(&Tmp, 4, 1, fp);      j+=4;//Reserved[2]
    Tmp = RESERVED1;            AmbaFS_fwrite(&Tmp, 4, 1, fp);      j+=4;//Reserved[3]
    Tmp = RESERVED1;            AmbaFS_fwrite(&Tmp, 4, 1, fp);      j+=4;//Reserved[4]
    Tmp = RESERVED1;            AmbaFS_fwrite(&Tmp, 4, 1, fp);      j+=4;//Reserved[5]
    Tmp = 1;                    AmbaFS_fwrite(&Tmp, 4, 1, fp);      j+=4;//CA table enable
    Tmp = CAL_CA_VER;           AmbaFS_fwrite(&Tmp, 4, 1, fp);      j+=4;//CA version
    Tmp = HorGridNum;           AmbaFS_fwrite(&Tmp, 4, 1, fp);      j+=4;// Horizontal grid number
    Tmp = VerGridNum;           AmbaFS_fwrite(&Tmp, 4, 1, fp);      j+=4;//Vertical grid number
    Tmp = TileWidthExp;         AmbaFS_fwrite(&Tmp, 4, 1, fp);      j+=4;// 4:16, 5:32, 6:64, 7:128, 8:256, 9:512
    Tmp = TileHeightExp;        AmbaFS_fwrite(&Tmp, 4, 1, fp);      j+=4;// 4:16, 5:32, 6:64, 7:128, 8:256, 9:512
    Tmp = StartX;               AmbaFS_fwrite(&Tmp, 4, 1, fp);      j+=4;// StartX
    Tmp = StartY;               AmbaFS_fwrite(&Tmp, 4, 1, fp);      j+=4;// StartY
    Tmp = ImgWidth;             AmbaFS_fwrite(&Tmp, 4, 1, fp);      j+=4;// ImgWidth
    Tmp = ImgHeight;            AmbaFS_fwrite(&Tmp, 4, 1, fp);      j+=4;// ImgHeight
    Tmp = HSubSampleFactorNum;  AmbaFS_fwrite(&Tmp, 1, 1, fp);      j+=1;// HSubSampleFactorNum
    Tmp = HSubSampleFactorDen;  AmbaFS_fwrite(&Tmp, 1, 1, fp);      j+=1;// HSubSampleFactorDen
    Tmp = VSubSampleFactorNum;  AmbaFS_fwrite(&Tmp, 1, 1, fp);      j+=1;// VSubSampleFactorNum
    Tmp = VSubSampleFactorDen;  AmbaFS_fwrite(&Tmp, 1, 1, fp);      j+=1;// VSubSampleFactorDen
    Tmp = RedScaleFactor;       AmbaFS_fwrite(&Tmp, 2, 1, fp);      j+=2;// RedScaleFactor
    Tmp = BlueScaleFactor;      AmbaFS_fwrite(&Tmp, 2, 1, fp);      j+=2;// BlueScaleFactor
    Tmp = RESERVED1;            AmbaFS_fwrite(&Tmp, 4, 1, fp);      j+=4;// Reserved1
    Tmp = RESERVED2;            AmbaFS_fwrite(&Tmp, 4, 1, fp);      j+=4;// Reserved2
    Tmp = CAZoomStep;           AmbaFS_fwrite(&Tmp, 4, 1, fp);      j+=4;// CAZoomStep
    Tmp = CAChannelId;          AmbaFS_fwrite(&Tmp, 4, 1, fp);      j+=4;// CAChannelId

    Rval = AmbaFS_fwrite(CATable, CAL_CA_SIZE-(sizeof(CA_Storage_Header_s)+32), 1, fp);
    if (Rval == 0) {
        AmbaPrint("AmbaFS_fwrite fail.");
        return CA_CALIB_WRITE_FILE_ERROR;
    }
    Rval = AmbaFS_fclose(fp);
    if (Rval != 0) {
        AmbaPrint("AmbaFS_fclose fail.2");
        return CA_CALIB_CLOSE_FILE_ERROR;
    }

    //H file
    CSHfileFn[0] = AppLib_CalibGetDriverLetter();
    Fid = AmbaFS_fopen(CSHfileFn, "w");
    if (Fid == NULL) {
        AmbaPrint("AmbaFS_fopen %s ca_ver  fail", CSHfileFn);
        return CA_CALIB_WRITE_FILE_ERROR;
    }
    //write CA header into file
    sprintf(Str, "%s",  "#define GRID_W ");
    AmbaFS_fwrite(Str, strlen(Str), 1, Fid);
    sprintf(Str, " %5d ",  CalibCAHeaderSpec[1]);
    AmbaFS_fwrite(Str, strlen(Str), 1, Fid);
    AmbaFS_fwrite(&ChangeLine, 1, 1, Fid);
    sprintf(Str, "%s",  "#define GRID_H ");
    AmbaFS_fwrite(Str, strlen(Str), 1, Fid);
    sprintf(Str, " %5d ",  CalibCAHeaderSpec[2]);
    AmbaFS_fwrite(Str, strlen(Str), 1, Fid);
    AmbaFS_fwrite(&ChangeLine, 1, 1, Fid);
    sprintf(Str, "%s",  "INT32 CalibCAHeaderSpec[64] =");
    AmbaFS_fwrite(Str, strlen(Str), 1, Fid);
    AmbaFS_fwrite(&ChangeLine, 1, 1, Fid);
    sprintf(Str, "%s",  "{");
    AmbaFS_fwrite(Str, strlen(Str), 1, Fid);
    AmbaFS_fwrite(&ChangeLine, 1, 1, Fid);
    sprintf(Str, "0x%X,",  CAL_CA_VER);
    AmbaFS_fwrite(Str, strlen(Str), 1, Fid);
    AmbaFS_fwrite(&ChangeLine, 1, 1, Fid);
    for (i=1; i<=16; i++) {
        sprintf(Str, " %5d, ", CalibCAHeaderSpec[i] );
        AmbaFS_fwrite(Str, strlen(Str), 1, Fid);
        AmbaFS_fwrite(&ChangeLine, 1, 1, Fid);
    }
    sprintf(Str, "%s",  "};");
    AmbaFS_fwrite(Str, strlen(Str), 1, Fid);
    AmbaFS_fwrite(&ChangeLine, 1, 1, Fid);
    AmbaFS_fwrite(&ChangeLine, 1, 1, Fid);

    //write CA table into file
    sprintf(Str, "%s",  "INT16 calib_ca_table_spec horizontal [HorGridNum * VerGridNum *2] =");
    AmbaFS_fwrite(Str, strlen(Str), 1, Fid);
    AmbaFS_fwrite(&ChangeLine, 1, 1, Fid);
    sprintf(Str, "%s",  "{");
    AmbaFS_fwrite(Str, strlen(Str), 1, Fid);
    AmbaFS_fwrite(&ChangeLine, 1, 1, Fid);
    for (j = 0; j < HorGridNum * VerGridNum; j++) {
        sprintf(Str, " %5d, ", CATable[j].X);
        AmbaFS_fwrite(Str, strlen(Str), 1, Fid);
        if (j % HorGridNum == (HorGridNum-1)) {
            AmbaFS_fwrite(&ChangeLine, 1, 1, Fid);
        }
    }
    sprintf(Str, "%s",  "};");
    AmbaFS_fwrite(Str, strlen(Str), 1, Fid);
    AmbaFS_fwrite(&ChangeLine, 1, 1, Fid);

    sprintf(Str, "%s",  "INT16 calib_ca_table_spec vertical [HorGridNum * VerGridNum *2] =");
    AmbaFS_fwrite(Str, strlen(Str), 1, Fid);
    AmbaFS_fwrite(&ChangeLine, 1, 1, Fid);
    sprintf(Str, "%s",  "{");
    AmbaFS_fwrite(Str, strlen(Str), 1, Fid);
    AmbaFS_fwrite(&ChangeLine, 1, 1, Fid);
    for (j = 0; j < HorGridNum * VerGridNum; j++) {
        sprintf(Str, " %5d, ", CATable[j].Y);
        AmbaFS_fwrite(Str, strlen(Str), 1, Fid);
        if (j % HorGridNum == (HorGridNum-1)) {
            AmbaFS_fwrite(&ChangeLine, 1, 1, Fid);
        }
    }
    sprintf(Str, "%s",  "};");
    AmbaFS_fwrite(Str, strlen(Str), 1, Fid);
    AmbaFS_fwrite(&ChangeLine, 1, 1, Fid);

    AmbaFS_fclose(Fid);
    if (Rval != 0) {
        AmbaPrint("AmbaFS_fclose fail.3");
        return CA_CALIB_CLOSE_FILE_ERROR;
    }

    {

        AMBA_DSP_IMG_MODE_CFG_s ImgMode;
        UINT32 Config;
        memset(&ImgMode, 0, sizeof(AMBA_DSP_IMG_MODE_CFG_s));
        Config = 0;//enable CA
        CAInfo[0].Version                            = CAL_CA_VER;
        CAInfo[0].HorGridNum                         = HorGridNum;
        CAInfo[0].VerGridNum                         = VerGridNum;
        CAInfo[0].TileWidthExp                       = TileWidthExp;
        CAInfo[0].TileHeightExp                      = TileHeightExp;
        CAInfo[0].VinSensorGeo.Width                 = ImgWidth;
        CAInfo[0].VinSensorGeo.Height                = ImgHeight;
        CAInfo[0].VinSensorGeo.StartX                = StartX;
        CAInfo[0].VinSensorGeo.StartY                = StartY;
        CAInfo[0].VinSensorGeo.HSubSample.FactorDen  = HSubSampleFactorDen;
        CAInfo[0].VinSensorGeo.HSubSample.FactorNum  = HSubSampleFactorNum;
        CAInfo[0].VinSensorGeo.VSubSample.FactorDen  = HSubSampleFactorDen;
        CAInfo[0].VinSensorGeo.VSubSample.FactorNum  = VSubSampleFactorNum;
        CAInfo[0].RedScaleFactor                     = RedScaleFactor;
        CAInfo[0].BlueScaleFactor                    = BlueScaleFactor;

        CAInfo[0].Reserved                           = RESERVED1;
        CAInfo[0].Reserved1                          = RESERVED2;
        CAInfo[0].pCaWarp                              = CATable;


        AmbaPrint("version: 0x%X",CAInfo[0].Version);
        AmbaPrint("horizontal grid number: %d",CAInfo[0].HorGridNum);
        AmbaPrint("vertical grid number: %d",CAInfo[0].VerGridNum);
        AmbaPrint("tile width: %d",CAInfo[0].TileWidthExp);
        AmbaPrint("tile height: %d",CAInfo[0].TileHeightExp);
        AmbaPrint("Image width: %d",CAInfo[0].VinSensorGeo.Width);
        AmbaPrint("Image height: %d",CAInfo[0].VinSensorGeo.Height);
        AmbaPrint("StartX: %d",CAInfo[0].VinSensorGeo.StartX);
        AmbaPrint("StartY: %d",CAInfo[0].VinSensorGeo.StartY);
        AmbaPrint("HSubSample FactorDen: %d",CAInfo[0].VinSensorGeo.HSubSample.FactorDen);
        AmbaPrint("HSubSample FactorNum: %d",CAInfo[0].VinSensorGeo.HSubSample.FactorNum);
        AmbaPrint("VSubSample FactorDen: %d",CAInfo[0].VinSensorGeo.VSubSample.FactorDen);
        AmbaPrint("VSubSample FactorNum: %d",CAInfo[0].VinSensorGeo.VSubSample.FactorNum);
        AmbaPrint("RedScaleFactor: %d",CAInfo[0].RedScaleFactor);
        AmbaPrint("BlueScaleFactor: %d",CAInfo[0].BlueScaleFactor);
        AmbaPrint("CA table address: 0x%X",CAInfo[0].pCaWarp);

        ImgMode.Pipe = AMBA_DSP_IMG_PIPE_VIDEO;
        AmbaDSP_WarpCore_SetCalibCawarpInfo(&ImgMode,&CAInfo[0]);
        AmbaDSP_WarpCore_CalcDspCawarp(&ImgMode, Config);
        AmbaDSP_WarpCore_SetDspCawarp(&ImgMode);
    }
    Rval = 0;
    AmbaKAL_BytePoolFree(TempExpectRel);
    AmbaKAL_BytePoolFree(TempRedLineRel);
    AmbaKAL_BytePoolFree(TempBlueLineRel);
    AmbaKAL_BytePoolFree(TempCATableBySpecRel);
    AmbaKAL_BytePoolFree(TempWpRel);
    AmbaKAL_BytePoolFree(TempCATableRel);
    /***     FILE STORE END  ****/

    //save the CA calibration data for raw encode
    CARawEncFn[0] = AppLib_CalibGetDriverLetter();
    Fid = AmbaFS_fopen(CARawEncFn, "w");
    if (Fid == NULL) {
        AmbaPrint("AmbaFS_fopen %s fail.", CARawEncFn);
        Rval = CA_CALIB_OPEN_FILE_ERROR;
        return Rval;
    }
    Rval = AmbaFS_fwrite(CATable, (HorGridNum*VerGridNum<<2), 1, Fid);
    if (Rval == 0) {
        AmbaPrint("AmbaFS_fwrite fail.");
        Rval = CA_CALIB_CLOSE_FILE_ERROR;
        return Rval;
    }
    Rval = AmbaFS_fclose(Fid);
    if (Rval != 0) {
        AmbaPrint("AmbaFS_fclose fail.");
        Rval = CA_CALIB_CLOSE_FILE_ERROR;
        return Rval;
    }

    ///text raw encode
    if (Rval == 0) {
        AmbaPrint("Gen CA succeed.");
    }


    return Rval;
}


/**
 *  @brief the upgrade function for CA calibration
 *
 *  the upgrade function for CA calibration
 *
 *  @param [in]CalSite calibration site status
 *  @param [in]CalObj calibration object
 *
 *  @return 0 success, -1 failure
 */
INT32 AppLibCalibCA_Upgrade(Cal_Obj_s *CalObj, Cal_Stie_Status_s* CalSite)
{
    if (CalObj->Version != CalSite->Version) {
        // This API is an example to handle calibration data upgrade
        AmbaPrint("[CAL] Site %s Version mismatch (FW:0x%08X, NAND:0x%08X)", CalObj->Name, CalObj->Version, CalSite->Version);
    }
    // The default behavior is to do-nothing when Version mismatch
    return 0;

}

/**
 *  @brief the entry function for CA calibration function
 *
 *  the entry function for CA calibration function
 *
 *  @param [in]Argc input number for CA calibration
 *  @param [in]Argv input value for CA calibration
 *  @param [in]CalSite calibration site status
 *  @param [in]CalObj calibration object
 *  @param [out]OutputStr output debug message
 *
 *  @return 0 success, -1 failure
 */
INT32 AppLibCalibCA_Func(INT32 Argc, char *Argv[], char *OutputStr, Cal_Stie_Status_s *CalSite, Cal_Obj_s *CalObj)
{
    return 0;
}


/**
 *  @brief the unit test function for CA calibration
 *
 *  the unit test function for CA calibration
 *
 *  @param [in] env environment
 *  @param [in]Argc input number for CA calibration
 *  @param [in]Argv input value for CA calibration
 *
 *  @return 0 success, -1 failure
 */
INT32 AppLibCA_UTFunc(INT32 Argc, char *Argv[])
{
    Cal_Obj_s           *CalObj;
    INT32 Rval = -1;

    CalObj = AppLib_CalibGetObj(CAL_CA_ID);
    if ((strcmp(Argv[2], "test") == 0)) {
        //register calibration site
        AppLib_CalibSiteInit();
        Rval = 0;
    } else if ((strcmp(Argv[2], "reset") == 0)) {
        AppLib_CalibMemReset(CAL_CA_ID);
        Rval = 0;
    } else if ((strcmp(Argv[2], "init") == 0)) {
        AppLibCalibCA_Init(CalObj);
        Rval = 0;
    } else if ((strcmp(Argv[2], "ca_spec") == 0)) {
        AppLibCalibCA_GenCAFromSpec(Argc,Argv);
        Rval = 0;
    } else if ((strcmp(Argv[2], "select") == 0)) {
        INT8 Enable       = atoi(Argv[3]);
        INT8 Channel      = atoi(Argv[4]);
        INT8 Id1          = atoi(Argv[5]);
        INT8 Id2          = atoi(Argv[6]);
        INT32 BlendRatio  = atoi(Argv[7]);
        INT32 Strength  = atoi(Argv[8]);
        Rval = AppLibCalibCA_SelectCATable(Enable, Channel, Id1, Id2, BlendRatio, Strength);
    }else if((strcmp(Argv[2], "debug") == 0)) {
        int Enable = atoi(Argv[3]);
        AppLibCalibCA_DebugEnable(Enable);
        Rval = 0;    
    } else if((strcmp(Argv[2], "info") == 0)){
        int i;

        if (AppCAControl.Enable) {
            AmbaPrint("enable = %d , CATableCount = %d", AppCAControl.Enable, AppCAControl.CATableCount);
            for(i = 0; i < MAX_CA_TABLE_COUNT; i++) {
                AmbaPrint("---------- storage #%d ----------", i);
                AmbaPrint("enable = %d", AppCAControl.CATable[i]->CAHeader.Enable);
                AmbaPrint("[swin_info] start_x = %u, start_y = %u, width = %u, height = %u",
                            AppCAControl.CATable[i]->CAHeader.VinSensorGeo.StartX, AppCAControl.CATable[i]->CAHeader.VinSensorGeo.StartY,
                            AppCAControl.CATable[i]->CAHeader.VinSensorGeo.Width, AppCAControl.CATable[i]->CAHeader.VinSensorGeo.Height);
                AmbaPrint("HorGridNum = %d, VerGridNum = %d, TileWidthExp = %d, TileHeightExp = %d", \
                            AppCAControl.CATable[i]->CAHeader.HorGridNum,AppCAControl.CATable[i]->CAHeader.HorGridNum, \
                            AppCAControl.CATable[i]->CAHeader.TileWidthExp,AppCAControl.CATable[i]->CAHeader.TileHeightExp);
            }
        } else {
            AmbaPrint("NO CA calibration");
        }
        Rval = 0;
    }

    if (Rval == -1) {
        AmbaPrint("t cal CA init : re-init CA");
        AmbaPrint("t cal ca ca_spec 0 [script.txt] ");
        AmbaPrint("t cal ca select Enable Channel Id1 Id2 BlendRatio Strength: select CA table");
        AmbaPrint("t cal ca debug enable :enable debug flag to print message");
        AmbaPrint("t cal ca info :CA calibration information");
    }

    return Rval;
}


