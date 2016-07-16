/**
  * @file src/app/apps/flow/usb/connectedcam/usb_msc_func.c
  *
  * Functions of USB MSC class
  *
  * History:
  *    2013/12/02 - [Martin Lai] created file
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


#include <apps/flow/usb/usb_msc.h>
#include <system/status.h>
#include <system/app_pref.h>
#include <system/app_util.h>


#define VALID_SLOT_IN_MSC    0x0000000F
#define CARD_MIN_NAND_STORAGE_SIZE_USB_MSC  (32*1024*1024)    /** 32 MB */

static int usb_msc_mount_devices(void)
{
    int ReturnValue = 0;
    AMBA_SCM_STATUS_s info;

    ApplibUsbMsc_ClassInit();

    /** Mount SD0 slot if it exists */
    if (APP_CHECKFLAGS(VALID_SLOT_IN_MSC, (0x1<<SCM_SLOT_SD0))) {
        DBGMSG("%s(): SCM_SLOT_SD0 = %d", __FUNCTION__, SCM_SLOT_SD0);
        AmbaFS_Prf2CheckFormat(SCM_SLOT_SD0);
        ReturnValue = AmbaSCM_GetSlotStatus(SCM_SLOT_SD0, &info);
        if (ReturnValue != -1) {
            ApplibUsbMsc_DoMount(SCM_SLOT_SD0);
        }
    }

    /** Mount SD1 slot if it exists */
    if (APP_CHECKFLAGS(VALID_SLOT_IN_MSC, (0x1<<SCM_SLOT_SD1))) {
        DBGMSG("%s(): SCM_SLOT_SD1 = %d", __FUNCTION__, SCM_SLOT_SD1);
        AmbaFS_Prf2CheckFormat(SCM_SLOT_SD1);
        ReturnValue = AmbaSCM_GetSlotStatus(SCM_SLOT_SD1, &info);
        if (ReturnValue != -1) {
            ApplibUsbMsc_DoMount(SCM_SLOT_SD1);
        }
    }

    /** Mount FL0 slot if it exists */
    if (APP_CHECKFLAGS(VALID_SLOT_IN_MSC, (0x1 << SCM_SLOT_FL0))) {
        DBGMSG("%s(): SCM_SLOT_FL0 = %d", __FUNCTION__, SCM_SLOT_FL0);
        AmbaFS_Prf2CheckFormat(SCM_SLOT_FL0);
        ReturnValue = AmbaSCM_GetSlotStatus(SCM_SLOT_FL0, &info);
        if ((ReturnValue == 0) && (info.SecsCnt * info.SecSize > CARD_MIN_NAND_STORAGE_SIZE_USB_MSC)) {
            ApplibUsbMsc_DoMount(SCM_SLOT_FL0);
        }
    }

    /** Mount FL1 slot if it exists **/
    if (APP_CHECKFLAGS(VALID_SLOT_IN_MSC, (0x1 << SCM_SLOT_FL1))) {
        DBGMSG("%s(): SCM_SLOT_FL1 = %d", __FUNCTION__, SCM_SLOT_FL1);
        AmbaFS_Prf2CheckFormat(SCM_SLOT_FL1);
        ReturnValue = AmbaSCM_GetSlotStatus(SCM_SLOT_FL1, &info);
        if ((ReturnValue == 0) && (info.SecsCnt * info.SecSize > CARD_MIN_NAND_STORAGE_SIZE_USB_MSC)) {
            ApplibUsbMsc_DoMount(SCM_SLOT_FL1);
        }
    }

    app_status.UsbStorageUnmount = 1;

    return ReturnValue;
}

static int usb_msc_unmount_devices(void)
{
    int ReturnValue = 0;

    ApplibUsbMsc_DoUnMount(SCM_SLOT_SD0);
    ApplibUsbMsc_DoUnMount(SCM_SLOT_SD1);
    ApplibUsbMsc_DoUnMount(SCM_SLOT_FL0);
    ApplibUsbMsc_DoUnMount(SCM_SLOT_FL1);

    AppUtil_PollingAllSlots();

    return ReturnValue;
}

