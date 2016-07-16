/**
 *  @file PrecMux.c
 *
 *  PRE-RECORD mux format implementation
 *
 *  **History**
 *      |Date       |Name        |Comments       |
 *      |-----------|------------|---------------|
 *      |2013/07/05 |clchan      |Created        |
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
#include "ApplibPrecMux.h"
#include <fifo/Fifo.h>
#include <format/Muxer.h>
#include "ApplibFormatLib.h"

#define Prec_Perror(level, color, fmt, ...) {\
    char Buffer[160];\
    snprintf(Buffer, 160, fmt, ##__VA_ARGS__);\
    Buffer[159] = '\0';\
    AmbaPrintColor(color, "[Error]%s:%u %s", __FUNCTION__, __LINE__, Buffer);\
}

#define PREC_MUX_TIME_TO_DTS(Time, TimeScale)            ((((UINT64)(Time) / 1000) * (TimeScale)) + (((UINT64)(Time) % 1000) * (TimeScale) + 999) / 1000)
#define PREC_MUX_TIME_TO_DTS_FLOOR(Time, TimeScale)      ((((UINT64)(Time) / 1000) * (TimeScale)) + ((UINT64)(Time) % 1000) * (TimeScale) / 1000)
#define PREC_MUX_DTS_TO_TIME_CEIL(DTS, TimeScale)        (((UINT64)(DTS) / (TimeScale)) * 1000 + (((UINT64)(DTS) % (TimeScale)) * 1000 + (TimeScale) - 1) / (TimeScale))
#define PREC_MUX_DTS_TO_TIME(DTS, TimeScale)             (((UINT64)(DTS) / (TimeScale)) * 1000 + (((UINT64)(DTS) % (TimeScale)) * 1000) / (TimeScale))
#define PREC_MUX_DURATION_TO_FRAME_COUNT(Duration, TimeScale, TimePerFrame)      ((UINT32)(((UINT64)(Duration) * (TimeScale)) / ((UINT64)(TimePerFrame) * 1000)))
#define PREC_MUX_SPLIT_DTS(InitDTS, MaxDuration, TimeScale)      ((InitDTS) + PREC_MUX_TIME_TO_DTS((MaxDuration), (TimeScale)))

#define DEFAULT_MAX_LENGTH  3000

static int AppLibPrecMux_ProcessVideo(PREC_MUX_HDLR_s *Hdlr, AMP_MEDIA_TRACK_INFO_s *Track, UINT32 MaxDuration);
static int AppLibPrecMux_ProcessAudio(PREC_MUX_HDLR_s *Hdlr, AMP_MEDIA_TRACK_INFO_s *Track);
static int AppLibPrecMux_ProcessText(PREC_MUX_HDLR_s *Hdlr, AMP_MEDIA_TRACK_INFO_s *Track);

static PREC_MUX_MGR_s g_PmuxMgr = {0};
static AMP_MUX_FORMAT_s g_Pmux;
static int event_fifo_prec_flag = 0;
static int G_splite_state = 0;
static int G_prec_end = 0;
static UINT64 g_target_dts = 0;

static int AppLibPrecMux_CheckVideoLimit(AMP_MEDIA_INFO_s *media, AMP_MEDIA_TRACK_INFO_s *track, AMP_STREAM_HDLR_s *stream, UINT32 maxDuration, UINT64 maxSize)
{
    const UINT64 MaxDTS = PREC_MUX_SPLIT_DTS(track->InitDTS, maxDuration, track->TimeScale);
    AMP_MEDIA_TRACK_INFO_s *DefTrack;
    K_ASSERT(media != NULL);
    K_ASSERT(media->MediaType == AMP_MEDIA_INFO_MOVIE);
    K_ASSERT(track != NULL);
    K_ASSERT(track->TrackType == AMP_MEDIA_TRACK_TYPE_VIDEO);
    if (track->DTS >= MaxDTS) {
        if (track->Info.Video.IsDefault){
            return AMP_FORMAT_EVENT_REACH_DURATION_LIMIT;
           }
        DefTrack = ApplibFormatLib_GetDefaultTrack(media, AMP_MEDIA_TRACK_TYPE_VIDEO);
        K_ASSERT(DefTrack != NULL);
        if (DefTrack->DTS == AMP_FORMAT_MAX_TIMESTAMP) {   // TODO: what if default video track is much shorter? (default EOS, but others limit)
            if (track->DTS >= DefTrack->NextDTS)
                return AMP_FORMAT_EVENT_REACH_DURATION_LIMIT;
        }
    }
    if (stream != NULL) {
        if (stream->Func->GetLength(stream) >= maxSize) {
            if (track->Info.Video.IsDefault)
                return AMP_FORMAT_EVENT_REACH_SIZE_LIMIT;
            DefTrack = ApplibFormatLib_GetDefaultTrack(media, AMP_MEDIA_TRACK_TYPE_VIDEO);
            K_ASSERT(DefTrack != NULL);
            if (DefTrack->DTS == AMP_FORMAT_MAX_TIMESTAMP) // TODO: what if default video track is much shorter? (default EOS, but others limit)
                return AMP_FORMAT_EVENT_REACH_SIZE_LIMIT;
        }
    }
    return AMP_FORMAT_EVENT_OK;
}

/**
 * Get the default pre-record muxer manager configure
 * @param [out] Config the buffer to get default configure
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
int AppLibPrecMux_GetInitDefaultCfg(PREC_MUX_INIT_CFG_s *config)
{
    K_ASSERT(config != NULL);
    memset(config, 0, sizeof(PREC_MUX_INIT_CFG_s));
    config->MaxHdlr = 1;
    config->BufferSize = AppLibPrecMux_GetRequiredBufferSize(config->MaxHdlr);
    return AMP_OK;
}

/**
 * Get the required buffer size
 * @param [in] MaxHdlr the number of pre-record muxers
 * @return the required buffer size
 */
