/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaSensor_MN34120.c
 *
 *  @Copyright      :: Copyright (C) 2013 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Control APIs of SONY MN34120 CMOS sensor with LVDS interface
 *
\*-------------------------------------------------------------------------------------------------------------------*/

#include <AmbaSSP.h>
#include <math.h>

#include "AmbaDSP.h"
#include "AmbaVIN.h"
#include "AmbaDSP_VIN.h"

#include "AmbaSPI.h"
#include "AmbaGPIO.h"
#include "AmbaPLL.h"
#include "AmbaSensor.h"
#include "AmbaSensor_MN34120.h"

#include "AmbaPrintk.h"

/*-----------------------------------------------------------------------------------------------*\
 * MN34120 sensor runtime info
\*-----------------------------------------------------------------------------------------------*/
MN34120_CTRL_s MN34120Ctrl = {0};
UINT8 MN34120SpiBuf[2][128];
UINT8 MN34120LeavePSMode = 0;

/*-----------------------------------------------------------------------------------------------*\
 * Configurations for Amba. VIN side
\*-----------------------------------------------------------------------------------------------*/
static AMBA_DSP_VIN_SLVS_PIN_PAIR_e MN34120LvdsLaneMux[][AMBA_DSP_NUM_VIN_SLVS_DATA_LANE] = {
    [MN34120_LVDS_8CH]   = {
        AMBA_DSP_VIN_PIN_SD_LVDS_0,
        AMBA_DSP_VIN_PIN_SD_LVDS_1,
        AMBA_DSP_VIN_PIN_SD_LVDS_2,
        AMBA_DSP_VIN_PIN_SD_LVDS_3,
        AMBA_DSP_VIN_PIN_SD_LVDS_4,
        AMBA_DSP_VIN_PIN_SD_LVDS_5,
        AMBA_DSP_VIN_PIN_SD_LVDS_6,
        AMBA_DSP_VIN_PIN_SD_LVDS_7
    },

    [MN34120_LVDS_6CH]   = {
        AMBA_DSP_VIN_PIN_SD_LVDS_0,
        AMBA_DSP_VIN_PIN_SD_LVDS_1,
        AMBA_DSP_VIN_PIN_SD_LVDS_2,
        AMBA_DSP_VIN_PIN_SD_LVDS_3,
        AMBA_DSP_VIN_PIN_SD_LVDS_4,
        AMBA_DSP_VIN_PIN_SD_LVDS_5,
    },

    [MN34120_LVDS_4CH]   = {
        AMBA_DSP_VIN_PIN_SD_LVDS_0,
        AMBA_DSP_VIN_PIN_SD_LVDS_1,
        AMBA_DSP_VIN_PIN_SD_LVDS_2,
        AMBA_DSP_VIN_PIN_SD_LVDS_3,
    },

    [MN34120_LVDS_1CH]   = {
        AMBA_DSP_VIN_PIN_SD_LVDS_0
    },
};

