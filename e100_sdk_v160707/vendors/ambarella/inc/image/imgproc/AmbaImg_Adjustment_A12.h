 /*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaImg_Adjustment_A9.h
 *
 *  @Copyright      :: Copyright (C) 2013 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Structure type definiton of A9 ADJ
 *
\*-------------------------------------------------------------------------------------------------------------------*/

#ifndef __AMBA_IMG_ADJUSTMENT_A9_H__
#define __AMBA_IMG_ADJUSTMENT_A9_H__

#include "AmbaImg_Adjustment_Def.h"
#include "AmbaDSP_Img3aStatistics.h"
#include "AmbaImg_AeAwb.h"

#define STILL_PARAM_LUT_NO    2

#define VIDEO_TV_TABLE_NO     4
#define VIDEO_PC_TABLE_NO     4
#define PHOTO_TABLE_NO        4

#define AEB_MAX_NUM           9

/* AEB control parameters */
typedef struct _AEB_INFO_s {
    UINT8    Num;             // 1~AEB_MAX_NUM
    INT8     EvBias[AEB_MAX_NUM];    // unit ev is 32, +4 31/32 EV ~ -4 31/32 EV
} AEB_INFO_s;

typedef struct _ADJ_LUT_AGC_WB_s_ {
    UINT8     Enable;
    ADJ_LUT_s LowTab[ADJ_NF_TABLE_COUNT];
    ADJ_LUT_s HighTab[ADJ_NF_TABLE_COUNT];
} ADJ_LUT_AGC_WB_s;

typedef struct _ADJ_BASIC_s_ {
    UINT8            BadPixelEnable;
    ADJ_LUT_s        BadPixel[ADJ_NF_TABLE_COUNT];
    ADJ_LUT_s        AliasingGbGrResampleEnable;
    ADJ_LUT_s        AliasingGbGrResample[ADJ_NF_TABLE_COUNT];
    UINT8            ChromaMedianEnable;
    ADJ_LUT_s        LowChromaMedian[ADJ_NF_TABLE_COUNT];
    ADJ_LUT_s        HighChromaMedian[ADJ_NF_TABLE_COUNT];
    UINT8            DemosaicEnable;
    ADJ_LUT_s        Demosaic[ADJ_NF_TABLE_COUNT];
    ADJ_LUT_AGC_WB_s CfaFilter;
}ADJ_BASIC_s;

typedef struct _FIR_COEFS_s_ {
    INT16 Coefs[9*25];
}FIR_COEFS_s;

typedef struct _SMOOTH_SELECT_s{
    UINT8 SmoothSelect[256];
}SMOOTH_SELECT_s;

typedef struct _DEF_FIR_s_ {
    ADJ_LUT_s   FirDirAmtStr[ADJ_NF_TABLE_COUNT];
    ADJ_LUT_s   FirIsoStr[ADJ_NF_TABLE_COUNT];
    FIR_COEFS_s FirCoefs[ADJ_NF_TABLE_COUNT];
} DEF_FIR_s;

typedef struct _DEF_SHARP_s_ {
    ADJ_LUT_s             FinShpNfBoth[ADJ_NF_TABLE_COUNT];
    ADJ_LUT_s             FinShpNfNLevelStrAdjust[ADJ_NF_TABLE_COUNT];
    DEF_FIR_s             FinShpNfN;

    DEF_FIR_s             FinShpNfS;
    AMBA_DSP_IMG_CORING_s FinShpNfSCoring[ADJ_NF_TABLE_COUNT];
    ADJ_LUT_s             FinShpNfSCoringIdxScale[ADJ_NF_TABLE_COUNT];
    ADJ_LUT_s             FinShpNfSMinCoringIdxResult[ADJ_NF_TABLE_COUNT];
    ADJ_LUT_s             FinShpNfSScaleCoring[ADJ_NF_TABLE_COUNT];

} DEF_SHARP_s;

typedef struct _DEF_ASF_INFO_s_ {
    UINT8       Enable;
    ADJ_LUT_s   SpatialFilter[ADJ_HISO_NF_TABLE_COUNT];
    ADJ_LUT_s   SpatialT0T1Div[ADJ_HISO_NF_TABLE_COUNT];
    ADJ_LUT_s   SpatialLevelStrAdjust[ADJ_HISO_NF_TABLE_COUNT];
    DEF_FIR_s   SpatialFir;
} DEF_ASF_INFO_s;

//karl A12 add li_wide_chroma_filter_combine
typedef struct _LI_WIDE_CHROMA_FILTER_COMBINE_s_{
    ADJ_LUT_s LiWideChromaCombineLowTab[ADJ_NF_TABLE_COUNT];
    ADJ_LUT_s LiWideChromaCombineHighTab[ADJ_NF_TABLE_COUNT];
} LI_WIDE_CHROMA_FILTER_COMBINE_s;
    
typedef struct _CHROMA_FILTER_COMBINE_s_ {
    UINT8       Enable;
//karl A12 remove    ADJ_LUT_s   ChromaFltComb0[ADJ_HISO_NF_TABLE_COUNT];
    ADJ_LUT_s   ChromaFltComb[ADJ_HISO_NF_TABLE_COUNT];
} CHROMA_FILTER_COMBINE_s;

typedef struct _LUMA_COMBINE_s_ {
    UINT8       Enable;
    ADJ_LUT_s   LumaComb[ADJ_HISO_NF_TABLE_COUNT];
} LUMA_COMBINE_s;

typedef struct _DEF_SHARP_INFO_s_ {
//karl A12 remove    UINT8       CdnrEnable;
//karl A12 remove    ADJ_LUT_s   CdnrLut[ADJ_NF_TABLE_COUNT];
//karl A12 remove    ADJ_LUT_s   ShpANotAsf[ADJ_NF_TABLE_COUNT];
//karl A12 add  
//    UINT8       LiProcessingSeletEnable;  
//    ADJ_LUT_s   LiProcessingSelect[ADJ_NF_TABLE_COUNT];    //li_color_dependent_luma_noise_reduction.enable also in this ADJ_LUT_s

    UINT8       ShpEnable;
    ADJ_LUT_s   SpatialFilter[ADJ_NF_TABLE_COUNT];
    ADJ_LUT_s   SpatialT0T1Div[ADJ_NF_TABLE_COUNT];
    ADJ_LUT_s   SpatialLevelStrAdjust[ADJ_NF_TABLE_COUNT];
    DEF_FIR_s   DefSharpAFir;
    DEF_SHARP_s DefSharpA;
//karl A12 remove    DEF_SHARP_s DefSharpB;
} DEF_SHARP_INFO_s;

typedef struct _VIDEO_FILTER_PARAM_s_ {
    ADJ_FILTER_INFO_s EvImg;                         //ADJ DEF
    UINT8             NfMaxTableCount;
    ADJ_DEF_s         Def;                           //ADJ DEF

    ADJ_BASIC_s       Basic;
    UINT8             MctfEnable;
    ADJ_LUT_s         LowMctfA[ADJ_NF_TABLE_COUNT];
    ADJ_LUT_s         HighMctfA[ADJ_NF_TABLE_COUNT];
    ADJ_LUT_s         LowMctfB[ADJ_NF_TABLE_COUNT];
    ADJ_LUT_s         HighMctfB[ADJ_NF_TABLE_COUNT];
    ADJ_LUT_s         LowMctfC[ADJ_NF_TABLE_COUNT]; //DSP filter TA
    ADJ_LUT_s         HighMctfC[ADJ_NF_TABLE_COUNT];	
    ADJ_LUT_AGC_WB_s  ChromaFilter;
//karl A12 add       
    UINT8             LiWideChromaFilterCombineEnable;
    LI_WIDE_CHROMA_FILTER_COMBINE_s        LiWideChromaFilterCombine;    

//karl A12 add  
    UINT8       LiProcessingSeletEnable;  
    ADJ_LUT_s   LiProcessingSelect[ADJ_NF_TABLE_COUNT];    //li_color_dependent_luma_noise_reduction.enable also in this ADJ_LUT_s  
  
    DEF_SHARP_INFO_s  SharpInfo;
} VIDEO_FILTER_PARAM_s;

