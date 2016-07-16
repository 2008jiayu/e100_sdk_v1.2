/**
 * @file src/app/connected/applib/src/display/ApplibDisplay.c
 *
 * Implementation of display Utilities
 *
 * History:
 *    2013/07/17 - [Martin Lai] created file
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

#include <display/Display.h>
#include <system/ApplibSys_Lcd.h>
#include <applib.h>
#include <AmbaHDMI.h>
//#define DEBUG_APPLIB_DISPLAY
#if defined(DEBUG_APPLIB_DISPLAY)
#define DBGMSG AmbaPrint
#define DBGMSGc(x) AmbaPrintColor(GREEN,x)
#define DBGMSGc2 AmbaPrintColor
#else
#define DBGMSG(...)
#define DBGMSGc(...)
#define DBGMSGc2(...)
#endif


/*************************************************************************
 * Display variables definitions
 ************************************************************************/
typedef struct _APPLIB_DISP_CHAN_s_ {
    int Id;
    UINT8 Flags;
#define DISP_CH_FLAGS_ENABLE    (0x01)
    UINT8 LcdBootReprogramCounter;
#define DISP_FCHAN_LCD_BOOT_REPROGRAM_CNT   (1)
    /* The time about reprogramming the LCD depends on different devices. The setting of LCD could be wrong if the waiting time is too short. */
#define DISP_DCHAN_LCD_BOOT_REPROGRAM_CNT   (1)
    UINT8 TVBootReprogramCounter;
#define DISP_FCHAN_TV_BOOT_CNT (1)
    UINT8 ThreeDMode;
    UINT32 SrcColor;
    UINT32 DispColor;
    AMP_DISP_DEV_CFG_s Config;
} APPLIB_DISP_CHAN_s;

static APPLIB_DISP_CHAN_s ApplibDispFchan = {
    -1, 0, 1, 1,
    0,
    DISP_COLOR_AUTO, DISP_COLOR_AUTO,
    {AMP_DISP_CHANNEL_FCHAN, AMP_DSIP_NONE, 0xFFFF, VAR_ANY}
};

static APPLIB_DISP_CHAN_s ApplibDispDchan = {
    -1, 0, 1, 1,
    0,
    DISP_COLOR_AUTO, DISP_COLOR_AUTO,
    {AMP_DISP_CHANNEL_DCHAN, AMP_DSIP_NONE, 0xFFFF, VAR_ANY}
};

#ifdef CONFIG_APP_ARD
static AMP_ROTATION_e ApplibDispDchanRotate = AMP_ROTATE_0;
static AMP_ROTATION_e ApplibDispFchanRotate = AMP_ROTATE_0;
#endif

#define MAX_ENC_DEC_WINDOW_NUM  (9)
static int FchanWindowId[MAX_ENC_DEC_WINDOW_NUM] = {0};
static int DchanWindowId[MAX_ENC_DEC_WINDOW_NUM] = {0};

#define MAX_APP_FCHAN_OSD_WINDOW_NUM (1)
#define MAX_APP_DCHAN_OSD_WINDOW_NUM (1)

#define MAX_APP_FCHAN_WINDOW_NUM (MAX_ENC_DEC_WINDOW_NUM + MAX_APP_FCHAN_OSD_WINDOW_NUM)
#define MAX_APP_DCHAN_WINDOW_NUM (MAX_ENC_DEC_WINDOW_NUM + MAX_APP_DCHAN_OSD_WINDOW_NUM)
static AMP_DISP_WINDOW_HDLR_s *WindowFchanHdlr[MAX_APP_FCHAN_WINDOW_NUM] = {NULL};
static AMP_DISP_WINDOW_HDLR_s *WindowDchanHdlr[MAX_APP_DCHAN_WINDOW_NUM] = {NULL};


/*************************************************************************
 * Display APIs - Static
 ************************************************************************/

// Display handler
AMP_DISP_HDLR_s *ApplibDispDchanHdlr = NULL;
AMP_DISP_HDLR_s *ApplibDispFchanHdlr = NULL;

/*************************************************************************
 * Display APIs
 ************************************************************************/
#ifdef CONFIG_APP_ARD
int AppLibDisp_Rotate(UINT32 dispChanID,AMP_ROTATION_e rotate)
{
        if (APPLIB_CHECKFLAGS(dispChanID, DISP_CH_FCHAN)){
            AmbaPrintColor(RED,"[AppLib - Display] <Rotate> on fchan -1(Not support)");
            //ApplibDispFchanRotate = rotate;
            return -1;
        }
        if (APPLIB_CHECKFLAGS(dispChanID, DISP_CH_DCHAN)){
            AmbaPrintColor(GREEN,"[AppLib - Display] <Rotate> on dchan: %d", rotate);
            ApplibDispDchanRotate = rotate;
            return 0;
        }
        return -1;
}

AMP_ROTATION_e AppLibDisp_GetRotate(UINT32 dispChanID)
{
        if (APPLIB_CHECKFLAGS(dispChanID, DISP_CH_FCHAN)){
            DBGMSG("[AppLib - Display] <get> rotate on fchan: %d", ApplibDispFchanRotate);
            return ApplibDispFchanRotate;
        }
        if (APPLIB_CHECKFLAGS(dispChanID, DISP_CH_DCHAN)){
            DBGMSG("[AppLib - Display] <get> rotate on dchan: %d", ApplibDispDchanRotate);
            return ApplibDispDchanRotate;
        }
        return AMP_ROTATE_0;
}
#endif

/**
 *  @brief Initialize the display.
 *
 *  Initialize the display.
 *
 *  @return >=0 success, <0 failure
 */
int AppLibDisp_Init(void)
{
    int ReturnValue = 0;
    AMP_DISP_INIT_CFG_s DispInitCfg = {0};
    AMP_DISP_CFG_s DispCfg;
    static void *DisphdlrAddrBufRaw = NULL;

    DBGMSGc2(BLUE, "[AppLib - Display] <Init> start");
    /* Display Init */
    AmpDisplay_GetDefaultInitCfg(&DispInitCfg);

    DBGMSGc2(GREEN, "[AppLib - Display] <Init> AmpDisplay_Init Get MaxDeviceInterHdlr = %d", DispInitCfg.MaxDeviceInterHdlr);
    DBGMSGc2(GREEN, "[AppLib - Display] <Init> AmpDisplay_Init Get MaxWindowInterHdlr = %d", DispInitCfg.MaxWindowInterHdlr);
    DispInitCfg.MemoryPoolSize = DispInitCfg.MemoryPoolSize;
    ReturnValue = AmpUtil_GetAlignedPool(APPLIB_G_MMPL, (void **)&DispInitCfg.MemoryPoolAddr, &DisphdlrAddrBufRaw, DispInitCfg.MemoryPoolSize, 32);
    if (ReturnValue < 0) {
        AmbaPrintColor(RED, "[AppLib - Display] <Init> AmpDisplay_Init Get memory Fail: ReturnValue = %d", ReturnValue);
        return ReturnValue;
    }
    ReturnValue = AmpDisplay_Init(&DispInitCfg);
    if (ReturnValue < 0) {
        AmbaPrintColor(RED, "[AppLib - Display] <Init> AmpDisplay_Init Fail: ReturnValue = %d", ReturnValue);
        AmbaKAL_BytePoolFree(DisphdlrAddrBufRaw);
        return ReturnValue;
    }

    /* Create DCHAN display handler */
    AmpDisplay_GetDefaultCfg(&DispCfg);
    DispCfg.Device.Channel = AMP_DISP_CHANNEL_DCHAN;
    DispCfg.Device.DeviceId = AMP_DISP_LCD;
    DispCfg.Device.DeviceMode = 0xFFFF;
    DispCfg.MaxNumWindow = MAX_APP_DCHAN_WINDOW_NUM;
    DispCfg.ScreenRotate = 0;
    DispCfg.SystemType = AMP_DISP_NTSC;
#ifdef CONFIG_APP_ARD
    DispCfg.Device.CustomCfg.EnCustomCfg = 1;
    DispCfg.Device.CustomCfg.Cfg.LCD.BackLight = AMP_DISP_LCD_BACKLIGHT_IGNORE;
#endif
    AmpDisplay_Create(&DispCfg, &ApplibDispDchanHdlr);

    /* Create FCHAN display handler */
    AmpDisplay_GetDefaultCfg(&DispCfg);
    DispCfg.Device.Channel = AMP_DISP_CHANNEL_FCHAN;
    DispCfg.Device.DeviceId = AMP_DISP_HDMI;
    DispCfg.Device.DeviceMode = AMP_DISP_ID_1080P;
    DispCfg.MaxNumWindow = MAX_APP_FCHAN_WINDOW_NUM;
    DispCfg.ScreenRotate = 0;
    DispCfg.SystemType = AMP_DISP_NTSC;
    AmpDisplay_Create(&DispCfg, &ApplibDispFchanHdlr);

    ApplibDispFchan.Id = AMP_DISP_CHANNEL_FCHAN;
    ApplibDispDchan.Id = AMP_DISP_CHANNEL_DCHAN;

    DBGMSGc2(BLUE, "[AppLib - Display] <Init> end: ReturnValue = %d", ReturnValue);

    return ReturnValue;
}

/**
 *  @brief Enable the flag of DChan or FChan.
 *
 *  Enable the flag of DChan or FChan.
 *
 *  @param [in] dispChanID Channel ID.
 *
 *  @return >=0 success, <0 failure
 */
int AppLibDisp_EnableChan(UINT32 dispChanID)
{
    if (APPLIB_CHECKFLAGS(dispChanID, DISP_CH_FCHAN) && (ApplibDispFchan.Id != -1)) {
        APPLIB_ADDFLAGS(ApplibDispFchan.Flags, DISP_CH_FLAGS_ENABLE);
    }
    if (APPLIB_CHECKFLAGS(dispChanID, DISP_CH_DCHAN) && (ApplibDispDchan.Id != -1)) {
        APPLIB_ADDFLAGS(ApplibDispDchan.Flags, DISP_CH_FLAGS_ENABLE);
    }
    return 0;
}

/**
 *  @brief Disable the flag of DChan or FChan.
 *
 *  Disable the flag of DChan or FChan.
 *
 *  @param [in] dispChanID Channel ID
 *
 *  @return >=0 success, <0 failure
 */
int AppLibDisp_DisableChan(UINT32 dispChanID)
{
    if (APPLIB_CHECKFLAGS(dispChanID, DISP_CH_FCHAN)) {
        APPLIB_REMOVEFLAGS(ApplibDispFchan.Flags, DISP_CH_FLAGS_ENABLE);
    }
    if (APPLIB_CHECKFLAGS(dispChanID, DISP_CH_DCHAN)) {
        APPLIB_REMOVEFLAGS(ApplibDispDchan.Flags, DISP_CH_FLAGS_ENABLE);
    }
    return 0;
}

/**
 *  @brief Check the flag of Dchan or Fchan.
 *
 *  Check the flag of Dchan or Fchan.
 *
 *  @param [in] dispChanID Channel ID
 *
 *  @return 1 enable, 0 disable
 */
int AppLibDisp_CheckChanEnabled(UINT32 dispChanID)
{
    if (APPLIB_CHECKFLAGS(dispChanID, DISP_CH_FCHAN) &&
        APPLIB_CHECKFLAGS(ApplibDispFchan.Flags, DISP_CH_FLAGS_ENABLE)) {
        return 1;
    } else if (APPLIB_CHECKFLAGS(dispChanID, DISP_CH_DCHAN) &&
        APPLIB_CHECKFLAGS(ApplibDispDchan.Flags, DISP_CH_FLAGS_ENABLE)) {
        return 1;
    } else {
        return 0;
    }
}

/**
 *  @brief Get the MW channel id.
 *
 *  Get the MW channel id.
 *
 *  @param [in] dispChanID Channel ID
 *
 *  @return >=0 MW channel id, <0 failure
 */
