/**
 * @file src/app/connected/applib/src/usb/ApplibUsb_Msc.c
 *
 *  USB Mass Storage Class functions.
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

#include <applib.h>
#include <AmbaDataType.h>
#include <usb/AmbaUSB_API.h>
#include <AmbaCardManager.h>


//#define DEBUG_APPLIB_USB_MSC
#if defined(DEBUG_APPLIB_USB_MSC)
#define DBGMSG AmbaPrint
#define DBGMSGc(x) AmbaPrintColor(GREEN,x)
#define DBGMSGc2 AmbaPrintColor
#else
#define DBGMSG(...)
#define DBGMSGc(...)
#define DBGMSGc2(...)
#endif

/*-----------------------------------------------------------------------------------------------*\
 * Define MSC FS structure.
\*-----------------------------------------------------------------------------------------------*/

/* Define Storage Class USB MEDIA types.  */
#define MSC_MEDIA_FAT_DISK                       0
#define MSC_MEDIA_CDROM                          5
#define MSC_MEDIA_OPTICAL_DISK                   7
#define MSC_MEDIA_IOMEGA_CLICK                   0x55

/*-----------------------------------------------------------------------------------------------*\
 * Global Variables/Functions
\*-----------------------------------------------------------------------------------------------*/

UINT8   ApplibUsbMscStorageVendorId[]="Ambarella";               // 8 Bytes
UINT8   ApplibUsbMscStorageProductId[]="A12 DSC EVK Platform";   // 16 Bytes CONFIG_APP_ARD
UINT8   ApplibUsbMscStorageProductVer[]="1000";                 // 4 Bytes

static MSC_FSLIB_OP_s MscFsOp = {
    AmbaMSC_Read,
    AmbaMSC_Write,
    AmbaSCM_GetSlotStatus
};


int ApplibUsbMsc_ClassInit(void)
{
    int ReturnValue = 0;
    DBGMSGc2(GREEN,"[Applib - Usb MSC] ApplibUsbMsc_ClassInit ");
    /* Register File system Operation.  */
    ReturnValue = AmbaUSB_Class_Msc_Init(&MscFsOp);
    if (ReturnValue < 0) {
        AmbaPrintColor(RED,"[Applib - Usb MSC] AmbaUSB_Class_Msc_Init fail");
    }
    return ReturnValue;
}

/**
 *  @brief Mount drive.
 *
 *  Mount drive.
 *
 *  @return >=0 success, <0 failure
 */
#if 1
int ApplibUsbMsc_DoMount_test(void)
{
    int ReturnValue = 0;
    DBGMSGc2(GREEN,"[Applib - Usb MSC] ApplibUsbMsc_DoMount ");
    ApplibUsbMsc_ClassInit();
    /* Mount SCM_SLOT_SD00 */
    ReturnValue = AmbaUSB_Class_Msc_SetInfo(ApplibUsbMscStorageVendorId, ApplibUsbMscStorageProductId, ApplibUsbMscStorageProductVer);
    if (ReturnValue < 0) {
        AmbaPrintColor(RED,"[Applib - Usb MSC] AmbaUSB_Class_Msc_SetInfo fail");
    }
    ReturnValue = AmbaUSB_Class_Msc_SetProp(SCM_SLOT_SD0, 0x80, 0, MSC_MEDIA_FAT_DISK);
    if (ReturnValue < 0) {
        AmbaPrintColor(RED,"[Applib - Usb MSC] AmbaUSB_Class_Msc_SetProp fail");
    }
    ReturnValue = AmbaUSB_Class_Msc_Mount(SCM_SLOT_SD0);
    if (ReturnValue < 0) {
        AmbaPrintColor(RED,"[Applib - Usb MSC] AmbaUSB_Class_Msc_Mount fail");
    }
    return ReturnValue;
}
#endif
int ApplibUsbMsc_DoMountInit(void)
{
    int ReturnValue = 0;
    DBGMSGc2(GREEN,"[Applib - Usb MSC] ApplibUsbMsc_DoMountInit ");
    ApplibUsbMsc_ClassInit();

    ReturnValue = AmbaUSB_Class_Msc_SetInfo(ApplibUsbMscStorageVendorId, ApplibUsbMscStorageProductId, ApplibUsbMscStorageProductVer);
    if (ReturnValue < 0) {
        AmbaPrintColor(RED,"[Applib - Usb MSC] AmbaUSB_Class_Msc_SetInfo fail");
    }

    return ReturnValue;
}



