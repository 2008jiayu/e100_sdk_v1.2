/**
 * @file src/app/connected/applib/src/image/ApplibImage.h
 *
 * Implementation of Image Utility interface.
 *
 * History:
 *    2013/09/03 - [Martin Lai] created file
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
 * affiliates.  In the absence of such an agreement, you agree to promptly notify and
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

#include <applib.h>
#include <imgproc/AmbaImg_VDsp_Handler.h>
#include <imgproc/AmbaImg_VIn_Handler.h>
#include <imgschdlr/scheduler.h>
#include <imgproc/AmbaImg_Adjustment_Def.h>
#include <imgproc/AmbaImg_Proc.h>
#include <imgproc/AmbaImg_Impl_Cmd.h>
#include <imgproc/AmbaImg_AaaDef.h>
#include "../AppLibTask_Priority.h"

//#define DEBUG_APPLIB_IMAGE
#if defined(DEBUG_APPLIB_IMAGE)
#define DBGMSG AmbaPrint
#define DBGMSGc(x) AmbaPrintColor(GREEN,x)
#define DBGMSGc2 AmbaPrintColor
#else
#define DBGMSG(...)
#define DBGMSGc(...)
#define DBGMSGc2(...)
#endif

/*************************************************************************
 * MW Image definitions
 ************************************************************************/

static UINT8 *MemISBuf;
static void *MemISBufRaw;
#if defined(MULTI_CHANNEL_VIN)
#define ENCODE_FOV_NUM           4
#define VOUT_FOV_NUM             5
#define TOTAL_FOV_NUM           (ENCODE_FOV_NUM + VOUT_FOV_NUM)
#define MAX_IMAGE_SCHDLR_NUM    (TOTAL_FOV_NUM)
#else
#define MAX_IMAGE_SCHDLR_NUM    (1)
#endif
// Image scheduler
AMBA_IMG_SCHDLR_HDLR_s *VencImgSchdlr[MAX_IMAGE_SCHDLR_NUM] = {NULL};
static int AppLibImageEnableFlag = 1;
static int AppLibImageInitFlag = -1;

static int AppLibImageAeAwbAdjEnableFlag = 0;

/*************************************************************************
 * MW Image APIs (Static)
 ************************************************************************/

/**
 *  @brief Vin changed prior callback function.
 *
 *  Vin changed prior callback function.
 *
 *  @param [in] hdlr Handler
 *  @param [in] event Event
 *  @param [in] info Information
 *
 *  @return >=0 success, <0 failure
 */
int AppLibImage_VinChangedPriorCallbackFunc(void *hdlr, UINT32 event, void *info)
{
    int ReturnValue = -1;
    if (AppLibImageEnableFlag == 0)
        return ReturnValue;

    if (AppLibImageAeAwbAdjEnableFlag) {
        ReturnValue = Amba_Img_VIn_Changed_Prior(hdlr, (UINT32 *)NULL);//Need to refine the flow about 3a.
        if (ReturnValue < 0) {
            AmbaPrintColor(RED,"[AppLib - Image] <Amba_Img_VIn_Changed_Prior> Create failure!!");
        } else {
            DBGMSGc2(GREEN,"[AppLib - Image] <Amba_Img_VIn_Changed_Prior>");
        }
    }
    return ReturnValue;
}

/**
 *  @brief Vin changed post callback function.
 *
 *  Vin changed post callback function.
 *
 *  @param [in] hdlr Handler
 *  @param [in] event Event
 *  @param [in] info Information
 *
 *  @return >=0 success, <0 failure
 */
int AppLibImage_VinChangedPostCallbackFunc(void *hdlr, UINT32 event, void *info)
{
    int ReturnValue = -1;
    if (AppLibImageEnableFlag == 0)
        return ReturnValue;

    if (AppLibImageAeAwbAdjEnableFlag) {
        ReturnValue = Amba_Img_VIn_Changed_Post(hdlr, (UINT32 *)NULL);//Need to refine the flow about 3a.
        if (ReturnValue < 0) {
            AmbaPrintColor(RED,"[AppLib - Image] <Amba_Img_VIn_Changed_Post> Create failure!!");
        } else {
            DBGMSGc2(GREEN,"[AppLib - Image] <Amba_Img_VIn_Changed_Post>");
        }
    }

    return ReturnValue;
}

