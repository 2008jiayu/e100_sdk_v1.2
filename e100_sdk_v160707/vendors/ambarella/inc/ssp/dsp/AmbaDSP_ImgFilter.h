/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaDSP_ImgFilter.h
 *
 *  @Copyright      :: Copyright (C) 2012 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Definitions & Constants for Ambarella DSP Image Kernel Normal Filter APIs
\*-------------------------------------------------------------------------------------------------------------------*/

#ifndef _AMBA_DSP_IMG_FILTER_H_
#define _AMBA_DSP_IMG_FILTER_H_

#include "AmbaDSP_ImgDef.h"
#include "AmbaDSP_ImgUtility.h" // AMBA_DSP_IMG_HDR_CFG_STATUS_s need

#define AMBA_DSP_IMG_NUM_EXPOSURE_CURVE         (256)
#define AMBA_DSP_IMG_NUM_TONE_CURVE             (256)
#define AMBA_DSP_IMG_NUM_CHROMA_GAIN_CURVE      (128)
#define AMBA_DSP_IMG_NUM_CORING_TABLE_INDEX     (256)
#define AMBA_DSP_IMG_NUM_MAX_FIR_COEFF          (10)
#define AMBA_DSP_IMG_NUM_VIDEO_MCTF             (4)
#define AMBA_DSP_IMG_NUM_MAX_HDR_EXPO           (4)
#define AMBA_DSP_IMG_NUM_MAX_HDR_BLEND          (AMBA_DSP_IMG_NUM_MAX_HDR_EXPO - 1)
#define AMBA_DSP_IMG_HDR_AMPLINEAR_LUT_SIZE     (353)
#define AMBA_DSP_IMG_HDR_ALPHA_TABLE_SIZE       (128) //9 *14 + 2(reserved)

#define AMBA_DSP_IMG_CC_3D_SIZE                 (17536)
#define AMBA_DSP_IMG_SEC_CC_SIZE                (20608)
#define AMBA_DSP_IMG_CC_REG_SIZE                (18752)

#define AMBA_DSP_IMG_AWB_UNIT_SHIFT             (12)

#define AMBA_DSP_IMG_MCTF_CFG_SIZE              (528)
#define AMBA_DSP_IMG_CC_CFG_SIZE                (20608)
#define AMBA_DSP_IMG_CMPR_CFG_SIZE              (544)

#define AMBA_DSP_IMG_SHP_A_SELECT_ASF (0x0)
#define AMBA_DSP_IMG_SHP_A_SELECT_SHP (0x1)
#define AMBA_DSP_IMG_SHP_A_SELECT_DE_EDGE (0x2)

typedef enum _AMBA_DSP_IMG_BAYER_PATTERN_e_ {
    AMBA_DSP_IMG_BAYER_PATTERN_RG = 0,
    AMBA_DSP_IMG_BAYER_PATTERN_BG,
    AMBA_DSP_IMG_BAYER_PATTERN_GR,
    AMBA_DSP_IMG_BAYER_PATTERN_GB
} AMBA_DSP_IMG_BAYER_PATTERN_e;

typedef struct _AMBA_DSP_IMG_SENSOR_INFO_s_ {
    UINT8   SensorID;
    UINT8   NumFieldsPerFormat; /* maxumum 8 fields per frame */
    UINT8   SensorResolution;   /* Number of bits for data representation */
    UINT8   SensorPattern;      /* Bayer patterns RG, BG, GR, GB */
    UINT8   FirstLineField[8];
    UINT32  SensorReadOutMode;
} AMBA_DSP_IMG_SENSOR_INFO_s;

typedef struct _AMBA_DSP_IMG_BLACK_CORRECTION_s_ {
    INT16   BlackR;
    INT16   BlackGr;
    INT16   BlackGb;
    INT16   BlackB;
} AMBA_DSP_IMG_BLACK_CORRECTION_s;

#define    AMBA_DSP_IMG_VIG_VER_1_0     0x20130218

typedef enum _AMBA_DSP_IMG_VIGNETTE_VIGSTRENGTHEFFECT_MODE_e_ {
    AMBA_DSP_IMG_VIGNETTE_DefaultMode = 0,
    AMBA_DSP_IMG_VIGNETTE_KeepRatioMode = 1,
} AMBA_DSP_IMG_VIGNETTE_VIGSTRENGTHEFFECT_MODE_e;

typedef struct _AMBA_DSP_IMG_CALIB_VIGNETTE_INFO_s_ {
    UINT32  Version;
    int     TableWidth;
    int     TableHeight;
    AMBA_DSP_IMG_VIN_SENSOR_GEOMETRY_s  CalibVinSensorGeo;
    UINT32  Reserved;                   // Reserved for extention.
    UINT32  Reserved1;                  // Reserved for extention.
    UINT16  *pVignetteRedGain;          // Vignette table array addr.
    UINT16  *pVignetteGreenEvenGain;    // Vignette table array addr.
    UINT16  *pVignetteGreenOddGain;     // Vignette table array addr.
    UINT16  *pVignetteBlueGain;         // Vignette table array addr.
} AMBA_DSP_IMG_CALIB_VIGNETTE_INFO_s;

typedef enum _AMBA_DSP_IMG_VIGNETTE_CONTROL_e_ {
	AMBA_DSP_IMG_VIGNETTE_CONTROL_VERT_FLIP     =   0x1,  /* vertical flip. */
	AMBA_DSP_IMG_VIGNETTE_CONTROL_HORZ_FLIP     =   0x2,  /* horizontal flip. */
} AMBA_DSP_IMG_VIGNETTE_CONTROL_e;

typedef struct _AMBA_DSP_IMG_VIGNETTE_CALC_INFO_s_ {
    UINT8   Enb;
    UINT8   GainShift;
    UINT8   VigStrengthEffectMode;
    UINT8   Control;
    UINT32  ChromaRatio;
    UINT32  VigStrength;
    AMBA_DSP_IMG_VIN_SENSOR_GEOMETRY_s  CurrentVinSensorGeo;
    AMBA_DSP_IMG_CALIB_VIGNETTE_INFO_s  CalibVignetteInfo;
} AMBA_DSP_IMG_VIGNETTE_CALC_INFO_s;

typedef struct _AMBA_DSP_IMG_BYPASS_VIGNETTE_INFO_s_ {
    UINT8   Enable;
    UINT16  GainShift;
    UINT16  *pRedGain;             // Pointer to one of tables in gain_path of A7l structure
    UINT16  *pGreenEvenGain;       // Pointer to one of tables in gain_path of A7l structure
    UINT16  *pGreenOddGain;        // Pointer to one of tables in gain_path of A7l structure
    UINT16  *pBlueGain;            // Pointer to one of tables in gain_path of A7l structure
} AMBA_DSP_IMG_BYPASS_VIGNETTE_INFO_s;

typedef struct _AMBA_DSP_IMG_CFA_LEACKAGE_FILTER_s_ {
    UINT32  Enb;
    INT8    AlphaRR;
    INT8    AlphaRB;
    INT8    AlphaBR;
    INT8    AlphaBB;
    UINT16  SaturationLevel;
} AMBA_DSP_IMG_CFA_LEAKAGE_FILTER_s;

typedef struct _AMBA_DSP_IMG_DBP_CORRECTION_s_ {
    UINT8   Enb;    /* 0: disable                       */
                    /* 1: hot 1st order, dark 2nd order */
                    /* 2: hot 2nd order, dark 1st order */
                    /* 3: hot 2nd order, dark 2nd order */
                    /* 4: hot 1st order, dark 1st order */
    UINT8   HotPixelStrength;
    UINT8   DarkPixelStrength;
    UINT8   CorrectionMethod;   /* 0: video, 1:still    */
} AMBA_DSP_IMG_DBP_CORRECTION_s;

#define    AMBA_DSP_IMG_SBP_VER_1_0     0x20130218
typedef struct _AMBA_DSP_IMG_CALIB_SBP_INFO_s_ {
    UINT32  Version;            /* 0x20130218 */
    UINT8   *SbpBuffer;
    AMBA_DSP_IMG_VIN_SENSOR_GEOMETRY_s  VinSensorGeo;/* Vin sensor geometry when calibrating. */
    UINT32  Reserved;           /* Reserved for extention. */
    UINT32  Reserved1;          /* Reserved for extention. */
} AMBA_DSP_IMG_CALIB_SBP_INFO_s;

typedef struct _AMBA_DSP_IMG_SBP_CORRECTION_s_ {
    UINT8   Enb;
    UINT8   Reserved[3];
    AMBA_DSP_IMG_VIN_SENSOR_GEOMETRY_s  CurrentVinSensorGeo;
    AMBA_DSP_IMG_CALIB_SBP_INFO_s       CalibSbpInfo;
} AMBA_DSP_IMG_SBP_CORRECTION_s;

typedef struct _AMBA_DSP_IMG_BYPASS_SBP_INFO_s_ {
    UINT8   Enable;
    UINT16  PixelMapWidth;
    UINT16  PixelMapHeight;
    UINT16  PixelMapPitch;
    UINT8   *pMap;
} AMBA_DSP_IMG_BYPASS_SBP_INFO_s;

