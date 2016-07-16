/**
 * @file inc/mw/net/rpcprog/AmbaIPC_RpcProg_RT_DataNotify.h
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

#ifndef _RPC_PROG_RT_DATA_NOTIFY_H_
#define _RPC_PROG_RT_DATA_NOTIFY_H_

#include "AmbaIPC_Rpc_Def.h"

#define RT_DATA_NOTIFY_PROG_ID			0x10000006
#define RT_DATA_NOTIFY_VER				1

/* Procedure ID */
#define RT_DATA_NOTIFY_PROC				1

/**
 * data daemon status
 */
typedef struct _RT_DATASVC_STATUS_s_ {
    short Status;                             /**< send status of data server */
    unsigned long long Bytes;           /**< send/recv data bytes */
    unsigned int Type;                      /**< notification type of data server */
    unsigned char ClientInfo[128];        /**< client identifier */
    char TransportType[16];                /**< transport protocol type */
    unsigned char Md5sum[32];       /**< md5 checksum */
} RT_DATASVC_STATUS_s;

/**
 * [in] RT_DATASVC_STATUS_s
 * [out] int
 */
int RT_DataNotify_Svc(RT_DATASVC_STATUS_s *pStatus, AMBA_IPC_SVC_RESULT_s *pRet);
AMBA_IPC_REPLY_STATUS_e RT_DataNotify_Clnt(RT_DATASVC_STATUS_s *pStatus, int *pResult, int Clnt);

#endif /* RPC_PROG_DATA_NOTIFY_H */

