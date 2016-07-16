 /**
  * @file inc/mw/recorder/AudioEnc.h
  *
  * Amba audio codec header
  *
  * Copyright (C) 2013, Ambarella, Inc.
  *
  * All rights reserved. No Part of this file may be reproduced, stored
  * in a retrieval system, or transmitted, in any form, or by any means,
  * electronic, mechanical, photocopying, recording, or otherwise,
  * without the prior consent of Ambarella, Inc.
  */

#ifndef _AUDIOENC_H_
#define _AUDIOENC_H_
/**
 * @defgroup AudioEnc
 * @brief Amba audio encode implementation
 *
 * Implementation the Amba audio encode module, include below function implementation
 *  1. Init Amba audio encode module function.
 *  2. Create Amba audio encode module function.
 *  3. Delete Amba audio encode module function.
 *  4. Get Amba audio encode module information
 *
 */

/**
 * @addtogroup AudioEnc
 * @ingroup Codec
 * @{
 */

#include <mw.h>
#include <recorder/Encode.h>
#include <AmbaAudio.h>

/**
 * amba callback
 */
typedef int (*AMP_CalibCB_f)(INT8 *curve, INT32 *dBFs, INT32 *THD_N);

/**
 *  init config
 */
typedef struct _AMP_AUDIOENC_INIT_CFG_s_ {
    UINT8* MemoryPoolAddr;           /**< working buffer start address */
    UINT32 MemoryPoolSize;           /**< size of the buffer */
} AMP_AUDIOENC_INIT_CFG_s;


typedef struct _AMP_AUDIOENC_INSTANCE_s_ {
    UINT32 *InputHdlr;                     /**< AIN handler of audio codec */
    UINT32 *EncodeHdlr;                    /**< AENC handler of audio codec */
    UINT32 *EncBuffHdlr;                   /**< AENC buffer handler of audio codec */
} AMP_AUDIOENC_INSTANCE_s;

/**
 * AudioEnc handler config
 */
typedef struct AMP_AUDIOENC_HDLR_CFG_s_ {
    AMBA_AUDIO_TYPE_e EncType;
    UINT8 EventDataReadySkipNum;            /**< fire "data ready" event to fifo per (1 + evenDataReadySkipNum) frames */
    UINT8 EncoderTaskPriority;              /**< Audio encoder task priority */
    UINT8 EncoderTaskCoreSelection;         /**< Audio encoder task core selection */
    UINT8 InputTaskPriority;                /**< Audio input receiver task priority */
    UINT8 InputTaskCoreSelection;           /**< Audio input receiver task core selection */
    UINT32 FadeInTime;                      /**< fade in when encode start */
    UINT32 FadeOutTime;                     /**< fade out when encode stop */
    UINT32 SrcSampleRate;                   /**< Sample frequency of the input */
    UINT32 DstSampleRate;                   /**< Sample frequency of the output */
    UINT16 SrcChannelMode;                  /**< Channel mode of the input */
    UINT16 DstChannelMode;                  /**< Channel mode of the output */
    UINT8 NumInstance;                      /**< number of audio instance in this handler */
    AMP_AUDIOENC_INSTANCE_s *AudioInstance; /**< Audio encode instance relative to audio_lib */
    union {
        AMBA_AUDIO_PCM_CONFIG_s PCMCfg;     /**< PCM encoder data structure */
        AMBA_AUDIO_AACENC_CONFIG_s AACCfg;  /**< AAC encoder data structure */
        AMBA_AUDIO_ADPCM_CONFIG_s ADPCMCfg; /**< ADPCM encoder data structure */
        AMBA_AUDIO_AC3ENC_CONFIG_s AC3Cfg;  /**< Dolby AC3 encoder data structure */
        AMBA_AUDIO_OPUSENC_CONFIG_s OpusCfg;/**< Opus encoder data structure */
    } Spec;
    AMP_CALLBACK_f cbCfgUpdated;            /**< setting take effect */
    AMP_CALLBACK_f cbEvent;                 /**< Event related to the encoder */
    AMP_ENC_BITSBUFFER_CFG_s BitsBufCfg;       /**< buffer config */
    INT8 *CalibCurveAddr;                  /**< Calib CurveAddress */
    INT32 *CalibworkBuffer;                     /**< Calibration working buffer address */
} AMP_AUDIOENC_HDLR_CFG_s;

/**
 * Configure an existing Audio handler
 */
