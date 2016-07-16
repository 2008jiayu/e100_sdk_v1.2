/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaAudio.h
 *
 *  @Copyright      :: Copyright (C) 2012 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Definitions & Constants for Audio Module: Support CODEC of AAC, ADPCM, AC3, and MPEG
\*-------------------------------------------------------------------------------------------------------------------*/

#ifndef _AMBA_AUDIO_H_
#define _AMBA_AUDIO_H_

typedef enum _AMBA_AUDIO_DEC_FLOW_e_ {
    VIDEO_AUDIO = 0,    /* AV pre-sync. and callback set_adec_ready */
    PURE_AUDIO,         /* No pre-sync. */
    PTS_AUDIO           /* Only AV pre-sync. */
} AMBA_AUDIO_DEC_FLOW_e;

typedef enum _AMBA_AUDIO_AAC_BS_TYPE_e_ {
    AAC_BS_RAW  = 0,
    AAC_BS_ADIF,
    AAC_BS_ADTS,
    AAC_BS_LOAS
} AMBA_AUDIO_AAC_BS_TYPE_e;

typedef enum _AMBA_AUDIO_PCM_FORMAT_e_ {
    Audio_BS_Intel = 0,
    Audio_BS_Motorola = 1,
} AMBA_AUDIO_PCM_FORMAT_e;

typedef enum _AMBA_AUDIO_TYPE_e_ {
    AMBA_AUDIO_PCM = 0x00,
    AMBA_AUDIO_AAC = 0x10,
    AMBA_AUDIO_AAC_PLUS = 0x11,
    AMBA_AUDIO_AAC_PLUS_V2 = 0x12,
    AMBA_AUDIO_ADPCM = 0x20,
    AMBA_AUDIO_AC3 = 0x30,
    AMBA_AUDIO_MPEG = 0x40,
    AMBA_AUDIO_OPUS = 0x50,
} AMBA_AUDIO_TYPE_e;

typedef enum _AMBA_AUDIO_OPUS_BS_TYPE_e_ {
    OPUS_BS_RAW  = 0,
    OPUS_BS_RTP,
} AMBA_AUDIO_OPUS_BS_TYPE_e;

/* PCM CODEC data structure */
typedef struct _AMBA_AUDIO_PCM_CONFIG_s_ {
    UINT32  BitsPerSample;          /* 8, 16, 24, or 32-bits per sample */
    UINT32  DataFormat;             /* Intel (LSB,MSB), Motorola (MSB, LSB) */
    UINT32  FrameSize;
} AMBA_AUDIO_PCM_CONFIG_s;

/* AAC encoder data structure */
typedef struct _AMBA_AUDIO_AACENC_CONFIG_s_ {
    AMBA_AUDIO_AAC_BS_TYPE_e  BitstreamType;
    UINT32  Bitrate;
} AMBA_AUDIO_AACENC_CONFIG_s;

/* AAC decoder data structure */
typedef struct _AMBA_AUDIO_AACDEC_CONFIG_s_  {
    AMBA_AUDIO_AAC_BS_TYPE_e  BitstreamType;
} AMBA_AUDIO_AACDEC_CONFIG_s;

/* Dolby AC3 encoder data structure */
typedef struct _AMBA_AUDIO_AC3ENC_CONFIG_s_ {
    UINT8   Ac3EncAcmod;
    UINT32  Ac3EncBitrate;
    UINT8   Ac3EncAgcEnable;
    UINT8   Ac3EncAgcCh2Enable;
    UINT8   Ac3EncDrcMode;
    UINT8   Ac3EncLfeEnable;
    UINT8   Ac3EncLfeFilterEnable;
    UINT8   Ac3EncTestMode;
    UINT8   Ac3EncSurroundDelayEnable;
    UINT8   Ac3EncBsEndian;
} AMBA_AUDIO_AC3ENC_CONFIG_s;