/**
 *  @brief Vin invalid callback function.
 *
 *  Vin invalid callback function.
 *
 *  @param [in] hdlr Handler
 *  @param [in] event Event
 *  @param [in] info Information
 *
 *  @return >=0 success, <0 failure
 */
int AppLibImage_VinInvalidCallbackFunc(void *hdlr, UINT32 event, void *info)
{
    int ReturnValue = -1;
    int i = 0;
    if (AppLibImageEnableFlag == 0)
        return ReturnValue;

    for (i = 0;i < MAX_IMAGE_SCHDLR_NUM;i++) {
        AppLibImage_EnableImgSchdlr(i , 0);
    }

    if (AppLibImageAeAwbAdjEnableFlag) {
        ReturnValue = Amba_Img_VIn_Invalid(hdlr, (UINT32 *)NULL);//Need to refine the flow about 3a.
        if (ReturnValue < 0) {
            AmbaPrintColor(RED,"[AppLib - Image] <VinInvalidCallbackFunc> Amba_Img_VIn_Valid failure!!");
        } else {
            DBGMSGc2(GREEN,"[AppLib - Image] <VinInvalidCallbackFunc> Amba_Img_VIn_Valid");
        }
    }
    return ReturnValue;
}

/**
 *  @brief Vin valid callback function.
 *
 *  Vin valid callback function.
 *
 *  @param [in] hdlr Handler
 *  @param [in] event Event
 *  @param [in] info Information
 *
 *  @return >=0 success, <0 failure
 */
int AppLibImage_VinValidCallbackFunc(void *hdlr, UINT32 event, void *info)
{
    int ReturnValue = -1;
    int i = 0;

    if (AppLibImageEnableFlag == 0)
        return ReturnValue;

    if (AppLibImageAeAwbAdjEnableFlag) {
        ReturnValue = Amba_Img_VIn_Valid(hdlr, (UINT32 *)NULL);//Need to refine the flow about 3a.
        if (ReturnValue < 0) {
            AmbaPrintColor(RED,"[AppLib - Image] <Amba_Img_VIn_Valid> Create failure!!");
        } else {
            DBGMSGc2(GREEN,"[AppLib - Image] <Amba_Img_VIn_Valid)>");
        }
    }

    for (i = 0;i < MAX_IMAGE_SCHDLR_NUM;i++) {
        AppLibImage_EnableImgSchdlr(i , 1);
    }
    return ReturnValue;
}

/**
 *  @brief Image CFA handler
 *
 *  Image CFA handler
 *
 *  @param [in] hdlr Handler
 *  @param [in] event Event
 *  @param [in] info Information
 *
 *  @return >=0 success, <0 failure
 */
int AppLibImage_CfaHandler(void *hdlr, UINT32 event, void *info)
{
    int ReturnValue = -1;
    if (AppLibImageEnableFlag == 0)
        return ReturnValue;

    if (AppLibImageAeAwbAdjEnableFlag) {
        ReturnValue = Amba_Img_VDspCfa_Handler(hdlr, (UINT32 *)info);//Need to refine the flow about 3a.
        if (ReturnValue < 0) {
            AmbaPrintColor(RED,"[AppLib - Image] <Amba_Img_VDspCfa_Handler> Create failure!!");
        } else {
            DBGMSGc2(GREEN,"[AppLib - Image] <Amba_Img_VDspCfa_Handler)>");
        }
    }

    return ReturnValue;
}

/**
 *  @brief Image RGB handler
 *
 *  Image RGB handler
 *
 *  @param [in] hdlr Handler
 *  @param [in] event Event
 *  @param [in] info Information
 *
 *  @return >=0 success, <0 failure
 */
