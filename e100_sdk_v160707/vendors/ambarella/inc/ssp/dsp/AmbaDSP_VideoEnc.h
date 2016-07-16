/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaDSP_VideoEnc.h
 *
 *  @Copyright      :: Copyright (C) 2012 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Definitions & Constants for Ambarella DSP H.264 Encoder APIs
 *
\*-------------------------------------------------------------------------------------------------------------------*/

#ifndef _AMBA_DSP_VIDEO_ENC_H_
#define _AMBA_DSP_VIDEO_ENC_H_

#include "AmbaDSP.h"

#define AMBA_DSP_MAX_NUM_STREAM_PER_SENSOR      3   /* maximum number of Video Streams per Sensor */
#define END_PIC_SIZE                            0x00FFFFFF

typedef enum _AMBA_DSP_VIDEO_ENC_STREAM_TYPE_e_ {
    AMBA_DSP_ENC_MAIN_STREAM = 0,
    AMBA_DSP_ENC_SECOND_STREAM,

    AMBA_DSP_NUM_VIDEO_ENC_STREAM
} AMBA_DSP_VIDEO_ENC_STREAM_TYPE_e;

typedef enum _AMBA_DSP_VIDEO_ENC_FRAME_TYPE_e_ {
    AMBA_DSP_FRAME_TYPE_IDR = 1,
    AMBA_DSP_FRAME_TYPE_I,
    AMBA_DSP_FRAME_TYPE_P,
    AMBA_DSP_FRAME_TYPE_B,
    AMBA_DSP_FRAME_TYPE_MJPEG
} AMBA_DSP_VIDEO_ENC_FRAME_TYPE_e;

typedef enum _AMBA_DSP_H264ENC_STOP_PAUSE_OPTION_e_ {
    AMBA_DSP_H264ENC_STOP_PAUSE_IMMEDIATELY = 0,
    AMBA_DSP_H264ENC_STOP_PAUSE_NEXT_IP,
    AMBA_DSP_H264ENC_STOP_PAUSE_NEXT_I,
    AMBA_DSP_H264ENC_STOP_PAUSE_NEXT_IDR,
    AMBA_DSP_H264ENC_STOP_MBSYNC,
    AMBA_DSP_H264ENC_STOP_EMERG
} AMBA_DSP_H264ENC_STOP_PAUSE_OPTION_e;

typedef enum _AMBA_DSP_ENC_REPEAT_DROP_OPTION_e_ {
    AMBA_DSP_VIDEO_DROP_FRAME_ONLY = 0,
    AMBA_DSP_VIDEO_DROP_AND_REPEAT_FRAME,
    AMBA_DSP_VIDEO_REPEAT_FRAME,
} AMBA_DSP_ENC_REPEAT_DROP_OPTION_e;

typedef enum _AMBA_DSP_BITRATE_CTRL_e {
    AMBA_DSP_BITRATE_CBR = 1,       /* Constant Bit Rate */
    AMBA_DSP_BITRATE_VBR = 3,       /* Variable Bit Rate */
    AMBA_DSP_BITRATE_SMART_VBR = 4  /* Bit Rate Control */
} AMBA_DSP_BITRATE_CTRL_e;

typedef struct _AMBA_DSP_H264ENC_VUI_s_ {
    UINT8   VuiEnable;
    UINT8   AspectRatioInfoPresentFlag;
    UINT8   OverscanInfoPresentFlag;
    UINT8   OverscanAppropriateFlag;
    UINT8   VideoSignalTypePresentFlag;
    UINT8   VideoFullRangeFlag;
    UINT8   ColourDescriptionPresentFlag;
    UINT8   ChromaLocInfoPresentFlag;
    UINT8   TimingInfoPresentFlag;
    UINT8   FixedFrameRateFlag;
    UINT8   NalHrdParametersPresentFlag;
    UINT8   VclHrdParametersPresentFlag;
    UINT8   LowDelayHrdFlag;
    UINT8   PicStructPresentFlag;
    UINT8   BitstreamRestrictionFlag;
    UINT8   MotionVectorsOverPicBoundariesFlag;
    UINT8   AspectRatioIdc;
    UINT16  SarWidth;
    UINT16  SarHeight;
    UINT8   VideoFormat;
    UINT8   ColourPrimaries;
    UINT8   TransferCharacteristics;
    UINT8   MatrixCoefficients;
    UINT8   ChromaSampleLocTypeTopField;
    UINT8   ChromaSampleLocTypeBottomField;
    UINT8   Log2MaxMvLengthHorizontal;
    UINT8   Log2MaxMvLengthVertical;
    UINT16  NumReorderFrames;
    UINT16  MaxDecFrameBuffering;
    UINT16  MaxBytesPerPicDenom;
    UINT16  MaxBitsPerMbDenom;
} AMBA_DSP_H264ENC_VUI_s;

