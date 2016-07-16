 /**
  * @file inc/mw/recorder/encode.h
  *
  * Encoder common header
  *
  * Copyright (C) 2013, Ambarella, Inc.
  *
  * All rights reserved. No Part of this file may be reproduced, stored
  * in a retrieval system, or transmitted, in any form, or by any means,
  * electronic, mechanical, photocopying, recording, or otherwise,
  * without the prior consent of Ambarella, Inc.
  */

#ifndef ENCODE_H_
#define ENCODE_H_

#include <mw.h>
#include <AmbaPrintk.h>
#include "AmbaSensor.h"

#define AMP_ENC_MAX_MULTICHAN_NUM    10    // B5 only support up to 10 channels

/**
 *  AMP_ENC_STOP_SCRIPT_NONE:        MW will stop running script at last stage
 *  AMP_ENC_STOP_SCRIPT_RIGHT_AWAY:  MW will stop running script right away,\n
 *                                       all remain raw/yuv/jpeg will be distcard
 */
#define AMP_ENC_STOP_SCRIPT_NONE  (0x1)
#define AMP_ENC_STOP_SCRIPT_RIGHT_AWAY (AMP_ENC_STOP_SCRIPT_NONE<<1)

/**
 * @defgroup EncodeMgr
 * @brief Encode pipeline manager implementation
 *
 * Implementation the Encode Pipeline module, include below function implementation
 *  1. Create Encode Pipeline module function.
 *  2. Delete Encode Pipeline module function.
 *  3. Add Encode Pipeline module function.
 *  4. Remove Encode Pipeline module function.
 *  5. Start/Stop Liveview
 *  6. Start/Stop/Pause/Resume encode
 *  7. Run/Stop still capture script
 *  8. Get Encode Pipeline module information
 */

/**
 * @addtogroup EncodeMgr
 * @ingroup FlowPipeline
 * @{
 */


/**
 * state function (liveview, record...etc) flags MSB for pipe, LSB for codec internal use.
 */
#define AMP_ENC_FUNC_FLAG_NONE                  0x00000000
#define AMP_ENC_FUNC_FLAG_WAIT                  0x80000000
#define AMP_ENC_FUNC_FLAG_NO_SHOW_VIDEO_PLANE   0x40000000
#define AMP_ENC_FUNC_FLAG_SUS_DSP_ON_STOP       0x20000000
#define AMP_ENC_FUNC_FLAG_IDLE_DSP_ON_STOP      0x10000000  /* use for Wireless transfer mode */
#define ENC_FLAG_CHECK(a, flag) ((a&flag) == flag)

/**
 * encode callback event
 */
typedef enum _AMP_ENC_EVENT_e_ {
    // Codec event
    AMP_ENC_EVENT_DATA_OVER_RUNOUT_THRESHOLD = AMP_ENC_EVENT_START_NUM, /**< buffer data over runout threshold */
    AMP_ENC_EVENT_DATA_OVERRUN,             /**< buffer data overrun */
    AMP_ENC_EVENT_DESC_OVERRUN,             /**< buffer data overrun */
    AMP_ENC_EVENT_LIVEVIEW_RAW_READY,       /**< liveview raw data ready, app could read out the data from ds image */
    AMP_ENC_EVENT_LIVEVIEW_DCHAN_YUV_READY, /**< liveview DCHAN yuv data ready, app could read out the data from ds image */
    AMP_ENC_EVENT_LIVEVIEW_FCHAN_YUV_READY, /**< liveview FCHAN yuv data ready, app could read out the data from ds image */
    AMP_ENC_EVENT_QUICKVIEW_DCHAN_YUV_READY,/**< quickview DCHAN yuv data ready, app could read out the data from ds image */
    AMP_ENC_EVENT_QUICKVIEW_FCHAN_YUV_READY,/**< quickview FCHAN yuv data ready, app could read out the data from ds image */
    AMP_ENC_EVENT_VCAP_YUV_READY,           /**< still capture yuv data ready, app could read out the data from ds image */
    AMP_ENC_EVENT_VCAP_2ND_YUV_READY,       /**< encode 2nd yuv data ready, app could read out the data from ds image */
    AMP_ENC_EVENT_VCAP_ME1_Y_READY,         /**< encode ME1 Y data ready, app could read out the data from ds image */
    AMP_ENC_EVENT_HYBRID_YUV_READY,         /**< encode Yuv data ready in HybridMode, app could read out the data from ds image */

    /* Stillenc event */
    AMP_ENC_EVENT_RAW_CAPTURE_START,        /**< inform APP that dsp start raw capture and provide its info */
    AMP_ENC_EVENT_RAW_CAPTURE_DONE,         /**< inform APP that codec already capture desired raw data */
    AMP_ENC_EVENT_BACK_TO_LIVEVIEW,         /**< inform APP that codec already back to liveview, only when script.b2lv>0 */
    AMP_ENC_EVENT_BG_PROCESS_DONE,          /**< inform APP that codec already process whole script */

    // Pipe event
    AMP_ENC_EVENT_STATE_CHANGED,             /**< event state changed. codec function may be async function and the event is for user to know the processing is done. */
    // DSP detect error event
    AMP_ENC_EVENT_ILLEGAL_SIGNAL,           /**< DSP detect Vin become illegal signal */
    AMP_ENC_EVENT_VDSP_ASSERT,               /**< Vdsp assertion */
    AMP_ENC_EVENT_DATA_FULLNESS_NOTIFY      /**<  buffer data fullness notify*/
} AMP_ENC_EVENT_e;


