/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaRTSL_VIC.h
 *
 *  @Copyright      :: Copyright (C) 2014 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Definitions & Constants for Vector Interrupt Controller (VIC) RTSL APIs
\*-------------------------------------------------------------------------------------------------------------------*/

#ifndef _AMBA_RTSL_VIC_H_
#define _AMBA_RTSL_VIC_H_

#include "AmbaINT_Def.h"

typedef struct _AMBA_VIC_INFO_s_ {
    AMBA_VIC_INT_CONFIG_e   Config;
    UINT32                  IntEnable;
} AMBA_VIC_INFO_s;

/*-----------------------------------------------------------------------------------------------*\
 * Defined in AmbaRTSL_VIC.c
\*-----------------------------------------------------------------------------------------------*/
extern AMBA_VIC_ISR_f AmbaVIC_IsrTable[];

extern void (*_AmbaPreIsrCallBack)(AMBA_VIC_INT_ID_e IntID);
extern void (*_AmbaPostIsrCallBack)(AMBA_VIC_INT_ID_e IntID);

#define AmbaPreIsrRegister(pISR)    _AmbaPreIsrCallBack = (pISR)
#define AmbaPostIsrRegister(pISR)   _AmbaPostIsrCallBack = (pISR)

int  AmbaRTSL_VicInit(void);
void AmbaRTSL_VicIntGlobalEnable(void);
void AmbaRTSL_VicIntGlobalDisable(void);

int  AmbaRTSL_VicIsrHook(AMBA_VIC_INT_ID_e IntID, UINT32 Priority,
                         AMBA_VIC_INT_CONFIG_e IntConfig,
                         AMBA_VIC_ISR_f IsrEntry);
int  AmbaRTSL_VicIntEnable(AMBA_VIC_INT_ID_e IntID);
int  AmbaRTSL_VicIntDisable(AMBA_VIC_INT_ID_e IntID);
int  AmbaRTSL_VicIntSetPriority(AMBA_VIC_INT_ID_e IntID, UINT32 Priority);

//__irq __arm void AmbaRTSL_VicIsrIRQ(void);
void AmbaRTSL_VicIsrIRQ(void) __attribute__((interrupt("IRQ")));
void AmbaRTSL_VicIsrFIQ(void) __attribute__((interrupt("FIQ")));
void AmbaRTSL_VicIntHandler(void);
UINT32 AmbaRTSL_VicGetIntID(void);
UINT32 AmbaRTSL_VicGetIntInfo(AMBA_VIC_INT_ID_e IntID, AMBA_VIC_INFO_s *pIntInfo);

#endif /* _AMBA_RTSL_VIC_H_ */
