/**
 * @file inc/mw/net/rpcprog/AmbaIPC_RpcProg_RT_NetFifo.h
 *
 * Header file for NetFifo RPC Services (RTOS side)
 *
 * Copyright (C) 2014, Ambarella, Inc.
 *
 * All rights reserved. No Part of this file may be reproduced, stored
 * in a retrieval system, or transmitted, in any form, or by any means,
 * electronic, mechanical, photocopying, recording, or otherwise,
 * without the prior consent of Ambarella, Inc.
 */
#ifndef _RPC_PROG_RT_NETFIFO_H_
#define _RPC_PROG_RT_NETFIFO_H_

#include "AmbaIPC_Rpc_Def.h"

#ifndef FOREVER
#define FOREVER 0xFFFFFFFF
#endif

//RPC_INFO definition
#define RT_NETFIFO_PROG_ID	0x10000004
#define RT_NETFIFO_HOST AMBA_IPC_HOST_THREADX
#define RT_NETFIFO_VER (1)
#define RT_NETFIFO_DEFULT_TIMEOUT (FOREVER)
#define RT_NETFIFO_NAME "LINK_RPC_SVC_NETFIFO"

//===== mw.h =====
#define RT_NETFIFO_EVENT_START_NUM (0x04000000) //Should be sync with AMP_FIFO_EVENT_START_NUM in mw.h

//===== fifo.h =====
#define RT_NETFIFO_MARK_EOS           0x00FFFFFF
#define RT_NETFIFO_MARK_EOS_PAUSE     0x00FFFFFE

typedef enum _RT_NETFIFO_EVENT_e_ {
    RT_NETFIFO_EVENT_DATA_CONSUMED = RT_NETFIFO_EVENT_START_NUM, /**< data consumed event to data provider (write fifo)*/
    RT_NETFIFO_EVENT_DATA_EOS, /**< data end of stream to data consumer (read fifo)*/
    RT_NETFIFO_EVENT_DATA_READY, /**< new data ready to data consumer (read fifo)*/
    RT_NETFIFO_EVENT_GET_WRITE_POINT, /**< get write pointer info from data consumer (write fifo)*/
    RT_NETFIFO_EVENT_RESET_FIFO /**< ask data consumer (write fifo) to reset everything for bitsfifo reuse*/
} RT_NETFIFO_EVENT_e;

typedef enum _RT_NETFIFO_FRMAE_TYPE_e_ {
    RT_NETFIFO_TYPE_MJPEG_FRAME = 0,    ///< MJPEG frame type
    RT_NETFIFO_TYPE_IDR_FRAME = 1,    ///< Idr frame type
    RT_NETFIFO_TYPE_I_FRAME = 2,      ///< I frame type
    RT_NETFIFO_TYPE_P_FRAME = 3,      ///< P frame type
    RT_NETFIFO_TYPE_B_FRAME = 4,      ///< B frame type
    RT_NETFIFO_TYPE_JPEG_FRAME = 5,   ///< jpeg main frame
    RT_NETFIFO_TYPE_THUMBNAIL_FRAME = 6,  ///< jpeg thumbnail frame
    RT_NETFIFO_TYPE_SCREENNAIL_FRAME = 7, ///< jpeg screennail frame
    RT_NETFIFO_TYPE_AUDIO_FRAME = 8,      ///< audio frame
    RT_NETFIFO_TYPE_UNDEFINED = 9,        ///< others

    RT_NETFIFO_TYPE_DECODE_MARK = 101, ///< used when feeding bitstream to dsp. will push out all frame. */
    RT_NETFIFO_TYPE_EOS = 255,                  ///< eos bits that feed to raw buffer

    RT_NETFIFO_TYPE_LAST = RT_NETFIFO_TYPE_EOS
} RT_NETFIFO_FRMAE_TYPE_e;

typedef struct _RT_NETFIFO_s_ {
    unsigned int nFifoId; /**< the unique of the fifo */
    void *Ctx; /**< private data of the fifo */
} RT_NETFIFO_HDLR_s;

typedef struct _RT_NETFIFO_BITS_DESC_s_ {
    unsigned int SeqNum; /**< sequential number of bits buffer */
    unsigned long long Pts; /**< time stamp in ticks */
    unsigned char Type; /**< data type of the entry. see RT_NETFIFO_FRMAE_TYPE_e*/
    unsigned char Completed; /**< if the buffer content a complete entry */
    unsigned short  Align; /** data size alignment (in bytes, align = 2^n, n is a integer )*/
    unsigned char* StartAddr; /**< start address of data */
    unsigned int Size; /**< real data size */
} RT_NETFIFO_BITS_DESC_s;


