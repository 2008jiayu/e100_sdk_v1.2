/**
 * @file src/app/connected/applib/src/recorder/ApplibRecorder_Audioenc
 *
 * Implementation of Aduio encoding config APIs
 *
 * History:
 *    2013/11/16 - [Martin Lai] created file
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
#include <AmbaAudio.h>
#include <AmbaUtility.h>
#include <recorder/Encode.h>
#include <recorder/AudioEnc.h>
#include <fifo/Fifo.h>
#include "../AppLibTask_Priority.h"
//#define DEBUG_APPLIB_AUDIO_ENC
#if defined(DEBUG_APPLIB_AUDIO_ENC)
#define DBGMSG AmbaPrint
#define DBGMSGc(x) AmbaPrintColor(GREEN,x)
#define DBGMSGc2 AmbaPrintColor
#else
#define DBGMSG(...)
#define DBGMSGc(...)
#define DBGMSGc2(...)
#endif


/**
 * audio encode stream setting
 */
 typedef struct _APPLIB_AUDIOENC_STREAM_SETTING_s_ {
    AMBA_AUDIO_TYPE_e EncType;   /**< Encode type */
    UINT32 SrcSampleRate;                   /**< Sample frequency of the input */
    UINT32 DstSampleRate;                   /**< Sample frequency of the output */
    UINT16 SrcChannelMode;                  /**< Channel mode of the input */
    UINT16 DstChannelMode;                  /**< Channel mode of the output */
    union {
        AMBA_AUDIO_PCM_CONFIG_s PCMCfg;
        AMBA_AUDIO_AACENC_CONFIG_s AACCfg;
    } Spec; /**< Audio Encode Specification */
} APPLIB_AUDIOENC_STREAM_SETTING_s;

/**
 * audio encode setting
 */
typedef struct _APPLIB_AUDIOENC_SETTING_s_ {
    UINT32 EncType; /**< Bit Rate */
    UINT16 DualStreams; /**< DualStreams */
#define AUDIO_DUAL_STREAMS_OFF  (0) /**< AUTO DUAL STREAMS OFF */
#define AUDIO_DUAL_STREAMS_ON   (1) /**< AUTO DUAL STREAMS ON */
} APPLIB_AUDIOENC_SETTING_s;

typedef enum _APPLIB_AUDIOENC_STREAM_SETTING_ID_e_ {
    AUDIO_STREAM_PRIMARY = 0,
    AUDIO_STREAM_SECONDARY,
    AUDIO_STREAM_SETTINGS_NUM
} APPLIB_AUDIOENC_STREAM_SETTING_ID_e;
static APPLIB_AUDIOENC_SETTING_s ApplibAudioEncAudioSetting = {0};
#if 0
static APPLIB_AUDIOENC_STREAM_SETTING_s ApplibAudioEncAudioStreamSettingTable[AUDIO_STREAM_SETTINGS_NUM] = {
    {AMBA_AUDIO_AAC, 48000, 48000, 2, 2, {AAC_BS_ADTS, 128000}}, //PRIMARY_PRESET_1
    {AMBA_AUDIO_PCM, 48000, 48000, 2, 2, {16, 0, 1024}} //PRIMARY_PRESET_2
};
#endif
/**audio default setting, enable PCM && AAC only*/
static APPLIB_AUDIOENC_STREAM_SETTING_s ApplibAudioEncAudioStreamDefaultSettingTable[2] = {
    {AMBA_AUDIO_PCM, 48000, 48000, 2, 1, {16, 0, 1024}},
    {AMBA_AUDIO_AAC, 48000, 48000, 2, 1, {AAC_BS_RAW, 128000}}
};


#ifdef CONFIG_APP_ARD
#include <stream/File.h>
#define ARD_AUDIO_ENC_TYPE  AUDIO_TYPE_AAC
#define AUDIO_SPLIT_TIME    (60)  //seconds
#define AUDIO_SPLIT_FRAME    ((AUDIO_SPLIT_TIME*48000)/1024)

static AMBA_KAL_SEM_t ApplibAudioEncPriSem = {0};
static UINT8 *ApplibAudioEncPriStack;
static void *ApplibAudioEncPriStackRaw;
static AMBA_KAL_TASK_t ApplibAudioEncPriMuxTask = {0};
static int _au_mux_tsk_init = 0;
static AMP_STREAM_HDLR_s *ApplibAudioRecStreamPri = NULL;
static char ApplibAudioFileName[MAX_FILENAME_LENGTH]={0};
static int SoundRecording = 0;
static UINT32 ApplibAudioFrame = 0;

int AppLibAudioEnc_CreateFileName(void);
int AppLibAudioEnc_SetAudioType(UINT8 IsAvRecording);

static APPLIB_AUDIOENC_STREAM_SETTING_s ApplibAudioEncAudioStreamDefaultSettingTableForSoundsRecording[2] = {
    {AMBA_AUDIO_PCM, 48000, 48000, 2, 1, {16, 0, 1024}},
    {AMBA_AUDIO_AAC, 48000, 48000, 2, 1, {AAC_BS_ADTS, 128000}}
};
#endif
/**
 *  @brief Set encode type.
 *
 *  Set encode type.
 *
 *  @param [in] enctype encode type
 *                  0:PCM
 *                  1:AAC
 *
 *  @return >=0 success, <0 failure
 */
int AppLibAudioEnc_SetEncType(int enctype)
{
    ApplibAudioEncAudioSetting.EncType= enctype;
    return 0;
}

/**
 *  @brief Get encode type.
 *
 *  Get encode type.
 *
 *  @return Encode type
 *
 */
UINT32 AppLibAudioEnc_GetEncType(void)
{
    return ApplibAudioEncAudioSetting.EncType;
}


/**
 *  @brief Modify encode type source sample rate.
 *
 *  Modify encode type source sample rate.
 *
 *  @param [in] samplerate sample rate
 *
 *  @return >=0 success, <0 failure
 */
int AppLibAudioEnc_SetSrcSampleRate(int samplerate)
{
    ApplibAudioEncAudioStreamDefaultSettingTable[ApplibAudioEncAudioSetting.EncType].SrcSampleRate= samplerate;
    return 0;
}

/**
 *  @brief Modify encode type destination sample rate.
 *
 *  Modify encode type destination sample rate.
 *
 *  @param [in] samplerate sample rate
 *
 *  @return >=0 success, <0 failure
 */
int AppLibAudioEnc_SetDstSampleRate(int samplerate)
{
    ApplibAudioEncAudioStreamDefaultSettingTable[ApplibAudioEncAudioSetting.EncType].DstSampleRate= samplerate;
    return 0;
}

/**
 *  @brief Get encode type source sample rate.
 *
 *  Get encode type source sample rate.
 *
 *
 *  @return Source sample rate
 */
