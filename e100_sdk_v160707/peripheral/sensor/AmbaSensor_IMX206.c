/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaSensor_IMX206.c
 *
 *  @Copyright      :: Copyright (C) 2013 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Control APIs of SONY IMX206 CMOS sensor with LVDS interface
\*-------------------------------------------------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "AmbaDataType.h"
#include "AmbaKAL.h"

#include "AmbaDSP.h"
#include "AmbaVIN.h"
#include "AmbaDSP_VIN.h"

#include "AmbaSPI.h"
#include "AmbaGPIO.h"
#include "AmbaPLL.h"
#include "AmbaSensor.h"
#include "AmbaSensor_IMX206.h"

#include "AmbaPrintk.h"

/*-----------------------------------------------------------------------------------------------*\
 * IMX206 sensor runtime info
\*-----------------------------------------------------------------------------------------------*/
IMX206_CTRL_s IMX206Ctrl = {0};
UINT8 IMX206SpiBuf[2][128];

/*-----------------------------------------------------------------------------------------------*\
 * Configurations for Amba. VIN side
\*-----------------------------------------------------------------------------------------------*/
static AMBA_DSP_VIN_SLVS_PIN_PAIR_e IMX206LvdsLaneMux[][AMBA_DSP_NUM_VIN_SLVS_DATA_LANE] = {
    [IMX206_LVDS_4CH]   = {
        AMBA_DSP_VIN_PIN_SD_LVDS_0,
        AMBA_DSP_VIN_PIN_SD_LVDS_1,
        AMBA_DSP_VIN_PIN_SD_LVDS_2,
        AMBA_DSP_VIN_PIN_SD_LVDS_3
    },

    [IMX206_LVDS_2CH]   = {
        AMBA_DSP_VIN_PIN_SD_LVDS_1,
        AMBA_DSP_VIN_PIN_SD_LVDS_2
    },

    [IMX206_LVDS_1CH]   = {
        AMBA_DSP_VIN_PIN_SD_LVDS_1
    },
};

