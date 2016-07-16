 /**
  * @file inc/mw/recorder/videoenc.h
  *
  * Amba video codec header
  *
  * Copyright (C) 2013, Ambarella, Inc.
  *
  * All rights reserved. No Part of this file may be reproduced, stored
  * in a retrieval system, or transmitted, in any form, or by any means,
  * electronic, mechanical, photocopying, recording, or otherwise,
  * without the prior consent of Ambarella, Inc.
  */

#ifndef _VIDEOENC_H_
#define _VIDEOENC_H_


/**
 * @defgroup VideoEnc
 * @brief Video Encode module implementation
 *
 * Implementation the Video Encode module, include below function implementation
 *  1. Initial Video Encode module function.
 *  2. Create Video Encode module function.
 *  3. Delete Video Encode module function.
 *  4. Config Video Encode module window layout
 *  5. Config Bitrate/Quality control
 *  6. Config Bitstream buffer
 *  7. Trigger TimeLapse Video capture
 *  8. Config Encode Blending
 *  9. Tirgger VideoTuning flow
 * 10. Get Encoding inforamtion
 *
 */
/**
 * @addtogroup VideoEnc
 * @ingroup Codec
 * @{
 */


#include <mw.h>
#include <recorder/Encode.h>
#include <vin/vin.h>

/**
 *  Ambarella multiple stream definition
 */
typedef enum _AMP_VIDEOENC_STREAM_ID_e_ {
    AMP_VIDEOENC_STREAM_PRIMARY = 0,
    AMP_VIDEOENC_STREAM_SECONDARY,
    AMP_VIDEOENC_STREAM_TERTIARY,
    AMP_VIDEOENC_STREAM_QUATERNARY,
    AMP_VIDEOENC_STREAM_STREAM_ID_NUM
} AMP_VIDEOENC_STREAM_ID_e;

/**
 *  Encoder type. 0 = unset
 */
typedef enum _AMP_VIDEOENC_STREAM_SPEC_e_ {
    AMP_VIDEOENC_CODER_AVCC = 1,          /**< H.264 */
    AMP_VIDEOENC_CODER_MJPEG,             /**< Motion JPEG */
    AMP_VIDEOENC_CODER_JPEG,              /**< Individual JPEG (PIV) */
    AMP_VIDEOENC_CODER_STREAM_TYPE_NUM
} AMP_VIDEOENC_STREAM_SPEC_e;

/**
 * Describe each VIN layout to ENC layout
 */
typedef struct _AMP_VIDEOENC_LAYER_DESC_s_ {
    UINT8 LayerId;                      /**< Layer ID */
    UINT8 EnableSourceArea;                 /**< sourceArea is valid or not. If not, use full source resolution */
    UINT8 EnableTargetArea;                 /**< TargetArea is valid or not */
    AMP_ENC_SOURCE_TYPE_e SourceType;      /**< from VIN or memory */
    void *Source;                       /**< from which vin */
    UINT32 SourceLayoutId;              /**< from Nth layout of the source */
    AMP_AREA_s SourceArea;              /**< Area cropped from the source plane */
    AMP_AREA_s TargetArea;              /**< Area within output plane (vinWidth and vinHeight)*/
    AMP_AREA_s MultiChanEncSrcArea;     /**< Area within Encode input plane*/
    AMP_AREA_s MultiChanEncDestArea;    /**< Area within Encode output plane*/
    AMP_2D_BUFFER_s MultiChanEncBlendTable;
    AMP_ROTATION_e SourceRotate;        /**< Rotate after crop */
    AMBA_DSP_BUF_s MemorySource;        /**< memory starting buffer when VIN comes from memory */
    UINT16 MemoryBufferNumber;          /**< memory buffer total number when VIN comes from memory */
    UINT8 MemoryRawBayerPattern;        /* Raw file bayer pattern when VIN comes from memory */
    UINT8 MemoryRawDataBits;            /* Raw file bits resolution when VIN comes from memory */
} AMP_VIDEOENC_LAYER_DESC_s;

/**
 * Layout summary
 */