/* Dolby AC3 decoder data structure */
typedef struct _AMBA_AUDIO_AC3DEC_CONFIG_s_ {
    UINT8   Ac3DecKCapableMode;     /* karaoke capable mode */
    UINT8   Ac3DecCompMode;         /* compression mode */
    UINT8   Ac3DecStereoMode;       /* stereo downmix mode */
    UINT8   Ac3DecDualMonoMode;     /* dual mono reproduction mode */
    UINT8   Ac3DecOutputMode;       /* output channel configuration */
    UINT8   Ac3DecOutLfeOn;         /* output subwoofer present flag */
    UINT16  Ac3DecOutPair;          /* output channel pair */
    UINT16  Ac3DecWordSize;         /* output word size code */
    UINT16  Ac3DecNumChans;         /* output number channel : 1~5*/
    INT32   Ac3DecDynRngScaleLow;   /* dynamic range scale factor (low): 0x0~0x7FFFFFFF*/
    INT32   Ac3DecDynRngScaleHi;    /* dynamic range scale factor (high): 0x0~0x7FFFFFFF */
    INT32   Ac3DecPcmScaleFac;      /* PCM scale factor: 0x0~0x7FFFFFFF */
    UINT8   Ac3DecBsEndian;         /* Bitstream Endian */
} AMBA_AUDIO_AC3DEC_CONFIG_s;

/* ADPCM CODEC data structure */
typedef struct _AMBA_AUDIO_ADPCM_CONFIG_s_ {
    UINT32 AdpcmFrameSize;
} AMBA_AUDIO_ADPCM_CONFIG_s;

/* OPUS encoder data structure */
typedef struct _AMBA_AUDIO_OPUSENC_CONFIG_s_ {
    UINT32 Bitrate;
    UINT32 FrameSize;
    UINT8  BitstreamType;
} AMBA_AUDIO_OPUSENC_CONFIG_s;

/* OPUS decoder data structure */
typedef struct _AMBA_AUDIO_OPUSDEC_CONFIG_s_ {
    UINT32 FrameSize;
    UINT8  BitstreamType;
} AMBA_AUDIO_OPUSDEC_CONFIG_s;

typedef enum _AMBA_AUDIO_DEC_NEEDED_SIZE_e_ {
    AAC_DEC_NEEDED_SIZE         =   2048,
    MP3_DEC_NEEDED_SIZE         =   2048,
    ADPCM_DEC_NEEDED_SIZE       =   2048,
    PCM_DEC_NEEDED_SIZE         =   1024 * 4 * 2,   /* 32bit pcm, 2ch */
    OPUS_DEC_NEEDED_SIZE        =   2048,
    AUDIO_DEC_MAX_NEEDED_SIZE   =   8192,
} AMBA_AUDIO_DEC_NEEDED_SIZE_e;

typedef enum _AMBA_AUDIO_ENC_NEEDED_SIZE_e_ {
    AAC_ENC_NEEDED_SIZE         =   1536,
    MP3_ENC_NEEDED_SIZE         =   2048,
    ADPCM_ENC_NEEDED_SIZE       =   2048,
    PCM_ENC_NEEDED_SIZE         =   1024 * 4 * 2,   /* 32bit pcm, 2ch */
    OPUS_ENC_NEEDED_SIZE        =   2048,
    AUDIO_ENC_MAX_NEEDED_SIZE   =   8192,
} AMBA_AUDIO_ENC_NEEDED_SIZE_e;

/* Audio channel mode */
typedef enum _AMBA_AUDIO_CH_MODE_e_ {
    AUDIO_CH_MODE_DUAL_MONO = 0x00,
    AUDIO_CH_MODE_C = 0x01,
    AUDIO_CH_MODE_L_R = 0x02,
    AUDIO_CH_MODE_L_C_R = 0x03,
    AUDIO_CH_MODE_L_R_S = 0x04,
    AUDIO_CH_MODE_L_C_R_S = 0x05,
    AUDIO_CH_MODE_L_R_LS_RS = 0x06,
    AUDIO_CH_MODE_L_C_R_LS_RS = 0x07,
    AUDIO_CH_MODE_DUAL_MONO_LFE = 0x10,
    AUDIO_CH_MODE_C_LFE = 0x11,
    AUDIO_CH_MODE_L_R_LFE = 0x12,
    AUDIO_CH_MODE_L_C_R_LFE = 0x13,
    AUDIO_CH_MODE_L_R_S_LFE = 0x14,
    AUDIO_CH_MODE_L_C_R_S_LFE = 0x15,
    AUDIO_CH_MODE_L_R_LS_RS_LFE = 0x16,
    AUDIO_CH_MODE_L_C_R_LS_RS_LFE = 0x17,
} AMBA_AUDIO_CH_MODE_e;

