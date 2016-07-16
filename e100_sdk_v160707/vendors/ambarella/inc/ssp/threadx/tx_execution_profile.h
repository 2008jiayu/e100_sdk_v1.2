/**************************************************************************/
/*                                                                        */
/*            Copyright (c) 1996-2010 by Express Logic Inc.               */
/*                                                                        */
/*  This software is copyrighted by and is the sole property of Express   */
/*  Logic, Inc.  All rights, title, ownership, or other interests         */
/*  in the software remain the property of Express Logic, Inc.  This      */
/*  software may only be used in accordance with the corresponding        */
/*  license agreement.  Any unauthorized use, duplication, transmission,  */
/*  distribution, or disclosure of this software is expressly forbidden.  */
/*                                                                        */
/*  This Copyright notice may not be removed or modified without prior    */
/*  written consent of Express Logic, Inc.                                */
/*                                                                        */
/*  Express Logic, Inc. reserves the right to modify this software        */
/*  without notice.                                                       */
/*                                                                        */
/*  Express Logic, Inc.                     info@expresslogic.com         */
/*  11423 West Bernardo Court               http://www.expresslogic.com   */
/*  San Diego, CA  92127                                                  */
/*                                                                        */
/**************************************************************************/


/**************************************************************************/
/**************************************************************************/
/**                                                                       */
/** ThreadX Component                                                     */
/**                                                                       */
/**   Execution Profile Kit                                               */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/

#ifndef TX_EXECUTION_PROFILE_H
#define TX_EXECUTION_PROFILE_H


/* The thread execution profile kit is designed to track thread execution time
   based on the hardware timer defined by TX_EXECUTION_TIME_SOURCE and
   TX_EXECUTION_MAX_TIME_SOURCE below. When the thread's total time reaches
   the maximum value, it remains there until the time is reset to 0 via a call
   to tx_thread_execution_time_reset. There are several assumptions to the
   operation of this kit, as follows:

   1. In tx_port.h replace:

        #define TX_THREAD_EXTENSION_3"

      with:

        #include "tx_execution_profile.h"

   2. The TX_EXECUTION_TIME_SOURCE and TX_EXECUTION_MAX_TIME_SOURCE macros are
      defined to utilize a local hardware time source.

   3. ThreadX 5.4 (or later) is being used, with the assembly code enabled to
      call the following routines from assembly code:

            VOID  _tx_execution_thread_enter(void);
            VOID  _tx_execution_thread_exit(void);
            VOID  _tx_execution_isr_enter(void);
            VOID  _tx_execution_isr_exit(void);

    4. The ThreadX library and application must be rebuilt with TX_ENABLE_EXECUTION_CHANGE_NOTIFY so
       that these macros are expanded in the TX_THREAD structure and so the assembly code macros
       are enabled to call the execution profile routines.

    5. Add tx_execution_profile.c to the application build.  */

/* Define the basic time typedef. This is different between 32-bit and 64-bit time accumulators. Don't forget
   to comment out the TX_THREAD_EXTENSION_3 found in tx_port.h.  */

#ifdef TX_EXECUTION_64BIT_TIME
typedef unsigned long long              EXECUTION_TIME;
#define TX_THREAD_EXTENSION_3           unsigned long long  tx_thread_execution_time_total; \
                                        unsigned long long  tx_thread_execution_time_last_start;
#define TX_EXECUTION_MAX_TIME_SOURCE    0xFFFFFFFFFFFFFFFF
#else
typedef unsigned long                   EXECUTION_TIME;
#define TX_THREAD_EXTENSION_3           unsigned long   tx_thread_execution_time_total; \
                                        unsigned long   tx_thread_execution_time_last_start;
#define TX_EXECUTION_MAX_TIME_SOURCE    0xFFFFFFFF
#endif


/* Define basic constants for the execution profile kit.  */

/*  Example for Cortex-M3 targets:
#define TX_EXECUTION_TIME_SOURCE         (EXECUTION_TIME) *((ULONG *) 0xE0001004)
*/

/*
#define TX_EXECUTION_TIME_SOURCE         (EXECUTION_TIME) 0
*/

#ifdef TX_ENABLE_EXECUTION_CHANGE_NOTIFY
#define AMBA_PROFILE
#endif

#ifdef AMBA_PROFILE
extern  ULONG     _tx_timer_system_clock;
#define TX_EXECUTION_TIME_SOURCE         (EXECUTION_TIME) _tx_timer_system_clock
#else
#define TX_EXECUTION_TIME_SOURCE         (EXECUTION_TIME) 0
#endif

/* Define APIs of the execution profile kit.  */
struct TX_THREAD_STRUCT;
VOID  _tx_execution_thread_enter(void);
VOID  _tx_execution_thread_exit(void);
VOID  _tx_execution_isr_enter(void);
VOID  _tx_execution_isr_exit(void);
UINT  _tx_execution_thread_time_reset(struct TX_THREAD_STRUCT *thread_ptr);
UINT  _tx_execution_thread_total_time_reset(void);
UINT  _tx_execution_isr_time_reset(void);
UINT  _tx_execution_idle_time_reset(void);
UINT  _tx_execution_thread_time_get(struct TX_THREAD_STRUCT *thread_ptr, EXECUTION_TIME *total_time);
UINT  _tx_execution_thread_total_time_get(EXECUTION_TIME *total_time);
UINT  _tx_execution_isr_time_get(EXECUTION_TIME *total_time);
UINT  _tx_execution_idle_time_get(EXECUTION_TIME *total_time);

#endif
