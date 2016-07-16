/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaImg_Impl_Cmd.h
 *
 *  @Copyright      :: Copyright (C) 2013 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Image Processing Function Implementation Command Definition
 *
\*-------------------------------------------------------------------------------------------------------------------*/

#ifndef __AMBA_IMG_IMPL_CMD_H__
#define __AMBA_IMG_IMPL_CMD_H__

#include "AmbaDataType.h"
extern int AmbaImg_Proc_Cmd(UINT32 cmd, UINT32 param1, UINT32 param2, UINT32 param3);

/* Image Proc Command */
#define MW_IP_CMD(x)                            (((UINT32) 0x0000 << 24) + 0x0000 + (x))

/* 3A Statistics */
#define MW_IP_SET_CFA_3A_STAT               MW_IP_CMD(0x001)
#define MW_IP_GET_CFA_3A_STAT               MW_IP_CMD(0x002)
#define MW_IP_SET_RGB_3A_STAT               MW_IP_CMD(0x003)
#define MW_IP_GET_RGB_3A_STAT               MW_IP_CMD(0x004)
#define MW_IP_GET_CFA_3A_STAT_ADDR          MW_IP_CMD(0x005)
#define MW_IP_GET_RGB_3A_STAT_ADDR          MW_IP_CMD(0x006)

/* 3A Control */
#define MW_IP_SET_AAA_OP_INFO               MW_IP_CMD(0xA00)
#define MW_IP_GET_AAA_OP_INFO               MW_IP_CMD(0xA01)
#define MW_IP_GET_SENSOR_STATUS             MW_IP_CMD(0xA02)
#define MW_IP_GET_FRAME_RATE                MW_IP_CMD(0xA03)
#define MW_IP_SET_FRAME_RATE                MW_IP_CMD(0xA63)
#define MW_IP_GET_CURR_FRAME_RATE           MW_IP_CMD(0xA64)
#define MW_IP_SET_CURR_FRAME_RATE           MW_IP_CMD(0xA65)

#define MW_IP_SET_DGAIN                     MW_IP_CMD(0xA04)
#define MW_IP_GET_DGAIN                     MW_IP_CMD(0xA05)
#define MW_IP_SET_GLOBAL_DGAIN              MW_IP_CMD(0xA06)
#define MW_IP_GET_GLOBAL_DGAIN              MW_IP_CMD(0xA07)
#define MW_IP_SET_WB_GAIN                   MW_IP_CMD(0xA08)
#define MW_IP_GET_WB_GAIN                   MW_IP_CMD(0xA09)


#define MW_IP_SET_PIPE_WB_GAIN              MW_IP_CMD(0xA60)
#define MW_IP_GET_PIPE_WB_GAIN              MW_IP_CMD(0xA61)

#define MW_IP_ADJ_VIDEO_HDR_INIT            MW_IP_CMD(0xA66)
#define MW_IP_ADJ_VIDEO_HDR_CONTROL         MW_IP_CMD(0xA67)
#define MW_IP_ADJ_VIDEO_HDR_CHK_DGAINMODE   MW_IP_CMD(0xA6C)

#define MW_IP_SET_GAMMA_VALUE               MW_IP_CMD(0xA68)
#define MW_IP_Get_GAMMA_VALUE               MW_IP_CMD(0xA69)

#define MW_IP_SET_FRO_WB_CHK                MW_IP_CMD(0xA6A)
#define MW_IP_Get_FRO_WB_CHK                MW_IP_CMD(0xA6B)

#define MW_IP_GET_ADJ_PARAMS_ADD            MW_IP_CMD(0xA0A)
#define MW_IP_SET_ADJ_PARAMS_ADD            MW_IP_CMD(0xA0B)
#define MW_IP_ADJ_VIDEO_CONTROL             MW_IP_CMD(0xA0C)
#define MW_IP_ADJ_STILL_CONTROL             MW_IP_CMD(0xA0D)
#define MW_IP_ADJ_AWBAE_CONTROL             MW_IP_CMD(0xA62)