typedef struct _AMBA_DSP_H264ENC_QP_CONTROL_s_ {
    UINT8 QpMinIFrame;
    UINT8 QpMaxIFrame;
    UINT8 QpMinPFrame;
    UINT8 QpMaxPFrame;
    UINT8 QpMinBFrame;
    UINT8 QpMaxBFrame;
} AMBA_DSP_H264ENC_QP_CONTROL_s;

typedef struct _AMBA_DSP_H264ENC_QUALITY_MODEL_s_ {
    UINT8 IorIDRFrameNeedsRateControlMask;
    UINT8 QPReduceNearIDRFrame;
    UINT8 ClassNumberLimitOfQPReduceNearIDRFrame;
    UINT8 QPReduceNearIFrame;
    UINT8 ClassNumberLimitOfQPReduceNearIFrame;

    INT8 Intra16by16Bias;
    INT8 Intra4by4Bias;
    INT8 Inter16by16Bias;
    INT8 Inter8by8Bias;

    INT8 Direct16by16Bias;
    INT8 Direct8by8Bias;
    INT8 MeLambdaQPOffset;
    INT8 AQPStrength;

    INT8 Alpha;
    INT8 Beta;
} AMBA_DSP_H264ENC_QUALITY_MODEL_s;

typedef struct _AMBA_DSP_H264ENC_GOP_CONFIG_s_ {
    UINT8 M;                  /**< Distance between P frame */
    UINT8 N;                  /**< Distance between I frame */
    UINT8 IDR;                /**< Distance between IDR frame */
    UINT8 GopStruct;          /**< Gop struct */
    UINT8 HierP;              /**< Hierarchical Gop or not */
} AMBA_DSP_H264ENC_GOP_CONFIG_s;

typedef struct _AMBA_DSP_H264ENC_ROI_CONFIG_s_ {
    UINT32 *RoiBuffer;          /**< roi mapping table of every MB */
    INT8   RoiDelta[3][4];      /**< roi delta value -51~51,
                                  * 3: 0 is I-Frame, 1 is P-Frame, 2 is B-Frame,
                                  * 4: four category (0~3) corresponding to roi_mapping table */
} AMBA_DSP_H264ENC_ROI_CONFIG_s;

typedef struct _AMBA_DSP_H264ENC_HQP_CONFIG_s_ {
    UINT8 HighPNumber;      /**< Number of high quality P pictures (Q picture) in GOP
                              *  Set it as N, (2^N)-1 high quality P pictures in GOP
                              */
    UINT8 QpMin;            /**< min QP for P picture, 0~51, default is 14 */
    UINT8 QpMax;            /**< max QP for P picture, 0~51, default is 51 */
    UINT8 QpReduce;         /**< how much better to make Q QP relative to P QP, 1~10, , default is 6 */
} AMBA_DSP_H264ENC_HQP_CONFIG_s;

typedef union _AMBA_DSP_H264ENC_REALTIME_Q_CTRL_u_ {
    UINT16   Data;

    struct {
        UINT16  BitRateCtrl:            1;      /* enable BitRate Control or not */
        UINT16  GopCtrl:                1;      /* enable Gop Control or not */
        UINT16  QpCtrl:                 1;      /* enable QP Control or not */
        UINT16  QModelCtrl:             1;      /* enable Q Model Control or not */
        UINT16  RoiCtrl:                1;      /* enable ROI Control or not */
        UINT16  HQpCtrl:                1;      /* enable H-QP Control or not */
        UINT16  ZmvCtrl:                1;      /* enable H-QP Control or not */
        UINT16  Reserved:               9;      /* Reserved */
    } Bits;
} AMBA_DSP_H264ENC_REALTIME_Q_CTRL_u;

typedef struct _AMBA_DSP_H264ENC_REALTIME_QUALITY_s_ {
    AMBA_DSP_H264ENC_REALTIME_Q_CTRL_u  CtrlFlag;
    UINT32                              BitRate;
    AMBA_DSP_H264ENC_GOP_CONFIG_s       GopConfig;
    AMBA_DSP_H264ENC_QP_CONTROL_s       QpConfig;
    AMBA_DSP_H264ENC_QUALITY_MODEL_s    QualityModelConfig;
    AMBA_DSP_H264ENC_ROI_CONFIG_s       RoiConfig;
    AMBA_DSP_H264ENC_HQP_CONFIG_s       HqpConfig;
    UINT32                              ZmvConfig;
} AMBA_DSP_H264ENC_REALTIME_QUALITY_s;

