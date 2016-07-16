/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaSensor_OV9750.c
 *
 *  @Copyright      :: Copyright (C) 2013 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Control APIs of OmniVision OV9750 CMOS sensor with MIPI interface
\*-------------------------------------------------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "AmbaDataType.h"
#include "AmbaKAL.h"
#include "AmbaPrintk.h"

#include "AmbaDSP_VIN.h"
#include "AmbaVIN.h"
#include "AmbaI2C.h"
#include "AmbaGPIO.h"
#include "AmbaPLL.h"
#include "AmbaSensor.h"
#include "AmbaSensor_OV9750.h"

/*-----------------------------------------------------------------------------------------------*\
 * OV9750 sensor runtime info
\*-----------------------------------------------------------------------------------------------*/
OV9750_CTRL_s OV9750Ctrl = {0};

/*-----------------------------------------------------------------------------------------------*\
 * Configurations for Amba. VIN side
\*-----------------------------------------------------------------------------------------------*/
static AMBA_DSP_VIN_MIPI_CONFIG_s OV9750VinConfig = {
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
        .BayerPattern       = AMBA_DSP_BAYER_RG,
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
    /* Default setting */
    .MipiCtrl               = {
    .HsSettleTime   = 45,
    .HsTermTime     = 14,
    .ClkSettleTime  = 63,
    .ClkTermTime    = 8,
    .ClkMissTime    = 12,
    .RxInitTime     = 64,
    }
};

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: OV9750_PrepareModeInfo
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
static void OV9750_PrepareModeInfo(UINT16 Mode, AMBA_SENSOR_MODE_INFO_s *pModeInfo, OV9750_FRAME_TIMING_s *pSensorPrivate)
{
    const AMBA_SENSOR_INPUT_INFO_s  *pInputInfo= &OV9750InputInfo[OV9750ModeInfoList[Mode].InputMode];
    const AMBA_SENSOR_OUTPUT_INFO_s *pOutputInfo= &OV9750OutputInfo[OV9750ModeInfoList[Mode].OutputMode];

    if (pSensorPrivate == NULL)
        pSensorPrivate = (OV9750_FRAME_TIMING_s *) &OV9750ModeInfoList[Mode].FrameTiming;
    else
        memcpy(pSensorPrivate, &OV9750ModeInfoList[Mode].FrameTiming, sizeof(OV9750_FRAME_TIMING_s));

    pModeInfo->Mode.Data = Mode;
    pModeInfo->LineLengthPck = OV9750ModeInfoList[Mode].FrameTiming.Linelengthpck;
    pModeInfo->FrameLengthLines = OV9750ModeInfoList[Mode].FrameTiming.FrameLengthLines;

    pModeInfo->NumExposureStepPerFrame = OV9750ModeInfoList[Mode].FrameTiming.FrameLengthLines;
    pModeInfo->PixelRate = (float) pOutputInfo->DataRate * pOutputInfo->NumDataLanes / pOutputInfo->NumDataBits;
    pModeInfo->RowTime = (float) pModeInfo->LineLengthPck / pModeInfo->PixelRate;
    pModeInfo->InternalExposureOffset = 0;
    pModeInfo->FrameTime.InputClk = OV9750ModeInfoList[Mode].FrameTiming.InputClk;
    memcpy(&pModeInfo->FrameTime.FrameRate, &(OV9750ModeInfoList[Mode].FrameTiming.FrameRate), sizeof(AMBA_DSP_FRAME_RATE_s));
    memcpy(&pModeInfo->InputInfo, pInputInfo, sizeof(AMBA_SENSOR_INPUT_INFO_s));
    memcpy(&pModeInfo->OutputInfo, pOutputInfo, sizeof(AMBA_SENSOR_OUTPUT_INFO_s));
    memcpy(&pModeInfo->MinFrameRate, &(OV9750ModeInfoList[Mode].FrameTiming.FrameRate), sizeof(AMBA_DSP_FRAME_RATE_s));
    //AmbaPrint("PixelRate:%f, RowTime:%f",pModeInfo->PixelRate,pModeInfo->RowTime);

    /* Only support 1 stage slow shutter */
    pModeInfo->MinFrameRate.TimeScale /= 2;

    /* HDR information */
    memset(&pModeInfo->HdrInfo, 0, sizeof(AMBA_SENSOR_HDR_INFO_s)); // Not support HDR
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: OV9750_ConfigVin
 *
 *  @Description:: Configure VIN to receieve output frames of the new readout mode
 *
 *  @Input      ::
 *      Chan:       Vin ID and sensor ID
 *      pModeInfo:  Details of the specified readout mode
 *      pFrameTime: Sensor frame time configuration
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static void OV9750_ConfigVin(AMBA_DSP_CHANNEL_ID_u Chan, AMBA_SENSOR_MODE_INFO_s *pModeInfo, OV9750_FRAME_TIMING_s *pFrameTime)
{
    AMBA_SENSOR_OUTPUT_INFO_s  *pImgInfo = &pModeInfo->OutputInfo;
    AMBA_DSP_VIN_MIPI_CONFIG_s *pVinCfg = &OV9750VinConfig;

    pVinCfg->NumActiveLanes = pImgInfo->NumDataLanes;
    pVinCfg->Info.NumDataBits = pImgInfo->NumDataBits;
    
    pVinCfg->RxHvSyncCtrl.NumActivePixels  = pImgInfo->OutputWidth;
    pVinCfg->RxHvSyncCtrl.NumActiveLines   = pImgInfo->OutputHeight;
    pVinCfg->RxHvSyncCtrl.NumTotalPixels   = pModeInfo->LineLengthPck - 1;
    pVinCfg->RxHvSyncCtrl.NumTotalLines    = pModeInfo->FrameLengthLines;

    pVinCfg->Info.BayerPattern = (AMBA_DSP_SENSOR_PATTERN_e) pImgInfo->CfaPattern;

    memcpy(&pVinCfg->Info.FrameRate, &pFrameTime->FrameRate, sizeof(AMBA_DSP_FRAME_RATE_s));
    memcpy(&pVinCfg->Info.DspPhaseShift, &pImgInfo->DspPhaseShift, sizeof(AMBA_DSP_PHASE_SHIFT_CTRL_s));

    AmbaDSP_VinConfigMIPI(Chan.Bits.VinID, pVinCfg);
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: OV9750_HardwareReset
 *
 *  @Description:: Reset OV9750 Image Sensor Device
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static inline void OV9750_HardwareReset(void)
{
    AmbaUserGPIO_SensorResetCtrl(0);
    AmbaKAL_TaskSleep(3);
    AmbaUserGPIO_SensorResetCtrl(1);
    AmbaKAL_TaskSleep(3);
}

static int OV9750_RegWrite(UINT16 Addr, UINT8 Data)
{
    UINT8 TxDataBuf[3];

    TxDataBuf[0] = (UINT8) ((Addr & 0xff00) >> 8);
    TxDataBuf[1] = (UINT8) (Addr & 0x00ff);
    TxDataBuf[2] = Data;

    if (AmbaI2C_Write(AMBA_I2C_CHANNEL0, AMBA_I2C_SPEED_FAST, 0x6c, 3, TxDataBuf, AMBA_KAL_WAIT_FOREVER) == NG)
        AmbaPrint("I2C does not work!!!!!");

    //AmbaPrint("Addr = 0x%04x, Data = 0x%02x", Addr, Data);

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: OV9750_RegisterWrite
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
static int OV9750_RegisterWrite(AMBA_DSP_CHANNEL_ID_u Chan, UINT16 Addr, UINT16 Data)
{
    UINT8 WriteData = (UINT8)Data;

    OV9750_RegWrite(Addr, WriteData);

    return OK;
}

static int OV9750_RegRead(UINT16 Addr, UINT8 *pRxData)
{
    UINT16 TxData[4];

    TxData[0] = AMBA_I2C_RESTART_FLAG | (0x6c);
    TxData[1] = (Addr >> 8);    /* Register Address [15:8] */
    TxData[2] = (Addr & 0xff);  /* Register Address [7:0]  */
    TxData[3] = AMBA_I2C_RESTART_FLAG | (0x6d);

    return AmbaI2C_ReadAfterWrite(AMBA_I2C_CHANNEL0, AMBA_I2C_SPEED_FAST, 4, (UINT16 *) TxData, 1, pRxData, AMBA_KAL_WAIT_FOREVER);
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: OV9750_RegisterRead
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
static int OV9750_RegisterRead(AMBA_DSP_CHANNEL_ID_u Chan, UINT16 Addr, UINT16 *pData)
{
    UINT8 ReadData;

    OV9750_RegRead(Addr, &ReadData);
    *pData = ReadData;

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: OV9750_SetStandbyOn
 *
 *  @Description:: Configure sensor into standby mode
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static void OV9750_SetStandbyOn(void)
{
    UINT32 Delay;

    Delay = OV9750Ctrl.Status.ModeInfo.FrameTime.FrameRate.TimeScale /
            OV9750Ctrl.Status.ModeInfo.FrameTime.FrameRate.NumUnitsInTick;

    OV9750_RegWrite(0x0100, 0x0);

    AmbaKAL_TaskSleep(Delay);
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: OV9750_SetStandbyOff
 *
 *  @Description:: Configure sensor to leave standby mode.
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static void OV9750_SetStandbyOff(void)
{
    OV9750_RegWrite(0x0100, 0x1);
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: OV9750_ChangeReadoutMode
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
static int OV9750_ChangeReadoutMode(UINT16 Mode)
{
    int i = 0;

    OV9750_RegWrite(0x0103, 0x01); // software reset

    AmbaKAL_TaskSleep(10); // delay 10ms after software reset

    for (i = 0; i < OV9750_NUM_READOUT_MODE_REG; i ++) {
        OV9750_RegWrite(OV9750RegTable[i].Addr, OV9750RegTable[i].Data[OV9750ModeInfoList[Mode].ReadoutMode]);
    }

    /* Reset current AE information */
    memset(&OV9750Ctrl.CurrentAEInfo, 0, sizeof(OV9750_CURRENT_AE_INFO_s));

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: OV9750_Init
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
static int OV9750_Init(AMBA_DSP_CHANNEL_ID_u Chan)
{
    const OV9750_FRAME_TIMING_s *pFrameTime = &OV9750ModeInfoList[0].FrameTiming;

    /* For Second VIN */
    if (Chan.Bits.VinID == 1) {
        AmbaPLL_SetEnetClkConfig(1);
        AmbaPLL_SetEnetClk(pFrameTime->InputClk);

    /* For Main VIN */
    } else {
        AmbaPLL_SetSensorClk(pFrameTime->InputClk);
        AmbaPLL_SetSensorClkDir(AMBA_PLL_SENSOR_CLK_OUTPUT);
    }

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: OV9750_Enable
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
static int OV9750_Enable(AMBA_DSP_CHANNEL_ID_u Chan)
{
    if (OV9750Ctrl.Status.DevStat.Bits.Sensor0Power == 1)
        return OK;

    /* Config mipi phy*/
    AmbaDSP_VinPhySetMIPI(Chan.Bits.VinID);
    
    OV9750Ctrl.Status.DevStat.Bits.Sensor0Power   = 1;
    OV9750Ctrl.Status.DevStat.Bits.Sensor0Standby = 1;

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: OV9750_Disable
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
static int OV9750_Disable(AMBA_DSP_CHANNEL_ID_u Chan)
{
    if (OV9750Ctrl.Status.DevStat.Bits.Sensor0Power == 0)
        return OK;

    OV9750_SetStandbyOn();

    OV9750Ctrl.Status.DevStat.Bits.Sensor0Power = 0;

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: OV9750_GetStatus
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
static int OV9750_GetStatus(AMBA_DSP_CHANNEL_ID_u Chan, AMBA_SENSOR_STATUS_INFO_s *pStatus)
{
    if (pStatus == NULL || OV9750Ctrl.Status.DevStat.Bits.Sensor0Power == 0)
        return NG;

    OV9750Ctrl.Status.TimeToAllPelExposed = 0;  /* TODO: for flash light or mechanical shutter control */

    memcpy(pStatus, &OV9750Ctrl.Status, sizeof(AMBA_SENSOR_STATUS_INFO_s));

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: OV9750_GetModeInfo
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
static int OV9750_GetModeInfo(AMBA_DSP_CHANNEL_ID_u Chan, AMBA_SENSOR_MODE_ID_u Mode, AMBA_SENSOR_MODE_INFO_s *pModeInfo)
{
    UINT16 SensorMode = Mode.Bits.Mode; // TODO

    if (SensorMode == AMBA_SENSOR_CURRENT_MODE)
        SensorMode = OV9750Ctrl.Status.ModeInfo.Mode.Bits.Mode;

    if (SensorMode >= AMBA_SENSOR_OV9750_NUM_MODE || pModeInfo == NULL)
        return NG;

    OV9750_PrepareModeInfo(SensorMode, pModeInfo, NULL);

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: OV9750_GetDeviceInfo
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
static int OV9750_GetDeviceInfo(AMBA_DSP_CHANNEL_ID_u Chan, AMBA_SENSOR_DEVICE_INFO_s *pDeviceInfo)
{
    if (pDeviceInfo == NULL)
        return NG;

    memcpy(pDeviceInfo, &OV9750DeviceInfo, sizeof(AMBA_SENSOR_DEVICE_INFO_s));

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: OV9750_GetCurrentGainFactor
 *
 *  @Description:: Get Sensor Current Gain Factor
 *
 *  @Input      ::
 *      Chan:   Vin ID and sensor ID
 *
 *  @Output     ::
 *      pGainFactor:    pointer to current gain factor
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static int OV9750_GetCurrentGainFactor(AMBA_DSP_CHANNEL_ID_u Chan, float *pGainFactor)
{
    UINT16 RealAgcGain = (OV9750Ctrl.CurrentAEInfo.CurrentAgcCtrl & 0xffff0000) >> 16;
    UINT16 RealWBGain = OV9750Ctrl.CurrentAEInfo.CurrentAgcCtrl & 0x0000ffff;

    *pGainFactor = ((float)RealAgcGain / 128) * ((float)RealWBGain / 1024);

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: OV9750_GetCurrentShutterSpeed
 *
 *  @Description:: Get Sensor Current Exposure Time
 *
 *  @Input      ::
 *      Chan:   Vin ID and sensor ID
 *
 *  @Output     ::
 *      pExposureTime:    pointer to current exposure time
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static int OV9750_GetCurrentShutterSpeed(AMBA_DSP_CHANNEL_ID_u Chan, float *pExposureTime)
{
    *pExposureTime = OV9750Ctrl.Status.ModeInfo.RowTime * OV9750Ctrl.CurrentAEInfo.CurrentShutterCtrl;

    //AmbaPrint("ExposureTime = %f", *pExposureTime);

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: OV9750_ConvertGainFactor
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
static int OV9750_ConvertGainFactor(AMBA_DSP_CHANNEL_ID_u Chan, float DesiredFactor, UINT32 *pActualFactor, UINT32 *pAnalogGainCtrl, UINT32 *pDigitalGainCtrl)
{
    float AgcFactor, WbFactor;
    UINT16 RealAgcGain = 1;
    UINT16 RealWBGain = 1;
    UINT8 AgcR3508 = 0;
    UINT8 AgcR3509 = 0;
    UINT8 WbMsb = 0;
    UINT8 WbLsb = 0;

    /* Maximum AGC gain is 15.5x */
    if (DesiredFactor > 15.5) {
        AgcFactor = 15.5;
        WbFactor = DesiredFactor / 15.5;
    } else {
        AgcFactor = DesiredFactor;
        WbFactor = 1.0;
    }

    /* Maximum WB gain is 4x */
    if (WbFactor > 4.0)
        WbFactor = 4.0;

    /* Convert AGC gain */
    RealAgcGain = AgcFactor * 128;
    AgcR3508 = (RealAgcGain & 0x1f00) >> 8;
    AgcR3509 = RealAgcGain & 0x00ff;

    /* Convert WB gain */
    RealWBGain = WbFactor * 1024;
    RealWBGain = (RealWBGain > 0x0fff) ? 0x0fff : RealWBGain;  // Max Wb gain
    WbMsb = (RealWBGain & 0xff00) >> 8;
    WbLsb = RealWBGain & 0x00ff;

    *pAnalogGainCtrl  = (AgcR3508 << 24) + (AgcR3509 << 16) + (WbMsb << 8) + WbLsb;
    *pDigitalGainCtrl = 0;
    *pActualFactor = 1048576 * ((float)RealAgcGain / 128) * ((float)RealWBGain / 1024);

    //AmbaPrint("ActualFactor << 20 = 0x%08x", *pActualFactor);
    //AmbaPrint("AgcR3508 = 0x%02x, AgcR3509 = 0x%02x", AgcR3508, AgcR3509);
    //AmbaPrint("WbMsb = 0x%02x, WbLsb = 0x%02x", WbMsb, WbLsb);

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: OV9750_ConvertShutterSpeed
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
static int OV9750_ConvertShutterSpeed(AMBA_DSP_CHANNEL_ID_u Chan, float ExposureTime, UINT32 *pShutterCtrl)
{
    AMBA_SENSOR_MODE_INFO_s *pModeInfo = &OV9750Ctrl.Status.ModeInfo;

    if (pShutterCtrl == NULL)
        return NG;

    *pShutterCtrl = (UINT32)(ExposureTime / pModeInfo->RowTime);

    if (*pShutterCtrl <= 1)
        *pShutterCtrl = 1;

    //AmbaPrint("ExposureTime:%f, ShutterCtrl:%d", ExposureTime, *pShutterCtrl);

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: OV9750_SetAnalogGainCtrl
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
static int OV9750_SetAnalogGainCtrl(AMBA_DSP_CHANNEL_ID_u Chan, UINT32 AnalogGainCtrl)
{
    UINT8 AgcR3508 = (AnalogGainCtrl & 0xff000000) >> 24;
    UINT8 AgcR3509 = (AnalogGainCtrl & 0x00ff0000) >> 16;
    UINT8 WbMsb = (AnalogGainCtrl & 0x0000ff00) >> 8;
    UINT8 WbLsb = (AnalogGainCtrl & 0x000000ff);
    UINT8 R366A = 0;

    if (AgcR3508 >= 0x4)
        R366A = 0x7;
    else if (AgcR3508 >= 0x2)
        R366A = 0x3;
    else if (AgcR3508 >= 0x1)
        R366A = 0x1;
    else
        R366A = 0x0;

    OV9750_RegWrite(0x3208, 0x00);  // group start

    OV9750_RegWrite(0x3508, AgcR3508);
    OV9750_RegWrite(0x3509, AgcR3509);

    /* R-channel */
    OV9750_RegWrite(0x5032, WbMsb);
    OV9750_RegWrite(0x5033, WbLsb);
    /* G-channel */
    OV9750_RegWrite(0x5034, WbMsb);
    OV9750_RegWrite(0x5035, WbLsb);
    /* B-channel */
    OV9750_RegWrite(0x5036, WbMsb);
    OV9750_RegWrite(0x5037, WbLsb);

    OV9750_RegWrite(0x366A, R366A);

    OV9750_RegWrite(0x3208, 0x10);  // group end
    OV9750_RegWrite(0x3208, 0xA0);  // group delay latch

    /* Update current AGC control */
    OV9750Ctrl.CurrentAEInfo.CurrentAgcCtrl = AnalogGainCtrl;

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: OV9750_SetShutterCtrl
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
static int OV9750_SetShutterCtrl(AMBA_DSP_CHANNEL_ID_u Chan, UINT32 ShutterCtrl)
{
    AMBA_SENSOR_MODE_INFO_s *pModeInfo = &OV9750Ctrl.Status.ModeInfo;

    /* Exposure line needs be smaller than VTS - 8 */
    if (ShutterCtrl >= pModeInfo->NumExposureStepPerFrame - 8)
        ShutterCtrl = pModeInfo->NumExposureStepPerFrame - 8;

    ShutterCtrl = ShutterCtrl << 4;
    OV9750_RegWrite(0x3500, (ShutterCtrl & 0x000f0000) >> 16);
    OV9750_RegWrite(0x3501, (ShutterCtrl & 0x0000ff00) >> 8);
    OV9750_RegWrite(0x3502, (ShutterCtrl & 0x000000ff) >> 0);

    /* Update current shutter control */
    OV9750Ctrl.CurrentAEInfo.CurrentShutterCtrl = ShutterCtrl;

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: OV9750_SetSlowShutterCtrl
 *
 *  @Description:: Set slow shutter control
 *
 *  @Input      ::
 *      Chan:               Vin ID and sensor ID
 *      pSlowShutterCtrl:   Electronic slow shutter control
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static int OV9750_SetSlowShutterCtrl(AMBA_DSP_CHANNEL_ID_u Chan, UINT32 SlowShutterCtrl)
{
    UINT32 TargetFrameLengthLines;

    if (SlowShutterCtrl < 1)
        SlowShutterCtrl = 1;

    TargetFrameLengthLines = OV9750Ctrl.FrameTime.FrameLengthLines * SlowShutterCtrl;

    /* Update frame rate information */
    OV9750Ctrl.Status.ModeInfo.FrameLengthLines = TargetFrameLengthLines;
    OV9750Ctrl.Status.ModeInfo.NumExposureStepPerFrame = TargetFrameLengthLines;
    OV9750Ctrl.Status.ModeInfo.FrameTime.FrameRate.TimeScale = OV9750Ctrl.FrameTime.FrameRate.TimeScale / SlowShutterCtrl;

    OV9750_RegWrite(0x380e, (TargetFrameLengthLines & 0x0000ff00) >> 8);
    OV9750_RegWrite(0x380f, (TargetFrameLengthLines & 0x000000ff));

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: OV9750_Config
 *
 *  @Description:: Set sensor to indicated mode
 *
 *  @Input      ::
 *      Chan:               Vin ID and sensor ID
 *      Mode:               Sensor Readout Mode Number
 *      ElecShutterMode:    Operating Mode of the Electronic Shutter
 *
 *  @Output     ::
 *      pModeInfo: pointer to mode info of target mode
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static int OV9750_Config(AMBA_DSP_CHANNEL_ID_u Chan, AMBA_SENSOR_MODE_ID_u Mode, AMBA_SENSOR_ESHUTTER_TYPE_e ElecShutterMode)
{
    AMBA_SENSOR_MODE_INFO_s *pModeInfo = NULL;
    OV9750_FRAME_TIMING_s *pFrameTime = NULL;
    UINT16 SensorMode = Mode.Bits.Mode;

    if (SensorMode >= AMBA_SENSOR_OV9750_NUM_MODE)
        return NG;

    AmbaPrint("============  [ OV9750 ]  ============");
    AmbaPrint("============ init mode:%d ============", SensorMode);

    OV9750Ctrl.Status.ElecShutterMode = ElecShutterMode;

    pModeInfo  = &OV9750Ctrl.Status.ModeInfo;
    pFrameTime = &OV9750Ctrl.FrameTime;
    OV9750_PrepareModeInfo(SensorMode, pModeInfo, pFrameTime);

    /* Adjust mipi-phy parameters */
    AmbaVIN_CalculateMphyConfig(pModeInfo->OutputInfo.DataRate, &OV9750VinConfig.MipiCtrl);
 
    //AmbaPrint("HsSettleTime = %d, HsTermTime = %d, ClkSettleTime = %d, ClkTermTime = %d, ClkMissTime = %d, RxInitTime = %d",
              //OV9750VinConfig.MipiCtrl.HsSettleTime, OV9750VinConfig.MipiCtrl.HsTermTime,
              //OV9750VinConfig.MipiCtrl.ClkSettleTime, OV9750VinConfig.MipiCtrl.ClkTermTime,
              //OV9750VinConfig.MipiCtrl.ClkMissTime, OV9750VinConfig.MipiCtrl.RxInitTime);

    /* Fine tune mipi-phy parameters */ 
    OV9750VinConfig.MipiCtrl.HsSettleTime = 28;

    /* Make VIN not to capture bad frames during readout mode transition */
    AmbaVIN_Reset(Chan.Bits.VinID, AMBA_VIN_MIPI, &OV9750VinConfig.MipiCtrl);
    AmbaKAL_TaskSleep(3);

    AmbaPLL_SetSensorClk(pModeInfo->FrameTime.InputClk);
    AmbaKAL_TaskSleep(3);

    OV9750_HardwareReset();

    OV9750_ChangeReadoutMode(SensorMode);
    OV9750_SetStandbyOff();

    OV9750_ConfigVin(Chan, pModeInfo, pFrameTime);

    OV9750Ctrl.Status.DevStat.Bits.Sensor0Standby = 0;

    return OK;
}

/*-----------------------------------------------------------------------------------------------*/
AMBA_SENSOR_OBJ_s AmbaSensor_OV9750Obj = {
    .Init                   = OV9750_Init,
    .Enable                 = OV9750_Enable,
    .Disable                = OV9750_Disable,
    .Config                 = OV9750_Config,
    .GetStatus              = OV9750_GetStatus,
    .GetModeInfo            = OV9750_GetModeInfo,
    .GetDeviceInfo          = OV9750_GetDeviceInfo,
    .GetCurrentGainFactor   = OV9750_GetCurrentGainFactor,
    .GetCurrentShutterSpeed = OV9750_GetCurrentShutterSpeed,
    .ConvertGainFactor      = OV9750_ConvertGainFactor,
    .ConvertShutterSpeed    = OV9750_ConvertShutterSpeed,
    .SetAnalogGainCtrl      = OV9750_SetAnalogGainCtrl,
    .SetDigitalGainCtrl     = NULL,
    .SetShutterCtrl         = OV9750_SetShutterCtrl,
    .SetSlowShutterCtrl     = OV9750_SetSlowShutterCtrl,

    .RegisterRead           = OV9750_RegisterRead,
    .RegisterWrite          = OV9750_RegisterWrite,
};
