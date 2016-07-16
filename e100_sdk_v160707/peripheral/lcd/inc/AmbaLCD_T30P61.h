/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaLCD_T30P61.h
 *
 *  @Copyright      :: Copyright (C) 2015 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Definitions & Constants for Shenzhen RZW LCD panel T30P61 APIs.
\*-------------------------------------------------------------------------------------------------------------------*/

#ifndef _AMBA_LCD_T30P61_H_
#define _AMBA_LCD_T30P61_H_

/*-----------------------------------------------------------------------------------------------*\
 * Defintions for all the screen modes.
\*-----------------------------------------------------------------------------------------------*/
typedef enum _AMBA_LCD_T30P61_SCREEN_MODE_e_ {
    AMBA_LCD_T30P61_SCREEN_MODE_WIDE = 0,         /* Wide Screen Mode */
    AMBA_LCD_T30P61_SCREEN_MODE_NARROW            /* Narrow Screen Mode */
} AMBA_LCD_T30P61_SCREEN_MODE_e;

/*-----------------------------------------------------------------------------------------------*\
 * Defintions for all the control signal types.
\*-----------------------------------------------------------------------------------------------*/
typedef enum _AMBA_LCD_T30P61_CTRL_TYPE_e_ {
    AMBA_LCD_T30P61_CTRL_DE = 0,                  /* Data Enable (DE) signal */
    AMBA_LCD_T30P61_CTRL_HV_SYNC                  /* H/V Sync signals */
} AMBA_LCD_T30P61_CTRL_TYPE_e;

typedef enum _AMBA_LCD_T30P61_MODE_e_ {
    AMBA_LCD_T30P61_960_240_60HZ = 0,             /* 960x480@60Hz */
    AMBA_LCD_T30P61_960_240_50HZ,                 /* 960x480@50Hz */

    AMBA_LCD_T30P61_NUM_MODE                      /* Number of LCD mode */
} AMBA_LCD_T30P61_MODE_e;

/*-----------------------------------------------------------------------------------------------*\
 * LCD DISPLAY CONFIG
\*-----------------------------------------------------------------------------------------------*/
typedef struct _AMBA_LCD_T30P61_CONFIG_s_ {
    UINT32  Width;          /* Horizontal display resolution of LCD panel */
    UINT32  Height;         /* Vertical display resolution of LCD panel */
    AMBA_DSP_FRAME_RATE_s                   FrameRate;      /* Frame rate */
    AMBA_LCD_T30P61_SCREEN_MODE_e         ScreenMode;     /* Wide screen mode or Narrow screen mode */

    AMBA_DSP_VOUT_PIXEL_CLOCK_OUTPUT_e      DeviceClock;    /* Source clock frequency of LCD device */
    AMBA_DSP_VOUT_DIGITAL_OUTPUT_MODE_e     OutputMode;     /* Digital Output Mode Register */
    AMBA_DSP_VOUT_LCD_COLOR_ORDER_e         EvenLineColor;  /* RGB color sequence of even lines */
    AMBA_DSP_VOUT_LCD_COLOR_ORDER_e         OddLineColor;   /* RGB color sequence of odd lines */

    AMBA_LCD_TIMING_s       VideoTiming;
} AMBA_LCD_T30P61_CONFIG_s;


/*-----------------------------------------------------------------------------------------------*\
 * T30P61 LCD Panel Management Structure
\*-----------------------------------------------------------------------------------------------*/
typedef struct _AMBA_LCD_T30P61_CTRL_s_ {
    AMBA_SPI_CHANNEL_e  SpiChanNo;  /* SPI control interface */
    AMBA_SPI_CONFIG_s   SpiConfig;  /* SPI control interface */
    AMBA_LCD_T30P61_CONFIG_s              *pDispConfig;
    AMBA_DSP_VOUT_DISPLAY_DIGITAL_CONFIG_s  PixelFormat;
    AMBA_DSP_VOUT_DISPLAY_TIMING_CONFIG_s   DisplayTiming;
} AMBA_LCD_T30P61_CTRL_s;

/*-----------------------------------------------------------------------------------------------*\
 * Defined in AmbaLCD_T30P61.c
\*-----------------------------------------------------------------------------------------------*/

#endif /* _AMBA_LCD_T30P61_H_ */
