/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaAudio_AK4951EN.h
 *
 *  @Copyright      :: Copyright (C) 2015 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: AK4951EN OKI semiconductor audio AD/DA CODEC driver
\*-------------------------------------------------------------------------------------------------------------------*/

#ifndef _AMBA_AUDIO_AK4951EN_H_
#define _AMBA_AUDIO_AK4951EN_H_

/** Return array size of structure */
/* #define ARRAY_SIZE(arr) (sizeof(arr) / sizeof(((typeof(arr)){})[0])) */
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]) /*+ __must_be_array(arr)*/)  

#define AK4951_00_POWER_MANAGEMENT1		0x00
#define AK4951_01_POWER_MANAGEMENT2		0x01
#define AK4951_02_SIGNAL_SELECT1		0x02   //change
#define AK4951_03_SIGNAL_SELECT2		0x03
#define AK4951_04_SIGNAL_SELECT3		0x04
#define AK4951_05_MODE_CONTROL_1		0x05	// AS AK4678'S PLL_MODE_0
#define AK4951_06_MODE_CONTROL_2		0x06
#define AK4951_07_MODE_CONTROL_3		0x07
#define AK4951_08_DIGITAL_MIC			0x08
#define AK4951_09_TIMER_SELECT			0x09
#define AK4951_0A_ALC_TIMER_SELECT		0x0A
#define AK4951_0B_ALC_MODE_CONTROL_1	0x0B
#define AK4951_0C_ALC_MODE_CONTROL_2	0x0C
#define AK4951_0D_L_INPUT_VOLUME_CONTROL	0x0D    //change
#define AK4951_0E_R_INPUT_VOLUME_CONTROL	0x0E	//change
#define AK4951_0F_ALC_VOLUME				0x0F
#define AK4951_10_L_MIC_GAIN				0x10			
#define AK4951_11_R_MIC_GAIN				0x11	
#define AK4951_12_BEEP_VOL_CONTROL		0x12		
#define AK4951_13_L_DIGITAL_VOLUME		0x13	//change
#define AK4951_14_R_DIGITAL_VOLUME		0x14	//change
#define AK4951_15_EQ_COM_GAIN_SELECT		0x15
#define AK4951_16_EQ2_COM_GAIN_SETTING	0x16
#define AK4951_17_EQ3_COM_GAIN_SETTING	0x17
#define AK4951_18_EQ4_COM_GAIN_SETTING	0x18
#define AK4951_19_EQ5_COM_GAIN_SETTING	0x19
#define AK4951_1A_HPF_FILTER_CONTROL		0x1A
#define AK4951_1B_D_FILTER_SELECT1			0x1B
#define AK4951_1C_D_FILTER_SELECT2			0x1C
#define AK4951_1D_D_FILTER_MODE				0x1D
#define AK4951_1E_HPF2_COEFFICIENT0		0x1E
#define AK4951_1F_HPF2_COEFFICIENT1		0x1F
#define AK4951_20_HPF2_COEFFICIENT2		0x20
#define AK4951_21_HPF2_COEFFICIENT3		0x21
#define AK4951_22_LPF_COEFFICIENT0		0x22
#define AK4951_23_LPF_COEFFICIENT1		0x23
#define AK4951_24_LPF_COEFFICIENT2		0x24
#define AK4951_25_LPF_COEFFICIENT3		0x25
#define AK4951_26_FIL3_COEFFICIENT0		0x26
#define AK4951_27_FIL3_COEFFICIENT1		0x27
#define AK4951_28_FIL3_COEFFICIENT2		0x28
#define AK4951_29_FIL3_COEFFICIENT3		0x29
#define AK4951_2A_EQ0_COEFFICIENT0		0x2A
#define AK4951_2B_EQ0_COEFFICIENT1		0x2B
#define AK4951_2C_EQ0_COEFFICIENT2		0x2C
#define AK4951_2D_EQ0_COEFFICIENT3		0x2D
#define AK4951_2E_EQ0_COEFFICIENT4		0x2E
#define AK4951_2F_EQ0_COEFFICIENT5		0x2F
#define AK4951_30_D_FILTER_SELECT3		0x30
#define AK4951_31_DEVICE_INFORMATION	0x31	//change
#define AK4951_32_E1_COEFFICIENT0		0x32  //change
#define AK4951_33_E1_COEFFICIENT1		0x33  //change
#define AK4951_34_E1_COEFFICIENT2		0x34  //change
#define AK4951_35_E1_COEFFICIENT3		0x35  //change
#define AK4951_36_E1_COEFFICIENT4		0x36  //change
#define AK4951_37_E1_COEFFICIENT5		0x37  //change
#define AK4951_38_E2_COEFFICIENT0		0x38  //change
#define AK4951_39_E2_COEFFICIENT1		0x39  //change
#define AK4951_3A_E2_COEFFICIENT2		0x3A  //change
#define AK4951_3B_E2_COEFFICIENT3		0x3B  //change
#define AK4951_3C_E2_COEFFICIENT4		0x3C  //change
#define AK4951_3D_E2_COEFFICIENT5		0x3D  //change
#define AK4951_3E_E3_COEFFICIENT0		0x3E  //change
#define AK4951_3F_E3_COEFFICIENT1		0x3F  //change
#define AK4951_40_E3_COEFFICIENT2		0x40  //change
#define AK4951_41_E3_COEFFICIENT3		0x41  //change
#define AK4951_42_E3_COEFFICIENT4		0x42  //change
#define AK4951_43_E3_COEFFICIENT5		0x43  //change
#define AK4951_44_E4_COEFFICIENT0		0x44  //change
#define AK4951_45_E4_COEFFICIENT1		0x45  //change
#define AK4951_46_E4_COEFFICIENT2		0x46  //change
#define AK4951_47_E4_COEFFICIENT3		0x47  //change
#define AK4951_48_E4_COEFFICIENT4		0x48  //change
#define AK4951_49_E4_COEFFICIENT5		0x49  //change
#define AK4951_4A_E5_COEFFICIENT0		0x4A  //change
#define AK4951_4B_E5_COEFFICIENT1		0x4B  //change
#define AK4951_4C_E5_COEFFICIENT2		0x4C  //change
#define AK4951_4D_E5_COEFFICIENT3		0x4D  //change
#define AK4951_4E_E5_COEFFICIENT4		0x4E  //change
#define AK4951_4F_E5_COEFFICIENT5		0x4F  //change

