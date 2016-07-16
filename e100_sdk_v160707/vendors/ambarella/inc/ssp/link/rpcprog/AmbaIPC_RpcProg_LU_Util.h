/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaIPC_Prog_Util.h
 *
 *  @Copyright      :: Copyright (C) 2013 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Definitions for AmbaIPC RPC utility program.
 *
 *
\*-------------------------------------------------------------------------------------------------------------------*/
#ifndef _AMBA_IPC_RPC_PROG_LU_UTIL_H_
#define _AMBA_IPC_RPC_PROG_LU_UTIL_H_

#include "AmbaIPC_Rpc_Def.h"

#define AMBA_RPC_PROG_LU_UTIL                  1	/* This is ModuleName */

/* user needs to define the program id*/
#define AMBA_RPC_PROG_LU_UTIL_PROG_ID		0x20000001 /* Please follw this naming rule: ModuleName_PROG_ID */
#define AMBA_RPC_PROG_LU_UTIL_VER			1
#define AMBA_RPC_PROG_LU_UTIL_HOST			AMBA_IPC_HOST_LINUX
#define EXEC_OUTPUT_SIZE		2048	/* The limit size for execution output */

typedef struct _AMBA_RPC_PROG_EXEC_ARG_s_ {
	char command[1];
} AMBA_RPC_PROG_EXEC_ARG_s;

/* procedure id*/
#define AMBA_RPC_PROG_LU_UTIL_EXEC1		1

AMBA_IPC_REPLY_STATUS_e AmbaRpcProg_Util_Exec1_Clnt(AMBA_RPC_PROG_EXEC_ARG_s *pArg, int *pResult, int Clnt );
void AmbaIpcProg_Util_Exec1_Svc(AMBA_RPC_PROG_EXEC_ARG_s *pArg, AMBA_IPC_SVC_RESULT_s *pRet);

/* procedure id*/
#define AMBA_RPC_PROG_LU_UTIL_EXEC2		2

AMBA_IPC_REPLY_STATUS_e AmbaRpcProg_Util_Exec2_Clnt(AMBA_RPC_PROG_EXEC_ARG_s *pArg, int *pResult, int Clnt );
void AmbaRpcProg_Util_Exec2_Svc(AMBA_RPC_PROG_EXEC_ARG_s *pArg, AMBA_IPC_SVC_RESULT_s *pRet);

#endif