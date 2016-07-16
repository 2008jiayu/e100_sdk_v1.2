/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaDSP_Liveview.h
 *
 *  @Copyright      :: Copyright (C) 2012 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Definitions & Constants for Ambarella DSP Liveview APIs
 *
\*-------------------------------------------------------------------------------------------------------------------*/

#ifndef _AMBA_DSP_LIVEVIEW_H_
#define _AMBA_DSP_LIVEVIEW_H_

#include "AmbaDSP.h"

typedef union _AMBA_DSP_LIVEVIEW_CTRL_FLAG_u_ {
    UINT16   Data;

    struct {
        UINT16  VideoProcMode:          1;      /* 0 - ExpressMode, 1 - HybridMode */
        UINT16  VideoAlgoMode:          2;      /* 0 - FastMode, 1 - LISO, 2 - HISO, only Valid in HybridMode */
        UINT16  VideoOSMode:            1;      /* 1 - OverSamplingMode, only Valid in ExpressMode */
        UINT16  EnableVideoTuningMode:  1;      /* 0 - Disable tuning mode; 1 - Enable turing mode */
        UINT16  LiveViewOnly:           1;      /* LiveView only and not h264 encode */
        UINT16  MultiChannelProc:       1;      /* 1 - enable multiple channel process; 0 - disable */
        UINT16  VideoHdrMode:           1;      /* 1 - enable videoHDR, currently only support in HybridMode */
        UINT16  VideoOBMode:            1;      /* 1 - enable VideoOB, currently only support in ExpressMode */
        UINT16  VinSelect:             2;      /* 0: default main only , 1: pip only, 2: both enabled */
        UINT16  Reserved:               5;      /* Reserved */
    } Bits;
} AMBA_DSP_LIVEVIEW_CTRL_FLAG_u;

typedef struct _AMBA_DSP_LIVEVIEW_EXT_BUF_s_ {
    UINT16                  MaxNumPicture;      /* maximum number of pictures: = buffer capacity */
    AMBA_DSP_RAW_BUF_s      *pRawBufAddr;       /* pointer to Raw buffers */
    AMBA_DSP_YUV_IMG_BUF_s  *pYuvBufAddr;       /* pointer to YUV buffers */
    UINT8                   RawBayerPattern;    /* Raw file bayer pattern */
    UINT8                   RawDataBits;        /* Raw file bits resolution */
} AMBA_DSP_LIVEVIEW_EXT_BUF_s;

typedef struct _AMBA_DSP_RAW_CAP_INFO_s_ {
    UINT32  CapAddr;                            /* DSP raw capture DRAM address */
    UINT32  Pitch;                              /* DRAM pitch */
    UINT16  FrameSyncCount;                     /* Frame sync count */
    UINT16  CapDreg;                            /* Capture DREG */
    UINT32  CapCount;                           /* Capture count */
} AMBA_DSP_RAW_CAP_INFO_s;

typedef struct _AMBA_DSP_LIVEVIEW_CONFIG_s_ {
    UINT8                   VinID;              /* Input VIN ID */
    UINT8                   ViewZoneID;         /* Given the raw window an View Zone ID */
    AMBA_DSP_WINDOW_s       RawInputWindow;     /* Input raw window from indicated VIN */

    AMBA_DSP_FRAME_RATE_s   FrameRate;          /* frame rate */
    UINT16                  SecFrameRateDivisor;/* 2nd frame rate divisor */

    UINT16  MainYuvWidth;                       /* main YUV width */
    UINT16  MainYuvHeight;                      /* main YUV height */
    UINT16  SecYuvWidth;                        /* 2nd YUV width */
    UINT16  SecYuvHeight;                       /* 2nd YUV height */

    AMBA_DSP_LIVEVIEW_EXT_BUF_s *pExtBuf;       /* pointer to External Image Buffers: NULL - none */
    AMBA_DSP_LIVEVIEW_EXT_BUF_s *pExtBuf2FOV;   /* pointer to External Image Buffers for 2-FOV support: NULL - none */

    UINT8   BlackbarPaddedMBSize;               /* Used when encode rotate */
    AMBA_DSP_ROTATE_FLIP_e  RotateFlip;         /* encode rotate/flip for this channel */
    UINT8   MainviewReportRate;                 /* mainview status report rate */

    AMBA_DSP_RAW_CAP_INFO_s   *pRawCapDramInfo; /* Pointer to a user provided dram structure, and DSP fill the raw capture information. */
} AMBA_DSP_LIVEVIEW_CONFIG_s;

typedef struct _AMBA_DSP_LIVEVIEW_CHANNEL_WINDOW_s_ {
    UINT8                   ViewZoneID;         /* source View Zone ID */
    AMBA_DSP_WINDOW_s       Window;             /* display window on VOUT plane for this channel */
    AMBA_DSP_ROTATE_FLIP_e  RotateFlip;         /* display rotate/flip for this channel */
    UINT8                   *pLumaAlphaTable;   /* pointer to luma alpha table for RotateFlip = AMBA_DSP_ROTATE_90, AMBA_DSP_ROTATE_90_VERT_FLIP,
                                                   AMBA_DSP_ROTATE_270, and AMBA_DSP_ROTATE_270_VERT_FLIP */
} AMBA_DSP_LIVEVIEW_CHANNEL_WINDOW_s;

typedef struct _AMBA_DSP_LIVEVIEW_DISP_CONFIG_s_ {
    AMBA_DSP_FRAME_RATE_s   FrameRate;          /* frame rate */
    UINT16                  DispWidth;          /* overall display width on vout */
    UINT16                  DispHeight;         /* overall display height on vout */
    UINT8                   MultiChannel;       /* 1 - enable multi channel display; 0 - disable */
    int                     NumChannel;         /* number of channels */
    AMBA_DSP_LIVEVIEW_CHANNEL_WINDOW_s  *pChanCfg;  /* pointer to channel configuration */
} AMBA_DSP_LIVEVIEW_DISP_CONFIG_s;

