/**
 * @file src/app/connected/applib/src/player/audio_decode/ApplibPlayer_AudioDec.c
 *
 * Implementation of MW audio player utility
 *
 * History:
 *    2015/01/06 - [Qiang Su] created file
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

#include <player/audio_decode/ApplibPlayer_AudioDec.h>
#include <applib.h>
#include <fifo/Fifo.h>
#include <player/Decode.h>
#include <player/AudioDec.h>
#include <player/Beep.h>
#include <stream/File.h>
#include <AmbaROM.h>
#include <string.h>
#include <comsvc/misc/util.h>
#include <stdlib.h>
#include <cfs/AmpCfs.h>
#include <AmbaUtility.h>
#include "../../AppLibTask_Priority.h"
#include "audio/ApplibAudio_beep.h"

//#define DEBUG_APPLIB_AUDIO_DEC
#if defined(DEBUG_APPLIB_AUDIO_DEC)
#define DBGMSG AmbaPrint
#define DBGMSGc(x) AmbaPrintColor(GREEN,x)
#define DBGMSGc2 AmbaPrintColor
#else
#define DBGMSG(...)
#define DBGMSGc(...)
#define DBGMSGc2(...)
#endif

extern int AppLibVideoDec_AmpAudioDec_Init(void);
extern int AppLibVideoDec_AudioCodecHandler_Create(AMBA_AUDIO_TYPE_e type,AMP_AVDEC_HDLR_s **pAudDecHdlr,UINT8** pRawBuf,UINT32* RawBufSize);
extern int AppLibVideoDec_AudioCodecHandler_Exit(void);

#define APPLIB_AUDIO_TASK_STACK_SIZE (8192)           ///< audio task stack size
#define APPLIBAUDIODEC_FEED_MAX_SIZE   (32<<10)

static AMP_DEC_PIPE_HDLR_s *DecPipeHdlr = NULL;
static APPLIB_AUDIO_PLAYER_STATE_e AudioPlayerState = APPLIB_AUDIO_PLAYER_STATE_INVALID;
static AMBA_AUDIO_TYPE_e AppLibAudioDecFiletype = AMBA_AUDIO_AAC;
static AMP_CFS_FILE_s *AppLibAudioDecRawFile = NULL;                    ///< file pointer for audio file
static UINT32 AppLibAudioDecRawFileSize = 0;                   ///< size of audio file
static UINT8 AppLibAudioVolume = APPLIB_AUDIO_MAX_VOLUME_LEVLE;
static AMP_AVDEC_HDLR_s *AppLibAudioDecHdlr = NULL;
static void *AppLibAudioStackRaw = NULL;   // Original address of stack for task
static void *AppLibAudioStack = NULL;      // Aligned address of stack for task
static int AppLibAudioDecDemuxTaskInit = 0;
static AMBA_KAL_MUTEX_t AppLibAudioDecFrmMutex = { 0 };     ///< mutex for frame feeding
static AMBA_KAL_TASK_t AppLibAudioDecDemuxTask = { 0 };     ///< task for audio file feeding
static UINT8* AppLibAudioDecHdlr_RawBuf = NULL;
static UINT32 AppLibAudioDecHdlr_RawBufSize = 0;
static AMP_FIFO_HDLR_s *AppLibAudDecFifoHdlr = NULL;      ///< fifo to feed audio file

static UINT8 AppLibAudioDecBeepEnable = 0;
static char *BeepFn[BEEP_ID_NUM] = {"power_on_48k.pcm","power_off_48k.pcm","error_48k.pcm","optone_48k.pcm", \
                                    "LLWS_48k.pcm", "FCMD_48k.pcm", "FCWS_48k.pcm", "LDWS_48k.pcm"};

/**
 * The function is a task that feeding audio file to fifo.\n
 *
 * @param info - not used
 */
