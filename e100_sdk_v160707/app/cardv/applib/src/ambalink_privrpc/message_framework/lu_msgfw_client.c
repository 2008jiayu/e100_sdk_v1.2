/**
 * @file app/connected/applib/src/ambalink_privrpc_message_framework/lu_msgfw_client.c
 *
 * Ambarella DSP Image Kernel CfaNoiseFilter APIs
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
#include <AmbaIPC_RpcProg_LU_Msg.h>
#include "ApplibAmbaRPC_Msg.h"

static int clnt = 0;

/*----------------------------------------------------------------------------*\
 *  @RoutineName::  RPC_LU_MSGFW_Init
 *
 *  @Description::  init function for lu_mshfw client
 *
 *  int : OK(0)/NG(-1)
 *
\*----------------------------------------------------------------------------*/
int RPC_LU_MSGFW_Init(void)
{
	AmbaPrint("RPC_LU_MSGFW_Init");
	if (clnt!=0) {
		AmbaPrint("RPC_LU_MSGFW_Init reinited.");
		return 0;
	}

	clnt = AmbaIPC_ClientCreate(AMBA_IPC_HOST_LINUX,
                    AMBA_RPC_PROG_LU_MSG_PROG_ID, AMBA_RPC_PROG_LU_MSG_VER);
	if (clnt==0) {
	    AmbaPrint("%s: Cient creation failed\n",__FUNCTION__);
	    return -1;
	}

	AmbaPrint("RPC_LU_MSGFW_Init done!");
	return 0;
}


/*----------------------------------------------------------------------------*\
 *  @RoutineName::  RPC_LU_MSGFW_Send
 *
 *  @Description::  send data to Linux
 *
 *  @Input      ::
 *      data_blk : specify the dst_mqid and data to be sent.
 *
 *  int : OK(0)/NG(-1)
 *
\*----------------------------------------------------------------------------*/
int RPC_LU_MSGFW_Send(AmbaRPC_Msg_DataBlk_t *data_blk)
{
	AMBA_RPC_PROG_LU_MSG_DATABLK_s *lu_blk;
	int status;

	if (clnt==0) {
		if (RPC_LU_MSGFW_Init()<0) {
			return -1;
		}
	}

	lu_blk = (AMBA_RPC_PROG_LU_MSG_DATABLK_s *)data_blk;
	status = AmbaIPC_ClientCall(clnt, AMBA_RPC_PROG_LU_MSG_FUNC_SEND,
                     (void*)lu_blk, sizeof(AMBA_RPC_PROG_LU_MSG_DATABLK_s),
                     NULL, 0, AMBA_RPC_PROG_LU_MSG_DEFULT_TIMEOUT);

	if (status!=AMBA_IPC_REPLY_SUCCESS) {
		AmbaPrint("%s: AmbaIPC_ClientCall failed (%d)\n",
                          __FUNCTION__,status);
		return -1;
	}

	return 0;
}


/*----------------------------------------------------------------------------*\
 *  @RoutineName::  RPC_LU_MSGFW_Release
 *
 *  @Description::  release function for lu_mshfw client
 *
 *  int : OK(0)/NG(-1)
 *
\*----------------------------------------------------------------------------*/
int RPC_LU_MSGFW_Release(void)
{
	if (clnt!=0) {
		AmbaIPC_ClientDestroy(clnt);
		clnt = 0;
	}

	return 0;
}