int AppLibDisp_GetChanID(UINT32 dispChanID)
{
    if (dispChanID == DISP_CH_FCHAN) {
        return ApplibDispFchan.Id;
    } else if (dispChanID == DISP_CH_DCHAN) {
        return ApplibDispDchan.Id;
    } else {
        AmbaPrintColor(RED, "[AppLib - Display] <GetChanID> Don't support DISP_CH_DUAL");
        return -1;
    }
}

/**
 *  @brief Get the information of display configuration
 *
 *  Get the information of display configuration
 *
 *  @param [in] dispChanID Channel ID
 *  @param [out] config The configuration
 *
 *  @return >=0 success, <0 failure
 */
int AppLibDisp_GetChanConfig(UINT32 dispChanID, AMP_DISP_DEV_CFG_s *config)
{
    if (dispChanID == DISP_CH_FCHAN) {
        memcpy(config, &ApplibDispFchan.Config, sizeof(AMP_DISP_DEV_CFG_s));
    } else if (dispChanID == DISP_CH_DCHAN) {
        memcpy(config, &ApplibDispDchan.Config, sizeof(AMP_DISP_DEV_CFG_s));
    } else {
        AmbaPrintColor(RED, "[AppLib - Display] <GetChanConfig> Don't support DISP_CH_DUAL");
        memset(config, 0, sizeof(AMP_DISP_DEV_CFG_s));
        return -1;
    }
    return 0;
}

/**
 *  @brief Get device ID
 *
 *  Get device ID
 *
 *  @param [in] dispChanID Channel ID
 *
 *  @return The device ID
 */
int AppLibDisp_GetDeviceID(UINT32 dispChanID)
{
    if (dispChanID == DISP_CH_FCHAN) {
        return ApplibDispFchan.Config.DeviceId;
    } else if (dispChanID == DISP_CH_DCHAN) {
        return ApplibDispDchan.Config.DeviceId;
    } else {
        AmbaPrintColor(RED, "[AppLib - Display] <GetDeviceID> Don't support DISP_CH_DUAL");
        return AMP_DSIP_NONE;
    }
}

/**
 *  @brief Get display mode
 *
 *  Get display mode
 *
 *  @param [in] dispChanID Channel ID
 *
 *  @return The display mode, 0xFFFF is error.
 */
int AppLibDisp_GetDispMode(UINT32 dispChanID)
{
    if (dispChanID == DISP_CH_FCHAN) {
        return ApplibDispFchan.Config.DeviceMode;
    } else if (dispChanID == DISP_CH_DCHAN) {
        return ApplibDispDchan.Config.DeviceMode;
    } else {
        AmbaPrintColor(RED, "[AppLib - Display] <GetDispMode> Don't support DISP_CH_DUAL");
        return 0xFFFF;
    }
}

/**
 *  @brief Set color mapping.
 *
 *  Set color mapping.
 *
 *  @param [in] dispChanID Channel ID
 *  @param [in] SrcColor Source color type
 *  @param [in] DispColor Display color type
 *
 *  @return >=0 success, <0 failure
 */
int AppLibDisp_SetColorMapping(UINT32 dispChanID, UINT32 srcColor, UINT32 dispColor)
{
    if (APPLIB_CHECKFLAGS(dispChanID, DISP_CH_FCHAN)) {
        ApplibDispFchan.SrcColor = srcColor;
        ApplibDispFchan.DispColor = dispColor;
    }
    if (APPLIB_CHECKFLAGS(dispChanID, DISP_CH_DCHAN)) {
        ApplibDispDchan.SrcColor = srcColor;
        ApplibDispDchan.DispColor = dispColor;
    }
    return 0;
}

/**
 *  @brief Get color mapping.
 *
 *  Get color mapping.
 *
 *  @param [in] dispChanID Channel ID
 *  @param [out] srcColor source color type
 *  @param [out] dispColor display color type
 *
 *  @return >=0 success, <0 failure
 */
int AppLibDisp_GetColorMapping(UINT32 dispChanID, UINT32 *srcColor, UINT32 *dispColor)
{
    if (dispChanID == DISP_CH_FCHAN) {
        *srcColor = ApplibDispFchan.SrcColor;
        *dispColor = ApplibDispFchan.DispColor;
    } else if (dispChanID == DISP_CH_DCHAN) {
        *srcColor = ApplibDispDchan.SrcColor;
        *dispColor = ApplibDispDchan.DispColor;
    } else {
        AmbaPrintColor(RED, "[AppLib - Display] <GetColorMapping> Don't support DISP_CH_DUAL");
        return -1;
    }
    return 0;
}

/**
 *  @brief Set 3D output mode.
 *
 *  Set 3D output mode.
 *
 *  @param [in] dispChanID Channel ID
 *  @param [in] mode 3D output mode
 *
 *  @return >=0 success, <0 failure
 */
int AppLibDisp_Set3DMode(UINT32 dispChanID, UINT32 mode)
    {
    if (dispChanID == DISP_CH_FCHAN) {
        ApplibDispFchan.ThreeDMode = mode;
    } else if (dispChanID == DISP_CH_DCHAN) {
        ApplibDispDchan.ThreeDMode = mode;
    } else {
        AmbaPrintColor(RED, "[AppLib - Display] <Set3DMode> Don't support DISP_CH_DUAL");
        return -1;
    }
    return 0;
}

/**
 *  @brief Get 3D output mode.
 *
 *  Get 3D output mode.
 *
 *  @param [in] dispChanID Channel ID
 *
 *  @return 3D output mode.
 */
int AppLibDisp_Get3DMode(UINT32 dispChanID)
{
    if (dispChanID == DISP_CH_FCHAN) {
        return ApplibDispFchan.ThreeDMode;
    } else if (dispChanID == DISP_CH_DCHAN) {
        return ApplibDispDchan.ThreeDMode;
    } else {
        AmbaPrintColor(RED, "[AppLib - Display] <Get3DMode> Don't support DISP_CH_DUAL");
        return -1;
    }
}

/**
 *  @brief Set the display dimension
 *
 *  Set the display dimension
 *
 *  @param [in] dispChanID Channel ID
 *  @param [in] outputDimension Output dimension.
 *
 *  @return >=0 success, <0 failure
 */
int AppLibDisp_SetDispDimension(UINT32 dispChanID, UINT8 outputDimension)
{
    int ReturnValue = 0;

    return ReturnValue;
}

/**
 *  @brief Check the device type of FChan.
 *
 *  Check the device type of FChan.
 *
 *  @param [in] dispDevId Display device id.
 *
 *  @return >=0 success, <0 failure
 */
static int AppLibDisp_SelectDeviceFchan(UINT32 dispDevId)
{
    int ReturnValue = 0;
    int CurDevId = 0;
    AMBA_HDMI_SINK_INFO_s SinkInfo;
    CurDevId = ApplibDispFchan.Config.DeviceId;

    if (APPLIB_CHECKFLAGS(ApplibDispFchan.Flags, DISP_CH_FLAGS_ENABLE)) {
        switch (dispDevId) {
        case AMP_DISP_HDMI:
            if (AppLibSysVout_CheckJackHDMI() && (AmbaHDMI_GetSinkInfo(&SinkInfo) >= 0)) {
                ApplibDispFchan.Config.DeviceId = AMP_DISP_HDMI;
            } else {
                ApplibDispFchan.Config.DeviceId = AMP_DSIP_NONE;
            }
            break;
        case AMP_DISP_CVBS:
            if (AppLibSysVout_CheckJackCs()) {
                ApplibDispFchan.Config.DeviceId = AMP_DISP_CVBS;
            } else {
                ApplibDispFchan.Config.DeviceId = AMP_DSIP_NONE;
            }
            break;
        case AMP_DISP_LCD:
            if (AppLibSysLcd_CheckEnabled(LCD_CH_FCHAN)) {
                ApplibDispFchan.Config.DeviceId = AMP_DISP_LCD;
            } else {
                ApplibDispFchan.Config.DeviceId = AMP_DSIP_NONE;
            }
            break;
        case DISP_ANY_DEV:
            if (AppLibSysVout_CheckJackHDMI() && (AmbaHDMI_GetSinkInfo(&SinkInfo) >= 0)) {
                ApplibDispFchan.Config.DeviceId = AMP_DISP_HDMI;
            } else if (AppLibSysVout_CheckJackCs()) {
                ApplibDispFchan.Config.DeviceId = AMP_DISP_CVBS;
            } else if (AppLibSysLcd_CheckEnabled(LCD_CH_FCHAN)) {
                ApplibDispFchan.Config.DeviceId = AMP_DISP_LCD;
            } else {
                ApplibDispFchan.Config.DeviceId = AMP_DSIP_NONE;
            }
            break;
        default:
            ApplibDispFchan.Config.DeviceId = AMP_DSIP_NONE;
            break;
        }
    } else {
        ApplibDispFchan.Config.DeviceId = AMP_DSIP_NONE;
    }

    if (ApplibDispFchan.Config.DeviceId == CurDevId) {
        ReturnValue += DISP_FCHAN_NO_CHANGE;
    }
    if (ApplibDispFchan.Config.DeviceId == AMP_DSIP_NONE) {
        if (ApplibDispFchan.Id != -1) {
            //DBGMSG("[AppLib - Display] <SelectDeviceFchan> No fchan device selected. Shut down fchan");
        }
        ReturnValue += DISP_FCHAN_NO_DEVICE;
    }

    return ReturnValue;
}

/**
 *  @brief Check the device type of DChan.
 *
 *  Check the device type of DChan.
 *
 *  @param [in] dispDevId Display device id.
 *
 *  @return >=0 success, <0 failure
 */
static int AppLibDisp_SelectDeviceDchan(UINT32 dispDevId)
{
    int ReturnValue = 0;
    int CurDevId = 0;

    CurDevId = ApplibDispDchan.Config.DeviceId;

    if (APPLIB_CHECKFLAGS(ApplibDispDchan.Flags, DISP_CH_FLAGS_ENABLE)) {
        if (AppLibSysLcd_CheckEnabled(LCD_CH_DCHAN)) {
            ApplibDispDchan.Config.DeviceId = AMP_DISP_LCD;
        } else {
            ApplibDispDchan.Config.DeviceId = AMP_DSIP_NONE;
        }
    } else {
        ApplibDispDchan.Config.DeviceId = AMP_DSIP_NONE;
    }

    if (ApplibDispDchan.Config.DeviceId == CurDevId) {
        ReturnValue += DISP_DCHAN_NO_CHANGE;
    }
    if (ApplibDispDchan.Config.DeviceId == AMP_DSIP_NONE) {
        if (ApplibDispDchan.Id != -1) {
            //DBGMSG("[AppLib - Display] <SelectDeviceDchan> No dchan device selected. Shut down dchan");
        }
        ReturnValue += DISP_DCHAN_NO_DEVICE;
    }

    return ReturnValue;
}

/**
 *  @brief Check the device type
 *
 *  Check the device type
 *
 *  @param [in] dispChanID Channel ID
 *  @param [in] dispDevId Display device id.
 *
 *  @return >=0 success, <0 failure
 */
int AppLibDisp_SelectDevice(UINT32 dispChanID, UINT32 dispDevId)
{
    int ReturnValue = 0;
    if (APPLIB_CHECKFLAGS(dispChanID, DISP_CH_FCHAN)) {
        ReturnValue += AppLibDisp_SelectDeviceFchan(dispDevId);
    }
    if (APPLIB_CHECKFLAGS(dispChanID, DISP_CH_DCHAN)) {
        ReturnValue += AppLibDisp_SelectDeviceDchan(dispDevId);
    }
    return ReturnValue;
}

/**
 *  @brief Config the display mode of fchan.
 *
 *  Config the display mode of fchan.
 *
 *  @param [in] voutDispMode display mode.
 *
 *  @return >=0 success, <0 failure
 */