/****************************************************************/
/*****        Video Adjustment parameters Structure         *****/
/****************************************************************/
typedef struct _ADJ_VIDEO_PARAM_s_ {
    UINT32               VersionNumber;
    UINT32               ParamVersionNum;
    ADJ_AWB_AE_s         AwbAe;                       //ADJ DEF
    VIDEO_FILTER_PARAM_s FilterParam;
} ADJ_VIDEO_PARAM_s;
#define ADJ_PARAM_VER (0x15010500)

/****************************************************************/
/*****        PHOTO Adjustment parameters Structure         *****/
/****************************************************************/
typedef struct _ADJ_PHOTO_PARAM_s_ {
    UINT32               VersionNumber;
    UINT32               ParamVersionNum;
    ADJ_AWB_AE_s         NormalAwbAe;                 //ADJ DEF
    ADJ_AWB_AE_s         FlashAwbAe;                  //ADJ DEF
    VIDEO_FILTER_PARAM_s FilterParam;
} ADJ_PHOTO_PARAM_s;

/****************************************************************/
/*****        STILL Adjustment parameters Structure         *****/
/****************************************************************/
typedef struct _ADJ_STILL_FAST_LISO_PARAM_s {

    UINT32               VersionNum;
    UINT32               ParamVersionNum;
    ////////////////////////////////////////////
    UINT8                NfMaxTableCount;
    ADJ_FILTER_INFO_s    NormalEvImg;
    ADJ_FILTER_INFO_s    FlashEvImg;
    ADJ_DEF_s            Def;
    ////////////////////////////////////////////
    ADJ_BASIC_s          Basic;

    ADJ_LUT_AGC_WB_s     ChromaFilter;
//karl add    
    UINT8             LiWideChromaFilterCombineEnable;
    LI_WIDE_CHROMA_FILTER_COMBINE_s        LiWideChromaFilterCombine;      
    
//karl A12 add  
    UINT8       LiProcessingSeletEnable;  
    ADJ_LUT_s   LiProcessingSelect[ADJ_NF_TABLE_COUNT];    //li_color_dependent_luma_noise_reduction.enable also in this ADJ_LUT_s     
    
    DEF_SHARP_INFO_s     SharpInfo;
    //ADJ_STR_CONTROL_s        DzoomControl;

} ADJ_STILL_FAST_LISO_PARAM_S;

typedef ADJ_STILL_FAST_LISO_PARAM_S    ADJ_STILL_FAST_PARAM_s;
#define ADJ_STILL_FAST_PARAM_VER    (0x14111100)
typedef ADJ_STILL_FAST_LISO_PARAM_S    ADJ_STILL_LOW_ISO_PARAM_s;
#define ADJ_STILL_LOW_ISO_PARAM_VER (0x14111100)


typedef struct _ADJ_STILL_HISO_PARAM_s_ {

    UINT32               VersionNum;
    UINT32               ParamVersionNum;
    ////////////////////////////////////////////
    UINT8                NfMaxTableCount;
    ADJ_FILTER_INFO_s    NormalEvImg;
    ADJ_FILTER_INFO_s    FlashEvImg;
    ADJ_DEF_s            Def;
    ////////////////////////////////////////////
    ADJ_BASIC_s          Basic;

    ADJ_LUT_AGC_WB_s     ChromaFilter;
    //DEF_SHARP_INFO_s     SharpInfo;
    //ADJ_STR_CONTROL_s        DzoomControl;
    //////////////////////////////////////////// Start of HISO
    ADJ_HISO_FILTER_INFO_s HIsoNormalEvImg;
    ADJ_HISO_FILTER_INFO_s HIsoFlashEvImg;
    ADJ_BASIC_s            HIsoBasic;

//karl A12 remove    
//    UINT8                  HIsoCdnrEnable;
//    ADJ_LUT_s              HIsoCdnrLut[ADJ_HISO_NF_TABLE_COUNT];

    DEF_ASF_INFO_s         HIsoAsf;
    DEF_ASF_INFO_s         HIsoHighAsf;
    DEF_ASF_INFO_s         HIsoMedAsf;
//karl A12 remove    DEF_ASF_INFO_s         HIsoMed2Asf;
    DEF_ASF_INFO_s         HIsoLowAsf;

    DEF_SHARP_s            HIsoHighSharp;
    DEF_SHARP_s            HIsoMedSharp;
//karl A12 remove    DEF_SHARP_s            HIsoLiSharp;

    DEF_ASF_INFO_s         HIsoChromaAsf;

    ADJ_LUT_AGC_WB_s       HIsoChromaFilterPre;
    ADJ_LUT_AGC_WB_s       HIsoChromaFilterHigh;
    ADJ_LUT_AGC_WB_s       HIsoChromaFilterMed;
    ADJ_LUT_AGC_WB_s       HIsoChromaFilterLow;
    ADJ_LUT_AGC_WB_s       HIsoChromaFilterVLow;
    ADJ_LUT_AGC_WB_s       HIsoChromaFilterLowAndVLow;

    CHROMA_FILTER_COMBINE_s HIsoChromaFilterMedCombine;
    CHROMA_FILTER_COMBINE_s HIsoChromaFilterLowCombine;
    CHROMA_FILTER_COMBINE_s HIsoChromaFilterVLowCombine;

    LUMA_COMBINE_s         HIsoLumaFilterCombine;
    LUMA_COMBINE_s         HIsoLowAsfCombine;

 //karl A12 remove    CHROMA_FILTER_COMBINE_s HIsoLiCombine;

    UINT8                  HIsoLiLumaMidHightFreqRcvrEnable;
    DEF_FIR_s              HIsoLiLumaMidHightFreqRcvr;
    SMOOTH_SELECT_s        HIsoLiLumaMidHightFreqRcvrSmoothSelect[ADJ_NF_TABLE_COUNT];
    
//karl A12 remove    UINT8                  HIsoLi2ndBlendEnable;
//karl A12 remove    ADJ_LUT_s              HIsoLi2ndBlend[ADJ_HISO_NF_TABLE_COUNT];

 //karl A12 remove    DEF_ASF_INFO_s         Li2ndAsf;
 //karl A12 remove   DEF_SHARP_s            Li2ndSharp;



} ADJ_STILL_HISO_PARAM_s;
#define ADJ_STILL_HIGH_ISO_PARAM_VER (0x15032500)

