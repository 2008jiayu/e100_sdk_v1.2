
/**
 * @file src/app/connected/applib/src/calibration/ApplibCalibVar.c
 *
 * sample code for register Calibration sites
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

#include <calibration/ApplibCalibMgr.h>
#include <AmbaPrintk.h>



/* ---------- */
// The DRAM shadow of each item is put here
/* 00 */ static UINT8 DataStatus[CAL_STATUS_SIZE];
/* 01 */ static UINT8 DataAF[CAL_AF_SIZE];
/* 02 */ static UINT8 DataGyro[CAL_GYRO_SIZE];
/* 03 */ static UINT8 DataMshutter[CAL_MSHUTTER_SIZE];
/* 04 */ static UINT8 DataIris[CAL_IRIS_SIZE];
/* 05 */ static UINT8 DataVignette[CAL_VIGNETTE_SIZE];
/* 06 */ static UINT8 DataWarp[CAL_WARP_SIZE];
/* 07 */ static UINT8 DataBPC[CAL_BPC_SIZE];
/* 08 */ static UINT8 DataWB[CAL_WB_SIZE];
/* 09 */ static UINT8 DataIso[CAL_ISO_SIZE];
/* 10 */ static UINT8 DataBLC[CAL_BLC_SIZE];
/* 11 */ static UINT8 DataFlash[CAL_FLASH_SIZE];
/* 12 */ static UINT8 DataAudio[CAL_AUDIO_SIZE];
/* 13 */ static UINT8 DataCA[CAL_CA_SIZE];
/* 14 */ static UINT8 DataLensShift[CAL_LENSSHIFT_SIZE];



extern int AppLibCalibVignette_Func(int argc, char *argv[], char *OutputStr, Cal_Stie_Status_s *CalSite, Cal_Obj_s *CalObj);
extern int AppLibCalibVignette_Init (Cal_Obj_s *CalObj);
extern int AppLibCalibBPC_Init(Cal_Obj_s *CalObj);
extern int AppLibCalibBPC_Func(int argc, char *argv[], char *OutputStr, Cal_Stie_Status_s *CalSite, Cal_Obj_s *CalObj);
extern int AppLibCalibBLC_Init(Cal_Obj_s *CalObj);
extern int AppLibCalibBLC_Func(int Argc, char *Argv[], char *OutputStr, Cal_Stie_Status_s *CalSite, Cal_Obj_s *CalObj);
extern int AppLibCalibBLC_Upgrade(Cal_Obj_s *CalObj, Cal_Stie_Status_s* CalSite);
extern int AppLibCalibWB_Init(Cal_Obj_s *CalObj);
extern int AppLibCalibWB_Func(int Argc, char *Argv[], char *OutputStr, Cal_Stie_Status_s *CalSite, Cal_Obj_s *CalObj);
extern int AppLibCalibWB_Upgrade(Cal_Obj_s *CalObj, Cal_Stie_Status_s* CalSite);
extern int AppLibCalibWarp_Init(Cal_Obj_s *CalObj);
extern int AppLibCalibWarp_Func(int Argc, char *Argv[], char *OutputStr, Cal_Stie_Status_s *CalSite, Cal_Obj_s *CalObj);
extern int AppLibCalibWarp_Upgrade(Cal_Obj_s *CalObj, Cal_Stie_Status_s* CalSite);
extern int AppLibCalibCA_Init(Cal_Obj_s *CalObj);
extern int AppLibCalibCA_Func(int Argc, char *Argv[], char *OutputStr, Cal_Stie_Status_s *CalSite, Cal_Obj_s *CalObj);
extern int AppLibCalibCA_Upgrade(Cal_Obj_s *CalObj, Cal_Stie_Status_s* CalSite);
extern int AppLibCalibGyro_Init(Cal_Obj_s *CalObj);
extern int AppLibCalibGyro_Func(int Argc, char *Argv[], char *OutputStr, Cal_Stie_Status_s *CalSite, Cal_Obj_s *CalObj);
extern int AppLibCalibGyro_Upgrade(Cal_Obj_s *CalObj, Cal_Stie_Status_s* CalSite);

// Calibration manager working table
Cal_Obj_s CalObjTable[NVD_CALIB_MAX_OBJS];



/* ---------- */
// Ambarella's calibration hookup function

