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
 [pf_path.h] -
 ****************************************************************************/
#ifndef __PF_PATH_H__
#define __PF_PATH_H__

#include "prfile2/pf_types.h"
#include "pf_str.h"

/****************************************************************************
  Macro Definitions
 ****************************************************************************/
/**
 * Return value on name match.
 */
#define MATCH               (0)
/**
 * Return value on no match.
 */
#define NOMATCH             (1)

/* initial path length(drive letter, colonge, separator) */
#define INIT_PATH_LENGTH    (3)

#define MIN_LEN_TAIL_SFN    (3)

/****************************************************************************
  Prototypes
 ****************************************************************************/
/*---------------------------------------------------------------------------
  PFPATH_MatchFileNameWithPattern - Check whether short or long name matches
                                    pattern.
 ----------------------------------------------------------------------------*/
PF_BOOL  PFPATH_MatchFileNameWithPattern(PF_CONST PF_CHAR *file_name,
                                         PF_CONST PF_STR  *p_pattern,
                                         PF_BOOL          is_short_search,
                                         PF_BOOL          is_long_name);

/*---------------------------------------------------------------------------
  PFPATH_InitTokenOfPath -
 ----------------------------------------------------------------------------*/
void  PFPATH_InitTokenOfPath(PF_STR* p_str, PF_CHAR* path, PF_U_LONG code_mode);

/*---------------------------------------------------------------------------
  PFPATH_GetNextTokenOfPath -
 ----------------------------------------------------------------------------*/
PF_ERROR  PFPATH_GetNextTokenOfPath(PF_STR* p_str, PF_BOOL wildcard);

/*---------------------------------------------------------------------------
  PFPATH_SplitPath - Split path into directory path and filename
 ----------------------------------------------------------------------------*/
PF_ERROR  PFPATH_SplitPath(PF_CONST PF_STR *p_path, PF_STR *p_dir_path, PF_STR *p_filename);

/*---------------------------------------------------------------------------
  PFPATH_SplitPathPattern - Split path into directory path and pattern
 ----------------------------------------------------------------------------*/
PF_ERROR  PFPATH_SplitPathPattern(PF_CONST PF_STR *p_path, PF_STR *p_dir_path, PF_STR *p_pattern);

/*---------------------------------------------------------------------------
  PFPATH_GetVolumeFromPath - Get PF_VOLUME structure corresponding to the
                             specified path.
 ----------------------------------------------------------------------------*/
PF_VOLUME  *PFPATH_GetVolumeFromPath(PF_CONST PF_STR *p_path);

/*---------------------------------------------------------------------------
  PFPATH_MatchShortNameWithPattern - Check whether short name matches pattern.
 ----------------------------------------------------------------------------*/
#define PFPATH_MatchShortNameWithPattern(short_name, p_pattern, is_short_search) \
        PFPATH_MatchFileNameWithPattern(short_name, p_pattern, is_short_search, PF_FALSE)

/*---------------------------------------------------------------------------
  PFPATH_MatchLongNameWithPattern - Check whether long name matches pattern.
 ----------------------------------------------------------------------------*/
#define PFPATH_MatchLongNameWithPattern(long_name, p_pattern) \
        PFPATH_MatchFileNameWithPattern(long_name, p_pattern, PF_FALSE, PF_TRUE)

/*---------------------------------------------------------------------------
 PFPATH_MatchUniFileNameWithPattern - Check whether filename matches pattern.
 ----------------------------------------------------------------------------*/
PF_BOOL  PFPATH_MatchUniFileNameWithPattern(PF_CONST PF_WCHAR *p_file_name,
                                            PF_CONST PF_WCHAR *p_pattern);

/*---------------------------------------------------------------------------
 PFPATH_putShortName - Store the short name from a regular string into a FAT
                       direntry.
 ----------------------------------------------------------------------------*/
PF_ERROR PFPATH_putShortName(PF_U_CHAR* pDirEntry, const PF_CHAR* short_name, PF_U_CHAR attr);

/*---------------------------------------------------------------------------
 PFPATH_getShortName - Load the short name from a FAT direntry into a regular
                       string.
 ----------------------------------------------------------------------------*/
PF_ERROR PFPATH_getShortName(PF_CHAR* short_name, const PF_U_CHAR* pDirEntry, PF_U_CHAR attr);

/*---------------------------------------------------------------------------
 PFPATH_getLongNameformShortName - Load a long name from a short file name
                                   with NT reserved flag.
 ----------------------------------------------------------------------------*/
void PFPATH_getLongNameformShortName(PF_CHAR* short_name, PF_CHAR* long_name, PF_U_CHAR flag);