typedef struct _VIDEO_HISO_FILTER_PARAM_s_{
    ADJ_FILTER_INFO_s    EvImg;  
    UINT8                NfMaxTableCount;
    ADJ_DEF_s            Def;

    ADJ_BASIC_s          Basic;
    UINT8                MctfEnable;
    ADJ_LUT_s            LowMctfA[ADJ_NF_TABLE_COUNT];
    ADJ_LUT_s            HighMctfA[ADJ_NF_TABLE_COUNT];
    ADJ_LUT_s            LowMctfB[ADJ_NF_TABLE_COUNT];
    ADJ_LUT_s            HighMctfB[ADJ_NF_TABLE_COUNT];    
    ADJ_LUT_AGC_WB_s     ChromaFilter;

    DEF_SHARP_INFO_s     SharpInfo;
    //////////////////////////////////////////// Start of HISO
    ADJ_HISO_FILTER_INFO_s  HIsoNormalEvImg;
    ADJ_BASIC_s             HIsoBasic;
                            
    DEF_ASF_INFO_s          HIsoAsf;
    DEF_ASF_INFO_s          HIsoHighAsf;
    DEF_ASF_INFO_s          HIsoMedAsf;
                            
    DEF_ASF_INFO_s          HIsoLowAsf;
                            
    DEF_SHARP_s             HIsoHighSharp;
    DEF_SHARP_s             HIsoMedSharp;
                            
    DEF_ASF_INFO_s          HIsoChromaAsf;
                            
    ADJ_LUT_AGC_WB_s        HIsoChromaFilterPre;
    ADJ_LUT_AGC_WB_s        HIsoChromaFilterHigh;
    ADJ_LUT_AGC_WB_s        HIsoChromaFilterMed;
    ADJ_LUT_AGC_WB_s        HIsoChromaFilterLow;
    ADJ_LUT_AGC_WB_s        HIsoChromaFilterVLow;
    ADJ_LUT_AGC_WB_s        HIsoChromaFilterLowAndVLow;

    CHROMA_FILTER_COMBINE_s HIsoChromaFilterMedCombine;
    CHROMA_FILTER_COMBINE_s HIsoChromaFilterLowCombine;
    CHROMA_FILTER_COMBINE_s HIsoChromaFilterVLowCombine;

    LUMA_COMBINE_s          HIsoLumaFilterCombine;
    LUMA_COMBINE_s          HIsoLowAsfCombine;
                            
    UINT8                   HIsoLiLumaMidHightFreqRcvrEnable;
    DEF_FIR_s               HIsoLiLumaMidHightFreqRcvr;
    SMOOTH_SELECT_s         HIsoLiLumaMidHightFreqRcvrSmoothSelect[ADJ_NF_TABLE_COUNT];

} VIDEO_HISO_FILTER_PARAM_s;

typedef struct _ADJ_VIDEO_HISO_PARAM_s_ {
    UINT32               VersionNumber;
    UINT32               ParamVersionNum;
    ADJ_AWB_AE_s         NormalAwbAe;                 //ADJ DEF
    VIDEO_HISO_FILTER_PARAM_s FilterParam;      
}ADJ_VIDEO_HISO_PARAM_s;

#define ADJ_VIDEO_HIGH_ISO_PARAM_VER (0x14111100)




typedef struct _ADJ_SHARP_FILTER_s_ {
    UINT8                                     SharpenBothUpdate;
    AMBA_DSP_IMG_SHARPEN_BOTH_s               SharpenBoth;
    UINT8                                     SharpenNoiseUpdate;
    AMBA_DSP_IMG_SHARPEN_NOISE_s              SharpenNoise;
    UINT8                                     SharpenFirUpdate;
    AMBA_DSP_IMG_FIR_s                        SharpenFir;
    UINT8                                     SharpenCoringUpdate;
    AMBA_DSP_IMG_CORING_s                     SharpenCoring;
    UINT8                                     SharpenCoringIndexScaleUpdate;
    AMBA_DSP_IMG_LEVEL_s                      SharpenCoringIndexScale;
    UINT8                                     SharpenMinCoringResultUpdate;
    AMBA_DSP_IMG_LEVEL_s                      SharpenMinCoringResult;
    UINT8                                     SharpenScaleCoringUpdate;
    AMBA_DSP_IMG_LEVEL_s                      SharpenScaleCoring;
} ADJ_SHARP_FILTER_s;



typedef struct _HDR_INFO_s_ {
    AMBA_AE_INFO_s           AeInfo[3];    // 0:long 1:short 2:very short
    AMBA_DSP_IMG_WB_GAIN_s   WbGain[3];    // 0:long 1:short 2:very short
} HDR_INFO_s;

typedef struct _GAMMA_INFO_s_ {
    double                   Value;
} GAMMA_INFO_s;
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

