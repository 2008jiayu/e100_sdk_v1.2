/**
 *  @FileName       :: AmbaHiber_Test.c
 *
 *  @Description    :: Test/Reference code for AmbaHiber.
 *
 *  @History        ::
 *      Date        Name        Comments
 *      10/15/2013  Kerson     Created
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

#include <stdio.h>
#include <AmbaDataType.h>
#include <AmbaLink.h>
#include <AmbaKAL.h>
#include <AmbaShell.h>

/*============================================================================*\
 *                                                                            *
 *                           Test module Management                           *
 *                                                                            *
\*============================================================================*/
static void HiberShowBootType(AMBA_SHELL_ENV_s *pEnv , int Argc, char **Argv)
{
    switch (AmbaLink_BootType(5000)) {
    case AMBALINK_WARM_BOOT:
        AmbaPrint("Linux is warm boot.");
        break;
    case AMBALINK_HIBER_BOOT:
        AmbaPrint("Linux is hibernation boot.");
        break;
    case AMBALINK_COLD_BOOT:
    default:
        AmbaPrint("Linux is cold boot.");
    }
}

static void HiberTest_Suspend(AMBA_SHELL_ENV_s *pEnv, int Mode)
{
    AmbaPrint("AmbaIPC_LinkCtrlSuspendLinux(%d)");
    AmbaIPC_LinkCtrlSuspendLinux(Mode);

    if (AmbaIPC_LinkCtrlWaitSuspendLinux(5000) == OK)
        AmbaPrint("AmbaIPC_LinkCtrlWaitSuspendLinux(%d) done.", Mode);
}

static void HiberTest_Resume(AMBA_SHELL_ENV_s *pEnv, int Mode)
{
    AmbaPrint("AmbaIPC_LinkCtrlResumeLinux(%d)", Mode);
    AmbaIPC_LinkCtrlResumeLinux(Mode);

    if (AmbaIPC_LinkCtrlWaitResumeLinux(5000) == OK)
    	AmbaPrint("AmbaIPC_LinkCtrlWaitResumeLinux(%d) done.", Mode);
}

static inline void HiberTestUsage(AMBA_SHELL_ENV_s *pEnv , int Argc, char **Argv)
{
    AmbaPrint("Usage: t %s <command>, where command is:", Argv[0]);
    AmbaPrint("\tsuspend <suspend_mode>, 0: hiber to NAND, 1: hiber to RAM, 2: standby to RAM, 3: Sleep to RAM");
    AmbaPrint("\tresume  <suspend_mode>, 0: hiber to NAND, 1: hiber to RAM, 2: standby to RAM, 3: Sleep to RAM");
    AmbaPrint("\tstress  <suspend_mode> <count>, 0: hiber to NAND, 1: hiber to RAM, 2: standby to RAM, 3: Sleep to RAM");
    AmbaPrint("\twipeout");
    AmbaPrint("\tboottype");
}

static int AppLib_HiberTestEntry(AMBA_SHELL_ENV_s *pEnv, int Argc, char **Argv)
{
	extern int AmbaIPC_HiberWipeout(UINT32 Flag);

	int Rval = 0;
    UINT32 Count, i;

	if (Argc == 3 && strcmp(Argv[1], "suspend") == 0) {
		sscanf(Argv[2], "%d", &Rval);
        HiberTest_Suspend(pEnv, Rval);

	} else if (Argc == 3 && strcmp(Argv[1], "resume") == 0) {
		sscanf(Argv[2], "%d", &Rval);
        HiberTest_Resume(pEnv, Rval);

	} else if (Argc == 4 && strcmp(Argv[1], "stress") == 0) {
		sscanf(Argv[2], "%d", &Rval);
   		sscanf(Argv[3], "%d", &Count);

        for (i = 0; i < Count; i++) {
            AmbaPrint("===== stress loop %d =====", i);

            HiberTest_Suspend(pEnv, Rval);
            HiberTest_Resume(pEnv, Rval);
        }

	} else if (Argc == 2 && strcmp(Argv[1], "wipeout") == 0) {
		Rval = AmbaIPC_HiberWipeout(0);

	} else if (Argc == 2 && strcmp(Argv[1], "boottype") == 0) {
		HiberShowBootType(pEnv, Argc, Argv);

	} else {
		HiberTestUsage(pEnv, Argc, Argv);
		return -1;
	}

    return Rval;
}

/*----------------------------------------------------------------------------*\
 *  @RoutineName::  AmbaHiber_TestInit
 *
 *  @Description::  Init test module
 *
 *  @Return     ::
 *      int : OK(0)/NG(-1)
 *
\*----------------------------------------------------------------------------*/
int AppLib_HiberTestAdd(void)
{
    AmbaPrint("Adding Test command hiber: %s %d",__FUNCTION__, __LINE__);
    AmbaTest_RegisterCommand("hiber", AppLib_HiberTestEntry);

    return 0;
}
