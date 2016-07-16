#include <net/NetFifo.h>
#include <net/NetUtility.h>
#include <net/ApplibNet.h>
#include <net/ApplibNet_Fifo.h>
#include <applib.h>

#include <recorder/VideoEnc.h>
#include <format/FormatDef.h>
#include <msgqueue.h>
#include <transcoder/video_decode/ApplibTranscoder_VideoDec.h>
#include "../AppLibTask_Priority.h"
#include "encmonitor/encMonitorService.h"
#include <AmbaRTC.h>

//#define NETFIFO_DBG_EN
#define NETFIFO_ERR_EN

#undef NETFIFO_DBG
#ifdef NETFIFO_DBG_EN
#define NETFIFO_DBG(fmt,args...) AmbaPrintColor(YELLOW,fmt,##args);
#else
#define NETFIFO_DBG(fmt,args...)
#endif

#undef NETFIFO_ERR
#ifdef NETFIFO_ERR_EN
#define NETFIFO_ERR(fmt,args...) AmbaPrintColor(RED,fmt,##args);
#else
#define NETFIFO_ERR(fmt,args...)
#endif

static UINT8 gNetFifoInited = 0;
static UINT8 gRTSPServerOn = 0; // 1: RTSP server is on, 0: RTSP server is off
static APPLIB_VIDEOENC_STREAM_LIST_s gStreamList = {0};
static AMP_NETFIFO_NOTIFY_TYPE_e gState = AMP_NETFIFO_NOTIFY_RELEASE;
static NET_STREAM_MODE_e gNetStreamMode = NET_STREAM_MODE_AV;
static NET_STREAM_BITRATE_STAT_s gNetBitrateStat = {0};

extern UINT8 *AudPriBitsBuf;
extern UINT8 *AudSecBitsBuf;

// update: get from ApplibRecorder_VideoEnc
#define AUDENC_VIDENC_BITSFIFO_SIZE (6 << 20)

//--------------------------------------------------------------------------------------------------
// net playback
//--------------------------------------------------------------------------------------------------
#define NETFIFO_PB_MGR_STACK_SIZE    (0x4000) // TODO: need to be checked
#define NETFIFO_PB_MGR_NAME          "AppLib_Net_Playback_Manager"
#define NETFIFO_PB_MGR_MSGQUEUE_SIZE (16)

#define NETFIFO_PB_MEDIA_STREAM_ID (0xabcd)


typedef struct _NETFIFO_PB_MGR_s_ {
    UINT8 IsInit;
    UINT8 Stack[NETFIFO_PB_MGR_STACK_SIZE];  /**< Stack */
    UINT8 MsgPool[sizeof(APP_NET_PB_MESSAGE_s)*NETFIFO_PB_MGR_MSGQUEUE_SIZE];   /**< Message memory pool. */
    AMBA_KAL_TASK_t Task;               /**< Task ID */
    AMBA_KAL_MSG_QUEUE_t MsgQueue;      /**< Message queue ID */
    APPLIB_NETFIFO_PB_f NetPlaybackEntry;
} NETFIFO_PB_MGR_s;

static NETFIFO_PB_MGR_s gNetFifoPBMgr = {0};
static char RequestedFilename[APPLIB_NETFIFO_MAX_FN_SIZE] = {0};


static int ConvertFilePathPrefix(char *pFilename, char *pRetFilename, int RetFilenameBufSize)
{
    char *pLnxMountPoint = "/tmp/SD0";
    char *pStrFound = NULL;
    int Len = 0;
    char *pTemp = NULL;

    if ((!pFilename) || (!pRetFilename) || (RetFilenameBufSize == 0)) {
        NETFIFO_ERR("[AppLib - NetFifo] <ConvertFilePathPrefix> invalid parameter");
        return-1;
    }

    pStrFound = strstr(pFilename, pLnxMountPoint);
    if (pStrFound) {
        if ((strlen(pStrFound) - strlen(pLnxMountPoint) + 3) > RetFilenameBufSize) {
            NETFIFO_ERR("[AppLib - NetFifo] <ConvertFilePathPrefix> filename buffer is not enough (need: %d)", (strlen(pStrFound) - strlen(pLnxMountPoint) + 3));
            return -1;
        }

        snprintf(pRetFilename, RetFilenameBufSize, "C:%s", pStrFound+strlen(pLnxMountPoint));
    } else {
        NETFIFO_ERR("[AppLib - NetFifo] <ConvertFilePathPrefix> file path is not a full path '%s'", pFilename);
        return -1;
    }

    Len = strlen(pRetFilename);
    pTemp = pRetFilename;
    while (Len--) {
        if(*pTemp == '/'){
            *pTemp = '\\';
        }

        pTemp++;
    }

    //AmbaPrintColor(YELLOW, "<ConvertFilePathPrefix> pFilename = -%s-, pRetFilename = -%s-", pFilename, pRetFilename);
    return 0;
}

static int PlaybackOpen(char *Filename)
{
    APP_NET_PB_MESSAGE_s Msg = {0};
    int ReturnValue = 0;

    if (gNetFifoPBMgr.IsInit != 1) {
        NETFIFO_ERR("[AppLib - NetFifo] <PlaybackOpen> RTSP playback not inited!");
        return -1;
    }

    if (!Filename) {
        NETFIFO_ERR("[AppLib - NetFifo] <PlaybackOpen> invalid parameter!");
        return -1;
    }

    /* convert file name format from linux style to CFS style.
         example: /tmp/SD0/filename -> C:\DCIM\100MEDIA\filename */
    ReturnValue = ConvertFilePathPrefix(Filename, RequestedFilename, sizeof(RequestedFilename));
    if (ReturnValue != 0) {
        return -1;
    }

    Msg.MessageID = APPLIB_NETFIFO_PLAYBACK_OPEN;
    snprintf(Msg.Filename, sizeof(Msg.Filename), "%s", RequestedFilename);

    ReturnValue = AppLibNetFifo_PlaybackSendMsg(&Msg, AMBA_KAL_NO_WAIT);
    if (ReturnValue != 0) {
         NETFIFO_ERR("[AppLib - NetFifo] <PlaybackPlay> AppLibNetFifo_PlaybackSendMsg() fail");
         ReturnValue = -1;
    }

    return ReturnValue;
}

static int PlaybackPlay(UINT32 StartTime)
{
    APP_NET_PB_MESSAGE_s Msg = {0};
    int ReturnValue = 0;

    if (gNetFifoPBMgr.IsInit != 1) {
        NETFIFO_ERR("[AppLib - NetFifo] <PlaybackOpen> RTSP playback not inited!");
        return -1;
    }

    Msg.MessageID = APPLIB_NETFIFO_PLAYBACK_PLAY;
    Msg.MessageData[0] = StartTime;
    snprintf(Msg.Filename, sizeof(Msg.Filename), "%s", RequestedFilename);

    ReturnValue = AppLibNetFifo_PlaybackSendMsg(&Msg, AMBA_KAL_NO_WAIT);
    if (ReturnValue != 0) {
         NETFIFO_ERR("[AppLib - NetFifo] <PlaybackPlay> AppLibNetFifo_PlaybackSendMsg() fail");
         ReturnValue = -1;
    }

    return ReturnValue;
}

static int PlaybackStop(void)
{
    APP_NET_PB_MESSAGE_s Msg = {0};
    int ReturnValue = 0;

    if (gNetFifoPBMgr.IsInit != 1) {
        NETFIFO_ERR("[AppLib - NetFifo] <PlaybackOpen> RTSP playback not inited!");
        return -1;
    }

    Msg.MessageID = APPLIB_NETFIFO_PLAYBACK_STOP;
    ReturnValue = AppLibNetFifo_PlaybackSendMsg(&Msg, AMBA_KAL_NO_WAIT);
    if (ReturnValue != 0) {
         NETFIFO_ERR("[AppLib - NetFifo] <PlaybackStop> AppLibNetFifo_PlaybackSendMsg() fail");
         ReturnValue = -1;
    }

    return ReturnValue;
}

static int PlaybackReset(void)
{
    APP_NET_PB_MESSAGE_s Msg = {0};
    int ReturnValue = 0;

    if (gNetFifoPBMgr.IsInit != 1) {
        NETFIFO_ERR("[AppLib - NetFifo] <PlaybackOpen> RTSP playback not inited!");
        return -1;
    }

    /* reset RequestedFilename if file opend but not played. */

    Msg.MessageID = APPLIB_NETFIFO_PLAYBACK_RESET;
    ReturnValue = AppLibNetFifo_PlaybackSendMsg(&Msg, AMBA_KAL_NO_WAIT);
    if (ReturnValue != 0) {
         NETFIFO_ERR("[AppLib - NetFifo] <PlaybackReset> AppLibNetFifo_PlaybackSendMsg() fail");
         ReturnValue = -1;
    }

    return ReturnValue;
}

static int PlaybackPause(void)
{
    APP_NET_PB_MESSAGE_s Msg = {0};
    int ReturnValue = 0;

    if (gNetFifoPBMgr.IsInit != 1) {
        NETFIFO_ERR("[AppLib - NetFifo] <PlaybackOpen> RTSP playback not inited!");
        return -1;
    }

    Msg.MessageID = APPLIB_NETFIFO_PLAYBACK_PAUSE;
    ReturnValue = AppLibNetFifo_PlaybackSendMsg(&Msg, AMBA_KAL_NO_WAIT);
    if (ReturnValue != 0) {
         NETFIFO_ERR("[AppLib - NetFifo] <PlaybackPause> AppLibNetFifo_PlaybackSendMsg() fail");
         ReturnValue = -1;
    }

    return ReturnValue;
}

