/**
 * @file src/app/connected/applib/src/transcoder/video_decode/ApplibTranscoder_VideoDec.c
 *
 * Implementation of MW video transcoder utility
 *
 * History:
 *    2015/02/06 - [cichen] created file
 *
 *
 * Copyright (c) 2015 Ambarella, Inc.
 *
 * This file and its contents (¡°Software¡±) are protected by intellectual property rights
 * including, without limitation, U.S. and/or foreign copyrights.  This Software is also the
 * confidential and proprietary information of Ambarella, Inc. and its licensors.  You may
 * not use, reproduce, disclose, distribute, modify, or otherwise prepare derivative
 * works of this Software or any portion thereof except pursuant to a signed license
 * agreement or nondisclosure agreement with Ambarella, Inc. or its authorized
 * affiliates.	In the absence of such an agreement, you agree to promptly notify and
 * return this Software to Ambarella, Inc.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF NON-
 * INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL AMBARELLA, INC. OR ITS AFFILIATES BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; COMPUTER FAILURE OR
 * MALFUNCTION; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <transcoder/video_decode/ApplibTranscoder_VideoDec.h>
#include <applib.h>
#include <fifo/Fifo.h>
#include <transcoder/DecTranscoder.h>
#include <comsvc/misc/util.h>
#include <AmbaUtility.h>
#include <format/ApplibFormat_DemuxMp4.h>
#include <format/Demuxer.h>
#include <transcoder/decode_utility/ApplibTranscoder_VideoTask.h>
#include "../../AppLibTask_Priority.h"

#define TRANS_VIDEO_DBG_EN
#define TRANS_VIDEO_ERR_EN

#undef TRANS_VIDEO_DBG
#ifdef TRANS_VIDEO_DBG_EN
#define TRANS_VIDEO_DBG(fmt,args...) AmbaPrintColor(CYAN,fmt,##args);
#else
#define TRANS_VIDEO_DBG(fmt,args...)
#endif

#undef TRANS_VIDEO_ERR
#ifdef TRANS_VIDEO_ERR_EN
#define TRANS_VIDEO_ERR(fmt,args...) AmbaPrintColor(RED,fmt,##args);
#else
#define TRANS_VIDEO_ERR(fmt,args...)
#endif


/**
 * Video player state
 */
typedef enum _APPLIB_VIDEO_PLAYER_STATE_e_ {
    APPLIB_VIDEO_PLAYER_STATE_INVALID = 0,  ///< Not ready.
    APPLIB_VIDEO_PLAYER_STATE_IDLE,         ///< Ready to load a file.
    APPLIB_VIDEO_PLAYER_STATE_PLAY,         ///< Video is playing.
    APPLIB_VIDEO_PLAYER_STATE_PAUSE,        ///< Video is paused.
    APPLIB_VIDEO_PLAYER_STATE_PAUSE_CHANGE, ///< Video is paused with some features (ex. speed, start time...) changed.
    APPLIB_VIDEO_PLAYER_STATE_NUM           ///< Total number of video states.
} APPLIB_VIDEO_PLAYER_STATE_e;

#define APPLIB_TO_DEMUX_SPEED(speed)    ((speed) >> 8)


// Video variables
#define VIDEODEC_RAW_SIZE           (20<<20) // Raw buffer size
#define VIDEODEC_RAW_DESC_NUM       (512) // Descriptor default number
#define APPLIB_PLAYER_NORMAL_SPEED  (0x100) // Normal speed
#define APPLIB_PLAYER_MAX_SPEED     ((APPLIB_PLAYER_NORMAL_SPEED) << 6) // Maximum speed
#define APPLIB_PLAYER_MIN_SPEED     ((APPLIB_PLAYER_NORMAL_SPEED) >> 6) // Minimum speed


static AMP_DEC_PIPE_HDLR_s *DecPipeHdlr = NULL;

#define APPLIB_PLAYER_FILE_NUM_MAX      (128)       ///< At most 128 split files
#define APPLIB_PLAYER_INVALID_FILE_ID   (0xFF)      ///< A value bigger than APPLIB_PLAYER_FILE_NUM_MAX indicating invalid file ID
static APPLIB_VIDEO_PLAYER_STATE_e VideoPlayerState = APPLIB_VIDEO_PLAYER_STATE_INVALID;

static UINT8 IsFileOpened = 0;

static UINT32 CurrentPlayingSpeed = APPLIB_PLAYER_NORMAL_SPEED;
static UINT32 PausedSpeed = APPLIB_PLAYER_NORMAL_SPEED; // The speed before the last pause action or start action with AutoPlay = 0.

static APPLIB_VIDEO_TRANSCODE_DIRECTION_e CurrentPlayingDir = APPLIB_VIDEO_TRANSCODE_FW; // Play forward
static AMP_AVDEC_TRICKPLAY_s Trick = { 0 };
static AMP_VIDEODEC_DISPLAY_s Display = { 0 };
static UINT32 VideoMagFactor = 100; // Normal size
static INT32 VideoShiftX = 0; // No shift on X-axis
static INT32 VideoShiftY = 0; // No shift on Y-axis

// Audio variables
#define AUDIODEC_RAW_SIZE (10<<20) // Raw buffer size
#define AUDIODEC_RAW_DESC_NUM (128) // Descriptor default number

static AMP_AVDEC_HDLR_s *gDecTransCodecVidHdlr = NULL;
static void* gDecTransCodecBufOri = NULL; ///< Original buffer address of video codec
static void* gDecTransRawBufOri = NULL;   ///< Original buffer address of video decode raw file
static void* gDecTransRawBuf = NULL;      ///< Aligned buffer address of video decode raw file

static AMP_AVDEC_HDLR_s *gDecTransCodecAudHdlr = NULL;
static void* gDecTransAudRawBufOri = NULL;   ///< Original buffer address of audio decode raw file
static void* gDecTransAudRawBuf = NULL;      ///< Aligned buffer address of audio decode raw file

static UINT8 gPauseFlag = 0;
static UINT8 gVideoDstCodec = 0;
static UINT8 gAudioDstCodec = 0;

static APPLIB_TRANSCODE_DECODE_MODE_e gTransDecMode = TRANSCODE_DECODE_MODE_AV; //temp

#define DEMUXER_FLAG_EVENT_END (1<<0)
#define DEMUXER_FLAG_EVENT_END_PROCESSED (1<<1)

static APP_QUERY_FILE_CB_f gQueryNextFile = NULL;
static AMBA_KAL_EVENT_FLAG_t gDemuxerEndEvent;
static AMBA_KAL_MUTEX_t gMutexFeedFile;
static UINT8 gTaskCanFeed = 0;
static UINT32 gDmxStartCount = 0;
static UINT32 gDmxEndCount = 0;
static UINT8 gFeedEndHandlingEntered = 0;

//--------------------------------------------------------------
//--temparily used start (Used to simulate netFifo)
//--------------------------------------------------------------
//#define SIMULATE_NETFIFO

#ifdef SIMULATE_NETFIFO
static AMBA_KAL_TASK_t _FakeCodecTask = {0};
static AMP_FIFO_HDLR_s *_FakeCodecFifoHdlr = NULL;
#define _FAKECODEC_STACK_SIZE (16<<10)
static char _FakeCodecStack[_FAKECODEC_STACK_SIZE];
AMBA_KAL_SEM_t _FakeSem = {0};

static int gFrameCount = 0;

static void _FakeCodec_Task(UINT32 info)
{
    AMP_BITS_DESC_s *Desc;
    //static int gFrameCount = 0;

    while (1) {
        AmbaKAL_SemTake(&_FakeSem, AMBA_KAL_WAIT_FOREVER);
        if (AmpFifo_PeekEntry(_FakeCodecFifoHdlr, &Desc, 0) == AMP_OK) {
            gFrameCount++;
            AmbaPrint("[%d] NewFrm incoming: @0x%x size:%d", gFrameCount, Desc->StartAddr, Desc->Size);
            AmpFifo_RemoveEntry(_FakeCodecFifoHdlr, 1);
        }
    }
}
int _FakeCodec_FifoCB(void *hdlr, UINT32 event, void* info)
{
    if (event == AMP_FIFO_EVENT_DATA_READY){
        //AmbaPrint("_FakeCodec_FifoCB on AMP_FIFO_EVENT_DATA_READY (0x%x)", event);
        AmbaKAL_SemGive(&_FakeSem);
    }
    return 0;
}

int _FakeCodec_EraseFifo(void)
{
    int ReturnValue =0 ;

    ReturnValue = AmpFifo_EraseAll(_FakeCodecFifoHdlr);
    if (ReturnValue != AMP_OK) {
        AmbaPrint("<%s> L%d Failed to erase video fifo data (%d).", __FUNCTION__, __LINE__, ReturnValue);
        ReturnValue = -1;
    }

    return ReturnValue;
}


int _FakeCodec_Init(void)
{
    AMP_FIFO_CFG_s fifoDefCfg = {0};
    static UINT8 init = 0;
    int er = 0;

    // create fifo
    AmpFifo_GetDefaultCfg(&fifoDefCfg);
    //fifoDefCfg.hCodec = (void*) (&gVideoDstCodec);//CreateTranscoderCodec; //_FakeCodec_Task;
    fifoDefCfg.hCodec = (void*) (&gAudioDstCodec);//CreateTranscoderCodec; //_FakeCodec_Task;
    fifoDefCfg.IsVirtual = 1;
    fifoDefCfg.NumEntries = 32;
    fifoDefCfg.cbEvent = _FakeCodec_FifoCB;
    AmpFifo_Create(&fifoDefCfg, &_FakeCodecFifoHdlr);
    AmbaPrint("_FakeCodecFifoHdlr = %x", _FakeCodecFifoHdlr);

    if (init != 0) {
        return 0;
    }
    init = 1;

    /* create sem  for frame read and write to fifo*/
    er = AmbaKAL_SemCreate(&_FakeSem, 0);
    if (er != OK) {
        AmbaPrint("Sem create failed: %d", er);
    }

    /* Create task */
    er = AmbaKAL_TaskCreate(&_FakeCodecTask, /* pTask */
    "_FakeCodec_FifoTask", /* pTaskName */
    71, /* Priority */
    _FakeCodec_Task, /* void (*EntryFunction)(UINT32) */
    0x0, /* EntryArg */
    (void *) _FakeCodecStack, /* pStackBase */
    _FAKECODEC_STACK_SIZE, /* StackByteSize */
    AMBA_KAL_AUTO_START); /* AutoStart */

    if (er != OK) {
        AmbaPrint("Task create failed: %d", er);
    }
    return 0;
}
#endif //SIMULATE_NETFIFO
//--------------------------------------------------------------
//--  temparily used end
//--------------------------------------------------------------