typedef enum _AMBA_AUDIO_DEBUG_LEVEL_BIT_e_ {
    AU_DBG_SILENT_BIT = 0x0,
    AU_DBG_API_BIT,
    AU_DBG_AIN_BIT,
    AU_DBG_AOUT_BIT,
    AU_DBG_AENC_BIT,
    AU_DBG_ADEC_BIT,
    AU_DBG_AIN_PROC2PROC_BIT,
    AU_DBG_AOUT_PROC2PROC_BIT,
    AU_DBG_AENC_PROC2PROC_BIT,
    AU_DBG_ADEC_PROC2PROC_BIT,
    AU_DBG_INFO_NUMBER,
} AMBA_AUDIO_DEBUG_LEVEL_BIT_e;

typedef enum _AMBA_AUDIO_DEBUG_LEVEL_e_  {
    AU_DBG_SILENT           = 0x1 << AU_DBG_SILENT_BIT,
    AU_DBG_API              = 0x1 << AU_DBG_API_BIT,
    AU_DBG_AIN              = 0x1 << AU_DBG_AIN_BIT,
    AU_DBG_AOUT             = 0x1 << AU_DBG_AOUT_BIT,
    AU_DBG_AENC             = 0x1 << AU_DBG_AENC_BIT,
    AU_DBG_ADEC             = 0x1 << AU_DBG_ADEC_BIT,
    AU_DBG_AIN_PROC2PROC    = 0x1 << AU_DBG_AIN_PROC2PROC_BIT,
    AU_DBG_AOUT_PROC2PROC   = 0x1 << AU_DBG_AOUT_PROC2PROC_BIT,
    AU_DBG_AENC_PROC2PROC   = 0x1 << AU_DBG_AENC_PROC2PROC_BIT,
    AU_DBG_ADEC_PROC2PROC   = 0x1 << AU_DBG_ADEC_PROC2PROC_BIT,
} AMBA_AUDIO_DEBUG_LEVEL_e;

typedef struct _AMBA_AUDIO_DESC_s_ {
    UINT32      *pHdlr;             /* handle of the task */
    UINT64      Pts;                /* Pts of this descriptor */
    UINT32      PicType;
    UINT32      DataSize;           /* Valid data size, from starting address */
    UINT8       *pBufAddr;          /* Buffer starting address */
} AMBA_AUDIO_DESC_s;

typedef AMBA_AUDIO_DESC_s* (*AMBA_AUDIO_DEC_GET_BS_DESC)(UINT32 *pHdlr);
typedef UINT8* (*AMBA_AUDIO_ENC_GET_BS_ADDR)(UINT32 *pHdlr);

typedef struct _AMBA_AUDIO_PMU_CONFIG_s_ {
    UINT32 CcntEn:4;
    UINT32 Pmnc0En:4;
    UINT32 Pmnc1En:4;
    UINT32 Pmnc0Event:8;
    UINT32 Pmnc1Event:8;
} AMBA_AUDIO_PMU_CONFIG_s;

/* Audio calibration mode */
typedef enum _AMBA_AUDIO_CALIB_MODE_e_ {
    AUDIO_CALIB_PROC = 0,
    AUDIO_CALIB_APPLY_INPUT,
    AUDIO_CALIB_APPLY_AAC_ENC,
} AMBA_AUDIO_CALIB_MODE_e;

typedef struct _AMBA_AUDIO_CALIB_PROC_CTRL_s_ {
    UINT32  CalibOperateMode;       /* 0: calculate silence calibre curve , 1: calculate dBFS and THD+N  2: calculate dBFS of whole frequency spectrum */
    INT8    *pCalibNoiseThAddr;     /* address of noise threshold array */
    INT8    *pCalibRangeAddr;       /* address of calibration range limit array, //ex. cannot decay more than 30dB for a certain band */
    INT32   CalibreFreqIdx;         /* under calibre_operate_mode=1, based on this frequency idx to calculate dBFS and THD+N */
    INT32   *pCalibBuffer;          /* Calibration process buffer address */
} AMBA_AUDIO_CALIB_PROC_CTRL_s;

typedef struct _AMBA_AUDIO_CALIB_APPLY_CTRL_s_ {
    INT32   *pCalibBuffer;          /* Calibration process buffer address */
    INT8    *pCalibCurveAddr;       /* address of calibration curve */
    UINT32  *pUseAacEncHdlr;        /* Encoder task handler that used AAC encoder to apply calibration curve */
} AMBA_AUDIO_CALIB_APPLY_CTRL_s;