int AppLibImage_VDspRgbHandler(void *hdlr, UINT32 event, void *info)
{
    int ReturnValue = -1;
    if (AppLibImageEnableFlag == 0)
        return ReturnValue;

    if (AppLibImageAeAwbAdjEnableFlag) {
        ReturnValue = Amba_Img_VDspRgb_Handler(hdlr, (UINT32 *)info);//Need to refine the flow about 3a.
        if (ReturnValue < 0) {
            AmbaPrintColor(RED,"[AppLib - Image] <Amba_Img_VDspRgb_Handler> Create failure!!");
        } else {
            DBGMSGc2(GREEN,"[AppLib - Image] <Amba_Img_VDspRgb_Handler)>");
        }
    }

    return ReturnValue;
}

/**
 *  @brief Applib image module initialization
 *
 *  Applib image module initialization
 *
 *  @return >=0 success, <0 failure
 */
int AppLibImage_Init(void)
{
    AMBA_IMG_SCHDLR_INIT_CFG_s ISInitCfg;
    UINT32 ISPoolSize = 0;
    int Er;
    if (AppLibImageInitFlag == 0) {
        return 0;
    }
    Er = AmbaImgSchdlr_QueryMemsize( MAX_IMAGE_SCHDLR_NUM, &ISPoolSize);
    if (Er != OK) {
        AmbaPrintColor(RED, "[AppLib - Image] <Init> Out of memory for imgschdlr!!");
        return -1;
    }
    AmbaImgSchdlr_GetInitDefaultCfg(&ISInitCfg);
    ISPoolSize += ISInitCfg.MsgTaskStackSize;

    Er = AmpUtil_GetAlignedPool(APPLIB_G_MMPL, (void **)&MemISBuf, &MemISBufRaw, ISPoolSize, 32);
    if (Er != OK) {
        AmbaPrintColor(RED, "[AppLib - Image] <Init> Out of memory for imgschdlr!!");
        return -1;
    }
    MemISBuf = (UINT8 *)ALIGN_32((UINT32)MemISBuf);

    ISInitCfg.MemoryPoolAddr = MemISBuf;
    ISInitCfg.MemoryPoolSize = ISPoolSize;
    ISInitCfg.MainViewNum = MAX_IMAGE_SCHDLR_NUM;
    ISInitCfg.IsoCfgNum = AmpResource_GetIKIsoConfigNumber(AMBA_DSP_IMG_PIPE_VIDEO);//The image scheduler is for video.
    ISInitCfg.MsgTaskPriority = APPLIB_IMAGE_SCHDLR_MSG_TASK_PRIORITY;
    Er = AmbaImgSchdlr_Init(&ISInitCfg);
    if (Er == AMP_OK) {
        AppLibImageInitFlag = 0;
    } else {
        AmbaPrintColor(RED, "[AppLib - Image] <Init> AmbaImgSchdlr_Init Fail!");
    }
    return Er;
}


/**
 *  @brief Create image scheduler
 *
 *  Create image scheduler
 *
 *  @param [in] param Scheduler parameter
 *
 *  @return >=0 success, <0 failure
 */
