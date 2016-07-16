/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaLink.h
 *
 *  @Copyright      :: Copyright (C) 2013 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Definitions & Constants for Ambarella CPU Link and Network Supports
 *
\*-------------------------------------------------------------------------------------------------------------------*/

#ifndef _AMBA_LINK_H_
#define _AMBA_LINK_H_

/**
 * @defgroup AmbaLink
 * @brief AmbaLink module implementation
 *
 * Implementation the AmbaLink module.
 * @{
 */

#define AMBALINK_CORTEX_TO_ARM_ADDR(x)  (0xC0000000 + x)

/**
 * @brief This function is used to initialize AmbaLink.
 *
 */
void AmbaLink_Init(void);

/**
 * @brief This function is used to load Linux.
 *
 * @return 0 - OK, others - NG
 */
int AmbaLink_Load(void);

/**
 * @brief This function is used to boot Linux.
 *
 * @param [in] TimeOutMs The value of time out in millisecond.
 *
 * @return 0 - OK, others - NG
 */
int AmbaLink_Boot(UINT32 TimeOut);

/**
 * @brief Initialize AmbaLink related service (AmbaIPC_MutexInit, AmbaIPC_SpinInit).
 *
 */
void AmbaLink_InitService(void);

/**
 * @brief This function is used to malloc memory in AmbaLink.
 *
 * @param [in] Size Memory request size
 *
 * @return Pointer to allocated memory buffer
 * @see AmbaLink_Free
 */
void *AmbaLink_Malloc(unsigned int Size);

/**
 * @brief This function is used to free memory in AmbaLink.
 *
 * @param [in] pPtr Point to the memory buffer to be freed
 * @see AmbaLink_Malloc
 */
void AmbaLink_Free(void *pPtr);


/**
 * @brief Notify the caller Linux is warmboot, coldboot or hibernation boot.
 *  The API is blocked until BootType can be guarateened.
 *
 * @param [in] TimeOutMs The value of time out in millisecond.
 *
 * @return 0 - OK, others - NG
 */
int AmbaLink_BootType(UINT32 TimeOutMs);


/**
 * @brief Suspend Linux to disk or dram.
 *
 * @param [in] SuspendMode The mode to suspend. See AMBA_LINK_SUSPEND_MODE_e.
 *
 * @return 0 - OK, -1 - NG
 */
int AmbaIPC_LinkCtrlSuspendLinux(UINT32 SuspendMode);

/**
 * @brief This function is blocked until suspend flow is complete.
 *
 * @param [in] TimeOutMs The value of time out in millisecond.
 *
 * @return 0 - OK, others - NG
 */
int AmbaIPC_LinkCtrlWaitSuspendLinux(UINT32 TimeOutMs);


/**
 * @brief Resume Linux from suspend state
 *
 * @param [in] SuspendMode To resume from SuspendMode. See AMBA_LINK_SUSPEND_MODE_e.
 *
 * @return 0 - OK, -1 - NG
 */
int AmbaIPC_LinkCtrlResumeLinux(UINT32 SuspendMode);

/**
 * @brief This function is blocked until resume flow is complete.
 *
 * @param [in] TimeOutMs The value of time out in millisecond.
 *
 * @return 0 - OK, others - NG
 */
int AmbaIPC_LinkCtrlWaitResumeLinux(UINT32 TimeOutMs);


/**
 * @brief This function is called after the initialization of hibernation service of ThreadX.
 *        For example, AmbaHiber test command can be initialized in this function.
 *
 * @return 0 - OK, others - NG
 */
extern INT_FUNCTION AmbaHiber_InitCallBack;


/**
 * @brief This function is called after the initialization of default IPC channels.
 * It is called before restoring the RPMSG information while resume during cold or warm boot
 * Note that the initialization of IPC channels created by users should be invoked
 * called in this callback function.
 *
 * @return 0 - OK, others - NG
 */
extern INT_FUNCTION AmbaLink_UserIpcInitCallBack;


/**
 * @brief  This function is called after the suspend is done for all suspend mode.
 * While this function is called, linux suspend is done.
 *
 * @param [in] SuspendMode The mode to suspend.
 *
 * @return 0 - OK, others - NG
 *
 */
extern VOID_UINT32_IN_FUNCTION AmbaIPC_LinkCtrlSuspendLinuxDoneCallBack;

/**
 * @brief This function is called after IPC is ready no matter on cold, warm and hiber boot.
 * While this function is called, linux resume is done.
 *
 * @param [in] SuspendMode The suspend mode to resume from.
 *
 * @return 0 - OK, others - NG
 */
extern VOID_UINT32_IN_FUNCTION AmbaIPC_LinkCtrlResumeLinuxDoneCallBack;

/**
 * File parameters: for opening a file in CFS
 */
