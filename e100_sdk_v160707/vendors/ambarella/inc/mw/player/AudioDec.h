/**
 * @file inc/mw/player/audiodec.h
 *
 * Amba audio decode header
 *
 * Copyright (C) 2013, Ambarella, Inc.
 *
 * All rights reserved. No Part of this file may be reproduced, stored
 * in a retrieval system, or transmitted, in any form, or by any means,
 * electronic, mechanical, photocopying, recording, or otherwise,
 * without the prior consent of Ambarella, Inc.
 */

#ifndef AUDIODEC_H_
#define AUDIODEC_H_

/**
 * @defgroup AudioDec
 * @brief Amba audio decoder implementation
 *
 * Audio decode module provide functions on audio decode.\n
 * Currently, we provide two decoding mode: PURE_AUDIO and VIDOE_AUDIO.\n
 * The module also provide function that init aout which should invoked before decode.
 *
 */

/**
 * @addtogroup AudioDec
 * @ingroup Codec
 * @{
 */

#include <AmbaAudio.h>
#include <fifo/Fifo.h>

typedef struct _AMP_AUDIODEC_INIT_CFG_s_ {
    UINT32 MaxHdlr; /**< max audio dec codec user could create */
    AMP_TASK_INFO_s TaskInfo; /**< Audio decode task info */
    UINT8 *WorkBuff; /**< Audio decode working buffer */
    UINT32 WorkBuffSize; /**< size of audio decode working buffer */
} AMP_AUDIODEC_INIT_CFG_s;

typedef struct _AMP_AUDIODEC_HDLR_CFG_s_ {
    // task create
    UINT8 DecoderTaskPriority; /**< Audio encoder task priority */
    AMBA_AUDIO_DEC_FLOW_e PureAudio; /**< codec for pure audio or video audio */

    // task setup and operation
    AMBA_AUDIO_TYPE_e DecType; /**< audio decoder type */
    UINT32 FadeInTime; /**< fade in when decode start */
    UINT32 FadeOutTime; /**< fade out when decode stop */
    UINT32 SrcSampleRate; /**< Sample frequency of the input */
    UINT32 DstSampleRate; /**< Sample frequency of the output */
    UINT16 SrcChannelMode; /**< Channel mode of the input */
    UINT16 DstChannelMode; /**< Channel mode of the output */
    UINT32 MaxSampleRate; /**< Max supporting sample frequency */
    UINT32 MaxChannelNum; /**< Max channel number */
    UINT32 MaxFrameSize; /**< Max frame size */
    UINT32 MaxChunkNum; /**< Max chunk number */
    UINT8 I2SIndex; /**< I2SIndex of output */
    UINT8 *CodecCacheWorkBuff; /**< buffer address for audio codec */
    UINT32 CodecCacheWorkSize; /**< buffer size for audio codec */
    union {
        AMBA_AUDIO_PCM_CONFIG_s PCMCfg;
        AMBA_AUDIO_AACDEC_CONFIG_s AACCfg;
        AMBA_AUDIO_OPUSDEC_CONFIG_s OPUSCfg;
    } Spec; /**< decode info */

    AMP_CALLBACK_f CbCfgUpdated; /**< setting take effect */
    AMP_CALLBACK_f CbEvent; /**< Event related to the encoder */

    UINT8* RawBuffer; /**< buffer address for audio raw file */
    UINT32 RawBufferSize; /**< size of buffer address for audio raw file */
    AMP_BITS_DESC_s* DescBuffer; /**< buffer address for audio descriptor */
    UINT32 DescBufferNum; /**< size of buffer address for audio descriptor */
    UINT8 DecoderTaskCoreSelectionBitMap; /**< Audio decoder task core select */
} AMP_AUDIODEC_HDLR_CFG_s;

typedef struct _AMP_AUDIODEC_DECODER_CFG_s_ {
    AMBA_AUDIO_TYPE_e DecType;
    UINT32 FadeInTime; /**< fade in when decode start */
    UINT32 FadeOutTime; /**< fade out when decode stop */
    UINT32 SrcSampleRate; /**< Sample frequency of the input */
    UINT16 SrcChannelMode; /**< Channel mode of the input */
    union {
        AMBA_AUDIO_PCM_CONFIG_s PCMCfg;
        AMBA_AUDIO_AACDEC_CONFIG_s AACCfg;
        AMBA_AUDIO_OPUSDEC_CONFIG_s OPUSCfg;
    } Spec; /**< decode info */
} AMP_AUDIODEC_DECODER_CFG_s;

/**
 * To get the default value of initial config for audio decode module
 *
 * @param [out] cfg - default module config
 *
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpAudioDec_GetInitDefaultCfg(AMP_AUDIODEC_INIT_CFG_s *cfg);

/**
 * To set the initial config for audio decode module
 *
 * @param [in] cfg - module config
 *
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpAudioDec_Init(AMP_AUDIODEC_INIT_CFG_s *cfg);

/**
 * To get audio decode codec default config
 *
 * @param [out] cfg - default codec config
 *
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpAudioDec_GetDefaultCfg(AMP_AUDIODEC_HDLR_CFG_s *cfg);

/**
 * To create audio decode codec
 *
 * @param [in] cfg - codec config
 * @param [out] hdlr - codec handler
 *
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpAudioDec_Create(AMP_AUDIODEC_HDLR_CFG_s *cfg,
                              AMP_AVDEC_HDLR_s **hdlr);

/**
 * To delete audio decode codec
 *
 * @param [in] hdlr - codec handler
 *
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpAudioDec_Delete(AMP_AVDEC_HDLR_s *hdlr);

/**
 * To config decoder parameter when idle
 *
 * @param [in] cfg - decoder config
 * @param [in] hdlr - codec handler
 *
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpAudioDec_DecoderCfg(AMP_AUDIODEC_DECODER_CFG_s *cfg,
                                  AMP_AVDEC_HDLR_s *hdlr);

/**
 * To init audio output task
 *
 * @param [in] outputCtrl - audio output resource
 * @param [in] priority - task priority
 *
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpAudio_OutputInit(UINT32* outputCtrl,
                               UINT8 priority);
/**
 * @}
 */
#endif /* AUDIODEC_H_ */