static int usb_msc_start(void)
{
    int ReturnValue = 0;

    AppLibUSB_Init();
    AmbaUSB_System_SetDeviceDataConn(0); // do not connect USB before mount devices
    /* Initialize the USB MSC. */
    ApplibUsbMsc_DoMountInit();
    /* Unmount the storage. */
    AppLibCard_Remove(AppLibCard_GetActiveSlot());
    /* Mount the storage on USB. */
    usb_msc_mount_devices();
    /* Start the USB class. */
    ApplibUsbMsc_Start();
    /* Init decoder. Decode black screen rather than use background source. */
    AppLibThmBasic_Init();

    return ReturnValue;
}

static int usb_msc_stop(void)
{
    int ReturnValue = 0;

    /* Deinit decoder. */
    AppLibThmBasic_Deinit();

    /* Stop the USB class. */
    ApplibUsbMsc_Stop();
    /* Unmount the storage. */
    usb_msc_unmount_devices();
    app_status.UsbStorageUnmount = 0;

    AppLibDisp_DeactivateWindow(DISP_CH_FCHAN, AppLibDisp_GetWindowId(DISP_CH_FCHAN, 0));
    AppLibDisp_DeactivateWindow(DISP_CH_DCHAN, AppLibDisp_GetWindowId(DISP_CH_DCHAN, 0));
    AppLibDisp_ChanStop(DISP_CH_FCHAN);
    /* Hide GUI */
    usb_msc.Gui(GUI_HIDE_ALL, 0, 0);
    usb_msc.Gui(GUI_FLUSH, 0, 0);

    return ReturnValue;
}


static int usb_msc_app_ready(void)
{
    int ReturnValue = 0;

    if (!APP_CHECKFLAGS(app_usb_msc.GFlags, APP_AFLAGS_READY)) {
        APP_ADDFLAGS(app_usb_msc.GFlags, APP_AFLAGS_READY);

        // ToDo: need to remove to handler when iav completes the dsp cmd queue mechanism
        AppLibGraph_Init();
        usb_msc.Func(USB_MSC_CHANGE_OSD, 0, 0);

        AppUtil_ReadyCheck(0);
        if (!APP_CHECKFLAGS(app_usb_msc.GFlags, APP_AFLAGS_READY)) {
            /* The system could switch the current app to other in the function "AppUtil_ReadyCheck". */
            return ReturnValue;
        }
    }

    /* Clean vout buffer */
    AppLibThmBasic_ClearScreen();
    usb_msc.Func(USB_MSC_GUI_INIT_SHOW, 0, 0);

    return ReturnValue;
}

static int usb_msc_detect_remove(void)
{
    int ReturnValue = 0;

#ifdef CONFIG_APP_ARD
    /*Will power off here.*/
    AppUtil_SwitchApp(APP_MISC_LOGO);
#else
    /** Switch to decode mode if the Fchan will be enabled and the app_status.jack_to_pb_mode is on.*/
    ReturnValue = AppLibDisp_SelectDevice(DISP_CH_FCHAN, DISP_ANY_DEV);
    if (APP_CHECKFLAGS(ReturnValue, DISP_FCHAN_NO_DEVICE) || (app_status.FchanDecModeOnly == 0) ) {
        app_status.LockDecMode = 0;
        AppUtil_SwitchApp(AppUtil_GetStartApp(0));
    } else {
        ReturnValue = AppLibDisp_ConfigMode(DISP_CH_FCHAN, AppLibSysVout_GetVoutMode(VOUT_DISP_MODE_1080P));
        if (ReturnValue < 0) {
            app_status.LockDecMode = 0;
            AppUtil_SwitchApp(AppUtil_GetStartApp(0));
        } else {
            /*Switch to thumbnail mode if the fchan will be enabled.*/
            app_status.LockDecMode = 1;
            AppUtil_SwitchApp(APP_THUMB_MOTION);
        }
    }
#endif

    return ReturnValue;
}

