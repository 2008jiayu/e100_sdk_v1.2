/**
 * @file src/app/connected/applib/inc/3a/iqparam/ApplibIQParamHandler.h
 *
 * Implementation of IQ param handler.
 *
 * History:
 *    2013/09/23 - [Martin Lai] created file
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

#ifndef APPLIB_IQPARAM_HANDLER_H_
#define APPLIB_IQPARAM_HANDLER_H_

#include <imgproc/AmbaImg_AaaDef.h>


#define IMG_MODE_TV           (0)     /**<IMG_MODE_TV (0)*/
#define IMG_MODE_PC           (1)     /**<IMG_MODE_PC (1)*/

#define LOAD_IQ_C_OR_ROM         (1)  /**< form C file :1; form ROM file 0*/

#define ADJ_VIDEO_TABLE_MAX_NO        (2)     /**<ADJ_VIDEO_TABLE_MAX_NO        (2)*/
#define ADJ_VIDEO_HISO_TABLE_MAX_NO   (1)
#define ADJ_PHOTO_TABLE_MAX_NO        (2)     /**<ADJ_PHOTO_TABLE_MAX_NO        (2)*/
#define ADJ_STILL_LISO_TABLE_MAX_NO   (2)     /**<ADJ_STILL_LISO_TABLE_MAX_NO   (2)*/
#define ADJ_STILL_HISO_TABLE_MAX_NO   (1)     /**<ADJ_STILL_HISO_TABLE_MAX_NO   (1)*/
#define SCENE_TABLE_MAX_NO            (5)     /**<SCENE_TABLE_MAX_NO            (5)*/
#define SCENE_TABLE_CONTAIN_SETS      (8)     /**<SCENE_TABLE_CONTAIN_SETS      (8)*/
#define SCENE_TABLE_TATOL_NO               (36)
#define IMG_TABLE_MAX_NO                   (1)
#define AAA_TABLE_MAX_NO                   (2)
#define ADJ_TABLE_MAX_NO                   (1)
#define ADJ_STILL_IDX_INFO_TABLE_MAX_NO    (1)
#define ADJ_VIDEO_IDX_INFO_TABLE_MAX_NO    (1)
#define DE_VIDEO_TABLE_MAX_NO              (1)
#define DE_STILL_TABLE_MAX_NO              (1)


#define ADJ_VIDEO_TABLE_VALID_NO        (2)    /**<ADJ_VIDEO_TABLE_VALID_NO        (2)*/
#define ADJ_VIDEO_HISO_TABLE_VALID_NO   (1)
#define ADJ_PHOTO_TABLE_VALID_NO        (2)    /**<ADJ_PHOTO_TABLE_VALID_NO        (2)*/
#define ADJ_STILL_LISO_TABLE_VALID_NO   (2)    /**<ADJ_STILL_LISO_TABLE_VALID_NO   (2)*/
#define ADJ_STILL_HISO_TABLE_VALID_NO   (1)    /**<ADJ_STILL_HISO_TABLE_VALID_NO   (1)*/
#define SCENE_TABLE_VALID_NO            (5)    /**<SCENE_TABLE_VALID_NO            (5)*/
#define IMG_TABLE_VALID_NO                 (1)
#define AAA_TABLE_VALID_NO                 (2)
#define ADJ_TABLE_VALID_NO                 (1)
#define ADJ_STILL_IDX_INFO_TABLE_VALID_NO  (1)
#define ADJ_VIDEO_IDX_INFO_TABLE_VALID_NO  (1)
#define DE_VIDEO_TABLE_VALID_NO            (1)
#define DE_STILL_TABLE_VALID_NO            (1)

#define IMG_PARAM                 (0)       /**<IMG_PARAM                 (0)*/
#define AAA_PARAM                 (1)       /**<AAA_PARAM                 (1)*/
#define ADJ_PARAM_VIDEO           (2)       /**<ADJ_PARAM_VIDEO           (2)*/
#define ADJ_PARAM_PHOTO           (3)       /**<ADJ_PARAM_PHOTO           (3)*/
#define ADJ_PARAM_LISO_STILL      (4)       /**<ADJ_PARAM_LISO_STILL      (4)*/
#define ADJ_PARAM_HISO_STILL      (5)       /**<ADJ_PARAM_HISO_STILL      (5)*/
#define ADJ_TABLE_PARAM           (6)       /**<ADJ_TABLE_PARAM           (6)*/
#define ADJ_PARAM_HISO_VIDEO      (7)