typedef struct AMBA_VFS_FILE_PARAM_s_ {
    char Filename[64];                                      /**< File name */
    UINT8 AsyncData;                                        /**< Asynchronous mode related data */
    UINT32 Alignment;                                       /**< Alignment in Byte, should be times of a cluster */
    UINT32 BytesToSync;                                     /**< File should be sync after BytesToSync bytes were write */
    UINT8 Mode;                                             /**< The open mode of the File */
    UINT8 AsyncMode;                                        /**< To indicate if the stream is in asynchronous mode. */
    UINT8 LowPriority;                                      /**< To indiFile if the stream is with low priority */
} AMBA_VFS_FILE_PARAM_s;

/**
 *  Define the virtula file system operation.
 */
typedef struct _AMBA_VFS_OP_s {
    void *(*fopen)(void *);
    int (*fclose)(void *);
    UINT64 (*fread)(void *, UINT64, UINT64, void*);
    UINT64 (*fwrite)(const void *, UINT64, UINT64, void*);
    int (*fseek)(void *, INT64, int);
    INT64 (*ftell)(void *);
    int (*FSync)(void *);
    UINT64 (*fappend)(void *, UINT64);
    int (*feof)(void *);
    int (*remove)(const char *);
    int (*Move)(const char *, const char *);
    int (*Stat)(const char *, void *);
    int (*Chmod)(const char *, int);
    int (*FirstDirEnt)(const char *, unsigned char, void *);
    int (*NextDirEnt)(void *);
    int (*Combine)(char *, char *);
    int (*Divide)(char *, char *, UINT32);
    int (*Cinsert)(const char *, UINT32, UINT32);
    int (*Cdelete)(const char *, UINT32, UINT32);
    int (*Mkdir)(const char *);
    int (*Rmdir)(const char *);
    int (*GetDev)(char, void *);
    int (*Format)(char, const char *);
    int (*Sync)(char, int);
    int (*GetFileParam)(AMBA_VFS_FILE_PARAM_s *fileParam);
} AMBA_VFS_OP_s;

/**
 * @brief This function is used to register the file system operation.
 * In ambafs, the registered file system operation is invoked. AmbaFS_XXX
 * is the default operation. Currently, the supported file system operations
 * are AmbaFS_XXX and AmpCFS_XXXX.
 *
 * @param [in] AMBA_VFS_OP_s The interface of the file system operation.
 * @see AMBA_VFS_OP_s
 */
int AmbaIPC_RegisterVFSOperation(AMBA_VFS_OP_s *Operation);

/**
 *  Define where to boot Linux on.
 */
typedef enum _AMBA_LINK_BOOT_ON_e_ {
    AMBALINK_BOOT_ON_CORTEX = 0,
    AMBALINK_BOOT_ON_ARM11,
} AMBA_LINK_BOOT_ON_e;

/**
 *  Define the hardware resource owner.
 */
typedef enum _AMBA_LINK_RESOURCE_OWNER_e_ {
    OWNER_RTOS = 0,
    OWNER_LINUX,
} AMBA_LINK_RESOURCE_OWNER_e;

/**
 *  AmbaIPC_LinkCtrlSuspendLinux related data structure.
 */

typedef enum _AMBA_LINK_SUSPEND_MODE_e_ {
    AMBA_LINK_HIBER_TO_DISK = 0,
    AMBA_LINK_HIBER_TO_RAM,
    AMBA_LINK_STANDBY_TO_RAM,
    AMBA_LINK_SLEEP_TO_RAM,
} AMBA_LINK_SUSPEND_MODE_e;

/**
 *  Define the boot type of AmbaLink.
 */
typedef enum _AMBA_LINK_BOOT_TYPE_e_ {
    AMBALINK_COLD_BOOT = 0,
    AMBALINK_WARM_BOOT,
    AMBALINK_HIBER_BOOT
} AMBA_LINK_BOOT_TYPE_e;


#define AMBALINK_LOAD_LINUX             0x1
#define AMBALINK_LINUX_LOADED           0x2
#define AMBALINK_BOOT_RPMSG_READY       0x4
#define AMBALINK_BOOT_IPC_READY         0x5
#define AMBALINK_SUSPEND_START          0x10
#define AMBALINK_SUSPEND_DONE           0x20
#define AMBALINK_RPMSG_RESTORE_DONE     0x40


/**
 *  Data structure of AmbaLink control.
 */
