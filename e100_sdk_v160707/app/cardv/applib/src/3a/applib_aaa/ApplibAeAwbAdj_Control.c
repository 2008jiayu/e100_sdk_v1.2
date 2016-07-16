/**
* @file src/app/connected/applib/src/3a/applib_aaa/ApplibAeAwbAd_Control.c
*
* Implementation of Async Operation - APP level
*
* History:
*    04/22/2014 - [wkche] created file
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
#include <AmbaKAL.h>

#include <AmbaDSP_ImgFilter.h>
#include <imgproc/AmbaImg_Proc.h>
#include <imgproc/AmbaImg_AeAwb.h>
#include <imgproc/AmbaImg_AaaDef.h>
#include <imgproc/AmbaImg_AntiFlicker.h>
#include "AmbaSample/AmbaSample_AeAwbAdj.h"
#include <imgproc/AmbaImg_Impl_Cmd.h>
#include <imgschdlr/scheduler.h>
#include <calibration/ApplibCalibAdjust.h>
#include <imgproc/AmbaImg_VDsp_Handler.h>

static AE_ALGO_INFO_s  AeAlgoInfo[10];
UINT32 IpctestChno = 0xFFFFFFFF;

void AppLibAdj_VideoHdr_Control(AMBA_AE_INFO_s    *videoAeInfo)
{
    static HDR_INFO_s               HdrInfo;

    memset(&HdrInfo, 0, sizeof(HdrInfo));
    HdrInfo.AeInfo[0] = videoAeInfo[0];
    HdrInfo.AeInfo[1] = videoAeInfo[1];

    AmbaImg_Proc_Cmd(MW_IP_GET_PIPE_WB_GAIN, 0, IP_MODE_VIDEO, (UINT32)&HdrInfo.WbGain[0]); //long
    AmbaImg_Proc_Cmd(MW_IP_GET_PIPE_WB_GAIN, 0, IP_MODE_VIDEO, (UINT32)&HdrInfo.WbGain[1]); //short
    AmbaImg_Proc_Cmd(MW_IP_ADJ_VIDEO_HDR_CONTROL, 0, (UINT32)&HdrInfo, 0);

    videoAeInfo[0] = HdrInfo.AeInfo[0];
    videoAeInfo[1] = HdrInfo.AeInfo[1];
}

 /**
 *  @brief To Setup MW exposure scheduler module
 *
 *  @param [in] chNo Channel number.
 *  @param [in] pMode Pointer to DSP mode
 *  @param [in] type Type of operation mode of MW exposure scheduler module
 *                   0 : Normal mode
 *                   1 : Direct set mode
 *
 *  @return None
 */
