/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaGPIO.h
 *
 *  @Copyright      :: Copyright (C) 2012 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Definitions & Constants for GPIO Middleware APIs
\*-------------------------------------------------------------------------------------------------------------------*/

#ifndef _AMBA_GPIO_H_
#define _AMBA_GPIO_H_

#include "AmbaGPIO_Def.h"

/*-----------------------------------------------------------------------------------------------*\
 * Defined in AmbaGPIO.c
\*-----------------------------------------------------------------------------------------------*/
int AmbaGPIO_Init(void);

int AmbaGPIO_SetDriveStrength(AMBA_GPIO_PIN_ID_e GpioPinID, AMBA_GPIO_PIN_DRIVE_STRENGTH_e PinDriveStrength);

int AmbaGPIO_ConfigInput(AMBA_GPIO_PIN_ID_e GpioPinID);
int AmbaGPIO_ConfigOutput(AMBA_GPIO_PIN_ID_e GpioPinID, AMBA_GPIO_PIN_LEVEL_e PinLevel);
int AmbaGPIO_ConfigAltFunc(AMBA_GPIO_PIN_ID_e GpioPinID);
int AmbaGPIO_GetPinInfo(AMBA_GPIO_PIN_ID_e GpioPinID, AMBA_GPIO_PIN_INFO_s *pPinInfo);

int AmbaGPIO_IsrHook(AMBA_GPIO_PIN_ID_e GpioPinID, AMBA_GPIO_INT_CONFIG_e IntConfig, AMBA_GPIO_ISR_f IsrEntry);
int AmbaGPIO_IntConfig(AMBA_GPIO_PIN_ID_e GpioPinID, AMBA_GPIO_INT_CONFIG_e IntConfig);
int AmbaGPIO_IntEnable(AMBA_GPIO_PIN_ID_e GpioPinID);
int AmbaGPIO_IntDisable(AMBA_GPIO_PIN_ID_e GpioPinID);

int AmbaGPIO_PullCtrl(AMBA_GPIO_PIN_ID_e GpioPinID, AMBA_GPIO_PIN_PULL_CTRL_e PullCtrl);

#endif /* _AMBA_GPIO_H_ */