#define MW_IP_SET_IMG_PARAM_ADD             MW_IP_CMD(0xA0E)
#define MW_IP_GET_IMG_PARAM_ADD             MW_IP_CMD(0xA0F)

#define MW_IP_SET_3A_STATUS                 MW_IP_CMD(0xA10)
#define MW_IP_GET_3A_STATUS                 MW_IP_CMD(0xA11)
#define MW_IP_SET_AE_STATUS                 MW_IP_CMD(0xA12)
#define MW_IP_SET_AWB_STATUS                MW_IP_CMD(0xA13)
#define MW_IP_SET_AF_STATUS                 MW_IP_CMD(0xA14)
#define MW_IP_GET_CURR_LV_NO                MW_IP_CMD(0xA15)
#define MW_IP_GET_CURR_WB_LUT_NO            MW_IP_CMD(0xA16)
#define MW_IP_GET_CURR_LV                   MW_IP_CMD(0xA17)


#define MW_IP_SET_AAA_PARAM                 MW_IP_CMD(0xA20)
#define MW_IP_GET_AAA_PARAM                 MW_IP_CMD(0xA21)

#define MW_IP_AMBA_AEAWBADJ_INIT            MW_IP_CMD(0xA22)
#define MW_IP_AMBA_AE_CONTROL               MW_IP_CMD(0xA23)
#define MW_IP_AMBA_AWB_CONTROL              MW_IP_CMD(0xA24)
#define MW_IP_AMBA_AWB_RAWENC_STILL         MW_IP_CMD(0xA25)
#define MW_IP_AMBA_AE_TEST_CMD              MW_IP_CMD(0xA26)
#define MW_IP_AMBA_AWB_TEST_CMD             MW_IP_CMD(0xA27)
#define MW_IP_AMBA_ADJ_TEST_CMD             MW_IP_CMD(0xA28)
#define MW_IP_AMBA_SC_TEST_CMD              MW_IP_CMD(0xA29)
#define MW_IP_AMBA_DE_TEST_CMD              MW_IP_CMD(0xA2A)
#define MW_IP_AMBA_POST_STILL               MW_IP_CMD(0xA2B)

#define MW_IP_AMBA_ADJ_GET_AE_TARGET        MW_IP_CMD(0xA2C)
#define MW_IP_AMBA_ADJ_GET_LE_RATIO         MW_IP_CMD(0xA2D)
#define MW_IP_AMBA_ADJ_GET_GAMMA_RATIO      MW_IP_CMD(0xA2E)
#define MW_IP_AMBA_ADJ_GET_AUTOKNEE         MW_IP_CMD(0xA2F)

#define MW_IP_GET_AE_TILE_INFO              MW_IP_CMD(0xA30)
#define MW_IP_GET_AWB_TILE_INFO             MW_IP_CMD(0xA31)

#define MW_IP_SET_ADJTABLE_PARAM            MW_IP_CMD(0xA32)
#define MW_IP_GET_ADJTABLE_PARAM            MW_IP_CMD(0xA33)
#define MW_IP_CHK_IQ_PARAM_VER              MW_IP_CMD(0xA34)

#define MW_IP_REGISTER_FUNC                 MW_IP_CMD(0xA40)
#define MW_IP_UNREGISTER_FUNC               MW_IP_CMD(0xA41)
#define MW_IP_GET_REG_FUNC                  MW_IP_CMD(0xA42)
#define MW_IP_PARAM_INIT                    MW_IP_CMD(0xA43)

#define MW_IP_SET_AE_INFO                   MW_IP_CMD(0xA50)
#define MW_IP_GET_AE_INFO                   MW_IP_CMD(0xA51)

#define MW_IP_SET_MEM_CTRLADDR              MW_IP_CMD(0xA52)
#define MW_IP_GET_MEM_CTRLADDR              MW_IP_CMD(0xA53)
#define MW_IP_SET_TOTAL_CH_COUNT            MW_IP_CMD(0xA54)
#define MW_IP_GET_TOTAL_CH_COUNT            MW_IP_CMD(0xA55)
#define MW_IP_SET_IP_MEM_INFO               MW_IP_CMD(0xA56)
#define MW_IP_GET_IP_MEM_INFO               MW_IP_CMD(0xA57)

