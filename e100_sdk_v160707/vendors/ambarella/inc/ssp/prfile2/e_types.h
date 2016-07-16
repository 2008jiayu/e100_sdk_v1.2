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
[e_types.h] ver1.0 rev3 - common types of parts
****************************************************************************/
#if !defined __E_TYPES_H__
#define __E_TYPES_H__

#if defined (__STDC_VERSION__) \
    && (__STDC_VERSION__ >= 199901L)


/* Supporting ISO/IEC 9899:1999 (C99) */

#include <inttypes.h>

#if defined (__GNUC__) \
    && ((__GNUC__ >= 4) \
        || ((__GNUC__ == 3) \
            && (__GNUC_MINOR__ >= 3)))

/*
 * GNU Compiler Collection (GCC)
 *
 * NOTE : e_types.h is certified only with the following versions:
 *  GCC 3.3.2 (eSOL)
 *  GCC 4.0.2 (eSOL)
 */

#define ESOL_INLINE static inline

#elif defined(__IAR_SYSTEMS_ICC__)

/*
 * IAR System ICC Compiler
 */

#define ESOL_INLINE static inline

#else /* (__GNUC__) .... */

/* unsupported combination */

#error unsupported compiler.

#endif /* #else */

#else /* (__STDC_VERSION__ >= 199901L) */


/* Non-supporting ISO/IEC 9899:1999 (C99) */

/* Common macro for the minimum and the maximum size of each type */
#define E_INT8_C(val)       (0  + (val))
#define E_UINT8_C(val)      (0U + (val))
#define E_INT16_C(val)      (0  + (val))
#define E_UINT16_C(val)     (0U + (val))
#define E_INT32_C(val)      (0  + (val))
#define E_UINT32_C(val)     (0U + (val))
#define E_INT64_C(val)      (0LL  + (val))
#define E_UINT64_C(val)     (0ULL + (val))


#if defined (__ARMCC_VERSION) \
    && (__ARMCC_VERSION >= 200000)

/*
 * RealView Compilation Tools (RVCT)
 *
 * NOTE : e_types.h is certified only with the following versions:
 *  RVCT 2.0 Patch (Build 155)
 *  RVCT 2.1 Patch (Build 558)
 *  RVCT 2.2 Patch (Build 503)
 */

#include <inttypes.h>

#define ESOL_INLINE static __inline


#elif defined (__GNUC__) \
    && ((__GNUC__ >= 4) \
        || ((__GNUC__ == 3) \
            && (__GNUC_MINOR__ >= 3)))

/*
 * GNU Compiler Collection (GCC)
 *
 * NOTE : e_types.h is certified only with the following versions:
 *  GCC 3.3.2 (eSOL)
 *  GCC 4.0.2 (eSOL)
 */

/* GNU Compiler Collection (GCC) Versin 3.3.2 */
#if defined(__PRKERNEL_AMB__) || defined(__TX_AMB__)
/* The arm-elf-gcc compiler from Ambarella does not supply inttypes.h    */
/* and other glibc headers; they are placed under 'system/include/ebcfg' */
#include <ebcfg/inttypes.h>
#else
#include <inttypes.h>
#endif

#define ESOL_INLINE static __inline__


#elif defined (__TMS470__)

/*
 * TMS470 C/C++ CODE GENERATION TOOLS
 *
 * NOTE : e_types.h is certified only with the following versions:
 *  Release 2.24
 *  Release 2.25
 *  Release 2.52
 *
 * NOTE : It is necessary to identify the supporting compiler version by using
 * a macro that is defined in each file.
 * limits.h is the common header that might be not removed and not
 * changed drastically.
 */
#include <limits.h>

#if defined (LLONG_MIN) \
    && defined (LLONG_MAX) \
    && defined (ULLONG_MAX)

/* Release 2.52 */

#include <inttypes.h>

#else /* Version */

/* Release 2.24 or Release 2.25 */

/* int<x>_t */
typedef signed char         int8_t;
typedef signed short int    int16_t;
typedef signed int          int32_t;

/* uint<x>_t */
typedef unsigned char       uint8_t;
typedef unsigned short int  uint16_t;
typedef unsigned int        uint32_t;

typedef signed int          intptr_t;
typedef unsigned int        uintptr_t;

/* minimum and maximum macro */
#define INT8_MAX            E_INT8_C(127)
#define INT16_MAX           E_INT16_C(32767)
#define INT32_MAX           E_INT32_C(2147483647)
#define INT64_MAX           E_INT64_C(9223372036854775807)

#define UINT8_MAX           E_UINT8_C(255)
#define UINT16_MAX          E_UINT16_C(65535)
#define UINT32_MAX          E_UINT32_C(4294967295)
#define UINT64_MAX          E_UINT64_C(18446744073709551615)

#define INT8_MIN            (-INT8_MAX-1)
#define INT16_MIN           (-INT16_MAX-1)
#define INT32_MIN           (-INT32_MAX-1)
#define INT64_MIN           (-INT64_MAX-1)

#define INTPTR_MAX          INT32_MAX
#define INTPTR_MIN          INT32_MIN
#define UINTPTR_MAX         UINT32_MAX

#endif /* Version */

#define ESOL_INLINE         static __inline


#elif defined (__HITACHI_VERSION__) \
    && (__HITACHI_VERSION__ >= 0x0600)

