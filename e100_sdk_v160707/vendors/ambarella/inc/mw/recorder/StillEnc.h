 /**
  * @file inc/mw/recorder/Stillenc.h
  *
  * Amba still codec header
  *
  * Copyright (C) 2013, Ambarella, Inc.
  *
  * All rights reserved. No Part of this file may be reproduced, stored
  * in a retrieval system, or transmitted, in any form, or by any means,
  * electronic, mechanical, photocopying, recording, or otherwise,
  * without the prior consent of Ambarella, Inc.
  */

#ifndef _STILLENC_H_
#define _STILLENC_H_

/**
 * @defgroup StillEnc
 * @brief Still Encode module implementation
 *
 * Implementation the Still Encode module, include below function implementation
 *  1. Initialize Still Encode module function.
 *  2. Create Still Encode module function.
 *  3. Delete Still Encode module function.
 *  4. Configure Still Encode module window layout
 *  5. Trigger to start raw capture
 *  6. Update window layout in IDLE state
 *  7. Control LiveView capture between LiveView and StillCapture
 *
 */

/**
 * @addtogroup StillEnc
 * @ingroup Codec
 * @{
 */

#include <mw.h>
#include <recorder/Encode.h>
#include <vin/vin.h>
#include "AmbaDSP_EventInfo.h"

/*****************************
 *   Macro                   *
 *****************************/
#define AMP_COMPRESSED_RAW_WIDTH(cols)    (ALIGN_32((cols*27)>>5))

/*****************************
 *   Enum                    *
 *****************************/
/**
 *  Ambarella multiple stream definition
 */
typedef enum _AMP_STILLENC_STREAM_ID_e_ {
    AMP_STILLENC_STREAM_PRIMARY = 0,
    AMP_STILLENC_STREAM_STREAM_ID_NUM
} AMP_STILLENC_STREAM_ID_e;

/**
 *  Encoder type
 */
typedef enum _AMP_STILLENC_STREAM_SPEC_e_ {
    AMP_STILLENC_CODER_JPEG,            /**< Individual JPEG */
    AMP_STILLENC_CODER_MPO,             /**< Individual 3D MPO */
    AMP_STILLENC_CODER_STREAM_TYPE_NUM
} AMP_STILLENC_STREAM_SPEC_e;

/**
 *  CFA raw type
 */
typedef enum _AMP_STILLENC_RAW_TYPE_e_ {
    AMP_STILLENC_RAW_COMPR = 0,         /**< AMBA compressed raw */
    AMP_STILLENC_RAW_UNCOMPR,           /**< uncompressed raw */
    AMP_STILLENC_RAW_COMPACT_14,        /**< 14bit pack umcompressed raw */
    AMP_STILLENC_RAW_COMPACT_12,        /**< 12bit pack umcompressed raw */
    AMP_STILLENC_RAW_COMPACT_10,        /**< 10bit pack umcompressed raw */
    AMP_STILLENC_RAW_NUM
} AMP_STILLENC_RAW_TYPE_e;

/*****************************
 *   Structure               *
 *****************************/
/**
 *  init config
 */
typedef struct _AMP_STILLENC_INIT_CFG_s_ {
    UINT8 MaxLayerPerEncoder;           /**< Maximum number of layer per encoder layout */
    AMP_YUV_COLOR_s BgColor;            /**< Background color of LiveView */
    AMP_TASK_INFO_s MainTaskInfo;       /**< general task information */
    AMP_TASK_INFO_s RawCapTaskInfo;     /**< raw caputre task information */
    AMP_TASK_INFO_s RawEncTaskInfo;     /**< raw encode task information */
    UINT32 MsgQueueNum;                 /**< number of entries per message queue */
    UINT8* MemoryPoolAddr;              /**< working buffer start address */
    UINT32 MemoryPoolSize;              /**< size of the buffer */
    UINT32 ScrptBufSize;                /**< size of Still encode script buffer */
} AMP_STILLENC_INIT_CFG_s;

typedef struct AMP_STILLRAWCAP_INIT_CFG_s_ {
    UINT8 MaxLayerPerEncoder;           /**< Maximum number of layer per encoder layout */
    AMP_TASK_INFO_s TaskInfo;           /**< vdsp/general task information */
    UINT32 MsgQueueNum;                 /**< number of entries per message queue */
    UINT8* MemoryPoolAddr;              /**< working buffer start address */
    UINT32 MemoryPoolSize;              /**< size of the buffer */
    UINT32 MemoryUsage;                 /**< memory usage */
} AMP_STILLRAWCAP_INIT_CFG_s;

typedef struct AMP_STILLRAWENC_INIT_CFG_s_ {
    UINT8 MaxLayerPerEncoder;           /**< Maximum number of layer per encoder layout */
    AMP_TASK_INFO_s TaskInfo;           /**< vdsp/general task information */
    UINT32 MsgQueueNum;                 /**< number of entries per message queue */
    UINT8* MemoryPoolAddr;              /**< working buffer start address */
    UINT32 MemoryPoolSize;              /**< size of the buffer */
    UINT32 MemoryUsage;                 /**< memory usage */
} AMP_STILLRAWENC_INIT_CFG_s;

