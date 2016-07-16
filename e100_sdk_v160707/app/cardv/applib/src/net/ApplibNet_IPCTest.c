/**
 * @file app/connected/applib/src/net/ApplibNet_IPCTest.c
 *
 * Porting from Test/Reference code for AmbaIPC
 *
 * History:
 *    2013/10/28 - [Keny huang ] created file
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
#include <string.h>

#ifndef CONFIG_APP_AMBA_LINK
#error "Fatal Error!! You need to add \"CONFIG_APP_AMBA_LINK\" into your project's defined symbols!"
#endif

#include <AmbaDataType.h>
#include <AmbaPrintk.h>
#include <AmbaKAL.h>
#include <common/common.h>

#include <AmbaTest.h>
#include <AmbaLink.h>
#include <AmbaUtility.h>
#include <AmbaIPC_RpcProg_R_Test.h>
#include <AmbaIPC_RpcProg_LU_Test.h>
#include <AmbaIPC_RpcProg_LU_Util.h>

#define AmbaPrintNN(pFmt, ...)       AmbaPrintk(0, 0, \
                                                0, AMBA_PRINTK_MESSAGE_LEVEL,  \
                                                AMBA_PRINTK_CONSOLE_LEVEL,   RED,                      \
                                                pFmt,                        ##__VA_ARGS__)

static AMBA_KAL_BYTE_POOL_t *g_HeapPool=NULL;

/*============================================================================*\
 *                                                                            *
 *                             TEST CODE for Spinlock                         *
 *                                                                            *
\*============================================================================*/

/*----------------------------------------------------------------------------*\
 *  @RoutineName::  IpcTestSlock
 *
 *  @Description::  spinlock test case, triggerred by "t ipc slock"
 *      should work in collabration with /proc/ambarella/spinlock on linux side
 *
 *  @Return     ::
 *      int : OK(0)/NG(-1)
\*----------------------------------------------------------------------------*/
static int IpcTestSlock(AMBA_SHELL_ENV_s *env, int argc, char **argv)
{
    UINT32 id, flags;

    if (argc == 1)
    {
        AmbaPrint("supported test commands");
        AmbaPrint("    slock n + : lock n");
        AmbaPrint("    slock n - : unlock n");
        AmbaPrint("    slock n   : lock and then unlock with irq");
        return 1;
    }

    sscanf(argv[1], "%u", &id);
    if (argc == 2)
    {
        AmbaPrint("try spinlock %d", id);
        AmbaIPC_SpinLockIrqSave(id, &flags);
        AmbaIPC_SpinUnlockIrqRestore(id, flags);
        AmbaPrint("done");
    }
    else if (argv[2][0] == '+')
    {
        AmbaIPC_SpinLock(id);
        AmbaPrint("Spinlock %d is locked", id);
    }
    else if (argv[2][0] == '-')
    {
        AmbaIPC_SpinUnlock(id);
        AmbaPrint("Spinlock %d is unlocked", id);
    }

    return 0;
}

/*============================================================================*\
 *                                                                            *
 *                              TEST CODE for MUTEX                           *
 *                                                                            *
\*============================================================================*/

/*----------------------------------------------------------------------------*\
 *  @RoutineName::  IpcTestMutex
 *
 *  @Description::  mutex test case, triggerred by "t ipc mutex"
 *      should work in collabration with /proc/ambarella/mutex on linux side
 *
 *  @Return     ::
 *      int : OK(0)/NG(-1)
\*----------------------------------------------------------------------------*/
static int IpcTestMutex(AMBA_SHELL_ENV_s *env, int argc, char **argv)
{
    UINT32 id;

    if (argc == 1)
    {
        AmbaPrint("supported test commands");
        AmbaPrint("    mutex n + [timeout]: lock n");
        AmbaPrint("    mutex n - : unlock n");
        AmbaPrint("    mutex n   : lock and then unlock with irq");
        return 1;
    }

    sscanf(argv[1], "%u", &id);
    if (argc == 2)
    {
        AmbaPrint("try mutex %d", id);
        AmbaIPC_MutexTake(id, AMBA_KAL_WAIT_FOREVER);
        AmbaIPC_MutexGive(id);
        AmbaPrint("done");
    }
    else if (argv[2][0] == '+')
    {
        ULONG ticks = AMBA_KAL_WAIT_FOREVER;
        UINT status;
        if (argc == 4)
        {
            sscanf(argv[3], "%lu", &ticks);
        }
        status = AmbaIPC_MutexTake(id, ticks);

        if (!status)
        {
            AmbaPrint("Mutex %d is locked", id);
        }
        else
        {
            AmbaPrint("timed out");
        }
    }
    else if (argv[2][0] == '-')
    {
        AmbaIPC_MutexGive(id);
        AmbaPrint("Mutex %d is unlocked", id);
    }

    return 0;
}

