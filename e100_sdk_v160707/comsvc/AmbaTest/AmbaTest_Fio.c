/**
 * system/src/fio/test/test.c
 *
 * History:
 *    2007/11/30 - [Charles Chiou] created file
 *
 * Copyright (C) 2004-2007, Ambarella, Inc.
 *
 * All rights reserved. No Part of this file may be reproduced, stored
 * in a retrieval system, or transmitted, in any form, or by any means,
 * electronic, mechanical, photocopying, recording, or otherwise,
 * without the prior consent of Ambarella, Inc.
 */

#include <stdio.h>
#include <string.h>

#include "AmbaDataType.h"
#include "AmbaKAL.h"

#define __TEST_IMPL__
#include "AmbaTest.h"

#define __TEST_DECL(name) extern int test_##name(AMBA_SHELL_ENV_s *, int, char **)
#define __TEST_ADD_CMD(name) AmbaTest_RegisterCommand(#name, test_##name)
#define __TEST_DEL_CMD(name) AmbaTest_UnRegisterCommand(#name, test_##name)

__TEST_DECL(nand);
__TEST_DECL(nand_op);
__TEST_DECL(nor_op);
__TEST_DECL(sd);
__TEST_DECL(fioprf_basic);
__TEST_DECL(fioprf_putimgs);
__TEST_DECL(fioprf_randvrfy);
__TEST_DECL(fioprf_lgfvrfy);
__TEST_DECL(fioprf_lgfcopy);
__TEST_DECL(fioprf_thruput);
__TEST_DECL(fioprf_cretree);
__TEST_DECL(fioprf_deltree);
__TEST_DECL(fioprf_boundary);
__TEST_DECL(fioprf_randseek);
__TEST_DECL(fioprf_lgfseek);
__TEST_DECL(fioprf_fixpat);
__TEST_DECL(fioprf_seeksave);
__TEST_DECL(fioprf_incseek);
__TEST_DECL(fioprf_parallel);
__TEST_DECL(fioprf_genfrag);
__TEST_DECL(paccess_time);
__TEST_DECL(chk_low_spd);
__TEST_DECL(fast_ecn);
__TEST_DECL(fioprf2_cinsert);
__TEST_DECL(fioprf2_performance_esol);
__TEST_DECL(fioprf_fsync);

__TEST_DECL(fioffs_basic);
__TEST_DECL(fioffs_randvrfy);
__TEST_DECL(fioffs_lgfvrfy);
__TEST_DECL(fioffs_uc);
__TEST_DECL(fioffs_thruput);
__TEST_DECL(fioffs_lgfseek);
__TEST_DECL(fioffs_randseek);
__TEST_DECL(fioffs_putimgs);
__TEST_DECL(fioffs_seeksave);
__TEST_DECL(fioffs_cretree);
__TEST_DECL(fioffs_deltree);
__TEST_DECL(fioffs_boundary);
__TEST_DECL(fioffs_fixpat);
__TEST_DECL(fioffs_lgfcopy);
__TEST_DECL(fioffs_cinsert);
__TEST_DECL(fioffs_incseek);
__TEST_DECL(fioffs_fsync);
__TEST_DECL(lkvfs);
__TEST_DECL(sd_shmoo);

void AmbaTest_FioAddCommands()
{
    //__TEST_ADD_CMD(nand);
    __TEST_ADD_CMD(nand_op);
    __TEST_ADD_CMD(nor_op);
    __TEST_ADD_CMD(sd);
    __TEST_ADD_CMD(fioprf_thruput);
    __TEST_ADD_CMD(fioprf_randvrfy);
    __TEST_ADD_CMD(sd_shmoo);
    __TEST_ADD_CMD(chk_low_spd);
    //__TEST_ADD_CMD(fioprf_basic);
    //__TEST_ADD_CMD(fioprf_putimgs);
    //__TEST_ADD_CMD(fioprf_lgfvrfy);
    //__TEST_ADD_CMD(fioprf_lgfcopy);
    //__TEST_ADD_CMD(fioprf_cretree);
    //__TEST_ADD_CMD(fioprf_deltree);
    //__TEST_ADD_CMD(fioprf_boundary);
    //__TEST_ADD_CMD(fioprf_randseek);
    //__TEST_ADD_CMD(fioprf_lgfseek);
    //__TEST_ADD_CMD(fioprf_fixpat);
    //__TEST_ADD_CMD(fioprf_seeksave);
    //__TEST_ADD_CMD(fioprf_incseek);
    //__TEST_ADD_CMD(fioprf_parallel);
    //__TEST_ADD_CMD(fioprf_genfrag);
    //__TEST_ADD_CMD(fioprf2_cinsert);
    //__TEST_ADD_CMD(fioprf2_performance_esol);
    //__TEST_ADD_CMD(fioprf_fsync);

    //__TEST_ADD_CMD(fioffs_randvrfy);
    //__TEST_ADD_CMD(fioffs_uc);
    //__TEST_ADD_CMD(fioffs_thruput);
    //__TEST_ADD_CMD(paccess_time);
    //__TEST_ADD_CMD(fast_ecn);
    //__TEST_ADD_CMD(lkvfs);
    //__TEST_ADD_CMD(fioffs_basic);
    //__TEST_ADD_CMD(fioffs_lgfseek);
    //__TEST_ADD_CMD(fioffs_randseek);
    //__TEST_ADD_CMD(fioffs_putimgs);
    //__TEST_ADD_CMD(fioffs_seeksave);
    //__TEST_ADD_CMD(fioffs_cretree);
    //__TEST_ADD_CMD(fioffs_deltree);
    //__TEST_ADD_CMD(fioffs_boundary);
    //__TEST_ADD_CMD(fioffs_fixpat);
    //__TEST_ADD_CMD(fioffs_lgfcopy);
    //__TEST_ADD_CMD(fioffs_lgfvrfy);
    //__TEST_ADD_CMD(fioffs_cinsert);
    //__TEST_ADD_CMD(fioffs_incseek);
    //__TEST_ADD_CMD(fioffs_fsync);
}


