/**
 * @file src/app/connected/applib/src/recorder/ApplibRecorder_LoopEnc
 *
 * Implementation of Loop Enc Card Full Handle
 *
 * History:
 *    2014/04/11 - [Annie Ting] created file
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

//#define DEBUG_APPLIB_LOOP_ENC
#if defined(DEBUG_APPLIB_LOOP_ENC)
#define DBGMSG AmbaPrint
#define DBGMSGc(x) AmbaPrintColor(GREEN,x)
#define DBGMSGc2 AmbaPrintColor
#else
#define DBGMSG(...)
#define DBGMSGc(...)
#define DBGMSGc2(...)
#endif




/** Global instance of manager */

static int ApplibLoopEncInitFlag = -1;


/**varible for loop encoder*/
static UINT32 Info = 0;
static UINT64 FileObjID = 0;
static char FileName[APP_MAX_FN_SIZE] = {0};



/**
 *  @brief Cack Back function for function search
 *
 *  search first able to delete file
 *
 *  @param [in] info point of info varible
 *  @param [in] FileObjID point of FileObjID  varible
 *  @param [in] FileName point of FileName  array
 *
 */
static int AppLibLoopEnc_SearchFirstFile(UINT32 param1, UINT32 param2)
{
    int ReturnValue = -1;
    int FileAmount = 0;
    AMP_CFS_STAT FileStat = {0};

    FileAmount =  AppLibStorageDmf_GetFileAmount(APPLIB_DCF_MEDIA_VIDEO, param1);
    FileObjID = AppLibStorageDmf_GetFirstFilePos(APPLIB_DCF_MEDIA_VIDEO, param1);
    ReturnValue = AppLibStorageDmf_GetFileName(APPLIB_DCF_MEDIA_VIDEO, ".MP4", APPLIB_DCF_EXT_OBJECT_SPLIT_FILE, param1, 0, CUR_OBJ(FileObjID), FileName);
    if (ReturnValue < 0) {
        AmbaPrint("[Applib - LoopEnc] <SearchFirstFile> Main file not exist, search for thm file.");
        ReturnValue = AppLibStorageDmf_GetFileName(APPLIB_DCF_MEDIA_VIDEO, ".MP4", APPLIB_DCF_EXT_OBJECT_SPLIT_THM, param1, 0, CUR_OBJ(FileObjID), FileName);
    }
    ReturnValue = AmpCFS_Stat(FileName,&FileStat);

    if (FileAmount > 0) {
      while (FileAmount > 0) {
            FileAmount --;
            if (!APPLIB_CHECKFLAGS(FileStat.Attr, AMP_CFS_ATTR_RDONLY)) {
                FileAmount = 0;
            } else {
                FileObjID = AppLibStorageDmf_GetNextFilePos(APPLIB_DCF_MEDIA_VIDEO, param1);
                ReturnValue = AppLibStorageDmf_GetFileName(APPLIB_DCF_MEDIA_VIDEO, ".MP4", APPLIB_DCF_EXT_OBJECT_SPLIT_FILE, param1, 0, CUR_OBJ(FileObjID), FileName);
                if (ReturnValue < 0) {
                    ReturnValue = AppLibStorageDmf_GetFileName(APPLIB_DCF_MEDIA_VIDEO, ".MP4", APPLIB_DCF_EXT_OBJECT_SPLIT_THM, param1, 0, CUR_OBJ(FileObjID), FileName);
                }
                ReturnValue = AmpCFS_Stat(FileName,&FileStat);
            }
        }
        if (ReturnValue == 0) {
            ReturnValue = APPLIB_ADDFLAGS(Info, LOOP_ENC_SEARCH_DONE);
        } else {
            ReturnValue = APPLIB_ADDFLAGS(Info, LOOP_ENC_SEARCH_ERROR);
        }
    } else {
        ReturnValue = APPLIB_ADDFLAGS(Info, LOOP_ENC_SEARCH_ERROR);
    }
    return ReturnValue;
}


/**
 *  @brief Cack Back function for function handle
 *
 *  delete file which search by function search
 *
 *  @param [in] info point of info varible
 *  @param [in] FileObjID point of FileObjID  varible
 *  @param [in] FileName point of FileName  array
 *
 */
