/**
* @file src/app/connected/applib/src/3a/applib_iqparam_handler/ApplibIQParam.c
*
* app init parameters function
*
* History:
*    07/12/2013 - [thwu] created file
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <AmbaDataType.h>
#include <AmbaPrintk.h>

#include <imgproc/AmbaImg_Proc.h>
#include <imgproc/AmbaImg_AeAwb.h>
#include "../applib_aaa/AmbaSample/AmbaSample_AeAwbAdj.h"
#include <3a/aaa/ApplibAeAwbAdj_Control.h>
#include <imgproc/AmbaImg_Impl_Cmd.h>
#include <3a/iqparam/ApplibIQParamHandler.h>
#include <AmbaROM.h>
#include "../../AppLibTask_Priority.h"


/**
*  @brief  Entry point of inital IQ parameters
*
*  @param [in] chNo channel number
*  @param [in] name  inital module string
*  @param [in] type  IMG_PARAM (0), AAA_PARAM  (1), ADJ_PARAM_VIDEO (2), ADJ_PARAM_PHOTO (3), ADJ_PARAM_LISO_STILL (4)
*
*  @return >=0 success, <0 failure
*/
INT32 App_Image_Init_Image_Params(UINT32 chNo, UINT32 ParamHandlerNo, const char *name, UINT32 type)
{
    INT32 ReturnValue = 0;
    IP_PARAM_s *IpParamTmp = NULL;

    ReturnValue = AppLibIQParam_Get_By_Name(ParamHandlerNo, name,(int)type, &IpParamTmp);
    if (ReturnValue < 0) {
        AmbaPrintColor(RED, "[Applib - IQParam] <App_Image_Init_Image_Params> AppLibIQParam_Get_By_Name fail");
        return -1;
    }
    if (IpParamTmp == NULL) {
        AmbaPrintColor(RED, "[Applib - IQParam] <App_Image_Init_Image_Params> IpParamTmp = NULL");
        return -1;
    } else if (IpParamTmp->Data == NULL) {
        AmbaPrintColor(RED, "[Applib - IQParam] <App_Image_Init_Image_Params> IpParamTmp->Data = NULL");
        return -1;
    } else {
        ReturnValue = AmbaImg_Proc_Cmd(MW_IP_CHK_IQ_PARAM_VER, (UINT32) chNo, IQ_PARAMS_IMG_DEF, (UINT32)IpParamTmp->Data);
        if (ReturnValue == 0) {
            ReturnValue = AmbaImg_Proc_Cmd(MW_IP_SET_IMG_PARAM_ADD, chNo, (UINT32)IpParamTmp->Data, 0);
        } else {
            AmbaPrintColor(RED, "[Applib - IQParam] <App_Image_Init_Image_Params> <MW_IP_CHK_IQ_PARAM_VER> Failure ReturnValue = %d", ReturnValue);
        }
    }
    return ReturnValue;
}

/**
*  @brief  Entry point of inital AAA parameters
*
*  @param [in] chNo channel number
*  @param [in] name  inital module string
*  @param [in] type  IMG_PARAM (0), AAA_PARAM  (1), ADJ_PARAM_VIDEO (2), ADJ_PARAM_PHOTO (3), ADJ_PARAM_LISO_STILL (4)
*
*  @return >=0 success, <0 failure
*/
INT32 App_Image_Init_AAA_Params(UINT32 chNo, UINT32 ParamHandlerNo, const char *name, UINT32 type)
{
    INT32 ReturnValue = 0;
    IP_PARAM_s *IpParamTmp = NULL;

    ReturnValue = AppLibIQParam_Get_By_Name(ParamHandlerNo, name, (int)type, &IpParamTmp);
    if (ReturnValue < 0) {
        AmbaPrintColor(RED, "[Applib - IQParam] <AppLibIQParam_Get_By_Name> AppLibIQParam_Get_By_Name fail");
        return -1;
    }

    if (IpParamTmp == NULL) {
        AmbaPrintColor(RED, "[Applib - IQParam] <App_Image_Init_AAA_Params> IpParamTmp = NULL");
        return -1;
    } else if (IpParamTmp->Data == NULL) {
        AmbaPrintColor(RED, "[Applib - IQParam] <App_Image_Init_AAA_Params> IpParamTmp->Data = NULL");
        return -1;
    } else {
        ReturnValue = AmbaImg_Proc_Cmd(MW_IP_CHK_IQ_PARAM_VER, (UINT32) chNo, IQ_PARAMS_AAA, (UINT32)IpParamTmp->Data);
        if (ReturnValue == 0) {
            ReturnValue = AmbaImg_Proc_Cmd(MW_IP_SET_AAA_PARAM, (UINT32)chNo, (UINT32)IpParamTmp->Data, 0);
        } else {
            AmbaPrintColor(RED, "[Applib - IQParam] <App_Image_Init_AAA_Params> <MW_IP_CHK_IQ_PARAM_VER> Failure ReturnValue = %d", ReturnValue);
        }
    }
    return ReturnValue;
}

