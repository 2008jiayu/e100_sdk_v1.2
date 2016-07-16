/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaSensor_AR0230.c
 *
 *  @Copyright      :: Copyright (C) 2014 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Control APIs of Aptina AR0230 CMOS sensor with HiSPi interface
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
#include "AmbaSensor_AR0230.h"


/*-----------------------------------------------------------------------------------------------*\
 * AR0230 sensor runtime info
\*-----------------------------------------------------------------------------------------------*/
static AR0230_CTRL_s AR0230Ctrl = {0};
AMBA_SENSOR_WB_GAIN_s AR0230DesiredWBGainFactor = {1.0, 1.0, 1.0, 1.0};

/*-----------------------------------------------------------------------------------------------*\
 * Configurations for Amba. VIN side
\*-----------------------------------------------------------------------------------------------*/
static AMBA_DSP_VIN_SLVS_PIN_PAIR_e AR0230LvdsLaneMux[][AMBA_DSP_NUM_VIN_SLVS_DATA_LANE] = {
    [AR0230_LVDS_4CH]    = {
        AMBA_DSP_VIN_PIN_SD_LVDS_0,
        AMBA_DSP_VIN_PIN_SD_LVDS_1,
        AMBA_DSP_VIN_PIN_SD_LVDS_2,
        AMBA_DSP_VIN_PIN_SD_LVDS_3
    },

    [AR0230_LVDS_2CH]    = {
        AMBA_DSP_VIN_PIN_SD_LVDS_0,
        AMBA_DSP_VIN_PIN_SD_LVDS_1,
    },

};


