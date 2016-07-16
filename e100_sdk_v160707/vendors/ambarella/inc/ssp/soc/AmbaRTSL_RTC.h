/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaRTSL_RTC.h
 *
 *  @Copyright      :: Copyright (C) 2012 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Definitions & Constants for Power and RTC (Real-Time Clock) Controller RTSL APIs
\*-------------------------------------------------------------------------------------------------------------------*/

#ifndef _AMBA_RTSL_RTC_H_
#define _AMBA_RTSL_RTC_H_

#include "AmbaCSL_RTC.h"

/*-----------------------------------------------------------------------------------------------*\
 * Defined in AmbaRTSL_RTC.c
\*-----------------------------------------------------------------------------------------------*/
void    AmbaRTSL_RtcSetSystemTime(UINT32 Seconds);
UINT32  AmbaRTSL_RtcGetSystemTime(void);

#endif /* _AMBA_RTSL_RTC_H_ */
