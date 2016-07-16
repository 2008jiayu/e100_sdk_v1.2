/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaShell_Reboot.c
 *
 *  @Copyright      :: Copyright (C) 2013 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: RCT (other than PLL) CSL
\*-------------------------------------------------------------------------------------------------------------------*/

#include    <stdio.h>
#include    <string.h>

#include    "AmbaDataType.h"
#include    "AmbaKAL.h"
#include    "AmbaShell.h"
#include    "AmbaSysCtrl.h"


static inline void usage(AMBA_SHELL_ENV_s *pEnv, int argc, char **argv)
{
    AmbaShell_Print(pEnv, "Usage: reboot [yes]\n");
}

int ambsh_reboot(AMBA_SHELL_ENV_s *pEnv, int argc, char **argv)
{
    if (argc != 2) {
        usage(pEnv, argc, argv);
        return -1;
    }

    if (strcmp(argv[1], "yes") != 0) {
        return 0;
    }

    AmbaShell_Print(pEnv, "rebooting...\r\n\r\n\r\n");

    AmbaSysSoftReset();

    return 0;
}

