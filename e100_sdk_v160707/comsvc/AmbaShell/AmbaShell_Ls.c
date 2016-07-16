/**
 * system/src/ui/ambsh/ls.c
 *
 * History:
 *    2005/11/10 - [Charles Chiou] created file
 *    2007/09/28 - [Charles Chiou] improved formatted output
 *
 * Copyright (C) 2004-2005, Ambarella, Inc.
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

#include    "AmbaFS.h"
#include    "AmbaUtility.h"

#define PROFILE_LS  0x2

static void ls_fast_search(char *path, char *name)
{
#if 0
    UINT32 reset;
    int AMBA_FS_get_cwd(char *);

    AMBA_FS_get_cwd(path);
    reset = (strcmp(AMBA_FS_get_search_path(), path)) ? 1 : 0;

    sprintf(path, "%s%s", path, "*.*");
    name = path;
    if (reset) {
        AMBA_FS_free_search_cache();
        AMBA_FS_set_search_pattern(0, 0);
        AMBA_FS_set_search_cache(path, 1);
    }
#endif
}

static int ls_current_dir(AMBA_SHELL_ENV_s *pEnv , int mode, char *name)
{
    static const char *month_str[] = {
        "Nul", "Jan", "Feb", "Mar", "Apr", "May", "Jun",
        "Jul", "Aug", "Sep", "Oct", "Nov", "Dec",
    };
    int Rval;
    AMBA_FS_DTA AMBA_FS_dta;
    int sec, min, hour;
    int day, month, year;
    UINT32 StartTime = 0, n = 0;
    char Buf[512];

    if (mode == PROFILE_LS) {
        AmbaUtility_GetTimeStart(&StartTime);
        AmbaShell_Print(pEnv , "start time = %d ms\n", StartTime);
    }

    AmbaShell_Ascii2Unicode(name, Buf);

    for (Rval = AmbaFS_FirstDirEnt(Buf, AMBA_FS_ATTR_ALL, &AMBA_FS_dta); Rval >= 0;
         Rval = AmbaFS_NextDirEnt(&AMBA_FS_dta)) {

        if (AMBA_FS_dta.Attribute & AMBA_FS_ATTR_VOLUME)
            continue;

        if (mode == PROFILE_LS) {
            n++;
            continue;
        }

        sec = (AMBA_FS_dta.Time & 0x1f) * 2;
        min = (AMBA_FS_dta.Time >> 5) & 0x3f;
        hour = (AMBA_FS_dta.Time >> 11) & 0x1f;
        day = AMBA_FS_dta.Date & 0x1f;
        month = (AMBA_FS_dta.Date >> 5) & 0xf;
        year = ((AMBA_FS_dta.Date >> 9) & 0x7f) + 1980;

        AmbaShell_Print(pEnv ,
                        "%c%c%c%c%c %s %2d %4d %.2d:%.2d:%.2d %12lld ",
                        (AMBA_FS_dta.Attribute & AMBA_FS_ATTR_DIR) ? 'd' : 'f',
                        (AMBA_FS_dta.Attribute & AMBA_FS_ATTR_RDONLY) ? 'r' : '-',
                        (AMBA_FS_dta.Attribute & AMBA_FS_ATTR_ARCH) ? 'a' : '-',
                        (AMBA_FS_dta.Attribute & AMBA_FS_ATTR_SYSTEM) ? 's' : '-',
                        (AMBA_FS_dta.Attribute & AMBA_FS_ATTR_HIDDEN) ? 'h' : '-',
                        month_str[month % 13], day, year,
                        hour, min, sec, AMBA_FS_dta.FileSize);
        if (AMBA_FS_dta.Attribute & AMBA_FS_ATTR_DIR)
            AmbaShell_Print(pEnv , "[");

        if (AMBA_FS_dta.LongName[0] != '\0') {
            AmbaShell_Unicode2Ascii(AMBA_FS_dta.LongName, Buf);
        } else {
            AmbaShell_Unicode2Ascii(AMBA_FS_dta.FileName, Buf);
        }
        AmbaShell_Print(pEnv, Buf);

        if (AMBA_FS_dta.Attribute & AMBA_FS_ATTR_DIR)
            AmbaShell_Print(pEnv , "]");
        AmbaShell_Print(pEnv , "\n");
    }

    if (mode == PROFILE_LS) {
        UINT32 EndTime;
        float f = AmbaUtility_GetTimeEnd(&StartTime, &EndTime);

        AmbaShell_Print(pEnv , "end time = %d ms\n", EndTime);
        AmbaShell_Print(pEnv , "%d files, total takes %f sec, each takes %f ms\n",
                        n, f / 1000, f / n);
    }

    return 0;
}

static int ls_recursive(AMBA_SHELL_ENV_s *pEnv , int mode, char *path, char *wild)
{
    int plen;
    int Rval;
    AMBA_FS_DTA dta;
    char* fname;
    char Buf[520];

    plen = strlen(path);
    AmbaShell_Print(pEnv , "\n%s:\n", path);
    strcat(path, wild);
    ls_current_dir(pEnv , mode, path);

    path[plen] = '*';
    path[plen + 1] = '\0';

    AmbaShell_Ascii2Unicode(path, Buf);

    for (Rval = AmbaFS_FirstDirEnt(Buf, AMBA_FS_ATTR_ALL, &dta); Rval >= 0;
         Rval = AmbaFS_NextDirEnt(&dta)) {

        if (!(dta.Attribute & AMBA_FS_ATTR_DIR))
            continue;

        fname = (dta.LongName[0] != '\0') ?
                (char *)dta.LongName :
                (char *)dta.FileName;

        AmbaShell_Unicode2Ascii(fname, Buf);
        strcpy(fname, Buf);

        if (fname[0] == '.' &&
            fname[1] == '\0')
            continue;
        if (fname[0] == '.' &&
            fname[1] == '.' &&
            fname[2] == '\0')
            continue;

        /* restore path to original */
        path[plen] = '\0';

        AmbaShell_Unicode2Ascii(dta.FileName, Buf);

        strcat(path, Buf);
        strcat(path, "/");
        ls_recursive(pEnv , mode, path, wild);
    }

    /* restore path to original */
    path[plen] = '\0';

    return 0;
}