static int PlaybackResume(void)
{
    APP_NET_PB_MESSAGE_s Msg = {0};
    int ReturnValue = 0;

    if (gNetFifoPBMgr.IsInit != 1) {
        NETFIFO_ERR("[AppLib - NetFifo] <PlaybackOpen> RTSP playback not inited!");
        return -1;
    }

    Msg.MessageID = APPLIB_NETFIFO_PLAYBACK_RESUME;
    ReturnValue = AppLibNetFifo_PlaybackSendMsg(&Msg, AMBA_KAL_NO_WAIT);
    if (ReturnValue != 0) {
         NETFIFO_ERR("[AppLib - NetFifo] <PlaybackResume> AppLibNetFifo_PlaybackSendMsg() fail");
         ReturnValue = -1;
    }

    return ReturnValue;
}

static int PlaybackGetVideoInfo(int *VideoInfo, int InfoType)
{
    APPLIB_MEDIA_INFO_s MediaInfo = {0};
    UINT32 DTS = 0;
    UINT32 TimeScale = 0;
    UINT32 TimePerFrame = 0;
    UINT64 Duration = 0;
    int i = 0;
    int ReturnValue = 0;

    if (gNetFifoPBMgr.IsInit != 1) {
        NETFIFO_ERR("[AppLib - NetFifo] <PlaybackOpen> RTSP playback not inited!");
        return -1;
    }

    if (!VideoInfo) {
        NETFIFO_ERR("[AppLib - NetFifo] <PlaybackGetVideoInfo> NULL pointer");
        return -1;
    }

    if (RequestedFilename[0] == '\0') {
        NETFIFO_ERR("[AppLib - NetFifo] <PlaybackGetVideoInfo> Filename has been reset");
        return -1;
    }

    *VideoInfo = 0;

    ReturnValue = AppLibFormat_GetMediaInfo(RequestedFilename, &MediaInfo);
    if (ReturnValue != 0) {
        NETFIFO_ERR("[AppLib - NetFifo] <PlaybackGetVideoInfo> AppLibFormat_GetMediaInfo() fail");
        return -1;
    }

    for (i=0;i<MediaInfo.MediaInfo.Movie->TrackCount;i++) {
        if (MediaInfo.MediaInfo.Movie->Track[i].TrackType == AMP_MEDIA_TRACK_TYPE_VIDEO) {
            break;
        }
    }

    if (i >= MediaInfo.MediaInfo.Movie->TrackCount) {
        NETFIFO_ERR("[AppLib - NetFifo] <PlaybackGetVideoInfo> No video track found");
        return -1;
    } else {
        DTS = MediaInfo.MediaInfo.Movie->Track[i].NextDTS;
        TimeScale = MediaInfo.MediaInfo.Movie->Track[i].TimeScale;
        TimePerFrame = MediaInfo.MediaInfo.Movie->Track[i].TimePerFrame;

        if (InfoType == AMP_NETFIFO_PLAYBACK_GET_VID_FTIME) {
            /* Time per video frame in ms */
            *VideoInfo = (1000 * TimePerFrame)/TimeScale;
        } else if (InfoType == AMP_NETFIFO_PLAYBACK_GET_VID_TICK) {
            /* Tick per video frame (need to be in 90K Base) */
            *VideoInfo = TimePerFrame * ((double)90000/(double)TimeScale);
        } else if (InfoType == AMP_NETFIFO_PLAYBACK_GET_DURATION) {
            /* File duration in ms. */
            Duration = (((UINT64)(DTS) / (TimeScale)) * 1000 + (((UINT64)(DTS) % (TimeScale)) * 1000) /(TimeScale)); //unit msec
            *VideoInfo = (int) Duration;
        } else {
            NETFIFO_ERR("[AppLib - NetFifo] <PlaybackGetVideoInfo> unknown type (0x%x)", InfoType);
            return -1;
        }
    }

    return 0;
}

static void PlaybackTask(UINT32 Info)
{
    if (gNetFifoPBMgr.NetPlaybackEntry != NULL) {
        gNetFifoPBMgr.NetPlaybackEntry();
    } else {
        NETFIFO_ERR("[AppLib - NetFifo] <PlaybackTask> NetPlaybackEntry is not registered");
    }
}
static int PlaybackInit(void)
{
    int ReturnValue = 0;

    if (gNetFifoPBMgr.IsInit == 1) {
        NETFIFO_DBG("[AppLib - NetFifo] <NetPlaybackInit> already inited.");
        return 0;
    }

    /* Create net fifo playback message queue */
    ReturnValue = AmbaKAL_MsgQueueCreate(&gNetFifoPBMgr.MsgQueue, gNetFifoPBMgr.MsgPool, sizeof(APP_NET_PB_MESSAGE_s), NETFIFO_PB_MGR_MSGQUEUE_SIZE);
    if (ReturnValue != OK) {
        NETFIFO_ERR("[AppLib - NetFifo] <PlaybackInit> Create Queue fail. (error code: %d)",ReturnValue);
        return ReturnValue;
    }

    /* Create net fifo playback task */
    ReturnValue = AmbaKAL_TaskCreate(&gNetFifoPBMgr.Task, /* pTask */
        NETFIFO_PB_MGR_NAME, /* pTaskName */
        APPLIB_NET_PLAYER_TASK_PRIORITY,/* Priority */
        PlaybackTask, /* void (*EntryFunction)(UINT32) */
        0x0, /* EntryArg */
        (void *) gNetFifoPBMgr.Stack, /* pStackBase */
        NETFIFO_PB_MGR_STACK_SIZE, /* StackByteSize */
        AMBA_KAL_AUTO_START); /* Do NOT Start */

    if (ReturnValue != OK) {
        NETFIFO_ERR("[AppLib - NetFifo] <PlaybackInit> Create '%s; task fail (error code: %d)",NETFIFO_PB_MGR_NAME, ReturnValue);
        return ReturnValue;
    }

    gNetFifoPBMgr.IsInit = 1;
    NETFIFO_DBG("[AppLib - NetFifo] <NetPlaybackInit> init success.");

    return 0;
}

#if 0
static int PlaybackUnInit(void)
{
    int ErrorCode = 0;
    int ReturnValue = 0;

    /* Return value of AmbaKAL_TaskTerminate() and AmbaKAL_TaskDelete()
            TX_THREAD_ERROR: The task is not created.
            TX_DELETE_ERROR: The task is not terminated.
      */

    ReturnValue = AmbaKAL_TaskTerminate(&gNetFifoPBMgr.Task);
    if ((ReturnValue != AMP_OK) && (ReturnValue != TX_THREAD_ERROR)) {
        NETFIFO_ERR("[AppLib - NetFifo] <PlaybackUnInit> Failed to terminate task (%d).", ReturnValue);
        ErrorCode = -1;
    }

    ReturnValue = AmbaKAL_TaskDelete(&gNetFifoPBMgr.Task);
    if ((ReturnValue != AMP_OK) && (ReturnValue != TX_THREAD_ERROR) && (ReturnValue != TX_DELETE_ERROR)) {
        NETFIFO_ERR("[AppLib - NetFifo] <PlaybackUnInit> Failed to delete task (%d).", ReturnValue);
        ErrorCode = -1;
    }

    if ((&gNetFifoPBMgr.MsgQueue) != NULL) {
        ReturnValue = AmbaKAL_MsgQueueDelete(&gNetFifoPBMgr.MsgQueue);
        if (ReturnValue != OK) {
            NETFIFO_ERR("[AppLib - NetFifo] <PlaybackUnInit> Failed to delete message queue (%d).", ReturnValue);
            ErrorCode = -1;
        }
    }

    gNetFifoPBMgr.IsInit = 0;

    return ReturnValue;
}
#endif

//--------------------------------------------------------------------------------------------------
// net streaming
//--------------------------------------------------------------------------------------------------
static APPLIB_VIDEOENC_STREAM_INFO_s* GetStreamInfo(int StreamId)
{
    int i;

    if (gStreamList.StreamCount == 0) {
        NETFIFO_ERR("[AppLib - NetFifo] <GetStreamInfo> No valid stream exist");
        return NULL;
    }

    for (i=0;i<gStreamList.StreamCount;i++) {
        if (gStreamList.StreamList[i].Id == StreamId) {
            return (&(gStreamList.StreamList[i]));
        }
    }

    NETFIFO_ERR("[AppLib - NetFifo] <GetStreamInfo> No matched stream found");
    return NULL;
}


/**
 * Return Stream ID list.
 */
static int GetStreamIDList(AMP_NETFIFO_MEDIA_STREAMID_LIST_s *slist)
{
    int Amount = 0;
    int ReturnValue = OK;

    if (slist == NULL) {
        NETFIFO_ERR("[AppLib - NetFifo] <GetStreamIDList> invalid param ('slist' is NULL)");
        return NG;
    }

    if (gState != AMP_NETFIFO_NOTIFY_STARTENC) {
        NETFIFO_DBG("[AppLib - NetFifo] <GetStreamIDList> no avalible streams.");
        slist->Amount = 0;
        return OK;
    }

    memset(&gStreamList, 0, sizeof(APPLIB_VIDEOENC_STREAM_LIST_s));
    ReturnValue = AppLibVideoEnc_GetValidStream(&gStreamList);
    if (ReturnValue != OK) {
        return NG;
    }

    slist->Amount = gStreamList.StreamCount;
    for (Amount=0;Amount<gStreamList.StreamCount;Amount++) {
        slist->StreamID_List[Amount] = gStreamList.StreamList[Amount].Id;
    }

    #if defined(NETFIFO_DBG_EN)
    {
        int i;
        NETFIFO_DBG("[AppLib - NetFifo] <GetStreamIDList> Amount: %d", slist->Amount);
        for (i=0;i<slist->Amount;i++) {
            NETFIFO_DBG("[AppLib - NetFifo] <GetStreamIDList> StreamID_List[%d]: %d", i, slist->StreamID_List[i]);
        }
    }
    #endif

    return OK;
}