void AppLibAudioDec_DemuxTask(UINT32 info)
{
    AMP_BITS_DESC_s Desc;
    AMP_BITS_DESC_s TmpDesc = { 0 };
    UINT32 FreeToEnd= 0;
    UINT32 Remain= 0;
    UINT32 FeedSz = 0;
    UINT8* RawBase = AppLibAudioDecHdlr_RawBuf;
    UINT8* RawLimit = (UINT8*) ((UINT32) AppLibAudioDecHdlr_RawBuf + AppLibAudioDecHdlr_RawBufSize - 1);
        //static UINT32 TotalFeed = 0;

    AmbaPrint("AppLibAudioDec_DemuxTask Start!");

    while (1) {
        UINT8 IsFeedEOS = 0; // Whether to feed EOS
        AmbaKAL_TaskSleep(1);
        AmbaKAL_MutexTake(&AppLibAudioDecFrmMutex, AMBA_KAL_WAIT_FOREVER );
        if (AppLibAudioDecRawFile == NULL || AppLibAudioDecRawFileSize == 0) {
            //AmbaPrintColor(GREEN,"File is invalid");
            AmbaKAL_MutexGive(&AppLibAudioDecFrmMutex);
            AmbaKAL_TaskSleep(10);
            continue;
        }

        // Initialize descriptor
        SET_ZERO(TmpDesc);

        // prepare entry
        AmpFifo_PrepareEntry(AppLibAudDecFifoHdlr, &Desc);
        TmpDesc.StartAddr = Desc.StartAddr;
        if (Desc.Size < APPLIBAUDIODEC_FEED_MAX_SIZE) {
            AmbaKAL_MutexGive(&AppLibAudioDecFrmMutex);
            AmbaKAL_TaskSleep(10);
            continue;
        }

        if (AppLibAudioDecRawFileSize > APPLIBAUDIODEC_FEED_MAX_SIZE) {
            FeedSz = APPLIBAUDIODEC_FEED_MAX_SIZE;
            AppLibAudioDecRawFileSize -= APPLIBAUDIODEC_FEED_MAX_SIZE;
            IsFeedEOS = 0;
        } else{
            FeedSz = AppLibAudioDecRawFileSize;
            AppLibAudioDecRawFileSize = 0;
            IsFeedEOS = 1;
        }

        TmpDesc.Size = FeedSz;
        //TotalFeed += FeedSz;
        TmpDesc.Type = AMP_FIFO_TYPE_AUDIO_FRAME;

        if ((TmpDesc.StartAddr + TmpDesc.Size - 1) <= RawLimit) {
            AmpCFS_fread(TmpDesc.StartAddr, TmpDesc.Size, 1, AppLibAudioDecRawFile);
        } else {
            FreeToEnd = RawLimit - TmpDesc.StartAddr + 1;
            Remain = TmpDesc.Size - FreeToEnd;
            AmpCFS_fread(TmpDesc.StartAddr, FreeToEnd, 1, AppLibAudioDecRawFile);
            AmpCFS_fread(RawBase, Remain, 1, AppLibAudioDecRawFile);
        }

        //AmbaPrintColor(GREEN,"write fifo ");
        // write to fifo
        AmpFifo_WriteEntry(AppLibAudDecFifoHdlr, &TmpDesc);
        TmpDesc.SeqNum++;

        if (IsFeedEOS) {
            TmpDesc.Type = AMP_FIFO_TYPE_EOS;
            TmpDesc.Size = AMP_FIFO_MARK_EOS;
            TmpDesc.StartAddr = 0;
            TmpDesc.Pts = 0;
            AmbaPrintColor(GREEN,"Feed EOS",TmpDesc.Type,TmpDesc.Size);
            // write to fifo
            AmpFifo_WriteEntry(AppLibAudDecFifoHdlr, &TmpDesc);
            TmpDesc.SeqNum++;
        }

        AmbaKAL_MutexGive(&AppLibAudioDecFrmMutex);
        //AmbaKAL_TaskSleep(1);
    }
}

/**
 * fifo callback function.\n
 * we do not need information from this callback for now.\n
 * Please check fifo document for more detail it could provide.
 */
int AppLibAudioDecTask_FifoCB(void *hdlr,
                                   UINT32 event,
                                   void* info)
{
    //AmbaPrint("AppLibAudioDecTask_FifoCB on Event: 0x%x ", event);
    return 0;
}

