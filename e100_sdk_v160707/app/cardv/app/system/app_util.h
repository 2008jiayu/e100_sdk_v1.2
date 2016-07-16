/**
 * @file app/connected/app/system/app_util.h
 *
 * Header of Demo application utility
 *
 * History:
 *    2013/08/16 - [Martin Lai] created file
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
#ifndef APP_SYS_UTIL_H_
#define APP_SYS_UTIL_H_

#include <framework/appdefines.h>
#ifdef CONFIG_APP_ARD
#include "app_fw_updater.h"
#endif

__BEGIN_C_PROTO__

extern int AppUtil_Init(void);
extern int AppUtil_ReadyCheck(UINT32 param);
extern int AppUtil_BusyCheck(UINT32 param);
extern int AppUtil_SwitchApp(int appId);
extern int AppUtil_SwitchMode(UINT32 param);
extern int AppUtil_GetStartApp(UINT32 param);
extern int AppUtil_CheckCardParam(UINT32 param);
extern int AppUtil_GetVoutMode(int param);
extern int AppUtil_StatusInit(void);
extern int AppUtil_PollingAllSlots(void);
extern int AppUtil_AsciiToUnicode(char *ascStr, UINT16 *uniStr);
#ifdef CONFIG_APP_ARD
extern int AppUtil_UsbChargeCheckingSet(UINT32 enable);
extern void AppUtil_CheckBatteryVoltageLevel(void);
extern void AppUtil_BatteryVoltageMonitor(void);
extern int AppUtil_AutoPowerOffInit(int param);
extern void AppUtil_PowerOffHandler(int eid);
extern int AppUtil_CheckSystemTestModeAutoPowerOff(void);
extern int AppUtil_CheckSystemTestModeLoopRec(void);
extern int AppUtil_SystemTestModePowerOff(void);
extern int AppUtil_SystemTestModeReboot(void);
extern int AppUtil_SystemTestModeStartRecording(void);
extern int AppUtil_GetSystemTestMode(void);
extern void AppUtil_Gps_Detect_Monitor(void);

extern void cardv_app_screen_save_timer_init(void);
extern void cardv_app_screen_handler_timer(int on);
extern void cardv_app_screen_handler(int on);
extern void cardv_app_screen_lock(void);
extern void cardv_app_screen_unlock(void);
extern int cardv_app_screen_check_lock(void);
extern void cardv_app_delay_poweroff_handler_timer(int on);
extern void cardv_app_delay_poweroff_init(void);
extern void app_screen_save_set(int en);
extern void app_set_scr_status(int status);
extern int app_check_scr_status(void);
extern int app_check_screen_status(void);
extern void app_set_menu_scr_status(int status);
extern int app_check_menu_scr_status(void);


#define LCD_SCR_TURN_OFF 0
#define LCD_SCR_TURN_ON 1

extern int AppUtil_GetRsndStorageIdleMsg(void);
extern void AppUtil_SetRsndStorageIdleMsg(int msg);
extern int AppUtil_SystemIsEncMode(void);
extern void AppUtil_BatteryVoltagePrint(void);
#endif
__END_C_PROTO__

#endif /* APP_SYS_UTIL_H_ */
