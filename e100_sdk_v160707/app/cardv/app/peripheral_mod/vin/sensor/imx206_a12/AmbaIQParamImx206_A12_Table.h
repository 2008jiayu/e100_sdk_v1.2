/**
 * @file app/connected/app/peripheral_mode/win/sensor/AmbaIQParamImx206_A12.c
 *
 * Implementation of SONY IMX206 related settings.
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

#ifndef __AMBA_IQPARAM_IMX206_A12_TABLE_H__
#define __AMBA_IQPARAM_IMX206_A12_TABLE_H__

#include <imgproc/AmbaImg_Adjustment_A12.h>
#include <imgproc/AmbaImg_AaaDef.h>
#include <imgproc/AmbaImg_Adjustment_Def.h>

extern IMG_PARAM_s AmbaIQParamImx206ImageParam;
extern AAA_PARAM_s AmbaIQParamImx206DefParams;
extern ADJ_TABLE_PARAM_s AmbaIQParamImx206AdjTableParam;
extern ADJ_VIDEO_PARAM_s AmbaIQParamImx206AdjVideoPc00;
extern ADJ_VIDEO_PARAM_s AmbaIQParamImx206AdjVideoPc01;
extern ADJ_VIDEO_PARAM_s AmbaIQParamImx206AdjVideoHIso00;
extern ADJ_STILL_FAST_LISO_PARAM_S AmbaIQParamImx206AdjStillLIso00;
extern ADJ_STILL_FAST_LISO_PARAM_S AmbaIQParamImx206AdjStillLIso01;
extern ADJ_PHOTO_PARAM_s AmbaIQParamImx206AdjPhotoPreview00;
extern ADJ_PHOTO_PARAM_s AmbaIQParamImx206AdjPhotoPreview01;
extern ADJ_STILL_HISO_PARAM_s AmbaIQParamImx206AdjStillHIso00;
extern ADJ_STILL_HISO_PARAM_s AmbaIQParamImx206AdjStillHIso01;
extern ADJ_STILL_IDX_INFO_s AmbaIQParamImx206StillParam;
extern ADJ_VIDEO_IDX_INFO_s AmbaIQParamImx206VideoParam;
extern SCENE_DATA_s SceneDataS01Imx206A12[8];
extern SCENE_DATA_s SceneDataS02Imx206A12[8];
extern SCENE_DATA_s SceneDataS03Imx206A12[8];
extern SCENE_DATA_s SceneDataS04Imx206A12[8];
extern SCENE_DATA_s SceneDataS05Imx206A12[8];
extern DE_PARAM_s DeVideoParamImx206A12;
extern DE_PARAM_s DeStillParamImx206A12;
//extern CALIBRATION_PARAM_s AmbaIQParamImx206CalibParams;

COLOR_TABLE_PATH_s GCcTableIMX206[10] = {
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


IP_TABLE_PATH_s GVideoAdjTablePathIMX206[ADJ_VIDEO_TABLE_VALID_NO] = {
    "adj_video_default_00_Imx206",
    "adj_video_default_01_Imx206",
};

IP_TABLE_PATH_s GVideoHIsoAdjTablePathIMX206[ADJ_VIDEO_HISO_TABLE_VALID_NO] = {
    "adj_hiso_video_default_00_Imx206",
//    "adj_hiso_video_default_01_Imx206",
};


IP_TABLE_PATH_s GPhotoAdjTablePathIMX206[ADJ_PHOTO_TABLE_VALID_NO] = {
    "adj_photo_default_00_Imx206",
    "adj_photo_default_01_Imx206",
};

IP_TABLE_PATH_s GStillLIsoAdjTablePathIMX206[ADJ_STILL_LISO_TABLE_VALID_NO] = {
    "adj_still_default_00_Imx206",
    "adj_still_default_01_Imx206",
};

IP_TABLE_PATH_s GStillHIsoAdjTablePathIMX206[ADJ_STILL_HISO_TABLE_VALID_NO] = {
    "adj_hiso_still_default_00_Imx206",
//    "adj_hiso_still_default_01_Imx206",
};

IP_TABLE_PATH_s GSceneDataTablePathIMX206[SCENE_TABLE_VALID_NO] = {
    "scene_data_s01_Imx206",
    "scene_data_s02_Imx206",
    "scene_data_s03_Imx206",
    "scene_data_s04_Imx206",
    "scene_data_s05_Imx206",
};

IP_TABLE_PATH_s GImgAdjTablePathIMX206 ={
    "img_default_Imx206"
};

IP_TABLE_PATH_s GAaaAdjTablePathIMX206[AAA_TABLE_VALID_NO]  ={
    "aaa_default_00_Imx206",
    "aaa_default_01_Imx206"
};

IP_TABLE_PATH_s GStillIdxInfoAdjTablePathIMX206 ={
    "adj_still_idx_Imx206"
};

IP_TABLE_PATH_s GVideoIdxInfoAdjTablePathIMX206 ={
    "adj_video_idx_Imx206"
};

IP_TABLE_PATH_s GDeVideoTablePathIMX206 ={
    "de_default_video_Imx206"
};

IP_TABLE_PATH_s GDeStillTablePathIMX206 ={
    "de_default_still_Imx206"
};
IP_TABLE_PATH_s GAdjTablePathIMX206 ={
    "adj_table_param_default_Imx206"
};


#endif
