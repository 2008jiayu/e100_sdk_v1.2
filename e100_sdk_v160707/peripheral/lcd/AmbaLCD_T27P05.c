/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaLCD_T27P05.c
 *
 *  @Copyright      :: Copyright (C) 2015 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Control APIs of T27P05 16:9
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

#include "AmbaGPIO_Def.h"
#include "AmbaDSP_VOUT.h"
#include "AmbaLCD_T27P05.h"
#include "AmbaPrintk.h"


/*-----------------------------------------------------------------------------------------------*\
 * Global instance for LCD info
\*-----------------------------------------------------------------------------------------------*/
static AMBA_LCD_T27P05_CTRL_s T27P05_Ctrl = {
    .SpiConfig = {
        .SlaveID       = 1,                                 /* Slave ID */
        .ProtocolMode  = AMBA_SPI_CPOL_HIGH_CPHA_HIGH,      /* SPI Protocol mode */
        .CsPolarity    = AMBA_SPI_CHIP_SELECT_ACTIVE_HIGH,  /* Slave select polarity */
        .DataFrameSize = 16,                                /* Data Frame Size in Bit */
        .BaudRate      = 500000                             /* Transfer BaudRate in Hz */
    },
};

/*-----------------------------------------------------------------------------------------------*\
 * T27P05 video mode info
\*-----------------------------------------------------------------------------------------------*/
static AMBA_LCD_T27P05_CONFIG_s T27P05_Config[AMBA_LCD_T27P05_NUM_MODE] = {
    [AMBA_LCD_T27P05_960_240_60HZ] = {
        .Width          = 960,
        .Height         = 240,
        .FrameRate      = {
            .Interlace  = 0,
            .TimeScale  = 60000,
            .NumUnitsInTick = 1001,
        },
        .ScreenMode     = AMBA_LCD_T27P05_SCREEN_MODE_WIDE,
        .DeviceClock    = AMBA_DSP_VOUT_PIXEL_CLOCK_FULL_DCLK,  /* clock frequency to LCD */
        .OutputMode     = AMBA_DSP_VOUT_LCD_1COLOR_PER_DOT,     /* pixel format */
        .EvenLineColor  = AMBA_DSP_VOUT_LCD_COLOR_GBR,          /* even line color order */
        .OddLineColor   = AMBA_DSP_VOUT_LCD_COLOR_RGB,          /* odd line color order */
        .VideoTiming    = {
            .PixelClock         = 27565687,
            .PixelRepetition    = 1,
            .Htotal             = 1716,
            .Vtotal             = 268,
            .HsyncColStart      = 0,
            .HsyncColEnd        = 2,
            .VsyncColStart      = 0,
            .VsyncColEnd        = 0,
            .VsyncRowStart      = 0,
            .VsyncRowEnd        = 2,
            .ActiveColStart     = 73,
            .ActiveColWidth     = 960,
            .ActiveRowStart     = 21,
            .ActiveRowHeight    = 240
        }
    },
    [AMBA_LCD_T27P05_960_240_50HZ] = {
        .Width          = 960,
        .Height         = 240,
        .FrameRate      = {
            .Interlace  = 0,
            .TimeScale  = 50,
            .NumUnitsInTick = 1,
        },
        .ScreenMode     = AMBA_LCD_T27P05_SCREEN_MODE_WIDE,
        .DeviceClock    = AMBA_DSP_VOUT_PIXEL_CLOCK_FULL_DCLK,  /* clock frequency to LCD */
        .OutputMode     = AMBA_DSP_VOUT_LCD_1COLOR_PER_DOT,     /* pixel format */
        .EvenLineColor  = AMBA_DSP_VOUT_LCD_COLOR_BGR,          /* even line color order */
        .OddLineColor   = AMBA_DSP_VOUT_LCD_COLOR_RBG,
        .VideoTiming    = {
            .PixelClock         = 27043200,
            .PixelRepetition    = 1,
            .Htotal             = 1728,
            .Vtotal             = 313,
            .HsyncColStart  = 0,
            .HsyncColEnd        = 2,
            .VsyncColStart  = 0,
            .VsyncColEnd        = 0,
            .VsyncRowStart  = 0,
            .VsyncRowEnd        = 2,
            .ActiveColStart     = 73,
            .ActiveColWidth     = 960,
            .ActiveRowStart     = 21,
            .ActiveRowHeight    = 240
        }
    }
};

