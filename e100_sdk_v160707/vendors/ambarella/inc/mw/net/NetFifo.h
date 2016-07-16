/**
 * @file NetFifo.h
 *
 * Copyright (C) 2015, Ambarella, Inc.
 *
 * All rights reserved. No Part of this file may be reproduced, stored
 * in a retrieval system, or transmitted, in any form, or by any means,
 * electronic, mechanical, photocopying, recording, or otherwise,
 * without the prior consent of Ambarella, Inc.
 */

#ifndef AMP_NETFIFO_H_
#define AMP_NETFIFO_H_

#include <mw.h>
#include <fifo/Fifo.h>

/**
 * @defgroup NETFIFO
 * @brief RTOS MW FIFO relay module
 *
 * NetFifo is used to relay RTOS MW FIFO for Linux user space programs.\n
 * With NetFifo, Linux program could retrieve/write media descriptor/frame from/to RTOS.\n
 *
 */

/**
 * @addtogroup NETFIO
 * @{
 */

/**
 * Media track type (in UINT8)
 */
typedef enum {
    AMP_NETFIFO_MEDIA_TRACK_TYPE_VIDEO = 0x01,  /**< The track's type is Video */
    AMP_NETFIFO_MEDIA_TRACK_TYPE_AUDIO = 0x02,  /**< The track's type is Audio */
    AMP_NETFIFO_MEDIA_TRACK_TYPE_TEXT = 0x03,   /**< The track's type is Text */
    AMP_NETFIFO_MEDIA_TRACK_TYPE_MAX = 0x04     /**< Max value, for check use */
} AMP_NETFIFO_MEDIA_TRACK_TYPE_e;

/**
 * video track configure
 */
typedef struct {
    UINT32 nGOPSize;            /**< The number of the picture between IDR pictures. */
    UINT32 nTrickRecDen;        /**< For AmpFormat_ConvertPTS(), the Denominator of the trick record. If is high frame rate, the denominator and numerator are specify the factor than default 30fps, Ex: 120fps -> Den = 1, Num = 4 */
    UINT32 nTrickRecNum;        /**< For AmpFormat_ConvertPTS(), the Numerator of the trick record. If is high frame rate, the denominator and numerator are specify the factor than default 30fps, Ex: 120fps -> Den = 1, Num = 4 */
    UINT32 nCodecTimeScale;     /**< TODO: It needs get from H264 bitstream, it is not configurable. */
    UINT16 nWidth;              /**< Picture width */
    UINT16 nHeight;             /**< Picture height */
    UINT16 nM;                  /**< The number of the picture between reference pictures(IDR, I, P) */
    UINT16 nN;                  /**< The number of the picture between I pictures */
    BOOL8 bDefault;             /**< The flag defines the track as default video track, if the media hasn't one video track. */
    UINT8 nMode;                /**< The value defines the picture mode of the video. It has progressive and interlaced mode. Interlaced mode has Field Per Sample and Frame Per Sample, See AMP_VIDEO_MODE_s */
    BOOL8 bClosedGOP;           /**< The structure of the Close GOP is I P B B P B B. The structure of the Open GOP is I B B P B B, If resume or auto split, the value always is Open GOP. */
} AMP_NETFIFO_VIDEO_TRACK_CFG_s;

/**
 * audio track configure
 */
typedef struct {
    UINT32 nSampleRate;         /**< The sample rate(Hz) of the audio track. */
    BOOL8 bDefault;             /**< The flag defines the track as default audio track, if the media hasn't one audio track. */
    UINT8 nChannels;            /**< The number of audio channel. */
    UINT8 nBitsPerSample;       /**< The per sample size of the audio track. Ex: 8 bits, 16 bits, ....n bits */
} AMP_NETFIFO_AUDIO_TRACK_CFG_s;

/**
 * text track configure
 */
typedef struct {
    BOOL8 bDefault;             /**< The flag defines the track as default text track, if the media hasn't one text track. */
} AMP_NETFIFO_TEXT_TRACK_CFG_s;

/**
 * media track configure
 */
typedef struct {
    UINT32 nMediaId;            /**< The media type of the track. The id is media id, See AMP_FORMAT_MID_e */
    UINT32 nTimeScale;          /**< Time scale, the same as the LCM of those of all tracks */
    UINT32 nTimePerFrame;       /**< The time of the frame that the unit of it is Time scale. */
    UINT32 nInitDelay;          /**< Initial delay time(ms) of the track. */
    void *hCodec;               /**< the codec which this track is working on. */
    UINT8 *pBufferBase;         /**< The start address of the FIFO of the track. User pushs data to the FIFO, the FIFO will write data to the address of the buffer. */
    UINT8 *pBufferLimit;        /**< The end address of the FIFO of the track, The data can't write overlap the address, FIFO size = FIFO buffer limit - FIFO buffer base. */
    union {
        AMP_NETFIFO_VIDEO_TRACK_CFG_s Video;   /**< See AMP_NETFIFO_VIDEO_TRACK_CFG_s, the information of the video track. */
        AMP_NETFIFO_AUDIO_TRACK_CFG_s Audio;   /**< See AMP_NETFIFO_AUDIO_TRACK_CFG_s, the information of the audio track. */
        AMP_NETFIFO_TEXT_TRACK_CFG_s Text;     /**< See AMP_NETFIFO_TEXT_TRACK_CFG_s, the information of the text track. */
    } Info;
    UINT8 nTrackType;           /**< Track type, See AMP_MEDIA_TRACK_TYPE_e */
} AMP_NETFIFO_MEDIA_TRACK_CFG_s;