#define AK4951_MAX_REGISTERS	        (AK4951_4F_E5_COEFFICIENT5)

#define AK4951_I2C_ADDR            		0x24	
#define AK4951_REG_NUM                  (AK4951_MAX_REGISTERS + 1)


/* Bitfield Definitions */

/* AK4951_POWER_MANAGEMENT 1 (0x00) */
#define AK4951_PMPFIL      	0x80		/* [0] Programmable Filter Block(HPF2/LPF5 Band EQ/ALC) Power Management / 0=PowerDown, 1=PowerUp*/
#define AK4951_PMVCM       	0x40		/* [0] VCOM and Regulator(2.3V) Power Management / 0=PowerDown, 1=PowerUp */
#define AK4951_PMBP        	0x20		/* [0] MIN-Amp Power Management / 0=PowerDown, 1=PowerUp */
//#define AK4951_PMSPK       	0x10		/* [0] Speaker-Amp Power Management / 0=PowerDown, 1=PowerUp */
//#define AK4951_PMLO        	0x08		/* [0] Stereo Line Output Power Management / 0=PowerDown, 1=PowerUp */
#define AK4951_PMDAC       	0x04		/* [0] DAC Power Management / 0=PowerDown, 1=PowerUp */
#define AK4951_PMADR       	0x02		/* [0] ADC Rch Power Management / 0=PowerDown, 1=PowerUp */
#define AK4951_PMADL       	0x01		/* [0] ADC Lch Power Management / 0=PowerDown, 1=PowerUp */

/* AK4951_POWER_MANAGEMENT 2 (0x01) */
#define AK4951_PMOSC      		0x80		/* [0] Internal Oscillator Power Management / 0=PowerDown, 1=PowerUp */
#define AK4951_PMHPR	     	0x20		/* [0] R Headphone Amplifier and charge Pump Power Management / 0=PowerDown, 1=PowerUp */
#define AK4951_PMHPL        	0x10		/* [0] L Headphone Amplifier and charge Pump Power Management / 0=PowerDown, 1=PowerUp */
#define AK4951_MS          		0x08		/* [0] Master / Slave Mode Select / 0=Slave, 1=Master */
#define AK4951_PMPLL          	0x04		/* [0] PLL Power Management / 0=PowerDown, 1=PowerUp */
#define AK4951_PMSL	    	0x02		/* [0] Speaker Amplifier or stereo Line Output power Management/ 0=power down, 1=Power up */
#define AK4951_LOSEL       	0x01		/* [0] Stereo Line Output Select / 0=Speaker Output(SPP/SPN Pins), 1=Stereo Line Output (Lout/Rout Pins) */

