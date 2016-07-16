/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaRTSL_DRAMC.h
 *
 *  @Copyright      :: Copyright (C) 2012 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Definitions & Constants for DRAMC RTSL APIs
\*-------------------------------------------------------------------------------------------------------------------*/

#ifndef _AMBA_RTSL_DRAMC_H_
#define _AMBA_RTSL_DRAMC_H_

typedef enum _AMBA_DRAM_ARBITER_PRIORITY_e_ {
    AMBA_DRAM_ARBITER_PRIORITY0 = 0,    /* Low priority for dsp clients (75 of total bandwidth) */
    AMBA_DRAM_ARBITER_PRIORITY1,        /* Low priority for dsp clients (81.25% of total bandwidth) */
    AMBA_DRAM_ARBITER_PRIORITY2,        /* Normal priority for dsp clients (87.5% of total bandwidth) */
    AMBA_DRAM_ARBITER_PRIORITY3,        /* High priority for dsp clients (93.75% of total bandwidth - large arbiter throttle period) */
    AMBA_DRAM_ARBITER_PRIORITY4,        /* High priority for dsp clients (93.75% of total bandwidth) */
    AMBA_DRAM_ARBITER_PRIORITY5,        /* High priority for dsp clients (96.8% of total bandwidth) */
    AMBA_DRAM_ARBITER_PRIORITY6,        /* High priority for dsp clients (100% of total bandwidth) */

    AMBA_NUM_DRAM_ARBITER_PRIORITY
} AMBA_DRAM_ARBITER_PRIORITY_e;

/*-----------------------------------------------------------------------------------------------*\
 * Defined in AmbaRTSL_PLL.c
\*-----------------------------------------------------------------------------------------------*/
void AmbaRTSL_DramcInit(void);
int  AmbaRTSL_DramcSetArbiterPriority(AMBA_DRAM_ARBITER_PRIORITY_e Priority);

/*---------------------------------------------------------------------------*/
#define AmbaRTSL_DramcSetSelfRefresh()      AmbaCSL_DramcSetSelfRefresh

#endif /* _AMBA_RTSL_DRAMC_H_ */