#define AMP_NETFIFO_MAX_TRACK_PER_MEDIA  4     /**< The max track number of a media. */

/**
 * movie information configure
 */
typedef struct {
    AMP_NETFIFO_MEDIA_TRACK_CFG_s Track[AMP_NETFIFO_MAX_TRACK_PER_MEDIA]; /**< See AMP_NETFIFO_MEDIA_TRACK_CFG_s, the tracks in the movie. */
    UINT8 nTrack;               /**< The number of Tracks in the movie. */
} AMP_NETFIFO_MOVIE_INFO_CFG_s;

/**
 * NetFifo default init config
 */
typedef struct _AMP_NETFIFO_INIT_CFG_s_ {
    AMP_CALLBACK_f cbEvent;     /**< the callback function for NetFifo status change */
    AMP_CALLBACK_f cbMediaInfo; /**< the callback function to retrieve Media configurtion */
    AMP_CALLBACK_f cbPlayback;  /**< the callback function to specify playback command */
    AMP_TASK_INFO_s RpcSvcTaskInfo; /**< The information of a Netfifo SVC task */
} AMP_NETFIFO_INIT_CFG_s;


#define AMP_NETFIFO_MEDIA_CMD_GET_STREAM_LIST  (0x0001)
#define AMP_NETFIFO_MEDIA_CMD_GET_INFO         (0x0002)

#define AMP_NETFIFO_GET_MEDIA_CMD(X) (X>>16)
#define AMP_NETFIFO_GET_MEDIA_PARAM(X) (X&0x0000ffff)

/**
 * Netfifo Media stream ID List.
 */
typedef struct _AMP_NETFIFO_MEDIA_STREAMID_LIST_s_ {
    int Amount;                 /**< Amount of valid list entry. */
    int StreamID_List[16];      /**< StreamID. */
} AMP_NETFIFO_MEDIA_STREAMID_LIST_s;

/**
 * Netfifo Playback OP info type
 */
typedef struct _AMP_NETFIFO_PLAYBACK_OP_INFO_s_ {
    UINT8  inParam[128];     /**< The input param of playback OP */
    UINT8* resParam;    /**< The response param for playback OP */
} AMP_NETFIFO_PLAYBACK_OP_INFO_s;

/**
 * Netfifo Notify type
 */
typedef enum _AMP_NETFIFO_NOTIFY_TYPE_e_ {
    AMP_NETFIFO_NOTIFY_STARTENC = 1,        /**< Start encode. May from STOP_ENC or SWITCHENCSESSION */
    AMP_NETFIFO_NOTIFY_STOPENC,             /**< Stop encode then stay in idle. Such as menu operation or switch to thumbnail mode */
    AMP_NETFIFO_NOTIFY_SWITCHENCSESSION,    /**< Stop encode then start another encode session (VF <-> REC) */
    AMP_NETFIFO_NOTIFY_STARTNETPLAY,        /**< Start playback for streaming. set param1 as stream_id */
    AMP_NETFIFO_NOTIFY_STOPNETPLAY,         /**< Stop playback for streaming. set param1 as STARTENC to indecate APP is recording */
    AMP_NETFIFO_NOTIFY_RELEASE              /**< RTOS NetFifo mudule released */
} AMP_NETFIFO_NOTIFY_TYPE_e;

/**
 * NETFifo event type
 */
typedef enum _AMP_NETFIFO_EVENT_e_ {
    AMP_NETFIFO_EVENT_START = 1,            /**< The start event of the NetFifo. The NetFifo start to run. */
    AMP_NETFIFO_EVENT_END,                  /**< The end event of the NetFifo. The NetFifo complete stopped. */
    AMP_NETFIFO_EVENT_SWITCHENCSESSION,     /**< The end event of the NetFifo. The NetFifo is stopped for enc session switching. */

    AMP_NETFIFO_EVENT_GENERAL_ERROR = 0xF0  /**< The error event of the NetFifo. */
} AMP_NETFIFO_EVENT_e;

/**
 * Network Playback op type
 */
