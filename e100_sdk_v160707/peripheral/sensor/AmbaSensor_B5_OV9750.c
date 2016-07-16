/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaSensor_B5_OV9750.c
 *
 *  @Copyright      :: Copyright (C) 2013 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Control APIs of OmniVision B5_OV9750 CMOS sensor with MIPI interface
\*-------------------------------------------------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "AmbaDataType.h"
#include "AmbaKAL.h"
#include "AmbaPrintk.h"
#include "AmbaINT.h"
#include "AmbaI2C.h"

#include "AmbaDSP_VIN.h"
#include "AmbaVIN.h"

#include "AmbaGPIO.h"
#include "AmbaPLL.h"
#include "AmbaSensor.h"
#include "AmbaSensor_B5_OV9750.h"
#include "AmbaINT.h"

#include "AmbaB5.h"
#include "AmbaB5_Communicate.h"
#include "AmbaB5_PLL.h"
#include "AmbaB5_VIN.h"
#include "AmbaB5_Prescaler.h"
#include "AmbaB5_VOUT.h"
#include "AmbaB5_SPI.h"
#include "AmbaB5_I2C.h"
#include "AmbaB5_GPIO.h"

#define AMBA_B5_AHB_BASE_ADDR                       0xe0000000
#define AMBA_B5_APB_BASE_ADDR                       0xe8000000

/*-----------------------------------------------------------------------------------------------*\
 * B5_OV9750 sensor runtime info
\*-----------------------------------------------------------------------------------------------*/
B5_OV9750_CTRL_s B5_OV9750Ctrl = {0};

static AMBA_B5_CHANNEL_s B5_OV9750_SysChan;
static AMBA_B5_CHANNEL_s B5_OV9750_CurChan;
static AMBA_B5_PIN_CONFIG_s B5_OV9750_PinConfig = {
    .B5nPinMux = AMBA_B5_PIN_B5N_CTRL_SPI,
    .B5fPinMux = AMBA_B5_PIN_B5F_CTRL_PWM_SINGLE_ENDED,
    .SensorPinMux = AMBA_B5_PIN_SENSOR_CTRL_I2C1,
    .VideoSyncPinMux = AMBA_B5_PIN_VIDEO_SYNC_NONE,
    .SlvsEcType = AMBA_B5_SLVS_EC_1LANE,
};
static AMBA_B5_COMPRESS_RATIO_e B5_OV9750_CompressRatio = AMBA_B5_COMPRESS_5P75;
extern AMBA_KAL_SEM_t AmbaB5_PllGetSwPllSem(void);
static AMBA_KAL_SEM_t SensorAeSem;
static UINT8 SensorSlaveID = 0x6c; /* Sensor slave ID for I2C */

