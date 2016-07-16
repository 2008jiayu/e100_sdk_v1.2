/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaPWM.h
 *
 *  @Copyright      :: Copyright (C) 2012 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Definitions & Constants for PWM Middleware APIs
\*-------------------------------------------------------------------------------------------------------------------*/

#ifndef _AMBA_PWM_H_
#define _AMBA_PWM_H_

#include "AmbaPWM_Def.h"

/*-----------------------------------------------------------------------------------------------*\
 * Defined in AmbaPWM.c
\*-----------------------------------------------------------------------------------------------*/
int AmbaPWM_Init(void);
int AmbaPWM_SetConfig(AMBA_PWM_CHANNEL_e PwmChanNo, AMBA_PWM_CONFIG_s *pPwmConfig);
int AmbaPWM_GetConfig(AMBA_PWM_CHANNEL_e PwmChanNo, AMBA_PWM_CONFIG_s *pPwmConfig);
int AmbaPWM_Enable(AMBA_PWM_CHANNEL_e PwmChanNo);
int AmbaPWM_Disable(AMBA_PWM_CHANNEL_e PwmChanNo);
int AmbaPWM_SetPulseWidth(AMBA_PWM_CHANNEL_e PwmChanNo, UINT32 PulseWidth);
int AmbaPWM_GetPulseWidth(AMBA_PWM_CHANNEL_e PwmChanNo, UINT32 *pPulseWidth);

#endif /* _AMBA_PWM_H_ */
