/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaLCD_SSD2828.h
 *
 *  @Copyright      :: Copyright (C) 2012 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Definitions & Constants for Wintek 4:3 LCD panel WD-F9648W APIs.
\*-------------------------------------------------------------------------------------------------------------------*/

#ifndef _AMBA_LCD_SSD2828_H_
#define _AMBA_LCD_SSD2828_H_

typedef enum _AMBA_SSD2828_REG_e_ {
    SSD2828_REG_R01H = 0x01,
    SSD2828_REG_R02H,
    SSD2828_REG_R03H,
    SSD2828_REG_R04H,
    SSD2828_REG_R05H,
    SSD2828_REG_R06H,
    SSD2828_REG_R07H,
    SSD2828_REG_R08H,
    SSD2828_REG_R09H,
    SSD2828_REG_R0AH,
    SSD2828_REG_R0BH,
    SSD2828_REG_R0CH,
    SSD2828_REG_R0DH,
    SSD2828_REG_R0EH,
    SSD2828_REG_R0FH,
    SSD2828_REG_R10H,
    SSD2828_REG_R11H,
    SSD2828_REG_R12H,
    SSD2828_REG_R13H,
    SSD2828_REG_R14H,
    SSD2828_REG_R15H,
    SSD2828_REG_R16H,
    SSD2828_REG_R17H,
    SSD2828_REG_R18H,
    SSD2828_REG_R19H,
    SSD2828_REG_R20H,
    SSD2828_REG_R21H,
    SSD2828_REG_R22H,
    SSD2828_REG_R23H,
    SSD2828_REG_R24H,
    SSD2828_REG_R25H,
    SSD2828_REG_R26H,
    SSD2828_REG_R27H,
    SSD2828_REG_R28H,
    SSD2828_REG_R29H,
    SSD2828_REG_R2AH,
    SSD2828_REG_R2BH,
    SSD2828_REG_R2CH,
    SSD2828_REG_R2DH,
    SSD2828_REG_R2EH,
    SSD2828_REG_R2FH,
    SSD2828_REG_R30H,
    SSD2828_REG_R31H,
    SSD2828_REG_R32H,
    SSD2828_REG_R33H,
    SSD2828_REG_R34H,
    SSD2828_REG_R35H,
    SSD2828_REG_R36H,
    SSD2828_REG_R37H,
    SSD2828_REG_R38H
} AMBA_SSD2828_REG_e;

typedef enum _AMBA_LCD_SSD2828_INPUT_FORMAT_e_ {
    AMBA_LCD_SSD2828_8BIT_RGB_THROUGH_MODE = 0,    /* 8-bit RGB Through Mode mode */
    AMBA_LCD_SSD2828_8BIT_RGB_DA_MODE,             /* 8-bit RGB DA Mode mode */
    AMBA_LCD_SSD2828_8BIT_RGB_720_MODE,            /* 8-bit RGB 720 Mode mode */
    AMBA_LCD_SSD2828_8BIT_RGB_640_MODE,            /* 8-bit RGB 640 Mode mode */
    AMBA_LCD_SSD2828_8BIT_YUV_720_MODE,            /* 8-bit YUV 720 Mode mode */
    AMBA_LCD_SSD2828_8BIT_YUV_640_MODE,            /* 8-bit YUV 640 Mode mode */
    AMBA_LCD_SSD2828_16BIT_RGB_THROUGH_MODE,       /* 16-bit RGB Through Mode mode */
    AMBA_LCD_SSD2828_16BIT_RGB_DA_MODE,            /* 16-bit RGB DA Mode mode */
    AMBA_LCD_SSD2828_16BIT_RGB_720_MODE,           /* 16-bit RGB 720 Mode mode */
    AMBA_LCD_SSD2828_16BIT_RGB_640_MODE,           /* 16-bit RGB 640 Mode mode */
    AMBA_LCD_SSD2828_24BIT_RGB_THROUGH_MODE,       /* 24-bit RGB Through Mode mode */
    AMBA_LCD_SSD2828_24BIT_RGB_DA_MODE             /* 24-bit RGB DA Mode mode */
} AMBA_LCD_SSD2828_INPUT_FORMAT_e;

