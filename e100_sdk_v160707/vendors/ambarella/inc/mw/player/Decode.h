/**
 * @file inc/mw/player/decode.h
 *
 * Amba common decoder header
 *
 * Copyright (C) 2013, Ambarella, Inc.
 *
 * All rights reserved. No Part of this file may be reproduced, stored
 * in a retrieval system, or transmitted, in any form, or by any means,
 * electronic, mechanical, photocopying, recording, or otherwise,
 * without the prior consent of Ambarella, Inc.
 */
#ifndef DECODE_H_
#define DECODE_H_

#include <mw.h>
#include <AmbaPrintk.h>

/**
 * decode callback event
 */
typedef enum _AMP_DEC_EVENT_e_ {
    AMP_DEC_EVENT_ERROR = AMP_DEC_EVENT_START_NUM, ///< event video decode runtime error

    //dec video
    AMP_DEC_EVENT_FIRST_FRAME_DISPLAYED, ///< event first frame displayed to vout done
    AMP_DEC_EVENT_PLAYBACK_EOS, ///< event first frame displayed to vout done
    AMP_DEC_EVENT_STATE_CHANGED, ///< event state changed. codec function may be async function and the event is for user to know the processing is done.
    AMP_DEC_EVENT_DATA_UNDERTHRSHOLDD,   ///< data in raw buffer under threshold.

    //dec still
    AMP_DEC_EVENT_JPEG_DEC_YUV_DISP_REPORT,         ///< YUV displayed on-screen event
    AMP_DEC_EVENT_JPEG_DEC_COMMON_BUFFER_REPORT     ///< LCD Vout buffer information event
} AMP_VIDEODEC_EVENT_e;

/**
 * video playback dirtection
 */
typedef enum _AMP_VIDEO_PLAY_DIRECTION_e_ {
    AMP_VIDEO_PLAY_FW,       ///< play in normal direction
    AMP_VIDEO_PLAY_BW        ///< play rewind
} AMP_AVDEC_PLAY_DIRECTION_e;

/**
 * trickplay status
 */
typedef struct _AMP_AVDEC_TRICKPLAY_s_ {
    UINT32 TimeOffsetOfFirstFrame;  ///< the first frame to push to stream in ms
    AMP_AVDEC_PLAY_DIRECTION_e Direction;      ///< playback direction
    UINT32 Speed;                              ///< playback speed
} AMP_AVDEC_TRICKPLAY_s;

typedef struct _AMP_VIDEODEC_DISPLAY_s_ {
    UINT16 SrcWidth;     ///< video width
    UINT16 SrcHeight;   ///< video height
    AMP_AREA_s AOI;     ///< area to display
} AMP_VIDEODEC_DISPLAY_s;

/**
 *
 */
typedef enum _AMP_DEC_CODEC_CONFIG_e_ {
    AMP_DEC_CODEC_CONFIG_PIPE,     ///< config the pipe the codec run on
    AMP_DEC_CODEC_CONFIG_SUS_DSP_ON_STOP  ///< suspend dsp after codec stop
} AMP_DEC_CODEC_CONFIG_e;

typedef struct _AMP_DEC_CODEC_CONFIG_SUS_DSP_ON_STOP_s_ {
    UINT32 Idx;             ///< AMP_DEC_CODEC_CONFIG_e
    UINT8 SusDspOnStop;     ///< if suspend dsp on stop
} AMP_DEC_CODEC_CONFIG_SUS_DSP_ON_STOP_s;

