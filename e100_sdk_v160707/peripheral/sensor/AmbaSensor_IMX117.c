/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaSensor_IMX117.c
 *
 *  @Copyright      :: Copyright (C) 2013 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Control APIs of SONY IMX117 CMOS sensor with LVDS interface
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
#include "AmbaSensor_IMX117.h"

#include "AmbaPrintk.h"

/*-----------------------------------------------------------------------------------------------*\
 * IMX117 sensor runtime info
\*-----------------------------------------------------------------------------------------------*/
IMX117_CTRL_s IMX117Ctrl = {0};
UINT8 IMX117SpiBuf[2][128];

/*-----------------------------------------------------------------------------------------------*\
 * Configurations for Amba. VIN side
\*-----------------------------------------------------------------------------------------------*/
static AMBA_DSP_VIN_SLVS_PIN_PAIR_e IMX117LvdsLaneMux[][AMBA_DSP_NUM_VIN_SLVS_DATA_LANE] = {
    [IMX117_LVDS_10CH]  = {
        AMBA_DSP_VIN_PIN_SD_LVDS_0,
        AMBA_DSP_VIN_PIN_SD_LVDS_1,
        AMBA_DSP_VIN_PIN_SD_LVDS_2,
        AMBA_DSP_VIN_PIN_SD_LVDS_8,
        AMBA_DSP_VIN_PIN_SD_LVDS_3,
        AMBA_DSP_VIN_PIN_SD_LVDS_4,
        AMBA_DSP_VIN_PIN_SD_LVDS_9,
        AMBA_DSP_VIN_PIN_SD_LVDS_5,
        AMBA_DSP_VIN_PIN_SD_LVDS_6,
        AMBA_DSP_VIN_PIN_SD_LVDS_7
    },

    [IMX117_LVDS_8CH]   = {
        AMBA_DSP_VIN_PIN_SD_LVDS_0,
        AMBA_DSP_VIN_PIN_SD_LVDS_1,
        AMBA_DSP_VIN_PIN_SD_LVDS_2,
        AMBA_DSP_VIN_PIN_SD_LVDS_3,
        AMBA_DSP_VIN_PIN_SD_LVDS_4,
        AMBA_DSP_VIN_PIN_SD_LVDS_5,
        AMBA_DSP_VIN_PIN_SD_LVDS_6,
        AMBA_DSP_VIN_PIN_SD_LVDS_7
    },

    [IMX117_LVDS_6CH]   = {
        AMBA_DSP_VIN_PIN_SD_LVDS_1,
        AMBA_DSP_VIN_PIN_SD_LVDS_2,
        AMBA_DSP_VIN_PIN_SD_LVDS_3,
        AMBA_DSP_VIN_PIN_SD_LVDS_4,
        AMBA_DSP_VIN_PIN_SD_LVDS_5,
        AMBA_DSP_VIN_PIN_SD_LVDS_6
    },

    [IMX117_LVDS_4CH]   = {
        AMBA_DSP_VIN_PIN_SD_LVDS_2,
        AMBA_DSP_VIN_PIN_SD_LVDS_3,
        AMBA_DSP_VIN_PIN_SD_LVDS_4,
        AMBA_DSP_VIN_PIN_SD_LVDS_5
    },

    [IMX117_LVDS_2CH]   = {
        AMBA_DSP_VIN_PIN_SD_LVDS_3,
        AMBA_DSP_VIN_PIN_SD_LVDS_4
    },

    [IMX117_LVDS_1CH]   = {
        AMBA_DSP_VIN_PIN_SD_LVDS_3
    },
};

static AMBA_DSP_VIN_SLVS_CONFIG_s IMX117VinConfig = {
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
                .Eol    =  0,
                .Sof    =  0,
                .Eof    =  0,
                .Sov    =  1,
                .Eov    =  0,
            },
            .PatternSol = 0x8000,
            .PatternEol = 0,
            .PatternSof = 0,
            .PatternEof = 0,
            .PatternSov = 0xab00,
            .PatternEov = 0,
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