/**
 * Encode pipeline type
 */
typedef enum _AMP_ENC_PIPE_TYPE_e_ {
    AMP_ENC_AV_PIPE,        /*< audio or video pipe */
    AMP_ENC_STILL_PIPE      /*< still pipe */
} AMP_ENC_PIPE_TYPE_e;

/**
 * Encode pipeline state
 */
typedef enum _AMP_ENC_PIPE_STATE_e_ {
    AMP_ENC_PIPE_STATE_IDLE,                /**< idle state */
    AMP_ENC_PIPE_STATE_LIVEVIEW,            /**< liveview state */
    AMP_ENC_PIPE_STATE_RECORDING,           /**< A/V recording state */
    AMP_ENC_PIPE_STATE_RECORDING_PAUSED,    /**< A/V record-pause state */
    AMP_ENC_PIPE_STATE_SCRIPT_PROCESSING,   /**< Still encode processing state */
    AMP_ENC_PIPE_STATE_UNKNOWN = 0xFF
} AMP_ENC_PIPE_STATE_e;


/**
 * AMP_ENC_EVENT_STATE_CHANGED event data
 */
typedef struct AMP_ENC_STATE_CHANGED_INFO_s_ {
    AMP_ENC_PIPE_STATE_e newState;      /**< new state event */
} AMP_ENC_STATE_CHANGED_INFO_s;


/**
 * bitstream buffer config
 */
#define AMP_ENC_ALIGN_BITSBUF(x)    (((x) + 16383) & 0xFFFFC000)
typedef struct AMP_ENC_BITSBUFFER_CFG_s_ {
    UINT8  *BitsBufAddr;            /**< Bitstream buffer address */
    UINT32  BitsBufSize;            /**< Bitstream buffer size. Must be multiple of 16384 */
    UINT8  *DescBufAddr;            /**< Descriptor buffer address */
    UINT32  DescBufSize;            /**< Descriptor buffer size */
    UINT32  BitsRunoutThreshold;    /**< indicate Bitstream's capacity is alomost full, Byte Unit */
} AMP_ENC_BITSBUFFER_CFG_s;

/**
 *  Encode source type
 */
typedef enum _AMP_VIDEOENC_SOURCE_TYPE_e_ {
    AMP_ENC_SOURCE_VIN = 0, /**< From VIN */
    AMP_ENC_SOURCE_MEMORY,  /**< From Dram memory buffer */
    AMP_ENC_SOURCE_NUM
} AMP_ENC_SOURCE_TYPE_e;

/**
 * Jpeg bitstream specific config
 */
