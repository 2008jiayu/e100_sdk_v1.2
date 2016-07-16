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
 [pf_w_apicmn.h] - Definitions and Declarations commonly used in PrFILE2 API.

 NOTES:
  - Except headers which are included by 'pf_w_apicmn.h' itself, any of
    source (*.c) and header (*.h) files of PrFILE2 shall include this
    header 'pf_w_apicmn.h' (or 'pf_w_common.h' which includes the header
    internally) prior to any other header.
 ****************************************************************************/
#ifndef __PF_W_APICMN_H__
#define __PF_W_APICMN_H__

#include "prfile2/pf_apicmn.h"

#if !PF_UNICODE_INTERFACE_SUPPORT
#error The configuration is illegal.
#endif /* !PF_UNICODE_INTERFACE_SUPPORT */

/****************************************************************************
  Public Type Definitions

  NOTES:
   - These types are used with API functions in user applications.
 ****************************************************************************/
/*---------------------------------------------------------------------------
  Information for both pf_w_fsfirst() and pf_w_fsnext().
 ----------------------------------------------------------------------------*/
typedef struct PF_W_DTA {
    /*-----------------------------------------------------------------------*
      User fields: this is for user area, fsexec API sets file/dir pointer to this area/
     *-----------------------------------------------------------------------*/
    /* File descriptor */
    PF_FILE*    p_file;

    /* Directory descriptor */
    PF_DIR*     p_dir;

    /*-----------------------------------------------------------------------*
      Internal fields: do not use these as they will change without notice
     *-----------------------------------------------------------------------*/
    /* Volume of the parent directory   */
    PF_VOLUME*  p_vol;

    /* Parent directory start cluster */
    PF_U_LONG   parent_start_cluster;

    /* Next entry position within parent */
    PF_U_LONG   parent_pos; /* started from 1 */

    /* Last access cluster information */
    PF_FAT_HINT hint;

    /* Status of PF_DTA */
    PF_U_LONG   status;

    /* Located path length */
#if PF_CHECK_PATH_LENGTH
    PF_U_LONG   path_len;
#endif /* PF_CHECK_PATH_LENGTH */

    /* Number of LFNs; If it is zero, string in 'long_name' field is not valid */
    PF_U_CHAR   num_entry_LFNs;

    /* Ordinal field of LFN entry, without LAST_LONG_ENTRY flag */
    PF_U_CHAR   ordinal;

    /* FAT  : Checksum field of LFN entry
     * exFAT: Hash field of File name     */
    PF_U_SHORT   check_sum;

#if PF_EXFAT_SUPPORT
    /* Parent directory GeneralSecondaryFlags */
    PF_U_CHAR parent_general_flg;

    /* Parent directory size in bytes */
    PF_FSIZE_T parent_dir_size;

    /* Parent Path */
    PF_WCHAR parent_path[LONG_NAME_PATH_CHARS + 1];

    /* Number of Secondary Entry */
    PF_U_CHAR   num_secondary;

    /* GeneralSecondaryFlags */
    PF_U_CHAR   general_flg;
#endif

    /* Attributes searched for */
    PF_U_CHAR   attr;

    /* Regular expression for file name */
    PF_CHAR     reg_exp[PATH_BUF_SIZE];


    /*-----------------------------------------------------------------------*
      Public fields:
     *-----------------------------------------------------------------------*/
#if PF_EXFAT_SUPPORT
    /* Modification milliseconds (actually contains a count of tenths of a second) */
    PF_U_SHORT  TimeMs;
#endif

    /* Modification time */
    PF_U_SHORT  Time;

    /* Modification date */
    PF_U_SHORT  Date;

#if PF_EXFAT_SUPPORT
    /* Offset from UTC of Modification time */
    PF_U_CHAR   UtcOffset;
#endif

#if PF_EXFAT_SUPPORT
    /* Access time */
    PF_U_SHORT  ATime;
#endif

    /* Access date */
    PF_U_SHORT  ADate;

#if PF_EXFAT_SUPPORT
    /* Offset from UTC of Access time */
    PF_U_CHAR   AUtcOffset;
#endif

    /* Create milliseconds (actually contains a count of tenths of a second) */
    PF_U_SHORT  CTimeMs;

    /* Create time */
    PF_U_SHORT  CTime;

    /* Create date */
    PF_U_SHORT  CDate;

#if PF_EXFAT_SUPPORT
    /* Offset from UTC of Create time */
    PF_U_CHAR   CUtcOffset;
#endif

    /* File size in bytes */
    PF_FSIZE_T  FileSize;

    /* File attributes */
    PF_U_SHORT  Attribute;

    /* Short file name (8.3) */
    PF_CHAR     FileName[SHORT_NAME_BUF_SIZE];

    /* Long file name */
    PF_CHAR     LongName[LONG_NAME_BUF_SIZE];

#if defined(__PRKERNEL_AMB__) || defined(_AMBA_KAL_)
    PF_U_LONG   StartClst;
#endif

    /*-----------------------------------------------------------------------*
      Internal fields for UNICODE: do not use these as they will change without notice
     *-----------------------------------------------------------------------*/
    /* Regular expression for file name (Unicode) */
    PF_WCHAR    reg_exp_uni[LONG_NAME_PATH_CHARS + 1];

    /*-----------------------------------------------------------------------*
      Public fields for UNICODE:
     *-----------------------------------------------------------------------*/
    /* Short file name for Unicode */
    PF_WCHAR    FileNameUni[SHORT_DOT_NAME_CHARS + 1];

    /* Long file name for Unicode */
    PF_WCHAR    LongNameUni[LONG_NAME_CHARS + 1];

} PF_W_DTA;