typedef struct _AMBA_DSP_IMG_GRID_POINT_s_ {
    INT16   X;
    INT16   Y;
} AMBA_DSP_IMG_GRID_POINT_s;

#define    AMBA_DSP_IMG_CAWARP_VER_1_0     0x20130125

typedef struct _AMBA_DSP_IMG_CALIB_CAWARP_INFO_s_ {
    UINT32  Version;            /* 0x20130125 */
    int     HorGridNum;         /* Horizontal grid number. */
    int     VerGridNum;         /* Vertical grid number. */
    int     TileWidthExp;       /* 4:16, 5:32, 6:64, 7:128, 8:256, 9:512 */
    int     TileHeightExp;      /* 4:16, 5:32, 6:64, 7:128, 8:256, 9:512 */
    AMBA_DSP_IMG_VIN_SENSOR_GEOMETRY_s  VinSensorGeo;   /* Vin sensor geometry when calibrating. */
    INT16   RedScaleFactor;
    INT16   BlueScaleFactor;
    UINT32  Enb2StageCompensation : 1;
    UINT32  Reserved;           /* Reserved for extention. */
    UINT32  Reserved1;          /* Reserved for extention. */
    AMBA_DSP_IMG_GRID_POINT_s   *pCaWarp;   /* Warp grid vector arrey. */
} AMBA_DSP_IMG_CALIB_CAWARP_INFO_s;

typedef struct _AMBA_DSP_IMG_CAWARP_CALC_INFO_s_ {
    UINT32   CaWarpEnb;
    UINT32   Control;

    AMBA_DSP_IMG_CALIB_CAWARP_INFO_s    CalibCaWarpInfo;

    AMBA_DSP_IMG_VIN_SENSOR_GEOMETRY_s  VinSensorGeo;       /* Current Vin sensor geometry. */
    AMBA_DSP_IMG_WIN_DIMENSION_s        R2rOutWinDim;       /* Raw 2 raw scaling output window */
    AMBA_DSP_IMG_WIN_GEOMETRY_s         DmyWinGeo;          /* Cropping concept */
    AMBA_DSP_IMG_WIN_DIMENSION_s        CfaWinDim;          /* Scaling concept */
} AMBA_DSP_IMG_CAWARP_CALC_INFO_s;

typedef struct _AMBA_DSP_IMG_BYPASS_CAWARP_INFO_s_ {
    UINT8   Enable;
    UINT16  HorzWarpEnable;
    UINT16  VertWarpEnable;
    UINT8   HorzPassGridArrayWidth;
    UINT8   HorzPassGridArrayHeight;
    UINT8   HorzPassHorzGridSpacingExponent;
    UINT8   HorzPassVertGridSpacingExponent;
    UINT8   VertPassGridArrayWidth;
    UINT8   VertPassGridArrayHeight;
    UINT8   VertPassHorzGridSpacingExponent;
    UINT8   VertPassVertGridSpacingExponent;
    UINT16  RedScaleFactor;
    UINT16  BlueScaleFactor;
    INT16   *pWarpHorzTable;
    INT16   *pWarpVertTable;
} AMBA_DSP_IMG_BYPASS_CAWARP_INFO_s;

typedef struct _AMBA_DSP_IMG_CAWARP_SET_INFO_s_ {
    UINT8   ResetVertCaWarp;
    UINT8   Reserved;
    UINT16  Reserved1;
} AMBA_DSP_IMG_CAWARP_SET_INFO_s;

typedef struct _AMBA_DSP_IMG_WB_GAIN_s_ {
    UINT32  GainR;
    UINT32  GainG;
    UINT32  GainB;
    UINT32  AeGain;
    UINT32  GlobalDGain;
} AMBA_DSP_IMG_WB_GAIN_s;

typedef struct _AMBA_DSP_IMG_DGAIN_SATURATION_s_ {
    UINT32  LevelRed;       /* 14:0 */
    UINT32  LevelGreenEven;
    UINT32  LevelGreenOdd;
    UINT32  LevelBlue;
} AMBA_DSP_IMG_DGAIN_SATURATION_s;

typedef struct _AMBA_DSP_IMG_CFA_NOISE_FILTER_s_ {
    UINT8   Enb;
    UINT16  NoiseLevel[3];          /* R/G/B, 0-8192 */
    UINT16  OriginalBlendStr[3];    /* R/G/B, 0-256 */
    UINT16  ExtentRegular[3];       /* R/G/B, 0-256 */
    UINT16  ExtentFine[3];          /* R/G/B, 0-256 */
    UINT16  StrengthFine[3];        /* R/G/B, 0-256 */
    UINT16  SelectivityRegular;     /* 0, 50, 100, 150, 200, 250 */
    UINT16  SelectivityFine;        /* 0, 50, 100, 150, 200, 250 */
} AMBA_DSP_IMG_CFA_NOISE_FILTER_s;

typedef struct _AMBA_DSP_IMG_ANTI_ALIASING_s_ {
    UINT32  Enb;                    //(prefix)_anti_aliasing.enable 0:4
    UINT16  Thresh;                 //(prefix)_anti_aliasing.thresh 0:16383 # when enable=4
    UINT16  LogFractionalCorrect;   //(prefix)_anti_aliasing.log_fractional_correct 0:7 # when enable=4
} AMBA_DSP_IMG_ANTI_ALIASING_s;

typedef struct _AMBA_DSP_IMG_LOCAL_EXPOSURE_s_ {
    UINT8   Enb;
    UINT8   Radius; /* 0-6; 4x4 to 16x16 */
    UINT8   LumaWeightRed;
    UINT8   LumaWeightGreen;
    UINT8   LumaWeightBlue;
    UINT8   LumaWeightShift;
    UINT16  GainCurveTable[AMBA_DSP_IMG_NUM_EXPOSURE_CURVE];
} AMBA_DSP_IMG_LOCAL_EXPOSURE_s;

typedef struct _AMBA_DSP_IMG_DEF_BLC_s_ {
    UINT8   Enb;
    UINT8   Reserved;

} AMBA_DSP_IMG_DEF_BLC_s;

typedef struct _AMBA_DSP_IMG_DEMOSAIC_s_ {
    UINT16  ActivityThresh; // 0:31
    UINT16  ActivityDifferenceThresh; // 0:16383
    UINT16  GradClipThresh; // 0:4095
    UINT16  GradNoiseThresh; // 0:4095
} AMBA_DSP_IMG_DEMOSAIC_s;

typedef struct _AMBA_DSP_IMG_COLOR_CORRECTION_s_ {
    UINT32  MatrixThreeDTableAddr;
    //UINT32  SecCcAddr;
} AMBA_DSP_IMG_COLOR_CORRECTION_s;

typedef struct _AMBA_DSP_IMG_COLOR_CORRECTION_REG_s_ {
    UINT32  RegSettingAddr;
} AMBA_DSP_IMG_COLOR_CORRECTION_REG_s;

typedef struct _AMBA_DSP_IMG_TONE_CURVE_s_ {
    UINT16  ToneCurveRed[AMBA_DSP_IMG_NUM_TONE_CURVE];
    UINT16  ToneCurveGreen[AMBA_DSP_IMG_NUM_TONE_CURVE];
    UINT16  ToneCurveBlue[AMBA_DSP_IMG_NUM_TONE_CURVE];
} AMBA_DSP_IMG_TONE_CURVE_s;

typedef struct _AMBA_DSP_IMG_RGB_TO_YUV_s_ {
    INT16   MatrixValues[9];
    INT16   YOffset;
    INT16   UOffset;
    INT16   VOffset;
} AMBA_DSP_IMG_RGB_TO_YUV_s;

typedef struct _AMBA_DSP_IMG_CHROMA_SCALE_s_ {
    UINT8   Enb; /* 0:1 */
    UINT8   Reserved;
    UINT16  Reserved1;
    UINT16  GainCurve[AMBA_DSP_IMG_NUM_CHROMA_GAIN_CURVE]; /* 0:4095 */
} AMBA_DSP_IMG_CHROMA_SCALE_s;

typedef struct _AMBA_DSP_IMG_CHROMA_MEDIAN_FILTER_s_{
    int     Enable;
    UINT16  CbAdaptiveStrength; // 0:256
    UINT16  CrAdaptiveStrength; // 0:256
    UINT8   CbNonAdaptiveStrength; // 0:31
    UINT8   CrNonAdaptiveStrength; // 0:31
    UINT16  CbAdaptiveAmount; // 0:256 
    UINT16  CrAdaptiveAmount; // 0:256
    UINT16  Reserved;
} AMBA_DSP_IMG_CHROMA_MEDIAN_FILTER_s;

typedef struct _AMBA_DSP_IMG_CDNR_INFO_s_ {
    UINT8 Enable;
} AMBA_DSP_IMG_CDNR_INFO_s;

typedef struct _AMBA_DSP_IMG_DEFER_COLOR_CORRECTION_s_{
    UINT8     Enable;
} AMBA_DSP_IMG_DEFER_COLOR_CORRECTION_s;

