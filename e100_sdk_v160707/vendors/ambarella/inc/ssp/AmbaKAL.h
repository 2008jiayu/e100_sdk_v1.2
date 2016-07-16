/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaKAL.h
 *
 *  @Copyright      :: Copyright (C) 2012 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Definitions for Ambarella KAL (Kernel Abstraction Layer)
\*-------------------------------------------------------------------------------------------------------------------*/

#ifndef _AMBA_KAL_H_
#define _AMBA_KAL_H_

#ifndef AMBA_KAL_NO_SMP
#define AMBA_KAL_NO_SMP
#endif

#ifndef AMBA_KAL_NO_SMP     /* if not specify NO_SMP, then support SMP */
#include "threadx/tx_api_SMP.h"
#define AMBA_KAL_CPU_CORE_MASK          TX_THREAD_SMP_CORE_MASK
#else
#include "threadx/tx_api_Uniprocessor.h"
#define AMBA_KAL_CPU_CORE_MASK          1
#endif  /* AMBA_KAL_NO_SMP */

#define AMBA_KAL_VER                    0x020000                            /* AmbaKAL version number: ##.##.## */

#define AMBA_KAL_SYS_TICK               1000                        /* System tick (Hz) */
#define AMBA_KAL_SYS_TICK_MS            (1000 / AMBA_KAL_SYS_TICK)  /* System tick (ms) */
#define AMBA_KAL_MiliSec2SysTick(ms)    (((ms) * AMBA_KAL_SYS_TICK) / 1000) /* convert Mili-Seconds to System-Ticks */

#define AMBA_KAL_TASK_HIGHEST_PRIORITY  0
#define AMBA_KAL_TASK_LOWEST_PRIORITY   (TX_MAX_PRIORITIES - 1)

typedef TX_THREAD                       AMBA_KAL_TASK_t;            /* Software Task */
typedef TX_SEMAPHORE                    AMBA_KAL_SEM_t;             /* Counting Semaphore */
typedef TX_MUTEX                        AMBA_KAL_MUTEX_t;           /* Mutex */
typedef TX_EVENT_FLAGS_GROUP            AMBA_KAL_EVENT_FLAG_t;      /* Event Flags */
typedef TX_QUEUE                        AMBA_KAL_MSG_QUEUE_t;       /* Message Queue */
typedef TX_TIMER                        AMBA_KAL_TIMER_t;           /* Application Timer */
typedef TX_BYTE_POOL                    AMBA_KAL_BYTE_POOL_t;       /* Memory Byte Pool */
typedef TX_BLOCK_POOL                   AMBA_KAL_BLOCK_POOL_t;      /* Memory Block Pool */

#define AMBA_KAL_NO_WAIT                TX_NO_WAIT
#define AMBA_KAL_WAIT_FOREVER           TX_WAIT_FOREVER

#define AMBA_KAL_AND                    TX_AND
#define AMBA_KAL_AND_CLEAR              TX_AND_CLEAR
#define AMBA_KAL_OR                     TX_OR
#define AMBA_KAL_OR_CLEAR               TX_OR_CLEAR

#define AMBA_KAL_AUTO_START             TX_AUTO_START
#define AMBA_KAL_DO_NOT_START           TX_DONT_START

#define AMBA_KAL_TASK_ENTRY             TX_THREAD_ENTRY
#define AMBA_KAL_TASK_EXIT              TX_THREAD_EXIT

/*-------------------------------------------------------*\
 * Software task current execution state
\*-------------------------------------------------------*/
#define AMBA_KAL_TASK_READY             TX_READY
#define AMBA_KAL_TASK_COMPLETED         TX_COMPLETED
#define AMBA_KAL_TASK_TERMINATED        TX_TERMINATED
#define AMBA_KAL_TASK_SUSPENDED         TX_SUSPENDED
#define AMBA_KAL_TASK_SLEEP             TX_SLEEP

typedef struct _AMBA_KAL_TASK_INFO_s_ {
    UINT32  CurState;                   /* current state */
    UINT32  Priority;                   /* priority */
    char    *pTaskName;                 /* pointer to the name of the software task */
} AMBA_KAL_TASK_INFO_s;

/*-----------------------------------------------------------------------------------------------*\
 * Defined in AmbaKAL_ThreadX.c
\*-----------------------------------------------------------------------------------------------*/
int AmbaKAL_TaskCreate(AMBA_KAL_TASK_t *pTask, char *pTaskName, UINT32 Priority,
                       void (*EntryFunction)(UINT32), UINT32 EntryArg,
                       void *pStackBase, UINT32 StackByteSize, UINT32 AutoStart);
