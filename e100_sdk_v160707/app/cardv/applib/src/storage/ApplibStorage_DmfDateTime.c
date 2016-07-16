/**
 * @file src/app/connected/applib/src/storage/ApplibStorage_DmfDateTime.c
 *
 * Implementation of storage DMF(Digital Media File System) utility
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

#include <applib.h>
#include <dcf/ApplibDcf_FilterDateTime.h>

//#define DEBUG_APPLIB_STORAGE_DMF_DATE_TIME
#if defined(DEBUG_APPLIB_STORAGE_DMF_DATE_TIME)
    #define DBGMSG AmbaPrint
    #define DBGMSGc(x) AmbaPrintColor(GREEN,x)
    #define DBGMSGc2 AmbaPrintColor
#else
    #define DBGMSG(...)
    #define DBGMSGc(...)
    #define DBGMSGc2(...)
#endif

#ifdef CONFIG_APP_ARD
int empty_dir_count = 0;
char empty_dir_group[20][MAX_FILENAME_LENGTH];
#endif

/*************************************************************************
 * digital media file system definitons
 ************************************************************************/
static APPLIB_STORAGE_DMF_s ApplibStorageDmfSetting = {0};

/*************************************************************************
 * File system and DMF APIs
 ************************************************************************/

/**
 *  @brief Initialize the default DCF
 *
 *  Initialize the default DCF
 *
 *  @return >=0 success, <0 failure
 */
static int AppLibStorageDmf_InitDcfDefault(void)
{
    int ReturnValue = 0;
#if 0
    int slot = 0;
#ifdef VALID_SLOT_IN_DMF
    int configed_amount = 0;
#endif
    dmf_config_t *dmf_config;
    MCHAR p_dir[] = {'M','I','S','C','\0'};
    MCHAR p_pfx[] = {'P','L','S','T','\0'};
    MCHAR p_ext[] = {'p','l','s','\0'};
    /**
     * Files will be put into DCIM\MEDIA folder with same filename
     * prefix.
     **/
    MCHAR dir[] = {'D','C','I','M','\0'};
    MCHAR pfx[] = {'M','E','D','I','A','\0'};
    MCHAR file[] = {'A','M','B','A','\0'};
    MCHAR ancilla_str[MAX_ANCILLA_STRLEN+1] = {'t','h','m','\0'};
    dcf_define_t dcf_define;

    dcf_define.root = NULL;
    dcf_define.dcim = dir;
    dcf_define.dir = pfx;
    dcf_define.file = file;
    dcf_define.ancilla_str = ancilla_str;

    DBGMSGc2(BLUE, "[DemoLib - Storage] <storage_init_dcf_type2> start");

    /** configure DMF */
    dmf_config = (dmf_config_t *)&dcf_define;

#ifdef VALID_SLOT_IN_DMF
    /** for each drive specified in VALID_SLOT_IN_DMF */
    for (slot=0; slot<SCRDMGR_SLOT_RF; slot++) {
        if (APP_CHECKFLAGS(VALID_SLOT_IN_DMF, (0x1<<slot))) {
            ReturnValue = storage_create_dmf(ST_DCF, DMF_DCIM_ROOT, slot+'A', dmf_config);
            if ((++configed_amount)>=5) {
                AmbaPrintColor(BLUE,"Reach Max_drive_count of DMF, last connfiged drive is %c",slot+'A');
                break;
            }
        }
    }
#else
    /** for drive A */
    ReturnValue = storage_create_dmf(ST_DCF, DMF_DCIM_ROOT, 'A', dmf_config);
    /** for drive C */
    ReturnValue = storage_create_dmf(ST_DCF, DMF_DCIM_ROOT, 'C', dmf_config);
    /** for drive D */
    ReturnValue = storage_create_dmf(ST_DCF, DMF_DCIM_ROOT, 'D', dmf_config);

    if (CHIP_REV == A2S || CHIP_REV == A2M || CHIP_REV == A5 || CHIP_REV == A5S || CHIP_REV == A7 || CHIP_REV == A5L || (CHIP_REV == I1)) {
        ReturnValue = storage_create_dmf(ST_DCF, DMF_DCIM_ROOT, 'L', dmf_config);
    } else {
        ReturnValue = storage_create_dmf(ST_DCF, DMF_DCIM_ROOT, 'E', dmf_config);
    }
    /** for drive I */
    ReturnValue = storage_create_dmf(ST_DCF, DMF_DCIM_ROOT, 'I', dmf_config);
#endif

    /** do DMF refresh after config done */
    slot = app_sys_get_active_slot();
    if (slot != -1) {
        MCHAR act[3] = {0};

        act[0] = slot +'A';
        act[1] = ':';
        act[2] = '\0';
        ReturnValue = app_dmf_refresh(act);
    }

    /** configure Playlist */
    ReturnValue = storage_config_plist(PLIST_TEXT, p_dir, p_pfx, p_ext, 4, 0);

#if defined(ENABLE_DPOF)
    /** config DPOF to enable auto print */
    ReturnValue = storage_config_dpof(1, 0, 0, NULL);
#endif

    DBGMSGc2(BLUE, "[AppLib - DMF] <InitDcfDefault> end: ReturnValue = %d", ReturnValue);
#endif
    return ReturnValue;
}

/**
 *  @brief Setup the file system
 *
 *  Setup the file system
 *
 *  @return >=0 success, <0 failure
 */
int AppLibStorageDmf_SetupFileSystem(void)
{
    int ReturnValue = 0;
#if 0
    dmf_dcf_config_t dcf_conf;

    /* get current dmf settings */
        ReturnValue = app_dmf_get_dcf_config(&dcf_conf);
        if (ReturnValue<0) {
            AmbaPrint("Fail to app_dmf_get_dcf_config (%d)\n",ReturnValue);
            return -1;
        }


        /*
         * In A7lSDK540 DemoApp, DMF_DUPLI_FORBIT_ALL is always on.
         * That is, no same dir/file index are allowed. (Both ignore)
         */
        dcf_conf.dupli_forbid = DMF_DUPLI_FORBID_ALL;

    /* prepare new dmf settings */
        //set minimal usable folder as 101XXXXX.
        dcf_conf.Min_dir_num=101;
        //set maximal file amount of each folder to 9999.
        dcf_conf.Max_file_num=9999;

        //Keep empty dir to avoid create next folder after remove empty dir
        dcf_conf.flags = dcf_conf.flags&(~(DMF_CFG_DEL_EMPTY_DIR));

        dcf_conf.flags = dcf_conf.flags|DMF_CFG_CHECK_POSTFIX|DMF_CFG_MK_PATH_LATER| \
                 DMF_CFG_FORCE_DIDX|DMF_CFG_SYNC_FATR|DMF_CFG_USE_EMPTY_DIR| \
                 DMF_CFG_SHARE_LAST|DMF_CFG_TIME_SORT;

        dcf_conf.date_sort_drive = 'L';
        dcf_conf.sort_table_rule = DMF_REUSE_SORT_TABLE;

        //not re-use fid in SONY rule
        dcf_conf.reuse_fid = 0;

        //do not create video file into folders with different postfix.
        dcf_conf.dir_occupy_flag = dcf_conf.dir_occupy_flag&(~(DMF_DIR_OCCUPY_VIDEO_ROOT));
        //create image file into folders with different postfix.
        dcf_conf.dir_occupy_flag = dcf_conf.dir_occupy_flag|DMF_DIR_OCCUPY_IMAGE_ROOT;

        //force to delete the same fnum file on image and video root
        dcf_conf.force_del_flag = dcf_conf.force_del_flag|DMF_FORCE_DEL_IMAGE_ROOT;
        dcf_conf.force_del_flag = dcf_conf.force_del_flag|DMF_FORCE_DEL_VIDEO_ROOT;

        //set split file digitals as 1
        dcf_conf.split_digitals = 1;

        /* setting duplicated directory forbidden */
        dcf_conf.dupli_forbid = DMF_DUPLI_FORBID_DIR;

        /* setting invalid filename forbidden */
        dcf_conf.invalid_forbid = DMF_INVALID_FORBID_FILE;

        //file attr setting
        dcf_conf.file_attr_rule |= DMF_FILE_ATTR_EMPTY_SIZE;

        //enable Serial mode
        if (ApplibStorageDmfSetting.Mode == DMF_MODE_SERIAL) {
            dcf_conf.flags= dcf_conf.flags | (DMF_CFG_KEEP_FIDX);
        }

    /* config dmf with new settings */
        ReturnValue = app_dmf_set_dcf_config(&dcf_conf);
        if (ReturnValue<0) {
            AmbaPrint("Fail to app_dmf_set_dcf_config (%d)\n",ReturnValue);
            return -1;
        }

    /* config file prefix filter */
        /* set the prefix list to allow MAQ0, MAH0 and MA30 for play back */
        /* prefix index should follow the video_res_mode definition */
        {
            MCHAR filter_items[3][5]={{'M','A','Q','\0'},{'M','A','H','\0'},{'M','A','3','\0'}};
            dmf_dcf_prefix_filter_t p_filter={0};

            for (ReturnValue=0;ReturnValue<3;ReturnValue++) {
                mstrcpy(p_filter.filter_list[ReturnValue],filter_items[ReturnValue]);
            }

            if (app_dmf_set_prefix_filter(DMF_VIDEO_ROOT,&p_filter,3)<0) {
                AmbaPrintColor(RED,"Fail to set prefix filter!!");
            }
        }
#endif
    DBGMSGc2(YELLOW, "==[%s-%d] ReturnValue:%d", __FUNCTION__, __LINE__, ReturnValue);
    return ReturnValue;
}


