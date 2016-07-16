/**
 * system/src/ui/ambsh/cleandir.c
 *
 * History:
 *    2007/03/02 - [Charles Chiou] created file
 *
 * Copyright (C) 2004-2007, Ambarella, Inc.
 *
 * All rights reserved. No Part of this file may be reproduced, stored
 * in a retrieval system, or transmitted, in any form, or by any means,
 * electronic, mechanical, photocopying, recording, or otherwise,
 * without the prior consent of Ambarella, Inc.
 */

#include    <stdio.h>
#include    <string.h>

#include    "AmbaDataType.h"
#include    "AmbaKAL.h"
#include    "AmbaShell.h"


#include    "AmbaFS_Def.h"
#include    "AmbaFS.h"

static inline void usage(AMBA_SHELL_ENV_s *env, int argc, char **argv)
{
    AmbaShell_Print(env, "Usage: %s [path] [pattern]\n", argv[0]);
}

int ambsh_cleandir(AMBA_SHELL_ENV_s *env, int argc, char **argv)
{
    int rval = 0;
    const char *path;
    const char *pattern = NULL;
    UINT32 mode;
    UINT32 count;

    if (argc != 3) {
        usage(env, argc, argv);
        return -1;
    }

    path = argv[1];
    if (argc > 2)
        pattern = argv[2];

    mode = AMBA_FS_MODE_SEARCH_TREE | (unsigned long) AMBA_FS_ATTR_ARCH;
    rval = AmbaFS_CleanDir(path, pattern, mode, &count);

    return rval;
}