/* Audio calibration control */
typedef struct _AMBA_AUDIO_CALIB_CTRL_s_ {
    AMBA_AUDIO_CALIB_MODE_e         CalibMode;
    AMBA_AUDIO_CALIB_PROC_CTRL_s    CalibProcCtrl;
    AMBA_AUDIO_CALIB_APPLY_CTRL_s   CalibApplyCtrl;
} AMBA_AUDIO_CALIB_CTRL_s;

/* Decoder Setup Information */
typedef struct __AMBA_AUDIO_DEC_SETUP_INFO_s_ {
    AMBA_AUDIO_DEC_FLOW_e PureAudio;
    UINT32                SrcSampleFreq;        /* Sample frequency of the input */
    UINT32                DstSampleFreq;        /* Sample frequency of the output */
    UINT32                SrcChMode;            /* Channel mode of the input */
    UINT32                DstChMode;            /* Channel mode of the output */
    AMBA_AUDIO_TYPE_e     DecType;
    AMBA_AUDIO_PMU_CONFIG_s Pmu;
    AMBA_AUDIO_DEC_GET_BS_DESC DecGetBsDesc;    /* Callback function for decoder get bitstream */
    void                  *pDecConfig;          /* CODEC configuration data pointer. */
} AMBA_AUDIO_DEC_SETUP_INFO_s;

/* Encoder Setup Information */
typedef struct __AMBA_AUDIO_ENC_SETUP_INFO_s_ {
    UINT32                SrcSampleFreq;        /* Sample frequency of the input */
    UINT32                DstSampleFreq;        /* Sample frequency of the output */
    UINT32                SrcChMode;            /* Channel mode of the input */
    UINT32                DstChMode;            /* Channel mode of the output */
    UINT32                PtsClock;             /* Clock of Pts */
    AMBA_AUDIO_TYPE_e     EncType;
    AMBA_AUDIO_PMU_CONFIG_s Pmu;
    AMBA_AUDIO_ENC_GET_BS_ADDR EncGetBsAddr;    /* Callback function for encoder get bitstream */
    void                  *pEncConfig;          /* CODEC configuration data pointer. */
} AMBA_AUDIO_ENC_SETUP_INFO_s;

/* Audio Event Information */
typedef struct __AMBA_AUDIO_EVENT_INFO_s_ {
    UINT32  *pHandle;   /* handle of the task */
    UINT32  *pAbu;      /* handle of connected ABU */
} AMBA_AUDIO_EVENT_INFO_s;

typedef struct __AMBA_AUDIO_CALIB_INFO_s_ {
    UINT32  *pHandle;   /* handle of the task */
    UINT32  Status;     /* status of calibration result */
} AMBA_AUDIO_CALIB_INFO_s;

typedef enum _AMBA_AUDIO_EVENT_ID_e_ {
    /* Encoder Event */
    AMBA_AUDIO_EVENT_ID_ENCODE_ONE_FRAME = 0,
    AMBA_AUDIO_EVENT_ID_ENCODE_FADE_OUT_DONE,
    AMBA_AUDIO_EVENT_ID_ENCODE_STOP,

    /* Input Event */
    AMBA_AUDIO_EVENT_ID_INPUT_DMA_STOP,
    AMBA_AUDIO_EVENT_ID_INPUT_CALIB_STATUS,

    /* Decoder Event */
    AMBA_AUDIO_EVENT_ID_DECODE_DATA_READY,
    AMBA_AUDIO_EVENT_ID_DECODE_FADE_OUT_DONE,
    AMBA_AUDIO_EVENT_ID_DECODE_STOP,
    AMBA_AUDIO_EVENT_ID_DECODE_EOS_STOP,
    AMBA_AUDIO_EVENT_ID_DECODE_RUN_TIME_ERROR,
    AMBA_AUDIO_EVENT_ID_DECODE_USE_ONE_FRAME,

    /* Output Event */
    AMBA_AUDIO_EVENT_ID_OUTPUT_DMA_STOP,
    AMBA_AUDIO_EVENT_ID_OUTPUT_ABU_GET_LOF,
    AMBA_AUDIO_EVENT_ID_OUTPUT_NO_ABU_OP,

    AMBA_AUDIO_NUM_EVENT
} AMBA_AUDIO_EVENT_ID_e;

typedef int (*AMBA_AUDIO_EVENT_HANDLER_f)(void *pEventData);

