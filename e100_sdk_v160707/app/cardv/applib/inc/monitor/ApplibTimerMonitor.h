/**
 * @file src/app/connected/applib/inc/monitor/ApplibTimerMonitor.h
 *
 * Header of Timer Monitor Utility interface.
 *
 * History:
 *    2014/05/26 - [Chester Chuang] created file
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

#ifndef APPLIB_TIMER_MONITOR_H_
#define APPLIB_TIMER_MONITOR_H_
/**
* @defgroup ApplibMonitor_Timer
* @brief Timer Monitor Utility interface.
*
*
*/

/**
 * @addtogroup ApplibMonitor_Timer
 * @ingroup Monitor
 * @{
 */

__BEGIN_C_PROTO__


/**
 *  Timer based monitor related prototype
 */
typedef struct _APPLIB_TIMER_BASED_MONITOR_HANDLER_s_ {

    void (*MonitorInit)(void);    /**< Invoke when enable handler              */
    void (*TimeUpCallBack)(void); /**< Invoke every time when time is up       */
    UINT32 Period;                /**< Period to trigger TimeUpCallCallBack()  */

} APPLIB_TIMER_BASED_MONITOR_HANDLER_s;

/**
 *  @brief Register a timer based handler
 *
 *  Register a timer based handler.
 *
 *  @param [in] hdlr monitor handler
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibTimerBasedMonitor_RegisterHandler(APPLIB_TIMER_BASED_MONITOR_HANDLER_s *hdlr);

/**
 *  @brief Unregister a timer based handler
 *
 *  Unregister a timer based handler.
 *
 *  @param [in] id handler id
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibTimerBasedMonitor_UnregisterHandler(UINT32 id);

/**
 *  @brief Enable/Disable a timer based handler
 *
 *  Enable/Disable a timer based handler.
 *
 *  @param [in] id handler id
 *  @param [in] enable enable/disable
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibTimerBasedMonitor_EnableHandler(UINT32 id, UINT32 enable);

/**
 *  @brief Release Timer based Monitor
 *
 *  Release Timer based Monitor.
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibTimerBasedMonitor_Release(void);

/**
 *  @brief Set handler period based on id
 *
 *  Set handler period based on id.
 *
 *  @param [in] id handler id
 *  @param [in] period handler period
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibTimerBasedMonitor_SetHandlerPeriod(UINT32 id, UINT32 period);

/**
 *  @brief Get handler period based on id
 *
 *  Get handler period based on id.
 *
 *  @param [in] id handler id
 *
 *  @return >=0 period, <0 failure
 */
extern int AppLibTimerBasedMonitor_GetHandlerPeriod(UINT32 id);


/**
 *  @brief Initialize Timer based Monitor
 *
 *  Initialize the Timer based Monitor
 *
 *  @param [in] taskPriority priority of the collection task
 *  @param [in] pStack stack for the task
 *  @param [in] stackSize size of the stack
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibTimerBasedMonitor_Init(UINT32 taskPriority, void *pStack, UINT32 stackSize);


__END_C_PROTO__
/**
 * @}
 */
#endif /* APPLIB_TIMER_MONITOR_H_ */