/**
*  @brief  Entry point of inital Adj Table parameters
*
*  @param [in] chNo channel number
*  @param [in] name  inital module string
*  @param [in] type  IMG_PARAM (0), AAA_PARAM  (1), ADJ_PARAM_VIDEO (2), ADJ_PARAM_PHOTO (3), ADJ_PARAM_LISO_STILL (4)
*
*  @return >=0 success, <0 failure
*/
INT32 App_Image_Init_AdjTable_Param(UINT32 chNo, UINT32 ParamHandlerNo, const char *name, UINT32 type)
{
    INT32 ReturnValue = -1;
    IP_PARAM_s *IpParamTmp = NULL;

    ReturnValue = AppLibIQParam_Get_By_Name(ParamHandlerNo, name, (int)type, &IpParamTmp);
    if (ReturnValue < 0) {
        AmbaPrintColor(RED, "[Applib - IQParam] <App_Image_Init_AdjTable_Param> AppLibIQParam_Get_By_Name fail");
        return -1;
    }

    if (IpParamTmp == NULL) {
        AmbaPrintColor(RED, "IpParamTmp = NULL");
        return -1;
    } else if (IpParamTmp->Data == NULL) {
        AmbaPrintColor(RED, "[Applib - IQParam] <App_Image_Init_AdjTable_Param> IpParamTmp->Data = NULL");
        return -1;
    } else {
        ReturnValue = AmbaImg_Proc_Cmd(MW_IP_SET_ADJTABLE_PARAM, (UINT32)chNo, (UINT32)IpParamTmp->Data, 0);
    }
    return ReturnValue;
}