typedef enum _RT_NETFIFO_CFG_INIT_DATA_FETCH_CONDITION_e_{
    RT_NETFIFO_CFG_INIT_DISABLE = 0,  /**< no neeed for init data */
    RT_NETFIFO_CFG_INIT_WITH_TIME, /**< get frames with given length */
    RT_NETFIFO_CFG_INIT_WITH_NUM_FRAME, /**< get frames with given frame number. ex. if you set  NumFrame to 100, you will get 100 frames(if there are 100 frames)*/
} RT_NETFIFO_CFG_INIT_DATA_FETCH_CONDITION_e;

typedef struct _RT_NETFIFO_CFG_INIT_DATA_s_ {
    unsigned char CreateFifoWithInitData; /**< If create fifo with init data (if data valid) */

    RT_NETFIFO_CFG_INIT_DATA_FETCH_CONDITION_e InitCondition; /**< define how to descript initial data required. */

    union {
        unsigned long long TimeLength; /**< In normal case, the value of backward fetch is 0.\n
         * it means fifo will output frames only after fifo is created.\n
         * As the valus is not 0, fifo will try to find out the longest valid frames
         */
        unsigned long long NumFrame; /**< number of frame */
    } InitParam; /**< parameter for determinate initial data*/

    RT_NETFIFO_FRMAE_TYPE_e FristFrameType; /**< Frame type of first frame for valid data, used on video stream to ensure start with Idr. */

    unsigned long long OnCreateFirstFramePts; /** [OUT] Pts of first frame, 0xFFFFFFFFFFFFFFFF if first frame is not valid on create */
    unsigned long long OnCreateTimeLength; /** [OUT] data length in fifo on create in ms */
} RT_NETFIFO_CFG_INIT_DATA_s;

typedef struct _RT_NETFIFO_CFG_s_ {
    void *hCodec; /**< the codec the fifo is working on. */
    unsigned int NumEntries; /**< Number of entries of a fifo */
    unsigned int IsVirtual; /**< Creating virtual fifo (for data flow) or not (for codec) */
    unsigned int cbEvent; /**< the callback function for fifo event */
    unsigned int EventDataConsumedThreshold; /**<
     * the threshold for event RT_NETFIFO_CALLBACK_EVENT_DATA_CONSUMED\n
     * event only triggered if remain data us under the threshold.
     * 0 to disable
     */
    unsigned int EventDataReadySkipNum; /**<
     * if not 0, RT_NETFIFO_CALLBACK_EVENT_DATA_READY will not be triggered every frame encoded.\n
     * it will be triggered every (eventDataReadySkipNum frame+1) frames
     */
    unsigned int cbGetWritePoint; /**<
     * the callback function invoked when we receive prepareSpace.\n
     * it should ONLY be used on fifo linked to codec \n
     * it only take effect on a codec read fifo.\n
     * the callback should be registered by a decode mgr for all decode codec\n
     */

    unsigned char SyncRpOnWrite;
    /**< if 0, rp of base fifo will be updated on remove of virtual fifo.\n
     *   if 1, rp of base fifo will be updated on write of base fifo.\n
     *   ONLY take effect on base fifo.
     */

    unsigned int RawBaseAddr; /**< when sync rp on write is on and RawBaseAddr/RawLimitAddr is given, base fifo will remove desc automatically on overwrite. \n
                        * only needed on base fifo and sync on write is on
                        */

    unsigned int RawLimitAddr; /**< when sync rp on write is on and RawBaseAddr/RawLimitAddr is given, base fifo will remove desc automatically on overwrite. \n
                        * only needed on base fifo and sync on write is on
                        */

    unsigned long long TickPerSecond; /**< TiackPerSecond used for Pts for the fifo. \n
    Could be 0 if we whould NOT like to use init data. */

    RT_NETFIFO_CFG_INIT_DATA_s InitData; /**< init status for virtual fifo */
} RT_NETFIFO_CFG_s;

typedef struct _RT_NETFIFO_INFO_s_ {
    unsigned int TotalEntries; /**< total number of entries */
    unsigned int AvailEntries; /**< number of entries with data*/
} RT_NETFIFO_INFO_s;

