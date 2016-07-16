/**
 * @file app/connected/applib/src/format/ApplibMuxer.c
 *
 * Muxer manager implementation (for demo APP only)
 *
 * History:
 *    2013/11/20 - [clchan  ] created file
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
#include <applib/format/ApplibMuxer.h>
#include <applib/format/ApplibPrecMux.h>
#include <applib.h>


static void Muxer_PerrorImpl(UINT32 level, UINT32 color, UINT32 line)
{
    char FileName[MAX_FILENAME_LENGTH] = {0};
    strncpy(FileName, (strrchr(__FILE__, '\\') + 1), (strlen(__FILE__) - strlen((strrchr(__FILE__, '\\') + 1)) + 1));
    AmbaPrint("[Error]%s:%u", FileName, line);
}

#define Muxer_Perror(level, color) {\
    Muxer_PerrorImpl(level, color, __LINE__);\
}

#define MMGR_STATUS_IDLE    0
#define MMGR_STATUS_INIT    1

/**
 * muxer state (in UINT8)
 */
typedef enum {
    APPLIB_MUXER_STATE_IDLE = 0x00,
    APPLIB_MUXER_STATE_PREC = 0x01,
    APPLIB_MUXER_STATE_START = 0x02,
    APPLIB_MUXER_STATE_RUNNING = 0x03,
    APPLIB_MUXER_STATE_PAUSING = 0x04,
    APPLIB_MUXER_STATE_PAUSED = 0x05,
    APPLIB_MUXER_STATE_RESUME = 0x06,
    APPLIB_MUXER_STATE_END = 0x10,
    APPLIB_MUXER_STATE_ERROR = 0xFF
} APPLIB_MUXER_STATE_e;

typedef struct {
    APPLIB_MUXER_PIPE_HDLR_s Hdlr;
    AMP_MUXER_PIPE_HDLR_s *Pipe;
    APPLIB_MUXER_GET_NAME_FP GetName;
    APPLIB_MUXER_ON_EVENT_FP OnEvent;
    AMP_MUX_FORMAT_HDLR_s *TmpFmt[AMP_MUXER_MAX_FORMAT_PER_PIPE];   /**< cache the formats for pre-record */
    BOOL8 Used;
    UINT8 SplitMode;
    UINT8 State;
} APPLIB_MUXER_PIPE_HDLR_IMPL_s;

/**
 * the core structure of MMGR
 */
typedef struct {
    AMBA_KAL_MUTEX_t Mutex;
    APPLIB_MUXER_PIPE_HDLR_IMPL_s Pipe[APPLIB_MUXER_MAX_PIPE];
    UINT8 Status;
} APPLIB_MUXER_MGR_s;

static APPLIB_MUXER_MGR_s g_Mmgr = {0};

int ApplibMuxer_GetInitDefaultCfg(APPLIB_MUXER_INIT_CFG_s *Config)
{
    K_ASSERT(Config != NULL);
    memset(Config, 0, sizeof(APPLIB_MUXER_INIT_CFG_s));
    Config->BufferSize = AmpMuxer_GetRequiredBufferSize(APPLIB_MUXER_MAX_PIPE, APPLIB_MUXER_MAX_TASK, APPLIB_MUXER_STACK_SIZE);
    return 0;
}

int ApplibMuxer_Init(APPLIB_MUXER_INIT_CFG_s *Config)
{
    K_ASSERT(Config != NULL);
    if (g_Mmgr.Status == MMGR_STATUS_IDLE) {
        memset(&g_Mmgr, 0, sizeof(APPLIB_MUXER_MGR_s));
        if (AmbaKAL_MutexCreate(&g_Mmgr.Mutex) == OK) {
            AMP_MUXER_INIT_CFG_s InitCfg;
            if (AmpMuxer_GetInitDefaultCfg(&InitCfg) == AMP_OK) {
                K_ASSERT(Config->Buffer != NULL);
                K_ASSERT(Config->BufferSize == AmpMuxer_GetRequiredBufferSize(APPLIB_MUXER_MAX_PIPE, APPLIB_MUXER_MAX_TASK, APPLIB_MUXER_STACK_SIZE));
                InitCfg.BufferSize = Config->BufferSize;
                InitCfg.Buffer = Config->Buffer;
                InitCfg.MaxPipe = APPLIB_MUXER_MAX_PIPE;
                InitCfg.MaxTask = APPLIB_MUXER_MAX_TASK;
                InitCfg.TaskInfo.Priority = APPLIB_MUXER_PRIORITY;
                InitCfg.TaskInfo.StackSize = APPLIB_MUXER_STACK_SIZE;
                if (AmpMuxer_Init(&InitCfg) == AMP_OK) {
                    g_Mmgr.Status = MMGR_STATUS_INIT;
                    return 0;
                }
            }
            AmbaKAL_MutexDelete(&g_Mmgr.Mutex);
        }
    }
    return -1;
}

int ApplibMuxer_GetDefaultCfg(APPLIB_MUXER_PIPE_CFG_s *Config)
{
    K_ASSERT(Config != NULL);
    memset(Config, 0, sizeof(APPLIB_MUXER_PIPE_CFG_s));
    Config->SplitCfg.MaxDuration = 30 * 60 * 1000;    // 30 min split
    Config->SplitCfg.MaxSize = 2ull * 1024 * 1024 * 1024;    // 2G split
    Config->SplitCfg.SplitMode = APPLIB_MUXER_SPLIT_CONNECTED;
    return 0;
}

static AMP_MEDIA_INFO_s *ApplibMuxer_NewMediaInfo(APPLIB_MUXER_MEDIA_CFG_s *Config)
{
    AMP_MOVIE_INFO_s *Movie;
    AMP_IMAGE_INFO_s *Image;
    AMP_SOUND_INFO_s *Sound;
    switch (Config->MediaType) {
    case AMP_MEDIA_INFO_MOVIE:
        if (AmpFormat_NewMovieInfo(Config->Name, &Movie) == AMP_OK) {
            // set movie info
            if (AmpMuxer_InitMovieInfo(Movie, &Config->Info.Movie) == AMP_OK)
                return (AMP_MEDIA_INFO_s *)Movie;
            AmpFormat_RelMovieInfo(Movie, TRUE);
        }
        break;
    case AMP_MEDIA_INFO_IMAGE:
        if (AmpFormat_NewImageInfo(Config->Name, &Image) == AMP_OK) {
            // set image info
            if (AmpMuxer_InitImageInfo(Image, &Config->Info.Image) == AMP_OK)
                return (AMP_MEDIA_INFO_s *)Image;
            AmpFormat_RelImageInfo(Image, TRUE);
        }
        break;
    case AMP_MEDIA_INFO_SOUND:
        if (AmpFormat_NewSoundInfo(Config->Name, &Sound) == AMP_OK) {
            // set sound info
            if (AmpMuxer_InitSoundInfo(Sound, &Config->Info.Sound) == AMP_OK)
                return (AMP_MEDIA_INFO_s *)Sound;
            AmpFormat_RelSoundInfo(Sound, TRUE);
        }
        break;
    default:
        Muxer_Perror(0, 0);
        break;
    }
    return NULL;
}