#if 0
/**
 * Return file duration in ms.
 *
 * @param [in] Filename          file name.
 * @param [out] RetDuration      Output. File duration.
 *
 * @return 0 - Success, Others - Failure
 */
static int GetFileDuration(char *Filename, UINT32 *RetDuration)
{
    UINT32 TimePerFrame = 0;
    UINT32 TimeScale = 0;
    UINT64 DTS = 0;
    APPLIB_MEDIA_INFO_s MediaInfo;
    int ReturnValue = 0;

    if ((!Filename) || (!RetDuration)) {
        TRANS_VIDEO_ERR("[Applib - TranscoderVideoDec] <GetFileDuration> %u invalid param", __LINE__);
        return -1;
    }

    ReturnValue = AppLibFormat_GetMediaInfo(Filename, &MediaInfo);
    if (ReturnValue != AMP_OK) {
        TRANS_VIDEO_ERR("[Applib - TranscoderVideoDec] <GetFileDuration> %u Failed to get media info", __LINE__);
        return -1;
    }

    DTS = MediaInfo.MediaInfo.Movie->Track[0].NextDTS;
    TimePerFrame = MediaInfo.MediaInfo.Movie->Track[0].TimePerFrame;
    TimeScale = MediaInfo.MediaInfo.Movie->Track[0].TimeScale;
    *RetDuration = DTS * 1000 / TimeScale;

    return 0;
}
#endif

/**
 * Event handler of demux callback.
 *
 * @param [in] hdlr             Pointer to handler.
 * @param [in] event            Evnet ID.
 * @param [in] hdlr             Event info.
 *
 * @return 0 - Success, Others - Failure
 */
static int AppLibTranscoderVideoDec_DemuxCallback(void *hdlr,
                                        UINT32 event,
                                        void* info)
{
    APPLIB_TRANSCODE_VIDEO_TASK_MSG_s VideoMsg;
    int Rval = 0;

    switch (event) {
    case AMP_DEMUXER_EVENT_START:
        //AmbaPrint("[Applib - TranscoderVideoDec] <DemuxCallBack> event AMP_DEMUXER_EVENT_START");
        gDmxStartCount++;
        break;
    case AMP_DEMUXER_EVENT_END:
        AmbaPrint("[Applib - TranscoderVideoDec] <DemuxCallBack> event AMP_DEMUXER_EVENT_END");
        #ifdef SIMULATE_NETFIFO
        gFrameCount = 0;
        #endif

        AmbaKAL_MutexTake(&gMutexFeedFile, AMBA_KAL_WAIT_FOREVER);
        gDmxEndCount++;
        AmbaKAL_EventFlagGive(&gDemuxerEndEvent, DEMUXER_FLAG_EVENT_END);
        AmbaKAL_MutexGive(&gMutexFeedFile);

        // Send message to video task
        // Initialize message
        SET_ZERO(VideoMsg);
        // Configure video message
        VideoMsg.MessageType = APPLIB_VIDEO_TASK_MSG_FEED_END;
        VideoMsg.AvcDecHdlr = hdlr;
        Rval = AppLibTranscoderVideoTask_SendVideoMsg(&VideoMsg, AMBA_KAL_NO_WAIT);
        if (Rval != 0) {
            TRANS_VIDEO_ERR("[Applib - TranscoderVideoDec] <DemuxCallBack> %u Failed to send message (%d)!", __LINE__, Rval);
            return -1;
        }
        break;
    default:
        // Do nothing
        break;
    }

    return 0;
}


int AppLibTranscoderVideoDec_FeedNextFile(UINT32 StartTime)
{
    char Filename[64] = {0};
    int ReturnValue = 0;
    UINT8 IsFeedEos; // Feed EOS to DSP at the end of the file


    if (gQueryNextFile == NULL) {
        TRANS_VIDEO_ERR("[Applib - TranscoderVideoDec] <FeedNextFile> %u null func pointer",__LINE__);
        return -1;
    }

    ReturnValue = gQueryNextFile(Filename, sizeof(Filename));
    if (ReturnValue != 0) {
        AmbaPrint("[Applib - TranscoderVideoDec] <FeedNextFile> %u no next file", __LINE__);
        return -2;
    }

    AmbaPrint("[Applib - TranscoderVideoDec] <FeedNextFile> %u file: %s", __LINE__, Filename);

    // Stop demuxer and decoder if the file is opened.
    if (IsFileOpened) {
        // Stop demuxer
        AmbaPrintColor(MAGENTA,"<%s> L%d call AppLibFormatDemuxMp4_Close()",__FUNCTION__,__LINE__);
        ReturnValue = AppLibFormatDemuxMp4_Close(0); // Do not erase fifo
        if (ReturnValue != 0) {
            TRANS_VIDEO_ERR("[Applib - TranscoderVideoDec] %u Failed to stop mp4 demuxer.", __LINE__);
            return ReturnValue;
        }
        IsFileOpened = 0;
    }

    if (gTransDecMode == TRANSCODE_DECODE_MODE_AV) {
        AppLibFormatDemuxMp4_SetCodecHdlrInfo(gDecTransCodecVidHdlr, gDecTransCodecAudHdlr,
                                                gDecTransRawBuf, VIDEODEC_RAW_SIZE,
                                                gDecTransAudRawBuf, AUDIODEC_RAW_SIZE);
    } else {
        AppLibFormatDemuxMp4_SetCodecHdlrInfo(gDecTransCodecVidHdlr, NULL, gDecTransRawBuf, VIDEODEC_RAW_SIZE, NULL, 0);
    }

    #if 0
    ReturnValue = GetFileDuration(Filename, &FileDuration);
    if (ReturnValue != 0) {
        TRANS_VIDEO_ERR("[Applib - TranscoderVideoDec] <FeedNextFile> %u Failed to get file duration", __LINE__);
        return -1;
    }

    AmbaPrint("FileDuration = %u", FileDuration);
    #endif

    IsFeedEos = 0;
    // Open a file and start demuxing
    ReturnValue = AppLibFormatDemuxMp4_Open(
                    Filename,
                    0, // Start time of current file. (TimeOffset is not included)
                    1, // Convert decode speed to demuxer speed
                    0, // Do not erase fifo
                    0, // netFifo does not care DTS
                    IsFeedEos,//Don't feed EOS
                    AppLibTranscoderVideoDec_DemuxCallback);

    if (ReturnValue != 0) {
        TRANS_VIDEO_ERR("[Applib - TranscoderVideoDec] <FeedNextFile> %u Failed to open and start a demuxer.", __LINE__);
        return -1;
    }

    IsFileOpened = 1;

    return 0;
}

static int AppLibTranscoderVideoDec_VideoDecCallBack(void *hdlr,
                                           UINT32 event,
                                           void* info)
{
    APPLIB_TRANSCODE_VIDEO_TASK_MSG_s VideoMsg;
    int Rval = 0;

    switch (event) {
    case AMP_DEC_EVENT_ERROR:
        AmbaPrint("[Applib - TranscoderVideoDec] <VideoDecCallBack> AMP_DEC_EVENT_ERROR!");
        break;
    case AMP_DEC_EVENT_FIRST_FRAME_DISPLAYED:
        AmbaPrint("[Applib - TranscoderVideoDec] <VideoDecCallBack> AMP_DEC_EVENT_FIRST_FRAME_DISPLAYED");
        break;
    case AMP_DEC_EVENT_PLAYBACK_EOS:
        AmbaPrint("[Applib - TranscoderVideoDec] <VideoDecCallBack> AMP_DEC_EVENT_PLAYBACK_EOS");
        // Send EOS message to video task
        // Initialize message
        SET_ZERO(VideoMsg);
        // Configure video message
        VideoMsg.MessageType = APPLIB_VIDEO_TASK_MSG_EOS;
        VideoMsg.AvcDecHdlr = hdlr;
        Rval = AppLibTranscoderVideoTask_SendVideoMsg(&VideoMsg, AMBA_KAL_NO_WAIT);
        if (Rval != 0) {
            TRANS_VIDEO_ERR("[Applib - TranscoderVideoDec] <VideoDecCallBack> Failed to send message (%d)!", Rval);
            return -1; // Error
        }
        break;
    case AMP_DEC_EVENT_STATE_CHANGED:
        AmbaPrint("[Applib - TranscoderVideoDec] <VideoDecCallBack> AMP_DEC_EVENT_STATE_CHANGED info: %x", info);
        break;
    case AMP_DEC_EVENT_DATA_UNDERTHRSHOLDD:
        //AmbaPrint("[Applib - TranscoderVideoDec] <DecCallback>  AMP_DEC_EVENT_DATA_UNDERTHRSHOLDD!");
        if (AppLibFormatDemuxMp4_CanRequestData() && (gPauseFlag == 0)) {
            //AmbaPrint("====> AppLibFormatDemuxMp4_DemuxOnDataRequest()");
            if (AppLibFormatDemuxMp4_DemuxOnDataRequest() != AMP_OK) {
                TRANS_VIDEO_ERR("[Applib - TranscoderVideoDec] <VideoDecCallBack> %u Failed to request data", __LINE__);
            }
        }
        break;
    default:
        TRANS_VIDEO_ERR("[Applib - TranscoderVideoDec] <VideoDecCallBack> Unknown event %X info: %x", event, info);
        break;
    }

    return 0;
}

