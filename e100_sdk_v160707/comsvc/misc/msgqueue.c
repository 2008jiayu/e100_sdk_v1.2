/**
 * @file src/mw/utils/MsgQueue.c
 *
 * Message queue handler that allows user to send larger message.   \n
 * The message queue in kernel has a 64-byte limit to each message.
 *
 * History:
 *    2014/01/16 - [phcheng] Create
 *
 * Copyright (C) 2013, Ambarella, Inc.
 *
 * All rights reserved. No Part of this file may be reproduced, stored
 * in a retrieval system, or transmitted, in any form, or by any means,
 * electronic, mechanical, photocopying, recording, or otherwise,
 * without the prior consent of Ambarella, Inc.
 */

#include <AmbaDataType.h>
#include <AmbaKAL.h>
#include <AmbaPrintk.h>
#include <msgqueue.h>

//#define AMP_MESSAGE_QUEUE_DEBUG ///< Printk when a message is enqueued or dequeued.

#define MIN(a,b) ((a)<(b)?(a):(b))

/**
 * An internal function that moves write pointer of a message queue handler to the next location.   \n
 * Assume all inputs are valid, so make sure they've been checked before.
 *
 * @param [in] MsgQueueHandler      Message queue handler.
 *
 * @return 0 - OK, others - Error
 */
static int AmpMsgQueue_MoveWritePointer_Internal(AMP_MSG_QUEUE_HDLR_s *MsgQueueHandler)
{
    UINT8 *MsgQueueLimit;       // The first address in which the data CANNOT be accessed
    UINT8 *WritePointerNext;    // A temporary space storing the next write point

    MsgQueueLimit = (UINT8*)MsgQueueHandler->MsgBufBase + (MsgQueueHandler->MsgSize * MsgQueueHandler->MaxNumMsg);
    WritePointerNext = (UINT8*)MsgQueueHandler->WritePointer + MsgQueueHandler->MsgSize;

    if (WritePointerNext >= MsgQueueLimit) { // Overstep the limit. NOTE: "=" is needed!
        // Ring buffer
        WritePointerNext = MsgQueueHandler->MsgBufBase;
    }
#ifdef AMP_MESSAGE_QUEUE_DEBUG
    AmbaPrintColor(GREEN, "[MW - Message Queue] <WP++> MsgBufBase = 0x%08x, MsgQueueLimit = 0x%08x, WP Before = 0x%08x, WP After = 0x%08x",
            MsgQueueHandler->MsgBufBase,
            MsgQueueLimit,
            MsgQueueHandler->WritePointer,
            WritePointerNext);
#endif // AMP_MESSAGE_QUEUE_DEBUG

    // Set write pointer
    MsgQueueHandler->WritePointer = WritePointerNext;

    return 0; // Success
}

/**
 * An internal function that moves read pointer of a message queue handler to the next location.    \n
 * Assume all inputs are valid, so make sure they've been checked before.
 *
 * @param [in] MsgQueueHandler      Message queue handler.
 *
 * @return 0 - OK, others - Error
 */
static int AmpMsgQueue_MoveReadPointer_Internal(AMP_MSG_QUEUE_HDLR_s *MsgQueueHandler)
{
    UINT8 *MsgQueueLimit;   // The first address in which the data CANNOT be accessed
    UINT8 *ReadPointerNext; // A temporary space storing the next read point

    MsgQueueLimit = (UINT8*)MsgQueueHandler->MsgBufBase + (MsgQueueHandler->MsgSize * MsgQueueHandler->MaxNumMsg);
    ReadPointerNext = (UINT8*)MsgQueueHandler->ReadPointer + MsgQueueHandler->MsgSize;

    if (ReadPointerNext >= MsgQueueLimit) { // Overstep the limit. NOTE: "=" is needed!
        // Ring buffer
        ReadPointerNext = MsgQueueHandler->MsgBufBase;
    }
#ifdef AMP_MESSAGE_QUEUE_DEBUG
    AmbaPrintColor(CYAN, "[MW - Message Queue] <RP++> MsgBufBase = 0x%08x, MsgQueueLimit = 0x%08x, RP Before = 0x%08x, RP After = 0x%08x",
            MsgQueueHandler->MsgBufBase,
            MsgQueueLimit,
            MsgQueueHandler->ReadPointer,
            ReadPointerNext);
#endif // AMP_MESSAGE_QUEUE_DEBUG

    // Set read pointer
    MsgQueueHandler->ReadPointer = ReadPointerNext;

    return 0; // Success
}

