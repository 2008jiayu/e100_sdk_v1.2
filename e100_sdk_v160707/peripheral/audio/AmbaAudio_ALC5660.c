/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaAudio_ALC5660.c
 *
 *  @Copyright      :: Copyright (C) 2012 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: REALTEK ALC5660 audio AD/DA CODEC driver
\*-------------------------------------------------------------------------------------------------------------------*/

#include "AmbaDataType.h"
#include "AmbaKAL.h"

#include "AmbaI2C.h"
#include "AmbaAudio_ALC5660.h"
#include "AmbaAudio_CODEC.h"

//#define AMBA_AUDIO_CODEC_DBG
#ifdef AMBA_AUDIO_CODEC_DBG
#include "AmbaPrintk.h"
#endif
#ifdef AMBA_AUDIO_CODEC_DBG
#define CODEC_DBG   AmbaPrint
#else
#define CODEC_DBG(...)
#endif

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: Alc5660_Read
 *
 *  @Description:: Audio Codec read register
 *
 *  @Input      ::
 *      Addr:   I2C Slave Address
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : I2C read Data / NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static int Alc5660_Read(UINT16 Addr)
{
    UINT16 WorkUINT16[3];
    UINT8 RxDataBuf[2];

    WorkUINT16[0] = AMBA_I2C_RESTART_FLAG | ALC5660_I2C_ADDR; /* Slave Address + r/w (0) */
    WorkUINT16[1] = Addr;                                           /* Sub Address */
    WorkUINT16[2] = AMBA_I2C_RESTART_FLAG | ALC5660_I2C_ADDR | 0x01;

    if (Addr < ALC5660_REG_NUM) {
        if (AmbaI2C_ReadAfterWrite(AMBA_I2C_CHANNEL0, AMBA_I2C_SPEED_FAST,
                                   3, WorkUINT16, 2, (UINT8 *)&RxDataBuf, AMBA_KAL_WAIT_FOREVER) != OK) {
            return NG;
        }
        CODEC_DBG("%s: Addr: %d, Data: 0x%x 0x%x", __func__, Addr, RxDataBuf[0],  RxDataBuf[1]);
        return (RxDataBuf[0] << 8) | (RxDataBuf[1]);
    } else {
        return NG;
    }
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: Alc5660_Write
 *
 *  @Description:: Audio Codec write register
 *
 *  @Input      ::
 *      Addr:   I2C Slave Address
 *      Data:   I2C write Data
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static int Alc5660_Write(UINT16 Addr, UINT16 Data)
{
    UINT8 TxDataBuf[3];
    int rval = 0;

    TxDataBuf[0] = (UINT8)Addr;
    TxDataBuf[1] = (UINT8)((Data & 0xFF00) >> 8);
    TxDataBuf[2] = (UINT8)(Data & 0x00FF);

    CODEC_DBG("%s: Addr: 0x%02x, Data: 0x%04x", __func__, Addr, Data);

    if (Addr < ALC5660_REG_NUM) {
        rval = AmbaI2C_Write(AMBA_I2C_CHANNEL0, AMBA_I2C_SPEED_FAST,
                             ALC5660_I2C_ADDR, 3, TxDataBuf,
                             AMBA_KAL_WAIT_FOREVER);
        Data = Alc5660_Read(Addr);
        CODEC_DBG(">>> Read back, Addr = 0x%02x 0x%04x", Addr, Data);

        return rval;
    } else {
        CODEC_DBG("Alc5660: Wrong address number! %d", Addr);
        return NG;
    }
}

static int Alc5660_Write_Mask(UINT16 Addr, UINT16 Data, UINT16 Mask)
{
    UINT16 CodecData = 0;

    if(!Mask)
        return OK;

    if(Mask != 0xffff) {
        CodecData = Alc5660_Read(Addr);
        //CODEC_DBG("Alc5660_Write_Mask Read Data 0x%04x \n", CodecData);
        CodecData &= ~Mask;
        CodecData |= (Data & Mask);
        //CODEC_DBG("Alc5660_Write_Mask 0x%02x 0x%04x 0x%4x\n", Addr, CodecData, Mask);
        //CODEC_DBG("Alc5660_Write: Addr: 0x%02x 0x%04x\n", Addr, CodecData);
        Alc5660_Write(Addr, CodecData);
    } else {
        Alc5660_Write(Addr, Data);
    }
    return OK;
}

/**
 * Audio Codec Initialization
 */
static int Alc5660_Init(void)
{
    Alc5660_Write(ALC5660_RESET, 0x0);
    Alc5660_Write(ALC5660_DUMMY_REG_1, 0x0821);//0xfa

    //set pr3d = 0x3600
    Alc5660_Write(ALC5660_PRI_REG_INDEX, 0x003d);
    Alc5660_Write(ALC5660_PRI_REG_DATA, 0x3600);

    Alc5660_Write(ALC5660_STEREO_DAC_MIXER, 0x0202);//0x2a
    Alc5660_Write(ALC5660_ADC_DAC_CLK_1, 0x0000);//0x73

    //set Dmic path
    Alc5660_Write(ALC5660_GPIO_1, 0x1000);
    Alc5660_Write(ALC5660_GPIO_3, 0x0004);
    Alc5660_Write(ALC5660_DIG_MIC, 0xA880);//0x75
    Alc5660_Write(ALC5660_STEREO_ADC_MIXER, 0x4040);//0x27

    //set HP path
    Alc5660_Write(ALC5660_HPMIX, 0xA000);//0x45

    //set SPK path
    Alc5660_Write(ALC5660_SPKMIX, 0xD800);//0x48

    return OK;
}

/**
 * Change operating frequency
 */
static int Alc5660_FreqConfig(UINT32 Freq)
{
    return OK;
}

/**
 * Mode configuration
 */
static int Alc5660_ModeConfig(AMBA_AUDIO_CODEC_MODE_e Mode)
{
    UINT16 data;

    data = Alc5660_Read(ALC5660_I2S_SDP) & 0xfffc;

    switch(Mode) {
        case LeftJustified:
            data |= ((UINT16)SDP_I2S_DF_LEFT);
            break;
        case RightJustified:
            CODEC_DBG("not supported mode\n");
            break;
        case MSBExtend:
            CODEC_DBG("not supported mode\n");
        case I2S:
            data |= ((UINT16)SDP_I2S_DF_I2S);
            break;
        case DSP:
            data |= ((UINT16)SDP_I2S_DF_PCM_A);
            break;
    }

    data |= ((UINT16)SDP_I2S_DL_24);

    Alc5660_Write(ALC5660_I2S_SDP, data);
    return OK;
}

/**
 * DAC Mute
 */
static int Alc5660_SetMute(UINT32 MuteEnable)
{
    if (MuteEnable) {
        Alc5660_Write_Mask(ALC5660_SPK_OUT_VOL, 0x8000, 0x8000);
        Alc5660_Write_Mask(ALC5660_HP_OUT_VOL, 0x8080, 0x8080);
    } else {
        Alc5660_Write_Mask(ALC5660_SPK_OUT_VOL,0x0000,0x8000);
        Alc5660_Write_Mask(ALC5660_HP_OUT_VOL,0x0000,0x8080);
    }
    return OK;
}

/**
 * Turn On Recording Path
 */
static int Alc5660_MicInputOn(void)
{
    Alc5660_Write_Mask(ALC5660_PWR_MANAG_DIG_1, 0x8000, 0x8000);
    Alc5660_Write_Mask(ALC5660_PWR_MANAG_DIG_2, 0x8000, 0x8000);
    return OK;
}

/**
 * Turn Off Recording Path
 */
static int Alc5660_MicInputOff(void)
{
    Alc5660_Write_Mask(ALC5660_PWR_MANAG_DIG_2, 0x0000, 0x8000);
    return OK;
}

/**
 * Turn on Playback Path
 */
static int Alc5660_SpeakerOutPowerOn(void)
{
    Alc5660_Write_Mask(ALC5660_PWR_MANAG_ANALOG_1, 0xA830, 0xE838);
    AmbaKAL_TaskSleep(20);
    Alc5660_Write_Mask(ALC5660_PWR_MANAG_ANALOG_1, 0xE838, 0xE838);
    Alc5660_Write_Mask(ALC5660_PWR_MANAG_DIG_1, 0x9800, 0x9800);
    Alc5660_Write_Mask(ALC5660_PWR_MANAG_DIG_2, 0x0800, 0x0800);

    Alc5660_Write_Mask(ALC5660_SPK_OUT_VOL,0x0000, 0x8000);
    Alc5660_Write_Mask(ALC5660_PWR_MANAG_DIG_1, 0x0001, 0x0001);
    return OK;
}

/**
 * Turn Off Playback Path
 */
static int Alc5660_SpeakerOutPowerOff(void)
{
    Alc5660_Write_Mask(ALC5660_PWR_MANAG_ANALOG_1, 0x0000, 0xE838);
    Alc5660_Write_Mask(ALC5660_PWR_MANAG_DIG_1, 0x0000, 0x0001);
    Alc5660_Write_Mask(ALC5660_SPK_OUT_VOL, 0x8000, 0x8000);
    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: Alc5660_SetOutput
 *
 *  @Description:: Audio Codec set output path
 *
 *  @Input      ::
 *      AMBA_AUDIO_CODEC_OUTPUT_MODE:   Audio Codec Output path
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static int Alc5660_SetOutput(AMBA_AUDIO_CODEC_OUTPUT_MODE_e OutputMode)
{
    if (OutputMode & AMBA_AUDIO_CODEC_SPEAKER_OUT) {
        Alc5660_SpeakerOutPowerOn();
    } else {
        Alc5660_SpeakerOutPowerOff();
    }
    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: Alc5660_SetInput
 *
 *  @Description:: Audio Codec set input path
 *
 *  @Input      ::
 *      AMBA_AUDIO_CODEC_INPUT_MODE:   Audio Codec Input path
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static int Alc5660_SetInput(AMBA_AUDIO_CODEC_INPUT_MODE_e InputMode)
{
    if (InputMode == AMBA_AUDIO_CODEC_MIC_IN) {
        Alc5660_MicInputOn();
    } else {
        Alc5660_MicInputOff();
    }
    return OK;
}

AMBA_AUDIO_CODEC_OBJ_s AmbaAudio_Alc5660Obj = {
    .Init       = Alc5660_Init,
    .ModeConfig = Alc5660_ModeConfig,
    .FreqConfig = Alc5660_FreqConfig,
    .SetOutput  = Alc5660_SetOutput,
    .SetInput   = Alc5660_SetInput,
    .SetMute    = Alc5660_SetMute,
    .Write      = Alc5660_Write,
    .Read       = Alc5660_Read,
};
