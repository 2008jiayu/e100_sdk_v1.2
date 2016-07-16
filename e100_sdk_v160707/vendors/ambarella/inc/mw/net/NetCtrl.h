/**
 * @file NetCtrl.h
 *
 * Copyright (C) 2015, Ambarella, Inc.
 *
 * All rights reserved. No Part of this file may be reproduced, stored
 * in a retrieval system, or transmitted, in any form, or by any means,
 * electronic, mechanical, photocopying, recording, or otherwise,
 * without the prior consent of Ambarella, Inc.
 */

#ifndef AMP_NETCTRL_H_
#define AMP_NETCTRL_H_

/**
 * @defgroup NETCTRL
 * @brief RTOS MW RPC Remote Service module
 *
 * NetCtrl is used to transfer message between RTOS and Linux user space programs.\n
 * With NetCtrl, Linux program could send/receive message from/to RTOS.\n
 *
 */

#include "AmbaDataType.h"
#include "AmbaKAL.h"

#define DATASVC_GET_FILE            0x01
#define DATASVC_GET_ENCAP_FILE      0x02
#define DATASVC_GET_THUMB           0x03
#define DATASVC_GET_ENCAP_THUMB     0x04
#define DATASVC_PUT_FILE            0x05

#define DATA_SERVER_IDLE            0x00
#define DATA_SERVER_SEND_BUSY       (1<<0)
#define DATA_SERVER_RECV_BUSY       (1<<1)
#define DATA_SERVER_SEND_CANCEL     (1<<2)
#define DATA_SERVER_RECV_CANCEL     (1<<3)
#define DATA_SERVER_CLOSE_CONNECT   (1<<4)

#define INST_TYPE_BLE 0
#define INST_TYPE_LNX_WIFI_BT 1

/**
 * NetCtrl error code
 */
typedef enum _AMP_NETCTRL_ERROR_e_ {
    AMP_NETCTRL_UNKNOWN_ERROR = -1,              /**< Unknown Error */
    AMP_NETCTRL_INVALID_PATH = -2,                    /**< Invalid File Path */
    AMP_NETCTRL_INVALID_PARAM = -3,                 /**< Invalid Parameter */
    AMP_NETCTRL_INVALID_OPERATION = -4,         /**< Invalid Operation */
    AMP_NETCTRL_NO_MORE_MEMORY = -5,             /**< Run out of Memory */
    AMP_NETCTRL_INSTANCE_EXISTED_ALREADY = -6,   /**< The instance is existed already*/
    AMP_NETCTRL_INSTANCE_NOT_EXISTED = -7,       /**< The instance is not existed */
    AMP_NETCTRL_CALLBACK_NULL = -8,               /**< The callback is NULL */
    AMP_NETCTRL_HEAP_NOT_ENOUGH = -9               /**< The given heap is not enough */
} AMP_NETCTRL_ERROR_e;

/**
 * NetCtrl default init config
 */
typedef struct _AMP_NETCTRL_INIT_CFG_s_ {
    UINT8* MemPoolAddr;      /**< buffer address*/
    UINT32 MemPoolSize;       /**< size of buffer */
} AMP_NETCTRL_INIT_CFG_s;

/**
 * NetCtrl command and client info
 */
typedef struct _AMP_NETCTRL_CMD_s_ {
   char Param[1024];          /**< json string */
   UINT32 ParamSize;        /**< size of json string */
   UINT32 ClientId;             /**< socket descriptor of accepted client */
} AMP_NETCTRL_CMD_s;

/**
 * NetCtrl command and client info
 */
typedef struct _AMP_NETCTRL_DATA_s_ {
   char Param[1024];          /**< json string */
   UINT32 ParamSize;        /**< size of json string */
   UINT32 ClientId;             /**< socket descriptor of accepted client */
} AMP_NETCTRL_DATA_s;

/**
 * NetCtrl result of linux commad
 */
typedef struct _AMP_NETCTRL_LNXCMD_RESULT_s_ {
    int Rval;                                /**< return value of execute linux command */
    unsigned int ResultSize;        /**< size of the result */
} AMP_NETCTRL_LNXCMD_RESULT_s;

/**
 * NetCtrl data request
 */
