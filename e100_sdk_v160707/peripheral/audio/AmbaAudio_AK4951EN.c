/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaAudio_AK4951EN.c
 *
 *  @Copyright      :: Copyright (C) 2015 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: AK4951EN audio AD/DA CODEC driver
\*-------------------------------------------------------------------------------------------------------------------*/

#include "AmbaDataType.h"
#include "AmbaKAL.h"

#include "AmbaI2C.h"
#include "AmbaAudio_AK4951EN.h"
#include "AmbaAudio_CODEC.h"

#define AMBA_AUDIO_CODEC_DBG
#ifdef AMBA_AUDIO_CODEC_DBG
#include "AmbaPrintk.h"
#endif
#ifdef AMBA_AUDIO_CODEC_DBG
#define CODEC_DBG   AmbaPrint
#else
#define CODEC_DBG(...)
#endif


typedef struct {
	UINT16	nAddr;	// Register Address at nAddr < 0x100
	UINT16	nMask;	// wait time(ms) at nAddr = AK4678_M_DELAY
	UINT16	nData;	// 
} AKSEQUENCE;

typedef struct {
	AKSEQUENCE    *akseq;
	UINT16        size;
} AKSEQ_SIZE;

//***********************************************//
//*   Register default values - need to update  *//
//***********************************************//

