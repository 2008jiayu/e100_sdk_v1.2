/*
     Copyright (C) 2009 By eSOL Co.,Ltd. Tokyo, Japan

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
 [pf_exfat_api.h] - Declarations of PrFILE2 API functions
 ****************************************************************************/
#ifndef __PF_EXFAT_API_H__
#define __PF_EXFAT_API_H__

#include "prfile2/pf_exfat_apicmn.h"

#if PF_EXFAT_SUPPORT
/****************************************************************************
  Alternative forms of PrFILE2 API functions
 ****************************************************************************/
#if (PF_STDLIB_ALIAS_SUPPORT && !PF_USE_API_STUB)
/* Support alias in stdlib.h and not support API stub */

  #define _setupcasebuf(a,b,c)    pf2_setupcasebuf(a,b,c)

#elif (PF_STDLIB_ALIAS_SUPPORT && PF_USE_API_STUB)

/* Support alias in stdlib.h and support API stub */

  #define _setupcasebuf(a,b,c)    pfstub_setupcasebuf(a,b,c)

#endif  /* PF_STDLIB_ALIAS_SUPPORT && !PF_USE_API_STUB */

#if !PF_USE_API_STUB

/* Not support API stub */

  #define pf_setupcasebuf(a,b,c)  pf2_setupcasebuf(a,b,c)

#else

/* Support API stub */

  #define pf_setupcasebuf(a,b,c)  pfstub_setupcasebuf(a,b,c) 

#endif  /* !PF_USE_API_STUB */

#endif  /* PF_EXFAT_SUPPORT */
#endif  /* __PF_EXFAT_API_H__ */