/*============================================================================*\
 *                                                                            *
 *                               TEST CODE for RPC                            *
 *                                                                            *
\*============================================================================*/
#if RPC_DEBUG
extern void CalcRpcProfile(void);
extern void AmbaIPC_InitProfile(void);
#endif
/*----------------------------------------------------------------------------*\
 *  @RoutineName::  AmbaRpcProg_Test_Print_Svc
 *
 *  @Description::  test case 1 for RPC
 *
 *  @Return     ::
 *      int :
 *
\*----------------------------------------------------------------------------*/
void AmbaRpcProg_R_Test_Print_Svc(const char *msg, AMBA_IPC_SVC_RESULT_s *pRet)
{
    AmbaPrint("\t[svc echo] %s", (char *)msg);
    pRet->Mode = AMBA_IPC_ASYNCHRONOUS;
    pRet->Status = AMBA_IPC_REPLY_SUCCESS;
}

/*----------------------------------------------------------------------------*\
 *  @RoutineName::  AmbaRpcProg_Test_Sum_Svc
 *
 *  @Description::  test case 2 for RPC
 *
 *  @Return     ::
 *      int :
 *
\*----------------------------------------------------------------------------*/
void AmbaRpcProg_R_Test_Sum_Svc(AMBA_RPC_PROG_R_TEST_SUM_ARG_s *pArg, AMBA_IPC_SVC_RESULT_s *ret)
{
    int sum;

    if (pArg->a >= 256 || pArg->b >= 256) {
        ret->Status = AMBA_IPC_REPLY_PARA_INVALID;
    } else {
        sum = pArg->a + pArg->b;
        memcpy(ret->pResult, &sum, 4);
        ret->Length = 4;
        ret->Status = AMBA_IPC_REPLY_SUCCESS;
    }
    ret->Mode = AMBA_IPC_SYNCHRONOUS;
}

/*----------------------------------------------------------------------------*\
 *  @RoutineName::  AmbaRpcProg_Test_Print_Clnt
 *
 *  @Description::  test case 1 for RPC client
 *
 *  @Return     ::
 *      int :
 *
\*----------------------------------------------------------------------------*/
AMBA_IPC_REPLY_STATUS_e AmbaRpcProg_LU_Test_Print_Clnt(const char *pStr, int *pResult, int Clnt)
{
    AMBA_IPC_REPLY_STATUS_e status;
    status = AmbaIPC_ClientCall(Clnt, AMBA_RPC_PROG_LU_TEST_PRINT, (void *) pStr, strlen(pStr)+1, NULL, 0, 0);
    return status;
}

/*----------------------------------------------------------------------------*\
 *  @RoutineName::  AmbaRpcProg_Test_Sum_Clnt
 *
 *  @Description::  test case 2 for RPC client
 *
 *  @Return     ::
 *      int :
 *
\*----------------------------------------------------------------------------*/
AMBA_IPC_REPLY_STATUS_e AmbaRpcProg_LU_Test_Sum_Clnt(AMBA_RPC_PROG_LU_TEST_SUM_ARG_s *pArg, int *Result, int Clnt)
{
    AMBA_IPC_REPLY_STATUS_e status;
    status = AmbaIPC_ClientCall(Clnt, AMBA_RPC_PROG_LU_TEST_SUM, pArg, sizeof(AMBA_RPC_PROG_LU_TEST_SUM_ARG_s), Result, 4, 1000);
    return status;
}

typedef struct IpcTestSvcInfo_s {
    UINT prog;
    char *stack;
}IpcTestSvcInfo_t;

#define IPCTEST_MAX_SVC_AMOUNT (10)
static IpcTestSvcInfo_t SvcInfo[IPCTEST_MAX_SVC_AMOUNT]={0};

