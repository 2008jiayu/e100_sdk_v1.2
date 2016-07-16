/**
* @file src/app/connected/applib/src/3a/applib_iqparam_handler/AppLibIQParamHandler.c
*
*  Implementation of IQ param handler
*
* History:
*    07/16/2013 - [wkche] created file
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
#include <AmbaFS.h>
#include <AmbaUtility.h>
#include <common/common.h>

#include <imgproc/AmbaImg_Impl_Cmd.h>
#include <3a/iqparam/ApplibIQParamHandler.h>
#include <AmbaROM.h>

//#define DEBUG_APPLIB_IQ_PARAM
#if defined(DEBUG_APPLIB_IQ_PARAM)
#define DBGMSG AmbaPrint
#define DBGMSGc(x) AmbaPrintColor(GREEN,x)
#define DBGMSGc2 AmbaPrintColor
#else
#define DBGMSG(...)
#define DBGMSGc(...)
#define DBGMSGc2(...)
#endif

#define __AMBA_IQ_PARAM_HANDLER_PRINTK__ 0
#ifdef __AMBA_IQ_PARAM_HANDLER_PRINTK__
#define AppLibIQParamHandlerPrintk AmbaPrint
#else
#define AppLibIQParamHandlerPrintk(...)
#endif

int GVideoLoaded = 0;
int GPhotoLoaded = 0;

static    UINT16 IqphSceneModeCc3d = SYSTEM_DEFAULT;
static    UINT16 IqphDeCc3d = SYSTEM_DEFAULT;

PARAM_HANDLER_s *pParamHdlr[2];

GIpParam_Info_s GIpParamInfo[2];
UINT16 GIpParamCategoryMaxNum[GIpParamCategoryNum];
UINT16 GIpParamValidTableFlag[GIpParamCategoryNum];

IP_PARAM_s GIpParam_tmp_1[IP_PARAM_MAX_NUM] = {0};
IP_PARAM_s GIpParam_tmp_2[IP_PARAM_MAX_NUM] = {0};

IP_PARAM_s *pGIpParam[2] = {NULL,NULL};


int _Init_Color_De_Video_Table(UINT32 chNo)
{
    int idx = 0;
    //DE_PARAM_s *de = AppLibIQParam_Get_DE_PARAM(DE_PARAM_VIDEO,PARAM_LOADED);
    DE_PARAM_s *de;
    de = &(pParamHdlr[chNo]->IqphDeVideo);
    for (idx = 0; idx < DIGITAL_LAST; idx++) {
        pParamHdlr[chNo]->IqphCcDeVideoTable[idx] = de->DeInfo[idx].Cc3dNo;
        //AmbaPrint("<%s>, %d, DeCcIdx : %d", __FUNCTION__, idx, IqphCcDeVideoTable[idx]);
    }
    return 0;
}

/**
*  @brief  Get Video Adj Table path
*
*  @param [in] paramLoaded 0(PARAM_UNLOAD), 1(PARAM_LOADED)
*  @param [in] tableNo  load table number
*
*  @return Video Adj param table rom path.
*/
IP_TABLE_PATH_s* AppLibIQParam_Get_Adj_Video_Table_Path(UINT32 chNo, int paramLoaded, int tableNo)
{
    return pParamHdlr[chNo]->IqphParamProxy.GetAdjVideoTablePath(PARAM_UNLOAD,tableNo);
}

/**
*  @brief  Get Video High Iso Adj Table path
*
*  @param [in] paramLoaded 0(PARAM_UNLOAD), 1(PARAM_LOADED)
*  @param [in] tableNo  load table number
*
*  @return Video Adj param table rom path.
*/
IP_TABLE_PATH_s* AppLibIQParam_Get_Adj_Video_HIso_Table_Path(UINT32 chNo, int paramLoaded, int tableNo)
{
    return pParamHdlr[chNo]->IqphParamProxy.GetAdjVideoHIsoTablePath(PARAM_UNLOAD,tableNo);
}

/**
*  @brief  Get Photo Adj Table path
*
*  @param [in] paramLoaded 0(PARAM_UNLOAD), 1(PARAM_LOADED)
*  @param [in] tableNo   load table number
*
*  @return Photo Adj param table rom path.
*/
IP_TABLE_PATH_s* AppLibIQParam_Get_Adj_Photo_Table_Path(UINT32 chNo, int paramLoaded, int tableNo)
{
    return pParamHdlr[chNo]->IqphParamProxy.GetAdjPhotoTablePath(PARAM_UNLOAD,tableNo);
}

/**
*  @brief  Get Still Low Iso Adj Table path
*
*  @param [in] paramLoaded 0(PARAM_UNLOAD), 1(PARAM_LOADED)
*  @param [in] tableNo   load table number
*
*  @return Still Low Iso Adj param table rom path.
*/
IP_TABLE_PATH_s* AppLibIQParam_Get_Adj_Still_LIso_Table_Path(UINT32 chNo, int paramLoaded, int tableNo)
{
    return pParamHdlr[chNo]->IqphParamProxy.GetAdjStillLIsoTablePath(PARAM_UNLOAD,tableNo);
}

/**
*  @brief  Get still high iso Adj Table path
*
*  @param [in] paramLoaded 0(PARAM_UNLOAD), 1(PARAM_LOADED)
*  @param [in] tableNo   load table number
*
*  @return Video Adj param table rom path.
*/
IP_TABLE_PATH_s* AppLibIQParam_Get_Adj_Still_HIso_Table_Path(UINT32 chNo, int paramLoaded, int tableNo)
{
    return pParamHdlr[chNo]->IqphParamProxy.GetAdjStillHIsoTablePath(PARAM_UNLOAD,tableNo);
}

