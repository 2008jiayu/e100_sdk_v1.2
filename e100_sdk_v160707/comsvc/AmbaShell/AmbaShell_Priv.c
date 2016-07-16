/**
 * system/src/ui/ambsh_priv.c
 *
 * Simple shell.
 *
 * History:
 *    2005/11/22 - [Charles Chiou] created file
 *
 * Copyright (C) 2004-2005, Ambarella, Inc.
 *
 * All rights reserved. No Part of this file may be reproduced, stored
 * in a retrieval system, or transmitted, in any form, or by any means,
 * electronic, mechanical, photocopying, recording, or otherwise,
 * without the prior consent of Ambarella, Inc.
 */

#include <stdio.h>
#include <stdarg.h>

#include <AmbaDataType.h>
#include "AmbaKAL.h"

#include "AmbaShell.h"
#include "AmbaShell_Priv.h"

extern int vsprintf(char *, const char *, va_list);

static int MyPrintf(AMBA_SHELL_ENV_s *env, char *fmt, ...)
{
    int l;
    char buf[1024];
    va_list args;

    va_start(args, fmt);
    l = vsprintf(buf, fmt, args);
    va_end(args);
    if (env != NULL &&
        (env->WriteIo.Type & AMBSH_IO_CONSOLE) &&
        l > 0 && buf[l - 1] == '\n') {
        buf[l - 1] = '\r';
        buf[l] = '\n';
        l++;
    }
    buf[l++] = '\0';

    return AmbaShell_Write(env, (const UINT8 *) buf, l);
}

static inline void AmbaShell_Print_indent(AMBA_SHELL_ENV_s *env, int level)
{
    int i;

    for (i = 0; i < level; i++)
        MyPrintf(env, "\t");
}

int ambsh_cmd_print(AMBA_SHELL_ENV_s *env, AMBA_SHELL_CMD_s *this, int level)
{
    int i = 0;

    for (; this; this = this->pNext) {
        switch (this->Type) {
        case AMBSH_CMD_PROC:
            AmbaShell_Print_indent(env, level);
            if (this->u.Proc.Argc == 0 && this->u.Proc.pSubshell) {
                MyPrintf(env, "fork(\n");
                ambsh_cmd_print(env, this->u.Proc.pSubshell, level + 1);
                AmbaShell_Print_indent(env, level);
                MyPrintf(env, ")");
            } else {
                MyPrintf(env, "%s(", this->u.Proc.Argv[0]);
                for (i = 1; i < this->u.Proc.Argc; i++) {
                    if (i > 1)
                        MyPrintf(env, ",");
                    MyPrintf(env, "%s", this->u.Proc.Argv[i]);
                }
                MyPrintf(env, ")");
            }
            if (this->u.Proc.InFile[0] != '\0')
                MyPrintf(env, " < %s ", this->u.Proc.InFile);
            if (this->u.Proc.OutFile[0] != '\0')
                MyPrintf(env, " > %s ", this->u.Proc.OutFile);
            if (this->u.Proc.AppendFile[0] != '\0')
                MyPrintf(env, " >> %s ", this->u.Proc.AppendFile);
            if (this->u.Proc.Background)
                MyPrintf(env, " &");

            if (this->u.Proc.pPipe) {
                MyPrintf(env, " | ");
                ambsh_cmd_print(env, this->u.Proc.pPipe, level);
            } else
                MyPrintf(env, "\n");
            break;
        case AMBSH_CMD_IFC:
            AmbaShell_Print_indent(env, level);
            MyPrintf(env, "if\n");
            ambsh_cmd_print(env, this->u.IfClause.pTest, level + 1);
            AmbaShell_Print_indent(env, level);
            MyPrintf(env, "then\n");
            ambsh_cmd_print(env, this->u.IfClause.ifpart, level + 1);
            AmbaShell_Print_indent(env, level);
            if (this->u.IfClause.pElsePart) {
                MyPrintf(env, "else\n");
                ambsh_cmd_print(env, this->u.IfClause.pElsePart, level + 1);
                AmbaShell_Print_indent(env, level);
            }
            MyPrintf(env, "fi\n");
            break;
        case AMBSH_CMD_WHILE:
            AmbaShell_Print_indent(env, level);
            MyPrintf(env, "while\n");
            ambsh_cmd_print(env, this->u.WhileClause.pTest, level + 1);
            AmbaShell_Print_indent(env, level);
            MyPrintf(env, "do\n");
            ambsh_cmd_print(env, this->u.WhileClause.pAction, level + 1);
            AmbaShell_Print_indent(env, level);
            MyPrintf(env, "done\n");
            break;
        case AMBSH_CMD_UNTIL:
            AmbaShell_Print_indent(env, level);
            MyPrintf(env, "until\n");
            ambsh_cmd_print(env, this->u.UntilClause.pTest, level + 1);
            AmbaShell_Print_indent(env, level);
            MyPrintf(env, "do\n");
            ambsh_cmd_print(env, this->u.UntilClause.pAction, level + 1);
            AmbaShell_Print_indent(env, level);
            MyPrintf(env, "done\n");
            break;
        case AMBSH_CMD_LOOP:
            AmbaShell_Print_indent(env, level);
            MyPrintf(env, "loop\n");
            ambsh_cmd_print(env, this->u.LoopClause.pTest, level + 1);
            AmbaShell_Print_indent(env, level);
            MyPrintf(env, "do\n");
            ambsh_cmd_print(env, this->u.LoopClause.pAction, level + 1);
            AmbaShell_Print_indent(env, level);
            MyPrintf(env, "done\n");
            break;
        }
    }

    return 0;
}

