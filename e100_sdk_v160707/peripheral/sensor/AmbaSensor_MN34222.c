/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaSensor_MN34222.c
 *
 *  @Copyright      :: Copyright (C) 2013 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Control APIs of Panasonic MN34222 CMOS sensor with LVDS interface
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
#include "AmbaSensor_MN34222.h"

#define X_Y_ABS(x)   (((x) < (0)) ? -(x) : (x))

/*-----------------------------------------------------------------------------------------------*\
 * MN34222 sensor runtime info
\*-----------------------------------------------------------------------------------------------*/
MN34222_CTRL_s MN34222Ctrl = {0};
static UINT8 SensorSlaveID = 0x6c; /* Sensor slave ID for I2C */

/*-----------------------------------------------------------------------------------------------*\
 * Lane mapping for SoC side VIN
\*-----------------------------------------------------------------------------------------------*/
static AMBA_DSP_VIN_SLVS_PIN_PAIR_e MN34222_LvdsLaneMux[AMBA_DSP_NUM_VIN_SLVS_DATA_LANE] = {
    AMBA_DSP_VIN_PIN_SD_LVDS_0,
    AMBA_DSP_VIN_PIN_SD_LVDS_1,
    AMBA_DSP_VIN_PIN_SD_LVDS_2,
    AMBA_DSP_VIN_PIN_SD_LVDS_3,
    AMBA_DSP_VIN_PIN_SD_LVDS_4,
    AMBA_DSP_VIN_PIN_SD_LVDS_5,
    AMBA_DSP_VIN_PIN_SD_LVDS_6,
    AMBA_DSP_VIN_PIN_SD_LVDS_7,
    AMBA_DSP_VIN_PIN_SD_LVDS_8,
    AMBA_DSP_VIN_PIN_SD_LVDS_9
};

