/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaIPC_Rpc_Def.h
 *
 *  @Copyright      :: Copyright (C) 2013 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Common definitions for AmbaIPC RPC program.
 *
\*-------------------------------------------------------------------------------------------------------------------*/
#ifndef __AMBAIPC_RPC_DEF_H__
#define __AMBAIPC_RPC_DEF_H__


/**
 * RPC server should be either in Linux or ThreadX.
 * @see AmbaIPC_ClientCreate
 */
typedef enum _AMBA_IPC_HOST_e_ {
	AMBA_IPC_HOST_LINUX = 0,
	AMBA_IPC_HOST_THREADX,
	AMBA_IPC_HOST_MAX
} AMBA_IPC_HOST_e;

/**
 * Definition of the status for RPC.
 *
 */
typedef enum _AMBA_IPC_REPLY_STATUS_e_ {
	AMBA_IPC_REPLY_SUCCESS = 0,
	AMBA_IPC_REPLY_PROG_UNAVAIL,
	AMBA_IPC_REPLY_PARA_INVALID,
	AMBA_IPC_REPLY_SYSTEM_ERROR,
	AMBA_IPC_REPLY_TIMEOUT,
	AMBA_IPC_INVALID_CLIENT_ID,
	AMBA_IPC_UNREGISTERED_SERVER,
	AMBA_IPC_REREGISTERED_SERVER,
	AMBA_IPC_SERVER_MEM_ALLOC_FAILED,
	AMBA_IPC_IS_NOT_READY,
	AMBA_IPC_CRC_ERROR,
	AMBA_IPC_NETLINK_ERROR,
	AMBA_IPC_STATUS_NUM,
	AMBA_IPC_REPLY_MAX = 0xFFFFFFFF
} AMBA_IPC_REPLY_STATUS_e;

/**
 * The procedure calls to binder.
 *
 */
typedef enum _AMBA_IPC_BINDER_e_ {
	AMBA_IPC_BINDER_BIND = 0,
	AMBA_IPC_BINDER_REGISTER,
	AMBA_IPC_BINDER_UNREGISTER,
	AMBA_IPC_BINDER_LIST
} AMBA_IPC_BINDER_e;

/**
 * The message type.
 *
 */
typedef enum _AMBA_IPC_MSG_e_ {
	AMBA_IPC_MSG_CALL = 0,
	AMBA_IPC_MSG_REPLY
} AMBA_IPC_MSG_e;

/**
 * The communication mode for RPC procedure.
 * @see AMBA_IPC_SVC_RESULT_s
 *
 */
typedef enum _AMBA_IPC_COMMUICATION_MODE_e_ {
	AMBA_IPC_SYNCHRONOUS = 0,
	AMBA_IPC_ASYNCHRONOUS,
	AMBA_IPC_MODE_MAX = 0xFFFFFFFF
} AMBA_IPC_COMMUICATION_MODE_e ;

/**
 * The data structure for storing the server result.
 * @see AMBA_IPC_COMMUICATION_MODE_e
 * @see AMBA_IPC_PROC_f
 */
typedef struct _AMBA_IPC_SVC_RESULT_s_ {
	int Length;							/**< The size of the result */
	void *pResult;						/**< Pointer to the calculated result */
	AMBA_IPC_COMMUICATION_MODE_e Mode;	/**< The communication mode of the procedure */
	AMBA_IPC_REPLY_STATUS_e Status;		/**< The status of the procedure */
} AMBA_IPC_SVC_RESULT_s;

/**
 * The function pointer prototype for RPC procedure.
 * All the RPC procedures need to follow this type.
 * @see AMBA_IPC_SVC_RESULT_s
 */
typedef void (*AMBA_IPC_PROC_f)(void *, AMBA_IPC_SVC_RESULT_s *);

/**
 * The procedure infomation for RPC program.
 *
 * @see AmbaIPC_SvcRegister
 * @see AMBA_IPC_PROC_f
 * @see AMBA_IPC_COMMUICATION_MODE_e
 */
typedef struct _AMBA_IPC_PROC_s_ {
    AMBA_IPC_PROC_f Proc;
    AMBA_IPC_COMMUICATION_MODE_e Mode;
} AMBA_IPC_PROC_s;


/**
 * The information for RPC program information such as the procedure information.
 *
 * @see AMBA_IPC_PROC_s
 */
typedef struct _AMBA_IPC_PROG_INFO_s_ {
	int ProcNum;					/**< The procedure number */
	AMBA_IPC_PROC_s *pProcInfo;		/**< The information of the procedures in the RPC program */
} AMBA_IPC_PROG_INFO_s;

#endif