typedef struct _AMBA_LINK_CTRL_s_ {
    UINT32  AmbaLinkMachineID;          /**< Machine ID */
    UINT32  AmbaLinkMachineRev;         /**< Machine revision */
    UINT32  AmbaLinkSharedMemAddr;      /**< Shared memory address */
    UINT32  AmbaLinkMemSize;            /**< Memory size for Linux */

    UINT32  AmbarellaPPMSize;           /**< Reserved private memory from DRAM start */
    UINT32  AmbarellaZRealAddr;         /**< Linux kernel start address */
    UINT32  AmbarellaTextOfs;           /**< Linux kernel text offset */
    UINT32  AmbarellaParamsPhys;        /**< ATAG address */

    UINT32  VringC0AndC1BufAddr;        /**< Start address of shared vq buff for Core0 and Core1 */
    UINT32  VringC0ToC1Addr;            /**< Vring buf address for Core0 -> Core1 */
    UINT32  VringC1ToC0Addr;            /**< Vring buf address for Core1 -> Core0 */
    UINT32  RpmsgNumBuf;                /**< Number of RPMSG supported */
    UINT32  RpmsgBufSize;               /**< Per RPMSG buf size */
    UINT32  RpcPktSize;                 /**< per RPC packet size */

    UINT32  AmbaIpcSpinLockBufAddr;     /**< AmbaIPC spinlock shared memory address */
    UINT32  AmbaIpcSpinLockBufSize;     /**< AmbaIPC spinlock memory size */
    UINT32  AmbaIpcMutexBufAddr;        /**< AmbaIPC mutex shared memory address */
    UINT32  AmbaIpcMutexBufSize;        /**< AmbaIPC mutex memory size */

    UINT32  AmbaRpcProfileAddr;         /**< Rpc profiling shared memoy address */
    UINT32  AmbaRpmsgProfileAddr;       /**< Rpmsg profiling shared memory address */

    UINT32  RpmsgSuspBackupAddr;        /**< Rpmsg suspend backup area addr */
    UINT32  RpmsgSuspBackupSize;        /**< Rpmsg suspend backup area size */
    UINT32  BossSuspBackupAddr;         /**< BOSS suspend backup area addr */
    UINT32  BossSuspBackupSize;         /**< BOSS suspend backup area size */
    UINT32  BossDataAddr;               /**< BOSS shared data addr */
    UINT32  BossDataSize;               /**< BOSS shared data size */

    UINT32  AmbaLinkAossAddr;           /**< AOSS info address for hibernation */
    UINT32  AmbaHiberImgCheck;          /**< Hibernation image check */

    const char *pKernelCmdLine;         /**< Point to Linux kernel command line */

    void  *pBytePool;                   /**< Memory pool for AmbaLink module */

    UINT32  AmbaLinkRunTarget;          /**< Determine the CPU target to run Linux. */
    UINT32  AmbaLinkUsbOwner;           /**< Determine the USB owner. */
    UINT32  AmbaLinkUartDmaOwner;       /**< Determine the UART DMA owner. */
    UINT32  AmbaLinkKernelSpaceAddr;    /**< Determine the kernel space start address. */

    void    *pPrivate;                  /**< Reserved pointer for private data */
} AMBA_LINK_CTRL_s;

/**
 *  Data structure of AmbaLink tasks.
 */
typedef struct _AMBA_LINK_TASK_CTRL_s_ {
    INT32   SvcThreadNum;               /**< The number of defalut svc thread */


    /* SVC task: It is a RPC server which is responsible for processing the rpc client requests. */
    UINT32  SvcPriority;                /**< The priority for defalut svc thread */
    UINT32  SvcStackSize;               /**< The stack size for defalut svc thread */
    UINT32  SvcCoreSelection;           /**< Assign the svc thread on a predefined core */

    /* RVQ task: It is a RPMSG task which is used to execute received RPMSG callback function. */
    /* SVQ task: It is a RPMSG task which is used to execute sending RPMSG callback function. */
    UINT32  RvqPriority;                /**< The priority for rvq thread */
    UINT32  RvqStackSize;               /**< The stack size for rvq thread */
    UINT32  RvqCoreSelection;           /**< Assign the rvq thread on a predefined core */
    UINT32  SvqPriority;                /**< The priority for svq thread */
    UINT32  SvqStackSize;               /**< The stack size for svq thread */
    UINT32  SvqCoreSelection;           /**< Assign the svq thread on a predefined core */

    /* VFS OPS task: It is a vfs task used to call rtos file system APIs. */
    /* VFS SG task:  It is a vfs task used to handle scatter/gather buffer and trigger real file system read/write. */
    UINT32  VfsOpsPriority;             /**< The priority for vfs ops thread */
    UINT32  VfsOpsStackSize;            /**< The stack size for vfs ops thread */
    UINT32  VfsOpsCoreSelection;        /**< Assign the vfs ops thread on a predefined core */
    UINT32  VfsSgPriority;              /**< The priority for vfs sg thread */
    UINT32  VfsSgStackSize;             /**< The stack size for vfs sg thread */
    UINT32  VfsSgCoreSelection;         /**< Assign the vfs sg thread on a predefined core */

    /**
     *  Fuse task:  The functionality of fuse task is the same to VFS task,
     *  but the file system is implemented in user space in linux.
     */
    UINT32  FusePriority;               /**< The priority for fuse thread */
    UINT32  FuseStackSize;              /**< The stack size for fuse thread */
    UINT32  FuseCoreSelection;          /**< Assign the fuse thread on a predefined core */

    /* Hiber task: It is a task used to handle hibernation or suspend flow, such as save hibernation image, suspend linux. */
    UINT32  HiberPriority;              /**< The priority for hiber suspend task */
    UINT32  HiberStackSize;             /**< The stack size for hiber suspend task */
    UINT32  HiberCoreSelection;         /**< Assign the hiber suspend thread on a predefined core */

    /* AmbaLink task: It is a AmbaLink init task used to initialize dual OSes modules for non-Boss system. */
    UINT32  LinkTskPriority;            /**< The priority for AmbaLink task */
    UINT32  LinkTskStackSize;           /**< The stack size for AmbaLink task */
    UINT32  LinkTskCoreSelection;       /**< Assign the AmbaLink thread on a predefined core */

    /* Boss task: It is a boss guest task used to run linux for Boss system. */
    UINT32  BossTskPriority;            /**< The priority for BOSS task */
    UINT32  BossTskStackSize;           /**< The stack size for BOSS task */
    UINT32  BossTskCoreSelection;       /**< Assign the BOSS task on a predefined core */

} AMBA_LINK_TASK_CTRL_s;
/**
 * The data structure is defined for received data through RPMSG.
 *
 */
