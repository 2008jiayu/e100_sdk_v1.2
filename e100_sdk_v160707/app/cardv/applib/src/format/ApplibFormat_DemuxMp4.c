/**
 * @file src/app/connected/applib/src/format/ApplibFormat_DemuxMp4.c
 *
 * Implementation of MW Mp4 Demuxer utility
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
#include <format/Demuxer.h>
#include <format/Mp4Dmx.h>
#include <stream/File.h>
#include <player/Decode.h>
#include <comsvc/misc/util.h>
#include <AmbaUtility.h>
#include "format/ApplibFormat_DemuxMp4.h"

//#define DEBUG_APPLIB_FORMAT_DEMUX_MP4
#if defined(DEBUG_APPLIB_FORMAT_DEMUX_MP4)
#define DBGMSG AmbaPrint
#define DBGMSGc(x) AmbaPrintColor(GREEN,x)
#define DBGMSGc2 AmbaPrintColor
#else
#define DBGMSG(...)
#define DBGMSGc(...)
#define DBGMSGc2(...)
#endif

//#define MP4_FN  L"C:\\TEST.MP4"

static AMP_STREAM_HDLR_s *DemuxMp4Stream = NULL;
static UINT8 DemuxMp4StreamIsOpen = 0; // Whether the DemuxMp4Stream handler has opened a file successsfully and has not closed it.
static AMP_MOVIE_INFO_s *DemuxMp4Movie = NULL;

//!!! SYNC ISSUE need a cmd to setup

#define VIDEODEC_RAW_DESC_NUM (512) // descriptor default number = 128
#define AUDIODEC_RAW_DESC_NUM (512) // descriptor default number = 128
static AMP_FIFO_HDLR_s *DemuxMp4VidFifoHdlr = NULL;
static AMP_FIFO_HDLR_s *DemuxMp4AudFifoHdlr = NULL;

static AMP_DEMUXER_PIPE_HDLR_s *DemuxMp4Pipe = NULL;
static AMP_DMX_FORMAT_HDLR_s *DemuxMp4Format = NULL;
static void *Mp4BufRaw = NULL;

static AMP_CALLBACK_f DemuxCb;

static int ApplibFormatDeMuxerMp4InitFlag = -1;
static int ApplibFormatDeMuxerMp4StartFlag = -1; // Stop

static void* VideoCodecHdlr = NULL;
static void* AudioCodecHdlr = NULL;
static void* VidRawBuf;
static UINT32 SzVidRawBuf;
static void* AudRawBuf;
static UINT32 SzAudRawBuf;

int AppLibFormatDemuxMp4_Init(void)
{
    if (ApplibFormatDeMuxerMp4InitFlag == 0) {
        return 0;
    }
    /* Initial Mp4 Demux */
    {
        AMP_MP4_DMX_INIT_CFG_s Mp4InitCfg;
        AmpMp4Dmx_GetInitDefaultCfg(&Mp4InitCfg);
        Mp4InitCfg.BufferSize = AmpMp4Dmx_GetRequiredBufferSize(Mp4InitCfg.MaxHdlr);
        if (AmpUtil_GetAlignedPool(APPLIB_G_MMPL, (void**) &(Mp4InitCfg.Buffer), &Mp4BufRaw, Mp4InitCfg.BufferSize, 1 << 5) != OK) {
            AmbaPrintColor(RED,"[Applib - Format] <Mp4DemuxerInit> %s:%u", __FUNCTION__, __LINE__);
            Mp4BufRaw = NULL;
            return -1;
        }
        if (AmpMp4Dmx_Init(&Mp4InitCfg) != AMP_OK) {
            AmbaPrintColor(RED,"[Applib - Format] <Mp4DemuxerInit> %s:%u", __FUNCTION__, __LINE__);
            AmbaKAL_BytePoolFree(Mp4BufRaw);
            Mp4BufRaw = NULL;
            return -1;
        }
        ApplibFormatDeMuxerMp4InitFlag = 0;
        ApplibFormatDeMuxerMp4StartFlag = -1; // Stop
    }
    return 0;
}