UINT16 AK4951_reg_cache[AK4951_REG_NUM] = {      
#if 1   // Only Play
		0xC4, //AK4951_00_POWER_MANAGEMENT1		0x00
		0x32, //AK4951_01_POWER_MANAGEMENT2		0x01
		0xA0, //AK4951_02_SIGNAL_SELECT1		0x02   //change
		0x40, //AK4951_03_SIGNAL_SELECT2		0x03
		0x44, //AK4951_04_SIGNAL_SELECT3		0x04
		0x53, //AK4951_05_MODE_CONTROL_1		0x05	// AS AK4678'S PLL_MODE_0
		0x0B, // AK4951_06_MODE_CONTROL_2		0x06
		0x14, // AK4951_07_MODE_CONTROL_3		0x07
		0x00, // AK4951_08_DIGITAL_MIC			0x08
		0x00, // AK4951_09_TIMER_SELECT			0x09
		0x6c, // AK4951_0A_ALC_TIMER_SELECT		0x0A
		0x2E, // AK4951_0B_ALC_MODE_CONTROL_1	0x0B
		0xA1, // AK4951_0C_ALC_MODE_CONTROL_2	0x0C
		0x91, // AK4951_0D_L_INPUT_VOLUME_CONTROL	0x0D    //change
		0xE1, // AK4951_0E_R_INPUT_VOLUME_CONTROL	0x0E	//change
		0x00, // AK4951_0F_ALC_VOLUME				0x0F
		0x80, // AK4951_10_L_MIC_GAIN				0x10			
		0x80, // AK4951_11_R_MIC_GAIN				0x11	
		0x00, // AK4951_12_BEEP_VOL_CONTROL		0x12		
		0x10, // AK4951_13_L_DIGITAL_VOLUME		0x13	//change
		0x10, // AK4951_14_R_DIGITAL_VOLUME		0x14	//change
		0x00, // AK4951_15_EQ_COM_GAIN_SELECT		0x15
		0x00, // AK4951_16_EQ2_COM_GAIN_SETTING	0x16
		0x00, // AK4951_17_EQ3_COM_GAIN_SETTING	0x17
		0x00, // AK4951_18_EQ4_COM_GAIN_SETTING	0x18
		0x00, // AK4951_19_EQ5_COM_GAIN_SETTING	0x19
		0x0C, // AK4951_1A_HPF_FILTER_CONTROL		0x1A
		0x01, // AK4951_1B_D_FILTER_SELECT1			0x1B
		0x00, // AK4951_1C_D_FILTER_SELECT2			0x1C
		0x03, // AK4951_1D_D_FILTER_MODE				0x1D
		0xB0, // AK4951_1E_HPF2_COEFFICIENT0		0x1E
		0x1F, // AK4951_1F_HPF2_COEFFICIENT1		0x1F
		0x9F, // AK4951_20_HPF2_COEFFICIENT2		0x20
		0x20, // AK4951_21_HPF2_COEFFICIENT3		0x21
		0x00, // AK4951_22_LPF_COEFFICIENT0		0x22
		0x00, // AK4951_23_LPF_COEFFICIENT1		0x23
		0x00, // AK4951_24_LPF_COEFFICIENT2		0x24
		0x00, // AK4951_25_LPF_COEFFICIENT3		0x25
		0x00, // AK4951_26_FIL3_COEFFICIENT0		0x26
		0x00, // AK4951_27_FIL3_COEFFICIENT1		0x27
		0x00, // AK4951_28_FIL3_COEFFICIENT2		0x28
		0x00, // AK4951_29_FIL3_COEFFICIENT3		0x29
		0x00, // AK4951_2A_EQ0_COEFFICIENT0		0x2A
		0x00, // AK4951_2B_EQ0_COEFFICIENT1		0x2B
		0x00, // AK4951_2C_EQ0_COEFFICIENT2		0x2C
		0x00, // AK4951_2D_EQ0_COEFFICIENT3		0x2D
		0x00, // AK4951_2E_EQ0_COEFFICIENT4		0x2E
		0x00, // AK4951_2F_EQ0_COEFFICIENT5		0x2F
		0x00, // AK4951_30_D_FILTER_SELECT3		0x30
		0x00, // AK4951_31_DEVICE_INFORMATION	0x31	//change
		0x00, // AK4951_32_E1_COEFFICIENT0		0x32  //change
		0x00, // AK4951_33_E1_COEFFICIENT1		0x33  //change
		0x00, // AK4951_34_E1_COEFFICIENT2		0x34  //change
		0x00, // AK4951_35_E1_COEFFICIENT3		0x35  //change
		0x00, // AK4951_36_E1_COEFFICIENT4		0x36  //change
		0x00, // AK4951_37_E1_COEFFICIENT5		0x37  //change
		0x00, // AK4951_38_E2_COEFFICIENT0		0x38  //change
		0x00, // AK4951_39_E2_COEFFICIENT1		0x39  //change
		0x00, // AK4951_3A_E2_COEFFICIENT2		0x3A  //change
		0x00, // AK4951_3B_E2_COEFFICIENT3		0x3B  //change
		0x00, // AK4951_3C_E2_COEFFICIENT4		0x3C  //change
		0x00, // AK4951_3D_E2_COEFFICIENT5		0x3D  //change
		0x00, // AK4951_3E_E3_COEFFICIENT0		0x3E  //change
		0x00, // AK4951_3F_E3_COEFFICIENT1		0x3F  //change
		0x00, // AK4951_40_E3_COEFFICIENT2		0x40  //change
		0x00, // AK4951_41_E3_COEFFICIENT3		0x41  //change
		0x00, // AK4951_42_E3_COEFFICIENT4		0x42  //change
		0x00, // AK4951_43_E3_COEFFICIENT5		0x43  //change
		0x00, // AK4951_44_E4_COEFFICIENT0		0x44  //change
		0x00, // AK4951_45_E4_COEFFICIENT1		0x45  //change
		0x00, // AK4951_46_E4_COEFFICIENT2		0x46  //change
		0x00, // AK4951_47_E4_COEFFICIENT3		0x47  //change
		0x00, // AK4951_48_E4_COEFFICIENT4		0x48  //change
		0x00, // AK4951_49_E4_COEFFICIENT5		0x49  //change
		0x00, // AK4951_4A_E5_COEFFICIENT0		0x4A  //change
		0x00, // AK4951_4B_E5_COEFFICIENT1		0x4B  //change
		0x00, // AK4951_4C_E5_COEFFICIENT2		0x4C  //change
		0x00, // AK4951_4D_E5_COEFFICIENT3		0x4D  //change
		0x00, // AK4951_4E_E5_COEFFICIENT4		0x4E  //change
		0x00, // AK4951_4F_E5_COEFFICIENT5		0x4F  //change
#endif
};

static AKSEQUENCE akseq_softmute_on[] = {
	{AK4951_07_MODE_CONTROL_3, 0x20,0x20}			/* [07H:20H] - soft mute = 1 */
};

static AKSEQUENCE akseq_softmute_off[] = {
	{AK4951_07_MODE_CONTROL_3, 0x20,0x00}			/* [07H:20H] - soft mute = 0 */
};