//===== NetFifo.h(Muxer.h) =====
typedef enum _RT_NETFIFO_MEDIA_TRACK_TYPE_e_ {
    RT_NETFIFO_MEDIA_TRACK_TYPE_VIDEO = 0x01,  /**< The track's type is Video */
    RT_NETFIFO_MEDIA_TRACK_TYPE_AUDIO = 0x02,  /**< The track's type is Audio */
    RT_NETFIFO_MEDIA_TRACK_TYPE_TEXT = 0x03,   /**< The track's type is Text */
    RT_NETFIFO_MEDIA_TRACK_TYPE_MAX = 0x04     /**< Max value, for check use */
} RT_NETFIFO_MEDIA_TRACK_TYPE_e;

typedef struct _RT_NETFIFO_VIDEO_TRACK_CFG_s_ {
    unsigned int nGOPSize;            /**< The number of the picture between IDR pictures. */
    unsigned int nTrickRecDen;        /**< For AmpFormat_ConvertPTS(), the Denominator of the trick record. If is high frame rate, the denominator and numerator are specify the factor than default 30fps, Ex: 120fps -> Den = 1, Num = 4 */
    unsigned int nTrickRecNum;        /**< For AmpFormat_ConvertPTS(), the Numerator of the trick record. If is high frame rate, the denominator and numerator are specify the factor than default 30fps, Ex: 120fps -> Den = 1, Num = 4 */
    unsigned int nCodecTimeScale;     /**< TODO: It needs get from H264 bitstream, it is not configurable. */
    unsigned short  nWidth;              /**< Picture width */
    unsigned short  nHeight;             /**< Picture height */
    unsigned short  nM;                  /**< The number of the picture between reference pictures(IDR, I, P) */
    unsigned short  nN;                  /**< The number of the picture between I pictures */
    unsigned char bDefault;             /**< The flag defines the track as default video track, if the media hasn't one video track. */
    unsigned char nMode;                /**< The value defines the picture mode of the video. It has progressive and interlaced mode. Interlaced mode has Field Per Sample and Frame Per Sample, See AMP_VIDEO_MODE_s */
    unsigned char bClosedGOP;           /**< The structure of the Close GOP is I P B B P B B. The structure of the Open GOP is I B B P B B, If resume or auto split, the value always is Open GOP. */
} RT_NETFIFO_VIDEO_TRACK_CFG_s;

typedef struct _RT_NETFIFO_AUDIO_TRACK_CFG_s_ {
    unsigned int nSampleRate;         /**< The sample rate(Hz) of the audio track. */
    unsigned char bDefault;             /**< The flag defines the track as default audio track, if the media hasn't one audio track. */
    unsigned char nChannels;            /**< The number of audio channel. */
    unsigned char nBitsPerSample;       /**< The per sample size of the audio track. Ex: 8 bits, 16 bits, ....n bits */
} RT_NETFIFO_AUDIO_TRACK_CFG_s;

typedef struct _RT_NETFIFO_TEXT_TRACK_CFG_s_ {
    unsigned char bDefault;             /**< The flag defines the track as default text track, if the media hasn't one text track. */
} RT_NETFIFO_TEXT_TRACK_CFG_s;

typedef struct _RT_NETFIFO_MEDIA_TRACK_CFG_s_{
    unsigned int nMediaId;            /**< The media type of the track. The id is media id, See AMP_FORMAT_MID_e */
    unsigned int nTimeScale;          /**< Time scale, the same as the LCM of those of all tracks */
    unsigned int nTimePerFrame;       /**< The time of the frame that the unit of it is Time scale. */
    unsigned int nInitDelay;          /**< Initial delay time(ms) of the track. */
    void *hCodec;		/**< the codec which this track is working on. */
    unsigned char *pBufferBase;            /**< The start address of the FIFO of the track. User pushs data to the FIFO, the FIFO will write data to the address of the buffer. */
    unsigned char *pBufferLimit;           /**< The end address of the FIFO of the track, The data can't write overlap the address, FIFO size = FIFO buffer limit - FIFO buffer base. */
    union {
        RT_NETFIFO_VIDEO_TRACK_CFG_s Video;
        RT_NETFIFO_AUDIO_TRACK_CFG_s Audio;
        RT_NETFIFO_TEXT_TRACK_CFG_s Text;
    } Info;
    unsigned char nTrackType;           /**< Track type, See AMP_MEDIA_TRACK_TYPE_e */
} RT_NETFIFO_MEDIA_TRACK_CFG_s;

#define RT_NETFIFO_MAX_TRACK_PER_MEDIA  4   /**< The max track number of a media. */

typedef struct _RT_NETFIFO_MOVIE_INFO_CFG_s_ {
    RT_NETFIFO_MEDIA_TRACK_CFG_s Track[RT_NETFIFO_MAX_TRACK_PER_MEDIA];
    unsigned char nTrack;               /**< The number of Tracks in the movie. */
} RT_NETFIFO_MOVIE_INFO_CFG_s;

