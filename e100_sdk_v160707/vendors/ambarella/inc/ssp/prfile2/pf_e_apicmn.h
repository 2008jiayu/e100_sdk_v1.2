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
 [pf_e_apicmn.h] - Definitions and Declarations commonly used in PrFILE2 API.

 NOTES:
  - Except headers which are included by 'pf_e_apicmn.h' itself, any of
    source (*.c) and header (*.h) files of PrFILE2 shall include this
    header 'pf_e_apicmn.h' (or 'pf_e_common.h' which includes the header
    internally) prior to any other header.
 ****************************************************************************/
#ifndef __PF_E_APICMN_H__
#define __PF_E_APICMN_H__

#include "prfile2/pf_apicmn.h"

/****************************************************************************
  PrFILE2 API Functions Prototypes for PFS
 ****************************************************************************/
/*---------------------------------------------------------------------------
 for direct call
 ----------------------------------------------------------------------------*/
PF_ERROR  pf2_e_buffering (PF_DRV_CHAR drv_char, PF_S_LONG mode);
PF_ERROR  pf2_e_cdelete   (PF_CONST PF_CHAR* fname, PF_U_LONG pos, PF_U_LONG num, PF_U_LONG *p_ret);
PF_ERROR  pf2_e_chmoddir  (PF_CONST PF_CHAR *dname, PF_CONST PF_CHAR *fname, PF_U_LONG mode, PF_U_LONG attr, PF_U_LONG *p_count);
PF_ERROR  pf2_e_cinsert   (PF_CONST PF_CHAR *fname, PF_U_LONG pos, PF_U_LONG num, PF_U_LONG *p_ret);
PF_ERROR  pf2_e_cleandir  (PF_CONST PF_CHAR *dname, PF_CONST PF_CHAR *fname, PF_U_LONG mode, PF_U_LONG *p_count);
PF_ERROR  pf2_e_closedir  (PF_DIR* p_dir);
PF_ERROR  pf2_e_combine   (PF_CONST PF_CHAR *base_path, PF_CONST PF_CHAR *add_path);
PF_ERROR  pf2_e_create    (PF_CONST PF_CHAR *path, PF_S_LONG mode, PF_FILE **p_fp);
PF_ERROR  pf2_e_deletedir (PF_CONST PF_CHAR *dname);
PF_ERROR  pf2_e_divide    (PF_CONST PF_CHAR *org_path, PF_CONST PF_CHAR *new_path, PF_SIZE_T offset);
PF_ERROR  pf2_e_fclose    (PF_FILE *p_file);
PF_ERROR  pf2_e_feof      (PF_FILE* p_file, PF_S_LONG *p_eof);
PF_ERROR  pf2_e_fopen     (PF_CONST PF_CHAR *path, PF_CONST PF_CHAR *sz_mode, PF_FILE **p_fp);
PF_ERROR  pf2_e_fread     (void *p_buf, PF_SIZE_T size, PF_SIZE_T count, PF_FILE *p_file, PF_SIZE_T *p_count);
PF_ERROR  pf2_e_fseek     (PF_FILE *p_file, PF_S_LONG lOffset, PF_S_LONG nOrigin);
PF_ERROR  pf2_e_fsexec    (PF_DTA* p_dta, PF_S_LONG type, PF_U_CHAR attrs);
PF_ERROR  pf2_e_fsfirst   (PF_CONST PF_CHAR *path, PF_U_CHAR attrs, PF_DTA *p_dta);
PF_ERROR  pf2_e_fstat     (PF_CONST PF_CHAR *path, PF_STAT *pStat);
PF_ERROR  pf2_e_fsync     (PF_FILE *p_file);
PF_ERROR  pf2_e_ftruncate (PF_FILE *p_file, PF_SIZE_T offset);
PF_ERROR  pf2_e_fwrite    (void* p_buf, PF_SIZE_T size, PF_SIZE_T count, PF_FILE *p_file, PF_SIZE_T *p_count);
PF_ERROR  pf2_e_devinf    (PF_DRV_CHAR drv_char, PF_DEV_INF *dev_inf);
PF_ERROR  pf2_e_getstamp  (PF_CONST PF_CHAR *path, PF_TIMESTMP *p_timestamp);
PF_ERROR  pf2_e_getvol    (PF_DRV_CHAR drv_char, PF_VOLTAB *voltab);
PF_ERROR  pf2_e_getvolcfg (PF_DRV_CHAR drv_char, PF_VOL_CFG *config);
PF_ERROR  pf2_e_mkdir     (PF_CONST PF_CHAR *path);
#if PF_BACKWARD_COMPATIBILITY_MOUNT
PF_ERROR  pf2_e_mount     (PF_DRV_TBL** drv_tbl);
#else /* !PF_BACKWARD_COMPATIBILITY_MOUNT */
PF_ERROR  pf2_e_mount     (PF_DRV_CHAR drv_char);
#endif /* !PF_BACKWARD_COMPATIBILITY_MOUNT */
PF_ERROR  pf2_e_opendir   (PF_CONST PF_CHAR *path, PF_DIR **pp_dir);
PF_ERROR  pf2_e_readdir   (PF_DIR* p_dir, PF_DIRENT* p_dirent);
PF_ERROR  pf2_e_remove    (PF_CONST PF_CHAR *path);
PF_ERROR  pf2_e_rename    (PF_CONST PF_CHAR *sOldName, PF_CONST PF_CHAR *sNewName);
PF_ERROR  pf2_e_rmdir     (PF_CONST PF_CHAR *path);
PF_ERROR  pf2_e_rmvvol    (PF_DRV_CHAR drv_char);
PF_ERROR  pf2_e_seekdir   (PF_DIR *p_dir, PF_S_LONG offset);
PF_ERROR  pf2_e_setencode (PF_U_LONG encode_mode);
PF_ERROR  pf2_e_setstamp  (PF_CONST PF_CHAR *path, PF_TIMESTMP *p_timestamp);
PF_ERROR  pf2_e_settailbuf(PF_DRV_CHAR drv_char, PF_U_LONG size);
PF_ERROR  pf2_e_setupfsi  (PF_DRV_CHAR drv_char, PF_U_SHORT flags);
PF_ERROR  pf2_e_setvol    (PF_DRV_CHAR drv_char, PF_CONST PF_CHAR *vname);
PF_ERROR  pf2_e_setvolcfg (PF_DRV_CHAR drv_char, PF_VOL_CFG *config);
PF_ERROR  pf2_e_unmount   (PF_DRV_CHAR drv_char, PF_U_LONG mode, PF_ERROR *p_true_errnum);
PF_ERROR  pf2_e_format    (PF_DRV_CHAR drv_char, PF_CONST PF_CHAR *param);
PF_ERROR  pf2_e_dlock     (PF_DIR *p_dir, PF_U_LONG lock_mode);
PF_ERROR  pf2_e_dlock_islocked(PF_CONST PF_CHAR *path, PF_BOOL *p_is_dlocked);
PF_ERROR  pf2_e_dlock_fislocked(PF_CONST PF_FILE *p_file, PF_BOOL *p_is_dlocked);
PF_ERROR  pf2_e_dlock_dislocked(PF_CONST PF_DIR *p_dir, PF_BOOL *p_is_dlocked);

