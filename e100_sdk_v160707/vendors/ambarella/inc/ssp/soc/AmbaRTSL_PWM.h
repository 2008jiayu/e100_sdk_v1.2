/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaRTSL_PWM.h
 *
 *  @Copyright      :: Copyright (C) 2013 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Definitions & Constants for PWM RTSL APIs
\*-------------------------------------------------------------------------------------------------------------------*/

#ifndef _AMBA_RTSL_PWM_H_
#define _AMBA_RTSL_PWM_H_

#include "AmbaPWM_Def.h"

/*-----------------------------------------------------------------------------------------------*\
 * Defined in AmbaRTSL_PWM.c
\*-----------------------------------------------------------------------------------------------*/
void   AmbaRTSL_PwmInit(void);

int    AmbaRTSL_PwmSetConfig(AMBA_PWM_CHANNEL_e PwmChanNo, AMBA_PWM_CONFIG_s *pPwmConfig);
void   AmbaRTSL_PwmGetConfig(AMBA_PWM_CHANNEL_e PwmChanNo, AMBA_PWM_CONFIG_s *pPwmConfig);
void   AmbaRTSL_PwmEnable(AMBA_PWM_CHANNEL_e PwmChanNo);
void   AmbaRTSL_PwmDisable(AMBA_PWM_CHANNEL_e PwmChanNo);
void   AmbaRTSL_PwmSetPulseWidth(AMBA_PWM_CHANNEL_e PwmChanNo, UINT32 PulseWidth);
int    AmbaRTSL_PwmGetPulseWidth(AMBA_PWM_CHANNEL_e PwmChanNo, UINT32 *pPulseWidth);

#endif /* _AMBA_RTSL_PWM_H_ */
