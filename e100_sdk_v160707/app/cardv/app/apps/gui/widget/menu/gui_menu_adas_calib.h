/**
  * @file src/app/apps/gui/widget/menu/sportcam/gui_menu_adas_calib.h
  *
  * Header for ADAS Calibration Menu GUI flow
  *
  * History:
  *    2013/11/22 - [Martin Lai] created file
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

#ifndef APP_GUI_WIDGET_MENU_ADAS_CALIB_H_
#define APP_GUI_WIDGET_MENU_ADAS_CALIB_H_

__BEGIN_C_PROTO__

/*************************************************************************
 * ADAS calibration menu GUI definitions
 ************************************************************************/
typedef enum _GUI_MENU_ADAS_CALIB_CMD_ID_e_ {
    GUI_FLUSH = 0,
    GUI_HIDE_ALL,
    GUI_HIDE_ADAS_CALIB_SKY,
    GUI_HIDE_ADAS_CALIB_HOOD,
    GUI_SHOW_ADAS_CALIB_SKY,
    GUI_SHOW_ADAS_CALIB_HOOD,
    GUI_UPDATE_ADAS_CALIB_SKY,
    GUI_UPDATE_ADAS_CALIB_HOOD
} GUI_MENU_ADAS_CALIB_CMD_ID_e;


/*************************************************************************
 * ADAS calibration menu Widget GUI functions
 ************************************************************************/
extern int gui_menu_adas_calib_func(UINT32 guiCmd, UINT32 param1, UINT32 param2);

__END_C_PROTO__

#endif /* APP_GUI_WIDGET_MENU_ADAS_CALIB_H_ */