UINT32 AppLibPrecMux_GetRequiredBufferSize(UINT8 maxHdlr)
{
    K_ASSERT(maxHdlr > 0);
    return (UINT32)maxHdlr * (sizeof(PREC_MUX_HDLR_s) + sizeof(void *));
}

/**
 * Initiate the core of pre-record muxer manager
 * @param [in] Config the initial configure
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
int AppLibPrecMux_Init(PREC_MUX_INIT_CFG_s *config)
{
    K_ASSERT(config != NULL);
    K_ASSERT(config->MaxHdlr > 0);
    K_ASSERT(config->Buffer != NULL);
    K_ASSERT(config->BufferSize == AppLibPrecMux_GetRequiredBufferSize(config->MaxHdlr));
    if (g_PmuxMgr.Init == FALSE) {
        if (AmbaKAL_MutexCreate(&g_PmuxMgr.Mutex) == OK) {
            if (AmbaKAL_BlockPoolCreate(&g_PmuxMgr.HdlrBpl, sizeof(PREC_MUX_HDLR_s), config->Buffer, config->BufferSize) == OK) {
                g_PmuxMgr.Init= TRUE;
                return 0;
            }
            Prec_Perror(0, 0, "create block pool failed");
            AmbaKAL_MutexDelete(&g_PmuxMgr.Mutex);
        } else {
            Prec_Perror(0, 0, "create mutex failed");
        }
    } else {
        Prec_Perror(0, 0, "already initiated");
    }
    return -1;

}

/**
 * Get the default pre-record muxer configure
 * @param [out] Config the buffer to get default configure
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
int AppLibPrecMux_GetDefaultCfg(PREC_MUX_CFG_s *config)
{
    K_ASSERT(config != NULL);
    memset(config, 0, sizeof(PREC_MUX_CFG_s));
    config->Length = DEFAULT_MAX_LENGTH;
    return AMP_OK;
}

static PREC_MUX_HDLR_s *AppLibPrecMux_CreateImpl(PREC_MUX_CFG_s *config)
{
    void *Buffer;
    if (AmbaKAL_BlockPoolAllocate(&g_PmuxMgr.HdlrBpl, &Buffer, AMBA_KAL_NO_WAIT) == OK) {
        PREC_MUX_HDLR_s * const Hdlr = (PREC_MUX_HDLR_s *)Buffer;
        memset(Hdlr, 0, sizeof(PREC_MUX_HDLR_s));
        if (AmbaKAL_MutexCreate(&Hdlr->Mutex) == OK) {
            Hdlr->Hdlr.Func = &g_Pmux;
            Hdlr->Hdlr.Param.Movie.MaxDuration = 0xFFFFFFFF;
            Hdlr->Hdlr.Param.Movie.MaxSize = 0xFFFFFFFFFFFFFFFFull;
            Hdlr->Length = config->Length;
            Hdlr->master = 0;            //default slave
            Hdlr->limitedend_speedup = 0;
            return Hdlr;
        }
        Prec_Perror(0, 0, "create mutex failed");
        AmbaKAL_BlockPoolFree(Hdlr);
    } else {
        Prec_Perror(0, 0, "allocate block pool failed");
    }
    return NULL;
}

/**
 * Create a pre-record mux
 * @param [in] Config the configure of the muxer
 * @param [out] Hdlr the double pointer to get the handler
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
int AppLibPrecMux_Create(PREC_MUX_CFG_s *config, AMP_MUX_FORMAT_HDLR_s **hdlr)
{
    int rval = AMP_ERROR_GENERAL_ERROR;
    K_ASSERT(config != NULL);
    K_ASSERT(hdlr != NULL);
    if (AmbaKAL_MutexTake(&g_PmuxMgr.Mutex, AMBA_KAL_WAIT_FOREVER) == OK) {
        PREC_MUX_HDLR_s *const PrecMux = AppLibPrecMux_CreateImpl(config);
        if (PrecMux != NULL) {
            *hdlr = (AMP_MUX_FORMAT_HDLR_s *)PrecMux;
            rval = AMP_OK;
        }
        AmbaKAL_MutexGive(&g_PmuxMgr.Mutex);
    }
    return rval;
}

static int AppLibPrecMux_DeleteImpl(PREC_MUX_HDLR_s *hdlr)
{
    int rval = 0;
    if (AmbaKAL_MutexTake(&hdlr->Mutex, AMBA_KAL_WAIT_FOREVER) == OK) {
        // delete mutex
        if (AmbaKAL_MutexDelete(&hdlr->Mutex) != OK) {
            Prec_Perror(0, 0, "delete mutex failed");
            rval = -1;
        }
        if (AmbaKAL_BlockPoolFree(hdlr) != OK) {
            Prec_Perror(0, 0, "free block pool failed");
            rval = -1;
        }
         event_fifo_prec_flag = 0;
    } else {
        Prec_Perror(0, 0, "take mutex failed");
        rval = -1;
    }
    return rval;
}

/**
 * Delete a pre-record muxer
 * @param [in] Hdlr the muxer handler to close
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
int AppLibPrecMux_Delete(AMP_MUX_FORMAT_HDLR_s *hdlr)
{
    int rval = AMP_ERROR_GENERAL_ERROR;
    K_ASSERT(hdlr != NULL);
    if (AmbaKAL_MutexTake(&g_PmuxMgr.Mutex, AMBA_KAL_WAIT_FOREVER) == OK) {
        if (AppLibPrecMux_DeleteImpl((PREC_MUX_HDLR_s *)hdlr) == 0)
            rval = AMP_OK;
        AmbaKAL_MutexGive(&g_PmuxMgr.Mutex);
    } else {
        Prec_Perror(0, 0, "take mutex failed");
    }
    return rval;
}

static int AppLibPrecMux_OpenImpl(PREC_MUX_HDLR_s *hdlr)
{
    AMP_MOVIE_INFO_s * const Movie = (AMP_MOVIE_INFO_s *)hdlr->Hdlr.Media;
    AMP_MEDIA_TRACK_INFO_s * const DefTrack = ApplibFormatLib_GetDefaultTrack((AMP_MEDIA_INFO_s *)Movie, AMP_MEDIA_TRACK_TYPE_VIDEO);
    K_ASSERT(DefTrack != NULL);
    memcpy(&hdlr->Log, DefTrack, sizeof(hdlr->Log));
    return 0;
}

static int AppLibPrecMux_Open(AMP_MUX_FORMAT_HDLR_s *hdlr)
{
    int rval = AMP_ERROR_GENERAL_ERROR;
    PREC_MUX_HDLR_s * const PrecMux = (PREC_MUX_HDLR_s *)hdlr;
    K_ASSERT(PrecMux != NULL);
    if (AmbaKAL_MutexTake(&PrecMux->Mutex, AMBA_KAL_WAIT_FOREVER) == OK) {
        if (!PrecMux->Open) {
            if (AppLibPrecMux_OpenImpl(PrecMux) == 0) {
                PrecMux->Open = TRUE;
                rval = AMP_OK;
            } else {
                Prec_Perror(0, 0, "AppLibPrecMux_OpenImpl() failed");
            }
        } else {
            Prec_Perror(0, 0, "not opened");
        }
        AmbaKAL_MutexGive(&PrecMux->Mutex);
    } else {
        Prec_Perror(0, 0, "take mutex failed");
    }
    return rval;
}

static int AppLibPrecMux_CloseImpl(PREC_MUX_HDLR_s *hdlr)
{
    AMP_MOVIE_INFO_s * const Movie = (AMP_MOVIE_INFO_s *)hdlr->Hdlr.Media;
    AMP_MEDIA_TRACK_INFO_s * const DefTrack = ApplibFormatLib_GetDefaultTrack((AMP_MEDIA_INFO_s *)Movie, AMP_MEDIA_TRACK_TYPE_VIDEO);
    const UINT32 MaxDuration = PREC_MUX_DTS_TO_TIME_CEIL(DefTrack->NextDTS - DefTrack->InitDTS, DefTrack->TimeScale);
    int r;
    AMP_MEDIA_TRACK_INFO_s *Track;
    K_ASSERT(DefTrack != NULL);
    while (0) {
        Track = ApplibFormatLib_GetShortestTrack((AMP_MEDIA_INFO_s *)Movie);    // also implies EOS check
        if (Track == NULL)
            break;
        if (Track->TrackType == AMP_MEDIA_TRACK_TYPE_VIDEO) {
            r = AppLibPrecMux_ProcessVideo(hdlr, Track, MaxDuration);
            if (r != AMP_FORMAT_EVENT_OK) {
                if (r == AMP_FORMAT_EVENT_REACH_DURATION_LIMIT) {
                    // stop muxing this track
                    Track->DTS = AMP_FORMAT_MAX_TIMESTAMP;
                    if (ApplibFormatLib_IsVideoComplete(Movie))
                        break;
                } else if (r == AMP_FORMAT_EVENT_DATA_EMPTY) {
                    AmbaKAL_TaskSleep(5);
                } else if (r != AMP_FORMAT_EVENT_EOS) {
                    Prec_Perror(0, 0, "fifo error");
                    return -1;
                }
            }
        } else if (Track->TrackType == AMP_MEDIA_TRACK_TYPE_AUDIO) {
            r = AppLibPrecMux_ProcessAudio(hdlr, Track);
            if (r != AMP_FORMAT_EVENT_OK) {
                if (r == AMP_FORMAT_EVENT_DATA_EMPTY) {
                    AmbaKAL_TaskSleep(5);
                } else if (r != AMP_FORMAT_EVENT_EOS) {
                    Prec_Perror(0, 0, "fifo error");
                    return -1;
                }
            }
        } else if (Track->TrackType == AMP_MEDIA_TRACK_TYPE_TEXT) {
            r = AppLibPrecMux_ProcessText(hdlr, Track);
            if (r != AMP_FORMAT_EVENT_OK) {
                if (r == AMP_FORMAT_EVENT_DATA_EMPTY) {
                    AmbaKAL_TaskSleep(5);
                } else if (r != AMP_FORMAT_EVENT_EOS) {
                    Prec_Perror(0, 0, "fifo error");
                    return -1;
                }
            }
        } else {
            Prec_Perror(0, 0, "incorrect track type");
            return -1;
        }
    }
    if (ApplibFormatLib_RestoreDTS((AMP_MEDIA_INFO_s *)Movie) != AMP_OK)
        return -1;
    return 0;
}

static int AppLibPrecMux_Close(AMP_MUX_FORMAT_HDLR_s *hdlr, UINT8 mode)
{
    int rval = AMP_ERROR_GENERAL_ERROR;
    PREC_MUX_HDLR_s * const PrecMux = (PREC_MUX_HDLR_s *)hdlr;
    K_ASSERT(PrecMux != NULL);
    if (AmbaKAL_MutexTake(&PrecMux->Mutex, AMBA_KAL_WAIT_FOREVER) == OK) {
        if (PrecMux->Open) {
            ApplibFormatLib_CheckEnd(hdlr->Media);
            if (AppLibPrecMux_CloseImpl(PrecMux) == 0) {
                AMP_MOVIE_INFO_s * const Movie = (AMP_MOVIE_INFO_s *)hdlr->Media;
                UINT32 i;
                char Buffer[256];
                sprintf(Buffer, "%s() : remove", __FUNCTION__);
                for (i=0; i<Movie->TrackCount; i++) {
                    char b[32];
                    AMP_MEDIA_TRACK_INFO_s * const Track = &Movie->Track[i];
                    switch (Track->TrackType) {
                    case AMP_MEDIA_TRACK_TYPE_VIDEO:
                        sprintf(b, " video[%u]", Track->FrameCount);
                        strcat(Buffer, b);
                        break;
                    case AMP_MEDIA_TRACK_TYPE_AUDIO:
                        sprintf(b, " audio[%u]", Track->FrameCount);
                        strcat(Buffer, b);
                        break;
                    case AMP_MEDIA_TRACK_TYPE_TEXT:
                        sprintf(b, " text[%u]", Track->FrameCount);
                        strcat(Buffer, b);
                        break;
                    default:
                        Prec_Perror(0, 0, "incorrect track type");
                        break;
                    }
                }
                AmbaPrint(Buffer);
                rval = AMP_OK;
            } else {
                Prec_Perror(0, 0, "AppLibPrecMux_CloseImpl() failed");
            }
            PrecMux->Open = FALSE;
        } else {
            Prec_Perror(0, 0, "not opened");
        }
        AmbaKAL_MutexGive(&PrecMux->Mutex);
    } else {
        Prec_Perror(0, 0, "take mutex failed");
    }
    return rval;
}

static int AppLibPrecMux_CheckVideoTrackImpl(AMP_MEDIA_TRACK_INFO_s *track, AMP_MEDIA_TRACK_INFO_s *log, UINT32 maxDuration, UINT32 length)
{
    AMP_FIFO_HDLR_s * const Fifo = track->Fifo;
    K_ASSERT(log->FrameCount >= track->FrameCount);
    if (log->DTS != AMP_FORMAT_MAX_TIMESTAMP) {
        // log->DTS >= log->NextDTS >= track->NextDTS (log->NextDTS will go ahead until EOS is met)
        // track->DTS >= track->NextDTS
        AMP_VIDEO_TRACK_INFO_s * const Video = &log->Info.Video;
        const UINT32 Diff = log->FrameCount - track->FrameCount;
        const UINT32 Pos = (Video->Mode == AMP_VIDEO_MODE_I_A_FRM_PER_SMP) ? (Diff << 1) : Diff;
        AMP_BITS_DESC_s *Frame1;
        AMP_BITS_DESC_s *Frame2;
        int er = AmpFifo_PeekEntry(Fifo, &Frame1, Pos);
        if (er == AMP_ERROR_FIFO_EMPTY){
            return AMP_FORMAT_EVENT_DATA_EMPTY;
         }
        if (er != AMP_OK) {
            Prec_Perror(0, 0, "fifo error");
            return AMP_FORMAT_EVENT_FIFO_ERROR;
        }
        if (Frame1->Size > AMP_FIFO_MARK_EOS) {
            Prec_Perror(0, 0, "incorrect size");
            return AMP_FORMAT_EVENT_FIFO_ERROR;
        }
        if (AMP_FORMAT_IS_EOS_FRAME(Frame1)) {
            log->DTS = AMP_FORMAT_MAX_TIMESTAMP;
        } else {
            if (Video->Mode > AMP_VIDEO_MODE_P) {
                er = AmpFifo_PeekEntry(Fifo, &Frame2, Pos + 1);
                if (er != AMP_OK) {
                    if (er == AMP_ERROR_FIFO_EMPTY)
                        return AMP_FORMAT_EVENT_DATA_EMPTY;
                    Prec_Perror(0, 0, "fifo error");
                    return AMP_FORMAT_EVENT_FIFO_ERROR;
                }
                if (Frame2->Size >= AMP_FIFO_MARK_EOS) {
                    Prec_Perror(0, 0, "incorrect size");
                    return AMP_FORMAT_EVENT_FIFO_ERROR;
                }
            }
            if ((Video->InitPTS == 0) && (Video->FrameCountAfterResume == 0))
                Video->InitPTS = Frame1->Pts;
            ApplibFormatLib_ConvertPTS(log, Frame1);
            ApplibFormatLib_UpdateMuxTrack(log, (Video->Mode == AMP_VIDEO_MODE_I_A_FLD_PER_SMP) ? 2 : 1);
#if 0
            {
               AMP_FIFO_INFO_s Info;
               if (AmpFifo_GetInfo(Fifo, &Info) == AMP_OK) {
                  const UINT32 Avail = Info.AvailEntries;
                  Prec_Perror(0, 1, "%d %d %llu %llu fifo avail %d %d",length,maxDuration,track->DTS,log->DTS,Avail,PREC_MUX_DURATION_TO_FRAME_COUNT(length, track->TimeScale, track->TimePerFrame / track->Info.Video.VFR));
             //     if(Avail>=PREC_MUX_DURATION_TO_FRAME_COUNT(length, track->TimeScale, track->TimePerFrame / track->Info.Video.VFR))
             //         return AMP_FORMAT_EVENT_OK;
             //      else
             //         return AMP_FORMAT_EVENT_DATA_EMPTY;
             }
            }
#endif
            if(event_fifo_prec_flag)
               return AMP_FORMAT_EVENT_OK;
            if (log->DTS <= PREC_MUX_TIME_TO_DTS_FLOOR(length, track->TimeScale)){
                return AMP_FORMAT_EVENT_DATA_EMPTY;
             }
        }
    }
    return AMP_FORMAT_EVENT_OK;
}

static int AppLibPrecMux_CheckVideoTrack(PREC_MUX_HDLR_s *hdlr, AMP_MEDIA_TRACK_INFO_s *track, UINT32 maxDuration)
{

    if (track->Info.Video.IsDefault == TRUE) {  // only default video track need to check ahead
        AMP_MEDIA_TRACK_INFO_s * const Log = &hdlr->Log;
        const int r = AppLibPrecMux_CheckVideoTrackImpl(track, Log, maxDuration, hdlr->Length);
        if (r != AMP_FORMAT_EVENT_OK) {
            if (r != AMP_FORMAT_EVENT_DATA_EMPTY)
                return r;
            // data empty, but might still be able to go ahead (if log > real)
            if (Log->FrameCount == track->FrameCount)   // really no data to process
                return AMP_FORMAT_EVENT_DATA_EMPTY;
            K_ASSERT(track->DTS != AMP_FORMAT_MAX_TIMESTAMP);
            // check if default video track has reached limit (limit could be too small or changed)
            if (track->DTS < PREC_MUX_SPLIT_DTS(track->InitDTS, (maxDuration <= hdlr->Length) ? 0 : (maxDuration - hdlr->Length), track->TimeScale)){
                return AMP_FORMAT_EVENT_DATA_EMPTY;
               }
            if (Log->DTS <= PREC_MUX_TIME_TO_DTS_FLOOR(maxDuration, track->TimeScale)){
                return AMP_FORMAT_EVENT_DATA_EMPTY;
              }
        }
    }
    return AMP_FORMAT_EVENT_OK;
}


static int AppLibPrecMux_ProcessVideo(PREC_MUX_HDLR_s *hdlr, AMP_MEDIA_TRACK_INFO_s *track, UINT32 maxDuration)
{
    AMP_FIFO_HDLR_s * const Fifo = track->Fifo;
    AMP_VIDEO_TRACK_INFO_s * const Video = &track->Info.Video;
    AMP_BITS_DESC_s *Frame1;
    AMP_BITS_DESC_s *Frame2;
    //UINT64 PTS;
    int er;
    int r = AppLibPrecMux_CheckVideoTrack(hdlr, track, maxDuration);
    if (r != AMP_FORMAT_EVENT_OK)
        return r;
    er = AmpFifo_PeekEntry(Fifo, &Frame1, 0);
    if (er == AMP_ERROR_FIFO_EMPTY) {
        if (Video->IsDefault) { // should not happen on default video track
            Prec_Perror(0, 0, "is default track");
            return AMP_FORMAT_EVENT_FIFO_ERROR;
        }
        return AMP_FORMAT_EVENT_DATA_EMPTY;
    }
    if (er != AMP_OK) {
        Prec_Perror(0, 0, "fifo error");
        return AMP_FORMAT_EVENT_FIFO_ERROR;
    }
    if (Frame1->Size > AMP_FIFO_MARK_EOS) {
        Prec_Perror(0, 0, "incorrect size");
        return AMP_FORMAT_EVENT_FIFO_ERROR;
    }
    if (AMP_FORMAT_IS_EOS_FRAME(Frame1)) {
        track->DTS = AMP_FORMAT_MAX_TIMESTAMP;
        AmpFifo_RemoveEntry(Fifo, 1);
        AmbaPrint("PRECMUX met Video EOS");
        return AMP_FORMAT_EVENT_EOS;
    }
    if (Video->Mode > AMP_VIDEO_MODE_P) {
        er = AmpFifo_PeekEntry(Fifo, &Frame2, 1);
        if (er != AMP_OK) {
            if (er == AMP_ERROR_FIFO_EMPTY) {
                if (Video->IsDefault) { // should not happen on default video track
                    Prec_Perror(0, 0, "is default track");
                    return AMP_FORMAT_EVENT_FIFO_ERROR;
                }
                return AMP_FORMAT_EVENT_DATA_EMPTY;
            }
            Prec_Perror(0, 0, "fifo error");
            return AMP_FORMAT_EVENT_FIFO_ERROR;
        }
        if (Frame2->Size >= AMP_FIFO_MARK_EOS) {
            Prec_Perror(0, 0, "incorrect size");
            return AMP_FORMAT_EVENT_FIFO_ERROR;
        }
    }
    if ((Video->InitPTS == 0) && (Video->FrameCountAfterResume == 0))
        Video->InitPTS = Frame1->Pts;
    if (Frame1->Type == AMP_FIFO_TYPE_IDR_FRAME) {
        UINT32 Max = (maxDuration <= hdlr->Length) ? 0 : (maxDuration - hdlr->Length);
        if(G_splite_state)
          Max = maxDuration;
        r = AppLibPrecMux_CheckVideoLimit(hdlr->Hdlr.Media, track, NULL, Max, 0xFFFFFFFFFFFFFFFFull);
        if((hdlr->master == 0)&&(r == AMP_FORMAT_EVENT_REACH_DURATION_LIMIT)&&(g_target_dts !=AMP_FORMAT_MAX_TIMESTAMP))
           r = AMP_FORMAT_EVENT_OK;
        if (r != AMP_FORMAT_EVENT_OK)
            return r;
    }
    event_fifo_prec_flag = 1;

    ApplibFormatLib_ConvertPTS(track, Frame1);
    //PTS = FormatLib_ConvertPTS(track, Frame1);
    //AmbaPrint("fifo[%p] video %u %u %llu %u, log:%llu", Fifo, Frame1->SeqNum, Frame1->Type, PTS, Frame1->Size, hdlr->Log.DTS);
    if (Video->Mode == AMP_VIDEO_MODE_P) {
        ApplibFormatLib_UpdateMuxTrack(track, 1);
        AmpFifo_RemoveEntry(Fifo, 1);
    } else {
        // interlace mode
        //AmbaPrint("fifo[%p] video %u %u %llu %u", Fifo, Frame2->SeqNum, Frame2->Type, PTS, Frame2->Size);
        ApplibFormatLib_UpdateMuxTrack(track, (Video->Mode == AMP_VIDEO_MODE_I_A_FRM_PER_SMP) ? 1 : 2);
        AmpFifo_RemoveEntry(Fifo, 2);
    }
    return AMP_FORMAT_EVENT_OK;
}

static int AppLibPrecMux_ProcessAudio(PREC_MUX_HDLR_s *hdlr, AMP_MEDIA_TRACK_INFO_s *track)
{
    AMP_FIFO_HDLR_s * const Fifo = track->Fifo;
    AMP_BITS_DESC_s *Frame;
    const int er = AmpFifo_PeekEntry(Fifo, &Frame, 0);
    if (er == AMP_ERROR_FIFO_EMPTY)
        return AMP_FORMAT_EVENT_DATA_EMPTY;
    if (er != AMP_OK) {
        Prec_Perror(0, 0, "fifo error");
        return AMP_FORMAT_EVENT_FIFO_ERROR;
    }
    if (Frame->Size > AMP_FIFO_MARK_EOS) {
        Prec_Perror(0, 0, "incorrect size");
        return AMP_FORMAT_EVENT_FIFO_ERROR;
    }
    if (AMP_FORMAT_IS_EOS_FRAME(Frame)) {
        track->DTS = AMP_FORMAT_MAX_TIMESTAMP;
        AmpFifo_RemoveEntry(Fifo, 1);
        AmbaPrint("PRECMUX met Audio EOS");
        return AMP_FORMAT_EVENT_EOS;
    }
    //AmbaPrint("fifo[%p] audio %u %u %llu %u, log:%llu", Fifo, Frame->SeqNum, Frame->Type, track->DTS, Frame->Size, hdlr->Log.DTS);
    ApplibFormatLib_UpdateMuxTrack(track, 1);
    AmpFifo_RemoveEntry(Fifo, 1);
    return AMP_FORMAT_EVENT_OK;
}

static int AppLibPrecMux_ProcessText(PREC_MUX_HDLR_s *hdlr, AMP_MEDIA_TRACK_INFO_s *track)
{
    AMP_FIFO_HDLR_s * const Fifo = track->Fifo;
    AMP_BITS_DESC_s *Frame;
    const int er = AmpFifo_PeekEntry(Fifo, &Frame, 0);
    if (er == AMP_ERROR_FIFO_EMPTY)
        return AMP_FORMAT_EVENT_DATA_EMPTY;
    if (er != AMP_OK) {
        Prec_Perror(0, 0, "fifo error");
        return AMP_FORMAT_EVENT_FIFO_ERROR;
    }
    if (Frame->Size > AMP_FIFO_MARK_EOS) {
        Prec_Perror(0, 0, "incorrect size");
        return AMP_FORMAT_EVENT_FIFO_ERROR;
    }
    if (AMP_FORMAT_IS_EOS_FRAME(Frame)) {
        track->DTS = AMP_FORMAT_MAX_TIMESTAMP;
        AmpFifo_RemoveEntry(Fifo, 1);
        AmbaPrint("PRECMUX met Text EOS");
        return AMP_FORMAT_EVENT_EOS;
    }
    //AmbaPrint("fifo[%p] text %u %u %llu %u, log:%llu", Fifo, Frame->SeqNum, Frame->Type, track->DTS, Frame->Size, hdlr->Log.DTS);
    ApplibFormatLib_UpdateMuxTrack(track, 1);
    AmpFifo_RemoveEntry(Fifo, 1);
    return AMP_FORMAT_EVENT_OK;
}

static int AppLibPrecMux_ProcessImpl(PREC_MUX_HDLR_s *hdlr, UINT64 targetTime, UINT32 maxDuration, UINT32 length)
{
    AMP_MOVIE_INFO_s * const Movie = (AMP_MOVIE_INFO_s *)hdlr->Hdlr.Media;
    UINT64 TargetDTS = PREC_MUX_TIME_TO_DTS_FLOOR(targetTime, Movie->Track[0].TimeScale);
    int r;
    AMP_MEDIA_TRACK_INFO_s *Track;

    if (targetTime == AMP_FORMAT_MAX_TIMESTAMP) {
        TargetDTS = AMP_FORMAT_MAX_TIMESTAMP;
    } else {
        TargetDTS = PREC_MUX_TIME_TO_DTS_FLOOR(targetTime, Movie->Track[0].TimeScale);
    }
    g_target_dts = TargetDTS;

    while (TRUE) {
        Track = ApplibFormatLib_GetShortestTrack((AMP_MEDIA_INFO_s *)Movie);    // also implies EOS check

        if (Track == NULL) /**< All Track EOS */
            return AMP_FORMAT_EVENT_EOS;

        if (hdlr->Log.NextDTS >= TargetDTS)
            break;
        if(G_prec_end&&(targetTime == AMP_FORMAT_MAX_TIMESTAMP))
            return AMP_FORMAT_EVENT_EOS;

        if (Track->TrackType == AMP_MEDIA_TRACK_TYPE_VIDEO) {
            r = AppLibPrecMux_ProcessVideo(hdlr, Track, maxDuration);
            if (r != AMP_FORMAT_EVENT_OK) {
                if (r == AMP_FORMAT_EVENT_REACH_DURATION_LIMIT) {
                    // stop muxing this track
                    Track->DTS = AMP_FORMAT_MAX_TIMESTAMP;
                    if (ApplibFormatLib_IsVideoComplete(Movie))
                        return r;
                } else if (r != AMP_FORMAT_EVENT_EOS) {
                    return r;
                }
            }
        } else if (Track->TrackType == AMP_MEDIA_TRACK_TYPE_AUDIO) {
            r = AppLibPrecMux_ProcessAudio(hdlr, Track);
            if (r != AMP_FORMAT_EVENT_OK) {
                if (r != AMP_FORMAT_EVENT_EOS)
                    return r;
            }
        } else if (Track->TrackType == AMP_MEDIA_TRACK_TYPE_TEXT) {
            r = AppLibPrecMux_ProcessText(hdlr, Track);
            if (r != AMP_FORMAT_EVENT_OK) {
                if (r != AMP_FORMAT_EVENT_EOS)
                    return r;
            }
        } else {
            Prec_Perror(0, 0, "incorrect track type");
            return AMP_FORMAT_EVENT_GENERAL_ERROR;
        }
    }
    return AMP_FORMAT_EVENT_OK;
}

