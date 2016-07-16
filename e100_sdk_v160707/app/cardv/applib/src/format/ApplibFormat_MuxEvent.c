
#include <applib.h>
#include <fifo/Fifo.h>
#include <format/Muxer.h>
#include <format/Mp4Mux.h>
#include <stream/File.h>
#include <AmbaNAND_Def.h>


//#define DEBUG_APPLIB_FORMAT_MUX_EVENT
#if defined(DEBUG_APPLIB_FORMAT_MUX_EVENT)
#define DBGMSG AmbaPrint
#define DBGMSGc(x) AmbaPrintColor(GREEN,x)
#define DBGMSGc2 AmbaPrintColor
#else
#define DBGMSG(...)
#define DBGMSGc(...)
#define DBGMSGc2(...)
#endif

#define EVENT_SPLIT_TIME    (30 * 1000)
static UINT32 EventFileLength = EVENT_SPLIT_TIME;

#define AUDENC_VIDENC_BITSFIFO_SIZE (10 << 20)

static UINT32 MuxMp4VidEncBitsFifoSize = 0;
static UINT32 MuxMp4VidEncDescSize = 0;

static char PriFileName[MAX_FILENAME_LENGTH];
static UINT32 PriFileId = 0;
static char SecFileName[MAX_FILENAME_LENGTH];
static UINT32 SecFileId = 0;

static AMP_INDEX_HDLR_s *MuxMp4IndexPri_Eventrecord = NULL;
static AMP_STREAM_HDLR_s *MuxMp4StreamPri_Eventrecord = NULL;
static AMP_MUX_FORMAT_HDLR_s *MuxMp4FormatPri_Eventrecord = NULL;
static AMP_MOVIE_INFO_s *MuxMp4MoviePri_Eventrecord = NULL;

static AMP_INDEX_HDLR_s *MuxMp4IndexSec_Eventrecord = NULL;
static AMP_STREAM_HDLR_s *MuxMp4StreamSec_Eventrecord = NULL;
static AMP_MUX_FORMAT_HDLR_s *MuxMp4FormatSec_Eventrecord = NULL;
static AMP_MOVIE_INFO_s *MuxMp4MovieSec_Eventrecord = NULL;
static AMP_MUXER_PIPE_HDLR_s *MuxMp4MuxPipe_Eventrecord = NULL;

static AMP_FIFO_HDLR_s *MuxMp4VideoPriFifoHdlr_Eventrecord = NULL;
static AMP_FIFO_HDLR_s *MuxMp4VideoSecFifoHdlr_Eventrecord = NULL;
static AMP_FIFO_HDLR_s *MuxMp4AudioPriFifoHdlr_Eventrecord = NULL;
static AMP_FIFO_HDLR_s *MuxMp4AudioSecFifoHdlr_Eventrecord = NULL;

#define SPLIT_TIME    (1000 * 60 * 25)  // 25 min split
#define SPLIT_SIZE    ((UINT64)3750 * 1024 * 1024)  // 4G split

static UINT32 MuxMp4SplitTime_Eventrecord = SPLIT_TIME;
static UINT64 MuxMp4SplitSize_Eventrecord = SPLIT_SIZE;

/* for TEXT track */
extern AMP_AVENC_HDLR_s *VideoEncExtendHdlr;
static AMP_FIFO_HDLR_s *TextTrackPriFifoHdlr_Eventrecord = NULL;
static AMP_FIFO_HDLR_s *TextTrackSecFifoHdlr_Eventrecord = NULL;
static UINT8 TextTrackEnableFlag_Eventrecord = 1;

static UINT64 RealOnCreateTimeLength = 0;
static UINT64 RealOnCreateTimeLengthSec = 0;

static int event_flag = 0;

int AppLibFormat_SetSplit_EventRecord(int split)
{
    EventFileLength = split;
    return 0;
}

int AppLibFormat_GetEventStatus(void)
{
    return event_flag;
}

static int AppLibFormatMuxEvent_TextTrack_Configure(AMP_MUX_MOVIE_INFO_CFG_s *MovieCfg, AMP_FIFO_HDLR_s *FifoHdlr)
{
    /* To get video buffer information. */
    APPLIB_VIDEOENC_EXTEND_BITS_BUFFER_SETTING_s VideoBitsBuffer = {0};
    AppLibExtendEnc_GetConfigure(&VideoBitsBuffer);

    /* To configure text track. */
    MovieCfg->Track[MovieCfg->TrackCount].TrackType = AMP_MEDIA_TRACK_TYPE_TEXT;
    MovieCfg->Track[MovieCfg->TrackCount].Fifo = FifoHdlr;
    MovieCfg->Track[MovieCfg->TrackCount].BufferBase = (UINT8 *)VideoBitsBuffer.BufferAddress;
    MovieCfg->Track[MovieCfg->TrackCount].BufferLimit = (UINT8 *)VideoBitsBuffer.BufferAddress + VideoBitsBuffer.BufferSize;
    MovieCfg->Track[MovieCfg->TrackCount].MediaId = AMP_FORMAT_MID_TEXT;
    MovieCfg->Track[MovieCfg->TrackCount].TimePerFrame = VideoBitsBuffer.FrameRateScale;
    MovieCfg->Track[MovieCfg->TrackCount].Info.Text.IsDefault = 1;
    // Modify frame rate and scale for HFR (>60P)
    MovieCfg->Track[MovieCfg->TrackCount].TimeScale = VideoBitsBuffer.FrameRate;
    AmbaPrint("<_TextTrack_Configure> TimeScale %d, TimePerFrame %d", MovieCfg->Track[MovieCfg->TrackCount].TimeScale, MovieCfg->Track[MovieCfg->TrackCount].TimePerFrame);
    /* To increase track count. */
    MovieCfg->TrackCount += 1;
    return AMP_OK;
}

static int AppLibFormatMuxEvent_TextTrack_FifoEventCB(void *hdlr, UINT32 event, void* info)
{
    //AmbaPrintColor(MAGENTA,"[Applib - Format] <Mp4Mux_ExtFifoCB>: 0x%x",event);
    if (0) {
        AMP_FIFO_INFO_s tmpInfo;
        AmpFifo_GetInfo((AMP_FIFO_HDLR_s *)hdlr, &tmpInfo);
        //AmbaPrintColor(MAGENTA,"[Applib - Format] <Mp4Mux_ExtFifoCB>: TotalEntries = %d, AvailEntries=%d",tmpInfo.TotalEntries,tmpInfo.AvailEntries);
    }

    switch (event) {
        case AMP_FIFO_EVENT_DATA_READY:
            AppLibFormatMuxMgr_DataReady(hdlr, info, VIDEO_EVENTRECORD_MUXER_HANDLER);
            //AmbaPrint("<_TextTrack_FifoEventCB> AMP_FIFO_EVENT_DATA_READY");
            break;
        case AMP_FIFO_EVENT_DATA_EOS:
            AppLibFormatMuxMgr_DataEos(hdlr, info);
            //AmbaPrint("<_TextTrack_FifoEventCB> AMP_FIFO_EVENT_DATA_EOS");
            break;
        default:
            AmbaPrint("[Applib - Format] <Mp4Mux_ExtFifoCB>: evnet 0x%x", event);
            break;
    }

    return 0;
}