#define ADJ_PARAM_STILL_IDX       (10)      /**<ADJ_PARAM_STILL_IDX       (10) */
#define ADJ_PARAM_VIDEO_IDX       (11)      /**<ADJ_PARAM_VIDEO_IDX       (10) */

#define SCENE_DATA                (12)      /**<SCENE_DATA                (11)*/

#define DE_PARAM_VIDEO            (16)      /**<DE_PARAM_VIDEO            (16) */
#define DE_PARAM_STILL            (17)      /**<DE_PARAM_STILL            (17) */
#define ADJ_CALIB_PARAM            (18)     /**<ADJ_CALIB_PARAM           (18)*/

#define PARAM_UNLOAD        (0)             /**<PARAM_UNLOAD        (0) */
#define PARAM_LOADED        (1)             /**<PARAM_LOADED        (1) */

#define DEF_CC_TABLE_NO         (5)         /**<DEF_CC_TABLE_NO         (5)*/

#define GIpParamCategoryNum               (13)     /**<GIpParamCategoryNum               (11)*/
#define GIpParamCategory_IMG              (0)      /**<GIpParamCategory_IMG              (0) */
#define GIpParamCategory_AAA              (1)      /**<GIpParamCategory_AAA              (1) */
#define GIpParamCategory_ADJ_STILL        (2)      /**<GIpParamCategory_ADJ_STILL        (2) */
#define GIpParamCategory_ADJ_HISO_STILL   (3)      /**<GIpParamCategory_ADJ_HISO_STILL   (3) */
#define GIpParamCategory_ADJ_VIDEO        (4)      /**<GIpParamCategory_ADJ_VIDEO        (4) */
#define GIpParamCategory_ADJ_PHOTO        (5)      /**<GIpParamCategory_ADJ_PHOTO        (5) */
#define GIpParamCategory_ADJ_STILL_IDX    (6)      /**<GIpParamCategory_ADJ_STILL_IDX    (6) */
#define GIpParamCategory_ADJ_VIDEO_IDX    (7)      /**<GIpParamCategory_ADJ_VIDEO_IDX    (6) */
#define GIpParamCategory_SCENE            (8)      /**<GIpParamCategory_SCENE            (7) */
#define GIpParamCategory_DE_VIDEO         (9)      /**<GIpParamCategory_DE_VIDEO         (8) */
#define GIpParamCategory_DE_STILL         (10)      /**<GIpParamCategory_DE_STILL         (9) */
#define GIpParamCategory_ADJ_TABLE        (11)     /**<GIpParamCategory_ADJ_TABLE        (10)*/
#define GIpParamCategory_ADJ_HISO_VIDEO   (12)
#define GIpParamCategory_CALIB            (12)     /**<GIpParamCategory_CALIB            (11)*/

//0: img_default
//1: aaa_default
//2: adj_still_default
//3: adj_hiso_still_default
//4: adj_video_default XXXX
//5: adj_photo_default
//6: adj_still_idx
//7: scene_data_s XXXX
//8: de_default_video
//9: de_default_still
//10:adj_table_param_default
//11:adj_calib_param

/**
 * parameter table path
 */
typedef struct _PARAM_TABLE_PATH_s_ {
    const char *ImageParam;       /**<Image Parameter*/
    const char *AaaParam;         /**<aaa Parameter*/
    const char *AdjPhotoParam;    /**<Adj Photo Parameter*/
    const char *AdjVideoParam;    /**<Adj Video Parameter*/
    const char *AdjStillLowIsoParam; /**<Adj Still Low Iso Paramter*/
} PARAM_TABLE_PATH_s;
/**
 * color table path
 */
typedef struct _COLOR_TABLE_PATH_s_ {
    const int     ColorIdx;      /**<color index*/
    const char    *Video[5];     /**<video*/
    const char    *Still[5];     /**<Still*/
} COLOR_TABLE_PATH_s;
/**
 * color table path
 */
typedef struct _IP_TABLE_PATH_s_ {
    const char *IpTablePath; /**<Ip Table Path*/
} IP_TABLE_PATH_s;
/**
 * parameter proxy
 */