typedef struct _AMP_NETCTRL_DATASVC_DATA_s_ {
    unsigned int MsgId;                            /**< data request type */
    char Filepath[512];                            /**< full path of file */
    char Md5sum[32];                             /**< md5 checksum of data */
    unsigned long long Offset;                  /**< number of bytes to offset from beginning of file */
    unsigned long long FetchFilesize;       /**< file size to fetch */
    unsigned int BlockSize;                 /**< the size of block (Kbyte)*/
    unsigned int MaximalSpeed;   /**< the maximal speed of transmitting file (Kbyte/sec)*/
    char *BufferAddr;                             /**< buffer address of data */
    unsigned char ClientInfo[128];           /**< client identifier */
    char TransportType[16];                   /**< transport protocol type */
} AMP_NETCTRL_DATASVC_DATA_s;

/**
 * NetCtrl result of data request
 */
typedef struct _AMP_NETCTRL_DATASVC_RESULT_s_ {
    int Rval;                                            /**< result of data request */
    unsigned long long RemSize;             /**< full size of file minus offset */
    unsigned long long TotalFilesize;       /**< size of the file */
} AMP_NETCTRL_DATASVC_RESULT_s;

/**
 * NetCtrl notification of data transmission
 */
typedef struct _AMP_NETCTRL_DATASVC_STATUS_s_ {
   INT16 Status;                              /**< status of send or receive data */
   UINT64 Bytes;                             /**< total bytes of send or receive */
   UINT32 Type;                              /**< notification of data request type */
   unsigned char ClientInfo[128];     /**< client identifier */
   char TransportType[16];             /**< transport protocol type */
   unsigned char Md5sum[32];        /**< md5 checksum of data */
} AMP_NETCTRL_DATASVC_STATUS_s;

/**
 * NetCtrl cancel data transmission info
 */
typedef struct _AMP_NETCTRL_DATASVC_CANCEL_TRANS_s_ {
    int MsgId;                                   /**< data request type */
    unsigned char ClientInfo[128];        /**< client identifier */
    char TransportType[16];                /**< transport protocol type */
    unsigned long long SentSize;        /**< size of file (AMBA_PUT_FILE) data portion */
} AMP_NETCTRL_DATASVC_CANCEL_TRANS_s;

/**
 * NetCtrl result of cancel data transmission
 */
typedef struct _AMP_NETCTRL_DATASVC_CANCEL_RESULT_s_ {
    int Rval;                               /**< result of cancel data transmission */
    unsigned long long TransSize;        /**< transferred bytes of send or receive */
} AMP_NETCTRL_DATASVC_CANCEL_RESULT_s;


 /**
  * The configure information of client instance
  */
 typedef struct _AMP_NETCTRL_CFG_s_ {
     UINT32 InstTypeId; /**< The type of transmitting commands*/
 } AMP_NETCTRL_CFG_s;



 /**
  * The information of handler
  * All ClinetId with the same InstTypeId of command server
  * are belong to the same instance.
  */
  typedef struct _AMP_NETCTRL_HDLR_INFO_s_ {
      UINT32 InstTypeId; /**< The type of transmitting commands*/
      UINT8* Hdlr; /**< The address of instance*/
  } AMP_NETCTRL_HDLR_INFO_s;
/**
 * The information for sending the result out by using the command client
 */
  typedef struct _AMP_NETCTRL_SEND_INFO_s {
      UINT32 ClientId; /**< The client id for wifi or BT2.0*/
      UINT32 Size; /**< The size of data which sould be sent data */
      char* MemAddr; /*< The pointer indicates the address of allocated memory whose data should be sent out*/
  } AMP_NETCTRL_SEND_INFO_s;

 /**
  * The information of handler for data server
  * All ClientInfo and TransportType with the same InstTypeId of data server
  * are belong to the same instance.
  */
  typedef struct _AMP_NETCTRL_DATASVC_HDLR_INFO_s_ {
      UINT32 InstTypeId; /**< The type of transmitting data*/
      UINT8* Hdlr; /**< The address of instance*/
  } AMP_NETCTRL_DATASVC_HDLR_INFO_s;

