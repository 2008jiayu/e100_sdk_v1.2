/*
     Copyright (C) 2010 By eSOL Co.,Ltd. Tokyo, Japan

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
 [pf_exfat_apicmn.h] - Definitions and Declarations commonly used in PrFILE2 API.

 NOTES:
  - Except headers which are included by 'pf_apicmn.h' itself, any of
    source (*.c) and header (*.h) files of PrFILE2 shall include this
    header 'pf_apicmn.h' (or 'pf_common.h' which includes the header
    internally) prior to any other header.
 ****************************************************************************/
#ifndef __PF_EXFAT_APICMN_H__
#define __PF_EXFAT_APICMN_H__

#include "prfile2/pf_apicmn.h"

#if PF_EXFAT_SUPPORT
/****************************************************************************
  Public Type Definitions

  NOTES:
   - These types are used with API functions in user applications.
 ****************************************************************************/
/*---------------------------------------------------------------------------
  Up-case Table Buffer information
 ----------------------------------------------------------------------------*/
struct PF_UPCASE_BUF_TAG
{
    /* pointer of Up-case Table Buffer */
    PF_U_CHAR   *buffer;

    /* Buffer Size */
    PF_U_LONG   size;

    /* flag */
    PF_U_LONG   flag;
};
typedef struct PF_UPCASE_BUF_TAG PF_UPCASE_BUF;

/* The flag of 'flag' of PF_UPCASE_BUF */
/* Bit 0 of flag */
#define PF_UPCASE_DATA_NOT_SET       (0x00000000)
#define PF_UPCASE_DATA_SET           (0x00000001)

/*---------------------------------------------------------------------------
  Buffering mode for pf_setupcasebuf()
 ----------------------------------------------------------------------------*/
#define PF_UPCASE_BUF_ENABLE         (1)
#define PF_UPCASE_BUF_DISABLE        (0)

/****************************************************************************
  PrFILE2 API Functions Prototypes
 ****************************************************************************/
/*---------------------------------------------------------------------------
  pf2_setupcasebuf - Set Up-case Table Buffer
 ----------------------------------------------------------------------------*/
int         pf2_setupcasebuf(char drv_char, PF_U_LONG mode, PF_UPCASE_BUF *p_upcasebuf);
#if PF_USE_API_STUB
int         pfstub_setupcasebuf(char drv_char, PF_U_LONG mode, PF_UPCASE_BUF *p_upcasebuf);
#endif /* PF_USE_API_STUB */

#endif  /* PF_EXFAT_SUPPORT */

#endif  /* __PF_EXFAT_APICMN_H__ */
