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
 [pf_service.h] - Service routines that are commonly used in PrFILE2.
 ****************************************************************************/
#ifndef __PF_SERVICE_H__
#define __PF_SERVICE_H__

#include "prfile2/pf_config.h"
#include "prfile2/pf_types.h"

/**
 * Swaps the order of the bytes in an 'PF_U_SHORT' if platform uses big
 * endian representation (i.e. if 'BIG_ENDIAN' is defined).
 */
#if PF_BIG_ENDIAN

  #define LE16_TO_U16(x)                ((PF_U_SHORT)((((x) >> 8) & 0x00FF) | \
                                                      (((x) << 8) & 0xFF00)))
  #define U16_TO_LE16(x)                LE16_TO_U16(x)

  #define LE32_TO_U32(x)                ((PF_U_LONG)((((x) >> 24) & 0x000000FF) | \
                                                     (((x) >>  8) & 0x0000FF00) | \
                                                     (((x) <<  8) & 0x00FF0000) | \
                                                     (((x) << 24) & 0xFF000000)))
  #define U32_TO_LE32(x)                LE32_TO_U32(x)

  #define LE64_TO_U64(x)                ((PF_UINT64)((((x) >> 56) & PF_UINT64_C(0x00000000000000FF)) | \
                                                     (((x) >> 40) & PF_UINT64_C(0x000000000000FF00)) | \
                                                     (((x) >> 24) & PF_UINT64_C(0x0000000000FF0000)) | \
                                                     (((x) >>  8) & PF_UINT64_C(0x00000000FF000000)) | \
                                                     (((x) <<  8) & PF_UINT64_C(0x000000FF00000000)) | \
                                                     (((x) << 24) & PF_UINT64_C(0x0000FF0000000000)) | \
                                                     (((x) << 40) & PF_UINT64_C(0x00FF000000000000)) | \
                                                     (((x) << 56) & PF_UINT64_C(0xFF00000000000000))))

  #define U64_TO_LE64(x)                LE64_TO_U64(x)

  #define BE16_TO_U16(x)                ((PF_U_SHORT)(x))
  #define U16_TO_BE16(x)                BE16_TO_U16(x)

  #define BE32_TO_U32(x)                ((PF_U_LONG)(x))
  #define U32_TO_BE32(x)                BE32_TO_U32(x)

  #define BE64_TO_U64(x)                ((PF_UINT64)(x))
  #define U64_TO_BE64(x)                BE64_TO_U64(x)

#else   /* !PF_BIG_ENDIAN */

  #define LE16_TO_U16(x)                ((PF_U_SHORT)(x))
  #define U16_TO_LE16(x)                LE16_TO_U16(x)

  #define LE32_TO_U32(x)                ((PF_U_LONG)(x))
  #define U32_TO_LE32(x)                LE32_TO_U32(x)

  #define LE64_TO_U64(x)                ((PF_UINT64)(x))
  #define U64_TO_LE64(x)                LE64_TO_U64(x)


  #define BE16_TO_U16(x)                ((PF_U_SHORT)((((x) >> 8) & 0x00FF) | \
                                                      (((x) << 8) & 0xFF00)))
  #define U16_TO_BE16(x)                BE16_TO_U16(x)

  #define BE32_TO_U32(x)                ((PF_U_LONG)((((x) >> 24) & 0x000000FF) | \
                                                     (((x) >>  8) & 0x0000FF00) | \
                                                     (((x) <<  8) & 0x00FF0000) | \
                                                     (((x) << 24) & 0xFF000000)))
  #define U32_TO_BE32(x)                BE32_TO_U32(x)

  #define BE64_TO_U64(x)                LE64_TO_U64(x)

  #define U64_TO_BE64(x)                ((PF_UINT64)((((x) >> 56) & PF_UINT64_C(0x00000000000000FF)) | \
                                                     (((x) >> 40) & PF_UINT64_C(0x000000000000FF00)) | \
                                                     (((x) >> 24) & PF_UINT64_C(0x0000000000FF0000)) | \
                                                     (((x) >>  8) & PF_UINT64_C(0x00000000FF000000)) | \
                                                     (((x) <<  8) & PF_UINT64_C(0x000000FF00000000)) | \
                                                     (((x) << 24) & PF_UINT64_C(0x0000FF0000000000)) | \
                                                     (((x) << 40) & PF_UINT64_C(0x00FF000000000000)) | \
                                                     (((x) << 56) & PF_UINT64_C(0xFF00000000000000))))

#endif  /* PF_BIG_ENDIAN */


#define LOAD_FROM_LE16_BUF(buf)         LE16_TO_U16(*(PF_U_SHORT*)(buf))
#define LOAD_FROM_LE32_BUF(buf)         LE32_TO_U32(*(PF_U_LONG *)(buf))

