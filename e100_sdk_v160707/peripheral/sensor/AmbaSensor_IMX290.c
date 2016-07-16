/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaSensor_IMX290.c
 *
 *  @Copyright      :: Copyright (C) 2015 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Control APIs of SONY IMX290 CMOS sensor with LVDS interface
 *
\*-------------------------------------------------------------------------------------------------------------------*/

#include <AmbaSSP.h>
#include <math.h>

#include "AmbaDSP.h"
#include "AmbaVIN.h"
#include "AmbaDSP_VIN.h"

#include "AmbaI2C.h"
#include "AmbaGPIO.h"
#include "AmbaPLL.h"
#include "AmbaSensor.h"
#include "AmbaSensor_IMX290.h"

#include "AmbaPrintk.h"

/*-----------------------------------------------------------------------------------------------*\
 * IMX290 sensor runtime info
\*-----------------------------------------------------------------------------------------------*/
IMX290_CTRL_s IMX290Ctrl = {0};

/*-----------------------------------------------------------------------------------------------*\
 * Configurations for Amba. VIN side
\*-----------------------------------------------------------------------------------------------*/
static AMBA_DSP_VIN_SLVS_PIN_PAIR_e IMX290LvdsLaneMux[][AMBA_DSP_NUM_VIN_SLVS_DATA_LANE] = {
    [IMX290_LVDS_8CH]   = {
        AMBA_DSP_VIN_PIN_SD_LVDS_0,
        AMBA_DSP_VIN_PIN_SD_LVDS_1,
        AMBA_DSP_VIN_PIN_SD_LVDS_2,
        AMBA_DSP_VIN_PIN_SD_LVDS_3,
        AMBA_DSP_VIN_PIN_SD_LVDS_4,
        AMBA_DSP_VIN_PIN_SD_LVDS_5,
        AMBA_DSP_VIN_PIN_SD_LVDS_6,
        AMBA_DSP_VIN_PIN_SD_LVDS_7
    },
    [IMX290_LVDS_4CH]   = {
        AMBA_DSP_VIN_PIN_SD_LVDS_0,
        AMBA_DSP_VIN_PIN_SD_LVDS_1,
        AMBA_DSP_VIN_PIN_SD_LVDS_2,
        AMBA_DSP_VIN_PIN_SD_LVDS_3,
    },
    [IMX290_LVDS_2CH]   = {
        AMBA_DSP_VIN_PIN_SD_LVDS_0,
        AMBA_DSP_VIN_PIN_SD_LVDS_1,
    },
};