/*----------------------------------------------------------------------------*\
 *  @RoutineName::  IpcTestRpc
 *
 *  @Description::  test case for RPC
 *
 *  @Return     ::
 *      int : OK(0)/NG(-1)
 *
\*----------------------------------------------------------------------------*/
static int IpcTestRpc(AMBA_SHELL_ENV_s *env, int argc, char **argv)
{
    AMBA_RPC_PROG_LU_TEST_SUM_ARG_s pSum[1];
    AMBA_RPC_PROG_EXEC_ARG_s *pExec = NULL;
    int clnt = 0, ret = 0, i = 0, rem_len = 0;
    char *output = NULL;
    char buffer[2048];
    AMBA_IPC_PROG_INFO_s prog_info[1];
    extern void AmbaIPC_ListSvc(AMBA_SHELL_ENV_s *env);

    if (argc == 1)
    {
        AmbaPrint("supported test commands");
        AmbaPrint("    rpc list:    list registered program");
        AmbaPrint("    rpc svc [prog_name] +: register program");
        AmbaPrint("    rpc svc [prog_name] -: unregister program");
        AmbaPrint("             supported programs: test (test rpc program)");
        AmbaPrint("                                 exec (execute command in Linux)");
        AmbaPrint("    rpc clnt test 1 str: echo [str]");
        AmbaPrint("    rpc clnt test 2 a: calulate a+a");
        AmbaPrint("    rpc clnt exec1 [command] (execute command in Linux and return the result)");
        AmbaPrint("    rpc clnt exec2 [command] (execute command in Linux)");
        AmbaPrint("    rpc profile start");
        AmbaPrint("    rpc profile stop");
        return 1;
    }

    if (!strcmp(argv[1], "list")) {
        AmbaIPC_ListSvc(env);
    } else if (!strcmp(argv[1], "svc")) {
        if (argc != 4) {
            AmbaPrint("invalid input for svc");
            return 0;
        }
        if (strcmp(argv[2],"test")==0) {
            if (argv[3][0] == '+') {
                char *stack = AmbaLink_Malloc(0x1000);
                prog_info->ProcNum = 2;
                prog_info->pProcInfo = AmbaLink_Malloc(prog_info->ProcNum*sizeof(AMBA_IPC_PROC_s));
                prog_info->pProcInfo[0].Mode = AMBA_IPC_ASYNCHRONOUS;
                prog_info->pProcInfo[1].Mode = AMBA_IPC_SYNCHRONOUS;
                prog_info->pProcInfo[0].Proc = (AMBA_IPC_PROC_f) &AmbaRpcProg_R_Test_Print_Svc;
                prog_info->pProcInfo[1].Proc = (AMBA_IPC_PROC_f) &AmbaRpcProg_R_Test_Sum_Svc;
                AmbaIPC_SvcRegister(AMBA_RPC_PROG_R_TEST_PROG_ID, AMBA_RPC_PROG_R_TEST_VER, "test_rpc_svc",
                                    65, stack, 0x1000, prog_info, 1);
                AmbaLink_Free((void*)prog_info->pProcInfo);

                //keep svc info
                SvcInfo[0].prog = AMBA_RPC_PROG_R_TEST_PROG_ID;
                SvcInfo[0].stack = stack;
            } else {
                //Unregister svc
                AmbaIPC_SvcUnregister(AMBA_RPC_PROG_R_TEST_PROG_ID, AMBA_RPC_PROG_R_TEST_VER);

                //release stack
                AmbaLink_Free((void*)SvcInfo[0].stack);
            }
        }
    } else if (!strcmp(argv[1], "clnt")) {
        if(strcmp(argv[2], "test")==0){
           clnt = AmbaIPC_ClientCreate(AMBA_IPC_HOST_LINUX, AMBA_RPC_PROG_LU_TEST_PROG_ID, AMBA_RPC_PROG_LU_TEST_VER);
        }
        else if(strcmp(argv[2], "exec1")==0 || strcmp(argv[2], "exec2")==0){
            clnt = AmbaIPC_ClientCreate((int)AMBA_RPC_PROG_LU_UTIL_HOST, (int)AMBA_RPC_PROG_LU_UTIL_PROG_ID, (int)AMBA_RPC_PROG_LU_UTIL_VER);
            pExec = AmbaLink_Malloc(sizeof(AMBA_RPC_PROG_EXEC_ARG_s)+100*sizeof(char));
            output = AmbaLink_Malloc(EXEC_OUTPUT_SIZE);
            memset(output, 0, EXEC_OUTPUT_SIZE);
            strcpy(pExec->command, argv[3]);
            /* Concatenate the arguments of the command from the input*/
            for(i=4; i<argc; i++) {
                strcat(pExec->command, " ");
                strcat(pExec->command, argv[i]);
            }
        }

        if (clnt == 0) {
            AmbaPrint("Client creation failed");
            return 0;
        }
        if (strcmp(argv[2], "test")==0) {
            switch (argv[3][0]) {
            case '1':
                AmbaRpcProg_LU_Test_Print_Clnt(argv[4], NULL, clnt);
                break;
            case '2':
                sscanf(argv[4], "%d", &(pSum->a));
                pSum->b = pSum->a;
                AmbaRpcProg_LU_Test_Sum_Clnt(pSum, &ret, clnt);
                AmbaPrint("\treturn value is %d", ret);
                break;
            }
        }
        if (strcmp(argv[2], "exec1")==0) {
            ret = AmbaRpcProg_Util_Exec1_Clnt(pExec, (int *)output, clnt);
            AmbaLink_Free((void*)pExec);
            if(ret != 0){
                AmbaPrint("\tExecute exec1 failed: status %d", ret);
                AmbaLink_Free((void*)output);
                return 0;
            }

        /* Note: The string length is limited to 1024 in AmbaShell_Print */
            rem_len = strlen(output)+1;
            i = 0;
            while ( rem_len>0 ) {
#if 1
                int sp;
                char *tp = buffer;
                for(sp=0; ((rem_len>sp)&&(sp<1024)); sp++){
                    if(output[i+sp]=='\n') {
                        *tp = '\n';
                        tp++;
                        *tp = '\r';
                    } else {
                        *tp = output[i+sp];
                    }
                    tp++;
                }
                *tp = '\0';
#else
                if (rem_len>1024) {
                    memcpy(buffer, &output[i], 1023);
                    buffer[1023]='\0';
                }
                else{
                    memcpy(buffer, &output[i], rem_len);
                }
#endif
                //AmbaPrint("%s", buffer);
                AmbaPrintNN("%s", buffer);
                rem_len -= 1024;
                i += 1024;
            }
            AmbaLink_Free((void*)output);
        }
        else if (strcmp(argv[2], "exec2")==0) {
            ret = AmbaRpcProg_Util_Exec2_Clnt(pExec, (int *)output, clnt);
            if(ret != 0){
                AmbaPrint("\tExecute exec1 failed: status %d", ret);
            }
            AmbaLink_Free((void*)output);
            AmbaLink_Free((void*)pExec);
        }
        AmbaIPC_ClientDestroy(clnt);
    } else if(!strcmp(argv[1], "profile")){
        if(!RPC_DEBUG){
            AmbaPrint("please turn on RPC_DEBUG and recompile");
            return 1;
        }
        if(!strcmp(argv[2], "start")){
    #if RPC_DEBUG
            AmbaIPC_InitProfile();  // clear the rpc profile
        }
        else if(!strcmp(argv[2], "stop")){
            CalcRpcProfile();
    #endif
        }
    }
    else {
        AmbaPrint("unknown  command %s", argv[1]);
    }

    return 0;
}