static int AppLibFormatMuxEvent_TextTrack_Init(AMP_FIFO_HDLR_s **FifoHdlr)
{
    AMP_FIFO_CFG_s FifoDefCfg = {0};
    APPLIB_VIDEOENC_EXTEND_BITS_BUFFER_SETTING_s VideoBitsBuffer = {0};
    AppLibExtendEnc_GetConfigure(&VideoBitsBuffer);

    /* Error Check. */
    if (!AppLibExtendEnc_GetEnableStatus()) {
        AmbaPrintColor(BLUE, "<_TextTrack_Init> Enable extend encode first!");
        return AMP_OK;
    }
    if (!TextTrackEnableFlag_Eventrecord) {
        AmbaPrintColor(BLUE, "<_TextTrack_Init> Disable text track.");
        return AMP_OK;
    }

    /* To initialize the text fifo. */
    AmpFifo_GetDefaultCfg(&FifoDefCfg);
    FifoDefCfg.hCodec = VideoEncExtendHdlr;
    FifoDefCfg.IsVirtual = 1;
    FifoDefCfg.NumEntries = 8192;
    FifoDefCfg.cbEvent = AppLibFormatMuxEvent_TextTrack_FifoEventCB;
    FifoDefCfg.InitData.CreateFifoWithInitData = 1;
    FifoDefCfg.InitData.InitCondition = AMP_FIFO_CFG_INIT_WITH_NUM_FRAME;
    if (*FifoHdlr == TextTrackPriFifoHdlr_Eventrecord ){
        FifoDefCfg.InitData.InitParam.NumFrame = (RealOnCreateTimeLength/VideoBitsBuffer.FrameRateScale);
    }else{
        FifoDefCfg.InitData.InitParam.NumFrame = (RealOnCreateTimeLengthSec/VideoBitsBuffer.FrameRateScale);
    }
    FifoDefCfg.InitData.FristFrameType = AMP_FIFO_TYPE_MJPEG_FRAME;
    AmpFifo_Create(&FifoDefCfg, FifoHdlr);

    return AMP_OK;
}

static int AppLibFormatMuxEvent_TextTrack_UnInit(AMP_FIFO_HDLR_s *FifoHdlr)
{
    /* To release resource. */
    AmpFifo_Delete(FifoHdlr);

    return AMP_OK;
}

/**
 *  @brief Stop the EventRecord muxer
 *
 *  Stop the EventRecord muxer
 *
 *  @return >=0 success, <0 failure
 */
int AppLibFormatMuxMp4_Event_End(void)
{
    int ReturnValue = 0;
    if (AmpMuxer_Stop(MuxMp4MuxPipe_Eventrecord) == AMP_OK){
        return ReturnValue;
    }
    else {
        AmbaPrintColor(RED,"[Applib - Format] <Mp4MuxCloseEventRecord> AppLibFormatMuxMp4_Event_End fail");
    }
    return ReturnValue;
}

/**
 *  @brief Close the EventRecord muxer
 *
 *  Close the EventRecord muxer
 *
 *  @return >=0 success, <0 failure
 */
int AppLibFormatMuxMp4_Close_EventRecord(void)
{
    int ReturnValue = 0;

    AmbaPrint("[Applib - Format] <Mp4MuxCloseEventRecord> %s delay Start", __FUNCTION__);
    ReturnValue = AmpMuxer_WaitComplete(MuxMp4MuxPipe_Eventrecord, AMBA_KAL_WAIT_FOREVER);
    AmbaPrint("[Applib - Format] <Mp4MuxCloseEventRecord> AmpMuxer_WaitComplete = %d", ReturnValue);
    AmbaPrint("[Applib - Format] <Mp4MuxCloseEventRecord> %s Start", __FUNCTION__);

    AppLibFormatMuxMp4_UnRegMuxMgr_EventRecord();

    /* Remove the Muxer pipe. */
    if (MuxMp4MuxPipe_Eventrecord!= NULL) {
        AmpMuxer_Remove(MuxMp4MuxPipe_Eventrecord);
        AmpMuxer_Delete(MuxMp4MuxPipe_Eventrecord);
        MuxMp4MuxPipe_Eventrecord= NULL;
    }

    /* Delete the Mp4 muxer. */
    if (MuxMp4FormatPri_Eventrecord!= NULL) {
        ReturnValue = AmpMp4Mux_Delete(MuxMp4FormatPri_Eventrecord);
        if (ReturnValue < 0) {
            AmbaPrintColor(RED,"[Applib - Format] <Mp4MuxCloseEventRecord> AmpMp4Mux_Delete fail ReturnValue = %d",ReturnValue);
        }
    }

    /* Delete the Mp4 muxer. */
    if (MuxMp4IndexPri_Eventrecord!= NULL) {
        AppLibIndex_DeleteHdlr(MuxMp4IndexPri_Eventrecord);
        MuxMp4IndexPri_Eventrecord = NULL;
    }
    MuxMp4FormatPri_Eventrecord = NULL;

    if (MuxMp4VideoPriFifoHdlr_Eventrecord!= NULL) {
        AmpFifo_Delete(MuxMp4VideoPriFifoHdlr_Eventrecord);
        MuxMp4VideoPriFifoHdlr_Eventrecord= NULL;
    }
#ifdef CONFIG_APP_ARD
    if (AppLibVideoEnc_GetDualStreams() && AppLibFormat_GetDualFileSaving()) {
#endif
		if (MuxMp4VideoSecFifoHdlr_Eventrecord!= NULL) {
       	 AmpFifo_Delete(MuxMp4VideoSecFifoHdlr_Eventrecord);
       	 MuxMp4VideoSecFifoHdlr_Eventrecord= NULL;
    	}
#ifdef CONFIG_APP_ARD
    }
#endif
    if (MuxMp4AudioPriFifoHdlr_Eventrecord!= NULL) {
        AmpFifo_Delete(MuxMp4AudioPriFifoHdlr_Eventrecord);
        MuxMp4AudioPriFifoHdlr_Eventrecord= NULL;
    }
#ifdef CONFIG_APP_ARD
    if (AppLibVideoEnc_GetDualStreams() && AppLibFormat_GetDualFileSaving()) {
#endif
    	if (MuxMp4AudioSecFifoHdlr_Eventrecord!= NULL) {
       		 AmpFifo_Delete(MuxMp4AudioSecFifoHdlr_Eventrecord);
   	    	 MuxMp4AudioSecFifoHdlr_Eventrecord= NULL;
    	}
#ifdef CONFIG_APP_ARD
    }
#endif
    /* Close and delete the stream. */
    if (MuxMp4StreamPri_Eventrecord!= NULL) {
        MuxMp4StreamPri_Eventrecord->Func->Close(MuxMp4StreamPri_Eventrecord);
        AmpFileStream_Delete(MuxMp4StreamPri_Eventrecord);
        MuxMp4StreamPri_Eventrecord= NULL;
    }

    /* Release the movie information. */
    if (MuxMp4MoviePri_Eventrecord!= NULL) {
        AmpFormat_RelMovieInfo(MuxMp4MoviePri_Eventrecord, TRUE);
        MuxMp4MoviePri_Eventrecord= NULL;
    }
#ifdef CONFIG_APP_ARD
    if (AppLibVideoEnc_GetDualStreams() && AppLibFormat_GetDualFileSaving()) {
#endif
    /* Delete the Mp4 muxer. */
    if (MuxMp4FormatSec_Eventrecord!= NULL) {
        ReturnValue = AmpMp4Mux_Delete(MuxMp4FormatSec_Eventrecord);
        if (ReturnValue < 0) {
            AmbaPrintColor(RED,"[Applib - Format] <Mp4MuxCloseEventRecord> AmpMp4Mux_Delete fail ReturnValue = %d",ReturnValue);
        }
    }

    /* Delete the Mp4 muxer. */
    if (MuxMp4IndexSec_Eventrecord!= NULL) {
        AppLibIndex_DeleteHdlr(MuxMp4IndexSec_Eventrecord);
        MuxMp4IndexSec_Eventrecord= NULL;
    }
    MuxMp4FormatSec_Eventrecord= NULL;


    /* Close and delete the stream. */
    if (MuxMp4StreamSec_Eventrecord!= NULL) {
        MuxMp4StreamSec_Eventrecord->Func->Close(MuxMp4StreamSec_Eventrecord);
        AmpFileStream_Delete(MuxMp4StreamSec_Eventrecord);
        MuxMp4StreamSec_Eventrecord= NULL;
    }

    /* Release the movie information. */
    if (MuxMp4MovieSec_Eventrecord!= NULL) {
        AmpFormat_RelMovieInfo(MuxMp4MovieSec_Eventrecord, TRUE);
        MuxMp4MovieSec_Eventrecord= NULL;
    }
#ifdef CONFIG_APP_ARD
    }
#endif
    // Text track
    if (TextTrackPriFifoHdlr_Eventrecord!= NULL) {
        AppLibFormatMuxEvent_TextTrack_UnInit(TextTrackPriFifoHdlr_Eventrecord);
        TextTrackPriFifoHdlr_Eventrecord= NULL;
    }
#ifdef CONFIG_APP_ARD
    if (AppLibVideoEnc_GetDualStreams() && AppLibFormat_GetDualFileSaving()) {
#endif
    	if (TextTrackSecFifoHdlr_Eventrecord!= NULL) {
       	 AppLibFormatMuxEvent_TextTrack_UnInit(TextTrackSecFifoHdlr_Eventrecord);
       	 TextTrackSecFifoHdlr_Eventrecord= NULL;
    	}
#ifdef CONFIG_APP_ARD
    }
#endif

    AmbaPrint("[Applib - Format] <Mp4MuxCloseEventRecord> %s Stop", __FUNCTION__);
    return 0;
}


