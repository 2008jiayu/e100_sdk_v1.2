/*-----------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaShell_Sleep.c
 *
 *  @Copyright      :: Copyright (C) 2014 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieVal system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Delay shell command.
\*-----------------------------------------------------------------------------------------------*/

#include <stdio.h>
#include <string.h>

#include "AmbaDataType.h"
#include "AmbaKAL.h"
#include "AmbaShell.h"

static inline void usage(AMBA_SHELL_ENV_s *env, int argc, char **argv)
{
    AmbaShell_Print(env, "Usage: %s [sec]\n", argv[0]);
}

int ambsh_sleep(AMBA_SHELL_ENV_s *env, int argc, char **argv)
{
    int t;

    if (argc != 2) {
        usage(env, argc, argv);
        return -1;
    }

    t = atol(argv[1]);
    AmbaKAL_TaskSleep(t * 1000);

    return 0;
}

