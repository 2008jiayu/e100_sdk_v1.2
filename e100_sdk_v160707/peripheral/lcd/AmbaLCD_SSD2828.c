/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaLCD_SSD2828.c
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
#include "AmbaLCD_SSD2828.h"
#include "AmbaPrintk.h"


#define HTOTLE     548
#define VTOTLE     889

#define ACT_H	   480
#define ACT_V      854

#define HSPW       4
#define HBP        32
#define HFP        (HTOTLE - ACT_H - HBP)

#define VSPW       1
#define VBP        31
#define VFP        (VTOTLE - ACT_V - VBP)


/*-----------------------------------------------------------------------------------------------*\
 * Global instance for LCD info
\*-----------------------------------------------------------------------------------------------*/
static AMBA_LCD_SSD2828_CTRL_s SSD2828_Ctrl = {
    .SpiConfig = {
        .SlaveID       = 2,                                 /* Slave ID */
        .ProtocolMode  = AMBA_SPI_CPOL_LOW_CPHA_LOW,     	/* SPI Protocol mode */
        .CsPolarity    = AMBA_SPI_CHIP_SELECT_ACTIVE_LOW,   /* Slave select polarity */
        .DataFrameSize = 9,                                	/* Data Frame Size in Bit */
        .BaudRate      = 500000,                            /* Transfer BaudRate in Hz */
        .LsbFirst 	   = 0
    },
};

/*-----------------------------------------------------------------------------------------------*\
 * SSD2828 video mode info
\*-----------------------------------------------------------------------------------------------*/
static AMBA_LCD_SSD2828_CONFIG_s SSD2828_Config[AMBA_LCD_SSD2828_NUM_MODE] = {
    [AMBA_LCD_SSD2828_960_480_60HZ] = {
        .Width          = ACT_H,
        .Height         = ACT_V,
        .FrameRate      = {
            .Interlace  = 0,
            .TimeScale  = 60000,
            .NumUnitsInTick = 1001,
        },
        .ScreenMode     = AMBA_LCD_SSD2828_SCREEN_MODE_WIDE,
        .DeviceClock    = AMBA_DSP_VOUT_PIXEL_CLOCK_FULL_DCLK,  /* clock frequency to LCD */
        .OutputMode     = AMBA_DSP_VOUT_LCD_RGB565,     		/* pixel format */
        .EvenLineColor  = AMBA_DSP_VOUT_LCD_COLOR_RGB,          /* even line color order */
        .OddLineColor   = AMBA_DSP_VOUT_LCD_COLOR_RGB,          /* odd line color order */
        .VideoTiming    = {
            .PixelClock         = 29201090,
            .PixelRepetition    = 1,
            .Htotal             = HTOTLE,
            .Vtotal             = VTOTLE,
            .HsyncColStart      = 0,
            .HsyncColEnd        = HSPW,
            .VsyncColStart      = 0,
            .VsyncColEnd        = 0,
            .VsyncRowStart      = 0,
            .VsyncRowEnd        = VSPW,
            .ActiveColStart     = HBP,
            .ActiveColWidth     = ACT_H,
            .ActiveRowStart     = VBP,
            .ActiveRowHeight    = ACT_V
        }
    },
    [AMBA_LCD_SSD2828_960_480_50HZ] = {
        .Width          = ACT_H,
        .Height         = ACT_V,
        .FrameRate      = {
            .Interlace  = 0,
            .TimeScale  = 50,
            .NumUnitsInTick = 1,
        },
        .ScreenMode     = AMBA_LCD_SSD2828_SCREEN_MODE_WIDE,
        .DeviceClock    = AMBA_DSP_VOUT_PIXEL_CLOCK_FULL_DCLK,  /* clock frequency to LCD */
        .OutputMode     = AMBA_DSP_VOUT_LCD_RGB565,				/* pixel format */
        .EvenLineColor  = AMBA_DSP_VOUT_LCD_COLOR_RGB,          /* even line color order */
        .OddLineColor   = AMBA_DSP_VOUT_LCD_COLOR_RGB,          /* odd line color order */
        .VideoTiming    = {
            .PixelClock         = 24358600,
            .PixelRepetition    = 1,
            .Htotal             = HTOTLE,
            .Vtotal             = VTOTLE,
            .HsyncColStart      = 0,
            .HsyncColEnd        = HSPW,
            .VsyncColStart      = 0,
            .VsyncColEnd        = 0,
            .VsyncRowStart      = 0,
            .VsyncRowEnd        = VSPW,
            .ActiveColStart     = HBP,
            .ActiveColWidth     = ACT_H,
            .ActiveRowStart     = VBP,
            .ActiveRowHeight    = ACT_V
        }
    }
};