static AMBA_DSP_VIN_SLVS_CONFIG_s IMX206VinConfig = {
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

static AMBA_DSP_VIN_MASTER_SYNC_CONFIG_s IMX206MasterSyncConfig = {
    .HSync = {
        .Period     = 0,
        .PulseWidth = 0,
        .Polarity   = 0
    },
    .VSync = {
        .Period     = 0,
        .PulseWidth = 0,
        .Polarity   = 0
    },
    .VSyncDelayCycles = 0,
    .ToggleHsyncInVblank = 1
};

/*-----------------------------------------------------------------------------------------------*\
 * SPI configuration, {SpiChanNo, SpiSlaveId, SpiMode, CsPol, DataFrmSize, BaudRate}
\*-----------------------------------------------------------------------------------------------*/
static AMBA_SPI_CONFIG_s IMX206SpiConfig = {
    .SlaveID       = AMBA_SENSOR_SPI_SLAVE_ID,                                 /* Slave ID */
    .ProtocolMode  = AMBA_SPI_CPOL_HIGH_CPHA_HIGH,      /* SPI Protocol mode */
    .CsPolarity    = AMBA_SPI_CHIP_SELECT_ACTIVE_LOW,   /* Slave select polarity */
    .DataFrameSize = 8,                                 /* Data Frame Size in Bit */
    .BaudRate      = 10000000                           /* Transfer BaudRate in Hz */
};

/*-----------------------------------------------------------------------------------------------*\
 * Reverse bit order in a byte for IMX206 SPI communication
\*-----------------------------------------------------------------------------------------------*/
static inline UINT8 IMX206_SpiReverseBitOrder(UINT16 Data)
{
    UINT8 DataOut = ((Data & 0x01) << 7) |
                    ((Data & 0x02) << 5) |
                    ((Data & 0x04) << 3) |
                    ((Data & 0x08) << 1) |
                    ((Data & 0x10) >> 1) |
                    ((Data & 0x20) >> 3) |
                    ((Data & 0x40) >> 5) |
                    ((Data & 0x80) >> 7);

    return DataOut;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX206_PrepareModeInfo
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
static void IMX206_PrepareModeInfo(AMBA_SENSOR_MODE_ID_u Mode, AMBA_SENSOR_MODE_INFO_s *pModeInfo)
{
    UINT16 SensorMode                             = Mode.Bits.Mode;
    IMX206_READOUT_MODE_e           ReadoutMode   = IMX206ModeInfoList[SensorMode].ReadoutMode;
    const IMX206_FRAME_TIMING_s     *pFrameTiming = &IMX206ModeInfoList[SensorMode].FrameTiming;
    const AMBA_SENSOR_OUTPUT_INFO_s *pOutputInfo  = &IMX206OutputInfo[ReadoutMode];
    const AMBA_SENSOR_INPUT_INFO_s  *pInputInfo;

    pInputInfo = &IMX206InputInfoNormalReadout[ReadoutMode];

    pModeInfo->Mode = Mode;
    pModeInfo->LineLengthPck = (pFrameTiming->NumTickPerXhs * pFrameTiming->NumXhsPerH) *
                               (pOutputInfo->DataRate / pFrameTiming->InputClk) *
                               (pOutputInfo->NumDataLanes) / (pOutputInfo->NumDataBits);
    pModeInfo->FrameLengthLines = pFrameTiming->NumXhsPerV / pFrameTiming->NumXhsPerH;
    pModeInfo->NumExposureStepPerFrame = pFrameTiming->NumXhsPerV;
    pModeInfo->PixelRate        = (float) pOutputInfo->DataRate * pOutputInfo->NumDataLanes / pOutputInfo->NumDataBits;
    pModeInfo->RowTime = (float) (pFrameTiming->NumTickPerXhs * pFrameTiming->NumXhsPerH) / pFrameTiming->InputClk;
    pModeInfo->InternalExposureOffset = 0;
    pModeInfo->FrameTime.InputClk = pFrameTiming->InputClk;
    memcpy(&pModeInfo->FrameTime.FrameRate, &pFrameTiming->FrameRate, sizeof(AMBA_DSP_FRAME_RATE_s));
    memcpy(&pModeInfo->InputInfo, pInputInfo, sizeof(AMBA_SENSOR_INPUT_INFO_s));
    memcpy(&pModeInfo->OutputInfo, pOutputInfo, sizeof(AMBA_SENSOR_OUTPUT_INFO_s));
    memcpy(&pModeInfo->MinFrameRate, &pFrameTiming->FrameRate, sizeof(AMBA_DSP_FRAME_RATE_s));

    /* HDR information */
    memset(&pModeInfo->HdrInfo, 0, sizeof(AMBA_SENSOR_HDR_INFO_s)); // Not support HDR

    /* updated minimum frame rate limitation */
    pModeInfo->MinFrameRate.NumUnitsInTick *= 0x3FFF / (pFrameTiming->NumXhsPerV / pFrameTiming->NumXvsPerV);
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX206_ConfigVin
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
static void IMX206_ConfigVin(AMBA_SENSOR_MODE_INFO_s *pModeInfo, IMX206_FRAME_TIMING_s *pFrameTime)
{
    AMBA_SENSOR_OUTPUT_INFO_s  *pImgInfo = &pModeInfo->OutputInfo;
    const IMX206_FRAME_TIMING_s *pImx206FrameTiming = &IMX206ModeInfoList[pModeInfo->Mode.Bits.Mode].FrameTiming;
    AMBA_DSP_VIN_SLVS_CONFIG_s *pVinCfg = &IMX206VinConfig;
    AMBA_DSP_VIN_MASTER_SYNC_CONFIG_s *pMasterSyncCfg = &IMX206MasterSyncConfig;

    AmbaDSP_VinPhySetSLVS(AMBA_VIN_CHANNEL0);

    memcpy(&pVinCfg->Info.FrameRate, &pFrameTime->FrameRate, sizeof(AMBA_DSP_FRAME_RATE_s));
    memcpy(&pVinCfg->Info.DspPhaseShift, &pImgInfo->DspPhaseShift, sizeof(AMBA_DSP_PHASE_SHIFT_CTRL_s));
    pVinCfg->Info.BayerPattern = (AMBA_DSP_SENSOR_PATTERN_e) pImgInfo->CfaPattern;
    pVinCfg->Info.NumDataBits = pImgInfo->NumDataBits;

    pVinCfg->NumActiveLanes = pImgInfo->NumDataLanes;
    switch (pVinCfg->NumActiveLanes) {
    case 4:
        pVinCfg->pLaneMapping = IMX206LvdsLaneMux[IMX206_LVDS_4CH];
        break;
    case 2:
        pVinCfg->pLaneMapping = IMX206LvdsLaneMux[IMX206_LVDS_2CH];
        break;
    case 1:
    default:
        pVinCfg->pLaneMapping = IMX206LvdsLaneMux[IMX206_LVDS_1CH];
        break;
    }
    pVinCfg->RxHvSyncCtrl.NumActivePixels = pModeInfo->OutputInfo.OutputWidth;
    pVinCfg->RxHvSyncCtrl.NumActiveLines  = pModeInfo->OutputInfo.OutputHeight;
    pVinCfg->RxHvSyncCtrl.NumTotalPixels  = pModeInfo->LineLengthPck - 1;
    pVinCfg->RxHvSyncCtrl.NumTotalLines   = pModeInfo->FrameLengthLines;

    AmbaDSP_VinConfigSLVS(AMBA_VIN_CHANNEL0, pVinCfg);

    pMasterSyncCfg->HSync.Period = pImx206FrameTiming->NumTickPerXhs;
    pMasterSyncCfg->HSync.PulseWidth = 8;
    pMasterSyncCfg->VSync.Period = pImx206FrameTiming->NumXhsPerV / pImx206FrameTiming->NumXvsPerV;
    pMasterSyncCfg->VSync.PulseWidth = 8;

    AmbaDSP_VinConfigMasterSync(AMBA_VIN_CHANNEL0, pMasterSyncCfg);
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX206_HardwareReset
 *
 *  @Description:: Reset IMX206 Image Sensor Device
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static inline void IMX206_HardwareReset(void)
{
    AmbaUserGPIO_SensorResetCtrl(0);
    AmbaKAL_TaskSleep(1);               /* XCLR Low level pulse width >= 100ns */
    AmbaUserGPIO_SensorResetCtrl(1);
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX206_RegRW
 *
 *  @Description:: Read/Write sensor registers through SPI bus
 *
 *  @Input      ::
 *      Addr:       Register Address
 *      pTxData:    Pointer to Write data buffer
 *      pRxData:    Pointer to Echo data buffer
 *      Size:       Number of Read/Write data
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static int IMX206_RegRW(UINT16 Addr, UINT8 *pTxData, UINT8 *pRxData, UINT8 Size)
{
    UINT32  i;
    UINT8   *pDataBuf;

    if ((Size + 3) > sizeof(IMX206SpiBuf[0]))
        return NG;

    pDataBuf = &IMX206SpiBuf[0][0];
    *pDataBuf++ = IMX206_SpiReverseBitOrder(0x81);         /* Chip ID */
    *pDataBuf++ = IMX206_SpiReverseBitOrder(Addr >> 8);    /* Register Address (high byte) */
    *pDataBuf++ = IMX206_SpiReverseBitOrder(Addr & 0xff);  /* Register Address (low byte) */

    for (i = 0; i < Size; i++) {
        *pDataBuf++ = IMX206_SpiReverseBitOrder(pTxData[i]);
    }

    AmbaSPI_Transfer(AMBA_SPI_MASTER, &IMX206SpiConfig, Size + 3, IMX206SpiBuf[0], IMX206SpiBuf[1], 1000);
    
    if (pRxData != NULL) {
        for (i = 0; i < Size; i++) {
            *pRxData++ = IMX206_SpiReverseBitOrder(IMX206SpiBuf[1][3 + i]);
        }
    }
    
#if 0 // debug
    AmbaSPI_Transfer(AMBA_SPI_MASTER, &IMX206SpiConfig, Size + 3, IMX206SpiBuf[0], IMX206SpiBuf[1], 1000);
    for (i = 0; i < Size; i++) {
        AmbaPrintColor(RED, "Wr %04Xh %02Xh, Rd %02Xh",
                  Addr + i, pTxData[i],
                  IMX206_SpiReverseBitOrder(IMX206SpiBuf[1][3 + i]));
    }
    //AmbaPrintColor(RED, "Address: 0x%04x, W: 0x%04x, R: 0x%04x", Addr, *pTxData, IMX206_SpiReverseBitOrder(IMX206SpiBuf[1][3]));
#endif
    return OK;

}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX206_RegisterWrite
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
static int IMX206_RegisterWrite(AMBA_DSP_CHANNEL_ID_u Chan, UINT16 Addr, UINT16 Data)
{
    UINT8 RxBuffer;
    UINT8 wData = (UINT8)Data;
    IMX206_RegRW(Addr, &wData, &RxBuffer, 1);

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX206_RegisterRead
 *
 *  @Description:: Read sensor registers through SPI bus
 *
 *  @Input      ::
 *      Chan:   Vin ID and sensor ID
 *      Addr:   Pointer to Read data buffer
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *      int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static int IMX206_RegisterRead(AMBA_DSP_CHANNEL_ID_u Chan, UINT16 Addr, UINT16 *pData)
{
    UINT8 TxData[1];
    UINT8 RxData[1];

    TxData[0] = 0x00;
    IMX206_RegRW(Addr, TxData, RxData, 1);
    TxData[0] = RxData[0];
    IMX206_RegRW(Addr, TxData, NULL, 1);

    *pData = RxData[0];

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX206_SetAnalogGainReg
 *
 *  @Description:: Configure sensor analog gain setting
 *
 *  @Input      ::
 *      PGC:    Analog gain setting
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static void IMX206_SetAnalogGainReg(UINT16 PGC)
{
    //UINT16 Apgc01;
    //UINT16 Apgc02;
    UINT8  TxData[2];
    UINT8  RxData[2];
    /*
    IMX206_READOUT_MODE_e ReadoutMode =
        IMX206ModeInfoList[IMX206Ctrl.Status.ModeInfo.Mode.Bits.Mode].ReadoutMode;
    */
    TxData[1] = (PGC >> 8) & 0x00FF;
    TxData[0] = PGC & 0x00FF;
    IMX206_RegRW(IMX206_PGC_LSB_REG, TxData, RxData, 2);

}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX206_SetDigitalGainReg
 *
 *  @Description:: Configure sensor digital gain setting
 *
 *  @Input      ::
 *           DGC:    Digital gain setting
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static void IMX206_SetDigitalGainReg(UINT8 DGC)
{
    UINT8 TxData[1];
    UINT8 RxData[1];

    TxData[0] = DGC;
    IMX206_RegRW(IMX206_DGAIN_REG, TxData, RxData, 1);
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX206_SetShutterReg
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
static UINT32 IMX206_IntegrationPeriodInFrame = 1;  /* current intergration period (unit: Frame) */
static void IMX206_SetShutterReg(UINT32 NumXhsEshrSpeed)
{
    UINT32 NumXhsPerXvs = IMX206Ctrl.FrameTime.NumXhsPerV / IMX206Ctrl.FrameTime.NumXvsPerV;
    IMX206_READOUT_MODE_e ReadoutMode = IMX206ModeInfoList[IMX206Ctrl.Status.ModeInfo.Mode.Bits.Mode].ReadoutMode;
    UINT32 SVR; /* specifies the exposure end vertical period */
    UINT32 SHR; /* specifies the exposure start horizontal period */
    UINT32 SPL = 0;

    UINT8  TxData[2];
    UINT8  RxData[2];

    SVR = IMX206_IntegrationPeriodInFrame * IMX206Ctrl.FrameTime.NumXvsPerV - 1;

    if (ReadoutMode != IMX206_TYPE_2_3_MODE_5) {
        SHR = (SVR + 1) * NumXhsPerXvs - NumXhsEshrSpeed;
    } else {
        SHR = ((SVR + 1) * NumXhsPerXvs - NumXhsEshrSpeed) / 4;
    }


    /* The length of sensor SHR register is 16 bits,
       thus SPL should be applied when SHR is larger than 16 bits. */
    if (SHR > 65535) {
        SPL = SHR / NumXhsPerXvs;
        SHR = SHR % NumXhsPerXvs;

        TxData[0] = SPL & 0x00FF;
        TxData[1] = (SPL >> 8) & 0x00FF;
        IMX206_RegRW(IMX206_SPL_LSB_REG, TxData, RxData, 2);
    }

    TxData[0] = SHR & 0x00FF;
    TxData[1] = (SHR >> 8) & 0x00FF;
    IMX206_RegRW(IMX206_SHR_LSB_REG, TxData, RxData, 2);

    /* update shutter ctrl information */
    IMX206Ctrl.CurrentShrCtrlSVR = SVR;
    IMX206Ctrl.CurrentShrCtrlSPL = SPL;
    IMX206Ctrl.CurrentShrCtrlSHR = SHR;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX206_SetSlowShutterReg
 *
 *  @Description:: Apply slow shutter setting
 *
 *  @Input      ::
 *     IntegrationPeriodInFrame: Number of frames in integation period
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static void IMX206_SetSlowShutterReg(UINT32 IntegrationPeriodInFrame)
{
    IMX206_FRAME_TIMING_s *pFrameTime = &IMX206Ctrl.FrameTime;
    UINT32 SVR = 0;
    UINT8  TxData[2];
    UINT8  RxData[2];

    IMX206_IntegrationPeriodInFrame = IntegrationPeriodInFrame;

    /* 1 frame might includes more than 1 XVS */
    SVR = IMX206_IntegrationPeriodInFrame * pFrameTime->NumXvsPerV - 1;

    TxData[0] = SVR & 0x00FF;
    TxData[1] = (SVR >> 8) & 0x00FF;

    IMX206_RegRW(IMX206_SVR_LSB_REG, TxData, RxData, 2);

    /* update shutter ctrl information */
    IMX206Ctrl.CurrentShrCtrlSVR = SVR;

    IMX206Ctrl.Status.ModeInfo.FrameTime.FrameRate.NumUnitsInTick = IMX206ModeInfoList[IMX206Ctrl.Status.ModeInfo.Mode.Bits.Mode].FrameTiming.FrameRate.NumUnitsInTick * IntegrationPeriodInFrame;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX206_SetStandbyOn
 *
 *  @Description:: Configure sensor into standby mode
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static void IMX206_SetStandbyOn(void)
{
    UINT8 RegData = 0x02;
    IMX206_RegRW(IMX206_OP_REG, &RegData, NULL, 1);
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX206_SetStandbyOff
 *
 *  @Description:: Configure sensor to leave standby mode
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static void IMX206_SetStandbyOff(void)
{
    UINT8 RegData = 0x00;
    IMX206_RegRW(IMX206_OP_REG, &RegData, NULL, 1);
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX206_ChangeReadoutMode
 *
 *  @Description:: Switch sensor to requested mode.
 *                 It's for the case that sensor have been initialized.
 *
 *  @Input      ::
 *      ReadoutMode: Sensor readout mode
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static int IMX206_ChangeReadoutMode(IMX206_READOUT_MODE_e ReadoutMode)
{
    UINT8 TxRegData[8];
    int i;

    for (i = 0; i < IMX206_NUM_READOUT_MODE_REG; i++) {

        IMX206_RegRW(IMX206RegTable[i].Addr, &IMX206RegTable[i].Data[ReadoutMode], NULL, 1);
    }

    /* address 0x0001: CLPSQRST */
    TxRegData[0] = 0x11;
    IMX206_RegRW(IMX206_DCKRST_CLPSQRST_REG, TxRegData, NULL, 1);

    /* reset gain/shutter ctrl information */
    IMX206Ctrl.CurrentShrCtrlSHR = 0xffffffff;
    IMX206Ctrl.CurrentShrCtrlSVR = 0xffffffff;
    IMX206Ctrl.CurrentShrCtrlSPL = 0xffffffff;
    IMX206Ctrl.CurrentAgc = 0xffffffff;
    IMX206Ctrl.CurrentDgc = 0xffffffff;

    return OK;
}
/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX206_Init
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
static int IMX206_Init(AMBA_DSP_CHANNEL_ID_u Chan)
{
    const IMX206_FRAME_TIMING_s *pFrameTime = &IMX206ModeInfoList[0].FrameTiming;
    AmbaPLL_SetSensorClk(pFrameTime->InputClk);             /* The default sensor input clock frequency */
    AmbaPLL_SetSensorClkDir(AMBA_PLL_SENSOR_CLK_OUTPUT);    /* Config clock output to sensor (or sensor SPI won't work) */

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX206_Enable
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
static int IMX206_Enable(AMBA_DSP_CHANNEL_ID_u Chan)
{
    //UINT8 RegData;

    if (!IMX206Ctrl.Status.DevStat.Bits.Sensor0Power) {
#if 0
        /* Turn On the Sensor Power if applicable */
        AmbaUserGPIO_SensorPowerCtrl(1);    /* Turn On the Sensor Power */

#endif
        /* the Sensor Power is ON, Sensor is at Standby mode */
        IMX206Ctrl.Status.DevStat.Bits.Sensor0Power =
            IMX206Ctrl.Status.DevStat.Bits.Sensor0Standby = 1;

    }
    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX206_Disable
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
static int IMX206_Disable(AMBA_DSP_CHANNEL_ID_u Chan)
{
    if (IMX206Ctrl.Status.DevStat.Bits.Sensor0Power == 0)
        return OK;

    IMX206_SetStandbyOn();
    IMX206Ctrl.Status.DevStat.Bits.Sensor0Standby = 1;  /* Sensor is at Standby mode */

#if 0
    /* Turn Off the Sensor Power if applicable */
    AmbaUserGPIO_SensorPowerCtrl(0);    /* Turn Off the Sensor Power */
    IMX206Ctrl.Status.DevStat.Bits.Sensor0Power = 0;
#endif

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX206_GetStatus
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
static int IMX206_GetStatus(AMBA_DSP_CHANNEL_ID_u Chan, AMBA_SENSOR_STATUS_INFO_s *pStatus)
{
    if (pStatus == NULL || IMX206Ctrl.Status.DevStat.Bits.Sensor0Power == 0)
        return NG;

    IMX206Ctrl.Status.TimeToAllPelExposed = 0;  /* TODO: for flash light or mechanical shutter control */

    memcpy(pStatus, &IMX206Ctrl.Status, sizeof(AMBA_SENSOR_STATUS_INFO_s));

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX206_GetModeInfo
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
static int IMX206_GetModeInfo(AMBA_DSP_CHANNEL_ID_u Chan, AMBA_SENSOR_MODE_ID_u Mode, AMBA_SENSOR_MODE_INFO_s *pModeInfo)
{
    UINT16 SensorMode = Mode.Bits.Mode;

    if (Mode.Bits.Mode == AMBA_SENSOR_CURRENT_MODE)
        Mode.Bits.Mode = IMX206Ctrl.Status.ModeInfo.Mode.Bits.Mode;

    if (SensorMode >= AMBA_SENSOR_IMX206_NUM_MODE || pModeInfo == NULL)
        return NG;

    IMX206_PrepareModeInfo(Mode, pModeInfo);

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX206_GetDeviceInfo
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
static int IMX206_GetDeviceInfo(AMBA_DSP_CHANNEL_ID_u Chan, AMBA_SENSOR_DEVICE_INFO_s *pDeviceInfo)
{
    if (pDeviceInfo == NULL)
        return NG;

    memcpy(pDeviceInfo, &IMX206DeviceInfo, sizeof(AMBA_SENSOR_DEVICE_INFO_s));

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX206_GetCurrentGainFactor
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
static int IMX206_GetCurrentGainFactor(AMBA_DSP_CHANNEL_ID_u Chan, float *pGainFactor)
{
    *pGainFactor = 2048. / (2048 - IMX206Ctrl.CurrentAgc) * (1 << IMX206Ctrl.CurrentDgc);

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX206_GetCurrentShutterSpeed
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
static int IMX206_GetCurrentShutterSpeed(AMBA_DSP_CHANNEL_ID_u Chan, float *pExposureTime)
{
    IMX206_READOUT_MODE_e ReadoutMode = IMX206ModeInfoList[IMX206Ctrl.Status.ModeInfo.Mode.Bits.Mode].ReadoutMode;

    if (ReadoutMode != IMX206_TYPE_2_3_MODE_5) {
        *pExposureTime = (IMX206Ctrl.Status.ModeInfo.RowTime / IMX206Ctrl.FrameTime.NumXhsPerH) *
                         ((IMX206Ctrl.CurrentShrCtrlSVR - IMX206Ctrl.CurrentShrCtrlSPL + 1) * (IMX206Ctrl.FrameTime.NumXhsPerV / IMX206Ctrl.FrameTime.NumXvsPerV) - IMX206Ctrl.CurrentShrCtrlSHR);
    } else {
        *pExposureTime = (IMX206Ctrl.Status.ModeInfo.RowTime / IMX206Ctrl.FrameTime.NumXhsPerH) *
                         ((IMX206Ctrl.CurrentShrCtrlSVR + 1) * (IMX206Ctrl.FrameTime.NumXhsPerV / IMX206Ctrl.FrameTime.NumXvsPerV) - (IMX206Ctrl.CurrentShrCtrlSHR * 4));
    }

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX206_ConvertGainFactor
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
static int IMX206_ConvertGainFactor(AMBA_DSP_CHANNEL_ID_u Chan, float DesiredFactor, UINT32 *pActualFactor, UINT32 *pAnalogGainCtrl, UINT32 *pDigitalGainCtrl)
{
    if (pActualFactor == NULL || pAnalogGainCtrl == NULL || pDigitalGainCtrl == NULL)
        return NG;

    if (DesiredFactor < 1.0) {
        *pAnalogGainCtrl = 0;
        *pDigitalGainCtrl = 0;

        *pActualFactor = 1 << 20; /* .20 fixed point */
    } else {
        float AnalogGain;
        float DigitalGain;

        DigitalGain = DesiredFactor / 16;

        if (DigitalGain <= 1.0) {
            *pDigitalGainCtrl = 0;
            AnalogGain = DesiredFactor;
        } else {
            int i;

            for (i = 1; i < 3; i++) {
                if (DigitalGain < (1 << i)) {
                    break;
                }
            }

            *pDigitalGainCtrl = i;
            DigitalGain = 1 << i;

            AnalogGain = DesiredFactor / DigitalGain;
        }

        *pAnalogGainCtrl = (UINT32) ((2048 * (AnalogGain - 1.0) / AnalogGain) + 0.5);

        if (*pAnalogGainCtrl > 1957) {
            *pAnalogGainCtrl = 1957;
        }

        *pActualFactor = (UINT32) (2048. * (1 << 20) / (2048 - *pAnalogGainCtrl) * (1 << *pDigitalGainCtrl) + 0.5); /* ActualFactor .20 fixed point */
    }

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX206_ConvertShutterSpeed
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
static int IMX206_ConvertShutterSpeed(AMBA_DSP_CHANNEL_ID_u Chan, float ExposureTime, UINT32 *pShutterCtrl)
{
    AMBA_SENSOR_MODE_INFO_s *pModeInfo = &IMX206Ctrl.Status.ModeInfo;
    IMX206_READOUT_MODE_e ReadoutMode = IMX206ModeInfoList[pModeInfo->Mode.Bits.Mode].ReadoutMode;
    UINT32 ExpXhsNum    = 0;
    UINT32 MinExpXhsNum = 0;
    UINT32 MaxExpXhsNum = 0;

    if (pShutterCtrl == NULL)
        return NG;

    ExpXhsNum = (UINT32)(ExposureTime / pModeInfo->RowTime);

    if (IMX206Ctrl.Status.ElecShutterMode == AMBA_SENSOR_ESHUTTER_TYPE_GLOBAL_RESET) {
        MinExpXhsNum = 130;
        MaxExpXhsNum = ((ExpXhsNum -1) / pModeInfo->NumExposureStepPerFrame + 1) * pModeInfo->NumExposureStepPerFrame;
    } else {
        switch (ReadoutMode) {
        case IMX206_TYPE_2_3_MODE_0:
            MinExpXhsNum = 4;
            MaxExpXhsNum = ((ExpXhsNum -1) / pModeInfo->NumExposureStepPerFrame + 1) * pModeInfo->NumExposureStepPerFrame - 10;
            break;
        case IMX206_TYPE_2_3_MODE_2:
        case IMX206_TYPE_2_3_MODE_2A:
            MinExpXhsNum = 4;
            MaxExpXhsNum = ((ExpXhsNum -1) / pModeInfo->NumExposureStepPerFrame + 1) * pModeInfo->NumExposureStepPerFrame - 11;
            break;
        case IMX206_TYPE_2_3_MODE_3:
        case IMX206_TYPE_2_3_MODE_3B:
            MinExpXhsNum = 4;
            MaxExpXhsNum = ((ExpXhsNum -1) / pModeInfo->NumExposureStepPerFrame + 1) * pModeInfo->NumExposureStepPerFrame - 12;
            break;
        case IMX206_TYPE_2_3_MODE_4:
            MinExpXhsNum = 2;
            MaxExpXhsNum = ((ExpXhsNum -1) / pModeInfo->NumExposureStepPerFrame + 1) * pModeInfo->NumExposureStepPerFrame - 6;
            break;
        case IMX206_TYPE_2_3_MODE_5:
            MinExpXhsNum = 8;
            MaxExpXhsNum = ((ExpXhsNum -1) / pModeInfo->NumExposureStepPerFrame + 1) * pModeInfo->NumExposureStepPerFrame - 6 * 4;
            MaxExpXhsNum = (MaxExpXhsNum > 65535) ? 65535 : MaxExpXhsNum; // MaxSHR of mode 5 is 0xFFFF
            break;
        default:
            AmbaPrintColor(RED, "no defined mode in shutter API");
            return NG;
        }
    }

    if (ExpXhsNum > MaxExpXhsNum)
        *pShutterCtrl = MaxExpXhsNum;
    else if (ExpXhsNum < MinExpXhsNum)
        *pShutterCtrl = MinExpXhsNum;
    else
        *pShutterCtrl = ExpXhsNum;

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX206_SetAnalogGainCtrl
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
static int IMX206_SetAnalogGainCtrl(AMBA_DSP_CHANNEL_ID_u Chan, UINT32 AnalogGainCtrl)
{
    IMX206_SetAnalogGainReg(AnalogGainCtrl);

    IMX206Ctrl.CurrentAgc = AnalogGainCtrl;

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX206_SetDigitalGainCtrl
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
static int IMX206_SetDigitalGainCtrl(AMBA_DSP_CHANNEL_ID_u Chan, UINT32 DigitalGainCtrl)
{
    IMX206_SetDigitalGainReg(DigitalGainCtrl);

    IMX206Ctrl.CurrentDgc = DigitalGainCtrl;

    return OK;
}


/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX206_SetShutterCtrl
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
static int IMX206_SetShutterCtrl(AMBA_DSP_CHANNEL_ID_u Chan, UINT32 ShutterCtrl)
{
    IMX206_SetShutterReg(ShutterCtrl);

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX206_SetSlowShutterCtrl
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
static int IMX206_SetSlowShutterCtrl(AMBA_DSP_CHANNEL_ID_u Chan, UINT32 SlowShutterCtrl)
{
    IMX206_SetSlowShutterReg(SlowShutterCtrl);
    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX206_Config
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
static int IMX206_Config(AMBA_DSP_CHANNEL_ID_u Chan, AMBA_SENSOR_MODE_ID_u Mode, AMBA_SENSOR_ESHUTTER_TYPE_e ElecShutterMode)
{
    AMBA_SENSOR_MODE_INFO_s *pModeInfo;
    UINT16 SensorMode = Mode.Bits.Mode;
    IMX206_READOUT_MODE_e ReadoutMode = IMX206ModeInfoList[SensorMode].ReadoutMode;
    UINT8 xhs_l, xhs_h;

    if (SensorMode >= AMBA_SENSOR_IMX206_NUM_MODE)
        return NG;

    AmbaPrintColor(RED, "[IMX206]Config");
    if ((IMX206Ctrl.Status.ModeInfo.Mode.Bits.Mode == SensorMode) &&
        (IMX206Ctrl.Status.ModeInfo.Mode.Bits.VerticalFlip != Mode.Bits.VerticalFlip) &&
        (IMX206Ctrl.Status.ElecShutterMode == ElecShutterMode) &&
        (IMX206Ctrl.Status.DevStat.Bits.Sensor0Power) &&
        (!IMX206Ctrl.Status.DevStat.Bits.Sensor0Standby)) {
        /* run-time flip sensor readout direction */
        UINT8 RxData = 0;
        UINT8 TxData = Mode.Bits.VerticalFlip;
        IMX206_RegRW(0x001a, &TxData, &RxData, 1);

        /* update status */
        pModeInfo = &IMX206Ctrl.Status.ModeInfo;
        IMX206_PrepareModeInfo(Mode, pModeInfo);
    } else {
        UINT8 RxData = 0;
        UINT8 TxData;
        IMX206_FRAME_TIMING_s *pFrameTime = &IMX206Ctrl.FrameTime;

        /* update status */
        IMX206Ctrl.Status.ElecShutterMode = ElecShutterMode;
        pModeInfo = &IMX206Ctrl.Status.ModeInfo;
        IMX206_PrepareModeInfo(Mode, pModeInfo);
        memcpy(pFrameTime, &IMX206ModeInfoList[SensorMode].FrameTiming, sizeof(IMX206_FRAME_TIMING_s));

        /* Make VIN not to capture bad frames during readout mode transition */
        AmbaVIN_Reset((AMBA_VIN_CHANNEL_e) Chan.Bits.VinID, AMBA_VIN_LVDS, NULL);
        /* reset master sync*/
        AmbaVIN_ResetHvSyncOutput((AMBA_VIN_CHANNEL_e) Chan.Bits.VinID);
        /* set pll */
        AmbaPLL_SetSensorClk(pModeInfo->FrameTime.InputClk);
        AmbaPLL_SetSoVinClk(pModeInfo->FrameTime.InputClk);
        AmbaKAL_TaskSleep(1);

        IMX206_HardwareReset();
        AmbaKAL_TaskSleep(1);

        IMX206_SetStandbyOn();
        AmbaKAL_TaskSleep(1);

        /* V1 period */
        IMX206_SetStandbyOff();

        AmbaKAL_TaskSleep(16);

        /* V2 period */
        TxData = 0x11;
        IMX206_RegRW(IMX206_DCKRST_CLPSQRST_REG, &TxData, NULL, 1);

        IMX206_ChangeReadoutMode(ReadoutMode);

        xhs_l = (IMX206ModeInfoList[SensorMode].FrameTiming.NumTickPerXhs/2) & 0x00ff;
        xhs_h = ((IMX206ModeInfoList[SensorMode].FrameTiming.NumTickPerXhs/2) & 0xff00) >> 8;
        AmbaPrintColor(GREEN, "IMX206ModeInfoList[SensorMode].FrameTiming.NumTickPerXhs/2=%d, (0x%2x%2x)", IMX206ModeInfoList[SensorMode].FrameTiming.NumTickPerXhs/2, xhs_h, xhs_l);

        TxData = xhs_l;
        IMX206_RegRW(IMX206_HCOUNT_HALF_LSB_REG, &TxData, &RxData, 1);
        TxData = xhs_h;
        IMX206_RegRW(IMX206_HCOUNT_HALF_MSB_REG, &TxData, &RxData, 1);

        /* config sensor readout direction */
        TxData = Mode.Bits.VerticalFlip;
        IMX206_RegRW(0x001a, &TxData, &RxData, 1);

        /* Make VIN not to capture bad frames during readout mode transition */
        AmbaVIN_Reset((AMBA_VIN_CHANNEL_e) Chan.Bits.VinID, AMBA_VIN_LVDS, NULL);

        IMX206_ConfigVin(pModeInfo, pFrameTime);
    }

    return OK;
}

/*-----------------------------------------------------------------------------------------------*/
AMBA_SENSOR_OBJ_s AmbaSensor_IMX206Obj = {
    .Init                   = IMX206_Init,
    .Enable                 = IMX206_Enable,
    .Disable                = IMX206_Disable,
    .Config                 = IMX206_Config,
    .GetStatus              = IMX206_GetStatus,
    .GetModeInfo            = IMX206_GetModeInfo,
    .GetDeviceInfo          = IMX206_GetDeviceInfo,
    .GetCurrentGainFactor   = IMX206_GetCurrentGainFactor,
    .GetCurrentShutterSpeed = IMX206_GetCurrentShutterSpeed,
    .ConvertGainFactor      = IMX206_ConvertGainFactor,
    .ConvertShutterSpeed    = IMX206_ConvertShutterSpeed,
    .SetAnalogGainCtrl      = IMX206_SetAnalogGainCtrl,
    .SetDigitalGainCtrl     = IMX206_SetDigitalGainCtrl,
    .SetShutterCtrl         = IMX206_SetShutterCtrl,
    .SetSlowShutterCtrl     = IMX206_SetSlowShutterCtrl,

    .RegisterRead           = IMX206_RegisterRead,
    .RegisterWrite          = IMX206_RegisterWrite,
};