static int AppLibFormatDemuxMp4_DemuxerCallBack(void *hdlr,
                                                UINT32 event,
                                                void* info)
{
//    AmbaPrint("Demolib - Format] <DemuxerCallBack> %s on Event: 0x%x", __FUNCTION__, event);
    switch (event) {
    case AMP_DEMUXER_EVENT_START:
        AmbaPrint("[Applib - Format] <Mp4DemuxerCallBack> AMP_DEMUXER_EVENT_START");
        if (DemuxCb) {
            DemuxCb(hdlr, event, info);
        }
        break;
    case AMP_DEMUXER_EVENT_END:
        AmbaPrint("[Applib - Format] <Mp4DemuxerCallBack> AMP_DEMUXER_EVENT_END");
        if (DemuxCb) {
            DemuxCb(hdlr, event, info);
        }
        break;
    default:
        AmbaPrintColor(RED, "[Applib - Format] <Mp4DemuxerCallBack> Unknown event %X info: %x", event, info);
#if 0
        if (event == AMP_DEMUXER_EVENT_GENERAL_ERROR) {
            AmbaPrint("[Applib - Format] <DemuxerCallBack> AMP_DEMUXER_EVENT_GENERAL_ERROR!");
        } else if (event == AMP_DEMUXER_EVENT_IO_ERROR) {
            AmbaPrint("[Applib - Format] <DemuxerCallBack> AMP_DEMUXER_EVENT_IO_ERROR!");
        } else if (event == AMP_DEMUXER_EVENT_FIFO_ERROR) {
            AmbaPrint("[Applib - Format] <DemuxerCallBack> AMP_DEMUXER_EVENT_FIFO_ERROR!");
        } else {
            AmbaPrint("[Applib - Format] <DemuxerCallBack> Unknown event %X info: %x", event, info);
        }
#endif
        break;
    }
    return 0;
}

int AppLibFormatDemuxMp4_SetCodecHdlrInfo(void* vidCodecHdlr,
                                          void* audCodecHdlr,
                                          void* vidRawBuf,
                                          UINT32 szVidRawBuf,
                                          void* audRawBuf,
                                          UINT32 szAudRawBuf)
{
    VideoCodecHdlr = vidCodecHdlr;
    AudioCodecHdlr = audCodecHdlr;
    VidRawBuf = vidRawBuf;
    SzVidRawBuf = szVidRawBuf;
    AudRawBuf = audRawBuf;
    SzAudRawBuf = szAudRawBuf;
    return 0;
}

/**
 *  @brief Demuxer's fifo call back function
 *
 *  Demuxer's fifo call back function
 *
 *  @param [in] hdlr Handler
 *  @param [in] event Event
 *  @param [in] info Infomation
 *
 *  @return >=0 success, <0 failure
 */
static int AppLibFormatDemuxMp4_DemuxerVideoFifoCB(void *hdlr,
                                              UINT32 event,
                                              void* info)
{
    AmbaPrint("[Applib - Format] <Mp4DemuxerFifoCB> %s on Event: 0x%x", __FUNCTION__, event);

    return 0;
}

/**
 *  @brief Demuxer's fifo call back function
 *
 *  Demuxer's fifo call back function
 *
 *  @param [in] hdlr Handler
 *  @param [in] event Event
 *  @param [in] info Infomation
 *
 *  @return >=0 success, <0 failure
 */
static int AppLibFormatDemuxMp4_DemuxerAudioFifoCB(void *hdlr,
                                              UINT32 event,
                                              void* info)
{
    AmbaPrint("[Applib - Format] <Mp4DemuxerFifoCB> %s on Event: 0x%x", __FUNCTION__, event);

    return 0;
}

static UINT32 MovieWidth = 0;
static UINT32 MovieHeight = 0;
int AppLibFormatDemuxMp4_GetMovieSize(UINT32 *width,
                                      UINT32 *height)
{
    *width = MovieWidth;
    *height = MovieHeight;
    return 0;
}

/**
 *  @brief Open the mp4 demuxer
 *
 *  Open the mp4 demuxer
 *
 *  @param [in] szName          File name
 *  @param [in] StartTime       Start time
 *  @param [in] Direction       Play direction
 *  @param [in] Speed           Play speed
 *  @param [in] IsErase         Whether erase the data processed before and reset read/write pointer of raw buffer
 *  @param [in] TimeOffset      Time offset (in ms) for each frame
 *  @param [in] IsFeedEos       Feed EOS to DSP at the end of the file
 *
 *  @return >=0 success, <0 failure
 */