typedef struct AMP_ENC_JPEG_CFG_s_ {
    UINT8 procMode;                     /**< ISO mode */
    UINT32 *procModeCfg;                /**< ISO config */
    UINT8 *mainQuantMatrixAddr;         /**< Default main quantization matrix, size = 128 */
    UINT8 *thumbnailQuantMatrixAddr;    /**< Default thumbnail quantization matrix, size = 128 */
    UINT8 *screennailQuantMatrixAddr;   /**< Default screennail quantization matrix, size = 128 */
    UINT8 qualityFactor;                /**< Quality factor to adjust QuantMatrix, 1~100. newQ[i] = (DefQ[i]*qF+50)/100 */
    UINT16 thumbnailWidth;              /**< Jpeg thumbnail (effect + black bar)width */
    UINT16 thumbnailHeight;             /**< Jpeg thumbnail (effect + black bar)height */
    UINT16 thumbnailActiveWidth;        /**< Jpeg thumbnail active(effect) width */
    UINT16 thumbnailActiveHeight;       /**< Jpeg thumbnail active(effect) height */
    UINT16 screennailWidth;             /**< Jpeg screennail (effect + black bar)width */
    UINT16 screennailHeight;            /**< Jpeg screennail (effect + black bar)height */
    UINT16 screennailActiveWidth;       /**< Jpeg screennail active(effect) width */
    UINT16 screennailActiveHeight;      /**< Jpeg screennail active(effect) height */
} AMP_ENC_JPEG_CFG_s;

/**
 * MultiChan Main Window config
 */
typedef struct _AMP_MULTI_CHAN_MAIN_WINDOW_CFG_s_ {
    UINT8                    Enable:1;          /**< Enable this input */
    UINT8                    ChannelNumber:7;   /**< sensor number in this view */
    AMBA_DSP_CHANNEL_ID_u    *ChannelID;        /**< Source/Sensor ID(Option) */
    AMP_AREA_s               *SensorCapWin;     /**< Capture window of each channel */
    UINT8                    MainViewID;        /* Main window view ID */
    AMP_AREA_s               MainCapWin;        /**< Main capture window of each view */
    UINT16                   MainWidth;         /**< Main window width */
    UINT16                   MainHeight;        /**< Main window height */
} AMP_MULTI_CHAN_MAIN_WINDOW_CFG_s;

/**
 * MultiChan Vout Window config
 */
typedef struct _AMP_MULTI_CHAN_VOUT_WINDOW_CFG_s_ {
    UINT8                   Enable:1;           /**< Enable this display channel */
    UINT8                   MainWindowView:7;   /**< Main window view ID */
    UINT8                   LayerID;            /**< Process ID, 0 means at the bottom */
    AMP_AREA_s              DisplayWin;         /**< dsiplay window */
    AMP_ROTATION_e          Rotate;             /**< Rotate */
    AMP_2D_BUFFER_s         BlendTable;         /**< Blending Table */
} AMP_MULTI_CHAN_VOUT_WINDOW_CFG_s;

/**
 * MultiChan Encode Window config
 */
typedef struct _AMP_MULTI_CHAN_ENCODE_WINDOW_CFG_s_ {
    UINT8                   Enable:1;           /**< Enable this encode channel */
    UINT8                   MainWindowView:7;   /**< Main window view ID */
    UINT8                   LayerID;            /**< Process ID, 0 means at the bottom */
    AMP_AREA_s              SrcWin;             /**< source window from main window */
    AMP_AREA_s              DestWin;            /**< destination window of encode window */
    AMP_ROTATION_e          Rotate;             /**< Rotate */
    AMP_2D_BUFFER_s         BlendTable;         /**< Blending Table */
} AMP_MULTI_CHAN_ENCODE_WINDOW_CFG_s;

/**
 * AV codec functions
 */
typedef struct _AMP_AVENC_CODEC_s_ {
    int (*open)(void* hdlr, UINT32 pipeId);          /**< open invoked while switch from idle state to running states */
    int (*close)(void* hdlr);                        /**< close invoked while switch from running state to idle states */
    int (*liveviewStart)(void* hdlr, UINT32 flag);   /**< liveviewStart invoked while live view start */
    int (*liveviewStop)(void* hdlr, UINT32 flag);    /**< liveviewStart invoked while live view stop, flag: 0 - show bgcolor, 1 - show last frame */
    int (*start)(void* hdlr, UINT32 flag);           /**< start invoked while system start to encode */
    int (*pause)(void* hdlr, UINT32 flag);           /**< pause invoked while system pause */
    int (*resume)(void* hdlr, UINT32 flag);          /**< resume invoked while system restart to encode */
    int (*stop)(void* hdlr, UINT32 flag);            /**< stop invoked while system stop */
} AMP_AVENC_CODEC_s;