int AppLibAudioDecTask_InitTask(void)
{
    int ReturnValue = 0;
    AMP_FIFO_CFG_s FifoDefCfg = { 0 };

    if(AppLibAudioDecDemuxTaskInit == 1){
        AmbaPrint("%s:%u Already initialized.", __FUNCTION__, __LINE__);
        return 0;
    }

    /* create fifo*/
    if(AppLibAudDecFifoHdlr == NULL){
        AmpFifo_GetDefaultCfg(&FifoDefCfg);
        FifoDefCfg.hCodec = AppLibAudioDecHdlr;
        FifoDefCfg.IsVirtual = 1;
        FifoDefCfg.NumEntries = 128;
        FifoDefCfg.cbEvent = AppLibAudioDecTask_FifoCB;
        AmpFifo_Create(&FifoDefCfg, &AppLibAudDecFifoHdlr);
    }

    if (AppLibAudioStackRaw == NULL) {
        ReturnValue = AmpUtil_GetAlignedPool(APPLIB_G_MMPL,(void**) &AppLibAudioStack,&AppLibAudioStackRaw,APPLIB_AUDIO_TASK_STACK_SIZE,1 << 5); // Align 32
        if (ReturnValue != AMP_OK) {
            AmbaPrintColor(RED, "[Applib - AudioDec] %s:%u Failed to allocate memory (%d).", __FUNCTION__, __LINE__, ReturnValue);
        }
    }

    /* Create task */
    ReturnValue = AmbaKAL_TaskCreate(&AppLibAudioDecDemuxTask, /* pTask */
        "AppLib_AudioDec_FWriteTask", /* pTaskName */
        17, /* Priority */
        AppLibAudioDec_DemuxTask, /* void (*EntryFunction)(UINT32) */
        0x0, /* EntryArg */
        (void *) AppLibAudioStack, /* pStackBase */
        APPLIB_AUDIO_TASK_STACK_SIZE, /* StackByteSize */
        AMBA_KAL_AUTO_START); /* AutoStart */

    if (ReturnValue != AMP_OK) {
        AmbaPrintColor(RED,"%s:Task create failed: %d", __FUNCTION__,ReturnValue);
    }

    /* create mutex  for frame read and write to fifo*/
    ReturnValue = AmbaKAL_MutexCreate(&AppLibAudioDecFrmMutex);
    if (ReturnValue != AMP_OK) {
        AmbaPrintColor(RED,"%s:Mutex create failed: %d", __FUNCTION__,ReturnValue);
    }

    AppLibAudioDecDemuxTaskInit = 1;
    return ReturnValue;
}

int AppLibAudioDecTask_DeinitTask(void)
{
    int ReturnValue = 0; // Success
    int Rval = 0; // Function call return

    if(AppLibAudioDecDemuxTaskInit == 0){
        return AMP_OK;
    }

    AmbaPrint("[Applib - AudioDec] <DeinitTask> Start");

    /** Step 1: Terminate task */
    Rval = AmbaKAL_TaskTerminate(&AppLibAudioDecDemuxTask);
    // TX_THREAD_ERROR: The task is not created.
    if ((Rval != AMP_OK) && (Rval != TX_THREAD_ERROR)) {
        AmbaPrintColor(RED,"[Applib - AudioDec] %s:%u Failed to terminate task (%d).", __FUNCTION__, __LINE__, Rval);
        ReturnValue = -1; // Error
    }

    /** Step 2: Delete task */
    Rval = AmbaKAL_TaskDelete(&AppLibAudioDecDemuxTask);
    // TX_THREAD_ERROR: The task is not created.
    // TX_DELETE_ERROR: The task is not terminated.
    if ((Rval != AMP_OK) && (Rval != TX_THREAD_ERROR) && (Rval != TX_DELETE_ERROR)) {
        AmbaPrintColor(RED,"[Applib - AudioDec] %s:%u Failed to delete task (%d).", __FUNCTION__, __LINE__, Rval);
        ReturnValue = -1; // Error
    }

    /** Step 3: Release stack */
    if (AppLibAudioStackRaw != NULL) {
        Rval = AmbaKAL_BytePoolFree(AppLibAudioStackRaw);
        if (Rval != AMP_OK) {
            AmbaPrintColor(RED,"[Applib - AudioDec] %s:%u Failed to release stack (%d).", __FUNCTION__, __LINE__, Rval);
            ReturnValue = -1; // Error
        }
        AppLibAudioStackRaw = NULL;
        AppLibAudioStack = NULL;
    }

    // Delete mutex
    Rval = AmbaKAL_MutexDelete(&AppLibAudioDecFrmMutex);
    if (Rval != AMP_OK) {
        AmbaPrintColor(RED, "Failed to delete mutex: %d", Rval);
    }

    /** Step 6: Reset flag */
    AppLibAudioDecDemuxTaskInit = 0;

    AmbaPrint("[Applib - AudioDec] <DeinitTask> End");
    return ReturnValue;
}