/**
*  @brief  Get scene data Table path
*
*  @param [in] paramLoaded 0(PARAM_UNLOAD), 1(PARAM_LOADED)
*  @param [in] tableNo   load table number
*
*  @return scene data table rom path.
*/
IP_TABLE_PATH_s* AppLibIQParam_Get_Scene_Data_Table_Path(UINT32 chNo, int paramLoaded, int tableNo)
{
    return pParamHdlr[chNo]->IqphParamProxy.GetSceneDataTablePath(PARAM_UNLOAD,tableNo);
}

/**
*  @brief  Get Img Table path
*
*
*  @return Img table rom path.
*/
IP_TABLE_PATH_s* AppLibIQParam_Get_Img_Table_Path(UINT32 chNo)
{
    return pParamHdlr[chNo]->IqphParamProxy.GetImgTablePath();
}

/**
*  @brief  Get Aaa Table path
*
*
*  @return Aaa table rom path.
*/
IP_TABLE_PATH_s* AppLibIQParam_Get_Aaa_Table_Path(UINT32 chNo, int tableNo)
{
    return pParamHdlr[chNo]->IqphParamProxy.GetAaaTablePath(tableNo);
}

/**
*  @brief  Get Adj_Still_Idx_Info Table path
*
*
*  @return Adj_Still_Idx_Info table rom path.
*/
IP_TABLE_PATH_s* AppLibIQParam_Get_Adj_Still_Idx_Info_Table_Path(UINT32 chNo)
{
    return pParamHdlr[chNo]->IqphParamProxy.GetAdjStillIdxInfoTablePath();
}

/**
*  @brief  Get Adj_Video_Idx_Info Table path
*
*
*  @return Adj_Video_Idx_Info rom path.
*/
IP_TABLE_PATH_s* AppLibIQParam_Get_Adj_Video_Idx_Info_Table_Path(UINT32 chNo)
{
    return pParamHdlr[chNo]->IqphParamProxy.GetAdjVideoIdxInfoTablePath();
}

/**
*  @brief  Get De Table path
*
*
*  @return De table rom path.
*/
IP_TABLE_PATH_s* AppLibIQParam_Get_De_Table_Path(UINT32 chNo, int mode)
{
    return pParamHdlr[chNo]->IqphParamProxy.GetDeTablePath(mode);
}

/**
*  @brief  Get Adj Table path
*
*
*  @return Adj table rom path.
*/
IP_TABLE_PATH_s* AppLibIQParam_Get_Adj_Table_Path(UINT32 chNo)
{
    return pParamHdlr[chNo]->IqphParamProxy.GetAdjTablePath();
}

/**
*  @brief Load IQ parameter table form ROM (bin files)
*
*  @param [in] pIpParam Iq parameters address
*  @param [in] pTablePath
*
*  @return >=0 success, <0 failure
*/
int _Load_Iq_Param_RomTable(IP_PARAM_s *pIpParam,const char* pTablePath)
{

    int RVal = -1;
    int FSize = 0;

//    FSize = AmbaROM_GetSize(AMBA_ROM_SYS_DATA, pIpParam->Name, 0x0);  //size
//                                                 //name     //read to memeory
//    RVal = AmbaROM_LoadByName(AMBA_ROM_SYS_DATA, pIpParam->Name, (UINT8*)pIpParam->Data, FSize, 0);

    FSize = AmbaROM_GetSize(AMBA_ROM_SYS_DATA, pTablePath, 0x0);  //size
                                                 //name     //read to memeory
    RVal = AmbaROM_LoadByName(AMBA_ROM_SYS_DATA, pTablePath , (UINT8*)pIpParam->Data, FSize, 0);
    if (RVal == -1) {
        AppLibIQParamHandlerPrintk("load IQ Params %s from ROMFS error :%d ",pIpParam->Name,FSize);
    } else {
        AppLibIQParamHandlerPrintk("load IQ Params %s from ROMFS: %d", pIpParam->Name, FSize);
    }

    return RVal;
}

