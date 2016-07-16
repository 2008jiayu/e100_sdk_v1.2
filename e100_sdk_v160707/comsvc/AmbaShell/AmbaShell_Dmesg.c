/*-----------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaShell_Dmesg.c
 *
 *  @Copyright      :: Copyright (C) 2014 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieVal system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: dmesg (display message or driver message) shell command.
\*-----------------------------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "AmbaDataType.h"
#include "AmbaKAL.h"
#include "AmbaShell.h"
#include "AmbaPrintk.h"

extern int vsprintf(char *, const char *, va_list);
extern int sprintf(char *, const char *fmt, ...);

static void Usage(AMBA_SHELL_ENV_s *pEnv, int Argc, char **Argv)
{
    AmbaShell_Print(pEnv, "Usage:\n", Argv[0]);
    AmbaShell_Print(pEnv, "%s [rtos|dsp] [on|off] - RTOS message ON/OFF\n", Argv[0]);
    AmbaShell_Print(pEnv, "%s [rtos|dsp|dsp_debug] -[num lines] - Print debug messages\n", Argv[0]);

}

static int DoPrintDsp(AMBA_SHELL_ENV_s *pEnv, AMBA_DSP_PRINTF_s *dsppf,
                      UINT32 CodeAddr, UINT32 MemdAddr, UINT32 MdxfAddr)
{

    static char Buf[1024]__attribute__((section("no_init")));
    int l;

    l = AmbaPrintk_PrintfDspMsgToBuf(Buf, dsppf, CodeAddr, MemdAddr, MdxfAddr);

    /* Process the EOL characters */
    if (Buf[l - 1] != '\n')
        Buf[l++] = '\n';
    Buf[l] = '\0';

    AmbaShell_Write(pEnv, (const UINT8 *) Buf, l);

    return 0;
}

static int ambsh_dmesg_console_enable(AMBA_SHELL_ENV_s *pEnv, int RtosMsgEnable, int DspMsgEnable)
{
    int RetVal;

    RetVal = AmbaPrint_SetConfig(RtosMsgEnable, DspMsgEnable);
    if (RetVal != OK) {
        AmbaShell_Print(pEnv, "Failed to set message ON/OFF\n");
        return RetVal;
    }

    if (RtosMsgEnable > 0)
        AmbaShell_Print(pEnv, "RTOS message: ON\n");
    else if (RtosMsgEnable == 0)
        AmbaShell_Print(pEnv, "RTOS message: OFF\n");

    if (DspMsgEnable > 0)
        AmbaShell_Print(pEnv, "DSP message: ON\n");
    else if (DspMsgEnable == 0)
        AmbaShell_Print(pEnv, "DSP message: OFF\n");

    return RetVal;
}


int ambsh_dmesg(AMBA_SHELL_ENV_s *pEnv, int Argc, char **Argv)
{
    int Rval = 0;
    int rtos = 0;
    int dsp = 0;
    int DspDebug = 0;
    UINT32 nl = 0;
    union {
        AMBA_PRINTK_RTOS_LAST_MSG_PARAM_s r;
        AMBA_PRINTK_DSP_LAST_MSG_PARAM_s d;
    } param;
    AMBA_DSP_PRINTF_s *dsppf;

    if (Argc != 3) {
        Usage(pEnv, Argc, Argv);
        return -1;
    }

    if (strcmp(Argv[1], "rtos") == 0) {
        rtos = 1;
    } else if (strcmp(Argv[1], "dsp") == 0) {
        dsp = 1;
    } else if (strcmp(Argv[1], "dsp_debug") == 0) {
        DspDebug = 1;
    } else {
        Usage(pEnv, Argc, Argv);
        return -1;
    }

    /* Enable/Disable RTOS message */
    if (rtos) {
        if (strcmp(Argv[2], "on") == 0)
            return ambsh_dmesg_console_enable(pEnv, 1, -1);
        else if (strcmp(Argv[2], "off") == 0)
            return ambsh_dmesg_console_enable(pEnv, 0, -1);
    } else if (dsp) {
        if (strcmp(Argv[2], "on") == 0)
            return ambsh_dmesg_console_enable(pEnv, -1, 1);
        else if (strcmp(Argv[2], "off") == 0)
            return ambsh_dmesg_console_enable(pEnv, -1, 0);
    }

    if (Argv[2][0] != '-') {
        Usage(pEnv, Argc, Argv);
        return -2;
    }

    nl = strtoul(((const char *) Argv[2] + 1), NULL, 0);

    if (rtos) {
        Rval = AmbaPrintk_GetRtosLastMsg(nl, &param.r);
        if (Rval > 0) {
            if (param.r.Head > param.r.Tail) {
                AmbaShell_Write(pEnv, (const UINT8 *) param.r.Head,
                                param.r.AbsEnd -
                                param.r.Head);
                AmbaShell_Write(pEnv, (const UINT8 *) param.r.AbsStart,
                                param.r.Tail -
                                param.r.AbsStart);
            } else {
                AmbaShell_Write(pEnv, (const UINT8 *) param.r.Head,
                                param.r.Tail -
                                param.r.Head);
            }
        }
    }

    if (dsp) {
        Rval = AmbaPrintk_GetDspLastMsg(nl, &param.d);
        if (Rval > 0) {
            if (param.d.Head > param.d.Tail) {
                for (dsppf = param.d.Head;
                     dsppf <= param.d.AbsEnd;
                     dsppf = (AMBA_DSP_PRINTF_s *)
                             ((UINT32) dsppf + sizeof(*dsppf)))
                    DoPrintDsp(pEnv, dsppf,
                               param.d.CodeAddr,
                               param.d.MemdAddr,
                               param.d.MdxfAddr);
                for (dsppf = param.d.AbsStart;
                     dsppf <= param.d.Tail;
                     dsppf = (AMBA_DSP_PRINTF_s *)
                             ((UINT32) dsppf + sizeof(*dsppf)))
                    DoPrintDsp(pEnv, dsppf,
                               param.d.CodeAddr,
                               param.d.MemdAddr,
                               param.d.MdxfAddr);
            } else {
                for (dsppf = param.d.Head;
                     dsppf <= param.d.Tail; ) {
                    DoPrintDsp(pEnv, dsppf,
                               param.d.CodeAddr,
                               param.d.MemdAddr,
                               param.d.MdxfAddr);
                    dsppf = (AMBA_DSP_PRINTF_s *)
                            ((UINT32) dsppf + sizeof(*dsppf));
                }
            }
        }
    }

    if (DspDebug) {
        Rval = AmbaPrintk_GetDspLastMsg(nl, &param.d);
        if (Rval > 0) {
            for (dsppf = param.d.AbsStart;
                 dsppf <= param.d.AbsEnd; ) {
                DoPrintDsp(pEnv, dsppf,
                           param.d.CodeAddr,
                           param.d.MemdAddr,
                           param.d.MdxfAddr);
                dsppf = (AMBA_DSP_PRINTF_s *)
                        ((UINT32) dsppf + sizeof(*dsppf));
            }
        }
    }

    return 0;
}