static int AppLibDisp_ConfigModeFchan(int voutDispMode)
{
    int ReturnValue = 0;
    int VoutDispAr = VAR_ANY;
    AMBA_HDMI_SINK_INFO_s SinkInfo;

    switch (ApplibDispFchan.Config.DeviceId) {
    case AMP_DISP_HDMI:
        ReturnValue = AmbaHDMI_GetSinkInfo(&SinkInfo);

        AmbaPrintColor(GREEN,"=================== NTSC =======================");
        AmbaPrintColor(GREEN,"[Applib - HDMI Monitor] AMP_DISP_ID_2160P30\t= %d",SinkInfo.pVideoInfo[AMP_DISP_ID_2160P30]);
        AmbaPrintColor(GREEN,"[Applib - HDMI Monitor] AMP_DISP_ID_2160P24\t= %d",SinkInfo.pVideoInfo[AMP_DISP_ID_2160P24]);
        AmbaPrintColor(GREEN,"[Applib - HDMI Monitor] AMP_DISP_ID_1080P\t= %d",SinkInfo.pVideoInfo[AMP_DISP_ID_1080P]);
        AmbaPrintColor(GREEN,"[Applib - HDMI Monitor] AMP_DISP_ID_1080P30\t= %d",SinkInfo.pVideoInfo[AMP_DISP_ID_1080P30]);
        AmbaPrintColor(GREEN,"[Applib - HDMI Monitor] AMP_DISP_ID_1080I\t= %d",SinkInfo.pVideoInfo[AMP_DISP_ID_1080I]);
        AmbaPrintColor(GREEN,"[Applib - HDMI Monitor] AMP_DISP_ID_1080P24\t= %d",SinkInfo.pVideoInfo[AMP_DISP_ID_1080P24]);
        AmbaPrintColor(GREEN,"[Applib - HDMI Monitor] AMP_DISP_ID_720P\t= %d",SinkInfo.pVideoInfo[AMP_DISP_ID_720P]);
        AmbaPrintColor(GREEN,"[Applib - HDMI Monitor] AMP_DISP_ID_720P30\t= %d",SinkInfo.pVideoInfo[AMP_DISP_ID_720P30]);
        AmbaPrintColor(GREEN,"[Applib - HDMI Monitor] AMP_DISP_ID_720P24\t= %d",SinkInfo.pVideoInfo[AMP_DISP_ID_720P24]);
        AmbaPrintColor(GREEN,"[Applib - HDMI Monitor] AMP_DISP_ID_480P\t= %d",SinkInfo.pVideoInfo[AMP_DISP_ID_480P]);
        AmbaPrintColor(GREEN,"[Applib - HDMI Monitor] AMP_DISP_ID_480I\t= %d",SinkInfo.pVideoInfo[AMP_DISP_ID_480I]);
        AmbaPrintColor(GREEN,"[Applib - HDMI Monitor] AMP_DISP_ID_DMT0659\t= %d",SinkInfo.pVideoInfo[AMP_DISP_ID_DMT0659]);

        AmbaPrintColor(GREEN,"==================== PAL ======================");
        AmbaPrintColor(GREEN,"[Applib - HDMI Monitor] AMP_DISP_ID_2160P25\t= %d",SinkInfo.pVideoInfo[AMP_DISP_ID_2160P25]);
        AmbaPrintColor(GREEN,"[Applib - HDMI Monitor] AMP_DISP_ID_2160P24\t= %d",SinkInfo.pVideoInfo[AMP_DISP_ID_2160P24]);
        AmbaPrintColor(GREEN,"[Applib - HDMI Monitor] AMP_DISP_ID_1080P50\t= %d",SinkInfo.pVideoInfo[AMP_DISP_ID_1080P50]);
        AmbaPrintColor(GREEN,"[Applib - HDMI Monitor] AMP_DISP_ID_1080P25\t= %d",SinkInfo.pVideoInfo[AMP_DISP_ID_1080P25]);
        AmbaPrintColor(GREEN,"[Applib - HDMI Monitor] AMP_DISP_ID_1080I\t= %d",SinkInfo.pVideoInfo[AMP_DISP_ID_1080I25]);
        AmbaPrintColor(GREEN,"[Applib - HDMI Monitor] AMP_DISP_ID_1080P24\t= %d",SinkInfo.pVideoInfo[AMP_DISP_ID_1080P24]);
        AmbaPrintColor(GREEN,"[Applib - HDMI Monitor] AMP_DISP_ID_720P50\t= %d",SinkInfo.pVideoInfo[AMP_DISP_ID_720P50]);
        AmbaPrintColor(GREEN,"[Applib - HDMI Monitor] AMP_DISP_ID_720P25\t= %d",SinkInfo.pVideoInfo[AMP_DISP_ID_720P25]);
        AmbaPrintColor(GREEN,"[Applib - HDMI Monitor] AMP_DISP_ID_720P24\t= %d",SinkInfo.pVideoInfo[AMP_DISP_ID_720P24]);
        AmbaPrintColor(GREEN,"[Applib - HDMI Monitor] AMP_DISP_ID_576P\t= %d",SinkInfo.pVideoInfo[AMP_DISP_ID_576P]);
        AmbaPrintColor(GREEN,"[Applib - HDMI Monitor] AMP_DISP_ID_576I\t= %d",SinkInfo.pVideoInfo[AMP_DISP_ID_576I]);
        AmbaPrintColor(GREEN,"[Applib - HDMI Monitor] AMP_DISP_ID_DMT0659\t= %d",SinkInfo.pVideoInfo[AMP_DISP_ID_DMT0659]);

         /** Check HDMI TV support required mode or not */
        if ((voutDispMode == AppLibSysVout_GetVoutMode(VOUT_DISP_MODE_2160P_HALF)) &&
            (!SinkInfo.pVideoInfo[AppLibSysVout_GetVoutMode(VOUT_DISP_MODE_2160P_HALF)])) {
            voutDispMode = AppLibSysVout_GetVoutMode(VOUT_DISP_MODE_1080P);
        }
        if ((voutDispMode == AppLibSysVout_GetVoutMode(VOUT_DISP_MODE_1080P)) &&
            (!SinkInfo.pVideoInfo[AppLibSysVout_GetVoutMode(VOUT_DISP_MODE_1080P)])) {
            voutDispMode = AppLibSysVout_GetVoutMode(VOUT_DISP_MODE_1080P_HALF);
        }
        if ((voutDispMode == AppLibSysVout_GetVoutMode(VOUT_DISP_MODE_1080P_HALF)) &&
           (!SinkInfo.pVideoInfo[AppLibSysVout_GetVoutMode(VOUT_DISP_MODE_1080P_HALF)])) {
            voutDispMode = AppLibSysVout_GetVoutMode(VOUT_DISP_MODE_1080I);
        }
        if ((voutDispMode == AppLibSysVout_GetVoutMode(VOUT_DISP_MODE_1080I)) &&
           (!SinkInfo.pVideoInfo[AppLibSysVout_GetVoutMode(VOUT_DISP_MODE_1080I)])) {
            voutDispMode = AppLibSysVout_GetVoutMode(VOUT_DISP_MODE_720P);
        }
        if ((voutDispMode == AppLibSysVout_GetVoutMode(VOUT_DISP_MODE_720P)) &&
            (!SinkInfo.pVideoInfo[AppLibSysVout_GetVoutMode(VOUT_DISP_MODE_720P)])) {
            voutDispMode = AppLibSysVout_GetVoutMode(VOUT_DISP_MODE_SDP);
        }
        if ((voutDispMode == AppLibSysVout_GetVoutMode(VOUT_DISP_MODE_SDP)) &&
            (!SinkInfo.pVideoInfo[AppLibSysVout_GetVoutMode(VOUT_DISP_MODE_SDP)])) {
            voutDispMode = AppLibSysVout_GetVoutMode(VOUT_DISP_MODE_SDI);
        }
        if ((voutDispMode == AppLibSysVout_GetVoutMode(VOUT_DISP_MODE_SDI)) &&
            (!SinkInfo.pVideoInfo[AppLibSysVout_GetVoutMode(VOUT_DISP_MODE_SDI)])) {
            voutDispMode = AppLibSysVout_GetVoutMode(VOUT_DISP_MODE_DMT0659);
        }
        if ((voutDispMode == AppLibSysVout_GetVoutMode(VOUT_DISP_MODE_DMT0659)) &&
            (!SinkInfo.pVideoInfo[AppLibSysVout_GetVoutMode(VOUT_DISP_MODE_DMT0659)])) {
            AmbaPrint("[AppLib - Display] <ConfigModeFchan> HDMI capability does not meet requirement, turn off Fchan");
            ApplibDispFchan.Config.DeviceId = AMP_DSIP_NONE;
            ReturnValue = -1;
            break;
        }

        // Set aspect ratio
        if ((voutDispMode == AppLibSysVout_GetVoutMode(VOUT_DISP_MODE_SDI)) ||
            (voutDispMode == AppLibSysVout_GetVoutMode(VOUT_DISP_MODE_DMT0659))) {
            // Set SDi(480i/576i) set to 4x3
            VoutDispAr = VAR_4x3;
        } else {
            VoutDispAr = VAR_16x9;
        }
        break;
    case AMP_DISP_CVBS:
        voutDispMode = AppLibSysVout_GetVoutMode(VOUT_DISP_MODE_SDI);
        /* Set the aspect ratio, SDi(480i/576i) set to 4x3 when the
        * channel is component. Composite is always SDi(4x3).
        */
        if (voutDispMode == AppLibSysVout_GetVoutMode(VOUT_DISP_MODE_SDI)) {
            VoutDispAr = VAR_4x3;
        } else {
            VoutDispAr = VAR_16x9;
        }
        break;
    case AMP_DISP_LCD:
        voutDispMode = AppLibSysLcd_GetDispMode(LCD_CH_FCHAN);
        VoutDispAr = AppLibSysLcd_GetDispAR(LCD_CH_FCHAN);
        break;
    default:
        DBGMSG("[AppLib - Display] <ConfigModeFchan> No fchan device");
        break;
    }
    //voutDispMode = AppLibSysVout_GetVoutMode(VOUT_DISP_MODE_1080P);
    //ApplibDispFchan.Config.DeviceId = AMP_DISP_HDMI;
    //VoutDispAr = VAR_16x9;
    /** set used vout mode */
    ApplibDispFchan.Config.DeviceMode = voutDispMode;
    ApplibDispFchan.Config.DeviceAr = VoutDispAr;

    return ReturnValue;
}

/**
 *  @brief Config the display mode of DChan.
 *
 *  Config the display mode of DChan.
 *
 *  @param [in] voutDispMode Display mode.
 *
 *  @return >=0 success, <0 failure
 */
static int AppLibDisp_ConfigModeDchan(int voutDispMode)
{
    int ReturnValue = 0;
    int VoutDispAr = VAR_ANY;

    switch (ApplibDispDchan.Config.DeviceId) {
    case AMP_DISP_LCD:
        voutDispMode = AppLibSysLcd_GetDispMode(LCD_CH_DCHAN);
        VoutDispAr = AppLibSysLcd_GetDispAR(LCD_CH_DCHAN);
        break;
    default:
        DBGMSG("[AppLib - Display] <ConfigModeDchan> No dchan device");
        break;
    }

    /** set used vout mode */
    ApplibDispDchan.Config.DeviceMode = voutDispMode;
    ApplibDispDchan.Config.DeviceAr = VoutDispAr;

    return ReturnValue;
}

/**
 *  @brief Config the display mode.
 *
 *  Config the display mode.
 *
 *  @param [in] dispChanID Channel ID
 *  @param [in] voutDispMode Display mode
 *
 *  @return >=0 success, <0 failure
 */
int AppLibDisp_ConfigMode(UINT32 dispChanID, int voutDispMode)
{
    int ReturnValue = 0;
    if (APPLIB_CHECKFLAGS(dispChanID, DISP_CH_FCHAN)) {
        ReturnValue += AppLibDisp_ConfigModeFchan(voutDispMode);
    }
    if (APPLIB_CHECKFLAGS(dispChanID, DISP_CH_DCHAN)) {
        ReturnValue += AppLibDisp_ConfigModeDchan(voutDispMode);
    }
    return ReturnValue;
}

