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
 [pf_apicmn.h] - Definitions and Declarations commonly used in PrFILE2 API.

 NOTES:
  - Except headers which are included by 'pf_apicmn.h' itself, any of
    source (*.c) and header (*.h) files of PrFILE2 shall include this
    header 'pf_apicmn.h' (or 'pf_common.h' which includes the header
    internally) prior to any other header.
 ****************************************************************************/
#ifndef __PF_APICMN_H__
#define __PF_APICMN_H__

#include "prfile2/pf_apicmn_def.h"

#if PF_UNICODE_INTERFACE_SUPPORT
#include "prfile2/pf_w_apicmn.h"
#endif /* PF_UNICODE_INTERFACE_SUPPORT */

#ifdef EB_PRFILE2_SUPPORT_PFS_IF
#include "prfile2/pf_e_apicmn.h"
#endif /* EB_PRFILE2_SUPPORT_PFS_IF */

#if PF_EXFAT_SUPPORT
#include "prfile2/pf_exfat_apicmn.h"
#endif /* PF_EXFAT_SUPPORT */


/****************************************************************************
  PrFILE2 API Functions Prototypes
 ****************************************************************************/
/*---------------------------------------------------------------------------
 pf2_init_prfile2 - Initialize PrFILE2.
 ----------------------------------------------------------------------------*/
int  pf2_init_prfile2(long config, void* param);
#if PF_USE_API_STUB
int  pfstub_init_prfile2(long config, void* param);
#endif /* PF_USE_API_STUB */

/*---------------------------------------------------------------------------
 pf2_create - Create a file.
 ----------------------------------------------------------------------------*/
PF_FILE*    pf2_create(PF_CONST char* path, int mode);
#if PF_USE_API_STUB
PF_FILE*    pfstub_create(PF_CONST char* path, int mode);
#endif /* PF_USE_API_STUB */

/*---------------------------------------------------------------------------
 pf2_fopen - open a file stream.
 ----------------------------------------------------------------------------*/
PF_FILE*    pf2_fopen(PF_CONST char* path, PF_CONST char* mode);
#if PF_USE_API_STUB
PF_FILE*    pfstub_fopen(PF_CONST char* path, PF_CONST char* mode);
#endif /* PF_USE_API_STUB */

/*---------------------------------------------------------------------------
 pf2_close - Close a file.
 ----------------------------------------------------------------------------*/
int         pf2_fclose(PF_FILE* p_file);
#if PF_USE_API_STUB
int         pfstub_fclose(PF_FILE* p_file);
#endif /* PF_USE_API_STUB */

/*---------------------------------------------------------------------------
  pf2_fread - Read data from a file.
 ----------------------------------------------------------------------------*/
PF_SIZE_T   pf2_fread(void* p_buf, PF_SIZE_T size, PF_SIZE_T count, PF_FILE* p_file);
#if PF_USE_API_STUB
PF_SIZE_T   pfstub_fread(void* p_buf, PF_SIZE_T size, PF_SIZE_T count, PF_FILE* p_file);
#endif /* PF_USE_API_STUB */

/*---------------------------------------------------------------------------
  pf2_fwrite - Write data into a file.
 ----------------------------------------------------------------------------*/
PF_SIZE_T  pf2_fwrite(void* p_buf, PF_SIZE_T size, PF_SIZE_T count, PF_FILE* p_file);
#if PF_USE_API_STUB
PF_SIZE_T  pfstub_fwrite(void* p_buf, PF_SIZE_T size, PF_SIZE_T count, PF_FILE* p_file);
#endif /* PF_USE_API_STUB */

/*---------------------------------------------------------------------------
 pf2_fseek -
 ----------------------------------------------------------------------------*/
int         pf2_fseek(PF_FILE*, PF_OFF_T, int);
#if PF_USE_API_STUB
int         pfstub_fseek(PF_FILE*, PF_OFF_T, int);
#endif /* PF_USE_API_STUB */

