/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaDataType.h
 *
 *  @Copyright      :: Copyright (C) 2011 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Data type definitions
\*-------------------------------------------------------------------------------------------------------------------*/

#ifndef _AMBA_DATA_TYPE_H_
#define _AMBA_DATA_TYPE_H_

#ifndef __ASM__

typedef signed char         INT8;           /* 8 bits, [-128, 127] */
typedef short               INT16;          /* 16 bits */
typedef int                 INT32;          /* 32 bits */
typedef long long           INT64;          /* 64 bits */

typedef unsigned char       UINT8;          /* 8 bits, [0, 255] */
typedef unsigned short      UINT16;         /* 16 bits */
typedef unsigned int        UINT32;         /* 32 bits */
typedef unsigned long long  UINT64;         /* 64 bits */
typedef unsigned short      WCHAR;         /* 16 bits */

typedef void            (*VOID_FUNCTION)(void);
typedef INT8            (*INT8_FUNCTION)(void);
typedef UINT8           (*UINT8_FUNCTION)(void);
typedef INT16           (*INT16_FUNCTION)(void);
typedef UINT16          (*UINT16_FUNCTION)(void);
typedef int             (*INT_FUNCTION)(void);
typedef UINT32          (*UINT32_FUNCTION)(void);

typedef void            (*VOID_INT8_IN_FUNCTION)(INT8);
typedef int             (*INT_INT8_IN_FUNCTION)(INT8);
typedef void            (*VOID_UINT8_IN_FUNCTION)(UINT8);
typedef int             (*INT_UINT8_IN_FUNCTION)(UINT8);

typedef void            (*VOID_INT16_IN_FUNCTION)(INT16);
typedef int             (*INT_INT16_IN_FUNCTION)(INT16);
typedef void            (*VOID_UINT16_IN_FUNCTION)(UINT16);
typedef int             (*INT_UINT16_IN_FUNCTION)(UINT16);

typedef void            (*VOID_INT32_IN_FUNCTION)(INT32);
typedef int             (*INT_INT32_IN_FUNCTION)(INT32);
typedef void            (*VOID_UINT32_IN_FUNCTION)(UINT32);
typedef int             (*INT_UINT32_IN_FUNCTION)(UINT32);

#define max(a, b)       (((a) > (b)) ? (a) : (b))
#define min(a, b)       (((a) < (b)) ? (a) : (b))
#define iabs(d)         (((d) >= 0) ? (d) : (-(d)))     /* for integer int iabs(int) */

#define GetUINT16(pByte)(UINT16)    ((((UINT16) ((pByte)[0])) << 8)     \
                                     | (((UINT16) ((pByte)[1]))))

#define GetUINT32(pByte)(UINT32)    ((((UINT32) ((pByte)[0])) << 24)    \
                                     | (((UINT32) ((pByte)[1])) << 16)  \
                                     | (((UINT32) ((pByte)[2])) << 8)   \
                                     | (((UINT32) ((pByte)[3]))))

#define GetArraySize(x) (sizeof(x) / sizeof((x)[0]))
#define DelayCycles(n)	{volatile UINT32 x=n; while (x--);}

#ifndef OK
#define OK              0
#endif  /* OK */

#ifndef NG
#define NG              (-1)
#endif  /* NG */

#ifndef ERROR
#define ERROR           (-1)
#endif  /* ERROR */

#ifndef TRUE
#define TRUE            1
#endif  /* TRUE */

#ifndef FALSE
#define FALSE           0
#endif  /* FALSE */

#define BIT0            0x00000001
#define BIT1            0x00000002
#define BIT2            0x00000004
#define BIT3            0x00000008
#define BIT4            0x00000010
#define BIT5            0x00000020
#define BIT6            0x00000040
#define BIT7            0x00000080
#define BIT8            0x00000100
#define BIT9            0x00000200
#define BIT10           0x00000400
#define BIT11           0x00000800
#define BIT12           0x00001000
#define BIT13           0x00002000
#define BIT14           0x00004000
#define BIT15           0x00008000
#define BIT16           0x00010000
#define BIT17           0x00020000
#define BIT18           0x00040000
#define BIT19           0x00080000
#define BIT20           0x00100000
#define BIT21           0x00200000
#define BIT22           0x00400000
#define BIT23           0x00800000
#define BIT24           0x01000000
#define BIT25           0x02000000
#define BIT26           0x04000000
#define BIT27           0x08000000
#define BIT28           0x10000000
#define BIT29           0x20000000
#define BIT30           0x40000000
#define BIT31           0x80000000

