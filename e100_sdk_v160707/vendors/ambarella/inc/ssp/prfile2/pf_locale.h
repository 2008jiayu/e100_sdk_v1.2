/*
     Copyright (C) 2005 By eSOL Co.,Ltd. Tokyo, Japan
 
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
 [pf_locale.h] - Locale Module
 ****************************************************************************/
#ifndef __PF_LOCALE_H__
#define __PF_LOCALE_H__

#include "prfile2/pf_apicmn.h"


/****************************************************************************
  Character-code-specific definitions

    In this section, one header file specified by 'PF_CHARACTER_CODE_SET' 
    is included, which defines the following macros (See above in detail):

    - TO_WCS(c_lead, c_trail)

 ****************************************************************************/
/*---------------------------------------------------------------------------
  Character Code Set for pathname
 ----------------------------------------------------------------------------*/
/* ASCII (7bit SBCS) */
#define ASCII               (1)
/* CP932 -- Shift JIS (8bit MBCS) */
#define CP932               (2)
/* CP950  -- BIG_5     (8bit MBCS) */
#define CP950               (3)
/* CP936  -- GBK       (8bit MBCS) */
#define CP936               (4)
/* CP1252 -- LATIN_1   (8bit SBCS) */
#define CP1252              (5)
/* CP1250 -- LATIN_2   (8bit SBCS) */
#define CP1250              (6)
/* CP874  -- THAI      (8bit SBCS) */
#define CP874               (7)
/* CP949  -- KOREAN    (8bit MBCS) */
#define CP949               (8)
/* CP1251 -- CYRILLIC  (8bit SBCS) */
#define CP1251              (9)
/* CP1256 -- ARABIC    (8bit SBCS) */
#define CP1256              (10)

#if PF_CHARACTER_CODE_SET == CP932

  #include "pf_cp932.h"

  #define OEM2UNICODE       PFCODE_CP932_OEM2Unicode
  #define UNICODE2OEM       PFCODE_CP932_Unicode2OEM
  #define OEMCHARWIDTH      PFCODE_CP932_OEMCharWidth
  #define ISOEMMBCHAR       PFCODE_CP932_isOEMMBchar
  #define UNICODECHARWIDTH  PFCODE_CP932_UnicodeCharWidth
  #define ISUNICODEMBCHAR   PFCODE_CP932_isUnicodeMBchar

#elif PF_CHARACTER_CODE_SET == CP950
  #include "pf_cp950.h"

  #define OEM2UNICODE       PFCODE_CP950_OEM2Unicode
  #define UNICODE2OEM       PFCODE_CP950_Unicode2OEM
  #define OEMCHARWIDTH      PFCODE_CP950_OEMCharWidth
  #define ISOEMMBCHAR       PFCODE_CP950_isOEMMBchar
  #define UNICODECHARWIDTH  PFCODE_CP950_UnicodeCharWidth
  #define ISUNICODEMBCHAR   PFCODE_CP950_isUnicodeMBchar

#elif PF_CHARACTER_CODE_SET == CP936
  #include "pf_cp936.h"

  #define OEM2UNICODE       PFCODE_CP936_OEM2Unicode
  #define UNICODE2OEM       PFCODE_CP936_Unicode2OEM
  #define OEMCHARWIDTH      PFCODE_CP936_OEMCharWidth
  #define ISOEMMBCHAR       PFCODE_CP936_isOEMMBchar
  #define UNICODECHARWIDTH  PFCODE_CP936_UnicodeCharWidth
  #define ISUNICODEMBCHAR   PFCODE_CP936_isUnicodeMBchar

#elif PF_CHARACTER_CODE_SET == CP1252
  #include "pf_cp1252.h"

  #define OEM2UNICODE       PFCODE_CP1252_OEM2Unicode
  #define UNICODE2OEM       PFCODE_CP1252_Unicode2OEM
  #define OEMCHARWIDTH      PFCODE_CP1252_OEMCharWidth
  #define ISOEMMBCHAR       PFCODE_CP1252_isOEMMBchar
  #define UNICODECHARWIDTH  PFCODE_CP1252_UnicodeCharWidth
  #define ISUNICODEMBCHAR   PFCODE_CP1252_isUnicodeMBchar

#elif PF_CHARACTER_CODE_SET == CP1250
  #include "pf_cp1250.h"

  #define OEM2UNICODE       PFCODE_CP1250_OEM2Unicode
  #define UNICODE2OEM       PFCODE_CP1250_Unicode2OEM
  #define OEMCHARWIDTH      PFCODE_CP1250_OEMCharWidth
  #define ISOEMMBCHAR       PFCODE_CP1250_isOEMMBchar
  #define UNICODECHARWIDTH  PFCODE_CP1250_UnicodeCharWidth
  #define ISUNICODEMBCHAR   PFCODE_CP1250_isUnicodeMBchar