static int AppLibFormatDemuxMp4_Mp4DemuxOpen(char *szName,
                                             UINT32 StartTime,
                                             UINT32 Speed,
                                             UINT8 IsErase,
                                             UINT32 TimeOffset,
                                             UINT8 IsFeedEos)
{
    int ReturnValue = 0; // Return value
    int Rval = 0; // Function call return. It's NOT the return of this function.

    DBGMSG("[Applib - Format] <Mp4DemuxOpen> Start");

    /* Create demux fifo */
    {
        AMP_FIFO_CFG_s FifoDefCfg = { 0 };
        AmpFifo_GetDefaultCfg(&FifoDefCfg);
        FifoDefCfg.hCodec = VideoCodecHdlr;
        FifoDefCfg.IsVirtual = 1;
        FifoDefCfg.NumEntries = VIDEODEC_RAW_DESC_NUM;
        FifoDefCfg.cbEvent = AppLibFormatDemuxMp4_DemuxerVideoFifoCB;
        if (AmpFifo_Create(&FifoDefCfg, &DemuxMp4VidFifoHdlr) != AMP_OK) {
            AmbaPrintColor(RED, "[Applib - Format] <VIDEO Mp4DemuxOpen> %s:%u", __FUNCTION__, __LINE__);
            ReturnValue = -1; // Error
            goto ReturnError;
        }
        if (AudioCodecHdlr) {
            AmpFifo_GetDefaultCfg(&FifoDefCfg);
            FifoDefCfg.hCodec = AudioCodecHdlr;
            FifoDefCfg.IsVirtual = 1;
            FifoDefCfg.NumEntries = AUDIODEC_RAW_DESC_NUM;
            FifoDefCfg.cbEvent = AppLibFormatDemuxMp4_DemuxerAudioFifoCB;
            if (AmpFifo_Create(&FifoDefCfg, &DemuxMp4AudFifoHdlr) != AMP_OK) {
                AmbaPrintColor(RED, "[Applib - Format] <AUDIO Mp4DemuxOpen> %s:%u", __FUNCTION__, __LINE__);
                ReturnValue = -1; // Error
                goto ReturnError;
            }
        }
    }

    // Erase video FIFO in order to reset read/write pointer of raw buffer
    // No need to erase audio FIFO
    if (IsErase) {
        Rval = AmpFifo_EraseAll(DemuxMp4VidFifoHdlr);
        if (Rval != AMP_OK) {
            AmbaPrintColor(RED, "[Applib - Format] %s:%u Failed to erase fifo (%d).", __FUNCTION__, __LINE__, Rval);
            ReturnValue = -1; // Error
            goto ReturnError;
        }
    }

    /* Open Media info (Mp4dmx) */
    {
        APPLIB_MEDIA_INFO_s mediaInfo;
        if (AppLibFormat_GetMediaInfo(szName, &mediaInfo) != AMP_OK) {
            AmbaPrintColor(RED, "[Applib - Format] <Mp4DemuxOpen> %s:%u Get media info failed.",
                    __FUNCTION__, __LINE__);
            ReturnValue = -1; // Error
            goto ReturnError;
        }
        if (mediaInfo.MediaInfoType != AMP_MEDIA_INFO_MOVIE) {
            AmbaPrintColor(RED, "[Applib - Format] <Mp4DemuxOpen> %s:%u Media type must be movie.",
                    __FUNCTION__, __LINE__);
            ReturnValue = -1; // Error
            goto ReturnError;
        }
        if (mediaInfo.MediaInfo.Movie == NULL) {
            AmbaPrintColor(RED, "[Applib - Format] <Mp4DemuxOpen> %s:%u mediaInfo.MediaInfo.Movie == NULL.",
                    __FUNCTION__, __LINE__);
            ReturnValue = -1; // Error
            goto ReturnError;
        }
        DemuxMp4Movie = mediaInfo.MediaInfo.Movie;
        // Get media info details FIXIT, find video track by searching all track
        MovieHeight = DemuxMp4Movie->Track[0].Info.Video.Height;
        MovieWidth = DemuxMp4Movie->Track[0].Info.Video.Width;
        AmbaPrintColor(GREEN, "[Applib - Format] <Mp4DemuxOpen> DemuxMp4Movie->Track[0].Info.Video.Height = %d",
                DemuxMp4Movie->Track[0].Info.Video.Height);
        AmbaPrintColor(GREEN, "[Applib - Format] <Mp4DemuxOpen> DemuxMp4Movie->Track[0].Info.Video.Width = %d",
                DemuxMp4Movie->Track[0].Info.Video.Width);
        AmbaPrintColor(GREEN, "[Applib - Format] <Mp4DemuxOpen> DemuxMp4Movie->Track[0].TimePerFrame = %d",
                DemuxMp4Movie->Track[0].TimePerFrame);
        AmbaPrintColor(GREEN, "[Applib - Format] <Mp4DemuxOpen> DemuxMp4Movie->Track[0].FrameCount = %d",
                DemuxMp4Movie->Track[0].FrameCount);
#if 0
        if (AppLibVideoDec_SetPtsFrame(DemuxMp4Movie->Track[0].FrameCount,
                DemuxMp4Movie->Track[0].TimePerFrame, DemuxMp4Movie->Track[0].TimeScale) != 0) {
            AmbaPrintColor(RED, "[Applib - Format] <Mp4DemuxOpen> %s:%u Failed to set EOS PTS.", __FUNCTION__, __LINE__);
            ReturnValue = -1; // Error
            goto ReturnError;
        }
#endif
    }

    /* Open File system */
    if (DemuxMp4Stream == NULL) {
        AMP_FILE_STREAM_CFG_s FileCfg;
        AmpFileStream_GetDefaultCfg(&FileCfg);
        FileCfg.Async = FALSE;
        ReturnValue = AmpFileStream_Create(&FileCfg, &DemuxMp4Stream);
        if (ReturnValue != AMP_OK) {
            AmbaPrintColor(RED, "[Applib - Format] <Mp4DemuxOpen> %s:%u Failed to create file stream (%d).", __FUNCTION__, __LINE__, ReturnValue);
            ReturnValue = -1; // Error
            goto ReturnError;
        }
        ReturnValue = DemuxMp4Stream->Func->Open(DemuxMp4Stream, szName, AMP_STREAM_MODE_RDONLY);
        if (ReturnValue != AMP_OK) {
            AmbaPrintColor(RED, "[Applib - Format] <Mp4DemuxOpen> %s:%u Failed to open file (%d).", __FUNCTION__, __LINE__, ReturnValue);
            DemuxMp4StreamIsOpen = 0;
            ReturnValue = -1; // Error
            goto ReturnError;
        }
        DemuxMp4StreamIsOpen = 1;
    }

    /* Initial Demuxer */
    {
        AMP_DMX_MOVIE_INFO_CFG_s MovieCfg;
        AmpFormat_GetMovieInfo(szName, AmpMp4Dmx_Parse, DemuxMp4Stream, &DemuxMp4Movie);
        AmpDemuxer_GetDefaultMovieInfoCfg(&MovieCfg, DemuxMp4Movie);
        MovieCfg.Track[0].Fifo = DemuxMp4VidFifoHdlr;
        MovieCfg.Track[0].BufferBase = (UINT8 *) VidRawBuf;
        MovieCfg.Track[0].BufferLimit = (UINT8 *) VidRawBuf + SzVidRawBuf;
        if (DemuxMp4Movie->TrackCount > 1 && DemuxMp4Movie->Track[1].TrackType == AMP_MEDIA_TRACK_TYPE_AUDIO) {
            MovieCfg.Track[1].Fifo = DemuxMp4AudFifoHdlr;
            MovieCfg.Track[1].BufferBase = (UINT8 *) AudRawBuf;
            MovieCfg.Track[1].BufferLimit = (UINT8 *) AudRawBuf + SzAudRawBuf;
        }
        MovieCfg.InitTime = TimeOffset;
        if (AmpDemuxer_InitMovieInfo(DemuxMp4Movie, &MovieCfg) != AMP_OK) {
            AmbaPrintColor(RED, "[Applib - Format] <Mp4DemuxOpen> %s:%u", __FUNCTION__, __LINE__);
            ReturnValue = -1; // Error
            goto ReturnError;
        }
    }

    /* Create Mp4 demux */
    if (DemuxMp4Format == NULL) {
        AMP_MP4_DMX_CFG_s Mp4Cfg = { 0 };
        AmpMp4Dmx_GetDefaultCfg(&Mp4Cfg);
        Mp4Cfg.Stream = DemuxMp4Stream;
        if (AmpMp4Dmx_Create(&Mp4Cfg, &DemuxMp4Format) != AMP_OK) {
            AmbaPrintColor(RED, "[Applib - Format] <Mp4DemuxOpen> %s:%u", __FUNCTION__, __LINE__);
            ReturnValue = -1; // Error
            goto ReturnError;
        }
        DemuxMp4Format->Param.Movie.End = (IsFeedEos) ? (1) : (0);
    }

    /* Create Demuxer pipe */
    if (DemuxMp4Pipe == NULL) {
        AMP_DEMUXER_PIPE_CFG_s DmxCfg;
        AmpDemuxer_GetDefaultCfg(&DmxCfg);
        DmxCfg.FormatCount = 1;
        DmxCfg.Format[0] = DemuxMp4Format;
        DmxCfg.Media[0] = (AMP_MEDIA_INFO_s *) DemuxMp4Movie;
        DmxCfg.OnEvent = AppLibFormatDemuxMp4_DemuxerCallBack;
        if (Speed == 0) {
                /**slow play speed will equal 0, but demuxer can't accept 0, assign 1*/
                Speed =1;
        }
        DmxCfg.Speed = Speed;
        if (AmpDemuxer_Create(&DmxCfg, &DemuxMp4Pipe) != AMP_OK) {
            AmbaPrintColor(RED, "[Applib - Format] <Mp4DemuxOpen> %s:%u", __FUNCTION__, __LINE__);
            ReturnValue = -1; // Error
            goto ReturnError;
        }
        if (AmpDemuxer_Add(DemuxMp4Pipe) != AMP_OK) {
            AmbaPrintColor(RED, "[Applib - Format] <Mp4DemuxOpen> %s:%u", __FUNCTION__, __LINE__);
            ReturnValue = -1; // Error
            goto ReturnError;
        }
    }

    DBGMSG("[Applib - Format] <Mp4DemuxOpen> End");

    return ReturnValue; // Success

    ReturnError:
    // Release resource
    AppLibFormatDemuxMp4_Close(1); // Erase fifo
    return ReturnValue; // Error
}