int AppLibImage_CreateImgSchdlr(AMBA_IMG_SCHDLR_CFG_s *param, UINT32 index)
{
    int ReturnValue = 0;
    AMBA_IMG_SCHDLR_CFG_s ISCfg = {0};
    AMBA_SENSOR_MODE_INFO_s SensorModeInfo;
    AMBA_SENSOR_STATUS_INFO_s SensorStatus = {0};
    if (AppLibImageEnableFlag == 0) {
        return -1;
    }

    if (index >= MAX_IMAGE_SCHDLR_NUM) {
        AmbaPrintColor(RED,"[AppLib - Image] <CreateImgSchdlr> The index %d is invalid", index);
        return -1;
    }
    DBGMSGc2(BLUE,"[AppLib - Image] <CreateImgSchdlr> start!");
    AmbaSensor_GetStatus(AppEncChannel, &SensorStatus);
    AmbaSensor_GetModeInfo(AppEncChannel, SensorStatus.ModeInfo.Mode, &SensorModeInfo);
    if (VencImgSchdlr[index] == NULL) {
        // Create ImgSchdlr instance
        AmbaImgSchdlr_GetDefaultCfg(&ISCfg);

        ISCfg.MainViewID = param->MainViewID; //single channle have one MainView
        ISCfg.Channel = param->Channel;
        ISCfg.Vin = param->Vin;
        ISCfg.cbEvent = param->cbEvent;
        ISCfg.TaskPriority = APPLIB_IMAGE_SCHDLR_TASK_PRIORITY;
        ISCfg.VideoProcMode = param->VideoProcMode;
        ReturnValue = AmbaImgSchdlr_Create(&ISCfg, &VencImgSchdlr[index]);  // One sensor (not vin) need one scheduler.
        if (ReturnValue != OK) {
            AmbaPrintColor(RED,"[AppLib - Image] <CreateImgSchdlr> Create VencImgSchdlr[%d] failure!!", index);
        }
    } else {
        AmbaPrintColor(RED,"[AppLib - Image] <CreateImgSchdlr> VencImgSchdlr[%d] Exist!!", index);
    }

    DBGMSGc2(BLUE,"[AppLib - Image] <CreateImgSchdlr> End!");
    return ReturnValue;
}

int AppLibImage_UpdateImgSchdlr(AMBA_IMG_SCHDLR_UPDATE_CFG_s *param, UINT32 index)
{
    int ReturnValue = 0;
    AMBA_IMG_SCHDLR_UPDATE_CFG_s ISCfg = {0};

    if (VencImgSchdlr[index]) {
        AmbaImgSchdlr_GetConfig(VencImgSchdlr[index], &ISCfg);

        ISCfg.VideoProcMode = param->VideoProcMode;
        ISCfg.AAAStatSampleRate = param->AAAStatSampleRate;
        ReturnValue = AmbaImgSchdlr_SetConfig(VencImgSchdlr[index], &ISCfg);//Need to refine the flow about 3a.
        if (ReturnValue < 0) {
            AmbaPrintColor(RED,"[AppLib - Image] <UpdateImgSchdlr> AmbaImgSchdlr_SetConfig failure!!");
        }
    }
}

int AppLibImage_EnableImgSchdlr(UINT32 index, UINT32 enable)
{
    int ReturnValue = 0;
    if (VencImgSchdlr[index]) {
        ReturnValue = AmbaImgSchdlr_Enable(VencImgSchdlr[index], enable);//Need to refine the flow about 3a.
        if (ReturnValue < 0) {
            AmbaPrintColor(RED,"[AppLib - Image] <EnableImgSchdlr> AmbaImgSchdlr_Enable failure!!");
        } else {
            DBGMSGc2(MAGENTA,"[AppLib - Image] <EnableImgSchdlr> AmbaImgSchdlr_Enable(VencImgSchdlr[%d], 1)", index);
        }
    } else {
        AmbaPrintColor(RED,"[AppLib - Image] <VinInvalidCallbackFunc> VencImgSchdlr[%d] is NULL!!", index);
    }
    return ReturnValue;
}


/**
 *  @brief Delete image scheduler
 *
 *  Delete image scheduler
 *
 *  @return >=0 success, <0 failure
 */
int AppLibImage_DeleteImgSchdlr(UINT32 index)
{
    int ReturnValue = 0;
    if (AppLibImageEnableFlag == 0) {
        return -1;
    }

    if (index >= MAX_IMAGE_SCHDLR_NUM) {
        AmbaPrintColor(RED,"[AppLib - Image] <DeleteImgSchdlr> The index %d is invalid", index);
        return -1;
    }
    DBGMSGc2(BLUE,"[AppLib - Image] <DeleteImgSchdlr> start!");
    if (VencImgSchdlr[index] != NULL) {
        ReturnValue = AmbaImgSchdlr_Delete(VencImgSchdlr[index]);
        if (ReturnValue == OK) {
            VencImgSchdlr[index] = NULL;
        } else {
            AmbaPrintColor(RED,"[AppLib - Image] <AmbaImgSchdlr_Delete> failure!!");
        }
    } else {
        AmbaPrintColor(RED,"[AppLib - Image] <AmbaImgSchdlr_Delete> schdlr doesn't exist!!");
    }
    DBGMSGc2(BLUE,"[AppLib - Image] <DeleteImgSchdlr> End!");
    return ReturnValue;
}

