/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaLinkMemoryCfg.c
 *
 *  @Copyright      :: Copyright (C) 2015 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: the module of Ambarella Network Support
\*-------------------------------------------------------------------------------------------------------------------*/

#include <stdio.h>

#include "AmbaDataType.h"
#include "AmbaCortexA9.h"
#include "AmbaRTSL_MMU.h"
#include "AmbaLink.h"
#include "AmbaKAL.h"

/*-----------------------------------------------------------------------------------------------*\
 * AmbaLink memory layout.
 *
 * DDR_ARM11_START  +-----------------------------------+   0x0000_0000
 *                  | Reserved memory                   |
 *                  |                                   |
 *                  +-----------------------------------+   __linux_start/AMBALINK_SHARE_MEM_ADDR / VRING_C0_AND_C1_BUF
 *                  | svq_buf (4MB)                     |
 *                  |                                   |
 *                  +-----------------------------------+
 *                  | rvq_buf (4MB)                     |
 *                  |                                   |
 *                  +-----------------------------------+   VRING_C0_TO_C1
 *                  | svq_vring (40KB)                  |
 *                  +-----------------------------------+   VRING_C1_TO_C0
 *                  | rvq_vring (40KB)                  |
 *                  +-----------------------------------+
 *                  | spin_lock (4KB)                   |
 *                  +-----------------------------------+
 *                  | global_mutex (4KB)                |
 *                  +-----------------------------------+
 *                  | rpmsg suspend backup area (128KB) |
 *                  +-----------------------------------+
 *                  | rpc profiling statistics &        |
 *                  | rpmsg profiling statistics (36KB) |
 *                  +-----------------------------------+
 *                  | AOSS (64 KB)                      |
 *                  +-----------------------------------+
 *                  | BOSS_SUSPEND_BACKUP (1 MB)        |
 *                  +-----------------------------------+
 *                  | BOSS_SHARED (4 KB)                |
 *                  +-----------------------------------+
 *                  | ...                               |
 *                  +-----------------------------------+   AMBARELLA_PARAMS_PHYS
 *                  | ATAG (16KB)                       |
 *                  +-----------------------------------+
 *                  | Kernel mmu table (16KB)           |
 *                  +-----------------------------------+   AMBARELLA_TEXTOFS
 *                  | Linux Kernel                      |   AMBALINK_MEMORY_SIZE
 *                  |                                   |
 *                  +-----------------------------------+
\*-----------------------------------------------------------------------------------------------*/

extern void *__linux_start;
extern void *__linux_end;

#if !defined(CONFIG_AMBALINK_MEM_SIZE)
#define CONFIG_AMBALINK_MEM_SIZE                0
#endif

#if !defined(CONFIG_AMBALINK_SHARED_MEM_SIZE)
#define CONFIG_AMBALINK_SHARED_MEM_SIZE         0
#endif

#if !defined(CONFIG_AMBALINK_RPMSG_NUM)
#define CONFIG_AMBALINK_RPMSG_NUM               0
#endif

#if !defined(CONFIG_AMBALINK_RPMSG_SIZE)
#define CONFIG_AMBALINK_RPMSG_SIZE              0
#endif

#if !defined(CONFIG_AMBALINK_VRING_SIZE)
#define CONFIG_AMBALINK_VRING_SIZE              0
#endif

#if !defined(CONFIG_AMBALINK_RPMSG_PROFILE_SIZE)
#define CONFIG_AMBALINK_RPMSG_PROFILE_SIZE      0
#endif

/*-----------------------------------------------------------------------------------------------*\
 * User defined
\*-----------------------------------------------------------------------------------------------*/
/* ATAG address: has to be 0x100000 boundary */
#define AMBARELLA_PARAMS_PHYS           (((UINT32) &__linux_start) + CONFIG_AMBALINK_SHARED_MEM_SIZE - \
                                        AMBA_CORTEX_A9_DRAM_VIRT_BASE_ADDR)
