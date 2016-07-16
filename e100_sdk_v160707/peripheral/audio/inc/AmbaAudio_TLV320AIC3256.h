/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaAudio_Tlv320Aic3256.h
 *
 *  @Copyright      :: Copyright (C) 2012 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: TI tlv320aic3256 audio AD/DA CODEC driver
\*-------------------------------------------------------------------------------------------------------------------*/

#ifndef _AMBA_AUDIO_TLV320AIC3256_H_
#define _AMBA_AUDIO_TLV320AIC3256_H_

#define TLV320AIC3256_I2C_ADDR  0x30
#define TLV320AIC3256_REG_NUM  128

#define SEND_SEQ(x) x,sizeof(x)/sizeof(x[0])

typedef struct {
    UINT8 RegOff;
    UINT8 RegVal;
} RegValue;

enum {
    REG_META_DELAY = 0x81,
    REG_META_SET,
    REG_META_RESET,
    REG_META_POLL
};

static const RegValue CodecReset[] = {
    {0x0, 0x0},		//#software reset
    {0x1, 0x1}
//     {0x0, 0xfe},	//#Delay 10ms
//     {0x0, 0xa},
};

static const RegValue CodecClkSetting[] = {
    {0x0, 0x0},
    {0x5, 0x91},
    {0x6, 0x07},
    {0x4, 0x03},
    {0x12, 0x8e},
    {0x13, 0x81},
    {0x0b, 0x8e},
    {0x0c, 0x81}
};

static const RegValue CodecSignalProc[] = {
    {0x0, 0x0},  // Select Page 0
    {0x3d, 0x07} // Set the ADC Mode to PRB_P1
};

static const RegValue CodecProcBlocks[] = {
    {0x0, 0x0},  // Select Page 0
    {0x3c, 0x08} // Set the DAC Mode to PRB_P8
};

static const RegValue CodecPowerSupplies[] = {
    {0x0, 0x1},    // Select Page 1
    {0x7c, 0x06},  // Charge Pump Peak Load Current selected Charge Pump Divider = 6, 333kHz
    {0x01, 0x0a},  // Disable weak AVDD to DVDD connection Charge Pump powered, source = int 8MHz OSC
    {0x02, 0x00},  // Enable Master Analog Power Control
    {0x7b, 0x05}   // Force REF charging time to 40ms
};

static const RegValue CodecInit[] = {
// 						//# Configure power supplies
// 						//# switch to page1
// {0x0, 0x1},
// 						//# Charge Pump 1x Current, 333kHz clock (8MHz/(6*4))
// {0x7c,0x6},
// 						//# Disable weak AVDD to DVDD connection, use OSC for CP
// {0x1,0xa},
// 						//# Enable Master Analog Power Control
// {0x2,0x0},
// 						//# Set the input power-up time to 3.1ms
// {0x47,0x32},
// 						//# Force REF charging time to 40ms
// {0x7b,0x5},
// 						//# switch to page0
// {0x0, 0x0},
// 						//# Mode = I2S, wordlength = 16bits
// {0x1b, 0x00}

    {0x00, 0x01}, //# switch to page1
    {0x01, 0x08}, //# Disable weak AVDD in presence of external AVDD supply
    {0x02, 0x00}, //# Enable Master Analog Power Control
    {0x3d, 0x00}, //# Select ADC PTM_R4
    {0x47, 0x32}, //# Set the input powerup time to 3.1ms (for ADC)
    {0x7b, 0x01} //# Set the REF charging time to 40ms
};

