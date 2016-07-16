/**
 * system/src/ui/ambsh/mv.c
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
#include    <ctype.h>

#include    "AmbaDataType.h"
#include    "AmbaKAL.h"
#include    "AmbaShell.h"
#include    "AmbaFS.h"

#define DEBUG_MSG(...)

int wcscmp(const wchar_t *s1, const wchar_t *s2);
char *wcsncpy(wchar_t *dest, const wchar_t *src, UINT32 n);
char *wcscat(wchar_t *dest, const wchar_t *src);
char *wcscpy(wchar_t *dest, const wchar_t *src);

static inline void usage(AMBA_SHELL_ENV_s *pEnv , int Argc, char **Argv)
{
    AmbaShell_Print(pEnv , "Usage: %s [from] [to]\n", Argv[0]);
}

char* AmbaFS_strncpy(char *dest, const char *src, unsigned int n)
{
    if (AmbaFS_GetCodeMode() == AMBA_FS_UNICODE)
        return (char *) wcsncpy((wchar_t *) dest, (const wchar_t *) src, n);
    else
        return strncpy(dest, src, n);
}


char* AmbaFS_strcpy(char *dest, const char *src)
{
    if (AmbaFS_GetCodeMode() == AMBA_FS_UNICODE)
        return (char *) wcscpy((wchar_t *) dest, (const wchar_t *) src);
    else
        return strcpy(dest, src);
}

char* AmbaFS_strcat(char *dest, const char *src)
{
    if (AmbaFS_GetCodeMode() == AMBA_FS_UNICODE)
        return (char *) wcscat((wchar_t *) dest, (const wchar_t *) src);
    else
        return strcat(dest, src);
}

int AmbaFS_strcmp(const char *s1, const char *s2)
{
    if (AmbaFS_GetCodeMode() == AMBA_FS_UNICODE)
        return wcscmp((const wchar_t *) s1, (const wchar_t *) s2);
    else
        return strcmp(s1, s2);
}

#define AMBA_FS_COPY_BUF_SIZE   (128 << 10)
int AmbaFS_Copy(const char *srcname, const char *dstname)
{
    int rval = 0;
    AMBA_FS_DTA ff_dta;
    char *tmp = NULL;
    char *tmp2 = NULL;
    char *buf = NULL;
    int ercd;
    int p, p2, w;
    AMBA_FS_FILE *fin = NULL;
    AMBA_FS_FILE *fout = NULL;
    int err = 0;
    char *ff_filename = NULL;

    ercd = AmbaShell_MemAlloc((void **)&buf, AMBA_FS_COPY_BUF_SIZE);
    if (ercd != OK) {
        DEBUG_MSG("AmbaFS_Copy: out of memory!\n");
        err = AMBA_FS_INTERNAL_ERROR;
        goto done;
    }

    ercd = AmbaShell_MemAlloc((void **)&tmp, AMBA_FS_COPY_BUF_SIZE);
    if (ercd != OK) {
        DEBUG_MSG("AmbaFS_Copy: out of memory!\n");
        err = AMBA_FS_INTERNAL_ERROR;
        goto done;
    }

    ercd = AmbaShell_MemAlloc((void **)&tmp2, AMBA_FS_COPY_BUF_SIZE);
    if (ercd != OK) {
        DEBUG_MSG("AmbaFS_Copy: out of memory!\n");
        err = AMBA_FS_INTERNAL_ERROR;
        goto done;
    }

    p = AmbaUtility_FindRepeatSep(srcname);
    w = AmbaUtility_FindWild(srcname);
    p2 = AmbaUtility_FindRepeatSep(dstname);

    for (rval = AmbaFS_FirstDirEnt(srcname, AMBA_FS_ATTR_ALL, &ff_dta); rval >= 0;
         rval = AmbaFS_NextDirEnt(&ff_dta)) {

        if (ff_dta.Attribute & AMBA_FS_ATTR_VOLUME)
            continue;
        if (ff_dta.Attribute & AMBA_FS_ATTR_DIR)
            continue;

        if (ff_dta.LongName[0]) {
            ff_filename = ff_dta.LongName;
        } else {
            ff_filename = ff_dta.FileName;
        }

        memset(tmp, 0, 512);
        if (p) {
            AmbaFS_strncpy(tmp, srcname, p);
            AmbaFS_strcat(tmp, (const char *) ff_filename);
        } else {
            AmbaFS_strcpy(tmp, (const char *) ff_filename);
        }

        if (w >= 0) {
            if (p2) {
                AmbaFS_strncpy(tmp2, dstname, p2);
                tmp2[p2] = '\0';
                AmbaFS_strcat(tmp2, (const char *) ff_filename);
            } else {
                AmbaFS_strcpy(tmp2, dstname);
                AmbaFS_strcat(tmp2, "\\");
                AmbaFS_strcat(tmp2, (const char *) ff_filename);
            }
        } else {
            if (AmbaFS_strcmp(dstname, ".") == 0 ||
                AmbaFS_strcmp(dstname, "..") == 0) {
                AmbaFS_strcpy(tmp2, dstname);
                AmbaFS_strcat(tmp2, "\\");
                AmbaFS_strcat(tmp2, (const char *) ff_filename);
            } else {
                AmbaFS_strcpy(tmp2, dstname);
            }
        }

        fin = AmbaFS_fopen(tmp, "r");
        if (fin == NULL) {
            DEBUG_MSG("AmbaFS_Copy: open file failed: %s", tmp);
            err = AMBA_FS_INTERNAL_ERROR;
            goto done;
        }

        fout = AmbaFS_fopen(tmp2, "w");
        if (fout == NULL) {
            DEBUG_MSG("AmbaFS_Copy: open file failed: %s", tmp2);
            err = AMBA_FS_INTERNAL_ERROR;
            goto done;
        }

        /* Loop for: read from srcname, and write to dstname */
        do {
            rval = AmbaFS_fread(buf, 1, AMBA_FS_COPY_BUF_SIZE, fin);
            if (rval > 0) {
                rval = AmbaFS_fwrite(buf, 1, rval, fout);
                if (rval <= 0) {
                    DEBUG_MSG("AmbaFS_Copy: fwrite failed: %d", rval);
                    err = AMBA_FS_INTERNAL_ERROR;
                }
            }
        } while (rval > 0);

        if (fin) {
            if (AmbaFS_fclose(fin) < 0)
                err = AMBA_FS_INTERNAL_ERROR;
            else
                fin = NULL;
        }
        if (fout) {
            if (AmbaFS_fclose(fout) < 0)
                err = AMBA_FS_INTERNAL_ERROR;
            else
                fout = NULL;
        }
        if (w < 0)
            break;
    }
