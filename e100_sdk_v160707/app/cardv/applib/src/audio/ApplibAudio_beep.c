/**
 * @file app/connected/applib/src/audio/beep.c
 *
 * Implementation of beep sound.
 *
 * History:
 *    2014/12/15 - [Jamie Cheng] created file
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
#include "audio/ApplibAudio_beep.h"
#include <AmbaROM.h>

#define AUDIO_OUTPUT_TASK_PRIORITY (59)
static void *BeepCodecBuf = NULL;
static AMBA_MEM_CTRL_s AudCtrlCacheBuf = { 0 };       ///< Cache buffer for audio output control
static AMBA_MEM_CTRL_s AudCtrlNonCacheBuf = { 0 };    ///< Non-cache buffer for audio output control

#define AUDIO_BEEP_PRIORITY  (131)
#define APPLIB_AUDIO_BEEP_TASK_MSG_NUM    (64) ///< Maximum number of beep messages in beep message pool.
#define AUDIO_BEEP_TASK_STACK_SIZE  (0x4000) ///< Size of the stack used by a beep Task (16kB).
APPLIB_AUDIO_BEEP_TASK_RESOURCE_s APPLIB_G_BEEP = { 0 };

static void *beepMsgPoolRaw = NULL;
static void *beepStackRaw = NULL;
static void *beepStack = NULL;

//#define DEBUG_APPLIB_AUDIOBEEP
#if defined(DEBUG_APPLIB_AUDIOBEEP)
#define DBGMSG AmbaPrint
#define DBGMSGc(x) AmbaPrintColor(GREEN,x)
#define DBGMSGc2 AmbaPrintColor
#else
#define DBGMSG(...)
#define DBGMSGc(...)
#define DBGMSGc2(...)
#endif

#ifdef CONFIG_APP_ARD
#define TIMEOUT_WAIT_TICK  (50)
static UINT8 AppLibAudio_Beep_Playing = 0;
void AppLibAudio_BeepSetPlayFlag(void)
{
    AppLibAudio_Beep_Playing = 1;
}

void AppLibAudio_Beep_Wait_Done(UINT32 TimeoutMs)
{
    INT32 time = 0;

    if(TimeoutMs < TIMEOUT_WAIT_TICK){
        time = 1;
    }else{
        time=TimeoutMs/TIMEOUT_WAIT_TICK;
    }

    while(AppLibAudio_Beep_Playing == 1){
        AmbaKAL_TaskSleep(TIMEOUT_WAIT_TICK);
        time--;
        if(time < 0){
            AmbaPrintColor(RED,"Beep_Wait_Done Timeout");
            break;
        }
    }
    AmbaPrintColor(GREEN,"Beep done");
}
#endif

int AppLibAudio_SendMsg(const APPLIB_AUDIO_BEEP_TASK_MSG_s *beepMsg, const UINT32 Timeout)
{
    return AmpMsgQueue_Send(&APPLIB_G_BEEP.beepMsgQueue, beepMsg, Timeout);
}
static void _AppLibAudio_BeepTask(UINT32 EntryArg)
{
    APPLIB_AUDIO_BEEP_TASK_MSG_s beepMsg = {0};       // Message
    int Rval = 0;                                   // Function call return value

    while (1) {
        // Clean message
        SET_ZERO(beepMsg);

        // Receive decode message.
        Rval = AmpMsgQueue_Receive(&APPLIB_G_BEEP.beepMsgQueue, &beepMsg, AMBA_KAL_WAIT_FOREVER);
        if (Rval != AMP_MQ_SUCCESS) {
            DBGMSGc2(RED,"[AppLib - Audio] <_AppLibAudio_BeepTask> Receive MQ error %d", Rval);
        }

        switch (beepMsg.MessageType) {
            case APPLIB_AUDIO_BEEP_TASK_MSG_PLAY_BEEP:
                DBGMSGc2(RED,"[AppLib - Audio] <_AppLibAudio_BeepTask> APPLIB_AUDIO_BEEP_TASK_MSG_PLAY_BEEP");
                Rval = AppLibAudio_BeepPlay(beepMsg.beepInfo);
#ifdef CONFIG_APP_ARD
                AppLibAudio_Beep_Playing = 0;
#endif
                if (Rval != 0) {
                    DBGMSGc2(RED,"[AppLib - Audio] <_AppLibAudio_BeepTask> APPLIB_AUDIO_BEEP_TASK_MSG_PLAY_BEEP error %d",Rval);
                }
                break;
            default:
                break;
        }
    }
}

static int _AppLibAudio_DeinitTask(void)
{
    int ReturnValue = 0; // Success
    int Rval = 0; // Function call return

    DBGMSG("[AppLib - Audio] <_AppLibAudio_DeinitTask> Start");

    /** Step 1: Reset flag */
    APPLIB_G_BEEP.IsInit = 0;

    /** Step 2: Delete task */
    Rval = AmbaKAL_TaskTerminate(&(APPLIB_G_BEEP.beepTask));
    // TX_THREAD_ERROR: The task is not created.
    if ((Rval != AMP_OK) && (Rval != TX_THREAD_ERROR)) { // TODO: Wait DSP definition of TX_THREAD_ERROR
        DBGMSG("[AppLib - Audio] <_AppLibAudio_DeinitTask> %s:%u Failed to terminate task (%d).", __FUNCTION__, __LINE__, Rval);
        ReturnValue = -1; // Error
    }
    Rval = AmbaKAL_TaskDelete(&(APPLIB_G_BEEP.beepTask));
    // TX_THREAD_ERROR: The task is not created.
    // TX_DELETE_ERROR: The task is not terminated.
    if ((Rval != AMP_OK) && (Rval != TX_THREAD_ERROR) && (Rval != TX_DELETE_ERROR)) { // TODO: Wait DSP definition of TX_THREAD_ERROR and TX_DELETE_ERROR
        DBGMSG("[AppLib - Audio] <_AppLibAudio_DeinitTask> %s:%u Failed to delete task (%d).", __FUNCTION__, __LINE__, Rval);
        ReturnValue = -1; // Error
    }

    /** Step 3: Release stack */
    if (beepStackRaw != NULL) {
        Rval = AmbaKAL_BytePoolFree(beepStackRaw);
        if (Rval != AMP_OK) {
            DBGMSG("[AppLib - Audio] <_AppLibAudio_DeinitTask> %s:%u Failed to release stack (%d).", __FUNCTION__, __LINE__, Rval);
            ReturnValue = -1; // Error
        }
        beepStackRaw = NULL;
        beepStack = NULL;
    }

    /** Step 4: Delete message queue */
    if (AmpMsgQueue_IsCreated(&APPLIB_G_BEEP.beepMsgQueue)) {
        Rval = AmpMsgQueue_Delete(&APPLIB_G_BEEP.beepMsgQueue);
        if (Rval != AMP_MQ_SUCCESS) {
            DBGMSG("[AppLib - Audio] <_AppLibAudio_DeinitTask> %s:%u Failed to delete message queue (%d).", __FUNCTION__, __LINE__, Rval);
            ReturnValue = -1; // Error
        }
    }

    /** Step 5: Release message pool */
    if (beepMsgPoolRaw != NULL) {
        if (AmbaKAL_BytePoolFree(beepMsgPoolRaw) != AMP_OK) {
            DBGMSG("[AppLib - Audio] <_AppLibAudio_DeinitTask> %s:%u Failed to release message pool.", __FUNCTION__, __LINE__);
            ReturnValue = -1; // Error
        }
        beepMsgPoolRaw = NULL;
        APPLIB_G_BEEP.beepMsgPool = NULL;
    }

    DBGMSG("[AppLib - Audio] <_AppLibAudio_DeinitTask> End");
    return ReturnValue;
}


