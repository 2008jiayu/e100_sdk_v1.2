#ifndef __H_APPLIB_NET_BASE__
#define __H_APPLIB_NET_BASE__

/**
 * @defgroup ApplibNet_Base
 * @brief Interfaces for AmbaLink, the base of net service
 *
 *
 */

/**
 * @addtogroup ApplibNet_Base
 * @ingroup NetService
 * @{
 */

#define NET_EVENT_RTOS_EVENT(x)     (0x00000000|x)  /**<Event ID of the event generated at RTOS side */
#define NET_EVENT_LINUX_EVENT(x)    (0x10000000|x)  /**<Event ID of the event generated at LINUX side */
#define NET_EVENT_RTOS_ERROR(x)     (0xF00000000|x) /**<Error number of the error happened at RTOS side */
#define NET_EVENT_LINUX_ERROR(x)    (0xF10000000|x) /**<Error number of the error happened at LINUX side */

/**
 * RTOS event type
 */
typedef enum _NET_EVENT_RTOS_e_ {
    NET_EVENT_RTOS_CARD_INSERT = NET_EVENT_RTOS_EVENT(1), /**< Card inserted */
    NET_EVENT_RTOS_CARD_REMOVE, /**< Card remoted */
    NET_EVENT_RTOS_CARD_FORMAT, /**< Card formated */
    NET_EVENT_RTOS_FILE_CREATE, /**< New file created */
    NET_EVENT_RTOS_FILE_DELETE, /**< File deleted */
    NET_EVENT_RTOS_GENERAL_ERROR = NET_EVENT_RTOS_ERROR(1), /**< General Error */
} NET_EVENT_RTOS_e;

/**
 * Linux event type
 */
typedef enum _NET_EVENT_LINUX_e_ {
    NET_EVENT_LINUX_BOOTED = NET_EVENT_LINUX_EVENT(1), /**< Linux boot done. */
    NET_EVENT_LINUX_NETWORK_READY, /**< Network ready */
    NET_EVENT_LINUX_NETWORK_OFF, /**< Network turned off */
    NET_EVENT_LINUX_GENERAL_ERROR = NET_EVENT_LINUX_ERROR(1), /**< General Error */
} NET_EVENT_LINUX_e;


/**
 *  @brief Initialize Linux
 *
 *  @param [in] pMemoryPool the pointer to the memory pool
 *  @param [in] HibernationEnabled the flag to indicate that hibernation is enabled or not. 1: enabled, otherwiase: disabled
 *
 *  @return 0 success, <0 failure
 */
int AppLibNetBase_InitAmbaLink(void *pMemoryPool, int HibernationEnabled);


/**
 *  @brief Get the boot status of Linux
 *
 *  @return 0: Linux not booted, 1: Linux has already booted
 */
int AppLibNetBase_GetBootStatus(void);

/**
 * @}
 */

#endif

