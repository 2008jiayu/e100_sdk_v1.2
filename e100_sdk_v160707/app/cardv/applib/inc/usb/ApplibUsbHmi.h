/**
 * @file src/app/connected/applib/inc/usb/ApplibUsbHmi.h
 *
 * Header file to define USB HMI messages.
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
#ifndef APPLIB_USB_HMI_H_
#define APPLIB_USB_HMI_H_
/**
* @defgroup ApplibUsbHmi
* @brief define USB HMI messages.
*
*
*/

/**
 * @addtogroup ApplibUsbHmi
 * @ingroup USB
 * @{
 */

/*
 * Used by usb classes and application
 */
#define USBMSG                        0xe0
#define HMSG_ID_USB_DETECT        0xe1
#define HMSG_ID_USB_MODE_CHANGE        0xe2
#define HMSG_ID_USB_SUSPEND        0xe3
#define HMSG_ID_USB_RESUME        0xe4
#define HMSG_ID_USB_MASS_STORAGE_ACCESS    0xe5
#define HMSG_ID_MASS_STORAGE_EJECTED    0xe6
#define HMSG_ID_USB_PC_CONFIGURED    0xe7

#define USBMSG_SET(x)                    ((USBMSG << 24) |  (x))
#define USBMSG_REBOOT            USBMSG_SET(0) /*!< [UAVC] Message to notify Application to Reboot. */
#define USBMSG_BOOT_DSP            USBMSG_SET(1) /*!< [UAVC] Message to notify Application to boot DSP. */
#define USBMSG_SET_PREVIEW_PARA        USBMSG_SET(2) /*!< [UAVC] Message to notify Application to config preview parameters. */
#define USBMSG_SET_ENCODE_PARA        USBMSG_SET(3) /*!< [UAVC] Message to notify Application to config encode parameters. */
#define USBMSG_SET_VIN_PARA        USBMSG_SET(4) /*!< [UAVC] Message to notify Application to config vin parameters. */
#define USBMSG_SET_VOUT_PARA        USBMSG_SET(5) /*!< [UAVC] Message to notify Application to config vout parameters. */
#define USBMSG_START_ENCODE        USBMSG_SET(6) /*!< [UAVC] Message to notify Application to start encode. */
#define USBMSG_STOP_ENCODE        USBMSG_SET(7) /*!< [UAVC] Message to notify Application to stop encode. */
#define USBMSG_SET_AUDIO_PARA        USBMSG_SET(8) /*!< [UAVC] Message to notify Application to config audio parameters. */
#define USBMSG_SET_VOUT_ON        USBMSG_SET(9) /*!< [UAVC] Message to notify Application to enable vout. */
#define USBMSG_SET_VOUT_OFF        USBMSG_SET(10) /*!< [UAVC] Message to notify Application to disable vout. */
#define USBMSG_SET_FRAMEBASE_MODE    USBMSG_SET(11) /*!< [UAVC] Message to notify Application to enter frame-based mode. */
#define USBMSG_SET_SHARPNESS        USBMSG_SET(12) /*!< [UAVC] Message to notify Application to config sharpness. */
#define USBMSG_SET_SATURATION        USBMSG_SET(13) /*!< [UAVC] Message to notify Application to config saturation. */
#define USBMSG_SET_BRIGHTNESS        USBMSG_SET(14) /*!< [UAVC] Message to notify Application to config brightness. */
#define USBMSG_SET_EXPOSURE        USBMSG_SET(15) /*!< [UAVC] Message to notify Application to config exposure. */
#define USBMSG_SET_CONTRAST        USBMSG_SET(16) /*!< [UAVC] Message to notify Application to config brightness. */
#define USBMSG_SET_HUE            USBMSG_SET(17) /*!< [UAVC] Message to notify Application to config contrast. */
#define USBMSG_SET_WB_SHIFT        USBMSG_SET(18) /*!< [UAVC] Message to notify Application to config hue. */
#define USBMSG_SET_AGC_VALUE        USBMSG_SET(19) /*!< [UAVC] Message to notify Application to config White Balance Shift. */
#define USBMSG_3A_ENABLE        USBMSG_SET(20) /*!< [UAVC] Message to notify Application to config sensor AGC value. */
#define USBMSG_GET_DISK_STATUS        USBMSG_SET(21) /*!< [UAVC] Message to notify Application to enable/disable 3A. */
#define USBMSG_ACCESS_DISK        USBMSG_SET(22) /*!< [UAVC] Unused. */
#define USBMSG_SET_ZOOM            USBMSG_SET(23) /*!< [UAVC] Message to notify Application to config zoom. */
#define USBMSG_SET_FOCUS        USBMSG_SET(24) /*!< [UAVC] Message to notify Application to config focus. */
#define USBMSG_FWUPDATE_BOOT_MODE    USBMSG_SET(25) /*!< [UAVC] Message to notify Application to enter FW update mode. */
#define USBMSG_SYS_TIME_START        USBMSG_SET(26) /*!< [UAVC] Unused. */
#define USBMSG_SYS_TIME_STOP        USBMSG_SET(27) /*!< [UAVC] Unused. */
#define USBMSG_START_WDT        USBMSG_SET(28) /*!< [UAVC] Unused. */
#define USBMSG_STOP_WDT            USBMSG_SET(29) /*!< [UAVC] Unused. */
#define USBMSG_SET_MCTF            USBMSG_SET(30) /*!< [UAVC] UnUsed. */
#define USBMSG_SET_MJPEG_QUALITY    USBMSG_SET(31) /*!< [UAVC] Message to notify Application to config MJPEG Quality. */
#define USBMSG_OSD_ON            USBMSG_SET(32) /*!< [UAVC] Message to notify Application to enable OSD. */
#define USBMSG_OSD_OFF            USBMSG_SET(33) /*!< [UAVC] Message to notify Application to disable OSD. */
#define USBMSG_FLICKER_MODE        USBMSG_SET(34) /*!< [UAVC] Message to notify Application to config flicker mode. */
#define USBMSG_SYSTEM_DIAGNOSTICS    USBMSG_SET(35) /*!< [UAVC] Message to notify Application to do system diagnostics. */
#define USBMSG_DEVICE_CONTROL        USBMSG_SET(36) /*!< [UAVC] Message to notify Application to do device-specific control. */
#define USBMSG_HW_VERSION        USBMSG_SET(37) /*!< [UAVC] Message to notify Application to return HW version. */
#define USBMSG_HW_SN            USBMSG_SET(38) /*!< [UAVC] Message to notify Application to return HW serial number. */
#define USBMSG_READ_ADC            USBMSG_SET(39) /*!< [UAVC] Message to notify Application to return ADC value. */
#define USBMSG_AWB_MODE            USBMSG_SET(40) /*!< [UAVC] Message to notify Application to config AWB mode. */
#define USBMSG_BW_MODE            USBMSG_SET(41) /*!< [UAVC] Message to notify Application to config BW mode. */
#define USBMSG_CALIBRATE_DEVICE        USBMSG_SET(42) /*!< [UAVC] Message to notify Application to calibate device. */
#define USBMSG_CFA_NOISE_FILTER        USBMSG_SET(43) /*!< [UAVC] Unused. */
#define USBMSG_CHROMA_MEDIAN_FILTER    USBMSG_SET(44) /*!< [UAVC] Unused. */
#define USBMSG_LUMA_DIRECTIONAL_FILTER    USBMSG_SET(45) /*!< [UAVC] Unused. */
#define USBMSG_LUMA_SHARPEN        USBMSG_SET(46) /*!< [UAVC] Unused. */
#define USBMSG_AIN_LEVEL        USBMSG_SET(47) /*!< [UAVC] Unused. */
#define USBMSG_CC_MATRIX        USBMSG_SET(48) /*!< [UAVC] Unused. */
#define USBMSG_GAMMA_CURVE        USBMSG_SET(49) /*!< [UAVC] Unused. */
#define USBMSG_SET_VBR_CONTROL        USBMSG_SET(50) /*!< [UAVC] Unused. */
#define USBMSG_MEASURE_FRAME_LATENCY    USBMSG_SET(51) /*!< [UAVC] Unused. */
#define USBMSG_SET_APERATURE        USBMSG_SET(52) /*!< [UAVC] Unused. */
#define USBMSG_GET_IMAGE_START        USBMSG_SET(53) /*!< [UAVC] Unused. */
#define USBMSG_GET_IMAGE_END        USBMSG_SET(54) /*!< [UAVC] Unused. */
#define USBMSG_SET_AUDIO_LOOPBACK    USBMSG_SET(55) /*!< [UAVC] Unused. */
#define USBMSG_AE_BLC            USBMSG_SET(56) /*!< [UAVC] Unused. */
#define USBMSG_SENSOR_PROPERTY        USBMSG_SET(57) /*!< [UAVC] Unused. */
#define USBMSG_SET_SYSTEM_PROFILE    USBMSG_SET(58) /*!< [UAVC] Unused. */
#define USBMSG_RESET_SYSTEM_PROFILE    USBMSG_SET(59) /*!< [UAVC] Unused. */
#define USBMSG_GET_TASK_INFO        USBMSG_SET(60) /*!< [UAVC] Unused. */
#define USBMSG_SET_AIN_DEVICE        USBMSG_SET(61) /*!< [UAVC] Unused. */
#define USBMSG_SET_MD_INFO        USBMSG_SET(62) /*!< [UAVC] Unused. */
#define USBMSG_TEST_IRIS        USBMSG_SET(63) /*!< [UAVC] Unused. */
#define USBMSG_USBAPP            USBMSG_SET(64) /*!< [UAVC] Unused. */
#define USBMSG_CLASS_INIT        USBMSG_SET(65) /*!< [UAVC] Unused. */
#define USBMSG_AUTO_IRIS        USBMSG_SET(66) /*!< [UAVC] Unused. */
#define USBMSG_CURRENT_CONFIG        USBMSG_SET(67) /*!< [UAVC] Unused. */
#define USBMSG_NAND_PROGRAM        USBMSG_SET(68) /*!< [UAVC] Unused. */
#define USBMSG_SET_H264_BITRATE        USBMSG_SET(69) /*!< [UAVC] Message to notify Application to config H264 bitrate. */
#define USBMSG_SET_BACKLIGHT_COMPENSATION    USBMSG_SET(70) /*!< Unused. */
#define USBMSG_SET_WB_TEMPERATURE        USBMSG_SET(71) /*!< [UAVC] Unused. */
#define USBMSG_SET_GAMMA            USBMSG_SET(72) /*!< [UAVC] Unused. */
#define USBMSG_SET_EXPOSURE_TIME_ABSOLUTE    USBMSG_SET(73) /*!< [UAVC] Message to notify Application to config exposure time(Absolute). */
#define USBMSG_SET_EXPOSURE_TIME_RELATIVE    USBMSG_SET(74) /*!< [UAVC] Message to notify Application to config exposure time(Relative). */

