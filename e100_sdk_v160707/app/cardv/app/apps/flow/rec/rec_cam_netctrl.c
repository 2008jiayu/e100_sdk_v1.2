#include <apps/flow/rec/rec_cam.h>
#include <system/app_pref.h>
#include <system/app_util.h>
#include <system/status.h>
#include <apps/flow/widget/menu/menu.h>
#include <apps/flow/widget/menu/menu_photo.h>
#include <apps/flow/widget/menu/menu_video.h>
#include <apps/flow/widget/menu/menu_setup.h>
#include <net/ApplibNet_JsonUtility.h>
#include <transcoder/still_decode/ApplibTranscoder_Thumb_Basic.h>
#include <AmbaVer.h>
#include <AmbaRTC.h>
#include <AmbaUtility.h>
#include "AmbaSysCtrl.h"

#define NET_OP_DBG_EN
#define NET_OP_ERR_EN

#undef NET_OP_DBG
#ifdef NET_OP_DBG_EN
#define NET_OP_DBG(fmt,args...) AmbaPrintColor(GREEN,fmt,##args);
#else
#define NET_OP_DBG(fmt,args...)
#endif

#undef JSON_ERR
#ifdef NET_OP_ERR_EN
#define NET_OP_ERR(fmt,args...) AmbaPrintColor(RED,fmt,##args);
#else
#define NET_OP_ERR(fmt,args...)
#endif

#define NOT_SYNC_MENU (1) // 0: have to synchronize with menu, 1: don't have to synchronize with menu

#define APP_PREF_DISABLE 0

#define APP_PREF_SETTABLE 0
#define APP_PREF_READONLY 1

#define STR_APP_STATUS              "app_status"
#define STR_PHOTO_CAP_MODE_PRECISE  "precise quality"
#define STR_PHOTO_CAP_MODE_PES      "precise quality cont."
#define STR_PHOTO_CAP_MODE_BURST    "burst quality cont."
#define STR_VIDEO_QUALITY_SFINE     "sfine"
#define STR_VIDEO_QUALITY_FINE      "fine"
#define STR_VIDEO_QUALITY_NORMAL    "normal"
#define STR_PHOTO_TIME_LAPSE_OFF    "off"
#define STR_PHOTO_TIME_LAPSE_500MS  "0.5"
#define STR_PHOTO_TIME_LAPSE_1S     "1"
#define STR_PHOTO_TIME_LAPSE_5S     "5"
#define STR_PHOTO_QUALITY_SFINE     "S.Fine"
#define STR_PHOTO_QUALITY_FINE      "Fine"
#define STR_PHOTO_QUALITY_NORMAL    "Normal"
#define STR_PHOTO_QUALITY_AUTO      "Auto"



#define YEAR_MAX    (2037)
#define YEAR_MIN    (2008)
#define MONTH_MAX   (12)
#define MONTH_MIN   (1)
#define DAY_MIN     (1)
#define HOUR_MAX_24 (23)
#define HOUR_MIN    (0)
#define MINUTE_MAX  (59)
#define MINUTE_MIN  (0)
#define SEC_MAX     (59)
#define SEC_MIN     (0)

typedef enum _NETCTRL_PREF_CONFIG_ID_e_{
    NETCTRL_PREF_VIDEO_QUALITY = 0,
    NETCTRL_PREF_VIDEO_RESOLUTION,
    NETCTRL_PREF_CAP_MODE,
    NETCTRL_PREF_CAMERA_CLOCK,
    NETCTRL_PREF_DEFAULT_SETTING,
    NETCTRL_PREF_PHOTO_TIMELAPSE,
    NETCTRL_PREF_PHOTO_QUALITY,
    NETCTRL_PREF_PHOTO_SIZE,
    NETCTRL_PREF_SETTING_NUM
}NETCTRL_PREF_CONFIG_ID_e;

typedef struct _NETCTRL_PREF_ITEM_s_{
    int Id;
    int Flag;
    char Name[64];
    int Permission;
    MENU_TAB_ID_e TableId;
    int ItemId;
    int (*GetItemValue)(char *Buf, int BufSize);
    int (*GetItemOptions)(int TableIdx);
    int (*SetItemValue)(char *Type, char *Param);
}NETCTRL_PREF_ITEM_s;

typedef struct _NETCTRL_MENU_SEL_s_{
    int Val;
    UINT8 Used;
    char Str[32];
}NETCTRL_MENU_SEL_s;

static NETCTRL_MENU_SEL_s gSensorResTable[MENU_VIDEO_SENSOR_RES_SEL_NUM];
static NETCTRL_MENU_SEL_s gPhotoQualitySelTable[MENU_PHOTO_QUALITY_SEL_NUM];
static NETCTRL_MENU_SEL_s gPhotoSizeSelTable[MENU_PHOTO_SIZE_SEL_NUM];


#define THUMB_BUF_SIZE (2 << 20)


/*----------------------------------------------------------------------*/
/* App Function Declarations (static)                                                                     */
/*----------------------------------------------------------------------*/
static int rec_cam_netctrl_record_start(void);
static int rec_cam_netctrl_record_stop(void);
static int rec_cam_netctrl_get_record_time(void);
static int rec_cam_netctrl_capture(void);
static int rec_cam_netctrl_continue_capture_stop(void);
static int rec_cam_netctrl_refresh_pref_table(void);
static int rec_cam_netctrl_get_all_cur_setting(void);
static int rec_cam_netctrl_get_setting_options(UINT32 Param1, UINT32 Param2);
static int rec_cam_netctrl_get_setting(UINT32 Param1, UINT32 Param2);
static int rec_cam_netctrl_set_setting(UINT32 Param1, UINT32 Param2);
static int rec_cam_netctrl_get_numb_files(UINT32 Param1, UINT32 Param2);
static int rec_cam_netctrl_get_device_info(void);
static int rec_cam_netctrl_vf_reset(void);
static int rec_cam_netctrl_vf_stop(void);
static int rec_cam_netctrl_get_thumb(UINT32 Param1, UINT32 Param2);
static int rec_cam_netctrl_get_media_info(UINT32 Param1, UINT32 Param2);
static int rec_cam_netctrl_format(UINT32 Param1, UINT32 Param2);
static int rec_cam_netctrl_format_done(UINT32 Param1, UINT32 Param2);
static int rec_cam_netctrl_get_space(UINT32 Param1, UINT32 Param2);
static int rec_cam_netctrl_custom_cmd(UINT32 Param1, UINT32 Param2);


/*----------------------------------------------------------------------*/
/* Preference Function Declarations                                                                     */
/*----------------------------------------------------------------------*/
static int rec_cam_get_app_status(char *Buf, int BufSize);
static int rec_cam_get_capture_mode(char *Buf, int BufSize);
static int rec_cam_get_capture_mode_options(int TableIdx);
static int rec_cam_set_capture_mode(char *Type, char *Param);
static int rec_cam_get_time(char *Buf, int BufSize);
static int rec_cam_get_time_options(int TableIdx);
static int rec_cam_set_time(char *Type, char *Param);
static int rec_cam_get_video_quality(char *Buf, int BufSize);
static int rec_cam_get_video_quality_options(int TableIdx);
static int rec_cam_set_video_quality(char *Type, char *Param);
static int rec_cam_get_sensor_resolution(char *Buf, int BufSize);
static int rec_cam_get_sensor_resolution_options(int TableIdx);
static int rec_cam_set_sensor_resolution(char *Type, char *Param);
static int rec_cam_get_default_setting_options(int TableIdx);
static int rec_cam_set_default_setting(char *Type, char *Param);
static int rec_cam_get_timelapse(char *Buf, int BufSize);
static int rec_cam_get_timelapse_options(int TableIdx);
static int rec_cam_set_timelapse(char *Type, char *Param);
static int rec_cam_get_photo_quality(char *Buf, int BufSize);
static int rec_cam_get_photo_quality_options(int TableIdx);
static int rec_cam_set_photo_quality(char *Type, char *Param);
static int rec_cam_get_photo_size(char *Buf, int BufSize);
static int rec_cam_get_photo_size_options(int TableIdx);
static int rec_cam_set_photo_size(char *Type, char *Param);



REC_CAM_NETCTRL_s rec_cam_netctrl_op = {
    .NetCtrlRecordStart = rec_cam_netctrl_record_start,
    .NetCtrlRecordStop = rec_cam_netctrl_record_stop,
    .NetCtrlGetRecordTime = rec_cam_netctrl_get_record_time,
    .NetCtrlCapture = rec_cam_netctrl_capture,
    .NetCtrlContinueCaptureStop = rec_cam_netctrl_continue_capture_stop,
    .NetCtrlRefreshPrefTable = rec_cam_netctrl_refresh_pref_table,
    .NetCtrlGetAllCurSetting = rec_cam_netctrl_get_all_cur_setting,
    .NetCtrlGetSettingOptions = rec_cam_netctrl_get_setting_options,
    .NetCtrlGetSetting = rec_cam_netctrl_get_setting,
    .NetCtrlSetSetting = rec_cam_netctrl_set_setting,
    .NetCtrlGetNumbFiles = rec_cam_netctrl_get_numb_files,
    .NetCtrlGetDeviceInfo = rec_cam_netctrl_get_device_info,
    .NetCtrlVFReset = rec_cam_netctrl_vf_reset,
    .NetCtrlVFStop = rec_cam_netctrl_vf_stop,
    .NetCtrlGetThumb = rec_cam_netctrl_get_thumb,
    .NetCtrlGetMediaInfo = rec_cam_netctrl_get_media_info,
    .NetCtrlFormat = rec_cam_netctrl_format,
    .NetCtrlFormatDone = rec_cam_netctrl_format_done,
    .NetCtrlGetSpace = rec_cam_netctrl_get_space,
    .NetCtrlCustomCmd = rec_cam_netctrl_custom_cmd,
};

static NETCTRL_PREF_ITEM_s NetCtrlPrefTable[] = {
    [0] = {
        .Id = NETCTRL_PREF_CAP_MODE,
        .Flag = APP_PREF_DISABLE,
        .Name = NETCTRL_STR_CAP_MODE,
        .Permission = APP_PREF_SETTABLE,
        .TableId = MENU_PHOTO,
        .ItemId = MENU_PHOTO_CAP_MODE,
        .GetItemValue = rec_cam_get_capture_mode,
        .GetItemOptions = rec_cam_get_capture_mode_options,
        .SetItemValue = rec_cam_set_capture_mode
    },
    [1] = {
        .Id = NETCTRL_PREF_CAMERA_CLOCK,
        .Flag = APP_PREF_DISABLE,
        .Name = NETCTRL_STR_CAMERA_CLOCK,
        .Permission = APP_PREF_SETTABLE,
        .TableId = MENU_SETUP,
        .ItemId = MENU_SETUP_TIME,
        .GetItemValue = rec_cam_get_time,
        .GetItemOptions = rec_cam_get_time_options,
        .SetItemValue = rec_cam_set_time
    },
    [2] = {
        .Id = NETCTRL_PREF_VIDEO_QUALITY,
        .Flag = APP_PREF_DISABLE,
        .Name = NETCTRL_STR_VIDEO_QUALITY,
        .Permission = APP_PREF_SETTABLE,
        .TableId = MENU_VIDEO,
        .ItemId = MENU_VIDEO_QUALITY,
        .GetItemValue = rec_cam_get_video_quality,
        .GetItemOptions = rec_cam_get_video_quality_options,
        .SetItemValue = rec_cam_set_video_quality
    },
    [3] = {
        .Id = NETCTRL_PREF_VIDEO_RESOLUTION,
        .Flag = APP_PREF_DISABLE,
        .Name = NETCTRL_STR_VIDEO_RESOLUTION,
        .Permission = APP_PREF_SETTABLE,
        .TableId = MENU_VIDEO,
        .ItemId = MENU_VIDEO_SENSOR_RES,
        .GetItemValue = rec_cam_get_sensor_resolution,
        .GetItemOptions = rec_cam_get_sensor_resolution_options,
        .SetItemValue = rec_cam_set_sensor_resolution
    },
    [4] = {
        .Id = NETCTRL_PREF_DEFAULT_SETTING,
        .Flag = APP_PREF_DISABLE,
        .Name = NETCTRL_STR_DEFAULT_SETTING,
        .Permission = APP_PREF_SETTABLE,
        .TableId = MENU_SETUP,
        .ItemId = MENU_SETUP_DEFAULT,
        .GetItemValue = NULL,
        .GetItemOptions = rec_cam_get_default_setting_options,
        .SetItemValue = rec_cam_set_default_setting
    },
    [5] = {
        .Id = NETCTRL_PREF_PHOTO_TIMELAPSE,
        .Flag = APP_PREF_DISABLE,
        .Name = NETCTRL_STR_PHOTO_TIMELAPSE,
        .Permission = APP_PREF_SETTABLE,
        .TableId = MENU_PHOTO,
        .ItemId = MENU_PHOTO_TIME_LAPSE,
        .GetItemValue = rec_cam_get_timelapse,
        .GetItemOptions = rec_cam_get_timelapse_options,
        .SetItemValue = rec_cam_set_timelapse
    },
    [6] = {
        .Id = NETCTRL_PREF_PHOTO_QUALITY,
        .Flag = APP_PREF_DISABLE,
        .Name = NETCTRL_STR_PHOTO_QUALITY,
        .Permission = APP_PREF_SETTABLE,
        .TableId = MENU_PHOTO,
        .ItemId = MENU_PHOTO_QUALITY,
        .GetItemValue = rec_cam_get_photo_quality,
        .GetItemOptions = rec_cam_get_photo_quality_options,
        .SetItemValue = rec_cam_set_photo_quality
    },
    [7] = {
        .Id = NETCTRL_PREF_PHOTO_SIZE,
        .Flag = APP_PREF_DISABLE,
        .Name = NETCTRL_STR_PHOTO_SIZE,
        .Permission = APP_PREF_SETTABLE,
        .TableId = MENU_PHOTO,
        .ItemId = MENU_PHOTO_SIZE,
        .GetItemValue = rec_cam_get_photo_size,
        .GetItemOptions = rec_cam_get_photo_size_options,
        .SetItemValue = rec_cam_set_photo_size
    }
};

#define PREF_TABLE_SIZE (sizeof(NetCtrlPrefTable)/sizeof(NetCtrlPrefTable[0]))

static int SendJsonString(APPLIB_JSON_OBJECT *JsonObject)
{
    APPLIB_JSON_STRING *JsonString = NULL;
    char *ReplyString = NULL;

    AppLibNetJsonUtility_JsonObjectToJsonString(JsonObject, &JsonString);
    AppLibNetJsonUtility_GetString(JsonString, &ReplyString);
    if (ReplyString) {
        AppLibNetControl_ReplyToLnx(ReplyString, strlen(ReplyString));
    }
    AppLibNetJsonUtility_FreeJsonString(JsonString);

    return 0;
}

static int GetDayMax(int year, int month)
{
    int Day = 31;
    switch (month) {
    case 2:
        /// Leap year check.
        if (year%400==0 || (year%100!=0 && year%4==0)) {
            Day = 29;
        } else {
            Day = 28;
        }
        break;
    case 4:
    case 6:
    case 9:
    case 11:
        Day = 30;
        break;
    default:
        Day = 31;
        break;
    }
    return Day;
}


