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
 [pf_w_api.h] - Declarations of PrFILE2 API functions
 ****************************************************************************/
#ifndef __PF_W_API_H__
#define __PF_W_API_H__

#include "prfile2/pf_w_apicmn.h"


/****************************************************************************
  Alternative forms of PrFILE2 API functions
 ****************************************************************************/
/* Not support API stub */
#if !PF_USE_API_STUB

  #define pf_w_create(a,b)          pf2_w_create(a,b)
  #define pf_w_fopen(a,b)           pf2_w_fopen(a,b)
  #define pf_w_fsfirst(a,b,c)       pf2_w_fsfirst(a,b,c)
  #define pf_w_fsnext(a)            pf2_w_fsnext(a)
  #define pf_w_fsexec(a,b,c)        pf2_w_fsexec(a,b,c)
  #define pf_w_remove(a)            pf2_w_remove(a)
  #define pf_w_rename(a,b)          pf2_w_rename(a,b)
  #define pf_w_move(a,b)            pf2_w_move(a,b)
  #define pf_w_mkdir(a)             pf2_w_mkdir(a)
  #define pf_w_rmdir(a)             pf2_w_rmdir(a)
  #define pf_w_chdir(a)             pf2_w_chdir(a)
  #define pf_w_fstat(a,b)           pf2_w_fstat(a,b)
  #define pf_w_chmod(a,b)           pf2_w_chmod(a,b)
  #define pf_w_chdmod(a,b)          pf2_w_chdmod(a,b)
  #define pf_w_combine(a,b)         pf2_w_combine(a,b)
  #define pf_w_fconcat(a,b)         pf2_w_fconcat(a,b)
  #define pf_w_divide(a,b,c)        pf2_w_divide(a,b,c)
  #define pf_w_xdivide(a,b,c)       pf2_w_xdivide(a,b,c)
  #define pf_w_fdivide(a)           pf2_w_fdivide(a)
  #define pf_w_cinsert(a,b,c)       pf2_w_cinsert(a,b,c)
  #define pf_w_insert(a,b)          pf2_w_insert(a,b)
  #define pf_w_cdelete(a,b,c)       pf2_w_cdelete(a,b,c)
  #define pf_w_cut(a,b)             pf2_w_cut(a,b)
  #define pf_w_opendir(a)           pf2_w_opendir(a)
  #define pf_w_createdir(a,b,c)     pf2_w_createdir(a,b,c)
  #define pf_w_cleandir(a,b,c,d)    pf2_w_cleandir(a,b,c,d)
  #define pf_w_deletedir(a)         pf2_w_deletedir(a)
  #define pf_w_chmoddir(a,b,c,d,e)  pf2_w_chmoddir(a,b,c,d,e)
  #define pf_w_truncate(a,b)        pf2_w_truncate(a,b)
  #define pf_w_getstamp(a,b)        pf2_w_getstamp(a,b)
  #define pf_w_setstamp(a,b)        pf2_w_setstamp(a,b)
  #define pf_w_fgetdta(a,b)         pf2_w_fgetdta(a,b)
  #define pf_w_setvol(a,b)          pf2_w_setvol(a,b)
  #define pf_w_getvol(a,b)          pf2_w_getvol(a,b)

#else

/* Support API stub */

  #define pf_w_create(a,b)          pfstub_w_create(a,b)
  #define pf_w_fopen(a,b)           pfstub_w_fopen(a,b)
  #define pf_w_fsfirst(a,b,c)       pfstub_w_fsfirst(a,b,c)
  #define pf_w_fsnext(a)            pfstub_w_fsnext(a)
  #define pf_w_fsexec(a,b,c)        pfstub_w_fsexec(a,b,c)
  #define pf_w_remove(a)            pfstub_w_remove(a)
  #define pf_w_rename(a,b)          pfstub_w_rename(a,b)
  #define pf_w_move(a,b)            pfstub_w_move(a,b)
  #define pf_w_mkdir(a)             pfstub_w_mkdir(a)
  #define pf_w_rmdir(a)             pfstub_w_rmdir(a)
  #define pf_w_chdir(a)             pfstub_w_chdir(a)
  #define pf_w_fstat(a,b)           pfstub_w_fstat(a,b)
  #define pf_w_chmod(a,b)           pfstub_w_chmod(a,b)
  #define pf_w_chdmod(a,b)          pfstub_w_chdmod(a,b)
  #define pf_w_combine(a,b)         pfstub_w_combine(a,b)
  #define pf_w_fconcat(a,b)         pfstub_w_fconcat(a,b)
  #define pf_w_divide(a,b,c)        pfstub_w_divide(a,b,c)
  #define pf_w_xdivide(a,b,c)       pfstub_w_xdivide(a,b,c)
  #define pf_w_fdivide(a)           pfstub_w_fdivide(a)
  #define pf_w_cinsert(a,b,c)       pfstub_w_cinsert(a,b,c)
  #define pf_w_insert(a,b)          pfstub_w_insert(a,b)
  #define pf_w_cdelete(a,b,c)       pfstub_w_cdelete(a,b,c)
  #define pf_w_cut(a,b)             pfstub_w_cut(a,b)
  #define pf_w_opendir(a)           pfstub_w_opendir(a)
  #define pf_w_createdir(a,b,c)     pfstub_w_createdir(a,b,c)
  #define pf_w_cleandir(a,b,c,d)    pfstub_w_cleandir(a,b,c,d)
  #define pf_w_deletedir(a)         pfstub_w_deletedir(a)
  #define pf_w_chmoddir(a,b,c,d,e)  pfstub_w_chmoddir(a,b,c,d,e)
  #define pf_w_truncate(a,b)        pfstub_w_truncate(a,b)
  #define pf_w_getstamp(a,b)        pfstub_w_getstamp(a,b)
  #define pf_w_setstamp(a,b)        pfstub_w_setstamp(a,b)
  #define pf_w_fgetdta(a,b)         pfstub_w_fgetdta(a,b)
  #define pf_w_setvol(a,b)          pfstub_w_setvol(a,b)
  #define pf_w_getvol(a,b)          pfstub_w_getvol(a,b)

#endif  /* !PF_USE_API_STUB */

#endif  /* __PF_W_API_H__ */