/**
 * Stillenc handler config
 */
typedef struct _AMP_STILLENC_HDLR_CFG_s_ {
    /* LiveView */
    AMP_SYSTEM_FREQ_s SysFreq;              /**< System frequency when handler state is not idle. Primary stream handler only */
    UINT8 *DspWorkBufAddr;                  /**< DSP working area address, only AMP_VIDEOENC_STREAM_PRIMARY checks this field, 0 = no change*/
    UINT32 DspWorkBufSize;                  /**< DSP working area size, only AMP_VIDEOENC_STREAM_PRIMARY checks this field, 0 = no change*/
    AMP_CALLBACK_f cbCfgUpdated;            /**< setting take effect */
    AMP_CALLBACK_f cbEvent;                 /**< Event related to the encoder */
    UINT8 EnableMultiView:2;                /**< Enable MultiView or not */
    UINT8 LiveViewProcMode:1;               /**< Enable Hybrid LiveView or not */
    UINT8 LiveViewAlgoMode:2;               /**< Algo Mode in Hybrid proc mode */
    UINT8 LiveViewOSMode:1;                 /**< Enable Express OverSampling or not */
    UINT8 LiveViewHdrMode:1;                /**< Enable Hybrid HDR or not, A12 only */
    UINT8 Interlace:1;                      /**< Interlaced encoding */
    AMP_VIDEOENC_LAYOUT_CFG_s MainLayout;   /**< Main window layout */
    UINT32 MainTimeScale;                   /**< Frame rate time scale */
    UINT32 MainTickPerPicture;              /**< Frame rate tick per picture. frame_rate = TimeScale/TickPerPicture/(1 + interlace)
                                                 29.97i: Timescale = 60000, TickPerPic = 1001: frame_rate = 60000/1001/(1+1)=29.97
                                                 59.94p: Timescale = 60000, TickPerPic = 1001: frame_rate = 60000/1001/(1+0)=59.94 */
    /* Still rawcap/rawenc */
    AMP_ENC_BITSBUFFER_CFG_s BitsBufCfg;       /**< buffer config */
    AMP_VIDEOENC_BITSTREAM_CFG_s BitsCfg;   /**< Bitstream config */
    UINT8 LiveviewOBModeEnable;                /**< Enable OB mode for liveview */
} AMP_STILLENC_HDLR_CFG_s;


/**
 * Stillenc main window config for a given handler
 */
typedef struct _AMP_STILLENC_MAIN_CFG_s_ {
    AMP_STLENC_HDLR_s *Hdlr;
    AMP_SYSTEM_FREQ_s SysFreq;              /**< System frequency when handler state is not idle. Primary stream handler only */
    UINT8 *DspWorkBufAddr;                  /**< DSP working area address, only AMP_VIDEOENC_STREAM_PRIMARY checks this field, 0 = no change*/
    UINT32 DspWorkBufSize;                  /**< DSP working area size, only AMP_VIDEOENC_STREAM_PRIMARY checks this field, 0 = no change*/
    UINT8 LiveViewProcMode:1;               /**< Enable Hybrid LiveView or not */
    UINT8 LiveViewAlgoMode:4;               /**< Algo Mode in Hybrid proc mode */
    UINT8 LiveViewOSMode:1;                 /**< Enable Express OverSampling or not */
    UINT8 LiveViewHdrMode:1;                /**< Enable Hybrid HDR or not, A12 only */
    UINT8 Interlace:1;                      /**< Interlaced encoding */
    AMP_VIDEOENC_LAYOUT_CFG_s MainLayout;   /**< Main window layout */
    UINT32 MainTimeScale;                   /**< Frame rate time scale */
    UINT32 MainTickPerPicture;              /**< Frame rate tick per picture. frame_rate = TimeScale/TickPerPicture/(1 + interlace)
                                                 29.97i: Timescale = 6000, TickPerPic = 1001: frame_rate = 6000/1001/(1+1)=29.97
                                                 29.97p: Timescale = 3000, TickPerPic = 1001: frame_rate = 3000/1001/(1+0)=29.97
                                                 59.94p: Timescale = 6000, TickPerPic = 1001: frame_rate = 6000/1001/(1+0)=59.94 */
} AMP_STILLENC_MAIN_CFG_s;


/*****************************
 *   Still Encode script     *
 *****************************/
/** Encode Script command code
 *  defined as an unsigned 32-bits dword
 *
 * +-----------+-----------+----------+
 * | bit 31-21 | bit 20-16 | bit 15-0 |
 * +-----------+-----------+----------+
 * |    0x0    |  codecID  | cmdCode  |
 * +-----------+-----------+----------+
 *
 */