typedef struct _AMP_VIDEOENC_LAYOUT_CFG_s_{
    UINT16 Width;        /**< main output width */
    UINT16 Height;       /**< main output height */
    UINT32 LayerNumber;     /**< number of layouts */
    AMP_VIDEOENC_LAYER_DESC_s *Layer; /**< layout descriptor */
} AMP_VIDEOENC_LAYOUT_CFG_s;

/**
 *  init config
 */
typedef struct AMP_VIDEOENC_INIT_CFG_s_ {
    UINT8 MaxLayerPerEncoder;           /**< Maximum number of layer per encoder layout */
    AMP_YUV_COLOR_s BackgroundColor;         /**< Background color of video */
    AMP_TASK_INFO_s TaskInfo;    /**< vdsp/general task information */
    UINT32 MsgQueueNumber;              /**< number of entries per message queue */
    UINT8* MemoryPoolAddr;           /**< working buffer start address */
    UINT32 MemoryPoolSize;           /**< size of the buffer */
} AMP_VIDEOENC_INIT_CFG_s;


/**
 * Encode OSD blending (date/time stamp) info
 */
typedef struct _AMP_VIDEOENC_BLEND_INFO_s_ {
    UINT8 BufferID;        /**< Blend buffer ID, 0 ~ 31 (at most 32 blending areas) */
    UINT8 Enable;        /**< 0: Disable, 1: Enable */
    UINT16 OffsetX;      /**< Horizontal offset with respect to main picture */
    UINT16 OffsetY;      /**< Vertical offset with respect to main picture */
    UINT16 Pitch;        /**< Blend buffer pitch */
    UINT16 Width;        /**< Blend buffer width */
    UINT16 Height;       /**< Blend buffer height */
    UINT8 *YAddr;        /**< Blend buffer luma address */
    UINT8 *UVAddr;       /**< Blend buffer chroma address */
    UINT8 *AlphaYAddr;   /**< Ahpla plane luma address */
    UINT8 *AlphaUVAddr;  /**< Ahpla plane chroma address */
} AMP_VIDEOENC_BLEND_INFO_s;


/**
 * Runtime video (h.264) bitrate change parameter
 */
typedef struct AMP_VIDEOENC_RUNTIME_BITRATE_CFG_s_ {
    UINT32 AverageBitrate;          /**< Average bitrate */
} AMP_VIDEOENC_RUNTIME_BITRATE_CFG_s;

/**
 * Runtime video (h.264) quality control patemete
 */
typedef struct AMP_VIDEOENC_RUNTIME_QUALITY_CFG_s_ {
    UINT32                              Cmd;
#define RC_BITRATE   (1<<0)
#define RC_GOP       (1<<1)
#define RC_QP        (1<<2)
#define RC_QMODEL    (1<<3)
#define RC_ROI       (1<<4)
#define RC_HQP       (1<<5)
#define RC_ZMV       (1<<6)
#define RC_FORCE_IDR (1<<7)
    UINT32 BitRate;

    UINT8 Enable;
    UINT8 M;
    UINT8 N;
    UINT8 IDR;

    UINT8 QpMinI;
    UINT8 QpMaxI;
    UINT8 QpMinP;
    UINT8 QpMaxP;
    UINT8 QpMinB;
    UINT8 QpMaxB;
    UINT8 HQpMax;
    UINT8 HQpMin;
    UINT8 HQpReduce;
    UINT8 HPNumber;

    UINT8 IorIDRFrameNeedsRateControlMask;
    UINT8 QPReduceNearIDRFrame;
    UINT8 ClassNumberLimitOfQPReduceNearIDRFrame;
    UINT8 QPReduceNearIFrame;
    UINT8 ClassNumberLimitOfQPReduceNearIFrame;
    INT8 Intra16x16Bias;    /**< -64 ~ 64 */
    INT8 Intra4x4Bias;      /**< -64 ~ 64 */
    INT8 Inter16x16Bias;    /**< -64 ~ 64 */
    INT8 Inter8x8Bias;      /**< -64 ~ 64 */
    INT8 Direct16x16Bias;   /**< -64 ~ 64 */
    INT8 Direct8x8Bias;     /**< -64 ~ 64 */
    INT8 MELambdaQpOffset;  /**< -64 ~ 64 */
    INT8 AQPStrength;       /**< 0: auto, -1: inverse AQP, 1-81: fixed strength, 1 for no AQP */
    INT8 LoopFilterAlpha;   /**< -6 ~ 6 */
    INT8 LoopFilterBeta;    /**< -6 ~ 6 */
    INT8 Alpha;
    INT8 Beta;

    UINT32 *RoiBufferAddr;
    INT8 RoiDelta[3][4];
    UINT32 ZmvThres;
} AMP_VIDEOENC_RUNTIME_QUALITY_CFG_s;

