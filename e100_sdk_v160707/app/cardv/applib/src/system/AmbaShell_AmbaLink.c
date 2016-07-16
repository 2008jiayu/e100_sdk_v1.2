/**
 * @file app/connected/system/AmbaShell_AmbaLink.c
 *
 * Definitions of Run Time Support Library for NAND flash
 *
 * History:
 *    2013/07/30 - [Evan Chen] created file
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

#include    <stdio.h>
#include    <string.h>

#include    <AmbaDataType.h>
#include    <AmbaKAL.h>
#include    <AmbaLink.h>
#include    "AmbaShell.h"

#ifdef CONFIG_APP_AMBA_LINK
static inline void usage(AMBA_SHELL_ENV_s *pEnv , int Argc, char **Argv)
{
    AmbaShell_Print(pEnv,
                    "Usage: %s <command>, where command is:\n"
                    "\tboot\n"
                    "\thiber_return\n"
                    "\thiber_resume\n"
                    "\thiber_wipeout\n"
#if 0
                    "\tboot-halt\n"
                    "\tcmdline [new value]\n"
                    "\tqboot\n"
                    "\treboot\n"
                    "\tsuspend\n"
                    "\tresume\n"
                    "\twipeout\n"
                    "\tprintk\n"
                    "\tabwipeout\n"
                    "\tabresume\n"
                    "\tabreturn\n"
                    "\tabdump d\n"
                    "\twowlan [cut power]\n"
                    "\tturbo [0 or 1]\n"
                    "\tpriority 115\n",
#endif
                    , Argv[0]);
}

int ambsh_ambalink(AMBA_SHELL_ENV_s *env, int Argc, char **Argv)
{
    int Rval;

    if (Argc == 2 && strcmp(Argv[1], "boot") == 0) {
        extern int AmbaLink_Boot(void);
        Rval = AmbaLink_Boot();
    } else if (Argc == 2 && strcmp(Argv[1], "hiber_return") == 0) {
        int AmbaIPC_HiberReturn(void);
        AmbaIPC_HiberReturn();
    } else if (Argc == 2 && strcmp(Argv[1], "hiber_resume") == 0) {
        int AmbaIPC_HiberResume(UINT32 Flag);
        AmbaIPC_HiberResume(0);
        __SEV();      /* Will make following WFE clear event flag but not sleep */
    } else if (Argc == 2 && strcmp(Argv[1], "hiber_wipeout") == 0) {
        int AmbaIPC_HiberWipeout(UINT32 Flag);
        AmbaIPC_HiberWipeout(0);
    } else {
        usage(env, Argc, Argv);
        return -1;
    }
    return Rval;
}
#else
int ambsh_ambalink(AMBA_SHELL_ENV_s *env, int Argc, char **Argv) { return 0; }
#endif