/*-----------------------------------------------------------------------------------------------*\
 * Lane mapping for SoC side VIN (receiving video from B5N)
\*-----------------------------------------------------------------------------------------------*/
static AMBA_DSP_VIN_SLVS_PIN_PAIR_e B5_LvdsLaneMux[AMBA_DSP_NUM_VIN_SLVS_DATA_LANE] = {
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
 * Configurations for SoC side VIN (receiving video from B5N)
\*-----------------------------------------------------------------------------------------------*/
static AMBA_DSP_VIN_SLVS_CONFIG_s B5_VinConfig = {
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
 * Configurations for B5 side VIN (receiving video from OmniVision OV9750)
\*-----------------------------------------------------------------------------------------------*/
static AMBA_B5_VIN_MIPI_CONFIG_s B5_OV9750_MipiVinConfig = {
    .NumDataBits            = 10,
    .DataTypeMask           = 0x00,
    .DataType               = AMBA_B5_VIN_MIPI_RAW10,
    .NumActiveLanes         = 2,
    .RxHvSyncCtrl           = {
        .NumActivePixels    = 0,
        .NumActiveLines     = 0,
        .NumTotalPixels     = 0,
        .NumTotalLines      = 0,
    },
    .VinVoutSync            = {
        {
            .SignalFreq = AMBA_B5_VIN_VOUT_SYNC_FRAME,
            .SignalLine = 1,
        },
        {
            .SignalFreq = AMBA_B5_VIN_VOUT_SYNC_FRAME,
            .SignalLine = 1,
        },
    },
    .MipiCtrl              = {
    .HsSettleTime  = 26,
    .HsTermTime    = 10,
    .ClkSettleTime = 12,
    .ClkTermTime   = 4,
    .ClkMissTime   = 8,
    .RxInitTime    = 30
    }
};

/*-----------------------------------------------------------------------------------------------*\
 * Vout configuration
\*-----------------------------------------------------------------------------------------------*/
static AMBA_B5_VOUT_CONFIG_s B5_VoutConfig = {
    .MaxHBlank = 0,
    .NumDataLane = 0,
    .PixelWidth = 0,
    .InputLinePixel = 0,
    .OutputFrameLine = 0,
    .MinHBlank = 100,
    .MinVBlank = 10,    /* Larger value will cause voutf fifo overflow (resulted in top visual errors) */
};

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: B5_OV9750_B5N_GPIO_Set
 *
 *  @Description:: Set B5N GPIO pin to output heigh
 *
 *  @Input      ::
 *      PinNum:    GPIO pin number
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
void B5_OV9750_B5N_GPIO_Set(UINT32 PinNum)
{
    UINT32 DataBuf[1];
    UINT32 Addr;

    DataBuf[0] = 0x1;
    Addr = AMBA_B5_APB_BASE_ADDR + 0x102c;
    AmbaB5N_RegWrite(Addr, 0, AMBA_B5_DATA_WIDTH_32BIT, 1, DataBuf);

    DataBuf[0] = (UINT32) (1 << PinNum);
    Addr = AMBA_B5_APB_BASE_ADDR + 0x1028;
    AmbaB5N_RegWrite(Addr, 0, AMBA_B5_DATA_WIDTH_32BIT, 1, DataBuf);

    Addr = AMBA_B5_APB_BASE_ADDR + 0x1004;
    AmbaB5N_RegRead(Addr, 0, AMBA_B5_DATA_WIDTH_32BIT, 1, DataBuf);
    DataBuf[0] = DataBuf[0] | (UINT32) (1 << PinNum);
    AmbaB5N_RegWrite(Addr, 0, AMBA_B5_DATA_WIDTH_32BIT, 1, DataBuf);

    DataBuf[0] = (UINT32) (1 << PinNum); // set 1
    Addr = AMBA_B5_APB_BASE_ADDR + 0x1000;
    AmbaB5N_RegWrite(Addr, 0, AMBA_B5_DATA_WIDTH_32BIT, 1, DataBuf);

    DataBuf[0] = 0xffffffff;
    Addr = AMBA_B5_APB_BASE_ADDR + 0x1024;
    AmbaB5N_RegWrite(Addr, 0, AMBA_B5_DATA_WIDTH_32BIT, 1, DataBuf);
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: B5_OV9750_B5N_GPIO_Clr
 *
 *  @Description:: Set B5N GPIO pin to output low
 *
 *  @Input      ::
 *      PinNum:    GPIO pin number
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
void B5_OV9750_B5N_GPIO_Clr(UINT32 PinNum)
{
    UINT32 DataBuf[1];
    UINT32 Addr;

    DataBuf[0] = 0x1;
    Addr = AMBA_B5_APB_BASE_ADDR + 0x102c;
    AmbaB5N_RegWrite(Addr, 0, AMBA_B5_DATA_WIDTH_32BIT, 1, DataBuf);

    DataBuf[0] = (UINT32) (1 << PinNum);
    Addr = AMBA_B5_APB_BASE_ADDR + 0x1028;
    AmbaB5N_RegWrite(Addr, 0, AMBA_B5_DATA_WIDTH_32BIT, 1, DataBuf);

    Addr = AMBA_B5_APB_BASE_ADDR + 0x1004;
    AmbaB5N_RegRead(Addr, 0, AMBA_B5_DATA_WIDTH_32BIT, 1, DataBuf);
    DataBuf[0] = DataBuf[0] | (UINT32) (1 << PinNum);
    AmbaB5N_RegWrite(Addr, 0, AMBA_B5_DATA_WIDTH_32BIT, 1, DataBuf);

    DataBuf[0] = (UINT32) (0 << PinNum); // set 0
    Addr = AMBA_B5_APB_BASE_ADDR + 0x1000;
    AmbaB5N_RegWrite(Addr, 0, AMBA_B5_DATA_WIDTH_32BIT, 1, DataBuf);

    DataBuf[0] = 0xffffffff;
    Addr = AMBA_B5_APB_BASE_ADDR + 0x1024;
    AmbaB5N_RegWrite(Addr, 0, AMBA_B5_DATA_WIDTH_32BIT, 1, DataBuf);
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: B5_OV9750_B5F_GPIO_Set
 *
 *  @Description:: Set B5F GPIO pin to output heigh
 *
 *  @Input      ::
 *      ChipID:    B5F ChipID
 *      PinNum:    GPIO pin number
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
void B5_OV9750_B5F_GPIO_Set(AMBA_B5_CHIP_ID_u ChipID, UINT32 PinNum)
{
    UINT32 DataBuf[1];
    UINT32 Addr;

    ChipID.Data = ChipID.Data & 0xf;

    DataBuf[0] = 0x1;
    Addr = AMBA_B5_APB_BASE_ADDR + 0x102c;
    AmbaB5F_RegWrite(ChipID, Addr, 0, AMBA_B5_DATA_WIDTH_32BIT, 1, DataBuf);

    DataBuf[0] = (UINT32) (1 << PinNum);
    Addr = AMBA_B5_APB_BASE_ADDR + 0x1028;
    AmbaB5F_RegWrite(ChipID, Addr, 0, AMBA_B5_DATA_WIDTH_32BIT, 1, DataBuf);

    Addr = AMBA_B5_APB_BASE_ADDR + 0x1004;
    AmbaB5_RegRead(ChipID.Data, Addr, 0, AMBA_B5_DATA_WIDTH_32BIT, 1, DataBuf);
    DataBuf[0] = DataBuf[0] | (UINT32) (1 << PinNum);
    AmbaB5F_RegWrite(ChipID, Addr, 0, AMBA_B5_DATA_WIDTH_32BIT, 1, DataBuf);

    DataBuf[0] = (UINT32) (1 << PinNum); // set 1
    Addr = AMBA_B5_APB_BASE_ADDR + 0x1000;
    AmbaB5F_RegWrite(ChipID, Addr, 0, AMBA_B5_DATA_WIDTH_32BIT, 1, DataBuf);

    DataBuf[0] = 0xffffffff;
    Addr = AMBA_B5_APB_BASE_ADDR + 0x1024;
    AmbaB5F_RegWrite(ChipID, Addr, 0, AMBA_B5_DATA_WIDTH_32BIT, 1, DataBuf);
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: B5_OV9750_B5F_GPIO_Clr
 *
 *  @Description:: Set B5F GPIO pin to output low
 *
 *  @Input      ::
 *      ChipID:    B5F ChipID
 *      PinNum:    GPIO pin number
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
void B5_OV9750_B5F_GPIO_Clr(AMBA_B5_CHIP_ID_u ChipID, UINT32 PinNum)
{
    UINT32 DataBuf[1];
    UINT32 Addr;

    ChipID.Data = ChipID.Data & 0xf;

    DataBuf[0] = 0x1;
    Addr = AMBA_B5_APB_BASE_ADDR + 0x102c;
    AmbaB5F_RegWrite(ChipID, Addr, 0, AMBA_B5_DATA_WIDTH_32BIT, 1, DataBuf);

    DataBuf[0] = (UINT32) (1 << PinNum);
    Addr = AMBA_B5_APB_BASE_ADDR + 0x1028;
    AmbaB5F_RegWrite(ChipID, Addr, 0, AMBA_B5_DATA_WIDTH_32BIT, 1, DataBuf);

    Addr = AMBA_B5_APB_BASE_ADDR + 0x1004;
    AmbaB5_RegRead(ChipID.Data, Addr, 0, AMBA_B5_DATA_WIDTH_32BIT, 1, DataBuf);
    DataBuf[0] = DataBuf[0] | (UINT32) (1 << PinNum);
    AmbaB5F_RegWrite(ChipID, Addr, 0, AMBA_B5_DATA_WIDTH_32BIT, 1, DataBuf);

    DataBuf[0] = (UINT32) (0 << PinNum); // set 0
    Addr = AMBA_B5_APB_BASE_ADDR + 0x1000;
    AmbaB5F_RegWrite(ChipID, Addr, 0, AMBA_B5_DATA_WIDTH_32BIT, 1, DataBuf);

    DataBuf[0] = 0xffffffff;
    Addr = AMBA_B5_APB_BASE_ADDR + 0x1024;
    AmbaB5F_RegWrite(ChipID, Addr, 0, AMBA_B5_DATA_WIDTH_32BIT, 1, DataBuf);
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: B5_OV9750_PrepareModeInfo
 *
 *  @Description:: Get sensor mode information
 *
 *  @Input      ::
 *      SensorCount: B5 ouput channel number
 *      Mode:   Sensor readout mode number
 *
 *  @Output     ::
 *      pModeInfo:  Details of the specified readout mode
 *      pSensorPrivate: Sensor frame time configuration
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static void B5_OV9750_PrepareModeInfo(UINT8 SensorCount, UINT16 Mode, AMBA_SENSOR_MODE_INFO_s *pModeInfo, B5_OV9750_MODE_INFO_s *pPrivateModeInfo)
{
    const AMBA_SENSOR_INPUT_INFO_s  *pInputInfo= &B5_OV9750InputInfo[B5_OV9750ModeInfoList[Mode].InputMode];
    const AMBA_SENSOR_OUTPUT_INFO_s *pOutputInfo= &B5_OV9750OutputInfo[B5_OV9750ModeInfoList[Mode].OutputMode];

    if (pPrivateModeInfo == NULL)
        pPrivateModeInfo = (B5_OV9750_MODE_INFO_s *) &B5_OV9750ModeInfoList[Mode];
    else
        memcpy(pPrivateModeInfo, &B5_OV9750ModeInfoList[Mode], sizeof(B5_OV9750_MODE_INFO_s));

    pModeInfo->Mode.Data = Mode;
    pModeInfo->LineLengthPck = pPrivateModeInfo->FrameTiming.Linelengthpck;
    pModeInfo->FrameLengthLines = pPrivateModeInfo->FrameTiming.FrameLengthLines;
    pModeInfo->NumExposureStepPerFrame = pPrivateModeInfo->FrameTiming.FrameLengthLines;
    pModeInfo->PixelRate = (float) pOutputInfo->DataRate * pOutputInfo->NumDataLanes / pOutputInfo->NumDataBits;
    pModeInfo->RowTime = (float) pPrivateModeInfo->FrameTiming.FrameRate.NumUnitsInTick / pPrivateModeInfo->FrameTiming.FrameRate.TimeScale / pPrivateModeInfo->FrameTiming.FrameLengthLines;
    pModeInfo->InternalExposureOffset = 0;
    pModeInfo->FrameTime.InputClk = pPrivateModeInfo->FrameTiming.InputClk;

    memcpy(&pModeInfo->FrameTime.FrameRate, &pPrivateModeInfo->FrameTiming.FrameRate, sizeof(AMBA_DSP_FRAME_RATE_s));
    memcpy(&pModeInfo->MinFrameRate, &pPrivateModeInfo->FrameTiming.FrameRate, sizeof(AMBA_DSP_FRAME_RATE_s));
    memcpy(&pModeInfo->InputInfo, pInputInfo, sizeof(AMBA_SENSOR_INPUT_INFO_s));
    memcpy(&pModeInfo->OutputInfo, pOutputInfo, sizeof(AMBA_SENSOR_OUTPUT_INFO_s));

    pModeInfo->OutputInfo.DataRate = B5_OV9750_B5nOutputInfo[0].DataRate;
    pModeInfo->OutputInfo.NumDataLanes = (SensorCount << 1);
    pModeInfo->OutputInfo.OutputWidth = SensorCount * pPrivateModeInfo->CfaOutputWidth;
    pModeInfo->OutputInfo.OutputHeight = pModeInfo->OutputInfo.RecordingPixels.Height;
    pModeInfo->OutputInfo.RecordingPixels.StartX = 0;
    pModeInfo->OutputInfo.RecordingPixels.StartY = 0;
    pModeInfo->OutputInfo.RecordingPixels.Width = pModeInfo->OutputInfo.OutputWidth;
    pModeInfo->OutputInfo.RecordingPixels.Height = pModeInfo->OutputInfo.OutputHeight;

    /* HDR information */
    memset(&pModeInfo->HdrInfo, 0, sizeof(AMBA_SENSOR_HDR_INFO_s)); // Not support HDR

    /* Only support 1 stage slow shutter */
    pModeInfo->MinFrameRate.TimeScale /= 2;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: B5_OV9750_ConfigVin
 *
 *  @Description:: Configure VIN to receive output frames of the new readout mode
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
static void B5_OV9750_ConfigVin(AMBA_DSP_CHANNEL_ID_u Chan, AMBA_SENSOR_MODE_INFO_s *pModeInfo, B5_OV9750_MODE_INFO_s *pPrivateModeInfo)
{
    AMBA_B5_VIN_WINDOW_s B5CropWindow;
    AMBA_SENSOR_OUTPUT_INFO_s *pImgInfo;
    AMBA_B5_VIN_MIPI_CONFIG_s *pB5MipiVinCfg = &B5_OV9750_MipiVinConfig;
    AMBA_DSP_VIN_SLVS_CONFIG_s *pVinCfg = &B5_VinConfig;
    UINT32 VoutHBlank;
    UINT32 BorderWidth = 0;  /* TODO */
    UINT32 FrameNum;
    UINT32 LaneNum ;
    UINT32 LvdsPixelNum; /* number of pixels per lane during a row period */

    /* Config B5 side VOUTF */
    pImgInfo = &pModeInfo->OutputInfo;

    FrameNum = pImgInfo->NumDataLanes / 2;  /* TODO: number of voutf input source */
    LaneNum = pImgInfo->NumDataLanes;  /* TODO */
    LvdsPixelNum = (UINT32)((double)pImgInfo->DataRate / pImgInfo->NumDataBits * pModeInfo->RowTime);
    VoutHBlank = LvdsPixelNum - 8 - (UINT32)((pPrivateModeInfo->CfaOutputWidth * FrameNum + BorderWidth * (FrameNum - 1)) / LaneNum + 1);

    B5_VoutConfig.InputLinePixel = pPrivateModeInfo->CfaOutputWidth;
    B5_VoutConfig.OutputFrameLine = pImgInfo->OutputHeight;
    B5_VoutConfig.PixelWidth = pImgInfo->NumDataBits;
    B5_VoutConfig.NumDataLane = LaneNum;  /* TODO */
    B5_VoutConfig.Channel[0] = (B5_OV9750_CurChan.SensorID & 1) ? B5_OV9750_CurChan.Active[0] : B5_OV9750_CurChan.Inactive[0];
    B5_VoutConfig.Channel[1] = (B5_OV9750_CurChan.SensorID & 2) ? B5_OV9750_CurChan.Active[1] : B5_OV9750_CurChan.Inactive[1];
    B5_VoutConfig.Channel[2] = (B5_OV9750_CurChan.SensorID & 4) ? B5_OV9750_CurChan.Active[2] : B5_OV9750_CurChan.Inactive[2];
    B5_VoutConfig.Channel[3] = (B5_OV9750_CurChan.SensorID & 8) ? B5_OV9750_CurChan.Active[3] : B5_OV9750_CurChan.Inactive[3];
    B5_VoutConfig.MinVBlank = pPrivateModeInfo->FrameTiming.FrameLengthLines - pImgInfo->OutputHeight;
    B5_VoutConfig.MinHBlank = 1 + (UINT32)(VoutHBlank / 2.0 - 1);
    B5_VoutConfig.MaxHBlank = (UINT32)((VoutHBlank + LvdsPixelNum) + 1);
    B5_VoutConfig.CompressRatio = B5_OV9750_CompressRatio;

    AmbaB5_VoutConfig(&B5_VoutConfig);
    AmbaB5_VoutEnable();

    /* Config B5 side VIN */
    pImgInfo = (AMBA_SENSOR_OUTPUT_INFO_s *)&B5_OV9750OutputInfo[pPrivateModeInfo->OutputMode];

    pB5MipiVinCfg->RxHvSyncCtrl.NumActivePixels = pImgInfo->OutputWidth;
    pB5MipiVinCfg->RxHvSyncCtrl.NumActiveLines= pImgInfo->OutputHeight;

    pB5MipiVinCfg->NumDataBits = pImgInfo->NumDataBits;
    pB5MipiVinCfg->NumActiveLanes = pImgInfo->NumDataLanes;

    B5CropWindow.StartX = pImgInfo->RecordingPixels.StartX;
    B5CropWindow.StartY = pImgInfo->RecordingPixels.StartY;
    B5CropWindow.EndX   = pImgInfo->RecordingPixels.Width + pImgInfo->RecordingPixels.StartX - 1;
    B5CropWindow.EndY   = pImgInfo->RecordingPixels.Height + pImgInfo->RecordingPixels.StartY - 1;

    AmbaB5_VinConfigMIPI(&B5_OV9750_CurChan, pB5MipiVinCfg);
    AmbaB5_VinCaptureConfig(&B5_OV9750_CurChan, &B5CropWindow);

    /* Config SoC side VIN */
    AmbaDSP_VinPhySetSLVS((AMBA_VIN_CHANNEL_e)Chan.Bits.VinID);

    pImgInfo = &pModeInfo->OutputInfo;
    pVinCfg->Info.NumDataBits = pImgInfo->NumDataBits;
    pVinCfg->NumActiveLanes = pImgInfo->NumDataLanes;
    pVinCfg->pLaneMapping = B5_LvdsLaneMux;
    pVinCfg->RxHvSyncCtrl.NumActivePixels = pImgInfo->OutputWidth;
    pVinCfg->RxHvSyncCtrl.NumActiveLines = pImgInfo->OutputHeight;
    pVinCfg->RxHvSyncCtrl.NumTotalPixels = pImgInfo->OutputWidth + B5_VoutConfig.MinHBlank - 1;
    pVinCfg->RxHvSyncCtrl.NumTotalLines = pModeInfo->FrameLengthLines;
    pVinCfg->Info.BayerPattern = (AMBA_DSP_SENSOR_PATTERN_e) pImgInfo->CfaPattern;

    AmbaPrint("pImgInfo->OutputWidth       = %d ", pImgInfo->OutputWidth);
    AmbaPrint("pImgInfo->OutputHeight      = %d ", pImgInfo->OutputHeight);
    AmbaPrint("pModeInfo->LineLengthPck    = %d ", pModeInfo->LineLengthPck);
    AmbaPrint("pModeInfo->FrameLengthLines = %d ", pModeInfo->FrameLengthLines);

    memcpy(&pVinCfg->Info.FrameRate, &pPrivateModeInfo->FrameTiming.FrameRate, sizeof(AMBA_DSP_FRAME_RATE_s));
    memcpy(&pVinCfg->Info.DspPhaseShift, &pImgInfo->DspPhaseShift, sizeof(AMBA_DSP_PHASE_SHIFT_CTRL_s));

    AmbaDSP_VinConfigSLVS((AMBA_VIN_CHANNEL_e)Chan.Bits.VinID, pVinCfg);
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: B5_OV9750_HardwareReset
 *
 *  @Description:: Reset B5_OV9750
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static inline void B5_OV9750_HardwareReset(AMBA_DSP_CHANNEL_ID_u Chan)
{
    /* For Second VIN */
    if (Chan.Bits.VinID == 1) {
        /* B5N GPIO 8 is for B5F power supply */
        /* GPIO_PIN_19 is for B5N reset */
        AmbaGPIO_ConfigOutput(GPIO_PIN_19, 0);
        AmbaKAL_TaskSleep(5);
        B5_OV9750_B5N_GPIO_Clr(30);
        AmbaKAL_TaskSleep(30);
        AmbaGPIO_ConfigOutput(GPIO_PIN_19, 1);
        AmbaKAL_TaskSleep(300);
        B5_OV9750_B5N_GPIO_Set(30);

    /* For Main VIN */
    } else {
        /* GPIO_PIN_19 is for B5F power supply */
        /* GPIO_PIN_21 is for B5N reset */
        AmbaGPIO_ConfigOutput(GPIO_PIN_21, 0);
        AmbaKAL_TaskSleep(5);
        AmbaGPIO_ConfigOutput(GPIO_PIN_19, 0);
        AmbaKAL_TaskSleep(30);
        AmbaGPIO_ConfigOutput(GPIO_PIN_21, 1);
        AmbaKAL_TaskSleep(300);
        AmbaGPIO_ConfigOutput(GPIO_PIN_19, 1);
    }

    AmbaKAL_TaskSleep(500); // Need to wait for a while before programing B5F/B5N
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: B5_OV9750_SensorHardwareReset
 *
 *  @Description:: Reset Sensors
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static void B5_OV9750_SensorHardwareReset(void)
{
    AMBA_B5_CHIP_ID_u ChipID;
    ChipID.Data = B5_OV9750_CurChan.SensorID;

    /* Sensor reset pin low */
    B5_OV9750_B5F_GPIO_Clr(ChipID, 31);

    AmbaKAL_TaskSleep(10);

    /* Sensor reset pin heigh */
    B5_OV9750_B5F_GPIO_Set(ChipID, 31);

    AmbaKAL_TaskSleep(10);
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: B5_OV9750_RegWrite
 *
 *  @Description:: Write sensor registers through I2C bus
 *
 *  @Input      ::
 *      pB5Chan:   Vin ID and sensor ID
 *      pTxData:   Pointer to Write data buffer
 *      Size:      Number of Write data
 *
 *  @Output     :: none
 *
 *  @Return     :: none
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static int B5_OV9750_RegWrite(AMBA_B5_CHANNEL_s *pB5Chan, UINT8 *pTxDataBuf, UINT8 DataSize)
{
    AMBA_B5_I2C_CHANNEL_e I2cChanNo = AMBA_B5_I2C_CHANNEL1;
    UINT32 SlaveAddr = SensorSlaveID;
    UINT32 RegAddr;

    RegAddr = pTxDataBuf[0];
    RegAddr = (RegAddr << 8) | pTxDataBuf[1];

    AmbaKAL_SemTake(&SensorAeSem, AMBA_KAL_WAIT_FOREVER);

    // AmbaB5_I2cWrite(pB5Chan, I2cChanNo, AMBA_B5_I2C_SPEED_FAST, SlaveAddr, DataSize, pTxDataBuf);
    AmbaB5_I2cBurstWrite(pB5Chan, I2cChanNo, AMBA_B5_I2C_SPEED_FAST, SlaveAddr, DataSize, pTxDataBuf);

    AmbaKAL_SemGive(&SensorAeSem);

    //Debug usage
    //for (i = 0; i < (DataSize - 2); i++)
        //AmbaPrint("[Write]ChipID = 0x%02x, Addr = 0x%04x, Data = 0x%02X",
                   //pB5Chan->SensorID, RegAddr + (UINT16)i, pTxDataBuf[2+i]);

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: B5_OV9750_RegRead
 *
 *  @Description:: Read sensor registers through I2C bus
 *
 *  @Input      ::
 *      pB5Chan:   Vin ID and sensor ID
 *      RegAddr:   Sensor Address
 *      pRxData:   Pointer to Read data buffer
 *
 *  @Output     :: none
 *
 *  @Return     :: none
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
int B5_OV9750_RegRead(AMBA_B5_CHANNEL_s *pB5Chan, UINT16 RegAddr, UINT8 *pRxDataBuf)
{
    AMBA_B5_I2C_TRANSACTION_s I2cTransaction[2];
    AMBA_B5_I2C_CHANNEL_e I2cChanNo = AMBA_B5_I2C_CHANNEL1;
    AMBA_B5_CHANNEL_e RxChan = AMBA_B5_CHANNEL0;

    UINT32 SlaveAddr = SensorSlaveID;
    UINT8 TxDataBuf[3];
    UINT8 RxDataBuf[3];
    UINT8 DataSize = 3;

    if (pB5Chan->SensorID & 0xf) {
        if (pB5Chan->SensorID == 1)
            RxChan = AMBA_B5_CHANNEL0;
        else if (pB5Chan->SensorID == 2)
            RxChan = AMBA_B5_CHANNEL1;
        else if (pB5Chan->SensorID == 4)
            RxChan = AMBA_B5_CHANNEL2;
        else if (pB5Chan->SensorID == 8)
            RxChan = AMBA_B5_CHANNEL3;
    }

    /* Fill the data buffer */
    TxDataBuf[0] = RegAddr >> 8;;
    TxDataBuf[1] = RegAddr & 0xff;

    I2cTransaction[0].SlaveAddr = SlaveAddr;
    I2cTransaction[0].DataSize = 2;
    I2cTransaction[0].pDataBuf = TxDataBuf;
    I2cTransaction[1].SlaveAddr = SlaveAddr | 0x1;
    I2cTransaction[1].DataSize = DataSize - 2;
    I2cTransaction[1].pDataBuf = RxDataBuf;

    AmbaKAL_SemTake(&SensorAeSem, AMBA_KAL_WAIT_FOREVER);

    AmbaB5_I2cReadAfterWrite(pB5Chan, I2cChanNo, AMBA_B5_I2C_SPEED_FAST, 1,
                             &I2cTransaction[0], &I2cTransaction[1], RxChan);

    AmbaKAL_SemGive(&SensorAeSem);

    *pRxDataBuf = RxDataBuf[0];

    //Debug usage
    //AmbaPrint("[Read]ChipID = 0x%02x, Addr = 0x%04x, Data = 0x%02X",
               //pB5Chan->SensorID, RegAddr, RxDataBuf[0]);

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: B5_OV9750_Write
 *
 *  @Description:: Write sensor registers through I2C bus
 *
 *  @Input      ::
 *      pB5Chan:   Vin ID and sensor ID
 *      Addr:      Register Address
 *      Data:      Write data
 *
 *  @Output     :: none
 *
 *  @Return     :: none
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static int B5_OV9750_Write(AMBA_B5_CHANNEL_s *pB5Chan, UINT16 Addr, UINT8 Data)
{
    UINT8 TxData[3];

    TxData[0] = Addr >> 8;
    TxData[1] = Addr & 0xff;
    TxData[2] = Data;

    B5_OV9750_RegWrite(pB5Chan, TxData, 3);

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: B5_OV9750_RegisterWrite
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
static int B5_OV9750_RegisterWrite(AMBA_DSP_CHANNEL_ID_u Chan, UINT16 Addr, UINT16 Data)
{
    UINT8 TxData[3];

    TxData[0] = Addr >> 8;
    TxData[1] = Addr & 0xff;
    TxData[2] = Data;

    B5_OV9750_CurChan.SensorID = Chan.Bits.SensorID;
    B5_OV9750_RegWrite(&B5_OV9750_CurChan, TxData, 3);

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: B5_OV9750_RegisterRead
 *
 *  @Description:: Read sensor registers API
 *
 *  @Input      ::
 *      Chan:   Vin ID and sensor ID
 *      Addr:   Register address
 *      pData:  pointer to Rx data buffer
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *      int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static int B5_OV9750_RegisterRead(AMBA_DSP_CHANNEL_ID_u Chan, UINT16 Addr, UINT16 *pData)
{
    UINT8 Data;

    B5_OV9750_CurChan.SensorID = Chan.Bits.SensorID;
    B5_OV9750_RegRead(&B5_OV9750_CurChan, Addr, &Data);

    *pData = Data;

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: B5_OV9750_SetStandbyOn
 *
 *  @Description:: Configure sensor into standby mode
 *
 *  @Input      :: none
 *      pB5Chan:   Vin ID and sensor ID
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static void B5_OV9750_SetStandbyOn(AMBA_B5_CHANNEL_s *pB5Chan)
{
    UINT32 Delay;

    Delay = B5_OV9750Ctrl.ModeInfo.FrameTiming.FrameRate.TimeScale /
            B5_OV9750Ctrl.ModeInfo.FrameTiming.FrameRate.NumUnitsInTick;

    B5_OV9750_Write(pB5Chan, 0x0100, 0x00);

    AmbaKAL_TaskSleep(Delay);
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: B5_OV9750_SetStandbyOff
 *
 *  @Description:: Configure sensor to leave standby mode.
 *
 *  @Input      ::
 *      pB5Chan:   Vin ID and sensor ID
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static void B5_OV9750_SetStandbyOff(AMBA_B5_CHANNEL_s *pB5Chan)
{
    B5_OV9750_Write(pB5Chan, 0x0100, 0x01);
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: B5_OV9750_ChangeReadoutMode
 *
 *  @Description:: Switch sensor to requested mode.
 *                 It's for the case that sensor have been initialized.
 *
 *  @Input      ::
 *      pB5Chan:   Vin ID and sensor ID
 *      Mode:      Sensor mode ID
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static int B5_OV9750_ChangeReadoutMode(AMBA_B5_CHANNEL_s *pB5Chan, UINT16 Mode)
{
    int i;

    /* Sensor software reset */
    B5_OV9750_Write(pB5Chan, 0x0103, 0x01);

    AmbaKAL_TaskSleep(10); // delay 10ms after software reset

    for (i = 0; i < B5_OV9750_NUM_READOUT_MODE_REG; i ++)
        B5_OV9750_Write(pB5Chan, B5_OV9750RegTable[i].Addr, B5_OV9750RegTable[i].Data[B5_OV9750ModeInfoList[Mode].ReadoutMode]);

    /* Reset current AE information */
    memset(&B5_OV9750Ctrl.CurrentAEInfo, 0, sizeof(B5_OV9750_CURRENT_AE_INFO_s));

    return OK;
}

#if 0
/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: B5_OV9750_VinHookFunc
 *
 *  @Description:: Hook this callback function as VIN ISR of Ambarella SoC
 *
 *  @Input      ::
 *      EntryArg:  Dummy argument
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static void B5_OV9750_VinHookFunc(int Entry) {
    AmbaB5_PllSwPllVinHookFunc((UINT32)Entry);
}
#endif

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: B5_OV9750_SysChanConvert
 *
 *  @Description:: Convert B5_OV9750_SysChan to mw configuration
 *
 *  @Input      ::
 *      Chan: Vin ID and sensor ID
 *
 *  @Output     :: none
 *      pB5Chan: Vin ID and sensor ID
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static void B5_OV9750_SysChanConvert(AMBA_DSP_CHANNEL_ID_u Chan, AMBA_B5_CHANNEL_s *pB5Chan)
{
    int i;

    pB5Chan->SensorID = 0;

    for (i = 0; i< AMBA_NUM_B5_CHANNEL; i++) {
        if (Chan.Bits.SensorID & (1 << i)) {
            pB5Chan->Active[i] = AMBA_B5_CHANNEL_FAR_END; // Fixed to far end sensor input
            //pB5Chan->Inactive[i] = AMBA_B5_CHANNEL_INTERNAL; // enable internal test pattern if there is sensor input
            pB5Chan->SensorID |= (1 << i);
        } else {
            pB5Chan->Active[i] = AMBA_B5_CHANNEL_DISABLED;
            pB5Chan->Inactive[i] = AMBA_B5_CHANNEL_DISABLED;
        }
    }
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: B5_OV9750_CurChanConvert
 *
 *  @Description:: Convert B5_OV9750_CurChan to current configuration
 *
 *  @Input      ::
 *      Chan: Vin ID and sensor ID
 *
 *  @Output     :: none
 *      pB5Chan: Vin ID and sensor ID
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static void B5_OV9750_CurChanConvert(AMBA_DSP_CHANNEL_ID_u Chan, AMBA_B5_CHANNEL_s *pB5Chan)
{
    int i;

    pB5Chan->SensorID = 0;

    for (i = 0; i< AMBA_NUM_B5_CHANNEL; i++) {
        if (Chan.Bits.SensorID & (1 << i) & B5_OV9750_SysChan.SensorID) {
            pB5Chan->Active[i] = B5_OV9750_SysChan.Active[i];
            pB5Chan->SensorID |= (1 << i);
        } else
            pB5Chan->Inactive[i] = B5_OV9750_SysChan.Inactive[i];
    }
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: B5_OV9750_Init
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
static int B5_OV9750_Init(AMBA_DSP_CHANNEL_ID_u Chan)
{
    /* For Second VIN */
    if (Chan.Bits.VinID == 1) {
        AmbaPLL_SetEnetClkConfig(1);
        AmbaPLL_SetEnetClk(24000000);

    /* For Main VIN */
    } else {
        AmbaPLL_SetSensorClk(24000000);
        AmbaPLL_SetSensorClkDir(AMBA_PLL_SENSOR_CLK_OUTPUT);
    }

    B5_OV9750_SysChanConvert(Chan, &B5_OV9750_SysChan); // Convert SysChan

#if 0 /* TBD */
    if (B5_OV9750_PinConfig.B5fPinMux == AMBA_B5_PIN_B5F_CTRL_PWM_SINGLE_ENDED) {
       AmbaINT_IsrHook(AMBA_VIC_INT_ID53_IDSP_PIP_VSYNC_SLAVE, AMBA_VIC_INT_ID53_IDSP_PIP_VSYNC_SLAVE, AMBA_VIC_INT_RISING_EDGE_TRIGGER, B5_OV9750_VinHookFunc);
       AmbaINT_IntEnable(AMBA_VIC_INT_ID53_IDSP_PIP_VSYNC_SLAVE);
    }
#endif

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: B5_OV9750_Enable
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
static int B5_OV9750_Enable(AMBA_DSP_CHANNEL_ID_u Chan)
{
    if (B5_OV9750Ctrl.Status.DevStat.Bits.Sensor0Power == 1)
        return OK;

    B5_OV9750Ctrl.Status.DevStat.Bits.Sensor0Power   = 1;
    B5_OV9750Ctrl.Status.DevStat.Bits.Sensor0Standby = 1;

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: B5_OV9750_Disable
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
static int B5_OV9750_Disable(AMBA_DSP_CHANNEL_ID_u Chan)
{
    if (B5_OV9750Ctrl.Status.DevStat.Bits.Sensor0Power == 0)
        return OK;

    B5_OV9750_CurChan.SensorID = Chan.Bits.SensorID;

    B5_OV9750_SetStandbyOn(&B5_OV9750_CurChan);

    B5_OV9750Ctrl.Status.DevStat.Bits.Sensor0Power = 0;

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: B5_OV9750_GetStatus
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
static int B5_OV9750_GetStatus(AMBA_DSP_CHANNEL_ID_u Chan, AMBA_SENSOR_STATUS_INFO_s *pStatus)
{
    if (pStatus == NULL || B5_OV9750Ctrl.Status.DevStat.Bits.Sensor0Power == 0)
        return NG;

    B5_OV9750Ctrl.Status.TimeToAllPelExposed = 0;  /* TODO: for flash light or mechanical shutter control */

    memcpy(pStatus, &B5_OV9750Ctrl.Status, sizeof(AMBA_SENSOR_STATUS_INFO_s));

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: B5_OV9750_GetModeInfo
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
static int B5_OV9750_GetModeInfo(AMBA_DSP_CHANNEL_ID_u Chan, AMBA_SENSOR_MODE_ID_u Mode, AMBA_SENSOR_MODE_INFO_s *pModeInfo)
{
    AMBA_B5_CHANNEL_s B5Chan;
    UINT16 SensorMode = Mode.Bits.Mode;

    if (SensorMode == AMBA_SENSOR_CURRENT_MODE)
        SensorMode = B5_OV9750Ctrl.Status.ModeInfo.Mode.Bits.Mode;

    if (SensorMode >= AMBA_SENSOR_B5_OV9750_NUM_MODE || pModeInfo == NULL)
        return NG;

    B5_OV9750_SysChanConvert(Chan, &B5Chan);
    B5_OV9750_PrepareModeInfo(AmbaB5_GetNumActiveChannel(&B5Chan), SensorMode, pModeInfo, NULL);

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: B5_OV9750_GetDeviceInfo
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
static int B5_OV9750_GetDeviceInfo(AMBA_DSP_CHANNEL_ID_u Chan, AMBA_SENSOR_DEVICE_INFO_s *pDeviceInfo)
{
    if (pDeviceInfo == NULL)
        return NG;

    memcpy(pDeviceInfo, &B5_OV9750DeviceInfo, sizeof(AMBA_SENSOR_DEVICE_INFO_s));

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: B5_OV9750_GetCurrentGainFactor
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
static int B5_OV9750_GetCurrentGainFactor(AMBA_DSP_CHANNEL_ID_u Chan, float *pGainFactor)
{
    UINT8 CurrentB5ChannelID = 0;
    UINT16 RealAgcGain;
    UINT16 RealWBGain;

    /* Need to select a specific B5 channel id for getting current gain factor */
    if ((Chan.Bits.SensorID != 0x1) && (Chan.Bits.SensorID != 0x2) && (Chan.Bits.SensorID != 0x4) && (Chan.Bits.SensorID != 0x8))

    CurrentB5ChannelID = (UINT8)log2((float)Chan.Bits.SensorID);

    RealAgcGain = (B5_OV9750Ctrl.CurrentAEInfo.CurrentAgcCtrl[CurrentB5ChannelID] & 0xffff0000) >> 16;
    RealWBGain = B5_OV9750Ctrl.CurrentAEInfo.CurrentAgcCtrl[CurrentB5ChannelID] & 0x0000ffff;

    *pGainFactor = ((float)RealAgcGain / 128) * ((float)RealWBGain / 1024);

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: B5_OV9750_GetCurrentShutterSpeed
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
static int B5_OV9750_GetCurrentShutterSpeed(AMBA_DSP_CHANNEL_ID_u Chan, float *pExposureTime)
{
    UINT8 CurrentB5ChannelID = 0;

    /* Need to select a specific B5 channel id for getting current exposure time */
    if ((Chan.Bits.SensorID != 0x1) && (Chan.Bits.SensorID != 0x2) && (Chan.Bits.SensorID != 0x4) && (Chan.Bits.SensorID != 0x8))
        return NG;

    CurrentB5ChannelID = (UINT8)log2((float)Chan.Bits.SensorID);

    *pExposureTime = B5_OV9750Ctrl.Status.ModeInfo.RowTime * B5_OV9750Ctrl.CurrentAEInfo.CurrentShutterCtrl[CurrentB5ChannelID];

    //AmbaPrint("ExposureTime = %f", *pExposureTime);

    return OK;
}


/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: B5_OV9750_ConvertGainFactor
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
static int B5_OV9750_ConvertGainFactor(AMBA_DSP_CHANNEL_ID_u Chan, float DesiredFactor, UINT32 *pActualFactor, UINT32 *pAnalogGainCtrl, UINT32 *pDigitalGainCtrl)
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
 *  @RoutineName:: B5_OV9750_ConvertShutterSpeed
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
static int B5_OV9750_ConvertShutterSpeed(AMBA_DSP_CHANNEL_ID_u Chan, float ExposureTime, UINT32 *pShutterCtrl)
{
    AMBA_SENSOR_MODE_INFO_s *pModeInfo = &B5_OV9750Ctrl.Status.ModeInfo;

    if (pShutterCtrl == NULL)
        return NG;

    *pShutterCtrl = (UINT32)(ExposureTime / pModeInfo->RowTime);

    if (*pShutterCtrl <= 1)
        *pShutterCtrl = 1;

    //AmbaPrint("ExposureTime:%f, ShutterCtrl:%d", ExposureTime, *pShutterCtrl);

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: B5_OV9750_SetAnalogGainCtrl
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
static int B5_OV9750_SetAnalogGainCtrl(AMBA_DSP_CHANNEL_ID_u Chan, UINT32 AnalogGainCtrl)
{
    UINT8 AgcR3508 = (AnalogGainCtrl & 0xff000000) >> 24;
    UINT8 AgcR3509 = (AnalogGainCtrl & 0x00ff0000) >> 16;
    UINT8 WbMsb = (AnalogGainCtrl & 0x0000ff00) >> 8;
    UINT8 WbLsb = (AnalogGainCtrl & 0x000000ff);
    UINT8 R366A = 0;
    UINT8 TxData[10];
    int i;

    B5_OV9750_CurChan.SensorID = Chan.Bits.SensorID;

    if (AgcR3508 >= 0x4)
        R366A = 0x7;
    else if (AgcR3508 >= 0x2)
        R366A = 0x3;
    else if (AgcR3508 >= 0x1)
        R366A = 0x1;
    else
        R366A = 0x0;

    /* Group start */
    TxData[0] = 0x32;
    TxData[1] = 0x08;
    TxData[2] = 0x00;
    B5_OV9750_RegWrite(&B5_OV9750_CurChan, TxData, 3);

    /* Sensor AGC gain */
    TxData[0] = 0x35;
    TxData[1] = 0x08;
    TxData[2] = AgcR3508;
    TxData[3] = AgcR3509;
    B5_OV9750_RegWrite(&B5_OV9750_CurChan, TxData, 4);

    /* Sensor DGC gain */
    TxData[0] = 0x50;
    TxData[1] = 0x32;
    TxData[2] = WbMsb;
    TxData[3] = WbLsb;
    TxData[4] = WbMsb;
    TxData[5] = WbLsb;
    TxData[6] = WbMsb;
    TxData[7] = WbLsb;
    B5_OV9750_RegWrite(&B5_OV9750_CurChan, TxData, 8);

    TxData[0] = 0x36;
    TxData[1] = 0x6a;
    TxData[2] = R366A;
    B5_OV9750_RegWrite(&B5_OV9750_CurChan, TxData, 3);

    /* Group end */
    TxData[0] = 0x32;
    TxData[1] = 0x08;
    TxData[2] = 0x10;
    B5_OV9750_RegWrite(&B5_OV9750_CurChan, TxData, 3);

    /* Group delay latch */
    TxData[0] = 0x32;
    TxData[1] = 0x08;
    TxData[2] = 0xa0;
    B5_OV9750_RegWrite(&B5_OV9750_CurChan, TxData, 3);

    /* Update current AGC control */
    for(i = 0; i < AMBA_NUM_B5_CHANNEL; i++) {
        if (B5_OV9750_CurChan.SensorID & (1 << i))
            B5_OV9750Ctrl.CurrentAEInfo.CurrentAgcCtrl[i] = AnalogGainCtrl;
    }

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: B5_OV9750_SetShutterCtrl
 *
 *  @Description:: Get Sensor Device Info
 *
 *  @Input      ::
 *      Chan:        Vin ID and sensor ID
 *      ShutterCtrl: Electronic shutter control
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static int B5_OV9750_SetShutterCtrl(AMBA_DSP_CHANNEL_ID_u Chan, UINT32 ShutterCtrl)
{
    AMBA_SENSOR_MODE_INFO_s *pModeInfo = &B5_OV9750Ctrl.Status.ModeInfo;
    UINT8 TxData[5];
    int i;

    B5_OV9750_CurChan.SensorID = Chan.Bits.SensorID;

    /* Exposure line needs be smaller than VTS - 8 */
    if (ShutterCtrl >= pModeInfo->NumExposureStepPerFrame - 8)
        ShutterCtrl = pModeInfo->NumExposureStepPerFrame - 8;

    TxData[0] = 0x3500 >> 8;
    TxData[1] = 0x3500 & 0xff;
    TxData[2] = (ShutterCtrl & 0x000ff000) >> 12;
    TxData[3] = (ShutterCtrl & 0x00000ff0) >> 4;
    TxData[4] = (ShutterCtrl & 0x0000000f) << 4;

    B5_OV9750_RegWrite(&B5_OV9750_CurChan, TxData, 5);

    /* Update current shutter control */
    for(i = 0; i < AMBA_NUM_B5_CHANNEL; i++) {
        if (B5_OV9750_CurChan.SensorID & (1 << i))
            B5_OV9750Ctrl.CurrentAEInfo.CurrentShutterCtrl[i] = ShutterCtrl;
    }

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: B5_OV9750_SetSlowShutterCtrl
 *
 *  @Description:: set slow shutter control
 *
 *  @Input      ::
 *      Chan:           Vin ID and sensor ID
 *      SlowShutterCtrl:   Electronic slow shutter control
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static int B5_OV9750_SetSlowShutterCtrl(AMBA_DSP_CHANNEL_ID_u Chan, UINT32 SlowShutterCtrl)
{
    UINT32 TargetFrameLengthLines;
    UINT8 TxData[4];

    if (SlowShutterCtrl < 1)
        SlowShutterCtrl = 1;

    TargetFrameLengthLines = B5_OV9750Ctrl.ModeInfo.FrameTiming.FrameLengthLines * SlowShutterCtrl;

    /* Update frame rate information */
    B5_OV9750Ctrl.Status.ModeInfo.FrameLengthLines = TargetFrameLengthLines;
    B5_OV9750Ctrl.Status.ModeInfo.NumExposureStepPerFrame = TargetFrameLengthLines;
    B5_OV9750Ctrl.Status.ModeInfo.FrameTime.FrameRate.TimeScale = B5_OV9750Ctrl.ModeInfo.FrameTiming.FrameRate.TimeScale / SlowShutterCtrl;

    B5_OV9750_CurChan.SensorID = Chan.Bits.SensorID;

    TxData[0] = 0x380e >> 8;
    TxData[1] = 0x380e & 0xff;
    TxData[2] = (TargetFrameLengthLines & 0x0000ff00) >> 8;
    TxData[3] = (TargetFrameLengthLines & 0x000000ff);

    B5_OV9750_RegWrite(&B5_OV9750_CurChan, TxData, 4);

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: B5_OV9750_PowerOptimization
 *
 *  @Description:: B5 power optimization
 *
 *  @Input      ::
 *      pB5Chan:    B5 channel config
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static void B5_OV9750_PowerOptimization(AMBA_B5_CHANNEL_s *pB5Chan)
{
    UINT32 DataBuf[1];
    UINT32 Addr;
    AMBA_B5_CHIP_ID_u ChipID;

    ChipID.Data = pB5Chan->SensorID;

    /* B5N */
    DataBuf[0] = 0x00d90f55;
    Addr = AMBA_B5_AHB_BASE_ADDR + 0xf8;
    AmbaB5N_RegWrite(Addr, 0, AMBA_B5_DATA_WIDTH_32BIT, 1, DataBuf);

    DataBuf[0] = 0x80000a21;
    Addr = AMBA_B5_AHB_BASE_ADDR + 0x114;
    AmbaB5N_RegWrite(Addr, 0, AMBA_B5_DATA_WIDTH_32BIT, 1, DataBuf);

    DataBuf[0] = 0x13fd0080;
    Addr = AMBA_B5_AHB_BASE_ADDR + 0x118;
    AmbaB5N_RegWrite(Addr, 0, AMBA_B5_DATA_WIDTH_32BIT, 1, DataBuf);

    //DataBuf[0] = 0x6ea1fd48;
    DataBuf[0] = 0x43e05d5e;
    Addr = AMBA_B5_AHB_BASE_ADDR + 0x134;
    AmbaB5N_RegWrite(Addr, 0, AMBA_B5_DATA_WIDTH_32BIT, 1, DataBuf);

    DataBuf[0] = 0xf;
    Addr = AMBA_B5_AHB_BASE_ADDR + 0x138;
    AmbaB5N_RegWrite(Addr, 0, AMBA_B5_DATA_WIDTH_32BIT, 1, DataBuf);

    /* B5F */
    DataBuf[0] = 0xd908aa; // turn off b5f tx
    Addr = AMBA_B5_AHB_BASE_ADDR + 0xf8;
    AmbaB5F_RegWrite(ChipID, Addr, 0, AMBA_B5_DATA_WIDTH_32BIT, 1, DataBuf);

    DataBuf[0] = 0x80000ae7;
    Addr = AMBA_B5_AHB_BASE_ADDR + 0x114;
    AmbaB5F_RegWrite(ChipID, Addr, 0, AMBA_B5_DATA_WIDTH_32BIT, 1, DataBuf);

    DataBuf[0] = 0x10013480; // turn off b5f 10 lvds tx
    Addr = AMBA_B5_AHB_BASE_ADDR + 0x118;
    AmbaB5F_RegWrite(ChipID, Addr, 0, AMBA_B5_DATA_WIDTH_32BIT, 1, DataBuf);

    //DataBuf[0] = 0x6e14f481;
    DataBuf[0] = 0x6a14b481;
    Addr = AMBA_B5_AHB_BASE_ADDR + 0x134;
    AmbaB5F_RegWrite(ChipID, Addr, 0, AMBA_B5_DATA_WIDTH_32BIT, 1, DataBuf);

    DataBuf[0] = 0x4;
    Addr = AMBA_B5_AHB_BASE_ADDR + 0x138;
    AmbaB5F_RegWrite(ChipID, Addr, 0, AMBA_B5_DATA_WIDTH_32BIT, 1, DataBuf);
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: B5_OV9750_MipiPhyAdjust
 *
 *  @Description:: Adjust MIPI-Phy parameters
 *
 *  @Input      ::
 *      SensorMode:   Sensor Mode ID
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static void B5_OV9750_MipiPhyAdjust(UINT16 SensorMode)
{
    const AMBA_SENSOR_OUTPUT_INFO_s *pOutputInfo = NULL;

    pOutputInfo = &B5_OV9750OutputInfo[B5_OV9750ModeInfoList[SensorMode].OutputMode];

    AmbaB5_VinCalculateDphyConfig(pOutputInfo->DataRate, &B5_OV9750_MipiVinConfig.MipiCtrl);

    if ((SensorMode == AMBA_SENSOR_B5_OV9750_V1_10_1280_960_30P_L2) || (SensorMode == AMBA_SENSOR_B5_OV9750_V1_10_1280_960_25P_L2))
        B5_OV9750_CompressRatio = AMBA_B5_COMPRESS_NONE;

}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: B5_OV9750_Config
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
static int B5_OV9750_Config(AMBA_DSP_CHANNEL_ID_u Chan, AMBA_SENSOR_MODE_ID_u Mode, AMBA_SENSOR_ESHUTTER_TYPE_e ElecShutterMode)
{
    AMBA_SENSOR_MODE_INFO_s *pModeInfo;
    B5_OV9750_MODE_INFO_s *pPrivateModeInfo;
    UINT16 SensorMode = Mode.Bits.Mode;
    static int NotFirstBoot = 0;

    if (SensorMode >= AMBA_SENSOR_B5_OV9750_NUM_MODE)
        return NG;

    AmbaPrint("============  [B5 OV9750]  ============");
    AmbaPrint("============ init mode:%d ============", SensorMode);

    B5_OV9750Ctrl.Status.ElecShutterMode = ElecShutterMode;

    AmbaVIN_Reset((AMBA_VIN_CHANNEL_e)Chan.Bits.VinID, AMBA_VIN_LVDS, NULL);

    AmbaB5_PllSwPllDisable(); // Disable sw pll

    if (NotFirstBoot == 1) {
        B5_OV9750_SetStandbyOn(&B5_OV9750_CurChan);
        AmbaB5_VoutReset();
        AmbaB5_VinReset(&B5_OV9750_CurChan);
    }

    B5_OV9750_CurChanConvert(Chan, &B5_OV9750_CurChan); // Convert CurChan based on SysChan and Chan

    //if (NotFirstBoot == 0) {
    if (1) { // hardware reset B5N/B5F each time
        int InitRval = 0;
        UINT8 BrokenFlag = 0;
        AmbaB5_SetPwmBrokenFlag(0); // Must reset the broken flag to 0
ReInit:
        B5_OV9750_HardwareReset(Chan);
        InitRval = AmbaB5_Init(&B5_OV9750_SysChan, &B5_OV9750_CurChan, &B5_OV9750_PinConfig);

        if (InitRval == NG) {
            BrokenFlag = AmbaB5_GetPwmBrokenFlag();
            //AmbaPrint("BrokenFlag = %d", BrokenFlag);
            Chan.Bits.SensorID = Chan.Bits.SensorID & ~BrokenFlag; // Block broken channels
            B5_OV9750_CurChanConvert(Chan, &B5_OV9750_CurChan); // Re-convert CurChan(passing broken channels)
            goto ReInit;
        }

        AmbaKAL_TaskSleep(10);
        AmbaB5_SetPwmBrokenFlag(BrokenFlag); // Set the actual broken flag(flag before re-init)
        SensorAeSem = AmbaB5_PllGetSwPllSem();
        AmbaB5_PllSwPllShowMsg(0); // Disable sw pll debug message
    }

    pModeInfo = &B5_OV9750Ctrl.Status.ModeInfo;
    pPrivateModeInfo = &B5_OV9750Ctrl.ModeInfo;
    B5_OV9750_PrepareModeInfo(AmbaB5_GetNumActiveChannel(&B5_OV9750_CurChan), SensorMode, pModeInfo, pPrivateModeInfo);

    AmbaB5_PllSetSensorClk(&B5_OV9750_CurChan, B5_OV9750Ctrl.ModeInfo.FrameTiming.InputClk); // Must be set before programing sensor
    AmbaKAL_TaskSleep(3);

    //if (NotFirstBoot == 0) {
    if (1) { // hardware reset sensor each time
        B5_OV9750_SensorHardwareReset();
        NotFirstBoot = 1;
    }

    B5_OV9750_ChangeReadoutMode(&B5_OV9750_CurChan, pPrivateModeInfo->ReadoutMode);
    B5_OV9750_PowerOptimization(&B5_OV9750_CurChan);

    /* Adjust MIPI-Phy parameters */
    B5_OV9750_MipiPhyAdjust(SensorMode);

    AmbaB5_VinPhySetMIPI(&B5_OV9750_CurChan, &B5_OV9750_MipiVinConfig.MipiCtrl);
    AmbaPrint("MIPI interface");

    AmbaB5_PrescalerInit(&B5_OV9750_CurChan, B5_OV9750OutputInfo[B5_OV9750ModeInfoList[SensorMode].OutputMode].RecordingPixels.Width, pPrivateModeInfo->CfaOutputWidth, 0);
    AmbaB5_Enable(&B5_OV9750_CurChan, pPrivateModeInfo->CfaOutputWidth, pModeInfo->OutputInfo.OutputHeight, pModeInfo->OutputInfo.NumDataBits, B5_OV9750_CompressRatio);
    AmbaB5_PllSwPllConfig(pPrivateModeInfo->FrameTiming.FrameRate.TimeScale /
                          pPrivateModeInfo->FrameTiming.FrameRate.NumUnitsInTick);

    B5_OV9750_SetStandbyOff(&B5_OV9750_CurChan);
    B5_OV9750_ConfigVin(Chan, pModeInfo, pPrivateModeInfo);

    AmbaB5_PllSwPllEnable(); // Enable sw pll

    B5_OV9750Ctrl.Status.DevStat.Bits.Sensor0Standby = 0;

    return OK;
}

/*-----------------------------------------------------------------------------------------------*/
AMBA_SENSOR_OBJ_s AmbaSensor_B5_OV9750Obj = {
    .Init                   = B5_OV9750_Init,
    .Enable                 = B5_OV9750_Enable,
    .Disable                = B5_OV9750_Disable,
    .Config                 = B5_OV9750_Config,
    .GetStatus              = B5_OV9750_GetStatus,
    .GetModeInfo            = B5_OV9750_GetModeInfo,
    .GetDeviceInfo          = B5_OV9750_GetDeviceInfo,
    .GetCurrentGainFactor   = B5_OV9750_GetCurrentGainFactor,
    .GetCurrentShutterSpeed = B5_OV9750_GetCurrentShutterSpeed,
    .ConvertGainFactor      = B5_OV9750_ConvertGainFactor,
    .ConvertShutterSpeed    = B5_OV9750_ConvertShutterSpeed,
    .SetAnalogGainCtrl      = B5_OV9750_SetAnalogGainCtrl,
    .SetDigitalGainCtrl     = NULL,
    .SetShutterCtrl         = B5_OV9750_SetShutterCtrl,
    .SetSlowShutterCtrl     = B5_OV9750_SetSlowShutterCtrl,

    .RegisterRead           = B5_OV9750_RegisterRead,
    .RegisterWrite          = B5_OV9750_RegisterWrite,
};