/*---------------------------------------------------------------------------
 pf2_fsfirst -
 ----------------------------------------------------------------------------*/
int         pf2_fsfirst(PF_CONST char*, unsigned char, PF_DTA*);
#if PF_USE_API_STUB
int         pfstub_fsfirst(PF_CONST char*, unsigned char, PF_DTA*);
#endif /* PF_USE_API_STUB */

/*---------------------------------------------------------------------------
 pf2_fsnext -
 ----------------------------------------------------------------------------*/
int         pf2_fsnext(PF_DTA*);
#if PF_USE_API_STUB
int         pfstub_fsnext(PF_DTA*);
#endif /* PF_USE_API_STUB */

/*---------------------------------------------------------------------------
  pf2_fsexec -- Change entry information or remove entry.
 ----------------------------------------------------------------------------*/
int         pf2_fsexec(PF_DTA*, int, unsigned char);
#if PF_USE_API_STUB
int         pfstub_fsexec(PF_DTA*, int, unsigned char);
#endif /* PF_USE_API_STUB */

/*---------------------------------------------------------------------------
 pf2_remove -
 ----------------------------------------------------------------------------*/
int         pf2_remove(PF_CONST char*);
#if PF_USE_API_STUB
int         pfstub_remove(PF_CONST char*);
#endif /* PF_USE_API_STUB */

/*---------------------------------------------------------------------------
 pf2_rename -
 ----------------------------------------------------------------------------*/
int         pf2_rename(PF_CONST char*, PF_CONST char*);
#if PF_USE_API_STUB
int         pfstub_rename(PF_CONST char*, PF_CONST char*);
#endif /* PF_USE_API_STUB */

/*---------------------------------------------------------------------------
  pf2_move -
 ----------------------------------------------------------------------------*/
int         pf2_move(PF_CONST char*, PF_CONST char*);
#if PF_USE_API_STUB
int         pfstub_move(PF_CONST char*, PF_CONST char*);
#endif /* PF_USE_API_STUB */

/*---------------------------------------------------------------------------
 pf2_mkdir -
 ----------------------------------------------------------------------------*/
int         pf2_mkdir(PF_CONST char*);
#if PF_USE_API_STUB
int         pfstub_mkdir(PF_CONST char*);
#endif /* PF_USE_API_STUB */

/*---------------------------------------------------------------------------
 pf2_createdir -
 ----------------------------------------------------------------------------*/
int         pf2_createdir(PF_CONST char*, int, PF_DTA*);
#if PF_USE_API_STUB
int         pfstub_createdir(PF_CONST char*, int, PF_DTA*);
#endif /* PF_USE_API_STUB */

/*---------------------------------------------------------------------------
 pf2_rmdir -
 ----------------------------------------------------------------------------*/
int         pf2_rmdir(PF_CONST char*);
#if PF_USE_API_STUB
int         pfstub_rmdir(PF_CONST char*);
#endif /* PF_USE_API_STUB */

/*---------------------------------------------------------------------------
 pf2_chdir -
 ----------------------------------------------------------------------------*/
int         pf2_chdir(PF_CONST char*);
#if PF_USE_API_STUB
int         pfstub_chdir(PF_CONST char*);
#endif /* PF_USE_API_STUB */

/*---------------------------------------------------------------------------
 pf2_fstat -
 ----------------------------------------------------------------------------*/
int         pf2_fstat(PF_CONST char*, PF_STAT*);
#if PF_USE_API_STUB
int         pfstub_fstat(PF_CONST char*, PF_STAT*);
#endif /* PF_USE_API_STUB */

/*---------------------------------------------------------------------------
 pf2_fpstat -
 ----------------------------------------------------------------------------*/
int         pf2_fpstat(PF_FILE*, PF_STAT*);
#if PF_USE_API_STUB
int         pfstub_fpstat(PF_FILE*, PF_STAT*);
#endif /* PF_USE_API_STUB */