static const RegValue RecordingOn[] = {
// 						//# switch to page0
// {0x0, 0x0},
// 						//# Power up N/MADC and set NADC = 1, MADC = 2
// {0x12, 0x81},
// {0x13, 0x82},
// 						//# ADC OSR = 128
// {0x14, 0x80},
// 						//# Select ADC PRB_R1
// {0x3d, 0x1},
// 						//# switch to page1
// {0x0, 0x1},
// 						//# Set the input common mode to 0.9V
// {0xa, 0x0},
// 						//# Select ADC PTM_R4
// {0x3d, 0x0},
// 						//# Enabke MICBIAS and set to 2.5V
// {0x33, 0x68},
// 						//# Set MicPGA startup delay to 3.1ms
// {0x47, 0x32},
// 						//# Set the REF charging time to 40ms
// {0x7b, 0x1},
// 						//# Route IN1R to RIGHT_P with input impedance of 20K
// {0x37, 0x80},
// 						//# Route IN1L to RIGHT_M with input impedance of 20K
// {0x39, 0x20},
// 						//# Unmute Right MICPGA, Gain selection of 6dB to make channel gain 0dB
// 						//# Register of 6dB with input impedance of 20K => Channel Gain of 0dB
// {0x3c, 0x14},
// 						//# Select Page 0
// {0x0, 0x0},
// 						//# Power up Left and Right ADC Channels
// {0x51, 0xc0},
// 						//# Unmute Left and Right ADC Digital Volume Control.
// {0x52, 0x00}
    {0x00, 0x01}, //# switch to page1
    {0x33, 0x40}, //# Power on MICBIAS
    //{0x34, 0x80}, //# Route IN1L to LEFT_P with 20K input impedance
    //{0x36, 0x80}, //# Route Common Mode to LEFT_M with impedance of 20K
    //{0x37, 0x80}, //# Route IN1R to RIGHT_P with input impedance of 20K
    //{0x39, 0x80}, //# Route Common Mode to RIGHT_M with impedance of 20K
    {0x34, 0x80}, //# Route IN1L to LEFT_P with input impedance of 20K
    {0x36, 0x20}, //# Route Common Mode to LEFT_M with impedance of 20K
    {0x37, 0x00}, //# disconnect RIGHT_P
    {0x39, 0x00}, //# disconnect RIGHT_P
    //{0x3b, 0x14}, //# Unmute Left MICPGA, Gain selection of 10dB to make channel gain 0dB Register of 6dB with input impedance of 20K => Channel Gain of 0dB
    {0x3b, 0x80}, //# Unmute Left MICPGA
    //{0x3c, 0x80}, //# Unmute Right MICPGA, Gain selection of 28dB to make channel gain 0dB Register of 6dB with input impedance of 20K => Channel Gain of 0dB
    {0x00, 0x00}, //# Select Page 0
    {0x53, 0x14}, //# Left ADC Channel Volume = 10dB
    //{0x54, 0x14}, //# Left ADC Channel Volume = 10dB
    {0x51, 0x80}, //# Power up LADC
    //{0x51, 0x40}, //# Power up RADC
    {0x52, 0x08}, //# Unmute LADC
    //{0x52, 0x80}, //# Unmute RADC
};

static const RegValue RecordingOff[] = {
    //# Switch to page1
    {0x0, 0x1},
    //# Set Left MICPGA to 0dB
    {0x3b, 0x80},
    //# Set Right MICPGA to 0dB
    {0x3c, 0x80},
    //# Disable MICBIAS
    {0x33, 0x00},
    //# Switch to page0
    {0x0,0x0},
    //# Mute Left and Right ADC
    {0x52, 0x88},
    //# Power down ADC
    {0x51,0x00},
    //# Power down N/MADC
    {0x12, 0x00},
    {0x13, 0x00},
};

static const RegValue PlaybackOn[] = {
// 						//# configuring clock tree
// 						//# Select Page 0
// {0x0, 0x0},
// 						//# Return to normal initializing sequence
// 						//# The codec receives: MCLK = 24MHz, sampling rate=44.1KHz
// 						//# Power up N/MDAC and set NDAC =1, MDAC = 2,
// {0xb, 0x81},
// {0xc, 0x82},
// 						//# DOSR = 128
// {0xd, 0x00},
// {0xe, 0x80},
// 						//# Set the DAC Mode to PRB_P1
// {0x3c, 0x1},
// 						//# Select Page 1
// {0x0, 0x1},
// 						//# Set the DAC PTM mode to PTM_P3/4
// {0x3, 0x0},
// {0x4, 0x0},
// 						//# Route Left DAC to LOL
// {0xe, 0x8},
// 						//# Route Right DAC to LOR
// {0xf, 0x8},
// 						//# Set the LOL gain to 5dB
// {0x12, 0x5},
// 						//# Set the LOR gain to 5dB
// {0x13, 0x5},
// 						//# Power up LOL and LOR drivers
// {0x09, 0xc},
// 						//# Select Page 0
// {0x0, 0x0},
// 						//# Set DAC gain = 0dB
// {0x41,0x00},
// {0x42,0x00},
// 						//# Power up the LDAC Channels with route the Left Audio digital data to
// 						//# Left Channel DAC and Right Audio digital data to Right Channel DAC
// {0x3f, 0x94},
// 						//# Unmute the DAC digital volume control and set RDAC as inverted version
// 						//# of Left DAC Modulator output.
// {0x40, 0x80},
    {0x00, 0x01}, //# Select Page 1
    //{0x0c, 0x08}, //# Route LDAC/RDAC to HPL/HPR
    {0x0e, 0x08}, //# Route LDAC to LOL
    {0x0f, 0x08}, //# Route RDAC to LOR
    {0x00, 0x00}, //# Select Page 0
    {0x3f, 0xd6}, //# Power up LDAC/RDAC
    {0x00, 0x01}, //# Select Page 1
    {0x7d, 0x12}, //# GCHP Mode, DC Offset corr Enabled on present routing
    //{0x10, 0x00}, //# un-mute HPL, 0dB Gain
    //{0x11, 0x00}, //# un-mute HPR, 0dB Gain
    {0x12, 0x00}, //# LOL not muted, 0dB gain
    {0x13, 0x00}, //# LOR not muted, 0dB gain
    {0x09, 0x0c}, //# Only power on LOL/LOR, Power off HPL/HPR drivers
    {0x00, 0x00}, //# Select Page 0
    {0x40, 0x00}  //# Unmute LDAC/RDAC

};