static int AppLibTranscoderVideoDec_AudioDecCallBack(void *hdlr,
                                           UINT32 event,
                                           void* info)
{
    switch (event) {
    case AMP_DEC_EVENT_ERROR:
        AmbaPrint("[Applib - TranscoderVideoDec] <AudioDecCallBack> AMP_DEC_EVENT_ERROR!");
        break;
    case AMP_DEC_EVENT_FIRST_FRAME_DISPLAYED:
        AmbaPrint("[Applib - TranscoderVideoDec] <AudioDecCallBack> AMP_DEC_EVENT_FIRST_FRAME_DISPLAYED");
        break;
    case AMP_DEC_EVENT_PLAYBACK_EOS:
        AmbaPrint("[Applib - TranscoderVideoDec] <AudioDecCallBack> AMP_DEC_EVENT_PLAYBACK_EOS");
        break;
    case AMP_DEC_EVENT_STATE_CHANGED:
        AmbaPrint("[Applib - TranscoderVideoDec] <AudioDecCallBack> AMP_DEC_EVENT_STATE_CHANGED info: %x", info);
        break;
    case AMP_DEC_EVENT_DATA_UNDERTHRSHOLDD:
        //AmbaPrint("[Applib - TranscoderVideoDec] <AudioDecCallBack> AMP_DEC_EVENT_DATA_UNDERTHRSHOLDD");
        if (AppLibFormatDemuxMp4_CanRequestData() && (gPauseFlag == 0)) {
            //AmbaPrint("====> AppLibFormatDemuxMp4_DemuxOnDataRequestAudio()");
            if (AppLibFormatDemuxMp4_DemuxOnDataRequestAudio() != AMP_OK) {
                TRANS_VIDEO_ERR("[Applib - TranscoderVideoDec] <AudioDecCallBack> Failed to request data");
            }
        }

        break;
    default:
        TRANS_VIDEO_ERR("[Applib - TranscoderVideoDec] <AudioDecCallBack> Unknown event %X info: %x", event, info);
        break;
    }

    return 0;
}



static int CreateTranscoderCodec(AMP_AVDEC_HDLR_s **pDecTransCodecHdlr , APPLIB_TRANSCODE_MEDIA_TYPE_e Type)
{
    AMP_DEC_TRANSCODER_CFG_s TransCfg = {0};
    AMP_DEC_TRANSCODER_INIT_CFG_s TransInitCfg = {0};
    static UINT8 TransInited = 0;
    int ReturnValue = 0;

    if (TransInited == 0) {
        AmpDecTranscoder_GetInitDefaultCfg(&TransInitCfg);
        if (AmpUtil_GetAlignedPool(APPLIB_G_MMPL, (void**) &(TransInitCfg.WorkingBuf), &gDecTransCodecBufOri,
                TransInitCfg.WorkingBufSize, 1 << 5) != AMP_OK) {
            TRANS_VIDEO_ERR("[Applib - TranscoderVideoDec] <CreateTranscoderCodec> Cannot allocate memory.");
            return -1;
        }
        AmpDecTranscoder_Init(&TransInitCfg);
        TransInited = 1;
    }

    AmpDecTranscoder_GetDefaultCfg(&TransCfg);
    if (Type == TRANSCODE_MEDIA_TYPE_VIDEO) {
        if (AmpUtil_GetAlignedPool(APPLIB_G_MMPL, (void**) &gDecTransRawBuf, &gDecTransRawBufOri,
                                    VIDEODEC_RAW_SIZE, 1 << 5) != AMP_OK) {
            TRANS_VIDEO_ERR("[Applib - TranscoderVideoDec] <CreateTranscoderCodec> Cannot allocate memory.");
            return -1;
        }

        TransCfg.RawBuffer = gDecTransRawBuf;
        TransCfg.RawBufferSize = VIDEODEC_RAW_SIZE;
        TransCfg.CbTranscode = AppLibTranscoderVideoDec_VideoDecCallBack;
        TransCfg.DstCodec = (AMP_AVDEC_HDLR_s*) (&gVideoDstCodec);
        TransCfg.DstCodecType = 1;
        ReturnValue = AmpDecTranscoder_Create(&TransCfg, pDecTransCodecHdlr);
        if (ReturnValue != AMP_OK) {
            TRANS_VIDEO_ERR("[Applib - TranscoderVideoDec] <CreateTranscoderCodec> AmpDecTranscoder_Create() fail (%d)", ReturnValue);
            return -1;
        }

        //AmbaPrint("[Applib - TranscoderVideoDec] <CreateTranscoderCodec> video pDecTransCodecHdlr = 0x%x Raw@ 0x%x - 0x%x", pDecTransCodecHdlr,
        //                                                                    gDecTransRawBuf, VIDEODEC_RAW_SIZE + gDecTransRawBuf - 1);
    } else {
        if (AmpUtil_GetAlignedPool(APPLIB_G_MMPL, (void**) &gDecTransAudRawBuf, &gDecTransAudRawBufOri,
                                    AUDIODEC_RAW_SIZE, 1 << 5) != AMP_OK) {
            TRANS_VIDEO_ERR("[Applib - TranscoderVideoDec] <CreateTranscoderCodec> Cannot allocate memory.");
            return -1;
        }

        TransCfg.RawBuffer = gDecTransAudRawBuf;
        TransCfg.RawBufferSize = AUDIODEC_RAW_SIZE;
        TransCfg.CbTranscode = AppLibTranscoderVideoDec_AudioDecCallBack;
        TransCfg.DstCodec = (AMP_AVDEC_HDLR_s*) (&gAudioDstCodec);
        TransCfg.DstCodecType = 1;
        ReturnValue = AmpDecTranscoder_Create(&TransCfg, pDecTransCodecHdlr);
        if (ReturnValue != AMP_OK) {
            TRANS_VIDEO_ERR("[Applib - TranscoderVideoDec] <CreateTranscoderCodec> AmpDecTranscoder_Create() fail (%d)", ReturnValue);
            return -1;
        }

        //AmbaPrint("[Applib - TranscoderVideoDec] <CreateTranscoderCodec> audio pDecTransCodecHdlr = 0x%x Raw@ 0x%x - 0x%x", pDecTransCodecHdlr,
        //                                                                    gDecTransAudRawBuf, AUDIODEC_RAW_SIZE + gDecTransAudRawBuf - 1);
    }



    return 0;
}

int AppLibTranscoderVideoDec_Init(void)
{
    int ReturnValue = -1;

    // Check state
    if (VideoPlayerState != APPLIB_VIDEO_PLAYER_STATE_INVALID) {
        TRANS_VIDEO_ERR("[Applib - TranscoderVideoDec] <Init> Already initialized.");
        goto ReturnSuccess;
    }

    if (AmbaKAL_EventFlagCreate(&gDemuxerEndEvent) != OK) {
        TRANS_VIDEO_ERR("[Applib - TranscoderVideoDec] <Init> Create event flag fail");
        goto ReturnError;
    }

    if (AmbaKAL_MutexCreate(&gMutexFeedFile) != OK) {
        TRANS_VIDEO_ERR("[Applib - TranscoderVideoDec] <Init> Create Mutex fail");
        goto ReturnError;
    }

    // Initialize decode task
    if (AppLibTranscoderVideoTask_IsTaskInitialized() == 0) {
        AppLibTranscoderVideoTask_InitTask();
    }

    if (gDecTransCodecVidHdlr == NULL) {
        ReturnValue = CreateTranscoderCodec(&gDecTransCodecVidHdlr, TRANSCODE_MEDIA_TYPE_VIDEO);
        if (ReturnValue != AMP_OK) {
            TRANS_VIDEO_ERR("[Applib - TranscoderVideoDec] <Init> Failed to create video transcoder codec (%d)", ReturnValue);
            goto ReturnError;
        }
    }

    if ((gDecTransCodecAudHdlr == NULL) && (gTransDecMode == TRANSCODE_DECODE_MODE_AV)) {
        ReturnValue = CreateTranscoderCodec(&gDecTransCodecAudHdlr, TRANSCODE_MEDIA_TYPE_AUDIO);
        if (ReturnValue != AMP_OK) {
            TRANS_VIDEO_ERR("[Applib - TranscoderVideoDec] <Init> Failed to create audio transcoder codec (%d)", ReturnValue);
            goto ReturnError;
        }
    }

    // Create decoder manager
    if (DecPipeHdlr == NULL) {
        AMP_DEC_PIPE_CFG_s PipeCfg;

        ReturnValue = AmpDec_GetDefaultCfg(&PipeCfg);
        if (ReturnValue != AMP_OK) {
            TRANS_VIDEO_ERR("[Applib - TranscoderVideoDec]<Init> Failed to get default decoder manager config (%d).", ReturnValue);
            goto ReturnError;
        }

        PipeCfg.Decoder[0] = gDecTransCodecVidHdlr;
        PipeCfg.NumDecoder = 1;
        if (gDecTransCodecAudHdlr) {
            PipeCfg.Decoder[1] = gDecTransCodecAudHdlr;
            PipeCfg.NumDecoder++;
        }
        PipeCfg.Type = AMP_DEC_VID_PIPE;

        ReturnValue = AmpDec_Create(&PipeCfg, &DecPipeHdlr);
        if (ReturnValue != AMP_OK) {
            TRANS_VIDEO_ERR("[Applib - TranscoderVideoDec] <Init> Failed to create decoder manager (%d).", ReturnValue);
            goto ReturnError;
        }
    }

    // Active pipe
    ReturnValue = AmpDec_Add(DecPipeHdlr);
    if (ReturnValue != AMP_OK) {
        TRANS_VIDEO_ERR("[Applib - TranscoderVideoDec] <Init> Failed to activate decoder manager (%d).", ReturnValue);
        goto ReturnError;
    }

    #ifdef SIMULATE_NETFIFO
    if (gTransDecMode == TRANSCODE_DECODE_MODE_AV) {
        _FakeCodec_Init(); // temporarily used
    }
    #endif

    // Initialize video settings
    VideoPlayerState = APPLIB_VIDEO_PLAYER_STATE_IDLE;
    CurrentPlayingSpeed = APPLIB_PLAYER_NORMAL_SPEED;
    PausedSpeed = APPLIB_PLAYER_NORMAL_SPEED;
    CurrentPlayingDir = APPLIB_VIDEO_TRANSCODE_FW; // Play forward
    SET_ZERO(Trick);
    SET_ZERO(Display);
    VideoMagFactor = 100; // Normal size
    VideoShiftX = 0; // No shift on X-axis
    VideoShiftY = 0; // No shift on Y-axis

ReturnSuccess:
    TRANS_VIDEO_DBG("[Applib - TranscoderVideoDec] <Init> success");

    return AMP_OK;

ReturnError:
    AppLibTranscoderVideoDec_Exit();

    TRANS_VIDEO_ERR("[Applib - TranscoderVideoDec] <Init> fail");
    return -1;
}

