/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaAudio_WM8974.c
 *
 *  @Copyright      :: Copyright (C) 2012 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: WM8974 audio AD/DA CODEC driver
\*-------------------------------------------------------------------------------------------------------------------*/

#include "AmbaDataType.h"
#include "AmbaKAL.h"

#include "AmbaI2C.h"
#include "AmbaAudio_WM8974.h"
#include "AmbaAudio_CODEC.h"

#define AMBA_AUDIO_CODEC_DBG
#ifdef AMBA_AUDIO_CODEC_DBG
#include    "AmbaPrintk.h"
#endif
#ifdef AMBA_AUDIO_CODEC_DBG
#define AMBA_AUDIO_CODEC_PRINT    AmbaPrint
#else
#define AMBA_AUDIO_CODEC_PRINT(...)
#endif

/* Register default values */
UINT16 WM8974_RegCache[WM8974_REG_NUM] = {
    0x000,  //0x0  : Software rest
    0x000,  //0x1  : Powermanagerment1
    0x000,  //0x2  : Powermanagerment2
    0x000,  //0x3  : Powermanagerment3
    0x050,  //0x4  : Audio interface
    0x000,  //0x5  : Companding ctrl
    0x140,  //0x6  : Clock gen ctrl
    0x000,  //0x7  : Addtional ctrl
    0x000,  //0x8  : GPIO
    0x000,  //0x9  : Jack detect control(WM8978 only)
    0x000,  //0xA  : DAC ctrl
    0x0ff,  //0xB  : DAC vol ctrl
    0x000,  //0xC  : (WM8978 only)
    0x000,  //0xD  : (WM8978 only)
    0x100,  //0xE  : ADC ctrl
    0x0ff,  //0xF  : (WM8978 only)
    0x000,  //0x10 : (WM8978 only)
    0x000,  //0x11 : reserved
    0x12c,  //0x12 : EQ1
    0x02c,  //0x13 : EQ2
    0x02c,  //0x14 : EQ3
    0x02c,  //0x15 : EQ4
    0x02c,  //0x16 : EQ5
    0x000,  //0x17 : reserved
    0x032,  //0x18 : DAC Limiter1
    0x000,  //0x19 : DAC Limiter2
    0x000,  //0x1A : reserved
    0x000,  //0x1B : Notch Filter1
    0x000,  //0x1C : Notch Filter2
    0x000,  //0x1D : Notch Filter3
    0x000,  //0x1E : Notch Filter4
    0x000,  //0x1F : reserved
    0x038,  //0x20 : ALC control1
    0x00b,  //0x21 : ALC control2
    0x032,  //0x22 : ALC control3
    0x000,  //0x23 : Noise Gate
    0x008,  //0x24 : PLL N
    0x00c,  //0x25 : PLL K1
    0x093,  //0x26 : PLL K2
    0x0e9,  //0x27 : PLL K3
    0x000,  //0x28 : Attenuation ctrl
    0x000,  //0x29 : (WM8978 only)
    0x000,  //0x2A : reserved
    0x000,  //0x2B : (WM8978 only)
    0x003,  //0x2C : Input ctrl
    0x010,  //0x2D : INP PGA gain ctrl
    0x000,  //0x2E : (WM8978 only)
    0x000,  //0x2F : ADC Boost ctrl(different reset value from wm8978)
    0x000,  //0x30 : (WM8978 only)
    0x002,  //0x31 : Ouptut ctrl
    0x000,  //0x32 : SPK mixer ctrl(different reset value from wm8978)
    0x000,  //0x33 : (WM8978 only)
    0x000,  //0x34 : (WM8978 only)
    0x000,  //0x35 : (WM8978 only)
    0x03f,  //0x36 : SPK volume ctrl
    0x000,  //0x37 : (WM8978 only)
    0x000,  //0x38 : MONO mixer ctrl(different reset value from wm8978)
};

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: WM8974_Write
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
static int WM8974_Write(UINT16 Addr, UINT16 Data)
{
    UINT16 Temp = 0;
    UINT8 TxDataBuf[2];

    if (Addr < WM8974_REG_NUM) {
        WM8974_RegCache[Addr] = Data;
        Temp = Data & 0x01ff;
        Temp = Temp >> 8;
        Addr = (Addr << 1 | Temp);
        TxDataBuf[0] = (UINT8)Addr;
        TxDataBuf[1] = (UINT8)Data;

        return AmbaI2C_Write(AMBA_I2C_CHANNEL0, AMBA_I2C_SPEED_FAST,
                           (WM8974_I2C_ADDR << 1), sizeof(TxDataBuf), TxDataBuf,
                           AMBA_KAL_WAIT_FOREVER);
    } else {
        return NG;
    }
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: WM8974_Read
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
static int WM8974_Read(UINT16 Addr)
{
    if (Addr < WM8974_REG_NUM) {
        return WM8974_RegCache[Addr];
    } else {
        return NG;
    }
}

static void WM8974_SpeakerOutPowerOn(void)
{
    UINT16 Data;

    Data = WM8974_Read(WM8974_PWR_MANAGE3_REG) |
    	((UINT16)WM8974_R03H_SPKPEN | (UINT16)WM8974_R03H_SPKNEN | (UINT16)WM8974_R03H_SPKMIXEN);
    WM8974_Write(WM8974_PWR_MANAGE3_REG, Data);
    Data = WM8974_Read(WM8974_SPK_MIXER_CTRL_REG) |
    	((UINT16)WM8974_R32H_DAC2SPK);
    WM8974_Write(WM8974_SPK_MIXER_CTRL_REG, Data);
}

static void WM8974_SpeakerOutPowerOff(void)
{
    UINT16 Data;

    Data = WM8974_Read(WM8974_PWR_MANAGE3_REG) &
    	((~((UINT16)WM8974_R03H_SPKPEN)) & (~((UINT16)WM8974_R03H_SPKNEN)) & (~((UINT16)WM8974_R03H_SPKMIXEN)));
    WM8974_Write(WM8974_PWR_MANAGE3_REG, Data);
    Data = WM8974_Read(WM8974_SPK_MIXER_CTRL_REG) &
    	(~((UINT16)WM8974_R32H_DAC2SPK));
    WM8974_Write(WM8974_SPK_MIXER_CTRL_REG, Data);
}

static void WM8974_MicInPowerOn(void)
{
    UINT16 Data;

    // open PGA path
    Data = WM8974_Read(WM8974_IN_PGA_VOL_REG) &
    	((UINT16)WM8974_R2DH_PGA_MASK) & (~((UINT16)WM8974_R2DH_INPPGAMUTE));
    //Data |= ((UINT16)WM8974_R2DH_PGA_MAX);
    Data |= ((UINT16)WM8974_R2DH_PGA_5dB);
    WM8974_Write(WM8974_IN_PGA_VOL_REG, Data);
    WM8974_Write(0x2c, 0x03);
    #ifdef ENABLE_ALC_NOISE_GATE
    // enable ALC, Nosie gate
    WM8974_Write(0x20, 0x138);  //Maximum Gain: +35.25dB, minimum gain: -12dB
    WM8974_Write(0x21, 0x00b);  // target level: 12dB FS
    WM8974_Write(0x22, 0x32);
    WM8974_Write(0x23, 0xc);    // Noise gate, level = -63dB
    #endif
    #ifdef ENABLE_NOTCH_FILTER
    // Notch filter, Fc=7887Hz, Fb=50Hz, Fs=48000Hz
    WM8974_Write(0x1b, 0xc0);
    WM8974_Write(0x1c, 0x35);
    WM8974_Write(0x1d, 0x20);
    WM8974_Write(0x1e, 0x15b);
    #endif

    // boost PGA path + 20dB and disable other paths
    WM8974_Write(0x2f, 0x100);
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: WM8974_SetOutput
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
static int WM8974_SetOutput(AMBA_AUDIO_CODEC_OUTPUT_MODE_e OutputMode)
{
    AMBA_AUDIO_CODEC_PRINT("%s: %d", __func__, OutputMode);
    if (OutputMode & AMBA_AUDIO_CODEC_SPEAKER_OUT) {
        WM8974_SpeakerOutPowerOn();
    } else {
        WM8974_SpeakerOutPowerOff();
    }
    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: WM8974_SetInput
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
static int WM8974_SetInput(AMBA_AUDIO_CODEC_INPUT_MODE_e InputMode)
{
    AMBA_AUDIO_CODEC_PRINT("%s: %d", __func__, InputMode);
    if (InputMode == AMBA_AUDIO_CODEC_MIC_IN) {
        WM8974_MicInPowerOn();
        return OK;
    } else {
        return NG;
    }
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: WM8974_SetMute
 *
 *  @Description:: Audio Codec Mute
 *
 *  @Input      ::
 *      MuteEnable:   1: Mute the Codec, 0: Un-mute the codec
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static int WM8974_SetMute(UINT32 MuteEnable)
{
    UINT16 Data;

    if (MuteEnable) {
        Data = WM8974_Read(WM8974_DAC_CTRL_REG) | ((UINT16)WM8974_R0AH_SOFTMUTE);
        WM8974_Write(WM8974_DAC_CTRL_REG, Data);
    } else {
        Data = WM8974_Read(WM8974_DAC_CTRL_REG) & (~((UINT16)WM8974_R0AH_SOFTMUTE));
        WM8974_Write(WM8974_DAC_CTRL_REG, Data);
    }
    AMBA_AUDIO_CODEC_PRINT("%s: %d", __func__, MuteEnable);
    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: WM8974_ModeConfig
 *
 *  @Description:: Audio Codec Data protocol configuration
 *
 *  @Input      ::
 *      Mode:   Audio Codec Data protocol
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static int WM8974_ModeConfig(AMBA_AUDIO_CODEC_MODE_e mode)
{
    UINT16 Data;

    switch(mode){
    case I2S:
    	break;
    default:
    	AmbaPrint("%s, not supported DAI mode now", __func__);
    	break;
    }

    Data = WM8974_Read(WM8974_AU_INTERFACE_CTRL_REG) &
    	((UINT16)WM8974_R04H_WL_MASK);

    /*
    switch(resolution) {
    case DAI_16bits:
    	Data &= ((UINT16)WM8974_R04H_WL_16B);
    	break;
    case DAI_24bits:
    	Data |= ((UINT16)WM8974_R04H_WL_24B);
    	break;
    default:
    	AmbaPrint("not supported resolution now");
    	break;
    } */
    Data |= ((UINT16)WM8974_R04H_WL_24B);
    WM8974_Write(WM8974_AU_INTERFACE_CTRL_REG, Data);
    AMBA_AUDIO_CODEC_PRINT("%s", __func__);
    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: WM8974_FreqConfig
 *
 *  @Description:: Audio Codec sampling frequency configuration
 *
 *  @Input      ::
 *      Freq:   Audio Codec sampling frequency (Hz)
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static int WM8974_FreqConfig(UINT32 Freq)
{
    UINT16 Data;

    Data = WM8974_Read(WM8974_ADDITIONAL_CTRL_REG) &
    	((UINT16)WM8974_R07H_SR_MASK);

    switch (Freq) {
    case 48000 :
    	Data |= ((UINT16)WM8974_R07H_SR_48K);
    	break;
    case 24000 :
    	Data |= ((UINT16)WM8974_R07H_SR_24K);
    	break;
    case 12000 :
    	Data |= ((UINT16)WM8974_R07H_SR_12K);
    	break;
    case 44100 :
    	Data |= ((UINT16)WM8974_R07H_SR_48K);
    	break;
    case 22050 :
    	Data |= ((UINT16)WM8974_R07H_SR_24K);
    	break;
    case 11025 :
    	Data |= ((UINT16)WM8974_R07H_SR_12K);
    	break;
    case 32000 :
    	Data |= ((UINT16)WM8974_R07H_SR_32K);
    	break;
    case 16000 :
    	Data |= ((UINT16)WM8974_R07H_SR_16K);
    	break;
    case 8000 :
    	Data |= ((UINT16)WM8974_R07H_SR_8K);
    	break;
    default:
    	AmbaPrint("%s, not supported frequency: %d", __func__, Freq);
      return NG;
    }
    WM8974_Write(WM8974_ADDITIONAL_CTRL_REG, Data);

    AMBA_AUDIO_CODEC_PRINT("WM8974_FreqConfig: %d", Freq);
    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: WM8974_Init
 *
 *  @Description:: Audio Codec Initialization
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static int WM8974_Init(void)
{
    UINT16 Data;
    INT32 Loop;

    /* Write default value to the instance  */
    for (Loop = 0; Loop < WM8974_REG_NUM; Loop++) {
      WM8974_Write(Loop, WM8974_RegCache[Loop]);
    }

    /* Invert ROUT2 output for speaker */
    /*
    Data = WM8974_Read(WM8974_BEEP_CTRL_REG) |
    	((UINT16)WM8974_R2BH_INVROUT2);
    WM8974_Write(WM8974_BEEP_CTRL_REG, Data);
    */

    /* Clock control, set codec as slave mode */
    Data = WM8974_Read(WM8974_CLK_GN_CTRL_REG) &
    	((UINT16)WM8974_R06H_CODEC_SLAVE);
    WM8974_Write(WM8974_CLK_GN_CTRL_REG, Data);

    /* Set BUFIOEN = 1 and VMIDSEL = 5kohm */
    Data = WM8974_Read(WM8974_PWR_MANAGE1_REG) &
    	((UINT16)WM8974_R01H_VMIDSEL_MASK);
    Data |= ((UINT16)WM8974_R01H_VMIDSEL_75K |
    	(UINT16)WM8974_R01H_BIASEN | (UINT16)WM8974_R01H_BUFIOEN);
    WM8974_Write(WM8974_PWR_MANAGE1_REG, Data);

    /* input boost enable*/
    Data = WM8974_Read(WM8974_PWR_MANAGE2_REG) |
    	((UINT16)WM8974_R02H_BOOSTEN);
    WM8974_Write(WM8974_PWR_MANAGE2_REG, Data);

    /* ADC power up */
    /* input PGA enable */
    Data = WM8974_Read(WM8974_PWR_MANAGE2_REG) |
    	((UINT16)WM8974_R02H_INPPGAEN);
    /* ADC enable */
    Data |= ((UINT16)WM8974_R02H_ADCEN);
    WM8974_Write(WM8974_PWR_MANAGE2_REG, Data);

    /* Mic Bias enable */
    Data = WM8974_Read(WM8974_PWR_MANAGE1_REG) |
    	((UINT16)WM8974_R01H_MICBEN);
    WM8974_Write(WM8974_PWR_MANAGE1_REG, Data);

    /* DAC power up */
    Data = WM8974_Read(WM8974_PWR_MANAGE3_REG) |
    	((UINT16)WM8974_R03H_DACEN);
    WM8974_Write(WM8974_PWR_MANAGE3_REG, Data);

    AMBA_AUDIO_CODEC_PRINT("%s", __func__);
    return OK;
}

AMBA_AUDIO_CODEC_OBJ_s AmbaAudio_WM8974Obj = {
    .Init       = WM8974_Init,
    .ModeConfig = WM8974_ModeConfig,
    .FreqConfig = WM8974_FreqConfig,
    .SetOutput  = WM8974_SetOutput,
    .SetInput   = WM8974_SetInput,
    .SetMute    = WM8974_SetMute,
    .Write      = WM8974_Write,
    .Read       = WM8974_Read,
};
