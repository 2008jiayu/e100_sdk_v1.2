/*-----------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaShell_Help.c
 *
 *  @Copyright      :: Copyright (C) 2014 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieVal system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Shell command help.
\*-----------------------------------------------------------------------------------------------*/

#include <stdio.h>
#include <string.h>

#include "AmbaDataType.h"
#include "AmbaKAL.h"
#include "AmbaShell.h"

int ambsh_help(AMBA_SHELL_ENV_s *env, int Argc, char **Argv)
{
    extern AMBA_SHELL_LIST_s AmbaShell_DefaultCmdList[];
    AMBA_SHELL_LIST_s *cur;
    int i;

    AmbaShell_Print(env, "supported built-in commands:\n");

    for (cur = AmbaShell_DefaultCmdList, i = 0; cur != NULL; cur = cur->pNext, i++) {
        if (strlen(cur->pName) <= 7 )
            AmbaShell_Print(env, "\t%s\t", cur->pName);
        else
            AmbaShell_Print(env, "\t%s", cur->pName);
        if ((i % 4) == 3)
            AmbaShell_Print(env, "\n");
    }
    AmbaShell_Print(env, "\n");

    return 0;
}