/**
*  @brief  Validate parametere tables
*
*  @param [in] pIpParam  Pointer to Image parameter tables
*  @param [in] tableNo   load table number
*
*  @return >=0 success, <0 failure
*/
int _Validate_Param(UINT32 chNo, IP_PARAM_s *pIpParam, UINT16 tableNo)
{
    static IP_TABLE_PATH_s* pTablePath = NULL;

    if (pIpParam == 0) {
        AmbaPrintColor(RED,"_Validate_Param == NULL");
        return -1;
    }

    switch (pIpParam->Type) {
        case IMG_PARAM:
                pTablePath = AppLibIQParam_Get_Img_Table_Path(chNo);
                _Load_Iq_Param_RomTable(pIpParam,pTablePath->IpTablePath);
            break;
        case AAA_PARAM:
                pTablePath = AppLibIQParam_Get_Aaa_Table_Path(chNo,tableNo);
                _Load_Iq_Param_RomTable(pIpParam,pTablePath->IpTablePath);
            break;
        case ADJ_TABLE_PARAM:
                pTablePath = AppLibIQParam_Get_Adj_Table_Path(chNo);
                _Load_Iq_Param_RomTable(pIpParam,pTablePath->IpTablePath);
            break;
        case ADJ_PARAM_VIDEO:
                pTablePath =  AppLibIQParam_Get_Adj_Video_Table_Path(chNo, PARAM_UNLOAD,tableNo);
                _Load_Iq_Param_RomTable(pIpParam,pTablePath->IpTablePath);
            break;
		case ADJ_PARAM_HISO_VIDEO:
                pTablePath =  AppLibIQParam_Get_Adj_Video_HIso_Table_Path(chNo, PARAM_UNLOAD,tableNo);
                _Load_Iq_Param_RomTable(pIpParam,pTablePath->IpTablePath);
            break;
        case ADJ_PARAM_PHOTO:
                pTablePath =  AppLibIQParam_Get_Adj_Photo_Table_Path(chNo, PARAM_UNLOAD,tableNo);
                _Load_Iq_Param_RomTable(pIpParam,pTablePath->IpTablePath);
            break;
        case ADJ_PARAM_LISO_STILL:
                pTablePath =  AppLibIQParam_Get_Adj_Still_LIso_Table_Path(chNo, PARAM_UNLOAD,tableNo);
                _Load_Iq_Param_RomTable(pIpParam,pTablePath->IpTablePath);
            break;
        case ADJ_PARAM_HISO_STILL:
                pTablePath =  AppLibIQParam_Get_Adj_Still_HIso_Table_Path(chNo, PARAM_UNLOAD,tableNo);
                _Load_Iq_Param_RomTable(pIpParam,pTablePath->IpTablePath);
            break;
        case ADJ_PARAM_STILL_IDX:
            	    pTablePath = AppLibIQParam_Get_Adj_Still_Idx_Info_Table_Path(chNo);
		    _Load_Iq_Param_RomTable(pIpParam,pTablePath->IpTablePath);
            break;
		case ADJ_PARAM_VIDEO_IDX:
		    pTablePath =  AppLibIQParam_Get_Adj_Video_Idx_Info_Table_Path(chNo);
                _Load_Iq_Param_RomTable(pIpParam,pTablePath->IpTablePath);
            break;
        case SCENE_DATA:
                pTablePath =  AppLibIQParam_Get_Scene_Data_Table_Path(chNo, PARAM_UNLOAD,tableNo);
                _Load_Iq_Param_RomTable(pIpParam,pTablePath->IpTablePath);
            break;
        case DE_PARAM_VIDEO:
                pTablePath =  AppLibIQParam_Get_De_Table_Path(chNo, IP_MODE_VIDEO);
                _Load_Iq_Param_RomTable(pIpParam,pTablePath->IpTablePath);
            break;
        case DE_PARAM_STILL:
                pTablePath =  AppLibIQParam_Get_De_Table_Path(chNo, IP_MODE_VIDEO);
                _Load_Iq_Param_RomTable(pIpParam,pTablePath->IpTablePath);
            break;
//        case ADJ_CALIB_PARAM:
//            pIpParam->Data = (CALIBRATION_PARAM_s*)AppLibIQParam_Get_Calib_Param();
//            break;
        default:
            break;

    }
    if (pIpParam->Data == NULL) {
        AmbaPrintColor(RED, "[Applib - IQParam] _Validate_Param IpParamTmp->Data == NULL  type = %d",pIpParam->Type);
    }
    return 0;
}

/**
*  @brief  Initialize parametere tables number
*
*
*  @return None
*/
void _Init_GIpParam_Flag(void)
{
    UINT16 i = 0;
    for (i=0;i<GIpParamCategoryNum;i++) {
        GIpParamCategoryMaxNum[i] =1;
        GIpParamValidTableFlag[i] =1;
    }

    GIpParamCategoryMaxNum[GIpParamCategory_ADJ_STILL]= ADJ_STILL_LISO_TABLE_MAX_NO;
    GIpParamCategoryMaxNum[GIpParamCategory_ADJ_HISO_STILL]= ADJ_STILL_HISO_TABLE_MAX_NO;
    GIpParamCategoryMaxNum[GIpParamCategory_ADJ_VIDEO]= ADJ_VIDEO_TABLE_MAX_NO;
    GIpParamCategoryMaxNum[GIpParamCategory_ADJ_PHOTO]= ADJ_PHOTO_TABLE_MAX_NO;
    GIpParamCategoryMaxNum[GIpParamCategory_SCENE]= SCENE_TABLE_MAX_NO;

    GIpParamValidTableFlag[GIpParamCategory_ADJ_STILL]= ADJ_STILL_LISO_TABLE_VALID_NO;
    GIpParamValidTableFlag[GIpParamCategory_ADJ_HISO_STILL]= ADJ_STILL_HISO_TABLE_VALID_NO;
    GIpParamValidTableFlag[GIpParamCategory_ADJ_VIDEO]= ADJ_VIDEO_TABLE_VALID_NO;
    GIpParamValidTableFlag[GIpParamCategory_ADJ_PHOTO]= ADJ_PHOTO_TABLE_VALID_NO;
    GIpParamValidTableFlag[GIpParamCategory_SCENE]= SCENE_TABLE_VALID_NO;

}

/**
*  @brief  Retrieve Cc table address
*
*  @param [in] type IMG_MODE_TV(TV mode), IMG_MODE_PC(PC mode)
*  @param [in] ccIdx  Cc table number
*
*  @return Cc table address
*/
UINT32 _Get_Cc_Table_Addr(UINT32 chNo, int type, int ccIdx)
{
    if (ccIdx < DEF_CC_TABLE_NO) {
        switch (type) {
        case IMG_MODE_TV:
            return (UINT32)&(pParamHdlr[chNo]->IqphVideoCcAddr[ccIdx]);
        case IMG_MODE_PC:
            return (UINT32)&(pParamHdlr[chNo]->IqphStillCcAddr[ccIdx]);
        default:
            //printk("unknown type");
            return 0;
        }
    }
    return 0;
}