typedef struct _AMBA_AUDIO_MEM_INFO_s_ {
    UINT32  *pHead;         /* Current memory head */
    UINT32  CurrentSize;    /* Current used size */
    UINT32  MaxSize;        /* Max memory size */
} AMBA_AUDIO_MEM_INFO_s;

typedef struct _AMBA_AUDIO_BUF_INFO_s_ {
    UINT32  *pHead;         /* Current memory head */
    UINT32  MaxSize;        /* Max memory size */
} AMBA_AUDIO_BUF_INFO_s;

/* Audio Task Create Information */
typedef struct _AMBA_AUDIO_TASK_CREATE_INFO_s_ {
    UINT32  MaxSampleFreq;  /* Max supporting sample frequency */
    UINT32  MaxChNum;       /* Max channel number */
    UINT32  MaxFrameSize;   /* Max frame size */
} AMBA_AUDIO_TASK_CREATE_INFO_s;

/* Audio I/O Create Information */
typedef struct _AMBA_AUDIO_IO_CREATE_INFO_s_ {
    UINT32  MaxSampleFreq;  /* Max supporting sample frequency */
    UINT32  MaxChNum;       /* Max channel number */
    UINT32  MaxDmaSize;     /* Max DMA frame size */
    UINT32  MaxDmaDescNum;  /* Max DMA Desc number */
    UINT32  I2sIndex;       /* 0 or 1th I2S */
} AMBA_AUDIO_IO_CREATE_INFO_s;

/* ABU Create Information */
typedef struct _AMBA_ABU_CREATE_INFO_s_ {
    UINT32  MaxSampleFreq;  /* Max supporting sample frequency */
    UINT32  MaxChNum;       /* Max channel number */
    UINT32  MaxChunkNum;    /* Max chunk number of ABU */
} AMBA_ABU_CREATE_INFO_s;

/* Audio combine Information */
typedef struct _AMBA_AUDIO_COMBINE_INFO_s_ {
    UINT32  *pSrcApu;   /* source audio process controller address */
    UINT32  *pDstApu;   /* distination audio process controller address */
    UINT32  *pAbu;      /* ABU controller address */
} AMBA_AUDIO_COMBINE_INFO_s;

/* EFX */
typedef enum _AMBA_AUDIO_INPUT_EFFECT_ID_e_ {
	AMBA_AUDIO_INPUT_EFFECT_VOLUME_ID = 0,
	AMBA_AUDIO_INPUT_EFFECT_FC1_ID,
	AMBA_AUDIO_INPUT_EFFECT_CALIB_ID,
	AMBA_AUDIO_INPUT_EFFECT_POWER_MONITOR_ID,
	AMBA_AUDIO_INPUT_EFFECT_FC2_ID,
	AMBA_AUDIO_DEFAULT_INPUT_EFFECT_NUM
} AMBA_AUDIO_INPUT_EFFECT_ID_e;

typedef enum _AMBA_AUDIO_OUTPUT_EFFECT_ID_e_ {
	AMBA_AUDIO_OUTPUT_EFFECT_VOLUME_ID = 0,
	AMBA_AUDIO_DEFAULT_OUTPUT_EFFECT_NUM
} AMBA_AUDIO_OUTPUT_EFFECT_ID_e;

typedef enum _AMBA_AUDIO_ENC_EFFECT_ID_e_ {
	AMBA_AUDIO_ENC_EFFECT_VOLUME_ID = 0,
	AMBA_AUDIO_ENC_EFFECT_FADEIO_ID,
	AMBA_AUDIO_ENC_EFFECT_SRC_ID,
	AMBA_AUDIO_DEFAULT_ENC_EFFECT_NUM
} AMBA_AUDIO_ENC_EFFECT_ID_e;

typedef enum _AMBA_AUDIO_DEC_EFFECT_ID_e_ {
	AMBA_AUDIO_DEC_EFFECT_VOLUME_ID = 0,
	AMBA_AUDIO_DEC_EFFECT_FADEIO_ID,
	AMBA_AUDIO_DEC_EFFECT_SRC_ID,
	AMBA_AUDIO_DEFAULT_DEC_EFFECT_NUM
} AMBA_AUDIO_DEC_EFFECT_ID_e;

#define MAX_AUDIO_EFFECT        16
#define MAX_AUDIO_VOLUME_LEVLE  64 //64

