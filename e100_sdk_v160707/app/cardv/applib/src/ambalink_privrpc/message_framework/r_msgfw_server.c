/**
 * @file app/connected/applib/src/ambalink_privrpc_message_framework/r_msgfw_server.c
 *
 * Implementation for r_msgfw service
 *
 * History:
 *    2013/12/02 - [Keny Huang ] created file
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

#include <stdio.h>
#include <AmbaDataType.h> //This has to be included before AmbaLink.h
#include <AmbaLink.h>
#include <AmbaPrintk.h>
#include <AmbaIPC_RpcProg_R_Msg.h>
#include "ApplibAmbaRPC_Msg.h"

static unsigned char *g_stk_addr = NULL;
static AMBA_RPC_R_MSG_RECV_f recv_cb = NULL;

#if 0
static void print_paramdata(unsigned char *Data,unsigned int DataLength)
{
	//int count;

	AmbaPrint("\tparam_size=%d\n",DataLength);
	/*AmbaPrint("\tparam=\n");
	for (count=0;count<(int)DataLength;count++) {
		if (count%16==0) {
			AmbaPrint("\n");
		}
		AmbaPrint("%02x ", Data[count]);
	}*/
	AmbaPrint("\n\n");
}

static void show_msgblk(amba_msgblk_t *msgblk)
{

	AmbaPrint("\nmsg_blk info:\n");
	AmbaPrint("\tpriority=%x\n", msgblk->priority);
	AmbaPrint("\tsrc_mqid=%x\n", msgblk->src_mqid);
	print_paramdata(msgblk->param, msgblk->param_size);
}
#endif

void R_MSGFW_SEND_FUNC_svc(AmbaRPC_MsgBlk_s *msgblk, AMBA_IPC_SVC_RESULT_s *ret)
{
	AmbaPrint("%s\n",__FUNCTION__);
	if (recv_cb!=NULL) {
		recv_cb(msgblk);
    }
        ret->Mode = AMBA_IPC_SYNCHRONOUS; //AMBA_IPC_ASYNCHRONOUS;
        ret->Status = AMBA_IPC_REPLY_SUCCESS;

}


/*----------------------------------------------------------------------------*\
 *  @RoutineName::  RPC_R_MSGFW_Init
 *
 *  @Description::  init function for r_msgfw server
 *
 *  @Return     ::
 *      int : OK(0)/NG(-1)
 *
\*----------------------------------------------------------------------------*/

int RPC_R_MSGFW_Init(int priority, unsigned char *stk_addr, unsigned int stksize)
{
	int status;
	AMBA_IPC_PROG_INFO_s prog_info[1];

	AmbaPrint("%s\n",__FUNCTION__);

	prog_info->ProcNum = 1;
        prog_info->pProcInfo = AmbaLink_Malloc(prog_info->ProcNum*sizeof(AMBA_IPC_PROC_s));
        prog_info->pProcInfo[0].Mode = AMBA_IPC_SYNCHRONOUS;
        prog_info->pProcInfo[0].Proc = (AMBA_IPC_PROC_f) &R_MSGFW_SEND_FUNC_svc;

	if (stk_addr==NULL || stksize==0) {
		AmbaPrint("%s: Invalid stack info. stk_addr=%p, stksize=%u\n",
			__FUNCTION__,stk_addr,stksize);
		return -1;
	}

	if (g_stk_addr==0) {
		status = AmbaIPC_SvcRegister(AMBA_RPC_PROG_R_MSG_PROG_ID, AMBA_RPC_PROG_R_MSG_VER, "i_msgfw_svc",
			priority, stk_addr, stksize, prog_info, 1);

		if (status!=0) {
			AmbaPrint("%s: AmbaIPC_SvcRegister fail.\n");
			return -1;
		}

		g_stk_addr = stk_addr;
	}
	AmbaLink_Free((void*)prog_info->pProcInfo);
	return 0;
}

/*----------------------------------------------------------------------------*\
 *  @RoutineName::  RPC_R_MSGFW_Release
 *
 *  @Description::  release function for r_msgfw server
 *
 *  @Return     ::
 *      (unsigned char *) : NG(NULL)/OK(address of stack)
 *
\*----------------------------------------------------------------------------*/
unsigned char *RPC_R_MSGFW_Release(void)
{
	int status;
	unsigned char *stk_addr = NULL;

	AmbaPrint("%s\n",__FUNCTION__);
	if (g_stk_addr!=NULL) {
		status = AmbaIPC_SvcUnregister(AMBA_RPC_PROG_R_MSG_PROG_ID, AMBA_RPC_PROG_R_MSG_VER);
		if (status!=0) {
			AmbaPrint("%s: AmbaIPC_SvcUnregister fail.\n");
			return NULL;
		}
		stk_addr = g_stk_addr;
		g_stk_addr = NULL;
	}

	return stk_addr;
}

/*----------------------------------------------------------------------------*\
 *  @RoutineName::  RPC_R_MSGFW_Reg_CB
 *
 *  @Description::  register callback for message from Linux
 *
 *  @Return     ::
 *      int : NG(-1)/OK(0)
 *
\*----------------------------------------------------------------------------*/
int RPC_R_MSGFW_Reg_CB(AMBA_RPC_R_MSG_RECV_f cb_func)
{
	recv_cb = cb_func;
	return 0;
}
