/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaSensor_B5_OV4689.c
 *
 *  @Copyright      :: Copyright (C) 2013 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Control APIs of OmniVision B5_OV4689 CMOS sensor with MIPI interface
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
#include "AmbaSensor_B5_OV4689.h"
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
 * B5_OV4689 sensor runtime info
\*-----------------------------------------------------------------------------------------------*/
B5_OV4689_CTRL_s B5_OV4689Ctrl = {0};

static AMBA_B5_CHANNEL_s B5_OV4689_SysChan;
static AMBA_B5_CHANNEL_s B5_OV4689_CurChan;
static AMBA_B5_PIN_CONFIG_s B5_OV4689_PinConfig = {
    .B5nPinMux = AMBA_B5_PIN_B5N_CTRL_SPI,
    .B5fPinMux = AMBA_B5_PIN_B5F_CTRL_PWM_SINGLE_ENDED,
    .SensorPinMux = AMBA_B5_PIN_SENSOR_CTRL_I2C1,
    .VideoSyncPinMux = AMBA_B5_PIN_VIDEO_SYNC_NONE,
    .SlvsEcType = AMBA_B5_SLVS_EC_3LANE,
};
static AMBA_B5_COMPRESS_RATIO_e B5_OV4689_CompressRatio = AMBA_B5_COMPRESS_NONE;
extern AMBA_KAL_SEM_t AmbaB5_PllGetSwPllSem(void);
static AMBA_KAL_SEM_t SensorAeSem;

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
 * Configurations for B5 side VIN (receiving video from OmniVision OV4689)
