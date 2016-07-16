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
 [pf_e_api.h] - Declarations of PrFILE2 API functions
 ****************************************************************************/
#ifndef __PF_E_API_H__
#define __PF_E_API_H__

#include "prfile2/pf_e_apicmn.h"


/****************************************************************************
  Alternative forms of PrFILE2 API functions
 ****************************************************************************/
/* Not support API stub */
#if !PF_USE_API_STUB

  #define pf_e_create(a,b,c)        pf2_e_create(a,b,c)
  #define pf_e_fopen(a,b,c)         pf2_e_fopen(a,b,c)
  #define pf_e_fclose(a)            pf2_e_fclose(a)
  #define pf_e_fread(a,b,c,d,e)     pf2_e_fread(a,b,c,d,e)
  #define pf_e_fwrite(a,b,c,d,e)    pf2_e_fwrite(a,b,c,d,e)
  #define pf_e_fseek(a,b,c)         pf2_e_fseek(a,b,c)
  #define pf_e_fsfirst(a,b,c)       pf2_e_fsfirst(a,b,c)
  #define pf_e_fsexec(a,b,c)        pf2_e_fsexec(a,b,c)
  #define pf_e_remove(a)            pf2_e_remove(a)
  #define pf_e_rename(a,b)          pf2_e_rename(a,b)
  #define pf_e_mkdir(a)             pf2_e_mkdir(a)
  #define pf_e_rmdir(a)             pf2_e_rmdir(a)
  #define pf_e_fstat(a,b)           pf2_e_fstat(a,b)
  #define pf_e_feof(a,b)            pf2_e_feof(a,b)
  #define pf_e_fsync(a)             pf2_e_fsync(a)
  #define pf_e_opendir(a,b)         pf2_e_opendir(a,b)
  #define pf_e_closedir(a)          pf2_e_closedir(a)
  #define pf_e_readdir(a,b)         pf2_e_readdir(a,b)
  #define pf_e_seekdir(a,b)         pf2_e_seekdir(a,b)
  #define pf_e_ftruncate(a,b)       pf2_e_ftruncate(a,b)
  #define pf_e_getstamp(a,b)        pf2_e_getstamp(a,b)
  #define pf_e_setstamp(a,b)        pf2_e_setstamp(a,b)
  #define pf_e_mount(a)             pf2_e_mount(a)
  #define pf_e_unmount(a,b,c)       pf2_e_unmount(a,b,c)
  #define pf_e_cinsert(a,b,c,d)     pf2_e_cinsert(a,b,c,d)
  #define pf_e_cdelete(a,b,c,d)     pf2_e_cdelete(a,b,c,d)
  #define pf_e_combine(a,b)         pf2_e_combine(a,b)
  #define pf_e_divide(a,b,c)        pf2_e_divide(a,b,c)
  #define pf_e_cleandir(a,b,c,d)    pf2_e_cleandir(a,b,c,d)
  #define pf_e_deletedir(a)         pf2_e_deletedir(a)
  #define pf_e_chmoddir(a,b,c,d,e)  pf2_e_chmoddir(a,b,c,d,e)
  #define pf_e_getdev(a,b)          pf2_e_devinf(a,b)
  #define pf_e_setvol(a,b)          pf2_e_setvol(a,b)
  #define pf_e_getvol(a,b)          pf2_e_getvol(a,b)
  #define pf_e_rmvvol(a)            pf2_e_rmvvol(a)
  #define pf_e_buffering(a,b)       pf2_e_buffering(a,b)
  #define pf_e_setupfsi(a,b)        pf2_e_setupfsi(a,b)
  #define pf_e_settailbuf(a,b)      pf2_e_settailbuf(a,b)
  #define pf_e_setvolcfg(a,b)       pf2_e_setvolcfg(a,b)
  #define pf_e_getvolcfg(a,b)       pf2_e_getvolcfg(a,b)
  #define pf_e_setencode(a)         pf2_e_setencode(a)
  #define pf_e_format(a,b)          pf2_e_format(a,b)
  #define pf_e_dlock(a,b)           pf2_e_dlock(a,b)
  #define pf_e_dlock_islocked(a,b)  pf2_e_dlock_islocked(a,b);
  #define pf_e_dlock_fislocked(a,b) pf2_e_dlock_fislocked(a,b);
  #define pf_e_dlock_dislocked(a,b) pf2_e_dlock_dislocked(a,b);