typedef union _AMBA_DSP_VIDEO_ENC_START_CONFIG_u_ {
    struct {
        UINT8                               EnableSlowShutter;  /**< obsolete in A12 */
        UINT8                               FirstGOPStartB;
        UINT8                               SyncWithPIV;        /**< only primary stream support, this flag is generally use for VideoThumbnail */
        UINT8                               EnableLoopFilter;
        UINT8                               LoopFilterAlpha;    /**< A12 fill in QModelCtrl */
        UINT8                               LoopFilterBeta;     /**< A12 fill in QModelCtrl */
        UINT8                               AuType;
        UINT8                               SyncBlending;
        UINT16                              FrameCroppingFlag;
        UINT16                              FrameCropLeftOffset;
        UINT16                              FrameCropRightOffset;
        UINT16                              FrameCropTopOffset;
        UINT16                              FrameCropBottomOffset;
        UINT8                               Reserved:8;
        UINT8                               ResRateMin;         /**< 0~100 for AQP */
        AMBA_DSP_H264ENC_VUI_s              Vui;
        AMBA_DSP_H264ENC_QUALITY_MODEL_s    QModelCtrl;
        UINT32 EncDuration;             /**< 0: continuous, the unit is frmae */
        UINT32 StartEncodeSkipFrames;   /**< 0: encode immediately */
    } H264;

    struct {
        UINT8  NumeratorOfFrameReduction;
        UINT8  DenominatorOfFrameReduction;
        UINT32 EncDuration;             /**< 0: continuous, the unit is frmae */
        UINT32 StartEncodeSkipFrames;   /**< 0: encode immediately */
    } MJpeg;
} AMBA_DSP_VIDEO_ENC_START_CONFIG_u;

typedef struct _AMBA_DSP_H264ENC_CONFIG_s_ {
    UINT8                           ProfileIDC;
    UINT8                           LevelIDC;
    UINT8                           IsCabac;
    UINT8                           M;
    UINT8                           N;
    UINT8                           GopStruct; // obsolete in A12
    UINT8                           HierPGop;
    UINT8                           NumPRef;
    UINT8                           NumBRef;
    UINT32                          QualityLevel;
    AMBA_DSP_BITRATE_CTRL_e         BitRateSetting;
    UINT8                           VbrComplexLevel;    /* complexity level. The desired picture quality level. */
    UINT8                           VbrPercent;         /* 0 ~ 99. The percentage of average rate that will be devoted to VBR. The rest is for CBR. */
    UINT16                          VbrMinRatio;        /* 0 ~ 100. The minimum rate that vbr will not dip below */
    UINT16                          VbrMaxRatio;        /* 100 ~ . The maximum rate the vbr will not go above */
    UINT32                          IdrInterval;
    UINT8                           *pBitsBufAddr;
    UINT32                          BitsBufSize;
    UINT8                           *pBitsBufStartAddr; /* Indicate a dedicate start bits address */
    UINT32                          BitRate;
    AMBA_DSP_H264ENC_QP_CONTROL_s   QPControl;
    AMBA_DSP_H264ENC_ROI_CONFIG_s   RoiControl;
    AMBA_DSP_H264ENC_HQP_CONFIG_s   HQPControl;
    UINT32                          ZmvThreshold;
} AMBA_DSP_H264ENC_CONFIG_s;

typedef struct _AMBA_DSP_MJPEG_ENC_CONFIG_s_ {
    UINT8   *pBitsBufAddr;
    UINT32  BitsBufSize;
    UINT8   *pBitsBufStartAddr; /* Indicate a dedicate start bits address */
    UINT8   *pQuantMatrixAddr;
} AMBA_DSP_MJPEG_ENC_CONFIG_s;

typedef union _AMBA_DSP_VIDEO_ENC_CONFIG_u_ {
    AMBA_DSP_H264ENC_CONFIG_s       H264Config;
    AMBA_DSP_MJPEG_ENC_CONFIG_s     MJpegConfig;
} AMBA_DSP_VIDEO_ENC_CONFIG_u;

typedef struct _AMBA_DSP_VIDEO_STREAM_ID_s_ {
    UINT8   StreamID;            /* Stream ID/Stream Type */
    UINT8   ViewZoneID;          /* source View Zone ID */
} AMBA_DSP_VIDEO_STREAM_ID_s;

