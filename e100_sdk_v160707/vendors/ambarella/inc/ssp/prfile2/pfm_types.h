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
 [pfm_types.h] - Definitions and Declarations commonly used in PrFILE2 Module.
 ****************************************************************************/
#ifndef __PFM_TYPES_H__
#define __PFM_TYPES_H__

#include "e_types.h"


/****************************************************************************
  Internal Definitions and Declarations

  NOTES:
   - These definitions are system-dependent; User can configure them for
     user's specific environment.
****************************************************************************/
/*==========================================================================
  Basic integer types commonly used in PrFILE2 Module
===========================================================================*/
#define PFM_CHAR            e_char_t            /* 8-bit integer           */
#define PFM_U_CHAR          uint8_t             /* 8-bit unsigned integer  */
#define PFM_S_CHAR          int8_t              /* 8-bit signed integer    */
#define PFM_U_SHORT         uint16_t            /* 16-bit unsigned integer */
#define PFM_S_SHORT         int16_t             /* 16-bit signed integer   */
#define PFM_U_LONG          uint32_t            /* 32-bit unsigned integer */
#define PFM_S_LONG          int32_t             /* 32-bit signed integer   */


/*---------------------------------------------------------------------------
  Internal Error Type
---------------------------------------------------------------------------*/
typedef PFM_S_LONG          PFM_ERROR;


/*---------------------------------------------------------------------------
  const definition for API

    This is for compatibility with PrFILE.

---------------------------------------------------------------------------*/
#define PFM_CONST           const


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
#define PFM_LOCAL           static

/*---------------------------------------------------------------------------
  Boolean
---------------------------------------------------------------------------*/
#ifndef PFM_BOOL
#define PFM_BOOL            e_bool_t
#endif

#ifndef PFM_FALSE
#define PFM_FALSE           e_false
#endif

#ifndef PFM_TRUE
#define PFM_TRUE            e_true
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
#ifndef PFM_OK
#define PFM_OK              (0)     /* No error; Done successfully  */
#endif

#ifndef PFM_ERR
#define PFM_ERR             (-1)    /* General error                */
#endif

/*---------------------------------------------------------------------------
  size_t
---------------------------------------------------------------------------*/
#define PFM_SIZE_T          uint32_t    /* size_t definition        */


#endif /* __PFM_TYPES_H__ */
