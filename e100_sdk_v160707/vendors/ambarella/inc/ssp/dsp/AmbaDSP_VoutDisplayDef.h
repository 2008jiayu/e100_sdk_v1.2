/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaDSP_VoutDisplayDef.h
 *
 *  @Copyright      :: Copyright (C) 2012 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Definitions & Constants for DSP Display Control
 *
\*-------------------------------------------------------------------------------------------------------------------*/

#ifndef _AMBA_DSP_VOUT_DISPLAY_DEF_H_
#define _AMBA_DSP_VOUT_DISPLAY_DEF_H_

typedef enum _AMBA_DSP_VOUT_ROTATE_TYPE_e_ {
    AMBA_DSP_VOUT_ROTATE_NORMAL = 0,                    /* Normal */
    AMBA_DSP_VOUT_ROTATE_MIRROR_HORIZONTAL,             /* Mirroring horizontally */
    AMBA_DSP_VOUT_ROTATE_FLIP_VERTICAL,                 /* Flipping vertically */
    AMBA_DSP_VOUT_ROTATE_180_DEGREE                     /* 180 degree rotation */
} AMBA_DSP_VOUT_ROTATE_TYPE_e;

typedef enum _AMBA_DSP_VOUT_CSC_MATRIX_e_ {
    AMBA_DSP_VOUT_CSC_IDENTITY,                         /* No color space conversion */
    AMBA_DSP_VOUT_CSC_YC601_YC709,                      /* YCbCr601 -> YCbCr709 */
    AMBA_DSP_VOUT_CSC_YC709_YC601,                      /* YCbCr709 -> YCbCr601 */
    AMBA_DSP_VOUT_CSC_LIMITED_YC601_RGB,                /* YCbCr [16-235] to RGB (BT.601) */
    AMBA_DSP_VOUT_CSC_LIMITED_YC709_RGB,                /* YCbCr [16-235] to RGB (BT.709) */
    AMBA_DSP_VOUT_CSC_FULL_YC601_RGB,                   /* YCbCr [0-255] to RGB (BT.601) */
    AMBA_DSP_VOUT_CSC_FULL_YC709_RGB                    /* YCbCr [0-255] to RGB (BT.709) */
} AMBA_DSP_VOUT_CSC_MATRIX_e;

typedef enum _AMBA_DSP_VOUT_COLOR_RANGE_e_ {
    AMBA_DSP_VOUT_COLOR_RANGE_0_255,                    /* Y/U/V = [0~ 255] */
    AMBA_DSP_VOUT_COLOR_RANGE_1_254,                    /* Y/U/V = [1~ 254] */
    AMBA_DSP_VOUT_COLOR_RANGE_16_235                    /* Y = [16~235],  U/V = [16~240] */
} AMBA_DSP_VOUT_COLOR_RANGE_e;

typedef enum _AMBA_DSP_VOUT_PIXEL_CLOCK_OUTPUT_e_ {
    AMBA_DSP_VOUT_PIXEL_CLOCK_NONE = 0,                 /* no pclk output */
    AMBA_DSP_VOUT_PIXEL_CLOCK_FULL_DCLK,                /* same as dclk frequency */
    AMBA_DSP_VOUT_PIXEL_CLOCK_HALF_DCLK,                /* half of dclk frequency */
    AMBA_DSP_VOUT_PIXEL_CLOCK_QUARTER_DCLK              /* quarter of dclk frequency */
} AMBA_DSP_VOUT_PIXEL_CLOCK_OUTPUT_e;

typedef enum _AMBA_DSP_VOUT_DIGITAL_OUTPUT_MODE_e_ {
    AMBA_DSP_VOUT_LCD_1COLOR_PER_DOT = 0,               /* LCD - (8-bit) Single color per pixel */
    AMBA_DSP_VOUT_LCD_3COLORS_PER_DOT,                  /* LCD - (8-bit) 3 colors per pixel, no dummy clock */
    AMBA_DSP_VOUT_LCD_3COLORS_DUMMY_PER_DOT,            /* LCD - (8-bit) 3 colors per pixel, dummy clock */
    AMBA_DSP_VOUT_LCD_RGB565,                           /* LCD - (16-bit) RGB 5:6:5 */
    AMBA_DSP_VOUT_BT656,                                /* BT.656 (8-bit) */
    AMBA_DSP_VOUT_BT601_16B,                            /* BT.601 (16-bit, YCbCr 4:2:2) */
    AMBA_DSP_VOUT_BT601_24B,                            /* BT.601 (24-bit, YCbCr 4:4:4 or RGB) */
    AMBA_DSP_VOUT_BT601_8B,                             /* BT.601 (8-bit, in Cb-Y-Cr-Y order) */
    AMBA_DSP_VOUT_CFA_BAYER                             /* Bayer CFA (8-bit) */
} AMBA_DSP_VOUT_DIGITAL_OUTPUT_MODE_e;

