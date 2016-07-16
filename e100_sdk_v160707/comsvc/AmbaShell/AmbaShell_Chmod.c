/**
 * system/src/ui/ambsh/chmod.c
 *
 * History:
 *    2005/11/10 - [Charles Chiou] created file
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

static inline void usage(AMBA_SHELL_ENV_s *pEnv , int Argc, char **Argv)
{
    AmbaShell_Print(pEnv , "Usage1: %s [+-R|+-A|+-S|+-H] [file]\n", Argv[0]);
    AmbaShell_Print(pEnv , "Usage2: %s -d [+-R|+-A|+-S|+-H] [dir] [file]\n", Argv[0]);
}

#define PLUS_R  0
#define MINUS_R 1
#define PLUS_A  2
#define MINUS_A 3
#define PLUS_S  4
#define MINUS_S 5
#define PLUS_H  6
#define MINUS_H 7

static inline int get_attrib_arg(char *arg)
{
    int plus = 0;

    if (arg[2] != '\0')
        return -1;

    switch (arg[0]) {
    case '+':
        plus = 1;
        break;
    case '-':
        plus = 0;
        break;
    default:
        return -2;
    }

    switch (arg[1]) {
    case 'R':
    case 'r':
        return plus ? PLUS_R : MINUS_R;
    case 'A':
    case 'a':
        return plus ? PLUS_A : MINUS_A;
    case 'S':
    case 's':
        return plus ? PLUS_S : MINUS_S;
    case 'H':
    case 'h':
        return plus ? PLUS_H : MINUS_H;
    }

    return -3;
}

static int do_chmod(AMBA_SHELL_ENV_s *pEnv , int Argc, char **Argv)
{
    int Rval;
    int i;
    AMBA_FS_STAT ff_stat;
    AMBA_FS_DTA ff_dta;
    UINT8 mask = 0xff;
    UINT8 orr = 0x00;
    int p;
    char *file;
    const char *fname;
    char tmp[AMBA_FS_NAME_LEN];
    char Buf[AMBA_FS_NAME_LEN];

    for (i = 1; i < Argc - 1; i++) {
        Rval = get_attrib_arg(Argv[i]);
        if (Rval < 0) {
            usage(pEnv , Argc, Argv);
            return -2;
        }

        switch (Rval) {
        case PLUS_R:
            orr  |=  AMBA_FS_ATTR_RDONLY;
            break;
        case MINUS_R:
            mask &= ~AMBA_FS_ATTR_RDONLY;
            break;
        case PLUS_A:
            orr  |=  AMBA_FS_ATTR_ARCH;
            break;
        case MINUS_A:
            mask &= ~AMBA_FS_ATTR_ARCH;
            break;
        case PLUS_S:
            orr  |=  AMBA_FS_ATTR_SYSTEM;
            break;
        case MINUS_S:
            mask &= ~AMBA_FS_ATTR_SYSTEM;
            break;
        case PLUS_H:
            orr  |=  AMBA_FS_ATTR_HIDDEN;
            break;
        case MINUS_H:
            mask &= ~AMBA_FS_ATTR_HIDDEN;
            break;
        }
    }

    AMBSH_CHKCWD();

    file = Argv[Argc - 1];
    p = AmbaUtility_FindRepeatSep(file);

    AmbaShell_Ascii2Unicode(file, Buf);

    for (Rval = AmbaFS_FirstDirEnt(Buf, AMBA_FS_ATTR_ALL, &ff_dta); Rval >= 0;
         Rval = AmbaFS_NextDirEnt(&ff_dta)) {

        if (ff_dta.Attribute & AMBA_FS_ATTR_VOLUME)
            continue;
        if (ff_dta.Attribute & AMBA_FS_ATTR_DIR)
            continue;

        fname = (ff_dta.LongName[0] != '\0') ? (char *)ff_dta.LongName :
                (char *)ff_dta.FileName;

        AmbaShell_Unicode2Ascii((char *)fname, Buf);
        strcpy((char *)fname, Buf);

        if (strcmp((const char *) fname, ".") == 0 ||
            strcmp((const char *) fname, "..") == 0)
            continue;

        if (p) {
            strncpy(tmp, file, p);
            tmp[p] = '\0';
            strcat(tmp, (const char *) fname);
        } else {
            strcpy(tmp, (const char *) fname);
        }

        AmbaShell_Ascii2Unicode(tmp, Buf);

        Rval = AmbaFS_Stat(Buf, &ff_stat);
        if (Rval < 0) {
            AmbaShell_Print(pEnv , "%s: '%s' %s!\n",
                            Argv[0],
                            tmp,
                            AmbaShell_GetFsError());
            return -3;
        }
        ff_stat.Attr &= mask;
        ff_stat.Attr |= orr;

        Rval = AmbaFS_Chmod(Buf, ff_stat.Attr);
        if (Rval < 0) {
            AmbaShell_Print(pEnv , "%s: '%s' %s!\n",
                            Argv[0],
                            tmp,
                            AmbaShell_GetFsError());
            return -4;
        }
    }

    return 0;
}

int do_chmoddir(AMBA_SHELL_ENV_s *pEnv , int Argc, char **Argv)
{
    int Rval;
    int i;
    UINT32 attr_add = 0;
    UINT32 attr_sub = 0;
    UINT32 add_cnt = 0;
    UINT32 sub_cnt = 0;
    char *dir;
    char *file;
    char DirBuf[512], FileBuf[512];


    for (i = 2; i < Argc - 2; i++) {
        Rval = get_attrib_arg(Argv[i]);
        if (Rval < 0) {
            usage(pEnv , Argc, Argv);
            return -2;
        }

        switch (Rval) {
        case PLUS_R:
            attr_add |= AMBA_FS_ATTR_RDONLY;
            break;
        case MINUS_R:
            attr_sub |= AMBA_FS_ATTR_RDONLY;
            break;
        case PLUS_A:
            attr_add |= AMBA_FS_ATTR_ARCH;
            break;
        case MINUS_A:
            attr_sub |= AMBA_FS_ATTR_ARCH;
            break;
        case PLUS_S:
            attr_add |= AMBA_FS_ATTR_SYSTEM;
            break;
        case MINUS_S:
            attr_sub |= AMBA_FS_ATTR_SYSTEM;
            break;
        case PLUS_H:
            attr_add |= AMBA_FS_ATTR_HIDDEN;
            break;
        case MINUS_H:
            attr_sub |= AMBA_FS_ATTR_HIDDEN;
            break;
        }
    }

    AMBSH_CHKCWD();

    Rval = 0;
    dir = Argv[Argc - 2];
    file = Argv[Argc - 1];

    AmbaShell_Ascii2Unicode(dir, DirBuf);
    AmbaShell_Ascii2Unicode(file, FileBuf);

    if (attr_add != 0) {
        attr_add |= AMBA_FS_MODE_ATTR_ADD;
        Rval = AmbaFS_ChmodDir(DirBuf, FileBuf, AMBA_FS_MODE_SEARCH_TREE | (UINT32)AMBA_FS_ATTR_ALL,
                               attr_add, &add_cnt);
        if (Rval != 0) {
            AmbaShell_Print(pEnv , "number of change: %d\n", add_cnt);
            goto done;
        }
    }
    if (attr_sub != 0) {
        attr_sub |= AMBA_FS_MODE_ATTR_SUB;
        Rval = AmbaFS_ChmodDir(DirBuf, FileBuf, AMBA_FS_MODE_SEARCH_TREE | (UINT32)AMBA_FS_ATTR_ALL,
                               attr_sub, &sub_cnt);
        if (Rval != 0) {
            AmbaShell_Print(pEnv , "number of change: %d\n", add_cnt);
            goto done;
        }
    }

done:
    return Rval;
}

int ambsh_chmod(AMBA_SHELL_ENV_s *pEnv , int Argc, char **Argv)
{
    int Rval = 0;

    if (Argc < 3 || Argc > 8) {
        usage(pEnv , Argc, Argv);
        return -1;
    }

    if (strncmp(Argv[1], "-d", 2) == 0) {
        if (Argc < 5) {
            usage(pEnv , Argc, Argv);
            return -1;
        }
        Rval = do_chmoddir(pEnv , Argc, Argv);
        if (Rval != 0) {
            AmbaShell_Print(pEnv , "do_chmoddir fail!\n");
        }
    } else {
        Rval = do_chmod(pEnv , Argc, Argv);
    }

    return Rval;
}

