/**
  * @file src/app/apps/flow/widget/menu/cardv/menu_factory.h
  *
  * Header of factory mode-related Menu Items
  *
  * History:
  *    2015/2/5 - [Emmett Xie] created file
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

#ifndef APP_WIDGET_MENU_FACTORY_H_
#define APP_WIDGET_MENU_FACTORY_H_

__BEGIN_C_PROTO__

/*************************************************************************
 * factory menu definitions
 ************************************************************************/
typedef enum _MENU_FACTORY_ITEM_ID_e_ {
    MENU_FACTORY_VERSION,
    MENU_FACTORY_MANUAL_CALI,
    MENU_FACTORY_GSENSOR_TEST,
    MENU_FACTORY_4G_TEST,
    MENU_FACTORY_GPS_TEST,
    MENU_FACTORY_REC_MODE,
    MENU_FACTORY_ITEM_NUM
} MENU_FACTORY_ITEM_ID_e;

typedef enum _MENU_FACTORY_MANUAL_CALI_SEL_ID_e_ {
    MENU_FACTORY_MANUAL_CALI_BLACK_LEVEL_CORRECTION = 0,
    MENU_FACTORY_MANUAL_CALI_BAD_PIXEL_CORRECTION,
    MENU_FACTORY_MANUAL_CALI_CHROMA_ABERRATION,
    MENU_FACTORY_MANUAL_CALI_WARP,
    MENU_FACTORY_MANUAL_CALI_VIGNETTE,
    MENU_FACTORY_MANUAL_CALI_WB_GOLDEN_SAMPLE,
    MENU_FACTORY_MANUAL_CALI_WBH,
    MENU_FACTORY_MANUAL_CALI_WBL,
    MENU_FACTORY_MANUAL_CALI_SEL_NUM
} MENU_FACTORY_MANUAL_CALI_SEL_ID_e;

typedef enum _MENU_FACTORY_REC_MODE_SEL_ID_e_ {
    MENU_FACTORY_REC_MODE_VIDEO_MODE = 0,
    MENU_FACTORY_REC_MODE_STILL_MODE,
    MENU_FACTORY_REC_MODE_SEL_NUM
} MENU_FACTORY_REC_MODE_SEL_ID_e;

__END_C_PROTO__

#endif /* APP_WIDGET_MENU_factory_H_ */