UINT32 AppLibAudioEnc_GetSrcSampleRate(void)
{
    return ApplibAudioEncAudioStreamDefaultSettingTable[ApplibAudioEncAudioSetting.EncType].SrcSampleRate;
}

/**
 *  @brief Get encode type source channel mode.
 *
 *  Get encode type source channel mode.
 *
 *
 *  @return source channel mode
 */
UINT32 AppLibAudioEnc_GetSrcChanMode(void)
{
    return ApplibAudioEncAudioStreamDefaultSettingTable[ApplibAudioEncAudioSetting.EncType].SrcChannelMode;
}

/**
 *  @brief Get encode type destination channel mode.
 *
 *  Get encode type destination channel mode.
 *
 *
 *  @return destination channel mode
 */
UINT32 AppLibAudioEnc_GetDstChanMode(void)
{
    return ApplibAudioEncAudioStreamDefaultSettingTable[ApplibAudioEncAudioSetting.EncType].DstChannelMode;
}


/**
 *  @brief Set the setting of audio encoding bit rate
 *
 *  Set the setting of audio encoding bit rate
 *
 *  @return >=0 success, <0 failure
 */
int AppLibAudioEnc_SetBitrate(int bitRate)
{
    ApplibAudioEncAudioStreamDefaultSettingTable[ApplibAudioEncAudioSetting.EncType].Spec.AACCfg.Bitrate= bitRate;
    return 0;
}

/**
 *  @brief Get the setting of audio encoding bit rate
 *
 *  Get the setting of audio encoding bit rate
 *
 *  @return >=0 success, <0 failure
 */
int AppLibAudioEnc_GetBitate(void)
{
    return ApplibAudioEncAudioStreamDefaultSettingTable[ApplibAudioEncAudioSetting.EncType].Spec.AACCfg.Bitrate;
}

/**
 *  @brief Set the setting of audio dual streams.
 *
 *  Set the setting of audio dual streams.
 *
 *  @param [in] dualStreams Dual streams setting.
 *
 *  @return >=0 success, <0 failure
 */
int AppLibAudioEnc_SetDualStreams(int dualStreams)
{
    ApplibAudioEncAudioSetting.DualStreams = dualStreams;
    return 0;
}

/**
 *  @brief Get the setting of audio dual streams.
 *
 *  Get the setting of audio dual streams.
 *
 *  @return >=0 success, <0 failure
 */
int AppLibAudioEnc_GetDualStreams(void)
{
    return ApplibAudioEncAudioSetting.DualStreams;
}

#define AUDENC_BITSFIFO_SIZE (10<<20) //10MB
UINT8 *AudPriBitsBuf;
static void *AudPriBitsBufRaw;
UINT8 *AudSecBitsBuf;
static void *AudSecBitsBufRaw;
#define AUDENC_DESC_SIZE 40*(1024)*10
static UINT8 *AudPriDescBuf;
static void *AudPriDescBufRaw;
static UINT8 *AudSecDescBuf;
static void *AudSecDescBufRaw;
// global var for AudioEnc
AMP_AVENC_HDLR_s *AudioEncPriHdlr = NULL;
AMP_AVENC_HDLR_s *AudioEncSecHdlr = NULL;
static AMP_ENC_PIPE_HDLR_s *AudioEncPipe = NULL;


static UINT8 *AudworkNCBuf;
static UINT8 *AudworkNCBufRaw;
static UINT8 *AudworkCacheBuf;
static UINT8 *AudworkCacheBufRaw;


// static for multi-stream encode
static AMP_AUDIOENC_INSTANCE_s AudioEncInstance[2] = {0};
static AMP_AUDIOENC_INSTANCE_s AudioEncSecInstance[2] = {0};
#define AUDENC_MULTI_AIN    0x1
#define AUDENC_MULTI_STREAM 0x2

static int ApplibAudioEncLiveviewInitFlag = -1;


static UINT8 *AppLibAudioEncMem = NULL;
static void *AppLibAudioEncMemBufRaw = NULL;
/**
 *  @brief Initial the video encoder.
 *
 *  Initial the video encoder.
 *
 *  @return >=0 success, <0 failure
 */
int AppLibAudioEnc_Init(void)
{
    int ReturnValue = 0;

    // Init AUDIOENC module
    if (ApplibAudioEncLiveviewInitFlag < 0) {
        AMP_AUDIOENC_INIT_CFG_s EncInitCfg;
        memset(&EncInitCfg, 0x0, sizeof(AMP_AUDIOENC_INIT_CFG_s));

        AmpAudioEnc_GetInitDefaultCfg(&EncInitCfg);
        if (AppLibAudioEncMem == NULL) {
            ReturnValue = AmpUtil_GetAlignedPool(APPLIB_G_MMPL, (void **)&AppLibAudioEncMem, &AppLibAudioEncMemBufRaw, EncInitCfg.MemoryPoolSize, 32);
            if (ReturnValue != OK) {
                AmbaPrint("[Applib - AudioEnc] <Init> Out of memory for audio!!");
                return ReturnValue;
            }
        }
        EncInitCfg.MemoryPoolAddr = AppLibAudioEncMem;
        ReturnValue = AmpAudioEnc_Init(&EncInitCfg);
        ApplibAudioEncLiveviewInitFlag = 0;
    }

    return ReturnValue;
}



/**
 *  @brief Codec Callback function
 *
 *  Codec Callback function
 *
 *  @param [in] hdlr Handler
 *  @param [in] event Event
 *  @param [in] info Infomation
 *
 *  @return >=0 success, <0 failure
 */
static int AppLibAudioEnc_AudioEncCallback(void *hdlr,UINT32 event, void *info)
{
    switch (event) {
    case AMP_ENC_EVENT_DATA_OVER_RUNOUT_THRESHOLD:
        AmbaPrint("[Applib - AudioEnc] <AudioEncCallback> AMP_ENC_EVENT_DATA_OVER_RUNOUT_THRESHOLD !");
        AppLibComSvcHcmgr_SendMsgNoWait(HMSG_MEMORY_FIFO_BUFFER_RUNOUT, 0, 0);
        break;
    case AMP_ENC_EVENT_DATA_OVERRUN:
        DBGMSG("[Applib - AudioEnc] <AudioEncCallback> AMP_ENC_EVENT_DATA_OVERRUN !");
        break;
    case AMP_ENC_EVENT_DESC_OVERRUN:      /**< buffer data overrun */
        AmbaPrint("[Applib - AudioEnc] <AudioEncCallback> AMP_ENC_EVENT_DESC_OVERRUN");
        break;
    default:
        AmbaPrint("[Applib - AudioEnc] <AudioEncCallback> Unknown event %X info: %x", event, info);
        break;
    }
    return 0;
}


