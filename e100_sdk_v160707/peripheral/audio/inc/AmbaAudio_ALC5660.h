/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaAudio_ALC5660.h
 *
 *  @Copyright      :: Copyright (C) 2012 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: REALTEK ALC5660 audio AD/DA CODEC driver
\*-------------------------------------------------------------------------------------------------------------------*/

#ifndef _AMBA_AUDIO_ALC5660_H_
#define _AMBA_AUDIO_ALC5660_H_

#define ALC5660_I2C_ADDR  0x38
#define ALC5660_REG_NUM  255

#define SEND_SEQ(x) x,sizeof(x)/sizeof(x[0])

#define ALC5660_RESET                0X00      //RESET CODEC TO DEFAULT
#define ALC5660_SPK_OUT_VOL          0X01      //SPEAKER OUT VOLUME
#define ALC5660_HP_OUT_VOL           0X02      //HEADPHONE OUTPUT VOLUME
#define ALC5660_IN_1_2               0X0D      //
#define ALC5660_IN_3_4               0X0E      //
#define ALC5660_DAC_DIG_VOL          0X19      //DAC1 DIGITAL VOLUME
#define ALC5660_ADC_DIG_VOL          0X1C      //ADC DIGITAL VOLUME
#define ALC5660_ADC_BST_VOL          0X1E      //ADC BOOST VOLUME
#define ALC5660_STEREO_ADC_MIXER     0X27      //STEREO ADC MIXER
#define ALC5660_ADC_TO_DAC_MIXER     0X29      //ADC TO DAC MIXER
#define ALC5660_STEREO_DAC_MIXER     0X2A      //STEREO DAC MIXER
#define ALC5660_DIG_INT_DATA         0X2F      //DIGITAL INTERFACE DATA
#define ALC5660_REC_LEFT_MIXER_1     0X3B      //RECORDER LEFT MIXER CONTROL_1
#define ALC5660_REC_LEFT_MIXER_2     0X3C      //RECORDER LEFT MIXER CONTROL_2
#define ALC5660_REC_RIGHT_MIXER_1    0X3D      //RECORDER RIGHT MIXER CONTROL_1
#define ALC5660_REC_RIGHT_MIXER_2    0X3E      //RECORDER RIGHT MIXER CONTROL_2
#define ALC5660_HPMIX                0X45      //HP MIXER CONTROL
#define ALC5660_SPK_LEFT_MIXER       0X46      //SPEAKER LEFT MIXER CONTROL
#define ALC5660_SPKMIX               0X48      //SPKOUT MIXER CONTROL
#define ALC5660_SPKMIX_RATIO         0X4A      //SPOLMIX / SPORMIX RATIO CONTROL
#define ALC5660_OUTPUT_LEFT_MIXER_1  0X4D      //OUTPUT LEFT MIXER CONTROL_1
#define ALC5660_OUTPUT_LEFT_MIXER_2  0X4E      //OUTPUT LEFT MIXER CONTROL_2
#define ALC5660_OUTPUT_LEFT_MIXER_3  0X4F      //OUTPUT LEFT MIXER CONTROL_3
#define ALC5660_OUTPUT_RIGHT_MIXER_1 0X50      //OUTPUT RIGHT MIXER CONTROL_1
#define ALC5660_OUTPUT_RIGHT_MIXER_2 0X51      //OUTPUT RIGHT MIXER CONTROL_2
#define ALC5660_OUTPUT_RIGHT_MIXER_3 0X52      //OUTPUT RIGHT MIXER CONTROL_3
#define ALC5660_PWR_MANAG_DIG_1      0X61      //POWER MANAGEMENT FOR DIGITAL_1
#define ALC5660_PWR_MANAG_DIG_2      0X62      //POWER MANAGEMENT FOR DIGITAL_2
#define ALC5660_PWR_MANAG_ANALOG_1   0X63      //POWER MANAGEMENT FOR ANALOG_1
#define ALC5660_PWR_MANAG_ANALOG_2   0X64      //POWER MANAGEMENT FOR ANALOG_2
#define ALC5660_PWR_MANAG_MIXER      0X65      //POWER MANAGEMENT FOR MIXER
#define ALC5660_PWR_MANAG_VOL        0X66      //POWER MANAGEMENT FOR VOLUME
#define ALC5660_PRI_REG_INDEX        0X6A
#define ALC5660_PRI_REG_DATA         0X6C
#define ALC5660_I2S_SDP              0X70      //I2S1 AUDIO SERIAL DATA PORT CONTROL
#define ALC5660_ADC_DAC_CLK_1        0X73      //ADC/DAC CLOCK CONTROL_1
#define ALC5660_ADC_DAC_CLK_2        0X74      //ADC/DAC CLOCK CONTROL_2
#define ALC5660_DIG_MIC              0X75      //DIGITAL MICROPHONE CONTROL
#define ALC5660_DEPOP_MODE_1         0X8E      //DE-POP MODE CONTROL_1
#define ALC5660_DEPOP_MODE_2         0X8F      //DE-POP MODE CONTROL_2
#define ALC5660_GPIO_1               0XC0      //GPIO CONTROL_1
#define ALC5660_GPIO_3               0XC2      //GPIO CONTROL_3
#define ALC5660_ADJ_HIGH_PASS_FILTER 0XD3      //ADJUSTABLE HIGH PASS FILTER CONTROL
#define ALC5660_DUMMY_REG_1          0XFA      //DUMMY REGISTER_1

//Data Length Slection
#define SDP_I2S_DL_MASK         (0x3<<2)    //Stereo Serial Data Length mask
#define SDP_I2S_DL_16         (0x0<<2)    //16 bits
#define SDP_I2S_DL_20         (0x1<<2)    //20 bits
#define SDP_I2S_DL_24         (0x2<<2)    //24 bits
#define SDP_I2S_DL_8          (0x3<<2)    //8  bits

//PCM Data Format Selection
#define SDP_I2S_DF_MASK         (0x3)     //main i2s Data Format mask
#define SDP_I2S_DF_I2S          (0x0)     //I2S FORMAT
#define SDP_I2S_DF_LEFT           (0x1)     //Left JUSTIFIED
#define SDP_I2S_DF_PCM_A        (0x2)     //PCM format A
#define SDP_I2S_DF_PCM_B        (0x3)     //PCM format B

#endif /* End of _AMBA_AUDIO_ALC5660_H_ */