/**
*  @brief  Entry point of inital ADJ parameters
*
*  @param [in] chNo channel number
*  @param [in] name  inital module string
*  @param [in] type  IMG_PARAM (0), AAA_PARAM  (1), ADJ_PARAM_VIDEO (2), ADJ_PARAM_PHOTO (3), ADJ_PARAM_LISO_STILL (4)
*
*  @return >=0 success, <0 failure
*/
INT32 App_Image_Init_Adj_Params(UINT32 chNo, UINT32 ParamHandlerNo, const char *name, UINT32 type)
{
    INT32 ReturnValue = -1;
    IP_PARAM_s *IpParamTmp = NULL;

    switch (type) {
    case ADJ_PARAM_VIDEO:
        ReturnValue = AppLibIQParam_Get_By_Name(ParamHandlerNo, name, ADJ_PARAM_VIDEO, &IpParamTmp);
        if (ReturnValue < 0) {
            AmbaPrintColor(RED, "[Applib - IQParam] <AppLibIQParam_Get_By_Name> AppLibIQParam_Get_By_Name fail");
            return -1;
        }
        if (IpParamTmp->Data == NULL) {
            AmbaPrintColor(RED, "[Applib - IQParam] ADJ_PARAM_VIDEO IpParamTmp->Data == NULL");
            ReturnValue = -1;
        } else {
            ReturnValue = AmbaImg_Proc_Cmd(MW_IP_CHK_IQ_PARAM_VER, (UINT32) chNo, IQ_PARAMS_VIDEO_ADJ, (UINT32)IpParamTmp->Data);
            if (ReturnValue == 0) {
                ReturnValue = AmbaImg_Proc_Cmd(MW_IP_SET_ADJ_PARAMS_ADD, (UINT32) chNo, IQ_PARAMS_VIDEO_ADJ, (UINT32)IpParamTmp->Data);
            } else {
                AmbaPrintColor(RED, "[Applib - IQParam] <MW_IP_CHK_IQ_PARAM_VER> Failure ReturnValue = %d", ReturnValue);
            }
        }
        break;
    case ADJ_PARAM_PHOTO:
        ReturnValue = AppLibIQParam_Get_By_Name(ParamHandlerNo, name, ADJ_PARAM_PHOTO, &IpParamTmp);
        if (ReturnValue < 0) {
            AmbaPrintColor(RED, "[Applib - IQParam] <AppLibIQParam_Get_By_Name> AppLibIQParam_Get_By_Name fail");
            return -1;
        }
        if (IpParamTmp->Data == NULL) {
            AmbaPrintColor(RED, "[Applib - IQParam] ADJ_PARAM_PHOTO IpParamTmp->Data == NULL");
            ReturnValue = -1;
        } else {
            ReturnValue = AmbaImg_Proc_Cmd(MW_IP_CHK_IQ_PARAM_VER, (UINT32) chNo, IQ_PARAMS_PHOTO_ADJ, (UINT32)IpParamTmp->Data);
            if (ReturnValue == 0) {
                ReturnValue = AmbaImg_Proc_Cmd(MW_IP_SET_ADJ_PARAMS_ADD, (UINT32) chNo, IQ_PARAMS_PHOTO_ADJ, (UINT32)IpParamTmp->Data);
            } else {
                AmbaPrintColor(RED, "[Applib - IQParam] <MW_IP_CHK_IQ_PARAM_VER> Failure ReturnValue = %d", ReturnValue);
            }
        }
        break;
    case ADJ_PARAM_LISO_STILL:
        ReturnValue = AppLibIQParam_Get_By_Name(ParamHandlerNo, name, ADJ_PARAM_LISO_STILL, &IpParamTmp);
        if (ReturnValue < 0) {
            AmbaPrintColor(RED, "[Applib - IQParam] <AppLibIQParam_Get_By_Name> AppLibIQParam_Get_By_Name fail");
            return -1;
        }
        if (IpParamTmp->Data == NULL) {
            AmbaPrintColor(RED, "[Applib - IQParam] ADJ_PARAM_LISO_STILL IpParamTmp->Data == NULL");
            ReturnValue = -1;
        } else {
            ReturnValue = AmbaImg_Proc_Cmd(MW_IP_CHK_IQ_PARAM_VER, (UINT32) chNo, IQ_PARAMS_STILL_LISO_ADJ, (UINT32)IpParamTmp->Data);
            if (ReturnValue == 0) {
                ReturnValue = AmbaImg_Proc_Cmd(MW_IP_SET_ADJ_PARAMS_ADD, (UINT32) chNo, IQ_PARAMS_STILL_LISO_ADJ, (UINT32)IpParamTmp->Data);
            } else {
                AmbaPrintColor(RED, "[Applib - IQParam] <MW_IP_CHK_IQ_PARAM_VER> Failure ReturnValue = %d", ReturnValue);
            }
        }
        break;
    case ADJ_PARAM_HISO_STILL:
        ReturnValue = AppLibIQParam_Get_By_Name(ParamHandlerNo, name, ADJ_PARAM_HISO_STILL, &IpParamTmp);
        if (ReturnValue < 0) {
            AmbaPrintColor(RED, "[Applib - IQParam] <AppLibIQParam_Get_By_Name> AppLibIQParam_Get_By_Name fail");
            return -1;
        }
        if (IpParamTmp->Data == NULL) {
            AmbaPrintColor(RED, "[Applib - IQParam] ADJ_PARAM_HISO_STILL IpParamTmp->Data == NULL");
            ReturnValue = -1;
        } else {
            ReturnValue = AmbaImg_Proc_Cmd(MW_IP_CHK_IQ_PARAM_VER, (UINT32) chNo, IQ_PARAMS_STILL_HISO_ADJ, (UINT32)IpParamTmp->Data);
            if (ReturnValue == 0) {
                ReturnValue = AmbaImg_Proc_Cmd(MW_IP_SET_ADJ_PARAMS_ADD, (UINT32) chNo, IQ_PARAMS_STILL_HISO_ADJ, (UINT32)IpParamTmp->Data);
            } else {
                AmbaPrintColor(RED, "[Applib - IQParam] <MW_IP_CHK_IQ_PARAM_VER> Failure ReturnValue = %d", ReturnValue);
            }
        }
        break;
    case ADJ_PARAM_STILL_IDX:
        ReturnValue = AppLibIQParam_Get_By_Name(ParamHandlerNo, name, ADJ_PARAM_STILL_IDX, &IpParamTmp);
        if (ReturnValue < 0) {
            AmbaPrintColor(RED, "[Applib - IQParam] <AppLibIQParam_Get_By_Name> AppLibIQParam_Get_By_Name fail");
            return -1;
        }
        if (IpParamTmp->Data == NULL) {
            AmbaPrintColor(RED, "[Applib - IQParam] ADJ_PARAM_STILL_IDX IpParamTmp->Data == NULL");
            ReturnValue = -1;
        } else {
            ReturnValue = AmbaImg_Proc_Cmd(MW_IP_CHK_IQ_PARAM_VER, (UINT32) chNo, IQ_PARAMS_STILL_IDX_INFO_ADJ, (UINT32)IpParamTmp->Data);
            if (ReturnValue == 0) {
                ReturnValue = AmbaImg_Proc_Cmd(MW_IP_SET_ADJ_PARAMS_ADD, (UINT32) chNo, IQ_PARAMS_STILL_IDX_INFO_ADJ, (UINT32)IpParamTmp->Data);
            } else {
                AmbaPrintColor(RED, "[Applib - IQParam] <MW_IP_CHK_IQ_PARAM_VER> Failure ReturnValue = %d", ReturnValue);
            }
        }
        break;
    default:
        AmbaPrint("[Applib - IQParam] <Init_Adj_Params> Unknown ADJ param type");
        break;
    }

    return ReturnValue;
}