/*-----------------------------------------------------------------------------------------------*\
 * Defintions for all the screen modes.
\*-----------------------------------------------------------------------------------------------*/
typedef enum _AMBA_LCD_SSD2828_SCREEN_MODE_e_ {
    AMBA_LCD_SSD2828_SCREEN_MODE_WIDE = 0,         /* Wide Screen Mode */
    AMBA_LCD_SSD2828_SCREEN_MODE_NARROW            /* Narrow Screen Mode */
} AMBA_LCD_SSD2828_SCREEN_MODE_e;

/*-----------------------------------------------------------------------------------------------*\
 * Defintions for all the control signal types.
\*-----------------------------------------------------------------------------------------------*/
typedef enum _AMBA_LCD_SSD2828_CTRL_TYPE_e_ {
    AMBA_LCD_SSD2828_CTRL_DE = 0,                  /* Data Enable (DE) signal */
    AMBA_LCD_SSD2828_CTRL_HV_SYNC                  /* H/V Sync signals */
} AMBA_LCD_SSD2828_CTRL_TYPE_e;

typedef enum _AMBA_LCD_SSD2828_MODE_e_ {
    AMBA_LCD_SSD2828_960_480_60HZ = 0,             /* 960x480@60Hz */
    AMBA_LCD_SSD2828_960_480_50HZ,                 /* 960x480@50Hz */

    AMBA_LCD_SSD2828_NUM_MODE                      /* Number of LCD mode */
} AMBA_LCD_SSD2828_MODE_e;

/*-----------------------------------------------------------------------------------------------*\
 * LCD DISPLAY CONFIG
\*-----------------------------------------------------------------------------------------------*/
typedef struct _AMBA_LCD_SSD2828_CONFIG_s_ {
    UINT32  Width;          /* Horizontal display resolution of LCD panel */
    UINT32  Height;         /* Vertical display resolution of LCD panel */
    AMBA_DSP_FRAME_RATE_s                   FrameRate;      /* Frame rate */
    AMBA_LCD_SSD2828_SCREEN_MODE_e         ScreenMode;     /* Wide screen mode or Narrow screen mode */

    AMBA_DSP_VOUT_PIXEL_CLOCK_OUTPUT_e      DeviceClock;    /* Source clock frequency of LCD device */
    AMBA_DSP_VOUT_DIGITAL_OUTPUT_MODE_e     OutputMode;     /* Digital Output Mode Register */
    AMBA_DSP_VOUT_LCD_COLOR_ORDER_e         EvenLineColor;  /* RGB color sequence of even lines */
    AMBA_DSP_VOUT_LCD_COLOR_ORDER_e         OddLineColor;   /* RGB color sequence of odd lines */

    AMBA_LCD_TIMING_s       VideoTiming;
} AMBA_LCD_SSD2828_CONFIG_s;


/*-----------------------------------------------------------------------------------------------*\
 * WD-F9648W LCD Panel Management Structure
\*-----------------------------------------------------------------------------------------------*/
typedef struct _AMBA_LCD_SSD2828_CTRL_s_ {
    AMBA_SPI_CONFIG_s                       SpiConfig;  /* SPI control interface */
    AMBA_LCD_SSD2828_CONFIG_s              *pDispConfig;
    AMBA_DSP_VOUT_DISPLAY_DIGITAL_CONFIG_s  PixelFormat;
    AMBA_DSP_VOUT_DISPLAY_TIMING_CONFIG_s   DisplayTiming;
} AMBA_LCD_SSD2828_CTRL_s;

/*-----------------------------------------------------------------------------------------------*\
 * Defined in AmbaLCD_SSD2828.c
\*-----------------------------------------------------------------------------------------------*/

#endif /* _AMBA_LCD_SSD2828_H_ */