/**
 * AMP_ENC_EVENT_LIVEVIEW_DCHAN_YUV_READY/AMP_ENC_EVENT_LIVEVIEW_FCHAN_YUV_READY event data
 */
typedef struct _AMP_ENC_YUV_INFO_s_ {
    UINT32 ChannelID;
    AMP_COLOR_FORMAT_e colorFmt;    /**< Yuv color format */
    UINT8  *yAddr;                  /**< Yuv buffer address */
    UINT8  *uvAddr;                  /**< Yuv buffer address */
    UINT32  ySize;                  /**< Yuv buffer size, ie. Pitch*Height */
    UINT16  pitch;                  /**< Yuv buffer pitch */
    UINT16  width;                  /**< Yuv buffer width */
    UINT16  height;                 /**< Yuv buffer height */
} AMP_ENC_YUV_INFO_s;

/**
 * RAW buffer information
 */
typedef struct _AMP_ENC_RAW_INFO_s_ {
    UINT8 compressed;   /**< CFA Raw buffer format */
    UINT8 *RawAddr;     /**< CFA Raw buffer address */
    UINT16 RawPitch;    /**< CFA Raw buffer pitch */
    UINT16 RawWidth;    /**< CFA Raw buffer width */
    UINT16 RawHeight;   /**< CFA Raw buffer height */
    UINT16 OBWidth;     /**< CFA Raw buffer OB region width */
    UINT16 OBHeight;    /**< CFA Raw buffer OB region height */
    UINT16 OBOffsetX;   /**< CFA Raw buffer OB region offset in x-axis */
    UINT16 OBOffsetY;   /**< CFA Raw buffer OB region offset in y-axis */
} AMP_ENC_RAW_INFO_s;

/**
 * common handler structure for video encode
 */
typedef struct _AMP_AVENC_HDLR_s_ {
    void *ctx;                      /**< pointer to codec context */
    AMP_AVENC_CODEC_s *function;    /**< pointer to codec operation functions */
} AMP_AVENC_HDLR_s;


/**
 * Still codec functions
 */
typedef struct _AMP_STLENC_CODEC_s_ {
    int (*getNumDesc)(UINT32 *numDesc);            /**< get how many descriptor the codec have */
    int (*open)(void* hdlr, UINT32 pipeId);        /**< open invoked while switch from idle state to running states */
    int (*close)(void* hdlr);                      /**< close invoked while switch from running state to idle states */
    int (*liveviewStart)(void* hdlr, UINT32 flag); /**< liveviewStart invoked while live view start */
    int (*liveviewStop)(void* hdlr, UINT32 flag);  /**< liveviewStart invoked while live view stop */
    int (*runScrpt)(void* hdlr, void* Scrpt, UINT32 flag);      /**< start to run script */
    int (*stopRunScrpt)(void* hdlr, UINT32 rule, UINT32 scrptID);  /**< stop to run script */
    int (*config)(UINT32 cfgIdx, void* info);      /**< config function that only use in mgr, should not invoked by app */
} AMP_STLENC_CODEC_s;

/**
 * common handler structure for still encode
 */
typedef struct _AMP_STLENC_HDLR_s_ {
    void *ctx;                      /**< pointer to codec context */
    AMP_STLENC_CODEC_s *function;   /**< pointer to codec operation functions */
} AMP_STLENC_HDLR_s;

/**
 * AMP_ENC_PIPE_INFO_s
 */
typedef struct _AMP_ENC_PIPE_INFO_s_ {
    AMP_ENC_PIPE_STATE_e state; /**< pipeline state */
    AMP_ENC_PIPE_TYPE_e type;   /**< pipeline type */
} AMP_ENC_PIPE_INFO_s;

/**
 * pipeline handler
 */
typedef struct _AMP_ENC_PIPE_HDLR_s_ {
    void *ctx; /**< the context of the pipe */
} AMP_ENC_PIPE_HDLR_s;