/**
 *  @brief EventRecord muxer's call back function
 *
 *  EventRecord muxer's call back function
 *
 *  @param [in] hdlr Handler
 *  @param [in] event Event
 *  @param [in] info Infomation
 *
 *  @return >=0 success, <0 failure
 */
static int AppLibFormatMuxMp4_MuxCB_EventRecord(void* hdlr, UINT32 event, void* info)
{
    int ReturnValue = 0;

    switch (event) {
    case AMP_MUXER_EVENT_START:
        //AppLibComSvcHcmgr_SendMsgNoWait(HMSG_MUXER_START, 0, 0);
        DBGMSG("[Applib - Format] <Mp4Mux_MuxCB_EventRecord> AMP_MUXER_EVENT_START");
        break;
    case AMP_MUXER_EVENT_END:
        event_flag = 0;
        AppLibComSvcHcmgr_SendMsgNoWait(HMSG_MUXER_END_EVENTRECORD, 0, 0);
        DBGMSG("[Applib - Format] <Mp4Mux_MuxCB_EventRecord> AMP_MUXER_EVENT_END");
        break;
    case AMP_MUXER_EVENT_REACH_LIMIT:
        AppLibComSvcHcmgr_SendMsgNoWait(HMSG_MUXER_REACH_LIMIT_EVENTRECORD, 0, 0);
        DBGMSG("[Applib - Format] <Mp4Mux_MuxCB_EventRecord> AMP_MUXER_EVENT_REACH_LIMIT");
        break;
    case AMP_MUXER_EVENT_IO_ERROR:
#ifdef CONFIG_APP_ARD
        event_flag = 0;
        AppLibComSvcHcmgr_SendMsgNoWait(HMSG_MUXER_IO_ERROR_EVENTRECORD, 0, 0);
#endif
        //AppLibComSvcHcmgr_SendMsgNoWait(HMSG_MUXER_IO_ERROR, 0, 0);
        AmbaPrint("[Applib - Format] <Mp4Mux_MuxCB_EventRecord> AMP_MUXER_EVENT_IO_ERROR");
        break;
    case AMP_MUXER_EVENT_FIFO_ERROR:
        //AppLibComSvcHcmgr_SendMsgNoWait(HMSG_MUXER_FIFO_ERROR, 0, 0);
        AmbaPrint("[Applib - Format] <Mp4Mux_MuxCB_EventRecord> AMP_MUXER_EVENT_FIFO_ERROR");
        break;
    case AMP_MUXER_EVENT_GENERAL_ERROR:
        //AppLibComSvcHcmgr_SendMsgNoWait(HMSG_MUXER_GENERAL_ERROR, 0, 0);
        AmbaPrint("[Applib - Format] <Mp4Mux_MuxCB_EventRecord> AMP_MUXER_EVENT_GENERAL_ERROR");
        break;
    default:
        AmbaPrint("[Applib - Format] <Mp4Mux_MuxCB_EventRecord> Unknown event %X info: %x", event, info);
        break;
    }
    return ReturnValue;
}

/**
 *  @brief Start the EventRecord muxer
 *
 *  Start the EventRecord muxer
 *
 *  @return >=0 success, <0 failure
 */