/*---------------------------------------------------------------------------
 pf2_chmod -
 ----------------------------------------------------------------------------*/
int         pf2_chmod(PF_CONST char*, int);
#if PF_USE_API_STUB
int         pfstub_chmod(PF_CONST char*, int);
#endif /* PF_USE_API_STUB */

/*---------------------------------------------------------------------------
 pf2_chdmod -
 ----------------------------------------------------------------------------*/
int         pf2_chdmod(PF_CONST char*, int);
#if PF_USE_API_STUB
int         pfstub_chdmod(PF_CONST char*, int);
#endif /* PF_USE_API_STUB */

/*---------------------------------------------------------------------------
 pf2_attach - Attach drives.
 ----------------------------------------------------------------------------*/
int         pf2_attach(PF_DRV_TBL** drv_tbl);
#if PF_USE_API_STUB
int         pfstub_attach(PF_DRV_TBL** drv_tbl);
#endif /* PF_USE_API_STUB */

/*---------------------------------------------------------------------------
 pf2_detach - Detach drive.
 ----------------------------------------------------------------------------*/
int  pf2_detach(char drive);
#if PF_USE_API_STUB
int  pfstub_detach(char drive);
#endif /* PF_USE_API_STUB */

/*---------------------------------------------------------------------------
 pf2_mount - Attach drives. (OBSOLETE) & No Backword Compatibility, Device Mount.
 ----------------------------------------------------------------------------*/
#if PF_BACKWARD_COMPATIBILITY_MOUNT
int         pf2_mount(PF_DRV_TBL** drv_tbl);
#else
int         pf2_mount(char drive);
#endif /* PF_BACKWARD_COMPATIBILITY_MOUNT */
#if PF_USE_API_STUB
#if PF_BACKWARD_COMPATIBILITY_MOUNT
int         pfstub_mount(PF_DRV_TBL** drv_tbl);
#else
int         pfstub_mount(char drive);
#endif /* PF_BACKWARD_COMPATIBILITY_MOUNT */
#endif /* PF_USE_API_STUB */

/*---------------------------------------------------------------------------
 pf2_unmount - Unmount drives.
 ----------------------------------------------------------------------------*/
int  pf2_unmount(char drive, PF_U_LONG mode);
#if PF_USE_API_STUB
int  pfstub_unmount(char drive, PF_U_LONG mode);
#endif /* PF_USE_API_STUB */

/*---------------------------------------------------------------------------
  pf2_format - Format a volume specified by 'drive'.
 ----------------------------------------------------------------------------*/
int         pf2_format(char drive, PF_CONST char* param);
#if PF_USE_API_STUB
int         pfstub_format(char drive, PF_CONST char* param);
#endif /* PF_USE_API_STUB */

/*---------------------------------------------------------------------------
 pf2_buffering -
 ----------------------------------------------------------------------------*/
int         pf2_buffering(char, int);
#if PF_USE_API_STUB
int         pfstub_buffering(char, int);
#endif /* PF_USE_API_STUB */

/*---------------------------------------------------------------------------
 pf2_ferror - returns error number of the last occurred error associated
              with the file specified by 'p_file'.
 ----------------------------------------------------------------------------*/
int         pf2_ferror(PF_FILE*);
#if PF_USE_API_STUB
int         pfstub_ferror(PF_FILE*);
#endif /* PF_USE_API_STUB */

/*---------------------------------------------------------------------------
 pf2_feof -
 ----------------------------------------------------------------------------*/
int         pf2_feof(PF_FILE*);
#if PF_USE_API_STUB
int         pfstub_feof(PF_FILE*);
#endif /* PF_USE_API_STUB */

/*---------------------------------------------------------------------------
 pf2_errnum - returns error number of the last occurred error.

 [Return Value]
    Error number of the last occurred error.
 ----------------------------------------------------------------------------*/
int         pf2_errnum(void);
#if PF_USE_API_STUB
int         pfstub_errnum(void);
#endif /* PF_USE_API_STUB */