static int SetupMediaInfo(int StreamID, AMP_NETFIFO_MOVIE_INFO_CFG_s *media_info)
{
    UINT8 *BitsBufAddr = NULL;
    UINT8 *DescBufAddr = NULL;
    UINT32 BitsBufSize = 0;
    UINT32 DescBufSize = 0;
    APPLIB_VIDEOENC_STREAM_INFO_s *pStreamInfo = NULL;
    APPLIB_SENSOR_VIDEO_ENC_CONFIG_s *VideoEncConfigData = {0};
    APPLIB_VIDEOENC_GOP_s *VideoEncGOPData = {0};

    pStreamInfo = GetStreamInfo(StreamID);
    if (pStreamInfo == NULL) {
        return NG;
    }

    AppLibRecorderMemMgr_GetBufAddr(&BitsBufAddr, &DescBufAddr);
    AppLibRecorderMemMgr_GetBufSize(&BitsBufSize, &DescBufSize);
    if ((BitsBufAddr == NULL) || (BitsBufSize == 0)) {
        NETFIFO_ERR("[AppLib - NetFifo] <SetupMediaInfo> Get Bit stream buffer addr: 0x%08X, size: %d",(UINT32)BitsBufAddr, BitsBufSize);
        return NG;
    }

    if (StreamID == AMP_VIDEOENC_STREAM_PRIMARY) {
        VideoEncConfigData = AppLibSysSensor_GetVideoConfig(AppLibVideoEnc_GetSensorVideoRes());
        if (VideoEncConfigData == NULL) {
            NETFIFO_ERR("[AppLib - NetFifo] <SetupMediaInfo> Get video Config fail");
            return NG;
        }

        VideoEncGOPData = AppLibSysSensor_GetVideoGOP(AppLibVideoEnc_GetSensorVideoRes());
        if (VideoEncGOPData == NULL) {
            NETFIFO_ERR("[AppLib - NetFifo] <SetupMediaInfo> Get video GOP fail");
            return NG;
        }

        media_info->nTrack = 1;
        media_info->Track[0].nMediaId = AMP_FORMAT_MID_AVC;
        media_info->Track[0].nTimeScale = VideoEncConfigData->EncNumerator;
        media_info->Track[0].nTimePerFrame = VideoEncConfigData->EncDenominator;
        media_info->Track[0].hCodec = pStreamInfo->HdlVideoEnc;
        media_info->Track[0].pBufferBase = BitsBufAddr;
        media_info->Track[0].pBufferLimit = BitsBufAddr + BitsBufSize;
        media_info->Track[0].nTrackType = AMP_NETFIFO_MEDIA_TRACK_TYPE_VIDEO;

        media_info->Track[0].Info.Video.bDefault = TRUE;
        media_info->Track[0].Info.Video.nMode = AMP_VIDEO_MODE_P;
        media_info->Track[0].Info.Video.nM = VideoEncGOPData->M;
        media_info->Track[0].Info.Video.nN = VideoEncGOPData->N;
        media_info->Track[0].Info.Video.nGOPSize = VideoEncGOPData->Idr;

        media_info->Track[0].Info.Video.nCodecTimeScale = VideoEncConfigData->EncNumerator;
        media_info->Track[0].Info.Video.nWidth = VideoEncConfigData->EncodeWidth;
        media_info->Track[0].Info.Video.nHeight = VideoEncConfigData->EncodeHeight;
        media_info->Track[0].Info.Video.nTrickRecNum = 1;
        media_info->Track[0].Info.Video.nTrickRecDen = 1;

        if ((AppLibVideoEnc_GetRecMode() == REC_MODE_AV) &&
            (AppLibNetFifo_GetStreamMode() == NET_STREAM_MODE_AV)) {
            media_info->nTrack++;
            if (AppLibAudioEnc_GetEncType() == AUDIO_TYPE_AAC) {
                media_info->Track[1].nMediaId = AMP_FORMAT_MID_AAC;
            } else if (AppLibAudioEnc_GetEncType() == AUDIO_TYPE_PCM) {
                 media_info->Track[1].nMediaId = AMP_FORMAT_MID_PCM;
            }

            media_info->Track[1].nTimeScale = media_info->Track[0].nTimeScale;
            media_info->Track[1].hCodec = pStreamInfo->HdlAudioEnc;
            media_info->Track[1].pBufferBase = AudPriBitsBuf;
            media_info->Track[1].pBufferLimit = AudPriBitsBuf + AUDENC_VIDENC_BITSFIFO_SIZE;
            media_info->Track[1].nTrackType = AMP_NETFIFO_MEDIA_TRACK_TYPE_AUDIO;

            media_info->Track[1].Info.Audio.nSampleRate = AppLibAudioEnc_GetSrcSampleRate();
            media_info->Track[1].Info.Audio.bDefault = TRUE;
            media_info->Track[1].Info.Audio.nChannels = AppLibAudioEnc_GetSrcChanMode();
            media_info->Track[1].nTimePerFrame = ((UINT64)media_info->Track[1].nTimeScale * 1024 / media_info->Track[1].Info.Audio.nSampleRate);
        }
    } else if (StreamID == AMP_VIDEOENC_STREAM_SECONDARY) {
        media_info->nTrack = 1;
        media_info->Track[0].nMediaId = AMP_FORMAT_MID_AVC;
        media_info->Track[0].nTimeScale = AppLibVideoEnc_GetSecStreamTimeScale();
        media_info->Track[0].nTimePerFrame = AppLibVideoEnc_GetSecStreamTick();
        media_info->Track[0].hCodec = pStreamInfo->HdlVideoEnc;
        media_info->Track[0].pBufferBase = BitsBufAddr;
        media_info->Track[0].pBufferLimit = BitsBufAddr + BitsBufSize;
        media_info->Track[0].nTrackType = AMP_NETFIFO_MEDIA_TRACK_TYPE_VIDEO;

        media_info->Track[0].Info.Video.bDefault = TRUE;
        media_info->Track[0].Info.Video.nMode = AMP_VIDEO_MODE_P;
        media_info->Track[0].Info.Video.nM = AppLibVideoEnc_GetSecStreamGopM();
        media_info->Track[0].Info.Video.nN = AppLibVideoEnc_GetSecStreamGopN();
        media_info->Track[0].Info.Video.nGOPSize = AppLibVideoEnc_GetSecStreamGopIDR();
        media_info->Track[0].Info.Video.nCodecTimeScale = media_info->Track[0].nTimeScale;
        media_info->Track[0].Info.Video.nWidth = AppLibVideoEnc_GetSecStreamW();
        media_info->Track[0].Info.Video.nHeight = AppLibVideoEnc_GetSecStreamH();
        media_info->Track[0].Info.Video.nTrickRecNum = 1;
        media_info->Track[0].Info.Video.nTrickRecDen = 1;

        if ((AppLibVideoEnc_GetRecMode() == REC_MODE_AV) &&
            (AppLibNetFifo_GetStreamMode() == NET_STREAM_MODE_AV)) {
            media_info->nTrack++;
            if (AppLibAudioEnc_GetEncType() == AUDIO_TYPE_AAC) {
                media_info->Track[1].nMediaId = AMP_FORMAT_MID_AAC;
            } else if (AppLibAudioEnc_GetEncType() == AUDIO_TYPE_PCM) {
                media_info->Track[1].nMediaId = AMP_FORMAT_MID_PCM;
            }

            media_info->Track[1].nTimeScale = media_info->Track[0].nTimeScale;
            media_info->Track[1].hCodec = pStreamInfo->HdlAudioEnc;
            media_info->Track[1].pBufferBase = AudSecBitsBuf;
            media_info->Track[1].pBufferLimit = AudSecBitsBuf + AUDENC_VIDENC_BITSFIFO_SIZE;
            media_info->Track[1].nTrackType = AMP_NETFIFO_MEDIA_TRACK_TYPE_AUDIO;

            media_info->Track[1].Info.Audio.nSampleRate = AppLibAudioEnc_GetSrcSampleRate();
            media_info->Track[1].Info.Audio.bDefault = TRUE;
            media_info->Track[1].Info.Audio.nChannels = AppLibAudioEnc_GetSrcChanMode();

            media_info->Track[1].nTimePerFrame = ((UINT64)media_info->Track[1].nTimeScale * 1024 / media_info->Track[1].Info.Audio.nSampleRate);
        }
    } else {
        NETFIFO_ERR("[AppLib - NetFifo] <SetupMediaInfo> unexpected stream id: 0x%08x", StreamID);
        return NG;
    }

    NETFIFO_DBG("[Applib - NetFifo] <SetupMediaInfo> media_info->nTrack =  %d", media_info->nTrack);
    #if 0
    NETFIFO_DBG("[Applib - NetFifo] <SetupMediaInfo> Stream info of ID 0x%08x", StreamID);
    NETFIFO_DBG("[Applib - NetFifo] <SetupMediaInfo> media_info->Track[0].TimeScale = %d",media_info->Track[0].nTimeScale);
    NETFIFO_DBG("[Applib - NetFifo] <SetupMediaInfo> media_info->Track[0].TimePerFrame = %d",media_info->Track[0].nTimePerFrame);
    NETFIFO_DBG("[Applib - NetFifo] <SetupMediaInfo> media_info->Track[0].Info.Video.M = %d",media_info->Track[0].Info.Video.nM);
    NETFIFO_DBG("[Applib - NetFifo] <SetupMediaInfo> media_info->Track[0].Info.Video.N = %d",media_info->Track[0].Info.Video.nN);
    NETFIFO_DBG("[Applib - NetFifo] <SetupMediaInfo> media_info->Track[0].Info.Video.GOPSize = %d",media_info->Track[0].Info.Video.nGOPSize);
    NETFIFO_DBG("[Applib - NetFifo] <SetupMediaInfo> media_info->Track[0].Info.Video.Width = %d",media_info->Track[0].Info.Video.nWidth);
    NETFIFO_DBG("[Applib - NetFifo] <SetupMediaInfo> media_info->Track[0].Info.Video.Height = %d",media_info->Track[0].Info.Video.nHeight);
    if (media_info->nTrack == 2) {
        NETFIFO_DBG("[Applib - NetFifo] <SetupMediaInfo> media_info->Track[1].nMediaId = %d",media_info->Track[1].nMediaId );
        NETFIFO_DBG("[Applib - NetFifo] <SetupMediaInfo> media_info->Track[1].TimeScale = %d",media_info->Track[1].nTimeScale);
        NETFIFO_DBG("[Applib - NetFifo] <SetupMediaInfo> media_info->Track[1].TimePerFrame = %d",media_info->Track[1].nTimePerFrame);
        NETFIFO_DBG("[Applib - NetFifo] <SetupMediaInfo> media_info->Track[1].Info.Audio.nSampleRate = %d",media_info->Track[1].Info.Audio.nSampleRate);
        NETFIFO_DBG("[Applib - NetFifo] <SetupMediaInfo> media_info->Track[1].Info..Audio.nChannels = %d",media_info->Track[1].Info.Audio.nChannels);
    }
    #endif

    return OK;
}