typedef struct _AMP_VIDDEC_CODEC_s_ {
    int (*GetNumDesc)(UINT32 *numDesc); /**< get how many descriptor the codec have */
    int (*Open)(void* hdlr); /**< open invoked while switch from idle state to running states */
    int (*Close)(void* hdlr); /**< close invoked while switch from running state to idle states */
    int (*PreStart)(void* hdlr,
                    AMP_AVDEC_TRICKPLAY_s *trickplay, /**< trickplay status */
                    AMP_VIDEODEC_DISPLAY_s *display); /**< start invoked while system start to prepare decode result */
    int (*Start)(void* hdlr,
                 AMP_VIDEODEC_DISPLAY_s *display); /**< start invoked while system start to push decode result */
    int (*Pause)(void* hdlr); /**< pause invoked while system pause */
    int (*Resume)(void* hdlr); /**< resume invoked while system restart to push decode result */
    int (*Step)(void* hdlr); /**< step invoked while system would like to step one frame */
    int (*StopWithLastFrm)(void* hdlr); /**< StopWithLastFrm invoked while system stop and want to keep last frame on screen*/
    int (*Stop)(void* hdlr); /**< stop invoked while system stop */
    int (*Config)(void* hdlr,
                  void* info); /**< config function that only use in mgr, should not invoked by app */
} AMP_AVDEC_CODEC_s;

/**
 * common handler structure for video decode
 */
typedef struct _AMP_VIDDEC_HDLR_s_ {
    void *Ctx;                         ///< pointer to codec context
    AMP_AVDEC_CODEC_s *Function;  ///< pointer to codec operation functions
} AMP_AVDEC_HDLR_s;

typedef struct _AMP_DECSTL_CODEC_s_ {
    int (*GetNumDesc)(UINT32 *numDesc); /**< get how many descriptor the codec have */
    int (*Open)(void* hdlr); /**< open invoked while switch from idle state to running states */
    int (*Close)(void* hdlr); /**< close invoked while switch from running state to idle states */
    int (*RunCmd)(void* hdlr,
                  void* Cmd); /**< run command */
    int (*Config)(void* hdlr,
                  void* info); /**< config function that only use in mgr, should not invoked by app */
    int (*Stop)(void* hdlr); /**< stop invoked while system stop */
    int (*Start)(void* hdlr); /**< start invoked while system start */
} AMP_STLDEC_CODEC_s;

/**
 * common handler structure for still decode
 */
typedef struct _AMP_STLDEC_HDLR_s_ {
    void *Ctx;                         ///< pointer to codec context
    AMP_STLDEC_CODEC_s *Function;  ///< pointer to codec operation functions
} AMP_STLDEC_HDLR_s;

// not support currently
#if 0
/**
 * AMP_DEC_SOURCE_DSEC
 */

typedef struct _AMP_DEC_SOURCE_s_ {
    void* Ctx;
}AMP_DEC_SOURCE_s;

/**
 * AMP_DEC_CONTENT_s    need dis
 */
typedef struct _AMP_DEC_CONTENT_s_ {
    UINT32 Checker; ///< a UINT32 which assign with a magic number when given default to check if customer get default first
    AMP_DEC_SOURCE_s Source;
    UINT8 CropFromSource;
    AMP_AREA_s CropArea;
    AMP_ROTATION_e Rotation;
    AMP_AREA_s TargetAreaOnPlane;
    UINT32 Layer;
}AMP_DEC_CONTENT_s;

/**
 * AMP_DEC_LAYOUT_DESC
 */
typedef struct _AMP_DEC_LAYOUT_s_ {
    UINT32 Checker; ///< a UINT32 which assign with a magic number when given default to check if customer get default first
    UINT32 OutputWidth;///< output size should follow the setting for display
    UINT32 OutputHeight;///< output size
#define MAX_DEC_LAYOUT_CONTENT  (2)
    UINT32 NumContent; ///< number of the content
    AMP_DEC_CONTENT_s Content[MAX_DEC_LAYOUT_CONTENT];///< content array
}AMP_DEC_LAYOUT_s;
#endif

/**
 * @defgroup DecodeMgr
 * @brief Decoder pipeline
 *
 * Decode pipe is designed for codecs that need to be operated with time sync.\n
 * In most case, it's used for video decoding that sync audio and video playback.
 *
 */

/**
 * @addtogroup DecodeMgr
 * @ingroup FlowPipeline
 * @{
 */

/**
 *
 */
typedef struct _AMP_DEC_PIPE_HDLR_s_ {
    void *Ctx; /**< the context of the pipe */
} AMP_DEC_PIPE_HDLR_s;

typedef enum _AMP_DEC_PIPE_TYPE_e_ {
    AMP_DEC_VID_PIPE,   ///< video dec pipe
    AMP_DEC_STL_PIPE    ///< still dec pipe
} AMP_DEC_PIPE_TYPE_e;