static int _AppLibAudio_BeepTaskInit(void)
{
    int Rval = 0;

    DBGMSG("[AppLib - Audio] <_AppLibAudio_BeepTaskInit> Start");

    /** Step 1: Create message pool for message queue */
    if (beepMsgPoolRaw == NULL) {
        Rval = AmpUtil_GetAlignedPool(
                APPLIB_G_MMPL,
                (void**) &(APPLIB_G_BEEP.beepMsgPool),
                &beepMsgPoolRaw,
                (sizeof(APPLIB_AUDIO_BEEP_TASK_MSG_s) * APPLIB_AUDIO_BEEP_TASK_MSG_NUM),
                1 << 5 // Align 32
            );
        if (Rval != AMP_OK) {
            DBGMSGc2(RED, "[AppLib - Audio] <_AppLibAudio_BeepTaskInit> %s:%u Failed to allocate memory (%d).", __FUNCTION__, __LINE__, Rval);
            goto ReturnError;
        }
    }

    /** Step 2: Create message queue */
    if (AmpMsgQueue_IsCreated(&APPLIB_G_BEEP.beepMsgQueue) == 0) {
        Rval = AmpMsgQueue_Create(
                &(APPLIB_G_BEEP.beepMsgQueue),
                APPLIB_G_BEEP.beepMsgPool,
                sizeof(APPLIB_AUDIO_BEEP_TASK_MSG_s),
                APPLIB_AUDIO_BEEP_TASK_MSG_NUM
            );
        if (Rval != AMP_MQ_SUCCESS) {
            DBGMSGc2(RED, "[AppLib - Audio] <_AppLibAudio_BeepTaskInit> %s:%u Failed to create message queue (%d).", __FUNCTION__, __LINE__, Rval);
            goto ReturnError;
        }
    }

    /** Step 3: Create stack for task */
    if (beepStackRaw == NULL) {
        Rval = AmpUtil_GetAlignedPool(
                APPLIB_G_MMPL,
                &beepStack,
                &beepStackRaw,
                AUDIO_BEEP_TASK_STACK_SIZE,
                1 << 5 // Align 32
            );
        if (Rval != AMP_OK) {
            DBGMSGc2(RED, "[AppLib - Audio] <_AppLibAudio_BeepTaskInit> %s:%u Failed to allocate memory (%d).", __FUNCTION__, __LINE__, Rval);
            goto ReturnError;
        }
    }

    /** Step 4: Create task */
    Rval = AmbaKAL_TaskCreate(
            &(APPLIB_G_BEEP.beepTask), // pTask
            "_AppLibAudio_BeepTask", // pTaskName
            AUDIO_BEEP_PRIORITY, // Priority
            _AppLibAudio_BeepTask, // void (*EntryFunction)(UINT32)
            0x0, // EntryArg
            beepStack, // pStackBase
            AUDIO_BEEP_TASK_STACK_SIZE, // StackByteSize
            AMBA_KAL_AUTO_START // AutoStart
        );
    if (Rval != AMP_OK) {
        DBGMSGc2(RED, "[AppLib - Audio] <_AppLibAudio_BeepTaskInit> %s:%u Failed to create task (%d).", __FUNCTION__, __LINE__, Rval);
        goto ReturnError;
    }

    /** Step 5: Set flag */
    APPLIB_G_BEEP.IsInit = 1;

    DBGMSG("[AppLib - Audio] <_AppLibAudio_BeepTaskInit> End");
    return 0; // Success

ReturnError:
    _AppLibAudio_DeinitTask();
    DBGMSG("[AppLib - Audio] <_AppLibAudio_BeepTaskInit> End");
    return -1; // Error
}

