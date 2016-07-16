/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaSensor_AR0330_PARALLEL.c
 *
 *  @Copyright      :: Copyright (C) 2014 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Control APIs of Aptina AR0330 CMOS sensor with PARALLEL interface
 *
\*-------------------------------------------------------------------------------------------------------------------*/

#include <AmbaSSP.h>
#include <math.h>

#include "AmbaDSP.h"
#include "AmbaDSP_VIN.h"
#include "AmbaVIN.h"

#include "AmbaI2C.h"
#include "AmbaGPIO.h"
#include "AmbaPLL.h"
#include "AmbaSensor.h"
#include "AmbaSensor_AR0330_PARALLEL.h"


/*-----------------------------------------------------------------------------------------------*\
 * AR0330_PARALLEL sensor runtime info
\*-----------------------------------------------------------------------------------------------*/
static AR0330_PARALLEL_CTRL_s AR0330_PARALLELCtrl = {0};

/*-----------------------------------------------------------------------------------------------*\
 * Configurations for Amba. VIN side
\*-----------------------------------------------------------------------------------------------*/
static AMBA_DSP_VIN_DVP_CONFIG_s AR0330_PARALLELVinConfig = {
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
        .BayerPattern       = AMBA_DSP_BAYER_GR,
        .NumDataBits        = 12,
        .NumSkipFrame       = 1,
    },

    .DvpType                = AMBA_DSP_VIN_DVP_SINGLE_PEL_SDR,
    .SyncType               = AMBA_DSP_VIN_SYNC_BT601,
    .DataClockEdge          = AMBA_DSP_VIN_SIGNAL_RISING_EDGE,
    .HsyncPolarity          = AMBA_DSP_VIN_SIGNAL_RISING_EDGE,
    .VsyncPolarity          = AMBA_DSP_VIN_SIGNAL_RISING_EDGE,
    .FieldPolarity          = AMBA_DSP_VIN_SIGNAL_RISING_EDGE,
    .SyncPinSelect  = {
        .FieldPinSelect = 0,
        .VsyncPinSelect = AMBA_SENSOR_PARALLEL_VSYNC_PIN_SEL,
        .HsyncPinSelect = AMBA_SENSOR_PARALLEL_HSYNC_PIN_SEL,
    },
    .RxHvSyncCtrl = {0, 0, 0, 0},
    .VinVoutSync = {
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
 *  @RoutineName:: AR0330_PARALLEL_PrepareModeInfo
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
static void AR0330_PARALLEL_PrepareModeInfo(UINT16 Mode, AMBA_SENSOR_MODE_INFO_s *pModeInfo, AR0330_PARALLEL_FRAME_TIMING_s *pSensorPrivate)
{
    const AMBA_SENSOR_INPUT_INFO_s  *pInputInfo= &AR0330_PARALLELInputInfo[AR0330_PARALLELModeInfoList[Mode].InputMode];
    const AMBA_SENSOR_OUTPUT_INFO_s *pOutputInfo= &AR0330_PARALLELOutputInfo[AR0330_PARALLELModeInfoList[Mode].OutputMode];

    if (pSensorPrivate == NULL)
        pSensorPrivate = (AR0330_PARALLEL_FRAME_TIMING_s *) &AR0330_PARALLELModeInfoList[Mode].FrameTiming;
    else
        memcpy(pSensorPrivate, &AR0330_PARALLELModeInfoList[Mode].FrameTiming, sizeof(AR0330_PARALLEL_FRAME_TIMING_s));

    pModeInfo->Mode.Data = Mode;
    pModeInfo->LineLengthPck = AR0330_PARALLELModeInfoList[Mode].FrameTiming.Linelengthpck;
    pModeInfo->FrameLengthLines = AR0330_PARALLELModeInfoList[Mode].FrameTiming.FrameLengthLines;

    pModeInfo->NumExposureStepPerFrame = AR0330_PARALLELModeInfoList[Mode].FrameTiming.FrameLengthLines;
    pModeInfo->PixelRate = (float) pOutputInfo->DataRate * pOutputInfo->NumDataLanes / pOutputInfo->NumDataBits;
    pModeInfo->RowTime = (float) pModeInfo->LineLengthPck / pModeInfo->PixelRate;
    pModeInfo->InternalExposureOffset = 0;
    pModeInfo->FrameTime.InputClk = AR0330_PARALLELModeInfoList[Mode].FrameTiming.InputClk;
    memcpy(&pModeInfo->FrameTime.FrameRate, &(AR0330_PARALLELModeInfoList[Mode].FrameTiming.FrameRate), sizeof(AMBA_DSP_FRAME_RATE_s));
    memcpy(&pModeInfo->InputInfo, pInputInfo, sizeof(AMBA_SENSOR_INPUT_INFO_s));
    memcpy(&pModeInfo->OutputInfo, pOutputInfo, sizeof(AMBA_SENSOR_OUTPUT_INFO_s));
    memcpy(&pModeInfo->MinFrameRate, &(AR0330_PARALLELModeInfoList[Mode].FrameTiming.FrameRate), sizeof(AMBA_DSP_FRAME_RATE_s));

    /* HDR information */
    memset(&pModeInfo->HdrInfo, 0, sizeof(AMBA_SENSOR_HDR_INFO_s)); // Not support HDR

    /* Only support 3 stage slow shutter */
    pModeInfo->MinFrameRate.TimeScale /= 8;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AR0330_PARALLEL_ConfigVin
 *
 *  @Description:: Configure VIN to receieve output frames of the new readout mode
 *
 *  @Input      ::
 *      VinChanNo:  Vin channel number
 *      pModeInfo:  Details of the specified readout mode
 *      pFrameTime: Sensor frame time configuration
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static void AR0330_PARALLEL_ConfigVin(AMBA_VIN_CHANNEL_e VinChanNo, AMBA_SENSOR_MODE_INFO_s *pModeInfo, AR0330_PARALLEL_FRAME_TIMING_s *pFrameTime)
{
    AMBA_SENSOR_OUTPUT_INFO_s  *pImgInfo = &pModeInfo->OutputInfo;
    AMBA_DSP_VIN_DVP_CONFIG_s *pVinCfg = &AR0330_PARALLELVinConfig;

    AmbaDSP_VinPhySetDVP(VinChanNo);

    memcpy(&pVinCfg->Info.FrameRate, &pFrameTime->FrameRate, sizeof(AMBA_DSP_FRAME_RATE_s));
    memcpy(&pVinCfg->Info.DspPhaseShift, &pImgInfo->DspPhaseShift, sizeof(AMBA_DSP_PHASE_SHIFT_CTRL_s));
    pVinCfg->Info.BayerPattern = (AMBA_DSP_SENSOR_PATTERN_e) pImgInfo->CfaPattern;
    pVinCfg->Info.NumDataBits = pImgInfo->NumDataBits;

    pVinCfg->RxHvSyncCtrl.NumActivePixels = pImgInfo->OutputWidth;
    pVinCfg->RxHvSyncCtrl.NumActiveLines  = pImgInfo->OutputHeight;
    pVinCfg->RxHvSyncCtrl.NumTotalPixels  = pModeInfo->LineLengthPck - 1;
    pVinCfg->RxHvSyncCtrl.NumTotalLines   = pModeInfo->FrameLengthLines;

    AmbaDSP_VinConfigDVP(VinChanNo, pVinCfg);
}

static int AR0330_PARALLEL_RegWrite(UINT16 Addr, UINT16 Data)
{
    UINT8 TxData[4];

    TxData[0] = (UINT8) ((Addr & 0xff00) >> 8);
    TxData[1] = (UINT8) (Addr & 0x00ff);
    TxData[2] = (UINT8) ((Data & 0xff00) >> 8);
    TxData[3] = (UINT8) (Data & 0x00ff);

    if (AmbaI2C_Write(AMBA_I2C_CHANNEL0,
                      AMBA_I2C_SPEED_STANDARD,
                      0x20, 4, TxData,
                      AMBA_KAL_WAIT_FOREVER) == NG)
        AmbaPrint("I2C does not work!!!!!");

    //AmbaPrint("[AR0330_PARALLEL] Addr=0x%04X, Data=0x%04X (%u)", Addr, Data, Data);

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AR0330_PARALLEL_RegisterWrite
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
static int AR0330_PARALLEL_RegisterWrite(AMBA_DSP_CHANNEL_ID_u Chan, UINT16 Addr, UINT16 Data)
{
    AR0330_PARALLEL_RegWrite(Addr, Data);

    return OK;
}

static int AR0330_PARALLEL_RegRead(UINT16 Addr, UINT16 *pRxData)
{
    int RetStatus;
    UINT16 TxData[4];
    UINT8  RxData[2];

    TxData[0] = AMBA_I2C_RESTART_FLAG | (0x20);
    TxData[1] = (Addr >> 8);    /* Register Address [15:8] */
    TxData[2] = (Addr & 0xff);  /* Register Address [7:0]  */
    TxData[3] = AMBA_I2C_RESTART_FLAG | (0x21);

    RetStatus = AmbaI2C_ReadAfterWrite(AMBA_I2C_CHANNEL0,
                                       AMBA_I2C_SPEED_STANDARD,
                                       4, (UINT16 *) TxData,
                                       2, RxData,
                                       AMBA_KAL_WAIT_FOREVER);

    *pRxData = (RxData[0] << 8) | RxData[1];

    return RetStatus;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AR0330_PARALLEL_RegisterRead
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
static int AR0330_PARALLEL_RegisterRead(AMBA_DSP_CHANNEL_ID_u Chan, UINT16 Addr, UINT16 *Data)
{
    AR0330_PARALLEL_RegRead(Addr, Data);

    return OK;
}

#if 0
static int AR0330_PARALLEL_RegByteRead(UINT16 Addr, UINT8 *pRxData)
{
    int RetStatus;
    UINT16 TxData[4];

    TxData[0] = AMBA_I2C_RESTART_FLAG | (0x20);
    TxData[1] = (Addr >> 8);    /* Register Address [15:8] */
    TxData[2] = (Addr & 0xff);  /* Register Address [7:0]  */
    TxData[3] = AMBA_I2C_RESTART_FLAG | (0x21);

    RetStatus = AmbaI2C_ReadAfterWrite(AMBA_I2C_CHANNEL0,
                                       AMBA_I2C_SPEED_STANDARD,
                                       4, (UINT16 *) TxData,
                                       1, pRxData,
                                       AMBA_KAL_WAIT_FOREVER);
    return RetStatus;
}
#endif
static void AR0330_PARALLEL_SetRegR0x301A(UINT32 RestartFrame, UINT32 Streaming)
{
    UINT16 R0x301A;

    R0x301A =
            1 << 12                  | // 1: disable serial interface (HiSPi)
            0 << 11                  | // force_pll_on
            0 << 10                  | // DONT restart if bad frame is detected
            0 << 9                   | // The sensor will produce bad frames as some register changed
            0 << 8                   | // input buffer related to GPI0/1/2/3 inputs are powered down & disabled
            1 << 7                   | // Parallel data interface is enabled (dep on bit[6])
            1 << 6                   | // Parallel interface is driven
            1 << 4                   | // reset_reg_unused
            1 << 3                   | // Forbids to change value of SMIA registers
            (Streaming > 0) << 2     | // Put the sensor in streaming mode
            (RestartFrame > 0) << 1  | // Causes sensor to truncate frame at the end of current row and start integrating next frame
            0 << 0;                    // Set the bit initiates a reset sequence

    // Typically, in normal streamming mode (restart_frame=0, streaming=1), the value is 0x10DC
    AR0330_PARALLEL_RegWrite(0x301A, R0x301A);
}

static int AR0330_PARALLEL_ConfirmStreamingMode(void)
{
    int PollingCnt = 200, RetStatus;
    UINT16 Data = 0xFFFF;

    /* Make sure that sensor is streaming */
    while (1) {
        /* Read frame count */
        AR0330_PARALLEL_RegRead(0x303A, &Data);

        AmbaPrint("confirm_streaming: frame_count = 0x%04X", Data);

        if (Data != 0xFFFF) {
            RetStatus = OK;
            break;
        }

        if (PollingCnt < 0) {
            RetStatus = NG;
            break;
        }
        AmbaKAL_TaskSleep(1);
        PollingCnt--;
    }
    return RetStatus;
}

static int AR0330_PARALLEL_ConfirmStandbyMode(void)
{
    int PollingCnt = 200, RetStatus;
    UINT16 Data = 0xFFFF;

    while (1) {
        /* Read frame status */
        AR0330_PARALLEL_RegRead(0x303C, &Data);

        AmbaPrint("confirm_standby: R0x303C = 0x%04X", Data);

        if ((Data & 0x0002) && (Data != 0xFFFF)) {
            RetStatus = OK;
            break;
        }

        if (PollingCnt < 0) {
            RetStatus = NG;
            break;
        }
        AmbaKAL_TaskSleep(1);
        PollingCnt--;
    }
    return RetStatus;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AR0330_PARALLEL_HardwareReset
 *
 *  @Description:: Reset (HW) AR0330_PARALLEL Image Sensor Device
 *
 *  @Input      ::
 *      VinChanNo:          Vin channel number
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static inline void AR0330_PARALLEL_HardwareReset(AMBA_VIN_CHANNEL_e VinChanNo)
{
    AmbaUserGPIO_SensorResetCtrl(0);
    AmbaKAL_TaskSleep(10);
    AmbaUserGPIO_SensorResetCtrl(1);
    AmbaKAL_TaskSleep(10);

    /* patch for cold-temp OTPM load issue */
    AR0330_PARALLEL_RegWrite(0x3052, 0xA114);
    AR0330_PARALLEL_RegWrite(0x304A, 0x0070);

    /* Wait for internal initialization */
    AmbaKAL_TaskSleep(10);

    AmbaPrint("Sensor HW Reset");
    if (AR0330_PARALLEL_ConfirmStandbyMode() == -1) {
        AmbaPrint("cannot enter standby mode after HW reset!");
    }
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AR0330_PARALLEL_SoftwareReset
 *
 *  @Description:: Reset (SW) AR0330_PARALLEL Image Sensor Device
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static inline void AR0330_PARALLEL_SoftwareReset(void)
{
    /* R0x301A[0]: software reset */
    AR0330_PARALLEL_RegWrite(0x301A, 0x10DD);

    /* Wait for internal initialization */
    AmbaKAL_TaskSleep(10);

    /* patch for cold-temp OTPM load issue */
    AR0330_PARALLEL_RegWrite(0x3052, 0xA114);
    AR0330_PARALLEL_RegWrite(0x304A, 0x0070);

    /* Wait for internal initialization */
    AmbaKAL_TaskSleep(10);

    AmbaPrint("Sensor SW Reset");
    if (AR0330_PARALLEL_ConfirmStandbyMode() == -1) {
        AmbaPrint("cannot enter standby mode after SW reset!");
    }
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AR0330_PARALLEL_SetStandbyOn
 *
 *  @Description:: Configure sensor into standby mode
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static void AR0330_PARALLEL_SetStandbyOn(void)
{
    AR0330_PARALLEL_SetRegR0x301A(0/*restart_frame*/, 0/*streaming*/);

    if (AR0330_PARALLEL_ConfirmStandbyMode() == -1)
        AmbaPrint("cannot enter standby mode!");
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AR0330_PARALLEL_SetStandbyOff
 *
 *  @Description:: Configure sensor to leave standby mode.
 *
 *  @Input      ::
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static void AR0330_PARALLEL_SetStandbyOff(void)
{
    AR0330_PARALLEL_SetRegR0x301A(0/*restart_frame*/, 1/*streaming*/);

    if (AR0330_PARALLEL_ConfirmStreamingMode() == -1)
        AmbaPrint("cannot enter streaming mode");
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AR0330_PARALLEL_ConfigIF
 *
 *  @Description:: Configure sensor output interface.
 *
 *  @Input      ::
 *      Mode:   Sensor readout mode number
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static int AR0330_PARALLEL_ConfigIF(UINT16 Mode)
{
    UINT16 Addr, Data;

    /* Configure output interface */
    Addr = AR0330_PARALLELPllRegTable[0].Addr;
    Data = AR0330_PARALLELPllRegTable[0].Data[AR0330_PARALLELModeInfoList[Mode].PllConfig];
    AR0330_PARALLEL_RegWrite(Addr, Data);
    AR0330_PARALLEL_RegWrite(0x301A, 0x10D8); /* Drive Pins, Parallel Enable, and SMIA Serializer Disable */

    AR0330_PARALLEL_RegWrite(0x3064, 0x1802); /* Disable Embedded Data */
    AR0330_PARALLEL_RegWrite(0x3078, 0x0001); /* Marker to say that 'Defaults' have been run */

    AR0330_PARALLEL_RegWrite(0x31E0, 0x0003); /* defect correction setting */
    AR0330_PARALLEL_RegWrite(0x306E, 0xFC10); /* slew rate */

    /* Toggle Flash on each frame */
    AR0330_PARALLEL_RegWrite(0x3046, 0x4038); /* Enable Flash Pin */
    AR0330_PARALLEL_RegWrite(0x3048, 0x8480); /* Flash Pulse Length */

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AR0330_PARALLEL_ConfigPLL
 *
 *  @Description:: Configure sensor PLL.
 *
 *  @Input      ::
 *      Mode:   Sensor readout mode number
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static int AR0330_PARALLEL_ConfigPLL(UINT16 Mode)
{
    int i;
    UINT16 Addr, Data;

    /* Configure sensor PLL */
    for (i = 1; i < AR0330_PARALLEL_NUM_PLL_REG; i++) {
        Addr = AR0330_PARALLELPllRegTable[i].Addr;
        Data = AR0330_PARALLELPllRegTable[i].Data[AR0330_PARALLELModeInfoList[Mode].PllConfig];
        AR0330_PARALLEL_RegWrite(Addr, Data);
    }

    AmbaKAL_TaskSleep(1);

    return OK;
}

static int AR0330_PARALLEL_ConfigReadoutRegs(UINT16 Mode, UINT32 StartIdx, UINT32 Num)
{
    int i;
    UINT16 Addr, Data;

    for (i = StartIdx; i < StartIdx + Num; i++) {
        Addr = AR0330_PARALLELRegTable[i].Addr;
        Data = AR0330_PARALLELRegTable[i].Data[AR0330_PARALLELModeInfoList[Mode].ReadoutMode];

        if (Data == NA)
            continue;

        AR0330_PARALLEL_RegWrite(Addr, Data);
    }

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AR0330_PARALLEL_ChangeReadoutMode
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
static int AR0330_PARALLEL_ChangeReadoutMode(UINT16 Mode)
{
    AR0330_PARALLEL_ConfigReadoutRegs(Mode,
                             AR0330_PARALLEL_RESOLUTION_REG_OFFSET,
                             AR0330_PARALLEL_RESOLUTION_REG_NUM);

    AR0330_PARALLEL_ConfigReadoutRegs(Mode,
                             AR0330_PARALLEL_ADC_REG_OFFSET,
                             AR0330_PARALLEL_ADC_REG_NUM);

    /* Sequencer patch for fixing the FPN and black shading issues */
    AR0330_PARALLEL_ConfigReadoutRegs(Mode,
                             AR0330_PARALLEL_SEQ_REG_OFFSET,
                             AR0330_PARALLEL_SEQ_REG_NUM);

    AR0330_PARALLEL_ConfigReadoutRegs(Mode,
                             AR0330_PARALLEL_RESERVED_REG_OFFSET,
                             AR0330_PARALLEL_RESERVED_REG_NUM);

	AR0330_PARALLELCtrl.GainFactor = 0xffffffff;
	AR0330_PARALLELCtrl.ShutterCtrl= 0xffffffff;
#if 0
    for(i = 0; i < AR0330_PARALLEL_NUM_READOUT_MODE_REG; i ++) {
        UINT8 Rval = 0;
        AR0330_PARALLEL_RegRead(AR0330_PARALLELRegTable[i].Addr, &Rval);
        AmbaPrint("Reg Read: Addr: 0x%x, data: 0x%x", AR0330_PARALLELRegTable[i].Addr, Rval);
    }
#endif
    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AR0330_PARALLEL_Init
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
static int AR0330_PARALLEL_Init(AMBA_DSP_CHANNEL_ID_u Chan)
{
    const AR0330_PARALLEL_FRAME_TIMING_s *pFrameTime = &AR0330_PARALLELModeInfoList[0].FrameTiming;
    AmbaPLL_SetSensorClk(pFrameTime->InputClk);             /* The default sensor input clock frequency */
    AmbaPLL_SetSensorClkDir(AMBA_PLL_SENSOR_CLK_OUTPUT);    /* Config clock output to sensor (or sensor SPI won't work) */

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AR0330_PARALLEL_Enable
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
static int AR0330_PARALLEL_Enable(AMBA_DSP_CHANNEL_ID_u Chan)
{
    if (AR0330_PARALLELCtrl.Status.DevStat.Bits.Sensor0Power == 1)
        return OK;

    AR0330_PARALLEL_HardwareReset((AMBA_VIN_CHANNEL_e) Chan.Bits.VinID);
    AR0330_PARALLEL_SoftwareReset();

    AR0330_PARALLELCtrl.Status.DevStat.Bits.Sensor0Power   = 1;
    AR0330_PARALLELCtrl.Status.DevStat.Bits.Sensor0Standby = 1;

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AR0330_PARALLEL_Disable
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
static int AR0330_PARALLEL_Disable(AMBA_DSP_CHANNEL_ID_u Chan)
{
    if (AR0330_PARALLELCtrl.Status.DevStat.Bits.Sensor0Power == 0)
        return OK;

    AR0330_PARALLEL_SetStandbyOn();

    AR0330_PARALLELCtrl.Status.DevStat.Bits.Sensor0Power = 0;

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AR0330_PARALLEL_ChangeFrameRate
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
static int AR0330_PARALLEL_ChangeFrameRate(AMBA_DSP_CHANNEL_ID_u Chan, AMBA_DSP_FRAME_RATE_s *pFrameRate)
{
    return NG;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AR0330_PARALLEL_SetGainFactor
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
static int AR0330_PARALLEL_SetGainFactor(AMBA_DSP_CHANNEL_ID_u Chan, float GainFactor)
{
    return NG;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AR0330_PARALLEL_SetEshutterSpeed
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
static int AR0330_PARALLEL_SetEshutterSpeed(AMBA_DSP_CHANNEL_ID_u Chan, float ShutterSpeed)
{
    return NG;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AR0330_PARALLEL_SetEshutterSpeed
 *
 *  @Description:: Get AR0330_PARALLEL OTPM revision
 *
 *  @Input      :: none
 *
 *  @Output     ::
 *      OTPMRev:   Pointer to AR0330_PARALLEL OTPM revision
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static int AR0330_PARALLEL_GetOTPMRev(UINT32 *OTPMRev)
{
    UINT16 R0x30F0, R0x3072;

    if (OTPMRev == NULL)
        return -1;

    *OTPMRev = 0;

    /* Read sensor(OTPM) revision number */
    AR0330_PARALLEL_RegRead(0x30F0, &R0x30F0);
    AR0330_PARALLEL_RegRead(0x3072, &R0x3072);

    if (R0x30F0 == 0x1200 && R0x3072 == 0x0000)
        *OTPMRev = 1; /* sensor rev 1, OTPM rev 1 */
    else if (R0x30F0 == 0x1208 && R0x3072 == 0x0000)
        *OTPMRev = 2; /* sensor rev 2.0, OTPM rev 2 */
    else if (R0x30F0 == 0x1208 && R0x3072 == 0x0006)
        *OTPMRev = 3; /* sensor rev 2.1, OTPM rev 3 */
    else if (R0x30F0 == 0x1208 && R0x3072 == 0x0007)
        *OTPMRev = 4; /* sensor rev 2.1, OTPM rev 4 */
    else if (R0x30F0 == 0x1208 && R0x3072 == 0x0008)
        *OTPMRev = 5; /* sensor rev 2.1, OTPM rev 5 */
    else {
        AmbaPrint("R0x30F0: 0x%04X, R0x3072: 0x%04X",
                  R0x30F0, R0x3072);
        return NG;
    }

    return OK;
}

static int AR0330_PARALLEL_InitSeq(UINT32 SensorMode)
{
    int i;
    UINT32 Rev;
    UINT16 Data;

    if (AR0330_PARALLEL_GetOTPMRev(&Rev) != OK)
        AmbaPrint("!!!!!! unknown OTPM revision !!!!!!");

    /* recommend default register and sequencer*/
    switch (Rev) {
    case 1:
        AmbaPrint("sensor rev 1, OTPM V1 initialization");
        AR0330_PARALLEL_RegWrite(0x30BA, 0x2C);
        AR0330_PARALLEL_RegWrite(0x30FE, 0x0080);
        AR0330_PARALLEL_RegWrite(0x31E0, 0x0003);
        AR0330_PARALLEL_RegRead(0x3ECE, &Data);
        Data = (Data | 0x00FF);
        AR0330_PARALLEL_RegWrite(0x3ECE, Data);
        AR0330_PARALLEL_RegWrite(0x3ED0, 0xE4F6);
        AR0330_PARALLEL_RegWrite(0x3ED2, 0x0146);
        AR0330_PARALLEL_RegWrite(0x3ED4, 0x8F6C);
        AR0330_PARALLEL_RegWrite(0x3ED6, 0x66CC);
        AR0330_PARALLEL_RegWrite(0x3ED8, 0x8C42);
        AR0330_PARALLEL_RegWrite(0x3EDA, 0x8822);
        AR0330_PARALLEL_RegWrite(0x3EDC, 0x2222);
        AR0330_PARALLEL_RegWrite(0x3EDE, 0x22C0);
        AR0330_PARALLEL_RegWrite(0x3EE0, 0x1500);
        AR0330_PARALLEL_RegWrite(0x3EE6, 0x0080);
        AR0330_PARALLEL_RegWrite(0x3EE8, 0x2027);
        AR0330_PARALLEL_RegWrite(0x3EEA, 0x001D);
        AR0330_PARALLEL_RegWrite(0x3F06, 0x046A);

        /* For OTPM V1 sensors only */
        for (i = 0; i < AR0330_PARALLEL_NUM_SEQUENCER_A_REG; i++) {
            AR0330_PARALLEL_RegWrite(AR0330_PARALLELSeqARegTable[i].Addr,
                            AR0330_PARALLELSeqARegTable[i].Data[0]);
        }

        break;
    case 2:
        AmbaPrint("sensor rev 2.0, OTPM V2 initialization");
        AR0330_PARALLEL_RegWrite(0x30BA, 0x2C);
        AR0330_PARALLEL_RegWrite(0x30FE, 0x0080);
        AR0330_PARALLEL_RegWrite(0x31E0, 0x0003);
        AR0330_PARALLEL_RegRead(0x3ECE, &Data);
        Data = (Data | 0x00FF);
        AR0330_PARALLEL_RegWrite(0x3ECE, Data);
        AR0330_PARALLEL_RegWrite(0x3ED0, 0xE4F6);
        AR0330_PARALLEL_RegWrite(0x3ED2, 0x0146);
        AR0330_PARALLEL_RegWrite(0x3ED4, 0x8F6C);
        AR0330_PARALLEL_RegWrite(0x3ED6, 0x66CC);
        AR0330_PARALLEL_RegWrite(0x3ED8, 0x8C42);
        AR0330_PARALLEL_RegWrite(0x3EDA, 0x889B);
        AR0330_PARALLEL_RegWrite(0x3EDC, 0x8863);
        AR0330_PARALLEL_RegWrite(0x3EDE, 0xAA04);
        AR0330_PARALLEL_RegWrite(0x3EE0, 0x15F0);
        AR0330_PARALLEL_RegWrite(0x3EE6, 0x008C);
        AR0330_PARALLEL_RegWrite(0x3EE8, 0x2024);
        AR0330_PARALLEL_RegWrite(0x3EEA, 0xFF1F);
        AR0330_PARALLEL_RegWrite(0x3F06, 0x046A);

        /* for OTPM V2 sensors */
        for (i = 0; i < AR0330_PARALLEL_NUM_SEQUENCER_B_REG; i++) {
            AR0330_PARALLEL_RegWrite(AR0330_PARALLELSeqBRegTable[i].Addr,
                            AR0330_PARALLELSeqBRegTable[i].Data[0]);
        }
        break;
    case 3:
        AmbaPrint("sensor rev 2.1, OTPM V3 initialization");
        AR0330_PARALLEL_RegWrite(0x31E0, 0x0003);

        AR0330_PARALLEL_RegWrite(0x3ED2, 0x0146);
        AR0330_PARALLEL_RegWrite(0x3ED4, 0x8F6C);
        AR0330_PARALLEL_RegWrite(0x3ED6, 0x66CC);
        AR0330_PARALLEL_RegWrite(0x3ED8, 0x8C42);

        /* sequencer patch 1 */
        AR0330_PARALLEL_RegWrite(0x3088, 0x800C);
        AR0330_PARALLEL_RegWrite(0x3086, 0x2045);
        break;
    case 4:
        AmbaPrint("sensor rev 2.1, OTPM V4 initialization");
        AR0330_PARALLEL_RegWrite(0x31E0, 0x0003);

        AR0330_PARALLEL_RegWrite(0x3ED2, 0x0146);

        AR0330_PARALLEL_RegWrite(0x3ED6, 0x66CC);
        AR0330_PARALLEL_RegWrite(0x3ED8, 0x8C42);
        break;
    case 5:
        AmbaPrint("sensor rev 2.1, OTPM V5 initialization");
        AR0330_PARALLEL_RegWrite(0x3ED2, 0x0146);
        break;
    default:
        break;
    }

    /**
     * Enable every-frame BLC (R0x3180[14]) by default to avoid black
     * level shift problem, which could happens when sensor operating
     * temperature is high.
     */
    AR0330_PARALLEL_RegWrite(0x3180, 0xc089);

    return OK;
}

#if 0 /* for debugging */
static void AR0330_PARALLEL_SequencerDump(void)
{
    int i;
    UINT8 DataH = 0xFF;
    UINT8 DataL = 0xFF;

    AR0330_PARALLEL_RegWrite(0x3088, 0xC000);
    AmbaPrint("----- Sequencer -----");
    for (i = 1; i <= 157 /* max entry num of sequencers */; i++) {
        AR0330_PARALLEL_RegByteRead(0x3086, &DataH);
        AR0330_PARALLEL_RegByteRead(0x3086, &DataL);
        AmbaPrint("%4d 0x%04X", i, (DataH << 8) | DataL);
    }
}

int AR0330_PARALLEL_RegDump(void)
{
    int i, NumRegs;
    UINT16 Data;
    UINT16 RegToDump[] = {
        0x0000, 0x0002, 0x0003, 0x0004, 0x0005, 0x0006, 0x0008, 0x0040,
        0x0041, 0x0042, 0x0044, 0x0046, 0x0048, 0x004A, 0x004C, 0x004E,
        0x0050, 0x0052, 0x0054, 0x0056, 0x0058, 0x005A, 0x005C, 0x005E,
        0x0080, 0x0084, 0x0086, 0x0088, 0x008A, 0x008C, 0x008E, 0x0090,
        0x0092, 0x00C0, 0x00C1, 0x00C2, 0x00C4, 0x00C6, 0x00C8, 0x00CA,
        0x00CC, 0x00CE, 0x0100, 0x0101, 0x0103, 0x0104, 0x0105, 0x0110,
        0x0111, 0x0112, 0x0120, 0x0200, 0x0202, 0x0204, 0x0206, 0x0208,
        0x020A, 0x020C, 0x020E, 0x0210, 0x0212, 0x0214, 0x0300, 0x0302,
        0x0304, 0x0306, 0x0308, 0x030A, 0x0340, 0x0342, 0x0344, 0x0346,
        0x0348, 0x034A, 0x034C, 0x034E, 0x0380, 0x0382, 0x0384, 0x0386,
        0x0400, 0x0402, 0x0404, 0x0406, 0x0500, 0x0600, 0x0602, 0x0604,
        0x0606, 0x0608, 0x060A, 0x060C, 0x060E, 0x0610, 0x1000, 0x1004,
        0x1006, 0x1008, 0x100A, 0x1080, 0x1084, 0x1086, 0x1088, 0x1100,
        0x1104, 0x1108, 0x110A, 0x110C, 0x1110, 0x1114, 0x1116, 0x1118,
        0x111C, 0x1120, 0x1122, 0x1124, 0x1128, 0x112C, 0x1130, 0x1134,
        0x1136, 0x1140, 0x1142, 0x1144, 0x1146, 0x1148, 0x114A, 0x1160,
        0x1162, 0x1164, 0x1168, 0x116C, 0x116E, 0x1170, 0x1174, 0x1180,
        0x1182, 0x1184, 0x1186, 0x11C0, 0x11C2, 0x11C4, 0x11C6, 0x1200,
        0x1204, 0x1206, 0x1208, 0x120A, 0x1300, 0x1400, 0x1402, 0x1404,
        0x1406, 0x1408, 0x140A, 0x140C, 0x140E, 0x1410, 0x3000, 0x3002,
        0x3004, 0x3006, 0x3008, 0x300A, 0x300C, 0x3010, 0x3012, 0x3014,
        0x3016, 0x3018, 0x301A, 0x301C, 0x301D, 0x301E, 0x3021, 0x3022,
        0x3023, 0x3024, 0x3026, 0x3028, 0x302A, 0x302C, 0x302E, 0x3030,
        0x3032, 0x3034, 0x3036, 0x3038, 0x303A, 0x303B, 0x303C, 0x3040,
        0x3044, 0x3046, 0x3048, 0x304A, 0x304C, 0x304E, 0x3050, 0x3052,
        0x3054, 0x3056, 0x3058, 0x305A, 0x305C, 0x305E, 0x3060, 0x3062,
        0x3064, 0x3066, 0x3068, 0x306A, 0x306C, 0x306E, 0x3070, 0x3072,
        0x3074, 0x3076, 0x3078, 0x307A, 0x3080, 0x30A0, 0x30A2, 0x30A4,
        0x30A6, 0x30A8, 0x30AA, 0x30AC, 0x30AE, 0x30B0, 0x30B2, 0x30B4,
        0x30BC, 0x30C0, 0x30C2, 0x30C4, 0x30C6, 0x30C8, 0x30CA, 0x30CC,
        0x30CE, 0x30D0, 0x30D2, 0x30D4, 0x30D6, 0x30D8, 0x30DA, 0x30DC,
        0x3130, 0x3132, 0x3134, 0x3136, 0x3138, 0x313A, 0x313C, 0x313E,
        0x315C, 0x315E, 0x3160, 0x3162, 0x3164, 0x3166, 0x3168, 0x316A,
        0x316C, 0x316E, 0x3170, 0x3172, 0x3174, 0x3176, 0x3178, 0x318A,
        0x318C, 0x318E, 0x3190, 0x31A0, 0x31A2, 0x31A4, 0x31A6, 0x31A8,
        0x31AA, 0x31AC, 0x31AE, 0x31B0, 0x31B2, 0x31B4, 0x31B6, 0x31B8,
        0x31BA, 0x31BC, 0x31BE, 0x31C0, 0x31C2, 0x31C4, 0x31C6, 0x31C8,
        0x31CA, 0x31CC, 0x31CE, 0x31DA, 0x31DC, 0x31DE, 0x31E0, 0x31E2,
        0x31E4, 0x31E8, 0x31EA, 0x31EC, 0x31EE, 0x31F2, 0x31F4, 0x31F6,
        0x31F8, 0x31FA, 0x31FC, 0x31FE, 0x3600, 0x3602, 0x3604, 0x3606,
        0x3608, 0x360A, 0x360C, 0x360E, 0x3610, 0x3612, 0x3614, 0x3616,
        0x3618, 0x361A, 0x361C, 0x361E, 0x3620, 0x3622, 0x3624, 0x3626,
        0x3640, 0x3642, 0x3644, 0x3646, 0x3648, 0x364A, 0x364C, 0x364E,
        0x3650, 0x3652, 0x3654, 0x3656, 0x3658, 0x365A, 0x365C, 0x365E,
        0x3660, 0x3662, 0x3664, 0x3666, 0x3680, 0x3682, 0x3684, 0x3686,
        0x3688, 0x368A, 0x368C, 0x368E, 0x3690, 0x3692, 0x3694, 0x3696,
        0x3698, 0x369A, 0x369C, 0x369E, 0x36A0, 0x36A2, 0x36A4, 0x36A6,
        0x36C0, 0x36C2, 0x36C4, 0x36C6, 0x36C8, 0x36CA, 0x36CC, 0x36CE,
        0x36D0, 0x36D2, 0x36D4, 0x36D6, 0x36D8, 0x36DA, 0x36DC, 0x36DE,
        0x36E0, 0x36E2, 0x36E4, 0x36E6, 0x3700, 0x3702, 0x3704, 0x3706,
        0x3708, 0x370A, 0x370C, 0x370E, 0x3710, 0x3712, 0x3714, 0x3716,
        0x3718, 0x371A, 0x371C, 0x371E, 0x3720, 0x3722, 0x3724, 0x3726,
        0x3780, 0x3782, 0x3784, 0x3800, 0x3802, 0x3804, 0x3806, 0x3808,
        0x380A, 0x380C, 0x380E, 0x3810, 0x3812, 0x3814, 0x3816, 0x3818,
        0x381A, 0x381C, 0x381E, 0x3820, 0x3822, 0x3824, 0x3826, 0x3828,
        0x382A, 0x382C, 0x382E, 0x3830, 0x3832, 0x3834, 0x3836, 0x3838,
        0x383A, 0x383C, 0x383E, 0x3840, 0x3842, 0x3844, 0x3846, 0x3848,
        0x384A, 0x384C, 0x384E, 0x3850, 0x3852, 0x3854, 0x3856, 0x3858,
        0x385A, 0x385C, 0x385E, 0x3860, 0x3862, 0x3864, 0x3866, 0x3868,
        0x386A, 0x386C, 0x386E, 0x3870, 0x3872, 0x3874, 0x3876, 0x3878,
        0x387A, 0x387C, 0x387E, 0x3880, 0x3882, 0x3884, 0x3886, 0x3888,
        0x388A, 0x388C, 0x388E, 0x3890, 0x3892, 0x3894, 0x3896, 0x3898,
        0x389A, 0x389C, 0x389E, 0x38A0, 0x38A2, 0x38A4, 0x38A6, 0x38A8,
        0x38AA, 0x38AC, 0x38AE, 0x38B0, 0x38B2, 0x38B4, 0x38B6, 0x38B8,
        0x38BA, 0x38BC, 0x38BE, 0x38C0, 0x38C2, 0x38C4, 0x38C6, 0x38C8,
        0x38CA, 0x38CC, 0x38CE, 0x38D0, 0x38D2, 0x38D4, 0x38D6, 0x38D8,
        0x38DA, 0x38DC, 0x38DE, 0x38E0, 0x38E2, 0x38E4, 0x38E6, 0x38E8,
        0x38EA, 0x38EC, 0x38EE, 0x38F0, 0x38F2, 0x38F4, 0x38F6, 0x38F8,
        0x38FA, 0x38FC, 0x38FE, 0x3900, 0x3902, 0x3904, 0x3906, 0x3908,
        0x390A, 0x390C, 0x390E, 0x3910, 0x3912, 0x3914, 0x3916, 0x3918,
        0x391A, 0x391C, 0x391E, 0x3920, 0x3922, 0x3924, 0x3926, 0x3928,
        0x392A, 0x392C, 0x392E, 0x3930, 0x3932, 0x3934, 0x3936, 0x3938,
        0x393A, 0x393C, 0x393E, 0x3940, 0x3942, 0x3944, 0x3946, 0x3948,
        0x394A, 0x394C, 0x394E, 0x3950, 0x3952, 0x3954, 0x3956, 0x3958,
        0x395A, 0x395C, 0x395E, 0x3960, 0x3962, 0x3964, 0x3966, 0x3968,
        0x396A, 0x396C, 0x396E, 0x3970, 0x3972, 0x3974, 0x3976, 0x3978,
        0x397A, 0x397C, 0x397E, 0x3980, 0x3982, 0x3984, 0x3986, 0x3988,
        0x398A, 0x398C, 0x398E, 0x3990, 0x3992, 0x3994, 0x3996, 0x3998,
        0x399A, 0x399C, 0x399E, 0x39A0, 0x39A2, 0x39A4, 0x39A6, 0x39A8,
        0x39AA, 0x39AC, 0x39AE, 0x39B0, 0x39B2, 0x39B4, 0x39B6, 0x39B8,
        0x39BA, 0x39BC, 0x39BE, 0x39C0, 0x39C2, 0x39C4, 0x39C6, 0x39C8,
        0x39CA, 0x39CC, 0x39CE, 0x39D0, 0x39D2, 0x39D4, 0x39D6, 0x39D8,
        0x39DA, 0x39DC, 0x39DE, 0x39E0, 0x39E2, 0x39E4, 0x39E6, 0x39E8,
        0x39EA, 0x39EC, 0x39EE, 0x39F0, 0x39F2, 0x39F4, 0x39F6, 0x39F8,
        0x39FA, 0x39FC, 0x39FE, 0x3E00, 0x3E02, 0x3E04, 0x3E06, 0x3E08,
        0x3E0A, 0x3E0C, 0x3E0E, 0x3E10, 0x3E12, 0x3E14, 0x3E16, 0x3E18,
        0x3E1A, 0x3E1C, 0x3E1E, 0x3E20, 0x3E22, 0x3E24, 0x3E26, 0x3E28,
        0x3E2A, 0x3E2C, 0x3E2E, 0x3E30, 0x3E32, 0x3E34, 0x3E36, 0x3E38,
        0x3E3A, 0x3E3C, 0x3E3E, 0x3E40, 0x3E42, 0x3E44, 0x3E46, 0x3E48,
        0x3E4A, 0x3E4C, 0x3E4E, 0x3E50, 0x3E52, 0x3E54, 0x3E56, 0x3E58,
        0x3E90, 0x3E92, 0x3E94, 0x3E96, 0x3E98, 0x3E9A, 0x3E9C, 0x3E9E,
        0x3EA0, 0x3EA2, 0x3EB0, 0x3EB2, 0x3EB4, 0x3EB6, 0x3EB8, 0x3ECC,
        0x3ECE, 0x3ED0, 0x3ED2, 0x3ED4, 0x3ED6, 0x3ED8, 0x3EDA, 0x3EDC,
        0x3EDE, 0x3EE0, 0x3EE2, 0x3EE4, 0x3EE6, 0x3EE8, 0x3EEA, 0x3EEC,
        0x3EEE, 0x3EF0, 0x3F06, 0x30FE, 0x30BA, 0x3042, 0x3088, 0x3086
    };

    /* Stop streaming with R0x301A[5]=1 */
    AR0330_PARALLEL_RegRead(0x301A, &Data);
    Data |= (1 << 5);
    Data &= ~(1 << 2);
    AR0330_PARALLEL_RegWrite(0x301A, Data);

    if (AR0330_PARALLEL_ConfirmStandbyMode() == -1)
        AmbaPrint("cannot enter standby mode!");

    /* dump registers */
    NumRegs = sizeof(RegToDump) / sizeof(RegToDump[0]);
    for (i = 0; i < NumRegs; i++) {
        UINT16 Addr = RegToDump[i];
        UINT16 Data;

        AR0330_PARALLEL_RegRead(Addr, &Data);
        AmbaPrint("REG= 0x%04X, 0x%04X (decimal: %u)",
                  Addr, Data, Data);
    }

    /* dump sequencer */
    AR0330_PARALLEL_SequencerDump();

    /* Start streaming */
    AR0330_PARALLEL_SetRegR0x301A(0/*restart_frame*/, 1/*streaming*/);

    return OK;
}
#endif

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AR0330_PARALLEL_GetStatus
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
static int AR0330_PARALLEL_GetStatus(AMBA_DSP_CHANNEL_ID_u Chan, AMBA_SENSOR_STATUS_INFO_s *pStatus)
{
    if (pStatus == NULL || AR0330_PARALLELCtrl.Status.DevStat.Bits.Sensor0Power == 0)
        return NG;

    AR0330_PARALLELCtrl.Status.TimeToAllPelExposed = 0;  /* TODO: for flash light or mechanical shutter control */

    memcpy(pStatus, &AR0330_PARALLELCtrl.Status, sizeof(AMBA_SENSOR_STATUS_INFO_s));

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AR0330_PARALLEL_GetModeInfo
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
static int AR0330_PARALLEL_GetModeInfo(AMBA_DSP_CHANNEL_ID_u Chan, AMBA_SENSOR_MODE_ID_u Mode, AMBA_SENSOR_MODE_INFO_s *pModeInfo)
{
    UINT16 SensorMode = Mode.Bits.Mode;

    if (SensorMode == AMBA_SENSOR_CURRENT_MODE)
        SensorMode = AR0330_PARALLELCtrl.Status.ModeInfo.Mode.Bits.Mode;

    if (SensorMode >= AMBA_SENSOR_AR0330_PARALLEL_NUM_MODE || pModeInfo == NULL)
        return NG;

    AR0330_PARALLEL_PrepareModeInfo(SensorMode, pModeInfo, NULL);

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AR0330_PARALLEL_GetDeviceInfo
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
static int AR0330_PARALLEL_GetDeviceInfo(AMBA_DSP_CHANNEL_ID_u Chan, AMBA_SENSOR_DEVICE_INFO_s *pDeviceInfo)
{
    if (pDeviceInfo == NULL)
        return NG;

    memcpy(pDeviceInfo, &AR0330_PARALLELDeviceInfo, sizeof(AMBA_SENSOR_DEVICE_INFO_s));

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AR0330_GetCurrentGainFactor
 *
 *  @Description:: Get Sensor Current Gain Factor
 *
 *  @Input      ::
 *      Chan:   Vin ID and sensor ID
 *
 *  @Output     ::
 *      pDeviceInfo:    pointer to current gain factor
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static int AR0330_PARALLEL_GetCurrentGainFactor(AMBA_DSP_CHANNEL_ID_u Chan, float *pGainFactor)
{
    *pGainFactor = ((float)AR0330_PARALLELCtrl.GainFactor / (1024.0 * 1024.0) );

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AR0330_GetCurrentShutterSpeed
 *
 *  @Description:: Get Sensor Current Exposure Time
 *
 *  @Input      ::
 *      Chan:   Vin ID and sensor ID
 *
 *  @Output     ::
 *      pDeviceInfo:    pointer to current exposure time
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static int AR0330_PARALLEL_GetCurrentShutterSpeed(AMBA_DSP_CHANNEL_ID_u Chan, float *pExposureTime)
{
    *pExposureTime = (AR0330_PARALLELCtrl.Status.ModeInfo.RowTime * (float)AR0330_PARALLELCtrl.ShutterCtrl);

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AR0330_PARALLEL_ConvertGainFactor
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
static int AR0330_PARALLEL_ConvertGainFactor(AMBA_DSP_CHANNEL_ID_u Chan, float DesiredFactor, UINT32 *pActualFactor, UINT32 *pAnalogGainCtrl, UINT32 *pDigitalGainCtrl)
{
    float DgcFactor = 0.0;
	float AgcFactor = 0.0;
    UINT16 AgcR3060 = 0;
    UINT16 DgcR305E = 0;
	UINT8 DgcFrac = 0;
	UINT8 DgcInt = 0;


    if (DesiredFactor > 127.9375) {
        AgcR3060 = 0x0030;  // Max Agc
        DgcFactor = 15.9921875;
        AgcFactor = 8;
    } else if (DesiredFactor >= 8){
        AgcR3060 =  0x0030;
        DgcFactor = DesiredFactor / 8.0;
        AgcFactor = 8;
    } else if (DesiredFactor >= 4){
        if(DesiredFactor >= 6.4) {
			AgcR3060 =  0x002c;
            DgcFactor = DesiredFactor / 6.4;
            AgcFactor = 6.4;
    	} else if(DesiredFactor >= 5.33) {
			AgcR3060 =  0x0028;
            DgcFactor = DesiredFactor / 5.33;
            AgcFactor = 5.33;
    	} else if(DesiredFactor >= 4.57) {
			AgcR3060 =  0x0024;
            DgcFactor = DesiredFactor / 4.57;
            AgcFactor = 4.57;
    	} else if(DesiredFactor >= 4.0) {
			AgcR3060 =  0x0020;
            DgcFactor = DesiredFactor / 4.0;
            AgcFactor = 4.0;
    	}
    } else if (DesiredFactor >= 2){
    	if(DesiredFactor >= 3.56) {
			AgcR3060 =  0x001e;
            DgcFactor = DesiredFactor / 3.56;
            AgcFactor = 3.56;
    	} else if(DesiredFactor >= 3.2) {
			AgcR3060 =  0x001c;
            DgcFactor = DesiredFactor / 3.2;
            AgcFactor = 3.2;
    	} else if(DesiredFactor >= 2.91) {
			AgcR3060 =  0x001a;
            DgcFactor = DesiredFactor / 2.91;
            AgcFactor =2.91;
    	} else if(DesiredFactor >= 2.67) {
			AgcR3060 =  0x0018;
            DgcFactor = DesiredFactor / 2.67;
            AgcFactor = 2.67;
    	} else if(DesiredFactor >= 2.46) {
			AgcR3060 =  0x0016;
            DgcFactor = DesiredFactor / 2.46;
            AgcFactor = 2.46;
    	} else if(DesiredFactor >= 2.29) {
			AgcR3060 =  0x0014;
            DgcFactor = DesiredFactor / 2.29;
            AgcFactor = 2.29;
    	} else if(DesiredFactor >= 2.13) {
			AgcR3060 =  0x0012;
            DgcFactor = DesiredFactor / 2.13;
            AgcFactor = 2.13;
    	} else if(DesiredFactor >= 2.0) {
			AgcR3060 =  0x0010;
            DgcFactor = DesiredFactor / 2.0;
            AgcFactor = 2.0;
    	}
    } else if (DesiredFactor >= 1){
    	if(DesiredFactor >= 1.88) {
			AgcR3060 =  0x000f;
            DgcFactor = DesiredFactor / 1.88;
            AgcFactor = 1.88;
    	} else if(DesiredFactor >= 1.78) {
			AgcR3060 =  0x000e;
            DgcFactor = DesiredFactor / 1.78;
            AgcFactor = 1.78;
    	} else if(DesiredFactor >= 1.68) {
			AgcR3060 =  0x000d;
            DgcFactor = DesiredFactor / 1.68;
            AgcFactor = 1.68;
    	} else if(DesiredFactor >= 1.60) {
			AgcR3060 =  0x000c;
            DgcFactor = DesiredFactor / 1.60;
            AgcFactor = 1.60;
    	} else if(DesiredFactor >= 1.52) {
			AgcR3060 =  0x000b;
            DgcFactor = DesiredFactor / 1.52;
            AgcFactor = 1.52;
    	} else if(DesiredFactor >= 1.45) {
			AgcR3060 =  0x000a;
            DgcFactor = DesiredFactor / 1.45;
            AgcFactor = 1.45;
    	} else if(DesiredFactor >= 1.39) {
			AgcR3060 =  0x0009;
            DgcFactor = DesiredFactor / 1.39;
            AgcFactor = 1.39;
    	} else if(DesiredFactor >= 1.33) {
			AgcR3060 =  0x0008;
            DgcFactor = DesiredFactor / 1.33;
            AgcFactor = 1.33;
    	} else if(DesiredFactor >= 1.28) {
			AgcR3060 =  0x0007;
            DgcFactor = DesiredFactor / 1.28;
            AgcFactor = 1.28;
    	} else if(DesiredFactor >= 1.23) {
			AgcR3060 =  0x0006;
            DgcFactor = DesiredFactor / 1.23;
            AgcFactor = 1.23;
    	} else if(DesiredFactor >= 1.19) {
			AgcR3060 =  0x0005;
            DgcFactor = DesiredFactor / 1.19;
            AgcFactor = 1.19;
    	} else if(DesiredFactor >= 1.14) {
			AgcR3060 =  0x0004;
            DgcFactor = DesiredFactor / 1.14;
            AgcFactor = 1.14;
    	} else if(DesiredFactor >= 1.1) {
			AgcR3060 =  0x0003;
            DgcFactor = DesiredFactor / 1.1;
            AgcFactor = 1.1;
    	} else if(DesiredFactor >= 1.07) {
			AgcR3060 =  0x0002;
            DgcFactor = DesiredFactor / 1.07;
            AgcFactor = 1.07;
    	} else if(DesiredFactor >= 1.03) {
			AgcR3060 =  0x0001;
            DgcFactor = DesiredFactor / 1.03;
            AgcFactor = 1.03;
    	} else if(DesiredFactor >= 1.0) {
			AgcR3060 =  0x0000;
            DgcFactor = DesiredFactor;
            AgcFactor = 1.0;
    	}

    }

    /* Set Digital gain */
    DgcInt = (UINT8)DgcFactor;
    DgcFrac = (UINT8)((DgcFactor - DgcInt) * 128);
    DgcR305E = (DgcInt << 7) + DgcFrac;

    *pAnalogGainCtrl  = AgcR3060;
    *pDigitalGainCtrl = DgcR305E;
    *pActualFactor = (UINT32)(1024 * 1024 * AgcFactor * ((float)DgcInt + ((float)DgcFrac / 128.0)));
    AR0330_PARALLELCtrl.GainFactor= *pActualFactor;
    //AmbaPrint("DesiredFactor = %f, DgcR305E = 0x%08x, AgcR3060 = 0x%08x, ActualFactor = %d",DesiredFactor, *pDigitalGainCtrl, *pAnalogGainCtrl, *pActualFactor);

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AR0330_PARALLEL_ConvertShutterSpeed
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
static int AR0330_PARALLEL_ConvertShutterSpeed(AMBA_DSP_CHANNEL_ID_u Chan, float ExposureTime, UINT32 *pShutterCtrl)
{
    AMBA_SENSOR_MODE_INFO_s *pModeInfo = &AR0330_PARALLELCtrl.Status.ModeInfo;

    if (pShutterCtrl == NULL)
        return NG;

    *pShutterCtrl = (UINT32)(ExposureTime / pModeInfo->RowTime);

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AR0330_PARALLEL_SetAnalogGainCtrl
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
static int AR0330_PARALLEL_SetAnalogGainCtrl(AMBA_DSP_CHANNEL_ID_u Chan, UINT32 AnalogGainCtrl)
{
    UINT16 Data = (UINT16) AnalogGainCtrl;

    AR0330_PARALLEL_RegWrite(0x3060, Data);

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AR0330_PARALLEL_SetDigitalGainCtrl
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
static int AR0330_PARALLEL_SetDigitalGainCtrl(AMBA_DSP_CHANNEL_ID_u Chan, UINT32 DigitalGainCtrl)
{
    UINT16 Data = (UINT16) DigitalGainCtrl;

    AR0330_PARALLEL_RegWrite(0x305E, Data);

    return OK;
}


/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AR0330_PARALLEL_SetShutterCtrl
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
static int AR0330_PARALLEL_SetShutterCtrl(AMBA_DSP_CHANNEL_ID_u Chan, UINT32 ShutterCtrl)
{
    AMBA_SENSOR_MODE_INFO_s *pModeInfo = &AR0330_PARALLELCtrl.Status.ModeInfo;
    UINT16 Data = 0;


	/* Exposure line needs be smaller than VTS - 2 */
	if (ShutterCtrl >= pModeInfo->NumExposureStepPerFrame  - 2)
		ShutterCtrl = pModeInfo->NumExposureStepPerFrame - 2;

	if (ShutterCtrl <= 1)
		ShutterCtrl = 1;

	Data = (UINT16)ShutterCtrl;

    AR0330_PARALLEL_RegWrite(0x3012, Data);

	AR0330_PARALLELCtrl.ShutterCtrl = ShutterCtrl;
    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AR0330_PARALLEL_SetSlowShutterCtrl
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
static int AR0330_PARALLEL_SetSlowShutterCtrl(AMBA_DSP_CHANNEL_ID_u Chan, UINT32 SlowShutterCtrl)
{
    UINT32 TargetFrameLengthLines = AR0330_PARALLELCtrl.FrameTime.FrameLengthLines * SlowShutterCtrl;

    if (SlowShutterCtrl < 1)
        SlowShutterCtrl = 1;

    AR0330_PARALLELCtrl.Status.ModeInfo.FrameLengthLines = TargetFrameLengthLines;
    AR0330_PARALLELCtrl.Status.ModeInfo.FrameTime.FrameRate.NumUnitsInTick =
		AR0330_PARALLELModeInfoList[AR0330_PARALLELCtrl.Status.ModeInfo.Mode.Bits.Mode].FrameTiming.FrameRate.NumUnitsInTick * SlowShutterCtrl;
	AR0330_PARALLELCtrl.Status.ModeInfo.NumExposureStepPerFrame = TargetFrameLengthLines;

	AR0330_PARALLEL_RegWrite(0x300A, (UINT16)TargetFrameLengthLines);
    //AmbaPrint("[AR0330]Linear slowshutter FrameLengthLines:%d, SlowShutterCtrl:%d, TargetFrameLengthLines:%d", AR0330_PARALLELCtrl.FrameTime.FrameLengthLines, SlowShutterCtrl, TargetFrameLengthLines);

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AR0330_PARALLEL_Config
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
static int AR0330_PARALLEL_Config(AMBA_DSP_CHANNEL_ID_u Chan, AMBA_SENSOR_MODE_ID_u Mode, AMBA_SENSOR_ESHUTTER_TYPE_e ElecShutterMode)
{
    AMBA_SENSOR_MODE_INFO_s *pModeInfo = NULL;
    AR0330_PARALLEL_FRAME_TIMING_s *pFrameTime = NULL;
    UINT16 SensorMode = Mode.Bits.Mode;
    AMBA_VIN_CHANNEL_e VinChanNo = (AMBA_VIN_CHANNEL_e) Chan.Bits.VinID;

    if (SensorMode >= AMBA_SENSOR_AR0330_PARALLEL_NUM_MODE)
        return NG;

    AmbaPrint("============  [ AR0330_PARALLEL ]  ============");
    AmbaPrint("============ init mode:%d ============", SensorMode);

    AR0330_PARALLELCtrl.Status.ElecShutterMode = ElecShutterMode;

    pModeInfo  = &AR0330_PARALLELCtrl.Status.ModeInfo;
    pFrameTime = &AR0330_PARALLELCtrl.FrameTime;
    AR0330_PARALLEL_PrepareModeInfo(SensorMode, pModeInfo, pFrameTime);

    /* Make VIN not to capture bad frames during readout mode transition */
    AmbaVIN_Reset(VinChanNo, AMBA_VIN_DVP, NULL);

    AmbaKAL_TaskSleep(3);

    AmbaPLL_SetSensorClk(pModeInfo->FrameTime.InputClk);

    AmbaKAL_TaskSleep(3);

    AR0330_PARALLEL_SoftwareReset();

    AR0330_PARALLEL_InitSeq(SensorMode);
    AR0330_PARALLEL_ConfigIF(SensorMode);
    AR0330_PARALLEL_ConfigPLL(SensorMode);
    AR0330_PARALLEL_ChangeReadoutMode(SensorMode);

    AR0330_PARALLEL_RegWrite(0x3012, 0x0008);

    AR0330_PARALLEL_SetStandbyOff();

    AR0330_PARALLEL_ConfigVin(VinChanNo, pModeInfo, pFrameTime);

    AR0330_PARALLELCtrl.Status.DevStat.Bits.Sensor0Standby = 0;

    return OK;
}

/*-----------------------------------------------------------------------------------------------*/
AMBA_SENSOR_OBJ_s AmbaSensor_AR0330_PARALLELObj = {
    .Init                   = AR0330_PARALLEL_Init,
    .Enable                 = AR0330_PARALLEL_Enable,
    .Disable                = AR0330_PARALLEL_Disable,
    .Config                 = AR0330_PARALLEL_Config,
    .ChangeFrameRate        = AR0330_PARALLEL_ChangeFrameRate,
    .GetStatus              = AR0330_PARALLEL_GetStatus,
    .GetModeInfo            = AR0330_PARALLEL_GetModeInfo,
    .GetDeviceInfo          = AR0330_PARALLEL_GetDeviceInfo,
    .GetCurrentGainFactor   = AR0330_PARALLEL_GetCurrentGainFactor,
    .GetCurrentShutterSpeed = AR0330_PARALLEL_GetCurrentShutterSpeed,

    .ConvertGainFactor      = AR0330_PARALLEL_ConvertGainFactor,
    .ConvertShutterSpeed    = AR0330_PARALLEL_ConvertShutterSpeed,

    .SetAnalogGainCtrl      = AR0330_PARALLEL_SetAnalogGainCtrl,
    .SetDigitalGainCtrl     = AR0330_PARALLEL_SetDigitalGainCtrl,
    .SetShutterCtrl         = AR0330_PARALLEL_SetShutterCtrl,
    .SetSlowShutterCtrl     = AR0330_PARALLEL_SetSlowShutterCtrl,

    .SetGainFactor          = AR0330_PARALLEL_SetGainFactor,
    .SetEshutterSpeed       = AR0330_PARALLEL_SetEshutterSpeed,

    .RegisterRead           = AR0330_PARALLEL_RegisterRead,
    .RegisterWrite          = AR0330_PARALLEL_RegisterWrite,

};
