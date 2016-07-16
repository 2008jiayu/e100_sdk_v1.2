/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaLCD_T30P61.c
 *
 *  @Copyright      :: Copyright (C) 2015 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Control APIs of Shenzhen RZW Display LCD panel T30P61
\*-------------------------------------------------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "AmbaDataType.h"
#include "AmbaKAL.h"

#include "AmbaPLL.h"
#include "AmbaSPI.h"
#include "AmbaLCD.h"

#include "AmbaDSP_VOUT.h"
#include "AmbaLCD_T30P61.h"


/*-----------------------------------------------------------------------------------------------*\
 * Global instance for LCD info
\*-----------------------------------------------------------------------------------------------*/
static AMBA_LCD_T30P61_CTRL_s T30P61_Ctrl = {
    .SpiChanNo = AMBA_SPI_MASTER,
    .SpiConfig = {
        .SlaveID       = 2,                  /* Slave ID */
        .ProtocolMode  = AMBA_SPI_CPOL_HIGH_CPHA_HIGH,      /* SPI Protocol mode */
		.CsPolarity    = AMBA_SPI_CHIP_SELECT_ACTIVE_LOW,	/* Slave select polarity */
        .DataFrameSize = 16,                                /* Data Frame Size in Bit */
        .BaudRate      = 500000,                            /* Transfer BaudRate in Hz */
    },
};

/*-----------------------------------------------------------------------------------------------*\
 * T30P61 video mode info
\*-----------------------------------------------------------------------------------------------*/
static AMBA_LCD_T30P61_CONFIG_s T30P61_Config[AMBA_LCD_T30P61_NUM_MODE] = {
    [AMBA_LCD_T30P61_960_240_60HZ] = {
        .Width          = 960,
        .Height         = 240,
        .FrameRate      = {
            .Interlace  = 0,
            .TimeScale  = 60000,
            .NumUnitsInTick = 1001,
        },
        .ScreenMode     = AMBA_LCD_T30P61_SCREEN_MODE_WIDE,
        .DeviceClock    = AMBA_DSP_VOUT_PIXEL_CLOCK_FULL_DCLK,  /* clock frequency to LCD */
        .OutputMode     = AMBA_DSP_VOUT_LCD_1COLOR_PER_DOT,     /* pixel format */
        .EvenLineColor  = AMBA_DSP_VOUT_LCD_COLOR_RGB,          /* even line color order */
        .OddLineColor   = AMBA_DSP_VOUT_LCD_COLOR_GBR,          /* odd line color order */
        .VideoTiming    = {
            .PixelClock         = 27000000,
            .PixelRepetition    = 1,
            .Htotal             = 1716,
            .Vtotal             = 262,
            .HsyncColStart      = 0,
            .HsyncColEnd        = 2,
            .VsyncColStart      = 0,
            .VsyncColEnd        = 0,
            .VsyncRowStart      = 0,
            .VsyncRowEnd        = 2,
            .ActiveColStart     = 70,
            .ActiveColWidth     = 960,
            .ActiveRowStart     = 21,
            .ActiveRowHeight    = 240
        }
    },
    [AMBA_LCD_T30P61_960_240_50HZ] = {
        .Width          = 960,
        .Height         = 240,
        .FrameRate      = {
            .Interlace  = 0,
            .TimeScale  = 50,
            .NumUnitsInTick = 1,
        },
        .ScreenMode     = AMBA_LCD_T30P61_SCREEN_MODE_WIDE,
        .DeviceClock    = AMBA_DSP_VOUT_PIXEL_CLOCK_FULL_DCLK,  /* clock frequency to LCD */
        .OutputMode     = AMBA_DSP_VOUT_LCD_1COLOR_PER_DOT,     /* pixel format */
        .EvenLineColor  = AMBA_DSP_VOUT_LCD_COLOR_RGB,          /* even line color order */
        .OddLineColor   = AMBA_DSP_VOUT_LCD_COLOR_GBR,          /* odd line color order */
        .VideoTiming    = {
            .PixelClock         = 22500000,
            .PixelRepetition    = 1,
            .Htotal             = 1716,
            .Vtotal             = 262,
            .HsyncColStart      = 0,
            .HsyncColEnd        = 2,
            .VsyncColStart      = 0,
            .VsyncColEnd        = 0,
            .VsyncRowStart      = 0,
            .VsyncRowEnd        = 2,
            .ActiveColStart     = 70,
            .ActiveColWidth     = 960,
            .ActiveRowStart     = 21,
            .ActiveRowHeight    = 240
        }
    }
};