static int ParseCamaraClockString(char *pTime, AMBA_RTC_TIME_SPEC_u *pTimeSpec)
{
    char *pParam = pTime;
    UINT32 Year = 0;
    UINT32 Month = 0;
    UINT32 Day = 0;
    UINT32 Hour = HOUR_MAX_24 + 1;
    UINT32 Minute = MINUTE_MAX + 1;
    UINT32 Second = SEC_MAX + 1;
    char *pTemp = NULL;

    if ((!pTime) || (!pTimeSpec)) {
       NET_OP_ERR("[rec_cam] <ParseCamaraClockString> invalid parameter");
       return -1;
    }

    NET_OP_DBG("[rec_cam] <ParseCamaraClockString> pTime = %s",pTime);
    memset(pTimeSpec, 0, sizeof(AMBA_RTC_TIME_SPEC_u));

    pTemp = strtok(pParam, "-");
    if (pTemp == NULL) {
        return -1;
    }
    Year = (UINT32) atoi(pTemp);

    pTemp = strtok(NULL, "-");
    if (pTemp == NULL) {
        return -1;
    }
    Month = (UINT32) atoi(pTemp);

    pTemp = strtok(NULL, " ");
    if (pTemp == NULL) {
        return -1;
    }
    Day = (UINT32) atoi(pTemp);

    pTemp = strtok(NULL,":");
    if( pTemp==NULL ) {
        return -1;
    }
    Hour = (UINT32) atoi(pTemp);

    pTemp = strtok(NULL,":");
    if (pTemp == NULL) {
        return -1;
    }
    Minute= (UINT32) atoi(pTemp);
    pTemp = strtok(NULL,":");
    if (pTemp==NULL) {
        return -1;
    }
    Second= (UINT32) atoi(pTemp);

    NET_OP_DBG("%04u-%02u-%02u %02u:%02u:%02u",Year, Month, Day, Hour, Minute, Second);

    if((Second > SEC_MAX) || (Minute > MINUTE_MAX) || (Hour > HOUR_MAX_24)) {
        return -2;
    }

    if((Second < SEC_MIN) || (Minute < MINUTE_MIN) || (Hour < HOUR_MIN)) {
        return -2;
    }

    if((Year > YEAR_MAX) || (Year < YEAR_MIN) || (Month > MONTH_MAX) || (Month < MONTH_MIN) || (Day < DAY_MIN)) {
        return -2;
    }

    if(Day > GetDayMax(Year, Month)) {
        return -2;
    }

    pTimeSpec->Calendar.Year = Year;
    pTimeSpec->Calendar.Month = Month;
    pTimeSpec->Calendar.Day = Day;
    pTimeSpec->Calendar.Hour= Hour;
    pTimeSpec->Calendar.Minute = Minute;
    pTimeSpec->Calendar.Second = Second;

    NET_OP_DBG("[rec_cam] <ParseCamaraClockString> OK!");

    return 0;
}

static int ConvertFilePathFormat(char *pFilename, char *pRetFilename, int RetFilenameBufSize)
{
    char *pLnxMountPoint = "/tmp/SD0";
    char *pStrFound = NULL;
    int Len = 0;
    char Cwd[64] = {0};
    char Fullpath[64] = {0};
    char *pTemp = NULL;

    if ((!pFilename) || (!pRetFilename) || (RetFilenameBufSize == 0)) {
        return-1;
    }

    // generate filename in full path fashion
    if (pFilename[0] != '/') {
        AppLibNetControl_GetCurrentWorkDir(Cwd, sizeof(Cwd));
        snprintf(Fullpath, sizeof(Fullpath), "%s/%s", Cwd, pFilename);
    } else {
        snprintf(Fullpath, sizeof(Fullpath), "%s", pFilename);
    }

    pStrFound = strstr(Fullpath, pLnxMountPoint);
    if (pStrFound) {
        snprintf(pRetFilename, RetFilenameBufSize, "C:%s", pStrFound+strlen(pLnxMountPoint));
    } else {
        snprintf(pRetFilename, RetFilenameBufSize, "%s", pFilename);
    }

    Len = strlen(pRetFilename);
    pTemp = pRetFilename;
    while (Len--) {
        if(*pTemp == '/'){
            *pTemp = '\\';
        }

        pTemp++;
    }

    AmbaPrintColor(CYAN, "<ConvertFilePathPrefix> pFilename = -%s-, pRetFilename = -%s-", pFilename, pRetFilename);
    return 0;
}

/*
* pfilename should be full path
* return 1: file exist
* return 0: file does not exist
*/
static int CheckFileExist(char *pFilename)
{
    AMP_CFS_STAT Status = {0};
    int ReturnValue = 0;

    if (!pFilename) {
        return 0;
    }

    ReturnValue = AmpCFS_Stat(pFilename, &Status);
    if (ReturnValue != AMP_OK) {
        return 0;
    }

    AmbaPrint("file: %s, Attr: 0x%x", pFilename, Status.Attr);

    return 1;
}

static int FormatCard(int SlotId)
{
    int ReturnValue = 0;
    int ErrorCode = 0;

    ReturnValue = AppLibCard_CheckStatus(CARD_CHECK_DELETE);

    if ((ReturnValue == 0) || (ReturnValue == CARD_STATUS_UNFORMAT_CARD) ||
        (ReturnValue == CARD_STATUS_INVALID_CARD) || (ReturnValue == CARD_STATUS_INVALID_FORMAT_CARD) ||
        (ReturnValue == CARD_STATUS_NOT_ENOUGH_SPACE)) {

        NET_OP_DBG("[rec_cam] <CheckCardStatus> CARD_OK");
        app_status.CardFmtParam = 0;
        AppLibComSvcAsyncOp_CardFormat(SlotId);
        ErrorCode = 0;
    } else {
        /** Can not format card.*/
        app_status.CardFmtParam = 0;

        if (ReturnValue == CARD_STATUS_NO_CARD) {
            NET_OP_ERR("[rec_cam] <CheckCardStatus> WARNING_NO_CARD");
            ErrorCode = ERROR_NETCTRL_INVALID_OPERATION;
        } else if (ReturnValue == CARD_STATUS_WP_CARD) {
            NET_OP_ERR("[rec_cam] <CheckCardStatus> WARNING_CARD_PROTECTED");
            ErrorCode = ERROR_NETCTRL_CARD_PROTECTED;
        } else {
            NET_OP_ERR("[rec_cam] <CheckCardStatus> WARNING_CARD_Error %d",ReturnValue);
            ErrorCode = ERROR_NETCTRL_INVALID_OPERATION;
        }
    }

    return ErrorCode;
}

static int BuuldUpSensorResSelTable(void)
{
    int ResNum = 0;
    int SensorID = 0;
    UINT16 *Str = NULL;
    int i = 0;

    /* Build up sensor resolution table */
    memset(gSensorResTable, 0, sizeof(gSensorResTable));
    ResNum = AppLibSysSensor_GetVideoResNum();
    for (i=0; i<MENU_VIDEO_SENSOR_RES_SEL_NUM; i++) {
        if (i < ResNum) {
            SensorID = AppLibSysSensor_GetVideoResID(i);
            Str = AppLibSysSensor_GetVideoResStr(SensorID);
            if (Str) {
                AmbaUtility_Unicode2Ascii(Str, gSensorResTable[i].Str);
                gSensorResTable[i].Val = SensorID;
                gSensorResTable[i].Used = 1;
            } else {
                gSensorResTable[i].Used = 0;
            }

        } else {
            gSensorResTable[i].Used = 0;
        }
    }

#if 0
    AmbaPrintColor(CYAN,"ResNum = %d",ResNum);
    for (i=0;i<MENU_VIDEO_SENSOR_RES_SEL_NUM;i++) {
        AmbaPrintColor(CYAN,"[%d] Used = %d, Val = %d, Str = %s", i,gSensorResTable[i].Used, gSensorResTable[i].Val, gSensorResTable[i].Str);
    }
#endif

    return 0;
}


static int BuildUpPhotoQualitySelTable(void)
{
    MENU_SEL_s *pSel = NULL;
    int i = 0;
    int SelNum = 0;

    memset(gPhotoQualitySelTable, 0, sizeof(gPhotoQualitySelTable));

    for (i=0;i<MENU_PHOTO_QUALITY_SEL_NUM;i++) {
        pSel = AppMenu_GetSel(MENU_PHOTO, MENU_PHOTO_QUALITY, i);
        if (pSel) {
            if (APP_CHECKFLAGS(pSel->Flags, MENU_SEL_FLAGS_ENABLE)) {
                gPhotoQualitySelTable[SelNum].Val = pSel->Val;
                gPhotoQualitySelTable[SelNum].Used = 1;

                switch (pSel->Val) {
                case PHOTO_QUALITY_SFINE:
                    snprintf(gPhotoQualitySelTable[SelNum].Str, sizeof(gPhotoQualitySelTable[SelNum].Str), STR_PHOTO_QUALITY_SFINE);
                    break;
                case PHOTO_QUALITY_FINE:
                    snprintf(gPhotoQualitySelTable[SelNum].Str, sizeof(gPhotoQualitySelTable[SelNum].Str), STR_PHOTO_QUALITY_FINE);
                    break;
                case PHOTO_QUALITY_NORMAL:
                    snprintf(gPhotoQualitySelTable[SelNum].Str, sizeof(gPhotoQualitySelTable[SelNum].Str), STR_PHOTO_QUALITY_NORMAL);
                    break;
                case PHOTO_QUALITY_AUTO:
                    snprintf(gPhotoQualitySelTable[SelNum].Str, sizeof(gPhotoQualitySelTable[SelNum].Str), STR_PHOTO_QUALITY_AUTO);
                    break;
                default:
                    snprintf(gPhotoQualitySelTable[SelNum].Str, sizeof(gPhotoQualitySelTable[SelNum].Str),"Unknow");
                    break;
                }

                SelNum ++;
            }
        }
    }

#if 0
    AmbaPrintColor(CYAN,"SelNum = %d",SelNum);
    for (i=0;i<MENU_PHOTO_QUALITY_SEL_NUM;i++) {
        AmbaPrintColor(CYAN,"[%d] Used = %d, Val = %d, Str = %s", i,gPhotoQualitySelTable[i].Used, gPhotoQualitySelTable[i].Val, gPhotoQualitySelTable[i].Str);
    }
#endif

    return 0;
}

static int BuildUpPhotoSizeSelTable(void)
{
    int PjpegConfigNum = 0;
    int i = 0;
    UINT16 *Str = NULL;

    memset(gPhotoSizeSelTable, 0, sizeof(gPhotoSizeSelTable));

    PjpegConfigNum = AppLibSysSensor_GetPjpegConfigNum(AppLibStillEnc_GetPhotoPjpegCapMode());
    AmbaPrint("<%s> L%d PjpegConfigNum = %d", __FUNCTION__, __LINE__, PjpegConfigNum);

    for (i=0; i<MENU_PHOTO_SIZE_SEL_NUM; i++) {
        if (i < PjpegConfigNum) {
            Str = AppLibSysSensor_GetPhotoSizeStr(AppLibStillEnc_GetPhotoPjpegCapMode(), i);
            if (Str) {
                AmbaUtility_Unicode2Ascii(Str, gPhotoSizeSelTable[i].Str);
                gPhotoSizeSelTable[i].Val = i;
                gPhotoSizeSelTable[i].Used = 1;
            } else {
                gPhotoSizeSelTable[i].Used = 0;
            }
        }
    }

#if 0
    AmbaPrintColor(CYAN,"PjpegConfigNum = %d",PjpegConfigNum);
    for (i=0;i<MENU_PHOTO_SIZE_SEL_NUM;i++) {
        AmbaPrintColor(CYAN,"[%d] Used = %d, Val = %d, Str = %s", i,gPhotoSizeSelTable[i].Used, gPhotoSizeSelTable[i].Val, gPhotoSizeSelTable[i].Str);
    }
#endif

    return 0;
}


static int rec_cam_netctrl_record_start(void)
{
    int ErrorCode = 0;
    int ReturnValue = -1;
    APPLIB_JSON_OBJECT *JsonObject = NULL;

    NET_OP_DBG("[rec_cam] <netctrl_record_start>");

    if (app_status.CurrEncMode != APP_VIDEO_ENC_MODE) {
        NET_OP_DBG("[rec_cam] <netctrl_record_start> It's not in video encode mode");
        AppLibNetControl_ReplyErrorCode(AMBA_RECORD_START, ERROR_NETCTRL_INVALID_OPERATION);
        return -1;
    }

    /* Close the menu or dialog. */
    AppWidget_Off(WIDGET_ALL, 0);

    if (rec_cam.RecCapState == REC_CAP_STATE_PREVIEW) {
        /* Check the card's status. */
        ReturnValue = rec_cam.Func(REC_CAM_CARD_CHECK_STATUS, 0, 0);
        if (ReturnValue == 0) {
            /* To record the clip if the card is ready. */
            ReturnValue = rec_cam.Func(REC_CAM_RECORD_START, 0, 0);
        }
    } else if (rec_cam.RecCapState == REC_CAP_STATE_VF){
        ReturnValue = rec_cam.Func(REC_CAM_VF_SWITCH_TO_RECORD, 0, 0);
    } else {
        NET_OP_ERR("[rec_cam] <netctrl_record_start> invalid state for starting record (state = %d)",rec_cam.RecCapState);
        ReturnValue = -1;
    }


    NET_OP_DBG("[rec_cam] <netctrl_record_start> start record %s",(ReturnValue) ? "fail" : "successfully");

    if (AppLibNetJsonUtility_CreateObject(&JsonObject) == 0) {
        ErrorCode = ReturnValue ? ERROR_NETCTRL_INVALID_OPERATION : 0;
        AppLibNetJsonUtility_AddIntegerObject(JsonObject, "rval", ErrorCode);
        AppLibNetJsonUtility_AddIntegerObject(JsonObject, "msg_id", AMBA_RECORD_START);
        SendJsonString(JsonObject);

        AppLibNetJsonUtility_FreeJsonObject(JsonObject);
    } else {
        AppLibNetControl_ReplyErrorCode(AMBA_RECORD_START, ERROR_NETCTRL_UNKNOWN_ERROR);
    }

    return ReturnValue;
}

static int rec_cam_netctrl_record_stop(void)
{
    UINT64 FileObjID = 0;
    char CurFn[APP_MAX_FN_SIZE] = {0};
    int ReturnValue = 0;
    int ErrorCode = 0;
    APPLIB_JSON_OBJECT *JsonObject = NULL;

    if (app_status.CurrEncMode != APP_VIDEO_ENC_MODE) {
        NET_OP_DBG("[rec_cam] <netctrl_record_start> It's not in video encode mode");
        AppLibNetControl_ReplyErrorCode(AMBA_RECORD_STOP, ERROR_NETCTRL_INVALID_OPERATION);
        return -1;
    }

    if (rec_cam.RecCapState != REC_CAP_STATE_RECORD) {
        NET_OP_ERR("[rec_cam] <netctrl_record_stop> It's not recording now. Do nothing.");
        AppLibNetControl_ReplyErrorCode(AMBA_RECORD_STOP, ERROR_NETCTRL_INVALID_OPERATION);
        return -1;
    }

    /* Stop recording. */
    ReturnValue = rec_cam.Func(REC_CAM_RECORD_STOP, 0, 0);
    if (ReturnValue == 0) {
        ReturnValue = -1;
        FileObjID = AppLibStorageDmf_GetLastFilePos(APPLIB_DCF_MEDIA_VIDEO, DCIM_HDLR);
        if (FileObjID > 0) {
            ReturnValue = AppLibStorageDmf_GetFileName(APPLIB_DCF_MEDIA_VIDEO, ".MP4", APPLIB_DCF_EXT_OBJECT_SPLIT_FILE, DCIM_HDLR, 0, FileObjID, CurFn);
            if (ReturnValue == 0) {
                NET_OP_DBG("[rec_cam] <netctrl_record_stop> CurFn: %s", CurFn);
            }
        }
    }

    if (ReturnValue != 0) {
        ErrorCode = ERROR_NETCTRL_UNKNOWN_ERROR;
    }


    if (AppLibNetJsonUtility_CreateObject(&JsonObject) == 0) {
        AppLibNetJsonUtility_AddIntegerObject(JsonObject,"rval", ErrorCode);
        AppLibNetJsonUtility_AddIntegerObject(JsonObject, "msg_id", AMBA_RECORD_STOP);
        AppLibNetJsonUtility_AddStringObject(JsonObject, "param", CurFn);
        SendJsonString(JsonObject);

        AppLibNetJsonUtility_FreeJsonObject(JsonObject);
    } else {
        AppLibNetControl_ReplyErrorCode(AMBA_RECORD_START, ERROR_NETCTRL_JSON_PACKAGE_ERROR);
    }

    return ReturnValue;
}