static int ApplibMuxer_RelMediaInfo(AMP_MEDIA_INFO_s *Media)
{
    switch (Media->MediaType) {
    case AMP_MEDIA_INFO_MOVIE:
        if (AmpFormat_RelMovieInfo((AMP_MOVIE_INFO_s *)Media, TRUE) == AMP_OK)
            return 0;
        break;
    case AMP_MEDIA_INFO_IMAGE:
        if (AmpFormat_RelImageInfo((AMP_IMAGE_INFO_s *)Media, TRUE) == AMP_OK)
            return 0;
        break;
    case AMP_MEDIA_INFO_SOUND:
        if (AmpFormat_RelSoundInfo((AMP_SOUND_INFO_s *)Media, TRUE) == AMP_OK)
            return 0;
        break;
    default:
        Muxer_Perror(0, 0);
        break;
    }
    return -1;
}

static AMP_MEDIA_INFO_s *ApplibMuxer_CopyMediaInfo(AMP_MEDIA_INFO_s *Ref, char *Name)
{
    AMP_MOVIE_INFO_s *Movie = NULL;
    AMP_IMAGE_INFO_s *Image = NULL;
    AMP_SOUND_INFO_s *Sound = NULL;
    switch (Ref->MediaType) {
    case AMP_MEDIA_INFO_MOVIE:
        if (AmpFormat_NewMovieInfo(Name, &Movie) == AMP_OK) {
            if (AmpFormat_CopyMovieInfo(Movie, (AMP_MOVIE_INFO_s *)Ref) == AMP_OK)
                return (AMP_MEDIA_INFO_s *)Movie;
            AmpFormat_RelMovieInfo(Movie, TRUE);
        }
        break;
    case AMP_MEDIA_INFO_IMAGE:
        if (AmpFormat_NewImageInfo(Name, &Image) == AMP_OK) {
            if (AmpFormat_CopyImageInfo(Image, (AMP_IMAGE_INFO_s *)Ref) == AMP_OK)
                return (AMP_MEDIA_INFO_s *)Image;
            AmpFormat_RelImageInfo(Image, TRUE);
        }
        break;
    case AMP_MEDIA_INFO_SOUND:
        if (AmpFormat_NewSoundInfo(Name, &Sound) == AMP_OK) {
            if (AmpFormat_CopySoundInfo(Sound, (AMP_SOUND_INFO_s *)Ref) == AMP_OK)
                return (AMP_MEDIA_INFO_s *)Sound;
            AmpFormat_RelSoundInfo(Sound, TRUE);
        }
        break;
    default:
        Muxer_Perror(0, 0);
        break;
    }
    return NULL;
}

static int ApplibMuxer_CloseFormat(APPLIB_MUXER_PIPE_HDLR_IMPL_s *Pipe, UINT8 Mode)
{
    int rval = 0;
    UINT32 i;
    AMP_MUX_FORMAT_HDLR_s *Format;
    AMP_MUXER_PIPE_HDLR_s * const MuxPipe = Pipe->Pipe;
    K_ASSERT(MuxPipe->FormatCount > 0);
    K_ASSERT(Pipe->State != APPLIB_MUXER_STATE_PREC);
    for (i=0; i<MuxPipe->FormatCount; i++) {
        Format = MuxPipe->Format[i];
        K_ASSERT(Format != NULL);
        if (Format->Func->Close(Format, Mode) != AMP_OK) {
            Muxer_Perror(0, 0);
            rval = -1;
        }
    }
    return rval;
}

static int ApplibMuxer_RemovePipe(APPLIB_MUXER_PIPE_HDLR_IMPL_s *Pipe)
{
    int rval = 0;
    UINT32 i;
    AMP_MUX_FORMAT_HDLR_s *Format;
    AMP_MUXER_PIPE_HDLR_s * const MuxPipe = Pipe->Pipe;
    AMP_STREAM_HDLR_s *pStream;
    const UINT32 FormatCount = MuxPipe->FormatCount;
    K_ASSERT(MuxPipe->FormatCount > 0);
    K_ASSERT(Pipe->State > APPLIB_MUXER_STATE_IDLE);
    K_ASSERT(Pipe->State < APPLIB_MUXER_STATE_END);
    if (AmpMuxer_Remove(MuxPipe) != AMP_OK)
        rval = -1;
    if (Pipe->State == APPLIB_MUXER_STATE_PREC) {
        // delete precmux, restore to the original one (no need to close)
        for (i=0; i<FormatCount; i++) {
            Format = MuxPipe->Format[i];
            K_ASSERT(Format != NULL);
            if (ApplibPrecMux_Delete(Format) != AMP_OK)
                rval = -1;
            MuxPipe->Format[i] = Pipe->TmpFmt[i];
        }
    }
    for (i=0; i<FormatCount; i++) {
        Format = MuxPipe->Format[i];
        K_ASSERT(Format != NULL);
        if (Pipe->State >= APPLIB_MUXER_STATE_START) {
            pStream = Format->Stream;
            K_ASSERT(pStream != NULL);
            if (pStream->Func->Close(pStream) != AMP_OK) {
                Muxer_Perror(0, 0);
                rval = -1;
            }
        }
        // release all media info
        K_ASSERT(Format->Media != NULL);
        if (ApplibMuxer_RelMediaInfo(Format->Media) < 0)
            rval = -1;
    }
    return rval;
}

static int ApplibMuxer_OnStart(APPLIB_MUXER_PIPE_HDLR_IMPL_s *Pipe)
{
    if (Pipe->State == APPLIB_MUXER_STATE_PREC)
        return 0;
    if (Pipe->State == APPLIB_MUXER_STATE_START) {
        Pipe->State = APPLIB_MUXER_STATE_RUNNING;
        if (Pipe->OnEvent(&Pipe->Hdlr, APPLIB_MUXER_EVENT_START) == 0)
            return 0;
        Muxer_Perror(0, 0);
    } else {
        Muxer_Perror(0, 0);
    }
    return -1;
}