/*---------------------------------------------------------------------------
  Volume label table for pf_w_getvol().
 ----------------------------------------------------------------------------*/
typedef struct PF_W_VOLTAB {
    /* Attributes of the volume label */
    PF_U_CHAR   v_attr;

    /* Date */
    PF_U_LONG   v_date;

    /* Time */
    PF_U_LONG   v_time;

    /* Volume label string (null-terminated) */
    PF_WCHAR     v_name[VOLUME_LABEL_CHARS+1];
} PF_W_VOLTAB;


/*---------------------------------------------------------------------------
  Divide information for pf_w_fdivide()
 ----------------------------------------------------------------------------*/
typedef struct PF_W_FDIVIDE
{
    /* Pointer to file path for file to divide */
    PF_WCHAR        *fbase;

    /* Cluster offset from the beginning of the file to the division location */
    PF_U_LONG       idivide;

    /* Pointer to file path for file of the first half created after division */
    PF_WCHAR        *f1half;

    /* Pointer to file path for file of the latter half created after division */
    PF_WCHAR        *f2half;

    /* Size of file of the first half */
    PF_FSIZE_T      f1size;
} PF_W_FDIVIDE;


/****************************************************************************
  PrFILE2 API Functions Prototypes
 ****************************************************************************/
/*---------------------------------------------------------------------------
 pf2_w_create - Create a file.

 [Description]
    The pf2_w_create() function creates a new file specified by 'path',
    opens a file stream associated with it, and then return a pointer to
    the PF_FILE structure associated with the stream.

    The second argument 'mode' is to specify open mode for the created
    file, but currently always ignored for compatibility with PrFILE1;
    The newly-created file is opened with update mode, both input and
    output may be performed on the associated stream.

    This function that called as follows:

        pf2_w_create(path, mode);

    is similar to the following function:

        pf2_w_fopen(path, "w+");

    However, pf2_w_create() fails to create a new file 'path' if it exists,
    where pf2_w_fopen() overwrites it.

 [Return Value]
    A pointer to the PF_FILE structure assocated to the stream for the
    newly-created file specified by 'path', or
    NULL if failed to create the file.
 ----------------------------------------------------------------------------*/
PF_FILE*    pf2_w_create(PF_CONST PF_WCHAR* path, int mode);
#if PF_USE_API_STUB
PF_FILE*    pfstub_w_create(PF_CONST PF_WCHAR* path, int mode);
#endif /* PF_USE_API_STUB */