int AppLibFormatDemuxMp4_Open(char* filename,
                              UINT32 startTime,
                              UINT32 speed,
                              UINT8 isErase,
                              UINT32 timeOffset,
                              UINT8 isFeedEos,
                              AMP_CALLBACK_f cbEventHdlr)
{
    int ReturnValue = 0;
    AMP_CFS_STAT CfsStat;
    {

        if (filename != NULL ) {
            AmbaPrint("[Applib - Format] <Mp4DemuxOpen> Filename =  %s", filename);
        } else {
            AmbaPrintColor(RED, "[Applib - Format] <Mp4DemuxOpen> Filename =  NULL");
        }
    }

    /* Open Mp4 demux */
    ReturnValue = AmpCFS_Stat(filename, &CfsStat);
    if (ReturnValue < 0) {
        AmbaPrintColor(RED, "[Applib - Format] <Mp4DemuxOpen> %s:%u Failed to get file stat (%d).", __FUNCTION__, __LINE__, ReturnValue);
        return ReturnValue;
    }
    if (AppLibFormatDemuxMp4_Init() != 0) {
        return -1;
    }
    ReturnValue = AppLibFormatDemuxMp4_Mp4DemuxOpen(filename, startTime, speed, isErase, timeOffset, isFeedEos);
    if (ReturnValue < 0) {
        AmbaPrintColor(RED, "[Applib - Format] <Mp4DemuxOpen> %s:%u", __FUNCTION__, __LINE__);
        return ReturnValue;
    }

    // Set callback
    DemuxCb = cbEventHdlr;

    return 0;
}