static int ApplibMuxer_ResetMovieConnected(AMP_MOVIE_INFO_s *Movie)
{
    UINT8 TrackId;
    if (ApplibFormatLib_AdjustDTS(Movie) != AMP_OK) {
        Muxer_Perror(0, 0);
        return -1;
    }
    for (TrackId = 0; TrackId < Movie->TrackCount; TrackId++) {
        AMP_MEDIA_TRACK_INFO_s *Track = &Movie->Track[TrackId];
        Track->InitDTS = Track->NextDTS = Track->DTS;
    }
    return 0;
}

static int ApplibMuxer_ResetMovieNewSession(AMP_MOVIE_INFO_s *Movie)
{
    UINT8 TrackId;
    UINT64 MinDTS;
    AMP_MEDIA_TRACK_INFO_s *MinTrack;
    if (ApplibFormatLib_AdjustDTS(Movie) != AMP_OK) {
        Muxer_Perror(0, 0);
        return -1;
    }
    MinTrack = ApplibFormatLib_GetShortestTrack((AMP_MEDIA_INFO_s *)Movie);
    K_ASSERT(MinTrack != NULL);
    MinDTS = MinTrack->DTS;
    for (TrackId = 0; TrackId < Movie->TrackCount; TrackId++) {
        AMP_MEDIA_TRACK_INFO_s *pTrack = &Movie->Track[TrackId];
        pTrack->InitDTS = pTrack->NextDTS = pTrack->DTS = (pTrack->DTS - MinDTS);
        if (pTrack->TrackType == AMP_MEDIA_TRACK_TYPE_VIDEO) {
            pTrack->Info.Video.RefDTS = pTrack->InitDTS;
            ApplibFormatLib_ResetPTS(&Movie->Track[TrackId]);
        }
    }
    return 0;
}

static int ApplibMuxer_ResetSoundConnected(AMP_SOUND_INFO_s *Sound)
{
    UINT32 i;
    for (i=0; i<Sound->TrackCount; i++) {
        Sound->Track[i].InitDTS = Sound->Track[i].NextDTS = Sound->Track[i].DTS;
    }
    return 0;
}

static int ApplibMuxer_ResetSoundNewSession(AMP_SOUND_INFO_s *Sound)
{
    UINT32 i;
    UINT64 DTS;
    AMP_MEDIA_TRACK_INFO_s * const DefTrack = ApplibFormatLib_GetDefaultTrack((AMP_MEDIA_INFO_s *)Sound, AMP_MEDIA_TRACK_TYPE_AUDIO);
    if (DefTrack == NULL) {
        Muxer_Perror(0, 0);
        return -1;
    }
    DTS = DefTrack->DTS;
    for (i=0; i<Sound->TrackCount; i++) {
        Sound->Track[i].DTS -= DTS;
        Sound->Track[i].InitDTS = Sound->Track[i].NextDTS = Sound->Track[i].DTS;
    }
    return 0;
}

typedef int (*APPLIB_MUXER_RESET_MEDIA_FP)(AMP_MEDIA_INFO_s *);

static int ApplibMuxer_ResetMediaConnected(AMP_MEDIA_INFO_s *Media)
{
    switch (Media->MediaType) {
    case AMP_MEDIA_INFO_MOVIE:
        return ApplibMuxer_ResetMovieConnected((AMP_MOVIE_INFO_s *)Media);
    case AMP_MEDIA_INFO_IMAGE:
        return 0;
    case AMP_MEDIA_INFO_SOUND:
        return ApplibMuxer_ResetSoundConnected((AMP_SOUND_INFO_s *)Media);
    default:
        Muxer_Perror(0, 0);
        break;
    }
    return -1;
}

static int ApplibMuxer_ResetMediaNewSession(AMP_MEDIA_INFO_s *Media)
{
    switch (Media->MediaType) {
    case AMP_MEDIA_INFO_MOVIE:
        return ApplibMuxer_ResetMovieNewSession((AMP_MOVIE_INFO_s *)Media);
    case AMP_MEDIA_INFO_IMAGE:
        return 0;
    case AMP_MEDIA_INFO_SOUND:
        return ApplibMuxer_ResetSoundNewSession((AMP_SOUND_INFO_s *)Media);
    default:
        Muxer_Perror(0, 0);
        break;
    }
    return -1;
}

static int ApplibMuxer_SplitImpl(APPLIB_MUXER_PIPE_HDLR_IMPL_s *Pipe, AMP_MUX_FORMAT_HDLR_s *Format, APPLIB_MUXER_RESET_MEDIA_FP ResetMedia)
{
    int rval = 0;
    char Name[MAX_FILENAME_LENGTH];
    AMP_STREAM_HDLR_s * const Stream = Format->Stream;
    if (Format->Func->Close(Format, (ResetMedia == ApplibMuxer_ResetMediaNewSession) ? AMP_MUX_FORMAT_CLOSE_DEFAULT : AMP_MUX_FORMAT_CLOSE_NOT_END) != AMP_OK) {
        Muxer_Perror(0, 0);
        rval = -1;
    }
    if (Stream->Func->Close(Stream) != AMP_OK) {
        Muxer_Perror(0, 0);
        rval = -1;
    }
    if (Pipe->GetName((APPLIB_MUXER_PIPE_HDLR_s *)Pipe, Format, Name, MAX_FILENAME_LENGTH) < 0) {
        Muxer_Perror(0, 0);
        rval = -1;
    }
    if (rval == 0) {
        AMP_MEDIA_INFO_s * const Media = ApplibMuxer_CopyMediaInfo(Format->Media, Name);
        if (Media != NULL) {
            if (ApplibMuxer_RelMediaInfo(Format->Media) == 0) {
                if (ResetMedia(Media) == 0) {
                    if (Stream->Func->Open(Stream, Name, AMP_STREAM_MODE_WRONLY) == AMP_OK) {
                        Format->Media = Media;
                        if (ApplibFormatLib_ResetMuxMediaInfo(Format->Media) == AMP_OK) {
                            if (Format->Func->Open(Format) == AMP_OK)
                                return 0;
                        }
                        Stream->Func->Close(Stream);
                    } else {
                        Muxer_Perror(0, 0);
                    }
                }
            }
            ApplibMuxer_RelMediaInfo(Media);
        } else {
            ApplibMuxer_RelMediaInfo(Format->Media);
        }
    } else {
        ApplibMuxer_RelMediaInfo(Format->Media);
    }
    return -1;
}