/* For HMSG_USB_DETECT messages */
#define HMSG_USB_DETECT(x)            (((UINT32) HMSG_ID_USB_DETECT << 24) + (x))
#define HMSG_USB_DETECT_REMOVE        HMSG_USB_DETECT(0x0000) /*!< Message to notify Application that USB cable is removed. */
#define HMSG_USB_DETECT_CONNECT        HMSG_USB_DETECT(0x0001) /*!< Message to notify Application that USB cable is inserted. */
#define HMSG_USB_DETECT_RESET        HMSG_USB_DETECT(0x0002) /*!< Message to notify Application that USB MSC device is reseted. */
#define HMSG_USB_DETECT_SUSPEND        HMSG_USB_DETECT(0x0003) /*!< Message to notify Application that USB device is suspended. */
#define HMSG_USB_DETECT_RESUME        HMSG_USB_DETECT(0x0004) /*!< Message to notify Application that USB device is resumed. */
#define HMSG_USB_DETECT_RX_NOT_EMPTY    HMSG_USB_DETECT(0x0005) /*!< Message to notify Application that USB RX FIFO is not empty. */

/* For HMSG_USB_MASS_STORAGE_ACCESS */
#define HMSG_USB_MSC_ACCESS(x)            (((UINT32) HMSG_ID_USB_MASS_STORAGE_ACCESS    << 24) + (x))
#define HMSG_USB_MSC_ACCESS_STOP    HMSG_USB_MSC_ACCESS(0x0000)
#define HMSG_USB_MSC_ACCESS_START    HMSG_USB_MSC_ACCESS(0x0001)

