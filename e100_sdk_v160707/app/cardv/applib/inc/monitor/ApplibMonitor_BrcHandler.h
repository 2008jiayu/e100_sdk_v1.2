/**
 * @file src/app/connected/applib/inc/monitor/ApplibMonitor_BrcHandler.h
 *
 * Header of Bitrate control handler interface.
 *
 * History:
 *    2014/07/23 - [Chester Chuang] created file
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
* @defgroup ApplibMonitor_BrcHandler
* @brief Bitrate control handler interface.
*
*
*/

/**
 * @addtogroup ApplibMonitor_BrcHandler
 * @ingroup Monitor
 * @{
 */
#include <applib.h>
__BEGIN_C_PROTO__

/**
 *  Amba bitrate control dzoom handler
 */
typedef struct _BITRATE_CONTROL_DZOOM_HANDLER_s_ {
    void (*HandlerCB)(UINT32 *targetBitRate, UINT32 currBitRate, UINT8 streamId); /**< Handler CB function */
    float DzoomFactorThres; /**< Threshold of Dzoom factor */
    UINT8 DebugPattern;  /**< Debug Pattern */
    UINT8 Reserved[3];  /**< Reserved */
} BITRATE_CONTROL_DZOOM_HANDLER_s;

/**
 *  Amba bitrate control luma handler
 */
typedef struct _BITRATE_CONTROL_LUMA_HANDLER_s_ {
    void (*HandlerCB)(UINT32 *targetBitRate, UINT32 currBitRate, UINT8 streamId); /**< Handler CB function */
    int LumaThres;       /**< Threshold of Luma */
    int LowLumaThres;    /**< Threshold of Low Luma */
    UINT8 DebugPattern; /**< Debug Pattern */
    UINT8 Reserved[3]; /**< Reserved */
} BITRATE_CONTROL_LUMA_HANDLER_s;

/**
 *  Amba bitrate control complexity handler
 */
typedef struct _BITRATE_CONTROL_COMPLEXITY_HANDLER_s_ {
    void (*HandlerCB)(UINT32 *targetBitRate, UINT32 currBitRate, UINT8 streamId);  /**< Handler CB function */
    int (*GetDayLumaThresCB)(int mode, UINT32 *threshold); /**< Get Day Luma Threshold CB */
    int (*GetComplexityRangeCB)(int mode, UINT32 *complexMin, UINT32 *complexMid, UINT32 *complexMax); /**< Get Complexity Range CB function */
    void (*GetPipeModeCB)(UINT8 *isVhdr, UINT8 *isOverSample); /**< Get Pipe Mode CB function */
    UINT8 DebugPattern; /**< Debug Pattern */
    UINT8 Reserved[3]; /**< Reserved */
} BITRATE_CONTROL_COMPLEXITY_HANDLER_s;


__END_C_PROTO__
/**
 * @}
 */
#endif /* APPLIB_TIMER_MONITOR_H_ */