/**
 * CBR control
 */
#pragma pack(4)
/**
 * Video quality control
 */
typedef struct AMP_VIDEOENC_H264_QUALITY_CONTROL_s_ {
    UINT8 IBeatMode;  /**< 0: None (disable)
                      1: Enable handling M-1 B frames after IDR frames
                      2: Enable handling the P frame just before IDR frames
                      4: Enable handling M-1 B frames before IDR frames
                      8: Enable handling the P frame just before I frames */
    UINT8 IDRQpAdjust;      /**< QP reduction amount near IDR frames */
    UINT8 IDRClassLimit;    /**< class number up to which QP reduction can apply for frames near IDR */
    UINT8 IQpAdjust;        /**< QP reduction amount near I frames */
    UINT8 IClassLimit;      /**< class number up to which QP reduction can apply for frames near I */
    INT8 Intra16x16Bias;    /**< -64 ~ 64 */
    INT8 Intra4x4Bias;      /**< -64 ~ 64 */
    INT8 Inter16x16Bias;    /**< -64 ~ 64 */
    INT8 Inter8x8Bias;      /**< -64 ~ 64 */
    INT8 Direct16x16Bias;   /**< -64 ~ 64 */
    INT8 Direct8x8Bias;     /**< -64 ~ 64 */
    INT8 MELambdaQpOffset;
    UINT8 AutoQpStrength;   /**< 0: Automatic = existing code, 1-81: fixed strength; 1 for no AQP */
    UINT8 LoopFilterEnable; /**< Loop filter enable */
    INT8  LoopFilterAlpha;  /**< Loop filter alpha value */
    INT8  LoopFilterBeta;   /**< Loop filter beta value */
} AMP_VIDEOENC_H264_QUALITY_CONTROL_s;

#define VIDEOENC_CBR         1
#define VIDEOENC_SMART_VBR   2

typedef struct AMP_VIDEOENC_H264_BITRATE_CONTROL_s_ {
    UINT8 BrcMode;                  /**< 1: CBR 2: SmartVBR */
    UINT32 AverageBitrate;          /**< Average bitrate */
    UINT32 MinBitrate;              /**< Max. bitrate, TBM when SSP support truely SmartVBR */
    UINT32 MaxBitrate;              /**< Min. bitrate, TBM when SSP support truely SmartVBR */
} AMP_VIDEOENC_H264_BITRATE_CONTROL_s;

typedef struct AMP_VIDEOENC_H264_QP_CONTROL_s_ {
    UINT8 QpMinI;
    UINT8 QpMaxI;
    UINT8 QpMinP;
    UINT8 QpMaxP;
    UINT8 QpMinB;
    UINT8 QpMaxB;
} AMP_VIDEOENC_H264_QP_CONTROL_s;

/**
 * H.264 bitstream SPS(Sequence Parameter Set) header \n
 * please refer to ITU-T H.264
 */
typedef struct AMP_VIDEOENC_H264_SPS_s_ {
    UINT8 profile_idc;
    UINT8 level_idc;
    UINT8 frame_cropping_flag;
    UINT8 num_ref_frame; // Obsolete in A12
    UINT16 frame_crop_left_offset;
    UINT16 frame_crop_right_offset;
    UINT16 frame_crop_top_offset;
    UINT16 frame_crop_bottom_offset;
    UINT8 log2_max_frame_num_minus4; // Obsolete in A12
    UINT8 log2_max_pic_order_cnt_lsb_minus4; // Obsolete in A12
} AMP_VIDEOENC_H264_SPS_s;