static UINT32* _AppLibAudio_CreateAudioOutputCtrl(void)
{
    static UINT32* AudOutCtrl = NULL;
    AMBA_AUDIO_IO_CREATE_INFO_s IOInfo = { 0 };
    UINT32 SizeOutCacheCtrl = 0;
    UINT32 SizeOutNCCtrl = 0;
    AMBA_AUDIO_BUF_INFO_s CachedInfo = { 0 };
    AMBA_AUDIO_BUF_INFO_s NonCachedInfo = { 0 };

    if (AudOutCtrl != NULL ) {
        return AudOutCtrl;
    }

    IOInfo.I2sIndex = 0;
    IOInfo.MaxChNum = 2;
    IOInfo.MaxDmaDescNum = 16;
    IOInfo.MaxDmaSize = 256;
    IOInfo.MaxSampleFreq = 48000;
    SizeOutCacheCtrl = AmbaAudio_OutputCachedSizeQuery(&IOInfo);
    SizeOutNCCtrl = AmbaAudio_OutputNonCachedSizeQuery(&IOInfo);

    AmbaKAL_MemAllocate(APPLIB_G_MMPL, &AudCtrlCacheBuf, SizeOutCacheCtrl, 1 << 5);
    AmbaKAL_MemAllocate(APPLIB_G_NC_MMPL, &AudCtrlNonCacheBuf, SizeOutNCCtrl, 1 << 5);

    CachedInfo.MaxSize = (SizeOutCacheCtrl);
    CachedInfo.pHead = AudCtrlCacheBuf.pMemAlignedBase;
    NonCachedInfo.MaxSize = (SizeOutNCCtrl);
    NonCachedInfo.pHead = AudCtrlNonCacheBuf.pMemAlignedBase;

    DBGMSG("Out Ctrl Mem: 0x%x 0x%x", (UINT32) CachedInfo.pHead, (UINT32) NonCachedInfo.pHead);
    AudOutCtrl = AmbaAudio_OutputCreate(&IOInfo, &CachedInfo, &NonCachedInfo);

    return AudOutCtrl;
}