/**
 *  @brief DMF module initialization
 *
 *  DMF module initialization
 *
 *  @param [in] dcfType DCF type
 *
 *  @return >=0 success, <0 failure
 */
int AppLibStorageDmf_Init(int dcfType)
{
    int ReturnValue = 0;
    DBGMSGc2(YELLOW, "==[%s-%d] ReturnValue:%d", __FUNCTION__, __LINE__, ReturnValue);
    ReturnValue = AppLibStorageDmf_SetupFileSystem();
    switch (dcfType) {
    case STORAGE_DCF_TYPE_DEFAULT:
    default:
        ReturnValue = AppLibStorageDmf_InitDcfDefault();
        break;
    }
    DBGMSGc2(YELLOW, "==[%s-%d] ReturnValue:%d", __FUNCTION__, __LINE__, ReturnValue);
    return ReturnValue;
}

/**
 *  @brief Get DMF module setting.
 *
 *  DMF module setting.
 *
 *  @param [out] setting DMF module setting.
 *
 *  @return >=0 success, <0 failure
 */
int AppLibStorageDmf_GetSetting(APPLIB_STORAGE_DMF_s *setting)
{
    DBGMSGc2(YELLOW, "==[%s-%d]", __FUNCTION__, __LINE__);
    memcpy(setting, &ApplibStorageDmfSetting, sizeof(APPLIB_STORAGE_DMF_s));
    return 0;
}

/**
 *  @brief Setup the DMF mode
 *
 *  Setup the DMF mode
 *
 *  @return >=0 success, <0 failure
 */
int AppLibStorageDmf_SetupMode(void)
{
    int ReturnValue = 0;
#if 0
    dmf_dcf_config_t dcf_conf;

    /** get current dmf config */
    ReturnValue = app_dmf_get_dcf_config(&dcf_conf);

    if (ReturnValue < 0) {
        AmbaPrint("1_Fail to MW_DMF_GET_CONFIG (%d)\n",ReturnValue);
        return -1;
    }


    /*
    * In A7lSDK540 DemoApp, DMF_DUPLI_FORBIT_ALL is always on.
    * That is, no same dir/file index are allowed. (Both ignore)
    */
    dcf_conf.dupli_forbid = DMF_DUPLI_FORBID_ALL;

    switch (ApplibStorageDmfSetting.Mode) {
    case DMF_MODE_RESET:
        /** set new para into dmf config */
        dcf_conf.flags = dcf_conf.flags & (~(DMF_CFG_KEEP_FIDX));
        ReturnValue = app_dmf_set_dcf_config(&dcf_conf);
        if (ReturnValue < 0) {
            AmbaPrint("2_Fail to MW_DMF_SET_CONFIG (%d)\n",ReturnValue);
            return -1;
        }
        break;
    case DMF_MODE_SERIAL:
    /** set new para into dmf config */
        dcf_conf.flags = dcf_conf.flags | (DMF_CFG_KEEP_FIDX);
        ReturnValue = app_dmf_set_dcf_config(&dcf_conf);
        if (ReturnValue < 0) {
            AmbaPrint("3_Fail to MW_DMF_SET_CONFIG (%d)\n",ReturnValue);
            return -1;
        }
        break;
    }
#endif
    DBGMSGc2(YELLOW, "==[%s-%d] ReturnValue:%d", __FUNCTION__, __LINE__, ReturnValue);
    return ReturnValue;
}

static APPLIB_DCF_HDLR_s *AppLibStorageDmfHdlr[STORAGE_DCF_HDLR_MAX] = {NULL};
static UINT32 AppLibStorageCurrentObj[4] = {0};
static INT32 AppLibStorageCurrentObjIdx[4] = {0};
static char *RootName[STORAGE_DCF_HDLR_MAX] = {NULL};
static char RootNameDef[64] = "C:\\DCIM";
static char RootNameEmergencyDef[16] = "C:\\EVENT";
static BOOL RefreshStatus = FALSE;

char g_AppLibStorageRootName[STORAGE_DCF_HDLR_MAX][32] = {'\0'};

#define MAIN_FILENAME_FILTER_STR    "A.MP4"     // so stupid
#define SEC_FILENAME_FILTER_STR     "B.MP4"     // so stupid
static char MovieExtendFile[5] = {'.','M','P','4','\0'};
static char PhotoExtendFile[5] = {'.','J','P','G','\0'};
#ifdef CONFIG_APP_ARD
static char SoundExtendFile[5] = {'.','A','A','C','\0'};
#endif

static int AppLibStorageDmf_CheckNaming(char *comparedName, char *extName, UINT32 StreamType)
{
  char *p = NULL;
    DBGMSGc2(YELLOW, "==[%s-%d] compareName:%s, extName :%s, StreamType: %d", __FUNCTION__, __LINE__, comparedName, extName, StreamType);
  p = strrchr(comparedName, '.');

  /* To check extend filename. */
  if (strcmp(p, extName) < 0) {
      return -1;
  }

  //AmbaPrint("compared fn: %s, StreamType %d", comparedName, StreamType);
  /* To check movie file. */
  if (strcmp(extName, MovieExtendFile) == 0) {
      if (StreamType == APPLIB_DCF_EXT_OBJECT_SPLIT_FILE) {  // main stream
          if (strstr(comparedName, MAIN_FILENAME_FILTER_STR) == NULL) {
                DBGMSGc2(YELLOW, "##[%s-%d] check fail", __FUNCTION__, __LINE__);
              return -1;
          }
      } else if (StreamType == APPLIB_DCF_EXT_OBJECT_SPLIT_THM) {   // 2nd stream
          if (strstr(comparedName, SEC_FILENAME_FILTER_STR) == NULL) {
                DBGMSGc2(YELLOW, "##[%s-%d] check fail", __FUNCTION__, __LINE__);
              return -1;
          }
      }
  } else if (strcmp(extName, PhotoExtendFile) == 0) {   /* To check photo file. */
      if (StreamType != APPLIB_DCF_EXT_OBJECT_SPLIT_FILE) {
            DBGMSGc2(YELLOW, "##[%s-%d] check fail", __FUNCTION__, __LINE__);
          return -1;
      }
#ifdef CONFIG_APP_ARD
    } else if (strcmp(extName, SoundExtendFile) == 0) {   /* To check sound file. */
        if (StreamType != APPLIB_DCF_EXT_OBJECT_SPLIT_FILE) {
            return -1;
        }
#endif
  } else {
      AmbaPrintColor(RED, "<AppLibStorageDmf_CheckNaming> neither movie nor photo.");
      return -1;
  }
    DBGMSGc2(YELLOW, "##[%s-%d] check match", __FUNCTION__, __LINE__);
  return 0;
}

/**
 *  @brief Create a DCF handler
 *
 *  Create a DCF handler
 *
 *  @return >=0 success, <0 failure
 */
static int AppLibStorageDmf_CreateHandler(char drive)
{
    int ReturnValue = 0;
    int i = 0;
    APPLIB_DCF_CFG_s DcfConfig;
    DBGMSGc2(YELLOW, "==[%s-%d]", __FUNCTION__, __LINE__);
    RootNameDef[0] = drive;
    //RootNameEmergencyDef[0] = drive;
    AmbaPrintColor(GREEN,"[AppLibStorageDmf_CreateHandler]sizeof rootname : %d", sizeof(RootName));
    RootName[0] = RootNameDef;
    strcpy(g_AppLibStorageRootName[0], RootNameDef);
#if defined(STORAGE_EMERGENCY_DCF_ON)
    RootNameEmergencyDef[0] = drive;
    RootName[1]= RootNameEmergencyDef;
    strcpy(g_AppLibStorageRootName[1], RootNameEmergencyDef);
#endif

    AppLibDCF_GetDefaultCfg(&DcfConfig);
    for (i=0;i<STORAGE_DCF_HDLR_MAX;i++) {
        strcpy(DcfConfig.RootList.List[0].Name, RootName[i]);
        AppLibStorageDmfHdlr[i] = AppLibDCF_Create(&DcfConfig);
        DBGMSGc2(YELLOW, "==[%s-%d] RootName[%d]:%s", __FUNCTION__, __LINE__,i, RootName[i]);
    }

    for (i=0;i<STORAGE_DCF_HDLR_MAX;i++) {
        if (AppLibStorageDmfHdlr[i] == NULL) {
            ReturnValue = -1;
            break;
        }
    }
    DBGMSGc2(YELLOW, "==[%s-%d] -END- ReturnValue:%d", __FUNCTION__, __LINE__, ReturnValue);
    return ReturnValue;
}

/**
 *  @brief Refresh the DCF handler
 *
 *  Refresh the DCF handler
 *
 *  @param [in] drive Drive id
 *
 *  @return >=0 success, <0 failure
 */
