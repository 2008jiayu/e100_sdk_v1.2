/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaDSP_WarpCor.h
 *
 *  @Copyright      :: Copyright (C) 2012 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Definitions & Constants for Ambarella DSP Image Kernel Warp structure and APIs
\*-------------------------------------------------------------------------------------------------------------------*/

#ifndef _AMBA_DSP_WARP_CORE_H_
#define _AMBA_DSP_WARP_CORE_H_

#include "AmbaDSP_ImgUtility.h"
#include "AmbaDSP_ImgFilter.h"

typedef struct _AMBA_DSP_IMG_OUT_WIN_INFO_s_ {
    AMBA_DSP_IMG_WIN_DIMENSION_s        MainWinDim;             /* Scaling concept */
    AMBA_DSP_IMG_WIN_DIMENSION_s        PrevWinDim[2];          /* 0:PrevA 1: PrevB */
    AMBA_DSP_IMG_WIN_DIMENSION_s        ScreennailDim;
    AMBA_DSP_IMG_WIN_DIMENSION_s        ThumbnailDim;
} AMBA_DSP_IMG_OUT_WIN_INFO_s;

typedef struct _AMBA_DSP_IMG_DZOOM_INFO_s_ {
    UINT32      ZoomX;      // 16.16 format
    UINT32      ZoomY;      // 16.16 format
    int         ShiftX;     // 16.16 format, unit in pixel in VinSensorGeo domain.
    int         ShiftY;     // 16.16 format, unit in pixel in VinSensorGeo domain.
} AMBA_DSP_IMG_DZOOM_INFO_s;

typedef struct _AMBA_DSP_IMG_EIS_INFO_s_ {
    UINT32      HorSkewPhaseInc;
    UINT32      VerSkewPhaseInc;
} AMBA_DSP_IMG_EIS_INFO_s;

typedef struct _AMBA_DSP_IMG_DMY_RANGE_s_ {
    UINT32      Left;
    UINT32      Right;
    UINT32      Top;
    UINT32      Bottom;
} AMBA_DSP_IMG_DMY_RANGE_s;

typedef struct _AMBA_DSP_IMG_WARP_REFERENCE_DMY_MARGIN_PIXELS_s_ {
    UINT32      Enable;
} AMBA_DSP_IMG_WARP_REFERENCE_DMY_MARGIN_PIXELS_s;

typedef struct _AMBA_DSP_IMG_SENSOR_FRAME_TIMING_s_ {
    UINT32      TimeScale;                  /* time scale */
    UINT32      NumUnitsInTick;             /* Frames per Second = TimeScale / (NumUnitsInTick * (1 + Interlace)) */
    UINT32      FrameLengthLines;
} AMBA_DSP_IMG_SENSOR_FRAME_TIMING_s;

typedef struct _AMBA_DSP_IMG_WARP_CALC_WIN_INFO_s_ {
/* Input parameters */
    AMBA_DSP_IMG_VIN_SENSOR_GEOMETRY_s  VinSensorGeo;
    AMBA_DSP_IMG_DZOOM_INFO_s                    DzoomInfo;
    AMBA_DSP_IMG_OUT_WIN_INFO_s                  OutWinInfo;
    AMBA_DSP_IMG_EIS_INFO_s                      EisInfo;
    AMBA_DSP_IMG_DMY_RANGE_s                     DmyRange;
    AMBA_DSP_IMG_WARP_REFERENCE_DMY_MARGIN_PIXELS_s  ReferenceDmyMargin;
/* Output parameters */
    AMBA_DSP_IMG_WIN_GEOMETRY_s         DmyWinGeo;
    AMBA_DSP_IMG_WIN_DIMENSION_s        CfaWinDim;
    AMBA_DSP_IMG_WIN_COORDINTATES_s     ActWinCrop;
	AMBA_DSP_IMG_WIN_COORDINTATES_s     ExtendedActWinCrop;
} AMBA_DSP_IMG_WARP_CALC_WIN_INFO_s;

typedef struct _AMBA_DSP_IMG_WARP_WIN_INFO_s_ {
    AMBA_DSP_IMG_DZOOM_INFO_s                    DzoomInfo;
    AMBA_DSP_IMG_WIN_GEOMETRY_s         VinActiveWin;
    AMBA_DSP_IMG_WIN_GEOMETRY_s         DmyWinGeo;
    AMBA_DSP_IMG_WIN_DIMENSION_s        CfaWinDim;
    AMBA_DSP_IMG_WIN_COORDINTATES_s     ActWinCrop;
    AMBA_DSP_IMG_WIN_DIMENSION_s        MainWinDim;    
    AMBA_DSP_IMG_WIN_DIMENSION_s        PrevWinDim[2];          /* 0:PrevA 1: PrevB */
    AMBA_DSP_IMG_WIN_DIMENSION_s        ScreennailDim;
    AMBA_DSP_IMG_WIN_DIMENSION_s        ThumbnailDim;
} AMBA_DSP_IMG_WARP_WIN_INFO_s;