/* add audio encode task, which share same audio encode engine */
/* flag definition */
#define AUDIOENC_TASK_ADD_ALL           0x0
#define AUDIOENC_TASK_ADD_EXCLUDE_AIN   0x1
#define AUDIOENC_TASK_ADD_EXCLUDE_AENC  (0x1<<1)
int AppLibAudioEnc_TaskAdd(UINT8 *workCacheBuf, UINT8 *workNCBuf, UINT32 cacheSize, UINT32 NCacheSize, UINT32 *inputHdlr, UINT32 *encodeHdlr, UINT32 *encBuffHdlr, UINT8 flag)
{

    AMBA_AUDIO_IO_CREATE_INFO_s inputInfo;
    AMBA_AUDIO_TASK_CREATE_INFO_s encInfo;
    AMBA_ABU_CREATE_INFO_s abuInfo;
    AMBA_AUDIO_BUF_INFO_s inC, inNonC;
    AMBA_AUDIO_COMBINE_INFO_s combine;
    UINT32 inputCachedSize = 0, inputNonCachedSize = 0, encSize = 0, abuSize = 0;
    UINT8 *Caddr = NULL, *NCaddr = NULL, *CaddrEnd = NULL, *NCaddrEnd = NULL;
    UINT32 *encAddr = NULL, *abuAddr = NULL;
    UINT32 *retHdlr = NULL;

    memset(&inputInfo, 0x0, sizeof(AMBA_AUDIO_IO_CREATE_INFO_s));
    memset(&encInfo, 0x0, sizeof(AMBA_AUDIO_TASK_CREATE_INFO_s));
    memset(&abuInfo, 0x0, sizeof(AMBA_ABU_CREATE_INFO_s));
    memset(&inC, 0x0, sizeof(AMBA_AUDIO_BUF_INFO_s));
    memset(&inNonC, 0x0, sizeof(AMBA_AUDIO_BUF_INFO_s));
    memset(&combine, 0x0, sizeof(AMBA_AUDIO_COMBINE_INFO_s));

    Caddr = (UINT8 *) ALIGN_4((UINT32)workCacheBuf);
    NCaddr = (UINT8 *) ALIGN_8((UINT32)workNCBuf);
    CaddrEnd = Caddr + cacheSize;
    NCaddrEnd = NCaddr + NCacheSize;

    if (flag&AUDIOENC_TASK_ADD_EXCLUDE_AIN) {
        //share ain
    } else {
        inputInfo.I2sIndex = 0; // depends on project?
        inputInfo.MaxChNum = 2;
        inputInfo.MaxDmaDescNum = 16; // depends on project?
        inputInfo.MaxDmaSize = 1024; // depends on project?
        inputInfo.MaxSampleFreq = 48000;
        inputCachedSize = AmbaAudio_InputCachedSizeQuery(&inputInfo);
        inputNonCachedSize = AmbaAudio_InputNonCachedSizeQuery(&inputInfo);
        inC.pHead = (UINT32 *)Caddr;
        inC.MaxSize = inputCachedSize;
        Caddr += inputCachedSize;
        if (Caddr > CaddrEnd) {
            AmbaPrintColor(RED,"[Applib - AudioEnc] <TaskAdd> AIN Input Cache buffer fail");
        }

        inNonC.pHead = (UINT32 *)NCaddr;
        inNonC.MaxSize = inputNonCachedSize;
        NCaddr += inputNonCachedSize;
        if (NCaddr > NCaddrEnd) {
            AmbaPrintColor(RED,"[Applib - AudioEnc] <TaskAdd> AIN Input NonCache buffer fail");
        }
        retHdlr = AmbaAudio_InputCreate(&inputInfo, &inC, &inNonC);
        if ((int)retHdlr == NG) {
            AmbaPrintColor(RED,"[Applib - AudioEnc] <TaskAdd> AIN cre fail");
        } else *inputHdlr = (UINT32)retHdlr;
    }

    if (flag&AUDIOENC_TASK_ADD_EXCLUDE_AENC) {
        //share aenc
    } else {
        encInfo.MaxSampleFreq = 48000;
        encInfo.MaxChNum = 2;
        encInfo.MaxFrameSize = 2048;
        encSize = AmbaAudio_EncSizeQuery(&encInfo);
        Caddr = (UINT8 *) ALIGN_4((UINT32)Caddr);
        encAddr = (UINT32 *)Caddr;
        Caddr += encSize;
        if (Caddr > CaddrEnd) {
            AmbaPrintColor(RED,"[Applib - AudioEnc] <TaskAdd> AENC buffer fail");
        }
        retHdlr = AmbaAudio_EncCreate(&encInfo, encAddr, encSize);
        if ((int)retHdlr == NG) {
            AmbaPrintColor(RED,"[Applib - AudioEnc] <TaskAdd> AENC cre fail");
        } else *encodeHdlr = (UINT32)retHdlr;
    }

    abuInfo.MaxSampleFreq = 48000;
    abuInfo.MaxChNum = 2;
    abuInfo.MaxChunkNum = 16; // depends on project?
    abuSize = AmbaAudio_BufferSizeQuery(&abuInfo);
    Caddr = (UINT8 *) ALIGN_4((UINT32)Caddr);
    abuAddr = (UINT32 *)Caddr;
    Caddr += abuSize;
    if (Caddr > CaddrEnd) {
        AmbaPrintColor(RED,"[Applib - AudioEnc] <TaskAdd> ABU buffer fail");
    }
    retHdlr = AmbaAudio_BufferCreate(&abuInfo, abuAddr, abuSize);
    if ((INT32)retHdlr == NG) {
        AmbaPrintColor(RED,"[Applib - AudioEnc] <TaskAdd> ABU cre fail");
    } else *encBuffHdlr = (UINT32)retHdlr;

    combine.pAbu = (UINT32 *)(*encBuffHdlr);
    combine.pSrcApu = (UINT32 *)(*inputHdlr);
    combine.pDstApu = (UINT32 *)(*encodeHdlr);
    if (AmbaAudio_Combine(&combine) != OK) {
        AmbaPrintColor(RED,"[Applib - AudioEnc] <TaskAdd> ACOMB fail");
    }

    return 0;
}

/**
 *  @brief Setup audio encoding parameter.
 *
 *  Setup audio encoding parameter.
 *
 *  @return >=0 success, <0 failure
 */