int AppLibStorageDmf_Refresh(char drive)
{
    int ReturnValue = 0, i = 0;
    DBGMSGc2(YELLOW, "==[%s-%d]", __FUNCTION__, __LINE__);
    AppLibComSvcHcmgr_SendMsgNoWait(HMSG_STORAGE_BUSY, drive, 0);
    if (AppLibStorageDmfHdlr[0] != NULL && drive != RootNameDef[0]) {
        AppLibStorageDmf_DeleteHandler();
    }
    if (AppLibStorageDmfHdlr[0] == NULL) {
        AppLibStorageDmf_CreateHandler(drive);
    }
#ifdef CONFIG_APP_ARD
	empty_dir_count = 0;
#endif

    for (i=0;i<STORAGE_DCF_HDLR_MAX;i++) {
		// Clean all DirIndex & FileCount data before SCAN all directory
		AppLibStorageDmf_InitFileCountInTheDir(0, i);
		AppLibStorageDmf_InitRtcDirIndex(i);
		AppLibStorageDmf_InitDirIndex(i);

        ReturnValue = AppLibDCF_Refresh(AppLibStorageDmfHdlr[i]);
        if (ReturnValue < 0)
            break;
        /**Reset current dcf and obj id to first id to avoid dcf and dmf object mismatch*/
        ReturnValue = AppLibDCF_FirstObject(AppLibStorageDmfHdlr[i]);
        AppLibStorageCurrentObjIdx[i] =0;
    }
#ifdef CONFIG_APP_ARD
	for (i=0;i<empty_dir_count;i++)
		AmpCFS_Rmdir(empty_dir_group[i]);
#endif
    if ((ReturnValue == -1) || (ReturnValue == -2)) { /** card error or DMF fail */
        AmbaPrintColor(RED, "<AppLibStorageDmf_Refresh> Referesh DMF table Fail ReturnValue = %d", ReturnValue);
    }

    /* Update refresh status to TRUE when DMF has been refreshed.
           Force to re-scan dirctory when creating file. */
    AppLibStorageDmf_SetRefreshStatus(TRUE);

    AppLibComSvcHcmgr_SendMsgNoWait(HMSG_STORAGE_IDLE, drive, 0);
    DBGMSGc2(YELLOW, "==[%s-%d] ReturnValue:%d", __FUNCTION__, __LINE__, ReturnValue);
    return ReturnValue;
}

/**
 *  @brief Delete the DCF handler
 *
 *  Delete the DCF handler
 *
 *  @return >=0 success, <0 failure
 */
int AppLibStorageDmf_DeleteHandler(void)
{
    int ReturnValue = 0, i = 0;
    DBGMSGc2(YELLOW, "==[%s-%d] ReturnValue:%d", __FUNCTION__, __LINE__, ReturnValue);
    if (AppLibStorageDmfHdlr[0] == NULL) {
        AmbaPrintColor(RED,"[Applib DMF] The DCF handler has not be created.");
        return -1;
    }
    for (i=0;i<STORAGE_DCF_HDLR_MAX;i++) {
        ReturnValue = AppLibDCF_Delete(AppLibStorageDmfHdlr[i]);
        if (ReturnValue < 0)
            break;
        AppLibStorageDmfHdlr[i] = NULL;
    }
    DBGMSGc2(YELLOW, "==[%s-%d] ReturnValue:%d", __FUNCTION__, __LINE__, ReturnValue);
    return ReturnValue;
}

/**
 *  @brief Get the position of the current file
 *
 *  Get the position of the current file
 *
 *  @param [in] mediaType Media type
 *  @param [in] Type DCF handler
 *
 *  @return >0 File position, the others is failure
 */
UINT64 AppLibStorageDmf_GetCurrFilePos(APPLIB_DCF_MEDIA_TYPE_e mediaType, UINT8 Type)
{
    int ReturnValue = 0;
    DBGMSGc2(YELLOW, "==[%s-%d] ReturnValue:%d", __FUNCTION__, __LINE__, ReturnValue);
    if (AppLibStorageDmfHdlr[Type] == NULL) {
        AmbaPrintColor(RED,"[%s-%d] The DCF handler has not be created.", __FUNCTION__, __LINE__);
        return -1;
    }
    if (AppLibDCF_GetBrowseMode(AppLibStorageDmfHdlr[Type]) != mediaType) {
        AppLibDCF_SetBrowseMode(AppLibStorageDmfHdlr[Type], mediaType);
    }
    ReturnValue = AppLibDCF_GetCurrentObjectId(AppLibStorageDmfHdlr[Type]);
    if (ReturnValue == 0) {
        AmbaPrintColor(RED,"[%s-%d] The DCF handler has not be created.", __FUNCTION__, __LINE__);
    } else {
        AppLibStorageCurrentObj[mediaType] = ReturnValue;
//        AppLibStorageCurrentObjIdx[mediaType] = 0;
    }
    DBGMSGc2(YELLOW, "##[%s-%d] CurObjId: %d, CurObjIdIdx: %d, ReturnValue: %llu", __FUNCTION__, __LINE__, ReturnValue, AppLibStorageCurrentObjIdx[mediaType], (((UINT64)AppLibStorageCurrentObjIdx[mediaType]) << 32) | ((UINT64)AppLibStorageCurrentObj[mediaType]));
    return (((UINT64)AppLibStorageCurrentObjIdx[mediaType]) << 32) | ((UINT64)AppLibStorageCurrentObj[mediaType]);
}

/**
 *  @brief Get the position of the first file
 *
 *  Get the position of the first file
 *
 *  @param [in] mediaType Media type
 *  @param [in] Type DCF handler
 *
 *  @return >0 File position, the others is failure
 */
UINT64 AppLibStorageDmf_GetFirstFilePos(APPLIB_DCF_MEDIA_TYPE_e mediaType, UINT8 Type)
{
    int ReturnValue = 0;
    DBGMSGc2(YELLOW, "==[%s-%d] ReturnValue:%d", __FUNCTION__, __LINE__, ReturnValue);
    if (AppLibStorageDmfHdlr[Type] == NULL) {
        AmbaPrintColor(RED,"[%s-%d] The DCF handler has not be created.", __FUNCTION__, __LINE__);
        return -1;
    }
    if (AppLibDCF_GetBrowseMode(AppLibStorageDmfHdlr[Type]) != mediaType) {
        AppLibDCF_SetBrowseMode(AppLibStorageDmfHdlr[Type], mediaType);
    }
    ReturnValue = AppLibDCF_FirstObject(AppLibStorageDmfHdlr[Type]);
    if (ReturnValue > 0) {
        DBGMSGc2(YELLOW,"[%s-%d] FirstObjId = %d, CurObjIdIdx = 0", __FUNCTION__, __LINE__, ReturnValue);
        AppLibStorageCurrentObj[mediaType] = ReturnValue;
        AppLibStorageCurrentObjIdx[mediaType] = 0;
    } else {
        AmbaPrintColor(RED,"[%s-%d] do AppLibDCF_FirstObject() Fail", __FUNCTION__, __LINE__);
    }
    DBGMSGc2(YELLOW, "##[%s-%d] ReturnValue:%d", __FUNCTION__, __LINE__, ReturnValue);
    return (((UINT64)AppLibStorageCurrentObjIdx[mediaType]) << 32) | ((UINT64)AppLibStorageCurrentObj[mediaType]);
}

/**
 *  @brief Get the position of the last file
 *
 *  Get the position of the last file
 *
 *  @param [in] mediaType Media type
 *  @param [in] Type DCF handler
 *
 *  @return >0 File position, the others is failure
 */
UINT64 AppLibStorageDmf_GetLastFilePos(APPLIB_DCF_MEDIA_TYPE_e mediaType, UINT8 Type)
{
    int ReturnValue = 0;
    DBGMSGc2(YELLOW, "==[%s-%d] ReturnValue:%d", __FUNCTION__, __LINE__, ReturnValue);
    if (AppLibStorageDmfHdlr[Type] == NULL) {
        AmbaPrintColor(RED,"[%s-%d] The DCF handler has not be created.", __FUNCTION__, __LINE__);
        return -1;
    }
    if (AppLibDCF_GetBrowseMode(AppLibStorageDmfHdlr[Type]) != mediaType) {
        AppLibDCF_SetBrowseMode(AppLibStorageDmfHdlr[Type], mediaType);
    }
    ReturnValue =  AppLibDCF_LastObject(AppLibStorageDmfHdlr[Type]);
    if (ReturnValue > 0) {
        AMP_DCF_FILE_LIST_s *list = NULL;
        AppLibStorageCurrentObj[mediaType] = ReturnValue;
        list = AppLibDCF_GetFileList(AppLibStorageDmfHdlr[Type], AppLibStorageCurrentObj[mediaType]);
        if (list != NULL) {
            AppLibStorageCurrentObjIdx[mediaType] = (list->Count - 1);
            AppLibDCF_RelFileList(AppLibStorageDmfHdlr[Type], list);
            DBGMSGc2(YELLOW,"[%s-%d] LastObjId = %d, CurObjIdIdx = %d", __FUNCTION__, __LINE__, ReturnValue, AppLibStorageCurrentObjIdx[mediaType]);
        } else {
            AmbaPrintColor(RED,"[%s-%d] do AppLibDCF_GetFileList() Fail", __FUNCTION__, __LINE__);
        }
    } else {
        AmbaPrintColor(RED,"[%s-%d] do AppLibDCF_LastObject() Fail", __FUNCTION__, __LINE__);
    }
    DBGMSGc2(YELLOW, "##[%s-%d] ReturnValue:%llu", __FUNCTION__, __LINE__, (((UINT64)AppLibStorageCurrentObjIdx[mediaType]) << 32) | ((UINT64)AppLibStorageCurrentObj[mediaType]));
    return (((UINT64)AppLibStorageCurrentObjIdx[mediaType]) << 32) | ((UINT64)AppLibStorageCurrentObj[mediaType]);
}

/**
 *  @brief Get the position of the next file
 *
 *  Get the position of the next file
 *
 *  @param [in] mediaType Media type
 *  @param [in] Type DCF handler
 *
 *  @return >0 File position, the others is failure
 */