static int rec_cam_netctrl_get_record_time(void)
{
    APPLIB_JSON_OBJECT *JsonObject = NULL;

    if ((app_status.CurrEncMode != APP_VIDEO_ENC_MODE) || (rec_cam.RecCapState != REC_CAP_STATE_RECORD)) {
        NET_OP_DBG("[rec_cam] <get_record_time> It's not in video encode mode");
        AppLibNetControl_ReplyErrorCode(AMBA_GET_RECORD_TIME, ERROR_NETCTRL_INVALID_OPERATION);
        return -1;
    }

    if (AppLibNetJsonUtility_CreateObject(&JsonObject) == 0) {
        AppLibNetJsonUtility_AddIntegerObject(JsonObject,"rval", 0);
        AppLibNetJsonUtility_AddIntegerObject(JsonObject, "msg_id", AMBA_GET_RECORD_TIME);
        AppLibNetJsonUtility_AddIntegerObject(JsonObject, "param", rec_cam.RecTime);
        SendJsonString(JsonObject);

        AppLibNetJsonUtility_FreeJsonObject(JsonObject);
    } else {
        AppLibNetControl_ReplyErrorCode(AMBA_RECORD_START, ERROR_NETCTRL_JSON_PACKAGE_ERROR);
    }

    return 0;
}

static int rec_cam_netctrl_capture(void)
{
    int ErrorCode = 0;
    int ReturnValue = 0;

    DBGMSGc2(GREEN,"[rec_cam] <netctrl_capture>");

    if (UserSetting->PhotoPref.PhotoCapMode == PHOTO_CAP_MODE_PES) {
        APP_ADDFLAGS(app_rec_cam.Flags, REC_CAM_FLAGS_SHUTTER_PRESSED);
    }

    if (app_rec_cam.Child != 0) {
        AppUtil_SwitchApp(APP_REC_CAM);    // shrink from Child apps
    }
    /* Close the menu or dialog. */
    AppWidget_Off(WIDGET_ALL, 0);

    //-- check the situation that is already in capture operation

    APP_ADDFLAGS(app_rec_cam.Flags,REC_CAM_FLAGS_CAPTURE_FROM_NETCTRL);
    if (rec_cam.RecCapState == REC_CAP_STATE_PREVIEW) {
        /* Check the card's status. */
        ReturnValue = rec_cam.Func(REC_CAM_CARD_CHECK_STATUS, 0, 0);
        if (ReturnValue == 0) {
            /* Capture the photo if the card is ready. */
            if (rec_cam.Func(REC_CAM_CAPTURE, 0, 0) != 0) {
                ErrorCode = ERROR_NETCTRL_INVALID_OPERATION;
            }
        } else {
            ErrorCode = ERROR_NETCTRL_NO_MORE_SPACE;
        }
    } else if (rec_cam.RecCapState == REC_CAP_STATE_VF) {
        /* Check the card's status. */
        ReturnValue = rec_cam.Func(REC_CAM_CARD_CHECK_STATUS, 0, 0);
        if (ReturnValue == 0) {
            NET_OP_DBG("[rec_cam] <netctrl_take_photo> REC_CAM_CAPTURE_ON_VF");
            rec_cam.Func(REC_CAM_CAPTURE_ON_VF, 0, 0);
        } else {
            ErrorCode = ERROR_NETCTRL_NO_MORE_SPACE;
        }
    } else if (rec_cam.RecCapState == REC_CAP_STATE_RECORD) {
        ErrorCode = ERROR_NETCTRL_PIV_NOT_ALLOWED;
    } else {
        ErrorCode = ERROR_NETCTRL_INVALID_OPERATION;
    }

    if (ErrorCode != 0) {
        APP_REMOVEFLAGS(app_rec_cam.Flags,REC_CAM_FLAGS_SHUTTER_PRESSED);
        APP_REMOVEFLAGS(app_rec_cam.Flags,REC_CAM_FLAGS_CAPTURE_FROM_NETCTRL);
        AppLibNetControl_ReplyErrorCode(AMBA_TAKE_PHOTO, ErrorCode);
    } else {
        if ((UserSetting->PhotoPref.PhotoCapMode == PHOTO_CAP_MODE_BURST) ||
                (UserSetting->PhotoPref.PhotoCapMode == PHOTO_CAP_MODE_PES)) {
            AppLibNetControl_ReplyErrorCode(AMBA_TAKE_PHOTO, 0);
        } else {
            /* According to the spec, the reply action should be postponed until the operation
                        of taking photo is completed in PHOTO_CAP_MODE_PRECISE mode. (wait for muxer end.) */
            ;
        }
    }

    return ReturnValue;
}

static int rec_cam_netctrl_continue_capture_stop(void)
{
    if ((UserSetting->PhotoPref.PhotoCapMode != PHOTO_CAP_MODE_PES) || (rec_cam.RecCapState != REC_CAP_STATE_CAPTURE)) {
        AppLibNetControl_ReplyErrorCode(AMBA_CONTINUE_CAPTURE_STOP, ERROR_NETCTRL_INVALID_OPERATION);
    } else {
        APP_REMOVEFLAGS(app_rec_cam.Flags, REC_CAM_FLAGS_SHUTTER_PRESSED);
    }

    return 0;
}


static int rec_cam_netctrl_refresh_pref_table(void)
{
    MENU_ITEM_s *pItem = NULL;
    int i = 0;

    for (i=0;i< PREF_TABLE_SIZE;i++) {
        pItem = AppMenu_GetItem(NetCtrlPrefTable[i].TableId, NetCtrlPrefTable[i].ItemId);
        NetCtrlPrefTable[i].Flag = pItem->Flags;
        NetCtrlPrefTable[i].Permission = (pItem->Flags & MENU_ITEM_FLAGS_LOCKED) ? APP_PREF_READONLY : APP_PREF_SETTABLE;
    }

#if 0
    /* Build up sensor resolution table */
    memset(gSensorResTable, 0, sizeof(gSensorResTable));
    ResNum = AppLibSysSensor_GetVideoResNum();
    for (i=0; i<MENU_VIDEO_SENSOR_RES_SEL_NUM; i++) {
        if (i < ResNum) {
            SensorID = AppLibSysSensor_GetVideoResID(i);
            Str = AppLibSysSensor_GetVideoResStr(SensorID);
            if (Str) {
                AmbaUtility_Unicode2Ascii(Str, gSensorResTable[i].Str);
                gSensorResTable[i].Val = SensorID;
                gSensorResTable[i].Used = 1;
            } else {
                gSensorResTable[i].Used = 0;
            }

        } else {
            gSensorResTable[i].Used = 0;
        }
    }


    AmbaPrintColor(CYAN,"ResNum = %d",ResNum);
    for (i=0;i<MENU_VIDEO_SENSOR_RES_SEL_NUM;i++) {
        AmbaPrintColor(CYAN,"[%d] Used = %d, Val = %d, Str = %s", i,gSensorResTable[i].Used, gSensorResTable[i].Val, gSensorResTable[i].Str);
    }
#endif

    BuuldUpSensorResSelTable();
    BuildUpPhotoQualitySelTable();
    BuildUpPhotoSizeSelTable();

    return 0;
}

static int rec_cam_netctrl_get_all_cur_setting(void)
{
    int i = 0;
    char*pValue = NULL;
    char*pValueRaw = NULL;
    int ValueBufSize = 64;
    APPLIB_JSON_OBJECT *JsonObject = NULL;
    APPLIB_JSON_OBJECT *JsonArrayObject = NULL;
    int ReturnValue = 0;

    ReturnValue = AmpUtil_GetAlignedPool(APPLIB_G_MMPL, (void**)&pValue, (void**)&pValueRaw, ValueBufSize, 32);
    if (ReturnValue < 0) {
        NET_OP_ERR("[rec_cam] <netctrl_get_all_cur_setting> Allocate buffer for json string fail");
        return -1;
    }

    ReturnValue = AppLibNetJsonUtility_CreateObject(&JsonObject);
    if (ReturnValue != 0) {
        AppLibNetControl_ReplyErrorCode(AMBA_GET_ALL_CURRENT_SETTINGS, ERROR_NETCTRL_JSON_PACKAGE_ERROR);
        AmbaKAL_BytePoolFree(pValueRaw);
        return -1;
    }

    ReturnValue = AppLibNetJsonUtility_CreateArrayObject(&JsonArrayObject);
    if (ReturnValue != 0) {
        AppLibNetControl_ReplyErrorCode(AMBA_GET_ALL_CURRENT_SETTINGS, ERROR_NETCTRL_JSON_PACKAGE_ERROR);
        AppLibNetJsonUtility_FreeJsonObject(JsonObject);
        AmbaKAL_BytePoolFree(pValueRaw);
        return -1;
    }

    for (i=0;i<PREF_TABLE_SIZE;i++) {
        if (NOT_SYNC_MENU || APP_CHECKFLAGS(NetCtrlPrefTable[i].Flag, MENU_ITEM_FLAGS_ENABLE)) {
            if (NetCtrlPrefTable[i].GetItemValue != NULL){
                ReturnValue = NetCtrlPrefTable[i].GetItemValue(pValue, ValueBufSize);
                if (ReturnValue != 0) {
                    NET_OP_ERR("[rec_cam] <netctrl_get_all_cur_setting> get item setting fail");
                    AppLibNetControl_ReplyErrorCode(AMBA_GET_ALL_CURRENT_SETTINGS, ERROR_NETCTRL_JSON_PACKAGE_ERROR);
                    AppLibNetJsonUtility_FreeJsonObject(JsonObject);
                    AppLibNetJsonUtility_FreeJsonArrayObject(JsonArrayObject);
                    AmbaKAL_BytePoolFree(pValueRaw);
                    return -1;
                }
            } else {
                snprintf(pValue, ValueBufSize,"n/a");
            }

            AppLibNetJsonUtility_AddStringObject(JsonArrayObject, NetCtrlPrefTable[i].Name, pValue);
        }
        else {
            NET_OP_DBG("[rec_cam] <netctrl_get_all_cur_setting> disable item %s", NetCtrlPrefTable[i].Name);
        }
    }

    AppLibNetJsonUtility_AddIntegerObject(JsonObject, "rval", 0);
    AppLibNetJsonUtility_AddIntegerObject(JsonObject, "msg_id", AMBA_GET_ALL_CURRENT_SETTINGS);
    AppLibNetJsonUtility_AddObject(JsonObject, "param", JsonArrayObject);
    SendJsonString(JsonObject);
    AppLibNetJsonUtility_FreeJsonArrayObject(JsonArrayObject);
    AppLibNetJsonUtility_FreeJsonObject(JsonObject);

    AmbaKAL_BytePoolFree(pValueRaw);

    return 0;
}

static int rec_cam_netctrl_get_setting_options(UINT32 Param1, UINT32 Param2)
{
    char *pJsonStringBuf = (char *)Param1;
    char *pJsonStringBufRaw = (char *)Param2;
    APPLIB_JSON_OBJECT *pJsonObject = NULL;
    char Param[64] = {0};
    int i = 0;
    int ReturnValue = 0;

    if ((!pJsonStringBuf) || (!pJsonStringBufRaw)) {
        NET_OP_ERR("[rec_cam] <get_setting_options> cmd argument buffer is NULL");
        AppLibNetControl_ReplyErrorCode(AMBA_GET_SINGLE_SETTING_OPTIONS, ERROR_NETCTRL_JSON_PACKAGE_ERROR);
        return -1;
    }

    ReturnValue = AppLibNetJsonUtility_JsonStringToJsonObject(&pJsonObject, pJsonStringBuf);
    if (ReturnValue != 0) {
        AppLibNetControl_ReplyErrorCode(AMBA_GET_SINGLE_SETTING_OPTIONS, ERROR_NETCTRL_JSON_PACKAGE_ERROR);
        AmbaKAL_BytePoolFree(pJsonStringBufRaw);
        return -1;
    }

    ReturnValue = AppLibNetJsonUtility_GetStringByKey(pJsonObject,"param", Param, sizeof(Param));
    if (ReturnValue != 0) {
        AppLibNetControl_ReplyErrorCode(AMBA_GET_SINGLE_SETTING_OPTIONS, ERROR_NETCTRL_JSON_PACKAGE_ERROR);
        AppLibNetJsonUtility_FreeJsonObject(pJsonObject);
        AmbaKAL_BytePoolFree(pJsonStringBufRaw);
        return -1;
    }

    /* Param1 is Pref config ID*/
    for (i=0;i<PREF_TABLE_SIZE;i++) {
        if (strcasecmp(NetCtrlPrefTable[i].Name, Param) == 0) {
            NetCtrlPrefTable[i].GetItemOptions(i);
            return 0;
        }
    }


    NET_OP_ERR("No preference matched!");

    AmbaKAL_BytePoolFree(pJsonStringBufRaw);
    return -1;
}

static int rec_cam_netctrl_get_setting(UINT32 Param1, UINT32 Param2)
{
    char *pJsonStringBuf = (char *)Param1;
    char *pJsonStringBufRaw = (char *)Param2;
    APPLIB_JSON_OBJECT *pJsonObject = NULL;
    APPLIB_JSON_OBJECT *pJsonObjectReply = NULL;
    char Type[64] = {0};
    char Param[64] = {0};
    int i = 0;
    int ReturnValue = 0;

    if ((!pJsonStringBuf) || (!pJsonStringBufRaw)) {
        NET_OP_ERR("[rec_cam] <netctrl_get_setting> cmd argument buffer is NULL");
        AppLibNetControl_ReplyErrorCode(AMBA_GET_SETTING, ERROR_NETCTRL_JSON_PACKAGE_ERROR);
        return -1;
    }

    ReturnValue = AppLibNetJsonUtility_JsonStringToJsonObject(&pJsonObject, pJsonStringBuf);
    if (ReturnValue != 0) {
        AppLibNetControl_ReplyErrorCode(AMBA_GET_SETTING, ERROR_NETCTRL_JSON_PACKAGE_ERROR);
        AmbaKAL_BytePoolFree(pJsonStringBufRaw);
        return -1;
    }

    ReturnValue = AppLibNetJsonUtility_GetStringByKey(pJsonObject,"type", Type, sizeof(Type));
    if (ReturnValue != 0) {
        AppLibNetControl_ReplyErrorCode(AMBA_GET_SETTING, ERROR_NETCTRL_JSON_PACKAGE_ERROR);
        AppLibNetJsonUtility_FreeJsonObject(pJsonObject);
        AmbaKAL_BytePoolFree(pJsonStringBufRaw);
        return -1;
    }

    if (strcasecmp(Type, STR_APP_STATUS) == 0) {
        ReturnValue = rec_cam_get_app_status(Param, sizeof(Param));
        if (ReturnValue != 0) {
            AppLibNetControl_ReplyErrorCode(AMBA_GET_SETTING, ERROR_NETCTRL_UNKNOWN_ERROR);
            AppLibNetJsonUtility_FreeJsonObject(pJsonObject);
            AmbaKAL_BytePoolFree(pJsonStringBufRaw);
            return -1;
        }
    } else {
        for (i=0;i<PREF_TABLE_SIZE;i++) {
            if (strcasecmp(Type, NetCtrlPrefTable[i].Name) == 0) {
                if (NetCtrlPrefTable[i].GetItemValue != NULL){
                    ReturnValue = NetCtrlPrefTable[i].GetItemValue(Param, sizeof(Param));
                } else {
                    AppLibNetControl_ReplyErrorCode(AMBA_GET_SETTING, ERROR_NETCTRL_OPERATION_UNSUPPORTED);
                    AppLibNetJsonUtility_FreeJsonObject(pJsonObject);
                    AmbaKAL_BytePoolFree(pJsonStringBufRaw);
                    return -1;
                }
                break;
            }
        }

        if (i >= PREF_TABLE_SIZE) {
            NET_OP_ERR("[rec_cam] <netctrl_get_setting> no matched type");
            AppLibNetControl_ReplyErrorCode(AMBA_GET_SETTING, ERROR_NETCTRL_INVALID_OPTION_VALUE);
            AppLibNetJsonUtility_FreeJsonObject(pJsonObject);
            AmbaKAL_BytePoolFree(pJsonStringBufRaw);
            return -1;
        }
    }

    AppLibNetJsonUtility_CreateObject(&pJsonObjectReply);
    AppLibNetJsonUtility_AddIntegerObject(pJsonObjectReply, "rval", 0);
    AppLibNetJsonUtility_AddIntegerObject(pJsonObjectReply, "msg_id", AMBA_GET_SETTING);
    AppLibNetJsonUtility_AddStringObject(pJsonObjectReply, "type", Type);
    AppLibNetJsonUtility_AddStringObject(pJsonObjectReply, "param", Param);
    SendJsonString(pJsonObjectReply);

    AppLibNetJsonUtility_FreeJsonObject(pJsonObject);
    AppLibNetJsonUtility_FreeJsonObject(pJsonObjectReply);

    AmbaKAL_BytePoolFree(pJsonStringBufRaw);

    return 0;
}

