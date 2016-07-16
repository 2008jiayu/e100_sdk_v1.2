/**
 * @file src/app/connected/applib/inc/storage/ApplibStorage_DmfDateTime.h
 *
 * Header of storage DMF(Digital Media File System) Apis
 *
 * History:
 *    2015/01/30 - [Evan Ji] created file
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

#ifndef APPLIB_STORAGE_DMF_DATE_TIME_H_
#define APPLIB_STORAGE_DMF_DATE_TIME_H_

/**
* @defgroup ApplibStorage_Dmf
* @brief storage DMF(Digital Media File System) Apis
*
*
*/

/**
 * @addtogroup ApplibStorage_Dmf
 * @ingroup Storage
 * @{
 */
#include <applib.h>
__BEGIN_C_PROTO__

/*************************************************************************
 * digital media file system definitions
 ************************************************************************/
#define STORAGE_DCF_TYPE_DEFAULT    (0)

/**used to decided dmf folder for emergency on or off*/
#define STORAGE_EMERGENCY_DCF_ON

#if defined(STORAGE_EMERGENCY_DCF_ON)
#define STORAGE_DCF_HDLR_MAX    (2)
#define DCIM_HDLR   (0)
#define EVENTRECORD_HDLR   (1)
#else
#define STORAGE_DCF_HDLR_MAX    (1)
#define DCIM_HDLR   (0)
#endif
/**
 * storage error message
 */
#define STORAGE_DMF_MKDIR_FAIL (-1)
#define STORAGE_DMF_UNREACHABLE (-2)
#define STORAGE_DMF_DIR_IDX_REACH_LIMIT (-3)
#define STORAGE_DMF_RTC_IDX_REACH_LIMIT    (-4)
#define STORAGE_DMF_INCORRECT_EXT_FILE_TYPE (-5)
#define STORAGE_DMF_DIR_NAME_ILLEGAL (-6)
#define STORAGE_DMF_EXT_OBJECT_IMAGE_THM_SET_FAIL (-7)
#define STORAGE_DMF_EXT_OBJECT_SPLIT_FILE_SET_FAIL (-8)
#define STORAGE_DMF_EXT_OBJECT_SPLIT_THM_SET_FAIL (-9)
/** extract current object ID and current object ID Index */
#define CUR_OBJ(x)  ((UINT32)x)
#define CUR_OBJ_IDX(x) ((UINT32)(x >> 32))
/**
 * storage dmf
 */
typedef struct _APPLIB_STORAGE_DMF_s_ {
    int Mode; /**< Storage dmf mode */
#define DMF_MODE_RESET    (0)
#define DMF_MODE_SERIAL    (1)
} APPLIB_STORAGE_DMF_s;

extern char g_AppLibStorageRootName[STORAGE_DCF_HDLR_MAX][32];
/*************************************************************************
 * File system and DMF APIs
 ************************************************************************/
/**
 *  DMF module initialization
 *
 *  @param [in] dcfType DCF type
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibStorageDmf_Init(int dcfType);

/**
 *  DMF module setting.
 *
 *  @param [out] setting DMF module setting.
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibStorageDmf_GetSetting(APPLIB_STORAGE_DMF_s *setting);

/**
 *  @brief Setup the DMF mode
 *
 *  Setup the DMF mode
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibStorageDmf_SetupMode(void);

/**
 *  Refresh the DCF handler
 *
 *  @param [in] drive Drive id
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibStorageDmf_Refresh(char drive);

/**
 *  Delete the DCF handler
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibStorageDmf_DeleteHandler(void);

/**
 *  Get the position of the current file
 *
 *  @param [in] mediaType Media type
 *  @param [in] Type DCF handler
 *
 *  @return >0 File position, the others is failure
 */
extern UINT64 AppLibStorageDmf_GetCurrFilePos(APPLIB_DCF_MEDIA_TYPE_e mediaType, UINT8 Type);

/**
 *  Get the position of the first file
 *
 *  @param [in] mediaType Media type
 *  @param [in] Type DCF handler
 *
 *  @return >0 File position, the others is failure
 */
extern UINT64 AppLibStorageDmf_GetFirstFilePos(APPLIB_DCF_MEDIA_TYPE_e mediaType, UINT8 Type);

/**
 *  Get the position of the last file
 *
 *  @param [in] mediaType Media type
 *  @param [in] Type DCF handler
 *
 *  @return >0 File position, the others is failure
 */
extern UINT64 AppLibStorageDmf_GetLastFilePos(APPLIB_DCF_MEDIA_TYPE_e mediaType, UINT8 Type);

