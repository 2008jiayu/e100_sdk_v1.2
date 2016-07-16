/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaPWC.h
 *
 *  @Copyright      :: Copyright (C) 2014 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Definitions & Constants for PWC Middleware APIs
\*-------------------------------------------------------------------------------------------------------------------*/

#ifndef _AMBA_PWC_H_
#define _AMBA_PWC_H_

#include "AmbaPWC_Def.h"

/*-----------------------------------------------------------------------------------------------*\
 * Defined in AmbaPWC.c
\*-----------------------------------------------------------------------------------------------*/
int AmbaPWC_Init(void);
int AmbaPWC_ConfigPowerUpSequence(UINT32 Delay0, UINT32 Delay1, UINT32 Delay2, UINT32 Delay3);
int AmbaPWC_WakeupByAlarm(UINT32 TimeDelay);
int AmbaPWC_ForcePseq3State(UINT32 State);
int AmbaPWC_SetDdrFreq1State(UINT32 State);
int AmbaPWC_SetLostPowerState(UINT32 State);
int AmbaPWC_SetUserDefinedStatus(UINT32 Status);

UINT32 AmbaPWC_GetStatus(void);
int AmbaPWC_GetWakeupByAlarmState(void);
int AmbaPWC_GetForcePseq3State(void);
int AmbaPWC_GetDdrFreq1State(void);
int AmbaPWC_GetLostPowerState(void);
UINT32 AmbaPWC_GetUserDefinedStatus(void);

int AmbaPWC_GetLowBatteryState(void);
AMBA_PWC_WAKEUP_REASON_e AmbaPWC_GetWakeUpReason(void);

int AmbaPWC_ForcePowerDownSequence(void);

int AmbaPWC_DisableWakeupByAlarm(void);
int AmbaPWC_ClearStatus(UINT32 ClearFlag);

#endif /* _AMBA_PWC_H_ */