#define STILLENC_CODEC_ID    0x0
#define AMP_STILL_ENC_SCRPT(x) ((((STILLENC_CODEC_ID&0x1F)<<16)|(x&0xFFFF))&0x1FFFFF)
#define SENC_GENCFG      AMP_STILL_ENC_SCRPT(0x0000)      /**< execute script general config */
#define SENC_RAWCAP      AMP_STILL_ENC_SCRPT(0x0001)      /**< execute raw capture */
#define SENC_RAW2YUV     AMP_STILL_ENC_SCRPT(0x0002)      /**< execute raw to yuv conversion */
#define SENC_YUV2JPG     AMP_STILL_ENC_SCRPT(0x0003)      /**< execute yuv to jpeg encode */
#define SENC_RAW2RAW     AMP_STILL_ENC_SCRPT(0x0004)      /**< execute raw to raw */
#define SENC_YUV2YUV     AMP_STILL_ENC_SCRPT(0x0005)      /**< execute yuv to yuv */
#define SENC_YUVBLND     AMP_STILL_ENC_SCRPT(0x0006)      /**< execute yuv blending */
#define SENC_YUVCOMB     AMP_STILL_ENC_SCRPT(0x0007)      /**< execute yuv combination */
#define SENC_RAW2HDR     AMP_STILL_ENC_SCRPT(0x0008)      /**< execute raw to hdr */
#define SENC_RAWOPER     AMP_STILL_ENC_SCRPT(0x0009)      /**< execute raw operation(subtract/accumulate) */
#define SENC_RAW2ME      AMP_STILL_ENC_SCRPT(0x000A)      /**< execute raw to me estimate */
#define SENC_YUV2NP      AMP_STILL_ENC_SCRPT(0x000B)      /**< execute yuv to NightPortrait */
#define SENC_RAW2MFH     AMP_STILL_ENC_SCRPT(0x000C)      /**< execute raw to MFHISO */

/**
 *  Scap area description
 */
typedef struct _AMP_RAW_CAPTURE_AREA_s_ {
    UINT32     VcapWidth;      /**< Vcap capture width */
    UINT32     VcapHeight;     /**< Vcap capture height */
    AMP_AREA_s EffectArea;     /**< effective capture area */
} AMP_RAW_CAPTURE_AREA_s;

/**
 *  Single capture Cont. parameters; //20131108 Chester: Temp create for PES verification, will be remove someday
 */
typedef struct _AMP_RAW_CAPTURE_PARAM_s_ {
    UINT8   EncID;          /**< encode mode ID */
    UINT8   EncLoop;        /**< Jpeg encode loop */
    UINT8   Compressed;     /**< CFA raw capture format */
    UINT8   CaptureCount;   /**< desired CFA raw capture number */
    UINT32  Iso;            /**< yuv ISO mode */
    UINT32  TargetSize;     /**< Jpeg target size */
} AMP_RAW_CAPTURE_PARAM_s;
/**
 *  Buffer allocation type
 */
typedef enum _AMP_ENC_SCRPT_BUFF_RULE_e_ {
    AMP_ENC_SCRPT_BUFF_FIXED = 0,       /**< buffer is provide in fixed address */
    AMP_ENC_SCRPT_BUFF_RING,            /**< ring buffer, APP provide strating address */
    AMP_ENC_SCRPT_BUFF_NUM
} AMP_ENC_SCRPT_BUFF_RULE_e;

/**
 *  Stop script running rule
 */
typedef enum _AMP_ENC_SCRPT_STOP_RULE_e_ {
    AMP_ENC_SCRPT_STOP_RIGHT_AWAY = 0,   /**< stop running script right away and discard all pending data*/
    AMP_ENC_SCRPT_STOP_CAPTURE_RAW,      /**< stop capture raw, but wait for all pending raw to do encode */
    AMP_ENC_SCRPT_STOP_PRE_CAPTURE,      /**< keep capture some raw then do encode (this is only for pre-capture mode) */
    AMP_ENC_SCRPT_STOP_NUM
} AMP_ENC_SCRPT_STOP_SCRPT_RULE_e;

/**
 *  QuickView configuration
 */
typedef struct _AMP_ENC_SCRP_QV_CFG_s_ {
    UINT8              DisableLCDQV:1;  /**< disable LCD quickview */
    UINT8              DisableHDMIQV:1; /**< disable HDMI quickview */
    UINT8              fastQuickView:1; /**< Enable fast quickview */
    UINT8              QVZoom:1;        /**< Enable QV Zoom function */
    UINT8              QV3dMode:4;      /**< QV 3d mode */
    UINT32             QVDelay_ms;      /**< QuickView delay time(msec unit) before back to LiveView */
    AMP_COLOR_FORMAT_e LCDDataFormat;   /**< LCD yuv data format */
    UINT8              *LCDLumaAddr;    /**< Desired LCD QV buffer */
    UINT8              *LCDChromaAddr;  /**< Desired LCD QV buffer */
    UINT16             LCDWidth;        /**< Desired LCD Width */
    UINT16             LCDHeight;       /**< Desired LCD Height */
    AMP_COLOR_FORMAT_e HDMIDataFormat;  /**< HDMI yuv data format */
    UINT8              *HDMILumaAddr;   /**< Desired HDMI QV buffer */
    UINT8              *HDMIChromaAddr; /**< Desired HDMI QV buffer */
    UINT16             HDMIWidth;       /**< Desired HDMI Width */
    UINT16             HDMIHeight;      /**< Desired HDMI Height */
} AMP_ENC_SCRP_QV_CFG_s;

