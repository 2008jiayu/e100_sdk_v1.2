/**
 * @file app/connected/applib/src/ambalink_prisvc/message_framework/ApplibAmbaLink_RPC.h
 *
 * Definitions & Constants for Ambarella CPU Link and Network Supports
 *
 * History:
 *    2013/12/10 - [Keny huang ] created file
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

#ifndef _AMBALINK_RPC_H_
#define _AMBALINK_RPC_H_

/*---------------------------------------------------------------------------*\
 * AmbaLink_RPC definition.
\*---------------------------------------------------------------------------*/
#define AMBALINK_RPC_SVC_STKSIZE (0x1000)

/*---------------------------------------------------------------------------*\
 * MSG framework related data structure.
\*---------------------------------------------------------------------------*/
typedef struct _AmbaRPC_MsgBlk_s_ {
   UINT8  Priority;
   UINT32 SrcMqId;
   UINT32 ParamSize;
   UINT8  Param[256]; /* Message contents */
} AmbaRPC_MsgBlk_s;

typedef struct _AmbaRPC_Msg_DataBlk_t_ {
   int DstMqId;
   AmbaRPC_MsgBlk_s MsgBlk;
} AmbaRPC_Msg_DataBlk_t;

typedef void (*AMBA_RPC_R_MSG_RECV_f)(AmbaRPC_MsgBlk_s *);

/*----------------------------------------------------------------------------*\
 * MSG framework interface
\*----------------------------------------------------------------------------*/
int AmbaRPC_MSG_Init(int priority, unsigned char *stk_addr, unsigned int stksize);
UINT8 *AmbaRPC_MSG_Release(void);
int AmbaRPC_MSG_Send(AmbaRPC_Msg_DataBlk_t *pDataBlk);
int AmbaRPC_MSG_Reg_CB(AMBA_RPC_R_MSG_RECV_f CbFun);


#endif  /* _AMBALINK_RPC_H_ */