/**
 *  @brief Stop image scheduler
 *
 *  Stop image scheduler
 *
 *  @return >=0 success, <0 failure
 */
int AppLibImage_StopImgSchdlr(UINT32 index)
{
    int ReturnValue = 0;

    if (AppLibImageEnableFlag == 0) {
        return -1;
    }
    if (index >= MAX_IMAGE_SCHDLR_NUM) {
        AmbaPrintColor(RED,"[AppLib - Image] <StopImgSchdlr> The index %d is invalid", index);
        return -1;
    }
    if (VencImgSchdlr[index] != NULL) {
        ReturnValue = AmbaImgSchdlr_Enable(VencImgSchdlr[index], 0);
        if (ReturnValue != OK) {
            AmbaPrintColor(RED,"[AppLib - Image] <AmbaImgSchdlr_Enable> failure!!");
        }
    } else {
        AmbaPrintColor(RED,"[AppLib - Image] <StopImgSchdlr> VencImgSchdlr[%d] is NULL!!", index);
    }
    return ReturnValue;
}

/**
 *  @brief Stop the 3a image.
 *
 *  Stop the 3a image.
 *
 *  @param [in] hdlr Handler
 *
 *  @return >=0 success, <0 failure
 */
int AppLibImage_Stop3A(void *hdlr)
{
    int ReturnValue = 0;
    if (AppLibImageEnableFlag == 0)
        return ReturnValue;

    if (AppLibImageAeAwbAdjEnableFlag) {
        ReturnValue = Amba_Img_VIn_Invalid(hdlr, (UINT32 *)NULL);
        if (ReturnValue != OK) {
            AmbaPrintColor(RED,"[AppLib - Image] <Amba_Img_VIn_Invalid> failure!!");
        }
    }
    return ReturnValue;
}

/**
 *  @brief Set the photo mode of 3A.
 *
 *  Set the photo mode of 3A.
 *
 *  @param [in] photoMode mode
 *
 *  @return >=0 success, <0 failure
 */
int AppLibImage_Set3APhotoMode(UINT32 photoMode)
{
    int ReturnValue = 0;

    if (AppLibImageEnableFlag == 0)
        return ReturnValue;

    if (AppLibImageAeAwbAdjEnableFlag) {
        //inform 3A LiveView type
        ReturnValue = AmbaImg_Proc_Cmd(MW_IP_SET_PHOTO_PREVIEW, (UINT32)&photoMode, 0, 0);
    }
    return ReturnValue;
}

