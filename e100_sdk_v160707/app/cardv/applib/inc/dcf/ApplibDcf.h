/**
 * @file src/app/connected/applib/inc/dcf/ApplibDcf.h
 *
 * Header of DCF Applib
 *
 * History:
 *    2013/11/06 - [YC Liao] created file
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

#ifndef APPLIB_DCF_H_
#define APPLIB_DCF_H_
/**
 * @defgroup DCF
 * @brief Applib DCF
 *
 * Implementation of
 * DCF related function
 *
 */

/**
* @defgroup ApplibDcf_Function
* @brief Function implementation
*
*
*/

/**
 * @addtogroup ApplibDcf_Function
 * @ingroup DCF
 * @{
 */
#include <wchar.h>
#include <cfs/AmpCfs.h>
#include <dcf/AmpDcf.h>

/**
 * Constants and structures define
 **/
#define APPLIB_DCF_INTERNAL_OBJECT_AMOUNT   100           /**<APPLIB_DCF_INTERNAL_OBJECT_AMOUNT   100  */
#define APPLIB_DCF_MEDIA_TYPE_AMOUNT        3             /**<APPLIB_DCF_MEDIA_TYPE_AMOUNT        3    */
#define APPLIB_DCF_MAX_ROOT                 2             /**<APPLIB_DCF_MAX_ROOT                 2    */

#define MAX_EXTNAME_LENGTH                  4            /**<MAX_EXTNAME_LENGTH                  4     */
#define APPLIB_DCF_MEDIA_IMAGE_EXT_NAME     "JPG"       /**<APPLIB_DCF_MEDIA_IMAGE_EXT_NAME     L"JPG"*/
#define APPLIB_DCF_MEDIA_AUDIO_EXT_NAME     "AAC"       /**<APPLIB_DCF_MEDIA_AUDIO_EXT_NAME     L"AAC"*/
#define APPLIB_DCF_MEDIA_VIDEO_EXT_NAME     "MP4"       /**<APPLIB_DCF_MEDIA_VIDEO_EXT_NAME     L"MP4"*/
/*!
 * Numbering modes
 */
typedef enum APPLIB_DCF_NUMBER_MODE_e_ {
    APPLIB_DCF_NUMBER_RESET = 0,/*!APPLIB_DCF_NUMBER_RESET */
    APPLIB_DCF_NUMBER_SERIAL /*!APPLIB_DCF_NUMBER_SERIAL */
} APPLIB_DCF_NUMBER_MODE_e;
/*!
 * Media types
 */
typedef enum APPLIB_DCF_MEDIA_TYPE_e_ {
    APPLIB_DCF_MEDIA_IMAGE = 0,/*!APPLIB_DCF_MEDIA_IMAGE */
    APPLIB_DCF_MEDIA_AUDIO,/*!APPLIB_DCF_MEDIA_AUDIO */
    APPLIB_DCF_MEDIA_VIDEO,/*!APPLIB_DCF_MEDIA_VIDEO */
    APPLIB_DCF_MEDIA_DCIM/*!APPLIB_DCF_MEDIA_DCIM */
} APPLIB_DCF_MEDIA_TYPE_e;
/*!
 * Extended object types
 */
typedef enum APPLIB_DCF_EXT_OBJECT_TYPE_e_ {
    APPLIB_DCF_EXT_OBJECT_IMAGE_THM = 0,/*!APPLIB_DCF_EXT_OBJECT_IMAGE_THM */
    APPLIB_DCF_EXT_OBJECT_VIDEO_THM,/*!APPLIB_DCF_EXT_OBJECT_VIDEO_THM */
    APPLIB_DCF_EXT_OBJECT_SPLIT_FILE,/*!APPLIB_DCF_EXT_OBJECT_SPLIT_FILE */
    APPLIB_DCF_EXT_OBJECT_SPLIT_THM/*!APPLIB_DCF_EXT_OBJECT_SPLIT_THM */
} APPLIB_DCF_EXT_OBJECT_TYPE_e;
/*!
 * Media structure: for storing extension name for a media type
 */
typedef struct _APPLIB_DCF_MEDIA_s_ {
    UINT8 MediaType; /**< MediaType */
    char ExtName[MAX_EXTNAME_LENGTH];/**< MediaType Extend name*/
} APPLIB_DCF_MEDIA_s;
/**
 * Media List: for storing extension names of all media types
 *
 */