/**
  * The destination information to indicate the specified source of client for data server
  */
  typedef struct _AMP_NETCTRL_DATASVC_DEST_INFO_s_ {
      unsigned char ClientInfo[128]; /**< client identifier */
      char TransportType[16]; /**< transport protocol type */
  } AMP_NETCTRL_DATASVC_DEST_INFO_s;


/**
 * The callback function for receiving commands
 */

typedef int (*AmpNetCtrl_CmdRcv_Cb)(AMP_NETCTRL_HDLR_INFO_s *hdlrInfo, AMP_NETCTRL_CMD_s *dataFromLnx);


/**
 * callback function prototype of data server
 */

typedef int (*AmpNetCtrl_DataSvc_Status_Cb)(AMP_NETCTRL_DATASVC_HDLR_INFO_s *hdlrInfo, AMP_NETCTRL_DATASVC_STATUS_s *statusFromDataSvc);



/**
 * Get netctrl package version info
 *
 * @return Version Number of NetCtrl Package
 */
extern int AmpNetCtrl_GetVer(void);

/**
 * Get default configuration for NetCtrl initial.
 *
 * @param [out] defaultCfg the init config
 *
 * @return 0 - OK, others - AMP_NETCTRL_ERROR_e
 * @see AMP_NETCTRL_ERROR_e
 * @see AMP_NETCTRL_INIT_CFG_s
 */
extern int AmpNetCtrl_GetInitDefaultCfg(AMP_NETCTRL_INIT_CFG_s *defaultCfg);

/**
 * NetCtrl initial function,
 *
 * @param [in] InitCfg the init config
 *
 * @return 0 - OK, others - AMP_NETCTRL_ERROR_e
 * @see AMP_NETCTRL_ERROR_e
 * @see AMP_NETCTRL_INIT_CFG_s
 */
extern int AmpNetCtrl_Init(AMP_NETCTRL_INIT_CFG_s *initCfg);

/**
 * NetCtrl release function,
 *
 * @return 0 - OK, others - AMP_NETCTRL_ERROR_e
 */
extern int AmpNetCtrl_Release(void);

/**
 * Initialize the server for Linux commands such as LS, CD, PWD, and so on.
 * Please call this function first if you want to call those functions which
 * will use the shell command in the linux such as AmpNetCtrl_LnxLs and so on.
 *
 * @param [in] NULL
 * @return 0 - OK, others - AMP_NETCTRL_ERROR_e
 *
 */
extern int AmpNetCtrl_LnxCmdInit(void);

/**
 * Release the server and resources of Linux commands such as LS, CD, PWD, and so on.
 * Please call this function if you don't want to call those functions which
 * will use the shell command in the linux anymore.
 *
 * @param [in] NULL
 * @return 0 - OK, others - AMP_NETCTRL_ERROR_e
 *
 */
extern int AmpNetCtrl_LnxCmdRelease(void);


/**
 * Send LS command for NetCtrl to list the contents of the directory,
 *
 * @param [in] param extra info for the LS command. Pass empty string if there is no need.
 * @param [out] retResult an array of content for the directory. An empty array ([ ]) is returned if the directory is empty.\n
 *                       (1). write the key and value of rval and msg_id, and the key of listing and colon to retResult buffer.\n
 *                       (2). the starting address of buffer to retResult will be shifted by the length of this content ({"rval":0,"msg_id":1282,"listing":)\n
 * @param [in] size the max size of buffer to retResult minus the length of this content ({"rval":0,"msg_id":1282,"listing":).
 * @param [out] lnxResult the result info of execute linux command
 * @see AMP_NETCTRL_LNXCMD_RESULT_s
 *
 * @return 0 - OK, others - AMP_NETCTRL_ERROR_e
 */
extern int AmpNetCtrl_LnxLs(char *param, char *retResult, UINT32 size, AMP_NETCTRL_LNXCMD_RESULT_s *lnxResult);

/**
 * Send CD command for NetCtrl to change the current working directory,
 *
 * @param [in] param destination directory paths for the CD command.
 * @param [out] retResult print working directory
 * @param [in] size the max size of buffer to retResult
 * @param [out] lnxResult the result info of execute linux command
 * @see AMP_NETCTRL_LNXCMD_RESULT_s
 *
 * @return 0 - OK, others - AMP_NETCTRL_ERROR_e
 */