/*============================================================================*\
 *                                                                            *
 *                              TEST CODE for RPMSG                           *
 *                                                                            *
\*============================================================================*/
static AMBA_IPC_HANDLE Channel;
#if RPC_DEBUG
extern void InitRpmsgProfile(void);
#endif
#if RPMSG_DEBUG
extern void CalcRpmsgProfile(void);
#endif

/*----------------------------------------------------------------------------*\
 *  @RoutineName::  MsgHandler
 *
 *  @Description::  Channel Message Handler
 *
 *  @Return     ::
 *      int : OK(0)/NG(-1)
 *
\*----------------------------------------------------------------------------*/
static int MsgHandler(AMBA_IPC_HANDLE IpcHandle, AMBA_IPC_MSG_CTRL_s *pMsgCtrl)
{
    AmbaPrint("recv message: [%s]", (char *)pMsgCtrl->pMsgData);
    return 0;
}


/*----------------------------------------------------------------------------*\
 *  @RoutineName::  IpcTestRpmsg
 *
 *  @Description::  test case for a "echo" RPMSG channel
 *
 *  @Return     ::
 *      int : OK(0)/NG(-1)
 *
\*----------------------------------------------------------------------------*/
static int IpcTestRpmsg(AMBA_SHELL_ENV_s *env, int argc, char **argv)
{
    int i, count = 8;
    char msg[128], *base = NULL;

    if(argc == 1)
    {
        AmbaPrint("supported rpmsg commands");
        AmbaPrint("    rpmsg test:    send 8 echo rpmsg continuously");
        AmbaPrint("    rpmsg test [str] [count] : send number of count echo [str] rpmsg continuously");
        AmbaPrint("    rpmsg profile start");
        AmbaPrint("    rpmsg profile stop");
        return 1;
    }

    if(!strcmp(argv[1], "profile")){
#if RPMSG_DEBUG
        if(!strcmp(argv[2], "start")){
            InitRpmsgProfile();
            return 0;
        }
        else if(!strcmp(argv[2], "stop") && RPMSG_DEBUG){
            CalcRpmsgProfile();
            return 0;
        }
#endif
    }

    if (argc == 2)
    {
        if(!strcmp(argv[1],"test"))
        {
            base = "Hello Linux! ";
        }
    }
    else
    {
        base = argv[2];
        if (argc == 4)
        {
            count = atoi(argv[3]);
            AmbaPrint("count is %d", count);
        }
    }

    for(i = 0; i < count; i++)
    {
        sprintf(msg, "%s %d", base, i);
        AmbaPrint("send message: [%s]", msg);
        AmbaIPC_Send(Channel, msg, strlen(msg) + 1);
    }

    return 1;
}
/*============================================================================*\
 *                                                                            *
 *                              TEST CODE for Ramfs                           *
 *                                                                            *
\*============================================================================*/
#define THRUPUT_TIME    20000   /* time of test cycle */
//#define BUFFER_SIZE     0x20000
#define BUFFER_SIZE     0x80000
#define RESULT_SIZE     15
#define MAX_ARGC        5

#define VRFY_FIXED      0
#define NUM_VRFY        16
#define VRFY_ALL        -1

struct thruput_field_s {
    int label;
    float   read_value;
    float   write_value;
};