int AppLibFormatMuxMp4_Open_EventRecord(void)
{
    int ReturnValue = 0;
    DBGMSG("[Applib - Format] <Mp4MuxOpenEventRecord> %s Open", __FUNCTION__);

    if (event_flag == 0)
        return ReturnValue;
    if (MuxMp4StreamPri_Eventrecord== NULL) {
        AMP_FILE_STREAM_CFG_s FileCfg;
        memset(&FileCfg, 0x0, sizeof(AMP_FILE_STREAM_CFG_s));
#ifdef CONFIG_APP_ARD
        AppLibFormatTakeMux();
#endif
#ifdef CONFIG_APP_ARD
#ifdef STORAGE_EMERGENCY_DCF_ON
        ReturnValue = AppLibStorageDmf_CreateFile(APPLIB_DCF_MEDIA_VIDEO, "MP4", EVENTRECORD_HDLR, PriFileName);
#else
        ReturnValue = AppLibStorageDmf_CreateFile(APPLIB_DCF_MEDIA_VIDEO, "MP4", DCIM_HDLR, PriFileName);
#endif
#else
        ReturnValue = AppLibStorageDmf_CreateFile(APPLIB_DCF_MEDIA_VIDEO, "MP4", EVENTRECORD_HDLR, PriFileName);
#endif
#ifdef CONFIG_APP_ARD
        AppLibFormatGiveMux();
#endif

        if (ReturnValue <= 0) {
            AmbaPrintColor(RED,"[Applib - Format] <Mp4MuxOpenEventRecord> %s:%u Get filename failure.", __FUNCTION__, __LINE__);
            return ReturnValue;
        } else {
            PriFileId = ReturnValue;
        }

        /* Open file stream */
        AmpFileStream_GetDefaultCfg(&FileCfg);
        FileCfg.Async = TRUE;
        FileCfg.BytesToSync = 0x1000;
        FileCfg.AsyncParam.MaxBank = 2;
        DBGMSG("[Applib - Format] <Mp4MuxOpenEventRecord> FileCfg.nBytesToSync = 0x%x",FileCfg.nBytesToSync);
        AmbaPrint("[Applib - Format] <Mp4MuxOpenEventRecord> FileCfg..AsyncParam.MaxBank = 0x%x",FileCfg.AsyncParam.MaxBank);
        AmpFileStream_Create(&FileCfg, &MuxMp4StreamPri_Eventrecord);
#ifdef CONFIG_APP_ARD
        AppLibFormatTakeMux();
#endif
        if (MuxMp4StreamPri_Eventrecord->Func->Open(MuxMp4StreamPri_Eventrecord, PriFileName, AMP_STREAM_MODE_WRONLY) != AMP_OK) {
            AmbaPrintColor(RED,"[Applib - Format] <Mp4MuxOpenEventRecord> %s:%u", __FUNCTION__, __LINE__);
#ifdef CONFIG_APP_ARD
            AppLibFormatGiveMux();
#endif
            return -1;
        }
#ifdef CONFIG_APP_ARD
        AppLibFormatGiveMux();
#endif
    }


    if (MuxMp4MoviePri_Eventrecord== NULL) {
        extern UINT8 *H264EncBitsBuf;
        extern UINT8 *AudPriBitsBuf;
        APPLIB_SENSOR_VIDEO_ENC_CONFIG_s *VideoEncConfigData = AppLibSysSensor_GetVideoConfig(AppLibVideoEnc_GetSensorVideoRes());
        APPLIB_VIDEOENC_GOP_s *VideoEncGOPData = AppLibSysSensor_GetVideoGOP(AppLibVideoEnc_GetSensorVideoRes());
        AMP_MUX_MOVIE_INFO_CFG_s MovieCfg;
        memset(&MovieCfg, 0x0, sizeof(AMP_MUX_MOVIE_INFO_CFG_s));


        /* Open media info */
        ReturnValue = AmpFormat_NewMovieInfo(PriFileName, &MuxMp4MoviePri_Eventrecord);
        if (ReturnValue < 0) {
            AmbaPrintColor(RED,"[Applib - Format] <Mp4MuxOpenEventRecord> AmpFormat_NewMovieInfo fail ReturnValue = %d",ReturnValue);
            return ReturnValue;
        }

        /* Set media info */
        AmpMuxer_GetDefaultMovieInfoCfg(&MovieCfg);
        MovieCfg.Track[0].TrackType = AMP_MEDIA_TRACK_TYPE_VIDEO;
        MovieCfg.Track[0].Fifo = MuxMp4VideoPriFifoHdlr_Eventrecord;
        MovieCfg.Track[0].BufferBase = H264EncBitsBuf;
        MovieCfg.Track[0].BufferLimit = (UINT8*)(H264EncBitsBuf + MuxMp4VidEncBitsFifoSize);
        MovieCfg.Track[0].MediaId = AMP_FORMAT_MID_AVC;
        MovieCfg.Track[0].TimeScale = VideoEncConfigData->EncNumerator;
        MovieCfg.Track[0].Info.Video.GOPSize = VideoEncGOPData->Idr;
        MovieCfg.Track[0].TimePerFrame = VideoEncConfigData->EncDenominator;
        MovieCfg.Track[0].Info.Video.CodecTimeScale = VideoEncConfigData->EncNumerator;
        MovieCfg.Track[0].Info.Video.Width = VideoEncConfigData->EncodeWidth;
        MovieCfg.Track[0].Info.Video.Height = VideoEncConfigData->EncodeHeight;
        MovieCfg.Track[0].Info.Video.M = VideoEncGOPData->M;
        MovieCfg.Track[0].Info.Video.N = VideoEncGOPData->N;
        MovieCfg.Track[0].Info.Video.IsDefault = TRUE;
        MovieCfg.Track[0].Info.Video.Mode = AMP_VIDEO_MODE_P;
        if (AppLibVideoEnc_GetRecMode() == REC_MODE_AV) {
            MovieCfg.TrackCount = 2;
            MovieCfg.Track[1].TrackType = AMP_MEDIA_TRACK_TYPE_AUDIO;
            MovieCfg.Track[1].Fifo = MuxMp4AudioPriFifoHdlr_Eventrecord;
            MovieCfg.Track[1].BufferBase = (UINT8 *)AudPriBitsBuf;
            MovieCfg.Track[1].BufferLimit = (UINT8 *)AudPriBitsBuf + AUDENC_VIDENC_BITSFIFO_SIZE;
            if (AppLibAudioEnc_GetEncType() == AUDIO_TYPE_AAC) {
                MovieCfg.Track[1].MediaId = AMP_FORMAT_MID_AAC;
            } else if (AppLibAudioEnc_GetEncType() == AUDIO_TYPE_PCM) {
                MovieCfg.Track[1].MediaId = AMP_FORMAT_MID_PCM;
            }
            MovieCfg.Track[1].TimeScale = AppLibAudioEnc_GetSrcSampleRate();
            MovieCfg.Track[1].Info.Audio.SampleRate = AppLibAudioEnc_GetSrcSampleRate();
            MovieCfg.Track[1].TimePerFrame = 1024;
            MovieCfg.Track[1].Info.Audio.Channels = AppLibAudioEnc_GetDstChanMode();
        } else {
            MovieCfg.TrackCount = 1;
        }

        // Text track
        if (TextTrackPriFifoHdlr_Eventrecord) {
            AppLibFormatMuxEvent_TextTrack_Configure(&MovieCfg, TextTrackPriFifoHdlr_Eventrecord);
        }

        ReturnValue = AmpMuxer_InitMovieInfo(MuxMp4MoviePri_Eventrecord, &MovieCfg);
        if (ReturnValue < 0) {
            AmbaPrintColor(RED,"[Applib - Format] <Mp4MuxOpenEventRecord> AmpMuxer_InitMovieInfo fail ReturnValue = %d",ReturnValue);
            return ReturnValue;
        } else {
#if 1
            AmbaPrintColor(GREEN,"[Applib - Format] <Mp4MuxOpenEventRecord> Main file");
            AmbaPrint("[Applib - Format] <Mp4MuxOpenEventRecord> MovieCfg.Track[0].Fifo = 0x%x",MovieCfg.Track[0].Fifo);
            AmbaPrint("[Applib - Format] <Mp4MuxOpenEventRecord> MovieCfg.Track[0].TimeScale = %d",MovieCfg.Track[0].TimeScale);
            AmbaPrint("[Applib - Format] <Mp4MuxOpenEventRecord> MovieCfg.Track[0].TimePerFrame = %d",MovieCfg.Track[0].TimePerFrame);
            AmbaPrint("[Applib - Format] <Mp4MuxOpenEventRecord> MovieCfg.Track[0].Info.Video.M = %d",MovieCfg.Track[0].Info.Video.M);
            AmbaPrint("[Applib - Format] <Mp4MuxOpenEventRecord> MovieCfg.Track[0].Info.Video.N = %d",MovieCfg.Track[0].Info.Video.N);
            AmbaPrint("[Applib - Format] <Mp4MuxOpenEventRecord> MovieCfg.Track[0].Info.Video.GOPSize = %d",MovieCfg.Track[0].Info.Video.GOPSize);
            AmbaPrint("[Applib - Format] <Mp4MuxOpenEventRecord> MovieCfg.Track[0].Info.Video.Width = %d",MovieCfg.Track[0].Info.Video.Width);
            AmbaPrint("[Applib - Format] <Mp4MuxOpenEventRecord> MovieCfg.Track[0].Info.Video.Height = %d",MovieCfg.Track[0].Info.Video.Height);
            AmbaPrint("[Applib - Format] <Mp4MuxOpenEventRecord> MovieCfg.Track[0].Info.Video.CodecTimeScale = %d",MovieCfg.Track[0].Info.Video.CodecTimeScale);
            AmbaPrint("[Applib - Format] <Mp4MuxOpenEventRecord> MovieCfg.Track[1].MediaId = %d",MovieCfg.Track[1].MediaId);
            AmbaPrint("[Applib - Format] <Mp4MuxOpenEventRecord> MovieCfg.Track[1].Info.Audio.SampleRate = %d",MovieCfg.Track[1].Info.Audio.SampleRate);
            AmbaPrint("[Applib - Format] <Mp4MuxOpenEventRecord> MovieCfg.Track[1].Info.Audio.Channels = %d",MovieCfg.Track[1].Info.Audio.Channels);
#endif
        }
    }

    /* Create Index */
    if (MuxMp4IndexPri_Eventrecord== NULL) {
        AppLibIndex_CreateHdlr(&MuxMp4IndexPri_Eventrecord);
    }

    if (MuxMp4FormatPri_Eventrecord== NULL) {
        AMP_MP4_MUX_CFG_s Mp4Cfg;
        AmbaPrint("[Applib - Format] <Mp4MuxOpenEventRecord> Mp4 Mux Create");
        AmpMp4Mux_GetDefaultCfg(&Mp4Cfg);

        Mp4Cfg.Stream = MuxMp4StreamPri_Eventrecord;
        Mp4Cfg.Index = MuxMp4IndexPri_Eventrecord;
        Mp4Cfg.TrickRecDivisor = 1;
        if (AmpMp4Mux_Create(&Mp4Cfg, &MuxMp4FormatPri_Eventrecord) != AMP_OK) {
            AmbaPrintColor(RED,"[Applib - Format] <Mp4MuxOpenEventRecord> %s:%u", __FUNCTION__, __LINE__);
            return -1;
        }
    }


    if (AppLibVideoEnc_GetDualStreams() && AppLibFormat_GetDualFileSaving()) {
        if (MuxMp4StreamSec_Eventrecord== NULL) {
            AMP_FILE_STREAM_CFG_s FileCfg;
            memset(&FileCfg, 0x0, sizeof(AMP_FILE_STREAM_CFG_s));

#ifdef CONFIG_APP_ARD
#ifdef STORAGE_EMERGENCY_DCF_ON
            ReturnValue = AppLibStorageDmf_CreateFileExtended(APPLIB_DCF_MEDIA_VIDEO, PriFileId, "MP4", APPLIB_DCF_EXT_OBJECT_VIDEO_THM, 0, EVENTRECORD_HDLR, SecFileName);
#else
			ReturnValue = AppLibStorageDmf_CreateFileExtended(APPLIB_DCF_MEDIA_VIDEO, PriFileId, "MP4", APPLIB_DCF_EXT_OBJECT_VIDEO_THM, 0, DCIM_HDLR, SecFileName);
#endif
#else
			ReturnValue = AppLibStorageDmf_CreateFileExtended(APPLIB_DCF_MEDIA_VIDEO, PriFileId, "MP4", APPLIB_DCF_EXT_OBJECT_VIDEO_THM, 0, EVENTRECORD_HDLR, SecFileName);
#endif
			if (ReturnValue < 0) {
                AmbaPrintColor(RED,"[Applib - Format] <Mp4MuxOpenEventRecord> %s:%u Get filename failure.", __FUNCTION__, __LINE__);
                return ReturnValue;
            } else {
                SecFileId = PriFileId;
            }
            /* Open file stream */
            AmpFileStream_GetDefaultCfg(&FileCfg);
            FileCfg.Async = TRUE;
            FileCfg.BytesToSync = 0x1000;
            FileCfg.AsyncParam.MaxBank = 2;
            ReturnValue = AmpFileStream_Create(&FileCfg, &MuxMp4StreamSec_Eventrecord);
            if (ReturnValue < 0) {
                AmbaPrintColor(RED,"[Applib - Format] <Mp4MuxOpenEventRecord> AmpFileStream_Create fail %s:%u", __FUNCTION__, __LINE__);
                return ReturnValue;
            }  else {
#ifdef CONFIG_APP_ARD
            AppLibFormatTakeMux();
#endif
                if (MuxMp4StreamSec_Eventrecord->Func->Open(MuxMp4StreamSec_Eventrecord, SecFileName, AMP_STREAM_MODE_WRONLY) != AMP_OK) {
                    AmbaPrintColor(RED,"[Applib - Format] <Mp4MuxOpenEventRecord> %s:%u", __FUNCTION__, __LINE__);
#ifdef CONFIG_APP_ARD
                    AppLibFormatGiveMux();
#endif
                    return -1;
                }
#ifdef CONFIG_APP_ARD
                AppLibFormatGiveMux();
#endif
            }
        }

        if (MuxMp4MovieSec_Eventrecord== NULL) {
            extern UINT8 *H264EncBitsBuf;
            AMP_MUX_MOVIE_INFO_CFG_s MovieCfg;

            memset(&MovieCfg, 0x0, sizeof(AMP_MUX_MOVIE_INFO_CFG_s));

            /* Open media info */
            ReturnValue = AmpFormat_NewMovieInfo(SecFileName, &MuxMp4MovieSec_Eventrecord);
            if (ReturnValue < 0) {
                AmbaPrintColor(RED,"[Applib - Format] <Mp4MuxOpenEventRecord> AmpFormat_NewMovieInfo fail ReturnValue = %d",ReturnValue);
                return ReturnValue;
            }

            /* Set media info */
            AmpMuxer_GetDefaultMovieInfoCfg(&MovieCfg);

            MovieCfg.Track[0].TrackType = AMP_MEDIA_TRACK_TYPE_VIDEO;
            MovieCfg.Track[0].Fifo = MuxMp4VideoSecFifoHdlr_Eventrecord;
            MovieCfg.Track[0].BufferBase = H264EncBitsBuf;
            MovieCfg.Track[0].BufferLimit = (UINT8*)(H264EncBitsBuf + MuxMp4VidEncBitsFifoSize);
            MovieCfg.Track[0].MediaId = AMP_FORMAT_MID_AVC;
            MovieCfg.Track[0].TimeScale = AppLibVideoEnc_GetSecStreamTimeScale();
            MovieCfg.Track[0].TimePerFrame = AppLibVideoEnc_GetSecStreamTick();
            MovieCfg.Track[0].Info.Video.Mode = AMP_VIDEO_MODE_P;
            MovieCfg.Track[0].Info.Video.M = AppLibVideoEnc_GetSecStreamGopM();
            MovieCfg.Track[0].Info.Video.N = AppLibVideoEnc_GetSecStreamGopN();
            MovieCfg.Track[0].Info.Video.GOPSize = AppLibVideoEnc_GetSecStreamGopIDR();
            MovieCfg.Track[0].Info.Video.CodecTimeScale = MovieCfg.Track[0].TimeScale;
            MovieCfg.Track[0].Info.Video.Width = AppLibVideoEnc_GetSecStreamW();
            MovieCfg.Track[0].Info.Video.Height = AppLibVideoEnc_GetSecStreamH();
            MovieCfg.Track[0].Info.Video.IsDefault = TRUE;
            if (AppLibVideoEnc_GetRecMode() == REC_MODE_AV) {
                MovieCfg.TrackCount = 2;
                MovieCfg.Track[1].TrackType = AMP_MEDIA_TRACK_TYPE_AUDIO;
                MovieCfg.Track[1].Fifo = MuxMp4AudioSecFifoHdlr_Eventrecord;
                if (AppLibAudioEnc_GetDualStreams()) {
                    extern UINT8 *AudSecBitsBuf;
                    MovieCfg.Track[1].BufferBase = (UINT8 *)AudSecBitsBuf;
                    MovieCfg.Track[1].BufferLimit = (UINT8 *)AudSecBitsBuf + AUDENC_VIDENC_BITSFIFO_SIZE;
                } else {
                    extern UINT8 *AudPriBitsBuf;
                    MovieCfg.Track[1].BufferBase = (UINT8 *)AudPriBitsBuf;
                    MovieCfg.Track[1].BufferLimit = (UINT8 *)AudPriBitsBuf + AUDENC_VIDENC_BITSFIFO_SIZE;
                }
                if (AppLibAudioEnc_GetEncType() == AUDIO_TYPE_AAC) {
                    MovieCfg.Track[1].MediaId = AMP_FORMAT_MID_AAC;
                } else if (AppLibAudioEnc_GetEncType() == AUDIO_TYPE_PCM) {
                    MovieCfg.Track[1].MediaId = AMP_FORMAT_MID_PCM;
                }
                MovieCfg.Track[1].Info.Audio.SampleRate = AppLibAudioEnc_GetSrcSampleRate();
                MovieCfg.Track[1].TimeScale = AppLibAudioEnc_GetSrcSampleRate();
                MovieCfg.Track[1].TimePerFrame = 1024;
                MovieCfg.Track[1].Info.Audio.Channels = AppLibAudioEnc_GetDstChanMode();
            } else {
                MovieCfg.TrackCount = 1;
            }

            // Text track
            if (TextTrackSecFifoHdlr_Eventrecord) {
                AppLibFormatMuxEvent_TextTrack_Configure(&MovieCfg, TextTrackSecFifoHdlr_Eventrecord);
            }

            ReturnValue = AmpMuxer_InitMovieInfo(MuxMp4MovieSec_Eventrecord, &MovieCfg);
            if (ReturnValue < 0) {
                AmbaPrintColor(RED,"[Applib - Format] <Mp4MuxOpenEventRecord> AmpMuxer_InitMovieInfo fail ReturnValue = %d",ReturnValue);
                return ReturnValue;
            } else {
#if 1
                AmbaPrintColor(GREEN,"[Applib - Format] <Mp4MuxOpen> Sec file");
                AmbaPrint("[Applib - Format] <Mp4MuxOpenEventRecord> MovieCfg.Track[0].Fifo = 0x%x",MovieCfg.Track[0].Fifo);
                AmbaPrint("[Applib - Format] <Mp4MuxOpenEventRecord> MovieCfg.Track[0].Info.Video.TimeScale = %d",MovieCfg.Track[0].TimeScale);
                AmbaPrint("[Applib - Format] <Mp4MuxOpenEventRecord> MovieCfg.Track[0].Info.Video.TimePerFrame = %d",MovieCfg.Track[0].TimePerFrame);
                AmbaPrint("[Applib - Format] <Mp4MuxOpenEventRecord> MovieCfg.Track[0].Info.Video.M = %d",MovieCfg.Track[0].Info.Video.M);
                AmbaPrint("[Applib - Format] <Mp4MuxOpenEventRecord> MovieCfg.Track[0].Info.Video.N = %d",MovieCfg.Track[0].Info.Video.N);
                AmbaPrint("[Applib - Format] <Mp4MuxOpenEventRecord> MovieCfg.Track[0].Info.Video.GOPSize = %d",MovieCfg.Track[0].Info.Video.GOPSize);
                AmbaPrint("[Applib - Format] <Mp4MuxOpenEventRecord> MovieCfg.Track[0].Info.Video.Width = %d",MovieCfg.Track[0].Info.Video.Width);
                AmbaPrint("[Applib - Format] <Mp4MuxOpenEventRecord> MovieCfg.Track[0].Info.Video.Height = %d",MovieCfg.Track[0].Info.Video.Height);
#endif
            }
        }
        /* Create Index */
        if (MuxMp4IndexSec_Eventrecord== NULL) {
            AppLibIndex_CreateHdlr(&MuxMp4IndexSec_Eventrecord);
        }
        if (MuxMp4FormatSec_Eventrecord== NULL) {
            AMP_MP4_MUX_CFG_s Mp4Cfg;
            AmbaPrint("[Applib - Format] <Mp4MuxOpenEventRecord> Mp4 Mux Create");
            AmpMp4Mux_GetDefaultCfg(&Mp4Cfg);

            Mp4Cfg.Stream = MuxMp4StreamSec_Eventrecord;
            Mp4Cfg.Index = MuxMp4IndexSec_Eventrecord;
            Mp4Cfg.TrickRecDivisor = 1;
            if (AmpMp4Mux_Create(&Mp4Cfg, &MuxMp4FormatSec_Eventrecord) != AMP_OK) {
                AmbaPrintColor(RED,"[Applib - Format] <Mp4MuxOpenEventRecord> %s:%u", __FUNCTION__, __LINE__);
                return -1;
            }
        }
    }


    /* Create muxer pipe */
    if (MuxMp4MuxPipe_Eventrecord== NULL) {
        AMP_MUXER_PIPE_CFG_s MuxPipeCfg;
        AmpMuxer_GetDefaultCfg(&MuxPipeCfg);
#ifdef CONFIG_APP_ARD
		MuxPipeCfg.ProcParam = 15000;
#endif
        MuxPipeCfg.Format[0] = MuxMp4FormatPri_Eventrecord;
        MuxPipeCfg.Media[0] = (AMP_MEDIA_INFO_s *)MuxMp4MoviePri_Eventrecord;
        if (AppLibVideoEnc_GetDualStreams() && AppLibFormat_GetDualFileSaving()) {
            MuxPipeCfg.FormatCount = 2;
            MuxPipeCfg.Format[1] = MuxMp4FormatSec_Eventrecord;
            MuxPipeCfg.Media[1] = (AMP_MEDIA_INFO_s *)MuxMp4MovieSec_Eventrecord;
        } else {
            MuxPipeCfg.FormatCount = 1;
        }

        MuxMp4SplitTime_Eventrecord = EventFileLength;
        MuxMp4SplitSize_Eventrecord = (UINT64)3750 * (UINT64)1024 * (UINT64)1024;

        MuxPipeCfg.MaxDuration = MuxMp4SplitTime_Eventrecord;
        MuxPipeCfg.MaxSize= MuxMp4SplitSize_Eventrecord;

        MuxPipeCfg.OnEvent = AppLibFormatMuxMp4_MuxCB_EventRecord;
        AmpMuxer_Create(&MuxPipeCfg, &MuxMp4MuxPipe_Eventrecord);
        AmpMuxer_Add(MuxMp4MuxPipe_Eventrecord);
        AmpMuxer_Start(MuxMp4MuxPipe_Eventrecord, 0);
#ifdef CONFIG_APP_ARD
        AppLibComSvcHcmgr_SendMsgNoWait(HMSG_MUXER_OPEN_EVENTRECORD, 0, 0);
#endif
    }

    DBGMSG("[Applib - Format] <Mp4MuxOpenEventRecord> %s End", __FUNCTION__);
    return 0;
}

