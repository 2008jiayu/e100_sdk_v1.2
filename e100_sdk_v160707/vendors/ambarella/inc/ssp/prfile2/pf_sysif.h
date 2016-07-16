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
 [pf_system.h] - System-dependent functions declarations

  NOTES:
   - All of functions declared in this file are system-dependent functions
     which PrFILE2's user must implement.

 ****************************************************************************/
#ifndef __PF_SYSIF_H__
#define __PF_SYSIF_H__

#include "prfile2/pf_types.h"

/****************************************************************************
  Prototypes
 ****************************************************************************/
/*---------------------------------------------------------------------------
 PFSYS_TimeStamp - transliterate lower-case characters to upper-case
 ----------------------------------------------------------------------------*/
void PFSYS_TimeStamp(PF_SYS_DATE* sdate, PF_SYS_TIME* stime);

#endif  /* __PF_SYSIF_H__ */
