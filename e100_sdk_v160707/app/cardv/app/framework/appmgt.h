/**
  * @file src/app/framework/appmgt.h
  *
  * Header of Application Management
  *
  * History:
  *    2013/07/09 - [Martin Lai] created file
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

#ifndef APP_APPMGT_H_
#define APP_APPMGT_H_

#include <framework/appdefines.h>

__BEGIN_C_PROTO__

/*************************************************************************
 * Application Structure Definitions
 ************************************************************************/
typedef struct _APP_APP_s_ {
    int Id;
    int Tier;
    int Parent;
    int Previous;
    int Child;
    UINT32 GFlags;
    UINT32 Flags;
    int (*Start)(void);
    int (*Stop)(void);
    int (*OnMessage)(UINT32 msg, UINT32 param1, UINT32 param2);
} APP_APP_s;

extern int AppAppMgt_Init(APP_APP_s **sysApps, int appMaxNum);
extern int AppAppMgt_Register(APP_APP_s *app);
extern int AppAppMgt_GetApp(int appId, APP_APP_s **app);
extern int AppAppMgt_GetCurApp(APP_APP_s **app);
extern int AppAppMgt_GetCurT1App(APP_APP_s **app);
extern int AppAppMgt_SwitchApp(int appId);

extern int AppAppMgt_CheckBusy(void);
extern int AppAppMgt_CheckIo(void);
extern int AppAppMgt_CheckIdle(void);

__END_C_PROTO__

#endif /* APP_APPMGT_H_ */
