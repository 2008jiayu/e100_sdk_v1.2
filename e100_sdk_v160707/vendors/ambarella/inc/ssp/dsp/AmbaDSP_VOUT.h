/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaDSP_VOUT.h
 *
 *  @Copyright      :: Copyright (C) 2012 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Definitions & Constants for DSP VOUT APIs
 *
\*-------------------------------------------------------------------------------------------------------------------*/

#ifndef _AMBA_DSP_VOUT_H_
#define _AMBA_DSP_VOUT_H_

#include "AmbaDSP.h"
#include "AmbaDSP_VoutDisplayDef.h"
#include "AmbaCVBS_Def.h"

typedef struct _AMBA_DSP_VOUT_CSC_CONFIG_s_ {
    struct {
        float   Coefficient[3];     /* Coefficients for single component */
        float   Constant;           /* Constant for single component */
        UINT16  LowerBound;         /* Lower bound of the clamping range */
        UINT16  UpperBound;         /* Upper bound of the clamping range */
    } Component[3];
} AMBA_DSP_VOUT_CSC_CONFIG_s;

/*-----------------------------------------------------------------------------------------------*\
 * Defined in AmbaDSP_VOUT.c
\*-----------------------------------------------------------------------------------------------*/
int AmbaDSP_VoutReset(AMBA_DSP_VOUT_IDX_e VoutIdx);

/*---------------------------------------------------------------------------*\
 * Mixer Layer Setup
\*---------------------------------------------------------------------------*/
int AmbaDSP_VoutMixerSetup(AMBA_DSP_VOUT_IDX_e VoutIdx, AMBA_DSP_VOUT_MIXER_CONFIG_s *pConfig);
int AmbaDSP_VoutMixerSetBackColor(AMBA_DSP_VOUT_IDX_e VoutIdx, UINT32 BackColorYUV);
int AmbaDSP_VoutMixerSetHighlight(AMBA_DSP_VOUT_IDX_e VoutIdx, UINT8 LumaThreshold, UINT32 HighlightColorYUV);
int AmbaDSP_VoutMixerSetCscMatrix(AMBA_DSP_VOUT_IDX_e VoutIdx,
                                  AMBA_DSP_VOUT_MIXER_CSC_CTRL_e CscCtrl,
                                  AMBA_DSP_VOUT_CSC_CONFIG_s *pCscConfig);

/*---------------------------------------------------------------------------*\
 * Video Layer Setup
\*---------------------------------------------------------------------------*/
typedef enum _AMBA_DSP_VOUT_VIDEO_SRC_e_ {
    AMBA_DSP_VOUT_VIDEO_SRC_DEFAULT_IMG = 0,    /* Default Image */
    AMBA_DSP_VOUT_VIDEO_SRC_BACK_COLOR,         /* Mixer Background Color */

    AMBA_DSP_VOUT_VIDEO_SRC_INTERNAL = 0x88     /* Internal Image Pipeline */
} AMBA_DSP_VOUT_VIDEO_SRC_e;

int AmbaDSP_VoutVideoCtrl(AMBA_DSP_VOUT_IDX_e VoutIdx, int Enable);
int AmbaDSP_VoutVideoSourceSel(AMBA_DSP_VOUT_IDX_e VoutIdx, int VideoSource);
int AmbaDSP_VoutVideoConfig(AMBA_DSP_VOUT_IDX_e VoutIdx, AMBA_DSP_ROTATE_FLIP_e RotateFlip);
int AmbaDSP_VoutVideoWindowSetup(AMBA_DSP_VOUT_IDX_e VoutIdx, AMBA_DSP_WINDOW_s *pWindow);
int AmbaDSP_VoutVideoGetState(AMBA_DSP_VOUT_IDX_e VoutIdx);
UINT8 AmbaDSP_VoutVideoGetSource(AMBA_DSP_VOUT_IDX_e VoutIdx);

