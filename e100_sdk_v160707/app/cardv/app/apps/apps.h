/**
  * @file src/app/apps/apps.h
  *
  * Header of Applications
  *
  * History:
  *    2013/07/09 - [Martin Lai] created file
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

#ifndef APP_APPLICATIONS_H_
#define APP_APPLICATIONS_H_

#include <framework/appdefines.h>
#include <framework/apphmi.h>
#include <framework/appmgt.h>
#include <bsp.h>
#include <applib.h>

__BEGIN_C_PROTO__

/* App entity declarations */
extern APP_APP_s app_main;
extern APP_APP_s app_rec_dsc;
extern APP_APP_s app_rec_dv;
extern APP_APP_s app_rec_sport_dv;
extern APP_APP_s app_rec_cam; /* CarDV REC */
extern APP_APP_s app_rec_sound;
extern APP_APP_s app_pb_multi;
extern APP_APP_s app_pb_multi2;
extern APP_APP_s app_pb_photo;
extern APP_APP_s app_pb_video;
extern APP_APP_s app_pb_sound;
extern APP_APP_s app_pb_transcoder;
extern APP_APP_s app_edtr_video;
extern APP_APP_s app_edtr_video_merge;
extern APP_APP_s app_thumb_motion;
extern APP_APP_s app_thumb_motion2;
extern APP_APP_s app_usb_msc;
extern APP_APP_s app_usb_pccam;
extern APP_APP_s app_usb_amage;
extern APP_APP_s app_usb_stillimage;
extern APP_APP_s app_usb_pictb;
extern APP_APP_s app_usb_pictb_print;
extern APP_APP_s app_misc;
extern APP_APP_s app_misc_mvrecover;
extern APP_APP_s app_misc_calibupdate;
extern APP_APP_s app_misc_fwupdate;
extern APP_APP_s app_misc_formatcard;
extern APP_APP_s app_misc_defsetting;
extern APP_APP_s app_misc_poweroff;
extern APP_APP_s app_misc_powersaving;
extern APP_APP_s app_misc_qrconfig;
extern APP_APP_s app_misc_wifistatus;
extern APP_APP_s app_misc_webupload;
extern APP_APP_s app_test_ddr_tuner;
extern APP_APP_s app_test_calib;
extern APP_APP_s app_hdmi_test;
#ifdef CONFIG_APP_ARD
extern APP_APP_s app_misc_logo;
#endif

#define APPS_NUM   (30)
extern int APP_MAIN;
extern int APP_REC_DSC;
extern int APP_REC_DV;
extern int APP_REC_SPORT_DV;
extern int APP_REC_CAM; /* CarDV REC */
extern int APP_REC_SOUND;
extern int APP_REC_CAR_VIDEO;
extern int APP_PB_MULTI;
extern int APP_PB_PHOTO;
extern int APP_PB_VIDEO;
extern int APP_PB_SOUND;
extern int APP_PB_TRANSCODER;
extern int APP_EDTR_VIDEO;
extern int APP_EDTR_VIDEO_MERGE;
extern int APP_THUMB_MOTION;
extern int APP_USB_MSC;
extern int APP_USB_PCCAM;
extern int APP_USB_AMAGE;
extern int APP_USB_STILLIMAGE;
extern int APP_USB_PICTB;
extern int APP_USB_PICTB_PRINT;
extern int APP_MISC;
extern int APP_MISC_CALIBUPDATE;
extern int APP_MISC_MVRECOVER;
extern int APP_MISC_FWUPDATE;
extern int APP_MISC_FORMATCARD;
extern int APP_MISC_DEFSETTING;
extern int APP_MISC_POWEROFF;
extern int APP_MISC_POWERSAVING;
extern int APP_MISC_WIFISTATUS;
extern int APP_MISC_WEBUPLOAD;
extern int APP_MISC_QRCONFIG;
extern int APP_TEST_DDR_TUNER;
extern int APP_TEST_CALIB;
extern int APP_HDMI_TEST;
#ifdef CONFIG_APP_ARD
extern int APP_MISC_LOGO;
extern int APP_MISC_POLICE_ID;
#endif
/**
 * Turn on this define if you want to run the HDMI test application.
 * Remote control button:
 *      1: 1080P                    2: 1080PHalf                3: 1080I
 *      4: 1080P24                 5: 720P                      6: 720PHalf
 *      7: 720P24                   8: SDP                       9: SDI
 *      100: RGB444 8Bits       0: YCC444 8Bits         Return: YCC422 12Bits
 */
//#define ENABLE_HDMI_TEST

__END_C_PROTO__

#endif /* APP_APPLICATIONS_H_ */