/* AK4951_POWER_MANAGEMENT 3 (0x02) */
//#define AK4951_SPPSN       	0x80		/* [0] Speaker-Amp Power-Save Mode / 0=Power Save Mode, 1=Normal Operation */
//#define AK4951_MICL        	0x10		/* [0] MIC Power Output Voltage Select / 0=2.4V, 1=2.0V */

#define AK4951_MGAIN_MASK	(0x47)		
#define AK4951_MGAIN_0DB    (0 << 0)	/* [0] Microphone Amplifier Gain Control (Table 24) Default: ¡°0110¡± (+18dB)*/	
#define AK4951_MGAIN_3DB    (1 << 0)
#define AK4951_MGAIN_6DB    (2 << 0)
#define AK4951_MGAIN_9DB    (3 << 0)
#define AK4951_MGAIN_12DB   (4 << 0)
#define AK4951_MGAIN_15DB   (5 << 0)
#define AK4951_MGAIN_18DB   (6 << 0)
#define AK4951_MGAIN_21DB   (7 << 0)
#define AK4951_MGAIN_24DB   (1 << 6)
#define AK4951_MGAIN_27DB   ((1 << 0)|(1<<6))
#define AK4951_MGAIN_MAX    ((2 << 0)|(1<<6))

/* AK4951_SIGNAL_SELECT 1 (0x02) */
/*
SLPSN: Speaker Amplifier or Stereo Line Output Power-Save Mode
LOSEL bit = ¡°0¡± (Speaker Output Select)
0: Power Save Mode (default)
1: Normal Operation
When SLPSN bit is ¡°0¡±, Speaker Amplifier is in power-save mode. In this mode, the SPP pin goes to
Hi-Z and the SPN pin outputs SVDD/2 voltage. When PMSL bit = ¡°1¡±, SLPSN bit is enabled. After
the PDN pin is set to ¡°L¡±, Speaker Amplifier is in power-down mode since PMSL bit is ¡°0¡±.
LOSEL bit = ¡°1¡± (Stereo Line Output Select)
0: Power Save Mode (default)
1: Normal Operation
When SLPSN bit is ¡°0¡±, Stereo line output is in power-save mode. In this mode, the LOUT/ROUT
pins output 1.5V or 1.3V. When PMSL bit = ¡°1¡±, SLPSN bit is enabled. After the PDN pin is set to
¡°L¡±, Stereo line output is in power-down mode since PMSL bit is ¡°0¡±.SLPSN: Speaker Amplifier or Stereo Line Output Power-Save Mode
LOSEL bit = ¡°0¡± (Speaker Output Select)
0: Power Save Mode (default)
1: Normal Operation
When SLPSN bit is ¡°0¡±, Speaker Amplifier is in power-save mode. In this mode, the SPP pin goes to
Hi-Z and the SPN pin outputs SVDD/2 voltage. When PMSL bit = ¡°1¡±, SLPSN bit is enabled. After
the PDN pin is set to ¡°L¡±, Speaker Amplifier is in power-down mode since PMSL bit is ¡°0¡±.
LOSEL bit = ¡°1¡± (Stereo Line Output Select)
0: Power Save Mode (default)
1: Normal Operation
When SLPSN bit is ¡°0¡±, Stereo line output is in power-save mode. In this mode, the LOUT/ROUT
pins output 1.5V or 1.3V. When PMSL bit = ¡°1¡±, SLPSN bit is enabled. After the PDN pin is set to
¡°L¡±, Stereo line output is in power-down mode since PMSL bit is ¡°0¡±.*/
#define AK4951_SLPSN   		0x80		/* [0] Speaker Amplifier Or Stereo Line Output Power_Save Mode */
#define AK4951_DACS			0x20		/* [0] Signal Switch Control from DAC to Speaker Amplifier 0: OFF (default) 1: ON*/
#define AK4951_MPSEL    		0x10		/* [0] MPWR Output Select 0: MPWR1 pin (default)  1: MPWR2 pin */
#define AK4951_PMMP     		0x08		/* [0] MPWR pin Power Management 0: Power down: Hi-Z (default) 1: Power up*/

