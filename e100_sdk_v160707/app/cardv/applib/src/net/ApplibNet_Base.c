#include <AmbaDataType.h>
#include <AmbaPrintk.h>
#include <AmbaLink.h>
#include <net/NetEventNotifier.h>
#include <net/ApplibNet_Base.h>
#include <net/ApplibNet.h>
#include <comsvc/ApplibComSvc_Hcmgr.h>
#include "AmbaSysCtrl.h"



extern AMBA_LINK_CTRL_s AmbaLinkCtrl;

#define NETBASE_DBG_EN
#define NETBASE_ERR_EN

#undef NETBASE_DBG
#ifdef NETBASE_DBG_EN
#define NETBASE_DBG(fmt,args...) AmbaPrintColor(GREEN,fmt,##args);
#else
#define NETBASE_DBG(fmt,args...)
#endif

#undef NETBASE_ERR
#ifdef NETBASE_ERR_EN
#define NETBASE_ERR(fmt,args...) AmbaPrintColor(RED,fmt,##args);
#else
#define NETBASE_ERR(fmt,args...)
#endif

static UINT8 gAmbaLinkBooted = 0; //0: not booted, 1: booted
static AMBA_KAL_TASK_t gHookDefaultUserRPCTask = { 0 };

#define HookDefaultUserRPCTaskStackSize (2048)
static UINT8 *pHookDefaultUserRPCTaskStack = NULL;
static UINT8 *pHookDefaultUserRPCTaskStackRaw = NULL;

static int RegisterNotifyEventCB(void);


static void AmbaLink_SuspendDoneCallBack(UINT32 SuspendMode)
{
    switch (SuspendMode) {
    case AMBA_LINK_HIBER_TO_DISK:
        NETBASE_DBG("[AppLib - NetBase] <AmbaLink_SuspendDoneCallBack> AMBA_LINK_HIBER_TO_DISK done");
        AmbaPrint("====> auto reboot...\r\n\r\n\r\n");
        AmbaKAL_TaskSleep(1000);
        AmbaSysSoftReset();
        break;
    case AMBA_LINK_HIBER_TO_RAM:
        NETBASE_DBG("[AppLib - NetBase] <AmbaLink_SuspendDoneCallBack> AMBA_LINK_HIBER_TO_RAM done");
        break;
    case AMBA_LINK_STANDBY_TO_RAM:
        NETBASE_DBG("[AppLib - NetBase] <AmbaLink_SuspendDoneCallBack> AMBA_LINK_STANDBY_TO_RAM done");
        break;
    case AMBA_LINK_SLEEP_TO_RAM:
        NETBASE_DBG("[AppLib - NetBase] <AmbaLink_SuspendDoneCallBack> AMBA_LINK_SLEEP_TO_RAM done");
        break;
    default:
        break;
    }

}

static void AmbaLink_ResumeDoneCallBack(UINT32 SuspendMode)
{
    int BootType = 0;

    BootType = AmbaLink_BootType(3000);
    BootType = AmbaLink_BootType(3000);

    NETBASE_DBG("[AppLib - NetBase] <AmbaLink_ResumeDoneCallBack> Linux is %s and IPC is ready!",
        (BootType == AMBALINK_COLD_BOOT) ? "ColdBoot" :
        (BootType == AMBALINK_WARM_BOOT) ? "WarmBoot" : "Hibernation Boot");

    if (BootType == AMBALINK_COLD_BOOT) {
        return;
    }

    switch (SuspendMode) {
    case AMBA_LINK_HIBER_TO_DISK:
        NETBASE_DBG("[AppLib - NetBase] <AmbaLink_ResumeDoneCallBack> AMBA_LINK_HIBER_TO_DISK done");
        break;
    case AMBA_LINK_HIBER_TO_RAM:
        NETBASE_DBG("[AppLib - NetBase] <AmbaLink_ResumeDoneCallBack> AMBA_LINK_HIBER_TO_RAM done");
        break;
    case AMBA_LINK_STANDBY_TO_RAM:
        NETBASE_DBG("[AppLib - NetBase] <AmbaLink_ResumeDoneCallBack> AMBA_LINK_STANDBY_TO_RAM done");
        break;
    case AMBA_LINK_SLEEP_TO_RAM:
        NETBASE_DBG("[AppLib - NetBase] <AmbaLink_ResumeDoneCallBack> AMBA_LINK_SLEEP_TO_RAM done");
        break;
    default:
        break;
    }

}

static int AmbaLink_NetEventNotifierCallBack(void *hdlr, UINT32 event, void* info)
{
	switch (event) {
	case NET_EVENT_LINUX_BOOTED:
		gAmbaLinkBooted = 1;
		NETBASE_DBG("[AppLib - NetBase] <AmbaLink_NetEventNotifierCB> NET_EVENT_LINUX_BOOTED");
		AppLibComSvcHcmgr_SendMsgNoWait(AMSG_EVENT_BOSS_BOOTED, 0, 0);
		break;
    case NET_EVENT_LINUX_NETWORK_READY:
        NETBASE_DBG("[AppLib - NetBase] <AmbaLink_NetEventNotifierCB> NET_EVENT_LINUX_NETWORK_READY");
        break;
    case NET_EVENT_LINUX_NETWORK_OFF:
        NETBASE_DBG("[AppLib - NetBase] <AmbaLink_NetEventNotifierCB> NET_EVENT_LINUX_NETWORK_OFF");
        break;
    default:
        NETBASE_ERR("[AppLib - NetBase] <AmbaLink_NetEventNotifierCB> unknown event");
		break;
	}

    return OK;
}