int AmpMsgQueue_Create(AMP_MSG_QUEUE_HDLR_s *MsgQueueHandler, void *MsgBufBase, const UINT32 MsgSize, const UINT32 MaxNumMsg)
{
    int ReturnValue = AMP_MQ_SUCCESS; // Return value
    int Rval = 0; // Function call return

    // Preliminary check
    if (MsgQueueHandler == NULL) {
        return AMP_MQ_INVALID_ERROR; // Invalid pointer of the message queue handler.
    }
    if (AmpMsgQueue_IsCreated(MsgQueueHandler)) {
        return AMP_MQ_CREATED_ERROR; // The message queue handler is already created.
    }
    if (MsgBufBase == NULL) {
        return AMP_MQ_BUF_ADDR_ERROR; // Invalid starting address of the message buffer.
    }
    if ((MsgSize * MaxNumMsg > MSG_QUEUE_SIZE_MAX)) {
        AmbaPrint("%s:%u Size of message queue buffer exceeds the limit of (%d) Bytes!", __FUNCTION__, __LINE__, MSG_QUEUE_SIZE_MAX);
        return AMP_MQ_BUF_SIZE_ERROR; // Invalid size of the message buffer.
    }
    if ((MsgSize * MaxNumMsg < MSG_QUEUE_SIZE_MIN)) {
        AmbaPrint("%s:%u Size of message queue buffer lower than the limit of (%d) Bytes!", __FUNCTION__, __LINE__, MSG_QUEUE_SIZE_MIN);
        return AMP_MQ_BUF_SIZE_ERROR; // Invalid size of the message buffer.
    }

    MsgQueueHandler->IsCreated = 1;
    MsgQueueHandler->MsgBufBase = MsgBufBase;
    MsgQueueHandler->MsgSize = MsgSize;
    MsgQueueHandler->MaxNumMsg = MaxNumMsg;
    MsgQueueHandler->ReadPointer = MsgBufBase;
    MsgQueueHandler->WritePointer = MsgBufBase;
    Rval = AmbaKAL_MutexCreate(&MsgQueueHandler->ReadMutex);
    if (Rval != 0) {
        AmbaPrint("%s:%u Failed to create mutex! (%d)", __FUNCTION__, __LINE__, Rval);
        ReturnValue = AMP_MQ_MUTEX_CREATE_ERROR;
        goto ReturnError;
    }
    Rval = AmbaKAL_MutexCreate(&MsgQueueHandler->WriteMutex);
    if (Rval != 0) {
        AmbaPrint("%s:%u Failed to create mutex! (%d)", __FUNCTION__, __LINE__, Rval);
        ReturnValue = AMP_MQ_MUTEX_CREATE_ERROR;
        goto ReturnError;
    }
    Rval = AmbaKAL_SemCreate(&MsgQueueHandler->MsgQueueReceiveSem, 0);
    if (Rval != 0) {
        AmbaPrint("%s:%u Failed to create semaphore! (%d)", __FUNCTION__, __LINE__, Rval);
        ReturnValue = AMP_MQ_SEM_CREATE_ERROR;
        goto ReturnError;
    }
    Rval = AmbaKAL_SemCreate(&MsgQueueHandler->MsgQueueSendSem, MaxNumMsg);
    if (Rval != 0) {
        AmbaPrint("%s:%u Failed to create semaphore! (%d)", __FUNCTION__, __LINE__, Rval);
        ReturnValue = AMP_MQ_SEM_CREATE_ERROR;
        goto ReturnError;
    }

    return AMP_MQ_SUCCESS; // Success

ReturnError:
    AmpMsgQueue_Delete(MsgQueueHandler);
    return ReturnValue;
}