/*---------------------------------------------------------------------------*\
 * Default Image Setup
\*---------------------------------------------------------------------------*/
int AmbaDSP_VoutDefaultImgSetup(AMBA_DSP_VOUT_IDX_e VoutIdx, AMBA_DSP_VOUT_DEFAULT_IMG_CONFIG_s *pDefaultImgConfig);

/*---------------------------------------------------------------------------*\
 * OSD Layer Setup
\*---------------------------------------------------------------------------*/
typedef struct _AMBA_DSP_VOUT_OSD_SCALER_s_ {
    UINT16  OffsetX;                /* Horizontal offset of output OSD window related to the active window */
    UINT16  OffsetY;                /* Vertical offset of output OSD window related to the active window */
    UINT16  Width;                  /* Number of pixels per line in the output OSD window */
    UINT16  Height;                 /* Number of lines in the output OSD window */
    UINT16  ScalerInputWidth;       /* OSD Scaler input width */
    UINT16  ScalerInputHeight;      /* OSD Scaler input height */
} AMBA_DSP_VOUT_OSD_SCALER_s;

int AmbaDSP_VoutOsdCtrl(AMBA_DSP_VOUT_IDX_e VoutIdx, int Enable);
int AmbaDSP_VoutOsdBufSetup(AMBA_DSP_VOUT_IDX_e VoutIdx, AMBA_DSP_VOUT_OSD_BUF_CONFIG_s *pBufConfig);
int AmbaDSP_VoutOsdWindowSetup(AMBA_DSP_VOUT_IDX_e VoutIdx, AMBA_DSP_WINDOW_s *pOsdWindow);
int AmbaDSP_VoutOsdScalerSetup(AMBA_DSP_VOUT_IDX_e VoutIdx, AMBA_DSP_VOUT_OSD_SCALER_s *pScalerCfg);
int AmbaDSP_VoutOsdSetDataFormat(AMBA_DSP_VOUT_IDX_e VoutIdx, AMBA_DSP_VOUT_OSD_DATA_FORMAT_e DataFormat);
int AmbaDSP_VoutOsdSetTransparent(AMBA_DSP_VOUT_IDX_e VoutIdx, int Enable, UINT16 Color);
int AmbaDSP_VoutOsdSetBlend(AMBA_DSP_VOUT_IDX_e VoutIdx, UINT8 GlobalBlend);
int AmbaDSP_VoutOsdSetCLUT(AMBA_DSP_VOUT_IDX_e VoutIdx, UINT32 *pCLUT);
int AmbaDSP_VoutOsdSwapByteCtrl(AMBA_DSP_VOUT_IDX_e VoutIdx, int Enable);
int AmbaDSP_VoutOsdGetState(AMBA_DSP_VOUT_IDX_e VoutIdx);

/*---------------------------------------------------------------------------*\
 * Display Setup
\*---------------------------------------------------------------------------*/
typedef struct _AMBA_DSP_VOUT_DISPLAY_TIMING_CONFIG_s_ {
    UINT32  PixelClock;                                     /* pixel clock in Hz */
    UINT16  FrameWidth;                                     /* Total Number of clock cycles per line */
    UINT16  FrameHeight;                                    /* Total Number of lines per frame */
    UINT16  FrameActiveColStart;                            /* Start column of active region */
    UINT16  FrameActiveColWidth;                            /* Column width of active region */
    UINT16  FrameActiveRowStart;                            /* Start row of active region */
    UINT16  FrameActiveRowHeight;                           /* Row height of active region */
    UINT8   Rotation;                                       /* Clockwise rotation degree */
    UINT8   Interlace;                                      /* 0 = Progressive scan, 1 = Interlaced scan */
    UINT16  VinVoutSyncDelay;                               /* Delay in unit of 0.1ms that vout should be spaced out with vin*/
} AMBA_DSP_VOUT_DISPLAY_TIMING_CONFIG_s;

