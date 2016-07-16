/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaLCD_WDF9648W.c
 *
 *  @Copyright      :: Copyright (C) 2013 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Control APIs of Wintek 4:3 LCD panel WD-F9648W
\*-------------------------------------------------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "AmbaDataType.h"
#include "AmbaKAL.h"
#include "AmbaPrint.h"

#include "AmbaPLL.h"
#include "AmbaSPI.h"
#include "AmbaLCD.h"

#include "AmbaDSP_VOUT.h"
#include "AmbaLCD_WDF9648W.h"

/*-----------------------------------------------------------------------------------------------*\
 * Global instance for LCD info
\*-----------------------------------------------------------------------------------------------*/
static AMBA_LCD_WDF9648W_CTRL_s WdF9648w_Ctrl = {
    .SpiConfig = {
        .SlaveID       = 2,                                 /* Slave ID */
        .ProtocolMode  = AMBA_SPI_CPOL_HIGH_CPHA_HIGH,      /* SPI Protocol mode */
        .CsPolarity    = AMBA_SPI_CHIP_SELECT_ACTIVE_LOW,   /* Slave select polarity */
        .DataFrameSize = 16,                                /* Data Frame Size in Bit */
        .BaudRate      = 500000                             /* Transfer BaudRate in Hz */
    },
};

/*-----------------------------------------------------------------------------------------------*\
 * WDF9648W video mode info
\*-----------------------------------------------------------------------------------------------*/
static AMBA_LCD_WDF9648W_CONFIG_s WdF9648w_Config[AMBA_LCD_WDF9648W_NUM_MODE] = {
    [AMBA_LCD_WDF9648W_960_480_60HZ] = {
        .Width          = 960,
        .Height         = 480,
        .FrameRate      = {
            .Interlace  = 0,
            .TimeScale  = 60000,
            .NumUnitsInTick = 1001,
        },
        .ScreenMode     = AMBA_LCD_WDF9648W_SCREEN_MODE_WIDE,
        .DeviceClock    = AMBA_DSP_VOUT_PIXEL_CLOCK_FULL_DCLK,  /* clock frequency to LCD */
        .OutputMode     = AMBA_DSP_VOUT_LCD_1COLOR_PER_DOT,     /* pixel format */
        .EvenLineColor  = AMBA_DSP_VOUT_LCD_COLOR_RGB,          /* even line color order */
        .OddLineColor   = AMBA_DSP_VOUT_LCD_COLOR_RGB,          /* odd line color order */
        .VideoTiming    = {
            .PixelClock         = 36000000,
            .PixelRepetition    = 1,
            .Htotal             = 1144,
            .Vtotal             = 525,
            .HsyncColStart      = 0,
            .HsyncColEnd        = 1,
            .VsyncColStart      = 0,
            .VsyncColEnd        = 0,
            .VsyncRowStart      = 0,
            .VsyncRowEnd        = 1,
            .ActiveColStart     = 32,
            .ActiveColWidth     = 960,
            .ActiveRowStart     = 42,
            .ActiveRowHeight    = 480
        }
    },
    [AMBA_LCD_WDF9648W_960_480_50HZ] = {
        .Width          = 960,
        .Height         = 480,
        .FrameRate      = {
            .Interlace  = 0,
            .TimeScale  = 50,
            .NumUnitsInTick = 1,
        },
        .ScreenMode     = AMBA_LCD_WDF9648W_SCREEN_MODE_WIDE,
        .DeviceClock    = AMBA_DSP_VOUT_PIXEL_CLOCK_FULL_DCLK,  /* clock frequency to LCD */
        .OutputMode     = AMBA_DSP_VOUT_LCD_1COLOR_PER_DOT,     /* pixel format */
        .EvenLineColor  = AMBA_DSP_VOUT_LCD_COLOR_RGB,          /* even line color order */
        .OddLineColor   = AMBA_DSP_VOUT_LCD_COLOR_RGB,          /* odd line color order */
        .VideoTiming    = {
            .PixelClock         = 30030000,
            .PixelRepetition    = 1,
            .Htotal             = 1144,
            .Vtotal             = 525,
            .HsyncColStart      = 0,
            .HsyncColEnd        = 1,
            .VsyncColStart      = 0,
            .VsyncColEnd        = 0,
            .VsyncRowStart      = 0,
            .VsyncRowEnd        = 1,
            .ActiveColStart     = 32,
            .ActiveColWidth     = 960,
            .ActiveRowStart     = 42,
            .ActiveRowHeight    = 480
        }
    }
};