int AppLibAudioEnc_Setup(void)
{
    int ReturnValue = 0;
    AMP_AUDIOENC_HDLR_CFG_s EncCfg[2];
    UINT32 Csize = 0, NCsize = 0;
    UINT32 retInputHdlr = 0, retEncodeHdlr = 0, retEncBuffHdlr = 0;
    UINT32 EncType = AUDIO_TYPE_AAC;
    UINT8 audioEncMultiAin = 0;//(AudioMultiMode&AUDENC_MULTI_AIN)? 1: 0;

    memset(&EncCfg, 0x0, 2*sizeof(AMP_AUDIOENC_HDLR_CFG_s));

    EncType = ApplibAudioEncAudioSetting.EncType;

    if (AudioEncPriHdlr == NULL) {
        // Create Audio encoder object
        AmpAudioEnc_GetDefaultCfg(&EncCfg[0]);

        // Encoder setup
        EncCfg[0].EncType = ApplibAudioEncAudioStreamDefaultSettingTable[EncType].EncType;
        EncCfg[0].SrcChannelMode =  ApplibAudioEncAudioStreamDefaultSettingTable[EncType].SrcChannelMode; // currently audio_lib treat this as channel number
        EncCfg[0].SrcSampleRate =  ApplibAudioEncAudioStreamDefaultSettingTable[EncType].SrcSampleRate;
        EncCfg[0].DstChannelMode =  ApplibAudioEncAudioStreamDefaultSettingTable[EncType].DstChannelMode; // currently audio_lib treat this as channel number
        EncCfg[0].DstSampleRate =  ApplibAudioEncAudioStreamDefaultSettingTable[EncType].DstSampleRate;
        if (EncType == AUDIO_TYPE_PCM) {
            EncCfg[0].Spec.PCMCfg = ApplibAudioEncAudioStreamDefaultSettingTable[EncType].Spec.PCMCfg;
        } else {
            EncCfg[0].Spec.AACCfg =  ApplibAudioEncAudioStreamDefaultSettingTable[EncType].Spec.AACCfg;
        }

        // Task priority, Input task should have higher priority than encode task
        EncCfg[0].EncoderTaskPriority = APPLIB_AUDIO_ENC_TASK_PRIORITY;
        EncCfg[0].InputTaskPriority = APPLIB_AUDIO_INPUT_TASK_PRIORITY;
        EncCfg[0].EncoderTaskCoreSelection = 1;
        EncCfg[0].InputTaskCoreSelection = 1;
        EncCfg[0].EventDataReadySkipNum = 0;
        EncCfg[0].FadeInTime = 0;
        EncCfg[0].FadeOutTime = 0;

        // Assign callback
        EncCfg[0].cbEvent = AppLibAudioEnc_AudioEncCallback;


        // Query working size
        {
            AMBA_AUDIO_IO_CREATE_INFO_s Input;
            AMBA_AUDIO_TASK_CREATE_INFO_s enc;
            AMBA_ABU_CREATE_INFO_s abu;

            memset(&Input, 0x0, sizeof(AMBA_AUDIO_IO_CREATE_INFO_s));
            memset(&enc, 0x0, sizeof(AMBA_AUDIO_TASK_CREATE_INFO_s));
            memset(&abu, 0x0, sizeof(AMBA_ABU_CREATE_INFO_s));

            Csize = NCsize = 0;

            // Cache need 4_align, NonCache need 8_align
            // audio Input
            Input.I2sIndex = 0; // depends on HW design
            Input.MaxChNum = 2;
            Input.MaxDmaDescNum = 16; // depends chip/project, means more buffer
            Input.MaxDmaSize = 1024; // depends chip/project, means more buffer
            Input.MaxSampleFreq = 48000;
            Csize = ALIGN_4(AmbaAudio_InputCachedSizeQuery(&Input));
            NCsize = ALIGN_8(AmbaAudio_InputNonCachedSizeQuery(&Input));

            // audio encode
            enc.MaxSampleFreq = 48000;
            enc.MaxChNum = 2;
            enc.MaxFrameSize = 2048;
            Csize += ALIGN_4(AmbaAudio_EncSizeQuery(&enc));

            abu.MaxSampleFreq = 48000;
            abu.MaxChNum = 2;
            abu.MaxChunkNum = 16; // depends preject, means more buffer
            Csize += ALIGN_4(AmbaAudio_BufferSizeQuery(&abu));
        }

        // Assign working buffer
        ReturnValue = AmpUtil_GetAlignedPool(APPLIB_G_MMPL, (void **)&AudworkCacheBuf, (void **)&AudworkCacheBufRaw, Csize*2, 32);
        if (ReturnValue != OK) {
            AmbaPrintColor(RED,"[Applib - AudioEnc] <Setup> Out of Cache memory for audio working!!");
        }
        ReturnValue = AmpUtil_GetAlignedPool(APPLIB_G_NC_MMPL, (void **)&AudworkNCBuf, (void **)&AudworkNCBufRaw, NCsize*2, 32);
        if (ReturnValue != OK) {
            AmbaPrintColor(RED,"[Applib - AudioEnc] <Setup> Out of NC memory for audio working!!");
        }



        // Assign callback
        EncCfg[0].cbEvent = AppLibAudioEnc_AudioEncCallback;

        // Assign bitstream/descriptor buffer
        ReturnValue = AmpUtil_GetAlignedPool(APPLIB_G_MMPL, (void **)&AudPriBitsBuf, &AudPriBitsBufRaw, AUDENC_BITSFIFO_SIZE, 32);
        if (ReturnValue != OK) {
            AmbaPrintColor(RED,"[Applib - AudioEnc] <Setup> Out of NC memory for bitsFifo!!");
        }
        EncCfg[0].BitsBufCfg.BitsBufAddr = AudPriBitsBuf;
        EncCfg[0].BitsBufCfg.BitsBufSize = AUDENC_BITSFIFO_SIZE;

        ReturnValue = AmpUtil_GetAlignedPool(APPLIB_G_MMPL, (void **)&AudPriDescBuf, &AudPriDescBufRaw, AUDENC_DESC_SIZE, 32);
        if (ReturnValue != OK) {
            AmbaPrintColor(RED,"[Applib - AudioEnc] <Setup> Out of NC memory for descFifo!!");
        }

        EncCfg[0].BitsBufCfg.DescBufAddr = AudPriDescBuf;
        EncCfg[0].BitsBufCfg.DescBufSize = AUDENC_DESC_SIZE;
        EncCfg[0].BitsBufCfg.BitsRunoutThreshold = AUDENC_BITSFIFO_SIZE - 2*1024*1024; // leave 2MB
        AmbaPrint("[Applib - AudioEnc] <Setup> [Pri]Bits 0x%X size %d Desc 0x%X size %d", AudPriBitsBuf, AUDENC_BITSFIFO_SIZE, AudPriDescBuf, AUDENC_DESC_SIZE);

        //Create Audio Input, Encode and EncodeBuffer resource
        if (audioEncMultiAin) {
            EncCfg[0].NumInstance = 2;
            AppLibAudioEnc_TaskAdd(AudworkCacheBuf, AudworkNCBuf, Csize, NCsize, &retInputHdlr, &retEncodeHdlr, &retEncBuffHdlr, AUDIOENC_TASK_ADD_ALL);
            AudioEncInstance[0].InputHdlr = (UINT32 *)retInputHdlr;
            AudioEncInstance[0].EncodeHdlr = (UINT32 *)retEncodeHdlr;
            AudioEncInstance[0].EncBuffHdlr = (UINT32 *)retEncBuffHdlr;

            AudworkCacheBuf += Csize;
            AudworkNCBuf += NCsize;
            AppLibAudioEnc_TaskAdd(AudworkCacheBuf, AudworkNCBuf, Csize, NCsize, &retInputHdlr, &retEncodeHdlr, &retEncBuffHdlr, AUDIOENC_TASK_ADD_EXCLUDE_AIN|AUDIOENC_TASK_ADD_EXCLUDE_AENC);
            AudioEncInstance[1].InputHdlr = AudioEncInstance[0].InputHdlr;
            AudioEncInstance[1].EncodeHdlr = AudioEncInstance[0].EncodeHdlr;
            AudioEncInstance[1].EncBuffHdlr = (UINT32 *)retEncBuffHdlr;
            EncCfg[0].AudioInstance = AudioEncInstance;
        } else {
            EncCfg[0].NumInstance = 1;
            AppLibAudioEnc_TaskAdd(AudworkCacheBuf, AudworkNCBuf, Csize, NCsize, &retInputHdlr, &retEncodeHdlr, &retEncBuffHdlr, AUDIOENC_TASK_ADD_ALL);
            AudioEncInstance[0].InputHdlr = (UINT32 *)retInputHdlr;
            AudioEncInstance[0].EncodeHdlr = (UINT32 *)retEncodeHdlr;
            AudioEncInstance[0].EncBuffHdlr = (UINT32 *)retEncBuffHdlr;
#ifdef CONFIG_APP_ARD            
            ReturnValue = AppLibAudio_EffectVolumeInstall(APPLIB_AUDIO_EFFECT_INPUT,AudioEncInstance[0].InputHdlr);
#endif
            EncCfg[0].AudioInstance = AudioEncInstance;
        }
        // Assign bitstream-specific configs
#ifdef DEBUG_APPLIB_AUDIO_ENC
        AmbaPrint("[Applib-Audio] EncCfg[0].EncType = %d ",EncCfg[0].EncType);
        AmbaPrint("[Applib-Audio] EncCfg[0].SrcChannelMode = %d ",EncCfg[0].SrcChannelMode);
        AmbaPrint("[Applib-Audio] EncCfg[0].SrcSampleRate = %d ",EncCfg[0].SrcSampleRate);
        AmbaPrint("[Applib-Audio] EncCfg[0].DstChannelMode = %d ",EncCfg[0].DstChannelMode);
        AmbaPrint("[Applib-Audio] EncCfg[0].DstSampleRate = %d ",EncCfg[0].DstSampleRate);
        AmbaPrint("[Applib-Audio] EncCfg[0].EncoderTaskPriority = %d ",EncCfg[0].EncoderTaskPriority);
        AmbaPrint("[Applib-Audio] EncCfg[0].InputTaskPriority = %d ",EncCfg[0].InputTaskPriority);
        AmbaPrint("[Applib-Audio] EncCfg[0].EventDataReadySkipNum = %d ",EncCfg[0].EventDataReadySkipNum);
        AmbaPrint("[Applib-Audio] EncCfg[0].FadeInTime = %d ",EncCfg[0].FadeInTime);
        AmbaPrint("[Applib-Audio] EncCfg[0].FadeOutTime = %d ",EncCfg[0].FadeOutTime);
        AmbaPrint("[Applib-Audio] EncCfg[0].cbEvent = %d ",EncCfg[0].cbEvent);
        AmbaPrint("[Applib-Audio] EncCfg[0].BitsBufCfg.DescBufAddr = 0x%x ",EncCfg[0].BitsBufCfg.DescBufAddr);
        AmbaPrint("[Applib-Audio] EncCfg[0].BitsBufCfg.DescBufSize = %d ",EncCfg[0].BitsBufCfg.DescBufSize);
        AmbaPrint("[Applib-Audio] EncCfg[0].BitsBufCfg.BitsRunoutThreshold = %d ",EncCfg[0].BitsBufCfg.BitsRunoutThreshold);
        AmbaPrint("[Applib-Audio] EncCfg[0].NumInstance = %d ",EncCfg[0].NumInstance);
        AmbaPrint("[Applib-Audio] EncCfg[0].AudioInstance = %d ",EncCfg[0].AudioInstance);
        AmbaAudio_SetDebugLevel(AU_DBG_AENC|AU_DBG_AENC_PROC2PROC|AU_DBG_AIN|AU_DBG_AIN_PROC2PROC);
#endif
        AmpAudioEnc_Create(&EncCfg[0], &AudioEncPriHdlr);
        }


    if (AppLibAudioEnc_GetDualStreams() && (AudioEncSecHdlr == NULL)) {
        EncCfg[1].EncType = ApplibAudioEncAudioStreamDefaultSettingTable[EncType].EncType;
        EncCfg[1].SrcChannelMode =  ApplibAudioEncAudioStreamDefaultSettingTable[EncType].SrcChannelMode; // currently audio_lib treat this as channel number
        EncCfg[1].SrcSampleRate =  ApplibAudioEncAudioStreamDefaultSettingTable[EncType].SrcSampleRate;
        EncCfg[1].DstChannelMode =  ApplibAudioEncAudioStreamDefaultSettingTable[EncType].DstChannelMode; // currently audio_lib treat this as channel number
        EncCfg[1].DstSampleRate =  ApplibAudioEncAudioStreamDefaultSettingTable[EncType].DstSampleRate;
        if (EncType == AUDIO_TYPE_PCM) {
            EncCfg[1].Spec.PCMCfg = ApplibAudioEncAudioStreamDefaultSettingTable[EncType].Spec.PCMCfg;
        } else {
            EncCfg[1].Spec.AACCfg =  ApplibAudioEncAudioStreamDefaultSettingTable[EncType].Spec.AACCfg;
        }

        EncCfg[1].EncoderTaskPriority = APPLIB_AUDIO_ENC_TASK_PRIORITY;
        EncCfg[1].InputTaskPriority = APPLIB_AUDIO_INPUT_TASK_PRIORITY;
        EncCfg[1].EncoderTaskCoreSelection = 1;
        EncCfg[1].InputTaskCoreSelection = 1;
        EncCfg[1].EventDataReadySkipNum = 0;
        EncCfg[1].FadeInTime = 0;
        EncCfg[1].FadeOutTime = 0;


        ReturnValue = AmpUtil_GetAlignedPool(APPLIB_G_MMPL, (void **)&AudSecBitsBuf, &AudSecBitsBufRaw, AUDENC_BITSFIFO_SIZE, 32);
        if (ReturnValue != OK) {
            AmbaPrintColor(RED,"[Applib - AudioEnc] <Setup> Out of NC memory for bitsFifo!!");
        }
        EncCfg[1].BitsBufCfg.BitsBufAddr = AudSecBitsBuf;
        EncCfg[1].BitsBufCfg.BitsBufSize = AUDENC_BITSFIFO_SIZE;
        ReturnValue = AmpUtil_GetAlignedPool(APPLIB_G_MMPL, (void **)&AudSecDescBuf, &AudSecDescBufRaw, AUDENC_DESC_SIZE, 32);
        if (ReturnValue != OK) {
            AmbaPrintColor(RED,"[Applib - AudioEnc] <Setup> Out of NC memory for descFifo!!");
        }

        EncCfg[1].BitsBufCfg.DescBufAddr = AudSecDescBuf;
        EncCfg[1].BitsBufCfg.DescBufSize = AUDENC_DESC_SIZE;
        EncCfg[1].BitsBufCfg.BitsRunoutThreshold = AUDENC_BITSFIFO_SIZE - 2*1024*1024; // leave 2MB
        AmbaPrint("[Applib - AudioEnc] <Setup> [Sec]Bits 0x%X size %d Desc 0x%X size %d", AudSecBitsBuf, AUDENC_BITSFIFO_SIZE, AudSecDescBuf, AUDENC_DESC_SIZE);

        //Create Audio Input, Encode and EncodeBuffer resource of 1st task
        if (audioEncMultiAin) {
            AudworkCacheBuf += Csize;
            AudworkNCBuf += NCsize;
            EncCfg[1].NumInstance = 1;
            AppLibAudioEnc_TaskAdd(AudworkCacheBuf, AudworkNCBuf, Csize, NCsize, &retInputHdlr, &retEncodeHdlr, &retEncBuffHdlr, AUDIOENC_TASK_ADD_EXCLUDE_AIN);
            AudioEncSecInstance[0].InputHdlr = AudioEncInstance[0].InputHdlr;
            AudioEncSecInstance[0].EncodeHdlr = (UINT32 *)retEncBuffHdlr;
            AudioEncSecInstance[0].EncBuffHdlr = (UINT32 *)retEncBuffHdlr;
            EncCfg[1].AudioInstance = AudioEncSecInstance;
        } else {
            EncCfg[1].NumInstance = 1;
            AudworkCacheBuf += Csize;
            AudworkNCBuf += NCsize;
            AppLibAudioEnc_TaskAdd(AudworkCacheBuf, AudworkNCBuf, Csize, NCsize, &retInputHdlr, &retEncodeHdlr, &retEncBuffHdlr, AUDIOENC_TASK_ADD_ALL);
            AudioEncSecInstance[0].InputHdlr = AudioEncInstance[0].InputHdlr;
            AudioEncSecInstance[0].EncodeHdlr = (UINT32 *)retEncodeHdlr;
            AudioEncSecInstance[0].EncBuffHdlr = (UINT32 *)retEncBuffHdlr;

            EncCfg[1].AudioInstance = AudioEncSecInstance;
        }
        // Assign bitstream-specific configs
        AmpAudioEnc_Create(&EncCfg[1], &AudioEncSecHdlr);
    }

    return 0;
}


