/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaWDT_Def.h
 *
 *  @Copyright      :: Copyright (C) 2012 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Common Definitions & Constants for WDT APIs
\*-------------------------------------------------------------------------------------------------------------------*/

#ifndef _AMBA_WDT_DEF_H_
#define _AMBA_WDT_DEF_H_

typedef enum _AMBA_WDT_MODE_e_ {
    AMBA_WDT_MODE_DISABLED = 0,         /* WDT is disabled */
    AMBA_WDT_MODE_SYS_RESET,            /* Generate a system reset signal */
    AMBA_WDT_MODE_IRQ,                  /* Generate an interrupt to the interrupt controller */

    AMBA_NUM_WDT_MODE
} AMBA_WDT_MODE_e;

typedef struct _AMBA_WDT_CONFIG_s_ {
    AMBA_WDT_MODE_e Mode;               /* WDT operation mode */
    UINT8   ResetIrqPulseWidth;         /* The Pulse Width of Reset and Interrupt */
    UINT32  CounterValue;               /* WDT Initial counter value (in ms) */

    void    (*TimeoutISR)(int IntID);   /* WDT Timeout (underflow) ISR for Interrupt mode */
} AMBA_WDT_CONFIG_s;

#endif  /* _AMBA_WDT_DEF_H_ */