int AmpMsgQueue_Delete(AMP_MSG_QUEUE_HDLR_s *MsgQueueHandler)
{
    int ReturnValue = AMP_MQ_SUCCESS; // Return value
    int Rval = 0; // Function call return

    // Preliminary check
    if (MsgQueueHandler == NULL) {
        return AMP_MQ_INVALID_ERROR; // Invalid pointer of the message queue handler.
    }
    if (AmpMsgQueue_IsCreated(MsgQueueHandler) == 0) {
        return AMP_MQ_NOT_CREATED_ERROR; // The message queue handler is not created.
    }

    Rval = AmbaKAL_SemDelete(&MsgQueueHandler->MsgQueueSendSem);
    if (Rval != 0) {
        AmbaPrint("%s:%u Failed to delete semaphore! (%d)", __FUNCTION__, __LINE__, Rval);
        ReturnValue = AMP_MQ_SEM_DELETE_ERROR;
    }
    Rval = AmbaKAL_SemDelete(&MsgQueueHandler->MsgQueueReceiveSem);
    if (Rval != 0) {
        AmbaPrint("%s:%u Failed to delete semaphore! (%d)", __FUNCTION__, __LINE__, Rval);
        ReturnValue = AMP_MQ_SEM_DELETE_ERROR;
    }
    Rval = AmbaKAL_MutexDelete(&MsgQueueHandler->WriteMutex);
    if (Rval != 0) {
        AmbaPrint("%s:%u Failed to delete mutex! (%d)", __FUNCTION__, __LINE__, Rval);
        ReturnValue = AMP_MQ_MUTEX_DELETE_ERROR;
    }
    Rval = AmbaKAL_MutexDelete(&MsgQueueHandler->ReadMutex);
    if (Rval != 0) {
        AmbaPrint("%s:%u Failed to delete mutex! (%d)", __FUNCTION__, __LINE__, Rval);
        ReturnValue = AMP_MQ_MUTEX_DELETE_ERROR;
    }
    MsgQueueHandler->WritePointer = 0;
    MsgQueueHandler->ReadPointer = 0;
    MsgQueueHandler->MaxNumMsg = 0;
    MsgQueueHandler->MsgSize = 0;
    MsgQueueHandler->MsgBufBase = 0;
    MsgQueueHandler->IsCreated = 0;

    return ReturnValue;
}

