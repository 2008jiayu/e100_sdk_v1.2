/**
 *  @FileName       :: AmbaIQParamOv4689hdr_A12_Table.h
 *
 *  @Description    :: Imx144 Image IQ parameters tables
 *
 *  @History        ::
 *      Date        Name        Comments
 *      01/08/2013  Eddie Chen  Created
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

#ifndef __AMBA_IQPARAM_OV4689HDR_A12_TABLE_H__
#define __AMBA_IQPARAM_OV4689HDR_A12_TABLE_H__
#include <imgproc/AmbaImg_AaaDef.h>
#include <imgproc/AmbaImg_Adjustment_Def.h>
#include <3a/iqparam/ApplibIQParamHandler.h>


COLOR_TABLE_PATH_s GCcTableOV4689hdr[10] = {
    { 0,
      "VideoCc0_Ov4689hdr",
      "VideoCc1_Ov4689hdr",
      "VideoCc2_Ov4689hdr",
      "VideoCc3_Ov4689hdr",
      "VideoCc4_Ov4689hdr",
      "StillCc0_Ov4689hdr",
      "StillCc1_Ov4689hdr",
      "StillCc2_Ov4689hdr",
      "StillCc3_Ov4689hdr",
      "StillCc4_Ov4689hdr",
    },
    { 1,
      "cc3d_cc_bw_gamma_lin_video_Ov4689hdr",
      "cc3d_cc_bw_gamma_lin_video_Ov4689hdr",
      "cc3d_cc_bw_gamma_lin_video_Ov4689hdr",
      "cc3d_cc_bw_gamma_lin_video_Ov4689hdr",
      "cc3d_cc_bw_gamma_lin_video_Ov4689hdr",
      "cc3d_cc_bw_gamma_lin_still_Ov4689hdr",
      "cc3d_cc_bw_gamma_lin_still_Ov4689hdr",
      "cc3d_cc_bw_gamma_lin_still_Ov4689hdr",
      "cc3d_cc_bw_gamma_lin_still_Ov4689hdr",
      "cc3d_cc_bw_gamma_lin_still_Ov4689hdr",
    },
    { -1 },
};


IP_TABLE_PATH_s GVideoAdjTablePathOV4689hdr[ADJ_VIDEO_TABLE_VALID_NO] = {
    "adj_video_default_00_Ov4689hdr",
    "adj_video_default_01_Ov4689hdr",
};

IP_TABLE_PATH_s GVideoHIsoAdjTablePathOV4689hdr[ADJ_VIDEO_HISO_TABLE_VALID_NO] = {
    "adj_hiso_video_default_00_Ov4689hdr",
//    "adj_hiso_video_default_01",
};

IP_TABLE_PATH_s GPhotoAdjTablePathOV4689hdr[ADJ_PHOTO_TABLE_VALID_NO] = {
    "adj_photo_default_00_Ov4689hdr",
    "adj_photo_default_01_Ov4689hdr",
};

IP_TABLE_PATH_s GStillLIsoAdjTablePathOV4689hdr[ADJ_STILL_LISO_TABLE_VALID_NO] = {
    "adj_still_default_00_Ov4689hdr",
    "adj_still_default_01_Ov4689hdr",
};

IP_TABLE_PATH_s GStillHIsoAdjTablePathOV4689hdr[ADJ_STILL_HISO_TABLE_VALID_NO] = {
    "adj_hiso_still_default_00_Ov4689hdr",
//    "adj_hiso_still_default_01_Ov4689hdr",
};

IP_TABLE_PATH_s GSceneDataTablePathOV4689hdr[SCENE_TABLE_VALID_NO] = {
    "scene_data_s01_Ov4689hdr",
    "scene_data_s02_Ov4689hdr",
    "scene_data_s03_Ov4689hdr",
    "scene_data_s04_Ov4689hdr",
    "scene_data_s05_Ov4689hdr",
};

IP_TABLE_PATH_s GImgAdjTablePathOV4689hdr ={
    "img_default_Ov4689hdr"
};

IP_TABLE_PATH_s GAaaAdjTablePathOV4689hdr[AAA_TABLE_VALID_NO] ={
    "aaa_default_00_Ov4689hdr",
    "aaa_default_01_Ov4689hdr"
};

IP_TABLE_PATH_s GStillIdxInfoAdjTablePathOV4689hdr ={
    "adj_still_idx_Ov4689hdr"
};

IP_TABLE_PATH_s GVideoIdxInfoAdjTablePathOV4689hdr ={
    "adj_video_idx_Ov4689hdr"
};

IP_TABLE_PATH_s GDeVideoTablePathOV4689hdr ={
    "de_default_video_Ov4689hdr"
};

IP_TABLE_PATH_s GDeStillTablePathOV4689hdr ={
    "de_default_still_Ov4689hdr"
};
IP_TABLE_PATH_s GAdjTablePathOV4689hdr ={
    "adj_table_param_default_Ov4689hdr"
};

IP_TABLE_ADDR_s GImgAddrOV4689hdr;
IP_TABLE_ADDR_s GAaaAddrOV4689hdr;
IP_TABLE_ADDR_s GAdjTableOV4689hdr;
IP_TABLE_ADDR_s GStillIdxInfoOV4689hdr;
IP_TABLE_ADDR_s GVideoIdxInfoOV4689hdr;
IP_TABLE_ADDR_s GDeVideoOV4689hdr;
IP_TABLE_ADDR_s GDeStillOV4689hdr;

#endif