/*---------------------------------------------------------------------------
 pf2_w_fopen - open a file stream.

 [Description]
    The pf2_w_fopen() function opens a file stream for the file specified by
    'path'. The 'mode' string begins with one of the following sequences:

    a
        Append: create a new file or open the file for writing at its
        end.
    a+
        Append: open the file or create it for update, writing at
        end-of-file.
    r
        Open the file for reading.
    r+
        Open the file for update (reading and/or writing).
    w
        Create the file for writing, or truncate it to zero length.
    w+
        Create the file for update, or truncate it to zero length.

    You can add the letter b to the end of any of the above sequences to
    indicate that the file is (or must be) a binary file (this is an ANSI
    requirement for portability to systems that make a distinction between
    text and binary files, such as DOS). For PrFILE2, there's no difference
    between "text" files and "binary" files.

    Opening a file in read mode (r in the mode) fails if the file doesn't
    exist or can't be read.

    Opening a file in append mode (a in the mode) causes all subsequent
    writes to the file to be forced to the current end-of-file, regardless
    of previous calls to the fseek() function.

    When a file is opened with update mode (+ in the mode), both input
    and output may be performed on the associated stream.

 [Return Value]
    A pointer to the PF_FILE structure assocated to the stream, or
    NULL if failed to open the stream.
 ----------------------------------------------------------------------------*/
PF_FILE*    pf2_w_fopen(PF_CONST PF_WCHAR* path, PF_CONST PF_WCHAR* mode);
#if PF_USE_API_STUB
PF_FILE*    pfstub_w_fopen(PF_CONST PF_WCHAR* path, PF_CONST PF_WCHAR* mode);
#endif /* PF_USE_API_STUB */


/*---------------------------------------------------------------------------
 pf2_w_fsfirst -
 ----------------------------------------------------------------------------*/
int         pf2_w_fsfirst(PF_CONST PF_WCHAR*, unsigned char, PF_W_DTA*);
#if PF_USE_API_STUB
int         pfstub_w_fsfirst(PF_CONST PF_WCHAR*, unsigned char, PF_W_DTA*);
#endif /* PF_USE_API_STUB */


/*---------------------------------------------------------------------------
 pf2_w_fsnext -
 ----------------------------------------------------------------------------*/
int         pf2_w_fsnext(PF_W_DTA*);
#if PF_USE_API_STUB
int         pfstub_w_fsnext(PF_W_DTA*);
#endif /* PF_USE_API_STUB */


/*---------------------------------------------------------------------------
  pf2_w_fsexec -- Change entry information or remove entry.
  [Description]
    This function operates entry information. The possible operation are
    remove entry, change attribute or open entry.

  [Return Value]
    OK if done successfully, or
    ERRs otherwise.
 ----------------------------------------------------------------------------*/
int         pf2_w_fsexec(PF_W_DTA*, int, unsigned char);
#if PF_USE_API_STUB
int         pfstub_w_fsexec(PF_W_DTA*, int, unsigned char);
#endif /* PF_USE_API_STUB */


/*---------------------------------------------------------------------------
 pf2_w_remove -
 ----------------------------------------------------------------------------*/
int         pf2_w_remove(PF_CONST PF_WCHAR*);
#if PF_USE_API_STUB
int         pfstub_w_remove(PF_CONST PF_WCHAR*);
#endif /* PF_USE_API_STUB */


/*---------------------------------------------------------------------------
 pf2_w_rename -
 ----------------------------------------------------------------------------*/
int         pf2_w_rename(PF_CONST PF_WCHAR*, PF_CONST PF_WCHAR*);
#if PF_USE_API_STUB
int         pfstub_w_rename(PF_CONST PF_WCHAR*, PF_CONST PF_WCHAR*);
#endif /* PF_USE_API_STUB */


/*---------------------------------------------------------------------------
  pf2_w_move -
 ----------------------------------------------------------------------------*/