/*---------------------------------------------------------------------------
 pf2_devinf -
 ----------------------------------------------------------------------------*/
int         pf2_devinf(char, PF_DEV_INF*);
#if PF_USE_API_STUB
int         pfstub_devinf(char, PF_DEV_INF*);
#endif /* PF_USE_API_STUB */

/*---------------------------------------------------------------------------
 pf2_setvol -
 ----------------------------------------------------------------------------*/
int         pf2_setvol(char, PF_CONST char*);
#if PF_USE_API_STUB
int         pfstub_setvol(char, PF_CONST char*);
#endif /* PF_USE_API_STUB */

/*---------------------------------------------------------------------------
 pf2_getvol -
 ----------------------------------------------------------------------------*/
int         pf2_getvol(char, PF_VOLTAB*);
#if PF_USE_API_STUB
int         pfstub_getvol(char, PF_VOLTAB*);
#endif /* PF_USE_API_STUB */

/*---------------------------------------------------------------------------
 pf2_rmvvol -
 ----------------------------------------------------------------------------*/
int         pf2_rmvvol(char);
#if PF_USE_API_STUB
int         pfstub_rmvvol(char);
#endif /* PF_USE_API_STUB */

/*---------------------------------------------------------------------------
 pf2_setupfsi -
 ----------------------------------------------------------------------------*/
int pf2_setupfsi(char, short);
#if PF_USE_API_STUB
int         pfstub_setupfsi(char, short);
#endif /* PF_USE_API_STUB */

#if PF_USE_CLSTLINK
/*---------------------------------------------------------------------------
 pf2_setclstlink -
 ----------------------------------------------------------------------------*/
int pf2_setclstlink(char, long, PF_CLUSTER*);
#if PF_USE_API_STUB
int         pfstub_setclstlink(char, long, PF_CLUSTER*);
#endif /* PF_USE_API_STUB */

/*---------------------------------------------------------------------------
 pf2_fsetclstlink -
 ----------------------------------------------------------------------------*/
int pf2_fsetclstlink(PF_FILE*, long, PF_CLUSTER_FILE*);
#if PF_USE_API_STUB
int         pfstub_fsetclstlink(PF_FILE*, long, PF_CLUSTER_FILE*);
#endif /* PF_USE_API_STUB */
#endif /* PF_USE_CLSTLINK*/

/*---------------------------------------------------------------------------
 pf2_sync -
 ----------------------------------------------------------------------------*/
int pf2_sync(char, long);
#if PF_USE_API_STUB
int         pfstub_sync(char, long);
#endif /* PF_USE_API_STUB */

/*---------------------------------------------------------------------------
 pf2_fsync -
 ----------------------------------------------------------------------------*/
int pf2_fsync(PF_FILE*);
#if PF_USE_API_STUB
int         pfstub_fsync(PF_FILE*);
#endif /* PF_USE_API_STUB */

/*---------------------------------------------------------------------------
 pf2_fappend -
 ----------------------------------------------------------------------------*/
PF_FSIZE_T   pf2_fappend(PF_FILE*, PF_FSIZE_T);
#if PF_USE_API_STUB
PF_FSIZE_T   pfstub_fappend(PF_FILE*, PF_FSIZE_T);
#endif /* PF_USE_API_STUB */

/*---------------------------------------------------------------------------
  pf2_fadjust -- Adjust the unused cluster in opened file.
 ----------------------------------------------------------------------------*/
int         pf2_fadjust(PF_FILE*);
#if PF_USE_API_STUB
int         pfstub_fadjust(PF_FILE*);
#endif /* PF_USE_API_STUB */

/*---------------------------------------------------------------------------
  pf2_fgetdta -- Get DTA from file pointer.
 ----------------------------------------------------------------------------*/
int         pf2_fgetdta(PF_FILE*, PF_DTA*);
#if PF_USE_API_STUB
int         pfstub_fgetdta(PF_FILE*, PF_DTA*);
#endif /* PF_USE_API_STUB */