int AppLibTranscoderVideoDec_GetStartDefaultCfg(APPLIB_VIDEO_TRANSCODE_START_INFO_s* OutputVideoStartInfo)
{
    if (OutputVideoStartInfo == NULL) {
        TRANS_VIDEO_ERR("[Applib - TranscoderVideoDec] <GetStartDefaultCfg> OutputVideoStartInfo is NULL");
        return -1;
    }
    OutputVideoStartInfo->Filename = NULL;
    OutputVideoStartInfo->AutoPlay = 1; // Open and play
    OutputVideoStartInfo->StartTime = 0; // Start at the beginning
    OutputVideoStartInfo->Direction = APPLIB_VIDEO_TRANSCODE_FW; // Play forward
    OutputVideoStartInfo->ResetSpeed = 1; // Reset to 1x speed
    OutputVideoStartInfo->ResetZoom = 1; // Reset to original size
    return 0;
}

int AppLibTranscoderVideoDec_GetMultiStartDefaultCfg(APPLIB_VIDEO_TRANSCODE_START_MULTI_INFO_s* OutputVideoStartInfo)
{
    if (OutputVideoStartInfo == NULL) {
        TRANS_VIDEO_ERR("[Applib - TranscoderVideoDec] <GetMultiStartDefaultCfg> OutputVideoStartInfo is NULL");
        return -1;
    }
    OutputVideoStartInfo->File = NULL;
    OutputVideoStartInfo->FileNum = 0;
    OutputVideoStartInfo->AutoPlay = 1; // Open and play
    OutputVideoStartInfo->StartTime = 0; // Start at the beginning
    OutputVideoStartInfo->Direction = APPLIB_VIDEO_TRANSCODE_FW; // Play forward
    OutputVideoStartInfo->ReloadFile = 1; // Play video specified in "File"
    OutputVideoStartInfo->ResetSpeed = 1; // Reset to 1x speed
    OutputVideoStartInfo->ResetZoom = 1; // Reset to original size
    return 0; // Success
}

int AppLibTranscoderVideoDec_GetMultiFileInfo(APPLIB_VIDEO_TRANSCODE_START_MULTI_INFO_s* VideoStartInfo)
{
    APPLIB_MEDIA_INFO_s MediaInfo; // Medio info of the video
    int i = 0;
    int Rval = 0; // Function call return value

    if (VideoStartInfo == NULL) {
        TRANS_VIDEO_ERR("[Applib - TranscoderVideoDec] <GetMultiFileInfo> OutputVideoStartInfo is NULL");
        return -1;
    }
    if (VideoStartInfo->File == NULL || VideoStartInfo->FileNum == 0 || VideoStartInfo->ReloadFile == 0) {
        TRANS_VIDEO_DBG("[Applib - TranscoderVideoDec] <GetMultiFileInfo> No file.");
        return 0;
    }

    // Set the EOS time before playing the clip
    for (i = 0; i < VideoStartInfo->FileNum; ++i) {
        UINT32 TimePerFrame = 0;
        UINT32 TimeScale = 0;
        UINT64 DTS = 0;
        Rval = AppLibFormat_GetMediaInfo(VideoStartInfo->File[i].Filename, &MediaInfo);
        if (Rval != AMP_OK) {
            TRANS_VIDEO_ERR("[Applib - TranscoderVideoDec] <GetMultiFileInfo> Failed to get media info");
            return -1; // Error
        }
        if (MediaInfo.MediaInfoType != AMP_MEDIA_INFO_MOVIE) {
            TRANS_VIDEO_ERR("[Applib - TranscoderVideoDec] <GetMultiFileInfo> Invalid media type (%d)", MediaInfo.MediaInfoType);
            return -1; // Error
        }
        if (MediaInfo.MediaInfo.Movie->Track[0].NextDTS == 0xFFFFFFFFFFFFFFFF) {
            TRANS_VIDEO_ERR("[Applib - TranscoderVideoDec] <GetMultiFileInfo> DTS is not valid");
            return -1; // Error
        }
        DTS = MediaInfo.MediaInfo.Movie->Track[0].NextDTS;
        TimePerFrame = MediaInfo.MediaInfo.Movie->Track[0].TimePerFrame;
        TimeScale = MediaInfo.MediaInfo.Movie->Track[0].TimeScale;
        VideoStartInfo->File[i].Duration = DTS * 1000 / TimeScale;
        if (i == 0) {
            VideoStartInfo->File[i].InitTime = 0;
        } else {
            VideoStartInfo->File[i].InitTime = VideoStartInfo->File[i - 1].InitTime + VideoStartInfo->File[i - 1].Duration;
        }
        // NOTE: EosFileTime is no longer used. MW video decoder will get EOS PTS from fifo.
        // This calculation is based on an assumption that the first PTS we got from DSP is 0.
        // However in some cases the first frame is not 0 and the formula should be (DTS). We'll reach EOS one frame earlyer.
        // Currently there's no way to find out which type a video is. Only demuxer knows that.
        VideoStartInfo->File[i].EosFileTime = (VideoStartInfo->File[i].InitTime) * TimeScale / 1000 + DTS - TimePerFrame;
        VideoStartInfo->File[i].DeltaFileTime = TimePerFrame;
        VideoStartInfo->File[i].FileTimeScale = TimeScale;
    }

    return 0; // Success
}

static int AppLibTranscoderVideoDec_CalculateDisplayArea(
        AMP_VIDEODEC_DISPLAY_s *OutputDisplay,
        const UINT32 Factor,
        const INT32 X,
        const INT32 Y)
{
    if (OutputDisplay == NULL) {
        return -1;
    }
    {
        const INT32 SrcWidth = (INT32) OutputDisplay->SrcWidth;
        const INT32 SrcHeight = (INT32) OutputDisplay->SrcHeight;
        const INT32 MagFactor = MAX((INT32) Factor, 100); // Factor cannot less than 100
        INT32 TempWidth = SrcWidth * 100 / MagFactor;
        INT32 TempHeight = SrcHeight * 100 / MagFactor;
        INT32 TempX = X + (SrcWidth - TempWidth) / 2;
        INT32 TempY = Y + (SrcHeight - TempHeight) / 2;
        INT32 MaxX = SrcWidth - TempWidth;      // SrcWidth >= TempWidth
        INT32 MaxY = SrcHeight - TempHeight;    // SrcHeight >= TempHeight
        OutputDisplay->AOI.Width = (UINT32) TempWidth;
        OutputDisplay->AOI.Height = (UINT32) TempHeight;
        OutputDisplay->AOI.X = (UINT32) MIN(MAX(TempX, 0), MaxX); // 0 <= X <= MaxX
        OutputDisplay->AOI.Y = (UINT32) MIN(MAX(TempY, 0), MaxY); // 0 <= Y <= MaxY
    }

    return 0;
}