/**
 * H.264 bitstream VUI of SPS header \n
 * please refer to ITU-T H.264
 */
typedef struct AMP_VIDEOENC_H264_VUI_s_ {
    UINT8 vui_enable:1;
    UINT8 aspect_ratio_info_present_flag : 1;
    UINT8 overscan_info_present_flag : 1;
    UINT8 overscan_appropriate_flag : 1;
    UINT8 video_signal_type_present_flag : 1;
    UINT8 video_full_range_flag : 1;
    UINT8 colour_description_present_flag : 1;
    UINT8 chroma_loc_info_present_flag : 1;
    UINT8 timing_info_present_flag : 1;
    UINT8 fixed_frame_rate_flag : 1;
    UINT8 nal_hrd_parameters_present_flag : 1;
    UINT8 vcl_hrd_parameters_present_flag : 1;
    UINT8 low_delay_hrd_flag : 1;   /**< == 1 - fixed_frame_rate_flag. To be removed! */
    UINT8 pic_struct_present_flag : 1;
    UINT8 bitstream_restriction_flag : 1;
    UINT8 motion_vectors_over_pic_boundaries_flag : 1;
    UINT8 aspect_ratio_idc;
    UINT16 sar_width;
    UINT16 sar_height;
    UINT8 video_format;
    UINT8 colour_primaries;
    UINT8 transfer_characteristics;
    UINT8 matrix_coefficients;
    UINT8 chroma_sample_loc_type_top_field : 4;
    UINT8 chroma_sample_loc_type_bottom_field : 4;
    UINT8 max_bytes_per_pic_denom; // Obsolete in A12
    UINT8 max_bits_per_mb_denom; // Obsolete in A12
    UINT8 log2_max_mv_length_horizontal;
    UINT8 log2_max_mv_length_vertical;
    UINT16 num_reorder_frames;
    UINT16 max_dec_frame_buffering; // Obsolete in A12
} AMP_VIDEOENC_H264_VUI_s;
#pragma pack()

/**
 * Video encode stop method
 */
typedef enum _AMP_VIDEOENC_H264_STOP_METHOD_e_ {
    AMP_VIDEOENC_STOP_IMMEDIATELY = 0,  /**< Stop at current frame */
    AMP_VIDEOENC_STOP_NEXT_IP,          /**< Stop at next I/P frame */
    AMP_VIDEOENC_STOP_NEXT_I,           /**< Stop at next I frame */
    AMP_VIDEOENC_STOP_NEXT_IDR          /**< Stop at next IDR frame */
} AMP_VIDEOENC_H264_STOP_METHOD_e;

/**
 * Video h264 bitstream specific config
 */
typedef struct AMP_VIDEOENC_H264_CFG_s_ {
    UINT8 GopM;                  /**< Distance between P frame */
    UINT8 GopN;                  /**< Distance between I frame */
    UINT8 GopIDR;                /**< Distance between IDR frame */
    UINT8 GopHierarchical;       /**< Hierarchical Gop or not */
    UINT8 Cabac;                 /**< CABAC or CAVLC */
    UINT8 StartFromBFrame;       /**< 1:start from B frame, 0: start from I frame when M > 1  */
    UINT8 Interlace;             /**< Interlaced encoding */
    UINT8 AuDelimiterType;       /**< 0: None
                                      1: AuDelimiter before SPS/PPS, with SEI
                                      2: AUDelimiter after SPS/PPS, with SEI
                                      3: No AuDelimiter, with SEI */
    UINT32 TimeScale;            /**< Frame rate time scale */
    UINT32 TickPerPicture;       /**< Frame rate tick per picture. frame_rate = TimeScale/TickPerPicture/(1 + interlace)
                                      29.97i: Timescale = 6000, TickPerPic = 1001: frame_rate = 6000/1001/(1+1)=29.97
                                      29.97p: Timescale = 3000, TickPerPic = 1001: frame_rate = 3000/1001/(1+0)=29.97
                                      59.94p: Timescale = 6000, TickPerPic = 1001: frame_rate = 6000/1001/(1+0)=59.94 */
    AMP_VIDEOENC_H264_STOP_METHOD_e StopMethod; /**< Stop method */
    UINT8 Reserved;
    UINT32 QualityLevel;            /**< Quality level */
    AMP_VIDEOENC_H264_SPS_s SPS;    /**< H.264 SPS header */
    AMP_VIDEOENC_H264_VUI_s VUI;    /**< H.264 VUI header */
    AMP_VIDEOENC_H264_BITRATE_CONTROL_s BitRateControl; /**< Bitrate control parameters */
    AMP_VIDEOENC_H264_QUALITY_CONTROL_s QualityControl; /**< Quality control parameters */
    AMP_VIDEOENC_H264_QP_CONTROL_s QPControl; /**< Qp control parameters */
} AMP_VIDEOENC_H264_CFG_s;

