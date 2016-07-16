/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaLCD_T20P52.c
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

#include "AmbaGPIO_Def.h"
#include "AmbaDSP_VOUT.h"
#include "AmbaLCD_T20P52.h"
#include "AmbaPrintk.h"

#if defined(CONFIG_BSP_LIBRA) 
#define LCD_REVERT 1
#endif
/*-----------------------------------------------------------------------------------------------*\
 * Global instance for LCD info
\*-----------------------------------------------------------------------------------------------*/
static AMBA_LCD_T20P52_CTRL_s T20P52_Ctrl = {
    .SpiConfig = {
        .SlaveID       = 1,                                 /* Slave ID */
        .ProtocolMode  = AMBA_SPI_CPOL_LOW_CPHA_LOW,      /* SPI Protocol mode */
        .CsPolarity    = AMBA_SPI_CHIP_SELECT_ACTIVE_LOW,   /* Slave select polarity */
        .DataFrameSize = 9,                                /* Data Frame Size in Bit */
        .BaudRate      = 500000                             /* Transfer BaudRate in Hz */
    },
};

/*-----------------------------------------------------------------------------------------------*\
 * T20P52 video mode info
\*-----------------------------------------------------------------------------------------------*/
static AMBA_LCD_T20P52_CONFIG_s T20P52_Config[AMBA_LCD_T20P52_NUM_MODE] = {
    [AMBA_LCD_T20P52_960_240_60HZ] = {
        .Width          = 960,
        .Height         = 240,
        .FrameRate      = {
            .Interlace  = 0,
            .TimeScale  = 60000,
            .NumUnitsInTick = 1001,
        },
        .ScreenMode     = AMBA_LCD_T20P52_SCREEN_MODE_WIDE,
        .DeviceClock    = AMBA_DSP_VOUT_PIXEL_CLOCK_FULL_DCLK,  /* clock frequency to LCD */
        .OutputMode     = AMBA_DSP_VOUT_LCD_1COLOR_PER_DOT,     /* pixel format */
        .EvenLineColor  = AMBA_DSP_VOUT_LCD_COLOR_RBG,          /* even line color order */
        .OddLineColor   = AMBA_DSP_VOUT_LCD_COLOR_RBG,          /* odd line color order */
        .VideoTiming    = {
            .PixelClock         = 17986993,
            .PixelRepetition    = 1,
            .Htotal             = 1141,
            .Vtotal             = 263,
            .HsyncColStart      = 0,
            .HsyncColEnd        = 2,
            .VsyncColStart      = 0,
            .VsyncColEnd        = 0,
            .VsyncRowStart      = 0,
            .VsyncRowEnd        = 5,
            .ActiveColStart     = 98,
            .ActiveColWidth     = 960,
            .ActiveRowStart     = 21,
            .ActiveRowHeight    = 240
        }
    },
    [AMBA_LCD_T20P52_960_240_50HZ] = {
        .Width          = 960,
        .Height         = 240,
        .FrameRate      = {
            .Interlace  = 0,
            .TimeScale  = 50,
            .NumUnitsInTick = 1,
        },
        .ScreenMode     = AMBA_LCD_T20P52_SCREEN_MODE_WIDE,
        .DeviceClock    = AMBA_DSP_VOUT_PIXEL_CLOCK_FULL_DCLK,  /* clock frequency to LCD */
        .OutputMode     = AMBA_DSP_VOUT_LCD_1COLOR_PER_DOT,     /* pixel format */
        .EvenLineColor  = AMBA_DSP_VOUT_LCD_COLOR_RGB,          /* even line color order */
        .OddLineColor   = AMBA_DSP_VOUT_LCD_COLOR_RGB,          /* odd line color order */
        .VideoTiming    = {
            .PixelClock         = 15004150,
            .PixelRepetition    = 1,
            .Htotal             = 1141,
            .Vtotal             = 263,
            .HsyncColStart      = 0,
            .HsyncColEnd        = 2,
            .VsyncColStart      = 0,
            .VsyncColEnd        = 0,
            .VsyncRowStart      = 0,
            .VsyncRowEnd        = 5,
            .ActiveColStart     = 98,
            .ActiveColWidth     = 960,
            .ActiveRowStart     = 21,
            .ActiveRowHeight    = 240
        }
    }
};

