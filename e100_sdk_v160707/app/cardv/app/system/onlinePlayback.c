#include "net/ApplibNet_Fifo.h"
#include "transcoder/video_decode/ApplibTranscoder_VideoDec.h"
#include "format/ApplibFormat.h"
#include "format/ApplibFormat_DemuxMp4.h"

//#define PLAY_REPEAT_ON

#define ONLINE_PB_DBG_EN
#define ONLINE_PB_ERR_EN

#undef ONLINE_PB_DBG
#ifdef ONLINE_PB_DBG_EN
#define ONLINE_PB_DBG(fmt,args...) AmbaPrintColor(CYAN,fmt,##args);
#else
#define ONLINE_PB_DBG(fmt,args...)
#endif

#undef ONLINE_PB_ERR
#ifdef ONLINE_PB_ERR_EN
#define ONLINE_PB_ERR(fmt,args...) AmbaPrintColor(RED,fmt,##args);
#else
#define ONLINE_PB_ERR(fmt,args...)
#endif

#define PLAYLIST_FILE_MUM (1)

/**
 * playback state
 */
typedef enum _PLAYBACK_STATE_e_ {
    PLAYBACK_STATE_IDLE,         ///< Ready to load a file.
    PLAYBACK_STATE_PLAY,         ///< playing.
    PLAYBACK_STATE_PAUSE,        ///< paused.
    PLAYBACK_STATE_NUM           ///< Total number of states.
} PLAYBACK_STATE_e;

typedef struct _PLAYLIST_s_ {
    char PlayList[PLAYLIST_FILE_MUM][APPLIB_NETFIFO_MAX_FN_SIZE];
    UINT32 ReadPointer;
    UINT32 WritePointer;
} PLAYLIST_s;

static PLAYBACK_STATE_e gPlaybackState = PLAYBACK_STATE_IDLE;
static UINT8 gSessionStartFlag = 0;  // 1: session start, 0: session stop
//static volatile UINT8 gDemuxerEnd = 0; //val

static PLAYLIST_s gPlayList = {0};
static AMBA_KAL_MUTEX_t MutexPlayList;
static UINT8 gQueryBreakFlag = 0;

//----------------------------------------------------------------------
// playlist
//----------------------------------------------------------------------
static int OnlinePlayback_IsPlayListFull(void)
{
    int IsFull = 0;

    // return 1: full, 0: not full
    AmbaKAL_MutexTake(&MutexPlayList, AMBA_KAL_WAIT_FOREVER);
    if (gPlayList.WritePointer == 1) {
        IsFull = 1;
    }
    AmbaKAL_MutexGive(&MutexPlayList);

    AmbaPrintColor(CYAN,"<%s> L%d IsFull = %d",__FUNCTION__,__LINE__,IsFull);

    return IsFull;
}

static int OnlinePlayback_IsPlayListEmpty(void)
{
    int IsEmpty = 0;

    // return 1: empty, 0: not empty
    AmbaKAL_MutexTake(&MutexPlayList, AMBA_KAL_WAIT_FOREVER);
    if (gPlayList.WritePointer == 0) {
        IsEmpty = 1;
    }
    AmbaKAL_MutexGive(&MutexPlayList);

    return IsEmpty;
}

static int OnlinePlayback_AddToPlayList(char *Filename)
{
    if (!Filename) {
        ONLINE_PB_ERR("[App OnlinePlayback] <AddToPlayList> Filename is null");
        return -1;
    }

    AmbaKAL_MutexTake(&MutexPlayList, AMBA_KAL_WAIT_FOREVER);
    if (gPlayList.WritePointer == 1) {
        ONLINE_PB_ERR("[App OnlinePlayback] <AddToPlayList> playlist is full");
        AmbaKAL_MutexGive(&MutexPlayList);
        return -1;
    }
    snprintf(gPlayList.PlayList[0], APPLIB_NETFIFO_MAX_FN_SIZE, "%s", Filename);
    gPlayList.WritePointer = 1;
    AmbaKAL_MutexGive(&MutexPlayList);
    return 0;
}

static int OnlinePlayback_RetrievePlayList(char *FilenameBuf, int FilenameBufSize)
{
    if ((!FilenameBuf) || (FilenameBufSize == 0)) {
        ONLINE_PB_ERR("[App OnlinePlayback] <RetrievePlayList> invalid param");
        return -1;
    }

    AmbaKAL_MutexTake(&MutexPlayList, AMBA_KAL_WAIT_FOREVER);
    if (gPlayList.WritePointer == 0) {
        ONLINE_PB_ERR("[App OnlinePlayback] <RetrievePlayList> playlist is empty");
        AmbaKAL_MutexGive(&MutexPlayList);
        return -1;
    }

    snprintf(FilenameBuf, FilenameBufSize, "%s", gPlayList.PlayList[0]);
    gPlayList.WritePointer = 0;

    AmbaKAL_MutexGive(&MutexPlayList);

    return 0;
}

