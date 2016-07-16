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

/*
   GCC will define __STRICT_ANSI_ with '-std=c99' and
   it will make bzero() having no declaration.
   And got warning due to '-Wimplicit-function-declaration'
*/
#ifdef __STRICT_ANSI__
#undef __STRICT_ANSI__
#endif

#include <string.h>
#include <_ansi.h>

/* Support the routine __aeabi_memclr like bzero.  Can't alias
   to bzero because it's not defined in the same translation
   unit.  */
void
__aeabi_memclr (void *dest, size_t n)
{
  bzero (dest, n);
}

/* Support the alias versions of __aeabi_memclr which may assume
   memory alignment.  */
void
__aeabi_memclr4 (void *dest, size_t n)
	_ATTRIBUTE ((alias ("__aeabi_memclr")));
void
__aeabi_memclr8 (void *dest, size_t n)
	_ATTRIBUTE ((alias ("__aeabi_memclr")));