void AppLibAeAwbAdj_Set_SchdlrExpInfo(UINT32 chNo, AMBA_DSP_IMG_MODE_CFG_s *pMode, UINT8 type, UINT16 UpdateFlg)
{
    AMBA_IMG_SCHDLR_EXP_s    VideoSchdlrExpInfo;
    AMBA_AE_INFO_s          VideoAeInfo[2];
    AMBA_DSP_IMG_WB_GAIN_s  VideoWbGain = {WB_UNIT_GAIN,WB_UNIT_GAIN,WB_UNIT_GAIN,WB_UNIT_GAIN,WB_UNIT_GAIN};
    UINT32 FrontWbCheck = 0;
    UINT8 HdrEnable = 0;
    UINT8 ForceHdrUpdate = 1;
    PIPELINE_CONTROL_s* VideoPipeCtrl = 0;
    AMBA_3A_OP_INFO_s       AaaOpInfo;

    memset(&VideoSchdlrExpInfo, 0, sizeof(AMBA_IMG_SCHDLR_EXP_s));
    memset(&VideoAeInfo, 0, sizeof(AMBA_AE_INFO_s)*2);

    AmbaImg_Proc_Cmd(MW_IP_GET_AAA_OP_INFO, chNo, (UINT32)&AaaOpInfo, 0);
    AmbaImg_Proc_Cmd(MW_IP_GET_VIDEO_HDR_ENABLE, chNo, (UINT32)&HdrEnable, 0);

    AmbaImg_Proc_Cmd(MW_IP_GET_AE_INFO, chNo, IP_MODE_VIDEO, (UINT32)&VideoAeInfo[0]);
    if(HdrEnable == IMG_DSP_HDR_MODE_0 && (AaaOpInfo.AeOp == ENABLE)){
        UINT32 VideoPipeAddr = 0;

        AmbaImg_Proc_Cmd(MW_IP_GET_AE_INFO, (chNo + 1), IP_MODE_VIDEO, (UINT32)&VideoAeInfo[1]);
        Amba_Img_Get_Video_Pipe_Ctrl_Addr((UINT32)chNo, &VideoPipeAddr);
        VideoPipeCtrl = (PIPELINE_CONTROL_s *)VideoPipeAddr;

        //AmbaPrint("+++++++ %12.5f,%5d,    %12.5f,%5d, +++++++++",VideoAeInfo[0].AgcGain,VideoAeInfo[0].Dgain,VideoAeInfo[1].AgcGain,VideoAeInfo[1].Dgain);
        AmbaImg_Proc_Cmd(MW_IP_ADJ_VIDEO_HDR_CHK_DGAINMODE, (UINT32)&VideoAeInfo[0],0,0);
        //AmbaPrint("        %12.5f,%5d,    %12.5f,%5d,          ",VideoAeInfo[0].AgcGain,VideoAeInfo[0].Dgain,VideoAeInfo[1].AgcGain,VideoAeInfo[1].Dgain);
        AppLibAdj_VideoHdr_Control(VideoAeInfo);
        //AmbaPrint("------- %12.5f,%5d,    %12.5f,%5d, ---------",VideoAeInfo[0].AgcGain,VideoAeInfo[0].Dgain,VideoAeInfo[1].AgcGain,VideoAeInfo[1].Dgain);
    }

    AmbaImg_Proc_Cmd(MW_IP_GET_MULTI_AE_ALGO_PARAMS, chNo, (UINT32)&AeAlgoInfo[chNo], 0);
    AmbaImg_Proc_Cmd(MW_IP_Get_FRO_WB_CHK, chNo, (UINT32)&FrontWbCheck, 0);

    if(FrontWbCheck == 0 || (HdrEnable == 0)){
       AmbaImg_Proc_Cmd(MW_IP_GET_PIPE_WB_GAIN, chNo, IP_MODE_VIDEO, (UINT32)&VideoWbGain);
    }

    VideoSchdlrExpInfo.Info.SubChannelNum = 0;
    VideoSchdlrExpInfo.Info.AGC[0] = VideoAeInfo[0].AgcGain;
    VideoSchdlrExpInfo.Info.Shutter[0] = VideoAeInfo[0].ShutterTime;
    VideoSchdlrExpInfo.Info.IrisIndex = VideoAeInfo[0].IrisIndex;
    VideoSchdlrExpInfo.Info.Mode = *pMode;
    VideoSchdlrExpInfo.Info.DGain = VideoWbGain;
    VideoSchdlrExpInfo.Info.DGain.AeGain = VideoAeInfo[0].Dgain;
    VideoSchdlrExpInfo.Info.DGain.GlobalDGain = AeAlgoInfo[chNo].DefSetting.GlobalDGain;
    if(HdrEnable == IMG_DSP_HDR_MODE_0){
        VideoSchdlrExpInfo.Info.SubChannelNum = 1;
        VideoSchdlrExpInfo.Info.AGC[1] = VideoAeInfo[1].AgcGain;
        VideoSchdlrExpInfo.Info.Shutter[1] = VideoAeInfo[1].ShutterTime;
        //AmbaPrint("%s, %f, %f", __FUNCTION__, VideoSchdlrExpInfo.Info.AGC[1], VideoSchdlrExpInfo.Info.Shutter[1]);

        /* HDR filter */
        if (VideoPipeCtrl) {
            if (ForceHdrUpdate || VideoPipeCtrl->HdrBlendingInfoUpdate) {
                memcpy(&VideoSchdlrExpInfo.Info.BlendInfo[0], &VideoPipeCtrl->HdrBlendingInfo, sizeof(AMBA_DSP_IMG_HDR_BLENDING_INFO_s));
                VideoSchdlrExpInfo.Info.HdrUpdated[0].Bits.BlendInfo = 1;
            }
            if (VideoPipeCtrl->HdrAlphaConfig0Update) {
                memcpy(&VideoSchdlrExpInfo.Info.AlphaCalc[0], &VideoPipeCtrl->HdrAlphaConfig0, sizeof(AMBA_DSP_IMG_HDR_ALPHA_CALC_CONFIG_s));
                VideoSchdlrExpInfo.Info.HdrUpdated[0].Bits.AlphaCalc = 1;
            }
            if (ForceHdrUpdate || VideoPipeCtrl->HdrAlphaThreshold0Update) {
                memcpy(&VideoSchdlrExpInfo.Info.AlphaThresh[0], &VideoPipeCtrl->HdrAlphaThreshold0, sizeof(AMBA_DSP_IMG_HDR_ALPHA_CALC_THRESH_s));
                VideoSchdlrExpInfo.Info.HdrUpdated[0].Bits.AlphaCalcThrd = 1;
            }
            if (VideoPipeCtrl->BlackCorrUpdate) {
                memcpy(&VideoSchdlrExpInfo.Info.BlackCorr[0], &VideoPipeCtrl->BlackCorr, sizeof(AMBA_DSP_IMG_BLACK_CORRECTION_s));
                VideoSchdlrExpInfo.Info.HdrUpdated[0].Bits.BlackCorr = 1;
            }
            if (ForceHdrUpdate || VideoPipeCtrl->HdrAmpLinearization0Update) {
                memcpy(&VideoSchdlrExpInfo.Info.AmpLinear[0], &VideoPipeCtrl->HdrAmpLinearization0, sizeof(AMBA_DSP_IMG_AMP_LINEARIZATION_s));
                VideoSchdlrExpInfo.Info.HdrUpdated[0].Bits.AmpLinear = 1;
            }
        }
    }
    if (type == 255) {
        VideoSchdlrExpInfo.Type = AMBA_IMG_SCHDLR_SET_TYPE_DIRECT;
        VideoSchdlrExpInfo.Info.AdjUpdated = 1;
        //fill hdr update above
        //VideoSchdlrExpInfo.Info.HdrUpdated = 1;
    } else {
        VideoSchdlrExpInfo.Type = AMBA_IMG_SCHDLR_SET_TYPE_NORMAL;//By scheduler
        VideoSchdlrExpInfo.Info.AdjUpdated = (UpdateFlg % 2);
        //fill hdr update above
        //VideoSchdlrExpInfo.Info.HdrUpdated = 1;
    }
    if(HdrEnable == IMG_DSP_HDR_MODE_0){
        if(AaaOpInfo.AeOp == ENABLE){
            AmbaImgSchdlr_SetExposure(chNo, &VideoSchdlrExpInfo);
        }    
    }else{
        AmbaImgSchdlr_SetExposure(chNo, &VideoSchdlrExpInfo);
    }
}