/**
*  @brief  Initialize Video Adj table Cc table address
*
*
*  @return >=0 success, <0 failure
*/
int _Init_Adj_Video_Color_Table_Addr(UINT32 chNo) {
    int i, CcIdx, CcType;
    ADJ_VIDEO_PARAM_s *pAdjVideo = 0;
    ADJ_VIDEO_HISO_PARAM_s *pAdjVideoHiso = 0;
    int tableNum = 0;

    /* Assign VIDEO ADJ color table address */
    //pAdjVideo = AppLibIQParam_Get_Adj_Video_Param(GVideoLoaded,tableNum);
    pAdjVideo = &(pParamHdlr[chNo]->IqphAdjVideo[tableNum]);

    if(pAdjVideo){
        for (i=0; i<ADJ_VIDEO_TABLE_VALID_NO; i++) {
                if (pAdjVideo) {
                    CcType = pAdjVideo[i].FilterParam.Def.Color.Type;
                    for (CcIdx=0; CcIdx<DEF_CC_TABLE_NO; CcIdx++) {
                        pAdjVideo[i].FilterParam.Def.Color.Table[CcIdx].MatrixThreeDTableAddr =
                            _Get_Cc_Table_Addr(chNo, CcType, CcIdx);
                    }
                }
            }
    }

    /* High-ISO mode */
    //pAdjVideoHiso = AmbaIQParam_Get_Adj_Video_High_Iso_Param(GVideoLoaded, 0); //FIXME
    pAdjVideoHiso = &(pParamHdlr[chNo]->IqphAdjHisoVideo[tableNum]);
    if(pAdjVideoHiso){
        for (i=0; i<ADJ_VIDEO_HISO_TABLE_VALID_NO; i++) {
            if (pAdjVideoHiso) {
                CcType = pAdjVideoHiso[i].FilterParam.Def.Color.Type;
                for (CcIdx=0; CcIdx<DEF_CC_TABLE_NO; CcIdx++) {
                    pAdjVideoHiso[i].FilterParam.Def.Color.Table[CcIdx].MatrixThreeDTableAddr =
                    _Get_Cc_Table_Addr(chNo, CcType, CcIdx);
                }
            }
        }
    }
    return 0;
}


/**
*  @brief  Initialize Still Adj table Cc table address
*
*
*  @return >=0 success, <0 failure
*/
int _Init_Adj_Still_Color_Table_Addr(UINT32 chNo) {
    int CcIdx = 0, CcType = 0, LutIdx = 0;
    ADJ_STILL_LOW_ISO_PARAM_s *pAdjStillLiso = 0;
    ADJ_STILL_HISO_PARAM_s * pAdjStillHiso = 0;

    /* Assign STILL ADJ color table address */
    /* Low-ISO mode */
    //pAdjStillLiso = AppLibIQParam_Get_Adj_Still_Low_Iso_Param(PARAM_LOADED, 0); //FIXME
    pAdjStillLiso = &(pParamHdlr[chNo]->IqphAdjLisoStill[0]);
    if (pAdjStillLiso) {
        for (LutIdx = 0; LutIdx < ADJ_STILL_LISO_TABLE_VALID_NO; LutIdx++) {
            CcType = pAdjStillLiso[LutIdx].Def.Color.Type;
            for (CcIdx=0; CcIdx<DEF_CC_TABLE_NO; CcIdx++) {
                pAdjStillLiso[LutIdx].Def.Color.Table[CcIdx].MatrixThreeDTableAddr =
                _Get_Cc_Table_Addr(chNo, CcType, CcIdx);
            }
        }
    }

    /* High-ISO mode */
    //pAdjStillHiso = AppLibIQParam_Get_Adj_Still_High_Iso_Param(PARAM_LOADED, 0); //FIXME
    pAdjStillHiso = &(pParamHdlr[chNo]->IqphAdjHisoStill[0]);
    if (pAdjStillHiso) {
        for (LutIdx = 0; LutIdx < ADJ_STILL_LISO_TABLE_VALID_NO; LutIdx++) {
            CcType = pAdjStillHiso[LutIdx].Def.Color.Type;
            for (CcIdx=0; CcIdx<5; CcIdx++) {
                pAdjStillHiso[LutIdx].Def.Color.Table[CcIdx].MatrixThreeDTableAddr =
               _Get_Cc_Table_Addr(chNo, CcType, CcIdx);
            }
        }
    }

    return 0;
}


/**
*  @brief  Initialize Cc table address
*
*  @param [in] modeSwitch mode switch
*
*  @return >=0 success, <0 failure
*/
int _Init_Color_Table_Addr(UINT32 chNo)
{
    int i = 0, CcIdx = 0, CcType = 0;
    IMG_PARAM_s *Img = 0;
    ADJ_PHOTO_PARAM_s *AdjPhoto = 0;

    //Img = AppLibIQParam_Get_Img_Param(PARAM_LOADED);
    Img = &(pParamHdlr[chNo]->IqphImg);
    if (Img) {
        /* Assign Image Param color table address */
        CcIdx = 1; // use index 1 as default
        CcType = IMG_MODE_TV;
        Img->ColorCorrVideo.MatrixThreeDTableAddr =
            _Get_Cc_Table_Addr(chNo, CcType, CcIdx);
        CcType = IMG_MODE_PC;
        Img->ColorCorrStill.MatrixThreeDTableAddr =
            _Get_Cc_Table_Addr(chNo, CcType, CcIdx);
    }

    /* Assign PHOTO ADJ color table address */
    //AdjPhoto = AppLibIQParam_Get_Adj_Photo_Param(PARAM_LOADED, 0); //FIXME
    AdjPhoto = &(pParamHdlr[chNo]->IqphAdjPhoto[0]);
    for (i=0; i< ADJ_PHOTO_TABLE_VALID_NO; i++) {
        if (AdjPhoto) {
            CcType = AdjPhoto[i].FilterParam.Def.Color.Type;
            for (CcIdx = 0; CcIdx < DEF_CC_TABLE_NO; CcIdx++) {
                AdjPhoto[i].FilterParam.Def.Color.Table[CcIdx].MatrixThreeDTableAddr =
                _Get_Cc_Table_Addr(chNo, CcType, CcIdx);
            }
        }
    }

    _Init_Adj_Video_Color_Table_Addr(chNo);
    _Init_Adj_Still_Color_Table_Addr(chNo);

    return 0;
}