static const RegValue PlaybackOff[] = {
    //# Select Page 1
    {0x0, 0x1},
    //# Mute LOL driver
    {0x12, 0x40},
    //# Mute LOR driver
    //w 30 13 40
    {0x13, 0x40},
    //# PowerDown HP and LO
    {0x09, 0x00},
    //# Un-route Left DAC to LOL
    {0xe, 0x0},
    //# Un-route Right DAC to LOR
    {0xf, 0x0},
    //# Select Page 0
//{0x00, 0x00},
    //# Power Down DAC
//{0x3f, 0x14}
};

static const RegValue CodecHpOutOn[] = {
    //# Select Page 0
    {0, 0},
    //# DAC Processing PRB_P8
    {0x3c, 0x08},
    //# Configure DAC Channel
    //# Select Page 01
    {0x0, 0x1},
    //# Disable Internal Crude AVdd in presence of external AVdd supply or before
    //# powering up internal AVdd LDO
    {0x1, 0x8},
    //# Enable Master Analog Power Control
    {0x2, 0x0},
    //# Set the REF charging time to 40ms
    {0x7b, 0x1},
    //# HP soft stepping settings for optimal pop performance at power up
    //# Rpop used is 6k with N = 6 and soft step = 20usec. This should work with 47uF coupling
    //# capacitor. Can try N=5,6 or 7 time constants as well. Trade-off delay vs ��pop? sound.
    {0x14, 0x25},
    //# Set the Input Common Mode to 0.9V and Output Common Mode for Headphone to
    //# Input Common Mode
    {0xa, 0x0},
    //# Route Left DAC to HPL
    {0xc, 0x8},
    //# Route Right DAC to HPR
    {0xd, 0x8},
    //# Set the DAC PTM mode to PTM_P3/4
    {0x3, 0x0},
    {0x4, 0x0},
    //# Unmute HPL and set gain to 0dB
    {0x10, 0x0},
    //# Unmute HPR and set gain to 0dB
    {0x11, 0x0},
    //# Power up HPL and HPR drivers
    {0x09, 0x30},

    //# Wait for 2.5 sec for soft stepping to take effect
    //# Else read Page 1, Register 63d, D(7:6). When = ��11? soft-stepping is complete
    //  �b?? m�[�Jde-pop??��

    //# Select Page 0
    {0x0, 0x0},
    //# Set DAC gain = 0dB
    {0x41,0x00},
    {0x42,0x00},
    //# Power up the Left and Right DAC Channels with route the Left Audio digital data to
    //# Left Channel DAC and Right Audio digital data to Right Channel DAC
    {0x3f, 0xd6},
    //# Unmute the DAC digital volume control
    {0x40, 0x0},
};


static const RegValue CodecHpOff[] = {
    //# Select Page 1
    {0x0, 0x1},
    //# Set HPL to mute
    {0x10, 0x40},
    //# Set HPR to mute
    {0x11, 0x40},
    //# PowerDown HP and LO
    {0x09, 0x00},
    //# Un-route Left DAC to HPL
    {0x0c, 0x00},
    //# Un-route Right DAC to HPR
    {0x0d, 0x00},
    //# Select Page 0
//{0x00, 0x00},
    //# Power Down DAC
//{0x3f, 0x14},
};

#endif /* End of _AMBA_AUDIO_TLV320AIC3256_H_ */