\*-----------------------------------------------------------------------------------------------*/
static AMBA_B5_VIN_MIPI_CONFIG_s B5_OV4689_MipiVinConfig = {
    .NumDataBits            = 10,
    .DataTypeMask           = 0x00,
    .DataType               = AMBA_B5_VIN_MIPI_RAW10,
    .NumActiveLanes         = 4,
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
    .HsSettleTime   = 32,
    .HsTermTime     = 16,
    .ClkSettleTime  = 32,
    .ClkTermTime    = 8,
    .ClkMissTime    = 8,
    .RxInitTime     = 64,
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
    .MinVBlank = 10,    /* larger value will cause voutf fifo overflow (resulted in top visual errors) */
};

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: B5_OV4689_B5N_GPIO_Set
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
void B5_OV4689_B5N_GPIO_Set(UINT32 PinNum)
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
 *  @RoutineName:: B5_OV4689_B5N_GPIO_Clr
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
void B5_OV4689_B5N_GPIO_Clr(UINT32 PinNum)
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
 *  @RoutineName:: B5_OV4689_B5F_GPIO_Set
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
void B5_OV4689_B5F_GPIO_Set(AMBA_B5_CHIP_ID_u ChipID, UINT32 PinNum)
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
 *  @RoutineName:: B5_OV4689_B5F_GPIO_Clr
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
void B5_OV4689_B5F_GPIO_Clr(AMBA_B5_CHIP_ID_u ChipID, UINT32 PinNum)
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
 *  @RoutineName:: B5_OV4689_PrepareModeInfo
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
 *      pSensorHdrInfo: Sensor HDR information
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static void B5_OV4689_PrepareModeInfo(UINT8 SensorCount, UINT16 Mode, AMBA_SENSOR_MODE_INFO_s *pModeInfo, B5_OV4689_MODE_INFO_s *pPrivateModeInfo,
                                      B5_OV4689_HDR_INFO_s *pSensorHdrInfo)
{
    const AMBA_SENSOR_INPUT_INFO_s  *pInputInfo= &B5_OV4689InputInfo[B5_OV4689ModeInfoList[Mode].InputMode];
    const AMBA_SENSOR_OUTPUT_INFO_s *pOutputInfo= &B5_OV4689OutputInfo[B5_OV4689ModeInfoList[Mode].OutputMode];

    if (pPrivateModeInfo == NULL)
        pPrivateModeInfo = (B5_OV4689_MODE_INFO_s *) &B5_OV4689ModeInfoList[Mode];
    else
        memcpy(pPrivateModeInfo, &B5_OV4689ModeInfoList[Mode], sizeof(B5_OV4689_MODE_INFO_s));

    if (pSensorHdrInfo == NULL)
        pSensorHdrInfo = (B5_OV4689_HDR_INFO_s *) &B5_OV4689ModeInfoList[Mode].HdrInfo;
    else
        memcpy(pSensorHdrInfo, &B5_OV4689ModeInfoList[Mode].HdrInfo, sizeof(B5_OV4689_HDR_INFO_s));

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

    pModeInfo->OutputInfo.DataRate = B5_OV4689_B5nOutputInfo[0].DataRate;
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
 *  @RoutineName:: B5_OV4689_ConfigVin
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
static void B5_OV4689_ConfigVin(AMBA_DSP_CHANNEL_ID_u Chan, AMBA_SENSOR_MODE_INFO_s *pModeInfo, B5_OV4689_MODE_INFO_s *pPrivateModeInfo)
{
    AMBA_B5_VIN_WINDOW_s B5CropWindow;
    AMBA_SENSOR_OUTPUT_INFO_s *pImgInfo;
    AMBA_B5_VIN_MIPI_CONFIG_s *pB5MipiVinCfg = &B5_OV4689_MipiVinConfig;
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
    B5_VoutConfig.NumDataLane = AmbaB5_GetNumActiveChannel(&B5_OV4689_CurChan) * 2;  /* TODO */
    B5_VoutConfig.Channel[0] = (B5_OV4689_CurChan.SensorID & 1) ? B5_OV4689_CurChan.Active[0] : B5_OV4689_CurChan.Inactive[0];
    B5_VoutConfig.Channel[1] = (B5_OV4689_CurChan.SensorID & 2) ? B5_OV4689_CurChan.Active[1] : B5_OV4689_CurChan.Inactive[1];
    B5_VoutConfig.Channel[2] = (B5_OV4689_CurChan.SensorID & 4) ? B5_OV4689_CurChan.Active[2] : B5_OV4689_CurChan.Inactive[2];
    B5_VoutConfig.Channel[3] = (B5_OV4689_CurChan.SensorID & 8) ? B5_OV4689_CurChan.Active[3] : B5_OV4689_CurChan.Inactive[3];
    B5_VoutConfig.MinVBlank = pPrivateModeInfo->FrameTiming.FrameLengthLines - pImgInfo->OutputHeight;
    B5_VoutConfig.MinHBlank = 1 + (UINT32)(VoutHBlank / 2.0 - 1);
    B5_VoutConfig.MaxHBlank = (UINT32)((VoutHBlank + LvdsPixelNum) + 1);
    B5_VoutConfig.CompressRatio = B5_OV4689_CompressRatio;

    AmbaB5_VoutConfig(&B5_VoutConfig);
    AmbaB5_VoutEnable();

    /* Config B5 side VIN */
    pImgInfo = (AMBA_SENSOR_OUTPUT_INFO_s *)&B5_OV4689OutputInfo[pPrivateModeInfo->OutputMode];

    pB5MipiVinCfg->RxHvSyncCtrl.NumActivePixels = pImgInfo->OutputWidth;
    pB5MipiVinCfg->RxHvSyncCtrl.NumActiveLines= pImgInfo->OutputHeight;

    pB5MipiVinCfg->NumDataBits = pImgInfo->NumDataBits;
    pB5MipiVinCfg->NumActiveLanes = pImgInfo->NumDataLanes;

    B5CropWindow.StartX = pImgInfo->RecordingPixels.StartX;
    B5CropWindow.StartY = pImgInfo->RecordingPixels.StartY;
    B5CropWindow.EndX   = pImgInfo->RecordingPixels.Width + pImgInfo->RecordingPixels.StartX - 1;
    B5CropWindow.EndY   = pImgInfo->RecordingPixels.Height + pImgInfo->RecordingPixels.StartY - 1;

    AmbaB5_VinConfigMIPI(&B5_OV4689_CurChan, pB5MipiVinCfg);

    AmbaB5_VinCaptureConfig(&B5_OV4689_CurChan, &B5CropWindow);

    /* Config SoC side VIN */

    /* For Second VIN */
    if (Chan.Bits.VinID == 1)
        AmbaDSP_VinPhySetSLVS(AMBA_VIN_CHANNEL1);
    else
    /* For MAIN VIN */
        AmbaDSP_VinPhySetSLVS(AMBA_VIN_CHANNEL0);

    pImgInfo = &pModeInfo->OutputInfo;
    pVinCfg->Info.NumDataBits = pImgInfo->NumDataBits;
    pVinCfg->NumActiveLanes = pImgInfo->NumDataLanes;
    pVinCfg->pLaneMapping = B5_LvdsLaneMux;

    pVinCfg->RxHvSyncCtrl.NumActivePixels = pImgInfo->OutputWidth;
    pVinCfg->RxHvSyncCtrl.NumActiveLines = pImgInfo->OutputHeight;
    pVinCfg->RxHvSyncCtrl.NumTotalPixels = pImgInfo->OutputWidth + B5_VoutConfig.MinHBlank - 1;
    pVinCfg->RxHvSyncCtrl.NumTotalLines = pModeInfo->FrameLengthLines;

    AmbaPrint("pImgInfo->OutputWidth       = %d ", pImgInfo->OutputWidth);
    AmbaPrint("pImgInfo->OutputHeight      = %d ", pImgInfo->OutputHeight);
    AmbaPrint("pModeInfo->LineLengthPck    = %d ", pModeInfo->LineLengthPck);
    AmbaPrint("pModeInfo->FrameLengthLines = %d ", pModeInfo->FrameLengthLines);

    pVinCfg->Info.BayerPattern = (AMBA_DSP_SENSOR_PATTERN_e) pImgInfo->CfaPattern;

    memcpy(&pVinCfg->Info.FrameRate, &pPrivateModeInfo->FrameTiming.FrameRate, sizeof(AMBA_DSP_FRAME_RATE_s));
    memcpy(&pVinCfg->Info.DspPhaseShift, &pImgInfo->DspPhaseShift, sizeof(AMBA_DSP_PHASE_SHIFT_CTRL_s));

    if (Chan.Bits.VinID == 1)
    /* For Second VIN */
        AmbaDSP_VinConfigSLVS(AMBA_VIN_CHANNEL1, pVinCfg);
    else
    /* For MAIN VIN */
        AmbaDSP_VinConfigSLVS(AMBA_VIN_CHANNEL0, pVinCfg);
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: B5_OV4689_HardwareReset
 *
 *  @Description:: Reset B5_OV4689
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static inline void B5_OV4689_HardwareReset(AMBA_DSP_CHANNEL_ID_u Chan)
{
    /* For Second VIN */
    if (Chan.Bits.VinID == 1) {
        /* B5N GPIO 8 is for B5F power supply */
        /* GPIO_PIN_19 is fot B5N reset */
        AmbaGPIO_ConfigOutput(GPIO_PIN_19, 0);
        AmbaKAL_TaskSleep(5);
        B5_OV4689_B5N_GPIO_Clr(30);
        AmbaKAL_TaskSleep(30);
        AmbaGPIO_ConfigOutput(GPIO_PIN_19, 1);
        AmbaKAL_TaskSleep(300);
        B5_OV4689_B5N_GPIO_Set(30);

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
 *  @RoutineName:: B5_OV4689_SensorHardwareReset
 *
 *  @Description:: Reset Sensors
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static void B5_OV4689_SensorHardwareReset(void)
{
    AMBA_B5_CHIP_ID_u ChipID;
    ChipID.Data = B5_OV4689_CurChan.SensorID;

    /* B5F GPIO 9 is for sensor PWDN pin */
    /* B5F GPIO 8 is for sensor reset pin */
    B5_OV4689_B5F_GPIO_Clr(ChipID, 31);
    AmbaKAL_TaskSleep(5);
    B5_OV4689_B5F_GPIO_Clr(ChipID, 30);

    AmbaKAL_TaskSleep(20);

    B5_OV4689_B5F_GPIO_Set(ChipID, 31);
    AmbaKAL_TaskSleep(5);
    B5_OV4689_B5F_GPIO_Set(ChipID, 30);

    AmbaKAL_TaskSleep(5);
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: B5_OV4689_RegWrite
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
static int B5_OV4689_RegWrite(AMBA_B5_CHANNEL_s *pB5Chan, UINT8 *pTxDataBuf, UINT8 DataSize)
{
    AMBA_B5_I2C_CHANNEL_e I2cChanNo = AMBA_B5_I2C_CHANNEL1;
    UINT32 SlaveAddr = 0x6C; // 0x6C is for OV4689
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
 *  @RoutineName:: B5_OV4689_RegRead
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
int B5_OV4689_RegRead(AMBA_B5_CHANNEL_s *pB5Chan, UINT16 RegAddr, UINT8 *pRxDataBuf)
{
    AMBA_B5_I2C_TRANSACTION_s I2cTransaction[2];
    AMBA_B5_I2C_CHANNEL_e I2cChanNo = AMBA_B5_I2C_CHANNEL1;
    AMBA_B5_CHANNEL_e RxChan = AMBA_B5_CHANNEL0;

    UINT32 SlaveAddr = 0x6C; // 0x6C is for OV4689
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
 *  @RoutineName:: B5_OV4689_Write
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
static int B5_OV4689_Write(AMBA_B5_CHANNEL_s *pB5Chan, UINT16 Addr, UINT8 Data)
{
    UINT8 TxData[3];

    TxData[0] = Addr >> 8;
    TxData[1] = Addr & 0xff;
    TxData[2] = Data;

    B5_OV4689_RegWrite(pB5Chan, TxData, 3);

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: B5_OV4689_RegisterWrite
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
static int B5_OV4689_RegisterWrite(AMBA_DSP_CHANNEL_ID_u Chan, UINT16 Addr, UINT16 Data)
{
    UINT8 TxData[3];

    TxData[0] = Addr >> 8;
    TxData[1] = Addr & 0xff;
    TxData[2] = Data;

    B5_OV4689_CurChan.SensorID = Chan.Bits.SensorID;
    B5_OV4689_RegWrite(&B5_OV4689_CurChan, TxData, 3);

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: B5_OV4689_RegisterRead
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
static int B5_OV4689_RegisterRead(AMBA_DSP_CHANNEL_ID_u Chan, UINT16 Addr, UINT16 *pData)
{
    UINT8 Data;

    B5_OV4689_CurChan.SensorID = Chan.Bits.SensorID;
    B5_OV4689_RegRead(&B5_OV4689_CurChan, Addr, &Data);

    *pData = Data;

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: B5_OV4689_SetStandbyOn
 *
 *  @Description:: Configure sensor into standby mode
 *
 *  @Input      :: none
 *      pB5Chan:   Vin ID and sensor ID
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static void B5_OV4689_SetStandbyOn(AMBA_B5_CHANNEL_s *pB5Chan)
{
    UINT32 Delay;

    Delay = B5_OV4689Ctrl.ModeInfo.FrameTiming.FrameRate.TimeScale /
            B5_OV4689Ctrl.ModeInfo.FrameTiming.FrameRate.NumUnitsInTick;

    B5_OV4689_Write(pB5Chan, 0x0100, 0x00);

    AmbaKAL_TaskSleep(Delay);
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: B5_OV4689_SetStandbyOff
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
static void B5_OV4689_SetStandbyOff(AMBA_B5_CHANNEL_s *pB5Chan)
{
    B5_OV4689_Write(pB5Chan, 0x0100, 0x01);
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: B5_OV4689_ChangeReadoutMode
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
static int B5_OV4689_ChangeReadoutMode(AMBA_B5_CHANNEL_s *pB5Chan, UINT16 Mode)
{
    int i;

    for (i = 0; i < B5_OV4689_NUM_READOUT_MODE_REG; i ++)
        B5_OV4689_Write(pB5Chan, B5_OV4689RegTable[i].Addr, B5_OV4689RegTable[i].Data[B5_OV4689ModeInfoList[Mode].ReadoutMode]);

    /* Reset current AE information */
    memset(&B5_OV4689Ctrl.CurrentAEInfo, 0, sizeof(B5_OV4689_CURRENT_AE_INFO_s));

    return OK;
}

#if 0
/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: B5_OV4689_VinHookFunc
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
static void B5_OV4689_VinHookFunc(int Entry) {
    AmbaB5_PllSwPllVinHookFunc((UINT32)Entry);
}
#endif

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: B5_OV4689_SysChanConvert
 *
 *  @Description:: Convert B5_OV4689_SysChan to mw configuration
 *
 *  @Input      ::
 *      Chan: Vin ID and sensor ID
 *
 *  @Output     :: none
 *      pB5Chan: Vin ID and sensor ID
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static void B5_OV4689_SysChanConvert(AMBA_DSP_CHANNEL_ID_u Chan, AMBA_B5_CHANNEL_s *pB5Chan)
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
 *  @RoutineName:: B5_OV4689_CurChanConvert
 *
 *  @Description:: Convert B5_OV4689_CurChan to current configuration
 *
 *  @Input      ::
 *      Chan: Vin ID and sensor ID
 *
 *  @Output     :: none
 *      pB5Chan: Vin ID and sensor ID
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static void B5_OV4689_CurChanConvert(AMBA_DSP_CHANNEL_ID_u Chan, AMBA_B5_CHANNEL_s *pB5Chan)
{
    int i;

    pB5Chan->SensorID = 0;

    for (i = 0; i< AMBA_NUM_B5_CHANNEL; i++) {
        if (Chan.Bits.SensorID & (1 << i) & B5_OV4689_SysChan.SensorID) {
            pB5Chan->Active[i] = B5_OV4689_SysChan.Active[i];
            pB5Chan->SensorID |= (1 << i);
        } else
            pB5Chan->Inactive[i] = B5_OV4689_SysChan.Inactive[i];
    }
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: B5_OV4689_Init
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
static int B5_OV4689_Init(AMBA_DSP_CHANNEL_ID_u Chan)
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

    B5_OV4689_SysChanConvert(Chan, &B5_OV4689_SysChan); // Convert SysChan

#if 0 /* TBD */
    if (B5_OV4689_PinConfig.B5fPinMux == AMBA_B5_PIN_B5F_CTRL_PWM_SINGLE_ENDED) {
       AmbaINT_IsrHook(AMBA_VIC_INT_ID53_IDSP_PIP_VSYNC_SLAVE, AMBA_VIC_INT_ID53_IDSP_PIP_VSYNC_SLAVE, AMBA_VIC_INT_RISING_EDGE_TRIGGER, B5_OV4689_VinHookFunc);
       AmbaINT_IntEnable(AMBA_VIC_INT_ID53_IDSP_PIP_VSYNC_SLAVE);
    }
#endif

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: B5_OV4689_Enable
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
static int B5_OV4689_Enable(AMBA_DSP_CHANNEL_ID_u Chan)
{
    if (B5_OV4689Ctrl.Status.DevStat.Bits.Sensor0Power == 1)
        return OK;

    B5_OV4689Ctrl.Status.DevStat.Bits.Sensor0Power   = 1;
    B5_OV4689Ctrl.Status.DevStat.Bits.Sensor0Standby = 1;

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: B5_OV4689_Disable
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
static int B5_OV4689_Disable(AMBA_DSP_CHANNEL_ID_u Chan)
{
    if (B5_OV4689Ctrl.Status.DevStat.Bits.Sensor0Power == 0)
        return OK;

    B5_OV4689_CurChan.SensorID = Chan.Bits.SensorID;

    B5_OV4689_SetStandbyOn(&B5_OV4689_CurChan);

    B5_OV4689Ctrl.Status.DevStat.Bits.Sensor0Power = 0;

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: B5_OV4689_InitSeq
 *
 *  @Description:: Initial sequence for OV4689.
 *
 *
 *  @Input      ::
 *      pB5Chan:   Vin ID and sensor ID
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static int B5_OV4689_InitSeq(AMBA_B5_CHANNEL_s *pB5Chan)
{
    B5_OV4689_Write(pB5Chan, 0x0103, 0x01);
    B5_OV4689_Write(pB5Chan, 0x3638, 0x00);
    B5_OV4689_Write(pB5Chan, 0x0304, 0x03);
    B5_OV4689_Write(pB5Chan, 0x030b, 0x00);
    B5_OV4689_Write(pB5Chan, 0x030d, 0x1e);
    B5_OV4689_Write(pB5Chan, 0x030e, 0x04);
    B5_OV4689_Write(pB5Chan, 0x030f, 0x01);
    B5_OV4689_Write(pB5Chan, 0x0312, 0x01);
    B5_OV4689_Write(pB5Chan, 0x031e, 0x00);
    B5_OV4689_Write(pB5Chan, 0x3000, 0x20);
    B5_OV4689_Write(pB5Chan, 0x3002, 0x00);
    B5_OV4689_Write(pB5Chan, 0x3020, 0x93);
    B5_OV4689_Write(pB5Chan, 0x3021, 0x03);
    B5_OV4689_Write(pB5Chan, 0x3022, 0x01);
    B5_OV4689_Write(pB5Chan, 0x3031, 0x0a);
    B5_OV4689_Write(pB5Chan, 0x303f, 0x0c);
    B5_OV4689_Write(pB5Chan, 0x3305, 0xf1);
    B5_OV4689_Write(pB5Chan, 0x3307, 0x04);
    B5_OV4689_Write(pB5Chan, 0x3309, 0x29);
    B5_OV4689_Write(pB5Chan, 0x3500, 0x00);
    B5_OV4689_Write(pB5Chan, 0x3502, 0x00);
    B5_OV4689_Write(pB5Chan, 0x3503, 0x74); // Shutter reflection_timing = 2 & gain reflection_timing = 1
    B5_OV4689_Write(pB5Chan, 0x3504, 0x00);
    B5_OV4689_Write(pB5Chan, 0x3505, 0x00);
    B5_OV4689_Write(pB5Chan, 0x3506, 0x00);
    B5_OV4689_Write(pB5Chan, 0x3507, 0x00);
    B5_OV4689_Write(pB5Chan, 0x3508, 0x00);
    B5_OV4689_Write(pB5Chan, 0x3509, 0x80);
    B5_OV4689_Write(pB5Chan, 0x350a, 0x00);
    B5_OV4689_Write(pB5Chan, 0x350b, 0x00);
    B5_OV4689_Write(pB5Chan, 0x350c, 0x00);
    B5_OV4689_Write(pB5Chan, 0x350d, 0x00);
    B5_OV4689_Write(pB5Chan, 0x350e, 0x00);
    B5_OV4689_Write(pB5Chan, 0x350f, 0x80);
    B5_OV4689_Write(pB5Chan, 0x3510, 0x00);
    B5_OV4689_Write(pB5Chan, 0x3511, 0x00);
    B5_OV4689_Write(pB5Chan, 0x3512, 0x00);
    B5_OV4689_Write(pB5Chan, 0x3513, 0x00);
    B5_OV4689_Write(pB5Chan, 0x3514, 0x00);
    B5_OV4689_Write(pB5Chan, 0x3515, 0x80);
    B5_OV4689_Write(pB5Chan, 0x3516, 0x00);
    B5_OV4689_Write(pB5Chan, 0x3517, 0x00);
    B5_OV4689_Write(pB5Chan, 0x3518, 0x00);
    B5_OV4689_Write(pB5Chan, 0x3519, 0x00);
    B5_OV4689_Write(pB5Chan, 0x351a, 0x00);
    B5_OV4689_Write(pB5Chan, 0x351b, 0x80);
    B5_OV4689_Write(pB5Chan, 0x351c, 0x00);
    B5_OV4689_Write(pB5Chan, 0x351d, 0x00);
    B5_OV4689_Write(pB5Chan, 0x351e, 0x00);
    B5_OV4689_Write(pB5Chan, 0x351f, 0x00);
    B5_OV4689_Write(pB5Chan, 0x3520, 0x00);
    B5_OV4689_Write(pB5Chan, 0x3521, 0x80);
    B5_OV4689_Write(pB5Chan, 0x3522, 0x08);
    B5_OV4689_Write(pB5Chan, 0x3524, 0x08);
    B5_OV4689_Write(pB5Chan, 0x3526, 0x08);
    B5_OV4689_Write(pB5Chan, 0x3528, 0x08);
    B5_OV4689_Write(pB5Chan, 0x352a, 0x08);
    B5_OV4689_Write(pB5Chan, 0x3602, 0x00);
    B5_OV4689_Write(pB5Chan, 0x3603, 0x40);
    B5_OV4689_Write(pB5Chan, 0x3604, 0x02);
    B5_OV4689_Write(pB5Chan, 0x3605, 0x00);
    B5_OV4689_Write(pB5Chan, 0x3606, 0x00);
    B5_OV4689_Write(pB5Chan, 0x3607, 0x00);
    B5_OV4689_Write(pB5Chan, 0x3609, 0x12);
    B5_OV4689_Write(pB5Chan, 0x360a, 0x40);
    B5_OV4689_Write(pB5Chan, 0x360c, 0x08);
    B5_OV4689_Write(pB5Chan, 0x360f, 0xe5);
    B5_OV4689_Write(pB5Chan, 0x3608, 0x8f);
    B5_OV4689_Write(pB5Chan, 0x3611, 0x00);
    B5_OV4689_Write(pB5Chan, 0x3613, 0xf7);
    B5_OV4689_Write(pB5Chan, 0x3616, 0x58);
    B5_OV4689_Write(pB5Chan, 0x3619, 0x99);
    B5_OV4689_Write(pB5Chan, 0x361b, 0x60);
    B5_OV4689_Write(pB5Chan, 0x361c, 0x7a);
    B5_OV4689_Write(pB5Chan, 0x361e, 0x79);
    B5_OV4689_Write(pB5Chan, 0x361f, 0x02);
    B5_OV4689_Write(pB5Chan, 0x3633, 0x10);
    B5_OV4689_Write(pB5Chan, 0x3634, 0x10);
    B5_OV4689_Write(pB5Chan, 0x3635, 0x10);
    B5_OV4689_Write(pB5Chan, 0x3636, 0x15);
    B5_OV4689_Write(pB5Chan, 0x3646, 0x86);
    B5_OV4689_Write(pB5Chan, 0x364a, 0x0b);
    B5_OV4689_Write(pB5Chan, 0x3700, 0x17);
    B5_OV4689_Write(pB5Chan, 0x3701, 0x22);
    B5_OV4689_Write(pB5Chan, 0x3703, 0x10);
    B5_OV4689_Write(pB5Chan, 0x370a, 0x37);
    B5_OV4689_Write(pB5Chan, 0x3705, 0x00);
    B5_OV4689_Write(pB5Chan, 0x3706, 0x63);
    B5_OV4689_Write(pB5Chan, 0x3709, 0x3c);
    B5_OV4689_Write(pB5Chan, 0x370b, 0x01);
    B5_OV4689_Write(pB5Chan, 0x370c, 0x30);
    B5_OV4689_Write(pB5Chan, 0x3710, 0x24);
    B5_OV4689_Write(pB5Chan, 0x3711, 0x0c);
    B5_OV4689_Write(pB5Chan, 0x3716, 0x00);
    B5_OV4689_Write(pB5Chan, 0x3720, 0x28);
    B5_OV4689_Write(pB5Chan, 0x3729, 0x7b);
    B5_OV4689_Write(pB5Chan, 0x372a, 0x84);
    B5_OV4689_Write(pB5Chan, 0x372b, 0xbd);
    B5_OV4689_Write(pB5Chan, 0x372c, 0xbc);
    B5_OV4689_Write(pB5Chan, 0x372e, 0x52);
    B5_OV4689_Write(pB5Chan, 0x373c, 0x0e);
    B5_OV4689_Write(pB5Chan, 0x373e, 0x33);
    B5_OV4689_Write(pB5Chan, 0x3743, 0x10);
    B5_OV4689_Write(pB5Chan, 0x3744, 0x88);
    B5_OV4689_Write(pB5Chan, 0x3745, 0xc0);
    B5_OV4689_Write(pB5Chan, 0x374c, 0x00);
    B5_OV4689_Write(pB5Chan, 0x374e, 0x23);
    B5_OV4689_Write(pB5Chan, 0x3751, 0x7b);
    B5_OV4689_Write(pB5Chan, 0x3752, 0x84);
    B5_OV4689_Write(pB5Chan, 0x3753, 0xbd);
    B5_OV4689_Write(pB5Chan, 0x3754, 0xbc);
    B5_OV4689_Write(pB5Chan, 0x3756, 0x52);
    B5_OV4689_Write(pB5Chan, 0x375c, 0x00);
    B5_OV4689_Write(pB5Chan, 0x3760, 0x00);
    B5_OV4689_Write(pB5Chan, 0x3761, 0x00);
    B5_OV4689_Write(pB5Chan, 0x3762, 0x00);
    B5_OV4689_Write(pB5Chan, 0x3763, 0x00);
    B5_OV4689_Write(pB5Chan, 0x3764, 0x00);
    B5_OV4689_Write(pB5Chan, 0x3767, 0x04);
    B5_OV4689_Write(pB5Chan, 0x3768, 0x04);
    B5_OV4689_Write(pB5Chan, 0x3769, 0x08);
    B5_OV4689_Write(pB5Chan, 0x376a, 0x08);
    B5_OV4689_Write(pB5Chan, 0x376c, 0x00);
    B5_OV4689_Write(pB5Chan, 0x376d, 0x00);
    B5_OV4689_Write(pB5Chan, 0x376e, 0x00);
    B5_OV4689_Write(pB5Chan, 0x3773, 0x00);
    B5_OV4689_Write(pB5Chan, 0x3774, 0x51);
    B5_OV4689_Write(pB5Chan, 0x3776, 0xbd);
    B5_OV4689_Write(pB5Chan, 0x3777, 0xbd);
    B5_OV4689_Write(pB5Chan, 0x3781, 0x18);
    B5_OV4689_Write(pB5Chan, 0x3783, 0x25);
    B5_OV4689_Write(pB5Chan, 0x3798, 0x1b);
    B5_OV4689_Write(pB5Chan, 0x382b, 0x01);
    B5_OV4689_Write(pB5Chan, 0x382d, 0x7f);
    B5_OV4689_Write(pB5Chan, 0x3837, 0x00);
    B5_OV4689_Write(pB5Chan, 0x3841, 0x02);
    B5_OV4689_Write(pB5Chan, 0x3846, 0x08);
    B5_OV4689_Write(pB5Chan, 0x3847, 0x07);
    B5_OV4689_Write(pB5Chan, 0x3d85, 0x36);
    B5_OV4689_Write(pB5Chan, 0x3d8c, 0x71);
    B5_OV4689_Write(pB5Chan, 0x3d8d, 0xcb);
    B5_OV4689_Write(pB5Chan, 0x3f0a, 0x00);
    B5_OV4689_Write(pB5Chan, 0x4000, 0x71); // BLC control
    B5_OV4689_Write(pB5Chan, 0x4002, 0x04);
    B5_OV4689_Write(pB5Chan, 0x400e, 0x00);
    B5_OV4689_Write(pB5Chan, 0x4011, 0x00);
    B5_OV4689_Write(pB5Chan, 0x401a, 0x00);
    B5_OV4689_Write(pB5Chan, 0x401b, 0x00);
    B5_OV4689_Write(pB5Chan, 0x401c, 0x00);
    B5_OV4689_Write(pB5Chan, 0x401d, 0x00);
    B5_OV4689_Write(pB5Chan, 0x401f, 0x00);
    B5_OV4689_Write(pB5Chan, 0x4020, 0x00);
    B5_OV4689_Write(pB5Chan, 0x4021, 0x10);
    B5_OV4689_Write(pB5Chan, 0x4028, 0x00);
    B5_OV4689_Write(pB5Chan, 0x4029, 0x02);
    B5_OV4689_Write(pB5Chan, 0x402c, 0x02);
    B5_OV4689_Write(pB5Chan, 0x402d, 0x02);
    B5_OV4689_Write(pB5Chan, 0x4302, 0xff);
    B5_OV4689_Write(pB5Chan, 0x4303, 0xff);
    B5_OV4689_Write(pB5Chan, 0x4304, 0x00);
    B5_OV4689_Write(pB5Chan, 0x4305, 0x00);
    B5_OV4689_Write(pB5Chan, 0x4306, 0x00);
    B5_OV4689_Write(pB5Chan, 0x4308, 0x02);
    B5_OV4689_Write(pB5Chan, 0x4500, 0x6c);
    B5_OV4689_Write(pB5Chan, 0x4501, 0xc4);
    B5_OV4689_Write(pB5Chan, 0x4503, 0x01);
    B5_OV4689_Write(pB5Chan, 0x4800, 0x04);
    B5_OV4689_Write(pB5Chan, 0x4813, 0x08);
    B5_OV4689_Write(pB5Chan, 0x481f, 0x40);
    B5_OV4689_Write(pB5Chan, 0x4829, 0x78);
    B5_OV4689_Write(pB5Chan, 0x4b00, 0x2a);
    B5_OV4689_Write(pB5Chan, 0x4b0d, 0x00);
    B5_OV4689_Write(pB5Chan, 0x4d00, 0x04);
    B5_OV4689_Write(pB5Chan, 0x4d01, 0x42);
    B5_OV4689_Write(pB5Chan, 0x4d02, 0xd1);
    B5_OV4689_Write(pB5Chan, 0x4d03, 0x93);
    B5_OV4689_Write(pB5Chan, 0x4d04, 0xf5);
    B5_OV4689_Write(pB5Chan, 0x4d05, 0xc1);
    B5_OV4689_Write(pB5Chan, 0x5000, 0xe3); // Disable WB gain and enable digital gain
    B5_OV4689_Write(pB5Chan, 0x5001, 0x11);
    B5_OV4689_Write(pB5Chan, 0x5004, 0x00);
    B5_OV4689_Write(pB5Chan, 0x500a, 0x00);
    B5_OV4689_Write(pB5Chan, 0x500b, 0x00);
    B5_OV4689_Write(pB5Chan, 0x5032, 0x00);
    B5_OV4689_Write(pB5Chan, 0x5040, 0x00);
    B5_OV4689_Write(pB5Chan, 0x5500, 0x00);
    B5_OV4689_Write(pB5Chan, 0x5501, 0x10);
    B5_OV4689_Write(pB5Chan, 0x5502, 0x01);
    B5_OV4689_Write(pB5Chan, 0x5503, 0x0f);
    B5_OV4689_Write(pB5Chan, 0x8000, 0x00);
    B5_OV4689_Write(pB5Chan, 0x8001, 0x00);
    B5_OV4689_Write(pB5Chan, 0x8002, 0x00);
    B5_OV4689_Write(pB5Chan, 0x8003, 0x00);
    B5_OV4689_Write(pB5Chan, 0x8004, 0x00);
    B5_OV4689_Write(pB5Chan, 0x8005, 0x00);
    B5_OV4689_Write(pB5Chan, 0x8006, 0x00);
    B5_OV4689_Write(pB5Chan, 0x8007, 0x00);
    B5_OV4689_Write(pB5Chan, 0x8008, 0x00);
    B5_OV4689_Write(pB5Chan, 0x3638, 0x00);

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: B5_OV4689_HdrSeq
 *
 *  @Description:: HDR setting for OV4689.
 *
 *  @Input      ::
 *      pB5Chan:   Vin ID and sensor ID
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static int B5_OV4689_HdrSeq(AMBA_B5_CHANNEL_s *pB5Chan, UINT32 SensorMode)
{
    if (B5_OV4689ModeInfoList[SensorMode].HdrInfo.HdrIsSupport == 1) {
         //NEW_STG_EN
         B5_OV4689_Write(pB5Chan, 0x5001, 0xd1);
         B5_OV4689_Write(pB5Chan, 0x3837, 0x80);
         B5_OV4689_Write(pB5Chan, 0x3806, 0x05);
         B5_OV4689_Write(pB5Chan, 0x3807, 0xfb);
         B5_OV4689_Write(pB5Chan, 0x3764, 0x01);
         B5_OV4689_Write(pB5Chan, 0x3760, 0x01);
         B5_OV4689_Write(pB5Chan, 0x3761, 0x2C);
         B5_OV4689_Write(pB5Chan, 0x3762, 0x00);
         B5_OV4689_Write(pB5Chan, 0x3763, 0x00);
         B5_OV4689_Write(pB5Chan, 0x3500, 0x00);
         B5_OV4689_Write(pB5Chan, 0x3501, 0x30);
         B5_OV4689_Write(pB5Chan, 0x3502, 0x00);
         B5_OV4689_Write(pB5Chan, 0x350a, 0x00);
         B5_OV4689_Write(pB5Chan, 0x350b, 0x0f);
         B5_OV4689_Write(pB5Chan, 0x350c, 0x80);
         //NEW_STG_HDR2
         B5_OV4689_Write(pB5Chan, 0x3760, 0x01);
         B5_OV4689_Write(pB5Chan, 0x3761, 0x2C);
         B5_OV4689_Write(pB5Chan, 0x3762, 0x00);
         B5_OV4689_Write(pB5Chan, 0x3763, 0x00);
         B5_OV4689_Write(pB5Chan, 0x3500, 0x00);
         B5_OV4689_Write(pB5Chan, 0x3501, 0x30);
         B5_OV4689_Write(pB5Chan, 0x3502, 0x00);
         B5_OV4689_Write(pB5Chan, 0x350a, 0x00);
         B5_OV4689_Write(pB5Chan, 0x350b, 0x06);
         B5_OV4689_Write(pB5Chan, 0x350c, 0x00);
         B5_OV4689_Write(pB5Chan, 0x3841, 0x03);
     } else {
         /* HDR_OFF */
         B5_OV4689_Write(pB5Chan, 0x5001, 0x11);
         B5_OV4689_Write(pB5Chan, 0x3837, 0x00);
         B5_OV4689_Write(pB5Chan, 0x3764, 0x00);
         B5_OV4689_Write(pB5Chan, 0x3760, 0x00);
         B5_OV4689_Write(pB5Chan, 0x3761, 0x00);
         B5_OV4689_Write(pB5Chan, 0x3762, 0x00);
         B5_OV4689_Write(pB5Chan, 0x3763, 0x00);
         B5_OV4689_Write(pB5Chan, 0x3500, 0x00);
         B5_OV4689_Write(pB5Chan, 0x3502, 0x00);
         B5_OV4689_Write(pB5Chan, 0x350a, 0x00);
         B5_OV4689_Write(pB5Chan, 0x350b, 0x00);
         B5_OV4689_Write(pB5Chan, 0x350c, 0x00);
         B5_OV4689_Write(pB5Chan, 0x3841, 0x02);
     }

     return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: B5_OV4689_GetStatus
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
static int B5_OV4689_GetStatus(AMBA_DSP_CHANNEL_ID_u Chan, AMBA_SENSOR_STATUS_INFO_s *pStatus)
{
    if (pStatus == NULL || B5_OV4689Ctrl.Status.DevStat.Bits.Sensor0Power == 0)
        return NG;

    B5_OV4689Ctrl.Status.TimeToAllPelExposed = 0;  /* TODO: for flash light or mechanical shutter control */

    memcpy(pStatus, &B5_OV4689Ctrl.Status, sizeof(AMBA_SENSOR_STATUS_INFO_s));

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: B5_OV4689_GetModeInfo
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
static int B5_OV4689_GetModeInfo(AMBA_DSP_CHANNEL_ID_u Chan, AMBA_SENSOR_MODE_ID_u Mode, AMBA_SENSOR_MODE_INFO_s *pModeInfo)
{
    AMBA_B5_CHANNEL_s B5Chan;
    UINT16 SensorMode = Mode.Bits.Mode;

    if (SensorMode == AMBA_SENSOR_CURRENT_MODE)
        SensorMode = B5_OV4689Ctrl.Status.ModeInfo.Mode.Bits.Mode;

    if (SensorMode >= AMBA_SENSOR_B5_OV4689_NUM_MODE || pModeInfo == NULL)
        return NG;

    B5_OV4689_SysChanConvert(Chan, &B5Chan);
    B5_OV4689_PrepareModeInfo(AmbaB5_GetNumActiveChannel(&B5Chan), SensorMode, pModeInfo, NULL, NULL);

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: B5_OV4689_GetDeviceInfo
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
static int B5_OV4689_GetDeviceInfo(AMBA_DSP_CHANNEL_ID_u Chan, AMBA_SENSOR_DEVICE_INFO_s *pDeviceInfo)
{
    if (pDeviceInfo == NULL)
        return NG;

    memcpy(pDeviceInfo, &B5_OV4689DeviceInfo, sizeof(AMBA_SENSOR_DEVICE_INFO_s));

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: B5_OV4689_GetCurrentGainFactor
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
static int B5_OV4689_GetCurrentGainFactor(AMBA_DSP_CHANNEL_ID_u Chan, float *pGainFactor)
{
    float AgcFactor = 1.0, DgcFactor = 1.0;
    UINT32 CurrentAgcCtrl;
    UINT32 CurrentDgcCtrl;
    UINT8 CurrentB5ChannelID = 0;
    UINT8 AgcR3508 = 0;
    UINT8 AgcR3509 = 0;

    /* Need to select a specific B5 channel id for getting current gain factor */
    if ((Chan.Bits.SensorID != 0x1) && (Chan.Bits.SensorID != 0x2) && (Chan.Bits.SensorID != 0x4) && (Chan.Bits.SensorID != 0x8))
        return NG;

    CurrentB5ChannelID = (UINT8)log2((float)Chan.Bits.SensorID);

    if (Chan.Bits.HdrID == 0x2) {
        CurrentAgcCtrl = B5_OV4689Ctrl.CurrentAEInfo.CurrentAgcCtrl[CurrentB5ChannelID][1]; // short exposure frame
        CurrentDgcCtrl = B5_OV4689Ctrl.CurrentAEInfo.CurrentDgcCtrl[CurrentB5ChannelID][1]; // short exposure frame
    } else {
        CurrentAgcCtrl = B5_OV4689Ctrl.CurrentAEInfo.CurrentAgcCtrl[CurrentB5ChannelID][0]; // long exposure frame
        CurrentDgcCtrl = B5_OV4689Ctrl.CurrentAEInfo.CurrentDgcCtrl[CurrentB5ChannelID][0]; // long exposure frame
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
 *  @RoutineName:: B5_OV4689_GetCurrentShutterSpeed
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
static int B5_OV4689_GetCurrentShutterSpeed(AMBA_DSP_CHANNEL_ID_u Chan, float *pExposureTime)
{
    UINT8 CurrentB5ChannelID = 0;

    /* Need to select a specific B5 channel id for getting current exposure time */
    if ((Chan.Bits.SensorID != 0x1) && (Chan.Bits.SensorID != 0x2) && (Chan.Bits.SensorID != 0x4) && (Chan.Bits.SensorID != 0x8))
        return NG;

    CurrentB5ChannelID = (UINT8)log2((float)Chan.Bits.SensorID);

    if (Chan.Bits.HdrID == 0x2)
        *pExposureTime = B5_OV4689Ctrl.Status.ModeInfo.RowTime * B5_OV4689Ctrl.CurrentAEInfo.CurrentShutterCtrl[CurrentB5ChannelID][1];
    else
        *pExposureTime = B5_OV4689Ctrl.Status.ModeInfo.RowTime * B5_OV4689Ctrl.CurrentAEInfo.CurrentShutterCtrl[CurrentB5ChannelID][0];

    //AmbaPrint("ExposureTime = %f", *pExposureTime);

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: B5_OV4689_ConvertGainFactor
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
static int B5_OV4689_ConvertGainFactor(AMBA_DSP_CHANNEL_ID_u Chan, float DesiredFactor, UINT32 *pActualFactor, UINT32 *pAnalogGainCtrl, UINT32 *pDigitalGainCtrl)
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

    *pAnalogGainCtrl = (AgcR3508 << 8) + AgcR3509;

    /* Dgc */
    DgcMsb = (UINT8)(DgcFactor * 2048 / 256);
    DgcRemainder = (UINT8)((int)(DgcFactor * 2048) % 64);
    DgcQuotient = (UINT16)((DgcFactor * 2048) / 64);
    ActualDgcFactor = ((float)DgcQuotient * 64 + DgcRemainder) / 2048;
    *pDigitalGainCtrl = (DgcQuotient << 16) + (DgcRemainder << 8) + DgcMsb;

    *pActualFactor = (UINT32)(ActualAgcFactor * ActualDgcFactor * 1048576);

    //AmbaPrint("DesiredFactor = %f, , ActualAgcFactor = %f, ActualDgcFactor = %f", DesiredFactor, ActualAgcFactor, ActualDgcFactor);
    //AmbaPrint("ActualFactor << 20 = 0x%08x", *pActualFactor);
    //AmbaPrint("AnalogGainCtrl = 0x%08x, DigitalGainCtrl = 0x%08x", *pAnalogGainCtrl, *pDigitalGainCtrl);

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: B5_OV4689_ConvertShutterSpeed
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
static int B5_OV4689_ConvertShutterSpeed(AMBA_DSP_CHANNEL_ID_u Chan, float ExposureTime, UINT32 *pShutterCtrl)
{
    AMBA_SENSOR_MODE_INFO_s *pModeInfo = &B5_OV4689Ctrl.Status.ModeInfo;

    if (pShutterCtrl == NULL)
        return NG;

    *pShutterCtrl = (UINT32)(ExposureTime / pModeInfo->RowTime);

    if (*pShutterCtrl <= 1)
        *pShutterCtrl = 1;

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: B5_OV4689_SetAnalogGainCtrl
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
static int B5_OV4689_SetAnalogGainCtrl(AMBA_DSP_CHANNEL_ID_u Chan, UINT32 AnalogGainCtrl)
{
    UINT8 AgcR3508 = (AnalogGainCtrl & 0x0000ff00) >> 8;
    UINT8 AgcR3509 = (AnalogGainCtrl & 0x000000ff);
    UINT8 TxData[4];
    int i;

    B5_OV4689_CurChan.SensorID = Chan.Bits.SensorID;

    /* For long exposure frame */
    TxData[0] = 0x3508 >> 8;
    TxData[1] = 0x3508 & 0xff;
    TxData[2] = AgcR3508;
    TxData[3] = AgcR3509;
    B5_OV4689_RegWrite(&B5_OV4689_CurChan, TxData, 4);

    /* Update current AGC control */
    for(i = 0; i < AMBA_NUM_B5_CHANNEL; i++) {
        if (B5_OV4689_CurChan.SensorID & (1 << i))
            B5_OV4689Ctrl.CurrentAEInfo.CurrentAgcCtrl[i][0] = AnalogGainCtrl;
    }

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: B5_OV4689_SetDigitalGainCtrl
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
static int B5_OV4689_SetDigitalGainCtrl(AMBA_DSP_CHANNEL_ID_u Chan, UINT32 DigitalGainCtrl)
{
    UINT8 AgcR352A = (DigitalGainCtrl & 0x000000ff);
    UINT8 AgcR352B = (DigitalGainCtrl & 0x0000ff00) >> 8;
    UINT8 TxData[4];
    int i;

    B5_OV4689_CurChan.SensorID = Chan.Bits.SensorID;

    /* For long exposure frame */
    TxData[0] = 0x352a >> 8;
    TxData[1] = 0x352a & 0xff;
    TxData[2] = AgcR352A;
    TxData[3] = AgcR352B;
    B5_OV4689_RegWrite(&B5_OV4689_CurChan, TxData, 4);

    /* Update current DGC control */
    for(i = 0; i < AMBA_NUM_B5_CHANNEL; i++) {
        if (B5_OV4689_CurChan.SensorID & (1 << i))
            B5_OV4689Ctrl.CurrentAEInfo.CurrentDgcCtrl[i][0] = DigitalGainCtrl;
    }

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: B5_OV4689_SetShutterCtrl
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
static int B5_OV4689_SetShutterCtrl(AMBA_DSP_CHANNEL_ID_u Chan, UINT32 ShutterCtrl)
{
    UINT8 TxData[5];
    AMBA_SENSOR_MODE_INFO_s *pModeInfo = &B5_OV4689Ctrl.Status.ModeInfo;
    int i;

    B5_OV4689_CurChan.SensorID = Chan.Bits.SensorID;

    /* Exposure line needs be smaller than VTS - 8 */
    if (ShutterCtrl >= pModeInfo->NumExposureStepPerFrame - 8)
        ShutterCtrl = pModeInfo->NumExposureStepPerFrame - 8;

    TxData[0] = 0x3500 >> 8;
    TxData[1] = 0x3500 & 0xff;
    TxData[2] = (ShutterCtrl & 0x000ff000) >> 12;
    TxData[3] = (ShutterCtrl & 0x00000ff0) >> 4;
    TxData[4] = (ShutterCtrl & 0x0000000f) << 4;

    B5_OV4689_RegWrite(&B5_OV4689_CurChan, TxData, 5);

    /* Update current shutter control */
    for(i = 0; i < AMBA_NUM_B5_CHANNEL; i++) {
        if (B5_OV4689_CurChan.SensorID & (1 << i))
            B5_OV4689Ctrl.CurrentAEInfo.CurrentShutterCtrl[i][0] = ShutterCtrl;
    }

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: B5_OV4689_SetSlowShutterCtrl
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
static int B5_OV4689_SetSlowShutterCtrl(AMBA_DSP_CHANNEL_ID_u Chan, UINT32 SlowShutterCtrl)
{
    UINT32 TargetFrameLengthLines;
    UINT8 TxData[4];

    if (SlowShutterCtrl < 1)
        SlowShutterCtrl = 1;

    TargetFrameLengthLines = B5_OV4689Ctrl.ModeInfo.FrameTiming.FrameLengthLines * SlowShutterCtrl;

    /* Update frame rate information */
    B5_OV4689Ctrl.Status.ModeInfo.FrameLengthLines = TargetFrameLengthLines;
    B5_OV4689Ctrl.Status.ModeInfo.NumExposureStepPerFrame = TargetFrameLengthLines;
    B5_OV4689Ctrl.Status.ModeInfo.FrameTime.FrameRate.TimeScale = B5_OV4689Ctrl.ModeInfo.FrameTiming.FrameRate.TimeScale / SlowShutterCtrl;

    if (B5_OV4689Ctrl.HdrInfo.HdrIsSupport == 1)
        TargetFrameLengthLines = TargetFrameLengthLines / 2;

    B5_OV4689_CurChan.SensorID = Chan.Bits.SensorID;

    TxData[0] = 0x380e >> 8;
    TxData[1] = 0x380e & 0xff;
    TxData[2] = (TargetFrameLengthLines & 0x0000ff00) >> 8;
    TxData[3] = (TargetFrameLengthLines & 0x000000ff);

    B5_OV4689_RegWrite(&B5_OV4689_CurChan, TxData, 4);

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: B5_OV4689_PowerOptimization
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
static void B5_OV4689_PowerOptimization(AMBA_B5_CHANNEL_s *pB5Chan)
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
 *  @RoutineName:: B5_OV4689_MipiPhyAdjust
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
static void B5_OV4689_MipiPhyAdjust(UINT16 SensorMode)
{
    const AMBA_SENSOR_OUTPUT_INFO_s *pOutputInfo = NULL;

    pOutputInfo = &B5_OV4689OutputInfo[B5_OV4689ModeInfoList[SensorMode].OutputMode];

    AmbaB5_VinCalculateDphyConfig(pOutputInfo->DataRate, &B5_OV4689_MipiVinConfig.MipiCtrl);
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: B5_OV4689_Config
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
static int B5_OV4689_Config(AMBA_DSP_CHANNEL_ID_u Chan, AMBA_SENSOR_MODE_ID_u Mode, AMBA_SENSOR_ESHUTTER_TYPE_e ElecShutterMode)
{
    AMBA_SENSOR_MODE_INFO_s *pModeInfo;
    B5_OV4689_MODE_INFO_s *pPrivateModeInfo;
    B5_OV4689_HDR_INFO_s *pHdrInfo;
    UINT16 SensorMode = Mode.Bits.Mode;
    static int NotFirstBoot = 0;

    if (SensorMode >= AMBA_SENSOR_B5_OV4689_NUM_MODE)
        return NG;

    AmbaPrint("============  [B5 OV4689]  ============");
    AmbaPrint("============ init mode:%d ============", SensorMode);

    B5_OV4689Ctrl.Status.ElecShutterMode = ElecShutterMode;

    /* For Second VIN */
    if (Chan.Bits.VinID == 1)
        AmbaVIN_Reset(AMBA_VIN_CHANNEL1, AMBA_VIN_LVDS, NULL);
    /* For Main VIN */
    else
        AmbaVIN_Reset(AMBA_VIN_CHANNEL0, AMBA_VIN_LVDS, NULL);

    AmbaB5_PllSwPllDisable(); // Disable sw pll

    if (NotFirstBoot == 1) {
        B5_OV4689_SetStandbyOn(&B5_OV4689_CurChan);
        AmbaB5_VoutReset();
        AmbaB5_VinReset(&B5_OV4689_CurChan);
    }

    B5_OV4689_CurChanConvert(Chan, &B5_OV4689_CurChan); // Convert CurChan based on SysChan and Chan

    //if (NotFirstBoot == 0) {
    if (1) { // hardware reset B5N/B5F each time
        int InitRval = 0;
        UINT8 BrokenFlag = 0;
        AmbaB5_SetPwmBrokenFlag(0); // Must reset the broken flag to 0
ReInit:
        B5_OV4689_HardwareReset(Chan);
        InitRval = AmbaB5_Init(&B5_OV4689_SysChan, &B5_OV4689_CurChan, &B5_OV4689_PinConfig);

        if (InitRval == NG) {
            BrokenFlag = AmbaB5_GetPwmBrokenFlag();
            //AmbaPrint("BrokenFlag = %d", BrokenFlag);
            Chan.Bits.SensorID = Chan.Bits.SensorID & ~BrokenFlag; // Block broken channels
            B5_OV4689_CurChanConvert(Chan, &B5_OV4689_CurChan); // Re-convert CurChan(passing broken channels)
            goto ReInit;
        }

        AmbaKAL_TaskSleep(10);
        AmbaB5_SetPwmBrokenFlag(BrokenFlag); // Set the actual broken flag(flag before re-init)
        SensorAeSem = AmbaB5_PllGetSwPllSem();
        AmbaB5_PllSwPllShowMsg(0); // Disable sw pll debug message
    }

    pModeInfo = &B5_OV4689Ctrl.Status.ModeInfo;
    pPrivateModeInfo = &B5_OV4689Ctrl.ModeInfo;
    pHdrInfo = &B5_OV4689Ctrl.HdrInfo;
    B5_OV4689_PrepareModeInfo(AmbaB5_GetNumActiveChannel(&B5_OV4689_CurChan), SensorMode, pModeInfo, pPrivateModeInfo, pHdrInfo);

    AmbaB5_PllSetSensorClk(&B5_OV4689_CurChan, B5_OV4689Ctrl.ModeInfo.FrameTiming.InputClk); // Must be set before programing sensor
    AmbaKAL_TaskSleep(3);

    //if (NotFirstBoot == 0) {
    if (1) { // hardware reset sensor each time
        B5_OV4689_SensorHardwareReset();
        B5_OV4689_InitSeq(&B5_OV4689_CurChan);
        NotFirstBoot = 1;
    }

    B5_OV4689_ChangeReadoutMode(&B5_OV4689_CurChan, SensorMode);
    B5_OV4689_HdrSeq(&B5_OV4689_CurChan, pPrivateModeInfo->ReadoutMode);
    B5_OV4689_PowerOptimization(&B5_OV4689_CurChan);

    /* Adjust MIPI-Phy parameters */
    B5_OV4689_MipiPhyAdjust(SensorMode);

    AmbaB5_VinPhySetMIPI(&B5_OV4689_CurChan, &B5_OV4689_MipiVinConfig.MipiCtrl);
    AmbaPrint("MIPI interface");

    AmbaB5_PrescalerInit(&B5_OV4689_CurChan, B5_OV4689OutputInfo[B5_OV4689ModeInfoList[SensorMode].OutputMode].RecordingPixels.Width, pPrivateModeInfo->CfaOutputWidth, 0);
    AmbaB5_Enable(&B5_OV4689_CurChan, pPrivateModeInfo->CfaOutputWidth, pModeInfo->OutputInfo.OutputHeight, pModeInfo->OutputInfo.NumDataBits, B5_OV4689_CompressRatio);
    AmbaB5_PllSwPllConfig(pPrivateModeInfo->FrameTiming.FrameRate.TimeScale /
                          pPrivateModeInfo->FrameTiming.FrameRate.NumUnitsInTick);

    B5_OV4689_SetStandbyOff(&B5_OV4689_CurChan);
    B5_OV4689_ConfigVin(Chan, pModeInfo, pPrivateModeInfo);

    AmbaB5_PllSwPllEnable(); // Enable sw pll

    B5_OV4689Ctrl.Status.DevStat.Bits.Sensor0Standby = 0;

    return OK;
}

/*-----------------------------------------------------------------------------------------------*/
AMBA_SENSOR_OBJ_s AmbaSensor_B5_OV4689Obj = {
    .Init                   = B5_OV4689_Init,
    .Enable                 = B5_OV4689_Enable,
    .Disable                = B5_OV4689_Disable,
    .Config                 = B5_OV4689_Config,
    .GetStatus              = B5_OV4689_GetStatus,
    .GetModeInfo            = B5_OV4689_GetModeInfo,
    .GetDeviceInfo          = B5_OV4689_GetDeviceInfo,
    .GetCurrentGainFactor   = B5_OV4689_GetCurrentGainFactor,
    .GetCurrentShutterSpeed = B5_OV4689_GetCurrentShutterSpeed,
    .ConvertGainFactor      = B5_OV4689_ConvertGainFactor,
    .ConvertShutterSpeed    = B5_OV4689_ConvertShutterSpeed,
    .SetAnalogGainCtrl      = B5_OV4689_SetAnalogGainCtrl,
    .SetDigitalGainCtrl     = B5_OV4689_SetDigitalGainCtrl,
    .SetShutterCtrl         = B5_OV4689_SetShutterCtrl,
    .SetSlowShutterCtrl     = B5_OV4689_SetSlowShutterCtrl,

    .RegisterRead           = B5_OV4689_RegisterRead,
    .RegisterWrite          = B5_OV4689_RegisterWrite,
};
