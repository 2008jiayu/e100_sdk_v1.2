/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaShell.c
 *
 *  @Copyright      :: Copyright (C) 2012 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Ambarella utilities.
\*-------------------------------------------------------------------------------------------------------------------*/

#include <stdio.h>
#include <string.h>

#include "AmbaDataType.h"
#include "AmbaKAL.h"
#include "AmbaFS.h"

#include "AmbaPrintk.h"

#include "AmbaShell.h"
#include "AmbaShell_Priv.h"

#ifndef AMBA_SHELL_NO_FS
#define AMBA_SHELL_ENABLE_HISTORY
#else
#undef AMBA_SHELL_ENABLE_HISTORY
#endif

#undef ENABLE_DEBUG_MSG_AMBSH

#ifdef ENABLE_DEBUG_MSG_AMBSH
#define     DEBUG_MSG AmbaPrint
#else
#define     DEBUG_MSG(...)
#endif

#define AMBSH_PROMPT_MSG                                                    \
    "*****************************************************************\n"   \
    "*                                                               *\n"   \
    "*                        AmbaShell ;)                           *\n"   \
    "*                                                               *\n"   \
    "*****************************************************************\n"   \
    "\n"

#define AMBSH_SIGNAL_EXIT   (-87654321)

extern void *MyMalloc(UINT32);
extern void MyFree(void *);

static char _AmbaShell_PromptBuf[8192]; /* buffer for VT100 parsed command */
static char _AmbaShell_RawBuf[128]; /* buffer for ctrl+c detection */
static int _AmbaShell_NumRawData;

static AMBA_SHELL_CONFIG_s AmbaShellConfig;

extern AMBA_SHELL_LIST_s AmbaShell_DefaultCmdList[];
extern UINT32 AmbaShell_DefaultCmdListSize;
AMBA_SHELL_ENV_s *_pAmbaShellRootProc = NULL;
AMBA_KAL_MUTEX_t _AmbaShell_EnvMutex;
AMBA_KAL_MUTEX_t _AmbaShell_CmdListMutex;

/* Forward declarations */
static AMBA_SHELL_ENV_s *SHELL_Create(AMBA_SHELL_ENV_s *pEnv, AMBA_SHELL_CMD_s *pCmd);
static void SHELL_Destroy(AMBA_SHELL_ENV_s *pEnv);
int AmbaShell_EnvTryFreeChild(AMBA_SHELL_ENV_s *pEnv);
int ambsh_env_force_kill_child(AMBA_SHELL_ENV_s *pEnv);
static int SHELL_WaitCmdProcDone(AMBA_SHELL_ENV_s *pEnv);
void AmbaShell_Prompt(AMBA_SHELL_ENV_s *pEnv);
AMBA_SHELL_PROC_f AmbaShell_FindProcImage(AMBA_SHELL_ENV_s *pEnv, const char *name);
static int SHELL_ExecuteCmdProc(AMBA_SHELL_ENV_s *pEnv, AMBA_SHELL_CMD_s *pCmd);
void AmbaShellTask(UINT32 exinf);

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: SHELL_ConsoleGetChar
 *
 *  @Description:: Get one char from console driver
 *
 *  @Input      ::
 *      pCtx:
 *      TimeOut:    timeout value
 *
 *  @Output     ::
 *      pChar:      output character
 *
 *  @Return     ::
 *          int : number of characters