//===== self sepcific =====
typedef struct _RT_NETFIFO_PEEKENTRY_ARG_s_ {
    RT_NETFIFO_HDLR_s *fifo;
    unsigned int distanceToLastEntry;
} RT_NETFIFO_PEEKENTRY_ARG_s;

typedef struct _RT_NETFIFO_REMOVEENTRY_ARG_s_ {
    RT_NETFIFO_HDLR_s *fifo;
    unsigned int EntriesToBeRemoved;
} RT_NETFIFO_REMOVEENTRY_ARG_s;

typedef struct _RT_NETFIFO_WRITEENTRY_ARG_s_ {
    RT_NETFIFO_HDLR_s *fifo;
    RT_NETFIFO_BITS_DESC_s desc;
} RT_NETFIFO_WRITEENTRY_ARG_s;

typedef struct _RT_NETFIFO_MEDIA_STREAMID_LIST_s_ {
    int Amount;
    int StreamID_List[16];
} RT_NETFIFO_MEDIA_STREAMID_LIST_s;

typedef struct _RT_NETFIFO_PLAYBACK_OP_PARAM_s_ {
    unsigned int OP;
    unsigned char Param[128];
} RT_NETFIFO_PLAYBACK_OP_PARAM_s;

//============ RPC_FUNC definition ============
enum _RT_NETFIFO_FUNC_e_ {
    RT_NETFIFO_FUNC_GETDEFAULTCFG = 1,
    RT_NETFIFO_FUNC_CREATE,
    RT_NETFIFO_FUNC_DELETE,
    RT_NETFIFO_FUNC_ERASEALL,
    RT_NETFIFO_FUNC_GETINFO,
    RT_NETFIFO_FUNC_PEEKENTRY,
    RT_NETFIFO_FUNC_REVPEEKENTRY,
    RT_NETFIFO_FUNC_REMOVEENTRY,
    RT_NETFIFO_FUNC_PREPAREENTRY,
    RT_NETFIFO_FUNC_WRITEENTRY,
    RT_NETFIFO_FUNC_GETMEDIASTREAMID,
    RT_NETFIFO_FUNC_GETMEDIAINFO,
    RT_NETFIFO_FUNC_PLAYBACK_OP,
    RT_NETFIFO_FUNC_REPORTSTATUS,

    RT_NETFIFO_FUNC_AMOUNT
};


/**
 * [in] NULL
 * [out] RT_NETFIFO_CFG_s
 */
AMBA_IPC_REPLY_STATUS_e RT_NetFifo_GetDefaultCfg_Clnt(void *pArg, int *pResult, int Clnt );
void RT_NetFifo_GetDefaultCfg_Svc(void *pArg, AMBA_IPC_SVC_RESULT_s *pRet);

/**
 * [in] RT_NETFIFO_CFG_s
 * [out] (RT_NETFIFO_HDLR_s**)
 */
AMBA_IPC_REPLY_STATUS_e RT_NetFifo_Create_Clnt(RT_NETFIFO_CFG_s *pArg, int *pResult, int Clnt );
void RT_NetFifo_Create_Svc(RT_NETFIFO_CFG_s *pArg, AMBA_IPC_SVC_RESULT_s *pRet);

/**
 * [in] (RT_NETFIFO_HDLR_s *)
 * [out] int
 */
AMBA_IPC_REPLY_STATUS_e RT_NetFifo_Delete_Clnt(RT_NETFIFO_HDLR_s **pArg, int *pResult, int Clnt );
void RT_NetFifo_Delete_Svc(RT_NETFIFO_HDLR_s **pArg, AMBA_IPC_SVC_RESULT_s *pRet);

/**
 * [in] (RT_NETFIFO_HDLR_s *)
 * [out] int
 */
AMBA_IPC_REPLY_STATUS_e RT_NetFifo_EraseAll_Clnt(RT_NETFIFO_HDLR_s **pArg, int *pResult, int Clnt );
void RT_NetFifo_EraseAll_Svc(RT_NETFIFO_HDLR_s **pArg, AMBA_IPC_SVC_RESULT_s *pRet);

/**
 * [in] (RT_NETFIFO_HDLR_s *)
 * [out] RT_NETFIFO_INFO_s
 */
AMBA_IPC_REPLY_STATUS_e RT_NetFifo_GetInfo_Clnt(RT_NETFIFO_HDLR_s **pArg, int *pResult, int Clnt );
void RT_NetFifo_GetInfo_Svc(RT_NETFIFO_HDLR_s **pArg, AMBA_IPC_SVC_RESULT_s *pRet);