static int SetupPbMediaInfo(char *filename, AMP_NETFIFO_MOVIE_INFO_CFG_s *media_info)
{
    UINT8 *BitsBufAddr = NULL;
    UINT32 BitsBufSize = 0;
    UINT8 *AudioBufAddr = NULL;
    UINT32 AudioBufSize = 0;
    void *VideoCodec = NULL;
    void *AudioCodec = NULL;
    APPLIB_MEDIA_INFO_s MediaInfo = {0};
    AMP_VIDEO_TRACK_INFO_s *pstVideoInfo = NULL;
    AMP_AUDIO_TRACK_INFO_s *pstAudioInfo = NULL;
    int i = 0;
    int VideoTrackIndex = -1;
    int AudioTrackIndex = -1;
    AMP_MEDIA_TRACK_INFO_s *pTrack = NULL;
    APPLIB_TRANSCODE_DECODE_MODE_e DecodeMode = TRANSCODE_DECODE_MODE_AV;
    int ReturnValue = 0;

    DecodeMode = AppLibTranscoderVideoDec_GetDecodeMode();

    AppLibTranscoderVideoDec_GetDstCodec(&VideoCodec, TRANSCODE_MEDIA_TYPE_VIDEO);
    if (VideoCodec == NULL) {
        NETFIFO_ERR("[AppLib - NetFifo] <SetupPbMediaInfo> VideoCodec is NULL");
        return NG;
    }

    ReturnValue = AppLibTranscoderVideoDec_GetBufferInfo(&BitsBufAddr, &BitsBufSize, TRANSCODE_MEDIA_TYPE_VIDEO);
    if (ReturnValue != 0) {
        NETFIFO_ERR("[AppLib - NetFifo] <SetupPbMediaInfo> Get Bit stream buffer info fail");
        return NG;
    }

    if (DecodeMode == TRANSCODE_DECODE_MODE_AV) {
        AppLibTranscoderVideoDec_GetDstCodec(&AudioCodec, TRANSCODE_MEDIA_TYPE_AUDIO);
        if (AudioCodec == NULL) {
            NETFIFO_ERR("[AppLib - NetFifo] <SetupPbMediaInfo> AudioCodec is NULL");
            return NG;
        }

        ReturnValue = AppLibTranscoderVideoDec_GetBufferInfo(&AudioBufAddr, &AudioBufSize, TRANSCODE_MEDIA_TYPE_AUDIO);
        if (ReturnValue != 0) {
            NETFIFO_ERR("[AppLib - NetFifo] <SetupPbMediaInfo> Get Bit stream buffer info fail");
            return NG;
        }
    }


    ReturnValue = AppLibFormat_GetMediaInfo(filename, &MediaInfo);
    if (ReturnValue != 0) {
        NETFIFO_ERR("[AppLib - NetFifo] <SetupPbMediaInfo> AppLibFormat_GetMediaInfo() fail");
        return NG;
    }

    #if 0 // debug
    {
        int i = 0;
        AMP_MEDIA_TRACK_INFO_s *pTrack;
        NETFIFO_DBG("[Applib - NetFifo] <SetupPbMediaInfo> Movie->TrackCount: %d", MediaInfo.MediaInfo.Movie->TrackCount);
        for (i=0;i<MediaInfo.MediaInfo.Movie->TrackCount;i++) {
            pTrack = &(MediaInfo.MediaInfo.Movie->Track[i]);
            NETFIFO_DBG("[Applib - NetFifo] <SetupPbMediaInfo> Track[%d]:", i);
            NETFIFO_DBG("[Applib - NetFifo] <SetupPbMediaInfo> pTrack->TrackType = %d", pTrack->TrackType);
            NETFIFO_DBG("[Applib - NetFifo] <SetupPbMediaInfo> pTrack->MediaId = %d\n", pTrack->MediaId);
        }
    }
    #endif

    //NETFIFO_DBG("[Applib - NetFifo] <SetupPbMediaInfo> Movie->TrackCount: %d", MediaInfo.MediaInfo.Movie->TrackCount);
    for (i=0;i<MediaInfo.MediaInfo.Movie->TrackCount;i++) {
        pTrack = &(MediaInfo.MediaInfo.Movie->Track[i]);
        if (pTrack->TrackType == AMP_MEDIA_TRACK_TYPE_VIDEO) {
            VideoTrackIndex = i;
            pstVideoInfo = &(pTrack->Info.Video);
        } else if (pTrack->TrackType == AMP_MEDIA_TRACK_TYPE_AUDIO) {
            if (DecodeMode == TRANSCODE_DECODE_MODE_AV) {
                AudioTrackIndex = i;
                pstAudioInfo = &(pTrack->Info.Audio);
            } else {
                AmbaPrint("[Applib - NetFifo] <SetupPbMediaInfo> skip audio track");
            }
        } else {
            ;
        }
    }

    i = 0;
    media_info->nTrack = 0;
    if ((VideoTrackIndex != -1) && VideoCodec) {
        media_info->nTrack++;
        media_info->Track[i].nMediaId = MediaInfo.MediaInfo.Movie->Track[VideoTrackIndex].MediaId;
        media_info->Track[i].nTimeScale = MediaInfo.MediaInfo.Movie->Track[VideoTrackIndex].TimeScale;
        media_info->Track[i].nTimePerFrame = MediaInfo.MediaInfo.Movie->Track[VideoTrackIndex].TimePerFrame;
        media_info->Track[i].hCodec = VideoCodec;
        media_info->Track[i].pBufferBase = BitsBufAddr;
        media_info->Track[i].pBufferLimit = BitsBufAddr + BitsBufSize;
        media_info->Track[i].nTrackType = AMP_NETFIFO_MEDIA_TRACK_TYPE_VIDEO;

        media_info->Track[i].Info.Video.bDefault = TRUE;
        media_info->Track[i].Info.Video.nMode = pstVideoInfo->Mode; //AMP_VIDEO_MODE_P;
        media_info->Track[i].Info.Video.nM = pstVideoInfo->M; //VideoEncGOPData->M;
        media_info->Track[i].Info.Video.nN = pstVideoInfo->N; //VideoEncGOPData->N;
        media_info->Track[i].Info.Video.nGOPSize = pstVideoInfo->GOPSize; //VideoEncGOPData->Idr;

        media_info->Track[i].Info.Video.nCodecTimeScale = MediaInfo.MediaInfo.Movie->Track[0].TimeScale;
        media_info->Track[i].Info.Video.nWidth = pstVideoInfo->Width;
        media_info->Track[i].Info.Video.nHeight = pstVideoInfo->Height;
        media_info->Track[i].Info.Video.nTrickRecNum = 1;
        media_info->Track[i].Info.Video.nTrickRecDen = 1;
        i++;
    }

    #if 0
    if ((AudioTrackIndex != -1) && (AudioCodec)
        && (AppLibNetFifo_GetStreamMode() == NET_STREAM_MODE_AV)) {
    #else
    if ((AudioTrackIndex != -1) && (AudioCodec)) {
    #endif
        media_info->nTrack++;
        media_info->Track[i].nMediaId = MediaInfo.MediaInfo.Movie->Track[AudioTrackIndex].MediaId;
        media_info->Track[i].nTimeScale = MediaInfo.MediaInfo.Movie->Track[VideoTrackIndex].TimeScale; //media_info->Track[0].nTimeScale; // same as video?
        media_info->Track[i].hCodec = AudioCodec;
        media_info->Track[i].pBufferBase = AudioBufAddr;
        media_info->Track[i].pBufferLimit = AudioBufAddr + AudioBufSize;
        media_info->Track[i].nTrackType = AMP_NETFIFO_MEDIA_TRACK_TYPE_AUDIO;

        media_info->Track[i].Info.Audio.nSampleRate = pstAudioInfo->SampleRate; //AppLibAudioEnc_GetSrcSampleRate();
        media_info->Track[i].Info.Audio.bDefault = TRUE;
        media_info->Track[i].Info.Audio.nChannels = pstAudioInfo->Channels; //AppLibAudioEnc_GetSrcChanMode();
        media_info->Track[i].nTimePerFrame = ((UINT64)media_info->Track[i].nTimeScale * 1024 / media_info->Track[i].Info.Audio.nSampleRate);
        i++;
    }


    NETFIFO_DBG("[Applib - NetFifo] <SetupPbMediaInfo> media_info->nTrack =  %d", media_info->nTrack);
    #if 1
    {
        int j = 0;

        for(j=0;j<media_info->nTrack;j++) {
            NETFIFO_DBG("[Applib - NetFifo] <SetupPbMediaInfo> media_info->Track[%d].nMediaId = %d",j,media_info->Track[j].nMediaId);
            NETFIFO_DBG("[Applib - NetFifo] <SetupPbMediaInfo> media_info->Track[%d].nTrackType = %d",j,media_info->Track[j].nTrackType);
            NETFIFO_DBG("[Applib - NetFifo] <SetupPbMediaInfo> media_info->Track[%d].hCodec = 0x%08x",j,media_info->Track[j].hCodec);
            if (media_info->Track[j].nTrackType == AMP_NETFIFO_MEDIA_TRACK_TYPE_VIDEO) {
                NETFIFO_DBG("[Applib - NetFifo] <SetupPbMediaInfo> media_info->Track[%d].TimeScale = %d",j,media_info->Track[j].nTimeScale);
                NETFIFO_DBG("[Applib - NetFifo] <SetupPbMediaInfo> media_info->Track[%d].TimePerFrame = %d",j,media_info->Track[j].nTimePerFrame);
                NETFIFO_DBG("[Applib - NetFifo] <SetupPbMediaInfo> media_info->Track[%d].Info.Video.M = %d",j,media_info->Track[j].Info.Video.nM);
                NETFIFO_DBG("[Applib - NetFifo] <SetupPbMediaInfo> media_info->Track[%d].Info.Video.N = %d",j,media_info->Track[j].Info.Video.nN);
                NETFIFO_DBG("[Applib - NetFifo] <SetupPbMediaInfo> media_info->Track[%d].Info.Video.GOPSize = %d",j,media_info->Track[j].Info.Video.nGOPSize);
                NETFIFO_DBG("[Applib - NetFifo] <SetupPbMediaInfo> media_info->Track[%d].Info.Video.Width = %d",j,media_info->Track[j].Info.Video.nWidth);
                NETFIFO_DBG("[Applib - NetFifo] <SetupPbMediaInfo> media_info->Track[%d].Info.Video.Height = %d",j,media_info->Track[j].Info.Video.nHeight);
            } else {
                NETFIFO_DBG("[Applib - NetFifo] <SetupPbMediaInfo> media_info->Track[%d].TimeScale = %d",j,media_info->Track[j].nTimeScale);
                NETFIFO_DBG("[Applib - NetFifo] <SetupPbMediaInfo> media_info->Track[%d].TimePerFrame = %d",j,media_info->Track[j].nTimePerFrame);
                NETFIFO_DBG("[Applib - NetFifo] <SetupPbMediaInfo> media_info->Track[%d].Info.Audio.nSampleRate = %d",j,media_info->Track[j].Info.Audio.nSampleRate);
                NETFIFO_DBG("[Applib - NetFifo] <SetupPbMediaInfo> media_info->Track[%d].Info..Audio.nChannels = %d",j,media_info->Track[j].Info.Audio.nChannels);
            }
        }
    }
    #endif

    return OK;
}


/**
 * Return Media Ifno for dedicated Stream ID.
 */
static int GetMediaInfo(int StreamID, AMP_NETFIFO_MOVIE_INFO_CFG_s *media_info)
{
    UINT32 ReturnValue = 0;

    NETFIFO_DBG("[AppLib - NetFifo] <GetMediaInfo> request MediaInfo for Stream 0x%08x",StreamID);

    if (media_info == NULL) {
        NETFIFO_ERR("[AppLib - NetFifo] <GetMediaInfo> invalid param.(media_info is NULL)");
        return NG;
    }

    ReturnValue = AmpNetFifo_GetDefaultMovieInfoCfg(media_info);
    if (ReturnValue != OK) {
        NETFIFO_ERR("[AppLib - NetFifo] <GetMediaInfo> Get default movie info cfg fail");
        return NG;
    }

    if (StreamID == NETFIFO_PB_MEDIA_STREAM_ID) {
        ReturnValue = SetupPbMediaInfo(RequestedFilename, media_info);
    } else {
        ReturnValue = SetupMediaInfo(StreamID, media_info);
    }

    if (ReturnValue != OK) {
        return NG;
    }

    return OK;
}

static int GetSPSAndPPS(char* Filename, char* ResParam)
{
    APPLIB_MEDIA_INFO_s MediaInfo = {0};
    char QueryFilename[APPLIB_NETFIFO_MAX_FN_SIZE] = {0};
    int ReturnValue = NG;
    int i=0;
    APPLIB_NETFIFO_SPS_PPS_s *SpsPps = (APPLIB_NETFIFO_SPS_PPS_s *) ResParam;

    if(strlen(Filename) == 0){ // if fn='\0', use current playback file
        snprintf(QueryFilename, sizeof(QueryFilename), "%s", RequestedFilename);
    } else {
        ReturnValue = ConvertFilePathPrefix(Filename, QueryFilename, sizeof(QueryFilename));
        if (ReturnValue != 0) {
            return NG;
        }
    }

    ReturnValue = AppLibFormat_GetMediaInfo(QueryFilename, &MediaInfo);
    if (ReturnValue != 0) {
        NETFIFO_ERR("[AppLib - NetFifo] <GetSPSAndPPS> AppLibFormat_GetMediaInfo() fail");
        return NG;
    }

    for (i=0;i<MediaInfo.MediaInfo.Movie->TrackCount;i++) { // find Video track
        if (MediaInfo.MediaInfo.Movie->Track[i].TrackType == AMP_MEDIA_TRACK_TYPE_VIDEO) {
            SpsPps->SpsLen = (unsigned int) MediaInfo.MediaInfo.Movie->PrivInfo.Iso.TrackInfo[i].Info.Video.SPSLen;
            memcpy(SpsPps->Sps, MediaInfo.MediaInfo.Movie->PrivInfo.Iso.TrackInfo[i].Info.Video.SPS, SpsPps->SpsLen);
            SpsPps->PpsLen = (unsigned int) MediaInfo.MediaInfo.Movie->PrivInfo.Iso.TrackInfo[i].Info.Video.PPSLen;
            memcpy(SpsPps->Pps, MediaInfo.MediaInfo.Movie->PrivInfo.Iso.TrackInfo[i].Info.Video.PPS, SpsPps->PpsLen);
        }
    }

    return OK;
}


#define NET_BITRATE_CONTROL_INCREMENT_STEP 10000
static int RRStatStart = 0;
/**
 * Control bitrate according to the frame lost report form linux.
 * only use in this file, maintain local
 */
static int BitrateControl(unsigned int FractionLost, unsigned int Jitter, double PGDelay)
{
    UINT32 ReturnValue = 0;
    UINT32 NextBitrate = 0;
    static int LastBitRateUpdateTime = 0;
    static int CurTime = 0;
    AMBA_RTC_TIME_SPEC_u TimeSpec = {0};

    NETFIFO_DBG("[AppLib - NetFifo] <BitrateControl> Start calculate bitrate according to report");
    if (RRStatStart == 0) {
        /*skip first rr_stat report*/
        return 0;
    }
    if (gNetBitrateStat.Inited == 0) {
        gNetBitrateStat.Inited = 1;
        if (gStreamList.ActiveStreamID == AMP_VIDEOENC_STREAM_PRIMARY) {
            APPLIB_VIDEOENC_BITRATE_s *VideoEncBitRateData = AppLibSysSensor_GetVideoBitRate(AppLibVideoEnc_GetSensorVideoRes(), AppLibVideoEnc_GetQuality());
            gNetBitrateStat.MaxBitRate = VideoEncBitRateData->BitRateAvg*1000*1000;
        } else {
            gNetBitrateStat.MaxBitRate = AppLibVideoEnc_GetSecStreamBitRate();
        }
        gNetBitrateStat.NetBandwidth = gNetBitrateStat.MaxBitRate;
        gNetBitrateStat.MaxStableBR= gNetBitrateStat.MaxBitRate;
    }

    gNetBitrateStat.CurBitRate = AppLibvideoEnc_GetCurAvgBitrate(gStreamList.ActiveStreamID);

    ReturnValue = AmbaRTC_GetSystemTime(AMBA_TIME_STD_TAI, &TimeSpec);
    CurTime = TimeSpec.Calendar.Hour*3600+TimeSpec.Calendar.Minute*60+TimeSpec.Calendar.Second;



    /* highest priority rule: if fr_lost > 50%, decrease the bitrate accordingly
     * The fraction of bitrate we decrease is empirical result which comes from testing with NetLimiter
     */
    if(FractionLost > 192){ // > 75%
        NextBitrate = gNetBitrateStat.CurBitRate >> 2;
        gNetBitrateStat.ZeroLost= 0;
        AmbaPrint("[AppLib - NetFifo] <BitrateControl> fraction lost over 75%, set bitrate=%d\n", NextBitrate);
    }else if(FractionLost > 180){ // > 70%
        NextBitrate = gNetBitrateStat.CurBitRate >> 1;
        gNetBitrateStat.ZeroLost = 0;
        AmbaPrint("[AppLib - NetFifo] <BitrateControl> fraction lost over 70%, set bitrate=%d\n", NextBitrate);
    }else if(FractionLost > 160){ // > 62%
        NextBitrate = gNetBitrateStat.CurBitRate*7/10;
        gNetBitrateStat.ZeroLost = 0;
        AmbaPrint("[AppLib - NetFifo] <BitrateControl> fraction lost over 62%, set bitrate=%d\n", NextBitrate);
    }else if(FractionLost > 128){ // > 50%
        NextBitrate = gNetBitrateStat.CurBitRate*17/20;
        gNetBitrateStat.ZeroLost = 0;
        AmbaPrint("[AppLib - NetFifo] <BitrateControl> fraction lost over 50%, set bitrate=%d\n", NextBitrate);
    }else{
        if (gNetBitrateStat.ZeroLost != 0 && FractionLost > 10) {
            /**Skip first frame lost != 0 to avoid sudden signal lost*/
            gNetBitrateStat.ZeroLost = 0;
            AmbaPrint("[AppLib - NetFifo] <BitrateControl> FractionLost=%d, reset lost_count\n", FractionLost);
            return OK;
        }
        if(gNetBitrateStat.InMiddleOfIncrement == 1){
            /* if fr_lost is caused by increment of bitrate, we set maxStableBR */
            if(FractionLost > 64 && FractionLost > gNetBitrateStat.LastFrameLost){ // > 25%
                gNetBitrateStat.MaxStableBR = gNetBitrateStat.CurBitRate - 2*NET_BITRATE_CONTROL_INCREMENT_STEP;
                NextBitrate = gNetBitrateStat.MaxStableBR - NET_BITRATE_CONTROL_INCREMENT_STEP;
                gNetBitrateStat.ZeroLost = 0;
                AmbaPrint("[AppLib - NetFifo] <BitrateControl> increment to %d but fr_lost=%d, set max_stable_brate =%d and reset lost_count\n", gNetBitrateStat.CurBitRate, FractionLost, gNetBitrateStat.MaxStableBR);
            }else if(FractionLost > 25 && FractionLost > gNetBitrateStat.LastFrameLost){ // > 10 %
                gNetBitrateStat.MaxStableBR = gNetBitrateStat.CurBitRate - 2*NET_BITRATE_CONTROL_INCREMENT_STEP;
                NextBitrate = gNetBitrateStat.MaxStableBR - NET_BITRATE_CONTROL_INCREMENT_STEP;
                gNetBitrateStat.ZeroLost = 0;
                AmbaPrint("[AppLib - NetFifo] <BitrateControl> increment to %d but fr_lost=%d, set max_stable_brate =%d and reset lost_count\n", gNetBitrateStat.CurBitRate, FractionLost, gNetBitrateStat.MaxStableBR);
            }else if(FractionLost > 10){//increment result in fr_lost
                gNetBitrateStat.MaxStableBR = gNetBitrateStat.CurBitRate - NET_BITRATE_CONTROL_INCREMENT_STEP;
                gNetBitrateStat.ZeroLost = 0;
                AmbaPrint("[AppLib - NetFifo] <BitrateControl> increment to %d but fr_lost=%d, set max_stable_brate =%d and reset lost_count\n", gNetBitrateStat.CurBitRate, FractionLost, gNetBitrateStat.MaxStableBR);
            }else{
                gNetBitrateStat.ZeroLost++;
                AmbaPrint("[AppLib - NetFifo] <BitrateControl> increment to %d and fr_lost=0\n", gNetBitrateStat.CurBitRate);
            }
            gNetBitrateStat.InMiddleOfIncrement = 0;
        } else {
            if(FractionLost > 64 && FractionLost > gNetBitrateStat.LastFrameLost){ // > 25%
                NextBitrate = gNetBitrateStat.CurBitRate*6/7;
                gNetBitrateStat.ZeroLost = 0;
                AmbaPrint("[AppLib - NetFifo] <BitrateControl> fraction lost over 25%, set bitrate=%d\n", NextBitrate);
            }else if(FractionLost > 25 && FractionLost > gNetBitrateStat.LastFrameLost){ // > 10 %
                NextBitrate = gNetBitrateStat.CurBitRate*13/14;
                gNetBitrateStat.ZeroLost = 0;
                AmbaPrint("[AppLib - NetFifo] <BitrateControl> fraction lost over 10%, set bitrate=%d\n", NextBitrate);
            }else if(FractionLost > 10){ //if we are in bad channel fr_lost can be 1-10
                gNetBitrateStat.ZeroLost = 0;
                AmbaPrint("[AppLib - NetFifo] <BitrateControl> FractionLost=%d, reset lost_count\n", FractionLost);
            }else{
                gNetBitrateStat.ZeroLost++;
                AmbaPrint("[AppLib - NetFifo] <BitrateControl> FractionLost=0, ++lost_count=%d\n", gNetBitrateStat.ZeroLost);
#if 1 //turn on to calculate maxStable bitrate accordring to (lastBR, curBR, LastFrameLost)
                if(gNetBitrateStat.InMiddleOfDecrement){
                    double delta = (double) gNetBitrateStat.LastFrameLost/ (double) 255;
                    gNetBitrateStat.MaxStableBR = (1-delta)*gNetBitrateStat.LastBitRate + delta*gNetBitrateStat.CurBitRate;
                    gNetBitrateStat.MaxStableBR = (gNetBitrateStat.MaxStableBR/10000)*10000;
                    AmbaPrint("[AppLib - NetFifo] <BitrateControl> delta=%lf , lastBitRate = %d, curBitRate=%d ,set max_stable bitrate=%d\n",delta,  gNetBitrateStat.LastBitRate, gNetBitrateStat.CurBitRate, gNetBitrateStat.MaxStableBR);
                    gNetBitrateStat.InMiddleOfDecrement = 0;
                }
#endif
                if(gNetBitrateStat.ZeroLost > 2 &&   (gNetBitrateStat.NetBandwidth + NET_BITRATE_CONTROL_INCREMENT_STEP) <= gNetBitrateStat.MaxStableBR && (CurTime - LastBitRateUpdateTime) > 5){
                    AMBA_IMG_ENC_MONITOR_BRC_HDLR_CFG_s cfg;
                    APPLIB_VIDEOENC_BITRATE_s *VideoEncBitRateData = AppLibSysSensor_GetVideoBitRate(AppLibVideoEnc_GetSensorVideoRes(), AppLibVideoEnc_GetQuality());

                    /* three consecutive fr_lost=0, try to raise net bandwith */
                    gNetBitrateStat.NetBandwidth += NET_BITRATE_CONTROL_INCREMENT_STEP;
                    gNetBitrateStat.ZeroLost = 0;
                    gNetBitrateStat.InMiddleOfIncrement = 1;

                    /**TODO:Set new bitrate to encoder monitor*/
                    cfg.MaxBitrate =gNetBitrateStat.NetBandwidth;
                    cfg.AverageBitrate = (UINT32)cfg.MaxBitrate/VideoEncBitRateData->BitRateRatioMax;
                    cfg.MinBitrate = (UINT32)cfg.AverageBitrate * VideoEncBitRateData->BitRateRatioMin;
                    if (gStreamList.ActiveStreamID == AMP_VIDEOENC_STREAM_PRIMARY) {
                        extern AMBA_IMG_ENC_MONITOR_BITRATE_HDLR_s *BrcHdlrPri;
                        AmbaEncMonitorBRC_RunTimeBitRateChange(&cfg, &BrcHdlrPri);
                    } else {
                        extern AMBA_IMG_ENC_MONITOR_BITRATE_HDLR_s *BrcHdlrSec;
                        AmbaEncMonitorBRC_RunTimeBitRateChange(&cfg, &BrcHdlrSec);
                    }
                    LastBitRateUpdateTime = CurTime;
                    /***/
                    AmbaPrint("[AppLib - NetFifo] <BitrateControl> try to raise bandwidth to %d(max_stable=%d)\n", gNetBitrateStat.NetBandwidth, gNetBitrateStat.MaxStableBR);
                }
#if 0 //turn on to raise max_stable automatically, we suggest this should be raised by retmoe command API(or reset streaming session)
                else if(RRStat.zeroLost > MAX_STATBLE_COUNT_RAISE && (RRStat.maxStableBR + INCREMENT_STEP)<= RRStat.maxBitRate){
                    RRStat.maxStableBR+=INCREMENT_STEP;
                    RRStat.zeroLost = 0;
                    AmbaPrint("[dynamic bitrate] try to raise max_stable_bitrate to %d\n", RRStat.maxStableBR);
                }
#endif
            }
        }
    }

    if(NextBitrate < gNetBitrateStat.CurBitRate && NextBitrate > 0 && (CurTime - LastBitRateUpdateTime) > 5){
        AMBA_IMG_ENC_MONITOR_BRC_HDLR_CFG_s cfg;
        APPLIB_VIDEOENC_BITRATE_s *VideoEncBitRateData = AppLibSysSensor_GetVideoBitRate(AppLibVideoEnc_GetSensorVideoRes(), AppLibVideoEnc_GetQuality());

        gNetBitrateStat.InMiddleOfDecrement = 1;
        gNetBitrateStat.NetBandwidth = (NextBitrate/10000)*10000;

        /**TODO:Set new bitrate to encoder monitor*/
        cfg.MaxBitrate =gNetBitrateStat.NetBandwidth;
        cfg.AverageBitrate = (UINT32)cfg.MaxBitrate/VideoEncBitRateData->BitRateRatioMax;
        cfg.MinBitrate = (UINT32)cfg.AverageBitrate * VideoEncBitRateData->BitRateRatioMin;

        if (gStreamList.ActiveStreamID == AMP_VIDEOENC_STREAM_PRIMARY) {
            extern AMBA_IMG_ENC_MONITOR_BITRATE_HDLR_s *BrcHdlrPri;
            AmbaEncMonitorBRC_RunTimeBitRateChange(&cfg, &BrcHdlrPri);
        } else {
            extern AMBA_IMG_ENC_MONITOR_BITRATE_HDLR_s *BrcHdlrSec;
            AmbaEncMonitorBRC_RunTimeBitRateChange(&cfg, &BrcHdlrSec);
        }
        LastBitRateUpdateTime = CurTime;
        /***/
        AmbaPrint("[AppLib - NetFifo] <BitrateControl> try to decrease bandwidth to %d(now=%d)\n", gNetBitrateStat.NetBandwidth, gNetBitrateStat.CurBitRate);
        gNetBitrateStat.LastBitRate = gNetBitrateStat.CurBitRate;
        gNetBitrateStat.CurBitRate = NextBitrate;
    }
    gNetBitrateStat.LastFrameLost = FractionLost;

    NETFIFO_DBG("[AppLib - NetFifo] <BitrateControl> End calculate bitrate according to report");
    return OK;
}


/**
 * callback function to handle NetFifo event.
 */
static int NetFifoEventCB(void *hdlr, UINT32 event, void* info)
{
    int ReturnValue = OK;

    NETFIFO_DBG("[AppLib - NetFifo] <NetFifoEventCB> NetFifo Event 0x%08x", event);

    switch (event) {
    case AMP_NETFIFO_EVENT_START:
        //NETFIFO_DBG("[AppLib - NetFifo] <EventCB> AMP_NETFIFO_EVENT_START");
        AppLibComSvcHcmgr_SendMsgNoWait(AMSG_NETFIFO_EVENT_START, 0, 0);
        break;
    case AMP_NETFIFO_EVENT_END:
        //NETFIFO_DBG("[AppLib - NetFifo] <EventCB> AMP_NETFIFO_EVENT_END");
        gState = AMP_NETFIFO_NOTIFY_STOPENC;
        AppLibComSvcHcmgr_SendMsgNoWait(AMSG_NETFIFO_EVENT_STOP, 0, 0);
        break;
    case AMP_NETFIFO_EVENT_GENERAL_ERROR:
        NETFIFO_ERR("[AppLib - NetFifo] <NetFifoEventCB> AMP_NETFIFO_EVENT_GENERAL_ERROR");
        break;
    default:
        NETFIFO_DBG("[AppLib - NetFifo] <NetFifoEventCB> unknown event");
        ReturnValue = NG;
        break;
    }

    return ReturnValue;
}

/**
 * callback function to handle get MediaInfo request.
 */
static int NetFifoMediaInfoCB(void *hdlr, UINT32 event, void* info)
{
    UINT16 Cmd = 0;
    int StreamId = -1;
    int ReturnValue = NG;

    NETFIFO_DBG("[AppLib - NetFifo] <NetFifoMediaInfoCB> event = 0x%x",event);
    Cmd = AMP_NETFIFO_GET_MEDIA_CMD(event);
    switch (Cmd) {
    case AMP_NETFIFO_MEDIA_CMD_GET_STREAM_LIST:
        ReturnValue = GetStreamIDList((AMP_NETFIFO_MEDIA_STREAMID_LIST_s *)info);
        break;
    case AMP_NETFIFO_MEDIA_CMD_GET_INFO:
        StreamId = AMP_NETFIFO_GET_MEDIA_PARAM(event);
        gStreamList.ActiveStreamID = StreamId;
        ReturnValue = GetMediaInfo(StreamId, (AMP_NETFIFO_MOVIE_INFO_CFG_s *)info);
        break;
    default:
        NETFIFO_ERR("[AppLib - NetFifo] <NetFifoMediaInfoCB> unsupported cmd 0x04%x, event:0x%08x", Cmd, event);
        ReturnValue = NG;
        break;
    }

    return ReturnValue;
}




/**
 * callback function to handle playback request.
 */
static int NetFifoPlaybackCB(void *hdlr, UINT32 event, void* info)
{
    int rval = 0;
    char *pFilename = NULL;
    UINT32 *pStartTime = NULL;
    UINT32 StartTime = 0;
    UINT32 *u_ptr;
    AMP_NETFIFO_PLAYBACK_OP_INFO_s *Param = NULL;
    int ReturnValue = 0;

    //NETFIFO_DBG("[AppLib - NetFifo] <PlaybackCB> Got NetFifo Playback request 0x%08x.",event);

    switch(event){
    case AMP_NETFIFO_PLAYBACK_OPEN:
        /* rval: 0 for success, -1 for fail */
        pFilename = (char *)info;
        if(pFilename == NULL){
            NETFIFO_ERR("[AppLib - NetFifo] <PlaybackCB> AMP_NETFIFO_PLAYBACK_OPEN - invalid parameter!");
            rval = -1;
        } else {
            NETFIFO_DBG("[AppLib - NetFifo] <PlaybackCB> AMP_NETFIFO_PLAYBACK_OPEN - filename %s",pFilename);
            ReturnValue = PlaybackOpen(pFilename);
            rval = (ReturnValue == 0) ? NETFIFO_PB_MEDIA_STREAM_ID : -1;
        }
        break;
    case AMP_NETFIFO_PLAYBACK_PLAY:
        /* rval: 0 for success, -1 for fail */
        pStartTime = (UINT32 *)info;
        if(pStartTime == NULL) {
            NETFIFO_DBG("[AppLib - NetFifo] <PlaybackCB> AMP_NETFIFO_PLAYBACK_PLAY - from beginning!");
        } else {
            NETFIFO_DBG("[AppLib - NetFifo] <PlaybackCB> AMP_NETFIFO_PLAYBACK_PLAY - from %d second",*pStartTime);
        }

        StartTime = (pStartTime == NULL) ? 0 : *pStartTime;
        ReturnValue = PlaybackPlay(StartTime);
        rval = (ReturnValue == 0) ? 0 : -1;
        break;
    case AMP_NETFIFO_PLAYBACK_STOP:
        /* rval: 0 for success, -1 for fail */
        NETFIFO_DBG("[AppLib - NetFifo] <PlaybackCB> AMP_NETFIFO_PLAYBACK_STOP");
        rval = PlaybackStop();
        break;
    case AMP_NETFIFO_PLAYBACK_RESET:
        /* rval: 0 for success, -1 for fail */
        /* file opened, but client leave before send PLAY command*/
        NETFIFO_DBG("[AppLib - NetFifo] <PlaybackCB> AMP_NETFIFO_PLAYBACK_RESET");
        rval = PlaybackReset();
        break;
    case AMP_NETFIFO_PLAYBACK_PAUSE:
        /* rval: 0 for success, -1 for fail */
        NETFIFO_DBG("[AppLib - NetFifo] <PlaybackCB> AMP_NETFIFO_PLAYBACK_PAUSE");
        rval = PlaybackPause();
        break;
    case AMP_NETFIFO_PLAYBACK_RESUME:
        /* rval: 0 for success, -1 for fail */
        NETFIFO_DBG("[AppLib - NetFifo] <PlaybackCB> AMP_NETFIFO_PLAYBACK_RESUM");
        rval = PlaybackResume();
        break;
    case AMP_NETFIFO_PLAYBACK_CONFIG:
        /* rval: 0 for success, -1 for fail */
        NETFIFO_DBG("[AppLib - NetFifo] <PlaybackCB> AMP_NETFIFO_PLAYBACK_CONFIG");
        rval = 0;
        break;
    case AMP_NETFIFO_PLAYBACK_GET_VID_FTIME:
        /* rval: Time per video frame in ms */
        NETFIFO_DBG("[AppLib - NetFifo] <PlaybackCB> AMP_NETFIFO_PLAYBACK_GET_VID_FTIME");
        //rval = (1000 * AppLibVideoEnc_GetSecStreamTick())/AppLibVideoEnc_GetSecStreamTimeScale();
        ReturnValue = PlaybackGetVideoInfo(&rval, AMP_NETFIFO_PLAYBACK_GET_VID_FTIME);
        if (ReturnValue != 0) {
            rval = -1;
        }
        break;
    case AMP_NETFIFO_PLAYBACK_GET_VID_TICK:
        /* rval: Tick per video frame (need to be in 90K Base)*/
        NETFIFO_DBG("[AppLib - NetFifo] <PlaybackCB> AMP_NETFIFO_PLAYBACK_GET_VID_TICK!");
        //rval = AppLibVideoEnc_GetSecStreamTick() * (90000/AppLibVideoEnc_GetSecStreamTimeScale());
        ReturnValue = PlaybackGetVideoInfo(&rval, AMP_NETFIFO_PLAYBACK_GET_VID_TICK);
        if (ReturnValue != 0) {
            rval = -1;
        }
        break;
    case AMP_NETFIFO_PLAYBACK_GET_AUD_FTIME:
        /* rval: Time per audio frame in ms */
        NETFIFO_DBG("[AppLib - NetFifo] <PlaybackCB> AMP_NETFIFO_PLAYBACK_GET_AUD_FTIME!");
        rval = 21;
        break;
    case AMP_NETFIFO_PLAYBACK_GET_DURATION:
        /* rval: File duration in ms. 0 to disable seek function. */
        NETFIFO_DBG("[AppLib - NetFifo] <PlaybackCB> AMP_NETFIFO_PLAYBACK_GET_DURATION!");
        #if 1
        ReturnValue = PlaybackGetVideoInfo(&rval, AMP_NETFIFO_PLAYBACK_GET_DURATION);
        if (ReturnValue != 0) {
            rval = -1;
        }
        #else
        rval = 0;
        #endif
        break;
    case AMP_NETFIFO_PLAYBACK_SET_PARAMETER:
        /* rval: 0 for success, -1 for fail */
        Param = (AMP_NETFIFO_PLAYBACK_OP_INFO_s *)info;
        if(Param == NULL){
            NETFIFO_ERR("[AppLib - NetFifo] <PlaybackCB> AMP_NETFIFO_PLAYBACK_SET_PARAMETER - invalid parameter!");
            rval = -1;
        } else {
            NETFIFO_DBG("[AppLib - NetFifo] <PlaybackCB> AMP_NETFIFO_PLAYBACK_SET_PARAMETER - extend string : %s",Param->inParam);
            //handle extend string and put the response into param->resParam
            snprintf((char*)Param->resParam, 128, "Append_response: 0\r\n"); //an example
            rval = 0;
        }
        break;
    case AMP_NETFIFO_PLAYBACK_SET_LIVE_BITRATE:
        /* rval: 0 for success, -1 for fail */
        /* param is bitrate in bps */
        u_ptr = (UINT32 *)info;

        if(u_ptr == NULL) {
            NETFIFO_ERR("[AppLib - NetFifo] <PlaybackCB> AMP_NETFIFO_PLAYBACK_SET_LIVE_BITRATE: invalid param!");
            rval = -1;
        } else {
            NETFIFO_DBG("[AppLib - NetFifo] <PlaybackCB> AMP_NETFIFO_PLAYBACK_SET_LIVE_BITRATE: wanted %d!",u_ptr[0]);
            rval = 0;
        }
    break;
    case AMP_NETFIFO_PLAYBACK_GET_LIVE_BITRATE:
        /* rval: runtime bitrate for Live streaming in bps*/
        NETFIFO_DBG("[AppLib - NetFifo] <PlaybackCB> AMP_NETFIFO_PLAYBACK_GET_LIVE_BITRATE!");
        rval = AppLibVideoEnc_GetSecStreamBitRate();
        break;
    case AMP_NETFIFO_PLAYBACK_GET_LIVE_AVG_BITRATE:
        /* rval: avg bitrate for Live streaming in bps*/
        NETFIFO_DBG("[AppLib - NetFifo] <PlaybackCB> AMP_NETFIFO_PLAYBACK_GET_LIVE_AVG_BITRATE!");
        rval = AppLibVideoEnc_GetSecStreamBitRate();
        break;
    case AMP_NETFIFO_PLAYBACK_SET_NET_BANDWIDTH:
        /* rval: 0 for success, -1 for fail */
        /* param is bandwidth in bps */
        u_ptr = (UINT32 *)info;
        RRStatStart = 0;
        if(u_ptr == NULL) {
            NETFIFO_ERR("[AppLib - NetFifo] <PlaybackCB> AMP_NETFIFO_PLAYBACK_SET_NET_BANDWIDTH: invalid param!");
            rval = -1;
        } else {
            NETFIFO_DBG("[AppLib - NetFifo] <PlaybackCB> AMP_NETFIFO_PLAYBACK_SET_NET_BANDWIDTH: wanted %d!",u_ptr[0]);
            rval = 0;
        }
        rval = 0;
        break;
    case AMP_NETFIFO_PLAYBACK_SEND_RR_STAT:
        {
            NET_STREAM_REPORT_s *report = (NET_STREAM_REPORT_s *) info;
            NETFIFO_DBG("[AppLib - NetFifo] <PlaybackCB> AMP_NETFIFO_PLAYBACK_SEND_RR_STAT: FrameLost = %d Jitter = %d PGDlay = %llu",report->FrameLost,report->Jitter,report->PGDelay);
            rval = BitrateControl(report->FrameLost, report->Jitter, report->PGDelay);
            /*skip first rr_stat report due to first report data is not suitable for reference usually is 225*/
            RRStatStart = 1;
            break;
        }
    case AMP_NETFIFO_PLAYBACK_GET_SPS_PPS:
        /* rval :0 for success, -1 for fail
            * strlen(inParam) == 0, return the sps&pps of the file opened by AMBA_NETFIFO_PLAYBACK_OPEN
            * strlen(inParam) != 0, return the sps&pps of the file specified in inParam
            */
        Param = (AMP_NETFIFO_PLAYBACK_OP_INFO_s *)info;
        if(Param == NULL){
            NETFIFO_ERR("[AppLib - NetFifo] <PlaybackCB> AMP_NETFIFO_PLAYBACK_GET_SPS_PPS - invalid parameter!");
            rval = -1;
        } else {
            NETFIFO_DBG("[AppLib - NetFifo] <PlaybackCB> AMP_NETFIFO_PLAYBACK_GET_SPS_PPS, fn=%s", (char*) Param->inParam);
            rval = GetSPSAndPPS((char*)Param->inParam, (char*)Param->resParam);
        }
        break;
    default:
        NETFIFO_ERR("[AppLib - NetFifo] <PlaybackCB> Unsuoported event 0x%08x",event);
        rval = -1;
        break;
   }

    return rval;
}

int AppLibNetFifo_NotifyAppStateChange(AMP_NETFIFO_NOTIFY_TYPE_e NewState)
{
    if (NewState == gState) {
        NETFIFO_DBG("[AppLib - NetFifo] <ChangeAppState> state not changed! (state = %d)", NewState);
        return NG;
    }

    if (AppLibNetBase_GetBootStatus() == 0) {
        NETFIFO_ERR("[AppLib - NetFifo] <ChangeAppState> Linux not booted! (state = %d)", NewState);
        return NG;
    }

    if (!gNetFifoInited) {
        NETFIFO_ERR("[AppLib - NetFifo] <ChangeAppState> Please init netFifo first!");
        return NG;
    }

    NETFIFO_DBG("[AppLib - NetFifo] <ChangeAppState> change state from %d to %d", gState, NewState);
    gState = NewState;
    AmpNetFifo_SendNotify(NewState, 0, 0);

    return OK;
}

int AppLibNetFifo_StartRTSPServer(void)
{
    int ReturnValue = OK;

    if (gRTSPServerOn) {
        NETFIFO_DBG("[AppLib - NetFifo] <StartRTSPServer> RTSP server is on already");
        return ReturnValue;
    }

    if ((AppLibVideoEnc_GetRecMode() == REC_MODE_AV) && (AppLibNetFifo_GetStreamMode() == NET_STREAM_MODE_AV)) {
        ReturnValue = AmpNetUtility_luExecNoResponse("/usr/bin/AmbaRTSPServer --en_audio --en_rtcp");
        NETFIFO_DBG("[AppLib - NetFifo] <StartRTSPServer> /usr/bin/AmbaRTSPServer --en_audio --en_rtcp");
    } else {
        ReturnValue = AmpNetUtility_luExecNoResponse("/usr/bin/AmbaRTSPServer");
        NETFIFO_DBG("[AppLib - NetFifo] <StartRTSPServer> /usr/bin/AmbaRTSPServer");
    }

    if (ReturnValue != 0) {
        NETFIFO_ERR("[AppLib - NetFifo] <StartRTSPServer> start RTSP server fail");
    } else {
    	NETFIFO_DBG("[AppLib - NetFifo] <StartRTSPServer> start RTSP server successfully");
        gRTSPServerOn = 1;
    }


    return ReturnValue;
}

int AppLibNetFifo_StopRTSPServer(void)
{
    int ReturnValue = OK;

    if(!gRTSPServerOn) {
        NETFIFO_DBG("[AppLib - NetFifo] <StopRTSPServer> RTSP server is off already");
        return ReturnValue;
    }

    ReturnValue = AmpNetUtility_luExecNoResponse("killall AmbaRTSPServer");
    if (ReturnValue != 0) {
        NETFIFO_ERR("[AppLib - NetFifo] <StopRTSPServer> stop RTSP server fail");
    } else {
        NETFIFO_DBG("[AppLib - NetFifo] <StopRTSPServer> stop RTSP server successfully");
        gRTSPServerOn = 0;
    }

    return ReturnValue;
}

int AppLibNetFifo_SetStreamMode(NET_STREAM_MODE_e mode)
{
    if (mode >= NET_STREAM_MODE_MAX) {
        return -1;
    }

    if ((mode == NET_STREAM_MODE_AV) && (AppLibVideoEnc_GetRecMode() == REC_MODE_VIDEO_ONLY)) {
        /* If the record mode is REC_MODE_VIDEO_ONLY, there is no audio stream existed.
                So net stream mode can't be set to NET_STREAM_MODE_AV in this case. */
        mode = NET_STREAM_MODE_VIDEO_ONLY;
    }

    gNetStreamMode = mode;

    return 0;
}

NET_STREAM_MODE_e AppLibNetFifo_GetStreamMode(void)
{
    return gNetStreamMode;
}


int AppLibNetFifo_GetInitStatus(void)
{
    return gNetFifoInited;
}

int AppLibNetFifo_PlaybackSendMsg(APP_NET_PB_MESSAGE_s *msg, UINT32 waitOption)
{
    int ReturnValue = 0;

    ReturnValue = AmbaKAL_MsgQueueSend(&gNetFifoPBMgr.MsgQueue, msg, waitOption);
    if (ReturnValue != OK) {
        NETFIFO_ERR("[AppLib - NetFifo] <PlaybackSendMsg> AmbaKAL_MsgQueueSend() fail");
        return -1;
    }

    return ReturnValue;
}

int AppLibNetFifo_PlaybackRecvMsg(APP_NET_PB_MESSAGE_s *msg, UINT32 waitOption)
{
    int ReturnValue = 0;

    ReturnValue = AmbaKAL_MsgQueueReceive(&gNetFifoPBMgr.MsgQueue, (void *)msg, waitOption);
    if (ReturnValue != OK) {
        NETFIFO_ERR("[AppLib - NetFifo] <PlaybackRecvMsg> AmbaKAL_MsgQueueReceive() fail");
        return -1;
    }

    return ReturnValue;
}

int AppLibNetFifo_PlaybackRegisterApp(APPLIB_NETFIFO_PB_f NetFifoPlayback)
{
    if (!NetFifoPlayback) {
        NETFIFO_ERR("[AppLib - NetFifo] <PlaybackRegisterApp> NetFifoPlayback is NULL");
        return -1;
    }

    gNetFifoPBMgr.NetPlaybackEntry = NetFifoPlayback;

    return 0;
}

int AppLibNetFifo_EnablePlayback(void)
{
    int ReturnValue = 0;

    if (gNetFifoInited != 1) {
        NETFIFO_ERR("[AppLib - NetFifo] <EnablePlayback> netFifo not inited. Please init netFifo first!");
        return -1;
    }

    ReturnValue = PlaybackInit();
    if (ReturnValue != 0) {
        return -1;
    }

    NETFIFO_DBG("[AppLib - NetFifo] <EnablePlayback> Enable RTSP Playback");

    return 0;
}

int AppLibNetFifo_Init(void)
{
    AMP_NETFIFO_INIT_CFG_s InitCfg = {0};
    int ReturnValue = OK;

    if (gNetFifoInited) {
        NETFIFO_DBG("[AppLib - NetFifo] <Init> init already");
        return ReturnValue;
    }

    memset((void*)&gStreamList, 0, sizeof(APPLIB_VIDEOENC_STREAM_LIST_s));

    AmpNetFifo_GetInitDefaultCfg(&InitCfg);
    InitCfg.cbEvent = NetFifoEventCB;
    InitCfg.cbMediaInfo = NetFifoMediaInfoCB;
    InitCfg.cbPlayback = NetFifoPlaybackCB;
    ReturnValue = AmpNetFifo_init(&InitCfg);
    if (ReturnValue == OK) {
        gNetFifoInited = 1;
        NETFIFO_DBG("[AppLib - NetFifo] <Init> init successfully");
        #if 0
        ReturnValue = PlaybackInit();
        if (ReturnValue != 0) {
            NETFIFO_ERR("[AppLib - NetFifo] <Init> PlaybackInit() fail");
        }
        #endif
    } else {
        NETFIFO_ERR("[AppLib - NetFifo] <Init> init fail");
    }

    /* Initialize  gNetStreamMode variable according to record mode. */
    if (AppLibVideoEnc_GetRecMode() == REC_MODE_AV) {
        gNetStreamMode = NET_STREAM_MODE_AV;
    } else {
        gNetStreamMode = NET_STREAM_MODE_VIDEO_ONLY;
    }

    return ReturnValue;
}