int AppLibTranscoderVideoDec_StartMultiple(const APPLIB_VIDEO_TRANSCODE_START_MULTI_INFO_s* VideoStartInfo)
{
    int ReturnValue = 0;
    UINT32 VideoStartTime = 0;
    UINT8 WaitDemuxEnd = 0;
    UINT32 ActualFlag = 0;

    // Preliminary check
    if (VideoStartInfo == NULL) {
        TRANS_VIDEO_ERR("[Applib - TranscoderVideoDec] <StartMultiple> %u VideoStartInfo is NULL", __LINE__);
        return -1;
    }

    // Check state
    if (VideoPlayerState == APPLIB_VIDEO_PLAYER_STATE_INVALID) {
        TRANS_VIDEO_ERR("[Applib - TranscoderVideoDec] <StartMultiple> %u Initialize first.", __LINE__);
        return -1;
    }
    if (DecPipeHdlr == NULL) {
        TRANS_VIDEO_ERR("[Applib - TranscoderVideoDec] <StartMultiple> %u DecPipeHdlr is NULL.", __LINE__);
        return -1;
    }

    // Check reset speed. Do this before AutoPlay checking.
    if (VideoStartInfo->ResetSpeed) {
        CurrentPlayingSpeed = APPLIB_PLAYER_NORMAL_SPEED;
        PausedSpeed = APPLIB_PLAYER_NORMAL_SPEED;
    }

    // Check AutoPlay
    if (VideoStartInfo->AutoPlay) {
        TRANS_VIDEO_DBG("[Applib - TranscoderVideoDec] <StartMultiple> %u open & play ", __LINE__);
        // Resume if it's paused.
        if (CurrentPlayingSpeed == 0) {
            CurrentPlayingSpeed = PausedSpeed;
        }
    }

    // Configure direction
    if (VideoStartInfo->Direction == APPLIB_VIDEO_TRANSCODE_FW) {
        CurrentPlayingDir = APPLIB_VIDEO_TRANSCODE_FW; // Play forward
    } else {
        CurrentPlayingDir = APPLIB_VIDEO_TRANSCODE_BW; // Play backward
    }


    /* Used to skip file feeding caused by AMP_DEMUXER_EVENT_END event.
          AppLibFormatDemuxMp4_Stop() maybe cause demuxer to send AMP_DEMUXER_EVENT_END event. */
    gTaskCanFeed = 0;

    // Stop decoder if the file is opened.
    if (IsFileOpened) {
        AmbaKAL_MutexTake(&gMutexFeedFile, AMBA_KAL_WAIT_FOREVER);
        if (gDmxStartCount > gDmxEndCount) {
            WaitDemuxEnd = 1;
            AmbaKAL_EventFlagClear(&gDemuxerEndEvent, (DEMUXER_FLAG_EVENT_END | DEMUXER_FLAG_EVENT_END_PROCESSED));
        }
        AmbaKAL_MutexGive(&gMutexFeedFile);

        /**stop demuxer*/
        ReturnValue = AppLibFormatDemuxMp4_Stop();
        // Stop decoder
        ReturnValue = AmpDec_StopWithLastFrm(DecPipeHdlr);
        if (ReturnValue != AMP_OK) {
            TRANS_VIDEO_ERR("[Applib - TranscoderVideoDec] <StartMultiple> %u Failed to stop video codec manager.", __LINE__);
            return ReturnValue;
        }

        if (WaitDemuxEnd) {
            // wait for demuxer end
            AmbaPrintColor(CYAN,"<%s> wait for demuxer end",__FUNCTION__);
            AmbaKAL_EventFlagTake(&gDemuxerEndEvent, (DEMUXER_FLAG_EVENT_END | DEMUXER_FLAG_EVENT_END_PROCESSED),
                                    TX_AND_CLEAR, &ActualFlag, AMBA_KAL_WAIT_FOREVER);
        }
    }

    VideoStartTime = VideoStartInfo->StartTime;

    if (VideoStartTime < 0) {
        AmbaPrintColor(RED,"\n[Applib - TranscoderVideoDec] <StartMultiple> VideoStartTime < 0\n");
    }

    TRANS_VIDEO_DBG("[Applib - TranscoderVideoDec] <StartMultiple> VideoStartTime = %d", VideoStartTime);
    AmbaPrintColor(MAGENTA, "<%s> L%d feed file (start/seek)",__FUNCTION__,__LINE__);
    /**Check if need open next file, if need close current file and open next*/
    if (AppLibTranscoderVideoDec_FeedNextFile(VideoStartTime) != 0) {
        return -1;
    }

    gTaskCanFeed = 1;

    // Zooming
    {
        UINT32 MovieWidth = 0, MovieHeight = 0;
        AppLibFormatDemuxMp4_GetMovieSize(&MovieWidth, &MovieHeight);
        TRANS_VIDEO_DBG("[Applib - TranscoderVideoDec] <StartMultiple> MovieWidth = %d, MovieHeight = %d", MovieWidth, MovieHeight);
        if ((MovieWidth == 0) && (MovieHeight == 0)) {
            MovieWidth = 1920;
            MovieHeight = 1080;
        }

        if (VideoStartInfo->ResetZoom) {
            VideoMagFactor = 100;
            VideoShiftX = 0;
            VideoShiftY = 0;
        }

        // Configure display settings
        Display.SrcWidth = MovieWidth;
        Display.SrcHeight = MovieHeight;
        // Calculate display area

        if (AppLibTranscoderVideoDec_CalculateDisplayArea(&Display, VideoMagFactor, VideoShiftX, VideoShiftY) != 0) {
            TRANS_VIDEO_ERR("[Applib - TranscoderVideoDec] <StartMultiple> %u Failed to calculate display area.", __LINE__);
            return -1;
        }

    }

    // Configure trick play
    Trick.Speed = CurrentPlayingSpeed;
    Trick.TimeOffsetOfFirstFrame = VideoStartTime;
    if (CurrentPlayingDir == APPLIB_VIDEO_TRANSCODE_FW) {
        Trick.Direction = AMP_VIDEO_PLAY_FW;
    } else {
        Trick.Direction = AMP_VIDEO_PLAY_BW;
    }

    ReturnValue = AppLibFormatDemuxMp4_Start(Trick.TimeOffsetOfFirstFrame,Trick.Direction,APPLIB_TO_DEMUX_SPEED(Trick.Speed));
    if (ReturnValue != AMP_OK) {
            TRANS_VIDEO_ERR("[Applib - TranscoderVideoDec] <StartMultiple> %u Failed to start video (%d).", __LINE__, ReturnValue);
            return ReturnValue;
    }

    {
        // Start playing video
        AmbaPrint("[Applib - TranscoderVideoDec] <StartMultiple> AmpDec_Start start");
        // Play video

        ReturnValue = AmpDec_Start(DecPipeHdlr, &Trick, &Display);

        if (VideoStartInfo->AutoPlay == 0) {
            AmbaPrintColor(GREEN, "[Applib - TranscoderVideoDec] <StartMultiple> %u open & pause", __LINE__);
            ReturnValue = AmpDec_Pause(DecPipeHdlr);
            if (ReturnValue != 0) {
                TRANS_VIDEO_ERR("[Applib - TranscoderVideoDec] <StartMultiple> pause fail");
            }
            // Open and pause
            // Remember speed befrore pause
            if (CurrentPlayingSpeed != 0) { // In case of duplicate pause actions.
                PausedSpeed = CurrentPlayingSpeed;
            }
            CurrentPlayingSpeed = 0;
            //PausedTime = VideoStartTime + FileInfoList[CurrentFeedingFileID].InitTime;
        }
        if (ReturnValue != AMP_OK) {
            TRANS_VIDEO_ERR("[Applib - TranscoderVideoDec] <StartMultiple> %u Failed to start playing video (%d)", __LINE__, ReturnValue);
            return ReturnValue;
        }
        AmbaPrint("[Applib - TranscoderVideoDec] <StartMultiple> AmpDec_Start end");

        // Set video state
        if (VideoStartInfo->AutoPlay) {
            VideoPlayerState = APPLIB_VIDEO_PLAYER_STATE_PLAY;
        } else {
            VideoPlayerState = APPLIB_VIDEO_PLAYER_STATE_PAUSE_CHANGE;
        }

    }

    return 0;
}

int AppLibTranscoderVideoDec_Start(const APPLIB_VIDEO_TRANSCODE_START_INFO_s* VideoStartInfo)
{
    //APPLIB_VIDEO_TRANSCODE_FILE_INFO_s File;
    APPLIB_VIDEO_TRANSCODE_START_MULTI_INFO_s StartInfo;

    if (VideoPlayerState == APPLIB_VIDEO_PLAYER_STATE_PAUSE) {
        gPauseFlag = 0;
    }

    // Set video configuration
    if (AppLibTranscoderVideoDec_GetMultiStartDefaultCfg(&StartInfo) != 0) {
        return -1;
    }

#if 1
    StartInfo.File = NULL;
    StartInfo.FileNum = 0;
#else
    File.Filename = VideoStartInfo->Filename;
    StartInfo.File = &File;
    StartInfo.FileNum = 1;
#endif
    StartInfo.AutoPlay = VideoStartInfo->AutoPlay;
    StartInfo.StartTime = VideoStartInfo->StartTime;
    StartInfo.Direction = VideoStartInfo->Direction;
    StartInfo.ReloadFile = 1;
    StartInfo.ResetSpeed = VideoStartInfo->ResetSpeed;
    StartInfo.ResetZoom = VideoStartInfo->ResetZoom;

    if (AppLibTranscoderVideoDec_StartMultiple(&StartInfo) != 0) {
        return -1;
    }

    return 0;
}


int AppLibTranscoderVideoDec_Pause(void)
{
    // Check state
    if (VideoPlayerState == APPLIB_VIDEO_PLAYER_STATE_INVALID) {
        TRANS_VIDEO_ERR("[Applib - TranscoderVideoDec] <Pause> Initialize first.");
        return -1;
    }
    // Preliminary check
    if (DecPipeHdlr == NULL) {
        TRANS_VIDEO_ERR("[Applib - TranscoderVideoDec] <Pause> DecPipeHdlr is NULL.");
        return -1;
    }
    if (CurrentPlayingSpeed == 0) {
        TRANS_VIDEO_ERR("[Applib - TranscoderVideoDec] <Pause> Unexpected speed.");
        return -1;
    }

    switch (VideoPlayerState) {
        case APPLIB_VIDEO_PLAYER_STATE_PLAY:
            gPauseFlag = 1;
            // Remember speed befrore pause
            PausedSpeed = CurrentPlayingSpeed;
            CurrentPlayingSpeed = 0;
            // Set video state
            VideoPlayerState = APPLIB_VIDEO_PLAYER_STATE_PAUSE;

            break;
        case APPLIB_VIDEO_PLAYER_STATE_PAUSE:
        case APPLIB_VIDEO_PLAYER_STATE_PAUSE_CHANGE:
            // Preliminary check
            if (CurrentPlayingSpeed != 0) {
                TRANS_VIDEO_ERR("[Applib - TranscoderVideoDec] <Pause> Unexpected speed.");
                return -1;
            }
            // Already paused
            break; // Do nothing and return success
        default:
            TRANS_VIDEO_ERR("[Applib - TranscoderVideoDec] <Pause> Pause at wrong state (%d).", VideoPlayerState);
            return -1; // Error
    }

    return 0; // Success
}


int AppLibTranscoderVideoDec_Resume(void)
{
    // Check state
    if (VideoPlayerState == APPLIB_VIDEO_PLAYER_STATE_INVALID) {
        TRANS_VIDEO_ERR("[Applib - TranscoderVideoDec] <Resume> Initialize first.");
        return -1;
    }

    AmbaPrint("[Applib - TranscoderVideoDec] Resume Start");
    switch (VideoPlayerState) {
        case APPLIB_VIDEO_PLAYER_STATE_PLAY:
            // Preliminary check
            if (CurrentPlayingSpeed == 0) {
                TRANS_VIDEO_ERR("[Applib - TranscoderVideoDec] <Resume> Unexpected speed.");
                return -1;
            }
            break; // Do nothing and return success
        case APPLIB_VIDEO_PLAYER_STATE_PAUSE_CHANGE:
        case APPLIB_VIDEO_PLAYER_STATE_PAUSE:
            // Preliminary check
            if (DecPipeHdlr == NULL) {
                TRANS_VIDEO_ERR("[Applib - TranscoderVideoDec] <Resume> DecPipeHdlr is NULL.");
                return -1;
            }
            if (CurrentPlayingSpeed != 0) {
                TRANS_VIDEO_ERR("[Applib - TranscoderVideoDec] <Resume> Unexpected speed.");
                return -1;
            }

            // Resume video
            gPauseFlag = 0;
            // Recover speed befrore pause
            CurrentPlayingSpeed = PausedSpeed;
            // Set video state
            VideoPlayerState = APPLIB_VIDEO_PLAYER_STATE_PLAY;

            if (AppLibFormatDemuxMp4_CanRequestData()) {
                if (AppLibFormatDemuxMp4_DemuxOnDataRequest() != AMP_OK) {
                    TRANS_VIDEO_ERR("[Applib - TranscoderVideoDec] <Resume> Failed to request data");
                }
            }
            break;

        default:
            TRANS_VIDEO_ERR("[Applib - TranscoderVideoDec] <Resume> Resume at wrong state (%d).", VideoPlayerState);
            return -1; // Error
    }

    return 0; // Success
}