int AmpMsgQueue_Flush(AMP_MSG_QUEUE_HDLR_s *MsgQueueHandler)
{
    int ReturnValue = AMP_MQ_SUCCESS; // Return value
    int Rval = 0; // Function call return.

    // Preliminary check
    if (MsgQueueHandler == NULL) {
        return AMP_MQ_INVALID_ERROR; // Invalid pointer of the message queue handler.
    }
    if (AmpMsgQueue_IsCreated(MsgQueueHandler) == 0) {
        return AMP_MQ_NOT_CREATED_ERROR; // The message queue handler is not created.
    }

    // Take mutex
    Rval = AmbaKAL_MutexTake(&MsgQueueHandler->WriteMutex, AMBA_KAL_WAIT_FOREVER);
    if (Rval != 0) {
        AmbaPrint("%s:%u Failed to take mutex!(%d)", __FUNCTION__, __LINE__, Rval);
        return AMP_MQ_MUTEX_TAKE_ERROR; // Failed to take mutex.
    }
    Rval = AmbaKAL_MutexTake(&MsgQueueHandler->ReadMutex, AMBA_KAL_WAIT_FOREVER);
    if (Rval != 0) {
        AmbaPrint("%s:%u Failed to take mutex!(%d)", __FUNCTION__, __LINE__, Rval);
        // Give mutex
        Rval = AmbaKAL_MutexGive(&MsgQueueHandler->WriteMutex);
        if (Rval != 0) {
            AmbaPrint("%s:%u Failed to give mutex!(%d)", __FUNCTION__, __LINE__, Rval);
            return AMP_MQ_MUTEX_GIVE_ERROR; // Failed to give mutex.
        }
        return AMP_MQ_MUTEX_TAKE_ERROR; // Failed to take mutex.
    }

    // Rebuild semaphore
    Rval = AmbaKAL_SemDelete(&MsgQueueHandler->MsgQueueReceiveSem);
    if (Rval != 0) {
        AmbaPrintColor(RED, "%s:%u Failed to delete semaphore!(%d)", __FUNCTION__, __LINE__, Rval);
        ReturnValue = AMP_MQ_SEM_DELETE_ERROR; // Failed to delete semaphore. An unexpected error.
    }
    Rval = AmbaKAL_SemDelete(&MsgQueueHandler->MsgQueueSendSem);
    if (Rval != 0) {
        AmbaPrintColor(RED, "%s:%u Failed to delete semaphore!(%d)", __FUNCTION__, __LINE__, Rval);
        ReturnValue = AMP_MQ_SEM_DELETE_ERROR; // Failed to delete semaphore. An unexpected error.
    }
    Rval = AmbaKAL_SemCreate(&MsgQueueHandler->MsgQueueReceiveSem, 0);
    if (Rval != 0) {
        AmbaPrintColor(RED, "%s:%u Failed to create semaphore!(%d)", __FUNCTION__, __LINE__, Rval);
        ReturnValue = AMP_MQ_SEM_CREATE_ERROR; // Failed to create semaphore. An unexpected error.
    }
    Rval = AmbaKAL_SemCreate(&MsgQueueHandler->MsgQueueSendSem, MsgQueueHandler->MaxNumMsg);
    if (Rval != 0) {
        AmbaPrintColor(RED, "%s:%u Failed to create semaphore!(%d)", __FUNCTION__, __LINE__, Rval);
        ReturnValue = AMP_MQ_SEM_CREATE_ERROR; // Failed to create semaphore. An unexpected error.
    }

    // Reset Read/Write Pointer
    MsgQueueHandler->ReadPointer = MsgQueueHandler->WritePointer = MsgQueueHandler->MsgBufBase;

    // Give mutex
    Rval = AmbaKAL_MutexGive(&MsgQueueHandler->ReadMutex);
    if (Rval != 0) {
        AmbaPrintColor(RED, "%s:%u Failed to give mutex!(%d)", __FUNCTION__, __LINE__, Rval);
        ReturnValue = AMP_MQ_MUTEX_GIVE_ERROR; // Failed to give mutex. An unexpected error.
    }
    Rval = AmbaKAL_MutexGive(&MsgQueueHandler->WriteMutex);
    if (Rval != 0) {
        AmbaPrintColor(RED, "%s:%u Failed to give mutex!(%d)", __FUNCTION__, __LINE__, Rval);
        ReturnValue = AMP_MQ_MUTEX_GIVE_ERROR; // Failed to give mutex. An unexpected error.
    }

    return ReturnValue;
}

/**
 * An internal function that get remaining time for a function.
 * Assume all inputs are valid, so make sure they've been checked before.
 *
 * @param [in] Timeout              The maximum running time (in ms) for a function.
 * @param [in] TimeLimit            The limit of time (in ms) for running a function.
 * @param [out] OutputTimeLeft      Time remained (in ms).
 *
 * @return 0 - OK, others - Error
 */
static int AmpMsgQueue_GetTimeLeft_Internal(const UINT32 Timeout, const UINT32 TimeLimit, UINT32 *OutputTimeLeft)
{
    if (Timeout == AMBA_KAL_WAIT_FOREVER) {
        *OutputTimeLeft = AMBA_KAL_WAIT_FOREVER;
    } else if (TimeLimit > AmbaSysTimer_GetTickCount()) {
        *OutputTimeLeft = TimeLimit - AmbaSysTimer_GetTickCount();
    } else {
        *OutputTimeLeft = 0;
    }

    return 0; // Success
}