#else

/* Support API stub */

  #define pf_e_create(a,b,c)        pfstub_e_create(a,b,c)
  #define pf_e_fopen(a,b,c)         pfstub_e_fopen(a,b,c)
  #define pf_e_fclose(a)            pfstub_e_fclose(a)
  #define pf_e_fread(a,b,c,d,e)     pfstub_e_fread(a,b,c,d,e)
  #define pf_e_fwrite(a,b,c,d,e)    pfstub_e_fwrite(a,b,c,d,e)
  #define pf_e_fseek(a,b,c)         pfstub_e_fseek(a,b,c)
  #define pf_e_fsfirst(a,b,c)       pfstub_e_fsfirst(a,b,c)
  #define pf_e_fsexec(a,b,c)        pfstub_e_fsexec(a,b,c)
  #define pf_e_remove(a)            pfstub_e_remove(a)
  #define pf_e_rename(a,b)          pfstub_e_rename(a,b)
  #define pf_e_mkdir(a)             pfstub_e_mkdir(a)
  #define pf_e_rmdir(a)             pfstub_e_rmdir(a)
  #define pf_e_fstat(a,b)           pfstub_e_fstat(a,b)
  #define pf_e_feof(a,b)            pfstub_e_feof(a,b)
  #define pf_e_fsync(a)             pfstub_e_fsync(a)
  #define pf_e_opendir(a,b)         pfstub_e_opendir(a,b)
  #define pf_e_closedir(a)          pfstub_e_closedir(a)
  #define pf_e_readdir(a,b)         pfstub_e_readdir(a,b)
  #define pf_e_seekdir(a,b)         pfstub_e_seekdir(a,b)
  #define pf_e_ftruncate(a,b)       pfstub_e_ftruncate(a,b)
  #define pf_e_getstamp(a,b)        pfstub_e_getstamp(a,b)
  #define pf_e_setstamp(a,b)        pfstub_e_setstamp(a,b)
  #define pf_e_mount(a)             pfstub_e_mount(a)
  #define pf_e_unmount(a,b,c)       pfstub_e_unmount(a,b,c)
  #define pf_e_cinsert(a,b,c,d)     pfstub_e_cinsert(a,b,c,d)
  #define pf_e_cdelete(a,b,c,d)     pfstub_e_cdelete(a,b,c,d)
  #define pf_e_combine(a,b)         pfstub_e_combine(a,b)
  #define pf_e_divide(a,b,c)        pfstub_e_divide(a,b,c)
  #define pf_e_cleandir(a,b,c,d)    pfstub_e_cleandir(a,b,c,d)
  #define pf_e_deletedir(a)         pfstub_e_deletedir(a)
  #define pf_e_chmoddir(a,b,c,d,e)  pfstub_e_chmoddir(a,b,c,d,e)
  #define pf_e_getdev(a,b)          pfstub_e_devinf(a,b)
  #define pf_e_setvol(a,b)          pfstub_e_setvol(a,b)
  #define pf_e_getvol(a,b)          pfstub_e_getvol(a,b)
  #define pf_e_rmvvol(a)            pfstub_e_rmvvol(a)
  #define pf_e_buffering(a,b)       pfstub_e_buffering(a,b)
  #define pf_e_setupfsi(a,b)        pfstub_e_setupfsi(a,b)
  #define pf_e_settailbuf(a,b)      pfstub_e_settailbuf(a,b)
  #define pf_e_setvolcfg(a,b)       pfstub_e_setvolcfg(a,b)
  #define pf_e_getvolcfg(a,b)       pfstub_e_getvolcfg(a,b)
  #define pf_e_setencode(a)         pfstub_e_setencode(a)
  #define pf_e_format(a,b)          pfstub_e_format(a,b)
  #define pf_e_dlock(a,b)           pfstub_e_dlock(a,b)
  #define pf_e_dlock_islocked(a,b)  pfstub_e_dlock_islocked(a,b);
  #define pf_e_dlock_fislocked(a,b) pfstub_e_dlock_fislocked(a,b);
  #define pf_e_dlock_dislocked(a,b) pfstub_e_dlock_dislocked(a,b);
#endif  /* !PF_USE_API_STUB */

#endif  /* __PF_E_API_H__ */