static AMBA_DSP_VIN_SLVS_CONFIG_s MN34120VinConfig = {
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

static AMBA_DSP_VIN_MASTER_SYNC_CONFIG_s MN34120MasterSyncConfig = {
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
static AMBA_SPI_CONFIG_s MN34120SpiConfig = {
    .SlaveID       = AMBA_SENSOR_SPI_SLAVE_ID,                                 /* Slave ID */
    .ProtocolMode  = AMBA_SPI_CPOL_HIGH_CPHA_HIGH,      /* SPI Protocol mode */
    .CsPolarity    = AMBA_SPI_CHIP_SELECT_ACTIVE_LOW,   /* Slave select polarity */
    .DataFrameSize = 8,                                 /* Data Frame Size in Bit */
    .BaudRate      = 10000000                           /* Transfer BaudRate in Hz */
};

/*-----------------------------------------------------------------------------------------------*\
 * Reverse bit order in a byte for MN34120 SPI communication
\*-----------------------------------------------------------------------------------------------*/
static inline UINT8 MN34120_SpiReverseBitOrder(UINT16 Data)
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
 *  @RoutineName:: MN34120_PrepareModeInfo
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
static void MN34120_PrepareModeInfo(AMBA_SENSOR_MODE_ID_u Mode, AMBA_SENSOR_MODE_INFO_s *pModeInfo)
{
    UINT16 SensorMode                             = Mode.Bits.Mode;
    MN34120_READOUT_MODE_e           ReadoutMode   = MN34120ModeInfoList[SensorMode].ReadoutMode;
    const MN34120_FRAME_TIMING_s     *pFrameTiming = &MN34120ModeInfoList[SensorMode].FrameTiming;
    const AMBA_SENSOR_OUTPUT_INFO_s *pOutputInfo  = &MN34120OutputInfo[ReadoutMode];
    const AMBA_SENSOR_INPUT_INFO_s  *pInputInfo;

    pInputInfo = &MN34120InputInfo[ReadoutMode];

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
 *  @RoutineName:: MN34120_ConfigVin
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
static void MN34120_ConfigVin(AMBA_SENSOR_MODE_INFO_s *pModeInfo, MN34120_FRAME_TIMING_s *pFrameTime)
{
    AMBA_SENSOR_OUTPUT_INFO_s  *pImgInfo = &pModeInfo->OutputInfo;
    const MN34120_FRAME_TIMING_s *pMN34120FrameTiming = &MN34120ModeInfoList[pModeInfo->Mode.Bits.Mode].FrameTiming;
    AMBA_DSP_VIN_SLVS_CONFIG_s *pVinCfg = &MN34120VinConfig;
    AMBA_DSP_VIN_MASTER_SYNC_CONFIG_s *pMasterSyncCfg = &MN34120MasterSyncConfig;

    AmbaDSP_VinPhySetSLVS(AMBA_VIN_CHANNEL0);

    memcpy(&pVinCfg->Info.FrameRate, &pFrameTime->FrameRate, sizeof(AMBA_DSP_FRAME_RATE_s));
    memcpy(&pVinCfg->Info.DspPhaseShift, &pImgInfo->DspPhaseShift, sizeof(AMBA_DSP_PHASE_SHIFT_CTRL_s));
    pVinCfg->Info.BayerPattern = (AMBA_DSP_SENSOR_PATTERN_e) pImgInfo->CfaPattern;
    pVinCfg->Info.NumDataBits = pImgInfo->NumDataBits;

    pVinCfg->NumActiveLanes = pImgInfo->NumDataLanes;
    switch (pVinCfg->NumActiveLanes) {
    case 8:
        pVinCfg->pLaneMapping = MN34120LvdsLaneMux[MN34120_LVDS_8CH];
        break;
    case 6:
        pVinCfg->pLaneMapping = MN34120LvdsLaneMux[MN34120_LVDS_6CH];
        break;
    case 4:
        pVinCfg->pLaneMapping = MN34120LvdsLaneMux[MN34120_LVDS_4CH];
        break;
    case 1:
    default:
        pVinCfg->pLaneMapping = MN34120LvdsLaneMux[MN34120_LVDS_1CH];
        break;
    }
    pVinCfg->RxHvSyncCtrl.NumActivePixels = pImgInfo->OutputWidth;
    pVinCfg->RxHvSyncCtrl.NumActiveLines  = pImgInfo->OutputHeight;
    pVinCfg->RxHvSyncCtrl.NumTotalPixels  = pModeInfo->LineLengthPck - 1;
    pVinCfg->RxHvSyncCtrl.NumTotalLines   = pModeInfo->FrameLengthLines;

    //AmbaPrintColor(RED,"OutputWidth:%d, OutputHeight:%d NumDataBits:%d",pImgInfo->OutputWidth, pImgInfo->OutputHeight, pImgInfo->NumDataBits);

    AmbaDSP_VinConfigSLVS(AMBA_VIN_CHANNEL0, pVinCfg);

    pMasterSyncCfg->HSync.Period = pMN34120FrameTiming->NumTickPerXhs;
    pMasterSyncCfg->HSync.PulseWidth = 8;
    pMasterSyncCfg->VSync.Period = pMN34120FrameTiming->NumXhsPerV / pMN34120FrameTiming->NumXvsPerV;
    pMasterSyncCfg->VSync.PulseWidth = 8;

    AmbaDSP_VinConfigMasterSync(AMBA_VIN_CHANNEL0, pMasterSyncCfg);
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MN34120_HardwareReset
 *
 *  @Description:: Reset MN34120 Image Sensor Device
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static inline void MN34120_HardwareReset(void)
{
    AmbaUserGPIO_SensorResetCtrl(0);
    AmbaKAL_TaskSleep(1);               /* XCLR Low level pulse width >= 100ns */
    AmbaUserGPIO_SensorResetCtrl(1);
    AmbaKAL_TaskSleep(1);
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MN34120_RegRW
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
static int MN34120_RegRW(UINT16 Addr, UINT16 *pTxData, UINT16 *pRxData, UINT8 Size)
{

    UINT32  i = 0;
    UINT8   *pDataBuf;

    if ((Size * 2 + 2) > sizeof(MN34120SpiBuf[0]))
        return NG;

    pDataBuf = &MN34120SpiBuf[0][0];
    *pDataBuf++ = MN34120_SpiReverseBitOrder(Addr & 0xff);  /* Register Address (low byte) */
    *pDataBuf++ = MN34120_SpiReverseBitOrder(Addr >> 8);    /* Register Address (high byte) */

    for (i = 0; i < Size; i++) {
        *pDataBuf++ = MN34120_SpiReverseBitOrder(pTxData[i] & 0xff);
        *pDataBuf++ = MN34120_SpiReverseBitOrder(pTxData[i] >> 8);
    }

    AmbaSPI_Transfer(AMBA_SPI_MASTER, &MN34120SpiConfig, Size * 2 + 2, MN34120SpiBuf[0], MN34120SpiBuf[1], 1000);
#if 0 //for debug
    AmbaSPI_Transfer(AMBA_SPI_MASTER, &MN34120SpiConfig, Size * 2 + 2, MN34120SpiBuf[0], MN34120SpiBuf[1], 1000);

    for (i = 0; i < Size; i++) {
        AmbaPrintColor(RED, "Wr %04Xh %02Xh, Rd %04Xh",
                  Addr + i, pTxData[i],
                  MN34120_SpiReverseBitOrder(MN34120SpiBuf[1][2 + 2 * i]) +
                  (MN34120_SpiReverseBitOrder(MN34120SpiBuf[1][2 + 2 * i + 1]) << 8));
    }
#endif
    return OK;

}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MN34120_RegisterWrite
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
static int MN34120_RegisterWrite(AMBA_DSP_CHANNEL_ID_u Chan, UINT16 Addr, UINT16 Data)
{
    UINT16 RxBuffer;
    MN34120_RegRW(Addr, &Data, &RxBuffer, 1);

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MN34120_SetAnalogGainReg
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
static void MN34120_SetAnalogGainReg(UINT16 AGC)
{
   AGC |= 0x6000;
   MN34120_RegRW(MN34120_AGAIN, &AGC, NULL, 1);
   //AmbaPrint("[MN34230]Set Analog Gain");
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MN34120_SetDigitalGainReg
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
static void MN34120_SetDigitalGainReg(UINT16 DGC)
{
    MN34120_RegRW(MN34120_DGAIN, &DGC, NULL, 1);
    //AmbaPrint("[MN34230]Set Digital Gain");
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MN34120_SetShutterReg
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
static UINT32 MN34120_IntegrationPeriodInXVS = 1;  /* current intergration period (unit: XVS) */
static void MN34120_SetShutterReg(UINT32 NumXhsEshrSpeed)
{
    AMBA_SENSOR_MODE_INFO_s *pModeInfo = &MN34120Ctrl.Status.ModeInfo;
    UINT32 NumXhsPerFrm = pModeInfo->NumExposureStepPerFrame;
    UINT32 R_LONG_EXPOSURE = MN34120_IntegrationPeriodInXVS - 1; /* specifies the exposure end vertical period */
    UINT16 SHT = 0; /* specifies the exposure start horizontal period */

    SHT = (UINT16) ((R_LONG_EXPOSURE + 1) * NumXhsPerFrm - NumXhsEshrSpeed);

    //AmbaPrint("[MN34120_SHT] NumXhsEshrSpeed:%d SHT:%d", NumXhsEshrSpeed, SHT);
    MN34120_RegRW(MN34120_SHTPOS, &SHT, NULL, 1);

    MN34120Ctrl.CurrentShrCtrlSHR = SHT;
    //AmbaPrint("[MN34230]Set Shutter");
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MN34120_SetSlowShutterReg
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
static void MN34120_SetSlowShutterReg(UINT32 IntegrationPeriodInFrame)
{
    UINT16 R_LONG_EXPOSURE = (UINT16) (IntegrationPeriodInFrame - 1);

    MN34120_IntegrationPeriodInXVS = IntegrationPeriodInFrame;

    MN34120Ctrl.CurrentShrCtrlSVR = R_LONG_EXPOSURE;
    MN34120Ctrl.Status.ModeInfo.FrameTime.FrameRate.NumUnitsInTick =
        MN34120ModeInfoList[MN34120Ctrl.Status.ModeInfo.Mode.Bits.Mode].FrameTiming.FrameRate.NumUnitsInTick * IntegrationPeriodInFrame;

    MN34120_RegRW(MN34120_LONG_EXPOSURE, &R_LONG_EXPOSURE, NULL, 1);
    //AmbaPrint("[MN34230]Set SlowShutter:%d", IntegrationPeriodInFrame);
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MN34120_SetStandbyOn
 *
 *  @Description:: Configure sensor into standby mode
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static void MN34120_SetStandbyOn(void)
{
    ////** Config sensor to consume minimal power */
    ///** Enter power saving mode 3*/
    UINT16 Reg      = 0x0000;


    // 1. TG reset / CKG reset
    Reg = 0x0063;
    MN34120_RegRW(MN34120_RESET, &Reg, NULL, 1);
    //2. stop VD/HD and wait few H cycles
    AmbaKAL_TaskSleep(1);
    AmbaUserGPIO_SensorResetCtrl(0);

    // 3. stop MCLK input and write power saving register
    Reg = 0x0101;
    MN34120_RegRW(MN34120_POWER_SAVING, &Reg, NULL, 1);

    //AmbaPrint("[MN34120]Standby On");
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MN34120_SetStandbyOff
 *
 *  @Description:: Configure sensor to leave standby mode
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static void MN34120_SetStandbyOff(void)
{
    ///** Restore sensor from consuming minimal power */
    /** Leave power saving mode 3*/

    UINT16 Reg      = 0x0;

    MN34120_HardwareReset();
    Reg = 0x0101;
    MN34120_RegRW(MN34120_POWER_SAVING, &Reg, NULL, 1);
    AmbaKAL_TaskSleep(1);

    MN34120LeavePSMode = 1;

    AmbaPrint("[MN34120]Standby Off");

}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MN34120_ChangeReadoutMode
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
static int MN34120_ChangeReadoutMode(MN34120_READOUT_MODE_e ReadoutMode)
{
    int i = 0;

    for (i = 0; i < MN34120_NUM_READOUT_MODE_REG; i++) {
        if (MN34120RegTable[i].Data[ReadoutMode] != NA) {
            MN34120_RegRW(MN34120RegTable[i].Addr, &MN34120RegTable[i].Data[ReadoutMode], NULL, 1);
        }
        if ((MN34120RegTable[i].Addr == 0x0008) && (MN34120RegTable[i].Data[ReadoutMode] == 0x0298)) {
            AmbaKAL_TaskSleep(30);
            AmbaPrint(" ================ dly 30ms ==================== ");
        }
        if (MN34120RegTable[i].Addr == 0x0011)
            AmbaKAL_TaskSleep(1);
        if (MN34120RegTable[i].Addr == 0x0000)
            AmbaKAL_TaskSleep(1);
    }

    /* reset gain/shutter ctrl information */
    MN34120Ctrl.CurrentShrCtrlSHR = 0xffffffff;
    MN34120Ctrl.CurrentShrCtrlSVR = 0xffffffff;
    MN34120Ctrl.GainFactor= 0xffffffff;

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MN34120_ConfigPll
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
static int MN34120_ConfigPll(MN34120_READOUT_MODE_e ReadoutMode)
{
    int i = 0 ;
    int AddrIndex[] = {2,6,7};
    int RegIndex = 0;
    for (i = 0; i < sizeof(AddrIndex)/sizeof(int); i++ ) {
        RegIndex = AddrIndex[i];
        if(MN34120RegTable[RegIndex].Data[ReadoutMode] != NA)
            MN34120_RegRW(MN34120RegTable[RegIndex].Addr, &MN34120RegTable[RegIndex].Data[ReadoutMode], NULL, 1);
        if(MN34120RegTable[RegIndex].Addr==MN34120_RESET)
            AmbaKAL_TaskSleep(1);
    }

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MN34120_ConfigSysReg
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
static int MN34120_ConfigSysReg(MN34120_READOUT_MODE_e ReadoutMode)
{
    int i = 0 ;
    int AddrIndex[] =
        {20 ,22 ,24 ,25 ,26 ,27 ,30 ,31 ,
         36 ,37 ,39 ,46 ,47 ,49 ,50 ,52 ,
         53 ,54 ,66 ,67 ,68 ,69 ,70 ,73 ,
         77 ,78 , // reset MN34120PA_R_AUTOSTILL_ENABLE
         81 ,93 ,100,101,104};
    int RegIndex = 0;
    for (i = 0; i < sizeof(AddrIndex)/sizeof(int); i++ ) {
        RegIndex = AddrIndex[i];
        if(MN34120RegTable[RegIndex].Data[ReadoutMode] != NA)
            MN34120_RegRW(MN34120RegTable[RegIndex].Addr, &MN34120RegTable[RegIndex].Data[ReadoutMode], NULL, 1);
    }

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MN34120_ConfigTgRstReg
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
static int MN34120_ConfigTgRstReg(MN34120_READOUT_MODE_e ReadoutMode)
{
    int i = 0 ;

    for (i = MN34120_TG_REG; i < MN34120_TG_REG + MN34120_NUM_TG_REG; i++) {
        if(MN34120RegTable[i].Data[ReadoutMode] != NA)
            MN34120_RegRW(MN34120RegTable[i].Addr, &MN34120RegTable[i].Data[ReadoutMode], NULL, 1);
    }

    /* reset gain/shutter ctrl information */
    MN34120Ctrl.CurrentShrCtrlSHR = 0xffffffff;
    MN34120Ctrl.CurrentShrCtrlSVR = 0xffffffff;
    MN34120Ctrl.GainFactor= 0xffffffff;

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MN34120_Init
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
static int MN34120_Init(AMBA_DSP_CHANNEL_ID_u Chan)
{
    const MN34120_FRAME_TIMING_s *pFrameTime = &MN34120ModeInfoList[0].FrameTiming;
    AmbaPLL_SetSensorClk(pFrameTime->InputClk);             /* The default sensor input clock frequency */
    AmbaPLL_SetSensorClkDir(AMBA_PLL_SENSOR_CLK_OUTPUT);    /* Config clock output to sensor (or sensor SPI won't work) */

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MN34120_Enable
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
static int MN34120_Enable(AMBA_DSP_CHANNEL_ID_u Chan)
{

    if (!MN34120Ctrl.Status.DevStat.Bits.Sensor0Power) {
#if 0
        /* Turn On the Sensor Power if applicable */
        AmbaUserGPIO_SensorPowerCtrl(1);    /* Turn On the Sensor Power */
#endif
        MN34120_HardwareReset();
        AmbaPrint("[MN34120] hw reset");

        /* the Sensor Power is ON, Sensor exit standby mode */
        MN34120Ctrl.Status.DevStat.Bits.Sensor0Power = 1;
        MN34120Ctrl.Status.DevStat.Bits.Sensor0Standby = 1;
    } else {
        MN34120_SetStandbyOff();
        MN34120Ctrl.Status.DevStat.Bits.Sensor0Standby = 0;
    }

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MN34120_Disable
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
static int MN34120_Disable(AMBA_DSP_CHANNEL_ID_u Chan)
{
    if (MN34120Ctrl.Status.DevStat.Bits.Sensor0Power == 0)
        return OK;

    MN34120_SetStandbyOn();
    MN34120Ctrl.Status.DevStat.Bits.Sensor0Standby = 1;  /* Sensor is at Standby mode */

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MN34120_GetStatus
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
static int MN34120_GetStatus(AMBA_DSP_CHANNEL_ID_u Chan, AMBA_SENSOR_STATUS_INFO_s *pStatus)
{
    if (pStatus == NULL || MN34120Ctrl.Status.DevStat.Bits.Sensor0Power == 0)
        return NG;

    MN34120Ctrl.Status.TimeToAllPelExposed = 0;  /* TODO: for flash light or mechanical shutter control */

    memcpy(pStatus, &MN34120Ctrl.Status, sizeof(AMBA_SENSOR_STATUS_INFO_s));

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MN34120_GetModeInfo
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
static int MN34120_GetModeInfo(AMBA_DSP_CHANNEL_ID_u Chan, AMBA_SENSOR_MODE_ID_u Mode, AMBA_SENSOR_MODE_INFO_s *pModeInfo)
{
    UINT16 SensorMode = Mode.Bits.Mode;

    if (Mode.Bits.Mode == AMBA_SENSOR_CURRENT_MODE)
        Mode.Bits.Mode = MN34120Ctrl.Status.ModeInfo.Mode.Bits.Mode;

    if (SensorMode >= AMBA_SENSOR_MN34120_NUM_MODE || pModeInfo == NULL)
        return NG;

    MN34120_PrepareModeInfo(Mode, pModeInfo);

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MN34120_GetDeviceInfo
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
static int MN34120_GetDeviceInfo(AMBA_DSP_CHANNEL_ID_u Chan, AMBA_SENSOR_DEVICE_INFO_s *pDeviceInfo)
{
    if (pDeviceInfo == NULL)
        return NG;

    memcpy(pDeviceInfo, &MN34120DeviceInfo, sizeof(AMBA_SENSOR_DEVICE_INFO_s));

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MN34120_GetCurrentGainFactor
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
static int MN34120_GetCurrentGainFactor(AMBA_DSP_CHANNEL_ID_u Chan, float *pGainFactor)
{
    *pGainFactor = ((float)MN34120Ctrl.GainFactor / (1024.0 * 1024.0) );

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MN34120_GetCurrentShutterSpeed
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
static int MN34120_GetCurrentShutterSpeed(AMBA_DSP_CHANNEL_ID_u Chan, float *pExposureTime)
{
    *pExposureTime = (MN34120Ctrl.Status.ModeInfo.RowTime / MN34120Ctrl.FrameTime.NumXhsPerH) *
                     ((MN34120Ctrl.CurrentShrCtrlSVR + 1) * (MN34120Ctrl.FrameTime.NumXhsPerV / MN34120Ctrl.FrameTime.NumXvsPerV) - MN34120Ctrl.CurrentShrCtrlSHR);

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MN34120_ConvertGainFactor
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
static int MN34120_ConvertGainFactor(AMBA_DSP_CHANNEL_ID_u Chan, float DesiredFactor, UINT32 *pActualFactor, UINT32 *pAnalogGainCtrl, UINT32 *pDigitalGainCtrl)
{
    float AnalogGain = 0.0;
    float Digitalgain = 0.0;
    float CompensateFactor = 0.0;
    float DigitalFactor = 0.0;

    CompensateFactor = DesiredFactor /
        (float)MN34120InputInfo[MN34120ModeInfoList[MN34120Ctrl.Status.ModeInfo.Mode.Bits.Mode].ReadoutMode].SummingFactor;

    if(CompensateFactor > 98.2172) {
        *pAnalogGainCtrl = 426;   //Max Again register value
        *pDigitalGainCtrl = 575;  //Max Dgain register value
        AnalogGain =  6.2643;     //Max Again
        Digitalgain =     15.6788; //Max Dgain
    } else if (CompensateFactor > 6.2643) {
        *pAnalogGainCtrl = 426;   //Max Again register value
        AnalogGain =  6.2643;     //Max Again

        DigitalFactor = CompensateFactor / 6.2643;
        *pDigitalGainCtrl = (UINT32) (((213.333333333333333 * log10(DigitalFactor)) + 320) + 0.5);
        Digitalgain = powf(10.0, 0.0046875 * (*pDigitalGainCtrl - 320));
    } else if (CompensateFactor > 0.0833) {
        *pAnalogGainCtrl = (UINT32) (((213.333333333333333 * log10(CompensateFactor)) + 256) + 0.5);

        if (*pAnalogGainCtrl >= 256)
            AnalogGain = powf(10.0, 0.0046875 * (*pAnalogGainCtrl - 256));
        else
            AnalogGain = 1.0 / powf(10.0, 0.0046875 * (256 - *pAnalogGainCtrl));

        *pDigitalGainCtrl = 320;  //Min Dgain register value
        Digitalgain = 1.0;        //Min Dgain
    } else if (CompensateFactor <= 0.0833) {
        *pAnalogGainCtrl = 64;    //Min Dgain register value
        *pDigitalGainCtrl = 218;  //Min Dgain register value
        AnalogGain = 0.25;        //Min Again
        Digitalgain = 0.3333;     //Min Dgain
    }

     *pActualFactor = (UINT32)(1024.0 * 1024.0 * MN34120InputInfo[MN34120ModeInfoList[MN34120Ctrl.Status.ModeInfo.Mode.Bits.Mode].ReadoutMode].SummingFactor * AnalogGain * Digitalgain + 0.5);
     MN34120Ctrl.GainFactor = *pActualFactor;
    //AmbaPrint("DigitalFactor:%f, AnalogGain:%f, Digitalgain:%f",DigitalFactor,AnalogGain,Digitalgain);
    //AmbaPrint("DesiredFactor = %f, CompensateFactor: %f, DgainReg = 0x%08x, AgainReg = 0x%08x, ActualFactor = %d",DesiredFactor, CompensateFactor, *pDigitalGainCtrl, *pAnalogGainCtrl, *pActualFactor);
    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MN34120_ConvertShutterSpeed
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
static int MN34120_ConvertShutterSpeed(AMBA_DSP_CHANNEL_ID_u Chan, float ExposureTime, UINT32 *pShutterCtrl)
{
    AMBA_SENSOR_MODE_INFO_s *pModeInfo = &MN34120Ctrl.Status.ModeInfo;
    UINT32 R_LONG_EXPOSURE = 0; /* specifies the exposure end vertical period */
    UINT32 MinShuCtrl = 0;
    UINT32 MaxShuCtrl = 0;

    if (pShutterCtrl == NULL)
        return NG;

    *pShutterCtrl = (UINT32)(ExposureTime / pModeInfo->RowTime);

    R_LONG_EXPOSURE =  (*pShutterCtrl -1) / pModeInfo->NumExposureStepPerFrame;

    MinShuCtrl = 0;
    MaxShuCtrl = ((R_LONG_EXPOSURE + 1) * pModeInfo->NumExposureStepPerFrame) - 4;

    if (*pShutterCtrl > MaxShuCtrl)
        *pShutterCtrl = MaxShuCtrl;
    else if (*pShutterCtrl < MinShuCtrl)
        *pShutterCtrl = MinShuCtrl;

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MN34120_SetAnalogGainCtrl
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
static int MN34120_SetAnalogGainCtrl(AMBA_DSP_CHANNEL_ID_u Chan, UINT32 AnalogGainCtrl)
{
    MN34120_SetAnalogGainReg(AnalogGainCtrl);

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MN34120_SetDigitalGainCtrl
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
static int MN34120_SetDigitalGainCtrl(AMBA_DSP_CHANNEL_ID_u Chan, UINT32 DigitalGainCtrl)
{
    MN34120_SetDigitalGainReg(DigitalGainCtrl);

    return OK;
}


/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MN34120_SetShutterCtrl
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
static int MN34120_SetShutterCtrl(AMBA_DSP_CHANNEL_ID_u Chan, UINT32 ShutterCtrl)
{
    MN34120_SetShutterReg(ShutterCtrl);

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MN34120_SetSlowShutterCtrl
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
static int MN34120_SetSlowShutterCtrl(AMBA_DSP_CHANNEL_ID_u Chan, UINT32 SlowShutterCtrl)
{
    MN34120_SetSlowShutterReg(SlowShutterCtrl);
    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MN34120_Config
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
static int MN34120_Config(AMBA_DSP_CHANNEL_ID_u Chan, AMBA_SENSOR_MODE_ID_u Mode, AMBA_SENSOR_ESHUTTER_TYPE_e ElecShutterMode)
{
    AMBA_SENSOR_MODE_INFO_s *pModeInfo;
    UINT16 SensorMode = Mode.Bits.Mode;
    UINT16 Reg = 0;
    static UINT8 FirstInit = 1;
    MN34120_READOUT_MODE_e ReadoutMode = MN34120ModeInfoList[SensorMode].ReadoutMode;
    MN34120_FRAME_TIMING_s *pFrameTime = &MN34120Ctrl.FrameTime;

    if (SensorMode >= AMBA_SENSOR_MN34120_NUM_MODE)
        return NG;

    AmbaPrint("============  [ MN34120 ]  ============");
    AmbaPrint("============ init mode:%d ============", SensorMode);

    /* update status */
    MN34120Ctrl.Status.ElecShutterMode = ElecShutterMode;
    pModeInfo = &MN34120Ctrl.Status.ModeInfo;
    MN34120_PrepareModeInfo(Mode, pModeInfo);
    memcpy(pFrameTime, &MN34120ModeInfoList[SensorMode].FrameTiming, sizeof(MN34120_FRAME_TIMING_s));

    /* reset master sync*/
    AmbaVIN_ResetHvSyncOutput((AMBA_VIN_CHANNEL_e) Chan.Bits.VinID);
    /* Make VIN not to capture bad frames during readout mode transition */
    AmbaVIN_Reset((AMBA_VIN_CHANNEL_e) Chan.Bits.VinID, AMBA_VIN_LVDS, NULL);

    /* do HW reset */
    /* set pll */
    AmbaPLL_SetSensorClk(pModeInfo->FrameTime.InputClk);
    AmbaPLL_SetSoVinClk(pModeInfo->FrameTime.InputClk);
    AmbaKAL_TaskSleep(1);

//    if (!MN34120Ctrl.Status.DevStat.Bits.Sensor0Power || MN34120Ctrl.Status.DevStat.Bits.Sensor0Standby) {
    if (!MN34120Ctrl.Status.DevStat.Bits.Sensor0Power || FirstInit) {
        /* if the Sensor Power is OFF, then enable the Sensor first */
        MN34120_Enable(Chan);
        AmbaKAL_TaskSleep(1);

        /* write registers of mode change to sensor */
        MN34120_ChangeReadoutMode(ReadoutMode);

        FirstInit = 0;

        //AmbaPrint("[MN34120]Config sensor with sensor enable");
    } else if (MN34120LeavePSMode) {
        /* write registers of mode change to sensor */
        MN34120_ChangeReadoutMode(ReadoutMode);
        MN34120LeavePSMode = 0;
        //AmbaPrint("[MN34120]Config sensor with sensor power saving mode");
    } else {
        //soft reset
        Reg = 0x0073;
        MN34120_RegRW(MN34120_RESET, &Reg, NULL, 1);
        AmbaKAL_TaskSleep(1);

        MN34120_ConfigPll(ReadoutMode);
        MN34120_ConfigSysReg(ReadoutMode);
        MN34120_ConfigTgRstReg(ReadoutMode);

        //AmbaPrint("[MN34120]Config sensor general case");
    }

    /* config vin */
    MN34120_ConfigVin(pModeInfo, pFrameTime);

    /* reset vin for A12*/
    AmbaVIN_Reset((AMBA_VIN_CHANNEL_e) Chan.Bits.VinID, AMBA_VIN_LVDS, NULL);
    MN34120Ctrl.Status.DevStat.Bits.Sensor0Standby = 0;

    return OK;
}

/*-----------------------------------------------------------------------------------------------*/
AMBA_SENSOR_OBJ_s AmbaSensor_MN34120Obj = {
    .Init                   = MN34120_Init,
    .Enable                 = MN34120_Enable,
    .Disable                = MN34120_Disable,
    .Config                 = MN34120_Config,
    .GetStatus              = MN34120_GetStatus,
    .GetModeInfo            = MN34120_GetModeInfo,
    .GetDeviceInfo          = MN34120_GetDeviceInfo,
    .GetCurrentGainFactor   = MN34120_GetCurrentGainFactor,
    .GetCurrentShutterSpeed = MN34120_GetCurrentShutterSpeed,

    .ConvertGainFactor      = MN34120_ConvertGainFactor,
    .ConvertShutterSpeed    = MN34120_ConvertShutterSpeed,

    .SetAnalogGainCtrl      = MN34120_SetAnalogGainCtrl,
    .SetDigitalGainCtrl     = MN34120_SetDigitalGainCtrl,
    .SetShutterCtrl         = MN34120_SetShutterCtrl,
    .SetSlowShutterCtrl     = MN34120_SetSlowShutterCtrl,

    .RegisterRead           = NULL,
    .RegisterWrite          = MN34120_RegisterWrite,
};
