/**
 * @file src/app/connected/applib/inc/comsvc/ApplibComSvc_Hcmgr.h
 *
 * Header of Host Control Manager implementation
 *
 * History:
 *    2013/09/12 - [Martin Lai] created file
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

#ifndef APPLIB_HCMGR_H_
#define APPLIB_HCMGR_H_
/**
 * @defgroup CommonService
 * @brief Common service functions
 *
 *
 */

/**
* @defgroup ApplibComSvc_Hcmgr
* @brief Host Control Manager implementation
*
*
*/

/**
 * @addtogroup ApplibComSvc_Hcmgr
 * @ingroup CommonService
 * @{
 */
#include <applib.h>

__BEGIN_C_PROTO__


/*************************************************************************
 * HCMGR message handler Definitions
 ************************************************************************/
/**
 * App Message Structure Definitions
 */
typedef struct _APP_MESSAGE_s_ {
    UINT32 MessageID;      /**< Message Id.*/
    UINT32 MessageData[2]; /**< Message data.*/
} APP_MESSAGE_s;

/**
 * App Handler Structure Definitions
 */
typedef struct _APPLIB_HCMGR_HANDLER_s {
    void (*HandlerMain)(void);  /**< Main handler. */
    int (*HandlerExit)(void);   /**< Exit handler. */
} APPLIB_HCMGR_HANDLER_s;

/**
 *  Attach handler function
 *
 *  @param[in] handler handler function pointer
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibComSvcHcmgr_AttachHandler(APPLIB_HCMGR_HANDLER_s *handler);

/**
 *  Detach handler function
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibComSvcHcmgr_DetachHandler(void);

/**
 *  Reset handler function pointer
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibComSvcHcmgr_ResetHandler(void);

/**
 *  Send message function
 *
 *  @param [in] msg Message object
 *  @param [in] waitOption Wait option
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibComSvcHcmgr_SndMsg(APP_MESSAGE_s *msg, UINT32 waitOption);

/**
 *  Receive message function
 *
 *  @param [in] msg Message object
 *  @param [in] waitOption Wait option
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibComSvcHcmgr_RcvMsg(APP_MESSAGE_s *msg, UINT32 waitOption);

/**
 * Host Control Manager pre-initial function
 * @return success or not
 */
extern int AppLibComSvcHcmgr_PreInit(void);

/**
 *  Host Control Manager initial function
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibComSvcHcmgr_Init(void);

/**
 *  Send the message to message queue with "WAIT_FOREVER" flag
 *
 *  @param [in] msg Message ID
 *  @param [in] param1 First parameter
 *  @param [in] param2 Second parameter
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibComSvcHcmgr_SendMsg(UINT32 msg, UINT32 param1, UINT32 param2);

/**
 *  Send the message to message queue with "NO_WAIT" flag
 *
 *  @param [in] msg Message ID
 *  @param [in] param1 First parameter
 *  @param [in] param2 Second parameter
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibComSvcHcmgr_SendMsgNoWait(UINT32 msg, UINT32 param1, UINT32 param2);

__END_C_PROTO__
/**
 * @}
 */
#endif /* APP_DEMOLIB_HCMGR_H_ */