int AmpMsgQueue_Receive(AMP_MSG_QUEUE_HDLR_s *MsgQueueHandler, void *OutputMsgDest, const UINT32 Timeout)
{
    int Rval = 0; // Function call return.
    const UINT32 TimeLimit = (AmbaSysTimer_GetTickCount() + Timeout >= Timeout) ? // In case of overflow
                             (AmbaSysTimer_GetTickCount() + Timeout) :
                             (AMBA_KAL_WAIT_FOREVER); // The limit of time (in ms) for running this function

// Use the limited waiting time to avoid dead lock. Consider this situation (it's a dead lock):
// 1. Thread A (Send message): Waiting for WriteMutex.
// 2. Thread B (Receive message): Took ReadMutex. Waiting for MsgQueueReceiveSem (Queue is empty).
// 3. Thread C (Flush): Took WriteMutex. Waiting for ReadMutex.
// Changing the order of taking WriteMutex and ReadMutex in Flush function won't help.
// If we take ReadMutex first in the flush function, a dead lock could happen when the queue is full.
// However, if there's a limited time of taking semaphore, the dead lock problem can be solved.
#define MQ_RECEIVE_TAKE_SEM_TIMEOUT (2) ///< The limited waiting time of taking semaphore while receiving a message.
// Sleep for a while to avoid busy waiting. But not too long. Recall that a frame must be processed
// within 17 ms while decoding videos.
#define MQ_RECEIVE_SLEEP_TIME (2) ///< Sleep for a while after a unsuccessful message reception to avoid busy waiting.

    // Preliminary check
    if (MsgQueueHandler == NULL) {
        return AMP_MQ_INVALID_ERROR; // Invalid pointer of the message queue handler.
    }
    if (AmpMsgQueue_IsCreated(MsgQueueHandler) == 0) {
        return AMP_MQ_NOT_CREATED_ERROR; // The message queue handler is not created.
    }
    if (OutputMsgDest == NULL) {
        return AMP_MQ_MSG_DEST_ERROR; // Invalid destination pointer for message.
    }

    while(1) {
        UINT32 TimeLeft = 0; // Time remained (in ms)
        UINT32 MutexTimeout = 0; // Maximum waiting time (in ms) for a mutex
        UINT32 SemTimeout = 0; // Maximum waiting time (in ms) for a semaphore
        // Get remaining time
        AmpMsgQueue_GetTimeLeft_Internal(Timeout, TimeLimit, &TimeLeft);
        // Set timeout
        MutexTimeout = TimeLeft;
        // Take mutex
        Rval = AmbaKAL_MutexTake(&MsgQueueHandler->ReadMutex, MutexTimeout);
        // Check result
        if (Rval == TX_NOT_AVAILABLE) { // TODO: Need DSP definition of TX_NOT_AVAILABLE
            AmbaPrint("%s:%u Take mutex timeout (over %u ms).", __FUNCTION__, __LINE__, MutexTimeout);
            return AMP_MQ_TIMEOUT_ERROR; // Time out
        } else if (Rval == TX_DELETED) { // TODO: Need DSP definition of TX_DELETED
            AmbaPrint("%s:%u Stop receiving message since the queue has been deleted (%d).", __FUNCTION__, __LINE__, Rval);
            return AMP_MQ_DELETED; // The queue has been deleted.
        } else if (Rval != 0) {
            // Sleep for a while to avoid busy waiting.
            AmbaKAL_TaskSleep(MQ_RECEIVE_SLEEP_TIME);
            continue; // Try again
        }
        // Take semaphore
        // Get remaining time
        AmpMsgQueue_GetTimeLeft_Internal(Timeout, TimeLimit, &TimeLeft);
        // Set timeout
        // NOTE: Use a limited waiting time to avoid dead lock.
        SemTimeout = MIN(TimeLeft, MQ_RECEIVE_TAKE_SEM_TIMEOUT);
        Rval = AmbaKAL_SemTake(&MsgQueueHandler->MsgQueueReceiveSem, SemTimeout);
        if (Rval != 0) {
            // Give mutex
            Rval = AmbaKAL_MutexGive(&MsgQueueHandler->ReadMutex);
            if (Rval != 0) {
                AmbaPrintColor(RED, "%s:%u Failed to give mutex!(%d)", __FUNCTION__, __LINE__, Rval);
                return AMP_MQ_MUTEX_GIVE_ERROR; // Failed to give mutex. Unexpected error.
            }
            // Get remaining time
            AmpMsgQueue_GetTimeLeft_Internal(Timeout, TimeLimit, &TimeLeft);
            // Check timeout
            if (TimeLeft == 0) {
                AmbaPrint("%s:%u Receive message timeout (over %u ms).", __FUNCTION__, __LINE__, Timeout);
                return AMP_MQ_TIMEOUT_ERROR; // Time out
            }
            // Sleep for a while to avoid busy waiting.
            AmbaKAL_TaskSleep(MIN(MQ_RECEIVE_SLEEP_TIME, TimeLeft));
            continue; // Try again
        }
        // Receive message
        memcpy(OutputMsgDest, MsgQueueHandler->ReadPointer, MsgQueueHandler->MsgSize);
#ifdef AMP_MESSAGE_QUEUE_DEBUG
        {
            UINT32* MsgAddr = OutputMsgDest; // Start address of the message
            AmbaPrintColor(CYAN, "[MW - Message Queue] <Dequeue> Copy msg of size %d Bytes from 0x%08x to 0x%08x. Msg = [0x%08x, 0x%08x, ...]",
                    MsgQueueHandler->MsgSize,
                    MsgQueueHandler->ReadPointer,
                    OutputMsgDest,
                    MsgAddr[0],
                    MsgAddr[1]);
        }
#endif // AMP_MESSAGE_QUEUE_DEBUG
        // Move read pointer to the next location
        Rval = AmpMsgQueue_MoveReadPointer_Internal(MsgQueueHandler);
        if (Rval != 0) {
            AmbaPrintColor(RED, "%s:%u Failed to move read pointer!(%d)", __FUNCTION__, __LINE__, Rval);
            return AMP_MQ_RP_ERROR; // Failed to move read pointer. Unexpected error.
        }
        // Give semaphore
        Rval = AmbaKAL_SemGive(&MsgQueueHandler->MsgQueueSendSem);
        if (Rval != 0) {
            AmbaPrintColor(RED, "%s:%u Failed to give semaphore!(%d)", __FUNCTION__, __LINE__, Rval);
            return AMP_MQ_SEM_GIVE_ERROR; // Failed to give semaphore. Unexpected error.
        }
        // Give mutex
        Rval = AmbaKAL_MutexGive(&MsgQueueHandler->ReadMutex);
        if (Rval != 0) {
            AmbaPrintColor(RED, "%s:%u Failed to give mutex!(%d)", __FUNCTION__, __LINE__, Rval);
            return AMP_MQ_MUTEX_GIVE_ERROR; // Failed to give mutex. Unexpected error.
        }
        return AMP_MQ_SUCCESS; // Success
    }
}

