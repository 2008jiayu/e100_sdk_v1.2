#ifndef __H_APPLIB_NET_CTRL__
#define __H_APPLIB_NET_CTRL__

/**
 * @defgroup ApplibNet_Control
 * @brief Interfaces for net control service
 *
 *
 */

/**
 * @addtogroup ApplibNet_Control
 * @ingroup NetService
 * @{
 */

#include <applib.h>
#include <net/Json_Utility.h>

#define REMOTE_API_VER "4.1.0"

/**
 * Applib Net Control Message Structure Definitions
 */
// This structure need to be re-considerate!!!
typedef struct _APP_NETCTRL_MESSAGE_s_ {
    UINT32 Token;           /**< Token Id.*/
    UINT32 MessageID;       /**< Message Id.*/
    UINT32 MessageData[2];  /**< Message data.*/
} APP_NETCTRL_MESSAGE_s;


#define AMBA_BOSS_MSG_ID   (0x00)   /**< Message group ID for remote control */
#define AMBA_BOSS_MSG(x)  (((unsigned int)AMBA_BOSS_MSG_ID << 24) + (x))    /**< Compose message ID for remote control */

#define AMBA_GET_SETTING                AMBA_BOSS_MSG(0x0001)   /**< message ID for getting setting */
#define AMBA_SET_SETTING                AMBA_BOSS_MSG(0x0002)   /**< message ID for applying setting */
#define AMBA_GET_ALL_CURRENT_SETTINGS   AMBA_BOSS_MSG(0x0003)   /**< message ID for getting all current setting  */
#define AMBA_FORMAT                     AMBA_BOSS_MSG(0x0004)   /**< message ID for formating */
#define AMBA_GET_SPACE                  AMBA_BOSS_MSG(0x0005)   /**< message ID for getting space */
#define AMBA_GET_NUMB_FILES             AMBA_BOSS_MSG(0x0006)   /**< message ID for getting number of files */
#define AMBA_NOTIFICATION               AMBA_BOSS_MSG(0x0007)   /**< message ID for notification */
#define AMBA_BURNIN_FW                  AMBA_BOSS_MSG(0x0008)   /**< message ID for firmware upgrade */
#define AMBA_GET_SINGLE_SETTING_OPTIONS AMBA_BOSS_MSG(0x0009)   /**< message ID for getting single setting option */
#define AMBA_PUT_GPS_INFO               AMBA_BOSS_MSG(0x000a)   /**< message ID for putting GPS info */
#define AMBA_GET_DEVICEINFO             AMBA_BOSS_MSG(0x000b)   /**< message ID for getting device info */
#define AMBA_DIGITAL_ZOOM               AMBA_BOSS_MSG(0x000e)   /**< message ID for digital zoom */
#define AMBA_DIGITAL_ZOOM_INFO          AMBA_BOSS_MSG(0x000f)   /**< message ID for digital zoom info */
#define AMBA_CHANGE_BITRATE             AMBA_BOSS_MSG(0x0010)   /**< message ID for changing bitrate */
#define AMBA_START_SESSION              AMBA_BOSS_MSG(0x0101)   /**< message ID for starting session */
#define AMBA_STOP_SESSION               AMBA_BOSS_MSG(0x0102)   /**< message ID for stopping session */
#define AMBA_RESET_VF                   AMBA_BOSS_MSG(0x0103)   /**< message ID for resetting VF */
#define AMBA_STOP_VF                    AMBA_BOSS_MSG(0x0104)   /**< message ID for stopping VF */
#define AMBA_SET_CLNT_INFO              AMBA_BOSS_MSG(0x0105)   /**< message ID for setting client info */
#define AMBA_SET_PASSWD                 AMBA_BOSS_MSG(0x0107)   /**< message ID for setting password */