/*----------------------------------------------------------------------------*\
 *  @RoutineName::  RfsTestThruput
 *
 *  @Description::  Test file system throughput.
 *
 *  @Return     ::
 *      int : OK(0)/NG(-1)
 *
\*----------------------------------------------------------------------------*/
static void RfsTestThruput(AMBA_SHELL_ENV_s *env, char* slot, UINT32* bs_multi, UINT32 align)
{
    int i;
    int one_shot;
    int rval = 0;
    char path[256], data[256];
    void *fout = NULL;
    void *fin = NULL;
    int flen, count = 0;
    UINT32 dtime;
    UINT32 systim_s, systim_e;
    float sec;
    float rate;
    UINT8 *huge_buf;
    UINT32 bs_idx = 0, bs_num = 0;
    UINT32 bs = bs_multi[0];
    struct thruput_field_s thruput_result[RESULT_SIZE];

    memset(thruput_result, 0, sizeof(thruput_result));

    huge_buf = AmbaLink_Malloc(BUFFER_SIZE);

    /* get bs number */
    for (i = 0; i < MAX_ARGC; i++) {
        if (bs_multi[i] != 0)
            bs_num ++;
    }

    if (bs_num == 0) {
        bs = 0x1000;
        one_shot = 0;
    } else {
        one_shot = 1;
#if 0
        if (align != 0) {
            rval = get_align_buf(&huge_buf, &huge_buf_raw,
                                 bs_multi[0], align, 1);
            if (rval < 0) {
                printk("fioprf_thruput get_align_buf fail");
                return;
            }
        }
#endif
    }

    for (; bs <= BUFFER_SIZE; bs <<= 0x1) {

        sprintf(path, "%s/throughput_%u.bin", slot, bs);

        /********************/
        /* Large file write */
        /********************/
        if (bs_num < 2) {
            AmbaPrint("--- large file write with bs=%d ---",
                             bs);
        } else {
            UINT32 i = 0;
            AmbaPrint("--- large file write with multi bs ---");
            while (bs_multi[i] > 0) {
                AmbaPrint("--- bs %d =  %d---", i,
                                 bs_multi[i]);
                i++;
            }
        }

        thruput_result[count].label = bs;

        AmbaUtility_GetTimeStart(&systim_s);

        fout = AmbaIPC_fopen(path, "w");
        if (fout == 0 ) {
            AmbaPrint("fopen failed (%d)", fout);
            continue;
        }

        for (i = 0, flen = 0; i >= 0; i++) {

            if (bs_multi[bs_idx] != 0) {
                bs = bs_multi[bs_idx];
                bs_idx ++;
            } else {
                bs_idx = 0;
            }

            rval = AmbaIPC_fwrite(huge_buf, bs, fout);
            if (rval < 0) {
                AmbaPrint("fwrite failed (%d)", rval);
            } else if (rval == 0) {
                rval = AmbaIPC_fseek(fout, 0, AMBA_IPC_RFS_SEEK_SET);
                if (rval < 0) {
                    AmbaPrint("fseek failed (%d)",
                                    rval);
                }
            }

            flen += rval;

            dtime = AmbaUtility_GetTimeEnd(&systim_s, &systim_e);

            if (dtime > THRUPUT_TIME)
                i = -10000;
        }

        rval = AmbaIPC_fclose(fout);
        if (rval < 0) {
            AmbaPrint("fclose failed (%d)", rval);
            continue;
        }

        dtime = AmbaUtility_GetTimeEnd(&systim_s, &systim_e);

        sec = ((float) dtime) / 1000;
        rate = (flen >> 17) / sec;
        AmbaPrint("(bs=%d) wrote %d bytes", bs, flen);
        AmbaPrint("(bs=%d) write time elapsed : %f sec", bs, sec);
        AmbaPrint("(bs=%d) write throughput : %f Mbit/sec", bs, rate);

        thruput_result[count].write_value = rate;

        /*******************/
        /* Large file read */
        /*******************/
        if (bs_num < 2) {
            AmbaPrint("--- large file read with bs=%d ---", bs);
        } else {
            UINT32 i = 0;
            AmbaPrint("--- large file read with multi bs ---");
            while (bs_multi[i] > 0) {
                AmbaPrint("--- bs %d =  %d---", i,
                                 bs_multi[i]);
                i++;
            }
        }

        AmbaUtility_GetTimeStart(&systim_s);

        fin = AmbaIPC_fopen(path, "r");
        if (fin == 0) {
            AmbaPrint("fopen failed (%d)", fin);
            continue;
        }

        bs_idx = 0;

        for (i = 0, flen = 0; i >= 0; i++) {

            if (bs_multi[bs_idx] != 0) {
                bs = bs_multi[bs_idx];
                bs_idx ++;
            } else {
                bs_idx = 0;
            }

            rval = AmbaIPC_fread(huge_buf, bs, fin);
            if (rval < 0) {
                AmbaPrint("fread failed (%d)", rval);
            } else if (rval == 0) {
                rval = AmbaIPC_fseek(fin, 0, AMBA_IPC_RFS_SEEK_SET);
                if (rval < 0) {
                    AmbaPrint("fseek failed (%d)",
                                    rval);
                }
            }

            flen += rval;

            dtime = AmbaUtility_GetTimeEnd(&systim_s, &systim_e);

            if (dtime > THRUPUT_TIME)
                i = -10000;
        }

        rval = AmbaIPC_fclose(fin);
        if (rval < 0) {
            AmbaPrint("fclose failed (%d)", rval);
            continue;
        }

        dtime = AmbaUtility_GetTimeEnd(&systim_s, &systim_e);

        sec = ((float) dtime) / 1000;
        rate = (flen >> 17) / sec;
        AmbaPrint("(bs=%d) read %d bytes", bs, flen);
        AmbaPrint("(bs=%d) read time elapsed : %f sec", bs, sec);
        AmbaPrint("(bs=%d) read throughput : %f Mbit/sec", bs, rate);

        thruput_result[count++].read_value = rate;

        /*******************/
        /* Remove the file */
        /*******************/

        AmbaIPC_remove(path);

        if (one_shot) {
            break;
        }
    }

    /* save thruput result to file */
    sprintf(path, "%s/thruput.txt", slot);
    fout = AmbaIPC_fopen(path, "w");
    if (fout == 0) {
        AmbaPrint("fopen failed (%d)", fout);
        return;
    }

    count = 0;
    while (thruput_result[count].label != 0) {
        sprintf(data, "--- large file write with bs=%dK : %f",
                (thruput_result[count].label >> 10),
                thruput_result[count].write_value);

        rval = AmbaIPC_fwrite(data, strlen(data), fout);
        if (rval < 0) {
            AmbaPrint("fwrite failed (%d)", rval);
        }

        sprintf(data, "--- large file read with bs=%dK : %f",
                (thruput_result[count].label >> 10),
                thruput_result[count].read_value);

        rval = AmbaIPC_fwrite(data, strlen(data), fout);
        if (rval < 0) {
            AmbaPrint("fwrite failed (%d)", rval);
        }
        count++;
    }

    rval = AmbaIPC_fclose(fout);
    if (rval < 0) {
        AmbaPrint("fclose failed (%d)", rval);
    }
    AmbaLink_Free(huge_buf);

    return;
}
/*----------------------------------------------------------------------------*\
 *  @RoutineName::  RfsDataVerify
 *
 *  @Description::  Verify whether the read and write buffer are consistent
 *
 *  @Return     ::
 *      int : OK(0)/NG(others)
 *
\*----------------------------------------------------------------------------*/
static int RfsDataVerify(AMBA_SHELL_ENV_s *env, UINT8* origin, UINT8* data, UINT32 len)
{
    int err_cnt = 0;
    UINT32 i;

    for (i = 0; i < len; i++) {
        if (origin[i] != data[i]) {
#if 0
            AmbaPrint("index=0x%x origin=0x%x, data=0x%x",
                            i, origin[i], data[i]);
#endif
            err_cnt++;
            break;
        }
    }
    if (err_cnt > 0)
        AmbaPrint("len=0x%x, err_cnt=0x%x\n, i = %d", len, err_cnt, i);

    return err_cnt;
}