typedef struct _AMBA_AUDIO_PLUGIN_EFFECT_CS_s_ {
    int *src;
    int *dest;
    UINT32 src_ch;
    UINT32 dest_ch;
    UINT32 src_size;
    UINT32 dest_size;
    void (*setup)(struct _AMBA_AUDIO_PLUGIN_EFFECT_CS_s_ *);
    void (*proc)(struct _AMBA_AUDIO_PLUGIN_EFFECT_CS_s_ *);
    void (*report)(struct _AMBA_AUDIO_PLUGIN_EFFECT_CS_s_ *);
    UINT32 size_of_self;
    void *self;
    UINT32 dest_auto_assign;
    UINT32 dest_ch_auto_assign;
    UINT32 dest_size_auto_assign;
} AMBA_AUDIO_PLUGIN_EFFECT_CS_s;

/*-----------------------------------------------------------------------------------------------*\
 * Defined in AmbaAudio_Dec.c
\*-----------------------------------------------------------------------------------------------*/
UINT32 AmbaAudio_DecSizeQuery(AMBA_AUDIO_TASK_CREATE_INFO_s *pInfo);
UINT32 *AmbaAudio_DecCreate(AMBA_AUDIO_TASK_CREATE_INFO_s *pInfo, UINT32 *pAddr, UINT32 BufferSize);
int AmbaAudio_DecDelete(UINT32 *pHdlr);
int AmbaAudio_DecTaskCreate(UINT32 *pHdlr, INT32 Priority, UINT32 CoreExlusionMap);
int AmbaAudio_DecTaskDelete(UINT32 *pHdlr);
int AmbaAudio_DecTaskSetUp(UINT32 *pHdlr, AMBA_AUDIO_DEC_SETUP_INFO_s *pConfig);
int AmbaAudio_DecTaskStart(UINT32 *pHdlr, UINT32 FadeInTime);
int AmbaAudio_DecTaskStop(UINT32 *pHdlr, UINT32 FadeOutTime);
int AmbaAudio_DecSetVpts(UINT32 *pHdlr, UINT32 Vpts);
int AmbaAudio_DecSetVolume(UINT32 *pHdlr, UINT32 Volumes);

/*-----------------------------------------------------------------------------------------------*\
 * Defined in AmbaAudio_Output.c
\*-----------------------------------------------------------------------------------------------*/
UINT32 AmbaAudio_OutputCachedSizeQuery(AMBA_AUDIO_IO_CREATE_INFO_s *pInfo);
UINT32 AmbaAudio_OutputNonCachedSizeQuery(AMBA_AUDIO_IO_CREATE_INFO_s *pInfo);
UINT32 *AmbaAudio_OutputCreate(AMBA_AUDIO_IO_CREATE_INFO_s *pInfo, AMBA_AUDIO_BUF_INFO_s *pCachedInfo, AMBA_AUDIO_BUF_INFO_s *pNonCachedInfo);
int AmbaAudio_OutputDelete(UINT32 *pHdlr);
int AmbaAudio_OutputTaskCreate(UINT32 *pHdlr, int Priority, UINT32 CoreExlusionMap);
int AmbaAudio_OutputTaskDelete(UINT32 *pHdlr);
int AmbaAudio_OutputTaskStart(UINT32 *pHdlr);
int AmbaAudio_OutputTaskStop(UINT32 *pHdlr);
int AmbaAudio_OutputSetVolume(UINT32 *pHdlr, UINT32 Volumes);
void AmbaAudio_OutputPluginEffectInstall(UINT32 *pHdlr, UINT32 Id, AMBA_AUDIO_PLUGIN_EFFECT_CS_s *pCs);
int AmbaAudio_OutputPluginEffectEnable(UINT32 *pHdlr, UINT32 Id);
int AmbaAudio_OutputPluginEffectDisable(UINT32 *pHdlr, UINT32 Id);
int AmbaAudio_OutputPluginEffectUpdate(UINT32 *pHdlr, UINT32 Id, AMBA_AUDIO_PLUGIN_EFFECT_CS_s *pCsShadow);

