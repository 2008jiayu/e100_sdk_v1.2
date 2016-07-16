/**
 * @file src/app/connected/applib/inc/va/ApplibVideoAnal_StmpHdlr.h
 *
 * Header of VA Stamp Handler APIs
 *
 * History:
 *    2015/01/06 - [Bill Chou] created file
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

#ifndef APPLIB_VIDEO_ANAL_STMPHDLR_H_
#define APPLIB_VIDEO_ANAL_STMPHDLR_H_

#include <applib.h>
__BEGIN_C_PROTO__

/*************************************************************************
 * Video definitions
 ************************************************************************/
#ifdef CONFIG_APP_ARD
#undef ENABLE_VA_STAMP
#else
#define ENABLE_VA_STAMP
#endif

#define VA_STMPHDLR_LDWS    (0x00000001)
#define VA_STMPHDLR_FCWS    (0x00000002)
#define VA_STMPHDLR_FCMD    (0x00000004)
#define VA_STMPHDLR_LLWS    (0x00000008)
#define VA_STMPHDLR_MD      (0x00000010)

#define VA_STMPHDLR_CALIB   (0x00000100)

#define VA_STMPHDLR_KEEP_TIME   (2) // in second

/*************************************************************************
 * Stamp Handler Setting APIs
 ************************************************************************/

extern int AppLibVideoAnal_StmpHdlr_AddEvent(UINT32 VaEvent);
extern UINT32 AppLibVideoAnal_StmpHdlr_GetEventFlag(void);

__END_C_PROTO__
/**
 * @}
 */
#endif /* APPLIB_VIDEO_ANAL_STMPHDLR_H_ */

