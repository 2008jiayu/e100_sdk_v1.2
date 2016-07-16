/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaB5_INT.h
 *
 *  @Copyright      :: Copyright (C) 2013 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Definitions & Constants for B5 Interrupt Controller APIs
\*-------------------------------------------------------------------------------------------------------------------*/

#ifndef _AMBA_B5_INT_H_
#define _AMBA_B5_INT_H_

#include "AmbaGPIO.h"

typedef enum _AMBA_B5_INT_ID_e_ {
    AMBA_B5_INT_ID0_SSI = 0,
    AMBA_B5_INT_ID1_GPIO,
    AMBA_B5_INT_ID2_I2C0,
    AMBA_B5_INT_ID3_I2C1,
    AMBA_B5_INT_ID4_PWM_ENC,
    AMBA_B5_INT_ID5_SSI_2SP,
    AMBA_B5_INT_ID6_CH0_DEPACKER,
    AMBA_B5_INT_ID7_CH1_DEPACKER,
    AMBA_B5_INT_ID8_CH2_DEPACKER,
    AMBA_B5_INT_ID9_CH3_DEPACKER,
    AMBA_B5_INT_ID10_RCT,
    AMBA_B5_INT_ID11_VIN_VSYNC0,
    AMBA_B5_INT_ID12_VIN_VSYNC1,
    AMBA_B5_INT_ID13_VIN_VSYNC2,
    AMBA_B5_INT_ID14_VIN_VSYNC3,
    AMBA_B5_INT_ID15_VIN_SOF0,
    AMBA_B5_INT_ID16_VIN_SOF1,
    AMBA_B5_INT_ID17_VIN_SOF2,
    AMBA_B5_INT_ID18_VIN_SOF3,
    AMBA_B5_INT_ID19_VIN_MASTER_VSYNC0,
    AMBA_B5_INT_ID20_VIN_MASTER_VSYNC1,
    AMBA_B5_INT_ID21_VIN_MASTER_VSYNC2,
    AMBA_B5_INT_ID22_VIN_MASTER_VSYNC3,
    AMBA_B5_INT_ID23,
    AMBA_B5_INT_ID24,
    AMBA_B5_INT_ID25,
    AMBA_B5_INT_ID26,
    AMBA_B5_INT_ID27_VOUTF,
    AMBA_B5_INT_ID28_VIN_DELAYED_VSYNC0,
    AMBA_B5_INT_ID29_VIN_DELAYED_VSYNC1,
    AMBA_B5_INT_ID30_VIN_DELAYED_VSYNC2,
    AMBA_B5_INT_ID31_VIN_DELAYED_VSYNC3,

    AMBA_NUM_B5_INTERRUPT   /* Total number of Interrupts supported */
} AMBA_B5_INT_ID_e;

typedef enum _AMBA_B5_INT_CONFIG_e_ {
    INT_HIGH_LEVEL_TRIGGER = 0,             /* Corresponding interrupt is High level sensitive */
    INT_RISING_EDGE_TRIGGER                 /* Corresponding interupt is edge-triggered */
} AMBA_B5_INT_CONFIG_e;

typedef void (*AMBA_B5_ISR_f)(int IntID);

/*-----------------------------------------------------------------------------------------------*\
 * Defined in AmbaB5_INT.c
\*-----------------------------------------------------------------------------------------------*/
int AmbaB5_IntInit(void);
int AmbaB5_IntIsrHook(AMBA_B5_INT_ID_e IntID, AMBA_B5_INT_CONFIG_e IntConfig, AMBA_B5_ISR_f IsrEntry);
int AmbaB5_IntEnable(AMBA_B5_INT_ID_e IntID);
int AmbaB5_IntDisable(AMBA_B5_INT_ID_e IntID);
void AmbaB5_IntHandler(int UnUsed);

#endif /* _AMBA_B5_INT_H_ */