static int AppLibAudioDecTask_open_file(char* FileName)
{
    AMP_CFS_FILE_PARAM_s cfsParam;

    AmbaKAL_MutexTake(&AppLibAudioDecFrmMutex, AMBA_KAL_WAIT_FOREVER );
    // open file
    if (AmpCFS_GetFileParam(&cfsParam) != AMP_OK) {
        AmbaPrintColor(RED,"AppLibAudioDecTask_open_file: GetFileParam failed");
        return -1;
    }
    cfsParam.Mode = AMP_CFS_FILE_MODE_READ_ONLY;

    strcpy((char *)cfsParam.Filename, FileName);
    AppLibAudioDecRawFile = AmpCFS_fopen(&cfsParam);

    if (AppLibAudioDecRawFile == NULL ) {
        AmbaPrintColor(RED,"AppLibAudioDecTask_open_file: raw open failed");
        return -1;
    }
    AmpCFS_fseek(AppLibAudioDecRawFile, 0, AMBA_FS_SEEK_END);
    AppLibAudioDecRawFileSize = AmpCFS_ftell(AppLibAudioDecRawFile);
    AmpCFS_fseek(AppLibAudioDecRawFile, 0, AMBA_FS_SEEK_START);
    AmbaKAL_MutexGive(&AppLibAudioDecFrmMutex);

    return 0;
}

int AppLibAudioDecTask_close_file(void)
{
    AmbaKAL_MutexTake(&AppLibAudioDecFrmMutex, AMBA_KAL_WAIT_FOREVER );
    if(AppLibAudioDecRawFile != NULL){
        AmpCFS_fclose(AppLibAudioDecRawFile);
        AppLibAudioDecRawFile = NULL;
        AppLibAudioDecRawFileSize = 0;
    }
    AmbaKAL_MutexGive(&AppLibAudioDecFrmMutex);
    return 0;
}

