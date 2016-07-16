/**
 * @file app/cardv/applib/src/3a/applib_aaa/ambasample/AmbaSample_Adj.c
 *
 * Sample Auto Adjust algorithm
 *
 * History:
 *    2013/03/12 - [Jyh-Jiun Li] created file
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

#include <AmbaDSP_ImgFilter.h>
#include <AmbaDSP_Img3aStatistics.h>

#include <imgproc/AmbaImg_Proc.h>
#include <imgproc/AmbaImg_AeAwb.h>
#include "AmbaSample_AeAwbAdj.h"
#include <imgproc/AmbaImg_Impl_Cmd.h>
#include <imgproc/AmbaImg_Adjustment_A12.h>

static UINT32 ADJ_TABLE_NO = 0;
#ifdef CONFIG_APP_ARD
static UINT8 MADJEnable = 0;/**Default Enable TA at HDR Mode*/
#else
static UINT8 MADJEnable = 1;/**Default Enable TA at HDR Mode*/
#endif
static ADJ_STILL_CONTROL_s AmbaAdjStillCtrlPre[8];
/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaSample_AdjGetNfEv
 *
 *  @Description:: Calculate nfIndex, shutterIndex, and evIndex
 *
 *  @Input      ::
 *    UINT32 chNo : channel number
 *
 *  @Output     :: none
 *    UINT32 *nfIndex      : Pointer to nfIndex
 *    UINT32 *shutterIndex : Pointer to shutterIndex
 *    UINT32 *evIndex      : Pointer to evIndex
 *
 *  @Return     :: none
 *
