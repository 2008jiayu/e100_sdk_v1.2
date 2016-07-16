/**
 * @file src/app/connected/applib/src/storage/ApplibStorage_Dmf.c
 *
 * Implementation of storage DMF(Digital Media File System) utility
 *
 * History:
 *    2013/12/10 - [Martin Lai] created file
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
#define DEBUG_APPLIB_STORAGE_DMF
#if defined(DEBUG_APPLIB_STORAGE_DMF)
    #define DBGMSG AmbaPrint
    #define DBGMSGc(x) AmbaPrintColor(GREEN,x)
    #define DBGMSGc2 AmbaPrintColor
#else
    #define DBGMSG(...)
    #define DBGMSGc(...)
    #define DBGMSGc2(...)
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
    ReturnValue = AppLibStorageDmf_SetupFileSystem();
    switch (dcfType) {
    case STORAGE_DCF_TYPE_DEFAULT:
    default:
        ReturnValue = AppLibStorageDmf_InitDcfDefault();
        break;
    }

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
    return ReturnValue;
}

static APPLIB_DCF_HDLR_s *AppLibStorageDmfHdlr[STORAGE_DCF_HDLR_MAX] = {NULL};
static UINT32 AppLibStorageCurrentObj[4] = {0};
static char *RootName[STORAGE_DCF_HDLR_MAX] = {NULL};
static char RootNameDef[64] = "C:\\DCIM";
static char RootNameEmergencyDef[64] = "C:\\EVENT";

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

    RootNameDef[0] = drive;
    //RootNameEmergencyDef[0] = drive;
    AmbaPrintColor(GREEN,"[AppLibStorageDmf_CreateHandler]sizeof rootname : %d", sizeof(RootName));
    RootName[0] = RootNameDef;
#if defined(STORAGE_EMERGENCY_DCF_ON)
    RootName[1]= RootNameEmergencyDef;
#endif

    AppLibDCF_GetDefaultCfg(&DcfConfig);
    for (i=0;i<STORAGE_DCF_HDLR_MAX;i++) {
        strcpy(DcfConfig.RootList.List[0].Name, RootName[i]);
        AppLibStorageDmfHdlr[i] = AppLibDCF_Create(&DcfConfig);
    }

    for (i=0;i<STORAGE_DCF_HDLR_MAX;i++) {
        if (AppLibStorageDmfHdlr[i] == NULL) {
            ReturnValue = -1;
            break;
        }
    }
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

    AppLibComSvcHcmgr_SendMsgNoWait(HMSG_STORAGE_BUSY, drive, 0);
    if (AppLibStorageDmfHdlr[0] == NULL) {
        AppLibStorageDmf_CreateHandler(drive);
    }

    for (i=0; i<STORAGE_DCF_HDLR_MAX; i++) {
        ReturnValue = AppLibDCF_Refresh(AppLibStorageDmfHdlr[i]);
        if (ReturnValue < 0)
            break;
    }

    if ((ReturnValue == -1) || (ReturnValue == -2)) { /** card error or DMF fail */
        AmbaPrintColor(RED, "<AppLibStorageDmf_Refresh> Referesh DMF table Fail ReturnValue = %d", ReturnValue);
    }

    AppLibComSvcHcmgr_SendMsgNoWait(HMSG_STORAGE_IDLE, drive, 0);

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
    if (AppLibStorageDmfHdlr[0] == NULL) {
        AmbaPrintColor(RED,"[Applib DMF] The DCF handler has not be created.");
        return -1;
    }
    for (i=0;i<STORAGE_DCF_HDLR_MAX;i++) {
        ReturnValue = AppLibDCF_Delete(AppLibStorageDmfHdlr[i]);
        if (ReturnValue < 0)
            break;
    }

    return ReturnValue;
}

/**
 *  @brief Get the position of the current file
 *
 *  Get the position of the current file
 *
 *  @param [in] mediaType Media type
 *
 *  @return >0 File position, the others is failure
 */
