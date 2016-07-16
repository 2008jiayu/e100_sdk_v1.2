/**
 * @file src/app/connected/applib/src/editor/ApplibEditor.c
 *
 * Implementation of Editor
 *
 * History:
 *    2014/01/14 - [Martin Lai] created file
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

#include <applib.h>
#include <format/Editor.h>
#include <format/Mp4Edt.h>
#include <AmbaCache_Def.h>
#include "../AppLibTask_Priority.h"

#define DEBUG_APPLIB_EDITOR
#if defined(DEBUG_APPLIB_EDITOR)
#define DBGMSG AmbaPrint
#define DBGMSGc(x) AmbaPrintColor(GREEN,x)
#define DBGMSGc2 AmbaPrintColor
#else
#define DBGMSG(...)
#define DBGMSGc(...)
#define DBGMSGc2(...)
#endif
static int ApplibEditorInitFlag = -1;
static AMP_EDT_FORMAT_HDLR_s *ApplibEditorHdlr = NULL;
static AMP_INDEX_HDLR_s *ApplibMp4EditorIndex = NULL;

/**
 *  @brief Initialize the editor module & mp4editor
 *
 *  Initialize the editor module
 *
 *  @return >=0 success, <0 failure
 */
int AppLibEditor_Init(void)
{
    /* Init format  */
    AMP_EDITOR_INIT_CFG_s EditorCfg = {0};
    AMP_MP4_EDT_INIT_CFG_s EdtInitCfg  = {0};
    void *EditorBufRaw = NULL;
    void *Mp4EditorBufRaw = NULL;
    int ReturnValue = -1;
    DBGMSG("[Applib - Editor] <Init> start");

    if (ApplibEditorInitFlag == 0) {
        return 0;
    }

    AmpEditor_GetInitDefaultCfg(&EditorCfg);
    EditorCfg.TaskInfo.Priority = APPLIB_EDITOR_TASK_PRIORITY;
    ReturnValue = AmpUtil_GetAlignedPool(APPLIB_G_MMPL, (void **)&EditorCfg.Buffer, &EditorBufRaw, EditorCfg.BufferSize, AMBA_CACHE_LINE_SIZE);
    if (ReturnValue < 0) {
        AmbaPrintColor(RED,"[Applib - Editor] <Init> Get buffer fail %s:%u  EditorCfg.BufferSize=%d", __FUNCTION__, __LINE__,EditorCfg.BufferSize);
        return ReturnValue;
    }
    ReturnValue = AmpEditor_Init(&EditorCfg);
    if (ReturnValue < 0) {
        AmbaPrintColor(RED,"[Applib - Editor] <Init> %s fail ReturnValue = %d", __FUNCTION__,ReturnValue);
        return ReturnValue;
    }
    AmpMp4Edt_GetInitDefaultCfg(&EdtInitCfg);
    if (AmpUtil_GetAlignedPool(APPLIB_G_MMPL, (void **)&EdtInitCfg.Buffer, &Mp4EditorBufRaw, EdtInitCfg.BufferSize, AMBA_CACHE_LINE_SIZE) != OK) {
        AmbaPrintColor(RED,"[Applib - Editor] Get buffer fail %s:%u", __FUNCTION__, __LINE__);
        return -1;
    }
    ReturnValue = AmpMp4Edt_Init(&EdtInitCfg);
    if (ReturnValue < 0) {
        AmbaPrintColor(RED,"[Applib - Editor] <MovieCrop2New> AmpMp4Edt_Init %s fail %u", __FUNCTION__,__LINE__);
        return ReturnValue;
    }
    ApplibEditorInitFlag = 0;
    DBGMSG("[Applib - Editor] <Init> end");
    return ReturnValue;
}


/**
 *  @brief The callback function for editor's process
 *
 *  The callback function for editor's process
 *
 *  @param [in] hdlr Handler
 *  @param [in] event Event id
 *  @param [in] info Information
 *
 *  @return >=0 success, <0 failure
 */