int AppLibFormatDemuxMp4_OpenStart(char* filename,
                                   UINT32 startTime,
                                   UINT8 direction,
                                   UINT32 speed,
                                   UINT8 isErase,
                                   UINT32 timeOffset,
                                   UINT8 isFeedEos,
                                   AMP_CALLBACK_f cbEventHdlr)
{
    // It's a hack function, wait FMT fix stop
    int ReturnValue;
    ReturnValue = AppLibFormatDemuxMp4_Open(filename, startTime, speed, isErase, timeOffset, isFeedEos, cbEventHdlr);
    if (ReturnValue < 0) {
        AmbaPrintColor(RED, "[Applib - Format] <Mp4DemuxOpenStart> %s:%u", __FUNCTION__, __LINE__);
        return ReturnValue;
    }
    ReturnValue = AppLibFormatDemuxMp4_Start(startTime, direction, speed);
    if (ReturnValue < 0) {
        AmbaPrintColor(RED, "[Applib - Format] <Mp4DemuxOpenStart> %s:%u", __FUNCTION__, __LINE__);
        return ReturnValue;
    }
    return 0;
}

int AppLibFormatDemuxMp4_Start(UINT32 startTime,
                               UINT8 direction,
                               UINT32 speed)
{
    int Rval = 0; // Functon call return
    AMP_MOVIE_INFO_s * Movie = NULL;

    if (DemuxMp4Pipe == NULL) {
        AmbaPrintColor(RED,"[Applib - Format] %s:%u DemuxMp4Pipe is NULL",__FUNCTION__,__LINE__);
        goto ReturnError;
    }

    Movie = (AMP_MOVIE_INFO_s *) DemuxMp4Pipe->Format[0]->Media;

    if (direction == 0) {
        AmbaPrint("[Applib - Format] <Mp4DemuxStart> Forward");
        Rval = AmpDemuxer_SetProcParam(DemuxMp4Pipe, 1200);
        if (Rval != AMP_OK) {
            AmbaPrintColor(RED, "[Applib - Format] %s:%u Failed to set parameter (%d).", __FUNCTION__, __LINE__, Rval);
            goto ReturnError;
        }

        // Seek and play
        if (speed != 1  ||Movie->Track[1].TrackType != AMP_MEDIA_TRACK_TYPE_AUDIO) {
            /**clear audio fifo info to stop audio when play spped is not normal*/
            Movie->Track[1].Fifo = NULL;
        } else {
            Movie->Track[1].Fifo = DemuxMp4AudFifoHdlr;
        }
        if (speed == 0) {
            /**slow play speed will equal 0, but demuxer can't accept 0, assign 1*/
            speed =1;
        }
        Rval = AmpDemuxer_Seek(DemuxMp4Pipe, startTime, direction, speed);
        if (Rval != AMP_OK) {
            AmbaPrintColor(RED, "[Applib - Format] %s:%u Failed to seek demuxer pipe (%d).", __FUNCTION__, __LINE__, Rval);
            goto ReturnError;
        }

        ApplibFormatDeMuxerMp4StartFlag = 0; // Start
        Rval = AmpDemuxer_OnDataRequest(DemuxMp4VidFifoHdlr);
        if (Rval != AMP_OK) {
            AmbaPrintColor(RED, "[Applib - Format] %s:%u Failed to run DataRequest (%d).", __FUNCTION__, __LINE__, Rval);
            goto ReturnError;
        }
        AmbaKAL_TaskSleep(100);
        Rval = AmpDemuxer_SetProcParam(DemuxMp4Pipe, 200);
        if (Rval != AMP_OK) {
            AmbaPrintColor(RED, "[Applib - Format] %s:%u Failed to set parameter (%d).", __FUNCTION__, __LINE__, Rval);
            goto ReturnError;
        }
    } else if (direction == 1) {
        AMP_MOVIE_INFO_s *Movie = NULL;
        AmbaPrint("[Applib - Format] <Mp4DemuxStart> Backward");
        if (speed == 0) {
                /**slow play speed will equal 0, but demuxer can't accept 0, assign 1*/
                speed =1;
        }
        Rval = AmpDemuxer_SetProcParam(DemuxMp4Pipe, 1200);
        if (Rval != AMP_OK) {
            AmbaPrintColor(RED, "[Applib - Format] %s:%u Failed to set parameter (%d).", __FUNCTION__, __LINE__, Rval);
            goto ReturnError;
        }
        // Seek and play
        /**clear audio fifo info to stop audio when play direction is not FW*/
        Movie = (AMP_MOVIE_INFO_s *) DemuxMp4Pipe->Format[0]->Media;
        Movie->Track[1].Fifo = NULL;

        Rval = AmpDemuxer_Seek(DemuxMp4Pipe, startTime, direction, speed);
        if (Rval != AMP_OK) {
            AmbaPrintColor(RED, "[Applib - Format] %s:%u Failed to seek demuxer pipe (%d).", __FUNCTION__, __LINE__, Rval);
            goto ReturnError;
        }
        ApplibFormatDeMuxerMp4StartFlag = 0; // Start
        Rval = AmpDemuxer_OnDataRequest(DemuxMp4VidFifoHdlr);
        if (Rval != AMP_OK) {
            AmbaPrintColor(RED, "[Applib - Format] %s:%u Failed to run DataRequest (%d).", __FUNCTION__, __LINE__, Rval);
            goto ReturnError;
        }
        AmbaKAL_TaskSleep(100);
        Rval = AmpDemuxer_SetProcParam(DemuxMp4Pipe, 200);
        if (Rval != AMP_OK) {
            AmbaPrintColor(RED, "[Applib - Format] %s:%u Failed to set parameter (%d).", __FUNCTION__, __LINE__, Rval);
            goto ReturnError;
        }
    } else {
        AmbaPrintColor(RED, "[Applib - Format] <Mp4DemuxStart> %s, Error direction %u", __FUNCTION__, direction);
        goto ReturnError;
    }

    return 0;

ReturnError:
    ApplibFormatDeMuxerMp4StartFlag = -1; // Stop
    return -1; // Error
}