static int AppLibPrecMux_Process(AMP_MUX_FORMAT_HDLR_s *hdlr, UINT64 param)
{
    int rval = AMP_FORMAT_EVENT_GENERAL_ERROR;
    PREC_MUX_HDLR_s * const PrecMux = (PREC_MUX_HDLR_s *)hdlr;
    K_ASSERT(PrecMux != NULL);
    if (AmbaKAL_MutexTake(&PrecMux->Mutex, AMBA_KAL_WAIT_FOREVER) == OK) {
        rval = AppLibPrecMux_ProcessImpl(PrecMux, param, hdlr->Param.Movie.MaxDuration, PrecMux->Length);
        AmbaKAL_MutexGive(&PrecMux->Mutex);
    } else {
        Prec_Perror(0, 0, "take mutex failed");
    }
    return rval;
}

static UINT64 AppLibPrecMux_GetMediaTime(AMP_MUX_FORMAT_HDLR_s *hdlr)
{
    PREC_MUX_HDLR_s * const PrecMux = (PREC_MUX_HDLR_s *)hdlr;
    AMP_MOVIE_INFO_s * const Movie = (AMP_MOVIE_INFO_s *)hdlr->Media;
    AMP_MEDIA_TRACK_INFO_s * const DefTrack = ApplibFormatLib_GetDefaultTrack((AMP_MEDIA_INFO_s *)Movie, AMP_MEDIA_TRACK_TYPE_VIDEO);
    K_ASSERT(hdlr != NULL);
    K_ASSERT(Movie != NULL);
    K_ASSERT(Movie->MediaType == AMP_MEDIA_INFO_MOVIE);
    K_ASSERT(DefTrack != NULL);
    if (!PrecMux->Open)  // not opened
        return PREC_MUX_DTS_TO_TIME(DefTrack->NextDTS, DefTrack->TimeScale);
    return PREC_MUX_DTS_TO_TIME(PrecMux->Log.NextDTS, DefTrack->TimeScale);
}