static IMGPROC_TASK_ENABLE_s DefTaskEnableTable[MAX_CHAN_NUM] = {{ 0, -1, -1, -1, -1, -1, -1, -1, -1, -1},  // 0  |
                                                                 { 1, -1, -1, -1, -1, -1, -1, -1, -1, -1},  // 1  V
                                                                 { 2, -1, -1, -1, -1, -1, -1, -1, -1, -1},  // 2
                                                                 { 3, -1, -1, -1, -1, -1, -1, -1, -1, -1},  // 3
                                                                 { 4, -1, -1, -1, -1, -1, -1, -1, -1, -1},  // 4
                                                                 { 5, -1, -1, -1, -1, -1, -1, -1, -1, -1},  // 5
                                                                 { 6, -1, -1, -1, -1, -1, -1, -1, -1, -1},  // 6
                                                                 { 7, -1, -1, -1, -1, -1, -1, -1, -1, -1},  // 7
                                                                 { 8, -1, -1, -1, -1, -1, -1, -1, -1, -1},  // 8
                                                                 { 9, -1, -1, -1, -1, -1, -1, -1, -1, -1}}; // 9
static IMGPROC_TASK_ENABLE_s TaskEnableTmp[MAX_CHAN_NUM];
 /**
 *  @brief To initialize the AE/AWB/ADJ algo related settings
 *
 *  @param [in] chNo Channel number.
 *  @param [in] initFlg All(0), Ae(1), Awb(2), Adj(3)
 *  @param [in] pMMPL pointer to mempool control block
 *
 *  @return None
 */