/**
 *  @brief The post-config after FChan started
 *
 *  The post-config after FChan started
 *
 *  @return >=0 success, <0 failure
 */
static int AppLibDisp_SetupFchanPostConfig(void)
{
    int ReturnValue = 0;
    int SrcColor = ApplibDispFchan.SrcColor, DispColor = ApplibDispFchan.DispColor;

    /* Set the color mapping.*/
    if (SrcColor == DISP_COLOR_AUTO) {
        //SrcColor = FROM_PC;
    }
    if (DispColor == DISP_COLOR_AUTO) {
        if (ApplibDispFchan.Config.DeviceId == AMP_DISP_LCD) {
            //DispColor = TO_PC;
        } else {
            //DispColor = TO_TV;
        }
    }
    AppLibDisp_ColorMapping(DISP_CH_FCHAN, SrcColor, DispColor);

    /* Re-config the LCD setting.*/
    if (ApplibDispFchan.Config.DeviceId == AMP_DISP_LCD) {
        AppLibSysLcd_ParamReconfig(LCD_CH_FCHAN);
    }


    return ReturnValue;
}

/**
 *  @brief Setup the config of FChan
 *
 *  Setup the config of FChan
 *
 *  @return >=0 success, <0 failure
 */
static int AppLibDisp_SetupFchan(void)
{
    int ReturnValue = 0;
    AMP_DISP_DEV_CFG_s DevCfg;
    if (ApplibDispFchan.Config.DeviceId != AMP_DSIP_NONE) {
        DBGMSG("[AppLib - Display] <SetupFchan> id = %d", ApplibDispFchan.Config.DeviceId);
        DBGMSG("[AppLib - Display] <SetupFchan> mode = %d", ApplibDispFchan.Config.DeviceMode);
        DBGMSG("[AppLib - Display] <SetupFchan> ar = 0x%X", ApplibDispFchan.Config.DeviceAr);
        DBGMSG("[AppLib - Display] <SetupFchan> ch = %d", ApplibDispFchan.Config.Channel);
        AmpDisplay_GetDeviceCfg(ApplibDispFchanHdlr, &DevCfg);
        DevCfg.DeviceAr = ApplibDispFchan.Config.DeviceAr;
        DevCfg.DeviceId = ApplibDispFchan.Config.DeviceId;
        DevCfg.DeviceMode = ApplibDispFchan.Config.DeviceMode;
        AmpDisplay_SetDeviceCfg(ApplibDispFchanHdlr, &DevCfg);

    }

    return ReturnValue;
}

/**
 *  @brief Setup the pixel format in config of FChan(HDMI)
 *
 *  Setup the pixel format in config of FChan(HDMI)
 *
 *  @return >=0 success, <0 failure
 */
int AppLibDisp_SetupFchanPxlFmt(UINT32 HdmiOutputFormat)
{
    int ReturnValue = 0;
    AMP_DISP_DEV_CFG_s DevCfg;
    if (ApplibDispFchan.Config.DeviceId != AMP_DSIP_NONE) {
        DBGMSG("[AppLib - Display] <SetupFchan> id = %d", ApplibDispFchan.Config.DeviceId);
        DBGMSG("[AppLib - Display] <SetupFchan> mode = %d", ApplibDispFchan.Config.DeviceMode);
        DBGMSG("[AppLib - Display] <SetupFchan> ar = 0x%X", ApplibDispFchan.Config.DeviceAr);
        DBGMSG("[AppLib - Display] <SetupFchan> ch = %d", ApplibDispFchan.Config.Channel);
        AmpDisplay_GetDeviceCfg(ApplibDispFchanHdlr, &DevCfg);
        DevCfg.DeviceId = AMP_DISP_HDMI;
        DevCfg.CustomCfg.EnCustomCfg = 1;
        if (HdmiOutputFormat == AMBA_DSP_VOUT_HDMI_RGB444_8B) {
            AmbaPrint("HDMI pixel format RGB444_8B");
            DevCfg.CustomCfg.Cfg.HDMI.FrameLayout = AMBA_HDMI_VIDEO_2D;
            DevCfg.CustomCfg.Cfg.HDMI.PixelFormat = AMBA_DSP_VOUT_HDMI_RGB444_8B;
            DevCfg.CustomCfg.Cfg.HDMI.QuantRange = AMBA_HDMI_QRANGE_DEFAULT;
            DevCfg.CustomCfg.Cfg.HDMI.SampleRate = HDMI_AUDIO_FS_48K;
            DevCfg.CustomCfg.Cfg.HDMI.SpeakerAlloc = HDMI_CA_2CH_FL_FR;
            DevCfg.CustomCfg.Cfg.HDMI.OverSample = HDMI_AUDIO_CLK_FREQ_128FS;
        } else if (HdmiOutputFormat == AMBA_DSP_VOUT_HDMI_YCC444_8B) {
            AmbaPrint("HDMI pixel format YUV444_8B");
            DevCfg.CustomCfg.Cfg.HDMI.FrameLayout = AMBA_HDMI_VIDEO_2D;
            DevCfg.CustomCfg.Cfg.HDMI.PixelFormat = AMBA_DSP_VOUT_HDMI_YCC444_8B;
            DevCfg.CustomCfg.Cfg.HDMI.QuantRange = AMBA_HDMI_QRANGE_DEFAULT;
            DevCfg.CustomCfg.Cfg.HDMI.SampleRate = HDMI_AUDIO_FS_48K;
            DevCfg.CustomCfg.Cfg.HDMI.SpeakerAlloc = HDMI_CA_2CH_FL_FR;
            DevCfg.CustomCfg.Cfg.HDMI.OverSample = HDMI_AUDIO_CLK_FREQ_128FS;
        } else if (HdmiOutputFormat == AMBA_DSP_VOUT_HDMI_YCC422_12B) {
            AmbaPrint("HDMI pixel format YUV422_12B");
            DevCfg.CustomCfg.Cfg.HDMI.FrameLayout = AMBA_HDMI_VIDEO_2D;
            DevCfg.CustomCfg.Cfg.HDMI.PixelFormat = AMBA_DSP_VOUT_HDMI_YCC422_12B;
            DevCfg.CustomCfg.Cfg.HDMI.QuantRange = AMBA_HDMI_QRANGE_DEFAULT;
            DevCfg.CustomCfg.Cfg.HDMI.SampleRate = HDMI_AUDIO_FS_48K;
            DevCfg.CustomCfg.Cfg.HDMI.SpeakerAlloc = HDMI_CA_2CH_FL_FR;
            DevCfg.CustomCfg.Cfg.HDMI.OverSample = HDMI_AUDIO_CLK_FREQ_128FS;
        }

        DevCfg.DeviceAr = ApplibDispFchan.Config.DeviceAr;
        DevCfg.DeviceId = ApplibDispFchan.Config.DeviceId;
        DevCfg.DeviceMode = ApplibDispFchan.Config.DeviceMode;
        AmpDisplay_SetDeviceCfg(ApplibDispFchanHdlr, &DevCfg);

    }

    return ReturnValue;
}

/**
 *  @brief The post-config after DChan started
 *
 *  The post-config after DChan started.
 *
 *  @return >=0 success, <0 failure
 */
static int AppLibDisp_SetupDchanPostConfig(void)
{
    int ReturnValue = 0;
    int SrcColor = ApplibDispDchan.SrcColor;

    /* Set the color mapping.*/
    if (SrcColor == DISP_COLOR_AUTO) {
        //SrcColor = FROM_PC;
    }
    //AppLibDisp_ColorMapping(DISP_CH_DCHAN, SrcColor, TO_PC);

    /* Re-config the LCD setting.*/
    if (ApplibDispDchan.Config.DeviceId == AMP_DISP_LCD) {
        AppLibSysLcd_ParamReconfig(LCD_CH_DCHAN);
    }

    return ReturnValue;
}

/**
 *  @brief Setup the config of DChan
 *
 *  Setup the config of DChan
 *
 *  @return >=0 success, <0 failure
 */
static int AppLibDisp_SetupDchan(void)
{
    int ReturnValue = 0;
    AMP_DISP_DEV_CFG_s DevCfg;
    if (ApplibDispDchan.Config.DeviceId != AMP_DSIP_NONE) {
        DBGMSG("[AppLib - Display] <SetupDchan> id = %d", ApplibDispDchan.Config.DeviceId);
        DBGMSG("[AppLib - Display] <SetupDchan> mode = %d", ApplibDispDchan.Config.DeviceMode);
        DBGMSG("[AppLib - Display] <SetupDchan> ar = 0x%X", ApplibDispDchan.Config.DeviceAr);
        DBGMSG("[AppLib - Display] <SetupDchan> ch = %d", ApplibDispDchan.Config.Channel);
        AmpDisplay_GetDeviceCfg(ApplibDispDchanHdlr, &DevCfg);
        DevCfg.DeviceAr = ApplibDispDchan.Config.DeviceAr;
        DevCfg.DeviceId = ApplibDispDchan.Config.DeviceId;
        DevCfg.DeviceMode = ApplibDispDchan.Config.DeviceMode;
        AmpDisplay_SetDeviceCfg(ApplibDispDchanHdlr, &DevCfg);
        if (ReturnValue < 0) {
            AmbaPrintColor(RED,"[AppLib - Display] <SetupDchan> fail ReturnValue = %d",ReturnValue);
        } else if (ReturnValue == 0) {
            AppLibDisp_SetupDchanPostConfig();
        }
    }

    return ReturnValue;
}

/**
 *  @brief Setup the config
 *
 *  Setup the config
 *
 *  @param [in] dispChanID Channel ID
 *
 *  @return >=0 success, <0 failure
 */
int AppLibDisp_SetupChan(UINT32 dispChanID)
{
    int ReturnValue = 0;

    if (APPLIB_CHECKFLAGS(dispChanID, DISP_CH_FCHAN)) {
        ReturnValue += AppLibDisp_SetupFchan();
    }
    if (APPLIB_CHECKFLAGS(dispChanID, DISP_CH_DCHAN)) {
        ReturnValue += AppLibDisp_SetupDchan();
    }
    return ReturnValue;
}

/**
 *  @brief Setup the color mapping
 *
 *  Setup the color mapping
 *
 *  @param [in] dispChanID Channel ID
 *
 *  @return >=0 success, <0 failure
 */
int AppLibDisp_SetupColorMapping(UINT32 dispChanID)
{
    int ReturnValue = 0;
    UINT32 SrcColor = 0, DispColor = 0;

    if (APPLIB_CHECKFLAGS(dispChanID, DISP_CH_FCHAN) &&
        (ApplibDispFchan.Config.DeviceId != AMP_DSIP_NONE)) {
        if (ApplibDispFchan.SrcColor == DISP_COLOR_AUTO) {
            //SrcColor = FROM_PC;
        } else {
            SrcColor = ApplibDispFchan.SrcColor;
        }
        if (ApplibDispFchan.DispColor == DISP_COLOR_AUTO) {
            if (ApplibDispFchan.Config.DeviceId == AMP_DISP_LCD) {
                //DispColor = TO_PC;
            } else {
                //DispColor = TO_TV;
            }
        } else {
            DispColor = ApplibDispFchan.DispColor;
        }
        ReturnValue = AppLibDisp_ColorMapping(DISP_CH_FCHAN, SrcColor, DispColor);
    }
    if (APPLIB_CHECKFLAGS(dispChanID, DISP_CH_DCHAN) &&
        (ApplibDispDchan.Config.DeviceId != AMP_DSIP_NONE)) {
        if (ApplibDispDchan.SrcColor == DISP_COLOR_AUTO) {
            //SrcColor = FROM_PC;
        } else {
            SrcColor = ApplibDispDchan.SrcColor;
        }
        //ReturnValue = AppLibDisp_ColorMapping(DISP_CH_DCHAN, SrcColor, TO_PC);
    }

    return ReturnValue;
}

