/*
     Copyright (C) 2008 By eSOL Co.,Ltd. Tokyo, Japan
 
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
 [pfm_api.h] - Declarations of PrFILE2 Extension-API functions
 ****************************************************************************/
#ifndef __PFM_API_H__
#define __PFM_API_H__

#include "prfile2/pfm_apicmn.h"


/****************************************************************************
  Alternative forms of PrFILE2 API functions
 ****************************************************************************/
/*---------------------------------------------------------------------------
  FAT Information API
 ----------------------------------------------------------------------------*/
#define pfm_fat_getBPB(a,b)                 pfm2_fat_getBPB(a,b)
#define pfm_fat_getClst(a,b,c,d,e)          pfm2_fat_getClst(a,b,c,d,e)
#define pfm_fat_searchFreeClst(a,b,c,d,e)   pfm2_fat_searchFreeClst(a,b,c,d,e)


#endif /* __PFM_API_H__ */