typedef struct _PARAM_PROXY_s_ {
    UINT32 Id; /**<sensor ID*/
    char Name[80]; /**<SEnsor Name*/
    int (*Init)(void);     /**< Module init interface */
    /** Module parameter get interface */
    IMG_PARAM_s* (*GetImgParam)(int paramLoaded); /**<get module image parameter*/
    AAA_PARAM_s* (*GetAaaParam)(void); /**<get module aaa parameter*/
    ADJ_VIDEO_PARAM_s* (*GetAdjVideoParam)(int paramLoaded, int tableNo); /**<get module Adj Video parameter*/
    ADJ_VIDEO_HISO_PARAM_s* (*GetAdjVideoHIsoParam)(int paramLoaded, int tableNo);
    ADJ_PHOTO_PARAM_s* (*GetAdjPhotoParam)(int paramLoaded, int tableNo); /**<get module Adj Photo parameter*/
    ADJ_STILL_LOW_ISO_PARAM_s* (*GetAdjStillLIsoParam)(int paramLoaded, int tableNo); /**<get module Adj Still Low Iso parameter*/
    ADJ_STILL_HISO_PARAM_s* (*GetAdjStillHIsoParam)(int paramLoaded, int tableNo); /**<get module Adj Still High Iso parameter*/
    ADJ_STILL_IDX_INFO_s* (*GetAdjStillIdxInfoParam)(int paramLoaded); /**<get module Adj Still Index Info parameter*/
    ADJ_VIDEO_IDX_INFO_s* (*GetAdjVideoIdxInfoParam)(int paramLoaded);
    SCENE_DATA_s* (*GetSceneData)(int sceneLoaded, int sceneSet); /**<get module Scene Data parameter*/
    DE_PARAM_s* (*GetDeParam)(int mode, int paramLoaded); /**<get module DeParam */
//    CALIBRATION_PARAM_s* (*GetCalibParam)(void);
    ADJ_TABLE_PARAM_s* (*GetAdjTableParam)(void); /**<get module Adj Table parameter*/

    /** Public API */
    COLOR_TABLE_PATH_s* (*GetColorTablePath)(void); /**<Get Color Table Path*/
    PARAM_TABLE_PATH_s* (*GetParamTablePath)(void); /**<Get Color Table Path*/
    IP_TABLE_PATH_s*    (*GetAdjVideoTablePath)(int paramLoaded, int tableNo); /**<Get Adj Video Table Path*/
    IP_TABLE_PATH_s*    (*GetAdjVideoHIsoTablePath)(int paramLoaded, int tableNo);
    IP_TABLE_PATH_s*    (*GetAdjPhotoTablePath)(int paramLoaded, int tableNo); /**<Get Adj Photo Table Path*/
    IP_TABLE_PATH_s*    (*GetAdjStillLIsoTablePath)(int paramLoaded, int tableNo); /**<Get Adj Still Low Iso Table Path*/
    IP_TABLE_PATH_s*    (*GetAdjStillHIsoTablePath)(int paramLoaded, int tableNo); /**<Get Still High Iso Table Path*/
    IP_TABLE_PATH_s*    (*GetSceneDataTablePath)(int paramLoaded, int sceneSet); /**<Get Scene Data Table Path*/
    IP_TABLE_PATH_s*    (*GetImgTablePath)(void);
    IP_TABLE_PATH_s*    (*GetAaaTablePath)(int tableNo);
    IP_TABLE_PATH_s*    (*GetAdjStillIdxInfoTablePath)(void);
    IP_TABLE_PATH_s*    (*GetAdjVideoIdxInfoTablePath)(void);
    IP_TABLE_PATH_s*    (*GetDeTablePath)(int mode);
    IP_TABLE_PATH_s*    (*GetAdjTablePath)(void);

} PARAM_PROXY_s;
/**
 * IP parameter
 */
typedef struct _IP_PARAM_s_ {
    char   Name[32];    /**< name */
    int     Type;     /**< type*/
    void    *Data;    /**< data*/
    int     Length;   /**< length*/
} IP_PARAM_s;

/**
 * test IP table address
 */
typedef struct _IP_TABLE_ADDR_s_ {
    void    *Data; /**< data*/
} IP_TABLE_ADDR_s;


