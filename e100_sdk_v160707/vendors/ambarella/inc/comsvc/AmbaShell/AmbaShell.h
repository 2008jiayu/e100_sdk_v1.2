/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaShell.h
 *
 *  @Copyright      :: Copyright (C) 2012 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Ambarella utilities.
\*-------------------------------------------------------------------------------------------------------------------*/

#ifndef _AMBA_SHELL_H_
#define _AMBA_SHELL_H_

#ifndef AMBSH_TASK_STACK_SIZE
#define AMBSH_TASK_STACK_SIZE   0x8000
#endif

#define MAX_CMD_ARGS    384
#define MAX_ARG_LEN     128
#define MAX_CWD_LEN     256

/**
 * Command list.
 */
typedef struct _AMBA_SHELL_CMD_s_ {
    int Type;
#define AMBSH_CMD_PROC  0
#define AMBSH_CMD_IFC   1
#define AMBSH_CMD_WHILE 2
#define AMBSH_CMD_UNTIL 3
#define AMBSH_CMD_LOOP  4

    union {
        /* Process */
        struct {
            char buf[MAX_CMD_ARGS][MAX_ARG_LEN];
            int Argc;                               /**< Arg length */
            char *Argv[MAX_CMD_ARGS];               /**< Arg list */
            int Background;                         /**< Background proc */
            char InFile[MAX_ARG_LEN];               /**< Input from file */
            char OutFile[MAX_ARG_LEN];              /**< Output to file */
            char AppendFile[MAX_ARG_LEN];           /**< Append to file */
            struct _AMBA_SHELL_CMD_s_ *pSubshell;   /**< Subshell */
            struct _AMBA_SHELL_CMD_s_ *pPipe;       /**< After '|' */
        } Proc;
        /* If-conditional */
        struct {
            struct _AMBA_SHELL_CMD_s_ *pTest;
            struct _AMBA_SHELL_CMD_s_ *ifpart;
            struct _AMBA_SHELL_CMD_s_ *pElsePart;
        } IfClause;
        /* While-loop */
        struct {
            struct _AMBA_SHELL_CMD_s_ *pTest;
            struct _AMBA_SHELL_CMD_s_ *pAction;
        } WhileClause;
        /* Until-loop */
        struct {
            struct _AMBA_SHELL_CMD_s_ *pTest;
            struct _AMBA_SHELL_CMD_s_ *pAction;
        } UntilClause;
        /* Loop times */
        struct {
            struct _AMBA_SHELL_CMD_s_ *pTest;
            struct _AMBA_SHELL_CMD_s_ *pAction;
        } LoopClause;

    } u;

    struct _AMBA_SHELL_CMD_s_ *pNext;   /**< Command after [;\n] */
} AMBA_SHELL_CMD_s;

#define AMBSH_VAR_SIZE  32

/**
 * A table of environment variables.
 */
typedef struct _AMBA_SHELL_VAR_s_ {
    char name[MAX_ARG_LEN];
    char vstr[MAX_ARG_LEN];
} AMBA_SHELL_VAR_s;

#define AMBSH_PIPE_SIZE 256

/**
 * A 'pipe' under ambsh.
 */
typedef struct _AMBA_SHELL_PIPE_s_ {
    AMBA_KAL_MSG_QUEUE_t    MsgQueue;
    UINT32                  MsgQueBuf[AMBSH_PIPE_SIZE];
} AMBA_SHELL_PIPE_s;

/**
 * Prompt history.
 */
typedef struct _AMBA_SHELL_HISTORY_s_ {
#define MAX_AMBSH_HISTORY 256
#define MAX_HISTORY_ENTRY 256
    char line[MAX_AMBSH_HISTORY][MAX_HISTORY_ENTRY];
    int on;     /**< enable/disable history save to storage */
    int head;
    int tail;
    int curr;
} AMBA_SHELL_HISTORY_s;

typedef int (*AMBA_CONSOLE_GET_CHAR_f)(void *, char *, int);
typedef int (*AMBA_CONSOLE_PUT_CHAR_f)(void *, char, int);
typedef int (*AMBA_CONSOLE_FLUSH_f)(void *);

/**
 * ambsh env.
 */
#define AMBSH_IO_NULL       0x0000
#define AMBSH_IO_CONSOLE    0x0001
#define AMBSH_IO_PIPE       0x0010
#define AMBSH_IO_FILE       0x0100
#define AMBSH_IO_FILE_CONT  0x0200
typedef struct _AMBA_SHELL_ENV_s_ {
    /** Stack */
    UINT8  Stack[AMBSH_TASK_STACK_SIZE];
    AMBA_KAL_TASK_t Task;
    UINT32 TaskId;

    UINT8  OriginalUnicodeMode;     /**< Task original uni-code mode */
    struct _AMBA_SHELL_ENV_s_ *pParent;
    struct _AMBA_SHELL_ENV_s_ *pChild;

    AMBA_SHELL_CMD_s *cmd;
    AMBA_SHELL_CMD_s *pSubshellCmd;
    char buf[MAX_CMD_ARGS][MAX_ARG_LEN];
    int Argc;           /**< Arg length */
    char *Argv[MAX_CMD_ARGS];   /**< Arg list */
    int Background;

    AMBA_SHELL_HISTORY_s *pHistory;
    AMBA_SHELL_VAR_s var[AMBSH_VAR_SIZE];
    int RetValue; /**< Return value of shell */
    char Cwd[MAX_CWD_LEN];  /**< Current working directory */

    void *pConsoleCtx;
    struct {
        int Type;
        AMBA_CONSOLE_GET_CHAR_f ConsoleGetCharCb;
        void *pFileInput;
        void *pPipe;
    } ReadIo;
    struct {
        int Type;
        AMBA_CONSOLE_PUT_CHAR_f ConsolePutCharCb;
        AMBA_CONSOLE_FLUSH_f ConsoleFlushCb;
        void *pFileOut;
        void *pFileAppend;
        void *pPipe;
    } WriteIo;
} AMBA_SHELL_ENV_s;