extern int AmpNetCtrl_LnxCd(char *param, char *retResult, UINT32 size, AMP_NETCTRL_LNXCMD_RESULT_s *lnxResult);

/**
 * Send PWD command for NetCtrl to obtain the current directory,
 *
 * @param [out] retResult print working directory
 * @param [in] size the max size of buffer to retResult
 * @param [out] lnxResult the result info of execute linux command
 * @see AMP_NETCTRL_LNXCMD_RESULT_s
 *
 * @return 0 - OK, others - AMP_NETCTRL_ERROR_e
 */
extern int AmpNetCtrl_LnxPwd(char *retResult, UINT32 size, AMP_NETCTRL_LNXCMD_RESULT_s *lnxResult);

/**
 * Get the current status of WIFI for NetCtrl,
 *
 * @param [in] option the option to get WIFI status. Pass NULL if there is no need.
 * @param [out] retResult the current status of WIFI
 * @param [in] size the max size of buffer to retResult
 *
 * @return 0 - OK, others - AMP_NETCTRL_ERROR_e
 */
extern int AmpNetCtrl_GetWifiStatus(char *option, char *retResult, UINT32 size, AMP_NETCTRL_LNXCMD_RESULT_s *lnxResult);

/**
 * Get the content of the WIFI configuration for NetCtrl,
 *
 * @param [in] option the option to get WIFI configuration. Pass NULL if there is no need.
 * @param [out] retResult the complete content of the WIFI configuration. The "\n" is the identified symbol between two settings.
 * @param [in] size the max size of buffer to retResult
 * @param [out] lnxResult the result info of execute linux command
 * @see AMP_NETCTRL_LNXCMD_RESULT_s
 *
 * @return 0 - OK, others - AMP_NETCTRL_ERROR_e
 */
extern int AmpNetCtrl_GetWifiConfig(char *option, char *retResult, UINT32 size, AMP_NETCTRL_LNXCMD_RESULT_s *lnxResult);

/**
 * Set the configuration of WIFI for NetCtrl,
 *
 * @param [in] config the content of the WIFI configuration. The "\n" is the identified symbol if the user wants to modify more than one settings.
 * @param [in] option the option to set WIFI configuration. Pass NULL if there is no need.
 * @param [out] lnxResult the result info of execute linux command
 * @see AMP_NETCTRL_LNXCMD_RESULT_s
 *
 * @return 0 - OK, others - AMP_NETCTRL_ERROR_e
 */
extern int AmpNetCtrl_SetWifiConfig(char *config, char *option, AMP_NETCTRL_LNXCMD_RESULT_s *lnxResult);

/**
 * Start the WIFI for NetCtrl,
 *
 * @param [in] option the option to start WIFI. Pass NULL if there is no need.
 *
 * @return 0 - OK, others - AMP_NETCTRL_ERROR_e
 */
extern int AmpNetCtrl_WifiStart(char *option);

/**
 * Stop the WIFI for NetCtrl,
 *
 * @param [in] option the option to stop WIFI. Pass NULL if there is no need.
 *
 * @return 0 - OK, others - AMP_NETCTRL_ERROR_e
 */
extern int AmpNetCtrl_WifiStop(char *option);

/**
 * Restart the WIFI for NetCtrl,
 *
 * @param [in] option the option to restart WIFI. Pass NULL if there is no need.
 *
 * @return 0 - OK, others - AMP_NETCTRL_ERROR_e
 */
extern int AmpNetCtrl_WifiRestart(char *option);

/**
  * @brief Get the default config of the instance for command server
  *
  * @param [in] NULL
  * @param [out] the default config
  * @return 0 - OK, negative value - AMP_NETCTRL_ERROR_e (ex. The instance has been existed)
  */
extern int AmpNetCtrl_GetInstanceDefaultCfg(AMP_NETCTRL_CFG_s *defaultCfg);


/**
  * @brief Create a instance with specified information.
  * This function is used to create an instance to do the
  * general operation such as sending commands, receiving commands and so on.
  *
  * @param [in] the initial info
  * @param [out] the information of instance
  * @return 0 - OK, negative value - AMP_NETCTRL_ERROR_e (ex. The instance has been existed)
  */
