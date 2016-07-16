/*-----------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaShell_RW.c
 *
 *  @Copyright      :: Copyright (C) 2014 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieVal system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Memory read/write shell command.
\*-----------------------------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "AmbaDataType.h"
#include "AmbaKAL.h"
#include "AmbaShell.h"
#include "AmbaHwIO.h"

static void usage_write(AMBA_SHELL_ENV_s *env, int argc, char **argv)
{
    AmbaShell_Print(env, "Usage: %s <addr> <val>\n", argv[0]);
}

static void usage_read(AMBA_SHELL_ENV_s *env, int argc, char **argv)
{
    AmbaShell_Print(env, "Usage: %s <addr>\n", argv[0]);
}

static int __ambsh_write(AMBA_SHELL_ENV_s *env, int argc, char **argv, int width)
{
    UINT32 addr, val;

    if (argc != 3) {
        usage_write(env, argc, argv);
        return -1;
    }

    addr = strtoul(argv[1], NULL, 0);
    val = strtoul(argv[2], NULL, 0);

    switch (width) {
    case 8:
        WriteByte(addr, (UINT8) val);
        return 0;
    case 16:
        WriteHalfWord(addr, (UINT16) val);
        return 0;
    case 32:
        WriteWord(addr, (UINT32) val);
        return 0;
    }

    return -4;
}

static int __ambsh_read(AMBA_SHELL_ENV_s *env, int argc, char **argv, int width)
{
    UINT32 addr;

    if (argc != 2) {
        usage_read(env, argc, argv);
        return -1;
    }

    addr = strtoul(argv[1], NULL, 0);

    switch (width) {
    case 8:
        AmbaShell_Print(env, "0x%.2x\n", ReadByte(addr));
        return 0;
    case 16:
        AmbaShell_Print(env, "0x%.4x\n", ReadHalfWord(addr));
        return 0;
    case 32:
        AmbaShell_Print(env, "0x%.8x\n", ReadWord(addr));
        return 0;
    }

    return -3;
}

int ambsh_writeb(AMBA_SHELL_ENV_s *env, int argc, char **argv)
{
    return __ambsh_write(env, argc, argv, 8);
}

int ambsh_writew(AMBA_SHELL_ENV_s *env, int argc, char **argv)
{
    return __ambsh_write(env, argc, argv, 16);
}

int ambsh_writel(AMBA_SHELL_ENV_s *env, int argc, char **argv)
{
    return __ambsh_write(env, argc, argv, 32);
}

int ambsh_readb(AMBA_SHELL_ENV_s *env, int argc, char **argv)
{
    return __ambsh_read(env, argc, argv, 8);
}

int ambsh_readw(AMBA_SHELL_ENV_s *env, int argc, char **argv)
{
    return __ambsh_read(env, argc, argv, 16);
}

int ambsh_readl(AMBA_SHELL_ENV_s *env, int argc, char **argv)
{
    return __ambsh_read(env, argc, argv, 32);
}

