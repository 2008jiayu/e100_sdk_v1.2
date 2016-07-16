 /**
  * @file src/app/framework/appdefines.h
  *
  * Application definition header file.
  *
  * History:
  *    2013/07/05 - [Martin Lai] created file
  *
  *
 * Copyright (c) 2015 Ambarella, Inc.
 *
 * This file and its contents (¡°Software¡±) are protected by intellectual property rights
 * including, without limitation, U.S. and/or foreign copyrights.  This Software is also the
 * confidential and proprietary information of Ambarella, Inc. and its licensors.  You may
 * not use, reproduce, disclose, distribute, modify, or otherwise prepare derivative
 * works of this Software or any portion thereof except pursuant to a signed license
 * agreement or nondisclosure agreement with Ambarella, Inc. or its authorized
 * affiliates.	In the absence of such an agreement, you agree to promptly notify and
 * return this Software to Ambarella, Inc.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF NON-
 * INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL AMBARELLA, INC. OR ITS AFFILIATES BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; COMPUTER FAILURE OR
 * MALFUNCTION; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  */

#ifndef APP_DEFINES_H_
#define APP_DEFINES_H_

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <AmbaDataType.h>
#include <mw.h>

__BEGIN_C_PROTO__

/*************************************************************************
 * Application Global Flags
 ************************************************************************/
#define APP_AFLAGS_INIT         (0x00000001)
#define APP_AFLAGS_START      (0x00000002)
#define APP_AFLAGS_READY     (0x00000004)
#define APP_AFLAGS_OVERLAP (0x00000008)
#define APP_AFLAGS_BUSY       (0x00000010)
#define APP_AFLAGS_IO           (0x00000020)
#define APP_AFLAGS_ERROR     (0x00000040)
#define APP_AFLAGS_POPUP     (0x00000080)

#define APP_ADDFLAGS(x, y)          ((x) |= (y))
#define APP_REMOVEFLAGS(x, y)    ((x) &= (~(y)))
#define APP_CHECKFLAGS(x, y)       ((x) & (y))

__END_C_PROTO__

#endif /* APP_DEFINES_H_ */