/**
* To init beep module and load beep in romfs into memory
*
* @return 0 - OK, others - failure
*/
int AppLibAudio_BeepInit(void)
{
    static UINT8 _init = 0;
    AMP_BEEP_INIT_CFG_t Cfg = {0};
    AMBA_AUDIO_TASK_CREATE_INFO_s DecInfo = {0};
    AMBA_ABU_CREATE_INFO_s AbuInfo = {0};
    UINT32 DecSize = 0;
    UINT32 AbuSize = 0;

    if (!_init) {
#ifndef CONFIG_APP_ARD
        AmpAudio_OutputInit(_AppLibAudio_CreateAudioOutputCtrl(), AUDIO_OUTPUT_TASK_PRIORITY);
#endif
        Cfg.SrcChannelMode = 2;
        Cfg.MaxChannelNum = 2;
        Cfg.MaxFrameSize = 4096;
        Cfg.MaxSampleRate = 48000;
#ifdef CONFIG_APP_ARD
        Cfg.MaxChunkNum = 32;
#else
        Cfg.MaxChunkNum = 16;
#endif
        Cfg.I2SIndex = 0;

        DecInfo.MaxSampleFreq = Cfg.MaxSampleRate;
        DecInfo.MaxChNum = Cfg.MaxChannelNum;
        DecInfo.MaxFrameSize = Cfg.MaxFrameSize;
        DecSize = AmbaAudio_DecSizeQuery(&DecInfo);

        AbuInfo.MaxSampleFreq = Cfg.MaxSampleRate;
        AbuInfo.MaxChNum = Cfg.MaxChannelNum;
        AbuInfo.MaxChunkNum = Cfg.MaxChunkNum;
        AbuSize = AmbaAudio_BufferSizeQuery(&AbuInfo);

        AmbaKAL_BytePoolAllocate(APPLIB_G_MMPL, &BeepCodecBuf, DecSize + AbuSize + 64, 100);

        Cfg.CodecCacheWorkBuff = BeepCodecBuf;
        Cfg.CodecCacheWorkSize = DecSize + AbuSize + 64;
        Cfg.PcmCfg.BitsPerSample = 16;
        Cfg.PcmCfg.DataFormat = 0;
        Cfg.PcmCfg.FrameSize = 1024;
        AmpBeep_Init(&Cfg);

        _AppLibAudio_BeepTaskInit();
        _init = 1;
    }

    return 0;
}