/**
 * pipeline control for AAA algorithms
 */
 typedef struct _PIPELINE_CONTROL_s_ {
    UINT8                                  ModeUpdate;
//karl SchdlrExpInfo WbGain will remove when a12
    UINT8                                  WbGainUpdate;
    AMBA_DSP_IMG_WB_GAIN_s                 WbGain;
    
    //
    //UINT8                                  VsyncExposureUpdate;
    //UINT8                                  ExposureUpdate;
    //AMBA_AE_INFO_s                         ExposureInfo;
    STILL_AF_INFO_s                        AfInfoStill;    /* for showing SAF focused areas on GUI after FLOCK */

    UINT8                                  RgbYuvMatrixUpdate;
    AMBA_DSP_IMG_RGB_TO_YUV_s              RgbYuvMatrix;

    /* CFA domain filters */
    UINT8                                  BlackCorrUpdate;                   //vp_01
    AMBA_DSP_IMG_BLACK_CORRECTION_s        BlackCorr;
    UINT8                                  AntiAliasingUpdate;                //vp_02
    AMBA_DSP_IMG_ANTI_ALIASING_s           AntiAliasing;
    UINT8                                  BadpixCorrUpdate;                  //vp_03
    AMBA_DSP_IMG_DBP_CORRECTION_s          BadpixCorr;

    UINT8                                  CfaFilterUpdate;                   //vp_04
    AMBA_DSP_IMG_CFA_NOISE_FILTER_s        CfaFilter;
    UINT8                                  LocalExposureUpdate;               //vp_05
    AMBA_DSP_IMG_LOCAL_EXPOSURE_s          LocalExposure;

    /* RGB domain filters */
    UINT8                                  DemosaicUpdate;                    //vp_06
    AMBA_DSP_IMG_DEMOSAIC_s                Demosaic;

    //UINT8                                  ColorCorrRegUpdate;
    //AMBA_DSP_IMG_COLOR_CORRECTION_REG_s    ColorCorrReg;
    UINT8                                  ColorCorrUpdate;                   //vp_07
    AMBA_DSP_IMG_COLOR_CORRECTION_s        ColorCorr;

    UINT8                                  GammaUpdate;                   //vp_08
    AMBA_DSP_IMG_TONE_CURVE_s              GammaTable;

    //UINT8                                  SpecificCcUpdate;
    //AMBA_DSP_IMG_SPECIFIG_CC_s             SpecificCc;

    /* Y domain filters */
    UINT8                                  ChromaScaleUpdate;                 //vp_09
    AMBA_DSP_IMG_CHROMA_SCALE_s            ChromaScale;
    UINT8                                  ChromaMedianUpdate;                //vp_10
    AMBA_DSP_IMG_CHROMA_MEDIAN_FILTER_s    ChromaMedian;

//karl >>>>>>>>>>>>>>>check below
    
//karl add li_processing_select    
    UINT8                                  LisoProcessSelectUpdate;
    AMBA_DSP_IMG_LISO_PROCESS_SELECT_s     LisoProcessSelect;
    UINT8                                  LiColorDependentLumaNoiseReductionUpdate;
    AMBA_DSP_IMG_CDNR_INFO_s               LiColorDependentLumaNoiseReduction;
    
    //A12 removeUINT8                                  ShpSelectUpdate;                  //vp_11
    //A12 remove      AMBA_DSP_IMG_SHP_A_SELECT_e            ShpASelect;

    UINT8                                  AsfUpdate;                         //vp_12
    AMBA_DSP_IMG_ASF_INFO_s                Asf;

    UINT8                                  SharpenBothUpdate;                //vp_13
    AMBA_DSP_IMG_SHARPEN_BOTH_s            SharpenBoth;
    UINT8                                  SharpenNoiseUpdate;               //vp_14
    AMBA_DSP_IMG_SHARPEN_NOISE_s           SharpenNoise;
    UINT8                                  SharpenFirUpdate;                 //vp_15
    AMBA_DSP_IMG_FIR_s                     SharpenFir;
    UINT8                                  SharpenCoringUpdate;              //vp_16
    AMBA_DSP_IMG_CORING_s                  SharpenCoring;
    UINT8                                  SharpenCoringIndexScaleUpdate;    //vp_17
    AMBA_DSP_IMG_LEVEL_s                   SharpenCoringIndexScale;
    UINT8                                  SharpenMinCoringResultUpdate;     //vp_18
    AMBA_DSP_IMG_LEVEL_s                   SharpenMinCoringResult;
    UINT8                                  SharpenScaleCoringUpdate;         //vp_19
    AMBA_DSP_IMG_LEVEL_s                   SharpenScaleCoring;

    //UINT8                                  SharpenBLinStrengthUpdate;
    //UINT16                                 SharpenBLinStrength;
    UINT8                                  ChromaFilterUpdate;                //vp_27
    AMBA_DSP_IMG_CHROMA_FILTER_s           ChromaFilter;
    UINT8                                  GbGrMismatchUpdate;                //vp_28
    AMBA_DSP_IMG_GBGR_MISMATCH_s           GbGrMismatch;

    UINT8                                  ResamplerStrUpdate;
    AMBA_DSP_IMG_RESAMPLER_STR_s           ResamplerStr;
    
//karl add li_wide_chroma_filter_combine    
    UINT8                                  LiWideChromaFilterCombineUpdate;
    AMBA_DSP_IMG_HISO_CHROMA_FILTER_COMBINE_s        LiWideChromaFilterCombine;
    
    /* Warp and MCTF related filters */
    UINT8                                  MctfInfoUpdate;                    //vp_29
    AMBA_DSP_IMG_VIDEO_MCTF_INFO_s         MctfInfo;
    AMBA_DSP_IMG_VIDEO_MCTF_TEMPORAL_ADJUST_s VideoMctfTemporalAdjust;

//karl ????  no li_color_dependent_luma_noise_reduction.enable    
    
   /* Start of Video HISO filter */    
    UINT8                                  HIsoCfaLeakageFilterUpdate;
    AMBA_DSP_IMG_CFA_LEAKAGE_FILTER_s      HIsoCfaLeakageFilter;

    UINT8                                  HIsoAntiAliasingUpdate;
    AMBA_DSP_IMG_ANTI_ALIASING_s           HIsoAntiAliasing;
    UINT8                                  HIsoBadpixCorrUpdate;
    AMBA_DSP_IMG_DBP_CORRECTION_s          HIsoBadpixCorr;
    UINT8                                  HIsoCfaFilterUpdate;
    AMBA_DSP_IMG_CFA_NOISE_FILTER_s        HIsoCfaFilter;
    UINT8                                  HIsoGbGrMismatchUpdate;
    AMBA_DSP_IMG_GBGR_MISMATCH_s           HIsoGbGrMismatch;
    UINT8                                  HIsoDemosaicUpdate;
    AMBA_DSP_IMG_DEMOSAIC_s                HIsoDemosaic;
    UINT8                                  HIsoChromaMedianUpdate;
    AMBA_DSP_IMG_CHROMA_MEDIAN_FILTER_s    HIsoChromaMedian;
    UINT8                                  HIsoCdnrUpdate;
    AMBA_DSP_IMG_CDNR_INFO_s               HIsoCdnr;
    UINT8                                  HIsoDeferColorCorrUpdate;
    AMBA_DSP_IMG_DEFER_COLOR_CORRECTION_s  HIsoDeferColorCorr;

    //HISO Luma ASF
    UINT8                                  HIsoAsfUpdate;
    AMBA_DSP_IMG_ASF_INFO_s                HIsoAsf;
    UINT8                                  HIsoHighAsfUpdate;
    AMBA_DSP_IMG_ASF_INFO_s                HIsoHighAsf;
    UINT8                                  HIsoMedAsfUpdate;
    AMBA_DSP_IMG_ASF_INFO_s                HIsoMedAsf;

    UINT8                                  HIsoLowAsfUpdate;
    AMBA_DSP_IMG_ASF_INFO_s                HIsoLowAsf;

    //High Sharpen
    UINT8                                  HIsoHighSharpenBothUpdate;
    AMBA_DSP_IMG_SHARPEN_BOTH_s            HIsoHighSharpenBoth;
    UINT8                                  HIsoHighSharpenNoiseUpdate;
    AMBA_DSP_IMG_SHARPEN_NOISE_s           HIsoHighSharpenNoise;
    UINT8                                  HIsoHighSharpenFirUpdate;
    AMBA_DSP_IMG_FIR_s                     HIsoHighSharpenFir;
    UINT8                                  HIsoHighSharpenCoringUpdate;
    AMBA_DSP_IMG_CORING_s                  HIsoHighSharpenCoring;
    UINT8                                  HIsoHighSharpenCoringIndexScaleUpdate;
    AMBA_DSP_IMG_LEVEL_s                   HIsoHighSharpenCoringIndexScale;
    UINT8                                  HIsoHighSharpenMinCoringResultUpdate;
    AMBA_DSP_IMG_LEVEL_s                   HIsoHighSharpenMinCoringResult;
    UINT8                                  HIsoHighSharpenScaleCoringUpdate;
    AMBA_DSP_IMG_LEVEL_s                   HIsoHighSharpenScaleCoring;

    //Med Sharpen
    UINT8                                  HIsoMedSharpenBothUpdate;
    AMBA_DSP_IMG_SHARPEN_BOTH_s            HIsoMedSharpenBoth;
    UINT8                                  HIsoMedSharpenNoiseUpdate;
    AMBA_DSP_IMG_SHARPEN_NOISE_s           HIsoMedSharpenNoise;
    UINT8                                  HIsoMedSharpenFirUpdate;
    AMBA_DSP_IMG_FIR_s                     HIsoMedSharpenFir;
    UINT8                                  HIsoMedSharpenCoringUpdate;
    AMBA_DSP_IMG_CORING_s                  HIsoMedSharpenCoring;
    UINT8                                  HIsoMedSharpenCoringIndexScaleUpdate;
    AMBA_DSP_IMG_LEVEL_s                   HIsoMedSharpenCoringIndexScale;
    UINT8                                  HIsoMedSharpenMinCoringResultUpdate;
    AMBA_DSP_IMG_LEVEL_s                   HIsoMedSharpenMinCoringResult;
    UINT8                                  HIsoMedSharpenScaleCoringUpdate;
    AMBA_DSP_IMG_LEVEL_s                   HIsoMedSharpenScaleCoring;

    //HISO Chroma ASF
    UINT8                                  HIsoChromaAsfUpdate;
    AMBA_DSP_IMG_CHROMA_ASF_INFO_s         HIsoChromaAsf;

    //HISO Chroma filter
    UINT8                                               HIsoChromaFilterPreUpdate;
    AMBA_DSP_IMG_HISO_CHROMA_FILTER_s                   HIsoChromaFilterPre;
    UINT8                                               HIsoChromaFilterHighUpdate;
    AMBA_DSP_IMG_CHROMA_FILTER_s                        HIsoChromaFilterHigh;
    UINT8                                               HIsoChromaFilterMedUpdate;
    AMBA_DSP_IMG_HISO_CHROMA_FILTER_s                   HIsoChromaFilterMed;
    UINT8                                               HIsoChromaFilterLowUpdate;
    AMBA_DSP_IMG_HISO_CHROMA_FILTER_s                   HIsoChromaFilterLow;
    UINT8                                               HIsoChromaFilterVLowUpdate;
    AMBA_DSP_IMG_HISO_CHROMA_FILTER_s                   HIsoChromaFilterVLow;
    UINT8                                               HIsoChromaFilterLowAndVLowUpdate;
    AMBA_DSP_IMG_HISO_CHROMA_LOW_VERY_LOW_FILTER_s      HIsoChromaFilterLowAndVLow;

    //HISO Chroma Combine
    UINT8                                     HIsoChromaFilterMedCombineUpdate;
    AMBA_DSP_IMG_HISO_CHROMA_FILTER_COMBINE_s HIsoChromaFilterMedCombine;
    UINT8                                     HIsoChromaFilterLowCombineUpdate;
    AMBA_DSP_IMG_HISO_CHROMA_FILTER_COMBINE_s HIsoChromaFilterLowCombine;
    UINT8                                     HIsoChromaFilterVLowCombineUpdate;
    AMBA_DSP_IMG_HISO_CHROMA_FILTER_COMBINE_s HIsoChromaFilterVLowCombine;

    //HISO Luma Combine
    UINT8                                   HIsoLumaFilterCombineUpdate;
    AMBA_DSP_IMG_HISO_LUMA_FILTER_COMBINE_s HIsoLumaFilterCombine;
    UINT8                                   HIsoLowAsfCombineUpdate;
    AMBA_DSP_IMG_HISO_LUMA_FILTER_COMBINE_s HIsoLowAsfCombine;

    //LISO/HISO Combine
 //karl A12 remove    UINT8                                     HIsoLiCombineUpdate;
 //karl A12 remove    AMBA_DSP_IMG_HISO_COMBINE_s               HIsoLiCombine;

    UINT8                                     HIsoLiLumaMidHighFreqRecoverUpdate;
    AMBA_DSP_IMG_HISO_FREQ_RECOVER_s          HIsoLiLumaMidHighFreqRecover;    
    
   /* End of Video HISO filter */ 
   
   
   /* Start of Video HDR filter */ 

   UINT8                                  HdrBlendingInfoUpdate;
   AMBA_DSP_IMG_HDR_BLENDING_INFO_s       HdrBlendingInfo;
   UINT8                                  HdrAlphaConfig0Update;
   AMBA_DSP_IMG_HDR_ALPHA_CALC_CONFIG_s   HdrAlphaConfig0;
   UINT8                                  HdrAlphaThreshold0Update;
   AMBA_DSP_IMG_HDR_ALPHA_CALC_THRESH_s   HdrAlphaThreshold0;
   UINT8                                  HdrAmpLinearization0Update;
   AMBA_DSP_IMG_AMP_LINEARIZATION_s       HdrAmpLinearization0;
   /* End of Video HDR filter */ 

} PIPELINE_CONTROL_s;