typedef struct _AMBA_IPC_MSG_CTRL_s_ {
    UINT32  Length;        /**< The length of the received data */
    void    *pMsgData;     /**< The pointer to the received data */
} AMBA_IPC_MSG_CTRL_s;

typedef void *AMBA_IPC_HANDLE;  /**<  RPMSG Channel */

/**
 * @brief This function is invoked when the channel receives the message.
 *
 * @param [in] IpcHandle RPMSG channel pointer
 * @param [in] pMsgCtrl The pointer to the received data
 *
 * @return 0 - OK, others - NG
 */
typedef int (*AMBA_IPC_MSG_HANDLER_f)(AMBA_IPC_HANDLE IpcHandle, AMBA_IPC_MSG_CTRL_s *pMsgCtrl);

/**
 * @brief This function is used to allocate and initialize a RPMsg channel.
 *
 * @param [in] pName Chaneel name
 * @param [in] MsgHandler RPMSG channel pointer
 *
 * @return 0 - OK, others - NG
 */
AMBA_IPC_HANDLE AmbaIPC_Alloc(const char *pName, AMBA_IPC_MSG_HANDLER_f MsgHandler);

/**
 * @brief This function is used to register a RPMsg channel.
 *
 * @param [in] Chaneel a RPMsg channel
 * @param [in] pRemote The remote host is fixed in Linux, so Remote use "NULL" by default.
 *
 * @return 0 - OK, -1 - NG
 */
int AmbaIPC_RegisterChannel(AMBA_IPC_HANDLE Channel, const char *pRemote);

/**
 * @brief This function is used to send a message through a RPMsg channel.
 * If no buffer is available, it will be blocked until a buffer becomes available.
 *
 * @param [in] Chaneel a RPMsg channel
 * @param [in] pData a message buffer.
 * @param [in] Length Message length.
 *
 * @return 0 - OK, -1 - NG
 */
int AmbaIPC_Send(AMBA_IPC_HANDLE Channel, void *pData, int Length);

/**
 * @brief This function is used to send a message through a RPMsg channel.
 * If there is no buffer available, it will immediately return as failed.
 *
 * @param [in] Chaneel a RPMsg channel
 * @param [in] pData a message buffer.
 * @param [in] Length Message length.
 *
 * @return 0 - OK, -1 - NG
 */
int AmbaIPC_TrySend(AMBA_IPC_HANDLE Channel, void *pData, int Length);

/**
 * @brief This function is used to unregister a RPMsg channel.
 *
 * @param [in] Chaneel a RPMsg channel
 *
 * @return 0 - OK, -1 - NG
 */
int AmbaIPC_UnregisterChannel(AMBA_IPC_HANDLE Channel);


/**
 * AmbaIPC global mutex and spinlock related APIs.
 *
 */

/**
 * @defgroup AmbaIPC_Mutex
 * @ingroup AmbaLink
 * @{
 */

/**
 * The provided global mutex id.
 *
 */
typedef enum _AMBA_IPC_MUTEX_IDX_e_ {
    AMBA_IPC_MUTEX_I2C_CHANNEL0 = 0,
    AMBA_IPC_MUTEX_I2C_CHANNEL1,
    AMBA_IPC_MUTEX_I2C_CHANNEL2,
    AMBA_IPC_MUTEX_SPI_CHANNEL0,
    AMBA_IPC_MUTEX_SPI_CHANNEL1,
    AMBA_IPC_MUTEX_SPI_CHANNEL2,
    AMBA_IPC_MUTEX_SD0,
    AMBA_IPC_MUTEX_SD1,
    AMBA_IPC_MUTEX_SD2,
    AMBA_IPC_MUTEX_NAND,
    AMBA_IPC_MUTEX_GPIO,
    AMBA_IPC_MUTEX_PLL,

    AMBA_IPC_NUM_MUTEX          /**< Total number of global mutex */
} AMBA_IPC_MUTEX_IDX_e;

/**
 * The provided global spinlock id.
 *
 */
typedef enum _AMBA_IPC_SPINLOCK_IDX_e_ {
    AMBA_IPC_SPINLOCK_GPIO,
    AMBA_IPC_SPINLOCK_BOSS,
    AMBA_IPC_SPINLOCK_L2C,

    AMBA_IPC_NUM_SPINLOCK      /**< Total number of global spin lock */
} AMBA_IPC_SPINLOCK_IDX_e;