int AmpMsgQueue_Send(AMP_MSG_QUEUE_HDLR_s *MsgQueueHandler, const void *MsgSource, const UINT32 Timeout)
{
    int Rval = 0; // Function call return.
    const UINT32 TimeLimit = (AmbaSysTimer_GetTickCount() + Timeout >= Timeout) ? // In case of overflow
                             (AmbaSysTimer_GetTickCount() + Timeout) :
                             (AMBA_KAL_WAIT_FOREVER); // The limit of time (in ms) for running this function
    //const UINT32 StartTime = AmbaSysTimer_GetTickCount(); // Start time (in ms)
    //UINT32 ExecTime = 0; // Execution time (in ms)

// Use the limited waiting time to avoid dead lock. Consider this situation (it's a dead lock):
// 1. Thread A (Send message): Waiting for WriteMutex.
// 2. Thread B (Receive message): Took ReadMutex. Waiting for MsgQueueReceiveSem (Queue is empty).
// 3. Thread C (Flush): Took WriteMutex. Waiting for ReadMutex.
// Changing the order of taking WriteMutex and ReadMutex in Flush function won't help.
// If we take ReadMutex first in the flush function, a dead lock could happen when the queue is full.
// However, if there's a limited time of taking semaphore, the dead lock problem can be solved.
#define MQ_SEND_TAKE_SEM_TIMEOUT (2) ///< The limited waiting time of taking semaphore while sending a message.
// Sleep for a while to avoid busy waiting. But not too long. Recall that a frame must be processed
// within 17 ms while decoding videos.
#define MQ_SEND_SLEEP_TIME (2) ///< Sleep for a while to avoid busy waiting.

    // Preliminary check
    if (MsgQueueHandler == NULL) {
        return AMP_MQ_INVALID_ERROR; // Invalid pointer of the message queue handler.
    }
    if (AmpMsgQueue_IsCreated(MsgQueueHandler) == 0) {
        return AMP_MQ_NOT_CREATED_ERROR; // The message queue handler is not created.
    }
    if (MsgSource == NULL) {
        return AMP_MQ_MSG_SRC_ERROR; // Invalid source pointer for message.
    }

    while(1) {
        UINT32 TimeLeft = 0; // Time remained (in ms)
        UINT32 MutexTimeout = 0; // Maximum waiting time (in ms) for a mutex
        UINT32 SemTimeout = 0; // Maximum waiting time (in ms) for a semaphore
        // Get remaining time
        AmpMsgQueue_GetTimeLeft_Internal(Timeout, TimeLimit, &TimeLeft);
        // Set timeout
        MutexTimeout = TimeLeft;
        // Take mutex
        Rval = AmbaKAL_MutexTake(&MsgQueueHandler->WriteMutex, MutexTimeout);
        // Check result
        if (Rval == TX_NOT_AVAILABLE) { // TODO: Need DSP definition of TX_NOT_AVAILABLE
            AmbaPrint("%s:%u Take mutex timeout (over %u ms).", __FUNCTION__, __LINE__, MutexTimeout);
            return AMP_MQ_TIMEOUT_ERROR; // Time out
        } else if (Rval == TX_DELETED) { // TODO: Need DSP definition of TX_DELETED
            AmbaPrint("%s:%u Stop sending message since the queue has been deleted (%d).", __FUNCTION__, __LINE__, Rval);
            return AMP_MQ_DELETED; // The queue has been deleted.
        } else if (Rval != 0) {
            // Sleep for a while to avoid busy waiting.
            AmbaKAL_TaskSleep(MQ_SEND_SLEEP_TIME);
            continue; // Try again
        }
        // Take semaphore
        // Get remaining time
        AmpMsgQueue_GetTimeLeft_Internal(Timeout, TimeLimit, &TimeLeft);
        // Set timeout
        // NOTE: Use a limited waiting time to avoid dead lock.
        SemTimeout = MIN(TimeLeft, MQ_SEND_TAKE_SEM_TIMEOUT);
        Rval = AmbaKAL_SemTake(&MsgQueueHandler->MsgQueueSendSem, SemTimeout);
        if (Rval != 0) {
            // Give mutex
            Rval = AmbaKAL_MutexGive(&MsgQueueHandler->WriteMutex);
            if (Rval != 0) {
                AmbaPrintColor(RED, "%s:%u Failed to give mutex!(%d)", __FUNCTION__, __LINE__, Rval);
                return AMP_MQ_MUTEX_GIVE_ERROR; // Failed to give mutex. Unexpected error.
            }
            // Get remaining time
            AmpMsgQueue_GetTimeLeft_Internal(Timeout, TimeLimit, &TimeLeft);
            // Check timeout
            if (TimeLeft == 0) {
                AmbaPrint("%s:%u Send message timeout (over %u ms).", __FUNCTION__, __LINE__, Timeout);
                return AMP_MQ_TIMEOUT_ERROR; // Time out
            }
            // Sleep for a while to avoid busy waiting.
            AmbaKAL_TaskSleep(MIN(MQ_SEND_SLEEP_TIME, TimeLeft));
            continue; // Try again
        }
        // Send message
        memcpy(MsgQueueHandler->WritePointer, MsgSource, MsgQueueHandler->MsgSize);
#ifdef AMP_MESSAGE_QUEUE_DEBUG
        {
            UINT32* MsgAddr = MsgQueueHandler->WritePointer; // Start address of the message
            AmbaPrintColor(GREEN, "[MW - Message Queue] <Enqueue> Copy msg of size %d Bytes from 0x%08x to 0x%08x. Msg = [0x%08x, 0x%08x, ...]",
                    MsgQueueHandler->MsgSize,
                    MsgSource,
                    MsgQueueHandler->WritePointer,
                    MsgAddr[0],
                    MsgAddr[1]);
        }
#endif // AMP_MESSAGE_QUEUE_DEBUG
        // Move write pointer to the next location
        Rval = AmpMsgQueue_MoveWritePointer_Internal(MsgQueueHandler);
        if (Rval != 0) {
            AmbaPrintColor(RED, "%s:%u Failed to move write pointer!(%d)", __FUNCTION__, __LINE__, Rval);
            return AMP_MQ_WP_ERROR; // Failed to move write pointer. Unexpected error.
        }
        // Give semaphore
        Rval = AmbaKAL_SemGive(&MsgQueueHandler->MsgQueueReceiveSem);
        if (Rval != 0) {
            AmbaPrintColor(RED, "%s:%u Failed to give semaphore!(%d)", __FUNCTION__, __LINE__, Rval);
            return AMP_MQ_SEM_GIVE_ERROR; // Failed to give semaphore. Unexpected error.
        }
        // Give mutex
        Rval = AmbaKAL_MutexGive(&MsgQueueHandler->WriteMutex);
        if (Rval != 0) {
            AmbaPrintColor(RED, "%s:%u Failed to give mutex!(%d)", __FUNCTION__, __LINE__, Rval);
            return AMP_MQ_MUTEX_GIVE_ERROR; // Failed to give mutex. Unexpected error.
        }
        return AMP_MQ_SUCCESS; // Success
    }
}