int         pf2_w_move(PF_CONST PF_WCHAR*, PF_CONST PF_WCHAR*);
#if PF_USE_API_STUB
int         pfstub_w_move(PF_CONST PF_WCHAR*, PF_CONST PF_WCHAR*);
#endif /* PF_USE_API_STUB */


/*---------------------------------------------------------------------------
 pf2_w_mkdir -
 ----------------------------------------------------------------------------*/
int         pf2_w_mkdir(PF_CONST PF_WCHAR*);
#if PF_USE_API_STUB
int         pfstub_w_mkdir(PF_CONST PF_WCHAR*);
#endif /* PF_USE_API_STUB */


/*---------------------------------------------------------------------------
 pf2_w_createdir -
 ----------------------------------------------------------------------------*/
int         pf2_w_createdir(PF_CONST PF_WCHAR*, int, PF_W_DTA*);
#if PF_USE_API_STUB
int         pfstub_w_createdir(PF_CONST PF_WCHAR*, int, PF_W_DTA*);
#endif /* PF_USE_API_STUB */


/*---------------------------------------------------------------------------
 pf2_w_rmdir -
 ----------------------------------------------------------------------------*/
int         pf2_w_rmdir(PF_CONST PF_WCHAR*);
#if PF_USE_API_STUB
int         pfstub_w_rmdir(PF_CONST PF_WCHAR*);
#endif /* PF_USE_API_STUB */


/*---------------------------------------------------------------------------
 pf2_w_chdir -
 ----------------------------------------------------------------------------*/
int         pf2_w_chdir(PF_CONST PF_WCHAR*);
#if PF_USE_API_STUB
int         pfstub_w_chdir(PF_CONST PF_WCHAR*);
#endif /* PF_USE_API_STUB */


/*---------------------------------------------------------------------------
 pf2_w_fstat -
 ----------------------------------------------------------------------------*/
int         pf2_w_fstat(PF_CONST PF_WCHAR*, PF_STAT*);
#if PF_USE_API_STUB
int         pfstub_w_fstat(PF_CONST PF_WCHAR*, PF_STAT*);
#endif /* PF_USE_API_STUB */


/*---------------------------------------------------------------------------
 pf2_w_chmod -
 ----------------------------------------------------------------------------*/
int         pf2_w_chmod(PF_CONST PF_WCHAR*, int);
#if PF_USE_API_STUB
int         pfstub_w_chmod(PF_CONST PF_WCHAR*, int);
#endif /* PF_USE_API_STUB */


/*---------------------------------------------------------------------------
 pf2_w_chdmod -
 ----------------------------------------------------------------------------*/
int         pf2_w_chdmod(PF_CONST PF_WCHAR*, int);
#if PF_USE_API_STUB
int         pfstub_w_chdmod(PF_CONST PF_WCHAR*, int);
#endif /* PF_USE_API_STUB */


/*---------------------------------------------------------------------------
 pf2_w_combine - Combine specified files.

  [Description]
    This function combine that user specified files.

  [Return Value]
    OK if done successfully, or
    ERRs otherwise.
 ----------------------------------------------------------------------------*/
int         pf2_w_combine(PF_CONST PF_WCHAR*, PF_CONST PF_WCHAR*);
#if PF_USE_API_STUB
int         pfstub_w_combine(PF_CONST PF_WCHAR*, PF_CONST PF_WCHAR*);
#endif /* PF_USE_API_STUB */


/*---------------------------------------------------------------------------
 pf2_w_fconcat - Combine specified files.

  [Description]
    This function combine that user specified files.

  [Return Value]
    OK if done successfully, or
    ERRs otherwise.
 ----------------------------------------------------------------------------*/
int         pf2_w_fconcat(PF_CONST PF_WCHAR*, PF_CONST PF_WCHAR*);
#if PF_USE_API_STUB
int         pfstub_w_fconcat(PF_CONST PF_WCHAR*, PF_CONST PF_WCHAR*);
#endif /* PF_USE_API_STUB */