int AppLibFormatDemuxMp4_Stop(void)
{
    int ReturnValue = 0;
    if (DemuxMp4Pipe != NULL) {
        AmpDemuxer_Stop(DemuxMp4Pipe); // Close the mp4 demuxer

        /**Wait for demuxer to stop to do other thing*/
        AmpDemuxer_WaitComplete(DemuxMp4Pipe,AMBA_KAL_WAIT_FOREVER);
    }

    if (DemuxMp4VidFifoHdlr != NULL) {
        ReturnValue = AmpFifo_EraseAll(DemuxMp4VidFifoHdlr);
        if (ReturnValue != AMP_OK) {
            AmbaPrint("[Applib - Format] %s:%u Failed to erase video fifo data (%d).", __FUNCTION__, __LINE__, ReturnValue);
            ReturnValue = -1;
        }
    }

    if (DemuxMp4AudFifoHdlr != NULL) {
        ReturnValue = AmpFifo_EraseAll(DemuxMp4AudFifoHdlr);
        if (ReturnValue != AMP_OK) {
            AmbaPrint("[Applib - Format] %s:%u Failed to erase audio fifo data (%d).", __FUNCTION__, __LINE__, ReturnValue);
            ReturnValue = -1;
        }
    }

    return 0;
}

/**
 *  Close the mp4 demuxer
 *
 *  @param [in] IsEraseFifo     Whether erase the data in video fifo and reset read/write pointer
 *
 *  @return >=0 success, <0 failure
 */