static AMBA_DSP_VIN_MASTER_SYNC_CONFIG_s IMX117MasterSyncConfig = {
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
static AMBA_SPI_CONFIG_s IMX117SpiConfig = {
    .SlaveID       = AMBA_SENSOR_SPI_SLAVE_ID,                                 /* Slave ID */
    .ProtocolMode  = AMBA_SPI_CPOL_HIGH_CPHA_HIGH,      /* SPI Protocol mode */
    .CsPolarity    = AMBA_SPI_CHIP_SELECT_ACTIVE_LOW,   /* Slave select polarity */
    .DataFrameSize = 8,                                 /* Data Frame Size in Bit */
    .BaudRate      = 10000000                           /* Transfer BaudRate in Hz */
};

/*-----------------------------------------------------------------------------------------------*\
 * Reverse bit order in a byte for IMX117 SPI communication
\*-----------------------------------------------------------------------------------------------*/
static inline UINT8 IMX117_SpiReverseBitOrder(UINT16 Data)
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
 *  @RoutineName:: IMX117_PrepareModeInfo
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
static void IMX117_PrepareModeInfo(AMBA_SENSOR_MODE_ID_u Mode, AMBA_SENSOR_MODE_INFO_s *pModeInfo)
{
    UINT16 SensorMode                             = Mode.Bits.Mode;
    IMX117_READOUT_MODE_e           ReadoutMode   = IMX117ModeInfoList[SensorMode].ReadoutMode;
    const IMX117_FRAME_TIMING_s     *pFrameTiming = &IMX117ModeInfoList[SensorMode].FrameTiming;
    const AMBA_SENSOR_OUTPUT_INFO_s *pOutputInfo  = &IMX117OutputInfo[ReadoutMode];
    const AMBA_SENSOR_INPUT_INFO_s  *pInputInfo;

    if (Mode.Bits.VerticalFlip == 0)
        pInputInfo = &IMX117InputInfoNormalReadout[ReadoutMode];
    else if (Mode.Bits.VerticalFlip == 1)
        pInputInfo = &IMX117InputInfoInversionReadout[ReadoutMode];

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
    pModeInfo->MinFrameRate.NumUnitsInTick *= 0x3fff / (pFrameTiming->NumXhsPerV / pFrameTiming->NumXvsPerV);
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX117_ConfigVin
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
static void IMX117_ConfigVin(AMBA_SENSOR_MODE_INFO_s *pModeInfo, IMX117_FRAME_TIMING_s *pFrameTime)
{
    AMBA_SENSOR_OUTPUT_INFO_s  *pImgInfo = &pModeInfo->OutputInfo;
    const IMX117_FRAME_TIMING_s *pImx117FrameTiming = &IMX117ModeInfoList[pModeInfo->Mode.Bits.Mode].FrameTiming;
    AMBA_DSP_VIN_SLVS_CONFIG_s *pVinCfg = &IMX117VinConfig;
    AMBA_DSP_VIN_MASTER_SYNC_CONFIG_s *pMasterSyncCfg = &IMX117MasterSyncConfig;

    AmbaDSP_VinPhySetSLVS(AMBA_VIN_CHANNEL0);

    memcpy(&pVinCfg->Info.FrameRate, &pFrameTime->FrameRate, sizeof(AMBA_DSP_FRAME_RATE_s));
    memcpy(&pVinCfg->Info.DspPhaseShift, &pImgInfo->DspPhaseShift, sizeof(AMBA_DSP_PHASE_SHIFT_CTRL_s));
    pVinCfg->Info.BayerPattern = (AMBA_DSP_SENSOR_PATTERN_e) pImgInfo->CfaPattern;
    pVinCfg->Info.NumDataBits = pImgInfo->NumDataBits;

    pVinCfg->NumActiveLanes = pImgInfo->NumDataLanes;
    switch (pVinCfg->NumActiveLanes) {
    case 10:
        pVinCfg->pLaneMapping = IMX117LvdsLaneMux[IMX117_LVDS_10CH];
        break;
    case 8:
        pVinCfg->pLaneMapping = IMX117LvdsLaneMux[IMX117_LVDS_8CH];
        break;
    case 6:
        pVinCfg->pLaneMapping = IMX117LvdsLaneMux[IMX117_LVDS_6CH];
        break;
    case 4:
        pVinCfg->pLaneMapping = IMX117LvdsLaneMux[IMX117_LVDS_4CH];
        break;
    case 2:
        pVinCfg->pLaneMapping = IMX117LvdsLaneMux[IMX117_LVDS_2CH];
        break;
    case 1:
    default:
        pVinCfg->pLaneMapping = IMX117LvdsLaneMux[IMX117_LVDS_1CH];
        break;
    }
    pVinCfg->RxHvSyncCtrl.NumActivePixels = pImgInfo->OutputWidth;
    pVinCfg->RxHvSyncCtrl.NumActiveLines  = pImgInfo->OutputHeight;
    pVinCfg->RxHvSyncCtrl.NumTotalPixels  = pModeInfo->LineLengthPck - 1;
    pVinCfg->RxHvSyncCtrl.NumTotalLines   = pModeInfo->FrameLengthLines;

    AmbaDSP_VinConfigSLVS(AMBA_VIN_CHANNEL0, pVinCfg);

    pMasterSyncCfg->HSync.Period = pImx117FrameTiming->NumTickPerXhs;
    pMasterSyncCfg->HSync.PulseWidth = 8;
    pMasterSyncCfg->VSync.Period = pImx117FrameTiming->NumXhsPerV;
    pMasterSyncCfg->VSync.PulseWidth = 8;

    AmbaDSP_VinConfigMasterSync(AMBA_VIN_CHANNEL0, pMasterSyncCfg);
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX117_HardwareReset
 *
 *  @Description:: Reset IMX117 Image Sensor Device
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static inline void IMX117_HardwareReset(void)
{
    AmbaUserGPIO_SensorResetCtrl(0);
    AmbaKAL_TaskSleep(1);               /* XCLR Low level pulse width >= 100ns */
    AmbaUserGPIO_SensorResetCtrl(1);
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX117_RegRW
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
static int IMX117_RegRW(UINT16 Addr, UINT8 *pTxData, UINT8 *pRxData, UINT8 Size)
{

    UINT32  i;
    UINT8   *pDataBuf;

    if ((Size + 3) > sizeof(IMX117SpiBuf[0]))
        return NG;

    pDataBuf = &IMX117SpiBuf[0][0];
    *pDataBuf++ = IMX117_SpiReverseBitOrder(0x81);         /* Chip ID */
    *pDataBuf++ = IMX117_SpiReverseBitOrder(Addr >> 8);    /* Register Address (high byte) */
    *pDataBuf++ = IMX117_SpiReverseBitOrder(Addr & 0xff);  /* Register Address (low byte) */

    for (i = 0; i < Size; i++) {
        *pDataBuf++ = IMX117_SpiReverseBitOrder(pTxData[i]);
    }

    AmbaSPI_Transfer(AMBA_SPI_MASTER, &IMX117SpiConfig, Size + 3, IMX117SpiBuf[0], IMX117SpiBuf[1], 1000);
#if 0 //for debug
    AmbaSPI_Transfer(AMBA_SPI_MASTER, &IMX117SpiConfig, Size + 3, IMX117SpiBuf[0], IMX117SpiBuf[1], 1000);

    for (i = 0; i < Size; i++) {
        AmbaPrintColor(RED, "Wr %04Xh %02Xh, Rd %02Xh",
                  Addr + i, pTxData[i],
                  IMX117_SpiReverseBitOrder(IMX117SpiBuf[1][3 + i]));
    }
#endif
    if (pRxData != NULL) {
        for (i = 0; i < Size; i++) {
            *pRxData++ = IMX117_SpiReverseBitOrder(IMX117SpiBuf[1][3 + i]);
        }
    }

    return OK;

}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX117_RegisterWrite
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
static int IMX117_RegisterWrite(AMBA_DSP_CHANNEL_ID_u Chan, UINT16 Addr, UINT16 Data)
{
    UINT8 TxData[1];

    TxData[0] = (UINT8) Data;
    IMX117_RegRW(Addr, TxData, NULL, 1);

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX117_RegisterRead
 *
 *  @Description:: Read sensor registers through SPI bus
 *
 *  @Input      ::
 *      Chan:   Vin ID and sensor ID
 *      Addr:   Register address
 *
 *  @Output     ::
 *      pData:  Pointer to Read data buffer
 *
 *  @Return     ::
 *      int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static int IMX117_RegisterRead(AMBA_DSP_CHANNEL_ID_u Chan, UINT16 Addr, UINT16 *pData)
{
    UINT8 TxData[1];
    UINT8 RxData[1];

    TxData[0] = 0x00;
    IMX117_RegRW(Addr, TxData, RxData, 1);
    TxData[0] = RxData[0];
    IMX117_RegRW(Addr, TxData, NULL, 1);

    *pData = RxData[0];

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX117_SetAnalogGainReg
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
static void IMX117_SetAnalogGainReg(UINT16 PGC)
{
    UINT16 Apgc01 = 0;
    UINT16 Apgc02 = 0;
    UINT8  TxData[2];
    UINT8  RxData[2];
    IMX117_READOUT_MODE_e ReadoutMode =
        IMX117ModeInfoList[IMX117Ctrl.Status.ModeInfo.Mode.Bits.Mode].ReadoutMode;

    if (ReadoutMode == IMX117_TYPE_2_3_MODE_0 ||
        ReadoutMode == IMX117_TYPE_2_5_MODE_0 ||
        ReadoutMode == IMX117_TYPE_2_3_MODE_0A) {
        Apgc01 = 0x03d;
        Apgc02 = 0x03c;
    } else {
        if (PGC <= 0x146) {
            Apgc01 = 0x01f;
            Apgc02 = 0x01e;
        } else if (PGC <= 0x400) {
            Apgc01 = 0x021;
            Apgc02 = 0x020;
        } else if (PGC <= 0x52c) {
            Apgc01 = 0x026;
            Apgc02 = 0x025;
        } else if (PGC <= 0x696) {
            Apgc01 = 0x028;
            Apgc02 = 0x027;
        } else if (PGC <= 0x74b) {
            Apgc01 = 0x02c;
            Apgc02 = 0x02b;
        } else if (PGC <= 0x7a5) {
            Apgc01 = 0x030;
            Apgc02 = 0x02f;
        }
    }

    TxData[1] = (PGC >> 8) & 0x00ff;
    TxData[0] = PGC & 0x00ff;
    IMX117_RegRW(IMX117_PGC_LSB_REG, TxData, RxData, 2);

    TxData[1] = (Apgc01 >> 8) & 0x00ff;
    TxData[0] = Apgc01 & 0x00ff;
    IMX117_RegRW(IMX117_APGC01_LSB_REG, TxData, RxData, 2);

    TxData[1] = (Apgc02 >> 8) & 0x00ff;
    TxData[0] = Apgc02 & 0x00ff;
    IMX117_RegRW(IMX117_APGC02_LSB_REG, TxData, RxData, 2);
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX117_SetDigitalGainReg
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
static void IMX117_SetDigitalGainReg(UINT8 DGC)
{
    UINT8 TxData[1];
    UINT8 RxData[1];

    TxData[0] = DGC;
    IMX117_RegRW(IMX117_DGAIN_REG, TxData, RxData, 1);
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX117_SetShutterReg
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
static UINT32 IMX117_IntegrationPeriodInXVS = 1;  /* current intergration period (unit: XVS) */
static void IMX117_SetShutterReg(UINT32 NumXhsEshrSpeed)
{
    UINT32 NumXhsPerXvs = IMX117Ctrl.FrameTime.NumXhsPerV / IMX117Ctrl.FrameTime.NumXvsPerV;
    IMX117_READOUT_MODE_e ReadoutMode =
        IMX117ModeInfoList[IMX117Ctrl.Status.ModeInfo.Mode.Bits.Mode].ReadoutMode;
    UINT32 SVR; /* specifies the exposure end vertical period */
    UINT32 SHR; /* specifies the exposure start horizontal period */

    UINT8  TxData[2];
    UINT8  RxData[2];

    SVR = IMX117_IntegrationPeriodInXVS - 1;
    if (ReadoutMode != IMX117_TYPE_2_3_MODE_6) {
        SHR = (SVR + 1) * NumXhsPerXvs - NumXhsEshrSpeed;
    } else {
        SHR = ((SVR + 1) * NumXhsPerXvs - NumXhsEshrSpeed) / 4;
    }

    
    /* The length of sensor SHR register is 16 bits,
       thus SPL should be applied when SHR is larger than 16 bits. */
    if (SHR > 65535) {
        UINT32 SPL = 0;
        SPL = SHR / NumXhsPerXvs;
        SHR = SHR % NumXhsPerXvs;

        TxData[0] = SPL & 0x00ff;
        TxData[1] = (SPL >> 8) & 0x00ff;
        IMX117_RegRW(IMX117_SPL_LSB_REG, TxData, RxData, 2);
    }

    TxData[0] = SHR & 0x00ff;
    TxData[1] = (SHR >> 8) & 0x00ff;
    IMX117_RegRW(IMX117_SHR_LSB_REG, TxData, RxData, 2);

    IMX117Ctrl.CurrentShrCtrlSHR = SHR;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX117_SetSlowShutterReg
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
static void IMX117_SetSlowShutterReg(UINT32 IntegrationPeriodInFrame)
{
    IMX117_FRAME_TIMING_s *pFrameTime = &IMX117Ctrl.FrameTime;
    UINT32 SVR = 0;
    UINT8  TxData[2];
    UINT8  RxData[2];

    /* 1 frame might includes more than 1 XVS */
    IMX117_IntegrationPeriodInXVS = IntegrationPeriodInFrame * pFrameTime->NumXvsPerV;

    SVR = IMX117_IntegrationPeriodInXVS - 1;

    TxData[0] = SVR & 0x00ff;
    TxData[1] = (SVR >> 8) & 0x00ff;

    IMX117_RegRW(IMX117_SVR_LSB_REG, TxData, RxData, 2);
    IMX117Ctrl.CurrentShrCtrlSVR = SVR;
    IMX117Ctrl.Status.ModeInfo.FrameTime.FrameRate.NumUnitsInTick =
        IMX117ModeInfoList[IMX117Ctrl.Status.ModeInfo.Mode.Bits.Mode].FrameTiming.FrameRate.NumUnitsInTick * IntegrationPeriodInFrame;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX117_SetStandbyOn
 *
 *  @Description:: Configure sensor into standby mode
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static void IMX117_SetStandbyOn(void)
{
    UINT8 RegData = 0x04 | 0x02;
    IMX117_RegRW(IMX117_OP_REG, &RegData, NULL, 1);
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX117_SetStandbyOff
 *
 *  @Description:: Configure sensor to leave standby mode
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static void IMX117_SetStandbyOff(void)
{
    UINT8 RegData = 0x04 | 0x00;
    IMX117_RegRW(IMX117_OP_REG, &RegData, NULL, 1);
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX117_ChangeReadoutMode
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
static int IMX117_ChangeReadoutMode(IMX117_READOUT_MODE_e ReadoutMode)
{
    UINT8 TxRegData[8];
    int i;

    for (i = 0; i < IMX117_NUM_READOUT_MODE_REG; i++) {
        if (IMX117RegTable[i].Data[ReadoutMode] != NA) {
            IMX117_RegRW(IMX117RegTable[i].Addr, &IMX117RegTable[i].Data[ReadoutMode], NULL, 1);
        }
    }

    /* Wait for Valid Data Output */
    AmbaKAL_TaskSleep(32);

    /* address 0x0001: CLPSQRST */
    TxRegData[0] = 0x11;
    IMX117_RegRW(IMX117_DCKRST_CLPSQRST_REG, TxRegData, NULL, 1);

    /* reset gain/shutter ctrl information */
    IMX117Ctrl.CurrentShrCtrlSHR = 0xffffffff;
    IMX117Ctrl.CurrentShrCtrlSVR = 0xffffffff;
    IMX117Ctrl.CurrentAgc = 0xffffffff;
    IMX117Ctrl.CurrentDgc = 0xffffffff;

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX117_Init
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
static int IMX117_Init(AMBA_DSP_CHANNEL_ID_u Chan)
{
    const IMX117_FRAME_TIMING_s *pFrameTime = &IMX117ModeInfoList[0].FrameTiming;
    AmbaPLL_SetSensorClk(pFrameTime->InputClk);             /* The default sensor input clock frequency */
    AmbaPLL_SetSensorClkDir(AMBA_PLL_SENSOR_CLK_OUTPUT);    /* Config clock output to sensor (or sensor SPI won't work) */

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX117_Enable
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
static int IMX117_Enable(AMBA_DSP_CHANNEL_ID_u Chan)
{

    if (!IMX117Ctrl.Status.DevStat.Bits.Sensor0Power) {
#if 0
        /* Turn On the Sensor Power if applicable */
        AmbaUserGPIO_SensorPowerCtrl(1);    /* Turn On the Sensor Power */
#endif
        /* the Sensor Power is ON, Sensor is at Standby mode */
        IMX117Ctrl.Status.DevStat.Bits.Sensor0Power =
            IMX117Ctrl.Status.DevStat.Bits.Sensor0Standby = 1;
    }

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX117_Disable
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
static int IMX117_Disable(AMBA_DSP_CHANNEL_ID_u Chan)
{
    if (IMX117Ctrl.Status.DevStat.Bits.Sensor0Power == 0)
        return OK;

    IMX117_SetStandbyOn();
    IMX117Ctrl.Status.DevStat.Bits.Sensor0Standby = 1;  /* Sensor is at Standby mode */

#if 0
    /* Turn Off the Sensor Power if applicable */
    AmbaUserGPIO_SensorPowerCtrl(0);    /* Turn Off the Sensor Power */
    IMX117Ctrl.Status.DevStat.Bits.Sensor0Power = 0;
#endif

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX117_GetStatus
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
static int IMX117_GetStatus(AMBA_DSP_CHANNEL_ID_u Chan, AMBA_SENSOR_STATUS_INFO_s *pStatus)
{
    if (pStatus == NULL || IMX117Ctrl.Status.DevStat.Bits.Sensor0Power == 0)
        return NG;

    IMX117Ctrl.Status.TimeToAllPelExposed = 0;  /* TODO: for flash light or mechanical shutter control */

    memcpy(pStatus, &IMX117Ctrl.Status, sizeof(AMBA_SENSOR_STATUS_INFO_s));

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX117_GetModeInfo
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
static int IMX117_GetModeInfo(AMBA_DSP_CHANNEL_ID_u Chan, AMBA_SENSOR_MODE_ID_u Mode, AMBA_SENSOR_MODE_INFO_s *pModeInfo)
{

    if (Mode.Bits.Mode == AMBA_SENSOR_CURRENT_MODE)
        Mode.Bits.Mode = IMX117Ctrl.Status.ModeInfo.Mode.Bits.Mode;

    if (Mode.Bits.Mode >= AMBA_SENSOR_IMX117_NUM_MODE || pModeInfo == NULL)
        return NG;

    IMX117_PrepareModeInfo(Mode, pModeInfo);

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX117_GetDeviceInfo
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
static int IMX117_GetDeviceInfo(AMBA_DSP_CHANNEL_ID_u Chan, AMBA_SENSOR_DEVICE_INFO_s *pDeviceInfo)
{
    if (pDeviceInfo == NULL)
        return NG;

    memcpy(pDeviceInfo, &IMX117DeviceInfo, sizeof(AMBA_SENSOR_DEVICE_INFO_s));

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX117_GetCurrentGainFactor
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
static int IMX117_GetCurrentGainFactor(AMBA_DSP_CHANNEL_ID_u Chan, float *pGainFactor)
{
    *pGainFactor = 2048. / (2048 - IMX117Ctrl.CurrentAgc) * (1 << IMX117Ctrl.CurrentDgc);

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX117_GetCurrentShutterSpeed
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
static int IMX117_GetCurrentShutterSpeed(AMBA_DSP_CHANNEL_ID_u Chan, float *pExposureTime)
{
    *pExposureTime = (IMX117Ctrl.Status.ModeInfo.RowTime / IMX117Ctrl.FrameTime.NumXhsPerH) *
                     ((IMX117Ctrl.CurrentShrCtrlSVR + 1) * (IMX117Ctrl.FrameTime.NumXhsPerV / IMX117Ctrl.FrameTime.NumXvsPerV) - IMX117Ctrl.CurrentShrCtrlSHR);

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX117_ConvertGainFactor
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
static int IMX117_ConvertGainFactor(AMBA_DSP_CHANNEL_ID_u Chan, float DesiredFactor, UINT32 *pActualFactor, UINT32 *pAnalogGainCtrl, UINT32 *pDigitalGainCtrl)
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

        DigitalGain = DesiredFactor / 22.5;

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
 *  @RoutineName:: IMX117_ConvertShutterSpeed
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
static int IMX117_ConvertShutterSpeed(AMBA_DSP_CHANNEL_ID_u Chan, float ExposureTime, UINT32 *pShutterCtrl)
{
    AMBA_SENSOR_MODE_INFO_s *pModeInfo = &IMX117Ctrl.Status.ModeInfo;
    const IMX117_FRAME_TIMING_s *pFrameTiming = &IMX117ModeInfoList[pModeInfo->Mode.Bits.Mode].FrameTiming;
    IMX117_READOUT_MODE_e ReadoutMode = IMX117ModeInfoList[pModeInfo->Mode.Bits.Mode].ReadoutMode;
    UINT32 ExpXhsNum    = 0;
    UINT32 MinExpXhsNum = 0;
    UINT32 MaxExpXhsNum = 0;

    if (pShutterCtrl == NULL)
        return NG;

    ExpXhsNum = (float)(ExposureTime / pModeInfo->RowTime);

    if (IMX117Ctrl.Status.ElecShutterMode == AMBA_SENSOR_ESHUTTER_TYPE_GLOBAL_RESET) {
        MinExpXhsNum = 130;
        MaxExpXhsNum = ((ExpXhsNum -1) / pModeInfo->NumExposureStepPerFrame + 1) * pModeInfo->NumExposureStepPerFrame;
    } else {
        switch (ReadoutMode) {
        case IMX117_TYPE_2_3_MODE_0:
        case IMX117_TYPE_2_3_MODE_0A:
        case IMX117_TYPE_2_5_MODE_0:
        case IMX117_TYPE_2_5_MODE_1:
        case IMX117_TYPE_4_4_MODE_1:
        case IMX117_CUSTOM_MODE:
            MinExpXhsNum = 4;
            MaxExpXhsNum = ((ExpXhsNum -1) / pModeInfo->NumExposureStepPerFrame + 1) * pModeInfo->NumExposureStepPerFrame - 8;
            break;

        case IMX117_TYPE_2_3_MODE_2A:
            MinExpXhsNum = 4;
            MaxExpXhsNum = ((ExpXhsNum -1) / pModeInfo->NumExposureStepPerFrame + 1) * pModeInfo->NumExposureStepPerFrame - 10;
            break;

        case IMX117_TYPE_2_3_MODE_3:
            MinExpXhsNum = 8;
            MaxExpXhsNum = ((ExpXhsNum -1) / pModeInfo->NumExposureStepPerFrame + 1) * pModeInfo->NumExposureStepPerFrame - 15;
            break;

        case IMX117_TYPE_2_3_MODE_4:
        case IMX117_TYPE_2_3_MODE_4A:
        case IMX117_TYPE_2_5_MODE_4:
        case IMX117_TYPE_2_3_MODE_5:
        case IMX117_TYPE_2_3_MODE_7:
            MinExpXhsNum = 2;
            MaxExpXhsNum = ((ExpXhsNum -1) / pModeInfo->NumExposureStepPerFrame + 1) * pModeInfo->NumExposureStepPerFrame - 4;
            break;
        case IMX117_TYPE_2_3_MODE_6:
            /* integration time = ([Number of XHS per XVS] * (SVR + 1) - (SHR * 4) ) * [XHS period] */
            MinExpXhsNum = 8;
            MaxExpXhsNum = ((ExpXhsNum -1) / pModeInfo->NumExposureStepPerFrame + 1) * pModeInfo->NumExposureStepPerFrame - 4 * 4;
            break;
        default:
            MinExpXhsNum = 4;
            MaxExpXhsNum = ((ExpXhsNum -1) / pModeInfo->NumExposureStepPerFrame + 1) * pModeInfo->NumExposureStepPerFrame - 8;
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
 *  @RoutineName:: IMX117_SetAnalogGainCtrl
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
static int IMX117_SetAnalogGainCtrl(AMBA_DSP_CHANNEL_ID_u Chan, UINT32 AnalogGainCtrl)
{
    IMX117_SetAnalogGainReg(AnalogGainCtrl);

    IMX117Ctrl.CurrentAgc = AnalogGainCtrl;

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX117_SetDigitalGainCtrl
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
static int IMX117_SetDigitalGainCtrl(AMBA_DSP_CHANNEL_ID_u Chan, UINT32 DigitalGainCtrl)
{
    IMX117_SetDigitalGainReg(DigitalGainCtrl);

    IMX117Ctrl.CurrentDgc = DigitalGainCtrl;

    return OK;
}


/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX117_SetShutterCtrl
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
static int IMX117_SetShutterCtrl(AMBA_DSP_CHANNEL_ID_u Chan, UINT32 ShutterCtrl)
{
    IMX117_SetShutterReg(ShutterCtrl * IMX117Ctrl.FrameTime.NumXhsPerH); //Convert Line to XHS

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX117_SetSlowShutterCtrl
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
static int IMX117_SetSlowShutterCtrl(AMBA_DSP_CHANNEL_ID_u Chan, UINT32 SlowShutterCtrl)
{
    IMX117_SetSlowShutterReg(SlowShutterCtrl);
    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX117_Config
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
static int IMX117_Config(AMBA_DSP_CHANNEL_ID_u Chan, AMBA_SENSOR_MODE_ID_u Mode, AMBA_SENSOR_ESHUTTER_TYPE_e ElecShutterMode)
{
    AMBA_SENSOR_MODE_INFO_s *pModeInfo;
    UINT16 SensorMode = Mode.Bits.Mode;
    IMX117_READOUT_MODE_e ReadoutMode = IMX117ModeInfoList[SensorMode].ReadoutMode;

    if (SensorMode >= AMBA_SENSOR_IMX117_NUM_MODE)
        return NG;

    if ((IMX117Ctrl.Status.ModeInfo.Mode.Bits.Mode == SensorMode) &&
        (IMX117Ctrl.Status.ModeInfo.Mode.Bits.VerticalFlip != Mode.Bits.VerticalFlip) &&
        (IMX117Ctrl.Status.ElecShutterMode == ElecShutterMode) &&
        (IMX117Ctrl.Status.DevStat.Bits.Sensor0Power) &&
        (!IMX117Ctrl.Status.DevStat.Bits.Sensor0Standby)) {
        /* run-time flip sensor readout direction */
        UINT8 RxData = 0;
        UINT8 TxData = Mode.Bits.VerticalFlip;
        IMX117_RegRW(0x001a, &TxData, &RxData, 1);

        /* update status */
        pModeInfo = &IMX117Ctrl.Status.ModeInfo;
        IMX117_PrepareModeInfo(Mode, pModeInfo);
    } else {
        UINT8 RxData = 0;
        UINT8 TxData = 0;
        IMX117_FRAME_TIMING_s *pFrameTime = &IMX117Ctrl.FrameTime;

        if (!IMX117Ctrl.Status.DevStat.Bits.Sensor0Power || IMX117Ctrl.Status.DevStat.Bits.Sensor0Standby) {
            /* if the Sensor Power if OFF, or Sensor is at Standby mode, then enable the Sensor first */
            IMX117_Enable(Chan);
        }

        /* update status */
        IMX117Ctrl.Status.ElecShutterMode = ElecShutterMode;
        pModeInfo = &IMX117Ctrl.Status.ModeInfo;
        IMX117_PrepareModeInfo(Mode, pModeInfo);
        memcpy(pFrameTime, &IMX117ModeInfoList[SensorMode].FrameTiming, sizeof(IMX117_FRAME_TIMING_s));

        /* force to do standby cancel seq for
           1. activated from standby mode, 2. switch from normal mode to normal mode */
        AmbaPrint("[IMX117] hw reset");

        /* reset master sync*/
        AmbaVIN_ResetHvSyncOutput((AMBA_VIN_CHANNEL_e) Chan.Bits.VinID);
        /* Make VIN not to capture bad frames during readout mode transition */
        AmbaVIN_Reset((AMBA_VIN_CHANNEL_e) Chan.Bits.VinID, AMBA_VIN_LVDS, NULL);

        /* do HW reset */
        /* set pll */
        AmbaPLL_SetSensorClk(pModeInfo->FrameTime.InputClk);
        AmbaPLL_SetSoVinClk(pModeInfo->FrameTime.InputClk);
        AmbaKAL_TaskSleep(1);

        IMX117_HardwareReset();
        AmbaKAL_TaskSleep(1);

        IMX117_SetStandbyOn();
        AmbaKAL_TaskSleep(1);

        /* V1 period */
        IMX117_SetStandbyOff();
        AmbaKAL_TaskSleep(16);

        /* V2 period */
        TxData = 0x11;
        IMX117_RegRW(IMX117_DCKRST_CLPSQRST_REG, &TxData, NULL, 1);


        /* write registers of mode change to sensor */
        IMX117_ChangeReadoutMode(ReadoutMode);

        RxData=0;
        TxData = Mode.Bits.VerticalFlip;
        /* config sensor readout direction */
        IMX117_RegRW(0x001a, &TxData, &RxData, 1);

        IMX117_SetSlowShutterReg(1);
        IMX117_SetShutterReg(pModeInfo->NumExposureStepPerFrame >> 1);
        IMX117_SetAnalogGainCtrl(Chan, 0);
        IMX117_SetDigitalGainCtrl(Chan, 0);
        AmbaKAL_TaskSleep(16);

        /* config vin */
        IMX117_ConfigVin(pModeInfo, pFrameTime);

        /* reset vin for A12*/
        AmbaVIN_Reset((AMBA_VIN_CHANNEL_e) Chan.Bits.VinID, AMBA_VIN_LVDS, NULL);
        IMX117Ctrl.Status.DevStat.Bits.Sensor0Standby = 0;

    }
    return OK;
}

/*-----------------------------------------------------------------------------------------------*/
AMBA_SENSOR_OBJ_s AmbaSensor_IMX117Obj = {
    .Init                   = IMX117_Init,
    .Enable                 = IMX117_Enable,
    .Disable                = IMX117_Disable,
    .Config                 = IMX117_Config,
    .GetStatus              = IMX117_GetStatus,
    .GetModeInfo            = IMX117_GetModeInfo,
    .GetDeviceInfo          = IMX117_GetDeviceInfo,
    .GetCurrentGainFactor   = IMX117_GetCurrentGainFactor,
    .GetCurrentShutterSpeed = IMX117_GetCurrentShutterSpeed,

    .ConvertGainFactor      = IMX117_ConvertGainFactor,
    .ConvertShutterSpeed    = IMX117_ConvertShutterSpeed,

    .SetAnalogGainCtrl      = IMX117_SetAnalogGainCtrl,
    .SetDigitalGainCtrl     = IMX117_SetDigitalGainCtrl,
    .SetShutterCtrl         = IMX117_SetShutterCtrl,
    .SetSlowShutterCtrl     = IMX117_SetSlowShutterCtrl,

    .RegisterRead           = IMX117_RegisterRead,
    .RegisterWrite          = IMX117_RegisterWrite,
};