void AppLibAeAwbAdj_Init(UINT32 chNo,UINT8 initFlg, AMBA_KAL_BYTE_POOL_t *pMMPL)
{
    AMBA_3A_OP_INFO_s  AaaOpInfo = {ENABLE, ENABLE, ENABLE, ENABLE};
    AMBA_3A_STATUS_s    VideoStatus = {AMBA_IDLE, AMBA_IDLE, AMBA_IDLE};
    AMBA_3A_STATUS_s    StillStatus = {AMBA_IDLE, AMBA_IDLE, AMBA_IDLE};
    AMBA_DSP_IMG_MODE_CFG_s Mode;
    UINT16 PipeMode = 0;
    UINT16 VideoAlgoMode = 0;
    AMBA_DSP_IMG_ALGO_MODE_e AlgoMode;
    UINT16 UpdateFlg = 0;
    UINT8  HdrEnable = 0;

    memset(&Mode, 0, sizeof(Mode));
    AmbaImg_Proc_Cmd(MW_IP_GET_MODE_CFG, chNo, 0, (UINT32)&Mode);
    AmbaImg_Proc_Cmd(MW_IP_GET_VIDEO_HDR_ENABLE, chNo, (UINT32)&HdrEnable, 0);
    if(HdrEnable == IMG_DSP_HDR_MODE_0){
        Mode.FuncMode = AMBA_DSP_IMG_FUNC_MODE_VHDR;
        Amba_Img_Get_TaskEnableTable(0, TaskEnableTmp);
        TaskEnableTmp[0].TaskEnableId[0] = 0;
        TaskEnableTmp[0].TaskEnableId[1] = 1;
        Amba_Img_Set_TaskEnableTable(0, TaskEnableTmp);
    } else {
        Amba_Img_Set_TaskEnableTable(0, DefTaskEnableTable); //for Ae
        Amba_Img_Set_TaskEnableTable(1, DefTaskEnableTable); //for Awb
        Amba_Img_Set_TaskEnableTable(2, DefTaskEnableTable); //for Adj
    }
    AmbaImg_Proc_Cmd(MW_IP_GET_PIPE_MODE, chNo, (UINT32)&PipeMode, 0);
    AmbaImg_Proc_Cmd(MW_IP_GET_VIDEO_ALGO_MODE, (UINT32)chNo, (UINT32)&VideoAlgoMode, 0);
    if (VideoAlgoMode == IP_MODE_LISO_VIDEO) {
        AlgoMode = AMBA_DSP_IMG_ALGO_MODE_LISO;
    } else if (VideoAlgoMode == IP_MODE_HISO_VIDEO) {
        AlgoMode = AMBA_DSP_IMG_ALGO_MODE_HISO;
    } else {
        AlgoMode = AMBA_DSP_IMG_ALGO_MODE_FAST;
    }

    if(PipeMode == IP_HYBRID_MODE){
        Mode.ContextId = AmbaImgSchdlr_GetIsoCtxIndex(chNo, AlgoMode);
    } else {
        Mode.ContextId = 0; // TBD
    }

    if (pMMPL != NULL) {
      AmbaImg_Proc_Cmd(MW_IP_SET_MEM_CTRLADDR, (UINT32)pMMPL, 0, 0);
    }

    //AmbaImg_Proc_Cmd(MW_IP_SET_AAA_OP_INFO, chNo, (UINT32)&AaaOpInfo, 0); //>>>>>> FIXME
    AmbaImg_Proc_Cmd(MW_IP_GET_AAA_OP_INFO, chNo, (UINT32)&AaaOpInfo, 0);
    AmbaImg_Proc_Cmd(MW_IP_SET_3A_STATUS, chNo, (UINT32)&VideoStatus, (UINT32)&StillStatus);



    if (AaaOpInfo.AeOp == ENABLE && (initFlg == 0 || initFlg == 1)) {
        UINT32 ChCount = 1,i;
        AmbaImg_Proc_Cmd(MW_IP_GET_TOTAL_CH_COUNT, (UINT32)&ChCount, 1, 0);
        //AmbaSample_AeInit(chNo);
        for (i = 0;i < ChCount;i++) {
             AmbaImg_Proc_Cmd(MW_IP_AMBA_AEAWBADJ_INIT, i, 1, 0);
        }
    }

    if (AaaOpInfo.AwbOp == ENABLE && (initFlg == 0 || initFlg == 2)) {
        //AmbaSample_AwbInit(chNo);
        AmbaImg_Proc_Cmd(MW_IP_AMBA_AEAWBADJ_INIT, chNo, 2, 0);
    }

    if (AaaOpInfo.AdjOp == ENABLE && (initFlg == 0 || initFlg == 3)) {

        AmbaSample_AdjInit(chNo);

        //AmbaImg_Proc_Cmd(MW_IP_AMBA_AEAWBADJ_INIT, chNo, 3, 0);
    }
    AmbaImg_Proc_Cmd(MW_IP_SET_VIDEO_PIPE_CTRL_PARAMS, chNo, (UINT32)&Mode, 0);
    Amba_Img_Get_Video_Pipe_Ctrl_UpdateFlg(chNo, &UpdateFlg);
    AppLibAeAwbAdj_Set_SchdlrExpInfo(chNo, &Mode, 255, UpdateFlg);
    AmbaImg_Proc_Cmd(MW_IP_RESET_VIDEO_PIPE_CTRL_FLAGS, chNo, 0, 0);
}

 /**
 *  @brief Entry point of AE/AWB/ADJ algo.
 *
 *  @param [in] chNo Handler for multi-task.
 *
 *  @return None
 */