/**
 *  @brief UnMount drive.
 *
 *  UnMount drive.
 *
 *  @return >=0 success, <0 failure
 */
INT32 ApplibUsbMsc_DoMount(UINT32 slot)
{
    int ReturnValue = 0;
    DBGMSGc2(GREEN,"[Applib - Usb MSC] ApplibUsbMsc_DoMount, slot = %d ", slot);

    ReturnValue = AmbaUSB_Class_Msc_SetProp(slot, 0x80, 0, MSC_MEDIA_FAT_DISK);
    if (ReturnValue < 0) {
        AmbaPrintColor(RED,"[Applib - Usb MSC] AmbaUSB_Class_Msc_SetProp fail");
    }

    ReturnValue = AmbaUSB_Class_Msc_Mount(slot);

    if (ReturnValue < 0) {
        AmbaPrintColor(RED,"[Applib - Usb MSC] AmbaUSB_Class_Msc_Mount fail Slot = %d", slot);
    }

    return ReturnValue;
}

/**
 *  @brief UnMount drive.
 *
 *  UnMount drive.
 *
 *  @return >=0 success, <0 failure
 */
INT32 ApplibUsbMsc_DoUnMount(UINT32 slot)
{
    int ReturnValue = 0;
    DBGMSGc2(GREEN,"[Applib - Usb MSC] ApplibUsbMsc_UnMount, slot = %d", slot);

    ReturnValue = AmbaUSB_Class_Msc_UnMount(slot);

    return ReturnValue;
}

#define APPLIB_USB_MSC_TASK_STACK_SIZE  (8 * 1024)
#define APPLIB_USB_MSC_TASK_STACK_PRIORITY  (70)

int ApplibUsbMsc_Start(void)
{
    int ReturnValue = 0;
    USB_CLASS_INIT_s ClassConfig = {UDC_CLASS_NONE, 0, 0};

    DBGMSGc2(GREEN,"[Applib - Usb MSC] Start ");

    /* Init USB custom device info.*/
    ReturnValue = AppLibUSB_Custom_SetDevInfo(APPLIB_USB_DEVICE_CLASS_MSC);
    if (ReturnValue < 0) {
        AmbaPrintColor(RED,"[Applib - Usb MSC] AppLibUSB_Custom_SetDevInfo fail");
    }

    /* Hook USB Class*/
    ClassConfig.classID = UDC_CLASS_MSC;
    ClassConfig.ClassTaskPriority = APPLIB_USB_MSC_TASK_STACK_PRIORITY;
    ClassConfig.ClassTaskStackSize = APPLIB_USB_MSC_TASK_STACK_SIZE;
    ReturnValue = AmbaUSB_System_ClassHook(&ClassConfig);
    if (ReturnValue < 0) {
        AmbaPrintColor(RED,"[Applib - Usb MSC] AmbaUSB_System_ClassHook fail");
    }

    // pollo - 2014/06/16 - Fix issue: [Jira][Amba168] USB: support for delayed initialization, Item 2:
    // Application will not initialize the USB class immediately after the vbus detect.
    // It will wait for switching to MTP mode.
    // The lower layer should keep the host happy till the application initializes the USB class.
    // 1. Add AmbaUSB_System_SetDeviceDataConn() for Applications to enable/disable USB device data connect.
    // 2. Add AmbaUSB_System_SetDeviceDataConnWithVbus() for Applications to setup USB device data connect status when VBUS is detected.
    //    1: Data connect will be enabled when VBUS is detected. PC will recognize it immediately.
    //    0: Data connect will NOT be enabled when VBUS is detected. PC will not recognize it until AmbaUSB_System_SetDeviceDataConn(1) is called.
    AmbaUSB_System_SetDeviceDataConn(1);

    return ReturnValue;
}

int ApplibUsbMsc_Stop(void)
{
    int ReturnValue = 0;
    USB_CLASS_INIT_s ClassConfig = {UDC_CLASS_NONE, 0, 0};

    DBGMSGc2(GREEN,"[Applib - Usb MSC] Stop ");
    ClassConfig.classID = UDC_CLASS_MSC;
    ClassConfig.ClassTaskPriority = APPLIB_USB_MSC_TASK_STACK_PRIORITY;
    ClassConfig.ClassTaskStackSize = APPLIB_USB_MSC_TASK_STACK_SIZE;
    AmbaUSB_System_ClassUnHook(&ClassConfig);
    if (ReturnValue < 0) {
        AmbaPrintColor(RED,"[Applib - Usb MSC] AmbaUSB_System_ClassHook fail");
    }

    return ReturnValue;
}