static int rec_cam_netctrl_set_setting(UINT32 Param1, UINT32 Param2)
{
    char *pJsonStringBuf = (char *)Param1;
    char *pJsonStringBufRaw = (char *)Param2;
    APPLIB_JSON_OBJECT *pJsonObject = NULL;
    char Type[64] = {0};
    char Param[64] = {0};
    int i = 0;
    int ReturnValue = 0;

    if ((!pJsonStringBuf) || (!pJsonStringBufRaw)) {
        NET_OP_ERR("[rec_cam] <netctrl_set_setting> cmd argument buffer is NULL");
        AppLibNetControl_ReplyErrorCode(AMBA_SET_SETTING, ERROR_NETCTRL_JSON_PACKAGE_ERROR);
        return -1;
    }

    ReturnValue = AppLibNetJsonUtility_JsonStringToJsonObject(&pJsonObject, pJsonStringBuf);
    if (ReturnValue != 0) {
        AppLibNetControl_ReplyErrorCode(AMBA_SET_SETTING, ERROR_NETCTRL_JSON_PACKAGE_ERROR);
        AmbaKAL_BytePoolFree(pJsonStringBufRaw);
        return -1;
    }

    ReturnValue = AppLibNetJsonUtility_GetStringByKey(pJsonObject,"type", Type, sizeof(Type));
    if (ReturnValue != 0) {
        NET_OP_ERR("[rec_cam] <netctrl_set_setting> parse type fail");
        AppLibNetControl_ReplyErrorCode(AMBA_SET_SETTING, ERROR_NETCTRL_JSON_PACKAGE_ERROR);
        AppLibNetJsonUtility_FreeJsonObject(pJsonObject);
        AmbaKAL_BytePoolFree(pJsonStringBufRaw);
        return -1;
    }

    ReturnValue = AppLibNetJsonUtility_GetStringByKey(pJsonObject,"param", Param, sizeof(Param));
    if (ReturnValue != 0) {
        NET_OP_ERR("[rec_cam] <netctrl_set_setting> parse param fail");
        AppLibNetControl_ReplyErrorCode(AMBA_SET_SETTING, ERROR_NETCTRL_JSON_PACKAGE_ERROR);
        AppLibNetJsonUtility_FreeJsonObject(pJsonObject);
        AmbaKAL_BytePoolFree(pJsonStringBufRaw);
        return -1;
    }

    for (i=0;i<PREF_TABLE_SIZE;i++) {
        if (strcasecmp(Type, NetCtrlPrefTable[i].Name) == 0) {
            ReturnValue = NetCtrlPrefTable[i].SetItemValue(Type, Param);
            break;
        }
    }

    if (i == PREF_TABLE_SIZE) {
        AppLibNetControl_ReplyErrorCode(AMBA_SET_SETTING, ERROR_NETCTRL_UNKNOWN_ERROR);
        ReturnValue = -1;
    }

    AppLibNetJsonUtility_FreeJsonObject(pJsonObject);


    AmbaKAL_BytePoolFree(pJsonStringBufRaw);

    return ReturnValue;
}

static int rec_cam_netctrl_get_numb_files(UINT32 Param1, UINT32 Param2)
{
    char *pJsonStringBuf = (char *)Param1;
    char *pJsonStringBufRaw = (char *)Param2;
    APPLIB_JSON_OBJECT *pJsonObject = NULL;
    APPLIB_JSON_OBJECT *pReplyJsonObject = NULL;
    char Type[64] = {0};
    int FileNum = 0;
    int ReturnValue = 0;

    if ((!pJsonStringBuf) || (!pJsonStringBufRaw)) {
        NET_OP_ERR("[rec_cam] <netctrl_get_numb_files> cmd argument buffer is NULL");
        AppLibNetControl_ReplyErrorCode(AMBA_GET_NUMB_FILES, ERROR_NETCTRL_JSON_PACKAGE_ERROR);
        return -1;
    }

    ReturnValue = AppLibNetJsonUtility_JsonStringToJsonObject(&pJsonObject, pJsonStringBuf);
    if (ReturnValue != 0) {
        AppLibNetControl_ReplyErrorCode(AMBA_GET_NUMB_FILES, ERROR_NETCTRL_JSON_PACKAGE_ERROR);
        AmbaKAL_BytePoolFree(pJsonStringBufRaw);
        return -1;
    }

    ReturnValue = AppLibNetJsonUtility_GetStringByKey(pJsonObject,"type", Type, sizeof(Type));
    if (ReturnValue != 0) {
        AppLibNetControl_ReplyErrorCode(AMBA_GET_NUMB_FILES, ERROR_NETCTRL_JSON_PACKAGE_ERROR);
        AppLibNetJsonUtility_FreeJsonObject(pJsonObject);
        AmbaKAL_BytePoolFree(pJsonStringBufRaw);
        return -1;
    }

    if (strcasecmp(Type , "total") == 0) {
        FileNum = AppLibStorageDmf_GetFileAmount(APPLIB_DCF_MEDIA_DCIM, DCIM_HDLR);
    } else if (strcasecmp(Type , "photo") == 0) {
        FileNum = AppLibStorageDmf_GetFileAmount(APPLIB_DCF_MEDIA_IMAGE, DCIM_HDLR);
    } else if (strcasecmp(Type , "video") == 0){
        FileNum = AppLibStorageDmf_GetFileAmount(APPLIB_DCF_MEDIA_VIDEO, DCIM_HDLR);
    } else {
        AppLibNetControl_ReplyErrorCode(AMBA_GET_NUMB_FILES, ERROR_NETCTRL_JSON_PACKAGE_ERROR);
        AppLibNetJsonUtility_FreeJsonObject(pJsonObject);
        AmbaKAL_BytePoolFree(pJsonStringBufRaw);
        return -1;
    }

    if (FileNum < 0) {
        AppLibNetControl_ReplyErrorCode(AMBA_GET_NUMB_FILES, ERROR_NETCTRL_UNKNOWN_ERROR);
        AppLibNetJsonUtility_FreeJsonObject(pJsonObject);
        AmbaKAL_BytePoolFree(pJsonStringBufRaw);
        return -1;
    }

    AppLibNetJsonUtility_CreateObject(&pReplyJsonObject);
    AppLibNetJsonUtility_AddIntegerObject(pReplyJsonObject, "rval", 0);
    AppLibNetJsonUtility_AddIntegerObject(pReplyJsonObject, "msg_id", AMBA_GET_NUMB_FILES);
    AppLibNetJsonUtility_AddIntegerObject(pReplyJsonObject, "param", FileNum);
    SendJsonString(pReplyJsonObject);

    AppLibNetJsonUtility_FreeJsonObject(pJsonObject);
    AppLibNetJsonUtility_FreeJsonObject(pReplyJsonObject);
    AmbaKAL_BytePoolFree(pJsonStringBufRaw);

    return 0;
}

static int rec_cam_netctrl_get_device_info(void)
{
    APPLIB_JSON_OBJECT *JsonObject = NULL;
    AMBA_VerInfo_s *pVerInfo = NULL;
    char *MediaFolder = "/tmp/SD0/DCIM";
    char *EventFolder = "/tmp/SD0/EVENT";

    pVerInfo = AmbaVer_GetVerInfo("libapp.a");
    if (!pVerInfo) {
        NET_OP_ERR("[rec_cam] <netctrl_get_device_info> AmbaVer_GetVerInfo() fail");
        AppLibNetControl_ReplyErrorCode(AMBA_GET_DEVICEINFO, ERROR_NETCTRL_UNKNOWN_ERROR);
        return -1;
    }

    AppLibNetJsonUtility_CreateObject(&JsonObject);
    AppLibNetJsonUtility_AddIntegerObject(JsonObject, "rval", 0);
    AppLibNetJsonUtility_AddIntegerObject(JsonObject, "msg_id", AMBA_GET_DEVICEINFO);
    AppLibNetJsonUtility_AddStringObject(JsonObject, "brand", "ambarella");
    AppLibNetJsonUtility_AddStringObject(JsonObject, "model", "ambarella");
    AppLibNetJsonUtility_AddStringObject(JsonObject, "chip", "A12");
    AppLibNetJsonUtility_AddStringObject(JsonObject, "app_type", "Car");
    AppLibNetJsonUtility_AddStringObject(JsonObject, "fw_ver", (char *)pVerInfo->CiIdStr);
    AppLibNetJsonUtility_AddStringObject(JsonObject, "api_ver", REMOTE_API_VER);
    AppLibNetJsonUtility_AddStringObject(JsonObject, "media_folder", MediaFolder);
    AppLibNetJsonUtility_AddStringObject(JsonObject, "event_folder", EventFolder);
    AppLibNetJsonUtility_AddStringObject(JsonObject, "sec_posfix", "_thm");
    AppLibNetJsonUtility_AddStringObject(JsonObject, "http", "Disable");

    SendJsonString(JsonObject);
    AppLibNetJsonUtility_FreeJsonObject(JsonObject);

    return 0;
}

static int rec_cam_netctrl_vf_reset(void)
{
    int ReturnValue = 0;

    if (app_status.CurrEncMode != APP_VIDEO_ENC_MODE) {
        NET_OP_DBG("[rec_cam] <netctrl_vf_reset> It's not in video encode mode");
        AppLibNetControl_ReplyErrorCode(AMBA_RESET_VF, ERROR_NETCTRL_INVALID_OPERATION);
        return -1;
    }

    APP_REMOVEFLAGS(rec_cam.NetCtrlFlags, APP_NETCTRL_FLAGS_VF_DISABLE);

    if (rec_cam.RecCapState == REC_CAP_STATE_PREVIEW) {
        ReturnValue = rec_cam.Func(REC_CAM_VF_START, 0, 0);
        if (ReturnValue == 0) {
            APP_ADDFLAGS(rec_cam.NetCtrlFlags, APP_NETCTRL_FLAGS_VF_RESET_DONE);
        } else {
            AppLibNetControl_ReplyErrorCode(AMBA_RESET_VF, ERROR_NETCTRL_SYSTEM_BUSY);
            return -1;
        }
    } else if (rec_cam.RecCapState == REC_CAP_STATE_VF) {
        ReturnValue = rec_cam.Func(REC_CAM_VF_STOP, 0, 0);
        if (ReturnValue == 0) {
            APP_ADDFLAGS(rec_cam.NetCtrlFlags, APP_NETCTRL_FLAGS_VF_RESET_DONE);
        } else {
            AppLibNetControl_ReplyErrorCode(AMBA_RESET_VF, ERROR_NETCTRL_UNKNOWN_ERROR);
            return -1;
        }
    } else {
        NET_OP_ERR("[rec_cam] <netctrl_vf_reset> It isn't in preview mode or view finder mode now");
        AppLibNetControl_ReplyErrorCode(AMBA_RESET_VF, ERROR_NETCTRL_SYSTEM_BUSY);
        return -1;
    }

    return 0;
}

static int rec_cam_netctrl_vf_stop(void)
{
    int ReturnValue = 0;

    if (app_status.CurrEncMode != APP_VIDEO_ENC_MODE) {
        NET_OP_DBG("[rec_cam] <netctrl_vf_reset> It's not in video encode mode");
        AppLibNetControl_ReplyErrorCode(AMBA_RESET_VF, ERROR_NETCTRL_INVALID_OPERATION);
        return -1;
    }

    if (rec_cam.RecCapState == REC_CAP_STATE_PREVIEW) {
        NET_OP_DBG("[rec_cam] <netctrl_vf_stop> It is in preview mode already");
        AppLibNetControl_ReplyErrorCode(AMBA_STOP_VF, 0);
        return -1;
    }

    if (rec_cam.RecCapState != REC_CAP_STATE_VF) {
        NET_OP_ERR("[rec_cam] <netctrl_vf_stop> It isn't in view finder mode now");
        AppLibNetControl_ReplyErrorCode(AMBA_STOP_VF, ERROR_NETCTRL_INVALID_OPERATION);
        return -1;
    }

    APP_ADDFLAGS(rec_cam.NetCtrlFlags, APP_NETCTRL_FLAGS_VF_DISABLE);
    APP_ADDFLAGS(rec_cam.NetCtrlFlags, APP_NETCTRL_FLAGS_VF_STOP_DONE);

    ReturnValue = rec_cam.Func(REC_CAM_VF_STOP, 0, 0);
    if (ReturnValue != 0) {
        AppLibNetControl_ReplyErrorCode(AMBA_STOP_VF, ERROR_NETCTRL_INVALID_OPERATION);
        return -1;
    }

    return ReturnValue;
}