/**
 *  @brief Start the display channel.
 *
 *  Start the display channel.
 *
 *  @param [in] dispChanID Channel ID
 *
 *  @return >=0 success, <0 failure
 */
int AppLibDisp_ChanStart(UINT32 dispChanID)
{
    int ReturnValue = 0;

#ifdef CONFIG_APP_ARD
#ifdef CONFIG_LCD_T27P05
    //LCD tuning  set csc matrix
    //INT32 LCDCsc [9] = {
    //      0x1f2504a7, 0x04a71dde, 0x00000872,
    //      0x000004a7, 0x004c072c, 0x7f077edf,
    //      0x00ff0000, 0x00ff0000, 0x00ff0000 };
    AMBA_DSP_VOUT_CSC_CONFIG_s pCscConfig = {
        .Component[0] = { .Coefficient = { 1.163086, -0.212891, -0.532227}, .Constant =  76.000000, .LowerBound = 0, .UpperBound = 255 },
        .Component[1] = { .Coefficient = { 1.163086, 2.111328, 0.000000}, .Constant =  -288.000000, .LowerBound = 0, .UpperBound = 255 },
        .Component[2] = { .Coefficient = { 1.163086, 0.000000, 1.792969}, .Constant =  -248.000000, .LowerBound = 0, .UpperBound = 255 },
        };
#endif
#endif

    if (APPLIB_CHECKFLAGS(dispChanID, DISP_CH_FCHAN) &&
        APPLIB_CHECKFLAGS(ApplibDispFchan.Flags, DISP_CH_FLAGS_ENABLE) &&
        (ApplibDispFchan.Config.DeviceId != AMP_DSIP_NONE)) {
        ReturnValue = AmpDisplay_Start(ApplibDispFchanHdlr);
        DBGMSGc2(MAGENTA,"[AppLib - Display] <ChanStart> FCHAN ReturnValue = %d",ReturnValue);
        if (ReturnValue < 0) {
            AmbaPrintColor(RED,"[AppLib - Display] <ChanStart> fail ReturnValue = %d",ReturnValue);
        } else if (ReturnValue == 0) {
            AppLibDisp_SetupFchanPostConfig();
        }
    }
    if (APPLIB_CHECKFLAGS(dispChanID, DISP_CH_DCHAN) &&
        APPLIB_CHECKFLAGS(ApplibDispDchan.Flags, DISP_CH_FLAGS_ENABLE) &&
        (ApplibDispDchan.Config.DeviceId != AMP_DSIP_NONE)) {
        ReturnValue = AmpDisplay_Start(ApplibDispDchanHdlr);
        DBGMSGc2(MAGENTA,"[AppLib - Display] <ChanStart> DCHAN ReturnValue = %d",ReturnValue);
        if (ReturnValue < 0) {
            AmbaPrintColor(RED,"[AppLib - Display] <ChanStart> fail ReturnValue = %d",ReturnValue);
        } else if (ReturnValue == 0) {
            AppLibDisp_SetupDchanPostConfig();
#ifdef CONFIG_APP_ARD
#ifdef CONFIG_LCD_T27P05
            //LCD tuning  set csc matrix
            AmbaDSP_VoutDisplaySetCscMatrix(ApplibDispDchan.Id, &pCscConfig);
#endif
#endif
        }
    }
    return ReturnValue;
}

/**
 *  @brief Stop the display channel.
 *
 *  Stop the display channel.
 *
 *  @param [in] dispChanID Channel ID
 *
 *  @return >=0 success, <0 failure
 */
int AppLibDisp_ChanStop(UINT32 dispChanID)
{
    int ReturnValue = 0;
    if (APPLIB_CHECKFLAGS(dispChanID, DISP_CH_FCHAN) &&
        APPLIB_CHECKFLAGS(ApplibDispFchan.Flags, DISP_CH_FLAGS_ENABLE)) {
        ReturnValue = AmpDisplay_Stop(ApplibDispFchanHdlr);
        DBGMSGc2(MAGENTA,"[AppLib - Display] <ChanStop> FCHAN ReturnValue = %d",ReturnValue);
    }
    if (APPLIB_CHECKFLAGS(dispChanID, DISP_CH_DCHAN) &&
        APPLIB_CHECKFLAGS(ApplibDispDchan.Flags, DISP_CH_FLAGS_ENABLE)) {
        ReturnValue = AmpDisplay_Stop(ApplibDispDchanHdlr);
        DBGMSGc2(MAGENTA,"[AppLib - Display] <ChanStop> DCHAN ReturnValue = %d",ReturnValue);
    }
    return ReturnValue;
}

/**
 *  @brief Trigger the flow that reprogram the Lcd after booting system
 *
 *  Trigger the flow that reprogram the Lcd after booting system.
 *
 *  @return >=0 success, <0 failure
 */
int AppLibDisp_TriggerLcdBootReprogram(void)
{
    int ReturnValue = -1;

    return ReturnValue;
}

/**
 *  @brief The flag that reprogram the Lcd after booting system
 *
 *  The flag that reprogram the Lcd after booting system.
 *
 *  @return >=0 success, <0 failure
 */
int AppLibDisp_SetLcdBootReprogram(UINT32 dispChanID)
{
    int ReturnValue = -1;

    return ReturnValue;
}

/**
 *  @brief Rotate the display
 *
 *  Rotate the display
 *
 *  @param [in] dispChanID Channel ID
 *  @param [in] mode Mode
 *
 *  @return >=0 success, <0 failure
 */
int AppLibDisp_RotateVideo(UINT32 dispChanID, int mode)
{
    if (APPLIB_CHECKFLAGS(dispChanID, DISP_CH_FCHAN) &&
        APPLIB_CHECKFLAGS(ApplibDispFchan.Flags, DISP_CH_FLAGS_ENABLE) &&
        (ApplibDispFchan.Config.DeviceId == AMP_DISP_LCD)) {
        DBGMSG("[AppLib - Display] <RotateVideo> rotate video on fchan: %d", mode);
//            return AMP_display_cmd(MW_DISPLAY_SCREEN_ROTATION, ApplibDispFchan.Id, mode);
    }
    if (APPLIB_CHECKFLAGS(dispChanID, DISP_CH_DCHAN) &&
        APPLIB_CHECKFLAGS(ApplibDispDchan.Flags, DISP_CH_FLAGS_ENABLE) &&
        (ApplibDispDchan.Config.DeviceId == AMP_DISP_LCD)) {
        DBGMSG("[AppLib - Display] <RotateVideo> rotate video on dchan: %d", mode);
//            return AMP_display_cmd(MW_DISPLAY_SCREEN_ROTATION, ApplibDispDchan.Id, mode);
    }
    return -1;
}

/**
 *  @brief Setup the color mapping
 *
 *  Setup the color mapping
 *
 *  @param [in] dispChanID Channel ID
 *  @param [in] srcColor Source color type
 *  @param [in] dispColor Display color type
 *
 *  @return >=0 success, <0 failure
 */
int AppLibDisp_ColorMapping(UINT32 dispChanID, UINT32 SrcColor, UINT32 DispColor)
{
    int ReturnValue = -1;
    //UINT32 color_mappings[10] = {0};
    //color_mappings[0] = SrcColor | DispColor;
    if (APPLIB_CHECKFLAGS(dispChanID, DISP_CH_FCHAN) &&
        APPLIB_CHECKFLAGS(ApplibDispFchan.Flags, DISP_CH_FLAGS_ENABLE) &&
        (ApplibDispFchan.Config.DeviceId != AMP_DSIP_NONE)) {
        DBGMSG("[AppLib - Display] <ColorMapping> color mapping on fchan (src / disp): 0x%X / 0x%X", SrcColor, DispColor);
    }
    if (APPLIB_CHECKFLAGS(dispChanID, DISP_CH_DCHAN) &&
        APPLIB_CHECKFLAGS(ApplibDispDchan.Flags, DISP_CH_FLAGS_ENABLE) &&
        (ApplibDispDchan.Config.DeviceId != AMP_DSIP_NONE)) {
        DBGMSG("[AppLib - Display] <ColorMapping> color mapping on dchan (src / disp): 0x%X / 0x%X", SrcColor, DispColor);
    }
    return ReturnValue;
}

/**
 *  @brief Switch the system type NTSC<->PAL
 *
 *  Switch the system type NTSC<->PAL
 *
 *  @param [in] voutDispMode Display mode
 *
 *  @return >=0 success, <0 failure
 */
int AppLibDisp_SwitchSystemType(int voutDispMode)
{
    int ReturnValue = 0;
    int FchanReturnValue = 0, DchanReturnValue = 0;
    int Result = 0;

    ReturnValue = AppLibDisp_SelectDevice(DISP_CH_FCHAN, DISP_ANY_DEV);
    if (APPLIB_CHECKFLAGS(ReturnValue, DISP_FCHAN_NO_DEVICE)) {
        DBGMSGc2(GREEN,"[AppLib - Display] <SwitchSystemType> Fchan no device, and program dchan only.");
        AppLibDisp_ConfigMode(DISP_CH_DCHAN, voutDispMode);
        ReturnValue = AppLibDisp_SetupChan(DISP_CH_DCHAN);
        ReturnValue = AppLibDisp_ChanStart(DISP_CH_DCHAN);
        if (ReturnValue < 0) {
            AmbaPrintColor(RED,"[AppLib - Display] <SwitchSystemType> Fail to switch display mode on Dchan!");
            Result = -1;
        }
    } else {
        ReturnValue = AppLibDisp_ConfigMode(DISP_CH_FCHAN, voutDispMode);
        if (ReturnValue < 0) {
            DBGMSGc2(GREEN,"[AppLib - Display] <SwitchSystemType>  Fchan doesn't support this display mode.");
            if (AppLibDisp_GetDeviceID(DISP_CH_DCHAN) == AMP_DSIP_NONE) {
                AmbaPrintColor(RED,"[AppLib - Display] <SwitchSystemType> Fail to switch system type! Fchan doesn't support this mode and Dchan doesn't have device!");
                Result = -1;
            } else {
                DBGMSGc2(GREEN,"[AppLib - Display] <SwitchSystemType>  Program dchan only.");
                AppLibDisp_ConfigMode(DISP_CH_DCHAN, voutDispMode);
                ReturnValue = AppLibDisp_SetupChan(DISP_CH_DCHAN);
                ReturnValue = AppLibDisp_ChanStart(DISP_CH_DCHAN);
                if (ReturnValue < 0) {
                    AmbaPrintColor(RED,"[AppLib - Display] <SwitchSystemType> Fail to switch display mode on Dchan!");
                    Result = -1;
                }
            }
        } else {
            if (AppLibDisp_GetDeviceID(DISP_CH_DCHAN) == AMP_DSIP_NONE) {
                DBGMSGc2(GREEN,"[AppLib - Display] <SwitchSystemType>  Dchan no device, and program Fchan only.");
                if (ReturnValue < 0) {
                    AmbaPrintColor(RED,"[AppLib - Display] <SwitchSystemType> Fail to switch display mode on Fchan and Dchan doesn't have device!");
                    Result = -1;
                }
            } else {
                AMP_DISP_INFO_s DispDev = {0};
                AppLibDisp_GetDeviceInfo(DISP_CH_FCHAN, &DispDev);
                if (!DispDev.DeviceInfo.Enable) {
                    DBGMSGc2(GREEN,"[AppLib - Display] <SwitchSystemType>  Enable Fchan and switch the display mode on Dchan!");
                    AppLibDisp_ConfigMode(DISP_CH_DCHAN, voutDispMode);
                    DchanReturnValue = AppLibDisp_SetupChan(DISP_CH_DCHAN);
                    DchanReturnValue = AppLibDisp_ChanStart(DISP_CH_DCHAN);
                    FchanReturnValue = AppLibDisp_SetupChan(DISP_CH_FCHAN);
                    FchanReturnValue = AppLibDisp_ChanStart(DISP_CH_FCHAN);
                    if ((FchanReturnValue < 0) && (DchanReturnValue < 0)) {
                        AmbaPrintColor(RED,"[AppLib - Display] <SwitchSystemType> Fail to switch display mode on Dchan and Fchan!");
                        Result = -1;
                    } else if (DchanReturnValue < 0) {
                        AmbaPrintColor(RED,"[AppLib - Display] <SwitchSystemType> Fail to switch display mode on Dchan!Turn off Dchan.");
                    } else if (FchanReturnValue < 0) {
                        AmbaPrintColor(RED,"[AppLib - Display] <SwitchSystemType> Fail to switch display mode on Fchan!Turn off Fchan.");
                    }
                } else {
                    DBGMSGc2(GREEN,"[AppLib - Display] <SwitchSystemType>  To program Dual vout.");
                    AppLibDisp_ConfigMode(DISP_CH_DCHAN, voutDispMode);
                    AppLibDisp_SwitchDualVoutType(&FchanReturnValue, &DchanReturnValue);
                    if ((FchanReturnValue < 0) && (DchanReturnValue < 0)) {
                        AmbaPrintColor(RED,"[AppLib - Display] <SwitchSystemType> Fail to switch display mode on Dchan and Fchan!");
                        Result = -1;
                    } else if (DchanReturnValue < 0) {
                        AmbaPrintColor(RED,"[AppLib - Display] <SwitchSystemType> Fail to switch display mode on Dchan!Turn off Dchan.");
                    } else if (FchanReturnValue < 0) {
                        AmbaPrintColor(RED,"[AppLib - Display] <SwitchSystemType> Fail to switch display mode on Fchan!Turn off Fchan.");
                    } else {
                        DBGMSGc2(GREEN,"[AppLib - Display] <SwitchSystemType> Program Dual vout successfully.");
                    }
                }
            }
        }
    }

    if (Result == -1) {
        AmbaPrintColor(RED,"[AppLib - Display] <SwitchSystemType> Fail to switch system type!");
    }

    return Result;
}