#define RPMSG_NUM_BUFS                  (CONFIG_AMBALINK_RPMSG_NUM)
#define RPMSG_BUF_SIZE                  (CONFIG_AMBALINK_RPMSG_SIZE)
#define VRING_SIZE                      (CONFIG_AMBALINK_VRING_SIZE)
#define RPC_RPMSG_PROFILE_SIZE          (CONFIG_AMBALINK_RPMSG_PROFILE_SIZE)

/*-----------------------------------------------------------------------------------------------*\
 * Calculate Ambalink shared memory size.
\*-----------------------------------------------------------------------------------------------*/

#define AMBALINK_SHARE_MEM_ADDR         ((UINT32) &__linux_start)               /* Shared memory base address */
#define AMBALINK_SHARE_MEM_SIZE         (CONFIG_AMBALINK_SHARED_MEM_SIZE)       /* Shared memory size */
#define AMBALINK_LINUX_MEM_SIZE         (CONFIG_AMBALINK_MEM_SIZE - CONFIG_AMBALINK_SHARED_MEM_SIZE)/* Linux memory size */

#define AMBALINK_TOTOAL_MEM_SIZE        (CONFIG_AMBALINK_MEM_SIZE)

#ifdef CONFIG_ENABLE_AMBALINK
int volatile AmbaLink_Enable = 1;
#else
int volatile AmbaLink_Enable = 0;
#endif

/* FIXME: System should provide a variable for eMMC boot. */
/*        But they do not provide currently. */
/*        Remove this if system is ready. */

#ifdef CONFIG_ENABLE_EMMC_BOOT
int volatile AmbaEMMC_Boot = 1;
#else
int volatile AmbaEMMC_Boot = 0;
#endif


#ifdef CONFIG_AMBALINK_CMDLINE
char KernelCmdLine[] = CONFIG_AMBALINK_CMDLINE;
#else
char KernelCmdLine[] = "ubi.mtd=8 root=ubi0:linux rootfstype=ubifs console=ttyS1 nr_cpus=1 maxcpus=0";
#endif

/*-----------------------------------------------------------------------------------------------*\
 * System defined and calcualted for Linux. (Linux config need to change accordingly.)
\*-----------------------------------------------------------------------------------------------*/
/* Private memory size. Reserved size before Linux. */
#define AMBARELLA_PPM_SIZE              AMBARELLA_PARAMS_PHYS

#define AMBARELLA_TEXTOFS               0x00008000      /* Kernel offset. */

/* Kernel start address. */
#define AMBARELLA_ZRELADDR              (AMBARELLA_PARAMS_PHYS + AMBARELLA_TEXTOFS)

/*-----------------------------------------------------------------------------------------------*\
 * Rpmsg defined and calcualted.
\*-----------------------------------------------------------------------------------------------*/
#define RPMSG_TOTAL_BUF_SPACE           (RPMSG_NUM_BUFS * RPMSG_BUF_SIZE)

#define VRING_C0_AND_C1_BUF             (AMBALINK_SHARE_MEM_ADDR)

#define MAX_VRING_SIZE                  0x13000
#define VRING_C0_TO_C1                  (VRING_C0_AND_C1_BUF + RPMSG_TOTAL_BUF_SPACE)
#define VRING_C1_TO_C0                  (VRING_C0_TO_C1 + VRING_SIZE)

/*-----------------------------------------------------------------------------------------------*\
 * Spin-lock defined and calcualted.
\*-----------------------------------------------------------------------------------------------*/
#define AMBAIPC_SPIN_LOCK_ADDR          (VRING_C1_TO_C0 + VRING_SIZE)
#define AMBAIPC_SPIN_LOCK_SIZE          0x1000

/*-----------------------------------------------------------------------------------------------*\
 * Global-mutex defined and calcualted.
\*-----------------------------------------------------------------------------------------------*/
#define AMBAIPC_MUTEX_ADDR              (AMBAIPC_SPIN_LOCK_ADDR + AMBAIPC_SPIN_LOCK_SIZE)
#define AMBAIPC_MUTEX_SIZE              0x1000