UINT64 AppLibStorageDmf_GetNextFilePos(APPLIB_DCF_MEDIA_TYPE_e mediaType, UINT8 Type)
{
    int ReturnValue = 0;
    AMP_DCF_FILE_LIST_s *list = NULL;
    DBGMSGc2(YELLOW, "==[%s-%d] ReturnValue:%d", __FUNCTION__, __LINE__, ReturnValue);
    if (AppLibStorageDmfHdlr[Type] == NULL) {
        AmbaPrintColor(RED,"[%s- %d] The DCF handler has not be created.", __FUNCTION__, __LINE__);
        return -1;
    }
    if (AppLibDCF_GetBrowseMode(AppLibStorageDmfHdlr[Type]) != mediaType) {
        AppLibDCF_SetBrowseMode(AppLibStorageDmfHdlr[Type], mediaType);
    }
    list = AppLibDCF_GetFileList(AppLibStorageDmfHdlr[Type], AppLibStorageCurrentObj[mediaType]);
    if (list != NULL) {
        // Current ObjId has 2 more files in this FileList
        if (AppLibStorageCurrentObjIdx[mediaType] < (list->Count - 1)) {
            // Current ObjIdIdx is less than the count of FileList, add 1 to current ObjIdIdx
            AppLibStorageCurrentObjIdx[mediaType]++;
            AppLibDCF_RelFileList(AppLibStorageDmfHdlr[Type], list);
            DBGMSGc2(YELLOW,"[%s- %d] NextObjId = %d, CurObjIdIdx = %d", __FUNCTION__, __LINE__, AppLibStorageCurrentObj[mediaType], AppLibStorageCurrentObjIdx[mediaType]);
        } else if (AppLibStorageCurrentObjIdx[mediaType] == (list->Count - 1)) {
            // Current ObjIdIdx is the same with the count of FileList, go to next ObjId and set current ObjIdIdx to 0
            AppLibDCF_RelFileList(AppLibStorageDmfHdlr[Type], list);
    ReturnValue =  AppLibDCF_NextObject(AppLibStorageDmfHdlr[Type]);
    if (ReturnValue > 0) {
        AppLibStorageCurrentObj[mediaType] = ReturnValue;
                AppLibStorageCurrentObjIdx[mediaType] = 0;
                DBGMSGc2(YELLOW,"[%s- %d] NextObjId = %d, CurObjIdIdx = 0", __FUNCTION__, __LINE__, ReturnValue);
            } else {
                AmbaPrintColor(RED,"[%s- %d] do AppLibDCF_NextObject Fail", __FUNCTION__, __LINE__);
            }
        } else {
            AmbaPrintColor(RED,"[%s- %d] It should not happen !!!!!!!!!!!", __FUNCTION__, __LINE__);
        }
    } else {
        AmbaPrintColor(RED,"[%s- %d] do AppLibDCF_GetFileList Fail", __FUNCTION__, __LINE__);
    }
    DBGMSGc2(YELLOW, "##[%s-%d] ReturnValue:%llu", __FUNCTION__, __LINE__, (((UINT64)AppLibStorageCurrentObjIdx[mediaType]) << 32) | ((UINT64)AppLibStorageCurrentObj[mediaType]));
    return (((UINT64)AppLibStorageCurrentObjIdx[mediaType]) << 32) | ((UINT64)AppLibStorageCurrentObj[mediaType]);
}

/**
 *  @brief Get the position of previous file
 *
 *  Get the position of previous file
 *
 *  @param [in] mediaType Media type
 *  @param [in] Type DCF handler
 *
 *  @return >0 File position, the others is failure
 */
UINT64 AppLibStorageDmf_GetPrevFilePos(APPLIB_DCF_MEDIA_TYPE_e mediaType, UINT8 Type)
{
    int ReturnValue = 0;
    DBGMSGc2(YELLOW, "==[%s-%d] ReturnValue:%d", __FUNCTION__, __LINE__, ReturnValue);
    if (AppLibStorageDmfHdlr[Type] == NULL) {
        AmbaPrintColor(RED,"[%s- %d] The DCF handler has not be created.", __FUNCTION__, __LINE__);
        return -1;
    }
    if (AppLibDCF_GetBrowseMode(AppLibStorageDmfHdlr[Type]) != mediaType) {
        AppLibDCF_SetBrowseMode(AppLibStorageDmfHdlr[Type], mediaType);
    }
    // Current ObjIdIdx is 0, go to previous ObjId and set current ObjIdIdx to the (count - 1) of FileList
    if (AppLibStorageCurrentObjIdx[mediaType] == 0) {
        ReturnValue =  AppLibDCF_PrevObject(AppLibStorageDmfHdlr[Type]);
        if (ReturnValue > 0) {
            AMP_DCF_FILE_LIST_s *list = NULL;
            AppLibStorageCurrentObj[mediaType] = ReturnValue;
            list = AppLibDCF_GetFileList(AppLibStorageDmfHdlr[Type], AppLibStorageCurrentObj[mediaType]);
            if (list != NULL) {
                AppLibStorageCurrentObjIdx[mediaType] = (list->Count - 1);
                AppLibDCF_RelFileList(AppLibStorageDmfHdlr[Type], list);
                DBGMSGc2(YELLOW,"[%s- %d] PrevObjId = %d, CurObjIdIdx = %d", __FUNCTION__, __LINE__, AppLibStorageCurrentObj[mediaType], AppLibStorageCurrentObjIdx[mediaType]);
            } else {
                AmbaPrintColor(RED,"[%s- %d] do AppLibDCF_GetFileList Fail", __FUNCTION__, __LINE__);
            }
        } else {
                DBGMSGc2(YELLOW,"[%s- %d] It is the first ObjId!!", __FUNCTION__, __LINE__);
        }
    } else if (AppLibStorageCurrentObjIdx[mediaType] > 0) {
        // Current ObjIdIdx is more than 0, minus 1 for current ObjIdIdx
        AppLibStorageCurrentObjIdx[mediaType]--;
        DBGMSGc2(YELLOW,"[%s- %d] PrevObjId = %d, CurObjIdIdx = %d", __FUNCTION__, __LINE__, AppLibStorageCurrentObj[mediaType], AppLibStorageCurrentObjIdx[mediaType]);
    } else {
        AmbaPrintColor(RED,"[%s- %d] It should not happen!!! PrevObjId = %d, CurObjIdIdx = %d", __FUNCTION__, __LINE__, AppLibStorageCurrentObj[mediaType], AppLibStorageCurrentObjIdx[mediaType]);
    }
    DBGMSGc2(YELLOW, "##[%s-%d] ReturnValue:%llu", __FUNCTION__, __LINE__, (((UINT64)AppLibStorageCurrentObjIdx[mediaType]) << 32) | ((UINT64)AppLibStorageCurrentObj[mediaType]));
    return (((UINT64)AppLibStorageCurrentObjIdx[mediaType]) << 32) | ((UINT64)AppLibStorageCurrentObj[mediaType]);
}

/**
 *  @brief Get file list
 *
 *  Get file list
 *
 *  @param [in] mediaType Media type
 *  @param [in] list File list
 *  @param [in] Type DCF handler
 *
 *  @return >=0 success, <0 failure
 */
int AppLibStorageDmf_GetFileList(APPLIB_DCF_MEDIA_TYPE_e mediaType, AMP_DCF_FILE_LIST_s *list, UINT8 Type)
{
    UINT32 ObjId = 0;
    DBGMSGc2(YELLOW, "==[%s-%d]", __FUNCTION__, __LINE__);
    if (AppLibStorageDmfHdlr[Type] == NULL) {
        AmbaPrintColor(RED,"[Applib DMF] The DCF handler has not be created.");
        return -1;
    }
    if (AppLibDCF_GetBrowseMode(AppLibStorageDmfHdlr[Type]) != mediaType) {
        AppLibDCF_SetBrowseMode(AppLibStorageDmfHdlr[Type], mediaType);
    }
    list = AppLibDCF_GetFileList(AppLibStorageDmfHdlr[Type], ObjId);
    if (list == NULL) {
        AmbaPrintColor(RED,"[Applib DMF] <GetFileList> The list is NULL.");
        return -1;
    }
    AppLibDCF_RelFileList(AppLibStorageDmfHdlr[Type], list);
    return 0;
}
#define DCF_FILE_THM_STR    "_thm"
/**
 *  @brief Get the file name By Root name
 *
 *  Get the file name
 *
 *  @param [in] mediaType Media type
 *  @param [in] extName extend file name
 *  @param [in] extType extend file type
 *  @param [in] Type Root name ID
 *  @param [in] Index file index for return file name
 *  @param [in] objId Object ID
 *  @param [out] filename File name
 *
 *  @return >=0 success, <0 failure
 */