/**
 * Video mjpeg bitstream specific config
 */
typedef struct AMP_VIDEOENC_MJPEG_CFG_s_ {
    UINT8 FrameRateDivisionFactor;  /**< 2: 1/2, 3: 1/3 framerate of VIN */
    UINT8 *QuantMatrixAddr;         /**< Quantization matrix, size = 128 */
} AMP_VIDEOENC_MJPEG_CFG_s;


/**
 * Video bitstream spec config
 */
typedef struct AMP_VIDEOENC_BITSTREAM_CFG_s_ {
    AMP_VIDEOENC_STREAM_SPEC_e StreamSpec;     /**< stream spec */
    UINT8                      TimeLapse;      /**< Time lapse encoding, only pri_stream can trigger */
    UINT8                      VideoThumbnail; /**< Video thumbnail */
    UINT16                     PIVMaxWidth;    /**< Max PIV width, height will follow AR */
    AMP_ROTATION_e Rotation;                   /**< Encode rotation */
    union {
        AMP_VIDEOENC_H264_CFG_s H264Cfg;       /**< H.264 specific setting */
        AMP_VIDEOENC_MJPEG_CFG_s MjpgCfg;      /**< MJPEG specific setting */
        AMP_ENC_JPEG_CFG_s JpgCfg;             /**< JPEG specific setting */
    } Spec;
} AMP_VIDEOENC_BITSTREAM_CFG_s;

/**
 * Videnc handler config
 */
typedef struct AMP_VIDEOENC_HDLR_CFG_s_ {
    AMP_VIDEOENC_STREAM_ID_e StreamId;      /**< stream id */
    UINT8 EventDataReadySkipNum;            /**< fire "data ready" event to fifo per (1 + evenDataReadySkipNum) frames */
    AMP_SYSTEM_FREQ_s SysFreq;              /**< System frequency when handler state is not idle. Primary stream handler only */
    UINT8 *DspWorkBufAddr;                  /**< DSP working area address, only AMP_VIDEOENC_STREAM_PRIMARY checks this field, 0 = no change*/
    UINT32 DspWorkBufSize;                  /**< DSP working area size, only AMP_VIDEOENC_STREAM_PRIMARY checks this field, 0 = no change*/
    AMP_CALLBACK_f cbCfgUpdated;            /**< setting take effect */
    AMP_CALLBACK_f cbEvent;                 /**< Event related to the encoder */
    UINT8 EnableLowDelayLiveview:1;         /**< Enable Low delay mode or not. A9 only. other chips no effect */
    UINT8 EnableMultiView:2;                /**< Enable MultiView or not */
    UINT8 LiveViewProcMode:1;               /**< Enable Hybrid LiveView or not, A12 only */
    UINT8 LiveViewAlgoMode:1;               /**< Algo Mode in Hybrid proc mode, A12 only */
    UINT8 LiveViewOSMode:1;                 /**< Enable Express OverSampling or not, A12 only */
    UINT8 LiveViewHdrMode:1;                /**< Enable Hybrid HDR or not, A12 only */
    UINT8 Interlace:1;                      /**< Interlaced encoding */
    AMP_VIDEOENC_LAYOUT_CFG_s MainLayout;   /**< Main window layout */
    UINT32 MainTimeScale;                   /**< Frame rate time scale */
    UINT32 MainTickPerPicture;              /**< Frame rate tick per picture. frame_rate = TimeScale/TickPerPicture/(1 + interlace)
                                                 29.97i: Timescale = 6000, TickPerPic = 1001: frame_rate = 6000/1001/(1+1)=29.97
                                                 59.94p: Timescale = 6000, TickPerPic = 1001: frame_rate = 6000/1001/(1+0)=59.94 */
    AMP_ENC_BITSBUFFER_CFG_s BitsBufCfg;    /**< buffer config */
    AMP_VIDEOENC_BITSTREAM_CFG_s BitsCfg;   /**< Bitstream config */
    AMP_VIN_SOURCE_e VinSource;             /**< Vin source */
    UINT8 LiveviewOBModeEnable;             /**< Enable OB mode for liveview */
    UINT8 VinSelect;                        /**< Vin Select, 0: default main only , 1: pip only, 2: both enabled */
} AMP_VIDEOENC_HDLR_CFG_s;