#define STORE_INTO_LE16_BUF(buf,val)    (*(PF_U_SHORT*)(buf) = U16_TO_LE16(val))
#define STORE_INTO_LE32_BUF(buf,val)    (*(PF_U_LONG *)(buf) = U32_TO_LE32(val))


#define LOAD_FROM_LE8_FIELD(buf,offset)         (*(PF_U_CHAR *)((buf)+(offset)))

#define LOAD_FROM_LE16_FIELD(buf,offset) ((PF_U_LONG)(buf + offset) & 0x1) ? \
                (((PF_U_SHORT)(*(PF_U_CHAR*)((buf)+(offset)))) | \
                ((PF_U_SHORT)(*(PF_U_CHAR*)((buf)+(offset+1))) << 8)) \
                : LE16_TO_U16(*(((PF_U_SHORT*)(buf))+(offset >> 1) + (offset & 0x1)))

#define LOAD_FROM_LE32_FIELD(buf,offset) (offset & 0x3) ? \
                ((PF_U_LONG)(*(PF_U_CHAR*)((buf)+(offset))) | \
                ((PF_U_LONG)(*(PF_U_CHAR*)((buf)+(offset+1))) << 8) | \
                ((PF_U_LONG)(*(PF_U_CHAR*)((buf)+(offset+2))) << 16) | \
                (PF_U_LONG)(*(PF_U_CHAR*)((buf)+(offset+3))) << 24) \
                : LE32_TO_U32(*(PF_U_LONG *)((buf)+(offset)))

#define STORE_INTO_LE8_FIELD(buf,offset,val)    (*(PF_U_CHAR *)((buf)+(offset)) = (val))

#define STORE_INTO_LE16_FIELD(buf,offset,val) \
            if ((PF_U_LONG)(buf + offset) & 0x1) { \
                (*(PF_U_CHAR*)((buf)+(offset)) = (PF_U_CHAR)(val)); \
                (*(PF_U_CHAR*)((buf)+(offset+1)) = (PF_U_CHAR)(val >> 8)); \
            } else { \
                (*((PF_U_SHORT*)(buf)+(offset >> 1) + (offset & 0x1)) = U16_TO_LE16(val)); \
            }

#define STORE_INTO_LE32_FIELD(buf,offset,val) \
            if ((PF_U_LONG)(buf + offset) & 0x3) { \
                (*(PF_U_CHAR *)((buf)+(offset)) = (PF_U_CHAR)(val)); \
                (*(PF_U_CHAR *)((buf)+(offset+1)) = (PF_U_CHAR)(val >> 8)); \
                (*(PF_U_CHAR *)((buf)+(offset+2)) = (PF_U_CHAR)(val >> 16)); \
                (*(PF_U_CHAR *)((buf)+(offset+3)) = (PF_U_CHAR)(val >> 24)); \
            } else { \
                (*((PF_U_LONG *)(buf)+(offset >> 2)+(offset & 0x3)) = U32_TO_LE32(val)); \
            }

/**
 * Writes the bits of 'val' determined by 'mask' as a
 * little endian 'PF_U_SHORT' at 'buf' (bits which are 0 in
 * 'mask' are left unchanged, the rest of them are overwritten).
 */
/*inline*/
void    PF_SET_LE_U16(const PF_U_SHORT val, PF_U_CHAR* buf, const PF_U_SHORT mask);

/**
 * Writes the bits of 'val' determined by 'mask' as a
 * little endian 'PF_U_LONG' at 'buf' (bits which are 0 in
 * 'mask' are left unchanged, the rest of them are overwritten).
 */
/*inline*/
void    PF_SET_LE_U32(const PF_U_LONG val, PF_U_CHAR* buf, const PF_U_LONG mask);

/**
 * Returns the 'PF_U_SHORT' value located (in little endian format) at
 * 'buf'.
 */
/*inline*/
PF_U_SHORT     PF_GET_LE_U16(const PF_U_CHAR* buf);

/**
 * Returns the 'PF_U_LONG' value located (in little endian format) at
 * 'buf'.
 */
/*inline*/
PF_U_LONG     PF_GET_LE_U32(const PF_U_CHAR* buf);


/*---------------------------------------------------------------------------
 PF_LE16_TO_U16_STR -
 ----------------------------------------------------------------------------*/
void PF_LE16_TO_U16_STR(PF_U_CHAR* sSrc, PF_U_LONG num);

#if PF_BIG_ENDIAN
#define LE16_TO_U16_STR(x,y)        PF_LE16_TO_U16_STR(x,y)
#else
#define LE16_TO_U16_STR(x,y)
#endif

#endif  /* __PF_SERVICE_H__ */