int ambsh_ls(AMBA_SHELL_ENV_s *pEnv , int Argc, char **Argv)
{
    int Rval;
    char path[256];
    int mode = 0;
    char *name;
    static const char *wild = "*.*";

    AMBSH_CHKCWD();

    /* initialize */
    mode = 0;
    name = (char *) wild;

    if ((Argc > 2) && (strcmp(Argv[1], "-R") == 0) ) {
        strcpy(path, pEnv ->Cwd);
        ls_recursive(pEnv , 1, path, Argv[2]);
        return 0;
    } else if ((Argc == 2) && (strcmp(Argv[1], "-R") == 0) ) {
        strcpy(path, pEnv ->Cwd);
        ls_recursive(pEnv , 1, path, (char *) wild);
        return 0;
    } else if ((Argc > 2) && (strcmp(Argv[1], "-l") == 0) ) {
        mode = 1;
        name = Argv[2];
    } else if ((Argc == 2) && (strcmp(Argv[1], "-l") == 0) ) {
        mode = 1;
    } else if ((Argc > 2) && (strcmp(Argv[1], "-f") == 0) ) {
        name = Argv[2];
        ls_fast_search(path, name);
    } else if ((Argc == 2) && (strcmp(Argv[1], "-f") == 0) ) {
        ls_fast_search(path, name);
    } else if ((Argc > 2) && (strcmp(Argv[1], "-p") == 0) ) {
        mode = PROFILE_LS;
        name = Argv[2];
    } else if ((Argc == 2) && (strcmp(Argv[1], "-p") == 0) ) {
        mode = PROFILE_LS;
    } else if (Argc > 1) {
        name = Argv[1];
    }

    ls_current_dir(pEnv , mode, name);

    return 0;
}

