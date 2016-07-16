/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaRTSL_GPIO_Def.h
 *
 *  @Copyright      :: Copyright (C) 2013 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Common definitions & constants for GPIO RTSL Control APIs
\*-------------------------------------------------------------------------------------------------------------------*/

#ifndef _AMBA_RTSL_GPIO_DEF_H_
#define _AMBA_RTSL_GPIO_DEF_H_

/*-----------------------------------------------------------------------------------------------*\
 * RTSL GPIO Management Structure
\*-----------------------------------------------------------------------------------------------*/
typedef struct _AMBA_RTSL_GPIO_CTRL_s_ {
    UINT32  Function[3];                /* GPIO or alternate function */
    UINT32  Direction;                  /* Input or output */
    UINT32  Mask;                       /* Read-only or read-writeable */
    UINT32  State;                      /* Logical zero or logical one */
    UINT32  PullEnable;                 /* Disable/enable internal resistance */
    UINT32  PullSelect;                 /* Internal resistance is pull-down or pull-up */
    UINT32  DriveStrength[2];           /* Driving strength is 3mA/6mA/12mA/18mA */

    UINT16  IntID;                      /* Interrupt ID */
    void    (*pISR)(int IntID);         /* pointer to the group ISR */

    AMBA_GPIO_ISR_f GPIO_IsrTable[32];  /* The table of line ISRs */
} AMBA_RTSL_GPIO_CTRL_s;

/*-----------------------------------------------------------------------------------------------*\
 * Defined in AmbaRTSL_GPIO.c
\*-----------------------------------------------------------------------------------------------*/
extern void (*AmbaRTSL_GpioIsrCallBack)(AMBA_GPIO_GROUP_e GpioGroupNo);

int AmbaRTSL_GpioIsrHook(AMBA_GPIO_PIN_ID_e GpioPinID, AMBA_GPIO_ISR_f IsrEntry);
int AmbaRTSL_GpioIntConfig(AMBA_GPIO_PIN_ID_e GpioPinID, AMBA_GPIO_INT_CONFIG_e IntConfig);
int AmbaRTSL_GpioIntEnable(AMBA_GPIO_PIN_ID_e GpioPinID);
int AmbaRTSL_GpioIntDisable(AMBA_GPIO_PIN_ID_e GpioPinID);
int AmbaRTSL_GpioIntClear(AMBA_GPIO_PIN_ID_e GpioPinID);

#endif /* _AMBA_RTSL_GPIO_DEF_H_ */
