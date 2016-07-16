#ifndef __H_APPLIB_NET__
#define __H_APPLIB_NET__

/**
 * @defgroup NetService
 * @brief Network services interface for interacting with remote device
 *
 *
 */

/**
 * @defgroup ApplibNet
 * @brief Common definition for network service
 *
 *
 */

/**
 * @addtogroup ApplibNet
 * @ingroup NetService
 * @{
 */

#include <applibhmi.h>

/**
* Message format:\n
*   |31 - 27|: MDL_APP_NET_ID \n
*   |26 - 24|: MSG_TYPE_HMI \n
*   |23 - 16|: net service sub group ID \n
*   |15 -  8|: reserved \n
*   | 7 -  0|: command index \n
*
**/
#define HMSG_APP_NET(x)                 MSG_ID(MDL_APP_NET_ID, MSG_TYPE_HMI, (x))
#define HMSG_APP_NET_CMD(subGroup, idx) HMSG_APP_NET(((UINT32)subGroup << 16) | (idx))    /**<To combine sub group ID and command index */

//-----------------------------------------------------------------------------------
// net service sub-group defintion
//-----------------------------------------------------------------------------------
#define AMSG_NET_CTRL_SYS_CMD       (0x00)   /**<group ID for system control command */
#define AMSG_NET_CTRL_SESSION_CMD   (0x01)   /**<group ID for session control command */
#define AMSG_NET_CTRL_VIDEO_CMD     (0x02)   /**<group ID for video control command */
#define AMSG_NET_CTRL_PHOTO_CMD     (0x03)   /**<group ID for photo control command */
#define AMSG_NET_CTRL_FS_CMD        (0x04)   /**<group ID for file system control command */
#define AMSG_NET_CTRL_WIFI_CMD      (0x05)   /**<group ID for wifi control commnad */
#define AMSG_NET_CTRL_MEDIA_CMD     (0x06)   /**<group ID for media control command */
#define AMSG_NET_CTRL_NOTIFY_CMD    (0x07)   /**<group ID for notification command */
#define AMSG_NET_CTRL_QUERY_CMD     (0x08)   /**<group ID for query command */
#define AMSG_NET_CTRL_CUSTOM_CMD    (0x09)   /**<group ID for custom command */
#define AMSG_NET_FIFO_EVNET         (0x20)   /**<group ID for net FIFO event */
#define AMSG_LINUX_EVNET            (0xFF)   /**<group ID for linux event */

//-----------------------------------------------------------------------------------
// message for NetCtrl
//-----------------------------------------------------------------------------------
#define AMSG_NETCTRL_SYS_GET_SETTING                HMSG_APP_NET_CMD(AMSG_NET_CTRL_SYS_CMD,0x00)    /**<message ID of getting setting */
#define AMSG_NETCTRL_SYS_SET_SETTING                HMSG_APP_NET_CMD(AMSG_NET_CTRL_SYS_CMD,0x01)    /**<message ID of updating setting */
#define AMSG_NETCTRL_SYS_GET_SETTING_ALL            HMSG_APP_NET_CMD(AMSG_NET_CTRL_SYS_CMD,0x02)    /**<message ID of getting ALL setting */
#define AMSG_NETCTRL_SYS_GET_SINGLE_SETTING_OPTION  HMSG_APP_NET_CMD(AMSG_NET_CTRL_SYS_CMD,0x03)    /**<message ID of getting options of single setting */
#define AMSG_NETCTRL_SYS_FORMAT                     HMSG_APP_NET_CMD(AMSG_NET_CTRL_SYS_CMD,0x04)    /**<message ID of formatting SD card */
#define AMSG_NETCTRL_SYS_GET_SPACE                  HMSG_APP_NET_CMD(AMSG_NET_CTRL_SYS_CMD,0x05)    /**<message ID of retrieving total memory space */
#define AMSG_NETCTRL_SYS_GET_NUMB_FILES             HMSG_APP_NET_CMD(AMSG_NET_CTRL_SYS_CMD,0x06)    /**<message ID of retrieving the number of files */
#define AMSG_NETCTRL_SYS_NOTIFI                     HMSG_APP_NET_CMD(AMSG_NET_CTRL_SYS_CMD,0x07)    /**<message ID of notifying event to remote device */
#define AMSG_NETCTRL_SYS_BURNIN_FW                  HMSG_APP_NET_CMD(AMSG_NET_CTRL_SYS_CMD,0x08)    /**<message ID of upgrading firmware */
#define AMSG_NETCTRL_SYS_FW_UPGRADE_DONE            HMSG_APP_NET_CMD(AMSG_NET_CTRL_SYS_CMD,0x09)    /**<message ID of upgrading firmware*/
#define AMSG_NETCTRL_SYS_PUT_GPS_INFO               HMSG_APP_NET_CMD(AMSG_NET_CTRL_SYS_CMD,0x0A)    /**<message ID of receiving GPS information */
#define AMSG_NETCTRL_SYS_GET_DEVICE_INFO            HMSG_APP_NET_CMD(AMSG_NET_CTRL_SYS_CMD,0x0B)    /**<message ID of getting device information */