typedef struct _AMP_AUDIOENC_CFG_s_ {
    UINT32 EventDataReadySkipNum;           /**< fire "data ready" event to fifo per (1 + evenDataReadySkipNum) frames */
    UINT32 FadeInTime;                      /**< fade in when encode start */
    UINT32 FadeOutTime;                     /**< fade out when encode stop */
    UINT32 SrcSampleRate;                   /**< Sample frequency of the input */
    UINT32 DstSampleRate;                   /**< Sample frequency of the output */
    UINT16 SrcChannelMode;                  /**< Channel mode of the input */
    UINT16 DstChannelMode;                  /**< Channel mode of the output */
    union {
        AMBA_AUDIO_PCM_CONFIG_s PCMCfg;     /**< PCM encoder data structure */
        AMBA_AUDIO_AACENC_CONFIG_s AACCfg;  /**< AAC encoder data structure */
        AMBA_AUDIO_ADPCM_CONFIG_s ADPCMCfg; /**< ADPCM encoder data structure */
        AMBA_AUDIO_AC3ENC_CONFIG_s AC3Cfg;  /**< Dolby AC3 encoder data structure */
        AMBA_AUDIO_OPUSENC_CONFIG_s OpusCfg;/**< Opus encoder data structure */
    } Spec;
    AMBA_AUDIO_TYPE_e EncType;
} AMP_AUDIOENC_CFG_s;


/**
 * Audioenc encoding session information
 */
typedef struct _AMP_AUDIOENC_ENCODING_INFO_s_ {
    UINT32 TotalFrames;         /**< Total encoded frames until now */
    UINT64 TotalBytes;          /**< Total encoded bytes until now, unit: Bytes */
} AMP_AUDIOENC_ENCODING_INFO_s;


/**
 * Get audio encode module default config for initialization
 *
 * @param [out] cfg audio encode module config
 *
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpAudioEnc_GetInitDefaultCfg(AMP_AUDIOENC_INIT_CFG_s *defInitCfg);

/**
 * Initialize audio encode module.\n
 * The function should only be invoked once.
 * User MUST invoke this function before using audio encode module.
 * The memory pool of the module will provide by user.
 *
 * @param [in] cfg audio encode module config
 *
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpAudioEnc_Init(AMP_AUDIOENC_INIT_CFG_s *cfg);

/**
 * Get full default setting of the audio codec
 *
 * @param [out] defCfg default config
 *
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpAudioEnc_GetDefaultCfg(AMP_AUDIOENC_HDLR_CFG_s *defCfg);

/**
 * Create an audio handler and set initial configuration
 *
 * @param [in] cfg initial configuration
 * @param [in,out] Hdlr audio encoder instance
 *
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpAudioEnc_Create(AMP_AUDIOENC_HDLR_CFG_s *cfg, AMP_AVENC_HDLR_s **aencHdlr);

/**
 * Delete an audio handler
 *
 * @param [in] Hdlr audio encoder instance
 *
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpAudioEnc_Delete(AMP_AVENC_HDLR_s *hdlr);

/**
 * Config audio encode handler
 *
 * @param [in] Hdlr audio encoder instance
 * @param [in] cfg encoder config
 *
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpAudioEnc_Config(AMP_AVENC_HDLR_s *hdlr, AMP_AUDIOENC_CFG_s *cfg);

/**
 * audio calibration create
 *
 * @param [in] workCacheBuf audio work cache buffer
 * @param [in] workNCBuf audio work non-cache buffer
 * @param [in] input audio io create info
 *
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpAudioCalib_Create(UINT8 *workCacheBuf, UINT8 *workNCBuf,AMBA_AUDIO_IO_CREATE_INFO_s *input);

/**
 * audio calibration setup
 *
 * @param [in] CalibInputHdlr Ain input hdlr
 * @param [in] pConfig audio calibration control info
 * @param [in] calib_cb audio calibration callback function
 *
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpAudioCalib_Setup(AMBA_AUDIO_CALIB_CTRL_s *pConfig, AMP_CalibCB_f cbClib);

/**
 * audio calibration disable
 *
 * @param [in] calib_cb audio calibration callback function
 *
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpAudioCalib_Disable(void);


/**
 * audio calibration delete
 *
 * @param [in] calib_cb audio calibration callback function
 *
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpAudioCalib_Delete(void);


/**
 * @}
 */

#endif /* _AUDIOENC_H_ */
