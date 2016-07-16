/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaAudio_CODEC.h
 *
 *  @Copyright      :: Copyright (C) 2012 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Audio AD/DA CODEC driver
\*-------------------------------------------------------------------------------------------------------------------*/

#ifndef _AMBA_AUDIO_CODEC_H_
#define _AMBA_AUDIO_CODEC_H_

typedef enum _AMBA_AUDIO_CODEC_INDEX_e_ {
    AMBA_AUDIO_CODEC_0,     /* 0th Audio CODEC */

    AMBA_NUM_AUDIO_CODEC    /* Number of Audio CODEC */
} AMBA_AUDIO_CODEC_INDEX_e;

typedef enum _AMBA_AUDIO_CODEC_MODE_e_ {
    LeftJustified,
    RightJustified,
    MSBExtend,
    I2S,
    DSP
} AMBA_AUDIO_CODEC_MODE_e;

typedef enum _AMBA_AUDIO_CODEC_OUTPUT_MODE_e_ {
    AMBA_AUDIO_CODEC_LINE_OUT     =   (0x1<<1),
    AMBA_AUDIO_CODEC_SPEAKER_OUT  =   (0x1),
} AMBA_AUDIO_CODEC_OUTPUT_MODE_e;

typedef enum _AMBA_AUDIO_CODEC_INPUT_MODE_e_ {
    AMBA_AUDIO_CODEC_LINE_IN  =   (0x1<<1),
    AMBA_AUDIO_CODEC_MIC_IN   =   (0x1),
} AMBA_AUDIO_CODEC_INPUT_MODE_e;

typedef struct _AMBA_AUDIO_CODEC_OBJ_s_ {
    int (*Init)(void);
    int (*ModeConfig)(AMBA_AUDIO_CODEC_MODE_e Mode);
    int (*FreqConfig)(UINT32 Freq);
    int (*SetOutput)(AMBA_AUDIO_CODEC_OUTPUT_MODE_e OutputMode);
    int (*SetInput)(AMBA_AUDIO_CODEC_INPUT_MODE_e InputMode);
    int (*SetMute)(UINT32 MuteEnable);
    int (*Write)(UINT16 Addr, UINT16 Data);
    int (*Read)(UINT16 Addr);
} AMBA_AUDIO_CODEC_OBJ_s;

/*-----------------------------------------------------------------------------------------------*\
 * Defined in AmbaAudio_CODEC.c
\*-----------------------------------------------------------------------------------------------*/
extern AMBA_AUDIO_CODEC_OBJ_s *pAmbaAudioCodecObj[];

#define AmbaAduio_CodecHook(Index, CodecObj)        pAmbaAudioCodecObj[Index] = CodecObj
#define AmbaAudio_CodecInit(Index)                  pAmbaAudioCodecObj[Index]->Init()
#define AmbaAudio_CodecModeConfig(Index, Mode)      pAmbaAudioCodecObj[Index]->ModeConfig(Mode)
#define AmbaAudio_CodecFreqConfig(Index, Freq)      pAmbaAudioCodecObj[Index]->FreqConfig(Freq)
#define AmbaAudio_CodecSetOutput(Index, OutputMode) pAmbaAudioCodecObj[Index]->SetOutput(OutputMode)
#define AmbaAudio_CodecSetInput(Index, InputMode)   pAmbaAudioCodecObj[Index]->SetInput(InputMode)
#define AmbaAudio_CodecSetMute(Index, MuteEnable)   pAmbaAudioCodecObj[Index]->SetMute(MuteEnable)
#define AmbaAudio_CodecWrite(Index, Addr, Data)     pAmbaAudioCodecObj[Index]->Write(Addr, Data)
#define AmbaAudio_CodecRead(Index, Addr)            pAmbaAudioCodecObj[Index]->Read(Addr)

#endif /* _AMBA_AUDIO_CODEC_H_ */