/* For HMSG_USB_MODE_CHANGE */
#define HMSG_USB_GET_PAR(x)            (((UINT32) HMSG_ID_USB_MODE_CHANGE << 24) + (x))
#define HMSG_USB_GET_PAR_NO        HMSG_USB_GET_PAR(0x0000)
#define HMSG_USB_GET_PAR_YES        HMSG_USB_GET_PAR(0x0001)
#define HMSG_USB_EMU_SET_KEY        HMSG_USB_GET_PAR(0x0002)

#define HMSG_USB_SET_PAR(x)            (((UINT32) HMSG_ID_USB_MODE_CHANGE << 24) + (x) + (0x1000))
#define HMSG_USB_PLAY_START        HMSG_USB_SET_PAR(0x0001)
#define HMSG_USB_PLAY_STOP        HMSG_USB_SET_PAR(0x0002)
#define HMSG_USB_REC_START        HMSG_USB_SET_PAR(0x0003)
#define HMSG_USB_REC_STOP        HMSG_USB_SET_PAR(0x0004)

/* For Audio/MJPEG recording */
#define HMSG_USB_AV_SWITCH_MODE        HMSG_USB_GET_PAR(0x0010)
#define HMSG_USB_AV_CHMOD_REQ        HMSG_USB_GET_PAR(0x0011)