/*---------------------------------------------------------------------------
 for stub call
 ----------------------------------------------------------------------------*/
#if PF_USE_API_STUB
PF_ERROR  pfstub_e_buffering (PF_DRV_CHAR drv_char, PF_S_LONG mode);
PF_ERROR  pfstub_e_cdelete   (PF_CONST PF_CHAR* fname, PF_U_LONG pos, PF_U_LONG num, PF_U_LONG *p_ret);
PF_ERROR  pfstub_e_chmoddir  (PF_CONST PF_CHAR *dname, PF_CONST PF_CHAR *fname, PF_U_LONG mode, PF_U_LONG attr, PF_U_LONG *p_count);
PF_ERROR  pfstub_e_cinsert   (PF_CONST PF_CHAR* fname, PF_U_LONG pos, PF_U_LONG num, PF_U_LONG *p_ret);
PF_ERROR  pfstub_e_cleandir  (PF_CONST PF_CHAR *dname, PF_CONST PF_CHAR *fname, PF_U_LONG mode, PF_U_LONG *p_count);
PF_ERROR  pfstub_e_closedir  (PF_DIR *p_dir);
PF_ERROR  pfstub_e_combine   (PF_CONST PF_CHAR *base_path, PF_CONST PF_CHAR *add_path);
PF_ERROR  pfstub_e_create    (PF_CONST PF_CHAR *path, PF_S_LONG mode, PF_FILE **p_fp);
PF_ERROR  pfstub_e_deletedir (PF_CONST PF_CHAR *dname);
PF_ERROR  pfstub_e_divide    (PF_CONST PF_CHAR *org_path, PF_CONST PF_CHAR *new_path, PF_SIZE_T offset);
PF_ERROR  pfstub_e_fadjust   (PF_FILE *p_file);
PF_ERROR  pfstub_e_fappend   (PF_FILE *fd, PF_SIZE_T byte, PF_SIZE_T *p_alloc_byte);
PF_ERROR  pfstub_e_fclose    (PF_FILE *p_file);
PF_ERROR  pfstub_e_feof      (PF_FILE* p_file, PF_S_LONG *p_eof);
PF_ERROR  pfstub_e_fopen     (PF_CONST PF_CHAR *path, PF_CONST PF_CHAR *sz_mode, PF_FILE **p_fp);
PF_ERROR  pfstub_e_fread     (void *p_buf, PF_SIZE_T size, PF_SIZE_T count, PF_FILE *p_file, PF_SIZE_T *p_count);
PF_ERROR  pfstub_e_fseek     (PF_FILE *p_file, PF_S_LONG lOffset, PF_S_LONG nOrigin);
PF_ERROR  pfstub_e_fsexec    (PF_DTA* p_dta, PF_S_LONG type, PF_U_CHAR attrs);
PF_ERROR  pfstub_e_fsfirst   (PF_CONST PF_CHAR *path, PF_U_CHAR attrs, PF_DTA *p_dta);
PF_ERROR  pfstub_e_fstat     (PF_CONST PF_CHAR *path, PF_STAT *pStat);
PF_ERROR  pfstub_e_fsync     (PF_FILE *p_file);
PF_ERROR  pfstub_e_ftruncate (PF_FILE *p_file, PF_SIZE_T offset);
PF_ERROR  pfstub_e_fwrite    (void* p_buf, PF_SIZE_T size, PF_SIZE_T count, PF_FILE *p_file, PF_SIZE_T *p_count);
PF_ERROR  pfstub_e_devinf    (PF_DRV_CHAR drv_char, PF_DEV_INF *dev_inf);
PF_ERROR  pfstub_e_getstamp  (PF_CONST PF_CHAR *path, PF_TIMESTMP *p_timestamp);
PF_ERROR  pfstub_e_getvol    (PF_DRV_CHAR drv_char, PF_VOLTAB *voltab);
PF_ERROR  pfstub_e_getvolcfg (PF_DRV_CHAR drv_char, PF_VOL_CFG *config);
PF_ERROR  pfstub_e_mkdir     (PF_CONST PF_CHAR *path);
#if PF_BACKWARD_COMPATIBILITY_MOUNT
PF_ERROR  pfstub_e_mount     (PF_DRV_TBL** drv_tbl);
#else /* !PF_BACKWARD_COMPATIBILITY_MOUNT */
PF_ERROR  pfstub_e_mount     (PF_DRV_CHAR drv_char);
#endif /* !PF_BACKWARD_COMPATIBILITY_MOUNT */
PF_ERROR  pfstub_e_opendir   (PF_CONST PF_CHAR *path, PF_DIR **pp_dir);
PF_ERROR  pfstub_e_readdir   (PF_DIR* p_dir, PF_DIRENT* p_dirent);
PF_ERROR  pfstub_e_remove    (PF_CONST PF_CHAR *path);
PF_ERROR  pfstub_e_rename    (PF_CONST PF_CHAR *sOldName, PF_CONST PF_CHAR *sNewName);
PF_ERROR  pfstub_e_rmdir     (PF_CONST PF_CHAR *path);
PF_ERROR  pfstub_e_rmvvol    (PF_DRV_CHAR drv_char);
PF_ERROR  pfstub_e_seekdir   (PF_DIR *p_dir, PF_S_LONG offset);
PF_ERROR  pfstub_e_setencode (PF_U_LONG encode_mode);
PF_ERROR  pfstub_e_setstamp  (PF_CONST PF_CHAR *path, PF_TIMESTMP *p_timestamp);
PF_ERROR  pfstub_e_settailbuf(PF_DRV_CHAR drv_char, PF_U_LONG size);
PF_ERROR  pfstub_e_setupfsi  (PF_DRV_CHAR drv_char, PF_U_SHORT flags);
PF_ERROR  pfstub_e_setvol    (PF_DRV_CHAR drv_char, PF_CONST PF_CHAR *vname);
PF_ERROR  pfstub_e_setvolcfg (PF_DRV_CHAR drv_char, PF_VOL_CFG *config);
PF_ERROR  pfstub_e_unmount   (PF_DRV_CHAR drv_char, PF_U_LONG mode, PF_ERROR *p_true_errnum);
PF_ERROR  pfstub_e_format    (PF_DRV_CHAR drv_char, PF_CONST PF_CHAR *param);
PF_ERROR  pfstub_e_dlock     (PF_DIR* p_dir, PF_U_LONG lock_mode);
PF_ERROR  pfstub_e_dlock_islocked(PF_CONST PF_CHAR *path, PF_BOOL *p_is_dlocked);
PF_ERROR  pfstub_e_dlock_fislocked(PF_CONST PF_FILE *p_file, PF_BOOL *p_is_dlocked);
PF_ERROR  pfstub_e_dlock_dislocked(PF_CONST PF_DIR *p_dir, PF_BOOL *p_is_dlocked);
#endif  /* PF_USE_API_STUB */

#endif  /* __PF_E_APICMN_H__ */
