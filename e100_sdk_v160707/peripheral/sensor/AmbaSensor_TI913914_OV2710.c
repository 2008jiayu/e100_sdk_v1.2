
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "AmbaDataType.h"
#include "AmbaKAL.h"

#include "AmbaDSP_VIN.h"
#include "AmbaVIN.h"
#include "AmbaI2C.h"
#include "AmbaGPIO.h"
#include "AmbaPLL.h"
#include "AmbaSensor.h"
#include "AmbaSensor_TI913914_OV2710.h"
#include "AmbaSbrg_DevDrv.h"
#include "AmbaSbrg_DevList.h"


#include "AmbaPrintk.h"

#define I2C_PASSBY_SERDES


/*-----------------------------------------------------------------------------------------------*\
 * OV2710 sensor runtime info
\*-----------------------------------------------------------------------------------------------*/
OV2710_CTRL_s OV2710Ctrl = {0};

/*-----------------------------------------------------------------------------------------------*\
 * Configurations for Amba. VIN side
\*-----------------------------------------------------------------------------------------------*/
static AMBA_DSP_VIN_DVP_CONFIG_s OV2710VinConfig = {
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
        .NumDataBits        = 10,
        .NumSkipFrame       = 1,
    },

    .DvpType               = AMBA_DSP_VIN_DVP_SINGLE_PEL_SDR,
    .SyncType               = AMBA_DSP_VIN_SYNC_BT601,
    .DataClockEdge          = AMBA_DSP_VIN_SIGNAL_RISING_EDGE, //From China team
    .HsyncPolarity          = AMBA_DSP_VIN_SIGNAL_RISING_EDGE,
    .VsyncPolarity          = AMBA_DSP_VIN_SIGNAL_FALLING_EDGE, //From China team
    .FieldPolarity          = AMBA_DSP_VIN_SIGNAL_RISING_EDGE,
    .SyncPinSelect  = {
        .FieldPinSelect = 0,
#ifdef CONFIG_BSP_LIBRA
		.VsyncPinSelect = 21,
		.HsyncPinSelect = 20,
#else		
		.VsyncPinSelect = 15,
		.HsyncPinSelect = 14,
#endif		

    },
    .RxHvSyncCtrl   = {0, 0, 0, 0},
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

};

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: OV2710_PrepareModeInfo
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
static void OV2710_PrepareModeInfo(UINT16 Mode, AMBA_SENSOR_MODE_INFO_s *pModeInfo, OV2710_FRAME_TIMING_s *pSensorPrivate)
{
    const AMBA_SENSOR_INPUT_INFO_s  *pInputInfo= &OV2710InputInfo[OV2710ModeInfoList[Mode].InputMode];
    const AMBA_SENSOR_OUTPUT_INFO_s *pOutputInfo= &OV2710OutputInfo[OV2710ModeInfoList[Mode].OutputMode];

    if (pSensorPrivate == NULL)
        pSensorPrivate = (OV2710_FRAME_TIMING_s *) &OV2710ModeInfoList[Mode].FrameTiming;
    else
        memcpy(pSensorPrivate, &OV2710ModeInfoList[Mode].FrameTiming, sizeof(OV2710_FRAME_TIMING_s));

    pModeInfo->Mode.Data = Mode;
    pModeInfo->LineLengthPck = OV2710ModeInfoList[Mode].FrameTiming.Linelengthpck;
    pModeInfo->FrameLengthLines = OV2710ModeInfoList[Mode].FrameTiming.FrameLengthLines;

    //AmbaPrint("LineLengthPck:%d, FrameLengthLines:%d",pModeInfo->LineLengthPck, pModeInfo->FrameLengthLines);

    pModeInfo->NumExposureStepPerFrame = OV2710ModeInfoList[Mode].FrameTiming.FrameLengthLines;
    pModeInfo->PixelRate        = (float)((float)pModeInfo->LineLengthPck * (float)pModeInfo->FrameLengthLines *
        (float)((float)OV2710ModeInfoList[Mode].FrameTiming.FrameRate.TimeScale / (float)OV2710ModeInfoList[Mode].FrameTiming.FrameRate.NumUnitsInTick));
    pModeInfo->RowTime = (float) (pModeInfo->LineLengthPck / pModeInfo->PixelRate);
    pModeInfo->InternalExposureOffset = 0;
    pModeInfo->FrameTime.InputClk = OV2710ModeInfoList[Mode].FrameTiming.InputClk;

    //AmbaPrint("PixelRate:%f, RowTime:%f",pModeInfo->PixelRate, pModeInfo->RowTime);

    memcpy(&pModeInfo->FrameTime.FrameRate, &(OV2710ModeInfoList[Mode].FrameTiming.FrameRate), sizeof(AMBA_DSP_FRAME_RATE_s));
    memcpy(&pModeInfo->InputInfo, pInputInfo, sizeof(AMBA_SENSOR_INPUT_INFO_s));
    memcpy(&pModeInfo->OutputInfo, pOutputInfo, sizeof(AMBA_SENSOR_OUTPUT_INFO_s));
    memcpy(&pModeInfo->MinFrameRate, &(OV2710ModeInfoList[Mode].FrameTiming.FrameRate), sizeof(AMBA_DSP_FRAME_RATE_s));

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
 *  @RoutineName:: OV2710_ConfigVin
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
static void OV2710_ConfigVin(AMBA_SENSOR_MODE_INFO_s *pModeInfo, OV2710_FRAME_TIMING_s *pFrameTime)
{
    AMBA_SENSOR_OUTPUT_INFO_s  *pImgInfo = &pModeInfo->OutputInfo;
    AMBA_DSP_VIN_DVP_CONFIG_s *pVinCfg = &OV2710VinConfig;

    /* config dvp phy*/
    AmbaDSP_VinPhySetDVP(AMBA_VIN_CHANNEL0);

    pVinCfg->Info.NumDataBits = pImgInfo->NumDataBits;

    pVinCfg->RxHvSyncCtrl.NumActivePixels  = pImgInfo->OutputWidth;
    pVinCfg->RxHvSyncCtrl.NumActiveLines   = pImgInfo->OutputHeight;
    pVinCfg->RxHvSyncCtrl.NumTotalPixels   = pModeInfo->LineLengthPck - 1;
    pVinCfg->RxHvSyncCtrl.NumTotalLines    = pModeInfo->FrameLengthLines;

    pVinCfg->Info.BayerPattern = (AMBA_DSP_SENSOR_PATTERN_e) pImgInfo->CfaPattern;

    memcpy(&pVinCfg->Info.FrameRate, &pFrameTime->FrameRate, sizeof(AMBA_DSP_FRAME_RATE_s));
    memcpy(&pVinCfg->Info.DspPhaseShift, &pImgInfo->DspPhaseShift, sizeof(AMBA_DSP_PHASE_SHIFT_CTRL_s));

    AmbaDSP_VinConfigDVP(AMBA_VIN_CHANNEL0, pVinCfg);
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: OV2710_HardwareReset
 *
 *  @Description:: Reset OV2710 Image Sensor Device
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static inline void OV2710_HardwareReset(void)
{
    AmbaUserGPIO_SensorResetCtrl(0);
    AmbaKAL_TaskSleep(3);
    AmbaUserGPIO_SensorResetCtrl(1);
}

static int OV2710_RegWrite(UINT16 Addr, UINT8 Data)
{
    UINT8 TxDataBuf[3];

    TxDataBuf[0] = (UINT8) ((Addr & 0xff00) >> 8);
    TxDataBuf[1] = (UINT8) (Addr & 0x00ff);
    TxDataBuf[2] = Data;

    if (AmbaI2C_Write(AMBA_I2C_CHANNEL0, AMBA_I2C_SPEED_STANDARD,
                      OV2710_SLAVE_ADDR_WR, 3, TxDataBuf,
                      AMBA_KAL_WAIT_FOREVER) == NG)
        AmbaPrint("I2C does not work!!!!!");

    //AmbaPrint("Addr = 0x%04x, Data = 0x%02x", Addr, Data);

    return OK;
}
/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: OV2710_RegisterWrite
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
static int OV2710_RegisterWrite(AMBA_DSP_CHANNEL_ID_u Chan, UINT16 Addr, UINT16 Data)
{
	UINT8 wData = (UINT8)Data;
    OV2710_RegWrite(Addr, wData);

	return OK;
}


static int OV2710_RegRead(UINT16 Addr, UINT8 *pRxData)
{
    UINT16 pTxData[4];

    pTxData[0] = AMBA_I2C_RESTART_FLAG | (OV2710_SLAVE_ADDR_WR);
    pTxData[1] = (Addr >> 8);    /* Register Address [15:8] */
    pTxData[2] = (Addr & 0xff);  /* Register Address [7:0]  */
    pTxData[3] = AMBA_I2C_RESTART_FLAG | (OV2710_SLAVE_ADDR_RD);

    return AmbaI2C_ReadAfterWrite(AMBA_I2C_CHANNEL0, AMBA_I2C_SPEED_STANDARD,
                                  4, (UINT16 *) pTxData, 1, pRxData, AMBA_KAL_WAIT_FOREVER);
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: OV2710_RegisterRead
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
static int OV2710_RegisterRead(AMBA_DSP_CHANNEL_ID_u Chan, UINT16 Addr, UINT16 *Data)
{
	static UINT8 rData;
    OV2710_RegRead(Addr, &rData);
	*Data = rData;

	return OK;
}


#if 0
/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: OV2710_SetStandbyOn
 *
 *  @Description:: Configure sensor into standby mode
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static void OV2710_SetStandbyOn(void)
{
    //OV2710_RegWrite(0x0100, 0x0);
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: OV2710_SetStandbyOff
 *
 *  @Description:: Configure sensor to leave standby mode.
 *
 *  @Input      ::
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static void OV2710_SetStandbyOff(void)
{
    //OV2710_RegWrite(0x0100, 0x1);
}
#endif
/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: OV2710_ChangeReadoutMode
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
static int OV2710_ChangeReadoutMode(UINT16 Mode)
{
    int i = 0;

    for (i = 0; i < OV2710_NUM_READOUT_MODE_REG; i ++) {
        OV2710_RegWrite(OV2710RegTable[i].Addr, OV2710RegTable[i].Data[Mode]);
    }
#if 0
    for(i = 0; i < OV2710_NUM_READOUT_MODE_REG; i ++) {
        UINT8 Rval = 0;
        OV2710_RegRead(OV2710RegTable[i].Addr, &Rval);
        AmbaPrint("Reg Read: Addr: 0x%x, data: 0x%x", OV2710RegTable[i].Addr, Rval);
    }
#endif
    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: OV2710_Init
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
static int OV2710_Init(AMBA_DSP_CHANNEL_ID_u Chan)
{
    const OV2710_FRAME_TIMING_s *pFrameTime = &OV2710ModeInfoList[0].FrameTiming;
    AmbaPLL_SetSensorClk(pFrameTime->InputClk);             /* The default sensor input clock frequency */
    AmbaPLL_SetSensorClkDir(AMBA_PLL_SENSOR_CLK_OUTPUT);    /* Config clock output to sensor (or sensor SPI won't work) */

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: OV2710_Enable
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
static int OV2710_Enable(AMBA_DSP_CHANNEL_ID_u Chan)
{
    if (OV2710Ctrl.Status.DevStat.Bits.Sensor0Power == 1)
        return OK;

    OV2710_HardwareReset();

    /* config dvp phy*/
    AmbaDSP_VinPhySetDVP(Chan.Bits.VinID);

    OV2710Ctrl.Status.DevStat.Bits.Sensor0Power   = 1;
    OV2710Ctrl.Status.DevStat.Bits.Sensor0Standby = 1;

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: OV2710_Disable
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
static int OV2710_Disable(AMBA_DSP_CHANNEL_ID_u Chan)
{
    if (OV2710Ctrl.Status.DevStat.Bits.Sensor0Power == 0)
        return OK;

    //OV2710_SetStandbyOn();

    OV2710Ctrl.Status.DevStat.Bits.Sensor0Power = 0;

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: OV2710_ChangeFrameRate
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
static int OV2710_ChangeFrameRate(AMBA_DSP_CHANNEL_ID_u Chan, AMBA_DSP_FRAME_RATE_s *pFrameRate)
{
    return NG;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: OV2710_SetGainFactor
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
static int OV2710_SetGainFactor(AMBA_DSP_CHANNEL_ID_u Chan, float GainFactor)
{
    return NG;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: OV2710_SetEshutterSpeed
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
static int OV2710_SetEshutterSpeed(AMBA_DSP_CHANNEL_ID_u Chan, float ShutterSpeed)
{
    return NG;
}

static int OV2710_InitSeq(UINT32 SensorMode)
{
    int i = 0;

    for (i = 0; i < OV2710_NUM_INIT_MODE_REG; i ++) {
        OV2710_RegWrite(OV2710InitRegTable[i].Addr, OV2710InitRegTable[i].Data);
    }
#if 0
    for(i = 0; i < OV2710_NUM_READOUT_MODE_REG; i ++) {
        UINT8 Rval = 0;
        OV2710_RegRead(OV2710RegTable[i].Addr, &Rval);
        AmbaPrint("Reg Read: Addr: 0x%x, data: 0x%x", OV2710RegTable[i].Addr, Rval);
    }
#endif

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: OV2710_GetStatus
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
static int OV2710_GetStatus(AMBA_DSP_CHANNEL_ID_u Chan, AMBA_SENSOR_STATUS_INFO_s *pStatus)
{
    if (pStatus == NULL || OV2710Ctrl.Status.DevStat.Bits.Sensor0Power == 0)
        return NG;

    OV2710Ctrl.Status.TimeToAllPelExposed = 0;  /* TODO: for flash light or mechanical shutter control */

    memcpy(pStatus, &OV2710Ctrl.Status, sizeof(AMBA_SENSOR_STATUS_INFO_s));

	//AmbaPrint("[OV2710_GetStatus]NumUnitsInTick:%d, TimeScale:%d",OV2710Ctrl.Status.ModeInfo.FrameTime.FrameRate.NumUnitsInTick,OV2710Ctrl.Status.ModeInfo.FrameTime.FrameRate.TimeScale);

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: OV2710_GetModeInfo
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
static int OV2710_GetModeInfo(AMBA_DSP_CHANNEL_ID_u Chan, AMBA_SENSOR_MODE_ID_u Mode, AMBA_SENSOR_MODE_INFO_s *pModeInfo)
{
    UINT16 SensorMode = Mode.Bits.Mode; // TODO

    if (SensorMode == AMBA_SENSOR_CURRENT_MODE)
        SensorMode = OV2710Ctrl.Status.ModeInfo.Mode.Bits.Mode;

    if (SensorMode >= AMBA_SENSOR_OV2710_NUM_MODE || pModeInfo == NULL)
        return NG;

    OV2710_PrepareModeInfo(SensorMode, pModeInfo, NULL);
	//AmbaPrint("[OV2710_GetModeInfo]NumUnitsInTick:%d, TimeScale:%d",pModeInfo->FrameTime.FrameRate.NumUnitsInTick,pModeInfo->FrameTime.FrameRate.TimeScale);


    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: OV2710_GetDeviceInfo
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
static int OV2710_GetDeviceInfo(AMBA_DSP_CHANNEL_ID_u Chan, AMBA_SENSOR_DEVICE_INFO_s *pDeviceInfo)
{
    if (pDeviceInfo == NULL)
        return NG;

    memcpy(pDeviceInfo, &OV2710DeviceInfo, sizeof(AMBA_SENSOR_DEVICE_INFO_s));

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: OV2710_ConvertGainFactor
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
static int OV2710_ConvertGainFactor(AMBA_DSP_CHANNEL_ID_u Chan, float DesiredFactor, UINT32 *pActualFactor, UINT32 *pAnalogGainCtrl, UINT32 *pDigitalGainCtrl)
{
    UINT8 Reg350A_H  = 0;
    UINT8 Reg350A_L  = 0;

    if (DesiredFactor >= 62.0) {
        *pDigitalGainCtrl = 2;  // Max Dgc
        Reg350A_H = 15;
        Reg350A_L = 15;
    } else if (DesiredFactor >= 32.0) {
        *pDigitalGainCtrl = 2;
        Reg350A_H = 15;
        Reg350A_L = ((DesiredFactor - 32) / 2);
    } else if (DesiredFactor >= 16.0) {
        *pDigitalGainCtrl = 1;
        Reg350A_H = 15;
        Reg350A_L = DesiredFactor - 16;
    } else if (DesiredFactor >= 8.0) {
        *pDigitalGainCtrl = 1;
        Reg350A_H = 7;
        Reg350A_L = ((DesiredFactor - 8) * 2);
    } else if (DesiredFactor >= 4.0) {
        *pDigitalGainCtrl = 1;
        Reg350A_H = 3;
        Reg350A_L = ((DesiredFactor - 4) * 4);
    } else if (DesiredFactor >= 2.0) {
        *pDigitalGainCtrl = 1;
        Reg350A_H = 1;
        Reg350A_L = ((DesiredFactor - 2) * 8);
    } else if (DesiredFactor >= 1.0) {
        *pDigitalGainCtrl = 1;
        Reg350A_H = 0;
        Reg350A_L = ((DesiredFactor - 1) * 16);
    }

    /*Both analog and digital gain set by analog gain set function*/
    *pAnalogGainCtrl  = ((*pDigitalGainCtrl - 1) << 8) + (Reg350A_H << 4) + Reg350A_L;
    *pActualFactor = (UINT32)((1<<20) * ((*pDigitalGainCtrl) * (((Reg350A_H >> 3) & 0x1) + 1) * (((Reg350A_H >> 2) & 0x1) + 1) * (((Reg350A_H >> 1) & 0x1) + 1) * ((Reg350A_H & 0x1) + 1) * (float)((Reg350A_L + 16) / 16.0)));

	//AmbaPrint("%d*%d*%d*%d*%d*%f",*pDigitalGainCtrl,(((Reg350A_H >> 3) & 0x1) + 1),(((Reg350A_H >> 2) & 0x1) + 1),((Reg350A_H >> 1) & 0x1 + 1),((Reg350A_H & 0x1) + 1),(float)((Reg350A_L + 16) / 16.0) );
    //AmbaPrint("Reg350A_H:%d, Reg350A_L:%d",Reg350A_H,Reg350A_L);
    //AmbaPrint("DesiredFactor:%f, ActualFactor:%d, AnalogGainCtrl:%d, DigitalGainCtrl:%d,", DesiredFactor, *pActualFactor, *pAnalogGainCtrl, *pDigitalGainCtrl);

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: OV2710_ConvertShutterSpeed
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
static int OV2710_ConvertShutterSpeed(AMBA_DSP_CHANNEL_ID_u Chan, float ExposureTime, UINT32 *pShutterCtrl)
{
    AMBA_SENSOR_MODE_INFO_s *pModeInfo = &OV2710Ctrl.Status.ModeInfo;

    if (pShutterCtrl == NULL)
        return NG;

    *pShutterCtrl = (UINT32)(ExposureTime / pModeInfo->RowTime);

    /* Exposure line needs be smaller than VTS - 14 */
    if (*pShutterCtrl >= pModeInfo->NumExposureStepPerFrame - 14)
        *pShutterCtrl = pModeInfo->NumExposureStepPerFrame - 14;

    if (*pShutterCtrl <= 1)
        *pShutterCtrl = 1;

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: OV2710_SetAnalogGainCtrl
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
static int OV2710_SetAnalogGainCtrl(AMBA_DSP_CHANNEL_ID_u Chan, UINT32 AnalogGainCtrl)
{
    UINT8 AgcR350A =  ((AnalogGainCtrl & 0x100) >> 8);
    UINT8 AgcR350B =  (AnalogGainCtrl & 0xff);

    OV2710_RegWrite(OV2710_GROUP_REG, 0x00); //Enable group 0

    OV2710_RegWrite(OV2710_AGC_REG_HIGH, AgcR350A);
    OV2710_RegWrite(OV2710_AGC_REG_LOW, AgcR350B);

    OV2710_RegWrite(OV2710_GROUP_REG, 0x10); //End group 0
    OV2710_RegWrite(OV2710_GROUP_REG, 0xA0); //Launch group 0

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: OV2710_SetDigitalGainCtrl
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
static int OV2710_SetDigitalGainCtrl(AMBA_DSP_CHANNEL_ID_u Chan, UINT32 DigitalGainCtrl)
{
    return OK;
}


/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: OV2710_SetShutterCtrl
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
static int OV2710_SetShutterCtrl(AMBA_DSP_CHANNEL_ID_u Chan, UINT32 ShutterCtrl)
{
	ShutterCtrl = ShutterCtrl << 4;
    OV2710_RegWrite(OV2710_EXPO_REG_HIGH, (ShutterCtrl & 0x000f0000) >> 16);
    OV2710_RegWrite(OV2710_EXPO_REG_MID, (ShutterCtrl & 0x0000ff00) >> 8);
    OV2710_RegWrite(OV2710_EXPO_REG_LOW, (ShutterCtrl & 0x000000ff) );

    //AmbaPrint("ShutterCtrl: %d",ShutterCtrl);

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: OV2710_SetSlowShutterCtrl
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
static int OV2710_SetSlowShutterCtrl(AMBA_DSP_CHANNEL_ID_u Chan, UINT32 SlowShutterCtrl)
{
    OV2710Ctrl.Status.ModeInfo.FrameTime.FrameRate.NumUnitsInTick =
        OV2710ModeInfoList[OV2710Ctrl.Status.ModeInfo.Mode.Bits.Mode].FrameTiming.FrameRate.NumUnitsInTick * SlowShutterCtrl;
    return OK;
}

#ifdef AMBA_SBRIDGE_DEV_DRV_H
/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: TI913914_Init
 *
 *  @Description:: Init TI 913/914
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static int TI913914_Init(void)
{
	int rval = 0;

#ifdef I2C_PASSBY_SERDES
    UINT16 devide_slave_id = OV2710_SLAVE_ADDR_WR; //device_slave_id(sensor_slave_id)

    if (AmbaSbrg_Config(SBRG_CHAN_ID_DS90UB91X, DS90UB91X_INIT, &devide_slave_id) == 0)
        rval = OK;
    else
        rval = NG;
#else
	if (AmbaSbrg_Config(SBRG_CHAN_ID_DS90UB91X, DS90UB91X_INIT, NULL) != 0)
		rval = NG;
#endif

	return rval;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: TI913914_ConfirmLock
 *
 *  @Description:: Confirm TI 913/914 lock status
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static int TI913914_ConfirmLock(void)
{
	UINT8 reg;
	UINT16 count = 0xffff;
	int rval = 0;

	AmbaSbrg_Config(SBRG_CHAN_ID_DS90UB91X, DS90UB91X_SELECT_914Q, NULL);

	while (count != 0) {
		AmbaSbrg_ReadReg(SBRG_CHAN_ID_DS90UB91X, 0x1c, &reg, 1);

		if (reg & 0x1) {
			AmbaPrint("=> TI914Q locked");
			break;
		} else {
			AmbaPrint("=> Waiting for TI914Q to lock!: 0x%x", reg);
			count--;
		}

		if (count == 0) {
			rval = NG;
		}
	}

	return rval;
}
#endif

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: OV2710_Config
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
static int OV2710_Config(AMBA_DSP_CHANNEL_ID_u Chan, AMBA_SENSOR_MODE_ID_u Mode, AMBA_SENSOR_ESHUTTER_TYPE_e ElecShutterMode)
{
    AMBA_SENSOR_MODE_INFO_s *pModeInfo = NULL;
    OV2710_FRAME_TIMING_s *pFrameTime = NULL;
    UINT16 SensorMode = Mode.Bits.Mode;
    static UINT8 FirstInit = 1;
#ifdef AMBA_SBRIDGE_DEV_DRV_H
    static UINT8 TI913914Inited = 0;
    UINT16 FrmaeTime = 0;
#endif

    if (SensorMode >= AMBA_SENSOR_OV2710_NUM_MODE)
        return NG;

    AmbaPrint("============  [ OV2710 ]  ============");
    AmbaPrint("============ init mode:%d ============", SensorMode);

    OV2710Ctrl.Status.ElecShutterMode = ElecShutterMode;

    pModeInfo  = &OV2710Ctrl.Status.ModeInfo;
    pFrameTime = &OV2710Ctrl.FrameTime;
    OV2710_PrepareModeInfo(SensorMode, pModeInfo, pFrameTime);

    /* Make VIN not to capture bad frames during readout mode transition */
    AmbaVIN_Reset(AMBA_VIN_CHANNEL0, AMBA_VIN_DVP, NULL);

    AmbaKAL_TaskSleep(3);

    AmbaPLL_SetSensorClk(pModeInfo->FrameTime.InputClk);

    AmbaKAL_TaskSleep(3);

    if(FirstInit) {
        AmbaSbrg_DriverInit();
        FirstInit = 0;
    }

#ifdef AMBA_SBRIDGE_DEV_DRV_H
    if (TI913914_Init()== OK) {
		TI913914Inited = 1;
	} else {
		TI913914Inited = 0;
		AmbaPrint("TI91X might not be applicable!");
	}
#endif

    OV2710_InitSeq(SensorMode);
    OV2710_ChangeReadoutMode(SensorMode);

    OV2710_ConfigVin(pModeInfo, pFrameTime);

#ifdef AMBA_SBRIDGE_DEV_DRV_H
    /* Skip first frame */
	FrmaeTime = (UINT32)((pFrameTime->FrameRate.NumUnitsInTick * 1000) / (pFrameTime->FrameRate.TimeScale) + 0.5);
	AmbaKAL_TaskSleep(FrmaeTime);

    if (TI913914Inited) {
		if (TI913914_ConfirmLock() == NG)
			AmbaPrint("TI914Q can not lock!");
	}
#endif

    OV2710Ctrl.Status.DevStat.Bits.Sensor0Standby = 0;

    return OK;
}

/*-----------------------------------------------------------------------------------------------*/
AMBA_SENSOR_OBJ_s AmbaSensor_OV2710Obj = {
    .Init                   = OV2710_Init,
    .Enable                 = OV2710_Enable,
    .Disable                = OV2710_Disable,
    .Config                 = OV2710_Config,
    .ChangeFrameRate        = OV2710_ChangeFrameRate,
    .GetStatus              = OV2710_GetStatus,
    .GetModeInfo            = OV2710_GetModeInfo,
    .GetDeviceInfo          = OV2710_GetDeviceInfo,
    .GetCurrentGainFactor   = NULL,
    .GetCurrentShutterSpeed = NULL,

    .ConvertGainFactor      = OV2710_ConvertGainFactor,
    .ConvertShutterSpeed    = OV2710_ConvertShutterSpeed,

    .SetAnalogGainCtrl      = OV2710_SetAnalogGainCtrl,
    .SetDigitalGainCtrl     = OV2710_SetDigitalGainCtrl,
    .SetShutterCtrl         = OV2710_SetShutterCtrl,
    .SetSlowShutterCtrl     = OV2710_SetSlowShutterCtrl,

    .SetGainFactor          = OV2710_SetGainFactor,
    .SetEshutterSpeed       = OV2710_SetEshutterSpeed,

    .RegisterRead           = OV2710_RegisterRead,
    .RegisterWrite          = OV2710_RegisterWrite,
};