static AMBA_DSP_VIN_SLVS_CONFIG_s AR0230VinConfig = {
    .Info = {
        .FrameRate = {
            .Interlace        = 0,
            .TimeScale        = 0,
            .NumUnitsInTick = 0,
        },
        .DspPhaseShift        = {
            .Horizontal     =    AMBA_DSP_PHASE_SHIFT_MODE_0,
            .Vertical        =    AMBA_DSP_PHASE_SHIFT_MODE_0,
        },
        .BayerPattern        = AMBA_DSP_BAYER_GR,
        .NumDataBits        = 0,
        .NumSkipFrame        = 1,
    },
    .pLaneMapping    = NULL,
    .NumActiveLanes = 0,
    .SyncDetectCtrl = {
        .SyncInterleaving    = 2,
        .ITU656Type         = 0,
        .CustomSyncCode     = {
            .PatternAlign    = 1,        /* MSB */
            .SyncCodeMask    = 0xff00,
            .DetectEnable    = {
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
    .RxHvSyncCtrl    = {0, 0, 0, 0},
    .VinTrigPulse    = { {0}, {0} },
    .VinVoutSync    = { {AMBA_DSP_VIN_VOUT_SYNC_FIELD, 5}, {AMBA_DSP_VIN_VOUT_SYNC_FIELD, 5} }
};


/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AR0230_PrepareModeInfo
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
static void AR0230_PrepareModeInfo(UINT16 Mode, AMBA_SENSOR_MODE_INFO_s *pModeInfo)
{
    const AMBA_SENSOR_INPUT_INFO_s  *pInputInfo = &AR0230InputInfo[Mode];
    const AMBA_SENSOR_OUTPUT_INFO_s *pOutputInfo = &AR0230OutputInfo[Mode];

    pModeInfo->Mode.Data = Mode;
    pModeInfo->LineLengthPck = AR0230ModeInfoList[Mode].FrameTiming.Linelengthpck;
    pModeInfo->FrameLengthLines = AR0230ModeInfoList[Mode].FrameTiming.FrameLengthLines;

    //AmbaPrint("LLP:%d, FLL:%d",pModeInfo->LineLengthPck,pModeInfo->FrameLengthLines);

    if(AR0230ModeInfoList[Mode].OperationMode == AR0230_OPERATION_MODE_INTERLEAVE)
        pModeInfo->NumExposureStepPerFrame = (AR0230ModeInfoList[Mode].FrameTiming.FrameLengthLines / 2);
    else
        pModeInfo->NumExposureStepPerFrame = AR0230ModeInfoList[Mode].FrameTiming.FrameLengthLines;

    pModeInfo->PixelRate = AR0230ModeInfoList[Mode].PixelRate;
    pModeInfo->RowTime = AR0230ModeInfoList[Mode].RowTime;
    pModeInfo->InternalExposureOffset = 0;
    pModeInfo->FrameTime.InputClk = AR0230ModeInfoList[Mode].FrameTiming.InputClk;
    memcpy(&pModeInfo->FrameTime.FrameRate, &(AR0230ModeInfoList[Mode].FrameTiming.FrameRate), sizeof(AMBA_DSP_FRAME_RATE_s));
    memcpy(&pModeInfo->InputInfo, pInputInfo, sizeof(AMBA_SENSOR_INPUT_INFO_s));
    memcpy(&pModeInfo->OutputInfo, pOutputInfo, sizeof(AMBA_SENSOR_OUTPUT_INFO_s));
    memcpy(&pModeInfo->MinFrameRate, &(AR0230ModeInfoList[Mode].FrameTiming.FrameRate), sizeof(AMBA_DSP_FRAME_RATE_s));

    /* Only support 3 stage slow shutter */
    if(AR0230ModeInfoList[Mode].HdrType == AMBA_SENSOR_HDR_NONE)
        pModeInfo->MinFrameRate.TimeScale /= 8;

    /* HDR information */
    pModeInfo->HdrInfo.HdrType = AR0230ModeInfoList[Mode].HdrType;
    pModeInfo->HdrInfo.ActiveChannels = AR0230ModeInfoList[Mode].ActiveChannels;
    if(AR0230ModeInfoList[Mode].HdrType != AMBA_SENSOR_HDR_NONE) {
        pModeInfo->HdrInfo.NotSupportIndividualGain = 1;
    }
    memcpy(&pModeInfo->HdrInfo.ChannelInfo[0], &(AR0230HdrLongExposureInfo[Mode]), sizeof(AMBA_SENSOR_HDR_CHANNEL_INFO_s));
    memcpy(&pModeInfo->HdrInfo.ChannelInfo[1], &(AR0230HdrShortExposureInfo[Mode]), sizeof(AMBA_SENSOR_HDR_CHANNEL_INFO_s));

}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AR0230_ConfigVin
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
static void AR0230_ConfigVin(AMBA_VIN_CHANNEL_e VinChanNo, AMBA_SENSOR_MODE_INFO_s *pModeInfo)
{
    AMBA_SENSOR_OUTPUT_INFO_s  *pImgInfo = &pModeInfo->OutputInfo;
    AMBA_DSP_VIN_SLVS_CONFIG_s *pVinCfg = &AR0230VinConfig;

    AmbaDSP_VinPhySetSLVS(AMBA_VIN_CHANNEL0);

    memcpy(&pVinCfg->Info.FrameRate, &pModeInfo->FrameTime.FrameRate, sizeof(AMBA_DSP_FRAME_RATE_s));
    memcpy(&pVinCfg->Info.DspPhaseShift, &pImgInfo->DspPhaseShift, sizeof(AMBA_DSP_PHASE_SHIFT_CTRL_s));
    pVinCfg->Info.BayerPattern = (AMBA_DSP_SENSOR_PATTERN_e) pImgInfo->CfaPattern;
    pVinCfg->Info.NumDataBits = pImgInfo->NumDataBits;

    pVinCfg->NumActiveLanes = pImgInfo->NumDataLanes;
    if (pVinCfg->NumActiveLanes == 4) {
        pVinCfg->pLaneMapping = AR0230LvdsLaneMux[AR0230_LVDS_4CH];
        pVinCfg->SyncDetectCtrl.SyncInterleaving = 2;
    }
    else if (pVinCfg->NumActiveLanes == 2){
        pVinCfg->pLaneMapping = AR0230LvdsLaneMux[AR0230_LVDS_2CH];
        pVinCfg->SyncDetectCtrl.SyncInterleaving = 1;
    }
    pVinCfg->RxHvSyncCtrl.NumActivePixels = 0;
    pVinCfg->RxHvSyncCtrl.NumActiveLines  = pImgInfo->OutputHeight;
    //AmbaPrint("LLP:%d, FLL:%d",pModeInfo->LineLengthPck,pModeInfo->FrameLengthLines);
    pVinCfg->RxHvSyncCtrl.NumTotalPixels  = pModeInfo->LineLengthPck - 1;
    pVinCfg->RxHvSyncCtrl.NumTotalLines   = pModeInfo->FrameLengthLines;

    AmbaDSP_VinConfigSLVS(AMBA_VIN_CHANNEL0, pVinCfg);
}

static int AR0230_RegWrite(UINT16 Addr, UINT16 Data)
{
    UINT8 TxData[4];

    TxData[0] = (UINT8) ((Addr & 0xff00) >> 8);
    TxData[1] = (UINT8) (Addr & 0x00ff);
    TxData[2] = (UINT8) ((Data & 0xff00) >> 8);
    TxData[3] = (UINT8) (Data & 0x00ff);

    if (AmbaI2C_Write(AMBA_I2C_CHANNEL0,
                      AMBA_I2C_SPEED_FAST,
                      0x20, 4, TxData,
                      AMBA_KAL_WAIT_FOREVER) == NG)
        AmbaPrint("I2C does not work!!!!!");

    //AmbaPrint("[AR0230] Addr=0x%04X, Data=0x%04X (%u)", Addr, Data, Data);
    //AmbaPrintk_Flush();

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AR0230_RegisterWrite
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
static int AR0230_RegisterWrite(AMBA_DSP_CHANNEL_ID_u Chan, UINT16 Addr, UINT16 Data)
{
    AR0230_RegWrite(Addr, Data);

    return OK;
}

#if 0
static int AR0230_RegByteRead(UINT16 Addr, UINT8 *pRxData)
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

static int AR0230_RegRead(UINT16 Addr, UINT16 *pRxData)
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
 *  @RoutineName:: AR0230_RegisterRead
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
static int AR0230_RegisterRead(AMBA_DSP_CHANNEL_ID_u Chan, UINT16 Addr, UINT16* Data)
{
    AR0230_RegRead(Addr, Data);

    return OK;
}


static void AR0230_SetRegR0x301A(UINT32 RestartFrame, UINT32 Streaming)
{
    UINT16 R0x301A;

    R0x301A =
            0 << 12                  | // 1: disable serial interface (HiSPi)
            0 << 11                  | // force_pll_on
            0 << 10                  | // DONT restart if bad frame is detected
            0 << 9                   | // The sensor will produce bad frames as some register changed
            0 << 8                   | // input buffer related to GPI0/1/2/3 inputs are powered down & disabled
            0 << 7                   | // Parallel data interface is enabled (dep on bit[6])
            1 << 6                   | // Parallel interface is driven
            1 << 4                   | // reset_reg_unused
            1 << 3                   | // Forbids to change value of SMIA registers
            (Streaming > 0) << 2     | // Put the sensor in streaming mode
            (RestartFrame > 0) << 1  | // Causes sensor to truncate frame at the end of current row and start integrating next frame
            0 << 0;                    // Set the bit initiates a reset sequence

    // Typically, in normal streamming mode (restart_frame=0, streaming=1), the value is 0x10DC
    AR0230_RegWrite(0x301A, R0x301A);
}

static int AR0230_ConfirmStreamingMode(void)
{
    int PollingCnt = 200, RetStatus;
    UINT16 Data = 0xFFFF;

    /* Make sure that sensor is streaming */
    while (1) {
        /* Read frame count */
        AR0230_RegRead(0x303A, &Data);

        //AmbaPrint("confirm_streaming: frame_count = 0x%04X", Data);

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

static int AR0230_ConfirmStandbyMode(void)
{
    int PollingCnt = 200, RetStatus;
    UINT16 Data = 0xFFFF;

    while (1) {
        /* Read frame status */
        AR0230_RegRead(0x303C, &Data);

        //AmbaPrint("confirm_standby: R0x303C = 0x%04X", Data);

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
 *  @RoutineName:: AR0230_HardwareReset
 *
 *  @Description:: Reset (HW) AR0230 Image Sensor Device
 *
 *  @Input      ::
 *      VinChanNo:          Vin channel number
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static inline void AR0230_HardwareReset(AMBA_VIN_CHANNEL_e VinChanNo)
{
    AmbaUserGPIO_SensorResetCtrl(0);
    AmbaKAL_TaskSleep(10);
    AmbaUserGPIO_SensorResetCtrl(1);
    AmbaKAL_TaskSleep(10);

    AmbaPrint("Sensor HW Reset");
    if (AR0230_ConfirmStandbyMode() == -1) {
        AmbaPrint("cannot enter standby mode after HW reset!");
    }
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AR0230_SoftwareReset
 *
 *  @Description:: Reset (SW) AR0230 Image Sensor Device
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static inline void AR0230_SoftwareReset(void)
{
    /* R0x301A[0]: software reset */
    AR0230_RegWrite(0x301A, 0x0059);

    /* Wait for internal initialization */
    AmbaKAL_TaskSleep(10);

    //AmbaPrint("Sensor SW Reset");
    if (AR0230_ConfirmStandbyMode() == -1) {
        AmbaPrint("cannot enter standby mode after SW reset!");
    }
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AR0230_SetStandbyOn
 *
 *  @Description:: Configure sensor into standby mode
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static void AR0230_SetStandbyOn(void)
{
    AR0230_SetRegR0x301A(0/*restart_frame*/, 0/*streaming*/);

    //AR0230_RegWrite(0x301A, 0x0058);

    if (AR0230_ConfirmStandbyMode() == -1)
        AmbaPrint("cannot enter standby mode!");
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AR0230_SetStandbyOff
 *
 *  @Description:: Configure sensor to leave standby mode.
 *
 *  @Input      ::
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static void AR0230_SetStandbyOff(void)
{
    AR0230_SetRegR0x301A(0/*restart_frame*/, 1/*streaming*/);

    //AR0230_RegWrite(0x301A, 0x005C);

    if (AR0230_ConfirmStreamingMode() == -1)
        AmbaPrint("cannot enter streaming mode");
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AR0230_ConfigOptSetting
 *
 *  @Description:: Configure sensor optimized setting.
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static int AR0230_ConfigOptSetting(void)
{
    int i;
    UINT16 Addr, Data;

    /* Configure sensor PLL */
    for (i = 0; i < AR0230_NUM_SEQUENCER_OPTIMIZED_REG; i++) {
        Addr = AR0230OptSeqRegTable[i].Addr;
        Data = AR0230OptSeqRegTable[i].Data[0];
        AR0230_RegWrite(Addr, Data);
    }

    AmbaKAL_TaskSleep(1);

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AR0230_ConfigLinearSeq
 *
 *  @Description:: Configure sensor linear mode setting.
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static int AR0230_ConfigLinearSeq(void)
{
    int i;
    UINT16 Addr, Data;

    /* Configure sensor PLL */
    for (i = 0; i < AR0230_NUM_SEQUENCER_LINEAR_REG; i++) {
        Addr = AR0230LinearSeqRegTable[i].Addr;
        Data = AR0230LinearSeqRegTable[i].Data[0];
        AR0230_RegWrite(Addr, Data);
    }

    AmbaKAL_TaskSleep(1);

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AR0230_ConfigHDRSeq
 *
 *  @Description:: Configure sensor HDR mode setting.
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static int AR0230_ConfigHDRSeq(void)
{
    int i;
    UINT16 Addr, Data;

    /* Configure sensor PLL */
    for (i = 0; i < AR0230_NUM_SEQUENCER_HDR_REG; i++) {
        Addr = AR0230HDRSeqRegTable[i].Addr;
        Data = AR0230HDRSeqRegTable[i].Data[0];
        AR0230_RegWrite(Addr, Data);
    }

    AmbaKAL_TaskSleep(1);

    return OK;
}


/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AR0230_ChangeReadoutMode
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
static int AR0230_ChangeReadoutMode(UINT16 Mode)
{

    int i;
    UINT16 Addr, Data;

    for (i = 0; i < AR0230_NUM_READOUT_MODE_REG; i++) {
        Addr = AR0230RegTable[i].Addr;
        Data = AR0230RegTable[i].Data[Mode];
        if (Data == NA)
            continue;
        AR0230_RegWrite(Addr, Data);
    }

    AR0230Ctrl.CurrentAgc = 0xffffffff;
    AR0230Ctrl.CurrentDgc = 0xffff;
    AR0230Ctrl.CurrentShtCtrl= 0xffffffff;

#if 0
    for(i = 0; i < AR0230_NUM_READOUT_MODE_REG; i ++) {
        UINT8 Rval = 0;
        AR0230_RegRead(AR0230RegTable[i].Addr, &Rval);
        AmbaPrint("Reg Read: Addr: 0x%x, data: 0x%x", AR0230RegTable[i].Addr, Rval);
    }
#endif
    return OK;
}

static void AR0230_PreCalculateModeInfo(void)
{
    int i;

    for (i = 0; i < AMBA_SENSOR_AR0230_NUM_MODE; i++) {
        if (AR0230ModeInfoList[i].PllSetting == AR0230_PLL_1) {
            AR0230ModeInfoList[i].PixelRate = (double)AR0230ModeInfoList[i].FrameTiming.InputClk * 66 / 4 / 6 * 2;
            AR0230ModeInfoList[i].RowTime =
                (double)AR0230ModeInfoList[i].FrameTiming.Linelengthpck / ((double)AR0230ModeInfoList[i].FrameTiming.InputClk * 66 / 4 / 6 * 2);
        } else if (AR0230ModeInfoList[i].PllSetting == AR0230_PLL_2) {
            AR0230ModeInfoList[i].PixelRate = (double)AR0230ModeInfoList[i].FrameTiming.InputClk * 154 / 8 / 7 * 2;
            AR0230ModeInfoList[i].RowTime =
                (double)AR0230ModeInfoList[i].FrameTiming.Linelengthpck / ((double)AR0230ModeInfoList[i].FrameTiming.InputClk * 154 / 8 / 7 * 2);
        } else if (AR0230ModeInfoList[i].PllSetting == AR0230_PLL_3) {
            AR0230ModeInfoList[i].PixelRate = (double)AR0230ModeInfoList[i].FrameTiming.InputClk * 66 / 4 / 12 * 2;
            AR0230ModeInfoList[i].RowTime =
                (double)AR0230ModeInfoList[i].FrameTiming.Linelengthpck / ((double)AR0230ModeInfoList[i].FrameTiming.InputClk * 66 / 4 / 12 * 2);
        } else if (AR0230ModeInfoList[i].PllSetting == AR0230_PLL_4) {
            AR0230ModeInfoList[i].PixelRate = (double)AR0230ModeInfoList[i].FrameTiming.InputClk * 154 / 8 / 14 * 2;
            AR0230ModeInfoList[i].RowTime =
                (double)AR0230ModeInfoList[i].FrameTiming.Linelengthpck / ((double)AR0230ModeInfoList[i].FrameTiming.InputClk * 154 / 8 / 14 * 2);
        }
    }
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AR0230_Init
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
static int AR0230_Init(AMBA_DSP_CHANNEL_ID_u Chan)
{
    const AR0230_FRAME_TIMING_s *pFrameTime = &AR0230ModeInfoList[0].FrameTiming;
    AmbaPLL_SetSensorClk(pFrameTime->InputClk);             /* The default sensor input clock frequency */
    AmbaPLL_SetSensorClkDir(AMBA_PLL_SENSOR_CLK_OUTPUT);    /* Config clock output to sensor (or sensor SPI won't work) */

    AR0230_PreCalculateModeInfo();
    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AR0230_Enable
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
static int AR0230_Enable(AMBA_DSP_CHANNEL_ID_u Chan)
{
    if (AR0230Ctrl.Status.DevStat.Bits.Sensor0Power == 1)
        return OK;

    AR0230_HardwareReset((AMBA_VIN_CHANNEL_e) Chan.Bits.VinID);
    AR0230_SoftwareReset();

    AR0230Ctrl.Status.DevStat.Bits.Sensor0Power   = 1;
    AR0230Ctrl.Status.DevStat.Bits.Sensor0Standby = 1;

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AR0230_Disable
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
static int AR0230_Disable(AMBA_DSP_CHANNEL_ID_u Chan)
{
    if (AR0230Ctrl.Status.DevStat.Bits.Sensor0Power == 0)
        return OK;

    AR0230_SetStandbyOn();

    AR0230Ctrl.Status.DevStat.Bits.Sensor0Power = 0;

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AR0230_ChangeFrameRate
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
static int AR0230_ChangeFrameRate(AMBA_DSP_CHANNEL_ID_u Chan, AMBA_DSP_FRAME_RATE_s *pFrameRate)
{
    return NG;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AR0230_SetGainFactor
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
static int AR0230_SetGainFactor(AMBA_DSP_CHANNEL_ID_u Chan, float GainFactor)
{
    return NG;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AR0230_SetEshutterSpeed
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
static int AR0230_SetEshutterSpeed(AMBA_DSP_CHANNEL_ID_u Chan, float ShutterSpeed)
{
    return NG;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AR0230_EnableALTM
 *
 *  @Description:: set AR0230 ALTM
 *
 *  @Input      ::
 *      Enable:    Enable ALTM or not
 *
 *  @Output     ::
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static void AR0230_EnableALTM(UINT8 Enable)
{
    if(Enable) {
        AR0230_RegWrite(0x2420, 0x0000);
        AR0230_RegWrite(0x2440, 0x0004);
        AR0230_RegWrite(0x2442, 0x0080);
        AR0230_RegWrite(0x301E, 0x0000);//Data Pedestal(Black Level)
        AR0230_RegWrite(0x2450, 0x0000);//ALTM Pedstal
        AR0230_RegWrite(0x320A, 0x0080);
        AR0230_RegWrite(0x31D0, 0x0000);
        AR0230_RegWrite(0x2400, 0x0002);//Enable ALTM 0x2400[0] = 0
        AR0230_RegWrite(0x2410, 0x0005);
        AR0230_RegWrite(0x2412, 0x002D);
        AR0230_RegWrite(0x2444, 0xF400);
        AR0230_RegWrite(0x2446, 0x0001);
        AR0230_RegWrite(0x2438, 0x0010);
        AR0230_RegWrite(0x243A, 0x0012);
        AR0230_RegWrite(0x243C, 0xFFFF);
        AR0230_RegWrite(0x243E, 0x0100);
    } else {
        AR0230_RegWrite(0x2400, 0x0001);//Disable ALTM 0x2400[0] = 1 Disable 0x2400[1] for power saving 20150402
        AR0230_RegWrite(0x301E, 0x00A8);//Data Pedestal(Black Level)
        AR0230_RegWrite(0x2450, 0x0000);
        AR0230_RegWrite(0x320A, 0x0080);
    }
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AR0230_Enable2DMC
 *
 *  @Description:: Set AR0230 2DMC
 *
 *  @Input      ::
 *      Enable:    Enable 2DMC or not
 *
 *  @Output     ::
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static void AR0230_Enable2DMC(UINT8 Enable)
{
    if(Enable) {
        AR0230_RegWrite(0x3190, 0x0000);
        AR0230_RegWrite(0x318A, 0x0E74);
        AR0230_RegWrite(0x318C, 0x4000);//Disable smoothing on 2DMC 20150402
        AR0230_RegWrite(0x3192, 0x0400);
        AR0230_RegWrite(0x3198, 0x183C);//Motion_detect_q1 0x3198[7:0]; Motion_detect_q2 0x3198[15:8]
    } else {
        AR0230_RegWrite(0x318C, 0x0000);
    }
}
#if 0
/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AR0230_EnableDLO
 *
 *  @Description:: Set AR0230 DLO
 *
 *  @Input      ::
 *      Enable:    Enable DLO or not
 *
 *  @Output     ::
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static void AR0230_EnableDLO(UINT8 Enable)
{
    if(Enable) {
        AR0230_RegWrite(0x3190, 0xE000);
        AR0230_RegWrite(0x3194, 0x0BB8);
        AR0230_RegWrite(0x3196, 0x0E74);

    } else {
        AR0230_RegWrite(0x3190, 0xE000);
    }
}
#endif
/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AR0230_EnableCompanding
 *
 *  @Description:: Set AR0230 Companding
 *
 *  @Input      ::
 *      Enable:    Enable Companding or not
 *
 *  @Output     ::
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static void AR0230_EnableCompanding(UINT8 Enable)
{
    if(Enable) {
        AR0230_RegWrite(0x31AC, 0x100C);
        AR0230_RegWrite(0x31D0, 0x0001);
    } else {
        AR0230_RegWrite(0x31D0, 0x0000);
    }
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AR0230_SetADACD
 *
 *  @Description:: Set AR0230 ADACD
 *
 *  @Input      ::
 *      Operation:    ADACD operation mode
 *
 *  @Output     ::
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static void AR0230_SetADACD(UINT32 Operation)
{
    UINT16 R0x3200;
    switch(Operation) {
        case AR0230_ADACD_DISABLE:
            AR0230_RegWrite(0x3200, 0x0000);//disable AdaCD 0x3200[1] = 0
            break;
        case AR0230_ADACD_LOWLIGHT:
            AR0230_RegRead(0x3200, &R0x3200);
            R0x3200 |= 2;
            AR0230_RegWrite(0x3200, R0x3200);//Enable AdaCD 0x3200[1] = 1
            break;
        case AR0230_ADACD_LOWDCG:
            AR0230_RegWrite(0x3206, 0x0B08);//adacd_noise_floor1
            AR0230_RegWrite(0x3208, 0x1E13);//adacd_noise_floor2
            AR0230_RegWrite(0x3202, 0x0080);//Adjusts AdaCD filter strength
            AR0230_RegWrite(0x3200, 0x02);
            break;
        case AR0230_ADACD_HIGHDCG:
            AR0230_RegWrite(0x3206, 0x1C0E);//adacd_noise_floor1
            AR0230_RegWrite(0x3208, 0x4E39);//adacd_noise_floor2
            AR0230_RegWrite(0x3202, 0x00B0);//Adjusts AdaCD filter strength
            AR0230_RegWrite(0x3200, 0x02);
            break;
    }
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AR0230_SetConversionGain
 *
 *  @Description:: Set AR0230 Conversion Gain Mode
 *
 *  @Input      ::
 *      Operation:    Conversion Gain Mode
 *
 *  @Output     ::
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static void AR0230_SetConversionGain(UINT8 OperationMode, UINT8 ConvGain)
{
    if (OperationMode == AR0230_OPERATION_MODE_LINEAR) {
        if (ConvGain == AR0230_LOW_CONVERSION_GAIN) {
            //AR0230_RegWrite(0x3060, 0x000B);
            AR0230_RegWrite(0x3096, 0x0080);//ROW_NOISE_ADJUST_TOP
            AR0230_RegWrite(0x3098, 0x0080);//ROW_NOISE_ADJUST_BTM
            AR0230_SetADACD(AR0230_ADACD_DISABLE);
            AR0230_RegWrite(0x3100, 0x0000);//Enable LCG
        } else {
            //AR0230_RegWrite(0x3060, 0x000B);
            AR0230_RegWrite(0x3096, 0x0080);//ROW_NOISE_ADJUST_TOP
            AR0230_RegWrite(0x3098, 0x0080);//ROW_NOISE_ADJUST_BTM
            AR0230_SetADACD(AR0230_ADACD_DISABLE);
            AR0230_RegWrite(0x3100, 0x0004);//Enable HCG additional 2.7x analog gain
        }
    } else {
        if (ConvGain == AR0230_LOW_CONVERSION_GAIN) {
            //AR0230_RegWrite(0x3060, 0x000B);
            AR0230_RegWrite(0x3096, 0x0480);//ROW_NOISE_ADJUST_TOP
            AR0230_RegWrite(0x3098, 0x0480);//ROW_NOISE_ADJUST_BTM
            AR0230_SetADACD(AR0230_ADACD_LOWDCG);
            AR0230_RegWrite(0x3100, 0x0000);//Enable LCG
        } else {
            //AR0230_RegWrite(0x3060, 0x000B);
            AR0230_RegWrite(0x3096, 0x0780);//ROW_NOISE_ADJUST_TOP
            AR0230_RegWrite(0x3098, 0x0780);//ROW_NOISE_ADJUST_BTM
            AR0230_SetADACD(AR0230_ADACD_HIGHDCG);
            AR0230_RegWrite(0x3100, 0x0004);//Enable HCG additional 2.7x analog gain
        }
    }
}


/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AR0230_SetHDRRatio
 *
 *  @Description:: Set AR0230 HDR ratio
 *
 *  @Input      ::
 *      Ratio:    HDR ratio
 *
 *  @Output     ::
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static void AR0230_SetHDRRatio(UINT8 Ratio)
{
    switch(Ratio) {
        case 32:
            AR0230_RegWrite(0x3082, 0x000C);
            break;
        case 16:
            AR0230_RegWrite(0x3082, 0x0008);
            break;
        case 8:
            AR0230_RegWrite(0x3082, 0x0004);
            break;
        case 4:
            AR0230_RegWrite(0x3082, 0x0000);
            break;
    }
    AR0230Ctrl.CurrentHDRRatio = Ratio;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AR0230_SetHiSPi
 *
 *  @Description:: Set AR0230 HiSPi
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static void AR0230_SetHiSPi(UINT8 NumLane)
{
    if(NumLane == 4)
        AR0230_RegWrite(0x31AE, 0x0304);
    else if(NumLane == 2)
        AR0230_RegWrite(0x31AE, 0x0302);

    AR0230_RegWrite(0x31C6, 0x0006);
    AR0230_RegWrite(0x306E, 0x9210);
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AR0230_SensorModeInit
 *
 *  @Description:: Set AR0230 sensor init register
 *
 *  @Input      ::
 *      Mode    Sensor mode
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static void AR0230_SensorModeInit(UINT32 Mode)
{
    if(AR0230ModeInfoList[Mode].OperationMode == AR0230_OPERATION_MODE_HDR_LINEAR) {
        AR0230_ConfigHDRSeq();
        AR0230_ConfigOptSetting();
        AR0230_RegWrite(0x30B0, 0x0118); //Update R0x30B0 at init 20150402
        AR0230_SetHDRRatio(16);
        //AR0230_RegWrite(0x30BA, 0x770C); //Remove R0x30BA setting from HDR init 20150402
        AR0230_RegWrite(0x31E0, 0x0200);  //2DDC Enable
        AR0230_EnableALTM(0);
        AR0230_Enable2DMC(1);
        AR0230_RegWrite(0x3060, 0x000B); //Set default analog gain 1.5x
        AR0230_SetConversionGain(AR0230ModeInfoList[Mode].OperationMode, AR0230_LOW_CONVERSION_GAIN);
        AR0230_EnableCompanding(0);
        AR0230_RegWrite(0x31AC, 0x100E); //data_format_bits
        AR0230_RegWrite(0x318E, 0x0000); //Disable gain_before_dlo 20150402

        AR0230_SetHiSPi(AR0230OutputInfo[Mode].NumDataLanes);
        AR0230_ChangeReadoutMode(Mode);
    } else if (AR0230ModeInfoList[Mode].OperationMode == AR0230_OPERATION_MODE_HDR_ALTM){
        AR0230_ConfigHDRSeq();
        AR0230_ConfigOptSetting();
        AR0230_RegWrite(0x30B0, 0x0118); //Update R0x30B0 at init 20150402
        AR0230_RegWrite(0x31AC, 0x0C0C); //data_format_bits
        AR0230_SetHDRRatio(16);
        //AR0230_RegWrite(0x30BA, 0x770C); //Remove R0x30BA setting from HDR init 20150402
        AR0230_RegWrite(0x31E0, 0x0200);  //2DDC Enable
        AR0230_EnableALTM(1);
        AR0230_Enable2DMC(1);
        AR0230_RegWrite(0x3060, 0x000B); //Set default analog gain 1.5x
        AR0230_SetConversionGain(AR0230ModeInfoList[Mode].OperationMode, AR0230_LOW_CONVERSION_GAIN);
        AR0230_RegWrite(0x318E, 0x0000); //Disable gain_before_dlo 20150402

        AR0230_SetHiSPi(AR0230OutputInfo[Mode].NumDataLanes);
        AR0230_ChangeReadoutMode(Mode);
    } else if (AR0230ModeInfoList[Mode].OperationMode == AR0230_OPERATION_MODE_LINEAR){
        AR0230_ConfigLinearSeq();
        AR0230_ConfigOptSetting();
        AR0230_RegWrite(0x30B0, 0x0118); //Update R0x30B0 at init 20150402
        AR0230_RegWrite(0x31AC, 0x0C0C); //data_format_bits
        AR0230_RegWrite(0x3082, 0x0009);
        AR0230_RegWrite(0x30BA, 0x769C); //Update R0x30BA setting 20150402
        AR0230_RegWrite(0x31E0, 0x0200); //2DDC Enable
        AR0230_EnableALTM(0);
        AR0230_Enable2DMC(0);
        AR0230_RegWrite(0x3060, 0x000B); //Set default analog gain 1.5x
        AR0230_SetConversionGain(AR0230ModeInfoList[Mode].OperationMode, AR0230_LOW_CONVERSION_GAIN);
        AR0230_SetADACD(AR0230_ADACD_DISABLE);
        AR0230_EnableCompanding(0);

        AR0230_SetHiSPi(AR0230OutputInfo[Mode].NumDataLanes);
        AR0230_ChangeReadoutMode(Mode);
        AR0230Ctrl.CurrentHDRRatio = 1;
    } else if (AR0230ModeInfoList[Mode].OperationMode == AR0230_OPERATION_MODE_INTERLEAVE){
        AR0230_ConfigHDRSeq();
        AR0230_ConfigOptSetting();
        AR0230_RegWrite(0x30B0, 0x0118); //Update R0x30B0 at init 20150402
        AR0230_RegWrite(0x31AC, 0x0C0C); //data_format_bits
        AR0230_SetHDRRatio(8);
        //AR0230_RegWrite(0x30BA, 0x770C); //Remove R0x30BA setting from HDR init 20150402
        AR0230_RegWrite(0x31E0, 0x0200);  //2DDC Enable
        AR0230_SetADACD(AR0230_ADACD_DISABLE);
        AR0230_Enable2DMC(0);
        AR0230_RegWrite(0x3060, 0x000B); //Set default analog gain 1.5x
        AR0230_SetConversionGain(AR0230ModeInfoList[Mode].OperationMode, AR0230_LOW_CONVERSION_GAIN);
        AR0230_EnableALTM(0);
        AR0230_EnableCompanding(0);
        AR0230_RegWrite(0x318E, 0x1000); //Disable gain_before_dlo 20150402

        AR0230_SetHiSPi(AR0230OutputInfo[Mode].NumDataLanes);
        AR0230_ChangeReadoutMode(Mode);
    }
}

#if 0 /* for debugging */
static void AR0230_SequencerDump(UINT8 mode)
{
    int i = 0;
    UINT8 DataH = 0xFF;
    UINT8 DataL = 0xFF;
    UINT16 RegNum = 0;

    if(mode) {
        RegNum = AR0230_NUM_SEQUENCER_HDR_REG;
        AR0230_RegWrite(0x3088, 0xC000);
    } else {
        RegNum = AR0230_NUM_SEQUENCER_LINEAR_REG;
        AR0230_RegWrite(0x3088, 0xC242);
    }

    AmbaPrint("----- Sequencer -----");
    for (i = 1; i <= RegNum - 1  /* max entry num of sequencers */; i++) {
        AR0230_RegByteRead(0x3086, &DataH);
        AR0230_RegByteRead(0x3086, &DataL);
        AmbaPrint("%4d 0x%04X", i, (DataH << 8) | DataL);
    }
}

int AR0230_RegDump(void)
{
    int i, NumRegs = 0;
    UINT16 Data = 0;

    /* Stop streaming with R0x301A[5]=1 */
    AR0230_SetRegR0x301A(0/*restart_frame*/, 0/*streaming*/);

    if (AR0230_ConfirmStandbyMode() == -1)
        AmbaPrint("cannot enter standby mode!");


    /* dump sequencer */
    if(AR0230ModeInfoList[AR0230Ctrl.Status.ModeInfo.Mode.Bits.Mode].OperationMode == AR0230_OPERATION_MODE_LINEAR)
        AR0230_SequencerDump(0);
    else
        AR0230_SequencerDump(1);
    /* Start streaming */
    AR0230_SetRegR0x301A(0/*restart_frame*/, 1/*streaming*/);

    return OK;
}
#endif

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AR0230_GetStatus
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
static int AR0230_GetStatus(AMBA_DSP_CHANNEL_ID_u Chan, AMBA_SENSOR_STATUS_INFO_s *pStatus)
{
    if (pStatus == NULL || AR0230Ctrl.Status.DevStat.Bits.Sensor0Power == 0)
        return NG;

    AR0230Ctrl.Status.TimeToAllPelExposed = 0;  /* TODO: for flash light or mechanical shutter control */

    memcpy(pStatus, &AR0230Ctrl.Status, sizeof(AMBA_SENSOR_STATUS_INFO_s));
    //AmbaPrint("AR0230_GetStatus()");
    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AR0230_GetModeInfo
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
static int AR0230_GetModeInfo(AMBA_DSP_CHANNEL_ID_u Chan, AMBA_SENSOR_MODE_ID_u Mode, AMBA_SENSOR_MODE_INFO_s *pModeInfo)
{
    UINT16 SensorMode = Mode.Bits.Mode; // TODO

    if (SensorMode == AMBA_SENSOR_CURRENT_MODE)
        SensorMode = AR0230Ctrl.Status.ModeInfo.Mode.Bits.Mode;

    if (SensorMode >= AMBA_SENSOR_AR0230_NUM_MODE || pModeInfo == NULL)
        return NG;

    AR0230_PrepareModeInfo(SensorMode, pModeInfo);
    //AmbaPrint("AR0230_GetModeInfo()");
    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AR0230_GetDeviceInfo
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
static int AR0230_GetDeviceInfo(AMBA_DSP_CHANNEL_ID_u Chan, AMBA_SENSOR_DEVICE_INFO_s *pDeviceInfo)
{
    if (pDeviceInfo == NULL)
        return NG;

    memcpy(pDeviceInfo, &AR0230DeviceInfo, sizeof(AMBA_SENSOR_DEVICE_INFO_s));
    //AmbaPrint("AR0230_GetDeviceInfo()");
    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AR0230_GetHdrInfo
 *
 *  @Description:: Get Sensor HDR Info
 *
 *  @Input      ::
 *      Chan:           Vin ID and sensor ID
 *      ShutterCtrl:    pointer to sensor exposure line
 *
 *  @Output     ::
 *      pHdrInfo:       pointer to Hdr info
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static int AR0230_GetHdrInfo(AMBA_DSP_CHANNEL_ID_u Chan, UINT32 *pShutterCtrl, AMBA_SENSOR_HDR_INFO_s *pHdrInfo)
{
    UINT16 SensorMode = AR0230Ctrl.Status.ModeInfo.Mode.Bits.Mode;

    if(AR0230ModeInfoList[SensorMode].OperationMode == AR0230_OPERATION_MODE_INTERLEAVE) {
        UINT32 NumExpStep = *pShutterCtrl >> 1; /* unit of pShutterCtrl is row */
        UINT32 MinShrWidth = 0;
        UINT32 MaxShrWidth = 0;
        UINT32 ShortExposure = 0;
        UINT16 T1T2Ratio = AR0230Ctrl.CurrentHDRRatio;

        if (pHdrInfo == NULL)
            return NG;

        if (T1T2Ratio == 32)
            MinShrWidth = T1T2Ratio * 2;
        else
            MinShrWidth = T1T2Ratio;

        MaxShrWidth = AR0230Ctrl.Status.ModeInfo.NumExposureStepPerFrame * T1T2Ratio / (T1T2Ratio + 1);
        if(MaxShrWidth > AR0230Ctrl.Status.ModeInfo.NumExposureStepPerFrame - 70)
            MaxShrWidth = AR0230Ctrl.Status.ModeInfo.NumExposureStepPerFrame -70;
        MaxShrWidth -= 6;

        if(NumExpStep > MaxShrWidth)
            NumExpStep = MaxShrWidth;
        if(NumExpStep < MinShrWidth)
            NumExpStep = MinShrWidth;

        ShortExposure = NumExpStep / T1T2Ratio;

        AR0230Ctrl.Status.ModeInfo.HdrInfo.ChannelInfo[1].EffectiveArea.StartY = ShortExposure + 2;

        memcpy(pHdrInfo, &AR0230Ctrl.Status.ModeInfo.HdrInfo, sizeof(AMBA_SENSOR_HDR_INFO_s));
#if 0
        AmbaPrint("HDR channel number:%d" ,AR0230_HdrInfo.ActiveChannels);
        for(int i = 0; i < AR0230_HdrInfo.ActiveChannels; ++i)
            AmbaPrint("Channel_%d StartX:%d, StartY:%d Width:%d, Height:%d" ,i,AR0230_HdrInfo.EffectiveArea[i].StartX, AR0230_HdrInfo.EffectiveArea[i].StartY, AR0230_HdrInfo.EffectiveArea[i].Width, AR0230_HdrInfo.EffectiveArea[i].Height);
#endif
    } else
        memcpy(pHdrInfo, &AR0230Ctrl.Status.ModeInfo.HdrInfo, sizeof(AMBA_SENSOR_HDR_INFO_s));

   /*T2 could be read by register R0x307C*/

    return OK;

}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AR0230_GetCurrentGainFactor
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
static int AR0230_GetCurrentGainFactor(AMBA_DSP_CHANNEL_ID_u Chan, float *pGainFactor)
{
    *pGainFactor = AR0230AgcRegTable[AR0230Ctrl.CurrentAgc].Factor * AR0230Ctrl.CurrentDgc / 133;

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AR0230_GetCurrentShutterSpeed
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
static int AR0230_GetCurrentShutterSpeed(AMBA_DSP_CHANNEL_ID_u Chan, float *pExposureTime)
{
    if(AR0230ModeInfoList[AR0230Ctrl.Status.ModeInfo.Mode.Bits.Mode].OperationMode == AR0230_OPERATION_MODE_INTERLEAVE)
       *pExposureTime = (AR0230Ctrl.Status.ModeInfo.RowTime * (float)(AR0230Ctrl.CurrentShtCtrl << 1));
    else
       *pExposureTime = (AR0230Ctrl.Status.ModeInfo.RowTime * (float)AR0230Ctrl.CurrentShtCtrl);

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AR0230_ConvertGainFactor
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
static int AR0230_ConvertGainFactor(AMBA_DSP_CHANNEL_ID_u Chan, float DesiredFactor, UINT32 *pActualFactor, UINT32 *pAnalogGainCtrl, UINT32 *pDigitalGainCtrl)
{
    float AgcGain = 0.0;
    float DgcGain = 0.0;
    float MaxAgcGain = 0.0;
    float MinAgcGain = 1.52;
    UINT16 Mode = AR0230Ctrl.Status.ModeInfo.Mode.Bits.Mode;
    UINT32 AgcTableIdx;
    //AmbaPrint("Original DesiredFactor:%f, ConvGain:%d,",DesiredFactor,ConvGain);

    (AR0230ModeInfoList[Mode].OperationMode == AR0230_OPERATION_MODE_LINEAR) ? (MaxAgcGain = 43.2) : (MaxAgcGain = 32.4);

    if (DesiredFactor < MinAgcGain)
        DesiredFactor = MinAgcGain;
    else if (DesiredFactor > MaxAgcGain)
        DesiredFactor = MaxAgcGain;

    for (AgcTableIdx = 0; AgcTableIdx < (AR0230_NUM_AGC_STEP - 1); AgcTableIdx ++) {
        if(DesiredFactor < AR0230AgcRegTable[AgcTableIdx + 1].Factor)
            break;
    }

    /* linear mode does not support 32.4x */
    if(AR0230ModeInfoList[Mode].OperationMode == AR0230_OPERATION_MODE_LINEAR) {
        if (AR0230AgcRegTable[AgcTableIdx].Data == 0x003B) {
            AgcTableIdx --;
        }
    }

    AgcGain = AR0230AgcRegTable[AgcTableIdx].Factor;
    DgcGain = (DesiredFactor / AgcGain);

    *pAnalogGainCtrl  = AgcTableIdx;   /* index of agc table */
    *pDigitalGainCtrl = DgcGain * 133; /* the setting of R0x305E */
    *pActualFactor = (UINT32)(1024 * 1024 * AgcGain * DgcGain);

//    AmbaPrint("DesiredFactor = %f, DgcR305E = 0x%08x, AgcR3060 = 0x%08x, AgcGain = %f, DgcGain = %f, ActualFactor = %u",
//      DesiredFactor, *pDigitalGainCtrl, *pAnalogGainCtrl, AgcGain, DgcGain, *pActualFactor);

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AR0230_ConvertWbGainFactor
 *
 *  @Description:: Convert HDR WB gain factor to WB gain control
 *
 *  @Input      ::
 *      Chan:             Vin ID and sensor ID
 *      DesiredFactor:      Desired WB gain factor
 *
 *  @Output     ::
 *      pWBGainCtrl:        HDR WB gain control
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static int AR0230_ConvertWbGainFactor(AMBA_DSP_CHANNEL_ID_u Chan, AMBA_SENSOR_WB_GAIN_s DesiredWBGainFactor, AMBA_SENSOR_WB_GAIN_s *pActualWBGainFactor,  AMBA_SENSOR_WB_CTRL_s *pWBGainCtrl)
{
#if 0
    float Dgc = 0.0;
    UINT16 DgcReg = 0;

    AR0230DesiredWBGainFactor.R = DesiredWBGainFactor.R;
    AR0230DesiredWBGainFactor.Gr = DesiredWBGainFactor.Gr;
    AR0230DesiredWBGainFactor.B = DesiredWBGainFactor.B;
    AR0230DesiredWBGainFactor.Gb = DesiredWBGainFactor.Gb;

    Dgc = DesiredWBGainFactor.R;
    DgcReg = (UINT16)(Dgc * 133);
    pWBGainCtrl->R = DgcReg;
    pActualWBGainFactor->R = (float)DgcReg / 133.0;

    Dgc = DesiredWBGainFactor.Gr;
    DgcReg = (UINT16)(Dgc * 133);
    pWBGainCtrl->Gr = DgcReg;
    pActualWBGainFactor->Gr = (float)DgcReg / 133.0;

    Dgc = DesiredWBGainFactor.Gb;
    DgcReg = (UINT16)(Dgc * 133);
    pWBGainCtrl->Gb = DgcReg;
    pActualWBGainFactor->Gb = (float)DgcReg / 133.0;

    Dgc = DesiredWBGainFactor.B;
    DgcReg = (UINT16)(Dgc * 133);
    pWBGainCtrl->B = DgcReg;
    pActualWBGainFactor->B = (float)DgcReg / 133.0;

    //AmbaPrint("DesiredWBGainFactor R:%f, Gr:%f, Gb:%f, B:%f", DesiredWBGainFactor.R, DesiredWBGainFactor.Gr, DesiredWBGainFactor.Gb, DesiredWBGainFactor.B);
    //AmbaPrint("WBGainCtrl R:%d, Gr:%d, Gb:%d, B:%d", pWBGainCtrl.R, pWBGainCtrl.Gr, pWBGainCtrl.Gb, pWBGainCtrl.B);
    //AmbaPrint("ActualWBGainFactor R:%f, Gr:%f, Gb:%f, B:%f", pActualWBGainFactor.R, pActualWBGainFactor.Gr, pActualWBGainFactor.Gb, pActualWBGainFactor.B);

    return OK;
#else
    return NG;
#endif
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AR0230_ConvertShutterSpeed
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
static int AR0230_ConvertShutterSpeed(AMBA_DSP_CHANNEL_ID_u Chan, float ExposureTime, UINT32 *pShutterCtrl)
{
    UINT16 Mode = AR0230Ctrl.Status.ModeInfo.Mode.Bits.Mode;
    UINT32 NumExposureStepPerFrame = 0;
    UINT32 MinShrWidth = 0;
    UINT32 MaxShrWidth = 0;
    UINT16 T1T2Ratio = AR0230Ctrl.CurrentHDRRatio;

    if (pShutterCtrl == NULL)
        return NG;

    if (AR0230ModeInfoList[Mode].OperationMode != AR0230_OPERATION_MODE_LINEAR) {
        /* exp time of SF cannot be configured directly. it's controlled by exp. time of LF.
           we need to calculate exp. info of SF by LF(SF * T1T2Ratio) */
        if (Chan.Bits.HdrID == 2)
            ExposureTime *= T1T2Ratio;
    }

    /* calculate exp. steps */
    if (AR0230ModeInfoList[Mode].OperationMode == AR0230_OPERATION_MODE_INTERLEAVE) {
        NumExposureStepPerFrame = AR0230ModeInfoList[Mode].FrameTiming.FrameLengthLines >> 1;
        *pShutterCtrl = (UINT32)(ExposureTime / (AR0230ModeInfoList[Mode].RowTime * 2));
    } else {
        NumExposureStepPerFrame = AR0230ModeInfoList[Mode].FrameTiming.FrameLengthLines;
        *pShutterCtrl = (UINT32)(ExposureTime / AR0230ModeInfoList[Mode].RowTime);
    }

    /* limit exp. setting to a valid value */
    if(AR0230ModeInfoList[Mode].OperationMode == AR0230_OPERATION_MODE_LINEAR) {
        MaxShrWidth= ((*pShutterCtrl - 1) / NumExposureStepPerFrame + 1) * NumExposureStepPerFrame - 4;
        MinShrWidth = 1;
        //AmbaPrint("[Linear]MaxShrWidth:%d, MinShrWidth,:%d, ShutterCtrl:%d",MaxShrWidth,MinShrWidth,ShutterCtrl);
    } else if (AR0230ModeInfoList[Mode].OperationMode == AR0230_OPERATION_MODE_HDR_ALTM ||
        AR0230ModeInfoList[Mode].OperationMode == AR0230_OPERATION_MODE_HDR_LINEAR){
        if (T1T2Ratio == 32)
            MinShrWidth = T1T2Ratio * 2;
        else
            MinShrWidth = T1T2Ratio;

        MaxShrWidth = 70 * T1T2Ratio;
        if(MaxShrWidth > NumExposureStepPerFrame - 70)
            MaxShrWidth = NumExposureStepPerFrame -70;
        MaxShrWidth -= 6;
        //AmbaPrint("[HDR]T1T2Ratio:%d, MaxShrWidth:%d, MinShrWidth,:%d, ShutterCtrl:%d",T1T2Ratio,MaxShrWidth,MinShrWidth,ShutterCtrl);
    } else if (AR0230ModeInfoList[Mode].OperationMode == AR0230_OPERATION_MODE_INTERLEAVE){
        if (T1T2Ratio == 32)
            MinShrWidth = T1T2Ratio * 2;
        else
            MinShrWidth = T1T2Ratio;

        /* 2015.9.7, update shutter time limitaion as
           Max T1 = min (HDR_ratio*(FLL - y_output-16), FLL *HDR_ratio/(HDR_ratio+1)) */
        MaxShrWidth = NumExposureStepPerFrame * T1T2Ratio / (T1T2Ratio + 1);
        if(MaxShrWidth >
            (T1T2Ratio * (NumExposureStepPerFrame - AR0230InputInfo[Mode].PhotodiodeArray.Height - 16)))
            MaxShrWidth = T1T2Ratio * (NumExposureStepPerFrame - AR0230InputInfo[Mode].PhotodiodeArray.Height - 16);

        //AmbaPrint("[Interleave]T1T2Ratio:%d, MaxShrWidth:%d, MinShrWidth,:%d, ShutterCtrl:%d",T1T2Ratio,MaxShrWidth,MinShrWidth,*pShutterCtrl);
    }

    if (*pShutterCtrl > MaxShrWidth)
        *pShutterCtrl = MaxShrWidth;
    if (*pShutterCtrl < MinShrWidth)
        *pShutterCtrl = MinShrWidth;

    if (Chan.Bits.HdrID == 2)
        *pShutterCtrl /= T1T2Ratio;

    /* the unit of ShutterCtrl is Row */
    if(AR0230ModeInfoList[Mode].OperationMode == AR0230_OPERATION_MODE_INTERLEAVE)
        *pShutterCtrl = *pShutterCtrl << 1;

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AR0230_SetAnalogGainCtrl
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
static int AR0230_SetAnalogGainCtrl(AMBA_DSP_CHANNEL_ID_u Chan, UINT32 AnalogGainCtrl)
{
    AR0230_SetConversionGain(AR0230ModeInfoList[AR0230Ctrl.Status.ModeInfo.Mode.Bits.Mode].OperationMode,
        AR0230AgcRegTable[AnalogGainCtrl].ConvGain);
    AR0230_RegWrite(0x3060, AR0230AgcRegTable[AnalogGainCtrl].Data);

    AR0230Ctrl.CurrentAgc = AnalogGainCtrl;

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AR0230_SetHdrAnalogGainCtrl
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
static int AR0230_SetHdrAnalogGainCtrl(AMBA_DSP_CHANNEL_ID_u Chan, UINT32 *AnalogGainCtrl)
{
    UINT32 AgcTableIdx = *AnalogGainCtrl;

    if (((Chan.Bits.HdrID & 0x1) && (Chan.Bits.HdrID & 0x2)) || (Chan.Bits.HdrID & 0x1)) {
        AR0230_SetConversionGain(AR0230ModeInfoList[AR0230Ctrl.Status.ModeInfo.Mode.Bits.Mode].OperationMode,
            AR0230AgcRegTable[AgcTableIdx].ConvGain);
        AR0230_RegWrite(0x3060, AR0230AgcRegTable[AgcTableIdx].Data);

        AR0230Ctrl.CurrentAgc = *AnalogGainCtrl;
    }

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AR0230_SetDigitalGainCtrl
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
static int AR0230_SetDigitalGainCtrl(AMBA_DSP_CHANNEL_ID_u Chan, UINT32 DigitalGainCtrl)
{
    UINT16 Data = (UINT16) DigitalGainCtrl;
    AR0230_RegWrite(0x305E, Data);

    AR0230Ctrl.CurrentDgc = DigitalGainCtrl;

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AR0230_SetHdrDigitalGainCtrl
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
static int AR0230_SetHdrDigitalGainCtrl(AMBA_DSP_CHANNEL_ID_u Chan, UINT32 *DigitalGainCtrl)
{
#if 0
    UINT32 Dgc = *DigitalGainCtrl;
    AR0230_RegWrite(0x3056, (UINT16)(AR0230DesiredWBGainFactor.Gr * Dgc));
    AR0230_RegWrite(0x3058, (UINT16)(AR0230DesiredWBGainFactor.B * Dgc));
    AR0230_RegWrite(0x305A, (UINT16)(AR0230DesiredWBGainFactor.R * Dgc));
    AR0230_RegWrite(0x305C, (UINT16)(AR0230DesiredWBGainFactor.Gb * Dgc));

    return OK;
#else
    UINT16 Data = (UINT16) *DigitalGainCtrl;
    AR0230_RegWrite(0x305E, Data);

    AR0230Ctrl.CurrentDgc = *DigitalGainCtrl;

    return OK;
#endif
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AR0230_SetShutterCtrl
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
static int AR0230_SetShutterCtrl(AMBA_DSP_CHANNEL_ID_u Chan, UINT32 ShutterCtrl)
{
    UINT16 Data = (UINT16)ShutterCtrl;

    if(AR0230ModeInfoList[AR0230Ctrl.Status.ModeInfo.Mode.Bits.Mode].OperationMode == AR0230_OPERATION_MODE_INTERLEAVE)
        /* the unit of shutter register is (2xRow) */
        Data = Data >> 1;

    AR0230_RegWrite(0x3012, Data);
    AR0230Ctrl.CurrentShtCtrl = Data;

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AR0230_SetHdrShutterCtrl
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
static int AR0230_SetHdrShutterCtrl(AMBA_DSP_CHANNEL_ID_u Chan, UINT32 *ShutterCtrl)
{
    UINT16 Mode = AR0230Ctrl.Status.ModeInfo.Mode.Bits.Mode;
    UINT16 LFData = (UINT16)(*ShutterCtrl++);
    //UINT16 SFData = (UINT16)(*ShutterCtrl);
    //UINT16 T1T2Ratio = AR0230Ctrl.CurrentHDRRatio;

    /* only integration time of Long exp slice can be configured */
    if (Chan.Bits.HdrID & 0x1) {
        if(AR0230ModeInfoList[Mode].OperationMode == AR0230_OPERATION_MODE_INTERLEAVE)
            /* the unit of shutter register is (2xRow) */
            LFData = LFData >> 1;

        AR0230_RegWrite(0x3012, LFData);
        AR0230Ctrl.CurrentShtCtrl = LFData;

        return OK;
    } else
        return NG;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName::AR0230_SetHdrWbGainCtrl
 *
 *  @Description:: Set HDR WB gain control
 *
 *  @Input      ::
 *      Chan:         Vin ID and sensor ID
 *      pWBGainCtrl:  HDR WB gain control
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static int AR0230_SetHdrWbGainCtrl(AMBA_DSP_CHANNEL_ID_u Chan, AMBA_SENSOR_WB_CTRL_s *pWBGainCtrl)
{
#if 0
    AR0230_RegWrite(0x3056, (UINT16)(pWBGainCtrl->Gr * AR0230Ctrl.DigitalGain));
    AR0230_RegWrite(0x3058, (UINT16)(pWBGainCtrl->B * AR0230Ctrl.DigitalGain));
    AR0230_RegWrite(0x305A, (UINT16)(pWBGainCtrl->R * AR0230Ctrl.DigitalGain));
    AR0230_RegWrite(0x305C, (UINT16)(pWBGainCtrl->Gb * AR0230Ctrl.DigitalGain));

    return OK;
#else
    return NG;
#endif
}


/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AR0230_SetSlowShutterCtrl
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
static int AR0230_SetSlowShutterCtrl(AMBA_DSP_CHANNEL_ID_u Chan, UINT32 SlowShutterCtrl)
{
    UINT16 Mode = AR0230Ctrl.Status.ModeInfo.Mode.Bits.Mode;

    if (SlowShutterCtrl < 1)
        SlowShutterCtrl = 1;

    if (AR0230ModeInfoList[Mode].OperationMode == AR0230_OPERATION_MODE_HDR_ALTM ||
        AR0230ModeInfoList[Mode].OperationMode == AR0230_OPERATION_MODE_HDR_LINEAR){

        UINT32 TargetLineLengthPck = AR0230ModeInfoList[Mode].FrameTiming.Linelengthpck * SlowShutterCtrl;
        AR0230Ctrl.Status.ModeInfo.LineLengthPck = TargetLineLengthPck;
        AR0230Ctrl.Status.ModeInfo.FrameTime.FrameRate.NumUnitsInTick = AR0230ModeInfoList[Mode].FrameTiming.FrameRate.NumUnitsInTick * SlowShutterCtrl;
        AR0230Ctrl.Status.ModeInfo.RowTime = TargetLineLengthPck / AR0230Ctrl.Status.ModeInfo.PixelRate;

        AR0230_RegWrite(0x300C, (UINT16)TargetLineLengthPck);
        //AmbaPrint("LLP:%d, FLL:%d",AR0230Ctrl.Status.ModeInfo.LineLengthPck, AR0230Ctrl.Status.ModeInfo.FrameLengthLines);
        //AmbaPrint("[AR0230]HDR slowshutter Linelengthpck:%d, SlowShutterCtrl:%d, TargetLineLengthPck:%d", AR0230Ctrl.FrameTime.Linelengthpck, SlowShutterCtrl, TargetLineLengthPck);
    } else {
        UINT32 TargetFrameLengthLines = AR0230ModeInfoList[Mode].FrameTiming.FrameLengthLines * SlowShutterCtrl;

        AR0230Ctrl.Status.ModeInfo.FrameLengthLines = TargetFrameLengthLines;
        AR0230Ctrl.Status.ModeInfo.FrameTime.FrameRate.NumUnitsInTick = AR0230ModeInfoList[Mode].FrameTiming.FrameRate.NumUnitsInTick * SlowShutterCtrl;

        if(AR0230ModeInfoList[Mode].OperationMode == AR0230_OPERATION_MODE_INTERLEAVE) {
            AR0230Ctrl.Status.ModeInfo.NumExposureStepPerFrame = TargetFrameLengthLines >> 1;
            AR0230_RegWrite(0x300A, (UINT16)TargetFrameLengthLines >> 1);
        } else {
            AR0230Ctrl.Status.ModeInfo.NumExposureStepPerFrame = TargetFrameLengthLines;
            AR0230_RegWrite(0x300A, (UINT16)TargetFrameLengthLines);
        }
        //AmbaPrint("[AR0230]Linear slowshutter FrameLengthLines:%d, SlowShutterCtrl:%d, TargetFrameLengthLines:%d", AR0230Ctrl.FrameTime.FrameLengthLines, SlowShutterCtrl, TargetFrameLengthLines);
    }

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AR0230_Config
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
static int AR0230_Config(AMBA_DSP_CHANNEL_ID_u Chan, AMBA_SENSOR_MODE_ID_u Mode, AMBA_SENSOR_ESHUTTER_TYPE_e ElecShutterMode)
{
    AMBA_SENSOR_MODE_INFO_s *pModeInfo = NULL;
    UINT16 SensorMode = Mode.Bits.Mode;
    AMBA_VIN_CHANNEL_e VinChanNo = (AMBA_VIN_CHANNEL_e) Chan.Bits.VinID;

    if (SensorMode >= AMBA_SENSOR_AR0230_NUM_MODE)
        return NG;

    AmbaPrint("============  [ AR0230 ]  ============");
    AmbaPrint("============ init mode:%d ============", SensorMode);

    AR0230Ctrl.Status.ElecShutterMode = ElecShutterMode;

    pModeInfo  = &AR0230Ctrl.Status.ModeInfo;
    AR0230_PrepareModeInfo(SensorMode, pModeInfo);

    /* Make VIN not to capture bad frames during readout mode transition */
    AmbaVIN_Reset((AMBA_VIN_CHANNEL_e) Chan.Bits.VinID, AMBA_VIN_LVDS, NULL);

    AmbaKAL_TaskSleep(3);

    AmbaPLL_SetSensorClk(pModeInfo->FrameTime.InputClk);

    AmbaKAL_TaskSleep(3);

    AR0230_SoftwareReset();

    AR0230_SensorModeInit(SensorMode);

    AR0230_SetStandbyOff();

    /* Reset vin again after vin receive the sensor clock */
    AmbaVIN_Reset((AMBA_VIN_CHANNEL_e) Chan.Bits.VinID, AMBA_VIN_LVDS, NULL);

    AR0230_ConfigVin(VinChanNo, pModeInfo);

    AR0230Ctrl.Status.DevStat.Bits.Sensor0Standby = 0;

    return OK;
}

/*-----------------------------------------------------------------------------------------------*/
AMBA_SENSOR_OBJ_s AmbaSensor_AR0230Obj = {
    .Init                   = AR0230_Init,
    .Enable                 = AR0230_Enable,
    .Disable                = AR0230_Disable,
    .Config                 = AR0230_Config,
    .ChangeFrameRate        = AR0230_ChangeFrameRate,
    .GetStatus              = AR0230_GetStatus,
    .GetModeInfo            = AR0230_GetModeInfo,
    .GetDeviceInfo          = AR0230_GetDeviceInfo,
    .GetCurrentGainFactor   = AR0230_GetCurrentGainFactor,
    .GetCurrentShutterSpeed = AR0230_GetCurrentShutterSpeed,

    .ConvertGainFactor      = AR0230_ConvertGainFactor,
    .ConvertShutterSpeed    = AR0230_ConvertShutterSpeed,

    .ConvertWbGainFactor    = AR0230_ConvertWbGainFactor,
    .SetHdrWbGainCtrl       = AR0230_SetHdrWbGainCtrl,

    .SetAnalogGainCtrl      = AR0230_SetAnalogGainCtrl,
    .SetDigitalGainCtrl     = AR0230_SetDigitalGainCtrl,
    .SetShutterCtrl         = AR0230_SetShutterCtrl,
    .SetSlowShutterCtrl     = AR0230_SetSlowShutterCtrl,

    .SetGainFactor          = AR0230_SetGainFactor,
    .SetEshutterSpeed       = AR0230_SetEshutterSpeed,

    .SetHdrAnalogGainCtrl   = AR0230_SetHdrAnalogGainCtrl,
    .SetHdrDigitalGainCtrl  = AR0230_SetHdrDigitalGainCtrl,
    .SetHdrShutterCtrl      = AR0230_SetHdrShutterCtrl,

    .RegisterRead           = AR0230_RegisterRead,
    .RegisterWrite          = AR0230_RegisterWrite,

    .GetHdrInfo             = AR0230_GetHdrInfo,

};