/**
*  @brief  Entry point of inital Scene parameters
*
*  @param [in] chNo channel number
*  @param [in] name  inital module string
*  @param [in] type  IMG_PARAM (0), AAA_PARAM  (1), ADJ_PARAM_VIDEO (2), ADJ_PARAM_PHOTO (3), ADJ_PARAM_LISO_STILL (4)
*
*  @return >=0 success, <0 failure
*/
INT32 App_Image_Init_Scene_Params(UINT32 chNo, UINT32 ParamHandlerNo, const char *name, UINT32 type)
{
    INT32 ReturnValue = 0;
    UINT16 SceneSetCount;
    UINT16 SceneSetMaxNum = 8;
    IP_PARAM_s *IpParamTmp = NULL;

    ReturnValue = AppLibIQParam_Get_By_Name(ParamHandlerNo, name, type, &IpParamTmp);;
    if (ReturnValue < 0) {
        AmbaPrintColor(RED, "[Applib - IQParam] <App_Image_Init_Scene_Params> AppLibIQParam_Get_By_Name fail");
        return -1;
    }

    if (IpParamTmp->Data == NULL) {
        AmbaPrintColor(RED, "[Applib - IQParam] <App_Image_Init_Scene_Params>  IpParamTmp->Data == NULL");
        return -1;
    }

    if (IpParamTmp) {
        if (strcmp(name, "scene_data_s01")==0) {
            for (SceneSetCount=0; SceneSetCount<SceneSetMaxNum;SceneSetCount++) {
                ReturnValue |= AmbaImg_Proc_Cmd(MW_IP_SET_SCENE_MODE_INFO, SceneSetCount, (((UINT32)IpParamTmp->Data)+SceneSetCount*sizeof(SCENE_DATA_s)), 0);
            }
        } else if (strcmp(name, "scene_data_s02")==0) {
            for (SceneSetCount=0; SceneSetCount<SceneSetMaxNum;SceneSetCount++) {
                ReturnValue |= AmbaImg_Proc_Cmd(MW_IP_SET_SCENE_MODE_INFO, (SceneSetCount+SceneSetMaxNum*1), (((UINT32)IpParamTmp->Data)+SceneSetCount*sizeof(SCENE_DATA_s)), 0);
            }
        } else if (strcmp(name, "scene_data_s03")==0) {
            for (SceneSetCount=0; SceneSetCount<SceneSetMaxNum;SceneSetCount++) {
                ReturnValue |= AmbaImg_Proc_Cmd(MW_IP_SET_SCENE_MODE_INFO, (SceneSetCount+SceneSetMaxNum*2), (((UINT32)IpParamTmp->Data)+SceneSetCount*sizeof(SCENE_DATA_s)), 0);
            }
        } else if (strcmp(name, "scene_data_s04")==0) {
            for (SceneSetCount=0; SceneSetCount<SceneSetMaxNum;SceneSetCount++) {
                ReturnValue |= AmbaImg_Proc_Cmd(MW_IP_SET_SCENE_MODE_INFO, (SceneSetCount+SceneSetMaxNum*3), (((UINT32)IpParamTmp->Data)+SceneSetCount*sizeof(SCENE_DATA_s)), 0);
            }
        } else if (strcmp(name, "scene_data_s05")==0) {
            for (SceneSetCount=0; SceneSetCount<SceneSetMaxNum;SceneSetCount++) {
                ReturnValue |= AmbaImg_Proc_Cmd(MW_IP_SET_SCENE_MODE_INFO, (SceneSetCount+SceneSetMaxNum*4), (((UINT32)IpParamTmp->Data)+SceneSetCount*sizeof(SCENE_DATA_s)), 0);
            }
        }
    }

      return ReturnValue;
}

