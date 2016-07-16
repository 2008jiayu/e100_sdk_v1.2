/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaSensor_OV4689.c
 *
 *  @Copyright      :: Copyright (C) 2013 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Control APIs of OmniVision OV4689 CMOS sensor with MIPI interface
\*-------------------------------------------------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <math.h>

#include "AmbaDataType.h"
#include "AmbaKAL.h"
#include "AmbaPrintk.h"

#include "AmbaDSP_VIN.h"
#include "AmbaVIN.h"
#include "AmbaI2C.h"
#include "AmbaGPIO.h"
#include "AmbaPLL.h"
#include "AmbaSensor.h"
#include "AmbaSensor_OV4689.h"

static UINT8 AEModeSwitchFlag = 0; // Temporary solution 20150911

/*-----------------------------------------------------------------------------------------------*\
 * OV4689 sensor runtime info
\*-----------------------------------------------------------------------------------------------*/
OV4689_CTRL_s OV4689Ctrl = {0};

/*-----------------------------------------------------------------------------------------------*\
 * Configurations for Amba. VIN side
\*-----------------------------------------------------------------------------------------------*/
static AMBA_DSP_VIN_MIPI_CONFIG_s OV4689VinConfig = {
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
    .MipiCtrl               = {
    .HsSettleTime   = 32,
    .HsTermTime     = 16,
    .ClkSettleTime  = 32,
    .ClkTermTime    = 8,
    .ClkMissTime    = 8,
    .RxInitTime     = 64,
    }
};

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: OV4689_PrepareModeInfo
 *
 *  @Description:: Get sensor mode information
 *
 *  @Input      ::
 *      Mode:   Sensor readout mode number
 *
 *  @Output     ::
 *      pModeInfo:      Details of the specified readout mode
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static void OV4689_PrepareModeInfo(UINT16 Mode, AMBA_SENSOR_MODE_INFO_s *pModeInfo)
{
    const AMBA_SENSOR_INPUT_INFO_s  *pInputInfo = &OV4689InputInfo[OV4689ModeInfoList[Mode].ReadoutMode];
    const AMBA_SENSOR_OUTPUT_INFO_s *pOutputInfo= &OV4689OutputInfo[OV4689ModeInfoList[Mode].ReadoutMode];

    pModeInfo->Mode.Data = Mode;
    pModeInfo->LineLengthPck = (UINT32)((UINT64)pOutputInfo->DataRate * pOutputInfo->NumDataLanes / pOutputInfo->NumDataBits *
                                         OV4689ModeInfoList[Mode].FrameTiming.FrameRate.NumUnitsInTick /
                                         OV4689ModeInfoList[Mode].FrameTiming.FrameRate.TimeScale /
                                         OV4689ModeInfoList[Mode].FrameTiming.FrameLengthLines);
    pModeInfo->FrameLengthLines = OV4689ModeInfoList[Mode].FrameTiming.FrameLengthLines;
    pModeInfo->NumExposureStepPerFrame = OV4689ModeInfoList[Mode].FrameTiming.FrameLengthLines;
    pModeInfo->PixelRate = (float) pOutputInfo->DataRate * pOutputInfo->NumDataLanes / pOutputInfo->NumDataBits;
    pModeInfo->RowTime = (float) pModeInfo->LineLengthPck / pModeInfo->PixelRate;
    pModeInfo->InternalExposureOffset = 0;
    pModeInfo->FrameTime.InputClk = OV4689ModeInfoList[Mode].FrameTiming.InputClk;
    memcpy(&pModeInfo->FrameTime.FrameRate, &(OV4689ModeInfoList[Mode].FrameTiming.FrameRate), sizeof(AMBA_DSP_FRAME_RATE_s));
    memcpy(&pModeInfo->InputInfo, pInputInfo, sizeof(AMBA_SENSOR_INPUT_INFO_s));
    memcpy(&pModeInfo->OutputInfo, pOutputInfo, sizeof(AMBA_SENSOR_OUTPUT_INFO_s));
    memcpy(&pModeInfo->MinFrameRate, &(OV4689ModeInfoList[Mode].FrameTiming.FrameRate), sizeof(AMBA_DSP_FRAME_RATE_s));
    memcpy(&pModeInfo->HdrInfo, &OV4689HdrInfo[Mode], sizeof(AMBA_SENSOR_HDR_INFO_s)); // HDR information

    /* Only support 1 stage slow shutter with none HDR modes */
    if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_NONE)
        pModeInfo->MinFrameRate.TimeScale /= 2;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: OV4689_ConfigVin
 *
 *  @Description:: Configure VIN to receive output frames of the new readout mode
 *
 *  @Input      ::
  *     Chan:       Vin ID and sensor ID
 *      pModeInfo:  Details of the specified readout mode
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static void OV4689_ConfigVin(AMBA_DSP_CHANNEL_ID_u Chan, AMBA_SENSOR_MODE_INFO_s *pModeInfo)
{
    AMBA_SENSOR_OUTPUT_INFO_s  *pImgInfo = &pModeInfo->OutputInfo;
    AMBA_DSP_VIN_MIPI_CONFIG_s *pVinCfg = &OV4689VinConfig;

    pVinCfg->NumActiveLanes = pImgInfo->NumDataLanes;
    pVinCfg->Info.NumDataBits = pImgInfo->NumDataBits;
    pVinCfg->Info.BayerPattern = (AMBA_DSP_SENSOR_PATTERN_e) pImgInfo->CfaPattern;

    pVinCfg->RxHvSyncCtrl.NumActivePixels  = pImgInfo->OutputWidth;
    pVinCfg->RxHvSyncCtrl.NumActiveLines   = pImgInfo->OutputHeight;
    pVinCfg->RxHvSyncCtrl.NumTotalPixels   = pModeInfo->LineLengthPck - 1;
    pVinCfg->RxHvSyncCtrl.NumTotalLines    = pModeInfo->FrameLengthLines;

    memcpy(&pVinCfg->Info.FrameRate, &pModeInfo->FrameTime.FrameRate, sizeof(AMBA_DSP_FRAME_RATE_s));
    memcpy(&pVinCfg->Info.DspPhaseShift, &pImgInfo->DspPhaseShift, sizeof(AMBA_DSP_PHASE_SHIFT_CTRL_s));

    AmbaDSP_VinConfigMIPI((AMBA_VIN_CHANNEL_e)Chan.Bits.VinID, pVinCfg);
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: OV4689_HardwareReset
 *
 *  @Description:: Reset OV4689 Image Sensor Device
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static inline void OV4689_HardwareReset(void)
{
    AmbaUserGPIO_SensorResetCtrl(0);
    AmbaKAL_TaskSleep(3);
    AmbaUserGPIO_SensorResetCtrl(1);
    AmbaKAL_TaskSleep(3);
}

static int OV4689_RegWrite(UINT16 Addr, UINT8 Data)
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
 *  @RoutineName:: OV4689_RegisterWrite
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
static int OV4689_RegisterWrite(AMBA_DSP_CHANNEL_ID_u Chan, UINT16 Addr, UINT16 Data)
{
    UINT8 WriteData = (UINT8)Data;

    OV4689_RegWrite(Addr, WriteData);

    return OK;
}

static int OV4689_RegRead(UINT16 Addr, UINT8 *pRxData)
{
    UINT16 pTxData[4];

    pTxData[0] = AMBA_I2C_RESTART_FLAG | (0x6c);
    pTxData[1] = (Addr >> 8);    /* Register Address [15:8] */
    pTxData[2] = (Addr & 0xff);  /* Register Address [7:0]  */
    pTxData[3] = AMBA_I2C_RESTART_FLAG | (0x6d);

    return AmbaI2C_ReadAfterWrite(AMBA_I2C_CHANNEL0, AMBA_I2C_SPEED_FAST,
                                  4, (UINT16 *) pTxData, 1, pRxData, AMBA_KAL_WAIT_FOREVER);
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: OV4689_RegisterRead
 *
 *  @Description:: Read sensor registers API
 *
 *  @Input      ::
 *      Chan:   Vin ID and sensor ID
 *      Addr:   Register address
 *      pData:  Pointer to register value
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *      int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static int OV4689_RegisterRead(AMBA_DSP_CHANNEL_ID_u Chan, UINT16 Addr, UINT16 *pData)
{
    static UINT8 ReadData;

    OV4689_RegRead(Addr, &ReadData);
    *pData = ReadData;

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: OV4689_SetAnalogGainReg
 *
 *  @Description:: Configure sensor analog gain setting
 *
 *  @Input      ::
 *    AnalogGainCtrl:    Analog gain control
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static void OV4689_SetAnalogGainReg(UINT32 AnalogGainCtrl)
{
    UINT8 AgcR3508 = (AnalogGainCtrl & 0x0000ff00) >> 8;
    UINT8 AgcR3509 = (AnalogGainCtrl & 0x000000ff);

    /* For long exposure frame */
    OV4689_RegWrite(0x3508, AgcR3508);
    OV4689_RegWrite(0x3509, AgcR3509);
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: OV4689_SetDigitalGainReg
 *
 *  @Description:: Configure sensor digital gain setting
 *
 *  @Input      ::
 *    DigitalGainCtrl:    Digital gain control
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static void OV4689_SetDigitalGainReg(UINT32 DigitalGainCtrl)
{
    UINT8 AgcR352A = (DigitalGainCtrl & 0x000000ff);
    UINT8 AgcR352B = (DigitalGainCtrl & 0x0000ff00) >> 8;

    /* For long exposure frame */
    OV4689_RegWrite(0x352a, AgcR352A);
    OV4689_RegWrite(0x352b, AgcR352B);
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: OV4689_SetShutterReg
 *
 *  @Description:: Apply electronic shutter setting
 *
 *  @Input      ::
 *      ShutterCtrl:    Electronic shutter control
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static void OV4689_SetShutterReg(UINT32 ShutterCtrl)
{
    ShutterCtrl = ShutterCtrl << 4;

    OV4689_RegWrite(0x3208, 0x01);  // group 1 start

    OV4689_RegWrite(0x3500, (ShutterCtrl & 0x000f0000) >> 16);
    OV4689_RegWrite(0x3501, (ShutterCtrl & 0x0000ff00) >> 8);
    OV4689_RegWrite(0x3502, (ShutterCtrl & 0x000000ff) >> 0);

    OV4689_RegWrite(0x3208, 0x11);  // group 1 end
    OV4689_RegWrite(0x3208, 0xE1);  // group 1 quick latch
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: OV4689_SetHdrAnalogGainReg
 *
 *  @Description:: Configure sensor analog gain setting
 *
 *  @Input      ::
 *      Chan:               Vin ID and sensor ID
 *      pAnalogGainCtrl:    Analog gain control for achievable gain factor
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static void OV4689_SetHdrAnalogGainReg(AMBA_DSP_CHANNEL_ID_u Chan, UINT32 *pAnalogGainCtrl)
{
    UINT8 AgcMsb;
    UINT8 AgcLsb;
    UINT8 DgcMsb;
    UINT8 DgcLsb;

    if (AEModeSwitchFlag == 1)
        OV4689_RegWrite(0x3208, 0x00);  // temporary group 0 start

    if (Chan.Bits.HdrID & 0x1) {
        /* For long exposure frame */
        AgcMsb = (*pAnalogGainCtrl & 0x0000ff00) >> 8;
        AgcLsb = (*pAnalogGainCtrl & 0x000000ff);

        OV4689_RegWrite(0x3508, AgcMsb);
        OV4689_RegWrite(0x3509, AgcLsb);

        /* For long exposure frame */
        DgcMsb = (*pAnalogGainCtrl & 0x00ff0000) >> 16;
        DgcLsb = (*pAnalogGainCtrl & 0xff000000) >> 24;

        OV4689_RegWrite(0x352a, DgcMsb);
        OV4689_RegWrite(0x352b, DgcLsb);
    }

    if (Chan.Bits.HdrID & 0x2){
        /* For short exposure frame */
        AgcMsb = (*(pAnalogGainCtrl + 1) & 0x0000ff00) >> 8;
        AgcLsb = (*(pAnalogGainCtrl + 1) & 0x000000ff);

        OV4689_RegWrite(0x350e, AgcMsb);
        OV4689_RegWrite(0x350f, AgcLsb);

        /* For short exposure frame */
        DgcMsb = (*(pAnalogGainCtrl + 1) & 0x00ff0000) >> 16;
        DgcLsb = (*(pAnalogGainCtrl + 1) & 0xff000000) >> 24;

        OV4689_RegWrite(0x3524, DgcMsb);
        OV4689_RegWrite(0x3525, DgcLsb);
    }

    if (AEModeSwitchFlag == 1) {
        OV4689_RegWrite(0x3208, 0x10);  // group 0 end
        OV4689_RegWrite(0x3208, 0xA0);  // group 0 delay latch
    }

    if (AEModeSwitchFlag == 0)
        AEModeSwitchFlag = 1;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: OV4689_SetHdrDigitalGainReg
 *
 *  @Description:: Configure sensor digital gain setting
 *
 *  @Input      ::
 *      Chan:               Vin ID and sensor ID
 *      pDigitalGainCtrl:   Digital gain control for achievable gain factor
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static void OV4689_SetHdrDigitalGainReg(AMBA_DSP_CHANNEL_ID_u Chan, UINT32 *pDigitalGainCtrl)
{
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: OV4689_SetHdrShutterReg
 *
 *  @Description:: Apply electronic shutter setting
 *
 *  @Input      ::
 *      Chan:           Vin ID and sensor ID
 *      pShutterCtrl:   Electronic shutter control
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static void OV4689_SetHdrShutterReg(AMBA_DSP_CHANNEL_ID_u Chan, UINT32 *pShutterCtrl)
{
    UINT32 ShutterCtrl;

    if (AEModeSwitchFlag == 1)
        OV4689_RegWrite(0x3208, 0x01);  // group 1 start

    if (Chan.Bits.HdrID & 0x1) {
        /* 1 exposure line = 2 row time in HDR mode */
        /* For long exposure frame */
        ShutterCtrl = (*pShutterCtrl / 2) << 4;
        OV4689_RegWrite(0x3500, (ShutterCtrl & 0x000f0000) >> 16);
        OV4689_RegWrite(0x3501, (ShutterCtrl & 0x0000ff00) >> 8);
        OV4689_RegWrite(0x3502, (ShutterCtrl & 0x000000ff) >> 0);
    }

    if (Chan.Bits.HdrID & 0x2) {
        /* 1 exposure line = 2 row time in HDR mode */
        /* For short exposure frame */
        ShutterCtrl = (*(pShutterCtrl + 1) / 2) << 4;
        OV4689_RegWrite(0x350a, (ShutterCtrl & 0x000f0000) >> 16);
        OV4689_RegWrite(0x350b, (ShutterCtrl & 0x0000ff00) >> 8);
        OV4689_RegWrite(0x350c, (ShutterCtrl & 0x000000ff) >> 0);
    }

    if (AEModeSwitchFlag == 1) {
        OV4689_RegWrite(0x3208, 0x11);  // group 1 end
        OV4689_RegWrite(0x3208, 0xA1);  // group 1 delay latch
    }
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: OV4689_SetSlowShutterReg
 *
 *  @Description:: Apply slow shutter setting
 *
 *  @Input      ::
 *      SlowShutterCtrl:    Electronic slow shutter control
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static void OV4689_SetSlowShutterReg(UINT32 SlowShutterCtrl)
{
    UINT32 TargetFrameLengthLines;

    if (SlowShutterCtrl < 1)
        SlowShutterCtrl = 1;

    TargetFrameLengthLines = OV4689Ctrl.ModeInfo.FrameLengthLines * SlowShutterCtrl;

    OV4689_RegWrite(0x380e, (TargetFrameLengthLines & 0x0000ff00) >> 8);
    OV4689_RegWrite(0x380f, (TargetFrameLengthLines & 0x000000ff));

    /* Update current frame rate information */
    OV4689Ctrl.Status.ModeInfo.FrameLengthLines = TargetFrameLengthLines;
    OV4689Ctrl.Status.ModeInfo.NumExposureStepPerFrame = TargetFrameLengthLines;
    OV4689Ctrl.Status.ModeInfo.FrameTime.FrameRate.TimeScale = OV4689Ctrl.ModeInfo.FrameTime.FrameRate.TimeScale / SlowShutterCtrl;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: OV4689_SetStandbyOn
 *
 *  @Description:: Configure sensor into standby mode
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static void OV4689_SetStandbyOn(void)
{
    UINT32 Delay;

    Delay = OV4689Ctrl.Status.ModeInfo.FrameTime.FrameRate.TimeScale /
            OV4689Ctrl.Status.ModeInfo.FrameTime.FrameRate.NumUnitsInTick;

    OV4689_RegWrite(0x0100, 0x0);

    AmbaKAL_TaskSleep(Delay);
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: OV4689_SetStandbyOff
 *
 *  @Description:: Configure sensor to leave standby mode.
 *
 *  @Input      ::
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static void OV4689_SetStandbyOff(void)
{
    OV4689_RegWrite(0x0100, 0x1);
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: OV4689_ChangeReadoutMode
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
static int OV4689_ChangeReadoutMode(UINT16 Mode)
{
    int i = 0;
    UINT16 FrameLengthLine;

    for (i = 0; i < OV4689_NUM_READOUT_MODE_REG; i ++) {
        OV4689_RegWrite(OV4689RegTable[i].Addr, OV4689RegTable[i].Data[OV4689ModeInfoList[Mode].ReadoutMode]);
    }

#if 0
    for(i = 0; i < OV4689_NUM_READOUT_MODE_REG; i ++) {
        UINT8 Rval = 0;
        OV4689_RegRead(OV4689RegTable[i].Addr, &Rval);
        AmbaPrint("Reg Read: Addr: 0x%x, data: 0x%x", OV4689RegTable[i].Addr, Rval);
    }
#endif

    /* Adjust frame rate to current mode setting */
    FrameLengthLine = OV4689ModeInfoList[Mode].FrameTiming.FrameLengthLines & 0xffff;

    if (OV4689HdrInfo[Mode].HdrType != AMBA_SENSOR_HDR_NONE)
        FrameLengthLine = FrameLengthLine / 2;

    OV4689_RegWrite(0x380e, (FrameLengthLine >> 8) & 0xff);
    OV4689_RegWrite(0x380f, FrameLengthLine & 0xff);

    /* Reset current AE information */
    memset(&OV4689Ctrl.CurrentAEInfo, 0, sizeof(OV4689_CURRENT_AE_INFO_s));

    return OK;
}

static void OV4689_InitSeq(void)
{
    OV4689_RegWrite(0x0103, 0x01);
    OV4689_RegWrite(0x3638, 0x00);
    OV4689_RegWrite(0x0304, 0x03);
    OV4689_RegWrite(0x030b, 0x00);
    OV4689_RegWrite(0x030d, 0x1e);
    OV4689_RegWrite(0x030e, 0x04);
    OV4689_RegWrite(0x030f, 0x01);
    OV4689_RegWrite(0x0312, 0x01);
    OV4689_RegWrite(0x031e, 0x00);
    OV4689_RegWrite(0x3000, 0x20);
    OV4689_RegWrite(0x3002, 0x00);
    OV4689_RegWrite(0x3020, 0x93);
    OV4689_RegWrite(0x3021, 0x03);
    OV4689_RegWrite(0x3022, 0x01);
    OV4689_RegWrite(0x3031, 0x0a);
    OV4689_RegWrite(0x303f, 0x0c);
    OV4689_RegWrite(0x3305, 0xf1);
    OV4689_RegWrite(0x3307, 0x04);
    OV4689_RegWrite(0x3309, 0x29);
    OV4689_RegWrite(0x3500, 0x00);
    OV4689_RegWrite(0x3502, 0x00);
    OV4689_RegWrite(0x3503, 0x74); // Shutter reflection_timing = 2 & gain reflection_timing = 1
    OV4689_RegWrite(0x3504, 0x00);
    OV4689_RegWrite(0x3505, 0x00);
    OV4689_RegWrite(0x3506, 0x00);
    OV4689_RegWrite(0x3507, 0x00);
    OV4689_RegWrite(0x3508, 0x00);
    OV4689_RegWrite(0x3509, 0x80);
    OV4689_RegWrite(0x350a, 0x00);
    OV4689_RegWrite(0x350b, 0x00);
    OV4689_RegWrite(0x350c, 0x00);
    OV4689_RegWrite(0x350d, 0x00);
    OV4689_RegWrite(0x350e, 0x00);
    OV4689_RegWrite(0x350f, 0x80);
    OV4689_RegWrite(0x3510, 0x00);
    OV4689_RegWrite(0x3511, 0x00);
    OV4689_RegWrite(0x3512, 0x00);
    OV4689_RegWrite(0x3513, 0x00);
    OV4689_RegWrite(0x3514, 0x00);
    OV4689_RegWrite(0x3515, 0x80);
    OV4689_RegWrite(0x3516, 0x00);
    OV4689_RegWrite(0x3517, 0x00);
    OV4689_RegWrite(0x3518, 0x00);
    OV4689_RegWrite(0x3519, 0x00);
    OV4689_RegWrite(0x351a, 0x00);
    OV4689_RegWrite(0x351b, 0x80);
    OV4689_RegWrite(0x351c, 0x00);
    OV4689_RegWrite(0x351d, 0x00);
    OV4689_RegWrite(0x351e, 0x00);
    OV4689_RegWrite(0x351f, 0x00);
    OV4689_RegWrite(0x3520, 0x00);
    OV4689_RegWrite(0x3521, 0x80);
    OV4689_RegWrite(0x3522, 0x08);
    OV4689_RegWrite(0x3524, 0x08);
    OV4689_RegWrite(0x3526, 0x08);
    OV4689_RegWrite(0x3528, 0x08);
    OV4689_RegWrite(0x352a, 0x08);
    OV4689_RegWrite(0x3602, 0x00);
    OV4689_RegWrite(0x3603, 0x40);
    OV4689_RegWrite(0x3604, 0x02);
    OV4689_RegWrite(0x3605, 0x00);
    OV4689_RegWrite(0x3606, 0x00);
    OV4689_RegWrite(0x3607, 0x00);
    OV4689_RegWrite(0x3609, 0x12);
    OV4689_RegWrite(0x360a, 0x40);
    OV4689_RegWrite(0x360c, 0x08);
    OV4689_RegWrite(0x360f, 0xe5);
    OV4689_RegWrite(0x3608, 0x8f);
    OV4689_RegWrite(0x3611, 0x00);
    OV4689_RegWrite(0x3613, 0xf7);
    OV4689_RegWrite(0x3616, 0x58);
    OV4689_RegWrite(0x3619, 0x99);
    OV4689_RegWrite(0x361b, 0x60);
    OV4689_RegWrite(0x361c, 0x7a);
    OV4689_RegWrite(0x361e, 0x79);
    OV4689_RegWrite(0x361f, 0x02);
    OV4689_RegWrite(0x3633, 0x10);
    OV4689_RegWrite(0x3634, 0x10);
    OV4689_RegWrite(0x3635, 0x10);
    OV4689_RegWrite(0x3636, 0x15);
    OV4689_RegWrite(0x3646, 0x86);
    OV4689_RegWrite(0x364a, 0x0b);
    OV4689_RegWrite(0x3700, 0x17);
    OV4689_RegWrite(0x3701, 0x22);
    OV4689_RegWrite(0x3703, 0x10);
    OV4689_RegWrite(0x370a, 0x37);
    OV4689_RegWrite(0x3705, 0x00);
    OV4689_RegWrite(0x3706, 0x63);
    OV4689_RegWrite(0x3709, 0x3c);
    OV4689_RegWrite(0x370b, 0x01);
    OV4689_RegWrite(0x370c, 0x30);
    OV4689_RegWrite(0x3710, 0x24);
    OV4689_RegWrite(0x3711, 0x0c);
    OV4689_RegWrite(0x3716, 0x00);
    OV4689_RegWrite(0x3720, 0x28);
    OV4689_RegWrite(0x3729, 0x7b);
    OV4689_RegWrite(0x372a, 0x84);
    OV4689_RegWrite(0x372b, 0xbd);
    OV4689_RegWrite(0x372c, 0xbc);
    OV4689_RegWrite(0x372e, 0x52);
    OV4689_RegWrite(0x373c, 0x0e);
    OV4689_RegWrite(0x373e, 0x33);
    OV4689_RegWrite(0x3743, 0x10);
    OV4689_RegWrite(0x3744, 0x88);
    OV4689_RegWrite(0x3745, 0xc0);
    OV4689_RegWrite(0x374c, 0x00);
    OV4689_RegWrite(0x374e, 0x23);
    OV4689_RegWrite(0x3751, 0x7b);
    OV4689_RegWrite(0x3752, 0x84);
    OV4689_RegWrite(0x3753, 0xbd);
    OV4689_RegWrite(0x3754, 0xbc);
    OV4689_RegWrite(0x3756, 0x52);
    OV4689_RegWrite(0x375c, 0x00);
    OV4689_RegWrite(0x3760, 0x00);
    OV4689_RegWrite(0x3761, 0x00);
    OV4689_RegWrite(0x3762, 0x00);
    OV4689_RegWrite(0x3763, 0x00);
    OV4689_RegWrite(0x3764, 0x00);
    OV4689_RegWrite(0x3767, 0x04);
    OV4689_RegWrite(0x3768, 0x04);
    OV4689_RegWrite(0x3769, 0x08);
    OV4689_RegWrite(0x376a, 0x08);
    OV4689_RegWrite(0x376c, 0x00);
    OV4689_RegWrite(0x376d, 0x00);
    OV4689_RegWrite(0x376e, 0x00);
    OV4689_RegWrite(0x3773, 0x00);
    OV4689_RegWrite(0x3774, 0x51);
    OV4689_RegWrite(0x3776, 0xbd);
    OV4689_RegWrite(0x3777, 0xbd);
    OV4689_RegWrite(0x3781, 0x18);
    OV4689_RegWrite(0x3783, 0x25);
    OV4689_RegWrite(0x3798, 0x1b);
    OV4689_RegWrite(0x382b, 0x01);
    OV4689_RegWrite(0x382d, 0x7f);
    OV4689_RegWrite(0x3837, 0x00);
    OV4689_RegWrite(0x3841, 0x02);
    OV4689_RegWrite(0x3846, 0x08);
    OV4689_RegWrite(0x3847, 0x07);
    OV4689_RegWrite(0x3d85, 0x36);
    OV4689_RegWrite(0x3d8c, 0x71);
    OV4689_RegWrite(0x3d8d, 0xcb);
    OV4689_RegWrite(0x3f0a, 0x00);
    OV4689_RegWrite(0x4000, 0x71); // BLC control
    OV4689_RegWrite(0x4002, 0x04);
    OV4689_RegWrite(0x400e, 0x00);
    OV4689_RegWrite(0x4011, 0x00);
    OV4689_RegWrite(0x401a, 0x00);
    OV4689_RegWrite(0x401b, 0x00);
    OV4689_RegWrite(0x401c, 0x00);
    OV4689_RegWrite(0x401d, 0x00);
    OV4689_RegWrite(0x401f, 0x00);
    OV4689_RegWrite(0x4020, 0x00);
    OV4689_RegWrite(0x4021, 0x10);
    OV4689_RegWrite(0x4028, 0x00);
    OV4689_RegWrite(0x4029, 0x02);
    OV4689_RegWrite(0x402c, 0x02);
    OV4689_RegWrite(0x402d, 0x02);
    OV4689_RegWrite(0x4302, 0xff);
    OV4689_RegWrite(0x4303, 0xff);
    OV4689_RegWrite(0x4304, 0x00);
    OV4689_RegWrite(0x4305, 0x00);
    OV4689_RegWrite(0x4306, 0x00);
    OV4689_RegWrite(0x4308, 0x02);
    OV4689_RegWrite(0x4500, 0x6c);
    OV4689_RegWrite(0x4501, 0xc4);
    OV4689_RegWrite(0x4503, 0x01);
    OV4689_RegWrite(0x4800, 0x04);
    OV4689_RegWrite(0x4813, 0x08);
    OV4689_RegWrite(0x481f, 0x40);
    OV4689_RegWrite(0x4829, 0x78);
    OV4689_RegWrite(0x4b00, 0x2a);
    OV4689_RegWrite(0x4b0d, 0x00);
    OV4689_RegWrite(0x4d00, 0x04);
    OV4689_RegWrite(0x4d01, 0x42);
    OV4689_RegWrite(0x4d02, 0xd1);
    OV4689_RegWrite(0x4d03, 0x93);
    OV4689_RegWrite(0x4d04, 0xf5);
    OV4689_RegWrite(0x4d05, 0xc1);
    OV4689_RegWrite(0x5000, 0xe3); // Disable WB gain and enable digital gain
    OV4689_RegWrite(0x5001, 0x11);
    OV4689_RegWrite(0x5004, 0x00);
    OV4689_RegWrite(0x500a, 0x00);
    OV4689_RegWrite(0x500b, 0x00);
    OV4689_RegWrite(0x5032, 0x00);
    OV4689_RegWrite(0x5040, 0x00);
    OV4689_RegWrite(0x5500, 0x00);
    OV4689_RegWrite(0x5501, 0x10);
    OV4689_RegWrite(0x5502, 0x01);
    OV4689_RegWrite(0x5503, 0x0f);
    OV4689_RegWrite(0x8000, 0x00);
    OV4689_RegWrite(0x8001, 0x00);
    OV4689_RegWrite(0x8002, 0x00);
    OV4689_RegWrite(0x8003, 0x00);
    OV4689_RegWrite(0x8004, 0x00);
    OV4689_RegWrite(0x8005, 0x00);
    OV4689_RegWrite(0x8006, 0x00);
    OV4689_RegWrite(0x8007, 0x00);
    OV4689_RegWrite(0x8008, 0x00);
    OV4689_RegWrite(0x3638, 0x00);
}

static void OV4689_HdrSeq(UINT32 SensorMode)
{
    if (OV4689HdrInfo[SensorMode].HdrType != AMBA_SENSOR_HDR_NONE) {
        /* NEW_STG_EN */
        OV4689_RegWrite(0x5001, 0xd1);
        OV4689_RegWrite(0x3837, 0x80);
        OV4689_RegWrite(0x3806, 0x05);
        OV4689_RegWrite(0x3807, 0xfb);
        OV4689_RegWrite(0x3764, 0x01);
        OV4689_RegWrite(0x3760, 0x01);
        OV4689_RegWrite(0x3761, 0x2C);
        OV4689_RegWrite(0x3762, 0x00);
        OV4689_RegWrite(0x3763, 0x00);
        OV4689_RegWrite(0x3500, 0x00);
        OV4689_RegWrite(0x3501, 0x30);
        OV4689_RegWrite(0x3502, 0x00);
        OV4689_RegWrite(0x350a, 0x00);
        OV4689_RegWrite(0x350b, 0x0f);
        OV4689_RegWrite(0x350c, 0x80);
        /*NEW_STG_HDR2 */
        OV4689_RegWrite(0x3760, 0x01);
        OV4689_RegWrite(0x3761, 0x2C);
        OV4689_RegWrite(0x3762, 0x00);
        OV4689_RegWrite(0x3763, 0x00);
        OV4689_RegWrite(0x3500, 0x00);
        OV4689_RegWrite(0x3501, 0x30);
        OV4689_RegWrite(0x3502, 0x00);
        OV4689_RegWrite(0x350a, 0x00);
        OV4689_RegWrite(0x350b, 0x06);
        OV4689_RegWrite(0x350c, 0x00);
        OV4689_RegWrite(0x3841, 0x03);
    } else {
        /* HDR_OFF */
        OV4689_RegWrite(0x5001, 0x11);
        OV4689_RegWrite(0x3837, 0x00);
        OV4689_RegWrite(0x3764, 0x00);
        OV4689_RegWrite(0x3760, 0x00);
        OV4689_RegWrite(0x3761, 0x00);
        OV4689_RegWrite(0x3762, 0x00);
        OV4689_RegWrite(0x3763, 0x00);
        OV4689_RegWrite(0x3500, 0x00);
        OV4689_RegWrite(0x3502, 0x00);
        OV4689_RegWrite(0x350a, 0x00);
        OV4689_RegWrite(0x350b, 0x00);
        OV4689_RegWrite(0x350c, 0x00);
        OV4689_RegWrite(0x3841, 0x02);
    }
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: OV4689_Init
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
static int OV4689_Init(AMBA_DSP_CHANNEL_ID_u Chan)
{
    const OV4689_FRAME_TIMING_s *pFrameTime = &OV4689ModeInfoList[0].FrameTiming;

    /* For Second VIN */
    if ((AMBA_VIN_CHANNEL_e)Chan.Bits.VinID == AMBA_VIN_CHANNEL1) {
        AmbaPLL_SetEnetClkConfig(1);
        AmbaPLL_SetEnetClk(pFrameTime->InputClk);               /* The default sensor input clock frequency */

    /* For Main VIN */
    } else {
        AmbaPLL_SetSensorClk(pFrameTime->InputClk);             /* The default sensor input clock frequency */
        AmbaPLL_SetSensorClkDir(AMBA_PLL_SENSOR_CLK_OUTPUT);    /* Config clock output to sensor (or sensor SPI won't work) */
    }

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: OV4689_Enable
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
static int OV4689_Enable(AMBA_DSP_CHANNEL_ID_u Chan)
{
    if (OV4689Ctrl.Status.DevStat.Bits.Sensor0Power == 1)
        return OK;

    OV4689_HardwareReset();

    /* Config MIPI Phy*/
    AmbaDSP_VinPhySetMIPI(Chan.Bits.VinID);

    OV4689Ctrl.Status.DevStat.Bits.Sensor0Power   = 1;
    OV4689Ctrl.Status.DevStat.Bits.Sensor0Standby = 1;

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: OV4689_Disable
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
static int OV4689_Disable(AMBA_DSP_CHANNEL_ID_u Chan)
{
    if (OV4689Ctrl.Status.DevStat.Bits.Sensor0Power == 0)
        return OK;

    OV4689_SetStandbyOn();

    OV4689Ctrl.Status.DevStat.Bits.Sensor0Power = 0;

    return OK;
}


/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: OV4689_GetStatus
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
static int OV4689_GetStatus(AMBA_DSP_CHANNEL_ID_u Chan, AMBA_SENSOR_STATUS_INFO_s *pStatus)
{
    if (pStatus == NULL || OV4689Ctrl.Status.DevStat.Bits.Sensor0Power == 0)
        return NG;

    OV4689Ctrl.Status.TimeToAllPelExposed = 0;  /* TODO: for flash light or mechanical shutter control */

    memcpy(pStatus, &OV4689Ctrl.Status, sizeof(AMBA_SENSOR_STATUS_INFO_s));

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: OV4689_GetModeInfo
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
static int OV4689_GetModeInfo(AMBA_DSP_CHANNEL_ID_u Chan, AMBA_SENSOR_MODE_ID_u Mode, AMBA_SENSOR_MODE_INFO_s *pModeInfo)
{
    UINT16 SensorMode = Mode.Bits.Mode; // TODO

    if (SensorMode == AMBA_SENSOR_CURRENT_MODE)
        SensorMode = OV4689Ctrl.Status.ModeInfo.Mode.Bits.Mode;

    if (SensorMode >= AMBA_SENSOR_OV4689_NUM_MODE || pModeInfo == NULL)
        return NG;

    OV4689_PrepareModeInfo(SensorMode, pModeInfo);

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: OV4689_GetDeviceInfo
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
static int OV4689_GetDeviceInfo(AMBA_DSP_CHANNEL_ID_u Chan, AMBA_SENSOR_DEVICE_INFO_s *pDeviceInfo)
{
    if (pDeviceInfo == NULL)
        return NG;

    memcpy(pDeviceInfo, &OV4689DeviceInfo, sizeof(AMBA_SENSOR_DEVICE_INFO_s));

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: OV4689_GetCurrentGainFactor
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
static int OV4689_GetCurrentGainFactor(AMBA_DSP_CHANNEL_ID_u Chan, float *pGainFactor)
{
    UINT32 CurrentAgcCtrl;
    UINT32 CurrentDgcCtrl;
    float AgcFactor = 1.0, DgcFactor = 1.0;
    UINT8 AgcR3508 = 0;
    UINT8 AgcR3509 = 0;

    if (Chan.Bits.HdrID == 0x2) {
        CurrentAgcCtrl = OV4689Ctrl.CurrentAEInfo.CurrentAgcCtrl[1]; // short exposure frame
        CurrentDgcCtrl = OV4689Ctrl.CurrentAEInfo.CurrentDgcCtrl[1]; // short exposure frame
    } else {
        CurrentAgcCtrl = OV4689Ctrl.CurrentAEInfo.CurrentAgcCtrl[0]; // long exposure frame
        CurrentDgcCtrl = OV4689Ctrl.CurrentAEInfo.CurrentDgcCtrl[0]; // long exposure frame
    }

    AgcR3508 = (CurrentAgcCtrl & 0x0000ff00) >> 8;
    AgcR3509 = (CurrentAgcCtrl & 0x000000ff);

    if (AgcR3508 == 0)
        AgcFactor = (float)AgcR3509 / 128;
    else if (AgcR3508 == 1)
        AgcFactor = (float)(AgcR3509 + 8) / 64;
    else if (AgcR3508 == 3)
        AgcFactor = (float)(AgcR3509 + 12) / 32;
    else if (AgcR3508 == 7)
        AgcFactor = (float)(AgcR3509 + 8) / 16;

    DgcFactor = (float)((((CurrentDgcCtrl & 0xffff0000) >> 16) * 64) + ((CurrentDgcCtrl & 0x0000ff00) >> 8)) / 2048;

    *pGainFactor = AgcFactor * DgcFactor;

    //AmbaPrint("GainFactor = %f, AgcFactor = %f, DgcFactor = %f", *pGainFactor, AgcFactor, DgcFactor);

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: OV4689_GetCurrentShutterSpeed
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
static int OV4689_GetCurrentShutterSpeed(AMBA_DSP_CHANNEL_ID_u Chan, float *pExposureTime)
{
    if (Chan.Bits.HdrID == 0x2)
        *pExposureTime = OV4689Ctrl.Status.ModeInfo.RowTime * OV4689Ctrl.CurrentAEInfo.CurrentShutterCtrl[1];
    else
        *pExposureTime = OV4689Ctrl.Status.ModeInfo.RowTime * OV4689Ctrl.CurrentAEInfo.CurrentShutterCtrl[0];

    //AmbaPrint("ExposureTime = %f", *pExposureTime);

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: OV4689_GetHdrInfo
 *
 *  @Description:: Get Sensor Current Exposure Time
 *
 *  @Input      ::
 *      Chan:            Vin ID and sensor ID
 *      *ShutterCtrl:    Electronic shutter control
 *  @Output     ::
 *      pExposureTime:   senosr hdr information
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static int OV4689_GetHdrInfo(AMBA_DSP_CHANNEL_ID_u Chan, UINT32 *pShutterCtrl, AMBA_SENSOR_HDR_INFO_s *pHdrInfo)
{
    memcpy(pHdrInfo, &OV4689Ctrl.Status.ModeInfo.HdrInfo, sizeof(AMBA_SENSOR_HDR_INFO_s));

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: OV4689_ConvertGainFactor
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
static int OV4689_ConvertGainFactor(AMBA_DSP_CHANNEL_ID_u Chan, float DesiredFactor, UINT32 *pActualFactor, UINT32 *pAnalogGainCtrl, UINT32 *pDigitalGainCtrl)
{
    float AgcFactor = 1.0, DgcFactor = 1.0, ActualAgcFactor = 1.0, ActualDgcFactor = 1.0;
    UINT8 AgcR3508 = 0;
    UINT8 AgcR3509 = 0;
    UINT8 DgcMsb = 0;
    UINT8 DgcRemainder = 0;
    UINT16 DgcQuotient = 0;

    if (DesiredFactor > 15.5) {
        AgcFactor = 15.5;  // Maximum Agc
        DgcFactor = DesiredFactor / 15.5;
    } else {
        AgcFactor = DesiredFactor;
        DgcFactor = 1.0;
    }

    if (DgcFactor > 15.99)
        DgcFactor = 15.99;  // Maximum Dgc

    /* Agc */
    if ((AgcFactor >= 1) && (AgcFactor < 2)) {
        AgcR3508 = 0;
        AgcR3509 = (UINT8)(AgcFactor * 128);
        ActualAgcFactor = (float)AgcR3509 / 128;
    } else if ((AgcFactor >= 2) &&  (AgcFactor < 4)) {
        AgcR3508 = 1;
        AgcR3509 = (UINT8)((AgcFactor * 64) - 8);
        ActualAgcFactor = (float)(AgcR3509 + 8) / 64;
    } else if ((AgcFactor >= 4) && (AgcFactor < 8)) {
        AgcR3508 = 3;
        AgcR3509 = (UINT8)((AgcFactor * 32) - 12);
        ActualAgcFactor = (float)(AgcR3509 + 12) / 32;
    } else if ((AgcFactor >= 8) && (AgcFactor <= 15.5)) {
        AgcR3508 = 7;
        AgcR3509 = (UINT8)((AgcFactor * 16) - 8);
        ActualAgcFactor = (float)(AgcR3509 + 8) / 16;
    }

    /* Dgc */
    DgcMsb = (UINT8)(DgcFactor * 2048 / 256);
    DgcRemainder = (UINT8)((int)(DgcFactor * 2048) % 64);
    DgcQuotient = (UINT16)((DgcFactor * 2048) / 64);
    ActualDgcFactor = ((float)DgcQuotient * 64 + DgcRemainder) / 2048;
    *pDigitalGainCtrl = (DgcQuotient << 16) + (DgcRemainder << 8) + DgcMsb;
    *pAnalogGainCtrl = (AgcR3508 << 8) + AgcR3509 + (DgcRemainder << 24) + (DgcMsb << 16);

    *pActualFactor = (UINT32)(ActualAgcFactor * ActualDgcFactor * 1048576);

    //AmbaPrint("DesiredFactor = %f, , ActualAgcFactor = %f, ActualDgcFactor = %f", DesiredFactor, ActualAgcFactor, ActualDgcFactor);
    //AmbaPrint("ActualFactor << 20 = 0x%08x", *pActualFactor);
    //AmbaPrint("AnalogGainCtrl = 0x%08x, DigitalGainCtrl = 0x%08x", *pAnalogGainCtrl, *pDigitalGainCtrl);

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: OV4689_ConvertShutterSpeed
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
static int OV4689_ConvertShutterSpeed(AMBA_DSP_CHANNEL_ID_u Chan, float ExposureTime, UINT32 *pShutterCtrl)
{
    AMBA_SENSOR_MODE_INFO_s *pModeInfo = &OV4689Ctrl.ModeInfo;

    if (pShutterCtrl == NULL)
        return NG;

    *pShutterCtrl = (UINT32)(ExposureTime / pModeInfo->RowTime);

    /* For HDR mode (not support slow-shutter)*/
    if (Chan.Bits.HdrID != 0) {

        *pShutterCtrl = (*pShutterCtrl % 2) ? *pShutterCtrl - 1 : *pShutterCtrl;

        /* For short exposure frame */
        if ((Chan.Bits.HdrID == 0x2) && (*pShutterCtrl >= pModeInfo->HdrInfo.ChannelInfo[1].MaxExposureLine))
            *pShutterCtrl = pModeInfo->HdrInfo.ChannelInfo[1].MaxExposureLine;
        /* For long exposure frame or default */
        else if (*pShutterCtrl >= pModeInfo->HdrInfo.ChannelInfo[0].MaxExposureLine)
            *pShutterCtrl = pModeInfo->HdrInfo.ChannelInfo[0].MaxExposureLine;

        /* Exposure lines needs to be bigger than 2 */
        if (*pShutterCtrl <= 2)
            *pShutterCtrl = 2;

    /* For none-HDR mode */
    } else {

        UINT32 ExposureFrames, ExposureTimeMaxMargin;

        ExposureFrames = (*pShutterCtrl / pModeInfo->NumExposureStepPerFrame);
        ExposureFrames = (*pShutterCtrl % pModeInfo->NumExposureStepPerFrame) ? ExposureFrames + 1 : ExposureFrames;
        ExposureTimeMaxMargin = ExposureFrames * pModeInfo->NumExposureStepPerFrame - 8;

        /* Exposure lines needs to be smaller than VTS - 8 */
        if (*pShutterCtrl >= ExposureTimeMaxMargin)
           *pShutterCtrl = ExposureTimeMaxMargin;

        /* Exposure lines needs to be bigger 1 */
        if (*pShutterCtrl <= 1)
            *pShutterCtrl = 1;
    }

    //AmbaPrint("ExposureTime:%f, ShutterCtrl:%d", ExposureTime, *pShutterCtrl);

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: OV4689_SetAnalogGainCtrl
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
static int OV4689_SetAnalogGainCtrl(AMBA_DSP_CHANNEL_ID_u Chan, UINT32 AnalogGainCtrl)
{
    OV4689_SetAnalogGainReg(AnalogGainCtrl);

    /* Update current AGC control */
    OV4689Ctrl.CurrentAEInfo.CurrentAgcCtrl[0] = AnalogGainCtrl;

    //AmbaPrint("Set Again: 0x%08X", AnalogGainCtrl);

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: OV4689_SetDigitalGainCtrl
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
static int OV4689_SetDigitalGainCtrl(AMBA_DSP_CHANNEL_ID_u Chan, UINT32 DigitalGainCtrl)
{
    OV4689_SetDigitalGainReg(DigitalGainCtrl);

    /* Update current DGC control */
    OV4689Ctrl.CurrentAEInfo.CurrentDgcCtrl[0] = DigitalGainCtrl;

    //AmbaPrint("Set Dgain: 0x%08X", DigitalGainCtrl);

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: OV4689_SetShutterCtrl
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
static int OV4689_SetShutterCtrl(AMBA_DSP_CHANNEL_ID_u Chan, UINT32 ShutterCtrl)
{
    OV4689_SetShutterReg(ShutterCtrl);

    /* Update current shutter control */
    OV4689Ctrl.CurrentAEInfo.CurrentShutterCtrl[0] = ShutterCtrl;

    //AmbaPrint("Set Shutter: 0x%08X", ShutterCtrl);

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: OV4689_SetHdrAnalogGainCtrl
 *
 *  @Description:: Set HDR analog gain control
 *
 *  @Input      ::
 *      Chan:               Vin ID and sensor ID
 *      pAnalogGainCtrl:    Analog gain control for achievable gain factor
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static int OV4689_SetHdrAnalogGainCtrl(AMBA_DSP_CHANNEL_ID_u Chan, UINT32 *pAnalogGainCtrl)
{
    OV4689_SetHdrAnalogGainReg(Chan, pAnalogGainCtrl);

    /* Update current AGC control */
    if (Chan.Bits.HdrID & 0x1)
        OV4689Ctrl.CurrentAEInfo.CurrentAgcCtrl[0] = *pAnalogGainCtrl;

    if (Chan.Bits.HdrID & 0x2)
        OV4689Ctrl.CurrentAEInfo.CurrentAgcCtrl[1] = *(pAnalogGainCtrl + 1);

    //AmbaPrint("Set Again[0]: 0x%08X, Again[1]: 0x%08X", *pAnalogGainCtrl, *(pAnalogGainCtrl + 1));

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: OV4689_SetHdrDigitalGainCtrl
 *
 *  @Description:: Set HDR digital gain control
 *
 *  @Input      ::
 *      Chan:               Vin ID and sensor ID
 *      pDigitalGainCtrl:   Digital gain control for achievable gain factor
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static int OV4689_SetHdrDigitalGainCtrl(AMBA_DSP_CHANNEL_ID_u Chan, UINT32 *pDigitalGainCtrl)
{
    OV4689_SetHdrDigitalGainReg(Chan, pDigitalGainCtrl);

    /* Update current DGC control */
    if (Chan.Bits.HdrID & 0x1)
        OV4689Ctrl.CurrentAEInfo.CurrentDgcCtrl[0] = *pDigitalGainCtrl;

    if (Chan.Bits.HdrID & 0x2)
        OV4689Ctrl.CurrentAEInfo.CurrentDgcCtrl[1] = *(pDigitalGainCtrl + 1);

    //AmbaPrint("Set Dgain[0]: 0x%08X, Dgain[1]: 0x%08X", *pDigitalGainCtrl, *(pDigitalGainCtrl + 1));

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: OV4689_SetHdrShutterCtrl
 *
 *  @Description:: set HDR shutter control
 *
 *  @Input      ::
 *      Chan:           Vin ID and sensor ID
 *      pShutterCtrl:   Electronic shutter control
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static int OV4689_SetHdrShutterCtrl(AMBA_DSP_CHANNEL_ID_u Chan, UINT32 *pShutterCtrl)
{
    OV4689_SetHdrShutterReg(Chan, pShutterCtrl);

    /* Update current shutter control */
    if (Chan.Bits.HdrID & 0x1)
        OV4689Ctrl.CurrentAEInfo.CurrentShutterCtrl[0] = *pShutterCtrl;

    if (Chan.Bits.HdrID & 0x2)
        OV4689Ctrl.CurrentAEInfo.CurrentShutterCtrl[1] = *(pShutterCtrl + 1);

    //AmbaPrint("Set Shutter[0]: 0x%08X, Shutter[1]: 0x%08X", *pShutterCtrl, *(pShutterCtrl + 1));

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: OV4689_SetSlowShutterCtrl
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
static int OV4689_SetSlowShutterCtrl(AMBA_DSP_CHANNEL_ID_u Chan, UINT32 SlowShutterCtrl)
{
    if (OV4689Ctrl.Status.ModeInfo.HdrInfo.HdrType != 0)
        return NG;

    OV4689_SetSlowShutterReg(SlowShutterCtrl);

    //AmbaPrint("Set SlowShutter: 0x%08X", SlowShutterCtrl);

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: OV4689_Config
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
static int OV4689_Config(AMBA_DSP_CHANNEL_ID_u Chan, AMBA_SENSOR_MODE_ID_u Mode, AMBA_SENSOR_ESHUTTER_TYPE_e ElecShutterMode)
{
    AMBA_SENSOR_MODE_INFO_s *pModeInfo = NULL;
    UINT16 SensorMode = Mode.Bits.Mode;
    static UINT8 NotFirstInit = 0;

    if (SensorMode >= AMBA_SENSOR_OV4689_NUM_MODE)
        return NG;

    AmbaPrint("============  [ OV4689 ]  ============");
    AmbaPrint("============ init mode:%d ============", SensorMode);

    OV4689Ctrl.Status.ElecShutterMode = ElecShutterMode;

    pModeInfo  = &OV4689Ctrl.Status.ModeInfo;
    OV4689_PrepareModeInfo(SensorMode, pModeInfo);
    memcpy(&OV4689Ctrl.ModeInfo, pModeInfo, sizeof(AMBA_SENSOR_MODE_INFO_s));

    if (NotFirstInit == 1)
        OV4689_SetStandbyOn();
    else
        NotFirstInit = 1;

    /* Adjust MIPI-Phy parameters */
    AmbaVIN_CalculateMphyConfig(pModeInfo->OutputInfo.DataRate, &OV4689VinConfig.MipiCtrl);

    /* Make VIN not to capture bad frames during readout mode transition */
    AmbaVIN_Reset((AMBA_VIN_CHANNEL_e)Chan.Bits.VinID, AMBA_VIN_MIPI, &OV4689VinConfig.MipiCtrl);

    AmbaKAL_TaskSleep(3);

    AmbaPLL_SetSensorClk(pModeInfo->FrameTime.InputClk);

    AmbaKAL_TaskSleep(3);

    OV4689_InitSeq();
    OV4689_ChangeReadoutMode(SensorMode);
    OV4689_HdrSeq(SensorMode);

    OV4689_SetStandbyOff();

    OV4689_ConfigVin(Chan, pModeInfo);

    OV4689Ctrl.Status.DevStat.Bits.Sensor0Standby = 0;

    /* To avoid setting gain and shutter at the same frame time in HDR mode after mode switch */
    AEModeSwitchFlag = 0;

    return OK;
}

/*-----------------------------------------------------------------------------------------------*/
AMBA_SENSOR_OBJ_s AmbaSensor_OV4689Obj = {
    .Init                   = OV4689_Init,
    .Enable                 = OV4689_Enable,
    .Disable                = OV4689_Disable,
    .Config                 = OV4689_Config,
    .GetStatus              = OV4689_GetStatus,
    .GetModeInfo            = OV4689_GetModeInfo,
    .GetDeviceInfo          = OV4689_GetDeviceInfo,
    .GetCurrentGainFactor   = OV4689_GetCurrentGainFactor,
    .GetCurrentShutterSpeed = OV4689_GetCurrentShutterSpeed,
    .GetHdrInfo             = OV4689_GetHdrInfo,
    .ConvertGainFactor      = OV4689_ConvertGainFactor,
    .ConvertShutterSpeed    = OV4689_ConvertShutterSpeed,
    .SetAnalogGainCtrl      = OV4689_SetAnalogGainCtrl,
    .SetDigitalGainCtrl     = OV4689_SetDigitalGainCtrl,
    .SetShutterCtrl         = OV4689_SetShutterCtrl,
    .SetHdrAnalogGainCtrl   = OV4689_SetHdrAnalogGainCtrl,
    .SetHdrDigitalGainCtrl  = OV4689_SetHdrDigitalGainCtrl,
    .SetHdrShutterCtrl      = OV4689_SetHdrShutterCtrl,
    .SetSlowShutterCtrl     = OV4689_SetSlowShutterCtrl,

    .RegisterRead           = OV4689_RegisterRead,
    .RegisterWrite          = OV4689_RegisterWrite,
};