static int ApplibMuxer_SplitConnected(APPLIB_MUXER_PIPE_HDLR_IMPL_s *Pipe)
{
    UINT32 i, j;
    const UINT32 FormatCount = Pipe->Pipe->FormatCount;
    AMP_MUX_FORMAT_HDLR_s *Format;
    AMP_STREAM_HDLR_s *Stream;
    for (i=0; i<FormatCount; i++) {
        Format = Pipe->Pipe->Format[i];
        if (ApplibMuxer_SplitImpl(Pipe, Format, ApplibMuxer_ResetMediaConnected) < 0)
            break;
    }
    if (i == FormatCount)
        return 0;
    // close other formats and their media info
    for (j=0; j<FormatCount; j++) {
        if (j != i) {
            Format = Pipe->Pipe->Format[j];
            if (Format->Func->Close(Format, AMP_MUX_FORMAT_CLOSE_RECOVER) != AMP_OK)
                Muxer_Perror(0, 0);
            ApplibMuxer_RelMediaInfo(Format->Media);
            Stream = Format->Stream;
            if (Stream->Func->Close(Stream) != AMP_OK)
                Muxer_Perror(0, 0);
        }
    }
    return -1;
}

static int ApplibMuxer_SplitNewSession(APPLIB_MUXER_PIPE_HDLR_IMPL_s *Pipe)
{
    UINT32 i, j;
    const UINT32 FormatCount = Pipe->Pipe->FormatCount;
    AMP_MUX_FORMAT_HDLR_s *Format;
    AMP_STREAM_HDLR_s *Stream;
    for (i=0; i<FormatCount; i++) {
        Format = Pipe->Pipe->Format[i];
        // TODO: must notify formats to discard the last frames
        if (ApplibMuxer_SplitImpl(Pipe, Format, ApplibMuxer_ResetMediaNewSession) < 0)
            break;
    }
    if (i == FormatCount)
        return 0;
    // close other formats and their media info
    for (j=0; j<FormatCount; j++) {
        if (j != i) {
            Format = Pipe->Pipe->Format[j];
            if (Format->Func->Close(Format, AMP_MUX_FORMAT_CLOSE_RECOVER) != AMP_OK)
                Muxer_Perror(0, 0);
            ApplibMuxer_RelMediaInfo(Format->Media);
            Stream = Format->Stream;
            if (Stream->Func->Close(Stream) != AMP_OK)
                Muxer_Perror(0, 0);
        }
    }
    return -1;
}

static int ApplibMuxer_SplitOff(APPLIB_MUXER_PIPE_HDLR_IMPL_s *Pipe)
{
    int rval = 0;
    UINT32 i;
    const UINT32 FormatCount = Pipe->Pipe->FormatCount;
    AMP_MUX_FORMAT_HDLR_s *Format;
    for (i=0; i<FormatCount; i++) {
        Format = Pipe->Pipe->Format[i];
        switch (Format->Media->MediaType) {
        case AMP_MEDIA_INFO_MOVIE:
            if (Format->Param.Movie.MaxDuration == 0xFFFFFFFF) {
                Muxer_Perror(0, 0);
                rval = -1;
            }
            if (Format->Param.Movie.MaxSize == 0xFFFFFFFFFFFFFFFFull) {
                Muxer_Perror(0, 0);
                rval = -1;
            }
            Format->Param.Movie.MaxDuration = 0xFFFFFFFF;
            Format->Param.Movie.MaxSize = 0xFFFFFFFFFFFFFFFFull;
            break;
        case AMP_MEDIA_INFO_IMAGE:
            break;
        case AMP_MEDIA_INFO_SOUND:
            if (Format->Param.Sound.MaxDuration == 0xFFFFFFFF) {
                Muxer_Perror(0, 0);
                rval = -1;
            }
            if (Format->Param.Sound.MaxSize == 0xFFFFFFFFFFFFFFFFull) {
                Muxer_Perror(0, 0);
                rval = -1;
            }
            Format->Param.Sound.MaxDuration = 0xFFFFFFFF;
            Format->Param.Sound.MaxSize = 0xFFFFFFFFFFFFFFFFull;
            break;
        default:
            Muxer_Perror(0, 0);
            rval = -1;
            break;
        }
    }
    if (rval < 0) {
        // close format and remove pipe on error
        ApplibMuxer_CloseFormat(Pipe, AMP_MUX_FORMAT_CLOSE_RECOVER);
        ApplibMuxer_RemovePipe(Pipe);
    }
    return rval;
}

static int ApplibMuxer_OnAutoSplit(APPLIB_MUXER_PIPE_HDLR_IMPL_s *Pipe)
{
    int rval = -1;
    K_ASSERT((Pipe->State == APPLIB_MUXER_STATE_RUNNING) || (Pipe->State == APPLIB_MUXER_STATE_PAUSING));
    K_ASSERT(Pipe->OnEvent != NULL);
    switch (Pipe->SplitMode) {
    case APPLIB_MUXER_SPLIT_CONNECTED:
        if (ApplibMuxer_SplitConnected(Pipe) == 0)
            rval = 0;
        break;
    case APPLIB_MUXER_SPLIT_NEW_SESSION:
        if (ApplibMuxer_SplitNewSession(Pipe) == 0)
            rval = 0;
        break;
    default:
        if (ApplibMuxer_SplitOff(Pipe) == 0)
            rval = 0;
        break;
    }
    if (rval == 0) {
        if (Pipe->OnEvent(&Pipe->Hdlr, APPLIB_MUXER_EVENT_AUTO_SPLIT) == 0)
            return 0;
        Muxer_Perror(0, 0);
        // close format and remove pipe on error
        ApplibMuxer_CloseFormat(Pipe, AMP_MUX_FORMAT_CLOSE_RECOVER);
        ApplibMuxer_RemovePipe(Pipe);
    }
    // set status to error to avoid close again
    Pipe->State = APPLIB_MUXER_STATE_ERROR;
    return -1;
}