int AmbaKAL_TaskDelete(AMBA_KAL_TASK_t *pTask);
int AmbaKAL_TaskTerminate(AMBA_KAL_TASK_t *pTask);
int AmbaKAL_TaskReset(AMBA_KAL_TASK_t *pTask);
int AmbaKAL_TaskSuspend(AMBA_KAL_TASK_t *pTask);
int AmbaKAL_TaskResume(AMBA_KAL_TASK_t *pTask);
int AmbaKAL_TaskSleep(int TimeValue);
int AmbaKAL_TaskQuery(AMBA_KAL_TASK_t *pTask, AMBA_KAL_TASK_INFO_s *pTaskInfo);
int AmbaKAL_TaskChangePriority(AMBA_KAL_TASK_t *pTask, UINT32 NewPriority, UINT32 *pOldPriority);
int AmbaKAL_TaskChangeTimeSlice(AMBA_KAL_TASK_t *pTask, UINT32 NewTimeSlice, UINT32 *pOldTimeSlice);
AMBA_KAL_TASK_t *AmbaKAL_TaskIdentify(void);

int AmbaKAL_TaskFpuEnable(void);
int AmbaKAL_TaskFpuDisable(void);

int AmbaKAL_TaskUserNotify(AMBA_KAL_TASK_t *pTask, void (*UserFunction)(AMBA_KAL_TASK_t *, UINT32));
int AmbaKAL_TaskUserValueGet(AMBA_KAL_TASK_t *pTask, UINT32 *pUserValue);
int AmbaKAL_TaskUserValueSet(AMBA_KAL_TASK_t *pTask, UINT32 UserValue);

int AmbaKAL_RegisterStackErrorHandler(void (*StackErrorHandler)(AMBA_KAL_TASK_t *));
int AmbaKAL_IsInISR(void);

int AmbaKAL_EnterCriticalSection(UINT32 *pProcessorStatusRegister);
int AmbaKAL_ExitCriticalSection(UINT32 ProcessorStatusRegister);

int AmbaKAL_SemCreate(AMBA_KAL_SEM_t *pSem, UINT32 InitCount);
int AmbaKAL_SemDelete(AMBA_KAL_SEM_t *pSem);
int AmbaKAL_SemTake(AMBA_KAL_SEM_t *pSem, UINT32 Timeout);
int AmbaKAL_SemGive(AMBA_KAL_SEM_t *pSem);
int AmbaKAL_SemQuery(AMBA_KAL_SEM_t *pSem, UINT32 *pCurCount);

int AmbaKAL_MutexCreate(AMBA_KAL_MUTEX_t *pMutex);
int AmbaKAL_MutexDelete(AMBA_KAL_MUTEX_t *pMutex);
int AmbaKAL_MutexTake(AMBA_KAL_MUTEX_t *pMutex, UINT32 Timeout);
int AmbaKAL_MutexGive(AMBA_KAL_MUTEX_t *pMutex);

int AmbaKAL_EventFlagCreate(AMBA_KAL_EVENT_FLAG_t *pEventFlag);
int AmbaKAL_EventFlagDelete(AMBA_KAL_EVENT_FLAG_t *pEventFlag);
int AmbaKAL_EventFlagTake(AMBA_KAL_EVENT_FLAG_t *pEventFlag, UINT32 ReqFlags, UINT32 Option,
                          UINT32 *pActualFlags, UINT32 Timeout);
int AmbaKAL_EventFlagGive(AMBA_KAL_EVENT_FLAG_t *pEventFlag, UINT32 Flags);
int AmbaKAL_EventFlagClear(AMBA_KAL_EVENT_FLAG_t *pEventFlag, UINT32 Flags);
int AmbaKAL_EventFlagQuery(AMBA_KAL_EVENT_FLAG_t *pEventFlag, UINT32 *pCurFlags);

int AmbaKAL_MsgQueueCreate(AMBA_KAL_MSG_QUEUE_t *pMsgQueue, void *pMsgQueueBase, UINT32 MsgSize, UINT32 MaxNumMsg);
int AmbaKAL_MsgQueueDelete(AMBA_KAL_MSG_QUEUE_t *pMsgQueue);
int AmbaKAL_MsgQueueFlush(AMBA_KAL_MSG_QUEUE_t *pMsgQueue);
int AmbaKAL_MsgQueueReceive(AMBA_KAL_MSG_QUEUE_t *pMsgQueue, void *pMsgDest, UINT32 Timeout);
int AmbaKAL_MsgQueueSend(AMBA_KAL_MSG_QUEUE_t *pMsgQueue, void *pMsgSource, UINT32 Timeout);
int AmbaKAL_MsgQueueQuery(AMBA_KAL_MSG_QUEUE_t *pMsgQueue, UINT32 *pCurCount, UINT32 *pCurRemainedSpace);

