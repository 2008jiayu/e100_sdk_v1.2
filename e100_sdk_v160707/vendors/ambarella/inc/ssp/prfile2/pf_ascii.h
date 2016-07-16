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
 [pf_ascii.h] - ASCII Code Module

    This module is for single byte character sets (SBCSs), including
    ASCII.
 ****************************************************************************/
#ifndef __PF_ASCII_H__
#define __PF_ASCII_H__

#include "pf_common.h"

/****************************************************************************
  Macro definitions
 ****************************************************************************/
#define ASCII_ASCII_GETA_MARK           0x5f    /* Character for unknown */
#define ASCII_UNICODE_GETA_MARK         0x005f  /* Character for unknown */

/****************************************************************************
  Prototypes
 ****************************************************************************/
/*---------------------------------------------------------------------------
  PFCODE_Ascii_OEM2Unicode - Change Ascii to Unicode(UCS-2)
 ----------------------------------------------------------------------------*/
PF_S_LONG PFCODE_Ascii_OEM2Unicode(const PF_CHAR* ascii_src, PF_WCHAR* uc_dst);

/*---------------------------------------------------------------------------
  PFCODE_Ascii_Unicode2OEM - Change Unicode(UCS-2) to Ascii
 ----------------------------------------------------------------------------*/
PF_S_LONG PFCODE_Ascii_Unicode2OEM(const PF_WCHAR* uc_src, PF_CHAR* ascii_dst);

/*---------------------------------------------------------------------------
  PFCODE_Ascii_OEMCharWidth - Get the width of Ascii charactor
 ----------------------------------------------------------------------------*/
PF_S_LONG PFCODE_Ascii_OEMCharWidth(const PF_CHAR *buf);

/*---------------------------------------------------------------------------
  PFCODE_Ascii_isOEMMBchar - Check multiple-byte character
 ----------------------------------------------------------------------------*/
PF_BOOL PFCODE_Ascii_isOEMMBchar(const PF_CHAR ascii, PF_U_LONG num);

/*---------------------------------------------------------------------------
  PFCODE_Ascii_UnicodeCharWidth - Get the width of Unicode charactor
 ----------------------------------------------------------------------------*/
PF_S_LONG PFCODE_Ascii_UnicodeCharWidth(const PF_WCHAR *buf);

/*---------------------------------------------------------------------------
  PFCODE_Ascii_isUnicodeMBchar - Check multiple-code character
 ----------------------------------------------------------------------------*/
PF_BOOL PFCODE_Ascii_isUnicodeMBchar(const PF_WCHAR uc_src, PF_U_LONG num);

#endif  /* __PF_ASCII_H__ */