typedef struct _AMBA_DSP_VOUT_DISPLAY_DIGITAL_CONFIG_s_ {
    AMBA_DSP_VOUT_DISPLAY_SYNC_CTRL_s   SyncCtrl;
    AMBA_DSP_VOUT_DIGITAL_OUTPUT_MODE_e OutputMode;         /* Digital Output Mode */
    AMBA_DSP_VOUT_PIXEL_CLOCK_OUTPUT_e  DeviceClock;        /* Source clock frequency of LCD device */
    AMBA_DSP_VOUT_LCD_COLOR_ORDER_e     EvenLineColor;      /* RGB color sequence of even lines */
    AMBA_DSP_VOUT_LCD_COLOR_ORDER_e     OddLineColor;       /* RGB color sequence of odd lines */
    AMBA_DSP_VOUT_SIGNAL_POLARITY_e     ClkSampleEdge;      /* Sample clock edge */
    AMBA_DSP_VOUT_SIGNAL_POLARITY_e     LineSyncPolarity;   /* Line sync signal polarity */
    AMBA_DSP_VOUT_SIGNAL_POLARITY_e     FrameSyncPolarity;  /* Frame sync signal polarity */
    AMBA_DSP_VOUT_SIGNAL_POLARITY_e     LineValidPolarity;  /* Line valid signal polarity */
    AMBA_DSP_VOUT_SIGNAL_POLARITY_e     FrameValidPolarity; /* Frame sync signal polarity */
    UINT8   Interlace;                                      /* 0 = Progressive scan, 1 = Interlaced scan */
    UINT16  RowTime;                                        /* Row time in clock cycles */
    UINT16  Bt656VBitStart;                                 /* Defines Start Row of V-Bit Assertion */
    UINT16  Bt656VBitEnd;                                   /* Defines End Row of V-Bit Assertion */
    UINT16  Bt656SavStart;                                  /* Defines Start Location of SAV Code */
} AMBA_DSP_VOUT_DISPLAY_DIGITAL_CONFIG_s;

typedef struct _AMBA_DSP_VOUT_DISPLAY_HDMI_CONFIG_s_ {
    AMBA_DSP_VOUT_DISPLAY_SYNC_CTRL_s   SyncCtrl;
    AMBA_DSP_VOUT_HDMI_OUTPUT_MODE_e    OutputMode;         /* HDMI Output Mode Register */
    AMBA_DSP_VOUT_SIGNAL_POLARITY_e     LineSyncPolarity;   /* Line sync signal polarity */
    AMBA_DSP_VOUT_SIGNAL_POLARITY_e     FrameSyncPolarity;  /* Frame sync signal polarity */
    UINT8   Interlace;                                      /* 0 = Progressive scan, 1 = Interlaced scan */
    UINT16  RowTime;                                        /* Row time in clock cycles */
} AMBA_DSP_VOUT_DISPLAY_HDMI_CONFIG_s;

/*-----------------------------------------------------------------------------------------------*\
 * Defined in AmbaDSP_Main.c
\*-----------------------------------------------------------------------------------------------*/
typedef enum _AMBA_DSP_VOUT_FRAME_RATE_e_ {
    VOUT_FRAME_RATE_29_97 = 0,          /* non-integral frame rate */
    VOUT_FRAME_RATE_59_94,
} AMBA_DSP_VOUT_FRAME_RATE_e;

typedef enum _AMBA_DSP_VOUT_FLIP_e_ {
    VOUT_NO_FLIP = 0,
    VOUT_FLIP_VERTICALLY_AND_HORIZONTALLY,
    VOUT_FLIP_HORIZONTALLY,
    VOUT_FLIP_VERTICALLY
} AMBA_DSP_VOUT_FLIP_e;

typedef enum _AMBA_DSP_VOUT_ROTATE_e_ {
    VOUT_NO_ROTATE = 0,
    VOUT_ROTATE_90_DEGREE
} AMBA_DSP_VOUT_ROTATE_e;