typedef struct _PIPELINE_STILL_CONTROL_s_ {

    UINT8                                  StillMode;
    UINT8                                  ModeUpdate;

    UINT8                                  WbGainUpdate;
    AMBA_DSP_IMG_WB_GAIN_s                 WbGain;

    UINT8                                  VsyncExposureUpdate;
    UINT8                                  ExposureUpdate;
    AMBA_AE_INFO_s                         ExposureInfo;
    STILL_AF_INFO_s                        AfInfoStill;    /* for showing SAF focused areas on GUI after FLOCK */

    UINT8                                  RgbYuvMatrixUpdate;
    AMBA_DSP_IMG_RGB_TO_YUV_s              RgbYuvMatrix;

    /* CFA domain filters */
    UINT8                                  BlackCorrUpdate;
    AMBA_DSP_IMG_BLACK_CORRECTION_s        BlackCorr;
    UINT8                                  AntiAliasingUpdate;
    AMBA_DSP_IMG_ANTI_ALIASING_s           AntiAliasing;
    UINT8                                  BadpixCorrUpdate;
    AMBA_DSP_IMG_DBP_CORRECTION_s          BadpixCorr;

    UINT8                                  CfaFilterUpdate;
    AMBA_DSP_IMG_CFA_NOISE_FILTER_s        CfaFilter;
    UINT8                                  LocalExposureUpdate;
    AMBA_DSP_IMG_LOCAL_EXPOSURE_s          LocalExposure;

    /* RGB domain filters */
    UINT8                                  DemosaicUpdate;
    AMBA_DSP_IMG_DEMOSAIC_s                Demosaic;

    //UINT8                                  ColorCorrRegUpdate;
    //AMBA_DSP_IMG_COLOR_CORRECTION_REG_s    ColorCorrReg;
    UINT8                                  ColorCorrUpdate;
    AMBA_DSP_IMG_COLOR_CORRECTION_s        ColorCorr;

    UINT8                                  GammaUpdate;
    AMBA_DSP_IMG_TONE_CURVE_s              GammaTable;

    //UINT8                                  SpecificCcUpdate;
    //AMBA_DSP_IMG_SPECIFIG_CC_s             SpecificCc;

    /* Y domain filters */
    UINT8                                  ChromaScaleUpdate;
    AMBA_DSP_IMG_CHROMA_SCALE_s            ChromaScale;
    UINT8                                  ChromaMedianUpdate;
    AMBA_DSP_IMG_CHROMA_MEDIAN_FILTER_s    ChromaMedian;

//karl add li_processing_select    
    UINT8                                  LisoProcessSelectUpdate;
    AMBA_DSP_IMG_LISO_PROCESS_SELECT_s     LisoProcessSelect;
    UINT8                                  LiColorDependentLumaNoiseReductionUpdate;
    AMBA_DSP_IMG_CDNR_INFO_s               LiColorDependentLumaNoiseReduction;    
    
    //UINT8                                  ShpASelectUpdate;
    //AMBA_DSP_IMG_SHP_A_SELECT_e            ShpASelect;
    
    UINT8                                  AsfUpdate;
    AMBA_DSP_IMG_ASF_INFO_s                Asf;

    UINT8                                  SharpenBothUpdate;
    AMBA_DSP_IMG_SHARPEN_BOTH_s            SharpenBoth;
    UINT8                                  SharpenNoiseUpdate;
    AMBA_DSP_IMG_SHARPEN_NOISE_s           SharpenNoise;
    UINT8                                  SharpenFirUpdate;
    AMBA_DSP_IMG_FIR_s                     SharpenFir;
    UINT8                                  SharpenCoringUpdate;
    AMBA_DSP_IMG_CORING_s                  SharpenCoring;
    UINT8                                  SharpenCoringIndexScaleUpdate;
    AMBA_DSP_IMG_LEVEL_s                   SharpenCoringIndexScale;
    UINT8                                  SharpenMinCoringResultUpdate;
    AMBA_DSP_IMG_LEVEL_s                   SharpenMinCoringResult;
    UINT8                                  SharpenScaleCoringUpdate;
    AMBA_DSP_IMG_LEVEL_s                   SharpenScaleCoring;

    //UINT8                                  SharpenBLinStrengthUpdate;
    //UINT16                                 SharpenBLinStrength;
    UINT8                                  ChromaFilterUpdate;
    AMBA_DSP_IMG_CHROMA_FILTER_s           ChromaFilter;
    UINT8                                  GbGrMismatchUpdate;
    AMBA_DSP_IMG_GBGR_MISMATCH_s           GbGrMismatch;

//karl add li_wide_chroma_filter_combine    
    UINT8                                  LiWideChromaFilterCombineUpdate;
    AMBA_DSP_IMG_HISO_CHROMA_FILTER_COMBINE_s        LiWideChromaFilterCombine;    


    /* Start of HISO filter */
    UINT8                                  HIsoCfaLeakageFilterUpdate;
    AMBA_DSP_IMG_CFA_LEAKAGE_FILTER_s      HIsoCfaLeakageFilter;

    UINT8                                  HIsoAntiAliasingUpdate;
    AMBA_DSP_IMG_ANTI_ALIASING_s           HIsoAntiAliasing;
    UINT8                                  HIsoBadpixCorrUpdate;
    AMBA_DSP_IMG_DBP_CORRECTION_s          HIsoBadpixCorr;
    UINT8                                  HIsoCfaFilterUpdate;
    AMBA_DSP_IMG_CFA_NOISE_FILTER_s        HIsoCfaFilter;
    UINT8                                  HIsoGbGrMismatchUpdate;
    AMBA_DSP_IMG_GBGR_MISMATCH_s           HIsoGbGrMismatch;
    UINT8                                  HIsoDemosaicUpdate;
    AMBA_DSP_IMG_DEMOSAIC_s                HIsoDemosaic;
    UINT8                                  HIsoChromaMedianUpdate;
    AMBA_DSP_IMG_CHROMA_MEDIAN_FILTER_s    HIsoChromaMedian;
    UINT8                                  HIsoCdnrUpdate;
    AMBA_DSP_IMG_CDNR_INFO_s               HIsoCdnr;
    UINT8                                  HIsoDeferColorCorrUpdate;
    AMBA_DSP_IMG_DEFER_COLOR_CORRECTION_s  HIsoDeferColorCorr;

    //HISO Luma ASF
    UINT8                                  HIsoAsfUpdate;
    AMBA_DSP_IMG_ASF_INFO_s                HIsoAsf;
    UINT8                                  HIsoHighAsfUpdate;
    AMBA_DSP_IMG_ASF_INFO_s                HIsoHighAsf;
    UINT8                                  HIsoMedAsfUpdate;
    AMBA_DSP_IMG_ASF_INFO_s                HIsoMedAsf;

    UINT8                                  HIsoLowAsfUpdate;
    AMBA_DSP_IMG_ASF_INFO_s                HIsoLowAsf;

    //High Sharpen
    UINT8                                  HIsoHighSharpenBothUpdate;
    AMBA_DSP_IMG_SHARPEN_BOTH_s            HIsoHighSharpenBoth;
    UINT8                                  HIsoHighSharpenNoiseUpdate;
    AMBA_DSP_IMG_SHARPEN_NOISE_s           HIsoHighSharpenNoise;
    UINT8                                  HIsoHighSharpenFirUpdate;
    AMBA_DSP_IMG_FIR_s                     HIsoHighSharpenFir;
    UINT8                                  HIsoHighSharpenCoringUpdate;
    AMBA_DSP_IMG_CORING_s                  HIsoHighSharpenCoring;
    UINT8                                  HIsoHighSharpenCoringIndexScaleUpdate;
    AMBA_DSP_IMG_LEVEL_s                   HIsoHighSharpenCoringIndexScale;
    UINT8                                  HIsoHighSharpenMinCoringResultUpdate;
    AMBA_DSP_IMG_LEVEL_s                   HIsoHighSharpenMinCoringResult;
    UINT8                                  HIsoHighSharpenScaleCoringUpdate;
    AMBA_DSP_IMG_LEVEL_s                   HIsoHighSharpenScaleCoring;

    //Med Sharpen
    UINT8                                  HIsoMedSharpenBothUpdate;
    AMBA_DSP_IMG_SHARPEN_BOTH_s            HIsoMedSharpenBoth;
    UINT8                                  HIsoMedSharpenNoiseUpdate;
    AMBA_DSP_IMG_SHARPEN_NOISE_s           HIsoMedSharpenNoise;
    UINT8                                  HIsoMedSharpenFirUpdate;
    AMBA_DSP_IMG_FIR_s                     HIsoMedSharpenFir;
    UINT8                                  HIsoMedSharpenCoringUpdate;
    AMBA_DSP_IMG_CORING_s                  HIsoMedSharpenCoring;
    UINT8                                  HIsoMedSharpenCoringIndexScaleUpdate;
    AMBA_DSP_IMG_LEVEL_s                   HIsoMedSharpenCoringIndexScale;
    UINT8                                  HIsoMedSharpenMinCoringResultUpdate;
    AMBA_DSP_IMG_LEVEL_s                   HIsoMedSharpenMinCoringResult;
    UINT8                                  HIsoMedSharpenScaleCoringUpdate;
    AMBA_DSP_IMG_LEVEL_s                   HIsoMedSharpenScaleCoring;

    //HISO Chroma ASF
    UINT8                                  HIsoChromaAsfUpdate;
    AMBA_DSP_IMG_CHROMA_ASF_INFO_s         HIsoChromaAsf;

    //HISO Chroma filter
    UINT8                                               HIsoChromaFilterPreUpdate;
    AMBA_DSP_IMG_HISO_CHROMA_FILTER_s                   HIsoChromaFilterPre;
    UINT8                                               HIsoChromaFilterHighUpdate;
    AMBA_DSP_IMG_CHROMA_FILTER_s                        HIsoChromaFilterHigh;
    UINT8                                               HIsoChromaFilterMedUpdate;
    AMBA_DSP_IMG_HISO_CHROMA_FILTER_s                   HIsoChromaFilterMed;
    UINT8                                               HIsoChromaFilterLowUpdate;
    AMBA_DSP_IMG_HISO_CHROMA_FILTER_s                   HIsoChromaFilterLow;
    UINT8                                               HIsoChromaFilterVLowUpdate;
    AMBA_DSP_IMG_HISO_CHROMA_FILTER_s                   HIsoChromaFilterVLow;
    UINT8                                               HIsoChromaFilterLowAndVLowUpdate;
    AMBA_DSP_IMG_HISO_CHROMA_LOW_VERY_LOW_FILTER_s      HIsoChromaFilterLowAndVLow;

    //HISO Chroma Combine
    UINT8                                     HIsoChromaFilterMedCombineUpdate;
    AMBA_DSP_IMG_HISO_CHROMA_FILTER_COMBINE_s HIsoChromaFilterMedCombine;
    UINT8                                     HIsoChromaFilterLowCombineUpdate;
    AMBA_DSP_IMG_HISO_CHROMA_FILTER_COMBINE_s HIsoChromaFilterLowCombine;
    UINT8                                     HIsoChromaFilterVLowCombineUpdate;
    AMBA_DSP_IMG_HISO_CHROMA_FILTER_COMBINE_s HIsoChromaFilterVLowCombine;

    //HISO Luma Combine
    UINT8                                   HIsoLumaFilterCombineUpdate;
    AMBA_DSP_IMG_HISO_LUMA_FILTER_COMBINE_s HIsoLumaFilterCombine;
    UINT8                                   HIsoLowAsfCombineUpdate;
    AMBA_DSP_IMG_HISO_LUMA_FILTER_COMBINE_s HIsoLowAsfCombine;

    //LISO/HISO Combine
 //karl A12 remove    UINT8                                     HIsoLiCombineUpdate;
 //karl A12 remove    AMBA_DSP_IMG_HISO_COMBINE_s               HIsoLiCombine;

    UINT8                                     HIsoLiLumaMidHighFreqRecoverUpdate;
    AMBA_DSP_IMG_HISO_FREQ_RECOVER_s          HIsoLiLumaMidHighFreqRecover;


    /* End of HISO filter   */
} PIPELINE_STILL_CONTROL_s;