static BOOL AppLibPrecMux_CheckDataReady(AMP_MUX_FORMAT_HDLR_s *hdlr, UINT32 delayTime)
{
    PREC_MUX_HDLR_s * const PrecMux = (PREC_MUX_HDLR_s *)hdlr;
    AMP_MOVIE_INFO_s * const Movie = (AMP_MOVIE_INFO_s *)hdlr->Media;
    AMP_MEDIA_TRACK_INFO_s * const Track = ApplibFormatLib_GetShortestTrack((AMP_MEDIA_INFO_s *)Movie);
    K_ASSERT(Movie != NULL);
    if (Track != NULL) {
        AMP_FIFO_HDLR_s * const Fifo = Track->Fifo;
        AMP_FIFO_INFO_s Info;
        K_ASSERT(Fifo != NULL);
        if (AmpMuxer_HasEOS(Fifo) == TRUE)
            return TRUE;
        if (AmpFifo_GetInfo(Fifo, &Info) == AMP_OK) {
            const UINT32 Avail = Info.AvailEntries;
            AMP_MEDIA_TRACK_INFO_s * const DefTrack = ApplibFormatLib_GetDefaultTrack((AMP_MEDIA_INFO_s *)Movie, AMP_MEDIA_TRACK_TYPE_VIDEO);
            UINT32 Diff = PREC_MUX_DURATION_TO_FRAME_COUNT(PrecMux->Length, Track->TimeScale, Track->TimePerFrame / Track->Info.Video.VFR);//PrecMux->Log.FrameCount - DefTrack->FrameCount;
            UINT32 Limit;
            K_ASSERT(DefTrack != NULL);
            if (Track->TrackType == AMP_MEDIA_TRACK_TYPE_VIDEO) {
                // TODO: peek last frame, use dummy Movie to call ConvertPTS() to get its PTS
                // if VFR, must use original TimePerFrame
                Limit = PREC_MUX_DURATION_TO_FRAME_COUNT(delayTime, Track->TimeScale, Track->TimePerFrame / Track->Info.Video.VFR);
                if (Track->Info.Video.Mode == AMP_VIDEO_MODE_I_A_FRM_PER_SMP) {
                    Limit = Limit << 1;
                    Diff = Diff << 1;
                }
            } else {
                Limit = PREC_MUX_DURATION_TO_FRAME_COUNT(delayTime, Track->TimeScale, Track->TimePerFrame);
            }
            if (Track == DefTrack) {
                if (Avail > Limit + Diff)
                    return TRUE;
            } else {
                if (Avail > Limit)
                    return TRUE;
            }
            if (Avail > 0) {
                // check if default video track has reached limit (also implies the check for eos)
                const UINT32 MaxDuration = hdlr->Param.Movie.MaxDuration;
                const UINT32 Max = (MaxDuration <= PrecMux->Length) ? 0 : (MaxDuration - PrecMux->Length);
                if (DefTrack->DTS >= PREC_MUX_SPLIT_DTS(DefTrack->InitDTS, Max, DefTrack->TimeScale))
                    return TRUE;
            }
        } else {
            Prec_Perror(0, 0, "AmpFifo_GetInfo() failed");
        }
    }
    return FALSE;
}

