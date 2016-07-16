#include <stdio.h>

#include "AmbaDataType.h"
#include "AmbaKAL.h"
#include "AmbaPrint.h"
#include "AmbaShell.h"
#include "AmbaTest.h"

int test_cpp(struct _AMBA_SHELL_ENV_s_ *env, int argc, char **argv)
{
//    AmbaPrint("cpp cmd: %s", argv[1]);
    {
        extern int cpp_test(void);

        cpp_test();
    }

    return 0;
}

int AmbaTest_cppAddCommands(void)
{
    AmbaPrint("Adding cpp tetsing commands");

    // hook command
    AmbaTest_RegisterCommand("cpp", test_cpp);

    return 0;
}


/* ------------------------------------------------------------------------- */

#ifdef __cplusplus
extern "C"
{
#endif

void *__dso_handle = NULL;

#ifdef __cplusplus
}
#endif