/*-----------------------------------------------------------------------------------------------*\
 * T30P61 video mode configuration
\*-----------------------------------------------------------------------------------------------*/
static AMBA_DSP_VOUT_ROTATE_TYPE_e T30P61_RotationTable[2][2] = {
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
 *  @RoutineName:: T30P61_Write
 *
 *  @Description:: Write T30P61 register
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
static int T30P61_Write(UINT16 Offset, UINT16 Data)
{
    UINT16 SpiCmd = (Offset << 8) | (Data & 0xFF);

    return AmbaSPI_Transfer(T30P61_Ctrl.SpiChanNo, &T30P61_Ctrl.SpiConfig, 1, &SpiCmd, NULL, 500);
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: T30P61_GetContrast
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
static UINT16 T30P61_GetContrast(float Gain)
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
 *  @RoutineName:: T30P61_GetBrightness
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
static UINT16 T30P61_GetBrightness(INT32 Offset)
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
 *  @RoutineName:: LCD_T30P61Enable
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
static int LCD_T30P61Enable(void)
{
    /*-----------------------------------------------------------------------*\
     * Configure LCD registers by SPI control interface.
    \*-----------------------------------------------------------------------*/
    T30P61_Write(0x05, 0x1C);       /* Reset all registers */
    T30P61_Write(0x05, 0x5C);       /* Normal operation */

    T30P61_Write(0x2B, 0x01);       /* Set to normal mode */
    T30P61_Write(0x03, 0x20);       /* Set brightness to 0x20 */

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: LCD_T30P61Disable
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
static int LCD_T30P61Disable(void)
{
    return T30P61_Write(0x2B, (UINT16) 0x00); /* Enter standby mode */
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: LCD_T30P61GetInfo
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
static int LCD_T30P61GetInfo(AMBA_LCD_INFO_s *pInfo)
{
    if (T30P61_Ctrl.pDispConfig == NULL)
        return NG;

    pInfo->Width = T30P61_Ctrl.pDispConfig->Width;
    pInfo->Height = T30P61_Ctrl.pDispConfig->Height;
    pInfo->AspectRatio.X = 16;
    pInfo->AspectRatio.Y = 9;
    memcpy(&pInfo->FrameRate, &T30P61_Ctrl.pDispConfig->FrameRate, sizeof(AMBA_DSP_FRAME_RATE_s));

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: LCD_T30P61Config
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
static int LCD_T30P61Config(AMBA_LCD_MODE_ID_u Mode)
{
    AMBA_DSP_VOUT_DISPLAY_DIGITAL_CONFIG_s *pPixelFormat = &T30P61_Ctrl.PixelFormat;
    AMBA_DSP_VOUT_DISPLAY_TIMING_CONFIG_s *pDisplayTiming = &T30P61_Ctrl.DisplayTiming;
    AMBA_LCD_T30P61_CONFIG_s *pDispConfig;

    if (Mode.Bits.Mode >= AMBA_LCD_T30P61_NUM_MODE)
        return NG;

    pDispConfig = T30P61_Ctrl.pDispConfig = &T30P61_Config[Mode.Bits.Mode];

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

    if (Mode.Bits.FlipHorizontal) {
        pPixelFormat->EvenLineColor   = (AMBA_DSP_VOUT_LCD_COLOR_ORDER_e)(pDispConfig->OddLineColor ^ 0x1);
        pPixelFormat->OddLineColor    = (AMBA_DSP_VOUT_LCD_COLOR_ORDER_e)(pDispConfig->EvenLineColor ^ 0x1);
    } else {
        pPixelFormat->EvenLineColor   = pDispConfig->EvenLineColor;
        pPixelFormat->OddLineColor    = pDispConfig->OddLineColor;
    }

    pDisplayTiming->Rotation = T30P61_RotationTable[Mode.Bits.FlipVertical][Mode.Bits.FlipHorizontal];
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
 *  @RoutineName:: LCD_T30P61SetBacklight
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
static int LCD_T30P61SetBacklight(INT32 EnableFlag)
{
    extern void AmbaUserGPIO_LcdCtrl(UINT32 LcdFlag);
    AmbaUserGPIO_LcdCtrl(EnableFlag);

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: LCD_T30P61SetBrightness
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
static int LCD_T30P61SetBrightness(INT32 Offset)
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

    return T30P61_Write(0x3, RegVal);    /* Offset of brightness RGB */
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: LCD_T30P61SetContrast
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
static int LCD_T30P61SetContrast(float Gain)
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

    return T30P61_Write(0x0D, RegVal);    /* RGB gain of contrast */
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: LCD_T30P61SetColorBalance
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
static int LCD_T30P61SetColorBalance(AMBA_LCD_COLOR_BALANCE_s *pColorBalance)
{
    UINT16 RegVal;

    if (pColorBalance == NULL)
        return NG;

    RegVal = T30P61_GetContrast(pColorBalance->GainRed);
    T30P61_Write(0x0E, RegVal);    /* R gain of contrast */
    RegVal = T30P61_GetBrightness(pColorBalance->OffsetRed);
    T30P61_Write(0x0F, RegVal);    /* R gain of brightness */

    RegVal = T30P61_GetContrast(pColorBalance->GainBlue);
    T30P61_Write(0x10, RegVal);    /* B gain of contrast */
    RegVal = T30P61_GetBrightness(pColorBalance->OffsetBlue);
    T30P61_Write(0x11, RegVal);    /* B gain of brightness */

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 * LCD driver object
\*-----------------------------------------------------------------------------------------------*/
AMBA_LCD_OBJECT_s AmbaLCD_T30P61Obj = {
    .LcdEnable = LCD_T30P61Enable,
    .LcdDisable = LCD_T30P61Disable,
    .LcdGetInfo = LCD_T30P61GetInfo,
    .LcdConfig = LCD_T30P61Config,
    .LcdSetBacklight = LCD_T30P61SetBacklight,
    .LcdSetBrightness = LCD_T30P61SetBrightness,
    .LcdSetContrast = LCD_T30P61SetContrast,
    .LcdSetColorBalance = LCD_T30P61SetColorBalance,

    .pName = "T30P61"
};
