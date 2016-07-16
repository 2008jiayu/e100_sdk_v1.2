/*
 * Copyright (c) 2014 ARM Ltd
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the company may not be used to endorse or promote
 *    products derived from this software without specific prior written
 *    permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ARM LTD ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL ARM LTD BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 * TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef __ARM_ARCH

/* ACLE standardises a set of pre-defines that describe the ARM architecture.
   These were mostly implemented in GCC around GCC-4.8; older versions
   have no, or only partial support.  To provide a level of backwards
   compatibility we try to work out what the definitions should be, given
   the older pre-defines that GCC did produce.  This isn't complete, but
   it should be enough for use by routines that depend on this header.  */

/* No need to handle ARMv8, GCC had ACLE support before that.  */

# if 1 //def __ARM_ARCH_7__
/* The common subset of ARMv7 in all profiles.  */
#  define __ARM_ARCH 7
#  define __ARM_ARCH_ISA_THUMB 2
#  define __ARM_FEATURE_CLZ
#  define __ARM_FEATURE_LDREX 7
//#  define __ARM_FEATURE_UNALIGNED
# endif

# if 1 //defined (__ARM_ARCH_7A__) || defined (__ARM_ARCH_7R__)
#  define __ARM_ARCH 7
#  define __ARM_ARCH_ISA_THUMB 2
#  define __ARM_ARCH_ISA_ARM
#  define __ARM_FEATURE_CLZ
#  define __ARM_FEATURE_SIMD32
//#  define __ARM_FEATURE_DSP
#  define __ARM_FEATURE_QBIT
#  define __ARM_FEATURE_SAT
//#  define __ARM_FEATURE_LDREX 15
//#  define __ARM_FEATURE_UNALIGNED
#  ifdef __ARM_ARCH_7A__
#   define __ARM_ARCH_PROFILE 'A'
#  else
#   define __ARM_ARCH_PROFILE 'R'
#  endif
# endif


#endif /* __ARM_ARCH  */