int AppLibStorageDmf_GetCurrFilePos(APPLIB_DCF_MEDIA_TYPE_e mediaType, UINT32 Type)
{
    int ReturnValue = 0;
    if (AppLibStorageDmfHdlr[Type] == NULL) {
        AmbaPrintColor(RED,"[Applib DMF] The DCF handler has not be created.");
        return -1;
    }
    if (AppLibDCF_GetBrowseMode(AppLibStorageDmfHdlr[Type]) != mediaType) {
        AppLibDCF_SetBrowseMode(AppLibStorageDmfHdlr[Type], mediaType);
    }
    ReturnValue = AppLibDCF_GetCurrentObjectId(AppLibStorageDmfHdlr[Type]);
    if (ReturnValue == 0) {
        AmbaPrintColor(RED,"[Applib DMF] The DCF handler has not be created.");
    }
    return ReturnValue;
}

/**
 *  @brief Get the position of the first file
 *
 *  Get the position of the first file
 *
 *  @param [in] mediaType Media type
 *
 *  @return >0 File position, the others is failure
 */
int AppLibStorageDmf_GetFirstFilePos(APPLIB_DCF_MEDIA_TYPE_e mediaType, UINT32 Type)
{
    int ReturnValue = 0;
    if (AppLibStorageDmfHdlr[Type] == NULL) {
        AmbaPrintColor(RED,"[Applib DMF] The DCF handler has not be created.");
        return -1;
    }
    if (AppLibDCF_GetBrowseMode(AppLibStorageDmfHdlr[Type]) != mediaType) {
        AppLibDCF_SetBrowseMode(AppLibStorageDmfHdlr[Type], mediaType);
    }
    ReturnValue = AppLibDCF_FirstObject(AppLibStorageDmfHdlr[Type]);
    if (ReturnValue > 0) {
        DBGMSGc2(GREEN,"[Applib DMF] GetLastFilePos GetFirstFilePos = %d", ReturnValue);
        AppLibStorageCurrentObj[mediaType] = ReturnValue;
    } else {
        DBGMSGc2(RED,"[Applib DMF] GetFirstFilePos Fail");
    }
    return AppLibStorageCurrentObj[mediaType];
}

/**
 *  @brief Get the position of the last file
 *
 *  Get the position of the last file
 *
 *  @param [in] mediaType Media type
 *
 *  @return >0 File position, the others is failure
 */
int AppLibStorageDmf_GetLastFilePos(APPLIB_DCF_MEDIA_TYPE_e mediaType, UINT32 Type)
{
    int ReturnValue = 0;
    if (AppLibStorageDmfHdlr[Type] == NULL) {
        AmbaPrintColor(RED,"[Applib DMF] The DCF handler has not be created.");
        return -1;
    }
    if (AppLibDCF_GetBrowseMode(AppLibStorageDmfHdlr[Type]) != mediaType) {
        AppLibDCF_SetBrowseMode(AppLibStorageDmfHdlr[Type], mediaType);
    }
    ReturnValue =  AppLibDCF_LastObject(AppLibStorageDmfHdlr[Type]);
    if (ReturnValue > 0) {
        DBGMSGc2(GREEN,"[Applib DMF] GetLastFilePos ReturnValue = %d", ReturnValue);
        AppLibStorageCurrentObj[mediaType] = ReturnValue;
    } else {
        DBGMSGc2(RED,"[Applib DMF] GetLastFilePos Fail");
    }
    return AppLibStorageCurrentObj[mediaType];
}

/**
 *  @brief Get the position of the next file
 *
 *  Get the position of the next file
 *
 *  @param [in] mediaType Media type
 *
 *  @return >0 File position, the others is failure
 */
int AppLibStorageDmf_GetNextFilePos(APPLIB_DCF_MEDIA_TYPE_e mediaType, UINT32 Type)
{
    int ReturnValue = 0;
    if (AppLibStorageDmfHdlr[Type] == NULL) {
        AmbaPrintColor(RED,"[Applib DMF] The DCF handler has not be created.");
        return -1;
    }
    if (AppLibDCF_GetBrowseMode(AppLibStorageDmfHdlr[Type]) != mediaType) {
        AppLibDCF_SetBrowseMode(AppLibStorageDmfHdlr[Type], mediaType);
    }
    ReturnValue =  AppLibDCF_NextObject(AppLibStorageDmfHdlr[Type]);
    if (ReturnValue > 0) {
        AppLibStorageCurrentObj[mediaType] = ReturnValue;
        DBGMSGc2(GREEN,"[Applib DMF] GetNextFilePos ReturnValue = %d", ReturnValue);
    } else {
        DBGMSGc2(RED,"[Applib DMF] GetNextFilePos Fail");
    }
    return AppLibStorageCurrentObj[mediaType];
}