typedef enum _AMBA_DSP_IMG_FLIP_e_ {
    IMG_NO_FLIP = 0,
    IMG_FLIP_HORIZONTALLY,
    IMG_FLIP_VERTICALLY
} AMBA_DSP_IMG_FLIP_e;

typedef enum _AMBA_DSP_IMG_ROTATE_e_ {
    IMG_NO_ROTATE = 0,
    IMG_ROTATE_90_DEGREE,
    IMG_ROTATE_180_DEGREE,
    IMG_ROTATE_270_DEGREE,
} AMBA_DSP_IMG_ROTATE_e;

int AmbaDSP_VoutRotateFlipMap(AMBA_DSP_ROTATE_FLIP_e RotateFlip, UINT8 *pRotate, UINT8 *pFlip);

typedef struct _AMBA_DSP_VOUT_DISPLAY_CVBS_CONFIG_s_ {
    UINT8   BlankLevel;                                     /* Blank level adjust */
    UINT8   SyncLevel;                                      /* Programmable Sync Level */
    UINT8   BlackLevel;                                     /* Black level adjust */
    UINT8   ClampLevel;                                     /* Used to clamp incoming black level to 0 */
    UINT16  Gain;                                           /* Gain value for output */
    UINT16  Offset;                                         /* Offset value for output */
    UINT16  ClampLower;                                     /* Lower bound of output value */
    UINT16  ClampUpper;                                     /* Upper bound of output value */
} AMBA_DSP_VOUT_DISPLAY_CVBS_CONFIG_s;

int AmbaDSP_VoutDisplayDigitalSetColorSequence(AMBA_DSP_VOUT_LCD_COLOR_ORDER_e EvenLineColor,
                                               AMBA_DSP_VOUT_LCD_COLOR_ORDER_e OddLineColor);

int AmbaDSP_VoutDisplayDigitalSetGamma(UINT8 (*pGammaTable)[AMBA_DSP_VOUT_DIGITAL_GAMMA_SIZE]);
int AmbaDSP_VoutDisplayDigitalGammaCtrl(int Enable);

int AmbaDSP_VoutDisplayTimingSetup(AMBA_DSP_VOUT_IDX_e VoutIdx, AMBA_DSP_VOUT_DISPLAY_TIMING_CONFIG_s *pDisplayTimingConfig);
int AmbaDSP_VoutDisplaySetVinVoutSync(AMBA_DSP_VOUT_IDX_e VoutIdx, int EnableFlag);
int AmbaDSP_VoutDisplaySetCscMatrix(AMBA_DSP_VOUT_TYPE_e VoutType, AMBA_DSP_VOUT_CSC_CONFIG_s *pCscConfig);
extern int AmbaDSP_VoutDisplayConfigCscMatrix(AMBA_DSP_VOUT_TYPE_e VoutType, AMBA_DSP_VOUT_CSC_CONFIG_s * pCscConfig, AMBA_DSP_VOUT_HDMI_OUTPUT_MODE_e pxlFmt);


int AmbaDSP_VoutDisplayDigitalSetup(AMBA_DSP_VOUT_DISPLAY_DIGITAL_CONFIG_s *pDisplayDigitalConfig);
int AmbaDSP_VoutDisplayHdmiSetup(AMBA_DSP_VOUT_DISPLAY_HDMI_CONFIG_s *pDisplayHdmiConfig);
int AmbaDSP_VoutDisplayCvbsSetup(AMBA_CVBS_TV_SYSTEM_e TvSystem);

int AmbaDSP_VoutDisplaySetDualVoutDelay(AMBA_DSP_VOUT_IDX_e VoutIdx, UINT32 DelayInMsX10);  /* unit = 0.1ms */

int AmbaDSP_VoutDisplaGetVout0SeamlessBufferPitchAlign(UINT32 *PitchAlign);

int AmbaDSP_VoutWaitUpdate(void);

#endif  /* _AMBA_DSP_VOUT_H_ */