/* AK4951_SIGNAL_SELECT 2 (0x03) */
#define AK4951_SPKG1   		0x80		/* [0] Speaker-Amp Output Gain Select (Table.53) */
#define AK4951_SPKG0   		0x40		/* [0] Speaker-Amp Output Gain Select */
#define AK4951_MICL	   		0x10		/* [0] MPWR pin Output Voltage Select 0: typ 2.4V (default) 1: typ 2.0V */
#define AK4951_INL1    		0x08		/* [0] ADC Lch Input Source Select (Table 23) Default: ¡°00¡± (LIN1 pin) */
#define AK4951_INL0     		0x04		/* [0] ADC Lch Input Source Select (Table 23) Default: ¡°00¡± (LIN1 pin) */
#define AK4951_INR1     		0x02		/* [0] ADC Rch Input Source Select (Table 23) Default: ¡°00¡± (RIN1 pin)*/
#define AK4951_INR0     		0x01		/* [0] ADC Rch Input Source Select (Table 23) Default: ¡°00¡± (RIN1 pin)*/

/* AK4951_SIGNAL_SELECT 3 (0x04) */
#define AK4951_LVCM1   		0x80		/* [0] Stereo Line Output Gain and Common Voltage Setting (Table 60) Default: ¡°01¡± (+2dB, 1.5V) */
#define AK4951_LVCM0   		0x40		/* [0] Stereo Line Output Gain and Common Voltage Setting (Table 60) Default: ¡°01¡± (+2dB, 1.5V) */
#define AK4951_DACL    		0x20		/* [0] Signal Switch Control from DAC to Stereo Line Amplifier 0: OFF (default) 1: ON*/
#define AK4951_PTS1    		0x08		/* [0] Soft Transition Control of ¡°BEEP ¡ú Headphone¡± Connection ON/OFF (Table 53) Default: ¡°01¡± */
#define AK4951_PTS0	   		0x04		/* [0] Soft Transition Control of ¡°BEEP ¡ú Headphone¡± Connection ON/OFF (Table 53) Default: ¡°01¡± */
#define AK4951_MONO1   		0x02		/* [0] Mono/Stereo Setting for DAC Input (Table 50) Default: ¡°00¡± (Stereo)*/
#define AK4951_MONO0   		0x01		/* [0] Mono/Stereo Setting for DAC Input (Table 50) Default: ¡°00¡± (Stereo)*/

/* AK4951_MODE_CONTROL 1 (0x05) */
#define AK4951_PLL_3		0x80	/*PLL Reference Clock Select (Table 6) Default: ¡°0101¡± (MCKI, 12.288MHz)*/
#define AK4951_PLL_2		0x40
#define AK4951_PLL_1		0x20
#define AK4951_PLL_0		0x10
#define AK4951_BCKO		0x08   /*BICK Output Frequency Setting in Master Mode (Table 11, Table 18) 0: 32fs (default) 1: 64fs*/
#define AK4951_CKOFF		0x04   /*LRCK, BICK and SDTO Output Setting in Master Mode 0: LRCK, BICK and SDTO Output (default) 1: LRCK, BICK and SDTO Stop ( ¡°L¡± output)*/
#define AK4951_DIF1		0x02 	/*Audio Interface Format (Table 20) Default: ¡°10¡± (MSB justified)*/
#define AK4951_DIF0    	0x01	/*Audio Interface Format (Table 20) Default: ¡°10¡± (MSB justified)*/

#define AK4957_BCKO      0x08					/* [0] Master Mode BICK Output Frequency Setting / 0=32fs(BICK), 1=64fs(BICK)(Table.15) */

#define AK4951_AUDIOIF_MASK			(0x03)		/* Audio Interface Format [DIF0/1] (Table.17) */
#define AK4951_AUDIOIF_MODE0		(0 << 0)	/* Audio Interface Format [DIF0/1] (Table.17) */
#define AK4951_AUDIOIF_MODE1		(1 << 0)	
#define AK4951_AUDIOIF_MODE2		(2 << 0)	/* [default] */
#define AK4951_AUDIOIF_MODE3		(3 << 0)

/* AK4951_MODE_CONTROL 2 (0x06) */
#define AK4951_CM1			0x80	/* MCKI Input Frequency Setting in EXT mode (Table 12, Table 15) Default: ¡°00¡± (256fs) */
#define AK4951_CM0        	0x40
#define AK4951_FS3        	0x08	/* Sampling frequency Setting (Table 7, Table 9, Table 13, Table 16) Default: ¡°1011¡± (fs=48kHz)*/
#define AK4951_FS2        	0x04	
#define AK4951_FS1          0x02
#define AK4951_FS0			0x01

