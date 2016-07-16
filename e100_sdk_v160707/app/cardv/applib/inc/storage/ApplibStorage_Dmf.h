/**
 * @file src/app/connected/applib/inc/storage/ApplibStorage_Dmf.h
 *
 * Header of storage DMF(Digital Media File System) Apis
 *
 * History:
 *    2013/12/05 - [Martin Lai] created file
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

#ifndef APPLIB_STORAGE_DMF_H_
#define APPLIB_STORAGE_DMF_H_

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
 * storage dmf
 */
typedef struct _APPLIB_STORAGE_DMF_s_ {
    int Mode; /**< Storage dmf mode */
#define DMF_MODE_RESET    (0)
#define DMF_MODE_SERIAL    (1)
} APPLIB_STORAGE_DMF_s;

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
extern int AppLibStorageDmf_GetCurrFilePos(APPLIB_DCF_MEDIA_TYPE_e mediaType, UINT32 Type);

/**
 *  Get the position of the first file
 *
 *  @param [in] mediaType Media type
 *  @param [in] Type DCF handler
 *
 *  @return >0 File position, the others is failure
 */
extern int AppLibStorageDmf_GetFirstFilePos(APPLIB_DCF_MEDIA_TYPE_e mediaType, UINT32 Type);

/**
 *  Get the position of the last file
 *
 *  @param [in] mediaType Media type
 *  @param [in] Type DCF handler
 *
 *  @return >0 File position, the others is failure
 */
extern int AppLibStorageDmf_GetLastFilePos(APPLIB_DCF_MEDIA_TYPE_e mediaType, UINT32 Type);

/**
 *  Get the position of the next file
 *
 *  @param [in] mediaType Media type
 *  @param [in] Type DCF handler
 *
 *  @return >0 File position, the others is failure
 */
extern int AppLibStorageDmf_GetNextFilePos(APPLIB_DCF_MEDIA_TYPE_e mediaType, UINT32 Type);

/**
 *  Get the position of previous file
 *
 *  @param [in] mediaType Media type
 *  @param [in] Type DCF handler
 *
 *  @return >0 File position, the others is failure
 */
extern int AppLibStorageDmf_GetPrevFilePos(APPLIB_DCF_MEDIA_TYPE_e mediaType, UINT32 Type);
/**
 *  Get file list
 *
 *  @param [in] mediaType Media type
 *  @param [in] list File list
*  @param [in] Type DCF handler
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibStorageDmf_GetFileList(APPLIB_DCF_MEDIA_TYPE_e mediaType, AMP_DCF_FILE_LIST_s *list, UINT32 Type);

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
extern int AppLibStorageDmf_GetFileName(APPLIB_DCF_MEDIA_TYPE_e mediaType, char *extName, UINT8 extType, UINT32 Type, UINT32 Index, UINT32 objId, char *filename);

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
extern int AppLibStorageDmf_GetFileNameByType(APPLIB_DCF_MEDIA_TYPE_e mediaType, UINT32 Type, UINT32 objId, char *filename);

/**
 *  Get the position of first directory
 *
 *  @param [in] mediaType Media type
 *  @param [in] Type DCF handler
 *
 *  @return >0 The position of first directory, otherwise failure
 */
extern int AppLibStorageDmf_GetFirstDirPos(APPLIB_DCF_MEDIA_TYPE_e mediaType, UINT32 Type);

/**
 *  Get the position of last directory
 *
 *  @param [in] mediaType Media type
 *  @param [in] Type DCF handler
 *
 *  @return >0 The position of last directory, otherwise failure
 */
extern int AppLibStorageDmf_GetLastDirPos(APPLIB_DCF_MEDIA_TYPE_e mediaType, UINT32 Type);

/**
 *  Get the position of next directory
 *
 *  @param [in] mediaType Media type
 *  @param [in] Type DCF handler
 *
 *  @return >0 The position of next directory, otherwise failure
 */
extern int AppLibStorageDmf_GetNextDirPos(APPLIB_DCF_MEDIA_TYPE_e mediaType, UINT32 Type);

/**
 *  Get the position of previous directory
 *
 *  @param [in] mediaType Media type
 *  @param [in] Type DCF handler
 *
 *  @return >0 The position of previous directory, otherwise failure
 */
extern int AppLibStorageDmf_GetPrevDirPos(APPLIB_DCF_MEDIA_TYPE_e mediaType, UINT32 Type);

/**
 *  Get the directory list
 *
 *  @param [in] mediaType Media type
 *  @param [in] list Directory list
 *  @param [in] Type DCF handler
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibStorageDmf_GetDirList(APPLIB_DCF_MEDIA_TYPE_e mediaType, AMP_DCF_DIR_LIST_s *list, UINT32 Type);

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
extern int AppLibStorageDmf_CreateFile(APPLIB_DCF_MEDIA_TYPE_e mediaType, char *extName, UINT32 Type, char *filename);

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
extern int AppLibStorageDmf_CreateFileByType(APPLIB_DCF_MEDIA_TYPE_e mediaType, UINT32 Type, char *extName, char *filename);

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
extern int AppLibStorageDmf_CreateFileExtended(APPLIB_DCF_MEDIA_TYPE_e mediaType, UINT32 objId, char *extName, UINT8 extType, UINT8 seqNum, UINT32 Type, char *filename);

/**
 *  Delete file in the table
 *
 *  @param [in] mediaType Media type
 *  @param [in] objId Object id
 *  @param [in] Type DCF handler
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibStorageDmf_DeleteFile(APPLIB_DCF_MEDIA_TYPE_e mediaType, UINT32 objId, UINT32 Type);

/**
 *  Get file amount
 *
 *  @param [in] mediaType Media type
 *  @param [in] Type DCF handler
 *
 *  @return File amount
 */
extern int AppLibStorageDmf_GetFileAmount(APPLIB_DCF_MEDIA_TYPE_e mediaType, UINT32 Type);

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
extern int AppLibStorageDmf_SetFileNumberMode(APPLIB_DCF_MEDIA_TYPE_e mediaType, APPLIB_DCF_NUMBER_MODE_e NumberMode, UINT32 Type);

__END_C_PROTO__
/**
 * @}
 */
#endif /* APPLIB_STORAGE_DMF_H_ */