/**
 *  @brief Unregister the callback function in the EventRecord muxer manager.
 *
 *  unregister the callback function in the EventRecord muxer manager.
 *
 *  @return >=0 success, <0 failure
 */
int AppLibFormatMuxMp4_UnRegMuxMgr_EventRecord(void)
{
    APPLIB_MUX_MGR_HANDLER_s Handler = {0};
    memset(&Handler, 0x0, sizeof(APPLIB_MUX_MGR_HANDLER_s));
    Handler.MuxerInit = NULL;
    Handler.MuxerOpen = &AppLibFormatMuxMp4_Open_EventRecord;
    Handler.MuxerClose = &AppLibFormatMuxMp4_Close_EventRecord;
    Handler.DataReadyNum = 1;
    Handler.Used = 0;
    Handler.Type = VIDEO_EVENTRECORD_MUXER_HANDLER;

    return AppLibFormatMuxMgr_UnRegMuxHandler(&Handler);
}

/**
 *  @brief Register the callback function in the EventRecord muxer manager.
 *
 *  Register the callback function in the EventRecord muxer manager.
 *
 *  @return >=0 success, <0 failure
 */
int AppLibFormatMuxMp4_RegMuxMgr_EventRecord(void)
{
    APPLIB_MUX_MGR_HANDLER_s Handler = {0};

    DBGMSG("[Applib - Format] <MuxMp4_RegMuxMgr_EventRecord>");
    memset(&Handler, 0x0, sizeof(APPLIB_MUX_MGR_HANDLER_s));
    Handler.MuxerInit = NULL;//&AppLibFormatMp4Mux_Init;
    Handler.MuxerOpen = &AppLibFormatMuxMp4_Open_EventRecord;
    Handler.MuxerClose = &AppLibFormatMuxMp4_Close_EventRecord;
    Handler.DataReadyNum = 1;
    Handler.Used = 1;
    Handler.Type = VIDEO_EVENTRECORD_MUXER_HANDLER;

    return AppLibFormatMuxMgr_RegMuxHandler(&Handler);
}