/*---------------------------------------------------------------------------
  pf2_w_divide -- Divide specified file.

  [Description]
    This function divide that user specified file.

  [Return Value]
    OK if done successfully, or
    ERRs otherwise.
 ----------------------------------------------------------------------------*/
int         pf2_w_divide(PF_CONST PF_WCHAR*, PF_CONST PF_WCHAR*, PF_FPOS_T);
#if PF_USE_API_STUB
int         pfstub_w_divide(PF_CONST PF_WCHAR*, PF_CONST PF_WCHAR*, PF_FPOS_T);
#endif /* PF_USE_API_STUB */


/*---------------------------------------------------------------------------
  pf2_w_xdivide -- Divide specified file.

  [Description]
    This function divide that user specified file.

  [Return Value]
    OK if done successfully, or
    ERRs otherwise.
 ----------------------------------------------------------------------------*/
int         pf2_w_xdivide(PF_CONST PF_WCHAR*, PF_CONST PF_WCHAR*, PF_SIZE_T);
#if PF_USE_API_STUB
int         pfstub_w_xdivide(PF_CONST PF_WCHAR*, PF_CONST PF_WCHAR*, PF_SIZE_T);
#endif /* PF_USE_API_STUB */


/*---------------------------------------------------------------------------
  pf2_w_fdivide -- Divide specified file.

  [Description]
    This function divide that user specified file.

  [Return Value]
    OK if done successfully, or
    ERRs otherwise.
 ----------------------------------------------------------------------------*/
int         pf2_w_fdivide(PF_W_FDIVIDE *);
#if PF_USE_API_STUB
int         pfstub_w_fdivide(PF_W_FDIVIDE *);
#endif /* PF_USE_API_STUB */


/*---------------------------------------------------------------------------
  pf2_w_cinsert -- Insert clusters in file.

  [Description]
    This function insert clusters in specified file.

  [Return Value]
    OK if done successfully, or
    ERRs otherwise.
 ----------------------------------------------------------------------------*/
int         pf2_w_cinsert(PF_CONST PF_WCHAR*, PF_U_LONG, PF_U_LONG);
#if PF_USE_API_STUB
int         pfstub_w_cinsert(PF_CONST PF_WCHAR*, PF_U_LONG, PF_U_LONG);
#endif /* PF_USE_API_STUB */


/*---------------------------------------------------------------------------
  pf2_w_insert -- Insert cluster in file.

  [Description]
    This function insert cluster in specified file.

  [Return Value]
    OK if done successfully, or
    ERRs otherwise.
 ----------------------------------------------------------------------------*/
int         pf2_w_insert(PF_CONST PF_WCHAR*, PF_U_LONG);
#if PF_USE_API_STUB
int         pfstub_w_insert(PF_CONST PF_WCHAR*, PF_U_LONG);
#endif /* PF_USE_API_STUB */


/*---------------------------------------------------------------------------
  pf2_w_cdelete -- Delete clusters in file.

  [Description]
    This function delete clusters in specified file.

  [Return Value]
    OK if done successfully, or
    ERRs otherwise.
 ----------------------------------------------------------------------------*/
int         pf2_w_cdelete(PF_CONST PF_WCHAR*, PF_U_LONG, PF_U_LONG);
#if PF_USE_API_STUB
int         pfstub_w_cdelete(PF_CONST PF_WCHAR*, PF_U_LONG, PF_U_LONG);
#endif /* PF_USE_API_STUB */


/*---------------------------------------------------------------------------
  pf2_w_cut -- Delete cluster in file.

  [Description]
    This function delete cluster in specified file.

  [Return Value]
    OK if done successfully, or
    ERRs otherwise.
 ----------------------------------------------------------------------------*/
int         pf2_w_cut(PF_CONST PF_WCHAR*, PF_U_LONG);
#if PF_USE_API_STUB
int         pfstub_w_cut(PF_CONST PF_WCHAR*, PF_U_LONG);
#endif /* PF_USE_API_STUB */


/*---------------------------------------------------------------------------
 pf2_w_opendir -
 ----------------------------------------------------------------------------*/