#define AMBA_IPC_MAX_NUM_SPIN_ID        8       /**< maximum number of spin ID */

/**
 * @brief This function is used to lock the global mutex.
 *
 * @param [in] ID Mutex ID
 * @param [in] Ticks Timeout value in millisecond
 *
 * @return 0 - OK, others - NG
 */
int  AmbaIPC_MutexTake(UINT32 MutexID, UINT32 Timeout);

/**
 * @brief This function is used to unlock the global mutex.
 *
 * @param [in] ID Mutex ID
 *
 * @return 0 - OK, others - NG
 */
int  AmbaIPC_MutexGive(UINT32 MutexID);

/**
 * @brief This function is used to assign a spin lock.
 * Note that it does not disable interrupts. It is used exclusively to
 * lock a specified spin lock across dual operating systems. If ISR is
 * anticipated to access a spin lock, the AmbaIPC_SpinLockIrqSave function
 * should be used.
 *
 * @param [in] ID Spin lock ID
 *
 * @return 0 - OK, others - NG
 * @see AmbaIPC_SpinUnlock
 * @see AmbaIPC_SpinLockIrqSave
 */
int  AmbaIPC_SpinLock(UINT32 SpinID);

/**
 * @brief This function is used to unlock a global spin lock.
 * Note that it does not disable interrupts. It is used exclusively to
 * unlock a specified spin lock across dual operating systems.
 *
 * @param [in] ID Spin lock ID
 *
 * @return 0 - OK, others - NG
 * @see AmbaIPC_SpinUnlockIrqRestore
 */
int  AmbaIPC_SpinUnlock(UINT32 SpinID);

/**
 * @brief This function is used to assign a spin lock.
 * It is used when ISR is anticipated to access a spin lock. It will disable
 * interrupts and save the IRQ status.
 *
 * @param [in] ID Spin lock ID
 * @param [in] pFlags The pointer to saved IRQ status
 *
 * @return 0 - OK, others - NG
 * @see AmbaIPC_SpinLockIrqSave
 */
int  AmbaIPC_SpinLockIrqSave(UINT32 SpinID, UINT32 *pFlags);

/**
 * @brief This function is used to unlock a global spin lock.
 * It is used when ISR is anticipated to access a spin lock. It will restore the
 * IRQ status.
 *
 * @param [in] ID Spin lock ID
 * @param [in] pFlags IRQ status to be restored
 *
 * @return 0 - OK, others - NG
 * @see AmbaIPC_SpinLockIrqSave
 */
int  AmbaIPC_SpinUnlockIrqRestore(UINT32 SpinID, UINT32 Flags);

/** @} */ // end of group AmbaIPC_Mutex

/*---------------------------------------------------------------------------*\
 * AmbaIPC remote-processor-call framework APIs
\*---------------------------------------------------------------------------*/
/**
 * @defgroup AmbaIPC_RPC
 * @ingroup AmbaLink
 * @{
 */
#include "AmbaIPC_Rpc_Def.h"
#define RPMSG_DEBUG     1
#define RPC_DEBUG       1   /**< RPMSG_DEBUG must be enabled if RPC_DEBUG is on */
/**
 * @brief This function is used to register a service for a rpc program.
 * In this function, you can specify whether the service run on a defalut a new
 * thread. If a new thread is requested, the parameters related to the new thread
 * should be given.
 *
 * @param [in] prog RPC program ID
 * @param [in] vers RPC progran version
 * @param [in] name The name for RPC program
 * @param [in] priority The priority for the new thread executing the svc
 * @param [in] stack The stack for the new thread
 * @param [in] stack_size The stack size for the new thread
 * @param [in] info RPC program information
 * @param [in] new_thread Specify if a new thread is to run the svc.
 *
 * @return 0 - OK, others - NG
 * @see AmbaIPC_SvcUnregister
 */
int AmbaIPC_SvcRegister(
        INT32 prog,
        INT32 vers,
        char * const name,
        UINT32 priority,
        void *stack, UINT32 stack_size, AMBA_IPC_PROG_INFO_s *info,
        int new_thread);

/**
 * @brief This function is used to unrefister a service.
 *
 * @param [in] Program RPC program ID
 * @param [in] Version RPC program version
 *
 * @return 0 - OK, others - NG
 * @see AmbaIPC_SvcRegister
 */
int AmbaIPC_SvcUnregister(INT32 program, INT32 version);

/**
 * @brief This function is used to create a client to the RPC program.
 * A connection is created, when a client is created. The connection is
 * for the specific program id and version. Note that RPC program id and
 * version should be consistent with the server. The client id will be
 * returned, if creating a client is successful.
 *
 * @param [in] host Specify the server is in Linux or RTOS.
 * @param [in] Program RPC program ID
 * @param [in] Version RPC program version
 *
 * @return Nonzero - OK, NULL - NG
 * @see AmbaIPC_ClientDestroy
 * @see AMBA_IPC_HOST_e
 */
