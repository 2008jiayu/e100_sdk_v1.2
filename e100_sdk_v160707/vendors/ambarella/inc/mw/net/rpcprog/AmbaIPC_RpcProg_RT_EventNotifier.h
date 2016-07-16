/**
 * @file inc/mw/net/rpcprog/AmbaIPC_RpcProg_RT_EventNotifier.h
 *
 * Header file for EventNotifier RPC Services (RTOS side)
 *
 * Copyright (C) 2014, Ambarella, Inc.
 *
 * All rights reserved. No Part of this file may be reproduced, stored
 * in a retrieval system, or transmitted, in any form, or by any means,
 * electronic, mechanical, photocopying, recording, or otherwise,
 * without the prior consent of Ambarella, Inc.
 */
#ifndef _RPC_PROG_RT_EVENTNOTIFIER_H_
#define _RPC_PROG_RT_EVENTNOTIFIER_H_

#include "AmbaIPC_Rpc_Def.h"

//RPC_INFO definition
#define RT_EVENTNOTIFIER_PROG_ID	0x10000009
#define RT_EVENTNOTIFIER_HOST AMBA_IPC_HOST_THREADX
#define RT_EVENTNOTIFIER_VER (1)
#define RT_EVENTNOTIFIER_DEFULT_TIMEOUT (0) //AMBA_IPC_ASYNCHRONOUS
#define RT_EVENTNOTIFIER_NAME "LINK_RPC_SVC_EVENTNOTIFIER"

#define RT_EVENTNOTIFIER_MAX_MSGBLK_PARAM_LEN (512)

typedef struct _RT_EVENTNOTIFIER_MSGBLK_s_ {
    unsigned int msg;
    unsigned int param_len;
    unsigned char param[RT_EVENTNOTIFIER_MAX_MSGBLK_PARAM_LEN];
} RT_EVENTNOTIFIER_MSGBLK_s;

//============ RPC_FUNC definition ============
enum _RT_EVENTNOTIFIER_FUNC_e_ {
    RT_EVENTNOTIFIER_FUNC_NOTIFY = 1,

    RT_EVENTNOTIFIER_FUNC_AMOUNT
};


/**
 * [[AMBA_IPC_ASYNCHRONOUS]]
 * [in] RT_EVENTNOTIFIER_MSGBLK_s
 * [out] int
 */
AMBA_IPC_REPLY_STATUS_e RT_EVENTNOTIFIER_Notify_Clnt(RT_EVENTNOTIFIER_MSGBLK_s *pArg, int *pResult, int Clnt );
void RT_EVENTNOTIFIER_Notify_Svc(RT_EVENTNOTIFIER_MSGBLK_s *pArg, AMBA_IPC_SVC_RESULT_s *pRet);


#endif /* _RPC_PROG_RT_EVENTNOTIFIER_H_ */