PF_DIR*     pf2_w_opendir(PF_CONST PF_WCHAR*);
#if PF_USE_API_STUB
PF_DIR*     pfstub_w_opendir(PF_CONST PF_WCHAR*);
#endif


/*---------------------------------------------------------------------------
 pf2_w_cleandir -
 ----------------------------------------------------------------------------*/
int         pf2_w_cleandir(PF_CONST PF_WCHAR* p_path, PF_CONST PF_WCHAR* p_pattern,
                           unsigned long mode, unsigned long* p_count);
#if PF_USE_API_STUB
int         pfstub_w_cleandir(PF_CONST PF_WCHAR* p_path, PF_CONST PF_WCHAR* p_pattern,
                              unsigned long mode, unsigned long* p_count);
#endif


/*---------------------------------------------------------------------------
 pf2_w_deletedir -
 ----------------------------------------------------------------------------*/
int         pf2_w_deletedir(PF_CONST PF_WCHAR* p_path);
#if PF_USE_API_STUB
int         pfstub_w_deletedir(PF_CONST PF_WCHAR* p_path);
#endif


/*---------------------------------------------------------------------------
 pf2_w_chmoddir -
 ----------------------------------------------------------------------------*/
int         pf2_w_chmoddir(PF_CONST PF_WCHAR *dname, PF_CONST PF_WCHAR *fname,
                           unsigned long mode, unsigned long attr,
                           unsigned long *p_count);
#if PF_USE_API_STUB
int         pfstub_w_chmoddir(PF_CONST PF_WCHAR *dname, PF_CONST PF_WCHAR *fname,
                              unsigned long mode, unsigned long attr,
                              unsigned long *p_count);
#endif


/*---------------------------------------------------------------------------
 pf2_w_truncate -
 ----------------------------------------------------------------------------*/
int         pf2_w_truncate(PF_CONST PF_WCHAR* p_path, PF_FPOS_T offset);
#if PF_USE_API_STUB
int         pfstub_w_truncate(PF_CONST PF_WCHAR* p_path, PF_FPOS_T offset);
#endif


/*---------------------------------------------------------------------------
 pf2_w_getstamp -
 ----------------------------------------------------------------------------*/
int         pf2_w_getstamp(PF_CONST PF_WCHAR* p_path, PF_TIMESTMP* p_timestamp);
#if PF_USE_API_STUB
int         pfstub_w_getstamp(PF_CONST PF_WCHAR* p_path, PF_TIMESTMP* p_timestamp);
#endif


/*---------------------------------------------------------------------------
 pf2_w_setstamp -
 ----------------------------------------------------------------------------*/
int         pf2_w_setstamp(PF_CONST PF_WCHAR* p_path, PF_TIMESTMP* p_timestamp);
#if PF_USE_API_STUB
int         pfstub_w_setstamp(PF_CONST PF_WCHAR* p_path, PF_TIMESTMP* p_timestamp);
#endif


/*---------------------------------------------------------------------------
 pf2_w_fgetdta -
 ----------------------------------------------------------------------------*/
int         pf2_w_fgetdta(PF_FILE *p_file, PF_W_DTA *p_dta);
#if PF_USE_API_STUB
int         pfstub_w_fgetdta(PF_FILE *p_file, PF_W_DTA *p_dta);
#endif


/*---------------------------------------------------------------------------
 pf2_w_setvol -
 ----------------------------------------------------------------------------*/
int         pf2_w_setvol(PF_WCHAR, PF_CONST PF_WCHAR*);
#if PF_USE_API_STUB
int         pfstub_w_setvol(PF_WCHAR, PF_CONST PF_WCHAR*);
#endif

/*---------------------------------------------------------------------------
 pf2_w_getvol -
 ----------------------------------------------------------------------------*/
int         pf2_w_getvol(PF_WCHAR, PF_W_VOLTAB*);
#if PF_USE_API_STUB
int         pfstub_w_getvol(PF_WCHAR, PF_W_VOLTAB*);
#endif

#endif  /* __PF_W_APICMN_H__ */
