/**
 * @file inc/mw/net/rpcprog/AmbaIPC_RpcProg_RT_NetCtrl.h
 *
 * Header file for NetCtrl RPC Services
 *
 * Copyright (C) 2014, Ambarella, Inc.
 *
 * All rights reserved. No Part of this file may be reproduced, stored
 * in a retrieval system, or transmitted, in any form, or by any means,
 * electronic, mechanical, photocopying, recording, or otherwise,
 * without the prior consent of Ambarella, Inc.
 */

#ifndef _RPC_PROG_RT_NETCTRL_H_
#define _RPC_PROG_RT_NETCTRL_H_

#include "AmbaIPC_Rpc_Def.h"

#define RT_NETCTRL_PROG_ID		0x10000005
#define	RT_NETCTRL_VER			1

/* Procedure ID */
#define RT_NETCTRL_SEND_SVC        1

/**
 * network control command with json string
 */
typedef struct _RT_NETCTRL_CMD_s_ {
   char  Param[1024];    /**< json string of cmd */
   unsigned int ParamSize;      /**< size of json string to cmd  */
   unsigned int ClientId;            /**< socket descriptor of accepted client */
} RT_NETCTRL_CMD_s;

/**
 * [in] RT_NETCTRL_CMD_s
 * [out] int
 */
int RT_NetCtrl_Send_Svc(RT_NETCTRL_CMD_s *pArg, AMBA_IPC_SVC_RESULT_s *pRet);
AMBA_IPC_REPLY_STATUS_e RT_NetCtrl_Send_Clnt(RT_NETCTRL_CMD_s *pArg, int *pResult, int Clnt);

#endif /* _RPC_PROG_RT_NETCTRL_H_ */