extern int AmpNetCtrl_CreateInstance(AMP_NETCTRL_CFG_s *insInfo, AMP_NETCTRL_HDLR_INFO_s *hdlrInfo);

 /**
  * @brief Get the instance of specified type.
  * This function is used to get the specified instance
  * if it has been created before.
  * Notice that a structure just have one instance at the same time.
  *
  * @param [in] the initial info
  * @param [out] the information of instance
  * @return 0 - The instance is existed and has been returned successfully
  * @return negative value - AMP_NETCTRL_ERROR_e (ex. The instance is not existed)
  */
extern int AmpNetCtrl_GetInstance(AMP_NETCTRL_CFG_s *insInfo, AMP_NETCTRL_HDLR_INFO_s *hdlrInfo);


/**
 * @brief Delete the instance of specified type.
 * This function is used to delete the specified instance
 * if it has been created before.
 *
 * @param [in] the handler info
 * @return 0 - The instance is existed and has been deleted successfully
 * @return negative value - AMP_NETCTRL_ERROR_e (ex. The instance is not existed)
 */
extern int AmpNetCtrl_DeleteInstance(AMP_NETCTRL_HDLR_INFO_s *hdlrInfo);

/**
 * @brief Check the status of the receiving function with specified handler.
 * This function is used to check the receiving function which is wether registered
 * or not.
 *
 * @param [in] the handler info
 * @return 0 - The receiving function has been registed
 * @return negative value - AMP_NETCTRL_ERROR_e (ex. The receiving function is NULL)
 */
extern int AmpNetCtrl_CheckRecvCbStatus(AMP_NETCTRL_HDLR_INFO_s *hdlrInfo);

/**
 * @brief Register the callback function for receiving commands.
 * This function is used to register the callback function for receiving
 * commands with specified handler.
 *
 * @param [in] the handler info
 * @param [in] the function should be registered
 * @return 0 - OK, negative value - AMP_NETCTRL_ERROR_e
 */

extern int AmpNetCtrl_RegCmdRcvCB(AMP_NETCTRL_HDLR_INFO_s *hdlrInfo, AmpNetCtrl_CmdRcv_Cb cbCmdRcv);

/**
 * @brief A general function for sending the returned result/notification.
 * This function is used to send the data from the indicated memory with specified handler
 * and it should be created first.
 *
 * @param [in] the handler info
 * @param [in] the sending info
 *
 * @return 0 - OK
 * @return negative value - AMP_NETCTRL_ERROR_e
 */
extern int AmpNetCtrl_Send(AMP_NETCTRL_HDLR_INFO_s *hdlrInfo, AMP_NETCTRL_SEND_INFO_s *sendinfo);

/**
  * @brief Get the default config of the instance for data server
  *
  * @param [in] NULL
  * @param [out] the default config
  * @return 0 - OK, negative value - AMP_NETCTRL_ERROR_e (ex. The instance has been existed)
  */
extern int AmpNetCtrl_DataSvc_GetInstanceDefaultCfg(AMP_NETCTRL_CFG_s *defaultCfg);


 /**
  * @brief Create a instance with specified information for sendind data.
  * This function is used to create an instance to do the
  * general operation such as sending data.
  *
  * @param [in] the initial info
  * @param [out] the information of instance
  * @return 0 - OK, negative value - AMP_NETCTRL_ERROR_e (ex. The instance has been existed)
  */
extern int AmpNetCtrl_DataSvc_CreateInstance(AMP_NETCTRL_CFG_s *insInfo, AMP_NETCTRL_DATASVC_HDLR_INFO_s *hdlrInfo);

/**
 * @brief Get the instance of specified type for sending data.
 * This function is used to get the specified instance
 * if it has been created before.
 * Notice that all structures just have one instance at the same time.
 *
 * @param [in] the initial info
 * @param [out] the information of instance
 * @return 0 - The instance is existed and has been returned successfully
 * @return negative value - AMP_NETCTRL_ERROR_e (ex. The instance is not existed)
 */
extern int AmpNetCtrl_DataSvc_GetInstance(AMP_NETCTRL_CFG_s *insInfo, AMP_NETCTRL_DATASVC_HDLR_INFO_s *hdlrInfo);