int OnlinePlayback_QueryNextFile(char *FilenameBuf, int FilenameBufSize)
{
#ifdef ONLINE_PB_DBG_EN
    int Count = 0;
#endif

    if ((FilenameBuf == NULL) || (FilenameBufSize == 0)) {
        ONLINE_PB_ERR("[App OnlinePlayback] <QueryNextFile> invalid param");
        return -1;
    }

    while (1) {
        if (gSessionStartFlag == 0) {
            /* session stop */
            AmbaPrintColor(CYAN,"<%s> L%d play session stop!",__FUNCTION__,__LINE__);
            return -1;
        } else if (gQueryBreakFlag == 1) {
            AmbaPrintColor(CYAN,"<%s> L%d query break!",__FUNCTION__,__LINE__);
            return -2;
        } else if ((gSessionStartFlag == 1) && (OnlinePlayback_IsPlayListEmpty() == 0)) {
            /* setup next played file name (retrieve from playlist) */
            OnlinePlayback_RetrievePlayList(FilenameBuf, FilenameBufSize);
#ifdef PLAY_REPEAT_ON
            OnlinePlayback_AddToPlayList(FilenameBuf);
#endif
            ONLINE_PB_DBG("[App OnlinePlayback] <QueryNextFile> FilenameBuf: %s",FilenameBuf);
            return 0;
        }

#ifdef ONLINE_PB_DBG_EN
        Count++;
        if (Count >= 100) {
            AmbaPrintColor(CYAN,"<%s> L%d wait!",__FUNCTION__,__LINE__);
            Count = 0;
        }
#endif

        AmbaKAL_TaskSleep(10);
    }

    return 0;
}

static int OnlinePlayback_Play(APP_NET_PB_MESSAGE_s *Msg)
{
    APPLIB_VIDEO_TRANSCODE_START_INFO_s StartInfo = {0};
    int ReturnValue = 0;

    if (!Msg) {
        ONLINE_PB_ERR("[App OnlinePlayback] <Play> NULL pointer");
        return -1;
    }

    if ((gSessionStartFlag == 1) && (Msg->MessageData[0] == 0)) {
        AmbaPrintColor(CYAN,"<%s> L%d AddToPlayList()",__FUNCTION__,__LINE__);
        // feed next played file name
        OnlinePlayback_AddToPlayList(Msg->Filename);
    } else {
        // first played file or seek
        if (gSessionStartFlag == 0) {
            AmbaPrintColor(CYAN,"<%s> L%d play session start",__FUNCTION__,__LINE__);
            gSessionStartFlag = 1;
        }

        if (gPlaybackState == PLAYBACK_STATE_PAUSE) {
            AmbaPrintColor(CYAN,"<%s> L%d seek",__FUNCTION__,__LINE__);
            AppLibTranscoderVideoDec_DisableNextFileFeed();
            if(AppLibTranscoderVideoDec_GetNextFileFeedStatus() == 1) {
                gQueryBreakFlag = 1;
                while (1) {
                    if(AppLibTranscoderVideoDec_GetNextFileFeedStatus() == 0) {
                        break;
                    }
                    AmbaKAL_TaskSleep(10);
                }
                gQueryBreakFlag = 0;
            }
        }

        OnlinePlayback_AddToPlayList(Msg->Filename);

        AppLibTranscoderVideoDec_GetStartDefaultCfg(&StartInfo);
        StartInfo.Filename = NULL;
        StartInfo.StartTime = Msg->MessageData[0];
        ReturnValue = AppLibTranscoderVideoDec_Start(&StartInfo);
        if (ReturnValue != 0) {
            ONLINE_PB_ERR("[App OnlinePlayback] <Play> AppLibTranscoderVideoDec_Start() fail");
            return -1;
        }

        gPlaybackState = PLAYBACK_STATE_PLAY;
    }

    return 0;
}

static int OnlinePlayback_Stop(void)
{
    int ReturnValue = 0;

    if (gPlaybackState == PLAYBACK_STATE_IDLE) {
        return ReturnValue;
    }

    AmbaKAL_MutexTake(&MutexPlayList, AMBA_KAL_WAIT_FOREVER);
    gSessionStartFlag = 0;
    /* reset playlist */
    memset(&gPlayList, 0, sizeof(PLAYLIST_s));
    AmbaKAL_MutexGive(&MutexPlayList);

    ReturnValue = AppLibTranscoderVideoDec_Stop();
    if (ReturnValue != 0) {
        ONLINE_PB_ERR("[App OnlinePlayback] <Stop> AppLibTranscoderVideoDec_Stop() fail");
        return -1;
    }

    gPlaybackState = PLAYBACK_STATE_IDLE;
    return ReturnValue;
}