//sub group: session control command
#define AMSG_NETCTRL_SESSION_START  HMSG_APP_NET_CMD(AMSG_NET_CTRL_SESSION_CMD,0x00)    /**<message ID of session start */
#define AMSG_NETCTRL_SESSION_STOP   HMSG_APP_NET_CMD(AMSG_NET_CTRL_SESSION_CMD,0x01)    /**<message ID of session stop */
#define AMSG_NETCTRL_VF_RESET       HMSG_APP_NET_CMD(AMSG_NET_CTRL_SESSION_CMD,0x02)    /**<message ID of VF reset */
#define AMSG_NETCTRL_VF_STOP        HMSG_APP_NET_CMD(AMSG_NET_CTRL_SESSION_CMD,0x03)    /**<message ID of VF stop */

//sub group: video control command
#define AMSG_NETCTRL_VIDEO_RECORD_START     HMSG_APP_NET_CMD(AMSG_NET_CTRL_VIDEO_CMD,0x00)  /**<message ID of record start */
#define AMSG_NETCTRL_VIDEO_RECORD_STOP      HMSG_APP_NET_CMD(AMSG_NET_CTRL_VIDEO_CMD,0x01)  /**<message ID of record stop */
#define AMSG_NETCTRL_VIDEO_GET_RECORD_TIME  HMSG_APP_NET_CMD(AMSG_NET_CTRL_VIDEO_CMD,0x02)  /**<message ID of getting record time */

//sub group: photo control command
#define AMSG_NETCTRL_PHOTO_TAKE_PHOTO               HMSG_APP_NET_CMD(AMSG_NET_CTRL_PHOTO_CMD,0x00)      /**<message ID of photo capture */
#define AMSG_NETCTRL_PHOTO_CONTINUE_CAPTURE_STOP    HMSG_APP_NET_CMD(AMSG_NET_CTRL_PHOTO_CMD,0x01)      /**<message ID of stopping photo capture */

//sub group: file system control command
#define AMSG_NETCTRL_FS_DEL_FILE    HMSG_APP_NET_CMD(AMSG_NET_CTRL_FS_CMD,0x00) /**<message ID of file deletion */

//sub group: wifi control command
#define AMSG_NETCTRL_WIFI_RESTART   HMSG_APP_NET_CMD(AMSG_NET_CTRL_WIFI_CMD,0x00)   /**<message ID of wifi restart */

//sub group: media control command
#define AMSG_NETCTRL_MEDIA_GET_THUMB        HMSG_APP_NET_CMD(AMSG_NET_CTRL_MEDIA_CMD,0x00)  /**<message ID of retrieving a thumbnail file */
#define AMSG_NETCTRL_MEDIA_GET_MEDIAINFO    HMSG_APP_NET_CMD(AMSG_NET_CTRL_MEDIA_CMD,0x01)  /**<message ID of getting media info */

//sub group: notify control command
#define AMSG_NETCTRL_NOTIFY HMSG_APP_NET_CMD(AMSG_NET_CTRL_NOTIFY_CMD,0x00) /**<message ID of notify */

//sub group: query command
#define AMSG_NETCTRL_QUERY_SESSION_HOLDER   HMSG_APP_NET_CMD(AMSG_NET_CTRL_QUERY_CMD,0x00)  /**<message ID of querying session holder */

//sub group:
#define AMSG_NETCTRL_CUSTOM_CMD   HMSG_APP_NET_CMD(AMSG_NET_CTRL_CUSTOM_CMD,0x00)  /**<message ID of custom command */

//-----------------------------------------------------------------------------------
// message for NetFiFo
//-----------------------------------------------------------------------------------
#define AMSG_NETFIFO_EVENT_START    HMSG_APP_NET_CMD(AMSG_NET_FIFO_EVNET, 0x01) /**<message ID of FIFO start event */
#define AMSG_NETFIFO_EVENT_STOP     HMSG_APP_NET_CMD(AMSG_NET_FIFO_EVNET, 0x02) /**<message ID of FIFO stop event */

//-----------------------------------------------------------------------------------
// message for linux event
//-----------------------------------------------------------------------------------
#define AMSG_EVENT_BOSS_BOOTED                   HMSG_APP_NET_CMD(AMSG_LINUX_EVNET, 0x01) /**<message ID of linux booted event */
#define AMSG_EVENT_BOSS_HIBERNATION_DONE         HMSG_APP_NET_CMD(AMSG_LINUX_EVNET, 0x02) /**<message ID of linux hibernation ready from cool boot  CONFIG_APP_ARD */


/**
 * @}
 */

#endif //__H_APPLIB_NET__

