/**
 * @file src/app/connected/applib/src/recorder/ApplibRecorder_Emergency
 *
 * Implementation of Emergency Record Handler CB Function
 *
 * History:
 *    2014/05/06 - [Annie Ting] created file
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
#include <AmbaAudio.h>
#include <AmbaUtility.h>
#include <recorder/Encode.h>
#include <recorder/AudioEnc.h>
#include <fifo/Fifo.h>


 //#define DEBUG_APPLIB_EMERGENCY
#if defined(DEBUG_APPLIB_EMERGENCY)
#define DBGMSG AmbaPrint
#define DBGMSGc(x) AmbaPrintColor(GREEN,x)
#define DBGMSGc2 AmbaPrintColor
#else
#define DBGMSG(...)
#define DBGMSGc(...)
#define DBGMSGc2(...)
#endif


static int ApplibRecorderEmInitFlag = -1;

static UINT32 FileObjIDOut = 0;
static UINT32 TimeStart,TimeEnd;
static WCHAR FileNameIn1[APP_MAX_FN_SIZE] = {0};
static WCHAR FileNameIn2[APP_MAX_FN_SIZE] = {0};
static WCHAR FileNameOut[APP_MAX_FN_SIZE] = {0};

/**
 *  @brief Cack Back function for function search
 *
 *  get file name form object id and caculate the cuting timestamp
 *
 *
 */
static int AppLibEmergency_GetSettingForCrop(UINT32 ObjID1, UINT32 SplitTime)
{
    int ReturnValue = -1;
    APPLIB_MEDIA_INFO_s  MediaInfo;
    UINT32 DTS,TimeScale;

    ReturnValue = AppLibStorageDmf_GetFileName(APPLIB_DCF_MEDIA_VIDEO, L".MP4", APPLIB_DCF_EXT_OBJECT_SPLIT_FILE, 0, 0, ObjID1, FileNameIn1);
    ReturnValue = AppLibFormat_GetMediaInfo(FileNameIn1, &MediaInfo);/**<get clip mediainfo*/
    if (ReturnValue == AMP_OK) {
        DTS = MediaInfo.MediaInfo.Movie->Track[0].DTS;
        TimeScale = MediaInfo.MediaInfo.Movie->Track[0].TimeScale;
        AmbaPrintColor(5,"DTS = %d,TimeScale = %d",DTS,MediaInfo.MediaInfo.Movie->Track[0].TimeScale);
        TimeEnd = (((UINT64)(DTS) / (TimeScale)) * 1000 + (((UINT64)(DTS) % (TimeScale)) * 1000) /(TimeScale));
        TimeStart = TimeEnd - SplitTime*1000;
        AmbaPrintColor(5,"crop file start time = %d,end time = %d",TimeStart,TimeEnd);
        FileObjIDOut = AppLibStorageDmf_CreateFileByType(APPLIB_DCF_MEDIA_VIDEO, 1,L"MP4", FileNameOut);
    } else {
        AmbaPrintColor(5,"get file media info fail stop emergency record return value = %d",ReturnValue);
    }

    return ReturnValue;
}

/**
 *  @brief Cack Back function for function handle
 *
 *  crop file to new file get by func search
 *
 *
 */
static int AppLibEmergency_CropFileToNew(void)
{
    int ReturnValue = 0;
    ReturnValue = AppLibEditor_MovieCrop2New(TimeStart,TimeEnd, FileNameIn1, FileNameOut);
    return ReturnValue;
}

/**
 *  @brief Cack Back function for function search
 *
 *  get file name form object id
 *
 *
 */
static int AppLibEmergency_GetFileNameForMerge(UINT32 ObjID1, UINT32 ObjID2)
{
    int ReturnValue = -1;
    ReturnValue = AppLibStorageDmf_GetFileName(APPLIB_DCF_MEDIA_VIDEO, L".MP4", APPLIB_DCF_EXT_OBJECT_SPLIT_FILE, 1, 0, ObjID1, FileNameIn1);
    ReturnValue = AppLibStorageDmf_GetFileName(APPLIB_DCF_MEDIA_VIDEO, L".MP4", APPLIB_DCF_EXT_OBJECT_SPLIT_FILE, 0, 0, ObjID2, FileNameIn2);

    return ReturnValue;
}