typedef struct APPLIB_DCF_MEDIA_LIST_s_ {
    UINT32 Count;/**< Media List Count */
    APPLIB_DCF_MEDIA_s List[APPLIB_DCF_MEDIA_TYPE_AMOUNT];/**< Media List */
} APPLIB_DCF_MEDIA_LIST_s;
/**
 * Root structure: for storing root name
 */
typedef struct _APPLIB_DCF_ROOT_s_ {
    char Name[MAX_FILENAME_LENGTH];/**< DCF Root Name */
} APPLIB_DCF_ROOT_s;
/**
 * Root List: for storing all root names
 */
typedef struct APPLIB_DCF_ROOT_LIST_s_ {
    UINT32 Count;/**< Root List Count */
    APPLIB_DCF_ROOT_s List[APPLIB_DCF_MAX_ROOT];/**< Root List */
} APPLIB_DCF_ROOT_LIST_s;
/**
 * Naming structure: for storing naming related function pointers
 */
typedef struct {
    BOOL (*IsIdValid)(AMP_DCF_FILTER_s *, UINT32);/**< Valid Flag */
    int (*GetObjectName)(AMP_DCF_FILTER_s *, UINT32, const char *, const char *, char *);/**< Get Object Name CB Function */
    int (*GetExtObjectName)(AMP_DCF_FILTER_s *, UINT32, const char *, UINT8, UINT8, const char *, char *);/**< GetExtend Object Name CB Function */
} APPLIB_DCF_NAMING_s;
/**
 * Configuration parameters for initialize APPLIB DCF
 */
typedef struct APPLIB_DCF_INIT_CFG_s_ {
    AMP_CFS_CFG_s CfsCfg;                               /**< Cfs config */
    AMP_DCF_INIT_CFG_s DcfInitCfg;                      /**< Dcf init config */
    APPLIB_DCF_MEDIA_LIST_s MediaTypes;                 /**< List of available Media types */
    UINT8 *Buffer;                                      /**< Point to the buffer */
    UINT32 BufferSize;                                  /**< the size of the buffer */
    UINT32 IntObjAmount;                                /**< amount of internal objects (created but not really exist in filesystem) */
} APPLIB_DCF_INIT_CFG_s;
/**
 * Configuration parameters for creating APPLIB DCF handlers
 */
typedef struct APPLIB_DCF_CFG_s_ {
    APPLIB_DCF_ROOT_LIST_s RootList;                    /**< Root List */
    UINT8  NamingRule;                                   /**< Naming Rule */
    UINT8  BrowseMode;                                  /**< Browse mode */
    UINT8  NumberMode;                                  /**< Mode of assigning a new ID */
    UINT32 PhotoLastIdx;    /**< last recorded photo object id*/
    UINT32 SoundLastIdx;    /**< last recorded sound object id*/
    UINT32 VideoLastIdx;    /**< last recorded video object id*/
    UINT32 MixLastIdx;    /**< last mix recorded object id*/
} APPLIB_DCF_CFG_s;
/**
 * Applib DCF handler
 */
typedef struct APPLIB_DCF_HDLR_s_ {
    APPLIB_DCF_ROOT_LIST_s RootList;                    /**< Root List */
    AMP_DCF_HDLR_s *DcfHdlr;                            /**< DCF Handler */
    APPLIB_DCF_NAMING_s *Naming;                        /**< DCF Handler */
    UINT32 ObjAmount[APPLIB_DCF_MEDIA_TYPE_AMOUNT+1];   /**< Object amount for every media */
    UINT32 LastInternalObjId;                           /**< Last internal object ID */
    UINT32 CurObjId;                                    /**< Current object id */
    UINT32 CurDnum;                                     /**< Current dir num */
    UINT8  NamingRule;                                  /**< Naming rule */
    UINT8  BrowseMode;                                  /**< Browse mode */
    UINT8  NumberMode;                                  /**< Mode of assigning a new ID */
    UINT8  Active;                                      /**< Active status */
} APPLIB_DCF_HDLR_s;

/**
 * DCF APIs
 **/

/**
 *  Get required buffer size for Applib DCF.
 *  @param [in] CfsCfg address of AppLibDCF config data
 *  @param [in] DcfInitCfg address of AppLibDCF init config data
 *  @param [in] IntObjAmount Internal object amount
 *  @return buffer size
 */
UINT32 AppLibDCF_GetRequiredBufSize(AMP_CFS_CFG_s *CfsCfg, AMP_DCF_INIT_CFG_s *DcfInitCfg, UINT32 IntObjAmount);

