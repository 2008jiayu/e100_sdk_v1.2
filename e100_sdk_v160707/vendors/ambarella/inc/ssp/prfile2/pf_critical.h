/*
     Copyright (C) 2004 By eSOL Co.,Ltd. Tokyo, Japan
 
     This software is protected by the law and the agreement concerning
     a Japanese country copyright method, an international agreement,
     and other intellectual property right and may be used and copied
     only in accordance with the terms of such license and with the inclusion
     of the above copyright notice.
 
     This software or any other copies thereof may not be provided
     or otherwise made available to any other person.  No title to
     and ownership of the software is hereby transferred.
 
     The information in this software is subject to change without
     notice and should not be construed as a commitment by eSOL Co.,Ltd.
 */
 /****************************************************************************
 [pf_critical.h] - 
 ****************************************************************************/
#ifndef     __PF_CRITICAL_H__
#define     __PF_CRITICAL_H__

#include "pf_common.h"

#if PF_CRITICAL_SECTION_ENABLE

/*---------------------------------------------------------------------------
  Structure to control the critical section
 ----------------------------------------------------------------------------*/
/* You should modify the typedef of this structure (PF_CRITICAL) */
typedef struct PF_CRITICAL
{
    PF_U_LONG   state;             /* Status                        */
    PF_S_LONG   LockCount;         /* Lock count                    */
    PF_S_LONG   Resource;          /* Resource for critical section */
    PF_S_LONG   owner;             /* Owner context ID              */
} PF_CRITICAL;

/*---------------------------------------------------------------------------
  Prototypes
 ----------------------------------------------------------------------------*/
void PF_initCritical(PF_CRITICAL* pIdCrit);
void PF_deleteCritical(PF_CRITICAL* pIdCrit);
void PF_enterCritical(PF_CRITICAL* pIdCrit);
void PF_exitCritical(PF_CRITICAL* pIdCrit);

/* You must not modify these macro name:    */
/*          INIT_CRITICAL(pIdCrit)          */
/*          DELETE_CRITICAL(pIdCrit)        */
/*          ENTER_VOLUME_CRITICAL(p_vol)    */
/*          EXIT_VOLUME_CRITICAL(p_vol)     */
/*          ENTER_VOLUME_SET_CRITICAL()     */
/*          EXIT_VOLUME_SET_CRITICAL()      */

#define INIT_CRITICAL(pIdCrit)          PF_initCritical(pIdCrit)
#define DELETE_CRITICAL(pIdCrit)        PF_deleteCritical(pIdCrit)

#define ENTER_VOLUME_CRITICAL(p_vol)    PF_enterCritical(&(p_vol)->critical)
#define EXIT_VOLUME_CRITICAL(p_vol)     PF_exitCritical(&(p_vol)->critical)

#define ENTER_VOLUME_SET_CRITICAL()     PF_enterCritical(&(CURRENT_VOL_SET()->critical))
#define EXIT_VOLUME_SET_CRITICAL()      PF_exitCritical(&(CURRENT_VOL_SET()->critical))

#else /* ! PF_CRITICAL_SECTION_ENABLE */

/* You must not modify these macro name:    */
/*          INIT_CRITICAL(pIdCrit)          */
/*          DELETE_CRITICAL(pIdCrit)        */
/*          ENTER_VOLUME_CRITICAL(p_vol)    */
/*          EXIT_VOLUME_CRITICAL(p_vol)     */
/*          ENTER_VOLUME_SET_CRITICAL()     */
/*          EXIT_VOLUME_SET_CRITICAL()      */

#define INIT_CRITICAL(pIdCrit)          do{;}while(0)
#define DELETE_CRITICAL(pIdCrit)        do{;}while(0)

#define ENTER_VOLUME_CRITICAL(p_vol)    do{;}while(0)
#define EXIT_VOLUME_CRITICAL(p_vol)     do{;}while(0)

#define ENTER_VOLUME_SET_CRITICAL()     do{;}while(0)
#define EXIT_VOLUME_SET_CRITICAL()      do{;}while(0)

#endif  /* PF_CRITICAL_SECTION_ENABLE  */

#endif /* __PF_CRITICAL_H__ */
