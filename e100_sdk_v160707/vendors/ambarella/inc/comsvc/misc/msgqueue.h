/**
 * @file inc/mw/utils/MsgQueue.h
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

#ifndef _MSG_QUEUE_H_
#define _MSG_QUEUE_H_

#include "AmbaKAL.h"

#define MSG_QUEUE_SIZE_MAX (1 << 20)    ///< The maximum size of the queue buffer (1MB)
#define MSG_QUEUE_SIZE_MIN (1)          ///< The minimum size of the queue buffer (1 Byte)

/**
 * The execution result of message queue handler functions
 */
typedef enum _AMP_MSG_QUEUE_RESULT_e_ {
    AMP_MQ_SUCCESS = 0,         ///< Success.
    AMP_MQ_INVALID_ERROR,       ///< Failure. Invalid pointer of the message queue handler.
    AMP_MQ_CREATED_ERROR,       ///< Failure. The message queue handler is ALREADY created.
    AMP_MQ_NOT_CREATED_ERROR,   ///< Failure. The message queue handler is NOT created.
    AMP_MQ_MUTEX_CREATE_ERROR,  ///< Failure. Failed to create mutex.
    AMP_MQ_MUTEX_DELETE_ERROR,  ///< Failure. Failed to delete mutex.
    AMP_MQ_MUTEX_TAKE_ERROR,    ///< Failure. Failed to take mutex.
    AMP_MQ_MUTEX_GIVE_ERROR,    ///< Failure. Failed to give mutex.
    AMP_MQ_SEM_CREATE_ERROR,    ///< Failure. Failed to create semaphore.
    AMP_MQ_SEM_DELETE_ERROR,    ///< Failure. Failed to delete semaphore.
    AMP_MQ_SEM_GIVE_ERROR,      ///< Failure. Failed to give semaphore.
    AMP_MQ_SEM_QUERY_ERROR,     ///< Failure. Failed to query semaphore.
    AMP_MQ_TIMEOUT_ERROR,       ///< Failure. Unable to retrieve a message for the duration of the specified time to wait.
    AMP_MQ_BUF_ADDR_ERROR,      ///< Failure. Invalid starting address of the message buffer.
    AMP_MQ_BUF_SIZE_ERROR,      ///< Failure. Invalid size (MsgSize * MaxNumMsg) of the message buffer.
    AMP_MQ_MSG_DEST_ERROR,      ///< Failure. Invalid destination pointer for message.
    AMP_MQ_MSG_SRC_ERROR,       ///< Failure. Invalid source pointer for message.
    AMP_MQ_OUTPUT_EC_ERROR,     ///< Failure. Invalid source pointer for OutputEnqueuedCount.
    AMP_MQ_OUTPUT_AC_ERROR,     ///< Failure. Invalid source pointer for OutputAvailableCount.
    AMP_MQ_RP_ERROR,            ///< Failure. Failed to move read pointer.
    AMP_MQ_WP_ERROR,            ///< Failure. Failed to move write pointer.
    AMP_MQ_DELETED              ///< Failure. The queue has been deleted while the thread was suspended.
} AMP_MSG_QUEUE_RESULT_e;

/**
 * Message queue handler.
 */
typedef struct _AMP_MSG_QUEUE_HDLR_s_ {
    /**
     * Whether the message queue is created.    \n
     * 0 - Not created, 1 - Created
     */
    UINT8 IsCreated;
    /**
     * Starting address of the buffer for messages.
     */
    void *MsgBufBase;
    /**
     * The size of each message in the queue.
     */
    UINT32 MsgSize;
    /**
     * Total number of messages available for the message queue.
     */
    UINT32 MaxNumMsg;
    /**
     * Read pointer. Next location to read message.
     */
    void *ReadPointer;
    /**
     * Write pointer. Next location to write message.
     */
    void *WritePointer;
    /**
     * Mutex of read pointer.
     */
    AMBA_KAL_MUTEX_t ReadMutex;
    /**
     * Mutex of write pointer.
     */
    AMBA_KAL_MUTEX_t WriteMutex;
    /**
     * Counting semaphore for receiving a message with initial value (0).   \n
     * The value indicates the number of messages currently in the queue waiting for being received.
     */
    AMBA_KAL_SEM_t MsgQueueReceiveSem;
    /**
     * Counting semaphore for sending a message with initial value (MaxNumMsg). \n
     * The value indicates the number of messages the queue currently has space for.
     */
    AMBA_KAL_SEM_t MsgQueueSendSem;
} AMP_MSG_QUEUE_HDLR_s;

