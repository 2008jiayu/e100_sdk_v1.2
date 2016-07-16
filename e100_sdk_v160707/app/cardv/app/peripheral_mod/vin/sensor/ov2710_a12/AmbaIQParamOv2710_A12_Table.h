/**
 * @file app/cardv/app/peripheral_mode/win/sensor/AmbaIQParamOv2710_A12.c
 *
 * Implementation of Ov2710 related settings.
 *
 * History:
 *    2015/02/06 - [Jim Meng] created file
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

#ifndef __AMBA_IQPARAM_OV2710_A12_TABLE_H__
#define __AMBA_IQPARAM_OV2710_A12_TABLE_H__

#include <imgproc/AmbaImg_Adjustment_A12.h>
#include <imgproc/AmbaImg_AaaDef.h>
#include <imgproc/AmbaImg_Adjustment_Def.h>
#include <3a/iqparam/ApplibIQParamHandler.h>
extern IMG_PARAM_s AmbaIQParamOv2710ImageParam;
extern AAA_PARAM_s AmbaIQParamOv2710DefParams;
extern ADJ_TABLE_PARAM_s AmbaIQParamOv2710AdjTableParam;
extern ADJ_VIDEO_PARAM_s AmbaIQParamOv2710AdjVideoPc00;
extern ADJ_VIDEO_PARAM_s AmbaIQParamOv2710AdjVideoPc01;
extern ADJ_VIDEO_HISO_PARAM_s AmbaIQParamOv2710AdjVideoHIso00;
extern ADJ_STILL_FAST_LISO_PARAM_S AmbaIQParamOv2710AdjStillLIso00;
extern ADJ_STILL_FAST_LISO_PARAM_S AmbaIQParamOv2710AdjStillLIso01;
extern ADJ_PHOTO_PARAM_s AmbaIQParamOv2710AdjPhotoPreview00;
extern ADJ_PHOTO_PARAM_s AmbaIQParamOv2710AdjPhotoPreview01;
extern ADJ_STILL_HISO_PARAM_s AmbaIQParamOv2710AdjStillHIso00;
extern ADJ_STILL_HISO_PARAM_s AmbaIQParamOv2710AdjStillHIso01;
extern ADJ_STILL_IDX_INFO_s AmbaIQParamOv2710StillParam;
extern ADJ_VIDEO_IDX_INFO_s AmbaIQParamOv2710VideoParam;
extern SCENE_DATA_s SceneDataS01Ov2710A12[8];
extern SCENE_DATA_s SceneDataS02Ov2710A12[8];
extern SCENE_DATA_s SceneDataS03Ov2710A12[8];
extern SCENE_DATA_s SceneDataS04Ov2710A12[8];
extern SCENE_DATA_s SceneDataS05Ov2710A12[8];
extern DE_PARAM_s DeVideoParamOv2710A12;
extern DE_PARAM_s DeStillParamOv2710A12;
//extern CALIBRATION_PARAM_s AmbaIQParamImx117CalibParams;

COLOR_TABLE_PATH_s GCcTableOv2710[10] = {
    { 0,
      "VideoCc0_Ar0230",
      "VideoCc1_Ar0230",
      "VideoCc2_Ar0230",
      "VideoCc3_Ar0230",
      "VideoCc4_Ar0230",
      "StillCc0_Ar0230",
      "StillCc1_Ar0230",
      "StillCc2_Ar0230",
      "StillCc3_Ar0230",
      "StillCc4_Ar0230",
    },
    { 1,
      "cc3d_cc_bw_gamma_lin_video_Ar0230.bin",
      "cc3d_cc_bw_gamma_lin_video_Ar0230.bin",
      "cc3d_cc_bw_gamma_lin_video_Ar0230.bin",
      "cc3d_cc_bw_gamma_lin_video_Ar0230.bin",
      "cc3d_cc_bw_gamma_lin_video_Ar0230.bin",
      "cc3d_cc_bw_gamma_lin_still_Ar0230.bin",
      "cc3d_cc_bw_gamma_lin_still_Ar0230.bin",
      "cc3d_cc_bw_gamma_lin_still_Ar0230.bin",
      "cc3d_cc_bw_gamma_lin_still_Ar0230.bin",
      "cc3d_cc_bw_gamma_lin_still_Ar0230.bin",
    },
    { -1 },
};


IP_TABLE_PATH_s GVideoAdjTablePathOv2710[ADJ_VIDEO_TABLE_VALID_NO] = {
    "adj_video_default_00_Ar0230",
    "adj_video_default_01_Ar0230",
};

IP_TABLE_PATH_s GVideoHIsoAdjTablePathOv2710[ADJ_VIDEO_HISO_TABLE_VALID_NO] = {
    "adj_hiso_video_default_00_Ar0230",
//    "adj_hiso_video_default_01",
};


IP_TABLE_PATH_s GPhotoAdjTablePathOv2710[ADJ_PHOTO_TABLE_VALID_NO] = {
    "adj_photo_default_00_Ar0230",
    "adj_photo_default_01_Ar0230",
};

IP_TABLE_PATH_s GStillLIsoAdjTablePathOv2710[ADJ_STILL_LISO_TABLE_VALID_NO] = {
    "adj_still_default_00_Ar0230",
    "adj_still_default_01_Ar0230",
};

IP_TABLE_PATH_s GStillHIsoAdjTablePathOv2710[ADJ_STILL_HISO_TABLE_VALID_NO] = {
    "adj_hiso_still_default_00_Ar0230",
//    "adj_hiso_still_default_01_Ar0230",
};

IP_TABLE_PATH_s GSceneDataTablePathOv2710[SCENE_TABLE_VALID_NO] = {
    "scene_data_s01_Ar0230",
    "scene_data_s02_Ar0230",
    "scene_data_s03_Ar0230",
    "scene_data_s04_Ar0230",
    "scene_data_s05_Ar0230",
};

IP_TABLE_PATH_s GImgAdjTablePathOv2710 ={
    "img_default_Ar0230"
};

IP_TABLE_PATH_s GAaaAdjTablePathOv2710[AAA_TABLE_VALID_NO] ={
    "aaa_default_00_Ar0230",
    "aaa_default_01_Ar0230"
};

IP_TABLE_PATH_s GStillIdxInfoAdjTablePathOv2710 ={
    "adj_still_idx_Ar0230"
};

IP_TABLE_PATH_s GVideoIdxInfoAdjTablePathOv2710 ={
    "adj_video_idx_Ar0230"
};

IP_TABLE_PATH_s GDeVideoTablePathOv2710 ={
    "de_default_video_Ar0230"
};

IP_TABLE_PATH_s GDeStillTablePathOv2710 ={
    "de_default_still_Ar0230"
};
IP_TABLE_PATH_s GAdjTablePathOv2710 ={
    "adj_table_param_default_Ar0230"
};

IP_TABLE_ADDR_s GImgAddrOv2710;
IP_TABLE_ADDR_s GAaaAddrOv2710;
IP_TABLE_ADDR_s GAdjTableOv2710;
IP_TABLE_ADDR_s GStillIdxInfoOv2710;
IP_TABLE_ADDR_s GVideoIdxInfoOv2710;
IP_TABLE_ADDR_s GDeVideoOv2710;
IP_TABLE_ADDR_s GDeStillOv2710;

#endif
