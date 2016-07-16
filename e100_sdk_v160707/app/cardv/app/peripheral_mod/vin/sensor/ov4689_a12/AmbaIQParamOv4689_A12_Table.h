/**
 * @file app/connected/app/peripheral_mode/win/sensor/AmbaIQParamOv4689_A12_Table.h
 *
 * Implementation of SONY OV4689 related settings.
 *
 * History:
 *    2013/01/08 - [Eddie Chen] created file
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

#ifndef __AMBA_IQPARAM_OV4689_A12_TABLE_H__
#define __AMBA_IQPARAM_OV4689_A12_TABLE_H__


#include <imgproc/AmbaImg_AaaDef.h>
#include <imgproc/AmbaImg_Adjustment_Def.h>
#include <3a/iqparam/ApplibIQParamHandler.h>
extern IMG_PARAM_s AmbaIQParamOv4689ImageParam;
extern AAA_PARAM_s AmbaIQParamOv4689DefParams;
extern ADJ_TABLE_PARAM_s AmbaIQParamOv4689AdjTableParam;
extern ADJ_VIDEO_PARAM_s AmbaIQParamOv4689AdjVideoPc00;
extern ADJ_VIDEO_PARAM_s AmbaIQParamOv4689AdjVideoPc01;
extern ADJ_VIDEO_HISO_PARAM_s AmbaIQParamOv4689AdjVideoHIso00;
extern ADJ_STILL_FAST_LISO_PARAM_S AmbaIQParamOv4689AdjStillLIso00;
extern ADJ_STILL_FAST_LISO_PARAM_S AmbaIQParamOv4689AdjStillLIso01;
extern ADJ_PHOTO_PARAM_s AmbaIQParamOv4689AdjPhotoPreview00;
extern ADJ_PHOTO_PARAM_s AmbaIQParamOv4689AdjPhotoPreview01;
extern ADJ_STILL_HISO_PARAM_s AmbaIQParamOv4689AdjStillHIso00;
extern ADJ_STILL_HISO_PARAM_s AmbaIQParamOv4689AdjStillHIso01;
extern ADJ_STILL_IDX_INFO_s AmbaIQParamOv4689StillParam;
extern ADJ_VIDEO_IDX_INFO_s AmbaIQParamOv4689VideoParam;
extern SCENE_DATA_s SceneDataS01Ov4689A12[8];
extern SCENE_DATA_s SceneDataS02Ov4689A12[8];
extern SCENE_DATA_s SceneDataS03Ov4689A12[8];
extern SCENE_DATA_s SceneDataS04Ov4689A12[8];
extern SCENE_DATA_s SceneDataS05Ov4689A12[8];
extern DE_PARAM_s DeVideoParamOv4689A12;
extern DE_PARAM_s DeStillParamOv4689A12;
//extern CALIBRATION_PARAM_s AmbaIQParamOv4689CalibParams;

COLOR_TABLE_PATH_s GCcTableOV4689[10] = {
    { 0,
      "VideoCc0",
      "VideoCc1",
      "VideoCc2",
      "VideoCc3",
      "VideoCc4",
      "StillCc0",
      "StillCc1",
      "StillCc2",
      "StillCc3",
      "StillCc4",
    },
    { 1,
      "cc3d_cc_bw_gamma_lin_video.bin",
      "cc3d_cc_bw_gamma_lin_video.bin",
      "cc3d_cc_bw_gamma_lin_video.bin",
      "cc3d_cc_bw_gamma_lin_video.bin",
      "cc3d_cc_bw_gamma_lin_video.bin",
      "cc3d_cc_bw_gamma_lin_still.bin",
      "cc3d_cc_bw_gamma_lin_still.bin",
      "cc3d_cc_bw_gamma_lin_still.bin",
      "cc3d_cc_bw_gamma_lin_still.bin",
      "cc3d_cc_bw_gamma_lin_still.bin",
    },
    { -1 },
};


IP_TABLE_PATH_s GVideoAdjTablePathOV4689[ADJ_VIDEO_TABLE_VALID_NO] = {
    "adj_video_default_00_Ov4689",
    "adj_video_default_01_Ov4689",
};

IP_TABLE_PATH_s GVideoHIsoAdjTablePathOV4689[ADJ_VIDEO_HISO_TABLE_VALID_NO] = {
    "adj_hiso_video_default_00_Ov4689",
//    "adj_hiso_video_default_01",
};

IP_TABLE_PATH_s GPhotoAdjTablePathOV4689[ADJ_PHOTO_TABLE_VALID_NO] = {
    "adj_photo_default_00_Ov4689",
    "adj_photo_default_01_Ov4689",
};

IP_TABLE_PATH_s GStillLIsoAdjTablePathOV4689[ADJ_STILL_LISO_TABLE_VALID_NO] = {
    "adj_still_default_00_Ov4689",
    "adj_still_default_01_Ov4689",
};

IP_TABLE_PATH_s GStillHIsoAdjTablePathOV4689[ADJ_STILL_HISO_TABLE_VALID_NO] = {
    "adj_hiso_still_default_00_Ov4689",
//    "adj_hiso_still_default_01_Ov4689",
};

IP_TABLE_PATH_s GSceneDataTablePathOV4689[SCENE_TABLE_VALID_NO] = {
    "scene_data_s01_Ov4689",
    "scene_data_s02_Ov4689",
    "scene_data_s03_Ov4689",
    "scene_data_s04_Ov4689",
    "scene_data_s05_Ov4689",
};

IP_TABLE_PATH_s GImgAdjTablePathOV4689 ={
    "img_default_Ov4689"
};

IP_TABLE_PATH_s GAaaAdjTablePathOV4689[AAA_TABLE_VALID_NO] ={
    "aaa_default_00_Ov4689",
    "aaa_default_01_Ov4689"
};

IP_TABLE_PATH_s GStillIdxInfoAdjTablePathOV4689 ={
    "adj_still_idx_Ov4689"
};

IP_TABLE_PATH_s GVideoIdxInfoAdjTablePathOV4689 ={
    "adj_video_idx_Ov4689"
};

IP_TABLE_PATH_s GDeVideoTablePathOV4689 ={
    "de_default_video_Ov4689"
};

IP_TABLE_PATH_s GDeStillTablePathOV4689 ={
    "de_default_still_Ov4689"
};
IP_TABLE_PATH_s GAdjTablePathOV4689 ={
    "adj_table_param_default_Ov4689"
};

IP_TABLE_ADDR_s GImgAddrOV4689;
IP_TABLE_ADDR_s GAaaAddrOV4689;
IP_TABLE_ADDR_s GAdjTableOV4689;
IP_TABLE_ADDR_s GStillIdxInfoOV4689;
IP_TABLE_ADDR_s GVideoIdxInfoOV4689;
IP_TABLE_ADDR_s GDeVideoOV4689;
IP_TABLE_ADDR_s GDeStillOV4689;

#endif
