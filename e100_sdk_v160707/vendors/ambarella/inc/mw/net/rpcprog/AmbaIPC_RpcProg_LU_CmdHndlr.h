/**
 * @file inc/mw/net/rpcprog/AmbaIPC_RpcProg_Lu_CmdHndlr.h
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

#ifndef _RPC_PROG_LU_CMD_HNDLR_H_
#define _RPC_PROG_LU_CMD_HNDLR_H_

#include "AmbaIPC_Rpc_Def.h"

#define LU_CMD_HNDLR_PROG_ID		0x20000006
#define	LU_CMD_HNDLR_VER			1

/* Procedure ID */
#define LU_CMD_HNDLR_PROC           1

/* Supported Linux service */
#define LU_CMD_LS                   0
#define LU_CMD_CD                   1
#define LU_CMD_PWD                  2
#define LU_CMD_GET_WIFI_STAT        3
#define LU_CMD_GET_WIFI_CFG         4
#define LU_CMD_SET_WIFI_CFG         5
#define LU_CMD_START_WIFI           6
#define LU_CMD_STOP_WIFI            7
#define LU_CMD_RESTART_WIFI         8

/**
 * linux command operation
 */
typedef struct _LU_LNXCMDHNDLR_CMD_s_ {
    unsigned int LuCmd;         /**< linux command id */
    unsigned int CmdSize;       /**< the string length of the command */
    unsigned int OutSize;       /**< the maximum size of the output */
    void* Result;                    /**< the result of execute linux command */
    char Cmd[1024];               /**< command string */
} LU_LNXCMDHNDLR_CMD_s;

/**
 * result of linux command operation
 */
typedef struct _LU_LNXCMDHNDLR_DATA_s_ {
    int Rval;                                /**< result of execute linux command */
    unsigned int ResultSize;       /**< size of the result */
} LU_LNXCMDHNDLR_DATA_s;

/**
 * [in] LU_LNXCMDHNDLR_CMD_s
 * [out] LU_LNXCMDHNDLR_DATA_s
 */
int LU_LnxCmdHndlr_Svc(LU_LNXCMDHNDLR_CMD_s *pArg, AMBA_IPC_SVC_RESULT_s *pResult);
AMBA_IPC_REPLY_STATUS_e LU_LnxCmdHndlr_Clnt(LU_LNXCMDHNDLR_CMD_s *pArg, LU_LNXCMDHNDLR_DATA_s *pResult, int Clnt);

#endif /* RPC_PROG_LU_CMD_HNDLR_H */

