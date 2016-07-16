/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaGPIO.h
 *
 *  @Copyright      :: Copyright (C) 2013 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Definitions & Constants for GPIO APIs
\*-------------------------------------------------------------------------------------------------------------------*/

#ifndef _AMBA_B5_GPIO_H_
#define _AMBA_B5_GPIO_H_

#define AMBA_B5_NUM_GPIO_PIN    32

typedef enum _AMBA_B5_GPIO_LEVEL_e_ {
    AMBA_B5_GPIO_LEVEL_LOW = 0,         /* Pin level state is low */
    AMBA_B5_GPIO_LEVEL_HIGH,            /* Pin level state is high */
    AMBA_B5_GPIO_LEVEL_UNKNOWN          /* Pin level state is unknown (due to pin mask is disabled) */
} AMBA_B5_GPIO_LEVEL_e;

/*-----------------------------------------------------------------------------------------------*\
 * Defined in AmbaB5_GPIO.c
\*-----------------------------------------------------------------------------------------------*/
int AmbaB5_GpioInit(void);
int AmbaB5_GpioSetInput(UINT8 GpioPinID);
int AmbaB5_GpioSetOutput(UINT8 GpioPinID, AMBA_B5_GPIO_LEVEL_e PinLevel);

#endif /* _AMBA_B5_GPIO_H_ */