static AMP_FIFO_HDLR_s *AudioPriFifoHdlr = NULL;
int AppLibAudioEnc_AudioFifoCB(void *hdlr, UINT32 event, void* info)
{
    AMP_BITS_DESC_s *desc;
#ifdef CONFIG_APP_ARD
    UINT32 *numFrames = info;
#endif
    AmpFifo_PeekEntry(AudioPriFifoHdlr, &desc, 0);
    switch (desc->Type) {
    case AMP_FIFO_TYPE_EOS:
        DBGMSG("[Applib - Audio] <AudioFifoCB>: AMP_FIFO_TYPE_EOS");
        break;
    default:
#ifndef CONFIG_APP_ARD
        AmbaPrint("[Applib - Audio] <AudioFifoCB>: desc->Type 0x%x", desc->Type);
#endif
        break;
    }
#ifndef CONFIG_APP_ARD
    AmbaPrintColor(GREEN,"[Applib - Audio] <AudioFifoCB>: 0x%x",event);
#endif
    switch (event) {
    case AMP_FIFO_EVENT_DATA_READY:
#ifdef CONFIG_APP_ARD
    {
        int i;
#endif
        DBGMSG("[Applib - Audio] <AudioFifoCB>: AMP_FIFO_EVENT_DATA_READY");
#ifdef CONFIG_APP_ARD
        for(i = 0; i < *numFrames; i++) {
            AmbaKAL_SemGive(&ApplibAudioEncPriSem);
        }
    }
#endif
        break;
    case AMP_FIFO_EVENT_DATA_EOS:
        DBGMSG("[Applib - Audio] <AudioFifoCB>: AMP_FIFO_EVENT_DATA_EOS");
#ifdef CONFIG_APP_ARD
        AmbaKAL_SemGive(&ApplibAudioEncPriSem);
#endif
        break;
    default:
        AmbaPrint("[Applib - Audio] <AudioFifoCB>: evnet 0x%x", event);
        break;
    }

    return 0;
}