static AMBA_DSP_VOUT_ROTATE_TYPE_e SSD2828_RotationTable[2][2] = {
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
 *  @RoutineName:: SSD2828_Write
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
static int SSD2828_Write(UINT16 Offset, UINT16 Data)
{
#if 0
    UINT16 SpiCmd = (Offset << 9) | (Data & 0xFF);

    return AmbaSPI_Transfer(AMBA_SPI_MASTER, &SSD2828_Ctrl.SpiConfig, 1, &SpiCmd, NULL, 500);

#else
	UINT32 SpiCmd = (Offset << 16) | (Data & 0xFFFF);

    return AmbaSPI_Transfer(AMBA_SPI_MASTER, &SSD2828_Ctrl.SpiConfig, 1, &SpiCmd, NULL, 500);

#endif
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: SSD2828_Write
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
static int SSD2828_Write_8bit(UINT16 Mask, UINT16 Data)
{

	UINT16 SpiCmd = ((Mask << 8) & 0x100 ) | (Data & 0xFF);

    return AmbaSPI_Transfer(AMBA_SPI_MASTER, &SSD2828_Ctrl.SpiConfig, 1, &SpiCmd, NULL, 500);
}

static int SSD2828_Write_SSD2828(UINT8 Reg, UINT16 Data)
{
	UINT16 Lsb = Data & 0x00FF;
	UINT16 Msb = (Data >> 8) &0x00FF;

	SSD2828_Write_8bit(0, Reg);
	SSD2828_Write_8bit(1, Lsb);
	SSD2828_Write_8bit(1, Msb);
    AmbaPrint("SD2828: Reg=0x%x Data=0x%x",Reg,Data);
    return 0;
}

static int SSD2828_to_LCD_Change_Page(UINT16 data)
{
	SSD2828_Write_8bit(0, 0xBC); // packet size
	SSD2828_Write_8bit(1, 0x06);
	SSD2828_Write_8bit(1, 0x00);
	SSD2828_Write_8bit(0, 0xBF); // manufacturer command
	SSD2828_Write_8bit(1, 0xFF);
	SSD2828_Write_8bit(1, 0xFF);
	SSD2828_Write_8bit(1, 0x98);
	SSD2828_Write_8bit(1, 0x06);
	SSD2828_Write_8bit(1, 0x04);
	SSD2828_Write_8bit(1, data);
}

static int SSD2828_to_Write_LCD_REG_DATA(UINT16 Reg, UINT16 Data)
{
	SSD2828_Write_8bit(0, 0xBC); // packet size
	SSD2828_Write_8bit(1, 0x02);
	SSD2828_Write_8bit(1, 0x00);
	SSD2828_Write_8bit(0, 0xBF); // manufacturer command
	SSD2828_Write_8bit(1, Reg);
	SSD2828_Write_8bit(1, Data);
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: SSD2828_SPI_Transfer
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
static int SSD2828_SPI_Transfer(int NumSpiCmd, const UINT16 *pSpiCmdBuf)
{
    int i;
    int RetVal = OK;
    UINT16 SpiCmd;

    /* Note that each serial command must consist of 16 bits of data */
    for (i = 0; i < NumSpiCmd; i++) {
        SpiCmd = pSpiCmdBuf[i];

        RetVal = AmbaSPI_Transfer(AMBA_SPI_MASTER, &SSD2828_Ctrl.SpiConfig, 1, &SpiCmd, NULL, 500);

        if (RetVal == TX_NO_EVENTS) {
            AmbaPrint("SSD2828_SPI_Transfer timed out!");
            return NG;
        } else if (RetVal == NG) {
            AmbaPrint("SSD2828_SPI_Transfer failed!");
            return NG;
        }
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: SSD2828_SetInputControl
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
static int SSD2828_SetInputControl(AMBA_LCD_SSD2828_CTRL_TYPE_e InputCtrl)
{

    AMBA_DSP_VOUT_DISPLAY_DIGITAL_CONFIG_s *pPixelFormat = &SSD2828_Ctrl.PixelFormat;
    UINT16 RegVal = 0x00;

    if (SSD2828_Ctrl.pDispConfig->ScreenMode == AMBA_LCD_SSD2828_SCREEN_MODE_NARROW)
        RegVal |= 0x80;

    if (InputCtrl == AMBA_LCD_SSD2828_CTRL_DE) {
        RegVal |= 0x10;
        if (pPixelFormat->LineValidPolarity == AMBA_DSP_VOUT_ACTIVE_LOW)
            RegVal |= 0x08;
    } else if (InputCtrl == AMBA_LCD_SSD2828_CTRL_HV_SYNC) {
        if (pPixelFormat->FrameSyncPolarity == AMBA_DSP_VOUT_ACTIVE_LOW)
            RegVal |= 0x04;
        if (pPixelFormat->LineSyncPolarity == AMBA_DSP_VOUT_ACTIVE_LOW)
            RegVal |= 0x02;
    }

    if (pPixelFormat->ClkSampleEdge == AMBA_DSP_VOUT_ACTIVE_LOW)
        RegVal |= 0x01;
#if 0
    return SSD2828_Write(SSD2828_REG_R02H, RegVal);
#else
	return OK;
#endif
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: SSD2828_SetInputFormat
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
static int SSD2828_SetInputFormat(AMBA_LCD_SSD2828_INPUT_FORMAT_e InputFormat)
{
#if 0
    UINT16 RegVal = 0x03;

    RegVal |= ((UINT16) InputFormat << 4);

    return SSD2828_Write(SSD2828_REG_R03H, RegVal);
#else
	return OK;
#endif
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: SSD2828_GetContrast
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
static UINT16 SSD2828_GetContrast(float Gain)
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
 *  @RoutineName:: SSD2828_GetBrightness
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
static UINT16 SSD2828_GetBrightness(INT32 Offset)
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
 *  @RoutineName:: LCD_Ssd2828Enable
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
#define SSD2828_SHUT_PIN		GPIO_PIN_19
#define SSD2828_RESET_PIN 	GPIO_PIN_20
#define SSD2828_SPI_EN_PIN	GPIO_PIN_37
#define LCD_GPIO_RESET_PIN	GPIO_PIN_50

static int LCD_Ssd2828Enable(void)
{
#if 1
	AmbaPrint("LCD_SSD2828 Enable....");
	AmbaGPIO_ConfigOutput(SSD2828_SHUT_PIN, AMBA_GPIO_LEVEL_LOW);
	AmbaKAL_TaskSleep(10);

	AmbaGPIO_ConfigOutput(SSD2828_RESET_PIN, AMBA_GPIO_LEVEL_HIGH);
	AmbaKAL_TaskSleep(10);
	AmbaGPIO_ConfigOutput(SSD2828_RESET_PIN, AMBA_GPIO_LEVEL_LOW);
	AmbaKAL_TaskSleep(20);
	AmbaGPIO_ConfigOutput(SSD2828_RESET_PIN, AMBA_GPIO_LEVEL_HIGH);
	AmbaKAL_TaskSleep(120);

	AmbaGPIO_ConfigOutput(LCD_GPIO_RESET_PIN, AMBA_GPIO_LEVEL_HIGH);
	AmbaKAL_TaskSleep(10);
	AmbaGPIO_ConfigOutput(LCD_GPIO_RESET_PIN, AMBA_GPIO_LEVEL_LOW);
	AmbaKAL_TaskSleep(20);
	AmbaGPIO_ConfigOutput(LCD_GPIO_RESET_PIN, AMBA_GPIO_LEVEL_HIGH);
	AmbaKAL_TaskSleep(10);

	//SSPI 0
    AmbaGPIO_ConfigOutput(SSD2828_SPI_EN_PIN, AMBA_GPIO_LEVEL_LOW);
	AmbaKAL_TaskSleep(10);	
#else
	AmbaGPIO_ConfigOutput(GPIO_PIN_6, AMBA_GPIO_LEVEL_HIGH);
	AmbaKAL_TaskSleep(10);
	AmbaGPIO_ConfigOutput(GPIO_PIN_6, AMBA_GPIO_LEVEL_LOW);
	AmbaKAL_TaskSleep(20);
	AmbaGPIO_ConfigOutput(GPIO_PIN_6, AMBA_GPIO_LEVEL_HIGH);
	AmbaKAL_TaskSleep(120);
#endif

	SSD2828_Write_SSD2828(0xB7,0x0050);
	SSD2828_Write_SSD2828(0xB8,0x0000);
	SSD2828_Write_SSD2828(0xB9,0x0000);
	SSD2828_Write_SSD2828(0xBA,0x4214);
	SSD2828_Write_SSD2828(0xBB,0x0005);
	SSD2828_Write_SSD2828(0xB9,0x0001);
	SSD2828_Write_SSD2828(0xDE,0x0001);// two lane
	SSD2828_Write_SSD2828(0xC9,0x2302); // write and DCS mode 0x0302 generic mode
	AmbaKAL_TaskSleep(10);

	{
		SSD2828_Write_SSD2828(0xB9,0x0001);
		SSD2828_Write_SSD2828(0xB7,0x0302);
		SSD2828_Write_SSD2828(0xB8,0x0010);

		SSD2828_to_LCD_Change_Page(0x01);     // Change to Page 1
		SSD2828_to_Write_LCD_REG_DATA(0x08,0x10);                // output SDA
		SSD2828_to_Write_LCD_REG_DATA(0x21,0x0D);                 // 0x01 DE = 1 Active
		SSD2828_to_Write_LCD_REG_DATA(0x25,VFP);                 // VFP
		SSD2828_to_Write_LCD_REG_DATA(0x26,VBP);                 // VBP
		SSD2828_to_Write_LCD_REG_DATA(0x27,(HBP&0xFF));                 // HBP[0..7]
		SSD2828_to_Write_LCD_REG_DATA(0x28,((HBP>>8)&0x03));                 // HBP[8..9]= HBP
		SSD2828_to_Write_LCD_REG_DATA(0x30,0x01);                 // 480 X 854
		SSD2828_to_Write_LCD_REG_DATA(0x31,0x00);                 // 02=2-dot Inversion  00
		SSD2828_to_Write_LCD_REG_DATA(0x40,0x16);               // BT  +2.5/-2.5 pump for DDVDH-L
		SSD2828_to_Write_LCD_REG_DATA(0x41,0x77);                // DVDDH DVDDL clamp
		SSD2828_to_Write_LCD_REG_DATA(0x42,0x02);                 // VGH/VGL
		SSD2828_to_Write_LCD_REG_DATA(0x44,0x16);                 // VGH/VGL
		SSD2828_to_Write_LCD_REG_DATA(0x50,0x69);                 // VGMP
		SSD2828_to_Write_LCD_REG_DATA(0x51,0x69);                 // VGMN
		SSD2828_to_Write_LCD_REG_DATA(0x52,0x00);                   //Flicker
		SSD2828_to_Write_LCD_REG_DATA(0x53,0x40);                  //Flicker  40 60
		SSD2828_to_Write_LCD_REG_DATA(0x57,0x50);
		SSD2828_to_Write_LCD_REG_DATA(0x60,0x07);                 // SDTI
		SSD2828_to_Write_LCD_REG_DATA(0x61,0x00);                // CRTI
		SSD2828_to_Write_LCD_REG_DATA(0x62,0x08);                 // EQTI
		SSD2828_to_Write_LCD_REG_DATA(0x63,0x00);               // PCTI
		//++++++++++++++++++ Gamma Setting ++++++++++++++++++//
		SSD2828_to_Write_LCD_REG_DATA(0xA0,0x00);  // Gamma 0
		SSD2828_to_Write_LCD_REG_DATA(0xA1,0x0B);  // Gamma 4
		SSD2828_to_Write_LCD_REG_DATA(0xA2,0x14);  // Gamma 8
		SSD2828_to_Write_LCD_REG_DATA(0xA3,0x0A);  // Gamma 16
		SSD2828_to_Write_LCD_REG_DATA(0xA4,0x03);  // Gamma 24
		SSD2828_to_Write_LCD_REG_DATA(0xA5,0x0C);  // Gamma 52
		SSD2828_to_Write_LCD_REG_DATA(0xA6,0x07);  // Gamma 80
		SSD2828_to_Write_LCD_REG_DATA(0xA7,0x03);  // Gamma 108
		SSD2828_to_Write_LCD_REG_DATA(0xA8,0x0B);  // Gamma 147
		SSD2828_to_Write_LCD_REG_DATA(0xA9,0x0D);  // Gamma 175
		SSD2828_to_Write_LCD_REG_DATA(0xAA,0x0C);  // Gamma 203
		SSD2828_to_Write_LCD_REG_DATA(0xAB,0x08);  // Gamma 231
		SSD2828_to_Write_LCD_REG_DATA(0xAC,0x0B);  // Gamma 239
		SSD2828_to_Write_LCD_REG_DATA(0xAD,0x1B);  // Gamma 247
		SSD2828_to_Write_LCD_REG_DATA(0xAE,0x08);  // Gamma 251
		SSD2828_to_Write_LCD_REG_DATA(0xAF,0x00);  // Gamma 255
		///==============Nagitive
		SSD2828_to_Write_LCD_REG_DATA(0xC0,0x00);  // Gamma 0
		SSD2828_to_Write_LCD_REG_DATA(0xC1,0x0D);  // Gamma 4
		SSD2828_to_Write_LCD_REG_DATA(0xC2,0x12);  // Gamma 8
		SSD2828_to_Write_LCD_REG_DATA(0xC3,0x10);  // Gamma 16
		SSD2828_to_Write_LCD_REG_DATA(0xC4,0x08);  // Gamma 24
		SSD2828_to_Write_LCD_REG_DATA(0xC5,0x0B);  // Gamma 52
		SSD2828_to_Write_LCD_REG_DATA(0xC6,0x05);  // Gamma 80
		SSD2828_to_Write_LCD_REG_DATA(0xC7,0x07);  // Gamma 108
		SSD2828_to_Write_LCD_REG_DATA(0xC8,0x03);  // Gamma 147
		SSD2828_to_Write_LCD_REG_DATA(0xC9,0x07);  // Gamma 175
		SSD2828_to_Write_LCD_REG_DATA(0xCA,0x0E);  // Gamma 203
		SSD2828_to_Write_LCD_REG_DATA(0xCB,0x04);  // Gamma 231
		SSD2828_to_Write_LCD_REG_DATA(0xCC,0x0C);  // Gamma 239
		SSD2828_to_Write_LCD_REG_DATA(0xCD,0x1C);  // Gamma 247
		SSD2828_to_Write_LCD_REG_DATA(0xCE,0x0A);  // Gamma 251
		SSD2828_to_Write_LCD_REG_DATA(0xCF,0x00);  // Gamma 255
		//****************************** Page 6 Command ******************************//
		SSD2828_to_LCD_Change_Page(0x06);     // Change to Page 6
		SSD2828_to_Write_LCD_REG_DATA(0x00,0x21);
		SSD2828_to_Write_LCD_REG_DATA(0x01,0x05);
		SSD2828_to_Write_LCD_REG_DATA(0x02,0x00);
		SSD2828_to_Write_LCD_REG_DATA(0x03,0x00);
		SSD2828_to_Write_LCD_REG_DATA(0x04,0x01);
		SSD2828_to_Write_LCD_REG_DATA(0x05,0x01);
		SSD2828_to_Write_LCD_REG_DATA(0x06,0x80);
		SSD2828_to_Write_LCD_REG_DATA(0x07,0x04);
		SSD2828_to_Write_LCD_REG_DATA(0x08,0x03);
		SSD2828_to_Write_LCD_REG_DATA(0x09,0x00);
		SSD2828_to_Write_LCD_REG_DATA(0x0A,0x00);
		SSD2828_to_Write_LCD_REG_DATA(0x0B,0x00);
		SSD2828_to_Write_LCD_REG_DATA(0x0C,0x01);
		SSD2828_to_Write_LCD_REG_DATA(0x0D,0x01);
		SSD2828_to_Write_LCD_REG_DATA(0x0E,0x00);
		SSD2828_to_Write_LCD_REG_DATA(0x0F,0x00);
		SSD2828_to_Write_LCD_REG_DATA(0x10,0x50);
		SSD2828_to_Write_LCD_REG_DATA(0x11,0x50);
		SSD2828_to_Write_LCD_REG_DATA(0x12,0x00);
		SSD2828_to_Write_LCD_REG_DATA(0x13,0x00);
		SSD2828_to_Write_LCD_REG_DATA(0x14,0x00);
		SSD2828_to_Write_LCD_REG_DATA(0x15,0xC0);
		SSD2828_to_Write_LCD_REG_DATA(0x16,0x08);
		SSD2828_to_Write_LCD_REG_DATA(0x17,0x00);
		SSD2828_to_Write_LCD_REG_DATA(0x18,0x00);
		SSD2828_to_Write_LCD_REG_DATA(0x19,0x00);
		SSD2828_to_Write_LCD_REG_DATA(0x1A,0x00);
		SSD2828_to_Write_LCD_REG_DATA(0x1B,0x00);
		SSD2828_to_Write_LCD_REG_DATA(0x1C,0x00);
		SSD2828_to_Write_LCD_REG_DATA(0x1D,0x00);
		SSD2828_to_Write_LCD_REG_DATA(0x20,0x01);
		SSD2828_to_Write_LCD_REG_DATA(0x21,0x23);
		SSD2828_to_Write_LCD_REG_DATA(0x22,0x45);
		SSD2828_to_Write_LCD_REG_DATA(0x23,0x67);
		SSD2828_to_Write_LCD_REG_DATA(0x24,0x01);
		SSD2828_to_Write_LCD_REG_DATA(0x25,0x23);
		SSD2828_to_Write_LCD_REG_DATA(0x26,0x45);
		SSD2828_to_Write_LCD_REG_DATA(0x27,0x67);
		SSD2828_to_Write_LCD_REG_DATA(0x30,0x02);
		SSD2828_to_Write_LCD_REG_DATA(0x31,0x22);
		SSD2828_to_Write_LCD_REG_DATA(0x32,0x11);
		SSD2828_to_Write_LCD_REG_DATA(0x33,0xAA);
		SSD2828_to_Write_LCD_REG_DATA(0x34,0xBB);
		SSD2828_to_Write_LCD_REG_DATA(0x35,0x66);
		SSD2828_to_Write_LCD_REG_DATA(0x36,0x00);
		SSD2828_to_Write_LCD_REG_DATA(0x37,0x22);
		SSD2828_to_Write_LCD_REG_DATA(0x38,0x22);
		SSD2828_to_Write_LCD_REG_DATA(0x39,0x22);
		SSD2828_to_Write_LCD_REG_DATA(0x3A,0x22);
		SSD2828_to_Write_LCD_REG_DATA(0x3B,0x22);
		SSD2828_to_Write_LCD_REG_DATA(0x3C,0x22);
		SSD2828_to_Write_LCD_REG_DATA(0x3D,0x22);
		SSD2828_to_Write_LCD_REG_DATA(0x3E,0x22);
		SSD2828_to_Write_LCD_REG_DATA(0x3F,0x22);
		SSD2828_to_Write_LCD_REG_DATA(0x40,0x22);
		SSD2828_to_Write_LCD_REG_DATA(0x53,0x10);  //VGLO
		//****************************** Page 7 Command ******************************//
		SSD2828_to_LCD_Change_Page(0x07);     // Change to Page 7
	    SSD2828_to_Write_LCD_REG_DATA(0x18,0x1D);
	    SSD2828_to_Write_LCD_REG_DATA(0x02,0x77);//???????????

		//****************************** Page 0 Command ******************************//
		SSD2828_to_LCD_Change_Page(0x00);     // Change to Page 0
		SSD2828_to_Write_LCD_REG_DATA(0x3A,0x77);

		AmbaKAL_TaskSleep(5);
		SSD2828_to_Write_LCD_REG_DATA(0x11,0x00);                 // Sleep-Out
	    AmbaKAL_TaskSleep(150);
		SSD2828_to_Write_LCD_REG_DATA(0x29,0x00);                 // Display On
		AmbaKAL_TaskSleep(50);
	}

    SSD2828_Write_SSD2828(0xB7,0x0050);
	SSD2828_Write_SSD2828(0xB8,0x0000);
	SSD2828_Write_SSD2828(0xB9,0x0000);
	SSD2828_Write_SSD2828(0xBa,0x8228);//0x8224);
	SSD2828_Write_SSD2828(0xBb,0x0006);
	SSD2828_Write_SSD2828(0xB9,0x0001);
	SSD2828_Write_SSD2828(0xc9,0x2302);
	AmbaKAL_TaskSleep(50);

	SSD2828_Write_SSD2828(0xca,0x2305);// two lane
	SSD2828_Write_SSD2828(0xcb,0x0510);
	SSD2828_Write_SSD2828(0xcc,0x1005);

	SSD2828_Write_SSD2828(0xB1,(((VSPW<<8)&0xFF00)|(HSPW&0x00ff)));
	SSD2828_Write_SSD2828(0xB2,(((VBP<<8)&0xFF00)|(HBP&0x00ff)));
	SSD2828_Write_SSD2828(0xB3,(((VFP<<8)&0xFF00)|(HFP&0x00ff)));
	SSD2828_Write_SSD2828(0xB4,ACT_H);
	SSD2828_Write_SSD2828(0xB5,ACT_V);
	SSD2828_Write_SSD2828(0xB6,0x0000);

	SSD2828_Write_SSD2828(0xDE,0x0001);// two lane
	SSD2828_Write_SSD2828(0xD6,0x0001);

	SSD2828_Write_SSD2828(0xB7,0x024B);

	//SPI EN
	AmbaKAL_TaskSleep(10);
	AmbaGPIO_ConfigOutput(SSD2828_SPI_EN_PIN, AMBA_GPIO_LEVEL_HIGH);	
    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: LCD_Ssd2828Disable
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
static int LCD_Ssd2828Disable(void)
{
#if 0
    return SSD2828_Write(SSD2828_REG_R01H, (UINT16) 0x00); /* Standby mode */
#else
	return OK;
#endif
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: LCD_Ssd2828GetInfo
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
static int LCD_Ssd2828GetInfo(AMBA_LCD_INFO_s *pInfo)
{
    if (SSD2828_Ctrl.pDispConfig == NULL)
        return NG;

    pInfo->Width = SSD2828_Ctrl.pDispConfig->Width;
    pInfo->Height = SSD2828_Ctrl.pDispConfig->Height;
    pInfo->AspectRatio.X = 9;
    pInfo->AspectRatio.Y = 16;
    memcpy(&pInfo->FrameRate, &SSD2828_Ctrl.pDispConfig->FrameRate, sizeof(AMBA_DSP_FRAME_RATE_s));

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: LCD_Ssd2828Config
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
static int LCD_Ssd2828Config(AMBA_LCD_MODE_ID_u Mode)
{
    AMBA_DSP_VOUT_DISPLAY_DIGITAL_CONFIG_s *pPixelFormat = &SSD2828_Ctrl.PixelFormat;
    AMBA_DSP_VOUT_DISPLAY_TIMING_CONFIG_s *pDisplayTiming = &SSD2828_Ctrl.DisplayTiming;
    AMBA_LCD_SSD2828_CONFIG_s *pDispConfig;

    if (Mode.Bits.Mode >= AMBA_LCD_SSD2828_NUM_MODE)
        return NG;

    pDispConfig = SSD2828_Ctrl.pDispConfig = &SSD2828_Config[Mode.Bits.Mode];

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

    pDisplayTiming->Rotation = SSD2828_RotationTable[Mode.Bits.FlipVertical][Mode.Bits.FlipHorizontal];
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
 *  @RoutineName:: LCD_Ssd2828SetBacklight
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
static int LCD_Ssd2828SetBacklight(INT32 EnableFlag)
{
  extern void AmbaUserGPIO_LcdCtrl(UINT32 LcdFlag);
    AmbaUserGPIO_LcdCtrl(EnableFlag);

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: LCD_Ssd2828SetBrightness
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
static int LCD_Ssd2828SetBrightness(INT32 Offset)
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

    return SSD2828_Write(SSD2828_REG_R0AH, RegVal);    /* Offset of brightness RGB */
#else
	return OK;
#endif
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: LCD_Ssd2828SetContrast
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
static int LCD_Ssd2828SetContrast(float Gain)
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

    return SSD2828_Write(SSD2828_REG_R0BH, RegVal);    /* RGB gain of contrast */
#else
	return OK;
#endif
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: LCD_Ssd2828SetColorBalance
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
static int LCD_Ssd2828SetColorBalance(AMBA_LCD_COLOR_BALANCE_s *pColorBalance)
{
#if 0
    UINT16 RegVal;

    if (pColorBalance == NULL)
        return NG;

    RegVal = SSD2828_GetContrast(pColorBalance->GainRed);
    SSD2828_Write(SSD2828_REG_R0CH, RegVal);    /* R gain of contrast */
    RegVal = SSD2828_GetBrightness(pColorBalance->OffsetRed);
    SSD2828_Write(SSD2828_REG_R0DH, RegVal);    /* R gain of brightness */

    RegVal = SSD2828_GetContrast(pColorBalance->GainBlue);
    SSD2828_Write(SSD2828_REG_R0EH, RegVal);    /* B gain of contrast */
    RegVal = SSD2828_GetBrightness(pColorBalance->OffsetBlue);
    SSD2828_Write(SSD2828_REG_R0FH, RegVal);    /* B gain of brightness */
#endif
    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 * LCD driver object
\*-----------------------------------------------------------------------------------------------*/
AMBA_LCD_OBJECT_s AmbaLCD_SSD2828Obj = {
    .LcdEnable = LCD_Ssd2828Enable,
    .LcdDisable = LCD_Ssd2828Disable,
    .LcdGetInfo = LCD_Ssd2828GetInfo,
    .LcdConfig = LCD_Ssd2828Config,
    .LcdSetBacklight = LCD_Ssd2828SetBacklight,
    .LcdSetBrightness = LCD_Ssd2828SetBrightness,
    .LcdSetContrast = LCD_Ssd2828SetContrast,
    .LcdSetColorBalance = LCD_Ssd2828SetColorBalance,

    .pName = "SSD2828"
};