/**
*  @brief  Entry point of inital DE parameters
*
*  @param [in] name  inital module string
*  @param [in] type  IMG_PARAM (0), AAA_PARAM  (1), ADJ_PARAM_VIDEO (2), ADJ_PARAM_PHOTO (3), ADJ_PARAM_LISO_STILL (4)
*
*  @return >=0 success, <0 failure
*/
INT32 App_Image_Init_De_Params(UINT32 chNo, UINT32 ParamHandlerNo, const char *name, UINT32 type)
{
    INT32 ReturnValue = -1;
    IP_PARAM_s *IpParamTmp = NULL;

    ReturnValue = AppLibIQParam_Get_By_Name(ParamHandlerNo, name, type, &IpParamTmp);
    if (ReturnValue < 0) {
        AmbaPrintColor(RED, "[Applib - IQParam] <App_Image_Init_De_Params> AppLibIQParam_Get_By_Name fail");
        return -1;
    }
    if (IpParamTmp == NULL) {
        AmbaPrintColor(RED, "[Applib - IQParam] <App_Image_Init_De_Params>  IpParamTmp == NULL");
        return -1;
    } else if (IpParamTmp->Data == NULL) {
        AmbaPrintColor(RED, "[Applib - IQParam] <App_Image_Init_De_Params>  IpParamTmp->Data == NULL");
        return -1;
    } else {
        if (strcmp(name, "de_default_still") == 0) {
            ReturnValue = AmbaImg_Proc_Cmd(MW_IP_SET_DE_PARAM, IP_MODE_STILL, (UINT32)IpParamTmp->Data, 0);
        } else {
            ReturnValue = AmbaImg_Proc_Cmd(MW_IP_SET_DE_PARAM, IP_MODE_VIDEO, (UINT32)IpParamTmp->Data, 0);
        }
    }
    return ReturnValue;
}