/*-----------------------------------------------------------------------------------------------*\
 * Defined in AmbaAudio_Enc.c
\*-----------------------------------------------------------------------------------------------*/
UINT32 AmbaAudio_EncSizeQuery(AMBA_AUDIO_TASK_CREATE_INFO_s *pInfo);
UINT32 *AmbaAudio_EncCreate(AMBA_AUDIO_TASK_CREATE_INFO_s *pInfo, UINT32 *pAddr, UINT32 BufferSize);
int AmbaAudio_EncDelete(UINT32 *pHdlr);
int AmbaAudio_EncTaskCreate(UINT32 *pHdlr, INT32 Priority, UINT32 CoreExlusionMap);
int AmbaAudio_EncTaskDelete(UINT32 *pHdlr);
int AmbaAudio_EncTaskSetUp(UINT32 *pHdlr, AMBA_AUDIO_ENC_SETUP_INFO_s *pConfig);
int AmbaAudio_EncTaskStart(UINT32 *pHdlr, UINT32 FadeInTime);
int AmbaAudio_EncTaskStop(UINT32 *pHdlr, UINT32 FadeOutTime);
int AmbaAudio_EncSetVolume(UINT32 *pHdlr, UINT32 Volumes);

/*-----------------------------------------------------------------------------------------------*\
 * Defined in AmbaAudio_Input.c
\*-----------------------------------------------------------------------------------------------*/
UINT32 AmbaAudio_InputCachedSizeQuery(AMBA_AUDIO_IO_CREATE_INFO_s *pInfo);
UINT32 AmbaAudio_InputNonCachedSizeQuery(AMBA_AUDIO_IO_CREATE_INFO_s *pInfo);
UINT32 *AmbaAudio_InputCreate(AMBA_AUDIO_IO_CREATE_INFO_s *pInfo, AMBA_AUDIO_BUF_INFO_s *pCachedInfo, AMBA_AUDIO_BUF_INFO_s *pNonCachedInfo);
int AmbaAudio_InputDelete(UINT32 *pHdlr);
int AmbaAudio_InputTaskCreate(UINT32 *pHdlr, int Priority, UINT32 CoreExlusionMap);
int AmbaAudio_InputTaskDelete(UINT32 *pHdlr);
int AmbaAudio_InputTaskStart(UINT32 *pHdlr);
int AmbaAudio_InputTaskStop(UINT32 *pHdlr);
int AmbaAudio_InputSetVolume(UINT32 *pHdlr, UINT32 Volumes);
int AmbaAudio_InputSetUpCalib(UINT32 *pHdlr, AMBA_AUDIO_CALIB_CTRL_s *pConfig);
int AmbaAudio_InputDisableCalib(UINT32 *pHdlr);
INT8 *AmbaAudio_InputCalibGetCurve(UINT32 *pHdlr);
INT32 *AmbaAudio_InputCalibGet_dBFS(UINT32 *pHdlr);
INT32 *AmbaAudio_InputCalibGetTHD_N(UINT32 *pHdlr);
INT8 *AmbaAudio_InputCalibGetFreqCurve(UINT32 *pHdlr);
int AmbaAudio_InputPowerMonitorEnable(UINT32 *pHdlr);
int AmbaAudio_InputPowerMonitorGetdB(UINT32 *pHdlr, UINT32 *pPower, UINT32 *pPowerPeak);
int AmbaAudio_InputPowerMonitorDisable(UINT32 *pHdlr);
void AmbaAudio_InputPluginEffectInstall(UINT32 *pHdlr, UINT32 Id, AMBA_AUDIO_PLUGIN_EFFECT_CS_s *pCs);
int AmbaAudio_InputPluginEffectEnable(UINT32 *pHdlr, UINT32 Id);
int AmbaAudio_InputPluginEffectDisable(UINT32 *pHdlr, UINT32 Id);
int AmbaAudio_InputPluginEffectUpdate(UINT32 *pHdlr, UINT32 Id, AMBA_AUDIO_PLUGIN_EFFECT_CS_s *pCsShadow);

/*-----------------------------------------------------------------------------------------------*\
 * Defined in AmbaAudio_Effect.c
\*-----------------------------------------------------------------------------------------------*/
UINT32 AmbaAudio_EffectCalibBufferSize(void);
int AmbaAudio_EffectUpdownSampleRateConvertSetup(UINT32 *pHdlr, UINT32 Id, UINT32 SampleRate, UINT32 OutSampleRate);