/*---------------------------------------------------------------------------
  pf2_finfo -- Get file information.
 ----------------------------------------------------------------------------*/
int         pf2_finfo(PF_FILE*, PF_INFO*);
#if PF_USE_API_STUB
int         pfstub_finfo(PF_FILE*, PF_INFO*);
#endif /* PF_USE_API_STUB */

/*---------------------------------------------------------------------------
 pf2_derrnum - returns error number of the last occurred driver error.
 ----------------------------------------------------------------------------*/
int         pf2_derrnum(char);
#if PF_USE_API_STUB
int         pfstub_derrnum(char);
#endif /* PF_USE_API_STUB */

/*---------------------------------------------------------------------------
 pf2_combine - Combine specified files.
 ----------------------------------------------------------------------------*/
int         pf2_combine(PF_CONST char*, PF_CONST char*);
#if PF_USE_API_STUB
int         pfstub_combine(PF_CONST char*, PF_CONST char*);
#endif /* PF_USE_API_STUB */

/*---------------------------------------------------------------------------
 pf2_fconcat - Combine specified files.
 ----------------------------------------------------------------------------*/
int         pf2_fconcat(PF_CONST char*, PF_CONST char*);
#if PF_USE_API_STUB
int         pfstub_fconcat(PF_CONST char*, PF_CONST char*);
#endif /* PF_USE_API_STUB */

/*---------------------------------------------------------------------------
  pf2_divide -- Divide specified file.
 ----------------------------------------------------------------------------*/
int         pf2_divide(PF_CONST char*, PF_CONST char*, PF_FPOS_T);
#if PF_USE_API_STUB
int         pfstub_divide(PF_CONST char*, PF_CONST char*, PF_FPOS_T);
#endif /* PF_USE_API_STUB */

/*---------------------------------------------------------------------------
  pf2_xdivide -- Divide specified file.
 ----------------------------------------------------------------------------*/
int         pf2_xdivide(PF_CONST char*, PF_CONST char*, PF_SIZE_T);
#if PF_USE_API_STUB
int         pfstub_xdivide(PF_CONST char*, PF_CONST char*, PF_SIZE_T);
#endif /* PF_USE_API_STUB */

/*---------------------------------------------------------------------------
  pf2_fdivide -- Divide specified file.
 ----------------------------------------------------------------------------*/
int         pf2_fdivide(PF_FDIVIDE *pfdivide);
#if PF_USE_API_STUB
int         pfstub_fdivide(PF_FDIVIDE *pfdivide);
#endif /* PF_USE_API_STUB */

/*---------------------------------------------------------------------------
  pf2_cinsert -- Insert clusters in file.
 ----------------------------------------------------------------------------*/
int         pf2_cinsert(PF_CONST char*, PF_U_LONG, PF_U_LONG);
#if PF_USE_API_STUB
int         pfstub_cinsert(PF_CONST char*, PF_U_LONG, PF_U_LONG);
#endif /* PF_USE_API_STUB */

/*---------------------------------------------------------------------------
  pf2_insert -- Insert cluster in file.
 ----------------------------------------------------------------------------*/
int         pf2_insert(PF_CONST char*, PF_U_LONG);
#if PF_USE_API_STUB
int         pfstub_insert(PF_CONST char*, PF_U_LONG);
#endif /* PF_USE_API_STUB */

/*---------------------------------------------------------------------------
  pf2_cdelete -- Delete clusters in file.
 ----------------------------------------------------------------------------*/
int         pf2_cdelete(PF_CONST char*, PF_U_LONG, PF_U_LONG);
#if PF_USE_API_STUB
int         pfstub_cdelete(PF_CONST char*, PF_U_LONG, PF_U_LONG);
#endif /* PF_USE_API_STUB */

/*---------------------------------------------------------------------------
  pf2_cut -- Delete cluster in file.
 ----------------------------------------------------------------------------*/