/**
 * Videoenc main window config for a given handler
 */
typedef struct AMP_VIDEOENC_MAIN_CFG_s_ {
    AMP_AVENC_HDLR_s *Hdlr;
    UINT16 EventDataReadySkipNum;            /**< fire "data ready" event to fifo per (1 + evenDataReadySkipNum) frames */
    AMP_SYSTEM_FREQ_s SysFreq;              /**< System frequency when handler state is not idle. Primary stream handler only */
    UINT8 *DspWorkBufAddr;                  /**< DSP working area address, only AMP_VIDEOENC_STREAM_PRIMARY checks this field, 0 = no change*/
    UINT32 DspWorkBufSize;                  /**< DSP working area size, only AMP_VIDEOENC_STREAM_PRIMARY checks this field, 0 = no change*/
    UINT8 LiveViewProcMode:1;               /**< Enable Hybrid LiveView or not */
    UINT8 LiveViewAlgoMode:4;               /**< Algo Mode in Hybrid proc mode */
    UINT8 LiveViewOSMode:1;                 /**< Enable Express OverSampling or not */
    UINT8 LiveViewHdrMode:1;                /**< Enable Hybrid HDR or not, A12 only */
    UINT8 Interlace:1;                      /**< Interlace */
    AMP_VIDEOENC_LAYOUT_CFG_s MainLayout;   /**< Main window layout */
    UINT32 MainTimeScale;                   /**< Frame rate time scale */
    UINT32 MainTickPerPicture;              /**< Frame rate tick per picture. frame_rate = TimeScale/TickPerPicture/(1 + interlace)
                                                 29.97i: Timescale = 6000, TickPerPic = 1001: frame_rate = 6000/1001/(1+1)=29.97
                                                 29.97p: Timescale = 3000, TickPerPic = 1001: frame_rate = 3000/1001/(1+0)=29.97
                                                 59.94p: Timescale = 6000, TickPerPic = 1001: frame_rate = 6000/1001/(1+0)=59.94 */
} AMP_VIDEOENC_MAIN_CFG_s;

/**
 * Information needed to generate default H.264 header
 */
typedef struct AMP_VIDEOENC_H264_HEADER_INFO_s_ {
    UINT16 Width;
    UINT16 Height;
    UINT8 Interlace;
    UINT8 GopM;
    AMP_ROTATION_e Rotation;
} AMP_VIDEOENC_H264_HEADER_INFO_s;

/**
 * Videoenc info type
 */
typedef enum _AMP_VIDEOENC_INTO_TYPE_e_ {
    AMP_VIDEOENC_INFO_CFG        /**< current configuration */
} AMP_VIDEOENC_INFO_TYPE_e;

/**
 * Videoenc runtime framerate config
 */