typedef struct _AMBA_DSP_IMG_LISO_PROCESS_SELECT_s_ {
    UINT8   AdvancedFeaturesEnable;   // 0:LISO 1:3-pass(MISO) 2:2-pass
    UINT8   UseSharpenNotAsf;     // 0:ASF 1:SHP ;unavailable if 1=li_processing_select.advanced_features_enable
} AMBA_DSP_IMG_LISO_PROCESS_SELECT_s;

typedef struct _AMBA_DSP_IMG_ALPHA_{
    UINT8   AlphaMinus;
    UINT8   AlphaPlus;
    UINT16  SmoothAdaptation;
    UINT16  SmoothEdgeAdaptation;
    UINT8   T0;
    UINT8   T1;
} AMBA_DSP_IMG_ALPHA_s;

typedef struct _AMBA_DSP_IMG_CORING_s_ {
    UINT8   Coring[AMBA_DSP_IMG_NUM_CORING_TABLE_INDEX];
    UINT8   FractionalBits;
} AMBA_DSP_IMG_CORING_s;

typedef struct _AMBA_DSP_IMG_FIR_{
    UINT8  Specify; /* 0:4 */
    UINT16 PerDirFirIsoStrengths[9]; /* 0:256; Specify = 3 */ 
    UINT16 PerDirFirDirStrengths[9]; /* 0:256; Specify = 3 */ 
    UINT16 PerDirFirDirAmounts[9]; /* 0:256; Specify = 3 */ 
    INT16 Coefs[9][25]; /* 0:1023; Specify = 1,4 */ 
    UINT16 StrengthIso; /* 0:256; Specify = 0,2 */ 
    UINT16 StrengthDir; /* 0:256; Specify = 2 */ 
    UINT8  WideEdgeDetect;
    UINT16 EdgeThresh; // Only hili_luma_mid_high_freq_recover need
} AMBA_DSP_IMG_FIR_s;

typedef struct _AMBA_DSP_IMG_LEVEL_s_ {
    UINT8   Low;
    UINT8   LowDelta;
    UINT8   LowStrength;
    UINT8   MidStrength;
    UINT8   High;
    UINT8   HighDelta;
    UINT8   HighStrength;
} AMBA_DSP_IMG_LEVEL_s;

typedef struct _AMBA_DSP_IMG_TABLE_INDEXING_s_{
    UINT8 YToneOffset;
    UINT8 YToneShift;
    UINT8 YToneBits;
    UINT8 UToneOffset;
    UINT8 UToneShift;
    UINT8 UToneBits;
    UINT8 VToneOffset;
    UINT8 VToneShift;
    UINT8 VToneBits;
    UINT8 *pTable;
    //UINT8 MaxYToneIndex;
    //UINT8 MaxUToneIndex;
    //UINT8 MaxVToneIndex;
} AMBA_DSP_IMG_TABLE_INDEXING_s;

typedef struct _AMBA_DSP_IMG_SHARPEN_BOTH_s_{
    UINT8  Enable;
    UINT8  Mode;
    UINT16 EdgeThresh;
    UINT8  WideEdgeDetect;
    UINT8 MaxChangeUp5x5;
    UINT8 MaxChangeDown5x5;
    UINT8 Reserved;
}AMBA_DSP_IMG_SHARPEN_BOTH_s;

typedef struct _AMBA_DSP_IMG_SHARPEN_NOISE_s_{
    UINT8 MaxChangeUp;
    UINT8 MaxChangeDown;
    AMBA_DSP_IMG_FIR_s          SpatialFir;
    AMBA_DSP_IMG_LEVEL_s        LevelStrAdjust;// 1 //Fir2OutScale
    // new in A12
    UINT8 LevelStrAdjustNotT0T1LevelBased;
    UINT8 T0; // T0<=T1, T1-T0<=15
    UINT8 T1;
    UINT8 AlphaMin;
    UINT8 AlphaMax;
    UINT8 Reserved;
    UINT16 Reserved1;
}AMBA_DSP_IMG_SHARPEN_NOISE_s;

typedef struct _AMBA_DSP_IMG_FULL_ADAPTATION_s_{
    UINT8                AlphaMinUp;
    UINT8                AlphaMaxUp;
    UINT8                T0Up;
    UINT8                T1Up;
    UINT8                AlphaMinDown;
    UINT8                AlphaMaxDown;
    UINT8                T0Down;
    UINT8                T1Down;
} AMBA_DSP_IMG_FULL_ADAPTATION_s;


typedef struct _AMBA_DSP_IMG_ASF_INFO_s_ {
    UINT8                   Enable;
    AMBA_DSP_IMG_FIR_s      Fir;
    UINT8                   DirectionalDecideT0;
    UINT8                   DirectionalDecideT1;
    AMBA_DSP_IMG_FULL_ADAPTATION_s      Adapt;
    AMBA_DSP_IMG_LEVEL_s    LevelStrAdjust;
    AMBA_DSP_IMG_LEVEL_s    T0T1Div;
    UINT8                   MaxChangeNotT0T1Alpha;
    UINT8                   MaxChangeUp;
    UINT8                   MaxChangeDown;
    UINT8                  Reserved;  /* to keep 32 alignment */
} AMBA_DSP_IMG_ASF_INFO_s;

typedef struct _AMBA_DSP_IMG_CHROMA_ASF_INFO_s_ {
    UINT8                   Enable;
    AMBA_DSP_IMG_FIR_s      Fir;
    UINT8                   DirectionalDecideT0;
    UINT8                   DirectionalDecideT1;
    UINT8                   AlphaMin;
    UINT8                   AlphaMax;
    UINT8                   T0;
    UINT8                   T1;
    AMBA_DSP_IMG_LEVEL_s    LevelStrAdjust;
    AMBA_DSP_IMG_LEVEL_s    T0T1Div;
    UINT8                   MaxChangeNotT0T1Alpha;
    UINT8                   MaxChange;
    UINT16                  Reserved;  /* to keep 32 alignment */
} AMBA_DSP_IMG_CHROMA_ASF_INFO_s;

typedef struct {
    int Enable;
    AMBA_DSP_IMG_FIR_s Fir;
    AMBA_DSP_IMG_LEVEL_s Coring1IndexMul;
    AMBA_DSP_IMG_LEVEL_s TurnOff;
    int DirectionalDecideT0;
    int DirectionalDecideT1;
    int FiltChroma;
    int Coring[256];
} AMBA_DSP_IMG_ASF_GUI_s;

typedef struct _AMBA_DSP_IMG_HISO_CHROMA_LOW_VERY_LOW_FILTER_s_ {
    UINT8 EdgeStartCb;
    UINT8 EdgeStartCr;
    UINT8 EdgeEndCb;
    UINT8 EdgeEndCr;
} AMBA_DSP_IMG_HISO_CHROMA_LOW_VERY_LOW_FILTER_s;

typedef struct _AMBA_DSP_IMG_HISO_CHROMA_FILTER_s_ {
    UINT8   Enable;
    UINT8   NoiseLevelCb;          /* 0-255 */
    UINT8   NoiseLevelCr;          /* 0-255 */
    UINT8   Reserved;
    UINT16  OriginalBlendStrengthCb; /* Cb 0-256  */
    UINT16  OriginalBlendStrengthCr; /* Cr 0-256  */
} AMBA_DSP_IMG_HISO_CHROMA_FILTER_s;

typedef struct _AMBA_DSP_IMG_HISO_CHROMA_FILTER_COMBINE_s_ {
    UINT8 T0Cb;
    UINT8 T0Cr;
    UINT8 T1Cb;
    UINT8 T1Cr;
    UINT8 AlphaMaxCb;
    UINT8 AlphaMaxCr;
    UINT8 AlphaMinCb;
    UINT8 AlphaMinCr;
    UINT8 MaxChangeCb;
    UINT8 MaxChangeCr;
    UINT8 Reserved[2];
} AMBA_DSP_IMG_HISO_CHROMA_FILTER_COMBINE_s;

typedef struct _AMBA_DSP_IMG_HISO_LUMA_FILTER_COMBINE_s_ {
    UINT8 T0;
    UINT8 T1;
    UINT8 AlphaMax;
    UINT8 AlphaMin;
    UINT8 MaxChange;
    UINT8 Reserved[3];
} AMBA_DSP_IMG_HISO_LUMA_FILTER_COMBINE_s;
// TODO: It should be removed
typedef struct _AMBA_DSP_IMG_HISO_COMBINE_s_ {
    UINT8 T0Cb;
    UINT8 T0Cr;
    UINT8 T0Y;
    UINT8 T1Cb;
    UINT8 T1Cr;
    UINT8 T1Y;
    UINT8 AlphaMaxCb;
    UINT8 AlphaMaxCr;
    UINT8 AlphaMaxY;
    UINT8 AlphaMinCb;
    UINT8 AlphaMinCr;
    UINT8 AlphaMinY;
    UINT8 MaxChangeNotT0T1LevelBasedCb;
    UINT8 MaxChangeNotT0T1LevelBasedCr;
    UINT8 MaxChangeNotT0T1LevelBasedY;
    UINT8 MaxChangeCb;
    UINT8 MaxChangeCr;
    UINT8 MaxChangeY;
    AMBA_DSP_IMG_LEVEL_s    EitherMaxChangeOrT0T1AddLevelCb;
    AMBA_DSP_IMG_LEVEL_s    EitherMaxChangeOrT0T1AddLevelCr;
    AMBA_DSP_IMG_LEVEL_s    EitherMaxChangeOrT0T1AddLevelY;
    UINT8 SignalPreserveCb;
    UINT8 SignalPreserveCr;
    UINT8 SignalPreserveY;
    AMBA_DSP_IMG_TABLE_INDEXING_s    ThreeD;
} AMBA_DSP_IMG_HISO_COMBINE_s;