/**
 *  Back to LiveView Timing
 */
typedef enum _AMP_ENC_SCRPT_B2LV_e_ {
    AMP_ENC_SCRPT_B2LV_NONE = 0,        /**< Do not care about back to LiveView, let APP do it */
    AMP_ENC_SCRPT_B2LV_JPEG_DONE,       /**< after whole jpeg encode is done */
    AMP_ENC_SCRPT_B2LV_RAW_DONE,        /**< after whole raw capture is done */
    AMP_ENC_SCRPT_B2LV_YUV_DONE,        /**< after whole yuv convert is done */
    AMP_ENC_SCRPT_B2LV_NUM
} AMP_ENC_SCRPT_B2LV_e;

/**
 * still encode postprocess yuv information
 */
typedef struct _AMP_STILLENC_POSTP_YUV_INFO_s_ {
    AMBA_DSP_YUV_FORMAT_e DataFormat;   /**< still Yuv data format */
    UINT8 *LumaAddr;                    /**< Main Y buffer address */
    UINT8 *ChromaAddr;                  /**< Main UV buffer address */
    UINT16 Pitch;                       /**< Main Yuv buffer pitch */
    UINT16 Width;                       /**< Main Yuv buffer width */
    UINT16 Height;                      /**< Main Yuv buffer height */

    UINT8 *ScrnLumaAddr;                /**< Screennail Y buffer address */
    UINT8 *ScrnChromaAddr;              /**< Screennail UV buffer address */
    UINT16 ScrnPitch;                   /**< Screennail Yuv buffer pitch */
    UINT16 ScrnWidth;                   /**< Screennail Yuv buffer width */
    UINT16 ScrnHeight;                  /**< Screennail Yuv buffer height */

    UINT8 *ThmLumaAddr;                 /**< Thumbnail Y buffer address */
    UINT8 *ThmChromaAddr;               /**< Thumbnail UV buffer address */
    UINT16 ThmPitch;                    /**< Thumbnail Yuv buffer pitch */
    UINT16 ThmWidth;                    /**< Thumbnail Yuv buffer width */
    UINT16 ThmHeight;                   /**< Thumbnail Yuv buffer height */
} AMP_STILLENC_POSTP_YUV_INFO_s;

/**
 * still encode post-process information
 */
typedef struct _AMP_STILLENC_POSTP_INFO_s_ {
    UINT32 StageCnt;           /**< script stage count */
    UINT8 State;               /**< encoder state */
    UINT8 IsRaw2Raw3A:1;       /**< indicate r2r is for AAA stats */
    UINT8 IsRaw2RawResample:1; /**< indicate r2r is for resample */
    UINT8 Reserved:6;
    UINT16 CfaIndex;           /**< CFA index for multi-slice raw */

    union {
        AMP_ENC_RAW_INFO_s                  RawInfo;        /**< CFA raw PP info */
        AMP_STILLENC_POSTP_YUV_INFO_s       YuvInfo;        /**< Yuv PP info */
        AMBA_DSP_EVENT_STILL_CFA_3A_DATA_s  *CfaStatInfo;   /**< CFA raw statistic info */
    } media;
} AMP_STILLENC_POSTP_INFO_s;

/**
 * still encode pre-process information
 */
typedef struct _AMP_STILLENC_PREP_INFO_s_ {
    UINT8 State;               /**< encoder state */
    UINT8 IsRaw2Raw3A:1;       /**< indicate r2r is for AAA stats */
    UINT8 IsRaw2RawResample:1; /**< indicate r2r is for resample */
    UINT8 Reserved:6;
    UINT8 Reserved1[2];
    UINT16 AeIdx;              /**< indicate current ae index to get correct ae info from 3a */
    UINT16 CfaIndex;           /**< indicate cfa tile index */
    UINT32 StageCnt;           /**< indicate current stage count */
    UINT32 JpegSerialNumber;   /**< indicate Jpeg Group Serial Number for Muxer to identify */
} AMP_STILLENC_PREP_INFO_s;

/**
 *  This data structure describes the interface of MW postproc handler
 */
typedef struct _AMP_STILLENC_POSTP_s_ {
    UINT32 (*Process)(AMP_STILLENC_POSTP_INFO_s *PostpInfo);
} AMP_STILLENC_POSTP_s;

/**
 *  This data structure describes the interface of MW preproc handler
 */
typedef struct _AMP_STILLENC_PREP_s_ {
    UINT32 (*Process)(AMP_STILLENC_PREP_INFO_s *PrepInfo);
} AMP_STILLENC_PREP_s;

/**
 * still encode Jpeg BRC control information
 */