static int AppLibFormatDemuxMp4_Close_Internal(UINT8 IsEraseFifo)
{
    int ReturnValue = 0;
    int Rval = 0; // Functon call return

    ApplibFormatDeMuxerMp4StartFlag = -1; // Must stop at first


    if (DemuxMp4Pipe != NULL) {
        AMP_MOVIE_INFO_s *Movie = NULL;
        Movie = (AMP_MOVIE_INFO_s *) DemuxMp4Pipe->Format[0]->Media;

        Rval = AmpDemuxer_Remove(DemuxMp4Pipe);
        if (Rval != AMP_OK) {
            AmbaPrint("[Applib - Format] %s:%u Failed to remove demuxer (%d).", __FUNCTION__, __LINE__, Rval);
            ReturnValue = -1;
        }

        if (DemuxMp4Movie->TrackCount > 1 && DemuxMp4Movie->Track[1].TrackType == AMP_MEDIA_TRACK_TYPE_AUDIO &&
            Movie->Track[1].Fifo == NULL) {
            /**audio fifo would be clear at backward or speed up play, recover before delete format*/
            Movie->Track[1].Fifo = DemuxMp4AudFifoHdlr;
        }
        Rval = AmpDemuxer_Delete(DemuxMp4Pipe);
        if (Rval != AMP_OK) {
            AmbaPrint("[Applib - Format] %s:%u Failed to delete demuxer (%d).", __FUNCTION__, __LINE__, Rval);
            ReturnValue = -1;
        }
        DemuxMp4Pipe = NULL;
    }

    if (DemuxMp4Format != NULL) {
        Rval = AmpMp4Dmx_Delete(DemuxMp4Format);
        if (Rval != AMP_OK) {
            AmbaPrint("[Applib - Format] %s:%u Failed to delete mp4 demuxer (%d).", __FUNCTION__, __LINE__, Rval);
            ReturnValue = -1;
        }
        DemuxMp4Format = NULL;
    }

    if (DemuxMp4Stream != NULL) {
        if (DemuxMp4StreamIsOpen) {
            Rval = DemuxMp4Stream->Func->Close(DemuxMp4Stream);
            if (Rval != AMP_OK) {
                AmbaPrint("[Applib - Format] %s:%u Failed to close mp4 file stream (%d).", __FUNCTION__, __LINE__, Rval);
                ReturnValue = -1;
            }
            DemuxMp4StreamIsOpen = 0;
        }
        Rval = AmpFileStream_Delete(DemuxMp4Stream);
        if (Rval != AMP_OK) {
            AmbaPrint("[Applib - Format] %s:%u Failed to delete mp4 file stream (%d).", __FUNCTION__, __LINE__, Rval);
            ReturnValue = -1;
        }
        DemuxMp4Stream = NULL;
    }

    if (DemuxMp4Movie != NULL) {
        Rval = AmpFormat_RelMovieInfo(DemuxMp4Movie, FALSE);
        if (Rval != AMP_OK) {
            AmbaPrint("[Applib - Format] %s:%u Failed to remove movie info (%d).", __FUNCTION__, __LINE__, Rval);
            ReturnValue = -1;
        }
        DemuxMp4Movie = NULL;
    }

    if (DemuxMp4VidFifoHdlr != NULL) {
        if (IsEraseFifo) {
            Rval = AmpFifo_EraseAll(DemuxMp4VidFifoHdlr);
            if (Rval != AMP_OK) {
                AmbaPrint("[Applib - Format] %s:%u Failed to erase video fifo data (%d).", __FUNCTION__, __LINE__, Rval);
                ReturnValue = -1;
            }
        }
        Rval = AmpFifo_Delete(DemuxMp4VidFifoHdlr);
        if (Rval != AMP_OK) {
            AmbaPrint("[Applib - Format] %s:%u Failed to delete video fifo (%d).", __FUNCTION__, __LINE__, Rval);
            ReturnValue = -1;
        }
        DemuxMp4VidFifoHdlr = NULL;
    }

    if (DemuxMp4AudFifoHdlr != NULL) {
        if (IsEraseFifo) {
            Rval = AmpFifo_EraseAll(DemuxMp4AudFifoHdlr);
            if (Rval != AMP_OK) {
                AmbaPrint("[Applib - Format] %s:%u Failed to erase audio fifo data (%d).", __FUNCTION__, __LINE__, Rval);
                ReturnValue = -1;
            }
        }

        Rval = AmpFifo_Delete(DemuxMp4AudFifoHdlr);
        if (Rval != AMP_OK) {
            AmbaPrint("[Applib - Format] %s:%u Failed to delete audio fifo (%d).", __FUNCTION__, __LINE__, Rval);
            ReturnValue = -1;
        }
        DemuxMp4AudFifoHdlr = NULL;
    }

    return ReturnValue;
}