//static int FrameNumberPri = 0;
//static int FrameNumberSec = 0;
/**
 *  @brief EventRecord muxer's fifo call back function
 *
 *  EventRecord muxer's fifo call back function
 *
 *  @param [in] hdlr Handler
 *  @param [in] event Event
 *  @param [in] info Infomation
 *
 *  @return >=0 success, <0 failure
 */
int  AppLibFormatMuxMp4_VideoFifoCB_EventRecord(void *hdlr, UINT32 event, void* info)
{
    //AmbaPrintColor(GREEN,"[Applib - Format] <Mp4Mux_VideoFifoCB>: 0x%x",event);
    if (0) {
        AMP_FIFO_INFO_s tmpInfo;
        AmpFifo_GetInfo((AMP_FIFO_HDLR_s *)hdlr, &tmpInfo);
        //AmbaPrintColor(GREEN,"[Applib - Format] <Mp4Mux_VideoFifoCB>: TotalEntries = %d, AvailEntries=%d",tmpInfo.TotalEntries,tmpInfo.AvailEntries);
    }
#if 0
    if (MuxMp4VideoPriFifoHdlr == hdlr) {
        FrameNumberPri ++ ;
    } else if (MuxMp4VideoSecFifoHdlr == hdlr) {
        FrameNumberSec ++ ;
    } else {
        AmbaPrintColor(GREEN,"[Applib - Format] <Mp4Mux_VideoFifoCB>: 0x%x",event);
    }
    AmbaPrintColor(GREEN,"[Applib - Format] <Mp4Mux_VideoFifoCB>: FrameNumberPri = %d, FrameNumberSec = %d",FrameNumberPri,FrameNumberSec);
#endif

    switch (event) {
    case AMP_FIFO_EVENT_DATA_READY:
        AppLibFormatMuxMgr_DataReady(hdlr, info, VIDEO_EVENTRECORD_MUXER_HANDLER);
        //AppLibFormatMuxMp4_Open();
        //AmbaPrintColor(GREEN,"[Applib - Format] <Mp4Mux_VideoFifoCB>: AMP_FIFO_EVENT_DATA_READY hdlr = 0x%x",hdlr);
        //AmpMuxer_OnDataReady((AMP_FIFO_HDLR_s *)hdlr);
        break;
    case AMP_FIFO_EVENT_DATA_EOS:
        //AmbaPrintColor(YELLOW,"[Applib - Format] <Mp4Mux_VideoFifoCB>: AMP_FIFO_EVENT_DATA_EOS hdlr = 0x%x",hdlr);
        AppLibFormatMuxMgr_DataEos(hdlr, info);
        //AmpMuxer_OnEOS((AMP_FIFO_HDLR_s *)hdlr);
        break;
    default:
        AmbaPrint("[Applib - Format] <Mp4Mux_VideoFifoCB_EventRecord>: evnet 0x%x", event);
        break;
    }

    return 0;
}

