/**
 * @file src/app/cardv/applib/inc/usb/ApplibUsb.h
 *
 * Header of USB
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

#ifndef APPLIB_USB_H_
#define APPLIB_USB_H_
/**
 * @defgroup USB
 * @brief USB related function implementation
 *
 * Implementation of
 * USB related function
 *
 */

/**
* @defgroup ApplibUsb
* @brief USB utility
*
*
*/

/**
 * @addtogroup ApplibUsb
 * @ingroup USB
 * @{
 */
#include <applib.h>

__BEGIN_C_PROTO__

typedef enum _APPLIB_USB_DEVICE_CLASS_e_ {
    APPLIB_USB_DEVICE_CLASS_NONE = 0,
    APPLIB_USB_DEVICE_CLASS_MSC,      /* mass storage class */
    APPLIB_USB_DEVICE_CLASS_MTP,      /* mtp class */
    APPLIB_USB_NUM_DEVICE_CLASS
} APPLIB_USB_DEVICE_CLASS_e;


/**
 *  @brief Initialization USB module
 *
 *  Initialization USB module
 *
 *  @return >=0 success, <0 failure
 */
extern INT32 AppLibUSB_Init(void);

/**
 *  @brief Initialization USB jack
 *
 *  Initialization USB jack
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibUSB_InitJack(void);

/**
 *  @brief Initialization USB MSC mount
 *
 *  Initialization USB MSC mount
 *
 *  @return >=0 success, <0 failure
 */
extern INT32 ApplibUsbMsc_MountInit(void);

/**
 *  @brief Initialization USB MSC DoMount
 *
 *  Initialization USB MSC DoMount
 *
 *
 *  @return >=0 success, <0 failure
 */
extern int ApplibUsbMsc_DoMountInit(void);

/**
 *  @brief USB MSC Mount drive
 *
 *  USB MSC Mount drive
 *
 *  @param [in] slot
 *
 *  @return >=0 success, <0 failure
 */
extern INT32 ApplibUsbMsc_DoMount(UINT32 slot);

/**
 *  @brief USB MSC Do Un Mount Drive
 *
 *  USB MSC Do Un Mount Drive
 *
 *  @param [in] slot
 *
 *  @return >=0 success, <0 failure
 */
extern INT32 ApplibUsbMsc_DoUnMount(UINT32 slot);

/**
 *  @brief Initialization USB MSC Start
 *
 *  Initialization USB MSC Start
 *
 *  @return >=0 success, <0 failure
 */
extern INT32 ApplibUsbMsc_Start(void);

/**
 *  @brief Initialization USB MSC Stop
 *
 *  Initialization USB MSC Stop
 *
 *  @return >=0 success, <0 failure
 */
extern INT32 ApplibUsbMsc_Stop(void);

/**
 *  @brief Initialization USB AMAGE start
 *
 *  Initialization USB AMAGE start
 *
 *  @return >=0 success, <0 failure
 */
extern INT32 ApplibUsbAmage_Start(void);

/**
 *  @brief Init USB custom device info
 *
 *  Init USB custom device info
 *
 *  @param [in] class Usb class
 *
 *  @return >=0 success, <0 failure
 */
extern UINT32 AppLibUSB_Custom_SetDevInfo(APPLIB_USB_DEVICE_CLASS_e class);
#ifdef CONFIG_APP_ARD
extern UINT32 AppLibUSB_GetVbusStatus(void);
#endif
__END_C_PROTO__
/**
 * @}
 */
#endif /* APPLIB_USB_H_ */