typedef struct _AMP_VIDEOENC_RUNTIME_FRAMERATE_CFG_s_ {
    AMP_AVENC_HDLR_s *Hdlr;
    UINT32 Divisor;
} AMP_VIDEOENC_RUNTIME_FRAMERATE_CFG_s;

/**
 * Videoenc encoding session information
 */
typedef struct _AMP_VIDEOENC_ENCODING_INFO_s_ {
    UINT32 TotalFrames;         /**< Total encoded frames until now */
    UINT32 AverageBitrate;      /**< Average bitrate until now, unit: Kbps */
    UINT64 TotalBytes;          /**< Total encoded bytes until now, unit: Bytes */
} AMP_VIDEOENC_ENCODING_INFO_s;

/**
 * VideoEnc PIV control information, @@TBM to applib
 */
typedef struct _AMP_VIDEOENC_PIV_CTRL_s_ {
    AMBA_SENSOR_MODE_ID_u       SensorMode;    /**< PIV sensor mode */
    UINT16                      CaptureWidth;  /**< vin capture width */
    UINT16                      CaptureHeight; /**< vin capture height */
    UINT16                      MainWidth;     /**< Main encode width */
    UINT16                      MainHeight;    /**< Main encode height */
    UINT16                      AspectRatio;   /**< aspect ratio */
} AMP_VIDEOENC_PIV_CTRL_s;

/**
 * get video enc module default config for initialization
 *
 * @param [out] cfg video encode module config
 *
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpVideoEnc_GetInitDefaultCfg(AMP_VIDEOENC_INIT_CFG_s *defInitCfg);

/**
 * Initialize video encode module.\n
 * The function should only be invoked once.
 * User MUST invoke this function before using video encode module.
 * The memory pool of the module will provide by user.
 *
 * @param [in] cfg video encode module config
 *
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpVideoEnc_Init(AMP_VIDEOENC_INIT_CFG_s *cfg);

/**
 * get full default setting of the module
 *
 * @param [out] defcfg config
 *
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpVideoEnc_GetDefaultCfg(AMP_VIDEOENC_HDLR_CFG_s *defCfg);

/**
 * To set the initial config for video encode module
 *
 * @param [in] cfg config
 * @param [out] vencHdlr encoder instance pointer
 *
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpVideoEnc_Create(AMP_VIDEOENC_HDLR_CFG_s *cfg, AMP_AVENC_HDLR_s **vencHdlr);

/**
 * Delete instance
 *
 * @param [in] hdlr encoder instance
 *
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpVideoEnc_Delete(AMP_AVENC_HDLR_s *hdlr);

/**
 * Config VIN/MAIN after handler is created
 *
 * @param [in] numVin Number of vin to be configured
 * @param [in] vinCfg Vin configuration array
 * @param [in] numMain Number of main window to be configured
 * @param [in] mainCfg Main window configuration array
 *
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpVideoEnc_ConfigVinMain(UINT32 numVin, AMP_VIN_RUNTIME_CFG_s *vinCfg, UINT32 numMain, AMP_VIDEOENC_MAIN_CFG_s *mainCfg);

/**
 * Config certain ID of MainView setting after handler is created
 *
 * @param [in] MainViewID MainView ID
 * @param [in] numMain Number of main window to be configured
 * @param [in] mainCfg Main window configuration array
 * @param [in] VerticalFlip need vertical flip when capture
 *
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpVideoEnc_ConfigMainView(UINT32 MainViewID, UINT32 numMain, AMP_VIDEOENC_MAIN_CFG_s *mainCfg, UINT8 VerticalFlip);

/**
 * get current layout
 *
 * @param [in] hdlr encoder instance
 * @param [in] layout current layout
 *
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpVideoEnc_GetLayout(AMP_AVENC_HDLR_s *hdlr, AMP_VIDEOENC_LAYOUT_CFG_s *layout);

/**
 * Config bitstream before encoding
 *
 * @param [in] hdlr encoder instance
 * @param [in] cfg config
 *
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
int AmpVideoEnc_SetBitstreamConfig(AMP_AVENC_HDLR_s *hdlr, AMP_VIDEOENC_BITSTREAM_CFG_s *cfg);

/**
 * Config bitstream buffer before encoding
 *
 * @param [in] hdlr encoder instance
 * @param [in] cfg buffer config
 *
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
int AmpVideoEnc_SetBitstreamBuffer(AMP_AVENC_HDLR_s *hdlr, AMP_ENC_BITSBUFFER_CFG_s *cfg);


/**
 * Get current bitstream config
 *
 * @param [in] hdlr encoder instance
 * @param [in] cfg config
 *
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
int AmpVideoEnc_GetBitstreamConfig(AMP_AVENC_HDLR_s *hdlr, AMP_VIDEOENC_BITSTREAM_CFG_s *cfg);

/**
 * Runtime change Bits runout threshold
 *
 * @param [in] hdlr encoder instance
 * @param [in] cfg runtime control configuration
 *
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
int AmpVideoEnc_SetRuntimeBitstreamThreshold(AMP_AVENC_HDLR_s *hdlr, UINT32 *bitsThreshold);

/**
 * Get default H.264 headers base on current configuration
 *
 * @param [in] info information for header generation
 * @param [out] sps default SPS header
 * @param [out] vui default VUI header
 *
 * @return none
 */
