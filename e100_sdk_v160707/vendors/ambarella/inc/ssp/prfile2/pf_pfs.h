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
  [pf_pfs.h] - 
 ***************************************************************************/
#ifndef __PF_PFS_H__
#define __PF_PFS_H__

/* PrFILE2 */
#include "prfile2/pf_api.h"

#ifdef EB_PRFILE2_SUPPORT_PFS_IF

#if PF_STDLIB_ALIAS_SUPPORT
#error If use "Support PFS Interface", "Support alias of standard C library" can not use.
#endif /* PF_STDLIB_ALIAS_SUPPORT */

/* LFS */
#include "sys/lfs.h"

/*--------------------------------------------------------
 Macros
--------------------------------------------------------*/
/* PFS originality error */
#define PFS_OK                    (0)
#define PFS_ERR                  (-1)

#define PFS_ERR_UNKNOWN          (-1)


/* Create drive magic number from flags  */
#define PFS_PF2_SFTNUM              (8)
#define PFS_GET_DRIVE_NUM(flags)    ((PF_U_LONG)flags >> PFS_PF2_SFTNUM)
#define PFS_SET_DRIVE_NUM(flags)    ((PF_U_LONG)flags << PFS_PF2_SFTNUM)


/*--------------------------------------------------------
 Prototypes
--------------------------------------------------------*/
/*---------------------------------------------------------------------------
 prf_pfs_init
    PrFILE PFS initialize
 Input  :   None
 Output :
 Return :   0(success), -1(error)
 Note   :
---------------------------------------------------------------------------*/
int     prf_pfs_init(void);


/*---------------------------------------------------------------------------
 prf_pfs_shutdown
    PrFILE PFS shutdown
 Input  :   None
 Output :
 Return :   0(success), -1(error)
 Note   :
---------------------------------------------------------------------------*/
int     prf_pfs_shutdown(void);



/*---------------------------------------------------------------------------
 prf_pfs_service
    PrFILE PFS Service Function
 Input  :   None
 Output :
 Return :   0(success), -1(error)
 Note   :
---------------------------------------------------------------------------*/
int     prf_pfs_service(lfs_t*);

#endif /* EB_PRFILE2_SUPPORT_PFS_IF */

#endif /* __PF_PFS_H__ */