/**
*  @brief  Load Still Color table
*
*  @param [in] colorIdx color table index
*  @param [in] post
*
*  @return >=0 success, <0 failure
*/
int _Load_Still_Color_Table(UINT32 chNo,int colorIdx, int post)
{
    //static int              PreColorIdx1 = 16383;
    int                     RVal = -1;
    int                     i, FSize;
    COLOR_TABLE_PATH_s      *OneColorCcPath;

    OneColorCcPath = pParamHdlr[chNo]->IqphParamProxy.GetColorTablePath();

    for (i=0; i< DEF_CC_TABLE_NO; i++) {
        FSize = AmbaROM_GetSize(AMBA_ROM_SYS_DATA, OneColorCcPath[colorIdx].Still[i], 0x0);
        RVal = AmbaROM_LoadByName(AMBA_ROM_SYS_DATA, OneColorCcPath[colorIdx].Still[i], pParamHdlr[chNo]->IqphStillCcAddr[i], FSize, 0);
        if (RVal == -1) {
            AppLibIQParamHandlerPrintk("load still cc 3D from ROMFS error, %d", i);
        } else {
            AppLibIQParamHandlerPrintk("load still cc 3D %s from ROMFS: %d", OneColorCcPath[colorIdx].Still[i], FSize);
        }
    }

    return RVal;
}


/**
*  @brief  Load Video Color table
*
*  @param [in] colorIdx color table index
*  @param [in] post
*
*  @return >=0 success, <0 failure
*/
int _Load_Video_Color_Table(UINT32 chNo,int colorIdx, int post)
{
    //static int              PreColorIdx0 = 16383;
    int RVal = -1;
    int i = 0, FSize = 0;
    COLOR_TABLE_PATH_s      *OneColorCcPath;

    OneColorCcPath = pParamHdlr[chNo]->IqphParamProxy.GetColorTablePath();

    for (i=0; i< DEF_CC_TABLE_NO; i++) {
        FSize = AmbaROM_GetSize(AMBA_ROM_SYS_DATA, OneColorCcPath[colorIdx].Video[i], 0x0);
        RVal = AmbaROM_LoadByName(AMBA_ROM_SYS_DATA, OneColorCcPath[colorIdx].Video[i], pParamHdlr[chNo]->IqphVideoCcAddr[i], FSize, 0);
        if (RVal == -1) {
            AppLibIQParamHandlerPrintk("load video cc 3D from ROMFS error, %d", i);
        } else {
            AppLibIQParamHandlerPrintk("load video cc 3D %s from ROMFS: %d", OneColorCcPath[colorIdx].Video[i], FSize);
        }
    }

    _Load_Still_Color_Table(chNo, colorIdx,0);

    return RVal;
}

int  AppLibIQParam_Get_ColorIdx_By_De(UINT32 chNo,int deMode)
{
    int ColorIdx = 0;

    if (deMode >= DIGITAL_LAST) {
        AmbaPrintColor(RED, "digital_effect %d exceeds max. digital effect", deMode);
        return -1;
    }

    ColorIdx = pParamHdlr[chNo]->IqphCcDeVideoTable[deMode];
    IqphDeCc3d = ColorIdx;
    if (IqphDeCc3d == SYSTEM_DEFAULT && IqphSceneModeCc3d == SYSTEM_DEFAULT) {
        ColorIdx = 0;
    } else if (IqphDeCc3d == SYSTEM_DEFAULT && IqphSceneModeCc3d != SYSTEM_DEFAULT) {
        ColorIdx = IqphSceneModeCc3d;
    }
    DBGMSG("[AppLib - IQParam] Digital Effect CC table : %d",ColorIdx);

    return ColorIdx;
}




int AppLibIQParam_DigitalEffect_Load_Color_Table(UINT32 chNo,int DigitalEffect, int post, UINT8 modeSwitch)
{
        int RVal = -1;
        int ColorIdx = 0;

    ColorIdx = AppLibIQParam_Get_ColorIdx_By_De(chNo, DigitalEffect);
    RVal = _Load_Video_Color_Table(chNo, ColorIdx, post);
    // If user switch digital effect or scene mode then it need to load cc still table
    if (modeSwitch)
            _Load_Still_Color_Table(chNo, ColorIdx,0);
    return RVal;
}

int _Load_Cc_Reg_Table(UINT32 chNo)
{
    int RVal = -1;
    int FSize = 0;

    FSize = AmbaROM_GetSize(AMBA_ROM_SYS_DATA, pParamHdlr[chNo]->IqphCcRegID, 0x0);
    RVal = AmbaROM_LoadByName(AMBA_ROM_SYS_DATA, pParamHdlr[chNo]->IqphCcRegID, &(pParamHdlr[chNo]->IqphCcReg[0]), FSize, 0);

    if (RVal == -1) {
        AppLibIQParamHandlerPrintk("load video Cc Reg from ROMFS error");
    } else {
        AppLibIQParamHandlerPrintk("load Cc Reg %s from ROMFS: %d, RVal :%d", (pParamHdlr[chNo]->IqphCcRegID), FSize, RVal);
    }
    AmbaImg_Proc_Cmd(MW_IP_SET_CC_REG_ADD, 0, (UINT32)&(pParamHdlr[chNo]->IqphCcReg[0]), 0);
    return RVal;
}

/**
*  @brief Remove the Param Proxy instance.
*
*
*  @return >=0 success, <0 failure
*/
int AppLibIQParam_Remove(UINT32 chNo)
{
    memset(&(pParamHdlr[chNo]->IqphParamProxy), 0x0, sizeof(PARAM_PROXY_s));
    return 0;
}