#define SetBit(value, bit)          value |= bit
#define ClearBit(value, bit)        value &= ~(bit)
#define CheckBit(value, bit)        (value & (bit))
#define ToggleBit(value, bit)       value ^= bit

#ifdef  __cplusplus
#ifndef __BEGIN_C_PROTO__
#define __BEGIN_C_PROTO__ extern "C" {
#endif
#ifndef __END_C_PROTO__
#define __END_C_PROTO__ }
#endif
#else
#ifndef __BEGIN_C_PROTO__
#define __BEGIN_C_PROTO__
#endif
#ifndef __END_C_PROTO__
#define __END_C_PROTO__
#endif
#endif


#ifdef __GNUC__
/* no_init */
#define __PRE_ATTRIB_NOINIT__
#define __POST_ATTRIB_NOINIT__      __attribute__((section(".bss.noinit")))

/* interrupt and exception function */
#define __PRE_ATTRIB_SWI__
#define __POST_ATTRIB_SWI__         __attribute__((interrupt("SWI")))
#define __PRE_ATTRIB_FIQ__
#define __POST_ATTRIB_FIQ__         __attribute__((interrupt("FIQ")))
#define __PRE_ATTRIB_IRQ__
#define __POST_ATTRIB_IRQ__         __attribute__((interrupt("IRQ")))

/* arm specific function */
#define __PRE_ATTRIB_ARM__
#define __POST_ATTRIB_ARM__         __attribute__((short_call))

/* alignment of data */
#define __PRE_ATTRIB_ALIGN__(x)
#define __POST_ATTRIB_ALIGN__(x)    __attribute__((aligned(x)))

/* weak function/data */
#define __PRE_ATTRIB_WEAK__
#define __POST_ATTRIB_WEAK__        __attribute__((weak))

/* packed data */
#define __PRE_ATTRIB_PACKED__
#define __POST_ATTRIB_PACKED__      __attribute__((packed))

/* root function and unreferenced data */
#define __PRE_ATTRIB_USED__
#define __POST_ATTRIB_USED__        __attribute__((used))

#elif defined(__ICCARM__)
#define __attribute__(x)

/* no_init */
#define __PRE_ATTRIB_NOINIT__       __no_init
#define __POST_ATTRIB_NOINIT__

/* interrupt and exception function */
#define __PRE_ATTRIB_SWI__          __swi
#define __POST_ATTRIB_SWI__
#define __PRE_ATTRIB_FIQ__          __fiq
#define __POST_ATTRIB_FIQ__
#define __PRE_ATTRIB_IRQ__          __irq
#define __POST_ATTRIB_IRQ__

/* arm specific function */
#define __PRE_ATTRIB_ARM__          __arm
#define __POST_ATTRIB_ARM__

/* alignment of data */
#define __PRE_ATTRIB_ALIGN__(x)     #pragma data_alignment(x)
#define __POST_ATTRIB_ALIGN__(x)

/* weak function/data */
#define __PRE_ATTRIB_WEAK__         __weak
#define __POST_ATTRIB_WEAK__

/* packed data */
#define __PRE_ATTRIB_PACKED__       __packed
#define __POST_ATTRIB_PACKED__

/* root function and unreferenced data */
#define __PRE_ATTRIB_USED__         __root
#define __POST_ATTRIB_USED__
#endif

#endif  /* !__ASM__ */
/*-----------------------------------------------------------------------------------------------*\
 * Defined for IAR C-Complier
\*-----------------------------------------------------------------------------------------------*/

#endif  /* _AMBA_DATA_TYPE_H_ */