/**
 *  Get the position of the next file
 *
 *  @param [in] mediaType Media type
 *  @param [in] Type DCF handler
 *
 *  @return >0 File position, the others is failure
 */
extern UINT64 AppLibStorageDmf_GetNextFilePos(APPLIB_DCF_MEDIA_TYPE_e mediaType, UINT8 Type);

/**
 *  Get the position of previous file
 *
 *  @param [in] mediaType Media type
 *  @param [in] Type DCF handler
 *
 *  @return >0 File position, the others is failure
 */
extern UINT64 AppLibStorageDmf_GetPrevFilePos(APPLIB_DCF_MEDIA_TYPE_e mediaType, UINT8 Type);
/**
 *  Get file list
 *
 *  @param [in] mediaType Media type
 *  @param [in] list File list
 *  @param [in] Type DCF handler
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibStorageDmf_GetFileList(APPLIB_DCF_MEDIA_TYPE_e mediaType, AMP_DCF_FILE_LIST_s *list, UINT8 Type);

/**
 *  Get the file name
 *
 *  @param [in] mediaType Media type
 *  @param [in] extName Extend Name
 *  @param [in] extType Extend type
 *  @param [in] Type handler type
 *  @param [in] Index file index
 *  @param [in] objId Object ID
 *  @param [out] filename File name
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibStorageDmf_GetFileName(APPLIB_DCF_MEDIA_TYPE_e mediaType, char *extName, UINT8 extType, UINT8 Type, UINT32 Index, UINT32 objId, char *filename);

/**
 *  @brief Get the file name By Root name
 *
 *  Get the file name
 *
 *  @param [in] mediaType Media type
 *  @param [in] objId Object ID
 *  @param [in] Type Root name ID
 *  @param [out] filename File name
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibStorageDmf_GetFileNameByType(APPLIB_DCF_MEDIA_TYPE_e mediaType, UINT8 Type, UINT32 objId, char *filename);

/**
 *  Get the position of first directory
 *
 *  @param [in] mediaType Media type
 *  @param [in] Type DCF handler
 *
 *  @return >0 The position of first directory, otherwise failure
 */
extern int AppLibStorageDmf_GetFirstDirPos(APPLIB_DCF_MEDIA_TYPE_e mediaType, UINT8 Type);

/**
 *  Get the position of last directory
 *
 *  @param [in] mediaType Media type
 *  @param [in] Type DCF handler
 *
 *  @return >0 The position of last directory, otherwise failure
 */
extern int AppLibStorageDmf_GetLastDirPos(APPLIB_DCF_MEDIA_TYPE_e mediaType, UINT8 Type);

/**
 *  Get the position of next directory
 *
 *  @param [in] mediaType Media type
 *
 *  @return >0 The position of next directory, otherwise failure
 */
extern int AppLibStorageDmf_GetNextDirPos(APPLIB_DCF_MEDIA_TYPE_e mediaType, UINT8 Type);

/**
 *  Get the position of previous directory
 *
 *  @param [in] mediaType Media type
 *  @param [in] Type DCF handler
 *
 *  @return >0 The position of previous directory, otherwise failure
 */
extern int AppLibStorageDmf_GetPrevDirPos(APPLIB_DCF_MEDIA_TYPE_e mediaType, UINT8 Type);

/**
 *  @brief Initialize current file amount in the working directory.
 *
 *  Initialize current file amount in the working directory.
 *
 *  @param [in] amount: files have in the working directory
 *  @param [in] Type DCF handler
 *
 *  @return >=0 success, <0 failure
 */
int AppLibStorageDmf_InitFileCountInTheDir(UINT32 amount, UINT8 Type);

/**
 *  @brief Minus current file amount in the working directory.
 *
 *  Minus current file amount in the working directory.
 *  The mximum files in the working directory is limited by DCF_FILE_AMOUNT_MAX.
 *
 *  @param [in] Type DCF handler
 *
 *  @return >=0 success, <0 failure
 */
int AppLibStorageDmf_MinusFileCountInTheDir(char *Name);

/**
 *  @brief Add current file amount in the working directory.
 *
 *  Add current file amount in the working directory.
 *  The mximum files in the working directory is limited by DCF_FILE_AMOUNT_MAX.
 *
 *  @param [in] Type DCF handler
 *
 *  @return >=0 success, <0 failure
 */
int AppLibStorageDmf_AddFileCountInTheDir(UINT8 Type);

