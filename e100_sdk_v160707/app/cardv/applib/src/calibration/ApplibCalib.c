/**
 * @file src/app/connected/applib/src/calibration/AppLib_Calib.c
 *
 * Unit test function for calibration
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
#include <string.h>
#include <math.h>
#include <AmbaPrintk.h>
#include <AmbaSensor.h>
#include <system/ApplibSys_Sensor.h>
#include <calibration/ApplibCalibMgr.h>
#include <calibration/ApplibCalibCli.h>
#include <calibration/bpc/ApplibCalibBpc.h>
#include <AmbaShell.h>
#include <AmbaTest.h>




extern int AppLib_CalibSiteInit(void);
extern int AppLibVignette_UTFunc(int Argc, char *Argv[]);
extern int AppLibCalibBPC_UTFunc(int Argc, char *Argv[]);
extern int AppLibBLC_UTFunc(int Argc, char *Argv[]);
extern int AppLibWB_UTFunc(int Argc, char *Argv[]);
extern int AppLibWarp_UTFunc(int Argc, char *Argv[]);
extern int AppLibCA_UTFunc(int Argc, char *Argv[]);
extern int AppLibCalibBPC_Init(Cal_Obj_s *CalObj);



/**
 *  @brief test cmd to do calibration
 *
 *  test cmd to do calibration
 *
 *  @param [in] env environment
 *  @param [in] argc the number of the input parameter
 *  @param [in] argv value of input parameter
 *
 *  @return >=0 success, <0 failure
 */
int AppLib_CalibMgr(AMBA_SHELL_ENV_s  *env, int argc, char *argv[])
{
    int rval = -1;
    int msg_available;
    char output_str[256];

    if ((strcmp(argv[2], "cmd") == 0)) {
        // Compose command string
        rval = AppLib_CalibCli(argc-3, argv+3, output_str, &msg_available);
        if (msg_available) {
            AmbaPrint("Output string: %s", output_str);
        }
    } else if ((strcmp(argv[2], "path") == 0)) {
        rval = AppLib_CalibPathIf(argv[3]);
    }
    if (rval == -1) {
        AmbaPrint("t cal -mgr cmd [cali_cmd]: calibration cmd i/f (arg num<15");
        AmbaPrint("t cal -mgr path : specify calibation script path");
    }
    return rval;
}

/**
 *  @brief test cmd to do calibration
 *
 *  print the useage for calibration unit test
 *
 */
void AppLib_CalibUseage(void)
{

    AmbaPrint(" calib test command:");
    AmbaPrint(" site_init : calibration site init");
    AmbaPrint(" mgr : for doing calibration ");
    AmbaPrint(" bpc : Amba bad pixel calibration");
    AmbaPrint(" vnc : Amba vignette calibration");
    AmbaPrint(" blc : Amba black level calibration");
    AmbaPrint(" wb : Amba black level calibration");
    AmbaPrint(" warp : warp calibration");
}

/**
 *  @brief entry function for calibration unit test
 *
 *  entry function for calibration unit test
 *
 *  @param [in] env environment
 *  @param [in] argc the number of the input parameter
 *  @param [in] argv value of input parameter
 *
 *  @return 0
 */
int AppLib_CalibTest(AMBA_SHELL_ENV_s  *env, int argc, char *argv[])
{
    AmbaPrint("AppLib_CalibTest cmd: %s", argv[1]);
    if (strcmp(argv[1],"mgr") == 0) {
        AppLib_CalibMgr(env, argc, argv);
    } else if (strcmp(argv[1],"site_init") == 0) {
        AppLib_CalibSiteInit();
    } else if (strcmp(argv[1],"bpc") == 0) {
        AppLibCalibBPC_UTFunc(argc, argv);
    } else if (strcmp(argv[1],"vnc") == 0) {
        AppLibVignette_UTFunc(argc, argv);
    } else if (strcmp(argv[1],"blc") == 0) {
        AppLibBLC_UTFunc(argc, argv);
    } else if (strcmp(argv[1],"wb") == 0) {
        AppLibWB_UTFunc(argc, argv);
    } else if (strcmp(argv[1],"warp") == 0) {
        AppLibWarp_UTFunc(argc, argv);
    } else if (strcmp(argv[1],"ca") == 0) {
        AppLibCA_UTFunc(argc, argv);
    } else {
        AppLib_CalibUseage();
    }
    return 0;
}