#if 0
static int ApplibMuxer_OnStopOnPaused(APPLIB_MUXER_PIPE_HDLR_IMPL_s *Pipe)
{
    if (Pipe->State == APPLIB_MUXER_STATE_PAUSED) {
        if (ApplibMuxer_CloseFormat(Pipe, AMP_MUX_FORMAT_CLOSE_DEFAULT) == 0)
            return 0;
    } else {
        Muxer_Perror(0, 0);
    }
    return -1;
}
#endif

static int ApplibMuxer_OnEnd(APPLIB_MUXER_PIPE_HDLR_IMPL_s *Pipe)
{
    K_ASSERT(Pipe->OnEvent != NULL);
    K_ASSERT((Pipe->State == APPLIB_MUXER_STATE_PREC) || (Pipe->State == APPLIB_MUXER_STATE_RUNNING) || (Pipe->State == APPLIB_MUXER_STATE_PAUSED));
    if (ApplibMuxer_RemovePipe(Pipe) == 0) {
        Pipe->State = APPLIB_MUXER_STATE_END;
        if (Pipe->OnEvent(&Pipe->Hdlr, APPLIB_MUXER_EVENT_END) == 0)
            return 0;
        Muxer_Perror(0, 0);
    }
    Pipe->State = APPLIB_MUXER_STATE_ERROR;   // to avoid close again
    if (Pipe->OnEvent(&Pipe->Hdlr, APPLIB_MUXER_EVENT_GENERAL_ERROR) < 0)
        Muxer_Perror(0, 0);
    return -1;
}

static int ApplibMuxer_OnIOError(APPLIB_MUXER_PIPE_HDLR_IMPL_s *Pipe)
{
    int rval = 0;
    K_ASSERT(Pipe->OnEvent != NULL);
    K_ASSERT(Pipe->State > APPLIB_MUXER_STATE_IDLE);
    K_ASSERT(Pipe->State != APPLIB_MUXER_STATE_END);
    if (Pipe->State < APPLIB_MUXER_STATE_ERROR) {
        if (ApplibMuxer_RemovePipe(Pipe) < 0)
            rval = -1;
        Pipe->State = APPLIB_MUXER_STATE_ERROR;
    }
    if (Pipe->OnEvent(&Pipe->Hdlr, APPLIB_MUXER_EVENT_IO_ERROR) < 0) {
        Muxer_Perror(0, 0);
        rval = -1;
    }
    return rval;
}

static int ApplibMuxer_OnFIFOError(APPLIB_MUXER_PIPE_HDLR_IMPL_s *Pipe)
{
    int rval = 0;
    K_ASSERT(Pipe->OnEvent != NULL);
    K_ASSERT(Pipe->State > APPLIB_MUXER_STATE_IDLE);
    K_ASSERT(Pipe->State != APPLIB_MUXER_STATE_END);
    if (Pipe->State < APPLIB_MUXER_STATE_ERROR) {
        if (ApplibMuxer_RemovePipe(Pipe) < 0)
            rval = -1;
        Pipe->State = APPLIB_MUXER_STATE_ERROR;
    }
    if (Pipe->OnEvent(&Pipe->Hdlr, APPLIB_MUXER_EVENT_FIFO_ERROR) < 0) {
        Muxer_Perror(0, 0);
        rval = -1;
    }
    return rval;
}

static int ApplibMuxer_OnGeneralError(APPLIB_MUXER_PIPE_HDLR_IMPL_s *Pipe)
{
    int rval = 0;
    K_ASSERT(Pipe->OnEvent != NULL);
    K_ASSERT(Pipe->State > APPLIB_MUXER_STATE_IDLE);
    K_ASSERT(Pipe->State != APPLIB_MUXER_STATE_END);
    if (Pipe->State < APPLIB_MUXER_STATE_ERROR) {
        if (ApplibMuxer_RemovePipe(Pipe) < 0)
            rval = -1;
        Pipe->State = APPLIB_MUXER_STATE_ERROR;
    }
    if (Pipe->OnEvent(&Pipe->Hdlr, APPLIB_MUXER_EVENT_GENERAL_ERROR) < 0) {
        Muxer_Perror(0, 0);
        rval = -1;
    }
    return rval;
}

static int ApplibMuxer_OnEventImpl(void *pHdlr, UINT32 nEvent, void *pInfo)
{
    UINT32 i;
    APPLIB_MUXER_PIPE_HDLR_IMPL_s *Pipe;
    for (i=0; i<APPLIB_MUXER_MAX_PIPE; i++) {
        Pipe = &g_Mmgr.Pipe[i];
        if (Pipe->Used && (Pipe->Pipe == pHdlr)) {
            K_ASSERT(Pipe->OnEvent != NULL);
            switch (nEvent) {
            case AMP_MUXER_EVENT_START:
                return ApplibMuxer_OnStart(Pipe);
            case AMP_MUXER_EVENT_END:
                return ApplibMuxer_OnEnd(Pipe);
#if 0
            case AMP_MUXER_EVENT_PAUSE:
                return ApplibMuxer_OnPause(Pipe);
            case AMP_MUXER_EVENT_RESUME:
                return ApplibMuxer_OnResume(Pipe);
#endif
            case AMP_MUXER_EVENT_REACH_LIMIT:
                return ApplibMuxer_OnAutoSplit(Pipe);
#if 0
            case AMP_MUXER_EVENT_STOP_ON_PAUSED:
                return ApplibMuxer_OnStopOnPaused(Pipe);
#endif
            case AMP_MUXER_EVENT_IO_ERROR:
                return ApplibMuxer_OnIOError(Pipe);
            case AMP_MUXER_EVENT_FIFO_ERROR:
                return ApplibMuxer_OnFIFOError(Pipe);
            case AMP_MUXER_EVENT_GENERAL_ERROR:
                return ApplibMuxer_OnGeneralError(Pipe);
            default:
                Muxer_Perror(0, 0);
                break;
            }
            return -1;
        }
    }
    Muxer_Perror(0, 0);
    return -1;
}

static int ApplibMuxer_OnEvent(void *pHdlr, UINT32 nEvent, void *pInfo)
{
    int rval = -1;
    K_ASSERT(pHdlr != NULL);
    if (AmbaKAL_MutexTake(&g_Mmgr.Mutex, AMBA_KAL_WAIT_FOREVER) == OK) {
        if (ApplibMuxer_OnEventImpl(pHdlr, nEvent, pInfo) == 0)
            rval = 0;
        AmbaKAL_MutexGive(&g_Mmgr.Mutex);
    } else {
        Muxer_Perror(0, 0);
    }
    return rval;
}