static int ApplibEditor_ProcessCB(void *hdlr, UINT32 event, void* info)
{
    switch (event) {
    case AMP_EDITOR_EVENT_OK:
        AppLibComSvcHcmgr_SendMsgNoWait(HMSG_EDTMGR_SUCCESS, 0, 0);
        DBGMSG("[Applib - Editor] <ProcessCB> AMP_EDITOR_EVENT_OK event %X info: %x", event, info);
        break;
    case AMP_EDITOR_EVENT_ERROR:
        AppLibComSvcHcmgr_SendMsgNoWait(HMSG_EDTMGR_FAIL, 0, 0);
        DBGMSG("[Applib - Editor] <ProcessCB> AMP_EDITOR_EVENT_ERROR event %X info: %x", event, info);
        break;
    default:
        AmbaPrint("[Applib - Editor] <ProcessCB> Unknown event %X info: %x", event, info);
        break;
    }

    return AMP_OK;
}

/**
 *  @brief  To corp clip to new file
 *
 *  To corp clip to new file
 *
 *  @param [in] TimeStart clip start time (ms)
 *  @param [in] TimeEnd clip end time (ms)
 *  @param [in] FileNameIn Input File name
 *  @param [in] FileNameOut output File name
 *
 *  @return >=0 success, <0 failure
 */
int AppLibEditor_MovieCrop2New(UINT32 TimeStart, UINT32 TimeEnd, char *FileNameIn, char *FileNameOut)
{
    int ReturnValue = -1;
    if (ApplibMp4EditorIndex == NULL) {
        AppLibIndex_CreateHdlr(&ApplibMp4EditorIndex);
    }

    if (ApplibEditorHdlr == NULL) {
        AMP_MP4_EDT_CFG_s Mp4EditCfg;
        Mp4EditCfg.Index = ApplibMp4EditorIndex;
        Mp4EditCfg.OnEvent = ApplibEditor_ProcessCB;
        AmpMp4Edt_Create(&Mp4EditCfg, &ApplibEditorHdlr);
        if (ApplibEditorHdlr == NULL) {
            AmbaPrintColor(RED,"[Applib - Editor] <MovieCrop2New> AmpMp4Edt_Create %s fail %u", __FUNCTION__,__LINE__);
            return -1;
        } else {
            ReturnValue = AmpEditor_Crop2New(ApplibEditorHdlr, FALSE, TimeStart, TimeEnd, FALSE, FileNameIn, FileNameOut);
        }
    } else {
        ReturnValue = AmpEditor_Crop2New(ApplibEditorHdlr, FALSE, TimeStart, TimeEnd, FALSE, FileNameIn, FileNameOut);
    }

    return ReturnValue;
}


/**
 *  @brief  To corp clip to new file
 *
 *  To corp clip to new file
 *
 *  @param [in] FileNameIn1 file 1 to merge
 *  @param [in] FileNameIn2 file 2 to merge
 *  @param [in] FileNameOut output filename.
 *
 *  @return >=0 success, <0 failure
 */
int AppLibEditor_MovieMerge(char *FileNameIn1, char *FileNameIn2)
{
    int ReturnValue = -1;
    if (ApplibMp4EditorIndex == NULL) {
        AppLibIndex_CreateHdlr(&ApplibMp4EditorIndex);
    }


    if (ApplibEditorHdlr == NULL) {
        AMP_MP4_EDT_CFG_s Mp4EditCfg;
        Mp4EditCfg.Index = ApplibMp4EditorIndex;
        Mp4EditCfg.OnEvent = ApplibEditor_ProcessCB;
        AmpMp4Edt_Create(&Mp4EditCfg, &ApplibEditorHdlr);
        if (ApplibEditorHdlr == NULL) {
            AmbaPrintColor(RED,"[Applib - Editor] <MovieCrop2New> AmpMp4Edt_Create %s fail %u", __FUNCTION__,__LINE__);
            return -1;
        } else {
            ReturnValue = AmpEditor_Merge(ApplibEditorHdlr, FALSE, FALSE, FALSE, FileNameIn1, FileNameIn2);
        }
    } else {
        ReturnValue = AmpEditor_Merge(ApplibEditorHdlr, FALSE, FALSE, FALSE, FileNameIn1, FileNameIn2);
    }

    return ReturnValue;
}