int AppLibStorageDmf_GetFileName(APPLIB_DCF_MEDIA_TYPE_e mediaType, char *extName, UINT8 extType, UINT8 Type, UINT32 Index, UINT32 objId, char *filename)
{
    AMP_DCF_FILE_LIST_s *List = NULL;
    DBGMSGc2(YELLOW, "==[%s-%d] extName:%s, extType:%d, Type:%d, Index:%d, objId:%d, filename:%s", __FUNCTION__, __LINE__, extName, extType, Type, Index, objId, filename);
    if (AppLibStorageDmfHdlr[Type] == NULL) {
        AmbaPrintColor(RED,"[Applib DMF] The DCF handler has not be created.");
        return -1;
    }

    if (objId == 0) {
        AmbaPrintColor(RED,"[Applib DMF] Object id = 0.");
        return -1;
    }

    List = AppLibDCF_GetFileList(AppLibStorageDmfHdlr[Type], objId);
    if (List == NULL) {
        AmbaPrintColor(RED,"[Applib DMF] <GetFileName> Get file list fail. objId = %d",objId);
        return -1;
    } else {
        int i = 0;
        if (Index == 0) {
            for ( i = List->Count -1; i >= 0 ; i--) {
                if (AppLibStorageDmf_CheckNaming(List->FileList[i].Name, extName, extType) == 0) {
                    memcpy(filename, List->FileList[i].Name, sizeof(char)*64);
                    AppLibDCF_RelFileList(AppLibStorageDmfHdlr[Type],List);
                    DBGMSGc2(YELLOW, "##[%s-%d] filename:%s, rval = 0", __FUNCTION__, __LINE__, filename);
                    return 0;
                }
            }
        } else {
            for ( i = 0; i < List->Count; i++) {
                if (AppLibStorageDmf_CheckNaming(List->FileList[i].Name, extName, extType) == 0) {
                    if (i == (Index - 1)) {
                        memcpy(filename, List->FileList[i].Name, sizeof(char)*64);
                        AppLibDCF_RelFileList(AppLibStorageDmfHdlr[Type],List);
                        DBGMSGc2(YELLOW, "##[%s-%d] filename:%s, rval = 0", __FUNCTION__, __LINE__, filename);
                        return 0;
                    }
                }
            }
        }
        AppLibDCF_RelFileList(AppLibStorageDmfHdlr[Type],List);
    }
    DBGMSGc2(YELLOW, "##[%s-%d] filename:%s, rval = -1", __FUNCTION__, __LINE__, filename);
    return -1;
}
/**
 *  @brief Get the position of first directory
 *
 *  Get the position of first directory
 *
 *  @param [in] mediaType Media type
 *  @param [in] Type DCF handler
 *
 *  @return >0 The position of first directory, otherwise failure
 */
int AppLibStorageDmf_GetFirstDirPos(APPLIB_DCF_MEDIA_TYPE_e mediaType, UINT8 Type)
{
    DBGMSGc2(YELLOW, "==[%s-%d]", __FUNCTION__, __LINE__);
    if (AppLibStorageDmfHdlr[Type] == NULL) {
        AmbaPrintColor(RED,"[Applib DMF] The DCF handler has not be created.");
        return -1;
    }
    if (AppLibDCF_GetBrowseMode(AppLibStorageDmfHdlr[Type]) != mediaType) {
        AppLibDCF_SetBrowseMode(AppLibStorageDmfHdlr[Type], mediaType);
    }
    return AppLibDCF_FirstDir(AppLibStorageDmfHdlr[Type]);
}

/**
 *  @brief Get the position of last directory
 *
 *  Get the position of last directory
 *
 *  @param [in] mediaType Media type
 *  @param [in] Type DCF handler
 *
 *  @return >0 The position of last directory, otherwise failure
 */
int AppLibStorageDmf_GetLastDirPos(APPLIB_DCF_MEDIA_TYPE_e mediaType, UINT8 Type)
{
    DBGMSGc2(YELLOW, "==[%s-%d]", __FUNCTION__, __LINE__);
    if (AppLibStorageDmfHdlr[Type] == NULL) {
        AmbaPrintColor(RED,"[Applib DMF] The DCF handler has not be created.");
        return -1;
    }
    if (AppLibDCF_GetBrowseMode(AppLibStorageDmfHdlr[Type]) != mediaType) {
        AppLibDCF_SetBrowseMode(AppLibStorageDmfHdlr[Type], mediaType);
    }
    return AppLibDCF_LastDir(AppLibStorageDmfHdlr[Type]);
}

/**
 *  @brief Get the position of next directory
 *
 *  Get the position of next directory
 *
 *  @param [in] mediaType Media type
 *  @param [in] Type DCF handler
 *
 *  @return >0 The position of next directory, otherwise failure
 */
int AppLibStorageDmf_GetNextDirPos(APPLIB_DCF_MEDIA_TYPE_e mediaType, UINT8 Type)
{
    DBGMSGc2(YELLOW, "==[%s-%d]", __FUNCTION__, __LINE__);
    if (AppLibStorageDmfHdlr[Type] == NULL) {
        AmbaPrintColor(RED,"[Applib DMF] The DCF handler has not be created.");
        return -1;
    }
    if (AppLibDCF_GetBrowseMode(AppLibStorageDmfHdlr[Type]) != mediaType) {
        AppLibDCF_SetBrowseMode(AppLibStorageDmfHdlr[Type], mediaType);
    }
    return AppLibDCF_NextDir(AppLibStorageDmfHdlr[Type]);
}

/**
 *  @brief Get the position of previous directory
 *
 *  Get the position of previous directory
 *
 *  @param [in] mediaType Media type
 *  @param [in] Type DCF handler
 *
 *  @return >0 The position of previous directory, otherwise failure
 */
int AppLibStorageDmf_GetPrevDirPos(APPLIB_DCF_MEDIA_TYPE_e mediaType, UINT8 Type)
{
    DBGMSGc2(YELLOW, "==[%s-%d]", __FUNCTION__, __LINE__);
    if (AppLibStorageDmfHdlr[Type] == NULL) {
        AmbaPrintColor(RED,"[Applib DMF] The DCF handler has not be created.");
        return -1;
    }
    if (AppLibDCF_GetBrowseMode(AppLibStorageDmfHdlr[Type]) != mediaType) {
        AppLibDCF_SetBrowseMode(AppLibStorageDmfHdlr[Type], mediaType);
    }
    return AppLibDCF_PrevDir(AppLibStorageDmfHdlr[Type]);
}

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
int AppLibStorageDmf_InitFileCountInTheDir(UINT32 amount, UINT8 Type)
{
    DBGMSGc2(YELLOW, "==[%s-%d]", __FUNCTION__, __LINE__);
    if (AppLibStorageDmfHdlr[Type] == NULL) {
        AmbaPrintColor(RED,"[Applib DMF] InitFileCountInTheDir Fail");
        return -1;
    }
    AppLibStorageDmfHdlr[Type]->CurFilesInDir = amount;
    DBGMSGc2(YELLOW, "==[%s -%d]  Init file amount: %d into DmfHdlr[%d]->CurFilesInDir", __FUNCTION__, __LINE__, AppLibStorageDmfHdlr[Type]->CurFilesInDir, Type);
    return AppLibStorageDmfHdlr[Type]->CurFilesInDir;
}

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
int AppLibStorageDmf_MinusFileCountInTheDir(char *Name)
{
    UINT8 DcfHdlrIdx = STORAGE_DCF_HDLR_MAX - 1;
    DBGMSGc2(YELLOW, "==[%s -%d] Name:%s", __FUNCTION__, __LINE__, Name);
     while (DcfHdlrIdx >= 0) {
        if (strstr(Name, RootName[DcfHdlrIdx]) != NULL) {
            break;
        } else {
            DcfHdlrIdx--;
        }
    }
    DBGMSGc2(YELLOW, "[%s -%d] DcfHdlr: %d", __FUNCTION__, __LINE__, DcfHdlrIdx);
    if (AppLibStorageDmfHdlr[DcfHdlrIdx] == NULL) {
        AmbaPrintColor(RED,"[Applib DMF] MinusFileCountInTheDir Fail");
        return -1;
    }
    //Check the file type for removing is JPEG or MP4
    if ((strncmp(&Name[strlen(Name) - 3], "jpg", 3) == 0) || (strncmp(&Name[strlen(Name) - 3], "JPG", 3) == 0)) {
        DBGMSGc2(GRAY, "[%s -%d]  It is a JPG file", __FUNCTION__, __LINE__);
        if (DCF_IS_DIGIT(Name[strlen(Name) - 5])) {
    AppLibStorageDmfHdlr[DcfHdlrIdx]->CurFilesInDir--;
    if (AppLibStorageDmfHdlr[DcfHdlrIdx]->CurFilesInDir < DCF_FILE_AMOUNT_MIN)
        AppLibStorageDmfHdlr[DcfHdlrIdx]->CurFilesInDir = 0;
    DBGMSGc2(YELLOW, "==[%s -%d]  Minux file amount: %d in DmfHdlr[%d]->CurFilesInDir", __FUNCTION__, __LINE__, AppLibStorageDmfHdlr[DcfHdlrIdx]->CurFilesInDir, DcfHdlrIdx);
        } else {
            DBGMSGc2(YELLOW, "==[%s -%d] It is not the jpeg, do not minus file amount!!", __FUNCTION__, __LINE__);
        }
    } else {
        // It is video file
        if (Name[strlen(Name) - 5] == MAIN_STREAM) {// only calculate main stream file for file amount  of specific folder\
            AppLibStorageDmfHdlr[DcfHdlrIdx]->CurFilesInDir--;
            if (AppLibStorageDmfHdlr[DcfHdlrIdx]->CurFilesInDir < DCF_FILE_AMOUNT_MIN)
                AppLibStorageDmfHdlr[DcfHdlrIdx]->CurFilesInDir = 0;
            DBGMSGc2(YELLOW, "==[%s -%d]  Minux file amount: %d in DmfHdlr[%d]->CurFilesInDir", __FUNCTION__, __LINE__, AppLibStorageDmfHdlr[DcfHdlrIdx]->CurFilesInDir, DcfHdlrIdx);
        } else {
            DBGMSGc2(YELLOW, "==[%s -%d] It is the 2nd streaming MP4 smail file, do not minus file amount!!", __FUNCTION__, __LINE__);
        }
    }
    return AppLibStorageDmfHdlr[DcfHdlrIdx]->CurFilesInDir;
}

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
int AppLibStorageDmf_AddFileCountInTheDir(UINT8 Type)
{
    DBGMSGc2(YELLOW, "==[%s -%d]", __FUNCTION__, __LINE__);
    if (AppLibStorageDmfHdlr[Type] == NULL) {
        AmbaPrintColor(RED,"[Applib DMF] AddFileCountInTheDir Fail");
        return -1;
    }
    AppLibStorageDmfHdlr[Type]->CurFilesInDir++;
    if (AppLibStorageDmfHdlr[Type]->CurFilesInDir > DCF_FILE_AMOUNT_MAX)
        AppLibStorageDmfHdlr[Type]->CurFilesInDir = DCF_FILE_AMOUNT_MIN;
    DBGMSGc2(YELLOW, "==[%s -%d]  Add file amount: %d into DmfHdlr[%d]->CurFilesInDir", __FUNCTION__, __LINE__, AppLibStorageDmfHdlr[Type]->CurFilesInDir, Type);
    return AppLibStorageDmfHdlr[Type]->CurFilesInDir;
}