/*-----------------------------------------------------------------------------------------------*\
 * Configurations for Amba. VIN side
\*-----------------------------------------------------------------------------------------------*/
static AMBA_DSP_VIN_SLVS_CONFIG_s MN34222VinConfig = {
    .Info = {
        .FrameRate = {
            .Interlace      = 0,
            .TimeScale      = 0,
            .NumUnitsInTick = 0,
        },
        .DspPhaseShift      = {
            .Horizontal     = AMBA_DSP_PHASE_SHIFT_MODE_0,
            .Vertical       = AMBA_DSP_PHASE_SHIFT_MODE_0,
        },
        .BayerPattern       = AMBA_DSP_BAYER_BG,
        .NumDataBits        = 0,
        .NumSkipFrame       = 1,
    },
    .pLaneMapping   = NULL,
    .NumActiveLanes = 0,
    .SyncDetectCtrl = {
        .SyncInterleaving   = 0,
        .ITU656Type         = 0,
        .CustomSyncCode     = {
            .PatternAlign   = 1, /* MSB */
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

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MN34222_PrepareModeInfo
 *
 *  @Description:: Get sensor mode information
 *
 *  @Input      ::
 *      Mode:   Sensor readout mode number
 *
 *  @Output     ::
 *      pModeInfo:        Details of the specified readout mode
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static void MN34222_PrepareModeInfo(UINT16 Mode, AMBA_SENSOR_MODE_INFO_s *pModeInfo)
{
    const AMBA_SENSOR_INPUT_INFO_s  *pInputInfo= &MN34222InputInfo[MN34222ModeInfoList[Mode].ReadoutMode];
    const AMBA_SENSOR_OUTPUT_INFO_s *pOutputInfo= &MN34222OutputInfo[MN34222ModeInfoList[Mode].ReadoutMode];

    pModeInfo->Mode.Data = Mode;
    pModeInfo->LineLengthPck = (UINT32)((UINT64)MN34222ModeInfoList[Mode].FrameTiming.NumTickPerXhs * MN34222ModeInfoList[Mode].FrameTiming.NumXhsPerH *
                                         pOutputInfo->DataRate / MN34222ModeInfoList[Mode].FrameTiming.InputClk *
                                         pOutputInfo->NumDataLanes / pOutputInfo->NumDataBits);
    pModeInfo->FrameLengthLines = MN34222ModeInfoList[Mode].FrameTiming.NumXhsPerV / MN34222ModeInfoList[Mode].FrameTiming.NumXhsPerH;
    pModeInfo->NumExposureStepPerFrame = MN34222ModeInfoList[Mode].FrameTiming.NumXhsPerV;
    pModeInfo->PixelRate = (float) pOutputInfo->DataRate * pOutputInfo->NumDataLanes / pOutputInfo->NumDataBits;
    pModeInfo->RowTime = (float) (MN34222ModeInfoList[Mode].FrameTiming.NumTickPerXhs * MN34222ModeInfoList[Mode].FrameTiming.NumXhsPerH) / MN34222ModeInfoList[Mode].FrameTiming.InputClk;
    pModeInfo->InternalExposureOffset = MN34222ModeInfoList[Mode].FrameTiming.InternalExposureOffset;
    pModeInfo->FrameTime.InputClk = MN34222ModeInfoList[Mode].FrameTiming.InputClk;
    memcpy(&pModeInfo->FrameTime.FrameRate, &(MN34222ModeInfoList[Mode].FrameTiming.FrameRate), sizeof(AMBA_DSP_FRAME_RATE_s));
    memcpy(&pModeInfo->InputInfo, pInputInfo, sizeof(AMBA_SENSOR_INPUT_INFO_s));
    memcpy(&pModeInfo->OutputInfo, pOutputInfo, sizeof(AMBA_SENSOR_OUTPUT_INFO_s));
    memcpy(&pModeInfo->MinFrameRate, &(MN34222ModeInfoList[Mode].FrameTiming.FrameRate), sizeof(AMBA_DSP_FRAME_RATE_s));
    memcpy(&pModeInfo->HdrInfo, &MN34222HdrInfo[Mode], sizeof(AMBA_SENSOR_HDR_INFO_s)); // HDR information

    /* Only support 1 stage slow shutter with none hdr mode*/
    if (pModeInfo->HdrInfo.HdrType == 0)
        pModeInfo->MinFrameRate.TimeScale /= 2;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MN34222_ConfigVin
 *
 *  @Description:: Configure VIN to receieve output frames of the new readout mode
 *
 *  @Input      ::
  *     Chan:       Vin ID and sensor ID
 *      pModeInfo:  Details of the specified readout mode
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static void MN34222_ConfigVin(AMBA_DSP_CHANNEL_ID_u Chan, AMBA_SENSOR_MODE_INFO_s *pModeInfo)
{
    AMBA_SENSOR_OUTPUT_INFO_s  *pImgInfo = &pModeInfo->OutputInfo;
    AMBA_DSP_VIN_SLVS_CONFIG_s *pVinCfg = &MN34222VinConfig;

    AmbaDSP_VinPhySetSLVS(AMBA_VIN_CHANNEL0); // Set VIN Phy to SLVS

    memcpy(&pVinCfg->Info.FrameRate, &pModeInfo->FrameTime.FrameRate, sizeof(AMBA_DSP_FRAME_RATE_s));
    memcpy(&pVinCfg->Info.DspPhaseShift, &pImgInfo->DspPhaseShift, sizeof(AMBA_DSP_PHASE_SHIFT_CTRL_s));
    pVinCfg->Info.BayerPattern = (AMBA_DSP_SENSOR_PATTERN_e) pImgInfo->CfaPattern;
    pVinCfg->Info.NumDataBits = pImgInfo->NumDataBits;
    pVinCfg->NumActiveLanes = pImgInfo->NumDataLanes;
    pVinCfg->pLaneMapping = MN34222_LvdsLaneMux;
    pVinCfg->RxHvSyncCtrl.NumActivePixels = pImgInfo->OutputWidth;
    pVinCfg->RxHvSyncCtrl.NumActiveLines  = pImgInfo->OutputHeight;
    pVinCfg->RxHvSyncCtrl.NumTotalPixels  = pModeInfo->LineLengthPck - 1;
    pVinCfg->RxHvSyncCtrl.NumTotalLines   = pModeInfo->FrameLengthLines;

    AmbaDSP_VinConfigSLVS((AMBA_VIN_CHANNEL_e)Chan.Bits.VinID, pVinCfg);
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MN34222_HardwareReset
 *
 *  @Description:: Reset MN34222 Image Sensor Device
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static inline void MN34222_HardwareReset(void)
{
    AmbaUserGPIO_SensorResetCtrl(0);
    AmbaKAL_TaskSleep(3);
    AmbaUserGPIO_SensorResetCtrl(1);
    AmbaKAL_TaskSleep(3);
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MN34222_RegWrite
 *
 *  @Description:: Write sensor registers
 *
 *  @Input      ::
 *      pTxDataBuf:   Pointer to TxData
 *      Size          TxData Size
 *  @Output     ::    none
 *
 *  @Return     ::
 *      int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static int MN34222_RegWrite(UINT8 *pTxDataBuf, int Size)
{
    if (AmbaI2C_Write(AMBA_I2C_CHANNEL0, AMBA_I2C_SPEED_FAST, SensorSlaveID, Size, pTxDataBuf, AMBA_KAL_WAIT_FOREVER) == NG)
        AmbaPrint("I2C does not work!!!!!");

    //for(int i; i < Size; i++)
        //AmbaPrint("TxData[%d] = 0x%02x", pTxDataBuf[i]);

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MN34222_RegisterWrite
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
static int MN34222_RegisterWrite(AMBA_DSP_CHANNEL_ID_u Chan, UINT16 Addr, UINT16 Data)
{
    UINT8 TxDataBuf[3];

    TxDataBuf[0] = (UINT8)((Addr & 0xff00) >> 8);
    TxDataBuf[1] = (UINT8)(Addr & 0x00ff);
    TxDataBuf[2] = (UINT8)Data;

    MN34222_RegWrite(TxDataBuf, 3);

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MN34222_RegRead
 *
 *  @Description:: Read sensor registers
 *
 *  @Input      ::
 *      Addr:      Register address
 *      pRxData:   Pointer to register value
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *      int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static int MN34222_RegRead(UINT16 Addr, UINT8 *pRxData)
{
    UINT16 pTxData[4];

    pTxData[0] = AMBA_I2C_RESTART_FLAG | (SensorSlaveID);
    pTxData[1] = (Addr >> 8);    /* Register Address [15:8] */
    pTxData[2] = (Addr & 0xff);  /* Register Address [7:0]  */
    pTxData[3] = AMBA_I2C_RESTART_FLAG | (SensorSlaveID + 1);

    return AmbaI2C_ReadAfterWrite(AMBA_I2C_CHANNEL0, AMBA_I2C_SPEED_FAST, 4, (UINT16 *) pTxData, 1, pRxData, AMBA_KAL_WAIT_FOREVER);
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MN34222_RegisterRead
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
static int MN34222_RegisterRead(AMBA_DSP_CHANNEL_ID_u Chan, UINT16 Addr, UINT16 *Data)
{
    static UINT8 ReadData;

    MN34222_RegRead(Addr, &ReadData);
    *Data = ReadData;

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MN34222_SetAnalogGainReg
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
static void MN34222_SetAnalogGainReg(UINT32 AnalogGainCtrl)
{
    UINT8 TxDataBuf[4];

    TxDataBuf[0] = 0x02;
    TxDataBuf[1] = 0x04;
    TxDataBuf[2] = (UINT8)((AnalogGainCtrl & 0xff00) >> 8);
    TxDataBuf[3] = (UINT8)(AnalogGainCtrl & 0x00ff);

    MN34222_RegWrite(TxDataBuf, 4);
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MN34222_SetDigitalGainReg
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
static void MN34222_SetDigitalGainReg(UINT32 DigitalGainCtrl)
{
    UINT8 TxDataBuf[10];

    TxDataBuf[0] = 0x02;
    TxDataBuf[1] = 0x0e;
    TxDataBuf[2] = (UINT8)((DigitalGainCtrl & 0xff00) >> 8);
    TxDataBuf[3] = (UINT8)(DigitalGainCtrl & 0x00ff);
    TxDataBuf[4] = (UINT8)((DigitalGainCtrl & 0xff00) >> 8);
    TxDataBuf[5] = (UINT8)(DigitalGainCtrl & 0x00ff);
    TxDataBuf[6] = (UINT8)((DigitalGainCtrl & 0xff00) >> 8);
    TxDataBuf[7] = (UINT8)(DigitalGainCtrl & 0x00ff);
    TxDataBuf[8] = (UINT8)((DigitalGainCtrl & 0xff00) >> 8);
    TxDataBuf[9] = (UINT8)(DigitalGainCtrl & 0x00ff);

    MN34222_RegWrite(TxDataBuf, 10);
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MN34222_SetShutterReg
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
static void MN34222_SetShutterReg(UINT32 ShutterCtrl)
{
    UINT32 SVR, SHR;
    UINT32 MaxSHR = MN34222Ctrl.Status.ModeInfo.NumExposureStepPerFrame - 2; // Considering slow-shutter case
    UINT32 MinSHR = 1;
    UINT8 TxDataBuf[4];

    SVR = ShutterCtrl / MN34222Ctrl.Status.ModeInfo.NumExposureStepPerFrame;       // Considering slow-shutter case
    SHR = ShutterCtrl - SVR * MN34222Ctrl.Status.ModeInfo.NumExposureStepPerFrame; // Considering slow-shutter case

    if(SHR < MinSHR)
        SHR = MinSHR;
    else if(SHR > MaxSHR)
        SHR = MaxSHR;

    if (SVR > 0) {
        TxDataBuf[0] = 0x31;
        TxDataBuf[1] = 0x22;
        TxDataBuf[2] = (UINT8)((SVR >> 8) & 0x00ff);
        TxDataBuf[3] = (UINT8)(SVR & 0x00ff);
        MN34222_RegWrite(TxDataBuf, 4);
    }

    TxDataBuf[0] = 0x02;
    TxDataBuf[1] = 0x02;
    TxDataBuf[2] = (UINT8)((SHR >> 8) & 0x00ff);
    TxDataBuf[3] = (UINT8)(SHR & 0x00ff);

    MN34222_RegWrite(TxDataBuf, 4);

    /* Update current shutter control */
    MN34222Ctrl.CurrentAEInfo.CurrentShrCtrlSVR[0] = SVR;
    MN34222Ctrl.CurrentAEInfo.CurrentShrCtrlSHR[0] = SHR;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MN34222_SetHdrAnalogGainReg
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
static void MN34222_SetHdrAnalogGainReg(AMBA_DSP_CHANNEL_ID_u Chan, UINT32 *pAnalogGainCtrl)
{
    UINT8 TxDataBuf[4];

    /* For long & short exposure frame */
    TxDataBuf[0] = 0x02;
    TxDataBuf[1] = 0x04;
    TxDataBuf[2] = (UINT8)((*pAnalogGainCtrl & 0xff00) >> 8);
    TxDataBuf[3] = (UINT8)(*pAnalogGainCtrl & 0x00ff);

    MN34222_RegWrite(TxDataBuf, 4);
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MN34222_SetHdrDigitalGainReg
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
static void MN34222_SetHdrDigitalGainReg(AMBA_DSP_CHANNEL_ID_u Chan, UINT32 *pDigitalGainCtrl)
{
    UINT8 TxDataBuf[10];

    if (Chan.Bits.HdrID & 0x1) {
        /* Digital gain & WB gain */
        /* For long exposure frame */
        TxDataBuf[0] = 0x02;
        TxDataBuf[1] = 0x0e;
        TxDataBuf[2] = (UINT8)((*pDigitalGainCtrl & 0xff00) >> 8);
        TxDataBuf[3] = (UINT8)(*pDigitalGainCtrl & 0x00ff);
        TxDataBuf[4] = (UINT8)((*pDigitalGainCtrl & 0xff00) >> 8);
        TxDataBuf[5] = (UINT8)(*pDigitalGainCtrl & 0x00ff);
        TxDataBuf[6] = (UINT8)((*pDigitalGainCtrl & 0xff00) >> 8);
        TxDataBuf[7] = (UINT8)(*pDigitalGainCtrl & 0x00ff);
        TxDataBuf[8] = (UINT8)((*pDigitalGainCtrl & 0xff00) >> 8);
        TxDataBuf[9] = (UINT8)(*pDigitalGainCtrl & 0x00ff);

        MN34222_RegWrite(TxDataBuf, 10);
    }

    if (Chan.Bits.HdrID & 0x2) {
        /* Digital gain & WB gain */
        /* For short exposure frame */
        TxDataBuf[0] = 0x31;
        TxDataBuf[1] = 0x0a;
        TxDataBuf[2] = (UINT8)((*(pDigitalGainCtrl + 1) & 0xff00) >> 8);
        TxDataBuf[3] = (UINT8)(*(pDigitalGainCtrl + 1) & 0x00ff);
        TxDataBuf[4] = (UINT8)((*(pDigitalGainCtrl + 1) & 0xff00) >> 8);
        TxDataBuf[5] = (UINT8)(*(pDigitalGainCtrl + 1) & 0x00ff);
        TxDataBuf[6] = (UINT8)((*(pDigitalGainCtrl + 1) & 0xff00) >> 8);
        TxDataBuf[7] = (UINT8)(*(pDigitalGainCtrl + 1) & 0x00ff);
        TxDataBuf[8] = (UINT8)((*(pDigitalGainCtrl + 1) & 0xff00) >> 8);
        TxDataBuf[9] = (UINT8)(*(pDigitalGainCtrl + 1) & 0x00ff);

        MN34222_RegWrite(TxDataBuf, 10);
    }
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MN34222_SetHdrShutterReg
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
static void MN34222_SetHdrShutterReg(AMBA_DSP_CHANNEL_ID_u Chan, UINT32 *pShutterCtrl)
{
    UINT8 TxDataBuf[4];

    if (Chan.Bits.HdrID & 0x1) {
        TxDataBuf[0] = 0x02;
        TxDataBuf[1] = 0x02;
        TxDataBuf[2] = (UINT8)(((*pShutterCtrl / 2) >> 8) & 0x00ff);
        TxDataBuf[3] = (UINT8)((*pShutterCtrl / 2) & 0x00ff);

        MN34222_RegWrite(TxDataBuf, 4);

        /* Update current shutter control */
        MN34222Ctrl.CurrentAEInfo.CurrentShrCtrlSVR[0] = 0;
        MN34222Ctrl.CurrentAEInfo.CurrentShrCtrlSHR[0] = *pShutterCtrl;
    }

    if (Chan.Bits.HdrID & 0x2) {
        TxDataBuf[0] = 0x31;
        TxDataBuf[1] = 0x2a;
        TxDataBuf[2] = (UINT8)(((*(pShutterCtrl + 1) / 2) >> 8) & 0x00ff);
        TxDataBuf[3] = (UINT8)((*(pShutterCtrl + 1) / 2) & 0x00ff);

        MN34222_RegWrite(TxDataBuf, 4);

        /* Update current shutter control */
        MN34222Ctrl.CurrentAEInfo.CurrentShrCtrlSVR[1] = 0;
        MN34222Ctrl.CurrentAEInfo.CurrentShrCtrlSHR[1] = *(pShutterCtrl + 1);
        MN34222Ctrl.Status.ModeInfo.HdrInfo.ChannelInfo[1].EffectiveArea.StartY = (MN34222Ctrl.CurrentAEInfo.CurrentShrCtrlSHR[1] / 2) + 16;
        MN34222Ctrl.Status.ModeInfo.HdrInfo.ChannelInfo[1].OpticalBlackPixels.StartY = (MN34222Ctrl.CurrentAEInfo.CurrentShrCtrlSHR[1] / 2) + 2;
    }
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MN34222_SetSlowShutterReg
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
static void MN34222_SetSlowShutterReg(UINT32 SlowShutterCtrl)
{
    UINT32 TargetFrameLengthLines;
    UINT8 TxDataBuf[4];

    if (SlowShutterCtrl < 1)
        SlowShutterCtrl = 1;

    TargetFrameLengthLines = MN34222Ctrl.ModeInfo.FrameLengthLines * SlowShutterCtrl;

    TxDataBuf[0] = 0x03;
    TxDataBuf[1] = 0x40;
    TxDataBuf[2] = (UINT8)((TargetFrameLengthLines >> 8) & 0x00ff);
    TxDataBuf[3] = (UINT8)(TargetFrameLengthLines & 0x00ff);

    MN34222_RegWrite(TxDataBuf, 4);

    /* Update current frame rate information */
    MN34222Ctrl.Status.ModeInfo.FrameLengthLines = TargetFrameLengthLines;
    MN34222Ctrl.Status.ModeInfo.NumExposureStepPerFrame = TargetFrameLengthLines;
    MN34222Ctrl.Status.ModeInfo.FrameTime.FrameRate.TimeScale = MN34222Ctrl.ModeInfo.FrameTime.FrameRate.TimeScale / SlowShutterCtrl;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MN34222_SetStandbyOn
 *
 *  @Description:: Configure sensor into standby mode
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static void MN34222_SetStandbyOn(void)
{
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MN34222_SetStandbyOff
 *
 *  @Description:: Configure sensor to leave standby mode.
 *
 *  @Input      ::
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static void MN34222_SetStandbyOff(void)
{
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MN34222_ChangeReadoutMode
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
static int MN34222_ChangeReadoutMode(UINT16 Mode)
{
    int i = 0;

    for (i = 0; i < MN34222_NUM_READOUT_MODE_REG; i ++) {

        UINT8 TxDataBuf[3];

        TxDataBuf[0] = (UINT8)((MN34222RegTable[i].Addr & 0xff00) >> 8);
        TxDataBuf[1] = (UINT8)(MN34222RegTable[i].Addr & 0x00ff);
        TxDataBuf[2] = (UINT8)MN34222RegTable[i].Data[MN34222ModeInfoList[Mode].ReadoutMode];
        MN34222_RegWrite(TxDataBuf, 3);

        AmbaKAL_TaskSleep(1);
    }

    /* Update HDR geometry information */
    if (MN34222HdrInfo[Mode].HdrType != 0) {
        UINT8 RegValMsb, RegValLsb;

        /* Reset exposure info. and window position info. of HDR mode */
        MN34222_RegRead(0x0202, &RegValMsb);
        MN34222_RegRead(0x0203, &RegValLsb);
        MN34222Ctrl.CurrentAEInfo.CurrentShrCtrlSHR[0] = ((RegValMsb << 8) + RegValLsb) * 2;

        MN34222_RegRead(0x312a, &RegValMsb);
        MN34222_RegRead(0x312b, &RegValLsb);
        MN34222Ctrl.CurrentAEInfo.CurrentShrCtrlSHR[1] = ((RegValMsb << 8) + RegValLsb) * 2;
        MN34222Ctrl.Status.ModeInfo.HdrInfo.ChannelInfo[1].EffectiveArea.StartY = (MN34222Ctrl.CurrentAEInfo.CurrentShrCtrlSHR[1] / 2) + 16;
        MN34222Ctrl.Status.ModeInfo.HdrInfo.ChannelInfo[1].OpticalBlackPixels.StartY = (MN34222Ctrl.CurrentAEInfo.CurrentShrCtrlSHR[1] / 2) + 2;
    }

    /* Reset current AE information */
    memset(&MN34222Ctrl.CurrentAEInfo, 0, sizeof(MN34222_CURRENT_AE_INFO_s));

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MN34222_Init
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
static int MN34222_Init(AMBA_DSP_CHANNEL_ID_u Chan)
{
    const MN34222_FRAME_TIMING_s *pFrameTime = &MN34222ModeInfoList[0].FrameTiming;
    AmbaPLL_SetSensorClk(pFrameTime->InputClk);             /* The default sensor input clock frequency */
    AmbaPLL_SetSensorClkDir(AMBA_PLL_SENSOR_CLK_OUTPUT);    /* Configure clock output to sensor (or sensor SPI won't work) */

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MN34222_Enable
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
static int MN34222_Enable(AMBA_DSP_CHANNEL_ID_u Chan)
{
    if (MN34222Ctrl.Status.DevStat.Bits.Sensor0Power == 1)
        return OK;

    MN34222Ctrl.Status.DevStat.Bits.Sensor0Power   = 1;
    MN34222Ctrl.Status.DevStat.Bits.Sensor0Standby = 1;

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MN34222_Disable
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
static int MN34222_Disable(AMBA_DSP_CHANNEL_ID_u Chan)
{
    if (MN34222Ctrl.Status.DevStat.Bits.Sensor0Power == 0)
        return OK;

    MN34222_SetStandbyOn();

    MN34222Ctrl.Status.DevStat.Bits.Sensor0Power = 0;

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MN34222_GetStatus
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
static int MN34222_GetStatus(AMBA_DSP_CHANNEL_ID_u Chan, AMBA_SENSOR_STATUS_INFO_s *pStatus)
{
    if (pStatus == NULL || MN34222Ctrl.Status.DevStat.Bits.Sensor0Power == 0)
        return NG;

    MN34222Ctrl.Status.TimeToAllPelExposed = 0;  /* TODO: for flash light or mechanical shutter control */

    memcpy(pStatus, &MN34222Ctrl.Status, sizeof(AMBA_SENSOR_STATUS_INFO_s));

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MN34222_GetModeInfo
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
static int MN34222_GetModeInfo(AMBA_DSP_CHANNEL_ID_u Chan, AMBA_SENSOR_MODE_ID_u Mode, AMBA_SENSOR_MODE_INFO_s *pModeInfo)
{
    UINT16 SensorMode = Mode.Bits.Mode; // TODO

    if (SensorMode == AMBA_SENSOR_CURRENT_MODE)
        SensorMode = MN34222Ctrl.Status.ModeInfo.Mode.Bits.Mode;

    if (SensorMode >= AMBA_SENSOR_MN34222_NUM_MODE || pModeInfo == NULL)
        return NG;

    MN34222_PrepareModeInfo(SensorMode, pModeInfo);

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MN34222_GetDeviceInfo
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
static int MN34222_GetDeviceInfo(AMBA_DSP_CHANNEL_ID_u Chan, AMBA_SENSOR_DEVICE_INFO_s *pDeviceInfo)
{
    if (pDeviceInfo == NULL)
        return NG;

    memcpy(pDeviceInfo, &MN34222DeviceInfo, sizeof(AMBA_SENSOR_DEVICE_INFO_s));

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MN34222_GetCurrentGainFactor
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
static int MN34222_GetCurrentGainFactor(AMBA_DSP_CHANNEL_ID_u Chan, float *pGainFactor)
{
    double AgcDbGain;
    double WbDbGain;
    UINT32 CurrentAgcCtrl;
    UINT32 CurrentWbCtrl;

    if (Chan.Bits.HdrID == 0x2) {
        CurrentAgcCtrl = MN34222Ctrl.CurrentAEInfo.CurrentAgcCtrl[1];
        CurrentWbCtrl = MN34222Ctrl.CurrentAEInfo.CurrentWbCtrl[1][0]; // Using R channel for linear mode only
    } else {
        CurrentAgcCtrl = MN34222Ctrl.CurrentAEInfo.CurrentAgcCtrl[0];
        CurrentWbCtrl = MN34222Ctrl.CurrentAEInfo.CurrentWbCtrl[0][0]; // Using R channel for linear mode only
    }

    AgcDbGain = (CurrentAgcCtrl - 256) * 30 / 320;

    if (MN34222Ctrl.Status.ModeInfo.Mode.Data == AMBA_SENSOR_MN34222_V1_12_1944X1092_30P_HDR_V_1500) {

        UINT32 WbCtrl = CurrentWbCtrl;

        /* Meet default setting  */
        if (WbCtrl == 0x13f)
            WbCtrl = 0x140;

        WbDbGain = (WbCtrl - 320) * 24 / 256;

    } else {
        WbDbGain = (CurrentWbCtrl - 256) * 30 / 320;
    }

    *pGainFactor = pow(10.0, (AgcDbGain / 20)) * pow(10.0, (WbDbGain / 20));

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MN34222_GetCurrentShutterSpeed
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
static int MN34222_GetCurrentShutterSpeed(AMBA_DSP_CHANNEL_ID_u Chan, float *pExposureTime)
{
    if (Chan.Bits.HdrID == 0x2)
        *pExposureTime = MN34222Ctrl.Status.ModeInfo.RowTime * ((float)MN34222Ctrl.CurrentAEInfo.CurrentShrCtrlSHR[1] + MN34222Ctrl.Status.ModeInfo.InternalExposureOffset);
    else
        *pExposureTime = MN34222Ctrl.Status.ModeInfo.RowTime * ((float)(MN34222Ctrl.CurrentAEInfo.CurrentShrCtrlSVR[0] * MN34222Ctrl.Status.ModeInfo.NumExposureStepPerFrame) +
                                                                (float)MN34222Ctrl.CurrentAEInfo.CurrentShrCtrlSHR[0] + MN34222Ctrl.Status.ModeInfo.InternalExposureOffset);

    //AmbaPrint("ExposureTime = %f", *pExposureTime);

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName::MN34222_GetHdrInfo
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
static int MN34222_GetHdrInfo(AMBA_DSP_CHANNEL_ID_u Chan, UINT32 *pShutterCtrl, AMBA_SENSOR_HDR_INFO_s *pHdrInfo)
{
    memcpy(pHdrInfo, &MN34222Ctrl.Status.ModeInfo.HdrInfo, sizeof(AMBA_SENSOR_HDR_INFO_s));

    pHdrInfo->ChannelInfo[1].EffectiveArea.StartY = (*(pShutterCtrl + 1) / 2) + 16;
    pHdrInfo->ChannelInfo[1].OpticalBlackPixels.StartY = (*(pShutterCtrl + 1) / 2) + 2;

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MN34222_ConvertGainFactor
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
static int MN34222_ConvertGainFactor(AMBA_DSP_CHANNEL_ID_u Chan, float DesiredFactor, UINT32 *pActualFactor, UINT32 *pAnalogGainCtrl, UINT32 *pDigitalGainCtrl)
{
    double DbGain = 20 * log10((double)DesiredFactor);
    double MaxDbGain, AgcDbGain, DgcDbGain;
    UINT32 Gain, WbRegValueRange, WbDbGainRange, WbBase;

    /* Maximum DbGain is 60dB(or 54dB for AMBA_SENSOR_MN34222_V1_12_1944X1092_30P_HDR_V_1500) */
    if (MN34222Ctrl.Status.ModeInfo.Mode.Data == AMBA_SENSOR_MN34222_V1_12_1944X1092_30P_HDR_V_1500) {
        MaxDbGain = 54;
        WbRegValueRange = 256;
        WbDbGainRange = 24;
        WbBase = 320;
    } else {
        MaxDbGain = 60;
        WbRegValueRange = 320;
        WbDbGainRange = 30;
        WbBase = 256;
    }

    if (DbGain < 0)
        DbGain = 0;

    if (DbGain > MaxDbGain)
        DbGain = MaxDbGain;

    if (DbGain > 30) {
        AgcDbGain = 30;
        DgcDbGain = DbGain - 30;
    } else {
        AgcDbGain = DbGain;
        DgcDbGain = 0;
    }

    /* For Agc */
    Gain = (AgcDbGain * 320 / 30) + 256;
    Gain = Gain / 4;
    Gain = Gain * 4;
    *pAnalogGainCtrl = Gain;

    /* For Dgc */
    Gain = (DgcDbGain * WbRegValueRange / WbDbGainRange) + WbBase;
    Gain = Gain / 4;
    Gain = Gain * 4;

    /* Meet default setting */
    if ((MN34222Ctrl.Status.ModeInfo.Mode.Data == AMBA_SENSOR_MN34222_V1_12_1944X1092_30P_HDR_V_1500) && (Gain == 0x140))
        Gain = 0x13f;

    *pDigitalGainCtrl = Gain;

    *pActualFactor = 1048576 * pow(10.0, ((float)AgcDbGain / 20)) * pow(10.0, ((float)DgcDbGain / 20));

    //AmbaPrint("AgcDbGain = %f, DgcDbGain = %f", AgcDbGain, DgcDbGain);
    //AmbaPrint("AnalogGainCtrl = 0x%08x, DigitalGainCtrl = 0x%08x", *pAnalogGainCtrl, *pDigitalGainCtrl);
    //AmbaPrint("ActualFactor << 20 = 0x%08x", *pActualFactor);

    return OK;
}

#if 0
/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MN34222_ConvertHdrGainFactor
 *
 *  @Description:: Convert HDR gain factor to HDR gain control
 *
 *  @Input      ::
 *      Chan:           Vin ID and sensor ID
 *      pHdrGainFactor: HDR gain factor
 *
 *  @Output     ::
 *      pHdrGainCtrl:   HDR gain control for achievable gain factor
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static int MN34222_ConvertHdrGainFactor(AMBA_DSP_CHANNEL_ID_u Chan, AMBA_SENSOR_HDR_GAIN_Factor_s *pHdrGainFactor, AMBA_SENSOR_HDR_GAIN_CTRL_s *pHdrGainCtrl)
{
    double DbGainMatrix[2][4];
    double DbGainMin[2] = {60.0, 60.0};
    double DbGainMax[2] = {1.0, 1.0};
    double AgcDbGain, MaxDbGain;

    int i, j;

    /* Maximum DbGain is 60dB(or 54dB for AMBA_SENSOR_MN34222_V1_12_1944X1092_30P_HDR_V_1500) */
    if (MN34222Ctrl.Status.ModeInfo.Mode.Data == AMBA_SENSOR_MN34222_V1_12_1944X1092_30P_HDR_V_1500)
        MaxDbGain = 54.0;
    else
        MaxDbGain = 60.0;

    /* Fill DbGainMatrix */
    for(i = 0; i < 2; i++) {
        DbGainMatrix[i][0] = 20 * log10((double)(pHdrGainFactor[i].DesiredFactor * pHdrGainFactor[i].DesiredWbFactor.R));
        DbGainMatrix[i][1] = 20 * log10((double)(pHdrGainFactor[i].DesiredFactor * pHdrGainFactor[i].DesiredWbFactor.Gr));
        DbGainMatrix[i][2] = 20 * log10((double)(pHdrGainFactor[i].DesiredFactor * pHdrGainFactor[i].DesiredWbFactor.Gb));
        DbGainMatrix[i][3] = 20 * log10((double)(pHdrGainFactor[i].DesiredFactor * pHdrGainFactor[i].DesiredWbFactor.B));
    }

    /* Limit DbGainMatrix from 0dB to 60dB(or 54dB for AMBA_SENSOR_MN34222_V1_12_1944X1092_30P_HDR_V_1500) */
    for(i = 0; i < 2; i++) {
        for(j = 0; j < 4; j++) {
            if (DbGainMatrix[i][j] > MaxDbGain)
                DbGainMatrix[i][j] = MaxDbGain;
            if (DbGainMatrix[i][j] < 0)
                DbGainMatrix[i][j] = 0;
            //AmbaPrint("DbGainMatrix[%d][%d] = %f", i, j, DbGainMatrix[i][j]);
        }
    }

    /* Calculate DbGainMin & DbGainMax */
    for(j = 0; j < 4; j++) {
        if (DbGainMatrix[0][j] > DbGainMax[0])
            DbGainMax[0] = DbGainMatrix[0][j];
        if (DbGainMatrix[0][j] < DbGainMin[0])
            DbGainMin[0] = DbGainMatrix[0][j];
        if (DbGainMatrix[1][j] > DbGainMax[1])
            DbGainMax[1] = DbGainMatrix[1][j];
        if (DbGainMatrix[1][j] < DbGainMin[1])
            DbGainMin[1] = DbGainMatrix[1][j];
    }

    /* ABS(LongDbGain - ShortDbGain) cannot exceed 30dB */
    if ((X_Y_ABS(DbGainMax[0] - DbGainMin[1]) > 30) || (X_Y_ABS(DbGainMax[1] - DbGainMin[0]) > 30))
        return NG;

    /* Select DbGainMin for AgcDbGain */
    if (DbGainMin[0] > DbGainMin[1])
        AgcDbGain = DbGainMin[1];
    else
        AgcDbGain = DbGainMin[0];

    /*Limit AgcDbGain to 30dB */
    if (AgcDbGain > 30)
        AgcDbGain = AgcDbGain - 30;
    //AmbaPrint("AgcDbGain = %f", AgcDbGain);

    /* Refine DbGainMatrix */
    for(i = 0; i < 2; i++) {
        for(j = 0; j < 4; j++) {
            DbGainMatrix[i][j] = DbGainMatrix[i][j] - AgcDbGain;
            //AmbaPrint("DbGainMatrix[%d][%d] = %f", i, j, DbGainMatrix[i][j]);
        }
    }

    for(i = 0; i < 2; i++) {
        UINT32 TempGain[4];
        UINT32 Gain, WbRegValueRange, WbDbGainRange, WbBase;

        if (MN34222Ctrl.Status.ModeInfo.Mode.Data == AMBA_SENSOR_MN34222_V1_12_1944X1092_30P_HDR_V_1500) {
            WbRegValueRange = 256;
            WbDbGainRange = 24;
            WbBase = 320;
        } else {
            WbRegValueRange = 320;
            WbDbGainRange = 30;
            WbBase = 256;
        }

        /* For Agc */
        Gain = (AgcDbGain * 320 / 30) + 256;
        Gain = Gain / 4;
        Gain = Gain * 4;
        pHdrGainCtrl[i].AnalogGainCtrl = Gain;

        // Return actual gain factor */
        pHdrGainCtrl[i].ActualFactor = pow(10.0, ((float)AgcDbGain / 20));

        /* For Wb gain */
        for(j = 0; j < 4; j++) {
            TempGain[j] = (DbGainMatrix[i][j] * WbRegValueRange / WbDbGainRange) + WbBase;
            TempGain[j] = TempGain[j] / 4;
            TempGain[j] = TempGain[j] * 4;

            /* Meet default setting */
            if ((MN34222Ctrl.Status.ModeInfo.Mode.Data == AMBA_SENSOR_MN34222_V1_12_1944X1092_30P_HDR_V_1500) && (TempGain[j] == 0x140))
                TempGain[j] = 0x13f;
        }

        pHdrGainCtrl[i].WbGainCtrl.R  = TempGain[0];
        pHdrGainCtrl[i].WbGainCtrl.Gr = TempGain[1];
        pHdrGainCtrl[i].WbGainCtrl.Gb = TempGain[2];
        pHdrGainCtrl[i].WbGainCtrl.B  = TempGain[3];

        // Return actual Wb gain factor */
        pHdrGainCtrl[i].ActualWbFactor.R  = pow(10.0, ((float)DbGainMatrix[i][0] / 20));
        pHdrGainCtrl[i].ActualWbFactor.Gr = pow(10.0, ((float)DbGainMatrix[i][1] / 20));
        pHdrGainCtrl[i].ActualWbFactor.Gb = pow(10.0, ((float)DbGainMatrix[i][2] / 20));
        pHdrGainCtrl[i].ActualWbFactor.B  = pow(10.0, ((float)DbGainMatrix[i][3] / 20));
    }


    return OK;
}
#endif

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MN34222_ConvertShutterSpeed
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
static int MN34222_ConvertShutterSpeed(AMBA_DSP_CHANNEL_ID_u Chan, float ExposureTime, UINT32 *pShutterCtrl)
{
    AMBA_SENSOR_MODE_INFO_s *pModeInfo = &MN34222Ctrl.ModeInfo;
    float CalculateExposureTime;

    if (pShutterCtrl == NULL)
        return NG;

    /* Actual exposure time = (exposure lines setting + offset) * row time */
    CalculateExposureTime = ExposureTime - (pModeInfo->RowTime * pModeInfo->InternalExposureOffset);

    *pShutterCtrl = (UINT32)((CalculateExposureTime + 0.000001)/ pModeInfo->RowTime); // Add 1us to avoid calculation error

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
        ExposureTimeMaxMargin = ExposureFrames * pModeInfo->NumExposureStepPerFrame - 2;

        /* Exposure lines needs to be smaller than VTS - 2 */
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
 *  @RoutineName:: MN34222_SetAnalogGainCtrl
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
static int MN34222_SetAnalogGainCtrl(AMBA_DSP_CHANNEL_ID_u Chan, UINT32 AnalogGainCtrl)
{
    MN34222_SetAnalogGainReg(AnalogGainCtrl);

    /* Update current AGC control */
    MN34222Ctrl.CurrentAEInfo.CurrentAgcCtrl[0] = AnalogGainCtrl;

    //AmbaPrint("Set Again: 0x%08X", AnalogGainCtrl);

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MN34222_SetDigitalGainCtrl
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
static int MN34222_SetDigitalGainCtrl(AMBA_DSP_CHANNEL_ID_u Chan, UINT32 DigitalGainCtrl)
{
    MN34222_SetDigitalGainReg(DigitalGainCtrl);

    /* Update current DGC control */
    MN34222Ctrl.CurrentAEInfo.CurrentWbCtrl[0][0] = DigitalGainCtrl;
    MN34222Ctrl.CurrentAEInfo.CurrentWbCtrl[0][1] = DigitalGainCtrl;
    MN34222Ctrl.CurrentAEInfo.CurrentWbCtrl[0][2] = DigitalGainCtrl;
    MN34222Ctrl.CurrentAEInfo.CurrentWbCtrl[0][3] = DigitalGainCtrl;

    //AmbaPrint("Set Dgain: 0x%08X", DigitalGainCtrl);

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MN34222_SetShutterCtrl
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
static int MN34222_SetShutterCtrl(AMBA_DSP_CHANNEL_ID_u Chan, UINT32 ShutterCtrl)
{
    MN34222_SetShutterReg(ShutterCtrl);

    //AmbaPrint("Set Shutter: 0x%08X", ShutterCtrl);

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MN34222_SetHdrAnalogGainCtrl
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
static int MN34222_SetHdrAnalogGainCtrl(AMBA_DSP_CHANNEL_ID_u Chan, UINT32 *pAnalogGainCtrl)
{
    MN34222_SetHdrAnalogGainReg(Chan, pAnalogGainCtrl);

    /* Update current AGC control */
    if (Chan.Bits.HdrID & 0x1)
        MN34222Ctrl.CurrentAEInfo.CurrentAgcCtrl[0] = *pAnalogGainCtrl;

    if (Chan.Bits.HdrID & 0x2)
        MN34222Ctrl.CurrentAEInfo.CurrentAgcCtrl[1] = *pAnalogGainCtrl;

    //AmbaPrint("Set Again[0]: 0x%08X, Again[1]: 0x%08X", *pAnalogGainCtrl, *(pAnalogGainCtrl + 1));

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MN34222_SetHdrDigitalGainCtrl
 *
 *  @Description:: Set HDR digital gain control
 *
 *  @Input      ::
 *      Chan:               Vin ID and sensor ID
 *      pAnalogGainCtrl:    HDR digital gain control for achievable gain factor
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static int MN34222_SetHdrDigitalGainCtrl(AMBA_DSP_CHANNEL_ID_u Chan, UINT32 *pDigitalGainCtrl)
{
    MN34222_SetHdrDigitalGainReg(Chan, pDigitalGainCtrl);

    /* Update current DGC control */
    if (Chan.Bits.HdrID & 0x1) {
        MN34222Ctrl.CurrentAEInfo.CurrentWbCtrl[0][0] = *pDigitalGainCtrl;
        MN34222Ctrl.CurrentAEInfo.CurrentWbCtrl[0][1] = *pDigitalGainCtrl;
        MN34222Ctrl.CurrentAEInfo.CurrentWbCtrl[0][2] = *pDigitalGainCtrl;
        MN34222Ctrl.CurrentAEInfo.CurrentWbCtrl[0][3] = *pDigitalGainCtrl;
    }

    if (Chan.Bits.HdrID & 0x2) {
        MN34222Ctrl.CurrentAEInfo.CurrentWbCtrl[1][0] = *(pDigitalGainCtrl + 1);
        MN34222Ctrl.CurrentAEInfo.CurrentWbCtrl[1][1] = *(pDigitalGainCtrl + 1);
        MN34222Ctrl.CurrentAEInfo.CurrentWbCtrl[1][2] = *(pDigitalGainCtrl + 1);
        MN34222Ctrl.CurrentAEInfo.CurrentWbCtrl[1][3] = *(pDigitalGainCtrl + 1);
    }

    //AmbaPrint("Set Dgain[0]: 0x%08X, Dgain[1]: 0x%08X", *pDigitalGainCtrl, *(pDigitalGainCtrl + 1));

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MN34222_SetHdrWbGainCtrl
 *
 *  @Description:: Set HDR WB gain control
 *
 *  @Input      ::
 *      Chan:               Vin ID and sensor ID
 *      pWbGainCtrl:        HDR WB gain control for achievable gain factor
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static int MN34222_SetHdrWbGainCtrl(AMBA_DSP_CHANNEL_ID_u Chan, AMBA_SENSOR_WB_CTRL_s *pWbGainCtrl)
{
#if 0
    UINT8 TxDataBuf[10];

    if ((Chan.Bits.HdrID & 0x1) && (Chan.Bits.HdrID & 0x2)) {

        /* Digital gain & WB gain */
        /* For long exposure frame */
        TxDataBuf[0] = 0x02;
        TxDataBuf[1] = 0x0e;
        TxDataBuf[2] = (UINT8)((pWbGainCtrl[0].Gr & 0xff00) >> 8);
        TxDataBuf[3] = (UINT8)(pWbGainCtrl[0].Gr & 0x00ff);
        TxDataBuf[4] = (UINT8)((pWbGainCtrl[0].R & 0xff00) >> 8);
        TxDataBuf[5] = (UINT8)(pWbGainCtrl[0].R & 0x00ff);
        TxDataBuf[6] = (UINT8)((pWbGainCtrl[0].B & 0xff00) >> 8);
        TxDataBuf[7] = (UINT8)(pWbGainCtrl[0].B & 0x00ff);
        TxDataBuf[8] = (UINT8)((pWbGainCtrl[0].Gb & 0xff00) >> 8);
        TxDataBuf[9] = (UINT8)(pWbGainCtrl[0].Gb & 0x00ff);

        MN34222_RegWrite(TxDataBuf, 10);

        /* For short exposure frame */
        TxDataBuf[0] = 0x31;
        TxDataBuf[1] = 0x0a;
        TxDataBuf[2] = (UINT8)((pWbGainCtrl[1].Gr & 0xff00) >> 8);
        TxDataBuf[3] = (UINT8)(pWbGainCtrl[1].Gr & 0x00ff);
        TxDataBuf[4] = (UINT8)((pWbGainCtrl[1].R & 0xff00) >> 8);
        TxDataBuf[5] = (UINT8)(pWbGainCtrl[1].R & 0x00ff);
        TxDataBuf[6] = (UINT8)((pWbGainCtrl[1].B & 0xff00) >> 8);
        TxDataBuf[7] = (UINT8)(pWbGainCtrl[1].B & 0x00ff);
        TxDataBuf[8] = (UINT8)((pWbGainCtrl[1].Gb & 0xff00) >> 8);
        TxDataBuf[9] = (UINT8)(pWbGainCtrl[1].Gb & 0x00ff);

        MN34222_RegWrite(TxDataBuf, 10);

        /* Update current DGC control */
        MN34222Ctrl.CurrentAEInfo.CurrentWbCtrl[0][0] = pWbGainCtrl[0].R;
        MN34222Ctrl.CurrentAEInfo.CurrentWbCtrl[0][1] = pWbGainCtrl[0].Gr;
        MN34222Ctrl.CurrentAEInfo.CurrentWbCtrl[0][2] = pWbGainCtrl[0].Gb;
        MN34222Ctrl.CurrentAEInfo.CurrentWbCtrl[0][3] = pWbGainCtrl[0].B;
        MN34222Ctrl.CurrentAEInfo.CurrentWbCtrl[1][0] = pWbGainCtrl[1].R;
        MN34222Ctrl.CurrentAEInfo.CurrentWbCtrl[1][1] = pWbGainCtrl[1].Gr;
        MN34222Ctrl.CurrentAEInfo.CurrentWbCtrl[1][2] = pWbGainCtrl[1].Gb;
        MN34222Ctrl.CurrentAEInfo.CurrentWbCtrl[1][3] = pWbGainCtrl[1].B;

    } else if (Chan.Bits.HdrID & 0x1) {

        /* Digital gain & WB gain */
        /* For long exposure frame */
        TxDataBuf[0] = 0x02;
        TxDataBuf[1] = 0x0e;
        TxDataBuf[2] = (UINT8)((pWbGainCtrl[0].Gr & 0xff00) >> 8);
        TxDataBuf[3] = (UINT8)(pWbGainCtrl[0].Gr & 0x00ff);
        TxDataBuf[4] = (UINT8)((pWbGainCtrl[0].R & 0xff00) >> 8);
        TxDataBuf[5] = (UINT8)(pWbGainCtrl[0].R & 0x00ff);
        TxDataBuf[6] = (UINT8)((pWbGainCtrl[0].B & 0xff00) >> 8);
        TxDataBuf[7] = (UINT8)(pWbGainCtrl[0].B & 0x00ff);
        TxDataBuf[8] = (UINT8)((pWbGainCtrl[0].Gb & 0xff00) >> 8);
        TxDataBuf[9] = (UINT8)(pWbGainCtrl[0].Gb & 0x00ff);

        MN34222_RegWrite(TxDataBuf, 10);

        /* Update current DGC control */
        MN34222Ctrl.CurrentAEInfo.CurrentWbCtrl[0][0] = pWbGainCtrl[0].R;
        MN34222Ctrl.CurrentAEInfo.CurrentWbCtrl[0][1] = pWbGainCtrl[0].Gr;
        MN34222Ctrl.CurrentAEInfo.CurrentWbCtrl[0][2] = pWbGainCtrl[0].Gb;
        MN34222Ctrl.CurrentAEInfo.CurrentWbCtrl[0][3] = pWbGainCtrl[0].B;

    } else if (Chan.Bits.HdrID & 0x2) {

        /* Digital gain & WB gain */
        /* For short exposure frame */
        TxDataBuf[0] = 0x31;
        TxDataBuf[1] = 0x0a;
        TxDataBuf[2] = (UINT8)((pWbGainCtrl[1].Gr & 0xff00) >> 8);
        TxDataBuf[3] = (UINT8)(pWbGainCtrl[1].Gr & 0x00ff);
        TxDataBuf[4] = (UINT8)((pWbGainCtrl[1].R & 0xff00) >> 8);
        TxDataBuf[5] = (UINT8)(pWbGainCtrl[1].R & 0x00ff);
        TxDataBuf[6] = (UINT8)((pWbGainCtrl[1].B & 0xff00) >> 8);
        TxDataBuf[7] = (UINT8)(pWbGainCtrl[1].B & 0x00ff);
        TxDataBuf[8] = (UINT8)((pWbGainCtrl[1].Gb & 0xff00) >> 8);
        TxDataBuf[9] = (UINT8)(pWbGainCtrl[1].Gb & 0x00ff);

        MN34222_RegWrite(TxDataBuf, 10);

        /* Update current DGC control */
        MN34222Ctrl.CurrentAEInfo.CurrentWbCtrl[1][0] = pWbGainCtrl[1].R;
        MN34222Ctrl.CurrentAEInfo.CurrentWbCtrl[1][1] = pWbGainCtrl[1].Gr;
        MN34222Ctrl.CurrentAEInfo.CurrentWbCtrl[1][2] = pWbGainCtrl[1].Gb;
        MN34222Ctrl.CurrentAEInfo.CurrentWbCtrl[1][3] = pWbGainCtrl[1].B;
    }
#endif

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MN34222_SetHdrShutterCtrl
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
static int MN34222_SetHdrShutterCtrl(AMBA_DSP_CHANNEL_ID_u Chan, UINT32 *pShutterCtrl)
{
    MN34222_SetHdrShutterReg(Chan, pShutterCtrl);

    //AmbaPrint("Set Shutter[0]: 0x%08X, Shutter[1]: 0x%08X", *pShutterCtrl, *(pShutterCtrl + 1));

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MN34222_SetSlowShutterCtrl
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
static int MN34222_SetSlowShutterCtrl(AMBA_DSP_CHANNEL_ID_u Chan, UINT32 SlowShutterCtrl)
{
    if (MN34222Ctrl.Status.ModeInfo.HdrInfo.HdrType != 0)
        return NG;

    MN34222_SetSlowShutterReg(SlowShutterCtrl);

    //AmbaPrint("Set SlowShutter: 0x%08X", SlowShutterCtrl);

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MN34222_Config
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
static int MN34222_Config(AMBA_DSP_CHANNEL_ID_u Chan, AMBA_SENSOR_MODE_ID_u Mode, AMBA_SENSOR_ESHUTTER_TYPE_e ElecShutterMode)
{
    AMBA_SENSOR_MODE_INFO_s *pModeInfo = NULL;
    UINT16 SensorMode = Mode.Bits.Mode;
    static UINT8 NotFirstInit = 0;

    if (SensorMode >= AMBA_SENSOR_MN34222_NUM_MODE)
        return NG;

    AmbaPrint("============  [ MN34222 ]  ============");
    AmbaPrint("============ init mode:%d ============", SensorMode);

    MN34222Ctrl.Status.ElecShutterMode = ElecShutterMode;

    pModeInfo  = &MN34222Ctrl.Status.ModeInfo;
    MN34222_PrepareModeInfo(SensorMode, pModeInfo);
    memcpy(&MN34222Ctrl.ModeInfo, pModeInfo, sizeof(AMBA_SENSOR_MODE_INFO_s));

    if (NotFirstInit == 1)
        MN34222_SetStandbyOn();
    else
        NotFirstInit = 1;

    /* Make VIN not to capture bad frames during readout mode transition */
    AmbaVIN_Reset((AMBA_VIN_CHANNEL_e) Chan.Bits.VinID, AMBA_VIN_LVDS, NULL);

    AmbaKAL_TaskSleep(3);

    AmbaPLL_SetSensorClk(pModeInfo->FrameTime.InputClk);

    AmbaKAL_TaskSleep(3);

    MN34222_HardwareReset();

    MN34222_ChangeReadoutMode(SensorMode);
    MN34222_SetStandbyOff();

    /* Reset vin again after vin receive the sensor clock */
    AmbaVIN_Reset((AMBA_VIN_CHANNEL_e) Chan.Bits.VinID, AMBA_VIN_LVDS, NULL);

    MN34222_ConfigVin(Chan, pModeInfo);

    MN34222Ctrl.Status.DevStat.Bits.Sensor0Standby = 0;

    return OK;
}

/*-----------------------------------------------------------------------------------------------*/
AMBA_SENSOR_OBJ_s AmbaSensor_MN34222Obj = {
    .Init                   = MN34222_Init,
    .Enable                 = MN34222_Enable,
    .Disable                = MN34222_Disable,
    .Config                 = MN34222_Config,
    .GetStatus              = MN34222_GetStatus,
    .GetModeInfo            = MN34222_GetModeInfo,
    .GetDeviceInfo          = MN34222_GetDeviceInfo,
    .GetCurrentGainFactor   = MN34222_GetCurrentGainFactor,
    .GetCurrentShutterSpeed = MN34222_GetCurrentShutterSpeed,
    .GetHdrInfo             = MN34222_GetHdrInfo,
    .ConvertGainFactor      = MN34222_ConvertGainFactor,
    .ConvertShutterSpeed    = MN34222_ConvertShutterSpeed,
    .SetAnalogGainCtrl      = MN34222_SetAnalogGainCtrl,
    .SetDigitalGainCtrl     = MN34222_SetDigitalGainCtrl,
    .SetShutterCtrl         = MN34222_SetShutterCtrl,
    .SetSlowShutterCtrl     = MN34222_SetSlowShutterCtrl,
    .ConvertHdrGainFactor   = NULL,
    .ConvertHdrShutterSpeed = NULL,
    .SetHdrAnalogGainCtrl   = MN34222_SetHdrAnalogGainCtrl,
    .SetHdrDigitalGainCtrl  = MN34222_SetHdrDigitalGainCtrl,
    .SetHdrWbGainCtrl       = MN34222_SetHdrWbGainCtrl,
    .SetHdrShutterCtrl      = MN34222_SetHdrShutterCtrl,

    .RegisterRead           = MN34222_RegisterRead,
    .RegisterWrite          = MN34222_RegisterWrite,
};
