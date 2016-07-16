/**
 * @file src/app/connected/applib/src/format/ApplibFormat_MuxMp4.c
 *
 * Implementation of MW Mp4 Muxer utility
 *
 * History:
 *    2013/11/06 - [Martin Lai] created file
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

#include <applib.h>
#include <fifo/Fifo.h>
#include <format/Muxer.h>
#include <format/Mp4Mux.h>
#include <stream/File.h>
#include <AmbaNAND_Def.h>


//#define DEBUG_APPLIB_FORMAT_MUX_MP4
#if defined(DEBUG_APPLIB_FORMAT_MUX_MP4)
#define DBGMSG AmbaPrint
#define DBGMSGc(x) AmbaPrintColor(GREEN,x)
#define DBGMSGc2 AmbaPrintColor
#else
#define DBGMSG(...)
#define DBGMSGc(...)
#define DBGMSGc2(...)
#endif

#define AUDENC_VIDENC_BITSFIFO_SIZE (10 << 20)

static UINT32 MuxMp4VidEncBitsFifoSize = 0;
static UINT32 MuxMp4VidEncDescSize = 0;

static UINT32 PriAudioFIFODelayStartFlag = 0;
static UINT32 SecAudioFIFODelayStartFlag = 0;
static UINT32 MuxOpenSendFlag = 0;

#ifdef CONFIG_APP_ARD
static UINT64 AudioFrameNumber = 0;
static UINT64 AudioFrameNumberSec = 0;
#endif

typedef struct _APPLIB_SPLIT_FILE_CB_s_ {
    int (*CreateAutoSplitFile) (APPLIB_DCF_MEDIA_TYPE_e mediaType, UINT32 objId, char *extName, UINT8 seqNum, char *filename);
    int (*CreateAutoSplitExtendFile) (APPLIB_DCF_MEDIA_TYPE_e mediaType, UINT32 objId, char *extName, UINT8 seqNum, char *filename);
} APPLIB_SPLIT_FILE_CB_s;

static char PriFileName[MAX_FILENAME_LENGTH];
static UINT32 PriFileId = 0;
static char SecFileName[MAX_FILENAME_LENGTH];
static UINT32 SecFileId = 0;

static AMP_INDEX_HDLR_s *MuxMp4IndexPri = NULL;
static AMP_STREAM_HDLR_s *MuxMp4StreamPri = NULL;
static AMP_MUX_FORMAT_HDLR_s *MuxMp4FormatPri = NULL;
static AMP_MOVIE_INFO_s *MuxMp4MoviePri = NULL;

static AMP_INDEX_HDLR_s *MuxMp4IndexSec = NULL;
static AMP_STREAM_HDLR_s *MuxMp4StreamSec = NULL;
static AMP_MUX_FORMAT_HDLR_s *MuxMp4FormatSec = NULL;
static AMP_MOVIE_INFO_s *MuxMp4MovieSec = NULL;
static AMP_MUXER_PIPE_HDLR_s *MuxMp4MuxPipe = NULL;

static AMP_FIFO_HDLR_s *MuxMp4VideoPriFifoHdlr = NULL;
static AMP_FIFO_HDLR_s *MuxMp4VideoSecFifoHdlr = NULL;
static AMP_FIFO_HDLR_s *MuxMp4AudioPriFifoHdlr = NULL;
static AMP_FIFO_HDLR_s *MuxMp4AudioSecFifoHdlr = NULL;

static UINT32 AutoSplitFileType = 0; /**<auto split create file type default = 0, 0: create extend file, 1 :create new file*/
static APPLIB_SPLIT_FILE_CB_s MuxMp4SplitFileCB = {0};


/**
 *  @brief Reset the mp4 muxer's DTS
 *
 *  Reset the mp4 muxer's DTS
 *
 *  @param [in] pTrack information of track
 *
 *  @return >=0 success, <0 failure
 */
static int AppLibFormatMuxMp4_ResetMovie(AMP_MOVIE_INFO_s *pMovie)
{
    UINT8 TrackId;
    UINT64 nMinDTS;
    AMP_MEDIA_TRACK_INFO_s *pMin;
    if (ApplibFormatLib_AdjustDTS(pMovie) != AMP_OK) {
        AmbaPrint("%s, %d", __FUNCTION__, __LINE__);
        return -1;
    }
    pMin = ApplibFormatLib_GetShortestTrack((AMP_MEDIA_INFO_s *)pMovie);
    K_ASSERT(pMin != NULL);
    nMinDTS = pMin->DTS;
    for (TrackId = 0; TrackId < pMovie->TrackCount; TrackId++) {
        AMP_MEDIA_TRACK_INFO_s *pTrack = &pMovie->Track[TrackId];
        pTrack->InitDTS = pTrack->NextDTS = pTrack->DTS = (pTrack->DTS - nMinDTS);
        if (pTrack->TrackType == AMP_MEDIA_TRACK_TYPE_VIDEO) {
            pTrack->Info.Video.RefDTS = pTrack->InitDTS;
            ApplibFormatLib_ResetPTS(&pMovie->Track[TrackId]);
        }
    }
    return 0;
}

#ifdef CONFIG_APP_ARD
#define SPLIT_OFF_TIME  (1000 * 60 * 60)
extern int AppLibEmptyTrackHandler_AddFile(char* filename);
#endif

#define SPLIT_TIME    (1000 * 60 * 25)  // 25 min split
#define SPLIT_SIZE    ((UINT64)3750 * 1024 * 1024)  // 4G split
static UINT32 MuxMp4SplitTime = SPLIT_TIME;
static UINT64 MuxMp4SplitSize = SPLIT_SIZE;

static UINT32 SplitCount = 0;

/* for TEXT track */
extern AMP_AVENC_HDLR_s *VideoEncExtendHdlr;
static AMP_FIFO_HDLR_s *TextTrackPriFifoHdlr = NULL;
static AMP_FIFO_HDLR_s *TextTrackSecFifoHdlr = NULL;

static UINT8 TextTrackEnableFlag = 1;
/*************************************************************************
 * APIs
 ************************************************************************/