/**
 *  @brief Switch the dual-vout type NTSC<->PAL
 *
 *  Switch the dual-vout type NTSC<->PAL
 *
 *  @param [out] fchanReturnValue The return value of FChan
 *  @param [out] dchanReturnValue The return value of DChan
 *
 *  @return >=0 success, <0 failure
 */
int AppLibDisp_SwitchDualVoutType(int *fchanReturnValue, int *dchanReturnValue)
{
    int ReturnValue = 0;
 //   mw_four_param_t ReturnValues = {0};

//    ReturnValues.param3 = ApplibDispFchan.Config.deviceMode; // set target mode for fchan
//   ReturnValues.param4 = ApplibDispDchan.Config.deviceMode; // set target mode for dchan
    if (AppLibSysVout_GetSystemType() == VOUT_SYS_PAL) {
    //    ReturnValue = AMP_display_cmd(MW_DISPLAY_SWITCH_NTSC_PAL, DISP_N2P_MANUAL, &ReturnValues);
    } else {    // VOUT_SYS_NTSC
    //    ReturnValue = AMP_display_cmd(MW_DISPLAY_SWITCH_NTSC_PAL, DISP_P2N_MANUAL, &ReturnValues);
    }
//    *FchanReturnValue = ReturnValues.param1;
//    *DchanReturnValue = ReturnValues.param2;

    if (*fchanReturnValue == 0) {
        AppLibDisp_SetupFchanPostConfig();
    }
    if (*dchanReturnValue == 0) {
        AppLibDisp_SetupDchanPostConfig();
    }
    return ReturnValue;
}

/**
 *  @brief Get the information of display device
 *
 *  Get the information of display device
 *
 *  @param [in] dispChanID Channel ID
 *  @param [out] dispDev The device's information
 *
 *  @return >=0 success, <0 failure
 */
int AppLibDisp_GetDeviceInfo(UINT32 dispChanID, AMP_DISP_INFO_s *dispDev)
{
    if ((dispChanID == DISP_CH_FCHAN) && (ApplibDispFchan.Id != -1)) {
        return AmpDisplay_GetInfo(ApplibDispFchanHdlr, dispDev);
    } else if ((dispChanID == DISP_CH_DCHAN) && (ApplibDispDchan.Id != -1)) {
        return AmpDisplay_GetInfo(ApplibDispDchanHdlr, dispDev);
    } else {
        return -1;
    }
}

/**
 *  @brief Initialize the window module
 *
 *  Initialize the window module
 *
 *  @return >=0 success, <0 failure
 */
int AppLibDisp_InitWindow(void)
{
    int ReturnValue = 0;
    AMP_DISP_WINDOW_CFG_s Window;
    int i = 0;
    memset(&Window, 0x0, sizeof(AMP_DISP_WINDOW_CFG_s));

    for (i = 0;i < MAX_ENC_DEC_WINDOW_NUM; i++) {
        // Creat LCD Window
        Window.Source = AMP_DISP_DEFIMG;
        Window.CropArea.Width = 0;
        Window.CropArea.Height = 0;
        Window.CropArea.X = 0;
        Window.CropArea.Y = 0;
        Window.TargetAreaOnPlane.Width = 960;
        Window.TargetAreaOnPlane.Height = 480;
        Window.TargetAreaOnPlane.X = 00;
        Window.TargetAreaOnPlane.Y = (480-Window.TargetAreaOnPlane.Height)/2;
        ReturnValue = AppLibDisp_AddWindow(DISP_CH_DCHAN, &Window);
        if (ReturnValue < 0) {
            DchanWindowId[i] = -1;
            AmbaPrintColor(RED, "[AppLib - Display] <InitWindow> Add Window fail");
        } else {
            DchanWindowId[i] = ReturnValue;
        }
    }

    for (i = 0;i < MAX_ENC_DEC_WINDOW_NUM; i++) {
        // Creat TV Window
        Window.Source = AMP_DISP_DEFIMG;
        Window.CropArea.Width = 0;
        Window.CropArea.Height = 0;
        Window.CropArea.X = 0;
        Window.CropArea.Y = 0;
        Window.TargetAreaOnPlane.Width = 1920;
        Window.TargetAreaOnPlane.Height = 1080;//  interlance should be consider in MW
        Window.TargetAreaOnPlane.X = 0;
        Window.TargetAreaOnPlane.Y = 0;
        ReturnValue = AppLibDisp_AddWindow(DISP_CH_FCHAN, &Window);
        if (ReturnValue < 0) {
            FchanWindowId[i] = -1;
            AmbaPrintColor(RED, "[AppLib - Display] <InitWindow> Add Window fail");
        } else {
            FchanWindowId[i] = ReturnValue;
        }
    }

    return ReturnValue;
}

/**
 *  @brief To get the window id
 *
 *  To get the window id
 *
 *  @param [in] dispChanID Channel ID
 *
 *  @return >=0 The window id, <0 failure
 */
int AppLibDisp_GetWindowId(UINT32 dispChanID, UINT32 slot)
{
    int ReturnValue = -1;

    if (dispChanID == DISP_CH_FCHAN) {
        if (ApplibDispFchan.Id == -1) {
            DBGMSG("[AppLib - Display] <GetWindowId> Fchan is not enabled.");
        } else {
            ReturnValue = FchanWindowId[slot];
        }
    } else if (dispChanID == DISP_CH_DCHAN) {
        if (ApplibDispDchan.Id == -1) {
            DBGMSG("[AppLib - Display] <GetWindowId> Dchan is not enabled.");
        } else {
            ReturnValue = DchanWindowId[slot];
        }
    } else {
        AmbaPrintColor(RED, "[AppLib - Display] <GetWindowId> Invalid paramter");
    }

    return ReturnValue;
}

/**
 *  @brief Find the empty slot of window
 *
 *  Find the empty slot of window
 *
 *  @param [in] dispChanID Channel ID
 *
 *  @return >=0 success, <0 failure
 */
static int AppLibDisp_FindEmptyWindowSlot(UINT32 dispChanID)
{
    int ReturnValue = -1;
    int i = 0;
    if (dispChanID == DISP_CH_FCHAN) {
        if (ApplibDispFchan.Id == -1) {
            DBGMSG("[AppLib - Display] <FindEmptyWindowSlot> Fchan is not enabled.");
        } else {
            for (i=0;i<MAX_APP_FCHAN_WINDOW_NUM;i++) {
                if (WindowFchanHdlr[i]==NULL)
                    return i;
            }
        }
    } else if (dispChanID == DISP_CH_DCHAN) {
        if (ApplibDispDchan.Id == -1) {
            DBGMSG("[AppLib - Display] <FindEmptyWindowSlot> Dchan is not enabled.");
        } else {
            for (i=0;i<MAX_APP_DCHAN_WINDOW_NUM;i++) {
                if (WindowDchanHdlr[i]==NULL)
                    return i;
            }
        }
    } else {
        AmbaPrintColor(RED, "[AppLib - Display] <FindEmptyWindowSlot> Invalid paramter");
    }

    return ReturnValue;
}

/**
 *  @brief Add a window
 *
 *  Add a window
 *
 *  @param [in] dispChanID Channel ID
 *  @param [in] config Configuration
 *
 *  @return >=0 success, <0 failure
 */
int AppLibDisp_AddWindow(UINT32 dispChanID, AMP_DISP_WINDOW_CFG_s *config)
{
    int ReturnValue = -1;
    int EmptySlot = 0;

    DBGMSG("[AppLib - Display] <AddWindow> start");

    DBGMSG("[AppLib - Display] <AddWindow> Window.source = %d",config->Source);
    DBGMSG("[AppLib - Display] <AddWindow> Window.cropArea.Width = %d",config->CropArea.Width);
    DBGMSG("[AppLib - Display] <AddWindow> Window.cropArea.Height = %d",config->CropArea.Height);
    DBGMSG("[AppLib - Display] <AddWindow> Window.cropArea.X = %d", config->CropArea.X);
    DBGMSG("[AppLib - Display] <AddWindow> Window.cropArea.Y = %d", config->CropArea.Y);
    DBGMSG("[AppLib - Display] <AddWindow> Window.targetAreaOnPlane.Width = %d", config->TargetAreaOnPlane.Width);
    DBGMSG("[AppLib - Display] <AddWindow> Window.targetAreaOnPlane.Height = %d", config->TargetAreaOnPlane.Height);
    DBGMSG("[AppLib - Display] <AddWindow> Window.targetAreaOnPlane.X = %d", config->TargetAreaOnPlane.X);
    DBGMSG("[AppLib - Display] <AddWindow> Window.targetAreaOnPlane.Y = %d", config->TargetAreaOnPlane.Y);
    if ((dispChanID == DISP_CH_FCHAN) && (ApplibDispFchan.Id != -1)) {
        EmptySlot = AppLibDisp_FindEmptyWindowSlot(DISP_CH_FCHAN);
        if (EmptySlot < 0) {
            AmbaPrintColor(RED, "[AppLib - Display] <AddWindow> Non empty slot in Fchan.");
        } else {
            ReturnValue = AmpDisplay_CreateWindow(ApplibDispFchanHdlr, config, &WindowFchanHdlr[EmptySlot]);
            if (ReturnValue < 0) {
                AmbaPrintColor(RED, "[AppLib - Display] <AddWindow> Create Window fail.");
                WindowFchanHdlr[EmptySlot] = NULL;
            } else {
                ReturnValue = EmptySlot;
            }
            DBGMSG("[AppLib - Display] <AddWindow> The Window address is [0x%X]", WindowFchanHdlr[EmptySlot]);
        }
    } else if ((dispChanID == DISP_CH_DCHAN) && (ApplibDispDchan.Id != -1)) {
        EmptySlot = AppLibDisp_FindEmptyWindowSlot(DISP_CH_DCHAN);
        if (EmptySlot < 0) {
            AmbaPrintColor(RED, "[AppLib - Display] <AddWindow> Non empty slot in Dchan.");
        } else {
            ReturnValue = AmpDisplay_CreateWindow(ApplibDispDchanHdlr, config, &WindowDchanHdlr[EmptySlot]);
            if (ReturnValue < 0) {
                AmbaPrintColor(RED, "[AppLib - Display] <AddWindow> Create Window fail.");
                WindowDchanHdlr[EmptySlot] = NULL;
            } else {
                ReturnValue = EmptySlot;
            }
            DBGMSG("[AppLib - Display] <AddWindow> The Window address is [0x%X]", WindowDchanHdlr[EmptySlot]);
        }
    } else {
        AmbaPrintColor(RED, "[AppLib - Display] <AddWindow> Invalid paramter");
        return -1;
    }

    return ReturnValue;
}

