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

extern int AmbaUtility_StringToUINT32(const char *pString, UINT32 *pValue);
static inline void usage(AMBA_SHELL_ENV_s *pEnv, int argc, char **argv)
{
    AmbaShell_Print(pEnv, "Usage: hotboot [mode]\n");
}

int ambsh_hotboot(AMBA_SHELL_ENV_s *pEnv, int argc, char **argv)
{
    UINT32 val;

    if (argc != 2) {
        usage(pEnv, argc, argv);
        return -1;
    }

    AmbaUtility_StringToUINT32(argv[1], &val);

    AmbaShell_Print(pEnv, "hotbooting 0x%.8x...\r\n\r\n\r\n", val);
    AmbaSysHotboot(val);

    return 0;
}