int AmbaKAL_TimerCreate(AMBA_KAL_TIMER_t *pTimer, UINT32 AutoStart,
                        void (*ExpirationFunction)(UINT32), UINT32 ExpirationArg,
                        UINT32 InitTicks, UINT32 ReloadTicks);
int AmbaKAL_TimerDelete(AMBA_KAL_TIMER_t *pTimer);
int AmbaKAL_TimerStart(AMBA_KAL_TIMER_t *pTimer);
int AmbaKAL_TimerStop(AMBA_KAL_TIMER_t *pTimer);
int AmbaKAL_TimerChange(AMBA_KAL_TIMER_t *pTimer, UINT32 InitTicks, UINT32 ReloadTicks, UINT32 AutoStart);

UINT32 AmbaKAL_GetTickCount(void);

/*-----------------------------------------------------------------------------------------------*/
int AmbaKAL_BytePoolCreate(AMBA_KAL_BYTE_POOL_t *pBytePool, void *pPoolBase, UINT32 PoolByteSize);
int AmbaKAL_BytePoolDelete(AMBA_KAL_BYTE_POOL_t *pBytePool);
int AmbaKAL_BytePoolAllocate(AMBA_KAL_BYTE_POOL_t *pBytePool, void **pMemBase, UINT32 MemByteSize, UINT32 Timeout);
int AmbaKAL_BytePoolFree(void *pMemBase); 
int AmbaKAL_BytePoolInfoGet(AMBA_KAL_BYTE_POOL_t *pBytePool, UINT32 *PoolFreeByteSize); 

int AmbaKAL_BlockPoolCreate(AMBA_KAL_BLOCK_POOL_t *pBlockPool, UINT32 BlockByteSize, void *pPoolBase, UINT32 PoolByteSize);
int AmbaKAL_BlockPoolDelete(AMBA_KAL_BLOCK_POOL_t *pBlockPool);
int AmbaKAL_BlockPoolAllocate(AMBA_KAL_BLOCK_POOL_t *pBlockPool, void **pBlockBase, UINT32 Timeout);
int AmbaKAL_BlockPoolFree(void *pBlockBase);

typedef struct _AMBA_MEM_CTRL_s_ {
    void    *pMemBase;          /* pointer to the base address of Allocated memory */
    void    *pMemAlignedBase;   /* pointer to the base address of Aligned memory */
} AMBA_MEM_CTRL_s;

int AmbaKAL_MemAllocate(AMBA_KAL_BYTE_POOL_t *pBytePool, AMBA_MEM_CTRL_s *pMemCtrl, UINT32 MemByteSize, UINT32 Alignment);
int AmbaKAL_MemFree(AMBA_MEM_CTRL_s *pMemCtrl);

/*-----------------------------------------------------------------------------------------------*\
 * Symmetric Multi-Processing (SMP) support
\*-----------------------------------------------------------------------------------------------*/
int AmbaKAL_TaskSmpCoreExclusionSet(AMBA_KAL_TASK_t *pTask, UINT32 ExclusionMap);
int AmbaKAL_TaskSmpCoreExclusionGet(AMBA_KAL_TASK_t *pTask, UINT32 *pExclusionMap);
int AmbaKAL_TaskSmpCurCoreGet(void);
int AmbaKAL_TimerSmpCoreExclusionSet(AMBA_KAL_TIMER_t *pTimer, UINT32 ExclusionMap);
int AmbaKAL_TimerSmpCoreExclusionGet(AMBA_KAL_TIMER_t *pTimer, UINT32 *pExclusionMap);

int AmbaKAL_TaskSmpCoreInclusionSet(AMBA_KAL_TASK_t *pTask, UINT32 InclusionMap);
int AmbaKAL_TaskSmpCoreInclusionGet(AMBA_KAL_TASK_t *pTask, UINT32 *pInclusionMap);
int AmbaKAL_TimerSmpCoreInclusionSet(AMBA_KAL_TIMER_t *pTimer, UINT32 InclusionMap);
int AmbaKAL_TimerSmpCoreInclusionGet(AMBA_KAL_TIMER_t *pTimer, UINT32 *pInclusionMap);
/*-----------------------------------------------------------------------------------------------*/

#endif /* _AMBA_KAL_H_ */