/*-----------------------------------------------------------------------------------------------*\
 * T20P52 video mode configuration
\*-----------------------------------------------------------------------------------------------*/
static AMBA_DSP_VOUT_ROTATE_TYPE_e T20P52_RotationTable[2][2] = {
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
 *  @RoutineName:: T20P52_Write
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
static int T20P52_Write(UINT16 Offset, UINT16 Data)
{
    UINT16 SpiCmd = (Offset << 9) | (Data & 0xFF);

    return AmbaSPI_Transfer(AMBA_SPI_MASTER, &T20P52_Ctrl.SpiConfig, 1, &SpiCmd, NULL, 500);
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: T20P52_SPI_Transfer
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
static int T20P52_SPI_Transfer(int NumSpiCmd, void *pSpiCmdBuf)
{
    int RetVal = 0;

    /* Note that each serial command must consist of 16 bits of data */
    RetVal = AmbaSPI_Transfer(AMBA_SPI_MASTER, &T20P52_Ctrl.SpiConfig, NumSpiCmd, pSpiCmdBuf, NULL, 3000);

    if (RetVal == TX_NO_EVENTS) {
        AmbaPrintColor(RED,"T20P52_SPI_Transfer timed out!");
        return NG;
    } else if (RetVal == NG) {
        AmbaPrintColor(RED,"T20P52_SPI_Transfer failed!");
        return NG;
    }

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: T20P52_GetContrast
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
static UINT16 T20P52_GetContrast(float Gain)
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
 *  @RoutineName:: T20P52_GetBrightness
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
static UINT16 T20P52_GetBrightness(INT32 Offset)
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


static int T20P52_SPI_Transfer_Data(const UINT16 data)
{
    int RetVal = 0;
    UINT16 SpiCmd = data;

    /* Note that each serial command must consist of 16 bits of data */

    RetVal = AmbaSPI_Transfer(AMBA_SPI_MASTER, &T20P52_Ctrl.SpiConfig, 1, &SpiCmd, NULL, 3000);

    if (RetVal == TX_NO_EVENTS) {
        AmbaPrintColor(RED,"T20P52_SPI_Transfer timed out!");
        return NG;
    } else if (RetVal == NG) {
        AmbaPrintColor(RED,"T20P52_SPI_Transfer failed!");
        return NG;
    }

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: LCD_T20P52Enable
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
static int LCD_T20P52Enable(void)
{
    return 0;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: LCD_T20P52Enable
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
int LCD_T20P52EnableConfig(void)
{
    int RetVal1 = 0, RetVal2 = 0;

    /*-----------------------------------------------------------------------*\
     * Configure LCD registers by SPI control interface.
    \*-----------------------------------------------------------------------*/

    AmbaGPIO_ConfigOutput(GPIO_PIN_31, AMBA_GPIO_LEVEL_HIGH);
    AmbaKAL_TaskSleep(20);
    AmbaGPIO_ConfigOutput(GPIO_PIN_31, AMBA_GPIO_LEVEL_LOW);
    AmbaKAL_TaskSleep(50);
    AmbaGPIO_ConfigOutput(GPIO_PIN_31, AMBA_GPIO_LEVEL_HIGH);

    //Set software reset
    T20P52_SPI_Transfer_Data(0x0001);
    AmbaKAL_TaskSleep(50);



    //AmbaPrintColor(GREEN,"[LCD T20P52] Set EXTC");
    //Set EXTC
    T20P52_SPI_Transfer_Data(0x00C8);
    T20P52_SPI_Transfer_Data(0x01FF);
    T20P52_SPI_Transfer_Data(0x0193);
    T20P52_SPI_Transfer_Data(0x0142);

    //Set power control 1
    //AmbaPrintColor(GREEN,"[LCD T20P52] Set power control 1");
    T20P52_SPI_Transfer_Data(0x00C0);
    T20P52_SPI_Transfer_Data(0x010F);//0F
    T20P52_SPI_Transfer_Data(0x010F);//0F

    //AmbaPrintColor(GREEN,"[LCD T20P52] Set power control 2");
    //Set power control 2
    T20P52_SPI_Transfer_Data(0x00C1);
    T20P52_SPI_Transfer_Data(0x0101);//01


    //AmbaPrintColor(GREEN,"[LCD T20P52] VCOM Control 1");
    //VCOM Control 1
    T20P52_SPI_Transfer_Data(0x00C5);
    T20P52_SPI_Transfer_Data(0x01DB);//DB

    //AmbaPrintColor(GREEN,"[LCD T20P52] Blanking Porch Control");
    //Blanking Porch Control
    T20P52_SPI_Transfer_Data(0x00B5);
    T20P52_SPI_Transfer_Data(0x0102);//VFP
    T20P52_SPI_Transfer_Data(0x0115);//VBP
    T20P52_SPI_Transfer_Data(0x0153);//HFP
    T20P52_SPI_Transfer_Data(0x0162);//HBP

    //AmbaPrintColor(GREEN,"[LCD T20P52] Memory Access Control");
    //Memory Access Control
    T20P52_SPI_Transfer_Data(0x0036);
#ifdef LCD_REVERT
	T20P52_SPI_Transfer_Data(0x0108);
#else
    T20P52_SPI_Transfer_Data(0x01C8);
#endif

    //AmbaPrintColor(GREEN,"[LCD T20P52] COLMOD: Pixel Format Set");
    //COLMOD: Pixel Format Set
    T20P52_SPI_Transfer_Data(0x003A);
    //T20P52_SPI_Transfer_Data(0x0166);     //18BIT PIX FOMART
    T20P52_SPI_Transfer_Data(0x0166);     //6BIT PIX FOMART


    //AmbaPrintColor(GREEN,"[LCD T20P52] RGB Interface Signal Control");
    //RGB Interface Signal Control
    T20P52_SPI_Transfer_Data(0x00B0);
    T20P52_SPI_Transfer_Data(0x01E0);

    //AmbaPrintColor(GREEN,"[LCD T20P52] Display Inversion Control");
    //Display Inversion Control
    T20P52_SPI_Transfer_Data(0x00B4);
    T20P52_SPI_Transfer_Data(0x0102);//02  inversion

    //AmbaPrintColor(GREEN,"[LCD T20P52] Entry Mode Set");
    //Entry Mode Set
    T20P52_SPI_Transfer_Data(0x00B7);
    T20P52_SPI_Transfer_Data(0x0107);


    //AmbaPrintColor(GREEN,"[LCD T20P52] Interface Control");
    //Interface Control
    T20P52_SPI_Transfer_Data(0x00F6);
    T20P52_SPI_Transfer_Data(0x0101);//01
    T20P52_SPI_Transfer_Data(0x0100);
    T20P52_SPI_Transfer_Data(0x0107);


    //AmbaPrintColor(GREEN,"[LCD T20P52] Positive Gamma Correction");
    //Positive Gamma Correction
    T20P52_SPI_Transfer_Data(0x00E0);
    T20P52_SPI_Transfer_Data(0x0100);
    T20P52_SPI_Transfer_Data(0x0105);
    T20P52_SPI_Transfer_Data(0x0108);
    T20P52_SPI_Transfer_Data(0x0102);
    T20P52_SPI_Transfer_Data(0x011A);
    T20P52_SPI_Transfer_Data(0x010C);
    T20P52_SPI_Transfer_Data(0x0142);
    T20P52_SPI_Transfer_Data(0x017A);
    T20P52_SPI_Transfer_Data(0x0154);
    T20P52_SPI_Transfer_Data(0x0108);
    T20P52_SPI_Transfer_Data(0x010D);
    T20P52_SPI_Transfer_Data(0x010C);
    T20P52_SPI_Transfer_Data(0x0123);
    T20P52_SPI_Transfer_Data(0x0125);
    T20P52_SPI_Transfer_Data(0x010F);


    //AmbaPrintColor(GREEN,"[LCD T20P52] Negative Gamma Correction");
    //Negative Gamma Correction
    T20P52_SPI_Transfer_Data(0x00E1);
    T20P52_SPI_Transfer_Data(0x0100);
    T20P52_SPI_Transfer_Data(0x0129);
    T20P52_SPI_Transfer_Data(0x012F);
    T20P52_SPI_Transfer_Data(0x0103);
    T20P52_SPI_Transfer_Data(0x010F);
    T20P52_SPI_Transfer_Data(0x0105);
    T20P52_SPI_Transfer_Data(0x0142);
    T20P52_SPI_Transfer_Data(0x0155);
    T20P52_SPI_Transfer_Data(0x0153);
    T20P52_SPI_Transfer_Data(0x0106);
    T20P52_SPI_Transfer_Data(0x010F);
    T20P52_SPI_Transfer_Data(0x010C);
    T20P52_SPI_Transfer_Data(0x0138);
    T20P52_SPI_Transfer_Data(0x013A);
    T20P52_SPI_Transfer_Data(0x010F);


    //AmbaPrintColor(GREEN,"[LCD T20P52] Display Inversion OFF");
    //Display Inversion OFF
    T20P52_SPI_Transfer_Data(0x0020);

    //AmbaPrintColor(GREEN,"[LCD T20P52] Sleep Out, Exit Sleep");
    //Sleep Out, Exit Sleep
    T20P52_SPI_Transfer_Data(0x0011);

    //AmbaPrintColor(GREEN,"[LCD T20P52] Display on");
    //Display on
    T20P52_SPI_Transfer_Data(0x0029);


    //AmbaPrintColor(GREEN,"[LCD T20P52] //Memory Write");
    //Memory Write
    T20P52_SPI_Transfer_Data(0x002C);


    if (RetVal1 == NG || RetVal2 == NG)
        return NG;

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: LCD_T20P52Disable
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
static int LCD_T20P52Disable(void)
{
    return 0;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: LCD_T20P52GetInfo
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
static int LCD_T20P52GetInfo(AMBA_LCD_INFO_s *pInfo)
{
    if (T20P52_Ctrl.pDispConfig == NULL)
        return NG;

    pInfo->Width = T20P52_Ctrl.pDispConfig->Width;
    pInfo->Height = T20P52_Ctrl.pDispConfig->Height;
    pInfo->AspectRatio.X = 4;
    pInfo->AspectRatio.Y = 3;
    memcpy(&pInfo->FrameRate, &T20P52_Ctrl.pDispConfig->FrameRate, sizeof(AMBA_DSP_FRAME_RATE_s));

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: LCD_T20P52Config
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
static int LCD_T20P52Config(AMBA_LCD_MODE_ID_u Mode)
{
    AMBA_DSP_VOUT_DISPLAY_DIGITAL_CONFIG_s *pPixelFormat = &T20P52_Ctrl.PixelFormat;
    AMBA_DSP_VOUT_DISPLAY_TIMING_CONFIG_s *pDisplayTiming = &T20P52_Ctrl.DisplayTiming;
    AMBA_LCD_T20P52_CONFIG_s *pDispConfig;

    if (Mode.Bits.Mode >= AMBA_LCD_T20P52_NUM_MODE)
        return NG;


    pDispConfig = T20P52_Ctrl.pDispConfig = &T20P52_Config[Mode.Bits.Mode];

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

    pDisplayTiming->Rotation = T20P52_RotationTable[Mode.Bits.FlipVertical][Mode.Bits.FlipHorizontal];
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
    LCD_T20P52EnableConfig();
    AmbaKAL_TaskSleep(100);
    AmbaDSP_VoutDisplayTimingSetup(AMBA_DSP_VOUT_LCD, pDisplayTiming);
    AmbaDSP_VoutDisplayDigitalSetup(pPixelFormat);

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: LCD_T20P52SetBacklight
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
static int LCD_T20P52SetBacklight(INT32 EnableFlag)
{
    extern void AmbaUserGPIO_LcdCtrl(UINT32 LcdFlag);
    AmbaUserGPIO_LcdCtrl(EnableFlag);

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: LCD_T20P52SetBrightness
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
static int LCD_T20P52SetBrightness(INT32 Offset)
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

    return T20P52_Write(T20P52_REG_R0AH, RegVal);    /* Offset of brightness RGB */
#else
    return 0;
#endif
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: LCD_T20P52SetContrast
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
static int LCD_T20P52SetContrast(float Gain)
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

    return T20P52_Write(T20P52_REG_R0BH, RegVal);    /* RGB gain of contrast */
#else
    return 0;
#endif
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: LCD_T20P52SetColorBalance
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
static int LCD_T20P52SetColorBalance(AMBA_LCD_COLOR_BALANCE_s *pColorBalance)
{
#if 0
    UINT16 RegVal;

    if (pColorBalance == NULL)
        return NG;

    RegVal = T20P52_GetContrast(pColorBalance->GainRed);
    T20P52_Write(T20P52_REG_R0CH, RegVal);    /* R gain of contrast */
    RegVal = T20P52_GetBrightness(pColorBalance->OffsetRed);
    T20P52_Write(T20P52_REG_R0DH, RegVal);    /* R gain of brightness */

    RegVal = T20P52_GetContrast(pColorBalance->GainBlue);
    T20P52_Write(T20P52_REG_R0EH, RegVal);    /* B gain of contrast */
    RegVal = T20P52_GetBrightness(pColorBalance->OffsetBlue);
    T20P52_Write(T20P52_REG_R0FH, RegVal);    /* B gain of brightness */

    return OK;
#else
    return 0;
#endif
}

/*-----------------------------------------------------------------------------------------------*\
 * LCD driver object
\*-----------------------------------------------------------------------------------------------*/
AMBA_LCD_OBJECT_s AmbaLCD_T20P52Obj = {
    .LcdEnable = LCD_T20P52Enable,
    .LcdDisable = LCD_T20P52Disable,
    .LcdGetInfo = LCD_T20P52GetInfo,
    .LcdConfig = LCD_T20P52Config,
    .LcdSetBacklight = LCD_T20P52SetBacklight,
    .LcdSetBrightness = LCD_T20P52SetBrightness,
    .LcdSetContrast = LCD_T20P52SetContrast,
    .LcdSetColorBalance = LCD_T20P52SetColorBalance,

    .pName = "T20P52"
};