typedef int (*AMBA_SHELL_PROC_f)(AMBA_SHELL_ENV_s *pEnv, int Argc, char **Argv);
typedef int (*AMBA_SHELL_PUT_CHAR_f)(int TxDataSize, char *pTxDataBuf, UINT32 TimeOut);
typedef int (*AMBA_SHELL_GET_CHAR_f)(int RxDataSize, char *pRxDataBuf, UINT32 TimeOut);

typedef struct _AMBA_SHELL_CONFIG_s_ {
    int     Priority;                           /* Priority */
	UINT32  SmpCoreinclusionMap;
	AMBA_KAL_BYTE_POOL_t *pCachedHeap;
	AMBA_SHELL_PUT_CHAR_f ConsolePutCharCb;
	AMBA_SHELL_GET_CHAR_f ConsoleGetCharCb;
} AMBA_SHELL_CONFIG_s;

/*-----------------------------------------------------------------------------------------------*\
 * Defined in AmbaShell_Utility.c
\*-----------------------------------------------------------------------------------------------*/
int AmbaShell_MemAlloc(void **pBuf, UINT32 Size);
int AmbaShell_MemFree(void * pMemBase);
void AmbaShell_Ascii2Unicode(char *AsciiString, char *UnicodeString);
void AmbaShell_Unicode2Ascii(char *UnicodeString, char *AsciiString);

/*-----------------------------------------------------------------------------------------------*\
 * Defined in AmbaShell.c
\*-----------------------------------------------------------------------------------------------*/
void AmbaShell_Init(AMBA_SHELL_CONFIG_s *pShellConfig);
int  AmbaShell_Start(int NumAutoExecScript, char *pAutoExecScript[]);

/*
 * Services provided by ambsh
 */
AMBA_SHELL_ENV_s *AmbaShell_GetEnv(void);
AMBA_SHELL_PROC_f AmbaShell_FindProcImage(AMBA_SHELL_ENV_s *pEnv, const char *pName);
int AmbaShell_Print(AMBA_SHELL_ENV_s *pEnv, const char *fmt, ...);
int AmbaShell_Printf(const char *fmt, ...);
int AmbaShell_GetCmdStringFromPrompt(AMBA_SHELL_ENV_s *pEnv, char *s, int n, int Echo);
int AmbaShell_GetStringInteractive(AMBA_SHELL_ENV_s *pEnv, char *s, int n, int Echo, char Echochar);
int AmbaShell_Read(AMBA_SHELL_ENV_s *pEnv, UINT8 *buf, unsigned int len);
int AmbaShell_ReadTimeout(AMBA_SHELL_ENV_s *pEnv, UINT8 *buf, unsigned int len, int tmo);
int AmbaShell_Write(AMBA_SHELL_ENV_s *pEnv, const UINT8 *buf, unsigned int len);
int AmbaShell_Flush(AMBA_SHELL_ENV_s *pEnv);
int AmbaShell_AddCmd(const char *name, AMBA_SHELL_PROC_f Proc);
int AmbaShell_DeleteCmd(const char *name, AMBA_SHELL_PROC_f Proc);
const char *AmbaShell_GetFsError(void);
AMBA_KAL_BYTE_POOL_t *AmbaShell_GetHeap(void);

static inline int AmbaShell_EnvArgString(AMBA_SHELL_ENV_s *pEnv, char *s, int Len)
{
    int i, l = 0;

    if (pEnv->Argc == 0) {
        strcpy(s, "ambsh");
    } else {
        s[0] = '\0';
        for (i = 0, l = 0; i < pEnv->Argc && l < Len; i++) {
            strncat(s + l, pEnv->Argv[i], Len - l - 1);
            l = strlen(s);
            if (l + 1 < Len) {
                s[l++] = ' ';
                s[l] = '\0';
            }
        }
    }

    return l;
}

#define AMBSH_CHKCWD(...) {                         \
        char Buf[MAX_CWD_LEN];                      \
        char Path[MAX_CWD_LEN];                     \
        int l;                                      \
        \
        strcpy(Path, pEnv->Cwd);                    \
        l = strlen(Path);                           \
        if (Path[l - 1] == '\\' && l != 3) {        \
            Path[l - 1] = '\0';                     \
        }                                           \
        AmbaShell_Ascii2Unicode(Path, Buf);         \
        Rval = AmbaFS_Chdir(Buf);                   \
        if (Rval < 0) {                             \
            AmbaShell_Print(pEnv,                   \
                            "cwd '%s' is offline!\n",   \
                            pEnv->Cwd);                 \
            return -9999;                           \
        }                                           \
    }

typedef struct _AMBA_SHELL_LIST_s_ {
    const char  *pName;
    AMBA_SHELL_PROC_f  Proc;
    struct _AMBA_SHELL_LIST_s_ *pNext;
} AMBA_SHELL_LIST_s;

/*
 * Services provided by tclsh
 */
extern int AmbaUtility_FindWild(const char *s);
extern int AmbaUtility_FindRepeatSep(const char *s);
extern int is_abs_path(const char *s);

#endif