typedef struct _AMBA_DSP_IMG_HISO_FREQ_RECOVER_s_ {
    AMBA_DSP_IMG_FIR_s      Fir;
    UINT8  SmoothSelect[AMBA_DSP_IMG_NUM_CORING_TABLE_INDEX];
    UINT8  MaxDown;
    UINT8  MaxUp;
    AMBA_DSP_IMG_LEVEL_s    Level;
} AMBA_DSP_IMG_HISO_FREQ_RECOVER_s;

typedef struct _AMBA_DSP_IMG_HISO_LUMA_BLEND_s_ {
    UINT8 Enable;
} AMBA_DSP_IMG_HISO_LUMA_BLEND_s;

typedef struct _AMBA_DSP_IMG_HISO_BLEND_s_ {
    AMBA_DSP_IMG_LEVEL_s    LumaLevel;
} AMBA_DSP_IMG_HISO_BLEND_s;


#define AMBA_DSP_IMG_RESAMP_COEFF_RECTWIN           (0x1)
#define AMBA_DSP_IMG_RESAMP_COEFF_M2                (0x2)
#define AMBA_DSP_IMG_RESAMP_COEFF_M4                (0x4)
#define AMBA_DSP_IMG_RESAMP_COEFF_LP_MEDIUM         (0x8)
#define AMBA_DSP_IMG_RESAMP_COEFF_LP_STRONG         (0x10)

#define AMBA_DSP_IMG_RESAMP_SELECT_CFA              (0x1)
#define AMBA_DSP_IMG_RESAMP_SELECT_MAIN             (0x2)
#define AMBA_DSP_IMG_RESAMP_SELECT_PRV_A            (0x4)
#define AMBA_DSP_IMG_RESAMP_SELECT_PRV_B            (0x8)
#define AMBA_DSP_IMG_RESAMP_SELECT_PRV_C            (0x10)

#define AMBA_DSP_IMG_RESAMP_COEFF_MODE_ALWAYS       (0)
#define AMBA_DSP_IMG_RESAMP_COEFF_MODE_ONE_FRAME    (1)

typedef struct _AMBA_DSP_IMG_RESAMPLER_COEF_ADJ_s_ {
    UINT32  ControlFlag;
    UINT16  ResamplerSelect;
    UINT16  Mode;
} AMBA_DSP_IMG_RESAMPLER_COEF_ADJ_s;

typedef struct _AMBA_DSP_IMG_RESAMPLER_STR_s_ {
#define RESMP_STR_SELECT_CFA_HORZ  (0x1)
#define RESMP_STR_SELECT_CFA_VERT  (0x2)
#define RESMP_STR_SELECT_MAIN_HORZ (0x4)
#define RESMP_STR_SELECT_MAIN_VERT (0x8)
    UINT16  Select;     /* Ref. RESMP_STR_SELECT_* */
    UINT16  CutoffFreq; /* 0:M, 1:M-2, 2:M-4, 3:M-6, 4:M-8, 5:M-10, 6:M-12, 7:M-14 */
} AMBA_DSP_IMG_RESAMPLER_STR_s;


typedef struct _AMBA_DSP_IMG_CHROMA_FILTER_s_ {
    UINT8  Enable;
    UINT8  NoiseLevelCb;            /* 0-255 */
    UINT8  NoiseLevelCr;            /* 0-255 */
    UINT8  Reserved;
    UINT16 OriginalBlendStrengthCb; /* Cb 0-256  */
    UINT16 OriginalBlendStrengthCr; /* Cr 0-256  */
    UINT16 Radius;                  /* 32-64-128 */
    UINT8  Reserved1[2];
} AMBA_DSP_IMG_CHROMA_FILTER_s;

#define    AMBA_DSP_IMG_WARP_VER_1_0     0x20130101
typedef struct _AMBA_DSP_IMG_CALIB_WARP_INFO_s_ {
    UINT32                              Version;        /* 0x20130101 */
    int                                 HorGridNum;     /* Horizontal grid number. */
    int                                 VerGridNum;     /* Vertical grid number. */
    int                                 TileWidthExp;   /* 4:16, 5:32, 6:64, 7:128, 8:256, 9:512 */
    int                                 TileHeightExp;  /* 4:16, 5:32, 6:64, 7:128, 8:256, 9:512 */
    AMBA_DSP_IMG_VIN_SENSOR_GEOMETRY_s  VinSensorGeo;   /* Vin sensor geometry when calibrating. */
    UINT32                              Enb2StageCompensation : 1; /*1:enable, 0 disable*/    
    UINT32                              Reserved;       /* Reserved for extention. */
    UINT32                              Reserved1;      /* Reserved for extention. */
    UINT32                              Reserved2;      /* Reserved for extention. */
    AMBA_DSP_IMG_GRID_POINT_s           *pWarp;         /* Warp grid vector arrey. */
} AMBA_DSP_IMG_CALIB_WARP_INFO_s;

typedef enum _AMBA_DSP_IMG_CALC_WARP_CONTROL_e_ {
	AMBA_DSP_IMG_CALC_WARP_CONTROL_SEC2_SCALE       = 0x1,  /* section 2 done all scaling. section 3 does not do scaling. */
	AMBA_DSP_IMG_CALC_WARP_CONTROL_VERT_FLIP        = 0x2,  /* vertical flip. */
	AMBA_DSP_IMG_CALC_WARP_CONTROL_HORZ_FLIP        = 0x4,  /* horizontal flip. */
	AMBA_DSP_IMG_CALC_WARP_CONTROL_PRE_CALCULATE    = 0x8,  /* pre calculation, calculate grid number, exponent only, but no table interpolation. */
	AMBA_DSP_IMG_CALC_WARP_CONTROL_SENSOR_H_FLIP    = 0x10, /* hflip by sensor not iDSP */
	AMBA_DSP_IMG_CALC_WARP_CONTROL_SENSOR_V_FLIP    = 0x20, /* vflip by sensor not iDSP */
	AMBA_DSP_IMG_CALC_WARP_CONTROL_DMA_V_FLIP       = 0x40, /* vflip by DMA */
} AMBA_DSP_IMG_CALC_WARP_CONTROL_e;

typedef struct _AMBA_DSP_IMG_WARP_CALC_INFO_s_ {
    /* Warp related settings */
    UINT32                              WarpEnb;
    UINT32                              Control;

    AMBA_DSP_IMG_CALIB_WARP_INFO_s      CalibWarpInfo;

    AMBA_DSP_IMG_VIN_SENSOR_GEOMETRY_s  VinSensorGeo;           /* Current Vin sensor geometry. */
    AMBA_DSP_IMG_WIN_DIMENSION_s        R2rOutWinDim;           /* Raw 2 raw scaling output window */
    AMBA_DSP_IMG_WIN_GEOMETRY_s         DmyWinGeo;              /* Cropping concept */
    AMBA_DSP_IMG_WIN_DIMENSION_s        CfaWinDim;              /* Scaling concept */
    AMBA_DSP_IMG_WIN_COORDINTATES_s     ActWinCrop;             /* Cropping concept */
    AMBA_DSP_IMG_WIN_DIMENSION_s        MainWinDim;             /* Scaling concept */
    AMBA_DSP_IMG_WIN_DIMENSION_s        PrevWinDim[2];          /* 0:PrevA 1: PrevB */
    AMBA_DSP_IMG_WIN_DIMENSION_s        ScreennailDim;
    AMBA_DSP_IMG_WIN_DIMENSION_s        ThumbnailDim;
    int                                 HorSkewPhaseInc;        /* For EIS */
    UINT32                              ExtraVertOutMode;       /* To support warp table that reference dummy window margin pixels */
    double                              Theta;    
} AMBA_DSP_IMG_WARP_CALC_INFO_s;

//typedef struct _AMBA_DSP_IMG_WARP_SET_INFO_s_ {
//    UINT8   ResetVertWarp;
//    UINT8   Reserved;
//    UINT16  Reserved1;
//} AMBA_DSP_IMG_WARP_SET_INFO_s;