int AppLibAudioEnc_EncodeSetup(void)
{
    int ReturnValue = 0;
    AppLibAudioEnc_Setup();

    // Register pipeline
    if (AudioEncPipe == NULL) {
        AMP_ENC_PIPE_CFG_s PipeCfg;
        memset(&PipeCfg, 0x0, sizeof(AMP_ENC_PIPE_CFG_s));
        // Register pipeline, audio pipe can support up to 16 encoder
        AmpEnc_GetDefaultCfg(&PipeCfg);
        //PipeCfg.cbEvent
        {
            PipeCfg.encoder[0] = AudioEncPriHdlr;
            PipeCfg.numEncoder = 1;
        }
        PipeCfg.type = AMP_ENC_AV_PIPE;
        AmpEnc_Create(&PipeCfg, &AudioEncPipe);

        AmpEnc_Add(AudioEncPipe);
    } else {
        AmbaPrintColor(RED,"[Applib - AudioEnc] <Setup> Current Not Support multi audio pipe 0x%X", AudioEncPipe);
    }

    if(AudioPriFifoHdlr == NULL) {
        AMP_FIFO_CFG_s FifoDefCfg;
        AmpFifo_GetDefaultCfg(&FifoDefCfg);
        FifoDefCfg.hCodec = AudioEncPriHdlr;
        FifoDefCfg.IsVirtual = 1;
        FifoDefCfg.NumEntries = 1024;
        FifoDefCfg.cbEvent = AppLibAudioEnc_AudioFifoCB;
        ReturnValue = AmpFifo_Create(&FifoDefCfg, &AudioPriFifoHdlr);
    }

    return ReturnValue;
}