//static int FrameAudioNumberPri = 0;
//static int FrameAudioNumberSec = 0;
/**
 *  @brief EventRecord muxer's fifo call back function
 *
 *  EventRecord muxer's fifo call back function
 *
 *  @param [in] hdlr Handler
 *  @param [in] event Event
 *  @param [in] info Infomation
 *
 *  @return >=0 success, <0 failure
 */
int  AppLibFormatMuxMp4_AudioFifoCB_EventRecord(void *hdlr, UINT32 event, void* info)
{

    //AmbaPrintColor(MAGENTA,"[Applib - Format] <Mp4Mux_AudioFifoCB>: 0x%x",event);
    if (0) {
        AMP_FIFO_INFO_s tmpInfo;
        AmpFifo_GetInfo((AMP_FIFO_HDLR_s *)hdlr, &tmpInfo);
        //AmbaPrintColor(MAGENTA,"[Applib - Format] <Mp4Mux_AudioFifoCB>: TotalEntries = %d, AvailEntries=%d",tmpInfo.TotalEntries,tmpInfo.AvailEntries);
    }
#if 0
    if (MuxMp4AudioPriFifoHdlr == hdlr) {
        FrameAudioNumberPri ++ ;
    } else if (MuxMp4AudioSecFifoHdlr == hdlr) {
        FrameAudioNumberSec ++ ;
    } else {
        AmbaPrintColor(GREEN,"[Applib - Format] <Mp4Mux_AduioFifoCB>: 0x%x",event);
    }
    AmbaPrintColor(GREEN,"[Applib - Format] <Mp4Mux_AduioFifoCB>: FrameNumberPri = %d, FrameNumberSec = %d",FrameAudioNumberPri,FrameAudioNumberSec);
#endif
    switch (event) {
    case AMP_FIFO_EVENT_DATA_READY:
        AppLibFormatMuxMgr_DataReady(hdlr, info, VIDEO_EVENTRECORD_MUXER_HANDLER);
        //AppLibFormatMuxMp4_Open();
        //AmbaPrint("[Applib - Format] <Mp4Mux_AudioFifoCB>: AMP_FIFO_EVENT_DATA_READY");
        //AmpMuxer_OnDataReady((AMP_FIFO_HDLR_s *)hdlr);
        break;
    case AMP_FIFO_EVENT_DATA_EOS:
        AppLibFormatMuxMgr_DataEos(hdlr, info);
        //AmbaPrint("[Applib - Format] <Mp4Mux_AudioFifoCB>: AMP_FIFO_EVENT_DATA_EOS");
        //AmpMuxer_OnEOS((AMP_FIFO_HDLR_s *)hdlr);
        break;
    default:
        AmbaPrint("[Applib - Format] <Mp4Mux_AudioFifoCB_EventRecord>: evnet 0x%x", event);
        break;
    }

    return 0;
}


/**
 *  @brief Start the eventrecord muxer after starting record
 *
 *  Start the eventrecord muxer
 *
 *  @return >=0 success, <0 failure
 */