int         pf2_cut(PF_CONST char*, PF_U_LONG);
#if PF_USE_API_STUB
int         pfstub_cut(PF_CONST char*, PF_U_LONG);
#endif /* PF_USE_API_STUB */

/*---------------------------------------------------------------------------
  pf2_settailbuf --
 ----------------------------------------------------------------------------*/
int         pf2_settailbuf(char, PF_U_LONG, PF_TAIL_BUF*);
#if PF_USE_API_STUB
int         pfstub_settailbuf(char, PF_U_LONG, PF_TAIL_BUF*);
#endif /* PF_USE_API_STUB */

/*---------------------------------------------------------------------------
  pf2_setvolcfg --
 ----------------------------------------------------------------------------*/
int         pf2_setvolcfg(char, PF_VOL_CFG*);
#if PF_USE_API_STUB
int         pfstub_setvolcfg(char, PF_VOL_CFG*);
#endif /* PF_USE_API_STUB */

/*---------------------------------------------------------------------------
  pf2_getvolcfg --
 ----------------------------------------------------------------------------*/
int         pf2_getvolcfg(char, PF_VOL_CFG*);
#if PF_USE_API_STUB
int         pfstub_getvolcfg(char, PF_VOL_CFG*);
#endif

/*---------------------------------------------------------------------------
  pf2_setcode --
 ----------------------------------------------------------------------------*/
int         pf2_setcode(PF_CHARCODE*);
#if PF_USE_API_STUB
int         pfstub_setcode(PF_CHARCODE*);
#endif

/*---------------------------------------------------------------------------
 pf2_opendir -
 ----------------------------------------------------------------------------*/
PF_DIR*     pf2_opendir(PF_CONST char*);
#if PF_USE_API_STUB
PF_DIR*     pfstub_opendir(PF_CONST char*);
#endif

/*---------------------------------------------------------------------------
 pf2_closedir -
 ----------------------------------------------------------------------------*/
int         pf2_closedir(PF_DIR* p_dir);
#if PF_USE_API_STUB
int         pfstub_closedir(PF_DIR* p_dir);
#endif

/*---------------------------------------------------------------------------
 pf2_readdir -
 ----------------------------------------------------------------------------*/
int         pf2_readdir(PF_DIR* p_dir, PF_DIRENT* p_dirent);
#if PF_USE_API_STUB
int         pfstub_readdir(PF_DIR* p_dir, PF_DIRENT* p_dirent);
#endif

/*---------------------------------------------------------------------------
 pf2_telldir -
 ----------------------------------------------------------------------------*/
int         pf2_telldir(PF_DIR* p_dir, PF_S_LONG* p_offset);
#if PF_USE_API_STUB
int         pfstub_telldir(PF_DIR* p_dir, PF_S_LONG* p_offset);
#endif

/*---------------------------------------------------------------------------
 pf2_seekdir -
 ----------------------------------------------------------------------------*/
int         pf2_seekdir(PF_DIR* p_dir, PF_S_LONG offset);
#if PF_USE_API_STUB
int         pfstub_seekdir(PF_DIR* p_dir, PF_S_LONG offset);
#endif

/*---------------------------------------------------------------------------
 pf2_rewinddir -
 ----------------------------------------------------------------------------*/
int         pf2_rewinddir(PF_DIR* p_dir);
#if PF_USE_API_STUB
int         pfstub_rewinddir(PF_DIR* p_dir);
#endif

/*---------------------------------------------------------------------------
 pf2_fchdir -
 ----------------------------------------------------------------------------*/
int         pf2_fchdir(PF_DIR* p_dir);
#if PF_USE_API_STUB
int         pfstub_fchdir(PF_DIR* p_dir);
#endif

/*---------------------------------------------------------------------------
 pf2_regctx -
 ----------------------------------------------------------------------------*/
int         pf2_regctx(void);
#if PF_USE_API_STUB
int         pfstub_regctx(void);
#endif

