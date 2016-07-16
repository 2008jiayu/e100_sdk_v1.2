/**
 * @file inc/mw/net/rpcprog/LU_NetFifo.h
 *
 * Header file for NetFifo RPC Services (RTOS side)
 *
 * Copyright (C) 2014, Ambarella, Inc.
 *
 * All rights reserved. No Part of this file may be reproduced, stored
 * in a retrieval system, or transmitted, in any form, or by any means,
 * electronic, mechanical, photocopying, recording, or otherwise,
 * without the prior consent of Ambarella, Inc.
 */
#ifndef _RPC_PROG_LU_NETFIFO_H_
#define _RPC_PROG_LU_NETFIFO_H_

#include "AmbaIPC_Rpc_Def.h"

#ifndef FOREVER
#define FOREVER 0xFFFFFFFF
#endif

//RPC_INFO definition
#define LU_NETFIFO_PROG_ID	0x20000004
#define LU_NETFIFO_HOST AMBA_IPC_HOST_LINUX
#define LU_NETFIFO_VER (1)
#define LU_NETFIFO_DEFULT_TIMEOUT (FOREVER)
#define LU_NETFIFO_NAME "AMBANETFIFO_LUSVC"


typedef enum _LU_NETFIFO_CONTROL_CMD_e_ {
    LU_NETFIFO_CMD_STARTENC = 1, /**< Start encode. May from STOP_ENC, SWITCHENCSESSION or STARTNETPLAY */
    LU_NETFIFO_CMD_STOPENC, /**< Stop encode then stay in idle. Such as menu operation or switch to thumbnail mode */
    LU_NETFIFO_CMD_SWITCHENCSESSION, /**< Stop encode then start another encode session (VF <-> REC) */
    LU_NETFIFO_CMD_STARTNETPLAY, /**< Start playback for streaming. set param1 as stream_id */
    LU_NETFIFO_CMD_STOPNETPLAY, /**< Stop playback for streaming. set param1 as STARTENC to indecate APP is recording */
    LU_NETFIFO_CMD_RELEASE /**< RTOS NetFifo mudule released */
} LU_NETFIFO_CONTROL_CMD_e;

typedef struct _LU_NETFIFO_CONTROL_ARG_s_ {
    unsigned int Cmd;
    unsigned int Param1;
    unsigned int Param2;
} LU_NETFIFO_CONTROL_ARG_s;

typedef struct _LU_NETFIFO_FIFOCALLBACK_ARG_s_ {
    unsigned int FifoHndlr;
    unsigned int Event;
    unsigned int InfoPtr;
} LU_NETFIFO_FIFOCALLBACK_ARG_s;

//============ RPC_FUNC definition ============
enum _LU_NETFIFO_FUNC_e_ {
	LU_NETFIFO_FUNC_CONTROLEVENT = 1,
	LU_NETFIFO_FUNC_FIFOEVENT,
	LU_NETFIFO_FUNC_FIFOGETWRITEPOINT,

	LU_NETFIFO_FUNC_AMOUNT
};

/**
 * [in] LU_NETFIFO_CONTROL_ARG_s
 * [out] int
 */
AMBA_IPC_REPLY_STATUS_e LU_NetFifo_ControlEvent_Clnt(LU_NETFIFO_CONTROL_ARG_s *pArg, int *pResult, int Clnt );
void LU_NetFifo_ControlEvent_Svc(LU_NETFIFO_CONTROL_ARG_s *pArg, AMBA_IPC_SVC_RESULT_s *pRet);

/**
 * [in] LU_NETFIFO_FIFOCALLBACK_ARG_s
 * [out] int
 */
AMBA_IPC_REPLY_STATUS_e LU_NetFifo_FifoEvent_Clnt(LU_NETFIFO_FIFOCALLBACK_ARG_s *pArg, int *pResult, int Clnt );
void LU_NetFifo_FifoEvent_Svc(LU_NETFIFO_FIFOCALLBACK_ARG_s *pArg, AMBA_IPC_SVC_RESULT_s *pRet);

/**
 * [in] LU_NETFIFO_FIFOCALLBACK_ARG_s
 * [out] int
 */
AMBA_IPC_REPLY_STATUS_e LU_NetFifo_FifoGetWritePoint_Clnt(LU_NETFIFO_FIFOCALLBACK_ARG_s *pArg, int *pResult, int Clnt );
void LU_NetFifo_FifoGetWritePointt_Svc(LU_NETFIFO_FIFOCALLBACK_ARG_s *pArg, AMBA_IPC_SVC_RESULT_s *pRet);


#endif /* _RPC_PROG_LU_NETFIFO_H_ */