typedef struct _PIPELINE_VEDIO_HISO_CONTROL_s_ {

    UINT8                                  StillMode;
    UINT8                                  ModeUpdate;

    UINT8                                  WbGainUpdate;
    AMBA_DSP_IMG_WB_GAIN_s                 WbGain;

    UINT8                                  VsyncExposureUpdate;
    UINT8                                  ExposureUpdate;
    AMBA_AE_INFO_s                         ExposureInfo;
    STILL_AF_INFO_s                        AfInfoStill;    /* for showing SAF focused areas on GUI after FLOCK */

    UINT8                                  RgbYuvMatrixUpdate;
    AMBA_DSP_IMG_RGB_TO_YUV_s              RgbYuvMatrix;

    /* CFA domain filters */
    UINT8                                  BlackCorrUpdate;
    AMBA_DSP_IMG_BLACK_CORRECTION_s        BlackCorr;
    UINT8                                  AntiAliasingUpdate;
    AMBA_DSP_IMG_ANTI_ALIASING_s           AntiAliasing;
    UINT8                                  BadpixCorrUpdate;
    AMBA_DSP_IMG_DBP_CORRECTION_s          BadpixCorr;

    UINT8                                  CfaFilterUpdate;
    AMBA_DSP_IMG_CFA_NOISE_FILTER_s        CfaFilter;
    UINT8                                  LocalExposureUpdate;
    AMBA_DSP_IMG_LOCAL_EXPOSURE_s          LocalExposure;

    /* RGB domain filters */
    UINT8                                  DemosaicUpdate;
    AMBA_DSP_IMG_DEMOSAIC_s                Demosaic;

    UINT8                                  ColorCorrUpdate;
    AMBA_DSP_IMG_COLOR_CORRECTION_s        ColorCorr;

    UINT8                                  GammaUpdate;
    AMBA_DSP_IMG_TONE_CURVE_s              GammaTable;

    /* Y domain filters */
    UINT8                                  ChromaScaleUpdate;
    AMBA_DSP_IMG_CHROMA_SCALE_s            ChromaScale;
    UINT8                                  ChromaMedianUpdate;
    AMBA_DSP_IMG_CHROMA_MEDIAN_FILTER_s    ChromaMedian;

//karl add li_processing_select  
#if 0 //not need for high hiso    
    UINT8                                  LisoProcessSelectUpdate;
    AMBA_DSP_IMG_LISO_PROCESS_SELECT_s     LisoProcessSelect;
    UINT8                                  LiColorDependentLumaNoiseReductionUpdate;
    AMBA_DSP_IMG_CDNR_INFO_s               LiColorDependentLumaNoiseReduction;    
#endif     
    UINT8                                  AsfUpdate;
    AMBA_DSP_IMG_ASF_INFO_s                Asf;

    UINT8                                  SharpenBothUpdate;
    AMBA_DSP_IMG_SHARPEN_BOTH_s            SharpenBoth;
    UINT8                                  SharpenNoiseUpdate;
    AMBA_DSP_IMG_SHARPEN_NOISE_s           SharpenNoise;
    UINT8                                  SharpenFirUpdate;
    AMBA_DSP_IMG_FIR_s                     SharpenFir;
    UINT8                                  SharpenCoringUpdate;
    AMBA_DSP_IMG_CORING_s                  SharpenCoring;
    UINT8                                  SharpenCoringIndexScaleUpdate;
    AMBA_DSP_IMG_LEVEL_s                   SharpenCoringIndexScale;
    UINT8                                  SharpenMinCoringResultUpdate;
    AMBA_DSP_IMG_LEVEL_s                   SharpenMinCoringResult;
    UINT8                                  SharpenScaleCoringUpdate;
    AMBA_DSP_IMG_LEVEL_s                   SharpenScaleCoring;

    UINT8                                  ChromaFilterUpdate;
    AMBA_DSP_IMG_CHROMA_FILTER_s           ChromaFilter;
    UINT8                                  GbGrMismatchUpdate;
    AMBA_DSP_IMG_GBGR_MISMATCH_s           GbGrMismatch;

    UINT8                                  LiWideChromaFilterCombineUpdate;
    AMBA_DSP_IMG_HISO_CHROMA_FILTER_COMBINE_s        LiWideChromaFilterCombine;    


    /* Start of HISO filter */
    UINT8                                  HIsoCfaLeakageFilterUpdate;
    AMBA_DSP_IMG_CFA_LEAKAGE_FILTER_s      HIsoCfaLeakageFilter;

    UINT8                                  HIsoAntiAliasingUpdate;
    AMBA_DSP_IMG_ANTI_ALIASING_s           HIsoAntiAliasing;
    UINT8                                  HIsoBadpixCorrUpdate;
    AMBA_DSP_IMG_DBP_CORRECTION_s          HIsoBadpixCorr;
    UINT8                                  HIsoCfaFilterUpdate;
    AMBA_DSP_IMG_CFA_NOISE_FILTER_s        HIsoCfaFilter;
    UINT8                                  HIsoGbGrMismatchUpdate;
    AMBA_DSP_IMG_GBGR_MISMATCH_s           HIsoGbGrMismatch;
    UINT8                                  HIsoDemosaicUpdate;
    AMBA_DSP_IMG_DEMOSAIC_s                HIsoDemosaic;
    UINT8                                  HIsoChromaMedianUpdate;
    AMBA_DSP_IMG_CHROMA_MEDIAN_FILTER_s    HIsoChromaMedian;
    UINT8                                  HIsoCdnrUpdate;
    AMBA_DSP_IMG_CDNR_INFO_s               HIsoCdnr;
    UINT8                                  HIsoDeferColorCorrUpdate;
    AMBA_DSP_IMG_DEFER_COLOR_CORRECTION_s  HIsoDeferColorCorr;

    //HISO Luma ASF
    UINT8                                  HIsoAsfUpdate;
    AMBA_DSP_IMG_ASF_INFO_s                HIsoAsf;
    UINT8                                  HIsoHighAsfUpdate;
    AMBA_DSP_IMG_ASF_INFO_s                HIsoHighAsf;
    UINT8                                  HIsoMedAsfUpdate;
    AMBA_DSP_IMG_ASF_INFO_s                HIsoMedAsf;

    UINT8                                  HIsoLowAsfUpdate;
    AMBA_DSP_IMG_ASF_INFO_s                HIsoLowAsf;

    //High Sharpen
    UINT8                                  HIsoHighSharpenBothUpdate;
    AMBA_DSP_IMG_SHARPEN_BOTH_s            HIsoHighSharpenBoth;
    UINT8                                  HIsoHighSharpenNoiseUpdate;
    AMBA_DSP_IMG_SHARPEN_NOISE_s           HIsoHighSharpenNoise;
    UINT8                                  HIsoHighSharpenFirUpdate;
    AMBA_DSP_IMG_FIR_s                     HIsoHighSharpenFir;
    UINT8                                  HIsoHighSharpenCoringUpdate;
    AMBA_DSP_IMG_CORING_s                  HIsoHighSharpenCoring;
    UINT8                                  HIsoHighSharpenCoringIndexScaleUpdate;
    AMBA_DSP_IMG_LEVEL_s                   HIsoHighSharpenCoringIndexScale;
    UINT8                                  HIsoHighSharpenMinCoringResultUpdate;
    AMBA_DSP_IMG_LEVEL_s                   HIsoHighSharpenMinCoringResult;
    UINT8                                  HIsoHighSharpenScaleCoringUpdate;
    AMBA_DSP_IMG_LEVEL_s                   HIsoHighSharpenScaleCoring;

    //Med Sharpen
    UINT8                                  HIsoMedSharpenBothUpdate;
    AMBA_DSP_IMG_SHARPEN_BOTH_s            HIsoMedSharpenBoth;
    UINT8                                  HIsoMedSharpenNoiseUpdate;
    AMBA_DSP_IMG_SHARPEN_NOISE_s           HIsoMedSharpenNoise;
    UINT8                                  HIsoMedSharpenFirUpdate;
    AMBA_DSP_IMG_FIR_s                     HIsoMedSharpenFir;
    UINT8                                  HIsoMedSharpenCoringUpdate;
    AMBA_DSP_IMG_CORING_s                  HIsoMedSharpenCoring;
    UINT8                                  HIsoMedSharpenCoringIndexScaleUpdate;
    AMBA_DSP_IMG_LEVEL_s                   HIsoMedSharpenCoringIndexScale;
    UINT8                                  HIsoMedSharpenMinCoringResultUpdate;
    AMBA_DSP_IMG_LEVEL_s                   HIsoMedSharpenMinCoringResult;
    UINT8                                  HIsoMedSharpenScaleCoringUpdate;
    AMBA_DSP_IMG_LEVEL_s                   HIsoMedSharpenScaleCoring;

    //HISO Chroma ASF
    UINT8                                  HIsoChromaAsfUpdate;
    AMBA_DSP_IMG_CHROMA_ASF_INFO_s         HIsoChromaAsf;

    //HISO Chroma filter
    UINT8                                               HIsoChromaFilterPreUpdate;
    AMBA_DSP_IMG_HISO_CHROMA_FILTER_s                   HIsoChromaFilterPre;
    UINT8                                               HIsoChromaFilterHighUpdate;
    AMBA_DSP_IMG_CHROMA_FILTER_s                        HIsoChromaFilterHigh;
    UINT8                                               HIsoChromaFilterMedUpdate;
    AMBA_DSP_IMG_HISO_CHROMA_FILTER_s                   HIsoChromaFilterMed;
    UINT8                                               HIsoChromaFilterLowUpdate;
    AMBA_DSP_IMG_HISO_CHROMA_FILTER_s                   HIsoChromaFilterLow;
    UINT8                                               HIsoChromaFilterVLowUpdate;
    AMBA_DSP_IMG_HISO_CHROMA_FILTER_s                   HIsoChromaFilterVLow;
    UINT8                                               HIsoChromaFilterLowAndVLowUpdate;
    AMBA_DSP_IMG_HISO_CHROMA_LOW_VERY_LOW_FILTER_s      HIsoChromaFilterLowAndVLow;

    //HISO Chroma Combine
    UINT8                                     HIsoChromaFilterMedCombineUpdate;
    AMBA_DSP_IMG_HISO_CHROMA_FILTER_COMBINE_s HIsoChromaFilterMedCombine;
    UINT8                                     HIsoChromaFilterLowCombineUpdate;
    AMBA_DSP_IMG_HISO_CHROMA_FILTER_COMBINE_s HIsoChromaFilterLowCombine;
    UINT8                                     HIsoChromaFilterVLowCombineUpdate;
    AMBA_DSP_IMG_HISO_CHROMA_FILTER_COMBINE_s HIsoChromaFilterVLowCombine;

    //HISO Luma Combine
    UINT8                                   HIsoLumaFilterCombineUpdate;
    AMBA_DSP_IMG_HISO_LUMA_FILTER_COMBINE_s HIsoLumaFilterCombine;
    UINT8                                   HIsoLowAsfCombineUpdate;
    AMBA_DSP_IMG_HISO_LUMA_FILTER_COMBINE_s HIsoLowAsfCombine;

    UINT8                                     HIsoLiLumaMidHighFreqRecoverUpdate;
    AMBA_DSP_IMG_HISO_FREQ_RECOVER_s          HIsoLiLumaMidHighFreqRecover;

    /* Warp and MCTF related filters */
    UINT8                                  MctfInfoUpdate;                    //vp_29
    AMBA_DSP_IMG_VIDEO_MCTF_INFO_s         MctfInfo;
    
    /* End of HISO filter   */
} PIPELINE_VEDIO_HISO_CONTROL_s;