static APPLIB_MUXER_PIPE_HDLR_IMPL_s *ApplibMuxer_CreateImpl(APPLIB_MUXER_PIPE_CFG_s *Config)
{
    UINT32 i;
    APPLIB_MUXER_PIPE_HDLR_IMPL_s *Pipe;
    K_ASSERT(Config->FormatCount > 0);
    K_ASSERT(Config->GetName != NULL);
    K_ASSERT(Config->OnEvent != NULL);
    for (i=0; i<APPLIB_MUXER_MAX_PIPE; i++) {
        Pipe = &g_Mmgr.Pipe[i];
        if (Pipe->Used == FALSE) {
            UINT j;
            const UINT32 FormatCount = Config->FormatCount;
            AMP_MEDIA_INFO_s *Media[AMP_MUXER_MAX_FORMAT_PER_PIPE] = {0};
            // create media info
            for (j=0; j<FormatCount; j++) {
                Media[j] = ApplibMuxer_NewMediaInfo(&Config->MediaCfg[j]);
                if (Media[j] == NULL)
                    break;
            }
            if (j == FormatCount) {
                AMP_MUXER_PIPE_CFG_s PipeCfg;
                if (AmpMuxer_GetDefaultCfg(&PipeCfg) == AMP_OK) {
                    UINT32 k;
                    for (k=0; k<FormatCount; k++) {
                        PipeCfg.Format[k] = Config->Format[k];
                        PipeCfg.Media[k] = Media[k];
                    }
                    if (k == FormatCount) {
                        PipeCfg.NewTask = Config->TaskCfg.NewTask;
                        PipeCfg.TaskPriority = Config->TaskCfg.TaskPriority;
                        PipeCfg.OnEvent = ApplibMuxer_OnEvent;
                        PipeCfg.MaxDuration = Config->SplitCfg.MaxDuration;
                        PipeCfg.MaxSize = Config->SplitCfg.MaxSize;
                        PipeCfg.FormatCount = Config->FormatCount;
                        if (AmpMuxer_Create(&PipeCfg, &Pipe->Pipe) == AMP_OK) {
                            Pipe->OnEvent = Config->OnEvent;
                            Pipe->GetName = Config->GetName;
                            Pipe->SplitMode = Config->SplitCfg.SplitMode;
                            Pipe->Used = TRUE;
                            return Pipe;
                        }
                    }
                }
            } else {
                Muxer_Perror(0, 0);
            }
            // release media info on error
            for (j=0; j<FormatCount; j++) {
                if (Media[j] != NULL)
                    ApplibMuxer_RelMediaInfo(Media[j]);
            }
            break;
        }
    }
    Muxer_Perror(0, 0);
    return NULL;
}

int ApplibMuxer_Create(APPLIB_MUXER_PIPE_CFG_s *Config, APPLIB_MUXER_PIPE_HDLR_s **Pipe)
{
    int rval = -1;
    K_ASSERT(Config != NULL);
    K_ASSERT(Pipe != NULL);
    if (AmbaKAL_MutexTake(&g_Mmgr.Mutex, AMBA_KAL_WAIT_FOREVER) == OK) {
        APPLIB_MUXER_PIPE_HDLR_IMPL_s * const pipe = ApplibMuxer_CreateImpl(Config);
        if (pipe != NULL) {
            *Pipe = (APPLIB_MUXER_PIPE_HDLR_s *)pipe;
            rval = 0;
        }
        AmbaKAL_MutexGive(&g_Mmgr.Mutex);
    } else {
        Muxer_Perror(0, 0);
    }
    return rval;
}

static int ApplibMuxer_DeleteImpl(APPLIB_MUXER_PIPE_HDLR_IMPL_s *Pipe)
{
    int rval = 0;
    if ((Pipe->State == APPLIB_MUXER_STATE_IDLE) || (Pipe->State >= APPLIB_MUXER_STATE_END)) {
        AMP_MUXER_PIPE_HDLR_s * const MuxPipe = Pipe->Pipe;
        K_ASSERT(Pipe->Used == TRUE);
        K_ASSERT(Pipe->Pipe != NULL);
        if (Pipe->State == APPLIB_MUXER_STATE_IDLE) {
            UINT32 i;
            const UINT32 FormatCount = MuxPipe->FormatCount;
            AMP_MUX_FORMAT_HDLR_s *Format;
            K_ASSERT(FormatCount > 0);
            // release all media info
            for (i=0; i<FormatCount; i++) {
                Format = MuxPipe->Format[i];
                K_ASSERT(Format != NULL);
                K_ASSERT(Format->Media != NULL);
                if (ApplibMuxer_RelMediaInfo(Format->Media) < 0)
                    rval = -1;
            }
        }
        if (AmpMuxer_Delete(MuxPipe) != AMP_OK)
            rval = -1;
        memset(Pipe, 0, sizeof(APPLIB_MUXER_PIPE_HDLR_IMPL_s));
    } else {
        Muxer_Perror(0, 0);
        rval = -1;
    }
    return rval;
}

int ApplibMuxer_Delete(APPLIB_MUXER_PIPE_HDLR_s *Pipe)
{
    int rval = -1;
    K_ASSERT(Pipe != NULL);
    if (AmbaKAL_MutexTake(&g_Mmgr.Mutex, AMBA_KAL_WAIT_FOREVER) == OK) {
        if (ApplibMuxer_DeleteImpl((APPLIB_MUXER_PIPE_HDLR_IMPL_s *)Pipe) == 0)
            rval = 0;
        AmbaKAL_MutexGive(&g_Mmgr.Mutex);
    } else {
        Muxer_Perror(0, 0);
    }
    return rval;
}

