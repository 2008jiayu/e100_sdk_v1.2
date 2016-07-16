/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaRTSL_GPIO.h
 *
 *  @Copyright      :: Copyright (C) 2012 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Definitions & Constants for GPIO Control APIs
\*-------------------------------------------------------------------------------------------------------------------*/

#ifndef _AMBA_RTSL_GPIO_H_
#define _AMBA_RTSL_GPIO_H_

#include "AmbaGPIO_Def.h"

/*-----------------------------------------------------------------------------------------------*\
 * Defined in AmbaRTSL_GPIO.c
\*-----------------------------------------------------------------------------------------------*/
void AmbaRTSL_GpioInit(AMBA_GPIO_PIN_GROUP_CONFIG_s *pGpioGroupConfig);
int AmbaRTSL_GpioSetDriveStrength(AMBA_GPIO_PIN_ID_e GpioPinID, AMBA_GPIO_PIN_DRIVE_STRENGTH_e PinDriveStrength);

int AmbaRTSL_GpioConfigInput(AMBA_GPIO_PIN_ID_e GpioPinConfig);
int AmbaRTSL_GpioConfigOutput(AMBA_GPIO_PIN_ID_e GpioPinConfig, AMBA_GPIO_PIN_LEVEL_e PinLevel);
int AmbaRTSL_GpioConfigAltFunc(AMBA_GPIO_PIN_ID_e GpioPinConfig);
int AmbaRTSL_GpioGetPinInfo(AMBA_GPIO_PIN_ID_e GpioPinID, AMBA_GPIO_PIN_INFO_s *pPinInfo);

int AmbaRTSL_GpioPullCtrl(AMBA_GPIO_PIN_ID_e GpioPinID, AMBA_GPIO_PIN_PULL_CTRL_e PullCtrl);

#endif /* _AMBA_RTSL_GPIO_H_ */