typedef struct _AMP_STILLENC_BRC_CTRL_s_ {
    UINT8                     *MainQTAddr;    /**< main quantization matrix, size = 128 */
    UINT8                     *ThmQTAddr;     /**< thumbnail quantization matrix, size = 128 */
    UINT8                     *ScrnQTAddr;    /**< screennail quantization matrix, size = 128 */
    UINT16                    TargetBitRate;  /**< target bit per pixel */
    UINT8                     Tolerance;      /**< bitrate tolerance, percentage unit(10 means 10%) */
    UINT8                     MaxEncLoop;     /**< maximum re-encode loop, TBD: ucode can not support loop>3 until multi-slice feature ready */
    UINT32 (*JpgBrcCB)(UINT16 targetBpp, UINT32 stillProc, UINT8* predictQ, UINT8 *curveNum, UINT32 *curveAddr); /**< Jpeg BRC callback function */
} AMP_STILLENC_BRC_CTRL_s;

/**
 * PIV OSD blending (date/time stamp) info
 */
typedef struct _AMP_PIVENC_BLEND_INFO_s_ {
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
} AMP_PIVENC_BLEND_INFO_s;


//128byte alignment per cmd
/**
 * SENC_GENCFG config structure
 */
typedef struct _AMP_SENC_SCRPT_GENCFG_s_ { //
    UINT32                    Cmd;
    UINT32                    RawToCap;            /**< raw capture number in one round */
    UINT32                    StillProcMode;       /**< still process mode */
    UINT32                    PreRawNumber;        /**< Pre-capture: keep how many raw file before release shutter key */
    UINT32                    PostRawNumber;       /**< Pre-capture: keep how many raw file after release shutter key */
    UINT8                     EndlessCapture:1;    /**< Executer capture until app terminate or reach target */
    UINT8                     RawEncRepeatStage:4; /**< Indicate which stage to execute repeat script*/
    UINT8                     RawEncRepeat:1;      /**< Execute script repeatedly until all raw files consumed */
    UINT8                     ScrnEnable:1;        /**< enable screennail */
    UINT8                     ThmEnable:1;         /**< enable thumbnail */
    UINT8                     CapProfEnable:1;     /**< enable capture profiling or not */
    UINT8                     CapProfShow:1;       /**< print capture profiling anchor or not */
    UINT8                     Reserved0:6;
    UINT8                     Reserved[2];
    AMP_ENC_SCRP_QV_CFG_s     QVConfig;            /**< QuickView config */
    AMP_ENC_SCRPT_B2LV_e      b2LVCfg;             /**< back to LiveView rule */
    AMBA_DSP_ROTATE_FLIP_e    EncRotateFlip;       /**< encode rotate flip setting */
    AMP_STILLENC_POSTP_s      *PostProc;           /**< postProc callback */
    AMP_STILLENC_PREP_s       *PreProc;            /**< postProc callback */

    AMP_STILLENC_BRC_CTRL_s   BrcCtrl;             /**< Jpeg BRC control information */

    UINT8                     RawDataBits;         /**< Raw file data bits, if raw comes from memory */
    UINT8                     RawBayerPattern;     /**< Raw file bayer pattern, if raw comes from memory */
    AMP_YUV_BUFFER_s          MainBuf;             /**< YUV main info */
    AMP_YUV_BUFFER_s          ScrnBuf;             /**< YUV screennail info, active_w/h in AOI */
    UINT16                    ScrnWidth;           /**< YUV screennail width */
    UINT16                    ScrnHeight;          /**< YUV screennail height */
    AMP_YUV_BUFFER_s          ThmBuf;              /**< YUV thumbnail info, acitive_w/h in AOI */
    UINT16                    ThmWidth;            /**< YUV thumbnail width */
    UINT16                    ThmHeight;           /**< YUV thumbnail height */
    AMP_PIVENC_BLEND_INFO_s   PIVMainBlendInfo[AMBA_DSP_MAX_OSD_BLEND_AREA_NUMBER]; /* OSD blending config for Main, (Scr+Thm) will follow main in the Express mode*/
    AMP_PIVENC_BLEND_INFO_s   PIVScrnBlendInfo[AMBA_DSP_MAX_OSD_BLEND_AREA_NUMBER]; /* OSD blending config for Scrn */
    AMP_PIVENC_BLEND_INFO_s   PIVThmBlendInfo[AMBA_DSP_MAX_OSD_BLEND_AREA_NUMBER]; /* OSD blending config for Thm */
} AMP_SENC_SCRPT_GENCFG_s;

/**
 * still encode raw capture control information
 */
