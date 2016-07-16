/**
 * @file src/app/connected/applib/src/system/ApplibSys_Vin.c
 *
 *  Implementation of vin Utility interface.
 *
 * History:
 *    2013/08/14 - [Martin Lai] created file
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

#include <applib.h>
#include "../AppLibTask_Priority.h"

//#define DEBUG_APPLIB_SYS_VIN
#if defined(DEBUG_APPLIB_SYS_VIN)
#define DBGMSG AmbaPrint
#define DBGMSGc(x) AmbaPrintColor(GREEN,x)
#define DBGMSGc2 AmbaPrintColor
#else
#define DBGMSG(...)
#define DBGMSGc(...)
#define DBGMSGc2(...)
#endif
/*************************************************************************
 * Vin system declaration
 ************************************************************************/
static APPLIB_VIN_SETTING_s ApplibVinSetting = {0};

/*************************************************************************
 * Vin APIs
 ************************************************************************/
static UINT8 *VinMem;
static void *VinMemBufRaw;
AMP_VIN_HDLR_s *AppVinA = NULL;
static int ApplibVinInitFlag = -1;

/**
 *  @brief Initialize the vin module.
 *
 *  Initialize the vin module.
 *
 *  @return >=0 success, <0 failure
 */
int AppLibSysVin_Init(void)
{
    int ReturnValue = 0;
    /** Init VIN module */
    AMP_VIN_INIT_CFG_s VinInitCfg;
    if (ApplibVinInitFlag == 0) {
        return ReturnValue;
    }
    memset(&VinInitCfg, 0x0, sizeof(AMP_VIN_INIT_CFG_s));

    AmpVin_GetInitDefaultCfg(&VinInitCfg);
    ReturnValue = AmpUtil_GetAlignedPool(APPLIB_G_MMPL, (void **)&VinMem, &VinMemBufRaw, VinInitCfg.MemoryPoolSize, 32);
    if (ReturnValue != OK) {
        AmbaPrintColor(RED,"[Applib - Vin] <Init> Out of memory for vin!!");
        return ReturnValue;
    } else {
        DBGMSG("[Applib - Vin] <Init> ReturnValue = %d, dispInitCfg.MemoryPoolSize = 0x%X MemoryPoolAddr=0x%X,",ReturnValue, VINMEMMSIZE, VinMem);
    }
    VinInitCfg.MemoryPoolAddr = VinMem;

    ReturnValue = AmpVin_Init(&VinInitCfg);
    if (ReturnValue == OK) {
        ApplibVinInitFlag = 0;
    } else {
        AmbaPrintColor(RED,"[Applib - Vin] <Init> AmpVin_Init failure!!");
    }
    if (AppVinA == NULL) {
        AMP_VIN_HDLR_CFG_s VinCfg;
        AMP_VIN_LAYOUT_CFG_s Layout;
        AMBA_SENSOR_MODE_ID_u Mode = {0};
        memset(&Layout, 0x0, sizeof(AMP_VIN_LAYOUT_CFG_s));
        // Create VIN instance
        AmpVin_GetDefaultCfg(&VinCfg);
        VinCfg.Channel = AppEncChannel;
        VinCfg.Mode = Mode;
        VinCfg.LayoutNumber = 1;
        VinCfg.HwCaptureWindow.Width = 960;
        VinCfg.HwCaptureWindow.Height = 540;
        VinCfg.HwCaptureWindow.X = 0;
        VinCfg.HwCaptureWindow.Y = 0;
        Layout.Width = 960;
        Layout.Height = 540;
        Layout.EnableSourceArea = 0; // Get all capture window to main
        Layout.DzoomFactorX = 1<<16;
        Layout.DzoomFactorY = 1<<16;
        Layout.DzoomOffsetX = 0;
        Layout.DzoomOffsetY = 0;
        VinCfg.Layout = &Layout;
        VinCfg.cbEvent = NULL;
        VinCfg.cbSwitch = NULL;
        VinCfg.TaskInfo.Priority = APPLIB_VIN_HDLR_TASK_PRIORITY;
        DBGMSG("[Applib - Vin] <Init> Vin create %d %d %d", VinCfg.Mode, Layout.Width, Layout.Height);
        ReturnValue = AmpVin_Create(&VinCfg, &AppVinA);
        if (ReturnValue == OK) {
            ApplibVinInitFlag = 0;
        } else {
            AmbaPrintColor(RED,"[Applib - Vin] <Init> AmpVin_Create failure!!");
        }
    }
    return ReturnValue;
}

/**
 *  @brief Configure the vin module
 *
 *  Configure the vin module
 *
 *  @param [in] vinCfg The vin setting.
 *
 *  @return >=0 success, <0 failure
 */
int AppLibSysVin_Config(AMP_VIN_CFG_s *vinCfg)
{
    int ReturnValue = 0;

    if (ApplibVinInitFlag < 0) {
        AmbaPrintColor(RED,"[Applib - Vin] <Config> Non initial !!");
        return -1;
    }
    ReturnValue = AmpVin_ConfigHandler(AppVinA, vinCfg);
    if (ReturnValue != OK) {
        DBGMSGc2(RED,"[Applib - Vin] <Config> failure!!");
    }

    return ReturnValue;
}


/**
 *  @brief Set the vin system type NTSC/PAL.
 *
 *  Set the vout system type NTSC/PAL.
 *
 *  @param [in] vinSys system type
 *
 *  @return >=0 success, <0 failure
 */
int AppLibSysVin_SetSystemType(int vinSys)
{
    ApplibVinSetting.System = vinSys;
    return 0;
}

/**
 *  @brief Get the vout system type.
 *
 *  Get the vin system type.
 *
 *  @return The vin system type.
 */
int AppLibSysVin_GetSystemType(void)
{
    return ApplibVinSetting.System;
}

/**
 *  @brief Set the vin dimension
 *
 *  Set the vin dimension
 *
 *  @param [in] vinDimension Vin dimension
 *
 *  @return >=0 success, <0 failure
 */
int AppLibSysVin_SetDimension(int vinDimension)
{
    ApplibVinSetting.Dimension = vinDimension;
    return 0;
}

/**
 *  @brief Get the vin dimension
 *
 *  Get the vin dimension
 *
 *  @return The vin dimension
 */
int AppLibSysVin_GetDimension(void)
{
    return ApplibVinSetting.Dimension;
}

/**
 *  @brief Get the vin source type.
 *
 *  Get the vin source type.
 *
 *  @param [in] vinSrc The vin source type
 *
 *  @return >=0 success, <0 failure
 */
int AppLibSysVin_SetSourceType(int vinSrc)
{
    ApplibVinSetting.Source = vinSrc;
    return 0;
}

/**
 *  @brief Get the vin source type.
 *
 *  Get the vin source type.
 *
 *  @return The vin source type.
 */
int AppLibSysVin_GetSourceType(void)
{
    return ApplibVinSetting.Source;
}

/**
 *  @brief Get the total setting of vin.
 *
 *  Get the total setting of vin.
 *
 *  @param [out] setting The vin setting
 *
 *  @return >=0 success, <0 failure
 */
int AppLibSysVin_GetSetting(APPLIB_VIN_SETTING_s *setting)
{
    memcpy(setting, &ApplibVinSetting, sizeof(APPLIB_VIN_SETTING_s));
    return 0;
}