static int AppLibPrecMux_Func(AMP_MUX_FORMAT_HDLR_s *hdlr, UINT32 cmd, UINT32 param)
{
    // TODO:
    return 0;
}

int  AppLibPrecMux_SetPrecLength(AMP_MUX_FORMAT_HDLR_s *hdlr, UINT32 length)
{
    PREC_MUX_HDLR_s * const PrecMux = (PREC_MUX_HDLR_s *)hdlr;
    AMP_MOVIE_INFO_s * const Movie = (AMP_MOVIE_INFO_s *)hdlr->Media;
    AMP_MEDIA_TRACK_INFO_s *Track;
    UINT32 gop_time;
    UINT64 gop_time_tick;
    UINT32 gop_add;
    K_ASSERT(PrecMux != NULL);
    Track = ApplibFormatLib_GetShortestTrack((AMP_MEDIA_INFO_s *)Movie);    // also implies EOS check
    gop_time_tick = ((Track->TimePerFrame / Track->Info.Video.VFR)*Track->Info.Video.GOPSize*1000);
    gop_time = ((Track->TimePerFrame / Track->Info.Video.VFR)*Track->Info.Video.GOPSize*1000)/Track->TimeScale;
    gop_add = (UINT32)(((UINT64)(length)*Track->TimeScale)%gop_time_tick);
    //Prec_Perror(0, 0, "AppLibPrecMux_SetPrecLength %d %d %d %d %d",gop_time,gop_add,Track->Info.Video.GOPSize,Track->TimeScale,Track->TimePerFrame);
    if(length){
      if(gop_add)
      PrecMux->Length = length+gop_time;
      else
      PrecMux->Length = length;
    }else
    PrecMux->Length = length;
    return AMP_OK;
}

int  AppLibPrecMux_SetSpliteState(int splite_state)
{
    G_splite_state = splite_state;
    return AMP_OK;
}
int  AppLibPrecMux_getSpliteState(void)
{
    return G_splite_state;
}

void AppLibPrecMux_setprecend(int end)
{
    G_prec_end = end;
}

void AppLibPrecMux_fifo_clear(void)
{
    event_fifo_prec_flag = 0;
}

void AppLibPrecMux_master_set(AMP_MUX_FORMAT_HDLR_s *hdlr,UINT8 master)
{
    PREC_MUX_HDLR_s * PrecMux = (PREC_MUX_HDLR_s *)hdlr;
    PrecMux->master = master;
}


static AMP_MUX_FORMAT_s g_Pmux = {
    AppLibPrecMux_Open,
    AppLibPrecMux_Close,
    AppLibPrecMux_Process,
    AppLibPrecMux_GetMediaTime,
    AppLibPrecMux_CheckDataReady,
    AppLibPrecMux_Func
};

