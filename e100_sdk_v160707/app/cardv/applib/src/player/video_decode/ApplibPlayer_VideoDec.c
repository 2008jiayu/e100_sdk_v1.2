/**
 * @file src/app/connected/applib/src/player/video_decode/ApplibPlayer_VideoDec.c
 *
 * Implementation of MW video player utility
 *
 * History:
 *    2013/09/26 - [Martin Lai] created file
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

#include <player/video_decode/ApplibPlayer_VideoDec.h>
#include <applib.h>
#include <fifo/Fifo.h>
#include <player/Decode.h>
#include <player/VideoDec.h>
#include <comsvc/misc/util.h>
#include <AmbaUtility.h>
#include <format/ApplibFormat_DemuxMp4.h>
#include <format/Demuxer.h>
#include <player/decode_utility/ApplibPlayer_VideoTask.h>
#include "../../AppLibTask_Priority.h"

//#define DEBUG_APPLIB_VIDEO_DEC
#if defined(DEBUG_APPLIB_VIDEO_DEC)
#define DBGMSG AmbaPrint
#define DBGMSGc(x) AmbaPrintColor(GREEN,x)
#define DBGMSGc2 AmbaPrintColor
#else
#define DBGMSG(...)
#define DBGMSGc(...)
#define DBGMSGc2(...)
#endif

#include <format/Muxer.h>
#include <format/Mp4Mux.h>
#include <index/Index.h>
#include <index/Temp.h>
#include <stream/File.h>

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

static int ApplibVideoDecInitFlag = -1;
static int ApplibAudioDecInitFlag = -1;

// Video variables
#define VIDEODEC_RAW_SIZE           (20<<20) // Raw buffer size
#define VIDEODEC_RAW_DESC_NUM       (512) // Descriptor default number
#define APPLIB_PLAYER_NORMAL_SPEED  (0x100) // Normal speed
#if defined(CONFIG_APP_ARD)
#define APPLIB_PLAYER_MAX_SPEED     ((APPLIB_PLAYER_NORMAL_SPEED) << 2) // Maximum speed
#define APPLIB_PLAYER_MIN_SPEED     ((APPLIB_PLAYER_NORMAL_SPEED) >> 0) // Minimum speed
#else
#define APPLIB_PLAYER_MAX_SPEED     ((APPLIB_PLAYER_NORMAL_SPEED) << 6) // Maximum speed
#define APPLIB_PLAYER_MIN_SPEED     ((APPLIB_PLAYER_NORMAL_SPEED) >> 6) // Minimum speed
#endif
static void* AvcCodecBufOri = NULL;
static UINT8* AvcRawBuf = NULL;
static void* AvcRawBufOri = NULL;
static void* AvcDescBufOri = NULL;
static AMP_AVDEC_HDLR_s *AvcDecHdlr = NULL;
static AMP_DEC_PIPE_HDLR_s *DecPipeHdlr = NULL;

#define APPLIB_PLAYER_FILE_NUM_MAX      (128)       ///< At most 128 split files
#define APPLIB_PLAYER_INVALID_FILE_ID   (0xFF)      ///< A value bigger than APPLIB_PLAYER_FILE_NUM_MAX indicating invalid file ID
static APPLIB_VIDEO_PLAYER_STATE_e VideoPlayerState = APPLIB_VIDEO_PLAYER_STATE_INVALID;
//static UINT8 CurrentPlayingFileID = APPLIB_PLAYER_INVALID_FILE_ID;
static UINT8 CurrentFeedingFileID = APPLIB_PLAYER_INVALID_FILE_ID;
static UINT8 NextPlayingFileID = APPLIB_PLAYER_INVALID_FILE_ID;
static UINT8 NextFeedingFileID = APPLIB_PLAYER_INVALID_FILE_ID;
static UINT8 IsFileOpened = 0;
static char FileNameList[APPLIB_PLAYER_FILE_NUM_MAX][MAX_FILENAME_LENGTH] = { 0 }; ///< A list of filenames
static APPLIB_VIDEO_FILE_INFO_s FileInfoList[APPLIB_PLAYER_FILE_NUM_MAX] = { 0 }; ///< A list of file informations
static UINT8 FileNumber = 0; ///< How many files in FileList are valid
static UINT32 CurrentPlayingSpeed = APPLIB_PLAYER_NORMAL_SPEED;
static UINT32 PausedSpeed = APPLIB_PLAYER_NORMAL_SPEED; // The speed before the last pause action or start action with AutoPlay = 0.
static UINT64 PausedTime = 0; // The video time before the last pause action
static APPLIB_VIDEO_PLAY_DIRECTION_e CurrentPlayingDir = APPLIB_VIDEO_PLAY_FW; // Play forward
static AMP_AVDEC_TRICKPLAY_s Trick = { 0 };
static AMP_VIDEODEC_DISPLAY_s Display = { 0 };
static UINT32 VideoMagFactor = 100; // Normal size
static INT32 VideoShiftX = 0; // No shift on X-axis
static INT32 VideoShiftY = 0; // No shift on Y-axis

// Audio variables
#define AUDIODEC_RAW_SIZE (10<<20) // Raw buffer size
#define AUDIODEC_RAW_DESC_NUM (128) // Descriptor default number


static void* audCodecModuleBuf = NULL;
static UINT8* AudRawBuf = NULL;
static void* AudRawBufOri = NULL;
static void* AudDescBuf = NULL;
static void* AudCodecBuf = NULL;
static AMP_AVDEC_HDLR_s *AudDecHdlr = NULL;
static AMP_AUDIODEC_HDLR_CFG_s AudCodecCfg;

static UINT8 DecErrOccurredFlag = 0;


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
static int AppLibVideoDec_GetNextFileID(const UINT8 FileNum,
                                        const UINT8 CurrentFileID,
                                        const APPLIB_VIDEO_PLAY_DIRECTION_e PlayingDir,
                                        UINT8 *NextFileID)
{
    if (FileNum == 0) { // No file
        *NextFileID = APPLIB_PLAYER_INVALID_FILE_ID; // No next file
        return -1; // Error
    }

    if (CurrentFileID == APPLIB_PLAYER_INVALID_FILE_ID) {
        return -1; // Error
    }

    if (CurrentFileID >= FileNum) { // CurrentFileID is too large
        return -1; // Error
    }

    if (PlayingDir == APPLIB_VIDEO_PLAY_FW) {
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
    return 0; // Success
}

/**
 * Determine whether to feed the next file.
 *
 * @return 0 - Cannot feed, Others - Can feed
 */
static UINT8 AppLibVideoDec_CanFeedNextFile(void)
{
    if (NextFeedingFileID == APPLIB_PLAYER_INVALID_FILE_ID) { // There's no file to feed
        return 0; // Cannot feed
    }

    return 1; // Can feed

// TODO: Determine by CurrentFeedingFileID and CurrentPlayingFileID if the feeding needs to be controled
//    if (CurrentFeedingFileID == APPLIB_PLAYER_INVALID_FILE_ID) { // There's no file feeding for now
//        return 1; // Can feed
//    }
//
//    // The maximum distance between CurrentFeedingFileID and CurrentPlayingFileID is (1)
//    if (CurrentFeedingFileID == CurrentPlayingFileID) {
//        return 1; // Can feed
//    }
//    else {
//        return 0; // Cannot feed
//    }
}

UINT8 AppLibVideoDec_PlayEOS(void)
{
    if (AppLibVideoDec_CanFeedNextFile() == 0) {
        CurrentFeedingFileID = APPLIB_PLAYER_INVALID_FILE_ID;
    }

      return 0;
}

/**
 * Event handler of demux callback.
 *
 * @param [in] hdlr             Pointer to handler.
 * @param [in] event            Evnet ID.
 * @param [in] hdlr             Event info.
 *
 * @return 0 - Success, Others - Failure
 */
static int AppLibVideoDec_DemuxCallback(void *hdlr,
                                        UINT32 event,
                                        void* info)
{
    APPLIB_VIDEO_TASK_MSG_s VideoMsg;
    int Rval = 0;

    switch (event) {
    case AMP_DEMUXER_EVENT_END:
        AmbaPrint("[Applib - Format] DemuxCallBack event AMP_DEMUXER_EVENT_END");
        // Send message to video task
        // Initialize message
        SET_ZERO(VideoMsg);
        // Configure video message
        VideoMsg.MessageType = APPLIB_VIDEO_TASK_MSG_FEED_END;
        VideoMsg.AvcDecHdlr = hdlr;
        Rval = AppLibVideoTask_SendVideoMsg(&VideoMsg, AMBA_KAL_WAIT_FOREVER);
        if (Rval != 0) {
            AmbaPrint("[Applib - VideoDec] <DecCallback> %s:%u Failed to send message (%d)!", __FUNCTION__, __LINE__, Rval);
            return -1; // Error
        }
        break;
    default:
        // Do nothing
        break;
    }

    return 0;
}

int AppLibVideoDec_CalStartTime(UINT64 VideoTime,UINT32 *StartTime)
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
int AppLibVideoDec_FeedNextFile(UINT32 StartTime)
{
    int ReturnValue = 0;

    if (AppLibVideoDec_CanFeedNextFile() && CurrentFeedingFileID != NextFeedingFileID) {

        UINT8 FileID;
        UINT8 IsFeedEos; // Feed EOS to DSP at the end of the file
        // Stop demuxer and decoder if the file is opened.
        if (IsFileOpened) {
            // Stop demuxer
            ReturnValue = AppLibFormatDemuxMp4_Close(0); // Do not erase fifo
            if (ReturnValue != 0) {
                AmbaPrintColor(RED, "[Applib - VideoDec] %s:%u Failed to stop mp4 demuxer.", __FUNCTION__, __LINE__);
                return ReturnValue;
            }
            IsFileOpened = 0;
        }

        {
            //char Ft[APPLIB_PLAYER_FILE_NUM_MAX] = { 0 };
            //AmbaUtility_Unicode2Ascii(FileNameList[NextPlayingFileID], Ft);
            AmbaPrint("[Applib - VideoDec] Open file = %s", FileNameList[NextPlayingFileID]);
        }
        AppLibFormatDemuxMp4_SetCodecHdlrInfo(AvcDecHdlr, AudDecHdlr, AvcRawBuf, VIDEODEC_RAW_SIZE, AudRawBuf,
                AUDIODEC_RAW_SIZE);

        if (AppLibVideoDec_GetNextFileID(FileNumber, NextFeedingFileID, CurrentPlayingDir, &FileID) != 0) {
            AmbaPrintColor(RED, "[Applib - VideoDec] %s:%u Failed to get next file ID.", __FUNCTION__, __LINE__);
            return -1; // Error
        }
        IsFeedEos = (FileID == APPLIB_PLAYER_INVALID_FILE_ID) ? (1) :(0);

        // Open a file and start demuxing
        ReturnValue = AppLibFormatDemuxMp4_Open(
                FileNameList[NextFeedingFileID],
                StartTime, // Start time of current file. (TimeOffset is not included)
                APPLIB_TO_DEMUX_SPEED(CurrentPlayingSpeed), // Convert decode speed to demuxer speed
                0, // Do not erase fifo
                FileInfoList[NextFeedingFileID].InitTime,
                IsFeedEos,
                AppLibVideoDec_DemuxCallback);
        if (ReturnValue != 0) {
            AmbaPrintColor(RED, "[Applib - VideoDec] %s:%u Failed to open and start a demuxer.", __FUNCTION__, __LINE__);
            return -1; // Error
        }
        IsFileOpened = 1;

        CurrentFeedingFileID = NextFeedingFileID;
        NextFeedingFileID = FileID;
    }

    return 0;
}