/*
 * SuperH RISC engine Compiler Package
 *
 * NOTE : e_types.h is certified only with the following versions:
 *  V.6.0C (eBinder only)
 *  V.9.00 (HEW only)
 *
 * NOTE : It is necessary to identify the supporting compiler version by using
 * a macro that is defined in each file.
 * limits.h is the common header that might be not removed and not
 * changed drastically.
 */
#include <limits.h>

#if defined (_LIMITS_H_)

/* with eBinder */

#include <inttypes.h>

#elif defined (_LIMITS) /* #if defined (_LIMITS_H_) */

/* with High-peformance Embedded Workshop (HEW) */

/* int<x>_t */
typedef signed char             int8_t;
typedef signed short int        int16_t;
typedef signed int              int32_t;
#if (__HITACHI_VERSION__ >= 0x0900)
typedef signed long long int    int64_t;
#endif /* #if (__HITACHI_VERSION__ >= 0x0900) */

/* uint<x>_t */
typedef unsigned char           uint8_t;
typedef unsigned short int      uint16_t;
typedef unsigned int            uint32_t;
#if (__HITACHI_VERSION__ >= 0x0900)
typedef unsigned long long int  uint64_t;
#endif /* #if (__HITACHI_VERSION__ >= 0x0900) */

typedef signed int              intptr_t;
typedef unsigned int            uintptr_t;

/* minimum and max macro */
#define INT8_MAX            SCHAR_MAX
#define INT16_MAX           SHRT_MAX
#define INT32_MAX           LONG_MAX

#define UINT8_MAX           UCHAR_MAX
#define UINT16_MAX          USHRT_MAX
#define UINT32_MAX          ULONG_MAX

#define INT8_MIN            SCHAR_MIN
#define INT16_MIN           SHRT_MIN
#define INT32_MIN           LONG_MIN

#if (__HITACHI_VERSION__ >= 0x0900)
#define INT64_MAX           LLONG_MAX
#define UINT64_MAX          ULLONG_MAX
#define INT64_MIN           LLONG_MIN
#else /* #if (__HITACHI_VERSION__ >= 0x0900) */
#define INT64_MAX           E_INT64_C(9223372036854775807)
#define UINT64_MAX          E_UINT64_C(18446744073709551615)
#define INT64_MIN           (-INT64_MAX-1)
#endif /* #else */

#define INTPTR_MAX          LONG_MAX
#define UINTPTR_MAX         ULONG_MAX
#define INTPTR_MIN          LONG_MIN

#else /* #elif defined (_LIMITS) */

/* unsupported combination */

#error unsupported compiler.

#endif /* #else */

/*
 * NOTE : if inlined functions area defined with SHC,
 * they MUST be done as follows:
 *
 * #if defined __HITACHI__
 * #pragma inline(foo)
 * #endif
 * ESOL_INLINE int foo(void)
 * {
 *     return 0;
 * }
 */
#define ESOL_INLINE         static

#elif defined (_MSC_VER)

/*
 * Microsoft Visual C++
 *
 * NOTE : e_types.h is certified only with the following version:
 *  6.0
 */

#include <stddef.h>
#include <limits.h>

/* int<x>_t */
typedef signed __int8       int8_t;
typedef signed __int16      int16_t;
typedef signed __int32      int32_t;
typedef signed __int64      int64_t;

/* uint<x>_t */
typedef unsigned __int8     uint8_t;
typedef unsigned __int16    uint16_t;
typedef unsigned __int32    uint32_t;
typedef unsigned __int64    uint64_t;

typedef signed __int32      intptr_t;
typedef unsigned __int32    uintptr_t;

/* minimum and max macro */
#define INT8_MAX            SCHAR_MAX
#define INT16_MAX           SHRT_MAX
#define INT32_MAX           LONG_MAX
#define INT64_MAX           E_INT64_C(9223372036854775807)

#define UINT8_MAX           UCHAR_MAX
#define UINT16_MAX          USHRT_MAX
#define UINT32_MAX          ULONG_MAX
#define UINT64_MAX          E_UINT64_C(18446744073709551615)

#define INT8_MIN            SCHAR_MIN
#define INT16_MIN           SHRT_MIN
#define INT32_MIN           LONG_MIN
#define INT64_MIN           (-INT64_MAX-1)

#define INTPTR_MIN          LONG_MIN
#define INTPTR_MAX          LONG_MAX
#define UINTPTR_MAX         ULONG_MAX

#define ESOL_INLINE         static __inline


#else  /* #elif defined (_MSC_VER) */

/*
 * unsupported compilation tools
 */
#error unsupported compiler.

#endif /* #else */

#endif /* (__STDC_VERSION__ >= 199901L) */

/*
 * Character type
 *
 * Do not use for 8bits-width data, only use for characters.
 * If you declare 8bits-width variable, you must use int8_t or uint8_t.
 * MUST NOT assume that the type is signed or unsigned,
 * because character value has no sign.
 */
typedef int8_t          e_char_t;

/*
 * Boolean type
 *
 * C99 defines _Bool to declare true or false, and _Bool must have
 * the size enough to store the value 0 and 1.
 * To clarify the width, e_types.h provides e_bool_t to store the
 * value of a logical value.
 */
typedef uint32_t            e_bool_t;
#define e_false             (0)
#define e_true              (1)

#endif /* __E_TYPES_H__ */