typedef struct _WB_1_TAB_s_ {
    UINT8     enable;
    ADJ_LUT_s low_tab[ADJ_NF_TABLE_COUNT];
    ADJ_LUT_s high_tab[ADJ_NF_TABLE_COUNT];
} WB_1_TAB_s; //wb_1_tab_t

typedef struct _ADJ_VIDEO_ALL_PARAM_s_ {
    ADJ_VIDEO_PARAM_s        *pAdjVideoParam;
    UINT8                    ColorStyle;
} ADJ_VIDEO_ALL_PARAM_s;

typedef struct _ADJ_PHOTO_ALL_PARAM_s_ {
    ADJ_PHOTO_PARAM_s        *pAdjPhotoParam;
} ADJ_PHOTO_ALL_PARAM_s;


typedef struct _IMG_PARAM_s_ {
    UINT32                                 VersionNum;
    
    AMBA_DSP_IMG_CFA_LEAKAGE_FILTER_s      CfaLeakageFilterVideo;
    AMBA_DSP_IMG_CFA_LEAKAGE_FILTER_s      CfaLeakageFilterStill;

    AMBA_DSP_IMG_AAA_STAT_INFO_s           AaaStatisticsInfo;
    
    AMBA_DSP_IMG_LOCAL_EXPOSURE_s          LocalExposureVideo;
    AMBA_DSP_IMG_LOCAL_EXPOSURE_s          LocalExposureStill;

    AMBA_DSP_IMG_COLOR_CORRECTION_s        ColorCorrVideo;
    AMBA_DSP_IMG_COLOR_CORRECTION_s        ColorCorrStill;

    AMBA_DSP_IMG_RGB_TO_YUV_s              RgbYuvMatrixVideoTv;
    AMBA_DSP_IMG_RGB_TO_YUV_s              RgbYuvMatrixVideoPc;
    AMBA_DSP_IMG_RGB_TO_YUV_s              RgbYuvMatrixStill;

    AMBA_DSP_IMG_CHROMA_SCALE_s            ChromaScaleVideo;
    AMBA_DSP_IMG_CHROMA_SCALE_s            ChromaScaleStill;

    AMBA_DSP_IMG_DGAIN_SATURATION_s        DGainSaturation;

    AMBA_DSP_IMG_CFA_LEAKAGE_FILTER_s      HIsoCfaLeakageFilter;
    AMBA_DSP_IMG_DEFER_COLOR_CORRECTION_s  HIsoDeferColorCorr;
    UINT32                                 HdrFrameNum;
    UINT32                                 FrontWbCheck;     //  0:IDSP    1:LinearTable    2:Sensor
    UINT32                                 ChkDgainMode;     // 0 : none, 1 : SensorDgain,  2 : LinearDgain, 3 : IdspDgain
    double                                 NonLinearGammaValue;
    UINT16                                 HdrLinearTableInputValue[353];
    AMBA_DSP_IMG_HDR_BLENDING_INFO_s       HdrBlendingInfo;
    AMBA_DSP_IMG_HDR_ALPHA_CALC_CONFIG_s   HdrAlphaConfig0;
    AMBA_DSP_IMG_HDR_ALPHA_CALC_THRESH_s   HdrAlphaThreshold0;
    AMBA_DSP_IMG_AMP_LINEARIZATION_s       HdrAmpLinearization0;

} IMG_PARAM_s;
#define IMG_PARAM_VER (0x14111100)

#endif  /*__AMBA_ADJUSTMENT_A12_H__ */