/*----------------------------------------------------------------------------*\
 *  @RoutineName::  RfsTestRandVrfy
 *
 *  @Description::  Generate a random pattern; write to media; read it back to
 *                  verify.
 *
 *  @Return     ::
 *      None
 *
\*----------------------------------------------------------------------------*/
void RfsTestRandVrfy(AMBA_SHELL_ENV_s *env, char *slot, int size, int mode, int delay, int keep)
{
    int i, n, dir = 0;
    int rval, er = 0;
    int flen, tsize, ndirs;
    UINT8 *huge_buf1, *huge_buf2;
    char path[128];
    AMBA_IPC_RFS_DEVINF_s dev_inf;
    void *fh = NULL;


    /***************************************/
    /* Write random pattern and verify */
    /***************************************/
    sprintf(path, "%s/test", slot);
    AmbaIPC_mkdir(path);


    if (size == 0)
        tsize = BUFFER_SIZE;
    else
        tsize = size;


    if (mode == VRFY_FIXED) {
        n = NUM_VRFY;
    } else {
        rval = AmbaIPC_getdev(slot, &dev_inf);
        if (rval == -1) {
            AmbaPrint("getdev error (%d)", rval);
            return;
        }
        n = (((UINT64)(dev_inf.ecl * dev_inf.spc)) * dev_inf.bps) / tsize;

        /* One directory = One cluster, we need to pre-decrease the
           free number of clusters */
        ndirs = n / 100;
        n = (((UINT64)((dev_inf.ecl - ndirs) * dev_inf.spc)) * dev_inf.bps) / tsize;

        if (mode != VRFY_ALL) {
            if (n > mode)
                n = mode;
        }
    }

    huge_buf1 = AmbaLink_Malloc(tsize + n);
    if (huge_buf1 == NULL)
        return;

    huge_buf2 = AmbaLink_Malloc(tsize + n);
    if (huge_buf2 == NULL)
        return;

    for (i = 0; i < (tsize + n); i++)
        *(huge_buf1 + i) = rand() / 256;

    AmbaPrint("%d files with random pattern to be tested", n);

    for (i = 0; i < n; i++) {

        UINT8 *pWrbuf = huge_buf1, *pRdBuf = huge_buf2;

        pWrbuf += i;
        pRdBuf += i;

        if (i % 100 == 0 || i == 0) {
            dir = i / 100 + 1;
            sprintf(path, "%s/test/%d", slot, dir);
            AmbaIPC_mkdir(path);
        }

        sprintf(path, "%s/test/%d/test%d_pat.bin", slot, dir, i);
        AmbaPrint("write %d bytes random pattern and verify---%s",
                        tsize, path);

        fh = AmbaIPC_fopen(path, "w");
        if (fh == NULL) {
            AmbaPrint("fopen '%s' failed (%d)", path, fh);
            goto done_pat;
        }

        flen = AmbaIPC_fwrite(pWrbuf, tsize, fh);
        if (flen != tsize) {
            AmbaPrint("[FAILED]wrote %d bytes to '%s'", flen, path);
            goto done_pat;
        }

        rval = AmbaIPC_fclose(fh);
        if (rval < 0) {
            AmbaPrint("fclose failed (%d)", rval);
            goto done_pat;
        }

        fh = AmbaIPC_fopen(path, "r");
        if (fh == NULL) {
            AmbaPrint("fopen '%s' failed (%d)", path, rval);
            goto done_pat;
        }

        flen = AmbaIPC_fread(pRdBuf, tsize, fh);
        if (flen != tsize) {
            AmbaPrint("[FAILED]read %d bytes to '%s'", flen, path);
            goto done_pat;
        }

        rval = AmbaIPC_fclose(fh);
        if (rval < 0) {
            AmbaPrint("fclose failed (%d)", rval);
            goto done_pat;
        }

        rval = RfsDataVerify(env, pWrbuf, pRdBuf, tsize);
        if (rval > 0) {
            AmbaPrint("write random pattern and verify fail"
                            "byte counts (%d)", rval);
            er++;
        } else
            AmbaPrint("write random pattern and verify ok");

        /* Remove the file */
        if (keep == 0)
            AmbaIPC_remove(path);

        if (delay > 0)
            AmbaKAL_TaskSleep(delay);
    }

done_pat:
    if (er > 0)
        AmbaPrint("%d files with random pattern tested are failed", er);
    else
        AmbaPrint("test_fioprf_randvrfy is successful");

    AmbaLink_Free(huge_buf1);
    AmbaLink_Free(huge_buf2);

    return;
}
/*----------------------------------------------------------------------------*\
 *  @RoutineName::  RfsListFile
 *
 *  @Description::  list the files in the specified directory
 *
 *  @Return     ::
 *      None
 *
\*----------------------------------------------------------------------------*/
static void RfsListFile(AMBA_SHELL_ENV_s *env, char *slot)
{
    UINT32 systim_s, systim_e;
    UINT32 dtime;
    void* dirp;
    float sec;
    AMBA_IPC_RFS_DIRENT_s *dirent;
    AMBA_IPC_RFS_STAT_s stat;
    char path[100];

    AmbaUtility_GetTimeStart(&systim_s);

    dirp = AmbaIPC_opendir(slot);

    dirent = AmbaIPC_readdir(dirp);
    while (dirent != NULL){
        memset(&stat, 0x0, sizeof(stat));
        memset(path, 0x0, 100);
        strcpy(path, slot);
        strcat(path, "/");
        strcat(path, dirent->name);
        if(AmbaIPC_stat(path, &stat) == 0)
            AmbaPrint("%s \t", dirent->name);
        dirent = AmbaIPC_readdir(dirp);
    }

    AmbaIPC_closedir(dirp);

    dtime = AmbaUtility_GetTimeEnd(&systim_s, &systim_e);
    sec = ((float) dtime) / 1000;
    AmbaPrint("\nread time elapsed : %f sec", sec);
}
/*----------------------------------------------------------------------------*\
 *  @RoutineName::  IpcTestRfs
 *
 *  @Description::  test case for Ramfs
 *
 *  @Return     ::
 *      int : OK(0)/NG(-1)
 *
\*----------------------------------------------------------------------------*/
static int IpcTestRfs(AMBA_SHELL_ENV_s *env, int argc, char **argv)
{
    if (argc == 1)
    {
        AmbaPrint("supported test commands");
        AmbaPrint("    rfs thruput [path]");
        AmbaPrint("    rfs randvrfy [path]");
        AmbaPrint("    rfs list [dir]");
        return 1;
    }

    if(!strcmp(argv[1], "thruput")) {
		UINT32 bs_multi[MAX_ARGC];

        if(argc != 3) {
            AmbaPrint("invalid input for thruput");
            return 0;
        }
        memset(bs_multi, 0, sizeof(bs_multi));
        RfsTestThruput(env, argv[2], bs_multi, 0);
        AmbaPrint("test_fioprf_thruput completed!");
    }
    else if (!strcmp(argv[1], "randvrfy")) {
        if(argc != 3) {
            AmbaPrint("invalid input for randvrfy");
            return 0;
        }
        RfsTestRandVrfy(env, argv[2], 0, 0, 0, 0);
        AmbaPrint("test_fioprf_randvrfy completed!");
    }
    else if (!strcmp(argv[1], "list")) {
        if(argc != 3) {
            AmbaPrint("invalid input for list");
            return 0;
        }
        RfsListFile(env, argv[2]);
    }
    else {
        AmbaPrint("command format error!");
    }

    return 0;
}