void AppLibAeAwbAdj_Control(UINT32 chNo)
{
    AMBA_DSP_IMG_MODE_CFG_s Mode;
    AMBA_3A_OP_INFO_s   AaaOpInfo;
    UINT16 PipeMode = 0;
    UINT16 VideoAlgoMode = 0;
    AMBA_DSP_IMG_ALGO_MODE_e AlgoMode;
    UINT16 UpdateFlg = 0;
    UINT8  HdrEnable = 0;

    memset(&Mode, 0, sizeof(Mode));
    AmbaImg_Proc_Cmd(MW_IP_GET_MODE_CFG, chNo, 0, (UINT32)&Mode);
    AmbaImg_Proc_Cmd(MW_IP_GET_VIDEO_HDR_ENABLE, chNo, (UINT32)&HdrEnable, 0);
    if(HdrEnable == IMG_DSP_HDR_MODE_0){
        Mode.FuncMode = AMBA_DSP_IMG_FUNC_MODE_VHDR;
    }
    AmbaImg_Proc_Cmd(MW_IP_GET_AAA_OP_INFO, chNo, (UINT32)&AaaOpInfo, 0);
    
    AmbaImg_Proc_Cmd(MW_IP_GET_PIPE_MODE, chNo, (UINT32)&PipeMode, 0);
    AmbaImg_Proc_Cmd(MW_IP_GET_VIDEO_ALGO_MODE, (UINT32)chNo, (UINT32)&VideoAlgoMode, 0);
    if (VideoAlgoMode == IP_MODE_LISO_VIDEO) {
        AlgoMode = AMBA_DSP_IMG_ALGO_MODE_LISO;
    } else if (VideoAlgoMode == IP_MODE_HISO_VIDEO) {
        AlgoMode = AMBA_DSP_IMG_ALGO_MODE_HISO;
    } else {
        AlgoMode = AMBA_DSP_IMG_ALGO_MODE_FAST;
    }

    if(PipeMode == IP_HYBRID_MODE){
        Mode.ContextId = AmbaImgSchdlr_GetIsoCtxIndex(chNo, AlgoMode);
	//AmbaPrint("@@@@111111 chNo : %d, Mode.ContextId : %d @@@@", chNo, Mode.ContextId);
    } else {
        Mode.ContextId = 0; // TBD
    }
    
    if (AaaOpInfo.AdjOp == ENABLE) {
        Amba_Img_Get_Video_Pipe_Ctrl_UpdateFlg(chNo, &UpdateFlg);
        AmbaImg_Proc_Cmd(MW_IP_SET_VIDEO_PIPE_CTRL_PARAMS, chNo, (UINT32)&Mode, 0);
//        AmbaImg_Proc_Cmd(MW_IP_RESET_VIDEO_PIPE_CTRL_FLAGS, chNo, 0, 0);
    }

    AppLibAeAwbAdj_Set_SchdlrExpInfo(chNo,&Mode, 0, UpdateFlg);
    if (AaaOpInfo.AdjOp == ENABLE) {
        AmbaImg_Proc_Cmd(MW_IP_RESET_VIDEO_PIPE_CTRL_FLAGS, chNo, 0, 0);
    }

    //AmbaPrint("<%s>, %d", __FUNCTION__, chNo);
}

 /**
 *  @brief Entry point of Ae algo.
 *
 *  @param [in] chNo Handler for multi-task.
 *
 *  @return None
 */