/**
 * [in] RT_NETFIFO_PEEKENTRY_ARG_s
 * [out] RT_NETFIFO_BITS_DESC_s
 */
AMBA_IPC_REPLY_STATUS_e RT_NetFifo_PeekEntry_Clnt(RT_NETFIFO_PEEKENTRY_ARG_s *pArg, int *pResult, int Clnt );
void RT_NetFifo_PeekEntry_Svc(RT_NETFIFO_PEEKENTRY_ARG_s *pArg, AMBA_IPC_SVC_RESULT_s *pRet);

/**
 * [in] RT_NETFIFO_PEEKENTRY_ARG_s
 * [out] RT_NETFIFO_BITS_DESC_s
 */
AMBA_IPC_REPLY_STATUS_e RT_NetFifo_RevPeekEntry_Clnt(RT_NETFIFO_PEEKENTRY_ARG_s *pArg, int *pResult, int Clnt );
void RT_NetFifo_RevPeekEntry_Svc(RT_NETFIFO_PEEKENTRY_ARG_s *pArg, AMBA_IPC_SVC_RESULT_s *pRet);

/**
 * [in] RT_NETFIFO_REMOVEENTRY_ARG_s
 * [out] int
 */
AMBA_IPC_REPLY_STATUS_e RT_NetFifo_RemoveEntry_Clnt(RT_NETFIFO_REMOVEENTRY_ARG_s *pArg, int *pResult, int Clnt );
void RT_NetFifo_RemoveEntry_Svc(RT_NETFIFO_REMOVEENTRY_ARG_s *pArg, AMBA_IPC_SVC_RESULT_s *pRet);

/**
 * [in] (RT_NETFIFO_HDLR_s *)
 * [out] RT_NETFIFO_BITS_DESC_s
 */
AMBA_IPC_REPLY_STATUS_e RT_NetFifo_PrepareEntry_Clnt(RT_NETFIFO_HDLR_s **pArg, int *pResult, int Clnt );
void RT_NetFifo_PrepareEntry_Svc(RT_NETFIFO_HDLR_s **pArg, AMBA_IPC_SVC_RESULT_s *pRet);

/**
 * [in] RT_NETFIFO_WRITEENTRY_ARG_s
 * [out] int
 */
AMBA_IPC_REPLY_STATUS_e RT_NetFifo_WriteEntry_Clnt(RT_NETFIFO_WRITEENTRY_ARG_s *pArg, int *pResult, int Clnt );
void RT_NetFifo_WriteEntry_Svc(RT_NETFIFO_WRITEENTRY_ARG_s *pArg, AMBA_IPC_SVC_RESULT_s *pRet);

/**
 * [in] void
 * [out] RT_NETFIFO_MEDIA_STREAMID_LIST_s
 */
AMBA_IPC_REPLY_STATUS_e RT_NetFifo_GetMediaStreamIDList_Clnt(void *pArg, int *pResult, int Clnt );
void RT_NetFifo_GetMediaStreamIDList_Svc(void *pArg, AMBA_IPC_SVC_RESULT_s *pRet);

/**
 * [in] int
 * [out] RT_NETFIFO_MOVIE_INFO_CFG_s
 */
AMBA_IPC_REPLY_STATUS_e RT_NetFifo_GetMediaInfo_Clnt(unsigned int *pArg, int *pResult, int Clnt );
void RT_NetFifo_GetMediaInfo_Svc(unsigned int *pArg, AMBA_IPC_SVC_RESULT_s *pRet);

/**
 * [in] RT_NETFIFO_PLAYBACK_OP_PARAM_s
 * [out] RT_NETFIFO_PLAYBACK_OP_PARAM_s
 */
AMBA_IPC_REPLY_STATUS_e RT_NetFifo_Playback_OP_Clnt(RT_NETFIFO_PLAYBACK_OP_PARAM_s *pArg, int *pResult, int Clnt );
void RT_NetFifo_Playback_OP_Svc(RT_NETFIFO_PLAYBACK_OP_PARAM_s *pArg, AMBA_IPC_SVC_RESULT_s *pRet);

/**
 * [in] unsigned int
 * [out] int
 */
AMBA_IPC_REPLY_STATUS_e RT_NetFifo_ReportStatus_Clnt(unsigned int *pArg, int *pResult, int Clnt );
void RT_NetFifo_ReportStatus_Svc(unsigned int *pArg, AMBA_IPC_SVC_RESULT_s *pRet);

#endif /* _RPC_PROG_RT_NETFIFO_H_ */
