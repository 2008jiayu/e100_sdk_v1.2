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
 [pf_str.h] - Substring Module

  NOTES:
   - No need to include this header for User Applications; This is used
     only for compiling PrFILE2 itself.
 ****************************************************************************/
#ifndef __PF_STR_H__
#define __PF_STR_H__

#include "prfile2/pf_types.h"


/****************************************************************************
  Macro Definitions
 ****************************************************************************/
/*---------------------------------------------------------------------------
  Code mode
 ----------------------------------------------------------------------------*/
#define PF_CODEMODE_LOCAL       (1)
#define PF_CODEMODE_UNICODE     (2)

/*---------------------------------------------------------------------------
  Select string
 ----------------------------------------------------------------------------*/
#define PF_TARGET_STR_HEAD      (1)
#define PF_TARGET_STR_TAIL      (2)


/****************************************************************************
  Type definitions
 ****************************************************************************/
/*---------------------------------------------------------------------------
  PF_STR - Substring which indicates a part of string.
 ----------------------------------------------------------------------------*/
typedef struct PF_STR
{
    const PF_CHAR*    p_head;       /* points to the first character in the
                                       substring for Unicode character. */
    const PF_CHAR*    p_tail;       /* points to the the character next to
                                       the last character in the substring. */
    PF_U_LONG         code_mode;    /* Current code mode. (Local or Unicode) */
} PF_STR;


/****************************************************************************
  Prototypes
 ****************************************************************************/
/*---------------------------------------------------------------------------
  PFSTR_SetCodeMode -
 ----------------------------------------------------------------------------*/
void  PFSTR_SetCodeMode(PF_STR* p_str, PF_U_LONG code_mode);


/*---------------------------------------------------------------------------
  PFSTR_GetCodeMode -
 ----------------------------------------------------------------------------*/
PF_U_LONG  PFSTR_GetCodeMode(PF_CONST PF_STR* p_str);


/*---------------------------------------------------------------------------
  PFSTR_GetStrPos -
 ----------------------------------------------------------------------------*/
PF_CHAR  *PFSTR_GetStrPos(PF_CONST PF_STR *p_str, PF_U_LONG target);


/*---------------------------------------------------------------------------
  PFSTR_MoveStrPos -
 ----------------------------------------------------------------------------*/
void  PFSTR_MoveStrPos(PF_STR* p_str, PF_S_SHORT num_char);


/*---------------------------------------------------------------------------
  PFSTR_InitStr - Initialize PF_STR substring with null-terminated string.
 ----------------------------------------------------------------------------*/
PF_ERROR  PFSTR_InitStr(PF_STR* p_str, PF_CONST PF_CHAR* s, PF_U_LONG code_mode);


/*---------------------------------------------------------------------------
  PFSTR_StrLen - Get length of PF_STR substring.
 ----------------------------------------------------------------------------*/
PF_U_SHORT  PFSTR_StrLen(PF_CONST PF_STR *p_str);


/*---------------------------------------------------------------------------
  PFSTR_StrNumChar - Get number of character in PF_STR substring.
 ----------------------------------------------------------------------------*/
PF_U_SHORT  PFSTR_StrNumChar(PF_CONST PF_STR *p_str, PF_U_LONG target);


/*---------------------------------------------------------------------------
  PFSTR_StrCmp - Compare PF_STR substring with null-terminated string.
 ----------------------------------------------------------------------------*/
PF_S_LONG  PFSTR_StrCmp(const PF_STR* p_str, const PF_CHAR* s);


/*---------------------------------------------------------------------------
  PFSTR_WStrCmp - Compare PF_STR substring with null-terminated w-string.
 ----------------------------------------------------------------------------*/
PF_S_LONG  PFSTR_WStrCmp(const PF_STR* p_str, const PF_WCHAR* ws);


/*---------------------------------------------------------------------------
  PFSTR_StrNCmp - Compare PF_STR substring with null-terminated string.
 ----------------------------------------------------------------------------*/
PF_S_LONG  PFSTR_StrNCmp(PF_CONST PF_STR *p_str, PF_CONST PF_CHAR *s, PF_U_LONG target, PF_S_SHORT offset, PF_U_SHORT num);


/*---------------------------------------------------------------------------
  PFSTR_ToUpperNStr - Copy PF_STR substrings to specify num string buffer,
                      with converting lower-case letters into upper-case.
 ----------------------------------------------------------------------------*/
void  PFSTR_ToUpperNStr(PF_CONST PF_STR *p_src, PF_U_SHORT num, PF_CHAR *dest);


/*---------------------------------------------------------------------------
  PFSTR_IsStrNull - Check whether PF_STR substring is empty.
 ----------------------------------------------------------------------------*/
#define PFSTR_IsStrNull(p_str)      ((p_str)->p_head == NULL)


#endif  /* __PF_STR_H__ */