int AppLibTranscoderVideoDec_Stop(void)
{
    int ReturnValue = 0;

    TRANS_VIDEO_DBG("[Applib - TranscoderVideoDec] <Stop> enter");

    // Check state
    if (VideoPlayerState == APPLIB_VIDEO_PLAYER_STATE_INVALID) {
        TRANS_VIDEO_ERR("[Applib - TranscoderVideoDec] <Stop> Initialize first.");
        return -1;
    }
    if (VideoPlayerState == APPLIB_VIDEO_PLAYER_STATE_IDLE) {
        // Do nothing
        TRANS_VIDEO_DBG("[Applib - TranscoderVideoDec] <Stop> idle, do nothing");
        return 0;
    }

    AppLibFormatDemuxMp4_Stop();

    if (AppLibFormatDemuxMp4_Close(1) != 0) { // Erase fifo
        TRANS_VIDEO_ERR("[Applib - TranscoderVideoDec] <Stop> Failed to close demuxer.");
        // Don't return
    }

    IsFileOpened = 0;

    if (DecPipeHdlr != NULL) {
        // Stop video
        ReturnValue = AmpDec_Stop(DecPipeHdlr);
        if (ReturnValue != AMP_OK) {
            TRANS_VIDEO_ERR("[Applib - TranscoderVideoDec] <Stop> Failed to stop video (%d).", ReturnValue);
            return -1;
        }
    } else {
        TRANS_VIDEO_DBG("[Applib - TranscoderVideoDec] <Stop> DecPipeHdlr is NULL.");
    }

    // Set video state
    VideoPlayerState = APPLIB_VIDEO_PLAYER_STATE_IDLE;

    gPauseFlag = 0; //reset pause flag
    gDmxStartCount = 0;
    gDmxEndCount = 0;

    TRANS_VIDEO_DBG("[Applib - TranscoderVideoDec] <Stop> end");

    return 0;
}

int AppLibTranscoderVideoDec_Exit(void)
{
    int ReturnValue = 0;

    TRANS_VIDEO_DBG("[Applib - TranscoderVideoDec] <Exit>");

    // Check state
    if (VideoPlayerState == APPLIB_VIDEO_PLAYER_STATE_INVALID) {
        TRANS_VIDEO_DBG("[Applib - TranscoderVideoDec] <Exit> Already exit.");
        return 0;
    }

    VideoPlayerState = APPLIB_VIDEO_PLAYER_STATE_INVALID;

    AppLibFormatDemuxMp4_Close(1); // Erase fifo
    IsFileOpened = 0;

    // deinit
    if (DecPipeHdlr != NULL) {
        ReturnValue = AmpDec_Stop(DecPipeHdlr);
        if (ReturnValue != AMP_OK) {
            TRANS_VIDEO_ERR("[Applib - TranscoderVideoDec] <Exit> %u Failed to stop codec manager (%d).", __LINE__, ReturnValue);
        }
        ReturnValue = AmpDec_Remove(DecPipeHdlr);
        if (ReturnValue != AMP_OK) {
            TRANS_VIDEO_ERR("[Applib - TranscoderVideoDec] <Exit> %u Failed to remove codec manager (%d).", __LINE__, ReturnValue);
        }
        ReturnValue = AmpDec_Delete(DecPipeHdlr);
        if (ReturnValue != AMP_OK) {
            TRANS_VIDEO_ERR("[Applib - TranscoderVideoDec] <Exit> %u Failed to delete codec manager (%d).", __LINE__, ReturnValue);
        }
        DecPipeHdlr = NULL;
    }

    if (gDecTransCodecVidHdlr) {
        ReturnValue = AmpDecTranscoder_Delete(gDecTransCodecVidHdlr);
         if (ReturnValue != AMP_OK) {
            TRANS_VIDEO_ERR("[Applib - TranscoderVideoDec] <Exit> %u Failed to delete transcoder (%d).", __LINE__, ReturnValue);
        }
        gDecTransCodecVidHdlr = NULL;
    }

    if (gDecTransRawBufOri) {
        ReturnValue = AmbaKAL_BytePoolFree(gDecTransRawBufOri);
        if (ReturnValue != AMP_OK) {
            TRANS_VIDEO_ERR("[Applib - TranscoderVideoDec] <Exit> %u Failed to free memory at 0x%08x (%d).", __LINE__, gDecTransRawBufOri, ReturnValue);
        }
        gDecTransRawBufOri = NULL;
    }

    if (gDecTransCodecAudHdlr) {
        ReturnValue = AmpDecTranscoder_Delete(gDecTransCodecAudHdlr);
         if (ReturnValue != AMP_OK) {
            TRANS_VIDEO_ERR("[Applib - TranscoderVideoDec] <Exit> %u Failed to delete transcoder (%d).", __LINE__, ReturnValue);
        }
        gDecTransCodecAudHdlr = NULL;
    }

    if (gDecTransAudRawBufOri) {
        ReturnValue = AmbaKAL_BytePoolFree(gDecTransAudRawBufOri);
        if (ReturnValue != AMP_OK) {
            TRANS_VIDEO_ERR("[Applib - TranscoderVideoDec] <Exit> %u Failed to free memory at 0x%08x (%d)", __LINE__, gDecTransAudRawBufOri, ReturnValue);
        }
        gDecTransAudRawBufOri = NULL;
    }


    // Deinit video task
    if (AppLibTranscoderVideoTask_IsTaskInitialized()) {
        AppLibTranscoderVideoTask_DeinitTask();
    }

    if (AmbaKAL_EventFlagDelete(&gDemuxerEndEvent) != OK) {
        TRANS_VIDEO_ERR("[Applib - TranscoderVideoDec] <Exit> Failed to delete event flag");
    }

    if (AmbaKAL_MutexDelete(&gMutexFeedFile) != OK) {
        TRANS_VIDEO_ERR("[Applib - TranscoderVideoDec] <Exit> Failed to delete mutex");
    }

    return ReturnValue;
}

int AppLibTranscoderVideoDec_RegQueryNextFileCb(APP_QUERY_FILE_CB_f QueryNextFileFunc)
{
    if (QueryNextFileFunc == NULL) {
        TRANS_VIDEO_ERR("[Applib - TranscoderVideoDec] <RegQueryNextFileCb> NULL param");
        return -1;
    }

    gQueryNextFile = QueryNextFileFunc;
    return 0;
}

int AppLibTranscoderVideoDec_GetDstCodec(void **DstCodec, APPLIB_TRANSCODE_MEDIA_TYPE_e Type)
{
    if (!DstCodec) {
        TRANS_VIDEO_ERR("[Applib - TranscoderVideoDec] <GetDstCodec> NULL param");
        return -1;
    }

    *DstCodec = (void *) NULL;

    if (Type == TRANSCODE_MEDIA_TYPE_VIDEO) {
        if (gDecTransCodecVidHdlr) {
            *DstCodec = (void *)(&gVideoDstCodec);
        }
    } else {
        if (gDecTransCodecAudHdlr) {
            *DstCodec = (void *)(&gAudioDstCodec);
        }
    }

    return 0;
}

int AppLibTranscoderVideoDec_GetBufferInfo(UINT8 **BufferAddr, UINT32 *BufferSize, APPLIB_TRANSCODE_MEDIA_TYPE_e Type)
{
    if (!BufferAddr || !BufferSize) {
        TRANS_VIDEO_ERR("[Applib - TranscoderVideoDec] <GetBufferInfo> NULL pointer");
        return -1;
    }

    if (Type == TRANSCODE_MEDIA_TYPE_VIDEO) {
        if (gDecTransRawBuf == NULL) {
            TRANS_VIDEO_ERR("[Applib - TranscoderVideoDec] <GetBufferInfo> gDecTransRawBuf is NULL");
            return -1;
        }

        *BufferAddr = (UINT8 *) gDecTransRawBuf;
        *BufferSize = VIDEODEC_RAW_SIZE;
    } else {
        if (gDecTransAudRawBuf == NULL) {
            TRANS_VIDEO_ERR("[Applib - TranscoderVideoDec] <GetBufferInfo> gDecTransAudRawBuf is NULL");
            return -1;
        }

        *BufferAddr = (UINT8 *) gDecTransAudRawBuf;
        *BufferSize = AUDIODEC_RAW_SIZE;
    }

    return 0;
}

int AppLibTranscoderVideoDec_SetDecodeMode(APPLIB_TRANSCODE_DECODE_MODE_e DecodeMode)
{
    if (DecodeMode >= TRANSCODE_DECODE_MODE_NUM) {
        TRANS_VIDEO_ERR("[Applib - TranscoderVideoDec] <SetDecodeMode> invalid param (DecodeMode = %d)", DecodeMode);
        return -1;
    }

    gTransDecMode = DecodeMode;
    return 0;
}

APPLIB_TRANSCODE_DECODE_MODE_e AppLibTranscoderVideoDec_GetDecodeMode(void)
{
    return gTransDecMode;
}

int AppLibTranscoderVideoDec_GetNextFileFeedStatus(void)
{
    return gFeedEndHandlingEntered;
}

int AppLibTranscoderVideoDec_DisableNextFileFeed(void)
{
    gTaskCanFeed = 0;

    return 0;
}

int AppLibTranscoderVideoDec_FeedEndHandling(void)
{
    int ReturnValue = 0;
    int ErrorCode = 0;

    gFeedEndHandlingEntered = 1;

    if (gTaskCanFeed == 0) {
        /* Don't do file feeding. */
        TRANS_VIDEO_DBG("[Applib - TranscoderVideoDec] <FeedEndHandling> skip file feeding");
    } else {
        AmbaPrintColor(MAGENTA, "[Applib - TranscoderVideoDec] <FeedEndHandling> feed file (next)");
        ReturnValue = AppLibTranscoderVideoDec_FeedNextFile(0);
        if (ReturnValue == 0) {
            AmbaPrintColor(MAGENTA,"[Applib - TranscoderVideoDec] <FeedEndHandling> call AppLibFormatDemuxMp4_Start()");
            ReturnValue = AppLibFormatDemuxMp4_Start(0, 0, 1);
            if (ReturnValue != AMP_OK) {
                TRANS_VIDEO_ERR("[Applib - TranscoderVideoDec] <FeedEndHandling> %u Failed to start video (%d).", __LINE__, ReturnValue);
                ErrorCode = -1;
            } else {
                ErrorCode = 0;
            }
        } else if (ReturnValue == -2){
            /* there is no next file to be displayed. */
            ErrorCode = 0;
        } else {
            ErrorCode = -1;
        }
    }

    AmbaKAL_MutexTake(&gMutexFeedFile, AMBA_KAL_WAIT_FOREVER);
    AmbaKAL_EventFlagGive(&gDemuxerEndEvent, DEMUXER_FLAG_EVENT_END_PROCESSED);
    AmbaKAL_MutexGive(&gMutexFeedFile);

    gFeedEndHandlingEntered = 0;

    return ErrorCode;
}


