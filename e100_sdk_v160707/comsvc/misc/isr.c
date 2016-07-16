 /**
  * @file src/comsvc/isr.c
  *
  * ISR hook service
  *
  * History:
  *    2013/03/02/ - [Jenghung Luo] created file
  *
  * Copyright (C) 2013, Ambarella, Inc.
  *
  * All rights reserved. No Part of this file may be reproduced, stored
  * in a retrieval system, or transmitted, in any form, or by any means,
  * electronic, mechanical, photocopying, recording, or otherwise,
  * without the prior consent of Ambarella, Inc.
  */
#include <stdio.h>
#include <string.h>
#include <AmbaDataType.h>
#include <AmbaKAL.h>
#include <AmbaINT.h>
#include <AmbaPrintk.h>
//#include <isr.h>

#define __ISR_DEBUG__

#ifdef __ISR_DEBUG__
#define isr_print    AmbaPrint
#else
#define isr_print(...)
#endif

#ifdef __GNUC__
#undef  __packed
#define __packed
#define __PACKED __attribute__((packed))
#else
#define __PACKED
#endif

#define AMP_MAX_ISR_HANDLER  4

typedef struct AMP_ISR_ARRAY_s_ {
    UINT8 enable[AMP_MAX_ISR_HANDLER];
    void (*handler[AMP_MAX_ISR_HANDLER])(int IrqNo);
}  __PACKED AMP_ISR_ARRAY_s;

static AMP_ISR_ARRAY_s __attribute__((aligned(32))) isr[AMBA_NUM_VIC_INTERRUPT] = {{{0}}};

/**
 * Isr ErrorLogging
 *
 * @param [in] Level level
 * @param [in] ErrCode Error code
 * @param [in] Str string to print
 *
 * @return none
 */
static void Isr_ErrorLog(int Level, int ErrCode, const char *Str)
{
    AmbaPrint("[%d] ISR #%d %s: %s", Level, ErrCode, Level?"WARN":"ERR", Str);
}

#define isr_perror(level, errcode, str) {Isr_ErrorLog(level, errcode, str);}



/**
 * IRQ checker. Currently for A9 only
 *
 * @param [in] IrqNo IRQ number
 *
 * @return OK - success, NG - fail
 */
static inline int AmpIsr_ValidCheck(int IrqNo)
{
    int rval = NG;

    if(IrqNo < AMBA_NUM_VIC_INTERRUPT) {
        switch(IrqNo) {
        case 0xFFFFFFFF:
                AmbaPrint("SSP occupied irq %d",IrqNo);
                break;
        default:
                rval = OK;
                break;
        }
    }

    return rval;

}

/**
 * ISR main dispatcher
 *
 * @return none
 */
static void AmpIsr_Main(int IrqNo)
{
    register int i;

    for(i = 0; i < AMP_MAX_ISR_HANDLER; i++) {
        if((isr[IrqNo].enable[i]) && (isr[IrqNo].handler[i] != NULL)) {
            isr[IrqNo].handler[i](IrqNo);
        }
    }

}

/**
 * ISR hook
 *
 * @param [in] IrqNo IRQ number
 * @param [in] Func ISR
 *
 * @return 0 - success, -1 - fail
 */
int AmpIsr_Hook(UINT32 IrqNo, void (*Func)(int IrqNo))
{
    int rval = -1;
    int i;

    if(AmpIsr_ValidCheck(IrqNo) != OK) {
        goto _DONE;
    }

    // Disable IRQ first
    AmbaINT_IntDisable((AMBA_VIC_INT_ID_e)IrqNo);

    // Check ISR pool
    for(i = 0; i < AMP_MAX_ISR_HANDLER; i++) {
        if(isr[IrqNo].handler[i] == NULL) {
            break;
        }
    }

    if(i == AMP_MAX_ISR_HANDLER) {
        isr_perror(0, 0, "pool full");
        // Enable IRQ
        AmbaINT_IntEnable((AMBA_VIC_INT_ID_e)IrqNo);
        goto _DONE;
    } else if(i == 0) {
        rval = AmbaINT_IsrHook((AMBA_VIC_INT_ID_e)IrqNo, 0, AMBA_VIC_INT_RISING_EDGE_TRIGGER, AmpIsr_Main);  // Hook ISR
        if (rval != 0) {
            AmbaPrint("irq %d hooked error",IrqNo);
            goto _DONE;
        }
    }

    // Add func to the pool
    isr[IrqNo].handler[i] = Func;

    // Enable IRQ
    AmbaINT_IntEnable((AMBA_VIC_INT_ID_e)IrqNo);

    rval = 0;

_DONE:

    return rval;
}


/**
 * ISR Control
 *
 * @param [in] IrqNo IRQ number
 * @param [in] Enable Enable or disable
 * @param [in] Func ISR
 *
 * @return 0 - success 1 - fail
 */
int AmpIsr_Control(UINT32 IrqNo, UINT32 Enable, void (*Func)(int IrqNo))
{
    int rval = -1;
    int i;

    if(AmpIsr_ValidCheck(IrqNo) != OK) {
        goto _DONE;
    }

    // Disable IRQ first
    AmbaINT_IntDisable((AMBA_VIC_INT_ID_e)IrqNo);

    // Check ISR pool
    for(i = 0; i < AMP_MAX_ISR_HANDLER; i++) {
        if(isr[IrqNo].handler[i] == Func) {
            break;
        }
    }

    if(i == AMP_MAX_ISR_HANDLER) {
        isr_perror(0, 0, "find isr");
         // Enable IRQ
        AmbaINT_IntEnable((AMBA_VIC_INT_ID_e)IrqNo);
       goto _DONE;
    }

    // Enable/Disable
    isr[IrqNo].enable[i] = Enable;

    // Enable IRQ
    AmbaINT_IntEnable((AMBA_VIC_INT_ID_e)IrqNo);

    rval = 0;

_DONE:

    return rval;
}