#define AK4951_FS_MASK				(0x0F)
#define AK4951_FS_8KHZ  	   		(0 << 0)		/* PLL Mode - Sampling Frequency [Default] (Table.5, Table.6) */
#define AK4951_FS_12KHZ  	   		(1 << 0)
#define AK4951_FS_16KHZ  	   		(2 << 0)
#define AK4951_FS_11K025HZ     		(5 << 0)
#define AK4951_FS_22K05HZ  	   		(7 << 0)
#define AK4951_FS_24KHZ  	   		(9 << 0)
#define AK4951_FS_32KHZ  	   		(10 << 0)
#define AK4951_FS_48KHZ  	   		(11 << 0)
#define AK4951_FS_44K1HZ  	   		(15 << 0)

#define AK4951_FS_256FS			(0 << 0)		/*8kHz ¡Ü fs ¡Ü 48kHz default*/ 
#define AK4951_FS_384FS			(1 << 0)		/* 8kHz ¡Ü fs ¡Ü 48kHz */
#define AK4951_FS_512FS			(2 << 0)		/* 8kHz ¡Ü fs ¡Ü 48kHz*/
#define AK4951_FS_1024FS			(3 << 0)		/* 8kHz ¡Ü fs ¡Ü 24kHz*/


/* AK4951_MODE_CONTROL 3 (0x07) */
#define AK4951_TSDSEL     	0x80		/* [0] Thermal Shutdown Mode Select 0: Automatic Power up (default) 1: Manual Power up*/
#define AK4951_THDET     	0x40		/* [0] Thermal Shutdown Detection Result 0: Normal Operation (default) 1: During Thermal Shutdown*/
#define AK4951_SMUTE     	0x20		/* [0] Soft Mute Control / 0=Normal, 1=DAC output soft-muted */
#define AK4951_DVOLC     	0x10		/* [1] Output Digital Volume Control Mode Select 0: Independent 1: Dependent (default) When DVOLC bi */
#define AK4951_IVOLC     	0x04		/* [1] IVOL Control / 0=Independent, 1=Dependent */

/* AK4951_DIGITAL_MIC (0x08) */
#define AK4951_READ     	0x80		/* 3-wire Serial Read Function Enable (Only the AK4951EG supports) 0: Disable (default) 1: Enable */
#define AK4951_PMDMR     	0x20		/* [0] Input Signal Select with Digital Microphone / (Table.20) */
#define AK4951_PMDML     	0x10		/* [0] Input Signal Select with Digital Microphone */
#define AK4951_DCLKE     	0x08		/* [0] DMCLK output clock control / 0=L output, 1=64fs */
#define AK4951_DCLKP     	0x02		/* [0] Lch Data Latching DMCLK edge select / 0=Rising Edge 1=Falling Edge */
#define AK4951_DMIC      	0x01		/* [0] Digital Microphone connection / 0=Analog Microphone, 1=Digital Microphone */

/* AK4951_TIMER_SELECT (0x09) */
#define AK4951_ADRST_1059FS    		(0 << 6)	/* ADC Initial Cycle Settings [default] (Table.19)*/
#define AK4951_ADRST_267FS    		(1 << 6)
#define AK4951_ADRST_531FS    		(2 << 6)
#define AK4951_ADRST_135FS    		(3 << 6)

#define AK4951_DVTM_816FS      		(0 << 0)	/* Digital Volume Soft Transition Time Setting [default] (Table.52)*/
#define AK4951_DVTM_204FS      		(1 << 0)

#define AK4951_FRN    				0x10		/* [0] ALC First Recovery Function Enable / 0=Enable, 1=Disable */
#define AK4951_MOFF	  			0x02		/*Soft Transition Control of ¡°BEEP ¡ú Headphone¡± Connection ON/OFF 0: Enable (default) 1: Disable*/

/* AK4951_ALC_TIMER_SELECT (0x0A) */
#define AK4951_IVTM_236FS			(0 << 6)
#define AK1951_IVTM_944FS			(1 << 6)