int AppLibFormatMuxMp4_StartOnRecording_EventRecord(int PreEventTime)
{
    int ReturnValue = 0;
    UINT64 VideoFirstFramePtsPri = 0;
    UINT64 VideoFirstFramePtsSec = 0;

    event_flag = 1;
    AmbaPrint("[Applib - Format] <StartOnRecordingEventRecord> %s Start", __FUNCTION__);
    AppLibRecorderMemMgr_GetBufSize(&MuxMp4VidEncBitsFifoSize, &MuxMp4VidEncDescSize);

    {
        //AppLibFormatMp4Mux_Init();//There is no need to init it again when start eventrecord
        //AppLibFormatMuxMgr_Init();//There is no need to init it again when start eventrecord
        AppLibFormatMuxMp4_RegMuxMgr_EventRecord();

        /* Create a virtual fifo for primary stream. */
        if (MuxMp4VideoPriFifoHdlr_Eventrecord== NULL) {
            extern AMP_AVENC_HDLR_s *VideoEncPri;
            AMP_FIFO_CFG_s FifoDefCfg;
            APPLIB_SENSOR_VIDEO_ENC_CONFIG_s *VideoEncConfigData = AppLibSysSensor_GetVideoConfig(AppLibVideoEnc_GetSensorVideoRes());
            memset(&FifoDefCfg, 0x0, sizeof(AMP_FIFO_CFG_s));
            AmpFifo_GetDefaultCfg(&FifoDefCfg);
            AmbaPrint("[Applib - Format] <Mp4MuxStart> VideoEncPri = 0x%08X",VideoEncPri);
            FifoDefCfg.hCodec = VideoEncPri;
            FifoDefCfg.IsVirtual = 1;
            FifoDefCfg.NumEntries = 8192;
            FifoDefCfg.cbEvent = AppLibFormatMuxMp4_VideoFifoCB_EventRecord;
            FifoDefCfg.InitData.CreateFifoWithInitData = 1; //--
            FifoDefCfg.InitData.InitCondition = AMP_FIFO_CFG_INIT_WITH_NUM_FRAME; //--
            FifoDefCfg.InitData.InitParam.NumFrame = ((VideoEncConfigData->EncNumerator*PreEventTime)/VideoEncConfigData->EncDenominator); //--
            FifoDefCfg.InitData.FristFrameType = AMP_FIFO_TYPE_IDR_FRAME; //--
            FifoDefCfg.TickPerSecond = VideoEncConfigData->EncNumerator;
            AmpFifo_Create(&FifoDefCfg, &MuxMp4VideoPriFifoHdlr_Eventrecord);
            VideoFirstFramePtsPri = FifoDefCfg.InitData.OnCreateFirstFramePts;
            RealOnCreateTimeLength = FifoDefCfg.InitData.OnCreateTimeLength;
        }
        if (AppLibVideoEnc_GetRecMode() == REC_MODE_AV) {
            if (MuxMp4AudioPriFifoHdlr_Eventrecord== NULL) {
                APPLIB_SENSOR_VIDEO_ENC_CONFIG_s *VideoEncConfigData = AppLibSysSensor_GetVideoConfig(AppLibVideoEnc_GetSensorVideoRes());
                extern AMP_AVENC_HDLR_s *AudioEncPriHdlr;
                AMP_FIFO_CFG_s FifoDefCfg;
                AmpFifo_GetDefaultCfg(&FifoDefCfg);
                FifoDefCfg.hCodec = AudioEncPriHdlr;
                FifoDefCfg.IsVirtual = 1;
                FifoDefCfg.NumEntries = 8192;
                FifoDefCfg.cbEvent = AppLibFormatMuxMp4_AudioFifoCB_EventRecord;
                FifoDefCfg.InitData.CreateFifoWithInitData = 1; //--
                FifoDefCfg.InitData.InitCondition = AMP_FIFO_CFG_INIT_WITH_START_TIME; //--
                FifoDefCfg.InitData.InitParam.StartTime = (VideoFirstFramePtsPri*1000)/VideoEncConfigData->EncNumerator;   //--
                FifoDefCfg.InitData.FristFrameType = AMP_FIFO_TYPE_AUDIO_FRAME; //--
                FifoDefCfg.TickPerSecond = 90000;
                AmpFifo_Create(&FifoDefCfg, &MuxMp4AudioPriFifoHdlr_Eventrecord);
            }
        }

        //Text track
        if (TextTrackPriFifoHdlr_Eventrecord== NULL) {
            AppLibFormatMuxEvent_TextTrack_Init(&TextTrackPriFifoHdlr_Eventrecord);
        }

        /* Create a virtual fifo for secondary stream. */
        if (AppLibVideoEnc_GetDualStreams() && AppLibFormat_GetDualFileSaving()) {
            if (MuxMp4VideoSecFifoHdlr_Eventrecord== NULL) {
                extern AMP_AVENC_HDLR_s *VideoEncSec;
                AMP_FIFO_CFG_s FifoDefCfg;
                AmpFifo_GetDefaultCfg(&FifoDefCfg);
                FifoDefCfg.hCodec = VideoEncSec;
                FifoDefCfg.IsVirtual = 1;
                FifoDefCfg.NumEntries = 8192;
                FifoDefCfg.cbEvent = AppLibFormatMuxMp4_VideoFifoCB_EventRecord;
                FifoDefCfg.InitData.CreateFifoWithInitData = 1; //--
                FifoDefCfg.InitData.InitCondition = AMP_FIFO_CFG_INIT_WITH_NUM_FRAME;    //--
                FifoDefCfg.InitData.InitParam.NumFrame = ((AppLibVideoEnc_GetSecStreamTimeScale()*PreEventTime)/AppLibVideoEnc_GetSecStreamTick());   //--
                FifoDefCfg.InitData.FristFrameType = AMP_FIFO_TYPE_IDR_FRAME; //--
                FifoDefCfg.TickPerSecond = AppLibVideoEnc_GetSecStreamTimeScale();
                AmpFifo_Create(&FifoDefCfg, &MuxMp4VideoSecFifoHdlr_Eventrecord);
                VideoFirstFramePtsSec = FifoDefCfg.InitData.OnCreateFirstFramePts;
                RealOnCreateTimeLengthSec = FifoDefCfg.InitData.OnCreateTimeLength;
            }
            if (AppLibVideoEnc_GetRecMode() == REC_MODE_AV) {
                if (MuxMp4AudioSecFifoHdlr_Eventrecord== NULL) {
                    extern AMP_AVENC_HDLR_s *AudioEncPriHdlr;
                    extern AMP_AVENC_HDLR_s *AudioEncSecHdlr;
                    AMP_FIFO_CFG_s FifoDefCfg;
                    AmpFifo_GetDefaultCfg(&FifoDefCfg);
                    if (AppLibAudioEnc_GetDualStreams()) {
                        FifoDefCfg.hCodec = AudioEncSecHdlr;
                    } else {
                        FifoDefCfg.hCodec = AudioEncPriHdlr;
                    }
                    FifoDefCfg.IsVirtual = 1;
                    FifoDefCfg.NumEntries = 8192;
                    FifoDefCfg.cbEvent = AppLibFormatMuxMp4_AudioFifoCB_EventRecord;
                    FifoDefCfg.InitData.CreateFifoWithInitData = 1; //--
                    FifoDefCfg.InitData.InitCondition = AMP_FIFO_CFG_INIT_WITH_START_TIME; //--
                    FifoDefCfg.InitData.InitParam.StartTime = ((VideoFirstFramePtsSec*1000)/AppLibVideoEnc_GetSecStreamTimeScale());   //--
                    FifoDefCfg.InitData.FristFrameType = AMP_FIFO_TYPE_AUDIO_FRAME; //--
                    FifoDefCfg.TickPerSecond = 90000;
                    AmpFifo_Create(&FifoDefCfg, &MuxMp4AudioSecFifoHdlr_Eventrecord);
                }
            }
        }
#ifdef CONFIG_APP_ARD
		if (AppLibVideoEnc_GetDualStreams() && AppLibFormat_GetDualFileSaving()) {
#endif
	    //Text track
        if (TextTrackSecFifoHdlr_Eventrecord== NULL) {
            AppLibFormatMuxEvent_TextTrack_Init(&TextTrackSecFifoHdlr_Eventrecord);
        }
#ifdef CONFIG_APP_ARD
		}
#endif
    }
    AmbaPrint("[Applib - Format] <StartOnRecordingEventRecord> %s End", __FUNCTION__);
    return ReturnValue;
}