int AppLibAudioDec_Init(void)
{
    int ReturnValue = -1;

    AmbaPrint("[Applib - AudioDec] <Init> start");

    // Check state
    if (AudioPlayerState != APPLIB_AUDIO_PLAYER_STATE_INVALID) {
        AmbaPrint("[Applib - AudioDec] %s:%u Already initialized.", __FUNCTION__, __LINE__);
        goto ReturnSuccess;
    }

    // Initialize audio codec module
    ReturnValue = AppLibVideoDec_AmpAudioDec_Init();
    if (ReturnValue != AMP_OK) {
        AmbaPrintColor(RED, "%s:%u Failed[%d]", __FUNCTION__, __LINE__,ReturnValue);
        goto ReturnError;
    }

    // Initialize audio codec
    ReturnValue = AppLibVideoDec_AudioCodecHandler_Create(AppLibAudioDecFiletype,&AppLibAudioDecHdlr,&AppLibAudioDecHdlr_RawBuf,&AppLibAudioDecHdlr_RawBufSize);
    if (ReturnValue != AMP_OK) {
        AmbaPrintColor(RED, "%s:%u Failed[%d]", __FUNCTION__, __LINE__,ReturnValue);
        goto ReturnError;
    }

    // Create decoder manager
    if (DecPipeHdlr == NULL) {
        AMP_DEC_PIPE_CFG_s PipeCfg;
        // Get default config
        ReturnValue = AmpDec_GetDefaultCfg(&PipeCfg);
        if (ReturnValue != AMP_OK) {
            AmbaPrintColor(RED, "[Applib - AudioDec] %s:%u Failed to get default decoder manager config (%d).", __FUNCTION__, __LINE__, ReturnValue);
            goto ReturnError;
        }

        // FIXIT WAIT demux fix
        PipeCfg.Decoder[0] = AppLibAudioDecHdlr;
        PipeCfg.NumDecoder = 1;
        PipeCfg.Type = AMP_DEC_VID_PIPE;
        ReturnValue = AmpDec_Create(&PipeCfg, &DecPipeHdlr);
        if (ReturnValue != AMP_OK) {
            AmbaPrintColor(RED, "[Applib - AudioDec] %s:%u Failed to create decoder manager (%d).", __FUNCTION__, __LINE__, ReturnValue);
            goto ReturnError;
        }
    }

    // Initialize decode task
    ReturnValue = AppLibAudioDecTask_InitTask();
    if (ReturnValue != AMP_OK) {
        AmbaPrintColor(RED, "%s:%u Failed[%d]", __FUNCTION__, __LINE__,ReturnValue);
        goto ReturnError;
    }

    ReturnValue = AppLibAudioDec_SetOutputVolume();
    if (ReturnValue != AMP_OK) {
        AmbaPrintColor(RED, "%s:%u Failed[%d]", __FUNCTION__, __LINE__,ReturnValue);
        goto ReturnError;
    }

    // Active pipe
    ReturnValue = AmpDec_Add(DecPipeHdlr);
    if (ReturnValue != AMP_OK) {
        AmbaPrintColor(RED, "[Applib - AudioDec] %s:%u Failed to activate decoder manager (%d).", __FUNCTION__, __LINE__, ReturnValue);
        goto ReturnError;
    }

    // Set audio state
    AudioPlayerState = APPLIB_AUDIO_PLAYER_STATE_IDLE;

    ReturnSuccess:
    AmbaPrint("[Applib - AudioDec] <Init> end");
    return AMP_OK;

    ReturnError:
    AppLibAudioDec_Exit();
    AmbaPrintColor(RED, "[Applib - AudioDec] <Init> Error end");
    return -1;
}

int AppLibAudioDec_Start(char* FileName)
{
    int ReturnValue = -1;
    AMP_AUDIODEC_DECODER_CFG_s Cfg;
    AMP_AVDEC_TRICKPLAY_s Trick = { 0 };

    // file open
    ReturnValue = AppLibAudioDecTask_open_file(FileName);
    if (ReturnValue != 0) {
        return -1;
    }

    AmbaKAL_TaskSleep(100);

    memset(&Cfg, 0, sizeof(Cfg));
    Cfg.SrcSampleRate = 48000;
    if(AppLibAudioDecFiletype == AMBA_AUDIO_AAC){
        Cfg.DecType = AMBA_AUDIO_AAC;
        Cfg.Spec.AACCfg.BitstreamType = AAC_BS_ADTS;
    }else{
        Cfg.DecType = AMBA_AUDIO_PCM;
        Cfg.Spec.PCMCfg.BitsPerSample = 16;
        Cfg.Spec.PCMCfg.DataFormat = 0;
        Cfg.Spec.PCMCfg.FrameSize = 1024;
    }
    Cfg.FadeInTime = 0;
    Cfg.FadeOutTime = 0;
#ifdef CONFIG_APP_ARD
    Cfg.SrcChannelMode = 2;
#else
    Cfg.SrcChannelMode = 2;
#endif
    AmpAudioDec_DecoderCfg(&Cfg, AppLibAudioDecHdlr);

    Trick.Speed = 0x100;
    ReturnValue = AmpDec_Start(DecPipeHdlr, &Trick, NULL );
    if (ReturnValue != AMP_OK) {
        AmbaPrintColor(RED, "[Applib - AudioDec] %s:%u Failed to start playing (%d).", __FUNCTION__, __LINE__, ReturnValue);
        return ReturnValue;
    }

    // Set audio state
    AudioPlayerState = APPLIB_AUDIO_PLAYER_STATE_PLAY;

    return AMP_OK;
}