/**
*  @brief  Attach the Sensor input device and enable the device control.
*
*  @param [in] par pionter to param proxy
*
*  @return >=0 success, <0 failure
*/
int AppLibIQParam_Attach(UINT32 chNo, PARAM_PROXY_s *par)
{
    if (par == NULL)
        return -1;

    AppLibIQParam_Remove(chNo);
    memcpy(&pParamHdlr[chNo]->IqphParamProxy, par, sizeof(PARAM_PROXY_s));
    return 0;
}


/**
*  @brief  Attach the Sensor input device and enable the device control.
*
*  @param [in] param
*  @param [in] name  Pointer to param name
*  @param [in] type  Param type
*
*  @return >=0 success, <0 failure
*/
int AppLibIQParam_Get_By_Name(UINT32 ParamHandlerNo, const char *name, const int type, IP_PARAM_s **param)
{
    int i = 0;

    *param = NULL;
    for (i = 0; pGIpParam[ParamHandlerNo][i].Length >= 0; i++) {
        if (strcmp(name, pGIpParam[ParamHandlerNo][i].Name) == 0) {
            if (type != pGIpParam[ParamHandlerNo][i].Type) {
                AmbaPrintColor(RED, "AppLibIQParam_Get_By_Name: type mismatch");
                return -1;
            }
            *param = &pGIpParam[ParamHandlerNo][i];
#if 1
            if (*param == NULL) {
                AmbaPrintColor(RED, "AppLibIQParam_Get_By_Name: *param == NULL");
                return -1;
            } else {
                DBGMSGc2(GREEN, "AppLibIQParam_Get_By_Name: type match = %d", i);
                return 0;
            }
#else
            return 0;
#endif
        }
    }

    return -1;
}


/**
*  @brief  init color scene table
*
*
*  @return >=0 success, <0 failure
*/
int _Init_Color_Scene_Table(UINT32 chNo)
{
    int SceneIdx = 0;
    SCENE_DATA_s *Scene = NULL;

    SceneIdx = 0;
    //Scene = AppLibIQParam_Get_Scene_Data(PARAM_LOADED,0);
    Scene = &(pParamHdlr[chNo]->IqphSceneData[0]);
    for (SceneIdx = 0; SceneIdx < SCENE_TABLE_TATOL_NO; SceneIdx++) {
        pParamHdlr[chNo]->IqphCcSceneTable[SceneIdx] = Scene[SceneIdx].Def.ColorTable;
        SceneIdx++;
    }

    return 0;
}

/**
*  @brief get cc table index for scene mode
*
*  @param [in] sceneMode scene mode
*
*  @return cc table index
*/
int AppLibIQParam_Get_Color_Idx_By_Scene(UINT32 chNo,int sceneMode)
{
    int ColorIdx = 0;

    if (sceneMode == SCENE_AUTO) {
        sceneMode = SCENE_OFF;
    }
    if (sceneMode >= SCENE_LAST) {
        AmbaPrintColor(RED,"sceneMode %d exceeds max. scene mode", sceneMode);
        return -1;
    }

    IqphSceneModeCc3d = pParamHdlr[chNo]->IqphCcSceneTable[sceneMode];
    ColorIdx = IqphSceneModeCc3d;

      if (IqphDeCc3d != SYSTEM_DEFAULT) {
          ColorIdx = IqphDeCc3d;
      } else if (IqphSceneModeCc3d == SYSTEM_DEFAULT) {
          ColorIdx = 0;
      }

    DBGMSG("[AppLib - IQParam] Scene Mode CC table : %d",ColorIdx);

    return ColorIdx;
}

/**
*  @brief  load  cc table index
*
*  @param [in] sceneMode select scene mode
*  @param [in] post
*  @param [in] modeSwitch  select mode
*
*  @return >=0 success, <0 failure
*/
int AppLibIQParam_Scene_Mode_Load_Color_Table(UINT32 chNo, int sceneMode, int post, UINT8 modeSwitch)
{
    int Rval = -1;
    int ColorIdx = 0;

    ColorIdx = AppLibIQParam_Get_Color_Idx_By_Scene(chNo, sceneMode);
    Rval = _Load_Video_Color_Table(chNo, ColorIdx, post);
    // If user switch digital effect or scene mode then it need to load cc still table
    if (modeSwitch)
    _Load_Still_Color_Table(chNo, ColorIdx,0);

    return Rval;
}

void _Init_GIpParam_Flag_Test(GIpParam_Info_s *pInfo)
{
    UINT16 i;
    for(i=0;i<GIpParamCategoryNum;i++) {
        pInfo->GIpParamCategoryMaxNum[i] =1;
        pInfo->GIpParamValidTableFlag[i] =1;
    }

    pInfo->GIpParamCategoryMaxNum[GIpParamCategory_AAA]= AAA_TABLE_MAX_NO;
    pInfo->GIpParamCategoryMaxNum[GIpParamCategory_ADJ_STILL]= ADJ_STILL_LISO_TABLE_MAX_NO;
    pInfo->GIpParamCategoryMaxNum[GIpParamCategory_ADJ_HISO_STILL]= ADJ_STILL_HISO_TABLE_MAX_NO;
    pInfo->GIpParamCategoryMaxNum[GIpParamCategory_ADJ_VIDEO]= ADJ_VIDEO_TABLE_MAX_NO;
    pInfo->GIpParamCategoryMaxNum[GIpParamCategory_ADJ_PHOTO]= ADJ_PHOTO_TABLE_MAX_NO;
    pInfo->GIpParamCategoryMaxNum[GIpParamCategory_SCENE]= SCENE_TABLE_MAX_NO;

    pInfo->GIpParamValidTableFlag[GIpParamCategory_AAA]= AAA_TABLE_VALID_NO;
    pInfo->GIpParamValidTableFlag[GIpParamCategory_ADJ_STILL]= ADJ_STILL_LISO_TABLE_VALID_NO;
    pInfo->GIpParamValidTableFlag[GIpParamCategory_ADJ_HISO_STILL]= ADJ_STILL_HISO_TABLE_VALID_NO;
    pInfo->GIpParamValidTableFlag[GIpParamCategory_ADJ_VIDEO]= ADJ_VIDEO_TABLE_VALID_NO;
    pInfo->GIpParamValidTableFlag[GIpParamCategory_ADJ_PHOTO]= ADJ_PHOTO_TABLE_VALID_NO;
    pInfo->GIpParamValidTableFlag[GIpParamCategory_SCENE]= SCENE_TABLE_VALID_NO;
}