#define AK4951_WTM_128FS			(0 << 2)	/* ALC Recovery Waiting Period [default] (Table.38) */
#define AK4951_WTM_256FS			(1 << 2)
#define AK4951_WTM_512FS			(2 << 2)
#define AK4951_WTM_1024FS			(3 << 2)

#define AK4951_RFST_0			(0 << 0)	/*  Fast Recovery Gain Step [dB] ALC First Recovey Speed (Table.41) */
#define AK4951_RFST_1			(1 << 0)
#define AK4951_RFST_2			(2 << 0)
#define AK4951_RFST_3			(3 << 0)

#define AK4951_EQFC_0			(0 << 4)
#define AK4951_EQFC_1			(1 << 4)
#define AK4951_EQFC_2			(2 << 4)
#define AK4951_EQFC_3			(3 << 4)


/* AK4951_ALC_MODE_CONTROL 1 (0x0B) */
#define AK4951_ALCEQN   	0x80		/* [0] ALC EQ Enable 0: ALC EQ On (default) 1: ALC EQ Off */
#define AK4951_LMTH2   	0x40		/* [0] ALC Limiter Detection Level / Recovery Counter Reset Level (Table 36) Default: ¡°000¡±*/
#define AK4951_ALC   		0x20		/* [0] ALC Enable 0: ALC Disable (default) 1: ALC Enable*/
#define AK4951_RGAIN2  	0x10		/* [0] ALC Recovery Gain Step (Table 39) Default: ¡°000¡± (0.00424dB)  */
#define AK4951_RGAIN1  	0x08		/* [0] ALC Recovery Gain Step (Table 39) Default: ¡°000¡± (0.00424dB)  */
#define AK4951_RGAIN0  	0x04		/* [0] ALC Recovery Gain Step (Table 39) Default: ¡°000¡± (0.00424dB)  */
#define AK4951_LMTH1  		0x02		/* [0] ALC Limiter Detection Level / Recovery Counter Reset Level (Table 36) Default: ¡°000¡±*/
#define AK4951_LMTH0  		0x01		/* [0] ALC Limiter Detection Level / Recovery Counter Reset Level (Table 36) Default: ¡°000¡±*/

/* AK4951_ALC_MODE_CONTROL 2 (0x0C) */
#define AK4951_REF7		0x80	/*[1] Reference Value at ALC Recovery Operation. 0.375dB step, 242 Level (Table 40) Default: ¡°E1H¡± (+30.0dB) */
#define AK4951_REF6		0x40	/*[1] Reference Value at ALC Recovery Operation. 0.375dB step, 242 Level (Table 40) Default: ¡°E1H¡± (+30.0dB) */
#define AK4951_REF5		0x20	/*[1] Reference Value at ALC Recovery Operation. 0.375dB step, 242 Level (Table 40) Default: ¡°E1H¡± (+30.0dB) */
#define AK4951_REF4		0x10	/*[0] Reference Value at ALC Recovery Operation. 0.375dB step, 242 Level (Table 40) Default: ¡°E1H¡± (+30.0dB) */
#define AK4951_REF3		0x08	/*[0] Reference Value at ALC Recovery Operation. 0.375dB step, 242 Level (Table 40) Default: ¡°E1H¡± (+30.0dB) */
#define AK4951_REF2		0x04	/*[0] Reference Value at ALC Recovery Operation. 0.375dB step, 242 Level (Table 40) Default: ¡°E1H¡± (+30.0dB) */
#define AK4951_REF1		0x02	/*[0] Reference Value at ALC Recovery Operation. 0.375dB step, 242 Level (Table 40) Default: ¡°E1H¡± (+30.0dB) */
#define AK4951_REF0		0x01	/*[1] Reference Value at ALC Recovery Operation. 0.375dB step, 242 Level (Table 40) Default: ¡°E1H¡± (+30.0dB) */

/* AK4951_L_Input_volume Control (0x0D) */
/* AK4951_R_Input_volume Control (0x0E) */


/* AK4951_LCH_MIC_GAIN (0x10) */
#define AK4951_LCH_MIC_GAIN	   0x80		/* [0] Lch MIC Sensitivity Correction 5-bits Mask - 0x0001 1111(0x1F) (Table.26) */

/* AK4951_RCH_MIC_GAIN (0x11) */
#define AK4951_RCH_MIC_GAIN	   0x80		/* [0] Rch MIC Sensitivity Correction 5-bits Mask - 0x0001 1111(0x1F) (Table.26) */