#define MW_IP_GET_VIDEO_OB_ENABLE           MW_IP_CMD(0xA70)
#define MW_IP_SET_VIDEO_OB_ENABLE           MW_IP_CMD(0xA71)
#define MW_IP_GET_STILL_OB_ENABLE           MW_IP_CMD(0xA72)
#define MW_IP_SET_STILL_OB_ENABLE           MW_IP_CMD(0xA73)
#define MW_IP_GET_OB_CONT_MODE_ENABLE       MW_IP_CMD(0xA74)
#define MW_IP_SET_OB_CONT_MODE_ENABLE       MW_IP_CMD(0xA75)
#define MW_IP_GET_OB_SKIPFRAMES             MW_IP_CMD(0xA76)
#define MW_IP_SET_OB_SKIPFRAMES             MW_IP_CMD(0xA77)
#define MW_IP_GET_IMAGE_INIT_OB_FLAG        MW_IP_CMD(0xA78)
#define MW_IP_SET_IMAGE_INIT_OB_FLAG        MW_IP_CMD(0xA79)
#define MW_IP_COMPUTE_STILL_OB              MW_IP_CMD(0xA7A)
#define MW_IP_GET_OBINFO                    MW_IP_CMD(0xA7B)

#define MW_IP_SET_ENVIRONMENT_INFO          MW_IP_CMD(0xA7C)
#define MW_IP_GET_ENVIRONMENT_INFO          MW_IP_CMD(0xA7D)
#define MW_IP_AMBA_POST_STILL_AE            MW_IP_CMD(0xA7E)
#define MW_IP_AMBA_POST_STILL_AWB           MW_IP_CMD(0xA7F)


#define MW_IP_GET_MULTI_AE_ALGO_PARAMS           MW_IP_CMD(0xB00)
#define MW_IP_SET_MULTI_AE_ALGO_PARAMS           MW_IP_CMD(0xB01)
#define MW_IP_SET_MULTI_AE_CONTROL_CAPABILITY    MW_IP_CMD(0xB02)
#define MW_IP_GET_MULTI_AE_CONTROL_CAPABILITY    MW_IP_CMD(0xB03)
#define MW_IP_SET_MULTI_AE_DEF_SETTING           MW_IP_CMD(0xB04)
#define MW_IP_GET_MULTI_AE_DEF_SETTING           MW_IP_CMD(0xB05)
#define MW_IP_SET_MULTI_AE_EV_LUT                MW_IP_CMD(0xB06)
#define MW_IP_GET_MULTI_AE_EV_LUT                MW_IP_CMD(0xB07)
#define MW_IP_EXPS_TO_EV_IDX                     MW_IP_CMD(0xB08)
#define MW_IP_EXPS_TO_NF_IDX                     MW_IP_CMD(0xB09)

#define MW_IP_GET_MULTI_AWB_ALGO_PARAMS          MW_IP_CMD(0xB0A)
#define MW_IP_SET_MULTI_AWB_ALGO_PARAMS          MW_IP_CMD(0xB0B)
#define MW_IP_SET_MULTI_AWB_CONTROL_CAPABILITY   MW_IP_CMD(0xB0C)
#define MW_IP_GET_MULTI_AWB_CONTROL_CAPABILITY   MW_IP_CMD(0xB0D)

#define MW_IP_SET_AEB_INFO                       MW_IP_CMD(0xB0E)
#define MW_IP_GET_AEB_INFO                       MW_IP_CMD(0xB0F)
#define MW_IP_SET_IQ_INFO                        MW_IP_CMD(0xB10)
#define MW_IP_GET_IQ_INFO                        MW_IP_CMD(0xB11)
#define MW_IP_CHK_STILL_IDX                      MW_IP_CMD(0xB12)
#define MW_IP_CHK_VIDEO_IDX                      MW_IP_CMD(0xB18)

#define MW_IP_SET_WB_SIM_INFO                    MW_IP_CMD(0xB13)
#define MW_IP_GET_WB_SIM_INFO                    MW_IP_CMD(0xB14)