static int rec_cam_netctrl_get_thumb(UINT32 Param1, UINT32 Param2)
{
    char *pJsonStringBuf = (char *)Param1;
    char *pJsonStringBufRaw = (char *)Param2;
    APPLIB_JSON_OBJECT *pJsonObject = NULL;
    static char Param[256] = {0};
    static char FilenameDCF[256] = {0};
    char Type[12] = {0};
    static UINT8 BasicThumbInited = 0;
    static void *ThumbBuf = NULL;
    static void *ThumbBufRaw = NULL;
    TRANS_STILL_DATA_BUF_s DataBuf = {0};
    int ReturnValue = 0;
    int ErrorCode = 0;

     if ((!pJsonStringBuf) || (!pJsonStringBufRaw)) {
        NET_OP_ERR("[rec_cam] <get_thumb> cmd argument buffer is NULL");
        AppLibNetControl_ReplyErrorCode(AMBA_GET_THUMB, ERROR_NETCTRL_JSON_PACKAGE_ERROR);
        return -1;
    }

    ReturnValue = AppLibNetJsonUtility_JsonStringToJsonObject(&pJsonObject, pJsonStringBuf);
    if (ReturnValue != 0) {
        AppLibNetControl_ReplyErrorCode(AMBA_GET_THUMB, ERROR_NETCTRL_JSON_PACKAGE_ERROR);
        AmbaKAL_BytePoolFree(pJsonStringBufRaw);
        return -1;
    }

    ReturnValue = AppLibNetJsonUtility_GetStringByKey(pJsonObject,"param", Param, sizeof(Param));
    if (ReturnValue != 0) {
        NET_OP_ERR("[rec_cam] <get_thumb> parse param fail");
        AppLibNetControl_ReplyErrorCode(AMBA_GET_THUMB, ERROR_NETCTRL_JSON_PACKAGE_ERROR);
        AppLibNetJsonUtility_FreeJsonObject(pJsonObject);
        AmbaKAL_BytePoolFree(pJsonStringBufRaw);
        return -1;
    }

    ConvertFilePathFormat(Param, FilenameDCF, sizeof(FilenameDCF));
    if (CheckFileExist(FilenameDCF) == 0) {
        NET_OP_ERR("[rec_cam] <get_thumb> file doesn't exist");
        AppLibNetControl_ReplyErrorCode(AMBA_GET_THUMB, ERROR_NETCTRL_INVALID_PATH);
        AppLibNetJsonUtility_FreeJsonObject(pJsonObject);
        AmbaKAL_BytePoolFree(pJsonStringBufRaw);
        return -1;
    }

    ReturnValue = AppLibNetJsonUtility_GetStringByKey(pJsonObject,"type", Type, sizeof(Type));
    if (ReturnValue != 0) {
        NET_OP_ERR("[rec_cam] <get_thumb> parse type fail");
        AppLibNetControl_ReplyErrorCode(AMBA_GET_THUMB, ERROR_NETCTRL_JSON_PACKAGE_ERROR);
        AppLibNetJsonUtility_FreeJsonObject(pJsonObject);
        AmbaKAL_BytePoolFree(pJsonStringBufRaw);
        return -1;
    }

    if (BasicThumbInited == 0) {
        ReturnValue = AppLibTranscoderThmBasic_Init();
        if (ReturnValue != 0) {
            NET_OP_ERR("[rec_cam] <get_thumb> AppLibTranscoderThmBasic_Init fail");
            AppLibNetControl_ReplyErrorCode(AMBA_GET_THUMB, ERROR_NETCTRL_UNKNOWN_ERROR);
            AppLibNetJsonUtility_FreeJsonObject(pJsonObject);
            AmbaKAL_BytePoolFree(pJsonStringBufRaw);
            return -1;
        }

        ReturnValue = AmpUtil_GetAlignedPool(APPLIB_G_MMPL, &ThumbBuf, &ThumbBufRaw, THUMB_BUF_SIZE, 32);
        if (ReturnValue != 0) {
            NET_OP_ERR("[rec_cam] <get_thumb> allocate memory fail");
            AppLibNetControl_ReplyErrorCode(AMBA_GET_THUMB, ERROR_NETCTRL_UNKNOWN_ERROR);
            AppLibNetJsonUtility_FreeJsonObject(pJsonObject);
            AmbaKAL_BytePoolFree(pJsonStringBufRaw);
            return -1;
        }

        BasicThumbInited = 1;
    }

    DataBuf.Buf = ThumbBuf;
    DataBuf.BufSize = THUMB_BUF_SIZE;
    DataBuf.RetDataSize = 0;
    if (strcasecmp(Type,"thumb") == 0) {
        ReturnValue = AppLibTranscoderThmBasic_GetImage(FilenameDCF, TRANS_STILL_IMAGE_SOURCE_THUMBNAIL, &DataBuf);
    } else if (strcasecmp(Type,"idr") == 0) {
        ReturnValue = AppLibTranscoderThmBasic_GetIdrFrame(FilenameDCF, &DataBuf);
    } else if (strcasecmp(Type,"fullview") == 0) {
        ReturnValue = AppLibTranscoderThmBasic_GetImage(FilenameDCF, TRANS_STILL_IMAGE_SOURCE_FULL, &DataBuf);
    } else {
        NET_OP_ERR("[rec_cam] <get_thumb> parse type fail");
        AppLibNetControl_ReplyErrorCode(AMBA_GET_THUMB, ERROR_NETCTRL_INVALID_TYPE);
        AppLibNetJsonUtility_FreeJsonObject(pJsonObject);
        AmbaKAL_BytePoolFree(pJsonStringBufRaw);
        return -1;
    }

    if (ReturnValue != 0) {
        NET_OP_ERR("[rec_cam] <get_thumb> get thumb fail");
        AppLibNetControl_ReplyErrorCode(AMBA_GET_THUMB, ERROR_NETCTRL_UNKNOWN_ERROR);
        AppLibNetJsonUtility_FreeJsonObject(pJsonObject);
        AmbaKAL_BytePoolFree(pJsonStringBufRaw);
        return -1;
    }

    ReturnValue = AppLibNetControl_SendThumb(DataBuf.Buf, DataBuf.RetDataSize, Type);
    if (ReturnValue != 0) {
        NET_OP_ERR("[rec_cam] <get_thumb> AppLibNetControl_SendThumb fail");
        ErrorCode = (ReturnValue == -3) ? ERROR_NETCTRL_SYSTEM_BUSY : ERROR_NETCTRL_UNKNOWN_ERROR;
        AppLibNetControl_ReplyErrorCode(AMBA_GET_THUMB, ErrorCode);
        AppLibNetJsonUtility_FreeJsonObject(pJsonObject);
        AmbaKAL_BytePoolFree(pJsonStringBufRaw);
        return -1;
    }

    AmbaKAL_BytePoolFree(pJsonStringBufRaw);

    return 0;
}

static int rec_cam_netctrl_get_media_info(UINT32 Param1, UINT32 Param2)
{
    char *pJsonStringBuf = (char *)Param1;
    char *pJsonStringBufRaw = (char *)Param2;
    APPLIB_JSON_OBJECT *pJsonObject = NULL;
    APPLIB_JSON_OBJECT *pReplyJsonObject = NULL;
    char Type[64] = {0};
    static char Param[256] = {0};
    static char FilenameDCF[256] = {0};
    int TimeUnit = 0;   // 1000: sec, 1: msec
    APPLIB_FILE_FORMAT_e Format = APPLIB_FILE_FORMAT_UNKNOWN;
    APPLIB_MEDIA_INFO_s MediaInfo = {0};
    UINT32 DTS, TimeScale;
    UINT32 Width = 0;
    UINT32 Height = 0;
    UINT64 TotalTime = 0;
    char Res[32] = {0};
    char Date[32] = {0};
    int ReturnValue = 0;

    if ((!pJsonStringBuf) || (!pJsonStringBufRaw)) {
        NET_OP_ERR("[rec_cam] <get_media_info> cmd argument buffer is NULL");
        AppLibNetControl_ReplyErrorCode(AMBA_GET_MEDIAINFO, ERROR_NETCTRL_JSON_PACKAGE_ERROR);
        return -1;
    }

    ReturnValue = AppLibNetJsonUtility_JsonStringToJsonObject(&pJsonObject, pJsonStringBuf);
    if (ReturnValue != 0) {
        AppLibNetControl_ReplyErrorCode(AMBA_GET_MEDIAINFO, ERROR_NETCTRL_JSON_PACKAGE_ERROR);
        AmbaKAL_BytePoolFree(pJsonStringBufRaw);
        return -1;
    }

    TimeUnit = 1000;
    if (strstr(pJsonStringBuf, "type") != NULL) {
        ReturnValue = AppLibNetJsonUtility_GetStringByKey(pJsonObject,"type", Type, sizeof(Type));
        if (ReturnValue == 0) {
            if (strcasecmp(Type,"msec") == 0) {
                TimeUnit = 1;
            }
        }
    }
    //AmbaPrintColor(CYAN,"[%s] TimeUnit = %d", __FUNCTION__, TimeUnit);

    ReturnValue = AppLibNetJsonUtility_GetStringByKey(pJsonObject,"param", Param, sizeof(Param));
    if (ReturnValue != 0) {
        NET_OP_ERR("[rec_cam] <get_media_info> parse param fail");
        AppLibNetControl_ReplyErrorCode(AMBA_GET_MEDIAINFO, ERROR_NETCTRL_JSON_PACKAGE_ERROR);
        AppLibNetJsonUtility_FreeJsonObject(pJsonObject);
        AmbaKAL_BytePoolFree(pJsonStringBufRaw);
        return -1;
    }

    ConvertFilePathFormat(Param, FilenameDCF, sizeof(FilenameDCF));
    if (CheckFileExist(FilenameDCF) == 0) {
        NET_OP_ERR("[rec_cam] <get_media_info> file doesn't exist");
        AppLibNetControl_ReplyErrorCode(AMBA_GET_MEDIAINFO, ERROR_NETCTRL_INVALID_PATH);
        AppLibNetJsonUtility_FreeJsonObject(pJsonObject);
        AmbaKAL_BytePoolFree(pJsonStringBufRaw);
        return -1;
    }

    ReturnValue = AppLibFormat_GetMediaInfo(FilenameDCF, &MediaInfo);
    if (ReturnValue != 0) {
        AppLibNetControl_ReplyErrorCode(AMBA_GET_MEDIAINFO, ERROR_NETCTRL_INVALID_PARAM);
        AmbaKAL_BytePoolFree(pJsonStringBufRaw);
        return -1;
    }

    Format = AppLibFormat_GetFileFormat(FilenameDCF);
    AppLibFormat_GetMediaTimeInfo(FilenameDCF, Date, sizeof(Date));

    ReturnValue = AppLibNetJsonUtility_CreateObject(&pReplyJsonObject);
    if (ReturnValue != 0) {
        AppLibNetControl_ReplyErrorCode(AMBA_GET_MEDIAINFO, ERROR_NETCTRL_JSON_PACKAGE_ERROR);
        AmbaKAL_BytePoolFree(pJsonStringBufRaw);
        return -1;
    }

    switch (Format) {
    case APPLIB_FILE_FORMAT_MP4:
    case APPLIB_FILE_FORMAT_MOV:
        DTS = MediaInfo.MediaInfo.Movie->Track[0].NextDTS;
        TimeScale = MediaInfo.MediaInfo.Movie->Track[0].TimeScale;
        TotalTime = (((UINT64)(DTS) / (TimeScale)) * 1000 + (((UINT64)(DTS) % (TimeScale)) * 1000) /(TimeScale)); //unit msec
        TotalTime /= TimeUnit;
        Width = MediaInfo.MediaInfo.Movie->Track[0].Info.Video.Width;
        Height = MediaInfo.MediaInfo.Movie->Track[0].Info.Video.Height;
        snprintf(Res, sizeof(Res), "%dx%d", Width, Height);
        AppLibNetJsonUtility_AddIntegerObject(pReplyJsonObject, "rval", 0);
        AppLibNetJsonUtility_AddIntegerObject(pReplyJsonObject, "msg_id", AMBA_GET_MEDIAINFO);
        AppLibNetJsonUtility_AddIntegerObject(pReplyJsonObject, "size", MediaInfo.MediaInfo.Movie->Size);
        AppLibNetJsonUtility_AddStringObject(pReplyJsonObject, "date", Date);
        AppLibNetJsonUtility_AddStringObject(pReplyJsonObject,"resolution", Res);
        AppLibNetJsonUtility_AddIntegerObject(pReplyJsonObject, "duration", TotalTime);
        AppLibNetJsonUtility_AddStringObject(pReplyJsonObject, "media_type", "mov");
        SendJsonString(pReplyJsonObject);
        break;
    case APPLIB_FILE_FORMAT_JPG:
    case APPLIB_FILE_FORMAT_THM:
        Width = MediaInfo.MediaInfo.Image->Frame[0].Width;
        Height = MediaInfo.MediaInfo.Image->Frame[0].Height;
        snprintf(Res, sizeof(Res), "%dx%d", Width, Height);
        AppLibNetJsonUtility_AddIntegerObject(pReplyJsonObject, "rval", 0);
        AppLibNetJsonUtility_AddIntegerObject(pReplyJsonObject, "msg_id", AMBA_GET_MEDIAINFO);
        AppLibNetJsonUtility_AddIntegerObject(pReplyJsonObject, "size", MediaInfo.MediaInfo.Image->Size);
        AppLibNetJsonUtility_AddStringObject(pReplyJsonObject, "date", Date);
        AppLibNetJsonUtility_AddStringObject(pReplyJsonObject,"resolution", Res);
        AppLibNetJsonUtility_AddStringObject(pReplyJsonObject, "media_type", "img");
        SendJsonString(pReplyJsonObject);
        break;
    default:
        AppLibNetControl_ReplyErrorCode(AMBA_GET_MEDIAINFO, ERROR_NETCTRL_UNKNOWN_ERROR);
        break;
    }


    AmbaKAL_BytePoolFree(pJsonStringBufRaw);

    return 0;
}



static int rec_cam_netctrl_format(UINT32 Param1, UINT32 Param2)
{
    char *pJsonStringBuf = (char *)Param1;
    char *pJsonStringBufRaw = (char *)Param2;
    APPLIB_JSON_OBJECT *pJsonObject = NULL;
    char Param[4] = {0};
    int Slot = 0;
    int SlotId = -1;
    int ReturnValue = 0;

    if ((!pJsonStringBuf) || (!pJsonStringBufRaw)) {
        NET_OP_ERR("[rec_cam] <netctrl_format> cmd argument buffer is NULL");
        AppLibNetControl_ReplyErrorCode(AMBA_FORMAT, ERROR_NETCTRL_JSON_PACKAGE_ERROR);
        return -1;
    }

    ReturnValue = AppLibNetJsonUtility_JsonStringToJsonObject(&pJsonObject, pJsonStringBuf);
    if (ReturnValue != 0) {
        AppLibNetControl_ReplyErrorCode(AMBA_FORMAT, ERROR_NETCTRL_JSON_PACKAGE_ERROR);
        AmbaKAL_BytePoolFree(pJsonStringBufRaw);
        return -1;
    }

    SlotId = AppLibCard_GetActiveSlot();
    if (SlotId < 0) {
        NET_OP_DBG("[rec_cam] <netctrl_format> no active slot");
        AppLibNetControl_ReplyErrorCode(AMBA_FORMAT, ERROR_NETCTRL_INVALID_OPERATION);
        AppLibNetJsonUtility_FreeJsonObject(pJsonObject);
        AmbaKAL_BytePoolFree(pJsonStringBufRaw);
        return -1;
    }

    ReturnValue = AppLibNetJsonUtility_GetStringByKey(pJsonObject,"param", Param, sizeof(Param));
    if (ReturnValue != 0) {
        NET_OP_ERR("[rec_cam] <netctrl_format> parse param fail");
        AppLibNetControl_ReplyErrorCode(AMBA_FORMAT, ERROR_NETCTRL_JSON_PACKAGE_ERROR);
        AppLibNetJsonUtility_FreeJsonObject(pJsonObject);
        AmbaKAL_BytePoolFree(pJsonStringBufRaw);
        return -1;
    } else {
        Slot = tolower((int)Param[0]);
        NET_OP_DBG("[rec_cam] <netctrl_format> Param: %s, Slot = %c, SlotId = %d", Param, Slot, SlotId);
        if (SlotId != Slot - 'a') {
            NET_OP_ERR("[rec_cam] <netctrl_format> invalid param %s",Param);
            AppLibNetControl_ReplyErrorCode(AMBA_FORMAT, ERROR_NETCTRL_INVALID_PARAM);
            AppLibNetJsonUtility_FreeJsonObject(pJsonObject);
            AmbaKAL_BytePoolFree(pJsonStringBufRaw);
            return -1;
        }
    }

    ReturnValue = FormatCard(SlotId);
    if (ReturnValue != 0) {
        /* Only reply execution result in error case here.*/
        AppLibNetControl_ReplyErrorCode(AMBA_FORMAT, ReturnValue);
    }

    AppLibNetJsonUtility_FreeJsonObject(pJsonObject);
    AmbaKAL_BytePoolFree(pJsonStringBufRaw);

    return 0;
}

static int rec_cam_netctrl_format_done(UINT32 Param1, UINT32 Param2)
{
    char Drive = 'A';
    int ReturnValue = 0;

    if (Param1 < 0) {
        NET_OP_DBG("[rec_cam] <netctrl_format_done> Format card fail");

        /** update card_status.format*/
        ReturnValue = AppLibCard_CheckStatus(CARD_CHECK_RESET);

        AppLibNetControl_ReplyErrorCode(AMBA_FORMAT, ERROR_NETCTRL_UNKNOWN_ERROR);
    } else {
        NET_OP_DBG("[rec_cam] <netctrl_format_done> Format card success");

        /** if active slot is formated, update root status by set root */
        Drive += AppLibCard_GetActiveSlot();
        ReturnValue = AppLibStorageDmf_Refresh(Drive);
        if (ReturnValue < 0) {
            AmbaPrintColor(RED, "[rec_cam] <netctrl_format_done> refresh dmf error!");
        }

        /** update card_status.format*/
        AppLibCard_CheckStatus(CARD_CHECK_RESET);
        AppLibNetControl_ReplyErrorCode(AMBA_FORMAT, 0);
    }

    return 0;
}

