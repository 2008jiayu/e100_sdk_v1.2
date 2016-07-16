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
 [pf_clib.h] - Alternatives of Standard C Library functions
 ****************************************************************************/
#ifndef __PF_CLIB_H__
#define __PF_CLIB_H__

#include "prfile2/pf_types.h"

/****************************************************************************
  Configuration (User Depand)
 ****************************************************************************/
  /*
   * If set, C Standard Library functions provided with compiler are used;
   * Otherwise alternative functions defined in 'pf_clib.h' are used instead.
   * Default : 0 (Fixed)
   *
   * NOTES:
   *  - Any function of FrFILE2 (including drivers) shall use the
   *    functions defined in 'pf_clib.h' (ex. pf_memcpy(), pf_strlen(), ...),
   *    not C Standard Library functions (memcpy(), strlen(), ...) directly.
   *  - This is equivalent of 'USE_CLIB_AS_UNDERLYING_LAYER' for PrFILE.
   */
  #define PF_USE_CLIB                       0


/****************************************************************************
  CLIB-compatible Functions
 ****************************************************************************/
#if PF_USE_CLIB

  #include <ctype.h>
  #include <string.h>

  #define pf_toupper(c)                     toupper(c)
  #define pf_memcpy(dst,src,length)         memcpy(dst,src,length)
  #define pf_memset(dst,c,len)              memset(dst,c,len)
  #define pf_memcmp(s1,s2,length)           memcmp(s1,s2,length)
  #define pf_strlen(s)                      strlen(s)
  #define pf_strnlen(s,maxlen)              strnlen(s,maxlen)
  #define pf_strcpy(dst,src)                strcpy(dst,src)
  #define pf_strncpy(dst,src,length)        strncpy(dst,src,length)
  #define pf_strchr(s,c)                    strchr(s,c)
  #define pf_strcat(dst,src)                strcat(dst,src)
  #define pf_strcmp(s1,s2)                  strcmp(s1,s2)
  #define pf_strncmp(s1,s2,length)          strncmp(s1,s2,length)

#else /* ! PF_USE_CLIB */

  PF_S_LONG   pf_toupper(PF_S_LONG c);
  void*       pf_memcpy(void* dst, const void* src, PF_SIZE_T length);
  void*       pf_memset(void* dst, PF_S_LONG c, PF_SIZE_T len);
  PF_S_LONG   pf_memcmp(const void* s1, const void* s2, PF_SIZE_T length);
  PF_SIZE_T   pf_strlen(const PF_CHAR* s);
  PF_SIZE_T   pf_strnlen(const PF_CHAR *s, PF_SIZE_T maxlen);
  PF_CHAR*    pf_strcpy(PF_CHAR* dst, const PF_CHAR* src);
  PF_CHAR*    pf_strncpy(PF_CHAR* dst, const PF_CHAR* src, PF_SIZE_T length);
  PF_CHAR*    pf_strchr(const PF_CHAR* s, PF_S_LONG c);
  PF_CHAR*    pf_strcat(PF_CHAR* dst, const PF_CHAR* src);
  PF_S_LONG   pf_strcmp(const PF_CHAR* s1, const PF_CHAR* s2);
  PF_S_LONG   pf_strncmp(const PF_CHAR* s1, const PF_CHAR* s2, PF_SIZE_T length);

#endif /* PF_USE_CLIB */

#endif /* __PF_CLIB_H__ */