/**
 * AMP_DEC_PIPE_STATE_e
 */
typedef enum _AMP_DEC_PIPE_STATE_e_ {
    AMP_DEC_PIPE_IDLE,
    AMP_DEC_PIPE_PLAY,
    AMP_DEC_PIPE_PAUSE,
    AMP_DEC_PIPE_PLAY_IDLE
} AMP_DEC_PIPE_STATE_e;

/**
 * AMP_DEC_PIPE_INFO_s
 */
typedef struct _AMP_DEC_PIPE_INFO_s_ {
    AMP_DEC_PIPE_STATE_e State;
} AMP_DEC_PIPE_INFO_s;

typedef struct _AMP_DECVID_PIPE_CFG_s_ {
    AMP_DEC_PIPE_TYPE_e Type;
    UINT32 NumDecoder;             ///< number of handler(s) in the pipe
#define MAX_DEC_PIPE_CODEC  (4)
    void *Decoder[MAX_DEC_PIPE_CODEC];    ///< pointer to handler(s) in the pipe
    AMP_CALLBACK_f CbEvent;         ///< video decode pipe call back function
} AMP_DEC_PIPE_CFG_s;

/**
 * To get default config of decode mgr
 *
 * @param [out] cfg - config of decode mgr
 *
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpDec_GetDefaultCfg(AMP_DEC_PIPE_CFG_s *cfg);

/**
 * To create decode mgr
 *
 * @param [in] cfg - config of decode mgr
 * @param [out] pipe - output pipe handler
 *
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpDec_Create(AMP_DEC_PIPE_CFG_s *cfg,
                         AMP_DEC_PIPE_HDLR_s **pipe);

/**
 * To delete a pipe hdlr
 *
 * @param [in] pipe - the pipe to delete
 *
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpDec_Delete(AMP_DEC_PIPE_HDLR_s *pipe);

/**
 * To add pipe to decode mgr
 *
 * @param [in] pipe - the pipe to add
 *
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpDec_Add(AMP_DEC_PIPE_HDLR_s *pipe);

/**
 * To remove pipe from decode mgr
 *
 * @param [in] pipe - the pipe to remove
 *
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpDec_Remove(AMP_DEC_PIPE_HDLR_s *pipe);

/**
 * To start sync pipe
 *
 * @param [in] pipe - the working pipe
 *
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpDec_Start(AMP_DEC_PIPE_HDLR_s *pipe,
                        AMP_AVDEC_TRICKPLAY_s *trick,
                        AMP_VIDEODEC_DISPLAY_s *display);

/**
 * To pause sync pipe
 *
 * @param [in] pipe - the working pipe
 *
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpDec_Pause(AMP_DEC_PIPE_HDLR_s *pipe);

/**
 * To resume the sync pipe
 *
 * @param [in] pipe - the working pipe
 *
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpDec_Resume(AMP_DEC_PIPE_HDLR_s *pipe);

/**
 * To stop the pipe and hold the last frame on-screen
 *
 * @param [in] pipe - the working pipe
 *
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpDec_StopWithLastFrm(AMP_DEC_PIPE_HDLR_s *pipe);

/**
 * To stop the pipe
 *
 * @param [in] pipe - the working pipe
 *
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpDec_Stop(AMP_DEC_PIPE_HDLR_s *pipe);

/**
 * One step forward on the pipe
 *
 * @param [in] pipe - the working pipe
 *
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpDec_Step(AMP_DEC_PIPE_HDLR_s *pipe);

/**
 * To config pipe behavior
 *
 * @param [in] pipe - the pipe to control
 * @param [in] cfg - pointer to config structure
 *
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpDec_Config(AMP_DEC_PIPE_HDLR_s *pipe, void* cfg);

/**
 * To get decode mgr info
 *
 * @param [in] pipe
 *
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpDec_GetInfo(AMP_DEC_PIPE_HDLR_s *pipe,
                          AMP_DEC_PIPE_INFO_s *info);

/**
 * @}
 */

#endif /* DECODE_H_ */
