/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaSensor_TI5150.c
 *
 *  @Copyright      :: Copyright (C) 2013 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Control APIs of OmniVision TI5150 CMOS sensor with MIPI interface
\*-------------------------------------------------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "AmbaDataType.h"
#include "AmbaKAL.h"

#include "AmbaVIN.h"
#include "AmbaDSP_VIN.h"
#include "AmbaI2C.h"
#include "AmbaGPIO.h"
#include "AmbaPLL.h"
#include "AmbaYuv.h"
#include "AmbaYuv_TI5150.h"

#include "AmbaPrintk.h"

/*-----------------------------------------------------------------------------------------------*\
 * TI5150 runtime status
\*-----------------------------------------------------------------------------------------------*/
AMBA_YUV_STATUS_INFO_s TI5150Status = {0};

/*-----------------------------------------------------------------------------------------------*\
 * Configurations for Amba. VIN side
\*-----------------------------------------------------------------------------------------------*/
static AMBA_DSP_VIN_DVP_CONFIG_s TI5150VinConfig = {
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
        .ColorSpace         = AMBA_DSP_COLOR_SPACE_YUV,
        .YuvOrder           = AMBA_DSP_CB_Y0_CR_Y1,
        .NumDataBits        = 8,
        .NumSkipFrame       = 1,
    },
    .DvpType        = AMBA_DSP_VIN_DVP_SINGLE_PEL_SDR,
    .SyncType       = AMBA_DSP_VIN_SYNC_BT656_LOWER_PEL,
    .DataClockEdge  = AMBA_DSP_VIN_SIGNAL_RISING_EDGE,
    .HsyncPolarity  = AMBA_DSP_VIN_SIGNAL_FALLING_EDGE,
    .VsyncPolarity  = AMBA_DSP_VIN_SIGNAL_FALLING_EDGE,
    .FieldPolarity  = AMBA_DSP_VIN_SIGNAL_FALLING_EDGE,
    .SyncPinSelect  = {
        .FieldPinSelect = 3,
        .VsyncPinSelect = 15,
        .HsyncPinSelect = 13
    },
    .RxHvSyncCtrl   = {0, 0, 0, 0},
    .VinVoutSync            = {
        {
            .SignalFreq = AMBA_DSP_VIN_VOUT_SYNC_FRAME,
            .SignalLine = 1,
        },
        {
            .SignalFreq = AMBA_DSP_VIN_VOUT_SYNC_FRAME,
            .SignalLine = 1,
        },
    }
};

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: TI5150_ConfigVin
 *
 *  @Description:: Configure VIN to receieve output data of the new mode
 *
 *  @Input      ::
 *      pModeInfo:  Details of the specified Yuv mode
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static void TI5150_ConfigVin(AMBA_YUV_OUTPUT_INFO_s *pOutputInfo)
{
    AMBA_DSP_VIN_DVP_CONFIG_s  *pVinCfg = &TI5150VinConfig;

    pVinCfg->RxHvSyncCtrl.NumActivePixels  = pOutputInfo->OutputWidth;
    pVinCfg->RxHvSyncCtrl.NumActiveLines   = pOutputInfo->OutputHeight;

    pVinCfg->Info.YuvOrder = pOutputInfo->YuvOrder;

    memcpy(&pVinCfg->Info.FrameRate, &pOutputInfo->FrameRate, sizeof(AMBA_DSP_FRAME_RATE_s));

    AmbaDSP_VinConfigDVP(AMBA_VIN_CHANNEL0, pVinCfg);
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: TI5150_HardwareReset
 *
 *  @Description:: Reset TI5150 Yuv Device
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static inline void TI5150_HardwareReset(void)
{
    AmbaUserGPIO_SensorResetCtrl(0);
    AmbaKAL_TaskSleep(3);
    AmbaUserGPIO_SensorResetCtrl(1);
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: TI5150_RegWrite
 *
 *  @Description:: Write TI5150 register through I2C
 *
 *  @Input      ::
 *              Addr: register address
 *              Data: the data to be written
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static int TI5150_RegWrite(UINT16 Addr, UINT8 Data)
{
    UINT8 TxDataBuf[2];

    TxDataBuf[0] = (UINT8) (Addr & 0x00ff);
    TxDataBuf[1] = Data;

    if (AmbaI2C_Write(AMBA_I2C_CHANNEL0, AMBA_I2C_SPEED_STANDARD,
                      0xba, 2, TxDataBuf,
                      AMBA_KAL_WAIT_FOREVER) == NG)
        AmbaPrint("I2C does not work!!!!!");

    //AmbaPrint("Addr = 0x%04x, Data = 0x%02x", Addr, Data);

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: TI5150_RegRead
 *
 *  @Description:: Read TI5150 register through I2C
 *
 *  @Input      ::
 *              Addr: register address
 *
 *  @Output     ::
 *              pRxData: received register data
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static int TI5150_RegRead(UINT16 Addr, UINT8 *pRxData)
{
    UINT16 pTxData[3];

    pTxData[0] = AMBA_I2C_RESTART_FLAG | (0xba);
    pTxData[1] = (Addr & 0xff);  /* Register Address [7:0]  */
    pTxData[2] = AMBA_I2C_RESTART_FLAG | (0xbb);

    return AmbaI2C_ReadAfterWrite(AMBA_I2C_CHANNEL0, AMBA_I2C_SPEED_STANDARD,
                                  3, (UINT16 *) pTxData, 1, pRxData, AMBA_KAL_WAIT_FOREVER);
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: TI5150_ChangeYuvMode
 *
 *  @Description:: Switch device to requested mode.
 *                 It's for the case that device have been initialized.
 *
 *  @Input      ::
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static int TI5150_ChangeYuvMode(UINT16 YuvMode)
{
    int i = 0;

    for (i = 0; i < TI5150_NUM_MODE_REG; i ++) {
        TI5150_RegWrite(TI5150RegTable[i].Addr, TI5150RegTable[i].Data[YuvMode]);
    }
#if 0
    for(i = 0; i < TI5150_NUM_READOUT_MODE_REG; i ++) {
        UINT8 Rval = 0;
        TI5150_RegRead(TI5150RegTable[i].Addr, &Rval);
        AmbaPrint("Reg Read: Addr: 0x%x, data: 0x%x", TI5150RegTable[i].Addr, Rval);
    }
#endif
    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: TI5150_Init
 *
 *  @Description:: Initialize Yuv device driver
 *
 *  @Input      ::
 *      Chan:   Vin ID
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static int TI5150_Init(AMBA_DSP_CHANNEL_ID_u Chan)
{
    AmbaPLL_SetSensorClk(TI5150ModeInfo[0].InputClk);       /* The default sensor input clock frequency */
    AmbaPLL_SetSensorClkDir(AMBA_PLL_SENSOR_CLK_OUTPUT);    /* Config clock output to sensor (or sensor SPI won't work) */

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: TI5150_Enable
 *
 *  @Description:: Power on Yuv Device
 *
 *  @Input      ::
 *      Chan:   Vin ID
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static int TI5150_Enable(AMBA_DSP_CHANNEL_ID_u Chan)
{
    TI5150_HardwareReset();

    /* config DVP phy*/
    AmbaDSP_VinPhySetDVP(Chan.Bits.VinID);

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: TI5150_UpdateDetectedInfo
 *
 *  @Description:: Update the information detected by Yuv device on the fly to TI5150Status
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static int TI5150_UpdateDetectedInfo(void)
{
    UINT8 RxData = 0x00;

    /* Get and update H/Vsync locked status */
    TI5150_RegRead(0x88, &RxData);
    if ((RxData & 0x6) == 0x6) {
        TI5150Status.IsHVSyncLocked = 1;
    } else {
        TI5150Status.IsHVSyncLocked = 0;
        return NG;
    }

    /* Get and update the info. of detected Video standard */
    TI5150_RegRead(0x8c, &RxData);
    switch (RxData & 0x0f) {
        case 0x01:
        case 0x09:
            TI5150Status.DetectedVideoStandard = AMBA_YUV_NTSC;
            break;
        case 0x03:
        case 0x05:
        case 0x07:
            TI5150Status.DetectedVideoStandard = AMBA_YUV_PAL;
            break;
        default:
            TI5150Status.IsHVSyncLocked = 0;
            return NG;
    }

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: TI5150_GetStatus
 *
 *  @Description:: Get current Yuv device status
 *
 *  @Input      ::
 *      Chan:   Vin ID
 *
 *  @Output     ::
 *      pStatus:    pointer to current status
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static int TI5150_GetStatus(AMBA_DSP_CHANNEL_ID_u Chan, AMBA_YUV_STATUS_INFO_s *pStatus)
{
    if (pStatus == NULL)
        return NG;

    TI5150_UpdateDetectedInfo();

    memcpy(pStatus, &TI5150Status, sizeof(AMBA_YUV_STATUS_INFO_s));

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: TI5150_GetModeInfo
 *
 *  @Description:: Get current Yuv device status
 *
 *  @Input      ::
 *      Chan:   Vin ID
 *
 *  @Output     ::
 *      pStatus:    pointer to current status
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static int TI5150_GetModeInfo(AMBA_DSP_CHANNEL_ID_u Chan, AMBA_YUV_MODE_ID_u Mode, AMBA_YUV_MODE_INFO_s *pModeInfo)
{
    UINT16 YuvMode = 0;

    if (pModeInfo == NULL)
        return NG;

    if ((Mode.Bits.ScanMethod != AMBA_YUV_INTERLACED) ||
        (Mode.Bits.InputSource != AMBA_YUV_CVBS))
        return NG;

    YuvMode = (Mode.Bits.VideoStandard == AMBA_YUV_NTSC) ? TI5150_720_480_60I : TI5150_720_576_50I;

    pModeInfo->Mode.Data = Mode.Data;
    memcpy(&pModeInfo->OutputInfo, &TI5150ModeInfo[YuvMode].OutputInfo, sizeof(AMBA_YUV_OUTPUT_INFO_s));

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: TI5150_Config
 *
 *  @Description:: Set Yuv device to indicated mode
 *
 *  @Input      ::
 *      Chan:   Vin ID
 *      Mode:   Yuv Mode
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static int TI5150_Config(AMBA_DSP_CHANNEL_ID_u Chan, AMBA_YUV_MODE_ID_u Mode)
{
    UINT16 YuvMode = 0;

    if ((Mode.Bits.ScanMethod != AMBA_YUV_INTERLACED) ||
        (Mode.Bits.InputSource != AMBA_YUV_CVBS))
        return NG;

    YuvMode = (Mode.Bits.VideoStandard == AMBA_YUV_NTSC) ? TI5150_720_480_60I : TI5150_720_576_50I;

    AmbaPrint("============  [ TI5150 ]  ============");
    AmbaPrint("============ init Yuv mode:%d ============", YuvMode);

    /* Update status */
    TI5150Status.ModeInfo.Mode.Data = Mode.Data;
    memcpy(&TI5150Status.ModeInfo.OutputInfo, &TI5150ModeInfo[YuvMode].OutputInfo, sizeof(AMBA_YUV_OUTPUT_INFO_s));
    TI5150_UpdateDetectedInfo();

    /* Make VIN not to capture bad frames during readout mode transition */
    AmbaVIN_Reset(AMBA_VIN_CHANNEL0, AMBA_VIN_DVP, NULL);

    AmbaKAL_TaskSleep(3);

    AmbaPLL_SetSensorClk(TI5150ModeInfo[YuvMode].InputClk);

    AmbaKAL_TaskSleep(3);

    TI5150_ChangeYuvMode(YuvMode);

    TI5150_ConfigVin((AMBA_YUV_OUTPUT_INFO_s *) &TI5150ModeInfo[YuvMode].OutputInfo);

    return OK;
}

/*-----------------------------------------------------------------------------------------------*/
AMBA_YUV_OBJ_s AmbaYuv_TI5150Obj = {
    .pName      = "TI5150",
    .Init       = TI5150_Init,
    .Enable     = TI5150_Enable,
    .Disable    = NULL,          /* TI5150 does not support SW standby mode. */
    .Config     = TI5150_Config,
    .GetStatus      = TI5150_GetStatus,
    .GetModeInfo    = TI5150_GetModeInfo
};