typedef struct _AMBA_DSP_IMG_BYPASS_WARP_DZOOM_INFO_s_ {
    // Warp part
    UINT8   Enable;
    UINT32  WarpControl;
    UINT8   GridArrayWidth;
    UINT8   GridArrayHeight;
    UINT8   HorzGridSpacingExponent;
    UINT8   VertGridSpacingExponent;
    UINT8   VertWarpEnable;
    UINT8   VertWarpGridArrayWidth;
    UINT8   VertWarpGridArrayHeight;
    UINT8   VertWarpHorzGridSpacingExponent;
    UINT8   VertWarpVertGridSpacingExponent;
    INT16   *pWarpHorizontalTable;
    INT16   *pWarpVerticalTable;

    // Dzoom part
    UINT8   DzoomEnable;
    UINT32  ActualLeftTopX;
    UINT32  ActualLeftTopY;
    UINT32  ActualRightBotX;
    UINT32  ActualRightBotY;
    UINT32  ZoomX;
    UINT32  ZoomY;
    UINT32  XCenterOffset;
    UINT32  YCenterOffset;
    INT32   HorSkewPhaseInc;
    UINT8   ForceV4tapDisable;
    UINT16  DummyWindowXLeft;
    UINT16  DummyWindowYTop;
    UINT16  DummyWindowWidth;
    UINT16  DummyWindowHeight;
    UINT16  CfaOutputWidth;
    UINT16  CfaOutputHeight;
    UINT32  extra_sec2_vert_out_vid_mode;
} AMBA_DSP_IMG_BYPASS_WARP_DZOOM_INFO_s;

typedef struct _AMBA_DSP_IMG_GMV_INFO_s_ {
    UINT16  Enb;
    INT16   MvX;
    INT16   MvY;
    UINT16  Reserved1;
} AMBA_DSP_IMG_GMV_INFO_s;

typedef struct _AMBA_DSP_IMG_VIDEO_MCTF_ONE_CHAN_s_ {
    UINT8   TemporalAlpha0;
    UINT8   TemporalAlpha1;
    UINT8   TemporalAlpha2;
    UINT8   TemporalAlpha3;
    UINT8   TemporalT0;
    UINT8   TemporalT1;
    UINT8   TemporalT2;
    UINT8   TemporalT3;
    UINT8   TemporalMaxChange;
    UINT16  Radius;         /* 0-256 */
    UINT16  StrengthThreeD;      /* 0-256 */
    UINT16  StrengthSpatial;     /* 0-256 */
    UINT16  LevelAdjust;    /* 0-256 */
} AMBA_DSP_IMG_VIDEO_MCTF_ONE_CHAN_s;

typedef struct _AMBA_DSP_IMG_VIDEO_MCTF_INFO_s_ {
    UINT8                                 Enable; // 0,1
    UINT16                              YMaxChange; // 0:255
    UINT16                              UMaxChange; // 0:255
    UINT16                              VMaxChange; // 0:255
    UINT8                               WeightingBasedOnLocalMotion; // 0,1
    UINT8                               Threshold0[4]; // 0:63
    UINT8                               Threshold1[4]; // 0:63
    UINT8                               Threshold2[4]; // 0:63
    UINT8                               Threshold3[4]; // 0:63
    UINT16                               Alpha1[4]; // 0:255
    UINT16                               Alpha2[4]; // 0:255
    UINT16                               Alpha3[4]; // 0:255
} AMBA_DSP_IMG_VIDEO_MCTF_INFO_s;

typedef struct _AMBA_DSP_IMG_VIDEO_MCTF_TEMPORAL_ADJUST_s_ {
    UINT8 FramesCombineThresh;//0:255
    UINT8 Min;//0:3
    UINT8 MotionDetectionDelay;//1:10
    UINT16 Mul;//0:65535
    UINT8 Sub;//0:3
    UINT8 SmoothDetection;//1:67
    UINT8 MotionDetectionDcMapHigh;
    UINT8 MotionDetectionDcMapHighDelta;
    UINT8 MotionDetectionDcMapHighStrength;
    UINT8 MotionDetectionDcMapLow;
    UINT8 MotionDetectionDcMapLowDelta;
    UINT8 MotionDetectionDcMapLowStrength;
    UINT8 MotionDetectionDcMapMidStrength;
    UINT8 Reserved;
} AMBA_DSP_IMG_VIDEO_MCTF_TEMPORAL_ADJUST_s;

typedef struct _AMBA_DSP_IMG_VIDEO_MCTF_GHOST_PRV_INFO_s_ {
    UINT8   Y;
    UINT8   Cb;
    UINT8   Cr;
    UINT8   Reserved;
} AMBA_DSP_IMG_VIDEO_MCTF_GHOST_PRV_INFO_s;

typedef struct _AMBA_DSP_IMG_GBGR_MISMATCH_s_ {
    UINT8  NarrowEnable;
    UINT8  WideEnable;
    UINT16 WideSafety;
    UINT16 WideThresh;
} AMBA_DSP_IMG_GBGR_MISMATCH_s;

typedef struct _AMBA_DSP_IMG_HDR_ALPHA_CALC_ALPHA_s_ {
    UINT8  PreAlphaMode;
    UINT8  Reserved[3];
    UINT32 AlphaTableAddr;
    UINT8  SaturationNumNei;
} AMBA_DSP_IMG_HDR_ALPHA_CALC_ALPHA_s;

typedef struct _AMBA_DSP_IMG_HDR_BLENDING_INFO_s_{
    UINT8 CurrentBlendingIndex;
    UINT8 MaxBlendingNumber;
} AMBA_DSP_IMG_HDR_BLENDING_INFO_s;

typedef struct _AMBA_DSP_IMG_HDR_BLK_LEVEL_AMPLINER_s_ {
    INT16 AmpLinearBlackR;
    INT16 AmpLinearBlackGr;
    INT16 AmpLinearBlackGb;
    INT16 AmpLinearBlackB;
    INT16 AmpHdrBlackR;
    INT16 AmpHdrBlackGr;
    INT16 AmpHdrBlackGb;
    INT16 AmpHdrBlackB;
    INT16 HdrBlendBlackR;
    INT16 HdrBlendBlackGr;
    INT16 HdrBlendBlackGb;
    INT16 HdrBlendBlackB;
} AMBA_DSP_IMG_HDR_BLKLVL_AMPLINEAR_s;

typedef struct _AMBA_DSP_IMG_HDR_ALPHA_CALC_CONFIG_s_ {
    UINT8  AvgRadius;
    UINT8  AvgMethod;
    INT8   BlendControl;
    UINT8  Reserved;
    UINT8  LumaAvgWeightR;
    UINT8  LumaAvgWeightGr;
    UINT8  LumaAvgWeightGb;
    UINT8  LumaAvgWeightB;
} AMBA_DSP_IMG_HDR_ALPHA_CALC_CONFIG_s;

typedef struct _AMBA_DSP_IMG_HDR_ALPHA_CALC_THRESH_s_ {
    UINT16 SaturationThresholdR;
    UINT16 SaturationThresholdGr;
    UINT16 SaturationThresholdGb;
    UINT16 SaturationThresholdB;
} AMBA_DSP_IMG_HDR_ALPHA_CALC_THRESH_s;

typedef struct _AMBA_DSP_IMG_HDR_ALPHA_CALC_EXPOSURE_s_ {
    UINT16 InputRowOffset;
    UINT16 InputRowHeight;
    UINT16 InputRowSkip;
    UINT16 Reserved;
} AMBA_DSP_IMG_HDR_ALPHA_CALC_EXPOSURE_s;

typedef struct _AMBA_DSP_IMG_AMP_LINEAR_INFO_s_ {
    UINT32 AmplinearEnable; // if disabled, amplinear stage would do nothing
    UINT32 MultiplierR;
    UINT32 MultiplierG;
    UINT32 MultiplierB;
    UINT32 LutAddr;         // 353 entries
} AMBA_DSP_IMG_AMP_LINEAR_INFO_s;

typedef struct _AMBA_DSP_IMG_AMP_LINEARIZATION_s_ {
    UINT8 Reserved0;//HdrMode;
    UINT8 Reserved1;//HdrStream0Sub;
    UINT8 Reserved2;//HdrStream1Sub;
    UINT8 Reserved3;//HdrStream1Shift;
    UINT8 Reserved4;//HdrAlphaMode;
    AMBA_DSP_IMG_AMP_LINEAR_INFO_s AmpLinear[2];
    UINT32 ShutterRatio;
} AMBA_DSP_IMG_AMP_LINEARIZATION_s;

typedef struct _AMBA_DSP_IMG_CONTRAST_ENHANCE_s_ {
    UINT16 LowPassRadius;
    UINT16 EnhanceGain;
} AMBA_DSP_IMG_CONTRAST_ENHANCE_s;

typedef struct _AMBA_DSP_IMG_HDR_RAW_OFFSET_s_ {
    UINT32 XOffset[AMBA_DSP_IMG_NUM_MAX_HDR_EXPO];
    UINT32 YOffset[AMBA_DSP_IMG_NUM_MAX_HDR_EXPO];
} AMBA_DSP_IMG_HDR_RAW_OFFSET_s;
typedef struct _AMBA_DSP_IMG_HDR_RAW_SIZE_s_ {
    UINT32 RawSingleExposureWidth;
    UINT32 RawSingleExposureHeight;
} AMBA_DSP_IMG_HDR_RAW_SIZE_s;