/**
 *  @brief Get current file amount in the working directory.
 *
 *  Get current file amount in the working directory.
 *
 *  @param [in] Type DCF handler
 *
 *  @return >=0 success, <0 failure
 */
int AppLibStorageDmf_GetFileCountInTheDir(UINT8 Type)
{
    DBGMSGc2(YELLOW, "==[%s-%d]", __FUNCTION__, __LINE__);
    if (AppLibStorageDmfHdlr[Type] == NULL) {
        AmbaPrintColor(RED,"[Applib DMF] GetFileCountInTheDir Fail");
        return -1;
    }
    DBGMSGc2(YELLOW, "==[%s-%d]  file amount: %d in DmfHdlr[%d]->CurFilesInDir", __FUNCTION__, __LINE__, AppLibStorageDmfHdlr[Type]->CurFilesInDir, Type);
    return AppLibStorageDmfHdlr[Type]->CurFilesInDir;
}

/**
 *  @brief Init RTC reset directory index.
 *
 *  Int RTC reset directory index to 0.
 *
 *  @param [in] Type DCF handler
 *
 *  @return >=0 success, <0 failure
 */
int AppLibStorageDmf_InitRtcDirIndex(UINT8 Type)
{
    DBGMSGc2(YELLOW, "==[%s-%d]", __FUNCTION__, __LINE__);
    if (AppLibStorageDmfHdlr[Type] == NULL) {
        AmbaPrintColor(RED,"[Applib DMF] InitRtcDirIndex Fail");
        return -1;
    }
    AppLibStorageDmfHdlr[Type]->CurRtcIdx = 0;
    DBGMSGc2(YELLOW, "==[%s-%d]  Init DmfHdlr[%d]->CurRtcIdx to 0", __FUNCTION__, __LINE__, Type);
    return AppLibStorageDmfHdlr[Type]->CurRtcIdx;
}

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
int AppLibStorageDmf_SetRtcDirIndex(UINT32 RtcIndex, UINT8 Type)
{
    DBGMSGc2(YELLOW, "==[%s-%d]", __FUNCTION__, __LINE__);
    if (AppLibStorageDmfHdlr[Type] == NULL) {
        AmbaPrintColor(RED,"[Applib DMF] SetRtcDirIndex Fail");
        return -1;
    }
    AppLibStorageDmfHdlr[Type]->CurRtcIdx = RtcIndex;
    DBGMSGc2(YELLOW, "==[%s-%d]  Set index: %d into DmfHdlr[%d]->CurRtcIdx", __FUNCTION__, __LINE__, AppLibStorageDmfHdlr[Type]->CurRtcIdx, Type);
    return AppLibStorageDmfHdlr[Type]->CurRtcIdx;
}

/**
 *  @brief Get current used RTC reset directory index.
 *
 *  Get current directory index.
 *
 *  @param [in] Type DCF handler
 *
 *  @return >=0 success, <0 failure
 */
int AppLibStorageDmf_GetRtcDirIndex(UINT8 Type)
{
    DBGMSGc2(YELLOW, "==[%s-%d]", __FUNCTION__, __LINE__);
    if (AppLibStorageDmfHdlr[Type] == NULL) {
        AmbaPrintColor(RED,"[Applib DMF] GetRtcDirIndex Fail");
        return -1;
    }
    DBGMSGc2(YELLOW, "==[%s-%d]  Get index: %d in DmfHdlr[%d]->CurRtcIdx", __FUNCTION__, __LINE__, AppLibStorageDmfHdlr[Type]->CurRtcIdx, Type);
    return AppLibStorageDmfHdlr[Type]->CurRtcIdx;
}


/**
 *  @brief Init all directory index to zero.
 *
 *  Init all directory index to -1 to unuse state.
 *
 *  @param [in] Type DCF handler
 *
 *  @return 0 success, <0 failure
 */
int AppLibStorageDmf_InitDirIndex(UINT8 Type)
{
	DBGMSGc2(YELLOW, "==[%s-%d]", __FUNCTION__, __LINE__);
    if (AppLibStorageDmfHdlr[Type] == NULL) {
        AmbaPrintColor(RED,"[Applib DMF] InitDirIndex Fail");
        return -1;
    }

	for (int i = 0; i <= DIR_DATE_AMOUNT; i++)
        AppLibStorageDmfHdlr[Type]->DirIdxTable[i] = -1;
    DBGMSGc2(YELLOW, "==[%s-%d] Set all DirIdxTable[n] in DmfHdlr[%d] to -1", __FUNCTION__, __LINE__, Type);
    return 0;
}

/**
 *  @brief Set current directory index into specitic directory number.
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
int AppLibStorageDmf_SetDirIndex(UINT32 dnum, UINT32 index, UINT8 Type)
{
    DBGMSGc2(YELLOW, "==[%s-%d]", __FUNCTION__, __LINE__);
    if (AppLibStorageDmfHdlr[Type] == NULL) {
        AmbaPrintColor(RED,"[Applib DMF] SetDirIndex Fail");
        return -1;
    }

    AppLibStorageDmfHdlr[Type]->DirIdxTable[dnum] = index;
    DBGMSGc2(YELLOW, "==[%s-%d] Set index: %d into DmfHdlr[%d]->DirIdxTable[%d]", __FUNCTION__, __LINE__, AppLibStorageDmfHdlr[Type]->DirIdxTable[dnum], Type, dnum);
    return AppLibStorageDmfHdlr[Type]->DirIdxTable[dnum];
    }

/**
 *  @brief Get current directory index from specific directory number
 *
 *  Get current directory index from specific directory number/offset.
 *
 *  @param [in] Type DCF handler
 *
 *  @return >=0 success, -1 this dnum does not use yet, -2 failure
 */
int AppLibStorageDmf_GetDirIndex(UINT32 dnum, UINT8 Type)
{
    DBGMSGc2(YELLOW, "==[%s-%d]", __FUNCTION__, __LINE__);
    if (AppLibStorageDmfHdlr[Type] == NULL) {
        AmbaPrintColor(RED,"[Applib DMF] GetDirIndex Fail");
        return -2;
    }
    DBGMSGc2(YELLOW, "==[%s-%d]  Get index: %d in DmfHdlr[%d]->DirIdxTable[%d]", __FUNCTION__, __LINE__, AppLibStorageDmfHdlr[Type]->DirIdxTable[dnum], Type, dnum);
    return AppLibStorageDmfHdlr[Type]->DirIdxTable[dnum];
}

/**
 *  @brief Get the directory list
 *
 *  Get the directory list
 *
 *  @param [in] mediaType Media type
 *  @param [in] list Directory list
 *  @param [in] Type DCF handler
 *
 *  @return >=0 success, <0 failure
 */
int AppLibStorageDmf_GetDirList(APPLIB_DCF_MEDIA_TYPE_e mediaType, AMP_DCF_DIR_LIST_s *list, UINT8 Type)
{
    DBGMSGc2(YELLOW, "==[%s-%d]", __FUNCTION__, __LINE__);
    if (AppLibStorageDmfHdlr[Type] == NULL) {
        AmbaPrintColor(RED,"[Applib DMF] GetPrevFilePos Fail");
        return -1;
    }

    if (AppLibDCF_GetBrowseMode(AppLibStorageDmfHdlr[Type]) != mediaType) {
        AppLibDCF_SetBrowseMode(AppLibStorageDmfHdlr[Type], mediaType);
    }
    list = AppLibDCF_GetDirList(AppLibStorageDmfHdlr[Type]);
    AppLibDCF_RelDirList(AppLibStorageDmfHdlr[Type], list);
    return 0;
}

/**
 *  @brief Create a file
 *
 *  Create a file
 *
 *  @param [in] mediaType Media type
 *  @param [in] extName Extent name
 *  @param [in] Type DCF handler
 *  @param [out] filename File name
 *
 *  @return >=0 success, <0 failure
 */