typedef enum _AMP_NETFIFO_PLAYBACK_OP_e_ {
    AMP_NETFIFO_PLAYBACK_OPEN = 1,      /**< open file for playback. */
    AMP_NETFIFO_PLAYBACK_PLAY,          /**< start playback. */
    AMP_NETFIFO_PLAYBACK_STOP,          /**< stop playback */
    AMP_NETFIFO_PLAYBACK_RESET,         /**< reset playback */
    AMP_NETFIFO_PLAYBACK_PAUSE,         /**< pause playback */
    AMP_NETFIFO_PLAYBACK_RESUME,        /**< resume playback */
    AMP_NETFIFO_PLAYBACK_CONFIG,        /**< enabling playback stream */
    AMP_NETFIFO_PLAYBACK_GET_VID_FTIME, /**< retrieve video time per frame */
    AMP_NETFIFO_PLAYBACK_GET_VID_TICK,  /**< retrieve video tick per frame */
    AMP_NETFIFO_PLAYBACK_GET_AUD_FTIME, /**< retrieve audio time per frame */
    AMP_NETFIFO_PLAYBACK_GET_DURATION,  /**< retrieve clip duration */
    AMP_NETFIFO_PLAYBACK_SET_LIVE_BITRATE,      /**< set AVG bitrate */
    AMP_NETFIFO_PLAYBACK_GET_LIVE_BITRATE,      /**< get latest reported bitrate from BRC */
    AMP_NETFIFO_PLAYBACK_GET_LIVE_AVG_BITRATE,  /**< get sensor setting */
    AMP_NETFIFO_PLAYBACK_SET_NET_BANDWIDTH,     /**< set bandwidth for BRC callback */
    AMP_NETFIFO_PLAYBACK_SEND_RR_STAT,     /**< send rr report */
    AMP_NETFIFO_PLAYBACK_SET_PARAMETER,     /**< send RTSP extend field */
    AMP_NETFIFO_PLAYBACK_GET_SPS_PPS    /**< get sps&pps of the specifid file or playback file */
} AMP_NETFIFO_PLAYBACK_OP_e;


/**
 * to get netfifo package version info
 *
 * @return Version Number of Net Package
 */
extern int AmpNetFifo_GetVer(void);

/**
 * Get default configuration for init.
 *
 * @param [out] defaultCfg the init config
 *
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 * @see AMP_NETFIFO_INIT_CFG_s
 */
extern int AmpNetFifo_GetInitDefaultCfg(AMP_NETFIFO_INIT_CFG_s *defaultCfg);

/**
 * module init function
 *
 * @param [in] InitCfg the init config
 *
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 * @see AMP_NETFIFO_INIT_CFG_s
 */
extern int AmpNetFifo_init(AMP_NETFIFO_INIT_CFG_s *InitCfg);

/**
 * Dummy Callback function to avoid encode fifo full when there is not valid consumer handler,
 *
 * @param [in] hdlr the fifo handler
 * @param [in] event the fifo event
 * @param [in] info the further info for the event
 *
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpNetFifo_DefaultCB(void *hdlr, UINT32 event, void* info);

/**
 * Get movie's configuration.
 *
 * @param [out] pConfig the default info of media configuration
 *
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 * @see AMP_NETFIFO_MOVIE_INFO_CFG_s
 */
extern int AmpNetFifo_GetDefaultMovieInfoCfg(AMP_NETFIFO_MOVIE_INFO_CFG_s *pConfig);

/**
 * Notify APP status/operation change.
 *
 * @param [in] notify Type for notification. (see AMP_NETFIFO_NOTIFY_TYPE_e)
 * @param [in] param1 extra info for the notification (1st). Pass 0 if there is no need.
 * @param [in] param2 extra info for the notification (2nd). Pass 0 if there is no need.
 *
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpNetFifo_SendNotify(UINT32 notify, UINT32 param1, UINT32 param2);

/**
 * Register Callback for NetFiFO event.
 *
 * The parameters for the call back are specified as:\n
 * hdlr: not used in this callback, will be set as NULL.\n
 * event: the event id, see AMP_NETFIFO_EVENT_e.\n
 * info: not used in this callback, will be set as NULL.\n
 *
 * callback should return as follow:\n
 * return 0 - OK, others - AMP_ER_CODE_e\n
 *
 * @param [in] cbEvent the callback function for Event.
 *
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpNetFifo_RegEventCb(AMP_CALLBACK_f cbEvent);

/**
 * Register Callback for NetFifo to get Encode Media configurations.\n
 *
 * The parameters for the call back are specified as:\n
 * hdlr: not used in this callback, will be set as NULL.\n
 * event: the Stream ID which this callback ask for.\n
 * info: the output AMP_NETFIFO_MOVIE_INFO_CFG_s buffer for this callback.\n
 *
 * callback should return as follow:\n
 * return 0 - OK, others - AMP_ER_CODE_e\n
 *
 * @param [in] cbMediaInfo the callback function for MediaInfo.
 *
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 *
 */
extern int AmpNetFifo_RegMediaInfoCb(AMP_CALLBACK_f cbMediaInfo);

/**
 * Register Callback for NetFiFO event.
 *
 * The parameters for the call back are specified as:\n
 * hdlr: not used in this callback, will be set as NULL.\n
 * event: the requested operation, see AMP_NETFIFO_PLAYBACK_OP_e.\n
 * info: extend info for the operation. will be different type for different operation.\n
 *
 * callback should return as follow:\n
 * return 0 - OK, others - AMP_ER_CODE_e\n
 *
 * @param [in] cbPlayback the callback function for Playback.
 *
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpNetFifo_RegPlaybackCb(AMP_CALLBACK_f cbPlayback);


/**
 * @}
 */

#endif /* AMP_NETFIFO_H_ */