/**
*  @brief  Entry point of inital of IQ parameters
*
*  @param [in] ChCount
*
*  @return >=0 success, <0 failure
*/
INT32 AppLibIQ_ImageInit(UINT32 ChCount)
{
    INT32 ReturnValue = 0;

    IMG_PROC_FUNC_s IpFuncTmp = {NULL, NULL, NULL, NULL};
    //AMBA_3A_OP_INFO_s  AaaOpInfo = {ENABLE, ENABLE, ENABLE, ENABLE};
    AMBA_IMG_TSK_Info_s AaaTaskInfo = {0};
    UINT32 i = 0;

    if (ChCount == 0) {
        AmbaPrintColor(RED, "[Applib - IQParam] AppLib_ImageInit the channel count can not be zero");
        ChCount = 1;
    }

     /**Set Ae task pripority */
    AaaTaskInfo.Priority = APPLIB_AE_TASK_PRIORITY;
    AaaTaskInfo.StackSize = AMBA_IMG_AE_TSK_STACK_SIZE;
    AaaTaskInfo.CoreExclusiveBitMap = 0xfffe;//this setting is no use in smp
    ReturnValue = AmbaImg_Proc_Cmd(MW_IP_SET_TASK_INFO_INIT, 0/**AE*/, 0, (UINT32) &AaaTaskInfo);
    /**Set Awb task pripority */
    AaaTaskInfo.Priority = APPLIB_AWB_TASK_PRIORITY;
    AaaTaskInfo.StackSize = AMBA_IMG_AWB_TSK_STACK_SIZE;
    ReturnValue = AmbaImg_Proc_Cmd(MW_IP_SET_TASK_INFO_INIT, 1/**AWB*/, 0, (UINT32) &AaaTaskInfo);
    /**Set Adj task pripority */
    AaaTaskInfo.Priority = APPLIB_ADJ_TASK_PRIORITY;
    AaaTaskInfo.StackSize = AMBA_IMG_ADJ_TSK_STACK_SIZE;
    ReturnValue = AmbaImg_Proc_Cmd(MW_IP_SET_TASK_INFO_INIT, 2/**ADJ*/, 0, (UINT32) &AaaTaskInfo);
    /**Set Ob task pripority */
    AaaTaskInfo.Priority = APPLIB_OB_TASK_PRIORITY;
    AaaTaskInfo.StackSize = AMBA_IMG_OB_TSK_STACK_SIZE;
    ReturnValue = AmbaImg_Proc_Cmd(MW_IP_SET_TASK_INFO_INIT, 3/**OB*/, 0, (UINT32) &AaaTaskInfo);

    ReturnValue = AmbaImg_Proc_Cmd(MW_IP_SET_TOTAL_CH_COUNT,(UINT32)ChCount , 0, 0);
    ReturnValue = AmbaImg_Proc_Cmd(MW_IP_SET_MEM_CTRLADDR, (UINT32)APPLIB_G_MMPL, 0, 0);
    ReturnValue = AmbaImg_Proc_Cmd(MW_IP_TASK_INIT, 0, ChCount, 0);
    ReturnValue = AmbaImg_Proc_Cmd(MW_IP_PARAM_INIT, ChCount, 0, 0);
    ReturnValue = AmbaImg_Proc_Cmd(MW_IP_GET_TOTAL_CH_COUNT,(UINT32)&ChCount , 0, 0);
    ReturnValue = AmbaImg_Proc_Cmd(MW_IP_VDSP_HDRL_MEM_INIT, ChCount,0, 0);


    IpFuncTmp.AeAwbAdj_Init    = AppLibAeAwbAdj_Init;
    IpFuncTmp.AeAwbAdj_Control = AppLibAeAwbAdj_Control;
    IpFuncTmp.Ae_Ctrl  =  AppLibAe_Ctrl;
    IpFuncTmp.Awb_Ctrl =  AppLibAwb_Ctrl;
    IpFuncTmp.Adj_Ctrl =  AppLibAdj_Ctrl;

    for (i = 0;i < ChCount; i++) {
        AmbaImg_Proc_Cmd(MW_IP_RESET_VIDEO_PIPE_CTRL_PARAMS, i, 0, 0);
        AmbaImg_Proc_Cmd(MW_IP_RESET_STILL_PIPE_CTRL_PARAMS, i, 0, 0);
        AmbaImg_Proc_Cmd(MW_IP_REGISTER_FUNC, i, (UINT32)&IpFuncTmp, 0);
        AppLibIQParam_Allocate_Param_Mem(i);
    }

    return ReturnValue;
}


