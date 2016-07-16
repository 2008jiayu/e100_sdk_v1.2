/**
 * @file app/connected/applib/src/ambalink_prisvc/message_framework/AmbaLink_RPC_Test.c
 *
 * Test/Reference code for AmbaIPC
 *
 * History:
 *    2013/12/02 - [Keny huang ] created file
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

#include <AmbaDataType.h>
#include <AmbaKAL.h>

#include <AmbaLink.h>
#include <AmbaTest.h>
#include <AmbaPrintk.h>
#include "ApplibAmbaLink_RPC.h"

#define AmbaPrintNN(pFmt, ...)       AmbaPrintk(0, 0, \
                                                0, AMBA_PRINTK_MESSAGE_LEVEL,  \
                                                AMBA_PRINTK_CONSOLE_LEVEL,   RED,                      \
                                                pFmt,                        ##__VA_ARGS__)

/*============================================================================*\
 *                                                                            *
 *                      TEST CODE for AmbaLink_RPC                            *
 *                                                                            *
\*============================================================================*/

static void print_paramdata(unsigned char *Data,unsigned int DataLength)
{
	int count;

	AmbaPrint("\tParamSize=%d\n",DataLength);
	AmbaPrintNN("Param=\n\r");
	for (count=0;count<(int)DataLength;count++) {
		if (count%16==0) {
			AmbaPrintNN("\n\r");
		}
		AmbaPrintNN("%02x ", Data[count]);
	}
        AmbaPrintNN("\n\r");
	AmbaPrint("\n\n");
}

static void show_msgblk(AmbaRPC_MsgBlk_s *msgblk)
{

	AmbaPrint("\nmsg_blk info:\n");
	AmbaPrint("\tPriority=%x\n", msgblk->Priority);
	AmbaPrint("\tSrcMqId=%x\n", msgblk->SrcMqId);
	print_paramdata(msgblk->Param, msgblk->ParamSize);
}

/*----------------------------------------------------------------------------*\
 *  @RoutineName::  rpc_msg_receiver
 *
 *  @Description::  process function for message from Linux side
 *
 *  @Input      ::
 *      msgblk  : message block
 *
 *  The msgfw will just pass message data in raw format (msgblk->param).
 *  User has to casting/parsing it in predefined format as sender defined.
 *
\*----------------------------------------------------------------------------*/
static void rpc_msg_receiver(AmbaRPC_MsgBlk_s *msgblk)
{
	int *msgid, *param1, *param2;

	msgid = (int *)(&msgblk->Param);
	switch (*msgid) {
	case 0xF0000036:
		AmbaPrint("rpc_msg_receiver: Menu Key!");
		break;
	case 0xF403002c:
		AmbaPrint("rpc_msg_receiver: Net Interface connected!");
		param1 = msgid+sizeof(int);
		param2 = param1+sizeof(int);
		AmbaPrint("param1:%x, param2:%x\n",*param1, *param2);
		break;
	default:
		AmbaPrint("unknown message: %x",msgid);
		show_msgblk(msgblk);
		break;
	}
}


/*----------------------------------------------------------------------------*\
 *  @RoutineName::  IpcTestRpc
 *
 *  @Description::  test case for RPC
 *
 *  @Return     ::
 *      int : OK(0)/NG(-1)
 *
\*----------------------------------------------------------------------------*/
static int rpc_msg_send(AMBA_SHELL_ENV_s *env, int argc, char **argv)
{
	int status;

	if (argc < 2)
	{
		AmbaShell_Print(env, "supported test commands\n");
		AmbaShell_Print(env, "    send [string]:    send message to Linux\n");
		return 1;
	}

	do {
		AmbaRPC_Msg_DataBlk_t datablk;
		AmbaRPC_MsgBlk_s *msgblk;
		char *cmd;
		int i, len, max_len;

		datablk.DstMqId = 0; //to active one
		msgblk = &datablk.MsgBlk;

		msgblk->Priority = 0x10;
		msgblk->SrcMqId = 0;

		cmd = (char *)msgblk->Param;
		cmd[0] = '\0';
		len = 0;
		max_len = 256;

		for (i = 1; i < argc; i++) {
			len += snprintf(cmd + len, max_len - len - 3, argv[i]);
			cmd[len++] = ' ';
		}
		cmd[len++] = '\n';
		cmd[len++] = '\0';

		msgblk->ParamSize = len;

		//show_msgblk(msgblk);

		status = AmbaRPC_MSG_Send(&datablk);
		if (status < 0) {
			AmbaShell_Print(env, "RPC_LU_MSGFW_Send fail\n");
			return -1;
		}
	} while (0);

	return 0;
}

/*============================================================================*\
 *                                                                            *
 *                           Test module Management                           *
 *                                                                            *
\*============================================================================*/
struct test_list_t {
    char*     name;
    //test_proc func;
    AMBA_TEST_COMMAND_HANDLER_f func;
};

static struct test_list_t test_lists[] = {
    {"send", rpc_msg_send},
};
#define TEST_LIST_SIZE (sizeof(test_lists)/sizeof(test_lists[0]))

static int IpcTestEntry(AMBA_SHELL_ENV_s *env, int argc, char **argv)
{
    int subcmd_invalid = 1, i;

    if (argc >= 2) {
        for (i = 0; i < TEST_LIST_SIZE; i++) {
            if (!strcmp(test_lists[i].name, argv[1])) {
                subcmd_invalid = 0;
                test_lists[i].func(env, argc - 1, &argv[1]);
                break;
            }
        }
    }

    if (subcmd_invalid) {
        AmbaShell_Print(env, "Supported test commands:\n");
        for (i = 0; i < TEST_LIST_SIZE; i++) {
            AmbaShell_Print(env, "    %s\n", test_lists[i].name);
        }
    }

    return 0;
}

/*----------------------------------------------------------------------------*\
 *  @RoutineName::  AmbaIPC_TestInit
 *
 *  @Description::  Init test module
 *
 *  @Return     ::
 *      int : OK(0)/NG(-1)
 *
\*----------------------------------------------------------------------------*/
int AmbaLink_RPCInit(void)
{
    int status;

    AmbaTest_Init();

    //register svc
    do {
	    unsigned char *stack;
	    stack = (unsigned char *)AmbaLink_Malloc(AMBALINK_RPC_SVC_STKSIZE);
            status = AmbaRPC_MSG_Init(15,stack,AMBALINK_RPC_SVC_STKSIZE);
	    if (status<0) {
	    	AmbaPrint("AmbaLink_RPCInit: RPC_R_MSGFW_Init fail!");
		return -1;
	    }
	    AmbaRPC_MSG_Reg_CB(rpc_msg_receiver);
    } while (0);

    //test_add_cmd("amba_rpc", IpcTestEntry);
    AmbaTest_RegisterCommand("amba_rpc", IpcTestEntry);


    return 0;
}