int AmbaDSP_ImgSetVinSensorInfo(AMBA_DSP_IMG_MODE_CFG_s *pMode, AMBA_DSP_IMG_SENSOR_INFO_s *pVinSensorInfo);
int AmbaDSP_ImgGetVinSensorInfo(AMBA_DSP_IMG_MODE_CFG_s *pMode, AMBA_DSP_IMG_SENSOR_INFO_s *pVinSensorInfo);

/* CFA domain filters */
int AmbaDSP_ImgSetStaticBlackLevel(AMBA_DSP_IMG_MODE_CFG_s *pMode, AMBA_DSP_IMG_BLACK_CORRECTION_s *pBlackCorr);
int AmbaDSP_ImgGetStaticBlackLevel(AMBA_DSP_IMG_MODE_CFG_s *pMode, AMBA_DSP_IMG_BLACK_CORRECTION_s *pBlackCorr);

int AmbaDSP_ImgCalcVignetteCompensation(AMBA_DSP_IMG_MODE_CFG_s *pMode, AMBA_DSP_IMG_VIGNETTE_CALC_INFO_s *pVignetteCalcInfo);
int AmbaDSP_ImgSetVignetteCompensation(AMBA_DSP_IMG_MODE_CFG_s *pMode);
int AmbaDSP_ImgGetVignetteCompensation(AMBA_DSP_IMG_MODE_CFG_s *pMode, AMBA_DSP_IMG_VIGNETTE_CALC_INFO_s *pVignetteCalcInfo);

int AmbaDSP_ImgSetCfaLeakageFilter(AMBA_DSP_IMG_MODE_CFG_s *pMode, AMBA_DSP_IMG_CFA_LEAKAGE_FILTER_s *pCfaLeakage);
int AmbaDSP_ImgGetCfaLeakageFilter(AMBA_DSP_IMG_MODE_CFG_s *pMode, AMBA_DSP_IMG_CFA_LEAKAGE_FILTER_s *pCfaLeakage);

int AmbaDSP_ImgSetAntiAliasing(AMBA_DSP_IMG_MODE_CFG_s *pMode, AMBA_DSP_IMG_ANTI_ALIASING_s *pAntiAliasing);
int AmbaDSP_ImgGetAntiAliasing(AMBA_DSP_IMG_MODE_CFG_s *pMode, AMBA_DSP_IMG_ANTI_ALIASING_s *pAntiAliasing);

int AmbaDSP_ImgSetDynamicBadPixelCorrection(AMBA_DSP_IMG_MODE_CFG_s *pMode, AMBA_DSP_IMG_DBP_CORRECTION_s *pDbpCorr);
int AmbaDSP_ImgGetDynamicBadPixelCorrection(AMBA_DSP_IMG_MODE_CFG_s *pMode, AMBA_DSP_IMG_DBP_CORRECTION_s *pDbpCorr);

int AmbaDSP_ImgSetStaticBadPixelCorrection(AMBA_DSP_IMG_MODE_CFG_s *pMode, AMBA_DSP_IMG_SBP_CORRECTION_s *pSbpCorr);
int AmbaDSP_ImgGetStaticBadPixelCorrection(AMBA_DSP_IMG_MODE_CFG_s *pMode, AMBA_DSP_IMG_SBP_CORRECTION_s *pSbpCorr);

int AmbaDSP_ImgCalcCawarpCompensation(AMBA_DSP_IMG_MODE_CFG_s *pMode, AMBA_DSP_IMG_CAWARP_CALC_INFO_s *pCaCalcInfo);
int AmbaDSP_ImgSetCawarpCompensation(AMBA_DSP_IMG_MODE_CFG_s *pMode);
int AmbaDSP_ImgGetCawarpCompensation(AMBA_DSP_IMG_MODE_CFG_s *pMode, AMBA_DSP_IMG_CAWARP_CALC_INFO_s *pCaCalcInfo);

int AmbaDSP_ImgSetWbGain(AMBA_DSP_IMG_MODE_CFG_s *pMode, AMBA_DSP_IMG_WB_GAIN_s *pWbGains);
int AmbaDSP_ImgGetWbGain(AMBA_DSP_IMG_MODE_CFG_s *pMode, AMBA_DSP_IMG_WB_GAIN_s *pWbGains);

int AmbaDSP_ImgSetDgainSaturationLevel(AMBA_DSP_IMG_MODE_CFG_s *pMode, AMBA_DSP_IMG_DGAIN_SATURATION_s *pDgainSat);
int AmbaDSP_ImgGetDgainSaturationLevel(AMBA_DSP_IMG_MODE_CFG_s *pMode, AMBA_DSP_IMG_DGAIN_SATURATION_s *pDgainSat);

int AmbaDSP_ImgSetCfaNoiseFilter(AMBA_DSP_IMG_MODE_CFG_s *pMode, AMBA_DSP_IMG_CFA_NOISE_FILTER_s *pCfaNoise);
int AmbaDSP_ImgGetCfaNoiseFilter(AMBA_DSP_IMG_MODE_CFG_s *pMode, AMBA_DSP_IMG_CFA_NOISE_FILTER_s *pCfaNoise);

int AmbaDSP_ImgSetLocalExposure(AMBA_DSP_IMG_MODE_CFG_s *pMode, AMBA_DSP_IMG_LOCAL_EXPOSURE_s *pLocalExposure);
int AmbaDSP_ImgGetLocalExposure(AMBA_DSP_IMG_MODE_CFG_s *pMode, AMBA_DSP_IMG_LOCAL_EXPOSURE_s *pLocalExposure);

int AmbaDSP_ImgSetDeferredBlackLevel(AMBA_DSP_IMG_MODE_CFG_s *pMode, AMBA_DSP_IMG_DEF_BLC_s *pDefBlc);
int AmbaDSP_ImgGetDeferredBlackLevel(AMBA_DSP_IMG_MODE_CFG_s *pMode, AMBA_DSP_IMG_DEF_BLC_s *pDefBlc);

/* RGB domain filters */
int AmbaDSP_ImgSetDemosaic(AMBA_DSP_IMG_MODE_CFG_s *pMode, AMBA_DSP_IMG_DEMOSAIC_s *pDemosaic);
int AmbaDSP_ImgGetDemosaic(AMBA_DSP_IMG_MODE_CFG_s *pMode, AMBA_DSP_IMG_DEMOSAIC_s *pDemosaic);

int AmbaDSP_ImgSetColorCorrectionReg(AMBA_DSP_IMG_MODE_CFG_s *pMode, AMBA_DSP_IMG_COLOR_CORRECTION_REG_s *pColorCorrReg);
int AmbaDSP_ImgGetColorCorrectionReg(AMBA_DSP_IMG_MODE_CFG_s *pMode, AMBA_DSP_IMG_COLOR_CORRECTION_REG_s *pColorCorrReg);
int AmbaDSP_ImgSetColorCorrection(AMBA_DSP_IMG_MODE_CFG_s *pMode, AMBA_DSP_IMG_COLOR_CORRECTION_s *pColorCorr);
int AmbaDSP_ImgGetColorCorrection(AMBA_DSP_IMG_MODE_CFG_s *pMode, AMBA_DSP_IMG_COLOR_CORRECTION_s *pColorCorr);

int AmbaDSP_ImgSetToneCurve(AMBA_DSP_IMG_MODE_CFG_s *pMode, AMBA_DSP_IMG_TONE_CURVE_s *pToneCurve);
int AmbaDSP_ImgGetToneCurve(AMBA_DSP_IMG_MODE_CFG_s *pMode, AMBA_DSP_IMG_TONE_CURVE_s  *pToneCurve);


/* Y domain filters */
int AmbaDSP_ImgSetRgbToYuvMatrix(AMBA_DSP_IMG_MODE_CFG_s *pMode, AMBA_DSP_IMG_RGB_TO_YUV_s *pRgbToYuv);
int AmbaDSP_ImgGetRgbToYuvMatrix(AMBA_DSP_IMG_MODE_CFG_s *pMode, AMBA_DSP_IMG_RGB_TO_YUV_s *pRgbToYuv);

int AmbaDSP_ImgSetChromaScale(AMBA_DSP_IMG_MODE_CFG_s *pMode, AMBA_DSP_IMG_CHROMA_SCALE_s *pChromaScale);
int AmbaDSP_ImgGetChromaScale(AMBA_DSP_IMG_MODE_CFG_s *pMode, AMBA_DSP_IMG_CHROMA_SCALE_s *pChromaScale);

int AmbaDSP_ImgSetChromaMedianFilter(AMBA_DSP_IMG_MODE_CFG_s *pMode, AMBA_DSP_IMG_CHROMA_MEDIAN_FILTER_s *pChromaMedian);
int AmbaDSP_ImgGetChromaMedianFilter(AMBA_DSP_IMG_MODE_CFG_s *pMode, AMBA_DSP_IMG_CHROMA_MEDIAN_FILTER_s *pChromaMedian);

int AmbaDSP_ImgSetColorDependentNoiseReduction(AMBA_DSP_IMG_MODE_CFG_s *pMode, AMBA_DSP_IMG_CDNR_INFO_s *pCdnr);
int AmbaDSP_ImgGetColorDependentNoiseReduction(AMBA_DSP_IMG_MODE_CFG_s *pMode, AMBA_DSP_IMG_CDNR_INFO_s *pCdnr);