/**
 * Create a message queue handler.
 *
 * @param [in] MsgQueueHandler      Message queue handler.
 * @param [in] MsgBufBase           Starting address of the buffer for messages.
 * @param [in] MsgSize              Size (in byte) of each message.
 * @param [in] MaxNumMsg            Maximum number of messages in a message queue.
 *
 * @return
 *      AMP_MQ_SUCCESS              - Success                                                               \n
 *      AMP_MQ_INVALID_ERROR        - Failure. Invalid pointer of the message queue handler.                \n
 *      AMP_MQ_CREATED_ERROR        - Failure. The message queue handler is already created.                \n
 *      AMP_MQ_BUF_ADDR_ERROR       - Failure. Invalid starting address of the message buffer.              \n
 *      AMP_MQ_BUF_SIZE_ERROR       - Failure. Invalid size (MsgSize * MaxNumMsg) of the message buffer.    \n
 *      AMP_MQ_MUTEX_CREATE_ERROR   - Failure. Failed to create mutex.                                      \n
 *      AMP_MQ_SEM_CREATE_ERROR     - Failure. Failed to create semaphore.
 */
extern int AmpMsgQueue_Create(AMP_MSG_QUEUE_HDLR_s *MsgQueueHandler, void *MsgBufBase, const UINT32 MsgSize, const UINT32 MaxNumMsg);

/**
 * Delete a message queue handler.  \n
 * All threads suspended receiving a message from this queue are given a (AMP_MQ_DELETED) return value.
 *
 * @param [in] MsgQueueHandler      Message queue handler.
 *
 * @return
 *      AMP_MQ_SUCCESS              - Success. Successful deletion of message queue handler.        \n
 *      AMP_MQ_INVALID_ERROR        - Failure. Invalid pointer of the message queue handler.        \n
 *      AMP_MQ_NOT_CREATED_ERROR    - Failure. The message queue handler is not created.            \n
 *      AMP_MQ_SEM_DELETE_ERROR     - Failure. Failed to delete semaphore.                          \n
 *      AMP_MQ_MUTEX_DELETE_ERROR   - Failure. Failed to delete mutex.
 */
extern int AmpMsgQueue_Delete(AMP_MSG_QUEUE_HDLR_s *MsgQueueHandler);

/**
 * Flush message queue.                                         \n
 * Deletes all messages stored in the specified message queue.  \n
 * If the queue is empty, this service does nothing.
 *
 * @param [in] MsgQueueHandler      Message queue handler.
 *
 * @return
 *      AMP_MQ_SUCCESS              - Success                                                       \n
 *      AMP_MQ_INVALID_ERROR        - Failure. Invalid pointer of the message queue handler.        \n
 *      AMP_MQ_NOT_CREATED_ERROR    - Failure. The message queue handler is not created.            \n
 *      AMP_MQ_MUTEX_TAKE_ERROR     - Failure. Failed to take mutex.                                \n
 *      AMP_MQ_MUTEX_GIVE_ERROR     - Failure. Failed to give mutex.                                \n
 *      AMP_MQ_SEM_DELETE_ERROR     - Failure. Failed to delete semaphore.                          \n
 *      AMP_MQ_SEM_CREATE_ERROR     - Failure. Failed to create semaphore.
 */
extern int AmpMsgQueue_Flush(AMP_MSG_QUEUE_HDLR_s *MsgQueueHandler);