INT32 AppLibIQ_ParamInit(UINT32 ChCount)
{
    INT32 ReturnValue = 0;
    UINT32 i = 0;
    UINT8 HdrEnable = 0;
    int ParamHandlerNo = 0;
    AMBA_3A_OP_INFO_s  AaaOpInfo = {ENABLE, ENABLE, ENABLE, ENABLE};

    ReturnValue = AmbaImg_Proc_Cmd(MW_IP_GET_VIDEO_HDR_ENABLE, 0, (UINT32)&HdrEnable, 0);


    if(HdrEnable == IMG_DSP_HDR_MODE_0){
        ParamHandlerNo = 1; /**Normal use handler 1 iq parameter function set*/
    } else {
        ParamHandlerNo = 0; /**Normal use handler 0 iq parameter function set*/
    }
    ReturnValue = AppLibIQParam_Init_Param_Proxy(ParamHandlerNo);
    for (i = 0;i < ChCount; i++) {
        ReturnValue += App_Image_Init_Image_Params(i,ParamHandlerNo, "img_default", IMG_PARAM);
        if((HdrEnable == IMG_DSP_HDR_MODE_0)&&(i == 1)){
            ReturnValue += App_Image_Init_AAA_Params(i,ParamHandlerNo, "aaa_default_01", AAA_PARAM);
        } else {
            ReturnValue += App_Image_Init_AAA_Params(i,ParamHandlerNo, "aaa_default_00", AAA_PARAM);
        }
        ReturnValue += App_Image_Init_AdjTable_Param(i,ParamHandlerNo, "adj_table_param_default", ADJ_TABLE_PARAM);
        ReturnValue += App_Image_Init_Adj_Params(i,ParamHandlerNo, "adj_video_default_00", ADJ_PARAM_VIDEO);
        ReturnValue += App_Image_Init_Adj_Params(i,ParamHandlerNo, "adj_photo_default_00", ADJ_PARAM_PHOTO);
        ReturnValue += App_Image_Init_Adj_Params(i,ParamHandlerNo, "adj_still_default_00", ADJ_PARAM_LISO_STILL);
        ReturnValue += App_Image_Init_Adj_Params(i,ParamHandlerNo, "adj_hiso_still_default_00", ADJ_PARAM_HISO_STILL);
        ReturnValue += App_Image_Init_Adj_Params(i,ParamHandlerNo, "adj_still_idx", ADJ_PARAM_STILL_IDX);
        ReturnValue += App_Image_Init_Scene_Params(i,ParamHandlerNo, "scene_data_s01", SCENE_DATA);
        ReturnValue += App_Image_Init_Scene_Params(i,ParamHandlerNo, "scene_data_s02", SCENE_DATA);
        ReturnValue += App_Image_Init_Scene_Params(i,ParamHandlerNo, "scene_data_s03", SCENE_DATA);
        ReturnValue += App_Image_Init_Scene_Params(i,ParamHandlerNo, "scene_data_s04", SCENE_DATA);
        ReturnValue += App_Image_Init_Scene_Params(i,ParamHandlerNo, "scene_data_s05", SCENE_DATA);
        ReturnValue += App_Image_Init_De_Params(i,ParamHandlerNo,"de_default_video", DE_PARAM_VIDEO);
        ReturnValue += App_Image_Init_De_Params(i,ParamHandlerNo,"de_default_still", DE_PARAM_STILL);
        if (ReturnValue == 0) {
            AmbaImg_Proc_Cmd(MW_IP_SET_AAA_OP_INFO, i, (UINT32)&AaaOpInfo, 0);
        } else {
            AmbaPrintColor(RED, "[Applib - IQParam] AppLibIQ_ParamInit Fail.");
        }
    }
    return ReturnValue;


}