typedef struct _GIpParam_Info_s_{
UINT16 GIpParamCategoryMaxNum[GIpParamCategoryNum];
UINT16 GIpParamValidTableFlag[GIpParamCategoryNum];
}GIpParam_Info_s;

#define IP_PARAM_MAX_NUM                (30)
typedef struct _PARAM_HANDLER_s_ {
    PARAM_PROXY_s IqphParamProxy;
    int GVideoLoaded;
    int GPhotoLoaded;
    int IqphCcSceneTable[56];
    int IqphCcDeVideoTable[DIGITAL_LAST];
    UINT16 IqphSceneModeCc3d;
    UINT16 IqphDeCc3d;
    /* ADJ parameters index table */
    UINT8 IqphVideoCcAddr[DEF_CC_TABLE_NO][CC_3D_SIZE];
    UINT8 IqphStillCcAddr[DEF_CC_TABLE_NO][CC_3D_SIZE];
    char *IqphCcRegID;
    UINT8 IqphCcReg[CC_REG_SIZE];
    ADJ_VIDEO_PARAM_s IqphAdjVideo[ADJ_VIDEO_TABLE_MAX_NO];
    ADJ_VIDEO_HISO_PARAM_s IqphAdjHisoVideo[ADJ_VIDEO_HISO_TABLE_MAX_NO];
    ADJ_PHOTO_PARAM_s IqphAdjPhoto[ADJ_PHOTO_TABLE_MAX_NO];
    ADJ_STILL_LOW_ISO_PARAM_s IqphAdjLisoStill[ADJ_STILL_LISO_TABLE_MAX_NO];
    ADJ_STILL_HISO_PARAM_s IqphAdjHisoStill[ADJ_STILL_HISO_TABLE_MAX_NO];
    SCENE_DATA_s IqphSceneData[SCENE_TABLE_CONTAIN_SETS*SCENE_TABLE_MAX_NO];
    UINT16 GIpParamCategoryMaxNum[GIpParamCategoryNum];// = {0x01,0x01,0x02,0x01,0x02,0x02,0x01,0x05,0x01,0x01};
    UINT16 GIpParamValidTableFlag[GIpParamCategoryNum];// = {0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x05,0x01,0x01};
    IP_PARAM_s GIpParam[IP_PARAM_MAX_NUM];
    IMG_PARAM_s IqphImg;
    AAA_PARAM_s IqphAaaDefault[AAA_TABLE_MAX_NO];
    ADJ_STILL_IDX_INFO_s IqphAdjStillIdx;
    ADJ_VIDEO_IDX_INFO_s IqphAdjVideoIdx;
    DE_PARAM_s IqphDeVideo;
    DE_PARAM_s IqphDeStill;
    ADJ_TABLE_PARAM_s IqphAdjTable;

} PARAM_HANDLER_s;
/*************************************************************************
 * IQ Param Handler Public APIs
 ************************************************************************/

/**
 *  Initialize parameter proxy
 *
 *  @param [in] modeSwitch mode switch
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibIQParam_Init_Param_Proxy(UINT32 chNo);

/**
 *  Remove the Param Proxy instance.
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibIQParam_Remove(UINT32 chNo);

/**
 *  Attach the Sensor input device and enable the device control.
 *
 *  @param [in] par Pointer to param proxy
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibIQParam_Attach(UINT32 chNo, PARAM_PROXY_s *par);

/**
 *  Attach the Sensor input device and enable the device control.
 *
 *  @param [in] name Pointer to param name
 *  @param [in] type Param type
 *  @param [out] param Pointer to IP param
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibIQParam_Get_By_Name(UINT32 chNo, const char *name, const int type, IP_PARAM_s **param);

/**
 *  Init IQ parameters.
 *
 *  @param [in] ChCount IQ channel count
 *
 *  @return =0 success, else failure
 */
extern INT32 AppLibIQ_ParamInit(UINT32 ChCount);

/**
*  @brief  Allocate parameter memory
*
*  @param [in] chNo chNo
*
*  @return int : 0(OK)/-1(NG)
*/
extern int AppLibIQParam_Allocate_Param_Mem(UINT32 chNo);

#endif /* APPLIB_IQPARAM_HANDLER_H_ */