int AppLibStorageDmf_CreateFile(APPLIB_DCF_MEDIA_TYPE_e mediaType, char *extName, UINT8 Type, char *filename)
{
    int ReturnValue = 0;
    DBGMSGc2(YELLOW, "==[%s-%d] extName:%s, filename:%s", __FUNCTION__, __LINE__, extName, filename);
    if (AppLibStorageDmfHdlr[Type] == NULL) {
        AmbaPrintColor(RED,"[Applib DMF] The DCF handler has not be created.");
        return -1;
    }

    ReturnValue = AppLibDCF_CreateObject(AppLibStorageDmfHdlr[Type], RootName[Type], extName, filename);
    if (ReturnValue > 0) {
        DBGMSGc2(GREEN,"[Applib DMF] CreateObject %d.",ReturnValue);
        AppLibStorageCurrentObj[mediaType] = ReturnValue;
        AppLibStorageCurrentObj[APPLIB_DCF_MEDIA_DCIM] = ReturnValue;

#ifdef CONFIG_APP_ARD /*  add debug info for QA */
		AmbaPrintColor(YELLOW, "==[%s-%d] extName:%s, filename:%s", __FUNCTION__, __LINE__, extName, filename);
#endif
    } else {
        AmbaPrintColor(RED,"[Applib DMF] CreateObject fail.");
        return -1;
    }

    return AppLibStorageCurrentObj[mediaType];
}

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
int AppLibStorageDmf_CreateFileByType(APPLIB_DCF_MEDIA_TYPE_e mediaType, UINT8 Type, char *extName, char *filename)
{
    int ReturnValue = 0;
    DBGMSGc2(YELLOW, "==[%s-%d] extName:%s, filename:%s", __FUNCTION__, __LINE__, extName, filename);
    if (AppLibStorageDmfHdlr[Type] == NULL) {
        AmbaPrintColor(RED,"[Applib DMF] The DCF handler has not be created.");
        return -1;
    }

    ReturnValue = AppLibDCF_CreateObject(AppLibStorageDmfHdlr[Type], RootName[Type], extName, filename);

    if (ReturnValue > 0) {
        DBGMSGc2(GREEN,"[Applib DMF] CreateObject %d.",ReturnValue);
    } else {
        AmbaPrintColor(RED,"[Applib DMF] CreateObject fail.");
        return -1;
    }

    return ReturnValue;
}


/**
 *  @brief Create a extended file
 *
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
int AppLibStorageDmf_CreateFileExtended(APPLIB_DCF_MEDIA_TYPE_e mediaType, UINT32 objId, char *extName, UINT8 extType, UINT8 seqNum, UINT8 Type, char *filename)
{
    DBGMSGc2(YELLOW, "==[%s-%d] objId:%d, extName:%s, filename:%s, seqNum:%d, filename:%s", __FUNCTION__, __LINE__, objId, extName, filename, seqNum, filename);
    if (AppLibStorageDmfHdlr[Type] == NULL) {
        AmbaPrintColor(RED,"[Applib DMF] The DCF handler has not be created.");
        return -1;
    }
    if (objId == 0) {
        AmbaPrintColor(RED,"[Applib DMF] Object id = 0.");
        return -1;
    } else {
        return AppLibDCF_CreateExtendedObject(AppLibStorageDmfHdlr[Type], objId, RootName[Type], extType, seqNum, extName, filename);
    }
}

/**
 *  @brief Delete file in the table
 *
 *  Delete file in the table
 *
 *  @param [in] mediaType Media type
 *  @param [in] objId Object id
 *  @param [in] Type DCF handler
 *
 *  @return >=0 success, <0 failure
 */
int AppLibStorageDmf_DeleteFile(APPLIB_DCF_MEDIA_TYPE_e mediaType, UINT32 objId, UINT8 Type)
{
    int ReturnValue = 0,i = 0;
    DBGMSGc2(YELLOW, "==[%s-%d] objId:%d", __FUNCTION__, __LINE__, objId);
    if (AppLibStorageDmfHdlr[Type] == NULL) {
        AmbaPrintColor(RED,"[Applib DMF] The DCF handler has not be created.");
        return -1;
    }

if (AppLibDCF_GetBrowseMode(AppLibStorageDmfHdlr[Type]) != mediaType) {
        AppLibDCF_SetBrowseMode(AppLibStorageDmfHdlr[Type], mediaType);
    }
    ReturnValue = AppLibDCF_DeleteObject(AppLibStorageDmfHdlr[Type], objId);
////////////add 05_26/////////////////////////////////
#ifdef CONFIG_APP_ARD
	for (i=0;i<empty_dir_count;i++)
	AmpCFS_Rmdir(empty_dir_group[i]);
#endif
/////////////////////////////////////////////
    /* Set refresh status to TRUE to force rescan dirctory when creating file. */
    AppLibStorageDmf_SetRefreshStatus(TRUE);

    return ReturnValue;
}



#ifdef CONFIG_APP_ARD
int AppLibStorageDmf_DeleteFileAll(APPLIB_DCF_MEDIA_TYPE_e mediaType, UINT8 Type)
{
    int ReturnValue = 0,i = 0;

    if (AppLibStorageDmfHdlr[Type] == NULL) {
        AmbaPrintColor(RED,"[Applib DMF] The DCF handler has not be created.");
        return -1;
    }

	if (AppLibDCF_GetBrowseMode(AppLibStorageDmfHdlr[Type]) != mediaType) {
        AppLibDCF_SetBrowseMode(AppLibStorageDmfHdlr[Type], mediaType);
    }
    ReturnValue = AppLibDCF_DeleteObjectAll(AppLibStorageDmfHdlr[Type]);
////////////add 05_26/////////////////////////////////
#ifdef CONFIG_APP_ARD
        for (i=0;i<empty_dir_count;i++)
        AmpCFS_Rmdir(empty_dir_group[i]);
#endif
/////////////////////////////////////////////

    return ReturnValue;
}
#endif

/**
 *  @brief Get file amount
 *
 *  Get file amount
 *
 *  @param [in] mediaType Media type
 *  @param [in] Type DCF handler
 *
 *  @return File amount
 */
int AppLibStorageDmf_GetFileAmount(APPLIB_DCF_MEDIA_TYPE_e mediaType, UINT8 Type)
{
    int FileObjAmount = 0;
    int FileAmount = 0;
    int ReturnValue = 0;
    AMP_DCF_FILE_LIST_s *List = NULL;

    DBGMSGc2(YELLOW, "==[%s-%d]", __FUNCTION__, __LINE__);
    if (AppLibStorageDmfHdlr[Type] == NULL) {
        AmbaPrintColor(RED,"[Applib DMF] The DCF handler has not be created.");
        return -1;
    }
    if (AppLibDCF_GetBrowseMode(AppLibStorageDmfHdlr[Type]) != mediaType) {
        AppLibDCF_SetBrowseMode(AppLibStorageDmfHdlr[Type], mediaType);
    }

    FileObjAmount = AppLibDCF_GetObjectAmount(AppLibStorageDmfHdlr[Type]);
    ReturnValue = AppLibDCF_GetCurrentObjectId(AppLibStorageDmfHdlr[Type]);
    if (ReturnValue == 0) {
        ReturnValue = AppLibDCF_FirstObject(AppLibStorageDmfHdlr[Type]);
    }
    for(int i = 0; i < FileObjAmount; i++) {
        List = AppLibDCF_GetFileList(AppLibStorageDmfHdlr[Type], ReturnValue);
        FileAmount = FileAmount + List->Count;
        ReturnValue =  AppLibDCF_NextObject(AppLibStorageDmfHdlr[Type]);
        if (ReturnValue == 0) {
            ReturnValue = AppLibDCF_FirstObject(AppLibStorageDmfHdlr[Type]);
        }
        AppLibDCF_RelFileList(AppLibStorageDmfHdlr[Type],List);
    }

    return FileAmount;
}

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
int AppLibStorageDmf_GetObjFileCount(UINT8 Type, UINT32 ObjID)
{
    int ReturnValue = 0;
    int FileCount = 0;
    AMP_DCF_FILE_LIST_s *List = NULL;

    List = AppLibDCF_GetFileList(AppLibStorageDmfHdlr[Type], ObjID);
    if (List != NULL) {
        FileCount = List->Count;
        AppLibDCF_RelFileList(AppLibStorageDmfHdlr[Type],List);
    }

    return FileCount;
}

/**
 *  @brief Get file object amount
 *
 *  Get file amount
 *
 *  @param [in] mediaType Media type
 *  @param [in] Type DCF handler
 *
 *  @return File amount
 */
int AppLibStorageDmf_GetFileObjAmount(APPLIB_DCF_MEDIA_TYPE_e mediaType, UINT8 Type)
{
    int FileObjAmount = 0;

    DBGMSGc2(YELLOW, "==[%s-%d]", __FUNCTION__, __LINE__);
    if (AppLibStorageDmfHdlr[Type] == NULL) {
        AmbaPrintColor(RED,"[Applib DMF] The DCF handler has not be created.");
        return -1;
    }
    if (AppLibDCF_GetBrowseMode(AppLibStorageDmfHdlr[Type]) != mediaType) {
        AppLibDCF_SetBrowseMode(AppLibStorageDmfHdlr[Type], mediaType);
    }
    FileObjAmount = AppLibDCF_GetObjectAmount(AppLibStorageDmfHdlr[Type]);

    return FileObjAmount;
}

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
int AppLibStorageDmf_SetFileNumberMode(APPLIB_DCF_MEDIA_TYPE_e mediaType, APPLIB_DCF_NUMBER_MODE_e NumberMode, UINT8 Type)
{
    int ReturnValue = 0;
    UINT32 FileObjID = 0;
    DBGMSGc2(YELLOW, "==[%s-%d]", __FUNCTION__, __LINE__);
    if (AppLibStorageDmfHdlr[Type] == NULL) {
        AmbaPrintColor(RED,"[Applib DMF] The DCF handler has not be created.");
        return -1;
    }
    if (AppLibDCF_GetBrowseMode(AppLibStorageDmfHdlr[Type]) != mediaType) {
        AppLibDCF_SetBrowseMode(AppLibStorageDmfHdlr[Type], mediaType);
    }
    FileObjID = AppLibStorageDmf_GetLastFilePos(mediaType, Type);
    ReturnValue = AppLibDCF_SetNumberMode(AppLibStorageDmfHdlr[Type],NumberMode, FileObjID);

    return ReturnValue;
}

