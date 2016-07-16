/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaRTSL_PWC.h
 *
 *  @Copyright      :: Copyright (C) 2014 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Definitions & Constants for PWC RTSL APIs
\*-------------------------------------------------------------------------------------------------------------------*/

#ifndef _AMBA_RTSL_PWC_H_
#define _AMBA_RTSL_PWC_H_

#include "AmbaPWC_Def.h"
#include "AmbaCSL_PWC.h"

#define AmbaRTSL_PwcConfigPowerUpSequence(d0, d1, d2, d3)   AmbaCSL_PwcConfigPowerUpSequence(d0, d1, d2, d3)

#define AmbaRTSL_PwcGetStatus()                     AmbaCSL_PwcGetCurStatus()
#define AmbaRTSL_PwcGetForcePseq3State()            AmbaCSL_PwcGetPseq3State()
#define AmbaRTSL_PwcGetAlarmWakeupEnableState()     AmbaCSL_PwcGetAlarmWakeupEnableState()
#define AmbaRTSL_PwcGetWakeupByAlarmState()         AmbaCSL_PwcGetWakeupByAlarmState()

#define AmbaRTSL_PwcGetDdrFreq1State()              AmbaCSL_PwcGetDdrFreq1State()
#define AmbaRTSL_PwcGetLostPowerState()             AmbaCSL_PwcGetLostPowerState()
#define AmbaRTSL_PwcGetUserDefinedStatus()          AmbaCSL_PwcGetUserDefinedStatus()

#define AmbaRTSL_PwcGetLowBatteryState()            AmbaCSL_PwcGetLowBatteryState()

#define AmbaRTSL_PwcTogglePCRST()                   AmbaCSL_PwcTogglePCRST()

/*-----------------------------------------------------------------------------------------------*\
 * Defined in AmbaRTSL_PWC.c
\*-----------------------------------------------------------------------------------------------*/
void AmbaRTSL_PwcInit(void);

void AmbaRTSL_PwcForcePseq3State(UINT32 State);
void AmbaRTSL_PwcWakeupByAlarm(UINT32 TimeDelay);
void AmbaRTSL_PwcSetDdrFreq1State(UINT32 State);
void AmbaRTSL_PwcSetLostPowerState(UINT32 State);
void AmbaRTSL_PwcSetWakeupByAlarmState(UINT32 State);
void AmbaRTSL_PwcSetUserDefinedStatus(UINT32 Status);

void AmbaRTSL_PwcDisableWakeupByAlarm(void);
void AmbaRTSL_PwcClearStatus(UINT32 ClearFlag);

AMBA_PWC_WAKEUP_REASON_e AmbaRTSL_PwcGetWakeUpReason(void);

#endif /* _AMBA_RTSL_PWC_H_ */