/* For Vendor Specific Message */
#define HMSG_USB_VENDOR_SET(x)            (((UINT32) HMSG_ID_USB_PC_CONFIGURED << 24) + (x))
#define HMSG_USB_VENDOR_ENB_MSC        HMSG_USB_VENDOR_SET(0x0000)
#define HMSG_USB_VENDOR_DIS_MSC        HMSG_USB_VENDOR_SET(0x0001)
#define HMSG_USB_VENDOR_ENB_PICT    HMSG_USB_VENDOR_SET(0x0002)
#define HMSG_USB_VENDOR_DIS_PICT    HMSG_USB_VENDOR_SET(0x0003)
#define HMSG_USB_VENDOR_ENB_STREAM    HMSG_USB_VENDOR_SET(0x0004)
#define HMSG_USB_VENDOR_DIS_STREAM    HMSG_USB_VENDOR_SET(0x0005)

/* USB->APP vendor specific pictbrdige error message */
#define HMSG_USB_PB_ERR_NOREASON    HMSG_USB_VENDOR_SET(0x0010)
#define HMSG_USB_PB_ERR_PAPER_ERR    HMSG_USB_VENDOR_SET(0x0011)
#define HMSG_USB_PB_ERR_INK_ERR        HMSG_USB_VENDOR_SET(0x0012)
#define HMSG_USB_PB_ERR_HW_ERR        HMSG_USB_VENDOR_SET(0x0013)
#define HMSG_USB_PB_ERR_FILE_ERR    HMSG_USB_VENDOR_SET(0x0014)
#define HMSG_USB_PB_ERR_COMMUNICATION_ERR    HMSG_USB_VENDOR_SET(0x0015)
/* USB->APP vendor specific pictbridge status message */
#define HMSG_USB_PB_TH_READY_DATE_ON    HMSG_USB_VENDOR_SET(0x0020)
#define HMSG_USB_PB_TH_READY_DATE_OFF    HMSG_USB_VENDOR_SET(0x0021)
#define HMSG_USB_PB_TH_READY_COPY_ON    HMSG_USB_VENDOR_SET(0x0022)
#define HMSG_USB_PB_TH_READY_COPY_OFF    HMSG_USB_VENDOR_SET(0x0023)
/* USB->APP vendor specific pictbridge print complete message */
#define HMSG_USB_PB_PRINT_COMPLETE    HMSG_USB_VENDOR_SET(0x0030)

#define HMSG_PICTBRIDGE_CONNECTED    HMSG_USB_VENDOR_SET(0x0100)
#define HMSG_PICTBRIDGE_DISCONNECTED    HMSG_USB_VENDOR_SET(0x0101)
#define HMSG_PICTBRIDGE_PRINTABLE    HMSG_USB_VENDOR_SET(0x0102)
#define HMSG_PICTBRIDGE_PRINT_END    HMSG_USB_VENDOR_SET(0x0103)
#define HMSG_PICTBRIDGE_NOTIFY_ERROR    HMSG_USB_VENDOR_SET(0x0104)
#define HMSG_PICTBRIDGE_NOTIFY_JOB_STATUS    HMSG_USB_VENDOR_SET(0x0105)
#define HMSG_PICTBRIDGE_NOTIFY_RESET_DONE    HMSG_USB_VENDOR_SET(0x0106)
#define HMSG_PICTBRIDGE_NOTIFY_PRINTER_STATUS    HMSG_USB_VENDOR_SET(0x0107)

#define HMSG_USB_SPEED_HIGH        HMSG_USB_VENDOR_SET(0x0200) /*!< Message to notify Application that USB is in High Speed. */
#define HMSG_USB_SPEED_FULL        HMSG_USB_VENDOR_SET(0x0201) /*!< Message to notify Application that USB is in Full Speed. */
#define HMSG_USB_CLASS_START        HMSG_USB_VENDOR_SET(0x0202) /*!< Message to notify Application that USB class is started. */
#define HMSG_USB_CLASS_STOP        HMSG_USB_VENDOR_SET(0x0203) /*!< Message to notify Application that USB class is stopped. */
#define HMSG_USB_RESET            HMSG_USB_VENDOR_SET(0x2004) /*!< Message to notify Application that USB device is reseted. */
#define HMSG_USB_SET_CFG        HMSG_USB_VENDOR_SET(0x2005) /*!< Message to notify Application that USB device is configured. */
__BEGIN_C_PROTO__

__END_C_PROTO__
/**
 * @}
 */
#endif /* APPLIB_USB_HMI_H_ */