static int ApplibMuxer_StartOnPrecImpl(APPLIB_MUXER_PIPE_HDLR_IMPL_s *Pipe)
{
    int rval = 0;
    AMP_MUXER_PIPE_HDLR_s * const MuxPipe = Pipe->Pipe;
    const UINT32 FormatCount = MuxPipe->FormatCount;
    UINT32 i, j;
    AMP_MUX_FORMAT_HDLR_s *Format;
    AMP_STREAM_HDLR_s *Stream;
    AMP_MEDIA_INFO_s *Media;
    // close precmux, restore to the original one
    for (i=0; i<FormatCount; i++) {
        Format = MuxPipe->Format[i];
        K_ASSERT(Format != NULL);
        if (Format->Func->Close(Format, AMP_MUX_FORMAT_CLOSE_NOT_END) != AMP_OK) {
            Muxer_Perror(0, 0);
            rval = -1;
        }
        if (ApplibPrecMux_Delete(Format) != AMP_OK)
            rval = -1;
        MuxPipe->Format[i] = Pipe->TmpFmt[i];
    }
    if (rval == 0) {
        for (i=0; i<FormatCount; i++) {
            Format = MuxPipe->Format[i];
            K_ASSERT(Format != NULL);
            Stream = Format->Stream;
            K_ASSERT(Stream != NULL);
            Media = Format->Media;
            K_ASSERT(Media != NULL);
            if (ApplibFormatLib_RestoreDTS(Media) != AMP_OK)
                break;
            if (ApplibMuxer_ResetMovieNewSession((AMP_MOVIE_INFO_s *)Media) < 0)
                break;
            if (Stream->Func->Open(Stream, Media->Name, AMP_STREAM_MODE_WRONLY) != AMP_OK) {
                Muxer_Perror(0, 0);
                break;
            }
        }
        if (i == FormatCount) {
            for (i=0; i<FormatCount; i++) {
                Format = MuxPipe->Format[i];
                if (ApplibFormatLib_ResetMuxMediaInfo(Format->Media) != AMP_OK) {
                    Muxer_Perror(0, 0);
                    break;
                }
                if (Format->Func->Open(Format) != AMP_OK) {
                    Muxer_Perror(0, 0);
                    break;
                }
            }
            if (i == FormatCount) {
                Pipe->State = APPLIB_MUXER_STATE_RUNNING;
                if (Pipe->OnEvent(&Pipe->Hdlr, APPLIB_MUXER_EVENT_START) == 0)
                    return 0;
            }
            for (j=0; j<i; j++) {
                Format = MuxPipe->Format[i];
                if (Format->Func->Close(Format, AMP_MUX_FORMAT_CLOSE_ABORT) != AMP_OK)
                    Muxer_Perror(0, 0);
            }
            for (j=0; j<FormatCount; j++) {
                Stream = MuxPipe->Format[j]->Stream;
                if (Stream->Func->Close(Stream) != AMP_OK)
                    Muxer_Perror(0, 0);
            }
        } else {
            for (j=0; j<i; j++) {
                Stream = MuxPipe->Format[j]->Stream;
                if (Stream->Func->Close(Stream) != AMP_OK)
                    Muxer_Perror(0, 0);
            }
        }
    }
    AmpMuxer_Remove(MuxPipe);
    // release media info on error
    for (i=0; i<FormatCount; i++) {
        Format = MuxPipe->Format[i];
        K_ASSERT(Format->Media != NULL);
        ApplibMuxer_RelMediaInfo(Format->Media);
    }
    return -1;
}

static int ApplibMuxer_StartOnPrec(APPLIB_MUXER_PIPE_HDLR_IMPL_s *Pipe)
{
    int rval = -1;
    AMP_MUXER_PIPE_HDLR_s * const MuxPipe = Pipe->Pipe;
    // lock muxer
    if (AmpMuxer_LockPipe(MuxPipe) == AMP_OK) {
        if (ApplibMuxer_StartOnPrecImpl(Pipe) == 0)
            rval = 0;
        AmpMuxer_UnlockPipe(MuxPipe);
    } else {
        Muxer_Perror(0, 0);
    }
    return rval;
}

static int ApplibMuxer_StartOnIdle(APPLIB_MUXER_PIPE_HDLR_IMPL_s *Pipe)
{
    AMP_MUXER_PIPE_HDLR_s * const MuxPipe = Pipe->Pipe;
    const UINT32 FormatCount = MuxPipe->FormatCount;
    UINT32 i, j;
    AMP_MUX_FORMAT_HDLR_s *Format;
    AMP_STREAM_HDLR_s *Stream;
    AMP_MEDIA_INFO_s *Media;
    for (i=0; i<FormatCount; i++) {
        Format = MuxPipe->Format[i];
        K_ASSERT(Format != NULL);
        Stream = Format->Stream;
        K_ASSERT(Stream != NULL);
        Media = Format->Media;
        K_ASSERT(Media != NULL);
        if (Stream->Func->Open(Stream, Media->Name, AMP_STREAM_MODE_WRONLY) != AMP_OK) {
            Muxer_Perror(0, 0);
            break;
        }
    }
    if (i == FormatCount) {
        if (AmpMuxer_Add(MuxPipe) == AMP_OK) {
            Pipe->State = APPLIB_MUXER_STATE_START;
            if (AmpMuxer_Start(MuxPipe, AMBA_KAL_NO_WAIT) == AMP_OK)
                return 0;
            AmpMuxer_Remove(MuxPipe);
        }
    }
    for (j=0; j<i; j++) {
        Format = MuxPipe->Format[j];
        Stream = Format->Stream;
        if (Stream->Func->Close(Stream) != AMP_OK)
            Muxer_Perror(0, 0);
    }
    for (i=0; i<FormatCount; i++) {
        Format = MuxPipe->Format[i];
        K_ASSERT(Format->Media != NULL);
        ApplibMuxer_RelMediaInfo(Format->Media);
    }
    return -1;
}

static int ApplibMuxer_StartImpl(APPLIB_MUXER_PIPE_HDLR_IMPL_s *Pipe)
{
    K_ASSERT(Pipe->Used == TRUE);
    K_ASSERT(Pipe->Pipe != NULL);
    K_ASSERT(Pipe->OnEvent != NULL);
    if (Pipe->State == APPLIB_MUXER_STATE_PREC) {
        if (ApplibMuxer_StartOnPrec(Pipe) == 0)
            return 0;
        Pipe->State = APPLIB_MUXER_STATE_ERROR;
        if (Pipe->OnEvent(&Pipe->Hdlr, APPLIB_MUXER_EVENT_GENERAL_ERROR) < 0)
            Muxer_Perror(0, 0);
    } else if (Pipe->State == APPLIB_MUXER_STATE_IDLE) {
        if (ApplibMuxer_StartOnIdle(Pipe) == 0)
            return 0;
        Pipe->State = APPLIB_MUXER_STATE_ERROR;
        if (Pipe->OnEvent(&Pipe->Hdlr, APPLIB_MUXER_EVENT_GENERAL_ERROR) < 0)
            Muxer_Perror(0, 0);
    } else {
        Muxer_Perror(0, 0);
    }
    return -1;
}