#elif PF_CHARACTER_CODE_SET == CP874
  #include "pf_cp874.h"

  #define OEM2UNICODE       PFCODE_CP874_OEM2Unicode
  #define UNICODE2OEM       PFCODE_CP874_Unicode2OEM
  #define OEMCHARWIDTH      PFCODE_CP874_OEMCharWidth
  #define ISOEMMBCHAR       PFCODE_CP874_isOEMMBchar
  #define UNICODECHARWIDTH  PFCODE_CP874_UnicodeCharWidth
  #define ISUNICODEMBCHAR   PFCODE_CP874_isUnicodeMBchar

#elif PF_CHARACTER_CODE_SET == CP949
  #include "pf_cp949.h"

  #define OEM2UNICODE       PFCODE_CP949_OEM2Unicode
  #define UNICODE2OEM       PFCODE_CP949_Unicode2OEM
  #define OEMCHARWIDTH      PFCODE_CP949_OEMCharWidth
  #define ISOEMMBCHAR       PFCODE_CP949_isOEMMBchar
  #define UNICODECHARWIDTH  PFCODE_CP949_UnicodeCharWidth
  #define ISUNICODEMBCHAR   PFCODE_CP949_isUnicodeMBchar

#elif PF_CHARACTER_CODE_SET == CP1251
  #include "pf_cp1251.h"

  #define OEM2UNICODE       PFCODE_CP1251_OEM2Unicode
  #define UNICODE2OEM       PFCODE_CP1251_Unicode2OEM
  #define OEMCHARWIDTH      PFCODE_CP1251_OEMCharWidth
  #define ISOEMMBCHAR       PFCODE_CP1251_isOEMMBchar
  #define UNICODECHARWIDTH  PFCODE_CP1251_UnicodeCharWidth
  #define ISUNICODEMBCHAR   PFCODE_CP1251_isUnicodeMBchar

#elif PF_CHARACTER_CODE_SET == CP1256
  #include "pf_cp1256.h"

  #define OEM2UNICODE       PFCODE_CP1256_OEM2Unicode
  #define UNICODE2OEM       PFCODE_CP1256_Unicode2OEM
  #define OEMCHARWIDTH      PFCODE_CP1256_OEMCharWidth
  #define ISOEMMBCHAR       PFCODE_CP1256_isOEMMBchar
  #define UNICODECHARWIDTH  PFCODE_CP1256_UnicodeCharWidth
  #define ISUNICODEMBCHAR   PFCODE_CP1256_isUnicodeMBchar

#elif PF_CHARACTER_CODE_SET == ASCII

  #include "pf_ascii.h"

  #define OEM2UNICODE       PFCODE_Ascii_OEM2Unicode
  #define UNICODE2OEM       PFCODE_Ascii_Unicode2OEM
  #define OEMCHARWIDTH      PFCODE_Ascii_OEMCharWidth
  #define ISOEMMBCHAR       PFCODE_Ascii_isOEMMBchar
  #define UNICODECHARWIDTH  PFCODE_Ascii_UnicodeCharWidth
  #define ISUNICODEMBCHAR   PFCODE_Ascii_isUnicodeMBchar

#else

  #error "Unsupported character code set"

#endif


/****************************************************************************
  Macro definitions
 ****************************************************************************/
/*---------------------------------------------------------------------------
  TO_WCS
 ----------------------------------------------------------------------------*/
#define TO_WCS(c1, c2)          ((PF_WCHAR)((((PF_U_CHAR)c1) << 8) + (PF_U_CHAR)c2))


/*---------------------------------------------------------------------------
  LOW_BYTE / HIGH_BYTE
 ----------------------------------------------------------------------------*/
#define LOW_BYTE(wc)            ((PF_U_CHAR)((wc) & 0xFF))
#define HIGH_BYTE(wc)           ((PF_U_CHAR)((wc) >> 8))


/****************************************************************************
  Prototypes
 ****************************************************************************/
/*---------------------------------------------------------------------------
  PFCODE_Combine_Width - Combine width of OEM and UNICODE.
 ----------------------------------------------------------------------------*/
PF_S_LONG PFCODE_Combine_Width(PF_S_SHORT oem_width, PF_S_SHORT uni_width);


/*---------------------------------------------------------------------------
  PFCODE_Divide_Width - Divide width to OEM and UNICODE.
 ----------------------------------------------------------------------------*/
void PFCODE_Divide_Width(PF_S_LONG width, PF_S_SHORT* oem_width, PF_S_SHORT* uni_width);


/****************************************************************************
  Alternative forms of get character width functions
 ****************************************************************************/
#define PF_COMBINE_WIDTH(a,b)     PFCODE_Combine_Width(a,b)
#define PF_DIVIDE_WIDTH(a,b,c)    PFCODE_Divide_Width(a,b,c)


#endif  /* __PF_LOCALE_H__ */