/*-----------------------------------------------------------------------------------------------*\
 * WDF9648W video mode configuration
\*-----------------------------------------------------------------------------------------------*/
static UINT16 WdF9648w_DefaultCtrl[] = {
    0x2280, 0x0C1D, 0x0E27, 0x280E, 0x2621, 0x2A03, 0x2C05, 0x2E29, 0x3023, 0x3200
};

static UINT16 WdF9648w_DefaultGamma[] = {
    0x4219, 0x441D, 0x460C, 0x4805, 0x4A03, 0x4C03, 0x4E03, 0x5005, 0x5207, 0x5403, 0x5602, 0x580F,
    0x5A1C, 0x5C1D, 0x5E0E, 0x6005, 0x6205, 0x6404, 0x6604, 0x6805, 0x6A07, 0x6C05, 0x6E02, 0x7000
};

static AMBA_DSP_VOUT_ROTATE_TYPE_e WdF9648w_RotationTable[2][2] = {
    [0] = {
        [0] = AMBA_DSP_VOUT_ROTATE_NORMAL,
        [1] = AMBA_DSP_VOUT_ROTATE_MIRROR_HORIZONTAL
    },
    [1] = {
        [0] = AMBA_DSP_VOUT_ROTATE_FLIP_VERTICAL,
        [1] = AMBA_DSP_VOUT_ROTATE_180_DEGREE
    },
};

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: WdF9648w_Write
 *
 *  @Description:: Write WD-F9648W register
 *
 *  @Input      ::
 *      Offset:     Register offset
 *      Data:       Register data
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static int WdF9648w_Write(UINT16 Offset, UINT16 Data)
{
    UINT16 SpiCmd = (Offset << 9) | (Data & 0xFF);

    return AmbaSPI_Transfer(AMBA_SPI_MASTER, &WdF9648w_Ctrl.SpiConfig, 1, &SpiCmd, NULL, 500);
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: WdF9648w_SPI_Transfer
 *
 *  @Description:: Transfer WD-F9648W commands via SPI
 *
 *  @Input      ::
 *      NumSpiCmd:    number of SPI commands
 *      pSpiCmdBuf:   pointer to SPI command buffer
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static int WdF9648w_SPI_Transfer(int NumSpiCmd, const UINT16 *pSpiCmdBuf)
{
    int i;
    int RetVal = OK;
    UINT16 SpiCmd;

    /* Note that each serial command must consist of 16 bits of data */
    for (i = 0; i < NumSpiCmd; i++) {
        SpiCmd = pSpiCmdBuf[i];

        RetVal = AmbaSPI_Transfer(AMBA_SPI_MASTER, &WdF9648w_Ctrl.SpiConfig, 1, &SpiCmd, NULL, 500);

        if (RetVal == TX_NO_EVENTS) {
            AmbaPrint("WdF9648w_SPI_Transfer timed out!");
            return NG;
        } else if (RetVal == NG) {
            AmbaPrint("WdF9648w_SPI_Transfer failed!");
            return NG;
        }
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: WdF9648w_SetInputControl
 *
 *  @Description:: Select DE/Sync(HV) mode
 *
 *  @Input      ::
 *      InputCtrl:  Data input mode
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static int WdF9648w_SetInputControl(AMBA_LCD_WDF9648W_CTRL_TYPE_e InputCtrl)
{
    AMBA_DSP_VOUT_DISPLAY_DIGITAL_CONFIG_s *pPixelFormat = &WdF9648w_Ctrl.PixelFormat;
    UINT16 RegVal = 0x00;

    if (WdF9648w_Ctrl.pDispConfig->ScreenMode == AMBA_LCD_WDF9648W_SCREEN_MODE_NARROW)
        RegVal |= 0x80;

    if (InputCtrl == AMBA_LCD_WDF9648W_CTRL_DE) {
        RegVal |= 0x10;
        if (pPixelFormat->LineValidPolarity == AMBA_DSP_VOUT_ACTIVE_LOW)
            RegVal |= 0x08;
    } else if (InputCtrl == AMBA_LCD_WDF9648W_CTRL_HV_SYNC) {
        if (pPixelFormat->FrameSyncPolarity == AMBA_DSP_VOUT_ACTIVE_LOW)
            RegVal |= 0x04;
        if (pPixelFormat->LineSyncPolarity == AMBA_DSP_VOUT_ACTIVE_LOW)
            RegVal |= 0x02;
    }

    if (pPixelFormat->ClkSampleEdge == AMBA_DSP_VOUT_ACTIVE_LOW)
        RegVal |= 0x01;

    return WdF9648w_Write(WDF9648W_REG_R02H, RegVal);
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: WdF9648w_SetInputFormat
 *
 *  @Description:: Set input format
 *
 *  @Input      ::
 *      InputFormat:    Input format selection
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static int WdF9648w_SetInputFormat(AMBA_LCD_WDF9648W_INPUT_FORMAT_e InputFormat)
{
    UINT16 RegVal = 0x03;

    RegVal |= ((UINT16) InputFormat << 4);

    return WdF9648w_Write(WDF9648W_REG_R03H, RegVal);
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: WdF9648w_GetContrast
 *
 *  @Description:: Adjust LCD panel contrast
 *
 *  @Input      ::
 *      Gain:   D_contrast = D_in * Gain, 0x40 = 1x gain, (1/256)x per step
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          UINT16: Register value
\*-----------------------------------------------------------------------------------------------*/
static UINT16 WdF9648w_GetContrast(float Gain)
{
    INT16 RegVal;

    /*-----------------------------------------------------------------------*\
     * 00h: 0.75x
     * 40h: 1.0x
     * 7Fh: 1.24609375 = (1 + 63/256)x
    \*-----------------------------------------------------------------------*/
    if (Gain < 0.751953125)
        RegVal = 0x00;
    else if (Gain >= 1.244140625)
        RegVal = 0x7F;
    else
        RegVal = (INT16) (((Gain - 1) + 0.001953125) / 0.00390625) + 0x40;

    return RegVal;  /* RGB gain of contrast */
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: WdF9648w_GetBrightness
 *
 *  @Description:: Adjust LCD panel brightness (black level)
 *
 *  @Input      ::
 *      Brightness: D_brightness = D_contrast + Offset, 0x40 = zero offset
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          UINT16: Register value
\*-----------------------------------------------------------------------------------------------*/
static UINT16 WdF9648w_GetBrightness(INT32 Offset)
{
    INT16 RegVal;

    /*-----------------------------------------------------------------------*\
     * 00h: Dark    (-64)
     * 40h: Default (0)
     * 7Fh: Bright  (+63)
    \*-----------------------------------------------------------------------*/
    if (Offset <= -64)
        RegVal = 0x00;
    else if (Offset >= 63)
        RegVal = 0x7F;
    else
        RegVal = (INT16) (0x40 + Offset);

    return RegVal;  /* Offset of brightness RGB */
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: LCD_Wdf9648wEnable
 *
 *  @Description:: Enable LCD panel device
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static int LCD_Wdf9648wEnable(void)
{
    UINT16 *pSpiCmdBuf;
    int NumSpiCmd, RetVal1, RetVal2;

    /*-----------------------------------------------------------------------*\
     * Configure LCD registers by SPI control interface.
    \*-----------------------------------------------------------------------*/
    WdF9648w_Write(WDF9648W_REG_R05H, 0);       /* GRB=0 : Global Reset */

    WdF9648w_SetInputFormat(AMBA_LCD_WDF9648W_8BIT_RGB_DA_MODE);

    pSpiCmdBuf = WdF9648w_DefaultCtrl;
    NumSpiCmd = sizeof(WdF9648w_DefaultCtrl) / sizeof(UINT16);
    RetVal1 = WdF9648w_SPI_Transfer(NumSpiCmd, pSpiCmdBuf);

    pSpiCmdBuf = WdF9648w_DefaultGamma;
    NumSpiCmd = sizeof(WdF9648w_DefaultGamma) / sizeof(UINT16);
    RetVal2 = WdF9648w_SPI_Transfer(NumSpiCmd, pSpiCmdBuf);

    WdF9648w_SetInputControl(AMBA_LCD_WDF9648W_CTRL_DE);

    WdF9648w_Write(WDF9648W_REG_R01H, (UINT16) 0x01);        /* Operating mode */

    if (RetVal1 == NG || RetVal2 == NG)
        return NG;

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: LCD_Wdf9648wDisable
 *
 *  @Description:: Disable LCD panel device
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static int LCD_Wdf9648wDisable(void)
{
    return WdF9648w_Write(WDF9648W_REG_R01H, (UINT16) 0x00); /* Standby mode */
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: LCD_Wdf9648wGetInfo
 *
 *  @Description:: Get vout configuration for current LCD display mode
 *
 *  @Input      :: none
 *
 *  @Output     ::
 *      pInfo:      pointer to LCD display mode info
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static int LCD_Wdf9648wGetInfo(AMBA_LCD_INFO_s *pInfo)
{
    if (WdF9648w_Ctrl.pDispConfig == NULL)
        return NG;

    pInfo->Width = WdF9648w_Ctrl.pDispConfig->Width;
    pInfo->Height = WdF9648w_Ctrl.pDispConfig->Height;
    pInfo->AspectRatio.X = 4;
    pInfo->AspectRatio.Y = 3;
    memcpy(&pInfo->FrameRate, &WdF9648w_Ctrl.pDispConfig->FrameRate, sizeof(AMBA_DSP_FRAME_RATE_s));

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: LCD_Wdf9648wConfig
 *
 *  @Description:: Configure LCD display mode
 *
 *  @Input      ::
 *      pLcdConfig: configuration of LCD display mode
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static int LCD_Wdf9648wConfig(AMBA_LCD_MODE_ID_u Mode)
{
    AMBA_DSP_VOUT_DISPLAY_DIGITAL_CONFIG_s *pPixelFormat = &WdF9648w_Ctrl.PixelFormat;
    AMBA_DSP_VOUT_DISPLAY_TIMING_CONFIG_s *pDisplayTiming = &WdF9648w_Ctrl.DisplayTiming;
    AMBA_LCD_WDF9648W_CONFIG_s *pDispConfig;

    if (Mode.Bits.Mode >= AMBA_LCD_WDF9648W_NUM_MODE)
        return NG;

    pDispConfig = WdF9648w_Ctrl.pDispConfig = &WdF9648w_Config[Mode.Bits.Mode];

    pPixelFormat->DeviceClock = pDispConfig->DeviceClock;
    pPixelFormat->OutputMode = pDispConfig->OutputMode;
    pPixelFormat->ClkSampleEdge = AMBA_DSP_VOUT_ACTIVE_HIGH;
    pPixelFormat->FrameValidPolarity = AMBA_DSP_VOUT_ACTIVE_HIGH;
    pPixelFormat->LineValidPolarity = AMBA_DSP_VOUT_ACTIVE_HIGH;
    pPixelFormat->FrameSyncPolarity = AMBA_DSP_VOUT_ACTIVE_LOW;
    pPixelFormat->LineSyncPolarity = AMBA_DSP_VOUT_ACTIVE_LOW;

    pPixelFormat->SyncCtrl.HSyncColStart = pDispConfig->VideoTiming.HsyncColStart;
    pPixelFormat->SyncCtrl.HSyncColEnd = pDispConfig->VideoTiming.HsyncColEnd;
    pPixelFormat->SyncCtrl.VSyncColStart = pDispConfig->VideoTiming.VsyncColStart;
    pPixelFormat->SyncCtrl.VSyncColEnd = pDispConfig->VideoTiming.VsyncColEnd;
    pPixelFormat->SyncCtrl.VSyncRowStart = pDispConfig->VideoTiming.VsyncRowStart;
    pPixelFormat->SyncCtrl.VSyncRowEnd = pDispConfig->VideoTiming.VsyncRowEnd;
    pPixelFormat->Interlace = 0;
    pPixelFormat->RowTime = pDispConfig->VideoTiming.Htotal;

    if (pDispConfig->OutputMode == AMBA_DSP_VOUT_LCD_3COLORS_DUMMY_PER_DOT) {
        pPixelFormat->EvenLineColor   = pDispConfig->EvenLineColor;
        pPixelFormat->OddLineColor    = pDispConfig->OddLineColor;
    } else if (Mode.Bits.FlipHorizontal) {
        pPixelFormat->EvenLineColor   = (AMBA_DSP_VOUT_LCD_COLOR_ORDER_e)(pDispConfig->OddLineColor ^ 0x1);
        pPixelFormat->OddLineColor    = (AMBA_DSP_VOUT_LCD_COLOR_ORDER_e)(pDispConfig->EvenLineColor ^ 0x1);
    } else {
        pPixelFormat->EvenLineColor   = pDispConfig->OddLineColor;
        pPixelFormat->OddLineColor    = pDispConfig->EvenLineColor;
    }

    pDisplayTiming->Rotation = WdF9648w_RotationTable[Mode.Bits.FlipVertical][Mode.Bits.FlipHorizontal];
    pDisplayTiming->PixelClock = pDispConfig->VideoTiming.PixelClock;
    pDisplayTiming->Interlace = 0;
    pDisplayTiming->FrameWidth = pDispConfig->VideoTiming.Htotal * pDispConfig->VideoTiming.PixelRepetition;
    pDisplayTiming->FrameHeight = pDispConfig->VideoTiming.Vtotal;
    pDisplayTiming->FrameActiveColStart = pDispConfig->VideoTiming.ActiveColStart;
    pDisplayTiming->FrameActiveColWidth = pDispConfig->VideoTiming.ActiveColWidth;
    pDisplayTiming->FrameActiveRowStart = pDispConfig->VideoTiming.ActiveRowStart;
    pDisplayTiming->FrameActiveRowHeight = pDispConfig->VideoTiming.ActiveRowHeight;

    AmbaPLL_SetVoutLcdClk(pDispConfig->VideoTiming.PixelClock);
    AmbaDSP_VoutDisplayTimingSetup(AMBA_DSP_VOUT_LCD, pDisplayTiming);
    AmbaDSP_VoutDisplayDigitalSetup(pPixelFormat);

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: LCD_Wdf9648wSetBacklight
 *
 *  @Description:: Turn LCD Backlight On/Off
 *
 *  @Input      ::
 *      EnableFlag: 1 = Backlight On, 0 = Backlight Off
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static int LCD_Wdf9648wSetBacklight(INT32 EnableFlag)
{
  extern void AmbaUserGPIO_LcdCtrl(UINT32 LcdFlag);
    AmbaUserGPIO_LcdCtrl(EnableFlag);

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: LCD_Wdf9648wSetBrightness
 *
 *  @Description:: Adjust LCD panel brightness (black level)
 *
 *  @Input      ::
 *      Offset: D_brightness = D_contrast + Offset, 0x40 = zero offset
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static int LCD_Wdf9648wSetBrightness(INT32 Offset)
{
    UINT16 RegVal;

    /*-----------------------------------------------------------------------*\
     * 00h: Dark    (-64)
     * 40h: Default (0)
     * 80h: Bright  (+64)
     * C0h: Bright  (+128)
     * FFh: Bright  (+191)
    \*-----------------------------------------------------------------------*/
    if (Offset <= -64)
        RegVal = 0x00;
    else if (Offset >= 191)
        RegVal = 0xFF;
    else
        RegVal = (UINT16) (0x40 + Offset);

    return WdF9648w_Write(WDF9648W_REG_R0AH, RegVal);    /* Offset of brightness RGB */
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: LCD_Wdf9648wSetContrast
 *
 *  @Description:: Adjust LCD panel contrast
 *
 *  @Input      ::
 *      Gain:   D_contrast = D_in * Gain, 0x40 = 1x gain, (1/64)x per step
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static int LCD_Wdf9648wSetContrast(float Gain)
{
    UINT16 RegVal;

    /*-----------------------------------------------------------------------*\
     * 00h: 0.0x
     * 40h: 1.0x
     * 80h: 2.0x
     * C0h: 3.0x
     * FFh: 3.984375x = (255/64)x
    \*-----------------------------------------------------------------------*/
    if (Gain < 0.0078125)
        RegVal = 0x00;
    else if (Gain > 3.9765625)
        RegVal = 0xFF;
    else
        RegVal = (UINT16) ((Gain + 0.0078125) / 0.015625);

    return WdF9648w_Write(WDF9648W_REG_R0BH, RegVal);    /* RGB gain of contrast */
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: LCD_Wdf9648wSetColorBalance
 *
 *  @Description:: Adjust LCD color balance
 *
 *  @Input      ::
 *      ColorBalance:   Gains and Offsets of each color channel
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static int LCD_Wdf9648wSetColorBalance(AMBA_LCD_COLOR_BALANCE_s *pColorBalance)
{
    UINT16 RegVal;

    if (pColorBalance == NULL)
        return NG;

    RegVal = WdF9648w_GetContrast(pColorBalance->GainRed);
    WdF9648w_Write(WDF9648W_REG_R0CH, RegVal);    /* R gain of contrast */
    RegVal = WdF9648w_GetBrightness(pColorBalance->OffsetRed);
    WdF9648w_Write(WDF9648W_REG_R0DH, RegVal);    /* R gain of brightness */

    RegVal = WdF9648w_GetContrast(pColorBalance->GainBlue);
    WdF9648w_Write(WDF9648W_REG_R0EH, RegVal);    /* B gain of contrast */
    RegVal = WdF9648w_GetBrightness(pColorBalance->OffsetBlue);
    WdF9648w_Write(WDF9648W_REG_R0FH, RegVal);    /* B gain of brightness */

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 * LCD driver object
\*-----------------------------------------------------------------------------------------------*/
AMBA_LCD_OBJECT_s AmbaLCD_WdF9648wObj = {
    .LcdEnable = LCD_Wdf9648wEnable,
    .LcdDisable = LCD_Wdf9648wDisable,
    .LcdGetInfo = LCD_Wdf9648wGetInfo,
    .LcdConfig = LCD_Wdf9648wConfig,
    .LcdSetBacklight = LCD_Wdf9648wSetBacklight,
    .LcdSetBrightness = LCD_Wdf9648wSetBrightness,
    .LcdSetContrast = LCD_Wdf9648wSetContrast,
    .LcdSetColorBalance = LCD_Wdf9648wSetColorBalance,

    .pName = "WDF9648W"
};