int ApplibMuxer_Start(APPLIB_MUXER_PIPE_HDLR_s *Pipe)
{
    int rval = -1;
    K_ASSERT(Pipe != NULL);
    if (AmbaKAL_MutexTake(&g_Mmgr.Mutex, AMBA_KAL_WAIT_FOREVER) == OK) {
        if (ApplibMuxer_StartImpl((APPLIB_MUXER_PIPE_HDLR_IMPL_s *)Pipe) == 0)
            rval = 0;
        AmbaKAL_MutexGive(&g_Mmgr.Mutex);
    } else {
        Muxer_Perror(0, 0);
    }
    return rval;
}

static int ApplibMuxer_StopImpl(APPLIB_MUXER_PIPE_HDLR_IMPL_s *Pipe)
{
    AMP_MUXER_PIPE_HDLR_s * const MuxPipe = Pipe->Pipe;
    K_ASSERT(Pipe->Used == TRUE);
    K_ASSERT(Pipe->Pipe != NULL);
    K_ASSERT(MuxPipe->FormatCount > 0);
    if ((Pipe->State > APPLIB_MUXER_STATE_IDLE) && (Pipe->State < APPLIB_MUXER_STATE_END)) {
        if (AmpMuxer_Stop(MuxPipe) == AMP_OK)
            return 0;
    } else {
        Muxer_Perror(0, 0);
    }
    return -1;
}

int ApplibMuxer_Stop(APPLIB_MUXER_PIPE_HDLR_s *Pipe)
{
    int rval = -1;
    K_ASSERT(Pipe != NULL);
    if (AmbaKAL_MutexTake(&g_Mmgr.Mutex, AMBA_KAL_WAIT_FOREVER) == OK) {
        if (ApplibMuxer_StopImpl((APPLIB_MUXER_PIPE_HDLR_IMPL_s *)Pipe) == 0)
            rval = 0;
        AmbaKAL_MutexGive(&g_Mmgr.Mutex);
    } else {
        Muxer_Perror(0, 0);
    }
    return rval;
}

static int ApplibMuxer_PrerecordImpl(APPLIB_MUXER_PIPE_HDLR_IMPL_s *Pipe, UINT32 Length)
{
    if (Pipe->State == APPLIB_MUXER_STATE_IDLE) {
        AMP_MUXER_PIPE_HDLR_s * const MuxPipe = Pipe->Pipe;
        const UINT32 FormatCount = MuxPipe->FormatCount;
        APPLIB_PREC_MUX_CFG_s PrecCfg;
        AMP_MUX_FORMAT_HDLR_s *Format;
        AMP_MEDIA_INFO_s *Media;
        UINT32 i;
        if (ApplibPrecMux_GetDefaultCfg(&PrecCfg) == AMP_OK) {
            UINT32 j;
            PrecCfg.Length = Length;
            for (i=0; i<FormatCount; i++) {
                Format = MuxPipe->Format[i];
                K_ASSERT(Format != NULL);
                Media = Format->Media;
                K_ASSERT(Media != NULL);
                if (Media->MediaType != AMP_MEDIA_INFO_MOVIE) {
                    Muxer_Perror(0, 0);
                    break;
                }
                Pipe->TmpFmt[i] = Format;
                if (ApplibPrecMux_Create(&PrecCfg, &Format) != AMP_OK) {
                    Muxer_Perror(0, 0);
                    break;
                }
                Format->Media = Media;
                MuxPipe->Format[i] = Format;
                Format->Param.Movie.MaxDuration = 0xFFFFFFFF; // to avoid precmux auto split
            }
            if (i == FormatCount) {
                if (AmpMuxer_Add(MuxPipe) == AMP_OK) {
                    Pipe->State = APPLIB_MUXER_STATE_PREC;
                    if (AmpMuxer_Start(MuxPipe, AMBA_KAL_NO_WAIT) == AMP_OK)
                        return 0;
                    AmpMuxer_Remove(MuxPipe);
                }
            }
            for (j=0; j<i; j++) {
                ApplibPrecMux_Delete(MuxPipe->Format[j]);
                MuxPipe->Format[j] = Pipe->TmpFmt[j];
            }
        }
        // release media info on error
        for (i=0; i<FormatCount; i++) {
            Format = MuxPipe->Format[i];
            K_ASSERT(Format->Media != NULL);
            ApplibMuxer_RelMediaInfo(Format->Media);
        }
        Pipe->State = APPLIB_MUXER_STATE_ERROR;
        if (Pipe->OnEvent(&Pipe->Hdlr, APPLIB_MUXER_EVENT_GENERAL_ERROR) < 0)
            Muxer_Perror(0, 0);
    } else {
        Muxer_Perror(0, 0);
    }
    return -1;
}

int ApplibMuxer_Prerecord(APPLIB_MUXER_PIPE_HDLR_s *Pipe, UINT32 Length)
{
    int rval = -1;
    K_ASSERT(Pipe != NULL);
    K_ASSERT(Length > 0);
    if (AmbaKAL_MutexTake(&g_Mmgr.Mutex, AMBA_KAL_WAIT_FOREVER) == OK) {
        if (ApplibMuxer_PrerecordImpl((APPLIB_MUXER_PIPE_HDLR_IMPL_s *)Pipe, Length) == 0)
            rval = 0;
        AmbaKAL_MutexGive(&g_Mmgr.Mutex);
    } else {
        Muxer_Perror(0, 0);
    }
    return rval;
}

int ApplibMuxer_WaitComplete(APPLIB_MUXER_PIPE_HDLR_s *Pipe, UINT32 TimeOut)
{
    APPLIB_MUXER_PIPE_HDLR_IMPL_s * const MuxPipe = (APPLIB_MUXER_PIPE_HDLR_IMPL_s *)Pipe;
    K_ASSERT(Pipe != NULL);
    K_ASSERT(MuxPipe->Pipe != NULL);
    if (AmpMuxer_WaitComplete(MuxPipe->Pipe, TimeOut) == 0)
        return 0;
    return -1;
}

int ApplibMuxer_OnDataReady(AMP_FIFO_HDLR_s *Fifo)
{
    K_ASSERT(Fifo != NULL);
    if (AmpMuxer_OnDataReady(Fifo) == AMP_OK)
        return 0;
    return -1;
}

int ApplibMuxer_OnEOS(AMP_FIFO_HDLR_s *Fifo)
{
    K_ASSERT(Fifo != NULL);
    if (AmpMuxer_OnEOS(Fifo) == AMP_OK)
        return 0;
    return -1;
}

