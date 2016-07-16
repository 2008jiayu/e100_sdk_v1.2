/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaAudio_WM8974.h
 *
 *  @Copyright      :: Copyright (C) 2012 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: WM8974 audio AD/DA CODEC driver
\*-------------------------------------------------------------------------------------------------------------------*/

#ifndef _AMBA_AUDIO_WM8974_H_
#define _AMBA_AUDIO_WM8974_H_

#define WM8974_I2C_ADDR           0x1a

#define WM8974_REG_NUM			57
//#define ENABLE_ALC_NOISE_GATE

typedef enum WM8974_REG_SETS_e_ {
	WM8974_SW_RESET_REG = 0x0,
	WM8974_PWR_MANAGE1_REG = 0x1,
	WM8974_PWR_MANAGE2_REG = 0x2,
	WM8974_PWR_MANAGE3_REG = 0x3,
	WM8974_AU_INTERFACE_CTRL_REG = 0x4,
	WM8974_COMPANDING_CTRL_REG = 0x5,
	WM8974_CLK_GN_CTRL_REG = 0x6,
	WM8974_ADDITIONAL_CTRL_REG = 0x7,
	WM8974_DAC_CTRL_REG = 0xa,
	WM8974_BEEP_CTRL_REG = 0x2b,
	WM8974_IN_PGA_VOL_REG = 0x2d,
  WM8974_SPK_MIXER_CTRL_REG = 0x32,
  WM8974_MONO_MIXER_CTRL_REG = 0x38,
} WM8974_REG_SETS_e;

typedef enum WM8974_REG_DEFINITIONS_e_ {
	WM8974_R01H_VMIDSEL_MASK = 0x1fc,
	WM8974_R01H_VMIDSEL_75K = 0x1,
	WM8974_R01H_VMIDSEL_5K = 0x3,
	WM8974_R01H_BUFIOEN = (1 << 2),
	WM8974_R01H_BIASEN = (1 << 3),
	WM8974_R01H_MICBEN = (1 << 4),

	WM8974_R02H_ADCEN = 1,
	WM8974_R02H_INPPGAEN = (1 << 2),
	WM8974_R02H_BOOSTEN = (1 << 4),

	WM8974_R03H_DACEN     = 1,
	WM8974_R03H_SPKMIXEN  = 1<<2,
	WM8974_R03H_MONOMIXEN = 1<<3,
	WM8974_R03H_SPKPEN    = 1<<5,
	WM8974_R03H_SPKNEN    = 1<<6,
	WM8974_R03H_MONOOUTEN = 1<<7,

	WM8974_R04H_WL_MASK = 0x19f,
	WM8974_R04H_WL_16B = 0x19f,
	WM8974_R04H_WL_24B = (1 << 6),

	WM8974_R05H_LOOPBACK = 1,

	WM8974_R06H_CODEC_SLAVE = 0x000,

	WM8974_R07H_SR_MASK = 0x1f1,
	WM8974_R07H_SR_48K = 0x0,
	WM8974_R07H_SR_32K = 0x2,
	WM8974_R07H_SR_24K = 0x4,
	WM8974_R07H_SR_16K = 0x6,
	WM8974_R07H_SR_12K = 0x8,
	WM8974_R07H_SR_8K = 0xa,

	WM8974_R0AH_SOFTMUTE = (1 << 6),

	WM8974_R2BH_INVROUT2 = (1 << 4),

	WM8974_R2DH_PGA_MASK = 0x1c0,
	WM8974_R2DH_PGA_MAX = 0x03f,
  WM8974_R2DH_PGA_24dB = 0x030,
  WM8974_R2DH_PGA_15dB = 0x024,
  WM8974_R2DH_PGA_5dB = 0x017,
	WM8974_R2DH_PGA_0dB = 0x010,
	WM8974_R2DH_INPPGAMUTE = (1 << 6),
	WM8974_R2DH_PGAZCL = (1 << 7),

	WM8974_R2EH_PGA_UPDATE = (1 << 8),

  WM8974_R32H_DAC2SPK = 1,

  WM8974_R38H_DAC2MONO = 1,
} WM8974_REG_DEFINITIONS_e;

#endif /* _AMBA_AUDIO_WM8974_H_ */
