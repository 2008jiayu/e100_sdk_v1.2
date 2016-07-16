/*-----------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaShell_Suspend.c
 *
 *  @Copyright      :: Copyright (C) 2014 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieVal system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: System suspend shell command.
\*-----------------------------------------------------------------------------------------------*/

#include <stdio.h>
#include <string.h>

#include "AmbaDataType.h"
#include "AmbaKAL.h"
#include "AmbaShell.h"
#include "AmbaPLL.h"
#include "AmbaGPIO.h"

extern int AmbaDSP_Suspend(void);

static inline void usage(AMBA_SHELL_ENV_s *env, int argc, char **argv)
{
    AmbaShell_Print(env, "Usage: %s [sr|standby]\n", argv[0]);
}

int ambsh_suspend(AMBA_SHELL_ENV_s *env, int argc, char **argv)
{
    UINT32 PowerDone;

    if (argc != 2) {
        usage(env, argc, argv);
        return -1;
    }

    if (strcmp(argv[1], "sr") == 0) {
        PowerDone = 1;
    } else if (strcmp(argv[1], "standby") == 0) {
        PowerDone = 0;
    } else {
        usage(env, argc, argv);
        return -1;
    }

    AmbaDSP_Suspend();
    AmbaPLL_SetStandbyMode(PowerDone, NULL, NULL);

    return 0;
}
