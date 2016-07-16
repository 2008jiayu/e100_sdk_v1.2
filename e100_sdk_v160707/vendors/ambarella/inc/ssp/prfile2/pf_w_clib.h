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
 [pf_w_clib.h] - Alternatives of Standard C Library functions
 ****************************************************************************/
#ifndef __PF_W_CLIB_H__
#define __PF_W_CLIB_H__

#include "prfile2/pf_clib.h"
#include "prfile2/pf_types.h"

/****************************************************************************
  CLIB-compatible Functions
 ****************************************************************************/
  PF_SIZE_T   pf_w_strlen(const PF_WCHAR* s);
  PF_WCHAR*   pf_w_strcpy(PF_WCHAR* dst, const PF_WCHAR* src);
  PF_WCHAR*   pf_w_strncpy(PF_WCHAR* dst, const PF_WCHAR* src, PF_SIZE_T length);
  PF_S_LONG   pf_w_strcmp(const PF_WCHAR* s1, const PF_WCHAR* s2);
  PF_S_LONG   pf_w_strncmp(const PF_WCHAR* s1, const PF_WCHAR* s2, PF_SIZE_T length);
  PF_WCHAR*   pf_w_strcat(PF_WCHAR* dst, const PF_WCHAR* src);
  PF_WCHAR*   pf_w_strchr(const PF_WCHAR *s, PF_WCHAR c);

#endif /* __PF_W_CLIB_H__ */
