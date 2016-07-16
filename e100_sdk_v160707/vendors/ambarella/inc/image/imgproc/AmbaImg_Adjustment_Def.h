 /*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaImg_Adjustment_Def.h
 *
 *  @Copyright      :: Copyright (C) 2013 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Structure type definiton of ADJ
 *
\*-------------------------------------------------------------------------------------------------------------------*/

#ifndef __AMBA_IMG_ADJUSTMENT_DEF_H__
#define __AMBA_IMG_ADJUSTMENT_DEF_H__

#include "AmbaImg_Proc.h"
#include "AmbaImg_AeAwb.h"
#include "AmbaDSP_ImgFilter.h"

#define SCENE_UNIT              128

#define LS_TONE_CTRL_CONFIG_SIZE    4096
#define NUM_EXPOSURE_CURVE          256
#define NUM_CHROMA_GAIN_CURVE       128

#define NUM_IN_LOOKUP               192*3
#define NUM_MATRIX                  16*16*16
#define NUM_OUT_LOOKUP              256

#define SIZE_IN                    (NUM_IN_LOOKUP*sizeof(UINT32))    /* 2112 bytes */    // NUM_IN_LOOKUP @ iav/iav_struct.h
#define CC_3D_SIZE                 AMBA_DSP_IMG_CC_3D_SIZE//(NUM_MATRIX*sizeof(UINT32))    /* 16384 bytes */    // NUM_MATRIX @ iav/iav_struct.h
#define CC_REG_SIZE                AMBA_DSP_IMG_CC_REG_SIZE
#define SIZE_OUT                   (NUM_OUT_LOOKUP*sizeof(UINT32))    /* 1024 bytes */    // NUM_OUT_LOOKUP @ iav/iav_struct.h
#define TONE_CURVE_SIZE            (256)

#define ADJ_EV_TABLE_COUNT    25
#define ADJ_NF_TABLE_COUNT    10
//#define ADJ_LISO_NF_TABLE_COUNT    12
#define ADJ_HISO_NF_TABLE_COUNT    10

#define IQ_PARAMS_IMG_DEF           0
#define IQ_PARAMS_VIDEO_ADJ         1
#define IQ_PARAMS_VIDEO_HISO_ADJ    2
#define IQ_PARAMS_PHOTO_ADJ         3
#define IQ_PARAMS_PHOTO_HISO_ADJ    4
#define IQ_PARAMS_STILL_LISO_ADJ    5
#define IQ_PARAMS_STILL_HISO_ADJ    6    
//#define IQ_PARAMS_STILL_PARAM       7
#define IQ_PARAMS_AAA               7
#define IQ_PARAMS_STILL_IDX_INFO_ADJ     8
#define IQ_PARAMS_VIDEO_IDX_INFO_ADJ     9

typedef struct _ADJ_LUT_s_ {
    INT16        Value[24];
} ADJ_LUT_s;

typedef struct _ADJ_AWB_AE_s_ {
    UINT8        MaxTableCount;
    ADJ_LUT_s    Table[ADJ_EV_TABLE_COUNT];
} ADJ_AWB_AE_s;

typedef struct _ADJ_FILTER_INFO_s_ {
    UINT8        TableCount;
    ADJ_LUT_s    Enable;
    ADJ_LUT_s    EvTable[ADJ_EV_TABLE_COUNT];
    ADJ_LUT_s    NfTable[ADJ_NF_TABLE_COUNT];
    UINT8        AQPEnable;
    ADJ_LUT_s    EvAQPTable[ADJ_EV_TABLE_COUNT];
} ADJ_FILTER_INFO_s;

typedef struct _ADJ_HISO_FILTER_INFO_s_ {
    UINT8        TableCount;
    ADJ_LUT_s    Enable;
    ADJ_LUT_s    EvTable[ADJ_EV_TABLE_COUNT];
} ADJ_HISO_FILTER_INFO_s;

typedef struct _ADJ_COLOR_CONTROL_s_ {
    UINT16    GainR;
    UINT16    GainB;
    UINT32    MatrixThreeDTableAddr;
    INT16     CcMatrix[9];
} ADJ_COLOR_CONTROL_s; //adj_color_control_t;

typedef struct _COLOR_3D_s_ {
    UINT8               Type;
    UINT8               Control;
    ADJ_COLOR_CONTROL_s Table[5];
} COLOR_3D_s; //color_3d_t;