const char *AmbaShell_GetEnv_var(AMBA_SHELL_ENV_s *env, const char *name)
{
    static const char *empty = "";
    int i;

    for (i = 0; i < AMBSH_VAR_SIZE; i++) {
        if (strcmp(name, env->var[i].name) == 0)
            return env->var[i].vstr;
    }

    return empty;
}

int ambsh_setenv_var(AMBA_SHELL_ENV_s *env, const char *name, const char *value,
                     int overwrite)
{
    int i, first;

    for (i = 0, first = -1; i < AMBSH_VAR_SIZE; i++) {
        if (first < 0 && env->var[i].name[0] == '\0')
            first = i;
        if (strcmp(name, env->var[i].name) == 0)
            break;
    }

    if (i < AMBSH_VAR_SIZE && overwrite) {
        strncpy(env->var[i].vstr, (const char *) value, sizeof(env->var[i].vstr));
        return 0;
    }

    if (i >= AMBSH_VAR_SIZE && first < AMBSH_VAR_SIZE && first >= 0) {
        strncpy(env->var[first].name, (const char *) name,
                sizeof(env->var[first].name));
        strncpy(env->var[first].vstr, (const char *) value,
                sizeof(env->var[first].vstr));
        return 0;
    }

    return -1;
}

void ambsh_unsetenv_var(AMBA_SHELL_ENV_s *env, const char *name)
{
    int i;

    for (i = 0; i < AMBSH_VAR_SIZE; i++) {
        if (strcmp(name, env->var[i].name) == 0) {
            env->var[i].name[0] = '\0';
            break;
        }
    }

}

int ambsh_subsvar_var(AMBA_SHELL_ENV_s *env, char *s, unsigned int size)
{
    int i, len;

    if (s == NULL)
        return -1;

    len = strlen(s);

    if (len < 2 || s[0] != '?')
        return -2;

    if (len == 2) {
#if 0
        /* Don't support these yet! */

        /* $@ */
        if (s[1] == '@') {

        }

        /* $* */
        if (s[1] == '*') {

        }
#endif

        /* $# */
        if (s[1] == '#') {
            snprintf(s, size, "%d", env->Argc);
            return 1;
        }

        /* $? */
        if (s[1] == '?') {
            snprintf(s, size, "%d", env->RetValue);
            return 1;
        }

        /* $$ */
        if (s[1] == '$') {
            snprintf(s, size, "%d", (int) env->TaskId);
            return 1;
        }
        /* $! */
        if (s[1] == '!') {
            AMBA_SHELL_ENV_s *pChild;

            if (env->pChild == NULL) {
                s[0] = '\0';
                return 0;
            }

            pChild = env->pChild;
            while (pChild->pChild != NULL)
                pChild = pChild->pChild;

            snprintf(s, size, "%d", (int) pChild->TaskId);
            return 1;
        }
    }

    /* ${arg #} */
    for (i = 0; i < MAX_CMD_ARGS; i++) {
        char match[8];

        sprintf(match, "%d", i);
        if (strcmp(s + 1, match) == 0) {
            strncpy(s, env->Argv[i], size);
            return 1;
        }
    }

    /* ${name} matched in the var list */
    for (i = 0; i < AMBSH_VAR_SIZE; i++) {
        if (strcmp(s + 1, env->var[i].name) == 0) {
            strncpy(s, env->var[i].vstr, size);
            return 1;
        }
    }

    return 0;
}