static int AppLibLoopEnc_DeleteFile(UINT32 param1, UINT32 param2)
{
    int ReturnValue = 0;
    int FileAmount = 0;
    char drive = 'A';    
    
    /**if search fail skip delete file*/
    if (APPLIB_CHECKFLAGS(Info, LOOP_ENC_SEARCH_ERROR)) {
        return ReturnValue;
    }

    FileAmount =  AppLibStorageDmf_GetFileAmount(APPLIB_DCF_MEDIA_VIDEO, param1);

    if (FileAmount > 0) {
        ReturnValue = AppLibStorageDmf_DeleteFile(APPLIB_DCF_MEDIA_VIDEO,FileObjID, param1);
        if (ReturnValue == 0) {
            ReturnValue = APPLIB_ADDFLAGS(Info, LOOP_ENC_HANDLE_DONE);
        } else {
            ReturnValue = APPLIB_ADDFLAGS(Info, LOOP_ENC_HANDLE_ERROR);
        }
      // drive += AppLibCard_GetActiveSlot();
      // ReturnValue = AppLibStorageDmf_Refresh(drive);      //modify by ouyangjian        
    }
    return ReturnValue;
}


/**
 *  @brief Cack Back function for function return
 *
 *  return loop enc result to app
 *
 *  @param [in] info point of info varible
 *  @param [in] FileObjID point of FileObjID  varible
 *  @param [in] FileName point of FileName  array
 *
 */
static int AppLibLoopEnc_Return(void)
{
    int ReturnValue = 0;
    if (APPLIB_CHECKFLAGS(Info, LOOP_ENC_SEARCH_ERROR)) {
        AppLibComSvcHcmgr_SendMsgNoWait(HMSG_LOOP_ENC_ERROR, 0, 0);
    } else if (APPLIB_CHECKFLAGS(Info, LOOP_ENC_HANDLE_ERROR)) {
        AppLibComSvcHcmgr_SendMsgNoWait(HMSG_LOOP_ENC_ERROR, 1, 0);
    } else if (APPLIB_CHECKFLAGS(Info, LOOP_ENC_SEARCH_DONE) && APPLIB_CHECKFLAGS(Info, LOOP_ENC_HANDLE_DONE)) {
        AppLibComSvcHcmgr_SendMsgNoWait(HMSG_LOOP_ENC_DONE, 0, 0);
    }

    Info = 0;
    return ReturnValue;
}

/**
 *  @brief feed back loop enc function status
 *
 *  return loop enc result to app
 *
 *  @param [in] info point of info varible
 *  @param [in] FileObjID point of FileObjID  varible
 *  @param [in] FileName point of FileName  array
 *
 *  @ReturnValue 0:do nothing 1:Loop enc all done 2:search file done
 *                       -1:Search file error  -2:Delete file error
 *
 */
int AppLibLoopEnc_StepCheck(void)
{
    int ReturnValue = 0;

    if (APPLIB_CHECKFLAGS(Info, LOOP_ENC_SEARCH_ERROR)) {
        ReturnValue = -1;/**<Search file error*/
    } else if (APPLIB_CHECKFLAGS(Info, LOOP_ENC_HANDLE_ERROR)) {
        ReturnValue = -2;/**<Delete file error*/
    } else if (APPLIB_CHECKFLAGS(Info, LOOP_ENC_SEARCH_DONE) && APPLIB_CHECKFLAGS(Info, LOOP_ENC_HANDLE_DONE)) {
        ReturnValue = 2;/**<Loop enc all done*/
    }  else if (APPLIB_CHECKFLAGS(Info, LOOP_ENC_SEARCH_DONE)) {
        ReturnValue = 1;/**<search file done*/
    }  else {
        ReturnValue = 0;/**<do nothing*/
    }

    return ReturnValue;
}


/**
 *  @brief Initialization of Loop Encoder manager.
 *
 *   Initialization of Loop Encoder manager.
 *
 *  @return >=0 success, <0 failure
 */
int AppLibLoopEnc_Init(void)
{
    int ReturnValue = 0;
    APPLIB_ENCODE_HANDLER_s LoopEncHandler = {0};

    if (ApplibLoopEncInitFlag == 0)
        return 0;
    DBGMSG("[Applib - AsyncOp] <AppLibLoopEnc_Init> start");



    /**register loop encoder handler*/
    LoopEncHandler.FuncSearch = &AppLibLoopEnc_SearchFirstFile;
    LoopEncHandler.FuncHandle = &AppLibLoopEnc_DeleteFile;
    LoopEncHandler.FuncReturn = &AppLibLoopEnc_Return;
    LoopEncHandler.Command = HMSG_LOOP_ENC_START;
    AppLibStorageAsyncOp_RegHandler(&LoopEncHandler);

    ApplibLoopEncInitFlag = 0;
    DBGMSG("[Applib - LoopEnc] <AppLibLoopEnc_Init> end: ReturnValue = %d", ReturnValue);

    return ReturnValue;
}