static int AppLibVideoDec_VideoDecCallBack(void *hdlr,
                                           UINT32 event,
                                           void* info)
{
    APPLIB_VIDEO_TASK_MSG_s VideoMsg;
    UINT32 ErrorCode = 0;
    int Rval = 0;

    switch (event) {
    case AMP_DEC_EVENT_ERROR:
        AmbaPrint("[Applib - VideoDec] <DecCallback> AMP_DEC_EVENT_ERROR!");
        break;
    case AMP_DEC_EVENT_FIRST_FRAME_DISPLAYED:
        AmbaPrint("[Applib - VideoDec] <DecCallback> AMP_DEC_EVENT_FIRST_FRAME_DISPLAYED");
        break;
    case AMP_DEC_EVENT_PLAYBACK_EOS:
#ifdef CONFIG_APP_ARD
        if (CurrentFeedingFileID == APPLIB_PLAYER_INVALID_FILE_ID) {
            AmbaPrintColor(RED,"AMP_DEC_EVENT_PLAYBACK_EOS:INVALID_FILE_ID");
            break;
         }
#endif

        AmbaPrint("[Applib - VideoDec] <DecCallback> AMP_DEC_EVENT_PLAYBACK_EOS");
        // Send EOS message to video task
        // Initialize message
        SET_ZERO(VideoMsg);
        // Configure video message
        VideoMsg.MessageType = APPLIB_VIDEO_TASK_MSG_EOS;
        VideoMsg.AvcDecHdlr = hdlr;
        Rval = AppLibVideoTask_SendVideoMsg(&VideoMsg, AMBA_KAL_WAIT_FOREVER);
        if (Rval != 0) {
            AmbaPrint("[Applib - VideoDec] <DecCallback> %s:%u Failed to send message (%d)!", __FUNCTION__, __LINE__, Rval);
            return -1; // Error
        }
        break;
    case AMP_DEC_EVENT_STATE_CHANGED:
        AmbaPrint("[Applib - VideoDec] <DecCallback> AMP_DEC_EVENT_STATE_CHANGED info: %x", info);
        break;
    case AMP_DEC_EVENT_DATA_UNDERTHRSHOLDD:
        //AmbaPrint("[Applib - VideoDec] <DecCallback>  AMP_DEC_EVENT_DATA_UNDERTHRSHOLDD!");
        if (AppLibFormatDemuxMp4_CanRequestData()) {
            if (AppLibFormatDemuxMp4_DemuxOnDataRequest() != AMP_OK) {
                AmbaPrintColor(GREEN, "[Applib - VideoDec] <DecCallback> %s:%u Failed to request data", __FUNCTION__, __LINE__);
            }
        }
        break;
    // case AMP_DEC_EVENT_DECODE_ERROR:
    //     if (DecErrOccurredFlag == 0) {
    //         DecErrOccurredFlag = 1;
    //         ErrorCode = *((UINT32 *)info);
    //         AmbaPrint("[Applib - VideoDec] <DecCallback> AMP_DEC_EVENT_DECODE_ERROR ErrorCode: %x", ErrorCode);
    //         SET_ZERO(VideoMsg);
    //         // Configure video message
    //         VideoMsg.MessageType = APPLIB_VIDEO_TASK_MSG_DEC_ERROR;
    //         VideoMsg.AvcDecHdlr = hdlr;
    //         Rval = AppLibVideoTask_SendVideoMsg(&VideoMsg, AMBA_KAL_WAIT_FOREVER);
    //         if (Rval != 0) {
    //             AmbaPrint("[Applib - VideoDec] <DecCallback> %s:%u Failed to send message (%d)!", __FUNCTION__, __LINE__, Rval);
    //             return -1; // Error
    //         }
    //     }
    //     break;

    default:
        AmbaPrint("[Applib - VideoDec] <DecCallback> Unknown event %X info: %x", event, info);
        break;
    }

    return 0;
}

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
#ifdef CONFIG_APP_ARD
    IOInfo.MaxDmaDescNum = 32;
#else
    IOInfo.MaxDmaDescNum = 16;