typedef struct _AMP_STILLENC_RAWCAP_FLOW_CTRL_s_ {
    AMBA_DSP_CHANNEL_ID_u       VinChan;       /**< vin channel, Bits.VinID is uniqueID, Bits.SensorID is Bitmap */
    AMBA_SENSOR_ESHUTTER_TYPE_e ShType;        /**< shutter type */
    AMBA_SENSOR_MODE_ID_u       SensorMode;    /**< still capture sensor mode */
    UINT32                      AeIdx;         /**< capture index(0~0xFFFFFFFF) of group */
    UINT32                      TotalRawToCap; /**< desired capture number */
    UINT32                      VcapWidth;     /**< capture index(0~0xFFFFFFFF) of group */
    UINT32                      VcapHeight;    /**< capture index(0~0xFFFFFFFF) of group */
    void (*OBMode)(UINT32 *modeId);            /**< ob mode callback */
} AMP_STILLENC_RAWCAP_FLOW_CTRL_s;

/**
 * still encode rawcapture CB structure
 */
typedef struct _AMP_STILLENC_RAWCAP_CB_s_ {
    UINT32 (*RawCapCB)(AMP_STILLENC_RAWCAP_FLOW_CTRL_s *Ctrl);          /**< first/single shot raw capture callback */
    UINT32 (*RawCapMultiCB)(AMP_STILLENC_RAWCAP_FLOW_CTRL_s *Ctrl);     /**< Multi shot raw capture callback */
    UINT32 (*RawCapMultiPrepCB)(AMBA_DSP_CHANNEL_ID_u VinChan,UINT32 AeIdx); /**< Multi shot preparation callback */
    UINT32 (*PreFlashCB)(void *Param);        /**< Preflash callback */
    UINT32 (*OBModeCB)(void *Param);          /**< ob mode check callback */
} AMP_STILL_RAWCAP_CB_s;

/**
 * still encode raw capture dsp control information
 */
typedef struct _AMP_STILLENC_RAWCAP_DSP_CTRL_s_ {
    AMBA_DSP_CHANNEL_ID_u   VinChan;      /**< vin channel */
    UINT8                   VidSkip;      /**< skip frame number */
    UINT32                  RawCapNum;    /**< raw capture number in one round */
    UINT32                  StillProc;    /**< still process mode */
} AMP_STILLENC_RAWCAP_DSP_CTRL_s;

/**
 * SENC_RAWCAP config structure
 */
typedef struct _AMP_SENC_SCRPT_RAWCAP_s_ { //
    UINT32                      Cmd;
    UINT8                       EnableOB:1;     /**< enable OB mode or not*/
    UINT8                       Reserved:7;
    UINT8                       Reserved1[3];
    AMP_ENC_SOURCE_TYPE_e       SrcType;        /**< from VIN or memory */
    AMBA_SENSOR_ESHUTTER_TYPE_e ShType;         /**< shutter type */
    AMBA_SENSOR_MODE_ID_u       SensorMode;     /**< still capture sensor mode */

    AMP_RAW_CAPTURE_AREA_s      FvRawCapArea;   /**< fv raw capture area */
    AMP_STILLENC_RAW_TYPE_e     FvRawType;      /**< fv raw type which will be saved to buffer */
    AMP_2D_BUFFER_s             FvRawBuf;       /**< destination of fv raw buff */
    AMP_ENC_SCRPT_BUFF_RULE_e   FvBufRule;      /**< FV buffer allocate rule */
    UINT32                      FvRingBufSize;  /**< total buffer size when bufRule=ring_buf */

    AMP_STILLENC_RAW_TYPE_e     QvRawType;      /**< qv raw type which will be saved to buffer */
    AMP_2D_BUFFER_s             QvRawBuf;       /**< destination of qv raw buff */
    AMP_ENC_SCRPT_BUFF_RULE_e   QvBufRule;      /**< QV buffer allocate rule */
    UINT32                      QvRingBufSize;  /**< total buffer size when bufRule=ring_buf */
    AMP_STILL_RAWCAP_CB_s       CapCB;          /**< raw capture relative callback structure */
} AMP_SENC_SCRPT_RAWCAP_s;

/**
 * SENC_RAW2YUV config structure
 */
typedef struct AMP_SENC_SCRPT_RAW2YUV_s_ { //
    UINT32                    Cmd;
    AMP_STILLENC_RAW_TYPE_e   RawType;          /**< raw type which will be save to buffer */
    AMP_2D_BUFFER_s           RawBuf;           /**< destination of raw buff */
    AMP_ENC_SCRPT_BUFF_RULE_e RawBufRule;       /**< buffer allocate rule */
    UINT32                    RingBufSize;      /**< total buffer size when bufRule=ring_buf */
    AMP_ENC_SCRPT_BUFF_RULE_e YuvBufRule;       /**< buffer allocate rule */
    UINT32                    YuvRingBufSize;   /**< total buffer size when bufRule=ring_buf */
} AMP_SENC_SCRPT_RAW2YUV_s;

/**
 * SENC_YUV2JPG config structure
 */
typedef struct AMP_SENC_SCRPT_YUV2JPG_s_ { //
    UINT32                    Cmd;
    AMP_ENC_SCRPT_BUFF_RULE_e YuvBufRule;       /**< buffer allocate rule */
    UINT32                    YuvRingBufSize;   /**< total buffer size when bufRule=ring_buf */
} AMP_SENC_SCRPT_YUV2JPG_s;