#define NUM_EXPOSURE_CURVE        256
#define NUM_CHROMA_GAIN_CURVE        128
typedef struct _ADJ_DEF_s_ {

    COLOR_3D_s                   Color;
    UINT8                        BlackLevelEnable;
    UINT16                       StartShutterIdx;
    UINT16                       ShutterTableNo;
    ADJ_LUT_s                    BlackLevelBase;
    ADJ_LUT_s                    ShutterBlackLevel[25];
    ADJ_LUT_s                    BlackLevel[ADJ_NF_TABLE_COUNT];
    AMBA_DSP_IMG_TONE_CURVE_s    Ratio255Gamma;
    AMBA_DSP_IMG_TONE_CURVE_s    Ratio128Gamma;
    AMBA_DSP_IMG_TONE_CURVE_s    Ratio0Gamma;
    UINT16                       LExpo255[NUM_EXPOSURE_CURVE];
    UINT16                       LExpo128[NUM_EXPOSURE_CURVE];
    UINT16                       LExpo0[NUM_EXPOSURE_CURVE];
    UINT16                       ChromaCurve255[NUM_CHROMA_GAIN_CURVE];
    UINT16                       ChromaCurve128[NUM_CHROMA_GAIN_CURVE];
    UINT16                       ChromaCurve0[NUM_CHROMA_GAIN_CURVE];
} ADJ_DEF_s; //adj_def_t;


//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

/* Adjustment parameters for agc noise control, WBGain Color control */
typedef struct _ADJ_CFA_NOISE_RATIO_VIDEO_s_ {
    UINT16    P120Bin[2];
    UINT16    P120NoNin[2];
    UINT16    P60Bin[2];
    UINT16    P60NoBin[2];
    UINT16    P30Bin[2];
    UINT16    P30NoBin[2];
} ADJ_CFA_NOISE_RATIO_VIDEO_s; //adj_cfa_noise_ratio_video_t

typedef struct _ADJ_LUMA_SHARPEN_RATIO_VIDEO_s_ {
    UINT16    P120Bin[3];
    UINT16    P120NoNin[3];
    UINT16    P60Bin[3];
    UINT16    P60NoBin[3];
    UINT16    P30Bin[3];
    UINT16    P30NoBin[3];
} ADJ_LUMA_SHARPEN_RATIO_VIDEO_s; //adj_luma_sharpen_ratio_video_t


/**
 * Still AF Lock Info
 */
#define FOCUSED_AREA        1
#define NON_FOCUSED_AREA    0
typedef struct _FOCUS_AREA_s_ {
    UINT8     Focused;    /* 1 if FOCUSED_AREA; 0 otherwise */
    UINT16    ColStart;    /* focus area column start in capture window */
    UINT16    RowStart;    /* focus area row start in capture window */
    UINT16    Width;        /* focus area width in capture window */
    UINT16    Height;        /* focus area height in capture window */
    UINT32    Score;        /* focus area score in capture window */
} FOCUS_AREA_s; //focus_area_t

#define SAF_FOCUS_FOUND        1
#define SAF_FOCUS_NOT_FOUND    0
typedef struct _STILL_AF_INFO_s_ {
    UINT8           LockState;    /* 1 if SAF_FOCUS_FOUND; 0 otherwise */
    UINT8           TotalAreas;    /* maximum number of focus areas */
    FOCUS_AREA_s    Area[40];    /* maximum 40 focus areas to support up to most AF tiles */
} STILL_AF_INFO_s; //still_af_info_t;

typedef struct _STR_INFO_s_ {
    INT16     Str;
    UINT16    Offset;
    UINT16    Adapt;
} STR_INFO_s; //str_info_t

#define STR_MAX_VALUE        19
typedef struct _STR_LUT_s_ {
    STR_INFO_s    Info[24];
} STR_LUT_s;

typedef struct _LUT_CONTROL_s_ {
    INT16        Start;
    INT16        End;

    STR_LUT_s    Lut;
} LUT_CONTROL_s; //;lut_control_t

typedef struct _INTERPO_INFO_s_ {
        UINT16    Index;
    UINT32    PrevIdx;
    UINT32    NextIdx;
    INT32    PrevWeight;
    INT32    NextWeight;
    UINT8    ShiftBit;
} INTERPO_INFO_s;//___INTERPO_INFO_s;

typedef struct _WB_INTERPO_s_ {
    INT32    PrevWeight;
    INT32    NextWeight;
    UINT8    ShiftBit;
} WB_INTERPO_s;//___WB_INTERPO_s;

typedef struct _LIMIT_s_ {
    INT32    DOWN;
    INT32    UP;
} LIMIT_s;//__LIMIT_s;