typedef struct _AMBA_DSP_VIDEO_ENC_STREAM_CONFIG_s_ {
    UINT8                        DataFmt;       /* Data format: 0 - H.264; 1 - Motion JPEG */
    UINT8                        IsIntervalCap;
    AMBA_DSP_VIDEO_STREAM_ID_s   StreamID;      /* Video Stream ID */
    AMBA_DSP_WINDOW_s            Window;        /* Window position and size */
    UINT32                       FrameRateDivisor;
    AMBA_DSP_VIDEO_ENC_CONFIG_u  EncConfig;
    AMBA_DSP_ROTATE_FLIP_e       RotateConfig;
    AMBA_DSP_FRAME_RATE_s        FrameRate;
} AMBA_DSP_VIDEO_ENC_STREAM_CONFIG_s;

typedef struct _AMBA_DSP_VIDEO_REPEAT_DROP_CONFIG_s_ {
    UINT8                              FrameCount;  /* max value is limited to 63 */
    AMBA_DSP_ENC_REPEAT_DROP_OPTION_e  Option;
} AMBA_DSP_VIDEO_REPEAT_DROP_CONFIG_s;

typedef struct _AMBA_DSP_VIDEO_ENC_RESOURCE_CONFIG_s_ {
    UINT8  DisablePiv:1;
    UINT8  DisableH264:1;
    UINT8  DisableMJPEG:1;
    UINT8  Disable2ndStream:1;
    UINT8  DisableHDPreview:1;
    UINT8  EnableSqueezeYuvBuffer:3;

    UINT16 MaxPivWidth;                             /* Max PIV width, height will follow Main AR */
    UINT16 MaxPivThumbWidth;                        /* Max PIV thumbnail width, height will follow Main AR */

} AMBA_DSP_VIDEO_ENC_RESOURCE_CONFIG_s;

/*-----------------------------------------------------------------------------------------------*\
 * Defined in AmbaDSP_VideoEnc.c
\*-----------------------------------------------------------------------------------------------*/
int AmbaDSP_VideoEncConfig(int MaxNumStream, AMBA_DSP_VIDEO_ENC_STREAM_CONFIG_s *pStreamConfig);
int AmbaDSP_VideoEncStreamConfig(int StreamIdx);

int AmbaDSP_VideoEncStart(int NumStream, int *pStreamIdx, AMBA_DSP_VIDEO_ENC_START_CONFIG_u *pStartConfig);
int AmbaDSP_VideoEncStop(int NumStream, int *pStreamIdx, AMBA_DSP_H264ENC_STOP_PAUSE_OPTION_e *pOption);
int AmbaDSP_VideoEncPause(int NumStream, int *pStreamIdx, AMBA_DSP_H264ENC_STOP_PAUSE_OPTION_e *pOption);
int AmbaDSP_VideoEncResume(int NumStream, int *pStreamIdx);
int AmbaDSP_VideoEncChangeFrameRate(int NumStream, int *pStreamIdx, UINT32 *pDivisor);
int AmbaDSP_VideoEncIntervalCapCtrl(UINT8 VinID);
int AmbaDSP_VideoEncRepeatDropCtrl(int NumStream, int *pStreamIdx, AMBA_DSP_VIDEO_REPEAT_DROP_CONFIG_s *pRepeatDropConfig);
int AmbaDSP_VideoEncSlowShutterCtrl(int NumStream, int *pStreamIdx, UINT32 *pUpsamplingRate);
int AmbaDSP_VideoEncBlendCtrl(int NumStream, int *pStreamIdx, AMBA_DSP_VIDEO_BLEND_CONFIG_s *pBlendConfig);
int AmbaDSP_VideoEncDemandIdr(int NumStream, int *pStreamIdx);

int AmbaDSP_VideoEncFeedRawData(AMBA_DSP_RAW_BUF_s *pExtRawBuf, UINT8 IsLast);

int AmbaDSP_VideoEncChangeBitRate(int NumStream, int *pStreamIdx, UINT32 *pBitRate);
int AmbaDSP_VideoEncRealTimeQualityCtrl(int NumStream, int *pStreamIdx, AMBA_DSP_H264ENC_REALTIME_QUALITY_s *pRealTimeQ);

int AmbaDSP_SetVideoEncResourceLimitation(AMBA_DSP_VIDEO_ENC_RESOURCE_CONFIG_s *pConfig);


#endif  /* _AMBA_DSP_VIDEO_ENC_H_ */