/**
 * pipeline configuration
 */
typedef struct _AMP_ENC_PIPE_CFG_s_ {
    AMP_ENC_PIPE_TYPE_e type;           /**< pipe type */
    UINT32 numEncoder;                  /**< number of handler(s) in the pipe */
#define MAX_ENC_PIPE_CODEC  (16)
    void *encoder[MAX_ENC_PIPE_CODEC];  /**< pointer to handler(s) in the pipe */
    AMP_CALLBACK_f cbEvent;             /**< callback function */
} AMP_ENC_PIPE_CFG_s;

/**
 * Get default pipeline configuration
 *
 * @param [out] cfg pipeline config
 *
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpEnc_GetDefaultCfg(AMP_ENC_PIPE_CFG_s *cfg);

/**
 * Create an encoder pipeline and set initial configuration
 *
 * @param [in] cfg pipeline config
 * @param [out] pipe pipeline instance
 *
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpEnc_Create(AMP_ENC_PIPE_CFG_s *cfg, AMP_ENC_PIPE_HDLR_s **pipe);

/**
 * Delete an encoder pipeline
 *
 * @param [in] pipe pipeline instance
 *
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpEnc_Delete(AMP_ENC_PIPE_HDLR_s *pipe);

/**
 * Add a pipeline to encode manager
 *
 * @param [in] pipe pipeline instance
 *
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpEnc_Add(AMP_ENC_PIPE_HDLR_s *pipe);

/**
 * Remove a pipeline from encode manager
 *
 * @param [in] pipe pipeline instance
 *
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpEnc_Remove(AMP_ENC_PIPE_HDLR_s *pipe);

/**
 * Ask a pipeline to start liveview
 *
 * @param [in] pipe pipeline instance
 * @param [in] flag flag of liveview operation
 *
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpEnc_StartLiveview(AMP_ENC_PIPE_HDLR_s *pipe, UINT32 flag);

/**
 *
 * Ask a pipeline to stop liveview
 *
 * @param [in] pipe pipeline instance
 * @param [in] flag flag of liveview operation
 *
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpEnc_StopLiveview(AMP_ENC_PIPE_HDLR_s *pipe, UINT32 flag);

/**
 *
 * Ask a pipeline to run script (still encode operation)
 *
 * @param [in] pipe pipeline instance
 * @param [in] script script
 *
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpEnc_RunScript(AMP_ENC_PIPE_HDLR_s *pipe, void *script, UINT32 flag);

/**
 *
 * Ask a pipeline to stop script (still encode operation)
 *
 * @param [in] pipe pipeline instance
 * @param [in] flag for stop script
 *
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpEnc_StopRunScript(AMP_ENC_PIPE_HDLR_s *pipe, UINT32 rule, UINT32 scrptID);

/**
 *
 * Ask a pipeline to start record
 *
 * @param [in] pipe pipeline instance
 * @param [in] flag flag for start record
 *
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpEnc_StartRecord(AMP_ENC_PIPE_HDLR_s *pipe, UINT32 flag);

/**
 *
 * Ask a pipeline to pause record
 *
 * @param [in] pipe pipeline instance
 * @param [in] flag flag for pause record
 *
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpEnc_PauseRecord(AMP_ENC_PIPE_HDLR_s *pipe, UINT32 flag);

/**
 *
 * Ask a pipeline to resume record
 *
 * @param [in] pipe pipeline instance
 * @param [in] flag flag for resume record
 *
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpEnc_ResumeRecord(AMP_ENC_PIPE_HDLR_s *pipe, UINT32 flag);

/**
 *
 * Ask a pipeline to stop record
 *
 * @param [in] pipe pipeline instance
 * @param [in] flag flag for stop record
 *
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpEnc_StopRecord(AMP_ENC_PIPE_HDLR_s *pipe, UINT32 flag);

/**
 * get encode mgr info
 *
 * @param [in] pipe pipeline instance
 * @param [out] info information
 *
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpEnc_GetInfo(AMP_ENC_PIPE_HDLR_s *pipe, AMP_ENC_PIPE_INFO_s *info);

/**
 * @}
 */

#endif /* ENCODE_H_ */