/*---------------------------------------------------------------------------
 pf2_unregctx -
 ----------------------------------------------------------------------------*/
int         pf2_unregctx(void);
#if PF_USE_API_STUB
int         pfstub_unregctx(void);
#endif

/*---------------------------------------------------------------------------
 pf2_flock -
 ----------------------------------------------------------------------------*/
int         pf2_flock(PF_FILE*, PF_U_LONG);
#if PF_USE_API_STUB
int         pfstub_flock(PF_FILE*, PF_U_LONG);
#endif

/*---------------------------------------------------------------------------
 pf2_setencode -
 ----------------------------------------------------------------------------*/
int         pf2_setencode(PF_U_LONG encode_mode);
#if PF_USE_API_STUB
int         pfstub_setencode(PF_U_LONG encode_mode);
#endif

/*---------------------------------------------------------------------------
 pf2_cleandir -
 ----------------------------------------------------------------------------*/
int         pf2_cleandir(PF_CONST char* p_path, PF_CONST char* p_pattern,
                         unsigned long mode, unsigned long* p_count);
#if PF_USE_API_STUB
int         pfstub_cleandir(PF_CONST char* p_path, PF_CONST char* p_pattern,
                            unsigned long mode, unsigned long* p_count);
#endif

/*---------------------------------------------------------------------------
 pf2_deletedir -
 ----------------------------------------------------------------------------*/
int         pf2_deletedir(PF_CONST char* p_path);
#if PF_USE_API_STUB
int         pfstub_deletedir(PF_CONST char* p_path);
#endif

/*---------------------------------------------------------------------------
 pf2_chmoddir -
 ----------------------------------------------------------------------------*/
int         pf2_chmoddir(PF_CONST char *dname, PF_CONST char *fname,
                         unsigned long mode, unsigned long attr,
                         unsigned long *p_count);
#if PF_USE_API_STUB
int         pfstub_chmoddir(PF_CONST char *dname, PF_CONST char *fname,
                            unsigned long mode, unsigned long attr,
                            unsigned long *p_count);
#endif

/*---------------------------------------------------------------------------
 pf2_getempent -
 ----------------------------------------------------------------------------*/
int         pf2_getempent(long *p_entries);
#if PF_USE_API_STUB
int         pfstub_getempent(long *p_entries);
#endif /* PF_USE_API_STUB */

/*---------------------------------------------------------------------------
 pf2_dgetempent -
 ----------------------------------------------------------------------------*/
int  pf2_dgetempent(char drive, long *p_entries);
#if PF_USE_API_STUB
int         pfstub_dgetempent(char drive, long *p_entries);
#endif /* PF_USE_API_STUB */

/*---------------------------------------------------------------------------
 pf2_truncate -
 ----------------------------------------------------------------------------*/
int         pf2_truncate(const char* p_path, PF_FPOS_T offset);
#if PF_USE_API_STUB
int         pfstub_truncate(const char* p_path, PF_FPOS_T offset);
#endif

/*---------------------------------------------------------------------------
 pf2_ftruncate -
 ----------------------------------------------------------------------------*/
int         pf2_ftruncate(PF_FILE* p_file, PF_FPOS_T offset);
#if PF_USE_API_STUB
int         pfstub_ftruncate(PF_FILE* p_file, PF_FPOS_T offset);
#endif

/*---------------------------------------------------------------------------
 pf2_getstamp -
 ----------------------------------------------------------------------------*/
int         pf2_getstamp(const char* p_path, PF_TIMESTMP* p_timestamp);
#if PF_USE_API_STUB
int         pfstub_getstamp(const char* p_path, PF_TIMESTMP* p_timestamp);
#endif

/*---------------------------------------------------------------------------
 pf2_setstamp -
 ----------------------------------------------------------------------------*/
int         pf2_setstamp(const char* p_path, PF_TIMESTMP* p_timestamp);
#if PF_USE_API_STUB
int         pfstub_setstamp(const char* p_path, PF_TIMESTMP* p_timestamp);
#endif