/**
 *  Get default config for initialize AppLib DCF.
 *  @param [out] InitConfig address of a AppLibDCF init config data
 */
extern void AppLibDCF_GetDefaultInitCfg(APPLIB_DCF_INIT_CFG_s *InitConfig);
/**
 *  Initialize AppLib DCF
 *  @param [in] InitConfig address of a Applib DCF init config data
 *  @return >=0 success, <0 failure
 */
extern int AppLibDCF_Init(APPLIB_DCF_INIT_CFG_s *InitConfig);

/**
 *  Set default config data for creating a DCF handler
 *  @param [in] Config address of default DCF config data
 */
extern void AppLibDCF_SetDefaultCfg(APPLIB_DCF_CFG_s *Config);

/**
 *  Get default config data for creating a DCF handler
 *  @param [in] Config address of a DCF config data
 */
extern void AppLibDCF_GetDefaultCfg(APPLIB_DCF_CFG_s *Config);
/**
 *  Create a DCF handler
 *  @param [in] Config address of a DCF config data
 *  @return !=NULL Handler, == NULL failure
 */
extern APPLIB_DCF_HDLR_s *AppLibDCF_Create(APPLIB_DCF_CFG_s *Config);
/**
 *  Goto first Object and return Object id
 *  @param [in] Hdlr Applib DCF Handler
 *  @return >=0 object id, <0 failure
 */
extern int AppLibDCF_FirstObject(APPLIB_DCF_HDLR_s *Hdlr);
/**
 *  Goto last Object and return Object id
 *  @param [in] Hdlr Applib DCF Handler
 *  @return >=0 object id, <0 failure
 */
extern int AppLibDCF_LastObject(APPLIB_DCF_HDLR_s *Hdlr);
/**
 *  Goto next Object and return Object id
 *  @param [in] Hdlr Applib DCF Handler
 *  @return >=0 object id, <0 failure
 */
extern int AppLibDCF_NextObject(APPLIB_DCF_HDLR_s *Hdlr);
/**
 *  Goto previous Object and return Object id
 *  @param [in] Hdlr Applib DCF Handler
 *  @return >=0 object id, <0 failure
 */
extern int AppLibDCF_PrevObject(APPLIB_DCF_HDLR_s *Hdlr);
/**
 *  Get current object Id
 *  @param [in] Hdlr Applib DCF Handler
 *  return object Id
 */
extern UINT32 AppLibDCF_GetCurrentObjectId(APPLIB_DCF_HDLR_s *Hdlr);
/**
 *  Get file list of current Object
 *  @param [in] Hdlr Applib DCF Handler
 *  @param [in] ObjId Object Id
 *  @return address of file list
 */
extern AMP_DCF_FILE_LIST_s *AppLibDCF_GetFileList(APPLIB_DCF_HDLR_s *Hdlr, UINT32 ObjId);
/**
 *  Release file list
 *  @param [in] Hdlr Applib DCF Handler
 *  @param [in] List address of file list
 *  @return >=0 success, <0 failure
 */
extern int AppLibDCF_RelFileList(APPLIB_DCF_HDLR_s *Hdlr, AMP_DCF_FILE_LIST_s *List);
/**
 *  Goto first directory and return dnum
 *  @param [in] Hdlr Applib DCF Handler
 *  @return >=0 dir number, <0 failure
 */
extern int AppLibDCF_FirstDir(APPLIB_DCF_HDLR_s *Hdlr);
/**
 *  Goto last directory and return dnum
 *  @param [in] Hdlr Applib DCF Handler
 *  @return >=0 dir number, <0 failure
 */
extern int AppLibDCF_LastDir(APPLIB_DCF_HDLR_s *Hdlr);
/**
 *  Goto next directory and return dnum
 *  @param [in] Hdlr Applib DCF Handler
 *  @return >=0 dir number, <0 failure
 */
extern int AppLibDCF_NextDir(APPLIB_DCF_HDLR_s *Hdlr);
/**
 *  Goto previous directory and return dnum
 *  @param [in] Hdlr Applib DCF Handler
 *  @return >=0 dir number, <0 failure
 */
extern int AppLibDCF_PrevDir(APPLIB_DCF_HDLR_s *Hdlr);
/**
 *  Get dir list of current dnum
 *  @param [in] Hdlr Applib DCF Handler
 *  @return dir list of current dnum
 */