/*-----------------------------------------------------------------------------------------------*\
 * T27P05 video mode configuration
\*-----------------------------------------------------------------------------------------------*/
static AMBA_DSP_VOUT_ROTATE_TYPE_e T27P05_RotationTable[2][2] = {
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
 *  @RoutineName:: T27P05_GetContrast
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
static UINT16 T27P05_GetContrast(float Gain)
{
    INT16 RegVal = 0;
#if 0
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
#endif
    return RegVal;  /* RGB gain of contrast */
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: T27P05_GetBrightness
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
static UINT16 T27P05_GetBrightness(INT32 Offset)
{
    INT16 RegVal = 0;
#if 0
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
#endif
    return RegVal;  /* Offset of brightness RGB */
}


int T27P05_SPI_Transfer_Data(const UINT16 data)
{
    int RetVal = 0;
    UINT16 SpiCmd = data;

    /* Note that each serial command must consist of 16 bits of data */
    AmbaGPIO_ConfigOutput(GPIO_PIN_50, AMBA_GPIO_LEVEL_LOW);
//    AmbaKAL_TaskSleep(3);

    RetVal = AmbaSPI_Transfer(AMBA_SPI_MASTER, &T27P05_Ctrl.SpiConfig, 1, &SpiCmd, NULL, 3000);

//    AmbaKAL_TaskSleep(3);
    AmbaGPIO_ConfigOutput(GPIO_PIN_50, AMBA_GPIO_LEVEL_HIGH);

    if (RetVal == TX_NO_EVENTS) {
        AmbaPrintColor(RED,"T27P05_SPI_Transfer timed out!");
        return NG;
    } else if (RetVal == NG) {
        AmbaPrintColor(RED,"T27P05_SPI_Transfer failed!");
        return NG;
    }

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: LCD_T27P05Enable
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
static int LCD_T27P05Enable(void)
{
    return 0;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: LCD_T27P05Enable
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
int LCD_T27P05EnableConfig(void)
{

    /*-----------------------------------------------------------------------*\
     * Configure LCD registers by SPI control interface.
    \*-----------------------------------------------------------------------*/
    AmbaPrint("LCD_T27P05EnableConfig...");

    AmbaGPIO_ConfigOutput(GPIO_PIN_50, AMBA_GPIO_LEVEL_HIGH);
    AmbaKAL_TaskSleep(10);

    //Set software reset
    T27P05_SPI_Transfer_Data(0x055f);
    AmbaKAL_TaskSleep(5);
    T27P05_SPI_Transfer_Data(0x051f);
    AmbaKAL_TaskSleep(2);
    T27P05_SPI_Transfer_Data(0x055f);
    AmbaKAL_TaskSleep(5);

    //Set EXTC
    // T27P05_SPI_Transfer_Data(0x2f71);
    T27P05_SPI_Transfer_Data(0x2b01);
    T27P05_SPI_Transfer_Data(0x0b80);
    T27P05_SPI_Transfer_Data(0x0009);
    T27P05_SPI_Transfer_Data(0x019f);
    T27P05_SPI_Transfer_Data(0x040B);
    T27P05_SPI_Transfer_Data(0x1604);

    AmbaKAL_TaskSleep(10);

    return 0;
}


/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: LCD_T27P05Disable
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
static int LCD_T27P05Disable(void)
{
    return 0;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: LCD_T27P05GetInfo
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
static int LCD_T27P05GetInfo(AMBA_LCD_INFO_s *pInfo)
{
    if (T27P05_Ctrl.pDispConfig == NULL)
        return NG;

    pInfo->Width = T27P05_Ctrl.pDispConfig->Width;
    pInfo->Height = T27P05_Ctrl.pDispConfig->Height;
    pInfo->AspectRatio.X = 16;
    pInfo->AspectRatio.Y = 9;
    memcpy(&pInfo->FrameRate, &T27P05_Ctrl.pDispConfig->FrameRate, sizeof(AMBA_DSP_FRAME_RATE_s));

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: LCD_T27P05Config
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
static int LCD_T27P05Config(AMBA_LCD_MODE_ID_u Mode)
{
    AMBA_DSP_VOUT_DISPLAY_DIGITAL_CONFIG_s *pPixelFormat = &T27P05_Ctrl.PixelFormat;
    AMBA_DSP_VOUT_DISPLAY_TIMING_CONFIG_s *pDisplayTiming = &T27P05_Ctrl.DisplayTiming;
    AMBA_LCD_T27P05_CONFIG_s *pDispConfig;

    if (Mode.Bits.Mode >= AMBA_LCD_T27P05_NUM_MODE)
        return NG;


    pDispConfig = T27P05_Ctrl.pDispConfig = &T27P05_Config[Mode.Bits.Mode];

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

    pDisplayTiming->Rotation = T27P05_RotationTable[Mode.Bits.FlipVertical][Mode.Bits.FlipHorizontal];
    pDisplayTiming->PixelClock = pDispConfig->VideoTiming.PixelClock;
    pDisplayTiming->Interlace = 0;
    pDisplayTiming->FrameWidth = pDispConfig->VideoTiming.Htotal * pDispConfig->VideoTiming.PixelRepetition;
    pDisplayTiming->FrameHeight = pDispConfig->VideoTiming.Vtotal;
    pDisplayTiming->FrameActiveColStart = pDispConfig->VideoTiming.ActiveColStart;
    pDisplayTiming->FrameActiveColWidth = pDispConfig->VideoTiming.ActiveColWidth;
    pDisplayTiming->FrameActiveRowStart = pDispConfig->VideoTiming.ActiveRowStart;
    pDisplayTiming->FrameActiveRowHeight = pDispConfig->VideoTiming.ActiveRowHeight;

    AmbaPLL_SetVoutLcdClk(pDispConfig->VideoTiming.PixelClock);
    AmbaKAL_TaskSleep(10);
    LCD_T27P05EnableConfig();
    AmbaKAL_TaskSleep(100);
    AmbaDSP_VoutDisplayTimingSetup(AMBA_DSP_VOUT_LCD, pDisplayTiming);
    AmbaDSP_VoutDisplayDigitalSetup(pPixelFormat);

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: LCD_T27P05SetBacklight
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
static int LCD_T27P05SetBacklight(INT32 EnableFlag)
{
    extern void AmbaUserGPIO_LcdCtrl(UINT32 LcdFlag);
    AmbaUserGPIO_LcdCtrl(EnableFlag);

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: LCD_T27P05SetBrightness
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
static int LCD_T27P05SetBrightness(INT32 Offset)
{
#if 0
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

    return T27P05_Write(T27P05_REG_R0AH, RegVal);    /* Offset of brightness RGB */
#else
    return OK;
#endif
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: LCD_T27P05SetContrast
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
static int LCD_T27P05SetContrast(float Gain)
{
#if 0
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

    return T27P05_Write(T27P05_REG_R0BH, RegVal);    /* RGB gain of contrast */
#else
    return OK;
#endif
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: LCD_T27P05SetColorBalance
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
static int LCD_T27P05SetColorBalance(AMBA_LCD_COLOR_BALANCE_s *pColorBalance)
{
#if 0
    UINT16 RegVal;

    if (pColorBalance == NULL)
        return NG;

    RegVal = T27P05_GetContrast(pColorBalance->GainRed);
    T27P05_Write(T27P05_REG_R0CH, RegVal);    /* R gain of contrast */
    RegVal = T27P05_GetBrightness(pColorBalance->OffsetRed);
    T27P05_Write(T27P05_REG_R0DH, RegVal);    /* R gain of brightness */

    RegVal = T27P05_GetContrast(pColorBalance->GainBlue);
    T27P05_Write(T27P05_REG_R0EH, RegVal);    /* B gain of contrast */
    RegVal = T27P05_GetBrightness(pColorBalance->OffsetBlue);
    T27P05_Write(T27P05_REG_R0FH, RegVal);    /* B gain of brightness */

    return OK;
#else
    return OK;
#endif
}

/*-----------------------------------------------------------------------------------------------*\
 * LCD driver object
\*-----------------------------------------------------------------------------------------------*/
AMBA_LCD_OBJECT_s AmbaLCD_T27P05Obj = {
    .LcdEnable = LCD_T27P05Enable,
    .LcdDisable = LCD_T27P05Disable,
    .LcdGetInfo = LCD_T27P05GetInfo,
    .LcdConfig = LCD_T27P05Config,
    .LcdSetBacklight = LCD_T27P05SetBacklight,
    .LcdSetBrightness = LCD_T27P05SetBrightness,
    .LcdSetContrast = LCD_T27P05SetContrast,
    .LcdSetColorBalance = LCD_T27P05SetColorBalance,

    .pName = "T27P05"
};
