#include <AmbaSSP.h>

#include "AmbaDSP_VIN.h"
#include "AmbaVIN.h"
#include "AmbaI2C.h"
#include "AmbaGPIO.h"
#include "AmbaPLL.h"
#include "AmbaSensor.h"
#include "AmbaSensor_OV10823.h"

/*-----------------------------------------------------------------------------------------------*\
 * OV10823 sensor runtime info
\*-----------------------------------------------------------------------------------------------*/
OV10823_CTRL_s OV10823Ctrl = {0};

/*-----------------------------------------------------------------------------------------------*\
 * Configurations for Amba. VIN side
\*-----------------------------------------------------------------------------------------------*/
static AMBA_DSP_VIN_MIPI_CONFIG_s OV10823VinConfig = {
    .Info = {
        .FrameRate = {
            .Interlace      = 0,
            .TimeScale      = 0,
            .NumUnitsInTick = 0,
        },
        .DspPhaseShift      = {
            .Horizontal     =   AMBA_DSP_PHASE_SHIFT_MODE_0,
            .Vertical       =   AMBA_DSP_PHASE_SHIFT_MODE_0,
        },
        .BayerPattern       = AMBA_DSP_BAYER_BG,
        .NumDataBits        = 0,
        .NumSkipFrame       = 1,
    },
    .DataTypeMask           = 0x00,
    .DataType               = AMBA_DSP_VIN_MIPI_RAW10,
    .NumActiveLanes         = 4,
    .RxHvSyncCtrl           = {
        .NumActivePixels    = 0,
        .NumActiveLines     = 0,
        .NumTotalPixels     = 0,
        .NumTotalLines      = 0,
    },

    .VinVoutSync            = {
        {
            .SignalFreq = AMBA_DSP_VIN_VOUT_SYNC_FRAME,
            .SignalLine = 1,
        },
        {
            .SignalFreq = AMBA_DSP_VIN_VOUT_SYNC_FRAME,
            .SignalLine = 1,
        },
    },
    .MipiCtrl               = {
    .HsTermTime     = 8,   //max:16            [4:0]
    .HsSettleTime   = 51,  //min:28 select:51  [10:5]
    .RxInitTime     = 124, //min:20            [17:11]
    .ClkMissTime    = 6,   //max:12            [22:18]
    .ClkSettleTime  = 63,  //min:13 select:63  [28:23]
    .ClkTermTime    = 6,   //max:7
    }
};

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: OV10823_PrepareModeInfo
 *
 *  @Description:: Get sensor mode information
 *
 *  @Input      ::
 *      Mode:   Sensor readout mode number
 *
 *  @Output     ::
 *      pModeInfo:  Details of the specified readout mode
 *      pSensorPrivate: Sensor frame time configuration
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static void OV10823_PrepareModeInfo(UINT16 Mode, AMBA_SENSOR_MODE_INFO_s *pModeInfo, OV10823_FRAME_TIMING_s *pSensorPrivate)
{
    const AMBA_SENSOR_INPUT_INFO_s  *pInputInfo= &OV10823InputInfo[OV10823ModeInfoList[Mode].InputMode];
    const AMBA_SENSOR_OUTPUT_INFO_s *pOutputInfo= &OV10823OutputInfo[OV10823ModeInfoList[Mode].OutputMode];

    if (pSensorPrivate == NULL)
        pSensorPrivate = (OV10823_FRAME_TIMING_s *) &OV10823ModeInfoList[Mode].FrameTiming;
    else
        memcpy(pSensorPrivate, &OV10823ModeInfoList[Mode].FrameTiming, sizeof(OV10823_FRAME_TIMING_s));

    pModeInfo->Mode.Data = Mode;
    pModeInfo->LineLengthPck = OV10823ModeInfoList[Mode].FrameTiming.Linelengthpck;
    pModeInfo->FrameLengthLines = OV10823ModeInfoList[Mode].FrameTiming.FrameLengthLines;

    pModeInfo->NumExposureStepPerFrame = OV10823ModeInfoList[Mode].FrameTiming.FrameLengthLines;
    pModeInfo->PixelRate        = (float) pOutputInfo->DataRate * pOutputInfo->NumDataLanes / pOutputInfo->NumDataBits;
    pModeInfo->RowTime = (float) pModeInfo->LineLengthPck / pModeInfo->PixelRate;
    pModeInfo->InternalExposureOffset = 0;
    pModeInfo->FrameTime.InputClk = OV10823ModeInfoList[Mode].FrameTiming.InputClk;
    memcpy(&pModeInfo->FrameTime.FrameRate, &(OV10823ModeInfoList[Mode].FrameTiming.FrameRate), sizeof(AMBA_DSP_FRAME_RATE_s));
    memcpy(&pModeInfo->InputInfo, pInputInfo, sizeof(AMBA_SENSOR_INPUT_INFO_s));
    memcpy(&pModeInfo->OutputInfo, pOutputInfo, sizeof(AMBA_SENSOR_OUTPUT_INFO_s));
    memcpy(&pModeInfo->MinFrameRate, &(OV10823ModeInfoList[Mode].FrameTiming.FrameRate), sizeof(AMBA_DSP_FRAME_RATE_s));

    /* HDR information */
    memset(&pModeInfo->HdrInfo, 0, sizeof(AMBA_SENSOR_HDR_INFO_s)); // Not support HDR

    /* updated minimum frame rate limitation */
    /* slowshutter is not implmented yet */
#if 0
    if (NumXhsPerXvs >= pModeInfo->FrameLengthLines)
        pModeInfo->MinFrameRate.NumUnitsInTick *= 0x3FFF / NumXhsPerXvs;                /* pVinCfg->SyncCtrl.MasterSync.NumTotalLines = [0x000,0x3FFF] */
    else
        pModeInfo->MinFrameRate.NumUnitsInTick *= 0x3FFF / pModeInfo->FrameLengthLines; /* pVinCfg->SyncCtrl.SlaveSync.NumTotalLines = [0x000,0x3FFF] */
#endif
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: OV10823_ConfigVin
 *
 *  @Description:: Configure VIN to receieve output frames of the new readout mode
 *
 *  @Input      ::
 *      pModeInfo:  Details of the specified readout mode
 *      pFrameTime: Sensor frame time configuration
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static void OV10823_ConfigVin(AMBA_SENSOR_MODE_INFO_s *pModeInfo, OV10823_FRAME_TIMING_s *pFrameTime)
{
    AMBA_SENSOR_OUTPUT_INFO_s  *pImgInfo = &pModeInfo->OutputInfo;
    AMBA_DSP_VIN_MIPI_CONFIG_s *pVinCfg = &OV10823VinConfig;

    pVinCfg->NumActiveLanes = pImgInfo->NumDataLanes;
    pVinCfg->Info.NumDataBits = pImgInfo->NumDataBits;

    pVinCfg->RxHvSyncCtrl.NumActivePixels  = pImgInfo->OutputWidth;
    pVinCfg->RxHvSyncCtrl.NumActiveLines   = pImgInfo->OutputHeight;
    pVinCfg->RxHvSyncCtrl.NumTotalPixels   = pModeInfo->LineLengthPck - 1;
    pVinCfg->RxHvSyncCtrl.NumTotalLines    = pModeInfo->FrameLengthLines;

    pVinCfg->Info.BayerPattern = (AMBA_DSP_SENSOR_PATTERN_e) pImgInfo->CfaPattern;

    memcpy(&pVinCfg->Info.FrameRate, &pFrameTime->FrameRate, sizeof(AMBA_DSP_FRAME_RATE_s));
    memcpy(&pVinCfg->Info.DspPhaseShift, &pImgInfo->DspPhaseShift, sizeof(AMBA_DSP_PHASE_SHIFT_CTRL_s));

    AmbaDSP_VinConfigMIPI(AMBA_VIN_CHANNEL0, pVinCfg);
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: OV10823_HardwareReset
 *
 *  @Description:: Reset OV10823 Image Sensor Device
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static inline void OV10823_HardwareReset(void)
{
    AmbaUserGPIO_SensorResetCtrl(0);
    AmbaKAL_TaskSleep(3);
    AmbaUserGPIO_SensorResetCtrl(1);
}

static int OV10823_RegWrite(UINT16 Addr, UINT8 Data)
{
    UINT8 TxDataBuf[3];

    TxDataBuf[0] = (UINT8) ((Addr & 0xff00) >> 8);
    TxDataBuf[1] = (UINT8) (Addr & 0x00ff);
    TxDataBuf[2] = Data;

    if (AmbaI2C_Write(AMBA_I2C_CHANNEL0, AMBA_I2C_SPEED_FAST,
                      0x6c, 3, TxDataBuf,
                      AMBA_KAL_WAIT_FOREVER) == NG)
        AmbaPrint("I2C does not work!!!!!");

    //AmbaPrint("Addr = 0x%04x, Data = 0x%02x", Addr, Data);

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: OV10823_RegisterWrite
 *
 *  @Description:: Write sensor registers API
 *
 *  @Input      ::
 *      Chan:   Vin ID and sensor ID
 *      Addr:   Register address
 *      Data:   Register value
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *      int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static int OV10823_RegisterWrite(AMBA_DSP_CHANNEL_ID_u Chan, UINT16 Addr, UINT16 Data)
{
    UINT8 wData = (UINT8)Data;
    OV10823_RegWrite(Addr, wData);

    return OK;
}

static int OV10823_RegRead(UINT16 Addr, UINT8 *pRxData)
{
    UINT16 pTxData[4];

    pTxData[0] = AMBA_I2C_RESTART_FLAG | (0x6c);
    pTxData[1] = (Addr >> 8);    /* Register Address [15:8] */
    pTxData[2] = (Addr & 0xff);  /* Register Address [7:0]  */
    pTxData[3] = AMBA_I2C_RESTART_FLAG | (0x6d);

    return AmbaI2C_ReadAfterWrite(AMBA_I2C_CHANNEL0, AMBA_I2C_SPEED_STANDARD,
                                  4, (UINT16 *) pTxData, 1, pRxData, AMBA_KAL_WAIT_FOREVER);
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: OV10823_RegisterRead
 *
 *  @Description:: Read sensor registers API
 *
 *  @Input      ::
 *      Chan:   Vin ID and sensor ID
 *      Addr:   Register address
 *      Data:   Register value
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *      int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static int OV10823_RegisterRead(AMBA_DSP_CHANNEL_ID_u Chan, UINT16 Addr, UINT16 *Data)
{
    static UINT8 rData;
    OV10823_RegRead(Addr, &rData);
    *Data = rData;
  
    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: OV10823_SetStandbyOn
 *
 *  @Description:: Configure sensor into standby mode
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static void OV10823_SetStandbyOn(void)
{
    OV10823_RegWrite(0x0100, 0x0);
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: OV10823_SetStandbyOff
 *
 *  @Description:: Configure sensor to leave standby mode.
 *
 *  @Input      ::
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static void OV10823_SetStandbyOff(void)
{
    OV10823_RegWrite(0x0100, 0x1);
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: OV10823_ChangeReadoutMode
 *
 *  @Description:: Switch sensor to requested mode.
 *                 It's for the case that sensor have been initialized.
 *
 *  @Input      ::
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static int OV10823_ChangeReadoutMode(UINT16 Mode)
{
    int i = 0;

    for (i = 0; i < OV10823_NUM_READOUT_MODE_REG; i ++) {
        OV10823_RegWrite(OV10823RegTable[i].Addr, OV10823RegTable[i].Data[OV10823ModeInfoList[Mode].ReadoutMode]);
    }
#if 0
    for(i = 0; i < OV10823_NUM_READOUT_MODE_REG; i ++) {
        UINT8 Rval = 0;
        OV10823_RegRead(OV10823RegTable[i].Addr, &Rval);
        AmbaPrint("Reg Read: Addr: 0x%4x, Wdata:0x%2x, Rdata:0x%2x", OV10823RegTable[i].Addr, OV10823RegTable[i].Data[OV10823ModeInfoList[Mode].ReadoutMode], Rval);
    }
#endif
    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: OV10823_Init
 *
 *  @Description:: Initialize sensor driver
 *
 *  @Input      ::
 *      Chan:   Vin ID and sensor ID
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static int OV10823_Init(AMBA_DSP_CHANNEL_ID_u Chan)
{
    const OV10823_FRAME_TIMING_s *pFrameTime = &OV10823ModeInfoList[0].FrameTiming;
    AmbaPLL_SetSensorClk(pFrameTime->InputClk);             /* The default sensor input clock frequency */
    AmbaPLL_SetSensorClkDir(AMBA_PLL_SENSOR_CLK_OUTPUT);    /* Config clock output to sensor (or sensor SPI won't work) */

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: OV10823_Enable
 *
 *  @Description:: Power on sensor
 *
 *  @Input      ::
 *      Chan:   Vin ID and sensor ID
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static int OV10823_Enable(AMBA_DSP_CHANNEL_ID_u Chan)
{
    if (OV10823Ctrl.Status.DevStat.Bits.Sensor0Power == 1)
        return OK;

    OV10823_HardwareReset();

    /* config mipi phy*/
    AmbaDSP_VinPhySetMIPI(Chan.Bits.VinID);

    OV10823Ctrl.Status.DevStat.Bits.Sensor0Power   = 1;
    OV10823Ctrl.Status.DevStat.Bits.Sensor0Standby = 1;

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: OV10823_Disable
 *
 *  @Description:: Power down sensor
 *
 *  @Input      ::
 *      Chan:   Vin ID and sensor ID
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static int OV10823_Disable(AMBA_DSP_CHANNEL_ID_u Chan)
{
    if (OV10823Ctrl.Status.DevStat.Bits.Sensor0Power == 0)
        return OK;

    OV10823_SetStandbyOn();

    OV10823Ctrl.Status.DevStat.Bits.Sensor0Power = 0;

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: OV10823_ChangeFrameRate
 *
 *  @Description:: change frame rate of current sensor mode
 *
 *  @Input      ::
 *      Chan:   Vin ID and sensor ID
 *      pFrameRate: target frame rate
 *
 *  @Output     ::
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static int OV10823_ChangeFrameRate(AMBA_DSP_CHANNEL_ID_u Chan, AMBA_DSP_FRAME_RATE_s *pFrameRate)
{
    return NG;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: OV10823_SetGainFactor
 *
 *  @Description:: Set gain value by the combination of analog gain and digital gain
 *
 *  @Input      ::
 *      Chan:       Vin ID and sensor ID
 *      GainFactor: A ratio of the sensor gain (both analog and digital) output to the sensor gain input.
 *                  Minimum is 0.0 dB, Maximum 45.045771199653990948648128508689 dB
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static int OV10823_SetGainFactor(AMBA_DSP_CHANNEL_ID_u Chan, float GainFactor)
{
    return NG;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: OV10823_SetEshutterSpeed
 *
 *  @Description:: Set sensor eshutter electronic shutter speed
 *
 *  @Input      ::
 *      Chan:   Vin ID and sensor ID
 *      EshutterSpeed:  Electronic shutter speed value (in unit of seconds)
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static int OV10823_SetEshutterSpeed(AMBA_DSP_CHANNEL_ID_u Chan, float ShutterSpeed)
{
    return NG;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: OV10823_GetStatus
 *
 *  @Description:: Get current sensor status
 *
 *  @Input      ::
 *      Chan:   Vin ID and sensor ID
 *
 *  @Output     ::
 *      pStatus:    pointer to current sensor status
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static int OV10823_GetStatus(AMBA_DSP_CHANNEL_ID_u Chan, AMBA_SENSOR_STATUS_INFO_s *pStatus)
{
    if (pStatus == NULL || OV10823Ctrl.Status.DevStat.Bits.Sensor0Power == 0)
        return NG;

    OV10823Ctrl.Status.TimeToAllPelExposed = 0;  /* TODO: for flash light or mechanical shutter control */

    memcpy(pStatus, &OV10823Ctrl.Status, sizeof(AMBA_SENSOR_STATUS_INFO_s));

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: OV10823_GetModeInfo
 *
 *  @Description:: Get Mode Info of indicated mode
 *
 *  @Input      ::
 *      Chan:   Vin ID and sensor ID
 *      Mode:   Sensor Readout Mode Number
 *
 *  @Output     ::
 *      pModeInfo: pointer to requested Mode Info
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static int OV10823_GetModeInfo(AMBA_DSP_CHANNEL_ID_u Chan, AMBA_SENSOR_MODE_ID_u Mode, AMBA_SENSOR_MODE_INFO_s *pModeInfo)
{
    UINT16 SensorMode = Mode.Bits.Mode; // TODO

    if (SensorMode == AMBA_SENSOR_CURRENT_MODE)
        SensorMode = OV10823Ctrl.Status.ModeInfo.Mode.Bits.Mode;

    if (SensorMode >= AMBA_SENSOR_OV10823_NUM_MODE || pModeInfo == NULL)
        return NG;

    OV10823_PrepareModeInfo(SensorMode, pModeInfo, NULL);

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: OV10823_GetDeviceInfo
 *
 *  @Description:: Get Sensor Device Info
 *
 *  @Input      ::
 *      Chan:   Vin ID and sensor ID
 *
 *  @Output     ::
 *      pDeviceInfo:    pointer to device info
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static int OV10823_GetDeviceInfo(AMBA_DSP_CHANNEL_ID_u Chan, AMBA_SENSOR_DEVICE_INFO_s *pDeviceInfo)
{
    if (pDeviceInfo == NULL)
        return NG;

    memcpy(pDeviceInfo, &OV10823DeviceInfo, sizeof(AMBA_SENSOR_DEVICE_INFO_s));

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: OV10823_ConvertGainFactor
 *
 *  @Description:: Convert gain factor to analog and digital gain control
 *
 *  @Input      ::
 *      Chan:               Vin ID and sensor ID
 *      DesiredFactor:      Desired gain factor
 *
 *  @Output     ::
 *      pActualFactor:      Achievable gain factor (12.20 fixed point)
 *      pAnalogGainCtrl:    Analog gain control for achievable gain factor
 *      pDigitalGainCtrl:   Digital gain control for achievable gain factor
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static int OV10823_ConvertGainFactor(AMBA_DSP_CHANNEL_ID_u Chan, float DesiredFactor, UINT32 *pActualFactor, UINT32 *pAnalogGainCtrl, UINT32 *pDigitalGainCtrl)
{
    float WbFactor = 0.0;
    UINT8 AgcR350a = 0;
    UINT8 AgcR350b = 0;
    UINT8 WbMsb = 0;
    UINT8 WbLsb = 0;

    //AmbaPrint("DesiredFactor;%f",DesiredFactor);
    if (DesiredFactor > 15.9375) {
        AgcR350a = 0x00;
        AgcR350b = 0xff;
        WbFactor = DesiredFactor / 15.9375;
        *pActualFactor = (UINT32) 1024 * 15.9375;
    } else {
        AgcR350a = 0x00;
        AgcR350b = (UINT8)(DesiredFactor * 16);
        *pActualFactor = (UINT32) 1024 * AgcR350b / 16;
    }
    
    /* Set WB gain */
    if (WbFactor > 1) {
        UINT16 WbGainFactor = (UINT16)(1024 * (WbFactor - 1) + 1024);
        WbGainFactor = (WbGainFactor > 0x0fff) ? 0x0fff : WbGainFactor;  // Max Wb gain
        WbMsb = (WbGainFactor & 0xff00) >> 8;
        WbLsb = WbGainFactor & 0x00ff;
        *pActualFactor = (*pActualFactor) * WbGainFactor;
    } else {
        WbMsb = 0x04;
        WbLsb = 0x00;
        *pActualFactor = (*pActualFactor) * 1024;
    }

    *pAnalogGainCtrl  = (AgcR350a << 8) + AgcR350b;
    *pDigitalGainCtrl = (WbMsb << 8) + WbLsb;

    //AmbaPrint("AgcR350a = 0x%02x, AgcR350b = 0x%02x, WbMsb = 0x%02x, WbLsb = 0x%02x, ActualFactor = %d, AnalogGainCtrl:0x%04x, DigitalGainCtrl:0x%04x", AgcR350a, AgcR350b,WbMsb,WbLsb,*pActualFactor,*pAnalogGainCtrl,*pDigitalGainCtrl);

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: OV10823_ConvertShutterSpeed
 *
 *  @Description:: Convert exposure time to shutter control
 *
 *  @Input      ::
 *      Chan:           Vin ID and sensor ID
 *      ExposureTime:   Exposure time / shutter speed
 *
 *  @Output     ::
 *      pShutterCtrl:   Electronic shutter control
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static int OV10823_ConvertShutterSpeed(AMBA_DSP_CHANNEL_ID_u Chan, float ExposureTime, UINT32 *pShutterCtrl)
{
    AMBA_SENSOR_MODE_INFO_s *pModeInfo = &OV10823Ctrl.Status.ModeInfo;

    if (pShutterCtrl == NULL)
        return NG;

    *pShutterCtrl = (UINT32)(ExposureTime / pModeInfo->RowTime);

    /* Exposure line needs be smaller than VTS - 8 */
    if (*pShutterCtrl >= pModeInfo->NumExposureStepPerFrame - 8)
        *pShutterCtrl = pModeInfo->NumExposureStepPerFrame - 8;

    if (*pShutterCtrl <= 1)
        *pShutterCtrl = 1;

    //AmbaPrint("ShutterCtrl:%d",*pShutterCtrl);
    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: OV10823_SetAnalogGainCtrl
 *
 *  @Description:: Set analog gain control
 *
 *  @Input      ::
 *      Chan:               Vin ID and sensor ID
 *      AnalogGainCtrl:     Analog gain control for achievable gain factor
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static int OV10823_SetAnalogGainCtrl(AMBA_DSP_CHANNEL_ID_u Chan, UINT32 AnalogGainCtrl)
{
    //UINT8 AgcR350a = (AnalogGainCtrl & 0x0000ff00) >> 8;
    UINT8 AgcR350b = (AnalogGainCtrl & 0x000000ff);

    //OV10823_RegWrite(OV10823_GROUP_ACCESS_REG, 0x00);

    /* For long exposure frame */
    //OV10823_RegWrite(OV10823_AGC_MSB_REG, AgcR350a);
    OV10823_RegWrite(OV10823_AGC_LSB_REG, AgcR350b);

    //OV10823_RegWrite(OV10823_GROUP_ACCESS_REG, 0x10);
    //OV10823_RegWrite(OV10823_GROUP_ACCESS_REG, 0xa0);

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: OV10823_SetDigitalGainCtrl
 *
 *  @Description:: Set digital gain control
 *
 *  @Input      ::
 *      Chan:               Vin ID and sensor ID
 *      DigitalGainCtrl:    Digital gain control for achievable gain factor
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static int OV10823_SetDigitalGainCtrl(AMBA_DSP_CHANNEL_ID_u Chan, UINT32 DigitalGainCtrl)
{
    UINT8 WbMsb = (DigitalGainCtrl & 0x0000ff00) >> 8;
    UINT8 WbLsb = (DigitalGainCtrl & 0x000000ff);

    OV10823_RegWrite(OV10823_GROUP_ACCESS_REG, 0x00);
    /* WB R-channel */
    OV10823_RegWrite(OV10823_WB_RED_MSB_REG, WbMsb); 
    OV10823_RegWrite(OV10823_WB_RED_LSB_REG, WbLsb); 

    /* WB G-channel */
    OV10823_RegWrite(OV10823_WB_GREEN_MSB_REG, WbMsb); 
    OV10823_RegWrite(OV10823_WB_GREEN_LSB_REG, WbLsb); 

    /* WB B-channel */
    OV10823_RegWrite(OV10823_WB_BLUE_MSB_REG, WbMsb); 
    OV10823_RegWrite(OV10823_WB_BLUE_LSB_REG, WbLsb);

    OV10823_RegWrite(OV10823_GROUP_ACCESS_REG, 0x10);
    OV10823_RegWrite(OV10823_GROUP_ACCESS_REG, 0xa0);

    return OK;
}


/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: OV10823_SetShutterCtrl
 *
 *  @Description:: set shutter control
 *
 *  @Input      ::
 *      Chan:           Vin ID and sensor ID
 *      ShutterCtrl:    Electronic shutter control
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static int OV10823_SetShutterCtrl(AMBA_DSP_CHANNEL_ID_u Chan, UINT32 ShutterCtrl)
{
    ShutterCtrl = ShutterCtrl << 4;

    //OV10823_RegWrite(OV10823_GROUP_ACCESS_REG, 0x00);    // Enable group 0

    OV10823_RegWrite(OV10823_EXP_MSB_REG, (ShutterCtrl & 0x000f0000) >> 16);
    OV10823_RegWrite(OV10823_EXP_MID_REG, (ShutterCtrl & 0x0000ff00) >> 8);
    OV10823_RegWrite(OV10823_EXP_LSB_REG, (ShutterCtrl & 0x000000ff) >> 0);

    //OV10823_RegWrite(OV10823_GROUP_ACCESS_REG, 0x10);    // End group 0
    //OV10823_RegWrite(OV10823_GROUP_ACCESS_REG, 0xA0);    // Launch group 0
    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: OV10823_SetSlowShutterCtrl
 *
 *  @Description:: set slow shutter control
 *
 *  @Input      ::
 *      Chan:           Vin ID and sensor ID
 *      pSlowShutterCtrl:   Electronic slow shutter control
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static int OV10823_SetSlowShutterCtrl(AMBA_DSP_CHANNEL_ID_u Chan, UINT32 SlowShutterCtrl)
{
    OV10823Ctrl.Status.ModeInfo.FrameTime.FrameRate.NumUnitsInTick =
        OV10823ModeInfoList[OV10823Ctrl.Status.ModeInfo.Mode.Bits.Mode].FrameTiming.FrameRate.NumUnitsInTick * SlowShutterCtrl;
    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: OV10823_Config
 *
 *  @Description:: Set sensor to indicated mode
 *
 *  @Input      ::
 *      Chan:   Vin ID and sensor ID
 *      Mode:   Sensor Readout Mode Number
 *      ElecShutterMode:    Operating Mode of the Electronic Shutter
 *
 *  @Output     ::
 *      pModeInfo: pointer to mode info of target mode
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static int OV10823_Config(AMBA_DSP_CHANNEL_ID_u Chan, AMBA_SENSOR_MODE_ID_u Mode, AMBA_SENSOR_ESHUTTER_TYPE_e ElecShutterMode)
{
    AMBA_SENSOR_MODE_INFO_s *pModeInfo = NULL;
    OV10823_FRAME_TIMING_s *pFrameTime = NULL;
    UINT16 SensorMode = Mode.Bits.Mode;

    if (SensorMode >= AMBA_SENSOR_OV10823_NUM_MODE)
        return NG;

    AmbaPrint("============  [ OV10823 ]  ============");
    AmbaPrint("============ init mode:%d ============", SensorMode);

    OV10823Ctrl.Status.ElecShutterMode = ElecShutterMode;

    pModeInfo  = &OV10823Ctrl.Status.ModeInfo;
    pFrameTime = &OV10823Ctrl.FrameTime;
    OV10823_PrepareModeInfo(SensorMode, pModeInfo, pFrameTime);
#if 1
    /* Adjust mipi-phy parameters */
    AmbaVIN_CalculateMphyConfig(pModeInfo->OutputInfo.DataRate, &OV10823VinConfig.MipiCtrl);
#endif
    /* Make VIN not to capture bad frames during readout mode transition */
    AmbaVIN_Reset(AMBA_VIN_CHANNEL0, AMBA_VIN_MIPI, &OV10823VinConfig.MipiCtrl);

    AmbaKAL_TaskSleep(3);

    AmbaPLL_SetSensorClk(pModeInfo->FrameTime.InputClk);

    AmbaKAL_TaskSleep(3);


    OV10823_RegWrite(OV10823_SOFT_RESET_REG, 0x01);
    AmbaKAL_TaskSleep(20);

    OV10823_ChangeReadoutMode(SensorMode);

    OV10823_SetStandbyOff();

    OV10823_ConfigVin(pModeInfo, pFrameTime);

    OV10823Ctrl.Status.DevStat.Bits.Sensor0Standby = 0;

    return OK;
}

/*-----------------------------------------------------------------------------------------------*/
AMBA_SENSOR_OBJ_s AmbaSensor_OV10823Obj = {
    .Init                   = OV10823_Init,
    .Enable                 = OV10823_Enable,
    .Disable                = OV10823_Disable,
    .Config                 = OV10823_Config,
    .ChangeFrameRate        = OV10823_ChangeFrameRate,
    .GetStatus              = OV10823_GetStatus,
    .GetModeInfo            = OV10823_GetModeInfo,
    .GetDeviceInfo          = OV10823_GetDeviceInfo,
    .GetCurrentGainFactor   = NULL,
    .GetCurrentShutterSpeed = NULL,

    .ConvertGainFactor      = OV10823_ConvertGainFactor,
    .ConvertShutterSpeed    = OV10823_ConvertShutterSpeed,

    .SetAnalogGainCtrl      = OV10823_SetAnalogGainCtrl,
    .SetDigitalGainCtrl     = OV10823_SetDigitalGainCtrl,
    .SetShutterCtrl         = OV10823_SetShutterCtrl,
    .SetSlowShutterCtrl     = OV10823_SetSlowShutterCtrl,

    .SetGainFactor          = OV10823_SetGainFactor,
    .SetEshutterSpeed       = OV10823_SetEshutterSpeed,

    .RegisterRead           = OV10823_RegisterRead,
    .RegisterWrite          = OV10823_RegisterWrite,
};