/*---------------------------------------------------------------------------
 pf2_getstmp -
 ----------------------------------------------------------------------------*/
int         pf2_getstmp(const char*     p_path,
                        PF_SYS_DATE*   p_mkdate,
                        PF_SYS_TIME*   p_mktime,
                        PF_SYS_DATE*   p_update,
                        PF_SYS_TIME*   p_uptime);
#if PF_USE_API_STUB
int         pfstub_getstmp(const char*  p_path,
                           PF_SYS_DATE* p_mkdate,
                           PF_SYS_TIME* p_mktime,
                           PF_SYS_DATE* p_update,
                           PF_SYS_TIME* p_uptime);
#endif

/*---------------------------------------------------------------------------
 pf2_setstmp -
 ----------------------------------------------------------------------------*/
int         pf2_setstmp(const char*     p_path,
                        PF_SYS_DATE*   p_mkdate,
                        PF_SYS_TIME*   p_mktime,
                        PF_SYS_DATE*   p_update,
                        PF_SYS_TIME*   p_uptime);
#if PF_USE_API_STUB
int         pfstub_setstmp(const char*  p_path,
                           PF_SYS_DATE* p_mkdate,
                           PF_SYS_TIME* p_mktime,
                           PF_SYS_DATE* p_update,
                           PF_SYS_TIME* p_uptime);
#endif

/*---------------------------------------------------------------------------
 pf2_fgetpos - Get current file I/O pointer
 ----------------------------------------------------------------------------*/
int         pf2_fgetpos(PF_FILE*, PF_FPOS_T*);
#if PF_USE_API_STUB
int         pfstub_fgetpos(PF_FILE*, PF_FPOS_T*);
#endif /* PF_USE_API_STUB */

/*---------------------------------------------------------------------------
 pf2_fsetpos - Set current file I/O pointer
 ----------------------------------------------------------------------------*/
int         pf2_fsetpos(PF_FILE*, const PF_FPOS_T*);
#if PF_USE_API_STUB
int         pfstub_fsetpos(PF_FILE*, const PF_FPOS_T*);
#endif /* PF_USE_API_STUB */

#if PF_MODULE_SUPPORT
/*---------------------------------------------------------------------------
 pf2_reg_module - Register/release a module I/F
 ----------------------------------------------------------------------------*/
int         pf2_reg_module(char, PF_MODULE_ID, void*, void*);
#if PF_USE_API_STUB
int         pfstub_reg_module(char, PF_MODULE_ID, void*, void*);
#endif /* PF_USE_API_STUB */

/*---------------------------------------------------------------------------
 pf2_ctrl_module - ioctl for the module
 ----------------------------------------------------------------------------*/
int         pf2_ctrl_module(char, PF_MODULE_ID, PF_U_LONG, void*);
#if PF_USE_API_STUB
int         pfstub_ctrl_module(char, PF_MODULE_ID, PF_U_LONG, void*);
#endif /* PF_USE_API_STUB */

int         pf2_ctrl_module_nb(char, PF_MODULE_ID, PF_U_LONG, void*);

/*---------------------------------------------------------------------------
 pf2_module_errnum - The last error of the module
 ----------------------------------------------------------------------------*/
int         pf2_module_errnum(char, PF_MODULE_ID);
#if PF_USE_API_STUB
int         pfstub_module_errnum(char, PF_MODULE_ID);
#endif /* PF_USE_API_STUB */

#endif /* PF_MODULE_SUPPORT */

/*---------------------------------------------------------------------------
  pf2_dlock -- Lock the directory
 ----------------------------------------------------------------------------*/
int         pf2_dlock(PF_DIR*, PF_U_LONG);
#if PF_USE_API_STUB
int         pfstub_dlock(PF_DIR*, PF_U_LONG);
#endif /* PF_USE_API_STUB */


#endif  /* __PF_APICMN_H__ */