void _Assgin_Data_Addr(UINT32 paramHandlerNo, IP_PARAM_s *pGIpParamTmp)
{
    int i=0,j;
    char strtemp[10];
    for(j=0;j<IMG_TABLE_MAX_NO;j++){
        strcpy((pGIpParamTmp+i)->Name,"img_default");
        (pGIpParamTmp+i)->Type = IMG_PARAM;
        (pGIpParamTmp+i)->Data = &(pParamHdlr[paramHandlerNo]->IqphImg);
        (pGIpParamTmp+i)->Length = sizeof(IMG_PARAM_s);
        i++;
    }
    for(j=0;j<AAA_TABLE_MAX_NO;j++){
        sprintf(strtemp,"%02d",j);
        strcpy((pGIpParamTmp+i)->Name,"aaa_default_");
        strcat((pGIpParamTmp+i)->Name,strtemp);
        (pGIpParamTmp+i)->Type = AAA_PARAM;
        (pGIpParamTmp+i)->Data = &(pParamHdlr[paramHandlerNo]->IqphAaaDefault[j]);
        (pGIpParamTmp+i)->Length = sizeof(AAA_PARAM_s);
        i++;
    }
    for(j=0;j<ADJ_STILL_LISO_TABLE_MAX_NO;j++){
        sprintf(strtemp,"%02d",j);
        strcpy((pGIpParamTmp+i)->Name,"adj_still_default_");
        strcat((pGIpParamTmp+i)->Name,strtemp);
        (pGIpParamTmp+i)->Type = ADJ_PARAM_LISO_STILL;
        (pGIpParamTmp+i)->Data = &(pParamHdlr[paramHandlerNo]->IqphAdjLisoStill[j]);
        (pGIpParamTmp+i)->Length = sizeof(ADJ_STILL_LOW_ISO_PARAM_s);
        i++;
    }
    for(j=0;j<ADJ_STILL_HISO_TABLE_MAX_NO;j++){
        sprintf(strtemp,"%02d",j);
        strcpy((pGIpParamTmp+i)->Name,"adj_hiso_still_default_");
        strcat((pGIpParamTmp+i)->Name,strtemp);
        (pGIpParamTmp+i)->Type = ADJ_PARAM_HISO_STILL;
        (pGIpParamTmp+i)->Data  = &(pParamHdlr[paramHandlerNo]->IqphAdjHisoStill[j]);
        (pGIpParamTmp+i)->Length = sizeof(ADJ_STILL_HISO_PARAM_s);
        i++;
    }
    for(j=0;j<ADJ_VIDEO_TABLE_MAX_NO;j++){
        sprintf(strtemp,"%02d",j);
        strcpy((pGIpParamTmp+i)->Name,"adj_video_default_");
        strcat((pGIpParamTmp+i)->Name,strtemp);
        (pGIpParamTmp+i)->Type = ADJ_PARAM_VIDEO;
        (pGIpParamTmp+i)->Data  = &(pParamHdlr[paramHandlerNo]->IqphAdjVideo[j]);
        (pGIpParamTmp+i)->Length = sizeof(ADJ_VIDEO_PARAM_s);
        i++;
    }
    for(j=0;j<ADJ_PHOTO_TABLE_MAX_NO;j++){
        sprintf(strtemp,"%02d",j);
        strcpy((pGIpParamTmp+i)->Name,"adj_photo_default_");
        strcat((pGIpParamTmp+i)->Name,strtemp);
        (pGIpParamTmp+i)->Type = ADJ_PARAM_PHOTO;
        (pGIpParamTmp+i)->Data  = &(pParamHdlr[paramHandlerNo]->IqphAdjPhoto[j]);
        (pGIpParamTmp+i)->Length = sizeof(ADJ_PHOTO_PARAM_s);
        i++;
    }
    for(j=0;j<ADJ_STILL_IDX_INFO_TABLE_MAX_NO;j++){
        strcpy((pGIpParamTmp+i)->Name,"adj_still_idx");
        (pGIpParamTmp+i)->Type = ADJ_PARAM_STILL_IDX;
        (pGIpParamTmp+i)->Data  = &(pParamHdlr[paramHandlerNo]->IqphAdjStillIdx);
        (pGIpParamTmp+i)->Length = sizeof(ADJ_STILL_IDX_INFO_s);
        i++;
    }
    for(j=0;j<ADJ_VIDEO_IDX_INFO_TABLE_MAX_NO;j++){
        strcpy((pGIpParamTmp+i)->Name,"adj_video_idx");
        (pGIpParamTmp+i)->Type = ADJ_PARAM_VIDEO_IDX;
        (pGIpParamTmp+i)->Data  = &(pParamHdlr[paramHandlerNo]->IqphAdjVideoIdx);
        (pGIpParamTmp+i)->Length = sizeof(ADJ_VIDEO_IDX_INFO_s);
        i++;
    }
    for(j=0;j<SCENE_TABLE_MAX_NO;j++){
        sprintf(strtemp,"%02d",(j+1));
        strcpy((pGIpParamTmp+i)->Name,"scene_data_s");
        strcat((pGIpParamTmp+i)->Name,strtemp);
        (pGIpParamTmp+i)->Type = SCENE_DATA;
        (pGIpParamTmp+i)->Data  = &(pParamHdlr[paramHandlerNo]->IqphSceneData[SCENE_TABLE_CONTAIN_SETS*j]);
        (pGIpParamTmp+i)->Length = sizeof(SCENE_DATA_s)*SCENE_TABLE_CONTAIN_SETS;
        i++;
    }
    for(j=0;j<DE_VIDEO_TABLE_MAX_NO;j++){
        strcpy((pGIpParamTmp+i)->Name,"de_default_video");
        (pGIpParamTmp+i)->Type = DE_PARAM_VIDEO;
        (pGIpParamTmp+i)->Data  = &(pParamHdlr[paramHandlerNo]->IqphDeVideo);
        (pGIpParamTmp+i)->Length = sizeof(DE_PARAM_s);
        i++;
    }
    for(j=0;j<DE_STILL_TABLE_MAX_NO;j++){
        strcpy((pGIpParamTmp+i)->Name,"de_default_still");
        (pGIpParamTmp+i)->Type = DE_PARAM_STILL;
        (pGIpParamTmp+i)->Data  = &(pParamHdlr[paramHandlerNo]->IqphDeStill);
        (pGIpParamTmp+i)->Length = sizeof(DE_PARAM_s);
        i++;
    }
    for(j=0;j<ADJ_TABLE_MAX_NO;j++){
        strcpy((pGIpParamTmp+i)->Name,"adj_table_param_default");
        (pGIpParamTmp+i)->Type = ADJ_TABLE_PARAM;
        (pGIpParamTmp+i)->Data  = &(pParamHdlr[paramHandlerNo]->IqphAdjTable);
        (pGIpParamTmp+i)->Length = sizeof(ADJ_TABLE_PARAM_s);
        i++;
    }
    for(j=0;j<ADJ_VIDEO_HISO_TABLE_MAX_NO;j++){
        sprintf(strtemp,"%02d",j);
        (pGIpParamTmp+i)->Type = ADJ_PARAM_HISO_VIDEO;
        strcpy((pGIpParamTmp+i)->Name,"adj_hiso_video_default_");
        strcat((pGIpParamTmp+i)->Name,strtemp);
        (pGIpParamTmp+i)->Data  = &(pParamHdlr[paramHandlerNo]->IqphAdjHisoVideo[j]);
        (pGIpParamTmp+i)->Length = sizeof(ADJ_VIDEO_HISO_PARAM_s);
        i++;
    }
    (pGIpParamTmp+i)->Length = -1;
}