#if 0 /* Not use */
/*---------------------------------------------------------------------------
 PFPATH_GetLengthFromShortname-
 ----------------------------------------------------------------------------*/
PF_U_LONG PFPATH_GetLengthFromShortname(const PF_CHAR* sSrc);
#endif /* Not use */

/*---------------------------------------------------------------------------
 PFPATH_GetLengthFromPathName-
 ----------------------------------------------------------------------------*/
PF_U_LONG PFPATH_GetLengthFromPathName(const PF_CHAR* sSrc);

#if 0 /* Not use */
/*---------------------------------------------------------------------------
 PFPATH_GetLengthFromUnicode -
 ----------------------------------------------------------------------------*/
PF_U_LONG PFPATH_GetLengthFromUnicode(const PF_WCHAR* sSrc);
#endif /* Not use */

/*---------------------------------------------------------------------------
 PFPATH_transformFromUnicodeToNormal -
 ----------------------------------------------------------------------------*/
PF_S_LONG PFPATH_transformFromUnicodeToNormal(PF_CHAR* sDest, const PF_WCHAR* sSrc);

/*---------------------------------------------------------------------------
 PFPATH_transformInUnicode -
 ----------------------------------------------------------------------------*/
PF_S_LONG PFPATH_transformInUnicode(PF_WCHAR* sDestStr, const PF_CHAR* sSrcStr);

#if PF_EXFAT_SUPPORT
/*---------------------------------------------------------------------------
 PFPATH_transformInUnicodeFromSTR -
 ----------------------------------------------------------------------------*/
PF_S_LONG PFPATH_transformInUnicodeFromSTR(PF_WCHAR* sDestStr, const PF_STR* p_pattern);
#endif /* #if PF_EXFAT_SUPPORT */

/*---------------------------------------------------------------------------
 PFPATH_parseShortName - this function extracting short name from a string
                         and return TRUE if it's need of long name else
                         return FALSE.
 ----------------------------------------------------------------------------*/
PF_BOOL PFPATH_parseShortName(PF_CHAR* pDest, PF_STR* p_pattern);

/*---------------------------------------------------------------------------
  PFPATH_get_tilde_of_TailSFN
 ----------------------------------------------------------------------------*/
PF_ERROR  PFPATH_get_tilde_of_TailSFN(const PF_CHAR *p_sfn,
                                      PF_U_LONG     *p_numeric,
                                      PF_U_LONG     *p_pos_tilde,
                                      PF_U_LONG     *p_pos_eob);

/*---------------------------------------------------------------------------
 PFPATH_parseShortNameNumeric -
 ----------------------------------------------------------------------------*/
PF_ERROR PFPATH_parseShortNameNumeric(PF_CHAR *p_sfn, PF_U_LONG new_numeric);

/*---------------------------------------------------------------------------
 PFPATH_SetSearchPattern -
 ----------------------------------------------------------------------------*/
void  PFPATH_SetSearchPattern(PF_CHAR* p_buf_local, PF_WCHAR* p_buf_unicode, PF_STR* p_pattern);

/*---------------------------------------------------------------------------
 PFPATH_CheckExtShortNameSignature - Check whether it is an correnct extended
                                     short file name signature
 ----------------------------------------------------------------------------*/
PF_BOOL PFPATH_CheckExtShortNameSignature(PF_CONST PF_STR *p_str);

/*---------------------------------------------------------------------------
 PFPATH_CheckExtShortName - Check whether it is an correnct extended short
                            file name
 ----------------------------------------------------------------------------*/
PF_U_LONG PFPATH_CheckExtShortName(PF_CONST PF_STR *p_str, PF_U_LONG target, PF_BOOL wildcard);

/*---------------------------------------------------------------------------
 PFPATH_GetExtShortNameIndex - Get an index of an extended short file name
 ----------------------------------------------------------------------------*/
PF_BOOL PFPATH_GetExtShortNameIndex(PF_CONST PF_STR *p_str, PF_U_LONG *p_index);

/*---------------------------------------------------------------------------
 PFPATH_AdjustExtShortName - Adjust an appropriate short file name
 ----------------------------------------------------------------------------*/
PF_ERROR PFPATH_AdjustExtShortName(PF_CHAR *pName, PF_U_LONG position);

/*---------------------------------------------------------------------------
  PFPATH_IsSeparator  - Check Separator.
 ----------------------------------------------------------------------------*/
#define PFPATH_IsSeparator(p_str, target, offset) \
            ((PFSTR_StrNCmp(p_str, (PF_CONST PF_CHAR*)"\\", target, offset, 1) == 0) || \
             (PFSTR_StrNCmp(p_str, (PF_CONST PF_CHAR*)"/", target, offset, 1) == 0))


#endif  /* __PF_PATH_H__ */
