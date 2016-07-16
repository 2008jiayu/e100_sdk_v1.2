/*
    Copyright(C) 2005 eSOL Co., Ltd. All rights reserved.

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
[pdm_types.h] - Definitions and Declarations commonly used in Disk Manager.
****************************************************************************/
#ifndef __PDM_TYPES_H__
#define __PDM_TYPES_H__

#include "e_types.h"
#include "prfile2/pf_config.h"

/****************************************************************************
  Internal Definitions and Declarations

  NOTES:
   - These definitions are system-dependent; User can configure them for
     user's specific environment.
****************************************************************************/
/*==========================================================================
  Basic integer types commonly used in Disk Manager
===========================================================================*/
#define PDM_CHAR            e_char_t            /* 8-bit integer           */
#define PDM_U_CHAR          uint8_t             /* 8-bit unsigned integer  */
#define PDM_S_CHAR          int8_t              /* 8-bit signed integer    */
#define PDM_U_SHORT         uint16_t            /* 16-bit unsigned integer */
#define PDM_S_SHORT         int16_t             /* 16-bit signed integer   */
#define PDM_U_LONG          uint32_t            /* 32-bit unsigned integer */
#define PDM_S_LONG          int32_t             /* 32-bit signed integer   */

#if PF_EXFAT_SUPPORT
#define PDM_INT64           int64_t             /* 64-bit signed integer   */
#define PDM_UINT64          uint64_t            /* 64-bit unsigned integer */
#endif /* PF_EXFAT_SUPPORT */


/*---------------------------------------------------------------------------
  64bit Value
 ----------------------------------------------------------------------------*/
#if defined (_MSC_VER) && (_MSC_VER <= 1300)
#define PDM_INT64_C(val)    (0I64  + (val))
#define PDM_UINT64_C(val)   (0UI64 + (val))
#else                       
#define PDM_INT64_C(val)    (0LL  + (val##LL))
#define PDM_UINT64_C(val)   (0ULL + (val##ULL))
#endif

#define PDM_ULL(val)        ((PDM_UINT64)PDM_UINT64_C(val))


/*---------------------------------------------------------------------------
  Internal Error Type
---------------------------------------------------------------------------*/
typedef PDM_S_LONG          PDM_ERROR;


/*---------------------------------------------------------------------------
  const definition for API

    This is for compatibility with PrFILE.

---------------------------------------------------------------------------*/
#define PDM_CONST           const


/*---------------------------------------------------------------------------
  LOCAL_IS_DEFINED_AS_STATIC

    If set, 'LOCAL' functions and variables (file-local functions/variables
    with 'LOCAL' pseudo storage class specifier) are defined as 'static'
    functions and variables to prevent them from being called by external
    functions.

    If not set, 'LOCAL' functions and variables are defined as 'global'
    functions/variables with external linkage. This is useful to debug
    with a old-fashioned debugger which does not handle 'static' symbols.

---------------------------------------------------------------------------*/
#define PDM_LOCAL           static

/*---------------------------------------------------------------------------
  Boolean
---------------------------------------------------------------------------*/
#ifndef PDM_BOOL
#define PDM_BOOL            e_bool_t
#endif

#ifndef PDM_FALSE
#define PDM_FALSE           e_false
#endif

#ifndef PDM_TRUE
#define PDM_TRUE            e_true
#endif

/*---------------------------------------------------------------------------
  NULL pointer
---------------------------------------------------------------------------*/
#ifndef NULL
#define NULL                ((void*)0)
#endif

/*---------------------------------------------------------------------------
  Internal Error numbers
---------------------------------------------------------------------------*/
#ifndef PDM_OK
#define PDM_OK              (0)     /* No error; Done successfully  */
#endif

#ifndef PDM_ERR
#define PDM_ERR             (-1)    /* General error                */
#endif


/*---------------------------------------------------------------------------
  size_t
---------------------------------------------------------------------------*/
#define PDM_SIZE_T          uint32_t    /* size_t definition        */

#if PF_EXFAT_SUPPORT
typedef uint64_t            PDM_FSIZE_T;
#else /* PF_EXFAT_SUPPORT */
typedef uint32_t            PDM_FSIZE_T;
#endif /* PF_EXFAT_SUPPORT */


/*---------------------------------------------------------------------------
  Sector, Cluster
 ----------------------------------------------------------------------------*/
/* Sector number, Number of sector */
#define PDM_SECT            PDM_U_LONG

/* Cluster number, Number of cluster */
#define PDM_CLST            PDM_U_LONG



#endif  /* __PDM_TYPES_H__ */
