/**
 * @file app/connected/applib/src/ambalink_prisvc/message_framework/AmbaLink_RPC_Msg.c
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

#include <AmbaDataType.h>
#include <AmbaKAL.h>
#include <AmbaPrint.h>
#include "ApplibAmbaRPC_Msg.h"


/*----------------------------------------------------------------------------*\
 *  @RoutineName::  AmbaRPC_MSG_Init
 *
 *  @Description::  Init Message framework module
 *
 *  @Return     ::
 *      int : OK(0)/NG(-1)
 *
\*----------------------------------------------------------------------------*/
int AmbaRPC_MSG_Init(int priority, unsigned char *stk_addr, unsigned int stksize)
{
    int status;

    status = RPC_R_MSGFW_Init(priority,stk_addr,stksize);
    if (status<0) {
    	AmbaPrint("AmbaRPC_MSG_Init: RPC_R_MSGFW_Init fail!");
	return -1;
    }

    /* do not init client due to it will wait forever now */
    //RPC_LU_MSGFW_Init();

    return 0;
}

/*----------------------------------------------------------------------------*\
 *  @RoutineName::  AmbaRPC_MSG_Release
 *
 *  @Description::  release Message framework module
 *
 *  @Return     ::
 *      UINT8 * : stack address
 *
\*----------------------------------------------------------------------------*/
UINT8 *AmbaRPC_MSG_Release(void)
{
    UINT8 *stk_addr;

    RPC_LU_MSGFW_Release();
    stk_addr=(UINT8 *)RPC_R_MSGFW_Release();

    return stk_addr;
}

/*----------------------------------------------------------------------------*\
 *  @RoutineName::  AmbaRPC_MSG_Send
 *
 *  @Description::  Send Message to Linux
 *
 *  @Input      ::  message data block to be send
 *
 *  @Return     ::
 *      int : OK(0)/NG(-1)
 *
\*----------------------------------------------------------------------------*/
int AmbaRPC_MSG_Send(AmbaRPC_Msg_DataBlk_t *pDataBlk)
{
    int status;

    status = RPC_LU_MSGFW_Send(pDataBlk);
    if (status<0) {
    	AmbaPrint("AmbaRPC_MSG_Send: RPC_LU_MSGFW_Send fail!");
	return -1;
    }

    return 0;
}

/*----------------------------------------------------------------------------*\
 *  @RoutineName::  AmbaRPC_MSG_Reg_CB
 *
 *  @Description::  register callback to handle message from Linux
 *
 *  @Input      ::  callback function
 *
 *  @Return     ::
 *      int : OK(0)/NG(-1)
 *
\*----------------------------------------------------------------------------*/
int AmbaRPC_MSG_Reg_CB(AMBA_RPC_R_MSG_RECV_f CbFun)
{
    return RPC_R_MSGFW_Reg_CB(CbFun);
}
