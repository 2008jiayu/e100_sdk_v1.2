/**
 * @file inc/mw/player/beep.h
 *
 * Amba Beep service header
 *
 * Copyright (C) 2013, Ambarella, Inc.
 *
 * All rights reserved. No Part of this file may be reproduced, stored
 * in a retrieval system, or transmitted, in any form, or by any means,
 * electronic, mechanical, photocopying, recording, or otherwise,
 * without the prior consent of Ambarella, Inc.
 */ 
#ifndef BEEP_H_
#define BEEP_H_

#include <AmbaAudio.h>

typedef struct _AMP_BEEP_INIT_CFG_t_ {
    UINT16 SrcChannelMode; /**< Channel mode of the input */
    UINT32 MaxSampleRate; /**< Max supporting sample frequency */
    UINT32 MaxChannelNum; /**< Max channel number */
    UINT32 MaxFrameSize; /**< Max frame size */
    UINT32 MaxChunkNum; /**< Max chunk number */
    UINT8 I2SIndex; /**< I2SIndex of output */
    AMBA_AUDIO_PCM_CONFIG_s PcmCfg; /** decode info */
    UINT8 *CodecCacheWorkBuff; /**< buffer address for audio codec */
    UINT32 CodecCacheWorkSize; /**< buffer size for audio codec */
    UINT32 DecTaskPriority; /**< beep decode task priority */
    UINT32 DecTaskCoreSelectionBitMap; /**< beep task core selection */
} AMP_BEEP_INIT_CFG_t;

typedef struct _AMP_BEEP_t_ {
    WCHAR *Fn; /**< file name  of beep sound */
    UINT8 *RawPcm; /**< raw buffer of beep sound */
    UINT32 RawSize; /**< size of file */
    UINT32 SrcSampleRate; /**< source sample rate of beep */
    UINT32 SrcChannelMode; /**< channel mode of beep */
    UINT32 DstSampleRate; /**< dst sample rate of beep */
} AMP_BEEP_t;

/**
 * To init beep module
 *
 * @param [in] cfg - configuration of beep module
 *
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpBeep_Init(AMP_BEEP_INIT_CFG_t *cfg);

/**
 * To output beep sound
 *
 * @param [in] beep - beep sound info
 *
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpBeep_Beep(AMP_BEEP_t *beep);

/**
 * To set beep volume (0~64)
 * 
 * @param [in] volume - volume level
 *
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e 
 */
extern int AmpBeep_SetVol(UINT32 volume);

/**
 * To stop current beep
 * 
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e 
 */
extern int AmpBeep_Stop(void);

#endif /* BEEP_H_ */