/**
 * Receive a message from the specified message queue handler.              \n
 * The retrieved message is copied from the queue into the memory area      \n
 * specified by the destination pointer. That message is then removed from  \n
 * the queue.                                                               \n
 * The specified destination memory area must be large enough to hold the   \n
 * message.
 *
 * @param [in] MsgQueueHandler      Message queue handler.
 * @param [out] OutputMsgDest       Message.
 * @param [in] Timeout              Time out value (in ms).
 *
 * @return
 *      AMP_MQ_SUCCESS              - Success                                                               \n
 *      AMP_MQ_INVALID_ERROR        - Failure. Invalid pointer of the message queue handler.                \n
 *      AMP_MQ_NOT_CREATED_ERROR    - Failure. The message queue handler is not created.                    \n
 *      AMP_MQ_MSG_DEST_ERROR       - Failure. Invalid destination pointer for message.                     \n
 *      AMP_MQ_TIMEOUT_ERROR        - Failure. Unable to retrieve a message for the duration of
 *                                    the specified time to wait.                                           \n
 *      AMP_MQ_MUTEX_GIVE_ERROR     - Failure. Failed to give mutex. This is an unexpected error.           \n
 *      AMP_MQ_SEM_GIVE_ERROR       - Failure. Failed to give semaphore. This is an unexpected error.       \n
 *      AMP_MQ_RP_ERROR             - Failure. Failed to move read pointer. This is an unexpected error.    \n
 *      AMP_MQ_DELETED              - Failure. The queue has been deleted while the thread was suspended.   \n
 */
extern int AmpMsgQueue_Receive(AMP_MSG_QUEUE_HDLR_s *MsgQueueHandler, void *OutputMsgDest, const UINT32 Timeout);

/**
 * Send a message to message queue handler.
 *
 * @param [in] MsgQueueHandler      Message queue handler.
 * @param [out] OutputMsgSource     Message.
 * @param [in] Timeout              Time out (in ms).
 *
 * @return
 *      AMP_MQ_SUCCESS              - Success                                                               \n
 *      AMP_MQ_INVALID_ERROR        - Failure. Invalid pointer of the message queue handler.                \n
 *      AMP_MQ_NOT_CREATED_ERROR    - Failure. The message queue handler is not created.                    \n
 *      AMP_MQ_MSG_SRC_ERROR        - Failure. Invalid source pointer for message.                          \n
 *      AMP_MQ_TIMEOUT_ERROR        - Failure. Unable to retrieve a message for the duration of
 *                                    the specified time to wait.                                           \n
 *      AMP_MQ_MUTEX_GIVE_ERROR     - Failure. Failed to give mutex. This is an unexpected error.           \n
 *      AMP_MQ_SEM_GIVE_ERROR       - Failure. Failed to give semaphore. This is an unexpected error.       \n
 *      AMP_MQ_WP_ERROR             - Failure. Failed to move write pointer. This is an unexpected error.
 */
extern int AmpMsgQueue_Send(AMP_MSG_QUEUE_HDLR_s *MsgQueueHandler, const void *MsgSource, const UINT32 Timeout);

/**
 * Retrieve information about the specified message queue handler.
 *
 * @param [in] MsgQueueHandler          Message queue handler.
 * @param [out] OutputEnqueuedCount     The number of messages currently in the queue.
 * @param [out] OutputAvailableCount    The number of messages the queue currently has space for.
 *
 * @return
 *      AMP_MQ_SUCCESS              - Success                                                               \n
 *      AMP_MQ_INVALID_ERROR        - Failure. Invalid pointer of the message queue handler.                \n
 *      AMP_MQ_NOT_CREATED_ERROR    - Failure. The message queue handler is not created.                    \n
 *      AMP_MQ_OUTPUT_EC_ERROR      - Failure. Invalid source pointer for OutputEnqueuedCount.              \n
 *      AMP_MQ_OUTPUT_AC_ERROR      - Failure. Invalid source pointer for OutputAvailableCount.             \n
 *      AMP_MQ_SEM_QUERY_ERROR      - Failure. Failed to query semaphore. This is an unexpected error.
 */
extern int AmpMsgQueue_Query(AMP_MSG_QUEUE_HDLR_s *MsgQueueHandler, UINT32 *OutputEnqueuedCount, UINT32 *OutputAvailableCount);

/**
 * Whether the message queue is created.
 *
 * @param [in] MsgQueueHandler      Message queue handler.
 *
 * @return 0 - Not created, others - Created
 */
extern int AmpMsgQueue_IsCreated(const AMP_MSG_QUEUE_HDLR_s *MsgQueueHandler);

#endif /* _MSG_QUEUE_H_ */