static int AppLibFormatMuxMp4_TextTrack_FifoEventCB(void *hdlr, UINT32 event, void* info)
{
    //AmbaPrintColor(MAGENTA,"[Applib - Format] <Mp4Mux_ExtFifoCB>: 0x%x",event);
    if (0) {
        AMP_FIFO_INFO_s tmpInfo;
        AmpFifo_GetInfo((AMP_FIFO_HDLR_s *)hdlr, &tmpInfo);
        //AmbaPrintColor(MAGENTA,"[Applib - Format] <Mp4Mux_ExtFifoCB>: TotalEntries = %d, AvailEntries=%d",tmpInfo.TotalEntries,tmpInfo.AvailEntries);
    }

    switch (event) {
        case AMP_FIFO_EVENT_DATA_READY:
            AppLibFormatMuxMgr_DataReady(hdlr, info, VIDEO_MUXER_HANDLER);
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

static int AppLibFormatMuxMp4_TextTrack_Configure(AMP_MUX_MOVIE_INFO_CFG_s *MovieCfg, AMP_FIFO_HDLR_s *FifoHdlr)
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
    MovieCfg->Track[MovieCfg->TrackCount].TimeScale = VideoBitsBuffer.FrameRate;
    AmbaPrint("<_TextTrack_Configure> TimeScale %d, TimePerFrame %d", MovieCfg->Track[MovieCfg->TrackCount].TimeScale, MovieCfg->Track[MovieCfg->TrackCount].TimePerFrame);
    /* To increase track count. */
    MovieCfg->TrackCount += 1;
    return AMP_OK;
}

static int AppLibFormatMuxMp4_TextTrack_Init(AMP_FIFO_HDLR_s **FifoHdlr)
{
    /* Error Check. */
    if (!AppLibExtendEnc_GetEnableStatus()) {
        AmbaPrintColor(BLUE, "<_TextTrack_Init> Enable extend encode first!");
        return AMP_OK;
    }
    if (!TextTrackEnableFlag) {
        AmbaPrintColor(BLUE, "<_TextTrack_Init> Disable text track.");
        return AMP_OK;
    }

    /* To initialize the text fifo. */
    AMP_FIFO_CFG_s FifoDefCfg;
    AmpFifo_GetDefaultCfg(&FifoDefCfg);
    FifoDefCfg.hCodec = VideoEncExtendHdlr;
    FifoDefCfg.IsVirtual = 1;
    FifoDefCfg.NumEntries = 8192;
    FifoDefCfg.cbEvent = AppLibFormatMuxMp4_TextTrack_FifoEventCB;
    AmpFifo_Create(&FifoDefCfg, FifoHdlr);
    return AMP_OK;
}

static int AppLibFormatMuxMp4_TextTrack_UnInit(AMP_FIFO_HDLR_s *FifoHdlr)
{
    /* To release resource. */
    AmpFifo_Delete(FifoHdlr);

    return AMP_OK;
}

/**
 *  @brief Set text track enable/disable status
 *
 *  Set text track module enable/disable status
 *
 *  @return 0 disable, >1 enable
 */
int AppLibFormatMuxMp4_TextTrack_SetEnableStatus(UINT8 enableTextTrack)
{
    if (!AppLibExtendEnc_GetEnableStatus()) {
        AmbaPrint("<_TextTrack_SetEnableStatus> Please enable extend encode first!");
        return AMP_OK;
    }
    TextTrackEnableFlag = enableTextTrack;
    AmbaPrint("<_TextTrack_SetEnableStatus> Enable flag: %d", TextTrackEnableFlag);
    return AMP_OK;
}

/**
 *  @brief Get text track enable/disable status
 *
 *  Get text track module enable/disable status
 *
 *  @return 0 disable, >1 enable
 */
UINT8 AppLibFormatMuxMp4_TextTrack_GetEnableStatus(void)
{
    AmbaPrint("<_TextTrack_GetEnableStatus> Enable flag: %d", TextTrackEnableFlag);
    return TextTrackEnableFlag;
}

/**
 *  @brief Sync stream data
 *
 *  Sync stream data
 *
 *  @return >=0 success, <0 failure
 */
int AppLibFormatMuxMp4_SyncStream(void)
{
    return MuxMp4StreamPri->Func->Func(MuxMp4StreamPri, AMP_STREAM_OP_SYNC, 0);//Temp
}

/**
 *  @brief create split file
 *
 *   By AutoSplitFileType create split file
 *
 *  @return >=0 success, <0 failure
 */
static int AppLibFormatMuxMp4_CreateAutoSplitFile(APPLIB_DCF_MEDIA_TYPE_e mediaType, UINT32 objId, char *extName,  UINT8 seqNum, char *filename)
{
    int ReturnValue = 0;
    if (AutoSplitFileType == 0) {
        AppLibStorageDmf_CreateFileExtended(mediaType, objId, extName, APPLIB_DCF_EXT_OBJECT_SPLIT_FILE, seqNum, DCIM_HDLR, filename);
    } else {
        PriFileId = AppLibStorageDmf_CreateFile(mediaType, extName, DCIM_HDLR, filename);
    }
    return ReturnValue;
}

/**
 *  @brief create split file
 *
 *   By AutoSplitFileType create split file
 *
 *  @return >=0 success, <0 failure
 */
static int AppLibFormatMuxMp4_CreateAutoSplitExtendFile(APPLIB_DCF_MEDIA_TYPE_e mediaType, UINT32 objId, char *extName, UINT8 seqNum, char *filename)
{
    int ReturnValue = 0;
    if (AutoSplitFileType == 0) {
        AppLibStorageDmf_CreateFileExtended(mediaType, objId, extName, APPLIB_DCF_EXT_OBJECT_SPLIT_THM, seqNum, DCIM_HDLR, filename);
    } else {
        AppLibStorageDmf_CreateFileExtended(mediaType, objId, extName, APPLIB_DCF_EXT_OBJECT_VIDEO_THM, seqNum, DCIM_HDLR, filename);
    }
    return ReturnValue;
}

/**
 *  @brief create split file
 *
 *   By AutoSplitFileType create split file
 *
 *  @return >=0 success, <0 failure
 */
int AppLibFormatMuxMp4_SetAutoSplitFileType(UINT32 Type)
{
    int ReturnValue = 0;
    AutoSplitFileType = Type;
    MuxMp4SplitFileCB.CreateAutoSplitFile = &AppLibFormatMuxMp4_CreateAutoSplitFile;
    MuxMp4SplitFileCB.CreateAutoSplitExtendFile = &AppLibFormatMuxMp4_CreateAutoSplitExtendFile;
    return ReturnValue;
}

/**
 *  @brief Auto split function
 *
 *  Auto split function
 *
 *  @return >=0 success, <0 failure
 */
static int AppLibFormatMuxMp4_AutoSplit(void)
{
    int ReturnValue = 0;
    AMP_MOVIE_INFO_s *Movie;
    char SplitFileName[MAX_FILENAME_LENGTH];
    ReturnValue = MuxMp4FormatPri->Func->Close(MuxMp4FormatPri, AMP_MUX_FORMAT_CLOSE_DEFAULT);
    if (ReturnValue != AMP_OK) {
        AmbaPrintColor(GREEN,"[Applib - Format] <Mp4Mux_AutoSplit> SplitCount = %d Close(MuxMp4FormatPri ReturnValue = %d", SplitCount,ReturnValue);
        K_ASSERT(0);
    }

    if (ReturnValue != AMP_OK) {
        K_ASSERT(0);
    }
    ReturnValue = MuxMp4StreamPri->Func->Close(MuxMp4StreamPri);
    if (ReturnValue != AMP_OK) {
        AmbaPrintColor(GREEN,"[Applib - Format] <Mp4Mux_AutoSplit> SplitCount = %d Close(MuxMp4StreamPri ReturnValue = %d", SplitCount,ReturnValue);
        K_ASSERT(0);
    }
#ifdef CONFIG_APP_ARD
    AppLibFormatTakeMux();
#endif
    /**auto split create new file by call back func*/
    if (MuxMp4SplitFileCB.CreateAutoSplitFile != NULL) {
        MuxMp4SplitFileCB.CreateAutoSplitFile(APPLIB_DCF_MEDIA_VIDEO, PriFileId, "MP4", SplitCount, SplitFileName);
    } else {
        AppLibStorageDmf_CreateFileExtended(APPLIB_DCF_MEDIA_VIDEO, PriFileId, "MP4", APPLIB_DCF_EXT_OBJECT_SPLIT_FILE, SplitCount, DCIM_HDLR, SplitFileName);
    }
#ifdef CONFIG_APP_ARD
    AppLibFormatGiveMux();
#endif
    /* Replace media info */
    ReturnValue = AmpFormat_NewMovieInfo(SplitFileName, &Movie);
    if (ReturnValue < 0) {
        AmbaPrintColor(GREEN,"[Applib - Format] <Mp4Mux_AutoSplit> SplitCount = %d AmpFormat_NewMovieInfo ReturnValue = %d", SplitCount,ReturnValue);
        K_ASSERT(0);
    }
    ReturnValue = AmpFormat_CopyMovieInfo(Movie, MuxMp4MoviePri);
    if (ReturnValue < 0) {
        AmbaPrintColor(GREEN,"[Applib - Format] <Mp4Mux_AutoSplit> SplitCount = %d AmpFormat_CopyMovieInfo ReturnValue = %d", SplitCount,ReturnValue);
        K_ASSERT(0);
    }
    ReturnValue = AmpFormat_RelMovieInfo(MuxMp4MoviePri, TRUE);
    if (ReturnValue < 0) {
        AmbaPrintColor(GREEN,"[Applib - Format] <Mp4Mux_AutoSplit> SplitCount = %d AmpFormat_RelMovieInfo ReturnValue = %d", SplitCount,ReturnValue);
        K_ASSERT(0);
    }

    MuxMp4MoviePri = Movie;
    MuxMp4FormatPri->Media = (AMP_MEDIA_INFO_s *)MuxMp4MoviePri;
    ApplibFormatLib_ResetMuxMediaInfo((AMP_MEDIA_INFO_s *)MuxMp4MoviePri);
    ReturnValue = AppLibFormatMuxMp4_ResetMovie(MuxMp4MoviePri);
    if (ReturnValue < 0) {
        AmbaPrintColor(GREEN,"[Applib - Format] <Mp4Mux_AutoSplit> SplitCount = %d AppLibFormatMuxMp4_ResetMovie ReturnValue = %d", SplitCount,ReturnValue);
        K_ASSERT(0);
    }
#ifdef CONFIG_APP_ARD
    AppLibFormatTakeMux();
#endif
    MuxMp4StreamPri->Func->Open(MuxMp4StreamPri, SplitFileName, AMP_STREAM_MODE_WRONLY);
#ifdef CONFIG_APP_ARD
    AppLibFormatGiveMux();
#endif
    MuxMp4FormatPri->Func->Open(MuxMp4FormatPri);
    if (AppLibVideoEnc_GetDualStreams() && AppLibFormat_GetDualFileSaving()) {
        ReturnValue = MuxMp4FormatSec->Func->Close(MuxMp4FormatSec, AMP_MUX_FORMAT_CLOSE_DEFAULT);
        if (ReturnValue != AMP_OK) {
            AmbaPrintColor(GREEN,"[Applib - Format] <Mp4Mux_AutoSplit> SplitCount = %d Close(MuxMp4FormatPri ReturnValue = %d", SplitCount,ReturnValue);
            K_ASSERT(0);
        }

        if (ReturnValue != AMP_OK) {
            K_ASSERT(0);
        }
        ReturnValue = MuxMp4StreamSec->Func->Close(MuxMp4StreamSec);
        if (ReturnValue != AMP_OK) {
            AmbaPrintColor(GREEN,"[Applib - Format] <Mp4Mux_AutoSplit> SplitCount = %d Close(MuxMp4StreamPri ReturnValue = %d", SplitCount,ReturnValue);
            K_ASSERT(0);
        }
        /**auto split create new file by call back func*/
        if (MuxMp4SplitFileCB.CreateAutoSplitExtendFile != NULL) {
            MuxMp4SplitFileCB.CreateAutoSplitExtendFile(APPLIB_DCF_MEDIA_VIDEO, PriFileId, "MP4", SplitCount, SplitFileName);
        } else {
            AppLibStorageDmf_CreateFileExtended(APPLIB_DCF_MEDIA_VIDEO, SecFileId, "MP4", APPLIB_DCF_EXT_OBJECT_SPLIT_THM, SplitCount, DCIM_HDLR, SplitFileName);
        }
        /* Replace media info */
        ReturnValue = AmpFormat_NewMovieInfo(SplitFileName, &Movie);
        if (ReturnValue < 0) {
            AmbaPrintColor(GREEN,"[Applib - Format] <Mp4Mux_AutoSplit> SplitCount = %d AmpFormat_NewMovieInfo ReturnValue = %d", SplitCount,ReturnValue);
            K_ASSERT(0);
        }
        ReturnValue = AmpFormat_CopyMovieInfo(Movie, MuxMp4MovieSec);
        if (ReturnValue < 0) {
            AmbaPrintColor(GREEN,"[Applib - Format] <Mp4Mux_AutoSplit> SplitCount = %d AmpFormat_CopyMovieInfo ReturnValue = %d", SplitCount,ReturnValue);
            K_ASSERT(0);
        }
        ReturnValue = AmpFormat_RelMovieInfo(MuxMp4MovieSec, TRUE);
        if (ReturnValue < 0) {
            AmbaPrintColor(GREEN,"[Applib - Format] <Mp4Mux_AutoSplit> SplitCount = %d AmpFormat_RelMovieInfo ReturnValue = %d", SplitCount,ReturnValue);
            K_ASSERT(0);
        }

        MuxMp4MovieSec = Movie;
        MuxMp4FormatSec->Media = (AMP_MEDIA_INFO_s *)MuxMp4MovieSec;
        ApplibFormatLib_ResetMuxMediaInfo((AMP_MEDIA_INFO_s *)MuxMp4MovieSec);
        ReturnValue = AppLibFormatMuxMp4_ResetMovie(MuxMp4MovieSec);
        if (ReturnValue < 0) {
            AmbaPrintColor(GREEN,"[Applib - Format] <Mp4Mux_AutoSplit> SplitCount = %d AppLibFormatMuxMp4_ResetMovie ReturnValue = %d", SplitCount,ReturnValue);
            K_ASSERT(0);
        }
#ifdef CONFIG_APP_ARD
        AppLibFormatTakeMux();
#endif
        MuxMp4StreamSec->Func->Open(MuxMp4StreamSec, SplitFileName, AMP_STREAM_MODE_WRONLY);
#ifdef CONFIG_APP_ARD
        AppLibFormatGiveMux();
#endif
        MuxMp4FormatSec->Func->Open(MuxMp4FormatSec);
    }

    SplitCount++;
    return ReturnValue;
}

/**
 *  @brief Force split function
 *
 *  Set the max duration to force clip split

 *  @param [in] SplitTime
 *  @return >=0 success, <0 failure
 */
int AppLibFormatMuxMp4_ForceSplit(int SplitTime)
{
    int ReturnValue = 0;
    MuxMp4FormatPri->Param.Movie.MaxDuration = SplitTime*1000;
    return ReturnValue;
}


/**
 *  @brief Mp4 muxer's call back function
 *
 *  Mp4 muxer's call back function
 *
 *  @param [in] hdlr Handler
 *  @param [in] event Event
 *  @param [in] info Infomation
 *
 *  @return >=0 success, <0 failure
 */
static int AppLibFormatMuxMp4_MuxCB(void* hdlr, UINT32 event, void* info)
{
    int ReturnValue = 0;

    switch (event) {
    case AMP_MUXER_EVENT_START:
        AppLibComSvcHcmgr_SendMsgNoWait(HMSG_MUXER_START, 0, 0);
        DBGMSG("[Applib - Format] <Mp4Mux_MuxCB> AMP_MUXER_EVENT_START");
        break;
    case AMP_MUXER_EVENT_END:
#ifdef CONFIG_APP_ARD
        if(info != NULL){
            /*If info == AMP_ERROR_FIFO_EMPTY, indicate that the current movie has empty track,it is invalid,need to delete it.*/
            /*Will Delete it in handler.c when received HMSG_DCF_FILE_CLOSE*/
            if((*(AMP_ER_CODE_e*)info) == AMP_ERROR_FIFO_EMPTY){
                if(MuxMp4MoviePri != NULL){
                    if(AppLibComSvcHcmgr_SendMsgNoWait(HMSG_MUXER_END_HAS_EMPTY_TRACK, 0, 0) != AMP_OK){
                        AmbaPrintColor(RED,"%s:%u failure.", __FUNCTION__, __LINE__);
                    }
                    if(AppLibEmptyTrackHandler_AddFile(((AMP_MEDIA_INFO_s *)MuxMp4MoviePri)->Name) != AMP_OK){
                        AmbaPrintColor(RED,"%s:%u failure.", __FUNCTION__, __LINE__);
                    }
                    AmbaPrintColor(RED,"EmptyTrack:%s",((AMP_MEDIA_INFO_s *)MuxMp4MoviePri)->Name);
                }

                if (AppLibVideoEnc_GetDualStreams() && AppLibFormat_GetDualFileSaving()) {
                    if(MuxMp4MovieSec != NULL){
                        if(AppLibComSvcHcmgr_SendMsgNoWait(HMSG_MUXER_END_HAS_EMPTY_TRACK, 0, 0) != AMP_OK){
                            AmbaPrintColor(RED,"%s:%u failure.", __FUNCTION__, __LINE__);
                        }
                        if(AppLibEmptyTrackHandler_AddFile(((AMP_MEDIA_INFO_s *)MuxMp4MovieSec)->Name) != AMP_OK){
                            AmbaPrintColor(RED,"%s:%u failure.", __FUNCTION__, __LINE__);
                        }
                        AmbaPrintColor(RED,"EmptyTrackSec:%s",((AMP_MEDIA_INFO_s *)MuxMp4MovieSec)->Name);
                    }
                }
            }
        }
#endif
        AppLibComSvcHcmgr_SendMsgNoWait(HMSG_MUXER_END, 0, 0);
        MuxOpenSendFlag = 0;
        DBGMSG("[Applib - Format] <Mp4Mux_MuxCB> AMP_MUXER_EVENT_END");
        break;
    case AMP_MUXER_EVENT_REACH_LIMIT:
#ifdef CONFIG_APP_ARD
        if (MuxMp4SplitTime == SPLIT_OFF_TIME && MuxMp4SplitSize == SPLIT_SIZE) {
            /*Workaround,enlarge MaxSize.need to remove...*/
            MuxMp4FormatPri->Param.Movie.MaxSize = SPLIT_SIZE+(200*1024*1024);
#else
        if (MuxMp4SplitTime == 0 && MuxMp4SplitSize == 0) {
#endif
            AppLibComSvcHcmgr_SendMsgNoWait(HMSG_MUXER_REACH_LIMIT, 1, 0);
        } else {
            ReturnValue = AppLibFormatMuxMp4_AutoSplit();
            AppLibComSvcHcmgr_SendMsgNoWait(HMSG_MUXER_REACH_LIMIT, 0, 0);
        }
        DBGMSG("[Applib - Format] <Mp4Mux_MuxCB> AMP_MUXER_EVENT_REACH_LIMIT");
        break;
    case AMP_MUXER_EVENT_IO_ERROR:
        AppLibComSvcHcmgr_SendMsgNoWait(HMSG_MUXER_IO_ERROR, 0, 0);
        DBGMSG("[Applib - Format] <Mp4Mux_MuxCB> AMP_MUXER_EVENT_IO_ERROR");
        break;
    case AMP_MUXER_EVENT_FIFO_ERROR:
        AppLibComSvcHcmgr_SendMsgNoWait(HMSG_MUXER_FIFO_ERROR, 0, 0);
        DBGMSG("[Applib - Format] <Mp4Mux_MuxCB> AMP_MUXER_EVENT_FIFO_ERROR");
        break;
    case AMP_MUXER_EVENT_GENERAL_ERROR:
        AppLibComSvcHcmgr_SendMsgNoWait(HMSG_MUXER_GENERAL_ERROR, 0, 0);
        DBGMSG("[Applib - Format] <Mp4Mux_MuxCB> AMP_MUXER_EVENT_GENERAL_ERROR");


        break;
    default:
        AmbaPrint("[Applib - Format] <Mp4Mux_MuxCB> Unknown event %X info: %x", event, info);
        break;
    }
    return ReturnValue;
}


/**
 *  @brief Start the mp4 muxer
 *
 *  Start the mp4 muxer
 *
 *  @return >=0 success, <0 failure
 */
int AppLibFormatMuxMp4_Open(void)
{
    int ReturnValue = 0;


    DBGMSG("[Applib - Format] <Mp4MuxOpen> %s Open", __FUNCTION__);


    if (MuxMp4StreamPri == NULL) {
        AMP_FILE_STREAM_CFG_s FileCfg;
        memset(&FileCfg, 0x0, sizeof(AMP_FILE_STREAM_CFG_s));
#ifdef CONFIG_APP_ARD
        AppLibFormatTakeMux();
#endif
        ReturnValue = AppLibStorageDmf_CreateFile(APPLIB_DCF_MEDIA_VIDEO, "MP4", DCIM_HDLR, PriFileName);
#ifdef CONFIG_APP_ARD
        AppLibFormatGiveMux();
#endif
        if (ReturnValue <= 0) {
            AmbaPrintColor(RED,"[Applib - Format] <Mp4MuxOpen> %s:%u Get filename failure.", __FUNCTION__, __LINE__);
            return ReturnValue;
        } else {
            PriFileId = ReturnValue;
        }

        /* Open file stream */
        AmpFileStream_GetDefaultCfg(&FileCfg);
        FileCfg.Async = TRUE;
        FileCfg.BytesToSync = 0x1000;
        FileCfg.AsyncParam.MaxBank = 2;
        FileCfg.Alignment = AppLibFormat_GetPriStreamFileSizeAlignment();
        DBGMSG("[Applib - Format] <Mp4MuxOpen> FileCfg.nBytesToSync = 0x%x",FileCfg.nBytesToSync);
        AmbaPrint("[Applib - Format] <Mp4MuxOpen> FileCfg..AsyncParam.MaxBank = 0x%x",FileCfg.AsyncParam.MaxBank);
        AmpFileStream_Create(&FileCfg, &MuxMp4StreamPri);
#ifdef CONFIG_APP_ARD
        AppLibFormatTakeMux();
#endif
        if (MuxMp4StreamPri->Func->Open(MuxMp4StreamPri, PriFileName, AMP_STREAM_MODE_WRONLY) != AMP_OK) {
            AmbaPrintColor(RED,"[Applib - Format] <Mp4MuxOpen> %s:%u", __FUNCTION__, __LINE__);
#ifdef CONFIG_APP_ARD
            AppLibFormatGiveMux();
#endif
            return -1;
        }
#ifdef CONFIG_APP_ARD
        AppLibFormatGiveMux();
#endif
    }


    if (MuxMp4MoviePri == NULL) {
        extern UINT8 *H264EncBitsBuf;
        extern UINT8 *AudPriBitsBuf;
        APPLIB_SENSOR_VIDEO_ENC_CONFIG_s *VideoEncConfigData = AppLibSysSensor_GetVideoConfig(AppLibVideoEnc_GetSensorVideoRes());
        APPLIB_VIDEOENC_GOP_s *VideoEncGOPData = AppLibSysSensor_GetVideoGOP(AppLibVideoEnc_GetSensorVideoRes());
        AMP_MUX_MOVIE_INFO_CFG_s MovieCfg;
        memset(&MovieCfg, 0x0, sizeof(AMP_MUX_MOVIE_INFO_CFG_s));


        /* Open media info */
        ReturnValue = AmpFormat_NewMovieInfo(PriFileName, &MuxMp4MoviePri);
        if (ReturnValue < 0) {
            AmbaPrintColor(RED,"[Applib - Format] <Mp4MuxOpen> AmpFormat_NewMovieInfo fail ReturnValue = %d",ReturnValue);
            return ReturnValue;
        }

        /* Set media info */
        AmpMuxer_GetDefaultMovieInfoCfg(&MovieCfg);
        MovieCfg.Track[0].TrackType = AMP_MEDIA_TRACK_TYPE_VIDEO;
        MovieCfg.Track[0].Fifo = MuxMp4VideoPriFifoHdlr;
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
            MovieCfg.Track[1].Fifo = MuxMp4AudioPriFifoHdlr;
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
        if (TextTrackPriFifoHdlr) {
            AppLibFormatMuxMp4_TextTrack_Configure(&MovieCfg, TextTrackPriFifoHdlr);
        }

        /* To initialize muer according to the movie configures */
        ReturnValue = AmpMuxer_InitMovieInfo(MuxMp4MoviePri, &MovieCfg);
        if (ReturnValue < 0) {
            AmbaPrintColor(RED,"[Applib - Format] <Mp4MuxOpen> AmpMuxer_InitMovieInfo fail ReturnValue = %d",ReturnValue);
            return ReturnValue;
        } else {
#if 1
            AmbaPrintColor(GREEN,"[Applib - Format] <Mp4MuxOpen> Main file");
            AmbaPrint("[Applib - Format] <Mp4MuxOpen> MovieCfg.Track[0].Fifo = 0x%x",MovieCfg.Track[0].Fifo);
            AmbaPrint("[Applib - Format] <Mp4MuxOpen> MovieCfg.Track[0].TimeScale = %d",MovieCfg.Track[0].TimeScale);
            AmbaPrint("[Applib - Format] <Mp4MuxOpen> MovieCfg.Track[0].TimePerFrame = %d",MovieCfg.Track[0].TimePerFrame);
            AmbaPrint("[Applib - Format] <Mp4MuxOpen> MovieCfg.Track[0].Info.Video.M = %d",MovieCfg.Track[0].Info.Video.M);
            AmbaPrint("[Applib - Format] <Mp4MuxOpen> MovieCfg.Track[0].Info.Video.N = %d",MovieCfg.Track[0].Info.Video.N);
            AmbaPrint("[Applib - Format] <Mp4MuxOpen> MovieCfg.Track[0].Info.Video.GOPSize = %d",MovieCfg.Track[0].Info.Video.GOPSize);
            AmbaPrint("[Applib - Format] <Mp4MuxOpen> MovieCfg.Track[0].Info.Video.Width = %d",MovieCfg.Track[0].Info.Video.Width);
            AmbaPrint("[Applib - Format] <Mp4MuxOpen> MovieCfg.Track[0].Info.Video.Height = %d",MovieCfg.Track[0].Info.Video.Height);
            AmbaPrint("[Applib - Format] <Mp4MuxOpen> MovieCfg.Track[0].Info.Video.CodecTimeScale = %d",MovieCfg.Track[0].Info.Video.CodecTimeScale);
            AmbaPrint("[Applib - Format] <Mp4MuxOpen> MovieCfg.Track[1].MediaId = %d",MovieCfg.Track[1].MediaId);
            AmbaPrint("[Applib - Format] <Mp4MuxOpen> MovieCfg.Track[1].Info.Audio.SampleRate = %d",MovieCfg.Track[1].Info.Audio.SampleRate);
            AmbaPrint("[Applib - Format] <Mp4MuxOpen> MovieCfg.Track[1].Info.Audio.Channels = %d",MovieCfg.Track[1].Info.Audio.Channels);
#endif
        }
    }

    /* Create Index */
    if (MuxMp4IndexPri == NULL) {
        AppLibIndex_CreateHdlr(&MuxMp4IndexPri);
    }

    if (MuxMp4FormatPri == NULL) {
        AMP_MP4_MUX_CFG_s Mp4Cfg;
        AmbaPrint("[Applib - Format] <Mp4MuxOpen> Mp4 Mux Create");
        AmpMp4Mux_GetDefaultCfg(&Mp4Cfg);
        Mp4Cfg.MaxIdxNum = 256;
        Mp4Cfg.Stream = MuxMp4StreamPri;
        Mp4Cfg.Index = MuxMp4IndexPri;
        Mp4Cfg.TrickRecDivisor = 1;
        if (AmpMp4Mux_Create(&Mp4Cfg, &MuxMp4FormatPri) != AMP_OK) {
            AmbaPrintColor(RED,"[Applib - Format] <Mp4MuxOpen> %s:%u", __FUNCTION__, __LINE__);
            return -1;
        }
    }


    if (AppLibVideoEnc_GetDualStreams() && AppLibFormat_GetDualFileSaving()) {
        if (MuxMp4StreamSec == NULL) {
            AMP_FILE_STREAM_CFG_s FileCfg;
            memset(&FileCfg, 0x0, sizeof(AMP_FILE_STREAM_CFG_s));

            ReturnValue = AppLibStorageDmf_CreateFileExtended(APPLIB_DCF_MEDIA_VIDEO, PriFileId, "MP4", APPLIB_DCF_EXT_OBJECT_VIDEO_THM, 0, DCIM_HDLR, SecFileName);
            if (ReturnValue < 0) {
                AmbaPrintColor(RED,"[Applib - Format] <Mp4MuxOpen> %s:%u Get filename failure.", __FUNCTION__, __LINE__);
                return ReturnValue;
            } else {
                SecFileId = PriFileId;
            }
            /* Open file stream */
            AmpFileStream_GetDefaultCfg(&FileCfg);
            FileCfg.Async = TRUE;
            FileCfg.BytesToSync = 0x1000;
            FileCfg.AsyncParam.MaxBank = 2;
            FileCfg.Alignment = AppLibFormat_GetSecStreamFileSizeAlignment();
            ReturnValue = AmpFileStream_Create(&FileCfg, &MuxMp4StreamSec);
            if (ReturnValue < 0) {
                AmbaPrintColor(RED,"[Applib - Format] <Mp4MuxOpen> AmpFileStream_Create fail %s:%u", __FUNCTION__, __LINE__);
                return ReturnValue;
            }  else {
#ifdef CONFIG_APP_ARD
            AppLibFormatTakeMux();
#endif
                if (MuxMp4StreamSec->Func->Open(MuxMp4StreamSec, SecFileName, AMP_STREAM_MODE_WRONLY) != AMP_OK) {
                    AmbaPrintColor(RED,"[Applib - Format] <Mp4MuxOpen> %s:%u", __FUNCTION__, __LINE__);
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

        if (MuxMp4MovieSec == NULL) {
            extern UINT8 *H264EncBitsBuf;
            AMP_MUX_MOVIE_INFO_CFG_s MovieCfg;

            memset(&MovieCfg, 0x0, sizeof(AMP_MUX_MOVIE_INFO_CFG_s));

            /* Open media info */
            ReturnValue = AmpFormat_NewMovieInfo(SecFileName, &MuxMp4MovieSec);
            if (ReturnValue < 0) {
                AmbaPrintColor(RED,"[Applib - Format] <Mp4MuxOpen> AmpFormat_NewMovieInfo fail ReturnValue = %d",ReturnValue);
                return ReturnValue;
            }

            /* Set media info */
            AmpMuxer_GetDefaultMovieInfoCfg(&MovieCfg);

            MovieCfg.Track[0].TrackType = AMP_MEDIA_TRACK_TYPE_VIDEO;
            MovieCfg.Track[0].Fifo = MuxMp4VideoSecFifoHdlr;
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
                MovieCfg.Track[1].Fifo = MuxMp4AudioSecFifoHdlr;
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
            if (TextTrackSecFifoHdlr) {
                AppLibFormatMuxMp4_TextTrack_Configure(&MovieCfg, TextTrackSecFifoHdlr);
            }

            ReturnValue = AmpMuxer_InitMovieInfo(MuxMp4MovieSec, &MovieCfg);
            if (ReturnValue < 0) {
                AmbaPrintColor(RED,"[Applib - Format] <Mp4MuxOpen> AmpMuxer_InitMovieInfo fail ReturnValue = %d",ReturnValue);
                return ReturnValue;
            } else {
#if 1
                AmbaPrintColor(GREEN,"[Applib - Format] <Mp4MuxOpen> Sec file");
                AmbaPrint("[Applib - Format] <Mp4MuxOpen> MovieCfg.Track[0].Fifo = 0x%x",MovieCfg.Track[0].Fifo);
                AmbaPrint("[Applib - Format] <Mp4MuxOpen> MovieCfg.Track[0].Info.Video.TimeScale = %d",MovieCfg.Track[0].TimeScale);
                AmbaPrint("[Applib - Format] <Mp4MuxOpen> MovieCfg.Track[0].Info.Video.TimePerFrame = %d",MovieCfg.Track[0].TimePerFrame);
                AmbaPrint("[Applib - Format] <Mp4MuxOpen> MovieCfg.Track[0].Info.Video.M = %d",MovieCfg.Track[0].Info.Video.M);
                AmbaPrint("[Applib - Format] <Mp4MuxOpen> MovieCfg.Track[0].Info.Video.N = %d",MovieCfg.Track[0].Info.Video.N);
                AmbaPrint("[Applib - Format] <Mp4MuxOpen> MovieCfg.Track[0].Info.Video.GOPSize = %d",MovieCfg.Track[0].Info.Video.GOPSize);
                AmbaPrint("[Applib - Format] <Mp4MuxOpen> MovieCfg.Track[0].Info.Video.Width = %d",MovieCfg.Track[0].Info.Video.Width);
                AmbaPrint("[Applib - Format] <Mp4MuxOpen> MovieCfg.Track[0].Info.Video.Height = %d",MovieCfg.Track[0].Info.Video.Height);
#endif
            }
        }
        /* Create Index */
        if (MuxMp4IndexSec == NULL) {
            AppLibIndex_CreateHdlr(&MuxMp4IndexSec);
        }
        if (MuxMp4FormatSec == NULL) {
            AMP_MP4_MUX_CFG_s Mp4Cfg;
            AmbaPrint("[Applib - Format] <Mp4MuxOpen> Mp4 Mux Create");
            AmpMp4Mux_GetDefaultCfg(&Mp4Cfg);
            Mp4Cfg.MaxIdxNum = 256;
            Mp4Cfg.Stream = MuxMp4StreamSec;
            Mp4Cfg.Index = MuxMp4IndexSec;
            Mp4Cfg.TrickRecDivisor = 1;
            if (AmpMp4Mux_Create(&Mp4Cfg, &MuxMp4FormatSec) != AMP_OK) {
                AmbaPrintColor(RED,"[Applib - Format] <Mp4MuxOpen> %s:%u", __FUNCTION__, __LINE__);
                return -1;
            }
        }
    }


    /* Create muxer pipe */
    if (MuxMp4MuxPipe == NULL) {
        AMP_MUXER_PIPE_CFG_s MuxPipeCfg;
        AmpMuxer_GetDefaultCfg(&MuxPipeCfg);
        MuxPipeCfg.Format[0] = MuxMp4FormatPri;
        MuxPipeCfg.Media[0] = (AMP_MEDIA_INFO_s *)MuxMp4MoviePri;
        if (AppLibVideoEnc_GetDualStreams() && AppLibFormat_GetDualFileSaving()) {
            MuxPipeCfg.FormatCount = 2;
            MuxPipeCfg.Format[1] = MuxMp4FormatSec;
            MuxPipeCfg.Media[1] = (AMP_MEDIA_INFO_s *)MuxMp4MovieSec;
        } else {
            MuxPipeCfg.FormatCount = 1;
        }

        {
            UINT32 SplitTime = 0;
            UINT64 SplitSize = 0;
            AppLibVideoEnc_GetSplitTimeSize(&SplitTime, &SplitSize);
            if ( SplitTime == 0 && SplitSize == 0) {
#ifdef CONFIG_APP_ARD
        /*We do not use time to split when split is off,set as 60 min*/
                MuxMp4SplitTime = SPLIT_OFF_TIME;
#else
                MuxMp4SplitTime = SPLIT_TIME;
#endif
                MuxMp4SplitSize = SPLIT_SIZE;
            } else {
                MuxMp4SplitTime = SplitTime;
                MuxMp4SplitSize = SplitSize;
            }
        }
        MuxPipeCfg.MaxDuration = MuxMp4SplitTime;
        MuxPipeCfg.MaxSize= MuxMp4SplitSize;

        MuxPipeCfg.OnEvent = AppLibFormatMuxMp4_MuxCB;
        AmpMuxer_Create(&MuxPipeCfg, &MuxMp4MuxPipe);
        AmpMuxer_Add(MuxMp4MuxPipe);
        AmpMuxer_Start(MuxMp4MuxPipe, 0);
    }

    DBGMSG("[Applib - Format] <Mp4MuxOpen> %s End", __FUNCTION__);
    return 0;
}

/**
 *  @brief Register the callback function in the EXIF muxer manager.
 *
 *  Register the callback function in the EXIF muxer manager.
 *
 *  @return >=0 success, <0 failure
 */
int AppLibFormatMuxMp4_RegMuxMgr(void)
{
    APPLIB_MUX_MGR_HANDLER_s Handler = {0};

    DBGMSG("[Applib - Format] <MuxMp4_RegMuxMgr>");
    memset(&Handler, 0x0, sizeof(APPLIB_MUX_MGR_HANDLER_s));
    Handler.MuxerInit = &AppLibFormat_Mp4MuxerInit;
    Handler.MuxerOpen = &AppLibFormatMuxMp4_Open;
    Handler.MuxerClose = &AppLibFormatMuxMp4_Close;
    Handler.DataReadyNum = 1;
    Handler.Used = 1;
    Handler.Type = VIDEO_MUXER_HANDLER;

    return AppLibFormatMuxMgr_RegMuxHandler(&Handler);
}

/**
 *  @brief Unregister the callback function in the EXIF muxer manager.
 *
 *  Register the callback function in the EXIF muxer manager.
 *
 *  @return >=0 success, <0 failure
 */
int AppLibFormatMuxMp4_UnRegMuxMgr(void)
{
    APPLIB_MUX_MGR_HANDLER_s Handler = {0};

    DBGMSG("[Applib - Format] <MuxMp4_RegMuxMgr>");
    memset(&Handler, 0x0, sizeof(APPLIB_MUX_MGR_HANDLER_s));
    Handler.MuxerInit = &AppLibFormat_Mp4MuxerInit;
    Handler.MuxerOpen = &AppLibFormatMuxMp4_Open;
    Handler.MuxerClose = &AppLibFormatMuxMp4_Close;
    Handler.DataReadyNum = 1;
    Handler.Used = 0;
    Handler.Type = VIDEO_MUXER_HANDLER;

    return AppLibFormatMuxMgr_UnRegMuxHandler(&Handler);
}

//static int FrameAudioNumberPri = 0;
//static int FrameAudioNumberSec = 0;
/**
 *  @brief Mp4 muxer's fifo call back function
 *
 *  Mp4 muxer's fifo call back function
 *
 *  @param [in] hdlr Handler
 *  @param [in] event Event
 *  @param [in] info Infomation
 *
 *  @return >=0 success, <0 failure
 */
int  AppLibFormatMuxMp4_AudioFifoCB(void *hdlr, UINT32 event, void* info)
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
            AppLibFormatMuxMgr_DataReady(hdlr, info, VIDEO_MUXER_HANDLER);
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
        AmbaPrint("[Applib - Format] <Mp4Mux_AudioFifoCB>: evnet 0x%x", event);
        break;
    }

    return 0;
}

//static int FrameNumberPri = 0;
//static int FrameNumberSec = 0;
/**
 *  @brief Mp4 muxer's fifo call back function
 *
 *  Mp4 muxer's fifo call back function
 *
 *  @param [in] hdlr Handler
 *  @param [in] event Event
 *  @param [in] info Infomation
 *
 *  @return >=0 success, <0 failure
 */
int  AppLibFormatMuxMp4_VideoFifoCB(void *hdlr, UINT32 event, void* info)
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
        /**create text fifo*/
        if (TextTrackPriFifoHdlr == NULL && PriAudioFIFODelayStartFlag == 1) {
            AppLibFormatMuxMp4_TextTrack_Init(&TextTrackPriFifoHdlr);
        }

        if (TextTrackSecFifoHdlr == NULL && SecAudioFIFODelayStartFlag == 1) {
            AppLibFormatMuxMp4_TextTrack_Init(&TextTrackSecFifoHdlr);
        }
        /**Resume audio FIFO when receive video data reay*/
        if (PriAudioFIFODelayStartFlag == 1 && MuxMp4VideoPriFifoHdlr == hdlr) {
            AMP_FIFO_CFG_INIT_DATA_s InitData;
            AMP_BITS_DESC_s *desc;
            APPLIB_SENSOR_VIDEO_ENC_CONFIG_s *VideoEncConfigData = AppLibSysSensor_GetVideoConfig(AppLibVideoEnc_GetSensorVideoRes());


            AmpFifo_PeekEntry(hdlr, &desc,0);
            InitData.InitCondition = AMP_FIFO_CFG_INIT_WITH_START_TIME;
            InitData.InitParam.StartTime = (desc->Pts*1000)/VideoEncConfigData->EncNumerator;

            /**resume audio fifo*/
            AmpFifo_Resume(MuxMp4AudioPriFifoHdlr,&InitData);
            AmbaPrintColor(CYAN,"[Applib - Format] <Mp4Mux_VideoFifoCB> Resume main audio fifo 0x%x",MuxMp4AudioPriFifoHdlr);
            PriAudioFIFODelayStartFlag = 0;
        }
        if (SecAudioFIFODelayStartFlag == 1 && MuxMp4VideoSecFifoHdlr == hdlr) {
            AMP_FIFO_CFG_INIT_DATA_s InitData;
            AMP_BITS_DESC_s *desc;


            AmpFifo_PeekEntry(hdlr, &desc,0);
            InitData.InitCondition = AMP_FIFO_CFG_INIT_WITH_START_TIME;
            InitData.InitParam.StartTime = (desc->Pts*1000)/AppLibVideoEnc_GetSecStreamTimeScale();

            /**resume audio fifo*/
            AmpFifo_Resume(MuxMp4AudioSecFifoHdlr,&InitData);
            AmbaPrintColor(CYAN,"[Applib - Format] <Mp4Mux_VideoFifoCB> Resume second audio fifo 0x%x",MuxMp4AudioSecFifoHdlr);
            SecAudioFIFODelayStartFlag = 0;
        }
        /**all stream receive at least one frame*/
        if (SecAudioFIFODelayStartFlag == 0 && PriAudioFIFODelayStartFlag == 0 && MuxOpenSendFlag == 0) {
            /**Send muxer open message to app, record can be stop after this message*/
            AppLibComSvcHcmgr_SendMsgNoWait(HMSG_MUXER_OPEN, 0, 0);
            MuxOpenSendFlag = 1;
        }

        AppLibFormatMuxMgr_DataReady(hdlr, info, VIDEO_MUXER_HANDLER);

        //AmbaPrintColor(GREEN,"[Applib - Format] <Mp4Mux_VideoFifoCB>: AMP_FIFO_EVENT_DATA_READY hdlr = 0x%x",hdlr);
        break;
    case AMP_FIFO_EVENT_DATA_EOS:
        //AmbaPrintColor(YELLOW,"[Applib - Format] <Mp4Mux_VideoFifoCB>: AMP_FIFO_EVENT_DATA_EOS hdlr = 0x%x",hdlr);
        AppLibFormatMuxMgr_DataEos(hdlr, info);
        break;
    default:
        AmbaPrint("[Applib - Format] <Mp4Mux_VideoFifoCB>: evnet 0x%x", event);
        break;
    }

    return 0;
}
/**
 *  @brief Start the mp4 muxer
 *
 *  Start the mp4 muxer
 *
 *  @return >=0 success, <0 failure
 */
int AppLibFormatMuxMp4_Start(void)
{
    int ReturnValue = 0;
#ifdef CONFIG_APP_ARD
    APPLIB_SENSOR_VIDEO_ENC_CONFIG_s *VideoEncConfigData = AppLibSysSensor_GetVideoConfig(AppLibVideoEnc_GetSensorVideoRes());
    double AudioSampleTimeSlot = 0;
#endif

    AmbaPrint("[Applib - Format] <Mp4MuxStart> %s Start", __FUNCTION__);

    AppLibRecorderMemMgr_GetBufSize(&MuxMp4VidEncBitsFifoSize, &MuxMp4VidEncDescSize);

    {
        AppLibFormat_Mp4MuxerInit();
        AppLibFormatMuxMgr_Init();
        AppLibFormatMuxMp4_RegMuxMgr();

        /* Create a virtual fifo for primary stream. */
        // Video track
        if (MuxMp4VideoPriFifoHdlr == NULL) {
            extern AMP_AVENC_HDLR_s *VideoEncPri;
            AMP_FIFO_CFG_s FifoDefCfg;
            memset(&FifoDefCfg, 0x0, sizeof(AMP_FIFO_CFG_s));
            AmpFifo_GetDefaultCfg(&FifoDefCfg);
            AmbaPrint("[Applib - Format] <Mp4MuxStart> VideoEncPri = %d",VideoEncPri);
#ifdef CONFIG_APP_ARD
            FifoDefCfg.hCodec = VideoEncPri;
            FifoDefCfg.IsVirtual = 1;
            FifoDefCfg.NumEntries = 8192;
            FifoDefCfg.cbEvent = AppLibFormatMuxMp4_VideoFifoCB;
            FifoDefCfg.InitData.CreateFifoWithInitData = 1; //--
            FifoDefCfg.InitData.InitCondition = AMP_FIFO_CFG_INIT_WITH_NUM_FRAME; //--
            FifoDefCfg.InitData.InitParam.NumFrame = 0; //--
            FifoDefCfg.InitData.FristFrameType = AMP_FIFO_TYPE_IDR_FRAME; //--
            FifoDefCfg.TickPerSecond = VideoEncConfigData->EncNumerator;

#else
            FifoDefCfg.hCodec = VideoEncPri;
            FifoDefCfg.IsVirtual = 1;
            FifoDefCfg.NumEntries = 8192;
            FifoDefCfg.cbEvent = AppLibFormatMuxMp4_VideoFifoCB;
#endif
            AmpFifo_Create(&FifoDefCfg, &MuxMp4VideoPriFifoHdlr);
        }
        // Audio track
        if (AppLibVideoEnc_GetRecMode() == REC_MODE_AV) {
            if (MuxMp4AudioPriFifoHdlr == NULL) {
                extern AMP_AVENC_HDLR_s *AudioEncPriHdlr;
                AMP_FIFO_CFG_s FifoDefCfg;
                AmpFifo_GetDefaultCfg(&FifoDefCfg);
#ifdef CONFIG_APP_ARD
                FifoDefCfg.hCodec = AudioEncPriHdlr;
                FifoDefCfg.IsVirtual = 1;
                FifoDefCfg.NumEntries = 8192;
                FifoDefCfg.cbEvent = AppLibFormatMuxMp4_AudioFifoCB;
                FifoDefCfg.InitData.CreateFifoWithInitData = 1; //--
                FifoDefCfg.InitData.InitCondition = AMP_FIFO_CFG_INIT_PAUSED; //--
                FifoDefCfg.InitData.InitParam.NumFrame= 0;   //--
                FifoDefCfg.InitData.FristFrameType = AMP_FIFO_TYPE_AUDIO_FRAME; //--
                FifoDefCfg.TickPerSecond = 90000;
#else
                FifoDefCfg.hCodec = AudioEncPriHdlr;
                FifoDefCfg.IsVirtual = 1;
                FifoDefCfg.NumEntries = 8192;
                FifoDefCfg.cbEvent = AppLibFormatMuxMp4_AudioFifoCB;
#endif
                AmpFifo_Create(&FifoDefCfg, &MuxMp4AudioPriFifoHdlr);
#ifdef CONFIG_APP_ARD
                PriAudioFIFODelayStartFlag = 1;
#endif

            }
        }

        //Text track
        if (TextTrackPriFifoHdlr == NULL) {
            AppLibFormatMuxMp4_TextTrack_Init(&TextTrackPriFifoHdlr);
        }

        /* Create a virtual fifo for secondary stream. */
        if (AppLibVideoEnc_GetDualStreams() && AppLibFormat_GetDualFileSaving()) {
            // Tideo track
            if (MuxMp4VideoSecFifoHdlr == NULL) {
                extern AMP_AVENC_HDLR_s *VideoEncSec;
                AMP_FIFO_CFG_s FifoDefCfg;
                AmpFifo_GetDefaultCfg(&FifoDefCfg);
                FifoDefCfg.hCodec = VideoEncSec;
                FifoDefCfg.IsVirtual = 1;
                FifoDefCfg.NumEntries = 8192;
                FifoDefCfg.cbEvent = AppLibFormatMuxMp4_VideoFifoCB;
                AmpFifo_Create(&FifoDefCfg, &MuxMp4VideoSecFifoHdlr);
            }
            // Audio track
            if (AppLibVideoEnc_GetRecMode() == REC_MODE_AV) {
                if (MuxMp4AudioSecFifoHdlr == NULL) {
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
                    FifoDefCfg.cbEvent = AppLibFormatMuxMp4_AudioFifoCB;
                    AmpFifo_Create(&FifoDefCfg, &MuxMp4AudioSecFifoHdlr);
                }
            }
            //Text track
            if (TextTrackSecFifoHdlr == NULL) {
                AppLibFormatMuxMp4_TextTrack_Init(&TextTrackSecFifoHdlr);
            }
        }
    }
    AmbaPrint("[Applib - Format] <Mp4MuxStart> %s End", __FUNCTION__);
    return ReturnValue;
}

/**
 *  @brief Start the mp4 muxer after starting record
 *
 *  Start the mp4 muxer
 *
 *  @return >=0 success, <0 failure
 */
int AppLibFormatMuxMp4_StartOnRecording(void)
{
    int ReturnValue = 0;
    APPLIB_SENSOR_VIDEO_ENC_CONFIG_s *VideoEncConfigData = AppLibSysSensor_GetVideoConfig(AppLibVideoEnc_GetSensorVideoRes());
#ifdef CONFIG_APP_ARD
    double AudioSampleTimeSlot = 0;
#endif

    AmbaPrint("[Applib - Format] <StartOnRecording> %s Start", __FUNCTION__);
    AppLibRecorderMemMgr_GetBufSize(&MuxMp4VidEncBitsFifoSize, &MuxMp4VidEncDescSize);

    {
        AppLibFormat_Mp4MuxerInit();
        AppLibFormatMuxMgr_Init();
        AppLibFormatMuxMp4_RegMuxMgr();

        /* Create a virtual fifo for primary stream. */
        // Video track
        if (MuxMp4VideoPriFifoHdlr == NULL) {
            extern AMP_AVENC_HDLR_s *VideoEncPri;
            AMP_FIFO_CFG_s FifoDefCfg;
#ifdef CONFIG_APP_ARD
            APPLIB_SENSOR_VIDEO_ENC_CONFIG_s *VideoEncConfigData = AppLibSysSensor_GetVideoConfig(AppLibVideoEnc_GetSensorVideoRes());
#endif
            memset(&FifoDefCfg, 0x0, sizeof(AMP_FIFO_CFG_s));
            AmpFifo_GetDefaultCfg(&FifoDefCfg);
            AmbaPrint("[Applib - Format] <Mp4MuxStart> VideoEncPri = 0x%08X",VideoEncPri);
            FifoDefCfg.hCodec = VideoEncPri;
            FifoDefCfg.IsVirtual = 1;
            FifoDefCfg.NumEntries = 8192;
            FifoDefCfg.cbEvent = AppLibFormatMuxMp4_VideoFifoCB;
            FifoDefCfg.InitData.CreateFifoWithInitData = 1; //--
            FifoDefCfg.InitData.InitCondition = AMP_FIFO_CFG_INIT_WITH_NUM_FRAME; //--
#ifdef CONFIG_APP_ARD
            if(AppLibVideoEnc_GetPreRecord() == 1){
              FifoDefCfg.InitData.InitParam.NumFrame = ((VideoEncConfigData->EncNumerator*AppLibVideoEnc_GetPreRecordTime())/VideoEncConfigData->EncDenominator); //---
              FifoDefCfg.TickPerSecond = VideoEncConfigData->EncNumerator;
              FifoDefCfg.InitData.FristFrameType = AMP_FIFO_TYPE_IDR_FRAME; //--
            }else{
              FifoDefCfg.InitData.InitParam.NumFrame = 0; //--
              FifoDefCfg.InitData.FristFrameType = AMP_FIFO_TYPE_IDR_FRAME; //--
              FifoDefCfg.TickPerSecond = VideoEncConfigData->EncNumerator;
            }
#else
            FifoDefCfg.InitData.InitParam.NumFrame = 0; //--
            FifoDefCfg.InitData.FristFrameType = AMP_FIFO_TYPE_IDR_FRAME; //--
            FifoDefCfg.TickPerSecond = VideoEncConfigData->EncNumerator;
#endif

            AmpFifo_Create(&FifoDefCfg, &MuxMp4VideoPriFifoHdlr);
                    AmbaPrintColor(CYAN,"[Applib - Format] <Mp4MuxStart> Create video FIFO");
#ifdef CONFIG_APP_ARD
            if(AppLibVideoEnc_GetPreRecord() == 1){
             AudioSampleTimeSlot = (1000*((double)1024/(double)AppLibAudioEnc_GetSrcSampleRate()));
             AudioFrameNumber = (FifoDefCfg.InitData.OnCreateTimeLength/AudioSampleTimeSlot);
            }
#endif

        }
        // Audio track
        if (AppLibVideoEnc_GetRecMode() == REC_MODE_AV) {
            if (MuxMp4AudioPriFifoHdlr == NULL) {
                /**resume audio and create text fifo when receive video data ready to avoid av not sync
                due to video need to wait IDR frame to start record*/

                extern AMP_AVENC_HDLR_s *AudioEncPriHdlr;
                AMP_FIFO_CFG_s FifoDefCfg;
                AmpFifo_GetDefaultCfg(&FifoDefCfg);
                FifoDefCfg.hCodec = AudioEncPriHdlr;
                FifoDefCfg.IsVirtual = 1;
                FifoDefCfg.NumEntries = 8192;
                FifoDefCfg.cbEvent = AppLibFormatMuxMp4_AudioFifoCB;
                FifoDefCfg.InitData.CreateFifoWithInitData = 1; //--
                FifoDefCfg.InitData.InitCondition = AMP_FIFO_CFG_INIT_PAUSED; //--
                FifoDefCfg.InitData.InitParam.NumFrame= 0;   //--
                FifoDefCfg.InitData.FristFrameType = AMP_FIFO_TYPE_AUDIO_FRAME; //--
                FifoDefCfg.TickPerSecond = 90000;
                AmpFifo_Create(&FifoDefCfg, &MuxMp4AudioPriFifoHdlr);
                AmbaPrintColor(CYAN,"[Applib - Format] <StartOnRecording>: Create audio FIFO 0x%x",MuxMp4AudioPriFifoHdlr);

                PriAudioFIFODelayStartFlag = 1;
            }
        } else {
            /** If video only, text fifo can direct create without wait video frame.
                If av mode, due to text fifo may receive data ready early than video will cause audio fifo not be create before muxer open
                so delay text fifo until received video data ready*/
            // Text track
            if (TextTrackPriFifoHdlr == NULL) {
                AppLibFormatMuxMp4_TextTrack_Init(&TextTrackPriFifoHdlr);
            }
        }
        /* Create a virtual fifo for secondary stream. */
        if (AppLibVideoEnc_GetDualStreams() && AppLibFormat_GetDualFileSaving()) {
            if (MuxMp4VideoSecFifoHdlr == NULL) {
                extern AMP_AVENC_HDLR_s *VideoEncSec;
                AMP_FIFO_CFG_s FifoDefCfg;
                AmpFifo_GetDefaultCfg(&FifoDefCfg);
                FifoDefCfg.hCodec = VideoEncSec;
                FifoDefCfg.IsVirtual = 1;
                FifoDefCfg.NumEntries = 8192;
                FifoDefCfg.cbEvent = AppLibFormatMuxMp4_VideoFifoCB;
                FifoDefCfg.InitData.CreateFifoWithInitData = 1; //--
                FifoDefCfg.InitData.InitCondition = AMP_FIFO_CFG_INIT_WITH_NUM_FRAME;    //--
#ifdef CONFIG_APP_ARD
                if(AppLibVideoEnc_GetPreRecord() == 1){
                   FifoDefCfg.InitData.InitParam.NumFrame = ((AppLibVideoEnc_GetSecStreamTimeScale()*AppLibVideoEnc_GetPreRecordTime())/AppLibVideoEnc_GetSecStreamTick());   //--
                   FifoDefCfg.TickPerSecond = AppLibVideoEnc_GetSecStreamTimeScale();
                   FifoDefCfg.InitData.FristFrameType = AMP_FIFO_TYPE_IDR_FRAME; //--
                }else{
                   FifoDefCfg.InitData.InitParam.NumFrame = 0; //--
                   FifoDefCfg.InitData.FristFrameType = AMP_FIFO_TYPE_IDR_FRAME; //--
                   FifoDefCfg.TickPerSecond = AppLibVideoEnc_GetSecStreamTimeScale();
                }
#else
                FifoDefCfg.InitData.InitParam.NumFrame = 0; //--
                FifoDefCfg.InitData.FristFrameType = AMP_FIFO_TYPE_IDR_FRAME; //--
                FifoDefCfg.TickPerSecond = AppLibVideoEnc_GetSecStreamTimeScale();
#endif

                AmpFifo_Create(&FifoDefCfg, &MuxMp4VideoSecFifoHdlr);

#ifdef CONFIG_APP_ARD
                if(AppLibVideoEnc_GetPreRecord() == 1)
                AudioFrameNumberSec = (FifoDefCfg.InitData.OnCreateTimeLength/AudioSampleTimeSlot);
#endif
            }
            if (AppLibVideoEnc_GetRecMode() == REC_MODE_AV) {
                if (MuxMp4AudioSecFifoHdlr == NULL) {
                    /**resume audio and create text fifo when receive video data ready to avoid av not sync
                    due to video need to wait IDR frame to start record*/
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
                    FifoDefCfg.cbEvent = AppLibFormatMuxMp4_AudioFifoCB;
                    FifoDefCfg.InitData.CreateFifoWithInitData = 1; //--
                    FifoDefCfg.InitData.InitCondition = AMP_FIFO_CFG_INIT_PAUSED; //--
                    FifoDefCfg.InitData.InitParam.NumFrame = 0;   //--
                    FifoDefCfg.InitData.FristFrameType = AMP_FIFO_TYPE_AUDIO_FRAME; //--
                    FifoDefCfg.TickPerSecond = 90000;
                    AmpFifo_Create(&FifoDefCfg, &MuxMp4AudioSecFifoHdlr);
                    AmbaPrintColor(CYAN,"[Applib - Format] <StartOnRecording>: Create audio FIFO 0x%x",MuxMp4AudioSecFifoHdlr);

                    SecAudioFIFODelayStartFlag = 1;
                }
            } else {
                //Text track
                if (TextTrackSecFifoHdlr == NULL) {
                    AppLibFormatMuxMp4_TextTrack_Init(&TextTrackSecFifoHdlr);
                }
            }
        }
    }
    AmbaPrint("[Applib - Format] <StartOnRecording> %s End", __FUNCTION__);
    return ReturnValue;
}


/**
 *  @brief Close the mp4 muxer
 *
 *  Close the mp4 muxer
 *
 *  @return >=0 success, <0 failure
 */
int AppLibFormatMuxMp4_Close(void)
{
    int ReturnValue = 0;

    AmbaPrint("[Applib - Format] <Mp4MuxClose> %s delay Start", __FUNCTION__);
    ReturnValue = AmpMuxer_WaitComplete(MuxMp4MuxPipe, AMBA_KAL_WAIT_FOREVER);
    AmbaPrint("[Applib - Format] <Mp4MuxClose> AmpMuxer_WaitComplete = %d", ReturnValue);
    AmbaPrint("[Applib - Format] <Mp4MuxClose> %s Start", __FUNCTION__);

    AppLibFormatMuxMp4_UnRegMuxMgr();

    /* Remove the Muxer pipe. */
    if (MuxMp4MuxPipe != NULL) {
        AmpMuxer_Remove(MuxMp4MuxPipe);
        AmpMuxer_Delete(MuxMp4MuxPipe);
        MuxMp4MuxPipe = NULL;
    }

    /* Delete the Mp4 muxer. */
    if (MuxMp4FormatPri != NULL) {
        ReturnValue = AmpMp4Mux_Delete(MuxMp4FormatPri);
        if (ReturnValue < 0) {
            AmbaPrintColor(RED,"[Applib - Format] <Mp4MuxClose> AmpMp4Mux_Delete fail ReturnValue = %d",ReturnValue);
        }
    }

    /* Delete the Mp4 muxer. */
    if (MuxMp4IndexPri != NULL) {
        AppLibIndex_DeleteHdlr(MuxMp4IndexPri);
        MuxMp4IndexPri = NULL;
    }
    MuxMp4FormatPri = NULL;

    /* Close and delete the stream. */
    if (MuxMp4StreamPri != NULL) {
        MuxMp4StreamPri->Func->Close(MuxMp4StreamPri);
        AmpFileStream_Delete(MuxMp4StreamPri);
        MuxMp4StreamPri = NULL;
    }

    /* Release the movie information. */
    if (MuxMp4MoviePri != NULL) {
        AmpFormat_RelMovieInfo(MuxMp4MoviePri, TRUE);
        MuxMp4MoviePri = NULL;
    }


    /* Delete the Mp4 muxer. */
    if (MuxMp4FormatSec != NULL) {
        ReturnValue = AmpMp4Mux_Delete(MuxMp4FormatSec);
        if (ReturnValue < 0) {
            AmbaPrintColor(RED,"[Applib - Format] <Mp4MuxClose> AmpMp4Mux_Delete fail ReturnValue = %d",ReturnValue);
        }
    }

    /* Delete the Mp4 muxer. */
    if (MuxMp4IndexSec != NULL) {
        AppLibIndex_DeleteHdlr(MuxMp4IndexSec);
        MuxMp4IndexSec = NULL;
    }
    MuxMp4FormatSec = NULL;

    /* Close and delete the stream. */
    if (MuxMp4StreamSec != NULL) {
        MuxMp4StreamSec->Func->Close(MuxMp4StreamSec);
        AmpFileStream_Delete(MuxMp4StreamSec);
        MuxMp4StreamSec = NULL;
    }

    /* Release the movie information. */
    if (MuxMp4MovieSec != NULL) {
        AmpFormat_RelMovieInfo(MuxMp4MovieSec, TRUE);
        MuxMp4MovieSec = NULL;
    }


    if (MuxMp4VideoPriFifoHdlr != NULL) {
        AmpFifo_Delete(MuxMp4VideoPriFifoHdlr);
        MuxMp4VideoPriFifoHdlr = NULL;
    }
    if (MuxMp4VideoSecFifoHdlr != NULL) {
        AmpFifo_Delete(MuxMp4VideoSecFifoHdlr);
        MuxMp4VideoSecFifoHdlr = NULL;
    }
    if (MuxMp4AudioPriFifoHdlr != NULL) {
        AmpFifo_Delete(MuxMp4AudioPriFifoHdlr);
        MuxMp4AudioPriFifoHdlr = NULL;
    }
    if (MuxMp4AudioSecFifoHdlr != NULL) {
        AmpFifo_Delete(MuxMp4AudioSecFifoHdlr);
        MuxMp4AudioSecFifoHdlr = NULL;
    }
    // Text track
    if (TextTrackPriFifoHdlr != NULL) {
        AppLibFormatMuxMp4_TextTrack_UnInit(TextTrackPriFifoHdlr);
        TextTrackPriFifoHdlr = NULL;
    }
    if (TextTrackSecFifoHdlr != NULL) {
        AppLibFormatMuxMp4_TextTrack_UnInit(TextTrackSecFifoHdlr);
        TextTrackSecFifoHdlr = NULL;
    }
    AmbaPrint("[Applib - Format] <Mp4MuxClose> %s Stop", __FUNCTION__);
    return 0;
}

/**
 *  @brief Close the mp4 muxer
 *
 *  Close the mp4 muxer
 *
 *  @return >=0 success, <0 failure
 */
int AppLibFormatMuxMp4_StreamError(void)
{
    int ReturnValue = 0;

    AppLibFormatMuxMp4_UnRegMuxMgr();

    AmbaPrint("[Applib - Format] <StreamError> %s Start", __FUNCTION__);
    /* Remove the Muxer pipe. */
    if (MuxMp4MuxPipe != NULL) {
        AmpMuxer_Remove(MuxMp4MuxPipe);
        AmpMuxer_Delete(MuxMp4MuxPipe);
        MuxMp4MuxPipe = NULL;
    }

    /* Delete the Mp4 muxer. */
    if (MuxMp4FormatPri != NULL) {
        ReturnValue = AmpMp4Mux_Delete(MuxMp4FormatPri);
        if (ReturnValue < 0) {
            AmbaPrintColor(RED,"[Applib - Format] <Mp4MuxClose> AmpMp4Mux_Delete fail ReturnValue = %d",ReturnValue);
        }
    }

    /* Delete the Mp4 muxer. */
    if (MuxMp4IndexPri != NULL) {
        AppLibIndex_DeleteHdlr(MuxMp4IndexPri);
        MuxMp4IndexPri = NULL;
    }
    /* Delete the Mp4 muxer. */
    MuxMp4FormatPri = NULL;

    /* Close and delete the stream. */
    if (MuxMp4StreamPri != NULL) {
        MuxMp4StreamPri->Func->Close(MuxMp4StreamPri);
        AmpFileStream_Delete(MuxMp4StreamPri);
        MuxMp4StreamPri = NULL;
    }

    /* Release the movie information. */
    if (MuxMp4MoviePri != NULL) {
        AmpFormat_RelMovieInfo(MuxMp4MoviePri, TRUE);
        MuxMp4MoviePri = NULL;
    }


    /* Delete the Mp4 muxer. */
    if (MuxMp4FormatSec != NULL) {
        ReturnValue = AmpMp4Mux_Delete(MuxMp4FormatSec);
        if (ReturnValue < 0) {
            AmbaPrintColor(RED,"[Applib - Format] <Mp4MuxClose> AmpMp4Mux_Delete fail ReturnValue = %d",ReturnValue);
        }
    }

    /* Delete the Mp4 muxer. */
    MuxMp4FormatSec = NULL;

    /* Close and delete the stream. */
    if (MuxMp4StreamSec != NULL) {
        MuxMp4StreamSec->Func->Close(MuxMp4StreamSec);
        AmpFileStream_Delete(MuxMp4StreamSec);
        MuxMp4StreamSec = NULL;
    }

    /* Release the movie information. */
    if (MuxMp4MovieSec != NULL) {
        AmpFormat_RelMovieInfo(MuxMp4MovieSec, TRUE);
        MuxMp4MovieSec = NULL;
    }


    if (MuxMp4VideoPriFifoHdlr != NULL) {
        AmpFifo_Delete(MuxMp4VideoPriFifoHdlr);
        MuxMp4VideoPriFifoHdlr = NULL;
    }
    if (MuxMp4VideoSecFifoHdlr != NULL) {
        AmpFifo_Delete(MuxMp4VideoSecFifoHdlr);
        MuxMp4VideoSecFifoHdlr = NULL;
    }
    if (MuxMp4AudioPriFifoHdlr != NULL) {
        AmpFifo_Delete(MuxMp4AudioPriFifoHdlr);
        MuxMp4AudioPriFifoHdlr = NULL;
    }
    if (MuxMp4AudioSecFifoHdlr != NULL) {
        AmpFifo_Delete(MuxMp4AudioSecFifoHdlr);
        MuxMp4AudioSecFifoHdlr = NULL;
    }
    // Text track
    if (TextTrackPriFifoHdlr != NULL) {
        AppLibFormatMuxMp4_TextTrack_UnInit(TextTrackPriFifoHdlr);
        TextTrackPriFifoHdlr = NULL;
    }
    if (TextTrackSecFifoHdlr != NULL) {
        AppLibFormatMuxMp4_TextTrack_UnInit(TextTrackSecFifoHdlr);
        TextTrackSecFifoHdlr = NULL;
    }
#ifdef CONFIG_APP_ARD
    MuxOpenSendFlag = 0;
#endif
    AmbaPrint("[Applib - Format] <Mp4MuxClose> %s Stop", __FUNCTION__);
    return 0;
}

#ifdef CONFIG_APP_ARD
static int AppLibFormatMuxMp4_SetMaxDuration(UINT32 maxDuration)
{
    if(MuxMp4MuxPipe != NULL){
        return AmpMuxer_SetMaxDuration(MuxMp4MuxPipe,maxDuration);
    }else{
        AmbaPrintColor(RED,"%s:%u Error", __FUNCTION__,__LINE__);
        return -1;
    }
}

int AppLibFormatMuxMp4_SetMaxDurationMax(void)
{
    int ReturnValue = AppLibFormatMuxMp4_SetMaxDuration(SPLIT_TIME);
    AmbaPrint("%s(), rVal=%d", __FUNCTION__,ReturnValue);
    return ReturnValue;
}
#endif

