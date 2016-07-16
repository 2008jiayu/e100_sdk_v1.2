/**
 * @file AmbaIPC_RpcProg_Lu_PT.h
 *
 * Header file for Pseudo Terminal RPC Services
 *
 *
 * Copyright (C) 2014, Ambarella, Inc.
 *
 * All rights reserved. No Part of this file may be reproduced, stored
 * in a retrieval system, or transmitted, in any form, or by any means,
 * electronic, mechanical, photocopying, recording, or otherwise,
 * without the prior consent of Ambarella, Inc.
 */

#ifndef _AMBA_IPC_RPC_PROG_LU_PT_H_
#define _AMBA_IPC_RPC_PROG_LU_PT_H_

#include "AmbaIPC_Rpc_Def.h"

#define AMBA_RPC_PROG_LU_PT_PROG_ID		0x20000008
#define AMBA_RPC_PROG_LU_PT_VER			1

#define AMBA_RPC_PROG_LU_PT            1
AMBA_IPC_REPLY_STATUS_e AmbaRpcProg_LU_PT_Clnt(const char *pStr, int *pResult, int Clnt);
void AmbaRpcProg_LU_PT_Svc(const char *pStr, AMBA_IPC_SVC_RESULT_s *pRet);

#endif