static AMBA_DSP_VIN_SLVS_CONFIG_s IMX290VinConfig = {
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
    .pLaneMapping   = NULL,
    .NumActiveLanes = 0,
    .SyncDetectCtrl = {
        .SyncInterleaving   = 0,
        .ITU656Type         = 0,
        .CustomSyncCode     = {
            .PatternAlign   = 1,        /* MSB */
            .SyncCodeMask   = 0xff00,
            .DetectEnable   = {
                .Sol    =  1,
                .Eol    =  1,
                .Sof    =  0,
                .Eof    =  0,
                .Sov    =  1,
                .Eov    =  1,
            },
            .PatternSol = 0x8000,
            .PatternEol = 0x9d00,
            .PatternSof = 0,
            .PatternEof = 0,
            .PatternSov = 0xab00,
            .PatternEov = 0xb600,
        },
    },
    .SplitCtrl = {
        .NumSplits = 0,
        .SplitWidth = 0,
    },
    .RxHvSyncCtrl   = {0, 0, 0, 0},
    .VinTrigPulse   = { {0}, {0} },
    .VinVoutSync    = { {AMBA_DSP_VIN_VOUT_SYNC_FIELD, 5}, {AMBA_DSP_VIN_VOUT_SYNC_FIELD, 5} }
};

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX290_PrepareModeInfo
 *
 *  @Description:: Get sensor mode information
 *
 *  @Input      ::
 *      Mode:   Sensor readout mode number
 *
 *  @Output     ::
 *      pModeInfo:  Details of the specified readout mode
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static void IMX290_PrepareModeInfo(UINT16 SensorMode, AMBA_SENSOR_MODE_INFO_s *pModeInfo)
{
    const IMX290_FRAME_TIMING_s *pFrameTiming = &IMX290ModeInfoList[SensorMode].FrameTiming;
    const AMBA_SENSOR_OUTPUT_INFO_s *pOutputInfo = &IMX290OutputInfo[IMX290ModeInfoList[SensorMode].ReadoutMode];
    const AMBA_SENSOR_INPUT_INFO_s *pInputInfo = &IMX290InputInfo[IMX290ModeInfoList[SensorMode].ReadoutMode];

    pModeInfo->Mode.Data = SensorMode;
    pModeInfo->LineLengthPck = pFrameTiming->Linelengthpck;
    pModeInfo->FrameLengthLines = pFrameTiming->FrameLengthLines;
    pModeInfo->NumExposureStepPerFrame = pFrameTiming->FrameLengthLines;
    pModeInfo->PixelRate = (float) pOutputInfo->DataRate * pOutputInfo->NumDataLanes / pOutputInfo->NumDataBits;
    pModeInfo->RowTime = (float) (pFrameTiming->Linelengthpck / pModeInfo->PixelRate);
    pModeInfo->InternalExposureOffset = 0;
    pModeInfo->FrameTime.InputClk = pFrameTiming->InputClk;
    memcpy(&pModeInfo->FrameTime.FrameRate, &pFrameTiming->FrameRate, sizeof(AMBA_DSP_FRAME_RATE_s));
    memcpy(&pModeInfo->InputInfo, pInputInfo, sizeof(AMBA_SENSOR_INPUT_INFO_s));
    memcpy(&pModeInfo->OutputInfo, pOutputInfo, sizeof(AMBA_SENSOR_OUTPUT_INFO_s));
    memcpy(&pModeInfo->MinFrameRate, &pFrameTiming->FrameRate, sizeof(AMBA_DSP_FRAME_RATE_s));
    memcpy(&pModeInfo->HdrInfo, &IMX290HdrInfo[SensorMode], sizeof(AMBA_SENSOR_HDR_INFO_s)); // HDR information

    /* Updated minimum frame rate limitation */
    if (pModeInfo->HdrInfo.HdrType == 0)
        pModeInfo->MinFrameRate.TimeScale /= 8;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX290_ConfigVin
 *
 *  @Description:: Configure VIN to receive output frames of the new readout mode
 *
 *  @Input      ::
 *      Chan:       Vin ID and sensor ID
 *      pModeInfo:  Details of the specified readout mode
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static void IMX290_ConfigVin(AMBA_DSP_CHANNEL_ID_u Chan, AMBA_SENSOR_MODE_INFO_s *pModeInfo)
{
    AMBA_SENSOR_OUTPUT_INFO_s  *pImgInfo = &pModeInfo->OutputInfo;
    AMBA_DSP_VIN_SLVS_CONFIG_s *pVinCfg = &IMX290VinConfig;
    UINT16 SensorMode = pModeInfo->Mode.Data;

    memcpy(&pVinCfg->Info.FrameRate, &pModeInfo->FrameTime.FrameRate, sizeof(AMBA_DSP_FRAME_RATE_s));
    memcpy(&pVinCfg->Info.DspPhaseShift, &pImgInfo->DspPhaseShift, sizeof(AMBA_DSP_PHASE_SHIFT_CTRL_s));
    pVinCfg->Info.BayerPattern = (AMBA_DSP_SENSOR_PATTERN_e) pImgInfo->CfaPattern;
    pVinCfg->Info.NumDataBits = pImgInfo->NumDataBits;
    pVinCfg->NumActiveLanes = pImgInfo->NumDataLanes;

    switch (pVinCfg->NumActiveLanes) {
    case 8:
        pVinCfg->pLaneMapping = IMX290LvdsLaneMux[IMX290_LVDS_8CH];
        break;
    case 4:
        pVinCfg->pLaneMapping = IMX290LvdsLaneMux[IMX290_LVDS_4CH];
        break;
    case 3:
    default:
        pVinCfg->pLaneMapping = IMX290LvdsLaneMux[IMX290_LVDS_2CH];
        break;
    }

    if(IMX290HdrInfo[SensorMode].HdrType != AMBA_SENSOR_HDR_TYPE_MULTI_SLICE) {
        pVinCfg->RxHvSyncCtrl.NumActivePixels = pImgInfo->OutputWidth;
        pVinCfg->RxHvSyncCtrl.NumActiveLines  = pImgInfo->OutputHeight;
    } else {
        pVinCfg->RxHvSyncCtrl.NumActivePixels = 1;
        pVinCfg->RxHvSyncCtrl.NumActiveLines  = 1;
    }

    pVinCfg->RxHvSyncCtrl.NumTotalPixels  = pModeInfo->LineLengthPck - 1;
    pVinCfg->RxHvSyncCtrl.NumTotalLines   = pModeInfo->FrameLengthLines;

    if(IMX290HdrInfo[SensorMode].HdrType == AMBA_SENSOR_HDR_TYPE_MULTI_SLICE) {

        pVinCfg->RxHvSyncCtrl.NumTotalLines = pModeInfo->FrameLengthLines / 2;
        pVinCfg->SplitCtrl.NumSplits = 0;

        if((SensorMode == AMBA_SENSOR_IMX290_1080P60_10BIT_DLO_2FRAME) || (SensorMode == AMBA_SENSOR_IMX290_1080P50_10BIT_DLO_2FRAME) ||
           (SensorMode == AMBA_SENSOR_IMX290_1080P60_P30_10BIT_DLO_2FRAME) || (SensorMode == AMBA_SENSOR_IMX290_1080P50_P25_10BIT_DLO_2FRAME))
            pVinCfg->SplitCtrl.SplitWidth= 2640;
        else if ((SensorMode == AMBA_SENSOR_IMX290_1080P60_10BIT_DLO_2FRAME_EXTEND_EXP_S) || (SensorMode == AMBA_SENSOR_IMX290_1080P50_10BIT_DLO_2FRAME_EXTEND_EXP_S))
            pVinCfg->SplitCtrl.SplitWidth= 2540;
        else
            pVinCfg->SplitCtrl.SplitWidth= 2200;

    } else {
        pVinCfg->SplitCtrl.NumSplits = 0;
        pVinCfg->SplitCtrl.SplitWidth= 0;
    }

    AmbaDSP_VinPhySetSLVS((AMBA_VIN_CHANNEL_e)Chan.Bits.VinID);
    AmbaDSP_VinConfigSLVS((AMBA_VIN_CHANNEL_e)Chan.Bits.VinID, pVinCfg);
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX290_HardwareReset
 *
 *  @Description:: Reset IMX290 Image Sensor Device
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static inline void IMX290_HardwareReset(void)
{
    AmbaUserGPIO_SensorResetCtrl(0);
    AmbaKAL_TaskSleep(1); /* XCLR Low level pulse width >= 100ns */
    AmbaUserGPIO_SensorResetCtrl(1);
    AmbaKAL_TaskSleep(1);
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX290_RegRW
 *
 *  @Description:: Read/Write sensor registers through SPI bus
 *
 *  @Input      ::
 *      Addr:       Register Address
 *      Data:       Value
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static int IMX290_RegRW(UINT16 Addr, UINT8 Data)
{
    UINT8 TxDataBuf[3];

    TxDataBuf[0] = (UINT8) ((Addr & 0xff00) >> 8);
    TxDataBuf[1] = (UINT8) (Addr & 0x00ff);
    TxDataBuf[2] = Data;

    if (AmbaI2C_Write(AMBA_I2C_CHANNEL0, AMBA_I2C_SPEED_FAST,
                      0x34, 3, TxDataBuf,
                      AMBA_KAL_WAIT_FOREVER) == NG)
        AmbaPrint("I2C does not work!!!!!");

    //AmbaPrint("Addr = 0x%04x, Data = 0x%02x", Addr, Data);

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX290_RegRead
 *
 *  @Description:: Read/Write sensor registers through SPI bus
 *
 *  @Input      ::
 *      Addr:       Register Address
 *      pRxData:    Pointer to data buffer
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static int IMX290_RegRead(UINT16 Addr, UINT8 *pRxData)
{
    UINT16 pTxData[4];

    pTxData[0] = AMBA_I2C_RESTART_FLAG | (0x34);
    pTxData[1] = (Addr >> 8);    /* Register Address [15:8] */
    pTxData[2] = (Addr & 0xff);  /* Register Address [7:0]  */
    pTxData[3] = AMBA_I2C_RESTART_FLAG | (0x35);

    return AmbaI2C_ReadAfterWrite(AMBA_I2C_CHANNEL0, AMBA_I2C_SPEED_STANDARD,
                                  4, (UINT16 *) pTxData, 1, pRxData, AMBA_KAL_WAIT_FOREVER);
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX290_RegisterWrite
 *
 *  @Description:: Write sensor registers through SPI bus
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
static int IMX290_RegisterWrite(AMBA_DSP_CHANNEL_ID_u Chan, UINT16 Addr, UINT16 Data)
{
    UINT8 WriteData = (UINT8)Data;

    IMX290_RegRW(Addr, WriteData);

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX290_RegisterRead
 *
 *  @Description:: Read sensor registers API
 *
 *  @Input      ::
 *      Chan:   Vin ID and sensor ID
 *      Addr:   Register address
 *      pData:  Pointer to data buffer
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *      int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static int IMX290_RegisterRead(AMBA_DSP_CHANNEL_ID_u Chan, UINT16 Addr, UINT16 *pData)
{
    static UINT8 ReadData;

    IMX290_RegRead(Addr, &ReadData);
    *pData = ReadData;

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX290_SetAnalogGainReg
 *
 *  @Description:: Configure sensor analog gain setting
 *
 *  @Input      ::
 *      Chan:      Vin ID and sensor ID
 *      AGC:       Analog gain setting
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static void IMX290_SetAnalogGainReg(AMBA_DSP_CHANNEL_ID_u Chan, UINT32 AGC)
{
    IMX290_RegRW(IMX290_AGAIN, (UINT8)AGC);
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX290_SetShutterReg
 *
 *  @Description:: Apply electronic shutter setting
 *
 *  @Input      ::
 *      NumXhsEshrSpeed:    Integration time in number of XHS period
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static void IMX290_SetShutterReg(UINT32 NumXhsEshrSpeed)
{
    UINT32 SHS1 = IMX290Ctrl.Status.ModeInfo.NumExposureStepPerFrame - NumXhsEshrSpeed;

    IMX290_RegRW(IMX290_SHS1_LSB, (SHS1 & 0xff));
    IMX290_RegRW(IMX290_SHS1_MSB, (SHS1 >> 8) & 0xff);
    IMX290_RegRW(IMX290_SHS1_HSB, (SHS1 >> 16) & 0x03);
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX290_SetSlowShutterReg
 *
 *  @Description:: Apply slow shutter setting
 *
 *  @Input      ::
 *     IntegrationPeriodInFrame: Number of frames in integration period
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static void IMX290_SetSlowShutterReg(UINT32 IntegrationPeriodInFrame)
{
    UINT32 TargetFrameLengthLines;

    if (IntegrationPeriodInFrame < 1)
        IntegrationPeriodInFrame = 1;

    TargetFrameLengthLines = IMX290Ctrl.ModeInfo.FrameLengthLines * IntegrationPeriodInFrame;

    IMX290_RegRW(IMX290_VMAX_LSB, (TargetFrameLengthLines & 0xff));
    IMX290_RegRW(IMX290_VMAX_MSB, (TargetFrameLengthLines >> 8) & 0xff);
    IMX290_RegRW(IMX290_VMAX_HSB, (TargetFrameLengthLines >> 16) & 0x03);

    /* Update frame rate information */
    IMX290Ctrl.Status.ModeInfo.FrameLengthLines = TargetFrameLengthLines;
    IMX290Ctrl.Status.ModeInfo.NumExposureStepPerFrame = TargetFrameLengthLines;
    IMX290Ctrl.Status.ModeInfo.FrameTime.FrameRate.TimeScale = IMX290Ctrl.ModeInfo.FrameTime.FrameRate.TimeScale / IntegrationPeriodInFrame;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX290_SetHdrAnalogGainReg
 *
 *  @Description:: Configure sensor analog gain setting
 *
 *  @Input      ::
 *      Chan:      Vin ID and sensor ID
 *      pAGC:      Pointer to analog gain setting
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static void IMX290_SetHdrAnalogGainReg(AMBA_DSP_CHANNEL_ID_u Chan, UINT32 *pAGC)
{
   IMX290_RegRW(IMX290_AGAIN, (UINT8)*pAGC);
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX290_SetHdrShutterReg
 *
 *  @Description:: Apply electronic shutter setting
 *
 *  @Input      ::
 *      Chan:           Vin ID and sensor ID
 *      pShutterCtrl:   Pointer to electronic shutter control
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static void IMX290_SetHdrShutterReg(AMBA_DSP_CHANNEL_ID_u Chan, UINT32 *pShutterCtrl)
{
    UINT32 SHSX;

    if (Chan.Bits.HdrID & 0x1) {
        SHSX = IMX290Ctrl.Status.ModeInfo.NumExposureStepPerFrame - *pShutterCtrl;
        IMX290_RegRW(IMX290_SHS2_LSB, (SHSX & 0xff));
        IMX290_RegRW(IMX290_SHS2_MSB, (SHSX >> 8) & 0xff);
        IMX290_RegRW(IMX290_SHS2_HSB, (SHSX >> 16) & 0x03);
    }

    if (Chan.Bits.HdrID & 0x2) {
        SHSX = IMX290Ctrl.Status.ModeInfo.HdrInfo.ChannelInfo[1].MaxExposureLine + 2 - *(pShutterCtrl + 1);
        IMX290_RegRW(IMX290_SHS1_LSB, (SHSX & 0xff));
        IMX290_RegRW(IMX290_SHS1_MSB, (SHSX >> 8) & 0xff);
        IMX290_RegRW(IMX290_SHS1_HSB, (SHSX >> 16) & 0x03);
    }
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX290_SetStandbyOn
 *
 *  @Description:: Configure sensor into standby mode
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static void IMX290_SetStandbyOn(void)
{
    IMX290_RegRW(IMX290_STANDBY, 0x01);
    AmbaKAL_TaskSleep(1);
    IMX290_RegRW(IMX290_MASTERSTOP, 0x01);
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX290_SetStandbyOff
 *
 *  @Description:: Configure sensor to leave standby mode
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static void IMX290_SetStandbyOff(void)
{
    IMX290_RegRW(IMX290_STANDBY, 0x00);
    AmbaKAL_TaskSleep(1);
    IMX290_RegRW(IMX290_MASTERSTOP, 0x00);
    AmbaKAL_TaskSleep(260); // Wait 8 frames
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX290_ChangeReadoutMode
 *
 *  @Description:: Switch sensor to requested mode.
 *                 It's for the case that sensor have been initialized.
 *
 *  @Input      ::
 *      SensorMode : Sensor mode
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static int IMX290_ChangeReadoutMode(UINT16 SensorMode)
{
    int i = 0;

    for (i = 0; i < IMX290_NUM_READOUT_MODE_REG; i++) {
        IMX290_RegRW(IMX290RegTable[i].Addr, IMX290RegTable[i].Data[IMX290ModeInfoList[SensorMode].ReadoutMode]);
    }

    /* reset gain/shutter ctrl information */
    IMX290Ctrl.AgcCtrl       = 0xffffffff;
    IMX290Ctrl.ShutterCtrl[0]= 0xffffffff;
    IMX290Ctrl.ShutterCtrl[1]= 0xffffffff;

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX290_Init
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
static int IMX290_Init(AMBA_DSP_CHANNEL_ID_u Chan)
{
    const IMX290_FRAME_TIMING_s *pFrameTime = &IMX290ModeInfoList[0].FrameTiming;

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
 *  @RoutineName:: IMX290_Enable
 *
 *  @Description:: Power on sensor
 *
 *  @Input      ::
 *      Chan: Vin ID and sensor ID
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static int IMX290_Enable(AMBA_DSP_CHANNEL_ID_u Chan)
{
    if (IMX290Ctrl.Status.DevStat.Bits.Sensor0Power)
        return OK;

    IMX290Ctrl.Status.DevStat.Bits.Sensor0Power = 1;
    IMX290Ctrl.Status.DevStat.Bits.Sensor0Standby = 1;

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX290_Disable
 *
 *  @Description:: Power down sensor
 *
 *  @Input      ::
 *      Chan: Vin ID and sensor ID
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static int IMX290_Disable(AMBA_DSP_CHANNEL_ID_u Chan)
{
    if (IMX290Ctrl.Status.DevStat.Bits.Sensor0Power == 0)
        return OK;

    IMX290_SetStandbyOn();

    IMX290Ctrl.Status.DevStat.Bits.Sensor0Standby = 1;  /* Sensor is at Standby mode */

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX290_GetStatus
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
static int IMX290_GetStatus(AMBA_DSP_CHANNEL_ID_u Chan, AMBA_SENSOR_STATUS_INFO_s *pStatus)
{
    if (pStatus == NULL || IMX290Ctrl.Status.DevStat.Bits.Sensor0Power == 0)
        return NG;

    IMX290Ctrl.Status.TimeToAllPelExposed = 0;  /* TODO: for flash light or mechanical shutter control */

    memcpy(pStatus, &IMX290Ctrl.Status, sizeof(AMBA_SENSOR_STATUS_INFO_s));

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX290_GetModeInfo
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
static int IMX290_GetModeInfo(AMBA_DSP_CHANNEL_ID_u Chan, AMBA_SENSOR_MODE_ID_u Mode, AMBA_SENSOR_MODE_INFO_s *pModeInfo)
{
    UINT16 SensorMode = Mode.Bits.Mode;

    if (Mode.Bits.Mode == AMBA_SENSOR_CURRENT_MODE)
        Mode.Bits.Mode = IMX290Ctrl.Status.ModeInfo.Mode.Bits.Mode;

    if (SensorMode >= AMBA_SENSOR_IMX290_NUM_MODE || pModeInfo == NULL)
        return NG;

    IMX290_PrepareModeInfo(SensorMode, pModeInfo);

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX290_GetDeviceInfo
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
static int IMX290_GetDeviceInfo(AMBA_DSP_CHANNEL_ID_u Chan, AMBA_SENSOR_DEVICE_INFO_s *pDeviceInfo)
{
    if (pDeviceInfo == NULL)
        return NG;

    memcpy(pDeviceInfo, &IMX290DeviceInfo, sizeof(AMBA_SENSOR_DEVICE_INFO_s));

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX290_GetHdrInfo
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
static int IMX290_GetHdrInfo(AMBA_DSP_CHANNEL_ID_u Chan, UINT32 *pShutterCtrl, AMBA_SENSOR_HDR_INFO_s *pHdrInfo)
{
    memcpy(pHdrInfo, &IMX290Ctrl.Status.ModeInfo.HdrInfo, sizeof(AMBA_SENSOR_HDR_INFO_s));

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX290_GetCurrentGainFactor
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
static int IMX290_GetCurrentGainFactor(AMBA_DSP_CHANNEL_ID_u Chan, float *pGainFactor)
{
    float AnalogGain = powf(10.0, 0.015 * IMX290Ctrl.AgcCtrl);

    *pGainFactor = (UINT32)(1048576.0 * AnalogGain);

    //AmbaPrint("GainFactor:%f", *pGainFactor);

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX290_GetCurrentShutterSpeed
 *
 *  @Description:: Get Sensor Current Exposure Time
 *
 *  @Input      ::
 *      Chan:            Vin ID and sensor ID
 *
 *  @Output     ::
 *      pExposureTime:   Pointer to current exposure time
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static int IMX290_GetCurrentShutterSpeed(AMBA_DSP_CHANNEL_ID_u Chan, float *pExposureTime)
{
    if (Chan.Bits.HdrID == 0x2)
        *pExposureTime = IMX290Ctrl.Status.ModeInfo.RowTime * IMX290Ctrl.ShutterCtrl[1];
    else
        *pExposureTime = IMX290Ctrl.Status.ModeInfo.RowTime * IMX290Ctrl.ShutterCtrl[0];

    //AmbaPrint("ExposureTime = %f", *pExposureTime);

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX290_ConvertGainFactor
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
static int IMX290_ConvertGainFactor(AMBA_DSP_CHANNEL_ID_u Chan, float DesiredFactor, UINT32 *pActualFactor, UINT32 *pAnalogGainCtrl, UINT32 *pDigitalGainCtrl)
{
    float AnalogGain = 0.0;

    if(DesiredFactor > 3981.071706) {
        *pAnalogGainCtrl = 0xf0;       // Maximum Again register value
        AnalogGain =  3981.071706;     // Maximum Again
    } else if (DesiredFactor >= 1) {
        *pAnalogGainCtrl = (UINT32)(66.666666666666667 * log10(DesiredFactor));
        AnalogGain =  powf(10.0, 0.015 * *pAnalogGainCtrl);
    } else {
       *pAnalogGainCtrl = 0;
       AnalogGain = 1;
    }

    *pActualFactor = (UINT32)(1048576.0 * AnalogGain);

    //AmbaPrint("AnalogGain:%f", AnalogGain);
    //AmbaPrint("DesiredFactor = %f, AgainReg = 0x%08x, ActualFactor = %d",DesiredFactor, *pAnalogGainCtrl, *pActualFactor);

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX290_ConvertShutterSpeed
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
static int IMX290_ConvertShutterSpeed(AMBA_DSP_CHANNEL_ID_u Chan, float ExposureTime, UINT32 *pShutterCtrl)
{
    AMBA_SENSOR_MODE_INFO_s *pModeInfo = &IMX290Ctrl.ModeInfo;

    if (pShutterCtrl == NULL)
        return NG;

    *pShutterCtrl = (UINT32)(ExposureTime / pModeInfo->RowTime);

    /* For HDR mode */
    if (Chan.Bits.HdrID != 0) {

        /* For short exposure frame */
        if ((Chan.Bits.HdrID == 0x2) && (*pShutterCtrl >= pModeInfo->HdrInfo.ChannelInfo[1].MaxExposureLine))
            *pShutterCtrl = pModeInfo->HdrInfo.ChannelInfo[1].MaxExposureLine;
        /* For long exposure frame or default */
        else if (*pShutterCtrl >= pModeInfo->HdrInfo.ChannelInfo[0].MaxExposureLine)
            *pShutterCtrl = pModeInfo->HdrInfo.ChannelInfo[0].MaxExposureLine;

        /* For short exposure frame */
        if ((Chan.Bits.HdrID == 0x2) && (*pShutterCtrl <= pModeInfo->HdrInfo.ChannelInfo[1].MinExposureLine))
            *pShutterCtrl = pModeInfo->HdrInfo.ChannelInfo[1].MinExposureLine;
        /* For long exposure frame or default */
        else if (*pShutterCtrl <= pModeInfo->HdrInfo.ChannelInfo[0].MinExposureLine)
            *pShutterCtrl = pModeInfo->HdrInfo.ChannelInfo[0].MinExposureLine;

    /* For none-HDR mode */
    } else {

        UINT32 ExposureFrames, ExposureTimeMaxMargin;

        ExposureFrames = (*pShutterCtrl / pModeInfo->NumExposureStepPerFrame);
        ExposureFrames = (*pShutterCtrl % pModeInfo->NumExposureStepPerFrame) ? ExposureFrames + 1 : ExposureFrames;
        ExposureTimeMaxMargin = ExposureFrames * pModeInfo->NumExposureStepPerFrame - 1;

        /* Exposure lines needs to be smaller than ExposureTimeMaxMargin */
        if (*pShutterCtrl >= ExposureTimeMaxMargin)
           *pShutterCtrl = ExposureTimeMaxMargin;

        /* Exposure lines needs to be bigger 2 */
        if (*pShutterCtrl <= 2)
            *pShutterCtrl = 2;
    }

    //AmbaPrint("[IMX290_ConvertShutterSpeed] ExposureTime: %f, pShutterCtrl: %d",ExposureTime, *pShutterCtrl);

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX290_SetAnalogGainCtrl
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
static int IMX290_SetAnalogGainCtrl(AMBA_DSP_CHANNEL_ID_u Chan, UINT32 AnalogGainCtrl)
{
    IMX290_SetAnalogGainReg(Chan, AnalogGainCtrl);

    /* Update current AGC control */
    IMX290Ctrl.AgcCtrl = AnalogGainCtrl;

    //AmbaPrint("Set Again: 0x%08X", AnalogGainCtrl);

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX290_SetHdrAnalogGainCtrl
 *
 *  @Description:: Set analog gain control
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
static int IMX290_SetHdrAnalogGainCtrl(AMBA_DSP_CHANNEL_ID_u Chan, UINT32 *pAnalogGainCtrl)
{
    IMX290_SetHdrAnalogGainReg(Chan, pAnalogGainCtrl);

    /* Update current AGC control */
    IMX290Ctrl.AgcCtrl = *pAnalogGainCtrl;

    //AmbaPrint("Set Again[0]: 0x%08X, Again[1]: 0x%08X", *pAnalogGainCtrl, *(pAnalogGainCtrl + 1));

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX290_SetShutterCtrl
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
static int IMX290_SetShutterCtrl(AMBA_DSP_CHANNEL_ID_u Chan, UINT32 ShutterCtrl)
{
    IMX290_SetShutterReg(ShutterCtrl);

    /* Update current shutter control */
    IMX290Ctrl.ShutterCtrl[0] = ShutterCtrl;

    //AmbaPrint("Set Shutter: 0x%08X", ShutterCtrl);

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX290_SetHdrShutterCtrl
 *
 *  @Description:: set shutter control
 *
 *  @Input      ::
 *      Chan:           Vin ID and sensor ID
 *      ShutterCtrl:    Pointer to electronic shutter control
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static int IMX290_SetHdrShutterCtrl(AMBA_DSP_CHANNEL_ID_u Chan, UINT32 *pShutterCtrl)
{
    IMX290_SetHdrShutterReg(Chan, pShutterCtrl);

    if (Chan.Bits.HdrID & 0x1) {
        /* Update current shutter control */
        IMX290Ctrl.ShutterCtrl[0] = *pShutterCtrl;
    }

    if (Chan.Bits.HdrID & 0x2) {
        /* Update current shutter control */
        IMX290Ctrl.ShutterCtrl[1] = *(pShutterCtrl + 1);
    }

    //AmbaPrint("Set Shutter[0]: 0x%08X, Shutter[1]: 0x%08X", *pShutterCtrl, *(pShutterCtrl + 1));

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX290_SetSlowShutterCtrl
 *
 *  @Description:: set slow shutter control
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
static int IMX290_SetSlowShutterCtrl(AMBA_DSP_CHANNEL_ID_u Chan, UINT32 SlowShutterCtrl)
{
    if (IMX290Ctrl.Status.ModeInfo.HdrInfo.HdrType != 0)
        return NG;

    IMX290_SetSlowShutterReg(SlowShutterCtrl);

    //AmbaPrint("Set SlowShutter: 0x%08X", SlowShutterCtrl);

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX290_Config
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
static int IMX290_Config(AMBA_DSP_CHANNEL_ID_u Chan, AMBA_SENSOR_MODE_ID_u Mode, AMBA_SENSOR_ESHUTTER_TYPE_e ElecShutterMode)
{
    AMBA_SENSOR_MODE_INFO_s *pModeInfo = NULL;
    UINT16 SensorMode = Mode.Bits.Mode;
    static UINT8 NotFirstInit = 0;

    if (SensorMode >= AMBA_SENSOR_IMX290_NUM_MODE)
        return NG;

    AmbaPrint("============  [ IMX290 ]  ============");
    AmbaPrint("============ init mode:%d ============", SensorMode);

    /* update status */
    IMX290Ctrl.Status.ElecShutterMode = ElecShutterMode;
    pModeInfo = &IMX290Ctrl.Status.ModeInfo;
    IMX290_PrepareModeInfo(SensorMode, pModeInfo);
    memcpy(&IMX290Ctrl.ModeInfo, pModeInfo, sizeof(AMBA_SENSOR_MODE_INFO_s));

    if (NotFirstInit == 1)
        IMX290_SetStandbyOn();
    else
        NotFirstInit = 1;

    /* Make VIN not to capture bad frames during readout mode transition */
    AmbaVIN_Reset((AMBA_VIN_CHANNEL_e)Chan.Bits.VinID, AMBA_VIN_LVDS, NULL);

    /* set pll */
    AmbaPLL_SetSensorClk(pModeInfo->FrameTime.InputClk);
    AmbaKAL_TaskSleep(1);

    IMX290_HardwareReset();

    for (int i = 0; i < IMX290_NUM_INIT_REG; i++) {
        IMX290_RegRW(IMX290InitRegTable[i].Addr, IMX290InitRegTable[i].Data);
    }

    AmbaKAL_TaskSleep(1);

    /* write registers of mode change to sensor */
    IMX290_ChangeReadoutMode(SensorMode);

    IMX290_SetStandbyOff();

    /* reset vin for A12*/
    AmbaVIN_Reset((AMBA_VIN_CHANNEL_e)Chan.Bits.VinID, AMBA_VIN_LVDS, NULL);

    /* config vin */
    IMX290_ConfigVin(Chan, pModeInfo);

    IMX290Ctrl.Status.DevStat.Bits.Sensor0Standby = 0;

    return OK;
}

/*-----------------------------------------------------------------------------------------------*/
AMBA_SENSOR_OBJ_s AmbaSensor_IMX290Obj = {
    .Init                   = IMX290_Init,
    .Enable                 = IMX290_Enable,
    .Disable                = IMX290_Disable,
    .Config                 = IMX290_Config,
    .GetStatus              = IMX290_GetStatus,
    .GetModeInfo            = IMX290_GetModeInfo,
    .GetDeviceInfo          = IMX290_GetDeviceInfo,
    .GetCurrentGainFactor   = IMX290_GetCurrentGainFactor,
    .GetCurrentShutterSpeed = IMX290_GetCurrentShutterSpeed,
    .GetHdrInfo             = IMX290_GetHdrInfo,
    .ConvertGainFactor      = IMX290_ConvertGainFactor,
    .ConvertShutterSpeed    = IMX290_ConvertShutterSpeed,
    .SetAnalogGainCtrl      = IMX290_SetAnalogGainCtrl,
    .SetDigitalGainCtrl     = NULL,
    .SetShutterCtrl         = IMX290_SetShutterCtrl,
    .SetSlowShutterCtrl     = IMX290_SetSlowShutterCtrl,
    .SetHdrAnalogGainCtrl   = IMX290_SetHdrAnalogGainCtrl,
    .SetHdrDigitalGainCtrl  = NULL,
    .SetHdrShutterCtrl      = IMX290_SetHdrShutterCtrl,

    .RegisterRead           = IMX290_RegisterRead,
    .RegisterWrite          = IMX290_RegisterWrite,
};
