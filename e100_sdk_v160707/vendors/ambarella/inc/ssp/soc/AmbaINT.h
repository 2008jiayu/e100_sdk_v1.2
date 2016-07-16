/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaINT.h
 *
 *  @Copyright      :: Copyright (C) 2014 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Definitions & Constants for Interrupt Controller (VIC) APIs
\*-------------------------------------------------------------------------------------------------------------------*/

#ifndef _AMBA_INT_H_
#define _AMBA_INT_H_

#include "AmbaINT_Def.h"

/*-----------------------------------------------------------------------------------------------*\
 * Defined in AmbaINT.c
\*-----------------------------------------------------------------------------------------------*/
int AmbaINT_Init(void);
int AmbaINT_IntGlobalEnable(void);
int AmbaINT_IntGlobalDisable(void);

int AmbaINT_IsrHook(AMBA_VIC_INT_ID_e IntID, UINT32 Priority,
                    AMBA_VIC_INT_CONFIG_e IntConfig,
                    AMBA_VIC_ISR_f IsrEntry);
int AmbaINT_IntEnable(AMBA_VIC_INT_ID_e IntID);
int AmbaINT_IntDisable(AMBA_VIC_INT_ID_e IntID);
int AmbaINT_IntSetPriority(AMBA_VIC_INT_ID_e IntID, UINT32 Priority);

#endif /* _AMBA_INT_H_ */