void AmpVideoEnc_GetDefaultH264Header(AMP_VIDEOENC_H264_HEADER_INFO_s *info, AMP_VIDEOENC_H264_SPS_s *sps, AMP_VIDEOENC_H264_VUI_s *vui);

/**
 * Runtime control bitrate during video recording state
 *
 * @param [in] hdlr encoder instance
 * @param [in] cfg runtime control configuration
 *
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
int AmpVideoEnc_SetRuntimeBitrate(AMP_AVENC_HDLR_s *hdlr, AMP_VIDEOENC_RUNTIME_BITRATE_CFG_s *cfg);

/**
 * Runtime control quality during video recording state
 *
 * @param [in] hdlr encoder instance
 * @param [in] cfg runtime control configuration
 *
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
int AmpVideoEnc_SetRuntimeQuality(AMP_AVENC_HDLR_s *hdlr, AMP_VIDEOENC_RUNTIME_QUALITY_CFG_s *cfg);

/**
 * Capture & encode one frame when time lapsed recoding
 *
 * @param [in] hdlr Encoder instance
 *
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
int AmpVideoEnc_CaptureTimeLapsedFrame(AMP_VIN_HDLR_s *Vin);

/**
 * Runtime change framerate
 *
 * @param [in] numHdlr number of handlers to change
 * @param [in] cfg handler and config array
 *
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
int AmpVideoEnc_SetRuntimeFrameRate(UINT32 numHdlr, AMP_VIDEOENC_RUNTIME_FRAMERATE_CFG_s *cfg);

/**
 * Set encode blend info
 *
 * @param [in] hdlr Encoder instance
 * @param [in] info Blend info
 *
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
int AmpVideoEnc_SetEncodeBlend(AMP_AVENC_HDLR_s *hdlr, AMP_VIDEOENC_BLEND_INFO_s *info);

/**
 * Feed raw/yuv for video RawEcnode
 *
 * @param [in] raw raw buffer information
 * @param [in] lastFrame indicate is last encode frame
 *
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
int AmpVideoEnc_FeedEncodeRaw(AMP_ENC_RAW_INFO_s *raw, UINT8 lastFrame);

/**
 * Get encoding session information
 *
 * @param [in] hdlr Encoder instance
 * @param [out] info Encoding info
 *
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
int AmpVideoEnc_GetEncodingInfo(AMP_AVENC_HDLR_s *hdlr, AMP_VIDEOENC_ENCODING_INFO_s *info);

/**
 * Reset encoding session information (to start over gathering encoding statistics)
 *
 * @param [in] hdlr Encoder instance
 *
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
int AmpVideoEnc_ResetEncodingInfo(AMP_AVENC_HDLR_s *hdlr);

/**
 * @}
 */

#endif /* _VIDEOENC_H_ */