#define MW_IP_SET_FLICKER_CMD                    MW_IP_CMD(0xB15)
#define MW_IP_GET_FLICKER_CMD                    MW_IP_CMD(0xB16)

#define MW_IP_EXPS_TO_ISO                        MW_IP_CMD(0xB17)

#define MW_IP_SET_MULTI_CURR_SCENE_MODE          MW_IP_CMD(0xB50)
#define MW_IP_GET_MULTI_CURR_SCENE_MODE          MW_IP_CMD(0xB51)
#define MW_IP_SET_SCENE_MODE_INFO                MW_IP_CMD(0xB52)
#define MW_IP_GET_SCENE_MODE_INFO                MW_IP_CMD(0xB53)

#define MW_IP_SET_CURR_SCENE_MODE                MW_IP_CMD(0xB54)
#define MW_IP_GET_CURR_SCENE_MODE                MW_IP_CMD(0xB55)
#define MW_IP_SET_DE_PARAM                       MW_IP_CMD(0xB56)
#define MW_IP_GET_DE_PARAM                       MW_IP_CMD(0xB57)

#define MW_IP_SET_DIGITAL_EFFECT                 MW_IP_CMD(0xB58)
#define MW_IP_GET_DIGITAL_EFFECT                 MW_IP_CMD(0xB59)

#define MW_IP_SET_IMAGE_BRIGHTNESS               MW_IP_CMD(0xB5A)
#define MW_IP_SET_IMAGE_CONTRAST                 MW_IP_CMD(0xB5B)
#define MW_IP_SET_IMAGE_SATURATION               MW_IP_CMD(0xB5C)
#define MW_IP_SET_IMAGE_HUE                      MW_IP_CMD(0xB5D)
#define MW_IP_SET_IMAGE_SHARPNESS                MW_IP_CMD(0xB5E)

#define MW_IP_GET_IMAGE_BRIGHTNESS               MW_IP_CMD(0xB5F)
#define MW_IP_GET_IMAGE_CONTRAST                 MW_IP_CMD(0xB60)
#define MW_IP_GET_IMAGE_SATURATION               MW_IP_CMD(0xB61)
#define MW_IP_GET_IMAGE_HUE                      MW_IP_CMD(0xB62)
#define MW_IP_GET_IMAGE_SHARPNESS                MW_IP_CMD(0xB63)

#define MW_IP_GET_WB_COMP_SET_INFO               MW_IP_CMD(0xB64)
#define MW_IP_SET_WB_COMP_SET_INFO               MW_IP_CMD(0xB65)
#define MW_IP_GET_WB_COMP_TARGET_INFO            MW_IP_CMD(0xB66)
#define MW_IP_SET_WB_COMP_TARGET_INFO            MW_IP_CMD(0xB67)

#define MW_IP_SET_MODE                           MW_IP_CMD(0xC00)
#define MW_IP_GET_MODE                           MW_IP_CMD(0xC01)
#define MW_IP_SET_PHOTO_PREVIEW                  MW_IP_CMD(0xC02)
#define MW_IP_CHK_PHOTO_PREVIEW                  MW_IP_CMD(0xC03)
#define MW_IP_GET_ADJ_TABLE_NO                   MW_IP_CMD(0xC04)
#define MW_IP_GET_CONTI_SHOTCOUNT                MW_IP_CMD(0xC05)
#define MW_IP_SET_CONTI_SHOTCOUNT                MW_IP_CMD(0xC06)

#define MW_IP_SET_ISO_COMP_INFO                  MW_IP_CMD(0xD00)
#define MW_IP_GET_ISO_COMP_INFO                  MW_IP_CMD(0xD01)