typedef struct _AMBA_DSP_LIVEVIEW_STREAM_CONFIG_s_ {
    UINT16  StreamID;                           /* Stream ID */
    UINT16  Width;                              /* overall stream width */
    UINT16  Height;                             /* overall stream height */
    int     NumChan;                            /* how many channel in this stream */
    AMBA_DSP_LIVEVIEW_CHANNEL_WINDOW_s *pChanCfg;   /* pointer to channel configuration */
} AMBA_DSP_LIVEVIEW_STREAM_CONFIG_s;

typedef struct _AMBA_DSP_LIVEVIEW_CHANNEL_CTRL_s_ {
    UINT8               Enable;                 /* 1 - enable, 0 - disable, NOT supported yet */
    UINT8               ViewZoneID;             /* View Zone ID */
    AMBA_DSP_WINDOW_s   RawInputWindow;         /* Input raw window */
    UINT8               EnableVerticalFlip;     /* 1 - enable, 0 - disable */
} AMBA_DSP_LIVEVIEW_CHANNEL_CTRL_s;

typedef struct _AMBA_EIS_COEFF_INFO_s_ {
    UINT32  ActualLeftTopX;
    UINT32  ActualLeftTopY;
    UINT32  ActualRightBotX;
    UINT32  ActualRightBotY;
    INT32   HotiSkewPhaseInc;
    UINT32  ZoomY;
    UINT16  DummyWindowXLeft;
    UINT16  DummyWindowYTop;
    UINT16  DummyWindowWidth;
    UINT16  DummyWindowHeight;
    UINT32  ArmSysTime;
    UINT32  WarpControl;
    UINT8   GridArrayWidth;
    UINT8   GridArrayHeight;
    UINT8   Horz_Grid_Spacing_Exponent;
    UINT8   Vert_Grid_Spacing_Exponent;
    UINT32  WarpHorizontalTableAddress;
    UINT32  WarpVerticalTableAddress;
    UINT8   VertWarpHorzGridSpacingExponent;
    UINT8   VertWarpVertGridSpacingExponent;
    UINT8   VertWarpEnable;
    UINT8   VertWarpGridArrayWidth;
    UINT8   VertWarpGridArrayHeight;
} AMBA_EIS_COEFF_INFO_s;

typedef struct _AMBA_EIS_UPDATE_INFO_s_ {
    UINT32  IsIdspConfig;
    UINT32  Sec2HoriOutLumaAddr1;
    UINT32  Sec2HoriOutLumaAddr2;
    UINT32  Sec2HoriOutChromaAddr1;
    UINT32  Sec2HoriOutChromaAddr2;
    UINT32  Sec2VertOutLumaAddr1;
    UINT32  Sec2VertOutLumaAddr2;
    UINT32  FlgPivStatus;
    UINT32  ArmSysTime;
} AMBA_EIS_UPDATE_INFO_s;

typedef struct _AMBA_DSP_LIVEVIEW_ISO_CONFIG_CTRL_s_ {
    UINT8               ViewZoneID;             /* View Zone ID */
    UINT32              CtxIndex;               /* Indicate which CtxIndex is updated */
    UINT32              CfgIndex;               /* Indicate which CfgIndex is updated */
    UINT32              HdrCfgIndex;            /* Indicate which HdrCfgIndex is updated */
} AMBA_DSP_LIVEVIEW_ISO_CONFIG_CTRL_s;

/*-----------------------------------------------------------------------------------------------*\
 * Defined in AmbaDSP_Liveview.c
\*-----------------------------------------------------------------------------------------------*/
int AmbaDSP_LiveviewConfig(AMBA_DSP_LIVEVIEW_CTRL_FLAG_u CtrlFlag, int NumChannel, AMBA_DSP_LIVEVIEW_CONFIG_s *pConfig);

int AmbaDSP_LiveviewDispConfig(AMBA_DSP_VOUT_IDX_e VoutIdx, AMBA_DSP_LIVEVIEW_DISP_CONFIG_s *pDispWinConfig);
int AmbaDSP_LiveviewDispCtrl(AMBA_DSP_VOUT_IDX_e VoutIdx, int Enable);

int AmbaDSP_LiveviewCtrl(int Enable, int Show);
int AmbaDSP_LiveviewChannelCtrl(AMBA_DSP_LIVEVIEW_CHANNEL_CTRL_s *pChannelCtrl);

int AmbaDSP_LiveviewEisConfig(AMBA_EIS_COEFF_INFO_s *pEisCoeffInfo, UINT8 Enable, UINT32 CmdReadDly);
int AmbaDSP_UpdateEisInfo(AMBA_EIS_UPDATE_INFO_s *EisUpdateInfo, UINT8 Write);
int AmbaDSP_LiveviewIsoConfigUpdate(AMBA_DSP_LIVEVIEW_ISO_CONFIG_CTRL_s *isoCfgCtrl);

int AmbaDSP_LiveviewWaitUpdate(UINT8 FrameCount);

/* for multi-channel */
int AmbaDSP_VideoEncMainStreamConfig(int NumStream, AMBA_DSP_LIVEVIEW_STREAM_CONFIG_s *pStreamConfig);
int AmbaDSP_VideoEncSecStreamConfig(AMBA_DSP_LIVEVIEW_STREAM_CONFIG_s *pStreamConfig);

#endif  /* _AMBA_DSP_LIVEVIEW_H_ */