static int usb_msc_update_fchan_vout(UINT32 msg)
{
    int ReturnValue = 0;

    switch (msg) {
    case HMSG_HDMI_INSERT_SET:
    case HMSG_HDMI_INSERT_CLR:
        AppLibSysVout_SetJackHDMI(app_status.HdmiPluginFlag);
        break;
    case HMSG_CS_INSERT_SET:
    case HMSG_CS_INSERT_CLR:
        AppLibSysVout_SetJackCs(app_status.CompositePluginFlag);
        break;
    default:
        AmbaPrint("[app_usb_msc] Vout no changed");
        return 0;
        break;
    }
    ReturnValue = AppLibDisp_SelectDevice(DISP_CH_FCHAN, DISP_ANY_DEV);
    if (APP_CHECKFLAGS(ReturnValue, DISP_FCHAN_NO_CHANGE)) {
        AmbaPrint("[app_usb_msc] Display FCHAN has no changed");
    } else {
        if (APP_CHECKFLAGS(ReturnValue, DISP_FCHAN_NO_DEVICE)) {
            AppLibGraph_DisableDraw(GRAPH_CH_FCHAN);
            AppLibDisp_ChanStop(DISP_CH_FCHAN);
            AppLibDisp_DeactivateWindow(DISP_CH_FCHAN, AppLibDisp_GetWindowId(DISP_CH_FCHAN, 0));
            AppLibDisp_FlushWindow(DISP_CH_FCHAN);
            app_status.LockDecMode = 0;
        } else {
            AppLibDisp_ConfigMode(DISP_CH_FCHAN, AppLibSysVout_GetVoutMode(VOUT_DISP_MODE_1080P));
            AppLibDisp_SetupChan(DISP_CH_FCHAN);
            AppLibDisp_ChanStart(DISP_CH_FCHAN);
            {
                AMP_DISP_WINDOW_CFG_s Window;
                AMP_DISP_INFO_s DispDev = {0};

                memset(&Window, 0, sizeof(AMP_DISP_WINDOW_CFG_s));

                ReturnValue = AppLibDisp_GetDeviceInfo(DISP_CH_FCHAN, &DispDev);
                if ((ReturnValue < 0) || (DispDev.DeviceInfo.Enable == 0)) {
                    DBGMSG("[app_usb_msc] FChan Disable. Disable the fchan Window");
                    AppLibGraph_DisableDraw(GRAPH_CH_FCHAN);
                    AppLibDisp_DeactivateWindow(DISP_CH_FCHAN, AppLibDisp_GetWindowId(DISP_CH_FCHAN, 0));
                    AppLibDisp_FlushWindow(DISP_CH_FCHAN);
                    app_status.LockDecMode = 0;
                } else {
                    /** FCHAN Window*/
                    AppLibGraph_EnableDraw(GRAPH_CH_FCHAN);
                    AppLibDisp_GetWindowConfig(DISP_CH_FCHAN, AppLibDisp_GetWindowId(DISP_CH_FCHAN, 0), &Window);
                    Window.Source = AMP_DISP_DEC;
                    Window.SourceDesc.Dec.DecHdlr = 0;
                    Window.CropArea.Width = 0;
                    Window.CropArea.Height = 0;
                    Window.CropArea.X = 0;
                    Window.CropArea.Y = 0;
                    Window.TargetAreaOnPlane.Width = DispDev.DeviceInfo.VoutWidth;
                    Window.TargetAreaOnPlane.Height = DispDev.DeviceInfo.VoutHeight;
                    Window.TargetAreaOnPlane.X = 0;
                    Window.TargetAreaOnPlane.Y = 0;
                    AppLibDisp_SetWindowConfig(DISP_CH_FCHAN, AppLibDisp_GetWindowId(DISP_CH_FCHAN, 0), &Window);
                    AppLibDisp_ActivateWindow(DISP_CH_FCHAN, AppLibDisp_GetWindowId(DISP_CH_FCHAN, 0));
                    AppLibDisp_FlushWindow(DISP_CH_FCHAN);
                }
            }
            AppLibGraph_SetWindowConfig(GRAPH_CH_FCHAN);
            AppLibGraph_ActivateWindow(GRAPH_CH_FCHAN);
            AppLibGraph_FlushWindow(GRAPH_CH_FCHAN);
            usb_msc.Gui(GUI_SET_LAYOUT, 0, 0);
            usb_msc.Gui(GUI_FLUSH, 0, 0);
        }
    }

    return ReturnValue;
}

