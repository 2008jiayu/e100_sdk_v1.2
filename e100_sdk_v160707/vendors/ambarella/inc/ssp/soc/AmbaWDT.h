/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaWDT.h
 *
 *  @Copyright      :: Copyright (C) 2012 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Definitions & Constants for WDT Middleware APIs
\*-------------------------------------------------------------------------------------------------------------------*/

#ifndef _AMBA_WDT_H_
#define _AMBA_WDT_H_

#include "AmbaWDT_Def.h"

/*-----------------------------------------------------------------------------------------------*\
 * Defined in AmbaWDT.c
\*-----------------------------------------------------------------------------------------------*/
int AmbaWDT_Start(AMBA_WDT_CONFIG_s *pWdtConfig);
void AmbaWDT_Restart(void);
int AmbaWDT_Stop(void);
AMBA_WDT_MODE_e AmbaWDT_GetMode(void);
int AmbaWDT_GetTimeOutStatus(void);
void AmbaWDT_ClearTimeOutStatus(void);

#endif /* _AMBA_WDT_H_ */