int AppLibFormatDemuxMp4_StillDec_Close(void)
{
    UINT8 IsEraseFifo = 0; // Do not erase fifo
    return AppLibFormatDemuxMp4_Close_Internal(IsEraseFifo);
}

int AppLibFormatDemuxMp4_Close(const UINT8 isEraseFifo)
{
    return AppLibFormatDemuxMp4_Close_Internal(isEraseFifo);
}

int AppLibFormatDemuxMp4_Feed(void* codecHdlr,
                              char* filename,
                              void* rawBuf,
                              UINT32 sizeRawBuf,
                              UINT32 *imageWidth,
                              UINT32 *imageHeight)
{
    int Rval = 0; // Functon call return
    UINT32 StartTime = 0; // Start from the beginning
    UINT32 Speed = 1; // Normal speed
    UINT8 IsErase = 1; // Erase fifo and reset RP and WP of raw buffer since raw buffer for still decode doesn't wrap RP and WP automatically
    UINT32 TimeOffset = 0; // No offset
    UINT8 IsFeedEos = 1; //  Feed EOS to DSP at the end of the file. The value doesn't matter here since we only need to feed one frame into DSP.
    AMP_CALLBACK_f CbEventHdlr = NULL; // No need to handle callback

    AppLibFormatDemuxMp4_SetCodecHdlrInfo(codecHdlr, NULL, rawBuf, sizeRawBuf, NULL, 0);
    if (AppLibFormatDemuxMp4_Open(filename, StartTime, Speed, IsErase, TimeOffset, IsFeedEos, CbEventHdlr) != 0) {
        return -1; // Error
    }

    Rval = AmpDemuxer_FeedFrame(DemuxMp4Format, 0, 0, AMP_FIFO_TYPE_IDR_FRAME);
    if (Rval != AMP_OK) {
        AmbaPrintColor(RED, "[Applib - Format] %s:%u Failed to feed frame (%d).", __FUNCTION__, __LINE__, Rval);
        goto ReturnError;
    }

    *imageWidth = DemuxMp4Movie->Track[0].Info.Video.Width;
    *imageHeight = DemuxMp4Movie->Track[0].Info.Video.Height;

    AppLibFormatDemuxMp4_Close_Internal(0); // Do not erase fifo
    return 0;

ReturnError:
    // Release resource
    AppLibFormatDemuxMp4_Close_Internal(1); // Erase fifo
    return -1; // Error
}

UINT8 AppLibFormatDemuxMp4_CanRequestData(void)
{
    if (ApplibFormatDeMuxerMp4InitFlag == -1) {
        return 0;
    }
    if (ApplibFormatDeMuxerMp4StartFlag == -1) {
        return 0;
    }
    if (DemuxMp4StreamIsOpen == 0) {
        return 0;
    }
    return 1;
}

int AppLibFormatDemuxMp4_DemuxOnDataRequest(void)
{
    //DBGMSG("[Applib - Format] <Mp4Demux> DemuxOnDataRequest");

    if (AppLibFormatDemuxMp4_CanRequestData()) {
        return AmpDemuxer_OnDataRequest(DemuxMp4VidFifoHdlr);
    } else {
        return -1; // Error
    }
}

int AppLibFormatDemuxMp4_DemuxOnDataRequestAudio(void)
{
    //DBGMSG("[Applib - Format] <Mp4Demux> DemuxOnDataRequestAudio");

    if (AppLibFormatDemuxMp4_CanRequestData() && (DemuxMp4AudFifoHdlr != NULL)) {
        return AmpDemuxer_OnDataRequest(DemuxMp4AudFifoHdlr);
    } else {
        return -1; // Error
    }
}