#if 0
/*************************************************************************
 * Applib video decoder module declaration
 ************************************************************************/
/**
 * Get the next file ID by current file ID.
 *
 * @param [in] FileNum          Total number of files.
 * @param [in] CurrentFileID    Current file ID.
 * @param [in] PlayingDir       Direction. APPLIB_VIDEO_PLAY_FW - Forward, APPLIB_VIDEO_PLAY_BW - Backward.
 * @param [out] NextFileID      Output. Next file ID.
 *
 * @return 0 - Success, Others - Failure
 */
static int AppLibTranscoderVideoDec_GetNextFileID(const UINT8 FileNum,
                                        const UINT8 CurrentFileID,
                                        const APPLIB_VIDEO_TRANSCODE_DIRECTION_e PlayingDir,
                                        UINT8 *NextFileID)
{
    if (FileNum == 0) { // No file
        *NextFileID = APPLIB_PLAYER_INVALID_FILE_ID; // No next file
        return -1;
    }

    if (CurrentFileID == APPLIB_PLAYER_INVALID_FILE_ID) {
        return -1;
    }

    if (CurrentFileID >= FileNum) { // CurrentFileID is too large
        return -1;
    }

    if (PlayingDir == APPLIB_VIDEO_TRANSCODE_FW) {
        // Play forward
        if (CurrentFileID >= (FileNum - 1)) {
            *NextFileID = APPLIB_PLAYER_INVALID_FILE_ID; // No next file
        } else {
            *NextFileID = CurrentFileID + 1;
        }
    } else {
        // Play backward
        if (CurrentFileID == 0) {
            *NextFileID = APPLIB_PLAYER_INVALID_FILE_ID; // No next file
        } else {
            *NextFileID = CurrentFileID - 1;
        }
    }
    return 0;
}

int AppLibTranscoderVideoDec_CalStartTime(UINT64 VideoTime,UINT32 *StartTime)
{
    // Find target file by start time
    UINT8 FileID = 0; // Index of file (about to be played) in the array
    UINT32 InitTime, Duration; // Information of file
    UINT32 EndTime = FileInfoList[FileNumber - 1].InitTime + FileInfoList[FileNumber - 1].Duration; // End time of the last file
    UINT32 TmpStartTime = 0;

    if (VideoTime < FileInfoList[0].InitTime) {
        TmpStartTime = 0;
        FileID = 0;
    } else if (VideoTime >= EndTime) {
        TmpStartTime = FileInfoList[FileNumber - 1].Duration;
        FileID = FileNumber - 1;
    } else {
        for (FileID = 0; FileID < FileNumber; ++FileID) {
            InitTime = FileInfoList[FileID].InitTime;
            Duration = FileInfoList[FileID].Duration;
            if ((InitTime <= VideoTime) && (VideoTime < (InitTime + Duration))) {
                TmpStartTime = VideoTime - InitTime;
                break;
            }
        }
    }

    *StartTime = TmpStartTime;
    NextPlayingFileID = FileID;
    NextFeedingFileID = FileID;

    return 0;
}

/**
 * Determine whether to feed the next file.
 *
 * @return 0 - Cannot feed, Others - Can feed
 */
static UINT8 AppLibTranscoderVideoDec_CanFeedNextFile(void)
{
    if (NextFeedingFileID == APPLIB_PLAYER_INVALID_FILE_ID) { // There's no file to feed
        return 0; // Cannot feed
    }

    return 1; // Can feed
}

UINT8 AppLibTranscoderVideoDec_PlayEOS(void)
{
    if (AppLibTranscoderVideoDec_CanFeedNextFile() == 0) {
        CurrentFeedingFileID = APPLIB_PLAYER_INVALID_FILE_ID;
    }

    return 0;
}

/*
 * For speed change only. Do not expect any change of direction, time offset or filename.
 */
static int AppLibTranscoderVideoDec_SpeedChange(const UINT32 NextSpeed)
{
    int ReturnValue = 0;
    UINT64 VideoTime = 0;   // Current video time
    UINT32 StartTime = 0;   // Time offset when start playing video

    // Check state
    if (VideoPlayerState == APPLIB_VIDEO_PLAYER_STATE_INVALID) {
        AmbaPrint("[Applib - TranscoderVideoDec] %s:%u Initialize first.", __FUNCTION__, __LINE__);
        return -1; // Error
    }

    switch (VideoPlayerState) {
        case APPLIB_VIDEO_PLAYER_STATE_PLAY: {
            // Preliminary check
            if (AvcDecHdlr == NULL) {
                AmbaPrintColor(RED, "[Applib - TranscoderVideoDec] %s:%u AvcDecHdlr is NULL.", __FUNCTION__, __LINE__);
                return -1; // Error
            }
            if (DecPipeHdlr == NULL) {
                AmbaPrintColor(RED, "[Applib - TranscoderVideoDec] %s:%u DecPipeHdlr is NULL.", __FUNCTION__, __LINE__);
                return -1; // Error
            }
            if (CurrentPlayingSpeed == 0) {
                AmbaPrintColor(RED, "[Applib - TranscoderVideoDec] %s:%u Unexpected speed.", __FUNCTION__, __LINE__);
                return -1; // Error
            }
            if (NextSpeed == CurrentPlayingSpeed) {
                // Do nothing
                return 0; // Success
            }

            // Change speed
            CurrentPlayingSpeed = NextSpeed;

            ReturnValue = AppLibFormatDemuxMp4_Stop();

            // Get current video time
            ReturnValue = AmpVideoDec_GetTime(AvcDecHdlr, &VideoTime);
            if (ReturnValue != AMP_OK) {
                AmbaPrintColor(RED, "[Applib - TranscoderVideoDec] %s:%u Failed to get video time (%d).", __FUNCTION__, __LINE__, ReturnValue);
                return -1; // Error
            }
            // Stop playing video while keeping the last frame on-screen
            ReturnValue = AmpDec_StopWithLastFrm(DecPipeHdlr);
            if (ReturnValue != AMP_OK) {
                AmbaPrintColor(RED, "[Applib - TranscoderVideoDec] %s:%u Failed to stop video with the last frame (%d).", __FUNCTION__, __LINE__, ReturnValue);
                return -1; // Error
            }

            AppLibTranscoderVideoDec_CalStartTime(VideoTime,&StartTime);

            /**Check if need open next file, if need close current file and open next*/
            if (AppLibTranscoderVideoDec_FeedNextFile(StartTime) != 0) {
                return -1; // Error
            }


            // Configure trick play
            Trick.Speed = CurrentPlayingSpeed; // Decode speed. Different form DemuxSpeed.
            Trick.TimeOffsetOfFirstFrame = StartTime;
            if (CurrentPlayingDir == APPLIB_VIDEO_TRANSCODE_FW) {
                Trick.Direction = AMP_VIDEO_PLAY_FW;
            } else {
                Trick.Direction = AMP_VIDEO_PLAY_BW;
            }
            ReturnValue = AppLibFormatDemuxMp4_Start(Trick.TimeOffsetOfFirstFrame,Trick.Direction,APPLIB_TO_DEMUX_SPEED(Trick.Speed));
            if (ReturnValue != AMP_OK) {
                AmbaPrintColor(RED, "[Applib - TranscoderVideoDec] %s:%u Failed to start video (%d).", __FUNCTION__, __LINE__, ReturnValue);
                return ReturnValue;
            }

            {
                UINT8 FileID;
                if (AppLibTranscoderVideoDec_GetNextFileID(FileNumber, NextPlayingFileID, CurrentPlayingDir, &FileID) != 0) {
                    AmbaPrintColor(RED, "[Applib - TranscoderVideoDec] %s:%u Failed to get next file ID.", __FUNCTION__, __LINE__);
                    return -1; // Error
                }

                // Start playing video
                AmbaPrint("[Applib - TranscoderVideoDec] %s: AmpDec_Start start", __FUNCTION__);
                // Play video
                ReturnValue = AmpDec_Start(DecPipeHdlr, &Trick, &Display);
                if (ReturnValue != AMP_OK) {
                    AmbaPrintColor(RED, "[Applib - TranscoderVideoDec] %s:%u Failed to start playing video (%d).", __FUNCTION__, __LINE__, ReturnValue);
                    return ReturnValue;
                }
                AmbaPrint("[Applib - TranscoderVideoDec] %s: AmpDec_Start end", __FUNCTION__);

                //CurrentPlayingFileID = NextPlayingFileID;
                NextPlayingFileID = FileID;
                NextFeedingFileID = FileID;
            }

        }
            break;
        default:
            AmbaPrint("[Applib - TranscoderVideoDec] %s:%u Change speed at wrong state (%d).", __FUNCTION__, __LINE__, VideoPlayerState);
            return -1; // Error
    }

    return 0; // Success
}

int AppLibTranscoderVideoDec_SpeedUp(UINT32 *CurSpeed)
{
    UINT32 NextSpeed = 0; // Next playing speed

    // Check state
    if (VideoPlayerState == APPLIB_VIDEO_PLAYER_STATE_INVALID) {
        AmbaPrint("[Applib - TranscoderVideoDec] %s:%u Initialize first.", __FUNCTION__, __LINE__);
        return -1; // Error
    }

    if (CurrentPlayingSpeed == 0) {
        NextSpeed = APPLIB_PLAYER_NORMAL_SPEED;
    } else {
        NextSpeed = MIN(CurrentPlayingSpeed << 1, APPLIB_PLAYER_MAX_SPEED);
    }

    // Change speed and start playing
    if (AppLibTranscoderVideoDec_SpeedChange(NextSpeed) != 0) {
        return -1; // Error
    }

    // Set output value
    *CurSpeed = CurrentPlayingSpeed;

    return 0; // Success
}

