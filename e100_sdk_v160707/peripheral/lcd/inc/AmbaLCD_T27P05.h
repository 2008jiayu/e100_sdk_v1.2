/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaLCD_T27P05.h
 *
 *  @Copyright      :: Copyright (C) 2015 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Definitions & Constants for Wintek 4:3 LCD panel WD-F9648W APIs.
\*-------------------------------------------------------------------------------------------------------------------*/

#ifndef _AMBA_LCD_T27P05_H_
#define _AMBA_LCD_T27P05_H_

/*-----------------------------------------------------------------------------------------------*\
 * Defintions for all the screen modes.
\*-----------------------------------------------------------------------------------------------*/
typedef enum _AMBA_LCD_T27P05_SCREEN_MODE_e_ {
    AMBA_LCD_T27P05_SCREEN_MODE_WIDE = 0,         /* Wide Screen Mode */
    AMBA_LCD_T27P05_SCREEN_MODE_NARROW            /* Narrow Screen Mode */
} AMBA_LCD_T27P05_SCREEN_MODE_e;

typedef enum _AMBA_LCD_T20P52_MODE_e_ {
    AMBA_LCD_T27P05_960_240_60HZ = 0,             /* 960x240@60Hz */
    AMBA_LCD_T27P05_960_240_50HZ,                 /* 960x240@50Hz */

    AMBA_LCD_T27P05_NUM_MODE                      /* Number of LCD mode */
} AMBA_LCD_T27P05_MODE_e;

/*-----------------------------------------------------------------------------------------------*\
 * LCD DISPLAY CONFIG
\*-----------------------------------------------------------------------------------------------*/
typedef struct _AMBA_LCD_T27P05_CONFIG_s_ {
    UINT32  Width;          /* Horizontal display resolution of LCD panel */
    UINT32  Height;         /* Vertical display resolution of LCD panel */
    AMBA_DSP_FRAME_RATE_s                   FrameRate;      /* Frame rate */
    AMBA_LCD_T27P05_SCREEN_MODE_e           ScreenMode;     /* Wide screen mode or Narrow screen mode */

    AMBA_DSP_VOUT_PIXEL_CLOCK_OUTPUT_e      DeviceClock;    /* Source clock frequency of LCD device */
    AMBA_DSP_VOUT_DIGITAL_OUTPUT_MODE_e     OutputMode;     /* Digital Output Mode Register */
    AMBA_DSP_VOUT_LCD_COLOR_ORDER_e         EvenLineColor;  /* RGB color sequence of even lines */
    AMBA_DSP_VOUT_LCD_COLOR_ORDER_e         OddLineColor;   /* RGB color sequence of odd lines */

    AMBA_LCD_TIMING_s       VideoTiming;
} AMBA_LCD_T27P05_CONFIG_s;


/*-----------------------------------------------------------------------------------------------*\
 * WD-F9648W LCD Panel Management Structure
\*-----------------------------------------------------------------------------------------------*/
typedef struct _AMBA_LCD_T20P52_CTRL_s_ {
    AMBA_SPI_CONFIG_s                       SpiConfig;  /* SPI control interface */
    AMBA_LCD_T27P05_CONFIG_s                *pDispConfig;
    AMBA_DSP_VOUT_DISPLAY_DIGITAL_CONFIG_s  PixelFormat;
    AMBA_DSP_VOUT_DISPLAY_TIMING_CONFIG_s   DisplayTiming;
} AMBA_LCD_T27P05_CTRL_s;

/*-----------------------------------------------------------------------------------------------*\
 * Defined in AmbaLCD_T27P05.c
\*-----------------------------------------------------------------------------------------------*/

#endif /* _AMBA_LCD_T27P05_H_ */