int AmbaIPC_ClientCreate(INT32 host, INT32 program, INT32 version);

/**
 * @brief This function is used to destroy a client to the RPC program.
 *
 * @param [in] clnt Client id returned by AmbaIPC_ClientCreate function.
 *
 * @return 0 - OK, others - NG
 * @see AmbaIPC_ClientCreate
 */
int AmbaIPC_ClientDestroy(INT32 clnt);

/**
 * @brief This function is used to create invoke RPC call.
 * After a connection is created, You can invoke RPC procedure of the RPC
 * RPC program. To remotely call the procedure, the procedure number should
 * be specified and the input and out parameter are also have to given.
 *
 * @param [in] client Client id retruned by AmbaIPC_ClientCreate function.
 * @param [in] proc RPC procedure number
 * @param [in] in The pointer to the input parameters
 * @param [in] in_len The size of the input parameters
 * @param [in] out The pointer to the output parameters
 * @param [in] out_len The size of the output parameters
 * @param [in] timeout The timeout value for waiting server response.
 *
 * @return 0 - OK, others - AMBA_IPC_REPLY_STATUS_e
 * @see AMBA_IPC_REPLY_STATUS_e
 */
AMBA_IPC_REPLY_STATUS_e AmbaIPC_ClientCall(INT32 client, INT32 proc,
                       void *in,  INT32 in_len,
                       void *out, INT32 out_len,
                       UINT32 timeout);

/**
 * @brief This function is used to get the description of error code.
 *
 * @param [in] ErrNum The error code got from rpc related function.
 *
 * @return - The error description.
 * @see AMBA_IPC_REPLY_STATUS_e
 */
const char *AmbaIPC_RpcStrError(int ErrNum);
/** @} */ // end of group AmbaIPC_RPC

/*---------------------------------------------------------------------------*\
 * AmbaIPC Streaming Support APIs
\*---------------------------------------------------------------------------*/
/**
 * @defgroup AmbaIPC_RFS
 * @ingroup AmbaLink
 * @{
 */

/**
 * @brief This function is used to open a remote file.
 *
 * @param [in] name The name of the target file.
 * @param [in] mode The file accessing mode r(read) or w(write)
 *
 * @return >0 - File pointer, others - NG
 * @see AmbaIPC_fclose
 */
void* AmbaIPC_fopen (const char *name, char *mode);

/**
 * @brief This function is used to close a remote file.
 *
 * @param [in] fp The file pointer returned by AmbaIPC_open function
 *
 * @return 0 - OK, <0 - NG
 * @see AmbaIPC_fopen
 */
int   AmbaIPC_fclose(void *fp);

/**
 * @brief This function is used to read a remote file.
 *
 * @param [in] buf The pointer to a buffer to store read data.
 * @param [in] size The size of read data
 * @param [in] fp The file pointer
 *
 * @return >0 - Total number of bytes read, others - NG
 * @see AmbaIPC_fwrite
 */
int   AmbaIPC_fread (void *buf, int size, void *fp);

/**
 * @brief This function is used to write a remote file.
 *
 * @param [in] buf The pointer to a buffer to be written.
 * @param [in] size The size of data to be written
 * @param [in] fp The file pointer
 *
 * @return >0 - Total number of bytes written, others - NG
 * @see AmbaIPC_fread
 */
int   AmbaIPC_fwrite(void *buf, int size, void *fp);

/**
 * The defintion for the reference posision in AmbaIPC_fseek.
 *
 * @see AmbaIPC_fseek
 */
#define AMBA_IPC_RFS_SEEK_SET 0  /**< Start of file */
#define AMBA_IPC_RFS_SEEK_CUR 1  /**< Current position of the file pointer */
#define AMBA_IPC_RFS_SEEK_END 2  /**< End of file */

/**
 * @brief This function is used to set file pointer to a new position.
 *
 * @param [in] fp The file pointer
 * @param [in] offset The bytes from the referenced position to a new position.
 * @param [in] origin The referenced position to set a new position.
 *
 * @return 0 - OK, <0 - NG
 */
INT64 AmbaIPC_fseek (void *fp,  INT64 offset, int origin);

/**
 * @brief This function is used to get the current position of the file pointer.
 *
 * @param [in] fp The file pointer
 *
 * @return >=0 - The current position, <0 - NG
 */
INT64 AmbaIPC_ftell (void *fp);

/**
 * @brief This function is used to create a remote directory.
 *
 * @param [in] name The name of the target directory.
 *
 * @return 0 - OK, <0 - NG
 */
int   AmbaIPC_mkdir (const char *name);

/**
 * @brief This function is used to remove a remote directory.
 *
 * @param [in] name The name of the target directory.
 *
 * @return 0 - OK, <0 - NG
 */
int   AmbaIPC_rmdir (const char *name);

/**
 * @brief This function is used to delete a remote file.
 *
 * @param [in] name The name of the target file.
 *
 * @return 0 - OK, <0 - NG
 */