/**
 *  @brief Start audio encoding.
 *
 *  Start audio encoding.
 *
 *  @return >=0 success, <0 failure
 */
int AppLibAudioEnc_EncodeStart(void)
{
    int ReturnValue = 0;

#ifdef CONFIG_APP_ARD
	AppLibAudioEnc_SetAudioType(0);
#endif
    ReturnValue = AmpEnc_StartRecord(AudioEncPipe, 0);
#ifdef CONFIG_APP_ARD
    AppLibAudioEnc_CreateFileName();    
    SoundRecording = 1;
    ApplibAudioFrame = 0;
#endif
    return ReturnValue;
}



/**
 *  @brief Stop encoding audio.
 *
 *  Stop encoding audio.
 *
 *  @return >=0 success, <0 failure
 */
int AppLibAudioEnc_EncodeStop(void)
{
    int ReturnValue = 0;

    ReturnValue = AmpEnc_StopRecord(AudioEncPipe, 0);
#ifndef CONFIG_APP_ARD    
    {
        AMP_BITS_DESC_s *desc;
        AmpFifo_PeekEntry(AudioPriFifoHdlr, &desc, 0);
        switch (desc->Type) {
        case AMP_FIFO_TYPE_EOS:
            AmbaPrint("[Applib - Format] <AppLibAudioEnc_EncodeStop>: AMP_FIFO_TYPE_EOS");
            break;
        default:
            AmbaPrint("[Applib - Format] <AppLibAudioEnc_EncodeStop>: desc->Type 0x%x", desc->Type);
            break;
        }
        }
#else
	AppLibAudioEnc_SetAudioType(1);
#endif

    return ReturnValue;
}


#ifdef CONFIG_APP_ARD
void AppLibAudioEnc_PriMuxTask(UINT32 info)
{
	AMP_BITS_DESC_s *desc;
	int er;
	UINT8 *bitsLimit = AudPriBitsBuf + AUDENC_BITSFIFO_SIZE - 1;

	AmbaPrint("AppLibAudioEnc_PriMuxTask Start");

	while (1) {
		AmbaKAL_SemTake(&ApplibAudioEncPriSem, AMBA_KAL_WAIT_FOREVER);

		if(SoundRecording ==0) {
			AmbaKAL_TaskSleep(50);
			continue;
		}

		if (ApplibAudioRecStreamPri == NULL) {            
			AMP_FILE_STREAM_CFG_s FileCfg;
			memset(&FileCfg, 0x0, sizeof(AMP_FILE_STREAM_CFG_s));
			AmpFileStream_GetDefaultCfg(&FileCfg);
			FileCfg.Async = TRUE;
			FileCfg.BytesToSync = 0x1000;
			FileCfg.AsyncParam.MaxBank = 2;
			AmpFileStream_Create(&FileCfg, &ApplibAudioRecStreamPri);
			if (ApplibAudioRecStreamPri->Func->Open(ApplibAudioRecStreamPri, ApplibAudioFileName, AMP_STREAM_MODE_WRONLY) != AMP_OK) {
				AmbaPrintColor(RED,"%s:%u", __FUNCTION__, __LINE__);
				ApplibAudioRecStreamPri = NULL;
			}else{
				AmbaPrint("%s opened,BytesToSync = 0x%x,MaxBank = 0x%x", ApplibAudioFileName,FileCfg.BytesToSync,FileCfg.AsyncParam.MaxBank);
			}
			bitsLimit = AudPriBitsBuf + AUDENC_BITSFIFO_SIZE - 1;
		}

		er = AmpFifo_PeekEntry(AudioPriFifoHdlr, &desc, 0);
		if (er == 0) {
		//AmbaPrint("Muxer PeekEntry: pts:%8lld size:%5d@0x%08x", desc->Pts, desc->Size, desc->StartAddr);
		} else {
			while (er != 0) {
				AmbaPrint("[Pri]Muxer PeekEntry: Empty...");
				AmbaKAL_TaskSleep(30);
				er = AmpFifo_PeekEntry(AudioPriFifoHdlr, &desc, 0);
			}
		}

		if (ApplibAudioRecStreamPri != NULL) {
			//AmbaPrint("[Pri]Write: 0x%x sz %d limit %X",  desc->StartAddr, desc->Size, bitsLimit);
			if (desc->Size == AMP_FIFO_MARK_EOS) {                 
				if(ApplibAudioRecStreamPri->Func->Close(ApplibAudioRecStreamPri) != AMP_OK) {
					AmbaPrintColor(RED,"%s:%u", __FUNCTION__, __LINE__);
				}
				AmpFileStream_Delete(ApplibAudioRecStreamPri);
				ApplibAudioRecStreamPri = NULL;

				//FileOffset = 0;                    
				SoundRecording = 0;
				ApplibAudioFrame = 0;
				AppLibComSvcHcmgr_SendMsgNoWait(HMSG_MUXER_END, 0, 0);
				AmbaPrint("[Pri]Muxer met EOS");

				AmpFifo_RemoveEntry(AudioPriFifoHdlr, 1);

				if (AudioPriFifoHdlr != NULL) {
					AmpFifo_Delete(AudioPriFifoHdlr);
					AudioPriFifoHdlr = NULL;
				}
				continue;
			} else {
			if (desc->StartAddr + desc->Size <= bitsLimit) {
				ApplibAudioRecStreamPri->Func->Write(ApplibAudioRecStreamPri,desc->Size,desc->StartAddr);
				}else{
					AmbaPrint("[Pri]Muxer Wrap Around");
					ApplibAudioRecStreamPri->Func->Write(ApplibAudioRecStreamPri,bitsLimit - desc->StartAddr + 1,desc->StartAddr);
					ApplibAudioRecStreamPri->Func->Write(ApplibAudioRecStreamPri,desc->Size - (bitsLimit - desc->StartAddr + 1),AudPriBitsBuf);
				}

				//split
				if(ApplibAudioFrame >= AUDIO_SPLIT_FRAME){                    	
					if(ApplibAudioRecStreamPri->Func->Close(ApplibAudioRecStreamPri) != AMP_OK) {
					AmbaPrintColor(RED,"%s:%u", __FUNCTION__, __LINE__);
				}
				AmpFileStream_Delete(ApplibAudioRecStreamPri);
				ApplibAudioRecStreamPri = NULL;			
				ApplibAudioFrame = 0;
				AppLibAudioEnc_CreateFileName();
				}
			}
		}
		AmpFifo_RemoveEntry(AudioPriFifoHdlr, 1);
		ApplibAudioFrame++;
	}
}

