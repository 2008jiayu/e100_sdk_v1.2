/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaLCD.h
 *
 *  @Copyright      :: Copyright (C) 2013 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Definitions & Constants for LCD Driver
\*-------------------------------------------------------------------------------------------------------------------*/

#ifndef _AMBA_LCD_H_
#define _AMBA_LCD_H_

#include "AmbaDSP.h"

/*---------------------------------------------------------------------------*\
 * DSP common definition
\*---------------------------------------------------------------------------*/
typedef union _AMBA_LCD_MODE_ID_u_ {
    UINT8   Data;                       /* this is an 8-bit data */

    struct {
        UINT8   Mode:           5;      /* LCD input mode */
        UINT8   FlipHorizontal: 1;      /* Flip the display horizontally */
        UINT8   FlipVertical:   1;      /* Flip the display vertically */
        UINT8   Stereoscopic:   1;      /* 3D display */
    } Bits;
} AMBA_LCD_MODE_ID_u;

/*-----------------------------------------------------------------------------------------------*\
 * This structure is used to provide necessary configuration for LCD color balance
\*-----------------------------------------------------------------------------------------------*/
typedef struct _AMBA_LCD_COLOR_BALANCE_s_ {
    INT16   OffsetRed;
    INT16   OffsetGreen;
    INT16   OffsetBlue;
    float   GainRed;
    float   GainGreen;
    float   GainBlue;
} AMBA_LCD_COLOR_BALANCE_s;

/*-----------------------------------------------------------------------------------------------*\
 * This structure is used to provide necessary configuration for LCD input video timing
\*-----------------------------------------------------------------------------------------------*/
typedef struct _AMBA_LCD_TIMING_s_ {
    UINT32  PixelClock;             /* Pixel clock frequency */
    UINT8   PixelRepetition;        /* Pixel repetition factor */

    UINT16  Htotal;                 /* Number of columns per row */
    UINT16  Vtotal;                 /* Number of rows per field */

    UINT16  HsyncColStart;          /* Start column of Hsync pulse */
    UINT16  HsyncColEnd;            /* End column of Hsync pluse */

    UINT16  VsyncColStart;          /* Start column of Vsync pulse */
    UINT16  VsyncColEnd;            /* End column of Vsync pulse */
    UINT16  VsyncRowStart;          /* Start row of Vsync pulse */
    UINT16  VsyncRowEnd;            /* End row of Vsync pulse */

    UINT16  ActiveColStart;         /* Start column of active region */
    UINT16  ActiveColWidth;         /* End column of active region */
    UINT16  ActiveRowStart;         /* Start row of active region */
    UINT16  ActiveRowHeight;        /* End row of active region */
} AMBA_LCD_TIMING_s;

typedef struct _AMBA_LCD_ASPECT_RATIO_s_ {
    UINT8   Y;  /* Low byte is height */
    UINT8   X;  /* High byte is width */
} AMBA_LCD_ASPECT_RATIO_s;

/*-----------------------------------------------------------------------------------------------*\
 * This structure is used to provide information of the current LCD configuration
\*-----------------------------------------------------------------------------------------------*/
typedef struct _AMBA_LCD_INFO_s_ {
    UINT16  Width;          /* Horizontal display resolution of LCD panel */
    UINT16  Height;         /* Vertical display resolution of LCD panel */
    AMBA_DSP_FRAME_RATE_s   FrameRate;      /* Frame rate of the current LCD configuration */
    AMBA_LCD_ASPECT_RATIO_s AspectRatio;    /* Aspect ratio of the LCD panel display */
} AMBA_LCD_INFO_s;

/*-----------------------------------------------------------------------------------------------*\
 * LCD Driver Object
\*-----------------------------------------------------------------------------------------------*/
typedef struct _AMBA_LCD_OBJECT_s_ {
    char    *pName;          /* Name of the LCD panel */

    int     (*LcdEnable)(void);
    int     (*LcdDisable)(void);
    int     (*LcdGetInfo)(AMBA_LCD_INFO_s *pLcdInfo);
    int     (*LcdConfig)(AMBA_LCD_MODE_ID_u Mode);
    int     (*LcdSetBacklight)(INT32 EnableFlag);
    int     (*LcdSetBrightness)(INT32 Offset);
    int     (*LcdSetContrast)(float Contrast);
    int     (*LcdSetColorBalance)(AMBA_LCD_COLOR_BALANCE_s *pColorBalance);
} AMBA_LCD_OBJECT_s;

/*-----------------------------------------------------------------------------------------------*\
 * Defined in AmbaLCD.c
\*-----------------------------------------------------------------------------------------------*/
extern AMBA_LCD_OBJECT_s *pAmbaLcdObj[];

#define AmbaLCD_Hook(Chan,pLcdObj)                      pAmbaLcdObj[Chan] = pLcdObj

#define AmbaLCD_Enable(Chan)                            ((pAmbaLcdObj[Chan]->LcdEnable == NULL) ? NG : pAmbaLcdObj[Chan]->LcdEnable())
#define AmbaLCD_Disable(Chan)                           ((pAmbaLcdObj[Chan]->LcdDisable == NULL) ? NG : pAmbaLcdObj[Chan]->LcdDisable())
#define AmbaLCD_GetInfo(Chan,pLcdInfo)                  ((pAmbaLcdObj[Chan]->LcdGetInfo == NULL) ? NG : pAmbaLcdObj[Chan]->LcdGetInfo(pLcdInfo))
#define AmbaLCD_Config(Chan,Mode)                       ((pAmbaLcdObj[Chan]->LcdConfig == NULL) ? NG : pAmbaLcdObj[Chan]->LcdConfig(Mode))
#define AmbaLCD_SetBacklight(Chan,EnableFlag)           ((pAmbaLcdObj[Chan]->LcdSetBacklight == NULL) ? NG : pAmbaLcdObj[Chan]->LcdSetBacklight(EnableFlag))
#define AmbaLCD_SetBrightness(Chan,Offset)              ((pAmbaLcdObj[Chan]->LcdSetBrightness == NULL) ? NG : pAmbaLcdObj[Chan]->LcdSetBrightness(Offset))
#define AmbaLCD_SetContrast(Chan,Contrast)              ((pAmbaLcdObj[Chan]->LcdSetContrast == NULL) ? NG : pAmbaLcdObj[Chan]->LcdSetContrast(Contrast))
#define AmbaLCD_SetColorBalance(Chan,pColorBalance)     ((pAmbaLcdObj[Chan]->LcdSetColorBalance == NULL) ? NG : pAmbaLcdObj[Chan]->LcdSetColorBalance(pColorBalance))

#endif  /* _AMBA_LCD_H_ */