/*-----------------------------------------------------------------------------------------------*\
 * Rpmsg suspend backup area.
\*-----------------------------------------------------------------------------------------------*/
#define RPMSG_SUSPEND_BACKUP_ADDR       (AMBAIPC_MUTEX_ADDR + AMBAIPC_MUTEX_SIZE)
#define RPMSG_SUSPEND_BACKUP_SIZE       0x20000

/*-----------------------------------------------------------------------------------------------*\
 * Rpc and rpmsg profiling statistics area.
\*-----------------------------------------------------------------------------------------------*/
#define RPC_PROFILE_ADDR                (RPMSG_SUSPEND_BACKUP_ADDR + RPMSG_SUSPEND_BACKUP_SIZE)
#define RPC_PROFILE_SIZE                0x1000
#define RPMSG_PROFILE_ADDR              (RPC_PROFILE_ADDR + RPC_PROFILE_SIZE)
#define MAX_RPC_RPMSG_PROFILE_SIZE      0x11000

#define RPC_HEADER_SIZE                 128
#define RPMSG_HEADER_SIZE               16
#define RPC_PAYLOAD_SIZE                (RPMSG_BUF_SIZE - RPC_HEADER_SIZE - RPMSG_HEADER_SIZE)

/*-----------------------------------------------------------------------------------------------*\
 * AOSS info address (for hibernation) defined and calcualted.
\*-----------------------------------------------------------------------------------------------*/
#define AMBALINK_AOSS_ADDR              (RPC_PROFILE_ADDR + RPC_RPMSG_PROFILE_SIZE)
#define AMBALINK_AOSS_SIZE              0x10000

/*-----------------------------------------------------------------------------------------------*\
 * Rpmsg suspend backup area.
\*-----------------------------------------------------------------------------------------------*/
#define BOSS_SUSPEND_BACKUP_ADDR        (AMBALINK_AOSS_ADDR + AMBALINK_AOSS_SIZE)
#define BOSS_SUSPEND_BACKUP_SIZE        0x100000

/*-----------------------------------------------------------------------------------------------*\
 * Rpmsg suspend backup area.
\*-----------------------------------------------------------------------------------------------*/
#define BOSS_SHARED_ADDR                (BOSS_SUSPEND_BACKUP_ADDR + BOSS_SUSPEND_BACKUP_SIZE)
#define BOSS_SHARED_SIZE                0x1000

#define AMBALINK_USED_SHARE_MEM_END_ADDR    (BOSS_SHARED_ADDR + BOSS_SHARED_SIZE)

/*-----------------------------------------------------------------------------------------------*\
 * System defined and calcualted.
\*-----------------------------------------------------------------------------------------------*/
#define AMBALINK_MACHINE_ID         0x1019          /* Machine ID. */
#define AMBALINK_MACHINE_REV        0x23283041      /* Machine revision. */

#ifdef CONFIG_AMBALINK_BOOT_ARM11_LINUX
#define AMBALINK_BOOT_TARGET        AMBALINK_BOOT_ON_ARM11
#else
#define AMBALINK_BOOT_TARGET        AMBALINK_BOOT_ON_CORTEX
#endif

/*-----------------------------------------------------------------------------------------------*\
 * The parameters for the tasks related to AmbaLink.
\*-----------------------------------------------------------------------------------------------*/
#define SVC_PRIORITY                97
#define SVC_STACK_SIZE              0x1000
#define SVC_THREAD_NUM              2
#define SVC_CORE_SELECTION          0x1
/* For core selection, 0x1 means running on core 0, 0x2 means running on core 1 231
and 0x3 means running on core 0 or core 1. */

#define RVQ_PRIORITY                82
#define RVQ_STACK_SIZE              0x1000
#define RVQ_CORE_SELECTION          0x1

#define SVQ_PRIORITY                80
#define SVQ_STACK_SIZE              0x1000
#define SVQ_CORE_SELECTION          0x1

#define VFS_OPS_PRIORITY            128
#define VFS_OPS_STACK_SIZE          0x4000
#define VFS_OPS_CORE_SELECTION      0x1