APPLIB_AUDIO_PLAYER_STATE_e AppLibAudioDec_GetPlayerState(void)
{
    return AudioPlayerState;
}

int AppLibAudioDec_Stop(void)
{
    int ReturnValue = 0;

    // Check state
    if (AudioPlayerState == APPLIB_AUDIO_PLAYER_STATE_INVALID) {
        AmbaPrint("[Applib - AudioDec] %s:%u Initialize first.", __FUNCTION__, __LINE__);
        return -1; // Error
    }
    if (AudioPlayerState == APPLIB_AUDIO_PLAYER_STATE_IDLE) {
        // Do nothing
        AmbaPrint("[Applib - AudioDec] Stop end");
        return 0; // Success
    }

    AppLibAudioDecTask_close_file();

    if (AmpFifo_EraseAll(AppLibAudDecFifoHdlr) != 0) { // Erase fifo
        AmbaPrintColor(RED, "[Applib - AudioDec] %s:%u Failed to close demuxer.", __FUNCTION__, __LINE__);
        // Don't return
    }

    // Set audio state
    AudioPlayerState = APPLIB_AUDIO_PLAYER_STATE_IDLE;

    if (DecPipeHdlr != NULL) {
        // Stop audio
        ReturnValue = AmpDec_Stop(DecPipeHdlr);
        if (ReturnValue != AMP_OK) {
            AmbaPrintColor(RED, "[Applib - AudioDec] %s:%u Failed to stop audio (%d).", __FUNCTION__, __LINE__, ReturnValue);
            return -1; // Error
        }
    } else {
        AmbaPrint("[Applib - AudioDec] %s:%u DecPipeHdlr is NULL.", __FUNCTION__, __LINE__);
    }

    AmbaPrint("[Applib - AudioDec] Stop end");

    return 0; // Success
}

int AppLibAudioDec_Exit(void)
{
    int ReturnValue = 0;

    AmbaPrint("[Applib - AudioDec] AppLibAudioDec_Exit");

    // Check state
    if (AudioPlayerState == APPLIB_AUDIO_PLAYER_STATE_INVALID) {
        AmbaPrint("[Applib - AudioDec] %s:%u Already exit.", __FUNCTION__, __LINE__);
        return 0;
    }

    AudioPlayerState = APPLIB_AUDIO_PLAYER_STATE_INVALID;

    if (AppLibAudDecFifoHdlr != NULL) {
        ReturnValue = AmpFifo_EraseAll(AppLibAudDecFifoHdlr);
        if (ReturnValue != AMP_OK) {
            AmbaPrint("[Applib - AudioDec] %s:%u Failed to erase audio fifo data (%d).", __FUNCTION__, __LINE__, ReturnValue);
            ReturnValue = -1;
        }

        ReturnValue = AmpFifo_Delete(AppLibAudDecFifoHdlr);
        if (ReturnValue != AMP_OK) {
            AmbaPrint("[Applib - AudioDec] %s:%u Failed to delete audio fifo (%d).", __FUNCTION__, __LINE__, ReturnValue);
            ReturnValue = -1;
        }
        AppLibAudDecFifoHdlr = NULL;
    }

    // deinit
    if (DecPipeHdlr != NULL) {
        ReturnValue = AmpDec_Stop(DecPipeHdlr);
        if (ReturnValue != AMP_OK) {
            AmbaPrint("[Applib - AudioDec] %s:%u Failed to stop codec manager (%d).", __FUNCTION__, __LINE__, ReturnValue);
        }
        ReturnValue = AmpDec_Remove(DecPipeHdlr);
        if (ReturnValue != AMP_OK) {
            AmbaPrint("[Applib - AudioDec] %s:%u Failed to remove codec manager (%d).", __FUNCTION__, __LINE__, ReturnValue);
        }
        ReturnValue = AmpDec_Delete(DecPipeHdlr);
        if (ReturnValue != AMP_OK) {
            AmbaPrint("[Applib - AudioDec] %s:%u Failed to delete codec manager (%d).", __FUNCTION__, __LINE__, ReturnValue);
        }
        DecPipeHdlr = NULL;
    }

    AppLibVideoDec_AudioCodecHandler_Exit();
    // Deinit audio task
    AppLibAudioDecTask_DeinitTask();

    return ReturnValue;
}