int   AmbaIPC_remove(const char *name);

/**
 * @brief This function is used to chane the name of a remote file.
 *
 * @param [in] old_name The original name of the target file.
 * @param [in] new_name The new name of the target file.
 *
 * @return 0 - OK, <0 - NG
 */
int   AmbaIPC_move  (const char *old_name, const char *new_name);

/**
 * @brief This function is used to change the access permission of a remote file.
 * Only numeric mode in chmod is provided.
 *
 * @param [in] file_name The name of the target file.
 * @param [in] mode The access permission in octal digits.
 *
 * @return 0 - OK, <0 - NG
 * @see AmbaIPC_chdmod
 */
int   AmbaIPC_chmod (const char *file_name, int mode);

/**
 * @brief This function is used to change the access permission of a remote directory.
 * Only numeric mode in chmod is provided.
 *
 * @param [in] dir_name The name of the target directory.
 * @param [in] mode The access permission in octal digits.
 *
 * @return 0 - OK, <0 - NG
 * @see AmbaIPC_chmod
 */
int   AmbaIPC_chdmod (const char *dir_name, int mode);

/**
 * @brief This function is used to attach the filesystem on device to the target directory.
 *
 * @param [in] dev_name The device which is desired to be attached.
 * @param [in] dir_name The target directory to mount.
 * @param [in] type The type of the file system
 *
 * @return 0 - OK, <0 - NG
 * @see AmbaIPC_umount
 */
int   AmbaIPC_mount (const char *dev_name, const char *dir_name, const char* type);

/**
 * @brief This function is used to unmount the file system.
 *
 * @param [in] dir_name The remote directory where the file system has been mounted.
 *
 * @return 0 - OK, <0 - NG
 * @see AmbaIPC_mount
 */
int   AmbaIPC_umount(const char *name);

/**
 * @brief This function is used to flush file system buffers.
 *
 * @return 0 - OK, <0 - NG
 * @see AmbaIPC_fsync
 */
int   AmbaIPC_sync  (void);

/**
 * @brief This function is used to synchronize all the modified buffer for the file.
 *
 * @param [in] fp The file pointer
 *
 * @return 0 - OK, <0 - NG
 */
int   AmbaIPC_fsync (void *fp);

/**
 * To define the data structure storing the file information.
 *
 * @see AmbaIPC_stat
 */
typedef struct {
    UINT64  ino;            /**< inode number */
    UINT32  dev;            /**< ID of device containing file */
    UINT16  mode;           /**< protection */
    UINT32  nlink;          /**< number of hard links */
    UINT32  uid;            /**< user ID of owner */
    UINT32  gid;            /**< group ID of owner */
    UINT32  rdev;           /**< device ID */
    INT64   size;           /**< total size in bytes */
    INT32   atime_sec;      /**< last access time */
    INT32   atime_nsec;     /**< last access time */
    INT32   mtime_sec;      /**< last modification time */
    INT32   mtime_nsec;     /**< last modification time */
    INT32   ctime_sec;      /**< last status change time */
    INT32   ctime_nsec;     /**< last status change time */
    UINT32  blksize;        /**< block size for file system  */
    UINT64  blocks;         /**< number of 512 byte blocks allocated */
} AMBA_IPC_RFS_STAT_s;

/**
 * @brief This function is used to get information about a file or directory.
 *
 * @param [in] name The name of the file or directory
 * @param [in] stat The data structure storing the file information.
 *
 * @return 0 - OK, <0 - NG
 * @see AMBA_IPC_RFS_STAT_s
 */
int   AmbaIPC_stat  (const char *name, AMBA_IPC_RFS_STAT_s *stat);


/**
 * To define the supported formate types.
 *
 * @see AMBA_IPC_RFS_DEVINF_s
 * @see AmbaIPC_getdev
 */
typedef enum _AMBA_IPC_RFS_FMT_TYPE_e_ {
    AMBA_IPC_RFS_FMT_FAT12 = 0,
    AMBA_IPC_RFS_FMT_FAT16 = 1,
    AMBA_IPC_RFS_FMT_FAT32 = 2,
    AMBA_IPC_RFS_FMT_EXFAT = 3
} AMBA_IPC_RFS_FMT_TYPE_e;

/**
 * To define the data structure storing device information.
 *
 * @see AMBA_IPC_RFS_FMT_TYPE_e
 * @see AmbaIPC_getdev
 */
typedef struct _AMBA_IPC_RFS_DEVINF_s_ {
    UINT32 cls;     /**< total number of clusters */
    UINT32 ecl;     /**< number of unused clusters */
    UINT32 bps;     /**< byte count per sector */
    UINT32 spc;     /**< sector count per cluster */
    UINT32 cpg;     /**< cluster count per cluster group */
    UINT32 ecg;     /**< number of unused cluster groups */
    AMBA_IPC_RFS_FMT_TYPE_e fmt;    /**< format type */
} AMBA_IPC_RFS_DEVINF_s;


