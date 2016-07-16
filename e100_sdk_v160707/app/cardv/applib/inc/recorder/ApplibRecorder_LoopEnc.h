/**
 * @file src/app/connected/applib/inc/recorder/ApplibRecorder_LoopEnc.h
 *
 * Header of Loop Encoder manager
 *
 * History:
 *    2014/04/11 - [Annie Ting] created file
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

#ifndef APPLIB_LOOP_ENC_MGR_H_
#define APPLIB_LOOP_ENC_MGR_H_

/**
* @defgroup ApplibRecorder_LoopEnc
* @brief Loop encode related function
*
*
*/

/**
 * @addtogroup ApplibRecorder_LoopEnc
 * @ingroup Recorder
 * @{
 */
#include <applib.h>

__BEGIN_C_PROTO__



/**info[1:0] represnt step finish*/
#define LOOP_ENC_SEARCH_DONE          0x01       /**<LOOP ENCODE STATUS MSG*/
#define LOOP_ENC_HANDLE_DONE          0x02       /**<LOOP ENCODE STATUS MSG*/
#define LOOP_ENC_SEARCH_ERROR        0x04        /**<LOOP ENCODE STATUS MSG*/
#define LOOP_ENC_HANDLE_ERROR        0x08        /**<LOOP ENCODE STATUS MSG*/
#define LOOP_ENC_CHECK_SEARCH        0x10        /**<LOOP ENCODE STATUS MSG*/
#define LOOP_ENC_CHECK_HANDLE        0x20        /**<LOOP ENCODE STATUS MSG*/

/**
 *  @brief Send message to Loop Encoder Mgr.
 *
 *  Send message to  Loop Encoder Mgr.
 *
 *  @param [in] msg Message ID
 *  @param [in] param1 first parameter
 *  @param [in] param2 second parameter
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibLoopEnc_SndMsg(UINT32 msg, UINT32 param1, UINT32 param2);



/**
 *  @brief Initialization of Loop Encoder manager.
 *
 *   Initialization of Loop Encoder manager.
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibLoopEnc_Init(void);

/**
 *  @brief feed back loop enc function status
 *
 *  return loop enc result to app
 *
 *
 *  @return 0:do nothing 1:Loop enc all done 2:search file done
 *                       -1:Search file error  -2:Delete file error
 *
 */
extern int AppLibLoopEnc_StepCheck(void);

__END_C_PROTO__
/**
 * @}
 */
#endif /* APPLIB_LOOP_ENC_MGR_H_ */