/**
*  @brief  Allocate parameter memory
*
*  @param [in] chNo chNo
*
*  @return int : 0(OK)/-1(NG)
*/
int AppLibIQParam_Allocate_Param_Mem(UINT32 chNo)
{
    int RVal = 0;
    extern AMBA_KAL_BYTE_POOL_t *APPLIB_G_MMPL;        /**< Cache memory pool for applib*/
    RVal = AmbaKAL_BytePoolAllocate(APPLIB_G_MMPL, (void **)&pParamHdlr[chNo], sizeof(PARAM_HANDLER_s), 100);
    if(RVal == 0){
        memset(pParamHdlr[chNo], 0, sizeof(PARAM_HANDLER_s));
        pParamHdlr[chNo]->IqphSceneModeCc3d = SYSTEM_DEFAULT;
        pParamHdlr[chNo]->IqphDeCc3d = SYSTEM_DEFAULT;
        pParamHdlr[chNo]->IqphCcRegID = "A12_CC_Reg.bin";
    }

    return (RVal);
}

/**
*  @brief  Initialize parameter memory
*
*  @param [in] paramHandlerNo
*
*  @return int : 0(OK)/-1(NG)
*/
int AppLibIQParam_Init_Param_Mem(UINT32 paramHandlerNo)
{
    UINT16 i,j,k;
    int RVal = 0;

    _Init_GIpParam_Flag_Test(GIpParamInfo);
    _Init_GIpParam_Flag_Test(&GIpParamInfo[1]);

    pGIpParam[0] = GIpParam_tmp_1;
    pGIpParam[1] = GIpParam_tmp_2;

    // Init GIpP aram
    _Assgin_Data_Addr(paramHandlerNo,pGIpParam[paramHandlerNo]);

    i=0;
    for(j=0;j<GIpParamCategoryNum;j++) {
        for(k=0;k<GIpParamInfo[paramHandlerNo].GIpParamCategoryMaxNum[j];k++) {
            if (k<GIpParamInfo[paramHandlerNo].GIpParamValidTableFlag[j]) {
                _Validate_Param(paramHandlerNo, &pGIpParam[paramHandlerNo][i],k);
            }
            i++;
        }
    }
    return (RVal);

}

/**
*  @brief  Initialize parameter proxy
*
*  @param [in] modeSwitch mode switch
*
*  @return >=0 success, <0 failure
*/
int AppLibIQParam_Init_Param_Proxy(UINT32 chNo)
{
    AppLibIQParam_Init_Param_Mem(chNo);
    _Init_Color_Table_Addr(chNo);
    _Load_Cc_Reg_Table(chNo);
    _Load_Video_Color_Table(chNo, 0, 0);// Test
    _Init_Color_Scene_Table(chNo);
    _Init_Color_De_Video_Table(chNo);

    return 0;
}

int AppLibIQParam_Free_Param_Mem(PARAM_HANDLER_s *pParamHdlr)
{
    int RVal = 0;

    RVal = AmbaKAL_BytePoolFree(pParamHdlr);
    return (RVal);
}