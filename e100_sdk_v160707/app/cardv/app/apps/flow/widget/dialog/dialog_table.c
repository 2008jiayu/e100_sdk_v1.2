/**
  * @file src/app/apps/flow/widget/dialog/connectedcam/dialog_table.c
  *
  *  Implementation of Dialog table
  *
  * History:
  *    2013/11/22 - [Martin Lai] created file
  *
  *
 * Copyright (c) 2015 Ambarella, Inc.
 *
 * This file and its contents (��Software��) are protected by intellectual property rights
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

#include <apps/gui/resource/gui_settle.h>
#include "dialog_table.h"

UINT32 dialog_subject_str_tbl[DIALOG_SUB_NUM] = {
    STR_DEL_YES_OR_NO,    // DIALOG_SUB_DEL
    STR_DEL_ALL_YES_OR_NO,
    STR_MVRECOVER_YES_OR_NO,
    STR_MVRECOVER_OK,
    STR_MVRECOVER_FAIL,
    STR_FWUPDATE_YES_OR_NO,
    STR_FWUPDATE_SUCCESS,
    STR_FWUPDATE_FAIL,
    STR_CALIBUPDATE_YES_OR_NO,
    STR_CALIBUPDATE_SUCCESS,
    STR_CALIBUPDATE_FAIL,
    STR_FORMAT_CARD_YES_OR_NO,
    STR_FORMAT_CARD_CAUTION,
    STR_FORMAT_CARD_NO_OPTIMUM,
    STR_FORMAT_CARD_FMT_UNKNOWN,
    STR_FORMAT_CARD_FAILED,
    STR_FORMAT_CARD_OK,
    STR_DEFAULT_SETTING_YES_OR_NO,
    STR_DEFAULT_SETTING_REBOOT,
    STR_SAVE_ROTATION,
    STR_REDEYE_REMOVAL_YES_OR_NO,
    STR_DYNAMIC_LIGHTING_YES_OR_NO,
    STR_VIDEO_EDIT_CROP_Y_N,
    STR_VIDEO_EDIT_PARTIAL_DEL_Y_N,
    STR_VIDEO_EDIT_DIVIDE_Y_N,
    STR_VIDEO_EDIT_MERGE_Y_N,
    STR_VIDEO_EDIT_OP_DONE,
    STR_VIDEO_EDIT_OP_FAIL,
    STR_VIDEO_EDIT_LIST_FULL,
    STR_VIDEO_EDIT_DISK_FULL,
    STR_VIDEO_EDIT_UNKNOWN_FORMAT,
    STR_VIDEO_EDIT_ILLEGAL_INTERVAL,
    STR_VIDEO_EDIT_WRONG_RESOLUTION,
    STR_VIDEO_EDIT_MEDIA_TYPE_MISMATCH,
    STR_VIDEO_EDIT_VIDEO_MISMATCH,
    STR_VIDEO_EDIT_AUDIO_MISMATCH,
    STR_VIDEO_EDIT_FUNC_NOT_AVAILABLE,
    STR_PHOTO_EDIT_NO_ORIENTATION,
    STR_PLIST_ADD_FILE_YES_OR_NO,
    STR_PLIST_ADD_FILE_SUCCESS,
    STR_PLIST_ADD_FILE_FAIL,
    STR_PLIST_CLEAR_PICKUP_FILE_YES_OR_NO,
    STR_UPLOAD_YES_OR_NO,
    STR_UPLOAD_TIME_OUT,
    STR_UNSUPPORT_FORMAT
};