int AmbaDSP_ImgSetLumaProcessingMode(AMBA_DSP_IMG_MODE_CFG_s *pMode, AMBA_DSP_IMG_LISO_PROCESS_SELECT_s *pLisoProcessSelect);
int AmbaDSP_ImgGetLumaProcessingMode(AMBA_DSP_IMG_MODE_CFG_s *pMode, AMBA_DSP_IMG_LISO_PROCESS_SELECT_s *pLisoProcessSelect);

int AmbaDSP_ImgSetAdvanceSpatialFilter(AMBA_DSP_IMG_MODE_CFG_s *pMode, AMBA_DSP_IMG_ASF_INFO_s *pAsf);
int AmbaDSP_ImgGetAdvanceSpatialFilter(AMBA_DSP_IMG_MODE_CFG_s *pMode, AMBA_DSP_IMG_ASF_INFO_s *pAsf);

int AmbaDSP_ImgSet1stSharpenNoiseBoth( AMBA_DSP_IMG_MODE_CFG_s *pMode, AMBA_DSP_IMG_SHARPEN_BOTH_s *pSharpenBoth);
int AmbaDSP_ImgGet1stSharpenNoiseBoth( AMBA_DSP_IMG_MODE_CFG_s *pMode, AMBA_DSP_IMG_SHARPEN_BOTH_s *pSharpenBoth);

int AmbaDSP_ImgSet1stSharpenNoiseNoise( AMBA_DSP_IMG_MODE_CFG_s *pMode, AMBA_DSP_IMG_SHARPEN_NOISE_s *pSharpenNoise);
int AmbaDSP_ImgGet1stSharpenNoiseNoise( AMBA_DSP_IMG_MODE_CFG_s *pMode, AMBA_DSP_IMG_SHARPEN_NOISE_s *pSharpenNoise);

int AmbaDSP_ImgSet1stSharpenNoiseSharpenFir(AMBA_DSP_IMG_MODE_CFG_s *pMode, AMBA_DSP_IMG_FIR_s *pSharpenFir);
int AmbaDSP_ImgGet1stSharpenNoiseSharpenFir(AMBA_DSP_IMG_MODE_CFG_s *pMode, AMBA_DSP_IMG_FIR_s *pSharpenFir);

int AmbaDSP_ImgSet1stSharpenNoiseSharpenCoring(AMBA_DSP_IMG_MODE_CFG_s *pMode, AMBA_DSP_IMG_CORING_s *pCoring);
int AmbaDSP_ImgGet1stSharpenNoiseSharpenCoring(AMBA_DSP_IMG_MODE_CFG_s *pMode, AMBA_DSP_IMG_CORING_s *pCoring);

int AmbaDSP_ImgSet1stSharpenNoiseSharpenCoringIndexScale(AMBA_DSP_IMG_MODE_CFG_s *pMode, AMBA_DSP_IMG_LEVEL_s *pLevel);
int AmbaDSP_ImgGet1stSharpenNoiseSharpenCoringIndexScale(AMBA_DSP_IMG_MODE_CFG_s *pMode, AMBA_DSP_IMG_LEVEL_s *pLevel);

int AmbaDSP_ImgSet1stSharpenNoiseSharpenMinCoringResult(AMBA_DSP_IMG_MODE_CFG_s *pMode, AMBA_DSP_IMG_LEVEL_s *pLevel);
int AmbaDSP_ImgGet1stSharpenNoiseSharpenMinCoringResult(AMBA_DSP_IMG_MODE_CFG_s *pMode, AMBA_DSP_IMG_LEVEL_s *pLevel);

int AmbaDSP_ImgSet1stSharpenNoiseSharpenScaleCoring(AMBA_DSP_IMG_MODE_CFG_s *pMode, AMBA_DSP_IMG_LEVEL_s *pLevel);
int AmbaDSP_ImgGet1stSharpenNoiseSharpenScaleCoring(AMBA_DSP_IMG_MODE_CFG_s *pMode, AMBA_DSP_IMG_LEVEL_s *pLevel);

int AmbaDSP_ImgSetFinalSharpenNoiseBoth( AMBA_DSP_IMG_MODE_CFG_s *pMode, AMBA_DSP_IMG_SHARPEN_BOTH_s *pSharpenBoth);
int AmbaDSP_ImgGetFinalSharpenNoiseBoth( AMBA_DSP_IMG_MODE_CFG_s *pMode, AMBA_DSP_IMG_SHARPEN_BOTH_s *pSharpenBoth);

int AmbaDSP_ImgSetFinalSharpenNoiseNoise( AMBA_DSP_IMG_MODE_CFG_s *pMode, AMBA_DSP_IMG_SHARPEN_NOISE_s *pSharpenNoise);
int AmbaDSP_ImgGetFinalSharpenNoiseNoise( AMBA_DSP_IMG_MODE_CFG_s *pMode, AMBA_DSP_IMG_SHARPEN_NOISE_s *pSharpenNoise);

int AmbaDSP_ImgSetFinalSharpenNoiseSharpenCoring(AMBA_DSP_IMG_MODE_CFG_s *pMode, AMBA_DSP_IMG_CORING_s *pCoring);
int AmbaDSP_ImgGetFinalSharpenNoiseSharpenCoring(AMBA_DSP_IMG_MODE_CFG_s *pMode, AMBA_DSP_IMG_CORING_s *pCoring);

int AmbaDSP_ImgSetFinalSharpenNoiseSharpenFir(AMBA_DSP_IMG_MODE_CFG_s *pMode, AMBA_DSP_IMG_FIR_s *pFir);
int AmbaDSP_ImgGetFinalSharpenNoiseSharpenFir(AMBA_DSP_IMG_MODE_CFG_s *pMode, AMBA_DSP_IMG_FIR_s *pFir);

int AmbaDSP_ImgSetFinalSharpenNoiseSharpenMinCoringResult(AMBA_DSP_IMG_MODE_CFG_s *pMode, AMBA_DSP_IMG_LEVEL_s *pUserLevel);
int AmbaDSP_ImgGetFinalSharpenNoiseSharpenMinCoringResult(AMBA_DSP_IMG_MODE_CFG_s *pMode, AMBA_DSP_IMG_LEVEL_s *pUserLevel);

int AmbaDSP_ImgSetFinalSharpenNoiseSharpenCoringIndexScale(AMBA_DSP_IMG_MODE_CFG_s *pMode, AMBA_DSP_IMG_LEVEL_s *pUserLevel);
int AmbaDSP_ImgGetFinalSharpenNoiseSharpenCoringIndexScale(AMBA_DSP_IMG_MODE_CFG_s *pMode, AMBA_DSP_IMG_LEVEL_s *pUserLevel);

int AmbaDSP_ImgSetFinalSharpenNoiseSharpenScaleCoring( AMBA_DSP_IMG_MODE_CFG_s *pMode, AMBA_DSP_IMG_LEVEL_s *pUserLevel);
int AmbaDSP_ImgGetFinalSharpenNoiseSharpenScaleCoring( AMBA_DSP_IMG_MODE_CFG_s *pMode, AMBA_DSP_IMG_LEVEL_s *pUserLevel);

int AmbaDSP_ImgSetResamplerCoefAdj(AMBA_DSP_IMG_MODE_CFG_s *pMode, AMBA_DSP_IMG_RESAMPLER_COEF_ADJ_s *pResamplerCoefAdj);
int AmbaDSP_ImgGetResamplerCoefAdj(AMBA_DSP_IMG_MODE_CFG_s *pMode, AMBA_DSP_IMG_RESAMPLER_COEF_ADJ_s *pResamplerCoefAdj);

int AmbaDSP_ImgSetResamplerStrength(AMBA_DSP_IMG_MODE_CFG_s *pMode, AMBA_DSP_IMG_RESAMPLER_STR_s *pResamplerStr);
int AmbaDSP_ImgGetResamplerStrength(AMBA_DSP_IMG_MODE_CFG_s *pMode, AMBA_DSP_IMG_RESAMPLER_STR_s   *pResamplerStr);

int AmbaDSP_ImgSetChromaFilter(AMBA_DSP_IMG_MODE_CFG_s *pMode, AMBA_DSP_IMG_CHROMA_FILTER_s *pChromaFilter);
int AmbaDSP_ImgGetChromaFilter(AMBA_DSP_IMG_MODE_CFG_s *pMode, AMBA_DSP_IMG_CHROMA_FILTER_s *pChromaFilter);

int AmbaDSP_ImgSetWideChromaFilter(AMBA_DSP_IMG_MODE_CFG_s *pMode, AMBA_DSP_IMG_CHROMA_FILTER_s *pWideChromaFilter);
int AmbaDSP_ImgGetWideChromaFilter(AMBA_DSP_IMG_MODE_CFG_s *pMode, AMBA_DSP_IMG_CHROMA_FILTER_s *pWideChromaFilter);