/*-----------------------------------------------------------------------------------------------*\
 * Defined in AmbaAudio_FileIO.c
\*-----------------------------------------------------------------------------------------------*/
UINT32 AmbaAudio_FileIoSizeQuery(UINT32 FrameSize, UINT32 ChNum);
UINT32 *AmbaAudio_FileIoCreate(UINT32 FrameSize, UINT32 ChNum, UINT32 *pAddr, UINT32 BufferSize);
int AmbaAudio_FileIoDelete(UINT32 *pHdlr);
int AmbaAudio_FileIoTaskDelete(UINT32 *pHdlr);
int AmbaAudio_FileIoTaskStart(UINT32 *pHdlr);
int AmbaAudio_FileInTaskCreate(UINT32 *pHdlr, INT32 Priority, UINT32 CoreExlusionMap);
int AmbaAudio_FileInTaskSetUp(UINT32 *pHdlr, char *pName);
int AmbaAudio_FileOutTaskCreate(UINT32 *pHdlr, INT32 Priority, UINT32 CoreExlusionMap);
int AmbaAudio_FileOutTaskSetUp(UINT32 *pHdlr, char *pName);
AMBA_AUDIO_DESC_s* AmbaAudio_FileInGetDesc(UINT32 *pHandle, UINT32 *pBuffer, UINT32 BufferSize);
int AmbaAudio_FileOutUpdateAddr(UINT32 *pHandle, UINT8 *pBuffer, UINT32 BufferSize, UINT32 Lof);

/*-----------------------------------------------------------------------------------------------*\
 * Defined in AmbaAudio_InternalBuffer.c
\*-----------------------------------------------------------------------------------------------*/
UINT32 AmbaAudio_BufferSizeQuery(AMBA_ABU_CREATE_INFO_s *pInfo);
UINT32 *AmbaAudio_BufferCreate(AMBA_ABU_CREATE_INFO_s *pInfo, UINT32 *pAddr, UINT32 BufferSize);
int AmbaAudio_BufferDelete(UINT32 *pHandle);
int AmbaAudio_BufferReset(UINT32 *pHandle);
int AmbaAudio_BufferOpenSrcIoNode(UINT32 *pHandle);
int AmbaAudio_BufferCloseSrcIoNode(UINT32 *pHandle);
int AmbaAudio_BufferOpenDstIoNode(UINT32 *pHandle);
int AmbaAudio_BufferCloseDstIoNode(UINT32 *pHandle);
int AmbaAudio_Combine(AMBA_AUDIO_COMBINE_INFO_s *pInfo);
int AmbaAudio_Detach(AMBA_AUDIO_COMBINE_INFO_s *pInfo);

/*-----------------------------------------------------------------------------------------------*\
 * Defined in AmbaAudio_System.c
\*-----------------------------------------------------------------------------------------------*/
int AmbaAudio_SetDebugLevel(UINT32 DebugLevel);

/*-----------------------------------------------------------------------------------------------*\
 * Macro Definitions
\*-----------------------------------------------------------------------------------------------*/
#define AmbaAudio_SetWritePts(pDesc, Pts)           pDesc->Pts = Pts
#define AmbaAudio_SetWritePicType(pDesc, PicType)   pDesc->PicType = PicType
#define AmbaAudio_GetWriteAddr(pDesc)               pDesc->pBufAddr
#define AmbaAudio_SetWriteSize(pDesc, Size)         pDesc->DataSize = Size

#define AmbaAudio_GetReadPts(pDesc)         pDesc->Pts
#define AmbaAudio_GetReadPicType(pDesc)     pDesc->PicType
#define AmbaAudio_GetReadAddr(pDesc)        pDesc->pBufAddr
#define AmbaAudio_GetReadSize(pDesc)        pDesc->DataSize

/*-----------------------------------------------------------------------------------------------*\
 * Defined in AmbaAudio_System.c
\*-----------------------------------------------------------------------------------------------*/
int AmbaAudio_EventHandlerCtrlReset(UINT32 *pHdlr);
int AmbaAudio_EventHandlerCtrlConfig(UINT32 *pHdlr, AMBA_AUDIO_EVENT_ID_e EventID,
                                     int MaxNumHandler,
                                     AMBA_AUDIO_EVENT_HANDLER_f *pEventHandlers);
int AmbaAudio_RegisterEventHandler(UINT32 *pHdlr, AMBA_AUDIO_EVENT_ID_e EventID, AMBA_AUDIO_EVENT_HANDLER_f EventHandler);
int AmbaAudio_UnRegisterEventHandler(UINT32 *pHdlr, AMBA_AUDIO_EVENT_ID_e EventID, AMBA_AUDIO_EVENT_HANDLER_f EventHandler);
void AmbaAudio_ProfileInit(UINT32 *pHdlr);
void AmbaAudio_ProfileRecResult(UINT32 *pHdlr, UINT32 TaskType);

#endif /* _AMBA_AUDIO_H_ */