void AppLibAudioDec_Beep_Init(void)
{
    // Initialize audio codec module
    if (AppLibVideoDec_AmpAudioDec_Init() != AMP_OK) {
        AmbaPrintColor(RED, "%s:%u Failed", __FUNCTION__, __LINE__);
    }
}

void AppLibAudioDec_Beep_Enable(UINT8 enable)
{
    AppLibAudioDecBeepEnable = enable;
}

#ifdef CONFIG_APP_ARD
static UINT8 BeepVol = APPLIB_AUDIO_MAX_VOLUME_LEVLE;
void AppLibAudioDec_BeepVolume(UINT8 Vol)
{
    BeepVol = Vol;
}
#endif

void AppLibAudioDec_Beep(BEEP_FILE_ID_e beep_id,UINT8 wait_done)
{
    int ReturnValue = -1;
    UINT32 TimeoutMs = 0;

    if((beep_id >= BEEP_POWER_ON) &&(beep_id < BEEP_ID_NUM) && AppLibAudioDecBeepEnable) {
        APPLIB_AUDIO_BEEP_TASK_MSG_s BeepMsg = {0};
#ifdef CONFIG_APP_ARD
        AppLibAudioDec_SetVolume(BeepVol);
#else
        AppLibAudioDec_SetVolume(APPLIB_AUDIO_MAX_VOLUME_LEVLE);
#endif
        BeepMsg.MessageType = APPLIB_AUDIO_BEEP_TASK_MSG_PLAY_BEEP;
        snprintf(BeepMsg.beepInfo.Fn,sizeof(BeepMsg.beepInfo.Fn),"%s",BeepFn[beep_id]);
        BeepMsg.beepInfo.BeepType = BEEP_FROM_FILE;
        BeepMsg.beepInfo.RawPcm = NULL;
        BeepMsg.beepInfo.RawSize = 0;
        BeepMsg.beepInfo.SrcChannelMode = 2;
        BeepMsg.beepInfo.SrcSampleRate = 48000;
        BeepMsg.beepInfo.DstSampleRate = BeepMsg.beepInfo.SrcSampleRate;

        ReturnValue = AppLibAudio_SendMsg(&BeepMsg, AMBA_KAL_NO_WAIT);
        if(ReturnValue != AMP_OK){
            AmbaPrintColor(RED, "%s:%u ,ReturnValue=%d Failed", __FUNCTION__, __LINE__,ReturnValue);
        }

        if(wait_done){
            /*If need wait for beep done,set timeout as 5s.TBD*/
            if(beep_id == BEEP_POWER_OFF){
                TimeoutMs = 5000;
            }else{
                TimeoutMs = 5000;
            }
            AppLibAudio_BeepSetPlayFlag();
            AppLibAudio_Beep_Wait_Done(TimeoutMs);
            AmbaPrintk_Flush();
        }
    }else{
        //AmbaPrintColor(RED, "%s:%u Failed", __FUNCTION__, __LINE__);
    }
}

int AppLibAudioDec_SetVolume(UINT8 Volumes)
{
    AmbaPrintColor(GREEN,"%s ,vol=%d", __FUNCTION__,Volumes);
    BeepVol = Volumes;
    AppLibAudioDec_SetOutputVolume();
    return 0;
}

extern UINT32* AmpAudioDec_GetAudOutputCtrl(UINT8 I2SIdx);
int AppLibAudioDec_SetOutputVolume(void)
{
    int ReturnValue = -1;
    UINT32* g_pOutputCtrl = AmpAudioDec_GetAudOutputCtrl(0);

    ReturnValue = AppLibAudio_EffectVolumeUpdate(APPLIB_AUDIO_EFFECT_OUTPUT,g_pOutputCtrl,BeepVol);

    if(ReturnValue< 0){
        AmbaPrintColor(RED,"%s:%u,vol=%d rval = %d", __FUNCTION__, __LINE__,BeepVol,ReturnValue);
    }

    return 0;
}