/**
 * SENC_RAW2RAW config structure
 */
typedef struct _AMP_STILLENC_RAW2RAW_ROI_s_ { //
    UINT16                    RoiColStart;      /**< roi column start x */
    UINT16                    RoiRowStart;      /**< roi row start y */
    UINT16                    RoiWidth;         /**< roi tile width, TBD: HW limitation */
    UINT16                    RoiHeight;        /**< roi tile height, TBD: HW limitation */
} AMP_STILLENC_RAW2RAW_ROI_s;

/**
 * SENC_RAW2RAW config structure
 */
typedef struct _AMP_SENC_SCRPT_RAW2RAW_s_ { //
    UINT32                    Cmd;
    AMP_STILLENC_RAW_TYPE_e   SrcRawType;        /**< raw type which will be save to buffer */
    AMP_2D_BUFFER_s           SrcRawBuf;         /**< source of raw buff */
    AMP_ENC_SCRPT_BUFF_RULE_e RawBufRule;

    AMP_STILLENC_RAW_TYPE_e   DstRawType;        /**< raw type which will be save to buffer */
    AMP_2D_BUFFER_s           DstRawBuf;         /**< destination of raw buff */

    /* ROI setting */
    UINT16                    TileNumber;        /**< total tile number */
    UINT32                    TileListAddr;      /**< tile list address */
    UINT32 (*IdspCfgSetting)(UINT16 Index);      /**< idsp config setting callback */
    UINT32                    Raw3AStatAddr;     /**< CFA 3A stat buffer address */
    UINT32                    Raw3AStatSize;     /**< CFA 3A stat buffer size */
} AMP_SENC_SCRPT_RAW2RAW_s;

/**
 * SENC_YUV2YUV config structure
 * USAGE : 1) yuv2yuv resample
 *         2) yuv2yuv idsp batch process (TBD)
 */
typedef struct AMP_SENC_SCRPT_YUV2YUV_s_ { //
    UINT32                    Cmd;
    AMP_YUV_BUFFER_s          SrcMainBuf;     /**< Source main buffer */
    AMP_YUV_BUFFER_s          DstMainBuf;     /**< destination yuv main buffer */
    AMP_YUV_BUFFER_s          ScrnBuf;        /**< yuv screennail buffer, active_w/h in AOI */
    UINT16                    ScrnWidth;      /**< screennail active + black_bar width */
    UINT16                    ScrnHeight;     /**< screennail active + black_bar height */
    AMP_YUV_BUFFER_s          ThmBuf;         /**< yuv thumbnail buffer, active_w/h in AOI */
    UINT16                    ThmWidth;       /**< thumbnail active + black_bar width */
    UINT16                    ThmHeight;      /**< thumbnail active + black_bar height */
    UINT8                     IdspBatchType;  /**< idsp batch buffer type */
    UINT32                    IdspBatchAddr;  /**< idsp batch buffer address */
} AMP_SENC_SCRPT_YUV2YUV_s;

/**
 *  Back to LiveView Timing
 */
typedef enum _AMP_SCRPT_MODE_e_ {
    AMP_SCRPT_MODE_STILL = 0,       /**< still mode */
    AMP_SCRPT_MODE_VIDEO,           /**< video mode */
    AMP_SCRPT_MODE_DECODE,          /**< decode mode */
    AMP_SCRPT_MODE_NUM
} AMP_SCRPT_MODE_e;

/**
 * Script structure
 */
typedef struct _AMP_SCRPT_CONFIG_s_ {
    AMP_SCRPT_MODE_e mode;               /**< Script mode */
    UINT32           (*StepPreproc)(void *StepInfo);
    UINT32           (*StepPostproc)(void *StepInfo, void *PpInfo);
    UINT32           ScriptStartAddr;    /**< Script start address */
    UINT32           ScriptTotalSize;    /**< Script total size */
    UINT8            ScriptStageNum;     /**< Script total stage number */
} AMP_SCRPT_CONFIG_s;

/**
 * still encode PlayBack Capture control information
 */
typedef struct _AMP_STILLENC_PB_CAPTURE_CTRL_s_ {
    AMP_YUV_BUFFER_s          MainBuf;          /**< yuv main buffer */
    UINT16                    MainWidth;        /**< main width, use for yuv rescale */
    UINT16                    MainHeight;       /**< main height, use for yuv rescale */
    UINT16                    ScrnWidth;        /**< screennail width */
    UINT16                    ScrnHeight;       /**< screennail height */
    UINT16                    ScrnActiveWidth;  /**< screennail active width */
    UINT16                    ScrnActiveHeight; /**< screennail active height */
    UINT16                    ThmWidth;         /**< thumbnail width */
    UINT16                    ThmHeight;        /**< thumbnail height */
    UINT16                    ThmActiveWidth;   /**< thumbnail active width */
    UINT16                    ThmActiveHeight;  /**< thumbnail active height */
    UINT8                     *MainQTAddr;      /**< main quantization matrix, size = 128 */
    UINT8                     *ThmQTAddr;       /**< thumbnail quantization matrix, size = 128 */
    UINT8                     *ScrnQTAddr;      /**< screennail quantization matrix, size = 128 */
} AMP_STILLENC_PB_CAPTURE_CTRL_s;