static int ReadDataFromRom(char *Filename, UINT8 **DataBuf, UINT8 **DataBufRaw, int *RetFileSize)
{
    int ReturnValue = 0;
    int FileSize = 0;
    UINT8 *Tmpbuf = NULL;
    UINT8 *TmpbufRaw = NULL;

    if ((!Filename) || (!DataBuf) || (!DataBufRaw) || (!RetFileSize)) {
        AmbaPrint("[AppLib - AudioBeep] <BeepPlayWithRomFile> invalid param. (Filename = 0x%x, DataBuf = 0x%x, DataBufRaw = 0x%x, RetFileSize = 0x%x)",
                                (UINT32)Filename, (UINT32)DataBuf, (UINT32)DataBufRaw, (UINT32)RetFileSize);
        return -1;
    }

    *DataBuf = NULL;
    *DataBufRaw = NULL;
    *RetFileSize = 0;

    AmbaPrint("[AppLib - AudioBeep] <ReadDataFromRom> Filename = %s", Filename);
    ReturnValue = AmbaROM_FileExists(AMBA_ROM_SYS_DATA, Filename);
    if (ReturnValue != 1) {
        AmbaPrintColor(RED,"[AppLib - AudioBeep] <ReadDataFromRom> %s is not exist.", Filename);
        return -1;
    }

    FileSize = AmbaROM_GetSize(AMBA_ROM_SYS_DATA, Filename, 0x0);
    if (FileSize <= 0) {
        AmbaPrintColor(RED,"[AppLib - AudioBeep] <ReadDataFromRom> FileSize (%d) <= 0 ", FileSize);
        return -1;
    } else {
        AmbaPrintColor(GREEN,"[AppLib - AudioBeep] <ReadDataFromRom> FileSize = %d.",FileSize);
        ReturnValue = AmpUtil_GetAlignedPool(APPLIB_G_MMPL, (void **)&Tmpbuf, (void **)&TmpbufRaw, FileSize, 32);
        if (ReturnValue < 0) {
            AmbaPrintColor(RED,"[AppLib - AudioBeep] <ReadDataFromRom> Memory fail.");
            return -1;
        }
    }
    ReturnValue = AmbaROM_LoadByName(AMBA_ROM_SYS_DATA, Filename, Tmpbuf, FileSize, 0);
    AmbaPrint("[AppLib - AudioBeep] <ReadDataFromRom> ReturnValue = %d", ReturnValue);
    if (ReturnValue == FileSize) {
        *RetFileSize = FileSize;
        *DataBuf = Tmpbuf;
        *DataBufRaw = TmpbufRaw;

        #if 0 //debug
        {
            AMBA_FS_FILE *File = NULL;
            char TempFn[] = "C:\\save.pcm";
            char mode[3] = {'w','b','\0'};
            File = AmbaFS_fopen((char const *)TempFn,(char const *) mode);
            AmbaFS_fwrite(Tmpbuf, FileSize, 1, File);
            AmbaFS_FSync(File);
            AmbaFS_fclose(File);
        }
        #endif

    } else {
        AmbaPrintColor(RED,"[AppLib - AudioBeep] <ReadDataFromRom> AmbaROM_LoadByName() fail.");
        AmbaKAL_BytePoolFree(TmpbufRaw);
        return -1;
    }

    return 0;
}
/**
* To output beep sound
*
* @param [in] beep - beep sound info
*
* @return 0 - OK, others - failure
*/
int AppLibAudio_BeepPlay(APPLIB_BEEP_s beep)
{
    int Rval = 0;
    UINT8 *DataBuf = NULL;
    UINT8 *DataBufRaw = NULL;
    int RetFileSize = 0;
    AMP_BEEP_t BeepInfo = { 0 };

    switch (beep.BeepType) {
        case BEEP_FROM_FILE:
            Rval = ReadDataFromRom(beep.Fn, &DataBuf, &DataBufRaw, &RetFileSize);
            if (Rval != 0) {
                return -1;
            } else {
                BeepInfo.Fn = NULL;
                BeepInfo.RawPcm = DataBuf;
                BeepInfo.RawSize = RetFileSize;
                BeepInfo.SrcChannelMode = beep.SrcChannelMode;
                BeepInfo.SrcSampleRate = beep.SrcSampleRate;
                BeepInfo.DstSampleRate = beep.DstSampleRate;
            }
            break;
        case BEEP_FROM_INDEX:
            //TODO :
            break;
        case BEEP_FROM_USER:
            BeepInfo.Fn = beep.Fn;
            BeepInfo.RawPcm = beep.RawPcm;
            BeepInfo.RawSize = beep.RawSize;
            BeepInfo.SrcChannelMode = beep.SrcChannelMode;
            BeepInfo.SrcSampleRate = beep.SrcSampleRate;
            BeepInfo.DstSampleRate = beep.DstSampleRate;
            break;
    }

    Rval = AmpBeep_Beep(&BeepInfo);

    if (DataBufRaw) {
        AmbaKAL_BytePoolFree(DataBufRaw);
    }

    return Rval;
}