/*----------------------------------------------------------------------------*\
 *  @RoutineName::  IpcTestUsb
 *
 *  @Description::  test case for USB owner
 *
 *  @Return     ::
 *      int : OK(0)/NG(-1)
 *
\*----------------------------------------------------------------------------*/
static int IpcTestUsb(AMBA_SHELL_ENV_s *env, int argc, char **argv)
{
    extern AMBA_LINK_CTRL_s AmbaLinkCtrl;
    extern void AmbaIPC_RpcUsbOwnerSwitch(int Owner);

    if (argc == 2 && atoi(argv[1]) == 1) {
        AmbaLinkCtrl.AmbaLinkUsbOwner = 1;
        AmbaIPC_RpcUsbOwnerSwitch(AmbaLinkCtrl.AmbaLinkUsbOwner);
    } else {
        AmbaPrint("supported test commands");
        AmbaPrint("    usb_owner [0|1]");
        AmbaPrint("        0: owner is RTOS");
        AmbaPrint("        1: owner is Linux");
        return 1;
    }

    return 0;
}

/*============================================================================*\
 *                                                                            *
 *                           Test module Management                           *
 *                                                                            *
\*============================================================================*/
struct test_list_t {
    char*     name;
    AMBA_TEST_COMMAND_HANDLER_f func;
};