Cal_Obj_s CalObjAmbarella[] = {
    /* 00 */ {1, CAL_STATUS_ID,   CAL_STATUS_SIZE,   CAL_STATUS_VER,   "STATUS",   DataStatus,               NULL,                NULL,              NULL },
    /* 01 */ {0, CAL_AF_ID,       CAL_AF_SIZE,       CAL_AF_VER,       "AF",       DataAF,             NULL,       NULL,       NULL },
    /* 02 */ {1, CAL_GYRO_ID,     CAL_GYRO_SIZE,     CAL_GYRO_VER,     "GYRO",     DataGyro,             AppLibCalibGyro_Init, AppLibCalibGyro_Upgrade, AppLibCalibGyro_Func },
    /* 03 */ {0, CAL_MSHUTTER_ID, CAL_MSHUTTER_SIZE, CAL_MSHUTTER_VER, "MSHUTTER", DataMshutter,             NULL,       NULL,       NULL },
    /* 04 */ {0, CAL_IRIS_ID,     CAL_IRIS_SIZE,     CAL_IRIS_VER,     "IRIS",     DataIris,             NULL,       NULL,       NULL },
    /* 05 */ {1, CAL_VIGNETTE_ID, CAL_VIGNETTE_SIZE, CAL_VIGNETTE_VER, "VIGNETTE", DataVignette, AppLibCalibVignette_Init, NULL, AppLibCalibVignette_Func },
    /* 06 */ {1, CAL_WARP_ID,     CAL_WARP_SIZE,     CAL_WARP_VER,     "WARP",     DataWarp,    AppLibCalibWarp_Init,     AppLibCalibWarp_Upgrade,AppLibCalibWarp_Func },
    /* 07 */ {1, CAL_BPC_ID,      CAL_BPC_SIZE,      CAL_BPC_VER,      "BPC",      DataBPC,     AppLibCalibBPC_Init,      NULL,      AppLibCalibBPC_Func },
    /* 08 */ {1, CAL_WB_ID,       CAL_WB_SIZE,       CAL_WB_VER,       "WB",       DataWB,     AppLibCalibWB_Init,  AppLibCalibWB_Upgrade,   AppLibCalibWB_Func},
    /* 09 */ {0, CAL_ISO_ID,      CAL_ISO_SIZE,      CAL_ISO_VER,      "ISO",      DataIso,             NULL,       NULL,       NULL },
    /* 10 */ {1, CAL_BLC_ID,      CAL_BLC_SIZE,      CAL_BLC_VER,      "BLC",      DataBLC,    AppLibCalibBLC_Init,AppLibCalibBLC_Upgrade, AppLibCalibBLC_Func },
    /* 11 */ {0, CAL_FLASH_ID,    CAL_FLASH_SIZE,    CAL_FLASH_VER,    "FLASH",    DataFlash,             NULL,       NULL,       NULL },
    /* 12 */ {0, CAL_AUDIO_ID,    CAL_AUDIO_SIZE,    CAL_AUDIO_VER,    "AUDIO",    DataAudio,             NULL,       NULL,       NULL },
    /* 13 */ {1, CAL_CA_ID,       CAL_CA_SIZE,       CAL_CA_VER,       "CA",       DataCA,             AppLibCalibCA_Init,       AppLibCalibCA_Upgrade,       AppLibCalibCA_Func },
    /* 14 */ {0, CAL_LENSSHIFT_ID,CAL_LENSSHIFT_SIZE,CAL_LENSSHIFT_VER,"LENSSHIFT",DataLensShift,      NULL,       NULL,       NULL },
};

/**
 *  @brief register calibration site
 *
 *  register calibration site
 *
 *  @param [in]CalObj: calibration object
 *
 *  @return 0
 */
 int AppLib_CalibSiteRegister(Cal_Obj_s *CalObj)
{
    UINT32 Index;

    Index = CalObj->Id;
    CalObjTable[Index] = *CalObj;

    return 0;
}

/**
 *  @brief register ambarella calibration site
 *
 *  register ambarella calibration site
 *
 *
 *  @return 0
 */
int AppLib_CalibSiteRegisterAmbarella(void)
{
    UINT32 Index;
    UINT32 NumItem = sizeof(CalObjAmbarella) / sizeof(Cal_Obj_s);
    for (Index=0; Index<NumItem; Index++) {
        AppLib_CalibSiteRegister(&CalObjAmbarella[Index]);
    }
    return 0;
}


/**
 *  @brief initial calibration sites for stage 0
 *
 *  initial calibration sites for stage 0
 *
 */
void AppLib_CalibInitStage0(void)
{

    AppLib_CalibInitFunc(CAL_STATUS_ID,CALIB_LOAD,CALIB_FULL_LOAD);
    AppLib_CalibInitFunc(CAL_VIGNETTE_ID,CALIB_LOAD,CALIB_FULL_LOAD);
    AppLib_CalibInitFunc(CAL_WARP_ID,CALIB_LOAD,CALIB_FULL_LOAD);
    AppLib_CalibInitFunc(CAL_CA_ID,CALIB_LOAD,CALIB_FULL_LOAD);

}


/**
 *  @brief initial calibration sites for stage 1
 *
 *  initial calibration sites for stage 1
 *
 */
void AppLib_CalibInitStage1(void)
{
    AppLib_CalibInitFunc(CAL_VIGNETTE_ID,CALIB_INIT,0);
    AppLib_CalibInitFunc(CAL_WARP_ID,CALIB_INIT,0);
    AppLib_CalibInitFunc(CAL_CA_ID,CALIB_INIT,0);
}

/**
 *  @brief initial calibration sites for stage 2
 *
 *  initial calibration sites for stage 2
 *
 */
void AppLib_CalibInitStage2(void)
{
    AppLib_CalibInitFunc(CAL_BPC_ID,CALIB_LOAD,CALIB_FULL_LOAD);
    AppLib_CalibInitFunc(CAL_WB_ID,CALIB_LOAD,CALIB_FULL_LOAD);
    AppLib_CalibInitFunc(CAL_BLC_ID,CALIB_LOAD,CALIB_FULL_LOAD);

    AppLib_CalibInitFunc(CAL_WB_ID,CALIB_INIT,0);
    AppLib_CalibInitFunc(CAL_BLC_ID,CALIB_INIT,0);
    AppLib_CalibInitFunc(CAL_BPC_ID,CALIB_INIT,0);
}

/**
 *  @brief register calibration site
 *
 *  register calibration site
 *
 *  @return 0
 */
int AppLib_CalibSiteInit(void)
{
    // Register Ambarella's calibration sites
    AppLib_CalibSiteRegisterAmbarella();
    //initial Nand
    AppLib_CalibNandInit();

    return 0;
}