typedef struct _ADJ_AEAWB_CONTROL_s_ {
    UINT16      LowTempRTarget;
    UINT16      LowTempBTarget;
    UINT16      D50RTarget;
    UINT16      D50BTarget;
    UINT16      HighTempRTarget;
    UINT16      HighTempBTarget;
    UINT16      AeTarget;
    UINT16      AutoKnee;
} ADJ_AEAWB_CONTROL_s; //adj_aeawb_control_t;

typedef struct _ADJ_STR_CONTROL_s_ {
    UINT8        TableCount;
    UINT8        Enable;
    INT16        Start;
    INT16        End;

    STR_LUT_s    Lut[12];
}ADJ_STR_CONTROL_s; //adj_str_control_t;

#define ISO_IDX_TYPE    0
#define AGC_IDX_TYPE    1
typedef struct _ADJ_IDX_INFO_s_ {
    UINT16    IdxType;
    INT32     MinValue;
    INT32     MinNf;
} ADJ_IDX_INFO_s; //adj_idx_info_t;

typedef struct _ADJ_STILL_IDX_INFO_s_ {
    UINT32            VersionNumber;
    UINT32            ParamVersionNum;
    ADJ_IDX_INFO_s    LowIso;
    ADJ_IDX_INFO_s    HighIso;
} ADJ_STILL_IDX_INFO_s;
#define ADJ_STILL_IDX_INFO_VER (0x14051500)

typedef struct _ADJ_VIDEO_IDX_INFO_s_ {
    UINT32            VersionNumber;
    UINT32            ParamVersionNum;
    ADJ_IDX_INFO_s    LowIso;
    ADJ_IDX_INFO_s    HighIso;
} ADJ_VIDEO_IDX_INFO_s;
#define ADJ_VIDEO_IDX_INFO_VER (0x14100800)
/*
typedef struct _IQ_CC_TABLE_ADDR_s_
{
    UINT32 CcTableAddr[5];
    UINT32 CcTableAddrLinBw;
    UINT32 Reserved;
    UINT32 Reserved1;
    UINT32 Reserved2;
    
}IQ_CC_TABLE_ADDR_s;


typedef struct _IQ_PARAM_ADDR_s_
{
    UINT32 AaaParamTableAddr;
    UINT32 ImageDefTableAddr;
    UINT32 VideoAdjTableAddr;
    UINT32 VideoAdjHIsoTableAddr;
    UINT32 PhotoAdjTableAddr;
    UINT32 PhotoAdjHIsoTableAddr;
    UINT32 StillAdjLIsoTableAddr;
    UINT32 StillAdjHIsoTableAddr;
    UINT32 StillParamTableAddr;
    
    UINT32 Reserved;
    UINT32 Reserved1;
    UINT32 Reserved2;

}IQ_PARAM_ADDR_s;
*/

typedef struct _ADJ_IQ_INFO_s_ {
    UINT8                        Mode;
    AMBA_AE_INFO_s               Ae;
    AMBA_DSP_IMG_WB_GAIN_s       Wb;
    UINT16                       DZoomStep;
    UINT32                       AwbAeParamAdd;
    UINT32                       ColorParamAdd;
    UINT32                       FilterParamAdd;
    UINT16                       AdjTableNo;
    UINT32                       HisoFilterParamAdd;
} ADJ_IQ_INFO_s;

typedef struct _ADJ_AWBAE_CONTROL_s_ {
    UINT8                  VinNum;
    UINT8                  Mode;
    UINT16                 EvIndex;
    AMBA_DSP_IMG_WB_GAIN_s WbGain;
    UINT32                 AwbAeParamAdd;
    UINT32                 ColorParamAdd;
} ADJ_AWBAE_CONTROL_s;

typedef struct _ADJ_VIDEO_CONTROL_s_ {
    UINT8                  Mode;
    UINT8                  Enable;
    UINT8                  VinNum;
    UINT16                 ShIndex;
    UINT16                 EvIndex;
    UINT16                 NfIndex;
    AMBA_DSP_IMG_WB_GAIN_s WbGain;
    UINT16                 DZoomStep;
    UINT32                 FilterParamAdd;
    UINT32                 ColorParamAdd;
} ADJ_VIDEO_CONTROL_s;

typedef struct _ADJ_STILL_CONTROL_s {
    UINT8                     StillMode;
    UINT16                    ShIndex;
    UINT16                    EvIndex;
    UINT16                    NfIndex;
    AMBA_DSP_IMG_WB_GAIN_s    WbGain;
    UINT16                    DZoomStep;
    UINT8                     FlashMode;
    UINT8                     LutNo;
} ADJ_STILL_CONTROL_s;