/**
 *  @brief Get current file amount in the working directory.
 *
 *  Get current file amount in the working directory.
 *
 *  @param [in] Type DCF handler
 *
 *  @return >=0 success, <0 failure
 */
int AppLibStorageDmf_GetFileCountInTheDir(UINT8 Type);

/**
 *  @brief Init RTC reset directory index.
 *
 *  Int RTC reset directory index to 0.
 *
 *  @param [in] Type DCF handler
 *
 *  @return >=0 success, <0 failure
 */
int AppLibStorageDmf_InitRtcDirIndex(UINT8 Type);

/**
 *  @brief Set current used RTC reset directory index.
 *
 *  Set current directory index.
 *  The range of RTC reset directory is from 1 to 9.
 *  Using this API to save it.
 *
 *  @param [in] RtcIndex: RTC reset directory index
 *  @param [in] Type DCF handler
 *
 *  @return >=0 success, <0 failure
 */
int AppLibStorageDmf_SetRtcDirIndex(UINT32 RtcIndex, UINT8 Type);

/**
 *  @brief Get current used RTC reset directory index.
 *
 *  Get current directory index.
 *
 *  @param [in] Type DCF handler
 *
 *  @return >=0 success, <0 failure
 */
int AppLibStorageDmf_GetRtcDirIndex(UINT8 Type);

/**
 *  @brief Init all directory index to zero.
 *
 *  Init all directory index to -1 to unuse state.
 *
 *  @param [in] Type DCF handler
 *
 *  @return 0 success, <0 failure
 */
int AppLibStorageDmf_InitDirIndex(UINT8 Type);

/**
 *  @brief Set directory index into specitic directory number.
 *
 *  Set current directory index into specific directory number/offset.
 *  The directory number uses the offset from specific date to the base date 2014/01/01.
 *  For example, the directory number of 2015/02/02 is the offset between this date to 2014/01/01.
 *  The index range of Each directory number is from 0 to 99. Using this API to save it.
 *
 *  @param [in] dnum: Directory number
 *  @param [in] index: Index of specific Directory number
 *  @param [in] Type DCF handler
 *
 *  @return >=0 success, <0 failure
 */
int AppLibStorageDmf_SetDirIndex(UINT32 dnum, UINT32 index, UINT8 Type);

/**
 *  @brief Get current directory index from specific directory number
 *
 *  Get current directory index from specific directory number/offset.
 *
 *  @param [in] Type DCF handler
 *
 *  @return >=0 success, <0 failure
 */
int AppLibStorageDmf_GetDirIndex(UINT32 dnum, UINT8 Type);

/**
 *  Get the directory list
 *
 *  @param [in] mediaType Media type
 *  @param [in] list Directory list
 *  @param [in] Type DCF handler
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibStorageDmf_GetDirList(APPLIB_DCF_MEDIA_TYPE_e mediaType, AMP_DCF_DIR_LIST_s *list, UINT8 Type);

/**
 *  Create a file
 *
 *  @param [in] mediaType Media type
 *  @param [in] extName Extent name
 *  @param [in] Type DCF handler
 *  @param [out] filename File name
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibStorageDmf_CreateFile(APPLIB_DCF_MEDIA_TYPE_e mediaType, char *extName, UINT8 Type, char *filename);

/**
 *  @brief Create a file
 *
 *  Create a file
 *
 *  @param [in] mediaType Media type
 *  @param [in] Type Root name ID
 *  @param [in] extName Extent name
 *  @param [out] filename File name
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibStorageDmf_CreateFileByType(APPLIB_DCF_MEDIA_TYPE_e mediaType, UINT8 Type, char *extName, char *filename);

/**
 *  Create a extended file
 *
 *  @param [in] mediaType Media type
 *  @param [in] objId Object Id
 *  @param [in] extName Extent name
 *  @param [in] extType Extent type
 *  @param [in] seqNum Sequential number
 *  @param [in] Type DCF handler
 *  @param [out] filename File name
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibStorageDmf_CreateFileExtended(APPLIB_DCF_MEDIA_TYPE_e mediaType, UINT32 objId, char *extName, UINT8 extType, UINT8 seqNum, UINT8 Type, char *filename);

/**
 *  Delete file in the table
 *
 *  @param [in] mediaType Media type
 *  @param [in] objId Object id
 *  @param [in] Type DCF handler
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibStorageDmf_DeleteFile(APPLIB_DCF_MEDIA_TYPE_e mediaType, UINT32 objId, UINT8 Type);

/**
 *  Get file amount
 *
 *  @param [in] mediaType Media type
 *  @param [in] Type DCF handler
 *
 *  @return File amount
 */