done:
    if (buf) {
        ercd = AmbaShell_MemFree(buf);
    }

    if (tmp) {
        ercd = AmbaShell_MemFree(tmp);
    }

    if (tmp2) {
        ercd = AmbaShell_MemFree(tmp2);
    }

    if (fin) {
        if (AmbaFS_fclose(fin) < 0)
            err = AMBA_FS_INTERNAL_ERROR;
    }
    if (fout) {
        if (AmbaFS_fclose(fout) < 0)
            err = AMBA_FS_INTERNAL_ERROR;
    }

    return err;
}

int AMBA_FS_remove_wildcard(const char *filename)
{
    int rval;
    AMBA_FS_DTA ff_dta;
    int p;
    char tmp[AMBA_FS_NAME_LEN];
    int count = 0;
    int err = 0;

    p = AmbaUtility_FindRepeatSep(filename);

    for (rval = AmbaFS_FirstDirEnt(filename, AMBA_FS_ATTR_ALL, &ff_dta); rval >= 0;
         rval = AmbaFS_NextDirEnt(&ff_dta)) {

        if (ff_dta.Attribute & AMBA_FS_ATTR_VOLUME)
            continue;
        if (ff_dta.Attribute & AMBA_FS_ATTR_DIR)
            continue;

        if (p) {
            strncpy(tmp, filename, p);
            tmp[p] = '\0';
            if ((strlen(tmp) + strlen(ff_dta.FileName)) < sizeof(tmp))
                strcat(tmp, (const char *) ff_dta.FileName);
            else
                err = AMBA_FS_INTERNAL_ERROR;
        } else {
            if (strlen(ff_dta.FileName) < sizeof(tmp))
                strcpy(tmp, (const char *) ff_dta.FileName);
            else
                err = AMBA_FS_INTERNAL_ERROR;
        }

        rval = AmbaFS_remove(tmp);
        if (rval < 0) {
            DEBUG_MSG("AmbaFS_remove_wildcard: AmbaFS_remove failed");
            err = AMBA_FS_INTERNAL_ERROR;
        }

        count++;
    }

    if (count == 0) {
        DEBUG_MSG("AMBA_FS_remove_wildcard: %s not found!\n", filename);
        /* It is not a error case if no file removed. */
        /* err = AMBA_FS_INTERNAL_ERROR; */
    }

    return err;
}