int AppLibTranscoderVideoDec_SpeedDown(UINT32 *CurSpeed)
{
    UINT32 NextSpeed = 0; // Next playing speed

    // Check state
    if (VideoPlayerState == APPLIB_VIDEO_PLAYER_STATE_INVALID) {
        AmbaPrint("[Applib - TranscoderVideoDec] %s:%u Initialize first.", __FUNCTION__, __LINE__);
        return -1; // Error
    }

    if (CurrentPlayingSpeed == 0) {
        NextSpeed = APPLIB_PLAYER_NORMAL_SPEED;
    } else {
        NextSpeed = MAX(CurrentPlayingSpeed >> 1, APPLIB_PLAYER_MIN_SPEED);
    }

    // Change speed and start playing
    if (AppLibTranscoderVideoDec_SpeedChange(NextSpeed) != 0) {
        return -1; // Error
    }

    // Set output value
    *CurSpeed = CurrentPlayingSpeed;

    return 0; // Success
}

int AppLibTranscoderVideoDec_Step(void)
{
    int ReturnValue = 0;

    // Check state
    if (VideoPlayerState == APPLIB_VIDEO_PLAYER_STATE_INVALID) {
        AmbaPrint("[Applib - TranscoderVideoDec] %s:%u Initialize first.", __FUNCTION__, __LINE__);
        return -1; // Error
    }

    if (DecPipeHdlr == NULL) {
        AmbaPrintColor(RED, "[Applib - TranscoderVideoDec] %s:%u DecPipeHdlr is NULL.", __FUNCTION__, __LINE__);
        return -1;
    }

    ReturnValue = AmpDec_Step(DecPipeHdlr);
    if (ReturnValue != AMP_OK) {
        AmbaPrintColor(RED, "[Applib - TranscoderVideoDec] %s:%u Failed to step one frame (%d).", __FUNCTION__, __LINE__, ReturnValue);
        return -1;
    }

    return ReturnValue;
}

int AppLibTranscoderVideoDec_Zoom(const UINT32 Factor, const INT32 X, const INT32 Y)
{
    int ReturnValue = 0;

    // Check state
    if (VideoPlayerState == APPLIB_VIDEO_PLAYER_STATE_INVALID) {
        AmbaPrint("[Applib - TranscoderVideoDec] %s:%u Initialize first.", __FUNCTION__, __LINE__);
        return -1; // Error
    }

    if (DecPipeHdlr == NULL) {
        AmbaPrintColor(RED, "[Applib - TranscoderVideoDec] %s:%u DecPipeHdlr is NULL.", __FUNCTION__, __LINE__);
        return -1;
    }

    switch (VideoPlayerState) {
        case APPLIB_VIDEO_PLAYER_STATE_IDLE:
            AmbaPrint("[Applib - TranscoderVideoDec] %s:%u Zoom video at wrong state (%d).", __FUNCTION__, __LINE__, VideoPlayerState);
            return -1; // Error
        default:
            // Configure display settings
            VideoMagFactor = Factor;
            VideoShiftX = X;
            VideoShiftY = Y;
            // Calculate display area
            if (AppLibTranscoderVideoDec_CalculateDisplayArea(&Display, VideoMagFactor, VideoShiftX, VideoShiftY) != 0) {
                AmbaPrintColor(RED, "[Applib - TranscoderVideoDec] %s:%u Failed to calculate display area.", __FUNCTION__, __LINE__);
                return -1;
            }

            // Play video
            ReturnValue = AmpDec_Start(DecPipeHdlr, NULL, &Display);
            if (ReturnValue != AMP_OK) {
                AmbaPrintColor(RED, "[Applib - TranscoderVideoDec] %s:%u Failed to zoom and start (%d).", __FUNCTION__, __LINE__, ReturnValue);
                return -1;
            }
            break;
    }


    AmbaPrint("[Applib - TranscoderVideoDec] Zoom end");

    return ReturnValue;
}

int AppLibTranscoderVideoDec_GetTime(UINT64 *time)
{
    int ReturnValue = -1;

    if (AvcDecHdlr == NULL) {
        AmbaPrint("[Applib - TranscoderVideoDec] %s:%u AvcDecHdlr is NULL.", __FUNCTION__, __LINE__);
        return -1;
    }

    ReturnValue = AmpVideoDec_GetTime(AvcDecHdlr, time);
    if (ReturnValue != AMP_OK) {
        AmbaPrintColor(RED, "[Applib - TranscoderVideoDec] %s:%u Failed to get current video time (%d).", __FUNCTION__, __LINE__, ReturnValue);
    }

    AmbaPrint("[Applib - TranscoderVideoDec] AppLibVideoDec_GetTime time = %lld", *time);

    return ReturnValue;
}

int AppLibTranscoderVideoDec_SetPtsFrame(UINT32 frameCount,
                               UINT32 timePerFrame,
                               UINT32 timePerSec)
{
    int ReturnValue = -1;
    UINT64 EosTime = 0;
    UINT32 Delta = timePerFrame;

    if (AvcDecHdlr == NULL) {
        AmbaPrint("[Applib - TranscoderVideoDec] %s:%u AvcDecHdlr is NULL.", __FUNCTION__, __LINE__);
        return -1;
    }

    if (frameCount <= 0) {
        AmbaPrint("[Applib - TranscoderVideoDec] %s:%u Invalid nFrameCount %u", __FUNCTION__, __LINE__, frameCount);
        return -1; // Error
    }

    EosTime = (((UINT64) timePerFrame) * (UINT64) (frameCount - 1));

    AmbaPrint(
            "[Applib - TranscoderVideoDec] %s frameCount = %u, timePerFrame = %u, EosTime = %lld, Delta = %u, timePerSec = %u",
            __FUNCTION__, frameCount, timePerFrame, EosTime, Delta, timePerSec);

    ReturnValue = AmpVideoDec_SetEosPts(AvcDecHdlr, EosTime, Delta, timePerSec);
    return ReturnValue;
}

int AppLibTranscoderVideoDec_SetEosPts(UINT64 eosFileTime,
                             UINT32 timePerFrame,
                             UINT32 timePerSec)
{
    int ReturnValue = -1;

    AmbaPrint(
            "[Applib - TranscoderVideoDec] %s eosFileTime = %lld, timePerFrame = %u, timePerSec = %u",
            __FUNCTION__, eosFileTime, timePerFrame, timePerSec);

    ReturnValue = AmpVideoDec_SetEosPts(AvcDecHdlr, eosFileTime, timePerFrame, timePerSec);
    return ReturnValue;
}

/*
static UINT32* CreateAudioOutputCtrl(void)
{
    static UINT32* audOutCtrl = NULL;
    UINT8* audCtrlCacheBufOri;
    UINT8* audCtrlNCacheBufOri;

    AMBA_AUDIO_IO_CREATE_INFO_s IOInfo = { 0 };
    UINT32 sizeOutCacheCtrl, sizeOutNCCtrl;
    AMBA_AUDIO_BUF_INFO_s cachedInfo;
    AMBA_AUDIO_BUF_INFO_s nonCachedInfo;

    AMBA_MEM_CTRL_s OutputCached, OutputNonCached;

    if (audOutCtrl != NULL) {
        return audOutCtrl;
    }

    IOInfo.I2sIndex = 0;
    IOInfo.MaxChNum = 2;
    IOInfo.MaxDmaDescNum = 16;
    IOInfo.MaxDmaSize = 256;
    IOInfo.MaxSampleFreq = 48000;
    sizeOutCacheCtrl = AmbaAudio_OutputCachedSizeQuery(&IOInfo);
    sizeOutNCCtrl = AmbaAudio_OutputNonCachedSizeQuery(&IOInfo);

    if (0) { // TBD somehow it wont work FIX IT!!!!!!!
        if (AmpUtil_GetAlignedPool(APPLIB_G_MMPL, &(OutputCached.pMemAlignedBase), (void**) &audCtrlCacheBufOri, ALIGN_4(sizeOutCacheCtrl), 1 << 6) != AMP_OK) {
            AmbaPrint("[Applib - TranscoderVideoDec] %s:%u Failed to allocate memory.", __FUNCTION__, __LINE__);
            return NULL;
        }

        // NOTE: The buffer of "OutputNonCached" must be allocated from a NON-CACHED buffer.
        if (AmpUtil_GetAlignedPool(APPLIB_G_NC_MMPL, &(OutputNonCached.pMemAlignedBase), (void**) &audCtrlNCacheBufOri, ALIGN_4(sizeOutNCCtrl), 1 << 6) != AMP_OK) {
            AmbaPrint("[Applib - TranscoderVideoDec] %s:%u Failed to allocate memory.", __FUNCTION__, __LINE__);
            return NULL;
        }
        AmbaPrint("==>> audCtrlNCacheBufOri 0x%x", audCtrlNCacheBufOri);
    } else {
        AmbaKAL_MemAllocate(APPLIB_G_MMPL, &OutputCached, sizeOutCacheCtrl, 32);
        // NOTE: The buffer of "OutputNonCached" must be allocated from a NON-CACHED buffer.
        AmbaKAL_MemAllocate(APPLIB_G_NC_MMPL, &OutputNonCached, sizeOutNCCtrl, 32);
    }

    cachedInfo.MaxSize = (sizeOutCacheCtrl);
    cachedInfo.pHead = OutputCached.pMemAlignedBase;
    nonCachedInfo.MaxSize = (sizeOutNCCtrl);
    nonCachedInfo.pHead = OutputNonCached.pMemAlignedBase;

    AmbaPrint("Out Ctrl Mem: 0x%x 0x%x", (UINT32) cachedInfo.pHead, (UINT32) nonCachedInfo.pHead);
    audOutCtrl = AmbaAudio_OutputCreate(&IOInfo, &cachedInfo, &nonCachedInfo);

    return audOutCtrl;
}

static int AudioDec_CodecCB(void* hdlr,
                            UINT32 event,
                            void* info)
{

    return 0;
}
*/

#endif

