/**
 * @file inc/mw/net/rpcprog/AmbaIPC_RpcProg_LU_NetCtrl.h
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

#ifndef _RPC_PROG_LU_NETCTRL_H_
#define _RPC_PROG_LU_NETCTRL_H_

#include "AmbaIPC_Rpc_Def.h"

#define LU_NETCTRL_PROG_ID		0x20000005
#define	LU_NETCTRL_VER			1

/* Procedure ID */
#define LU_NETCTRL_REPLY_SVC        1

/**
 * response of network control command
 */
typedef struct _LU_NETCTRL_DATA_s_ {
   char *Param;                       /**< json string context */
   unsigned int ParamSize;      /**< size of json string */
   unsigned int ClientId;            /**< socket descriptor of accepted client */
} LU_NETCTRL_DATA_s;

/**
 * [in] LU_NETCTRL_DATA_s
 * [out] int
 */
int LU_NetCtrl_Reply_Svc(LU_NETCTRL_DATA_s *pArg, AMBA_IPC_SVC_RESULT_s *pRet);
AMBA_IPC_REPLY_STATUS_e LU_NetCtrl_Reply_Clnt(LU_NETCTRL_DATA_s *pArg, int *pResult, int nClnt);

#endif /* _RPC_PROG_LU_NETCTRL_H_ */

