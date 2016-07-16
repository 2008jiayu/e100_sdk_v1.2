/**
 *  @FileName       :: AmbaIPC_RpcProg_LU_Msg.h
 *
 *  @Description    :: Definitions for AmbaIPC Message Framework.
 *
 *  @History        ::
 *      Date        Name        			Comments
 *      12/12/2013  Keny       				Created
 *
 *
 * Copyright (c) 2015 Ambarella, Inc.
 *
 * This file and its contents (¡°Software¡±) are protected by intellectual property rights
 * including, without limitation, U.S. and/or foreign copyrights.  This Software is also the
 * confidential and proprietary information of Ambarella, Inc. and its licensors.  You may
 * not use, reproduce, disclose, distribute, modify, or otherwise prepare derivative
 * works of this Software or any portion thereof except pursuant to a signed license
 * agreement or nondisclosure agreement with Ambarella, Inc. or its authorized
 * affiliates.	In the absence of such an agreement, you agree to promptly notify and
 * return this Software to Ambarella, Inc.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF NON-
 * INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL AMBARELLA, INC. OR ITS AFFILIATES BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; COMPUTER FAILURE OR
 * MALFUNCTION; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#ifndef _AMBA_IPC_RPC_PROG_LU_MSG_H_
#define _AMBA_IPC_RPC_PROG_LU_MSG_H_

#include "AmbaIPC_Rpc_Def.h"

//RPC_INFO definition
#define AMBA_RPC_PROG_LU_MSG_PROG_ID 0x20000002
#define AMBA_RPC_PROG_LU_MSG_HOST AMBA_IPC_HOST_LINUX
#define AMBA_RPC_PROG_LU_MSG_VER (1)
#define AMBA_RPC_PROG_LU_MSG_DEFULT_TIMEOUT (500)

//RPC_FUNC definition
#define AMBA_RPC_PROG_LU_MSG_FUNC_SEND (1)

typedef struct _AMBA_RPC_PROG_LU_MSG_MSGBLK_s_ {
	unsigned char priority;
	unsigned int src_mqid;
	unsigned int param_size;
	unsigned char param[256]; /* Message contents */
} AMBA_RPC_PROG_LU_MSG_MSGBLK_s;

typedef struct _AMBA_RPC_PROG_LU_MSG_DATABLK_s_ {
	int dst_mqid;
	AMBA_RPC_PROG_LU_MSG_MSGBLK_s msg_blk;
} AMBA_RPC_PROG_LU_MSG_DATABLK_s;

AMBA_IPC_REPLY_STATUS_e AmbaRpcProg_LU_Msg_Send_Clnt(AMBA_RPC_PROG_LU_MSG_DATABLK_s *pArg, int *pResult, int Clnt );
void AmbaRpcProg_LU_Msg_Send_Svc(AMBA_RPC_PROG_LU_MSG_DATABLK_s *pArg, AMBA_IPC_SVC_RESULT_s *pRet);

#endif /* _AMBA_IPC_RPC_PROG_LU_MSG_H_ */