#define AMBA_RECORD_START               AMBA_BOSS_MSG(0x0201)   /**< message ID for record start */
#define AMBA_RECORD_STOP                AMBA_BOSS_MSG(0x0202)   /**< message ID for record stop */
#define AMBA_GET_RECORD_TIME            AMBA_BOSS_MSG(0x0203)   /**< message ID for getting record time */
#define AMBA_FORCE_SPLIT                AMBA_BOSS_MSG(0x0204)   /**< message ID for forcing split */
#define AMBA_TAKE_PHOTO                 AMBA_BOSS_MSG(0x0301)   /**< message ID for taking photo */
#define AMBA_CONTINUE_CAPTURE_STOP      AMBA_BOSS_MSG(0x0302)   /**< message ID for stopping taking photo */
#define AMBA_FOCUS                      AMBA_BOSS_MSG(0x0303)   /**< message ID for commanding camera to do focus */

#define AMBA_GET_THUMB                  AMBA_BOSS_MSG(0x0401)   /**< message ID for getting thumbnail */
#define AMBA_GET_MEDIAINFO              AMBA_BOSS_MSG(0x0402)   /**< message ID for getting media info */
#define AMBA_SET_MEDIA_ATTRIBUTE        AMBA_BOSS_MSG(0x0403)   /**< message ID for setting media attribute */
#define AMBA_DEL_FILE                   AMBA_BOSS_MSG(0x0501)   /**< message ID for deleting file */
#define AMBA_LS                         AMBA_BOSS_MSG(0x0502)   /**< message ID for LS */
#define AMBA_CD                         AMBA_BOSS_MSG(0x0503)   /**< message ID for CD */
#define AMBA_PWD                        AMBA_BOSS_MSG(0x0504)   /**< message ID for PWD */
#define AMBA_GET_FILE                   AMBA_BOSS_MSG(0x0505)   /**< message ID for getting file */
#define AMBA_PUT_FILE                   AMBA_BOSS_MSG(0x0506)   /**< message ID for putting file */
#define AMBA_SET_FILE_ATTRIBUTE         AMBA_BOSS_MSG(0x0508)   /**< message ID for setting file attribute */
#define AMBA_CANCEL_FILE_XFER		    AMBA_BOSS_MSG(0x0507)   /**< message ID for cancelling get file */
#define AMBA_MKDIR		                AMBA_BOSS_MSG(0x0509)   /**< message ID for creating directory */

#define AMBA_WIFI_RESTART               AMBA_BOSS_MSG(0x0601)   /**< message ID for WIFI restart */
#define AMBA_SET_WIFI_SETTING           AMBA_BOSS_MSG(0x0602)   /**< message ID for applying WIFI setting */
#define AMBA_GET_WIFI_SETTING           AMBA_BOSS_MSG(0x0603)   /**< message ID for getting WIFI setting */
#define AMBA_WIFI_STOP                  AMBA_BOSS_MSG(0x0604)   /**< message ID for WIFI stop */
#define AMBA_WIFI_START                 AMBA_BOSS_MSG(0x0605)   /**< message ID for WIFI start */
#define AMBA_GET_WIFI_STATUS            AMBA_BOSS_MSG(0x0606)   /**< message ID for getting WIFI status */
#define AMBA_QUERY_SESSION_HOLDER       AMBA_BOSS_MSG(0x0701)   /**< message ID for querying session holder */

#define AMBA_CUSTOM_CMD_BASE            (0x10000000)   /**<base message ID for custom command */