/**
 *  @brief Delete the window
 *
 *  Delete the window
 *
 *  @param [in] dispChanID Channel ID
 *  @param [in] slot Slot
 *
 *  @return >=0 success, <0 failure
 */
int AppLibDisp_DeleteWindow(UINT32 dispChanID, int slot)
{
    int ReturnValue = -1;

    if ((dispChanID == DISP_CH_FCHAN) && (ApplibDispFchan.Id != -1)) {
        DBGMSG("[AppLib - Display] <DeleteWindow> Addr = 0x%X", WindowFchanHdlr[slot]);
        if (WindowFchanHdlr[slot] == NULL) {
            AmbaPrintColor(RED, "[AppLib - Display] <DeleteWindow> The Window address is NULL !!!");
            return -1;
        } else {
            ReturnValue = AmpDisplay_DeleteWindow(WindowFchanHdlr[slot]);
        }
    } else if ((dispChanID == DISP_CH_DCHAN) && (ApplibDispDchan.Id != -1)) {
        DBGMSG("[AppLib - Display] <DeleteWindow> Addr = 0x%X", WindowDchanHdlr[slot]);
        if (WindowDchanHdlr[slot] == NULL) {
            AmbaPrintColor(RED, "[AppLib - Display] <DeleteWindow> The Window address is NULL !!!");
            return -1;
        } else {
            ReturnValue = AmpDisplay_DeleteWindow(WindowDchanHdlr[slot]);
        }
    } else {
        AmbaPrintColor(RED, "[AppLib - Display] <DeleteWindow> Invalid paramter");
    }

    return ReturnValue;
}

/**
 *  @brief Set the winsow configuration
 *
 *  Set the winsow configuration
 *
 *  @param [in] dispChanID Channel ID
 *  @param [in] slot Slot
 *  @param [in] config Configuration
 *
 *  @return >=0 success, <0 failure
 */
int AppLibDisp_SetWindowConfig(UINT32 dispChanID, int slot, AMP_DISP_WINDOW_CFG_s *config)
{
    int ReturnValue = -1;
#if 0
    AMP_DISP_WINDOW_CFG_s Window;

    if ((dispChanID == DISP_CH_FCHAN) && (ApplibDispFchan.Id != -1)) {
        DBGMSG("[AppLib - Display] <SetWindowConfig> Addr = 0x%X", WindowFchanHdlr[slot]);
        if (WindowFchanHdlr[slot] == NULL) {
            AmbaPrintColor(RED, "[AppLib - Display] <SetWindowConfig> The Window address is NULL !!!");
            return -1;
        } else {
            ReturnValue = AmpDisplay_GetWindowCfg(WindowFchanHdlr[slot], &Window);
        }
    } else if ((dispChanID == DISP_CH_DCHAN) && (ApplibDispDchan.Id != -1)) {
        DBGMSG("[AppLib - Display] <SetWindowConfig> Addr = 0x%X", WindowDchanHdlr[slot]);
        if (WindowDchanHdlr[slot] == NULL) {
            AmbaPrintColor(RED, "[AppLib - Display] <SetWindowConfig>The Window address is NULL !!!");
            return -1;
        } else {
            ReturnValue = AmpDisplay_GetWindowCfg(WindowDchanHdlr[slot], &Window);
        }
    } else {
        AmbaPrintColor(RED, "[AppLib - Display] <SetWindowConfig> Invalid paramter");
        return -1;
    }

    Window.Source = config->Source;
    Window.CropArea.Width = config->CropArea.Width;
    Window.CropArea.Height = config->CropArea.Height;
    Window.CropArea.X = config->CropArea.X;
    Window.CropArea.Y = config->CropArea.Y;
    Window.TargetAreaOnPlane.Width = config->TargetAreaOnPlane.Width;
    Window.TargetAreaOnPlane.Height = config->TargetAreaOnPlane.Height;
    Window.TargetAreaOnPlane.X = config->TargetAreaOnPlane.X;
    Window.TargetAreaOnPlane.Y = config->TargetAreaOnPlane.Y;

    DBGMSG("[AppLib - Display] <SetWindowConfig> Window.CropArea.Width = %d",Window.CropArea.Width);
    DBGMSG("[AppLib - Display] <SetWindowConfig> Window.CropArea.Height = %d",Window.CropArea.Height);
    DBGMSG("[AppLib - Display] <SetWindowConfig> Window.TargetAreaOnPlane.Width= %d",Window.TargetAreaOnPlane.Width);
    DBGMSG("[AppLib - Display] <SetWindowConfig> Window.TargetAreaOnPlane.Height = %d", Window.TargetAreaOnPlane.Height);
#endif
    DBGMSG("[AppLib - Display] <SetWindowConfig> Window.source = %d", config->Source);
    DBGMSG("[AppLib - Display] <SetWindowConfig> Window.cropArea.Width = %d",config->CropArea.Width);
    DBGMSG("[AppLib - Display] <SetWindowConfig> Window.cropArea.Height = %d",config->CropArea.Height);
    DBGMSG("[AppLib - Display] <SetWindowConfig> Window.cropArea.X = %d", config->CropArea.X);
    DBGMSG("[AppLib - Display] <SetWindowConfig> Window.cropArea.Y = %d", config->CropArea.Y);
    DBGMSG("[AppLib - Display] <SetWindowConfig> Window.targetAreaOnPlane.Width = %d", config->TargetAreaOnPlane.Width);
    DBGMSG("[AppLib - Display] <SetWindowConfig> Window.targetAreaOnPlane.Height = %d", config->TargetAreaOnPlane.Height);
    DBGMSG("[AppLib - Display] <SetWindowConfig> Window.targetAreaOnPlane.X = %d", config->TargetAreaOnPlane.X);
    DBGMSG("[AppLib - Display] <SetWindowConfig> Window.targetAreaOnPlane.Y = %d", config->TargetAreaOnPlane.Y);

    if ((dispChanID == DISP_CH_FCHAN) && (ApplibDispFchan.Id != -1)) {
        if (WindowFchanHdlr[slot] == NULL) {
            AmbaPrintColor(RED, "[AppLib - Display] <SetWindowConfig> The Window address is NULL !!!");
            return -1;
        } else {
            ReturnValue = AmpDisplay_SetWindowCfg(WindowFchanHdlr[slot], config);
        }
    } else if ((dispChanID == DISP_CH_DCHAN) && (ApplibDispDchan.Id != -1)) {
        if (WindowDchanHdlr[slot] == NULL) {
            AmbaPrintColor(RED, "[AppLib - Display] <SetWindowConfig> The Window address is NULL !!!");
            return -1;
        } else {
            ReturnValue = AmpDisplay_SetWindowCfg(WindowDchanHdlr[slot], config);
        }
    } else {
        AmbaPrintColor(RED, "[AppLib - Display] <SetWindowConfig> Invalid paramter");
    }
    return ReturnValue;
}

/**
 *  @brief Get the window configuration
 *
 *  Get the window configuration
 *
 *  @param [in] dispChanID Channel ID
 *  @param [in] slot Slot
 *  @param [out] config Configuration
 *
 *  @return >=0 success, <0 failure
 */
int AppLibDisp_GetWindowConfig(UINT32 dispChanID, int slot, AMP_DISP_WINDOW_CFG_s *config)
{
    int ReturnValue = -1;
    if ((dispChanID == DISP_CH_FCHAN) && (ApplibDispFchan.Id != -1)) {
        DBGMSG("[AppLib - Display] <SetWindowConfig> Addr = 0x%X", WindowFchanHdlr[slot]);
        if (WindowFchanHdlr[slot] == NULL) {
            AmbaPrintColor(RED, "[AppLib - Display] <SetWindowConfig> The Window address is NULL !!!");
            return -1;
        } else {
            ReturnValue = AmpDisplay_GetWindowCfg(WindowFchanHdlr[slot], config);
        }
    } else if ((dispChanID == DISP_CH_DCHAN) && (ApplibDispDchan.Id != -1)) {
        DBGMSG("[AppLib - Display] <SetWindowConfig> Addr = 0x%X", WindowDchanHdlr[slot]);
        if (WindowDchanHdlr[slot] == NULL) {
            AmbaPrintColor(RED, "[AppLib - Display] <SetWindowConfig>The Window address is NULL !!!");
            return -1;
        } else {
            ReturnValue = AmpDisplay_GetWindowCfg(WindowDchanHdlr[slot], config);
        }
    } else {
        AmbaPrintColor(RED, "[AppLib - Display] <SetWindowConfig> Invalid paramter");
        return -1;
    }

    return ReturnValue;
}

/**
 *  @brief Get the window handler
 *
 *  Get the window handler
 *
 *  @param [in] dispChanID Channel ID
 *  @param [in] slot Slot
 *
 *  @return The window handler
 */
AMP_DISP_WINDOW_HDLR_s *AppLibDisp_GetWindowHandler(UINT32 dispChanID, int slot)
{
    if ((dispChanID == DISP_CH_FCHAN) && (ApplibDispFchan.Id != -1)) {
        return WindowFchanHdlr[slot];
    } else if ((dispChanID == DISP_CH_DCHAN) && (ApplibDispDchan.Id != -1)) {
        return WindowDchanHdlr[slot];
    } else {
        AmbaPrintColor(RED, "[AppLib - Display] <GetWindowHandler> Invalid paramter");
    }
    return 0;
}

/**
 *  @brief Activate the window
 *
 *  Activate the window
 *
 *  @param [in] dispChanID Channel ID
 *  @param [in] slot Slot
 *
 *  @return >=0 success, <0 failure
 */
int AppLibDisp_ActivateWindow(UINT32 dispChanID, int slot)
{
    int ReturnValue = -1;
    if ((dispChanID == DISP_CH_FCHAN) && (ApplibDispFchan.Id != -1)) {
        DBGMSGc2(GREEN, "[AppLib - Display] <ActivateWindow> fchan Addr = 0x%X", WindowFchanHdlr[slot]);
        if (WindowFchanHdlr[slot] == NULL) {
            AmbaPrintColor(RED, "[AppLib - Display] <ActivateWindow> The Window address is NULL !!!");
            return -1;
        } else {
            ReturnValue = AmpDisplay_SetWindowActivateFlag(WindowFchanHdlr[slot], 1);
        }
    } else if ((dispChanID == DISP_CH_DCHAN) && (ApplibDispDchan.Id != -1)) {
        DBGMSGc2(GREEN,"[AppLib - Display] <ActivateWindow> dchan Addr = 0x%X", WindowDchanHdlr[slot]);
        if (WindowDchanHdlr[slot] == NULL) {
            AmbaPrintColor(RED, "[AppLib - Display] <ActivateWindow> The Window address is NULL !!!");
            return -1;
        } else {
            ReturnValue = AmpDisplay_SetWindowActivateFlag(WindowDchanHdlr[slot], 1);
        }
    } else {
        AmbaPrintColor(RED, "[AppLib - Display] <ActivateWindow> Invalid paramter");
    }
    if (ReturnValue < 0) {
        AmbaPrintColor(RED,"[AppLib - Display] <ActivateWindow> Failure.");
    }
    return ReturnValue;
}