int AppLibAudioEnc_Mux_Setup(void)
{
    int er;

    if(_au_mux_tsk_init == 0) {
        // Create simple muxer semophore
        if (AmbaKAL_SemCreate(&ApplibAudioEncPriSem, 0) != OK) {
            AmbaPrint("AppLib: Semaphore creation failed");
        }

        // Create simple muxer task        
        er = AmpUtil_GetAlignedPool(APPLIB_G_MMPL, (void **)&ApplibAudioEncPriStack, &ApplibAudioEncPriStackRaw, 6400+32, 32);
        if (er != OK) {
            AmbaPrint("Out of memory for muxer stack!!");
        }

        if (AmbaKAL_TaskCreate(&ApplibAudioEncPriMuxTask, "AppLibAudio Encoder Primary Muxing Task", 10, \
             AppLibAudioEnc_PriMuxTask, 0x0, ApplibAudioEncPriStack, 6400, AMBA_KAL_AUTO_START) != OK) {
             AmbaPrint("AppLibAudioEnc: Primary Muxer task creation failed");
        }    

        ApplibAudioFrame = 0;
        _au_mux_tsk_init = 1;
    }else{
        AmbaPrint("AppLibAudioEnc_Mux_Setup already init.");
    }

    return 0;
}

int AppLibAudioEnc_SetAudioType(UINT8 IsAvRecording)
{
	int ReturnValue = 0;
	int timeout = 50;
	AMP_AUDIOENC_CFG_s cfg = {0};
	
	if(IsAvRecording){
		cfg.EventDataReadySkipNum = 0;
		cfg.FadeInTime = 0;
		cfg.FadeOutTime= 0;
		cfg.SrcSampleRate= ApplibAudioEncAudioStreamDefaultSettingTable[ApplibAudioEncAudioSetting.EncType].SrcSampleRate;
		cfg.DstSampleRate= ApplibAudioEncAudioStreamDefaultSettingTable[ApplibAudioEncAudioSetting.EncType].DstSampleRate;
		cfg.SrcChannelMode= ApplibAudioEncAudioStreamDefaultSettingTable[ApplibAudioEncAudioSetting.EncType].SrcChannelMode;
		cfg.DstChannelMode= ApplibAudioEncAudioStreamDefaultSettingTable[ApplibAudioEncAudioSetting.EncType].DstChannelMode;
		cfg.EncType= ApplibAudioEncAudioStreamDefaultSettingTable[ApplibAudioEncAudioSetting.EncType].EncType;

	        if (ApplibAudioEncAudioSetting.EncType == AUDIO_TYPE_PCM) {
	        		cfg.Spec.PCMCfg= ApplibAudioEncAudioStreamDefaultSettingTable[ApplibAudioEncAudioSetting.EncType].Spec.PCMCfg;        
	        } else {
			cfg.Spec.AACCfg= ApplibAudioEncAudioStreamDefaultSettingTable[ApplibAudioEncAudioSetting.EncType].Spec.AACCfg; 	   
	        }

		while(AmpAudioEnc_Config(AudioEncPriHdlr,&cfg) != AMP_OK){
			AmbaKAL_TaskSleep(10);
			if((timeout--) < 0){
				ReturnValue = -1;
				break;
			}
		}
	}else{
		cfg.EventDataReadySkipNum = 0;
		cfg.FadeInTime = 0;
		cfg.FadeOutTime= 0;
		cfg.SrcSampleRate= ApplibAudioEncAudioStreamDefaultSettingTableForSoundsRecording[ApplibAudioEncAudioSetting.EncType].SrcSampleRate;
		cfg.DstSampleRate= ApplibAudioEncAudioStreamDefaultSettingTableForSoundsRecording[ApplibAudioEncAudioSetting.EncType].DstSampleRate;
		cfg.SrcChannelMode= ApplibAudioEncAudioStreamDefaultSettingTableForSoundsRecording[ApplibAudioEncAudioSetting.EncType].SrcChannelMode;
		cfg.DstChannelMode= ApplibAudioEncAudioStreamDefaultSettingTableForSoundsRecording[ApplibAudioEncAudioSetting.EncType].DstChannelMode;
		cfg.EncType= ApplibAudioEncAudioStreamDefaultSettingTableForSoundsRecording[ApplibAudioEncAudioSetting.EncType].EncType;

		if (ApplibAudioEncAudioSetting.EncType == AUDIO_TYPE_PCM) {
			cfg.Spec.PCMCfg= ApplibAudioEncAudioStreamDefaultSettingTableForSoundsRecording[ApplibAudioEncAudioSetting.EncType].Spec.PCMCfg; 	   
		} else {
			cfg.Spec.AACCfg= ApplibAudioEncAudioStreamDefaultSettingTableForSoundsRecording[ApplibAudioEncAudioSetting.EncType].Spec.AACCfg; 	   
		}
		
		while(AmpAudioEnc_Config(AudioEncPriHdlr,&cfg) != AMP_OK){
			AmbaKAL_TaskSleep(10);
			if((timeout--) < 0){
				ReturnValue = -1;
				break;
			}
		}
	}
	
	if (ReturnValue != AMP_OK) {
		AmbaPrintColor(RED, "%s:%u Failed (%d).", __FUNCTION__, __LINE__, ReturnValue);
	}

	return ReturnValue;
}

int AppLibAudioEnc_CreateFileName(void)
{
    int ReturnValue = 0;
    char FnExt[4] = "AAC";
    
    if(ApplibAudioEncAudioSetting.EncType == AUDIO_TYPE_PCM){
    	strcpy(FnExt,"PCM");
    }
	
    ReturnValue = AppLibStorageDmf_CreateFile(APPLIB_DCF_MEDIA_AUDIO, FnExt, DCIM_HDLR, ApplibAudioFileName);   
   
    if (ReturnValue <= 0) {
        AmbaPrintColor(RED,"[AppLibAudioEnc] %s:%u Get filename failure.", __FUNCTION__, __LINE__);
        return -1;
    }
    
    return ReturnValue;
}

int AppLibAudioEnc_Mute(UINT8 enable_mute)
{
	int ReturnValue = 0;

	if(enable_mute){
		ReturnValue = AppLibAudio_EffectVolumeUpdate(APPLIB_AUDIO_EFFECT_INPUT,AudioEncInstance[0].InputHdlr,0);
	}else{
		ReturnValue = AppLibAudio_EffectVolumeUpdate(APPLIB_AUDIO_EFFECT_INPUT,AudioEncInstance[0].InputHdlr,APPLIB_AUDIO_MAX_VOLUME_LEVLE);
	}

	if (ReturnValue < 0) {
		AmbaPrintColor(RED,"%s:%u failure. rval = %d", __FUNCTION__, __LINE__,ReturnValue);
		return -1;
	}
	return ReturnValue;
}
#endif