typedef enum _AMBA_DSP_VOUT_HDMI_OUTPUT_MODE_e_ {
    AMBA_DSP_VOUT_HDMI_YCC444_8B = 0,                   /* HDMI - YCbCr 4:4:4 8 bits per channel */
    AMBA_DSP_VOUT_HDMI_RGB444_8B,                       /* HDMI - RGB 4:4:4 8 bits per channel */
    AMBA_DSP_VOUT_HDMI_YCC422_12B                       /* HDMI - YCbCr 4:2:2 12 bits per channel */
} AMBA_DSP_VOUT_HDMI_OUTPUT_MODE_e;

typedef enum _AMBA_DSP_VOUT_LCD_COLOR_ORDER_e_ {
    AMBA_DSP_VOUT_LCD_COLOR_RGB = 0,                    /* R-G-B */
    AMBA_DSP_VOUT_LCD_COLOR_BGR,                        /* B-G-R (reversed R-G-B) */
    AMBA_DSP_VOUT_LCD_COLOR_GBR,                        /* G-B-R */
    AMBA_DSP_VOUT_LCD_COLOR_RBG,                        /* R-B-G (reversed G-B-R) */
    AMBA_DSP_VOUT_LCD_COLOR_BRG,                        /* B-R-G */
    AMBA_DSP_VOUT_LCD_COLOR_GRB                         /* G-R-B (reversed B-R-G) */
} AMBA_DSP_VOUT_LCD_COLOR_ORDER_e;

typedef enum _AMBA_DSP_VOUT_CFA_BAYER_COLOR_e_ {
    AMBA_DSP_VOUT_CFA_BAYER_RG = 0,                     /* Bayer CFA - RG */
    AMBA_DSP_VOUT_CFA_BAYER_GR,                         /* Bayer CFA - GR */
    AMBA_DSP_VOUT_CFA_BAYER_GB,                         /* Bayer CFA - GB */
    AMBA_DSP_VOUT_CFA_BAYER_BG                          /* Bayer CFA - BG */
} AMBA_DSP_VOUT_CFA_BAYER_COLOR_e;

typedef enum _AMBA_DSP_VOUT_SIGNAL_POLARITY_e_ {
    AMBA_DSP_VOUT_ACTIVE_HIGH,                          /* Positive pulse or rising edge */
    AMBA_DSP_VOUT_ACTIVE_LOW                            /* Negative pulse or falling edge */
} AMBA_DSP_VOUT_SIGNAL_POLARITY_e;

/*-----------------------------------------------------------------------------------------------*\
 * VOUT Configuration Structure
\*-----------------------------------------------------------------------------------------------*/
typedef struct _AMBA_DSP_VOUT_DISPLAY_SYNC_CTRL_s_ {
    UINT16  HSyncColStart;                              /* Start column of Hsync pulse */
    UINT16  HSyncColEnd;                                /* End column of Hsync pluse */
    UINT16  VSyncColStart;                              /* Start column of Vsync pulse */
    UINT16  VSyncColEnd;                                /* End column of Vsync pulse */
    UINT16  VSyncRowStart;                              /* Start row of Vsync pulse */
    UINT16  VSyncRowEnd;                                /* End row of Vsync pulse */
} AMBA_DSP_VOUT_DISPLAY_SYNC_CTRL_s;

typedef struct _AMBA_DSP_VOUT_DISPLAY_CVBS_VBI_s_ {
    UINT8   VbiRepeatCount;                             /* Output VBI bit for N clock cycle */
    UINT16  VbiLevelOne;                                /* Value to output when VBI bit is 1 */
    UINT16  VbiLevelZero;                               /* Value to output when VBI bit is 0 */
    UINT16  VbiField0StartRow;                          /* Start row of VBI insertion on field 0 */
    UINT16  VbiField1StartRow;                          /* Start row of VBI insertion on field 1 */
    UINT16  VbiStartColumn;                             /* Start column of VBI insertion */
    UINT16  VbiEndColumn;                               /* End column of VBI insertion */
    UINT32  VbiData[12];                                /* Bit patterns to be output as VBI data */
} AMBA_DSP_VOUT_DISPLAY_CVBS_VBI_s;

#endif  /* _AMBA_DSP_VOUT_DISPLAY_DEF_H_ */