static struct test_list_t test_lists[] = {
    {"slock", IpcTestSlock},
    {"mutex", IpcTestMutex},
    {"rpmsg", IpcTestRpmsg},
    {"rpc",   IpcTestRpc},
    {"rfs",   IpcTestRfs},
    {"usb_owner",   IpcTestUsb},
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
        AmbaPrint("Supported test commands:");
        for (i = 0; i < TEST_LIST_SIZE; i++) {
            AmbaPrint("    %s", test_lists[i].name);
        }
    }

    return 0;
}

//extern int AmbaLink_RPCInit(void);
/*----------------------------------------------------------------------------*\
 *  @RoutineName::  AmpUT_IPCTestInit
 *
 *  @Description::  Init test module
 *
 *  @Return     ::
 *      int : OK(0)/NG(-1)
 *
\*----------------------------------------------------------------------------*/
int AmpUT_IPCTestInit(AMBA_KAL_BYTE_POOL_t *pCachedHeap)
{
    if (pCachedHeap==NULL) {
        AmbaPrint("Error!! %s: pCachedHeap is %p",__FUNCTION__,pCachedHeap);
        return -1;
    }
    g_HeapPool = pCachedHeap;

    return 0;
}

/*----------------------------------------------------------------------------*\
 *  @RoutineName::  AmpUT_IPCTestAdd
 *
 *  @Description::  register IPC test module
 *
 *  @Return     ::
 *      int : OK(0)/NG(-1)
 *
\*----------------------------------------------------------------------------*/
int AmpUT_IPCTestAdd(void)
{
    //AmbaPrint("Adding Test command ipc: %s %d",__FUNCTION__, __LINE__);

    if (g_HeapPool==NULL) {
        AmbaPrint("Error!! %s: g_HeapPool is %p",__FUNCTION__,g_HeapPool);
        return AMP_ERROR_GENERAL_ERROR;
    }

    Channel = AmbaIPC_Alloc("echo_cortex", MsgHandler);
	if (Channel == NULL) {
		AmbaPrint("%s: AmbaIPC_Alloc failed!", __func__);
		return -1;
	}
    AmbaIPC_RegisterChannel(Channel, NULL);

    AmbaTest_RegisterCommand("ipc", IpcTestEntry);

    return AMP_OK;
}

int AppLibNetIPCTest_Add(void *pMemoryPool)
{
	int ReturnValue = 0;

	AmbaPrint("register AmbaLink_BootDoneCallBack");
	ReturnValue = AmpUT_IPCTestInit(pMemoryPool);
	if (ReturnValue != 0) {
		AmbaPrintColor(RED,"Cannot register AmbaLink_BootDoneCallBack");
		return -1;
	}

	AmbaLink_UserIpcInitCallBack = AmpUT_IPCTestAdd;

	return 0;
}

