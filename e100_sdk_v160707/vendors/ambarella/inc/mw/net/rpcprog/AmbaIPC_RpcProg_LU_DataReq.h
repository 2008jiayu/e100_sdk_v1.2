/**
 * @file inc/mw/net/rpcprog/AmbaIPC_RpcProg_Lu_DataReq.h
 *
 * Header file for NetCtrl RPC Services
 *
 *
 * Copyright (C) 2014, Ambarella, Inc.
 *
 * All rights reserved. No Part of this file may be reproduced, stored
 * in a retrieval system, or transmitted, in any form, or by any means,
 * electronic, mechanical, photocopying, recording, or otherwise,
 * without the prior consent of Ambarella, Inc.
 */

#ifndef _RPC_PROG_LU_DATA_REQ_H_
#define _RPC_PROG_LU_DATA_REQ_H_

#include "AmbaIPC_Rpc_Def.h"

#define LU_DATA_REQ_PROG_ID                 0x20000007
#define LU_DATA_REQ_VER                          1

/**
 * struct of data request info
 */
typedef struct _LU_DATASVC_DATA_s_ {
    unsigned int MsgId;                         /**< send data type*/
    char Filepath[512];                         /**< filepath */
    char Md5sum[32];                          /**< md5sum */
    unsigned long long Offset;               /**< file offset */
    unsigned long long FetchFilesize;    /**< fetch file size */
    unsigned int BlockSize;                 /**< the size of block (Kbyte)*/
    unsigned int MaximalSpeed;   /**< the maximal speed of transmitting file (Kbyte/sec)*/
    char *BufferAddr;                          /**< data buffer */
    unsigned char ClientInfo[128];        /**< client identifier */
    char TransportType[16];                /**< transport protocol type */
} LU_DATASVC_DATA_s;

/**
 * struct of data request result
 */
typedef struct _LU_DATASVC_RESULT_s_ {
    int Rval;                                           /**< result value */
    unsigned long long RemSize;            /**< remain file size */
    unsigned long long TotalFilesize;      /**< total file size */
} LU_DATASVC_RESULT_s;

/* Procedure ID */
#define LU_DATA_REQ_PROC                        1

/**
 * [in] LU_DATASVC_DATA_s
 * [out] LU_DATASVC_RESULT_s
 */
int LU_DataReq_Svc(LU_DATASVC_DATA_s *pArg, AMBA_IPC_SVC_RESULT_s *pResult);
AMBA_IPC_REPLY_STATUS_e LU_DataReq_Clnt(LU_DATASVC_DATA_s *pArg, LU_DATASVC_RESULT_s *pResult, int Clnt);

/* Procedure ID */
#define LU_DATA_GET_STATUS_PROC	        2

/**
 * struct of data transmission status
 */
typedef struct _LU_DATA_GETSTATUS_s_ {
    unsigned char ClientInfo[128];        /**< client identifier */
    char TransportType[16];                /**< transport protocol type */
} LU_DATA_GETSTATUS_s;

/**
 * [in] LU_DATA_GETSTATUS_s
 * [out] int
 */
int LU_Data_GetStatus_Svc(LU_DATA_GETSTATUS_s *pArg, AMBA_IPC_SVC_RESULT_s *pResult);
AMBA_IPC_REPLY_STATUS_e LU_Data_GetStatus_Clnt(LU_DATA_GETSTATUS_s *pArg, int *pResult, int Clnt);

/* Procedure ID */
#define LU_DATA_CANCEL_TRANS_PROC	        3

/**
 * cancel data transmission info
 */
typedef struct _LU_DATA_CANCEL_TRANS_s_ {
    int MsgId;                                   /**< data request type */
    unsigned char ClientInfo[128];        /**< client identifier */
    char TransportType[16];                /**< transport protocol type */
    unsigned long long SentSize;        /**< size of file (AMBA_PUT_FILE) data portion */
} LU_DATA_CANCEL_TRANS_s;

/**
 * cancel data transmission result
 */
typedef struct _LU_DATA_CANCEL_RESULT_s_ {
    int Rval;                                        /**< result value */
    unsigned long long TransSize;        /**< transferred size */
} LU_DATA_CANCEL_RESULT_s;

/**
 * [in] LU_DATA_CANCEL_TRANS_s
 * [out] LU_DATA_CANCEL_RESULT_s
 */
int LU_Data_CancelTrans_Svc(LU_DATA_CANCEL_TRANS_s *pArg, AMBA_IPC_SVC_RESULT_s *pResult);
AMBA_IPC_REPLY_STATUS_e LU_Data_CancelTrans_Clnt(LU_DATA_CANCEL_TRANS_s *pArg, LU_DATA_CANCEL_RESULT_s *pResult, int Clnt);

/* Procedure ID */
#define LU_DATA_CLOSE_CONNECTION_PROC	        4

/**
 * struct of close data connection
 */
typedef struct _LU_DATA_CLOSE_CONNECT_s_ {
    unsigned char ClientInfo[128];        /**< client identifier */
    char TransportType[16];                /**< transport protocol type */
} LU_DATA_CLOSE_CONNECT_s;

/**
 * [in] LU_DATA_CLOSE_CONNECT_s
 * [out] int
 */
int LU_Data_CloseConnection_Svc(LU_DATA_CLOSE_CONNECT_s *pArg, AMBA_IPC_SVC_RESULT_s *pResult);
AMBA_IPC_REPLY_STATUS_e LU_Data_CloseConnection_Clnt(LU_DATA_CLOSE_CONNECT_s *pArg, int *pResult, int Clnt);

#endif /* RPC_PROG_LU_DATA_REQ_H */