/**
 *  @brief Get the position of previous file
 *
 *  Get the position of previous file
 *
 *  @param [in] mediaType Media type
 *
 *  @return >0 File position, the others is failure
 */
int AppLibStorageDmf_GetPrevFilePos(APPLIB_DCF_MEDIA_TYPE_e mediaType, UINT32 Type)
{
    int ReturnValue = 0;
    if (AppLibStorageDmfHdlr[Type] == NULL) {
        AmbaPrintColor(RED,"[Applib DMF] The DCF handler has not be created.");
        return -1;
    }
    if (AppLibDCF_GetBrowseMode(AppLibStorageDmfHdlr[Type]) != mediaType) {
        AppLibDCF_SetBrowseMode(AppLibStorageDmfHdlr[Type], mediaType);
    }
    ReturnValue =  AppLibDCF_PrevObject(AppLibStorageDmfHdlr[Type]);
    if (ReturnValue > 0) {
        DBGMSGc2(GREEN,"[Applib DMF] type= %d, GetPrevFilePos ReturnValue = %d", mediaType, ReturnValue);
        AppLibStorageCurrentObj[mediaType] = ReturnValue;
    } else {
        DBGMSGc2(RED,"[Applib DMF] GetPrevFilePos Fail");
    }
    return AppLibStorageCurrentObj[mediaType];
}

/**
 *  @brief Get file list
 *
 *  Get file list
 *
 *  @param [in] mediaType Media type
 *  @param [in] list File list
 *
 *  @return >=0 success, <0 failure
 */
int AppLibStorageDmf_GetFileList(APPLIB_DCF_MEDIA_TYPE_e mediaType, AMP_DCF_FILE_LIST_s *list, UINT32 Type)
{
    UINT32 ObjId = 0;
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
 *  @param [in] extType extend file type
 *  @param [in] Type Root name ID
 *  @param [in] Index file index for return file name
 *  @param [in] objId Object ID
 *  @param [out] filename File name
 *
 *  @return >=0 success, <0 failure
 */
int AppLibStorageDmf_GetFileName(APPLIB_DCF_MEDIA_TYPE_e mediaType, char *extName, UINT8 extType, UINT32 Type, UINT32 Index, UINT32 objId, char *filename)
{
    AMP_DCF_FILE_LIST_s *List = NULL;

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
        int i;
        char *p;
        if (Index == 0) {
            for ( i = List->Count -1; i >= 0 ; i--) {
                p = strrchr(List->FileList[i].Name, '.');
                if (strcmp(p, extName) == 0) {
                    if ( (extType == APPLIB_DCF_EXT_OBJECT_SPLIT_FILE) ^ (strstr(List->FileList[i].Name,DCF_FILE_THM_STR) != NULL)) {
                        memcpy(filename, List->FileList[i].Name, sizeof(char)*64);
                        AppLibDCF_RelFileList(AppLibStorageDmfHdlr[Type],List);
                        return 0;
                    }
                }
            }
        } else {
            int IndexCount = 1;
            for ( i = 0; i < List->Count; i++) {
                p = strrchr(List->FileList[i].Name, '.');
                if (strcmp(p, extName) == 0) {
                    if ( (extType == APPLIB_DCF_EXT_OBJECT_SPLIT_FILE) ^ (strstr(List->FileList[i].Name,DCF_FILE_THM_STR) != NULL) ) {
                        if (IndexCount == Index) {
                            memcpy(filename, List->FileList[i].Name, sizeof(char)*64);
                            AppLibDCF_RelFileList(AppLibStorageDmfHdlr[Type],List);
                            return 0;
                        } else {
                            IndexCount++;
                        }
                    }
                }
            }
        }
        AppLibDCF_RelFileList(AppLibStorageDmfHdlr[Type],List);
    }
    return -1;
}
/**
 *  @brief Get the position of first directory
 *
 *  Get the position of first directory
 *
 *  @param [in] mediaType Media type
 *
 *  @return >0 The position of first directory, otherwise failure
 */