/**
 *  @brief Cack Back function for function search
 *
 *  get file name form object id, and create new file for merge
 *
 *
 */
static int AppLibEmergency_GetNewFileNameForMerge(UINT32 ObjID1, UINT32 ObjID2)
{
    int ReturnValue = -1;
    ReturnValue = AppLibStorageDmf_GetFileName(APPLIB_DCF_MEDIA_VIDEO, L".MP4", APPLIB_DCF_EXT_OBJECT_SPLIT_FILE, 0, 0, ObjID1, FileNameIn1);
    ReturnValue = AppLibStorageDmf_GetFileName(APPLIB_DCF_MEDIA_VIDEO, L".MP4", APPLIB_DCF_EXT_OBJECT_SPLIT_FILE, 0, 0, ObjID2, FileNameIn2);
    FileObjIDOut = AppLibStorageDmf_CreateFileByType(APPLIB_DCF_MEDIA_VIDEO, 1,L"MP4", FileNameOut);
    return ReturnValue;
}

/**
 *  @brief Cack Back function for function handle
 *
 *  merge file to new file get by func search
 *
 *
 */
static int AppLibEmergency_MergeFile(void)
{
    int ReturnValue = -1;
    ReturnValue = AppLibEditor_MovieMerge(FileNameIn1, FileNameIn2);
    return ReturnValue;
}

/**
 *  @brief Cack Back function for function handle
 *
 *  merge file to new file get by func search
 *
 *
 */
static int AppLibEmergency_MergeFileToNew(void)
{
    int ReturnValue = -1;
    ReturnValue = AppLibEditor_MovieMerge(FileNameIn1, FileNameIn2);
    ReturnValue = AmpCFS_Move(FileNameIn1,FileNameOut);
    return ReturnValue;
}


/**
 *  @brief Cack Back function for function return
 *
 *  return new file object id
 *
 *
 */
static int AppLibEmergency_ReturnObjectID(void)
{
    int ReturnValue = 0;
    AppLibComSvcHcmgr_SendMsgNoWait(HMSG_EM_RECORD_RETURN, FileObjIDOut, 0);
    return ReturnValue;
}

/**
 *  @brief Initialization of Emergency record CB function.
 *
 *   Initialization of Emergency record CB function.
 *
 *  @return >=0 success, <0 failure
 */
int AppLibEmergency_Init(void)
{
    int ReturnValue = 0;
    APPLIB_ENCODE_HANDLER_s Merge2NewHandler = {0};
    APPLIB_ENCODE_HANDLER_s Merge2FirstHandler = {0};
    APPLIB_ENCODE_HANDLER_s Crop2NewHandler = {0};

    if (ApplibRecorderEmInitFlag == 0)
        return 0;

    Merge2NewHandler.FuncSearch = &AppLibEmergency_GetNewFileNameForMerge;
    Merge2NewHandler.FuncHandle = &AppLibEmergency_MergeFileToNew;
    Merge2NewHandler.FuncReturn = &AppLibEmergency_ReturnObjectID;
    Merge2NewHandler.Command = HMSG_EM_RECORD_MERGE2NEW;

    Merge2FirstHandler.FuncSearch = &AppLibEmergency_GetFileNameForMerge;
    Merge2FirstHandler.FuncHandle = &AppLibEmergency_MergeFile;
    Merge2FirstHandler.FuncReturn = NULL;
    Merge2FirstHandler.Command = HMSG_EM_RECORD_MERGE2FIRST;

    Crop2NewHandler.FuncSearch = &AppLibEmergency_GetSettingForCrop;
    Crop2NewHandler.FuncHandle = &AppLibEmergency_CropFileToNew;
    Crop2NewHandler.FuncReturn = &AppLibEmergency_ReturnObjectID;
    Crop2NewHandler.Command = HMSG_EM_RECORD_CROP2NEW;

    AppLibStorageAsyncOp_RegHandler(&Merge2NewHandler);
    AppLibStorageAsyncOp_RegHandler(&Merge2FirstHandler);
    AppLibStorageAsyncOp_RegHandler(&Crop2NewHandler);

    ApplibRecorderEmInitFlag = 0;

    return ReturnValue;
}