static int OnlinePlayback_Reset(void)
{
    return 0;
}

static int OnlinePlayback_Pause(void)
{
    int ReturnValue = 0;

    if (gPlaybackState != PLAYBACK_STATE_PLAY) {
        ONLINE_PB_ERR("[App OnlinePlayback] <Pause> gPlaybackState != PLAYBACK_STATE_PLAY");
        return -1;
    }

    ReturnValue = AppLibTranscoderVideoDec_Pause();
    if (ReturnValue != 0) {
        ONLINE_PB_ERR("[App OnlinePlayback] <Pause> AppLibTranscoderVideoDec_Pause() fail");
        return -1;
    }

    gPlaybackState = PLAYBACK_STATE_PAUSE;
    return 0;
}

static int OnlinePlayback_Resume(void)
{
    int ReturnValue = 0;

    if (gPlaybackState != PLAYBACK_STATE_PAUSE) {
        ONLINE_PB_ERR("[App OnlinePlayback] <Resume> gPlaybackState != PLAYBACK_STATE_PAUSE");
        return -1;
    }

    ReturnValue = AppLibTranscoderVideoDec_Resume();
    if (ReturnValue != 0) {
        ONLINE_PB_ERR("[App OnlinePlayback] <Resume> AppLibTranscoderVideoDec_Pause() fail");
        return -1;
    }

    gPlaybackState = PLAYBACK_STATE_PLAY;
    return 0;
}

void OnlinePlayback(void)
{
    APP_NET_PB_MESSAGE_s Msg = {0};
    int ReturnValue = 0;

    /** Initialize the demuxer. */
    ReturnValue = AppLibFormat_DemuxerInit();
    if (ReturnValue != 0) {
        ONLINE_PB_ERR("[App] <OnlinePlayback> AppLibFormat_DemuxerInit() fail");
    }

    ReturnValue = AppLibTranscoderVideoDec_Init();
    if (ReturnValue != 0) {
        ONLINE_PB_ERR("[App] <OnlinePlayback> AppLibTranscoderVideoDec_Init() fail");
    }

    AppLibTranscoderVideoDec_RegQueryNextFileCb(OnlinePlayback_QueryNextFile);

    ReturnValue = AppLibFormatDemuxMp4_Init();
    if (ReturnValue != 0) {
        ONLINE_PB_ERR("[App] <OnlinePlayback> AppLibFormatDemuxMp4_Init() fail");
    }

    ReturnValue = AmbaKAL_MutexCreate(&MutexPlayList);
    if (ReturnValue != OK) {
        ONLINE_PB_ERR("[App] <OnlinePlayback> Create Mutex fail = %d", ReturnValue);
        return;
    }

    AmbaKAL_MutexTake(&MutexPlayList, AMBA_KAL_WAIT_FOREVER);
    memset(&gPlayList, 0, sizeof(PLAYLIST_s));
    AmbaKAL_MutexGive(&MutexPlayList);

    while (1) {
        AppLibNetFifo_PlaybackRecvMsg((void *)&Msg, AMBA_KAL_WAIT_FOREVER);
        ONLINE_PB_DBG("[App] <OnlinePlayback> Received msg: 0x%X (Param1 = 0x%X, Param2 = 0x%X)",
                                                           Msg.MessageID, Msg.MessageData[0], Msg.MessageData[1]);

        // check open status before playing
        switch (Msg.MessageID) {
        case APPLIB_NETFIFO_PLAYBACK_OPEN:
            //OnlinePlayback_Open(&Msg);
            break;
        case APPLIB_NETFIFO_PLAYBACK_PLAY:
            OnlinePlayback_Play(&Msg);
            break;
        case APPLIB_NETFIFO_PLAYBACK_STOP:
            OnlinePlayback_Stop();
            break;
        case APPLIB_NETFIFO_PLAYBACK_RESET:
            OnlinePlayback_Reset();
            break;
        case APPLIB_NETFIFO_PLAYBACK_PAUSE:
            OnlinePlayback_Pause();
            break;
        case APPLIB_NETFIFO_PLAYBACK_RESUME:
            OnlinePlayback_Resume();
            break;
        default:
            AmbaPrintColor(YELLOW, "<%s> L%d unknown msg id %d ",__FUNCTION__,__LINE__,Msg.MessageID);
            break;
        }
    }
}
//-----------------------------------------------------------------------------------------------