static int rec_cam_netctrl_get_space(UINT32 Param1, UINT32 Param2)
{
    char *pJsonStringBuf = (char *)Param1;
    char *pJsonStringBufRaw = (char *)Param2;
    APPLIB_JSON_OBJECT *pJsonObject = NULL;
    APPLIB_JSON_OBJECT *pReplyJsonObject = NULL;
    char Type[8] = {0};
    UINT64 Size = 0;
    char ActiveDrive = 0;
    int ErrorCode = 0;
    int ReturnValue = 0;

    if ((!pJsonStringBuf) || (!pJsonStringBufRaw)) {
        NET_OP_ERR("[rec_cam] <netctrl_custom_cmd> cmd argument buffer is NULL");
        AppLibNetControl_ReplyErrorCode(AMBA_GET_SPACE, ERROR_NETCTRL_JSON_PACKAGE_ERROR);
        return -1;
    }

    ReturnValue = AppLibNetJsonUtility_JsonStringToJsonObject(&pJsonObject, pJsonStringBuf);
    if (ReturnValue != 0) {
        AppLibNetControl_ReplyErrorCode(AMBA_GET_SPACE, ERROR_NETCTRL_JSON_PACKAGE_ERROR);
        AmbaKAL_BytePoolFree(pJsonStringBufRaw);
        return -1;
    }

    ReturnValue = AppLibNetJsonUtility_GetStringByKey(pJsonObject,"type", Type, sizeof(Type));
    if (ReturnValue != 0) {
        NET_OP_ERR("[rec_cam] <netctrl_get_space> parse type fail");
        AppLibNetControl_ReplyErrorCode(AMBA_GET_SPACE, ERROR_NETCTRL_JSON_PACKAGE_ERROR);
        AppLibNetJsonUtility_FreeJsonObject(pJsonObject);
        AmbaKAL_BytePoolFree(pJsonStringBufRaw);
        return -1;
    }

    ErrorCode = 0;
    if(AppLibCard_GetActiveSlot() != -1) {
        ActiveDrive = AppLibCard_GetActiveDrive();
        //NET_OP_DBG("[rec_cam] <netctrl_get_space> ActiveDrive = %d",ActiveDrive);
        if (strcasecmp(Type, "total") == 0) {
            Size = AppLibCard_GetTotalSpace(ActiveDrive) >> 10;
        } else if (strcasecmp(Type, "free") == 0) {
            Size = AppLibCard_GetFreeSpace(ActiveDrive) >> 10; // in KBytes
        } else {
            NET_OP_ERR("[rec_cam] <netctrl_get_space> parse type fail");
            ErrorCode = ERROR_NETCTRL_JSON_PACKAGE_ERROR;
        }
    } else {
        NET_OP_DBG("[rec_cam] <netctrl_get_space> no active drive");
        ErrorCode = ERROR_NETCTRL_UNKNOWN_ERROR;
    }

    //NET_OP_DBG("[rec_cam] <netctrl_get_space> %s %d KBytes",Type, Size);

    ReturnValue = AppLibNetJsonUtility_CreateObject(&pReplyJsonObject);
    if (ReturnValue != 0) {
        AppLibNetControl_ReplyErrorCode(AMBA_GET_SPACE, ERROR_NETCTRL_JSON_PACKAGE_ERROR);
        AppLibNetJsonUtility_FreeJsonObject(pJsonObject);
        AmbaKAL_BytePoolFree(pJsonStringBufRaw);
        return -1;
    }

    AppLibNetJsonUtility_AddIntegerObject(pReplyJsonObject,"rval", ErrorCode);
    AppLibNetJsonUtility_AddIntegerObject(pReplyJsonObject,"msg_id", AMBA_GET_SPACE);
    if (ErrorCode == 0) {
        AppLibNetJsonUtility_AddIntegerObject(pReplyJsonObject,"param", Size);
    }
    SendJsonString(pReplyJsonObject);

    AppLibNetJsonUtility_FreeJsonObject(pReplyJsonObject);
    AppLibNetJsonUtility_FreeJsonObject(pJsonObject);
    AmbaKAL_BytePoolFree(pJsonStringBufRaw);

    return 0;
}

static int rec_cam_netctrl_custom_cmd(UINT32 Param1, UINT32 Param2)
{
    char *pJsonStringBuf = (char *)Param1;
    char *pJsonStringBufRaw = (char *)Param2;
    APPLIB_JSON_OBJECT *pJsonObject = NULL;
    int MsgId = 0;
    int ReturnValue = 0;

    if ((!pJsonStringBuf) || (!pJsonStringBufRaw)) {
        NET_OP_ERR("[rec_cam] <netctrl_custom_cmd> cmd argument buffer is NULL");
        AppLibNetControl_ReplyErrorCode(AMBA_CUSTOM_CMD_BASE, ERROR_NETCTRL_JSON_PACKAGE_ERROR);
        return -1;
    }

    AmbaPrint("[rec_cam] <netctrl_custom_cmd> %s",pJsonStringBuf);

    ReturnValue = AppLibNetJsonUtility_JsonStringToJsonObject(&pJsonObject, pJsonStringBuf);
    if (ReturnValue != 0) {
        AppLibNetControl_ReplyErrorCode(AMBA_CUSTOM_CMD_BASE, ERROR_NETCTRL_JSON_PACKAGE_ERROR);
        AmbaKAL_BytePoolFree(pJsonStringBufRaw);
        return -1;
    }

    ReturnValue = AppLibNetJsonUtility_GetIntegerByKey(pJsonObject,"msg_id", &MsgId);
    if (ReturnValue != 0) {
        NET_OP_ERR("[rec_cam] <netctrl_custom_cmd> parse msg id fail");
        AppLibNetControl_ReplyErrorCode(AMBA_CUSTOM_CMD_BASE, ERROR_NETCTRL_JSON_PACKAGE_ERROR);
        AppLibNetJsonUtility_FreeJsonObject(pJsonObject);
        AmbaKAL_BytePoolFree(pJsonStringBufRaw);
        return -1;
    }

    AppLibNetControl_ReplyErrorCode(MsgId, 0);

    AmbaKAL_BytePoolFree(pJsonStringBufRaw);

    return 0;
}


/*----------------------------------------------------------------------*/
/* Preference Function                                                                                        */
/*----------------------------------------------------------------------*/
static int rec_cam_get_app_status(char *Buf, int BufSize)
{
    if (!Buf || (BufSize == 0)) {
        NET_OP_ERR("[rec_cam] <get_app_status> invalid parameter");
        return -1;
    }

    if (app_status.CurrEncMode == APP_VIDEO_ENC_MODE) {
        switch (rec_cam.RecCapState) {
        case REC_CAP_STATE_PREVIEW:
        case REC_CAP_STATE_RESET:
            snprintf(Buf, BufSize, "%s", "idle");
            break;
        case REC_CAP_STATE_RECORD:
            snprintf(Buf, BufSize, "%s", "record");
            break;
        case REC_CAP_STATE_CAPTURE:
            snprintf(Buf, BufSize, "%s", "capture");
            break;
        case REC_CAP_STATE_VF:
        case REC_CAP_STATE_TRANSIT_TO_VF:
            snprintf(Buf, BufSize, "%s", "vf");
            break;
        default:
            snprintf(Buf, BufSize, "%s","unknown");
            break;
        }
    } else {
        snprintf(Buf, BufSize, "photo_mode");
    }

    return 0;
}

static int rec_cam_get_capture_mode(char *Buf, int BufSize)
{
    if (!Buf || (BufSize == 0)) {
        NET_OP_ERR("[rec_cam] <get_capture_mode> invalid parameter");
        return -1;
    }

    switch (UserSetting->PhotoPref.PhotoCapMode) {
    case PHOTO_CAP_MODE_PRECISE:
        snprintf(Buf, BufSize, "%s", STR_PHOTO_CAP_MODE_PRECISE);
        break;
    case PHOTO_CAP_MODE_PES:
        snprintf(Buf, BufSize, "%s",STR_PHOTO_CAP_MODE_PES);
        break;
    case PHOTO_CAP_MODE_BURST:
        snprintf(Buf, BufSize, "%s",STR_PHOTO_CAP_MODE_BURST);
        break;
    default:
        snprintf(Buf, BufSize,"unknown");
        break;
    }

    return 0;
}

static int rec_cam_get_capture_mode_options(int TableIdx)
{
    APPLIB_JSON_OBJECT *JsonObject = NULL;
    APPLIB_JSON_OBJECT *JsonArrayObject = NULL;
    int ReturnValue = 0;

    ReturnValue = AppLibNetJsonUtility_CreateObject(&JsonObject);
    if (ReturnValue != 0) {
        AppLibNetControl_ReplyErrorCode(AMBA_GET_SINGLE_SETTING_OPTIONS, ERROR_NETCTRL_JSON_PACKAGE_ERROR);
        return -1;
    }

    ReturnValue = AppLibNetJsonUtility_CreateArrayObject(&JsonArrayObject);
    if (ReturnValue != 0) {
        AppLibNetControl_ReplyErrorCode(AMBA_GET_SINGLE_SETTING_OPTIONS, ERROR_NETCTRL_JSON_PACKAGE_ERROR);
        AppLibNetJsonUtility_FreeJsonObject(JsonObject);
        return -1;
    }

    AppLibNetJsonUtility_AddIntegerObject(JsonObject,"rval", 0);
    AppLibNetJsonUtility_AddIntegerObject(JsonObject,"msg_id", AMBA_GET_SINGLE_SETTING_OPTIONS);

    if (NetCtrlPrefTable[TableIdx].Permission == APP_PREF_READONLY) {
        AppLibNetJsonUtility_AddStringObject(JsonObject, "permission", "readonly");
    } else {
        AppLibNetJsonUtility_AddStringObject(JsonObject, "permission", "settable");
    }

    AppLibNetJsonUtility_AddStringObject(JsonObject, "param", NETCTRL_STR_CAP_MODE);
    AppLibNetJsonUtility_AddObjectToArray(JsonArrayObject, STR_PHOTO_CAP_MODE_PRECISE);
    AppLibNetJsonUtility_AddObjectToArray(JsonArrayObject, STR_PHOTO_CAP_MODE_PES);
    AppLibNetJsonUtility_AddObjectToArray(JsonArrayObject, STR_PHOTO_CAP_MODE_BURST);
    AppLibNetJsonUtility_AddObject(JsonObject, "options", JsonArrayObject);
    SendJsonString(JsonObject);
    AppLibNetJsonUtility_FreeJsonArrayObject(JsonArrayObject);
    AppLibNetJsonUtility_FreeJsonObject(JsonObject);

    return 0;
}

static int rec_cam_set_capture_mode(char *Type, char *Param)
{
    int ReturnValue = 0;
    int ErrorCode = 0;
    APPLIB_JSON_OBJECT *JsonObject = NULL;
    int PjpegConfigNum = 0;
    APP_APP_s *curapp;

    if (strcasecmp(Param ,STR_PHOTO_CAP_MODE_PRECISE) == 0) {
        UserSetting->PhotoPref.PhotoCapMode = PHOTO_CAP_MODE_PRECISE;
    } else if (strcasecmp(Param ,STR_PHOTO_CAP_MODE_PES) == 0) {
        UserSetting->PhotoPref.PhotoCapMode = PHOTO_CAP_MODE_PES;
    } else if (strcasecmp(Param ,STR_PHOTO_CAP_MODE_BURST) == 0) {
        UserSetting->PhotoPref.PhotoCapMode = PHOTO_CAP_MODE_BURST;
    } else {
        ErrorCode = ERROR_NETCTRL_INVALID_PARAM;
    }

    if (ErrorCode == 0) {
        AppLibStillEnc_SetNormCapMode(UserSetting->PhotoPref.PhotoCapMode);
        PjpegConfigNum = AppLibSysSensor_GetPjpegConfigNum(AppLibStillEnc_GetPhotoPjpegCapMode());
        if (AppLibStillEnc_GetPhotoPjpegConfigId() >= PjpegConfigNum) {
            UserSetting->PhotoPref.PhotoSize = 0;
            AppLibStillEnc_SetSizeID(UserSetting->PhotoPref.PhotoSize);
        }
        /** multi capture has higher priority to capture mode,
                 when there has multi capture, lock the capture mode menu item */
        if (AppLibStillEnc_GetMultiCapMode() == PHOTO_MULTI_CAP_OFF) {
            AppLibStillEnc_SetNormCapMode(AppLibStillEnc_GetNormCapMode());
            AppMenu_UnlockItem(MENU_PHOTO, MENU_PHOTO_CAP_MODE);
        } else {
            AppLibStillEnc_SetMultiCapMode(AppLibStillEnc_GetMultiCapMode());
            AppMenu_LockItem(MENU_PHOTO, MENU_PHOTO_CAP_MODE);
        }

        /* Send the message to the current app. */

        AppAppMgt_GetCurApp(&curapp);
        curapp->OnMessage(AMSG_CMD_SET_PHOTO_CAPTURE_MODE, 0, 0);
    }


    AppLibNetJsonUtility_CreateObject(&JsonObject);
    AppLibNetJsonUtility_AddIntegerObject(JsonObject, "rval", ErrorCode);
    AppLibNetJsonUtility_AddIntegerObject(JsonObject, "msg_id", AMBA_SET_SETTING);
    AppLibNetJsonUtility_AddStringObject(JsonObject, "type", Type);
    SendJsonString(JsonObject);
    AppLibNetJsonUtility_FreeJsonObject(JsonObject);

    if (ErrorCode != 0) {
        ReturnValue = -1;
    }

    return ReturnValue;
}

static int rec_cam_get_time(char *Buf, int BufSize)
{
    AMBA_RTC_TIME_SPEC_u TimeSpec = {0};
    int ReturnValue = 0;

    if (!Buf || (BufSize == 0)) {
        NET_OP_ERR("[rec_cam] <get_time> invalid parameter");
        return -1;
    }

    ReturnValue = AmbaRTC_GetSystemTime(AMBA_TIME_STD_TAI, &TimeSpec);
    if (ReturnValue != 0) {
        return -1;
    }

    snprintf(Buf, BufSize, "%04d-%02d-%02d %02d:%02d:%02d", TimeSpec.Calendar.Year, TimeSpec.Calendar.Month,
                                                            TimeSpec.Calendar.Day, TimeSpec.Calendar.Hour,
                                                            TimeSpec.Calendar.Minute, TimeSpec.Calendar.Second);
    NET_OP_DBG("[rec_cam] <get_time> buf: %s",Buf);

    return 0;
}

static int rec_cam_get_time_options(int TableIdx)
{
    APPLIB_JSON_OBJECT *JsonObject = NULL;
    APPLIB_JSON_OBJECT *JsonArrayObject = NULL;
    int ReturnValue = 0;

    ReturnValue = AppLibNetJsonUtility_CreateObject(&JsonObject);
    if (ReturnValue != 0) {
        AppLibNetControl_ReplyErrorCode(AMBA_GET_SINGLE_SETTING_OPTIONS, ERROR_NETCTRL_JSON_PACKAGE_ERROR);
        return -1;
    }

    ReturnValue = AppLibNetJsonUtility_CreateArrayObject(&JsonArrayObject);
    if (ReturnValue != 0) {
        AppLibNetControl_ReplyErrorCode(AMBA_GET_SINGLE_SETTING_OPTIONS, ERROR_NETCTRL_JSON_PACKAGE_ERROR);
        AppLibNetJsonUtility_FreeJsonObject(JsonObject);
        return -1;
    }

    AppLibNetJsonUtility_AddIntegerObject(JsonObject,"rval", 0);
    AppLibNetJsonUtility_AddIntegerObject(JsonObject,"msg_id", AMBA_GET_SINGLE_SETTING_OPTIONS);

    if (NetCtrlPrefTable[TableIdx].Permission == APP_PREF_READONLY) {
        AppLibNetJsonUtility_AddStringObject(JsonObject, "permission", "readonly");
    } else {
        AppLibNetJsonUtility_AddStringObject(JsonObject, "permission", "settable");
    }

    AppLibNetJsonUtility_AddStringObject(JsonObject, "param", NETCTRL_STR_CAMERA_CLOCK);
    AppLibNetJsonUtility_AddObjectToArray(JsonArrayObject, "YYYY-MM-DD HH:MM:SS");
    AppLibNetJsonUtility_AddObject(JsonObject, "options", JsonArrayObject);
    SendJsonString(JsonObject);

    AppLibNetJsonUtility_FreeJsonArrayObject(JsonArrayObject);
    AppLibNetJsonUtility_FreeJsonObject(JsonObject);


    return 0;
}