/**
 * still encode Ituner Raw Capture control information, TBM(this part will only disclose to APP)
 */
typedef struct _AMP_STILLENC_ITUNER_RAWCAPTURE_CTRL_s_ {
    UINT8   RawBufSource;   /**< Raw buffer source, 0 : Allocate by Lib, 1 : Allocate by APP */
    union {
        AMP_ENC_RAW_INFO_s      Raw;            /**< pitch should be 32alignment */
        AMP_STILLENC_POSTP_s    *PostProc;      /**< RawPostProc callback */
    } RawBuff;
    AMBA_SENSOR_ESHUTTER_TYPE_e ShType;
    AMBA_SENSOR_MODE_ID_u       SensorMode;     /**< still capture sensor mode */

    //TBD, YUV/JPEG
} AMP_STILLENC_ITUNER_RAWCAPTURE_CTRL_s;

/*****************************
 *   API prototype           *
 *****************************/
/**
 * Videoenc info type
 */
typedef enum _AMP_STILLENC_INTO_TYPE_e_ {
    AMP_STILLENC_INFO_CFG        /**< current configuration */
} AMP_STILLENC_INFO_TYPE_e;


/**
 * Get still enc module default config for initialization
 *
 * @param [out] defInitCfg Initial default config
 *
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpStillEnc_GetInitDefaultCfg(AMP_STILLENC_INIT_CFG_s *defInitCfg);

/**
 * Initialize still encode module.\n
 * The function should only be invoked once.
 * User MUST invoke this function before using still encode module.
 * The memory pool of the module will provide by user.
 *
 * @param [in] cfg still encode module config
 *
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpStillEnc_Init(AMP_STILLENC_INIT_CFG_s *cfg);

/**
 * Get full default setting of the module
 *
 * @param [out] defCfg default configuration
 *
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpStillEnc_GetDefaultCfg(AMP_STILLENC_HDLR_CFG_s *defCfg);

/**
 * Create an still encoder pipeline and set initial configuration
 *
 * @param [in] pipe pipeline instance
 *
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpStillEnc_Create(AMP_STILLENC_HDLR_CFG_s *cfg, AMP_STLENC_HDLR_s **sencHdlr);

/**
 * Delete an still encoder pipeline
 *
 * @param [in] pipe pipeline instance
 *
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpStillEnc_Delete(AMP_STLENC_HDLR_s *hdlr);

/**
 * Config VIN/MAIN after handler is created
 *
 * @param [in] numVin Number of vin to be configured
 * @param [in] vinCfg Vin configuration array
 * @param [in] mainCfg Main window configuration array
 *
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
int AmpStillEnc_ConfigVinMain(UINT32 numVin, AMP_VIN_RUNTIME_CFG_s *vinCfg, AMP_STILLENC_MAIN_CFG_s *mainCfg);

/**
 * start raw capture, this API should be invoke after LiveView stop and sensor program done
 *
 * @param [in] hdlr encoder instance
 * @param [in] ctrl dsp control information
 *
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
int AmpStillEnc_StartRawCapture(AMP_STLENC_HDLR_s *hdlr, AMP_STILLENC_RAWCAP_DSP_CTRL_s *ctrl);

/**
 * start following raw capture, this API will should be invoke after 1st raw capture is done
 *
 * @param [in] hdlr encoder instance
 * @param [in] ctrl dsp control information
 *
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
int AmpStillEnc_StartFollowingRawCapture(AMP_STLENC_HDLR_s *hdlr, AMP_STILLENC_RAWCAP_DSP_CTRL_s *dspCtrl);

/**
 * Update VIN/MAIN configuration when in IDLE state
 *
 * @param [in] numVin Number of vin to be configured
 * @param [in] vinCfg Vin configuration array
 * @param [in] mainCfg Main window configuration array
 *
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
int AmpStillEnc_UpdateVinMain(UINT32 numVin, AMP_VIN_RUNTIME_CFG_s *vinCfg, AMP_STILLENC_MAIN_CFG_s *mainCfg);

/**
 * Control LiveView capture between LiveView and StillCapture
 *
 * @param [in] Enable Enable or disable liveview capture
 * @param [in] ShowVideoPlane Show videoplane
 *
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
int AmpStillEnc_EnableLiveviewCapture(UINT8 Enable, UINT8 ShowVideoPlane);
#define AMP_STILL_STOP_LIVEVIEW              0
#define AMP_STILL_PREPARE_TO_VIDEO_LIVEVIEW  1
#define AMP_STILL_PREPARE_TO_STILL_CAPTURE   2
#define AMP_STILL_PREPARE_TO_STILL_LIVEVIEW  3
#define AMP_STILL_SWITCH_TO_STILL_LIVEVIEW   4

/**
 * @}
 */

#endif /* _VIDEOENC_H_ */