/**
 *  @brief Set system boot status
 *
 *  Set System Boot Status
 *
 *  @param [in] Type DCF handler
 *
 *  @param [in] Boot status
 *
 *  @return >=0 success, <0 failure
 */
INT8 AppLibStorageDmf_SetBootStatus(UINT8 Type, INT8 Status)
{
    DBGMSGc2(YELLOW, "==[%s-%d]", __FUNCTION__, __LINE__);
    if (AppLibStorageDmfHdlr[Type] == NULL) {
        AmbaPrintColor(RED,"[Applib DMF] The DCF handler has not be created.");
        return -1;
    }
    AppLibStorageDmfHdlr[Type]->BootStatus = Status;

    return AppLibStorageDmfHdlr[Type]->BootStatus;
}

/**
 *  @brief Get System Boot Status
 *
 *  Get System Boot Status
 *
 *  @param [in] Type DCF handler
 *
 *  @return >=0 success, <0 failure
 */
INT8 AppLibStorageDmf_GetBootStatus(UINT8 Type)
{
    DBGMSGc2(YELLOW, "==[%s-%d]", __FUNCTION__, __LINE__);
    if (AppLibStorageDmfHdlr[Type] == NULL) {
        AmbaPrintColor(RED,"[Applib DMF] The DCF handler has not be created.");
        return -1;
    }
    DBGMSGc2(YELLOW, "##[%s-%d] DmfHdlr[%d]->BootStatus:%d", __FUNCTION__, __LINE__, Type, AppLibStorageDmfHdlr[Type]->BootStatus);
    return AppLibStorageDmfHdlr[Type]->BootStatus;
}

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
int AppLibStorageDmf_SetCurDnum(UINT8 Type, UINT32 Dnum)
{
    DBGMSGc2(YELLOW, "==[%s-%d]", __FUNCTION__, __LINE__);
    if (AppLibStorageDmfHdlr[Type] == NULL) {
        AmbaPrintColor(RED,"[Applib DMF] The DCF handler has not be created.");
        return -1;
    }
    AppLibStorageDmfHdlr[Type]->CurDnum= Dnum;
    DBGMSGc2(YELLOW, "##[%s-%d] DmfHdlr[%d]->CurDnum:%d", __FUNCTION__, __LINE__, Type, AppLibStorageDmfHdlr[Type]->CurDnum);
    return AppLibStorageDmfHdlr[Type]->CurDnum;
}

/**
 *  @brief Get Current Dnum
 *
 *  Get Current Dnum
 *
 *  @param [in] Type DCF handler
 *
 *  @return >=0 success, <0 failure
 */
int AppLibStorageDmf_GetCurDnum(UINT8 Type)
{
    DBGMSGc2(YELLOW, "==[%s-%d]", __FUNCTION__, __LINE__);
    if (AppLibStorageDmfHdlr[Type] == NULL) {
        AmbaPrintColor(RED,"[Applib DMF] The DCF handler has not be created.");
        return -1;
    }
    DBGMSGc2(YELLOW, "##[%s-%d] DmfHdlr[%d]->CurDnum:%d", __FUNCTION__, __LINE__, Type, AppLibStorageDmfHdlr[Type]->CurDnum);
    return AppLibStorageDmfHdlr[Type]->CurDnum;
    }

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
INT8 AppLibStorageDmf_SetCurDnumIdx(UINT8 Type, INT8 DnumIdx)
{
    DBGMSGc2(YELLOW, "==[%s-%d]", __FUNCTION__, __LINE__);
    if (AppLibStorageDmfHdlr[Type] == NULL) {
        AmbaPrintColor(RED,"[Applib DMF] The DCF handler has not be created.");
        return -1;
    }
    AppLibStorageDmfHdlr[Type]->CurDirIdx= DnumIdx;
    DBGMSGc2(YELLOW, "##[%s-%d] DmfHdlr[%d]->CurDirIdx:%d", __FUNCTION__, __LINE__, Type, AppLibStorageDmfHdlr[Type]->CurDirIdx);
    return AppLibStorageDmfHdlr[Type]->CurDirIdx;
}

/**
 *  @brief Get Current Dnum Index
 *
 *  Get Current Dnum Index
 *
 *  @param [in] Type DCF handler
 *
 *  @return >=0 success, <0 failure
 */
INT8 AppLibStorageDmf_GetCurDnumIdx(UINT8 Type)
{
    DBGMSGc2(YELLOW, "==[%s-%d]", __FUNCTION__, __LINE__);
    if (AppLibStorageDmfHdlr[Type] == NULL) {
        AmbaPrintColor(RED,"[Applib DMF] The DCF handler has not be created.");
        return -1;
    }
    DBGMSGc2(YELLOW, "##[%s-%d] DmfHdlr[%d]->CurDirIdx:%d", __FUNCTION__, __LINE__, Type, AppLibStorageDmfHdlr[Type]->CurDirIdx);
    return AppLibStorageDmfHdlr[Type]->CurDirIdx;
}

/**
 *  @brief Set DMF Refresh Status
 *
 *  Set DMF Refresh Status. Set to TURE if DCF is refreshed.
 *
 *  @param [in] Refreshed DCF is refreshed or not.
 *
 *  @return 0
 */
int AppLibStorageDmf_SetRefreshStatus(BOOL Refreshed)
{
    RefreshStatus = Refreshed;
    return 0;
}

/**
 *  @brief Get DMF Refresh Status
 *
 *  Get DMF Refresh Status.
 *
 *  @param [in] ClearStatus Reset refreshed status to FLASE if ClearStatus is TRUE.
 *
 *  @return TRUE DCF has been refreshed, FALSE DCF has never refreshed
 */
BOOL AppLibStorageDmf_GetRefreshStatus(BOOL ClearStatus)
{
    BOOL Refreshed = RefreshStatus;

    if (ClearStatus) {
       RefreshStatus = FALSE;
    }

    return Refreshed;
}

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
int AppLibStorageDmf_ScanDirForMaxFnum(UINT8 DcfHdlrIdx, UINT32 DnumScanned, char *DirPath, UINT32 *RetMaxFnum, UINT32 *RetFileAmount)
{
    int FileObjAmount = 0;
    UINT32 ObjectId = 0;
    UINT32 Dnum = 0;
    UINT32 Fnum = 0;
    UINT32 MaxFnum = 0;
    UINT32 FileAmount = 0;
    UINT32 RootNameLen = 0;
    AMP_DCF_FILE_LIST_s *List = NULL;

    if (DcfHdlrIdx >= STORAGE_DCF_HDLR_MAX) {
        AmbaPrintColor(RED,"[Applib DMF] invalid DCF handler index. (DcfHdlrIdx = %d)",DcfHdlrIdx);
        return -1;
    }

    if (AppLibStorageDmfHdlr[DcfHdlrIdx] == NULL) {
        AmbaPrintColor(RED,"[Applib DMF] The DCF handler has not be created.");
        return -1;
    }

    if (!DirPath || !RetMaxFnum || !RetFileAmount) {
        AmbaPrintColor(RED,"[Applib DMF] invalid param (DirName: 0x%x ,RetMaxFnum: 0x%x, RetFileAmount: 0x%x)",
                                                        DirPath, RetMaxFnum, RetFileAmount);
        return -1;
    }

    RootNameLen = strlen(RootName[DcfHdlrIdx]);

    FileObjAmount = AppLibDCF_GetObjectAmount(AppLibStorageDmfHdlr[DcfHdlrIdx]);
    ObjectId = AppLibDCF_GetCurrentObjectId(AppLibStorageDmfHdlr[DcfHdlrIdx]);
    if (ObjectId == 0) {
        ObjectId = AppLibDCF_FirstObject(AppLibStorageDmfHdlr[DcfHdlrIdx]);
    }

    DBGMSGc2(YELLOW,"##[%s-%d] DirPath: %s",__FUNCTION__,__LINE__,DirPath);
    for(int i = 0; i < FileObjAmount; i++) {
        Dnum = AppLibDCF_IdToDnum(AppLibStorageDmfHdlr[DcfHdlrIdx],ObjectId);
        Fnum = AppLibDCF_IdToFnum(AppLibStorageDmfHdlr[DcfHdlrIdx],ObjectId);
        List = AppLibDCF_GetFileList(AppLibStorageDmfHdlr[DcfHdlrIdx], ObjectId);
        if (List && (Dnum == DnumScanned)) {
            DBGMSGc2(YELLOW,"##[%s-%d] file name: %s",__FUNCTION__,__LINE__,List->FileList[0].Name);

            /* Comapre List->FileList[0] with DirName is the workaround for the case that
                      different dirs have the same dnum.*/
            if (strncmp(List->FileList[0].Name, DirPath, (RootNameLen + 1+ DCF_DIR_PATTERN_LEN)) == 0) {
                if (Fnum > MaxFnum) {
                    MaxFnum = Fnum;
                }
                FileAmount++;
            }
        }

        ObjectId =  AppLibDCF_NextObject(AppLibStorageDmfHdlr[DcfHdlrIdx]);
        if (ObjectId == 0) {
            ObjectId = AppLibDCF_FirstObject(AppLibStorageDmfHdlr[DcfHdlrIdx]);
        }
        AppLibDCF_RelFileList(AppLibStorageDmfHdlr[DcfHdlrIdx],List);
    }

    *RetMaxFnum = MaxFnum;
    *RetFileAmount = FileAmount;
    AmbaPrintColor(YELLOW,"##[%s-%d] *RetMaxFnum = %d, *RetFileAmount = %d",__FUNCTION__,__LINE__,*RetMaxFnum,*RetFileAmount);

    return 0;
}