extern AMP_DCF_DIR_LIST_s *AppLibDCF_GetDirList(APPLIB_DCF_HDLR_s *Hdlr);
/**
 *  Release dir list
 *  @param [in] Hdlr Applib DCF Handler
 *  @param [in] List address of dir list
 *  @return >=0 success, <0 failure
 */
extern int AppLibDCF_RelDirList(APPLIB_DCF_HDLR_s *Hdlr, AMP_DCF_DIR_LIST_s *List);
/**
 *  Create an object and return a name of the new object
 *  @param [in]  Hdlr Applib DCF handler
 *  @param [in]  RootName Rootname
 *  @param [in]  ExtName extension name
 *  @param [out] ObjName address of object name
 *  @return >0 object id, <=0 failure
 */
extern int AppLibDCF_CreateObject(APPLIB_DCF_HDLR_s *Hdlr, const char *RootName, const char *ExtName, char *ObjName);
/**
 *  Create an extended object
 *  @param [in]  Hdlr Applib DCF handler
 *  @param [in]  ObjId object id
 *  @param [in]  RootName Rootname
 *  @param [in]  ExtType Extended Object type
 *  @param [in]  SeqNum Sequence number of split file
 *  @param [in]  ExtName extension name
 *  @param [out] ObjName address of object name
 *  @return >=0 success, <0 failure
 */
extern int AppLibDCF_CreateExtendedObject(APPLIB_DCF_HDLR_s *Hdlr, UINT32 ObjId, const char *RootName, UINT8 ExtType, UINT8 SeqNum, const char *ExtName, char *ObjName);
/**
 *  Delete object files
 *  @param [in] Hdlr Applib DCF handler
 *  @param [in] ObjId object id
 *  @return >=0 success, <0 failure
 */
extern int AppLibDCF_DeleteObject(APPLIB_DCF_HDLR_s *Hdlr, UINT32 ObjId);
/**
 *  Translate object name to id
 *  @param [in] Hdlr Applib DCF handler
 *  @param [in] ObjName object name
 *  @return >0 success, =0 failure
 */
extern UINT32 AppLibDCF_NameToId(APPLIB_DCF_HDLR_s *Hdlr, char *ObjName);
/**
 *  Get object amount, internal objects are not included
 *  @param [in] Hdlr Applib DCF handler
 *  @return object amount
 */
extern int AppLibDCF_GetObjectAmount(APPLIB_DCF_HDLR_s *Hdlr);
/**
 *  Get media type of an object
 *  @param [in] Name filename
 *  @return MediaType
 */
extern int AppLibDCF_GetMediaType(char *Name);
/**
 *  Set browse mode
 *  @param [in] Hdlr Applib DCF Handler
 *  @param [in] MediaType Media Type
 *  return object Id
 */
extern UINT32 AppLibDCF_SetBrowseMode(APPLIB_DCF_HDLR_s *Hdlr, APPLIB_DCF_MEDIA_TYPE_e MediaType);

/**
 *  Get browse mode
 *  @param [in] Hdlr Applib DCF Handler
 *  @return browse mode
 */
extern int AppLibDCF_GetBrowseMode(APPLIB_DCF_HDLR_s *Hdlr);

/**
 *  Get number mode
 *  @param [in] Hdlr Applib DCF Handler
 *  @return number mode
 */
extern int AppLibDCF_GetNumberMode(APPLIB_DCF_HDLR_s *Hdlr);
/**
 *  Set number mode
 *  @param [in] Hdlr Applib DCF Handler
 *  @param [in] NumberMode number mode
 *  @param [in] ObjId object id of last object
 *  @return >=0 success, <0 failure
 */
extern int AppLibDCF_SetNumberMode(APPLIB_DCF_HDLR_s *Hdlr, APPLIB_DCF_NUMBER_MODE_e NumberMode, UINT ObjId);
/**
 *  Delete a handler
 *  @param [in] Hdlr Applib DCF handler
 *  @return >=0 success, <0 failure
 */
extern int AppLibDCF_Delete(APPLIB_DCF_HDLR_s *Hdlr);
/**
 *  Refresh DCF & CFS
 *  @param [in] Hdlr APPLIB DCF handler
 *  @return >=0 Object Id, <0 failure
 */
extern int AppLibDCF_Refresh(APPLIB_DCF_HDLR_s *Hdlr);

/**
 * Debug
 */

/**
 *  List internal objects
 */
extern void AppLibDCF_ListInternalObject(void);
/**
 * @}
 */
#endif /* APPLIB_DCF_H_ */