int AmbaFS_MoveWildcard(const char *srcname, const char *dstname)
{
    int rval;
    int w1, w2, p2;

    w1 = AmbaUtility_FindWild(srcname);
    w2 = AmbaUtility_FindWild(dstname);
    p2 = AmbaUtility_FindRepeatSep(dstname);

    if (w2 >= 0) {
        DEBUG_MSG("[to] '%s' is wild!\n", dstname);
        rval = AMBA_FS_INTERNAL_ERROR;
        goto done;
    }

    /* Move operation (copy ; remove) */
    if (w1 >= 0 || p2 || dstname[0] == '.') {
        rval = AmbaFS_Copy(srcname, dstname);
        if (rval == 0)
            rval = AMBA_FS_remove_wildcard(srcname);

        if (rval < 0)
            rval = AMBA_FS_INTERNAL_ERROR;
        goto done;
    }

    /* Rename operation */
    rval = AmbaFS_rename(srcname, dstname);
    if (rval < 0) {
        DEBUG_MSG("pf1_move: AMBA_FS_rename failed");
        rval = AMBA_FS_INTERNAL_ERROR;
        goto done;
    }

done:
    return rval;
}

int ambsh_mv(AMBA_SHELL_ENV_s *pEnv , int Argc, char **Argv)
{
    int Rval;
    AMBA_FS_DTA ff_dta;
    char tmp[AMBA_FS_NAME_LEN];
    char tmp2[AMBA_FS_NAME_LEN];
    char *src = NULL;
    char *dst = NULL;
    int p, p2, w;
    int src_drive, dst_drive;
    int count = 0;
    const char* fname;

    if (Argc != 3) {
        usage(pEnv , Argc, Argv);
        Rval = -1;
        goto done;
    }

    AMBSH_CHKCWD();

    src = Argv[1];
    p = AmbaUtility_FindRepeatSep(src);
    w = AmbaUtility_FindWild(src);

    dst = Argv[2];
    if (strlen(dst) > sizeof(tmp2)) {
        AmbaShell_Print(pEnv , "%s: string is too long\n", Argv[2]);
        return -2;
    }

    p2 = AmbaUtility_FindRepeatSep(dst);

    if (src[1] == ':')
        src_drive = tolower((int) src[0]);
    else
        src_drive = tolower((int) pEnv ->Cwd[0]);

    if (dst[1] == ':')
        dst_drive = tolower((int) dst[0]);
    else
        dst_drive = tolower((int) pEnv ->Cwd[0]);

    /* Move operation (copy ; remove) */
    if (src_drive != dst_drive) {
        Rval = AmbaFS_MoveWildcard(src, dst);
        if (Rval < 0)
            AmbaShell_Print(pEnv , "%s: %s!\n", Argv[0], AmbaShell_GetFsError());
        return Rval;
    }

    for (Rval = AmbaFS_FirstDirEnt(src, AMBA_FS_ATTR_ALL, &ff_dta); Rval >= 0;
         Rval = AmbaFS_NextDirEnt(&ff_dta)) {

        if (ff_dta.Attribute & AMBA_FS_ATTR_VOLUME)
            continue;
        if (ff_dta.Attribute & AMBA_FS_ATTR_DIR)
            continue;

        fname = (ff_dta.LongName[0] != '\0') ? (char *)ff_dta.LongName :
                (char *)ff_dta.FileName;
        if (p) {
            strncpy(tmp, src, p);
            tmp[p] = '\0';
            strcat(tmp, (const char *) fname);
        } else {
            strcpy(tmp, (const char *) fname);
        }

        if (w >= 0) {
            if (p2) {
                strncpy(tmp2, dst, p2);
                tmp2[p2] = '\0';
                strcat(tmp2, (const char *) fname);
            } else {
                strcpy(tmp2, dst);
                strcat(tmp2, "\\");
                strcat(tmp2, (const char *) fname);
            }
        } else {
            if (strcmp(dst, ".") == 0 ||
                strcmp(dst, "..") == 0) {
                strcpy(tmp2, dst);
                strcat(tmp2, "\\");
                strcat(tmp2, (const char *) fname);
            } else {
                strcpy(tmp2, dst);
            }
        }

        Rval = AmbaFS_Move(tmp, tmp2);
        if (Rval < 0) {
            AmbaShell_Print(pEnv, "%s: %s!\n", Argv[0], AmbaShell_GetFsError());
            return -2;
        }
        count++;
    }

    if (count == 0) {
        AmbaShell_Print(pEnv , "%s: %s not found!\n", Argv[0], Argv[1]);
        return -3;
    }

done:

    return 0;
}