/* AK4951_BEEP_CONTROL (0x12) */
#define AK4951_HPZ      	0x40		/* [0] Pull-down Setting of Headphone Amplifier 0: Pull-down by a 10?(typ) resistor (default) 1: Hi-Z*/
#define AK4951_BPVCM     	0x10		/* [0] Common Voltage Setting of MIN Input Amp / 0=1.15V, 1=1.65V (Table.46)*/
#define AK4951_BEEPS      	0x40		/* [0]  Signal Switch Control from the BEEP pin to Speaker Amplifier 0: OFF (default) 1: ON*/
#define AK4951_BEEPH      0x10		/* [0]  Signal Switch Control from the BEEP pin to Headphone Amplifier 0: OFF (default) 1: ON*/
#define AK4951_BPLVL3     	0x08		/* [0] BEEP Output Level Setting (Table 55) Default: ¡°0000¡± (0dB) */
#define AK4951_BPLVL2     	0x04		/* [0] BEEP Output Level Setting (Table 55) Default: ¡°0000¡± (0dB) */
#define AK4951_BPLVL1     	0x02		/* [0] BEEP Output Level Setting (Table 55) Default: ¡°0000¡± (0dB) */
#define AK4951_BPLVL0     	0x01		/* [0] BEEP Output Level Setting (Table 55) Default: ¡°0000¡± (0dB) */

/* AK4951_Digital_Filter_Select1 (0x1B) */
#define AK4951_HPFC1 		0x04		/* [0] Cut-off Frequency Settings of HPF1 / (Table.25) */
#define AK4951_HPFC0 		0x02		/* [0] Cut-off Frequency Settings of HPF1 */
#define AK4951_HPFAD 		0x01		/* [1] HPF1 Control of ADC / 0=OFF, 1=ON */
#define AK4951_HPF_MAX 		0x07		/* [1] Max for HPF frequency */
#define AK4951_WC_LVL0		0x00		/* wind-cut level 0 --> HPFC1:HPFC0=00 */
#define AK4951_WC_LVL1		0x01		/* wind-cut level 1 --> HPFC1:HPFC0=01 */
#define AK4951_WC_LVL2		0x02		/* wind-cut level 2 --> HPFC1:HPFC0=10 */
#define AK4951_WC_LVL3		0x03		/* wind-cut level 3 --> HPFC1:HPFC0=11 */
#define AK4951_WC_LVL_MAX		0x03		/* wind-cut level MAX --> HPFC1:HPFC0=11 */
#define AK4951_WC_LEF_SFT		0x01		/* wind-cut level left shift bit */


/* AK4951_DIGITAL_FILTER_SELECT 2 (0x1C) */
#define AK4951_LPF      	0x02		/* [0] LPF Coefficient Setting Enable / 0=OFF, 1=ON */
#define AK4951_HPF      	0x01		/* [0] HPF2 Coefficient Setting Enable/ 0=OFF, 1=ON */
#define AK4951_GN1      	0x80		/* [0] Gain BLock Gain Setting  / (Table.27) */
#define AK4951_GN0      	0x40		/* [0] Gain BLock Gain Setting */
#define AK4951_EQ0      	0x20		/* [0] Gain Correction Filter(EQ0) / 0=OFF, 1=ON */
#define AK4951_FIL3     	0x10		/* [0] Stereo Emphasis Filter Control / 0=OFF, 1=ON */

/* AK4951_DIGITAL_FILTER_MODE (0x1D) */
#define AK4951_PFVOL1   		0x20		/* [0] Sidetone Digital Volume (Table 48) Default: 00 (0dB)*/
#define AK4951_PFVOL0   		0x10		/* [0] Sidetone Digital Volume (Table 48) Default: 00 (0dB)*/
#define AK4951_PFDAC1   		0x08		/* [0] DAC Input Signal Select (Table 49) Default: 00 (SDTI)*/
#define AK4951_PFDAC0   		0x04		/* [0] DAC Input Signal Select (Table 49) Default: 00 (SDTI)*/
#define AK4951_ADCPF   		0x02		/* [1] Programmable Filter/ALC Input Signal Select / 0=SDTI, 1=ADC Output */
#define AK4951_PFSDO   		0x01		/* [1] SDTO Output Signal Select / 0=ADC Output, 1=Programmable Filter/ALC Output */