static int usb_msc_change_display(void)
{
    int ReturnValue = 0;

    AppLibDisp_SelectDevice(DISP_CH_FCHAN | DISP_CH_DCHAN, DISP_ANY_DEV);
    AppLibDisp_ConfigMode(DISP_CH_FCHAN | DISP_CH_DCHAN, AppLibSysVout_GetVoutMode(VOUT_DISP_MODE_1080P));
    AppLibDisp_SetupChan(DISP_CH_FCHAN | DISP_CH_DCHAN);
    AppLibDisp_ChanStart(DISP_CH_FCHAN | DISP_CH_DCHAN);
    {
        AMP_DISP_WINDOW_CFG_s Window;
        AMP_DISP_INFO_s DispDev = {0};

        memset(&Window, 0x0, sizeof(AMP_DISP_WINDOW_CFG_s));

        ReturnValue = AppLibDisp_GetDeviceInfo(DISP_CH_FCHAN, &DispDev);
        if ((ReturnValue < 0) || (DispDev.DeviceInfo.Enable == 0)) {
            DBGMSG("[app_usb_msc] FChan Disable. Disable the fchan Window");
            AppLibDisp_DeactivateWindow(DISP_CH_FCHAN, AppLibDisp_GetWindowId(DISP_CH_FCHAN, 0));
            AppLibGraph_DisableDraw(GRAPH_CH_FCHAN);
        } else {
            /** FCHAN Window*/
            AppLibDisp_GetWindowConfig(DISP_CH_FCHAN, AppLibDisp_GetWindowId(DISP_CH_FCHAN, 0), &Window);
            Window.Source = AMP_DISP_BACKGROUND_COLOR;
            Window.SourceDesc.Dec.DecHdlr = 0;
            Window.CropArea.Width = 0;
            Window.CropArea.Height = 0;
            Window.CropArea.X = 0;
            Window.CropArea.Y = 0;
            Window.TargetAreaOnPlane.Width = DispDev.DeviceInfo.VoutWidth;
            Window.TargetAreaOnPlane.Height = DispDev.DeviceInfo.VoutHeight;
            Window.TargetAreaOnPlane.X = 0;
            Window.TargetAreaOnPlane.Y = 0;
            AppLibDisp_SetWindowConfig(DISP_CH_FCHAN, AppLibDisp_GetWindowId(DISP_CH_FCHAN, 0), &Window);
            AppLibDisp_ActivateWindow(DISP_CH_FCHAN, AppLibDisp_GetWindowId(DISP_CH_FCHAN, 0));
            AppLibGraph_EnableDraw(GRAPH_CH_FCHAN);
        }

        ReturnValue = AppLibDisp_GetDeviceInfo(DISP_CH_DCHAN, &DispDev);
        if ((ReturnValue < 0) || (DispDev.DeviceInfo.Enable == 0)) {
            DBGMSG("[app_usb_msc] DChan Disable. Disable the Dchan Window");
            AppLibDisp_DeactivateWindow(DISP_CH_DCHAN, AppLibDisp_GetWindowId(DISP_CH_DCHAN, 0));
            AppLibGraph_DisableDraw(GRAPH_CH_DCHAN);
        } else {
            /** DCHAN Window*/
            AppLibDisp_GetWindowConfig(DISP_CH_DCHAN, AppLibDisp_GetWindowId(DISP_CH_DCHAN, 0), &Window);
            Window.Source = AMP_DISP_BACKGROUND_COLOR;
            Window.SourceDesc.Dec.DecHdlr = 0;
            Window.CropArea.Width = 0;
            Window.CropArea.Height = 0;
            Window.CropArea.X = 0;
            Window.CropArea.Y = 0;
            Window.TargetAreaOnPlane.Width = DispDev.DeviceInfo.VoutWidth;
            Window.TargetAreaOnPlane.Height = DispDev.DeviceInfo.VoutHeight;
            Window.TargetAreaOnPlane.X = 0;
            Window.TargetAreaOnPlane.Y = 0;
            AppLibDisp_SetWindowConfig(DISP_CH_DCHAN, AppLibDisp_GetWindowId(DISP_CH_DCHAN, 0), &Window);
            AppLibDisp_ActivateWindow(DISP_CH_DCHAN, AppLibDisp_GetWindowId(DISP_CH_DCHAN, 0));
            AppLibGraph_EnableDraw(GRAPH_CH_DCHAN);
        }
        AppLibDisp_FlushWindow(DISP_CH_FCHAN | DISP_CH_DCHAN);
    }

    return ReturnValue;
}