/**
 * @brief This function is used to get device information.
 *
 * @param [in] path The path where the device is located.
 * @param [in] devinf The data structure storing the device information.
 *
 * @return 0 - OK, <0 - NG
 * @see AMBA_IPC_RFS_DEVINF_s
 * @see AMBA_IPC_RFS_FMT_TYPE_e
 */
int   AmbaIPC_getdev(const char *path, AMBA_IPC_RFS_DEVINF_s *devinf);

/**
 * @brief This function is used to check whether the file pointer points to EOF.
 *
 * @param [in] fp File pointer
 *
 * @return 0 - OK, <0 - NG
 */
int   AmbaIPC_feof  (void* fp);

/**
 * @brief This function is used to open a remote directory.
 *
 * @param [in] name The name of the remote directory.
 *
 * @return >0 - File pointer, NULL- NG
 * @see AmabaIPC_closedir
 */
void* AmbaIPC_opendir(const char *name);

/**
 * To define the data structure storing the information for file
 * read from the remote directory.
 *
 * @see AmbaIPC_readdir
 */
typedef struct {
    UINT64  ino;        /**< inode number */
    UINT64  off;        /**< offset to the next dirent */
    UINT16  reclen;     /**< length of this record */
    UINT8   type;       /**< type of file */
    char    name[0];    /**< filename */
} AMBA_IPC_RFS_DIRENT_s;


/**
 * @brief This function is used to read from a remote directory.
 *
 * @param [in] dirp Pointer to the structure storing the information
 * for a remote directory.
 *
 * @return >0 - Direcoty entry pointer, NULL - NG
 * @see AMBA_IPC_RFS_DIRENT_s
 */
AMBA_IPC_RFS_DIRENT_s* AmbaIPC_readdir(void *dirp);

/**
 * @brief This function is used to close a remote directory.
 *
 * @param [in] dirp Pointer to the structure storing the information for
 * a remote directory.
 *
 * @return 0 - OK, <0 - NG
 * @see AmbaIPC_opendir
 */
int   AmbaIPC_closedir(void *dirp);

/**
 * @brief This function is used to change the current working directory
 * to the directory specified in the path.
 *
 * @param [in] path The target path
 *
 * @return >0 - OK, <0 - NG
 */
int   AmbaIPC_chdir (const char* path);
/** @} */ // end of group AmbaIPC_RFS

/*---------------------------------------------------------------------------*\
 * AmbaBOSS (Single core with dual-OSes) Support APIs
\*---------------------------------------------------------------------------*/
/**
 * @defgroup AmbaBOSS (Single core with dual-OSes)
 * @ingroup AmbaLink
 * @{
 */

/**
 * @brief This function is used to Get the interrupt ID in BOSS.
 *
 */
UINT32 *AmbaLink_GetSavedIntIDAddr(void);
/**
 * @brief This function is used to disable all guest interrupts in BOSS.
 *
 */
void AmbaLink_DisableGuestIrq(void);

/**
 * @brief This function is used to enable all guest interrupts in BOSS.
 *
 */
void AmbaLink_EnableGuestIrq(void);

/**
 * @brief This function is used to disable a interrupt in BOSS.
 *
 * @param [in] Irq The interrupr ID
 *
 */
void AmbaLink_DisableIrq(int Irq);

/**
 * @brief This function is used to enable a interrupts in BOSS.
 *
 * @param [in] Irq The interrupr ID
 *
 */
void AmbaLink_EnableIrq(int Irq);

/**
 * @brief This function is used to set the owner of an interrupt.
 * The value of Update parameter can be
 *      0 - Set IRQ to RTOS and do not update the VIC
 *      1 - Set IRQ to RTOS and update the VIC
 *      2 - Set IRQ to Linux and do not update the VIC
 *      3 - Set IRQ to Linux and update the VIC
 *
 * @param [in] Irq The interrupr ID
 * @param [in] Update Update to the interrupt controller or not
 *
 */
void AmbaLink_SetIrqOwner(int Irq, int Update);

/**
 * @brief This function is used to get the owner of an BOSS IRQ.
 *
 * @param [in] Irq The interrupt ID
 *
 * @return 0 - RTOS, 1 - Linux
 */
int AmbaLink_GetIrqOwner(int Irq);

/**
 * @brief This function is used to change the priority of BOSS.
 *
 * @param [in] NewPriority
 *
 * @return 0 - OK
 */
int AmbaLink_ChangeBossPriority(int NewPriority);

/**
 * @brief This function is used to enable a interrupts in BOSS.
 *
 * @param [in] Irq The interrupr ID
 *
 */
void AmbaLink_AdjustBossSchedulePeriod(UINT32 Period);

/**
 * @brief This function is used to enable or disable force scheduling BOSS task feature.
 * Note that the tasks that have lower priority than BOSS task will be blocked.
 *
 * @param [in] Enable Enable this feature or not.
 *
 */
void AmbaLink_ForceScheduleBossEnable(int Enable);

/** @} */ // end of group AmbaBOSS

/** @} */ // end of AmbaLink

#endif  /* _AMBA_LINK_H_ */