#define ADJ_SENSOR_X1PC        201
#define ADJ_SENSOR_X2PC           202
#define ADJ_SENSOR_X3PC           203
#define ADJ_SENSOR_X4PC        204
#define ADJ_SENSOR_X8PC        205

#define ADJ_SENSOR_X1TV        101
#define ADJ_SENSOR_X2TV        102
#define ADJ_SENSOR_X3TV        103
#define ADJ_SENSOR_X4TV        104
#define ADJ_SENSOR_X8TV        105

#define ADJ_R3840x2160P030V1C1X01  118042001
#define ADJ_R_P_V_C_X__  100000000


#define ADJ_R1920X1080P030V1C1X02  104042202
#define ADJ_R1280X0720P030V1C1X02  108042202
#define ADJ_R1920X1080P030V1C1X01  104042201
#define ADJ_R1920X1080P030V0C1X01  104041201

#define ADJ_R1920X1080P060V0C1X01  104091201
#define ADJ_R2304X1296P030V1C1X01  103042201

#define ADJ_R1920X1080P045V0C1X01  104061201

#define ADJ_R2560X1080P030V1C1X01  102042201
#define ADJ_R_________P030V_C_X__  100040000
#define ADJ_R2304X1296P030V1C1X01  103042201
#define ADJ_R1920X1080P045V0C1X01  104061201

#define ADJ_R1920X1080P060V0C1X02  104091202
#define ADJ_R1920X1080P048V0C1X02  104071202
#define ADJ_R1920X1080P024V1C1X02  104022202

#define ADJ_R1280X0720P060V0C1X02  108091202
#define ADJ_R2560X1440P030V1C1X01  101042201
#define ADJ_R2560X1440P___V_C_X__  101000000
#define ADJ_R1920X1080P___V_C_X__  104000000

/******************************************************

#define ADJ_R2560X1080P030V1C1X01  102042201

  ADJ_ R2560X1080  P030  V1  C1  X01
              102    04   2   2   01

 R_________ : 100  ignore
 R2560X1440 : 101    R2560X1080 : 102    R2304X1296 : 103
 R1920X1080 : 104    R1600X1200 : 105    R1600X0900 : 106
 R1440X1080 : 107    R1280X0720 : 108    R1280X0960 : 109
 R0960X0540 : 110    R0848X0480 : 111    R0720X0480 : 112
 R0640X0480 : 113    R0640X0360 : 114    R0432X0240 : 115
 R0352X0240 : 116    R0320X0240 : 117    R3840X2160 : 118

 P___ : 00  ignore
 P015 : 01    P024 : 02    P025 : 03    P030 : 04    P040 : 05
 P045 : 06    P048 : 07    P050 : 08    P060 : 09    P100 : 10
 P120 : 11    P200 : 12    P240 : 13

 V_ : 0   ignore
 V0 : 1   oversaming mode is not enabled
 V1 : 2   oversaming mode is enabled

 C_ : 0   ignore
 C0 : 1   TV mode
 C1 : 2   PC mode

 X__ : 00   ignore
 X01 : 01   non_Binnning        X02 : 02  Binnning_2X
 X03 : 03   Binnning_3X         X04 : 04  Binnning_4X
 X05 : 05   Binnning_8X

**********************************************************/
typedef struct _ADJ_VIDEO_MODE_s_ {
    UINT32    Define;
    UINT32    Resolution;
    UINT32    FrameRate;
    UINT8     OverSampling;
    UINT8     ColorStyle;
    UINT32    Binning;
} ADJ_VIDEO_MODE_s;

#define ADJ_TABLE_COUNT_MAX_NUM    8
#define ADJ_TABLE_SAME_MAX_NUM    8

typedef struct _ADJ_TABLE_IDX_s_ {
    UINT32    MaxTableCount;
    UINT32    MaxTableSame;
    UINT32    Index[ADJ_TABLE_COUNT_MAX_NUM][ADJ_TABLE_SAME_MAX_NUM];
} ADJ_TABLE_IDX_s;

typedef struct _ADJ_TABLE_PARAM_s_ {
    UINT32             VersionNum;
    UINT32             ParamVersionNum;
    ADJ_TABLE_IDX_s    Video;
    ADJ_TABLE_IDX_s    PhotoPreview;
} ADJ_TABLE_PARAM_s;
#define ADJ_TABLE_PARAM_VER (0x14010300)

typedef struct _ADJ_AQP_INFO_s_ {
    UINT8        UpdateFlg;
    ADJ_LUT_s    AQPParams;
} ADJ_AQP_INFO_s;

#endif  /*__AMBA_ADJUSTMENT_DEF_H__ */
