/**
 * @file src/app/connected/applib/inc/comsvc/ApplibComSvc_Timer.h
 *
 * Header of Timers
 *
 * History:
 *    2013/07/10 - [Martin Lai] created file
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

#ifndef APPLIB_TIMER_H_
#define APPLIB_TIMER_H_
/**
* @defgroup ApplibComSvc_Timer
* @brief Timers
*
*
*/

/**
 * @addtogroup ApplibComSvc_Timer
 * @ingroup CommonService
 * @{
 */
#include <applib.h>

__BEGIN_C_PROTO__

/*************************************************************************
 * Timer definition
 ************************************************************************/
#define MAX_TIMER_HANDLER   (8) /**<MAX_TIMER_HANDLER   (8)*/

typedef void (*AppTimer_Handler)(int eid); /**< App Timer Handler*/

/**
 *  There are maximum of 8 HMI timers can be used.
 *  Please make sure that TIMER_NUM is equal or less than 8.
 */
typedef enum _APPLIB_TIMER_ID_e_ {
    TIMER_CHECK = 0,
    TIMER_1HZ,
    TIMER_2HZ,
    TIMER_4HZ,
    TIMER_10HZ,
    TIMER_20HZ,
    TIMER_5S,
    TIMER_30S,
    TIMER_NUM
} APPLIB_TIMER_ID_e;

#define HMSG_TIMER_CHECK        HMSG_COMSVC_MODULE_TIMER(TIMER_CHECK)  /**<HMSG_TIMER_CHECK*/
#define HMSG_TIMER_1HZ          HMSG_COMSVC_MODULE_TIMER(TIMER_1HZ)    /**<HMSG_TIMER_1HZ  */
#define HMSG_TIMER_2HZ          HMSG_COMSVC_MODULE_TIMER(TIMER_2HZ)    /**<HMSG_TIMER_2HZ  */
#define HMSG_TIMER_4HZ          HMSG_COMSVC_MODULE_TIMER(TIMER_4HZ)    /**<HMSG_TIMER_4HZ  */
#define HMSG_TIMER_10HZ         HMSG_COMSVC_MODULE_TIMER(TIMER_10HZ)   /**<HMSG_TIMER_10HZ */
#define HMSG_TIMER_20HZ         HMSG_COMSVC_MODULE_TIMER(TIMER_20HZ)   /**<HMSG_TIMER_20HZ */
#define HMSG_TIMER_5S           HMSG_COMSVC_MODULE_TIMER(TIMER_5S)     /**<HMSG_TIMER_5S   */
#define HMSG_TIMER_30S          HMSG_COMSVC_MODULE_TIMER(TIMER_30S)    /**<HMSG_TIMER_30S  */

#define TIMER_TICK              (1)  /**<TIMER_TICK              (1) */
#define TIMER_UNREGISTER        (2)  /**<TIMER_UNREGISTER        (2) */

/*************************************************************************
 * Timer APIs
 ************************************************************************/
/**
 *  The initialization of timer library.
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibComSvcTimer_Init(void);

/**
 *  This API unregister all timers, except the auto power off timer.
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibComSvcTimer_UnregisterAll(void);

/**
 *  Register a timer
 *
 *  @param [in] tid Timer id
 *  @param [in] handler Timer handler
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibComSvcTimer_Register(int tid, AppTimer_Handler handler);

/**
 *  Unregister a timer
 *
 *  @param [in] tid Timer id
 *  @param [in] handler Timer Handler
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibComSvcTimer_Unregister(int tid, AppTimer_Handler handler);

/**
 *  To handle the timers.
 *
 *  @param [in] tid timer id
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibComSvcTimer_Handler(int tid);

__END_C_PROTO__

#endif /* APPLIB_TIMER_H_ */