/**
 *  @brief  release the handler after doing editor
 *
 * release the handler after doing editor
 *
 *
 *  @return >=0 success, <0 failure
 */
int AppLibEditor_EditComplete(void)
{
    int ReturnValue = 0;
    if (ApplibEditorHdlr != NULL) {
        ReturnValue = AmpMp4Edt_Delete(ApplibEditorHdlr);
        if (ReturnValue != AMP_OK) {
            AmbaPrintColor(RED,"[Applib - Editor] <EditComplete> AmpMp4Edt_Delete %s fail %u", __FUNCTION__,__LINE__);
        } else {
            ApplibEditorHdlr = NULL;
        }
    }
    if (ApplibMp4EditorIndex != NULL) {
        AppLibIndex_DeleteHdlr(ApplibMp4EditorIndex);
        ApplibMp4EditorIndex = NULL;
    }
    return ReturnValue;
}

/**
 *  @brief  To recover the video clip.
 *
 *  To recover the video clip.
 *
 *  @param [in] filename File name
 *
 *  @return >=0 success, <0 failure
 */
int AppLibEditor_MovieRecover(char *filename)
{
    int ReturnValue = -1;
#ifdef CONFIG_APP_ARD
	if(filename == NULL){
		return -1;
	}
	
	if(strlen(filename) == 0){
		return -1;
	}
#endif    
    if (ApplibMp4EditorIndex == NULL) {
        AppLibIndex_CreateHdlr(&ApplibMp4EditorIndex);
    }

    if (ApplibEditorHdlr == NULL) {
        AMP_MP4_EDT_CFG_s Mp4EditCfg;
        AmpMp4Edt_GetDefaultCfg(&Mp4EditCfg);
        Mp4EditCfg.Index = ApplibMp4EditorIndex;
        Mp4EditCfg.OnEvent = ApplibEditor_ProcessCB;
        AmpMp4Edt_Create(&Mp4EditCfg, &ApplibEditorHdlr);
        if (ApplibEditorHdlr == NULL) {
            AmbaPrintColor(RED,"[Applib - Editor] <MovieRecover> AmpMp4Edt_Create %s fail %u", __FUNCTION__,__LINE__);
            return -1;
        } else {
            ReturnValue = AmpEditor_Recover(ApplibEditorHdlr, FALSE, filename);
        }
    } else {
        ReturnValue = AmpEditor_Recover(ApplibEditorHdlr, FALSE, filename);
    }

    return ReturnValue;
}


/**
 *  @brief To release the resource when the system finish recovering clip.
 *
 *  To release the resource when the system finish recovering clip.
 *
 *  @return >=0 success, <0 failure
 */
int AppLibEditor_MovieRecoverComplete(void)
{
    int ReturnValue = 0;
    if (ApplibEditorHdlr != NULL) {
        ReturnValue = AmpMp4Edt_Delete(ApplibEditorHdlr);
        if (ReturnValue != AMP_OK) {
            AmbaPrintColor(RED,"[Applib - Editor] <MovieRecoverComplete> AmpMp4Edt_Delete %s fail %u", __FUNCTION__,__LINE__);
        } else {
            ApplibEditorHdlr = NULL;
        }
    }
    if (ApplibMp4EditorIndex != NULL) {
        AmpTempIdx_Delete(ApplibMp4EditorIndex);
        ApplibMp4EditorIndex = NULL;
    }
    return ReturnValue;
}