int AmpMsgQueue_Query(AMP_MSG_QUEUE_HDLR_s *MsgQueueHandler, UINT32 *OutputEnqueuedCount, UINT32 *OutputAvailableCount)
{
    int Rval = 0; // Function call return.
    UINT32 SendSemValue = 0; // Current value of MsgQueueSendSem

    // Preliminary check
    if (MsgQueueHandler == NULL) {
        return AMP_MQ_INVALID_ERROR; // Invalid pointer of the message queue handler.
    }
    if (AmpMsgQueue_IsCreated(MsgQueueHandler) == 0) {
        return AMP_MQ_NOT_CREATED_ERROR; // The message queue handler is not created.
    }
    if (OutputEnqueuedCount == NULL) {
        return AMP_MQ_OUTPUT_EC_ERROR; // Invalid source pointer for OutputEnqueuedCount.
    }
    if (OutputAvailableCount == NULL) {
        return AMP_MQ_OUTPUT_AC_ERROR; // Invalid source pointer for OutputAvailableCount.
    }

    // Set default output values
    *OutputEnqueuedCount = *OutputAvailableCount = 0;

    Rval = AmbaKAL_SemQuery(&MsgQueueHandler->MsgQueueSendSem, &SendSemValue);
    if (Rval != 0) {
        AmbaPrintColor(RED, "%s:%u Failed to query semaphore!(%d)", __FUNCTION__, __LINE__, Rval);
        return AMP_MQ_SEM_QUERY_ERROR; // Failed to query semaphore. This is an unexpected error.
    }
    *OutputAvailableCount = SendSemValue;

    //AmbaKAL_SemQuery(&MsgQueueHandler->MsgQueueReceiveSem, &ReceiveSemValue); Don't do this!!
    // The sum of ReceiveSemValue and SendSemValue may equal to (MaxNumMsg - 1) or (MaxNumMsg - 2)
    // while sending message, receiving message, or both.
    // Return (MaxNumMsg - SendSemValue) to get currect value.
    *OutputEnqueuedCount = MsgQueueHandler->MaxNumMsg - SendSemValue;
    return AMP_MQ_SUCCESS; // Success
}

int AmpMsgQueue_IsCreated(const AMP_MSG_QUEUE_HDLR_s *MsgQueueHandler)
{
    return (MsgQueueHandler == NULL) ? (0) : (MsgQueueHandler->IsCreated); // Success
}