\*-----------------------------------------------------------------------------------------------*/
static int SHELL_ConsoleGetChar(void *pCtx, char *pChar, int TimeOut)
{
    UINT32 NumChar = 0x1;

    if (AmbaShellConfig.ConsoleGetCharCb != NULL)
        return AmbaShellConfig.ConsoleGetCharCb(NumChar, pChar, (UINT32)TimeOut);

    return 0;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: SHELL_ConsolePutChar
 *
 *  @Description:: Put one char to console driver
 *
 *  @Input      ::
 *      pCtx:
 *      pChar:      output character
 *      TimeOut:    timeout value
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : number of characters
\*-----------------------------------------------------------------------------------------------*/
static int SHELL_ConsolePutChar(void *pCtx, char Char, int TimeOut)
{
    if (AmbaShellConfig.ConsolePutCharCb != NULL)
        return AmbaShellConfig.ConsolePutCharCb(1, &Char, (UINT32)TimeOut);

    return 0;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: SHELL_ConsoleFlush
 *
 *  @Description:: Flush i/o buffer of console driver
 *
 *  @Input      ::
 *      pCtx:
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static int SHELL_ConsoleFlush(void *pCtx)
{
    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: SHELL_IoRedirectInit
 *
 *  @Description:: Initialize I/O redirection
 *
 *  @Input      ::
 *      pShellEnv:  shell environment
 *      pCmd:       command arguments
 *
 *  @Output     ::
 *      pFileInput:     file pointer for input
 *      pFileOutput:    file pointer for output
 *      pFileAppend:    file pointer for append
 *      pPipe:          pipe pointer
 *
 *  @Return     ::
 *          AMBA_SHELL_ENV_s * : allocated shell control
\*-----------------------------------------------------------------------------------------------*/
static int SHELL_IoRedirectInit(AMBA_SHELL_ENV_s *pShellEnv, AMBA_SHELL_CMD_s *pCmd,
                                AMBA_FS_FILE **pFileInput, AMBA_FS_FILE **pFileOutput, AMBA_FS_FILE **pFileAppend, AMBA_SHELL_PIPE_s **pPipe)
{
#ifndef AMBA_SHELL_NO_FS
    char FileName[512];

    /* Is it a null device? */
    if (strcmp(pCmd->u.Proc.InFile, "/dev/null") == 0 ||
        strcmp(pCmd->u.Proc.InFile, "\\dev\\null") == 0 ||
        strcmp(pCmd->u.Proc.OutFile, "/dev/null") == 0  ||
        strcmp(pCmd->u.Proc.OutFile, "\\dev\\null") == 0 ||
        strcmp(pCmd->u.Proc.AppendFile, "/dev/null") == 0 ||
        strcmp(pCmd->u.Proc.AppendFile, "\\dev\\null") == 0) {
        return NG;
    }

    /* Open I/O redirect input file */
    if (pCmd->u.Proc.InFile[0] != '\0') {
        AmbaShell_Ascii2Unicode(pCmd->u.Proc.InFile, FileName);

        *pFileInput = AmbaFS_fopen(FileName, "r");
        if (*pFileInput == NULL) {
            AmbaShell_Print(pShellEnv, "cannot open '%s'!\n", pCmd->u.Proc.InFile);
        }
    }

    /* Open I/O redirect output file */
    if (pCmd->u.Proc.OutFile[0] != '\0') {
        AmbaShell_Ascii2Unicode(pCmd->u.Proc.OutFile, FileName);

        *pFileOutput = AmbaFS_fopen(FileName, "w");
        if (*pFileOutput == NULL) {
            AmbaShell_Print(pShellEnv, "cannot open '%s'!\n", pCmd->u.Proc.OutFile);
        }
    }

    /* Open I/O redirect append file */
    if (pCmd->u.Proc.AppendFile[0] != '\0') {
        AmbaShell_Ascii2Unicode(pCmd->u.Proc.AppendFile, FileName);

        *pFileAppend = AmbaFS_fopen(FileName, "a");
        if (*pFileAppend == NULL) {
            AmbaShell_Print(pShellEnv, "cannot open '%s'!\n", pCmd->u.Proc.AppendFile);
        }
    }

    /* Create pipe */
    if (pCmd->u.Proc.pPipe != NULL) {
        *pPipe = MyMalloc(sizeof(AMBA_SHELL_PIPE_s));
        if (*pPipe) {
            if (AmbaKAL_MsgQueueCreate(&(*pPipe)->MsgQueue, &(*pPipe)->MsgQueBuf,
                                       AMBSH_PIPE_SIZE << 2, AMBSH_PIPE_SIZE) != OK) {
                MyFree(pPipe);
                *pPipe = NULL;
            }
        }
    }
#endif

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: SHELL_IoRedirectExit
 *
 *  @Description:: Terminate I/O redirection
 *
 *  @Input      ::
 *      pShellEnv:  shell environment
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static void SHELL_IoRedirectExit(AMBA_SHELL_ENV_s *pShellEnv)
{
#ifndef AMBA_SHELL_NO_FS
    if (pShellEnv->ReadIo.pFileInput) {
        if (pShellEnv->ReadIo.Type & AMBSH_IO_FILE) {
            AmbaFS_fclose((AMBA_FS_FILE *) pShellEnv->ReadIo.pFileInput);
        }
        pShellEnv->ReadIo.pFileInput = NULL;
    }

    if (pShellEnv->ReadIo.pPipe) {
        /* Do nothing, because the parent closes the pipe! */
        pShellEnv->ReadIo.pPipe = NULL;
    }

    if (pShellEnv->WriteIo.pFileOut) {
        if (pShellEnv->WriteIo.Type & AMBSH_IO_FILE) {
            AmbaFS_fclose((AMBA_FS_FILE *) pShellEnv->WriteIo.pFileOut);
        }
        pShellEnv->WriteIo.pFileOut = NULL;
    }

    if (pShellEnv->WriteIo.pFileAppend) {
        if (pShellEnv->WriteIo.Type & AMBSH_IO_FILE) {
            AmbaFS_fclose((AMBA_FS_FILE *) pShellEnv->WriteIo.pFileAppend);
        }
        pShellEnv->WriteIo.pFileAppend = NULL;
    }

    if (pShellEnv->WriteIo.pPipe) {
        AMBA_SHELL_PIPE_s *pPipe = (AMBA_SHELL_PIPE_s *) pShellEnv->WriteIo.pPipe;
        AmbaKAL_MsgQueueDelete(&pPipe->MsgQueue);
        MyFree(pPipe);
        pShellEnv->WriteIo.pPipe = NULL;
    }
#endif
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: SHELL_Create
 *
 *  @Description:: Create a shell environment
 *
 *  @Input      ::
 *      pParentShell:   parent shell
 *      pCmd:           initial arguments for shell
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          AMBA_SHELL_ENV_s * : allocated shell control
\*-----------------------------------------------------------------------------------------------*/
static AMBA_SHELL_ENV_s *SHELL_Create(AMBA_SHELL_ENV_s *pParentShell, AMBA_SHELL_CMD_s *pCmd)
{
    AMBA_SHELL_ENV_s *pEnv = NULL;
    AMBA_FS_FILE *pFileInput = NULL, *pFileOutput = NULL, *pFileAppend = NULL;
    AMBA_SHELL_PIPE_s *pPipe = NULL;
    AMBA_SHELL_ENV_s *pCur;
    int i, NullDev = 0;
    char TaskString[64];

    /* Allocate a new environment */
    if (AmbaShell_MemAlloc((void *)&pEnv, sizeof(AMBA_SHELL_ENV_s)) != OK) {
        AmbaShell_Print(pParentShell, "out of memory!\n");
        return NULL;
    }

    if (pParentShell) {
        memcpy(pEnv, pParentShell, sizeof(AMBA_SHELL_ENV_s));
        memset(&pEnv->ReadIo, 0x0, sizeof(pEnv->ReadIo));
        memset(&pEnv->WriteIo, 0x0, sizeof(pEnv->WriteIo));
        memset(&pEnv->Task, 0x0, sizeof(AMBA_KAL_TASK_t));
    } else {
        memset(pEnv, 0x0, sizeof(AMBA_SHELL_ENV_s));
    }

    pEnv->pParent = pParentShell;
    pEnv->pChild = NULL;
    pEnv->RetValue = 0;

    if (pEnv->pParent)
        strcpy(pEnv->Cwd, pEnv->pParent->Cwd);
    else
        strcpy(pEnv->Cwd, "a:\\");

    pEnv->cmd = pCmd;
    if (pCmd) {
        pEnv->pSubshellCmd = pCmd->u.Proc.pSubshell;
        pEnv->Background = pCmd->u.Proc.Background;
        pEnv->Argc = pCmd->u.Proc.Argc;
    } else {
        pEnv->pSubshellCmd = NULL;
        pEnv->Background = 0;
        pEnv->Argc = 0;
    }

    /* assign argument buffers */
    for (i = 0; i < MAX_CMD_ARGS; i++)
        pEnv->Argv[i] = &pEnv->buf[i][0];

    for (i = 0; i < pEnv->Argc; i++) {
        strncpy(pEnv->Argv[i], pCmd->u.Proc.Argv[i], MAX_ARG_LEN);
    }

    if (pCmd != NULL && pCmd->Type == AMBSH_CMD_PROC) {
        SHELL_IoRedirectInit(pParentShell, pCmd, &pFileInput, &pFileOutput, &pFileAppend, &pPipe);

        if (pFileInput) {
            pEnv->ReadIo.Type |= AMBSH_IO_FILE;
            pEnv->ReadIo.pFileInput = (void *) pFileInput;
        }

        if (pEnv->pParent && pEnv->pParent->WriteIo.pPipe) {
            pEnv->ReadIo.Type |= AMBSH_IO_PIPE;
            pEnv->ReadIo.pPipe = pEnv->pParent->WriteIo.pPipe;
        }

        if (pFileOutput) {
            pEnv->WriteIo.Type |= AMBSH_IO_FILE;
            pEnv->WriteIo.pFileOut = (void *) pFileOutput;
        }

        if (pFileAppend) {
            pEnv->WriteIo.Type |= AMBSH_IO_FILE;
            pEnv->WriteIo.pFileAppend = (void *) pFileAppend;
        }

        if (pPipe) {
            pEnv->WriteIo.Type |= AMBSH_IO_PIPE;
            pEnv->WriteIo.pPipe = (void *) pPipe;
        }
    }

    if (pEnv->pParent == NULL) {
        pEnv->ReadIo.Type |= AMBSH_IO_CONSOLE;
    } else {
        if (pEnv->ReadIo.pFileInput == NULL &&
            (pEnv->pParent->ReadIo.Type & (AMBSH_IO_FILE | AMBSH_IO_FILE_CONT))) {
            pEnv->ReadIo.Type |= AMBSH_IO_FILE_CONT;
            pEnv->ReadIo.pFileInput = (void *) pEnv->pParent->ReadIo.pFileInput;
        } else if (pEnv->pParent->ReadIo.Type & AMBSH_IO_CONSOLE && !NullDev) {
            pEnv->ReadIo.Type |= AMBSH_IO_CONSOLE;
        }
    }

    if (pEnv->pParent == NULL) {
        pEnv->WriteIo.Type |= AMBSH_IO_CONSOLE;
    } else {
        if ((pEnv->WriteIo.pFileOut == NULL && pEnv->WriteIo.pFileAppend == NULL) &&
            (pEnv->pParent->WriteIo.Type & (AMBSH_IO_FILE | AMBSH_IO_FILE_CONT))) {
            pEnv->WriteIo.Type |= AMBSH_IO_FILE_CONT;
            pEnv->WriteIo.pFileOut = (void *) pEnv->pParent->WriteIo.pFileOut;
            pEnv->WriteIo.pFileAppend = (void *) pEnv->pParent->WriteIo.pFileAppend;
        } else if (pEnv->pParent->WriteIo.Type & AMBSH_IO_CONSOLE  && !NullDev) {
            pEnv->WriteIo.Type |= AMBSH_IO_CONSOLE;
        }
    }

    if (pParentShell) {
        pEnv->ReadIo.ConsoleGetCharCb = pParentShell->ReadIo.ConsoleGetCharCb;
        pEnv->WriteIo.ConsolePutCharCb = pParentShell->WriteIo.ConsolePutCharCb;
        pEnv->WriteIo.ConsoleFlushCb = pParentShell->WriteIo.ConsoleFlushCb;
    } else {
        pEnv->ReadIo.ConsoleGetCharCb = SHELL_ConsoleGetChar;
        pEnv->WriteIo.ConsolePutCharCb = SHELL_ConsolePutChar;
        pEnv->WriteIo.ConsoleFlushCb = SHELL_ConsoleFlush;
    }

    /* Set task ID name */
    AmbaShell_EnvArgString(pEnv, TaskString, sizeof(TaskString));

    /* Create a task but do not start it! */
    AmbaKAL_TaskCreate(&pEnv->Task, "AmbaShellTask", AmbaShellConfig.Priority, AmbaShellTask, (UINT32)pEnv,
                       pEnv->Stack, AMBSH_TASK_STACK_SIZE, TX_DONT_START);

    /* Append it to the global env list */
    AmbaKAL_MutexTake(&_AmbaShell_EnvMutex, AMBA_KAL_WAIT_FOREVER);
    if (_pAmbaShellRootProc == NULL) {
        _pAmbaShellRootProc = pEnv;
    } else {
        pCur = _pAmbaShellRootProc;
        while (pCur->pChild != NULL)
            pCur = pCur->pChild;
        pCur->pChild = pEnv;
    }
    AmbaKAL_MutexGive(&_AmbaShell_EnvMutex);

    return pEnv;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: SHELL_Destroy
 *
 *  @Description:: Destroy a shell environment
 *
 *  @Input      ::
 *      pShellEnv:  shell to be destroyed
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static void SHELL_Destroy(AMBA_SHELL_ENV_s *pShellEnv)
{
    AMBA_SHELL_ENV_s *pEnv;

    if (pShellEnv == NULL)
        return;

    SHELL_IoRedirectExit(pShellEnv);

    /* Remove the entry from the global env list */
    AmbaKAL_MutexTake(&_AmbaShell_EnvMutex, AMBA_KAL_WAIT_FOREVER);
    if (pShellEnv == _pAmbaShellRootProc)
        _pAmbaShellRootProc = pShellEnv->pChild;
    else {
        pEnv = _pAmbaShellRootProc;
        while (pEnv->pChild) {
            if (pEnv->pChild == pShellEnv) {
                pEnv->pChild = pShellEnv->pChild;
                break;
            }
            pEnv = pEnv->pChild;
        }
    }
    AmbaKAL_MutexGive(&_AmbaShell_EnvMutex);

    AmbaKAL_TaskTerminate(&pShellEnv->Task);
    AmbaKAL_TaskDelete(&pShellEnv->Task);
    AmbaKAL_BytePoolFree((void *) pShellEnv);
}

/**
 * Try to free the pEnv->pChild list of environments (which became background
 * tasks).
 */
int AmbaShell_EnvTryFreeChild(AMBA_SHELL_ENV_s *pEnv)
{
    char TaskString[64];
    AMBA_SHELL_ENV_s *e, *c;
    int n = 0;
    int active = 0;

    if (pEnv == NULL)
        return 0;

    for (e = pEnv->pChild, n = 1; e; e = e->pChild, n++) {
        UINT State = pEnv->pChild->Task.tx_thread_state;

        if (State == TX_READY) {
            active++;
        } else if (e->Background) {
            AmbaShell_CmdFree(e->cmd);
            AmbaShell_EnvArgString(pEnv->pChild, TaskString, sizeof(TaskString));
            AmbaShell_Print(pEnv, "[%d] Done\t%s\n", n, TaskString);
            e->Background = 0;
        }
    }

    if (active == 0) {
        for (e = c = pEnv->pChild; e; e = c) {
            c = e->pChild;
            e->pChild = NULL;
            SHELL_Destroy(e);
        }
        pEnv->pChild = NULL;
    }

    return active;
}

/**
 * Forcibly remove any remaining child ambsh tasks.
 */
int ambsh_env_force_kill_child(AMBA_SHELL_ENV_s *pEnv)
{
    int count = 0;
    AMBA_SHELL_ENV_s *e, *c;

    for (e = c = pEnv->pChild; e; e = c) {
        AmbaKAL_TaskTerminate(&e->Task);
        c = e->pChild;
        e->pChild = NULL;
        SHELL_Destroy(e);
        count++;
    }

    pEnv->pChild = NULL;

    return count;
}

/**
 * Wait for execution completion.
 */
static int SHELL_WaitCmdProcDone(AMBA_SHELL_ENV_s *pEnv)
{
    AMBA_SHELL_ENV_s *pRootProc = _pAmbaShellRootProc;
    int Rval = AMBSH_SIGNAL_EXIT;
    char Input;

    _AmbaShell_NumRawData = 0;

    for (;;) {
        if ((pEnv->Task.tx_thread_state == TX_COMPLETED) ||
            (pEnv->Task.tx_thread_state == TX_TERMINATED)) {
            Rval = pEnv->RetValue;
            break;
        }

        if (pRootProc == NULL) {
            if (AmbaShell_ReadTimeout(pRootProc, (UINT8 *) &Input, 1, 10) > 0) {
                if (_AmbaShell_NumRawData < GetArraySize(_AmbaShell_RawBuf))
                    _AmbaShell_RawBuf[_AmbaShell_NumRawData++] = Input;
            }

            if (Input == 0x3) { /* 0x3 is Ctrl+C according to ASCII table */
                AmbaKAL_TaskResume(&pEnv->Task);
                AmbaKAL_MutexGive(&_AmbaShell_EnvMutex);
                _AmbaShell_NumRawData = 0;  /* throw away cached characters */
            }
        } else {
            AmbaKAL_TaskSleep(10);
        }
    }

    return Input == 0x3 ? AMBSH_SIGNAL_EXIT : Rval;
}

#define AMBSH_VT100_NOSEQ           -1
#define AMBSH_VT100_MORE            0
#define AMBSH_VT100_CURSOR_UP       1
#define AMBSH_VT100_CURSOR_DOWN     2
#define AMBSH_VT100_CURSOR_RIGHT    3
#define AMBSH_VT100_CURSOR_LEFT     4
#define AMBSH_VT100_INSERT          5
#define AMBSH_VT100_HOME            6
#define AMBSH_VT100_PRIOR           7
#define AMBSH_VT100_DELETE          8
#define AMBSH_VT100_END             9
#define AMBSH_VT100_NEXT            10
#define AMBSH_VT100_F1              11
#define AMBSH_VT100_F2              12
#define AMBSH_VT100_F3              13
#define AMBSH_VT100_F4              14
#define AMBSH_VT100_F5              15
#define AMBSH_VT100_F6              16
#define AMBSH_VT100_F7              17
#define AMBSH_VT100_F8              18
#define AMBSH_VT100_F9              19
#define AMBSH_VT100_F10             20
#define AMBSH_VT100_F11             21
#define AMBSH_VT100_F12             22
#define AMBSH_VT100_PRINT_SCREEN    23
#define AMBSH_VT100_SCROLL_LOCK     24
#define AMBSH_VT100_PAUSE_BREAK     25
#define AMBSH_VT100_NUM_LOCK        26
#define AMBSH_VT100_DIVIDE          27
#define AMBSH_VT100_MULTIPLY        28
#define AMBSH_VT100_SUBTRACT        29
#define AMBSH_VT100_ADD             30
#define AMBSH_VT100_DECIMAL         31
#define AMBSH_VT100_0               32
#define AMBSH_VT100_1               33
#define AMBSH_VT100_2               34
#define AMBSH_VT100_3               35
#define AMBSH_VT100_4               36
#define AMBSH_VT100_5               37
#define AMBSH_VT100_6               38
#define AMBSH_VT100_7               39
#define AMBSH_VT100_8               40
#define AMBSH_VT100_9               41

struct vt100_seq_s {
    const char *seq;
    int len;
    int code;
};

static struct vt100_seq_s vt100_seq[] = {
    { "\x1b[A",  3, AMBSH_VT100_CURSOR_UP       },
    { "\x1b[B",  3, AMBSH_VT100_CURSOR_DOWN     },
    { "\x1b[C",  3, AMBSH_VT100_CURSOR_RIGHT    },
    { "\x1b[D",  3, AMBSH_VT100_CURSOR_LEFT     },

    { "\x1b[1~", 4, AMBSH_VT100_INSERT      },
    { "\x1b[2~", 4, AMBSH_VT100_HOME        },
    { "\x1b[3~", 4, AMBSH_VT100_PRIOR       },
    { "\x1b[4~", 4, AMBSH_VT100_DELETE      },
    { "\x1b[5~", 4, AMBSH_VT100_END         },
    { "\x1b[6~", 4, AMBSH_VT100_NEXT        },
#if 0   /* No use */
    { "\x1b[17~", 5, AMBSH_VT100_F1         },
    { "\x1b[18~", 5, AMBSH_VT100_F2         },
    { "\x1b[19~", 5, AMBSH_VT100_F3         },
    { "\x1b[20~", 5, AMBSH_VT100_F4         },
    { "\x1b[21~", 5, AMBSH_VT100_F5         },
    { "\x1b[23~", 5, AMBSH_VT100_F6         },
    { "\x1b[24~", 5, AMBSH_VT100_F7         },
    { "\x1b[25~", 5, AMBSH_VT100_F8         },
    { "\x1b[26~", 5, AMBSH_VT100_F9         },
    { "\x1b[28~", 5, AMBSH_VT100_F10        },
    { "\x1b[29~", 5, AMBSH_VT100_F11        },
    { "\x1b[31~", 5, AMBSH_VT100_F12        },

    { "\x1b[32~", 5, AMBSH_VT100_PRINT_SCREEN   },
    { "\x1b[33~", 5, AMBSH_VT100_SCROLL_LOCK    },
    { "\x1b[34~", 5, AMBSH_VT100_PAUSE_BREAK    },

    { "\x1bOP", 3, AMBSH_VT100_NUM_LOCK     },
    { "\x1bOQ", 3, AMBSH_VT100_DIVIDE       },
    { "\x1bOR", 3, AMBSH_VT100_MULTIPLY     },
    { "\x1bOS", 3, AMBSH_VT100_SUBTRACT     },
    { "\x1bOm", 3, AMBSH_VT100_ADD          },
    { "\x1bOn", 3, AMBSH_VT100_DECIMAL      },
    { "\x1bOp", 3, AMBSH_VT100_0            },
    { "\x1bOq", 3, AMBSH_VT100_1            },
    { "\x1bOr", 3, AMBSH_VT100_2            },
    { "\x1bOs", 3, AMBSH_VT100_3            },
    { "\x1bOt", 3, AMBSH_VT100_4            },
    { "\x1bOu", 3, AMBSH_VT100_5            },
    { "\x1bOv", 3, AMBSH_VT100_6            },
    { "\x1bOw", 3, AMBSH_VT100_7            },
    { "\x1bOx", 3, AMBSH_VT100_8            },
    { "\x1bOy", 3, AMBSH_VT100_9            },
#endif
    { NULL, -1, -1 },
};

/**
 * Insert a character to string.
 */
static int StringInsertChar(char *pSrc, int Size, char ch, int *pLocate)
{
    int i;
    int Len = strlen(pSrc);
    int loc = *pLocate;

    if ((pSrc == NULL) || (loc > Len) || (loc >= Size) || ((Len + 1) >= Size))
        return -1;

    pSrc[Len + 1] = '\0';

    for (i = Len; i > loc; i--) {
        pSrc[i] = pSrc[i - 1];
    }

    pSrc[loc] = ch;

    *pLocate += 1;

    return 0;
}

/**
 * Remove a character from string.
 */
static int StringRemoveChar(char *pSrc, int size, int *pLocate)
{
    int i;
    int Len = strlen(pSrc);
    int loc = *pLocate;

    K_ASSERT(pSrc != NULL);
    K_ASSERT(loc <= Len);
    K_ASSERT(loc < size);

    if (loc == Len) {
        pSrc[Len - 1] = '\0';
        *pLocate -= 1;
        return 0;
    }

    for (i = (loc - 1); i <= Len; i++) {
        pSrc[i] = pSrc[i + 1];
    }

    *pLocate -= 1;

    return 0;
}

/**
 * Match VT100 escape sequence.
 */
static int ambsh_match_vt100(const char *vt100, int n)
{
    int more = 0;
    int i, j;

    for (i = 0; vt100_seq[i].seq != NULL; i++) {
        if (n < vt100_seq[i].len)
            more = 1;
        if (n == vt100_seq[i].len) {
            for (j = 0; j < n && vt100[j] == vt100_seq[i].seq[j];
                 j++);
            if (j == n)
                return vt100_seq[i].code;
        }
    }

    if (more)
        return AMBSH_VT100_MORE;

    return AMBSH_VT100_NOSEQ;
}

#if defined(AMBA_SHELL_ENABLE_HISTORY)
static int g_LoadHistoryFromFile = 0;
#endif

#define HISTORY_FILE    "a:\\.history"

/**
 * load history from a:\.history
 */
int LoadHistory(AMBA_SHELL_ENV_s *pEnv)
{
    int Ercd;
    char Buf[MAX_HISTORY_ENTRY];
    AMBA_FS_STAT FileStat;
    char *pHistoryBuf;
    AMBA_FS_FILE *pFsHistory;
    int c, x, Rval;

    //    if (AmbaNFTL_IsInit(NFTL_ID_STORAGE) == 0)
    //        return -1;

    /* load history data */
    Rval = AmbaFS_Stat(HISTORY_FILE, &FileStat);
    if (Rval < 0) {
        DEBUG_MSG(".history file does not exist!");
        /*      return -2; */
        return 0;
    }

    AmbaShell_Ascii2Unicode(HISTORY_FILE, Buf);

    pFsHistory = AmbaFS_fopen(Buf, "r");
    if (pFsHistory == NULL) {
        DEBUG_MSG("open .history file failed!");
        return -3;
    }

    /* Allocate memory for prompt history */
    Ercd = AmbaShell_MemAlloc((void *) &pHistoryBuf, FileStat.Size);
    if (Ercd != OK) {
        DEBUG_MSG("Unable to allocate shell: out of memory!");
        Rval = -4;
        goto end_load_history;
    }
    memset(pHistoryBuf, 0x0, FileStat.Size);

    Rval = AmbaFS_fread(pHistoryBuf, 1, FileStat.Size, pFsHistory);
    if (Rval != FileStat.Size) {
        DEBUG_MSG("Read data error!");
        Rval = -5;
    } else {
        x = 0;
        for (c = 0; c < FileStat.Size; c++) {
            Buf[x++] = pHistoryBuf[c];

            if (x >= MAX_HISTORY_ENTRY) {
                DEBUG_MSG("<err> buffer overflow!");

                /* drop error data */
                while (pHistoryBuf[c] != 0x0a) {
                    c++;
                }
                x = 0;
            }

            if (Buf[x - 1] == 0x0a) {
                Buf[x - 1] = '\n';
                Buf[x]     = '\0';

                strncpy(pEnv->pHistory->line[pEnv->pHistory->tail],
                        Buf,
                        sizeof(pEnv->pHistory->line[0]));
                pEnv->pHistory->tail++;
                if (pEnv->pHistory->tail >= MAX_AMBSH_HISTORY)
                    pEnv->pHistory->tail = 0;
                memset(pEnv->pHistory->line[pEnv->pHistory->tail],
                       0, MAX_HISTORY_ENTRY);
                if (pEnv->pHistory->tail == pEnv->pHistory->head)
                    pEnv->pHistory->head++;
                if (pEnv->pHistory->head >= MAX_AMBSH_HISTORY)
                    pEnv->pHistory->head = 0;
                x = 0;
            }
        }
        AmbaShell_Flush(pEnv);
    }

    /* Free history buffer */
    AmbaKAL_BytePoolFree((void *) pHistoryBuf);

end_load_history:
    Rval = AmbaFS_fclose(pFsHistory);
    if (Rval < 0) {
        DEBUG_MSG("Unable to close history file!");
        Rval = -6;
    }

    return Rval;
}

/**
 * Replace current input with history.
 */
#if defined(AMBA_SHELL_ENABLE_HISTORY)
static void AmbaShell_ReplaceHistory(AMBA_SHELL_ENV_s *pEnv,
                                     int up, char *s, int *nread)
{
    const char *pRplstr = NULL;

    if ((pEnv->pHistory == NULL) || (pEnv->pHistory->on == 0))
        return;

    if (up) {
        if (pEnv->pHistory->curr == pEnv->pHistory->head)
            return;
        pEnv->pHistory->curr--;
        if (pEnv->pHistory->curr < 0)
            pEnv->pHistory->curr = (MAX_AMBSH_HISTORY - 1);
    } else {
        if (pEnv->pHistory->curr == pEnv->pHistory->tail)
            return;
        pEnv->pHistory->curr++;
        if (pEnv->pHistory->curr >= MAX_AMBSH_HISTORY)
            pEnv->pHistory->curr = 0;
    }
    pRplstr = pEnv->pHistory->line[pEnv->pHistory->curr];

    while (*nread > 0) {
        *nread = *nread - 1;
        AmbaShell_Write(pEnv, (const UINT8 *)"\x1b[D\x1b[P", 6);
    }

    while (*pRplstr != '\n' && *pRplstr != '\r' && *pRplstr != '\0') {
        AmbaShell_Write(pEnv, (const UINT8 *) pRplstr, 1);
        *s++ = *pRplstr++;
        *nread = *nread + 1;
    }

    AmbaShell_Flush(pEnv);
}
#endif  /* AMBA_SHELL_ENABLE_HISTORY */

/**
 * Get command string from prompt.
 */
int AmbaShell_GetCmdStringFromPrompt(AMBA_SHELL_ENV_s *pEnv,
                                     char *s, int n, int Echo)
{
    char c;
    int nread, cursor, Rval;
    int maxread = n - 1;
    char command[10];   /* command buffer */
    char vt100[10];     /* VT100 control buffer */
    int nvt100 = 0;
    int i = 0;

    for (nread = cursor = 0; nread < maxread; ) {
        /* get one char for parsing */
        if (_AmbaShell_NumRawData) {
            c = _AmbaShell_RawBuf[i++];

            if (i == _AmbaShell_NumRawData)
                _AmbaShell_NumRawData = 0;
        } else if (AmbaShell_Read(pEnv, (UINT8 *) &c, 1) != 1) {
            DEBUG_MSG("\n<err> AmbaShell_Read failed");
            return -1;
        }

        /* parse character */
        if ((UINT32)c >= 0x80) {
            DEBUG_MSG("\n<err> illegal code");
            _AmbaShell_NumRawData = 0;
            return -2;
        }

        /* handle spectial key */
        if (nvt100 > 0) {
            vt100[nvt100++] = c;
            if (nvt100 >= 10) {
                DEBUG_MSG("<err> vt100 buffer overflow");
                nvt100 = 0;
            }
            switch (ambsh_match_vt100(vt100, nvt100)) {
            case AMBSH_VT100_CURSOR_UP:
#if defined(AMBA_SHELL_ENABLE_HISTORY)
                if ((pEnv->pHistory != NULL)  &&
                    (pEnv->pHistory->on)      &&
                    (g_LoadHistoryFromFile == 0)) {
                    Rval = LoadHistory(pEnv);
                    if (Rval >= 0) {
                        g_LoadHistoryFromFile = 1;
                    }
                    pEnv->pHistory->curr = pEnv->pHistory->tail;
                }
#endif
                /* move cursor to end */
                if (cursor < nread) {
                    if (Echo) {
                        sprintf(command, "\x1b[%dC",
                                (nread - cursor));
                        AmbaShell_Write(pEnv,
                                        (const UINT8 *) command,
                                        strlen(command));
                        AmbaShell_Flush(pEnv);
                    }
                    cursor = nread;
                }
#if defined(AMBA_SHELL_ENABLE_HISTORY)
                AmbaShell_ReplaceHistory(pEnv, 1, s, &nread);
#endif
                cursor = nread;
                nvt100 = 0;
                continue;
            case AMBSH_VT100_CURSOR_DOWN:
                /* move cursor to end */
                if (cursor < nread) {
                    if (Echo) {
                        sprintf(command, "\x1b[%dC",
                                (nread - cursor));
                        AmbaShell_Write(pEnv,
                                        (const UINT8 *) command,
                                        strlen(command));
                        AmbaShell_Flush(pEnv);
                    }
                    cursor = nread;
                }
#if defined(AMBA_SHELL_ENABLE_HISTORY)
                AmbaShell_ReplaceHistory(pEnv, 0, s, &nread);
#endif
                cursor = nread;
                nvt100 = 0;
                continue;
            case AMBSH_VT100_CURSOR_RIGHT:
                if (cursor < nread) {
                    if (Echo) {
                        AmbaShell_Write(pEnv, (const UINT8 *)"\x1b[C", 3);
                        AmbaShell_Flush(pEnv);
                    }
                    cursor++;
                }
                nvt100 = 0;
                continue;
            case AMBSH_VT100_CURSOR_LEFT:
                if (cursor > 0) {
                    if (Echo) {
                        AmbaShell_Write(pEnv, (const UINT8 *)"\x1b[D", 3);
                        AmbaShell_Flush(pEnv);
                    }
                    cursor--;
                }
                nvt100 = 0;
                continue;
            case AMBSH_VT100_INSERT:
                AmbaShell_Write(pEnv, (const UINT8 *)"\x1b[4h", 4);
                nvt100 = 0;
                continue;
            case AMBSH_VT100_HOME:
                if (cursor > 0) {
                    if (Echo) {
                        sprintf(command, "\x1b[%dD",
                                cursor);
                        AmbaShell_Write(pEnv,
                                        (const UINT8 *) command,
                                        strlen(command));
                        AmbaShell_Flush(pEnv);
                    }
                    cursor = 0;
                }
                nvt100 = 0;
                continue;
            case AMBSH_VT100_END:
                if (cursor < nread) {
                    if (Echo) {
                        sprintf(command, "\x1b[%dC",
                                (nread - cursor));
                        AmbaShell_Write(pEnv,
                                        (const UINT8 *) command,
                                        strlen(command));
                        AmbaShell_Flush(pEnv);
                    }
                    cursor = nread;
                }
                nvt100 = 0;
                continue;
            case AMBSH_VT100_MORE:
                continue;
            default:
                /* receive vt100 command doesn't support */
                DEBUG_MSG("\n<err> illegal command");
                return -3;
            }
        }

        if ((c == '\r') || (c == '\n')) {
            if (Echo) {
                AmbaShell_Write(pEnv, (const UINT8 *)"\r\n", 2);
                AmbaShell_Flush(pEnv);
            }

            s[nread++] = '\0';
            Rval = (nread - 1);
            goto done;

        } else if (c == 0x8) {  /* Back space */
            if ((nread > 0) && (cursor > 0)) {
                if (Echo) {
                    AmbaShell_Write(pEnv, (const UINT8 *)"\x1b[D\x1b[P", 6);
                    AmbaShell_Flush(pEnv);
                }
                StringRemoveChar(s, maxread, &cursor);
                nread--;
            }
        } else if (c == 0x3) {  /* Ctrl + C */
            if (Echo) {
                AmbaShell_Write(pEnv, (const UINT8 *)"\r\n", 2);
                AmbaShell_Flush(pEnv);
            }
            nread = 0;
            s[0] = '\0';
            break;
        } else if (c == 0x1b) { /* Escape */
            vt100[0] = c;
            nvt100 = 1;
        } else if ((UINT32)c < 0x20 || (UINT32)c > 0x7f) {
            /* Ignore non-ASCII, control characters */
        } else {
            if (StringInsertChar(s, maxread, c, &cursor) == 0) {
                nread++;
                if (Echo) {
                    AmbaShell_Write(pEnv, (const UINT8 *) &c, 1);
                    AmbaShell_Flush(pEnv);
                }
            }
        }
    }

    Rval = nread;

done:
    _AmbaShell_NumRawData = 0;

    if (Rval > 0) {
        s[Rval++] = '\n';
        s[Rval]   = '\0';
    }

#if defined(AMBA_SHELL_ENABLE_HISTORY)
    /* save command to history buffer */
    if ((pEnv->pHistory != NULL) && (pEnv->pHistory->on) &&
        (strlen(s) > 0)) {

        /* load history from file */
        if ((g_LoadHistoryFromFile == 0) && (LoadHistory(pEnv) >= 0)) {
            g_LoadHistoryFromFile = 1;
        }

        /* save command to history */
        strncpy(pEnv->pHistory->line[pEnv->pHistory->tail],
                s, sizeof(pEnv->pHistory->line[0]));
        pEnv->pHistory->tail++;
        if (pEnv->pHistory->tail >= MAX_AMBSH_HISTORY)
            pEnv->pHistory->tail = 0;
        memset(pEnv->pHistory->line[pEnv->pHistory->tail], 0,
               MAX_HISTORY_ENTRY);
        if (pEnv->pHistory->tail == pEnv->pHistory->head)
            pEnv->pHistory->head++;
        if (pEnv->pHistory->head >= MAX_AMBSH_HISTORY)
            pEnv->pHistory->head = 0;

        pEnv->pHistory->curr = pEnv->pHistory->tail;
    }
#endif

    return Rval;
}

/**
 * Interactive shell.
 */
void AmbaShell_Prompt(AMBA_SHELL_ENV_s *pEnv)
{
    int l;
    AMBA_SHELL_CMD_s *pCmd;
#if defined(AMBA_SHELL_ENABLE_HISTORY)
    int Rval;
    int Ercd;
    int c, Len;
    AMBA_FS_FILE *pFsHistory;
#endif

    if (pEnv->WriteIo.ConsolePutCharCb == SHELL_ConsolePutChar) {
        /* Set terminal to insert mode */
        AmbaShell_Write(pEnv, (const UINT8 *)"\x1b[4h", 4);
    }

    AmbaShell_Print(pEnv, AMBSH_PROMPT_MSG);
    AmbaShell_Print(pEnv, "Type 'help' for help\n\n");

#if defined(AMBA_SHELL_ENABLE_HISTORY)
    /* Allocate memory for prompt history */
    if (pEnv->pHistory == NULL) {
        Ercd = AmbaShell_MemAlloc((void **)&pEnv->pHistory, sizeof(*(pEnv->pHistory)));
        if (Ercd != OK) {
            DEBUG_MSG("Unable to allocate hisotry: out of memory!");
        } else {
            memset(pEnv->pHistory, 0x0, sizeof(*pEnv->pHistory));
            /* default enable history save to storage */
            pEnv->pHistory->on = 1;
        }
    }
#endif

    /* Endless command loop */
    while (1) {
        AmbaShell_EnvTryFreeChild(pEnv);
        AmbaShell_Print(pEnv, "%s> ", pEnv->Cwd);
        AmbaShell_Flush(pEnv);

        memset(_AmbaShell_PromptBuf, 0x0, sizeof(_AmbaShell_PromptBuf));

        l = AmbaShell_GetCmdStringFromPrompt(pEnv, _AmbaShell_PromptBuf, sizeof(_AmbaShell_PromptBuf) - 2, 1);
        if (l <= 0) {
            _AmbaShell_NumRawData = 0;
            continue;
        }

#ifdef AMBA_SHELL_ENABLE_HISTORY
        if ((pEnv->pHistory != NULL) && (pEnv->pHistory->on)) {
            /* save history to file */

            char Buf[512];

            AmbaShell_Ascii2Unicode(HISTORY_FILE, Buf);

            if (g_LoadHistoryFromFile) {
                pFsHistory = AmbaFS_fopen(Buf, "w");
                DEBUG_MSG("open history file at write mode");
            } else {
                pFsHistory = AmbaFS_fopen(Buf, "a");
                DEBUG_MSG("open history file at append mode");
            }
            if (pFsHistory == NULL) {
                DEBUG_MSG("Can not open history file!");
                goto done;
            }

            for (c = pEnv->pHistory->head; c < pEnv->pHistory->tail; c++) {
                if (c >= MAX_AMBSH_HISTORY)
                    c = 0;

                Len = strlen(pEnv->pHistory->line[c]);
                Rval = AmbaFS_fwrite(pEnv->pHistory->line[c], 1, Len, pFsHistory);
                if (Rval != Len) {
                    DEBUG_MSG(
                        "Write data to history file error!");
                    break;
                }
            }

            Rval = AmbaFS_fclose(pFsHistory);
            if (Rval < 0) {
                DEBUG_MSG("Unable to close history file!");
            }

            /* load history from file */
            if (g_LoadHistoryFromFile == 0) {
                Rval = LoadHistory(pEnv);
                if (Rval >= 0) {
                    g_LoadHistoryFromFile = 1;
                }
                pEnv->pHistory->curr = pEnv->pHistory->tail;
            }
        }
done:
#endif
        pCmd = ambsh_yyparse_text(_AmbaShell_PromptBuf, l);
        if (pCmd == NULL) {
            AmbaShell_Print(pEnv, "%s\n", ambsh_yyparse_err());
            continue;
        }

        SHELL_ExecuteCmdProc(pEnv, pCmd);
        if (!pCmd->u.Proc.Background)
            AmbaShell_CmdFree(pCmd);
    }
}

/**
 * Find built-in commands.
 */
AMBA_SHELL_PROC_f AmbaShell_FindProcImage(AMBA_SHELL_ENV_s *pEnv, const char *pName)
{
    AMBA_SHELL_LIST_s *pCur;

    /* Search the ".ambsh" section for built-in commands */
    for (pCur = AmbaShell_DefaultCmdList; pCur != NULL; pCur = pCur->pNext) {
        if (strcmp(pName, pCur->pName) == 0) {
            return pCur->Proc;
        }
    }

    return NULL;
}

#define AMBSH_CHECK_SIGNAL()                \
    if (pEnv->RetValue == AMBSH_SIGNAL_EXIT)       \
        goto done;

/**
 * Execute the parsed cmd structure.
 */
static int SHELL_ExecuteCmdProc(AMBA_SHELL_ENV_s *pEnv, AMBA_SHELL_CMD_s *pCmd)
{
    AMBA_SHELL_ENV_s *pChild = NULL;
    int count;

    if (pCmd == NULL)
        return 0;

    switch (pCmd->Type) {
    case AMBSH_CMD_PROC:
        pChild = SHELL_Create(pEnv, pCmd);
        if (pChild == NULL) {
            pEnv->RetValue = -1;
            goto done;
        }
        AmbaKAL_TaskResume(&pChild->Task);

        if (pCmd->u.Proc.pPipe) {
            pEnv->RetValue = SHELL_ExecuteCmdProc(pChild, pCmd->u.Proc.pPipe);
        }

        if (pCmd->u.Proc.Background) {
            pEnv->RetValue = 0;
        } else {
            /* Otherwise, sleep and wait for the child to   */
            /* complete!                                    */
            pEnv->RetValue = SHELL_WaitCmdProcDone(pChild);
            SHELL_Destroy(pChild);
        }
        break;
    case AMBSH_CMD_IFC:
        /* Execute the test part of the IF-conditional */
        pEnv->RetValue = SHELL_ExecuteCmdProc(pEnv, pCmd->u.IfClause.pTest);
        AMBSH_CHECK_SIGNAL();

        if (pEnv->RetValue == 0) {
            /* Execute the 'if' part of the IF-conditional */
            pEnv->RetValue = SHELL_ExecuteCmdProc(pEnv, pCmd->u.IfClause.ifpart);
        } else {
            /* Execute the 'else' part of the IF-conditional */
            pEnv->RetValue = SHELL_ExecuteCmdProc(pEnv, pCmd->u.IfClause.pElsePart);
        }
        AMBSH_CHECK_SIGNAL();
        pEnv->RetValue = 0;
        break;
    case AMBSH_CMD_WHILE:
        for (;;) {
            /* Execute the test part of the WHILE-loop */
            pEnv->RetValue = SHELL_ExecuteCmdProc(pEnv, pCmd->u.WhileClause.pTest);
            AMBSH_CHECK_SIGNAL();

            if (pEnv->RetValue == 0) {
                /* Execute the action part of the WHILE-loop */
                pEnv->RetValue = SHELL_ExecuteCmdProc(pEnv, pCmd->u.WhileClause.pAction);
                AMBSH_CHECK_SIGNAL();
            } else {
                break;
            }
        }
        break;
    case AMBSH_CMD_UNTIL:
        for (;;) {
            /* Execute the test part of the UNTIL-loop */
            pEnv->RetValue = SHELL_ExecuteCmdProc(pEnv, pCmd->u.UntilClause.pTest);
            AMBSH_CHECK_SIGNAL();

            if (pEnv->RetValue != 0) {
                /* Execute the action part of the UNTIL-loop */
                SHELL_ExecuteCmdProc(pEnv, pCmd->u.UntilClause.pAction);
                AMBSH_CHECK_SIGNAL();
            } else {
                break;
            }
        }
        break;
    case AMBSH_CMD_LOOP:
        count = atoi((char const *)pCmd->u.LoopClause.pTest->u.Proc.buf);
        for (; count > 0; count--) {
            /* Execute the action part of the LOOP-loop */
            SHELL_ExecuteCmdProc(pEnv, pCmd->u.LoopClause.pAction);
            AMBSH_CHECK_SIGNAL();
        }
        break;
    }

    if (pCmd->pNext)
        pEnv->RetValue = SHELL_ExecuteCmdProc(pEnv, pCmd->pNext);

done:
    AmbaShell_Flush(pEnv);

    return pEnv->RetValue;
}

/*-----------------------------------------------------------------------------------------------*\
*  @RoutineName:: AmbaShellTask
*
*  @Description:: Task of ambsh.
*
*  @Input      :: none
*
*  @Output     :: none
*
*  @Return     ::
*          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
void AmbaShellTask(UINT32 exinf)
{
    AMBA_SHELL_ENV_s *pEnv = (AMBA_SHELL_ENV_s *) exinf;
    AMBA_SHELL_PROC_f Proc;
#ifndef AMBA_SHELL_NO_FS
    AMBA_SHELL_CMD_s *pCmd;
    int l, Len, Rval = 0;
    char Buf[512];
#endif

    /* Check if this is a subshell command */
    if (pEnv->pSubshellCmd) {
        pEnv->RetValue = SHELL_ExecuteCmdProc(pEnv, pEnv->pSubshellCmd);
        goto done;
    }

    /* Check if this is an interactive shell */
    if (pEnv->Argc == 0) {
        AmbaShell_Prompt(pEnv);
    }

#ifdef AMBA_SHELL_NO_FS
    goto try_func_proc;
#else
    Len = strlen(pEnv->Argv[0]);
    if (Len > 4 && strcmp(&pEnv->Argv[0][Len - 4], ".ash") == 0)
        goto try_file_proc;
    else
        goto try_func_proc;


try_file_proc:

    /* Search the file system */
    /* Currently only support .ash scripts */

    /* Make sure to have correct path for ff_chdir. */
    l = strlen(pEnv->Cwd);
    if (pEnv->Cwd[l - 1] == '\\')
        pEnv->Cwd[l - 1] = '\0';

    AmbaShell_Ascii2Unicode(pEnv->Cwd, Buf);

    Rval = AmbaFS_Chdir(Buf);
    if (Rval < 0) {
        pEnv->RetValue = -1;
        goto done;
    }

    /* Restore '\' character back */
    pEnv->Cwd[l - 1] = '\\';

    pCmd = ambsh_yyparse_file(pEnv->Argv[0]);
    if (pCmd) {
        pEnv->RetValue = SHELL_ExecuteCmdProc(pEnv, pCmd);
        AmbaShell_CmdFree(pCmd);
        goto done;
    }

#if defined(SUPPORT_ROMFS)
    /* Search the ROMFS */
    pCmd = ambsh_yyparse_rom_file(pEnv->Argv[0]);
    if (pCmd) {
        pEnv->RetValue = SHELL_ExecuteCmdProc(pEnv, pCmd);
        AmbaShell_CmdFree(pCmd);
        goto done;
    }
#endif
    pEnv->RetValue = -1;
#endif

try_func_proc:

    /* Find a function to execute the command */
    Proc = AmbaShell_FindProcImage(pEnv, pEnv->Argv[0]);
    if (Proc != NULL) {
        pEnv->RetValue = Proc(pEnv, pEnv->Argc, pEnv->Argv);
    } else {
        AmbaShell_Print(pEnv, "-ambsh: %s: command not found!\n", pEnv->Argv[0]);
        pEnv->RetValue = -1;
    }

done:

    /* Make sure that all child tasks are no longer active */
    ambsh_env_force_kill_child(pEnv);

    /* FIXME */
    /* Wake up parent task if not a background task */
    if (pEnv->pParent && !pEnv->Background)
        AmbaKAL_TaskResume(&pEnv->pParent->Task);

    //    AmbaKAL_TaskSuspend(&pEnv->Task);
}

/*-----------------------------------------------------------------------------------------------*\
*  @RoutineName:: AmbaShell_Print
*
*  @Description:: Service function for getting the environment variable of an ambsh
*                 session based on the caller's task ID.
*
*  @Input      :: none
*
*  @Output     :: none
*
*  @Return     ::
*          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
AMBA_SHELL_ENV_s *AmbaShell_GetEnv(void)
{
    AMBA_SHELL_ENV_s *pEnv;
    AMBA_KAL_TASK_t *pTask = NULL;

    pTask = AmbaKAL_TaskIdentify();
    if (pTask == NULL)
        return NULL;

    for (pEnv = _pAmbaShellRootProc; pEnv != NULL; pEnv = pEnv->pChild) {
        if (pEnv->Task.tx_thread_id == pTask->tx_thread_id)
            return pEnv;
    }

    return NULL;
}

/*-----------------------------------------------------------------------------------------------*\
*  @RoutineName:: AmbaShell_Print
*
*  @Description:: Service function for formatted print.
*
*  @Input      :: none
*
*  @Output     :: none
*
*  @Return     ::
*          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
int AmbaShell_Print(AMBA_SHELL_ENV_s *pEnv, const char *fmt, ...)
{
    int l;
    char Buf[1024];
    va_list Args;

    if (pEnv == NULL)
        return -1;

    va_start(Args, fmt);
    l = vsprintf(Buf, fmt, Args);
    va_end(Args);
    Buf[l++] = '\0';

    return AmbaShell_Write(pEnv, (const UINT8 *) Buf, l);
}

/*-----------------------------------------------------------------------------------------------*\
*  @RoutineName:: AmbaShell_Printf
*
*  @Description:: Service function for formatted print.
*
*  @Input      :: none
*
*  @Output     :: none
*
*  @Return     ::
*          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
int AmbaShell_Printf(const char *fmt, ...)
{
    int l;
    AMBA_SHELL_ENV_s *pEnv;
    char Buf[1024];
    va_list Args;

    pEnv = AmbaShell_GetEnv();
    if (pEnv == NULL)
        return -1;

    va_start(Args, fmt);
    l = vsprintf(Buf, fmt, Args);
    va_end(Args);
    Buf[l++] = '\0';

    return AmbaShell_Write(pEnv, (const UINT8 *) Buf, l);
}

/*-----------------------------------------------------------------------------------------------*\
*  @RoutineName:: AmbaShell_GetStringInteractive
*
*  @Description:: Service function for getting a string from input stream (up to '\n').
*
*  @Input      :: none
*
*  @Output     :: none
*
*  @Return     ::
*          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
int AmbaShell_GetStringInteractive(AMBA_SHELL_ENV_s *pEnv, char *s, int n,
                                   int Echo, char Echochar)
{
    int l = 0;

    if (pEnv == NULL || s == NULL || n < 0)
        return -1;

    if (pEnv->ReadIo.Type != AMBSH_IO_CONSOLE ||
        pEnv->WriteIo.Type != AMBSH_IO_CONSOLE)
        return -2;

    AmbaKAL_TaskSuspend(&pEnv->pParent->Task);
    for (l = 0; l < n - 1; l++, s++) {
        if (AmbaShell_Read(pEnv, (UINT8 *) s, 1) <= 0)
            break;
        if (Echo) {
            if (Echochar != '\0')
                pEnv->WriteIo.ConsolePutCharCb(pEnv->pConsoleCtx, Echochar, (int)AMBA_KAL_WAIT_FOREVER);
            else
                pEnv->WriteIo.ConsolePutCharCb(pEnv->pConsoleCtx, *s, (int)AMBA_KAL_WAIT_FOREVER);
        }
        if (*s == '\n' || *s == '\r') {
            s++;
            *s = '\0';
            break;
        }
    }
    AmbaKAL_TaskResume(&pEnv->pParent->Task);

    return l;
}

/**
 * Service function for getting data from input stream.
 */
int AmbaShell_ReadTimeout(AMBA_SHELL_ENV_s *pEnv, UINT8 *pBuf, unsigned int Len, int Timeout)
{
    int Rval = 0;
    int l = 0;

    if (pEnv == NULL)
        return -1;

#ifndef AMBA_SHELL_NO_FS
    if (pEnv->ReadIo.pFileInput) {
        l = 0;
        do {
            Rval = AmbaFS_fread(pBuf + l, 1, (Len - l), pEnv->ReadIo.pFileInput);
            if (Rval != Len - l)
                break;
            l += Rval;
        } while (l < Len);
    }

    if (pEnv->ReadIo.pPipe) {
        for (l = 0; l < Len; l++) {
            AMBA_SHELL_PIPE_s *pPipe = (AMBA_SHELL_PIPE_s *) pEnv->ReadIo.pPipe;
            UINT32 data;
            AmbaKAL_MsgQueueReceive(&pPipe->MsgQueue, (void *)&data, 0);
        }
    }
#endif

    if ((pEnv->ReadIo.Type & AMBSH_IO_CONSOLE) && l == 0) {
        while (l < Len) {
            Rval = pEnv->ReadIo.ConsoleGetCharCb(pEnv->pConsoleCtx, (char *)pBuf + l, Timeout);
            if (Rval == 0)
                break;
            l += Rval;
        }
    }

    return l;
}

/**
 * Service function for getting data from input stream.
 */
int AmbaShell_Read(AMBA_SHELL_ENV_s *pEnv, UINT8 *buf, unsigned int Len)
{
    int NumRead;

    do {
        NumRead = AmbaShell_ReadTimeout(pEnv, buf, Len, (int)AMBA_KAL_WAIT_FOREVER);
    } while (NumRead == 0);

    return NumRead;
}

/**
 * Service function for writing to output stream.
 */
int AmbaShell_Write(AMBA_SHELL_ENV_s *pEnv, const UINT8 *pBuf, unsigned int Len)
{
#ifndef AMBA_SHELL_NO_FS
    int Rval = 0;
#endif
    int l = 0;

    if (pEnv == NULL)
        return -1;

#ifndef AMBA_SHELL_NO_FS
    if (pEnv->WriteIo.pFileOut) {
        l = 0;
        if (pBuf[Len - 1] == '\0')
            Len--;
        do {
            Rval = AmbaFS_fwrite((UINT8 *) pBuf + l, 1, Len - l, pEnv->WriteIo.pFileOut);
            if (Rval != Len - l)
                break;
            l += Rval;
        } while (l < Len);
    }

    if (pEnv->WriteIo.pFileAppend) {
        l = 0;
        if (pBuf[Len - 1] == '\0')
            Len--;
        do {
            Rval = AmbaFS_fwrite((UINT8 *) pBuf + l, 1, Len - l, pEnv->WriteIo.pFileAppend);
            if (Rval < 0)
                break;
            l += Rval;
        } while (l < Len);
    }

    if (pEnv->WriteIo.pPipe) {
        for (l = 0; l < Len; l++) {
            int Ercd;
            AMBA_SHELL_PIPE_s *pPipe = (AMBA_SHELL_PIPE_s *) pEnv->WriteIo.pPipe;
            int Data;
            do {
                Data = pBuf[l];
                Ercd = AmbaKAL_MsgQueueSend(&pPipe->MsgQueue, &Data, 0);
                if (Ercd != OK)
                    Ercd = AmbaKAL_MsgQueueReceive(&pPipe->MsgQueue, &Data, 0);
            } while (Ercd != OK);
        }
    }
#endif

    if ((pEnv->WriteIo.Type & AMBSH_IO_CONSOLE) && l == 0) {
        for (l = 0; l < Len; l++) {
            if (pBuf[l] == '\n')
                pEnv->WriteIo.ConsolePutCharCb(pEnv->pConsoleCtx, '\r', (int)AMBA_KAL_WAIT_FOREVER);
            pEnv->WriteIo.ConsolePutCharCb(pEnv->pConsoleCtx, pBuf[l], (int)AMBA_KAL_WAIT_FOREVER);
        }
    }

    return l;
}

const char *AmbaShell_GetFsError(void)
{
    int Error;

    AmbaFS_GetError(&Error);

#if defined(SUPPORT_ROMFS)
    if (Type == AMBA_FS_RFS)
        return AmbaROMFS_GetError(Error);
#endif

    return "unknown prfile error";
}

/**
 * Service function for flushing output stream.
 */
int AmbaShell_Flush(AMBA_SHELL_ENV_s *pEnv)
{
    if (pEnv == NULL)
        return -1;

    if ((pEnv->WriteIo.Type & AMBSH_IO_CONSOLE)) {
        pEnv->WriteIo.ConsoleFlushCb(pEnv->pConsoleCtx);
    }

    return 0;
}

/**
 * Get Heap base address.
 */
AMBA_KAL_BYTE_POOL_t *AmbaShell_GetHeap(void)
{
    return AmbaShellConfig.pCachedHeap;
}

/**
 *
 */
/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaShell_Init
 *
 *  @Description:: Initialize ambsh environment.
 *
 *  @Input      ::
 *      pCachedHeap:        external memory for heap purpose
 *      ConsolePutCharCb:   Console put char function
 *      ConsoleGetCharCb:   COnsole get char function
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
void AmbaShell_Init(AMBA_SHELL_CONFIG_s *pShellConfig)
{
    extern void reset_lexer(void);
    int i;

    memcpy(&AmbaShellConfig, pShellConfig, sizeof(AMBA_SHELL_CONFIG_s));

    AmbaKAL_MutexCreate(&_AmbaShell_EnvMutex);
    AmbaKAL_MutexCreate(&_AmbaShell_CmdListMutex);

    /* Sanitize the shell command list */
    for (i = 0; i < AmbaShell_DefaultCmdListSize - 1; i++) {
        AmbaShell_DefaultCmdList[i].pNext = &AmbaShell_DefaultCmdList[i + 1];
    }
    AmbaShell_DefaultCmdList[i].pNext = NULL;

    _AmbaShell_NumRawData = 0;

    reset_lexer();
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaShell_Start
 *
 *  @Description:: Start ambsh
 *
 *  @Input      ::
 *      NumAutoExecScript:  Number of automatic execution scripts
 *      pAutoExecScript:    Automatic execution scripts
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
int AmbaShell_Start(int NumAutoExecScript, char *pAutoExecScript[])
{
#ifndef AMBA_SHELL_NO_FS
    int i;
    AMBA_FS_STAT FileStat;
    AMBA_SHELL_ENV_s *pAutoProc = NULL;
#endif

    if (_pAmbaShellRootProc != NULL)
        return OK;

    /* Create the root shell */
    SHELL_Create(NULL, NULL);

    /* Start the root shell */
    if (_pAmbaShellRootProc != NULL)
        AmbaKAL_TaskResume(&_pAmbaShellRootProc->Task);

#ifndef AMBA_SHELL_NO_FS
    if (pAutoExecScript == NULL)
        NumAutoExecScript = 0;

    /* Delay 1 sec to finish the I/O initialization and flush */
    /* out the screen (as a result of boot-up printks). */
    AmbaKAL_TaskSleep(1000);

    /* Search for autoexec script on file systems */
    for (i = 0; i < NumAutoExecScript; i++) {
        if (AmbaFS_Stat(pAutoExecScript[i], &FileStat))
            continue;

        /* Found a valid script file, execute it! */
        AmbaPrint("execute script: %s ...", pAutoExecScript[i]);
        pAutoProc = SHELL_Create(NULL, NULL);
        pAutoProc->Cwd[0] = pAutoExecScript[i][0];
        pAutoProc->Cwd[1] = pAutoExecScript[i][1];
        pAutoProc->Cwd[2] = pAutoExecScript[i][2];
        pAutoProc->Cwd[3] = '\0';
        pAutoProc->Argc = 1;
        strcpy(pAutoProc->Argv[0], pAutoExecScript[i]);
        AmbaKAL_TaskResume(&pAutoProc->Task);

        /* Wait for the script to finish */
        do {
            AmbaKAL_TaskSleep(100);
        } while (pAutoProc->Task.tx_thread_state != TX_COMPLETED &&
                 pAutoProc->Task.tx_thread_state != TX_TERMINATED);

        SHELL_Destroy(pAutoProc);
        break;
    }
#endif

    return OK;
}