int AmbaDSP_ImgSetWideChromaFilterCombine(AMBA_DSP_IMG_MODE_CFG_s *pMode, AMBA_DSP_IMG_HISO_CHROMA_FILTER_COMBINE_s        *pWideChromaFilterCombine);
int AmbaDSP_ImgGetWideChromaFilterCombine(AMBA_DSP_IMG_MODE_CFG_s *pMode, AMBA_DSP_IMG_HISO_CHROMA_FILTER_COMBINE_s        *pWideChromaFilterCombine);

int AmbaDSP_ImgSetGbGrMismatch(AMBA_DSP_IMG_MODE_CFG_s *pMode, AMBA_DSP_IMG_GBGR_MISMATCH_s *pGbGrMismatch);
int AmbaDSP_ImgGetGbGrMismatch(AMBA_DSP_IMG_MODE_CFG_s *pMode, AMBA_DSP_IMG_GBGR_MISMATCH_s *pGbGrMismatch);

/* Warp and MCTF related filters */
int AmbaDSP_ImgCalcWarpCompensation(AMBA_DSP_IMG_MODE_CFG_s *pMode, AMBA_DSP_IMG_WARP_CALC_INFO_s *pWarpCalcInfo);
int AmbaDSP_ImgSetWarpCompensation(AMBA_DSP_IMG_MODE_CFG_s *pMode);
int AmbaDSP_ImgGetWarpCompensation(AMBA_DSP_IMG_MODE_CFG_s *pMode, AMBA_DSP_IMG_WARP_CALC_INFO_s *pWarpCalcInfo);
int AmbaDSP_ImgSetWarpAndCawarpCompensation(AMBA_DSP_IMG_MODE_CFG_s *pMode);

int AmbaDSP_ImgSetVideoMctf(AMBA_DSP_IMG_MODE_CFG_s *pMode, AMBA_DSP_IMG_VIDEO_MCTF_INFO_s *pMctfInfo);
int AmbaDSP_ImgGetVideoMctf(AMBA_DSP_IMG_MODE_CFG_s *pMode, AMBA_DSP_IMG_VIDEO_MCTF_INFO_s *pMctfInfo);

int AmbaDSP_ImgSetVideoMctfTemporalAdjust(AMBA_DSP_IMG_MODE_CFG_s *pMode, AMBA_DSP_IMG_VIDEO_MCTF_TEMPORAL_ADJUST_s*pMctfTemporalAdjust);
int AmbaDSP_ImgGetVideoMctfTemporalAdjust(AMBA_DSP_IMG_MODE_CFG_s *pMode, AMBA_DSP_IMG_VIDEO_MCTF_TEMPORAL_ADJUST_s *pMctfTemporalAdjust);


/* Video HDR related filters */
int AmbaDSP_ImgSetHdrBlendingIndex(AMBA_DSP_IMG_MODE_CFG_s *pMode, AMBA_DSP_IMG_HDR_BLENDING_INFO_s *pHdrBlendingInfo);
/*
int AmbaDSP_ImgSetAmplinearStaticBlackLevel(AMBA_DSP_IMG_MODE_CFG_s *pMode, AMBA_DSP_IMG_HDR_BLKLVL_AMPLINEAR_s *pAmplinearBlackCorr);
int AmbaDSP_ImgGetAmplinearStaticBlackLevel(AMBA_DSP_IMG_MODE_CFG_s *pMode, AMBA_DSP_IMG_HDR_BLKLVL_AMPLINEAR_s *pAmplinearBlackCorr);
int AmbaDSP_ImgSetHdrStaticBlackLevel(AMBA_DSP_IMG_MODE_CFG_s *pMode, AMBA_DSP_IMG_BLACK_CORRECTION_s *pBlackCorr);
int AmbaDSP_ImgGetHdrStaticBlackLevel(AMBA_DSP_IMG_MODE_CFG_s *pMode, AMBA_DSP_IMG_BLACK_CORRECTION_s *pBlackCorr);
*/
int AmbaDSP_ImgSetHdrAlphaCalcConfig(AMBA_DSP_IMG_MODE_CFG_s *pMode, AMBA_DSP_IMG_HDR_ALPHA_CALC_CONFIG_s *pHdrAlphaConfig);
int AmbaDSP_ImgGetHdrAlphaCalcConfig(AMBA_DSP_IMG_MODE_CFG_s *pMode, AMBA_DSP_IMG_HDR_ALPHA_CALC_CONFIG_s *pHdrAlphaConfig);

int AmbaDSP_ImgSetHdrAlphaCalcThreshold(AMBA_DSP_IMG_MODE_CFG_s *pMode, AMBA_DSP_IMG_HDR_ALPHA_CALC_THRESH_s *pHdrAlphaThreshold);
int AmbaDSP_ImgGetHdrAlphaCalcThreshold(AMBA_DSP_IMG_MODE_CFG_s *pMode, AMBA_DSP_IMG_HDR_ALPHA_CALC_THRESH_s *pHdrAlphaThreshold);

int AmbaDSP_ImgSetHdrAlphaCalcBlackLevel(AMBA_DSP_IMG_MODE_CFG_s *pMode, AMBA_DSP_IMG_BLACK_CORRECTION_s *pHdrAlphaBlackCorr);
int AmbaDSP_ImgGetHdrAlphaCalcBlackLevel(AMBA_DSP_IMG_MODE_CFG_s *pMode, AMBA_DSP_IMG_BLACK_CORRECTION_s *pHdrAlphaBlackCorr);

/*
int AmbaDSP_ImgSetHdrAlphaCalcExposure(AMBA_DSP_IMG_MODE_CFG_s *pMode, AMBA_DSP_IMG_HDR_ALPHA_CALC_EXPOSURE_s *pHdrAlpahExposure);
int AmbaDSP_ImgGetHdrAlphaCalcExposure(AMBA_DSP_IMG_MODE_CFG_s *pMode, AMBA_DSP_IMG_HDR_ALPHA_CALC_EXPOSURE_s *pHdrAlpahExposure);
*/

int AmbaDSP_ImgSetHdrAlphaCalcAlpha(AMBA_DSP_IMG_MODE_CFG_s *pMode, AMBA_DSP_IMG_HDR_ALPHA_CALC_ALPHA_s *pHdrAlphaAlpha);
int AmbaDSP_ImgGetHdrAlphaCalcAlpha(AMBA_DSP_IMG_MODE_CFG_s *pMode, AMBA_DSP_IMG_HDR_ALPHA_CALC_ALPHA_s *pHdrAlphaAlpha);

int AmbaDSP_ImgSetAmpLinearization(AMBA_DSP_IMG_MODE_CFG_s *pMode, AMBA_DSP_IMG_AMP_LINEARIZATION_s *pHdrAmpLinearization);
int AmbaDSP_ImgGetAmpLinearization(AMBA_DSP_IMG_MODE_CFG_s *pMode, AMBA_DSP_IMG_AMP_LINEARIZATION_s *pHdrAmpLinearization);

int AmbaDSP_ImgSetContrastEnhance(AMBA_DSP_IMG_MODE_CFG_s *pMode, AMBA_DSP_IMG_CONTRAST_ENHANCE_s *pContrastEnhance);
int AmbaDSP_ImgGetContrastEnhance(AMBA_DSP_IMG_MODE_CFG_s *pMode, AMBA_DSP_IMG_CONTRAST_ENHANCE_s *pContrastEnhance);

int AmbaDSP_ImgSetHdrRawOffset(AMBA_DSP_IMG_MODE_CFG_s *pMode, AMBA_DSP_IMG_HDR_RAW_OFFSET_s *pHdrRawOffset);
int AmbaDSP_ImgGetHdrRawOffset(AMBA_DSP_IMG_MODE_CFG_s *pMode, AMBA_DSP_IMG_HDR_RAW_OFFSET_s *pHdrRawOffset);

int AmbaDSP_ImgSetHdrRawSize(AMBA_DSP_IMG_MODE_CFG_s *pMode, AMBA_DSP_IMG_HDR_RAW_SIZE_s *pHdrRawSize);
int AmbaDSP_ImgGetHdrRawSize(AMBA_DSP_IMG_MODE_CFG_s *pMode, AMBA_DSP_IMG_HDR_RAW_SIZE_s *pHdrRawSize);

int AmbaDSP_ImgSetHdrPostExe(AMBA_DSP_IMG_MODE_CFG_s *pMode);
int AmbaDSP_ImgVideoHdrPrintCfg(AMBA_DSP_IMG_MODE_CFG_s *pMode, AMBA_DSP_IMG_CFG_INFO_s *CfgInfo);
int AmbaDSP_ImgGetHdrCfgStatus(AMBA_DSP_IMG_CFG_INFO_s *pCfgInfo, AMBA_DSP_IMG_HDR_CFG_STATUS_s *pHdrStatus);
int AmbaDSP_ImgDumpHdrDspBlendRaw(AMBA_DSP_IMG_MODE_CFG_s *pMode, UINT32 RawBufferAddr, UINT32 BufferSize, char *Prefix);
int AmbaDSP_ImgGetHdrDspBlendInfoCallBackFunction(void *pHdrBlendInfo);
#endif  /* _AMBA_DSP_IMG_FILTER_H_ */