/* Inform 3A to lock AE/AWB before capture */
UINT32 AppLibImage_Lock3A(void)
{
    UINT8 CurrMode = IP_PREVIEW_MODE;
    UINT8 NextMode = IP_CAPTURE_MODE;
    UINT32 ChNo = 0;
    AMBA_3A_OP_INFO_s AaaOpInfo;
    AMBA_3A_STATUS_s VideoStatus;
    AMBA_3A_STATUS_s StillStatus;


    memset(&AaaOpInfo, 0x0, sizeof(AaaOpInfo));
    memset(&VideoStatus, 0x0, sizeof(VideoStatus));
    memset(&StillStatus, 0x0, sizeof(StillStatus));

    AmbaImg_Proc_Cmd(MW_IP_GET_AAA_OP_INFO, ChNo, (UINT32)&AaaOpInfo, 0);
    AmbaImg_Proc_Cmd(MW_IP_SET_MODE, (UINT32)&CurrMode, (UINT32)&NextMode, 0);
    AmbaImg_Proc_Cmd(MW_IP_GET_3A_STATUS, ChNo, (UINT32)&VideoStatus, (UINT32)&StillStatus);

    AmbaImg_Proc_Cmd(MW_IP_SET_CAP_FORMAT, ChNo, 0x0000/*IMG_CAP_NORMAL*/, 0);


    // Wait AE lock
    if (AaaOpInfo.AeOp == ENABLE) {
        while (StillStatus.Ae != AMBA_LOCK) {
            AmbaKAL_TaskSleep(3);
            AmbaImg_Proc_Cmd(MW_IP_GET_3A_STATUS, ChNo, (UINT32)&VideoStatus, (UINT32)&StillStatus);
        }
    } else if (AaaOpInfo.AeOp == DISABLE) {
        AmbaImg_Proc_Cmd(MW_IP_GET_3A_STATUS, ChNo, (UINT32)&VideoStatus, (UINT32)&StillStatus);
        VideoStatus.Ae = AMBA_LOCK;
        StillStatus.Ae = AMBA_LOCK;
        AmbaImg_Proc_Cmd(MW_IP_SET_AE_STATUS, ChNo, (UINT32)VideoStatus.Ae, (UINT32)StillStatus.Ae);
    }

    // Wait AWB lock
    if (AaaOpInfo.AwbOp == ENABLE) {
        while (StillStatus.Awb != AMBA_LOCK) {
            AmbaKAL_TaskSleep(3);
            AmbaImg_Proc_Cmd(MW_IP_GET_3A_STATUS, ChNo, (UINT32)&VideoStatus, (UINT32)&StillStatus);
        }
    } else if (AaaOpInfo.AwbOp == DISABLE) {
        AmbaImg_Proc_Cmd(MW_IP_GET_3A_STATUS, ChNo, (UINT32)&VideoStatus, (UINT32)&StillStatus);
        VideoStatus.Awb = AMBA_LOCK;
        StillStatus.Awb = AMBA_LOCK;
        AmbaImg_Proc_Cmd(MW_IP_SET_AWB_STATUS, ChNo, (UINT32)VideoStatus.Awb, (UINT32)StillStatus.Awb);
    }

    return OK;
}

/* Inform 3A to unlock AE/AWB before b2lv */
UINT32 AppLibImage_UnLock3A(void)
{
    UINT8 CurrMode = IP_PREVIEW_MODE;
    UINT8 NextMode = IP_PREVIEW_MODE;
    UINT32 ChNo = 0;
    AMBA_3A_STATUS_s VideoStatus;
    AMBA_3A_STATUS_s StillStatus;


    AmbaImg_Proc_Cmd(MW_IP_SET_MODE, (UINT32)&CurrMode, (UINT32)&NextMode, 0);
    AmbaImg_Proc_Cmd(MW_IP_GET_3A_STATUS, ChNo, (UINT32)&VideoStatus, (UINT32)&StillStatus);

    VideoStatus.Ae = AMBA_IDLE;
    StillStatus.Ae = AMBA_IDLE;
    AmbaImg_Proc_Cmd(MW_IP_SET_AE_STATUS, ChNo, (UINT32)VideoStatus.Ae, (UINT32)StillStatus.Ae);

    VideoStatus.Awb = AMBA_IDLE;
    StillStatus.Awb = AMBA_IDLE;
    AmbaImg_Proc_Cmd(MW_IP_SET_AWB_STATUS, ChNo, (UINT32)VideoStatus.Awb, (UINT32)StillStatus.Awb);


    return OK;
}

/**
 *  @brief Set 3A enabled
 *
 *   Set 3A enabled
 *
 *  @param [in] enable Enable parameter
 *
 *  @return >=0 success, <0 failure
 */
int AppLibImage_Set3A(int enable)
{
    AppLibImageAeAwbAdjEnableFlag = enable;
    return 0;
}