/**
 *  @brief Deactivate the window
 *
 *  Deactivate the window
 *
 *  @param [in] dispChanID Channel ID
 *  @param [in] slot Slot
 *
 *  @return >=0 success, <0 failure
 */
int AppLibDisp_DeactivateWindow(UINT32 dispChanID, int slot)
{
    int ReturnValue = -1;
    if ((dispChanID == DISP_CH_FCHAN) && (ApplibDispFchan.Id != -1)) {
        DBGMSG("[AppLib - Display] <DeactivateWindow> Addr = 0x%X", WindowFchanHdlr[slot]);
        if (WindowFchanHdlr[slot] == NULL) {
            AmbaPrintColor(RED, "[AppLib - Display] <DeactivateWindow> The Window address is NULL");
            return -1;
        } else {
            ReturnValue = AmpDisplay_SetWindowActivateFlag(WindowFchanHdlr[slot], 0);
        }
    } else if ((dispChanID == DISP_CH_DCHAN) && (ApplibDispDchan.Id != -1)) {
        DBGMSG("[AppLib - Display] <DeactivateWindow>, Addr = 0x%X", WindowDchanHdlr[slot]);
        if (WindowDchanHdlr[slot] == NULL) {
            AmbaPrintColor(RED, "[AppLib - Display] <DeactivateWindow> The Window address is NULL");
            return -1;
        } else {
            ReturnValue = AmpDisplay_SetWindowActivateFlag(WindowDchanHdlr[slot], 0);
        }
    } else {
        AmbaPrintColor(RED, "[AppLib - Display] <DeactivateWindow> Invalid paramter");
    }

    return ReturnValue;
}

/**
 *  @brief Update window configuration
 *
 *  Update window configuration
 *
 *  @param [in] dispChanID Channel ID
 *
 *  @return >=0 success, <0 failure
 */
int AppLibDisp_FlushWindow(UINT32 dispChanID)
{
    int ReturnValue = -1;
    if (APPLIB_CHECKFLAGS(dispChanID, DISP_CH_FCHAN) && (ApplibDispFchan.Id != -1)) {
        DBGMSG("[AppLib - Display] <FlushWindow> FCHAN");
        ReturnValue = AmpDisplay_Update(ApplibDispFchanHdlr);
    }
    if (APPLIB_CHECKFLAGS(dispChanID, DISP_CH_DCHAN) && (ApplibDispDchan.Id != -1)) {
        DBGMSG("[AppLib - Display] <FlushWindow> DCHAN");
        ReturnValue = AmpDisplay_Update(ApplibDispDchanHdlr);
    }

    return ReturnValue;
}

/**
 *  @brief Calculate the preview window size.
 *
 *  Calculate the preview window size.
 *
 *  @param [in,out] prevParam The parameter of video preview.
 */
void AppLibDisp_CalcPreviewWindowSize(APPLIB_VOUT_PREVIEW_PARAM_s *prevParam)
{
    int ar = 0;
    int ReturnValue = 0;
    AMP_DISP_INFO_s DispDev = {0};
    AMP_DISP_DEV_CFG_s DispConfig;
    UINT32 ArX = 0, ArY = 0, DispAr = 0;
#ifdef CONFIG_APP_ARD
    int LcdVoutNeedRotate = 0;
#endif
    if ((prevParam->ChanID == DISP_CH_FCHAN) && (ApplibDispFchan.Id == -1)) {
        return;
    } else if ((prevParam->ChanID == DISP_CH_DCHAN) && (ApplibDispDchan.Id == -1)) {
        return;
    }

    ReturnValue = AppLibDisp_GetDeviceInfo(prevParam->ChanID,&DispDev);
    if (ReturnValue < 0) {
        AmbaPrintColor(RED,"[AppLib - Display] <CalcPreviewWindowSize> GetDeviceInfo failure.");
    } else {
        DBGMSG("[AppLib - Display] <CalcPreviewWindowSize> DispDev.deviceInfo.enable = %d", DispDev.DeviceInfo.Enable);
        DBGMSG("[AppLib - Display] <CalcPreviewWindowSize> DispDev.deviceInfo.voutWidth = %d", DispDev.DeviceInfo.VoutWidth);
        DBGMSG("[AppLib - Display] <CalcPreviewWindowSize> DispDev.deviceInfo.voutHeight = %d", DispDev.DeviceInfo.VoutHeight);
        DBGMSG("[AppLib - Display] <CalcPreviewWindowSize> DispDev.deviceInfo.devPixelAr = %f", DispDev.DeviceInfo.DevPixelAr);
    }
    ReturnValue = AppLibDisp_GetChanConfig(prevParam->ChanID, &DispConfig);
    if (ReturnValue < 0) {
        AmbaPrintColor(RED,"[AppLib - Display] <CalcPreviewWindowSize> GetChanConfig failure.");
    }

    DispAr = DispConfig.DeviceAr;
    if (DispDev.DeviceInfo.Enable == 0) {
        DBGMSGc2(BLUE,"[AppLib - Display] <calcPreviewWindowSize> VOUT %d disabled. To use the default value.", prevParam->ChanID);
        DispAr = AppLibSysLcd_GetDispAR(LCD_CH_DCHAN);
        AppLibSysLcd_GetDimensions(LCD_CH_DCHAN, &DispDev.DeviceInfo.VoutWidth, &DispDev.DeviceInfo.VoutHeight);
        DispDev.DeviceInfo.DevPixelAr = (1.0*GET_VAR_X(DispAr)*DispDev.DeviceInfo.VoutHeight)/(1.0*GET_VAR_Y(DispAr)*DispDev.DeviceInfo.VoutWidth);
    }

#ifdef CONFIG_APP_ARD
    if((prevParam->ChanID == DISP_CH_DCHAN)&&(AppLibDisp_GetRotate(DISP_CH_DCHAN) == AMP_ROTATE_90)){
        extern int AppUtil_SystemIsEncMode(void);
        if(AppUtil_SystemIsEncMode()){
            if(AppLibVideoEnc_GetSensorVideoRes()==SENSOR_VIDEO_RES_WQHD_HALF_HDR){
                LcdVoutNeedRotate = 1;
            }else{
                LcdVoutNeedRotate = 1;
            }
        }else{
            LcdVoutNeedRotate = 1;
        }
    }else{
        LcdVoutNeedRotate = 0;
    }
#endif
#ifdef CONFIG_APP_ARD
    if(LcdVoutNeedRotate){
        UINT16 tmpval;
        DispAr = VAR_16x9;
        tmpval = DispDev.DeviceInfo.VoutWidth;
        DispDev.DeviceInfo.VoutWidth = DispDev.DeviceInfo.VoutHeight;
        DispDev.DeviceInfo.VoutHeight = tmpval;
        DispDev.DeviceInfo.DevPixelAr = (1.0*GET_VAR_X(DispAr)*DispDev.DeviceInfo.VoutHeight)/(1.0*GET_VAR_Y(DispAr)*DispDev.DeviceInfo.VoutWidth);
    }
#endif

    DBGMSGc2(GREEN,"[AppLib - Display] <calcPreviewWindowSize> DispDev.pixel_ar = %f, DispAr = %d  DispDev.deviceInfo.voutWidth=%d DispDev.deviceInfo.voutHeight = %d",DispDev.DeviceInfo.DevPixelAr,DispAr,DispDev.DeviceInfo.VoutWidth,DispDev.DeviceInfo.VoutHeight);
    ar = (prevParam->AspectRatio == VAR_ANY) ? DispAr : prevParam->AspectRatio;
    ArX = GET_VAR_X(ar);
    ArY = GET_VAR_Y(ar);
    DBGMSGc2(GREEN,"[AppLib - Display] <calcPreviewWindowSize> prevParam->AspectRatio = %d , ar = %d,  DispAr = %d",prevParam->AspectRatio,ar,DispAr);
    {
        UINT16 KeepWidth = 1;

        switch (DispAr) {
        case VAR_16x9:
            KeepWidth = 0;
            break;
        case VAR_3x2:
            if (ar != VAR_16x9) {
                KeepWidth = 0;
            }
            break;
        case VAR_4x3:
            if (ar == VAR_1x1) {
                KeepWidth = 0;
            }
            break;
        case VAR_1x1:
            KeepWidth = 0;
            break;
        case VAR_2x3:
        case VAR_3x4:
        case VAR_9x16:
            break;
        default:
            AmbaPrintColor(RED,"[AppLib - Display] <calcPreviewWindowSize> Unsupported device AR");
            break;
        }

        _CALC:
        if (KeepWidth) {
            UINT16 real_w = (UINT16)((float)DispDev.DeviceInfo.VoutWidth*DispDev.DeviceInfo.DevPixelAr);

            DBGMSGc2(GREEN,"[AppLib - Display] <calcPreviewWindowSize> keep width");
            prevParam->Preview.Width = DispDev.DeviceInfo.VoutWidth;
            prevParam->Preview.X = 0;
            prevParam->Preview.Height = real_w*ArY/ArX;
            prevParam->Preview.Height = (prevParam->Preview.Height+3) & 0xFFFC;
            // The video preview size should always be times of 16 to make AR consistent.
          //  prevParam->Preview.Width = prevParam->Preview.Width & 0xfff0;
           // prevParam->Preview.Height = prevParam->Preview.Height & 0xfff0;
            prevParam->Preview.Y = (DispDev.DeviceInfo.VoutHeight - prevParam->Preview.Height) / 2;

            // Result height is too big, use height to recalc constraint by height again.
            if (prevParam->Preview.Height > DispDev.DeviceInfo.VoutHeight) {
            KeepWidth = 1 - KeepWidth;
            goto _CALC;
            }
        } else {
            UINT16 real_h = (UINT16)((float)DispDev.DeviceInfo.VoutHeight/DispDev.DeviceInfo.DevPixelAr);

            DBGMSGc2(GREEN,"[AppLib - Display] <calcPreviewWindowSize> keep height");
            prevParam->Preview.Height = DispDev.DeviceInfo.VoutHeight;
            prevParam->Preview.Y = 0;
            prevParam->Preview.Width = real_h*ArX/ArY;
#ifdef CONFIG_APP_ARD
            if(LcdVoutNeedRotate){
                prevParam->Preview.Width = (prevParam->Preview.Width) & 0xFFFC;
            }else{
#endif
            prevParam->Preview.Width = (prevParam->Preview.Width+3) & 0xFFFC;
#ifdef CONFIG_APP_ARD
            }
#endif
            // The video preview size should always be times of 16 to make AR consistent.
            //prevParam->Preview.Width = prevParam->Preview.Width & 0xfff0;
           // prevParam->Preview.Height = prevParam->Preview.Height & 0xfff0;
            prevParam->Preview.X = (DispDev.DeviceInfo.VoutWidth - prevParam->Preview.Width) / 2;

            // Result width is too big, use height to recalc constraint by width again.
            if (prevParam->Preview.Width > DispDev.DeviceInfo.VoutWidth) {
                KeepWidth = 1 - KeepWidth;
                goto _CALC;
            }
        }
    }

#ifdef CONFIG_APP_ARD
    if(LcdVoutNeedRotate){
        UINT32 tmp;
        tmp = prevParam->Preview.Width;
        prevParam->Preview.Width = prevParam->Preview.Height;
        prevParam->Preview.Height = tmp;

        tmp = prevParam->Preview.X;
        prevParam->Preview.X = prevParam->Preview.Y;
        prevParam->Preview.Y = tmp;

        if(prevParam->Preview.Y < 4){
            prevParam->Preview.Y = 4;
            prevParam->Preview.Height -= prevParam->Preview.Y;
        }
        prevParam->Preview.Height &= 0xFFFC;
    }
#endif

    DBGMSG("[AppLib - Display] preview_w=%d,preview_h=%d,vout_x=%d,vout_y=%d",prevParam->Preview.Width,prevParam->Preview.Height,prevParam->Preview.X,prevParam->Preview.Y);
}


