/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaRTSL_WDT.h
 *
 *  @Copyright      :: Copyright (C) 2012 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Definitions & Constants for WDT control APIs
\*-------------------------------------------------------------------------------------------------------------------*/

#ifndef _AMBA_RTSL_WDT_H_
#define _AMBA_RTSL_WDT_H_

#include "AmbaWDT_Def.h"

/*-----------------------------------------------------------------------------------------------*\
 * Defined in AmbaRTSL_WDT.c
\*-----------------------------------------------------------------------------------------------*/
int AmbaRTSL_WdtStart(AMBA_WDT_CONFIG_s *pWdtConfig);
void AmbaRTSL_WdtRestart(void);
void AmbaRTSL_WdtStop(void);
AMBA_WDT_MODE_e AmbaRTSL_WdtGetMode(void);
int AmbaRTSL_WdtGetTimeOutStatus(void);
void AmbaRTSL_WdtClearTimeOutStatus(void);

void AmbaRTSL_WdtSetResetWidth(UINT8 Width);
int AmbaRTSL_WdtGetResetWidth(void);
void AmbaRTSL_WdtSetWdtMode(AMBA_WDT_MODE_e WdtMode);

int AmbaRTSL_WdtGetRebootSrc(void);

#endif  /* _AMBA_RTSL_WDT_H_ */