static AKSEQUENCE akseq_micin_rec_on[] = {
	{AK4951_00_POWER_MANAGEMENT1, 0x00,0xC3},		/* [00H:C3H] - Filter/Vcom power up, ADC-L/Rch power up */
	{AK4951_01_POWER_MANAGEMENT2, 0x00, 0x00},		/* [01H:D0H] - PLL slave mode, power up Microphone */
	{AK4951_02_SIGNAL_SELECT1, 0x00, 0x0E},
	{AK4951_03_SIGNAL_SELECT2, 0x00, 0x00},
	{AK4951_0C_ALC_MODE_CONTROL_2, 0x00, 0xE1},
	{AK4951_0D_L_INPUT_VOLUME_CONTROL, 0x00, 0xE1},
	{AK4951_13_L_DIGITAL_VOLUME, 0x00, 0x10},
	{AK4951_14_R_DIGITAL_VOLUME, 0x00, 0x10},
	{AK4951_1A_HPF_FILTER_CONTROL, 0x00, 0x2C},
	{AK4951_1D_D_FILTER_MODE, 0x00,0x03},			/* [1DH:00H] - using Mode2-playback & recording */
};

static AKSEQUENCE akseq_micin_rec_off[] = {
	{AK4951_00_POWER_MANAGEMENT1, 0x00, 0x00},		/* [00H:40H] - Vcom disable, ADC-L/Rch disable */
};

static AKSEQUENCE akseq_spkout_on[] = {
	{AK4951_00_POWER_MANAGEMENT1, 0x00,0xC4},		/* [00H:C3H] - Filter/Vcom power up, ADC-L/Rch power up */
	{AK4951_01_POWER_MANAGEMENT2, 0x00, 0x32},		/* [01H:D0H] - PLL slave mode, power up Microphone */
	{AK4951_02_SIGNAL_SELECT1, 0x00, 0xA0},
	{AK4951_03_SIGNAL_SELECT2, 0x00, 0x40},
	{AK4951_0C_ALC_MODE_CONTROL_2, 0x00, 0xA1},
	{AK4951_0D_L_INPUT_VOLUME_CONTROL, 0x00, 0x91},
	{AK4951_13_L_DIGITAL_VOLUME, 0x00, 0x00},
	{AK4951_14_R_DIGITAL_VOLUME, 0x00, 0x00},
	{AK4951_1A_HPF_FILTER_CONTROL, 0x00, 0x0C},
	{AK4951_1D_D_FILTER_MODE, 0x00,0x03},			/* [1DH:00H] - using Mode2-playback & recording */
};

static AKSEQUENCE akseq_spkout_off[] = {
	{AK4951_00_POWER_MANAGEMENT1, 0xFF,0x00}		/* [00H:40H] - VCom and regular(2.3V) power-up */	
};

static AKSEQUENCE akseq_lineamp_on[] = {
	{AK4951_1D_D_FILTER_MODE, 0x07,0x00},			/* [1DH:00H] - SDTO output[ADC], ALC input[SDTI]  */	
	{AK4951_04_SIGNAL_SELECT3, 0,0x31},				/* [04H:31H] - Stereo output power in save mode */	
	{AK4951_00_POWER_MANAGEMENT1, 0,0x0C},			/* [00H:0CH] - DAC power-up, Sterero line output power-up */	
	{AK4951_04_SIGNAL_SELECT3, 0,0x11}				/* [04H:11H] - Stereo output power in normal mode */
};

static AKSEQUENCE akseq_lineamp_off[] = {
	{AK4951_00_POWER_MANAGEMENT1, 0x04,0x00},		/* [00H:00H] -  DAC power-down, Sterero line output power-down */	
};

static AKSEQUENCE akseq_extslave_on_pre[] = {
	{AK4951_05_MODE_CONTROL_1, 0, 0x02}				/* [05H:02H] - PLL disable */
};

static AKSEQUENCE akseq_extslave_on[] = {
	{AK4951_00_POWER_MANAGEMENT1, 0,0x40},			/* [00H:40H] - VCOM Power up & Regulator 2.3V */	
	{AK4951_M_DELAY, 10, 0}							/* delay 1ms - waiting power up time */
};