#endif
    IOInfo.MaxDmaSize = 256;
    IOInfo.MaxSampleFreq = 48000;
    sizeOutCacheCtrl = AmbaAudio_OutputCachedSizeQuery(&IOInfo);
    sizeOutNCCtrl = AmbaAudio_OutputNonCachedSizeQuery(&IOInfo);

    if (0) { // TBD somehow it wont work FIX IT!!!!!!!
        if (AmpUtil_GetAlignedPool(APPLIB_G_MMPL, &(OutputCached.pMemAlignedBase), (void**) &audCtrlCacheBufOri, ALIGN_4(sizeOutCacheCtrl), 1 << 6) != AMP_OK) {
            AmbaPrint("[Applib - VideoDec] %s:%u Failed to allocate memory.", __FUNCTION__, __LINE__);
            return NULL;
        }

        // NOTE: The buffer of "OutputNonCached" must be allocated from a NON-CACHED buffer.
        if (AmpUtil_GetAlignedPool(APPLIB_G_NC_MMPL, &(OutputNonCached.pMemAlignedBase), (void**) &audCtrlNCacheBufOri, ALIGN_4(sizeOutNCCtrl), 1 << 6) != AMP_OK) {
            AmbaPrint("[Applib - VideoDec] %s:%u Failed to allocate memory.", __FUNCTION__, __LINE__);
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
#ifdef CONFIG_APP_ARD
    AppLibAudio_EffectVolumeInstall(APPLIB_AUDIO_EFFECT_OUTPUT,audOutCtrl);
#endif

    return audOutCtrl;
}

static int AudioDec_CodecCB(void* hdlr,
                            UINT32 event,
                            void* info)
{
#ifdef CONFIG_APP_ARD
    switch (event) {
    case AMP_DEC_EVENT_ERROR:
        AmbaPrint("[Applib - AudioDec] <DecCallback> AMP_DEC_EVENT_ERROR!");
        break;
    case AMP_DEC_EVENT_FIRST_FRAME_DISPLAYED:
        AmbaPrint("[Applib - AudioDec] <DecCallback> AMP_DEC_EVENT_FIRST_FRAME_DISPLAYED");
        break;
    case AMP_DEC_EVENT_PLAYBACK_EOS:
        AmbaPrint("[Applib - AudioDec] <DecCallback> AMP_DEC_EVENT_PLAYBACK_EOS");
        // Send EOS message to App
        if(AppLibAudioDec_GetPlayerState() == APPLIB_AUDIO_PLAYER_STATE_PLAY) {
            if (AppLibComSvcHcmgr_SendMsgNoWait(HMSG_PLAYER_PLY_EOS, 0, 0) != AMP_OK) {
                AmbaPrintColor(RED,"[Applib - VideoDec] <DecCallback> %s:%u Failed to send message!", __FUNCTION__, __LINE__);
            }
        }
        break;
    case AMP_DEC_EVENT_STATE_CHANGED:
        AmbaPrint("[Applib - AudioDec] <DecCallback> AMP_DEC_EVENT_STATE_CHANGED info: %x", info);
        break;
    case AMP_DEC_EVENT_DATA_UNDERTHRSHOLDD:
        AmbaPrint("[Applib - AudioDec] <DecCallback>  AMP_DEC_EVENT_DATA_UNDERTHRSHOLDD!");
        break;
    default:
        AmbaPrint("[Applib - AudioDec] <DecCallback> Unknown event %X info: %x", event, info);
        break;
    }
#endif
    return 0;
}

int AppLibVideoDec_Init(void)
{
    int ReturnValue = -1;

    AmbaPrint("[Applib - VideoDec] <Init> start");

    // Check state
    if (VideoPlayerState != APPLIB_VIDEO_PLAYER_STATE_INVALID) {
        AmbaPrint("[Applib - VideoDec] %s:%u Already initialized.", __FUNCTION__, __LINE__);
        goto ReturnSuccess;
    }

    // Initialize decode task
    if (AppLibVideoTask_IsTaskInitialized() == 0) {
        AppLibVideoTask_InitTask();
    }

    // Initialize video codec module
    if (ApplibVideoDecInitFlag < 0) {
        AMP_VIDEODEC_INIT_CFG_s CodecInitCfg;
        ReturnValue = AmpVideoDec_GetInitDefaultCfg(&CodecInitCfg);
        if (ReturnValue != AMP_OK) {
            AmbaPrintColor(RED, "[Applib - VideoDec] %s:%u Failed to get default video decoder config (%d).", __FUNCTION__, __LINE__, ReturnValue);
            goto ReturnError;
        }
        if (AvcCodecBufOri != NULL) {
            // Print warning. Memory is not cleaned up.
            AmbaPrintColor(RED, "[Applib - VideoDec] %s:%u Free memory at 0x%08x.", __FUNCTION__, __LINE__, AvcCodecBufOri);
            AmbaKAL_BytePoolFree(AvcCodecBufOri);
            AvcCodecBufOri = NULL;
        }
        ReturnValue = AmpUtil_GetAlignedPool(APPLIB_G_MMPL, (void**) &(CodecInitCfg.Buf), &AvcCodecBufOri, CodecInitCfg.BufSize, 1 << 5);
        if (ReturnValue != AMP_OK) {
            AmbaPrintColor(RED, "[Applib - VideoDec] %s:%u Failed to allocate memory (%d).", __FUNCTION__, __LINE__, ReturnValue);
            goto ReturnError;
        }
        CodecInitCfg.TaskInfo.Priority = APPLIB_VIDEO_DEC_TASK_PRIORITY;
        ReturnValue = AmpVideoDec_Init(&CodecInitCfg);
        if (ReturnValue != AMP_OK) {
            AmbaPrintColor(RED, "[Applib - VideoDec] %s:%u Failed to initialize video decoder (%d).", __FUNCTION__, __LINE__, ReturnValue);
            goto ReturnError;
        }
        ApplibVideoDecInitFlag = 0;
    }

    if (ApplibAudioDecInitFlag < 0) {
        AMP_AUDIODEC_INIT_CFG_s CodecInitCfg;
        // Initialize system output
        ReturnValue = AmpAudio_OutputInit(CreateAudioOutputCtrl(), APPLIB_AUDIO_OUTPUT_TASK_PRIORITY);
        if (ReturnValue != AMP_OK) {
            AmbaPrintColor(RED, "[Applib - VideoDec] %s:%u Failed to initialize system output (%d).", __FUNCTION__, __LINE__, ReturnValue);
            goto ReturnError;
        }
        // Initialize codec module
        ReturnValue = AmpAudioDec_GetInitDefaultCfg(&CodecInitCfg);
        if (ReturnValue != AMP_OK) {
            AmbaPrintColor(RED, "[Applib - VideoDec] %s:%u Failed to get default audio decoder config (%d).", __FUNCTION__, __LINE__, ReturnValue);
            goto ReturnError;
        }

        if (audCodecModuleBuf != NULL) {
            // Print warnning. Memory is not cleaned up.
            AmbaPrintColor(RED, "[Applib - VideoDec] %s:%u Free memory at 0x%08x.", __FUNCTION__, __LINE__, audCodecModuleBuf);
            AmbaKAL_BytePoolFree(audCodecModuleBuf);
            audCodecModuleBuf = NULL;
        }
        ReturnValue = AmpUtil_GetAlignedPool(APPLIB_G_MMPL, (void**) &(CodecInitCfg.WorkBuff), &audCodecModuleBuf, CodecInitCfg.WorkBuffSize, 1 << 5);
        if (ReturnValue != AMP_OK) {
            AmbaPrintColor(RED, "[Applib - VideoDec] %s:%u Failed to allocate memory (%d).", __FUNCTION__, __LINE__, ReturnValue);
            goto ReturnError;
        }
        CodecInitCfg.TaskInfo.Priority = APPLIB_AUDIO_DEC_TASK_PRIORITY;
        ReturnValue = AmpAudioDec_Init(&CodecInitCfg);
        if (ReturnValue != AMP_OK) {
            AmbaPrintColor(RED, "[Applib - VideoDec] %s:%u Failed to initialize audio decoder (%d).", __FUNCTION__, __LINE__, ReturnValue);
            goto ReturnError;
        }
        ApplibAudioDecInitFlag = 0;
    }

    // Initialize audio codec
    // FIXIT WAIT demux fix
    if (AudDecHdlr == NULL)
    {
        //AMP_DEC_PIPE_CFG_s pipeCfg;

        AMBA_AUDIO_TASK_CREATE_INFO_s DecInfo;
        AMBA_ABU_CREATE_INFO_s AbuInfo;
        UINT32 DecSize, AbuSize;

        // Create audio codec hdlr
        // Get default config
        ReturnValue = AmpAudioDec_GetDefaultCfg(&AudCodecCfg);
        if (ReturnValue != AMP_OK) {
            AmbaPrintColor(RED, "[Applib - VideoDec] %s:%u Failed to get default audio codec handler config (%d).", __FUNCTION__, __LINE__, ReturnValue);
            goto ReturnError;
        }
        // Allocate memory for raw buffer
        if (AudRawBuf != NULL) {
            // Print warnning. Memory is not cleaned up.
            AmbaPrintColor(RED, "[Applib - VideoDec] %s:%u Free memory at 0x%08x.", __FUNCTION__, __LINE__, AudRawBuf);
            //AmbaKAL_BytePoolFree(AudRawBufOri);
            AudRawBuf = NULL;
        }
        /*ReturnValue = AmpUtil_GetAlignedPool(APPLIB_G_MMPL, &AudRawBuf, &AudRawBufOri, AUDIODEC_RAW_SIZE, 1 << 6);
        if (ReturnValue != AMP_OK) {
            AmbaPrintColor(RED, "[Applib - VideoDec] %s:%u Failed to allocate memory (%d).", __FUNCTION__, __LINE__, ReturnValue);
            goto ReturnError;
        }*/
        AppLibComSvcMemMgr_AllocateDSPMemory(&AudRawBuf, AUDIODEC_RAW_SIZE);
        AudCodecCfg.RawBuffer = (UINT8*) AudRawBuf;
        AudCodecCfg.RawBufferSize = AUDIODEC_RAW_SIZE;

        // Allocate memory for descriptor buffer
        if (AudDescBuf != NULL) {
            // Print warnning. Memory is not cleaned up.
            AmbaPrintColor(RED, "[Applib - VideoDec] %s:%u Free memory at 0x%08x.", __FUNCTION__, __LINE__, AudDescBuf);
            AmbaKAL_BytePoolFree(AudDescBuf);
            AudDescBuf = NULL;
        }
        ReturnValue = AmpUtil_GetAlignedPool(
                        APPLIB_G_MMPL,
                        (void**) &(AudCodecCfg.DescBuffer),
                        &AudDescBuf,
                        AUDIODEC_RAW_DESC_NUM * sizeof(AMP_BITS_DESC_s),
                        1 << 5);
        if (ReturnValue != AMP_OK) {
            AmbaPrintColor(RED, "[Applib - VideoDec] %s:%u Failed to allocate memory (%d).", __FUNCTION__, __LINE__, ReturnValue);
            goto ReturnError;
        }
        AudCodecCfg.DescBufferNum = AUDIODEC_RAW_DESC_NUM;

        AudCodecCfg.CbEvent = AudioDec_CodecCB;

        AudCodecCfg.PureAudio = VIDEO_AUDIO;

        AudCodecCfg.MaxChannelNum = 2;
        AudCodecCfg.MaxFrameSize = 4096;
        AudCodecCfg.MaxSampleRate = 48000;
        AudCodecCfg.MaxChunkNum = 16;
        AudCodecCfg.I2SIndex = 0;

        if (1) { //AAC FIXIT should from demuxer
            AudCodecCfg.DecType = AMBA_AUDIO_AAC;
            AudCodecCfg.DstSampleRate = 48000;
            AudCodecCfg.SrcSampleRate = 48000;
            AudCodecCfg.DstChannelMode = 2;
            AudCodecCfg.SrcChannelMode = 2;
            AudCodecCfg.FadeInTime = 0;
            AudCodecCfg.FadeOutTime = 0;

            AudCodecCfg.Spec.AACCfg.BitstreamType = AAC_BS_RAW;
        }

        DecInfo.MaxSampleFreq = AudCodecCfg.MaxSampleRate;
        DecInfo.MaxChNum = AudCodecCfg.MaxChannelNum;
        DecInfo.MaxFrameSize = AudCodecCfg.MaxFrameSize;
        DecSize = AmbaAudio_DecSizeQuery(&DecInfo);

        AbuInfo.MaxSampleFreq = AudCodecCfg.MaxSampleRate;
        AbuInfo.MaxChNum = AudCodecCfg.MaxChannelNum;
        AbuInfo.MaxChunkNum = AudCodecCfg.MaxChunkNum;
        AbuSize = AmbaAudio_BufferSizeQuery(&AbuInfo);

        if (AudCodecBuf != NULL) {
            // Print warnning. Memory is not cleaned up.
            AmbaPrintColor(RED, "[Applib - VideoDec] %s:%u Free memory at 0x%08x.", __FUNCTION__, __LINE__, AudCodecBuf);
            AmbaKAL_BytePoolFree(AudCodecBuf);
            AudCodecBuf = NULL;
        }
        ReturnValue = AmpUtil_GetAlignedPool(
                        APPLIB_G_MMPL,
                        (void**) &(AudCodecCfg.CodecCacheWorkBuff),
                        &AudCodecBuf,
                        DecSize + AbuSize,
                        1 << 6);
        if (ReturnValue != AMP_OK) {
            AmbaPrintColor(RED, "[Applib - VideoDec] %s:%u Failed to allocate memory (%d).", __FUNCTION__, __LINE__, ReturnValue);
            goto ReturnError;
        }
        AudCodecCfg.CodecCacheWorkSize = DecSize + AbuSize;
        AmbaPrint("audio codec buffer : 0x%x, %d", AudCodecCfg.CodecCacheWorkBuff, AudCodecCfg.CodecCacheWorkSize);

        // Create audio codec handler
        ReturnValue = AmpAudioDec_Create(&AudCodecCfg, &AudDecHdlr);
        if (ReturnValue != AMP_OK) {
            AmbaPrintColor(RED, "[Applib - VideoDec] %s:%u Failed to create audio codec handler (%d).", __FUNCTION__, __LINE__, ReturnValue);
            goto ReturnError;
        }
    }

    // Create video codec handler
    if (AvcDecHdlr == NULL) {
        AMP_VIDEODEC_CFG_s CodecCfg;
        // Get default config
        ReturnValue = AmpVideoDec_GetDefaultCfg(&CodecCfg);
        if (ReturnValue != AMP_OK) {
            AmbaPrintColor(RED, "[Applib - VideoDec] %s:%u Failed to get default video codec handler config (%d).", __FUNCTION__, __LINE__, ReturnValue);
            goto ReturnError;
        }
        // Allocate memory for raw buffer
        if (AvcRawBuf != NULL) {
            // Print warnning. Memory is not cleaned up.
            AmbaPrintColor(RED, "[Applib - VideoDec] %s:%u Free memory at 0x%08x.", __FUNCTION__, __LINE__, AvcRawBuf);
            //AmbaKAL_BytePoolFree(AvcRawBufOri);
            AvcRawBuf = NULL;
        }
        /*ReturnValue = AmpUtil_GetAlignedPool(APPLIB_G_MMPL, &AvcRawBuf, &AvcRawBufOri, VIDEODEC_RAW_SIZE, 1 << 6);
        if (ReturnValue != AMP_OK) {
            AmbaPrintColor(RED, "[Applib - VideoDec] %s:%u Failed to allocate memory (%d).", __FUNCTION__, __LINE__, ReturnValue);
            goto ReturnError;
        }*/
        AppLibComSvcMemMgr_AllocateDSPMemory(&AvcRawBuf, VIDEODEC_RAW_SIZE);
        CodecCfg.RawBuffer = (char*) AvcRawBuf;
        AmbaPrint("%x -> %x", AvcRawBuf, CodecCfg.RawBuffer);
        CodecCfg.RawBufferSize = VIDEODEC_RAW_SIZE;

        // Allocate memory for descriptor buffer
        if (AvcDescBufOri != NULL) {
            // Print warnning. Memory is not cleaned up.
            AmbaPrintColor(RED, "[Applib - VideoDec] %s:%u Free memory at 0x%08x.", __FUNCTION__, __LINE__, AvcDescBufOri);
            AmbaKAL_BytePoolFree(AvcDescBufOri);
            AvcDescBufOri = NULL;
        }
        ReturnValue = AmpUtil_GetAlignedPool(
                        APPLIB_G_MMPL,
                        (void**) &(CodecCfg.DescBuffer),
                        &AvcDescBufOri,
                        VIDEODEC_RAW_DESC_NUM * sizeof(AMP_BITS_DESC_s),
                        1 << 5);
        if (ReturnValue != AMP_OK) {
            AmbaPrintColor(RED, "[Applib - VideoDec] %s:%u Failed to allocate memory (%d).", __FUNCTION__, __LINE__, ReturnValue);
            goto ReturnError;
        }
        CodecCfg.NumDescBuffer = VIDEODEC_RAW_DESC_NUM;
        CodecCfg.CbCodecEvent = AppLibVideoDec_VideoDecCallBack;

        // Feature config
        CodecCfg.Feature.MaxVoutWidth = 3840;
        CodecCfg.Feature.MaxVoutHeight = 2160;

        // Create video codec handler
        ReturnValue = AmpVideoDec_Create(&CodecCfg, &AvcDecHdlr);
        if (ReturnValue != AMP_OK) {
            AmbaPrintColor(RED, "[Applib - VideoDec] %s:%u Failed to create video codec handler (%d).", __FUNCTION__, __LINE__, ReturnValue);
            goto ReturnError;
        }
    }

    // Create decoder manager
    if (DecPipeHdlr == NULL) {
        AMP_DEC_PIPE_CFG_s PipeCfg;
        // Get default config
        ReturnValue = AmpDec_GetDefaultCfg(&PipeCfg);
        if (ReturnValue != AMP_OK) {
            AmbaPrintColor(RED, "[Applib - VideoDec] %s:%u Failed to get default decoder manager config (%d).", __FUNCTION__, __LINE__, ReturnValue);
            goto ReturnError;
        }
        PipeCfg.Decoder[0] = AvcDecHdlr;
        // FIXIT WAIT demux fix
        PipeCfg.Decoder[1] = AudDecHdlr;
        PipeCfg.NumDecoder = 2;
        PipeCfg.Type = AMP_DEC_VID_PIPE;
        ReturnValue = AmpDec_Create(&PipeCfg, &DecPipeHdlr);
        if (ReturnValue != AMP_OK) {
            AmbaPrintColor(RED, "[Applib - VideoDec] %s:%u Failed to create decoder manager (%d).", __FUNCTION__, __LINE__, ReturnValue);
            goto ReturnError;
        }
    }

    // Active pipe
    ReturnValue = AmpDec_Add(DecPipeHdlr);
    if (ReturnValue != AMP_OK) {
        AmbaPrintColor(RED, "[Applib - VideoDec] %s:%u Failed to activate decoder manager (%d).", __FUNCTION__, __LINE__, ReturnValue);
        goto ReturnError;
    }
#ifdef CONFIG_APP_ARD
    AppLibAudioDec_SetOutputVolume();
#endif
    // Initialize video settings
    VideoPlayerState = APPLIB_VIDEO_PLAYER_STATE_IDLE;
    FileNumber = 0;
    //CurrentPlayingFileID = APPLIB_PLAYER_INVALID_FILE_ID;
    CurrentFeedingFileID = APPLIB_PLAYER_INVALID_FILE_ID;
    NextPlayingFileID = APPLIB_PLAYER_INVALID_FILE_ID;
    NextFeedingFileID = APPLIB_PLAYER_INVALID_FILE_ID;
    CurrentPlayingSpeed = APPLIB_PLAYER_NORMAL_SPEED;
    PausedSpeed = APPLIB_PLAYER_NORMAL_SPEED;
    CurrentPlayingDir = APPLIB_VIDEO_PLAY_FW; // Play forward
    SET_ZERO(Trick);
    SET_ZERO(Display);
    VideoMagFactor = 100; // Normal size
    VideoShiftX = 0; // No shift on X-axis
    VideoShiftY = 0; // No shift on Y-axis

ReturnSuccess:
    AmbaPrint("[Applib - VideoDec] <Init> end");
    return AMP_OK;

ReturnError:
    AppLibVideoDec_Exit();

    AmbaPrint("[Applib - VideoDec] <Init> end");
    return -1;
}

int AppLibVideoDec_GetStartDefaultCfg(APPLIB_VIDEO_START_INFO_s* OutputVideoStartInfo)
{
    if (OutputVideoStartInfo == NULL) {
        AmbaPrintColor(GREEN, "[Applib - VideoDec] %s:%u OutputVideoStartInfo is NULL", __FUNCTION__, __LINE__);
        return -1;
    }
    OutputVideoStartInfo->Filename = NULL;
    OutputVideoStartInfo->AutoPlay = 1; // Open and play
    OutputVideoStartInfo->StartTime = 0; // Start at the beginning
    OutputVideoStartInfo->Direction = APPLIB_VIDEO_PLAY_FW; // Play forward
    OutputVideoStartInfo->ResetSpeed = 1; // Reset to 1x speed
    OutputVideoStartInfo->ResetZoom = 1; // Reset to original size
    return 0; // Success
}

int AppLibVideoDec_GetMultiStartDefaultCfg(APPLIB_VIDEO_START_MULTI_INFO_s* OutputVideoStartInfo)
{
    if (OutputVideoStartInfo == NULL) {
        AmbaPrintColor(GREEN, "[Applib - VideoDec] %s:%u OutputVideoStartInfo is NULL", __FUNCTION__, __LINE__);
        return -1;
    }
    OutputVideoStartInfo->File = NULL;
    OutputVideoStartInfo->FileNum = 0;
    OutputVideoStartInfo->AutoPlay = 1; // Open and play
    OutputVideoStartInfo->StartTime = 0; // Start at the beginning
    OutputVideoStartInfo->Direction = APPLIB_VIDEO_PLAY_FW; // Play forward
    OutputVideoStartInfo->ReloadFile = 1; // Play video specified in "File"
    OutputVideoStartInfo->ResetSpeed = 1; // Reset to 1x speed
    OutputVideoStartInfo->ResetZoom = 1; // Reset to original size
    return 0; // Success
}

int AppLibVideoDec_GetMultiFileInfo(APPLIB_VIDEO_START_MULTI_INFO_s* VideoStartInfo)
{
    APPLIB_MEDIA_INFO_s MediaInfo; // Medio info of the video
    int i = 0;
    int Rval = 0; // Function call return value

    if (VideoStartInfo == NULL) {
        AmbaPrintColor(GREEN, "[Applib - VideoDec] %s:%u OutputVideoStartInfo is NULL", __FUNCTION__, __LINE__);
        return -1;
    }
    if (VideoStartInfo->File == NULL || VideoStartInfo->FileNum == 0 || VideoStartInfo->ReloadFile == 0) {
        AmbaPrint("[Applib - VideoDec] %s:%u No file.", __FUNCTION__, __LINE__);
        return 0;
    }

    // Set the EOS time before playing the clip
    for (i = 0; i < VideoStartInfo->FileNum; ++i) {
        UINT32 TimePerFrame = 0;
        UINT32 TimeScale = 0;
        UINT64 DTS = 0;
        Rval = AppLibFormat_GetMediaInfo(VideoStartInfo->File[i].Filename, &MediaInfo);
        if (Rval != AMP_OK) {
            AmbaPrintColor(RED, "[Applib - VideoDec] %s:%u Failed to get media info", __FUNCTION__, __LINE__);
            return -1; // Error
        }
        if (MediaInfo.MediaInfoType != AMP_MEDIA_INFO_MOVIE) {
            AmbaPrintColor(RED, "[Applib - VideoDec] %s:%u Invalid media type (%d)", __FUNCTION__, __LINE__, MediaInfo.MediaInfoType);
            return -1; // Error
        }
        if (MediaInfo.MediaInfo.Movie->Track[0].NextDTS == 0xFFFFFFFFFFFFFFFF) {
            AmbaPrintColor(RED, "[Applib - VideoDec] %s:%u DTS is not valid", __FUNCTION__, __LINE__);
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

static int AppLibVideoDec_CalculateDisplayArea(
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

int AppLibVideoDec_StartMultiple(const APPLIB_VIDEO_START_MULTI_INFO_s* VideoStartInfo)
{
    int ReturnValue = 0;
    UINT32 VideoStartTime = 0;
    UINT32 i = 0;

    // Preliminary check
    if (VideoStartInfo == NULL) {
        AmbaPrintColor(RED, "[Applib - VideoDec] %s:%u VideoStartInfo is NULL", __FUNCTION__, __LINE__);
        return -1;
    }
    if (VideoStartInfo->ReloadFile) {
        if (VideoStartInfo->File == NULL) {
            AmbaPrintColor(RED, "[Applib - VideoDec] %s:%u Filename is NULL", __FUNCTION__, __LINE__);
            return -1;
        }
        if (VideoStartInfo->FileNum == 0) {
            AmbaPrintColor(RED, "[Applib - VideoDec] %s:%u No file", __FUNCTION__, __LINE__);
            return -1;
        }
    }
    // Check state
    if (VideoPlayerState == APPLIB_VIDEO_PLAYER_STATE_INVALID) {
        AmbaPrint("[Applib - VideoDec] %s:%u Initialize first.", __FUNCTION__, __LINE__);
        return -1;
    }
    if (DecPipeHdlr == NULL) {
        AmbaPrint("[Applib - VideoDec] %s:%u DecPipeHdlr is NULL.", __FUNCTION__, __LINE__);
        return -1;
    }

    // Check reset speed. Do this before AutoPlay checking.
    if (VideoStartInfo->ResetSpeed) {
        CurrentPlayingSpeed = APPLIB_PLAYER_NORMAL_SPEED;
        PausedSpeed = APPLIB_PLAYER_NORMAL_SPEED;
    }

    // Check AutoPlay
    if (VideoStartInfo->AutoPlay) {
        AmbaPrintColor(GREEN, "[Applib - VideoDec] open play %s:%u", __FUNCTION__, __LINE__);
        // Resume if it's paused.
        if (CurrentPlayingSpeed == 0) {
            CurrentPlayingSpeed = PausedSpeed;
        }
    }

    // Configure direction
    if (VideoStartInfo->Direction == APPLIB_VIDEO_PLAY_FW) {
        CurrentPlayingDir = APPLIB_VIDEO_PLAY_FW; // Play forward
    } else {
        CurrentPlayingDir = APPLIB_VIDEO_PLAY_BW; // Play backward
    }

    // Copy file informations
    if (VideoStartInfo->ReloadFile) {
        FileNumber = VideoStartInfo->FileNum;
        for (i = 0; i < FileNumber; ++i) {
            memcpy(FileNameList[i], VideoStartInfo->File[i].Filename, sizeof(FileNameList[i]));
            memcpy(&FileInfoList[i], &VideoStartInfo->File[i], sizeof(FileInfoList[i]));
            FileInfoList[i].Filename = FileNameList[i];
        }
    }


    // Stop decoder if the file is opened.
    if (IsFileOpened) {
        /**stop demuxer*/
        ReturnValue = AppLibFormatDemuxMp4_Stop();
        // Stop decoder
        ReturnValue = AmpDec_StopWithLastFrm(DecPipeHdlr);
        if (ReturnValue != AMP_OK) {
            AmbaPrintColor(RED, "[Applib - VideoDec] %s:%u Failed to stop video codec manager.", __FUNCTION__, __LINE__);
            return ReturnValue;
        }
    }
    AppLibVideoDec_CalStartTime(VideoStartInfo->StartTime,&VideoStartTime);

    /**Check if need open next file, if need close current file and open next*/
    if (AppLibVideoDec_FeedNextFile(VideoStartTime) != 0) {
        return -1; // Error
    }

    // Zooming
    {
        UINT32 MovieWidth = 0, MovieHeight = 0;
        AppLibFormatDemuxMp4_GetMovieSize(&MovieWidth, &MovieHeight);
        AmbaPrintColor(GREEN, "MovieWidth = %d, MovieHeight = %d", MovieWidth, MovieHeight);
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
        if (AppLibVideoDec_CalculateDisplayArea(&Display, VideoMagFactor, VideoShiftX, VideoShiftY) != 0) {
            AmbaPrint("[Applib - VideoDec] %s:%u Failed to calculate display area.", __FUNCTION__, __LINE__);
            return -1;
        }
    }

    // Configure trick play

    if (VideoStartInfo->AutoPlay == 0) {
        if (CurrentPlayingSpeed != 0) {
            PausedSpeed = CurrentPlayingSpeed;
        } else {
            PausedSpeed = APPLIB_PLAYER_NORMAL_SPEED;
        }
        CurrentPlayingSpeed = 0;
    }


    Trick.Speed = CurrentPlayingSpeed;
    Trick.TimeOffsetOfFirstFrame = VideoStartTime;
    if (CurrentPlayingDir == APPLIB_VIDEO_PLAY_FW) {
        Trick.Direction = AMP_VIDEO_PLAY_FW;
    } else {
        Trick.Direction = AMP_VIDEO_PLAY_BW;
    }

    ReturnValue = AppLibFormatDemuxMp4_Start(Trick.TimeOffsetOfFirstFrame,Trick.Direction,APPLIB_TO_DEMUX_SPEED(Trick.Speed));
    if (ReturnValue != AMP_OK) {
            AmbaPrintColor(RED, "[Applib - VideoDec] %s:%u Failed to start video (%d).", __FUNCTION__, __LINE__, ReturnValue);
            return ReturnValue;
    }

    {
        UINT8 FileID;

        if (AppLibVideoDec_GetNextFileID(FileNumber, NextPlayingFileID, CurrentPlayingDir, &FileID) != 0) {
            AmbaPrintColor(RED, "[Applib - VideoDec] %s:%u Failed to get next file ID.", __FUNCTION__, __LINE__);
            return -1; // Error
        }

        // Start playing video
        AmbaPrint("[Applib - VideoDec] %s: AmpDec_Start start", __FUNCTION__);
        // Play video
        ReturnValue = AmpDec_Start(DecPipeHdlr, &Trick, &Display);
        if (VideoStartInfo->AutoPlay == 0) {
            AmbaPrintColor(GREEN, "[Applib - VideoDec] open pause %s:%u", __FUNCTION__, __LINE__);
            //AmpDec_Pause(DecPipeHdlr);
            // Open and pause
            // Remember speed befrore pause
            if (CurrentPlayingSpeed != 0) { // In case of duplicate pause actions.
                PausedSpeed = CurrentPlayingSpeed;
            }
            CurrentPlayingSpeed = 0;
            PausedTime = VideoStartTime + FileInfoList[CurrentFeedingFileID].InitTime;
        }
        if (ReturnValue != AMP_OK) {
            AmbaPrintColor(RED, "[Applib - VideoDec] %s:%u Failed to start playing video (%d).", __FUNCTION__, __LINE__, ReturnValue);
            return ReturnValue;
        }
        AmbaPrint("[Applib - VideoDec] %s: AmpDec_Start end", __FUNCTION__);

        // Set video state
        if (VideoStartInfo->AutoPlay) {
            VideoPlayerState = APPLIB_VIDEO_PLAYER_STATE_PLAY;
        } else {
            VideoPlayerState = APPLIB_VIDEO_PLAYER_STATE_PAUSE_CHANGE;
        }

        //CurrentPlayingFileID = NextPlayingFileID; // TODO: Update CurrentPlayingFileID by setting EOS of each file and handle EOS callback from MW video decoder
        NextPlayingFileID = FileID;
        NextFeedingFileID = FileID;

    }

    return 0; // Success
}

int AppLibVideoDec_Start(const APPLIB_VIDEO_START_INFO_s* VideoStartInfo)
{
    APPLIB_VIDEO_FILE_INFO_s File;
    APPLIB_VIDEO_START_MULTI_INFO_s StartInfo;

    // Set video configuration
    if (AppLibVideoDec_GetMultiStartDefaultCfg(&StartInfo) != 0) {
        return -1;
    }
    File.Filename = VideoStartInfo->Filename;
    StartInfo.File = &File;
    StartInfo.FileNum = 1;
    StartInfo.AutoPlay = VideoStartInfo->AutoPlay;
    StartInfo.StartTime = VideoStartInfo->StartTime;
    StartInfo.Direction = VideoStartInfo->Direction;
    StartInfo.ReloadFile = 1;
    StartInfo.ResetSpeed = VideoStartInfo->ResetSpeed;
    StartInfo.ResetZoom = VideoStartInfo->ResetZoom;

    if (AppLibVideoDec_GetMultiFileInfo(&StartInfo) != 0) {
        return -1;
    }

    if (AppLibVideoDec_StartMultiple(&StartInfo) != 0) {
        return -1;
    }

    return 0;
}

/*
 * For speed change only. Do not expect any change of direction, time offset or filename.
 */
static int AppLibVideoDec_SpeedChange(const UINT32 NextSpeed)
{
    int ReturnValue = 0;
    UINT64 VideoTime = 0;   // Current video time
    UINT32 StartTime = 0;   // Time offset when start playing video

    // Check state
    if (VideoPlayerState == APPLIB_VIDEO_PLAYER_STATE_INVALID) {
        AmbaPrint("[Applib - VideoDec] %s:%u Initialize first.", __FUNCTION__, __LINE__);
        return -1; // Error
    }

    switch (VideoPlayerState) {
        case APPLIB_VIDEO_PLAYER_STATE_PLAY: {
            // Preliminary check
            if (AvcDecHdlr == NULL) {
                AmbaPrintColor(RED, "[Applib - VideoDec] %s:%u AvcDecHdlr is NULL.", __FUNCTION__, __LINE__);
                return -1; // Error
            }
            if (DecPipeHdlr == NULL) {
                AmbaPrintColor(RED, "[Applib - VideoDec] %s:%u DecPipeHdlr is NULL.", __FUNCTION__, __LINE__);
                return -1; // Error
            }
            if (CurrentPlayingSpeed == 0) {
                AmbaPrintColor(RED, "[Applib - VideoDec] %s:%u Unexpected speed.", __FUNCTION__, __LINE__);
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
                AmbaPrintColor(RED, "[Applib - VideoDec] %s:%u Failed to get video time (%d).", __FUNCTION__, __LINE__, ReturnValue);
                return -1; // Error
            }
            // Stop playing video while keeping the last frame on-screen
            ReturnValue = AmpDec_StopWithLastFrm(DecPipeHdlr);
            if (ReturnValue != AMP_OK) {
                AmbaPrintColor(RED, "[Applib - VideoDec] %s:%u Failed to stop video with the last frame (%d).", __FUNCTION__, __LINE__, ReturnValue);
                return -1; // Error
            }

            AppLibVideoDec_CalStartTime(VideoTime,&StartTime);

            /**Check if need open next file, if need close current file and open next*/
            if (AppLibVideoDec_FeedNextFile(StartTime) != 0) {
                return -1; // Error
            }


            // Configure trick play
            Trick.Speed = CurrentPlayingSpeed; // Decode speed. Different form DemuxSpeed.
            Trick.TimeOffsetOfFirstFrame = StartTime;
            if (CurrentPlayingDir == APPLIB_VIDEO_PLAY_FW) {
                Trick.Direction = AMP_VIDEO_PLAY_FW;
            } else {
                Trick.Direction = AMP_VIDEO_PLAY_BW;
            }
            ReturnValue = AppLibFormatDemuxMp4_Start(Trick.TimeOffsetOfFirstFrame,Trick.Direction,APPLIB_TO_DEMUX_SPEED(Trick.Speed));
            if (ReturnValue != AMP_OK) {
                AmbaPrintColor(RED, "[Applib - VideoDec] %s:%u Failed to start video (%d).", __FUNCTION__, __LINE__, ReturnValue);
                return ReturnValue;
            }

            {
                UINT8 FileID;
                if (AppLibVideoDec_GetNextFileID(FileNumber, NextPlayingFileID, CurrentPlayingDir, &FileID) != 0) {
                    AmbaPrintColor(RED, "[Applib - VideoDec] %s:%u Failed to get next file ID.", __FUNCTION__, __LINE__);
                    return -1; // Error
                }

                // Start playing video
                AmbaPrint("[Applib - VideoDec] %s: AmpDec_Start start", __FUNCTION__);
                // Play video
                ReturnValue = AmpDec_Start(DecPipeHdlr, &Trick, &Display);
                if (ReturnValue != AMP_OK) {
                    AmbaPrintColor(RED, "[Applib - VideoDec] %s:%u Failed to start playing video (%d).", __FUNCTION__, __LINE__, ReturnValue);
                    return ReturnValue;
                }
                AmbaPrint("[Applib - VideoDec] %s: AmpDec_Start end", __FUNCTION__);

                //CurrentPlayingFileID = NextPlayingFileID;
                NextPlayingFileID = FileID;
                NextFeedingFileID = FileID;
            }

        }
            break;
        default:
            AmbaPrint("[Applib - VideoDec] %s:%u Change speed at wrong state (%d).", __FUNCTION__, __LINE__, VideoPlayerState);
            return -1; // Error
    }

    return 0; // Success
}

int AppLibVideoDec_SpeedUp(UINT32 *CurSpeed)
{
    UINT32 NextSpeed = 0; // Next playing speed

    // Check state
    if (VideoPlayerState == APPLIB_VIDEO_PLAYER_STATE_INVALID) {
        AmbaPrint("[Applib - VideoDec] %s:%u Initialize first.", __FUNCTION__, __LINE__);
        return -1; // Error
    }

    if (CurrentFeedingFileID == APPLIB_PLAYER_INVALID_FILE_ID) {
        AmbaPrint("[Applib - VideoDec] %s:%u  Current file play EOS.", __FUNCTION__, __LINE__);
        return -1; // Error
    }

    if (CurrentPlayingSpeed == 0) {
        NextSpeed = APPLIB_PLAYER_NORMAL_SPEED;
    } else {
        NextSpeed = MIN(CurrentPlayingSpeed << 1, APPLIB_PLAYER_MAX_SPEED);
    }

    // Change speed and start playing
    if (AppLibVideoDec_SpeedChange(NextSpeed) != 0) {
        return -1; // Error
    }

    // Set output value
    *CurSpeed = CurrentPlayingSpeed;

    return 0; // Success
}

int AppLibVideoDec_SpeedDown(UINT32 *CurSpeed)
{
    UINT32 NextSpeed = 0; // Next playing speed

    // Check state
    if (VideoPlayerState == APPLIB_VIDEO_PLAYER_STATE_INVALID) {
        AmbaPrint("[Applib - VideoDec] %s:%u Initialize first.", __FUNCTION__, __LINE__);
        return -1; // Error
    }

    if (CurrentFeedingFileID == APPLIB_PLAYER_INVALID_FILE_ID) {
        AmbaPrint("[Applib - VideoDec] %s:%u  Current file play EOS.", __FUNCTION__, __LINE__);
        return -1; // Error
    }

    if (CurrentPlayingSpeed == 0) {
        NextSpeed = APPLIB_PLAYER_NORMAL_SPEED;
    } else {
        NextSpeed = MAX(CurrentPlayingSpeed >> 1, APPLIB_PLAYER_MIN_SPEED);
    }

    // Change speed and start playing
    if (AppLibVideoDec_SpeedChange(NextSpeed) != 0) {
        return -1; // Error
    }

    // Set output value
    *CurSpeed = CurrentPlayingSpeed;

    return 0; // Success
}

int AppLibVideoDec_Pause(void)
{
    int ReturnValue = 0;

    // Check state
    if (VideoPlayerState == APPLIB_VIDEO_PLAYER_STATE_INVALID) {
        AmbaPrint("[Applib - VideoDec] %s:%u Initialize first.", __FUNCTION__, __LINE__);
        return -1; // Error
    }
    // Preliminary check
    if (DecPipeHdlr == NULL) {
        AmbaPrintColor(RED, "[Applib - VideoDec] %s:%u DecPipeHdlr is NULL.", __FUNCTION__, __LINE__);
        return -1;
    }
    if (CurrentPlayingSpeed == 0) {
        AmbaPrintColor(RED, "[Applib - VideoDec] %s:%u Unexpected speed.", __FUNCTION__, __LINE__);
        return -1;
    }

    switch (VideoPlayerState) {
        case APPLIB_VIDEO_PLAYER_STATE_PLAY:
            // Pause video
            ReturnValue = AmpDec_Pause(DecPipeHdlr);
            if (ReturnValue != AMP_OK) {
                AmbaPrintColor(RED, "[Applib - VideoDec] %s:%u Failed to pause video (%d).", __FUNCTION__, __LINE__, ReturnValue);
                return -1;
            }
            // Get current video time
            ReturnValue = AmpVideoDec_GetTime(AvcDecHdlr, &PausedTime);
            if (ReturnValue != AMP_OK) {
                AmbaPrintColor(RED, "[Applib - VideoDec] %s:%u Failed to get video time (%d).", __FUNCTION__, __LINE__, ReturnValue);
                return -1; // Error
            }
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
                AmbaPrintColor(RED, "[Applib - VideoDec] %s:%u Unexpected speed.", __FUNCTION__, __LINE__);
                return -1;
            }
            // Already paused
            break; // Do nothing and return success
        default:
            AmbaPrint("[Applib - VideoDec] %s:%u Pause at wrong state (%d).", __FUNCTION__, __LINE__, VideoPlayerState);
            return -1; // Error
    }

    return 0; // Success
}

int AppLibVideoDec_Resume(void)
{
    int ReturnValue = 0;

    // Check state
    if (VideoPlayerState == APPLIB_VIDEO_PLAYER_STATE_INVALID) {
        AmbaPrint("[Applib - VideoDec] %s:%u Initialize first.", __FUNCTION__, __LINE__);
        return -1; // Error
    }

    AmbaPrint("[Applib - VideoDec] Resume Start");
    switch (VideoPlayerState) {
        case APPLIB_VIDEO_PLAYER_STATE_PLAY:
            // Preliminary check
            if (CurrentPlayingSpeed == 0) {
                AmbaPrintColor(RED, "[Applib - VideoDec] %s:%u Unexpected speed.", __FUNCTION__, __LINE__);
                return -1;
            }
            break; // Do nothing and return success
        case APPLIB_VIDEO_PLAYER_STATE_PAUSE_CHANGE:
        case APPLIB_VIDEO_PLAYER_STATE_PAUSE:
            // Preliminary check
            if (DecPipeHdlr == NULL) {
                AmbaPrintColor(RED, "[Applib - VideoDec] %s:%u DecPipeHdlr is NULL.", __FUNCTION__, __LINE__);
                return -1;
            }
            if (CurrentPlayingSpeed != 0) {
                AmbaPrintColor(RED, "[Applib - VideoDec] %s:%u Unexpected speed.", __FUNCTION__, __LINE__);
                return -1;
            }
#ifdef CONFIG_APP_ARD
    AmbaKAL_TaskSleep(500);
#endif
            // Resume video
            ReturnValue = AmpDec_Resume(DecPipeHdlr);
            if (ReturnValue != AMP_OK) {
                AmbaPrintColor(RED, "[Applib - VideoDec] %s:%u Failed to resume video (%d).", __FUNCTION__, __LINE__, ReturnValue);
                return -1;
            }

            // Recover speed befrore pause
            CurrentPlayingSpeed = PausedSpeed;
            // Set video state
            VideoPlayerState = APPLIB_VIDEO_PLAYER_STATE_PLAY;
            break;
         /*
        case APPLIB_VIDEO_PLAYER_STATE_PAUSE_CHANGE:
            {
                APPLIB_VIDEO_START_MULTI_INFO_s VideoStartInfo;
                // Preliminary check
                if (AvcDecHdlr == NULL) {
                    AmbaPrintColor(RED, "[Applib - VideoDec] %s:%u AvcDecHdlr is NULL.", __FUNCTION__, __LINE__);
                    return -1; // Error
                }
                if (CurrentPlayingSpeed != 0) {
                    AmbaPrintColor(RED, "[Applib - VideoDec] %s:%u Unexpected speed.", __FUNCTION__, __LINE__);
                    return -1; // Error
                }

                // Configure video settings
                if (AppLibVideoDec_GetMultiStartDefaultCfg(&VideoStartInfo) != 0) {
                    return -1;
                }
                VideoStartInfo.AutoPlay = 1;
                VideoStartInfo.StartTime = PausedTime;
                VideoStartInfo.Direction = CurrentPlayingDir;
                VideoStartInfo.ReloadFile = 0; // Play previious files
                VideoStartInfo.ResetSpeed = 0; // Play at previous speed
                VideoStartInfo.ResetZoom = 0;
                if (AppLibVideoDec_StartMultiple(&VideoStartInfo) != 0) {
                    return -1; // Error
                }
            }
            break;
            */
        default:
            AmbaPrint("[Applib - VideoDec] %s:%u Resume at wrong state (%d).", __FUNCTION__, __LINE__, VideoPlayerState);
            return -1; // Error
    }

    return 0; // Success
}

int AppLibVideoDec_Step(void)
{
    int ReturnValue = 0;

    // Check state
    if (VideoPlayerState == APPLIB_VIDEO_PLAYER_STATE_INVALID) {
        AmbaPrint("[Applib - VideoDec] %s:%u Initialize first.", __FUNCTION__, __LINE__);
        return -1; // Error
    }

    if (DecPipeHdlr == NULL) {
        AmbaPrintColor(RED, "[Applib - VideoDec] %s:%u DecPipeHdlr is NULL.", __FUNCTION__, __LINE__);
        return -1;
    }

    ReturnValue = AmpDec_Step(DecPipeHdlr);
    if (ReturnValue != AMP_OK) {
        AmbaPrintColor(RED, "[Applib - VideoDec] %s:%u Failed to step one frame (%d).", __FUNCTION__, __LINE__, ReturnValue);
        return -1;
    }

    return ReturnValue;
}

int AppLibVideoDec_Stop(void)
{
    int ReturnValue = 0;

    // Check state
    if (VideoPlayerState == APPLIB_VIDEO_PLAYER_STATE_INVALID) {
        AmbaPrint("[Applib - VideoDec] %s:%u Initialize first.", __FUNCTION__, __LINE__);
        return -1; // Error
    }
    if (VideoPlayerState == APPLIB_VIDEO_PLAYER_STATE_IDLE) {
        // Do nothing
        AmbaPrint("[Applib - VideoDec] Stop end");
        return 0; // Success
    }
    AppLibFormatDemuxMp4_Stop();

    if (AppLibFormatDemuxMp4_Close(1) != 0) { // Erase fifo
        AmbaPrintColor(RED, "[Applib - VideoDec] %s:%u Failed to close demuxer.", __FUNCTION__, __LINE__);
        // Don't return
    }
    IsFileOpened = 0;

    if (DecPipeHdlr != NULL) {
        // Stop video
        ReturnValue = AmpDec_Stop(DecPipeHdlr);
        if (ReturnValue != AMP_OK) {
            AmbaPrintColor(RED, "[Applib - VideoDec] %s:%u Failed to stop video (%d).", __FUNCTION__, __LINE__, ReturnValue);
            return -1; // Error
        }
    } else {
        AmbaPrint("[Applib - VideoDec] %s:%u DecPipeHdlr is NULL.", __FUNCTION__, __LINE__);
    }

    // Set video state
    VideoPlayerState = APPLIB_VIDEO_PLAYER_STATE_IDLE;
    //reset current feeding file
    CurrentFeedingFileID = APPLIB_PLAYER_INVALID_FILE_ID;
    AmbaPrint("[Applib - VideoDec] Stop end");

    return 0; // Success
}

int AppLibVideoDec_Zoom(const UINT32 Factor, const INT32 X, const INT32 Y)
{
    int ReturnValue = 0;

    // Check state
    if (VideoPlayerState == APPLIB_VIDEO_PLAYER_STATE_INVALID) {
        AmbaPrint("[Applib - VideoDec] %s:%u Initialize first.", __FUNCTION__, __LINE__);
        return -1; // Error
    }

    if (DecPipeHdlr == NULL) {
        AmbaPrintColor(RED, "[Applib - VideoDec] %s:%u DecPipeHdlr is NULL.", __FUNCTION__, __LINE__);
        return -1;
    }

    switch (VideoPlayerState) {
        case APPLIB_VIDEO_PLAYER_STATE_IDLE:
            AmbaPrint("[Applib - VideoDec] %s:%u Zoom video at wrong state (%d).", __FUNCTION__, __LINE__, VideoPlayerState);
            return -1; // Error
        default:
            // Configure display settings
            VideoMagFactor = Factor;
            VideoShiftX = X;
            VideoShiftY = Y;
            // Calculate display area
            if (AppLibVideoDec_CalculateDisplayArea(&Display, VideoMagFactor, VideoShiftX, VideoShiftY) != 0) {
                AmbaPrintColor(RED, "[Applib - VideoDec] %s:%u Failed to calculate display area.", __FUNCTION__, __LINE__);
                return -1;
            }

            // Play video
            ReturnValue = AmpDec_Start(DecPipeHdlr, NULL, &Display);
            if (ReturnValue != AMP_OK) {
                AmbaPrintColor(RED, "[Applib - VideoDec] %s:%u Failed to zoom and start (%d).", __FUNCTION__, __LINE__, ReturnValue);
                return -1;
            }
            break;
    }


    AmbaPrint("[Applib - VideoDec] Zoom end");

    return ReturnValue;
}

int AppLibVideoDec_Exit(void)
{
    int ReturnValue = 0;

    AmbaPrint("[Applib - VideoDec] AppLibVideoDec_Exit");

    // Check state
    if (VideoPlayerState == APPLIB_VIDEO_PLAYER_STATE_INVALID) {
        AmbaPrint("[Applib - VideoDec] %s:%u Already exit.", __FUNCTION__, __LINE__);
        return 0;
    }

    VideoPlayerState = APPLIB_VIDEO_PLAYER_STATE_INVALID;

    AppLibFormatDemuxMp4_Close(1); // Erase fifo
    IsFileOpened = 0;

    // deinit
    if (DecPipeHdlr != NULL) {
        ReturnValue = AmpDec_Stop(DecPipeHdlr);
        if (ReturnValue != AMP_OK) {
            AmbaPrint("[Applib - VideoDec] %s:%u Failed to stop codec manager (%d).", __FUNCTION__, __LINE__, ReturnValue);
        }
        ReturnValue = AmpDec_Remove(DecPipeHdlr);
        if (ReturnValue != AMP_OK) {
            AmbaPrint("[Applib - VideoDec] %s:%u Failed to remove codec manager (%d).", __FUNCTION__, __LINE__, ReturnValue);
        }
        ReturnValue = AmpDec_Delete(DecPipeHdlr);
        if (ReturnValue != AMP_OK) {
            AmbaPrint("[Applib - VideoDec] %s:%u Failed to delete codec manager (%d).", __FUNCTION__, __LINE__, ReturnValue);
        }
        DecPipeHdlr = NULL;
    }

    if (AvcDecHdlr != NULL) {
        ReturnValue = AmpVideoDec_Delete(AvcDecHdlr);
        if (ReturnValue != AMP_OK) {
            AmbaPrint("[Applib - VideoDec] %s:%u Failed to delete video decoder (%d).", __FUNCTION__, __LINE__, ReturnValue);
        }
        AvcDecHdlr = NULL;
    }

    if (AudDecHdlr != NULL) {
        ReturnValue = AmpAudioDec_Delete(AudDecHdlr);
        if (ReturnValue != AMP_OK) {
            AmbaPrint("[Applib - VideoDec] %s:%u Failed to delete audio decoder (%d).", __FUNCTION__, __LINE__, ReturnValue);
        }
        AudDecHdlr = NULL;
    }

    if (AvcDescBufOri != NULL) {
        ReturnValue = AmbaKAL_BytePoolFree(AvcDescBufOri);
        if (ReturnValue != AMP_OK) {
            AmbaPrint("[Applib - VideoDec] %s:%u Failed to free memory at 0x%08x (%d).", __FUNCTION__, __LINE__, AvcDescBufOri, ReturnValue);
        }
        AvcDescBufOri = NULL;
    }

    if (AvcRawBuf != NULL) {
        /*ReturnValue = AmbaKAL_BytePoolFree(AvcRawBufOri);
        if (ReturnValue != AMP_OK) {
            AmbaPrint("[Applib - VideoDec] %s:%u Failed to free memory at 0x%08x (%d).", __FUNCTION__, __LINE__, AvcRawBufOri, ReturnValue);
        }*/
        AppLibComSvcMemMgr_FreeDSPMemory();
        AvcRawBufOri = NULL;
        AvcRawBuf = NULL;
    }

    if (AudDescBuf != NULL) {
        ReturnValue = AmbaKAL_BytePoolFree(AudDescBuf);
        if (ReturnValue != AMP_OK) {
            AmbaPrint("[Applib - VideoDec] %s:%u Failed to free memory at 0x%08x (%d).", __FUNCTION__, __LINE__, AudDescBuf, ReturnValue);
        }
        AudDescBuf = NULL;
    }

    if (AudRawBuf != NULL) {
        /*ReturnValue = AmbaKAL_BytePoolFree(AudRawBufOri);
        if (ReturnValue != AMP_OK) {
            AmbaPrint("[Applib - VideoDec] %s:%u Failed to free memory at 0x%08x (%d).", __FUNCTION__, __LINE__, AudRawBufOri, ReturnValue);
        }*/
        /**AppLibComSvcMemMgr_FreeDSPMemory only need to call once*/
        AudRawBufOri = NULL;
        AudRawBuf = NULL;
    }

    if (AudCodecBuf != NULL) {
        ReturnValue = AmbaKAL_BytePoolFree(AudCodecBuf);
        if (ReturnValue != AMP_OK) {
            AmbaPrint("[Applib - VideoDec] %s:%u Failed to free memory at 0x%08x (%d).", __FUNCTION__, __LINE__, AudCodecBuf, ReturnValue);
        }
        AudCodecBuf =  NULL;
    }

    // Deinit video task
    if (AppLibVideoTask_IsTaskInitialized()) {
        AppLibVideoTask_DeinitTask();
    }

    return ReturnValue;
}

int AppLibVideoDec_GetTime(UINT64 *time)
{
    int ReturnValue = -1;

    if (AvcDecHdlr == NULL) {
        AmbaPrint("[Applib - VideoDec] %s:%u AvcDecHdlr is NULL.", __FUNCTION__, __LINE__);
        return -1;
    }

    ReturnValue = AmpVideoDec_GetTime(AvcDecHdlr, time);
    if (ReturnValue != AMP_OK) {
        AmbaPrintColor(RED, "[Applib - VideoDec] %s:%u Failed to get current video time (%d).", __FUNCTION__, __LINE__, ReturnValue);
    }

    DBGMSG("[Applib - VideoDec] AppLibVideoDec_GetTime time = %lld", *time);

    return ReturnValue;
}

int AppLibVideoDec_SetPtsFrame(UINT32 frameCount,
                               UINT32 timePerFrame,
                               UINT32 timePerSec)
{
    int ReturnValue = -1;
    UINT64 EosTime = 0;
    UINT32 Delta = timePerFrame;

    if (AvcDecHdlr == NULL) {
        AmbaPrint("[Applib - VideoDec] %s:%u AvcDecHdlr is NULL.", __FUNCTION__, __LINE__);
        return -1;
    }

    if (frameCount <= 0) {
        AmbaPrint("[Applib - VideoDec] %s:%u Invalid nFrameCount %u", __FUNCTION__, __LINE__, frameCount);
        return -1; // Error
    }

    EosTime = (((UINT64) timePerFrame) * (UINT64) (frameCount - 1));

    AmbaPrint(
            "[Applib - VideoDec] %s frameCount = %u, timePerFrame = %u, EosTime = %lld, Delta = %u, timePerSec = %u",
            __FUNCTION__, frameCount, timePerFrame, EosTime, Delta, timePerSec);

    ReturnValue = AmpVideoDec_SetEosPts(AvcDecHdlr, EosTime, Delta, timePerSec);
    return ReturnValue;
}

int AppLibVideoDec_SetEosPts(UINT64 eosFileTime,
                             UINT32 timePerFrame,
                             UINT32 timePerSec)
{
    int ReturnValue = -1;

    AmbaPrint(
            "[Applib - VideoDec] %s eosFileTime = %lld, timePerFrame = %u, timePerSec = %u",
            __FUNCTION__, eosFileTime, timePerFrame, timePerSec);

    ReturnValue = AmpVideoDec_SetEosPts(AvcDecHdlr, eosFileTime, timePerFrame, timePerSec);
    return ReturnValue;
}
int AppLibVideoDec_DecodeErrorHandling(void)
{
    int ReturnValue = 0;

    ReturnValue = AppLibVideoDec_Stop();

    /* 'DecErrOccurredFlag' should be cleared after decoder has stopped already.
            Since AmpDec_Stop() is a blocking function, decoder will be stopped after AppLibVideoDec_Stop().
           So 'DecErrOccurredFlag' is cleared after AppLibVideoDec_Stop(). */
    DecErrOccurredFlag = 0;

    return ReturnValue;
}
#ifdef CONFIG_APP_ARD
int AppLibVideoDec_AmpAudioDec_Init(void)
{
    int ReturnValue = -1;

        // Initialize codec module
    if (ApplibAudioDecInitFlag < 0) {
            AMP_AUDIODEC_INIT_CFG_s CodecInitCfg;
            // Initialize system output
            ReturnValue = AmpAudio_OutputInit(CreateAudioOutputCtrl(), APPLIB_AUDIO_OUTPUT_TASK_PRIORITY);
            if (ReturnValue != AMP_OK) {
                AmbaPrintColor(RED, "[Applib - VideoDec] %s:%u Failed to initialize system output (%d).", __FUNCTION__, __LINE__, ReturnValue);
            }
            // Initialize codec module
            ReturnValue = AmpAudioDec_GetInitDefaultCfg(&CodecInitCfg);
            if (ReturnValue != AMP_OK) {
                AmbaPrintColor(RED, "[Applib - VideoDec] %s:%u Failed to get default audio decoder config (%d).", __FUNCTION__, __LINE__, ReturnValue);
            }

	        if (audCodecModuleBuf != NULL) {
	            // Print warnning. Memory is not cleaned up.
	            AmbaPrintColor(RED, "[Applib - VideoDec] %s:%u Free memory at 0x%08x.", __FUNCTION__, __LINE__, audCodecModuleBuf);
	            AmbaKAL_BytePoolFree(audCodecModuleBuf);
	            audCodecModuleBuf = NULL;
	        }
	        ReturnValue = AmpUtil_GetAlignedPool(APPLIB_G_MMPL, (void**) &(CodecInitCfg.WorkBuff), &audCodecModuleBuf, CodecInitCfg.WorkBuffSize, 1 << 5);
	        if (ReturnValue != AMP_OK) {
	            AmbaPrintColor(RED, "[Applib - VideoDec] %s:%u Failed to allocate memory (%d).", __FUNCTION__, __LINE__, ReturnValue);
	        }
	        CodecInitCfg.TaskInfo.Priority = APPLIB_AUDIO_DEC_TASK_PRIORITY;
	        ReturnValue = AmpAudioDec_Init(&CodecInitCfg);
	        if (ReturnValue != AMP_OK) {
	            AmbaPrintColor(RED, "[Applib - VideoDec] %s:%u Failed to initialize audio decoder (%d).", __FUNCTION__, __LINE__, ReturnValue);
	        }
	        ApplibAudioDecInitFlag = 0;
	}else{
		ReturnValue = AMP_OK;
	}

	return ReturnValue;
}

int AppLibVideoDec_AudioCodecHandler_Create(AMBA_AUDIO_TYPE_e type,AMP_AVDEC_HDLR_s **pAudDecHdlr,UINT8** pRawBuf,UINT32* RawBufSize)
{
	int ReturnValue = -1;

	if (AudDecHdlr == NULL) {
		AMBA_AUDIO_TASK_CREATE_INFO_s DecInfo;
		AMBA_ABU_CREATE_INFO_s AbuInfo;
		UINT32 DecSize, AbuSize;

		// Create audio codec hdlr
		// Get default config
		ReturnValue = AmpAudioDec_GetDefaultCfg(&AudCodecCfg);
		if (ReturnValue != AMP_OK) {
			AmbaPrintColor(RED, "[Applib - VideoDec] %s:%u Failed to get default audio codec handler config (%d).", __FUNCTION__, __LINE__, ReturnValue);
		}
		// Allocate memory for raw buffer
		if (AudRawBufOri != NULL) {
			// Print warnning. Memory is not cleaned up.
			AmbaPrintColor(RED, "[Applib - VideoDec] %s:%u Free memory at 0x%08x.", __FUNCTION__, __LINE__, AudRawBufOri);
			AmbaKAL_BytePoolFree(AudRawBufOri);
			AudRawBufOri = NULL;
		}
		ReturnValue = AmpUtil_GetAlignedPool(APPLIB_G_MMPL, &AudRawBuf, &AudRawBufOri, AUDIODEC_RAW_SIZE, 1 << 6);
		if (ReturnValue != AMP_OK) {
			AmbaPrintColor(RED, "[Applib - VideoDec] %s:%u Failed to allocate memory (%d).", __FUNCTION__, __LINE__, ReturnValue);
		}
		AudCodecCfg.RawBuffer = (UINT8*) AudRawBuf;
		AudCodecCfg.RawBufferSize = AUDIODEC_RAW_SIZE;
		if(pRawBuf){
			*pRawBuf = AudCodecCfg.RawBuffer;
		}
		if(RawBufSize){
			*RawBufSize = AudCodecCfg.RawBufferSize;
		}

		// Allocate memory for descriptor buffer
		if (AudDescBuf != NULL) {
			// Print warnning. Memory is not cleaned up.
			AmbaPrintColor(RED, "[Applib - VideoDec] %s:%u Free memory at 0x%08x.", __FUNCTION__, __LINE__, AudDescBuf);
			AmbaKAL_BytePoolFree(AudDescBuf);
			AudDescBuf = NULL;
		}
		ReturnValue = AmpUtil_GetAlignedPool(APPLIB_G_MMPL,(void**) &(AudCodecCfg.DescBuffer),&AudDescBuf,AUDIODEC_RAW_DESC_NUM * sizeof(AMP_BITS_DESC_s),1 << 5);
		if (ReturnValue != AMP_OK) {
			AmbaPrintColor(RED, "[Applib - VideoDec] %s:%u Failed to allocate memory (%d).", __FUNCTION__, __LINE__, ReturnValue);
		}
		AudCodecCfg.DescBufferNum = AUDIODEC_RAW_DESC_NUM;
		AudCodecCfg.CbEvent = AudioDec_CodecCB;
		AudCodecCfg.PureAudio = PURE_AUDIO;
		AudCodecCfg.MaxChannelNum = 2;
		AudCodecCfg.MaxFrameSize = 4096;
		AudCodecCfg.MaxSampleRate = 48000;
		AudCodecCfg.MaxChunkNum = 16;
		AudCodecCfg.I2SIndex = 0;

		if (type == AMBA_AUDIO_AAC) { //AAC FIXIT should from demuxer
			AudCodecCfg.DecType = AMBA_AUDIO_AAC;
			AudCodecCfg.DstSampleRate = 48000;
			AudCodecCfg.SrcSampleRate = 48000;
#ifdef CONFIG_APP_ARD
			AudCodecCfg.DstChannelMode = 2;
			AudCodecCfg.SrcChannelMode = 2;
#else
			AudCodecCfg.DstChannelMode = 2;
			AudCodecCfg.SrcChannelMode = 2;
#endif
			AudCodecCfg.FadeInTime = 0;
			AudCodecCfg.FadeOutTime = 0;
			AudCodecCfg.Spec.AACCfg.BitstreamType = AAC_BS_RAW;
		}else if(type == AMBA_AUDIO_PCM){
		        AudCodecCfg.DecType = AMBA_AUDIO_PCM;
		        AudCodecCfg.DstSampleRate = 16000; //48000;
		        AudCodecCfg.SrcSampleRate = 16000; //48000;
#ifdef CONFIG_APP_ARD
		        AudCodecCfg.DstChannelMode = 2;
		        AudCodecCfg.SrcChannelMode = 2;
#else
		        AudCodecCfg.DstChannelMode = 2;
		        AudCodecCfg.SrcChannelMode = 2;
#endif
		        AudCodecCfg.FadeInTime = 0;
		        AudCodecCfg.FadeOutTime = 0;

		        AudCodecCfg.Spec.PCMCfg.BitsPerSample = 16;
		        AudCodecCfg.Spec.PCMCfg.DataFormat = 0;
		        AudCodecCfg.Spec.PCMCfg.FrameSize = 1024;
		}else{
			AmbaPrintColor(RED, "[Applib - VideoDec] %s:%u Unknown audio type (%d).", __FUNCTION__, __LINE__, type);
		}

		DecInfo.MaxSampleFreq = AudCodecCfg.MaxSampleRate;
		DecInfo.MaxChNum = AudCodecCfg.MaxChannelNum;
		DecInfo.MaxFrameSize = AudCodecCfg.MaxFrameSize;
		DecSize = AmbaAudio_DecSizeQuery(&DecInfo);

		AbuInfo.MaxSampleFreq = AudCodecCfg.MaxSampleRate;
		AbuInfo.MaxChNum = AudCodecCfg.MaxChannelNum;
		AbuInfo.MaxChunkNum = AudCodecCfg.MaxChunkNum;
		AbuSize = AmbaAudio_BufferSizeQuery(&AbuInfo);

		if (AudCodecBuf != NULL) {
			// Print warnning. Memory is not cleaned up.
			AmbaPrintColor(RED, "[Applib - VideoDec] %s:%u Free memory at 0x%08x.", __FUNCTION__, __LINE__, AudCodecBuf);
			AmbaKAL_BytePoolFree(AudCodecBuf);
			AudCodecBuf = NULL;
		}
		ReturnValue = AmpUtil_GetAlignedPool(APPLIB_G_MMPL,(void**) &(AudCodecCfg.CodecCacheWorkBuff),&AudCodecBuf,DecSize + AbuSize,1 << 6);
		if (ReturnValue != AMP_OK) {
			AmbaPrintColor(RED, "[Applib - VideoDec] %s:%u Failed to allocate memory (%d).", __FUNCTION__, __LINE__, ReturnValue);
		}
		AudCodecCfg.CodecCacheWorkSize = DecSize + AbuSize;
		AmbaPrint("audio codec buffer : 0x%x, %d", AudCodecCfg.CodecCacheWorkBuff, AudCodecCfg.CodecCacheWorkSize);

		// Create audio codec handler
		ReturnValue = AmpAudioDec_Create(&AudCodecCfg, &AudDecHdlr);
		if (ReturnValue != AMP_OK) {
			AmbaPrintColor(RED, "[Applib - VideoDec] %s:%u Failed to create audio codec handler (%d).", __FUNCTION__, __LINE__, ReturnValue);
		}

		if(pAudDecHdlr != NULL){
			*pAudDecHdlr = AudDecHdlr;
		}
	} else {
		ReturnValue = AMP_OK;
	}

	return ReturnValue;
}

int AppLibVideoDec_AudioCodecHandler_Exit(void)
{
    int ReturnValue = 0;
    AmbaPrint("[Applib - VideoDec] %s",__FUNCTION__);

    if (AudDecHdlr != NULL) {
        ReturnValue = AmpAudioDec_Delete(AudDecHdlr);
        if (ReturnValue != AMP_OK) {
            AmbaPrintColor(RED, "[Applib - VideoDec] %s:%u Failed to delete audio decoder (%d).", __FUNCTION__, __LINE__, ReturnValue);
        }
        AudDecHdlr = NULL;
    }

    if (AudDescBuf != NULL) {
        ReturnValue = AmbaKAL_BytePoolFree(AudDescBuf);
        if (ReturnValue != AMP_OK) {
            AmbaPrintColor(RED, "[Applib - VideoDec] %s:%u Failed to free memory at 0x%08x (%d).", __FUNCTION__, __LINE__, AudDescBuf, ReturnValue);
        }
        AudDescBuf = NULL;
    }

    if (AudRawBufOri != NULL) {
        ReturnValue = AmbaKAL_BytePoolFree(AudRawBufOri);
        if (ReturnValue != AMP_OK) {
           AmbaPrintColor(RED, "[Applib - VideoDec] %s:%u Failed to free memory at 0x%08x (%d).", __FUNCTION__, __LINE__, AudRawBufOri, ReturnValue);
        }
        AudRawBufOri = NULL;
        AudRawBuf = NULL;
    }

    if (AudCodecBuf != NULL) {
        ReturnValue = AmbaKAL_BytePoolFree(AudCodecBuf);
        if (ReturnValue != AMP_OK) {
            AmbaPrintColor(RED, "[Applib - VideoDec] %s:%u Failed to free memory at 0x%08x (%d).", __FUNCTION__, __LINE__, AudCodecBuf, ReturnValue);
        }
        AudCodecBuf =  NULL;
    }

    AmbaPrint("[Applib - VideoDec] %s end",__FUNCTION__);
    return ReturnValue;
}
#endif