/**
 *  @brief Enable the anit-flicker
 *
 *  Enable the anit-flicker
 *
 *  @param [in] enable Enable parameter
 *
 *  @return >=0 success, <0 failure
 */
#ifdef CONFIG_APP_ARD
int AppLibImage_EnableAntiFlicker(int enable,int flicker_type)
#else
int AppLibImage_EnableAntiFlicker(int enable)
#endif
{
    int ReturnValue = 0;
    AE_EV_LUT_s EvLut;
    AmbaPrintColor(GREEN, "%s(): flicker enable = %d, flicker_type = %d (0:Auto, 1:60HZ, 2:50HZ)", __FUNCTION__, enable, flicker_type);
    ReturnValue = AmbaImg_Proc_Cmd(MW_IP_GET_MULTI_AE_EV_LUT, 0, (UINT32)&EvLut,0);
    if (ReturnValue != OK) {
        AmbaPrintColor(RED,"[AppLib - Image] <EnableAntiFlicker> MW_IP_GET_MULTI_AE_EV_LUT failure!!");
    }
#ifdef CONFIG_APP_ARD
    EvLut.FlickerMode = flicker_type;
#else
    EvLut.FlickerMode = ANTI_FLICKER_AUTO;
#endif
    ReturnValue = AmbaImg_Proc_Cmd(MW_IP_SET_MULTI_AE_EV_LUT, 0, (UINT32)&EvLut,0);
    if (ReturnValue != OK) {
        AmbaPrintColor(RED,"[AppLib - Image] <EnableAntiFlicker> MW_IP_SET_MULTI_AE_EV_LUT failure!!");
    }
    ReturnValue = AmbaImg_Proc_Cmd(MW_IP_SET_FLICKER_CMD, (UINT32)enable, 0, 0);
    if (ReturnValue != OK) {
        AmbaPrintColor(RED,"[AppLib - Image] <EnableAntiFlicker> MW_IP_SET_FLICKER_CMD failure!!");
    } else {
        DBGMSGc2(GREEN,"[AppLib - Image] <EnableAntiFlicker> MW_IP_SET_FLICKER_CMD success!!");
    }

    return ReturnValue;
}

/**
 *  @brief Set the mode of the anit-flicker
 *
 *  @param [in] mode Flicker mode
 *
 *  @return >=0 success, <0 failure
 */
int AppLibImage_SetAntiFlickerMode(UINT8 Mode)
{
    int ReturnValue = -1;
    AE_EV_LUT_s EvLut;
    AmbaPrintColor(GREEN, "%s(): set anti-flicker type = %d (0:Auto, 1:60HZ, 2:50HZ, 20:NO_50HZ)", __FUNCTION__, Mode);
    ReturnValue = AmbaImg_Proc_Cmd(MW_IP_GET_MULTI_AE_EV_LUT, 0, (UINT32)&EvLut, 0);
    if (ReturnValue != OK) {
        AmbaPrintColor(RED, "[AppLib - Image] <SetAntiFlicker> MW_IP_GET_MULTI_AE_EV_LUT failure!!");
    }

    EvLut.FlickerMode = Mode;
    ReturnValue = AmbaImg_Proc_Cmd(MW_IP_SET_MULTI_AE_EV_LUT, 0, (UINT32)&EvLut, 0);
    if (ReturnValue != OK) {
        AmbaPrintColor(RED, "[AppLib - Image] <SetAntiFlicker> MW_IP_SET_MULTI_AE_EV_LUT failure!!");
    }

    ReturnValue = AmbaImg_Proc_Cmd(MW_IP_SET_FLICKER_CMD, (UINT32)1, 0, 0);
    if (ReturnValue != OK) {
        AmbaPrintColor(RED, "[AppLib - Image] <SetAntiFlicker> MW_IP_SET_FLICKER_CMD failure!!");
    } else {
        DBGMSGc2(GREEN,"[AppLib - Image] <EnableAntiFlicker> MW_IP_SET_FLICKER_CMD success!!");
    }

    return ReturnValue;
}