//-- This function should be called after linux booted.
static int RegisterNotifyEventCB(void)
{
    AMP_NETEVENTNOTIFIER_INIT_CFG_s InitCfg;
    int ReturnValue = 0;
    static UINT8 RegisterAlready = 0;

    if (RegisterAlready) {
        return OK;
    }

    //-- Register liunx event notification callback function to receive event from linux.
    ReturnValue = AmpNetEventNotifier_GetInitDefaultCfg(&InitCfg);
    if (ReturnValue < 0) {
        NETBASE_ERR("[AppLib - NetBase] <RegisterNotifyEventCB> GetInitDefaultCfg fail. %d",ReturnValue);
        return NG;
    }

    InitCfg.cbEvent = AmbaLink_NetEventNotifierCallBack;
    ReturnValue = AmpNetEventNotifier_init(&InitCfg);
    if (ReturnValue < 0) {
        NETBASE_ERR("[AppLib - NetBase] <RegisterNotifyEventCB> AmpNetEventNotifier_init fail. %d",ReturnValue);
        return NG;
    }

    RegisterAlready = 1;

    return OK;
}

/* Task to wait Linux RPC module ready and hook default User RPC services */
void AmbaLink_HookDefaultUserRPC(UINT32 info)
{
    int BootType;

    BootType = AmbaLink_BootType(30000); //blocked until Linux ready
    //NETBASE_DBG("[AppLib - NetBase] <AmbaLink_HookDefaultUserRPC> AmbaLink_HookDefaultUserRPC: info = %u",info);

    if (BootType == AMBALINK_COLD_BOOT) {
        if(info == 1){ //hibernation enabled
            /* system should not do anything for cold boot, which will do hibernation later. */
            /* If your project does not enable hibernation, you have to remove this protection. */
            return;
        }
    }

    /* Init Event Notifier to receive Linux boot_done event */
    //NETBASE_DBG("[AppLib - NetBase] <AmbaLink_HookDefaultUserRPC> RegisterNotifyEventCB");
    RegisterNotifyEventCB();
}

static void AmbaLink_KillHookDefaultUserRPCTask(AMBA_KAL_TASK_t *pTask, UINT32 Condition)
{
    if (Condition == TX_THREAD_EXIT) {
        if (AmbaKAL_TaskDelete(pTask) != OK) {
            NETBASE_ERR("[AppLib - NetBase] <AmbaLink_KillHookDefaultUserRPCTask> delete task failed!");
        }

        if (pHookDefaultUserRPCTaskStackRaw) {
            AmbaKAL_BytePoolFree(pHookDefaultUserRPCTaskStackRaw);
        }
    }
}

int AppLibNetBase_GetBootStatus(void)
{
    return gAmbaLinkBooted;
}

int AppLibNetBase_InitAmbaLink(void *pMemoryPool, int HibernationEnabled)
{
    int ReturnValue = 0;

    if (!pMemoryPool) {
        NETBASE_ERR("[AppLib - NetBase] <InitAmbaLink> AMBA_LINK Enable fail. (pMemoryPool is NULL)");
        return NG;
    }

    //-- initialize Amba Link
    NETBASE_DBG("[AppLib - NetBase] <InitAmbaLink> AMBA_LINK enable");
    AmbaIPC_LinkCtrlSuspendLinuxDoneCallBack = (VOID_UINT32_IN_FUNCTION) AmbaLink_SuspendDoneCallBack;
    AmbaIPC_LinkCtrlResumeLinuxDoneCallBack = (VOID_UINT32_IN_FUNCTION) AmbaLink_ResumeDoneCallBack;
    AmbaLinkCtrl.pBytePool = pMemoryPool;

    AmbaLink_Init();
    AmbaLink_Load();
    AmbaLink_Boot(10000);

    /* Allocate stack for task 'HookDefaultUserRPCTask' */
    ReturnValue = AmpUtil_GetAlignedPool(APPLIB_G_MMPL,
                                        (void**)&pHookDefaultUserRPCTaskStack,
                                        (void**)&pHookDefaultUserRPCTaskStackRaw,
                                        HookDefaultUserRPCTaskStackSize, 32);
    if (ReturnValue < 0) {
        NETBASE_ERR("[AppLib - NetBase] <InitAmbaLink> Allocate stack buffer fail");
        return -1;
    }

    HibernationEnabled = (HibernationEnabled == 1) ? 1 : 0;

    /* Create task to hook default service right after RPC ready */
    AmbaKAL_TaskCreate(&gHookDefaultUserRPCTask, /* pTask */
                        "HookDefaultUserRPCTask", /* pTaskName */
                        50, /* Priority */
                        AmbaLink_HookDefaultUserRPC, /* void (*EntryFunction)(UINT32) */
                        HibernationEnabled, /* EntryArg, 1:hibernation enabled, otherwaie disabled */
                        (void *) pHookDefaultUserRPCTaskStack, /* pStackBase */
                        HookDefaultUserRPCTaskStackSize, /* StackByteSize */
                        AMBA_KAL_AUTO_START); /* AutoStart */

    if (tx_thread_entry_exit_notify(&gHookDefaultUserRPCTask, AmbaLink_KillHookDefaultUserRPCTask) != OK) {
        NETBASE_ERR("[AppLib - NetBase] <InitAmbaLink> Fail to do thread_entry_exit_notify(AmbaLink_KillHookDefaultUserRPCTask)");
    }

	return OK;
}