#define MW_IP_TASK_INIT                          MW_IP_CMD(0xD02)
#define MW_IP_VDSP_HDRL_MEM_INIT                 MW_IP_CMD(0xD03)
#define MW_IP_RESET_VIDEO_PIPE_CTRL_PARAMS       MW_IP_CMD(0xD04)
#define MW_IP_RESET_STILL_PIPE_CTRL_PARAMS       MW_IP_CMD(0xD05)
#define MW_IP_MEMMGR_INIT                        MW_IP_CMD(0xD06)
#define MW_IP_SET_VIDEO_PIPE_CTRL_PARAMS         MW_IP_CMD(0xD07)
#define MW_IP_SET_STILL_PIPE_CTRL_PARAMS         MW_IP_CMD(0xD08)
#define MW_IP_SET_CC_REG_ADD                     MW_IP_CMD(0xD09)
#define MW_IP_RESET_VIDEO_PIPE_CTRL_FLAGS        MW_IP_CMD(0xD0A)
#define MW_IP_SET_TASK_INFO_INIT                 MW_IP_CMD(0xD0B)
#define MW_IP_GET_TASK_INFO_INIT                 MW_IP_CMD(0xD0C)

#define MW_IP_SAVEBIN                            MW_IP_CMD(0xE00)
#define MW_IP_LOADBIN                            MW_IP_CMD(0xE01)

#define MW_IP_SET_DEBUG_EXIF_FLAG                MW_IP_CMD(0x0E2)
#define MW_IP_GET_DEBUG_EXIF_FLAG                MW_IP_CMD(0x0E3)

#define MW_IP_GET_AAALIB_VER                     MW_IP_CMD(0x0E4)
#define MW_IP_GET_IPLIB_VER                      MW_IP_CMD(0x0E5)

#define MW_IP_SET_CAP_FORMAT                     MW_IP_CMD(0x0E6)
#define MW_IP_GET_CAP_FORMAT                     MW_IP_CMD(0x0E7)

#define MW_IP_SET_LIVEVIEW_INFO                  MW_IP_CMD(0x0E8)
#define MW_IP_GET_LIVEVIEW_INFO                  MW_IP_CMD(0x0E9)

#define MW_IP_GET_MODE_CFG                       MW_IP_CMD(0x0EA)
#define MW_IP_GET_PIPE_MODE                      MW_IP_CMD(0x0EB)
#define MW_IP_SET_PIPE_MODE                      MW_IP_CMD(0x0EC)
#define MW_IP_GET_VIDEO_ALGO_MODE                MW_IP_CMD(0x0ED)
#define MW_IP_SET_VIDEO_ALGO_MODE                MW_IP_CMD(0x0EE)

#define MW_IP_GET_ADJ_AQP_INFO                   MW_IP_CMD(0x0F0)
#define MW_IP_SET_ADJ_AQP_INFO                   MW_IP_CMD(0x0F1)

#define MW_IP_GET_VIDEO_HDR_ENABLE               MW_IP_CMD(0x0F2)
#define MW_IP_SET_VIDEO_HDR_ENABLE               MW_IP_CMD(0x0F3)
#define MW_IP_GET_STILL_HDR_ENABLE               MW_IP_CMD(0x0F4)
#define MW_IP_SET_STILL_HDR_ENABLE               MW_IP_CMD(0x0F5)


/* EIS Control */
#define MW_IP_AMBA_EIS_ENABLE                    MW_IP_CMD(0x1000)
#define MW_IP_AMBA_EIS_DISABLE                   MW_IP_CMD(0x1001)
#define MW_IP_AMBA_EIS_INIT                      MW_IP_CMD(0x1002)
#define MW_IP_AMBA_EIS_MODE_INIT                 MW_IP_CMD(0x1003)
#define MW_IP_AMBA_EIS_SET_FOCAL_LENGTH          MW_IP_CMD(0x1004)
#define MW_IP_AMBA_EIS_ENABLE_LBC                MW_IP_CMD(0x1005)
#define MW_IP_AMBA_EIS_ENABLE_LDC                MW_IP_CMD(0x1006)
#define MW_IP_AMBA_EIS_ENABLE_ROTATION           MW_IP_CMD(0x1007)
#define MW_IP_AMBA_EIS_ENABLE_PERSPECTIVE        MW_IP_CMD(0x1008)
#define MW_IP_AMBA_EIS_SET_VIDEO_SAMPLING        MW_IP_CMD(0x1009)
#define MW_IP_AMBA_EIS_TEST_CMD                  MW_IP_CMD(0x10FF)


////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////



#endif /*  __IMAGE_ALGO_A5_H__ */