/**
 * @brief Delete the instance of specified type for sending data.
 * This function is used to delete the specified instance
 * if it has been created before.
 *
 * @param [in] the handler info
 * @return 0 - The instance is existed and has been deleted successfully
 * @return negative value - AMP_NETCTRL_ERROR_e (ex. The instance is not existed)
 */
extern int AmpNetCtrl_DataSvc_DeleteInstance(AMP_NETCTRL_DATASVC_HDLR_INFO_s *hdlrInfo);


/**
 * @brief Check the status of the receiving function of data server with specified handler.
 * This function is used to check the receiving function which is whether registered
 * or not.
 *
 * @param [in] the handler info
 * @return 0 - The receiving function has been registed
 * @return negative value - AMP_NETCTRL_ERROR_e (ex. The receiving function is NULL)
 */
extern int AmpNetCtrl_DataSvc_CheckRecvCbStatus(AMP_NETCTRL_DATASVC_HDLR_INFO_s *hdlrInfo);


/**
 * @brief Register the callback function for receiving the notification from data server.
 * This function is used to register the callback function for receiving
 * the notification with specified handler.
 *
 * @param [in] the handler info
 * @param [in] the function should be registered
 * @return 0 - OK, negative value - AMP_NETCTRL_ERROR_e
 */

extern int AmpNetCtrl_DataSvc_RegRecvCb(AMP_NETCTRL_DATASVC_HDLR_INFO_s *hdlrInfo, AmpNetCtrl_DataSvc_Status_Cb cbStatusNotify);

/**
 * Send the requirements to transmit the data
 *
 * @param [in] the information of instance
 * @param [in] dataReq the info of data transmission
 * @see AMP_NETCTRL_DATASVC_DATA_s
 * @param [out] dataResult the result of data request
 * @see AMP_NETCTRL_DATASVC_RESULT_s
 *
 * @return 0 - OK, others - AMP_NETCTRL_ERROR_e
 */

extern int AmpNetCtrl_DataSvc_Send(AMP_NETCTRL_DATASVC_HDLR_INFO_s *hdlrInfo, AMP_NETCTRL_DATASVC_DATA_s *dataReq,
                                   AMP_NETCTRL_DATASVC_RESULT_s *dataResult);
/**
 * Get the current status of data server
 *
 * @param [in] the information of instance
 * @param [in] the detailed information of data server
 * @param [out] retResult the status of daemon
 *
 * @return 0 - OK, others - AMP_NETCTRL_ERROR_e
 */

extern int AmpNetCtrl_DataSvc_GetStatus(AMP_NETCTRL_DATASVC_HDLR_INFO_s *hdlrInfo,
                                        AMP_NETCTRL_DATASVC_DEST_INFO_s *destInfo,
                                        int *retResult);

/**
 * Cancel the data transmission
 *
 * @param [in] the information of instance
 * @param [in] cancelTrans the info of cancel data transmission
 * @see AMP_NETCTRL_DATASVC_CANCEL_TRANS_s
 * @param [out] cancelResult the status of cancel data transmission
 * @see AMP_NETCTRL_DATASVC_CANCEL_RESULT_s
 *
 * @return 0 - OK, others - AMP_NETCTRL_ERROR_e
 */

extern int AmpNetCtrl_DataSvc_CancelDataTrans(AMP_NETCTRL_DATASVC_HDLR_INFO_s *hdlrInfo,
                                              AMP_NETCTRL_DATASVC_CANCEL_TRANS_s *cancelTrans,
                                              AMP_NETCTRL_DATASVC_CANCEL_RESULT_s *cancelResult);

/**
 * Close the connection
 *
 * @param [in] the information of instance
 * @param [in] the detailed information of data server
 * @param [out] retResult the status of close connection
 *
 * @return 0 - OK, others - AMP_NETCTRL_ERROR_e
 */

extern int AmpNetCtrl_DataSvc_CloseConnection(AMP_NETCTRL_DATASVC_HDLR_INFO_s *hdlrInfo,
                                              AMP_NETCTRL_DATASVC_DEST_INFO_s *destInfo,
                                              int *retResult);

#endif /* AMP_NETCTRL_H_ */