\*-----------------------------------------------------------------------------------------------*/
void AmbaSample_AdjGetNfEv(UINT32 chNo,UINT32 *nfIndex,UINT32 *shutterIndex,UINT32 *evIndex)
{
    float       shutter, agc;
    UINT32      dgain;
    float       BaseRate = 1.001 / 30;

    AeGetVideoExposureValue(chNo, &shutter, &agc, &dgain);
    agc = agc * 1024 / 6;
    *nfIndex = (UINT32)agc * dgain / 4096;
    *shutterIndex = (UINT32)(shutter * 3 * 1024 / BaseRate);
    shutter = (UINT32)(shutter * 7 * 1024 / BaseRate);
    *evIndex = (UINT32)shutter + (*nfIndex);
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: _Decide_AdjTableNo
 *
 *  @Description:: decide ADJ table number
 *
 *  @Input      ::
 *    none
 *
 *  @Output     ::
 *   UINT32 *pAdjTableNo : pointer to ADJ table number
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
void _Decide_AdjTableNo(UINT32 *pAdjTableNo)
{
    /*
    *  Setup conditions to choose ADJ table number
    */
    *pAdjTableNo = ADJ_TABLE_NO;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaSample_AdjInit
 *
 *  @Description:: To initialize the ADJ algorithm
 *
 *  @Input      ::
 *    UINT32 chNo : channel number
 *
 *  @Output     :: none
 *
 *  @Return     :: none
 *
\*-----------------------------------------------------------------------------------------------*/
extern int Amba_Adj_Video_Init(UINT32 chNo);
void AmbaSample_AdjInit(UINT32 chNo)
{
    UINT32                    NfIndex = 0,ShutterIndex = 0,EvIndex = 0,IsoValue = 100,Dgain = 4096;
    UINT8                     ChkPhotoPreview = 0;
    ADJ_IQ_INFO_s             AdjVideoIqInfo;
    AMBA_DSP_IMG_WB_GAIN_s    WbGain;
    ADJ_VIDEO_PARAM_s         *AdjVideoAdd;
    ADJ_VIDEO_HISO_PARAM_s    *AdjVideoHIsoAdd;
    ADJ_PHOTO_PARAM_s         *AdjPhotoAdd;
    UINT32                    VideoAddTmp, TableNo = 0;
    AMBA_AE_INFO_s            AeInfotmp;
    UINT16 PipeMode = 0;
    UINT16 VideoAlgoMode = 0;
    AMBA_DSP_IMG_ALGO_MODE_e AlgoMode;
    AMBA_DSP_IMG_MODE_CFG_s   Mode;
    UINT8                     HdrEnable = 0;


    Amba_Adj_Video_Init(chNo);
    //Amba AE algo.
    //////////////////////////////////////////////////
    AmbaImg_Proc_Cmd(MW_IP_GET_AE_INFO, (UINT32)chNo, IP_MODE_VIDEO, (UINT32)&AeInfotmp);
    ShutterIndex = AeInfotmp.ShutterIndex;
    EvIndex = AeInfotmp.EvIndex;
    NfIndex = AeInfotmp.NfIndex;
    Dgain =  AeInfotmp.Dgain;
    IsoValue = AeInfotmp.IsoValue;
    //////////////////////////////////////////////////
    AmbaImg_Proc_Cmd(MW_IP_GET_VIDEO_ALGO_MODE, (UINT32)chNo, (UINT32)&VideoAlgoMode, 0);
    AmbaImg_Proc_Cmd(MW_IP_CHK_PHOTO_PREVIEW, (UINT32)&ChkPhotoPreview, 0, 0);
    //AmbaSample_AdjGetNfEv(chNo,&NfIndex,&ShutterIndex,&EvIndex);
    AmbaImg_Proc_Cmd(MW_IP_GET_VIDEO_HDR_ENABLE, chNo, (UINT32)&HdrEnable, 0);
    AmbaImg_Proc_Cmd(MW_IP_GET_PIPE_WB_GAIN, chNo, IP_MODE_VIDEO, (UINT32)&WbGain);

    AdjVideoIqInfo.Ae.ShutterIndex = ShutterIndex;
    AdjVideoIqInfo.Ae.EvIndex = EvIndex;
    AdjVideoIqInfo.Ae.NfIndex = NfIndex;
    AdjVideoIqInfo.Ae.Dgain = Dgain;
    AdjVideoIqInfo.Ae.IsoValue = IsoValue;
    AdjVideoIqInfo.Ae.Flash = 0;
    AdjVideoIqInfo.Wb = WbGain;

     if (ChkPhotoPreview == 0) {

	 #ifdef CONFIG_APP_ARD
	 if(MADJEnable==1 ) {
	 #else
      if(MADJEnable==1 && HdrEnable) {
	  #endif
            TableNo = 1;
	  } else {
		AmbaImg_Proc_Cmd(MW_IP_GET_ADJ_TABLE_NO, 0, (UINT32)&TableNo, 0);
		#ifdef CONFIG_APP_ARD
		// do nothing
		#else
		_Decide_AdjTableNo(&TableNo); //Get Adj Table No.
		#endif
		
	}

        if(VideoAlgoMode == IP_MODE_HISO_VIDEO){
            AmbaImg_Proc_Cmd(MW_IP_GET_ADJ_PARAMS_ADD,(UINT32)chNo, IQ_PARAMS_VIDEO_HISO_ADJ, (UINT32)&VideoAddTmp);
            AdjVideoHIsoAdd = (ADJ_VIDEO_HISO_PARAM_s *)VideoAddTmp;
            AdjVideoIqInfo.Mode = IP_PREVIEW_MODE;
            AdjVideoIqInfo.AwbAeParamAdd = (UINT32)&AdjVideoHIsoAdd[TableNo].NormalAwbAe;
            AdjVideoIqInfo.HisoFilterParamAdd = (UINT32)&AdjVideoHIsoAdd[TableNo].FilterParam;
            AdjVideoIqInfo.ColorParamAdd  = (UINT32)&AdjVideoHIsoAdd[TableNo].FilterParam.Def.Color;
        }else{
            AmbaImg_Proc_Cmd(MW_IP_GET_ADJ_PARAMS_ADD,(UINT32)chNo, IQ_PARAMS_VIDEO_ADJ, (UINT32)&VideoAddTmp);
            AdjVideoAdd = (ADJ_VIDEO_PARAM_s *)VideoAddTmp;
            AdjVideoIqInfo.Mode = IP_PREVIEW_MODE;
            AdjVideoIqInfo.AwbAeParamAdd = (UINT32)&AdjVideoAdd[TableNo].AwbAe;
            AdjVideoIqInfo.FilterParamAdd = (UINT32)&AdjVideoAdd[TableNo].FilterParam;
            AdjVideoIqInfo.ColorParamAdd  = (UINT32)&AdjVideoAdd[TableNo].FilterParam.Def.Color;
        }

    } else {

        AmbaImg_Proc_Cmd(MW_IP_GET_ADJ_TABLE_NO, 1, (UINT32)&TableNo, 0);
		#ifdef CONFIG_APP_ARD
		// do nothing
		#else
		_Decide_AdjTableNo(&TableNo); //Get Adj Table No.
		#endif
        AmbaImg_Proc_Cmd(MW_IP_GET_ADJ_PARAMS_ADD,(UINT32)chNo, IQ_PARAMS_PHOTO_ADJ, (UINT32)&VideoAddTmp);
        AdjPhotoAdd = (ADJ_PHOTO_PARAM_s *)VideoAddTmp;
        VideoAlgoMode = IP_MODE_LISO_VIDEO;
        AdjVideoIqInfo.Mode = IP_CAPTURE_MODE;
        AdjVideoIqInfo.AwbAeParamAdd = (UINT32)&AdjPhotoAdd[TableNo].NormalAwbAe;
        AdjVideoIqInfo.FilterParamAdd = (UINT32)&AdjPhotoAdd[TableNo].FilterParam;
        AdjVideoIqInfo.ColorParamAdd  = (UINT32)&AdjPhotoAdd[TableNo].FilterParam.Def.Color;

    }
    AmbaImg_Proc_Cmd(MW_IP_SET_IQ_INFO, (UINT32)chNo, (UINT32)IP_MODE_VIDEO , (UINT32)&AdjVideoIqInfo);
    if (VideoAlgoMode == IP_MODE_HISO_VIDEO) {
        AdjVideoIqInfo.Mode = IP_MODE_HISO_VIDEO;
    } else {
        AdjVideoIqInfo.Mode = IP_MODE_LISO_VIDEO;
    }

    AmbaImg_Proc_Cmd(MW_IP_ADJ_VIDEO_CONTROL, (UINT32)chNo , (UINT32)&AdjVideoIqInfo , 1);
    AmbaImg_Proc_Cmd(MW_IP_GET_MODE_CFG, chNo, 0, (UINT32)&Mode);
    //Get mode info.
    AmbaImg_Proc_Cmd(MW_IP_GET_PIPE_MODE, chNo, (UINT32)&PipeMode, 0);
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
    if(HdrEnable == IMG_DSP_HDR_MODE_0){
        AmbaSample_Adj_VideoHdr_Init();
        Mode.FuncMode = AMBA_DSP_IMG_FUNC_MODE_VHDR;
    } else {
        HDR_INFO_s HdrInfo;
        memset(&HdrInfo, 0, sizeof(HdrInfo));
        AmbaImg_Proc_Cmd(MW_IP_ADJ_VIDEO_HDR_INIT, 0, (UINT32)&HdrInfo, 0);
    }
    AmbaImg_Proc_Cmd(MW_IP_SET_VIDEO_PIPE_CTRL_PARAMS, chNo, (UINT32)&Mode, 0);
    //AmbaImg_Proc_Cmd(MW_IP_RESET_VIDEO_PIPE_CTRL_FLAGS, chNo, 0, 0);

}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaSample_AdjControl
 *
 *  @Description:: ADJ algorithm control API
 *
 *  @Input      ::
 *    UINT32 chNo : channel number
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
void AmbaSample_AdjControl(UINT32 chNo)
{
    UINT32                    NfIndex = 0,ShutterIndex = 0,EvIndex = 0,IsoValue = 100,Dgain = 4096;
    UINT8                     ChkPhotoPreview = 0;
    ADJ_IQ_INFO_s             AdjVideoIqInfo, AdjVideoIqInfo1;
    AMBA_DSP_IMG_WB_GAIN_s    WbGain;
    ADJ_VIDEO_PARAM_s         *AdjVideoAdd;
    ADJ_VIDEO_HISO_PARAM_s    *AdjVideoHIsoAdd;
    ADJ_PHOTO_PARAM_s         *AdjPhotoAdd;
    UINT32                    VideoAddTmp, TableNo = 0;
    AMBA_AE_INFO_s AeInfotmp;
    UINT16                    VideoAlgoMode = 0;
    UINT8                     HdrEnable = 0;

    //Amba AE algo.
    //////////////////////////////////////////////////
    AmbaImg_Proc_Cmd(MW_IP_GET_AE_INFO, (UINT32)chNo, IP_MODE_VIDEO, (UINT32)&AeInfotmp);
    ShutterIndex = AeInfotmp.ShutterIndex;
    EvIndex = AmbaImg_Proc_Cmd(MW_IP_EXPS_TO_EV_IDX, (UINT32)chNo, IP_MODE_VIDEO, (UINT32)&AeInfotmp);
    NfIndex = AmbaImg_Proc_Cmd(MW_IP_EXPS_TO_NF_IDX, (UINT32)chNo, IP_MODE_VIDEO, (UINT32)&AeInfotmp);
    Dgain =  AeInfotmp.Dgain;
    IsoValue = AeInfotmp.IsoValue;
    //////////////////////////////////////////////////
    //AmbaSample_AdjGetNfEv(chNo,&NfIndex,&ShutterIndex,&EvIndex);
    AmbaImg_Proc_Cmd(MW_IP_GET_VIDEO_ALGO_MODE, (UINT32)chNo, (UINT32)&VideoAlgoMode, 0);
    AmbaImg_Proc_Cmd(MW_IP_CHK_PHOTO_PREVIEW, (UINT32)&ChkPhotoPreview, 0, 0);
    AmbaImg_Proc_Cmd(MW_IP_GET_VIDEO_HDR_ENABLE, chNo, (UINT32)&HdrEnable, 0);
    AmbaImg_Proc_Cmd(MW_IP_GET_PIPE_WB_GAIN, chNo, IP_MODE_VIDEO, (UINT32)&WbGain);

    AdjVideoIqInfo.Ae.ShutterIndex = ShutterIndex;
    AdjVideoIqInfo.Ae.EvIndex = EvIndex;
    AdjVideoIqInfo.Ae.NfIndex = NfIndex;
    AdjVideoIqInfo.Ae.Dgain = Dgain;
    AdjVideoIqInfo.Ae.IsoValue = IsoValue;
    AdjVideoIqInfo.Ae.Flash = 0;
    AdjVideoIqInfo.Wb = WbGain;

    if (ChkPhotoPreview == 0) {

		#ifdef CONFIG_APP_ARD
		if(MADJEnable==1){
		#else
        if(MADJEnable==1 && HdrEnable) {
		#endif
            TableNo = 1;
	  } else {
		AmbaImg_Proc_Cmd(MW_IP_GET_ADJ_TABLE_NO, 0, (UINT32)&TableNo, 0);
		#ifdef CONFIG_APP_ARD
		// do nothing
		#else
		_Decide_AdjTableNo(&TableNo); //Get Adj Table No.
		#endif
        }

        if(VideoAlgoMode == IP_MODE_HISO_VIDEO){
            AmbaImg_Proc_Cmd(MW_IP_GET_ADJ_PARAMS_ADD,(UINT32)chNo, IQ_PARAMS_VIDEO_HISO_ADJ, (UINT32)&VideoAddTmp);
            AdjVideoHIsoAdd = (ADJ_VIDEO_HISO_PARAM_s *)VideoAddTmp;
            AdjVideoIqInfo.Mode = IP_PREVIEW_MODE;
            AdjVideoIqInfo.AwbAeParamAdd = (UINT32)&AdjVideoHIsoAdd[TableNo].NormalAwbAe;
            AdjVideoIqInfo.HisoFilterParamAdd = (UINT32)&AdjVideoHIsoAdd[TableNo].FilterParam;
            AdjVideoIqInfo.ColorParamAdd  = (UINT32)&AdjVideoHIsoAdd[TableNo].FilterParam.Def.Color;
        }else{
            AmbaImg_Proc_Cmd(MW_IP_GET_ADJ_PARAMS_ADD,(UINT32)chNo, IQ_PARAMS_VIDEO_ADJ, (UINT32)&VideoAddTmp);
            AdjVideoAdd = (ADJ_VIDEO_PARAM_s *)VideoAddTmp;
            AdjVideoIqInfo.Mode = IP_PREVIEW_MODE;
            AdjVideoIqInfo.AwbAeParamAdd = (UINT32)&AdjVideoAdd[TableNo].AwbAe;
            AdjVideoIqInfo.FilterParamAdd = (UINT32)&AdjVideoAdd[TableNo].FilterParam;
            AdjVideoIqInfo.ColorParamAdd  = (UINT32)&AdjVideoAdd[TableNo].FilterParam.Def.Color;
        }

    } else {

        AmbaImg_Proc_Cmd(MW_IP_GET_ADJ_TABLE_NO, 1, (UINT32)&TableNo, 0);
		#ifdef CONFIG_APP_ARD
		// do nothing
		#else
		_Decide_AdjTableNo(&TableNo); //Get Adj Table No.
		#endif
        AmbaImg_Proc_Cmd(MW_IP_GET_ADJ_PARAMS_ADD,(UINT32)chNo, IQ_PARAMS_PHOTO_ADJ, (UINT32)&VideoAddTmp);
        AdjPhotoAdd = (ADJ_PHOTO_PARAM_s *)VideoAddTmp;
        VideoAlgoMode = IP_MODE_LISO_VIDEO;
        AdjVideoIqInfo.Mode = IP_CAPTURE_MODE;
        AdjVideoIqInfo.AwbAeParamAdd = (UINT32)&AdjPhotoAdd[TableNo].NormalAwbAe;
        AdjVideoIqInfo.FilterParamAdd = (UINT32)&AdjPhotoAdd[TableNo].FilterParam;
        AdjVideoIqInfo.ColorParamAdd  = (UINT32)&AdjPhotoAdd[TableNo].FilterParam.Def.Color;

    }
    AmbaImg_Proc_Cmd(MW_IP_ADJ_AWBAE_CONTROL, (UINT32)chNo , (UINT32)&AdjVideoIqInfo , 0); //Amba only


    if(HdrEnable == IMG_DSP_HDR_MODE_0){
        AdjVideoIqInfo1 = AdjVideoIqInfo;
        //Amba AE algo.
        //////////////////////////////////////////////////
        AmbaImg_Proc_Cmd(MW_IP_GET_AE_INFO, (UINT32)chNo + 1, IP_MODE_VIDEO, (UINT32)&AeInfotmp);
        ShutterIndex = AeInfotmp.ShutterIndex;
        EvIndex = AmbaImg_Proc_Cmd(MW_IP_EXPS_TO_EV_IDX, (UINT32)chNo + 1, IP_MODE_VIDEO, (UINT32)&AeInfotmp);
        NfIndex = AmbaImg_Proc_Cmd(MW_IP_EXPS_TO_NF_IDX, (UINT32)chNo + 1, IP_MODE_VIDEO, (UINT32)&AeInfotmp);
        Dgain =  AeInfotmp.Dgain;
        IsoValue = AeInfotmp.IsoValue;
        //////////////////////////////////////////////////

        AdjVideoIqInfo1.Ae.ShutterIndex = ShutterIndex;
        AdjVideoIqInfo1.Ae.EvIndex = EvIndex;
        AdjVideoIqInfo1.Ae.NfIndex = NfIndex;
        AdjVideoIqInfo1.Ae.Dgain = Dgain;
        AdjVideoIqInfo1.Ae.IsoValue = IsoValue;
        AdjVideoIqInfo1.Ae.Flash = 0;
        AdjVideoIqInfo1.Wb = WbGain;
        AmbaImg_Proc_Cmd(MW_IP_ADJ_AWBAE_CONTROL, (UINT32)chNo + 1 , (UINT32)&AdjVideoIqInfo1 , 0); //Amba only, HDR
    }

    AmbaImg_Proc_Cmd(MW_IP_SET_IQ_INFO, (UINT32)chNo, (UINT32)IP_MODE_VIDEO , (UINT32)&AdjVideoIqInfo);
    if (VideoAlgoMode == IP_MODE_HISO_VIDEO) {
        AdjVideoIqInfo.Mode = IP_MODE_HISO_VIDEO;
    } else {
        AdjVideoIqInfo.Mode = IP_MODE_LISO_VIDEO;
    }

    //if(HdrEnable == IMG_DSP_HDR_MODE_0){
        //AmbaSample_Adj_VideoHdr_Control();
	//AmbaPrint("%s, chNo : %d", __FUNCTION__, chNo);
    //}
    AmbaImg_Proc_Cmd(MW_IP_ADJ_VIDEO_CONTROL, (UINT32)chNo , (UINT32)&AdjVideoIqInfo , 1);

}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaSample_AdjStillControl
 *
 *  @Description:: STILL ADJ algorithm control API
 *
 *  @Input      ::
 *    UINT32 chNo : channel number
 *    UINT32 aeIndx : Index to certain still AE information
 *    AMBA_DSP_IMG_MODE_CFG_s *mode : pointer to DSP control mode
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static float BaseStillBlcShtTime = 60.0;
void AmbaSample_AdjStillControl(UINT32 chNo, UINT32 aeIndx, AMBA_DSP_IMG_MODE_CFG_s *mode, UINT16 algoMode)
{
    /* Run Adj compute @ LISO */
    UINT8 StillAlgoMode = 0;
    UINT32 ChNoChk = 0;//, StillAdjTableNo = 0;
    UINT16 ShutterIndex = 0;
    AMBA_3A_OP_INFO_s AaaOpInfo;
    AMBA_AE_INFO_s stillAeInfo[MAX_AEB_NUM];
    ADJ_STILL_CONTROL_s adjStillCtrl;
    AMBA_DSP_IMG_WB_GAIN_s    StillWbGain = {WB_UNIT_GAIN,WB_UNIT_GAIN,WB_UNIT_GAIN,WB_UNIT_GAIN,WB_UNIT_GAIN};

    memset(&adjStillCtrl, 0x0, sizeof(ADJ_STILL_CONTROL_s));

    AmbaImg_Proc_Cmd(MW_IP_GET_AAA_OP_INFO, chNo, (UINT32)&AaaOpInfo, 0);

    if (chNo > 9) {
        ChNoChk = 9;
    } else {
        ChNoChk = chNo;
    }
    if (AaaOpInfo.AdjOp == 1) {
        memset(&adjStillCtrl, 0x0, sizeof(ADJ_STILL_CONTROL_s));
        AmbaImg_Proc_Cmd(MW_IP_GET_AE_INFO, chNo, IP_MODE_STILL, (UINT32)stillAeInfo);
        AmbaImg_Proc_Cmd(MW_IP_GET_PIPE_WB_GAIN, chNo, IP_MODE_STILL, (UINT32)&StillWbGain);

        StillAlgoMode = (algoMode == 0)? (IP_MODE_HISO_STILL): (IP_MODE_LISO_STILL);
        ShutterIndex = (UINT16)(log2(BaseStillBlcShtTime/stillAeInfo[aeIndx].ShutterTime) * 128);
        //AmbaPrint("\n\n\n<%s>, ShutterIndex : %4d ", __FUNCTION__, ShutterIndex);
        adjStillCtrl.StillMode = StillAlgoMode;//stillAeInfo[0].Mode;//IP_MODE_HISO_STILL;//IP_MODE_LISO_STILL;
        adjStillCtrl.ShIndex = ShutterIndex;//stillAeInfo[aeIndx].ShutterIndex;
        adjStillCtrl.EvIndex = stillAeInfo[aeIndx].EvIndex;
        adjStillCtrl.NfIndex = stillAeInfo[aeIndx].NfIndex;
        adjStillCtrl.WbGain = StillWbGain;
        adjStillCtrl.DZoomStep = 0;
        adjStillCtrl.FlashMode = 0;
        adjStillCtrl.LutNo = 0;

        AmbaPrint("\n\n StillMode : %d, ShIndex : %5d, EvIndex : %5d, NfIndex : %5d \n\n",
        adjStillCtrl.StillMode,adjStillCtrl.ShIndex,adjStillCtrl.EvIndex,adjStillCtrl.NfIndex);

        AmbaAdjStillCtrlPre[ChNoChk] = adjStillCtrl;

    } else if (AmbaAdjStillCtrlPre[ChNoChk].ShIndex == 0) {
        AmbaAdjStillCtrlPre[ChNoChk].StillMode = IP_MODE_LISO_STILL;
        AmbaAdjStillCtrlPre[ChNoChk].ShIndex = 1012;
        AmbaAdjStillCtrlPre[ChNoChk].EvIndex = 0;
        AmbaAdjStillCtrlPre[ChNoChk].NfIndex = 0;
        AmbaAdjStillCtrlPre[ChNoChk].WbGain = StillWbGain;
        AmbaAdjStillCtrlPre[ChNoChk].DZoomStep = 0;
        AmbaAdjStillCtrlPre[ChNoChk].FlashMode = 0;
        AmbaAdjStillCtrlPre[ChNoChk].LutNo = 0;
    }

    AmbaImg_Proc_Cmd(MW_IP_ADJ_STILL_CONTROL, chNo , (UINT32)&AmbaAdjStillCtrlPre[ChNoChk] , 0);
    AmbaImg_Proc_Cmd(MW_IP_SET_STILL_PIPE_CTRL_PARAMS, chNo , (UINT32)mode , 0);
}

void AmbaSample_Adj_VideoHdr_Init(void)
{
     HDR_INFO_s               HdrInfo;

     memset(&HdrInfo, 0, sizeof(HdrInfo));

     AmbaImg_Proc_Cmd(MW_IP_GET_AE_INFO, 0, IP_MODE_VIDEO, (UINT32)&HdrInfo.AeInfo[0]); //long
     AmbaImg_Proc_Cmd(MW_IP_GET_AE_INFO, 1, IP_MODE_VIDEO, (UINT32)&HdrInfo.AeInfo[1]); //short

     AmbaImg_Proc_Cmd(MW_IP_GET_PIPE_WB_GAIN, 0, IP_MODE_VIDEO, (UINT32)&HdrInfo.WbGain[0]); //long
     AmbaImg_Proc_Cmd(MW_IP_GET_PIPE_WB_GAIN, 0, IP_MODE_VIDEO, (UINT32)&HdrInfo.WbGain[1]); //short

     AmbaImg_Proc_Cmd(MW_IP_ADJ_VIDEO_HDR_INIT, 0, (UINT32)&HdrInfo, 0);
}