static int usb_msc_change_osd(void)
{
    int ReturnValue = 0;

    AppLibGraph_SetWindowConfig(GRAPH_CH_FCHAN | GRAPH_CH_DCHAN);
    AppLibGraph_ActivateWindow(GRAPH_CH_FCHAN | GRAPH_CH_DCHAN);
    AppLibGraph_FlushWindow(GRAPH_CH_FCHAN | GRAPH_CH_DCHAN);
    usb_msc.Gui(GUI_SET_LAYOUT, 0, 0);
    usb_msc.Gui(GUI_FLUSH, 0, 0);

    return ReturnValue;
}

static int usb_msc_start_show_gui(void)
{
    int ReturnValue = 0;
    usb_msc.Gui(GUI_APP_ICON_SHOW, 0, 0);
    usb_msc.Gui(GUI_FLUSH, 0, 0);
    return ReturnValue;
}

static int usb_msc_card_insert(void)
{
    int ReturnValue = 0;
    DBGMSGc2(GREEN, "%s:%d, card inserted", __FUNCTION__, __LINE__);
    AmbaUSB_System_SetDeviceDataConn(0);
    /* Mount the storage on USB. */
    usb_msc_mount_devices();
    AmbaUSB_System_SetDeviceDataConn(1);
    return ReturnValue;
}

static int usb_msc_card_remove(void)
{
    int ReturnValue = 0;
    /* Unmount the storage. */
    usb_msc_unmount_devices();
    return ReturnValue;
}

/**
 *  @brief The functions of video playback application
 *
 *  The functions of video playback application
 *
 *  @param[in] funcId Function id
 *  @param[in] param1 first parameter
 *  @param[in] param2 second parameter
 *
 *  @return >=0 success, <0 failure
 */
int usb_msc_func(UINT32 funcId, UINT32 param1, UINT32 param2)
{
    int ReturnValue = 0;

    switch (funcId) {
    case USB_MSC_INIT:
        break;
    case USB_MSC_START:
        usb_msc_start();
        break;
    case USB_MSC_STOP:
        usb_msc_stop();
        break;
    case USB_MSC_APP_READY:
        usb_msc_app_ready();
        break;
    case USB_MSC_DETECT_REMOVE:
        usb_msc_detect_remove();
        break;
    case USB_MSC_UPDATE_FCHAN_VOUT:
        usb_msc_update_fchan_vout(param1);
        break;
    case USB_MSC_CHANGE_DISPLAY:
        usb_msc_change_display();
        break;
    case USB_MSC_CHANGE_OSD:
        usb_msc_change_osd();
        break;
    case USB_MSC_GUI_INIT_SHOW:
        usb_msc_start_show_gui();
        break;
    case USB_MSC_CARD_INSERT:
        usb_msc_card_insert();
        break;
    case USB_MSC_CARD_REMOVE:
        usb_msc_card_remove();
        break;
    default:
        break;
    }

    return ReturnValue;
}