static int rec_cam_set_time(char *Type, char *Param)
{
    AMBA_RTC_TIME_SPEC_u TimeSpec = {0};
    int ErrorCode = 0;
    APPLIB_JSON_OBJECT *JsonObject = NULL;
    int ReturnValue = 0;

    if ((!Type) || (!Param)) {
        NET_OP_ERR("[rec_cam] <set_time> invalid parameter");
        AppLibNetControl_ReplyErrorCode(AMBA_SET_SETTING, ERROR_NETCTRL_INVALID_PARAM);
        return -1;
    }

    NET_OP_DBG("[rec_cam] <set_time> Param = %s");
    ReturnValue = ParseCamaraClockString(Param, &TimeSpec);
    if (ReturnValue == 0) {
        ReturnValue = AmbaRTC_SetSystemTime(AMBA_TIME_STD_TAI, &TimeSpec);
        if(ReturnValue != 0) {
            ErrorCode = ERROR_NETCTRL_UNKNOWN_ERROR;
        }
    } else if (ReturnValue == -1) {
        NET_OP_ERR("[rec_cam] <set_time> json syntax error");
        ErrorCode = ERROR_NETCTRL_JSON_SYNTAX_ERROR;
    } else {
        NET_OP_ERR("[rec_cam] <set_time> invalid setting");
        ErrorCode = ERROR_NETCTRL_INVALID_OPTION_VALUE;
    }

    AppLibNetJsonUtility_CreateObject(&JsonObject);
    AppLibNetJsonUtility_AddIntegerObject(JsonObject, "rval", ErrorCode);
    AppLibNetJsonUtility_AddIntegerObject(JsonObject, "msg_id", AMBA_SET_SETTING);
    AppLibNetJsonUtility_AddStringObject(JsonObject, "type", NETCTRL_STR_CAMERA_CLOCK);
    SendJsonString(JsonObject);

    AppLibNetJsonUtility_FreeJsonObject(JsonObject);

    return 0;
}

static int rec_cam_get_video_quality(char *Buf, int BufSize)
{
    if (!Buf || (BufSize == 0)) {
        NET_OP_ERR("[rec_cam] <get_capture_mode> invalid parameter");
        return -1;
    }

    switch (UserSetting->VideoPref.VideoQuality) {
    case VIDEO_QUALITY_SFINE:
        snprintf(Buf, BufSize, "%s", STR_VIDEO_QUALITY_SFINE);
        break;
    case VIDEO_QUALITY_FINE:
        snprintf(Buf, BufSize, "%s",STR_VIDEO_QUALITY_FINE);
        break;
    case VIDEO_QUALITY_NORMAL:
        snprintf(Buf, BufSize, "%s", STR_VIDEO_QUALITY_NORMAL);
        break;
    default:
        snprintf(Buf, BufSize, "%s","unknown");
        break;
    }

    return 0;
}

static int rec_cam_get_video_quality_options(int TableIdx)
{
    APPLIB_JSON_OBJECT *JsonObject = NULL;
    APPLIB_JSON_OBJECT *JsonArrayObject = NULL;
    int ReturnValue = 0;

    ReturnValue = AppLibNetJsonUtility_CreateObject(&JsonObject);
    if (ReturnValue != 0) {
        AppLibNetControl_ReplyErrorCode(AMBA_GET_SINGLE_SETTING_OPTIONS, ERROR_NETCTRL_JSON_PACKAGE_ERROR);
        return -1;
    }

    ReturnValue = AppLibNetJsonUtility_CreateArrayObject(&JsonArrayObject);
    if (ReturnValue != 0) {
        AppLibNetControl_ReplyErrorCode(AMBA_GET_SINGLE_SETTING_OPTIONS, ERROR_NETCTRL_JSON_PACKAGE_ERROR);
        AppLibNetJsonUtility_FreeJsonObject(JsonObject);
        return -1;
    }

    AppLibNetJsonUtility_AddIntegerObject(JsonObject, "rval", 0);
    AppLibNetJsonUtility_AddIntegerObject(JsonObject, "msg_id", AMBA_GET_SINGLE_SETTING_OPTIONS);

    if (NetCtrlPrefTable[TableIdx].Permission == APP_PREF_READONLY) {
        AppLibNetJsonUtility_AddStringObject(JsonObject, "permission", "readonly");
    } else {
        AppLibNetJsonUtility_AddStringObject(JsonObject, "permission", "settable");
    }

    AppLibNetJsonUtility_AddStringObject(JsonObject, "param", NETCTRL_STR_VIDEO_QUALITY);
    AppLibNetJsonUtility_AddObjectToArray(JsonArrayObject, STR_VIDEO_QUALITY_SFINE);
    AppLibNetJsonUtility_AddObjectToArray(JsonArrayObject, STR_VIDEO_QUALITY_FINE);
    AppLibNetJsonUtility_AddObjectToArray(JsonArrayObject, STR_VIDEO_QUALITY_NORMAL);
    AppLibNetJsonUtility_AddObject(JsonObject, "options", JsonArrayObject);
    SendJsonString(JsonObject);

    AppLibNetJsonUtility_FreeJsonArrayObject(JsonArrayObject);
    AppLibNetJsonUtility_FreeJsonObject(JsonObject);

    return 0;
}

static int rec_cam_set_video_quality(char *Type, char *Param)
{
    APPLIB_JSON_OBJECT *JsonObject = NULL;
    int ErrorCode = 0;

    if ((!Type) || (!Param)) {
        NET_OP_ERR("[rec_cam] <set_video_quality> invalid parameter");
        AppLibNetControl_ReplyErrorCode(AMBA_SET_SETTING, ERROR_NETCTRL_INVALID_PARAM);
        return -1;
    }

    if (app_status.CurrEncMode == APP_VIDEO_ENC_MODE) {
        if (rec_cam.RecCapState != REC_CAP_STATE_PREVIEW) {
            NET_OP_ERR("[rec_cam] <set_video_quality> Please stop record/vf first");
            AppLibNetControl_ReplyErrorCode(AMBA_SET_SETTING, ERROR_NETCTRL_INVALID_OPERATION);
            return -1;
        } else if (APP_CHECKFLAGS(app_rec_cam.GFlags, APP_AFLAGS_BUSY)) {
            NET_OP_ERR("[rec_cam] <set_video_quality> system busy");
            AppLibNetControl_ReplyErrorCode(AMBA_SET_SETTING, ERROR_NETCTRL_SYSTEM_BUSY);
            return -1;
        }
    }

    NET_OP_DBG("[rec_cam] <set_video_quality> Param = %s",Param);

    if (strcasecmp(Param, STR_VIDEO_QUALITY_SFINE) == 0) {
        UserSetting->VideoPref.VideoQuality = VIDEO_QUALITY_SFINE;
    } else if (strcasecmp(Param, STR_VIDEO_QUALITY_FINE) == 0) {
        UserSetting->VideoPref.VideoQuality = VIDEO_QUALITY_FINE;
    } else if (strcasecmp(Param, STR_VIDEO_QUALITY_NORMAL) == 0) {
        UserSetting->VideoPref.VideoQuality = VIDEO_QUALITY_NORMAL;
    } else {
        NET_OP_ERR("[rec_cam] <set_video_quality> unknown parameter (Param = %s)",Param);
        AppLibNetControl_ReplyErrorCode(AMBA_SET_SETTING, ERROR_NETCTRL_INVALID_PARAM);
        ErrorCode = ERROR_NETCTRL_INVALID_PARAM;
    }

    if (ErrorCode == 0) {
        rec_cam.Func(REC_CAM_SET_VIDEO_QUALITY, UserSetting->VideoPref.VideoQuality, 0);
    }

    AppLibNetJsonUtility_CreateObject(&JsonObject);
    AppLibNetJsonUtility_AddIntegerObject(JsonObject, "rval", ErrorCode);
    AppLibNetJsonUtility_AddIntegerObject(JsonObject, "msg_id", AMBA_SET_SETTING);
    AppLibNetJsonUtility_AddStringObject(JsonObject, "type", NETCTRL_STR_VIDEO_QUALITY);
    SendJsonString(JsonObject);

    AppLibNetJsonUtility_FreeJsonObject(JsonObject);

    return 0;
}

static int rec_cam_get_sensor_resolution(char *Buf, int BufSize)
{
    int i = 0;

    if (!Buf || (BufSize == 0)) {
        NET_OP_ERR("[rec_cam] <get_sensor_resolution> invalid parameter");
        return -1;
    }

    for (i=0;i<MENU_VIDEO_SENSOR_RES_SEL_NUM;i++) {
        if (UserSetting->VideoPref.SensorVideoRes == gSensorResTable[i].Val) {
            if (gSensorResTable[i].Used == 1) {
                snprintf(Buf, BufSize, "%s", gSensorResTable[i].Str);
                return 0;
            }
        }
    }

    snprintf(Buf, BufSize, "%s", "unknown");

    return -1;
}

static int rec_cam_get_sensor_resolution_options(int TableIdx)
{
    APPLIB_JSON_OBJECT *JsonObject = NULL;
    APPLIB_JSON_OBJECT *JsonArrayObject = NULL;
    int i = 0;
    int ReturnValue = 0;

    ReturnValue = AppLibNetJsonUtility_CreateObject(&JsonObject);
    if (ReturnValue != 0) {
        AppLibNetControl_ReplyErrorCode(AMBA_GET_SINGLE_SETTING_OPTIONS, ERROR_NETCTRL_JSON_PACKAGE_ERROR);
        return -1;
    }

    ReturnValue = AppLibNetJsonUtility_CreateArrayObject(&JsonArrayObject);
    if (ReturnValue != 0) {
        AppLibNetControl_ReplyErrorCode(AMBA_GET_SINGLE_SETTING_OPTIONS, ERROR_NETCTRL_JSON_PACKAGE_ERROR);
        AppLibNetJsonUtility_FreeJsonObject(JsonObject);
        return -1;
    }

    AppLibNetJsonUtility_AddIntegerObject(JsonObject, "rval", 0);
    AppLibNetJsonUtility_AddIntegerObject(JsonObject, "msg_id", AMBA_GET_SINGLE_SETTING_OPTIONS);

    if (NetCtrlPrefTable[TableIdx].Permission == APP_PREF_READONLY) {
        AppLibNetJsonUtility_AddStringObject(JsonObject, "permission", "readonly");
    } else {
        AppLibNetJsonUtility_AddStringObject(JsonObject, "permission", "settable");
    }

    AppLibNetJsonUtility_AddStringObject(JsonObject, "param", NETCTRL_STR_VIDEO_RESOLUTION);
    for (i=0;i<MENU_VIDEO_SENSOR_RES_SEL_NUM;i++) {
        if (gSensorResTable[i].Used == 1) {
            AppLibNetJsonUtility_AddObjectToArray(JsonArrayObject, gSensorResTable[i].Str);
        }
    }

    AppLibNetJsonUtility_AddObject(JsonObject, "options", JsonArrayObject);
    SendJsonString(JsonObject);

    AppLibNetJsonUtility_FreeJsonArrayObject(JsonArrayObject);
    AppLibNetJsonUtility_FreeJsonObject(JsonObject);

    return 0;
}

static int rec_cam_set_sensor_resolution(char *Type, char *Param)
{
    APPLIB_JSON_OBJECT *JsonObject = NULL;
    int ErrorCode = 0;
    int i = 0;

    if ((!Type) || (!Param)) {
        NET_OP_ERR("[rec_cam] <set_sensor_resolution> invalid parameter");
        AppLibNetControl_ReplyErrorCode(AMBA_SET_SETTING, ERROR_NETCTRL_INVALID_PARAM);
        return -1;
    }

    if (app_status.CurrEncMode == APP_VIDEO_ENC_MODE) {
        if (rec_cam.RecCapState != REC_CAP_STATE_PREVIEW) {
            NET_OP_ERR("[rec_cam] <set_video_quality> Please stop record/vf first");
            AppLibNetControl_ReplyErrorCode(AMBA_SET_SETTING, ERROR_NETCTRL_INVALID_OPERATION);
            return -1;
        } else if (APP_CHECKFLAGS(app_rec_cam.GFlags, APP_AFLAGS_BUSY)) {
            NET_OP_ERR("[rec_cam] <set_video_quality> system busy");
            AppLibNetControl_ReplyErrorCode(AMBA_SET_SETTING, ERROR_NETCTRL_SYSTEM_BUSY);
            return -1;
        }
    }

    //AmbaPrintColor(CYAN,"Param = %s",Param);
    for (i=0;i<MENU_VIDEO_SENSOR_RES_SEL_NUM;i++) {
        if (strcasecmp(Param, gSensorResTable[i].Str) == 0) {
            UserSetting->VideoPref.SensorVideoRes = gSensorResTable[i].Val;
            AmbaPrintColor(CYAN,"UserSetting->VideoPref.SensorVideoRes = %d",UserSetting->VideoPref.SensorVideoRes);

            rec_cam.Func(REC_CAM_SET_VIDEO_RES, UserSetting->VideoPref.SensorVideoRes, 0);
            break;
        }
    }

    if (i >= MENU_VIDEO_SENSOR_RES_SEL_NUM) {
        ErrorCode = ERROR_NETCTRL_INVALID_PARAM;
    }

    AppLibNetJsonUtility_CreateObject(&JsonObject);
    AppLibNetJsonUtility_AddIntegerObject(JsonObject, "rval", ErrorCode);
    AppLibNetJsonUtility_AddIntegerObject(JsonObject, "msg_id", AMBA_SET_SETTING);
    AppLibNetJsonUtility_AddStringObject(JsonObject, "type", NETCTRL_STR_VIDEO_RESOLUTION);
    SendJsonString(JsonObject);

    AppLibNetJsonUtility_FreeJsonObject(JsonObject);

    return 0;
}

static int rec_cam_get_default_setting_options(int TableIdx)
{
    APPLIB_JSON_OBJECT *JsonObject = NULL;
    APPLIB_JSON_OBJECT *JsonArrayObject = NULL;
    int ReturnValue = 0;

    ReturnValue = AppLibNetJsonUtility_CreateObject(&JsonObject);
    if (ReturnValue != 0) {
        AppLibNetControl_ReplyErrorCode(AMBA_GET_SINGLE_SETTING_OPTIONS, ERROR_NETCTRL_JSON_PACKAGE_ERROR);
        return -1;
    }

    ReturnValue = AppLibNetJsonUtility_CreateArrayObject(&JsonArrayObject);
    if (ReturnValue != 0) {
        AppLibNetControl_ReplyErrorCode(AMBA_GET_SINGLE_SETTING_OPTIONS, ERROR_NETCTRL_JSON_PACKAGE_ERROR);
        AppLibNetJsonUtility_FreeJsonObject(JsonObject);
        return -1;
    }

    AppLibNetJsonUtility_AddIntegerObject(JsonObject,"rval", 0);
    AppLibNetJsonUtility_AddIntegerObject(JsonObject,"msg_id", AMBA_GET_SINGLE_SETTING_OPTIONS);

    if (NetCtrlPrefTable[TableIdx].Permission == APP_PREF_READONLY) {
        AppLibNetJsonUtility_AddStringObject(JsonObject, "permission", "readonly");
    } else {
        AppLibNetJsonUtility_AddStringObject(JsonObject, "permission", "settable");
    }

    AppLibNetJsonUtility_AddStringObject(JsonObject, "param", NETCTRL_STR_DEFAULT_SETTING);
    AppLibNetJsonUtility_AddObjectToArray(JsonArrayObject, "on");
    AppLibNetJsonUtility_AddObjectToArray(JsonArrayObject, "off");
    AppLibNetJsonUtility_AddObject(JsonObject, "options", JsonArrayObject);
    SendJsonString(JsonObject);
    AppLibNetJsonUtility_FreeJsonArrayObject(JsonArrayObject);
    AppLibNetJsonUtility_FreeJsonObject(JsonObject);

    return 0;
}