int AppLibStorageDmf_GetFirstDirPos(APPLIB_DCF_MEDIA_TYPE_e mediaType, UINT32 Type)
{
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
 *
 *  @return >0 The position of last directory, otherwise failure
 */
int AppLibStorageDmf_GetLastDirPos(APPLIB_DCF_MEDIA_TYPE_e mediaType, UINT32 Type)
{
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
 *
 *  @return >0 The position of next directory, otherwise failure
 */
int AppLibStorageDmf_GetNextDirPos(APPLIB_DCF_MEDIA_TYPE_e mediaType, UINT32 Type)
{
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
 *
 *  @return >0 The position of previous directory, otherwise failure
 */
int AppLibStorageDmf_GetPrevDirPos(APPLIB_DCF_MEDIA_TYPE_e mediaType, UINT32 Type)
{
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
 *  @brief Get the directory list
 *
 *  Get the directory list
 *
 *  @param [in] mediaType Media type
 *  @param [in] list Directory list
 *
 *  @return >=0 success, <0 failure
 */
int AppLibStorageDmf_GetDirList(APPLIB_DCF_MEDIA_TYPE_e mediaType, AMP_DCF_DIR_LIST_s *list, UINT32 Type)
{
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
 *  @param [out] filename File name
 *
 *  @return >=0 success, <0 failure
 */
int AppLibStorageDmf_CreateFile(APPLIB_DCF_MEDIA_TYPE_e mediaType, char *extName, UINT32 Type, char *filename)
{
    int ReturnValue = 0;
    if (AppLibStorageDmfHdlr[Type] == NULL) {
        AmbaPrintColor(RED,"[Applib DMF] The DCF handler has not be created.");
        return -1;
    }

    ReturnValue = AppLibDCF_CreateObject(AppLibStorageDmfHdlr[Type], RootName[Type], extName, filename);
    if (ReturnValue > 0) {
        DBGMSGc2(GREEN,"[Applib DMF] CreateObject %d.",ReturnValue);
        AppLibStorageCurrentObj[mediaType] = ReturnValue;
        AppLibStorageCurrentObj[APPLIB_DCF_MEDIA_DCIM] = ReturnValue;
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
int AppLibStorageDmf_CreateFileByType(APPLIB_DCF_MEDIA_TYPE_e mediaType, UINT32 Type, char *extName, char *filename)
{
    int ReturnValue = 0;
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
 *  @param [out] filename File name
 *
 *  @return >=0 success, <0 failure
 */
int AppLibStorageDmf_CreateFileExtended(APPLIB_DCF_MEDIA_TYPE_e mediaType, UINT32 objId, char *extName, UINT8 extType, UINT8 seqNum, UINT32 Type, char *filename)
{
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
 *
 *  @return >=0 success, <0 failure
 */
int AppLibStorageDmf_DeleteFile(APPLIB_DCF_MEDIA_TYPE_e mediaType, UINT32 objId, UINT32 Type)
{
    int ReturnValue = 0,i = 0;

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

    return ReturnValue;
}

/**
 *  @brief Get file amount
 *
 *  Get file amount
 *
 *  @param [in] mediaType Media type
 *
 *  @return File amount
 */
int AppLibStorageDmf_GetFileAmount(APPLIB_DCF_MEDIA_TYPE_e mediaType, UINT32 Type)
{
    if (AppLibStorageDmfHdlr[Type] == NULL) {
        AmbaPrintColor(RED,"[Applib DMF] The DCF handler has not be created.");
        return -1;
    }
if (AppLibDCF_GetBrowseMode(AppLibStorageDmfHdlr[Type]) != mediaType) {
        AppLibDCF_SetBrowseMode(AppLibStorageDmfHdlr[Type], mediaType);
    }
    return AppLibDCF_GetObjectAmount(AppLibStorageDmfHdlr[Type]);
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
 *  @return >=0 success, <0 failure
 */
int AppLibStorageDmf_SetFileNumberMode(APPLIB_DCF_MEDIA_TYPE_e mediaType, APPLIB_DCF_NUMBER_MODE_e NumberMode, UINT32 Type)
{
    int ReturnValue = 0;
    UINT32 FileObjID = 0;
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