void AppLibAe_Ctrl(UINT32 chNo)
{
    AMBA_3A_OP_INFO_s   AaaOpInfo;
    AMBA_3A_STATUS_s    VideoStatus = {0, 0, 0};
    AMBA_3A_STATUS_s    StillStatus = {0, 0, 0};

    memset(&AaaOpInfo, 0, sizeof(AMBA_3A_OP_INFO_s));
    //AmbaPrint("<%s>, %d", __FUNCTION__, chNo);
    AmbaImg_Proc_Cmd(MW_IP_GET_AAA_OP_INFO, chNo, (UINT32)&AaaOpInfo, 0);
    AmbaImg_Proc_Cmd(MW_IP_GET_3A_STATUS, chNo, (UINT32)&VideoStatus, (UINT32)&StillStatus);
    if (AaaOpInfo.AeOp == ENABLE) {
        {  /* Start of flicker detection */
            FLICKER_DETECT_STATUS_s     FlkDetectStatus;
            Img_Get_Flicker_Detection_Status(&FlkDetectStatus);
            if (FlkDetectStatus.Enable == ENABLE) {
                if (FlkDetectStatus.Running == STOP) {
                    Img_Flicker_Detection(DISABLE, 0);
                }
                Img_Flicker_Detection(FlkDetectStatus.Enable, 0);
            }
        }  /* End of flicker detection */

        AmbaImg_Proc_Cmd(MW_IP_AMBA_AE_CONTROL, chNo, (UINT32)&VideoStatus, (UINT32)&StillStatus);
        AmbaImg_Proc_Cmd(MW_IP_SET_AE_STATUS, chNo, (UINT32)VideoStatus.Ae, (UINT32)StillStatus.Ae);
    }

}

 /**
 *  @brief Entry point of Awb algo.
 *
 *  @param [in] chNo Handler for multi-task.
 *
 *  @return None
 */
void AppLibAwb_Ctrl(UINT32 chNo)
{
    AMBA_3A_OP_INFO_s   AaaOpInfo;
    AMBA_3A_STATUS_s    VideoStatus = {0, 0, 0};
    AMBA_3A_STATUS_s    StillStatus = {0, 0, 0};

    memset(&AaaOpInfo, 0, sizeof(AMBA_3A_OP_INFO_s));
    AmbaImg_Proc_Cmd(MW_IP_GET_AAA_OP_INFO, chNo, (UINT32)&AaaOpInfo, 0);
    AmbaImg_Proc_Cmd(MW_IP_GET_3A_STATUS, chNo, (UINT32)&VideoStatus, (UINT32)&StillStatus);
    if (AaaOpInfo.AwbOp == ENABLE) {
        //AmbaSample_AwbControl(chNo, &VideoStatus, &VideoStatus);
        AmbaImg_Proc_Cmd(MW_IP_AMBA_AWB_CONTROL, chNo, (UINT32)&VideoStatus, (UINT32)&StillStatus);
        //AmbaImg_Proc_Cmd(MW_IP_SET_3A_STATUS, chNo, (UINT32)&VideoStatus, (UINT32)&StillStatus);
        AmbaImg_Proc_Cmd(MW_IP_SET_AWB_STATUS, chNo, (UINT32)VideoStatus.Awb, (UINT32)StillStatus.Awb);
        //AmbaPrint("<%s>, %d", __FUNCTION__, chNo);
    }

}

/**
*  @brief Entry point of ADJ algo.
*
*  @param [in] chNo Handler for multi-task.
*
*  @return None
*/
void AppLibAdj_Ctrl(UINT32 chNo)
{
    AMBA_3A_OP_INFO_s   AaaOpInfo;
    AMBA_DSP_IMG_MODE_CFG_s Mode;

    memset(&AaaOpInfo, 0, sizeof(AMBA_3A_OP_INFO_s));
    memset(&Mode, 0, sizeof(Mode));
    AmbaImg_Proc_Cmd(MW_IP_GET_MODE_CFG, chNo, 0, (UINT32)&Mode);
    AmbaImg_Proc_Cmd(MW_IP_GET_AAA_OP_INFO, chNo, (UINT32)&AaaOpInfo, 0);
    if (AaaOpInfo.AdjOp == ENABLE) {
        AmbaSample_AdjControl(chNo);
        AppLibCalibAdjust_Func();
    }
}