/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AK4951_Read
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
int AK4951_Read(UINT16 Addr)
{
	UINT8 RxDataBuf = 0;

	#if 1
	UINT16 WorkUINT16[3];

	WorkUINT16[0] = AMBA_I2C_RESTART_FLAG | AK4951_I2C_ADDR; /* Slave Address + r/w (0) */
	WorkUINT16[1] = Addr;											/* Sub Address */
	WorkUINT16[2] = AMBA_I2C_RESTART_FLAG | AK4951_I2C_ADDR | 0x01;
	#else
	UINT8 WorkUINT16[3];

	WorkUINT16[0] = AK4951_I2C_ADDR; /* Slave Address + r/w (0) */
	WorkUINT16[1] = Addr;											/* Sub Address */
	WorkUINT16[2] = AK4951_I2C_ADDR | 0x01;

	#endif

	if (Addr < AK4951_REG_NUM) {
		#if 1
		if (AmbaI2C_ReadAfterWrite(AMBA_I2C_CHANNEL2, AMBA_I2C_SPEED_FAST,
								   3, WorkUINT16, 1, &RxDataBuf, 5000) != OK) {
		#else
		if( AmbaI2C_Read(AMBA_I2C_CHANNEL2, 
			             AMBA_I2C_SPEED_STANDARD, 
			             (AK4951_I2C_ADDR | 0x01), 
			             1, 
			             &RxDataBuf, 
			             1000)!= OK){
		#endif
			return NG;
		}
		CODEC_DBG("%s: Addr: %d, Data: %d", __func__, Addr, RxDataBuf);
		return RxDataBuf;
	} else {
		CODEC_DBG("%s: Addr: %d, Data: %d fail!", __func__, Addr, RxDataBuf);
		return NG;
	}
	
}


/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AK4951_Write
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
int AK4951_Write(UINT16 Addr, UINT8 Data)
{
	UINT8 TxDataBuf[2];
	int rval = 0;

	TxDataBuf[0] = (UINT8)Addr;
	TxDataBuf[1] = (UINT8)(Data);

	CODEC_DBG("[%s]: Addr: 0x%02x, Data: 0x%02x", __func__, Addr, Data);

	if (Addr < AK4951_REG_NUM) {
		rval = AmbaI2C_Write(AMBA_I2C_CHANNEL2, AMBA_I2C_SPEED_FAST,
							 AK4951_I2C_ADDR, 2, TxDataBuf,
							 AMBA_KAL_WAIT_FOREVER);
		//Data = AK4951_Read(Addr);
		//CODEC_DBG(">>> Read back, Addr = 0x%02x 0x%04x", Addr, Data);

		return rval;
	} else {
		CODEC_DBG("[AK4951_Write]: Wrong address number! %d", Addr);
		return NG;
	}
}


static int AK4951_SequeceExec( AKSEQUENCE *akseq,UINT32 nSqcount)
{
	UINT32	i; 
	UINT16 data;
	
	CODEC_DBG("[AK4951] %s(%d): %d   adderss= 0x%x ",__FUNCTION__,__LINE__, nSqcount,akseq[0].nAddr);

	for ( i = 0 ; i < nSqcount; i++ ) {
		switch(akseq[i].nAddr) {
			case AK4951_M_DELAY:
				AmbaKAL_TaskSleep(akseq[i].nMask);
				break;

			default:
			    data = AK4951_Read(akseq[i].nAddr);
				data &= ((UINT16) ~akseq[i].nMask);				/* if Mask=0x00, means there is no mask */
				data |= akseq[i].nData ;
				AK4951_Write(akseq[i].nAddr, data );
				break;
		}
	}

	return(0);
}


static int AK4951_SetMute(UINT32 MuteEnable)
{
    if (MuteEnable) {
		AK4951_SequeceExec( akseq_softmute_on, ARRAY_SIZE(akseq_softmute_on));
    } else {
		AK4951_SequeceExec( akseq_softmute_off, ARRAY_SIZE(akseq_softmute_off));
    }
    return OK;
}

/**
 * Change operating frequency
 */
static int AK4951_FreqConfig(UINT32 Freq)
{
	UINT16	data;
	
	/* Codec's sampling feq depends on MCLK input, fs=MCLK/256	 */
	/* MCLK should be in 1.88MHz~12.288MHz, then fs would get 7.35KHz~48KHz, */
	AK4951_SequeceExec(akseq_extslave_on_pre, ARRAY_SIZE(akseq_extslave_on_pre));	
	
	/* set value */
	data = AK4951_Read(AK4951_06_MODE_CONTROL_2);
	data = ( data & (UINT16) ~AK4951_FS_MASK) | AK4951_FS_256FS;
	AK4951_Write(AK4951_06_MODE_CONTROL_2, data);
	
	/* set value done */
	AK4951_SequeceExec(akseq_extslave_on, ARRAY_SIZE(akseq_extslave_on));	


    return OK;
}

/**
 * Mode configuration
 */
static int AK4951_ModeConfig(AMBA_AUDIO_CODEC_MODE_e Mode)
{
	UINT16 data = AK4951_Read(AK4951_05_MODE_CONTROL_1);
	data &= ((UINT16) ~AK4951_AUDIOIF_MASK);						/* clear audio IF mode */

	switch(Mode){
		case LeftJustified:				// Mode2: (I/O) MSB-Justified
			data |= (UINT16) AK4951_AUDIOIF_MODE2;
			break;
		case RightJustified:				
			data |= (UINT16) AK4951_AUDIOIF_MODE1;
			break;
		case MSBExtend:
			/* do nothing */
			break;
		case I2S:							// Mode 3 : I2S Compatible
			data |= (UINT16) AK4951_AUDIOIF_MODE3;			
			break;
		case DSP:							// not support 
			/* do nothing */
			break;
	}

	// must same setting trans & rcv
	AK4951_Write(AK4951_05_MODE_CONTROL_1, data);

    return OK;
}


static void AK4951_LineoutPowerOn(void)
{
	AK4951_SequeceExec(akseq_lineamp_on, ARRAY_SIZE(akseq_lineamp_on));	
}

static void AK4951_LineoutPowerDown(void)
{
	AK4951_SequeceExec(akseq_lineamp_off, ARRAY_SIZE(akseq_lineamp_off));	
}

static void AK4951_SpoutPowerOn(void)
{
	AK4951_SequeceExec(akseq_spkout_on, ARRAY_SIZE(akseq_spkout_on));	
}

static void AK4951_SpoutPowerDown(void)
{
	AK4951_SequeceExec(akseq_spkout_off, ARRAY_SIZE(akseq_spkout_off));	
}

static void AK4951_MicinPowerOn(void)
{
	AK4951_SequeceExec(akseq_micin_rec_on, ARRAY_SIZE(akseq_micin_rec_on));	
}

static void AK4951_MicinPowerDown(void)
{
	AK4951_SequeceExec(akseq_micin_rec_off, ARRAY_SIZE(akseq_micin_rec_off));	
}

static void AK4951_DummyCommand(void)
{
	AK4951_Write(AK4951_00_POWER_MANAGEMENT1, 0x00); 	/* Addr:00H, Data: 00H - dummy command */
}


//***************************************//
//*       Initial Audio CODEC           *//
//***************************************//
static int AK4951_Init(void)
{
	int i; 

	/* Dummy Command - AK4951 special command, need send after PDN pin rise to High */
	AK4951_DummyCommand();
  
	/* Write default value to the instance  */	
	/* This can be bypass since all registers in AK4951 will return to default value after reset*/

	for (i = 0; i < AK4951_REG_NUM; i++) {
		AK4951_Write(i, AK4951_reg_cache[i]);
	}	

	//for (i = 0; i < AK4951_REG_NUM; i++) {
	//	AK4951_write(reg_codec, i, AK4951_reg_cache[i]);
	//}	
	
	return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AK4951_SetOutput
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
static int AK4951_SetOutput(AMBA_AUDIO_CODEC_OUTPUT_MODE_e OutputMode)
{
    if (OutputMode & AMBA_AUDIO_CODEC_SPEAKER_OUT) {
        AK4951_SpoutPowerOn();
    } else {
        AK4951_SpoutPowerDown();
    }
    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AK4951_SetInput
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
static int AK4951_SetInput(AMBA_AUDIO_CODEC_INPUT_MODE_e InputMode)
{
    if (InputMode == AMBA_AUDIO_CODEC_MIC_IN) {
        AK4951_MicinPowerOn();
    } else {
        AK4951_MicinPowerDown();
    }
    return OK;
}


AMBA_AUDIO_CODEC_OBJ_s AmbaAudio_AK4951Obj = {
    .Init       = AK4951_Init,
    .ModeConfig = AK4951_ModeConfig,
    .FreqConfig = AK4951_FreqConfig,
    .SetOutput  = AK4951_SetOutput,
    .SetInput   = AK4951_SetInput,
    .SetMute    = AK4951_SetMute,
    .Write      = AK4951_Write,
    .Read       = AK4951_Read,
};