#define VFS_SG_PRIORITY             130
#define VFS_SG_STACK_SIZE           0x1000
#define VFS_SG_CORE_SELECTION       0x1

#define FUSE_PRIORITY               132
#define FUSE_STACK_SIZE             0x4000
#define FUSE_CORE_SELECTION         0

#define HIBER_PRIORITY              140
#define HIBER_STACK_SIZE            0x1000
#define HIBER_CORE_SELECTION        0x1

#define LINK_PRIORITY               129
#define LINK_STACK_SIZE             0x2000
#define LINK_CORE_SELECTION         0x1

#define BOSS_PRIORITY               (AMBA_KAL_TASK_LOWEST_PRIORITY)
#define BOSS_STACK_SIZE             0x8000
#define BOSS_CORE_SELECTION         0x1

AMBA_LINK_CTRL_s AmbaLinkCtrl;
AMBA_LINK_TASK_CTRL_s AmbaLinkTaskCtrl;

/* Tell Linux only resume these GPIO pins. */
/* SDIO */
char OnlyResumeGpios[] = "81, 82, 83, 84, 85, 86, 87, 88";

void AmbaLink_LinkCtrlCfg(void)
{
    AmbaLinkCtrl.AmbaLinkMachineID          = AMBALINK_MACHINE_ID;
    AmbaLinkCtrl.AmbaLinkMachineRev         = AMBALINK_MACHINE_REV;
    AmbaLinkCtrl.AmbaLinkSharedMemAddr      = AMBALINK_SHARE_MEM_ADDR;
    AmbaLinkCtrl.AmbaLinkMemSize            = AMBALINK_LINUX_MEM_SIZE;

    AmbaLinkCtrl.AmbarellaPPMSize           = AMBARELLA_PPM_SIZE;
    AmbaLinkCtrl.AmbarellaZRealAddr         = AmbaRTSL_MmuPhysToVirt(AMBARELLA_ZRELADDR);
    AmbaLinkCtrl.AmbarellaTextOfs           = AMBARELLA_TEXTOFS;
    AmbaLinkCtrl.AmbarellaParamsPhys        = AMBARELLA_PARAMS_PHYS;

    AmbaLinkCtrl.AmbaLinkAossAddr           = AMBALINK_AOSS_ADDR;
    AmbaLinkCtrl.AmbaHiberImgCheck          = 0;

    AmbaLinkCtrl.VringC0AndC1BufAddr        = VRING_C0_AND_C1_BUF;
    AmbaLinkCtrl.VringC0ToC1Addr            = VRING_C0_TO_C1;
    AmbaLinkCtrl.VringC1ToC0Addr            = VRING_C1_TO_C0;
    AmbaLinkCtrl.RpmsgNumBuf                = RPMSG_NUM_BUFS;
    AmbaLinkCtrl.RpmsgBufSize               = RPMSG_BUF_SIZE;
    AmbaLinkCtrl.RpmsgSuspBackupAddr        = RPMSG_SUSPEND_BACKUP_ADDR;
    AmbaLinkCtrl.RpmsgSuspBackupSize        = RPMSG_SUSPEND_BACKUP_SIZE;
    AmbaLinkCtrl.RpcPktSize                 = RPC_PAYLOAD_SIZE;

    AmbaLinkCtrl.AmbaIpcSpinLockBufAddr     = AMBAIPC_SPIN_LOCK_ADDR;
    AmbaLinkCtrl.AmbaIpcSpinLockBufSize     = AMBAIPC_SPIN_LOCK_SIZE;
    AmbaLinkCtrl.AmbaIpcMutexBufAddr        = AMBAIPC_MUTEX_ADDR;
    AmbaLinkCtrl.AmbaIpcMutexBufSize        = AMBAIPC_MUTEX_SIZE;

    AmbaLinkCtrl.AmbaRpcProfileAddr         = RPC_PROFILE_ADDR;
    AmbaLinkCtrl.AmbaRpmsgProfileAddr       = RPMSG_PROFILE_ADDR;

    AmbaLinkCtrl.BossSuspBackupAddr         = BOSS_SUSPEND_BACKUP_ADDR;
    AmbaLinkCtrl.BossSuspBackupSize         = BOSS_SUSPEND_BACKUP_SIZE;
    AmbaLinkCtrl.BossDataAddr               = BOSS_SHARED_ADDR;
    AmbaLinkCtrl.BossDataSize               = BOSS_SHARED_SIZE;

    AmbaLinkCtrl.pKernelCmdLine             = KernelCmdLine;

    AmbaLinkCtrl.AmbaLinkRunTarget          = AMBALINK_BOOT_TARGET;
    AmbaLinkCtrl.AmbaLinkUsbOwner           = OWNER_RTOS;

    /* This virtual address is related to Linux CONFIG_VMSPLIT_1/2/3G. */
    /* Designed for CONFIG_VMSPLIT_1G. */
    AmbaLinkCtrl.AmbaLinkKernelSpaceAddr    = 0x40000000;

    AmbaLinkCtrl.pPrivate                   = (void *) OnlyResumeGpios;

    /* set the task-related configuratoin */
    AmbaLinkTaskCtrl.SvcPriority            = SVC_PRIORITY;
    AmbaLinkTaskCtrl.SvcStackSize           = SVC_STACK_SIZE;
    AmbaLinkTaskCtrl.SvcCoreSelection       = SVC_CORE_SELECTION;
    AmbaLinkTaskCtrl.SvcThreadNum           = SVC_THREAD_NUM;

    AmbaLinkTaskCtrl.RvqPriority            = RVQ_PRIORITY;
    AmbaLinkTaskCtrl.RvqStackSize           = RVQ_STACK_SIZE;
    AmbaLinkTaskCtrl.RvqCoreSelection       = RVQ_CORE_SELECTION;
    AmbaLinkTaskCtrl.SvqPriority            = SVQ_PRIORITY;
    AmbaLinkTaskCtrl.SvqStackSize           = SVQ_STACK_SIZE;
    AmbaLinkTaskCtrl.SvqCoreSelection       = SVQ_CORE_SELECTION;

    AmbaLinkTaskCtrl.VfsOpsPriority         = VFS_OPS_PRIORITY;
    AmbaLinkTaskCtrl.VfsOpsStackSize        = VFS_OPS_STACK_SIZE;
    AmbaLinkTaskCtrl.VfsOpsCoreSelection    = VFS_OPS_CORE_SELECTION;

    AmbaLinkTaskCtrl.VfsSgPriority          = VFS_SG_PRIORITY;
    AmbaLinkTaskCtrl.VfsSgStackSize         = VFS_SG_STACK_SIZE;
    AmbaLinkTaskCtrl.VfsSgCoreSelection     = VFS_SG_CORE_SELECTION;

    AmbaLinkTaskCtrl.FusePriority           = FUSE_PRIORITY;
    AmbaLinkTaskCtrl.FuseStackSize          = FUSE_STACK_SIZE;
    AmbaLinkTaskCtrl.FuseCoreSelection      = FUSE_CORE_SELECTION;

    AmbaLinkTaskCtrl.HiberPriority          = HIBER_PRIORITY;
    AmbaLinkTaskCtrl.HiberStackSize         = HIBER_STACK_SIZE;
    AmbaLinkTaskCtrl.HiberCoreSelection     = HIBER_CORE_SELECTION;

    AmbaLinkTaskCtrl.LinkTskPriority        = LINK_PRIORITY;
    AmbaLinkTaskCtrl.LinkTskStackSize       = LINK_STACK_SIZE;
    AmbaLinkTaskCtrl.LinkTskCoreSelection   = LINK_CORE_SELECTION;

    AmbaLinkTaskCtrl.BossTskPriority        = BOSS_PRIORITY;
    AmbaLinkTaskCtrl.BossTskStackSize       = BOSS_STACK_SIZE;
    AmbaLinkTaskCtrl.BossTskCoreSelection   = BOSS_CORE_SELECTION;
}
