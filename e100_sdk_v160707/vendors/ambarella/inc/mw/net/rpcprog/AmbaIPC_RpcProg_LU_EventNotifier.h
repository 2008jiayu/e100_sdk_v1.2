/**
 * @file inc/mw/net/rpcprog/AmbaIPC_RpcProg_LU_EventNotifier.h
 *
 * Header file for EventNotifier RPC Services (Linux side)
 *
 * Copyright (C) 2014, Ambarella, Inc.
 *
 * All rights reserved. No Part of this file may be reproduced, stored
 * in a retrieval system, or transmitted, in any form, or by any means,
 * electronic, mechanical, photocopying, recording, or otherwise,
 * without the prior consent of Ambarella, Inc.
 */
#ifndef _RPC_PROG_LU_EVENTNOTIFIER_H_
#define _RPC_PROG_LU_EVENTNOTIFIER_H_

#include "AmbaIPC_Rpc_Def.h"

//RPC_INFO definition
#define LU_EVENTNOTIFIER_PROG_ID	0x20000009
#define LU_EVENTNOTIFIER_HOST AMBA_IPC_HOST_LINUX
#define LU_EVENTNOTIFIER_VER (1)
#define LU_EVENTNOTIFIER_DEFULT_TIMEOUT (0) //AMBA_IPC_ASYNCHRONOUS
#define LU_EVENTNOTIFIER_NAME "AmbaEventNotifier_LUSVC"

typedef struct _LU_EVENTNOTIFIER_MSGBLK_s_ {
    unsigned int msg;
    unsigned int param_len;
    unsigned char param[512];
} LU_EVENTNOTIFIER_MSGBLK_s;

//============ RPC_FUNC definition ============
enum _LU_EVENTNOTIFIER_FUNC_e_ {
	LU_EVENTNOTIFIER_FUNC_NOTIFY = 1,

	LU_EVENTNOTIFIER_FUNC_AMOUNT
};

/**
 * [[AMBA_IPC_ASYNCHRONOUS]]
 * [in] LU_EVENTNOTIFIER_MSGBLK_s
 * [out] int
 */
AMBA_IPC_REPLY_STATUS_e LU_EVENTNOTIFIER_Notify_Clnt(LU_EVENTNOTIFIER_MSGBLK_s *pArg, int *pResult, int Clnt );
void LU_EVENTNOTIFIER_Notify_Svc(LU_EVENTNOTIFIER_MSGBLK_s *pArg, AMBA_IPC_SVC_RESULT_s *pRet);


#endif /* _RPC_PROG_LU_EVENTNOTIFIER_H_ */