/**
 *  @brief register the unit test function for calibration
 *
 *  register the unit test function for calibration
 *
 *  @return 0
 */
int AppLib_CalibModuleInit(void)
{
    // hook command
    AmbaTest_RegisterCommand("cal", AppLib_CalibTest);

    return AMP_OK;
}

/**
 *  @brief Load the calibration data
 *
 *  Load the calibration data
 *
 *  @return 0
 */
int AppLibCalib_Load(void)
{
    AppLib_CalibSiteInit();
#if 1    
    AppLib_CalibInitFunc(CAL_STATUS_ID,CALIB_LOAD,CALIB_FULL_LOAD);

    AppLib_CalibInitFunc(CAL_VIGNETTE_ID,CALIB_LOAD,CALIB_FULL_LOAD);
    AppLib_CalibInitFunc(CAL_WARP_ID,CALIB_LOAD,CALIB_FULL_LOAD);
    AppLib_CalibInitFunc(CAL_BPC_ID,CALIB_LOAD,CALIB_FULL_LOAD);
    AppLib_CalibInitFunc(CAL_WB_ID,CALIB_LOAD,CALIB_FULL_LOAD);
    AppLib_CalibInitFunc(CAL_BLC_ID,CALIB_LOAD,CALIB_FULL_LOAD);
    AppLib_CalibInitFunc(CAL_CA_ID,CALIB_LOAD,CALIB_FULL_LOAD);
    AppLib_CalibInitFunc(CAL_GYRO_ID,CALIB_LOAD,CALIB_FULL_LOAD);
    
#else    
    AppLib_CalibInit(CAL_STATUS_ID,CALIB_LOAD);

    AppLib_CalibInit(CAL_VIGNETTE_ID,CALIB_LOAD);
    AppLib_CalibInit(CAL_WARP_ID,CALIB_LOAD);
    AppLib_CalibInit(CAL_BPC_ID,CALIB_LOAD);
    AppLib_CalibInit(CAL_WB_ID,CALIB_LOAD);
    AppLib_CalibInit(CAL_BLC_ID,CALIB_LOAD);
    AppLib_CalibInit(CAL_CA_ID,CALIB_LOAD);
#endif
    return 0;
}

/**
 *  @brief initialize the App calibration
 *
 *  initialize the App calibration
 *
 *  @return 0
 */
int AppLibCalib_Init(void)
{
#if 1
    AppLib_CalibInitFunc(CAL_VIGNETTE_ID,CALIB_INIT,0);
    AppLib_CalibInitFunc(CAL_WARP_ID,CALIB_INIT,0);
    AppLib_CalibInitFunc(CAL_BPC_ID,CALIB_INIT,0);
    AppLib_CalibInitFunc(CAL_WB_ID,CALIB_INIT,0);
    AppLib_CalibInitFunc(CAL_BLC_ID,CALIB_INIT,0);
    AppLib_CalibInitFunc(CAL_CA_ID,CALIB_INIT,0);
    AppLib_CalibInitFunc(CAL_GYRO_ID,CALIB_INIT,0);
#else
    AppLib_CalibInit(CAL_VIGNETTE_ID,CALIB_INIT);
    AppLib_CalibInit(CAL_WARP_ID,CALIB_INIT);
    AppLib_CalibInit(CAL_BPC_ID,CALIB_INIT);
    AppLib_CalibInit(CAL_WB_ID,CALIB_INIT);
    AppLib_CalibInit(CAL_BLC_ID,CALIB_INIT);
    AppLib_CalibInit(CAL_CA_ID,CALIB_INIT);
#endif
    return 0;
}

