/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaAudio_Tlv320Aic3256.c
 *
 *  @Copyright      :: Copyright (C) 2012 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: TI tlv320aic3256 audio AD/DA CODEC driver
\*-------------------------------------------------------------------------------------------------------------------*/

#include "AmbaDataType.h"
#include "AmbaKAL.h"

#include "AmbaI2C.h"
#include "AmbaAudio_TLV320AIC3256.h"
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

static int Tlv320Aic3256_SendSeq(const RegValue *pSeq, int Nseq);

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: Tlv320Aic3256_Write
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
static int Tlv320Aic3256_Write(UINT16 Addr, UINT16 Data)
{
    UINT8 TxDataBuf[2];

    TxDataBuf[0] = (UINT8)Addr;
    TxDataBuf[1] = (UINT8)Data;

    CODEC_DBG("%s: Addr: %d, Data: %d", __func__, Addr, Data);

    if (Addr < TLV320AIC3256_REG_NUM) {
        return AmbaI2C_Write(AMBA_I2C_CHANNEL0, AMBA_I2C_SPEED_STANDARD,
                             TLV320AIC3256_I2C_ADDR, sizeof(TxDataBuf), TxDataBuf,
                             AMBA_KAL_WAIT_FOREVER);
    } else {
        CODEC_DBG("Tlv320Aic3256: Wrong address number! %d", Addr);
        return NG;
    }
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: Tlv320Aic3256_Read
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
static int Tlv320Aic3256_Read(UINT16 Addr)
{
    UINT8 RxDataBuf;

    UINT16 WorkUINT16[3];

    WorkUINT16[0] = AMBA_I2C_RESTART_FLAG | TLV320AIC3256_I2C_ADDR; /* Slave Address + r/w (0) */
    WorkUINT16[1] = Addr;                                           /* Sub Address */
    WorkUINT16[2] = AMBA_I2C_RESTART_FLAG | TLV320AIC3256_I2C_ADDR | 0x01;

    if (Addr < TLV320AIC3256_REG_NUM) {
        if (AmbaI2C_ReadAfterWrite(AMBA_I2C_CHANNEL0, AMBA_I2C_SPEED_STANDARD,
                                   3, WorkUINT16, 1, &RxDataBuf, AMBA_KAL_WAIT_FOREVER) != OK) {
            return NG;
        }
        CODEC_DBG("%s: Addr: %d, Data: %d", __func__, Addr, RxDataBuf);
        return RxDataBuf;
    } else {
        return NG;
    }
}

/**
 * Audio Codec Reset
 */
static int Tlv320Aic3256_SoftwareReset(void)
{
    return Tlv320Aic3256_SendSeq(SEND_SEQ(CodecReset));
}

/**
 * Audio Codec Initialization
 */
static int Tlv320Aic3256_Init(void)
{
    if (OK != Tlv320Aic3256_SoftwareReset())
        return NG;
    if (OK != Tlv320Aic3256_SendSeq(SEND_SEQ(CodecClkSetting)))
        return NG;
    if (OK != Tlv320Aic3256_SendSeq(SEND_SEQ(CodecSignalProc)))
        return NG;
    return Tlv320Aic3256_SendSeq(SEND_SEQ(CodecInit));
}

/**
 * Change operating frequency
 */
static int Tlv320Aic3256_FreqConfig(UINT32 Freq)
{
    return OK;
}

/**
 * Mode configuration
 */
static int Tlv320Aic3256_ModeConfig(AMBA_AUDIO_CODEC_MODE_e Mode)
{
    return OK;
}

/**
 * DAC Mute
 */
static int Tlv320Aic3256_SetMute(UINT32 MuteEnable)
{
    return OK;
}

/**
 * Turn On Recording Path
 */
static int Tlv320Aic3256_MicInputOn(void)
{
    return Tlv320Aic3256_SendSeq(SEND_SEQ(RecordingOn));
}

/**
 * Turn Off Recording Path
 */
static int Tlv320Aic3256_MicInputOff(void)
{
    return Tlv320Aic3256_SendSeq(SEND_SEQ(RecordingOff));
}

/**
 * Turn on Playback Path
 */
static int Tlv320Aic3256_LineoutPowerOn(void)
{
    if (OK != Tlv320Aic3256_SendSeq(SEND_SEQ(CodecProcBlocks)))
        return NG;
    if (OK != Tlv320Aic3256_SendSeq(SEND_SEQ(CodecPowerSupplies)))
        return NG;
    return Tlv320Aic3256_SendSeq(SEND_SEQ(PlaybackOn));
}

/**
 * Turn Off Playback Path
 */
static int Tlv320Aic3256_LineoutPowerOff(void)
{
    return Tlv320Aic3256_SendSeq(SEND_SEQ(PlaybackOff));
}

#if 0 /* Wait for TI FAE fix bug */
/**
 * Turn on Headphone Path
 */
static int Tlv320Aic3256_HpOutOn(void)
{
    return Tlv320Aic3256_SendSeq(SEND_SEQ(CodecHpOutOn));
}

/**
 * Turn Off Headphone Path
 */
static int Tlv320Aic3256_HpOutOff(void)
{
    return Tlv320Aic3256_SendSeq(SEND_SEQ(CodecHpOff));
}
#endif

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: Tlv320Aic3256_SetOutput
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
static int Tlv320Aic3256_SetOutput(AMBA_AUDIO_CODEC_OUTPUT_MODE_e OutputMode)
{
    if (OutputMode & AMBA_AUDIO_CODEC_LINE_OUT) {
        Tlv320Aic3256_LineoutPowerOn();
    } else {
        Tlv320Aic3256_LineoutPowerOff();
    }
#if 0 /* Wait for TI FAE fix bug */
    if (OutputMode & AMBA_AUDIO_CODEC_SPEAKER_OUT) {
        Tlv320Aic3256_HpOutOn();
    } else {
        Tlv320Aic3256_HpOutOff();
    }
#endif
    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: Tlv320Aic3256_SetInput
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
static int Tlv320Aic3256_SetInput(AMBA_AUDIO_CODEC_INPUT_MODE_e InputMode)
{
    if (InputMode == AMBA_AUDIO_CODEC_MIC_IN) {
        Tlv320Aic3256_MicInputOn();
    } else {
        Tlv320Aic3256_MicInputOff();
    }
    return OK;
}

static int Tlv320Aic3256_SendSeq(const RegValue *pSeq, int Nseq)
{
    UINT16 i, Data, Cmd = 0;
    CODEC_DBG("Tlv320Aic3256_SendSeq start!!");
    for (i = 0; i < Nseq; ++i) {
        switch (pSeq[i].RegOff) {
        case REG_META_DELAY:
            AmbaKAL_TaskSleep(pSeq[i].RegVal);
            break;
        case REG_META_SET:
        case REG_META_RESET:
        case REG_META_POLL:
            Cmd = pSeq[i].RegVal;
            break;
        default:
            if (Cmd) {
                do {
                    Data = Tlv320Aic3256_Read(pSeq[i].RegOff);
                    CODEC_DBG("Data1 = %d", Data);
                } while (Cmd == REG_META_POLL && (Data & pSeq[i].RegVal) != pSeq[i].RegVal);
                if (Cmd == REG_META_SET) {
                    Data |= pSeq[i].RegVal;
                } else if (Cmd == REG_META_RESET) {
                    Data &= ~pSeq[i].RegVal;
                }
            } else {
                Data  = pSeq[i].RegVal;
            }
            if (Cmd != REG_META_POLL) {
                Tlv320Aic3256_Write(pSeq[i].RegOff , Data);
                Data = Tlv320Aic3256_Read(pSeq[i].RegOff);
                CODEC_DBG("Data2 = %d", Data);
            }
            Cmd = 0;
        }
    }
    CODEC_DBG("Tlv320Aic3256_SendSeq end!!");
    return OK;
}

AMBA_AUDIO_CODEC_OBJ_s AmbaAudio_Tlv320Aic3256Obj = {
    .Init       = Tlv320Aic3256_Init,
    .ModeConfig = Tlv320Aic3256_ModeConfig,
    .FreqConfig = Tlv320Aic3256_FreqConfig,
    .SetOutput  = Tlv320Aic3256_SetOutput,
    .SetInput   = Tlv320Aic3256_SetInput,
    .SetMute    = Tlv320Aic3256_SetMute,
    .Write      = Tlv320Aic3256_Write,
    .Read       = Tlv320Aic3256_Read,
};