/* AK4951_DIGITAL_FITER_SELECT 2 (0x30) */
#define AK4951_EQ5      	0x10		/* [0] Equalizer 5 Coefficient Setting Enable / 0=Disable, 1=Enable */
#define AK4951_EQ4      	0x08		/* [0] Equalizer 4 Coefficient Setting Enable / 0=Disable, 1=Enable */
#define AK4951_EQ3      	0x04		/* [0] Equalizer 3 Coefficient Setting Enable / 0=Disable, 1=Enable */
#define AK4951_EQ2      	0x02		/* [0] Equalizer 2 Coefficient Setting Enable / 0=Disable, 1=Enable */
#define AK4951_EQ1      	0x01		/* [0] Equalizer 1 Coefficient Setting Enable / 0=Disable, 1=Enable */

/* AK4951_EQ_COMMON_GAIN_SETTING (0x15) */
#define AK4951_EQC5       	0x10		/* [0] Equalizer 5 Common Gain Selector / 0=Disable, 1=Enable */
#define AK4951_EQC4       	0x08		/* [0] Equalizer 4 Common Gain Selector / 0=Disable, 1=Enable */
#define AK4951_EQC3       	0x04		/* [0] Equalizer 3 Common Gain Selector / 0=Disable, 1=Enable */
#define AK4951_EQC2       	0x02		/* [0] Equalizer 2 Common Gain Selector / 0=Disable, 1=Enable */

/* AK4951_05_MODE_CONTROL 1 (0x05) - PLL Fields */
#define AK4951_PLL_MASK				0xF0

/* AK4951_06_MODE_CONTROL 2 (0x06) - SAMPLING FREQUENCY Fields */
//#define AK4951_FS_MASK				0x0F

/* AK4951_06_MODE_CONTROL 2 (0x06) - MCKO FREQUENCY Fields */
#define AK4951_MCKO_MASK			0xC0


/* AK4951 Sequence setting */
#define AK4951_M_DELAY	0x100
//#define AK4951_CLOCKIN_START  0x101
//#define AK4951_CLOCKIN_STOP   0x102

#if 0
static UINT8 AK4951_ply_ALC_vol_table[33] = {
        /* AKM: mute,  -49~+32db */
	0x00, 0x0E, 0x15, 0x1C, 0x23, 0x2A, 0x31, 0x38,
	0x3F, 0x46, 0x4D, 0x54, 0x5B, 0x62, 0x69, 0x70,
	0x77, 0x7E, 0x85, 0x8C, 0x93, 0x9A, 0xA1, 0xA8,
	0xAF, 0xB6, 0xBD, 0xC4, 0xCB, 0xD2, 0xD9, 0xE0,
	0xE7
};

static UINT8 AK4951_rec_ALC_vol_table[33] = {
        /* AKM: mute, -49~+32db */
	0x00, 0x0E, 0x15, 0x1C, 0x23, 0x2A, 0x31, 0x38,
	0x3F, 0x46, 0x4D, 0x54, 0x5B, 0x62, 0x69, 0x70,
	0x77, 0x7E, 0x85, 0x8C, 0x93, 0x9A, 0xA1, 0xA8,
	0xAF, 0xB6, 0xBD, 0xC4, 0xCB, 0xD2, 0xD9, 0xE0,
	0xE7
};

static UINT8 AK4951_spk_digvol_table_r[33] = {
        /* AKM: mute, -89.5~12db */
	0xFF, 0xBA, 0xB4, 0xAE, 0xA8, 0xA2, 0x9C, 0x96,
	0x90, 0x8A, 0x84, 0x7E, 0x78, 0x72, 0x6C, 0x66,
	0x60, 0x5A, 0x54, 0x4E, 0x48, 0x42, 0x3C, 0x36,
	0x30, 0x2A, 0x24, 0x1E, 0x18, 0x12, 0x0C, 0x06,
	0x0
};

static UINT8 AK4951_spk_digvol_table_l[33] = {
       /* AKM: mute, -89.5~12db */
   0xFF, 0xBA, 0xB4, 0xAE, 0xA8, 0xA2, 0x9C, 0x96,
   0x90, 0x8A, 0x84, 0x7E, 0x78, 0x72, 0x6C, 0x66,
   0x60, 0x5A, 0x54, 0x4E, 0x48, 0x42, 0x3C, 0x36,
   0x30, 0x2A, 0x24, 0x1E, 0x18, 0x12, 0x0C, 0x06,
   0x0

};
#endif

#endif /* End of _AMBA_AUDIO_AK4951EN_H_ */
