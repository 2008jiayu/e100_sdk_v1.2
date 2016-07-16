/**
 * @file src/app/connected/applib/src/usb/ApplibUsb.c
 *
 *  USB gerenal API.
 *
 * History:
 *    2013/11/29 - [Martin Lai] created file
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
#include <AmbaPrintk.h>
#include <usb/AmbaUSB_API.h>
#include <usb/AmbaUSB_Host_API.h>
#include <applib.h>

//#define DEBUG_APPLIB_USB
#if defined(DEBUG_APPLIB_USB)
#define DBGMSG AmbaPrint
#define DBGMSGc(x) AmbaPrintColor(GREEN,x)
#define DBGMSGc2 AmbaPrintColor
#else
#define DBGMSG(...)
#define DBGMSGc(...)
#define DBGMSGc2(...)
#endif

#define APPLIB_USB_MEM_BUF_SIZE       (640*1024)
UINT32 AppLibUSBVbusConnect = 0;

static UINT32 CurClass = APPLIB_USB_DEVICE_CLASS_NONE;

/**
 *  @brief Callback function about USB connected
 *
 *  Callback function about USB connected
 */
static void AppLibUSB_VbusConnect(void)
{
    DBGMSGc2(GREEN,"[Applib - Usb] Vbus Connect!!");
    AppLibUSBVbusConnect = 1;
    AppLibComSvcHcmgr_SendMsgNoWait(HMSG_USB_DETECT_CONNECT, 0, 0);
}

/**
 *  @brief Callback function about USB disconnected
 *
 *  Callback function about USB disconnected
 */
static void AppLibUSB_VbusDisConnect(void)
{
    DBGMSGc2(GREEN,"[Applib - Usb] Vbus Disconnect!!");
    AppLibUSBVbusConnect = 0;
    AppLibComSvcHcmgr_SendMsgNoWait(HMSG_USB_DETECT_REMOVE, 0, 0);
}

static void AppLibUSB_DeviceSystemInit(void)
{
    int ReturnValue = 0;
    /* Initialization of USB Class*/
    USB_SYSTEM_INIT_s Sysconfig = {0};
    UINT8 *Buf = NULL, *BufRaw = NULL;

    ReturnValue = AmpUtil_GetAlignedPool(APPLIB_G_MMPL, (void **)&Buf, (void **)&BufRaw, APPLIB_USB_MEM_BUF_SIZE, 32);
    if (ReturnValue < 0) {
        AmbaPrintColor(RED,"[Applib - USB] <Init> %s:%u", __FUNCTION__, __LINE__);
    }
    Sysconfig.MemPoolPtr = Buf;
    Sysconfig.TotalMemSize = APPLIB_USB_MEM_BUF_SIZE;
    ReturnValue = AmbaUSB_System_DeviceSystemSetup(&Sysconfig);
    if (ReturnValue < 0) {
        AmbaPrintColor(RED,"[Applib - Usb] AmbaUSB_System_Init fail");
    }
}

static void AppLibUSB_DeviceSystemStart(void)
{
    AmbaPrint("[Applib - Usb] DeviceSystemStart");
    AppLibUSB_DeviceSystemInit();
}

static void AppLibUSB_DeviceSystemRelease(void)
{
    AmbaPrint("[Applib - Usb] DeviceSystemRelease");
//    AmbaUSB_DeviceSystemDestroy();
//    if (AmbaUSB_ClassCtrl[AmbaUsbDeviceClass].RelFunc) {
//        AmbaUSB_ClassCtrl[AmbaUsbDeviceClass].RelFunc();
//    }
}

static void AppLibUSB_DeviceSystemConfigured(UINT16 index)
{
    AmbaPrint("[Applib - Usb] Device System Configured, Index = %d", index);
}

static void AppLibUSB_DeviceSystemSuspended(void)
{
    AmbaPrint("[Applib - Usb] USB Device Suspended");
}

static void AppLibUSB_DeviceSystemResumed(void)
{
    AmbaPrint("[Applib - Usb] USB Device Resumed");
}

static void AppLibUSB_DeviceSystemReset(void)
{
    AmbaPrint("[Applib - Usb] USB Device Reset");
}

USB_DEV_VBUS_CB_s UsbVbusCb = {
    AppLibUSB_VbusConnect,
    AppLibUSB_VbusDisConnect,
    AppLibUSB_DeviceSystemStart,
    AppLibUSB_DeviceSystemRelease,
    AppLibUSB_DeviceSystemConfigured,
    AppLibUSB_DeviceSystemSuspended,
    AppLibUSB_DeviceSystemResumed,
    AppLibUSB_DeviceSystemReset
};


/**
 *  @brief Register the callback function about USB connected
 *
 *  Register the callback function about USB connected
 *
 *  @return >=0 success, <0 failure
 */
static int USB_RegisterVbusCallback(void)
{
    return AmbaUSB_System_RegisterVbusCallback(&UsbVbusCb);
}

static int AppLibUsbInit = -1;

/**
 *  @brief Initialization USB module
 *
 *  Initialization USB module
 *
 *  @return >=0 success, <0 failure
 */
int AppLibUSB_Init(void)
{
    int ReturnValue = 0;
    if (AppLibUsbInit == 0) {
        return 0;
    }

    DBGMSGc2(GREEN,"[Applib - Usb] AppLibUSB_Init start.");

    /* Initialization of USB Module. */
    AmbaUSB_System_SetMemoryPool(APPLIB_G_MMPL, APPLIB_G_NC_MMPL);

    /* Swicth the IRQ owner to RTOS. */
    AmbaUSB_System_SetUsbOwner(USB_IRQ_RTOS, 0);

    /* Switch Usb Phy0 owner to device. */
    AmbaUSB_Host_Init_SwitchUsbOwner(UDC_OWN_PORT);

    // Disconnect USB till USB class is hooked.
    // Otherwise host would find unknown device.
    AmbaUSB_System_SetDeviceDataConn(0);

    DBGMSGc2(GREEN,"[Applib - Usb] AppLibUSB_Init done.");
    AppLibUsbInit = 0;

    return ReturnValue;
}

/**
 *  @brief Initialization USB jack
 *
 *  Initialization USB jack
 *
 *  @return >=0 success, <0 failure
 */
int AppLibUSB_InitJack(void)
{
    int ReturnValue = 0;

    ReturnValue = USB_RegisterVbusCallback();
    if (ReturnValue < 0) {
        AmbaPrintColor(RED,"[Applib - Usb] USB_RegisterVbusCallback fail");
    }

    return ReturnValue;
}



/**
 *  @brief Simulate USB cable plug/unplug.
 *
 *  Simulate USB cable plug/unplug.
 *
 *  @param [in] enable Enable
 *
 *  @return >=0 success, <0 failure
 */
int AppLibUSB_SetConnection(UINT32 enable)
{
    AmbaUSB_System_SetConnect(enable);
    return 0;
}

/**
 *  @brief Get current active USB class
 *
 *  Get current active USB class
 *
 *  @return Class
 */
int AppLibUSB_GetCurClass(void)
{
    return CurClass;
}

#ifdef CONFIG_APP_ARD
UINT32 AppLibUSB_GetVbusStatus(void)
{
    return AmbaUSB_System_GetVbusStatus();
}
#endif