static int rec_cam_set_default_setting(char *Type, char *Param)
{
    APPLIB_JSON_OBJECT *JsonObject = NULL;
    int ErrorCode = 0;

    if (APP_CHECKFLAGS(app_rec_cam.GFlags, APP_AFLAGS_BUSY)) {
        NET_OP_ERR("[rec_cam] <set_default_setting> system busy");
        AppLibNetControl_ReplyErrorCode(AMBA_SET_SETTING, ERROR_NETCTRL_SYSTEM_BUSY);
        return -1;
    }

    if (strcasecmp(Param ,"on") == 0) {
        /* Set the "initial version", system will refresh the preference when next boot.*/
        UserSetting->SystemPref.SystemVersion = 0;
        /* Save the system preference.  */
        AppPref_Save();
    } else {
        ErrorCode = ERROR_NETCTRL_INVALID_PARAM;
    }

    AppLibNetJsonUtility_CreateObject(&JsonObject);
    AppLibNetJsonUtility_AddIntegerObject(JsonObject, "rval", ErrorCode);
    AppLibNetJsonUtility_AddIntegerObject(JsonObject, "msg_id", AMBA_SET_SETTING);
    AppLibNetJsonUtility_AddStringObject(JsonObject, "type", NETCTRL_STR_DEFAULT_SETTING);
    SendJsonString(JsonObject);

    AppLibNetJsonUtility_FreeJsonObject(JsonObject);

    if (ErrorCode == 0) {
        AmbaPrintColor(RED, "rebooting...\r\n\r\n\r\n");
        AmbaKAL_TaskSleep(1000);
        AmbaSysSoftReset();
    }

    return 0;
}

static int rec_cam_get_timelapse(char *Buf, int BufSize)
{

    if (!Buf || (BufSize == 0)) {
        NET_OP_ERR("[rec_cam] <get_timelapse> invalid parameter");
        return -1;
    }

    switch (UserSetting->PhotoPref.TimeLapse) {
    case PHOTO_TIME_LAPSE_OFF:
        snprintf(Buf, BufSize, "%s", STR_PHOTO_TIME_LAPSE_OFF);
        break;
    case PHOTO_TIME_LAPSE_500MS:
        snprintf(Buf, BufSize, "%s",STR_PHOTO_TIME_LAPSE_500MS);
        break;
    case PHOTO_TIME_LAPSE_1S:
        snprintf(Buf, BufSize, "%s",STR_PHOTO_TIME_LAPSE_1S);
        break;
    case PHOTO_TIME_LAPSE_5S:
        snprintf(Buf, BufSize, "%s",STR_PHOTO_TIME_LAPSE_5S);
        break;
    default:
        snprintf(Buf, BufSize,"unknown");
        break;
    }

    return 0;
}

static int rec_cam_get_timelapse_options(int TableIdx)
{
    APPLIB_JSON_OBJECT *JsonObject = NULL;
    APPLIB_JSON_OBJECT *JsonArrayObject = NULL;
    int ReturnValue = 0;

    ReturnValue = AppLibNetJsonUtility_CreateObject(&JsonObject);
    if (ReturnValue != 0) {
        AppLibNetControl_ReplyErrorCode(AMBA_GET_SINGLE_SETTING_OPTIONS, ERROR_NETCTRL_JSON_PACKAGE_ERROR);
        return -1;
    }

    ReturnValue = AppLibNetJsonUtility_CreateArrayObject(&JsonArrayObject);
    if (ReturnValue != 0) {
        AppLibNetControl_ReplyErrorCode(AMBA_GET_SINGLE_SETTING_OPTIONS, ERROR_NETCTRL_JSON_PACKAGE_ERROR);
        AppLibNetJsonUtility_FreeJsonObject(JsonObject);
        return -1;
    }

    AppLibNetJsonUtility_AddIntegerObject(JsonObject,"rval", 0);
    AppLibNetJsonUtility_AddIntegerObject(JsonObject,"msg_id", AMBA_GET_SINGLE_SETTING_OPTIONS);

    if (NetCtrlPrefTable[TableIdx].Permission == APP_PREF_READONLY) {
        AppLibNetJsonUtility_AddStringObject(JsonObject, "permission", "readonly");
    } else {
        AppLibNetJsonUtility_AddStringObject(JsonObject, "permission", "settable");
    }

    AppLibNetJsonUtility_AddStringObject(JsonObject, "param", NETCTRL_STR_PHOTO_TIMELAPSE);
    AppLibNetJsonUtility_AddObjectToArray(JsonArrayObject, STR_PHOTO_TIME_LAPSE_OFF);
    AppLibNetJsonUtility_AddObjectToArray(JsonArrayObject, STR_PHOTO_TIME_LAPSE_500MS);
    AppLibNetJsonUtility_AddObjectToArray(JsonArrayObject, STR_PHOTO_TIME_LAPSE_1S);
    AppLibNetJsonUtility_AddObjectToArray(JsonArrayObject, STR_PHOTO_TIME_LAPSE_5S);
    AppLibNetJsonUtility_AddObject(JsonObject, "options", JsonArrayObject);
    SendJsonString(JsonObject);
    AppLibNetJsonUtility_FreeJsonArrayObject(JsonArrayObject);
    AppLibNetJsonUtility_FreeJsonObject(JsonObject);

    return 0;
}

static int rec_cam_set_timelapse(char *Type, char *Param)
{
    int ReturnValue = 0;
    int ErrorCode = 0;
    APPLIB_JSON_OBJECT *JsonObject = NULL;

    if (!APP_CHECKFLAGS(app_rec_cam.Flags, REC_CAM_FLAGS_STILL_CAPTURE)) {
        if (strcasecmp(Param ,STR_PHOTO_TIME_LAPSE_OFF) == 0) {
        UserSetting->PhotoPref.TimeLapse = PHOTO_TIME_LAPSE_OFF;
        } else if (strcasecmp(Param ,STR_PHOTO_TIME_LAPSE_500MS) == 0) {
            UserSetting->PhotoPref.TimeLapse = PHOTO_TIME_LAPSE_500MS;
        } else if (strcasecmp(Param ,STR_PHOTO_TIME_LAPSE_1S) == 0) {
            UserSetting->PhotoPref.TimeLapse = PHOTO_TIME_LAPSE_1S;
        } else if (strcasecmp(Param ,STR_PHOTO_TIME_LAPSE_5S) == 0) {
            UserSetting->PhotoPref.TimeLapse = PHOTO_TIME_LAPSE_5S;
        } else {
            ErrorCode = ERROR_NETCTRL_INVALID_PARAM;
        }
    } else {
        NET_OP_ERR("[rec_cam] <set_timelapse> flag REC_CAM_FLAGS_STILL_CAPTURE is on");
        ErrorCode = ERROR_NETCTRL_SYSTEM_BUSY;
    }

    AppLibNetJsonUtility_CreateObject(&JsonObject);
    AppLibNetJsonUtility_AddIntegerObject(JsonObject, "rval", ErrorCode);
    AppLibNetJsonUtility_AddIntegerObject(JsonObject, "msg_id", AMBA_SET_SETTING);
    AppLibNetJsonUtility_AddStringObject(JsonObject, "type", Type);
    SendJsonString(JsonObject);
    AppLibNetJsonUtility_FreeJsonObject(JsonObject);

    if (ErrorCode != 0) {
        ReturnValue = -1;
    }

    return ReturnValue;
}

static int rec_cam_get_photo_quality(char *Buf, int BufSize)
{
    int i = 0;

    if (!Buf || (BufSize == 0)) {
        NET_OP_ERR("[rec_cam] <get_capture_mode> invalid parameter");
        return -1;
    }

    for (i=0;i<MENU_PHOTO_QUALITY_SEL_NUM;i++) {
        if (gPhotoQualitySelTable[i].Used == 1) {
            if (gPhotoQualitySelTable[i].Val == UserSetting->PhotoPref.PhotoQuality) {
                snprintf(Buf, BufSize, "%s", gPhotoQualitySelTable[i].Str);
                return 0;
            }
        }
    }

    snprintf(Buf, BufSize, "%s","unknown");

    return -1;
}

static int rec_cam_get_photo_quality_options(int TableIdx)
{
    APPLIB_JSON_OBJECT *JsonObject = NULL;
    APPLIB_JSON_OBJECT *JsonArrayObject = NULL;
    int i = 0;
    int ReturnValue = 0;

    ReturnValue = AppLibNetJsonUtility_CreateObject(&JsonObject);
    if (ReturnValue != 0) {
        AppLibNetControl_ReplyErrorCode(AMBA_GET_SINGLE_SETTING_OPTIONS, ERROR_NETCTRL_JSON_PACKAGE_ERROR);
        return -1;
    }

    ReturnValue = AppLibNetJsonUtility_CreateArrayObject(&JsonArrayObject);
    if (ReturnValue != 0) {
        AppLibNetControl_ReplyErrorCode(AMBA_GET_SINGLE_SETTING_OPTIONS, ERROR_NETCTRL_JSON_PACKAGE_ERROR);
        AppLibNetJsonUtility_FreeJsonObject(JsonObject);
        return -1;
    }

    AppLibNetJsonUtility_AddIntegerObject(JsonObject, "rval", 0);
    AppLibNetJsonUtility_AddIntegerObject(JsonObject, "msg_id", AMBA_GET_SINGLE_SETTING_OPTIONS);

    if (NetCtrlPrefTable[TableIdx].Permission == APP_PREF_READONLY) {
        AppLibNetJsonUtility_AddStringObject(JsonObject, "permission", "readonly");
    } else {
        AppLibNetJsonUtility_AddStringObject(JsonObject, "permission", "settable");
    }

    AppLibNetJsonUtility_AddStringObject(JsonObject, "param", NETCTRL_STR_PHOTO_QUALITY);
    for (i=0;i<MENU_PHOTO_QUALITY_SEL_NUM;i++) {
        if (gPhotoQualitySelTable[i].Used == 1) {
            AppLibNetJsonUtility_AddObjectToArray(JsonArrayObject, gPhotoQualitySelTable[i].Str);
        }
    }

    AppLibNetJsonUtility_AddObject(JsonObject, "options", JsonArrayObject);
    SendJsonString(JsonObject);

    AppLibNetJsonUtility_FreeJsonArrayObject(JsonArrayObject);
    AppLibNetJsonUtility_FreeJsonObject(JsonObject);

    return 0;
}

static int rec_cam_set_photo_quality(char *Type, char *Param)
{
    int Quality = -1;
    int i = 0;
    int ErrorCode = 0;
    APPLIB_JSON_OBJECT *JsonObject = NULL;

    for (i=0;i<MENU_PHOTO_QUALITY_SEL_NUM;i++) {
        if (strcasecmp(Param, gPhotoQualitySelTable[i].Str) == 0) {
            Quality = gPhotoQualitySelTable[i].Val;
            break;
        }
    }

    if (i >= MENU_PHOTO_QUALITY_SEL_NUM) {
        ErrorCode = ERROR_NETCTRL_INVALID_PARAM;
    } else {
        if (Quality != UserSetting->PhotoPref.PhotoQuality) {
            AmbaPrint("<%s> L%d update photo size",__FUNCTION__,__LINE__);
            UserSetting->PhotoPref.PhotoQuality = Quality;
            AppLibStillEnc_SetQualityMode(Quality);
        }

    }

    AppLibNetJsonUtility_CreateObject(&JsonObject);
    AppLibNetJsonUtility_AddIntegerObject(JsonObject, "rval", ErrorCode);
    AppLibNetJsonUtility_AddIntegerObject(JsonObject, "msg_id", AMBA_SET_SETTING);
    AppLibNetJsonUtility_AddStringObject(JsonObject, "type", Type);
    SendJsonString(JsonObject);
    AppLibNetJsonUtility_FreeJsonObject(JsonObject);

    return (ErrorCode == 0) ? 0 : -1;
}

static int rec_cam_get_photo_size(char *Buf, int BufSize)
{
    int i = 0;

    if (!Buf || (BufSize == 0)) {
        NET_OP_ERR("[rec_cam] <get_capture_mode> invalid parameter");
        return -1;
    }

    for (i=0;i<MENU_PHOTO_SIZE_SEL_NUM;i++) {
        if (gPhotoSizeSelTable[i].Used == 1) {
            if (gPhotoSizeSelTable[i].Val == UserSetting->PhotoPref.PhotoSize) {
                snprintf(Buf, BufSize, "%s", gPhotoSizeSelTable[i].Str);
                return 0;
            }
        }
    }

    snprintf(Buf, BufSize, "%s","unknown");

    return -1;
}

static int rec_cam_get_photo_size_options(int TableIdx)
{
    APPLIB_JSON_OBJECT *JsonObject = NULL;
    APPLIB_JSON_OBJECT *JsonArrayObject = NULL;
    int i = 0;
    int ReturnValue = 0;

    ReturnValue = AppLibNetJsonUtility_CreateObject(&JsonObject);
    if (ReturnValue != 0) {
        AppLibNetControl_ReplyErrorCode(AMBA_GET_SINGLE_SETTING_OPTIONS, ERROR_NETCTRL_JSON_PACKAGE_ERROR);
        return -1;
    }

    ReturnValue = AppLibNetJsonUtility_CreateArrayObject(&JsonArrayObject);
    if (ReturnValue != 0) {
        AppLibNetControl_ReplyErrorCode(AMBA_GET_SINGLE_SETTING_OPTIONS, ERROR_NETCTRL_JSON_PACKAGE_ERROR);
        AppLibNetJsonUtility_FreeJsonObject(JsonObject);
        return -1;
    }

    AppLibNetJsonUtility_AddIntegerObject(JsonObject, "rval", 0);
    AppLibNetJsonUtility_AddIntegerObject(JsonObject, "msg_id", AMBA_GET_SINGLE_SETTING_OPTIONS);

    if (NetCtrlPrefTable[TableIdx].Permission == APP_PREF_READONLY) {
        AppLibNetJsonUtility_AddStringObject(JsonObject, "permission", "readonly");
    } else {
        AppLibNetJsonUtility_AddStringObject(JsonObject, "permission", "settable");
    }

    AppLibNetJsonUtility_AddStringObject(JsonObject, "param", NETCTRL_STR_PHOTO_SIZE);
    for (i=0;i<MENU_PHOTO_SIZE_SEL_NUM;i++) {
        if (gPhotoSizeSelTable[i].Used == 1) {
            AppLibNetJsonUtility_AddObjectToArray(JsonArrayObject, gPhotoSizeSelTable[i].Str);
        }
    }

    AppLibNetJsonUtility_AddObject(JsonObject, "options", JsonArrayObject);
    SendJsonString(JsonObject);

    AppLibNetJsonUtility_FreeJsonArrayObject(JsonArrayObject);
    AppLibNetJsonUtility_FreeJsonObject(JsonObject);

    return 0;
}

static int rec_cam_set_photo_size(char *Type, char *Param)
{
    int SizeId = -1;
    int i = 0;
    int ErrorCode = 0;
    APPLIB_JSON_OBJECT *JsonObject = NULL;

    for (i=0;i<MENU_PHOTO_SIZE_SEL_NUM;i++) {
        if (strcasecmp(Param, gPhotoSizeSelTable[i].Str) == 0) {
            SizeId = gPhotoSizeSelTable[i].Val;
            break;
        }
    }

    if (i >= MENU_PHOTO_SIZE_SEL_NUM) {
        ErrorCode = ERROR_NETCTRL_INVALID_PARAM;
    } else {
        if (SizeId != UserSetting->PhotoPref.PhotoSize) {
            AmbaPrint("<%s> L%d update photo size",__FUNCTION__,__LINE__);
            UserSetting->PhotoPref.PhotoSize = SizeId;
            AppLibStillEnc_SetSizeID(SizeId);
            if (app_status.CurrEncMode == APP_STILL_ENC_MODE) {
                AppLibStillEnc_LiveViewSetup();
            }
        }
    }

    AppLibNetJsonUtility_CreateObject(&JsonObject);
    AppLibNetJsonUtility_AddIntegerObject(JsonObject, "rval", ErrorCode);
    AppLibNetJsonUtility_AddIntegerObject(JsonObject, "msg_id", AMBA_SET_SETTING);
    AppLibNetJsonUtility_AddStringObject(JsonObject, "type", Type);
    SendJsonString(JsonObject);
    AppLibNetJsonUtility_FreeJsonObject(JsonObject);

    return (ErrorCode == 0) ? 0 : -1;
}