typedef struct _AMBA_DSP_IMG_WARP_CALC_CFA_INFO_s_ {
    AMBA_DSP_IMG_WIN_DIMENSION_s        *pInputWinDim;
    AMBA_DSP_IMG_WIN_DIMENSION_s        *pMainWinDim;
    AMBA_DSP_IMG_WIN_DIMENSION_s        *pOutCfaWinDim;
    AMBA_DSP_IMG_MODE_CFG_s             *pMode;
} AMBA_DSP_IMG_WARP_CALC_CFA_INFO_s;

/*---------------------------------------------------------------------------*\
 * Defined in AmbaDSP_WarpCore.c
\*---------------------------------------------------------------------------*/
int AmbaDSP_WarpCore_Init(void);
int AmbaDSP_WarpCore_SetVinSensorGeo(AMBA_DSP_IMG_MODE_CFG_s *pMode, AMBA_DSP_IMG_VIN_SENSOR_GEOMETRY_s *pVinSensorGeo);
int AmbaDSP_WarpCore_SetVinActiveWin(AMBA_DSP_IMG_MODE_CFG_s *pMode, AMBA_DSP_IMG_WIN_GEOMETRY_s *pVinActiveWin);
int AmbaDSP_WarpCore_SetR2rOutWin(AMBA_DSP_IMG_MODE_CFG_s *pMode, AMBA_DSP_IMG_WIN_DIMENSION_s *pR2rOutWinDim);
int AmbaDSP_WarpCore_SetCalibWarpInfo(AMBA_DSP_IMG_MODE_CFG_s *pMode, AMBA_DSP_IMG_CALIB_WARP_INFO_s *pCalibWarpInfo);
int AmbaDSP_WarpCore_SetCalibCawarpInfo(AMBA_DSP_IMG_MODE_CFG_s *pMode, AMBA_DSP_IMG_CALIB_CAWARP_INFO_s *pCalibCawarpInfo);
int AmbaDSP_WarpCore_SetOutputWin(AMBA_DSP_IMG_MODE_CFG_s *pMode, AMBA_DSP_IMG_OUT_WIN_INFO_s *pOutWinInfo);
int AmbaDSP_WarpCore_SetDzoomFactor(AMBA_DSP_IMG_MODE_CFG_s *pMode, AMBA_DSP_IMG_DZOOM_INFO_s *pDzoomInfo);
int AmbaDSP_WarpCore_SetEisInfo(AMBA_DSP_IMG_MODE_CFG_s *pMode, AMBA_DSP_IMG_EIS_INFO_s *pEisInfo);
int AmbaDSP_WarpCore_SetDummyWinMarginRange(AMBA_DSP_IMG_MODE_CFG_s *pMode, AMBA_DSP_IMG_DMY_RANGE_s *pDmyRange);
int AmbaDSP_WarpCore_SetWarpReferenceDummyWinMarginPixels(AMBA_DSP_IMG_MODE_CFG_s *pMode, AMBA_DSP_IMG_WARP_REFERENCE_DMY_MARGIN_PIXELS_s *pReferenceDmyMargin);
int AmbaDSP_WarpCore_SetSensorFrameTiming(AMBA_DSP_IMG_MODE_CFG_s *pMode, AMBA_DSP_IMG_SENSOR_FRAME_TIMING_s *pSsrFrmTiming);
int AmbaDSP_WarpCore_SetDspVideoMode(AMBA_DSP_IMG_MODE_CFG_s *pMode, UINT32 DspVideoMode);

#if 0
#define AMBA_DSP_IMG_WARP_SET_VIDEO_NON_OVERSAMPLING_MODE   0
#define AMBA_DSP_IMG_WARP_SET_VIDEO_OVERSAMPLING_MODE       1
#else
#define AMBA_DSP_IMG_WARP_SET_VIDEO_EXPRESS_MODE_NONSTITCH    0
#define AMBA_DSP_IMG_WARP_SET_VIDEO_EXPRESS_MODE_STITCH       1
#define AMBA_DSP_IMG_WARP_SET_VIDEO_HYBIRD_MODE_NONSTITCH     2
#define AMBA_DSP_IMG_WARP_SET_VIDEO_HYBIRD_MODE_STITCH        3
#endif

int AmbaDSP_WarpCore_SetVertWarpFlipEnb(AMBA_DSP_IMG_MODE_CFG_s * pMode, UINT32 VertWarpFlipEnb);

#define AMBA_DSP_IMG_WARP_CONFIG_FORCE_DISABLE   1
#define AMBA_DSP_IMG_WARP_CONFIG_PRE_CALCULATE   2

int AmbaDSP_WarpCore_CalcDspWarp(AMBA_DSP_IMG_MODE_CFG_s *pMode, UINT32 Config);
int AmbaDSP_WarpCore_SetDspWarp(AMBA_DSP_IMG_MODE_CFG_s *pMode);
int AmbaDSP_WarpCore_CalcDspCawarp(AMBA_DSP_IMG_MODE_CFG_s *pMode, UINT32 Config);
int AmbaDSP_WarpCore_SetDspCawarp(AMBA_DSP_IMG_MODE_CFG_s *pMode);
int AmbaDSP_WarpCore_SetDspWarpAndCawarp(AMBA_DSP_IMG_MODE_CFG_s *pMode);
#endif /* _AMBA_DSP_WARP_CORE_H_ */
