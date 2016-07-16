/*-----------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaShell_Echo.c
 *
 *  @Copyright      :: Copyright (C) 2014 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieVal system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Echo shell command.
\*-----------------------------------------------------------------------------------------------*/

#include <stdio.h>
#include <string.h>

#include "AmbaDataType.h"
#include "AmbaKAL.h"
#include "AmbaShell.h"

int ambsh_echo(AMBA_SHELL_ENV_s *env, int Argc, char **Argv)
{
    int i;

    for (i = 1; i < Argc; i++) {
        if (i > 1)
            AmbaShell_Print(env, " ");
        AmbaShell_Print(env, Argv[i]);
    }
    AmbaShell_Print(env, "\n");
    return 0;
}