#define ERROR_NETCTRL_UNKNOWN_ERROR         (-1)    /**< unknown error */
#define ERROR_NETCTRL_SESSION_START_FAIL    (-3)    /**< to start session fail */
#define ERROR_NETCTRL_INVALID_TOKEN         (-4)    /**< invalid token detected */
#define ERROR_NETCTRL_REACH_MAX_CLNT        (-5)    /**< reach maximum client number*/
#define ERROR_NETCTRL_JSON_PACKAGE_ERROR    (-7)    /**< json package error */
#define ERROR_NETCTRL_JSON_PACKAGE_TIMEOUT  (-8)    /**< json package timeout */
#define ERROR_NETCTRL_JSON_SYNTAX_ERROR     (-9)    /**< json syntax error */
#define ERROR_NETCTRL_INVALID_OPTION_VALUE  (-13)   /**< invalid option value */
#define ERROR_NETCTRL_INVALID_OPERATION     (-14)   /**< invalid operation */
#define ERROR_NETCTRL_HDMI_INSERTED         (-16)   /**< HDMI inserted */
#define ERROR_NETCTRL_NO_MORE_SPACE         (-17)   /**< no more space */
#define ERROR_NETCTRL_CARD_PROTECTED        (-18)   /**< card protected */
#define ERROR_NETCTRL_NO_MORE_MEMORY        (-19)   /**< no more memory */
#define ERROR_NETCTRL_PIV_NOT_ALLOWED       (-20)   /**< PIV doesn't allowed */
#define ERROR_NETCTRL_SYSTEM_BUSY           (-21)   /**< system busy */
#define ERROR_NETCTRL_APP_NOT_READY         (-22)   /**< app not ready */
#define ERROR_NETCTRL_OPERATION_UNSUPPORTED (-23)   /**< operation unsupported */
#define ERROR_NETCTRL_INVALID_TYPE          (-24)   /**< invalid type */
#define ERROR_NETCTRL_INVALID_PARAM         (-25)   /**< invalid param */
#define ERROR_NETCTRL_INVALID_PATH          (-26)   /**< invalid path */
#define ERROR_NETCTRL_DIR_EXIST             (-27)   /**< directory existed */
#define ERROR_NETCTRL_PERMISSION_DENIED     (-28)   /**< permission denied */
#define ERROR_NETCTRL_AUTHENTICATION_FAILED (-29)   /**< authentication failed */




#define NETCTRL_STR_CAMERA_CLOCK        "camera_clock"
#define NETCTRL_STR_APP_STATUS          "app_status"
#define NETCTRL_STR_DEFAULT_SETTING     "default_setting"
#define NETCTRL_STR_STREAM_TYPE         "stream_type"
#define NETCTRL_STR_VIDEO_RESOLUTION    "video_resolution"
#define NETCTRL_STR_VIDEO_STAMP         "video_stamp"
#define NETCTRL_STR_VIDEO_QUALITY       "video_quality"
#define NETCTRL_STR_CAP_MODE            "capture_mode"
#define NETCTRL_STR_PHOTO_SIZE          "photo_size"
#define NETCTRL_STR_PHOTO_STAMP         "photo_stamp"
#define NETCTRL_STR_PHOTO_QUALITY       "photo_quality"
#define NETCTRL_STR_PHOTO_TIMELAPSE     "timelapse_photo"

/**
 *  @brief Initialize net control module
 *
 *  @return 0 success, <0 failure
 */
int AppLibNetControl_Init(void *pMemoryPool);


/**
 *  @brief Reply the result to linux after executing net control command that issuing from linux
 *
 *  @param [in] pStr pointer to json string
 *  @param [in] StringLength the length of json string 'pStr'
 *
 *  @return 0 success, <0 failure
 */
int AppLibNetControl_ReplyToLnx(char *pStr, UINT32 StringLength);


/**
 *  @brief Reply the result to linux after executing net control command that issuing from linux
 *
 *  @param [in] MsgId message id of net control commnad
 *  @param [in] ErrorCode error code
 *
 *  @return 0 success, <0 failure
 */
int AppLibNetControl_ReplyErrorCode(int MsgId, int ErrorCode);

/**
 *  @brief Send IDR frame or thumbmail to remote App
 *
 *  @param [in] DataBuf data buffer that contains IDR frame or thumbnail
 *  @param [in] DataSize data size in data buffer
 *  @param [in] ThumbType data type of data buffer. The possible values of ThumbType are "thumb", "idr", and "fullview".
 *
 *  @return 0 success, <0 failure
 */
int AppLibNetControl_SendThumb(UINT8 *DataBuf, UINT32 DataSize, char *ThumbType);

/**
 *  @brief Get current working directory
 *
 *  @param [in] Buf buffer address
 *  @param [in] BufSize buffer size
 *
 *  @return 0 success, <0 failure
 */
int AppLibNetControl_GetCurrentWorkDir(char *Buf, UINT32 BufSize);


/**
 * @}
 */

#endif //__H_APPLIB_NET_CTRL__