extern int AppLibStorageDmf_GetFileAmount(APPLIB_DCF_MEDIA_TYPE_e mediaType, UINT8 Type);
/**
 *  @brief Get object file count
 *
 *  Get Get object file count
 *
 *  @param [in] Type DCF handler
 *  @param [in] ObjID object ID
 *
 *  @return File count
 */
extern int AppLibStorageDmf_GetObjFileCount(UINT8 Type, UINT32 ObjID);

/**
 *  @brief Get object file count
 *
 *  Get Get object file count
 *
 *  @param [in] Type DCF handler
 *  @param [in] ObjID object ID
 *
 *  @return File count
 */
extern int AppLibStorageDmf_GetObjFileCount(UINT8 Type, UINT32 ObjID);
/**
 *  @brief Set File Number Mode
 *
 *  Set File Number Mode
 *
 *  @param [in] mediaType Media type
 *
 *  @param [in] NumberMode Number Mode
 *
 *  @param [in] Type DCF handler
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibStorageDmf_SetFileNumberMode(APPLIB_DCF_MEDIA_TYPE_e mediaType, APPLIB_DCF_NUMBER_MODE_e NumberMode, UINT8 Type);

/**
 *  @brief Set System Boot Status
 *
 *  Set System Boot Status
 *
 *  @param [in] Type DCF handler
 *
 *  @param [in] Boot status, The values are defined by APPLIB_DCF_BOOT_STATUS_e in ApplibDcfDateTime.h
 *
 *  @return >=0 success, <0 failure
 */
extern INT8 AppLibStorageDmf_SetBootStatus(UINT8 Type, INT8 Status);

/**
 *  @brief Get System Boot Status
 *
 *  Get System Boot Status
 *
 *  @param [in] Type DCF handler
 *
 *  @return >=0 success, <0 failure
 */
extern INT8 AppLibStorageDmf_GetBootStatus(UINT8 Type);

/**
 *  @brief Set Current Dnum
 *
 *  Set Current Dnum
 *
 *  @param [in] Type DCF handler
 *
 *  @param [in] Current Dnum
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibStorageDmf_SetCurDnum(UINT8 Type, UINT32 Dnum);

/**
 *  @brief Get Current Dnum
 *
 *  Get Current Dnum
 *
 *  @param [in] Type DCF handler
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibStorageDmf_GetCurDnum(UINT8 Type);

/**
 *  @brief Set Current Dnum Index
 *
 *  Set Current Dnum Index
 *
 *  @param [in] Type DCF handler
 *
 *  @param [in] Current Dnum index
 *
 *  @return >=0 success, <0 failure
 */
extern INT8 AppLibStorageDmf_SetCurDnumIdx(UINT8 Type, INT8 DnumIdx);

/**
 *  @brief Get Current Dnum Index
 *
 *  Get Current Dnum Index
 *
 *  @param [in] Type DCF handler
 *
 *  @return >=0 success, <0 failure
 */
extern INT8 AppLibStorageDmf_GetCurDnumIdx(UINT8 Type);

/**
 *  @brief Set DMF Refresh Status
 *
 *  Set DMF Refresh Status. Set to TURE if DCF is refreshed.
 *
 *  @param [in] Refreshed DCF is refreshed or not.
 *
 *  @return 0
 */
extern int AppLibStorageDmf_SetRefreshStatus(BOOL Refreshed);

/**
 *  @brief Get DMF Refresh Status
 *
 *  Get DMF Refresh Status.
 *
 *  @param [in] ClearStatus Reset refreshed status to FLASE if ClearStatus is TRUE.
 *
 *  @return TRUE DCF has been refreshed, FALSE DCF has never refreshed
 */
extern BOOL AppLibStorageDmf_GetRefreshStatus(BOOL ClearStatus);

/**
 *  @brief Find out the max fnum in the specified directory
 *
 *  Find out the max fnum in the specified directory.
 *
 *  @param [in] DcfHdlrIdx DCF handler index
 *  @param [in] DnumScanned Dnum
 *  @param [in] DirPath Directory Path
 *  @param [out] RetMaxFnum The max fnum found
 *  @param [out] RetFileAmount The number of file in specified directory
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibStorageDmf_ScanDirForMaxFnum(UINT8 DcfHdlrIdx, UINT32 DnumScanned, char *DirPath,
                                                                 UINT32 *RetMaxFnum, UINT32 *RetFileAmount);


__END_C_PROTO__
/**
 * @}
 */
#endif /* APPLIB_STORAGE_DMF_DATE_TIME_H_ */
